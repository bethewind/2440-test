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
*	File Name : sblock.h
*  
*	File Description : This file implements the API functons for Security subsystem.
*				    This Test code is oriented by SA Verification Code
*
*	Author : wonjoon.jang
*	Dept. : AP Development Team
*	Created Date : 2007/01/18
*	Version : 0.1 
* 
*	History
*	- Created(wonjoon.jang 2007/01/18)
*  
**************************************************************************************/

#ifndef __SBLOCK_H__
#define __SBLOCK_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "def.h"

typedef enum
{
	AES, DES, HASH
} SBLK_TYPE;

typedef enum
{
	ECB, CBC, CTR, HMAC, SHA1, PRNG
} OPER_MODE;

typedef enum
{
	ENC, DEC
} DIR_SEL;

typedef enum
{
	LASTBYTE_1ST,
	LASTBYTE_2ND,
	LASTBYTE_3RD,
	LASTBYTE_4TH
} LVALID_BYTE;

typedef struct
{
	SBLK_TYPE m_eSblkType;
	OPER_MODE m_eOperMode;
} SBLK;

void SBLK_Init(SBLK_TYPE eSblkType, OPER_MODE eOperMode, DIR_SEL eDirSel);
void SBLK_SetFifo(u32 uRxMsgLen, u32 uRxBlkSz,
	u32 uTxMsgLen, u32 uTxBlkSz, LVALID_BYTE eRxLValid);
void SBLK_StartFifo(void);
void SBLK_PutDataToRxFifo(u32 uSrcAddr, u32 uSize);
u8 SBLK_IsFifoTransDone(void);
void SBLK_GetDataFromTxFifo(u32 uDstAddr, u32 uSize);
void SBLK_ClearIntPending(u32 uTXnRX);
void SBLK_SetSDMA(u32 uRxSrcAddr, u32 uRxSize, u32 uTxDstAddr, u32 uTxSize);
void SBLK_StartSDMA(void);
void SBLK_PutDataToInReg(u32 uSrcAddr, u32 uSize, LVALID_BYTE eLValid);
void SBLK_StartByManual(void);
void SBLK_GetDataFromOutReg(u32 uDstAddr, u32 uSize);
u8 SBLK_IsOutputReady(void);

void SBLK_GetAvailableRxFifoSize(u32* uSize);
void SBLK_Reset(void);

 void SBLK_DispFifoRxCtrl(u32 uFRx_Ctrl);
 void SBLK_DispFifoTxCtrl(u32 uFTx_Ctrl);
void SBLK_Init_Test(SBLK_TYPE eSblkType, OPER_MODE eOperMode, DIR_SEL eDirSel);
void SBLK_Init_Test2(SBLK_TYPE eSblkType, OPER_MODE eOperMode, DIR_SEL eDirSel);
void SBLK_Init_Test3(SBLK_TYPE eSblkType, OPER_MODE eOperMode, DIR_SEL eDirSel);
void SBLK_Init_T(SBLK_TYPE eSblkType, OPER_MODE eOperMode, DIR_SEL eDirSel);
void SBLK_GetDataFromOutReg_Test(u32 uDstAddr, u32 uSize);
void SBLK_PutDataToInReg1(u32 uSrcAddr, u32 uSize, LVALID_BYTE eLValid);

#ifdef __cplusplus
}
#endif

#endif 


