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
*	File Name : clock_test.h
*  
*	File Description : This file declares prototypes of clock module Test funcions.
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

#ifndef __CLOCK_TEST_H__
#define __CLOCK_TEST_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "def.h"

void MemoryRWTest(void);

void Test_Locktime(void);
void Test_ManualChangePLL(void);
void Test_OnOffPll(void);
void Test_ManChangeCLKDivider(void);
void Test_CLKOUT(void) ;
void Test_ChangeOPMode(void) ;
void Test_PMS_Value_Change(void);
void Test_PMS_Value_Change(void);
void Test_DIVISION_Value_Change_Sync(void);
void Test_DIVISION_Value_Change_ASync(void);
void Test_DIVISION_Value_Change_Sync_Sep(void);
void Test_Divider_Manual(void);
void Test_DIVISION_withExternal(void);



void SYSCT_Clock(void);




#ifdef __cplusplus
}
#endif

#endif 


