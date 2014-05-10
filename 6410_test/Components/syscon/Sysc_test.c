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
*	File Name : sysc_test.c
*  
*	File Description : This file implements the functons for System controller test.
*
*	Author : Wonjoon Jang
*	Dept. : AP Development Team
*	Created Date : 200701/10
*	Version : 0.1 
* 
*	History
*	- Created(wonjoon.jang 2007/01/10)
*  
**************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#include "def.h"
#include "option.h"
#include "library.h"
#include "sfr6410.h"
#include "system.h"
#include "intc.h"
#include "gpio.h"
#include "sysc.h"

//#include	"power_test.h"
#include	"clock_test.h"
#include  "reset_test.h"
#include	"power_test.h"

extern void SYSCT_Previous(void);

void SYSC_CLKInform(void)
{
	u32 uTTT0, uTTT1;

	SYSC_CtrlCLKOUT( eCLKOUT_APLLOUT, 4);


	SYSC_GetClkInform();
	UART_InitDebugCh(0, 115200);
		
	uTTT0 = SYSC_RdLockDetect(eAPLL);
	uTTT1= SYSC_RdLockDetect(eMPLL);
	
	
	UART_Printf("\n\n");
	UART_Printf("***************************************************\n");
	UART_Printf("*       S3C6410 - Test firmware v0.1              *\n");
	UART_Printf("***************************************************\n");
	UART_Printf("System ID : Revision [%d], Pass [%d]\n", g_System_Revision, g_System_Pass);
	UART_Printf("ARMCLK: %.2fMHz  HCLKx2: %.2fMHz  HCLK: %.2fMHz  PCLK: %.2fMHz\n",(float)g_ARMCLK/1.0e6, (float)g_HCLKx2/1.0e6, (float)g_HCLK/1.0e6, (float)g_PCLK/1.0e6);
	UART_Printf("SYNC Mode : %d\n", g_SYNCACK);
	UART_Printf("APLL Lock :  %d,  MPLL_Lock: %d\n", uTTT0, uTTT1);
	UART_Printf("AMPLL: %.2fMHz  \n",(float)g_MPLL/1.0e6);
	UART_Printf("\n");    

	
}	



///////////////////////////////////////////////////////////////////////////////////
////////////////////                   Syscon Main Test                  /////////////////////////// 
///////////////////////////////////////////////////////////////////////////////////

const testFuncMenu sysc_menu[] =
{
	SYSCT_PowerMode,					"Power Mode Test",
	SYSCT_Clock,						"Clock Test",
	SYSCT_Reset,						"Reset Test",
	SYSC_STOP,							"STOP Test",
	SYSC_IDLE,							"IDLE Test",
	SYSC_SLEEP0,						"SLEEP Test",
	SYSC_DEEPSTOP,						"DEEP STOP Test",	
	SYSC_STOP_Config,					"Stop Config. Test",
//	SYSC_STOP_Debugging,				"STOP Debugging Test",	
	SYSC_STOP_Aging,					"STOP Aging",
	SYSC_IDLE_AGING,					"IDLE Aging",
	SYSC_DEEPSTOP_Aging,				"DeepStop Aging",
//	SYSCT_Previous,						"PreviousProblem",
	SYSC_WDT_Reset,					"WatchDog Reset",
	
	SYSC_IDLE_Current,					"IDLE Current",
	SYSC_STOP_Current,					"STOP Current",
	SYSC_STOP_Current2,				"STOP Current2",
	SYSC_SLEEP_Current,				"SLEEP Current",
	SYSC_DEEPSTOP_Current,				"DEEPSTOP Current",	
	0, 0
};

void SYSC_Test(void)
{
	u32 i;
	s32 uSel;

	UART_Printf("[SYSC_Test]\n\n");

	Outp32Inform(0,0);
	Outp32Inform(1,0);
	Outp32Inform(2,0);
	Outp32Inform(3,0);
	Outp32Inform(4,0);
	Outp32Inform(5,0);
	Outp32Inform(6,0);
	Outp32Inform(7,0);
	
	while(1)
	{
		for (i=0; (u32)(sysc_menu[i].desc)!=0; i++)
			UART_Printf("%2d: %s\n", i, sysc_menu[i].desc);

		UART_Printf("\nSelect the function to test : ");
		uSel =UART_GetIntNum();
		UART_Printf("\n");
		if(uSel == -1) 
			break;

		if (uSel>=0 && uSel<(sizeof(sysc_menu)/8-1))
			(sysc_menu[uSel].func) ();
	}
}	


