/**************************************************************************************
* 
*	Project Name : S3C6400 Validation
*
*	Copyright 2006 by Samsung Electronics, Inc.
*	All rights reserved.
*
*	Project Description :
*		This software is only for validating functions of the S3C6400.
*		Anybody can use this software without our permission.
*  
*--------------------------------------------------------------------------------------
* 
*	File Name : dmc.h
*  
*	File Description : This file implements the API functons for DRAM controller.
*
*	Author : Wonjoon.jang
*	Dept. : AP Development Team
*	Created Date : 2007/01/05
*	Version : 0.1 
* 
*	History
*	- Created(Wonjoon.jang 2007/01/05)
*  
**************************************************************************************/
#ifndef __DMC_H__
#define __DMC_H__


#ifdef __cplusplus
extern "C" {
#endif

void DMC_ChangeStateFromLowPowerToReady(void);
void DMC_ChangeStateFromReadyToLowPower(void);
void DMC_ChangeSDRAMParameter(unsigned int uMemType, unsigned int uMemClk);
void DMC_PreChangeSDRAMParameter(void);

#ifdef __cplusplus
}
#endif

#endif //__DMC_H__



