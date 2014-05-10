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
*	File Name : power_test.h
*  
*	File Description : This file declares prototypes of Power Mode Test funcions.
*
*	Author : Wonjoon Jang
*	Dept. : AP Development Team
*	Created Date : 2007/01/10
*	Version : 0.1 
* 
*	History
*	- Created(Wonjoon.jang  2007/01/10)
*  
**************************************************************************************/

#ifndef __POWER_TEST_H__
#define __POWER_TEST_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "def.h"

void InitData_SDRAM(int check_start, int check_size);
void CheckData_SDRAM(int check_start, int check_size);

void SetAlarmWakeup (void);
void SetEintWakeup (void);
//void SetBatteryFaultEvent(BATFLT_eMODE eBATFLT);

void IDLET_Basic (void);
void IDLET_Aging (void);
void STOPT_Basic (void);
void DEEPSTOP_Basic (void);
void STOPT_CFG(void);
void SLEEPT_Basic (void);
void ESLEEPT_Basic (void);
void NORCFG_Test (void);
void CLKGate_Test(void);

void SYSCT_PowerMode(void);

void __irq Isr_WUBATF(void);
void __irq Isr_WUEint9(void);
void __irq Isr_WUEint10(void);
void __irq Isr_WUEint11(void);


#ifdef __cplusplus
}
#endif

#endif 


