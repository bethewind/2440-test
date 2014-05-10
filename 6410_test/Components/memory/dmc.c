/**************************************************************************************
* 
*	Project Name : S3C6410 Validation
*
*	Copyright 2006 by Samsung Electronics, Inc.
*	All rights reserved.
*
*	Project Description :
*		This software is only for validating functions of the S3C6410.
*		Anybody can use this software without our permission.
*  
*--------------------------------------------------------------------------------------
* 
*	File Name : dmc.c
*  
*	File Description : This file implements the API functons for DRAM controller.
*
*	Author : Wonjoon Jang
*	Dept. : AP Development Team
*	Created Date : 2007/01/05
*	Version : 0.1 
* 
*	History
*	- Created(Wonjoon.jang 2007/01/05)
*       - Revised for S3C6410 (Sunil.Roe 2008/02/28)
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
#include "dmc.h"

#define 	DMC(__n) 		( ( volatile oDMC_REGS * ) ( g_DMCBase[__n] ) )
#define 	DMC_CONNUM			(2)

// define global variables
static void *			g_DMCBase[DMC_CONNUM];


//SFR
typedef struct tag_DMC_REGS
{
	u32 rMEMSTAT;		// 0x7E001000 (DMC1 Case)
	u32 rMEMCCMD;		// 0x7E001004
	u32 rDIRECTCMD;		// 0x7E001008
	u32 rMEMCFG;		// 0x7E00100C
	u32 rREFRESH;		// 0x7E001010
	u32 rCASLAT;		// 0x7E001014
	u32 rT_DQSS;		// 0x7E001018
	u32 rT_MRD;			// 0x7E00101C
	u32 rT_RAS;			// 0x7E001020
	u32 rT_RC;			// 0x7E001024
	u32 rT_RCD;			// 0x7E001028
	u32 rT_RFC;			// 0x7E00102C
	u32 rT_RP;			// 0x7E001030
	u32 rT_RRD;			// 0x7E001034
	u32 rT_WR;			// 0x7E001038
	u32 rT_WTR;			// 0x7E00103C
	u32 rT_XP;			// 0x7E001040
	u32 rT_XSR;			// 0x7E001044
	u32 rT_ESR;			// 0x7E001048
	u32 rMEMCFG2;		// 0x7E00104C

	u32 reserved1[44];

	u32 rID_0_CFG;		// 0x7E001100
	u32 rID_1_CFG;		// 0x7E001104
	u32 rID_2_CFG;		// 0x7E001108
	u32 rID_3_CFG;		// 0x7E00110C
	u32 rID_4_CFG;		// 0x7E001110
	u32 rID_5_CFG;		// 0x7E001104
	u32 rID_6_CFG;		// 0x7E001108
	u32 rID_7_CFG;		// 0x7E00110C
	u32 rID_8_CFG;		// 0x7E001120
	u32 rID_9_CFG;		// 0x7E001124
	u32 rID_10_CFG;		// 0x7E001128
	u32 rID_11_CFG;		// 0x7E00112C
	u32 rID_12_CFG;		// 0x7E001130
	u32 rID_13_CFG;		// 0x7E001134
	u32 rID_14_CFG;		// 0x7E001138
	u32 rID_15_CFG;		// 0x7E00113C

	u32 reserved2[48];

	u32 rCHIP_0_CFG;	// 0x7E001200
	u32 rCHIP_1_CFG;	// 0x7E001204

	u32 reserved3[62];

	u32 rUSER_STAT;		// 0x7E001300
	u32 rUSER_CFG;		// 0x7E001304
		
} 
oDMC_REGS;






	
//////////
// Function Name : DMC_Init
// Function Description : This function initializes a certain DMC Controller.(for SMDK6410)
//					  DMC0 => X16, mDDR (512Mb),  DMC1=> X16*2, mDDR (1025Mb)
// Input : 	uController - DMC Controller No. 
// Output : 	
//			
// Version : v0.1
void	DMC_Init(u8 uController, DMC_eStopCLK eStopClk, DMC_eAutoPD eAutoPD, u32 uPDprd )
{
	u32 uBaseAddress;
	u32 uPara_REF,uPara_RAS, uPara_RC, uPara_RCD, uScheduled_Para;
	u32 uPara_RFC, uPara_RP, uPara_RRD, uPara_WR, uPara_WTR, uPara_XSR;

	if(uController == 0)
	{
		uBaseAddress = DMC0_BASE;
	}
	else if(uController == 1)
	{
		uBaseAddress = DMC1_BASE;
	}
	else
	{
		
	}
	
	g_DMCBase[uController] = (void *)uBaseAddress;



	// Get system Clock Information.  g_HCLK
	SYSC_GetClkInform();

	//Enter the Configuration Mode
	Outp32(&DMC(uController)->rMEMCCMD, (0x4<<0));	// Memc_cmd = 0x4

	// Timing Parameter Setup
	uPara_REF = (((g_HCLK /1000 *eDDR_REF) -1)/1000000 + 1);	
	Outp32(&DMC(uController)->rREFRESH, uPara_REF);

	Outp32(&DMC(uController)->rCASLAT, (0x3<<1));	// CAS Latency = 3
	Outp32(&DMC(uController)->rT_DQSS, 0x1);			// mDDR = 0x1 (0.75 ~ 1.25)
	Outp32(&DMC(uController)->rT_MRD, 0x2);			// Mode Register Cmd Time
	uPara_RAS = (((g_HCLK /1000 *eDDR_RAS) -1)/1000000 + 1);
	Outp32(&DMC(uController)->rT_RAS, uPara_RAS);	// RAS(Row Active time) to Precharge delay , (min 45ns,  7@133MHz, margin:1 clock)
	uPara_RC = (((g_HCLK /1000 *eDDR_RC) -1)/1000000 + 1);
	Outp32(&DMC(uController)->rT_RC, uPara_RC);		// RC(Row Cycle time) : Active bank x to Active bank x delay (min. 67.5ns, 10@133MHz, margin: 1clock)
	uPara_RCD = (((g_HCLK /1000 *eDDR_RCD) -1)/1000000 + 1);
	if (uPara_RCD <4)
		{
			 uScheduled_Para = 3;
		}
	else
		{
			uScheduled_Para = uPara_RCD;
		}
	uPara_RCD = ((uScheduled_Para-3)<<3) | (uPara_RCD);
	Outp32(&DMC(uController)->rT_RCD, uPara_RCD);	// RAS to CAS Delay, (min 22.5ns, 4@133MHz, margin:1clock)
	uPara_RFC = (((g_HCLK /1000 *eDDR_RFC) -1)/1000000 + 1);
	if (uPara_RFC <4)
		{
			 uScheduled_Para = 3;
		}
	else
		{
			uScheduled_Para = uPara_RFC;
		}
	uPara_RFC = ((uScheduled_Para-3)<<3) | (uPara_RFC);		// Auto Refresh to cmd time(>=t_RC), (min 80ns, 11@133MHz)
	Outp32(&DMC(uController)->rT_RFC, uPara_RFC);
	uPara_RP = (((g_HCLK /1000 *eDDR_RP) -1)/1000000 + 1);
	if (uPara_RP <4)
		{
			 uScheduled_Para = 3;
		}
	else
		{
			uScheduled_Para = uPara_RP;
		}
	uPara_RP = ((uScheduled_Para-3)<<3) | (uPara_RP);
	Outp32(&DMC(uController)->rT_RP, uPara_RP);		// Precharge to RAS delay (Row Pre-charge time) (min 22.5ns, 4@133MHz, margin:1clock)
	uPara_RRD = (((g_HCLK /1000 *eDDR_RRD) -1)/1000000 + 1);
	Outp32(&DMC(uController)->rT_RRD, uPara_RRD);	//  Active bank x to Active bank y delay (min 15ns, 3@133MHz, margin:1clock)
	uPara_WR = (((g_HCLK /1000 *eDDR_WR) -1)/1000000 + 1);
	Outp32(&DMC(uController)->rT_WR, uPara_WR);		//  write to precharge delay (min 15ns, 3@133MHz, margin:1clock)
//	uPara_WTR= (((g_HCLK /1000 *eDDR_RP) -1)/1000000 + 1) + 2;
	uPara_WTR = 2;
	Outp32(&DMC(uController)->rT_WTR, uPara_WTR);		//  write to read delay (tDAL?) (2clk + tRP) // 확인 필요 
	Outp32(&DMC(uController)->rT_XP, 2);				//  exit power down cmd time ( 1clk + 1.5ns)
	uPara_XSR= (((g_HCLK /1000 *eDDR_XSR) -1)/1000000 + 1) ;
	Outp32(&DMC(uController)->rT_XSR,uPara_XSR);		// exit self refresh cmd time, 
	Outp32(&DMC(uController)->rT_ESR,uPara_XSR);		// self refresh cmd time // 확인 필요 

	
		if(uController == 0)
	{
		// Memory Configuration Register
		Outp32(&DMC(uController)->rMEMCFG,
			(0<<21) |		// 1chip
			(0<<18) |		// Qos master selection by ARID[3:0]
			(2<<15) |		// Burst 4
			(eStopClk<<14) |		// Disable Stop Mem Clock
			(eAutoPD<<13) |		// Disable Auto Power Down
			(uPDprd<<7)   |		// Auto Power Down Period
			(eAP_bit<<6)	|		// Auto Precharge bit in bit 10
			(eRow_bit<<3)	|		// 13bit Row bits
			(eCol_bit<<0) );		// 10bit Column bits

		// Memory Configuration Register 2
		Outp32(&DMC(uController)->rMEMCFG2,
			(1<<11) |		// Read delay 1 cycle from the Pad I/F ( 0x1 => mDDR)
			(3<<8)   |		// Memory Type (mDDR)
			(0<<6) |			// Width ( 16bit)
			(0<<4) |			// Bank bits = 2bit
			(0<<2) |			// DQM state
			(1<<0) );		// Sync. clock scheme

		// CHIP0 Configure
		Outp32(&DMC(uController)->rCHIP_0_CFG,  0x140fc); // Bank-Row-Column, 0x4000_0000 ~ 0x43ff_ffff ( 64MB)
			
	}
	else if(uController == 1)
	{
		// Memory Configuration Register
		Outp32(&DMC(uController)->rMEMCFG,
			(0<<31) |		// Disable Individul CKE Control
			(0<<21) |		// 1chip
			(0<<18) |		// Qos master selection by ARID[3:0]
			(2<<15) |		// Burst 4
			(eStopClk<<14) |		// Disable Stop Mem Clock ( 0)
			(eAutoPD<<13) |		// Disable Auto Power Down (0)
			(uPDprd<<7)   |		// Auto Power Down Period
			(eAP_bit<<6)	|		// Auto Precharge bit in bit 10
			(eRow_bit<<3)	|		// 13bit Row bits
			(eCol_bit<<0) );		// 10bit Column bits

		// Memory Configuration Register 2	
		Outp32(&DMC(uController)->rMEMCFG2,
			(1<<11) |		// Read delay 1 cycle from the Pad I/F ( 0x1 => mDDR)
			(3<<8)   |		// Memory Type (mDDR)
			(1<<6) |			// Width ( 32bit)
			(0<<4) |			// Bank bits = 2bit
			(0<<2) |			// DQM state
			(1<<0) );		// Sync. clock scheme

		// CHIP0 Configure
		Outp32(&DMC(uController)->rCHIP_0_CFG,  0x150f8); // Bank-Row-Column, 0x5000_0000 ~ 0x57ff_ffff ( 128MB)	
	}

	// Direct Command - External Memory Initialize

		// NOP	
		Outp32(&DMC(uController)->rDIRECTCMD,
			(0<<20) |		// Chip Address  - Chip 0
			(3<<18) );		// Command - NOP
		
		// Precharge All	
		Outp32(&DMC(uController)->rDIRECTCMD,
			(0<<20) |		// Chip Address  - Chip 0
			(0<<18) 	);		// Command - PALL	

		// AutoRefresh 2 times	
		Outp32(&DMC(uController)->rDIRECTCMD,
			(0<<20) |		// Chip Address  - Chip 0
			(1<<18) );		// Command - Autorefresh
				
		Outp32(&DMC(uController)->rDIRECTCMD,
			(0<<20) |		// Chip Address  - Chip 0
			(1<<18) );		// Command - Autorefresh

		// MRS	
		Outp32(&DMC(uController)->rDIRECTCMD,
			(0<<20) |		// Chip Address  - Chip 0
			(2<<18) |		// Command - MRS/EMRS
			(0<<16) |		// [17:16] - MRS ( 0 )
			(0x32<<0));		//  [6:4]- CAS Latency - 3, 
			   				// [3] - Burst Type (Sequential), [2:0] - Burst Length(4) -> 8
		
		// EMRS	
		Outp32(&DMC(uController)->rDIRECTCMD,
			(0<<20) |		// Chip Address  - Chip 0
			(2<<18) |		// Command - MRS/EMRS
			(2<<16) |		// [17:16] - EMRS (2)
			(0x0<<0));		// [6:5]- Full Strength, 
			   				// [2:0] - PASR - Full Array
			

		//'GO'  Mode
		Outp32(&DMC(uController)->rMEMCCMD, (0x0<<0));	// Go Command	

		// Check Controller State
		while((Inp32(&DMC(uController)->rMEMSTAT))&0x3 == 1  );
		
	
}




//////////
// Function Name : Test SPI
// Function Description : This function initializes a certain DMC Controller.(for SMDK6410)
//					  DMC0 => X16, mDDR (512Mb),  DMC1=> X16*2, mDDR (1025Mb)
// Input : 	uController - DMC Controller No. 
// Output : 	
//			
// Version : v0.1
void DMC_RdSTATUS (u8 uController)
{

	u32 uBaseAddress, uTemp;
	
	if(uController == 0)
	{
		uBaseAddress = DMC0_BASE;
	}
	else if(uController == 1)
	{
		uBaseAddress = DMC1_BASE;
	}
	else
	{
		
	}
	
	g_DMCBase[uController] = (void *)uBaseAddress;
	uTemp = Inp32(&DMC(uController)->rMEMSTAT);
	uTemp= uTemp & 0x3;

	DisplayLED(uTemp);
	
}




