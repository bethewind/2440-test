/**************************************************************************************
* 
*	Project Name : S3C6410 Validation
*
*	Copyright 2007 by Samsung Electronics, Inc.
*	All rights reserved.
*
*	Project Description :
*		This software is only for validating functions of the S3C6410.
*		Anybody can use this software without our permission.
*  
*--------------------------------------------------------------------------------------
* 
*	File Name : tvenc.h
*  
*	File Description : This file implements the functons for TV Encoder controller header.
*
*	Author : Heemyung.noh
*	Dept. : AP Development Team
*	Created Date : 2007/01/15
*	Version : 0.1 
* 
*	History
*	- Created(Heemyung.noh 2007/01/15)
*  
**************************************************************************************/


#ifndef __TVENC_H__
#define __TVENC_H__

typedef enum
{
	eBlackStretch, eWhiteStretch, eBlueStretch
} eSTRETCH_COLOR;

typedef enum
{
	eCOMPOSITE, eS_VIDEO
} eTV_CONN_TYPE;

typedef enum
{
	eBLACK, eBLUE, eRED, eMAGENTA, eGREEN, eCYAN, eYELLOW, eWHITE
} eBG_COLOR;

typedef enum
{
	eMUTE_Y, eMUTE_CB, eMUTE_CR
} eMUTE_TYPE;

typedef enum
{
	eAGC4L, eAGC2L, eN01, eN02, eP01, eP02
}eMACROPATTERN;

void TVENC_TurnOnOff(u8 uOnOff);
void TVENC_SetImageSize(u32 uWSize, u32 uHSize);
void TVENC_ClearUnderrunInt(void);

void TVENC_SetTvConMode(TV_STANDARDS tvmode, eTV_CONN_TYPE tvout);
void TVENC_SetDemoWinSize(u32 uHsz, u32 uVsz, u32 uHst, u32 uVst);
void TVENC_SetEncPedestal(u8 bOnOff);
void TVENC_SetSubCarrierFreq(u32 uFreq);
void TVENC_SetFscDTO(u32 uVal);
void TVENC_DisableFscDTO(void);
void TVENC_SetBackGround(u8 bSmeUsed, u32 uColNum, u32 uLumaOffset);
void TVENC_SetBgVavHav(u32 uHavLen, u32 uVavLen, u32 uHavSt, u32 uVavSt);

void TVENC_SetHuePhase(u32 uInc);
u32 TVENC_GetHuePhase(void);
void TVENC_SetContrast(u32 uContrast);
u32 TVENC_GetContrast(void);
void TVENC_SetBright(u32 uBright);
u32 TVENC_GetBright(void);
void TVENC_SetCbGain(u32 uCbGain);
u32 TVENC_GetCbGain(void);
void TVENC_SetCrGain(u32 uCrGain);
u32 TVENC_GetCrGain(void);
void TVENC_EnableMacroVision(TV_STANDARDS eTvmode, eMACROPATTERN ePattern);
void TVENC_DisableMacroVision(void);
void TVENC_EnableGammaControl(u8 bEnable);
void TVENC_SetGammaGain(u32 uGamma);
u32 TVENC_GetGammaGain(void);
void TVENC_EnableMuteControl(u8 bEnable);
void TVENC_SetMuteYCbCr(u32 uY, u32 uCb, u32 uCr);
void TVENC_GetMuteYCbCr(u32 *uY, u32 *uCb, u32 *uCr);
void TVENC_GetActiveWinCenter(u32 *uVer, u32 *uHor);
void TVENC_SetActiveWinCenter(u32 uVer, u32 uHor);
void TVENC_EnableEnhancerDemoWindow(u8 bEnable);
void TVENC_GetEnhancerDemoWindow(u32 *uVWinSize, u32 *uVStart, u32 *uHWinSize, u32 *uHStart);
void TVENC_SetEnhancerDemoWindow(u32 uVWinSize, u32 uVStart, u32 uHWinSize, u32 uHStart);
void TVENC_EnableBackground(u8 bEnable);
void TVENC_GetBackground(u32 *uColor, u32 *uHStart, u32 *uVStart, u32 *uHVisualSize, u32 *uVVisualSize);
void TVENC_SetBackground(u32 uColor, u32 uHStart, u32 uVStart, u32 uHVisualSize, u32 uVVisualSize);
void TVENC_SetSharpness(u32 uSharpness);
u32 TVENC_GetSharpness(void);
void TVENC_DisplayTVout(TV_STANDARDS eTvmode, eTV_CONN_TYPE eTvout, u32 uSizeX, u32 uSizeY);
	

#endif //__TV_H__

