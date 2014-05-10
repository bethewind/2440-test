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
*	File Description : This file implements the API functons for Static Memory Controller
*
*	Author : Wonjoon Jang
*	Dept. : AP Development Team
*	Created Date : 2007/01/06
*	Version : 0.1 
* 
*	History
*	- Created(Wonjoon.jang 2007/01/06)
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
#include "sromc.h"
#include "Otg_dev.h"

#define 	SROMC 		( ( volatile oSROMC_REGS * ) ( g_SROMCBase ) )
	
// define global variables
static void *			g_SROMCBase;
//SFR
typedef struct tag_SROMC_REGS
{
	u32 rSROM_BW;		// 0x70000000 
	u32 rSROM_BC0;
	u32 rSROM_BC1;
	u32 rSROM_BC2;
	u32 rSROM_BC3;
	u32 rSROM_BC4;
	u32 rSROM_BC5;
			
} 
oSROMC_REGS;


void SROMC_Init(void)
{
	g_SROMCBase = (void *)SROM_BASE;
}


void	SROMC_SetBusWidth(SROMC_eBANK uBank, Data_eWidth eDWidth)
{
	u32 uConValue;

	// Bus Width & Wait Control
	uConValue = Inp32(&SROMC->rSROM_BW);
	uConValue = (uConValue & ~(0x1<<(uBank*4))) | (eDWidth<<(uBank*4));
	Outp32(&SROMC->rSROM_BW, uConValue);	
}

void	SROMC_SetWaitControl(SROMC_eBANK uBank, WAIT_eCTL eWAITCTL)
{
	u32 uConValue;

	// Bus Width & Wait Control
	uConValue = Inp32(&SROMC->rSROM_BW);
	uConValue = (uConValue & ~(0x1<<(uBank*4+2))) | (eWAITCTL<<(uBank*4+2));
	Outp32(&SROMC->rSROM_BW, uConValue);	
}


void	SROMC_SetByteEnable(SROMC_eBANK uBank, Byte_eCTL eByteCTL)
{
	u32 uConValue;

	// Bus Width & Wait Control
	uConValue = Inp32(&SROMC->rSROM_BW);
	uConValue = (uConValue & ~(0x1<<(uBank*4+3))) | (eByteCTL<<(uBank*4+3));
	Outp32(&SROMC->rSROM_BW, uConValue);	
}

void SROMC_SetTiming(	SROMC_eBANK uBank, Bank_eTiming eTacs, Bank_eTiming eTcos,Bank_eTiming eTacc, 
							 Bank_eTiming eTcoh, Bank_eTiming eTcah, Bank_eTiming eTacp, Page_eMode ePage)
{
	u32 uConValue;
	volatile u32 *pSROMC_BC_Addr;
	
	pSROMC_BC_Addr = &(SROMC->rSROM_BC0);
	pSROMC_BC_Addr = pSROMC_BC_Addr + uBank;
	
	uConValue = (eTacs<<28)|(eTcos<<24)|((eTacc-1)<<16)|(eTcoh<<12)|(eTcah<<8)|(eTacp<<4)|(ePage<<0);
	*pSROMC_BC_Addr = uConValue;
}

//////////
// Function Name : SROMC_SetBank
// Function Description : This function initializes SROMC Banks.
// Input : 		uBank : Selected Bank. No.
//				eByteCTL : UL/BL Control for the selected Bank.
//				eWAITCTL: WAIT Control for the selected Bank.
//				eDWidth  :  Data Width Control for the selected Bank.
// 				ePage	:  Page Mode Control for the selected Bank.
//				eTacs	: Tacs Control for the selected Bank.
//				eTcos	: Tcos Control for the selected Bank.
//				eTacc	: Tacc Control for the selected Bank.
//				eTcoh	: Tcoh Control for the selected Bank.
//				eTcah	: Tcah Control for the selected Bank.
//				eTacp	: Tacp Control for the selected Bank.
// Output : 	
//			
// Version : v0.1
void	SROMC_SetBank(u8 uBank, Byte_eCTL eByteCTL, WAIT_eCTL eWAITCTL, Data_eWidth eDWidth, Page_eMode ePage,
	Bank_eTiming eTacs, Bank_eTiming eTcos,Bank_eTiming eTacc, Bank_eTiming eTcoh, Bank_eTiming eTcah, Bank_eTiming eTacp)
{
	u32 uBaseAddress = 0;
	u32 uConValue  = 0;
	volatile u32 *pSROMC_BC_Addr = NULL;
	volatile u32 *pSROMC_BW_Addr = NULL;

	// Bus Width & Wait Control
	pSROMC_BW_Addr = &(SROMC->rSROM_BW);
	uConValue = Inp32(&SROMC->rSROM_BW);
	uConValue = (uConValue & ~(0xF<<(uBank*4))) | (eDWidth<<(uBank*4))|(eWAITCTL<<(uBank*4 + 2))|(eByteCTL<<(uBank*4+3));
	*pSROMC_BW_Addr = uConValue;
	// Bank Control Register
	pSROMC_BC_Addr = &(SROMC->rSROM_BC0);
	pSROMC_BC_Addr = pSROMC_BC_Addr + uBank;
	uConValue = (eTacs<<28)|(eTcos<<24)|((eTacc-1)<<16)|(eTcoh<<12)|(eTcah<<8)|(eTacp<<4)|(ePage<<0);
	*pSROMC_BC_Addr = uConValue;
}


