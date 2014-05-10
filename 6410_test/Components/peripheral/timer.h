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
*	File Name : timer.c
*  
*	File Description : This file implements the API functons for Timer (PWM, WDT).
*
*	Author : Woojin.Kim
*	Dept. : AP Development Team
*	Created Date : 2006/12/27
*	Version : 0.1 
* 
*	History
*	- Created(Woojin.Kim 2006/12/27)
*  
**************************************************************************************/

#include "def.h"
#include "sfr6410.h"

#ifndef __TIMER_H__
#define __TIMER_H__

#ifdef __cplusplus
extern "C" {
#endif


#include "def.h"
#include "sfr6410.h"


enum TIMER_REG
{
	rTCFG0   = PWMTIMER_BASE+0x00,
	rTCFG1   = PWMTIMER_BASE+0x04,
	rTCON    = PWMTIMER_BASE+0x08,
	rTCNTB0  = PWMTIMER_BASE+0x0C,
	rTCMPB0  = PWMTIMER_BASE+0x10,
	rTCNTO0  = PWMTIMER_BASE+0x14,
	rTCNTB1  = PWMTIMER_BASE+0x18,
	rTCMPB1  = PWMTIMER_BASE+0x1C,
	rTCNTO1  = PWMTIMER_BASE+0x20,
	rTCNTB2  = PWMTIMER_BASE+0x24,
	rTCMPB2  = PWMTIMER_BASE+0x28,
	rTCNTO2  = PWMTIMER_BASE+0x2C,
	rTCNTB3  = PWMTIMER_BASE+0x30,
	rTCMPB3  = PWMTIMER_BASE+0x34,
	rTCNTO3  = PWMTIMER_BASE+0x38,
	rTCNTB4  = PWMTIMER_BASE+0x3C,
	rTCNTO4  = PWMTIMER_BASE+0x40,
	rTINT_CSTAT = PWMTIMER_BASE+0x44

/*
	rWTCON= WDT_BASE+0x00,
	rWTDAT = WDT_BASE+0x04,
	rWTCNT = WDT_BASE+0x08,
	rWTCLRINT = WDT_BASE+0x0C
*/	
	
};


typedef enum eDelay_measure
{
	micro	=0,
	milli		=1,
}
eDelay_M;

void PWM_UserSelect(void);
void PWM_Select(u32 uTimer, u32 uPrescaler, u32 uDivider, u32 uTCNTB, u32 uTCMPB, u32 uEnDz, u32 uDzlen, u32 uEnDMA, u32 uAutoreload, u32 uEnInverter,u32 uEnInt);
void PWM_stopAll(void);
void PWM_stop(u32 uNum);
u32 PWM_ReadrTCMPB(u32 uNum);
//void WDT_operate(u32 uEnReset, u32 uEnInt, u32 uSelectCLK, u32 uEnWDT, u32 uPrescaler,u32 uWTDAT, u32 uWTCNT);
void __irq Isr_TIMEROS(void);
void __irq Isr_TIMER0(void);
void __irq Isr_TIMER1(void);
void __irq Isr_TIMER2(void);
void __irq Isr_TIMER3(void);
void __irq Isr_TIMER4(void);
void __irq Isr_WDT(void);


void DelayfrTimer(eDelay_M cMeasure, u32 uDnum);
void StartTimer(u32 uTsel);
u32 StopTimer(u32 uTsel);
void CalibrateDelay(void);

#ifdef __cplusplus
}
#endif


#endif //__TIMER_H__
