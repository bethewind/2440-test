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
*	File Name : register_addr.h
*  
*	File Description : This file defines the register address to test 
*                      Register Read&Write
*
*	Author : Heemyung.Noh
*	Dept. : AP Development Team
*	Created Date : 2007/02/07
*	Version : 0.1 
* 
*	History
*	- Created(Heemyung.Noh 2007/02/07)
*  
**************************************************************************************/
#ifndef __REGISTER_ADDR_H__
#define __REGISTER_ADDR_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "sfr6410.h"
#include "def.h"

// Register Read&Write Test
#define		RO			(0x00)
#define		WO			(0x01)
#define		RW			(0x11)
#define		DPDB		(0x00)		//Default Pattern,		Default BitMask
#define		DPPB		(0x01)		//Default Pattern, 		Private BitMask
#define		PPDB		(0x10)		//Private Pattern, 		Default BitMask
#define		PPPB		(0x11)		//Private Pattern, 		Private BitMask

typedef  struct st_REG
{
	s8 name[64];
	u32 uAddr;
	u8 uBitLen;
	u8 uRWType;	
	u8 uFlag;						//Option Flag(DPDB, DPPB, PPDB PPPB)
	u32 uPrivateBitMask;
	u32 rValue;	
}REGINFO;


void RPRINTF(REGINFO sReg, u32 uPattern, u8 uRegStatus, s32 Nth,u8 uInDetail);
void TestSFR(void);

#ifdef __cplusplus
}
#endif

#endif /*__LIBRARY_H__*/

