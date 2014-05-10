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
*	File Name : cf.c
*  
*	File Description : This file implements the driver functions for CF Controller.
*
*	Author : Sunil,Roe
*	Dept. : AP Development Team
*	Created Date : 2007/2/17
*	Version : 0.1 
* 
*	History
*	- Created(Sunil,Roe 2007/2/17)
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

//=========================================================================
//					          	          MUX_REG REGISTER AREA
//=========================================================================
/*---------------------------------- APIs of MUX_REG Registers   ---------------------------------*/
//////////
// Function Name : CF_SetMUXReg
// Function Description : 
//   This function changes internal IDE mode, output port, and card power enable of CF Controller.
// Input : 	eOutputEnFlag - Output port enable/disable
//			eCardPwrFlag	- Card power on/off		
//			eModeFlag	- PCCARD/TrueIDE mode
// Output : NONE
// Version : v0.1 
void CF_SetMUXReg(eCF_MUX_OUTPUT eOutputEnFlag, eCF_MUX_CARDPWR eCardPwrFlag, eCF_MUX_IDEMODE eModeFlag)
{
	Outp32(ATA_MUX, ((eOutputEnFlag << 2) | (eCardPwrFlag << 1) | (eModeFlag << 0)));
}

//////////
// Function Name : CF_SetEBI
// Function Description : 
//   This function sets EBI of Syscon for CF controller.
// Input : NONE
// Output : NONE
// Version : v0.1 
void CF_SetEBI(u8 ucOpMode)
{
	SYSC_CtrlCSMEM0(eCS_SROMC, eCS_SROMC, eCS_CFC, eCS_CFC);		// bank4_cfg->CF,bank5_cfg->CF 
	// Set Buffer Direction pin 
//	GPIO_SetFunctionEach(eGPIO_M, eGPIO_5, 3);	// Set XhiINTR pin as CF Data Direction
	GPIO_SetFunctionEach(eGPIO_B, eGPIO_4, 4);	// Set XirSDBW pin as CF Data Direction

 	if (ucOpMode == DIRECT_MODE)	
 	{
		UART_Printf(" * CF EBI MODE : Direct Mode\n\n");
		SYSC_IndepCF();		// select dedicated CF path (direct path)
 	}
	else
	{
		UART_Printf(" * CF EBI MODE : InDirect Mode\n\n");
		SYSC_EBICF();		// select EBI path (for Indirect path)
	}
}

//////////
// Function Name : CF_SetMem
// Function Description : 
//   This function sets up data to specified address of memory.
// Input : eSize - size of data
//		uDstAddr - address of destination
//		uValue - data value
// Output : NONE
// Version : v0.1 
void CF_SetMem(eACCESS_SIZE eSize, u32 uDstAddr, u32 uValue)
{
	switch(eSize)
	{
		case eBYTE :
//	UART_Printf("DstAddr:0x%x, Value:0x%x\n", uDstAddr, uValue);			
			Outp8(uDstAddr, uValue);
			break;
		case eHWORD :
			Outp16(uDstAddr, uValue);
			break;
		case eWORD :
			Outp32(uDstAddr, uValue);
			break;
	}
}

