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
*	File Name : watchdogtimer.h
*  
*	File Description : This file implements the API functons for WDT 
*
*	Author : DAEDOO
*	Dept. : AP Development Team
*	Created Date : 2008/03/11
*	Version : 0.0 
* 
*	History
*	- Created(DAEDOO 2008/03/11)
*  
**************************************************************************************/
#include <stdio.h>
#include <stdlib.h>

#include "option.h"
#include "library.h"
#include "sfr6410.h"
#include "system.h"
#include "watchdogtimer.h"
#include "intc.h"
#include "sysc.h"

 volatile u32 g_uIntCounterWT = 0;

//////////
// Function Name : Isr_WDT
// Function Description : This function is Interrupt Service Routine of WatchDog Timer
// Input : NONE
// Output : NONE (increment of g_uIntCounterWT value)
// Version : v0.1
void __irq Isr_WDT(void)
{

	Outp32(rWTCLRINT ,1);    // Watchdog timer interrupt clear 	
	INTC_Disable(NUM_WDT);
	
	g_uIntCounterWT++;
	UART_Printf("%d ", g_uIntCounterWT);
	 
    	INTC_ClearVectAddr();    
	INTC_Enable(NUM_WDT);
}

//////////
// Function Name : WDT_operate
// Function Description : This function operate Watchdog Timer by Input
// Input :	uEnReset, uEnInt, uEnWDT [0:Disable		1:Enbale]
//			uSelectCLK (clk division factor) [0:16		1:32		2:64		3:128]
//			uPrescaler [1~256]
//			uWTDAT [0~2^15]
//			uWTCNT [0~2^15]   
// Output : NONE
// Version : v0.1

#if 1

#if 0
void WDT_Init(u32 uReset, u32 uInt, u32 uDiv, u32 uEnWdt, u32 uPrescaler, u32 uDat, u32 uCnt)
{

	float fWDTclk;
	
	Outp32(rWTCON,0);
	Outp32(rWTDAT,0);
	Outp32(rWTCNT,0);
	Outp32(rWTDAT,uDat);
	Outp32(rWTCNT,uCnt);

	fWDTclk = (1/(float)((float)g_PCLK/((float)uPrescaler+1)/(1<<(uDiv+4))))*uDat;
	UART_Printf("WDT_clk = %f sec\n",fWDTclk);

	Outp32(rWTCON,(uReset<<0)|(uInt<<2)|(uDiv<<3)|(uEnWdt<<5)|((uPrescaler)<<8));

}
#else



void WDT_Init(u32 uPreVal, DIV_FACTOR div_factor, u32 uWdtDat)
{
	u32 div_clk;
	float wdt_time;

	switch (div_factor)
	{
		case DIV16:
			div_clk = CLKSEL_16;
			break;
		case DIV32:
			div_clk = CLKSEL_32;
			break;
		case DIV64:
			div_clk = CLKSEL_64;
			break;
		case DIV128:
			div_clk = CLKSEL_128;
			break;
	}

	Outp32(rWTCON, PREVAL(uPreVal) | div_clk);
	Outp32(rWTDAT, uWdtDat);
	Outp32(rWTCNT, uWdtDat);

}

void WDT_Enable(u8 bData)
{
	u32 uRead;

	uRead = Inp32(rWTCON);
	Outp32(rWTCON, uRead&~(1<<5)|(bData<<5));
}

void WDT_IntEnable(u8 bData)
{
	u32 uRead;

	uRead = Inp32(rWTCON);
	Outp32(rWTCON, uRead&~(1<<2)|(bData<<2));

}

void WDT_EnableReset(u8 bData)
{
	u32 uRead;

	uRead = Inp32(rWTCON);
	Outp32(rWTCON, uRead&~(1<<0)|(bData<<0));
}


//////////
// Function Name : CalibrateDelay
// Function Description : This function calibrates delayLoopCount as about 100us by using WDT
// Input : NONE 
// Output : NONE
// Version : 

extern u32 delayLoopCount;

void CalibrateDelay(void)
{
	u32 i, tmpDelay=100;
	u32 uPCLK_MHz=g_PCLK/1000000;
	
	delayLoopCount = 5000;
	
	Outp32(rWTCON, (uPCLK_MHz-1)<<8);
	Outp32(rWTDAT, 0xffff);						//for first update
	Outp32(rWTCNT, 0xffff);						//resolution=16us @any PCLK 
	Outp32(rWTCON, ((uPCLK_MHz-1)<<8)|(1<<5));	//Watch-dog timer start
	
	for(;tmpDelay>0;tmpDelay--)
		for(i=0;i<delayLoopCount;i++);
	
	Outp32(rWTCON, (uPCLK_MHz-1)<<8);   		//Watch-dog timer stop
	i = 0xffff - Inp32(rWTCNT);                 // 1count->16us, 100*5000 cycle runtime = 
	delayLoopCount = 50000000/(i*16);         	//100*5000:16*i[us]=1*x:100[us] -> x=50000000/(16*i)

}


#endif


#endif

