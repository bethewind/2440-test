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
*	File Name : rotator.c
*  
*	File Description : This file includes the API functions for rotator.
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


#include "system.h"
#include "lcd.h"
#include "glib.h"
#include "glib_font.h"
#include "rotator.h"
#include "camera.h"
#include "post.h"
#include "timer.h"
#include "intc.h"



// Header file for text drawing
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#define	rROTATOR_CTRLCFG				(ROTATOR_BASE+0x00)
#define	rROTATOR_SRCADDRREG0			(ROTATOR_BASE+0x04)
#define	rROTATOR_SRCADDRREG1			(ROTATOR_BASE+0x08)
#define	rROTATOR_SRCADDRREG2			(ROTATOR_BASE+0x0c)
#define	rROTATOR_SRCSIZEREG			(ROTATOR_BASE+0x10)
#define	rROTATOR_DESTADDRREG0		(ROTATOR_BASE+0x18)
#define	rROTATOR_DESTADDRREG1		(ROTATOR_BASE+0x1c)
#define	rROTATOR_DESTADDRREG2		(ROTATOR_BASE+0x20)
#define	rROTATOR_STATCFG				(ROTATOR_BASE+0x2c)


ROTATOR oROTATOR;
volatile u8 bRotatorDone;
volatile u8 bPostIntOccured;

const static u32 uLcdStAddr = CODEC_MEM_ST; 

static POST oPost;
static POST oSc;


void ROTATOR_GetFrmSz(u16 *usRotHorSize, u16 *usRotVerSize)
{
	u32 uSize;

	uSize=Inp32(rROTATOR_SRCSIZEREG);
	*usRotHorSize=(u16)(uSize&0xffff);
	*usRotVerSize=(u16)((uSize>>16)&0xffff);

}
void ROTATOR_GetFbAddr(u32 *uRotSrcAddr, u32 *uRotDstAddr)
{
	*uRotSrcAddr=Inp32(rROTATOR_SRCADDRREG0);
	*uRotDstAddr=Inp32(rROTATOR_DESTADDRREG0);

}
void ROTATOR_GetBppMode(CSPACE *eBpp)
{

	u32 uCTRLCFG;
	uCTRLCFG=Inp32(rROTATOR_CTRLCFG);
	switch((uCTRLCFG)&(0x7<<13)) {
		case ROTATOR_BIT_IN_YC420:
			*eBpp=YC420;
			break;
		case ROTATOR_BIT_IN_YC422:
//			*eBpp=YC422;
			*eBpp=YCBYCR;
			break;
		case ROTATOR_BIT_IN_RGB565:
			*eBpp=RGB16;
			break;
		case ROTATOR_BIT_IN_RGB888:
			*eBpp=RGB24;
			break;
		default:
			Assert(0);
			break;
	}
}

void ROTATOR_GetLcdFbAddr(u32 *uLcdFbAddr0, u32 *uLcdFbAddr1)
{
	*uLcdFbAddr0=oROTATOR.m_uLcdFbAddr0;
	*uLcdFbAddr1=oROTATOR.m_uLcdFbAddr1;	
}

void ROTATOR_DrawImage(CSPACE eBpp, u16 usRotHorSize, u16 usRotVerSize, u32 uRotSrcAddr)
{
#ifdef NATURALIMAGE
	u32 i,j,k;
#endif	

	GLIB_InitInstance(uRotSrcAddr, usRotHorSize, usRotVerSize, eBpp);	

#ifdef NATURALIMAGE
	k=0;
	switch(eBpp) {
		case YC420:
			break;
		case YCBYCR:
		case YCRYCB:			
		case CBYCRY:	
			break;
		case RGB16:
			if(usRotHorSize==240) {
				for(j=0; j<usRotVerSize; j++)
					for(i=0; i<usRotHorSize; i++)
						GLIB_PutPixel2(i, j, sun240320_16bpp[k++]);
			}
			else if(usRotHorSize==320) {
				for(j=0; j<usRotVerSize; j++)
					for(i=0; i<usRotHorSize; i++)
						GLIB_PutPixel2(i, j, sun320240_16bpp[k++]);
			}	
			else ;
			break;	
		case RGB24:
			if(usRotHorSize==240) {
				for(j=0; j<usRotVerSize; j++)
					for(i=0; i<usRotHorSize; i++)
						GLIB_PutPixel2(i, j, sun240320_24bpp[k++]);
			}
			else if(usRotHorSize==320) {
				for(j=0; j<usRotVerSize; j++)
					for(i=0; i<usRotHorSize; i++)
						GLIB_PutPixel2(i, j, sun320240_24bpp[k++]);
			}	
			else ;
			break;
		default:
			Assert(0);
			break;
	}
#else
	GLIB_DrawPattern(usRotHorSize, usRotVerSize);
#endif
}

u8 ROTATOR_CompFbData(CSPACE eBpp, ROTATOR_ROT_TYPE eRotType, u32 uSrcWidth, u32 uSrcHeight, u32 uInX, u32 uInY, 
	                                                 u32 uRotWidth, u32 uRotHeight, u32 uRotX, u32 uRotY, u32 uSrcFbAddr, u32 uRotFbAddr)
{
	u32 uOrgData, uRotData;
	u8 ucOrgDataY, ucOrgDataCb0, ucOrgDataCr0;
	u8 ucOrgDataCb1, ucOrgDataCr1;
	u8 ucRotDataY, ucRotDataCb, ucRotDataCr;	
	u32 uTempData0, uTempData1;

	uOrgData=uRotData=0;

	switch(eBpp) {
		case RGB16:
			uOrgData = (u32)(*(volatile unsigned short *)(uSrcFbAddr + (uInY*uSrcWidth +uInX)*2));
			uRotData = (u32)(*(volatile unsigned short *)(uRotFbAddr + (uRotY*uRotWidth +uRotX)*2));
			break;
		case RGB24:
			uOrgData = *(volatile unsigned int *)(uSrcFbAddr + (uInY*uSrcWidth +uInX)*4);
			uRotData = *(volatile unsigned int *)(uRotFbAddr + (uRotY*uRotWidth +uRotX)*4);
			break;
		case YC420:
			ucOrgDataY   = *(volatile unsigned char *)(uSrcFbAddr + (uInY*uSrcWidth +uInX));
			ucOrgDataCb0 = *(volatile unsigned char *)(uSrcFbAddr + uSrcWidth*uSrcHeight + (uInY/2*uSrcWidth/2 +uInX/2));
			ucOrgDataCr0 = *(volatile unsigned char *)(uSrcFbAddr + uSrcWidth*uSrcHeight + uSrcWidth*uSrcHeight/4 +(uInY/2*uSrcWidth/2 +uInX/2));
			uOrgData=(ucOrgDataY<<16)|(ucOrgDataCb0<<8)|(ucOrgDataCr0);
			ucRotDataY   = *(volatile unsigned char *)(uRotFbAddr + (uRotY*uRotWidth +uRotX));
			ucRotDataCb = *(volatile unsigned char *)(uRotFbAddr + uRotWidth*uRotHeight + (uRotY/2*uRotWidth/2 +uRotX/2));
			ucRotDataCr  = *(volatile unsigned char *)(uRotFbAddr + uRotWidth*uRotHeight + uRotWidth*uRotHeight/4 +(uRotY/2*uRotWidth/2 +uRotX/2));			
			uRotData=(ucRotDataY<<16)|(ucRotDataCb<<8)|(ucRotDataCr);
			break;
		case YCRYCB:			
		case YCBYCR:
		case CBYCRY:
			if(eRotType == ROTATOR_TYPE_ROT_90 || eRotType==ROTATOR_TYPE_ROT_270) {
				uTempData0   = *(volatile unsigned int *)(uSrcFbAddr + (uInY*uSrcWidth/2 +uInX/2)*4);
				uTempData1   = (uInY%2) ? *(volatile unsigned int *)(uSrcFbAddr + ((uInY-1)*uSrcWidth/2 +uInX/2)*4) :
										*(volatile unsigned int *)(uSrcFbAddr + ((uInY+1)*uSrcWidth/2 +uInX/2)*4);
				ucOrgDataY =  (uTempData0>>((((uInX)%2)*16)))&0xff;
				ucOrgDataCb0= (uTempData0>>24)&0xff;
				ucOrgDataCr0= (uTempData0>>8)&0xff;
				ucOrgDataCb1= (uTempData1>>24)&0xff;
				ucOrgDataCr1= (uTempData1>>8)&0xff;
				uOrgData=(ucOrgDataY<<16)|(((ucOrgDataCb0+ucOrgDataCb1)/2)<<8)|((ucOrgDataCr0+ucOrgDataCr1)/2);
				uTempData0   = *(volatile unsigned int *)(uRotFbAddr + (uRotY*uRotWidth/2 +uRotX/2)*4);
				ucRotDataY =  (uTempData0>>((((uRotX)%2)*16)))&0xff;
				ucRotDataCb= (uTempData0>>24)&0xff;
				ucRotDataCr= (uTempData0>>8)&0xff;			
				uRotData=(ucRotDataY<<16)|(ucRotDataCb<<8)|(ucRotDataCr);	
			}
			else {
				uTempData0   = *(volatile unsigned int *)(uSrcFbAddr + (uInY*uSrcWidth/2 +uInX/2)*4);
				ucOrgDataY =  (uTempData0>>((((uInX)%2)*16)))&0xff;
				ucOrgDataCb0= (uTempData0>>24)&0xff;
				ucOrgDataCr0= (uTempData0>>8)&0xff;
				uOrgData=(ucOrgDataY<<16)|(ucOrgDataCb0<<8)|(ucOrgDataCr0);
				uTempData0   = *(volatile unsigned int *)(uRotFbAddr + (uRotY*uRotWidth/2 +uRotX/2)*4);
				ucRotDataY =  (uTempData0>>((((uRotX)%2)*16)))&0xff;
				ucRotDataCb= (uTempData0>>24)&0xff;
				ucRotDataCr= (uTempData0>>8)&0xff;			
				uRotData=(ucRotDataY<<16)|(ucRotDataCb<<8)|(ucRotDataCr);				
			}
			break;
		default:
			break;
	}
	if(uOrgData != uRotData) {
		UART_Printf("[0x%x, 0x%x]=0x%x, [0x%x,0x%x]=0x%x\n", uInX, uInY, uOrgData, uRotX, uRotY, uRotData);
		return false;
	}	
	else	 return true;
}

void ROTATOR_GetRotateCoordinate(u32 uSrcX, u32 uSrcY, u32 uSrcWidth, u32 uSrcHeight, ROTATOR_ROT_TYPE eRotType, 
	                                                               u32 *uRotX, u32 *uRotY, u32 *uRotWidth, u32 *uRotHeight) 
{

	switch(eRotType) {
		case  ROTATOR_TYPE_ROT_90 : // No Rotate. bypass.
			*uRotX = uSrcHeight-1-uSrcY;
			*uRotY = uSrcX;
			*uRotWidth=uSrcHeight;
			*uRotHeight=uSrcWidth;
			break;
		case  ROTATOR_TYPE_ROT_180 : // 90 degree Rotation
			*uRotX = uSrcWidth-1-uSrcX;
			*uRotY = uSrcHeight-1-uSrcY;
			*uRotWidth=uSrcWidth;
			*uRotHeight=uSrcHeight;
			break;
		case  ROTATOR_TYPE_ROT_270 : // 180 degree Rotation
			*uRotX = uSrcY;
			*uRotY = uSrcWidth-1-uSrcX;
			*uRotWidth=uSrcHeight;
			*uRotHeight=uSrcWidth;
			break;
		case  ROTATOR_TYPE_FLIP_HOR: // 270 degree Rotation
			*uRotX = uSrcWidth-1-uSrcX;
			*uRotY = uSrcY;
			*uRotWidth=uSrcWidth;
			*uRotHeight=uSrcHeight;
			break;
		case ROTATOR_TYPE_FLIP_VER: // X-flip
			*uRotX = uSrcX;
			*uRotY = uSrcHeight-1-uSrcY;
			*uRotWidth=uSrcWidth;
			*uRotHeight=uSrcHeight;			
			break;
		default :
			Assert(0);
			break;
	}
}


u8 ROTATOR_AuditOperation(CSPACE eBpp, u32 uWidth, u32 uHeight, ROTATOR_ROT_TYPE eRotType, 
	                                                    u32 uInFbAddr, u32 uOutFbAddr)
{
	u32 i,j;
	u32 uRotX, uRotY;
	u32 uRotWidth, uRotHeight;


	for(j=0; j<uHeight; j++)
		for(i=0; i<uWidth; i++) {
			ROTATOR_GetRotateCoordinate(i, j, uWidth, uHeight, eRotType, &uRotX, &uRotY, &uRotWidth, &uRotHeight);
			if(!ROTATOR_CompFbData(eBpp, eRotType, uWidth, uHeight, i, j, uRotWidth, uRotHeight, uRotX, uRotY, uInFbAddr, uOutFbAddr)) return false;
		}	
	return true;
}

void __irq ROTATOR_PostISR(void)
{
	bPostIntOccured = TRUE;
	POST_ClearPending(&oPost);
	INTC_ClearVectAddr();
}


void ROTATOR_ConvertColorSpace(CSPACE eInFmt, u32 uInFbAddr, u32 uWidth, u32 uHeight, CSPACE eOutFmt, u32 uOutFbAddr)
{

	INTC_Enable(9);
	INTC_SetVectAddr(9, ROTATOR_PostISR);
	POST_EnableInterrupt(POST_LEVEL_INT, &oPost);

	bPostIntOccured=0;
	POST_InitIpForDmaInDmaOut(uWidth, uHeight, uInFbAddr, eInFmt, 
								uWidth, uHeight, uOutFbAddr, eOutFmt,
								1, FALSE, ONE_SHOT, &oPost);
	//POST_InitIpForDmaInDmaOut(	uImgHSz, uImgVSz, uGlibStAddr, eSrcDataFmt, 
	//							uLcdHSz, uLcdVSz, uPostStAddr, eMidDataFmt,
	//							1, FALSE, ONE_SHOT, &oPost);	
	POST_StartProcessing(&oPost);
	while(!bPostIntOccured);
	bPostIntOccured=0;
	POST_DisableInterrupt(&oPost);
	POST_ClearPending(&oPost);	
	INTC_Disable(9);	
}


void ROTATOR_SetAddr (CSPACE eBpp, u32 uFbAddr, u16 usHorSize, u16 usVerSize)
{
	u32 uOffset;

	Outp32(rROTATOR_SRCADDRREG0, uFbAddr);
	oROTATOR.m_uSrcAddr0=uFbAddr;

	uOffset=usHorSize*usVerSize;
	switch(eBpp) {
		case YC420:
			Outp32(rROTATOR_SRCADDRREG1, uFbAddr+uOffset);
			Outp32(rROTATOR_SRCADDRREG2, uFbAddr+uOffset+uOffset/4);
			Outp32(rROTATOR_DESTADDRREG0, uFbAddr+uOffset+uOffset/2);
			Outp32(rROTATOR_DESTADDRREG1, uFbAddr+uOffset*2+uOffset/2);
			Outp32(rROTATOR_DESTADDRREG2, uFbAddr+uOffset*2+uOffset/2+uOffset/4);
			oROTATOR.m_uSrcAddr1=uFbAddr+uOffset;
			oROTATOR.m_uSrcAddr2=uFbAddr+uOffset+uOffset/4;
			oROTATOR.m_uDstAddr0=uFbAddr+uOffset+uOffset/2;
			oROTATOR.m_uDstAddr1=uFbAddr+uOffset*2+uOffset/2;
			oROTATOR.m_uDstAddr2=uFbAddr+uOffset*2+uOffset/2+uOffset/4;
			break;
		case YCRYCB:	
		case YCBYCR:
		case CBYCRY:	
			Outp32(rROTATOR_SRCADDRREG1, 0);
			Outp32(rROTATOR_SRCADDRREG2, 0);
			Outp32(rROTATOR_DESTADDRREG0, uFbAddr+uOffset*2);
			Outp32(rROTATOR_DESTADDRREG1, 0);
			Outp32(rROTATOR_DESTADDRREG2, 0);			
			oROTATOR.m_uSrcAddr1=0;
			oROTATOR.m_uSrcAddr2=0;
			oROTATOR.m_uDstAddr0=uFbAddr+uOffset*2;
			oROTATOR.m_uDstAddr1=0;
			oROTATOR.m_uDstAddr2=0;			
			break; 
		case RGB16:
			Outp32(rROTATOR_SRCADDRREG1, 0);
			Outp32(rROTATOR_SRCADDRREG2, 0);
			Outp32(rROTATOR_DESTADDRREG0, uFbAddr+uOffset*2);
			Outp32(rROTATOR_DESTADDRREG1, 0);
			Outp32(rROTATOR_DESTADDRREG2, 0);
			oROTATOR.m_uSrcAddr1=0;
			oROTATOR.m_uSrcAddr2=0;
			oROTATOR.m_uDstAddr0=uFbAddr+uOffset*2;
			oROTATOR.m_uDstAddr1=0;
			oROTATOR.m_uDstAddr2=0;
			break;
		case RGB24:
			Outp32(rROTATOR_SRCADDRREG1, 0);
			Outp32(rROTATOR_SRCADDRREG2, 0);
			Outp32(rROTATOR_DESTADDRREG0, uFbAddr+uOffset*4);
			Outp32(rROTATOR_DESTADDRREG1, 0);
			Outp32(rROTATOR_DESTADDRREG2, 0);
			oROTATOR.m_uSrcAddr1=0;
			oROTATOR.m_uSrcAddr2=0;
			oROTATOR.m_uDstAddr0=uFbAddr+uOffset*4;
			oROTATOR.m_uDstAddr1=0;
			oROTATOR.m_uDstAddr2=0;				
			break;
		default:
			Assert(0);
			break;
	}
	
}

void ROTATOR_SetSrcAddr(CSPACE eBpp, u32 uFbAddr, u16 usHorSize, u16 usVerSize)
{
	u32 uOffset;

	Outp32(rROTATOR_SRCADDRREG0, uFbAddr);
	oROTATOR.m_uSrcAddr0=uFbAddr;

	uOffset=usHorSize*usVerSize;
	switch(eBpp) {
		case YC420:
			Outp32(rROTATOR_SRCADDRREG1, uFbAddr+uOffset);
			Outp32(rROTATOR_SRCADDRREG2, uFbAddr+uOffset+uOffset/4);
			oROTATOR.m_uSrcAddr1=uFbAddr+uOffset;
			oROTATOR.m_uSrcAddr2=uFbAddr+uOffset+uOffset/4;
			break;
		case YCRYCB:	
		case YCBYCR:
		case CBYCRY:	
			Outp32(rROTATOR_SRCADDRREG1, 0);
			Outp32(rROTATOR_SRCADDRREG2, 0);
			oROTATOR.m_uSrcAddr1=0;
			oROTATOR.m_uSrcAddr2=0;			
			break; 
		case RGB16:
			Outp32(rROTATOR_SRCADDRREG1, 0);
			Outp32(rROTATOR_SRCADDRREG2, 0);
			oROTATOR.m_uSrcAddr1=0;
			oROTATOR.m_uSrcAddr2=0;
			break;
		case RGB24:
			Outp32(rROTATOR_SRCADDRREG1, 0);
			Outp32(rROTATOR_SRCADDRREG2, 0);
			oROTATOR.m_uSrcAddr1=0;
			oROTATOR.m_uSrcAddr2=0;
			break;
		default:
			Assert(0);
			break;
	}
	
}

void ROTATOR_SetDstAddr(CSPACE eBpp, u32 uFbAddr, u16 usHorSize, u16 usVerSize)
{
	u32 uOffset;

	Outp32(rROTATOR_DESTADDRREG0, uFbAddr);
	oROTATOR.m_uDstAddr0=uFbAddr;
	uOffset=usHorSize*usVerSize;
	switch(eBpp) {
		case YC420:
			Outp32(rROTATOR_DESTADDRREG1, uFbAddr+uOffset);
			Outp32(rROTATOR_SRCADDRREG2, uFbAddr+uOffset+uOffset/4);
			oROTATOR.m_uDstAddr1=uFbAddr+uOffset;
			oROTATOR.m_uDstAddr2=uFbAddr+uOffset+uOffset/4;
			break;
		case YCRYCB:	
		case YCBYCR:
		case CBYCRY:	
			Outp32(rROTATOR_DESTADDRREG1, 0);
			Outp32(rROTATOR_DESTADDRREG2, 0);
			oROTATOR.m_uDstAddr1=0;
			oROTATOR.m_uDstAddr2=0;
			break; 
		case RGB16:
			Outp32(rROTATOR_DESTADDRREG1, 0);
			Outp32(rROTATOR_DESTADDRREG2, 0);
			oROTATOR.m_uDstAddr1=0;
			oROTATOR.m_uDstAddr2=0;
			break;
		case RGB24:
			Outp32(rROTATOR_DESTADDRREG1, 0);
			Outp32(rROTATOR_DESTADDRREG2, 0);
			oROTATOR.m_uDstAddr1=0;
			oROTATOR.m_uDstAddr2=0;
			break;
		default:
			Assert(0);
			break;
	}	
}

void ROTATOR_SetSrcSize(u16 usHorSize, u16 usVerSize)
{
	Outp32(rROTATOR_SRCSIZEREG, (usVerSize<<16)|usHorSize);
	oROTATOR.m_usHorSize=usHorSize;
	oROTATOR.m_usVerSize=usVerSize;	
}

void ROTATOR_SetInFormat(CSPACE eBpp)
{
	u16 usInFormat;

	usInFormat=	(eBpp==YC420) ? ROTATOR_BIT_IN_YC420 :
				(eBpp==YCBYCR || eBpp==CBYCRY || eBpp==YCRYCB) ? ROTATOR_BIT_IN_YC422 :
				(eBpp==RGB16) ? ROTATOR_BIT_IN_RGB565 :
				(eBpp==RGB24) ? ROTATOR_BIT_IN_RGB888 : ROTATOR_BIT_IN_RGB565;
	Outp32(rROTATOR_CTRLCFG, (Inp32(rROTATOR_CTRLCFG)&~(0x7<<13))|usInFormat);		
	oROTATOR.m_usInFormat=usInFormat;
}

void ROTATOR_SetRotType(ROTATOR_ROT_TYPE eRotType)
{
	u8 ucRotType;

	ucRotType=	(eRotType==ROTATOR_TYPE_NONE) ? ROTATOR_BIT_NONE :
				(eRotType==ROTATOR_TYPE_FLIP_VER) ? ROTATOR_BIT_FLIP_VER:
				(eRotType==ROTATOR_TYPE_FLIP_HOR) ? ROTATOR_BIT_FLIP_HOR :
				(eRotType==ROTATOR_TYPE_ROT_90) ? ROTATOR_BIT_ROT_90 :
				(eRotType==ROTATOR_TYPE_ROT_180) ? ROTATOR_BIT_ROT_180 :
				(eRotType==ROTATOR_TYPE_ROT_270) ? ROTATOR_BIT_ROT_270 : ROTATOR_BIT_NONE;
	Outp32(rROTATOR_CTRLCFG, (Inp32(rROTATOR_CTRLCFG)&~(0xf<<4))|ucRotType);	
	oROTATOR.m_ucRotType=ucRotType;	
}



void ROTATOR_Start(void)
{
	Outp32(rROTATOR_CTRLCFG, Inp32(rROTATOR_CTRLCFG)|(ROTATOR_BIT_START));
//	while((Inp32(rROTATOR_CTRLCFG)&ROTATOR_BIT_START)); // This line should be disabled when One More Job is enabled.
}

void ROTATOR_CheckDone(void)
{
	u32 uSTATCFG;
#if 0 
	while(!(Inp32(rROTATOR_STATCFG)&(1<<8)));

#else // Checking the behavior of pending bit
	do	{
		uSTATCFG=Inp32(rROTATOR_STATCFG);
		UART_Printf("STATCFG:0x%x\n", uSTATCFG);
	}	
	while(!(uSTATCFG&(1<<8)));
#endif
	UART_Printf("ROTATOR is done[0x%x]!\n", Inp32(rROTATOR_STATCFG));
}

void ROTATOR_CheckIdle(void)
{
	while(Inp32(rROTATOR_STATCFG)&0x3);
}

void ROTATOR_IntEnable(void)
{
	bRotatorDone=0;
	oROTATOR.m_bIntEn=true;
	Inp32(rROTATOR_STATCFG);
	Outp32(rROTATOR_CTRLCFG, Inp32(rROTATOR_CTRLCFG)|ROTATOR_BIT_INT_EN);
}

void ROTATOR_IntDisable(void)
{
	bRotatorDone=0;
	oROTATOR.m_bIntEn=false;
	Inp32(rROTATOR_STATCFG);
	Outp32(rROTATOR_CTRLCFG, (Inp32(rROTATOR_CTRLCFG)&~(ROTATOR_BIT_INT_EN)));
}

void __irq Isr_Rotator(void)
{
	//u32 uSTATCFG;
	
	bRotatorDone=1;
#if 1 
	Inp32(rROTATOR_STATCFG);
	
#else // Checking the behavior of pending bit
		do	{
			uSTATCFG=Inp32(rROTATOR_STATCFG);
			UART_Printf("STATCFG:0x%x\n", uSTATCFG);
		}	
		while(!(uSTATCFG&(1<<8)));
#endif
//	UART_Printf("Rotator Done Interrupt is occured[0x%x]!\n", Inp32(rROTATOR_STATCFG));
	INTC_ClearVectAddr();
}

void __irq Isr_RotatorOneMoreJob(void)
{
	//u32 uSTATCFG;
	
	bRotatorDone=1;
#if 1 
	Inp32(rROTATOR_STATCFG);
	
#else // Checking the behavior of pending bit
		do	{
			uSTATCFG=Inp32(rROTATOR_STATCFG);
			UART_Printf("STATCFG:0x%x\n", uSTATCFG);
		}	
		while(!(uSTATCFG&(1<<8)));
#endif
	UART_Printf("Rotator Done Interrupt!\n");
//	UART_Printf("Rotator Done Interrupt is occured[0x%x]!\n", Inp32(rROTATOR_STATCFG));
	INTC_ClearVectAddr();
}

void ROTATOR_CheckIntDone(void)
{
	while(!bRotatorDone);
	bRotatorDone=0;
}



void ROTATOR_InitRegs(void)
{
	Outp32(rROTATOR_CTRLCFG, (oROTATOR.m_bIntEn<<24)|oROTATOR.m_usInFormat|oROTATOR.m_ucRotType|oROTATOR.m_bStartRot);
	Outp32(rROTATOR_SRCADDRREG0, oROTATOR.m_uSrcAddr0);
	Outp32(rROTATOR_SRCADDRREG1, oROTATOR.m_uSrcAddr1);
	Outp32(rROTATOR_SRCADDRREG2, oROTATOR.m_uSrcAddr2);
	Outp32(rROTATOR_SRCSIZEREG, (oROTATOR.m_usVerSize<<16)|oROTATOR.m_usHorSize);
	Outp32(rROTATOR_DESTADDRREG0, oROTATOR.m_uDstAddr0);
	Outp32(rROTATOR_DESTADDRREG1, oROTATOR.m_uDstAddr1);
	Outp32(rROTATOR_DESTADDRREG2, oROTATOR.m_uDstAddr2);	
}



void ROTATOR_InitSetting(CSPACE eBgBpp, u16 usHorSize, u16 usVerSize, u32 uRotSrcAddr, u32 uRotDstAddr, ROTATOR_ROT_TYPE eRotType)
{

	u32 uOffset;

	if(oROTATOR.m_bRotInit) {
		oROTATOR.m_bIntEn=false;
		oROTATOR.m_bStartRot=false;
	}
	oROTATOR.m_ucRotType=	(eRotType==ROTATOR_TYPE_NONE) ? ROTATOR_BIT_NONE :
							(eRotType==ROTATOR_TYPE_FLIP_VER) ? ROTATOR_BIT_FLIP_VER:
							(eRotType==ROTATOR_TYPE_FLIP_HOR) ? ROTATOR_BIT_FLIP_HOR :
							(eRotType==ROTATOR_TYPE_ROT_90) ? ROTATOR_BIT_ROT_90 :
							(eRotType==ROTATOR_TYPE_ROT_180) ? ROTATOR_BIT_ROT_180 :
							(eRotType==ROTATOR_TYPE_ROT_270) ? ROTATOR_BIT_ROT_270 : ROTATOR_BIT_NONE;
	oROTATOR.m_usHorSize=usHorSize;
	oROTATOR.m_usVerSize=usVerSize;

	oROTATOR.m_uSrcAddr0=uRotSrcAddr;	
	oROTATOR.m_uDstAddr0=uRotDstAddr;
	
	uOffset=usHorSize*usVerSize;
	switch(eBgBpp) {
		case YC420:
			oROTATOR.m_usInFormat=ROTATOR_BIT_IN_YC420;
			oROTATOR.m_uSrcAddr1=oROTATOR.m_uSrcAddr0+uOffset;
			oROTATOR.m_uSrcAddr2=oROTATOR.m_uSrcAddr1+uOffset/4;
			oROTATOR.m_uDstAddr1=oROTATOR.m_uDstAddr0+uOffset; 
			oROTATOR.m_uDstAddr2=oROTATOR.m_uDstAddr1+uOffset/4; 
			break;
		case YCRYCB:	
		case YCBYCR:
		case CBYCRY:	
			oROTATOR.m_usInFormat=ROTATOR_BIT_IN_YC422;
			oROTATOR.m_uSrcAddr1=0;
			oROTATOR.m_uSrcAddr2=0;
			oROTATOR.m_uDstAddr1=0;	
			oROTATOR.m_uDstAddr2=0;				
			break; 
		case RGB16:
			oROTATOR.m_usInFormat=ROTATOR_BIT_IN_RGB565;
			oROTATOR.m_uSrcAddr1=0;
			oROTATOR.m_uSrcAddr2=0;
			oROTATOR.m_uDstAddr1=0; 
			oROTATOR.m_uDstAddr2=0; 
			break;
		case RGB24:
			oROTATOR.m_usInFormat=ROTATOR_BIT_IN_RGB888;
			oROTATOR.m_uSrcAddr1=0;
			oROTATOR.m_uSrcAddr2=0;
			oROTATOR.m_uDstAddr1=0; 
			oROTATOR.m_uDstAddr2=0; 
			break;
		default:
			Assert(0);
			break;
	}

	ROTATOR_InitRegs();

}

void ROTATOR_InitLcd(LCD_WINDOW eWin, CSPACE eBpp)
{
	u32 uLcdFbAddr;
	u32 uLcdWidth, uLcdHeight, uLcdOffsetX, uLcdOffsetY;


	if(eWin==WIN0) {
		uLcdFbAddr=oROTATOR.m_uLcdFbAddr0;
		uLcdHeight=oROTATOR.m_uLcdHeight0;
		uLcdWidth=oROTATOR.m_uLcdWidth0;
		uLcdOffsetX=oROTATOR.m_uLcdOffsetX0;
		uLcdOffsetY=oROTATOR.m_uLcdOffsetY0;		
	}	
	else if(eWin==WIN1) {
		uLcdFbAddr=oROTATOR.m_uLcdFbAddr1;
		uLcdHeight=oROTATOR.m_uLcdHeight1;
		uLcdWidth=oROTATOR.m_uLcdWidth1;
		uLcdOffsetX=oROTATOR.m_uLcdOffsetX1;
		uLcdOffsetY=oROTATOR.m_uLcdOffsetY1;		
	}
	else Assert(0);

	LCD_SetPort();
	LCD_InitLDI(MAIN);
	LCD_InitBase();
	LCD_InitWin(eBpp, uLcdWidth, uLcdHeight, 0, 0, uLcdWidth, uLcdHeight, uLcdOffsetX, uLcdOffsetY, uLcdFbAddr, eWin, 0);	
	LCD_SetWinOnOff(1, eWin);

}

void ROTATOR_Init(u8 ucTestMode)
{

	u32 uLcdWidth0, uLcdHeight0, uLcdWidth1, uLcdHeight1;
	CSPACE eBgBpp;
	LCD_WINDOW eBgWin0, eBgWin1;
	u32 uLcdFbAddr0, uLcdFbAddr1;
	u32 uRotSrcAddr;
	u16 usRotWidth, usRotHeight;
	u32 uOffsetX, uOffsetY;


	eBgWin0=WIN0;
	eBgWin1=WIN1;
	eBgBpp=RGB16;


	uLcdFbAddr0  = uLcdStAddr;
	uLcdFbAddr1  = uLcdStAddr+0x0800000;
	uRotSrcAddr  = uLcdStAddr+0x1000000;

	oROTATOR.m_uLcdFbAddr0=uLcdFbAddr0;
	oROTATOR.m_uLcdFbAddr1=uLcdFbAddr1;


	#if (LCD_MODULE_TYPE== LTV350QV_RGB)
		UART_Printf("Selected LCD Module Type: LTV350QV_RGB\n");
		uLcdWidth0=320;
		uLcdHeight0=240;
		uLcdWidth1=uLcdHeight1=240;
		uOffsetX=(320-240)/2;
		uOffsetY=0;
		oROTATOR.m_uLcdOffsetX1=uOffsetX;
		oROTATOR.m_uLcdOffsetY1=uOffsetY;	
	#elif (LCD_MODULE_TYPE == LTS222QV_CPU)
		UART_Printf("Selected LCD Module Type: LTS222QV_CPU\n");
		uLcdWidth0=240;
		uLcdHeight0=320;	
		uLcdWidth1=uLcdHeight1=240;
		uOffsetX=0;
		uOffsetY=(320-240)/2;
		oROTATOR.m_uLcdOffsetX1=uOffsetX;
		oROTATOR.m_uLcdOffsetY1=uOffsetY;	
	#elif (LCD_MODULE_TYPE == LTE480WV_RGB)
		UART_Printf("Selected LCD Module Type: LTE480WV_RGB\n");
		uLcdWidth0=800;
		uLcdHeight0=480;	
		uLcdWidth1=uLcdHeight1=800;
		uOffsetX=0;
		uOffsetY=(800-480)/2;
		oROTATOR.m_uLcdOffsetX1=uOffsetX;
		oROTATOR.m_uLcdOffsetY1=uOffsetY;	
	#elif (LCD_MODULE_TYPE == LTP700WV_RGB)
		UART_Printf("Selected LCD Module Type: LTP700WV_RGB\n");
		uLcdWidth0=800;
		uLcdHeight0=480;	
		uLcdWidth1=uLcdHeight1=800;
		uOffsetX=0;
		uOffsetY=(800-480)/2;
		oROTATOR.m_uLcdOffsetX1=uOffsetX;
		oROTATOR.m_uLcdOffsetY1=uOffsetY;
	#else
		Assert(0);
	#endif
	oROTATOR.m_uLcdWidth0=uLcdWidth0;
	oROTATOR.m_uLcdWidth1=uLcdWidth1;
	oROTATOR.m_uLcdHeight0=uLcdHeight0;
	oROTATOR.m_uLcdHeight1=uLcdHeight1;
	oROTATOR.m_uLcdOffsetX0=0;
	oROTATOR.m_uLcdOffsetY0=0;

	POST_InitCh(POST_A, &oPost);
	POST_InitCh(POST_B, &oSc);


	LCD_SetPort();
	LCD_InitLDI(MAIN);

	LCD_SetPort();
	LCD_InitLDI(MAIN);
	LCD_InitBase();
	
	LCD_InitWin(eBgBpp, uLcdWidth0, uLcdHeight0, 0, 0, uLcdWidth0, uLcdHeight0, 0, 0, uLcdFbAddr0, eBgWin0, false);
	LCD_SetWinOnOff(1, eBgWin0);
	GLIB_InitInstance(uLcdFbAddr0, uLcdWidth0, uLcdHeight0, eBgBpp);
	GLIB_DrawPattern(uLcdWidth0, uLcdHeight0);
	usRotWidth=(u16)uLcdHeight0;
	usRotHeight=(u16)uLcdWidth0;

	if(ucTestMode==DOUBLE_WINDOW) {
		LCD_InitWin(eBgBpp, uLcdWidth1, uLcdHeight1, 0, 0, uLcdWidth1, uLcdHeight1, uOffsetX, uOffsetY, uLcdFbAddr1, eBgWin1, false);
		LCD_SetWinOnOff(1, eBgWin1);	
		GLIB_InitInstance(uLcdFbAddr1, uLcdWidth1, uLcdHeight1, eBgBpp);
		GLIB_DrawPattern2(uLcdWidth1, uLcdHeight1);
		usRotWidth=(u16)uLcdHeight1;
		usRotHeight=(u16)uLcdWidth1;
	}
//	GLIB_PutLine(0, 0, 0, uLcdHeight0-1, C_BLUE);
	
	LCD_Start();

	GLIB_InitInstance(uRotSrcAddr, usRotWidth, usRotHeight, eBgBpp);
	oROTATOR.m_bRotInit=true;
	ROTATOR_InitSetting(eBgBpp, usRotWidth, usRotHeight, uRotSrcAddr, uLcdFbAddr0, ROTATOR_TYPE_ROT_90);
	oROTATOR.m_bRotInit=false;
	
	UART_Printf("lcdwidht=%d, lcdheight=%d\n", uLcdWidth0, uLcdHeight0);
	UART_Printf("rotator width=%d, rotator height=%d\n", usRotWidth, usRotHeight);
}


