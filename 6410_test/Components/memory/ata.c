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
*	File Name : ata.c
*  
*	File Description : This file implements the driver functions for CF Controller and PIO/UDMA mode on it.
*
*	Author : Sunil,Roe
*	Dept. : AP Development Team
*	Created Date : 2007/1/26
*	Version : 0.1 
* 
*	History
*	- Created(Sunil,Roe 2007/1/26)
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
#include "timer.h"
#include "cf.h"
#include "ata.h"
#include "gpio.h"


#define ATA(__n) 				( ( volatile oATA_REGS * ) ( g_paATASFRBase[__n] ) )

#define SMDK6400 1
/*=========================================================================
 *          	               ata Command
 *=========================================================================
 */
#define ATA_CMD_IDENTIFYDEVICE		0xec
#define ATA_CMD_READSECTOR		0x20
#define ATA_CMD_READMULTIPLE		0xc4
#define ATA_CMD_READDMA			0xc8
#define ATA_CMD_WRITESECTOR		0x30
#define ATA_CMD_WRITEMULTIPLE		0xc5
#define ATA_CMD_WRITEDMA			0xca
#define ATA_CMD_SETFEATURES		0xEF

typedef struct tag_ATA_REGS
{
	u32	rrATA_CONTROL;		// ATA enable and clock down status
	u32	rrATA_STATUS;		// ATA status
	u32	rrATA_COMMAND;		// ATA command
	u32	rrATA_SWRST;		// ATA software reset
	u32	rrATA_IRQ;			// ATA interrupt sources
	u32	rrATA_IRQ_MASK;		// ATA interrupt mask
	u32	rrATA_CFG;			// ATA configuration for ATA interface
	u32	rReserved1;
	u32	rReserved2;
	u32	rReserved3;
	u32	rReserved4;
	u32	rrATA_PIO_TIME;		// ATA PIO timing
	u32	rrATA_UDMA_TIME;	// ATA UDMA timing
	u32	rrATA_XFR_NUM;		// ATA transfer number
	u32	rrATA_XFR_CNT;		// ATA current transfer count
	u32	rrATA_TBUF_START;	// ATA start address of track buffer
	u32	rrATA_TBUF_SIZE;	// ATA size of track buffer
	u32	rrATA_SBUF_START;	// ATA start address of source buffer
	u32	rrATA_SBUF_SIZE;	// ATA size of source buffer
	u32	rrATA_CADR_TBUF;	// ATA current write address of track buffer
	u32	rrATA_CADR_SBUF;	// ATA current read address of source buffer
	u32	rrATA_PIO_DTR;		// ATA PIO device data register
	u32	rrATA_PIO_FED;		// ATA PIO device Feature/Error register
	u32	rrATA_PIO_SCR;		// ATA PIO sector count register
	u32	rrATA_PIO_LLR;		// ATA PIO device LBA Low register
	u32	rrATA_PIO_LMR;		// ATA PIO device LBA middle register
	u32	rrATA_PIO_LHR;		// ATA PIO device LBA high register
	u32	rrATA_PIO_DVR;		// ATA PIO device register
	u32	rrATA_PIO_CSD;		// ATA PIO device command/status register
	u32	rrATA_PIO_DAD;		// ATA PIO device control/alternate status register
	u32	rrATA_PIO_READY;	// ATA PIO read data from device data register	
	u32	rrATA_PIO_RDATA;	// ATA PIO read data from device data register
	u32	rReserved5;
	u32	rReserved6;
	u32	rReserved7;
	u32	rReserved8;
	u32	rrBUS_FIFO_STATUS;	// ATA internal AHB FIFO status
	u32	rrATA_FIFO_STATUS;	// ATA internal ATA FIFO status
} 
oATA_REGS;

// define global variables
static void *				g_paATASFRBase[ATA_CONNUM];
extern volatile ATA_oInform	g_oaATAInform[];
extern volatile ATA_oInform	g_oaATAInformMaster[ATA_CONNUM];
extern volatile ATA_oInform	g_oaATAInformSlave[ATA_CONNUM];

/*---------------------------------- Init Functions  ---------------------------------*/
void ATA_SetSFRBaseAddr(u8 ucCon, u32 uBaseAddress);
void ATA_InitPort(u8, u8);
void ATA_InitBufferControl(void);
void ATA_SetTimingParams(u8 ucCon, eATA_MODE_6400 eMode, u32 uValue);
/*---------------------------------- Functions for Implement ATA Test functions ---------------------------------*/
void ATA_WaitForDeviceReady(u8 ucCon);
u8 ATA_GetDeviceReg(u8 ucCon, eCF_TASKFILE_Id eTaskFileRegId);
u16 ATA_GetDataFromDevice(u8 ucCon);
void ATA_SetDataToDevice(u8 ucCon, u16 usData);
u8 ATA_WriteSectors_PIO(u8 ucCon, u32 uLBA, u32 uSectorCount, u32 uSrcAddress, eATA_SLOT_SEL eSlotSel);
u8 ATA_ReadSectors_PIO(u8 ucCon, u32 uLBA, u32 uSectorCount, u32 uDstAddress, eATA_SLOT_SEL eSlotSel);
u8 ATA_WriteSectors_PDMA(u8 ucCon, u32 uLBA, u32 uSectorCount, u32 uSrcAddress, eATA_SLOT_SEL eSlotSel);
u8 ATA_ReadSectors_PDMA(u8 ucCon, u32 uLBA, u32 uSectorCount, u32 uDstAddress, eATA_SLOT_SEL eSlotSel);
u8 ATA_WriteSectors_UDMA(u8 ucCon, u32 uLBA, u32 uSectorCount, u32 uSrcAddress, eATA_SLOT_SEL eSlotsel);
u8 ATA_ReadSectors_UDMA(u8 ucCon, u32 uLBA, u32 uSectorCount, u32 uDstAddress, eATA_SLOT_SEL eSlotsel);
void ATA_SetDevicePosition(u8 ucCon, u32 uLba, u32 uSectorCount,eATA_SLOT_SEL eSlotsel);
u8 ATA_IsWritingSectorsDone(u8 ucCon);
u8 ATA_IsReadingSectorsDone(u8 ucCon);
u8 ATA_IsWritingBlocksDone(u8 ucCon);
u8 ATA_IsReadingBlocksDone(u8 ucCon);
u8 ATA_IsDmaDone(u8 ucCon);
/*---------------------------------- APIs of ATA_STATUS Registers ---------------------------------*/
void ATA_WaitForTransferDone(u8 ucCon);
void ATA_SetSwRst(u8 ucCon, u8 ucEnFlag);
void ATA_SetTransferCommand(u8 ucCon, eATA_XFR_CMD eXfrCmd);
void ATA_SetIRQ(u8 ucCon, eATA_IRQ_SRC eIrqSrc);
void ATA_SetIRQMask(u8 ucCon, eATA_IRQ_SRC eIrqSrc);
/*---------------------------------- APIs of ATA_CFG Registers ---------------------------------*/
void ATA_SetConfig(u8 ucCon, eATA_MODE_6400 eMode, eATA_DMA_DIR eDmaDir);
void ATA_SetUdmaAutoMode(u8 ucCon, u8 ucEnFlag);
void ATA_SetEndian(u8 ucCon, eATA_ENDIAN eEndianMode);
void ATA_SetDMAXferDir(u8 ucCon, eATA_DMA_DIR eDmaDir);
void ATA_SetXferMode(u8 ucCon, eATA_MODE_6400 eAtaMode);
void ATA_SetIORDY(u8 ucCon, u8 ucEnFlag);
void ATA_SetRst(u8 ucCon, u8 ucEnFlag);
void ATA_SetSBufStart(u8 ucCon, u32 uBufAddr);
void ATA_SetTBufStart(u8 ucCon, u32 uBufAddr);
void ATA_SetSBufSize(u8 ucCon, u32 uSize);
void ATA_SetTBufSize(u8 ucCon, u32 uSize);
void ATA_SetXfrNum(u8 ucCon, u32 uNum);
u8 ATA_GetTaskFileRegValue(u8 ucCon, eCF_TASKFILE_Id uATATaskFileRegId);
u16 ATA_GetTaskFileRegValue16(u8 ucCon, eCF_TASKFILE_Id uATATaskFileRegId);
void ATA_SetTaskFileRegValue(u8 ucCon, eCF_TASKFILE_Id uATATaskFileRegId, u32 uValue);
void ATA_SetTaskFileRegValue16(u8 ucCon, eCF_TASKFILE_Id uATATaskFileRegId, u16 uValue);
/*---------------------------------- APIs of ATA_FIFO_STATUS Registers ---------------------------------*/
void ATA_WaitForHostReady(u8 ucCon);
/*---------------------------------- APIs of general ATA ---------------------------------*/
u32 ATA_GetRegAddr(u8 ucCon, eATA_Id uATARegId);
u32 ATA_GetRegValue(u8 ucCon, eATA_Id uATARegId);
void ATA_SetRegValue(u8 ucCon, eATA_Id uATARegId, u32 uValue);

/*---------------------------------- Init Functions  ---------------------------------*/
//////////
// Function Name : ATA_Init
// Function Description : This function initializes a certain ATA Controller.
// Input : 	ucCon - ATA Controller Number 
// Output : 	TRUE 	- Memory Device is reset
//			FALSE	- Memory Device is not reset because of ERROR
// Version : v0.1
u8	ATA_Init(u8 ucCon, u8 ucOpMode)
{
	u32 uSFRBaseAddress;			// for PCCARD Controller Base Register

	Assert( (ucCon == ATA_CON0) );

	if (ucCon == ATA_CON0)
	{
		uSFRBaseAddress			= ATA_BASE;
	}
	else
	{	
		return FALSE;
	}

	// EBI Setup for CF controller
	CF_SetEBI(ucOpMode);
	ATA_SetSFRBaseAddr(ucCon, uSFRBaseAddress);
	ATA_InitBuffer(ucCon);
	ATA_InitPort(ucCon,ucOpMode);

	return TRUE;
}

//////////
// Function Name : ATA_SetSFRBaseAddr
// Function Description : This function sets up the base address of ATA Registers.
// Input : 	ucCon		- ATA controller number
//			uBaseAddress	- the baseaddress of ATA registers
// Output : 	NONE
// Version : v0.1
void ATA_SetSFRBaseAddr(u8 ucCon, u32 uBaseAddress)
{
	g_paATASFRBase[ucCon] = (void *)uBaseAddress;
}

//////////
// Function Name : ATA_InitBuffer
// Function Description : This function sets up the buffer address of ATA Registers.
// Input : 	ucCon		- ATA controller number
// Output : 	NONE
// Version : v0.1
void ATA_InitBuffer(u8 ucCon)
{
	g_oaATAInform[ucCon].puATAWriteBuf = (u32*) CF_WRITE_BUF;
	g_oaATAInform[ucCon].puATAReadBuf = (u32*) (CF_WRITE_BUF + 0x400000); 
}

//////////
// Function Name : ATA_InitPort
// Function Description : 
//   This function Initialize ports as ATA.
// Input : ucCon - ATA Controller Number 
// Output : NONE
// Version : v0.1 
void ATA_InitPort(u8 ucCon, u8 ucOpMode)
{
	switch(ucCon)
	{
		case ATA_CON0 :
		 	if (ucOpMode == DIRECT_MODE)	
		 	{ 
#if 1	// default.. OK 	A1,D1,C1 through Buffer(6400)
				GPIO_SetFunctionAll(eGPIO_K, 0x55555555, 0x55555555);	// D: Set XhiDATA[15:0] pins as CF Data[15:0] 
				GPIO_SetFunctionAll(eGPIO_L, 0x00000666, 0x0);			// A: Set XhiADDR[2:0] pins as CF ADDR[2:0]
				GPIO_SetFunctionAll(eGPIO_M, 0x00066666, 0x0);			// C: Set Xhi control pins as CF control pins(IORDY, IOWR, IORD, CE[1], CE[0])
#endif

#if 0	// default.. OK 	A1,D1,C1 Direct mode(6410 direct)
				GPIO_SetFunctionAll(eGPIO_K, 0x55555555, 0x55555555);	// D: Set XhiDATA[15:0] pins as CF Data[15:0] 
				GPIO_SetFunctionAll(eGPIO_L, 0x66666666, 0x0);			// A: Set XhiADDR[2:0] pins as CF ADDR[2:0],Set XhiADDR[3:7] pins as CF Config pins(INTata, RESETata, INPACKata, REGata, CData)
				GPIO_SetFunctionAll(eGPIO_M, 0x00066666, 0x0);			// C: Set Xhi control pins as CF control pins(IORDY, IOWR, IORD, CE[1], CE[0])
#endif


#if 0	// OK 	A1,D1,C2
				GPIO_SetFunctionAll(eGPIO_K, 0x55555555, 0x55555555);	// D: Set XhiDATA[15:0] pins as CF Data[15:0] 
				GPIO_SetFunctionAll(eGPIO_L, 0x66666666, 0x00055555);	// A1: Set XhiADDR[2:0] pins as CF ADDR[2:0], C2 : Set XhiADDR[12:8] as CF control pins(IORDY, IOWR, IORD, CE[1], CE[0])
#endif
#if 0	// A1,D1,D2,C2
				GPIO_SetFunctionAll(eGPIO_K, 0x55555555, 0x00000055);	// D1: Set XhiDATA[9:0] pins as CF Data[9:0] 
				GPIO_SetFunctionAll(eGPIO_M, 0x00555555, 0x00000000);	// D2: Set Xhi control pins as CF Data[15:10] 
				GPIO_SetFunctionAll(eGPIO_L, 0x00000666, 0x00055555);	// A1: Set XhiADDR[2:0] pins as CF ADDR[2:0], C2 : Set XhiADDR[12:8] as CF control pins(IORDY, IOWR, IORD, CE[1], CE[0])
#endif
#if 0	// A1,D1,D2,D3,C2
				GPIO_SetFunctionAll(eGPIO_K, 0x55555555, 0x00000000);	// D1: Set XhiDATA[7:0] pins as CF Data[7:0] 
				GPIO_SetFunctionAll(eGPIO_M, 0x00555555, 0x00000000);	// D2: Set Xhi control pins as CF Data[15:10] 
				GPIO_SetFunctionAll(eGPIO_L, 0x00000666, 0x05555555);	// A1: Set XhiADDR[2:0] pins as CF ADDR[2:0], C2 : Set XhiADDR[12:8] as CF control pins(IORDY, IOWR, IORD, CE[1], CE[0]), D3 : Set XhiData[17:16] as CF Data[9:8] 
#endif
#if 0	// OK		A4,D1,C2
				GPIO_SetFunctionAll(eGPIO_K, 0x55555555, 0x55555555);	// Set XhiDATA[15:0] pins as CF Data[15:0] 
				GPIO_SetFunctionAll(eGPIO_B, 0x00000555, 0x00000000);	// Set XuRXD2, XuTXD2, XuRXD3 pins as CF ADDR[2:0]
				GPIO_SetFunctionAll(eGPIO_L, 0x66666000, 0x00055555);	// A: Set XhiADDR[3:7] pins as CF Config pins(INTata, RESETata, INPACKata, REGata, CData), Set XhiADDR[12:8] as CF control pins(IORDY, IOWR, IORD, CE[1], CE[0])
//				GPIO_SetFunctionAll(eGPIO_L, 0x00000000, 0x00055555);	// Set XhiADDR[12:8] as CF control pins(IORDY, IOWR, IORD, CE[1], CE[0])
#endif
#if 0	// only for type2 CPU board	A10,D1,C2 OK
				GPIO_SetFunctionAll(eGPIO_K, 0x55555555, 0x55555555);	// Set XhiDATA[15:0] pins as CF Data[15:0] 
				GPIO_SetFunctionAll(eGPIO_H, 0x66000000, 0x00000006);	// Set MMC1DATA[6:4] pins as CF ADDR[2:0]
				GPIO_SetFunctionAll(eGPIO_L, 0x66666000, 0x00055555);	// A: Set XhiADDR[3:7] pins as CF Config pins(INTata, RESETata, INPACKata, REGata, CData), Set XhiADDR[12:8] as CF control pins(IORDY, IOWR, IORD, CE[1], CE[0])
//				GPIO_SetFunctionAll(eGPIO_L, 0x00000000, 0x00055555);	// Set XhiADDR[12:8] as CF control pins(IORDY, IOWR, IORD, CE[1], CE[0])
#endif
#if 0	// for GPIO test :A1, A2, D1,D2,D3,C2
				GPIO_SetFunctionAll(eGPIO_K, 0x55555555, 0x00000000);	// D: Set XhiDATA[7:0] pins as CF Data[7:0] 
				GPIO_SetFunctionAll(eGPIO_M, 0x00066666, 0x00000000);	// D2: Set Xhi control pins as CF Data[15:10] 				
				GPIO_SetFunctionAll(eGPIO_A, 0x00005500, 0x00000000);	// A2: Set XuCTSn0, XuRTSn0 pins as CF ADDR[2:0]
				GPIO_SetFunctionAll(eGPIO_L, 0x00060000, 0x02255555);	// A1: Set XhiADDR2 pin as CF ADDR2, D3: Set XhiDATA[17:16] pins as CF Data[9:8] , C: Set Xhi control pins as CF control pins(IORDY, IOWR, IORD, CE[1], CE[0])				
#endif
#if 0	// for GPIO test :A1, A3,D1,C1
				GPIO_SetFunctionAll(eGPIO_K, 0x55555555, 0x55555555);	// D: Set XhiDATA[15:0] pins as CF Data[15:0] 
				GPIO_SetFunctionAll(eGPIO_L, 0x00000600, 0x00000000);	// A1: Set XhiADDR2 pins as CF ADDR2
				GPIO_SetFunctionAll(eGPIO_A, 0x55000000, 0x00000000);	// A3: Set XuCTSn1, XuRTSn1 pins as CF ADDR[2:0]
				GPIO_SetFunctionAll(eGPIO_M, 0x00066666, 0x00000000);	// C: Set Xhi control pins as CF control pins(IORDY, IOWR, IORD, CE[1], CE[0])
#endif
#if 0	// for GPIO test :A5,D1,C1
				GPIO_SetFunctionAll(eGPIO_K, 0x55555555, 0x55555555);	// D: Set XhiDATA[15:0] pins as CF Data[15:0] 
				GPIO_SetFunctionAll(eGPIO_C, 0x00000555, 0x00000000);	// A5: Set XspiMISO0, XspiCLK0, XspiMOSI0 pins as CF ADDR[2:0]
				GPIO_SetFunctionAll(eGPIO_M, 0x00066666, 0x00000000);	// C: Set Xhi control pins as CF control pins(IORDY, IOWR, IORD, CE[1], CE[0])
#endif
#if 0	// for GPIO test :A6,D1,C1
				GPIO_SetFunctionAll(eGPIO_K, 0x55555555, 0x55555555);	// D: Set XhiDATA[15:0] pins as CF Data[15:0] 
				GPIO_SetFunctionAll(eGPIO_D, 0x00000ddd, 0x00000000);	// A6: Set XpcmDCLK0, XpcmEXTCLK0, XpcmFSYNC0 pins as CF ADDR[2:0]
				GPIO_SetFunctionAll(eGPIO_M, 0x00066666, 0x00000000);	// C: Set Xhi control pins as CF control pins(IORDY, IOWR, IORD, CE[1], CE[0])
#endif
#if 0	// for GPIO test :A7,D1,C1
				GPIO_SetFunctionAll(eGPIO_K, 0x55555555, 0x55555555);	// D: Set XhiDATA[15:0] pins as CF Data[15:0] 
				GPIO_SetFunctionAll(eGPIO_N, 0x003f0000, 0x00000000);	// A7: Set XEINT[10:8] pins as CF ADDR[2:0]
				GPIO_SetFunctionAll(eGPIO_M, 0x00066666, 0x00000000);	// C: Set Xhi control pins as CF control pins(IORDY, IOWR, IORD, CE[1], CE[0])
#endif
#if 0	// for GPIO test : A8,D1,C1
				GPIO_SetFunctionAll(eGPIO_K, 0x55555555, 0x55555555);	// D: Set XhiDATA[15:0] pins as CF Data[15:0] 
				GPIO_SetFunctionAll(eGPIO_G, 0x00000555, 0x00000000);	// A8: Set XmmcCLK0, XmmcCMD0, XmmcDAT0 pins as CF ADDR[2:0]
				GPIO_SetFunctionAll(eGPIO_M, 0x00066666, 0x00000000);	// C: Set Xhi control pins as CF control pins(IORDY, IOWR, IORD, CE[1], CE[0])
#endif
#if 0	// for GPIO test :A9,D1,C1
				GPIO_SetFunctionAll(eGPIO_K, 0x55555555, 0x55555555);	// D: Set XhiDATA[15:0] pins as CF Data[15:0] 
				GPIO_SetFunctionAll(eGPIO_H, 0x00000555, 0x00000000);	// A9: Set XmmcCLK1, XmmcCMD1, XmmcDAT1 pins as CF ADDR[2:0]
				GPIO_SetFunctionAll(eGPIO_M, 0x00066666, 0x00000000);	// C: Set Xhi control pins as CF control pins(IORDY, IOWR, IORD, CE[1], CE[0])
#endif
				OpenConsole();		// This line must be here because Uart ports(GPIO_A) are changed after setting GPIO_K.
		 	}

			// GPIO_M[5] (XhiINTR) should be output high in order to connect nOE_CF signal to VDD_SMEM(high).
			// (refer to the unit TS5A3159_SPDT(U613) in SMDK6400 Rev0.1's schematic page 7)
			GPIO_SetFunctionEach(eGPIO_M, eGPIO_5, 1);	// set XhINTR/CF Data Dir./GPM5 as output
			GPIO_SetDataEach(eGPIO_M, eGPIO_5 , 1); 		//GPM[5] -> High
			
			//--- CF controller - PC card mode setting ---//
			// Output pad disable, Card power off, PC card mode
			CF_SetMUXReg(eCF_MUX_OUTPUT_DISABLE, eCF_MUX_CARDPWR_OFF, eCF_MUX_MODE_IDE);
//			DelayfrTimer( milli, 100);	// wait for 100ms, be positively necessary
			// Output pad enable, Card power off, PC card mode
			CF_SetMUXReg(eCF_MUX_OUTPUT_ENABLE, eCF_MUX_CARDPWR_OFF, eCF_MUX_MODE_IDE);
//			DelayfrTimer( milli, 100);	// wait for 100ms, be positively necessary
			// Card Power on (PC Card mode)
			CF_SetMUXReg(eCF_MUX_OUTPUT_ENABLE, eCF_MUX_CARDPWR_ON, eCF_MUX_MODE_IDE);
			DelayfrTimer( milli, 100);	// wait for 100ms, be positively necessary
			
			// Card configuration
			ATA_SetTimingParams(ucCon, eATA_MODE_PIO, 0x1C238);
			ATA_SetTimingParams(ucCon, eATA_MODE_UDMA, 0x20B1362);
			ATA_SetEndian(ucCon, eATA_ENDIAN_LITTLE);	
			ATA_SetEnable(ucCon, ENABLE);
			DelayfrTimer( milli, 100);	// wait for 200ms, be positively necessary

#if 0	// for pull-down-dedicated nCD pins
			GPIO_SetFunctionEach(eGPIO_P, eGPIO_14, 1);	// set CData/EINT as output
			GPIO_SetDataEach(eGPIO_P, eGPIO_14 , 1); 		//GPN[8] -> High
			DelayfrTimer(milli,500);						//Delay about 10ms

			GPIO_SetFunctionEach(eGPIO_P, eGPIO_14, 2);	// set CData/EINT as CData
			GPIO_SetDataEach(eGPIO_P, eGPIO_14 , 1); 		//GPN[8] -> High
			DelayfrTimer(milli,500);						//Delay about 10ms
#endif
#if 1	// for CF card boot up signal
			GPIO_SetFunctionEach(eGPIO_N, eGPIO_8, 1);	// set XEINT8/ADDR_CF0/GPN8 as output
			GPIO_SetDataEach(eGPIO_N, eGPIO_8 , 1); 		//GPN[8] -> High
			DelayfrTimer(milli,1000);						//Delay about 10ms
			GPIO_SetDataEach(eGPIO_N, eGPIO_8 , 0); 		//GPN[8] -> Low	
#endif			
			break;
		default :
			break;
	}

}

void ATA_InitBufferControl(void) // added by junon for second UDMA test b'd 060902
{
#if 0
#ifdef __EVT1
	rGPACDH = 0x1aa8a; // GPA10 RDATA_OEN setting
#else
	rGPBCON = rGPBCON & ~((3<<8)|(3)) | (1<<8)|(1); // GPB0,4 output setting (TOUT0, TCLK - TP21,20)
	rGPBCON = rGPBCON & ~((3<<12)|(3<<10)) | (1<<12)|(1<<10); // GPB5,6 output setting (nXBACK, nXBREQ)
	rGPBDAT = rGPBDAT & ~((7<<4)|(1)) | (1<<6); // GPB6->high, GPB0,4,5->low
#endif	
	UART_Printf("IBC - rGPBCON = 0x%x \n",rGPBCON);
#endif
}

//extern u8 g_ucATAConNum;
void 	ATA_ChangeBufferControl(eATA_MODE_6400 mode) // only for SMDK b'd 060902 using additianal logic
{
#if 0
	UART_Printf("MODE5:%d, mode:%d\n", g_oaATAInform[g_ucATAConNum].eAtaMode, mode);
	if (mode == eATA_MODE_UDMA)
		rGPBDAT = rGPBDAT | (1<<4)|(1); // GPB0->high,GPB4->high => UDMA mode 
	else // PIO
		rGPBDAT = rGPBDAT & ~(1<<4) | (1); // GPB0->high,GPB4->low => ATA PIO mode	  	
	UART_Printf("BC - rGPBDAT = 0x%x \n",(u32)rGPBDAT);
#endif	
}

void ATA_SetTimingParams(u8 ucCon, eATA_MODE_6400 eMode, u32 uValue)
{
	switch(eMode)
	{
		case eATA_MODE_PIOCPU:
		case eATA_MODE_PIODMA:
			ATA_SetRegValue(ucCon, eATA_PIO_TIME, uValue);
			break;
		case eATA_MODE_UDMA :
			ATA_SetRegValue(ucCon, eATA_UDMA_TIME, uValue);
			break;			
	}
}
/*---------------------------------- Functions for Implement ATA Test functions ---------------------------------*/
//////////
// Function Name : ATA_TestResetAll
// Function Description : This function implements reset function of ATA device.
// Input : 	ucCon - ATA Controller Number
// Output : 	NONE
// Version : v0.1
void ATA_TestResetAll(u8 ucCon)
{
	// TDelay timer setting
	DelayfrTimer( milli, 2);	// wait for 2ms
	DelayfrTimer( micro, 25);	// wait for 25us

	ATA_SetSwRst( ucCon, RESET);	// CF controller reset
	DelayfrTimer( micro, 5);	// wait for 5us

	ATA_SetSwRst( ucCon, NORESET);
	DelayfrTimer( milli, 2);	// wait for 2ms

	ATA_SetRst( ucCon, RESET);		// ata device reset.	
	DelayfrTimer( micro, 5);	// wait for 5us

	ATA_SetRst( ucCon, NORESET);		// ata device no reset.	
	DelayfrTimer( milli, 250);	// wait for 200ms

}

void ATA_ChangeATAMode(u8 ucCon)
{
	// CF controller - True IDE mode setting
	// Output pad disable, Card power off, PC card mode
	CF_SetMUXReg(eCF_MUX_OUTPUT_DISABLE, eCF_MUX_CARDPWR_OFF, eCF_MUX_MODE_IDE);
	DelayfrTimer( milli, 1);	// wait for 1ms
	// Output pad enable, Card power off, PC card mode
	CF_SetMUXReg(eCF_MUX_OUTPUT_ENABLE, eCF_MUX_CARDPWR_OFF, eCF_MUX_MODE_IDE);
	DelayfrTimer( milli, 1);	// wait for 1ms
	// Card Power on (PC Card mode)
	CF_SetMUXReg(eCF_MUX_OUTPUT_ENABLE, eCF_MUX_CARDPWR_ON, eCF_MUX_MODE_IDE);
	DelayfrTimer( milli, 400);	// wait for 400ms
		
}

//////////
// Function Name : ATA_IdentifyDevice
// Function Description : This function identifies ATA device.
// Input : 	ucCon - ATA Controller Number
// Output : 	NONE
// Version : v0.1
void ATA_IdentifyDevice(u8 ucCon,eATA_SLOT_SEL eSlotsel)
{
	u16 usReadBuffer[ATA_SECTORSIZE/2];
	u8	ucTempBuffer;
	volatile u8*	ucTempString;
	unsigned int tBuf[4];
	u32 uLoopCnt = 0;

	for (uLoopCnt=0;uLoopCnt<ATA_SECTORSIZE/2;uLoopCnt++)
		usReadBuffer[uLoopCnt] = 1;

	ATA_SetEndian(ucCon, eATA_ENDIAN_BIG);		// Set Big endian (must be)

	ATA_SetIRQMask(ucCon, eATA_IRQ_ALL);		// Mask all interrupt sources

	if(eSlotsel == eATA_SLAVE)
		{
		ATA_SetTaskFileRegValue(ucCon, eCF_TASKFILE_DEVICE, 0x50);		// Slave, Slot1
		UART_Printf("\n###  Slave slot identification ###\n");
		}
	else
		{
		ATA_SetTaskFileRegValue(ucCon, eCF_TASKFILE_DEVICE, 0x40);		// Master, Slot0
		UART_Printf("\n###  Master slot identification ###\n");
		}

	ATA_SetTaskFileRegValue(ucCon, eCF_TASKFILE_COMMAND, ATA_CMD_IDENTIFYDEVICE);
	
	ATA_WaitForDeviceReady(ucCon);

	UART_Printf("\n");
	for(uLoopCnt=0; uLoopCnt<ATA_SECTORSIZE/2; uLoopCnt++) {
		usReadBuffer[uLoopCnt] = ATA_GetDataFromDevice(ucCon);
//		UART_Printf("(DATA%d:0x%04X)", uLoopCnt, usReadBuffer[uLoopCnt]);
	}
	UART_Printf("\n");

	ATA_WaitForDeviceReady(ucCon);

	//
	//verify identify data~~~~~~~~~~~~~~~~~~~~~~~~~~
	//
	ucTempString = (u8 *)&usReadBuffer[10];
	UART_Printf("\nSerial Number :");
	for(uLoopCnt=0;uLoopCnt<20;uLoopCnt++) UART_Printf("%c",*(ucTempString+uLoopCnt));

	ucTempString = (u8 *)&usReadBuffer[27];
	UART_Printf("\nModel Number :");
	for(uLoopCnt=0;uLoopCnt<40;uLoopCnt++) UART_Printf("%c",*(ucTempString+uLoopCnt));

	tBuf[0] = (u8)(usReadBuffer[61]&0xff);
	tBuf[1] = (u8)((usReadBuffer[61]&0xff00)>>8);
	tBuf[2] = (u8)(usReadBuffer[60]&0xff);
	tBuf[3] = (u8)((usReadBuffer[60]&0xff00)>>8);
	g_oaATAInform[ucCon].uMaxSectors = (u32)((tBuf[0]<<24)|(tBuf[1]<<16)|(tBuf[2]<<8)|tBuf[3]);
	UART_Printf("\nMax Sectors : %d\n",g_oaATAInform[ucCon].uMaxSectors);

	// Caution: readBuffer[x] - Big Endian, so upper byte means LSB..
	g_oaATAInform[ucCon].uMaxMultiple = (usReadBuffer[47]>>8)&0xFF;
	UART_Printf("\nMax Multiple : %02X\n",g_oaATAInform[ucCon].uMaxMultiple);
	if (usReadBuffer[59]&0x1) { //multiple sector setting is valid
		g_oaATAInform[ucCon].uCurrentMultiple = (usReadBuffer[59]>>8)&0xFF;
		UART_Printf("Current Multiple : %03X\n",g_oaATAInform[ucCon].uCurrentMultiple);
	}
	
	if ((usReadBuffer[64]>>8)&0x3 == 1) g_oaATAInform[ucCon].eMaxPioMode = eATA_PIO3;
	else if ((usReadBuffer[64]>>8)&0x3 == 3) g_oaATAInform[ucCon].eMaxPioMode = eATA_PIO4;
	else g_oaATAInform[ucCon].eMaxPioMode = eATA_PIO2;
	UART_Printf("Max PIO Mode : %d\n",g_oaATAInform[ucCon].eMaxPioMode);

	g_oaATAInform[ucCon].eMaxUdmaMode = eATA_UDMA0;
	ucTempBuffer = usReadBuffer[88]>>8;
	for(uLoopCnt=5;uLoopCnt>=1;uLoopCnt--) {
		if(ucTempBuffer&(0x01<<(uLoopCnt-1))) {
			g_oaATAInform[ucCon].eMaxUdmaMode = (eATA_UDMAMODE) (uLoopCnt-1);
			break; 
		}
	}

	g_oaATAInform[ucCon].eCurrentUdmaMode =eATA_UDMA0;
	ucTempBuffer = usReadBuffer[88]&0x00ff;
	for(uLoopCnt=0;uLoopCnt<5;uLoopCnt++) {
		if(ucTempBuffer&(0x01<<uLoopCnt)) {
			g_oaATAInform[ucCon].eCurrentUdmaMode = (eATA_UDMAMODE)uLoopCnt;
			break; ///
		}
	}

	UART_Printf("Max UDMA Mode : %d\n", g_oaATAInform[ucCon].eMaxUdmaMode);
	UART_Printf("Current UDMA Mode : %d\n", g_oaATAInform[ucCon].eCurrentUdmaMode);
	//
	//verify identify data~~~~~~~~~~~~~~~~~~~~~~~END
	//

	ATA_SetEndian(ucCon, eATA_ENDIAN_LITTLE);	
}



//////////
// Function Name : ATA_IdentifyDevice
// Function Description : This function identifies ATA device.
// Input : 	ucCon - ATA Controller Number
// Output : 	NONE
// Version : v0.1
void ATA_IdentifyDeviceSlotSel(u8 ucCon,eATA_SLOT_SEL eSlotsel)
{
	u16 usReadBuffer[ATA_SECTORSIZE/2];
	u8	ucTempBuffer;
	volatile u8*	ucTempString;
	unsigned int tBuf[4];
	u32 uLoopCnt = 0;

	for (uLoopCnt=0;uLoopCnt<ATA_SECTORSIZE/2;uLoopCnt++)
		usReadBuffer[uLoopCnt] = 1;

	ATA_SetEndian(ucCon, eATA_ENDIAN_BIG);		// Set Big endian (must be)

	ATA_SetIRQMask(ucCon, eATA_IRQ_ALL);		// Mask all interrupt sources

	if(eSlotsel == eATA_SLAVE)
		{
		ATA_SetTaskFileRegValue(ucCon, eCF_TASKFILE_DEVICE, 0x50);		// Slave, Slot1
		UART_Printf("\n###  Slave slot identification ###\n");
		}
	else
		{
		ATA_SetTaskFileRegValue(ucCon, eCF_TASKFILE_DEVICE, 0x40);		// Master, Slot0
		UART_Printf("\n###  Master slot identification ###\n");
		}

	ATA_SetTaskFileRegValue(ucCon, eCF_TASKFILE_COMMAND, ATA_CMD_IDENTIFYDEVICE);
	
	ATA_WaitForDeviceReady(ucCon);

	UART_Printf("\n");
	for(uLoopCnt=0; uLoopCnt<ATA_SECTORSIZE/2; uLoopCnt++) {
		usReadBuffer[uLoopCnt] = ATA_GetDataFromDevice(ucCon);
//		UART_Printf("(DATA%d:0x%04X)", uLoopCnt, usReadBuffer[uLoopCnt]);
	}
	UART_Printf("\n");

	ATA_WaitForDeviceReady(ucCon);

	//
	//verify identify data~~~~~~~~~~~~~~~~~~~~~~~~~~
	//
	ucTempString = (u8 *)&usReadBuffer[10];
	UART_Printf("\nSerial Number :");
	for(uLoopCnt=0;uLoopCnt<20;uLoopCnt++) UART_Printf("%c",*(ucTempString+uLoopCnt));

	ucTempString = (u8 *)&usReadBuffer[27];
	UART_Printf("\nModel Number :");
	for(uLoopCnt=0;uLoopCnt<40;uLoopCnt++) UART_Printf("%c",*(ucTempString+uLoopCnt));

	tBuf[0] = (u8)(usReadBuffer[61]&0xff);
	tBuf[1] = (u8)((usReadBuffer[61]&0xff00)>>8);
	tBuf[2] = (u8)(usReadBuffer[60]&0xff);
	tBuf[3] = (u8)((usReadBuffer[60]&0xff00)>>8);

	if(eSlotsel == eATA_SLAVE)
		{
		g_oaATAInformSlave[ucCon].uMaxSectors = (u32)((tBuf[0]<<24)|(tBuf[1]<<16)|(tBuf[2]<<8)|tBuf[3]);
		UART_Printf("\nMax Sectors : %d\n",g_oaATAInformSlave[ucCon].uMaxSectors);

		// Caution: readBuffer[x] - Big Endian, so upper byte means LSB..
		g_oaATAInformSlave[ucCon].uMaxMultiple = (usReadBuffer[47]>>8)&0xFF;
		UART_Printf("\nMax Multiple : %02X\n",g_oaATAInformSlave[ucCon].uMaxMultiple);
		if (usReadBuffer[59]&0x1) { //multiple sector setting is valid
			g_oaATAInformSlave[ucCon].uCurrentMultiple = (usReadBuffer[59]>>8)&0xFF;
			UART_Printf("Current Multiple : %03X\n",g_oaATAInformSlave[ucCon].uCurrentMultiple);
		}
		
		if ((usReadBuffer[64]>>8)&0x3 == 1) g_oaATAInformSlave[ucCon].eMaxPioMode = eATA_PIO3;
		else if ((usReadBuffer[64]>>8)&0x3 == 3) g_oaATAInformSlave[ucCon].eMaxPioMode = eATA_PIO4;
		else g_oaATAInformSlave[ucCon].eMaxPioMode = eATA_PIO2;
		UART_Printf("Max PIO Mode : %d\n",g_oaATAInformSlave[ucCon].eMaxPioMode);

		g_oaATAInformSlave[ucCon].eMaxUdmaMode = eATA_UDMA0;
		ucTempBuffer = usReadBuffer[88]>>8;
		for(uLoopCnt=5;uLoopCnt>=1;uLoopCnt--) {
			if(ucTempBuffer&(0x01<<(uLoopCnt-1))) {
				g_oaATAInformSlave[ucCon].eMaxUdmaMode = (eATA_UDMAMODE) (uLoopCnt-1);
				break; 
			}
		}

		g_oaATAInformSlave[ucCon].eCurrentUdmaMode =eATA_UDMA0;
		ucTempBuffer = usReadBuffer[88]&0x00ff;
		for(uLoopCnt=0;uLoopCnt<5;uLoopCnt++) {
			if(ucTempBuffer&(0x01<<uLoopCnt)) {
				g_oaATAInformSlave[ucCon].eCurrentUdmaMode = (eATA_UDMAMODE)uLoopCnt;
				break; ///
			}
		}

		UART_Printf("Max UDMA Mode : %d\n", g_oaATAInformSlave[ucCon].eMaxUdmaMode);
		UART_Printf("Current UDMA Mode : %d\n", g_oaATAInformSlave[ucCon].eCurrentUdmaMode);
		//
		//verify identify data~~~~~~~~~~~~~~~~~~~~~~~END
		//
		}


	else
	{
		g_oaATAInformMaster[ucCon].uMaxSectors = (u32)((tBuf[0]<<24)|(tBuf[1]<<16)|(tBuf[2]<<8)|tBuf[3]);
		UART_Printf("\nMax Sectors : %d\n",g_oaATAInformMaster[ucCon].uMaxSectors);

		// Caution: readBuffer[x] - Big Endian, so upper byte means LSB..
		g_oaATAInformMaster[ucCon].uMaxMultiple = (usReadBuffer[47]>>8)&0xFF;
		UART_Printf("\nMax Multiple : %02X\n",g_oaATAInformMaster[ucCon].uMaxMultiple);
		if (usReadBuffer[59]&0x1) { //multiple sector setting is valid
			g_oaATAInformMaster[ucCon].uCurrentMultiple = (usReadBuffer[59]>>8)&0xFF;
			UART_Printf("Current Multiple : %03X\n",g_oaATAInformMaster[ucCon].uCurrentMultiple);
		}
		
		if ((usReadBuffer[64]>>8)&0x3 == 1) g_oaATAInformMaster[ucCon].eMaxPioMode = eATA_PIO3;
		else if ((usReadBuffer[64]>>8)&0x3 == 3) g_oaATAInformMaster[ucCon].eMaxPioMode = eATA_PIO4;
		else g_oaATAInformMaster[ucCon].eMaxPioMode = eATA_PIO2;
		UART_Printf("Max PIO Mode : %d\n",g_oaATAInformMaster[ucCon].eMaxPioMode);

		g_oaATAInformMaster[ucCon].eMaxUdmaMode = eATA_UDMA0;
		ucTempBuffer = usReadBuffer[88]>>8;
		for(uLoopCnt=5;uLoopCnt>=1;uLoopCnt--) {
			if(ucTempBuffer&(0x01<<(uLoopCnt-1))) {
				g_oaATAInformMaster[ucCon].eMaxUdmaMode = (eATA_UDMAMODE) (uLoopCnt-1);
				break; 
			}
		}

		g_oaATAInformMaster[ucCon].eCurrentUdmaMode =eATA_UDMA0;
		ucTempBuffer = usReadBuffer[88]&0x00ff;
		for(uLoopCnt=0;uLoopCnt<5;uLoopCnt++) {
			if(ucTempBuffer&(0x01<<uLoopCnt)) {
				g_oaATAInformMaster[ucCon].eCurrentUdmaMode = (eATA_UDMAMODE)uLoopCnt;
				break; ///
			}
		}

		UART_Printf("Max UDMA Mode : %d\n", g_oaATAInformMaster[ucCon].eMaxUdmaMode);
		UART_Printf("Current UDMA Mode : %d\n", g_oaATAInformMaster[ucCon].eCurrentUdmaMode);
		//
		//verify identify data~~~~~~~~~~~~~~~~~~~~~~~END
		//
	}


	
	ATA_SetEndian(ucCon, eATA_ENDIAN_LITTLE);	

}



//////////
// Function Name : ATA_WaitForDeviceReady
// Function Description : This function awaits ready state of ATA device.
// Input : 	ucCon - ATA Controller Number
// Output : 	NONE
// Version : v0.1
void ATA_WaitForDeviceReady(u8 ucCon)
{
	u8 ucReadData = 0;

	while(1)
	{
		ucReadData = ATA_GetDeviceReg(ucCon, eCF_TASKFILE_ALTANATE);
		ucReadData = ATA_GetDeviceReg(ucCon, eCF_TASKFILE_STATUS);
			
		if ( (ucReadData&eATA_XFR_STATUS_DEVICE_BUSY) == 0 )
			break;
	}	
}

//////////
// Function Name : ATA_WaitForDeviceReady
// Function Description : This function gets data from ATA device register.
// Input : 	ucCon - ATA Controller Number
//			eTaskFileRegId - Id of TaskFileRegister
// Output : 	NONE
// Version : v0.1
u8 ATA_GetDeviceReg(u8 ucCon, eCF_TASKFILE_Id eTaskFileRegId)
{
	u8 ucTempRead = 0;
	
	ATA_WaitForHostReady(ucCon);
	ucTempRead = ATA_GetTaskFileRegValue(ucCon, eTaskFileRegId);
	ATA_WaitForHostReady(ucCon);
	ucTempRead = (u8)(ATA_GetRegValue(ucCon, eATA_PIO_RDATA)&0xff);

	return ucTempRead;
}

//////////
// Function Name : ATA_GetDataFromDevice
// Function Description : This function gets data from ATA device register.
// Input : 	ucCon - ATA Controller Number
// Output : 	NONE
// Version : v0.1
u16 ATA_GetDataFromDevice(u8 ucCon)
{
	u32 ucTempRead = 0;
	
	ATA_WaitForHostReady(ucCon);
	ucTempRead = ATA_GetTaskFileRegValue16(ucCon, eCF_TASKFILE_DATA);
	ATA_WaitForHostReady(ucCon);
	ucTempRead = ATA_GetRegValue(ucCon, eATA_PIO_RDATA);

	return (u16)(ucTempRead&0xffff);
}

//////////
// Function Name : ATA_WaitForDeviceReady
// Function Description : This function gets data from ATA device register.
// Input : 	ucCon - ATA Controller Number
//			eTaskFileRegId - Id of TaskFileRegister
// Output : 	NONE
// Version : v0.1
void ATA_SetDataToDevice(u8 ucCon, u16 usData)
{
	ATA_WaitForHostReady(ucCon);
	ATA_SetTaskFileRegValue16(ucCon, eCF_TASKFILE_DATA, usData);
}

//////////
// Function Name : ATA_WaitForDeviceReady
// Function Description : This function sets up ATA mode as PIO.
// Input : 	ucCon - ATA Controller Number
//			ePioMode - PIO0/PIO1/PIO2/PIO3/PIO4
// Output : 	NONE
// Version : v0.1
//-------------------------------------------------------------
// PIO mode maximum transfer rate
// PIO0 : 3.3MB/s
// PIO1 : 5.2MB/s
// PIO2 : 8.3MB/s
// PIO3 : 11.1MB/s
// PIO4 : 16.7MB/s
//-------------------------------------------------------------

void ATA_SetPIOMode(u8 ucCon, eATA_PIOMODE ePioMode, eATA_SLOT_SEL eSlotSel)
{
	u32 uT1;
	u32 uT2;
	u32 uTeoc;
	u32 uLoopCnt;
	
	u32 uPioTime[5];
	u32 uPioT1[5] = {70,50,30,30,30};    // min = {70,50,30,30,25};  edited by junon 060827
	u32 uPioT2[5] = {290,290,290,80,70}; // min = {290,290,290,80,70};
	u32 uPioTeoc[5] = {20,20,10,10,10};  // min = {20,15,10,10,10};

	u32 uCycleTime = (unsigned int)(1000000000/g_HCLK);

	for (uLoopCnt=0; uLoopCnt<5; uLoopCnt++)
	{
		uT1   = (uPioT1[uLoopCnt]  /uCycleTime)&0xff;  // edited by junon 060827
		uT2   = (uPioT2[uLoopCnt]  /uCycleTime)&0xff;
		uTeoc = (uPioTeoc[uLoopCnt]/uCycleTime)&0x0f;
		uPioTime[uLoopCnt] = (uTeoc<<12)|(uT2<<4)|uT1;
		UART_Printf("PIO%dTIME = %x\n", uLoopCnt, uPioTime[uLoopCnt]);
	}

	ATA_SetIRQ(ucCon, eATA_IRQ_ALL);
	ATA_SetIRQMask(ucCon, (eATA_IRQ_SRC)0x0);			// unmask all

	ATA_SetTaskFileRegValue(ucCon, eCF_TASKFILE_CONTROL,	0x00);
	ATA_SetTaskFileRegValue(ucCon, eCF_TASKFILE_FEATURE,	0x03);	//set transfer mode based on value in Sector Count register
	ATA_SetTaskFileRegValue(ucCon, eCF_TASKFILE_SECTOR,	0x08|(ePioMode&0x7));	// PIO flow control transfer mode
	ATA_SetTaskFileRegValue(ucCon, eCF_TASKFILE_LOWLBA,	0x00);
	ATA_SetTaskFileRegValue(ucCon, eCF_TASKFILE_MIDLBA,		0x00);
	ATA_SetTaskFileRegValue(ucCon, eCF_TASKFILE_HIGHLBA,	0x00);


	if(eSlotSel == eATA_SLAVE)
	ATA_SetTaskFileRegValue(ucCon, eCF_TASKFILE_DEVICE,		0x50);
	else
	ATA_SetTaskFileRegValue(ucCon, eCF_TASKFILE_DEVICE,		0x40);

	ATA_SetTaskFileRegValue(ucCon, eCF_TASKFILE_COMMAND,	ATA_CMD_SETFEATURES);	//set feature command

	ATA_WaitForDeviceReady(ucCon);

	switch(ePioMode) { 
		case eATA_PIO1:
			ATA_SetTimingParams(ucCon, eATA_MODE_PIO, uPioTime[1]);
			ATA_SetIORDY(ucCon, DISABLE);
			break;
		case eATA_PIO2:
			ATA_SetTimingParams(ucCon, eATA_MODE_PIO, uPioTime[2]);
			ATA_SetIORDY(ucCon, DISABLE);
			break;
		case eATA_PIO3:
			ATA_SetTimingParams(ucCon, eATA_MODE_PIO, uPioTime[3]);
			ATA_SetIORDY(ucCon, ENABLE);
			break;
		case eATA_PIO4:
			ATA_SetTimingParams(ucCon, eATA_MODE_PIO, uPioTime[4]);
			ATA_SetIORDY(ucCon, ENABLE);
			break;
		default:
			ATA_SetTimingParams(ucCon, eATA_MODE_PIO, uPioTime[0]);
			ATA_SetIORDY(ucCon, DISABLE);
			break;
		}
	g_oaATAInform[ucCon].eCurrentPioMode = ePioMode;	
}


//////////
// Function Name : ATA_SetUdmaMode
// Function Description : This function sets up ATA mode as UDMA
// Input : 	ucCon - ATA Controller Number
//			eUdmaMode - UDMA0/UDMA1/UDMA2/UDMA3/UDMA4
//			eSlotSel - eATA_MASTER / eATA_SLAVE
// Output : 	NONE
// Version : v0.1
//-------------------------------------------------------------
// UDMA mode maximum transfer rate
// UDMA0 : 16.7MB/s
// UDMA1 : 25.0MB/s
// UDMA2 : 33.3MB/s
// UDMA3 : 44.4MB/s
// UDMA4 : 66.7MB/s
// UDMA5 : 100 MB/s 
//-------------------------------------------------------------
void ATA_SetUdmaMode(u8 ucCon, eATA_UDMAMODE eUdmaMode, eATA_SLOT_SEL eSlotSel)
{
	u32 uTdvh1	= 0;
	u32 uTdvs	= 0;
	u32 uTrp		= 0;
	u32 uTss		= 0;
	u32 uTackenv	= 0;
	u32 uLoopCnt	= 0;
#if 0
	u32 uUdmaTime[5]	= {0};					// Tdvh+Tdvs min = {120,80,60,45,30}
	u32 uUdmaTdvh[5]	= {10,10,10,10,10}; 	// min = {6.2,6.2,6.2,6.2,6.2}; edited by junon 060827
	u32 uUdmaTdvs[5]	= {100,60,50,35,20}; 	// min = {70,48,31,20,6.7};
	u32 uUdmaTrp[5]		= {160,125,100,100,100}; // min = {160,125,100,100,100};
	u32 uUdmaTss[5]		= {50,50,50,50,50};		 // min = {50,50,50,50,50};
	u32 uUdmaTackenvMin[5]	= {20,20,20,20,20};
#else
	u32 uUdmaTime[5]	= {0};					// Tdvh+Tdvs min = {120,80,60,45,30}
	u32 uUdmaTdvh[5]	= {7,7,7,7,7}; 	// min = {6.2,6.2,6.2,6.2,6.2}; edited by junon 060827
	u32 uUdmaTdvs[5]	= {70,48,31,20,7}; 	// min = {70,48,31,20,6.7};
	u32 uUdmaTrp[5]		= {160,125,100,100,100}; // min = {160,125,100,100,100};
	u32 uUdmaTss[5]		= {50,50,50,50,50};		 // min = {50,50,50,50,50};
	u32 uUdmaTackenvMin[5]	= {20,20,20,20,20};
#endif
	u32 uCycleTime = (u32)(1000000000/g_HCLK);	

	for (uLoopCnt=0; uLoopCnt<5; uLoopCnt++)
	{
		uTdvh1	= (uUdmaTdvh[uLoopCnt] / uCycleTime)&0x0f;		// edited by junon 060827
		uTdvs	= (uUdmaTdvs[uLoopCnt] / uCycleTime)&0xff;
		uTrp	= (uUdmaTrp[uLoopCnt]  / uCycleTime)&0xff;
		uTss	= (uUdmaTss[uLoopCnt]  / uCycleTime)&0x0f;
		uTackenv= (uUdmaTackenvMin[uLoopCnt]/uCycleTime)&0x0f;
		uUdmaTime[uLoopCnt] = (uTdvh1<<24)|(uTdvs<<16)|(uTrp<<8)|(uTss<<4)|uTackenv;
		UART_Printf("UDMA%dTIME = %x\n", uLoopCnt, uUdmaTime[uLoopCnt]);
	}	

	ATA_SetIRQ(ucCon, eATA_IRQ_ALL);
	ATA_SetIRQMask(ucCon, (eATA_IRQ_SRC)0x0);			// unmask all

	ATA_SetTaskFileRegValue(ucCon, eCF_TASKFILE_CONTROL,	0x00);
	ATA_SetTaskFileRegValue(ucCon, eCF_TASKFILE_FEATURE,	0x03);	//set transfer mode based on value in Sector Count register
	ATA_SetTaskFileRegValue(ucCon, eCF_TASKFILE_SECTOR,	0x40|(eUdmaMode&0x7));	// PIO flow control transfer mode
	ATA_SetTaskFileRegValue(ucCon, eCF_TASKFILE_LOWLBA,	0x00);
	ATA_SetTaskFileRegValue(ucCon, eCF_TASKFILE_MIDLBA,		0x00);
	ATA_SetTaskFileRegValue(ucCon, eCF_TASKFILE_HIGHLBA,	0x00);

	if(eSlotSel == eATA_SLAVE)
	ATA_SetTaskFileRegValue(ucCon, eCF_TASKFILE_DEVICE,		0x50);
	else
	ATA_SetTaskFileRegValue(ucCon, eCF_TASKFILE_DEVICE,		0x40);		
	
	ATA_SetTaskFileRegValue(ucCon, eCF_TASKFILE_COMMAND,	ATA_CMD_SETFEATURES);	//set feature command

	ATA_WaitForDeviceReady(ucCon);

	switch(eUdmaMode) { 
		case eATA_UDMA0:
			ATA_SetTimingParams(ucCon, eATA_MODE_UDMA, uUdmaTime[0]);
			break;
		case eATA_UDMA1:
			ATA_SetTimingParams(ucCon, eATA_MODE_UDMA, uUdmaTime[1]);
			break;
		case eATA_UDMA2:
			ATA_SetTimingParams(ucCon, eATA_MODE_UDMA, uUdmaTime[2]);
			break;
		case eATA_UDMA3:
			ATA_SetTimingParams(ucCon, eATA_MODE_UDMA, uUdmaTime[3]);
			break;
		case eATA_UDMA4:
			ATA_SetTimingParams(ucCon, eATA_MODE_UDMA, uUdmaTime[4]);
			break;
		default:
			UART_Printf("Wrong UDMA mode in SetUdmaMode()\n");
			break;
		}
	g_oaATAInform[ucCon].eCurrentUdmaMode = eUdmaMode;	
}





//////////
// Function Name : ATA_WriteBlocks
// Function Description : This function writes block data to device.
// Input : 	ucCon - ATA Controller Number
//			uStBlock - LBA Block
//			uBlocks	- Block Count
//			uBufAddr - Address of Buffer
// Output : 	true/false
// Version : v0.1
u8 ATA_WriteBlocks(u8 ucCon, u32 uStBlock, u32 uBlocks, u32 uBufAddr, eATA_SLOT_SEL eSlotSel)
{
	eATA_MODE_6400	eAtaMode	= g_oaATAInform[ucCon].eAtaMode;
	u8			bStatus		= FALSE;
#if PERF_TEST_ATA
	u32			uElapsedTime = 0;
#endif

	UART_Printf("WriteBlocks() in %s mode...\n", ATA_GetModeName(eAtaMode));

#if PERF_TEST_ATA
	StartTimer(0);
#endif
	switch(eAtaMode)
	{
		case eATA_MODE_PIOCPU :
			bStatus = ATA_WriteSectors_PIO( ucCon, uStBlock, uBlocks, uBufAddr, (eATA_SLOT_SEL)eSlotSel);
			break;
		case eATA_MODE_PIODMA :
			bStatus = ATA_WriteSectors_PDMA( ucCon, uStBlock, uBlocks, uBufAddr, (eATA_SLOT_SEL)eSlotSel);
			break;
		case eATA_MODE_UDMA :
			bStatus = ATA_WriteSectors_UDMA( ucCon, uStBlock, uBlocks, uBufAddr, (eATA_SLOT_SEL)eSlotSel);
			break;
		default : 
			UART_Printf("Not supported mode in WriteBlocks()\n");
			break;
	}
#if PERF_TEST_ATA
	uElapsedTime = StopTimer(0);
	UART_Printf(" Writing time : %d us, Performance : %lf MByte/sec\n"
			,uElapsedTime,(float)(uBlocks*512./uElapsedTime));
#endif

	return bStatus;
}

//////////
// Function Name : ATA_ReadBlocks
// Function Description : This function reads block data from device to host.
// Input : 	ucCon - ATA Controller Number
//			uStBlock - LBA Block
//			uBlocks	- Block Count
//			uBufAddr - Address of Buffer
// Output : 	true/false
// Version : v0.1
u8 ATA_ReadBlocks(u8 ucCon, u32 uStBlock, u32 uBlocks, u32 uBufAddr, eATA_SLOT_SEL eSlotSel)
{
	eATA_MODE_6400	eAtaMode	= g_oaATAInform[ucCon].eAtaMode;
	u8			bStatus		= FALSE;
#if PERF_TEST_ATA
	u32			uElapsedTime = 0;
#endif
	UART_Printf("ReadBlocks() in %s mode...\n", ATA_GetModeName(eAtaMode));
#if PERF_TEST_ATA
	StartTimer(0);
#endif
	switch(eAtaMode)
	{
		case eATA_MODE_PIOCPU :
			bStatus = ATA_ReadSectors_PIO( ucCon, uStBlock, uBlocks, uBufAddr, (eATA_SLOT_SEL)eSlotSel);
			break;
		case eATA_MODE_PIODMA :
			bStatus = ATA_ReadSectors_PDMA( ucCon, uStBlock, uBlocks, uBufAddr, (eATA_SLOT_SEL)eSlotSel);
			break;
		case eATA_MODE_UDMA :
			bStatus = ATA_ReadSectors_UDMA( ucCon, uStBlock, uBlocks, uBufAddr,(eATA_SLOT_SEL)eSlotSel);
			break;
		default : 
			UART_Printf("Not supported mode in WriteBlocks()\n");
			break;
	}
#if PERF_TEST_ATA
	uElapsedTime = StopTimer(0);
	UART_Printf(" Reading time : %d us, Performance : %lf MByte/sec\n"
			,uElapsedTime,(float)(uBlocks*512./uElapsedTime));
#endif
	return bStatus;
}


//////////
// Function Name : ATA_WriteSectors_PIO
// Function Description : This function writes block data to device in PIO mode.
// Input : 	ucCon - ATA Controller Number
//			uLBA 		- LBA Block
//			uSectorCount	- Sector Count
//			uSrcAddress	- Address of Buffer
// Output : 	true/false
// Version : v0.1
u8 ATA_WriteSectors_PIO(u8 ucCon, u32 uLBA, u32 uSectorCount, u32 uSrcAddress, eATA_SLOT_SEL eSlotSel)
{
	u32 uCurrentCount;
	u32 uRemainderCount;
	u32 uCurrentLba;
	u32 uCurrentSrcAddr;
	u32 uRound;
	u16* usAtaHostAddr;
	u32 uLoopCnt;

#if PERF_TEST_PIO	
	u32			uElapsedTime = 0;
#endif

	uRemainderCount = uSectorCount;
	uCurrentLba = uLBA;
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
			
			uCurrentLba = uLBA + uRound*256;
			uCurrentSrcAddr = uSrcAddress + uRound*256*ATA_SECTORSIZE;
			usAtaHostAddr = (u16*)uCurrentSrcAddr;
			
			ATA_SetDevicePosition(ucCon, uCurrentLba, uCurrentCount,eSlotSel);
			ATA_SetTaskFileRegValue(ucCon, eCF_TASKFILE_COMMAND, ATA_CMD_WRITESECTOR);

			while(uCurrentCount-- ) 
				{	
				ATA_WaitForDeviceReady(ucCon);
#if PERF_TEST_PIO	
	StartTimer(2);
#endif
					for (uLoopCnt=0;uLoopCnt<ATA_SECTORSIZE/2;uLoopCnt++)
					{			
						ATA_SetDataToDevice(ucCon, *usAtaHostAddr);
						usAtaHostAddr++;
					}
#if PERF_TEST_PIO
	uElapsedTime += StopTimer(2);
#endif
				}	
			
			ATA_WaitForDeviceReady(ucCon);

			uRound++;
		}
#if PERF_TEST_PIO
	UART_Printf(" PIO Writing time : %d us, Performance : %lf MByte/sec\n"
			,uElapsedTime,(float)(uSectorCount*512./uElapsedTime));
#endif

	return TRUE;
}

//////////
// Function Name : ATA_ReadSectors_PIO
// Function Description : This function writes block data to device in PIO mode.
// Input : 	ucCon - ATA Controller Number
//			uLBA 		- LBA Block
//			uSectorCount	- Sector Count
//			uSrcAddress	- Address of Buffer
// Output : 	true/false
// Version : v0.1
u8 ATA_ReadSectors_PIO(u8 ucCon, u32 uLBA, u32 uSectorCount, u32 uDstAddress, eATA_SLOT_SEL eSlotSel)
{
	u32 uCurrentCount;
	u32 uRemainderCount;
	u32 uCurrentLba;
	u32 uCurrentDstAddr;
	u32 uRound;
	u16* usAtaHostAddr;
	u32 uLoopCnt;

#if PERF_TEST_PIO	
	u32			uElapsedTime = 0;
#endif

	uRemainderCount = uSectorCount;
	uCurrentLba = uLBA;
	uRound = 0;	
	
	while(uRemainderCount != 0) {
		if(uRemainderCount>256) {
			uCurrentCount = 256; //0 means 256
			uRemainderCount -= 256;
		} else {
			uCurrentCount = uRemainderCount;
			uRemainderCount = 0;
		}
		uCurrentLba = uLBA + uRound*256;
		uCurrentDstAddr = uDstAddress + uRound*256*ATA_SECTORSIZE;
		usAtaHostAddr = (u16*)uCurrentDstAddr;
		
		ATA_SetDevicePosition(ucCon, uCurrentLba, uCurrentCount,eSlotSel);
		ATA_SetTaskFileRegValue(ucCon, eCF_TASKFILE_COMMAND, ATA_CMD_READSECTOR);

		while(uCurrentCount-- ) {	
			ATA_WaitForDeviceReady(ucCon);
#if PERF_TEST_PIO	
	StartTimer(2);
#endif
			for (uLoopCnt=0;uLoopCnt<ATA_SECTORSIZE/2;uLoopCnt++) {			
				*usAtaHostAddr = ATA_GetDataFromDevice(ucCon);
				usAtaHostAddr++;
			}
#if PERF_TEST_PIO
	uElapsedTime += StopTimer(2);
#endif
		}		
		ATA_WaitForDeviceReady(ucCon);

		uRound++;
	}
#if PERF_TEST_PIO
	UART_Printf(" PIO Reading time : %d us, Performance : %lf MByte/sec\n"
			,uElapsedTime,(float)(uSectorCount*512./uElapsedTime));
#endif
	return TRUE;
}

//////////
// Function Name : ATA_ReadSectors_PDMA
// Function Description : This function writes sector data to device in UDMA mode.
// Input : 	ucCon - ATA Controller Number
//			uLBA 		- LBA Block
//			uSectorCount	- Sector Count
//			uSrcAddress	- Address of Buffer
// Output : 	true/false
// Version : v0.1
u8 ATA_ReadSectors_PDMA(u8 ucCon, u32 uLBA, u32 uSectorCount, u32 uDstAddress, eATA_SLOT_SEL eSlotSel)
{
	u32 uCurrentCount;
	u32 uRemainderCount;
	u32 uCurrentLba;
	u32 uCurrentDstAddr;
	u32 uRound;
	u32 uRoundSub;

#if PERF_TEST_UDMA	
	u32			uElapsedTime = 0;
#endif

	uRemainderCount = uSectorCount;
	uRound = 0;	
	uRoundSub = 0;		

	while(uRemainderCount != 0) 
	{
		if(uRemainderCount>256) {
			uCurrentCount = 256; //0 means 256
			uRemainderCount -= 256;
		} else {
			uCurrentCount = uRemainderCount;
			uRemainderCount = 0;
		}

		uCurrentLba = uLBA + uRound*256;
		
		ATA_SetDevicePosition(ucCon, uCurrentLba, uCurrentCount,eSlotSel);	
		ATA_SetTaskFileRegValue(ucCon, eCF_TASKFILE_COMMAND, ATA_CMD_READSECTOR);
		ATA_WaitForDeviceReady(ucCon);
		
		uRoundSub = 0;
		while(uCurrentCount != 0) 
		{
			
			uCurrentDstAddr = uDstAddress + (uRound*256+uRoundSub)*ATA_SECTORSIZE;

			/*Source Buffer 1 Setting*/
			ATA_SetTBufStart( ucCon, uCurrentDstAddr);
			ATA_SetTBufSize( ucCon, ATA_SECTORSIZE-1);
			ATA_SetXfrNum(ucCon, ATA_SECTORSIZE);

			ATA_WaitForDeviceReady(ucCon);
			
			ATA_SetConfig(ucCon, eATA_MODE_PIODMA, eATA_DMA_READ_DATA);
			
#if PERF_TEST_UDMA	
		StartTimer(1);
#endif

			/*ATA Transfer Command */
			ATA_SetTransferCommand(ucCon, eATA_XFR_CMD_START);
			ATA_WaitForTransferDone(ucCon);

			ATA_SetTransferCommand(ucCon, eATA_XFR_CMD_ABORT);
			
			ATA_SetConfig(ucCon, eATA_MODE_PIOCPU, eATA_DMA_READ_DATA);
#if PERF_TEST_UDMA
		uElapsedTime += StopTimer(1);
#endif
			uRoundSub++;
			uCurrentCount--;
		}
		uRound++;
		ATA_WaitForDeviceReady(ucCon);		
	}
	
	ATA_WaitForDeviceReady(ucCon);
#if PERF_TEST_UDMA
	UART_Printf(" PDMA Reading time : %d us, Performance : %lf MByte/sec\n"
			,uElapsedTime,(float)(uSectorCount*512./uElapsedTime));
#endif

	return TRUE;
}

//////////
// Function Name : ATA_WriteSectors_PDMA
// Function Description : This function writes sector data to device in PDMA mode.
// Input : 	ucCon - ATA Controller Number
//			uLBA 		- LBA Block
//			uSectorCount	- Sector Count
//			uSrcAddress	- Address of Buffer
// Output : 	true/false
// Version : v0.1
u8 ATA_WriteSectors_PDMA(u8 ucCon, u32 uLBA, u32 uSectorCount, u32 uSrcAddress, eATA_SLOT_SEL eSlotSel)
{
	u32 uCurrentCount;
	u32 uRemainderCount;
	u32 uCurrentLba;
	u32 uCurrentSrcAddr;
	u32 uRound;
	u32 uRoundSub;
	
#if PERF_TEST_UDMA	
	u32			uElapsedTime = 0;
#endif

	uRemainderCount = uSectorCount;
	uRound = 0;
	uRoundSub = 0;

	while(uRemainderCount != 0) 
	{
		if(uRemainderCount>256) {
			uCurrentCount = 256; //0 means 256
			uRemainderCount -= 256;
		} else {
			uCurrentCount = uRemainderCount;
			uRemainderCount = 0;
		}

		uCurrentLba = uLBA + uRound*256;
		
		ATA_SetDevicePosition(ucCon, uCurrentLba, uCurrentCount,eSlotSel);
		ATA_SetTaskFileRegValue(ucCon, eCF_TASKFILE_COMMAND, ATA_CMD_WRITESECTOR);
		ATA_WaitForDeviceReady(ucCon);

		uRoundSub = 0;
		while(uCurrentCount != 0) {
		
			uCurrentSrcAddr = uSrcAddress + (uRound*256+uRoundSub)*ATA_SECTORSIZE;

			/*Source Buffer 1 Setting*/
			ATA_SetSBufStart( ucCon, uCurrentSrcAddr);
			ATA_SetSBufSize( ucCon, ATA_SECTORSIZE-1);
			ATA_SetXfrNum(ucCon, ATA_SECTORSIZE);

			ATA_WaitForDeviceReady(ucCon);	
			
			ATA_SetConfig(ucCon, eATA_MODE_PIODMA, eATA_DMA_WRITE_DATA);		
			
#if PERF_TEST_UDMA	
		StartTimer(1);
#endif

			/*ATA Transfer Command */
			ATA_SetTransferCommand(ucCon, eATA_XFR_CMD_START);
			ATA_WaitForTransferDone(ucCon);

			ATA_SetTransferCommand(ucCon, eATA_XFR_CMD_ABORT);
#if PERF_TEST_UDMA
		uElapsedTime += StopTimer(1);
#endif
			ATA_SetConfig(ucCon, eATA_MODE_PIOCPU, eATA_DMA_WRITE_DATA);
			uRoundSub++;
			uCurrentCount--;
		}
		uRound++;
		ATA_WaitForDeviceReady(ucCon);		
	}
	ATA_WaitForDeviceReady(ucCon);
#if PERF_TEST_UDMA
	UART_Printf(" PDMA Writing time : %d us, Performance : %lf MByte/sec\n"
			,uElapsedTime,(float)(uSectorCount*512./uElapsedTime));
#endif

	return TRUE;
}

//////////
// Function Name : ATA_WriteSectors_UDMA
// Function Description : This function writes sector data to device in UDMA mode.
// Input : 	ucCon - ATA Controller Number
//			uLBA 		- LBA Block
//			uSectorCount	- Sector Count
//			uSrcAddress	- Address of Buffer
// Output : 	true/false
// Version : v0.1
u8 ATA_WriteSectors_UDMA(u8 ucCon, u32 uLBA, u32 uSectorCount, u32 uSrcAddress, eATA_SLOT_SEL eSlotsel)
{
	u32 uCurrentCount;
	u32 uRemainderCount;
	u32 uCurrentLba;
	u32 uCurrentSrcAddr;
	u32 uRound;

#if PERF_TEST_UDMA	
	u32			uElapsedTime = 0;
#endif

	uRemainderCount = uSectorCount;
	uRound = 0;	
	
	while(uRemainderCount != 0) {
		if(uRemainderCount>256) {
			uCurrentCount = 256; //0 means 256
			uRemainderCount -= 256;
		} else {
			uCurrentCount = uRemainderCount;
			uRemainderCount = 0;
		}
		uCurrentLba = uLBA + uRound*256;
		uCurrentSrcAddr = uSrcAddress + uRound*256*ATA_SECTORSIZE;

		/*Source Buffer 1 Setting*/
		ATA_SetSBufStart( ucCon, uCurrentSrcAddr);
		ATA_SetSBufSize( ucCon, uCurrentCount*ATA_SECTORSIZE);
		ATA_SetXfrNum(ucCon, uCurrentCount*ATA_SECTORSIZE);
		
		ATA_SetDevicePosition(ucCon, uCurrentLba, uCurrentCount, eSlotsel);
		ATA_SetTaskFileRegValue(ucCon, eCF_TASKFILE_COMMAND, ATA_CMD_WRITEDMA);

		ATA_WaitForDeviceReady(ucCon);
		ATA_SetConfig(ucCon, eATA_MODE_UDMA, eATA_DMA_WRITE_DATA);

#if PERF_TEST_UDMA	
	StartTimer(1);
#endif

		/*ATA Transfer Command */
		ATA_SetTransferCommand(ucCon, eATA_XFR_CMD_START);
		ATA_WaitForTransferDone(ucCon);

		ATA_SetTransferCommand(ucCon, eATA_XFR_CMD_ABORT);
#if PERF_TEST_UDMA
	uElapsedTime += StopTimer(1);
#endif

		ATA_SetConfig(ucCon, eATA_MODE_PIOCPU, eATA_DMA_WRITE_DATA);		//?
		ATA_WaitForDeviceReady(ucCon);

		ATA_SetUdmaAutoMode(ucCon, DISABLE);

		uRound++;
	}

#if PERF_TEST_UDMA
	UART_Printf(" UDMA Writing time : %d us, Performance : %lf MByte/sec\n"
			,uElapsedTime,(float)(uSectorCount*512./uElapsedTime));
#endif

	return TRUE;
}

//////////
// Function Name : ATA_ReadSectors_UDMA
// Function Description : This function writes sector data to device in UDMA mode.
// Input : 	ucCon - ATA Controller Number
//			uLBA 		- LBA Block
//			uSectorCount	- Sector Count
//			uSrcAddress	- Address of Buffer
// Output : 	true/false
// Version : v0.1
u8 ATA_ReadSectors_UDMA(u8 ucCon, u32 uLBA, u32 uSectorCount, u32 uDstAddress, eATA_SLOT_SEL eSlotsel)
{
	u32 uCurrentCount;
	u32 uRemainderCount;
	u32 uCurrentLba;
	u32 uCurrentDstAddr;
	u32 uRound;

#if PERF_TEST_UDMA	
	u32			uElapsedTime = 0;
#endif

	uRemainderCount = uSectorCount;
	uRound = 0;	
	
	while(uRemainderCount != 0) {
		if(uRemainderCount>256) {
			uCurrentCount = 256; //0 means 256
			uRemainderCount -= 256;
		} else {
			uCurrentCount = uRemainderCount;
			uRemainderCount = 0;
		}
		uCurrentLba = uLBA + uRound*256;
		uCurrentDstAddr = uDstAddress + uRound*256*ATA_SECTORSIZE;

		/*Source Buffer 1 Setting*/
		ATA_SetTBufStart( ucCon, uCurrentDstAddr);
		ATA_SetTBufSize( ucCon, uCurrentCount*ATA_SECTORSIZE);
		ATA_SetXfrNum(ucCon, uCurrentCount*ATA_SECTORSIZE);
		
		ATA_SetDevicePosition(ucCon, uCurrentLba, uCurrentCount, eSlotsel);
		ATA_SetTaskFileRegValue(ucCon, eCF_TASKFILE_COMMAND, ATA_CMD_READDMA);

		ATA_WaitForDeviceReady(ucCon);
		ATA_SetConfig(ucCon, eATA_MODE_UDMA, eATA_DMA_READ_DATA);

#if PERF_TEST_UDMA	
	StartTimer(1);
#endif

		/*ATA Transfer Command */
		ATA_SetTransferCommand(ucCon, eATA_XFR_CMD_START);

		ATA_WaitForTransferDone(ucCon);	// Host

		ATA_SetTransferCommand(ucCon, eATA_XFR_CMD_ABORT);

#if PERF_TEST_UDMA
	uElapsedTime += StopTimer(1);
#endif

		ATA_SetConfig(ucCon, eATA_MODE_PIOCPU, eATA_DMA_READ_DATA);
		ATA_WaitForDeviceReady(ucCon);

		ATA_SetUdmaAutoMode(ucCon, DISABLE);

		uRound++;
	}

#if PERF_TEST_UDMA
	UART_Printf(" UDMA Reading time : %d us, Performance : %lf MByte/sec\n"
			,uElapsedTime,(float)(uSectorCount*512./uElapsedTime));
#endif

	return TRUE;
}

void ATA_SetDevicePosition(u8 ucCon, u32 uLba, u32 uSectorCount,eATA_SLOT_SEL eSlotsel)
{
	ATA_SetTaskFileRegValue(ucCon, eCF_TASKFILE_SECTOR,	uSectorCount&0xff);	
	ATA_SetTaskFileRegValue(ucCon, eCF_TASKFILE_LOWLBA,	uLba&0xff);
	ATA_SetTaskFileRegValue(ucCon, eCF_TASKFILE_MIDLBA,		(uLba>>8)&0xff);
	ATA_SetTaskFileRegValue(ucCon, eCF_TASKFILE_HIGHLBA,	(uLba>>16)&0xff);
	if (eSlotsel == eATA_SLAVE)
	ATA_SetTaskFileRegValue(ucCon, eCF_TASKFILE_DEVICE,		(uLba>>24)&0xf|0x50);
	else
	ATA_SetTaskFileRegValue(ucCon, eCF_TASKFILE_DEVICE,		(uLba>>24)&0xf|0x40);		
}


//////////
// Function Name : ATA_StartWritingSectors
// Function Description : This function sets writing function which transfers block data to device.
// Input : 	ucCon - ATA Controller Number
//			uStBlock 		- LBA Block
//			uBlocks		- Block Count
//			uBufAddr		- Buffer Address
// Output : 	true/false
// Version : v0.1
u8 ATA_StartWritingSectors(u8 ucCon, u32 uStBlock, u32 uBlocks, u32 uBufAddr, eATA_SLOT_SEL eSlotsel)
{
	u32 uDeviceCmd = 0;

//	UART_Printf("uStBlock:%d, uBlocks:%d, uBufAddr:0x%x\n", uStBlock, uBlocks, uBufAddr);
	uDeviceCmd = (g_oaATAInform[ucCon].eAtaMode == eATA_MODE_UDMA) ? ATA_CMD_WRITEDMA : ATA_CMD_WRITESECTOR;

	/*Source Buffer Setting*/
	ATA_SetSBufStart( ucCon, uBufAddr);
	ATA_SetSBufSize( ucCon, uBlocks*ATA_SECTORSIZE);
	ATA_SetXfrNum(ucCon, uBlocks*ATA_SECTORSIZE);

	ATA_SetDevicePosition(ucCon, uStBlock, uBlocks, eSlotsel);
	ATA_SetTaskFileRegValue( ucCon, eCF_TASKFILE_COMMAND , uDeviceCmd);

	ATA_WaitForDeviceReady( ucCon);
//	UART_Printf("(");
	ATA_SetConfig(ucCon, g_oaATAInform[ucCon].eAtaMode, eATA_DMA_WRITE_DATA);
//	UART_Printf(")");
#if 1	
	ATA_SetTransferCommand(ucCon, eATA_XFR_CMD_START);

	#if 0	// These lines are only for ATA Bug Test
	for ( uLoopCnt = 0; uLoopCnt < 1000; uLoopCnt++)
	{
		ATA_SetTransferCommand(ucCon, eATA_XFR_CMD_STOP);
		ATA_SetTransferCommand(ucCon, eATA_XFR_CMD_CONTINUE);
	}
	UART_Printf("$");
	#endif
#else
	Set_Ata_Cmd_START();
#endif
	return TRUE;
	
}

//////////
// Function Name : ATA_StartReadingSectors
// Function Description : This function sets reading function which transfers block data from device.
// Input : 	ucCon - ATA Controller Number
//			uStBlock 		- LBA Block
//			uBlocks		- Block Count
//			uBufAddr		- Buffer Address
// Output : 	true/false
// Version : v0.1
u8 ATA_StartReadingSectors(u8 ucCon, u32 uStBlock, u32 uBlocks, u32 uBufAddr, eATA_SLOT_SEL eSlotSel)
{
	u32 uDeviceCmd = 0;
	
//	UART_Printf("uStBlock:%d, uBlocks:%d, uBufAddr:0x%x\n", uStBlock, uBlocks, uBufAddr);
	uDeviceCmd = (g_oaATAInform[ucCon].eAtaMode == eATA_MODE_UDMA) ? ATA_CMD_READDMA : ATA_CMD_READSECTOR;

	/*Target Buffer Setting*/
	ATA_SetTBufStart( ucCon, uBufAddr);
	ATA_SetTBufSize( ucCon, uBlocks*ATA_SECTORSIZE);
	ATA_SetXfrNum(ucCon, uBlocks*ATA_SECTORSIZE);

	ATA_SetDevicePosition(ucCon, uStBlock, uBlocks, eSlotSel);
	ATA_SetTaskFileRegValue( ucCon, eCF_TASKFILE_COMMAND , uDeviceCmd);
	
	ATA_WaitForDeviceReady( ucCon);
//	UART_Printf("(");
	ATA_SetConfig(ucCon, g_oaATAInform[ucCon].eAtaMode, eATA_DMA_READ_DATA);
//	UART_Printf(")");
#if 1
	ATA_SetTransferCommand(ucCon, eATA_XFR_CMD_START);
#else
	Set_Ata_Cmd_START();
#endif
	return TRUE;
	
}

//////////
// Function Name : ATA_IsWritingBlocksDone
// Function Description : This function checks that data transfer to device has done.
// Input : 	ucCon - ATA Controller Number
// Output : 	true/false
// Version : v0.1
u8 ATA_IsWritingSectorsDone(u8 ucCon)
{
	u8			bRetValue	= false;

	ATA_WaitForTransferDone(ucCon);
	bRetValue	= ATA_IsDmaDone(ucCon);
	return bRetValue;
}

//////////
// Function Name : ATA_IsReadingBlocksDone
// Function Description : This function checks that data transfer from device to host has done.
// Input : 	ucCon - ATA Controller Number
// Output : 	true/false
// Version : v0.1
u8 ATA_IsReadingSectorsDone(u8 ucCon)
{
	return ATA_IsWritingSectorsDone(ucCon);
}


//////////
// Function Name : ATA_IsDmaDone
// Function Description : This function checks that data DMA transfer to device has done.
// Input : 	ucCon - ATA Controller Number
// Output : 	NONE
// Version : v0.1
u8 ATA_IsDmaDone(u8 ucCon)
{
	ATA_WaitForHostReady( ucCon);
	ATA_SetTransferCommand(ucCon, eATA_XFR_CMD_ABORT);
	ATA_WaitForHostReady( ucCon);
	ATA_SetConfig( ucCon, eATA_MODE_PIOCPU, eATA_DMA_WRITE_DATA);
	ATA_WaitForDeviceReady(ucCon);
	
	if (g_oaATAInform[ucCon].eAtaMode == eATA_MODE_UDMA)
	{
		ATA_SetUdmaAutoMode(ucCon, DISABLE);
	}

	return TRUE;
}
/*---------------------------------- APIs of ATA_CONTROL Registers ---------------------------------*/
//////////
// Function Name : ATA_SetEnable
// Function Description : This function enables/disables ATA controller.
// Input : 	ucCon - ATA Controller Number
//			ucEnFlag - ATA Enable/Disable
// Output : 	NONE
// Version : v0.1
void ATA_SetEnable(u8 ucCon, u8 ucEnFlag)
{
	u32		uRegValue = 0;

	uRegValue = ATA_GetRegValue(ucCon, eATA_CONTROL);
	uRegValue = uRegValue & ~(0x1) | (ucEnFlag << 0);

	ATA_SetRegValue(ucCon, eATA_CONTROL, uRegValue);			
}
/*---------------------------------- APIs of ATA_STATUS Registers ---------------------------------*/
//////////
// Function Name : ATA_WaitForTransferDone
// Function Description : This function implements wait function by checking ATA STATUS Register.
// Input : 	ucCon - ATA Controller Number
// Output : 	NONE
// Version : v0.1
void ATA_WaitForTransferDone(u8 ucCon)
{
	u32	uRegValue = 0;
//	UART_Printf("MODE3:%s\n", ATA_GetModeName(g_oaATAInform[ucCon].eAtaMode));
	do {
		ATA_WaitForHostReady(ucCon); /// needed
		uRegValue	= ATA_GetRegValue(ucCon, eATA_STATUS);
//		UART_Printf("%s,%d:Value:0x%x\n", __FUNCTION__, __LINE__, uRegValue);
	} while((uRegValue & 3)!=0);
	
}

/*---------------------------------- APIs of ATA_COMMAND Registers ---------------------------------*/
//////////
// Function Name : ATA_SetTransferCommand
// Function Description : This function sets up ATA transfer command.
// Input : 	ucCon - ATA Controller Number
//			eXfrCmd - STOP/START/ABORT/CONTINUE
// Output : 	NONE
// Version : v0.1
void ATA_SetTransferCommand(u8 ucCon, eATA_XFR_CMD eXfrCmd)
{
	u32		uRegValue = 0;

	uRegValue = ATA_GetRegValue(ucCon, eATA_COMMAND);
	uRegValue = uRegValue & ~(0x3) | (eXfrCmd << 0);

	ATA_SetRegValue(ucCon, eATA_COMMAND, uRegValue);			
}
/*---------------------------------- APIs of ATA_SWRST Registers ---------------------------------*/
//////////
// Function Name : ATA_SetSwRst
// Function Description : This function implements s/w reset function of ATA controller.
// Input : 	ucCon - ATA Controller Number
//			eIrqSrc - Interrupt source to mask
// Output : 	NONE
// Version : v0.1
void ATA_SetSwRst(u8 ucCon, u8 ucEnFlag)
{
	u32		uRegValue = 0;

	uRegValue = ATA_GetRegValue(ucCon, eATA_SWRST);
	uRegValue = uRegValue & ~(0x1) | (ucEnFlag << 0);

	ATA_SetRegValue(ucCon, eATA_SWRST, uRegValue);			
}

/*---------------------------------- APIs of ATA_IRQ/ATA_IRQ_MASK Registers ---------------------------------*/
//////////
// Function Name : ATA_SetIRQMask
// Function Description : This function implements interrupt masking function of ATA controller.
// Input : 	ucCon - ATA Controller Number
//			eIrqSrc - Interrupt source to mask
// Output : 	NONE
// Version : v0.1
void ATA_SetIRQ(u8 ucCon, eATA_IRQ_SRC eIrqSrc)
{
	u32		uRegValue = 0;

	uRegValue = ATA_GetRegValue(ucCon, eATA_IRQ);
	uRegValue = uRegValue & ~(0x1f) | (eIrqSrc << 0);

	ATA_SetRegValue(ucCon, eATA_IRQ, uRegValue);			
}

//////////
// Function Name : ATA_SetIRQMask
// Function Description : This function implements interrupt masking function of ATA controller.
// Input : 	ucCon - ATA Controller Number
//			eIrqSrc - Interrupt source to mask
// Output : 	NONE
// Version : v0.1
void ATA_SetIRQMask(u8 ucCon, eATA_IRQ_SRC eIrqSrc)
{
	u32		uRegValue = 0;

	uRegValue = ATA_GetRegValue(ucCon, eATA_IRQ_MASK);
	uRegValue = uRegValue & ~(0x1f) | (eIrqSrc << 0);

	ATA_SetRegValue(ucCon, eATA_IRQ_MASK, uRegValue);			
}

u8 ATA_FindInterruptRequest(u8 ucCon, u32* uNthBit)
{
	u32	uLoopCnt = 0;
	u32	uRegValue1 = 0;
	u32	uRegValue2 = 0;
	
	for(uLoopCnt=0;uLoopCnt< ATA_INT_NUM;uLoopCnt++) {
		uRegValue1 = ATA_GetRegValue(ucCon, eATA_IRQ);
		uRegValue2 = ATA_GetRegValue(ucCon, eATA_IRQ_MASK);
		if(((uRegValue1&(~uRegValue2))>>uLoopCnt)&0x01) 
			break;
	}

	*uNthBit = uLoopCnt;
	
	if (uLoopCnt == ATA_INT_NUM)
		return FALSE;
	else
		return TRUE;
	
}

void ATA_ClearPending(u8 ucCon, u32 uSrcInt)
{
	u32		uRegValue = 0;

	uRegValue = ATA_GetRegValue(ucCon, eATA_IRQ);
	uRegValue = uRegValue & ~(0x1 << uSrcInt) | (0x1 << uSrcInt);

	ATA_SetRegValue(ucCon, eATA_IRQ, uRegValue);			
}

void ATA_ClearAllInterrupt(u8 ucCon)
{
	u32		uRegValue = 0;

	uRegValue = ATA_GetRegValue(ucCon, eATA_IRQ);
	uRegValue |= (0x1f << 0);

	ATA_SetRegValue(ucCon, eATA_IRQ, uRegValue);			
}
/*---------------------------------- APIs of ATA_CFG Registers ---------------------------------*/
void ATA_SetConfig(u8 ucCon, eATA_MODE_6400 eMode, eATA_DMA_DIR eDmaDir)
{
	switch(eMode)
	{
		case eATA_MODE_PIOCPU : 
			ATA_SetXferMode(ucCon, eATA_MODE_PIOCPU);	// set PIO_CPU class
			break;
		case eATA_MODE_PIODMA : 
			ATA_SetXferMode(ucCon, eATA_MODE_PIODMA);	// set PDMA class
			ATA_SetDMAXferDir(ucCon, eDmaDir);			// DMA read/write mode
			break;
		case eATA_MODE_UDMA : 
			ATA_SetXferMode(ucCon, eATA_MODE_UDMA);	// set UDMA class
			ATA_SetUdmaAutoMode(ucCon, ENABLE);		// set ATA DMA auto mode (enable multi block transfer)
			ATA_SetDMAXferDir(ucCon, eDmaDir);			// DMA read/write mode
			break;
		default :
			break;
	}
}

//////////
// Function Name : ATA_SetUdmaAutoMode
// Function Description : This function sets up UDMA auto mode.
// Input : 	ucCon - ATA Controller Number
//			ucEnFlag - UDMA auto mode Enable/Disable
// Output : 	NONE
// Version : v0.1
void ATA_SetUdmaAutoMode(u8 ucCon, u8 ucEnFlag)
{
	u32		uRegValue = 0;

	uRegValue = ATA_GetRegValue(ucCon, eATA_CFG);
	uRegValue = uRegValue & ~(0x200) | (ucEnFlag << 9);		

	ATA_SetRegValue(ucCon, eATA_CFG, uRegValue);			
}

u8 ATA_IsTBufFullContinue(u8 ucCon)
{
	u32		uRegValue = 0;
	u8		bStatus = FALSE;

	uRegValue = ATA_GetRegValue(ucCon, eATA_CFG);
	bStatus = (uRegValue & (0x80)) ? TRUE : FALSE;		

	return bStatus;
}

u8 ATA_IsSBufEmptyContinue(u8 ucCon)
{
	u32		uRegValue = 0;
	u8		bStatus = FALSE;

	uRegValue = ATA_GetRegValue(ucCon, eATA_CFG);
	bStatus = (uRegValue & (0x100)) ? TRUE : FALSE;		

	return bStatus;
}

//////////
// Function Name : ATA_SetEndian
// Function Description : This function sets up the endian mode of ATA controller.
// Input : 	ucCon - ATA Controller Number
//			eEndianMode - Little/Big endian
// Output : 	NONE
// Version : v0.1
void ATA_SetEndian(u8 ucCon, eATA_ENDIAN eEndianMode)
{
	u32		uRegValue = 0;

	uRegValue = ATA_GetRegValue(ucCon, eATA_CFG);
	uRegValue = uRegValue & ~(0x40) | (eEndianMode << 6);		

	ATA_SetRegValue(ucCon, eATA_CFG, uRegValue);			
}

//////////
// Function Name : ATA_SetDMAXferDir
// Function Description : This function sets up the DMA direction of ATA controller.
// Input : 	ucCon - ATA Controller Number
//			eDmaDir - DMA transfer direction flag (read DMA/write DMA)
// Output : 	NONE
// Version : v0.1
void ATA_SetDMAXferDir(u8 ucCon, eATA_DMA_DIR eDmaDir)
{
	u32		uRegValue = 0;

	uRegValue = ATA_GetRegValue(ucCon, eATA_CFG);
	uRegValue = uRegValue & ~(0x10) | (eDmaDir << 4);		

	ATA_SetRegValue(ucCon, eATA_CFG, uRegValue);			
}

//////////
// Function Name : ATA_SetIORDY
// Function Description : This function sets up ATA mode.
// Input : 	ucCon - ATA Controller Number
//			eAtaMode - PIO CPU/PIO DMA/UDMA
// Output : 	NONE
// Version : v0.1
void ATA_SetXferMode(u8 ucCon, eATA_MODE_6400 eAtaMode)
{
	u32		uRegValue = 0;

	uRegValue = ATA_GetRegValue(ucCon, eATA_CFG);
	uRegValue = uRegValue & ~(0xc) | (eAtaMode << 2);		

	ATA_SetRegValue(ucCon, eATA_CFG, uRegValue);			
}

//////////
// Function Name : ATA_SetIORDY
// Function Description : This function sets up IORDY of ATA controller.
// Input : 	ucCon - ATA Controller Number
//			ucEnFlag - IORDY Enable/Disable
// Output : 	NONE
// Version : v0.1
void ATA_SetIORDY(u8 ucCon, u8 ucEnFlag)
{
	u32		uRegValue = 0;

	uRegValue = ATA_GetRegValue(ucCon, eATA_CFG);
	uRegValue = uRegValue & ~(0x2) | (ucEnFlag << 1);		

	ATA_SetRegValue(ucCon, eATA_CFG, uRegValue);			
}

//////////
// Function Name : ATA_SetRst
// Function Description : This function sets up reset of ATA controller.
// Input : 	ucCon - ATA Controller Number
//			ucEnFlag - IORDY Enable/Disable
// Output : 	NONE
// Version : v0.1
void ATA_SetRst(u8 ucCon, u8 ucEnFlag)
{
	u32		uRegValue = 0;

	uRegValue = ATA_GetRegValue(ucCon, eATA_CFG);
	uRegValue = uRegValue & ~(0x1) | (ucEnFlag << 0);		

	ATA_SetRegValue(ucCon, eATA_CFG, uRegValue);			
}
/*---------------------------------- APIs of ATA Transfer Registers ---------------------------------*/
//////////
// Function Name : ATA_SetSBufStart
// Function Description : This function sets the start address of source buffer.
// Input : 	ucCon 			- ATA Controller Number
//			uBufAddr			- the start address of source buffer
// Output : 	NONE
// Version : v0.1
void ATA_SetSBufStart(u8 ucCon, u32 uBufAddr)
{
	u32		uRegValue = 0;

	uRegValue = ATA_GetRegValue(ucCon, eATA_SBUF_START);
//	uRegValue = uRegValue & ~(0xfffffffc) | (uBufAddr << 2);		
	uRegValue = uRegValue & ~(0xfffffffc) | (uBufAddr << 0);		

	ATA_SetRegValue(ucCon, eATA_SBUF_START, uRegValue);			
}

//////////
// Function Name : ATA_SetTBufStart
// Function Description : This function sets the start address of target buffer.
// Input : 	ucCon 			- ATA Controller Number
//			uBufAddr			- the target address of source buffer
// Output : 	NONE
// Version : v0.1
void ATA_SetTBufStart(u8 ucCon, u32 uBufAddr)
{
	u32		uRegValue = 0;

	uRegValue = ATA_GetRegValue(ucCon, eATA_TBUF_START);
//	uRegValue = uRegValue & ~(0xfffffffc) | (uBufAddr << 2);		
	uRegValue = uRegValue & ~(0xfffffffc) | (uBufAddr << 0);		

	ATA_SetRegValue(ucCon, eATA_TBUF_START, uRegValue);			
}

//////////
// Function Name : ATA_SetSBufSize
// Function Description : This function sets the start address of source buffer.
// Input : 	ucCon 			- ATA Controller Number
//			uBufAddr			- the start address of source buffer
// Output : 	NONE
// Version : v0.1
void ATA_SetSBufSize(u8 ucCon, u32 uSize)
{
	u32		uRegValue = 0;

	uRegValue = ATA_GetRegValue(ucCon, eATA_SBUF_SIZE);
//	uRegValue = uRegValue & ~(0xffffffe0) | (uSize << 5);		
	uRegValue = uRegValue & ~(0xffffffe0) | (uSize << 0);		

	ATA_SetRegValue(ucCon, eATA_SBUF_SIZE, uRegValue);			
}

//////////
// Function Name : ATA_SetTBufSize
// Function Description : This function sets the start address of target buffer.
// Input : 	ucCon 			- ATA Controller Number
//			uBufAddr			- the start address of target  buffer
// Output : 	NONE
// Version : v0.1
void ATA_SetTBufSize(u8 ucCon, u32 uSize)
{
	u32		uRegValue = 0;

	uRegValue = ATA_GetRegValue(ucCon, eATA_TBUF_SIZE);
//	uRegValue = uRegValue & ~(0xffffffe0) | (uSize << 5);		
	uRegValue = uRegValue & ~(0xffffffe0) | (uSize << 0);		

	ATA_SetRegValue(ucCon, eATA_TBUF_SIZE, uRegValue);			
}

//////////
// Function Name : ATA_SetXfrNum
// Function Description : This function sets the start address of source buffer.
// Input : 	ucCon 			- ATA Controller Number
//			uNum			- Data Transfer number
// Output : 	NONE
// Version : v0.1
void ATA_SetXfrNum(u8 ucCon, u32 uNum)
{
	u32		uRegValue = 0;

	uRegValue = ATA_GetRegValue(ucCon, eATA_XFR_NUM);
//	uRegValue = uRegValue & ~(0xfffffffe) | (uNum << 1);
	uRegValue = uRegValue & ~(0xfffffffe) | (uNum << 0);

	ATA_SetRegValue(ucCon, eATA_XFR_NUM, uRegValue);			
}

/*---------------------------------- APIs of ATA TASK FILE Registers ---------------------------------*/
//////////
// Function Name : ATA_GetTaskFileRegValue
// Function Description : This function gets the value of ATA Registers.
// Input : 	ucCon 				- ATA Controller Number
//			uATATaskFileRegId		- the Id of ATA Task File Registers
// Output : 	*puATAReg			- the value of specified register
// Version : v0.1
u8 ATA_GetTaskFileRegValue(u8 ucCon, eCF_TASKFILE_Id uATATaskFileRegId)
{
	volatile u32 *	puATABaseAddr;
	volatile u8 *	puATAReg;	

	puATABaseAddr			= &(ATA(ucCon)->rrATA_PIO_DTR);
	puATAReg				= (u8 *)(puATABaseAddr + uATATaskFileRegId);
//	UART_Printf("[ATA_GetTaskFileRegValue]puATABase : 0x%x, RegID:0x%x, DestAddr : 0x%x, Result : 0x%x\n", puATABaseAddr, uATATaskFileRegId, puATAReg, *puATAReg);	
	return *puATAReg;	
}

u16 ATA_GetTaskFileRegValue16(u8 ucCon, eCF_TASKFILE_Id uATATaskFileRegId)
{
	volatile u32 *	puATABaseAddr;
	volatile u16 *	puATAReg;	

	puATABaseAddr			= &(ATA(ucCon)->rrATA_PIO_DTR);
	puATAReg				= (u16 *)(puATABaseAddr + uATATaskFileRegId);
//	UART_Printf("[ATA_GetTaskFileRegValue16]puATABase : 0x%x, RegID:0x%x, DestAddr : 0x%x, Result : 0x%x\n", puATABaseAddr, uATATaskFileRegId, puATAReg, *puATAReg);	
	return *puATAReg;	
}
//////////
// Function Name : ATA_SetTaskFileRegValue
// Function Description : This function sets up the value of ATA TaskFile Registers.
// Input : 	ucCon 				- ATA Controller Number
//			uATATaskFileRegId		- the Id of ATA Task File Registers
//			uValue				- the value of register
// Output : 	NONE
// Version : v0.1
void ATA_SetTaskFileRegValue(u8 ucCon, eCF_TASKFILE_Id uATATaskFileRegId, u32 uValue)
{
	volatile u32 *	puATABaseAddr;
	volatile u8 *	puATAReg;	

	ATA_WaitForHostReady(ucCon);
	
	puATABaseAddr		= &(ATA(ucCon)->rrATA_PIO_DTR);
	puATAReg			= (u8 *)(puATABaseAddr + uATATaskFileRegId);
	*puATAReg			= uValue;
//	UART_Printf("[ATA_SetTaskFileRegValue]puATABase : 0x%x, RegID:0x%x, DestAddr : 0x%x, Value : 0x%x\n", puATABaseAddr, uATATaskFileRegId, puATAReg, uValue);	
}

void ATA_SetTaskFileRegValue16(u8 ucCon, eCF_TASKFILE_Id uATATaskFileRegId, u16 uValue)
{
	volatile u32 *	puATABaseAddr;
	volatile u16 *	puATAReg;	

	ATA_WaitForHostReady(ucCon);
	
	puATABaseAddr		= &(ATA(ucCon)->rrATA_PIO_DTR);
	puATAReg			= (u16 *)(puATABaseAddr + uATATaskFileRegId);
	*puATAReg			= uValue;
//	UART_Printf("[ATA_SetTaskFileRegValue16]puATABase : 0x%x, RegID:0x%x, DestAddr : 0x%x, Value : 0x%x\n", puATABaseAddr, uATATaskFileRegId, puATAReg, uValue);	
}
/*---------------------------------- APIs of ATA_FIFO_STATUS Registers ---------------------------------*/
//////////
// Function Name : ATA_WaitForHostReady
// Function Description : This function awaits the ready state of Host.
// Input : 	ucCon 	- ATA Controller Number
// Output : 	NONE
// Version : v0.1
void ATA_WaitForHostReady(u8 ucCon)
{
	u32			ucTaskFileRegValue 	= 0;
	
	do
	{
		ucTaskFileRegValue = ATA_GetRegValue( ucCon, eATA_FIFO_STATUS );
//		UART_Printf("[WaitForHostReady]ucTaskFileRegValue:0x%x\n", ucTaskFileRegValue);
	} while ( (ucTaskFileRegValue >> 28) != 0 ) ;
}

/*---------------------------------- APIs of general ATA ---------------------------------*/

//////////
// Function Name : ATA_GetRegAddr
// Function Description : This function gets the address of ATA Registers.
// Input : 	ucATARegId		- the Id of ATA Registers
// Output : 	*puATAReg		- the value of specified register
// Version : v0.1
u32 ATA_GetRegAddr(u8 ucCon, eATA_Id uATARegId)
{
	volatile u32 *	puATABaseAddr;
	volatile u32 *	puATAReg;	
	u32			uATARegAddr = 0;

	puATABaseAddr			= &(ATA(ucCon)->rrATA_CONTROL);
	puATAReg				= puATABaseAddr + uATARegId;

	uATARegAddr			= (u32)&puATAReg;
	return uATARegAddr;	
}

//////////
// Function Name : ATA_GetRegValue
// Function Description : This function gets the value of ATA Registers.
// Input : 	ucATARegId		- the Id of ATA Registers
// Output : 	*puATAReg		- the value of specified register
// Version : v0.1
u32 ATA_GetRegValue(u8 ucCon, eATA_Id uATARegId)
{
	volatile u32 *	puATABaseAddr;
	volatile u32 *	puATAReg;	

	puATABaseAddr			= &(ATA(ucCon)->rrATA_CONTROL);
	puATAReg				= puATABaseAddr + uATARegId;
//	UART_Printf("[ATA_GetRegValue]ATABaseAddr : 0x%x, ATAId:0x%d, ATAReg: 0x%x, Value:0x%x \n", puATABaseAddr, uATARegId, puATAReg, *puATAReg);
	return *puATAReg;	
}

//////////
// Function Name : ATA_SetRegValue
// Function Description : This function sets the value of ATA Registers.
// Input : 	ucATARegId		- the Id of ATA Registers
//			uValue			- the value of register
// Output : 	NONE
// Version : v0.1
void ATA_SetRegValue(u8 ucCon, eATA_Id uATARegId, u32 uValue)
{
	volatile u32 *	puATABaseAddr;
	volatile u32 *	puATAReg;	

	puATABaseAddr		= &(ATA(ucCon)->rrATA_CONTROL);
	puATAReg			= puATABaseAddr + uATARegId;
//	UART_Printf("[ATA_SetRegValue]ATABaseAddr : 0x%x, ATAReg: 0x%x, Value:0x%x \n", puATABaseAddr, puATAReg, uValue);
	*puATAReg			= uValue;
}

//////////
// Function Name : ATA_GetModeName
// Function Description : 
//   This function returns ATA mode string.
// Input : eATAMode - PIO/PIOCPU/PDMA/UDMA
// Output : the name of ATA mode
// Version : v0.1 
u8 *ATA_GetModeName(eATA_MODE_6400 eAtaMode)
{
	switch(eAtaMode)
	{
		case eATA_MODE_PIOCPU :
			return "PIO";
		case eATA_MODE_PIODMA :
			return "PIODMA";
		case eATA_MODE_UDMA :
			return "UDMA";
		case eATA_MODE_NONE :
			return "NONE";			
		default :
			return "NONE";			
	}
}


