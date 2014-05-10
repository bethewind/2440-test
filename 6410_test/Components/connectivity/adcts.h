/**************************************************************************************
* 
*	Project Name : S3C6410 Validation
*
*	Copyright 2006~2008 by Samsung Electronics, Inc.
*	All rights reserved.
*
*	Project Description :
*		This software is only for validating functions of the S3C6410.
*		Anybody can use this software without our permission.
*  
*--------------------------------------------------------------------------------------
* 
*	File Name : adcts.h
*  
*	File Description : This file defines the register access function
*						and declares prototypes of adcts funcions
*
*	Author : Sunil,Roe
*	Dept. : AP Development Team
*	Created Date : 2007/1/2
*	Version : 0.1 
* 
*	History
*	- Created(Sunil,Roe 2007/1/2)
*	- Edited by OnPil,Shin (SOP) on 2008/03/05 ~ 
*  
**************************************************************************************/
#ifndef __ADCTS_H__
#define __ADCTS_H__
 
  
#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include "def.h"

#define	RESSEL12BIT			1	// A/D conversion   0: 10-bit,   1: 12-bit

#define	ENABLE					1
#define	DISABLE					0
#define	SELECT					1
#define	DESELECT				0

#define	STYLUS_DOWN			0
#define	STYLUS_UP				1

#define	ADCTS_TOUCH_INIT		2
#define	ADCTS_TOUCH_DOWN	0
#define	ADCTS_TOUCH_UP		1

#define	ADCTS_PRSCVL			10

typedef enum ADCTS_Id
{
	eADCTS_ADCCON = 0,
	eADCTS_ADCTSC = 1,
	eADCTS_ADCDLY = 2,
	eADCTS_ADCDAT0 = 3,
	eADCTS_ADCDAT1 = 4,
	eADCTS_ADCUPDN = 5,
	eADCTS_ADCCLRINT = 6,
	eADCTS_ADCCLRINTPNDNUP = 8,
}
eADCTS_Id;

typedef enum ADCTS_MeasureModeId
{
	eADCTS_MEASURE_NOP = 0,
	eADCTS_MEASURE_MANUAL_X = 1,
	eADCTS_MEASURE_MANUAL_Y = 2,
	eADCTS_MEASURE_AUTO_SEQ = 3,
	eADCTS_MEASURE_FOR_STYLUS = 4,
}
eADCTS_MeasureModeId;

typedef struct ADCTS_Inform
{	
	u32	uADCSampleData;	
	u8	ucADCflag;
	u8	ucTouchStatus;
}
ADCTS_oInform;

u8	ADCTS_Init(void);
//void ADCTS_Test(void);

//void ADC_ConvertADCNormallyInPolling(void);
//void ADC_ConvertADCNormallyInADCInterrupt(void);
//void ADC_CheckADCLinearity(void);
//void ADCTS_ConvertXYPosSeparatelyInPolling(void);
//void ADCTS_ConvertXYPosSequentiallyInPolling(void);
//void ADCTS_StylusCheckInPENUPDNInterrupt(void);
//void ADCTS_StylusTrackingUsingAutoConvPENDNIntADCPolling(void);

void __irq	ADC_NormalISR(void);
void __irq	ADCTS_StylusTrackingISR(void);
void __irq	ADCTS_StylusTrackingISR_Auto(void);

/*---------------------------------- APIs of rADCCON ---------------------------------*/
void ADC_InitADCCON(void);	
u8 ADC_IsEOCFlag(void);
void ADC_EnablePrescaler(u8);
void ADC_SetPrescaler(u8);
void ADC_SelectADCCh(u8);
void ADC_SelectStandbyMode(u8);
void ADC_EnableReadStart(u8);
void ADC_EnableStart(u8);
u8 ADC_IsAfterStartup(void);

/*---------------------------------- APIs of rADCTSC ---------------------------------*/
void ADCTS_SetMeasureMode(u8 );
void ADCTS_SetStylusDetectMode(u8 );
	
/*---------------------------------- APIs of rADCDLY ---------------------------------*/
void ADCTS_SetDelayClkSrc(u8 );
void ADCTS_SetDelay(u32 );

/*---------------------------------- APIs of rADCDATx ---------------------------------*/
u8 ADCTS_GetXPStylusIntState(void);
u8 ADCTS_GetYPStylusIntState(void);
u32 ADCTS_GetXPData(void);
u32 ADCTS_GetYPData(void);

/*---------------------------------- APIs of INT Clear Registers ---------------------------------*/
void ADCTS_SetADCClearInt(void);
void ADCTS_SetADCClearWKInt(void);

/*---------------------------------- APIs of general ADCTS ---------------------------------*/
u32 ADCTS_GetRegValue(u8);
void ADCTS_SetRegValue(u8, u32);

u8 ADCTS_AutoTest(void);		// Added by SOP on 2008/05/10

#ifdef __cplusplus
}
#endif

#endif /*__ADCTS_H__*/
