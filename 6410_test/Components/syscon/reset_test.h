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
*	File Name : reset_test.h
*  
*	File Description : This file declares prototypes of reset Mode Test funcions.
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

#ifndef __RESET_TEST_H__
#define __RESET_TEST_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "def.h"


void Test_WarmReset(void);
void Test_SoftReset(void);

void SYSCT_Reset(void);




#ifdef __cplusplus
}
#endif

#endif 


