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
*	File Name : library.h
*  
*	File Description : This file defines the register access function
*						and declares prototypes of library funcions
*
*	Author : Haksoo,Kim
*	Dept. : AP Development Team
*	Created Date : 2006/11/08
*	Version : 0.1 
* 
*	History
*	- Created(Haksoo,Kim 2006/11/08)
*	- Added Pause() by OnPil, Shin on 2008/03/04
*  
**************************************************************************************/
#ifndef __LIBRARY_H__
#define __LIBRARY_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include "def.h"
#include "uart.h"
#include "register_addr.h"

#define Assert(a) {if (!(a)) {UART_Printf("\n %s(line %d)\n", __FILE__, __LINE__); exit(0);}}

#define Outp32(addr, data)	(*(volatile u32 *)(addr) = (data))
#define Outp16(addr, data)	(*(volatile u16 *)(addr) = (data))
#define Outp8(addr, data)	(*(volatile u8 *)(addr) = (data))
#define Inp32(addr)			(*(volatile u32 *)(addr))
#if 1 // LCD SPISetting jungil
#define Inp32_SPI(addr, data) (data = (*(volatile u32 *)(addr)))
#endif

#define Inp16(addr)			(*(volatile u16 *)(addr))
#define Inp8(addr)			(*(volatile u8 *)(addr))


void InitUartPort(u8 ch, u8 flowControl);
void InitLED(void);
void DisplayLED(u8 data);
void OpenConsole(void);
s32 GetIntNum(void);
u32 DownloadImageThruUart(u8 *DownloadAddress);
u32 DownloadImageThruUsbOtg(u8 *DownloadAddress);
void UploadImageThruUsbOtg(void);
void Delay(u32 usec);
u32 Pow(u32 x, u32 y);
void Copy(u32 sa, u32 da, u32 words);
void Copy8(u32 sa, u32 da, u32 bytes);
void Copy16(u32 sa, u32 da, u32 Hwords);
u8 Compare( u32 a0,  u32 a1,  u32 words);
void Dump32(u32 addr, u32 words);
void Dump16(u32 addr, u32 hwords);
void Dump8(u32 addr, u32 bytes);
void Stop(void);
u32 LoadFromFile(const char* fileName, u32 uDstAddr);
void LoadFromFile1(const char* fileName, u32 uDstAddr, u32* uFileSize);
void SaveToFile(const char* fileName, u32 fileSize, u32 uSrcAddr);

void ConvertBmpToRgb16bpp(u32 fromAddr, u32 toAddr, u32 xSize, u32 ySize);
void ConvertBmpToRgb24bpp(u32 fromAddr, u32 toAddr, u32 xSize, u32 ySize);
void ConvertImgSzToNumber(IMG_SIZE eSize, u32* uHsz, u32* uVsz);
void RPRINTF(REGINFO sReg, u32 uPattern, u8 uRegStatus, s32 Nth,u8 uInDetail);
void TestSFR(void);
u32 GetBitPosition(u32 uValue);
void UART_Printf(const char *fmt,...);
u32 Inp32Not4ByteAlign(u32 addr);

void Pause(void);		// Added by SOP on 2008/03/04

#ifdef __cplusplus
}
#endif

#endif /*__LIBRARY_H__*/

