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
*	File Name : RTC_test.c
*  
*	File Description : This file implements the API functons for RTC test.
*
*	Author : DAEDOO
*	Dept. : AP Development Team
*	Created Date : 2008/03/17
*	Version : 0.1 
* 
*	History
*	- Created(DAEDOO 2008/03/17)
*  
**************************************************************************************/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "rtc.h"
#include "def.h"
#include "option.h"
#include "library.h"
#include "sfr6410.h"
#include "system.h"
#include "sysc.h"
#include "intc.h"
//#include "gpio.h"
//#include "timer.h"

u32 uCntTick=false; 
u32 uAlarm=false;


void __irq Isr_RTC_Tick(void)
{
	RTC_ClearPending(TimeTic);
	//RTC_ClearPending(0);
	
	uCntTick = true;
	
	INTC_ClearVectAddr();
}


//////////
// Function Name : Isr_RTC_InspectTick
// Function Description : This function is Interrupt Service Routine of RTC Tick Timer for checking CURTICKCNT
// Input : NONE
// Output : NONE
// Version : v0.1

void __irq Isr_RTC_InspectTick(void)
{
	INTC_Disable(NUM_RTC_TIC);
	RTC_ClearPending(TimeTic);
	
	uCntTick = 1;
	UART_Printf("Tick INT Occured!!\n");
	
	INTC_ClearVectAddr();
}

//////////
// Function Name : Isr_RTC_Alm
// Function Description : This function is Interrupt Service Routine of RTC Alarm
// Input : NONE
// Output : NONE
// Version : v0.1

void __irq Isr_RTC_Alm(void)
{
	INTC_Disable(NUM_RTC_ALARM);
	RTC_ClearPending(ALARM);
	
	uAlarm=true;
	UART_Printf("\nRTC Alarm Occur!!!\n");	

	INTC_ClearVectAddr();
}

void RTC_RealTimeDisplay(void)
{
	RTC_Enable(true);
	RTC_TickTimeEnable(true);
	
   	while(!UART_GetKey())
	{
		RTC_Print();
	}
	
	RTC_TickTimeEnable(false);
	RTC_Enable(false);
}


void RTC_DisplayAndClkOut(void)
{

	uCntTick = false;

	RTC_TimeInit(8,5,9,6,10,48,55);	// 9 -> 0X10	
	
	RTC_TickClkSelect(CLK_16384Hz);	
	RTC_TickCnt((0x8000>>CLK_16384Hz));

	SYSC_CtrlCLKOUT(eCLKOUT_RTC,0);    	

	INTC_SetVectAddr(NUM_RTC_TIC,Isr_RTC_Tick);
	INTC_Enable(NUM_RTC_TIC);

	RTC_Enable(true);
	RTC_TickTimeEnable(true);

   	while(!UART_GetKey())
	{
		while(!uCntTick);	// Wait Tick Interrupt
		RTC_Print();
		uCntTick = 0;
	}

	RTC_TickTimeEnable(false);
	INTC_Disable(NUM_RTC_TIC);
	RTC_Enable(false);	
	
}

void RTC_TimeTick(void)
{
	u32 uSelect;
	uCntTick = false;

	RTC_TimeInit(8,03,31,2,23,59,55);

	UART_Printf("[0]1/32768\t [1]:1/16384\t[2]1/8192\t[3]1/4096\n[4]1/2048\t [5]1/1024\t[6]1/512\t[7]1/256\n[8]1/128\t [9]1/64\t[10]1/32\t[11]1/16\n[12]1/8\t\t [13]1/4\t[14]1/2\t\t[15]1/1\n\n");
	UART_Printf("Select Tick Source Clock [0~15] : ");

	uSelect=UART_GetIntNum();

	if (uSelect == -1)
	Assert(0);
		
	UART_Printf("\n");

	RTC_TickClkSelect((TickTimerClk)uSelect);	
	RTC_TickCnt((0x8000>>uSelect));

	SYSC_CtrlCLKOUT(eCLKOUT_TICK,0);    

	INTC_SetVectAddr(NUM_RTC_TIC,Isr_RTC_Tick);
	INTC_Enable(NUM_RTC_TIC);

	RTC_TickTimeEnable(true);
	RTC_Enable(true);

   	while(!UART_GetKey())
	{
		while(!uCntTick);	// Wait Tick Interrupt
		RTC_Print();
		uCntTick = 0;
	}
	
	RTC_TickTimeEnable(false);
	RTC_Enable(false);
	INTC_Disable(NUM_RTC_TIC);
	
}

void RTC_Alarm(void)
{
	uAlarm = false;
	
	RTC_TimeInit(InitYEAR, InitMONTH, InitDATE, InitDAY, InitHOUR, InitMIN, InitSEC);
	RTC_AlarmTimeInit(AlmYear, AlmMon, AlmDate, AlmHour, AlmMin, AlmSec);
	RTC_AlarmEnable(true, true, true, true, true, true, true);
	
	INTC_SetVectAddr(NUM_RTC_ALARM,Isr_RTC_Alm);
	INTC_Enable(NUM_RTC_ALARM);

	RTC_Print();
	UART_Printf("After 5sec, Alarm Interrupt Occur\n");
	
	while(uAlarm==false);

	RTC_Print();
	UART_Printf("\nRTC alarm test OK\n");
	
	INTC_Disable(NUM_RTC_ALARM);
}


const testFuncMenu g_aRTCTestFunc[] =
{
	RTC_RealTimeDisplay,		"RTC Display		",
	RTC_DisplayAndClkOut,	"RTC Time Display and ClockOut		",
	RTC_TimeTick,			"RTC Time Tick Int Test        ",	
	RTC_Alarm,				"RTC Alarm Test	",
//	RTC_inspect_tick,			"RTC Tick Count check",
	0,0
};

void RTC_Test(void)
{
	u32 uCountFunc=0;
	s32 iSel=0;

	while(1)
	{
	UART_Printf("\n\n================== RTC Function Test =====================\n\n");
	
		for (uCountFunc=0; (u32)(g_aRTCTestFunc[uCountFunc].desc)!=0; uCountFunc++)
			UART_Printf("%2d: %s\n", uCountFunc, g_aRTCTestFunc[uCountFunc].desc);

		UART_Printf("\nSelect the function to test : ");
		iSel =UART_GetIntNum();
		UART_Printf("\n");
		if(iSel == -1) 
			break;

		if (iSel>=0 && iSel<(sizeof(g_aRTCTestFunc)/8-1))
			(g_aRTCTestFunc[iSel].func) ();
	}
}	





