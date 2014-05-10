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


#ifndef __WDT_H__
#define __WDT_H__


#ifdef __cplusplus
extern "C" {
#endif

#include "def.h"
#include "sfr6410.h"

enum WDT_REG
{
	rWTCON		= WDT_BASE+0x00,
	rWTDAT 	= WDT_BASE+0x04,
	rWTCNT 		= WDT_BASE+0x08,
	rWTCLRINT 	= WDT_BASE+0x0C
	
};

// Test Variable
#define PreValMax	0xFF

// Control Register
#define PREVAL(n)			(((n)&0xFF)<<8)
#define WDT_DISABLE		(0<<5)
#define WDT_ENABLE			(1<<5)
#define CLKSEL_16			(0<<3)
#define CLKSEL_32			(1<<3)
#define CLKSEL_64			(2<<3)
#define CLKSEL_128			(3<<3)
#define INT_DISABLE			(0<<2)
#define INT_ENABLE			(1<<2)
#define RST_DISABLE			(0<<0)
#define RST_ENABLE			(1<<0)

typedef enum eDIV_FACTOR {
	DIV16, DIV32, DIV64, DIV128
}DIV_FACTOR;

//void WDT_operate(u32 uEnReset, u32 uEnInt, u32 uSelectCLK, u32 uEnWDT, u32 uPrescaler,u32 uWTDAT, u32 uWTCNT);
void __irq Isr_WDT(void);

#if 1

void WDT_Init(u32 uPreVal, DIV_FACTOR div_factor, u32 uWdtDat);
void WDT_Enable(u8 bData);
void WDT_IntEnable(u8 bData);
void WDT_EnableReset(u8 bData);
void CalibrateDelay(void);

#endif

#ifdef __cplusplus
}
#endif


#endif //__WDT_H__
