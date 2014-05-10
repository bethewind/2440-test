/**************************************************************************************
* 
*	Project Name : S3C6400 Validation
*
*	Copyright 2006 by Samsung Electronics, Inc.
*	All rights reserved.
*
*	Project Description :
*		This software is only for validating functions of the S3C6400.
*		Anybody can use this software without our permission.
*  
*--------------------------------------------------------------------------------------
* 
*	File Name : pccard.c
*  
*	File Description : This file implements the driver functions for CF Controller and PCCARD mode on it.
*
*	Author : Sunil,Roe
*	Dept. : AP Development Team
*	Created Date : 2007/1/17
*	Version : 0.1 
* 
*	History
*	- Created(Sunil,Roe 2007/1/17)
*   
**************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "def.h"
#include "option.h"
#include "library.h"
#include "sfr6410.h"
#include "system.h"
#include "intc.h"
#include "gpio.h"
#include "sysc.h"

#include "timer.h"
#include "cf.h"
#include "pccard.h"


#define DBG_PCCARD 0

#ifdef DBG_PCCARD
#define DbgPccard(x) UART_Printf x
#else
#define DbgPccard(x) 0;
#endif


// PC card SFR base
#define PCCARD_SFR_BASE	CFCON_BASE+0x1820

// PC Card Memory map
#define PCCARD_ATT_BASE	CFCON_BASE					// refer to attribute memory region
#define PCCARD_IO_BASE		CFCON_BASE+0x800  			// refer to IO region
#define PCCARD_MEM_BASE	CFCON_BASE+0x1000 			// refer to common memory region

#define PCCARD_MEM_MODE		0
#define PCCARD_IO_MODE1		1
#define PCCARD_IO_MODE2		2
#define PCCARD_IO_MODE3		3




/*=========================================================================
 *        	             I/O configuration address
 *=========================================================================
 */

#define PCCARD_ATT_CONFIG_BASE		(PCCARD_ATT_BASE + 0x200)
#define PCCARD_ATT_CONFIG_OPTION		(PCCARD_ATT_CONFIG_BASE)
#define PCCARD_ATT_CONFIG_STATUS		(PCCARD_ATT_CONFIG_BASE + 0x2)
#define PCCARD_ATT_CONFIG_REPLACE	(PCCARD_ATT_CONFIG_BASE + 0x4)
#define PCCARD_ATT_CONFIG_SOCKET		(PCCARD_ATT_CONFIG_BASE + 0x6)

#define PCCARD(__n) 				( ( volatile oPCCARD_REGS * ) ( g_paPCCARDSFRBase[__n] ) )
#define PCCARD_ATTR(__n) 		( ( volatile oPCCARDATT_REGS * ) ( g_paPCCARDATTBase[__n] ) )
#define PCCARD_TASKFILE(__n) 	( ( volatile oPCCARDTAKSFILE_REGS * ) ( g_paPCCARDTaskFileBase[__n] ) )

typedef struct tag_PCCARD_REGS
{
	u32 rrPCCARD_CFG;		// PCCARD configuration & status register
	u32 rrPCCARD_INT;		// PCCARD interrupt mask & source register
	u32 rrPCCARD_ATTR;		// PCCARD attribute memory area operation timing config register
	u32 rrPCCARD_IO;		// PCCARD attribute memory area operation timing config register
	u32 rrPCCARD_COMM;		// PCCARD attribute memory area operation timing config register
} 
oPCCARD_REGS;

typedef struct tag_PCCARDATT_REGS
{
	u16 rPCCARD_CONFIG_OPTION;		// PCCARD configuration option register (offset : 0x0)
	u16 rPCCARD_CONFIG_STATUS;		// PCCARD configuration status register (offset : 0x2)
	u16 rPCCARD_CONFIG_REPLACE;		// PCCARD pin replacement register (offset : 0x4)
	u16 rPCCARD_CONFIG_SOCKET;		// PCCARD socket copy register (offset : 0x6)
} 
oPCCARDATT_REGS;

typedef struct tag_PCCARDTAKSFILE_REGS
{
	u8 rPCCARD_DATA;		// TaskFile Register : Data register
	u8 rPCCARD_FEATURE;	// TaskFile Register : Feature register
	u8 rPCCARD_SECTOR;		// TaskFile Register : Sector count register
	u8 rPCCARD_LOWLBA;		// TaskFile Register : Sector Number register
	u8 rPCCARD_MIDLBA;		// TaskFile Register : Cylinder low register
	u8 rPCCARD_HIGHLBA;	// TaskFile Register : Cylinder high register
	u8 rPCCARD_DEVICE;		// TaskFile Register : Drive head register
	u8 rPCCARD_STATUS;		// TaskFile Register : Status register
	u8 rPCCARD_COMMAND;	// TaskFile Register : Command register
} 
oPCCARDTAKSFILE_REGS;
 
// define global variables
static void *				g_paPCCARDSFRBase[PCCARD_CONNUM];
static void *				g_paPCCARDATTBase[PCCARD_CONNUM];
static void *				g_paPCCARDTaskFileBase[PCCARD_CONNUM];
extern volatile PCCARD_oInform	g_oaPCCARDInform[];
extern u8 				g_ucPCCardConNum;

//=========================================================================
//					          	          PCCARD Controller AREA
//=========================================================================
/*---------------------------------- Init Functions  ---------------------------------*/
//////////
// Function Name : PCCARD_Init
// Function Description : This function initializes a certain PCCARD Controller.
// Input : 	ucCon - PCCARD Controller Number 
// Output : 	TRUE 	- Memory Device is reset
//			FALSE	- Memory Device is not reset because of ERROR
// Version : v0.1
u8	PCCARD_Init(u8 ucCon)
{
	u32 uSFRBaseAddress;			// for PCCARD Controller Base Register
//	u32 uTaskFileBaseAddress;		// for Task File Base Register
	u32 uATTConfigBaseAddress;		// for Attribute memory Base register

	Assert( (ucCon == PCCARD_CON0) );

	if(ucCon == PCCARD_CON0)
	{
		uSFRBaseAddress			= PCCARD_SFR_BASE;
		uATTConfigBaseAddress	= PCCARD_ATT_CONFIG_BASE;
//		g_oaPCMInform[ucCon].ucDMANum		= NUM_DMA0;
//		g_oaPCMInform[ucCon].ucDMACon 		= DMA0;
//		g_oaPCMInform[ucCon].ucDMATxSrc		= DMA0_PCM0_TX;
//		g_oaPCMInform[ucCon].ucDMARxSrc		= DMA0_PCM0_RX;
	}
	else
	{
		return FALSE;
	}

	// EBI Setup for CF controller	
	CF_SetEBI(INDIRECT_MODE);

	PCCARD_SetSFRBaseAddr(ucCon, uSFRBaseAddress);
//	PCCARD_SetTaskFileBaseAddr(ucCon, uTaskFileBaseAddress);
	PCCARD_SetAttConfigBaseAddr(ucCon, uATTConfigBaseAddress);
	// Initialize PCM Buffer
	PCCARD_InitBuffer(ucCon);
	PCCARD_InitPort(ucCon);
	
	return TRUE;	
}

//////////
// Function Name : PCCARD_InitBuffer
// Function Description : This function initializes buffers used by PCM Controller.
// Input : 	ucCon - PCCARD Controller Number 
// Output : 	NONE
// Version : v0.1
void PCCARD_InitBuffer(u8 ucCon)
{
	g_oaPCCARDInform[ucCon].puPCCARDWriteBuf = (u32*) CF_WRITE_BUF;
	g_oaPCCARDInform[ucCon].puPCCARDReadBuf = (u32*) (CF_WRITE_BUF + 0x400000); 
}

//////////
// Function Name : PCCARD_InitPort
// Function Description : 
//   This function Initialize ports as PCCARD.
// Input : ucCon - PCCARD Controller Number 
// Output : NONE
// Version : v0.1 
void PCCARD_InitPort(u8 ucCon)
{
	// GPIO_M[5] (XhiINTR) should be output low in order to connect Xm0OEata to nOE_CF signal.
	// (refer to the unit TS5A3159_SPDT(U613) in SMDK6400 Rev0.1's schematic page 7)
	GPIO_SetFunctionEach(eGPIO_M, eGPIO_5, 1);	// set XhiINTR/GPM5 as output
	GPIO_SetDataEach(eGPIO_M, eGPIO_5 , 0); 		//GPM[5] -> Low	
	
	switch(ucCon)
	{
		case PCCARD_CON0 :
			//--- CF controller - PC card mode setting ---//
			// Output pad disable, Card power off, PC card mode
			CF_SetMUXReg(eCF_MUX_OUTPUT_DISABLE, eCF_MUX_CARDPWR_OFF, eCF_MUX_MODE_PCCARD);
			DelayfrTimer(micro, 100);	// 100us delay
			// Output pad enable, Card power off, PC card mode
			CF_SetMUXReg(eCF_MUX_OUTPUT_ENABLE, eCF_MUX_CARDPWR_OFF, eCF_MUX_MODE_PCCARD);
			DelayfrTimer(micro, 100);	// 100us delay
			// Card Power on (PC Card mode)
			CF_SetMUXReg(eCF_MUX_OUTPUT_ENABLE, eCF_MUX_CARDPWR_ON, eCF_MUX_MODE_PCCARD);

			// wait for at least 20ms (SanDisk: 25ms, MicroDrive: 125ms, Hagiwara: )
			// No other method available.
			DelayfrTimer(milli, 125);	// 125ms delay
			break;
		default :
			break;
	}
#if 1	// for CF card boot up signal
	GPIO_SetFunctionEach(eGPIO_N, eGPIO_8, 1);	// set XEINT8/ADDR_CF0/GPN8 as output
	GPIO_SetDataEach(eGPIO_N, eGPIO_8 , 1); 		//GPN[8] -> High
	DelayfrTimer(milli,100);						//Delay about 10ms
	GPIO_SetDataEach(eGPIO_N, eGPIO_8 , 0); 		//GPN[8] -> Low	
#endif			
}

//////////
// Function Name : PCCARD_ReturnPort
// Function Description : 
//   This function Initialize ports as default GPIO(input).
// Input : ucCon - PCCARD Controller Number 
// Output : NONE
// Version : v0.1 
void PCCARD_ReturnPort(u8 ucCon)
{
	switch(ucCon)
	{
		case PCCARD_CON0 :
//			GPIO_SetFunctionAll(eGPIO_D, 0x0, 0x0);
			break;
		default :
			break;
	}
	CF_SetMUXReg(eCF_MUX_OUTPUT_DISABLE, eCF_MUX_CARDPWR_OFF, eCF_MUX_MODE_PCCARD);

	// GPIO_M[5] (XhiINTR) should be output high in order to connect VDD_SMEM to nOE_CF signal, 
	// because Xm0OEata would be zero if the state of CF card is a TrueIDE mode.
	// So, in order to avoid making low state input of glue logic, nOE_CF should be connected to VDD_SMEM.	
	// (refer to the unit SN74AUC2G08(U607) in SMDK6400 Rev0.1's schematic page 7)
	GPIO_SetFunctionEach(eGPIO_M, eGPIO_5, 1);	// set XhiINTR/GPM5 as output
	GPIO_SetDataEach(eGPIO_M, eGPIO_5 , 1); 		//GPM[5] -> High	
	
}

/*---------------------------------- APIs of PCCARD_CNFG&STATUS Registers ---------------------------------*/
//////////
// Function Name : PCCARD_SetConfig
// Function Description : This function sets up PCCARD Control Register.
// Input : 	ucCon - PCCARD Controller Number
//			ePccardWidth - 8bit/16bit selection
// Output : 	NONE
// Version : v0.1
void PCCARD_SetConfig(u8 ucCon, ePCCARD_WIDTH ePccardWidth)
{
	u32		uRegValue = 0;

	uRegValue = PCCARD_GetRegValue(ucCon, ePCCARD_CNFG_STATUS);
	uRegValue = uRegValue & ~(0x1f<< 8 ) | (1<< 12) | (1<<11) | (ePccardWidth<<10) | (ePccardWidth<<9) | (ePccardWidth<<8);		

	PCCARD_SetRegValue(ucCon, ePCCARD_CNFG_STATUS, uRegValue);		

}

/*---------------------------------- APIs of PCCARD_INTMSK&SRC Registers ---------------------------------*/
//////////
// Function Name : PCCARD_ResetCard
// Function Description : This function resets CF Card in PCCARD mode.
// Input : 	ucCon 		- PCCARD Controller Number
//			ucResetFlag	- RESET/NORESET
// Output : 	NONE
// Version : v0.1
void PCCARD_ResetCard(u8 ucCon, u8 ucResetFlag)
{
	u32		uRegValue = 0;

	uRegValue = PCCARD_GetRegValue(ucCon, ePCCARD_CNFG_STATUS);
	uRegValue = uRegValue & ~(0x1<< 13 ) | (ucResetFlag<<13);		

	PCCARD_SetRegValue(ucCon, ePCCARD_CNFG_STATUS, uRegValue);		
}

//////////
// Function Name : PCCARD_SetINTMask
// Function Description : This function sets up PCCARD Interrupt Mask Register.
// Input : 	ucCon 		- PCCARD Controller Number
//			ucMaskFlag	- MASK/UNMASK
// Output : 	NONE
// Version : v0.1
void PCCARD_SetINTMask(u8 ucCon, u8 ucMaskFlag)
{
	u32		uRegValue = 0;

	uRegValue = PCCARD_GetRegValue(ucCon, ePCCARD_INTMSK_SRC);
	uRegValue = uRegValue & ~(0x7<< 8 ) | (ucMaskFlag<<10) | (ucMaskFlag<<9) | (ucMaskFlag<<8);		

	PCCARD_SetRegValue(ucCon, ePCCARD_INTMSK_SRC, uRegValue);		
}

/*---------------------------------- APIs of PCCARD_ATTR Registers ---------------------------------*/
/*---------------------------------- APIs of PCCARD_IO Registers ---------------------------------*/
/*---------------------------------- APIs of PCCARD_COMM Registers ---------------------------------*/
//////////
// Function Name : PCCARD_SetINTMask
// Function Description : This function sets up PCCARD Interrupt Mask Register.
// Input : 	ucCon 			- PCCARD Controller Number
//			ucHoldTiming		- Hold timing value
//			ucCmdTiming		- Cmd timing value
//			ucSetupTiming	- Setup timing value
// Output : 	NONE
// Version : v0.1
void PCCARD_SetCardAccessTiming(u8 ucCon, u8 ucHoldTiming, u8 ucCmdTiming, u8 ucSetupTiming)
{
	u32		uRegValue = 0;

	// Timing Hold, Command, Setup in attr mem mode
	uRegValue = PCCARD_GetRegValue(ucCon, ePCCARD_ATTR);
	uRegValue = (ucHoldTiming<<16)|(ucCmdTiming<<8)|(ucSetupTiming);	
	PCCARD_SetRegValue(ucCon, ePCCARD_ATTR, uRegValue);		
	// Timing Hold, Command, Setup in IO mode
	uRegValue = PCCARD_GetRegValue(ucCon, ePCCARD_IO);
	uRegValue = (ucHoldTiming<<16)|(ucCmdTiming<<8)|(ucSetupTiming);	
	PCCARD_SetRegValue(ucCon, ePCCARD_IO, uRegValue);		
	// Timing Hold, Command, Setup in comm mem mode
	uRegValue = PCCARD_GetRegValue(ucCon, ePCCARD_COMM);
	uRegValue = (ucHoldTiming<<16)|(ucCmdTiming<<8)|(ucSetupTiming);	
	PCCARD_SetRegValue(ucCon, ePCCARD_COMM, uRegValue);		
}

/*---------------------------------- APIs of general PCCARD ---------------------------------*/

//////////
// Function Name : PCCARD_SetSFRBaseAddr
// Function Description : This function sets up the base address of PCCARD Registers.
// Input : 	ucCon		- PCCRAD controller number
// Output : 	uBaseAddress	- the baseaddress of PCCARD registers
// Version : v0.1
void PCCARD_SetSFRBaseAddr(u8 ucCon, u32 uBaseAddress)
{
	g_paPCCARDSFRBase[ucCon] = (void *)uBaseAddress;
}

//////////
// Function Name : PCCARD_GetRegAddr
// Function Description : This function gets the address of PCCARD Registers.
// Input : 	ucPCCARDRegId		- the Id of PCCARD Registers
// Output : 	*puPCCARDReg		- the value of specified register
// Version : v0.1
u32 PCCARD_GetRegAddr(u8 ucCon, ePCCARD_Id uPCCARDRegId)
{
	volatile u32 *	puPCCARDBaseAddr;
	volatile u32 *	puPCCARDReg;	
	u32			uPCCARDRegAddr = 0;

	puPCCARDBaseAddr			= &(PCCARD(ucCon)->rrPCCARD_CFG);
	puPCCARDReg				= puPCCARDBaseAddr + uPCCARDRegId;

	uPCCARDRegAddr			= (u32)&puPCCARDReg;
	return uPCCARDRegAddr;	
}

//////////
// Function Name : PCCARD_GetRegValue
// Function Description : This function gets the value of PCCARD Registers.
// Input : 	ucPCCARDRegId		- the Id of PCCARD Registers
// Output : 	*puPCCARDReg		- the value of specified register
// Version : v0.1
u32 PCCARD_GetRegValue(u8 ucCon, ePCCARD_Id uPCCARDRegId)
{
	volatile u32 *	puPCCARDBaseAddr;
	volatile u32 *	puPCCARDReg;	

	puPCCARDBaseAddr			= &(PCCARD(ucCon)->rrPCCARD_CFG);
	puPCCARDReg				= puPCCARDBaseAddr + uPCCARDRegId;

	return *puPCCARDReg;	
}

//////////
// Function Name : PCCARD_SetRegValue
// Function Description : This function sets the value of PCCARD Registers.
// Input : 	ucPCCARDRegId		- the Id of PCCARD Registers
//			uValue			- the value of register
// Output : 	NONE
// Version : v0.1
void PCCARD_SetRegValue(u8 ucCon, ePCCARD_Id uPCCARDRegId, u32 uValue)
{
	volatile u32 *	puPCCARDBaseAddr;
	volatile u32 *	puPCCARDReg;	

	puPCCARDBaseAddr		= &(PCCARD(ucCon)->rrPCCARD_CFG);
	puPCCARDReg			= puPCCARDBaseAddr + uPCCARDRegId;

	*puPCCARDReg			= uValue;
}

//=========================================================================
//					          	          TASK FILE REGISTER AREA
//=========================================================================
/*---------------------------------- APIs of Task File Registers of memory/IO1/IO2/IO3 mode in PCCARD mode  ---------------------------------*/
//////////
// Function Name : PCCARD_ChangePCCARDMode
// Function Description : 
//   This function changes PCCARD mode by matching the address of Task File Registers.
// Input : 	ucCon - PCCARD Controller Number 
//			ePccardMode		- MEM_MODE/IO1_MODE/IO2_MODE/IO3_MODE
// Output : NONE
// Version : v0.1 
void PCCARD_ChangePCCARDMode(u8 ucCon, ePCCARD_MODE ePccardMode)
{
	switch(ePccardMode)
	{
		// contiguous I/O map
		case ePCCARD_CONTIGUOUS_IO :
			g_oaPCCARDInform[ucCon].PCCARD_DATA 		= (PCCARD_IO_BASE + 0x00);
			g_oaPCCARDInform[ucCon].PCCARD_FEATURE	= (PCCARD_IO_BASE + 0x01);
			g_oaPCCARDInform[ucCon].PCCARD_SECTOR	= (PCCARD_IO_BASE + 0x02);
			g_oaPCCARDInform[ucCon].PCCARD_LOWLBA	= (PCCARD_IO_BASE + 0x03);
			g_oaPCCARDInform[ucCon].PCCARD_MIDLBA	= (PCCARD_IO_BASE + 0x04);
			g_oaPCCARDInform[ucCon].PCCARD_HIGHLBA	= (PCCARD_IO_BASE + 0x05);
			g_oaPCCARDInform[ucCon].PCCARD_DEVICE	= (PCCARD_IO_BASE + 0x06);
			g_oaPCCARDInform[ucCon].PCCARD_STATUS	= (PCCARD_IO_BASE + 0x07);
			g_oaPCCARDInform[ucCon].PCCARD_COMMAND	= (PCCARD_IO_BASE + 0x07);			

			PCCARD_SetTaskFileBaseAddr(ucCon, (PCCARD_IO_BASE + 0x00));
			break;
		// Primary I/O map
		case ePCCARD_PRIMARY_IO:
			g_oaPCCARDInform[ucCon].PCCARD_DATA 		= (PCCARD_IO_BASE + 0x1F0);
			g_oaPCCARDInform[ucCon].PCCARD_FEATURE	= (PCCARD_IO_BASE + 0x1F1);
			g_oaPCCARDInform[ucCon].PCCARD_SECTOR	= (PCCARD_IO_BASE + 0x1F2);
			g_oaPCCARDInform[ucCon].PCCARD_LOWLBA	= (PCCARD_IO_BASE + 0x1F3);
			g_oaPCCARDInform[ucCon].PCCARD_MIDLBA	= (PCCARD_IO_BASE + 0x1F4);
			g_oaPCCARDInform[ucCon].PCCARD_HIGHLBA	= (PCCARD_IO_BASE + 0x1F5);
			g_oaPCCARDInform[ucCon].PCCARD_DEVICE	= (PCCARD_IO_BASE + 0x1F6);
			g_oaPCCARDInform[ucCon].PCCARD_STATUS	= (PCCARD_IO_BASE + 0x1F7);
			g_oaPCCARDInform[ucCon].PCCARD_COMMAND	= (PCCARD_IO_BASE + 0x1F7);			

			PCCARD_SetTaskFileBaseAddr(ucCon, (PCCARD_IO_BASE + 0x1F0));
			break;
		// Secondary I/O map
		case ePCCARD_SECONDARY_IO :
			g_oaPCCARDInform[ucCon].PCCARD_DATA 		= (PCCARD_IO_BASE + 0x170);
			g_oaPCCARDInform[ucCon].PCCARD_FEATURE	= (PCCARD_IO_BASE + 0x171);
			g_oaPCCARDInform[ucCon].PCCARD_SECTOR	= (PCCARD_IO_BASE + 0x172);
			g_oaPCCARDInform[ucCon].PCCARD_LOWLBA	= (PCCARD_IO_BASE + 0x173);
			g_oaPCCARDInform[ucCon].PCCARD_MIDLBA	= (PCCARD_IO_BASE + 0x174);
			g_oaPCCARDInform[ucCon].PCCARD_HIGHLBA	= (PCCARD_IO_BASE + 0x175);
			g_oaPCCARDInform[ucCon].PCCARD_DEVICE	= (PCCARD_IO_BASE + 0x176);
			g_oaPCCARDInform[ucCon].PCCARD_STATUS	= (PCCARD_IO_BASE + 0x177);
			g_oaPCCARDInform[ucCon].PCCARD_COMMAND	= (PCCARD_IO_BASE + 0x177);			

			PCCARD_SetTaskFileBaseAddr(ucCon, (PCCARD_IO_BASE + 0x170));
			break;
		// defulat : PCCARD_MEM_MODE
		case ePCCARD_COMMON_MEMORY :
			g_oaPCCARDInform[ucCon].PCCARD_DATA 		= (PCCARD_MEM_BASE + 0x00);
			g_oaPCCARDInform[ucCon].PCCARD_FEATURE	= (PCCARD_MEM_BASE + 0x01);
			g_oaPCCARDInform[ucCon].PCCARD_SECTOR	= (PCCARD_MEM_BASE + 0x02);
			g_oaPCCARDInform[ucCon].PCCARD_LOWLBA	= (PCCARD_MEM_BASE + 0x03);
			g_oaPCCARDInform[ucCon].PCCARD_MIDLBA	= (PCCARD_MEM_BASE + 0x04);
			g_oaPCCARDInform[ucCon].PCCARD_HIGHLBA	= (PCCARD_MEM_BASE + 0x05);
			g_oaPCCARDInform[ucCon].PCCARD_DEVICE	= (PCCARD_MEM_BASE + 0x06);
			g_oaPCCARDInform[ucCon].PCCARD_STATUS	= (PCCARD_MEM_BASE + 0x07);
			g_oaPCCARDInform[ucCon].PCCARD_COMMAND	= (PCCARD_MEM_BASE + 0x07);			

			PCCARD_SetTaskFileBaseAddr(ucCon, (PCCARD_MEM_BASE + 0x00));
			break;
	}
	PCCARD_SetCardMode(ucCon, 0, ePccardMode);	// 0 is offset of  PCCARD_CONFIG_OPTION register
}

//////////
// Function Name : PCCARD_SetTaskFileBaseAddr
// Function Description : This function sets up the base address of PCCARD Registers.
// Input : 	ucCon		- PCCRAD controller number
// Output : 	uBaseAddress	- the baseaddress of PCCARD Task File registers
// Version : v0.1
void PCCARD_SetTaskFileBaseAddr(u8 ucCon, u32 uBaseAddress)
{
	g_paPCCARDTaskFileBase[ucCon] = (void *)uBaseAddress;
}

//////////
// Function Name : PCCARD_GetTaskFileRegValue
// Function Description : This function gets the value of PCCARD Registers.
// Input : 	uPCCARDTaskFileRegId		- the Id of PCCARD Task File Registers
// Output : 	*puPCCARDReg			- the value of specified register
// Version : v0.1
u8 PCCARD_GetTaskFileRegValue(u8 ucCon, eCF_TASKFILE_Id uPCCARDTaskFileRegId)
{
	volatile u8 *	puPCCARDBaseAddr;
	volatile u8 *	puPCCARDReg;	

	puPCCARDBaseAddr			= &(PCCARD_TASKFILE(ucCon)->rPCCARD_DATA);
	puPCCARDReg				= puPCCARDBaseAddr + uPCCARDTaskFileRegId;
//	UART_Printf("[GET]puPCCARDBase : 0x%x, RegID:0x%x, DestAddr : 0x%x, Result : 0x%x\n", puPCCARDBaseAddr, uPCCARDTaskFileRegId, puPCCARDReg, *puPCCARDReg);	
	return *puPCCARDReg;	
}

u16 PCCARD_GetTaskFileRegValue16(u8 ucCon, eCF_TASKFILE_Id uPCCARDTaskFileRegId)
{
	volatile u16 *	puPCCARDBaseAddr;
	volatile u16 *	puPCCARDReg;	

	puPCCARDBaseAddr			= (u16*)&(PCCARD_TASKFILE(ucCon)->rPCCARD_DATA);
	puPCCARDReg				= puPCCARDBaseAddr + uPCCARDTaskFileRegId;
	return *puPCCARDReg;	
}

//////////
// Function Name : PCCARD_SetTaskFileRegValue
// Function Description : This function clears the value of PCCARD TaskFile Registers.
// Input : 	ucPCCARDRegId		- the Id of PCCARD Registers
//			uValue			- the value of register
// Output : 	NONE
// Version : v0.1
void PCCARD_SetTaskFileRegValue(u8 ucCon, eCF_TASKFILE_Id uPCCARDTaskFileRegId, u32 uValue)
{
	volatile u8 *	puPCCARDBaseAddr;
	volatile u8 *	puPCCARDReg;	
	u8			ucTaskFileRegValue 	= 0;
	
	do
	{
		ucTaskFileRegValue = PCCARD_GetTaskFileRegValue( ucCon, eCF_TASKFILE_STATUS);
	} while (	ucTaskFileRegValue == 0x80 ) ;
	
	puPCCARDBaseAddr		= &(PCCARD_TASKFILE(ucCon)->rPCCARD_DATA);
	puPCCARDReg			= puPCCARDBaseAddr + uPCCARDTaskFileRegId;
	*puPCCARDReg			= uValue;
//	UART_Printf("[SET]puPCCARDBase : 0x%x, RegID:0x%x, DestAddr : 0x%x, Value : 0x%x\n", puPCCARDBaseAddr, uPCCARDTaskFileRegId, puPCCARDReg, uValue);	
}

void PCCARD_SetTaskFileRegValue16(u8 ucCon, eCF_TASKFILE_Id uPCCARDTaskFileRegId, u16 uValue)
{
	volatile u16 *	puPCCARDBaseAddr;
	volatile u16 *	puPCCARDReg;	
	u8			ucTaskFileRegValue 	= 0;
	
	do
	{
		ucTaskFileRegValue = PCCARD_GetTaskFileRegValue( ucCon, eCF_TASKFILE_STATUS);
	} while (	ucTaskFileRegValue == 0x80 ) ;
	
	puPCCARDBaseAddr		= (u16*)&(PCCARD_TASKFILE(ucCon)->rPCCARD_DATA);
	puPCCARDReg			= puPCCARDBaseAddr + uPCCARDTaskFileRegId;
	*puPCCARDReg			= uValue;
//	UART_Printf("[SET16]puPCCARDBase : 0x%x, RegID:0x%x, DestAddr : 0x%x, Value : 0x%x\n", puPCCARDBaseAddr, uPCCARDTaskFileRegId, puPCCARDReg, uValue);	
}


//////////
// Function Name : PCCARD_SetDevicePosition
// Function Description : This function implements setting of PCCARD TaskFile Registers without command reg.
// Input : 	ucCon		- PCCRAD controller number
//			uLba			- LBA number
//			uSectorCount	- Block number
// Output : 	NONE
// Version : v0.1

void PCCARD_SetDevicePosition(u8 ucCon, u32 uLba, u32 uSectorCount)
{
	PCCARD_SetTaskFileRegValue(ucCon, eCF_TASKFILE_FEATURE, 0);
	PCCARD_SetTaskFileRegValue(ucCon, eCF_TASKFILE_SECTOR,	uSectorCount);
	PCCARD_SetTaskFileRegValue(ucCon, eCF_TASKFILE_LOWLBA,	(uLba&0xff));
	PCCARD_SetTaskFileRegValue(ucCon, eCF_TASKFILE_MIDLBA,	((uLba>>  8)&0xff));
	PCCARD_SetTaskFileRegValue(ucCon, eCF_TASKFILE_HIGHLBA,	((uLba>>16)&0xff));
	PCCARD_SetTaskFileRegValue(ucCon, eCF_TASKFILE_DEVICE,	((uLba>>24)&0x0f) | 0xe0);	//LBA enabled, Drive 0
}



//////////
// Function Name : PCCARD_ReadBlocks
// Function Description : This function implements block write function to PCCARD TaskFile Registers.
// Input : 	ucCon		- PCCRAD controller number
//			uDeviceLba	- LBA number
//			uBlockCount	- Block number
//			uBufAddr		- Buffer Address
// Output : 	NONE
// Version : v0.1
void PCCARD_ReadBlocks(u8 ucCon, u32 uDeviceLba, u32 uBlockCount, u32 uBufAddr)
{

	u32 uCurrentCount;
	u32 uRemainderCount;
	u32 uCurrentLba;
	u32 uCurrentDstAddr;
	u32 uRound;
	u16* usAtaHostAddr;
	u32 uLoopCnt;
	u8		ucTaskFileRegValue 	= 0;	

#if PERF_TEST_PCCARD	
	u32			uElapsedTime = 0;
#endif

	uRemainderCount = uBlockCount;
	uCurrentLba = uDeviceLba;
	uRound = 0;	
	
	while(uRemainderCount != 0) {
		if(uRemainderCount>256) {
			uCurrentCount = 256; //0 means 256
			uRemainderCount -= 256;
		} else {
			uCurrentCount = uRemainderCount;
			uRemainderCount = 0;
		}
		uCurrentLba = uDeviceLba + uRound*256;
		uCurrentDstAddr = uBufAddr+ uRound*256*512;
		usAtaHostAddr = (u16*)uCurrentDstAddr;
		
		PCCARD_SetDevicePosition(ucCon, uCurrentLba, uCurrentCount);
		PCCARD_SetTaskFileRegValue(ucCon, eCF_TASKFILE_COMMAND, eATA_CMD_READSECTOR);

		while(uCurrentCount-- )
			{	
				//-------- step 3. Read status register & compare with '0x58'  ------------
				//  0x58 : Data of device side is ready. The host should not write the command register when 0x58. (DRQ = 1)
				do
				{
					ucTaskFileRegValue = PCCARD_GetTaskFileRegValue(ucCon, eCF_TASKFILE_STATUS);
					Delay(1); // need over 25MHz
			//		UART_Printf("[STATUS:0x%x]\n", ucTaskFileRegValue);
				}
				while( ucTaskFileRegValue != 0x58) ;
#if PERF_TEST_PCCARD	
	StartTimer(2);
#endif
			for (uLoopCnt=0;uLoopCnt<(512/2);uLoopCnt++) 
				{			
					*usAtaHostAddr = PCCARD_GetTaskFileRegValue16(ucCon, eCF_TASKFILE_DATA);
					usAtaHostAddr++;
				}
#if PERF_TEST_PCCARD
	uElapsedTime += StopTimer(2);
#endif
		}		
		
		//-------- step 3. Read status register & compare with '0x50'  ------------
		do
		{
			ucTaskFileRegValue = PCCARD_GetTaskFileRegValue(ucCon, eCF_TASKFILE_STATUS);
			Delay(1); // need over 25MHz
//			UART_Printf("[STATUS:0x%x]\n", ucTaskFileRegValue);
		}
		while( ucTaskFileRegValue != 0x50 && ucTaskFileRegValue != 0x58 ) ;

		uRound++;
	}
#if PERF_TEST_PCCARD
	UART_Printf(" PCCard Mode Reading time : %d us, Performance : %lf MByte/sec\n"
			,uElapsedTime,(float)(uBlockCount*512./uElapsedTime));
#endif
//	return TRUE;
}


/*
	u8		ucTaskFileRegValue 	= 0;
	u32		uBlockLoopCount		= 0;
	u32		uByteLoopCount		= 0;
	u16*	usCurrBufAddr		= (u16*) g_oaPCCARDInform[ucCon].puPCCARDReadBuf;
	u16		usReadData			= 0;

//	UART_Printf("uDeviceLab : 0x%x, uBlockCount : 0x%x\n", uDeviceLba, uBlockCount);
#if PERF_TEST_PCCARD	
	u32			uElapsedTime = 0;
#endif
	
	//-------- step 1. Set up task file registers ------------
	// uBlocks: sector count
	// uLba:     (head&0x0f)<<24 | (LBA_high<<16) | (LBA_mid<<8) | LBA_low
	// uSrcAddr:    host address
	PCCARD_SetTaskFileRegValue(ucCon, eCF_TASKFILE_FEATURE, 0);
	PCCARD_SetTaskFileRegValue(ucCon, eCF_TASKFILE_SECTOR,	uBlockCount);
	PCCARD_SetTaskFileRegValue(ucCon, eCF_TASKFILE_LOWLBA,	(uDeviceLba&0xff));
	PCCARD_SetTaskFileRegValue(ucCon, eCF_TASKFILE_MIDLBA,	((uDeviceLba>>  8)&0xff));
	PCCARD_SetTaskFileRegValue(ucCon, eCF_TASKFILE_HIGHLBA,	((uDeviceLba>>16)&0xff));
	PCCARD_SetTaskFileRegValue(ucCon, eCF_TASKFILE_DEVICE,	((uDeviceLba>>24)&0x0f) | 0xe0);	//LBA enabled, Drive 0
#ifdef DBG_PCCARD
	ucTaskFileRegValue = PCCARD_GetTaskFileRegValue(ucCon, eCF_TASKFILE_SECTOR);
	UART_Printf("Sector W:0x%x, R:0x%x\n",uBlockCount,ucTaskFileRegValue);
	ucTaskFileRegValue = PCCARD_GetTaskFileRegValue(ucCon, eCF_TASKFILE_LOWLBA);
	UART_Printf("lLBA W:0x%x, R:0x%x\n",(uDeviceLba&0x00ff),ucTaskFileRegValue);
	ucTaskFileRegValue = PCCARD_GetTaskFileRegValue(ucCon, eCF_TASKFILE_MIDLBA);
	UART_Printf("mLBA W:0x%x, R:0x%x\n",((uDeviceLba>>8)&0x00ff),ucTaskFileRegValue);
	ucTaskFileRegValue = PCCARD_GetTaskFileRegValue(ucCon, eCF_TASKFILE_HIGHLBA);
	UART_Printf("hLBA W:0x%x, R:0x%x\n",((uDeviceLba>>16)&0x00ff),ucTaskFileRegValue);
	ucTaskFileRegValue = PCCARD_GetTaskFileRegValue(ucCon, eCF_TASKFILE_DEVICE);
	UART_Printf("device W:0x%x, R:0x%x\n",((uDeviceLba>>24)&0x000f) | 0xe0,ucTaskFileRegValue);
#endif

	//-------- step 2. Set '0x30(eATA_CMD_WRITESECTOR)' in the command register of task file registers ------------
	PCCARD_SetTaskFileRegValue(ucCon, eCF_TASKFILE_COMMAND,	eATA_CMD_READSECTOR );

	//-------- step 3. Read status register & compare with '0x58'  ------------
	//  0x58 : Data of device side is ready. The host should not write the command register when 0x58. (DRQ = 1)
	do
	{
		ucTaskFileRegValue = PCCARD_GetTaskFileRegValue(ucCon, eCF_TASKFILE_STATUS);
		Delay(1); // need over 25MHz
//		UART_Printf("[STATUS:0x%x]\n", ucTaskFileRegValue);
	}
	while( ucTaskFileRegValue != 0x58) ;
	
#if PERF_TEST_PCCARD	
	StartTimer(0);
#endif
	//-------- step 4. Writes the data register until 256 times(512bytes) ------------
	// Note. We can transfer data in 16bit mode because PCCARD_DATA register is overlapped to the PCCARD_ERROR register.
	for ( uBlockLoopCount = 0; uBlockLoopCount < uBlockCount; uBlockLoopCount++ )
	{
		for ( uByteLoopCount = 0; uByteLoopCount <  (512/2); uByteLoopCount++ )
		{
			usReadData = PCCARD_GetTaskFileRegValue16(ucCon, eCF_TASKFILE_DATA);
//			UART_Printf("BufADDR:0x%x, Data : 0x%x\n", usCurrBufAddr, usReadData);
			*usCurrBufAddr =  usReadData;
			usCurrBufAddr++;
		}
		
		//-------- step 3. Read status register & compare with '0x50'  ------------
		do
		{
			ucTaskFileRegValue = PCCARD_GetTaskFileRegValue(ucCon, eCF_TASKFILE_STATUS);
			Delay(1); // need over 25MHz
//			UART_Printf("[STATUS:0x%x]\n", ucTaskFileRegValue);
		}
		while( ucTaskFileRegValue != 0x50 && ucTaskFileRegValue != 0x58 ) ;
	}
#if PERF_TEST_PCCARD
	uElapsedTime = StopTimer(0);
	UART_Printf(" PCCard Mode Reading time : %d us, Performance : %lf MByte/sec\n"
			,uElapsedTime,(float)(uBlockCount*512./uElapsedTime));
#endif	

}
*/




//////////
// Function Name : PCCARD_WriteBlocks
// Function Description : This function implements block write function to PCCARD TaskFile Registers.
// Input : 	ucCon		- PCCRAD controller number
//			uDeviceLba	- LBA number
//			uBlockCount	- Block number
//			uBufAddr		- Buffer Address
// Output : 	NONE
// Version : v0.1
void PCCARD_WriteBlocks(u8 ucCon, u32 uDeviceLba, u32 uBlockCount, u32 uBufAddr)
{
	u32 uCurrentCount;
	u32 uRemainderCount;
	u32 uCurrentLba;
	u32 uCurrentSrcAddr;
	u32 uRound;
	u16* usAtaHostAddr;
	u32 uLoopCnt;
	u8		ucTaskFileRegValue 	= 0;	
/*
	u8		ucTaskFileRegValue 	= 0;
	u32		uBlockLoopCount		= 0;
	u32		uByteLoopCount		= 0;
	u16*	usCurrBufAddr		= (u16*) g_oaPCCARDInform[ucCon].puPCCARDWriteBuf;

//	UART_Printf("uDeviceLab : 0x%x, uBlockCount : 0x%x\n", uDeviceLba, uBlockCount);
#if PERF_TEST_PCCARD	
	u32			uElapsedTime = 0;
#endif


	//-------- step 1. Set up task file registers ------------
	// uBlocks: sector count
	// uLba:     (head&0x0f)<<24 | (LBA_high<<16) | (LBA_mid<<8) | LBA_low
	// uSrcAddr:    host address
	PCCARD_SetTaskFileRegValue(ucCon, eCF_TASKFILE_FEATURE, 0);
	PCCARD_SetTaskFileRegValue(ucCon, eCF_TASKFILE_SECTOR,	uBlockCount);
	PCCARD_SetTaskFileRegValue(ucCon, eCF_TASKFILE_LOWLBA,	(uDeviceLba&0xff));
	PCCARD_SetTaskFileRegValue(ucCon, eCF_TASKFILE_MIDLBA,	((uDeviceLba>>  8)&0xff));
	PCCARD_SetTaskFileRegValue(ucCon, eCF_TASKFILE_HIGHLBA,	((uDeviceLba>>16)&0xff));
	PCCARD_SetTaskFileRegValue(ucCon, eCF_TASKFILE_DEVICE,	((uDeviceLba>>24)&0x0f) | 0xe0);	//LBA enabled, Drive 0




#ifdef DBG_PCCARD
	ucTaskFileRegValue = PCCARD_GetTaskFileRegValue(ucCon, eCF_TASKFILE_SECTOR);
	UART_Printf("Sector W:0x%x, R:0x%x\n",uBlockCount,ucTaskFileRegValue);
	ucTaskFileRegValue = PCCARD_GetTaskFileRegValue(ucCon, eCF_TASKFILE_LOWLBA);
	UART_Printf("lLBA W:0x%x, R:0x%x\n",(uDeviceLba&0x00ff),ucTaskFileRegValue);
	ucTaskFileRegValue = PCCARD_GetTaskFileRegValue(ucCon, eCF_TASKFILE_MIDLBA);
	UART_Printf("mLBA W:0x%x, R:0x%x\n",((uDeviceLba>>8)&0x00ff),ucTaskFileRegValue);
	ucTaskFileRegValue = PCCARD_GetTaskFileRegValue(ucCon, eCF_TASKFILE_HIGHLBA);
	UART_Printf("hLBA W:0x%x, R:0x%x\n",((uDeviceLba>>16)&0x00ff),ucTaskFileRegValue);
	ucTaskFileRegValue = PCCARD_GetTaskFileRegValue(ucCon, eCF_TASKFILE_DEVICE);
	UART_Printf("device W:0x%x, R:0x%x\n",((uDeviceLba>>24)&0x000f) | 0xe0,ucTaskFileRegValue);
#endif
*/
#if PERF_TEST_PCCARD	
	u32			uElapsedTime = 0;
#endif

	uRemainderCount = uBlockCount;
	uCurrentLba = uDeviceLba;
	uRound = 0;	
	
	while(uRemainderCount != 0) 
		{
			if(uRemainderCount>256) 
				{
					uCurrentCount = 256; 			//0 means 256
					uRemainderCount -= 256;
				} 
			else
				{
					uCurrentCount = uRemainderCount;
					uRemainderCount = 0;
				}
			
			uCurrentLba = uDeviceLba + uRound*256;
			uCurrentSrcAddr = uBufAddr+ uRound*256*512;
			usAtaHostAddr = (u16*)uCurrentSrcAddr;
			
			PCCARD_SetDevicePosition(ucCon, uCurrentLba, uCurrentCount);
			PCCARD_SetTaskFileRegValue(ucCon, eCF_TASKFILE_COMMAND, eATA_CMD_WRITESECTOR);

			while(uCurrentCount-- ) 
				{	
					//-------- step 3. Read status register & compare with '0x58'  ------------
					//  0x58 : Data of device side is ready. The host should not write the command register when 0x58. (DRQ = 1)
					do
					{
						ucTaskFileRegValue = PCCARD_GetTaskFileRegValue(ucCon, eCF_TASKFILE_STATUS);
						Delay(1); // need over 25MHz
//						UART_Printf("[STATUS:0x%x]\n", ucTaskFileRegValue);
					}
					while( ucTaskFileRegValue != 0x58) ;
#if PERF_TEST_PCCARD	
	StartTimer(2);
#endif
					for (uLoopCnt=0;uLoopCnt<(512/2);uLoopCnt++)
					{			
						PCCARD_SetTaskFileRegValue16(ucCon, eCF_TASKFILE_DATA,	*usAtaHostAddr );
						usAtaHostAddr++;
					}
#if PERF_TEST_PCCARD
	uElapsedTime += StopTimer(2);
#endif

		
				}	




				//-------- step 3. Read status register & compare with '0x50'  ------------
				do
				{
					ucTaskFileRegValue = PCCARD_GetTaskFileRegValue(ucCon, eCF_TASKFILE_STATUS);
					Delay(1); // need over 25MHz
		//			UART_Printf("[STATUS:0x%x]\n", ucTaskFileRegValue);
				}
				while( ucTaskFileRegValue != 0x50 && ucTaskFileRegValue != 0x58 ) ;

							
					uRound++;
		}



	
#if PERF_TEST_PCCARD
	UART_Printf(" PCCard Mode Writing time : %d us, Performance : %lf MByte/sec\n"
			,uElapsedTime,(float)(uBlockCount*512./uElapsedTime));
	
#endif

//	return TRUE;
}



/*




	//-------- step 2. Set '0x30(eATA_CMD_WRITESECTOR)' in the command register of task file registers ------------
	PCCARD_SetTaskFileRegValue(ucCon, eCF_TASKFILE_COMMAND,	eATA_CMD_WRITESECTOR );

	//-------- step 3. Read status register & compare with '0x58'  ------------
	//  0x58 : Data of device side is ready. The host should not write the command register when 0x58. (DRQ = 1)
	do
	{
		ucTaskFileRegValue = PCCARD_GetTaskFileRegValue(ucCon, eCF_TASKFILE_STATUS);
		Delay(1); // need over 25MHz
		UART_Printf("[STATUS:0x%x]\n", ucTaskFileRegValue);
	}
	while( ucTaskFileRegValue != 0x58) ;

#if PERF_TEST_PCCARD	
	StartTimer(0);
#endif

	//-------- step 4. Writes the data register until 256 times(512bytes) ------------
	// Note. We can transfer data in 16bit mode because PCCARD_DATA register is overlapped to the PCCARD_ERROR register.
	for ( uBlockLoopCount = 0; uBlockLoopCount < uBlockCount; uBlockLoopCount++ )
	{
		for ( uByteLoopCount = 0; uByteLoopCount <  (512/2); uByteLoopCount++ )
		{
			PCCARD_SetTaskFileRegValue16(ucCon, eCF_TASKFILE_DATA,	*usCurrBufAddr );
//			UART_Printf("BufADDR:0x%x\n", usCurrBufAddr);
			usCurrBufAddr++;
		}
		
		//-------- step 3. Read status register & compare with '0x50'  ------------
		do
		{
			ucTaskFileRegValue = PCCARD_GetTaskFileRegValue(ucCon, eCF_TASKFILE_STATUS);
			Delay(1); // need over 25MHz
//			UART_Printf("[STATUS:0x%x]\n", ucTaskFileRegValue);
		}
		while( ucTaskFileRegValue != 0x50 && ucTaskFileRegValue != 0x58 ) ;
	}
#if PERF_TEST_PCCARD
	uElapsedTime = StopTimer(0);
	UART_Printf(" PCCard Mode Writing time : %d us, Performance : %lf MByte/sec\n"
			,uElapsedTime,(float)(uBlockCount*512./uElapsedTime));
#endif
	
	
}
*/
//=========================================================================
//					          	          ATTRIBUTE MEMORY AREA
//=========================================================================
/*---------------------------------- APIs of controling Attribute memory area in PCCARD mode  ---------------------------------*/
//////////
// Function Name : PCCARD_SetAttConfigBaseAddr
// Function Description : This function sets up the base address of PCCARD Attribute Configuration Registers.
// Input : 	ucCon		- PCCRAD controller number
// Output : 	uBaseAddress	- the baseaddress of PCCARD Task File registers
// Version : v0.1
void PCCARD_SetAttConfigBaseAddr(u8 ucCon, u32 uBaseAddress)
{
	g_paPCCARDATTBase[ucCon] = (void *)uBaseAddress;
}

//////////
// Function Name : PCCARD_SetCardMode
// Function Description : 
//   This function changes CF CARD mode.
// Input : 	ucCon 		- PCCARD Controller Number 
//			ePccardAttId	- Attribute configuration register select
//			ePccardMode	-MEM/IO1/IO2/IO3 mode select
// Output : NONE
// Version : v0.1 
void PCCARD_SetCardMode(u8 ucCon, u8 ucPccardAttIdOffset, ePCCARD_MODE ePccardMode)
{
	u8		uRegValue = 0;

	uRegValue = PCCARD_GetAttRegValue(ucCon, ucPccardAttIdOffset);
	uRegValue = uRegValue & ~(0x3f<< 0 ) | (ePccardMode<<0);		

	PCCARD_SetAttRegValue(ucCon, ucPccardAttIdOffset, uRegValue);	
}

//////////
// Function Name : PCCARD_GetAttRegValue
// Function Description : This function gets the value of PCCARD Registers.
// Input : 	ucCon					- PCCRAD controller number
//			uPCCARDAttRegId			- the Id of PCCARD Attribute Configuration Register
// Output : 	*puPCCARDReg			- the value of specified register
// Version : v0.1
u8 PCCARD_GetAttRegValue(u8 ucCon, u8 uPCCARDAttRegIdOffset)
{
	volatile u16 *	puPCCARDAttBaseAddr;
	volatile u16 *	puPCCARDReg;	

	puPCCARDAttBaseAddr			= &(PCCARD_ATTR(ucCon)->rPCCARD_CONFIG_OPTION);
	puPCCARDReg				= puPCCARDAttBaseAddr + uPCCARDAttRegIdOffset;

	return (u8)(*puPCCARDReg);	
}

//////////
// Function Name : PCCARD_SetAttRegValue
// Function Description : This function clears the value of PCCARD TaskFile Registers.
// Input : 	ucCon			- PCCRAD controller number
//			ucPCCARDRegId	- the Id of PCCARD Registers
//			uValue			- the value of register
// Output : 	NONE
// Version : v0.1
void PCCARD_SetAttRegValue(u8 ucCon, u8 uPCCARDAttRegIdOffset, u8 uValue)
{
	volatile u16 *	puPCCARDAttBaseAddr;
	volatile u16 *	puPCCARDReg;	

	puPCCARDAttBaseAddr		= &(PCCARD_ATTR(ucCon)->rPCCARD_CONFIG_OPTION);
	puPCCARDReg			= puPCCARDAttBaseAddr + uPCCARDAttRegIdOffset;

	*puPCCARDReg			= uValue;
}

//////////
// Function Name : PCCARD_GetAttribData
// Function Description : This function gets the value of PCCARD Registers.
// Input : 	ucCon		- PCCRAD controller number
//			uIndex		- the Index of PCCARD Attribute Memory
// Output : 	*puPCCARDReg			- the value of specified register
// Version : v0.1
u8 PCCARD_GetAttribData(u8 ucCon, u8 uIndex)
{
	volatile u16 *	puPCCARDAttBaseAddr;
	volatile u16 *	puPCCARDReg;	

	puPCCARDAttBaseAddr			= &(PCCARD_ATTR(ucCon)->rPCCARD_CONFIG_OPTION);
	puPCCARDAttBaseAddr			-= 0x100;
	puPCCARDReg				= puPCCARDAttBaseAddr + uIndex;
//	UART_Printf("uIndex:0x%x, BaseAddr:0x%x, DestAddr:0x%x\n", uIndex, puPCCARDAttBaseAddr, puPCCARDReg);
	return (u8)(*puPCCARDReg);
}

//////////
// Function Name : PCCARD_SetAttribData
// Function Description : This function clears the value of PCCARD TaskFile Registers.
// Input : 	ucCon		- PCCRAD controller number
//			uIndex		- the Index of PCCARD Attribute Memory
//			uValue		- the value of register
// Output : 	NONE
// Version : v0.1
void PCCARD_SetAttribData(u8 ucCon, u8 uIndex, u8 uValue)
{
	volatile u16 *	puPCCARDAttBaseAddr;
	volatile u8 *	puPCCARDReg;	

	puPCCARDAttBaseAddr		= &(PCCARD_ATTR(ucCon)->rPCCARD_CONFIG_OPTION);
	puPCCARDAttBaseAddr		-= 0x100;
	puPCCARDReg			= (u8 *)puPCCARDAttBaseAddr + uIndex;

	*puPCCARDReg			= uValue;
}



