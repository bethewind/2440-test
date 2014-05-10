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
*	File Name : rotator.h
*  
*	File Description : This is header file for rotator.c
*
*	Author : Sekwang Kim
*	Dept. : AP Development Team
*	Created Date : 2007/01/25
*	Version : 0.1a 
* 
*	History
*	-
* 	-
*  
**************************************************************************************/


#ifndef __ROTATOR_H__
#define __ROTATOR_H__

#ifdef __cplusplus
extern "C" {
#endif

#define ROTATOR_PROFILE
//#define NATURALIMAGE


#ifdef NATURALIMAGE
#include "sun320240_16bpp.h"
#include "sun320240_24bpp.h"
#include "sun240320_16bpp.h"
#include "sun240320_24bpp.h"
#endif


#define SINGLE_WINDOW			(1)
#define DOUBLE_WINDOW			(2)
#define ROTATOR_TEST_MODE		SINGLE_WINDOW

#define ROTATOR_BIT_NONE			(0<<4)
#define ROTATOR_BIT_ROT_90			(4<<4)
#define ROTATOR_BIT_ROT_180		(8<<4)
#define ROTATOR_BIT_ROT_270		(0xc<<4)
#define ROTATOR_BIT_FLIP_VER		(0x2<<4)
#define ROTATOR_BIT_FLIP_HOR		(0x3<<4)

#define ROTATOR_BIT_IN_YC420		(0<<13)
#define ROTATOR_BIT_IN_YC422		(3<<13)
#define ROTATOR_BIT_IN_RGB565		(4<<13)
#define ROTATOR_BIT_IN_RGB888		(5<<13)

#define ROTATOR_BIT_START			(1<<0)
#define ROTATOR_BIT_STOP			(0<<0)

#define ROTATOR_BIT_INT_EN			(1<<24)
#define ROTATOR_BIT_INT_DIS		(0<<24)





typedef enum
{
	ROTATOR_TYPE_NONE, 
	ROTATOR_TYPE_ROT_90,
	ROTATOR_TYPE_ROT_180,
	ROTATOR_TYPE_ROT_270,
	ROTATOR_TYPE_FLIP_VER,
	ROTATOR_TYPE_FLIP_HOR	
} ROTATOR_ROT_TYPE;




typedef struct
{
	u8 m_bRotInit;
	u8 m_bIntEn;
	u16 m_usInFormat;
	u8 m_ucRotType;
	u8 m_bStartRot;
	u32 m_uSrcAddr0;
	u32 m_uSrcAddr1;
	u32 m_uSrcAddr2;	
	u16 m_usVerSize;
	u16 m_usHorSize;

	u32 m_uDstAddr0;
	u32 m_uDstAddr1;
	u32 m_uDstAddr2;

	u32 m_uLcdFbAddr0;
	u32 m_uLcdFbAddr1;

	u32 m_uLcdWidth0;
	u32 m_uLcdWidth1;
	u32 m_uLcdHeight0;
	u32 m_uLcdHeight1;

	u32 m_uLcdOffsetX0;
	u32 m_uLcdOffsetX1;
	u32 m_uLcdOffsetY0;
	u32 m_uLcdOffsetY1;	
	
} ROTATOR;

void ROTATOR_GetFrmSz(u16 *usRotHorSize, u16 *usRotVerSize);
void ROTATOR_GetFbAddr(u32 *uRotSrcAddr, u32 *uRotDstAddr);
void ROTATOR_GetBppMode(CSPACE *eBpp);
void ROTATOR_GetLcdFbAddr(u32 *uLcdFbAddr0, u32 *uLcdFbAddr1);



void ROTATOR_DrawImage(CSPACE eBpp, u16 usRotHorSize, u16 usRotVerSize, u32 uRotSrcAddr);
u8 ROTATOR_CompFbData(CSPACE eBpp, ROTATOR_ROT_TYPE eRotType, u32 uSrcWidth, u32 uSrcHeight, u32 uInX, u32 uInY, 
	                                                 u32 uRotWidth, u32 uRotHeight, u32 uRotX, u32 uRotY, u32 uSrcFbAddr, u32 uRotFbAddr);
void ROTATOR_GetRotateCoordinate(u32 uSrcX, u32 uSrcY, u32 uSrcWidth, u32 uSrcHeight, ROTATOR_ROT_TYPE eRotType, 
	                                                               u32 *uRotX, u32 *uRotY, u32 *uRotWidth, u32 *uRotHeight);
u8 ROTATOR_AuditOperation(CSPACE eBpp, u32 uWidth, u32 uHeight, ROTATOR_ROT_TYPE eRotType, 
	                                                    u32 uInFbAddr, u32 uOutFbAddr);


void ROTATOR_ConvertColorSpace(CSPACE eInFmt, u32 uInFbAddr, u32 uWidth, u32 uHeight, CSPACE eOutFmt, u32 uOutFbAddr);
void ROTATOR_InitLcd(LCD_WINDOW eWin, CSPACE eBpp);

void ROTATOR_CheckDone(void);
void ROTATOR_CheckIdle(void);
void ROTATOR_IntEnable(void);
void ROTATOR_IntDisable(void);
void __irq Isr_Rotator(void);
void __irq Isr_RotatorOneMoreJob(void);
void ROTATOR_CheckIntDone(void);




void ROTATOR_SetAddr (CSPACE eBpp, u32 uFbAddr, u16 usHorSize, u16 usVerSize);
void ROTATOR_SetSrcAddr (CSPACE eBpp, u32 uFbAddr, u16 usHorSize, u16 usVerSize);
void ROTATOR_SetDstAddr(CSPACE eBpp, u32 uFbAddr, u16 usHorSize, u16 usVerSize);
void ROTATOR_SetSrcSize(u16 usHorSize, u16 usVerSize);
void ROTATOR_SetInFormat(CSPACE eBpp);
void ROTATOR_SetRotType(ROTATOR_ROT_TYPE eRotType);
void ROTATOR_Start(void);

void ROTATOR_InitRegs(void);
void ROTATOR_InitSetting(CSPACE eBgBpp, u16 usRotHorSize, u16 usRotVerSize, u32 uRotSrcAddr, u32 uRotDstAddr, ROTATOR_ROT_TYPE eRotType);
void ROTATOR_Init(u8 ucTestMode);

#ifdef __cplusplus
}
#endif

#endif /*__ROTATOR_H__*/
