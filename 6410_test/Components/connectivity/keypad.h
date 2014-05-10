/**************************************************************************************
* 
*	Project Name : S3C6410 Validation
*
*	Copyright 2008 by Samsung Electronics, Inc.
*	All rights reserved.
*
*	Project Description :
*		This software is only for validating functions of the S3C6410.
*		Anybody can use this software without our permission.
*  
*--------------------------------------------------------------------------------------
* 
*	File Name : keypad.h
*  
*	File Description : This file defines prototype of KEYPAD APIs
*
*	Author : Sung-Hyun, Na
*	Dept. : AP Development Team
*	Created Date : 2008/02/15
*	Version : 0.1 
* 
*	History
*	- Created(Sung-Hyun, Na 2008/02/15)
*  
**************************************************************************************/

#ifndef __KEYPAD_SOFT_H__
#define __KEYPAD_SOFT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include "def.h"
#include "sfr6410.h"

#define KEYIF_COLNUM		8
#define KEYIF_ROWNUM		8


enum KEYPAD_SFR
{
	rKEYIFCON		= 0x0,
	rKEYIFSTSCLR		= 0x4,
	rKEYIFCOL		= 0x8,
	rKEYIFROW		= 0xc,
	rKEYIFFC			= 0x10
};

#define KEYIFOutp32(offset, x) 			Outp32(KEYPADIF_BASE+offset, x)
#define KEYIFInp32(offset) 				Inp32(KEYPADIF_BASE+offset)

#define KEYPAD_EN_WakeupINT		(1 << 4)
#define KEYPAD_EN_FilterCounter		(1 << 3)
#define KEYPAD_EN_DebounceFilter		(1 << 2)
#define KEYPAD_EN_RisingINT			(1 << 1)
#define KEYPAD_EN_FallingINT			(1 << 0)

#define KEYPAD_ST_FallingINT			(0xff << 0)
#define KEYPAD_ST_RigingINT			(0xff << 8)

#define KEYPAD_ST_KeyCol				(0xff << 0)
#define KEYPAD_EN_KeyCol				(0xff << 8)

#define KEYPAD_ST_KeyRow			(0xff << 0)

typedef enum
{
	KeyPort0,
	KeyPort1,
}KEYPORT;

typedef enum
{
	DF_RTCCLK	= 0,
	DF_OSC		= 1
}DFSOURCE;

typedef enum
{
	Key_Falling 	= 1,
	Key_Rising	= 2,
	Key_Both		= 3,
}KEY_INTSOURCE;

typedef struct 
{
	KEYPORT 		m_ePort;
	DFSOURCE		m_eDFilterSource;
	KEY_INTSOURCE	m_eIntSource;
	u32				m_uWakeup;
	u32				m_uFilterEn;
	u32 				m_uColEn;	
	u32				m_uFilterVal;
}KEYPAD_INFOR;

typedef struct testFuncKEYPAD {
	void (*func)(KEYPAD_INFOR *); 
	const char *desc;
} testFuncKEYPAD;

void KEYPAD_Init(void);
KEYPAD_INFOR* KEYPAD_GetInfor(void);

void __irq Isr_KEYPAD(void);
void __irq Isr_KEYPAD_WakeUp(void);


void KEYPAD_SetPort(KEYPAD_INFOR* rhs_pInfor);
void KEYPAD_InitPort(KEYPORT ePort);
void KEYPAD_ClosePort(KEYPORT ePort);

void KEYPAD_SetKEYIFCON(KEYPAD_INFOR* rhs_pInfor);

void KEYPAD_Sel_Wakeup(KEYPAD_INFOR* rhs_pInfor);
void KEYPAD_EnableWakeup(u32);

void KEYPAD_SetFilterCNT(KEYPAD_INFOR* rhs_pInfor);
void KEYPAD_EnableFilterCNT(u32);

void KEYPAD_SetDBFilter(KEYPAD_INFOR* rhs_pInfor);
void KEYPAD_EnableDBFilter(u32 uSel);

void KEYPAD_SetINT(KEYPAD_INFOR* rhs_pInfor);
void KEYPAD_SelectINT(KEY_INTSOURCE eSel, u32 uEnable);

void KEYPAD_SelectDFCNT(KEYPAD_INFOR* rhs_pInfor);

void KEYPAD_STSCLR(u8 ucP_INT, u8 ucR_INT);
u32 KEYIFSTSCLR_GetReg(void);

void KEYPAD_COL(u8 ucCOL,u8 ucCOLLEnable);
u32 KEYPAD_ROW(void);

#ifdef __cplusplus
}
#endif

#endif

