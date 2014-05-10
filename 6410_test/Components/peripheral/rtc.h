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
*	File Name : rtc.h
*  
*	File Description : This file implements the API functons for RTC(Real Time Clock).
*
*	Author : Woojin.Kim
*	Dept. : AP Development Team
*	Created Date : 2007/01/06
*	Version : 0.1 
* 
*	History
*	- Created(Woojin.Kim 2007/01/06)
*  
**************************************************************************************/


#ifndef __RTC_H__
#define __RTC_H__

#include "def.h"
#include "sfr6410.h"


enum RTC_SFR 
{
	rINTP       	= RTC_BASE + 0x30,
	rRTCCON     	= RTC_BASE + 0x40,
	rTICCNT     	= RTC_BASE + 0x44,
	rRTCALM     	= RTC_BASE + 0x50,
	rALMSEC     	= RTC_BASE + 0x54,
	rALMMIN     	= RTC_BASE + 0x58,
	rALMHOUR    	= RTC_BASE + 0x5c,
	rALMDATE    	= RTC_BASE + 0x60,
	rALMMON     	= RTC_BASE + 0x64,
	rALMYEAR    	= RTC_BASE + 0x68,
//	RTCRST     = RTC_BASE + 0x6c,
	rBCDSEC     	= RTC_BASE + 0x70,
	rBCDMIN     	= RTC_BASE + 0x74,
	rBCDHOUR    	= RTC_BASE + 0x78,
	rBCDDATE    	= RTC_BASE + 0x7c,
	rBCDDAY     	= RTC_BASE + 0x80,
	rBCDMON     	= RTC_BASE + 0x84,
	rBCDYEAR   	= RTC_BASE + 0x88,
	rCURTICCNT 	= RTC_BASE + 0x90
//	rRTCLVD     = RTC_BASE + 0x94
};

/*
#define	TICKCLKSEL_32768Hz,	(0x0<<4)
#define	TICKCLKSEL_16384Hz,	(0x1<<4)
#define	TICKCLKSEL_8192Hz, 	(0x2<<4)
#define	TICKCLKSEL_4096Hz, 	(0x3<<4)
#define	TICKCLKSEL_2048Hz, 	(0x4<<4)
#define	TICKCLKSEL_1024Hz, 	(0x5<<4)
#define	TICKCLKSEL_512Hz,  	(0x6<<4)
#define	TICKCLKSEL_256Hz,  	(0x7<<4)
#define	TICKCLKSEL_128Hz, 	(0x8<<4)
#define	TICKCLKSEL_64Hz,  	(0x9<<4)
#define	TICKCLKSEL_32Hz,  	(0xa<<4)
#define	TICKCLKSEL_16Hz,   	(0xb<<4)
#define	TICKCLKSEL_8Hz,    	(0xc<<4)
#define	TICKCLKSEL_4Hz,    	(0xd<<4)
#define	TICKCLKSEL_2Hz,    	(0xe<<4)
#define	TICKCLKSEL_1Hz     	(0xf<<4)
*/

#define InitYEAR    	(99)
#define InitMONTH   	(12)
#define InitDATE    	(31)
#define InitDAY     	(07)      //SUN:1 MON:2 TUE:3 WED:4 THU:5 FRI:6 SAT:7   
#define InitHOUR    	(23)
#define InitMIN     	(59)
#define InitSEC     	(55)

#define AlmYear  	(00)
#define AlmMon  	(01)
#define AlmDate  (01)
#define AlmHour  (00)
#define AlmMin    	(00)
#define AlmSec    (00)

#define ALARM	(1<<1)
#define TimeTic	(1<<0)

typedef enum eDIV_SUBCLK {

	CLK_32768Hz = 0,
	CLK_16384Hz,
	CLK_8192Hz,
	CLK_4096Hz,
	CLK_2048Hz,
	CLK_1024Hz,
	CLK_512Hz,
	CLK_256Hz,
	CLK_128Hz,
	CLK_64Hz,
	CLK_32Hz,
	CLK_16Hz,
	CLK_8Hz,
	CLK_4Hz,
	CLK_2Hz,
	CLK_1Hz
	
}TickTimerClk;


#if 0

void RTC_SetCON(u32 uEnTic, u32 uTicsel, u32 uRstCLK, u32 uSelCNT, u32 uSelCLK, u32 uEnRTC);
void RTC_SetTime(u32 uYear, u32 uMon, u32 uDate, u32 uDay, u32 uHour, u32 uMin, u32 uSec);
void RTC_Init(void);
void RTC_SetTickCNT(u32 uTickcnt);
void RTC_Print(void);
void RTC_PrintCURTICK(void);

void RTC_SetAlmTime(u32 uYear, u32 uMon, u32 uDate, u32 uHour, u32 uMin, u32 uSec);
void RTC_SetAlmEn(u32 enAlm, u32 enYear, u32 enMon, u32 enDate, u32 enHour,u32 enMin,u32 enSec);
void RTC_PrintAlm(void);

void RTC_SetLVD(u32 enLVD);
u32 RTC_PrintLVD(void);

void RTC_ClearIntP(u32 uSelectP);

void __irq Isr_RTC_Tick(void);
void __irq Isr_RTC_InspectTick(void);
void __irq Isr_RTC_Alm(void);

#else

/*
void __irq Isr_RTC_Tick(void);
void __irq Isr_RTC_InspectTick(void);
void __irq Isr_RTC_Alm(void);
*/

void RTC_ClearPending(u32 uPending);
void RTC_TimeInit(u32 uYear, u32 uMon, u32 uDate, u32 uDay, u32 uHour, u32 uMin, u32 uSec);
void RTC_Enable(u8 bData);
void RTC_TickTimeEnable(u8 bData);
void RTC_TickClkSelect(TickTimerClk TICKCLKSEL);
void RTC_ClkReset(u8 bData);
void RTC_ClkSelect(u8 bClk);
void RTC_TickCnt(u32 uTickCnt);

void RTC_AlarmTimeInit(u32 uYear, u32 uMon, u32 uDate, u32 uHour, u32 uMin, u32 uSec);
void RTC_AlarmEnable(u8 bAll, u8 bYear, u8 bMon, u8 bDate, u8 bHour, u8 bMin, u8 bSec);

void RTC_Print(void);





#endif


#endif // __RTC_H__
