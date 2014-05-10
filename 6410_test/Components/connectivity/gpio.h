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
*	File Name : gpio.h
*  
*	File Description : This file declares prototypes of GPIO API funcions.
*
*	Author : Heemyung.Noh
*	Dept. : AP Development Team
*	Created Date : 2006/12/26
*	Version : 0.1 
* 
*	History
*	- Created(Heemyung.Noh 2006/12/26)
*     - Added EINT (Wonjoon.jang 2007/01/03)
*  
**************************************************************************************/

#ifndef __GPIO_H__
#define __GPIO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "def.h"


#define		Low_Level		0x0
#define		High_Level		0x1
#define		Falling_Edge	0x2
#define		Rising_Edge		0x4
#define		Both_Edge		0x6



typedef enum GPIO_Id
{
	// Value & 0xF000 : The muxing bit number of the configuration register
	// Value & 0x0F00 : Control register number per each GPIO Port
	// Value & 0x00FF : Offset from GPIO base address(Value * 0x10)
	
	eGPIO_A = 0x4100,    		// muxing bit : 4, 	control register : 1,	offset : 0, 		
	eGPIO_B = 0x4102,    		// muxing bit : 4, 	control register : 1,	offset : 0x02*0x10
	eGPIO_C = 0x4104,    		// muxing bit : 4, 	control register : 1,	offset : 0x04*0x10
	eGPIO_D = 0x4106,    		// muxing bit : 4, 	control register : 1,	offset : 0x06*0x10
	eGPIO_E = 0x4108,    		// muxing bit : 4, 	control register : 1,	offset : 0x08*0x10
	eGPIO_F = 0x210A,    		// muxing bit : 2, 	control register : 1,	offset : 0x0A*0x10
	eGPIO_G = 0x410C,    		// muxing bit : 4, 	control register : 1,	offset : 0x0C*0x10
	eGPIO_H = 0x420E,    		// muxing bit : 4, 	control register : 2,	offset : 0x0E*0x10
	eGPIO_I = 0x2110,    		// muxing bit : 2, 	control register : 1,	offset : 0x10*0x10
	eGPIO_J = 0x2112,    		// muxing bit : 2, 	control register : 1,	offset : 0x12*0x10
	
	eGPIO_K = 0x4280,    		// muxing bit : 4, 	control register : 2,	offset : 0x80*0x10
	eGPIO_L = 0x4281,    		// muxing bit : 4, 	control register : 2,	offset : 0x81*0x10
	eGPIO_M = 0x4182,    		// muxing bit : 4, 	control register : 1,	offset : 0x82*0x10
	eGPIO_N = 0x2183,    		// muxing bit : 2, 	control register : 1,	offset : 0x83*0x10
	
	eGPIO_O = 0x2114,    		// muxing bit : 2, 	control register : 1,	offset : 0x14*0x10
	eGPIO_P = 0x2116,    		// muxing bit : 2, 	control register : 1,	offset : 0x16*0x10
	eGPIO_Q = 0x2118   		// muxing bit : 2, 	control register : 1,	offset : 0x18*0x10
}
GPIO_eId;

typedef enum GPIO_BitPos
{
	eGPIO_0	= (u32)(0),
	eGPIO_1	= (u32)(1),
	eGPIO_2	= (u32)(2),
	eGPIO_3	= (u32)(3),
	eGPIO_4	= (u32)(4),
	eGPIO_5	= (u32)(5),
	eGPIO_6	= (u32)(6),
	eGPIO_7	= (u32)(7),
	eGPIO_8	= (u32)(8),
	eGPIO_9	= (u32)(9),
	eGPIO_10	= (u32)(10),
	eGPIO_11	= (u32)(11),
	eGPIO_12	= (u32)(12),
	eGPIO_13	= (u32)(13),
	eGPIO_14	= (u32)(14),
	eGPIO_15	= (u32)(15)
}GPIO_eBitPos;

/*
typedef enum INT_TYPE
{
	Low		= (u32) 0x0,
	High	= (u32) 0x1,
	Falling	= (u32) 0x2,
	Rising	= (u32) 0x4,
	Both	= (u32) 0x6
}INT_eTYPE;
*/

typedef enum FLT_TYPE
{
	eDisFLT	= (u32) 0x0,
	eEnFLT	= (u32) 0x1, 		// EINT Group 1~9
	eDLYFLT	= (u32) 0x2,		// 2'b10 => Filter Enable, Delay Filter
	eDIGFLT	= (u32) 0x3
}FLT_eTYPE;

typedef enum LCD_TYPE
{
	eHOSTIF	= 	0,
	eRGBIF		=	1,
	eYCbCr		=	2,
	eHidden		=	3
}LCD_eTYPE;

void GPIO_Init(void);
void GPIO_SetFunctionEach(GPIO_eId Id, GPIO_eBitPos eBitPos, u32 uFunction);
void GPIO_SetFunctionAll(GPIO_eId Id, u32 uValue0, u32 uValue1);
void GPIO_SetDataEach(GPIO_eId Id, GPIO_eBitPos eBitPos, u32 uValue);
void GPIO_SetDataAll(GPIO_eId Id, u32 uValue);
u32 GPIO_GetDataAll(GPIO_eId Id);
void GPIO_SetPullUpDownEach(GPIO_eId Id, GPIO_eBitPos eBitPos, u32 uValue);
void GPIO_SetPullUpDownAll(GPIO_eId Id, u32 uValue);
void GPIO_SetConRegSleepAll(GPIO_eId Id, u32 uValue);
void GPIO_SetPullUDSleepAll(GPIO_eId Id, u32 uValue);	

void GPIO_SetLCDType(LCD_eTYPE eLCDType);

// EINT Group0 => Wakeup Source
void GPIO_SetEint0(u32 uEINT_No , u32  eIntType, FLT_eTYPE eFltType,  u32 uFltWidth);
void GPIO_EINT0ClrPend(u32 uEINT_No );
void GPIO_EINT0DisMask(u32 uEINT_No );
void GPIO_EINT0EnMask(u32 uEINT_No );



// EINT Group 1
void GPIO_SetEint1(u32 uEINT_No , u32 uIntType, FLT_eTYPE eFltType,  u32 uFltWidth);
void GPIO_EINT1ClrPend(u32 uEINT_No );
void GPIO_EINT1DisMask(u32 uEINT_No );
void GPIO_EINT1EnMask(u32 uEINT_No );

// EINT Group 2
void GPIO_SetEint2(u32 uEINT_No , u32 uIntType, FLT_eTYPE eFltType,  u32 uFltWidth);
void GPIO_EINT2ClrPend(u32 uEINT_No );
void GPIO_EINT2DisMask(u32 uEINT_No );
void GPIO_EINT2EnMask(u32 uEINT_No );

// EINT Group 3
void GPIO_SetEint3(u32 uEINT_No , u32 uIntType, FLT_eTYPE eFltType,  u32 uFltWidth);
void GPIO_EINT3ClrPend(u32 uEINT_No );
void GPIO_EINT3DisMask(u32 uEINT_No );
void GPIO_EINT3EnMask(u32 uEINT_No );

// EINT Group 4
void GPIO_SetEint4(u32 uEINT_No , u32 uIntType, FLT_eTYPE eFltType,  u32 uFltWidth);
void GPIO_EINT4ClrPend(u32 uEINT_No );
void GPIO_EINT4DisMask(u32 uEINT_No );
void GPIO_EINT4EnMask(u32 uEINT_No );

// EINT Group 5
void GPIO_SetEint5(u32 uEINT_No , u32 uIntType, FLT_eTYPE eFltType,  u32 uFltWidth);
void GPIO_EINT5ClrPend(u32 uEINT_No );
void GPIO_EINT5DisMask(u32 uEINT_No );
void GPIO_EINT5EnMask(u32 uEINT_No );

// EINT Group 6
void GPIO_SetEint6(u32 uEINT_No , u32 uIntType, FLT_eTYPE eFltType,  u32 uFltWidth);
void GPIO_EINT6ClrPend(u32 uEINT_No );
void GPIO_EINT6DisMask(u32 uEINT_No );
void GPIO_EINT6EnMask(u32 uEINT_No );

// EINT Group 7
void GPIO_SetEint7(u32 uEINT_No , u32 uIntType, FLT_eTYPE eFltType,  u32 uFltWidth);
void GPIO_EINT7ClrPend(u32 uEINT_No );
void GPIO_EINT7DisMask(u32 uEINT_No );
void GPIO_EINT7EnMask(u32 uEINT_No );

// EINT Group 8
void GPIO_SetEint8(u32 uEINT_No , u32 uIntType, FLT_eTYPE eFltType,  u32 uFltWidth);
void GPIO_EINT8ClrPend(u32 uEINT_No );
void GPIO_EINT8DisMask(u32 uEINT_No );
void GPIO_EINT8EnMask(u32 uEINT_No );

// EINT Group 9
void GPIO_SetEint9(u32 uEINT_No , u32 uIntType, FLT_eTYPE eFltType,  u32 uFltWidth);
void GPIO_EINT9ClrPend(u32 uEINT_No );
void GPIO_EINT9DisMask(u32 uEINT_No );
void GPIO_EINT9EnMask(u32 uEINT_No );

//added by rb1004
void GPIO_SetMem0DrvStrength(u32 uDrvStr);


void __irq Isr_Eint9(void);
void __irq Isr_Eint10(void);
void __irq Isr_Eint11(void);

#ifdef __cplusplus
}
#endif

#endif //__GPIO_H__

