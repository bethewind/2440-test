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
*	File Name : cameram.c
*  
*	File Description : Camera module setting code
*
*	Author : 
*	Dept. : AP Development Team
*	Created Date : 
*	Version : 0.1 
* 
*	History
*	- Created
*  
**************************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "system.h"
#include "cameram.h"
#include "iic.h"
#include "intc.h"
#include "option.h"
#include "library.h"
#include "sfr6410.h"
#include "sysc.h"
#include "Camera.h"

CAMERA oCamera;




void CAMERA_IICOpen(int Frequency)
{
	IIC_Open(Frequency);
}


void CAMERA_IICClose(void)
{
	IIC_Close();
}

//////////
// Function Name : CAMERA_Read_Write
// Function Description : Camera Module 2M / 1.3M SFR Read Write
// Input : 	u32 uCAMTYPE(0: S5K3AAE / 1:S5K3BAF) ,CAM_ATTR eCcir(601or656Format), CSPACE eSrcFmt(Camera Source Firmat)
//                  SUB_SAMPLING eSub(S5K3VAF module Camera Image Setting Value)
// Output :    	None
void CAMERA_Read_Write(u32 uCAMTYPE,CAM_ATTR eCcir, CSPACE eSrcFmt, SUB_SAMPLING eSub)
{
	u8 x;
	u32 uOutSel;
	u32 uYcSel;
	u32 uCrCbSel;
	
	CAMERA_IICOpen(300000);

	switch ( uCAMTYPE )
	{
		case 0:
			oCamera.m_ucSlaveAddr = 0x5a;

			CAMERA_SetReg(0xec, 0x00);
			//	CAMERA_SetReg(0x72, 0x7d);	//25 Mhz MCLK Setting
			//	CAMERA_SetReg(0x72, 0xc8);	// 40 Mhz MCLK Setting
			CAMERA_SetReg(0x72, 0x64);	// 20 Mhz MCLK Setting
			//	CAMERA_SetReg(0x72, 0x3c);	// 12.5 Mhz MCLK Setting
	
			CAMERA_SetReg(0xec, 0x00);
			CAMERA_GetReg(0x72, &x);
			//	UART_Printf("\nCheck 25Mhz Setting = 0x7D ---->  %x", x);
			UART_Printf("\nCheck 20Mhz Setting = 0x64 ---->  0x%x", x);	
	
			CAMERA_SetReg(0xec, 0x07);
			CAMERA_SetReg(0x37, 0x00);
			//	CAMERA_SetReg(0x37, 0x5);	// 12.5 Mhz Setting

			CAMERA_SetReg(0xec, 0x00);
			CAMERA_GetReg(0x02, &x);
			UART_Printf("\nImage Size Setting is 0x00 ===> 0x%x(0:SXGA 1:VGA)\n",  x);
			
			Delay(20000);

			// Camera data format, YC order setting
			uOutSel = (eCcir == CCIR601) ? (1<<2) : (0<<2);
			uYcSel = (eSrcFmt == YCBYCR || eSrcFmt == YCRYCB) ? (1<<1) : (0<<1);
			uCrCbSel = (eSrcFmt == YCRYCB || eSrcFmt == CRYCBY) ? (1<<0) : (0<<0);
			CAMERA_SetReg(0xec, 0x01);
			//	CAMERA_SetReg(0x6a, uOutSel | uYcSel | uCrCbSel);
			CAMERA_SetReg(0x6a, uOutSel | uYcSel | 1);

			CAMERA_SetReg(0xec, 0x01);
			CAMERA_GetReg(0x6a, &x);
			UART_Printf("\nImage Data 0x01======> 0x%x", x);

			break;
		case 1:
			CAMERA_SetReg(0xfc, 0x01);
			CAMERA_SetReg(0x03, 0x01); // sw reset
			//PLL SETTING MCLK = 27Mhz PLL_CLK = 64 PCLK = 32
			CAMERA_SetReg(0xfc, 0x01);
			CAMERA_SetReg(0x04, 0x03); //PCLK = PLL / 2^1

			CAMERA_SetReg(0xfc, 0x01);
			CAMERA_GetReg(0x04, &x); // PCLK Setting Value
			UART_Printf("\nPCLK Divider Setting Value 0x03 = 0x%x", x);
			
			CAMERA_SetReg(0xfc, 0x02);
			CAMERA_SetReg(0x52, 0x80);  //PLL M=128d

			CAMERA_SetReg(0xfc, 0x02);
			CAMERA_GetReg(0x52, &x); // PLL _M = 128
			UART_Printf("\nPLL_M Value 128 = 0x%d", x);
			
			CAMERA_SetReg(0xfc, 0x02);
			CAMERA_SetReg(0x50, 0x54); // PLL_S = 2 

			CAMERA_SetReg(0xfc, 0x02);
			CAMERA_GetReg(0x50, &x); // P , S Value
			UART_Printf("\nP/S Setting Value 0x54 = 0x%x", x);
			
			CAMERA_SetSizeInS5K3BAF(eSub);
			/// Camera data format, YC order setting
			uOutSel = (eCcir == CCIR601) ? (1<<2) : (0<<2);
			uYcSel = (eSrcFmt == YCBYCR || eSrcFmt == YCRYCB) ? (1<<1) : (0<<1);
			uCrCbSel = (eSrcFmt == YCRYCB || eSrcFmt == CRYCBY) ? (1<<0) : (0<<0);
			CAMERA_SetReg(0xfc, 0x01);
			CAMERA_SetReg(0x02, uOutSel|uYcSel|uCrCbSel);

			CAMERA_SetReg(0xfc, 0x01);
			CAMERA_GetReg(0x02, &x); // Image Size Setting Value
			UART_Printf("\nImage Size Setting  Value 0x07 = 0x%x\n\n", x);
			

			break;
		default : 
			break;
	}

	CAMERA_IICClose();
}


void CAMERA_SetReg_4CA(u16 ucSubAddr, u16 ucData)
{
	u16 D[2];

	D[0] = ucSubAddr;
	D[1] = ucData;
//	IIC_Write(oCamera.m_ucSlaveAddr, D[0], D[1]);
//	IIC_WriteA16D16(oCamera.m_ucSlaveAddr, D[0], D[1]);				
}

// add. jungil 080630
void CAMERA_GetReg_4CA(u16 ucSubAddr, u16 ucData)
{
	u16 D[2];
	u16 * RD[2];

	D[0] = ucSubAddr;
	D[1] = ucData;
//	IIC_Write(oCamera.m_ucSlaveAddr, D[0], D[1]);
//	IIC_ReadA16D16_2(oCamera.m_ucSlaveAddr, D[0], RD[0]);				
}


//////////
// Function Name : CAMERA_SetReg
// Function Description : camera module register setting
// Input : 	u8 ucSubAddr(adress), u8 ucData(data)
// Output :    	None
#if 0
void CAMERA_SetReg(u8 ucSubAddr, u8 ucData)
{
	u8 D[2];

	D[0] = ucSubAddr;
	D[1] = ucData;
	IIC_Write(oCamera.m_ucSlaveAddr, D[0], D[1]);
//	IIC_SetWrite(oCamera.m_ucSlaveAddr, D, 2);	
//	IIC_Wait();
}
#else // for Debugg
void CAMERA_SetReg(u8 ucSubAddr, u8 ucData)
{
	u8 D[2];
	u8 x;

	D[0] = ucSubAddr;
	D[1] = ucData;
//	IIC_SetWrite(oCamera.m_ucSlaveAddr, D, 2);
	IIC_Write(oCamera.m_ucSlaveAddr, D[0], D[1]); // modify S.O.P G
//	IIC_Wait();

//	delayLoop(100000);

//	if ( ucSubAddr != 0xef)
	if ( ucSubAddr != 0xfc)		
	{
		CAMERA_GetReg(ucSubAddr, &x);
		if ( ucData != x )
		{
			UART_Printf("Addr = %x, Value=%x, Real=%x\n", ucSubAddr, ucData, x);
		}
	}
	else
	{
		UART_Printf("-------------------Page = , %x\n", ucData);

	}
}
#endif



void CAMERA_SetSizeInS5K4BAF(SUB_SAMPLING eSub)
{
	u8 x;
	u8 ucSub;

	oCamera.m_ucSlaveAddr = 0x5a;
	/// Size subsampling setting
	ucSub = (eSub == SUB_SAMPLING0) ? 0 :
				(eSub == SUB_SAMPLING2) ? 9 :
				(eSub == SUB_SAMPLING4) ? 11 :
				0xff;
	Assert(ucSub < 0xff);
	CAMERA_SetReg(0xfc, 0x00);
	CAMERA_SetReg(0x02, ucSub);
}



//////////
// Function Name : CAMERA_GetReg
// Function Description : get camera module data
// Input : 	None
// Output :    	None
#if 0 
void CAMERA_GetReg(u8 ucSubAddr, u8* ucData)
{
	IIC_SetWrite( oCamera.m_ucSlaveAddr, &ucSubAddr, 1);
	IIC_SetRead( oCamera.m_ucSlaveAddr, ucData, 1);
	IIC_Wait();
}
#else
void CAMERA_GetReg(u8 ucSubAddr, u8* ucData)
{
	IIC_SetWrite( oCamera.m_ucSlaveAddr, &ucSubAddr, 1);
	IIC_SetRead( oCamera.m_ucSlaveAddr, ucData, 1);
	IIC_Wait();
}
#endif




//////////
// Function Name : CAMERA_InitOV7620
// Function Description : cameramodule setting OV7620
// Input : 	None
// Output :    	None
void CAMERA_InitOV7620(CAM_ATTR eCcir, u32 uBits, u8 bInv, CSPACE eSrcFmt)
{
	u8 ucVal;

//	IIC_Init();
	oCamera.m_ucSlaveAddr = 0x42;
	
//	CAMERA_SetReg(0x12, 0x80);	// Camera Soft reset. Self cleared after reset.
//	Delay(100);

	CAMERA_SetReg(0x00, 0x00);	// AGC gain
	CAMERA_SetReg(0x01, 0x80);	// blue gain
	CAMERA_SetReg(0x02, 0x80);	// red gain

	CAMERA_SetReg(0x03, 0xb0);	// saturation control
	CAMERA_SetReg(0x06, 0x60);	// set brightness - CJH
	CAMERA_SetReg(0x0c, 0x24);	// set white balance blue background
	CAMERA_SetReg(0x0d, 0x24);	// set white balance red background
	CAMERA_SetReg(0x10, 0xff);	// set auto exposure time, (not used in auto adjust mode)

	ucVal = 0;
	if (bInv)
	{
		ucVal |= (0<<7);
		ucVal |= (1<<6);
	}
	else
	{
		ucVal |= (1<<7);
		ucVal |= (0<<6);
	}
	ucVal |= (0<<0);
	CAMERA_SetReg(0x11, ucVal);

	ucVal = 0;
	if ( eSrcFmt == YCBYCR || eSrcFmt == YCRYCB)
		ucVal |= (1<<4); 
	ucVal |= (1<<2); // enable white balnace
	ucVal |= (1<<5); // enable AGC
	CAMERA_SetReg(0x12, ucVal);

	ucVal = 0;
	ucVal |= (uBits == 8 ? 1 : 0)<<5;
	ucVal |= (eCcir == CCIR656 ? 1 : 0)<<4;
	CAMERA_SetReg(0x13, ucVal);

	Delay(10);

	ucVal |= 0;
	if (eSrcFmt == CBYCRY || eSrcFmt == YCBYCR)
		ucVal |= (1<<0);
	ucVal |= (bInv  ? 1 : 0)<<6;
	CAMERA_SetReg(0x15, ucVal);

	Delay(10);	
	
	CAMERA_SetReg(0x16, 0x03);	// Field mode : 11b-FRAME mode -> FD<7:2> is useless (default)
	CAMERA_SetReg(0x17, 0x2f);	// 47 -> Horizontal Start
	CAMERA_SetReg(0x18, 0xcf);	// (207-47)*4=640 -> Horizontal End
	CAMERA_SetReg(0x19, 0x06);	// 6 -> Vertical Start
	CAMERA_SetReg(0x1a, 0xf5);	// (245-6+1)*2=480 -> Vertical End
	CAMERA_SetReg(0x1b, 0x00);	// pixel shift=0

				//(??old,incorrect) limit vertical size to 480, second stage aperture correction enable
	CAMERA_SetReg(0x20, (0<<7));	// limit vertical size to 480  -> bit 7: Modified CCIR656 format vertical sizing enabled
	CAMERA_SetReg(0x21, 0x80);	// Y Channel Offset Adjustment - 0 , direction - Subtract
	CAMERA_SetReg(0x22, 0x80);	// U Channel Offset Adjustment - 0
	CAMERA_SetReg(0x23, 0x00);	// Crystal Current control : maximum current
	CAMERA_SetReg(0x26, 0xa2);	// digital sharpness threshold(64mV), magnitude(100%). 
	
	CAMERA_SetReg(0x27, 0xe2 );	// default, disable CCIR rang clip
				//?? old value: 0xea, but bit[3] is reserved, why 1 ?
	CAMERA_SetReg(0x29, (0<<6));	// CHSYNC & VSYNC master mode
	CAMERA_SetReg(0x2a, 0x00);	// default(
				// (?? old,incorrect)frame rate high, 60Hz, 50Hz:0x80, UV delay 2 pixel. - CJH
	CAMERA_SetReg(0x2b, 0x00);	// default
				// (??old,incorrect) frame rate low, 60Hz, 50Hz:0xac
	CAMERA_SetReg(0x2c, 0x88);	// default
	CAMERA_SetReg(0x2e, 0x80);	// default
	CAMERA_SetReg(0x2f, 0x44);	// default
	CAMERA_SetReg(0x60, 0x27);	// default 
	CAMERA_SetReg(0x61, 0x82);	// YUV mode (7'st bit -> 1(YUV mode)) 
	CAMERA_SetReg(0x62, 0x5f);	// RGB Gamma control, gamma=010_1111b
	CAMERA_SetReg(0x63, 0xd5);	// reserved value ???
	CAMERA_SetReg(0x64, 0x57);	// enable Y Gamma,gammma=010_1011b	
	CAMERA_SetReg(0x65, 0x80+(3<<0));//??? bit[7:3]is reserved 01000b, why 0x80?, ADC_ref=11b(1.26V peak)
	CAMERA_SetReg(0x66, 0x55);	// default 
	CAMERA_SetReg(0x68, 0xcf);	// not default
	CAMERA_SetReg(0x69, 0x76);	// not default
	CAMERA_SetReg(0x6a, 0x22);	// not default
	CAMERA_SetReg(0x6b, 0x00);	// ??reserved
	CAMERA_SetReg(0x6c, 0x08);	// ??reserved
	CAMERA_SetReg(0x6d, 0x48);	// ??reserved
	CAMERA_SetReg(0x6e, 0x80);	// ??reserved
	CAMERA_SetReg(0x6f, 0x0c);	// ?? how? -CJH ( default=0x3a)
	CAMERA_SetReg(0x70, 0x89);	// -CJH, accelerated saturation mode(fast), -> if 3'st bit is zero, acclerated saturation mode
	CAMERA_SetReg(0x71, 0x00);	//  default, freerunning PCLK
	CAMERA_SetReg(0x72, 0x14);	//  default
	CAMERA_SetReg(0x73, 0x54);	//  default
	CAMERA_SetReg(0x75, 0x0e);	// ?? -CJH (bit[6:3]=reserved)
	CAMERA_SetReg(0x76, 0x00);	// default
	CAMERA_SetReg(0x77, 0xff);	// ??reserved
	CAMERA_SetReg(0x78, 0x80);	// ??reserved
	CAMERA_SetReg(0x79, 0x80);	// ??reserved
	CAMERA_SetReg(0x7a, 0x80);	// ??reserved
	CAMERA_SetReg(0x7b, 0xe6);	// ???reserved -CJH 
	CAMERA_SetReg(0x7c, 0x00);	// default

	ucVal = 0;
	ucVal |= (bInv ? 1 : 0)<<3;
	ucVal |= (1<<7);
	ucVal |= (1<<4);
	ucVal |= (1<<2);
	CAMERA_SetReg(0x14, ucVal);
	
	CAMERA_SetReg(0x24, 0x10);	// for progressive - CJH
	CAMERA_SetReg(0x25, 0x8a);	// for progressive - CJH 
	CAMERA_SetReg(0x28, 0x20);	// select progressive mode. -> if 5'st bit is '1', progressive scan mode 
	CAMERA_SetReg(0x2d, 0x95);	// auto brightness(4'st bit is '1'), banding filter enabled
	CAMERA_SetReg(0x67, 0x92);	// Color space=601 YCrCb, Y signal delay=0
	CAMERA_SetReg(0x74, 0x00);	// AGC maximum gain=2x.(COMM[6:5] is 00) -CJH
}



void CAMERA_InitS5K3AAE(ITU_R_STANDARD eItuR, CSPACE eSrcFmt, IMG_SIZE eSrcSz)
{
	u32 uOutSel;
	u32 uYcSel;
	u32 uCrCbSel;
	u8 x;


	CAMERA_IICOpen(100000);
	oCamera.m_ucSlaveAddr = 0x5a;
	CAMERA_SetReg(0xec, 0x00);
	CAMERA_SetReg(0x02, 0x01);
	CAMERA_SetReg(0x72, 0xf0);
	CAMERA_SetReg(0x74, 0x08);
	CAMERA_SetReg(0x76, 0xb0);
	CAMERA_SetReg(0x77, 0xd0);
	CAMERA_SetReg(0x79, 0x05);
	CAMERA_SetReg(0x7a, 0x03);

	CAMERA_SetReg(0xec, 0x01);
	CAMERA_SetReg(0x8b, 0x03);



/*
	CAMERA_IICOpen(100000); 
	oCamera.m_ucSlaveAddr = 0x5a;
	CAMERA_SetReg(0xec, 0x00);
	CAMERA_SetReg(0x71, 0x00); // PCLK setting	

	#if 1
	CAMERA_SetSizeInS5K3AAE(eSrcSz);

	Delay(20000);

	// Camera data format, YC order setting
	uOutSel = (eItuR == BT601) ? (1<<2) : (0<<2);
	uYcSel = (eSrcFmt == YCBYCR || eSrcFmt == YCRYCB) ? (1<<1) : (0<<1);
	uCrCbSel = (eSrcFmt == YCRYCB || eSrcFmt == CRYCBY) ? (1<<0) : (0<<0);
	CAMERA_SetReg(0xec, 0x01);
//	CAMERA_SetReg(0x6a, uOutSel | uYcSel | uCrCbSel);
	CAMERA_SetReg(0x6a, uOutSel | uYcSel | 1);

	CAMERA_SetReg(0xec, 0x01);
	CAMERA_GetReg(0x6a, &x);
	Disp("\nImage Data 0x01======> 0x%x\n", x);
	Disp("1.6ah = %02x\n", x);
	CAMERA_SetReg(0xec, 0x00);	// Bright Setting. 
	CAMERA_SetReg(0x76, 0xff);	// Brightness
	CAMERA_SetReg(0x77, 0xd0);	// Color Level
	CAMERA_SetReg(0x79, 0x05);	// White Balance R
	CAMERA_SetReg(0x7a, 0x03);	// Write Balance B
	#elif 0
	CAMERA_SetReg(0xec, 0x01);
	CAMERA_SetReg(0x6a, 0x01); 
	u8 x;
	GetReg(0x6a, x);	
	Disp("1.6ah = %02x\n", x);
	#elif 0
	u8 t1;
	CAMERA_SetReg(0xec, 0x00);
	GetReg(0xb0, t1);
	Disp("0.0xb0: 0x%x \n",  t1);
	GetReg(0xb1, t1);
	Disp("0.0xb1: 0x%x \n",  t1);
	GetReg(0xb2, t1);
	Disp("0.0xb2: 0x%x \n",  t1);
	GetReg(0xb3, t1);
	Disp("0.0xb3: 0x%x \n",  t1);
	///CAMERA_SetReg(0x7b, 0xff);

	CAMERA_SetReg(0xec, 0x01);
	GetReg(0x00, t1);
	Disp("1.0x00: 0x%x \n",  t1);

	CAMERA_SetReg(0xec, 0x01);
	CAMERA_SetReg(0x6a, 0x03);
	#endif
*/	
}


//////////
// Function Name : CAMERA_InitS5K3AAE
// Function Description : camera module(S5K3AAE) initaialize
// Input : 	CAM_ATTR eCcir(Camera Order), CSPACE eSrcFmt(Camera source format), IMG_SIZE eSrcSz(Camera source size)
// Output :    	None
void CAMERA_InitS5K3AAE_VGA(void)
{
	oCamera.m_ucSlaveAddr = 0x5a;

	CAMERA_IICOpen(100000);

	//***************************************************
	CAMERA_SetReg(0xec, 0x00);
	CAMERA_SetReg(0x72, 0x7d);	//25 Mhz MCLK Setting
//	CAMERA_SetReg(0x72, 0xc8);	//40 Mhz MCLK Setting	
	CAMERA_SetReg(0xec, 0x07);
	CAMERA_SetReg(0x37, 0x0);   //25 Mhz MCLK Setting

	CAMERA_SetReg(0xec,0x00);
//	CAMERA_SetReg(0x02,0x01);	// VGA Setting, Scale Down
	CAMERA_SetReg(0x02,0x00);	// VGA Setting, Scale Down	
//	CAMERA_SetReg(0x02,0x30);	// VGA Setting, Sub Sampleing
	CAMERA_SetReg(0xec, 0x01);
	CAMERA_SetReg(0x6a, 0x01);  // CRYCBY Setting. ITU656 Format
	//***************************************************

#if 1 // VGA Scale down SFR Setting Code
	// ScaleDown
	CAMERA_SetReg(0xec,0x04);
	//VGA scaledown
	CAMERA_SetReg(0x2d,0x7d);//<1.69h>
	CAMERA_SetReg(0x2e,0x01);//06	//<1.6ah>
//	CAMERA_SetReg(0x2f,0x05);//HCNT_END_H
//	CAMERA_SetReg(0x30,0x9f);//HCNT_END_L
//	CAMERA_SetReg(0x36,0x10);//42	//HBLK_START_L   
//	CAMERA_SetReg(0x37,0x04);//HBLK_OFFSET_H
//	CAMERA_SetReg(0x38,0xff);  //HBLK_OFFSET_L
//	CAMERA_SetReg(0x3a,0x05);  //VBLK_START_L 
//	CAMERA_SetReg(0x40,0x11);//45   //HS656_START_L
//	CAMERA_SetReg(0x42,0x01);//HS656_OFFSET_L
//	CAMERA_SetReg(0x44,0x0a);//VS656_START_L
//	CAMERA_SetReg(0x4c,0x00);//<1.93h>
//	CAMERA_SetReg(0x4d,0x00);//<1.94h>
//	CAMERA_SetReg(0x4e,0x00);//<1.95h>
//-------------------------------------------------------------------------
#endif

#if 0
	/////////////////////////////
	///Flicker setting
	CAMERA_SetReg(0xec,0x00);
//	CAMERA_SetReg(0x74,0x18);  // Auto Flicker start 60hz for 7.5fps
	CAMERA_SetReg(0x74,0x00);  // Auto Flicker start 60hz for 7.5fps	

	/////////////////////////////
	// Frame AE 
	CAMERA_SetReg(0xec,0x00);
//	CAMERA_SetReg(0x73,0x11);	// frame 1/2
	CAMERA_SetReg(0x73,0x00);	// frame 1/2	
#endif

#if 0 // Subsampleing 640*480 Code
	//QVGA Subsampling 640x480
	CAMERA_SetReg(0xec,0x03);
	CAMERA_SetReg(0x51,0x00);	//Vblank
	CAMERA_SetReg(0x52,0x7e);
	CAMERA_SetReg(0x53,0x01);	//HSIZE Min is 1070
	CAMERA_SetReg(0x54,0xa6);
	CAMERA_SetReg(0x55,0x0f);	// <1.69h>
	CAMERA_SetReg(0x56,0x01);	//07	// <1.6ah>
	CAMERA_SetReg(0x5e,0x3a);	//3b	// <1.78h> HBLK_START
	CAMERA_SetReg(0x5f,0x02);
	CAMERA_SetReg(0x60,0x80);
	CAMERA_SetReg(0x63,0x01);
	CAMERA_SetReg(0x64,0xe0);
	CAMERA_SetReg(0x68,0x3a);	//3b	// <1.82h> HS656_START
	CAMERA_SetReg(0x69,0x02);
	CAMERA_SetReg(0x6a,0x80);
	CAMERA_SetReg(0x6d,0x01);
	CAMERA_SetReg(0x6e,0xe0);

	/////////////////////////////
	///Flicker setting
	CAMERA_SetReg(0xec,0x00);
	CAMERA_SetReg(0x74,0x18);  // Auto Flicker start 60hz for 7.5fps

	/////////////////////////////
	// Frame AE 
	CAMERA_SetReg(0xec,0x00);
//	CAMERA_SetReg(0x73,0x11);	// frame 1/2
	CAMERA_SetReg(0x73,0x00);	// frame 1/2	
#endif


#if 1
	CAMERA_SetReg(0xec,0x01);
	CAMERA_SetReg(0x3b,0x18);	//GrGb correction
	CAMERA_SetReg(0xaa,0xe8);//e0	//For AWB, High Threshold Value of Y signal
	CAMERA_SetReg(0xab,0x10);//30	//For AWB, Low Threshold Value of Y signal

	//***************************************************
	// page 2
	CAMERA_SetReg(0xec,0x02);
	CAMERA_SetReg(0x29,0x4f);	//Clamp On(Sun Spot)
	CAMERA_SetReg(0x2d,0x05);	//APS Bias Current (I_APS_CTRL)

#endif

#if 0 
	//***************************************************
	// page 3
	/////////////////////////////
	// Subsampling
	//QVGA Subsampling 640x480
	CAMERA_SetReg(0xec,0x03);
	CAMERA_SetReg(0x51,0x00);	//Vblank
	CAMERA_SetReg(0x52,0x7e);
	CAMERA_SetReg(0x53,0x01);	//HSIZE Min is 1070
	CAMERA_SetReg(0x54,0xa6);
	CAMERA_SetReg(0x55,0x0f);	// <1.69h>
	CAMERA_SetReg(0x56,0x06);	//07	// <1.6ah>
	CAMERA_SetReg(0x5e,0x3a);	//3b	// <1.78h> HBLK_START
	CAMERA_SetReg(0x5f,0x02);
	CAMERA_SetReg(0x60,0x80);
	CAMERA_SetReg(0x63,0x01);
	CAMERA_SetReg(0x64,0xe0);
	CAMERA_SetReg(0x68,0x3a);	//3b	// <1.82h> HS656_START
	CAMERA_SetReg(0x69,0x02);
	CAMERA_SetReg(0x6a,0x80);
	CAMERA_SetReg(0x6d,0x01);
	CAMERA_SetReg(0x6e,0xe0);
	//QVGA Subsampling 320x240
	CAMERA_SetReg(0xa1,0x00);	//Vblank
	CAMERA_SetReg(0xa2,0x7e);
	CAMERA_SetReg(0xa3,0x01);	//HSIZE Min is 1070
	CAMERA_SetReg(0xa4,0xa6);
	CAMERA_SetReg(0xa5,0x0f);	// <1.69h>
	CAMERA_SetReg(0xa6,0x07);	// <1.6ah>
	CAMERA_SetReg(0xae,0x39);	// <1.78h> HBLK_START
	CAMERA_SetReg(0xaf,0x01);	//320
	CAMERA_SetReg(0xb0,0x40);
	CAMERA_SetReg(0xb3,0x00);	//240
	CAMERA_SetReg(0xb4,0xf0);
	CAMERA_SetReg(0xb8,0x39);	// <1.82h> HS656_START
	CAMERA_SetReg(0xb9,0x01); 	//320 
	CAMERA_SetReg(0xba,0x40);
	CAMERA_SetReg(0xbd,0x00);  	//240
	CAMERA_SetReg(0xbe,0xf0);
#endif

	//***************************************************
	// page 6
	CAMERA_SetReg(0xec,0x06);
	CAMERA_SetReg(0xbc,0x0b);	//Modify wrp wcp for mirror, VGA
	CAMERA_SetReg(0xbd,0x0c);
	CAMERA_SetReg(0xbe,0x0b);
	CAMERA_SetReg(0xbf,0x0c);
	CAMERA_SetReg(0xdc,0x0b);	//Modify wrp wcp for mirror, QVGA
	CAMERA_SetReg(0xdd,0x0c);
	CAMERA_SetReg(0xde,0x0b);
	CAMERA_SetReg(0xdf,0x0c);

#if 0 
	//***************************************************
	// page 4
	/////////////////////////////
	// ScaleDown
	CAMERA_SetReg(0xec,0x04);
	//VGA scaledown
	CAMERA_SetReg(0x2d,0x7d	);//<1.69h>
	CAMERA_SetReg(0x2e,0x07	);//06	//<1.6ah>
	CAMERA_SetReg(0x2f,0x05	);//HCNT_END_H
	CAMERA_SetReg(0x30,0x9f	);//HCNT_END_L
	CAMERA_SetReg(0x36,0x10	);//42	//HBLK_START_L   
	CAMERA_SetReg(0x37,0x04	);//HBLK_OFFSET_H
	CAMERA_SetReg(0x38,0xff );  //HBLK_OFFSET_L
	CAMERA_SetReg(0x3a,0x05 );  //VBLK_START_L 
	CAMERA_SetReg(0x40,0x11	);//45   //HS656_START_L
	CAMERA_SetReg(0x42,0x01	);//HS656_OFFSET_L
	CAMERA_SetReg(0x44,0x0a	);//VS656_START_L
	CAMERA_SetReg(0x4c,0x00	);//<1.93h>
	CAMERA_SetReg(0x4d,0x00	);//<1.94h>
	CAMERA_SetReg(0x4e,0x00	);//<1.95h>
	//QVGA scaledown
	CAMERA_SetReg(0x55,0x35);	//<1.69h>
	CAMERA_SetReg(0x56,0x06); 	//<1.6ah>
	CAMERA_SetReg(0x57,0x05);	//HCNT_END_H
	CAMERA_SetReg(0x58,0x9f);	//HCNT_END_L
	CAMERA_SetReg(0x5e,0x6b);	//b3	//HBLK_START_L
	CAMERA_SetReg(0x60,0x00);  	//HBLK_OFFSET_L
	CAMERA_SetReg(0x64,0xf2);	//VBLK_OFFSET_L
	CAMERA_SetReg(0x68,0x6c);	//b4	//HS656_START_L
	CAMERA_SetReg(0x6c,0x06);   //VS656_START_L
	CAMERA_SetReg(0x74,0x00);	//<1.93h>
	CAMERA_SetReg(0x75,0x00);	//<1.94h>
	CAMERA_SetReg(0x76,0x00);	//<1.95h>
	//QQVGA scaledown
	CAMERA_SetReg(0x7d,0x57);	//<1.69h>
	CAMERA_SetReg(0x7e,0x07);	//<1.6ah>   
	CAMERA_SetReg(0x7f,0x05);	//HCNT_END_H
	CAMERA_SetReg(0x80,0x9f);	//HCNT_END_L     
	CAMERA_SetReg(0x86,0x2f);	//7d   //HBLK_START_L   
	CAMERA_SetReg(0x8f,0x05);	//HS656_START_H 
	CAMERA_SetReg(0x90,0xa3);	//bf	//HS656_START_L 
	CAMERA_SetReg(0x9c,0x00);	//<1.93h>
	CAMERA_SetReg(0x9d,0x00);	//<1.94h>
	CAMERA_SetReg(0x9e,0x00);	//<1.95h>
	//CIF scaledown
	CAMERA_SetReg(0xa5,0x51);	//<1.69h>
	CAMERA_SetReg(0xa6,0x06);	//<1.6ah>      
	CAMERA_SetReg(0xae,0xc9);   //HBLK_START_L 
	CAMERA_SetReg(0xb0,0x7f);   //HBLK_OFFSET_L
	CAMERA_SetReg(0xb2,0x04); 	//VBLK_START_L 
	CAMERA_SetReg(0xb8,0xaf);	//HS656_START_L
	CAMERA_SetReg(0xba,0xa0);	//HS656_OFFSET_L
	CAMERA_SetReg(0xbc,0x08);	//VS656_START_L
	//QCIF scaledown
	CAMERA_SetReg(0xcd,0x33);	//<1.69h>
	CAMERA_SetReg(0xce,0x07);	//<1.6ah>     
	CAMERA_SetReg(0xd6,0x8e);   //HBLK_START_L
	CAMERA_SetReg(0xd8,0x81);	//HBLK_OFFSET_L
	CAMERA_SetReg(0xda,0x03);   //VBLK_CAMERA_SetReg(0xTART_L 
	CAMERA_SetReg(0xe0,0x7e);	//HS656_START_L 
	CAMERA_SetReg(0xe2,0x90);   //HS656_OFFSET_L
	CAMERA_SetReg(0xe4,0x08);   //VS656_START_L 
#endif

#if 1 // Tunning Code
	//***************************************************
	// page 5
	CAMERA_SetReg(0xec,0x05);
	//s000f	//Y Shading, RGB Shading ON
	CAMERA_SetReg(0x00,0x0d);	//Y Shading, RGB Shading OFF

	//RGB Shading
	CAMERA_SetReg(0x05,0x02);
	CAMERA_SetReg(0x06,0x02);
	CAMERA_SetReg(0x07,0x02);
	CAMERA_SetReg(0x08,0x02);  

	///Y Shading
	CAMERA_SetReg(0x2d,0xf0);	//c8	//d0	
	CAMERA_SetReg(0x2e,0xc0);	//a8	//b0	
	CAMERA_SetReg(0x2f,0xa0);	//98	//a0	
	CAMERA_SetReg(0x30,0x84);	//88	//88	
	CAMERA_SetReg(0x31,0x84);	//88	//88	
	CAMERA_SetReg(0x32,0xa0);	//98	//a0	
	CAMERA_SetReg(0x33,0xc0);	//a8	//b0	
	CAMERA_SetReg(0x34,0xf0);	//c8	//d0	
	  
	CAMERA_SetReg(0x35,0xf0);	//c8	//d0	
	CAMERA_SetReg(0x36,0xc0);	//a8	//b0	
	CAMERA_SetReg(0x37,0xa0);	//98	//98	
	CAMERA_SetReg(0x38,0x84);	//88	//88	
	CAMERA_SetReg(0x39,0x84);	//88	//88	
	CAMERA_SetReg(0x3a,0xa0);	//98	//98	
	CAMERA_SetReg(0x3b,0xc0);	//a8	//b0	
	CAMERA_SetReg(0x3c,0xf0);	//c8	//d0	

	//***************************************************
	// page 7
	CAMERA_SetReg(0xec,0x07);
	CAMERA_SetReg(0x17,0x44);	//AWBGgain
	CAMERA_SetReg(0x11,0xfe);	//Ggain offset
	CAMERA_SetReg(0x80,0x0c);	//AE weight

	//***************************************************
	// page 0
	CAMERA_SetReg(0xec,0x00);
	CAMERA_SetReg(0x23,0x32);	//using wrpwcp mirror
	CAMERA_SetReg(0x6c,0x00);	//AE target Low
	CAMERA_SetReg(0x77,0xd0);	//color level
	CAMERA_SetReg(0x7e,0x86);	//DBPRM On, Color Suppress On, Y gain On, Digital clamp Off
	CAMERA_SetReg(0x83,0xa0);	//c0	//Color suppress
	CAMERA_SetReg(0xbb,0x01);	//AWB AE diff
	CAMERA_SetReg(0xbc,0x00);

	////////////////////////////
	//Edge Suppress
	CAMERA_SetReg(0xe0,0x0f);
	CAMERA_SetReg(0xe1,0x50);	//AGC Min to Start Edge suppress
	CAMERA_SetReg(0xe2,0x60);	//AGC Max
	CAMERA_SetReg(0xe3,0x10);	//Edge Coefficient for AGC Min
	CAMERA_SetReg(0xe4,0x00);	//Edge Coefficient for AGC Max
	CAMERA_SetReg(0xe5,0x60);	//AGC Position
	CAMERA_SetReg(0xe6,0x40);	//Edge Gain lower than the position
	CAMERA_SetReg(0xe7,0x00);	//Edge Gain higer than the position

	/////////////////////////////
	//Adjust AWB boundary
	CAMERA_SetReg(0x34,0x05);	//Rgain_max
	CAMERA_SetReg(0x35,0x0b);	//Bgain_max
	CAMERA_SetReg(0x36,0x09);	//R_limit high
	CAMERA_SetReg(0x37,0x0b);	//B_limit high
	CAMERA_SetReg(0x39,0x07);	//Blimit Low
	CAMERA_SetReg(0x3a,0x0a);	//slop bottom
	CAMERA_SetReg(0x3b,0x0e);	//slop top

	//Adjust RGBgain of AWB
	CAMERA_SetReg(0x79,0x00);	//Rgain offset
	CAMERA_SetReg(0x7a,0x02);	//Bgain offset

	//White Point
	CAMERA_SetReg(0x40,0x1c);	        
	CAMERA_SetReg(0x41,0x4a);	        
	CAMERA_SetReg(0x42,0x21);	//22	
	CAMERA_SetReg(0x43,0x3c);	//3a	
	CAMERA_SetReg(0x44,0x2e);	//2e	
	CAMERA_SetReg(0x45,0x2d);	//2c	

	//Hue, Gain control                           
	CAMERA_SetReg(0x48,0xd0);	//d0	//80                  
	CAMERA_SetReg(0x49,0xfe);	//fe	//80  
	CAMERA_SetReg(0x4a,0x48);	//48	//00                  
	CAMERA_SetReg(0x4b,0x30);	//38	//00   
	CAMERA_SetReg(0x4c,0xfe);	//fe	//80   
	CAMERA_SetReg(0x4d,0xfe);	//fe	//80   
	CAMERA_SetReg(0x4e,0x60);	//48	//00  
	CAMERA_SetReg(0x4f,0x00);	//10	//00   
	     	//       
	CAMERA_SetReg(0x50,0xc8);	//c0	//80                       
	CAMERA_SetReg(0x51,0xfe);	//fe	//80       
	CAMERA_SetReg(0x52,0x4a);	//4a	//00               
	CAMERA_SetReg(0x53,0x30);	//38	//00
	CAMERA_SetReg(0x54,0xfe);	//fe	//80
	CAMERA_SetReg(0x55,0xfe);	//fe	//80     
	CAMERA_SetReg(0x56,0x60);	//50	//00     
	CAMERA_SetReg(0x57,0x00);	//10	//00
	          
	CAMERA_SetReg(0x58,0xb0);	//80
	CAMERA_SetReg(0x59,0xfe);	//80
	CAMERA_SetReg(0x5a,0x50);	//00
	CAMERA_SetReg(0x5b,0x48);	//00
	CAMERA_SetReg(0x5c,0xf0);	//80
	CAMERA_SetReg(0x5d,0xfe);	//80
	CAMERA_SetReg(0x5e,0x50);	//00
	CAMERA_SetReg(0x5f,0x10);	//00
#endif 


	CAMERA_SetReg(0xec, 0x00);	// Bright Setting. 
	CAMERA_SetReg(0x76, 0xa0);	// Brightness

	/////////////////////////////
	///Flicker setting
	CAMERA_SetReg(0xec,0x00);
//	CAMERA_SetReg(0x74,0x18);  // Auto Flicker start 60hz for 7.5fps
	CAMERA_SetReg(0x74,0x00);  // Auto Flicker start 60hz for 7.5fps	

	/////////////////////////////
	// Frame AE 
	CAMERA_SetReg(0xec,0x00);
//	CAMERA_SetReg(0x73,0x11);	// frame 1/2
	CAMERA_SetReg(0x73,0x00);	// frame 1/2	


	CAMERA_IICClose();



}


//////////
// Function Name : CAMERA_SetSizeInS5K3AAE
// Function Description : camera module(S5K3AAE) Size calucate
// Input : 	IMG_SIZE eSrcSize(Source size)
// Output :    	None
void CAMERA_SetSizeInS5K3AAE(IMG_SIZE eSrcSize)
{

	u8 y, x, ucSrcSize;
	int i;

	oCamera.m_ucSlaveAddr = 0x5a;

	// Size scaling setting
	ucSrcSize = 
		(eSrcSize == SXGA) ? (0) : 
		(eSrcSize == VGA) ? (1) :
		(eSrcSize == QVGA) ? (2):
		(eSrcSize == QQVGA) ? (3) :
		(eSrcSize == CIF) ? (4) :
		(eSrcSize == QCIF) ? (5) :
		0xff;
	Assert(ucSrcSize != 0xff);

//	SetEVT6(eSrcSize);

	CAMERA_SetReg(0xec, 0x00);	
	CAMERA_SetReg(0x02, ucSrcSize);
//	CAMERA_SetReg(0x02, 0x00);

	CAMERA_SetReg(0xec, 0x00);
	CAMERA_GetReg(0x02, &x);
	UART_Printf("\nImage Size Setting is %d ===> %d(0:SXGA 1:VGA)\n", ucSrcSize, x);

	CAMERA_SetReg(0xec, 0x04);
	if(eSrcSize == SXGA)
		y=0;
	else if(eSrcSize == VGA)
		y=0x28;
	for(i=0; i <= 0x27 ; i++)
	{
		CAMERA_GetReg(y+i, &x);
//		UART_Printf("4.%x = %x\n",i+y,x);
	}


}



/*
//////////
// Function Name : CAMERA_InitS5K4AAF
// Function Description : camera module(S5K4AAF) initaialize
// Input : 	CAM_ATTR eCcir(Camera Order), CSPACE eSrcFmt(Camera source format), IMG_SIZE eSrcSz(Camera source size)
// Output :    	None
void CAMERA_InitS5K4AAF(CAM_ATTR eCcir, CSPACE eSrcFmt, IMG_SIZE eSrcSize)
{
	u8 x;
	u8 ucSrcSize;
	u8 uOutSel;
	u8 uYcSel;
	u8 uCrCbSel;

//	IIC_Init();
	oCamera.m_ucSlaveAddr = 0x5a;

#if 1
//	GetReg(0x02, x);
//	UART_Printf(" 0.02h = %02x\n", x);
	CAMERA_SetReg(0xec, 0x00);
	CAMERA_SetReg(0x71, 0x00); // PCLK setting
#else  // 9.5
	CAMERA_SetReg(0xec, 0x01);
	CAMERA_SetReg(0xa5, 0x01);		//ARM Clock divider=ARM_CLK_DIV (1/2) (ARM_CLK range = 14MHz ~ 44MHz)				
	
	//In case of PCLK = 27MHz (PLL_CLK = Pixel_CLK*2, ARM_CLK = PLL_CLK/ARM_CLK_DIV )
	CAMERA_SetReg(0xec, 0x01);
	CAMERA_SetReg(0xa1, 54);		//PLL M=54	
	CAMERA_SetReg(0xa2, 0x48);      	//PLL S=1,P=8
#endif
	// Size scaling
	//--------------
	ucSrcSize = 
		(eSrcSize == SXGA) ? (0) : 
		(eSrcSize == VGA) ? (1) :
		(eSrcSize == QVGA) ? (2) :
		(eSrcSize == QQVGA) ? (3) :
		(eSrcSize == CIF) ? (4) :
		(eSrcSize == QCIF) ? (5) :
		0xff;
	Assert(ucSrcSize != 0xff);
		
	CAMERA_SetReg(0xec, 0x00);
	CAMERA_SetReg(0x02, ucSrcSize);	

#if 1	// if deleted, no operation..
	// Camera data format, YC order
	//------------------------------
	uOutSel = (eCcir == CCIR601) ? (1<<2) : (0<<2);
	uYcSel = (eSrcFmt == YCBYCR || eSrcFmt == YCRYCB) ? (1<<1) : (0<<1);
	uCrCbSel = (eSrcFmt == YCRYCB || eSrcFmt == CRYCBY) ? (1<<0) : (0<<0);
	
	CAMERA_SetReg(0xec, 0x01);
	CAMERA_GetReg(0x7f, &x);
	UART_Printf("1.7fh = %02x, 0x%02x\n", x, uOutSel | uYcSel | uCrCbSel);	
	CAMERA_SetReg(0x7f, uOutSel | uYcSel | uCrCbSel);	
///	CAMERA_SetReg(0x7f, 0x40|uOutSel | uYcSel | 1);


///	GetReg(0x7f, x);
///	UART_Printf("1.7fh = %02x\n", x);
#endif
	

}




//////////
// Function Name : CAMERA_SetSizeInS5K4AAF
// Function Description : camera module(S5K4AAF) Size calucate
// Input : 	IMG_SIZE eSrcSize(Source size)
// Output :    	None
void CAMERA_SetSizeInS5K4AAF(IMG_SIZE eSrcSize)
{
	u8 ucSrcSize;

	oCamera.m_ucSlaveAddr = 0x5a;	

	// PCLK setting
//	CAMERA_SetReg(0xec, 0x00);
//	CAMERA_SetReg(0x71, 0x00); 

	// Size scaling
	//--------------
	ucSrcSize = 
		(eSrcSize == SXGA) ? (0) : 
		(eSrcSize == VGA) ? (1) :
		(eSrcSize == QVGA) ? (2) :
		(eSrcSize == QQVGA) ? (3) :
		(eSrcSize == CIF) ? (4) :
		(eSrcSize == QCIF) ? (5) :
		0xff;
	Assert(ucSrcSize != 0xff);
		
	CAMERA_SetReg(0xec, 0x00);
	CAMERA_SetReg(0x02, ucSrcSize);	

#if 0	// if deleted, no operation..
	// Camera data format, YC order
	//------------------------------
///	u8 uOutSel = (eCcir == CCIR601) ? (1<<2) : (0<<2);
///	u8 uYcSel = (eSrcFmt == YCBYCR || eSrcFmt == YCRYCB) ? (1<<1) : (0<<1);
///	u8 uCrCbSel = (eSrcFmt == YCRYCB || eSrcFmt == CRYCBY) ? (1<<0) : (0<<0);
	
	CAMERA_SetReg(0xec, 0x01);
//	GetReg(0x7f, x);
//	UART_Printf("1.7fh = %02x, 0x%02x\n", x, uOutSel | uYcSel | uCrCbSel);	
	///CAMERA_SetReg(0x7f, uOutSel | uYcSel | uCrCbSel);	
	CAMERA_SetReg(0x7f, 0x46); ////
#endif
}

*/


void CAMERA_InitS5K4BAF2(CAM_ATTR eCcir, CSPACE eSrcFmt, SUB_SAMPLING eSub)
{
	u32 uHCLKx2;

	SYSC_GetClkInform(); // Get HCLKx2 Frequency
	uHCLKx2 = g_HCLKx2/1000000;
	oCamera.m_ucSlaveAddr = 0x5a;

	CAMERA_IICOpen(100000); 

	CAMERA_SetReg(0xfc,0x07);
	CAMERA_SetReg(0x66,0x01);// Watch Dog Time On
	CAMERA_SetReg(0xfc,0x00);
	CAMERA_SetReg(0x00,0xAA);// For EDS Check
	CAMERA_SetReg(0x21,0x03);// peter0223 추가

	CAMERA_SetReg(0xfc,0x01);
	CAMERA_SetReg(0x04,0x01);// ARM Clock Divider  

	CAMERA_SetReg(0xfc,0x02);
	CAMERA_SetReg(0x30,0x90);// Analog offset   
	CAMERA_SetReg(0x37,0x0d);// Global Gain
	CAMERA_SetReg(0x2d,0x48);// Double Shutter
	CAMERA_SetReg(0x60,0x00);// Blank_Adrs
	CAMERA_SetReg(0x45,0x1e);//0e// CDS Timing for Average Sub_Sampling
	CAMERA_SetReg(0x47,0x2f);
	CAMERA_SetReg(0x02,0x0e);// ADC Resolution
	CAMERA_SetReg(0x3d,0x06);// Frame ADLC
	CAMERA_SetReg(0x4d,0x08);// Doubler Volatage
	CAMERA_SetReg(0x54,0x02);// Double Shutter
	CAMERA_SetReg(0x55,0x1e);// Line ADLC
	CAMERA_SetReg(0x56,0x30);// 
	CAMERA_SetReg(0x59,0x00);// LineADLC offset
	CAMERA_SetReg(0x5b,0x08);// R_Ref_Ctrl
	CAMERA_SetReg(0x44,0x63);// CLP_EN
	CAMERA_SetReg(0x4A,0x10);// Clamp Control
	CAMERA_SetReg(0x42,0x02);// 
	CAMERA_SetReg(0x43,0xef);//

	CAMERA_SetReg(0xfc,0x03);
	CAMERA_SetReg(0x2c,0x00);// crcb_sel for Sub-Sampling Table
	CAMERA_SetReg(0x05,0x46);// Output Image Size Set for Capture
	CAMERA_SetReg(0x07,0xb6);
	CAMERA_SetReg(0x0e,0x04);
	CAMERA_SetReg(0x12,0x03);

	CAMERA_SetReg(0xfc,0x04);
	CAMERA_SetReg(0x32,0x04);
	CAMERA_SetReg(0x33,0xbc);

	CAMERA_SetReg(0xfc,0x04);
	CAMERA_SetReg(0xc5,0x26);// Output Image Size Set for Preview
	CAMERA_SetReg(0xc7,0x5e);
	CAMERA_SetReg(0xce,0x04);
	CAMERA_SetReg(0xd2,0x04);
	CAMERA_SetReg(0xec,0x00);//CrCb sel
	CAMERA_SetReg(0xc0,0x06);
	CAMERA_SetReg(0xc1,0x70);
	CAMERA_SetReg(0xc2,0x02);
	CAMERA_SetReg(0xc3,0x87);

	CAMERA_SetReg(0xfc,0x07);
	CAMERA_SetReg(0x05,0x00);
	CAMERA_SetReg(0x06,0x00);
	CAMERA_SetReg(0x07,0x8b);
	CAMERA_SetReg(0x08,0xf5);
	CAMERA_SetReg(0x09,0x00);
	CAMERA_SetReg(0x0a,0xb4);
	CAMERA_SetReg(0x0b,0x00);
	CAMERA_SetReg(0x0c,0xea);
	CAMERA_SetReg(0x0d,0x00);
	CAMERA_SetReg(0x0e,0x40);

	CAMERA_SetReg(0xfc,0x00);
	CAMERA_SetReg(0x70,0x02);

	CAMERA_SetReg(0xfc,0x03);
	CAMERA_SetReg(0x2c,0x00);
	CAMERA_SetReg(0x5c,0x00);
	CAMERA_SetReg(0x8c,0x00);
	CAMERA_SetReg(0xbc,0x00);

	CAMERA_SetReg(0xfc,0x04);
	CAMERA_SetReg(0x5c,0x00);
	CAMERA_SetReg(0xfc,0x00);            
	CAMERA_SetReg(0x73,0x21);// Frmae AE Enable peter
	CAMERA_SetReg(0x20,0x02);// Change AWB Mode

	CAMERA_SetReg(0xfc,0x00);
	CAMERA_SetReg(0x6c,0xb0);// AE target  
	CAMERA_SetReg(0x6d,0x00);  

	CAMERA_SetReg(0xfc,0x20);
	CAMERA_SetReg(0x16,0x5a);// for Prevating AE Hunting

	CAMERA_SetReg(0xfc,0x00);
	CAMERA_SetReg(0x78,0x6a);// AGC Max

	CAMERA_SetReg(0xfc,0x20);
	CAMERA_SetReg(0x16,0x60);// Frame AE Start

	CAMERA_SetReg(0xfc,0x20);
	CAMERA_SetReg(0x57,0x18);// Stable_Frame_AE 
	CAMERA_SetReg(0x2C,0x30);// For Forbidden Area
	CAMERA_SetReg(0x2E,0x00);// For Forbidden Area  
	CAMERA_SetReg(0x14,0x70);        
	CAMERA_SetReg(0x01,0x00);// Stepless_Off      

	CAMERA_SetReg(0xfc,0x07);
	CAMERA_SetReg(0x11,0x02);// AWB G Gain offset

	CAMERA_SetReg(0xfc,0x07);
	CAMERA_SetReg(0x3e,0x0a);// AWB Cut R max

	CAMERA_SetReg(0xfc,0x01);
	CAMERA_SetReg(0xc8,0xd0);// AWB Y Max e0  Y값이 어느 이상일때 그 이상값을 짤라내는 세팅값

	CAMERA_SetReg(0xfc,0x00);
	CAMERA_SetReg(0x3e,0x20);//30 AWB Y_min
	CAMERA_SetReg(0x3d,0x10);// AWB Y_min Low

	CAMERA_SetReg(0xfc,0x22);
	CAMERA_SetReg(0x8c,0x04);// AWB Min Y Weight AWB 할때 Y신호를 기준으로 하는데 가중치를 바꾸는 것.
	CAMERA_SetReg(0x8d,0x16);// AWB Max Y Weight

	CAMERA_SetReg(0xfc,0x00);
	CAMERA_SetReg(0x32,0x04);// AWB moving average 8 frame
	CAMERA_SetReg(0x81,0x10);// AWB G gain suppress Disable 
	CAMERA_SetReg(0xbc,0xf0);
	CAMERA_SetReg(0x29,0x04);// Y level H
	CAMERA_SetReg(0x2a,0x00);// Y level L
	CAMERA_SetReg(0x2b,0x03);// color level H
	CAMERA_SetReg(0x2c,0xc8);// color level L

	CAMERA_SetReg(0xfc,0x07);
	CAMERA_SetReg(0x37,0x00);// Flicker Add for 32Mhz

	CAMERA_SetReg(0xfc,0x00);
	CAMERA_SetReg(0x72,0xa0);// Flicker for 32MHz
	CAMERA_SetReg(0x74,0x08);// flicker 60Hz Fix  

	CAMERA_SetReg(0xfc,0x20);
	CAMERA_SetReg(0x02,0x02);// Flicker Dgain Mode
	 
	CAMERA_SetReg(0xfc,0x00);
	//{0x23,0x);// Mirror Option
	CAMERA_SetReg(0x62,0x0a);// Mirror Option          
	 
	CAMERA_SetReg(0xfc,0x02);
	CAMERA_SetReg(0x4e,0x00);// IO current 8mA set
	CAMERA_SetReg(0x4e,0x00);// IO current 8mA set
	CAMERA_SetReg(0x4e,0x00);// IO current 8mA set
	CAMERA_SetReg(0x4e,0x00);// IO current 8mA set
	CAMERA_SetReg(0x4f,0x0a);// 2a IO current 48mA set
	CAMERA_SetReg(0x4f,0x0a);// IO current 48mA set
	CAMERA_SetReg(0x4f,0x0a);// IO current 48mA set
	CAMERA_SetReg(0x4f,0x0a);// IO current 48mA set
	 
	CAMERA_SetReg(0xfc,0x01);
	CAMERA_SetReg(0x0c,0x03);// Full YC Enable
	//{0x0c,03);// Full YC Enable
	//CAMERA_SetReg(/{0x02,02);// crcb_sel
	//CAMERA_SetReg(/{0x02,02);// crcb_sel  peter0222 추가
	//CAMERA_SetReg(/{0x01,01);// pclk      peter0222 추가
	//CAMERA_SetReg(/{0x01,01);
	 
	//========);============================================
	//  COLOR );
	//========);============================================
	 
	CAMERA_SetReg(0xfc,0x01);// color matrix                   
	CAMERA_SetReg(0x51,0x0A);       
	CAMERA_SetReg(0x52,0x42);       
	CAMERA_SetReg(0x53,0xF9);       
	CAMERA_SetReg(0x54,0x80);       
	CAMERA_SetReg(0x55,0x00);       
	CAMERA_SetReg(0x56,0x3D);       
	CAMERA_SetReg(0x57,0xFE);       
	CAMERA_SetReg(0x58,0x0B);       
	CAMERA_SetReg(0x59,0x06);       
	CAMERA_SetReg(0x5A,0x9C);       
	CAMERA_SetReg(0x5B,0xFF);       
	CAMERA_SetReg(0x5C,0x59);                          
	CAMERA_SetReg(0x5D,0xFF);                          
	CAMERA_SetReg(0x5E,0xD8);                          
	CAMERA_SetReg(0x5F,0xFC);                          
	CAMERA_SetReg(0x60,0x2E);               
	CAMERA_SetReg(0x61,0x07);                     
	CAMERA_SetReg(0x62,0xFA);             
	 
	//========);============================================
	//  EDGE E);MENT                                       
	//========);============================================
	 
	CAMERA_SetReg(0xfc,0x00);
	CAMERA_SetReg(0x89,0x03);// Edge Suppress On

	CAMERA_SetReg(0xfc,0x0b);
	CAMERA_SetReg(0x42,0x50);// Edge AGC MIN
	CAMERA_SetReg(0x43,0x60);// Edge AGC MAX
	CAMERA_SetReg(0x45,0x18);// positive gain AGC MIN    
	CAMERA_SetReg(0x49,0x0a);// positive gain AGC MAX 
	CAMERA_SetReg(0x4d,0x18);// negative gain AGC MIN
	CAMERA_SetReg(0x51,0x0a);// negative gain AGC MAX
	 
	CAMERA_SetReg(0xfc,0x05);
	CAMERA_SetReg(0x34,0x20);// APTCLP  
	CAMERA_SetReg(0x35,0x09);// APTSC  
	CAMERA_SetReg(0x36,0x0b);// ENHANCE 
	CAMERA_SetReg(0x3f,0x00);// NON-LIN   
	CAMERA_SetReg(0x42,0x10);// EGFALL
	CAMERA_SetReg(0x43,0x00);// HLFALL
	CAMERA_SetReg(0x45,0xa0);// EGREF
	CAMERA_SetReg(0x46,0x7a);// HLREF
	CAMERA_SetReg(0x47,0x40);// LLREF
	CAMERA_SetReg(0x48,0x0c);
	CAMERA_SetReg(0x49,0x31);// CSSEL  EGSEL  CS_DLY 
	CAMERA_SetReg(0x40,0x41);// Y delay
	 
	//========);============================================
	//  GAMMA );                                
	//========);============================================                                                                                                                                         -
	 
	CAMERA_SetReg(0xfc,0x01);
	CAMERA_SetReg(0x6F,0x0A);// R  
	CAMERA_SetReg(0x70,0x1A);
	CAMERA_SetReg(0x71,0x7A);
	CAMERA_SetReg(0x72,0xF8);
	CAMERA_SetReg(0x73,0x00);
	CAMERA_SetReg(0x74,0xA0);
	CAMERA_SetReg(0x75,0x18);
	CAMERA_SetReg(0x76,0x65);
	CAMERA_SetReg(0x77,0xAD);
	CAMERA_SetReg(0x78,0x6A);
	CAMERA_SetReg(0x79,0xE2);
	CAMERA_SetReg(0x7A,0x12);
	CAMERA_SetReg(0x7B,0x3D);
	CAMERA_SetReg(0x7C,0x5A);
	CAMERA_SetReg(0x7D,0xBF);
	CAMERA_SetReg(0x7E,0x72);
	CAMERA_SetReg(0x7F,0x88); 
	CAMERA_SetReg(0x80,0x9D); 
	CAMERA_SetReg(0x81,0xB0); 
	CAMERA_SetReg(0x82,0xFF);
	CAMERA_SetReg(0x83,0xC0);   
	CAMERA_SetReg(0x84,0xCF);           
	CAMERA_SetReg(0x85,0xDA);           
	CAMERA_SetReg(0x86,0xFC);
	CAMERA_SetReg(0x87,0x08);//G 
	CAMERA_SetReg(0x88,0x12);
	CAMERA_SetReg(0x89,0x42);        
	CAMERA_SetReg(0x8A,0xBA);        
	CAMERA_SetReg(0x8B,0x00);
	CAMERA_SetReg(0x8C,0x75);
	CAMERA_SetReg(0x8D,0xED);   
	CAMERA_SetReg(0x8E,0x42);   
	CAMERA_SetReg(0x8F,0x80);
	CAMERA_SetReg(0x90,0x5A);
	CAMERA_SetReg(0x91,0xB5);
	CAMERA_SetReg(0x92,0xE5);
	CAMERA_SetReg(0x93,0x10);
	CAMERA_SetReg(0x94,0x35);
	CAMERA_SetReg(0x95,0xAF);
	CAMERA_SetReg(0x96,0x55);
	CAMERA_SetReg(0x97,0x70);
	CAMERA_SetReg(0x98,0x88);
	CAMERA_SetReg(0x99,0x9D);
	CAMERA_SetReg(0x9A,0xFF);
	CAMERA_SetReg(0x9B,0xB1);
	CAMERA_SetReg(0x9C,0xC4);
	CAMERA_SetReg(0x9D,0xD5);
	CAMERA_SetReg(0x9E,0xFC);
	CAMERA_SetReg(0x9F,0x05);//B  
	CAMERA_SetReg(0xA0,0x18);
	CAMERA_SetReg(0xA1,0x42);
	CAMERA_SetReg(0xA2,0xd7);          
	CAMERA_SetReg(0xA3,0x00);
	CAMERA_SetReg(0xA4,0xB6);
	CAMERA_SetReg(0xA5,0x3b);     
	CAMERA_SetReg(0xA6,0x88);
	CAMERA_SetReg(0xA7,0xC8);
	CAMERA_SetReg(0xA8,0x6A);
	CAMERA_SetReg(0xA9,0x00);
	CAMERA_SetReg(0xAA,0x30);
	CAMERA_SetReg(0xAB,0x58);
	CAMERA_SetReg(0xAC,0x78);
	CAMERA_SetReg(0xAD,0xFF);
	CAMERA_SetReg(0xAE,0x90);
	CAMERA_SetReg(0xAF,0xA5);
	CAMERA_SetReg(0xB0,0xB6);
	CAMERA_SetReg(0xB1,0xC5);
	CAMERA_SetReg(0xB2,0xFF);
	CAMERA_SetReg(0xB3,0xD0);
	CAMERA_SetReg(0xB4,0xD6);
	CAMERA_SetReg(0xB5,0xDA);
	CAMERA_SetReg(0xB6,0xFC);
	//========);============================================   
	//  HUE CO);                                    
	//========);============================================   
	 
	CAMERA_SetReg(0xfc,0x00);
	CAMERA_SetReg(0x48,0x34);// 2000K
	CAMERA_SetReg(0x49,0x34);
	CAMERA_SetReg(0x4a,0xf4);
	CAMERA_SetReg(0x4b,0x00);
	CAMERA_SetReg(0x4c,0x44);
	CAMERA_SetReg(0x4d,0x3c);
	CAMERA_SetReg(0x4e,0xf0);
	CAMERA_SetReg(0x4f,0x0c);
	CAMERA_SetReg(0x50,0x34);// 3000K
	CAMERA_SetReg(0x51,0x34);
	CAMERA_SetReg(0x52,0xf4);
	CAMERA_SetReg(0x53,0x00);
	CAMERA_SetReg(0x54,0x44);
	CAMERA_SetReg(0x55,0x3c);
	CAMERA_SetReg(0x56,0xf0);
	CAMERA_SetReg(0x57,0x0c);
	CAMERA_SetReg(0x58,0x34);// 5100K
	CAMERA_SetReg(0x59,0x30);
	CAMERA_SetReg(0x5a,0x00);
	CAMERA_SetReg(0x5b,0x04);
	CAMERA_SetReg(0x5c,0x40);
	CAMERA_SetReg(0x5d,0x2c);
	CAMERA_SetReg(0x5e,0xfc);
	CAMERA_SetReg(0x5f,0x04);
	 
	//========);============================================   
	//  UPPRE0);NCTION                                   
	//========);============================================     
	 
	CAMERA_SetReg(0xfc,0x00);
	CAMERA_SetReg(0x7e,0xf4);

	//========);============================================   
	//  BPR   );                        
	//========);============================================    
	 
	CAMERA_SetReg(0xfc,0x01);     
	CAMERA_SetReg(0x3d,0x10);

	CAMERA_SetReg(0xfc,0x0b);
	CAMERA_SetReg(0x0b,0x00);// ISP BPR On start
	CAMERA_SetReg(0x0c,0x20);// Th13 AGC Min
	CAMERA_SetReg(0x0d,0x40);// Th13 AGC Max
	CAMERA_SetReg(0x0e,0x00);// Th1 Max H for AGCMIN
	CAMERA_SetReg(0x0f,0x20);// Th1 Max L for AGCMIN
	CAMERA_SetReg(0x10,0x00);// Th1 Min H for AGCMAX
	CAMERA_SetReg(0x11,0x10);// Th1 Min L for AGCMAX
	CAMERA_SetReg(0x12,0x00);// Th3 Max H for AGCMIN
	CAMERA_SetReg(0x13,0x00);// Th3 Max L for AGCMIN
	CAMERA_SetReg(0x14,0xff);// Th3 Min H for AGCMAX
	CAMERA_SetReg(0x15,0xff);// Th3 Min L for AGCMAX
	CAMERA_SetReg(0x16,0x20);// Th57 AGC Min
	CAMERA_SetReg(0x17,0x40);// Th57 AGC Max
	CAMERA_SetReg(0x18,0x00);// Th5 Max H for AGCMIN
	CAMERA_SetReg(0x19,0x00);// Th5 Max L for AGCMIN
	CAMERA_SetReg(0x1a,0x00);// Th5 Min H for AGCMAX
	CAMERA_SetReg(0x1b,0x20);// Th5 Min L for AGCMAX
	CAMERA_SetReg(0x1c,0x00);// Th7 Max H for AGCMIN
	CAMERA_SetReg(0x1d,0x00);// Th7 Max L for AGCMIN
	CAMERA_SetReg(0x1e,0x00);// Th7 Min H for AGCMAX
	CAMERA_SetReg(0x1f,0x20);// Th7 Min L for AGCMAX       
	 
	//========);============================================   
	//  GR/GB );TION                                   
	//========);============================================  
	 
	CAMERA_SetReg(0xfc,0x01);
	CAMERA_SetReg(0x45,0x0c); 

	CAMERA_SetReg(0xfc,0x0b);
	CAMERA_SetReg(0x21,0x00);// start AGC
	CAMERA_SetReg(0x22,0x18);// AGCMIN
	CAMERA_SetReg(0x23,0x58);// AGCMAX
	CAMERA_SetReg(0x24,0x0d);// G Th AGCMIN
	CAMERA_SetReg(0x25,0x30);// G Th AGCMAX
	CAMERA_SetReg(0x26,0x0d);// RB Th AGCMIN
	CAMERA_SetReg(0x27,0x30);// RB Th AGCMAX

	//========);============================================   
	//  NR    );                      
	//========);============================================ 
	 
	CAMERA_SetReg(0xfc,0x01);
	CAMERA_SetReg(0x4C,0x01);// NR Enable
	CAMERA_SetReg(0x49,0x15);// Sig_Th Mult
	CAMERA_SetReg(0x4B,0x0A);// Pre_Th Mult

	CAMERA_SetReg(0xfc,0x0b);                          
	CAMERA_SetReg(0x28,0x00);// NR start AGC            
	CAMERA_SetReg(0x29,0x00);// SIG Th AGCMIN H   
	CAMERA_SetReg(0x2a,0x14);// SIG Th AGCMIN L   
	CAMERA_SetReg(0x2b,0x00);// SIG Th AGCMAX H   
	CAMERA_SetReg(0x2c,0x14);// SIG Th AGCMAX L   
	CAMERA_SetReg(0x2d,0x00);// PRE Th AGCMIN H   
	CAMERA_SetReg(0x2e,0x90);// PRE Th AGCMIN L   
	CAMERA_SetReg(0x2f,0x01);// PRE Th AGCMAX H   
	CAMERA_SetReg(0x30,0x00);// PRE Th AGCMAX L   
	CAMERA_SetReg(0x31,0x00);// POST Th AGCMIN H  
	CAMERA_SetReg(0x32,0xa0);// POST Th AGCMIN L  
	CAMERA_SetReg(0x33,0x01);// POST Th AGCMAX H
	CAMERA_SetReg(0x34,0x10);// POST Th AGCMAX L  
	 
	//========);============================================   
	//  1D-Y/C);-LPF                                
	//========);============================================   
	 
	CAMERA_SetReg(0xfc,0x01);                         
	CAMERA_SetReg(0x05,0xc0);        

	CAMERA_SetReg(0xfc,0x0b);                         
	CAMERA_SetReg(0x35,0x00);// YLPF start AGC      
	CAMERA_SetReg(0x36,0x40);// YLPF01 AGCMIN       
	CAMERA_SetReg(0x37,0x60);// YLPF01 AGCMAX       
	CAMERA_SetReg(0x38,0x00);// YLPF SIG01 Th AGCMINH
	CAMERA_SetReg(0x39,0x18);// YLPF SIG01 Th AGCMINL   
	CAMERA_SetReg(0x3a,0x00);// YLPF SIG01 Th AGCMAXH   
	CAMERA_SetReg(0x3b,0x40);// YLPF SIG01 Th AGCMAXH               
	CAMERA_SetReg(0x3c,0x50);// YLPF02 AGCMIN                   
	CAMERA_SetReg(0x3d,0x60);// YLPF02 AGCMAX                   
	CAMERA_SetReg(0x3e,0x00);// YLPF SIG02 Th AGCMINH   
	CAMERA_SetReg(0x3f,0x30);// YLPF SIG02 Th AGCMINL   
	CAMERA_SetReg(0x40,0x00);// YLPF SIG02 Th AGCMAXH   
	CAMERA_SetReg(0x41,0x40);// YLPF SIG02 Th AGCMAXH   
	CAMERA_SetReg(0xd4,0x40);// CLPF AGCMIN       
	CAMERA_SetReg(0xd5,0x60);// CLPF AGCMAX       
	CAMERA_SetReg(0xd6,0xb0);// CLPF SIG01 Th AGCMIN
	CAMERA_SetReg(0xd7,0xf0);// CLPF SIG01 Th AGCMAX  
	CAMERA_SetReg(0xd8,0xb0);// CLPF SIG02 Th AGCMIN  
	CAMERA_SetReg(0xd9,0xf0);// CLPF SIG02 Th AGCMAX     
	 
	//========);============================================   
	//  COLOR );SS                                
	//========);============================================   
	 
	CAMERA_SetReg(0xfc,0x0b);
	CAMERA_SetReg(0x08,0x58);// Color suppress AGC MIN
	CAMERA_SetReg(0x09,0x03);// Color suppress MIN H
	CAMERA_SetReg(0x0a,0x80);// Color suppress MIN L    
	 
	//========);============================================   
	//  SHADIN);                         
	//========);============================================   
	 
	CAMERA_SetReg(0xfc,0x09);
	//Shading);for 3BAFX
	//s90000//);ng off
	// DSP9_SH);_H 
	 
	CAMERA_SetReg(0x01,0x06);
	CAMERA_SetReg(0x02,0x40);

	// DSP9_SH);T_H 
	CAMERA_SetReg(0x03,0x04);
	CAMERA_SetReg(0x04,0xB0);
	// DSP9_SH); 
	CAMERA_SetReg(0x05,0x03);
	CAMERA_SetReg(0x06,0x1A);
	CAMERA_SetReg(0x07,0x02);
	CAMERA_SetReg(0x08,0x4E);
	// DSP9_SH); 
	CAMERA_SetReg(0x09,0x03);
	CAMERA_SetReg(0x0A,0x27);
	CAMERA_SetReg(0x0B,0x02);
	CAMERA_SetReg(0x0C,0x11);
	 
	// DSP9_SH); 
	CAMERA_SetReg(0x0D,0x03);
	CAMERA_SetReg(0x0E,0x15);
	CAMERA_SetReg(0x0F,0x01);
	CAMERA_SetReg(0x10,0xE3);
	// DSP9_SH);H_R 
	CAMERA_SetReg(0x1D,0x85);
	CAMERA_SetReg(0x1E,0x55);
	CAMERA_SetReg(0x1F,0x77);
	CAMERA_SetReg(0x20,0x9E);
	CAMERA_SetReg(0x23,0x7F);
	CAMERA_SetReg(0x24,0xE6);
	CAMERA_SetReg(0x21,0x7F);
	CAMERA_SetReg(0x22,0xE6);
	 
	// DSP9_SH);H_G 
	 
	CAMERA_SetReg(0x25,0x82);
	CAMERA_SetReg(0x26,0x9A);
	CAMERA_SetReg(0x27,0x78);
	CAMERA_SetReg(0x28,0xC0);
	CAMERA_SetReg(0x2B,0x76);
	CAMERA_SetReg(0x2C,0x07);
	CAMERA_SetReg(0x29,0x86);
	CAMERA_SetReg(0x2A,0x09);
	 
	// DSP9_SH);H_B 
	CAMERA_SetReg(0x2D,0x85);
	CAMERA_SetReg(0x2E,0x55);
	CAMERA_SetReg(0x2F,0x75);
	CAMERA_SetReg(0x30,0x6D);
	CAMERA_SetReg(0x33,0x74);
	CAMERA_SetReg(0x34,0xA2);
	CAMERA_SetReg(0x31,0x84);
	CAMERA_SetReg(0x32,0xA2);
	 
	// DSP9_SH);0H 
	 
	CAMERA_SetReg(0x35,0x01);
	CAMERA_SetReg(0x36,0x01);
	CAMERA_SetReg(0x37,0x01);
	CAMERA_SetReg(0x38,0x14);
	CAMERA_SetReg(0x39,0x01);
	CAMERA_SetReg(0x3A,0x45);
	CAMERA_SetReg(0x3B,0x01);
	CAMERA_SetReg(0x3C,0x8A);
	CAMERA_SetReg(0x3D,0x01);
	CAMERA_SetReg(0x3E,0xA3);
	CAMERA_SetReg(0x3F,0x01);
	CAMERA_SetReg(0x40,0xB9);
	CAMERA_SetReg(0x41,0x01);
	CAMERA_SetReg(0x42,0xD9);
	CAMERA_SetReg(0x43,0x01);
	CAMERA_SetReg(0x44,0xF6);
	// DSP9_SH);0H 
	CAMERA_SetReg(0x45,0x01);
	CAMERA_SetReg(0x46,0x00);
	CAMERA_SetReg(0x47,0x01);
	CAMERA_SetReg(0x48,0x0E);
	CAMERA_SetReg(0x49,0x01);
	CAMERA_SetReg(0x4A,0x34);
	CAMERA_SetReg(0x4B,0x01);
	CAMERA_SetReg(0x4C,0x68);
	CAMERA_SetReg(0x4D,0x01);
	CAMERA_SetReg(0x4E,0x76);
	CAMERA_SetReg(0x4F,0x01);
	CAMERA_SetReg(0x50,0x94);
	CAMERA_SetReg(0x51,0x01);
	CAMERA_SetReg(0x52,0xAB);
	CAMERA_SetReg(0x53,0x01);
	CAMERA_SetReg(0x54,0xC3);
	// DSP9_SH);0H 
	CAMERA_SetReg(0x55,0x01);
	CAMERA_SetReg(0x56,0x00);
	CAMERA_SetReg(0x57,0x01);
	CAMERA_SetReg(0x58,0x0C);
	CAMERA_SetReg(0x59,0x01);
	CAMERA_SetReg(0x5A,0x2B);
	CAMERA_SetReg(0x5B,0x01);
	CAMERA_SetReg(0x5C,0x5D);
	CAMERA_SetReg(0x5D,0x01);
	CAMERA_SetReg(0x5E,0x70);
	CAMERA_SetReg(0x5F,0x01);
	CAMERA_SetReg(0x60,0x8A);
	CAMERA_SetReg(0x61,0x01);
	CAMERA_SetReg(0x62,0xA1);
	CAMERA_SetReg(0x63,0x01);
	CAMERA_SetReg(0x64,0xB3);
	// DSP9_SH);R1H 
	CAMERA_SetReg(0x65,0x00);
	CAMERA_SetReg(0x66,0x98);
	CAMERA_SetReg(0x67,0x2C);
	CAMERA_SetReg(0x68,0x02);
	CAMERA_SetReg(0x69,0x60);
	CAMERA_SetReg(0x6A,0xB0);
	CAMERA_SetReg(0x6B,0x05);
	CAMERA_SetReg(0x6C,0x59);
	CAMERA_SetReg(0x6D,0x8C);
	CAMERA_SetReg(0x6E,0x07);
	CAMERA_SetReg(0x6F,0x48);
	CAMERA_SetReg(0x70,0x1B);
	CAMERA_SetReg(0x71,0x09);
	CAMERA_SetReg(0x72,0x82);
	CAMERA_SetReg(0x73,0xC0);
	CAMERA_SetReg(0x74,0x0C);
	CAMERA_SetReg(0x75,0x09);
	CAMERA_SetReg(0x76,0x7B);
	CAMERA_SetReg(0x77,0x0E);
	CAMERA_SetReg(0x78,0xDC);
	CAMERA_SetReg(0x79,0x4D);
	// DSP9_SH);G1H 
	CAMERA_SetReg(0x7A,0x00);
	CAMERA_SetReg(0x7B,0xAD);
	CAMERA_SetReg(0x7C,0x76);
	CAMERA_SetReg(0x7D,0x02);
	CAMERA_SetReg(0x7E,0xB5);
	CAMERA_SetReg(0x7F,0xD7);
	CAMERA_SetReg(0x80,0x06);
	CAMERA_SetReg(0x81,0x19);
	CAMERA_SetReg(0x82,0x23);
	CAMERA_SetReg(0x83,0x08);
	CAMERA_SetReg(0x84,0x4C);
	CAMERA_SetReg(0x85,0xE2);
	CAMERA_SetReg(0x86,0x0A);
	CAMERA_SetReg(0x87,0xD7);
	CAMERA_SetReg(0x88,0x5C);
	CAMERA_SetReg(0x89,0x0D);
	CAMERA_SetReg(0x8A,0xB8);
	CAMERA_SetReg(0x8B,0x90);
	CAMERA_SetReg(0x8C,0x10);
	CAMERA_SetReg(0x8D,0xF0);
	CAMERA_SetReg(0x8E,0x7F);
	// DSP9_SH);B1H 
	CAMERA_SetReg(0x8F,0x00);
	CAMERA_SetReg(0x90,0xC1);
	CAMERA_SetReg(0x91,0xD0);
	CAMERA_SetReg(0x92,0x03);
	CAMERA_SetReg(0x93,0x07);
	CAMERA_SetReg(0x94,0x3F);
	CAMERA_SetReg(0x95,0x06);
	CAMERA_SetReg(0x96,0xD0);
	CAMERA_SetReg(0x97,0x4F);
	CAMERA_SetReg(0x98,0x09);
	CAMERA_SetReg(0x99,0x46);
	CAMERA_SetReg(0x9A,0x32);
	CAMERA_SetReg(0x9B,0x0C);
	CAMERA_SetReg(0x9C,0x1C);
	CAMERA_SetReg(0x9D,0xFE);
	CAMERA_SetReg(0x9E,0x0F);
	CAMERA_SetReg(0x9F,0x54);
	CAMERA_SetReg(0xA0,0xB1);
	CAMERA_SetReg(0xA1,0x12);
	CAMERA_SetReg(0xA2,0xED);
	CAMERA_SetReg(0xA3,0x4C);
	 
	// DSP9_SH);R0H 
	CAMERA_SetReg(0xA4,0x6B);
	CAMERA_SetReg(0xA5,0xAA);
	CAMERA_SetReg(0xA6,0x23);
	CAMERA_SetReg(0xA7,0xE3);
	CAMERA_SetReg(0xA8,0x15);
	CAMERA_SetReg(0xA9,0x88);
	CAMERA_SetReg(0xAA,0x21);
	CAMERA_SetReg(0xAB,0x20);
	CAMERA_SetReg(0xAC,0x1C);
	CAMERA_SetReg(0xAD,0xB6);
	CAMERA_SetReg(0xAE,0x19);
	CAMERA_SetReg(0xAF,0x55);
	CAMERA_SetReg(0xB0,0x16);
	CAMERA_SetReg(0xB1,0xAA);
	// DSP9_SH);G0H 
	CAMERA_SetReg(0xB2,0x5E);
	CAMERA_SetReg(0xB3,0x74);
	CAMERA_SetReg(0xB4,0x1F);
	CAMERA_SetReg(0xB5,0x7C);
	CAMERA_SetReg(0xB6,0x12);
	CAMERA_SetReg(0xB7,0xE4);
	CAMERA_SetReg(0xB8,0x1D);
	CAMERA_SetReg(0xB9,0x10);
	CAMERA_SetReg(0xBA,0x19);
	CAMERA_SetReg(0xBB,0x30);
	CAMERA_SetReg(0xBC,0x16);
	CAMERA_SetReg(0xBD,0x39);
	CAMERA_SetReg(0xBE,0x13);
	CAMERA_SetReg(0xBF,0xE2);
	// DSP9_SH);B0H 
	CAMERA_SetReg(0xC0,0x54);
	CAMERA_SetReg(0xC1,0x89);
	CAMERA_SetReg(0xC2,0x1C);
	CAMERA_SetReg(0xC3,0x2D);
	CAMERA_SetReg(0xC4,0x10);
	CAMERA_SetReg(0xC5,0xE8);
	CAMERA_SetReg(0xC6,0x1A);
	CAMERA_SetReg(0xC7,0x02);
	CAMERA_SetReg(0xC8,0x16);
	CAMERA_SetReg(0xC9,0x8A);
	CAMERA_SetReg(0xCA,0x13);
	CAMERA_SetReg(0xCB,0xE4);
	CAMERA_SetReg(0xCC,0x11);
	CAMERA_SetReg(0xCD,0xCC);
	CAMERA_SetReg(0x00,0x02); //0xhading on
		 
	//========);============================================   
	//  X-SHAD);                           
	//========);============================================   
	 
	CAMERA_SetReg(0xfc,0x1B);
	CAMERA_SetReg(0x80,0x01);
	CAMERA_SetReg(0x81,0x00);
	CAMERA_SetReg(0x82,0x4C);
	CAMERA_SetReg(0x83,0x00);
	CAMERA_SetReg(0x84,0x86);
	CAMERA_SetReg(0x85,0x03);
	CAMERA_SetReg(0x86,0x5E);
	CAMERA_SetReg(0x87,0x00);
	CAMERA_SetReg(0x88,0x07);
	CAMERA_SetReg(0x89,0xA4);
	CAMERA_SetReg(0x90,0x00);
	CAMERA_SetReg(0x91,0x12);
	CAMERA_SetReg(0x92,0x00);
	CAMERA_SetReg(0x93,0x12);
	CAMERA_SetReg(0x94,0x00);
	CAMERA_SetReg(0x95,0x12);
	CAMERA_SetReg(0x96,0x00);
	CAMERA_SetReg(0x97,0x12);
	CAMERA_SetReg(0x98,0x00);
	CAMERA_SetReg(0x99,0x12);
	CAMERA_SetReg(0x9A,0x00);
	CAMERA_SetReg(0x9B,0x12);
	CAMERA_SetReg(0x9C,0x00);
	CAMERA_SetReg(0x9D,0x12);
	CAMERA_SetReg(0x9E,0x00);
	CAMERA_SetReg(0x9F,0x12);
	CAMERA_SetReg(0xA0,0x00);
	CAMERA_SetReg(0xA1,0x12);
	CAMERA_SetReg(0xA2,0x00);
	CAMERA_SetReg(0xA3,0x12);
	CAMERA_SetReg(0xA4,0x00);
	CAMERA_SetReg(0xA5,0x12);
	CAMERA_SetReg(0xA6,0x00);
	CAMERA_SetReg(0xA7,0x12);
	CAMERA_SetReg(0xA8,0x00);
	CAMERA_SetReg(0xA9,0x12);
	CAMERA_SetReg(0xAA,0x00);
	CAMERA_SetReg(0xAB,0x12);
	CAMERA_SetReg(0xAC,0x00);
	CAMERA_SetReg(0xAD,0x12);
	CAMERA_SetReg(0xAE,0x00);
	CAMERA_SetReg(0xAF,0x12);
	CAMERA_SetReg(0xB0,0x00);
	CAMERA_SetReg(0xB1,0x12);
	CAMERA_SetReg(0xB2,0x00);
	CAMERA_SetReg(0xB3,0x12);
	CAMERA_SetReg(0xB4,0x00);
	CAMERA_SetReg(0xB5,0x12);
	CAMERA_SetReg(0xB6,0x00);
	CAMERA_SetReg(0xB7,0x15);
	CAMERA_SetReg(0xB8,0x00);
	CAMERA_SetReg(0xB9,0x12);
	CAMERA_SetReg(0xBA,0x00);
	CAMERA_SetReg(0xBB,0x12);
	CAMERA_SetReg(0xBC,0x00);
	CAMERA_SetReg(0xBD,0x12);
	CAMERA_SetReg(0xBE,0x00);
	CAMERA_SetReg(0xBF,0x12);
	CAMERA_SetReg(0xC0,0x00);
	CAMERA_SetReg(0xC1,0x12);
	CAMERA_SetReg(0xC2,0x00);
	CAMERA_SetReg(0xC3,0x12);
	CAMERA_SetReg(0xC4,0x00);
	CAMERA_SetReg(0xC5,0x12);
	CAMERA_SetReg(0xC6,0x00);
	CAMERA_SetReg(0xC7,0x12);
	CAMERA_SetReg(0xC8,0x00);
	CAMERA_SetReg(0xC9,0x12);
	CAMERA_SetReg(0xCA,0x00);
	CAMERA_SetReg(0xCB,0x12);
	CAMERA_SetReg(0xCC,0x00);
	CAMERA_SetReg(0xCD,0x12);
	CAMERA_SetReg(0xCE,0x00);
	CAMERA_SetReg(0xCF,0x12);
	CAMERA_SetReg(0xD0,0x00);
	CAMERA_SetReg(0xD1,0x12);
	CAMERA_SetReg(0xD2,0x00);
	CAMERA_SetReg(0xD3,0x12);
	CAMERA_SetReg(0xD4,0x00);
	CAMERA_SetReg(0xD5,0x12);
	// x-);g temp. correlation factor
	CAMERA_SetReg(0xfc,0x0b);               
	CAMERA_SetReg(0xda,0x00);// t0(3100K)   
	CAMERA_SetReg(0xdb,0xac);               
	CAMERA_SetReg(0xdc,0x01);// tc(5100K)   
	CAMERA_SetReg(0xdd,0x30);// default eeh 
		 
	CAMERA_SetReg(0xfc,0x00);               
	CAMERA_SetReg(0x81,0x10);// xshading tem
	CAMERA_SetReg(0xfc,0x1b);               
	CAMERA_SetReg(0x80,0x01);// X-Shading On
	 
	//========);============================================   
	//  AE WIN);IGHT          
	//========);============================================  
	 
	CAMERA_SetReg(0xfc,0x00);
	CAMERA_SetReg(0x03,0x4b);// AE Suppress On
	CAMERA_SetReg(0xfc,0x06);
	CAMERA_SetReg(0x01,0x35);// UXGA AE Window
	CAMERA_SetReg(0x03,0xc2);
	 
	CAMERA_SetReg(0x05,0x48);
	CAMERA_SetReg(0x07,0xb8);
	CAMERA_SetReg(0x31,0x2a);// Subsampling AE Window
	CAMERA_SetReg(0x33,0x61);
	CAMERA_SetReg(0x35,0x28);
	CAMERA_SetReg(0x37,0x5c);
	CAMERA_SetReg(0x39,0x28);
	CAMERA_SetReg(0x3B,0x5A);
	CAMERA_SetReg(0x3D,0x10);// 1c
	CAMERA_SetReg(0x3F,0x44);
	CAMERA_SetReg(0xfc,0x20);
	CAMERA_SetReg(0x60,0x11);
	CAMERA_SetReg(0x61,0x11);
	CAMERA_SetReg(0x62,0x11);
	CAMERA_SetReg(0x63,0x11);
	CAMERA_SetReg(0x64,0x11);
	CAMERA_SetReg(0x65,0x22);
	CAMERA_SetReg(0x66,0x22);
	CAMERA_SetReg(0x67,0x11);
	CAMERA_SetReg(0x68,0x11);
	CAMERA_SetReg(0x69,0x33);
	CAMERA_SetReg(0x6a,0x33);
	CAMERA_SetReg(0x6b,0x11);
	CAMERA_SetReg(0x6c,0x12);
	CAMERA_SetReg(0x6d,0x55);
	CAMERA_SetReg(0x6e,0x55);
	CAMERA_SetReg(0x6f,0x21);
	CAMERA_SetReg(0x70,0x13);
	CAMERA_SetReg(0x71,0x55);
	CAMERA_SetReg(0x72,0x55);
	CAMERA_SetReg(0x73,0x31);
	CAMERA_SetReg(0x74,0x33);
	CAMERA_SetReg(0x75,0x33);
	CAMERA_SetReg(0x76,0x33);
	CAMERA_SetReg(0x77,0x33);
	//========);============================================   
	//  SAIT A);            
	//========);============================================   
	//========);===================
	// White P);
	//========);===================  
	 
	CAMERA_SetReg(0xfc,0x22);// White Point (For Hue Control & MWB)    
	CAMERA_SetReg(0x01,0xD0);// D65
	CAMERA_SetReg(0x03,0x9B); 
	CAMERA_SetReg(0x05,0xC0);// 5000K                                   
	CAMERA_SetReg(0x07,0xB8);
	CAMERA_SetReg(0x09,0xA7);// CWF
	CAMERA_SetReg(0x0b,0xDC);
	CAMERA_SetReg(0x0d,0x98);// 3000K
	CAMERA_SetReg(0x0f,0xE0);                                    
	CAMERA_SetReg(0x11,0x85);// A 
	CAMERA_SetReg(0x12,0x00);                                    
	CAMERA_SetReg(0x13,0xF6);                                        
	CAMERA_SetReg(0x15,0x80);// 2000K 
	CAMERA_SetReg(0x16,0x01);                           
	CAMERA_SetReg(0x17,0x00);              
	//========);===================
	// Basic S); 
	//========);===================
	 
	CAMERA_SetReg(0xfc,0x22);
	CAMERA_SetReg(0xA0,0x01);
	CAMERA_SetReg(0xA1,0x3F);
	CAMERA_SetReg(0xA2,0x0E);
	CAMERA_SetReg(0xA3,0x65);
	CAMERA_SetReg(0xA4,0x07);
	CAMERA_SetReg(0xA5,0xF4);
	CAMERA_SetReg(0xA6,0x11);
	CAMERA_SetReg(0xA7,0xC8);
	CAMERA_SetReg(0xA9,0x02);
	CAMERA_SetReg(0xAA,0x43);
	CAMERA_SetReg(0xAB,0x26);
	CAMERA_SetReg(0xAC,0x1F);
	CAMERA_SetReg(0xAD,0x02);
	CAMERA_SetReg(0xAE,0x2C);
	CAMERA_SetReg(0xAF,0x19);
	CAMERA_SetReg(0xB0,0x0F);
	 
	 
	CAMERA_SetReg(0x94,0x3C);
	CAMERA_SetReg(0x95,0xCC);
	CAMERA_SetReg(0x96,0x5C);
	CAMERA_SetReg(0x97,0x4D);
	CAMERA_SetReg(0xD0,0xA8);
	CAMERA_SetReg(0xD1,0x29);
	CAMERA_SetReg(0xD2,0x39);
	CAMERA_SetReg(0xD3,0x22);
	CAMERA_SetReg(0xD4,0x30);
	CAMERA_SetReg(0xDB,0x29);
	CAMERA_SetReg(0xDC,0x7E);
	CAMERA_SetReg(0xDD,0x22);
	CAMERA_SetReg(0xE7,0x00);
	CAMERA_SetReg(0xE8,0xca);
	CAMERA_SetReg(0xE9,0x00);
	CAMERA_SetReg(0xEA,0x62);
	CAMERA_SetReg(0xEB,0x00);
	CAMERA_SetReg(0xEC,0x00);
	CAMERA_SetReg(0xEE,0x97);
	 
	//========);===================
	// Pixel F);Setting 
	//========);===================
	 
	CAMERA_SetReg(0xFC,0x07);
	CAMERA_SetReg(0x95,0x8F);
	CAMERA_SetReg(0xfc,0x01);
	CAMERA_SetReg(0xD3,0x4B);
	CAMERA_SetReg(0xD4,0x00);
	CAMERA_SetReg(0xD5,0x38);
	CAMERA_SetReg(0xD6,0x00);
	CAMERA_SetReg(0xD7,0x60);
	CAMERA_SetReg(0xD8,0x00);
	CAMERA_SetReg(0xD9,0x4E);
	CAMERA_SetReg(0xDA,0x00);
	CAMERA_SetReg(0xDB,0x27);
	CAMERA_SetReg(0xDC,0x15);
	CAMERA_SetReg(0xDD,0x23);
	CAMERA_SetReg(0xDE,0xAD);
	CAMERA_SetReg(0xDF,0x24);
	CAMERA_SetReg(0xE0,0x01);
	CAMERA_SetReg(0xE1,0x17);
	CAMERA_SetReg(0xE2,0x4A);
	CAMERA_SetReg(0xE3,0x36);
	CAMERA_SetReg(0xE4,0x40);
	CAMERA_SetReg(0xE5,0x40);
	CAMERA_SetReg(0xE6,0x40);
	CAMERA_SetReg(0xE7,0x40);
	CAMERA_SetReg(0xE8,0x30);
	CAMERA_SetReg(0xE9,0x3D);
	CAMERA_SetReg(0xEA,0x17);
	CAMERA_SetReg(0xEB,0x01);

	//========);===================
	// Polygon);egion Tune
	//========);===================
	 
	CAMERA_SetReg(0xfc,0x22);
	CAMERA_SetReg(0x18,0x00);// 1
	CAMERA_SetReg(0x19,0x5a);
	CAMERA_SetReg(0x1a,0xf8);
	CAMERA_SetReg(0x1b,0x00);// 2
	CAMERA_SetReg(0x1c,0x59);
	CAMERA_SetReg(0x1d,0xCC);
	CAMERA_SetReg(0x1e,0x00);// 3
	CAMERA_SetReg(0x1f,0x74);
	CAMERA_SetReg(0x20,0xB3);
	CAMERA_SetReg(0x21,0x00);// 4
	CAMERA_SetReg(0x22,0x86);
	CAMERA_SetReg(0x23,0xA2);
	 
	CAMERA_SetReg(0x24,0x00);// 5
	CAMERA_SetReg(0x25,0x94);
	CAMERA_SetReg(0x26,0x89);                      
	CAMERA_SetReg(0x27,0x00);// 6 
	CAMERA_SetReg(0x28,0xA6);
	CAMERA_SetReg(0x29,0x76);
	CAMERA_SetReg(0x2A,0x00);// 7 
	CAMERA_SetReg(0x2B,0xd0);
	CAMERA_SetReg(0x2C,0x5e);
	CAMERA_SetReg(0x2D,0x00);// 8
	CAMERA_SetReg(0x2E,0xfa);
	CAMERA_SetReg(0x2F,0x47);
	CAMERA_SetReg(0x30,0x00);// 9
	CAMERA_SetReg(0x31,0xfD);
	CAMERA_SetReg(0x32,0x5D);
	CAMERA_SetReg(0x33,0x00);// 10
	CAMERA_SetReg(0x34,0xBB);
	CAMERA_SetReg(0x35,0x7c);
	CAMERA_SetReg(0x36,0x00);// 11
	CAMERA_SetReg(0x37,0xAD);
	CAMERA_SetReg(0x38,0x88);
	CAMERA_SetReg(0x39,0x00);// 12
	CAMERA_SetReg(0x3A,0x9A);
	CAMERA_SetReg(0x3B,0xA3);
	CAMERA_SetReg(0x3C,0x00);// 13
	CAMERA_SetReg(0x3D,0x7C);
	CAMERA_SetReg(0x3E,0xDD);
	CAMERA_SetReg(0x3F,0x00);// 14
	CAMERA_SetReg(0x40,0x00);
	CAMERA_SetReg(0x41,0x00);

	//========);=================== 
	// Moving );on Weight            
	//========);===================
	 
	CAMERA_SetReg(0xfc,0x22);
	CAMERA_SetReg(0x98,0x07);
	 
	//========);===================
	// EIT Thr);
	//========);===================
	 
	CAMERA_SetReg(0xfc,0x22);
	CAMERA_SetReg(0xb1,0x00);// {0xunny
	CAMERA_SetReg(0xb2,0x03);
	CAMERA_SetReg(0xb3,0x00);
	CAMERA_SetReg(0xb4,0xc1);
	CAMERA_SetReg(0xb5,0x00);// Cloudy  
	CAMERA_SetReg(0xb6,0x05);
	CAMERA_SetReg(0xb7,0xc9);
	CAMERA_SetReg(0xb9,0x81);
	CAMERA_SetReg(0xd7,0x00);// Shade 
	CAMERA_SetReg(0xd8,0x35);
	CAMERA_SetReg(0xd9,0x20);
	CAMERA_SetReg(0xda,0x81);
	//========);===================
	// Gain Of);
	//========);===================
	CAMERA_SetReg(0xfc,0x00);
	CAMERA_SetReg(0x79,0xF9);
	CAMERA_SetReg(0x7A,0x02);// Global AWB gain off{0xet
	CAMERA_SetReg(0xfc,0x22);                            
	CAMERA_SetReg(0x58,0xf6);// D65 R Off{0xet              
	CAMERA_SetReg(0x59,0xff);// D65 B Off{0xet              
	CAMERA_SetReg(0x5A,0xfa);// 5000K R Off{0xet            
	CAMERA_SetReg(0x5B,0xFe);// 5000K B Off{0xet            
	CAMERA_SetReg(0x5C,0xfb);// CWF R Off{0xet              
	CAMERA_SetReg(0x5D,0xFe);// CWF B Off{0xet              
	CAMERA_SetReg(0x5E,0xfb);// 3000K R Off{0xet            
	CAMERA_SetReg(0x5F,0xFb);// 3000K B Off{0xet            
	CAMERA_SetReg(0x60,0xfb);// A R Off0xet                
	CAMERA_SetReg(0x61,0xfb);// A B Off0xet                
	CAMERA_SetReg(0x62,0xfb);// 2000K R Off0xet            
	CAMERA_SetReg(0x63,0xfb);// 2000K B Off0xet            
	CAMERA_SetReg(0xde,0x00);// LARGE OBJECT BUG FIX
	CAMERA_SetReg(0xf0,0x6a);// RB Ratio
	//========);===================
	// Green S);y Enhance
	//========);===================
	 
	CAMERA_SetReg(0xfc,0x22);
	CAMERA_SetReg(0xb9,0x00);
	CAMERA_SetReg(0xba,0x00);
	CAMERA_SetReg(0xbb,0x00);
	CAMERA_SetReg(0xbc,0x00);
	CAMERA_SetReg(0xe5,0x01);
	CAMERA_SetReg(0xe6,0xff);
	CAMERA_SetReg(0xbd,0x90);
	 
	//========);============================================   
	//  Specia);ct
	//========);============================================            
	 
	CAMERA_SetReg(0xfc,0x07);// Special Effect
	CAMERA_SetReg(0x30,0xc0);
	CAMERA_SetReg(0x31,0x20);
	CAMERA_SetReg(0x32,0x40);
	CAMERA_SetReg(0x33,0xc0);
	CAMERA_SetReg(0x34,0x00);
	CAMERA_SetReg(0x35,0xb0);
	 
	CAMERA_SetReg(0xfc,0x00);            
	CAMERA_SetReg(0x73,0x21);// Frmae AE Enable}, peter0223 위치 변경

	CAMERA_SetReg(0xfc,0x04);
	CAMERA_SetReg(0xc0,0x06);
	CAMERA_SetReg(0xc1,0x70);
	CAMERA_SetReg(0xFF,0xFF);//REGISTER END

	CAMERA_IICClose();

}


void CAMERA_InitS5K4BAF(CAM_ATTR eCcir, CSPACE eSrcFmt, SUB_SAMPLING eSub)
{
	u32 uHCLKx2;

	SYSC_GetClkInform(); // Get HCLKx2 Frequency
	uHCLKx2 = g_HCLKx2/1000000;
	
	oCamera.m_ucSlaveAddr = 0x5a;

	CAMERA_IICOpen(200000); 

//==========================================================
//	CAMERA INITIAL (Analog & Clock Setting)
//==========================================================
	CAMERA_SetReg(0xfc, 0x07);
	CAMERA_SetReg(0x66, 0x01);// WDT
	CAMERA_SetReg(0xfc, 0x00);
	CAMERA_SetReg(0x00, 0xaa);// For EDS Check
	CAMERA_SetReg(0x21, 0x03);// peter0223 added
	
	CAMERA_SetReg(0xfc, 0x01);
	CAMERA_SetReg(0x04, 0x01);// ARM Clock Divider
	
	CAMERA_SetReg(0xfc, 0x02);// Analog setting   
	CAMERA_SetReg(0x55, 0x1e);// LineADLC on(s551a), off(s550a)
	CAMERA_SetReg(0x56, 0x10);// BPR 16code
	CAMERA_SetReg(0x30, 0x82);// Analog offset (capture =?h)
	CAMERA_SetReg(0x37, 0x25);// Global Gain (default:31)
	
	CAMERA_SetReg(0x57, 0x80);// // LineADLC Roffset
	CAMERA_SetReg(0x58, 0x80);//89	//90  // LineADLC Goffset
	CAMERA_SetReg(0x59, 0x80);//90  // LineADLC offset don't care
	
	CAMERA_SetReg(0x44, 0x64);//clamp en[6]=1 on
	CAMERA_SetReg(0x4a, 0x30);//clamp level 0011h [7]~[4]
	
	CAMERA_SetReg(0x2d, 0x48);// double shutter (default:00)
	CAMERA_SetReg(0x4d, 0x08);// Voltage doubler (default:04)
	CAMERA_SetReg(0x4e, 0x00);// IO current 8mA set
	CAMERA_SetReg(0x4f, 0x8a);// IO current 48mA set
	
	CAMERA_SetReg(0x66, 0x41);// 1st comp current 2uA
	CAMERA_SetReg(0x43, 0xef);// ec_comp
	CAMERA_SetReg(0x62, 0x60);// LD control , CFPN_EN off



//==========================================================   
//	Table Set for Sub-Sampling
//==========================================================
	CAMERA_SetReg(0xfc, 0x03);
	CAMERA_SetReg(0x01, 0x60);
	//CAMERA_SetReg(0x2e, 0x00);
	CAMERA_SetReg(0x2e, 0x03);//DHL
	CAMERA_SetReg(0x05, 0x46);// Output Image Size Set for Capture
	CAMERA_SetReg(0x07, 0xb6);
	CAMERA_SetReg(0x0e, 0x04);
	CAMERA_SetReg(0x12, 0x03);
	
	CAMERA_SetReg(0xfc, 0x04);
	CAMERA_SetReg(0xc5, 0x26);// Output Image Size Set for Preview
	CAMERA_SetReg(0xc7, 0x5e);
	CAMERA_SetReg(0xce, 0x04);
	CAMERA_SetReg(0xd2, 0x04);
	//CAMERA_SetReg(0xee, 0x00);//DHL
	CAMERA_SetReg(0xee, 0x01);	
	CAMERA_SetReg(0xc0, 0x06);
	CAMERA_SetReg(0xc1, 0x60);//frame_H
	CAMERA_SetReg(0xc2, 0x02);
	CAMERA_SetReg(0xc3, 0x8d);//frame_V
	
	CAMERA_SetReg(0xfc, 0x07);
	CAMERA_SetReg(0x05, 0x00);
	CAMERA_SetReg(0x06, 0x00);
	CAMERA_SetReg(0x07, 0x8b);
	CAMERA_SetReg(0x08, 0xf5);
	CAMERA_SetReg(0x09, 0x00);
	CAMERA_SetReg(0x0a, 0xb4);
	CAMERA_SetReg(0x0b, 0x00);
	CAMERA_SetReg(0x0c, 0xea);
	CAMERA_SetReg(0x0d, 0x00);
	CAMERA_SetReg(0x0e, 0x40);

	
//==========================================================
//	COMMAND SET 
//==========================================================
	CAMERA_SetReg(0xfc, 0x00);
	CAMERA_SetReg(0x70, 0x02);
	
	CAMERA_SetReg(0xfc, 0x00);
	CAMERA_SetReg(0x73, 0x11);//21 Frmae AE Enable, peter0223
	CAMERA_SetReg(0x20, 0x02);// Change AWB Mode
	
	CAMERA_SetReg(0xfc, 0x00);
	CAMERA_SetReg(0x78, 0x6a);// AGC Max
	
	CAMERA_SetReg(0xfc, 0x00);
	CAMERA_SetReg(0x6c, 0xa0);// AE target
	CAMERA_SetReg(0x6d, 0x00);
	
	CAMERA_SetReg(0xfc, 0x20);
	CAMERA_SetReg(0x16, 0x5a);// AGC frame AE start _for Prevating AE Hunting
	CAMERA_SetReg(0x57, 0x18);// Stable_Frame_AE
	
	CAMERA_SetReg(0xfc, 0x00);
	CAMERA_SetReg(0x83, 0x06);//low condition shutter off // Double shutter off
	
	CAMERA_SetReg(0xfc, 0x0b);
	CAMERA_SetReg(0x5c, 0x69);//70	//AGC value to start shutter on/off suppress
	CAMERA_SetReg(0x5d, 0x65);//60   //AGC value to start double shutter on/off suppress

	CAMERA_SetReg(0xfc, 0x20);
	CAMERA_SetReg(0x25, 0x00);// CINTR Min
	CAMERA_SetReg(0x2a, 0x01);// forbidden
	CAMERA_SetReg(0x2b, 0x02);// For Forbidden Area
	CAMERA_SetReg(0x2c, 0x0a);
	CAMERA_SetReg(0x2d, 0x00);// For Forbidden Area
	CAMERA_SetReg(0x2e, 0x00);
	CAMERA_SetReg(0x2f, 0x05);// forbidden
	CAMERA_SetReg(0x14, 0x78);//70
	CAMERA_SetReg(0x01, 0x00);// Stepless_Off
	
	CAMERA_SetReg(0xfc, 0x00);
	CAMERA_SetReg(0x29, 0x04);// Y level 
	CAMERA_SetReg(0x2a, 0x00);
	CAMERA_SetReg(0x2b, 0x03);// C level
	CAMERA_SetReg(0x2c, 0x80);//60
	
	CAMERA_SetReg(0xfc, 0x07);
	CAMERA_SetReg(0x37, 0x00);// Flicker 
	
	CAMERA_SetReg(0xfc, 0x00);
	CAMERA_SetReg(0x72, 0xa0);// Flicker for 32MHz
	CAMERA_SetReg(0x74, 0x08);// flicker 60Hz fix
	CAMERA_SetReg(0xfc, 0x20);
	CAMERA_SetReg(0x02, 0x12);//02 Flicker Dgain Mode
	CAMERA_SetReg(0xfc, 0x00);
	CAMERA_SetReg(0x62, 0x02);// Hue Control Enable
	
	CAMERA_SetReg(0xfc, 0x01);
	//CAMERA_SetReg(0x0c, 0x02);// Full YC Enable
	CAMERA_SetReg(0x0C, 0x03);//Donghoon

	
//==========================================================
//	COLOR MATRIX
//==========================================================	
	CAMERA_SetReg(0xfc, 0x01);	//DL gain 60
	CAMERA_SetReg(0x51, 0x08);	//06	//08  07	
	CAMERA_SetReg(0x52, 0xe8);	//df	//9B  E7	
	CAMERA_SetReg(0x53, 0xfc);	//fd	//FC  FB	
	CAMERA_SetReg(0x54, 0x33);	//09	//07  B9	
	CAMERA_SetReg(0x55, 0xfe);	//00	//FF  00	
	CAMERA_SetReg(0x56, 0xe6);	//17	//5E  5F	
	CAMERA_SetReg(0x57, 0xfe);	//fe	//FD  FD	
	CAMERA_SetReg(0x58, 0x3d);	//4f	//0E  46	
	CAMERA_SetReg(0x59, 0x08);	//06	//07  05	
	CAMERA_SetReg(0x5a, 0x21);	//9b	//EE  E6	
	CAMERA_SetReg(0x5b, 0xfd);	//ff	//FF  00	
	CAMERA_SetReg(0x5c, 0xa3);	//17	//05  D3	
	CAMERA_SetReg(0x5d, 0xff);	//ff	//FF  FF	
	CAMERA_SetReg(0x5e, 0xbc);	//81	//7A  53	
	CAMERA_SetReg(0x5f, 0xfc);	//fd	//FC  FB	
	CAMERA_SetReg(0x60, 0x96);	//5b	//23  B1	
	CAMERA_SetReg(0x61, 0x07);	//07	//08  08	
	CAMERA_SetReg(0x62, 0xaf);	//24	//64  FD	 

//==========================================================
//	EDGE ENHANCEMENT                                       
//==========================================================
	CAMERA_SetReg(0xfc, 0x05);
	CAMERA_SetReg(0x12, 0x3d);
	CAMERA_SetReg(0x13, 0x3b);
	CAMERA_SetReg(0x14, 0x38);
	CAMERA_SetReg(0x15, 0x3b);
	CAMERA_SetReg(0x16, 0x3d);
	
	CAMERA_SetReg(0x17, 0x3b);
	CAMERA_SetReg(0x18, 0x05);
	CAMERA_SetReg(0x19, 0x09);
	CAMERA_SetReg(0x1a, 0x05);
	CAMERA_SetReg(0x1b, 0x3b);
	
	CAMERA_SetReg(0x1c, 0x38);
	CAMERA_SetReg(0x1d, 0x09);
	CAMERA_SetReg(0x1e, 0x1c);
	CAMERA_SetReg(0x1f, 0x09);
	CAMERA_SetReg(0x20, 0x38);
	
	CAMERA_SetReg(0x21, 0x3b);
	CAMERA_SetReg(0x22, 0x05);
	CAMERA_SetReg(0x23, 0x09);
	CAMERA_SetReg(0x24, 0x05);
	CAMERA_SetReg(0x25, 0x3b);
	
	CAMERA_SetReg(0x26, 0x3d);
	CAMERA_SetReg(0x27, 0x3b);
	CAMERA_SetReg(0x28, 0x38);
	CAMERA_SetReg(0x29, 0x3b);
	CAMERA_SetReg(0x2a, 0x3d);
	
	CAMERA_SetReg(0xfc, 0x00);
	CAMERA_SetReg(0x89, 0x00);// Edge Suppress On
	CAMERA_SetReg(0xfc, 0x0b);
	CAMERA_SetReg(0x42, 0x50);// Edge AGC MIN
	CAMERA_SetReg(0x43, 0x60);// Edge AGC MAX
	CAMERA_SetReg(0x45, 0x18);// positive gain AGC MIN
	CAMERA_SetReg(0x49, 0x06);// positive gain AGC MAX 
	CAMERA_SetReg(0x4d, 0x18);// negative gain AGC MIN
	CAMERA_SetReg(0x51, 0x06);// negative gain AGC MAX
	
	CAMERA_SetReg(0xfc, 0x05);
	CAMERA_SetReg(0x34, 0x28);// APTCLP
	CAMERA_SetReg(0x35, 0x03);// APTSC
	CAMERA_SetReg(0x36, 0x0b);// ENHANCE
	CAMERA_SetReg(0x3f, 0x00);// NON-LIN
	CAMERA_SetReg(0x42, 0x10);// EGFALL
	CAMERA_SetReg(0x43, 0x00);// HLFALL
	CAMERA_SetReg(0x45, 0xa0);// EGREF
	CAMERA_SetReg(0x46, 0x7a);// HLREF
	CAMERA_SetReg(0x47, 0x40);// LLREF
	CAMERA_SetReg(0x48, 0x0c);
	CAMERA_SetReg(0x49, 0x31);// CSSEL  EGSEL  CS_DLY

	CAMERA_SetReg(0x40, 0x41);// Y delay
	
	// New Wide Luma Edge
	CAMERA_SetReg(0xfc, 0x1d);
	CAMERA_SetReg(0x86, 0x00);
	CAMERA_SetReg(0x87, 0x60);
	CAMERA_SetReg(0x88, 0x01);
	CAMERA_SetReg(0x89, 0x20);	
	CAMERA_SetReg(0x8a, 0x00);
	CAMERA_SetReg(0x8b, 0x00);
	CAMERA_SetReg(0x8c, 0x00);
	CAMERA_SetReg(0x8d, 0x00);
	CAMERA_SetReg(0x8e, 0x00);
	CAMERA_SetReg(0x8f, 0x20);
	CAMERA_SetReg(0x90, 0x00);
	CAMERA_SetReg(0x91, 0x00);
	CAMERA_SetReg(0x92, 0x00);
	CAMERA_SetReg(0x93, 0x0a);	
	CAMERA_SetReg(0x94, 0x00);
	CAMERA_SetReg(0x95, 0x00);
	CAMERA_SetReg(0x96, 0x00);
	CAMERA_SetReg(0x97, 0x20);
	CAMERA_SetReg(0x98, 0x00);
	CAMERA_SetReg(0x99, 0x00);
	CAMERA_SetReg(0x9a, 0xff);
	CAMERA_SetReg(0x9b, 0xea);
	CAMERA_SetReg(0x9c, 0xaa);
	CAMERA_SetReg(0x9d, 0xab);	
	CAMERA_SetReg(0x9e, 0xff);
	CAMERA_SetReg(0x9f, 0xf1);
	CAMERA_SetReg(0xa0, 0x55);
	CAMERA_SetReg(0xa1, 0x56);
	CAMERA_SetReg(0xa2, 0x07);
	
	CAMERA_SetReg(0x85, 0x01);
	
//==========================================================
//	GAMMA                                       
//==========================================================	
	CAMERA_SetReg(0xfc, 0x1d);
	CAMERA_SetReg(0x00, 0x0b);
	CAMERA_SetReg(0x01, 0x18);
	CAMERA_SetReg(0x02, 0x3d);
	CAMERA_SetReg(0x03, 0x9c);
	CAMERA_SetReg(0x04, 0x00);
	CAMERA_SetReg(0x05, 0x0c);
	CAMERA_SetReg(0x06, 0x76);
	CAMERA_SetReg(0x07, 0xc2);
	CAMERA_SetReg(0x08, 0x00);
	CAMERA_SetReg(0x09, 0x56);
	CAMERA_SetReg(0x0a, 0x34);
	CAMERA_SetReg(0x0b, 0x60);
	CAMERA_SetReg(0x0c, 0x85);
	CAMERA_SetReg(0x0d, 0xa7);
	CAMERA_SetReg(0x0e, 0xaa);
	CAMERA_SetReg(0x0f, 0xc6);	
	CAMERA_SetReg(0x10, 0xe2);
	CAMERA_SetReg(0x11, 0xfc);
	CAMERA_SetReg(0x12, 0x13);
	CAMERA_SetReg(0x13, 0xab);
	CAMERA_SetReg(0x14, 0x29);
	CAMERA_SetReg(0x15, 0x3c);
	CAMERA_SetReg(0x16, 0x4b);
	CAMERA_SetReg(0x17, 0x5a);
	CAMERA_SetReg(0x18, 0xff);
	CAMERA_SetReg(0x19, 0x69);
	CAMERA_SetReg(0x1a, 0x78);
	CAMERA_SetReg(0x1b, 0x84);
	CAMERA_SetReg(0x1c, 0x91);
	CAMERA_SetReg(0x1d, 0xff);
	CAMERA_SetReg(0x1e, 0x9c);
	CAMERA_SetReg(0x1f, 0xa7);	
	CAMERA_SetReg(0x20, 0xb2);
	CAMERA_SetReg(0x21, 0xbd);
	CAMERA_SetReg(0x22, 0xff);
	CAMERA_SetReg(0x23, 0xc7);
	CAMERA_SetReg(0x24, 0xd2);
	CAMERA_SetReg(0x25, 0xdb);
	CAMERA_SetReg(0x26, 0xe4);
	CAMERA_SetReg(0x27, 0xff);
	CAMERA_SetReg(0x28, 0xec);
	CAMERA_SetReg(0x29, 0xf5);
	CAMERA_SetReg(0x2a, 0xf0);
	CAMERA_SetReg(0x2b, 0x0b);
	CAMERA_SetReg(0x2c, 0x18);
	CAMERA_SetReg(0x2d, 0x3d);
	CAMERA_SetReg(0x2e, 0x9c);
	CAMERA_SetReg(0x2f, 0x00);	
	CAMERA_SetReg(0x30, 0x0c);
	CAMERA_SetReg(0x31, 0x76);
	CAMERA_SetReg(0x32, 0xc2);
	CAMERA_SetReg(0x33, 0x00);
	CAMERA_SetReg(0x34, 0x56);
	CAMERA_SetReg(0x35, 0x34);
	CAMERA_SetReg(0x36, 0x60);
	CAMERA_SetReg(0x37, 0x85);
	CAMERA_SetReg(0x38, 0xa7);
	CAMERA_SetReg(0x39, 0xaa);
	CAMERA_SetReg(0x3a, 0xc6);
	CAMERA_SetReg(0x3b, 0xe2);
	CAMERA_SetReg(0x3c, 0xfc);
	CAMERA_SetReg(0x3d, 0x13);
	CAMERA_SetReg(0x3e, 0xab);
	CAMERA_SetReg(0x3f, 0x29);	
	CAMERA_SetReg(0x40, 0x3c);
	CAMERA_SetReg(0x41, 0x4b);
	CAMERA_SetReg(0x42, 0x5a);
	CAMERA_SetReg(0x43, 0xff);
	CAMERA_SetReg(0x44, 0x69);
	CAMERA_SetReg(0x45, 0x78);
	CAMERA_SetReg(0x46, 0x84);
	CAMERA_SetReg(0x47, 0x91);
	CAMERA_SetReg(0x48, 0xff);
	CAMERA_SetReg(0x49, 0x9c);
	CAMERA_SetReg(0x4a, 0xa7);
	CAMERA_SetReg(0x4b, 0xb2);
	CAMERA_SetReg(0x4c, 0xbd);
	CAMERA_SetReg(0x4d, 0xff);
	CAMERA_SetReg(0x4e, 0xc7);
	CAMERA_SetReg(0x4f, 0xd2);		
	CAMERA_SetReg(0x50, 0xdb);
	CAMERA_SetReg(0x51, 0xe4);
	CAMERA_SetReg(0x52, 0xff);
	CAMERA_SetReg(0x53, 0xec);
	CAMERA_SetReg(0x54, 0xf5);
	CAMERA_SetReg(0x55, 0xf0);
	CAMERA_SetReg(0x56, 0x0b);
	CAMERA_SetReg(0x57, 0x18);
	CAMERA_SetReg(0x58, 0x3d);
	CAMERA_SetReg(0x59, 0x9c);
	CAMERA_SetReg(0x5a, 0x00);
	CAMERA_SetReg(0x5b, 0x0c);
	CAMERA_SetReg(0x5c, 0x76);
	CAMERA_SetReg(0x5d, 0xc2);
	CAMERA_SetReg(0x5e, 0x00);
	CAMERA_SetReg(0x5f, 0x56);		
	CAMERA_SetReg(0x60, 0x34);
	CAMERA_SetReg(0x61, 0x60);
	CAMERA_SetReg(0x62, 0x85);
	CAMERA_SetReg(0x63, 0xa7);
	CAMERA_SetReg(0x64, 0xaa);
	CAMERA_SetReg(0x65, 0xc6);
	CAMERA_SetReg(0x66, 0xe2);
	CAMERA_SetReg(0x67, 0xfc);
	CAMERA_SetReg(0x68, 0x13);
	CAMERA_SetReg(0x69, 0xab);
	CAMERA_SetReg(0x6a, 0x29);
	CAMERA_SetReg(0x6b, 0x3c);
	CAMERA_SetReg(0x6c, 0x4b);
	CAMERA_SetReg(0x6d, 0x5a);
	CAMERA_SetReg(0x6e, 0xff);
	CAMERA_SetReg(0x6f, 0x69);	
	CAMERA_SetReg(0x70, 0x78);
	CAMERA_SetReg(0x71, 0x84);
	CAMERA_SetReg(0x72, 0x91);
	CAMERA_SetReg(0x73, 0xff);
	CAMERA_SetReg(0x74, 0x9c);
	CAMERA_SetReg(0x75, 0xa7);
	CAMERA_SetReg(0x76, 0xb2);
	CAMERA_SetReg(0x77, 0xbd);
	CAMERA_SetReg(0x78, 0xff);
	CAMERA_SetReg(0x79, 0xc7);
	CAMERA_SetReg(0x7a, 0xd2);
	CAMERA_SetReg(0x7b, 0xdb);
	CAMERA_SetReg(0x7c, 0xe4);
	CAMERA_SetReg(0x7d, 0xff);
	CAMERA_SetReg(0x7e, 0xec);
	CAMERA_SetReg(0x7f, 0xf5);
	CAMERA_SetReg(0x80, 0xf0);

//==========================================================   
//	HUE CONTROL                                     
//==========================================================
	CAMERA_SetReg(0xfc, 0x00);
	CAMERA_SetReg(0x48, 0x40);// 2000K
	CAMERA_SetReg(0x49, 0x30);
	CAMERA_SetReg(0x4a, 0x00);
	CAMERA_SetReg(0x4b, 0x00);
	CAMERA_SetReg(0x4c, 0x30);
	CAMERA_SetReg(0x4d, 0x38);
	CAMERA_SetReg(0x4e, 0x00);
	CAMERA_SetReg(0x4f, 0x00);

	CAMERA_SetReg(0x50, 0x40);// 3000K
	CAMERA_SetReg(0x51, 0x30);
	CAMERA_SetReg(0x52, 0x00);
	CAMERA_SetReg(0x53, 0x00);
	CAMERA_SetReg(0x54, 0x30);
	CAMERA_SetReg(0x55, 0x38);
	CAMERA_SetReg(0x56, 0x00);
	CAMERA_SetReg(0x57, 0x00);

	CAMERA_SetReg(0x58, 0x3c);//40	              // 5100K
	CAMERA_SetReg(0x59, 0x30);//4a                   //40
	CAMERA_SetReg(0x5a, 0x00);//0c                   //00
	CAMERA_SetReg(0x5b, 0x00);//00
	CAMERA_SetReg(0x5c, 0x30);//4a
	CAMERA_SetReg(0x5d, 0x38);//40
	CAMERA_SetReg(0x5e, 0x00);//f6                   //15 
	CAMERA_SetReg(0x5f, 0xfc);//00

//==========================================================   
//	SUPPRESS FUNCTION                                   
//==========================================================
	CAMERA_SetReg(0xfc, 0x00);
	CAMERA_SetReg(0x7e, 0xf4);

//==========================================================   
//	BPR                                 
//========================================================== 
	CAMERA_SetReg(0xfc, 0x0b);
	CAMERA_SetReg(0x3d, 0x10);
	
	CAMERA_SetReg(0xfc, 0x0b);
	CAMERA_SetReg(0x0b, 0x00);
	CAMERA_SetReg(0x0c, 0x40);
	CAMERA_SetReg(0x0d, 0x5a);
	CAMERA_SetReg(0x0e, 0x00);
	CAMERA_SetReg(0x0f, 0x20);
	CAMERA_SetReg(0x10, 0x00);
	CAMERA_SetReg(0x11, 0x10);
	CAMERA_SetReg(0x12, 0x00);
	CAMERA_SetReg(0x13, 0x7f);
	CAMERA_SetReg(0x14, 0x03);
	CAMERA_SetReg(0x15, 0xff);
	CAMERA_SetReg(0x16, 0x48);
	CAMERA_SetReg(0x17, 0x60);
	CAMERA_SetReg(0x18, 0x00);
	CAMERA_SetReg(0x19, 0x00);
	CAMERA_SetReg(0x1a, 0x00);
	CAMERA_SetReg(0x1b, 0x20);
	CAMERA_SetReg(0x1c, 0x00);
	CAMERA_SetReg(0x1d, 0x00);
	CAMERA_SetReg(0x1e, 0x00);
	CAMERA_SetReg(0x1f, 0x20);

//==========================================================  	  
//	GR/GB CORRECTION                                      
//========================================================== 
	CAMERA_SetReg(0xfc, 0x01);
	CAMERA_SetReg(0x45, 0x0c);
	CAMERA_SetReg(0xfc, 0x0b);
	CAMERA_SetReg(0x21, 0x00);
	CAMERA_SetReg(0x22, 0x40);
	CAMERA_SetReg(0x23, 0x60);
	CAMERA_SetReg(0x24, 0x0d);
	CAMERA_SetReg(0x25, 0x20);
	CAMERA_SetReg(0x26, 0x0d);
	CAMERA_SetReg(0x27, 0x20);

//==========================================================    
//	NR                                                      
//==========================================================  
	CAMERA_SetReg(0xfc, 0x01);
	CAMERA_SetReg(0x4c, 0x01);
	CAMERA_SetReg(0x49, 0x15);
	CAMERA_SetReg(0x4b, 0x0a);
	
	CAMERA_SetReg(0xfc, 0x0b);
	CAMERA_SetReg(0x28, 0x00);
	CAMERA_SetReg(0x29, 0x00);
	CAMERA_SetReg(0x2a, 0x14);
	CAMERA_SetReg(0x2b, 0x00);
	CAMERA_SetReg(0x2c, 0x14);
	CAMERA_SetReg(0x2d, 0x00);
	CAMERA_SetReg(0x2e, 0xD0);
	CAMERA_SetReg(0x2f, 0x02);
	CAMERA_SetReg(0x30, 0x00);
	CAMERA_SetReg(0x31, 0x00);
	CAMERA_SetReg(0x32, 0xa0);
	CAMERA_SetReg(0x33, 0x00);
	CAMERA_SetReg(0x34, 0xe0);
	
//========================================================== 
//	1D-Y/C-SIGMA-LPF                                     
//==========================================================
	CAMERA_SetReg(0xfc, 0x01);
	CAMERA_SetReg(0x05, 0xC0);
	    
	CAMERA_SetReg(0xfc, 0x0b);
	CAMERA_SetReg(0x35, 0x00);
	CAMERA_SetReg(0x36, 0x40);
	CAMERA_SetReg(0x37, 0x60);
	CAMERA_SetReg(0x38, 0x00);
	CAMERA_SetReg(0x39, 0x18);
	CAMERA_SetReg(0x3a, 0x00);
	CAMERA_SetReg(0x3b, 0x40);
	CAMERA_SetReg(0x3c, 0x50);
	CAMERA_SetReg(0x3d, 0x60);
	CAMERA_SetReg(0x3e, 0x00);
	CAMERA_SetReg(0x3f, 0x30);
	CAMERA_SetReg(0x40, 0x00);
	CAMERA_SetReg(0x41, 0x40);
	CAMERA_SetReg(0xd4, 0x40);
	CAMERA_SetReg(0xd5, 0x60);
	CAMERA_SetReg(0xd6, 0xb0);
	CAMERA_SetReg(0xd7, 0xf0);
	CAMERA_SetReg(0xd8, 0xb0);
	CAMERA_SetReg(0xd9, 0xf0);

//========================================================== 
//	COLOR SUPPRESS                                       
//==========================================================
	CAMERA_SetReg(0xfc, 0x0b);
	CAMERA_SetReg(0x08, 0x58);
	CAMERA_SetReg(0x09, 0x03);
	CAMERA_SetReg(0x0a, 0x00);

//========================================================== 
//	SHADING                                              
//==========================================================
	CAMERA_SetReg(0xfc, 0x09);
	
	CAMERA_SetReg(0x01, 0x06);
	CAMERA_SetReg(0x02, 0x40);
	
	CAMERA_SetReg(0x03, 0x04);
	CAMERA_SetReg(0x04, 0xB0);
	    
	CAMERA_SetReg(0x05, 0x03);
	CAMERA_SetReg(0x06, 0x20);
	CAMERA_SetReg(0x07, 0x02);
	CAMERA_SetReg(0x08, 0x91);
	    
	CAMERA_SetReg(0x09, 0x03);
	CAMERA_SetReg(0x0A, 0x25);
	CAMERA_SetReg(0x0B, 0x02);
	CAMERA_SetReg(0x0C, 0x64);
	    
	CAMERA_SetReg(0x0D, 0x03);
	CAMERA_SetReg(0x0E, 0x0F);
	CAMERA_SetReg(0x0F, 0x02);
	CAMERA_SetReg(0x10, 0x4E);
	    
	CAMERA_SetReg(0x1D, 0x80);
	CAMERA_SetReg(0x1E, 0x00);
	CAMERA_SetReg(0x1F, 0x80);
	CAMERA_SetReg(0x20, 0x00);
	CAMERA_SetReg(0x23, 0x85);
	CAMERA_SetReg(0x24, 0x52);
	CAMERA_SetReg(0x21, 0x79);
	CAMERA_SetReg(0x22, 0xE6);
	    
	CAMERA_SetReg(0x25, 0x80);
	CAMERA_SetReg(0x26, 0x00);
	CAMERA_SetReg(0x27, 0x80);
	CAMERA_SetReg(0x28, 0x00);
	CAMERA_SetReg(0x2B, 0x81);
	CAMERA_SetReg(0x2C, 0x48);
	CAMERA_SetReg(0x29, 0x81);
	CAMERA_SetReg(0x2A, 0x48);
	    
	CAMERA_SetReg(0x2D, 0x80);
	CAMERA_SetReg(0x2E, 0x00);
	CAMERA_SetReg(0x2F, 0x80);
	CAMERA_SetReg(0x30, 0x00);
	CAMERA_SetReg(0x33, 0x7C);
	CAMERA_SetReg(0x34, 0x45);
	CAMERA_SetReg(0x31, 0x7D);
	CAMERA_SetReg(0x32, 0x7D);
	
	CAMERA_SetReg(0x35, 0x01);
	CAMERA_SetReg(0x36, 0x00);
	CAMERA_SetReg(0x37, 0x01);
	CAMERA_SetReg(0x38, 0x11);
	CAMERA_SetReg(0x39, 0x01);
	CAMERA_SetReg(0x3A, 0x4E);
	CAMERA_SetReg(0x3B, 0x01);
	CAMERA_SetReg(0x3C, 0xAB);
	CAMERA_SetReg(0x3D, 0x01);
	CAMERA_SetReg(0x3E, 0xDC);
	CAMERA_SetReg(0x3F, 0x02);
	CAMERA_SetReg(0x40, 0x1A);
	CAMERA_SetReg(0x41, 0x02);
	CAMERA_SetReg(0x42, 0x6A);
	CAMERA_SetReg(0x43, 0x02);
	CAMERA_SetReg(0x44, 0xD3);
	    
	CAMERA_SetReg(0x45, 0x01);
	CAMERA_SetReg(0x46, 0x00);
	CAMERA_SetReg(0x47, 0x01);
	CAMERA_SetReg(0x48, 0x0E);
	CAMERA_SetReg(0x49, 0x01);
	CAMERA_SetReg(0x4A, 0x40);
	CAMERA_SetReg(0x4B, 0x01);
	CAMERA_SetReg(0x4C, 0x8A);
	CAMERA_SetReg(0x4D, 0x01);
	CAMERA_SetReg(0x4E, 0xB5);
	CAMERA_SetReg(0x4F, 0x01);
	CAMERA_SetReg(0x50, 0xE8);
	CAMERA_SetReg(0x51, 0x02);
	CAMERA_SetReg(0x52, 0x27);
	CAMERA_SetReg(0x53, 0x02);
	CAMERA_SetReg(0x54, 0x84);
	
	CAMERA_SetReg(0x55, 0x01);
	CAMERA_SetReg(0x56, 0x00);
	CAMERA_SetReg(0x57, 0x01);
	CAMERA_SetReg(0x58, 0x0C);
	CAMERA_SetReg(0x59, 0x01);
	CAMERA_SetReg(0x5A, 0x37);
	CAMERA_SetReg(0x5B, 0x01);
	CAMERA_SetReg(0x5C, 0x74);
	CAMERA_SetReg(0x5D, 0x01);
	CAMERA_SetReg(0x5E, 0x96);
	CAMERA_SetReg(0x5F, 0x01);
	CAMERA_SetReg(0x60, 0xC9);
	CAMERA_SetReg(0x61, 0x02);
	CAMERA_SetReg(0x62, 0x04);
	CAMERA_SetReg(0x63, 0x02);
	CAMERA_SetReg(0x64, 0x4B);
	
	CAMERA_SetReg(0x65, 0x00);
	CAMERA_SetReg(0x66, 0x9A);
	CAMERA_SetReg(0x67, 0x2D);
	CAMERA_SetReg(0x68, 0x02);
	CAMERA_SetReg(0x69, 0x68);
	CAMERA_SetReg(0x6A, 0xB6);
	CAMERA_SetReg(0x6B, 0x05);
	CAMERA_SetReg(0x6C, 0x6B);
	CAMERA_SetReg(0x6D, 0x99);
	CAMERA_SetReg(0x6E, 0x07);
	CAMERA_SetReg(0x6F, 0x60);
	CAMERA_SetReg(0x70, 0xAD);
	CAMERA_SetReg(0x71, 0x09);
	CAMERA_SetReg(0x72, 0xA2);
	CAMERA_SetReg(0x73, 0xD7);
	CAMERA_SetReg(0x74, 0x0C);
	CAMERA_SetReg(0x75, 0x32);
	CAMERA_SetReg(0x76, 0x19);
	CAMERA_SetReg(0x77, 0x0F);
	CAMERA_SetReg(0x78, 0x0E);
	CAMERA_SetReg(0x79, 0x70);
	
	CAMERA_SetReg(0x7A, 0x00);
	CAMERA_SetReg(0x7B, 0x9C);
	CAMERA_SetReg(0x7C, 0x9F);
	CAMERA_SetReg(0x7D, 0x02);
	CAMERA_SetReg(0x7E, 0x72);
	CAMERA_SetReg(0x7F, 0x7A);
	CAMERA_SetReg(0x80, 0x05);
	CAMERA_SetReg(0x81, 0x81);
	CAMERA_SetReg(0x82, 0x94);
	CAMERA_SetReg(0x83, 0x07);
	CAMERA_SetReg(0x84, 0x7E);
	CAMERA_SetReg(0x85, 0x97);
	CAMERA_SetReg(0x86, 0x09);
	CAMERA_SetReg(0x87, 0xC9);
	CAMERA_SetReg(0x88, 0xEA);
	CAMERA_SetReg(0x89, 0x0C);
	CAMERA_SetReg(0x8A, 0x63);
	CAMERA_SetReg(0x8B, 0x8C);
	CAMERA_SetReg(0x8C, 0x0F);
	CAMERA_SetReg(0x8D, 0x4B);
	CAMERA_SetReg(0x8E, 0x7E);
	
	CAMERA_SetReg(0x8F, 0x00);
	CAMERA_SetReg(0x90, 0x9E);
	CAMERA_SetReg(0x91, 0xBD);
	CAMERA_SetReg(0x92, 0x02);
	CAMERA_SetReg(0x93, 0x7A);
	CAMERA_SetReg(0x94, 0xF5);
	CAMERA_SetReg(0x95, 0x05);
	CAMERA_SetReg(0x96, 0x94);
	CAMERA_SetReg(0x97, 0xA8);
	CAMERA_SetReg(0x98, 0x07);
	CAMERA_SetReg(0x99, 0x98);
	CAMERA_SetReg(0x9A, 0x8F);
	CAMERA_SetReg(0x9B, 0x09);
	CAMERA_SetReg(0x9C, 0xEB);
	CAMERA_SetReg(0x9D, 0xD5);
	CAMERA_SetReg(0x9E, 0x0C);
	CAMERA_SetReg(0x9F, 0x8E);
	CAMERA_SetReg(0xA0, 0x7A);
	CAMERA_SetReg(0xA1, 0x0F);
	CAMERA_SetReg(0xA2, 0x80);
	CAMERA_SetReg(0xA3, 0x7D);
	
	CAMERA_SetReg(0xA4, 0x6A);
	CAMERA_SetReg(0xA5, 0x44);
	CAMERA_SetReg(0xA6, 0x23);
	CAMERA_SetReg(0xA7, 0x6C);
	CAMERA_SetReg(0xA8, 0x15);
	CAMERA_SetReg(0xA9, 0x40);
	CAMERA_SetReg(0xAA, 0x20);
	CAMERA_SetReg(0xAB, 0xB2);
	CAMERA_SetReg(0xAC, 0x1C);
	CAMERA_SetReg(0xAD, 0x56);
	CAMERA_SetReg(0xAE, 0x19);
	CAMERA_SetReg(0xAF, 0x01);
	CAMERA_SetReg(0xB0, 0x16);
	CAMERA_SetReg(0xB1, 0x5F);
	
	CAMERA_SetReg(0xB2, 0x68);
	CAMERA_SetReg(0xB3, 0x9C);
	CAMERA_SetReg(0xB4, 0x22);
	CAMERA_SetReg(0xB5, 0xDE);
	CAMERA_SetReg(0xB6, 0x14);
	CAMERA_SetReg(0xB7, 0xEC);
	CAMERA_SetReg(0xB8, 0x20);
	CAMERA_SetReg(0xB9, 0x30);
	CAMERA_SetReg(0xBA, 0x1B);
	CAMERA_SetReg(0xBB, 0xE5);
	CAMERA_SetReg(0xBC, 0x18);
	CAMERA_SetReg(0xBD, 0x9D);
	CAMERA_SetReg(0xBE, 0x16);
	CAMERA_SetReg(0xBF, 0x05);
	
	CAMERA_SetReg(0xC0, 0x67);
	CAMERA_SetReg(0xC1, 0x36);
	CAMERA_SetReg(0xC2, 0x22);
	CAMERA_SetReg(0xC3, 0x67);
	CAMERA_SetReg(0xC4, 0x14);
	CAMERA_SetReg(0xC5, 0xA4);
	CAMERA_SetReg(0xC6, 0x1F);
	CAMERA_SetReg(0xC7, 0xC2);
	CAMERA_SetReg(0xC8, 0x1B);
	CAMERA_SetReg(0xC9, 0x86);
	CAMERA_SetReg(0xCA, 0x18);
	CAMERA_SetReg(0xCB, 0x49);
	CAMERA_SetReg(0xCC, 0x15);
	CAMERA_SetReg(0xCD, 0xBA);
	        
	CAMERA_SetReg(0x00, 0x02);// shading on

//========================================================== 
//	X-SHADING                                            
//==========================================================
	CAMERA_SetReg(0xfc, 0x1B);
	CAMERA_SetReg(0x80, 0x01);
	CAMERA_SetReg(0x81, 0x00);
	CAMERA_SetReg(0x82, 0x4C);
	CAMERA_SetReg(0x83, 0x00);
	CAMERA_SetReg(0x84, 0x86);
	CAMERA_SetReg(0x85, 0x03);
	CAMERA_SetReg(0x86, 0x5E);
	CAMERA_SetReg(0x87, 0x00);
	CAMERA_SetReg(0x88, 0x07);
	CAMERA_SetReg(0x89, 0xA4);
	CAMERA_SetReg(0x90, 0x00);
	CAMERA_SetReg(0x91, 0x88);
	CAMERA_SetReg(0x92, 0x00);
	CAMERA_SetReg(0x93, 0xC1);
	CAMERA_SetReg(0x94, 0x00);
	CAMERA_SetReg(0x95, 0xF7);
	CAMERA_SetReg(0x96, 0x01);
	CAMERA_SetReg(0x97, 0x21);
	CAMERA_SetReg(0x98, 0x01);
	CAMERA_SetReg(0x99, 0x37);
	CAMERA_SetReg(0x9A, 0x01);
	CAMERA_SetReg(0x9B, 0x0C);
	CAMERA_SetReg(0x9C, 0x00);
	CAMERA_SetReg(0x9D, 0xCE);
	CAMERA_SetReg(0x9E, 0x00);
	CAMERA_SetReg(0x9F, 0x3B);
	CAMERA_SetReg(0xA0, 0x00);
	CAMERA_SetReg(0xA1, 0x5B);
	CAMERA_SetReg(0xA2, 0x00);
	CAMERA_SetReg(0xA3, 0x7A);
	CAMERA_SetReg(0xA4, 0x00);
	CAMERA_SetReg(0xA5, 0x92);
	CAMERA_SetReg(0xA6, 0x00);
	CAMERA_SetReg(0xA7, 0x91);
	CAMERA_SetReg(0xA8, 0x00);
	CAMERA_SetReg(0xA9, 0x81);
	CAMERA_SetReg(0xAA, 0x00);
	CAMERA_SetReg(0xAB, 0x60);
	CAMERA_SetReg(0xAC, 0x07);
	CAMERA_SetReg(0xAD, 0xCB);
	CAMERA_SetReg(0xAE, 0x07);
	CAMERA_SetReg(0xAF, 0xC5);
	CAMERA_SetReg(0xB0, 0x07);
	CAMERA_SetReg(0xB1, 0xBB);
	CAMERA_SetReg(0xB2, 0x07);
	CAMERA_SetReg(0xB3, 0xAA);
	CAMERA_SetReg(0xB4, 0x07);
	CAMERA_SetReg(0xB5, 0xA9);
	CAMERA_SetReg(0xB6, 0x07);
	CAMERA_SetReg(0xB7, 0xB2);
	CAMERA_SetReg(0xB8, 0x07);
	CAMERA_SetReg(0xB9, 0xBF);
	CAMERA_SetReg(0xBA, 0x07);
	CAMERA_SetReg(0xBB, 0x5E);
	CAMERA_SetReg(0xBC, 0x07);
	CAMERA_SetReg(0xBD, 0x3C);
	CAMERA_SetReg(0xBE, 0x06);
	CAMERA_SetReg(0xBF, 0xF9);
	CAMERA_SetReg(0xC0, 0x06);
	CAMERA_SetReg(0xC1, 0xBD);
	CAMERA_SetReg(0xC2, 0x06);
	CAMERA_SetReg(0xC3, 0xB8);
	CAMERA_SetReg(0xC4, 0x06);
	CAMERA_SetReg(0xC5, 0xE2);
	CAMERA_SetReg(0xC6, 0x07);
	CAMERA_SetReg(0xC7, 0x1A);
	CAMERA_SetReg(0xC8, 0x07);
	CAMERA_SetReg(0xC9, 0x15);
	CAMERA_SetReg(0xCA, 0x06);
	CAMERA_SetReg(0xCB, 0xDE);
	CAMERA_SetReg(0xCC, 0x06);
	CAMERA_SetReg(0xCD, 0x9C);
	CAMERA_SetReg(0xCE, 0x06);
	CAMERA_SetReg(0xCF, 0x6F);
	CAMERA_SetReg(0xD0, 0x06);
	CAMERA_SetReg(0xD1, 0x5E);
	CAMERA_SetReg(0xD2, 0x06);
	CAMERA_SetReg(0xD3, 0x84);
	CAMERA_SetReg(0xD4, 0x06);
	CAMERA_SetReg(0xD5, 0xCA);
	    
	CAMERA_SetReg(0xfc, 0x0b);
	CAMERA_SetReg(0xda, 0x00);
	CAMERA_SetReg(0xdb, 0x9c);
	CAMERA_SetReg(0xdc, 0x00);
	CAMERA_SetReg(0xdd, 0xd1);
	    
	CAMERA_SetReg(0xfc, 0x1b);
	CAMERA_SetReg(0x80, 0x01);

//========================================================== 
//	AE WINDOW WEIGHT                                     
//==========================================================
	CAMERA_SetReg(0xfc, 0x00);
	CAMERA_SetReg(0x03, 0x4b);
	CAMERA_SetReg(0xfc, 0x06);
	CAMERA_SetReg(0x01, 0x35);
	CAMERA_SetReg(0x03, 0xc2);
	CAMERA_SetReg(0x05, 0x48);
	CAMERA_SetReg(0x07, 0xb8);
	CAMERA_SetReg(0x31, 0x2a);
	CAMERA_SetReg(0x33, 0x61);
	CAMERA_SetReg(0x35, 0x28);
	CAMERA_SetReg(0x37, 0x5c);
	    
	CAMERA_SetReg(0xfc, 0x20);
	CAMERA_SetReg(0x60, 0x11);
	CAMERA_SetReg(0x61, 0x11);
	CAMERA_SetReg(0x62, 0x11);
	CAMERA_SetReg(0x63, 0x11);
	CAMERA_SetReg(0x64, 0x11);
	CAMERA_SetReg(0x65, 0x22);
	CAMERA_SetReg(0x66, 0x22);
	CAMERA_SetReg(0x67, 0x11);
	CAMERA_SetReg(0x68, 0x11);
	CAMERA_SetReg(0x69, 0x33);
	CAMERA_SetReg(0x6a, 0x33);
	CAMERA_SetReg(0x6b, 0x11);
	CAMERA_SetReg(0x6c, 0x12);
	CAMERA_SetReg(0x6d, 0x55);
	CAMERA_SetReg(0x6e, 0x55);
	CAMERA_SetReg(0x6f, 0x21);
	CAMERA_SetReg(0x70, 0x13);
	CAMERA_SetReg(0x71, 0x55);
	CAMERA_SetReg(0x72, 0x55);
	CAMERA_SetReg(0x73, 0x31);
	CAMERA_SetReg(0x74, 0x33);
	CAMERA_SetReg(0x75, 0x33);
	CAMERA_SetReg(0x76, 0x33);
	CAMERA_SetReg(0x77, 0x33);

//==========================================================  
//	SAIT AWB                                              
//==========================================================
	CAMERA_SetReg(0xfc, 0x00);
	CAMERA_SetReg(0x7b, 0x00);
	    
	CAMERA_SetReg(0xfc, 0x07);
	CAMERA_SetReg(0x3c, 0x10);
	CAMERA_SetReg(0x3d, 0x10);
	CAMERA_SetReg(0x3e, 0x10);
	CAMERA_SetReg(0x3f, 0x10);
	    
	CAMERA_SetReg(0xfc, 0x01);
	CAMERA_SetReg(0xc8, 0xe0);
	CAMERA_SetReg(0xfc, 0x00);
	CAMERA_SetReg(0x3e, 0x10);
	    
	CAMERA_SetReg(0xfc, 0x00);
	CAMERA_SetReg(0x3e, 0x10);
	CAMERA_SetReg(0x3d, 0x04);
	CAMERA_SetReg(0x32, 0x02);
	CAMERA_SetReg(0x81, 0x10);
	CAMERA_SetReg(0xbc, 0xf0);
	CAMERA_SetReg(0xfc, 0x22);
	CAMERA_SetReg(0x8c, 0x04);
	CAMERA_SetReg(0x8d, 0x06);
	    
	CAMERA_SetReg(0xfc, 0x07);
	CAMERA_SetReg(0x97, 0x00);

//=================================                           
// White Point                                                
//=================================
	CAMERA_SetReg(0xfc, 0x22);
	CAMERA_SetReg(0x01, 0xD8);
	CAMERA_SetReg(0x03, 0xA1);
	CAMERA_SetReg(0x05, 0xCA);
	CAMERA_SetReg(0x07, 0xC8);
	CAMERA_SetReg(0x09, 0xB3);
	CAMERA_SetReg(0x0b, 0xE2);
	CAMERA_SetReg(0x0d, 0xA0);
	CAMERA_SetReg(0x0f, 0xF0);
	CAMERA_SetReg(0x11, 0x94);
	CAMERA_SetReg(0x12, 0x00);
	CAMERA_SetReg(0x13, 0xFD);
	CAMERA_SetReg(0x15, 0x88);
	CAMERA_SetReg(0x16, 0x01);
	CAMERA_SetReg(0x17, 0x10);

//=================================                                  
// Basic Setting                            
//=================================
	CAMERA_SetReg(0xfc, 0x22);
	CAMERA_SetReg(0xA8, 0xFF);
	    
	CAMERA_SetReg(0xA0, 0x01);
	CAMERA_SetReg(0xA1, 0x38);
	CAMERA_SetReg(0xA2, 0x0E);
	CAMERA_SetReg(0xA3, 0x6D);
	CAMERA_SetReg(0xA4, 0x07);
	CAMERA_SetReg(0xA5, 0xF5);
	CAMERA_SetReg(0xA6, 0x11);
	CAMERA_SetReg(0xA7, 0xBE);
	CAMERA_SetReg(0xA9, 0x02);
	CAMERA_SetReg(0xAA, 0xD2);
	CAMERA_SetReg(0xAB, 0x00);
	CAMERA_SetReg(0xAC, 0x00);
	CAMERA_SetReg(0xAD, 0x02);
	CAMERA_SetReg(0xAE, 0x3F);
	CAMERA_SetReg(0xAF, 0x19);
	CAMERA_SetReg(0xB0, 0x91);
	CAMERA_SetReg(0x94, 0x3D);
	CAMERA_SetReg(0x95, 0x00);
	CAMERA_SetReg(0x96, 0x58);
	CAMERA_SetReg(0x97, 0x80);
	CAMERA_SetReg(0xD0, 0xA2);
	CAMERA_SetReg(0xD1, 0x2E);
	CAMERA_SetReg(0xD2, 0x4D);
	CAMERA_SetReg(0xD3, 0x28);
	CAMERA_SetReg(0xD4, 0x90);
	CAMERA_SetReg(0xDB, 0x2E);
	CAMERA_SetReg(0xDC, 0x7A);
	CAMERA_SetReg(0xDD, 0x28);
	CAMERA_SetReg(0xE7, 0x00);
	CAMERA_SetReg(0xE8, 0xc7);
	CAMERA_SetReg(0xE9, 0x00);
	CAMERA_SetReg(0xEA, 0x62);
	CAMERA_SetReg(0xEB, 0xD2);
	CAMERA_SetReg(0xEC, 0xD9);
	CAMERA_SetReg(0xEE, 0xA6);
	    
	CAMERA_SetReg(0xfc, 0x00);
	CAMERA_SetReg(0x8a, 0x02);
    
//=================================
// Pixel Filter Setting            
//=================================
	CAMERA_SetReg(0xFC, 0x07);
	CAMERA_SetReg(0x95, 0xCF);
	    
	CAMERA_SetReg(0xfc, 0x01);
	CAMERA_SetReg(0xd3, 0x4f);
	CAMERA_SetReg(0xd4, 0x00);
	CAMERA_SetReg(0xd5, 0x3c);
	CAMERA_SetReg(0xd6, 0x80);
	CAMERA_SetReg(0xd7, 0x61);
	CAMERA_SetReg(0xd8, 0x00);
	CAMERA_SetReg(0xd9, 0x49);
	CAMERA_SetReg(0xda, 0x00);
	CAMERA_SetReg(0xdb, 0x24);
	CAMERA_SetReg(0xdc, 0x4b);
	CAMERA_SetReg(0xdd, 0x23);
	CAMERA_SetReg(0xde, 0xf2);
	CAMERA_SetReg(0xdf, 0x20);
	CAMERA_SetReg(0xe0, 0x73);
	CAMERA_SetReg(0xe1, 0x18);
	CAMERA_SetReg(0xe2, 0x69);
	CAMERA_SetReg(0xe3, 0x31);
	CAMERA_SetReg(0xe4, 0x40);
	CAMERA_SetReg(0xe5, 0x34);
	CAMERA_SetReg(0xe6, 0x40);
	CAMERA_SetReg(0xe7, 0x40);
	CAMERA_SetReg(0xe8, 0x32);
	CAMERA_SetReg(0xe9, 0x40);
	CAMERA_SetReg(0xea, 0x1c);
	CAMERA_SetReg(0xeb, 0x00);

//================================= 
// Polygon AWB Region Tune          
//================================= 

	// AWB3 - Polygon Region
	CAMERA_SetReg(0xfc, 0x22);
	CAMERA_SetReg(0x18, 0x00);
	CAMERA_SetReg(0x19, 0x4b);
	CAMERA_SetReg(0x1a, 0xfd);
	CAMERA_SetReg(0x1b, 0x00);
	CAMERA_SetReg(0x1c, 0x41);
	CAMERA_SetReg(0x1d, 0xd9);
	CAMERA_SetReg(0x1e, 0x00);
	CAMERA_SetReg(0x1f, 0x66);
	CAMERA_SetReg(0x20, 0xa9);
	CAMERA_SetReg(0x21, 0x00);
	CAMERA_SetReg(0x22, 0x8b);
	CAMERA_SetReg(0x23, 0x82);
	CAMERA_SetReg(0x24, 0x00);
	CAMERA_SetReg(0x25, 0xa4);
	CAMERA_SetReg(0x26, 0x6c);
	CAMERA_SetReg(0x27, 0x00);
	CAMERA_SetReg(0x28, 0xbd);
	CAMERA_SetReg(0x29, 0x5d);
	CAMERA_SetReg(0x2a, 0x00);
	CAMERA_SetReg(0x2b, 0xdc);
	CAMERA_SetReg(0x2c, 0x4d);
	CAMERA_SetReg(0x2d, 0x00);
	CAMERA_SetReg(0x2e, 0xdc);
	CAMERA_SetReg(0x2f, 0x63);
	CAMERA_SetReg(0x30, 0x00);
	CAMERA_SetReg(0x31, 0xc1);
	CAMERA_SetReg(0x32, 0x72);
	CAMERA_SetReg(0x33, 0x00);
	CAMERA_SetReg(0x34, 0xab);
	CAMERA_SetReg(0x35, 0x84);
	CAMERA_SetReg(0x36, 0x00);
	CAMERA_SetReg(0x37, 0x99);
	CAMERA_SetReg(0x38, 0xa0);
	CAMERA_SetReg(0x39, 0x00);
	CAMERA_SetReg(0x3a, 0x81);
	CAMERA_SetReg(0x3b, 0xe9);
	CAMERA_SetReg(0x3c, 0x00);
	CAMERA_SetReg(0x3d, 0x00);
	CAMERA_SetReg(0x3e, 0x00);
	CAMERA_SetReg(0x3f, 0x00);
	CAMERA_SetReg(0x40, 0x00);
	CAMERA_SetReg(0x41, 0x00);

//================================= 
// Moving Equation Weight           
//=================================
	CAMERA_SetReg(0xfc, 0x22);
	CAMERA_SetReg(0x98, 0x07);

//=================================
// EIT Threshold                   
//=================================
	CAMERA_SetReg(0xfc, 0x22);
	CAMERA_SetReg(0xb1, 0x00);
	CAMERA_SetReg(0xb2, 0x02);
	CAMERA_SetReg(0xb3, 0x00);
	CAMERA_SetReg(0xb4, 0xC1);
	    
	CAMERA_SetReg(0xb5, 0x00);
	CAMERA_SetReg(0xb6, 0x02);
	CAMERA_SetReg(0xb7, 0x00);
	CAMERA_SetReg(0xb9, 0xc2);
	    
	CAMERA_SetReg(0xd7, 0x00);
	CAMERA_SetReg(0xd8, 0x35);
	CAMERA_SetReg(0xd9, 0x20);
	CAMERA_SetReg(0xda, 0x81);

//=================================
// Gain Offset                     
//=================================
	CAMERA_SetReg(0xfc, 0x00);
	CAMERA_SetReg(0x79, 0xf8);
	CAMERA_SetReg(0x7a, 0x08);
	    
	CAMERA_SetReg(0xfc, 0x07);
	CAMERA_SetReg(0x11, 0x01);
	    
	CAMERA_SetReg(0xfc, 0x22);
	CAMERA_SetReg(0x58, 0xf8);
	CAMERA_SetReg(0x59, 0x00);
	CAMERA_SetReg(0x5A, 0xfc);
	CAMERA_SetReg(0x5B, 0x00);
	CAMERA_SetReg(0x5C, 0x00);
	CAMERA_SetReg(0x5D, 0x00);
	CAMERA_SetReg(0x5E, 0x00);
	CAMERA_SetReg(0x5F, 0x00);
	CAMERA_SetReg(0x60, 0x00);
	CAMERA_SetReg(0x61, 0xf8);
	CAMERA_SetReg(0x62, 0x00);
	CAMERA_SetReg(0x63, 0xf0);
	    
	CAMERA_SetReg(0xde, 0x00);
	CAMERA_SetReg(0xf0, 0x6a);

//=================================
// Green Stablity Enhance          
//=================================
	CAMERA_SetReg(0xfc, 0x22);
	CAMERA_SetReg(0xb9, 0x00);
	CAMERA_SetReg(0xba, 0x00);
	CAMERA_SetReg(0xbb, 0x00);
	CAMERA_SetReg(0xbc, 0x00);
	CAMERA_SetReg(0xe5, 0x01);
	CAMERA_SetReg(0xe6, 0xff);
	CAMERA_SetReg(0xbd, 0x8c);

//========================================================== 
//	Special Effect                                       
//==========================================================
	CAMERA_SetReg(0xfc, 0x07);
	CAMERA_SetReg(0x30, 0xc0);
	CAMERA_SetReg(0x31, 0x20);
	CAMERA_SetReg(0x32, 0x40);
	CAMERA_SetReg(0x33, 0xc0);
	CAMERA_SetReg(0x34, 0x00);
	CAMERA_SetReg(0x35, 0xb0);
/*	
*/
//==========================================================
//	ETC                                      
//==========================================================
	CAMERA_SetReg(0xfc, 0x01);
	CAMERA_SetReg(0x01, 0x01);
	CAMERA_SetReg(0x00, 0x90);
	CAMERA_SetReg(0xfc, 0x02);
	CAMERA_SetReg(0x03, 0x20);
	    
	CAMERA_SetReg(0xfc, 0x20);
	CAMERA_SetReg(0x0f, 0x00);
	    
	CAMERA_SetReg(0xfc, 0x00);
	CAMERA_SetReg(0x02, 0x09);

	    
	CAMERA_SetReg(0xfc, 0x01);
	//CAMERA_SetReg(0x02, 0x00);
	CAMERA_SetReg(0x02, 0x07);//Donghoon

	CAMERA_SetSizeInS5K4BAF(eSub);

	CAMERA_IICClose();
}



void CAMERA_InitS5K4BA(CAM_ATTR eCcir, CSPACE eSrcFmt, SUB_SAMPLING eSub)
{
	u32 uHCLKx2;
	u32 IMAGE = false;

	SYSC_GetClkInform(); // Get HCLKx2 Frequency
	uHCLKx2 = g_HCLKx2/1000000;
	
	oCamera.m_ucSlaveAddr = 0x5a;



	CAMERA_IICOpen(100000); 
#if 1 // add jungil from Image Team


	CAMERA_SetReg(0xfc,0x07);
	CAMERA_SetReg(0x66,0x01);// WDT
	CAMERA_SetReg(0xfc,0x00);
	CAMERA_SetReg(0x00,0xAA);// For EDS Check
	CAMERA_SetReg(0x21,0x03);// peter0223 added
	
	CAMERA_SetReg(0xfc,0x01);
	CAMERA_SetReg(0x04,0x01);// ARM Clock Divider  
	
	CAMERA_SetReg(0xfc,0x02);// Analog setting                               
	CAMERA_SetReg(0x55,0x1E);// LineADLC on(s551a) off(s550a)
	CAMERA_SetReg(0x56,0x10);// BPR 16code
	CAMERA_SetReg(0x30,0x82);// Analog offset (capture =?h)
	CAMERA_SetReg(0x37,0x17);// Global Gain (default:31)
	
	CAMERA_SetReg(0x57,0x80);// LineADLC Roffset
	CAMERA_SetReg(0x58,0x80);//89	//90 // LineADLC Goffset
	CAMERA_SetReg(0x59,0x80);//90 // LineADLC offset don't care
	
	CAMERA_SetReg(0x44,0x64);// clamp en[6]=1 on
	#if IMAGE
	CAMERA_SetReg(0x4a,0x40);// clamp level 0011h [7]~[4]
	#else
	CAMERA_SetReg(0x4a,0x30);// clamp level 0011h [7]~[4]
	#endif
	
	CAMERA_SetReg(0xfc,0x02);           
	CAMERA_SetReg(0x2d,0x48);// double shutter (default:00)
	CAMERA_SetReg(0x4D,0x08);// Voltage doubler (default:04)
	#if IMAGE
	CAMERA_SetReg(0x4e,0x1a);// IO current 8mA set
	CAMERA_SetReg(0x4f,0xfa);// IO current 88mA set               
	#else
	CAMERA_SetReg(0x4e,0x00);// IO current 8mA set
	CAMERA_SetReg(0x4f,0x8a);// IO current 88mA set               
	#endif
			
//	CAMERA_SetReg(0x66,0x41);// 1st comp current 2uA
	CAMERA_SetReg(0x43,0xEF);// ec_comp
//	CAMERA_SetReg(0x62,0x60);// LD control  CFPN_EN off
			
	CAMERA_SetReg(0xfc,0x20);// onejump delay for AE haunting 0817
	CAMERA_SetReg(0x09,0x03);// onejump delay for AE haunting 0817     
			
	CAMERA_SetReg(0xfc,0x03);
	#if IMAGE
	CAMERA_SetReg(0x2e,0x00);
	#else 
	CAMERA_SetReg(0x01,0x60);
	CAMERA_SetReg(0x2e,0x03);
	#endif
	CAMERA_SetReg(0x05,0x46);// Output Image Size Set for Capture
	CAMERA_SetReg(0x07,0xb6);
	CAMERA_SetReg(0x0e,0x04);
	CAMERA_SetReg(0x12,0x03);
	
	#if IMAGE
	CAMERA_SetReg(0x02,0x04);
	CAMERA_SetReg(0x03,0xfd);
	#endif
	
	CAMERA_SetReg(0xfc,0x04);
	CAMERA_SetReg(0xc5,0x26);// Output Image Size Set for Preview
	CAMERA_SetReg(0xc7,0x5e);
	CAMERA_SetReg(0xce,0x04);
	CAMERA_SetReg(0xd2,0x04);
	#if IMAGE
	CAMERA_SetReg(0xee,0x00);
	CAMERA_SetReg(0xc0,0x05);
	CAMERA_SetReg(0xc1,0x40);
	CAMERA_SetReg(0xc2,0x03);
	CAMERA_SetReg(0xc3,0x16);
	#else
	CAMERA_SetReg(0xee,0x01);
	CAMERA_SetReg(0xc0,0x06);
	CAMERA_SetReg(0xc1,0x60);
	CAMERA_SetReg(0xc2,0x02);
	CAMERA_SetReg(0xc3,0x8d);
	#endif
			
	CAMERA_SetReg(0xfc,0x07);
	CAMERA_SetReg(0x05,0x00);
	CAMERA_SetReg(0x06,0x00);
	CAMERA_SetReg(0x07,0x8b);
	CAMERA_SetReg(0x08,0xf5);
	CAMERA_SetReg(0x09,0x00);
	CAMERA_SetReg(0x0a,0xb4);
	CAMERA_SetReg(0x0b,0x00);
	CAMERA_SetReg(0x0c,0xea);
	CAMERA_SetReg(0x0d,0x00);
	CAMERA_SetReg(0x0e,0x40);
	
/*
	CAMERA_SetReg(0xfc,0x00);               
	CAMERA_SetReg(0x70,0x02);
	
	CAMERA_SetReg(0xfc,0x00);
	CAMERA_SetReg(0x73,0x21);//21 Frmae AE Enable peter0223 
	CAMERA_SetReg(0x20,0x02);// Change AWB Mode
	
	CAMERA_SetReg(0xfc,0x00);
	CAMERA_SetReg(0x78,0x60);// AGC Max
	
	CAMERA_SetReg(0xfc,0x00);
	CAMERA_SetReg(0x6c,0xa0);// AE target 
	CAMERA_SetReg(0x6d,0x00);
	
	CAMERA_SetReg(0xfc,0x20);
	CAMERA_SetReg(0x16,0x5a);// AGC frame AE start _for Prevating AE Hunting
	CAMERA_SetReg(0x57,0x18);// Stable_Frame_AE     
	
	CAMERA_SetReg(0xfc,0x00);
	CAMERA_SetReg(0x83,0x06);// low condition shutter off // Double shutter off
	
	CAMERA_SetReg(0xfc,0x0b);
	CAMERA_SetReg(0x5c,0x64);//70	//AGC value to start shutter on/off suppress
	CAMERA_SetReg(0x5d,0x60);//60   //AGC value to start double shutter on/off suppress       
	
	CAMERA_SetReg(0xfc,0x20);
	CAMERA_SetReg(0x25,0x00);// CINTR Min
	CAMERA_SetReg(0x2a,0x01);// forbidden
	CAMERA_SetReg(0x2b,0x02);// For Forbidden Area
	CAMERA_SetReg(0x2c,0x0a);
	CAMERA_SetReg(0x2d,0x00);// For Forbidden Area
	CAMERA_SetReg(0x2e,0x00);
	CAMERA_SetReg(0x2f,0x05);// forbidden
	CAMERA_SetReg(0x14,0x78);//70  
	CAMERA_SetReg(0x01,0x00);// Stepless_Off      
	
	CAMERA_SetReg(0xfc,0x00);
	CAMERA_SetReg(0x29,0x04);// Y level  
	CAMERA_SetReg(0x2a,0x00);
	CAMERA_SetReg(0x2b,0x04);// C level
	CAMERA_SetReg(0x2c,0x00);//60
	
	CAMERA_SetReg(0xfc,0x07);
	CAMERA_SetReg(0x37,0x00);// Flicker     	
	
	CAMERA_SetReg(0xfc,0x00);
	CAMERA_SetReg(0x72,0xb4);// Flicker for 36MHz
	CAMERA_SetReg(0x74,0x04);// flicker 50Hz fix       
	CAMERA_SetReg(0xfc,0x20);
	CAMERA_SetReg(0x01,0x00);// stepless_off 
	CAMERA_SetReg(0x02,0x02);//02 Flicker Dgain Mode     
	CAMERA_SetReg(0xfc,0x00);
	CAMERA_SetReg(0x62,0x02);// Hue Control Enable	      
	
	CAMERA_SetReg(0xfc,0x01);
	CAMERA_SetReg(0x0c,0x03);// Full YC Enable
	

	CAMERA_SetReg(0xfc,0x01);//01	//DL gain 60
	CAMERA_SetReg(0x51,0x07);//08	//06	//08  07	
	CAMERA_SetReg(0x52,0x40);//e8	//df	//9B  E7	
	CAMERA_SetReg(0x53,0xfc);//fc	//fd	//FC  FB	
	CAMERA_SetReg(0x54,0x80);//33	//09	//07  B9	
	CAMERA_SetReg(0x55,0x00);//fe	//00	//FF  00	
	CAMERA_SetReg(0x56,0x3f);//e6	//17	//5E  5F	
	CAMERA_SetReg(0x57,0xfe);//fe	//fe	//FD  FD	
	CAMERA_SetReg(0x58,0x7f);//3d	//4f	//0E  46	
	CAMERA_SetReg(0x59,0x06);//08	//06	//07  05	
	CAMERA_SetReg(0x5A,0x6a);//21	//9b	//EE  E6	
	CAMERA_SetReg(0x5B,0xff);//fd	//ff	//FF  00	
	CAMERA_SetReg(0x5C,0x18);//a3	//17	//05  D3	
	CAMERA_SetReg(0x5D,0xff);//ff	//ff	//FF  FF	
	CAMERA_SetReg(0x5E,0xd7);//bc	//81	//7A  53	
	CAMERA_SetReg(0x5F,0xfd);//fc	//fd	//FC  FB	
	CAMERA_SetReg(0x60,0x1c);//96	//5b	//23  B1	
	CAMERA_SetReg(0x61,0x07);//07	//07	//08  08	
	CAMERA_SetReg(0x62,0x0d);//af	//24	//64  FD	  
	
	CAMERA_SetReg(0xfc,0x05);
	CAMERA_SetReg(0x12,0x3d);
	CAMERA_SetReg(0x13,0x3b);
	CAMERA_SetReg(0x14,0x38);
	CAMERA_SetReg(0x15,0x3b);
	CAMERA_SetReg(0x16,0x3d);
	
	CAMERA_SetReg(0x17,0x3b);
	CAMERA_SetReg(0x18,0x05);
	CAMERA_SetReg(0x19,0x09);
	CAMERA_SetReg(0x1A,0x05);
	CAMERA_SetReg(0x1B,0x3b);
	
	CAMERA_SetReg(0x1C,0x38);
	CAMERA_SetReg(0x1D,0x09);
	CAMERA_SetReg(0x1E,0x1c);
	CAMERA_SetReg(0x1F,0x09);
	CAMERA_SetReg(0x20,0x38);
	
	CAMERA_SetReg(0x21,0x3b);
	CAMERA_SetReg(0x22,0x05);
	CAMERA_SetReg(0x23,0x09);
	CAMERA_SetReg(0x24,0x05);
	CAMERA_SetReg(0x25,0x3b);
	
	CAMERA_SetReg(0x26,0x3d);
	CAMERA_SetReg(0x27,0x3b);
	CAMERA_SetReg(0x28,0x38);
	CAMERA_SetReg(0x29,0x3b);
	CAMERA_SetReg(0x2A,0x3d);
	
	CAMERA_SetReg(0xfc,0x00);
	CAMERA_SetReg(0x89,0x00);// Edge Suppress On
	CAMERA_SetReg(0xfc,0x0b);
	CAMERA_SetReg(0x42,0x20);// Edge AGC MIN
	CAMERA_SetReg(0x43,0x50);// Edge AGC MAX
	CAMERA_SetReg(0x45,0x1e);// positive gain AGC MIN    
	CAMERA_SetReg(0x49,0x06);// positive gain AGC MAX 
	CAMERA_SetReg(0x4d,0x1e);// negative gain AGC MIN
	CAMERA_SetReg(0x51,0x06);// negative gain AGC MAX
	
	CAMERA_SetReg(0xfc,0x05);
	CAMERA_SetReg(0x34,0x20);// APTCLP  
	CAMERA_SetReg(0x35,0x04);// APTSC  
	CAMERA_SetReg(0x36,0x0b);// ENHANCE 
	CAMERA_SetReg(0x3f,0x00);// NON-LIN   
	CAMERA_SetReg(0x42,0x10);// EGFALL
	CAMERA_SetReg(0x43,0x00);// HLFALL
	CAMERA_SetReg(0x45,0xa0);// EGREF
	CAMERA_SetReg(0x46,0x7a);// HLREF
	CAMERA_SetReg(0x47,0x40);// LLREF
	CAMERA_SetReg(0x48,0x0c);
	CAMERA_SetReg(0x49,0x31);// CSSEL  EGSEL  CS_DLY 
	
	CAMERA_SetReg(0x40,0x41);// Y delay
	
	CAMERA_SetReg(0xfc,0x1d);
	CAMERA_SetReg(0x86,0x00);// EE_WL_LUMA_MIN_H
	CAMERA_SetReg(0x87,0x60);// EE_WL_LUMA_MIN_L
	CAMERA_SetReg(0x88,0x01);// EE_WL_LUMA_MAX_H
	CAMERA_SetReg(0x89,0x20);// EE_WL_LUMA_MAX_L
	
	CAMERA_SetReg(0xfc,0x1e);
	CAMERA_SetReg(0x00,0x00);// P_MIN
	CAMERA_SetReg(0x01,0x00);
	CAMERA_SetReg(0x02,0x00);
	CAMERA_SetReg(0x03,0x00);
	CAMERA_SetReg(0x04,0x00);
	CAMERA_SetReg(0x05,0x1e);
	CAMERA_SetReg(0x06,0x00);
	CAMERA_SetReg(0x07,0x00);
	
	CAMERA_SetReg(0x08,0x00);// P_MAX
	CAMERA_SetReg(0x09,0x00);
	CAMERA_SetReg(0x0a,0x00);
	CAMERA_SetReg(0x0b,0x00);
	CAMERA_SetReg(0x0c,0x00);
	CAMERA_SetReg(0x0d,0x08);
	CAMERA_SetReg(0x0e,0x00);
	CAMERA_SetReg(0x0f,0x00);
	
	CAMERA_SetReg(0x10,0x00);// N_MIN
	CAMERA_SetReg(0x11,0x0A);
	CAMERA_SetReg(0x12,0x00);
	CAMERA_SetReg(0x13,0x00);
	CAMERA_SetReg(0x14,0x00);
	CAMERA_SetReg(0x15,0x1e);
	CAMERA_SetReg(0x16,0x00);
	CAMERA_SetReg(0x17,0x00);
	
	CAMERA_SetReg(0x18,0x00);// N_MAX
	CAMERA_SetReg(0x19,0x00);
	CAMERA_SetReg(0x1a,0x00);
	CAMERA_SetReg(0x1b,0x00);
	CAMERA_SetReg(0x1c,0x00);
	CAMERA_SetReg(0x1d,0x08);
	CAMERA_SetReg(0x1e,0x00);
	CAMERA_SetReg(0x1f,0x00);
	
	CAMERA_SetReg(0xfc,0x00);// Wide luma On
	CAMERA_SetReg(0x89,0xc3);
	
	CAMERA_SetReg(0xfc,0x1e);
	CAMERA_SetReg(0x85,0x01);// Wide luma On
	
	CAMERA_SetReg(0xfc,0x1d);
	CAMERA_SetReg(0x00,0x09);
	CAMERA_SetReg(0x01,0x18);
	CAMERA_SetReg(0x02,0x48);
	CAMERA_SetReg(0x03,0xc0);
	CAMERA_SetReg(0x04,0x00);
	CAMERA_SetReg(0x05,0x27);
	CAMERA_SetReg(0x06,0x82);
	CAMERA_SetReg(0x07,0xcd);
	CAMERA_SetReg(0x08,0x0a);
	CAMERA_SetReg(0x09,0x56);
	CAMERA_SetReg(0x0a,0x39);
	CAMERA_SetReg(0x0b,0x60);
	CAMERA_SetReg(0x0c,0x85);
	CAMERA_SetReg(0x0d,0xa7);
	CAMERA_SetReg(0x0e,0xaa);
	CAMERA_SetReg(0x0f,0xc5);
	CAMERA_SetReg(0x10,0xe2);
	CAMERA_SetReg(0x11,0xfb);
	CAMERA_SetReg(0x12,0x13);
	CAMERA_SetReg(0x13,0xab);
	CAMERA_SetReg(0x14,0x28);
	CAMERA_SetReg(0x15,0x3c);
	CAMERA_SetReg(0x16,0x4b);
	CAMERA_SetReg(0x17,0x5a);
	CAMERA_SetReg(0x18,0xff);
	CAMERA_SetReg(0x19,0x69);
	CAMERA_SetReg(0x1a,0x78);
	CAMERA_SetReg(0x1b,0x85);
	CAMERA_SetReg(0x1c,0x91);
	CAMERA_SetReg(0x1d,0xff);
	CAMERA_SetReg(0x1e,0x9c);
	CAMERA_SetReg(0x1f,0xa7);
	CAMERA_SetReg(0x20,0xb2);
	CAMERA_SetReg(0x21,0xbd);
	CAMERA_SetReg(0x22,0xff);
	CAMERA_SetReg(0x23,0xc7);
	CAMERA_SetReg(0x24,0xd2);
	CAMERA_SetReg(0x25,0xdb);
	CAMERA_SetReg(0x26,0xe4);
	CAMERA_SetReg(0x27,0xff);
	CAMERA_SetReg(0x28,0xec);
	CAMERA_SetReg(0x29,0xf5);
	CAMERA_SetReg(0x2a,0xf0);
	CAMERA_SetReg(0x2b,0x09);
	CAMERA_SetReg(0x2c,0x18);
	CAMERA_SetReg(0x2d,0x48);
	CAMERA_SetReg(0x2e,0xc0);
	CAMERA_SetReg(0x2f,0x00);
	CAMERA_SetReg(0x30,0x27);
	CAMERA_SetReg(0x31,0x82);
	CAMERA_SetReg(0x32,0xcd);
	CAMERA_SetReg(0x33,0x0a);
	CAMERA_SetReg(0x34,0x56);
	CAMERA_SetReg(0x35,0x39);
	CAMERA_SetReg(0x36,0x60);
	CAMERA_SetReg(0x37,0x85);
	CAMERA_SetReg(0x38,0xa7);
	CAMERA_SetReg(0x39,0xaa);
	CAMERA_SetReg(0x3a,0xc5);
	CAMERA_SetReg(0x3b,0xe2);
	CAMERA_SetReg(0x3c,0xfb);
	CAMERA_SetReg(0x3d,0x13);
	CAMERA_SetReg(0x3e,0xab);
	CAMERA_SetReg(0x3f,0x28);
	CAMERA_SetReg(0x40,0x3c);
	CAMERA_SetReg(0x41,0x4b);
	CAMERA_SetReg(0x42,0x5a);
	CAMERA_SetReg(0x43,0xff);
	CAMERA_SetReg(0x44,0x69);
	CAMERA_SetReg(0x45,0x78);
	CAMERA_SetReg(0x46,0x85);
	CAMERA_SetReg(0x47,0x91);
	CAMERA_SetReg(0x48,0xff);
	CAMERA_SetReg(0x49,0x9c);
	CAMERA_SetReg(0x4a,0xa7);
	CAMERA_SetReg(0x4b,0xb2);
	CAMERA_SetReg(0x4c,0xbd);
	CAMERA_SetReg(0x4d,0xff);
	CAMERA_SetReg(0x4e,0xc7);
	CAMERA_SetReg(0x4f,0xd2);
	CAMERA_SetReg(0x50,0xdb);
	CAMERA_SetReg(0x51,0xe4);
	CAMERA_SetReg(0x52,0xff);
	CAMERA_SetReg(0x53,0xec);
	CAMERA_SetReg(0x54,0xf5);
	CAMERA_SetReg(0x55,0xf0);
	CAMERA_SetReg(0x56,0x03);
	CAMERA_SetReg(0x57,0x0b);
	CAMERA_SetReg(0x58,0x27);
	CAMERA_SetReg(0x59,0x95);
	CAMERA_SetReg(0x5a,0x00);
	CAMERA_SetReg(0x5b,0x13);
	CAMERA_SetReg(0x5c,0x76);
	CAMERA_SetReg(0x5d,0xcd);
	CAMERA_SetReg(0x5e,0x0a);
	CAMERA_SetReg(0x5f,0x56);
	CAMERA_SetReg(0x60,0x39);
	CAMERA_SetReg(0x61,0x60);
	CAMERA_SetReg(0x62,0x85);
	CAMERA_SetReg(0x63,0xa7);
	CAMERA_SetReg(0x64,0xaa);
	CAMERA_SetReg(0x65,0xc5);
	CAMERA_SetReg(0x66,0xe2);
	CAMERA_SetReg(0x67,0xfb);
	CAMERA_SetReg(0x68,0x13);
	CAMERA_SetReg(0x69,0xab);
	CAMERA_SetReg(0x6a,0x28);
	CAMERA_SetReg(0x6b,0x3c);
	CAMERA_SetReg(0x6c,0x4b);
	CAMERA_SetReg(0x6d,0x5a);
	CAMERA_SetReg(0x6e,0xff);
	CAMERA_SetReg(0x6f,0x69);
	CAMERA_SetReg(0x70,0x78);
	CAMERA_SetReg(0x71,0x85);
	CAMERA_SetReg(0x72,0x91);
	CAMERA_SetReg(0x73,0xff);
	CAMERA_SetReg(0x74,0x9c);
	CAMERA_SetReg(0x75,0xa7);
	CAMERA_SetReg(0x76,0xb2);
	CAMERA_SetReg(0x77,0xbd);
	CAMERA_SetReg(0x78,0xff);
	CAMERA_SetReg(0x79,0xc7);
	CAMERA_SetReg(0x7a,0xd2);
	CAMERA_SetReg(0x7b,0xdb);
	CAMERA_SetReg(0x7c,0xe4);
	CAMERA_SetReg(0x7d,0xff);
	CAMERA_SetReg(0x7e,0xec);
	CAMERA_SetReg(0x7f,0xf5);
	CAMERA_SetReg(0x80,0xf0);
	
	CAMERA_SetReg(0xfc,0x00);                              
	CAMERA_SetReg(0x48,0x40);// 2000K            
	CAMERA_SetReg(0x49,0x40);	                     
	CAMERA_SetReg(0x4a,0x00);	                     
	CAMERA_SetReg(0x4b,0x00);	                     
	CAMERA_SetReg(0x4c,0x40);	                     
	CAMERA_SetReg(0x4d,0x40);                  
	CAMERA_SetReg(0x4e,0x00);	                     
	CAMERA_SetReg(0x4f,0x00);	                     
	
	CAMERA_SetReg(0x50,0x40);// 3000K            
	CAMERA_SetReg(0x51,0x40);	                     
	CAMERA_SetReg(0x52,0x00);	                     
	CAMERA_SetReg(0x53,0x00);	                     
	CAMERA_SetReg(0x54,0x40);	                     
	CAMERA_SetReg(0x55,0x40);                     
	CAMERA_SetReg(0x56,0x00);	                     
	CAMERA_SetReg(0x57,0x00);
	
	CAMERA_SetReg(0x58,0x38);             
	CAMERA_SetReg(0x59,0x35);                  
	CAMERA_SetReg(0x5a,0x02);                  
	CAMERA_SetReg(0x5b,0x00); 
	CAMERA_SetReg(0x5c,0x40); 
	CAMERA_SetReg(0x5d,0x3a); 
	CAMERA_SetReg(0x5e,0x00);                  
	CAMERA_SetReg(0x5f,0xfe);             
	
	CAMERA_SetReg(0xfc,0x00);// SUPPRESS FUNCTION     
	CAMERA_SetReg(0x7e,0xf4);                                     
	
	CAMERA_SetReg(0xfc,0x01);        
	CAMERA_SetReg(0x3d,0x10);
	
	CAMERA_SetReg(0xfc,0x0b);  
	CAMERA_SetReg(0x0b,0x00);// ISP BPR On Start
	CAMERA_SetReg(0x0c,0x40);// Th13 AGC Min
	CAMERA_SetReg(0x0d,0x5a);// Th13 AGC Max
	CAMERA_SetReg(0x0e,0x00);// Th1 Max H for AGCMIN
	CAMERA_SetReg(0x0f,0x15);//0822 semi Th1 Max L for AGCMIN
	CAMERA_SetReg(0x10,0x00);// Th1 Min H for AGCMAX
	CAMERA_SetReg(0x11,0x20);// 0822 semi Th1 Min L for AGCMAX
	CAMERA_SetReg(0x12,0x00);// Th3 Max H for AGCMIN
	CAMERA_SetReg(0x13,0x7f);// Th3 Max L for AGCMIN
	CAMERA_SetReg(0x14,0x03);// Th3 Min H for AGCMAX
	CAMERA_SetReg(0x15,0xff);// Th3 Min L for AGCMAX
	CAMERA_SetReg(0x16,0x48);// Th57 AGC Min
	CAMERA_SetReg(0x17,0x60);// Th57 AGC Max
	CAMERA_SetReg(0x18,0x00);// Th5 Max H for AGCMIN
	CAMERA_SetReg(0x19,0x00);// Th5 Max L for AGCMIN
	CAMERA_SetReg(0x1a,0x00);// Th5 Min H for AGCMAX
	CAMERA_SetReg(0x1b,0x20);// Th5 Min L for AGCMAX
	CAMERA_SetReg(0x1c,0x00);// Th7 Max H for AGCMIN
	CAMERA_SetReg(0x1d,0x00);// Th7 Max L for AGCMIN
	CAMERA_SetReg(0x1e,0x00);// Th7 Min H for AGCMAX
	CAMERA_SetReg(0x1f,0x20);// Th7 Min L for AGCMAX 	
	
	CAMERA_SetReg(0xfc,0x01);// GR/GB CORRECTION             
	CAMERA_SetReg(0x45,0x0c);  	                       
	CAMERA_SetReg(0xfc,0x0b);                                 
	CAMERA_SetReg(0x21,0x00);// Start AGC                   
	CAMERA_SetReg(0x22,0x05);// AGCMIN                      
	CAMERA_SetReg(0x23,0x33);// AGCMAX                      
	CAMERA_SetReg(0x24,0x0a);// G Th AGCMIN                 
	CAMERA_SetReg(0x25,0x25);// G Th AGCMAX                 
	CAMERA_SetReg(0x26,0x0a);// RB Th AGCMIN                
	CAMERA_SetReg(0x27,0x25);// RB Th AGCMAX      
	
	CAMERA_SetReg(0xfc,0x01);  
	CAMERA_SetReg(0x4C,0x01);// NR Enable
	CAMERA_SetReg(0x49,0x15);// Sig_Th Mult
	CAMERA_SetReg(0x4B,0x0A);// Pre_Th Mult
	
	CAMERA_SetReg(0xfc,0x0b);                            
	CAMERA_SetReg(0x28,0x00);// NR start AGC	           
	CAMERA_SetReg(0x29,0x00);// SIG Th AGCMIN H   
	CAMERA_SetReg(0x2a,0x14);// SIG Th AGCMIN L   
	CAMERA_SetReg(0x2b,0x00);// SIG Th AGCMAX H   
	CAMERA_SetReg(0x2c,0x14);// SIG Th AGCMAX L   
	CAMERA_SetReg(0x2d,0x00);// PRE Th AGCMIN H   
	CAMERA_SetReg(0x2e,0xc0);// PRE Th AGCMIN L   
	CAMERA_SetReg(0x2f,0x01);//01 PRE Th AGCMAX H   
	CAMERA_SetReg(0x30,0x00);//57 c0 // PRE Th AGCMAX L   
	CAMERA_SetReg(0x31,0x00);// POST Th AGCMIN H  
	CAMERA_SetReg(0x32,0xd0);//b0 POST Th AGCMIN L  
	CAMERA_SetReg(0x33,0x01);// POST Th AGCMAX H
	CAMERA_SetReg(0x34,0x10);// POST Th AGCMAX L  
	
	CAMERA_SetReg(0xfc,0x01);                            
	CAMERA_SetReg(0x05,0xC0);
	
	CAMERA_SetReg(0xfc,0x0b);                            
	CAMERA_SetReg(0x35,0x00);// YLPF start AGC      
	CAMERA_SetReg(0x36,0x20);// YLPF01 AGCMIN       
	CAMERA_SetReg(0x37,0x50);// YLPF01 AGCMAX       
	CAMERA_SetReg(0x38,0x00);// YLPF SIG01 Th AGCMINH
	CAMERA_SetReg(0x39,0x18);// YLPF SIG01 Th AGCMINL   
	CAMERA_SetReg(0x3a,0x00);// YLPF SIG01 Th AGCMAXH   
	CAMERA_SetReg(0x3b,0x40);// YLPF SIG01 Th AGCMAXH               
	CAMERA_SetReg(0x3c,0x20);// YLPF02 AGCMIN                   
	CAMERA_SetReg(0x3d,0x50);// YLPF02 AGCMAX                   
	CAMERA_SetReg(0x3e,0x00);// YLPF SIG02 Th AGCMINH   
	CAMERA_SetReg(0x3f,0x30);// YLPF SIG02 Th AGCMINL   
	CAMERA_SetReg(0x40,0x00);// YLPF SIG02 Th AGCMAXH   
	CAMERA_SetReg(0x41,0x40);// YLPF SIG02 Th AGCMAXH   
	CAMERA_SetReg(0xd4,0x20);// CLPF AGCMIN       
	CAMERA_SetReg(0xd5,0x50);// CLPF AGCMAX       
	CAMERA_SetReg(0xd6,0xb0);// CLPF SIG01 Th AGCMIN
	CAMERA_SetReg(0xd7,0xf0);// CLPF SIG01 Th AGCMAX  
	CAMERA_SetReg(0xd8,0xb0);// CLPF SIG02 Th AGCMIN  
	CAMERA_SetReg(0xd9,0xf0);// CLPF SIG02 Th AGCMAX     
	
	CAMERA_SetReg(0xfc,0x0b);// COLOR SUPPRESS 
	CAMERA_SetReg(0x08,0x58);// Color suppress AGC MIN
	CAMERA_SetReg(0x09,0x04);// Color suppress MIN H
	CAMERA_SetReg(0x0a,0x00);// Color suppress MIN L     
	
	CAMERA_SetReg(0xfc,0x09);
	CAMERA_SetReg(0x01,0x06);
	CAMERA_SetReg(0x02,0x40);
	CAMERA_SetReg(0x03,0x04);
	CAMERA_SetReg(0x04,0xB0);
	CAMERA_SetReg(0x05,0x02);
	CAMERA_SetReg(0x06,0xf8);
	CAMERA_SetReg(0x07,0x02);
	CAMERA_SetReg(0x08,0x58);
	CAMERA_SetReg(0x09,0x02);
	CAMERA_SetReg(0x0A,0xe4);
	CAMERA_SetReg(0x0B,0x02);
	CAMERA_SetReg(0x0C,0xa8);
	CAMERA_SetReg(0x0D,0x02);
	CAMERA_SetReg(0x0E,0xf8);
	CAMERA_SetReg(0x0F,0x02);
	CAMERA_SetReg(0x10,0xbc);
	CAMERA_SetReg(0x1D,0x80);
	CAMERA_SetReg(0x1E,0x00);
	CAMERA_SetReg(0x1F,0x80);
	CAMERA_SetReg(0x20,0x00);
	CAMERA_SetReg(0x23,0x80);
	CAMERA_SetReg(0x24,0x00);
	CAMERA_SetReg(0x21,0x80);
	CAMERA_SetReg(0x22,0x00);
	CAMERA_SetReg(0x25,0x80);
	CAMERA_SetReg(0x26,0x00);
	CAMERA_SetReg(0x27,0x80);
	CAMERA_SetReg(0x28,0x00);
	CAMERA_SetReg(0x2B,0x80);
	CAMERA_SetReg(0x2C,0x00);
	CAMERA_SetReg(0x29,0x80);
	CAMERA_SetReg(0x2A,0x00);
	CAMERA_SetReg(0x2D,0x80);
	CAMERA_SetReg(0x2E,0x00);
	CAMERA_SetReg(0x2F,0x80);
	CAMERA_SetReg(0x30,0x00);
	CAMERA_SetReg(0x33,0x80);
	CAMERA_SetReg(0x34,0x00);
	CAMERA_SetReg(0x31,0x80);
	CAMERA_SetReg(0x32,0x00);
	CAMERA_SetReg(0x35,0x01);
	CAMERA_SetReg(0x36,0x00);
	CAMERA_SetReg(0x37,0x01);
	CAMERA_SetReg(0x38,0x0a);
	CAMERA_SetReg(0x39,0x01);
	CAMERA_SetReg(0x3A,0x25);
	CAMERA_SetReg(0x3B,0x01);
	CAMERA_SetReg(0x3C,0x51);
	CAMERA_SetReg(0x3D,0x01);
	CAMERA_SetReg(0x3E,0x6f);
	CAMERA_SetReg(0x3F,0x01);
	CAMERA_SetReg(0x40,0x94);
	CAMERA_SetReg(0x41,0x01);
	CAMERA_SetReg(0x42,0xb5);
	CAMERA_SetReg(0x43,0x01);
	CAMERA_SetReg(0x44,0xde);
	CAMERA_SetReg(0x45,0x01);
	CAMERA_SetReg(0x46,0x00);
	CAMERA_SetReg(0x47,0x01);
	CAMERA_SetReg(0x48,0x04);
	CAMERA_SetReg(0x49,0x01);
	CAMERA_SetReg(0x4A,0x1c);
	CAMERA_SetReg(0x4B,0x01);
	CAMERA_SetReg(0x4C,0x41);
	CAMERA_SetReg(0x4D,0x01);
	CAMERA_SetReg(0x4E,0x59);
	CAMERA_SetReg(0x4F,0x01);
	CAMERA_SetReg(0x50,0x72);
	CAMERA_SetReg(0x51,0x01);
	CAMERA_SetReg(0x52,0x92);
	CAMERA_SetReg(0x53,0x01);
	CAMERA_SetReg(0x54,0xb7);
	CAMERA_SetReg(0x55,0x01);
	CAMERA_SetReg(0x56,0x00);
	CAMERA_SetReg(0x57,0x01);
	CAMERA_SetReg(0x58,0x02);
	CAMERA_SetReg(0x59,0x01);
	CAMERA_SetReg(0x5A,0x14);
	CAMERA_SetReg(0x5B,0x01);
	CAMERA_SetReg(0x5C,0x33);
	CAMERA_SetReg(0x5D,0x01);
	CAMERA_SetReg(0x5E,0x47);
	CAMERA_SetReg(0x5F,0x01);
	CAMERA_SetReg(0x60,0x5d);
	CAMERA_SetReg(0x61,0x01);
	CAMERA_SetReg(0x62,0x76);
	CAMERA_SetReg(0x63,0x01);
	CAMERA_SetReg(0x64,0x8e);
	
	CAMERA_SetReg(0x65,0x00);
	CAMERA_SetReg(0x66,0x90);
	CAMERA_SetReg(0x67,0xEC);
	CAMERA_SetReg(0x68,0x02);
	CAMERA_SetReg(0x69,0x43);
	CAMERA_SetReg(0x6A,0xB1);
	CAMERA_SetReg(0x6B,0x05);
	CAMERA_SetReg(0x6C,0x18);
	CAMERA_SetReg(0x6D,0x4F);
	CAMERA_SetReg(0x6E,0x06);
	CAMERA_SetReg(0x6F,0xEF);
	CAMERA_SetReg(0x70,0x4E);
	CAMERA_SetReg(0x71,0x09);
	CAMERA_SetReg(0x72,0x0E);
	CAMERA_SetReg(0x73,0xC5);
	CAMERA_SetReg(0x74,0x0B);
	CAMERA_SetReg(0x75,0x76);
	CAMERA_SetReg(0x76,0xB1);
	CAMERA_SetReg(0x77,0x0E);
	CAMERA_SetReg(0x78,0x27);
	CAMERA_SetReg(0x79,0x13);
	
	CAMERA_SetReg(0x7A,0x00);
	CAMERA_SetReg(0x7B,0x9E);
	CAMERA_SetReg(0x7C,0x17);
	CAMERA_SetReg(0x7D,0x02);
	CAMERA_SetReg(0x7E,0x78);
	CAMERA_SetReg(0x7F,0x5B);
	CAMERA_SetReg(0x80,0x05);
	CAMERA_SetReg(0x81,0x8E);
	CAMERA_SetReg(0x82,0xCE);
	CAMERA_SetReg(0x83,0x07);
	CAMERA_SetReg(0x84,0x90);
	CAMERA_SetReg(0x85,0x98);
	CAMERA_SetReg(0x86,0x09);
	CAMERA_SetReg(0x87,0xE1);
	CAMERA_SetReg(0x88,0x6E);
	CAMERA_SetReg(0x89,0x0C);
	CAMERA_SetReg(0x8A,0x81);
	CAMERA_SetReg(0x8B,0x4F);
	CAMERA_SetReg(0x8C,0x0F);
	CAMERA_SetReg(0x8D,0x70);
	CAMERA_SetReg(0x8E,0x3B);
	
	CAMERA_SetReg(0x8F,0x00);
	CAMERA_SetReg(0x90,0x9C);
	CAMERA_SetReg(0x91,0x43);
	CAMERA_SetReg(0x92,0x02);
	CAMERA_SetReg(0x93,0x71);
	CAMERA_SetReg(0x94,0x0A);
	CAMERA_SetReg(0x95,0x05);
	CAMERA_SetReg(0x96,0x7E);
	CAMERA_SetReg(0x97,0x57);
	CAMERA_SetReg(0x98,0x07);
	CAMERA_SetReg(0x99,0x7A);
	CAMERA_SetReg(0x9A,0x2F);
	CAMERA_SetReg(0x9B,0x09);
	CAMERA_SetReg(0x9C,0xC4);
	CAMERA_SetReg(0x9D,0x28);
	CAMERA_SetReg(0x9E,0x0C);
	CAMERA_SetReg(0x9F,0x5C);
	CAMERA_SetReg(0xA0,0x43);
	CAMERA_SetReg(0xA1,0x0F);
	CAMERA_SetReg(0xA2,0x42);
	CAMERA_SetReg(0xA3,0x7F);
	
	CAMERA_SetReg(0xA4,0x71);
	CAMERA_SetReg(0xA5,0x0D);
	CAMERA_SetReg(0xA6,0x25);
	CAMERA_SetReg(0xA7,0xAF);
	CAMERA_SetReg(0xA8,0x16);
	CAMERA_SetReg(0xA9,0x9C);
	CAMERA_SetReg(0xAA,0x22);
	CAMERA_SetReg(0xAB,0xC9);
	CAMERA_SetReg(0xAC,0x1E);
	CAMERA_SetReg(0xAD,0x25);
	CAMERA_SetReg(0xAE,0x1A);
	CAMERA_SetReg(0xAF,0x99);
	CAMERA_SetReg(0xB0,0x17);
	CAMERA_SetReg(0xB1,0xCC);
	
	CAMERA_SetReg(0xB2,0x67);
	CAMERA_SetReg(0xB3,0xA3);
	CAMERA_SetReg(0xB4,0x22);
	CAMERA_SetReg(0xB5,0x8B);
	CAMERA_SetReg(0xB6,0x14);
	CAMERA_SetReg(0xB7,0xBA);
	CAMERA_SetReg(0xB8,0x1F);
	CAMERA_SetReg(0xB9,0xE3);
	CAMERA_SetReg(0xBA,0x1B);
	CAMERA_SetReg(0xBB,0xA2);
	CAMERA_SetReg(0xBC,0x18);
	CAMERA_SetReg(0xBD,0x62);
	CAMERA_SetReg(0xBE,0x15);
	CAMERA_SetReg(0xBF,0xD1);
	
	CAMERA_SetReg(0xC0,0x68);
	CAMERA_SetReg(0xC1,0xD9);
	CAMERA_SetReg(0xC2,0x22);
	CAMERA_SetReg(0xC3,0xF3);
	CAMERA_SetReg(0xC4,0x14);
	CAMERA_SetReg(0xC5,0xF8);
	CAMERA_SetReg(0xC6,0x20);
	CAMERA_SetReg(0xC7,0x43);
	CAMERA_SetReg(0xC8,0x1B);
	CAMERA_SetReg(0xC9,0xF5);
	CAMERA_SetReg(0xCA,0x18);
	CAMERA_SetReg(0xCB,0xAB);
	CAMERA_SetReg(0xCC,0x16);
	CAMERA_SetReg(0xCD,0x12);
	
	CAMERA_SetReg(0x00,0x02);// shading on
	
	CAMERA_SetReg(0xfc,0x1B);// X-SHADING 
	CAMERA_SetReg(0x80,0x01);
	CAMERA_SetReg(0x81,0x00);
	CAMERA_SetReg(0x82,0x4C);
	CAMERA_SetReg(0x83,0x00);
	CAMERA_SetReg(0x84,0x86);
	CAMERA_SetReg(0x85,0x03);
	CAMERA_SetReg(0x86,0x5E);
	CAMERA_SetReg(0x87,0x00);
	CAMERA_SetReg(0x88,0x07);
	CAMERA_SetReg(0x89,0xA4);
	CAMERA_SetReg(0x90,0x00);
	CAMERA_SetReg(0x91,0x88);
	CAMERA_SetReg(0x92,0x00);
	CAMERA_SetReg(0x93,0xC1);
	CAMERA_SetReg(0x94,0x00);
	CAMERA_SetReg(0x95,0xF7);
	CAMERA_SetReg(0x96,0x01);
	CAMERA_SetReg(0x97,0x21);
	CAMERA_SetReg(0x98,0x01);
	CAMERA_SetReg(0x99,0x37);
	CAMERA_SetReg(0x9A,0x01);
	CAMERA_SetReg(0x9B,0x0C);
	CAMERA_SetReg(0x9C,0x00);
	CAMERA_SetReg(0x9D,0xCE);
	CAMERA_SetReg(0x9E,0x00);
	CAMERA_SetReg(0x9F,0x3B);
	CAMERA_SetReg(0xA0,0x00);
	CAMERA_SetReg(0xA1,0x5B);
	CAMERA_SetReg(0xA2,0x00);
	CAMERA_SetReg(0xA3,0x7A);
	CAMERA_SetReg(0xA4,0x00);
	CAMERA_SetReg(0xA5,0x92);
	CAMERA_SetReg(0xA6,0x00);
	CAMERA_SetReg(0xA7,0x91);
	CAMERA_SetReg(0xA8,0x00);
	CAMERA_SetReg(0xA9,0x81);
	CAMERA_SetReg(0xAA,0x00);
	CAMERA_SetReg(0xAB,0x60);
	CAMERA_SetReg(0xAC,0x07);
	CAMERA_SetReg(0xAD,0xCB);
	CAMERA_SetReg(0xAE,0x07);
	CAMERA_SetReg(0xAF,0xC5);
	CAMERA_SetReg(0xB0,0x07);
	CAMERA_SetReg(0xB1,0xBB);
	CAMERA_SetReg(0xB2,0x07);
	CAMERA_SetReg(0xB3,0xAA);
	CAMERA_SetReg(0xB4,0x07);
	CAMERA_SetReg(0xB5,0xA9);
	CAMERA_SetReg(0xB6,0x07);
	CAMERA_SetReg(0xB7,0xB2);
	CAMERA_SetReg(0xB8,0x07);
	CAMERA_SetReg(0xB9,0xBF);
	CAMERA_SetReg(0xBA,0x07);
	CAMERA_SetReg(0xBB,0x5E);
	CAMERA_SetReg(0xBC,0x07);
	CAMERA_SetReg(0xBD,0x3C);
	CAMERA_SetReg(0xBE,0x06);
	CAMERA_SetReg(0xBF,0xF9);
	CAMERA_SetReg(0xC0,0x06);
	CAMERA_SetReg(0xC1,0xBD);
	CAMERA_SetReg(0xC2,0x06);
	CAMERA_SetReg(0xC3,0xB8);
	CAMERA_SetReg(0xC4,0x06);
	CAMERA_SetReg(0xC5,0xE2);
	CAMERA_SetReg(0xC6,0x07);
	CAMERA_SetReg(0xC7,0x1A);
	CAMERA_SetReg(0xC8,0x07);
	CAMERA_SetReg(0xC9,0x15);
	CAMERA_SetReg(0xCA,0x06);
	CAMERA_SetReg(0xCB,0xDE);
	CAMERA_SetReg(0xCC,0x06);
	CAMERA_SetReg(0xCD,0x9C);
	CAMERA_SetReg(0xCE,0x06);
	CAMERA_SetReg(0xCF,0x6F);
	CAMERA_SetReg(0xD0,0x06);
	CAMERA_SetReg(0xD1,0x5E);
	CAMERA_SetReg(0xD2,0x06);
	CAMERA_SetReg(0xD3,0x84);
	CAMERA_SetReg(0xD4,0x06);
	CAMERA_SetReg(0xD5,0xCA);
	
	CAMERA_SetReg(0xfc,0x0b);// x-shading temp. correlation factor
	CAMERA_SetReg(0xda,0x00);// t0(3100K)
	CAMERA_SetReg(0xdb,0x9c);
	CAMERA_SetReg(0xdc,0x00);// tc(5100K)
	CAMERA_SetReg(0xdd,0xd1);// default eeh
	
	CAMERA_SetReg(0xfc,0x1b);       
	CAMERA_SetReg(0x80,0x01);// X-Shading On
	
	CAMERA_SetReg(0xfc,0x00);// AE WINDOW WEIGHT  
	CAMERA_SetReg(0x03,0x4b);// AE Suppress On
	CAMERA_SetReg(0xfc,0x06);
	CAMERA_SetReg(0x01,0x35);// UXGA AE Window
	CAMERA_SetReg(0x03,0xc2);
	CAMERA_SetReg(0x05,0x48);
	CAMERA_SetReg(0x07,0xb8);
	CAMERA_SetReg(0x31,0x2a);// Subsampling AE Window
	CAMERA_SetReg(0x33,0x61);
	CAMERA_SetReg(0x35,0x28);
	CAMERA_SetReg(0x37,0x5c);
	
	CAMERA_SetReg(0xfc,0x20); 	
	CAMERA_SetReg(0x60,0x33);//11		
	CAMERA_SetReg(0x61,0x33);//11		
	CAMERA_SetReg(0x62,0x33);//11		
	CAMERA_SetReg(0x63,0x33);//11		
	CAMERA_SetReg(0x64,0x31);//11		
	CAMERA_SetReg(0x65,0x55);//22	 
	CAMERA_SetReg(0x66,0x55);//22	 
	CAMERA_SetReg(0x67,0x13);//11		
	CAMERA_SetReg(0x68,0x21);//11	 
	CAMERA_SetReg(0x69,0x55);//33	 
	CAMERA_SetReg(0x6a,0x55);//33	 
	CAMERA_SetReg(0x6b,0x12);//11	 
	CAMERA_SetReg(0x6c,0x11);//12	 
	CAMERA_SetReg(0x6d,0x33);//55	 
	CAMERA_SetReg(0x6e,0x33);//55	 
	CAMERA_SetReg(0x6f,0x11);//21	 
	CAMERA_SetReg(0x70,0x11);//13	 
	CAMERA_SetReg(0x71,0x22);//55	 
	CAMERA_SetReg(0x72,0x22);//55	 
	CAMERA_SetReg(0x73,0x11);//31	 
	CAMERA_SetReg(0x74,0x11);//33	 
	CAMERA_SetReg(0x75,0x11);//33	 
	CAMERA_SetReg(0x76,0x11);//33	 
	CAMERA_SetReg(0x77,0x11);//33	   
	
	CAMERA_SetReg(0xfc,0x00);
	CAMERA_SetReg(0x7b,0x00);// AE AWB ON  
	
	CAMERA_SetReg(0xfc,0x07);
	CAMERA_SetReg(0x3c,0x10);// AWB cut R min 
	CAMERA_SetReg(0x3d,0x10);// AWB cut B min
	CAMERA_SetReg(0x3e,0x10);// AWB cut R max
	CAMERA_SetReg(0x3f,0x10);// AWB cut B max
	
	CAMERA_SetReg(0xfc,0x01);
	CAMERA_SetReg(0xc8,0xe0);// AWB Y Max
	CAMERA_SetReg(0xfc,0x00);
	CAMERA_SetReg(0x3e,0x10);// AWB Y_min   
	
	CAMERA_SetReg(0xfc,0x00);
	CAMERA_SetReg(0x3e,0x10);// AWB Y_min Normal     
	CAMERA_SetReg(0x3d,0x04);// AWB Y_min Low
	CAMERA_SetReg(0x32,0x02);// AWB moving average 8 frame
	CAMERA_SetReg(0x81,0x10);// AWB G gain suppress Disable 
	CAMERA_SetReg(0xbc,0xf0);
	CAMERA_SetReg(0xfc,0x22);                      
	CAMERA_SetReg(0x8c,0x04);// AWB Min Y Weight check
	CAMERA_SetReg(0x8d,0x06);// AWB Max Y Weight check
	
	CAMERA_SetReg(0xfc,0x07);
	CAMERA_SetReg(0x97,0x00);// S/W Y_lowpass Filter OFF
	
	CAMERA_SetReg(0xfc,0x22);// White Point (For Hue Control & MWB)          
	CAMERA_SetReg(0x01,0xD8);// D65                                     
	CAMERA_SetReg(0x03,0xA1);                                         
	CAMERA_SetReg(0x05,0xCA);// 5000K                
	CAMERA_SetReg(0x07,0xC8);                      
	CAMERA_SetReg(0x09,0xB3);// CWF                                  
	CAMERA_SetReg(0x0b,0xE2);                                        
	CAMERA_SetReg(0x0d,0xA0);// 3000K                                        
	CAMERA_SetReg(0x0f,0xF0);                                                
	CAMERA_SetReg(0x11,0x94);// A                                    
	CAMERA_SetReg(0x12,0x00);                                                
	CAMERA_SetReg(0x13,0xFD);                                   
	CAMERA_SetReg(0x15,0x88);// 2000K                                
	CAMERA_SetReg(0x16,0x01);                                                   
	CAMERA_SetReg(0x17,0x10);
	
	CAMERA_SetReg(0xfc,0x22);
	CAMERA_SetReg(0xA8,0xFF);
	
	CAMERA_SetReg(0xA0,0x01);
	CAMERA_SetReg(0xA1,0x38);
	CAMERA_SetReg(0xA2,0x0E);
	CAMERA_SetReg(0xA3,0x6D);
	CAMERA_SetReg(0xA4,0x07);
	CAMERA_SetReg(0xA5,0xF5);
	CAMERA_SetReg(0xA6,0x11);
	CAMERA_SetReg(0xA7,0xBE);
	CAMERA_SetReg(0xA9,0x02);
	CAMERA_SetReg(0xAA,0xD2);
	CAMERA_SetReg(0xAB,0x00);
	CAMERA_SetReg(0xAC,0x00);
	CAMERA_SetReg(0xAD,0x02);
	CAMERA_SetReg(0xAE,0x3F);
	CAMERA_SetReg(0xAF,0x19);
	CAMERA_SetReg(0xB0,0x91);
	CAMERA_SetReg(0x94,0x3D);
	CAMERA_SetReg(0x95,0x00);
	CAMERA_SetReg(0x96,0x58);
	CAMERA_SetReg(0x97,0x80);
	CAMERA_SetReg(0xD0,0xA2);
	CAMERA_SetReg(0xD1,0x2E);
	CAMERA_SetReg(0xD2,0x4D);
	CAMERA_SetReg(0xD3,0x28);
	CAMERA_SetReg(0xD4,0x90);
	CAMERA_SetReg(0xDB,0x2E);
	CAMERA_SetReg(0xDC,0x7A);
	CAMERA_SetReg(0xDD,0x28);
	CAMERA_SetReg(0xE7,0x00);
	CAMERA_SetReg(0xE8,0xae);
	CAMERA_SetReg(0xE9,0x00);
	CAMERA_SetReg(0xEA,0x80);
	CAMERA_SetReg(0xEB,0x03);
	CAMERA_SetReg(0xEC,0xc1);
	CAMERA_SetReg(0xEE,0xa6);
	
	CAMERA_SetReg(0xfc,0x00);
	CAMERA_SetReg(0x8a,0x02);// AWB difference threshold check     
	
	CAMERA_SetReg(0xFC,0x07);
	CAMERA_SetReg(0x95,0xCF);
	
	CAMERA_SetReg(0xfc,0x01);
	CAMERA_SetReg(0xd3,0x4f);
	CAMERA_SetReg(0xd4,0x00);
	CAMERA_SetReg(0xd5,0x3c);
	CAMERA_SetReg(0xd6,0x80);
	CAMERA_SetReg(0xd7,0x61);
	CAMERA_SetReg(0xd8,0x00);
	CAMERA_SetReg(0xd9,0x49);
	CAMERA_SetReg(0xda,0x00);
	CAMERA_SetReg(0xdb,0x24);
	CAMERA_SetReg(0xdc,0x4b);
	CAMERA_SetReg(0xdd,0x23);
	CAMERA_SetReg(0xde,0xf2);
	CAMERA_SetReg(0xdf,0x20);
	CAMERA_SetReg(0xe0,0x73);
	CAMERA_SetReg(0xe1,0x18);
	CAMERA_SetReg(0xe2,0x69);
	CAMERA_SetReg(0xe3,0x31);
	CAMERA_SetReg(0xe4,0x40);
	CAMERA_SetReg(0xe5,0x34);
	CAMERA_SetReg(0xe6,0x40);
	CAMERA_SetReg(0xe7,0x40);
	CAMERA_SetReg(0xe8,0x32);
	CAMERA_SetReg(0xe9,0x40);
	CAMERA_SetReg(0xea,0x1c);
	CAMERA_SetReg(0xeb,0x00);
	
	CAMERA_SetReg(0xfc,0x22);
	CAMERA_SetReg(0x18,0x00);
	CAMERA_SetReg(0x19,0x4b);
	CAMERA_SetReg(0x1a,0xfd);
	CAMERA_SetReg(0x1b,0x00);
	CAMERA_SetReg(0x1c,0x41);
	CAMERA_SetReg(0x1d,0xd9);
	CAMERA_SetReg(0x1e,0x00);
	CAMERA_SetReg(0x1f,0x66);
	CAMERA_SetReg(0x20,0xa9);
	CAMERA_SetReg(0x21,0x00);
	CAMERA_SetReg(0x22,0x8b);
	CAMERA_SetReg(0x23,0x82);
	CAMERA_SetReg(0x24,0x00);
	CAMERA_SetReg(0x25,0xa4);
	CAMERA_SetReg(0x26,0x6c);
	CAMERA_SetReg(0x27,0x00);
	CAMERA_SetReg(0x28,0xbd);
	CAMERA_SetReg(0x29,0x5d);
	CAMERA_SetReg(0x2a,0x00);
	CAMERA_SetReg(0x2b,0xdc);
	CAMERA_SetReg(0x2c,0x4d);
	CAMERA_SetReg(0x2d,0x00);
	CAMERA_SetReg(0x2e,0xdc);
	CAMERA_SetReg(0x2f,0x63);
	CAMERA_SetReg(0x30,0x00);
	CAMERA_SetReg(0x31,0xc1);
	CAMERA_SetReg(0x32,0x72);
	CAMERA_SetReg(0x33,0x00);
	CAMERA_SetReg(0x34,0xab);
	CAMERA_SetReg(0x35,0x84);
	CAMERA_SetReg(0x36,0x00);
	CAMERA_SetReg(0x37,0x99);
	CAMERA_SetReg(0x38,0xa0);
	CAMERA_SetReg(0x39,0x00);
	CAMERA_SetReg(0x3a,0x81);
	CAMERA_SetReg(0x3b,0xe9);
	CAMERA_SetReg(0x3c,0x00);
	CAMERA_SetReg(0x3d,0x00);
	CAMERA_SetReg(0x3e,0x00);
	CAMERA_SetReg(0x3f,0x00);
	CAMERA_SetReg(0x40,0x00);
	CAMERA_SetReg(0x41,0x00);
	
	CAMERA_SetReg(0xfc,0x22);
	CAMERA_SetReg(0x98,0x07);// Moving Equation weight
	
	CAMERA_SetReg(0xfc,0x22);
	CAMERA_SetReg(0xb1,0x00);// Sunny
	CAMERA_SetReg(0xb2,0x04);//04
	CAMERA_SetReg(0xb3,0x00);
	CAMERA_SetReg(0xb4,0xC1);
	
	CAMERA_SetReg(0xb5,0x00);// Cloudy  
	CAMERA_SetReg(0xb6,0x04);//04
	CAMERA_SetReg(0xb7,0x00);
	CAMERA_SetReg(0xb9,0xc2);
	
	CAMERA_SetReg(0xd7,0x00);// Shade 
	CAMERA_SetReg(0xd8,0x35);
	CAMERA_SetReg(0xd9,0x20);
	CAMERA_SetReg(0xda,0x81);
	
	CAMERA_SetReg(0xfc,0x00);
	CAMERA_SetReg(0x79,0xf2);
	CAMERA_SetReg(0x7a,0x0b);//10
	
	CAMERA_SetReg(0xfc,0x07);
	CAMERA_SetReg(0x11,0xff);
	
	CAMERA_SetReg(0xfc,0x22);       
	CAMERA_SetReg(0x58,0xfa);//00	             // D65 R Offset
	CAMERA_SetReg(0x59,0xfb);//00	             // D65 B Offset
	CAMERA_SetReg(0x5A,0xfd);//00	             // 5000K R Offset
	CAMERA_SetReg(0x5B,0x00);//00	             // 5000K B Offset
	CAMERA_SetReg(0x5C,0xfe);//00	             // CWF R Offset
	CAMERA_SetReg(0x5D,0x00);// CWF B Offset
	CAMERA_SetReg(0x5E,0xfe);// 3000K R Offset
	CAMERA_SetReg(0x5F,0x00);//00	             // 3000K B Offset
	CAMERA_SetReg(0x60,0xfe);//00	             // A R Offset 
	CAMERA_SetReg(0x61,0x00);//00	             // A B Offset 
	CAMERA_SetReg(0x62,0x06);// 2000K R Offset
	CAMERA_SetReg(0x63,0x00);//00 	           // 2000K B Offset
	
	CAMERA_SetReg(0xde,0x00);// LARGE OBJECT BUG FIX
	CAMERA_SetReg(0xf0,0x6a);// RB Ratio
	
	CAMERA_SetReg(0xfc,0x22);
	CAMERA_SetReg(0xb9,0x00);
	CAMERA_SetReg(0xba,0x00);
	CAMERA_SetReg(0xbb,0x00);
	CAMERA_SetReg(0xbc,0x00);
	CAMERA_SetReg(0xe5,0x01);
	CAMERA_SetReg(0xe6,0xff);
	CAMERA_SetReg(0xbd,0x0c);           
	
	CAMERA_SetReg(0xfc,0x07);// Special Effect
	CAMERA_SetReg(0x30,0xc0);
	CAMERA_SetReg(0x31,0x20);
	CAMERA_SetReg(0x32,0x40);
	CAMERA_SetReg(0x33,0xc0);
	CAMERA_SetReg(0x34,0x00);
	CAMERA_SetReg(0x35,0xb0);  
	
	CAMERA_SetReg(0xfc,0x01);               
	CAMERA_SetReg(0x01,0x01);// VCK Inversion
	CAMERA_SetReg(0x00,0x90);// sck inv.
	CAMERA_SetReg(0xfc,0x02);
	CAMERA_SetReg(0x03,0x20);// SCK inv.
	
	CAMERA_SetReg(0xfc,0x20);// page 20
	CAMERA_SetReg(0x0f,0x00);// Weight Mirrored (0-63)
	
	CAMERA_SetReg(0xfc,0x01);               
	CAMERA_SetReg(0x02,0x00);// CrCb_sel
	
	CAMERA_SetReg(0xfc,0x00);
	CAMERA_SetReg(0x50,0x40);// 3000K            
	CAMERA_SetReg(0x51,0x40);	                     
	CAMERA_SetReg(0x52,0x00);	                     
	CAMERA_SetReg(0x53,0x00);	                     
	CAMERA_SetReg(0x54,0x40);	                     
	CAMERA_SetReg(0x55,0x40);                     
	CAMERA_SetReg(0x56,0x00);	                     
	CAMERA_SetReg(0x57,0x00);
	
	CAMERA_SetReg(0xfc,0x05);
	CAMERA_SetReg(0x34,0x20);// APTCLP  
	CAMERA_SetReg(0x35,0x04);// APTSC 
	
	CAMERA_SetReg(0xfc,0x00);// page 0
	CAMERA_SetReg(0x03,0x4b);// AE/AWB On
	CAMERA_SetReg(0x7e,0xf4);// Suppress On
	
	CAMERA_SetReg(0xfc,0x02);// page 2
	CAMERA_SetReg(0x37,0x17);// Global Gain
	
	CAMERA_SetReg(0xfc,0x00);// page 0
	CAMERA_SetReg(0x75,0x05);// Mirror
	
	CAMERA_SetReg(0xfc,0x1d);
	CAMERA_SetReg(0x85,0x01);// New Edge On
	
	CAMERA_SetReg(0xfc,0x00);
	CAMERA_SetReg(0x02,0x09);

	*/

//	CAMERA_SetReg(0xfc, 0x01);
	//CAMERA_SetReg(0x02, 0x00);
//	CAMERA_SetReg(0x02, 0x02);//Donghoon

	CAMERA_SetSizeInS5K4BAF(eSub);

	CAMERA_IICClose();


	
#endif


	CAMERA_IICClose();

}


void CAMERA_Init_K5K4CA()
{
	u32 uHCLKx2;
	u32 IMAGE = false;
	u16 ucData;

	SYSC_GetClkInform(); // Get HCLKx2 Frequency
	uHCLKx2 = g_HCLKx2/1000000;
	
	oCamera.m_ucSlaveAddr = 0xac;

	CAMERA_IICOpen(100000); 

	CAMERA_SetReg_4CA(0xfcfc, 0xd000);                                
	CAMERA_SetReg_4CA(0x0014, 0x0001);                 
	
	CAMERA_SetReg_4CA(0x1002, 0x1101);                                

	CAMERA_GetReg_4CA(oCamera.m_ucSlaveAddr,  ucData);

	CAMERA_IICClose();

}







//////////
// Function Name : CAMERA_InitS5K3BAF
// Function Description : camera module(S5K3BA) initaialize
// Input : 	CAM_ATTR eCcir(Camera Order), CSPACE eSrcFmt(Camera source format), IMG_SIZE eSrcSz(Camera source size)
// Output :    	None
void CAMERA_InitS5K3BAF(CAM_ATTR eCcir, CSPACE eSrcFmt, SUB_SAMPLING eSub)
{
	u32 uHCLKx2;

	SYSC_GetClkInform(); // Get HCLKx2 Frequency
	uHCLKx2 = g_HCLKx2/1000000;
	
	oCamera.m_ucSlaveAddr = 0x5a;

	CAMERA_IICOpen(300000);  // 100K SMDK 

 	 //1. initial setting 
	CAMERA_SetReg(0xfc, 0x01);                                
	CAMERA_SetReg(0x04, 0x03);  //ARM Clock divider(1/4)     

	//CAMERA_SetReg(0xfc, 0x02);                                
	//CAMERA_SetReg(0x52, 0x80);    // PLL M                     
	//CAMERA_SetReg(0x50, 0x14);    //1b    // PLL S,P (15fps)    
	//In case of PCLK = 64MHz 

	CAMERA_SetReg(0xfc, 0x02);
	CAMERA_SetReg(0x52, 0x80);  //PLL M M= 128 setting. 
//	CAMERA_SetReg(0x52, 0x);  //PLL M M= 128 setting. 	

	switch ( uHCLKx2 )
	{
		case 200:
			CAMERA_SetReg(0xfc, 0x02);
			CAMERA_SetReg(0x50, 0x19);      //PLL S= 0 , P = 25 PCLK=128/2=64Mhz 15 Frmae Setting 
//			CAMERA_SetReg(0x50, 0x14);      //PLL S= 0 , P = 20 PCLK=128/2=64Mhz 15 Frmae Setting 			
//			CAMERA_SetReg(0x50, 0x59);      //PLL S= 1 , P = 25 PCLK=64/2=32Mhz , 7~8Frame Setting
			break;
		case 266:
			CAMERA_SetReg(0xfc, 0x02);
			CAMERA_SetReg(0x50, 0x1a);      //PLL S= 0 , P = 26 : d-> S-0, P=13
			break;
		default:
			UART_Printf("Check HCLKx2 is 200 or 266Mhz!!\n");
			Assert(0);
			break;
	}

	CAMERA_SetReg(0xfc, 0x07);
	CAMERA_SetReg(0x58, 0x10);
	CAMERA_SetReg(0x59, 0x00);
	CAMERA_SetReg(0x5A, 0x00);
	CAMERA_SetReg(0x5B, 0x6c);

	CAMERA_SetReg(0xfc, 0xf0); 
	CAMERA_SetReg(0x00, 0x40); 

	CAMERA_SetReg(0xfc, 0x00);
	CAMERA_SetReg(0x62, 0x02);
	CAMERA_SetReg(0xbc, 0xe0);  // AWB_AE_DIFF

	CAMERA_SetReg(0xfc, 0x03);  //************************************************* 
	CAMERA_SetReg(0x2d, 0x03); 
	CAMERA_SetReg(0xfc, 0x01);  
	CAMERA_SetReg(0x02, 0x02);   // YCbCr Order

	CAMERA_SetReg(0xfc, 0x02);    
	CAMERA_SetReg(0x4a, 0xc1);  // SC type selection
	CAMERA_SetReg(0x37, 0x18);  //16    // SC type global gain 
	CAMERA_SetReg(0x47, 0xc4);     // r-ramp  by chin

	CAMERA_SetReg(0xfc, 0x01);  //AWB Window Area (except sky)     
	CAMERA_SetReg(0xc4, 0x01);      
	CAMERA_SetReg(0xc5, 0x4e);           
	CAMERA_SetReg(0xc7, 0x6e);

	CAMERA_SetReg(0xfc, 0x02);
	CAMERA_SetReg(0x30, 0x84);  //Analog offset

	CAMERA_SetReg(0xfc, 0x00);
	CAMERA_SetReg(0x3d, 0x10);  //AWB Low Y limit 

	CAMERA_SetReg(0xfc, 0x02);
	CAMERA_SetReg(0x3d, 0x06);  //ADLC OFF 
	CAMERA_SetReg(0x44, 0x5b);  //clamp enable
	CAMERA_SetReg(0x55, 0x03);

	CAMERA_SetReg(0xfc, 0x06); 
	CAMERA_SetReg(0x0c, 0x01); 
	CAMERA_SetReg(0x0d, 0x4e); 
	CAMERA_SetReg(0x0f, 0x6e);  

	CAMERA_SetReg(0xfc, 0x00);
	CAMERA_SetReg(0x78, 0x58);  //AGC MAX (30lux_Micron밝기Y=60code)

	CAMERA_SetReg(0xfc, 0x02);
	CAMERA_SetReg(0x45, 0x8c);  //CDS timing_저조도 greenish 해결(15fps)
	CAMERA_SetReg(0x49, 0x80);  // APS Current 2uA

	CAMERA_SetReg(0xfc, 0x01);
	CAMERA_SetReg(0x25, 0x14);  //10    //Digital Clamp 

	CAMERA_SetReg(0xfc, 0x00);
	CAMERA_SetReg(0x6d, 0x01);   //AE target high (Macbeth white=240) 
	CAMERA_SetReg(0x6c, 0x00);  //AE target (Macbeth white=240) 
	//CAMERA_SetReg(0x6d, 0x00);

	//2. ISP tuning //******************************************
	//ISP_tuning
	CAMERA_SetReg(0xfc, 0x00);  
	CAMERA_SetReg(0x01, 0x00);    // I2C hold mode off 

	CAMERA_SetReg(0xfc, 0x01);  
	CAMERA_SetReg(0x00, 0x00);      // ISP BPR Off
	CAMERA_SetReg(0x0c, 0x02);      // Full YC
	CAMERA_SetReg(0xc8, 0x19);  // AWB Y Max

	CAMERA_SetReg(0xfc, 0x00);
	CAMERA_SetReg(0x81, 0x00);      // AWB G gain suppress disable
	CAMERA_SetReg(0x29, 0x04);
	CAMERA_SetReg(0x2a, 0x00);
	CAMERA_SetReg(0x2b, 0x04);  // color level                  
	CAMERA_SetReg(0x2c, 0x00);  

	CAMERA_SetReg(0xfc, 0x07);  
	CAMERA_SetReg(0x11, 0x00);      // G offset
	CAMERA_SetReg(0x37, 0x00);      // Flicker Add

	CAMERA_SetReg(0xfc, 0x00);  
	CAMERA_SetReg(0x72, 0xa0);      // Flicker for 32MHz
	CAMERA_SetReg(0x74, 0x18);      // Flicker
	CAMERA_SetReg(0x73, 0x00);  // Frame AE

	CAMERA_SetReg(0xfc, 0x05);                   
	CAMERA_SetReg(0x64, 0x00);      // Darkslice R
	CAMERA_SetReg(0x65, 0x00);      // Darkslice G
	CAMERA_SetReg(0x66, 0x00);      // Darkslice B

	//Edge                                  
	CAMERA_SetReg(0xfc, 0x05); 
	CAMERA_SetReg(0x2c, 0x0a);  //14    // positive gain                   
	CAMERA_SetReg(0x30, 0x0a);  //10    // negative edge gain
	CAMERA_SetReg(0x34, 0x1a);   // APTCLP  
	CAMERA_SetReg(0x35, 0x10);  //0a   // APTSC  
	CAMERA_SetReg(0x36, 0x0b);   // ENHANCE 
	CAMERA_SetReg(0x3f, 0x00);   // NON-LIN 
	CAMERA_SetReg(0x45, 0x30);   // EGREF  
	CAMERA_SetReg(0x47, 0x00);   // LLREF
	CAMERA_SetReg(0x48, 0x08);   // by chin
	CAMERA_SetReg(0x49, 0x39);   // CSSEL  EGSEL  CS_DLY by 
	CAMERA_SetReg(0x40, 0x41);  // Y delay

	////////////////////////////////////
	CAMERA_SetReg(0xfc, 0x00);  
	CAMERA_SetReg(0x7e, 0xfc);  
	//s7e8c  //NR GrGb off
	// [7]: BPR [6]:Noise Filter(1D/NR) [4]: GrGb Enable [3]:BPR Data Threshold 
	// [2]: color suppress [1]: Y gain suppress [0]: Digital Clamp
	///////////////////////////////////
	////////////////////////////////////
	// GrGb Correction setting
	CAMERA_SetReg(0xfc, 0x01);  
	CAMERA_SetReg(0x44, 0x0c);       

	//s4400      
	/// [4]: GrGb full [3]: GrGb On
	/// [2]: GrGb Rb On 
	CAMERA_SetReg(0xfc, 0x0b);  
	CAMERA_SetReg(0x21, 0x00);      // Start AGC
	CAMERA_SetReg(0x22, 0x10);      // AGCMIN
	CAMERA_SetReg(0x23, 0x50);      // AGCMAX
	CAMERA_SetReg(0x24, 0x18);  // G Th AGCMIN(23d)
	CAMERA_SetReg(0x25, 0x52);      // G Th AGCMAX(50d)
	CAMERA_SetReg(0x26, 0x38);  // RB Th AGCMIN
	CAMERA_SetReg(0x27, 0x52);      // RB Th AGCMAX
	// GrGb Correction setting End

	///////////////////////////////////
	// BPR Setting      
	CAMERA_SetReg(0xfc, 0x01);         
	CAMERA_SetReg(0x3f, 0x00);      // setting because S/W bug

	CAMERA_SetReg(0xfc, 0x0b);  
	CAMERA_SetReg(0x0b, 0x00);      // ISP BPR On Start
	CAMERA_SetReg(0x0c, 0x00);      // Th13 AGC Min
	CAMERA_SetReg(0x0d, 0x5a);  // Th13 AGC Max
	CAMERA_SetReg(0x0e, 0x01);  //00    // Th1 Max H for AGCMIN
	CAMERA_SetReg(0x0f, 0xff);  //c0    // Th1 Max L for AGCMIN
	CAMERA_SetReg(0x10, 0x00);      // Th1 Min H for AGCMAX
	CAMERA_SetReg(0x11, 0x10);      //00    // Th1 Min L for AGCMAX
	CAMERA_SetReg(0x12, 0xff);      // Th3 Max H for AGCMIN
	CAMERA_SetReg(0x13, 0xff);     // Th3 Max L for AGCMIN
	CAMERA_SetReg(0x14, 0xff);     // Th3 Min H for AGCMAX
	CAMERA_SetReg(0x15, 0xff);     // Th3 Min L for AGCMAX
	///////////////////////////////////////////

	// NR Setting
	CAMERA_SetReg(0xfc, 0x01);  
	CAMERA_SetReg(0x4b, 0x01);      // NR Enable
	//s4b00     // NR Enable

	CAMERA_SetReg(0xfc, 0x0b);                             
	CAMERA_SetReg(0x28, 0x00);      //NR Start AGC             
	CAMERA_SetReg(0x29, 0x00);      // SIG Th AGCMIN H   
	CAMERA_SetReg(0x2a, 0x0a);  //14    // SIG Th AGCMIN L   
	CAMERA_SetReg(0x2b, 0x00);      // SIG Th AGCMAX H   
	CAMERA_SetReg(0x2c, 0x0a);  //14    // SIG Th AGCMAX L   
	CAMERA_SetReg(0x2d, 0x00);      // PRE Th AGCMIN H   
	CAMERA_SetReg(0x2e, 0xc0);  //64    // PRE Th AGCMIN L(100d)   
	CAMERA_SetReg(0x2f, 0x01);      // PRE Th AGCMAX H(300d)   
	CAMERA_SetReg(0x30, 0x2c);      // PRE Th AGCMAX L   
	CAMERA_SetReg(0x31, 0x00);      // POST Th AGCMIN H  
	CAMERA_SetReg(0x32, 0xe0);  //64    // POST Th AGCMIN L(100d)  
	CAMERA_SetReg(0x33, 0x01);      // POST Th AGCMAX H(300d)
	CAMERA_SetReg(0x34, 0x2c);      // POST Th AGCMAX L  
	// NR Setting End

	////////////////////////////////
	// Color suppress setting
	CAMERA_SetReg(0xfc, 0x0b);  
	CAMERA_SetReg(0x08, 0x50);      // C suppress AGC MIN
	CAMERA_SetReg(0x09, 0x03);      // C suppress MIN H
	CAMERA_SetReg(0x0a, 0x80);      // C suppress MIN L
	// C Suppress Setting End

	CAMERA_SetReg(0xfc, 0x05);
	CAMERA_SetReg(0x4a, 0x00);  //01    // Edge Color Suppress, 9/13
	///////////////////////////////

	// 1D Y LPF Filter             
	CAMERA_SetReg(0xfc, 0x01);                            
	//s05e0     // Default s60        
	CAMERA_SetReg(0x05, 0x60);      // Default s60        
	//[7]: Y LPF filter On [6]: Clap On

	CAMERA_SetReg(0xfc, 0x0b);                            
	CAMERA_SetReg(0x35, 0x00);      // YLPF Start AGC      
	CAMERA_SetReg(0x36, 0x50);      // YLPF01 AGCMIN       
	CAMERA_SetReg(0x37, 0x50);      // YLPF01 AGCMAX       
	CAMERA_SetReg(0x38, 0x00);      // YLPF SIG01 Th AGCMINH
	CAMERA_SetReg(0x39, 0x90);  //00    // YLPF SIG01 Th AGCMINL   
	CAMERA_SetReg(0x3a, 0x01);   // YLPF SIG01 Th AGCMAXH   
	CAMERA_SetReg(0x3b, 0xa0);   // YLPF SIG01 Th AGCMAXL               
	CAMERA_SetReg(0x3c, 0x50);      // YLPF02 AGCMIN           
	CAMERA_SetReg(0x3d, 0x50);   // YLPF02 AGCMAX           
	CAMERA_SetReg(0x3e, 0x00);   // YLPF SIG02 Th AGCMINH   
	CAMERA_SetReg(0x3f, 0xa0);  //00   // YLPF SIG02 Th AGCMINL   
	CAMERA_SetReg(0x40, 0x01);   // YLPF SIG02 Th AGCMAXH   s73
	CAMERA_SetReg(0x41, 0xb0);   // YLPF SIG02 Th AGCMAXL   
	// Y LPF Filter setting End 

	// SET EDGE COLOR SUPPRESS AND Y-LPF(엄 책임님 mail 추가)************************************ 
	CAMERA_SetReg(0xfc, 0x05);
	CAMERA_SetReg(0x42, 0x1F);
	CAMERA_SetReg(0x43, 0x1F);
	CAMERA_SetReg(0x44, 0x0E);
	CAMERA_SetReg(0x45, 0x8C);    //더 suppres하고자하면 5a, 0x 그 이하는 side effect 때문에 안됨.
	CAMERA_SetReg(0x46, 0x7A);
	CAMERA_SetReg(0x47, 0x60);
	CAMERA_SetReg(0x48, 0x0C);
	CAMERA_SetReg(0x49, 0x39);
	CAMERA_SetReg(0x4A, 0x01);
	CAMERA_SetReg(0x4B, 0xB1);
	CAMERA_SetReg(0x4C, 0x3B);
	CAMERA_SetReg(0x4D, 0x14);
	//*******************************************************************************************  
	///////////////////////////////////////////
	// NR Setting
	CAMERA_SetReg(0xfc, 0x01);  
	CAMERA_SetReg(0x4b, 0x01);      // NR Enable
	// Set multipliers (which are not suppressed)_(엄 책임님 mail 추가)**************************
	CAMERA_SetReg(0xfc, 0x01);
	CAMERA_SetReg(0x48, 0x11);
	// Suppressed parameters

	CAMERA_SetReg(0xfc, 0x0B);
	CAMERA_SetReg(0x21, 0x00);
	CAMERA_SetReg(0x22, 0x10);
	CAMERA_SetReg(0x23, 0x60);
	CAMERA_SetReg(0x24, 0x10);
	CAMERA_SetReg(0x25, 0x28);
	CAMERA_SetReg(0x26, 0x08);
	CAMERA_SetReg(0x27, 0x20);
	CAMERA_SetReg(0x28, 0x00);    //NR Start AGC                    
	CAMERA_SetReg(0x29, 0x00);    // SIG Th AGCMIN H              
	CAMERA_SetReg(0x2A, 0x02);    // SIG Th AGCMIN L      
	CAMERA_SetReg(0x2B, 0x00);    // SIG Th AGCMAX H              
	CAMERA_SetReg(0x2C, 0x14);    // SIG Th AGCMAX L      
	CAMERA_SetReg(0x2D, 0x03);    // PRE Th AGCMIN H              
	CAMERA_SetReg(0x2E, 0x84);    // PRE Th AGCMIN L
	CAMERA_SetReg(0x2F, 0x03);    // PRE Th AGCMAX H        
	CAMERA_SetReg(0x30, 0x84);    // PRE Th AGCMAX L              
	CAMERA_SetReg(0x31, 0x00);    // POST Th AGCMIN H             
	CAMERA_SetReg(0x32, 0x00);    // POST Th AGCMIN L
	CAMERA_SetReg(0x33, 0x00);    // POST Th AGCMAX H      
	CAMERA_SetReg(0x34, 0xC8);    // POST Th AGCMAX L             
	CAMERA_SetReg(0x35, 0x00);  //1D Y filter setting 
	CAMERA_SetReg(0x36, 0x10);
	CAMERA_SetReg(0x37, 0x50);
	CAMERA_SetReg(0x38, 0x00);
	CAMERA_SetReg(0x39, 0x14);
	CAMERA_SetReg(0x3A, 0x00);
	CAMERA_SetReg(0x3B, 0x50);
	CAMERA_SetReg(0x3C, 0x10);
	CAMERA_SetReg(0x3D, 0x50);
	CAMERA_SetReg(0x3E, 0x00);
	CAMERA_SetReg(0x3F, 0x28);
	CAMERA_SetReg(0x40, 0x00);
	CAMERA_SetReg(0x41, 0xA0);
	//*******************************************************************************************

	//엄 책임님 mail 추가 ***********************************************************************
	// To avoid AWB tracking @ max AGC gain even though AE is unstable state
	CAMERA_SetReg(0xfc, 0x00);
	CAMERA_SetReg(0xba, 0x50);   // AE Target minus AE Average
	CAMERA_SetReg(0xbb, 0x00);
	CAMERA_SetReg(0xbc, 0x00);
	//*******************************************************************************************

	//3. AE weight & etc linear
	// AE Window Weight linear(EVT1)0929   
	CAMERA_SetReg(0xfc, 0x20);  // upper window weight zero
	CAMERA_SetReg(0x60, 0x11);
	CAMERA_SetReg(0x61, 0x11);
	CAMERA_SetReg(0x62, 0x11);
	CAMERA_SetReg(0x63, 0x11);
	CAMERA_SetReg(0x64, 0x11);
	CAMERA_SetReg(0x65, 0x11);
	CAMERA_SetReg(0x66, 0x11);
	CAMERA_SetReg(0x67, 0x11);
	CAMERA_SetReg(0x68, 0x11);
	CAMERA_SetReg(0x69, 0x11);
	CAMERA_SetReg(0x6a, 0x11);
	CAMERA_SetReg(0x6b, 0x11);
	CAMERA_SetReg(0x6c, 0x11);
	CAMERA_SetReg(0x6d, 0x11);
	CAMERA_SetReg(0x6e, 0x11);
	CAMERA_SetReg(0x6f, 0x11);
	CAMERA_SetReg(0x70, 0x11);
	CAMERA_SetReg(0x71, 0x11);
	CAMERA_SetReg(0x72, 0x11);
	CAMERA_SetReg(0x73, 0x11);
	CAMERA_SetReg(0x74, 0x11);
	CAMERA_SetReg(0x75, 0x11);
	CAMERA_SetReg(0x76, 0x11);
	CAMERA_SetReg(0x77, 0x11);
	CAMERA_SetReg(0x78, 0x11);
	CAMERA_SetReg(0x79, 0x11);
	CAMERA_SetReg(0x7a, 0x11);
	CAMERA_SetReg(0x7b, 0x11);
	CAMERA_SetReg(0x7c, 0x11);
	CAMERA_SetReg(0x7d, 0x11);
	CAMERA_SetReg(0x7e, 0x11);
	CAMERA_SetReg(0x7f, 0x11);

	// AE window Weight setting End
	//hue gain linear //
	CAMERA_SetReg(0xfc, 0x00);                          
	CAMERA_SetReg(0x48, 0x40);     
	CAMERA_SetReg(0x49, 0x40);   
	CAMERA_SetReg(0x4a, 0x00);     
	CAMERA_SetReg(0x4b, 0x00);     
	CAMERA_SetReg(0x4c, 0x40);     
	CAMERA_SetReg(0x4d, 0x40);     
	CAMERA_SetReg(0x4e, 0x00);     
	CAMERA_SetReg(0x4f, 0x00);     
	CAMERA_SetReg(0x50, 0x40);     
	CAMERA_SetReg(0x51, 0x40);     
	CAMERA_SetReg(0x52, 0x00);     
	CAMERA_SetReg(0x53, 0x00);     
	CAMERA_SetReg(0x54, 0x40);     
	CAMERA_SetReg(0x55, 0x40);     
	CAMERA_SetReg(0x56, 0x00);     
	CAMERA_SetReg(0x57, 0x00);     
	CAMERA_SetReg(0x58, 0x40);     
	CAMERA_SetReg(0x59, 0x40);     
	CAMERA_SetReg(0x5a, 0x00);     
	CAMERA_SetReg(0x5b, 0x00);    
	CAMERA_SetReg(0x5c, 0x40);     
	CAMERA_SetReg(0x5d, 0x40);     
	CAMERA_SetReg(0x5e, 0x00);     
	CAMERA_SetReg(0x5f, 0x00); 
	CAMERA_SetReg(0x62, 0x00);  //hue enable OFF

	//4. shading (Flex향 3000K manual shading) 
	CAMERA_SetReg(0xfc, 0x09);
	// DSP9_SH_WIDTH_H 
	CAMERA_SetReg(0x01, 0x06);
	CAMERA_SetReg(0x02, 0x40);
	// DSP9_SH_HEIGHT_H 
	CAMERA_SetReg(0x03, 0x04);
	CAMERA_SetReg(0x04, 0xB0);
	CAMERA_SetReg(0x05, 0x03);
	CAMERA_SetReg(0x06, 0x13);
	CAMERA_SetReg(0x07, 0x02);
	CAMERA_SetReg(0x08, 0x5A);
	CAMERA_SetReg(0x09, 0x03);
	CAMERA_SetReg(0x0A, 0x15);
	CAMERA_SetReg(0x0B, 0x02);
	CAMERA_SetReg(0x0C, 0x5B);
	CAMERA_SetReg(0x0D, 0x03);
	CAMERA_SetReg(0x0E, 0x0D);
	CAMERA_SetReg(0x0F, 0x02);
	CAMERA_SetReg(0x10, 0x5D);
	CAMERA_SetReg(0x1D, 0x80);
	CAMERA_SetReg(0x1E, 0x00);
	CAMERA_SetReg(0x1F, 0x80);
	CAMERA_SetReg(0x20, 0x00);
	CAMERA_SetReg(0x23, 0x80);
	CAMERA_SetReg(0x24, 0x00);
	CAMERA_SetReg(0x21, 0x80);
	CAMERA_SetReg(0x22, 0x00);
	CAMERA_SetReg(0x25, 0x80);
	CAMERA_SetReg(0x26, 0x00);
	CAMERA_SetReg(0x27, 0x80);
	CAMERA_SetReg(0x28, 0x00);
	CAMERA_SetReg(0x2B, 0x80);
	CAMERA_SetReg(0x2C, 0x00);
	CAMERA_SetReg(0x29, 0x80);
	CAMERA_SetReg(0x2A, 0x00);
	CAMERA_SetReg(0x2D, 0x80);
	CAMERA_SetReg(0x2E, 0x00);
	CAMERA_SetReg(0x2F, 0x80);
	CAMERA_SetReg(0x30, 0x00);
	CAMERA_SetReg(0x33, 0x80);
	CAMERA_SetReg(0x34, 0x00);
	CAMERA_SetReg(0x31, 0x80);
	CAMERA_SetReg(0x32, 0x00);
	// DSP9_SH_VAL_R0H 
	CAMERA_SetReg(0x35, 0x01);
	CAMERA_SetReg(0x36, 0x00);
	CAMERA_SetReg(0x37, 0x01);
	CAMERA_SetReg(0x38, 0x0F);
	CAMERA_SetReg(0x39, 0x01);
	CAMERA_SetReg(0x3A, 0x42);
	CAMERA_SetReg(0x3B, 0x01);
	CAMERA_SetReg(0x3C, 0x9C);
	CAMERA_SetReg(0x3D, 0x01);
	CAMERA_SetReg(0x3E, 0xD0);
	CAMERA_SetReg(0x3F, 0x02);
	CAMERA_SetReg(0x40, 0x0F);
	CAMERA_SetReg(0x41, 0x02);
	CAMERA_SetReg(0x42, 0x3D);
	CAMERA_SetReg(0x43, 0x02);
	CAMERA_SetReg(0x44, 0x5E);
	CAMERA_SetReg(0x45, 0x01);
	CAMERA_SetReg(0x46, 0x00);
	CAMERA_SetReg(0x47, 0x01);
	CAMERA_SetReg(0x48, 0x0A);
	CAMERA_SetReg(0x49, 0x01);
	CAMERA_SetReg(0x4A, 0x2E);
	CAMERA_SetReg(0x4B, 0x01);
	CAMERA_SetReg(0x4C, 0x66);
	CAMERA_SetReg(0x4D, 0x01);
	CAMERA_SetReg(0x4E, 0x89);
	CAMERA_SetReg(0x4F, 0x01);
	CAMERA_SetReg(0x50, 0xB7);
	CAMERA_SetReg(0x51, 0x01);
	CAMERA_SetReg(0x52, 0xD8);
	CAMERA_SetReg(0x53, 0x01);
	CAMERA_SetReg(0x54, 0xFA);
	// DS9_SH_VAL_B0H
	CAMERA_SetReg(0x55, 0x01);
	CAMERA_SetReg(0x56, 0x00);
	CAMERA_SetReg(0x57, 0x01);
	CAMERA_SetReg(0x58, 0x0A);
	CAMERA_SetReg(0x59, 0x01);
	CAMERA_SetReg(0x5A, 0x28);
	CAMERA_SetReg(0x5B, 0x01);
	CAMERA_SetReg(0x5C, 0x59);
	CAMERA_SetReg(0x5D, 0x01);
	CAMERA_SetReg(0x5E, 0x7A);
	CAMERA_SetReg(0x5F, 0x01);
	CAMERA_SetReg(0x60, 0xA1);
	CAMERA_SetReg(0x61, 0x01);
	CAMERA_SetReg(0x62, 0xC0);
	CAMERA_SetReg(0x63, 0x01);
	CAMERA_SetReg(0x64, 0xDC);
	// DSP9_SH_M_R2_R1H 
	CAMERA_SetReg(0x65, 0x00);
	CAMERA_SetReg(0x66, 0x9F);
	CAMERA_SetReg(0x67, 0xE6);
	CAMERA_SetReg(0x68, 0x02);
	CAMERA_SetReg(0x69, 0x7F);
	CAMERA_SetReg(0x6A, 0x9B);
	CAMERA_SetReg(0x6B, 0x05);
	CAMERA_SetReg(0x6C, 0x9F);
	CAMERA_SetReg(0x6D, 0x1E);
	CAMERA_SetReg(0x6E, 0x07);
	CAMERA_SetReg(0x6F, 0xA6);
	CAMERA_SetReg(0x70, 0xCC);
	CAMERA_SetReg(0x71, 0x09);
	CAMERA_SetReg(0x72, 0xFE);
	CAMERA_SetReg(0x73, 0x6E);
	CAMERA_SetReg(0x74, 0x0C);
	CAMERA_SetReg(0x75, 0xA6);
	CAMERA_SetReg(0x76, 0x04);
	CAMERA_SetReg(0x77, 0x0F);
	CAMERA_SetReg(0x78, 0x9D);
	CAMERA_SetReg(0x79, 0x8C);
	// DSP9_SH_M_R2_G1H 
	CAMERA_SetReg(0x7A, 0x00);
	CAMERA_SetReg(0x7B, 0x9F);
	CAMERA_SetReg(0x7C, 0x95);
	CAMERA_SetReg(0x7D, 0x02);
	CAMERA_SetReg(0x7E, 0x7E);
	CAMERA_SetReg(0x7F, 0x54);
	CAMERA_SetReg(0x80, 0x05);
	CAMERA_SetReg(0x81, 0x9C);
	CAMERA_SetReg(0x82, 0x3E);
	CAMERA_SetReg(0x83, 0x07);
	CAMERA_SetReg(0x84, 0xA2);
	CAMERA_SetReg(0x85, 0xE3);
	CAMERA_SetReg(0x86, 0x09);
	CAMERA_SetReg(0x87, 0xF9);
	CAMERA_SetReg(0x88, 0x53);
	CAMERA_SetReg(0x89, 0x0C);
	CAMERA_SetReg(0x8A, 0x9F);
	CAMERA_SetReg(0x8B, 0x8D);
	CAMERA_SetReg(0x8C, 0x0F);
	CAMERA_SetReg(0x8D, 0x95);
	CAMERA_SetReg(0x8E, 0x91);
	// DSP9_SH_M_R2_B1H 
	CAMERA_SetReg(0x8F, 0x00);
	CAMERA_SetReg(0x90, 0xA1);
	CAMERA_SetReg(0x91, 0xFF);
	CAMERA_SetReg(0x92, 0x02);
	CAMERA_SetReg(0x93, 0x87);
	CAMERA_SetReg(0x94, 0xFD);
	CAMERA_SetReg(0x95, 0x05);
	CAMERA_SetReg(0x96, 0xB1);
	CAMERA_SetReg(0x97, 0xFA);
	CAMERA_SetReg(0x98, 0x07);
	CAMERA_SetReg(0x99, 0xC0);
	CAMERA_SetReg(0x9A, 0x79);
	CAMERA_SetReg(0x9B, 0x0A);
	CAMERA_SetReg(0x9C, 0x1F);
	CAMERA_SetReg(0x9D, 0xF6);
	CAMERA_SetReg(0x9E, 0x0C);
	CAMERA_SetReg(0x9F, 0xD0);
	CAMERA_SetReg(0xA0, 0x74);
	CAMERA_SetReg(0xA1, 0x0F);
	CAMERA_SetReg(0xA2, 0xD1);
	CAMERA_SetReg(0xA3, 0xF1);
	// DSP9_SH_SUB_RR0H 
	CAMERA_SetReg(0xA4, 0x66);
	CAMERA_SetReg(0xA5, 0x76);
	CAMERA_SetReg(0xA6, 0x22);
	CAMERA_SetReg(0xA7, 0x27);
	CAMERA_SetReg(0xA8, 0x14);
	CAMERA_SetReg(0xA9, 0x7E);
	CAMERA_SetReg(0xAA, 0x1F);
	CAMERA_SetReg(0xAB, 0x86);
	CAMERA_SetReg(0xAC, 0x1B);
	CAMERA_SetReg(0xAD, 0x52);
	CAMERA_SetReg(0xAE, 0x18);
	CAMERA_SetReg(0xAF, 0x1B);
	CAMERA_SetReg(0xB0, 0x15);
	CAMERA_SetReg(0xB1, 0x92);
	// DSP9_SH_SUB_RG0H 
	CAMERA_SetReg(0xB2, 0x66);
	CAMERA_SetReg(0xB3, 0xAA);
	CAMERA_SetReg(0xB4, 0x22);
	CAMERA_SetReg(0xB5, 0x38);
	CAMERA_SetReg(0xB6, 0x14);
	CAMERA_SetReg(0xB7, 0x88);
	CAMERA_SetReg(0xB8, 0x1F);
	CAMERA_SetReg(0xB9, 0x97);
	CAMERA_SetReg(0xBA, 0x1B);
	CAMERA_SetReg(0xBB, 0x60);
	CAMERA_SetReg(0xBC, 0x18);
	CAMERA_SetReg(0xBD, 0x28);
	CAMERA_SetReg(0xBE, 0x15);
	CAMERA_SetReg(0xBF, 0x9D);
	// DSP9_SH_SUB_RB0H 
	CAMERA_SetReg(0xC0, 0x65);
	CAMERA_SetReg(0xC1, 0x23);
	CAMERA_SetReg(0xC2, 0x21);
	CAMERA_SetReg(0xC3, 0xB6);
	CAMERA_SetReg(0xC4, 0x14);
	CAMERA_SetReg(0xC5, 0x3A);
	CAMERA_SetReg(0xC6, 0x1F);
	CAMERA_SetReg(0xC7, 0x1E);
	CAMERA_SetReg(0xC8, 0x1A);
	CAMERA_SetReg(0xC9, 0xF8);
	CAMERA_SetReg(0xCA, 0x17);
	CAMERA_SetReg(0xCB, 0xCC);
	CAMERA_SetReg(0xCC, 0x15);
	CAMERA_SetReg(0xCD, 0x4A);
	CAMERA_SetReg(0x00, 0x02);  // shading on

	CAMERA_SetReg(0xfc, 0x00);
	CAMERA_SetReg(0x79, 0xf4);
	CAMERA_SetReg(0x7a, 0x09);

	//5.color correction
	//1229 CCM
	//2.0251    -1.0203 -0.0048 
	//-0.7080   1.8970  -0.1889 
	//-0.468    -0.444  1.912
	CAMERA_SetReg(0xfc, 0x01);
	CAMERA_SetReg(0x51, 0x08);  //R
	CAMERA_SetReg(0x52, 0x18);
	CAMERA_SetReg(0x53, 0xfb);
	CAMERA_SetReg(0x54, 0xec);
	CAMERA_SetReg(0x55, 0xff);
	CAMERA_SetReg(0x56, 0xfc);
	CAMERA_SetReg(0x57, 0xfd);  //G
	CAMERA_SetReg(0x58, 0x2c);
	CAMERA_SetReg(0x59, 0x07);
	CAMERA_SetReg(0x5a, 0x95);
	CAMERA_SetReg(0x5b, 0xff);
	CAMERA_SetReg(0x5c, 0x3f);
	CAMERA_SetReg(0x5d, 0xfe);  //B
	CAMERA_SetReg(0x5e, 0x22);  
	CAMERA_SetReg(0x5f, 0xfe);  
	CAMERA_SetReg(0x60, 0x3a);  
	CAMERA_SetReg(0x61, 0x07);  
	CAMERA_SetReg(0x62, 0xa5);  

	//6.gamma 
	//Gamma 
	CAMERA_SetReg(0xfc, 0x01);           
	// R                                               
	CAMERA_SetReg(0x6F, 0x05);  
	CAMERA_SetReg(0x70, 0x14);  
	CAMERA_SetReg(0x71, 0x3c);  
	CAMERA_SetReg(0x72, 0x96);  
	CAMERA_SetReg(0x73, 0x00);  
	CAMERA_SetReg(0x74, 0x2c);  
	CAMERA_SetReg(0x75, 0xa2);  
	CAMERA_SetReg(0x76, 0xfc);  
	CAMERA_SetReg(0x77, 0x44);  
	CAMERA_SetReg(0x78, 0x56);  
	CAMERA_SetReg(0x79, 0x80);  
	CAMERA_SetReg(0x7A, 0xb7);  
	CAMERA_SetReg(0x7B, 0xed);
	CAMERA_SetReg(0x7C, 0x16);
	CAMERA_SetReg(0x7D, 0xab);
	CAMERA_SetReg(0x7E, 0x3c);
	CAMERA_SetReg(0x7F, 0x61);
	CAMERA_SetReg(0x80, 0x83);
	CAMERA_SetReg(0x81, 0xa4);                            
	CAMERA_SetReg(0x82, 0xff);                              
	CAMERA_SetReg(0x83, 0xc4);                              
	CAMERA_SetReg(0x84, 0xe2);                            
	CAMERA_SetReg(0x85, 0xff);                              
	CAMERA_SetReg(0x86, 0xff);                              
	// G                                  
	CAMERA_SetReg(0x87, 0x05);  
	CAMERA_SetReg(0x88, 0x14);  
	CAMERA_SetReg(0x89, 0x3c);        
	CAMERA_SetReg(0x8A, 0x96);  
	CAMERA_SetReg(0x8B, 0x00);  
	CAMERA_SetReg(0x8C, 0x2c);  
	CAMERA_SetReg(0x8D, 0xa2);  
	CAMERA_SetReg(0x8E, 0xfc);  
	CAMERA_SetReg(0x8F, 0x44);  
	CAMERA_SetReg(0x90, 0x56);  
	CAMERA_SetReg(0x91, 0x80);  
	CAMERA_SetReg(0x92, 0xb7);  
	CAMERA_SetReg(0x93, 0xed);        
	CAMERA_SetReg(0x94, 0x16);          
	CAMERA_SetReg(0x95, 0xab);          
	CAMERA_SetReg(0x96, 0x3c);               
	CAMERA_SetReg(0x97, 0x61);               
	CAMERA_SetReg(0x98, 0x83);              
	CAMERA_SetReg(0x99, 0xa4);               
	CAMERA_SetReg(0x9A, 0xff);        
	CAMERA_SetReg(0x9B, 0xc4);          
	CAMERA_SetReg(0x9C, 0xe2);          
	CAMERA_SetReg(0x9D, 0xff);          
	CAMERA_SetReg(0x9E, 0xff);          
	//B      
	CAMERA_SetReg(0x9F, 0x05);  
	CAMERA_SetReg(0xA0, 0x10);  
	CAMERA_SetReg(0xA1, 0x30);  
	CAMERA_SetReg(0xA2, 0x70);  
	CAMERA_SetReg(0xA3, 0x00);  
	CAMERA_SetReg(0xA4, 0x2c);  
	CAMERA_SetReg(0xA5, 0xa2);  
	CAMERA_SetReg(0xA6, 0xfc);  
	CAMERA_SetReg(0xA7, 0x44);       
	CAMERA_SetReg(0xA8, 0x56);       
	                
	CAMERA_SetReg(0xA9, 0x80);       
	CAMERA_SetReg(0xAA, 0xb7);       
	CAMERA_SetReg(0xAB, 0xed);  
	CAMERA_SetReg(0xAC, 0x16);  
	CAMERA_SetReg(0xAD, 0xab);  
	        
	CAMERA_SetReg(0xAE, 0x3c);  
	CAMERA_SetReg(0xAF, 0x61);  
	CAMERA_SetReg(0xB0, 0x83);  
	CAMERA_SetReg(0xB1, 0xa4);  
	CAMERA_SetReg(0xB2, 0xff);  
	        
	CAMERA_SetReg(0xB3, 0xc4);  
	CAMERA_SetReg(0xB4, 0xe2);  
	CAMERA_SetReg(0xB5, 0xff);  
	CAMERA_SetReg(0xB6, 0xff);

	//7.hue 
	CAMERA_SetReg(0xFC, 0x00);
	CAMERA_SetReg(0x62, 0x00);    // hue auto control off

	CAMERA_SetReg(0xFC, 0x05);
	CAMERA_SetReg(0x4E, 0x60);
	CAMERA_SetReg(0x4F, 0xA0);
	CAMERA_SetReg(0x50, 0x35);
	CAMERA_SetReg(0x51, 0xA0);
	CAMERA_SetReg(0x52, 0x20);
	CAMERA_SetReg(0x53, 0x01);
	CAMERA_SetReg(0x54, 0xE0);
	CAMERA_SetReg(0x55, 0xE0);
	CAMERA_SetReg(0x56, 0x54);
	CAMERA_SetReg(0x57, 0x20);
	CAMERA_SetReg(0x58, 0x20);
	CAMERA_SetReg(0x59, 0xF0);

	//8.white point 
	//AWB Start Point
	CAMERA_SetReg(0xfc, 0x07);
	CAMERA_SetReg(0x05, 0x00);
	CAMERA_SetReg(0x06, 0x08);
	CAMERA_SetReg(0x07, 0x1b);
	CAMERA_SetReg(0x08, 0xf0);
	CAMERA_SetReg(0x09, 0x00);  // R
	CAMERA_SetReg(0x0a, 0xa8);  
	CAMERA_SetReg(0x0b, 0x00);  // B
	CAMERA_SetReg(0x0c, 0xb0);
	CAMERA_SetReg(0x0d, 0x00);  // G
	CAMERA_SetReg(0x0e, 0x40);

	CAMERA_SetReg(0xfc, 0x00);
	CAMERA_SetReg(0x70, 0x02);

	CAMERA_SetReg(0x40, 0x8a);  //2000K
	CAMERA_SetReg(0x41, 0xe5);  
	CAMERA_SetReg(0x42, 0x95);  //3100K
	CAMERA_SetReg(0x43, 0xba);  
	CAMERA_SetReg(0x44, 0xbc);  //5100K
	CAMERA_SetReg(0x45, 0x99);        

	CAMERA_SetReg(0x34, 0x24);
	CAMERA_SetReg(0x35, 0x10);
	CAMERA_SetReg(0x36, 0x13);
	CAMERA_SetReg(0x37, 0x04);
	CAMERA_SetReg(0x38, 0x10);
	CAMERA_SetReg(0x39, 0x28);
	CAMERA_SetReg(0x3a, 0x1e);
	CAMERA_SetReg(0x3b, 0x2a);

	CAMERA_SetReg(0x31, 0x00);  // skin tone[6], CW delete[5]  

//	CAMERA_SetReg(0xfc, 0x03);
//	CAMERA_SetReg(0x2d, 0x01);

	CAMERA_SetSizeInS5K3BAF(eSub);

//	CAMERA_SetReg(0xfc, 0x00);
//	CAMERA_SetReg(0x02, 0x09);

//	CAMERA_SetReg(0xfc, 0x04);
//	CAMERA_SetReg(0x2d, 0x01);
   
	//sfc07                                     
	//s521f //R center(CW)                    
	//s539f //B center (5 bit,signed)         
	//s540f //0c   //radius 
	//                         
	//s551b //08    // R center(skin tone)            
	//s569f //86    // B center                       
	//s574f //ff   // width and depth                
	//////////////////////////////////////                                          
	//new 
	CAMERA_IICClose();

}

//////////
// Function Name : CAMERA_SetSizeInS5K3BAF
// Function Description : camera module(S5K3BA) Size calucate
// Input : 	IMG_SIZE eSrcSize(Source size)
// Output :    	None
void CAMERA_SetSizeInS5K3BAF(SUB_SAMPLING eSub)
{
	u8 ucSub;

	oCamera.m_ucSlaveAddr = 0x5a;
	/// Size subsampling setting
	ucSub = (eSub == SUB_SAMPLING0) ? 0 :
				(eSub == SUB_SAMPLING2) ? 9 :
				(eSub == SUB_SAMPLING4) ? 11 :
				0xff;
	Assert(ucSub < 0xff);
	CAMERA_SetReg(0xfc, 0x00);
	CAMERA_SetReg(0x02, ucSub);
}


void CAMERA_InitA3AFX_QVGA_20FR(void)
{
	//u32 uHCLKx2;

	SYSC_GetClkInform(); // Get HCLKx2 Frequency
	//uHCLKx2 = g_HCLKx2/1000000;
	
	oCamera.m_ucSlaveAddr = 0xc4;

// 	 CAMERA_IICOpen(100000); 
 	 CAMERA_IICOpen(16000000); 


	//3.13 Pearl setting으로 시작.

	//------------------------------------------------------------------
	// Set File 
	// - S5KA3AFX_xshade_061018.xls generates
	// - date : 2007-03-12오후 3:20:55
	//------------------------------------------------------------------

	CAMERA_SetReg(0xef,0x01);
	CAMERA_SetReg(0x70,0x00); //AE,AWB OFF

	//QVGA
	CAMERA_SetReg(0xef,0x00);
	CAMERA_SetReg(0x7c,0xe0);
	CAMERA_SetReg(0x81,0x01);
	CAMERA_SetReg(0x82,0x50);
	CAMERA_SetReg(0x7f,0x00);
	CAMERA_SetReg(0x80,0xf0);
	CAMERA_SetReg(0xc2,0x09);
	CAMERA_SetReg(0xc6,0x01);
	CAMERA_SetReg(0xc7,0x40);
	CAMERA_SetReg(0xc4,0x00);
	CAMERA_SetReg(0xc5,0xF0);

	//// Page : 00
	//CAMERA_SetReg(0xef,0x00);
	//CAMERA_SetReg(0x0d,0x03);
	//CAMERA_SetReg(0x0e,0x00);
	//CAMERA_SetReg(0x0f,0x1F);
	//CAMERA_SetReg(0x10,0x00);
	//CAMERA_SetReg(0x11,0x37);
	//CAMERA_SetReg(0x12,0x08);
	//CAMERA_SetReg(0x13,0x42);
	//CAMERA_SetReg(0x14,0x00);
	//CAMERA_SetReg(0x15,0x12);
	//CAMERA_SetReg(0x16,0x9E);
	//CAMERA_SetReg(0x19,0x00);
	//CAMERA_SetReg(0x17,0x07);
	//CAMERA_SetReg(0x18,0xE0);
	//CAMERA_SetReg(0x19,0x01);
	//CAMERA_SetReg(0x17,0x07);
	//CAMERA_SetReg(0x18,0xE3);
	//CAMERA_SetReg(0x19,0x02);
	//CAMERA_SetReg(0x17,0x07);
	//CAMERA_SetReg(0x18,0xED);
	//CAMERA_SetReg(0x19,0x03);
	//CAMERA_SetReg(0x17,0x07);
	//CAMERA_SetReg(0x18,0xCA);
	//CAMERA_SetReg(0x19,0x04);
	//CAMERA_SetReg(0x17,0x07);
	//CAMERA_SetReg(0x18,0xD2);
	//CAMERA_SetReg(0x19,0x05);
	//CAMERA_SetReg(0x17,0x07);
	//CAMERA_SetReg(0x18,0xEC);
	//CAMERA_SetReg(0x19,0x06);
	//CAMERA_SetReg(0x17,0x07);
	//CAMERA_SetReg(0x18,0xE5);
	//CAMERA_SetReg(0x19,0x07);
	//CAMERA_SetReg(0x17,0x00);
	//CAMERA_SetReg(0x18,0x12);
	//CAMERA_SetReg(0x19,0x08);
	//CAMERA_SetReg(0x17,0x07);
	//CAMERA_SetReg(0x18,0xE6);
	//CAMERA_SetReg(0x19,0x09);
	//CAMERA_SetReg(0x17,0x00);
	//CAMERA_SetReg(0x18,0x08);
	//CAMERA_SetReg(0x19,0x0A);
	//CAMERA_SetReg(0x17,0x07);
	//CAMERA_SetReg(0x18,0xEA);
	//CAMERA_SetReg(0x19,0x0B);
	//CAMERA_SetReg(0x17,0x07);
	//CAMERA_SetReg(0x18,0xDA);
	//CAMERA_SetReg(0x19,0x0C);
	//CAMERA_SetReg(0x17,0x07);
	//CAMERA_SetReg(0x18,0xE3);
	//CAMERA_SetReg(0x19,0x0D);
	//CAMERA_SetReg(0x17,0x07);
	//CAMERA_SetReg(0x18,0xF9);
	//CAMERA_SetReg(0x19,0x0E);
	//CAMERA_SetReg(0x17,0x00);
	//CAMERA_SetReg(0x18,0x1C);
	//CAMERA_SetReg(0x19,0x0F);
	//CAMERA_SetReg(0x17,0x00);
	//CAMERA_SetReg(0x18,0x12);
	//CAMERA_SetReg(0x19,0x10);
	//CAMERA_SetReg(0x17,0x00);
	//CAMERA_SetReg(0x18,0x18);
	//CAMERA_SetReg(0x19,0x11);
	//CAMERA_SetReg(0x17,0x00);
	//CAMERA_SetReg(0x18,0x08);
	//CAMERA_SetReg(0x19,0x12);
	//CAMERA_SetReg(0x17,0x07);
	//CAMERA_SetReg(0x18,0xEB);
	//CAMERA_SetReg(0x19,0x13);
	//CAMERA_SetReg(0x17,0x07);
	//CAMERA_SetReg(0x18,0xF9);
	//CAMERA_SetReg(0x19,0x14);
	//CAMERA_SetReg(0x17,0x07);
	//CAMERA_SetReg(0x18,0xF4);
	//CAMERA_SetReg(0x19,0x15);
	//CAMERA_SetReg(0x17,0x00);
	//CAMERA_SetReg(0x18,0x1F);
	//CAMERA_SetReg(0x19,0x16);
	//CAMERA_SetReg(0x17,0x00);
	//CAMERA_SetReg(0x18,0x15);
	//CAMERA_SetReg(0x19,0x17);
	//CAMERA_SetReg(0x17,0x00);
	//CAMERA_SetReg(0x18,0x11);
	//CAMERA_SetReg(0x19,0x18);
	//CAMERA_SetReg(0x17,0x00);
	//CAMERA_SetReg(0x18,0x0E);
	//CAMERA_SetReg(0x19,0x19);
	//CAMERA_SetReg(0x17,0x00);
	//CAMERA_SetReg(0x18,0x08);
	//CAMERA_SetReg(0x19,0x1A);
	//CAMERA_SetReg(0x17,0x07);
	//CAMERA_SetReg(0x18,0xFE);
	//CAMERA_SetReg(0x19,0x1B);
	//CAMERA_SetReg(0x17,0x07);
	//CAMERA_SetReg(0x18,0xF4);
	//CAMERA_SetReg(0x19,0x1C);
	//CAMERA_SetReg(0x17,0x00);
	//CAMERA_SetReg(0x18,0x24);
	//CAMERA_SetReg(0x19,0x1D);
	//CAMERA_SetReg(0x17,0x00);
	//CAMERA_SetReg(0x18,0x03);
	//CAMERA_SetReg(0x19,0x1E);
	//CAMERA_SetReg(0x17,0x00);
	//CAMERA_SetReg(0x18,0x02);
	//CAMERA_SetReg(0x19,0x1F);
	//CAMERA_SetReg(0x17,0x07);
	//CAMERA_SetReg(0x18,0xFF);
	//CAMERA_SetReg(0x19,0x20);
	//CAMERA_SetReg(0x17,0x00);
	//CAMERA_SetReg(0x18,0x12);
	//CAMERA_SetReg(0x19,0x21);
	//CAMERA_SetReg(0x17,0x07);
	//CAMERA_SetReg(0x18,0xE5);
	//CAMERA_SetReg(0x19,0x22);
	//CAMERA_SetReg(0x17,0x07);
	//CAMERA_SetReg(0x18,0xD1);
	//CAMERA_SetReg(0x19,0x40);
	//

	//CAMERA_SetReg(0xef,0x02);
	//CAMERA_SetReg(0x59,0x50); //P, S
	//CAMERA_SetReg(0x5a,0x32); //M
	//CAMERA_SetReg(0x58,0x21); //PLL

	//////////////////////////////////////////////////////////

	CAMERA_SetReg(0xef,0x02);   // CIS page
	//CAMERA_SetReg(0x4b,0x5E);   // PCLK driving current max
	//CAMERA_SetReg(0x23,0xd8);   // Tx width
	CAMERA_SetReg(0x3a,0x20);   // double shutter off
	CAMERA_SetReg(0x65,0x08);   // CIS mode
	CAMERA_SetReg(0x0a,0x90);   // Display WCW
	CAMERA_SetReg(0x01,0x26);   // E-shutter enable
	                 
	CAMERA_SetReg(0x03,0x78);   // S1R END						**** Revised at EVT1
	CAMERA_SetReg(0x05,0x00);   // S1S end value control
	CAMERA_SetReg(0x07,0xE5);   // s2  END
	CAMERA_SetReg(0x09,0x0E);   // S4  END
	CAMERA_SetReg(0x0b,0x84);	//85	//81   // Analog offset
	CAMERA_SetReg(0x13,0xa0);   //20   // R_REF
	//CAMERA_SetReg(0x15,0x80);   // EC COMP 100'b
	CAMERA_SetReg(0x1c,0x58);   //20070304AM //57   // global gain						**** Revised at EVT1
	CAMERA_SetReg(0x24,0x3d);   // i_cds						**** Revised at EVT1
	CAMERA_SetReg(0x26,0x25);   // clamp level control					**** Revised at EVT1
	CAMERA_SetReg(0x29,0x8f);   // CFPN start frame (16);
	CAMERA_SetReg(0x4b,0x5b);   // IO driving current
	CAMERA_SetReg(0x56,0x05);   // Line ADLC
	//CAMERA_SetReg(0x58,0x61);   // amp bias current source select (71h : self bias);
	CAMERA_SetReg(0x5f,0x06);   // aps bias current      
	CAMERA_SetReg(0x67,0x3c);   // EC_COMP & SAVG_END
	CAMERA_SetReg(0x68,0x10);   // i_rmp control
	CAMERA_SetReg(0x69,0x13);   // i_mid

	//CAMERA_SetReg(0x58,0x21);	// PLL on
	//CAMERA_SetReg(0x59,0x1A);	// pll_s=0, pll_p=26,
	//CAMERA_SetReg(0x5A,0x1A);	// pll_m=26

	CAMERA_SetReg(0x28,0x9B);   // CFPN on						**** Revised at EVT1




	//061011
	CAMERA_SetReg(0xef,0x01);
	CAMERA_SetReg(0xd1,0xaa);   //89:16.7MHz, d2:24MHz 64:13MHz dc:25MHz 13MHz //d2	//d2(24MHz); Mclk Value (Mclk - 30d);/100,000 
	//CAMERA_SetReg(0xd1,0x64);	//64(13MHz); Mclk Value (Mclk - 30d);/100,000 
	//CAMERA_SetReg(0xd9,0x15);   //AE saturation CHIP_DEBUG

	//////////
	//page 0
	//////////
	CAMERA_SetReg(0xef,0x00);
	//CAMERA_SetReg(0x09,0x3e);   //gr gain

	CAMERA_SetReg(0xef,0x00);
	CAMERA_SetReg(0x27,0x40);  //08  //GR/GB Correction Limitation Value
	CAMERA_SetReg(0x29,0x60);  //GrGb Edge Threshold Value

	// AE Window
	CAMERA_SetReg(0xef,0x00);
	CAMERA_SetReg(0x55,0x00);	//Window diplay off
	CAMERA_SetReg(0x58,0x00);
	CAMERA_SetReg(0x59,0x00);
	CAMERA_SetReg(0x5a,0x02);
	CAMERA_SetReg(0x5b,0x80);
	CAMERA_SetReg(0x5c,0x00);
	CAMERA_SetReg(0x5d,0x00);
	CAMERA_SetReg(0x5e,0x01);
	CAMERA_SetReg(0x5f,0xe0);
	CAMERA_SetReg(0x60,0x00);
	CAMERA_SetReg(0x61,0x00);
	CAMERA_SetReg(0x62,0x02);
	CAMERA_SetReg(0x63,0x80);
	CAMERA_SetReg(0x64,0x00);
	CAMERA_SetReg(0x65,0x00);
	CAMERA_SetReg(0x66,0x01);
	CAMERA_SetReg(0x67,0xe0);

	CAMERA_SetReg(0xef,0x01);	// AE weight
	CAMERA_SetReg(0x77,0x01);
	CAMERA_SetReg(0x78,0x01);
	//CAMERA_SetReg(0x79,0x03);
	//CAMERA_SetReg(0x7a,0x03);
	//CAMERA_SetReg(0x7b,0x04);
	//CAMERA_SetReg(0x7c,0x04);

	CAMERA_SetReg(0xef,0x00);
	//CAMERA_SetReg(0x57,0x18);  // AE Lux min threshold

	CAMERA_SetReg(0xd9,0xc0);  // color edge threshold

	////AWB Luminance check
	////20070212 Added for low light starting env.
	CAMERA_SetReg(0xef,0x00);
	CAMERA_SetReg(0xad,0x03);	//Luminance upper_level[9:8]
	CAMERA_SetReg(0xae,0x20);	//Luminance upper_level[7:0]
	CAMERA_SetReg(0xaf,0x00);	//Luminance lower_level[9:8]
	CAMERA_SetReg(0xb0,0x90);	//Luminance lower_level[7:0]

	/////////////////////////////////////////////////////
	//page 1/////////////////////////////////////////////
	/////////////////////////////////////////////////////
	CAMERA_SetReg(0xef,0x01);
	////20070212 Modified for low light starting env.
	CAMERA_SetReg(0x83,0x72);	//7d	//AWB R/B Max Gain : <0.44>'s max
	CAMERA_SetReg(0x84,0x38);	//30	//AWB R/B Min Gain : <0.42>'s max

	CAMERA_SetReg(0xa8,0x41);   //20070222  //shutter Control
	CAMERA_SetReg(0xa9,0xd4);   //20070222  //Shutter Threshold

	CAMERA_SetReg(0xaf,0x04);	//20070213PM	//00   //L.L Brightness
	CAMERA_SetReg(0xb4,0x88);	//20070213	//b0   //a0   // N.L contrast
	CAMERA_SetReg(0xb5,0x88);	//20070213	//b0   //a0   // L.L contrast 
	CAMERA_SetReg(0xbc,0x66);   //20070305PM //26	//20070213	//AWB skip point (default:16);

	CAMERA_SetReg(0xb7,0x46);   //58   //68   // N.L color saturation
	CAMERA_SetReg(0xb8,0x46);   //20070305PM //30   //52   // L.L color saturation

	//CAMERA_SetReg(0x70,0x97);   //17 // fix PAL(50Hz);
	CAMERA_SetReg(0x71,0x7a); //6a);	//20070213PM	//55   //5a   //68   //70   //6f   //4d	//60	//78   //AE target
	CAMERA_SetReg(0x72,0x09);   //20070214PM  //07   //0c   //12   //0a   //05	//12   // AE threshold
	//CAMERA_SetReg(0x73,0x34);	//20070214PM  //AE Step Guard Value 
	CAMERA_SetReg(0x74,0x6b);   //60   //70	//74	//70   // low agc max
	CAMERA_SetReg(0x75,0x40);   // normal agc max
	  
	CAMERA_SetReg(0xa6,0x40);  //80  //N.L sharpness H/V Gain
	CAMERA_SetReg(0xab,0x40);  //38  //18  //60 //N.L NR Edge 
	CAMERA_SetReg(0xad,0x80);  //40  //20  //L.L NR Edge
	//CAMERA_SetReg(0xa3,0x30); //AWB speed //EVT0은 고정되어 있음.(0x03); //EVT1은 변경 가능.

	CAMERA_SetReg(0xcc,0x09);   //08   //Flicker threshold 12
	CAMERA_SetReg(0xd5,0x22);   //23	//32  //Chip_Debug2

	CAMERA_SetReg(0xe0,0x06);   // NT Max
	CAMERA_SetReg(0xe2,0x06);   //07   //18   // Low NT max
	CAMERA_SetReg(0xe1,0x05);   // PAL Max
	CAMERA_SetReg(0xe3,0x05);   //07   //18   // Low PAL max

	CAMERA_SetReg(0xe4,0x00);   // 24MHz VblankH (H.L); //FPS (24MHz, 15fps);
	CAMERA_SetReg(0xe5,0x77);   //ea   // 24MHz NT VblankL (H.L);
	//CAMERA_SetReg(0xe4,0x00);	// 13MHz VblankH (H.L); //FPS (24MHz, 15fps);
	//CAMERA_SetReg(0xe5,0x2a);	// 13MHz VblankL (H.L);

	CAMERA_SetReg(0xe6,0x00);   //20070215AM // 24MHz VblankH (H.L); //FPS (24MHz, 15fps);
	CAMERA_SetReg(0xe7,0x2a);   //20070215AM //ea   // 24MHz NT VblankL (H.L);

	CAMERA_SetReg(0xe8,0x68);   //44  //84 //97  // ae speed
	CAMERA_SetReg(0xe9,0x84); //Double shutter ON
	//CAMERA_SetReg(0xe9,0x87);
	CAMERA_SetReg(0xef,0x00);
	//CAMERA_SetReg(0x8c,0x0e); //skin color detection
	CAMERA_SetReg(0xc8,0x09);   //0a  //YC delay


	////////////////////////////////////////
	// Color Correction -> Shading -> Gamma -> AWB -> Hue
	////////////////////////////////////////

	// Color Correction
	                   
	//CAMERA_SetReg(0xef,0x00);         //	//D65-1	//D65	//CW	//U30-6	//U30	//Old
	//CAMERA_SetReg(0x4c,0x40);         //95	//95	//81	//8a	//50	//62	//56
	//CAMERA_SetReg(0x4d,0x92);         //c2	//c2	//ba	//d2	//af	//a9	//8d
	//CAMERA_SetReg(0x4e,0x0f);         //92	//92	//87	//07	//1f	//07	//89
	//CAMERA_SetReg(0x4f,0x9a);         //9b	//9b	//98	//9e	//9e	//a2	//94
	//CAMERA_SetReg(0x50,0x3b);         //73	//73	//64	//4c	//2e	//47	//5c
	//CAMERA_SetReg(0x51,0x1e);         //98	//98	//8c	//12	//30	//1b	//8b
	//CAMERA_SetReg(0x52,0x82);         //80	//80	//80	//80	//86	//85	//80
	//CAMERA_SetReg(0x53,0xbf);         //b3	//b3	//ad	//cd	//eb	//f2	//91
	//CAMERA_SetReg(0x54,0x7f);         //73	//73	//6d	//8e	//b2	//b8	//51

	////CW
	CAMERA_SetReg(0xef,0x00);
	CAMERA_SetReg(0x4c,0x92);
	CAMERA_SetReg(0x4d,0xcf);
	CAMERA_SetReg(0x4e,0x82);
	CAMERA_SetReg(0x4f,0xad);
	CAMERA_SetReg(0x50,0x6e);
	CAMERA_SetReg(0x51,0x80);
	CAMERA_SetReg(0x52,0x87);
	CAMERA_SetReg(0x53,0xeb);
	CAMERA_SetReg(0x54,0xb2);
	//
	////D65
	//CAMERA_SetReg(0xef,0x00);
	//CAMERA_SetReg(0x4c,0x8a);
	//CAMERA_SetReg(0x4d,0xc2);
	//CAMERA_SetReg(0x4e,0x88);
	//CAMERA_SetReg(0x4f,0x9f);
	//CAMERA_SetReg(0x50,0x72);
	//CAMERA_SetReg(0x51,0x93);
	//CAMERA_SetReg(0x52,0x81);
	//CAMERA_SetReg(0x53,0xca);
	//CAMERA_SetReg(0x54,0x8c);

	//Default
	//CAMERA_SetReg(0xef,0x00);
	//CAMERA_SetReg(0x4c,0x56);
	//CAMERA_SetReg(0x4d,0x8d);
	//CAMERA_SetReg(0x4e,0x89);
	//CAMERA_SetReg(0x4f,0x96);
	//CAMERA_SetReg(0x50,0x5d);
	//CAMERA_SetReg(0x51,0x88);
	//CAMERA_SetReg(0x52,0x94);
	//CAMERA_SetReg(0x53,0x89);
	//CAMERA_SetReg(0x54,0x5d);

	//// Shading2   //// RGB Shading// RGB Shading// RGB Shading
	//              //              //              	      //linear//old
	CAMERA_SetReg(0xef,0x00);         //sef00         //sef00         //00    //00	//00	//00
	CAMERA_SetReg(0x30,0x01);         //s3001         //s3001         //01    //01	//01	//01
	CAMERA_SetReg(0x32,0xb8);         //s3200         //s3200         //00    //00	//00	//00
	CAMERA_SetReg(0x36,0x90);         //s3600         //s3600         //00    //00	//00	//00
	CAMERA_SetReg(0x3a,0x90);         //s3a00         //s3a00         //00    //00	//00	//00
	CAMERA_SetReg(0x33,0x01);         //s3301         //s3300         //00    //01	//00	//01
	CAMERA_SetReg(0x34,0x22);         //s3450         //s3400         //32    //50	//00	//50
	CAMERA_SetReg(0x37,0x01);         //s3701         //s3700         //00    //01	//00	//01
	CAMERA_SetReg(0x38,0x22);         //s3850         //s3800         //50    //50	//00	//50
	CAMERA_SetReg(0x3b,0x01);         //s3b01         //s3b00         //00    //01	//00	//01
	CAMERA_SetReg(0x3c,0x22);         //s3c50         //s3c00         //00    //50	//00	//50
	CAMERA_SetReg(0x31,0xf8);         //s31df         //s3187         //ff    //ff	//5e	//ff
	CAMERA_SetReg(0x35,0xce);         //s35b8         //s3569         //d7    //da	//54	//da
	CAMERA_SetReg(0x39,0xca);         //s39a9         //s395a         //be    //e6	//4d	//e6
	CAMERA_SetReg(0x3f,0x01);         //s3f01         //s3f01         //01    //01	//01	//01
	CAMERA_SetReg(0x40,0x54);         //s4040         //s4040         //4a    //40	//40	//40
	CAMERA_SetReg(0x3d,0x01);         //s3d00         //s3d01         //00    //00	//01	//00
	CAMERA_SetReg(0x3e,0x04);         //s3ef0         //s3e18         //fa    //f0	//18	//f0

	//gamma5
	CAMERA_SetReg(0xef,0x00);
	//Gamma(Red);
	CAMERA_SetReg(0x48,0x00);
	CAMERA_SetReg(0x49,0x00);
	CAMERA_SetReg(0x4A,0x08);
	CAMERA_SetReg(0x4B,0x1B);
	CAMERA_SetReg(0x48,0x01);
	CAMERA_SetReg(0x49,0x00);
	CAMERA_SetReg(0x4A,0x10);
	CAMERA_SetReg(0x4B,0x4E);
	CAMERA_SetReg(0x48,0x02);
	CAMERA_SetReg(0x49,0x00);
	CAMERA_SetReg(0x4A,0x20);
	CAMERA_SetReg(0x4B,0x6F);
	CAMERA_SetReg(0x48,0x03);
	CAMERA_SetReg(0x49,0x00);
	CAMERA_SetReg(0x4A,0x40);
	CAMERA_SetReg(0x4B,0x59);
	CAMERA_SetReg(0x48,0x04);
	CAMERA_SetReg(0x49,0x00);
	CAMERA_SetReg(0x4A,0x80);
	CAMERA_SetReg(0x4B,0x33);
	CAMERA_SetReg(0x48,0x05);
	CAMERA_SetReg(0x49,0x01);
	CAMERA_SetReg(0x4A,0x00);
	CAMERA_SetReg(0x4B,0x2B);
	CAMERA_SetReg(0x48,0x06);
	CAMERA_SetReg(0x49,0x01);
	CAMERA_SetReg(0x4A,0xC0);
	CAMERA_SetReg(0x4B,0x20);
	CAMERA_SetReg(0x48,0x07);
	CAMERA_SetReg(0x49,0x02);
	CAMERA_SetReg(0x4A,0x80);
	CAMERA_SetReg(0x4B,0x19);
	CAMERA_SetReg(0x48,0x08);
	CAMERA_SetReg(0x49,0x03);
	CAMERA_SetReg(0x4A,0x00);
	CAMERA_SetReg(0x4B,0x16);
	CAMERA_SetReg(0x48,0x09);
	CAMERA_SetReg(0x49,0x03);
	CAMERA_SetReg(0x4A,0x80);
	CAMERA_SetReg(0x4B,0x13);
	CAMERA_SetReg(0x48,0x0A);
	CAMERA_SetReg(0x49,0x03);
	CAMERA_SetReg(0x4A,0xC0);
	CAMERA_SetReg(0x4B,0x12);
	CAMERA_SetReg(0x48,0x0B);
	CAMERA_SetReg(0x4B,0x14);
	//Gamma(Green);
	CAMERA_SetReg(0x48,0x10);
	CAMERA_SetReg(0x49,0x00);
	CAMERA_SetReg(0x4A,0x08);
	CAMERA_SetReg(0x4B,0x1B);
	CAMERA_SetReg(0x48,0x11);
	CAMERA_SetReg(0x49,0x00);
	CAMERA_SetReg(0x4A,0x10);
	CAMERA_SetReg(0x4B,0x4E);
	CAMERA_SetReg(0x48,0x12);
	CAMERA_SetReg(0x49,0x00);
	CAMERA_SetReg(0x4A,0x20);
	CAMERA_SetReg(0x4B,0x6F);
	CAMERA_SetReg(0x48,0x13);
	CAMERA_SetReg(0x49,0x00);
	CAMERA_SetReg(0x4A,0x40);
	CAMERA_SetReg(0x4B,0x59);
	CAMERA_SetReg(0x48,0x14);
	CAMERA_SetReg(0x49,0x00);
	CAMERA_SetReg(0x4A,0x80);
	CAMERA_SetReg(0x4B,0x33);
	CAMERA_SetReg(0x48,0x15);
	CAMERA_SetReg(0x49,0x01);
	CAMERA_SetReg(0x4A,0x00);
	CAMERA_SetReg(0x4B,0x2B);
	CAMERA_SetReg(0x48,0x16);
	CAMERA_SetReg(0x49,0x01);
	CAMERA_SetReg(0x4A,0xC0);
	CAMERA_SetReg(0x4B,0x20);
	CAMERA_SetReg(0x48,0x17);
	CAMERA_SetReg(0x49,0x02);
	CAMERA_SetReg(0x4A,0x80);
	CAMERA_SetReg(0x4B,0x19);
	CAMERA_SetReg(0x48,0x18);
	CAMERA_SetReg(0x49,0x03);
	CAMERA_SetReg(0x4A,0x00);
	CAMERA_SetReg(0x4B,0x16);
	CAMERA_SetReg(0x48,0x19);
	CAMERA_SetReg(0x49,0x03);
	CAMERA_SetReg(0x4A,0x80);
	CAMERA_SetReg(0x4B,0x13);
	CAMERA_SetReg(0x48,0x1A);
	CAMERA_SetReg(0x49,0x03);
	CAMERA_SetReg(0x4A,0xC0);
	CAMERA_SetReg(0x4B,0x12);
	CAMERA_SetReg(0x48,0x1B);
	CAMERA_SetReg(0x4B,0x14);
	//Gamma(Blue);
	CAMERA_SetReg(0x48,0x20);
	CAMERA_SetReg(0x49,0x00);
	CAMERA_SetReg(0x4A,0x08);
	CAMERA_SetReg(0x4B,0x1B);
	CAMERA_SetReg(0x48,0x21);
	CAMERA_SetReg(0x49,0x00);
	CAMERA_SetReg(0x4A,0x10);
	CAMERA_SetReg(0x4B,0x4E);
	CAMERA_SetReg(0x48,0x22);
	CAMERA_SetReg(0x49,0x00);
	CAMERA_SetReg(0x4A,0x20);
	CAMERA_SetReg(0x4B,0x6F);
	CAMERA_SetReg(0x48,0x23);
	CAMERA_SetReg(0x49,0x00);
	CAMERA_SetReg(0x4A,0x40);
	CAMERA_SetReg(0x4B,0x59);
	CAMERA_SetReg(0x48,0x24);
	CAMERA_SetReg(0x49,0x00);
	CAMERA_SetReg(0x4A,0x80);
	CAMERA_SetReg(0x4B,0x33);
	CAMERA_SetReg(0x48,0x25);
	CAMERA_SetReg(0x49,0x01);
	CAMERA_SetReg(0x4A,0x00);
	CAMERA_SetReg(0x4B,0x2B);
	CAMERA_SetReg(0x48,0x26);
	CAMERA_SetReg(0x49,0x01);
	CAMERA_SetReg(0x4A,0xC0);
	CAMERA_SetReg(0x4B,0x20);
	CAMERA_SetReg(0x48,0x27);
	CAMERA_SetReg(0x49,0x02);
	CAMERA_SetReg(0x4A,0x80);
	CAMERA_SetReg(0x4B,0x19);
	CAMERA_SetReg(0x48,0x28);
	CAMERA_SetReg(0x49,0x03);
	CAMERA_SetReg(0x4A,0x00);
	CAMERA_SetReg(0x4B,0x16);
	CAMERA_SetReg(0x48,0x29);
	CAMERA_SetReg(0x49,0x03);
	CAMERA_SetReg(0x4A,0x80);
	CAMERA_SetReg(0x4B,0x13);
	CAMERA_SetReg(0x48,0x2A);
	CAMERA_SetReg(0x49,0x03);
	CAMERA_SetReg(0x4A,0xC0);
	CAMERA_SetReg(0x4B,0x12);
	CAMERA_SetReg(0x48,0x2B);
	CAMERA_SetReg(0x4B,0x14);
	CAMERA_SetReg(0x48,0x80);

	//AWB
	//CAMERA_SetReg(0xef,0x00);
	//CAMERA_SetReg(0x93,0x01);   // AWB map
	//CAMERA_SetReg(0xa0,0x0f);
	//CAMERA_SetReg(0x9f,0x0f);
	//CAMERA_SetReg(0x93,0x00);

	//AWB mode : auto(??);
	//CAMERA_SetReg(0xef,0x01);
	//CAMERA_SetReg(0x80,0x00);

	// Hue

	CAMERA_SetReg(0xef,0x00);
	CAMERA_SetReg(0x8c,0x0c);	//Skin Color Detection On/Off
	CAMERA_SetReg(0x74,0x01);	//20070214 //00	//Hue & Saturation Off=00, On=01
	CAMERA_SetReg(0x79,0x46);   //20070214 //58	//Saturation Gain
	CAMERA_SetReg(0x75,0x01);	//Hue & Saturation Cosine 1
	CAMERA_SetReg(0x76,0x00);   //a0   //20070214 //ff	//Hue & Saturation Cosine 2
	CAMERA_SetReg(0x77,0x02);	//Hue & Saturation Sine 1
	CAMERA_SetReg(0x78,0x00);   //20070214 //11	//Hue & Saturation Sine 2
	CAMERA_SetReg(0x72,0x88);	//20070213	//b0   //90	//Contrast Gain
	//CAMERA_SetReg(0x73,0x89);	//Brightness Gain
	CAMERA_SetReg(0xd8,0x00);   //Edge color suppress 

	//CAMERA_SetReg(0xef,0x02);
	//CAMERA_SetReg(0x0f,0x40); 

	//AWB white's count low 0f00 --> 0708
	CAMERA_SetReg(0xef,0x01);
	CAMERA_SetReg(0x85,0x00);   //07	//0f	//07
	CAMERA_SetReg(0x86,0xf0);	//08	//00	//08

	//AWB boundary
	//CAMERA_SetReg(0xef,0x00);
	//CAMERA_SetReg(0xb1,0x00);
	//CAMERA_SetReg(0xb2,0x21);
	//CAMERA_SetReg(0xb3,0x00);
	//CAMERA_SetReg(0xb4,0x2c);
	//CAMERA_SetReg(0xb5,0x01);
	//CAMERA_SetReg(0xb6,0xe0);
	//CAMERA_SetReg(0xb7,0x02);
	//CAMERA_SetReg(0xb8,0x70);

	// Bad Pixel Replacement

	//CAMERA_SetReg(0xef,0x00);
	//CAMERA_SetReg(0x1a,0x01);
	//CAMERA_SetReg(0x1b,0x00);
	//CAMERA_SetReg(0x1e,0x00);
	//CAMERA_SetReg(0x1c,0x00);
	//CAMERA_SetReg(0x1f,0x00);
	//CAMERA_SetReg(0x1d,0x00);
	//CAMERA_SetReg(0x20,0x00);

	CAMERA_SetReg(0xef,0x01);
	CAMERA_SetReg(0xb9,0x08);  //10  //30	//BPR Gain Setting
	CAMERA_SetReg(0xba,0x00);
	CAMERA_SetReg(0xbd,0x00);

	CAMERA_SetReg(0xeb,0x50);  //40  //6f   //Low Light Indication AGC

	//20070213
	//AWB Map 1
	CAMERA_SetReg(0xef,0x01);
	CAMERA_SetReg(0x94,0x80);    //00
	CAMERA_SetReg(0x95,0xc0);    //c0
	CAMERA_SetReg(0x96,0xc0);    //f0
	CAMERA_SetReg(0x97,0xf0);    //c0
	CAMERA_SetReg(0x98,0x78);    //78
	CAMERA_SetReg(0x99,0x78);    //78
	CAMERA_SetReg(0x9a,0x7c);    //7c
	CAMERA_SetReg(0x9b,0x3c);    //3f
	CAMERA_SetReg(0x9c,0x1e);    //1f
	CAMERA_SetReg(0x9d,0x0f);    //0f
	CAMERA_SetReg(0x9e,0x07);    //07
	CAMERA_SetReg(0x9f,0x03);    //03
	CAMERA_SetReg(0xa0,0x00);    //01
	CAMERA_SetReg(0xa1,0xc8);  
	CAMERA_SetReg(0xa2,0x02);
	CAMERA_SetReg(0x93,0x02);

	//20070213
	//AWB Map Direct Write
	CAMERA_SetReg(0xef,0x00);
	CAMERA_SetReg(0x94,0x80);    //00
	CAMERA_SetReg(0x95,0xc0);    //c0
	CAMERA_SetReg(0x96,0xc0);    //f0
	CAMERA_SetReg(0x97,0xf0);    //c0
	CAMERA_SetReg(0x98,0x78);    //78
	CAMERA_SetReg(0x99,0x78);    //78
	CAMERA_SetReg(0xa0,0x7c);    //7c
	CAMERA_SetReg(0xa1,0x3c);    //3f
	CAMERA_SetReg(0xa2,0x1e);    //1f
	CAMERA_SetReg(0xa3,0x0f);    //0f
	CAMERA_SetReg(0xa4,0x07);    //07
	CAMERA_SetReg(0xa5,0x03);    //03
	CAMERA_SetReg(0xa6,0x00);    //01

	//Auto Flicker Correction
	CAMERA_SetReg(0xef,0x01);
	CAMERA_SetReg(0xdd,0x31);
	CAMERA_SetReg(0xcd,0x21);
	CAMERA_SetReg(0xde,0x31);

	//Frame AE Control - L.L 5fps
	//*** Frame 고정시는 아래 부분을 모두 주석처리 할 것.***
	CAMERA_SetReg(0xef,0x00);
	CAMERA_SetReg(0x57,0x00);
	//CAMERA_SetReg(0xef,0x01);
	//CAMERA_SetReg(0xe2,0x18);
	//CAMERA_SetReg(0xe3,0x16);
	//CAMERA_SetReg(0xe8,0x88);
	//******************************************************

	//CAMERA_SetReg(0xef,0x00);
	//CAMERA_SetReg(0x43,0x40);  //20070222  //fixed value
	//CAMERA_SetReg(0x42,0x3d);  //AWB R start point
	//CAMERA_SetReg(0x44,0x6b);  //AWB B start point

	CAMERA_SetReg(0xef,0x01);  //shadow line
	CAMERA_SetReg(0xa5,0x00);

	CAMERA_SetReg(0x70,0x17);   //17 // fix PAL(50Hz);



	CAMERA_SetReg(0xef,0x00);
	CAMERA_SetReg(0xb9,0x02);
	CAMERA_SetReg(0xba,0x80);
	CAMERA_SetReg(0xbb,0x80);
	CAMERA_SetReg(0xbc,0x80); //98
	CAMERA_SetReg(0xbd,0x00); //30


//	CAMERA_SetReg(0xef,0x01);  //shadow line
//	CAMERA_GetReg(0x70, &x);
//	UART_Printf("0x70 = 0x97 --> %x\n", x);

//	CAMERA_SetReg(0xef,0x00);
//	CAMERA_GetReg(0xcb, &x);
//	UART_Printf("0xca = 0x00 --> %x\n", x);


	CAMERA_IICClose();


}

void CAMERA_InitA3AFX_QVGA_15FR(void)
{
	//u32 uHCLKx2;

	SYSC_GetClkInform(); // Get HCLKx2 Frequency
	//uHCLKx2 = g_HCLKx2/1000000;
	
	oCamera.m_ucSlaveAddr = 0xc4;

// 	 CAMERA_IICOpen(100000); 
 	 CAMERA_IICOpen(16000000); 


	//3.13 Pearl setting으로 시작.

	//------------------------------------------------------------------
	// Set File 
	// - S5KA3AFX_xshade_061018.xls generates
	// - date : 2007-03-12오후 3:20:55
	//------------------------------------------------------------------

	CAMERA_SetReg(0xef,0x01);
	CAMERA_SetReg(0x70,0x00); //AE,AWB OFF

	//QVGA
	CAMERA_SetReg(0xef,0x00);
	CAMERA_SetReg(0x7c,0xe0);
	CAMERA_SetReg(0x81,0x01);
	CAMERA_SetReg(0x82,0x50);
	CAMERA_SetReg(0x7f,0x00);
	CAMERA_SetReg(0x80,0xf0);
	CAMERA_SetReg(0xc2,0x09);
	CAMERA_SetReg(0xc6,0x01);
	CAMERA_SetReg(0xc7,0x40);
	CAMERA_SetReg(0xc4,0x00);
	CAMERA_SetReg(0xc5,0xF0);

	//// Page : 00
	//CAMERA_SetReg(0xef,0x00);
	//CAMERA_SetReg(0x0d,0x03);
	//CAMERA_SetReg(0x0e,0x00);
	//CAMERA_SetReg(0x0f,0x1F);
	//CAMERA_SetReg(0x10,0x00);
	//CAMERA_SetReg(0x11,0x37);
	//CAMERA_SetReg(0x12,0x08);
	//CAMERA_SetReg(0x13,0x42);
	//CAMERA_SetReg(0x14,0x00);
	//CAMERA_SetReg(0x15,0x12);
	//CAMERA_SetReg(0x16,0x9E);
	//CAMERA_SetReg(0x19,0x00);
	//CAMERA_SetReg(0x17,0x07);
	//CAMERA_SetReg(0x18,0xE0);
	//CAMERA_SetReg(0x19,0x01);
	//CAMERA_SetReg(0x17,0x07);
	//CAMERA_SetReg(0x18,0xE3);
	//CAMERA_SetReg(0x19,0x02);
	//CAMERA_SetReg(0x17,0x07);
	//CAMERA_SetReg(0x18,0xED);
	//CAMERA_SetReg(0x19,0x03);
	//CAMERA_SetReg(0x17,0x07);
	//CAMERA_SetReg(0x18,0xCA);
	//CAMERA_SetReg(0x19,0x04);
	//CAMERA_SetReg(0x17,0x07);
	//CAMERA_SetReg(0x18,0xD2);
	//CAMERA_SetReg(0x19,0x05);
	//CAMERA_SetReg(0x17,0x07);
	//CAMERA_SetReg(0x18,0xEC);
	//CAMERA_SetReg(0x19,0x06);
	//CAMERA_SetReg(0x17,0x07);
	//CAMERA_SetReg(0x18,0xE5);
	//CAMERA_SetReg(0x19,0x07);
	//CAMERA_SetReg(0x17,0x00);
	//CAMERA_SetReg(0x18,0x12);
	//CAMERA_SetReg(0x19,0x08);
	//CAMERA_SetReg(0x17,0x07);
	//CAMERA_SetReg(0x18,0xE6);
	//CAMERA_SetReg(0x19,0x09);
	//CAMERA_SetReg(0x17,0x00);
	//CAMERA_SetReg(0x18,0x08);
	//CAMERA_SetReg(0x19,0x0A);
	//CAMERA_SetReg(0x17,0x07);
	//CAMERA_SetReg(0x18,0xEA);
	//CAMERA_SetReg(0x19,0x0B);
	//CAMERA_SetReg(0x17,0x07);
	//CAMERA_SetReg(0x18,0xDA);
	//CAMERA_SetReg(0x19,0x0C);
	//CAMERA_SetReg(0x17,0x07);
	//CAMERA_SetReg(0x18,0xE3);
	//CAMERA_SetReg(0x19,0x0D);
	//CAMERA_SetReg(0x17,0x07);
	//CAMERA_SetReg(0x18,0xF9);
	//CAMERA_SetReg(0x19,0x0E);
	//CAMERA_SetReg(0x17,0x00);
	//CAMERA_SetReg(0x18,0x1C);
	//CAMERA_SetReg(0x19,0x0F);
	//CAMERA_SetReg(0x17,0x00);
	//CAMERA_SetReg(0x18,0x12);
	//CAMERA_SetReg(0x19,0x10);
	//CAMERA_SetReg(0x17,0x00);
	//CAMERA_SetReg(0x18,0x18);
	//CAMERA_SetReg(0x19,0x11);
	//CAMERA_SetReg(0x17,0x00);
	//CAMERA_SetReg(0x18,0x08);
	//CAMERA_SetReg(0x19,0x12);
	//CAMERA_SetReg(0x17,0x07);
	//CAMERA_SetReg(0x18,0xEB);
	//CAMERA_SetReg(0x19,0x13);
	//CAMERA_SetReg(0x17,0x07);
	//CAMERA_SetReg(0x18,0xF9);
	//CAMERA_SetReg(0x19,0x14);
	//CAMERA_SetReg(0x17,0x07);
	//CAMERA_SetReg(0x18,0xF4);
	//CAMERA_SetReg(0x19,0x15);
	//CAMERA_SetReg(0x17,0x00);
	//CAMERA_SetReg(0x18,0x1F);
	//CAMERA_SetReg(0x19,0x16);
	//CAMERA_SetReg(0x17,0x00);
	//CAMERA_SetReg(0x18,0x15);
	//CAMERA_SetReg(0x19,0x17);
	//CAMERA_SetReg(0x17,0x00);
	//CAMERA_SetReg(0x18,0x11);
	//CAMERA_SetReg(0x19,0x18);
	//CAMERA_SetReg(0x17,0x00);
	//CAMERA_SetReg(0x18,0x0E);
	//CAMERA_SetReg(0x19,0x19);
	//CAMERA_SetReg(0x17,0x00);
	//CAMERA_SetReg(0x18,0x08);
	//CAMERA_SetReg(0x19,0x1A);
	//CAMERA_SetReg(0x17,0x07);
	//CAMERA_SetReg(0x18,0xFE);
	//CAMERA_SetReg(0x19,0x1B);
	//CAMERA_SetReg(0x17,0x07);
	//CAMERA_SetReg(0x18,0xF4);
	//CAMERA_SetReg(0x19,0x1C);
	//CAMERA_SetReg(0x17,0x00);
	//CAMERA_SetReg(0x18,0x24);
	//CAMERA_SetReg(0x19,0x1D);
	//CAMERA_SetReg(0x17,0x00);
	//CAMERA_SetReg(0x18,0x03);
	//CAMERA_SetReg(0x19,0x1E);
	//CAMERA_SetReg(0x17,0x00);
	//CAMERA_SetReg(0x18,0x02);
	//CAMERA_SetReg(0x19,0x1F);
	//CAMERA_SetReg(0x17,0x07);
	//CAMERA_SetReg(0x18,0xFF);
	//CAMERA_SetReg(0x19,0x20);
	//CAMERA_SetReg(0x17,0x00);
	//CAMERA_SetReg(0x18,0x12);
	//CAMERA_SetReg(0x19,0x21);
	//CAMERA_SetReg(0x17,0x07);
	//CAMERA_SetReg(0x18,0xE5);
	//CAMERA_SetReg(0x19,0x22);
	//CAMERA_SetReg(0x17,0x07);
	//CAMERA_SetReg(0x18,0xD1);
	//CAMERA_SetReg(0x19,0x40);
	//

	//CAMERA_SetReg(0xef,0x02);
	//CAMERA_SetReg(0x59,0x50); //P, S
	//CAMERA_SetReg(0x5a,0x32); //M
	//CAMERA_SetReg(0x58,0x21); //PLL

	//////////////////////////////////////////////////////////

	CAMERA_SetReg(0xef,0x02);   // CIS page
	//CAMERA_SetReg(0x4b,0x5E);   // PCLK driving current max
	//CAMERA_SetReg(0x23,0xd8);   // Tx width
	CAMERA_SetReg(0x3a,0x20);   // double shutter off
	CAMERA_SetReg(0x65,0x08);   // CIS mode
	CAMERA_SetReg(0x0a,0x90);   // Display WCW
	CAMERA_SetReg(0x01,0x26);   // E-shutter enable
	                 
	CAMERA_SetReg(0x03,0x78);   // S1R END						**** Revised at EVT1
	CAMERA_SetReg(0x05,0x00);   // S1S end value control
	CAMERA_SetReg(0x07,0xE5);   // s2  END
	CAMERA_SetReg(0x09,0x0E);   // S4  END
	CAMERA_SetReg(0x0b,0x84);	//85	//81   // Analog offset
	CAMERA_SetReg(0x13,0xa0);   //20   // R_REF
	//CAMERA_SetReg(0x15,0x80);   // EC COMP 100'b
	CAMERA_SetReg(0x1c,0x58);   //20070304AM //57   // global gain						**** Revised at EVT1
	CAMERA_SetReg(0x24,0x3d);   // i_cds						**** Revised at EVT1
	CAMERA_SetReg(0x26,0x25);   // clamp level control					**** Revised at EVT1
	CAMERA_SetReg(0x29,0x8f);   // CFPN start frame (16);
	CAMERA_SetReg(0x4b,0x5b);   // IO driving current
	CAMERA_SetReg(0x56,0x05);   // Line ADLC
	//CAMERA_SetReg(0x58,0x61);   // amp bias current source select (71h : self bias);
	CAMERA_SetReg(0x5f,0x06);   // aps bias current      
	CAMERA_SetReg(0x67,0x3c);   // EC_COMP & SAVG_END
	CAMERA_SetReg(0x68,0x10);   // i_rmp control
	CAMERA_SetReg(0x69,0x13);   // i_mid

	//CAMERA_SetReg(0x58,0x21);	// PLL on
	//CAMERA_SetReg(0x59,0x1A);	// pll_s=0, pll_p=26,
	//CAMERA_SetReg(0x5A,0x1A);	// pll_m=26

	CAMERA_SetReg(0x28,0x9B);   // CFPN on						**** Revised at EVT1




	//061011
	CAMERA_SetReg(0xef,0x01);
	CAMERA_SetReg(0xd1,0xaa);   //89:16.7MHz, d2:24MHz 64:13MHz dc:25MHz 13MHz //d2	//d2(24MHz); Mclk Value (Mclk - 30d);/100,000 
	//CAMERA_SetReg(0xd1,0x64);	//64(13MHz); Mclk Value (Mclk - 30d);/100,000 
	//CAMERA_SetReg(0xd9,0x15);   //AE saturation CHIP_DEBUG

	//////////
	//page 0
	//////////
	CAMERA_SetReg(0xef,0x00);
	//CAMERA_SetReg(0x09,0x3e);   //gr gain

	CAMERA_SetReg(0xef,0x00);
	CAMERA_SetReg(0x27,0x40);  //08  //GR/GB Correction Limitation Value
	CAMERA_SetReg(0x29,0x60);  //GrGb Edge Threshold Value

	// AE Window
	CAMERA_SetReg(0xef,0x00);
	CAMERA_SetReg(0x55,0x00);	//Window diplay off
	CAMERA_SetReg(0x58,0x00);
	CAMERA_SetReg(0x59,0x00);
	CAMERA_SetReg(0x5a,0x02);
	CAMERA_SetReg(0x5b,0x80);
	CAMERA_SetReg(0x5c,0x00);
	CAMERA_SetReg(0x5d,0x00);
	CAMERA_SetReg(0x5e,0x01);
	CAMERA_SetReg(0x5f,0xe0);
	CAMERA_SetReg(0x60,0x00);
	CAMERA_SetReg(0x61,0x00);
	CAMERA_SetReg(0x62,0x02);
	CAMERA_SetReg(0x63,0x80);
	CAMERA_SetReg(0x64,0x00);
	CAMERA_SetReg(0x65,0x00);
	CAMERA_SetReg(0x66,0x01);
	CAMERA_SetReg(0x67,0xe0);

	CAMERA_SetReg(0xef,0x01);	// AE weight
	CAMERA_SetReg(0x77,0x01);
	CAMERA_SetReg(0x78,0x01);
	//CAMERA_SetReg(0x79,0x03);
	//CAMERA_SetReg(0x7a,0x03);
	//CAMERA_SetReg(0x7b,0x04);
	//CAMERA_SetReg(0x7c,0x04);

	CAMERA_SetReg(0xef,0x00);
	//CAMERA_SetReg(0x57,0x18);  // AE Lux min threshold

	CAMERA_SetReg(0xd9,0xc0);  // color edge threshold

	////AWB Luminance check
	////20070212 Added for low light starting env.
	CAMERA_SetReg(0xef,0x00);
	CAMERA_SetReg(0xad,0x03);	//Luminance upper_level[9:8]
	CAMERA_SetReg(0xae,0x20);	//Luminance upper_level[7:0]
	CAMERA_SetReg(0xaf,0x00);	//Luminance lower_level[9:8]
	CAMERA_SetReg(0xb0,0x90);	//Luminance lower_level[7:0]

	/////////////////////////////////////////////////////
	//page 1/////////////////////////////////////////////
	/////////////////////////////////////////////////////
	CAMERA_SetReg(0xef,0x01);
	////20070212 Modified for low light starting env.
	CAMERA_SetReg(0x83,0x72);	//7d	//AWB R/B Max Gain : <0.44>'s max
	CAMERA_SetReg(0x84,0x38);	//30	//AWB R/B Min Gain : <0.42>'s max

	CAMERA_SetReg(0xa8,0x41);   //20070222  //shutter Control
	CAMERA_SetReg(0xa9,0xd4);   //20070222  //Shutter Threshold

	CAMERA_SetReg(0xaf,0x04);	//20070213PM	//00   //L.L Brightness
	CAMERA_SetReg(0xb4,0x88);	//20070213	//b0   //a0   // N.L contrast
	CAMERA_SetReg(0xb5,0x88);	//20070213	//b0   //a0   // L.L contrast 
	CAMERA_SetReg(0xbc,0x66);   //20070305PM //26	//20070213	//AWB skip point (default:16);

	CAMERA_SetReg(0xb7,0x46);   //58   //68   // N.L color saturation
	CAMERA_SetReg(0xb8,0x46);   //20070305PM //30   //52   // L.L color saturation

	//CAMERA_SetReg(0x70,0x97);   //17 // fix PAL(50Hz);
	CAMERA_SetReg(0x71,0x7a); //6a);	//20070213PM	//55   //5a   //68   //70   //6f   //4d	//60	//78   //AE target
	CAMERA_SetReg(0x72,0x09);   //20070214PM  //07   //0c   //12   //0a   //05	//12   // AE threshold
	//CAMERA_SetReg(0x73,0x34);	//20070214PM  //AE Step Guard Value 
	CAMERA_SetReg(0x74,0x6b);   //60   //70	//74	//70   // low agc max
	CAMERA_SetReg(0x75,0x40);   // normal agc max
	  
	CAMERA_SetReg(0xa6,0x40);  //80  //N.L sharpness H/V Gain
	CAMERA_SetReg(0xab,0x40);  //38  //18  //60 //N.L NR Edge 
	CAMERA_SetReg(0xad,0x80);  //40  //20  //L.L NR Edge
	//CAMERA_SetReg(0xa3,0x30); //AWB speed //EVT0은 고정되어 있음.(0x03); //EVT1은 변경 가능.

	CAMERA_SetReg(0xcc,0x09);   //08   //Flicker threshold 12
	CAMERA_SetReg(0xd5,0x22);   //23	//32  //Chip_Debug2

	CAMERA_SetReg(0xe0,0x08);   // NT Max
	CAMERA_SetReg(0xe2,0x08);   //07   //18   // Low NT max
	CAMERA_SetReg(0xe1,0x05);   // PAL Max
	CAMERA_SetReg(0xe3,0x05);   //07   //18   // Low PAL max

	CAMERA_SetReg(0xe4,0x01);   // 24MHz VblankH (H.L); //FPS (24MHz, 15fps);
	CAMERA_SetReg(0xe5,0x44);   //ea   // 24MHz NT VblankL (H.L);
	//CAMERA_SetReg(0xe4,0x00);	// 13MHz VblankH (H.L); //FPS (24MHz, 15fps);
	//CAMERA_SetReg(0xe5,0x2a);	// 13MHz VblankL (H.L);

	CAMERA_SetReg(0xe6,0x00);   //20070215AM // 24MHz VblankH (H.L); //FPS (24MHz, 15fps);
	CAMERA_SetReg(0xe7,0x2a);   //20070215AM //ea   // 24MHz NT VblankL (H.L);

	CAMERA_SetReg(0xe8,0x68);   //44  //84 //97  // ae speed
	CAMERA_SetReg(0xe9,0x84); //Double shutter ON
	//CAMERA_SetReg(0xe9,0x87);
	CAMERA_SetReg(0xef,0x00);
	//CAMERA_SetReg(0x8c,0x0e); //skin color detection
	CAMERA_SetReg(0xc8,0x09);   //0a  //YC delay


	////////////////////////////////////////
	// Color Correction -> Shading -> Gamma -> AWB -> Hue
	////////////////////////////////////////

	// Color Correction
	                   
	//CAMERA_SetReg(0xef,0x00);         //	//D65-1	//D65	//CW	//U30-6	//U30	//Old
	//CAMERA_SetReg(0x4c,0x40);         //95	//95	//81	//8a	//50	//62	//56
	//CAMERA_SetReg(0x4d,0x92);         //c2	//c2	//ba	//d2	//af	//a9	//8d
	//CAMERA_SetReg(0x4e,0x0f);         //92	//92	//87	//07	//1f	//07	//89
	//CAMERA_SetReg(0x4f,0x9a);         //9b	//9b	//98	//9e	//9e	//a2	//94
	//CAMERA_SetReg(0x50,0x3b);         //73	//73	//64	//4c	//2e	//47	//5c
	//CAMERA_SetReg(0x51,0x1e);         //98	//98	//8c	//12	//30	//1b	//8b
	//CAMERA_SetReg(0x52,0x82);         //80	//80	//80	//80	//86	//85	//80
	//CAMERA_SetReg(0x53,0xbf);         //b3	//b3	//ad	//cd	//eb	//f2	//91
	//CAMERA_SetReg(0x54,0x7f);         //73	//73	//6d	//8e	//b2	//b8	//51

	////CW
	CAMERA_SetReg(0xef,0x00);
	CAMERA_SetReg(0x4c,0x92);
	CAMERA_SetReg(0x4d,0xcf);
	CAMERA_SetReg(0x4e,0x82);
	CAMERA_SetReg(0x4f,0xad);
	CAMERA_SetReg(0x50,0x6e);
	CAMERA_SetReg(0x51,0x80);
	CAMERA_SetReg(0x52,0x87);
	CAMERA_SetReg(0x53,0xeb);
	CAMERA_SetReg(0x54,0xb2);
	//
	////D65
	//CAMERA_SetReg(0xef,0x00);
	//CAMERA_SetReg(0x4c,0x8a);
	//CAMERA_SetReg(0x4d,0xc2);
	//CAMERA_SetReg(0x4e,0x88);
	//CAMERA_SetReg(0x4f,0x9f);
	//CAMERA_SetReg(0x50,0x72);
	//CAMERA_SetReg(0x51,0x93);
	//CAMERA_SetReg(0x52,0x81);
	//CAMERA_SetReg(0x53,0xca);
	//CAMERA_SetReg(0x54,0x8c);

	//Default
	//CAMERA_SetReg(0xef,0x00);
	//CAMERA_SetReg(0x4c,0x56);
	//CAMERA_SetReg(0x4d,0x8d);
	//CAMERA_SetReg(0x4e,0x89);
	//CAMERA_SetReg(0x4f,0x96);
	//CAMERA_SetReg(0x50,0x5d);
	//CAMERA_SetReg(0x51,0x88);
	//CAMERA_SetReg(0x52,0x94);
	//CAMERA_SetReg(0x53,0x89);
	//CAMERA_SetReg(0x54,0x5d);

	//// Shading2   //// RGB Shading// RGB Shading// RGB Shading
	//              //              //              	      //linear//old
	CAMERA_SetReg(0xef,0x00);         //sef00         //sef00         //00    //00	//00	//00
	CAMERA_SetReg(0x30,0x01);         //s3001         //s3001         //01    //01	//01	//01
	CAMERA_SetReg(0x32,0xb8);         //s3200         //s3200         //00    //00	//00	//00
	CAMERA_SetReg(0x36,0x90);         //s3600         //s3600         //00    //00	//00	//00
	CAMERA_SetReg(0x3a,0x90);         //s3a00         //s3a00         //00    //00	//00	//00
	CAMERA_SetReg(0x33,0x01);         //s3301         //s3300         //00    //01	//00	//01
	CAMERA_SetReg(0x34,0x22);         //s3450         //s3400         //32    //50	//00	//50
	CAMERA_SetReg(0x37,0x01);         //s3701         //s3700         //00    //01	//00	//01
	CAMERA_SetReg(0x38,0x22);         //s3850         //s3800         //50    //50	//00	//50
	CAMERA_SetReg(0x3b,0x01);         //s3b01         //s3b00         //00    //01	//00	//01
	CAMERA_SetReg(0x3c,0x22);         //s3c50         //s3c00         //00    //50	//00	//50
	CAMERA_SetReg(0x31,0xf8);         //s31df         //s3187         //ff    //ff	//5e	//ff
	CAMERA_SetReg(0x35,0xce);         //s35b8         //s3569         //d7    //da	//54	//da
	CAMERA_SetReg(0x39,0xca);         //s39a9         //s395a         //be    //e6	//4d	//e6
	CAMERA_SetReg(0x3f,0x01);         //s3f01         //s3f01         //01    //01	//01	//01
	CAMERA_SetReg(0x40,0x54);         //s4040         //s4040         //4a    //40	//40	//40
	CAMERA_SetReg(0x3d,0x01);         //s3d00         //s3d01         //00    //00	//01	//00
	CAMERA_SetReg(0x3e,0x04);         //s3ef0         //s3e18         //fa    //f0	//18	//f0

	//gamma5
	CAMERA_SetReg(0xef,0x00);
	//Gamma(Red);
	CAMERA_SetReg(0x48,0x00);
	CAMERA_SetReg(0x49,0x00);
	CAMERA_SetReg(0x4A,0x08);
	CAMERA_SetReg(0x4B,0x1B);
	CAMERA_SetReg(0x48,0x01);
	CAMERA_SetReg(0x49,0x00);
	CAMERA_SetReg(0x4A,0x10);
	CAMERA_SetReg(0x4B,0x4E);
	CAMERA_SetReg(0x48,0x02);
	CAMERA_SetReg(0x49,0x00);
	CAMERA_SetReg(0x4A,0x20);
	CAMERA_SetReg(0x4B,0x6F);
	CAMERA_SetReg(0x48,0x03);
	CAMERA_SetReg(0x49,0x00);
	CAMERA_SetReg(0x4A,0x40);
	CAMERA_SetReg(0x4B,0x59);
	CAMERA_SetReg(0x48,0x04);
	CAMERA_SetReg(0x49,0x00);
	CAMERA_SetReg(0x4A,0x80);
	CAMERA_SetReg(0x4B,0x33);
	CAMERA_SetReg(0x48,0x05);
	CAMERA_SetReg(0x49,0x01);
	CAMERA_SetReg(0x4A,0x00);
	CAMERA_SetReg(0x4B,0x2B);
	CAMERA_SetReg(0x48,0x06);
	CAMERA_SetReg(0x49,0x01);
	CAMERA_SetReg(0x4A,0xC0);
	CAMERA_SetReg(0x4B,0x20);
	CAMERA_SetReg(0x48,0x07);
	CAMERA_SetReg(0x49,0x02);
	CAMERA_SetReg(0x4A,0x80);
	CAMERA_SetReg(0x4B,0x19);
	CAMERA_SetReg(0x48,0x08);
	CAMERA_SetReg(0x49,0x03);
	CAMERA_SetReg(0x4A,0x00);
	CAMERA_SetReg(0x4B,0x16);
	CAMERA_SetReg(0x48,0x09);
	CAMERA_SetReg(0x49,0x03);
	CAMERA_SetReg(0x4A,0x80);
	CAMERA_SetReg(0x4B,0x13);
	CAMERA_SetReg(0x48,0x0A);
	CAMERA_SetReg(0x49,0x03);
	CAMERA_SetReg(0x4A,0xC0);
	CAMERA_SetReg(0x4B,0x12);
	CAMERA_SetReg(0x48,0x0B);
	CAMERA_SetReg(0x4B,0x14);
	//Gamma(Green);
	CAMERA_SetReg(0x48,0x10);
	CAMERA_SetReg(0x49,0x00);
	CAMERA_SetReg(0x4A,0x08);
	CAMERA_SetReg(0x4B,0x1B);
	CAMERA_SetReg(0x48,0x11);
	CAMERA_SetReg(0x49,0x00);
	CAMERA_SetReg(0x4A,0x10);
	CAMERA_SetReg(0x4B,0x4E);
	CAMERA_SetReg(0x48,0x12);
	CAMERA_SetReg(0x49,0x00);
	CAMERA_SetReg(0x4A,0x20);
	CAMERA_SetReg(0x4B,0x6F);
	CAMERA_SetReg(0x48,0x13);
	CAMERA_SetReg(0x49,0x00);
	CAMERA_SetReg(0x4A,0x40);
	CAMERA_SetReg(0x4B,0x59);
	CAMERA_SetReg(0x48,0x14);
	CAMERA_SetReg(0x49,0x00);
	CAMERA_SetReg(0x4A,0x80);
	CAMERA_SetReg(0x4B,0x33);
	CAMERA_SetReg(0x48,0x15);
	CAMERA_SetReg(0x49,0x01);
	CAMERA_SetReg(0x4A,0x00);
	CAMERA_SetReg(0x4B,0x2B);
	CAMERA_SetReg(0x48,0x16);
	CAMERA_SetReg(0x49,0x01);
	CAMERA_SetReg(0x4A,0xC0);
	CAMERA_SetReg(0x4B,0x20);
	CAMERA_SetReg(0x48,0x17);
	CAMERA_SetReg(0x49,0x02);
	CAMERA_SetReg(0x4A,0x80);
	CAMERA_SetReg(0x4B,0x19);
	CAMERA_SetReg(0x48,0x18);
	CAMERA_SetReg(0x49,0x03);
	CAMERA_SetReg(0x4A,0x00);
	CAMERA_SetReg(0x4B,0x16);
	CAMERA_SetReg(0x48,0x19);
	CAMERA_SetReg(0x49,0x03);
	CAMERA_SetReg(0x4A,0x80);
	CAMERA_SetReg(0x4B,0x13);
	CAMERA_SetReg(0x48,0x1A);
	CAMERA_SetReg(0x49,0x03);
	CAMERA_SetReg(0x4A,0xC0);
	CAMERA_SetReg(0x4B,0x12);
	CAMERA_SetReg(0x48,0x1B);
	CAMERA_SetReg(0x4B,0x14);
	//Gamma(Blue);
	CAMERA_SetReg(0x48,0x20);
	CAMERA_SetReg(0x49,0x00);
	CAMERA_SetReg(0x4A,0x08);
	CAMERA_SetReg(0x4B,0x1B);
	CAMERA_SetReg(0x48,0x21);
	CAMERA_SetReg(0x49,0x00);
	CAMERA_SetReg(0x4A,0x10);
	CAMERA_SetReg(0x4B,0x4E);
	CAMERA_SetReg(0x48,0x22);
	CAMERA_SetReg(0x49,0x00);
	CAMERA_SetReg(0x4A,0x20);
	CAMERA_SetReg(0x4B,0x6F);
	CAMERA_SetReg(0x48,0x23);
	CAMERA_SetReg(0x49,0x00);
	CAMERA_SetReg(0x4A,0x40);
	CAMERA_SetReg(0x4B,0x59);
	CAMERA_SetReg(0x48,0x24);
	CAMERA_SetReg(0x49,0x00);
	CAMERA_SetReg(0x4A,0x80);
	CAMERA_SetReg(0x4B,0x33);
	CAMERA_SetReg(0x48,0x25);
	CAMERA_SetReg(0x49,0x01);
	CAMERA_SetReg(0x4A,0x00);
	CAMERA_SetReg(0x4B,0x2B);
	CAMERA_SetReg(0x48,0x26);
	CAMERA_SetReg(0x49,0x01);
	CAMERA_SetReg(0x4A,0xC0);
	CAMERA_SetReg(0x4B,0x20);
	CAMERA_SetReg(0x48,0x27);
	CAMERA_SetReg(0x49,0x02);
	CAMERA_SetReg(0x4A,0x80);
	CAMERA_SetReg(0x4B,0x19);
	CAMERA_SetReg(0x48,0x28);
	CAMERA_SetReg(0x49,0x03);
	CAMERA_SetReg(0x4A,0x00);
	CAMERA_SetReg(0x4B,0x16);
	CAMERA_SetReg(0x48,0x29);
	CAMERA_SetReg(0x49,0x03);
	CAMERA_SetReg(0x4A,0x80);
	CAMERA_SetReg(0x4B,0x13);
	CAMERA_SetReg(0x48,0x2A);
	CAMERA_SetReg(0x49,0x03);
	CAMERA_SetReg(0x4A,0xC0);
	CAMERA_SetReg(0x4B,0x12);
	CAMERA_SetReg(0x48,0x2B);
	CAMERA_SetReg(0x4B,0x14);
	CAMERA_SetReg(0x48,0x80);

	//AWB
	//CAMERA_SetReg(0xef,0x00);
	//CAMERA_SetReg(0x93,0x01);   // AWB map
	//CAMERA_SetReg(0xa0,0x0f);
	//CAMERA_SetReg(0x9f,0x0f);
	//CAMERA_SetReg(0x93,0x00);

	//AWB mode : auto(??);
	//CAMERA_SetReg(0xef,0x01);
	//CAMERA_SetReg(0x80,0x00);

	// Hue

	CAMERA_SetReg(0xef,0x00);
	CAMERA_SetReg(0x8c,0x0c);	//Skin Color Detection On/Off
	CAMERA_SetReg(0x74,0x01);	//20070214 //00	//Hue & Saturation Off=00, On=01
	CAMERA_SetReg(0x79,0x46);   //20070214 //58	//Saturation Gain
	CAMERA_SetReg(0x75,0x01);	//Hue & Saturation Cosine 1
	CAMERA_SetReg(0x76,0x00);   //a0   //20070214 //ff	//Hue & Saturation Cosine 2
	CAMERA_SetReg(0x77,0x02);	//Hue & Saturation Sine 1
	CAMERA_SetReg(0x78,0x00);   //20070214 //11	//Hue & Saturation Sine 2
	CAMERA_SetReg(0x72,0x88);	//20070213	//b0   //90	//Contrast Gain
	//CAMERA_SetReg(0x73,0x89);	//Brightness Gain
	CAMERA_SetReg(0xd8,0x00);   //Edge color suppress 

	//CAMERA_SetReg(0xef,0x02);
	//CAMERA_SetReg(0x0f,0x40); 

	//AWB white's count low 0f00 --> 0708
	CAMERA_SetReg(0xef,0x01);
	CAMERA_SetReg(0x85,0x00);   //07	//0f	//07
	CAMERA_SetReg(0x86,0xf0);	//08	//00	//08

	//AWB boundary
	//CAMERA_SetReg(0xef,0x00);
	//CAMERA_SetReg(0xb1,0x00);
	//CAMERA_SetReg(0xb2,0x21);
	//CAMERA_SetReg(0xb3,0x00);
	//CAMERA_SetReg(0xb4,0x2c);
	//CAMERA_SetReg(0xb5,0x01);
	//CAMERA_SetReg(0xb6,0xe0);
	//CAMERA_SetReg(0xb7,0x02);
	//CAMERA_SetReg(0xb8,0x70);

	// Bad Pixel Replacement

	//CAMERA_SetReg(0xef,0x00);
	//CAMERA_SetReg(0x1a,0x01);
	//CAMERA_SetReg(0x1b,0x00);
	//CAMERA_SetReg(0x1e,0x00);
	//CAMERA_SetReg(0x1c,0x00);
	//CAMERA_SetReg(0x1f,0x00);
	//CAMERA_SetReg(0x1d,0x00);
	//CAMERA_SetReg(0x20,0x00);

	CAMERA_SetReg(0xef,0x01);
	CAMERA_SetReg(0xb9,0x08);  //10  //30	//BPR Gain Setting
	CAMERA_SetReg(0xba,0x00);
	CAMERA_SetReg(0xbd,0x00);

	CAMERA_SetReg(0xeb,0x50);  //40  //6f   //Low Light Indication AGC

	//20070213
	//AWB Map 1
	CAMERA_SetReg(0xef,0x01);
	CAMERA_SetReg(0x94,0x80);    //00
	CAMERA_SetReg(0x95,0xc0);    //c0
	CAMERA_SetReg(0x96,0xc0);    //f0
	CAMERA_SetReg(0x97,0xf0);    //c0
	CAMERA_SetReg(0x98,0x78);    //78
	CAMERA_SetReg(0x99,0x78);    //78
	CAMERA_SetReg(0x9a,0x7c);    //7c
	CAMERA_SetReg(0x9b,0x3c);    //3f
	CAMERA_SetReg(0x9c,0x1e);    //1f
	CAMERA_SetReg(0x9d,0x0f);    //0f
	CAMERA_SetReg(0x9e,0x07);    //07
	CAMERA_SetReg(0x9f,0x03);    //03
	CAMERA_SetReg(0xa0,0x00);    //01
	CAMERA_SetReg(0xa1,0xc8);  
	CAMERA_SetReg(0xa2,0x02);
	CAMERA_SetReg(0x93,0x02);

	//20070213
	//AWB Map Direct Write
	CAMERA_SetReg(0xef,0x00);
	CAMERA_SetReg(0x94,0x80);    //00
	CAMERA_SetReg(0x95,0xc0);    //c0
	CAMERA_SetReg(0x96,0xc0);    //f0
	CAMERA_SetReg(0x97,0xf0);    //c0
	CAMERA_SetReg(0x98,0x78);    //78
	CAMERA_SetReg(0x99,0x78);    //78
	CAMERA_SetReg(0xa0,0x7c);    //7c
	CAMERA_SetReg(0xa1,0x3c);    //3f
	CAMERA_SetReg(0xa2,0x1e);    //1f
	CAMERA_SetReg(0xa3,0x0f);    //0f
	CAMERA_SetReg(0xa4,0x07);    //07
	CAMERA_SetReg(0xa5,0x03);    //03
	CAMERA_SetReg(0xa6,0x00);    //01

	//Auto Flicker Correction
	CAMERA_SetReg(0xef,0x01);
	CAMERA_SetReg(0xdd,0x31);
	CAMERA_SetReg(0xcd,0x21);
	CAMERA_SetReg(0xde,0x31);

	//Frame AE Control - L.L 5fps
	//*** Frame 고정시는 아래 부분을 모두 주석처리 할 것.***
	CAMERA_SetReg(0xef,0x00);
	CAMERA_SetReg(0x57,0x00);
	//CAMERA_SetReg(0xef,0x01);
	//CAMERA_SetReg(0xe2,0x18);
	//CAMERA_SetReg(0xe3,0x16);
	//CAMERA_SetReg(0xe8,0x88);
	//******************************************************

	//CAMERA_SetReg(0xef,0x00);
	//CAMERA_SetReg(0x43,0x40);  //20070222  //fixed value
	//CAMERA_SetReg(0x42,0x3d);  //AWB R start point
	//CAMERA_SetReg(0x44,0x6b);  //AWB B start point

	CAMERA_SetReg(0xef,0x01);  //shadow line
	CAMERA_SetReg(0xa5,0x00);

	CAMERA_SetReg(0x70,0x17);   //17 // fix PAL(50Hz);



	CAMERA_SetReg(0xef,0x00);
	CAMERA_SetReg(0xb9,0x02);
	CAMERA_SetReg(0xba,0x80);
	CAMERA_SetReg(0xbb,0x80);
	CAMERA_SetReg(0xbc,0x80); //98
	CAMERA_SetReg(0xbd,0x00); //30


//	CAMERA_SetReg(0xef,0x01);  //shadow line
//	CAMERA_GetReg(0x70, &x);
//	UART_Printf("0x70 = 0x97 --> %x\n", x);

//	CAMERA_SetReg(0xef,0x00);
//	CAMERA_GetReg(0xcb, &x);
//	UART_Printf("0xca = 0x00 --> %x\n", x);


	CAMERA_IICClose();


}


void CAMERA_InitA3AFX_QCIF_30FR(void)
{
	//u32 uHCLKx2;

	SYSC_GetClkInform(); // Get HCLKx2 Frequency
	//uHCLKx2 = g_HCLKx2/1000000;
	
	oCamera.m_ucSlaveAddr = 0xc4;

// 	 CAMERA_IICOpen(100000); 
  CAMERA_IICOpen(16000000); 
	// 3.13 Pearl setting부터 시작.
	// AWB_Low/High_CNT 조정(subsampling에 맞도록)
	//------------------------------------------------------------------
	// Set File 
	// - S5KA3AFX_xshade_061018.xls generates
	// - date : 2007-03-12오후 3:20:55
	//------------------------------------------------------------------

	CAMERA_SetReg(0xef,0x01);
	CAMERA_SetReg(0x70,0x00); //AE,AWB OFF

	//// Page : 00
	//CAMERA_SetReg(0xef,0x00);
	//CAMERA_SetReg(0x0d,0x03);
	//CAMERA_SetReg(0x0e,0x00);
	//CAMERA_SetReg(0x0f,0x1F);
	//CAMERA_SetReg(0x10,0x00);
	//CAMERA_SetReg(0x11,0x37);
	//CAMERA_SetReg(0x12,0x08);
	//CAMERA_SetReg(0x13,0x42);
	//CAMERA_SetReg(0x14,0x00);
	//CAMERA_SetReg(0x15,0x12);
	//CAMERA_SetReg(0x16,0x9E);
	//CAMERA_SetReg(0x19,0x00);
	//CAMERA_SetReg(0x17,0x07);
	//CAMERA_SetReg(0x18,0xE0);
	//CAMERA_SetReg(0x19,0x01);
	//CAMERA_SetReg(0x17,0x07);
	//CAMERA_SetReg(0x18,0xE3);
	//CAMERA_SetReg(0x19,0x02);
	//CAMERA_SetReg(0x17,0x07);
	//CAMERA_SetReg(0x18,0xED);
	//CAMERA_SetReg(0x19,0x03);
	//CAMERA_SetReg(0x17,0x07);
	//CAMERA_SetReg(0x18,0xCA);
	//CAMERA_SetReg(0x19,0x04);
	//CAMERA_SetReg(0x17,0x07);
	//CAMERA_SetReg(0x18,0xD2);
	//CAMERA_SetReg(0x19,0x05);
	//CAMERA_SetReg(0x17,0x07);
	//CAMERA_SetReg(0x18,0xEC);
	//CAMERA_SetReg(0x19,0x06);
	//CAMERA_SetReg(0x17,0x07);
	//CAMERA_SetReg(0x18,0xE5);
	//CAMERA_SetReg(0x19,0x07);
	//CAMERA_SetReg(0x17,0x00);
	//CAMERA_SetReg(0x18,0x12);
	//CAMERA_SetReg(0x19,0x08);
	//CAMERA_SetReg(0x17,0x07);
	//CAMERA_SetReg(0x18,0xE6);
	//CAMERA_SetReg(0x19,0x09);
	//CAMERA_SetReg(0x17,0x00);
	//CAMERA_SetReg(0x18,0x08);
	//CAMERA_SetReg(0x19,0x0A);
	//CAMERA_SetReg(0x17,0x07);
	//CAMERA_SetReg(0x18,0xEA);
	//CAMERA_SetReg(0x19,0x0B);
	//CAMERA_SetReg(0x17,0x07);
	//CAMERA_SetReg(0x18,0xDA);
	//CAMERA_SetReg(0x19,0x0C);
	//CAMERA_SetReg(0x17,0x07);
	//CAMERA_SetReg(0x18,0xE3);
	//CAMERA_SetReg(0x19,0x0D);
	//CAMERA_SetReg(0x17,0x07);
	//CAMERA_SetReg(0x18,0xF9);
	//CAMERA_SetReg(0x19,0x0E);
	//CAMERA_SetReg(0x17,0x00);
	//CAMERA_SetReg(0x18,0x1C);
	//CAMERA_SetReg(0x19,0x0F);
	//CAMERA_SetReg(0x17,0x00);
	//CAMERA_SetReg(0x18,0x12);
	//CAMERA_SetReg(0x19,0x10);
	//CAMERA_SetReg(0x17,0x00);
	//CAMERA_SetReg(0x18,0x18);
	//CAMERA_SetReg(0x19,0x11);
	//CAMERA_SetReg(0x17,0x00);
	//CAMERA_SetReg(0x18,0x08);
	//CAMERA_SetReg(0x19,0x12);
	//CAMERA_SetReg(0x17,0x07);
	//CAMERA_SetReg(0x18,0xEB);
	//CAMERA_SetReg(0x19,0x13);
	//CAMERA_SetReg(0x17,0x07);
	//CAMERA_SetReg(0x18,0xF9);
	//CAMERA_SetReg(0x19,0x14);
	//CAMERA_SetReg(0x17,0x07);
	//CAMERA_SetReg(0x18,0xF4);
	//CAMERA_SetReg(0x19,0x15);
	//CAMERA_SetReg(0x17,0x00);
	//CAMERA_SetReg(0x18,0x1F);
	//CAMERA_SetReg(0x19,0x16);
	//CAMERA_SetReg(0x17,0x00);
	//CAMERA_SetReg(0x18,0x15);
	//CAMERA_SetReg(0x19,0x17);
	//CAMERA_SetReg(0x17,0x00);
	//CAMERA_SetReg(0x18,0x11);
	//CAMERA_SetReg(0x19,0x18);
	//CAMERA_SetReg(0x17,0x00);
	//CAMERA_SetReg(0x18,0x0E);
	//CAMERA_SetReg(0x19,0x19);
	//CAMERA_SetReg(0x17,0x00);
	//CAMERA_SetReg(0x18,0x08);
	//CAMERA_SetReg(0x19,0x1A);
	//CAMERA_SetReg(0x17,0x07);
	//CAMERA_SetReg(0x18,0xFE);
	//CAMERA_SetReg(0x19,0x1B);
	//CAMERA_SetReg(0x17,0x07);
	//CAMERA_SetReg(0x18,0xF4);
	//CAMERA_SetReg(0x19,0x1C);
	//CAMERA_SetReg(0x17,0x00);
	//CAMERA_SetReg(0x18,0x24);
	//CAMERA_SetReg(0x19,0x1D);
	//CAMERA_SetReg(0x17,0x00);
	//CAMERA_SetReg(0x18,0x03);
	//CAMERA_SetReg(0x19,0x1E);
	//CAMERA_SetReg(0x17,0x00);
	//CAMERA_SetReg(0x18,0x02);
	//CAMERA_SetReg(0x19,0x1F);
	//CAMERA_SetReg(0x17,0x07);
	//CAMERA_SetReg(0x18,0xFF);
	//CAMERA_SetReg(0x19,0x20);
	//CAMERA_SetReg(0x17,0x00);
	//CAMERA_SetReg(0x18,0x12);
	//CAMERA_SetReg(0x19,0x21);
	//CAMERA_SetReg(0x17,0x07);
	//CAMERA_SetReg(0x18,0xE5);
	//CAMERA_SetReg(0x19,0x22);
	//CAMERA_SetReg(0x17,0x07);
	//CAMERA_SetReg(0x18,0xD1);
	//CAMERA_SetReg(0x19,0x40);
	//


	//////////////////////////////////////////////////////////

	CAMERA_SetReg(0xef,0x02);   // CIS page
	CAMERA_SetReg(0x02,0x05);   //sub
	//s4b5E   // PCLK driving current max
	//s23d8   // Tx width
	CAMERA_SetReg(0x3a,0x20);   // double shutter off
	CAMERA_SetReg(0x65,0x08);   // CIS mode
	CAMERA_SetReg(0x0a,0x90);   // Display WCW
	CAMERA_SetReg(0x01,0x26);   // E-shutter enable

	CAMERA_SetReg(0x03,0x78);   // S1R END						**** Revised at EVT1
	CAMERA_SetReg(0x05,0x80);   //00   // S1S end value control
	CAMERA_SetReg(0x07,0xE5);   // S2  END
	CAMERA_SetReg(0x09,0x0E);   // S4  END
	CAMERA_SetReg(0x0b,0x87);	//85	//81   // Analog offset
	CAMERA_SetReg(0x13,0xa0);   //20   // R_REF
	//CAMERA_SetReg(0x15,0x80);   // EC COMP 100'b
	CAMERA_SetReg(0x1c,0x52);   //58   //20070304AM //57   // global gain						**** Revised at EVT1
	CAMERA_SetReg(0x24,0x3d);   // i_cds						**** Revised at EVT1
	CAMERA_SetReg(0x26,0x25);   // clamp level control					**** Revised at EVT1
	CAMERA_SetReg(0x29,0x8f);   // CFPN start frame (16)
	CAMERA_SetReg(0x4b,0x5b);   // IO driving current
	CAMERA_SetReg(0x56,0x05);   // Line ADLC
	CAMERA_SetReg(0x58,0x61);   // amp bias current source select (71h : self bias)
	CAMERA_SetReg(0x5f,0x06);   // aps bias current      
	CAMERA_SetReg(0x67,0x3c);   // EC_COMP & SAVG_END
	CAMERA_SetReg(0x68,0x10);   // i_rmp control
	CAMERA_SetReg(0x69,0x13);   // i_mid

	//CAMERA_SetReg(0x58,0x21);	// PLL on
	//CAMERA_SetReg(0x59,0x1A);	// pll_s=0, pll_p=26,
	//CAMERA_SetReg(0x5A,0x1A);	// pll_m=26

	CAMERA_SetReg(0x28,0x8b);   //9B   // CFPN on	
//						**** Revised at EVT1

	//061011
	CAMERA_SetReg(0xef,0x01);
	CAMERA_SetReg(0xd1,0xaa);   //89:16.7MHz, d2:24MHz 64:13MHz dc:25MHz 13MHz //d2	//d2(24MHz) Mclk Value (Mclk - 30d)/100,000 
	//CAMERA_SetReg(0xd1,0x64);	//64(13MHz) Mclk Value (Mclk - 30d)/100,000 
	//CAMERA_SetReg(0xd9,0x15);   //AE saturation CHIP_DEBUG

	//////////
	//page 0
	//////////
	CAMERA_SetReg(0xef,0x00);
	//CAMERA_SetReg(0x09,0x3e);   //gr gain

	CAMERA_SetReg(0xef,0x00);
	CAMERA_SetReg(0x27,0x40);  //08  //GR/GB Correction Limitation Value
	CAMERA_SetReg(0x29,0x60);  //GrGb Edge Threshold Value

	// AE Window    //// AE Window
	CAMERA_SetReg(0xef,0x00);           //sef00
	CAMERA_SetReg(0x55,0x00);           //s5500	//Window diplay off
	CAMERA_SetReg(0x58,0x00);           //s5800
	CAMERA_SetReg(0x59,0x1f);           //s5900
	CAMERA_SetReg(0x5a,0x01);           //s5a02
	CAMERA_SetReg(0x5b,0x44);           //s5b80
	CAMERA_SetReg(0x5c,0x00);           //s5c00
	CAMERA_SetReg(0x5d,0x1d);           //s5d00
	CAMERA_SetReg(0x5e,0x00);           //s5e01
	CAMERA_SetReg(0x5f,0xec);           //s5fe0
	CAMERA_SetReg(0x60,0x00);           //s6000
	CAMERA_SetReg(0x61,0x28);           //s6100
	CAMERA_SetReg(0x62,0x01);           //s6202
	CAMERA_SetReg(0x63,0x3b);           //s6380
	CAMERA_SetReg(0x64,0x00);           //s6400
	CAMERA_SetReg(0x65,0x23);           //s6500
	CAMERA_SetReg(0x66,0x00);           //s6601
	CAMERA_SetReg(0x67,0xe6);           //s67e0
	                //
	CAMERA_SetReg(0xef,0x01);           //sef01	// AE weight
	CAMERA_SetReg(0x77,0x01);           //s7701
	CAMERA_SetReg(0x78,0x01);           //s7801
	CAMERA_SetReg(0x79,0x00);           ////s7903
	CAMERA_SetReg(0x7a,0x00);           ////s7a03
	CAMERA_SetReg(0x7b,0x00);           ////s7b04
	CAMERA_SetReg(0x7c,0x00);           ////s7c04

	CAMERA_SetReg(0xef,0x00);
	//CAMERA_SetReg(0x57,0x18);  // AE Lux min threshold

	CAMERA_SetReg(0xd9,0xc0);  // color edge threshold

	////AWB Luminance check
	////20070212 Added for low light starting env.
	CAMERA_SetReg(0xef,0x00);
	CAMERA_SetReg(0xad,0x03);	//Luminance upper_level[9:8]
	CAMERA_SetReg(0xae,0x20);	//Luminance upper_level[7:0]
	CAMERA_SetReg(0xaf,0x00);	//Luminance lower_level[9:8]
	CAMERA_SetReg(0xb0,0x90);	//Luminance lower_level[7:0]

	/////////////////////////////////////////////////////
	//page 1/////////////////////////////////////////////
	/////////////////////////////////////////////////////
	CAMERA_SetReg(0xef,0x01);
	////20070212 Modified for low light starting env.
	CAMERA_SetReg(0x83,0x72);	//7d	//AWB R/B Max Gain : <0.44>'s max
	CAMERA_SetReg(0x84,0x38);	//30	//AWB R/B Min Gain : <0.42>'s max

	CAMERA_SetReg(0xa8,0x41);   //20070222  //shutter Control
	CAMERA_SetReg(0xa9,0xd4);   //20070222  //Shutter Threshold

	CAMERA_SetReg(0xaf,0x04);   //0f	//20070213PM	//00   //L.L Brightness
	CAMERA_SetReg(0xb4,0x88);	//20070213	//b0   //a0   // N.L contrast
	CAMERA_SetReg(0xb5,0x88);	//20070213	//b0   //a0   // L.L contrast 
	CAMERA_SetReg(0xbc,0x66);   //20070305PM //26	//20070213	//AWB skip point (default:16)

	CAMERA_SetReg(0xb7,0x46);   //58   //68   // N.L color saturation
	CAMERA_SetReg(0xb8,0x46);   //20070305PM //30   //52   // L.L color saturation

	//CAMERA_SetReg(0x7097   //17 // fix PAL(50Hz)
	CAMERA_SetReg(0x71,0x7a);	//20070213PM	//55   //5a   //68   //70   //6f   //4d	//60	//78   //AE target
	CAMERA_SetReg(0x72,0x09);   //20070214PM  //07   //0c   //12   //0a   //05	//12   // AE threshold
	//CAMERA_SetReg(0x73,0x34);	//20070214PM  //AE Step Guard Value 
	CAMERA_SetReg(0x74,0x6b);   //60   //70	//74	//70   // low agc max
	CAMERA_SetReg(0x75,0x40);   // normal agc max
	  
	CAMERA_SetReg(0xa6,0x40);  //80  //N.L sharpness H/V Gain
	CAMERA_SetReg(0xab,0x40);  //38  //18  //60 //N.L NR Edge 
	CAMERA_SetReg(0xad,0x80);  //40  //20  //L.L NR Edge
	//CAMERA_SetReg(0xa3,0x30); //AWB speed //EVT0은 고정되어 있음.(0x03) //EVT1은 변경 가능.

	CAMERA_SetReg(0xcc,0x09);   //08   //Flicker threshold 12
	CAMERA_SetReg(0xd5,0x22);   //23	//32  //Chip_Debug2

	CAMERA_SetReg(0xe0,0x04);   // NT Max
	CAMERA_SetReg(0xe2,0x04);   //07   //18   // Low NT max
	CAMERA_SetReg(0xe1,0x03);   // PAL Max
	CAMERA_SetReg(0xe3,0x03);   //07   //18   // Low PAL max

	CAMERA_SetReg(0xe4,0x00);   // 24MHz VblankH (H.L) //FPS (24MHz, 15fps)
	CAMERA_SetReg(0xe5,0xa2);   //ea   // 24MHz NT VblankL (H.L)
	//CAMERA_SetReg(0xe4,0x00);	// 13MHz VblankH (H.L) //FPS (24MHz, 15fps)
	//CAMERA_SetReg(0xe5,0x2a);	// 13MHz VblankL (H.L)

	CAMERA_SetReg(0xe6,0x00);   //20070215AM // 24MHz VblankH (H.L) //FPS (24MHz, 15fps)
	CAMERA_SetReg(0xe7,0x5e);   //20070215AM //ea   // 24MHz NT VblankL (H.L)

	CAMERA_SetReg(0xe8,0x68);   //44  //84 //97  // ae speed

	CAMERA_SetReg(0xe9,0x84);   //87
	CAMERA_SetReg(0xef,0x00);
	//CAMERA_SetReg(0x8c,0x0e); //skin color detection
	CAMERA_SetReg(0xc8,0x09);   //0a  //YC delay


	////////////////////////////////////////
	// Color Correction -> Shading -> Gamma -> AWB -> Hue
	////////////////////////////////////////

	// Color Correction
	                   
	//CAMERA_SetReg(0xef,0x00);         //	//D65-1	//D65	//CW	//U30-6	//U30	//Old
	//CAMERA_SetReg(0x4c,0x40);         //95	//95	//81	//8a	//50	//62	//56
	//CAMERA_SetReg(0x4d,0x92);         //c2	//c2	//ba	//d2	//af	//a9	//8d
	//CAMERA_SetReg(0x4e,0x0f);         //92	//92	//87	//07	//1f	//07	//89
	//CAMERA_SetReg(0x4f,0x9a);         //9b	//9b	//98	//9e	//9e	//a2	//94
	//CAMERA_SetReg(0x50,0x3b);         //73	//73	//64	//4c	//2e	//47	//5c
	//CAMERA_SetReg(0x51,0x1e);         //98	//98	//8c	//12	//30	//1b	//8b
	//CAMERA_SetReg(0x52,0x82);         //80	//80	//80	//80	//86	//85	//80
	//CAMERA_SetReg(0x53,0xbf);         //b3	//b3	//ad	//cd	//eb	//f2	//91
	//CAMERA_SetReg(0x54,0x7f);         //73	//73	//6d	//8e	//b2	//b8	//51

	////CW
	CAMERA_SetReg(0xef,0x00);
	CAMERA_SetReg(0x4c,0x92);
	CAMERA_SetReg(0x4d,0xcf);
	CAMERA_SetReg(0x4e,0x82);
	CAMERA_SetReg(0x4f,0xad);
	CAMERA_SetReg(0x50,0x6e);
	CAMERA_SetReg(0x51,0x80);
	CAMERA_SetReg(0x52,0x87);
	CAMERA_SetReg(0x53,0xeb);
	CAMERA_SetReg(0x54,0xb2);
	//
	////D65
	//CAMERA_SetReg(0xef,0x00);
	//CAMERA_SetReg(0x4c,0x8a);
	//CAMERA_SetReg(0x4d,0xc2);
	//CAMERA_SetReg(0x4e,0x88);
	//CAMERA_SetReg(0x4f,0x9f);
	//CAMERA_SetReg(0x50,0x72);
	//CAMERA_SetReg(0x51,0x93);
	//CAMERA_SetReg(0x52,0x81);
	//CAMERA_SetReg(0x53,0xca);
	//CAMERA_SetReg(0x54,0x8c);

	//Default
	//CAMERA_SetReg(0xef,0x00);
	//CAMERA_SetReg(0x4c,0x56);
	//CAMERA_SetReg(0x4d,0x8d);
	//CAMERA_SetReg(0x4e,0x89);
	//CAMERA_SetReg(0x4f,0x96);
	//CAMERA_SetReg(0x50,0x5d);
	//CAMERA_SetReg(0x51,0x88);
	//CAMERA_SetReg(0x52,0x94);
	//CAMERA_SetReg(0x53,0x89);
	//CAMERA_SetReg(0x54,0x5d);

	// RGB Shading  //// RGB Shading    ////// Shading2   //// RGB Shading// RGB Shading// RGB Shading
	                //                  ////              //              //              	      //linear//old
	CAMERA_SetReg(0xef,0x00);           //sef00             //sef00         //sef00         //sef00         //00    //00	//00	//00
	CAMERA_SetReg(0x30,0x01);           //s3001             //s3001         //s3001         //s3001         //01    //01	//01	//01
	CAMERA_SetReg(0x32,0x64);           //s323c             //s32b8         //s3200         //s3200         //00    //00	//00	//00
	CAMERA_SetReg(0x36,0x3c);           //s363c             //s3690         //s3600         //s3600         //00    //00	//00	//00
	CAMERA_SetReg(0x3a,0x3c);           //s3a3c             //s3a90         //s3a00         //s3a00         //00    //00	//00	//00
	CAMERA_SetReg(0x33,0x00);           //s3300             //s3301         //s3301         //s3300         //00    //01	//00	//01
	CAMERA_SetReg(0x34,0x00);           //s3400             //s3422         //s3450         //s3400         //32    //50	//00	//50
	CAMERA_SetReg(0x37,0x00);           //s3700             //s3701         //s3701         //s3700         //00    //01	//00	//01
	CAMERA_SetReg(0x38,0x00);           //s3800             //s3822         //s3850         //s3800         //50    //50	//00	//50
	CAMERA_SetReg(0x3b,0x00);           //s3b00             //s3b01         //s3b01         //s3b00         //00    //01	//00	//01
	CAMERA_SetReg(0x3c,0x00);           //s3c00             //s3c22         //s3c50         //s3c00         //00    //50	//00	//50
	CAMERA_SetReg(0x31,0xf8);           //s31f8             //s31f8         //s31df         //s3187         //ff    //ff	//5e	//ff
	CAMERA_SetReg(0x35,0xce);           //s35ce             //s35ce         //s35b8         //s3569         //d7    //da	//54	//da
	CAMERA_SetReg(0x39,0xc8);           //s39ca             //s39ca         //s39a9         //s395a         //be    //e6	//4d	//e6
	CAMERA_SetReg(0x3f,0x00);           //s3f00             //s3f01         //s3f01         //s3f01         //01    //01	//01	//01
	CAMERA_SetReg(0x40,0xa0);           //s40a0             //s4054         //s4040         //s4040         //4a    //40	//40	//40
	CAMERA_SetReg(0x3d,0x00);           //s3d00             //s3d01         //s3d00         //s3d01         //00    //00	//01	//00
	CAMERA_SetReg(0x3e,0x78);           //s3e78             //s3e04         //s3ef0         //s3e18         //fa    //f0	//18	//f0
	    //
	//gamma5
	CAMERA_SetReg(0xef,0x00);
	//Gamma(Red)
	CAMERA_SetReg(0x48,0x00);
	CAMERA_SetReg(0x49,0x00);
	CAMERA_SetReg(0x4A,0x08);
	CAMERA_SetReg(0x4B,0x1B);
	CAMERA_SetReg(0x48,0x01);
	CAMERA_SetReg(0x49,0x00);
	CAMERA_SetReg(0x4A,0x10);
	CAMERA_SetReg(0x4B,0x4E);
	CAMERA_SetReg(0x48,0x02);
	CAMERA_SetReg(0x49,0x00);
	CAMERA_SetReg(0x4A,0x20);
	CAMERA_SetReg(0x4B,0x6F);
	CAMERA_SetReg(0x48,0x03);
	CAMERA_SetReg(0x49,0x00);
	CAMERA_SetReg(0x4A,0x40);
	CAMERA_SetReg(0x4B,0x59);
	CAMERA_SetReg(0x48,0x04);
	CAMERA_SetReg(0x49,0x00);
	CAMERA_SetReg(0x4A,0x80);
	CAMERA_SetReg(0x4B,0x33);
	CAMERA_SetReg(0x48,0x05);
	CAMERA_SetReg(0x49,0x01);
	CAMERA_SetReg(0x4A,0x00);
	CAMERA_SetReg(0x4B,0x2B);
	CAMERA_SetReg(0x48,0x06);
	CAMERA_SetReg(0x49,0x01);
	CAMERA_SetReg(0x4A,0xC0);
	CAMERA_SetReg(0x4B,0x20);
	CAMERA_SetReg(0x48,0x07);
	CAMERA_SetReg(0x49,0x02);
	CAMERA_SetReg(0x4A,0x80);
	CAMERA_SetReg(0x4B,0x19);
	CAMERA_SetReg(0x48,0x08);
	CAMERA_SetReg(0x49,0x03);
	CAMERA_SetReg(0x4A,0x00);
	CAMERA_SetReg(0x4B,0x16);
	CAMERA_SetReg(0x48,0x09);
	CAMERA_SetReg(0x49,0x03);
	CAMERA_SetReg(0x4A,0x80);
	CAMERA_SetReg(0x4B,0x13);
	CAMERA_SetReg(0x48,0x0A);
	CAMERA_SetReg(0x49,0x03);
	CAMERA_SetReg(0x4A,0xC0);
	CAMERA_SetReg(0x4B,0x12);
	CAMERA_SetReg(0x48,0x0B);
	CAMERA_SetReg(0x4B,0x14);
	//Gamma(Green)
	CAMERA_SetReg(0x48,0x10);
	CAMERA_SetReg(0x49,0x00);
	CAMERA_SetReg(0x4A,0x08);
	CAMERA_SetReg(0x4B,0x1B);
	CAMERA_SetReg(0x48,0x11);
	CAMERA_SetReg(0x49,0x00);
	CAMERA_SetReg(0x4A,0x10);
	CAMERA_SetReg(0x4B,0x4E);
	CAMERA_SetReg(0x48,0x12);
	CAMERA_SetReg(0x49,0x00);
	CAMERA_SetReg(0x4A,0x20);
	CAMERA_SetReg(0x4B,0x6F);
	CAMERA_SetReg(0x48,0x13);
	CAMERA_SetReg(0x49,0x00);
	CAMERA_SetReg(0x4A,0x40);
	CAMERA_SetReg(0x4B,0x59);
	CAMERA_SetReg(0x48,0x14);
	CAMERA_SetReg(0x49,0x00);
	CAMERA_SetReg(0x4A,0x80);
	CAMERA_SetReg(0x4B,0x33);
	CAMERA_SetReg(0x48,0x15);
	CAMERA_SetReg(0x49,0x01);
	CAMERA_SetReg(0x4A,0x00);
	CAMERA_SetReg(0x4B,0x2B);
	CAMERA_SetReg(0x48,0x16);
	CAMERA_SetReg(0x49,0x01);
	CAMERA_SetReg(0x4A,0xC0);
	CAMERA_SetReg(0x4B,0x20);
	CAMERA_SetReg(0x48,0x17);
	CAMERA_SetReg(0x49,0x02);
	CAMERA_SetReg(0x4A,0x80);
	CAMERA_SetReg(0x4B,0x19);
	CAMERA_SetReg(0x48,0x18);
	CAMERA_SetReg(0x49,0x03);
	CAMERA_SetReg(0x4A,0x00);
	CAMERA_SetReg(0x4B,0x16);
	CAMERA_SetReg(0x48,0x19);
	CAMERA_SetReg(0x49,0x03);
	CAMERA_SetReg(0x4A,0x80);
	CAMERA_SetReg(0x4B,0x13);
	CAMERA_SetReg(0x48,0x1A);
	CAMERA_SetReg(0x49,0x03);
	CAMERA_SetReg(0x4A,0xC0);
	CAMERA_SetReg(0x4B,0x12);
	CAMERA_SetReg(0x48,0x1B);
	CAMERA_SetReg(0x4B,0x14);
	//Gamma(Blue)
	CAMERA_SetReg(0x48,0x20);
	CAMERA_SetReg(0x49,0x00);
	CAMERA_SetReg(0x4A,0x08);
	CAMERA_SetReg(0x4B,0x1B);
	CAMERA_SetReg(0x48,0x21);
	CAMERA_SetReg(0x49,0x00);
	CAMERA_SetReg(0x4A,0x10);
	CAMERA_SetReg(0x4B,0x4E);
	CAMERA_SetReg(0x48,0x22);
	CAMERA_SetReg(0x49,0x00);
	CAMERA_SetReg(0x4A,0x20);
	CAMERA_SetReg(0x4B,0x6F);
	CAMERA_SetReg(0x48,0x23);
	CAMERA_SetReg(0x49,0x00);
	CAMERA_SetReg(0x4A,0x40);
	CAMERA_SetReg(0x4B,0x59);
	CAMERA_SetReg(0x48,0x24);
	CAMERA_SetReg(0x49,0x00);
	CAMERA_SetReg(0x4A,0x80);
	CAMERA_SetReg(0x4B,0x33);
	CAMERA_SetReg(0x48,0x25);
	CAMERA_SetReg(0x49,0x01);
	CAMERA_SetReg(0x4A,0x00);
	CAMERA_SetReg(0x4B,0x2B);
	CAMERA_SetReg(0x48,0x26);
	CAMERA_SetReg(0x49,0x01);
	CAMERA_SetReg(0x4A,0xC0);
	CAMERA_SetReg(0x4B,0x20);
	CAMERA_SetReg(0x48,0x27);
	CAMERA_SetReg(0x49,0x02);
	CAMERA_SetReg(0x4A,0x80);
	CAMERA_SetReg(0x4B,0x19);
	CAMERA_SetReg(0x48,0x28);
	CAMERA_SetReg(0x49,0x03);
	CAMERA_SetReg(0x4A,0x00);
	CAMERA_SetReg(0x4B,0x16);
	CAMERA_SetReg(0x48,0x29);
	CAMERA_SetReg(0x49,0x03);
	CAMERA_SetReg(0x4A,0x80);
	CAMERA_SetReg(0x4B,0x13);
	CAMERA_SetReg(0x48,0x2A);
	CAMERA_SetReg(0x49,0x03);
	CAMERA_SetReg(0x4A,0xC0);
	CAMERA_SetReg(0x4B,0x12);
	CAMERA_SetReg(0x48,0x2B);
	CAMERA_SetReg(0x4B,0x14);
	CAMERA_SetReg(0x48,0x80);

	//AWB
	//CAMERA_SetReg(0xef,0x00);
	//CAMERA_SetReg(0x93,0x01);   // AWB map
	//CAMERA_SetReg(0xa0,0x0f);
	//CAMERA_SetReg(0x9f,0x0f);
	//CAMERA_SetReg(0x93,0x00);

	//AWB mode : auto(??)
	//CAMERA_SetReg(0xef,0x01);
	//CAMERA_SetReg(0x80,0x00);

	// Hue

	CAMERA_SetReg(0xef,0x00);
	CAMERA_SetReg(0x8c,0x0c);	//Skin Color Detection On/Off
	CAMERA_SetReg(0x74,0x01);	//20070214 //00	//Hue & Saturation Off=00, On=01
	CAMERA_SetReg(0x79,0x46);   //20070214 //58	//Saturation Gain
	CAMERA_SetReg(0x75,0x01);	//Hue & Saturation Cosine 1
	CAMERA_SetReg(0x76,0x00);   //a0   //20070214 //ff	//Hue & Saturation Cosine 2
	CAMERA_SetReg(0x77,0x02);	//Hue & Saturation Sine 1
	CAMERA_SetReg(0x78,0x00);   //20070214 //11	//Hue & Saturation Sine 2
	CAMERA_SetReg(0x72,0x88);	//20070213	//b0   //90	//Contrast Gain
	//CAMERA_SetReg(0x73,0x89);	//Brightness Gain
	CAMERA_SetReg(0xd8,0x00);   //Edge color suppress 

	//CAMERA_SetReg(0xef,0x02);
	//CAMERA_SetReg(0x0f,0x40); 

	//AWB white's count low 0f00 --> 0708
	CAMERA_SetReg(0xef,0x01);
	CAMERA_SetReg(0x85,0x00);   //07	//0f	//07
	CAMERA_SetReg(0x86,0x3c);   //2d	//08	//00	//08
	CAMERA_SetReg(0x87,0x13);   //0e
	CAMERA_SetReg(0x88,0xc0);   //d0

	//AWB boundary
	CAMERA_SetReg(0xef,0x00);
	CAMERA_SetReg(0xb1,0x00);
	CAMERA_SetReg(0xb2,0x00);
	CAMERA_SetReg(0xb3,0x00);
	CAMERA_SetReg(0xb4,0x00);
	CAMERA_SetReg(0xb5,0x00);
	CAMERA_SetReg(0xb6,0xf0);
	CAMERA_SetReg(0xb7,0x01);
	CAMERA_SetReg(0xb8,0x40);

	// Bad Pixel Replacement

	//CAMERA_SetReg(0xef,0x00);
	//CAMERA_SetReg(0x1a,0x01);
	//CAMERA_SetReg(0x1b,0x00);
	//CAMERA_SetReg(0x1e,0x00);
	//CAMERA_SetReg(0x1c,0x00);
	//CAMERA_SetReg(0x1f,0x00);
	//CAMERA_SetReg(0x1d,0x00);
	//CAMERA_SetReg(0x20,0x00);

	CAMERA_SetReg(0xef,0x01);
	CAMERA_SetReg(0xb9,0x08);  //10  //30	//BPR Gain Setting
	CAMERA_SetReg(0xba,0x00);
	CAMERA_SetReg(0xbd,0x00);

	CAMERA_SetReg(0xeb,0x60);  //40  //6f   //Low Light Indication AGC

	//20070213
	//AWB Map 1
	CAMERA_SetReg(0xef,0x01);
	CAMERA_SetReg(0x94,0x80);    //00
	CAMERA_SetReg(0x95,0xc0);    //c0
	CAMERA_SetReg(0x96,0xc0);    //f0
	CAMERA_SetReg(0x97,0xf0);    //c0
	CAMERA_SetReg(0x98,0x78);    //78
	CAMERA_SetReg(0x99,0x78);    //78
	CAMERA_SetReg(0x9a,0x7c);    //7c
	CAMERA_SetReg(0x9b,0x3c);    //3f
	CAMERA_SetReg(0x9c,0x1e);    //1f
	CAMERA_SetReg(0x9d,0x0f);    //0f
	CAMERA_SetReg(0x9e,0x07);    //07
	CAMERA_SetReg(0x9f,0x03);    //03
	CAMERA_SetReg(0xa0,0x00);    //01
	CAMERA_SetReg(0xa1,0xc8);  
	CAMERA_SetReg(0xa2,0x02);
	CAMERA_SetReg(0x93,0x82);    //AWB window //02

	//20070213
	//AWB Map Direct Write
	CAMERA_SetReg(0xef,0x00);
	CAMERA_SetReg(0x94,0x80);    //00
	CAMERA_SetReg(0x95,0xc0);    //c0
	CAMERA_SetReg(0x96,0xc0);    //f0
	CAMERA_SetReg(0x97,0xf0);    //c0
	CAMERA_SetReg(0x98,0x78);    //78
	CAMERA_SetReg(0x99,0x78);    //78
	CAMERA_SetReg(0xa0,0x7c);    //7c
	CAMERA_SetReg(0xa1,0x3c);    //3f
	CAMERA_SetReg(0xa2,0x1e);    //1f
	CAMERA_SetReg(0xa3,0x0f);    //0f
	CAMERA_SetReg(0xa4,0x07);    //07
	CAMERA_SetReg(0xa5,0x03);    //03
	CAMERA_SetReg(0xa6,0x00);    //01

	//Auto Flicker Correction
	CAMERA_SetReg(0xef,0x01);
	CAMERA_SetReg(0xdd,0x31);
	CAMERA_SetReg(0xcd,0x21);
	CAMERA_SetReg(0xde,0x31);

	//Frame AE Control - L.L 5fps
	//*** Frame 고정시는 아래 부분을 모두 주석처리 할 것.***
	CAMERA_SetReg(0xef,0x00);
	CAMERA_SetReg(0x57,0x00);
	//CAMERA_SetReg(0xef,0x01);
	//CAMERA_SetReg(0xe2,0x18);
	//CAMERA_SetReg(0xe3,0x16);
	//CAMERA_SetReg(0xe8,0x88);
	//******************************************************

	//CAMERA_SetReg(0xef,0x00);
	//CAMERA_SetReg(0x43,0x40);  //20070222  //fixed value
	//CAMERA_SetReg(0x42,0x3d);  //AWB R start point
	//CAMERA_SetReg(0x44,0x6b);  //AWB B start point

	CAMERA_SetReg(0xef,0x01);  //shadow line
	CAMERA_SetReg(0xa5,0x00);

	CAMERA_SetReg(0xef,0x00);
	CAMERA_SetReg(0xb9,0x02);
	CAMERA_SetReg(0xba,0x80);
	CAMERA_SetReg(0xbb,0x80);
	CAMERA_SetReg(0xbc,0x80); //98
	CAMERA_SetReg(0xbd,0x00); //30

	//
	CAMERA_SetReg(0xef,0x00);
	CAMERA_SetReg(0x04,0x00);
	CAMERA_SetReg(0x05,0xf8);
	CAMERA_SetReg(0xc2,0x08);
	CAMERA_SetReg(0xc3,0x08);
	CAMERA_SetReg(0xc4,0x00);
	CAMERA_SetReg(0xc5,0xf0);
	CAMERA_SetReg(0xc6,0x01);
	CAMERA_SetReg(0xc7,0x40);

	CAMERA_SetReg(0xef,0x02);
	CAMERA_SetReg(0x0a,0x9a);
	CAMERA_SetReg(0x08,0xf7);

	CAMERA_SetReg(0x1a,0x01);
	CAMERA_SetReg(0x1b,0xe6);

	CAMERA_SetReg(0xef,0x01);
	CAMERA_SetReg(0xca,0x01);
	CAMERA_SetReg(0xcb,0xe6);

	CAMERA_SetReg(0x70,0x17);   //17 // fix PAL(50Hz)

	//QVGA to QCIF
	CAMERA_SetReg(0xef,0x00);
	CAMERA_SetReg(0x06,0x01);
	CAMERA_SetReg(0x07,0x48);
	CAMERA_SetReg(0x04,0x00);
	CAMERA_SetReg(0x05,0xf8);

	CAMERA_SetReg(0x7d,0x01);
	CAMERA_SetReg(0x7e,0x48);
	CAMERA_SetReg(0x7b,0x00);
	CAMERA_SetReg(0x7c,0xf8);

	CAMERA_SetReg(0x81,0x00);
	CAMERA_SetReg(0x82,0xc0); //b0
	CAMERA_SetReg(0x7f,0x00);
	CAMERA_SetReg(0x80,0xa0); //90

	CAMERA_SetReg(0xc6,0x00);
	CAMERA_SetReg(0xc7,0xb0);
	CAMERA_SetReg(0xc4,0x00);
	CAMERA_SetReg(0xc5,0x90);

	CAMERA_IICClose();


}


void CAMERA_InitA3AFX_QCIF_15FR(void)
{
	//u32 uHCLKx2;

	SYSC_GetClkInform(); // Get HCLKx2 Frequency
	//uHCLKx2 = g_HCLKx2/1000000;
	
	oCamera.m_ucSlaveAddr = 0xc4;

// 	 CAMERA_IICOpen(100000); 
  CAMERA_IICOpen(16000000); 
	// 3.13 Pearl setting부터 시작.
	// AWB_Low/High_CNT 조정(subsampling에 맞도록)
	//------------------------------------------------------------------
	// Set File 
	// - S5KA3AFX_xshade_061018.xls generates
	// - date : 2007-03-12오후 3:20:55
	//------------------------------------------------------------------

	CAMERA_SetReg(0xef,0x01);
	CAMERA_SetReg(0x70,0x00); //AE,AWB OFF

	//// Page : 00
	//CAMERA_SetReg(0xef,0x00);
	//CAMERA_SetReg(0x0d,0x03);
	//CAMERA_SetReg(0x0e,0x00);
	//CAMERA_SetReg(0x0f,0x1F);
	//CAMERA_SetReg(0x10,0x00);
	//CAMERA_SetReg(0x11,0x37);
	//CAMERA_SetReg(0x12,0x08);
	//CAMERA_SetReg(0x13,0x42);
	//CAMERA_SetReg(0x14,0x00);
	//CAMERA_SetReg(0x15,0x12);
	//CAMERA_SetReg(0x16,0x9E);
	//CAMERA_SetReg(0x19,0x00);
	//CAMERA_SetReg(0x17,0x07);
	//CAMERA_SetReg(0x18,0xE0);
	//CAMERA_SetReg(0x19,0x01);
	//CAMERA_SetReg(0x17,0x07);
	//CAMERA_SetReg(0x18,0xE3);
	//CAMERA_SetReg(0x19,0x02);
	//CAMERA_SetReg(0x17,0x07);
	//CAMERA_SetReg(0x18,0xED);
	//CAMERA_SetReg(0x19,0x03);
	//CAMERA_SetReg(0x17,0x07);
	//CAMERA_SetReg(0x18,0xCA);
	//CAMERA_SetReg(0x19,0x04);
	//CAMERA_SetReg(0x17,0x07);
	//CAMERA_SetReg(0x18,0xD2);
	//CAMERA_SetReg(0x19,0x05);
	//CAMERA_SetReg(0x17,0x07);
	//CAMERA_SetReg(0x18,0xEC);
	//CAMERA_SetReg(0x19,0x06);
	//CAMERA_SetReg(0x17,0x07);
	//CAMERA_SetReg(0x18,0xE5);
	//CAMERA_SetReg(0x19,0x07);
	//CAMERA_SetReg(0x17,0x00);
	//CAMERA_SetReg(0x18,0x12);
	//CAMERA_SetReg(0x19,0x08);
	//CAMERA_SetReg(0x17,0x07);
	//CAMERA_SetReg(0x18,0xE6);
	//CAMERA_SetReg(0x19,0x09);
	//CAMERA_SetReg(0x17,0x00);
	//CAMERA_SetReg(0x18,0x08);
	//CAMERA_SetReg(0x19,0x0A);
	//CAMERA_SetReg(0x17,0x07);
	//CAMERA_SetReg(0x18,0xEA);
	//CAMERA_SetReg(0x19,0x0B);
	//CAMERA_SetReg(0x17,0x07);
	//CAMERA_SetReg(0x18,0xDA);
	//CAMERA_SetReg(0x19,0x0C);
	//CAMERA_SetReg(0x17,0x07);
	//CAMERA_SetReg(0x18,0xE3);
	//CAMERA_SetReg(0x19,0x0D);
	//CAMERA_SetReg(0x17,0x07);
	//CAMERA_SetReg(0x18,0xF9);
	//CAMERA_SetReg(0x19,0x0E);
	//CAMERA_SetReg(0x17,0x00);
	//CAMERA_SetReg(0x18,0x1C);
	//CAMERA_SetReg(0x19,0x0F);
	//CAMERA_SetReg(0x17,0x00);
	//CAMERA_SetReg(0x18,0x12);
	//CAMERA_SetReg(0x19,0x10);
	//CAMERA_SetReg(0x17,0x00);
	//CAMERA_SetReg(0x18,0x18);
	//CAMERA_SetReg(0x19,0x11);
	//CAMERA_SetReg(0x17,0x00);
	//CAMERA_SetReg(0x18,0x08);
	//CAMERA_SetReg(0x19,0x12);
	//CAMERA_SetReg(0x17,0x07);
	//CAMERA_SetReg(0x18,0xEB);
	//CAMERA_SetReg(0x19,0x13);
	//CAMERA_SetReg(0x17,0x07);
	//CAMERA_SetReg(0x18,0xF9);
	//CAMERA_SetReg(0x19,0x14);
	//CAMERA_SetReg(0x17,0x07);
	//CAMERA_SetReg(0x18,0xF4);
	//CAMERA_SetReg(0x19,0x15);
	//CAMERA_SetReg(0x17,0x00);
	//CAMERA_SetReg(0x18,0x1F);
	//CAMERA_SetReg(0x19,0x16);
	//CAMERA_SetReg(0x17,0x00);
	//CAMERA_SetReg(0x18,0x15);
	//CAMERA_SetReg(0x19,0x17);
	//CAMERA_SetReg(0x17,0x00);
	//CAMERA_SetReg(0x18,0x11);
	//CAMERA_SetReg(0x19,0x18);
	//CAMERA_SetReg(0x17,0x00);
	//CAMERA_SetReg(0x18,0x0E);
	//CAMERA_SetReg(0x19,0x19);
	//CAMERA_SetReg(0x17,0x00);
	//CAMERA_SetReg(0x18,0x08);
	//CAMERA_SetReg(0x19,0x1A);
	//CAMERA_SetReg(0x17,0x07);
	//CAMERA_SetReg(0x18,0xFE);
	//CAMERA_SetReg(0x19,0x1B);
	//CAMERA_SetReg(0x17,0x07);
	//CAMERA_SetReg(0x18,0xF4);
	//CAMERA_SetReg(0x19,0x1C);
	//CAMERA_SetReg(0x17,0x00);
	//CAMERA_SetReg(0x18,0x24);
	//CAMERA_SetReg(0x19,0x1D);
	//CAMERA_SetReg(0x17,0x00);
	//CAMERA_SetReg(0x18,0x03);
	//CAMERA_SetReg(0x19,0x1E);
	//CAMERA_SetReg(0x17,0x00);
	//CAMERA_SetReg(0x18,0x02);
	//CAMERA_SetReg(0x19,0x1F);
	//CAMERA_SetReg(0x17,0x07);
	//CAMERA_SetReg(0x18,0xFF);
	//CAMERA_SetReg(0x19,0x20);
	//CAMERA_SetReg(0x17,0x00);
	//CAMERA_SetReg(0x18,0x12);
	//CAMERA_SetReg(0x19,0x21);
	//CAMERA_SetReg(0x17,0x07);
	//CAMERA_SetReg(0x18,0xE5);
	//CAMERA_SetReg(0x19,0x22);
	//CAMERA_SetReg(0x17,0x07);
	//CAMERA_SetReg(0x18,0xD1);
	//CAMERA_SetReg(0x19,0x40);
	//


	//////////////////////////////////////////////////////////

	CAMERA_SetReg(0xef,0x02);   // CIS page
	CAMERA_SetReg(0x02,0x05);   //sub
	//s4b5E   // PCLK driving current max
	//s23d8   // Tx width
	CAMERA_SetReg(0x3a,0x20);   // double shutter off
	CAMERA_SetReg(0x65,0x08);   // CIS mode
	CAMERA_SetReg(0x0a,0x90);   // Display WCW
	CAMERA_SetReg(0x01,0x26);   // E-shutter enable

	CAMERA_SetReg(0x03,0x78);   // S1R END						**** Revised at EVT1
	CAMERA_SetReg(0x05,0x80);   //00   // S1S end value control
	CAMERA_SetReg(0x07,0xE5);   // S2  END
	CAMERA_SetReg(0x09,0x0E);   // S4  END
	CAMERA_SetReg(0x0b,0x87);	//85	//81   // Analog offset
	CAMERA_SetReg(0x13,0xa0);   //20   // R_REF
	//CAMERA_SetReg(0x15,0x80);   // EC COMP 100'b
	CAMERA_SetReg(0x1c,0x52);   //58   //20070304AM //57   // global gain						**** Revised at EVT1
	CAMERA_SetReg(0x24,0x3d);   // i_cds						**** Revised at EVT1
	CAMERA_SetReg(0x26,0x25);   // clamp level control					**** Revised at EVT1
	CAMERA_SetReg(0x29,0x8f);   // CFPN start frame (16)
	CAMERA_SetReg(0x4b,0x5b);   // IO driving current
	CAMERA_SetReg(0x56,0x05);   // Line ADLC
	CAMERA_SetReg(0x58,0x61);   // amp bias current source select (71h : self bias)
	CAMERA_SetReg(0x5f,0x06);   // aps bias current      
	CAMERA_SetReg(0x67,0x3c);   // EC_COMP & SAVG_END
	CAMERA_SetReg(0x68,0x10);   // i_rmp control
	CAMERA_SetReg(0x69,0x13);   // i_mid

	//CAMERA_SetReg(0x58,0x21);	// PLL on
	//CAMERA_SetReg(0x59,0x1A);	// pll_s=0, pll_p=26,
	//CAMERA_SetReg(0x5A,0x1A);	// pll_m=26

	CAMERA_SetReg(0x28,0x8b);   //9B   // CFPN on	
//						**** Revised at EVT1

	//061011
	CAMERA_SetReg(0xef,0x01);
	CAMERA_SetReg(0xd1,0xaa);   //89:16.7MHz, d2:24MHz 64:13MHz dc:25MHz 13MHz //d2	//d2(24MHz) Mclk Value (Mclk - 30d)/100,000 
	//CAMERA_SetReg(0xd1,0x64);	//64(13MHz) Mclk Value (Mclk - 30d)/100,000 
	//CAMERA_SetReg(0xd9,0x15);   //AE saturation CHIP_DEBUG

	//////////
	//page 0
	//////////
	CAMERA_SetReg(0xef,0x00);
	//CAMERA_SetReg(0x09,0x3e);   //gr gain

	CAMERA_SetReg(0xef,0x00);
	CAMERA_SetReg(0x27,0x40);  //08  //GR/GB Correction Limitation Value
	CAMERA_SetReg(0x29,0x60);  //GrGb Edge Threshold Value

	// AE Window    //// AE Window
	CAMERA_SetReg(0xef,0x00);           //sef00
	CAMERA_SetReg(0x55,0x00);           //s5500	//Window diplay off
	CAMERA_SetReg(0x58,0x00);           //s5800
	CAMERA_SetReg(0x59,0x1f);           //s5900
	CAMERA_SetReg(0x5a,0x01);           //s5a02
	CAMERA_SetReg(0x5b,0x44);           //s5b80
	CAMERA_SetReg(0x5c,0x00);           //s5c00
	CAMERA_SetReg(0x5d,0x1d);           //s5d00
	CAMERA_SetReg(0x5e,0x00);           //s5e01
	CAMERA_SetReg(0x5f,0xec);           //s5fe0
	CAMERA_SetReg(0x60,0x00);           //s6000
	CAMERA_SetReg(0x61,0x28);           //s6100
	CAMERA_SetReg(0x62,0x01);           //s6202
	CAMERA_SetReg(0x63,0x3b);           //s6380
	CAMERA_SetReg(0x64,0x00);           //s6400
	CAMERA_SetReg(0x65,0x23);           //s6500
	CAMERA_SetReg(0x66,0x00);           //s6601
	CAMERA_SetReg(0x67,0xe6);           //s67e0
	                //
	CAMERA_SetReg(0xef,0x01);           //sef01	// AE weight
	CAMERA_SetReg(0x77,0x01);           //s7701
	CAMERA_SetReg(0x78,0x01);           //s7801
	CAMERA_SetReg(0x79,0x00);           ////s7903
	CAMERA_SetReg(0x7a,0x00);           ////s7a03
	CAMERA_SetReg(0x7b,0x00);           ////s7b04
	CAMERA_SetReg(0x7c,0x00);           ////s7c04

	CAMERA_SetReg(0xef,0x00);
	//CAMERA_SetReg(0x57,0x18);  // AE Lux min threshold

	CAMERA_SetReg(0xd9,0xc0);  // color edge threshold

	////AWB Luminance check
	////20070212 Added for low light starting env.
	CAMERA_SetReg(0xef,0x00);
	CAMERA_SetReg(0xad,0x03);	//Luminance upper_level[9:8]
	CAMERA_SetReg(0xae,0x20);	//Luminance upper_level[7:0]
	CAMERA_SetReg(0xaf,0x00);	//Luminance lower_level[9:8]
	CAMERA_SetReg(0xb0,0x90);	//Luminance lower_level[7:0]

	/////////////////////////////////////////////////////
	//page 1/////////////////////////////////////////////
	/////////////////////////////////////////////////////
	CAMERA_SetReg(0xef,0x01);
	////20070212 Modified for low light starting env.
	CAMERA_SetReg(0x83,0x72);	//7d	//AWB R/B Max Gain : <0.44>'s max
	CAMERA_SetReg(0x84,0x38);	//30	//AWB R/B Min Gain : <0.42>'s max

	CAMERA_SetReg(0xa8,0x41);   //20070222  //shutter Control
	CAMERA_SetReg(0xa9,0xd4);   //20070222  //Shutter Threshold

	CAMERA_SetReg(0xaf,0x04);   //0f	//20070213PM	//00   //L.L Brightness
	CAMERA_SetReg(0xb4,0x88);	//20070213	//b0   //a0   // N.L contrast
	CAMERA_SetReg(0xb5,0x88);	//20070213	//b0   //a0   // L.L contrast 
	CAMERA_SetReg(0xbc,0x66);   //20070305PM //26	//20070213	//AWB skip point (default:16)

	CAMERA_SetReg(0xb7,0x46);   //58   //68   // N.L color saturation
	CAMERA_SetReg(0xb8,0x46);   //20070305PM //30   //52   // L.L color saturation

	//CAMERA_SetReg(0x7097   //17 // fix PAL(50Hz)
	CAMERA_SetReg(0x71,0x7a);	//20070213PM	//55   //5a   //68   //70   //6f   //4d	//60	//78   //AE target
	CAMERA_SetReg(0x72,0x09);   //20070214PM  //07   //0c   //12   //0a   //05	//12   // AE threshold
	//CAMERA_SetReg(0x73,0x34);	//20070214PM  //AE Step Guard Value 
	CAMERA_SetReg(0x74,0x6b);   //60   //70	//74	//70   // low agc max
	CAMERA_SetReg(0x75,0x40);   // normal agc max
	  
	CAMERA_SetReg(0xa6,0x40);  //80  //N.L sharpness H/V Gain
	CAMERA_SetReg(0xab,0x40);  //38  //18  //60 //N.L NR Edge 
	CAMERA_SetReg(0xad,0x80);  //40  //20  //L.L NR Edge
	//CAMERA_SetReg(0xa3,0x30); //AWB speed //EVT0은 고정되어 있음.(0x03) //EVT1은 변경 가능.

	CAMERA_SetReg(0xcc,0x09);   //08   //Flicker threshold 12
	CAMERA_SetReg(0xd5,0x22);   //23	//32  //Chip_Debug2

	CAMERA_SetReg(0xe0,0x08);   // NT Max
	CAMERA_SetReg(0xe2,0x08);   //07   //18   // Low NT max
	CAMERA_SetReg(0xe1,0x03);   // PAL Max
	CAMERA_SetReg(0xe3,0x03);   //07   //18   // Low PAL max

	CAMERA_SetReg(0xe4,0x02);   // 24MHz VblankH (H.L) //FPS (24MHz, 15fps)
	CAMERA_SetReg(0xe5,0x3c);   //ea   // 24MHz NT VblankL (H.L)
	//CAMERA_SetReg(0xe4,0x00);	// 13MHz VblankH (H.L) //FPS (24MHz, 15fps)
	//CAMERA_SetReg(0xe5,0x2a);	// 13MHz VblankL (H.L)

	CAMERA_SetReg(0xe6,0x00);   //20070215AM // 24MHz VblankH (H.L) //FPS (24MHz, 15fps)
	CAMERA_SetReg(0xe7,0x5e);   //20070215AM //ea   // 24MHz NT VblankL (H.L)

	CAMERA_SetReg(0xe8,0x68);   //44  //84 //97  // ae speed

	CAMERA_SetReg(0xe9,0x84);   //87
	CAMERA_SetReg(0xef,0x00);
	//CAMERA_SetReg(0x8c,0x0e); //skin color detection
	CAMERA_SetReg(0xc8,0x09);   //0a  //YC delay


	////////////////////////////////////////
	// Color Correction -> Shading -> Gamma -> AWB -> Hue
	////////////////////////////////////////

	// Color Correction
	                   
	//CAMERA_SetReg(0xef,0x00);         //	//D65-1	//D65	//CW	//U30-6	//U30	//Old
	//CAMERA_SetReg(0x4c,0x40);         //95	//95	//81	//8a	//50	//62	//56
	//CAMERA_SetReg(0x4d,0x92);         //c2	//c2	//ba	//d2	//af	//a9	//8d
	//CAMERA_SetReg(0x4e,0x0f);         //92	//92	//87	//07	//1f	//07	//89
	//CAMERA_SetReg(0x4f,0x9a);         //9b	//9b	//98	//9e	//9e	//a2	//94
	//CAMERA_SetReg(0x50,0x3b);         //73	//73	//64	//4c	//2e	//47	//5c
	//CAMERA_SetReg(0x51,0x1e);         //98	//98	//8c	//12	//30	//1b	//8b
	//CAMERA_SetReg(0x52,0x82);         //80	//80	//80	//80	//86	//85	//80
	//CAMERA_SetReg(0x53,0xbf);         //b3	//b3	//ad	//cd	//eb	//f2	//91
	//CAMERA_SetReg(0x54,0x7f);         //73	//73	//6d	//8e	//b2	//b8	//51

	////CW
	CAMERA_SetReg(0xef,0x00);
	CAMERA_SetReg(0x4c,0x92);
	CAMERA_SetReg(0x4d,0xcf);
	CAMERA_SetReg(0x4e,0x82);
	CAMERA_SetReg(0x4f,0xad);
	CAMERA_SetReg(0x50,0x6e);
	CAMERA_SetReg(0x51,0x80);
	CAMERA_SetReg(0x52,0x87);
	CAMERA_SetReg(0x53,0xeb);
	CAMERA_SetReg(0x54,0xb2);
	//
	////D65
	//CAMERA_SetReg(0xef,0x00);
	//CAMERA_SetReg(0x4c,0x8a);
	//CAMERA_SetReg(0x4d,0xc2);
	//CAMERA_SetReg(0x4e,0x88);
	//CAMERA_SetReg(0x4f,0x9f);
	//CAMERA_SetReg(0x50,0x72);
	//CAMERA_SetReg(0x51,0x93);
	//CAMERA_SetReg(0x52,0x81);
	//CAMERA_SetReg(0x53,0xca);
	//CAMERA_SetReg(0x54,0x8c);

	//Default
	//CAMERA_SetReg(0xef,0x00);
	//CAMERA_SetReg(0x4c,0x56);
	//CAMERA_SetReg(0x4d,0x8d);
	//CAMERA_SetReg(0x4e,0x89);
	//CAMERA_SetReg(0x4f,0x96);
	//CAMERA_SetReg(0x50,0x5d);
	//CAMERA_SetReg(0x51,0x88);
	//CAMERA_SetReg(0x52,0x94);
	//CAMERA_SetReg(0x53,0x89);
	//CAMERA_SetReg(0x54,0x5d);

	// RGB Shading  //// RGB Shading    ////// Shading2   //// RGB Shading// RGB Shading// RGB Shading
	                //                  ////              //              //              	      //linear//old
	CAMERA_SetReg(0xef,0x00);           //sef00             //sef00         //sef00         //sef00         //00    //00	//00	//00
	CAMERA_SetReg(0x30,0x01);           //s3001             //s3001         //s3001         //s3001         //01    //01	//01	//01
	CAMERA_SetReg(0x32,0x64);           //s323c             //s32b8         //s3200         //s3200         //00    //00	//00	//00
	CAMERA_SetReg(0x36,0x3c);           //s363c             //s3690         //s3600         //s3600         //00    //00	//00	//00
	CAMERA_SetReg(0x3a,0x3c);           //s3a3c             //s3a90         //s3a00         //s3a00         //00    //00	//00	//00
	CAMERA_SetReg(0x33,0x00);           //s3300             //s3301         //s3301         //s3300         //00    //01	//00	//01
	CAMERA_SetReg(0x34,0x00);           //s3400             //s3422         //s3450         //s3400         //32    //50	//00	//50
	CAMERA_SetReg(0x37,0x00);           //s3700             //s3701         //s3701         //s3700         //00    //01	//00	//01
	CAMERA_SetReg(0x38,0x00);           //s3800             //s3822         //s3850         //s3800         //50    //50	//00	//50
	CAMERA_SetReg(0x3b,0x00);           //s3b00             //s3b01         //s3b01         //s3b00         //00    //01	//00	//01
	CAMERA_SetReg(0x3c,0x00);           //s3c00             //s3c22         //s3c50         //s3c00         //00    //50	//00	//50
	CAMERA_SetReg(0x31,0xf8);           //s31f8             //s31f8         //s31df         //s3187         //ff    //ff	//5e	//ff
	CAMERA_SetReg(0x35,0xce);           //s35ce             //s35ce         //s35b8         //s3569         //d7    //da	//54	//da
	CAMERA_SetReg(0x39,0xc8);           //s39ca             //s39ca         //s39a9         //s395a         //be    //e6	//4d	//e6
	CAMERA_SetReg(0x3f,0x00);           //s3f00             //s3f01         //s3f01         //s3f01         //01    //01	//01	//01
	CAMERA_SetReg(0x40,0xa0);           //s40a0             //s4054         //s4040         //s4040         //4a    //40	//40	//40
	CAMERA_SetReg(0x3d,0x00);           //s3d00             //s3d01         //s3d00         //s3d01         //00    //00	//01	//00
	CAMERA_SetReg(0x3e,0x78);           //s3e78             //s3e04         //s3ef0         //s3e18         //fa    //f0	//18	//f0
	    //
	//gamma5
	CAMERA_SetReg(0xef,0x00);
	//Gamma(Red)
	CAMERA_SetReg(0x48,0x00);
	CAMERA_SetReg(0x49,0x00);
	CAMERA_SetReg(0x4A,0x08);
	CAMERA_SetReg(0x4B,0x1B);
	CAMERA_SetReg(0x48,0x01);
	CAMERA_SetReg(0x49,0x00);
	CAMERA_SetReg(0x4A,0x10);
	CAMERA_SetReg(0x4B,0x4E);
	CAMERA_SetReg(0x48,0x02);
	CAMERA_SetReg(0x49,0x00);
	CAMERA_SetReg(0x4A,0x20);
	CAMERA_SetReg(0x4B,0x6F);
	CAMERA_SetReg(0x48,0x03);
	CAMERA_SetReg(0x49,0x00);
	CAMERA_SetReg(0x4A,0x40);
	CAMERA_SetReg(0x4B,0x59);
	CAMERA_SetReg(0x48,0x04);
	CAMERA_SetReg(0x49,0x00);
	CAMERA_SetReg(0x4A,0x80);
	CAMERA_SetReg(0x4B,0x33);
	CAMERA_SetReg(0x48,0x05);
	CAMERA_SetReg(0x49,0x01);
	CAMERA_SetReg(0x4A,0x00);
	CAMERA_SetReg(0x4B,0x2B);
	CAMERA_SetReg(0x48,0x06);
	CAMERA_SetReg(0x49,0x01);
	CAMERA_SetReg(0x4A,0xC0);
	CAMERA_SetReg(0x4B,0x20);
	CAMERA_SetReg(0x48,0x07);
	CAMERA_SetReg(0x49,0x02);
	CAMERA_SetReg(0x4A,0x80);
	CAMERA_SetReg(0x4B,0x19);
	CAMERA_SetReg(0x48,0x08);
	CAMERA_SetReg(0x49,0x03);
	CAMERA_SetReg(0x4A,0x00);
	CAMERA_SetReg(0x4B,0x16);
	CAMERA_SetReg(0x48,0x09);
	CAMERA_SetReg(0x49,0x03);
	CAMERA_SetReg(0x4A,0x80);
	CAMERA_SetReg(0x4B,0x13);
	CAMERA_SetReg(0x48,0x0A);
	CAMERA_SetReg(0x49,0x03);
	CAMERA_SetReg(0x4A,0xC0);
	CAMERA_SetReg(0x4B,0x12);
	CAMERA_SetReg(0x48,0x0B);
	CAMERA_SetReg(0x4B,0x14);
	//Gamma(Green)
	CAMERA_SetReg(0x48,0x10);
	CAMERA_SetReg(0x49,0x00);
	CAMERA_SetReg(0x4A,0x08);
	CAMERA_SetReg(0x4B,0x1B);
	CAMERA_SetReg(0x48,0x11);
	CAMERA_SetReg(0x49,0x00);
	CAMERA_SetReg(0x4A,0x10);
	CAMERA_SetReg(0x4B,0x4E);
	CAMERA_SetReg(0x48,0x12);
	CAMERA_SetReg(0x49,0x00);
	CAMERA_SetReg(0x4A,0x20);
	CAMERA_SetReg(0x4B,0x6F);
	CAMERA_SetReg(0x48,0x13);
	CAMERA_SetReg(0x49,0x00);
	CAMERA_SetReg(0x4A,0x40);
	CAMERA_SetReg(0x4B,0x59);
	CAMERA_SetReg(0x48,0x14);
	CAMERA_SetReg(0x49,0x00);
	CAMERA_SetReg(0x4A,0x80);
	CAMERA_SetReg(0x4B,0x33);
	CAMERA_SetReg(0x48,0x15);
	CAMERA_SetReg(0x49,0x01);
	CAMERA_SetReg(0x4A,0x00);
	CAMERA_SetReg(0x4B,0x2B);
	CAMERA_SetReg(0x48,0x16);
	CAMERA_SetReg(0x49,0x01);
	CAMERA_SetReg(0x4A,0xC0);
	CAMERA_SetReg(0x4B,0x20);
	CAMERA_SetReg(0x48,0x17);
	CAMERA_SetReg(0x49,0x02);
	CAMERA_SetReg(0x4A,0x80);
	CAMERA_SetReg(0x4B,0x19);
	CAMERA_SetReg(0x48,0x18);
	CAMERA_SetReg(0x49,0x03);
	CAMERA_SetReg(0x4A,0x00);
	CAMERA_SetReg(0x4B,0x16);
	CAMERA_SetReg(0x48,0x19);
	CAMERA_SetReg(0x49,0x03);
	CAMERA_SetReg(0x4A,0x80);
	CAMERA_SetReg(0x4B,0x13);
	CAMERA_SetReg(0x48,0x1A);
	CAMERA_SetReg(0x49,0x03);
	CAMERA_SetReg(0x4A,0xC0);
	CAMERA_SetReg(0x4B,0x12);
	CAMERA_SetReg(0x48,0x1B);
	CAMERA_SetReg(0x4B,0x14);
	//Gamma(Blue)
	CAMERA_SetReg(0x48,0x20);
	CAMERA_SetReg(0x49,0x00);
	CAMERA_SetReg(0x4A,0x08);
	CAMERA_SetReg(0x4B,0x1B);
	CAMERA_SetReg(0x48,0x21);
	CAMERA_SetReg(0x49,0x00);
	CAMERA_SetReg(0x4A,0x10);
	CAMERA_SetReg(0x4B,0x4E);
	CAMERA_SetReg(0x48,0x22);
	CAMERA_SetReg(0x49,0x00);
	CAMERA_SetReg(0x4A,0x20);
	CAMERA_SetReg(0x4B,0x6F);
	CAMERA_SetReg(0x48,0x23);
	CAMERA_SetReg(0x49,0x00);
	CAMERA_SetReg(0x4A,0x40);
	CAMERA_SetReg(0x4B,0x59);
	CAMERA_SetReg(0x48,0x24);
	CAMERA_SetReg(0x49,0x00);
	CAMERA_SetReg(0x4A,0x80);
	CAMERA_SetReg(0x4B,0x33);
	CAMERA_SetReg(0x48,0x25);
	CAMERA_SetReg(0x49,0x01);
	CAMERA_SetReg(0x4A,0x00);
	CAMERA_SetReg(0x4B,0x2B);
	CAMERA_SetReg(0x48,0x26);
	CAMERA_SetReg(0x49,0x01);
	CAMERA_SetReg(0x4A,0xC0);
	CAMERA_SetReg(0x4B,0x20);
	CAMERA_SetReg(0x48,0x27);
	CAMERA_SetReg(0x49,0x02);
	CAMERA_SetReg(0x4A,0x80);
	CAMERA_SetReg(0x4B,0x19);
	CAMERA_SetReg(0x48,0x28);
	CAMERA_SetReg(0x49,0x03);
	CAMERA_SetReg(0x4A,0x00);
	CAMERA_SetReg(0x4B,0x16);
	CAMERA_SetReg(0x48,0x29);
	CAMERA_SetReg(0x49,0x03);
	CAMERA_SetReg(0x4A,0x80);
	CAMERA_SetReg(0x4B,0x13);
	CAMERA_SetReg(0x48,0x2A);
	CAMERA_SetReg(0x49,0x03);
	CAMERA_SetReg(0x4A,0xC0);
	CAMERA_SetReg(0x4B,0x12);
	CAMERA_SetReg(0x48,0x2B);
	CAMERA_SetReg(0x4B,0x14);
	CAMERA_SetReg(0x48,0x80);

	//AWB
	//CAMERA_SetReg(0xef,0x00);
	//CAMERA_SetReg(0x93,0x01);   // AWB map
	//CAMERA_SetReg(0xa0,0x0f);
	//CAMERA_SetReg(0x9f,0x0f);
	//CAMERA_SetReg(0x93,0x00);

	//AWB mode : auto(??)
	//CAMERA_SetReg(0xef,0x01);
	//CAMERA_SetReg(0x80,0x00);

	// Hue

	CAMERA_SetReg(0xef,0x00);
	CAMERA_SetReg(0x8c,0x0c);	//Skin Color Detection On/Off
	CAMERA_SetReg(0x74,0x01);	//20070214 //00	//Hue & Saturation Off=00, On=01
	CAMERA_SetReg(0x79,0x46);   //20070214 //58	//Saturation Gain
	CAMERA_SetReg(0x75,0x01);	//Hue & Saturation Cosine 1
	CAMERA_SetReg(0x76,0x00);   //a0   //20070214 //ff	//Hue & Saturation Cosine 2
	CAMERA_SetReg(0x77,0x02);	//Hue & Saturation Sine 1
	CAMERA_SetReg(0x78,0x00);   //20070214 //11	//Hue & Saturation Sine 2
	CAMERA_SetReg(0x72,0x88);	//20070213	//b0   //90	//Contrast Gain
	//CAMERA_SetReg(0x73,0x89);	//Brightness Gain
	CAMERA_SetReg(0xd8,0x00);   //Edge color suppress 

	//CAMERA_SetReg(0xef,0x02);
	//CAMERA_SetReg(0x0f,0x40); 

	//AWB white's count low 0f00 --> 0708
	CAMERA_SetReg(0xef,0x01);
	CAMERA_SetReg(0x85,0x00);   //07	//0f	//07
	CAMERA_SetReg(0x86,0x3c);   //2d	//08	//00	//08
	CAMERA_SetReg(0x87,0x13);   //0e
	CAMERA_SetReg(0x88,0xc0);   //d0

	//AWB boundary
	CAMERA_SetReg(0xef,0x00);
	CAMERA_SetReg(0xb1,0x00);
	CAMERA_SetReg(0xb2,0x00);
	CAMERA_SetReg(0xb3,0x00);
	CAMERA_SetReg(0xb4,0x00);
	CAMERA_SetReg(0xb5,0x00);
	CAMERA_SetReg(0xb6,0xf0);
	CAMERA_SetReg(0xb7,0x01);
	CAMERA_SetReg(0xb8,0x40);

	// Bad Pixel Replacement

	//CAMERA_SetReg(0xef,0x00);
	//CAMERA_SetReg(0x1a,0x01);
	//CAMERA_SetReg(0x1b,0x00);
	//CAMERA_SetReg(0x1e,0x00);
	//CAMERA_SetReg(0x1c,0x00);
	//CAMERA_SetReg(0x1f,0x00);
	//CAMERA_SetReg(0x1d,0x00);
	//CAMERA_SetReg(0x20,0x00);

	CAMERA_SetReg(0xef,0x01);
	CAMERA_SetReg(0xb9,0x08);  //10  //30	//BPR Gain Setting
	CAMERA_SetReg(0xba,0x00);
	CAMERA_SetReg(0xbd,0x00);

	CAMERA_SetReg(0xeb,0x60);  //40  //6f   //Low Light Indication AGC

	//20070213
	//AWB Map 1
	CAMERA_SetReg(0xef,0x01);
	CAMERA_SetReg(0x94,0x80);    //00
	CAMERA_SetReg(0x95,0xc0);    //c0
	CAMERA_SetReg(0x96,0xc0);    //f0
	CAMERA_SetReg(0x97,0xf0);    //c0
	CAMERA_SetReg(0x98,0x78);    //78
	CAMERA_SetReg(0x99,0x78);    //78
	CAMERA_SetReg(0x9a,0x7c);    //7c
	CAMERA_SetReg(0x9b,0x3c);    //3f
	CAMERA_SetReg(0x9c,0x1e);    //1f
	CAMERA_SetReg(0x9d,0x0f);    //0f
	CAMERA_SetReg(0x9e,0x07);    //07
	CAMERA_SetReg(0x9f,0x03);    //03
	CAMERA_SetReg(0xa0,0x00);    //01
	CAMERA_SetReg(0xa1,0xc8);  
	CAMERA_SetReg(0xa2,0x02);
	CAMERA_SetReg(0x93,0x82);    //AWB window //02

	//20070213
	//AWB Map Direct Write
	CAMERA_SetReg(0xef,0x00);
	CAMERA_SetReg(0x94,0x80);    //00
	CAMERA_SetReg(0x95,0xc0);    //c0
	CAMERA_SetReg(0x96,0xc0);    //f0
	CAMERA_SetReg(0x97,0xf0);    //c0
	CAMERA_SetReg(0x98,0x78);    //78
	CAMERA_SetReg(0x99,0x78);    //78
	CAMERA_SetReg(0xa0,0x7c);    //7c
	CAMERA_SetReg(0xa1,0x3c);    //3f
	CAMERA_SetReg(0xa2,0x1e);    //1f
	CAMERA_SetReg(0xa3,0x0f);    //0f
	CAMERA_SetReg(0xa4,0x07);    //07
	CAMERA_SetReg(0xa5,0x03);    //03
	CAMERA_SetReg(0xa6,0x00);    //01

	//Auto Flicker Correction
	CAMERA_SetReg(0xef,0x01);
	CAMERA_SetReg(0xdd,0x31);
	CAMERA_SetReg(0xcd,0x21);
	CAMERA_SetReg(0xde,0x31);

	//Frame AE Control - L.L 5fps
	//*** Frame 고정시는 아래 부분을 모두 주석처리 할 것.***
	CAMERA_SetReg(0xef,0x00);
	CAMERA_SetReg(0x57,0x00);
	//CAMERA_SetReg(0xef,0x01);
	//CAMERA_SetReg(0xe2,0x18);
	//CAMERA_SetReg(0xe3,0x16);
	//CAMERA_SetReg(0xe8,0x88);
	//******************************************************

	//CAMERA_SetReg(0xef,0x00);
	//CAMERA_SetReg(0x43,0x40);  //20070222  //fixed value
	//CAMERA_SetReg(0x42,0x3d);  //AWB R start point
	//CAMERA_SetReg(0x44,0x6b);  //AWB B start point

	CAMERA_SetReg(0xef,0x01);  //shadow line
	CAMERA_SetReg(0xa5,0x00);

	CAMERA_SetReg(0xef,0x00);
	CAMERA_SetReg(0xb9,0x02);
	CAMERA_SetReg(0xba,0x80);
	CAMERA_SetReg(0xbb,0x80);
	CAMERA_SetReg(0xbc,0x80); //98
	CAMERA_SetReg(0xbd,0x00); //30

	//
	CAMERA_SetReg(0xef,0x00);
	CAMERA_SetReg(0x04,0x00);
	CAMERA_SetReg(0x05,0xf8);
	CAMERA_SetReg(0xc2,0x08);
	CAMERA_SetReg(0xc3,0x08);
	CAMERA_SetReg(0xc4,0x00);
	CAMERA_SetReg(0xc5,0xf0);
	CAMERA_SetReg(0xc6,0x01);
	CAMERA_SetReg(0xc7,0x40);

	CAMERA_SetReg(0xef,0x02);
	CAMERA_SetReg(0x0a,0x9a);
	CAMERA_SetReg(0x08,0xf7);

	CAMERA_SetReg(0x1a,0x01);
	CAMERA_SetReg(0x1b,0xe6);

	CAMERA_SetReg(0xef,0x01);
	CAMERA_SetReg(0xca,0x01);
	CAMERA_SetReg(0xcb,0xe6);

	CAMERA_SetReg(0x70,0x17);   //17 // fix PAL(50Hz)

	//QVGA to QCIF
	CAMERA_SetReg(0xef,0x00);
	CAMERA_SetReg(0x06,0x01);
	CAMERA_SetReg(0x07,0x48);
	CAMERA_SetReg(0x04,0x00);
	CAMERA_SetReg(0x05,0xf8);

	CAMERA_SetReg(0x7d,0x01);
	CAMERA_SetReg(0x7e,0x48);
	CAMERA_SetReg(0x7b,0x00);
	CAMERA_SetReg(0x7c,0xf8);

	CAMERA_SetReg(0x81,0x00);
	CAMERA_SetReg(0x82,0xc0); //b0
	CAMERA_SetReg(0x7f,0x00);
	CAMERA_SetReg(0x80,0xa0); //90

	CAMERA_SetReg(0xc6,0x00);
	CAMERA_SetReg(0xc7,0xb0);
	CAMERA_SetReg(0xc4,0x00);
	CAMERA_SetReg(0xc5,0x90);

	CAMERA_IICClose();


}

