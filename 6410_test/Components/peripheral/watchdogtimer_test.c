/**************************************************************************************
* 
*	Project Name : S3C6400 Validation
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
*	File Name : Timer_test.c
*  
*	File Description : This file implements the API functons for WDT test.
*
*	Author : DAEDOO
*	Dept. : AP Development Team
*	Created Date : 2008/03/11
*	Version : 0.0 
* 
*	History
*	- Created(DAEDOO, 2008/03/11)
*  
**************************************************************************************/


/* Include Files */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>


#include "def.h"
#include "option.h"
#include "library.h"
#include "sfr6410.h"
#include "system.h"
#include "watchdogtimer.h"
#include "sysc.h"
#include "intc.h"

extern volatile u32 g_uIntCounterWT;
	
void WDT_Test(void);
void WDT_NormalTimer(void);
void WDT_Reset(void);
void WDT_FullTest(void);

const testFuncMenu g_aWDTTestFunc[] =
{	

	WDT_NormalTimer,		"WDT Normal Interrupt   ",
	WDT_Reset,				"WDT Reset       ",
	WDT_FullTest,			"WDT Full Test		",	
	0,0
};

void WDT_Test(void)
{
	u32 uCountFunc=0;
	s32 iSel=0;
	
	while(1)
	{

	UART_Printf("\n\n================== Timer Function Test =====================\n\n");

		for (uCountFunc=0; (u32)(g_aWDTTestFunc[uCountFunc].desc)!=0; uCountFunc++)
			UART_Printf("%2d: %s\n", uCountFunc, g_aWDTTestFunc[uCountFunc].desc);

		UART_Printf("\nSelect the function to test : ");
		iSel =UART_GetIntNum();
		UART_Printf("\n");
		if(iSel == -1) 
			break;

		if (iSel>=0 && iSel<(sizeof(g_aWDTTestFunc)/8-1))
			(g_aWDTTestFunc[iSel].func) ();
	}
}	

////////////
// File Name : WDT_IntReq
// File Description : This function checks WDT Interrupt by changing division factor, prescaler value & WTDAT
// Input : NONE
// Output : NONE
// Version : 
void WDT_NormalTimer(void)
{
	DIV_FACTOR div_factor;
	u32 uPreVal, uClkDiv, uWdtDat = 0xffff;
	uPreVal = ((float)g_PCLK/1000000-1);

	// WDT INT Init //
	g_uIntCounterWT = 0;
	INTC_SetVectAddr(NUM_WDT,Isr_WDT);	
	INTC_Enable(NUM_WDT);

	// WDT Init
	WDT_EnableReset(false);
	WDT_IntEnable(false);
	WDT_Enable(false);

	Disp("Input Prescaler Value [0 ~ 255] : ");
	uPreVal = UART_GetIntNum();

	Disp("Select Clock Division Factor (1:16, 2:32, 3:64, 4:128) : ");
	uClkDiv = UART_GetIntNum();

	Disp("Press anykey to start WDTIMER..\n");
	UART_Getc();

	if (uClkDiv == 1)
		div_factor = DIV16;
	else if (uClkDiv == 2)
		div_factor = DIV32;
	else if (uClkDiv == 3)
		div_factor = DIV64;
	else if (uClkDiv == 4)
		div_factor = DIV128;

	WDT_Init(uPreVal, div_factor, uWdtDat);

	WDT_EnableReset(false);
	WDT_IntEnable(true);
	WDT_Enable(true);

	while(g_uIntCounterWT != 10); // variable

	INTC_Disable(NUM_WDT);
	
	WDT_IntEnable(false);
	WDT_Enable(false);

}


////////////
// File Name : WDT_Rest
// File Description : This function checks watchdog reset
// Input : NONE
// Output : NONE
// Version : 
void WDT_Reset(void)
{
	u32 uPreVal, uClkDiv, uWdtDat = 0x8888;
	DIV_FACTOR div_factor;

	// WDT INT Init //

	g_uIntCounterWT = 0;
	uPreVal = ((float)g_PCLK/1000000-1);
	
	INTC_SetVectAddr(NUM_WDT,Isr_WDT);	
	INTC_Enable(NUM_WDT);

	WDT_EnableReset(false);
	WDT_IntEnable(false);
	WDT_Enable(false);

	Disp("Input Prescaler Value [0 ~ 255] : ");
	uPreVal = UART_GetIntNum();

	Disp("Select Clock Division Factor (1:16, 2:32, 3:64, 4:128) : ");
	uClkDiv = UART_GetIntNum();

	Disp("Press anykey to start WDT Reset Test\n");
	UART_Getc();

	if (uClkDiv == 1)
		div_factor = DIV16;
	else if (uClkDiv == 2)
		div_factor = DIV32;
	else if (uClkDiv == 3)
		div_factor = DIV64;
	else if (uClkDiv == 4)
		div_factor = DIV128;

	WDT_Init(uPreVal, div_factor, uWdtDat);

	WDT_EnableReset(true);
	WDT_IntEnable(true);
	WDT_Enable(true);

	UART_Getc();

	INTC_Disable(NUM_WDT);

	WDT_EnableReset(false);
	WDT_IntEnable(false);
	WDT_Enable(false);

}

void WDT_FullTest(void)
{
	u32 uPreVal, uClkDiv, uWdtDat;
	DIV_FACTOR div_factor;

	UART_Printf("WDT Full Function Test!!!\nPress any key.....");
	UART_Getc();
	UART_Printf("\n");
		
	g_uIntCounterWT = 0;
	uPreVal = ((float)g_PCLK/1000000-1);
	UART_Printf("PCLK = %.2fMHz, prescaler = %d\n", (float)g_PCLK/1000000, uPreVal);

	INTC_SetVectAddr(NUM_WDT,Isr_WDT);	
	INTC_Enable(NUM_WDT);

	WDT_EnableReset(false);
	WDT_IntEnable(false);
	WDT_Enable(false);

	uPreVal = 0;
	div_factor = DIV16;
	uWdtDat = 0x1000;
	while(uPreVal <= PreValMax)
	{
		UART_Printf("\nClock Division Factor: %d(dec), Prescaler: %d(dec)\n",(1<<(uClkDiv+4)),uPreVal);
		
		WDT_Init(uPreVal, div_factor, uWdtDat);
		WDT_EnableReset(false);
		WDT_IntEnable(true);
		WDT_Enable(true);

		while(g_uIntCounterWT != 10); // variable
		uPreVal = uPreVal + 10;
		g_uIntCounterWT = 0;

	}

	uPreVal = 0;
	div_factor = DIV32;
	uWdtDat = 0x1000;
	while(uPreVal <= PreValMax)
	{
		UART_Printf("\nClock Division Factor: %d(dec), Prescaler: %d(dec)\n",(1<<(uClkDiv+4)),uPreVal);
		
		WDT_Init(uPreVal, div_factor, uWdtDat);
		WDT_EnableReset(false);
		WDT_IntEnable(true);
		WDT_Enable(true);

		while(g_uIntCounterWT != 10); // variable
		uPreVal = uPreVal + 10;
		g_uIntCounterWT = 0;

	}

	uPreVal = 0;
	div_factor = DIV64;
	uWdtDat = 0x1000;
	while(uPreVal <= PreValMax)
	{
		UART_Printf("\nClock Division Factor: %d(dec), Prescaler: %d(dec)\n",(1<<(uClkDiv+4)),uPreVal);
		
		WDT_Init(uPreVal, div_factor, uWdtDat);
		WDT_EnableReset(false);
		WDT_IntEnable(true);
		WDT_Enable(true);

		while(g_uIntCounterWT != 10); // variable
		uPreVal = uPreVal + 10;
		g_uIntCounterWT = 0;

	}

	uPreVal = 0;
	div_factor = DIV128;
	uWdtDat = 0x1000;
	while(uPreVal <= PreValMax)
	{
		UART_Printf("\nClock Division Factor: %d(dec), Prescaler: %d(dec)\n",(1<<(uClkDiv+4)),uPreVal);
		
		WDT_Init(uPreVal, div_factor, uWdtDat);
		WDT_EnableReset(false);
		WDT_IntEnable(true);
		WDT_Enable(true);

		while(g_uIntCounterWT != 10); // variable
		uPreVal = uPreVal + 10;
		g_uIntCounterWT = 0;

	}

	UART_Printf("WDT Reset Test....\n");

	uPreVal = 100;
	div_factor = DIV16;
	uWdtDat = 0x100;
	
	WDT_Init(uPreVal, div_factor, uWdtDat);
	WDT_EnableReset(true);
	WDT_IntEnable(true);
	WDT_Enable(true);

	while(g_uIntCounterWT<1);

	INTC_Disable(NUM_WDT);
	WDT_IntEnable(false);
	WDT_Enable(false);

}



