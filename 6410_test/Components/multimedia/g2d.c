/**************************************************************************************
* 
*	Project Name : S3C6400 Validation
*
*	Copyright 2006 by Samsung Electronics, Inc.
*	All rights reserved.
*
*	Project Description :
*		This software is only for validating functions of the S3C6400.
*		Anybody can use this software without our permission.
*  
*--------------------------------------------------------------------------------------
* 
*	File Name : g2d.c
*  
*	File Description : This file includes the API functions for 2D.
*
*	Author : Sekwang Kim
*	Dept. : AP Development Team
*	Created Date : 2007/01/25
*	Version : 0.1a 
* 
*	History
*	- Created by Jang JiHyun, Lee YunSoo
* 	- Modified by Sekwang Kim(2007/01/25)
*  
**************************************************************************************/


#include "system.h"
//#include "vicint.h"
#include "lcd.h"
#include "glib.h"
#include "glib_font.h"
#include "g2d.h"


// Header file for text drawing
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
//#include <timer.h>

// boaz.kim 070903
//#include "sun320240_16bpp.h"
//#include "sun320240_24bpp.h"
#include "sun240320_16bpp.h"
#include "sun240320_24bpp.h"



#define G2D_BASE_TEMP			0x73000000 //temporarily used for the IP verfication of 2D v2.0

#define	rG2D_CONTROL             	(G2D_BASE_TEMP+0x00)
#define	rG2D_INTEN                   (G2D_BASE_TEMP+0x04)
#define	rG2D_FIFO_INTC      (G2D_BASE_TEMP+0x08)
#define	rG2D_INTC_PEND		(G2D_BASE_TEMP+0x0c)
#define	rG2D_FIFO_STAT           (G2D_BASE_TEMP+0x10)

//#define	rG2D_FB_BA                   (G2D_BASE_TEMP+0x20)		//Spec out in V2.0

#define	rG2D_CMDR0                  (G2D_BASE_TEMP+0x100)
#define	rG2D_CMDR1                   (G2D_BASE_TEMP+0x104)
#define	rG2D_CMDR2                   (G2D_BASE_TEMP+0x108)
#define	rG2D_CMDR3                   (G2D_BASE_TEMP+0x10c)
#define	rG2D_CMDR4                   (G2D_BASE_TEMP+0x110)
#define	rG2D_CMDR5                   (G2D_BASE_TEMP+0x114)
#define	rG2D_CMDR6                   (G2D_BASE_TEMP+0x118)	//Reserved
#define	rG2D_CMDR7                   (G2D_BASE_TEMP+0x11c)

//#define	rG2D_COLOR_MODE          (G2D_BASE_TEMP+0x200) //changed in v2.0

//#define	rG2D_HORI_RES               (G2D_BASE_TEMP+0x204)

//newly in v2.0
#define	rG2D_SRC_RES				(G2D_BASE_TEMP+0x200)
#define	rG2D_SRC_HORI_RES			(G2D_BASE_TEMP+0x204)
#define	rG2D_SRC_VERT_RES			(G2D_BASE_TEMP+0x208)

#define	rG2D_SC_RES					(G2D_BASE_TEMP+0x210)		//SC_RES_REG
#define	rG2D_SC_HORI_RES			(G2D_BASE_TEMP+0x214)		//SC_HORI_RES _REG
#define	rG2D_SC_VERT_RES			(G2D_BASE_TEMP+0x218) 	//SC_VERT_RES _REG

//----------
#define	rG2D_CW_LT					(G2D_BASE_TEMP+0x220)
#define	rG2D_CW_LT_X	    	   	(G2D_BASE_TEMP+0x224)
#define	rG2D_CW_LT_Y   		    	(G2D_BASE_TEMP+0x228)
#define	rG2D_CW_RB   		    	(G2D_BASE_TEMP+0x230)
#define	rG2D_CW_RB_X   				(G2D_BASE_TEMP+0x234)
#define	rG2D_CW_RB_Y   				(G2D_BASE_TEMP+0x238)

#define	rG2D_COORD0              	(G2D_BASE_TEMP+0x300)
#define	rG2D_COORD0_X               (G2D_BASE_TEMP+0x304)
#define	rG2D_COORD0_Y               (G2D_BASE_TEMP+0x308)
#define	rG2D_COORD1              	(G2D_BASE_TEMP+0x310)
#define	rG2D_COORD1_X               (G2D_BASE_TEMP+0x314)
#define	rG2D_COORD1_Y               (G2D_BASE_TEMP+0x318)
#define	rG2D_COORD2              	(G2D_BASE_TEMP+0x320)
#define	rG2D_COORD2_X               (G2D_BASE_TEMP+0x324)
#define	rG2D_COORD2_Y               (G2D_BASE_TEMP+0x328)
#define	rG2D_COORD3              	(G2D_BASE_TEMP+0x330)
#define	rG2D_COORD3_X               (G2D_BASE_TEMP+0x334)
#define	rG2D_COORD3_Y               (G2D_BASE_TEMP+0x338)

#define	rG2D_ROT_OC              	(G2D_BASE_TEMP+0x340)
#define	rG2D_ROT_OC_X                (G2D_BASE_TEMP+0x344)
#define	rG2D_ROT_OC_Y                (G2D_BASE_TEMP+0x348)
#define	rG2D_ROTATE             	  (G2D_BASE_TEMP+0x34c)

#define	rG2D_ENDIAN    			 	(G2D_BASE_TEMP+0x350)

#define	rG2D_X_INCR                  (G2D_BASE_TEMP+0x400)
#define	rG2D_Y_INCR                  (G2D_BASE_TEMP+0x404)

#define	rG2D_ROP                 	(G2D_BASE_TEMP+0x410)
#define	rG2D_ALPHA                   (G2D_BASE_TEMP+0x420)

#define	rG2D_FG_COLOR                (G2D_BASE_TEMP+0x500)
#define	rG2D_BG_COLOR                (G2D_BASE_TEMP+0x504)
#define	rG2D_BS_COLOR                (G2D_BASE_TEMP+0x508)

//newly in v2.0
#define	rG2D_SRC_COLOR_MODE                (G2D_BASE_TEMP+0x510)
#define	rG2D_DEST_COLOR_MODE                (G2D_BASE_TEMP+0x514)

#define	rG2D_PATTERN_ADDR            (G2D_BASE_TEMP+0x600)
#define	rG2D_PATOFF		          	(G2D_BASE_TEMP+0x700)
#define	rG2D_PATOFF_X               (G2D_BASE_TEMP+0x704)
#define	rG2D_PATOFF_Y               (G2D_BASE_TEMP+0x708)                                      																																								
#define	rG2D_COLORKEY_CNTL           (G2D_BASE_TEMP+0x720)	//STENCIL_CNTL_REG	
#define	rG2D_COLORKEY_DR_MIN         (G2D_BASE_TEMP+0x724)	//STENCIL_DR_MIN_REG
#define	rG2D_COLORKEY_DR_MAX         (G2D_BASE_TEMP+0x728)	//STENCIL_DR_MAX_REG

//Newly added
#define	rG2D_SRC_BASE_ADDR         (G2D_BASE_TEMP+0x730)
#define	rG2D_DEST_BASE_ADDR       (G2D_BASE_TEMP+0x734) //mostly frame buffer address


volatile u8 bG2dDone;
volatile unsigned uIntCnt;

G2D oG2d;



void G2D_DrawPattern(void)
{
	G2D_DrawPattern1(0, 0, oG2d.m_uMaxDx+1, oG2d.m_uMaxDy+1);
}

void G2D_InitIp(u32 uFbStAddr, CSPACE eBpp)
{
	G2D_InitIp2(uFbStAddr, eBpp, 240, 320, 0, 0, 240, 320);
}

void G2D_InitIp1(u32 uFbStAddr, CSPACE eBpp, u32 uCwMaxHSz, u32 uCwMaxVSz)
{
	G2D_InitIp2(uFbStAddr, eBpp, uCwMaxHSz, uCwMaxVSz, 0, 0, 240, 320);
}

void G2D_InitIp2(u32 uFbStAddr, CSPACE eBpp, u32 uCwMaxHSz, u32 uCwMaxVSz,
                                                  u32 uX1_Cw, u32 uY1_Cw, u32 uX2_Cw, u32 uY2_Cw)
{
	Assert(uCwMaxHSz <= 2048); // Max horizontal size of clipping window should be 2048.
	Assert(uCwMaxVSz <= 2048); // Max vertical size of clipping window should be 2048.

	G2D_Reset(); // SW reset

	// Initialize color
	if (eBpp == RGB16)
	{
		oG2d.m_uColorVal[G2D_BLACK] = 0x0;
		oG2d.m_uColorVal[G2D_RED]   = 0x1f<<11;
		oG2d.m_uColorVal[G2D_GREEN] = 0x3f<<5;
		oG2d.m_uColorVal[G2D_BLUE]  = 0x1f<<0;
		oG2d.m_uColorVal[G2D_WHITE] = 0xffff;
	}
	else if (eBpp == RGB24)
	{
		oG2d.m_uColorVal[G2D_BLACK] = 0x0;
		oG2d.m_uColorVal[G2D_RED]   = 0xff0000;
		oG2d.m_uColorVal[G2D_GREEN] = 0xff00;
		oG2d.m_uColorVal[G2D_BLUE]  = 0xff;
		oG2d.m_uColorVal[G2D_WHITE] = 0xffffff;
	}
	else
		Assert(0);

	oG2d.m_uFbStAddr = uFbStAddr;

	oG2d.m_uColorVal[G2D_YELLOW] = (oG2d.m_uColorVal[G2D_RED] | oG2d.m_uColorVal[G2D_GREEN]);
	oG2d.m_uColorVal[G2D_CYAN] = (oG2d.m_uColorVal[G2D_GREEN] | oG2d.m_uColorVal[G2D_BLUE]);
	oG2d.m_uColorVal[G2D_MAGENTA] = (oG2d.m_uColorVal[G2D_RED] | oG2d.m_uColorVal[G2D_BLUE]);

	oG2d.m_uHoriRes = uCwMaxHSz;

	//oG2d.m_uMaxDx = uCwMaxHSz - 1;
	//oG2d.m_uMaxDy = uCwMaxVSz - 1;

	oG2d.m_uMaxDx = uCwMaxHSz; //in v2.0
	oG2d.m_uMaxDy = uCwMaxVSz;
	

	oG2d.m_uCwX1 = uX1_Cw;
	oG2d.m_uCwY1 = uY1_Cw;

	oG2d.m_uCwX2 = uX2_Cw;
	oG2d.m_uCwY2 = uY2_Cw;

	oG2d.m_uBytes = (eBpp == RGB16) ? 2 : 4;

	oG2d.m_upFontType = (u8 *)font8x15;
	oG2d.m_uFontWidth = 8;
	oG2d.m_uFontHeight = 15;

	oG2d.m_eBpp = eBpp;

	oG2d.m_bIsBitBlt = false;

	G2D_DisableEffect(); // Disable per-pixel/per-plane alpha blending and fading
	G2D_SetColorKeyOff();
	G2D_InitRegs(eBpp);
}

void G2D_InitRegs(CSPACE eBpp)
{
	u32 uBppMode;

	G2D_CheckFifo(17);

	uBppMode = 	(eBpp == RGB16) ? G2D_COLOR_RGB_565:
				(eBpp == RGBA16) ? G2D_COLOR_RGBA_5551 :
				(eBpp == ARGB16) ? G2D_COLOR_ARGB_1555 :
				(eBpp == RGBA24) ? G2D_COLOR_RGBA_8888 :
				(eBpp == ARGB24) ? G2D_COLOR_ARGB_8888 :
				(eBpp == RGB24) ? G2D_COLOR_XRGB_8888 :
				(eBpp == RGBX24) ? G2D_COLOR_RGBX_8888 : G2D_COLOR_XRGB_8888;

	//Outp32(rG2D_COLOR_MODE, uBppMode);

	Outp32(rG2D_SRC_COLOR_MODE, uBppMode);		//put same in src and dest this time
	Outp32(rG2D_DEST_COLOR_MODE, uBppMode);

	
	//Outp32(rG2D_FB_BA, oG2d.m_uFbStAddr);
	Outp32(rG2D_DEST_BASE_ADDR, oG2d.m_uFbStAddr); //v2.0	

	Outp32(rG2D_SRC_BASE_ADDR, oG2d.m_uFbStAddr); //v2.0	

	//Outp32(rG2D_HORI_RES, oG2d.m_uHoriRes);	//lcdX should be 4n

	Outp32(rG2D_SRC_HORI_RES, oG2d.m_uHoriRes);	//new in v2.0
	Outp32(rG2D_SRC_VERT_RES, oG2d.m_uVertRes);	//new in v2.0

	Outp32(rG2D_SC_HORI_RES, oG2d.m_uMaxDx);
	Outp32(rG2D_SC_VERT_RES, oG2d.m_uMaxDy);
	
	Outp32(rG2D_CW_LT_X, oG2d.m_uCwX1);
	Outp32(rG2D_CW_LT_Y, oG2d.m_uCwY1);
	Outp32(rG2D_CW_RB_X, oG2d.m_uCwX2);
	Outp32(rG2D_CW_RB_Y, oG2d.m_uCwY2);

	Outp32(rG2D_ALPHA, FADING_OFFSET_DISABLE | ALPHA_VALUE_DISABLE);

	Outp32(rG2D_FG_COLOR, oG2d.m_uColorVal[G2D_WHITE]); // set color to both font and foreground color
	Outp32(rG2D_BG_COLOR, oG2d.m_uColorVal[G2D_BLACK]);
	Outp32(rG2D_BS_COLOR, oG2d.m_uColorVal[G2D_BLUE]); // Set blue color to blue screen color

	Outp32(rG2D_ROP, G2D_OPERAND3_FG_BIT | G2D_NO_ALPHA_BIT | OPAQUE_ENABLE | G2D_ROP_SRC_ONLY);

	Outp32(rG2D_ROT_OC_X, 0);
	Outp32(rG2D_ROT_OC_Y, 0);
	Outp32(rG2D_ROTATE, G2D_ROTATION_0_DEG_BIT);
	Outp32(rG2D_ALPHA, 0);
}

void G2D_Reset(void)
{
    Outp32(rG2D_CONTROL, 1);  //assert G2D reset
    Outp32(rG2D_CONTROL, 0);  //deassert G2D reset
}

void G2D_WaitForIdleStatus(void)
{
	while(!(Inp32(rG2D_FIFO_STAT) & G2D_DE_STATUS_FA_BIT));
}

void G2D_CheckFifo(u32 uEmptyFifo)
{
	while( ((Inp32(rG2D_FIFO_STAT)&0x7e)>>1) > (FIFO_NUM - uEmptyFifo) );
}

// Set Ternary raster operation
// Support 256 raster operation
// Refer to ternary raster operation table if you know 256 ROP

// Set Alpha Value
void G2D_SetAlphaValue(u8 ucAlphaVal)
{
	ucAlphaVal &= 0xff;
	Outp32(rG2D_ALPHA, (Inp32(rG2D_ALPHA)&=~0xff) | ucAlphaVal);
}

// Set alpha blending mode
void G2D_SetAlphaMode(G2D_ALPHA_BLENDING_MODE eMode)
{
	u32 uAlphaBlend;

	uAlphaBlend =
		(eMode == G2D_NO_ALPHA_MODE) ? G2D_NO_ALPHA_BIT :
		(eMode == G2D_PP_ALPHA_SOURCE_MODE) ? G2D_PP_ALPHA_SOURCE_BIT :
		(eMode == G2D_ALPHA_MODE) ? G2D_ALPHA_BIT : 
		(eMode == G2D_FADING_MODE) ? G2D_FADING_BIT : G2D_NO_ALPHA_BIT;

	Outp32(rG2D_ROP, (Inp32(rG2D_ROP) &= ~(0x7<<10)) | uAlphaBlend);
}

// Set fade value
void G2D_SetFadingValue(u8 ucFadeVal)
{
	ucFadeVal &= 0xff;
	Outp32(rG2D_ALPHA, (Inp32(rG2D_ALPHA)&= ~(0xff<<8)) | (ucFadeVal<<8));
}

void G2D_DisableEffect(void)
{
	Outp32(rG2D_ROP, (Inp32(rG2D_ROP)&~(0x7<<10)));
}

void G2D_EnablePlaneAlphaBlending(u8 ucAlphaVal)
{
	ucAlphaVal &= 0xff;

	// Set Alpha Blending Mode
	Outp32(rG2D_ROP, (Inp32(rG2D_ROP) &= ~(0x7<<10)) | G2D_ALPHA_BIT);


	// Set Alpha Value
	Outp32(rG2D_ALPHA, (Inp32(rG2D_ALPHA) &= ~(0xff)) | ucAlphaVal);

	oG2d.m_ucAlphaVal = ucAlphaVal;
	oG2d.m_bIsAlphaCall = true;
}

void G2D_DisablePlaneAlphaBlending(void)
{
	G2D_DisableEffect();
}

void G2D_EnablePixelAlphaBlending(void) // Only Support 24bpp and Only used in BitBlt
{

	Assert(oG2d.m_eBpp == RGB24);

	Outp32(rG2D_ROP, (Inp32(rG2D_ROP) &= ~(0x7<<10)) | G2D_PP_ALPHA_SOURCE_BIT);
}

void G2D_DisablePixelAlphaBlending(void) // Only Support 24bpp and only used in BitBlt
{
	Assert(oG2d.m_eBpp == RGB24);
	G2D_DisableEffect();
}

void G2D_EnableFadding(u8 ucFadingVal)
{
	u8 ucAlphaVal;

	ucAlphaVal = (oG2d.m_bIsAlphaCall == true) ? oG2d.m_ucAlphaVal : 255;

	ucFadingVal &= 0xff;

	// Set Fadding Mode	
	Outp32(rG2D_ROP, (Inp32(rG2D_ROP) &= ~(0x7<<10)) | G2D_FADING_BIT);

	// Set Fadding Value	
	Outp32(rG2D_ALPHA, (Inp32(rG2D_ALPHA) &= ~(0xff<<8)) | (ucFadingVal<<8) | (ucAlphaVal<<0));
}

void G2D_DisableFadding(void)
{
	G2D_DisableEffect();
}

void G2D_SetRotationOrgX(u16 usRotOrgX)
{
	Outp16(rG2D_ROT_OC_X, usRotOrgX);
}

void G2D_SetRotationOrgY(u16 usRotOrgY)
{
	Outp16(rG2D_ROT_OC_Y, usRotOrgY);
}

// Clear Frame irrelevant to clipping window size


// Draw Pixel







void G2D_DrawPattern1(u32 uPosX, u32 uPosY, u32 uHsz, u32 uVsz)
{
	u32 uSubHsz, uSubVsz;

	Assert( !((uHsz%8)  || (uVsz%8)) );
	Assert( (uPosX+uHsz<=(oG2d.m_uMaxDx+1)) && (uPosY+uVsz<=(oG2d.m_uMaxDy+1)) );

	G2D_FillRectangleEcolor(uPosX, uPosY, uHsz, uVsz, G2D_WHITE);

	// Draw Color Bar
	uSubHsz = uHsz/8;
	uSubVsz = uVsz/2;

	G2D_FillRectangleEcolor(uPosX, uPosY, uSubHsz, uSubVsz, G2D_RED);
	G2D_FillRectangleEcolor(uSubHsz*1, uPosY, uSubHsz, uSubVsz, G2D_GREEN);
	G2D_FillRectangleEcolor(uSubHsz*2, uPosY, uSubHsz, uSubVsz, G2D_BLUE);
	G2D_FillRectangleEcolor(uSubHsz*3, uPosY, uSubHsz, uSubVsz, G2D_CYAN);
	G2D_FillRectangleEcolor(uSubHsz*4, uPosY, uSubHsz, uSubVsz, G2D_MAGENTA);
	G2D_FillRectangleEcolor(uSubHsz*5, uPosY, uSubHsz, uSubVsz, G2D_YELLOW);
	G2D_FillRectangleEcolor(uSubHsz*6, uPosY, uSubHsz, uSubVsz, G2D_WHITE);
	G2D_FillRectangleEcolor(uSubHsz*7, uPosY, uSubHsz, uSubVsz, G2D_BLACK);

	// Draw Pattern
	uSubHsz = uHsz/2;
	G2D_PutRectangleEcolor(uPosX, uSubVsz, uHsz, uSubVsz, G2D_RED);
	G2D_PutLineEcolor(uPosX, uSubVsz, uHsz-1, uVsz-1, G2D_BLACK, true);
	G2D_PutLineEcolor(uHsz, uSubVsz, uPosX, uVsz-1, G2D_MAGENTA, true);

	uSubHsz = uHsz/2;
	G2D_PutLineEcolor(uSubHsz, uSubVsz, uSubHsz-1, uVsz-1, G2D_GREEN, true);

	uSubVsz = uVsz*3/4;
	G2D_PutLineEcolor(uPosX, uSubVsz, uHsz-1, uSubVsz-1, G2D_BLUE, true);
	
}

void G2D_SetXYIncrFormat(u32 uDividend, u32 uDivisor, u32* uResult)
{
    int i;
    u32 uQuotient;
    u32 uUnderPoint=0;

	Assert(uDivisor != 0);

    uQuotient = (u32)(uDividend/uDivisor);

	Assert(uQuotient <= 2048); // Quotient should be less than 2048.

    uDividend-=(uQuotient*uDivisor);

    for (i=0; i<12; i++)
    {
		uDividend <<= 1;
		uUnderPoint <<= 1;
		if (uDividend >= uDivisor)
		{
		    uUnderPoint = uUnderPoint | 1;
		    uDividend -= uDivisor;
		}
    }

    uUnderPoint = (uUnderPoint + 1) >> 1;

	*uResult =  uUnderPoint|(uQuotient<<11);
}

void G2D_BitBlt(u16 usSrcStX, u16 usSrcStY, u16 usSrcEndX, u16 usSrcEndY,
  	                  u16 usDstStX, u16 usDstStY, u16 usDstEndX, u16 usDstEndY, u8 bIsStretch)
{
	u16 usSrcWidth, usSrcHeight;
	u16 usDstWidth, usDstHeight;
	u32 uXYIncr;
	u32 uCmdRegVal;

	// Check boundary of X coordiante
	Assert( (usSrcStX >= oG2d.m_uCwX1)&&(usSrcEndX >= oG2d.m_uCwX1) );
	// Coordinate X of source image or destination image should be less than that of clipping window.
	Assert( (usSrcEndX <= oG2d.m_uCwX2)&&(usDstEndX <= oG2d.m_uCwX2) );

	// Check boundary of Y coordinate
	Assert( (usSrcStY >= oG2d.m_uCwY1)&&(usSrcEndY >= oG2d.m_uCwY1) );
	// Coordinate Y of source image or destination image should be less than that of clipping window.
	Assert( (usSrcEndY <= oG2d.m_uCwY2)&&(usDstEndY <= oG2d.m_uCwY2) );

    G2D_CheckFifo(17);



    Outp32(rG2D_COORD0_X, usSrcStX);
    Outp32(rG2D_COORD0_Y, usSrcStY);
    Outp32(rG2D_COORD1_X, usSrcEndX);
    Outp32(rG2D_COORD1_Y, usSrcEndY);

    Outp32(rG2D_COORD2_X, usDstStX);
    Outp32(rG2D_COORD2_Y, usDstStY);
    Outp32(rG2D_COORD3_X, usDstEndX);
    Outp32(rG2D_COORD3_Y, usDstEndY);

	if(bIsStretch==true) {
		usSrcWidth=ABS(usSrcStX-usSrcEndX);
		usDstWidth=ABS(usDstStX-usDstEndX);
		usSrcHeight=ABS(usSrcStY-usSrcEndY);
		usDstHeight=ABS(usDstStY-usDstEndY);

		G2D_SetXYIncrFormat(usSrcWidth, usDstWidth, &uXYIncr);
		Outp32(rG2D_X_INCR, uXYIncr);

		G2D_SetXYIncrFormat(usSrcHeight, usDstHeight, &uXYIncr);
		Outp32(rG2D_Y_INCR, uXYIncr);
    	}

		uCmdRegVal=Inp32(rG2D_CMDR1);
		uCmdRegVal &= ~(0x3<<0);
		uCmdRegVal |= (bIsStretch == true) ? G2D_STRETCH_BITBLT_BIT : G2D_NORMAL_BITBLT_BIT;
		Outp32(rG2D_CMDR1, uCmdRegVal);
#ifdef PROFILE
		PWM_PushTimer(); //Timer 0 will start
#endif
#ifdef INTERRUPT_MODE
		while(!bG2dDone);
		bG2dDone=0;
#endif
	
}

// Get Original Coordinate (X,Y) to rotate window
// usDestStX, usDesStY : Target Destination after rotation
// (usSrcX1, usSrcY1), (usSrcX2, usSrcY2) : Coordinate (X1,Y1), (X2, Y2) before rotation
// usRotDegree : support only 90/180/270 degrees
// usOrigX, usOrigY : Rotation Coordinate. the register value for rG2D_ROT_OC_X and rG2D_ROT_OC_Y

// formula to get usOrigX, usOrigY
// | usDestX - usOrigX |   | cosA -sinA | | usSRCX - usOrigX |
// |                   | = |            | |                  |
// | usDestY - usOrigY |   | sinA  cosA | | usSRCY - usOrigY |
//
//
//( if A == 90 degrees, usSRCX = usSrcX1 and usSRCY = usSrcY2
// else if A == 180 degrees, usSRCX = usSrcX2 and usSRCY = usSrcY2
// else if A == 270 degrees, usSRCX = usSrcX1 and usSRCY = usSrcY2 )


// cf. SRC window coordinate
//
// (usSrcX1, usSrcY1)                (usSrcX2, usSrcY1)
//         *---------------------------------*
//         |                                 |
//         |                                 |
//         |            Window               |
//         |                                 |
//         |                                 |
//         *---------------------------------*
// (usSrcX1, usSrcY2)                 (usSrcX2, usSrcY2)

void G2D_GetRotationOrgXY(u16 usSrcX1, u16 usSrcY1, u16 usSrcX2, u16 usSrcY2, u16 usDestX1, u16 usDestY1,
                             ROT_DEG eRotDegree, u16* usOrgX, u16* usOrgY)
{
	G2D_CheckFifo(17);

	switch(eRotDegree)
	{
		case ROT_0:
			return;
		case ROT_90:
			*usOrgX = (usDestX1 - usDestY1 + usSrcX1 + usSrcY2)/2;
			*usOrgY = (usDestX1 + usDestY1 - usSrcX1 + usSrcY2)/2;

			break;
		case ROT_180:
			*usOrgX = (usDestX1 + usSrcX2)/2;
			*usOrgY = (usDestY1 + usSrcY2)/2;

			break;
		case ROT_270:
			*usOrgX = (usDestX1 + usDestY1 + usSrcX2 - usSrcY1)/2;
			*usOrgY = (usDestY1 - usDestX1 + usSrcX2 + usSrcY1)/2;

			break;
		default:
			Assert(0); // UnSupported Rotation Degree!
			break;
	}

}

void G2D_RotateImage(
	u16 usSrcX1, u16 usSrcY1, u16 usSrcX2, u16 usSrcY2,
	u16 usDestX1, u16 usDestY1, ROT_DEG eRotDegree)
{
	u16 usOrgX, usOrgY;
	u32 uRotDegree;

	G2D_GetRotationOrgXY(usSrcX1, usSrcY1, usSrcX2, usSrcY2, usDestX1, usDestY1, eRotDegree, &usOrgX, &usOrgY);

	G2D_CheckFifo(17);

	Outp16(rG2D_ROT_OC_X, usOrgX);
	Outp16(rG2D_ROT_OC_Y, usOrgY);

	uRotDegree =
		(eRotDegree == ROT_0) ? G2D_ROTATION_0_DEG_BIT :
		(eRotDegree == ROT_90) ? G2D_ROTATION_90_DEG_BIT :
		(eRotDegree == ROT_180) ? G2D_ROTATION_180_DEG_BIT : G2D_ROTATION_270_DEG_BIT;

	Outp32(rG2D_ROTATE, uRotDegree);
}

void G2D_RotateWithBitBlt(
	u16 usSrcX1, u16 usSrcY1, u16 usSrcX2, u16 usSrcY2,
	u16 usDestX1, u16 usDestY1,   ROT_DEG eRotDegree)
{
	u16 usOrgX, usOrgY;
	u32 uRotDegree;

	G2D_GetRotationOrgXY(usSrcX1, usSrcY1, usSrcX2, usSrcY2, usDestX1, usDestY1, eRotDegree, &usOrgX, &usOrgY);

	G2D_CheckFifo(17);

	Outp16(rG2D_ROT_OC_X, usOrgX);
	Outp16(rG2D_ROT_OC_Y, usOrgY);

	uRotDegree =
		(eRotDegree == ROT_0) ? G2D_ROTATION_0_DEG_BIT :
		(eRotDegree == ROT_90) ? G2D_ROTATION_90_DEG_BIT :
		(eRotDegree == ROT_180) ? G2D_ROTATION_180_DEG_BIT : G2D_ROTATION_270_DEG_BIT ;

	Outp32(rG2D_ROTATE, uRotDegree);

	G2D_BitBlt(usSrcX1, usSrcY1, usSrcX2, usSrcY2, usSrcX1, usSrcY1, usSrcX2, usSrcY2, false);
}

// if ucTransMode is '1', Transparent Mode
// else '0', Opaque Mode
void G2D_SetTransparentMode1(u8 bIsTransparent, u32 uBsColor)
{
	u32 uRopRegVal;

	G2D_CheckFifo(17);

	uRopRegVal=Inp32(rG2D_ROP);

	uRopRegVal =
		(bIsTransparent == 1) ? (uRopRegVal | G2D_TRANSPARENT_BIT) : (uRopRegVal & ~(G2D_TRANSPARENT_BIT));

	Outp32(rG2D_ROP, uRopRegVal);

	// register Blue Screen Color
	Outp32(rG2D_BS_COLOR, uBsColor);
}

void G2D_SetTransparentMode(u8 bIsTransparent, G2D_COLOR eBsColor)
{
	G2D_SetTransparentMode1(bIsTransparent, oG2d.m_uColorVal[eBsColor]);
}

// if ucTransMode is '1', Transparent Mode
// else '0', Opaque Mode
void G2D_SetColorKeyOn1(u32 uBsColor)
{
	G2D_CheckFifo(17);

	Outp32(rG2D_ROP, Inp32(rG2D_ROP) | G2D_TRANSPARENT_BIT);

	// register Blue Screen Color
	Outp32(rG2D_BS_COLOR, uBsColor);
}

void G2D_SetColorKeyOn(G2D_COLOR eBsColor)
{
	G2D_SetColorKeyOn1(oG2d.m_uColorVal[eBsColor]);
}

void G2D_SetColorKeyOff(void)
{
	G2D_CheckFifo(17);

	// Blue screen off
	Outp32(rG2D_ROP, Inp32(rG2D_ROP) & ~(G2D_TRANSPARENT_BIT));

	// color key off	
	Outp32(rG2D_COLORKEY_CNTL, (Inp32(rG2D_COLORKEY_CNTL) &= ~(0x1f)));
}



void G2D_SetPatternOffsetX(u8 ucPatOffsetX)
{
	ucPatOffsetX &= 0x7; // ucPatOffsetX[2:0]

	G2D_CheckFifo(17);
	Outp32(rG2D_PATOFF_X, ucPatOffsetX);
}

void G2D_SetPatternOffsetY(u8 ucPatOffsetY)
{
	ucPatOffsetY &= 0x7; // ucPatOffsetY[2:0]

	G2D_CheckFifo(17);
	Outp32(rG2D_PATOFF_Y, ucPatOffsetY);
}





void G2D_SetFgEcolor(G2D_COLOR eFgColor)
{
	G2D_SetFgColor(oG2d.m_uColorVal[eFgColor]);
}

void G2D_SetFgColor(u32 uFgColor)
{
	uFgColor &= 0x00ffffff;
	Outp32(rG2D_FG_COLOR, uFgColor);
}
void G2D_SetBgEcolor(G2D_COLOR eBgColor)
{
	G2D_SetBgColor(oG2d.m_uColorVal[eBgColor]);
}

void G2D_SetBgColor(u32 uBgColor)
{
	uBgColor &= 0x00ffffff;
	Outp32(rG2D_BG_COLOR, uBgColor);
}


void G2D_SetFbAddr(u32 uFbAddr, u32 uWhichBuffer)
{
	if(uWhichBuffer==SRCBUF)
		Outp32(rG2D_SRC_BASE_ADDR, uFbAddr);
	else if(uWhichBuffer==DSTBUF)
		Outp32(rG2D_DEST_BASE_ADDR, uFbAddr);
	
}


void G2D_SetBppMode(CSPACE eBpp, u32 uWhichBuffer)
{

	u32 uBpp;
	
	switch(eBpp)
	{
		case RGB16:
			uBpp = 0;
			break;
		case RGBA16:
			uBpp = 1;
			break;
		case ARGB16:
			uBpp = 2;
			break;
		case RGBA24:
			uBpp = 3;
			break;
		case ARGB24:
			uBpp = 4;
			break;
		case RGB24:
			uBpp = 5;
			break;
		case RGBX24:
			uBpp = 6;
			break;
		default:
			uBpp = 5;
			break;			
	}

	if(uWhichBuffer==SRCBUF)
		Outp32(rG2D_SRC_COLOR_MODE, (Inp32(rG2D_SRC_COLOR_MODE)&~0x7)|(uBpp));
	else if(uWhichBuffer==DSTBUF)
		Outp32(rG2D_DEST_COLOR_MODE, (Inp32(rG2D_DEST_COLOR_MODE)&~0x7)|(uBpp));	
}



// if bIsTransparent == 1, Transparent Mode and uBgColor isn't applied
// else if bIsTransparent ==0, Opaque Mode
void G2D_SetFontColor1(u32 uFontColor, u32 uBgColor, u8 bIsTransparent)
{
	u32 uRopRegVal;

	G2D_CheckFifo(17);

	uRopRegVal=Inp32(rG2D_ROP);

	uRopRegVal =
		(bIsTransparent == 1) ? (uRopRegVal | G2D_TRANSPARENT_BIT) : (uRopRegVal & ~(G2D_TRANSPARENT_BIT));

	Outp32(rG2D_ROP, uRopRegVal);


	Outp32(rG2D_FG_COLOR, uFontColor);
	Outp32(rG2D_BG_COLOR, uBgColor);
}

void G2D_SetFontColor(G2D_COLOR eFontColor, G2D_COLOR eBgColor, u8 bIsTransparent)
{
	G2D_SetFontColor1(oG2d.m_uColorVal[eFontColor], oG2d.m_uColorVal[eBgColor], bIsTransparent);
}

/* v.12
void G2D_PutString(u32 uPosX, u32 uPosY, char* cStr)
{
	u8* ucOneLetter;
	u32 uHostData = 0x0;
	u32 uTempData = 0x0;
	int i, j, k;

	u32 uPosX2, uPosY2;
	u32 uFontOffset;
	u32 uAddressOffset;
	// Only support 8X15 font size
	// All font size will be implemented, but now only support 8X15 font size

	if(oG2d.m_bIsBitBlt==true) UART_Printf("Host to screen BitBLT!\n");
		
	if(oG2d.m_uFontWidth==8 && oG2d.m_uFontHeight==8)
		uFontOffset = 8;
	else if(oG2d.m_uFontWidth==8 && oG2d.m_uFontHeight==15)
		uFontOffset = 15;
	else if(oG2d.m_uFontWidth==8 && oG2d.m_uFontHeight==16)
		uFontOffset = 16;	
	
	uPosX2 = uPosX + oG2d.m_uFontWidth - 1;
	uPosY2 = uPosY + oG2d.m_uFontHeight - 1;

	G2D_CheckFifo(17);


	for (i=0; i<strlen(cStr); i++)
	{
		if (oG2d.m_bIsBitBlt == true)
		{
			Outp32(rG2D_COORD2_X, uPosX); // COORD_0: LeftTop
			Outp32(rG2D_COORD2_Y, uPosY);
			Outp32(rG2D_COORD3_X, uPosX2); // COORD_1: RightBottom
			Outp32(rG2D_COORD3_Y, uPosY2);
			//UART_Printf("HS BitBLT!\n");
		}
		if (oG2d.m_bIsBitBlt == false)
		{
			Outp32(rG2D_COORD0_X, uPosX); // COORD_0: LeftTop
			Outp32(rG2D_COORD0_Y, uPosY);
			Outp32(rG2D_COORD1_X, uPosX2); // COORD_1: RightBottom
			Outp32(rG2D_COORD1_Y, uPosY2);
		}

		//ucOneLetter = oG2d.m_upFontType + (cStr[i]-START_ASCII)*15; // get start address of chosen letter in font8x15
		ucOneLetter = oG2d.m_upFontType + (cStr[i]-START_ASCII)*uFontOffset; // get start address of chosen letter in font8x15

		uHostData = uTempData = 0;

		for (j=0; j<4; j++) // Generate of first 1 word of data for a font (start)
		{
			uTempData = (u32)ucOneLetter[j];
			uHostData |= (uTempData << (24 - 8*j));
		}

		if (oG2d.m_bIsScr2Scr==1 && oG2d.m_bIsBitBlt ==0) {
			uAddressOffset=((oG2d.m_uFontWidth/8*oG2d.m_uFontHeight+3)/4)*4;			
			G2D_SetCMDR7(oG2d.m_uFontAddr+(cStr[i]-START_ASCII)*uAddressOffset);
#ifdef INTERRUPT_MODE
			while(!bG2dDone);
			bG2dDone=0;
#endif							
		}
		else if (oG2d.m_bIsBitBlt) {
			Outp32(rG2D_CMDR2, uHostData);
		}	
		else {
			Outp32(rG2D_CMDR4, uHostData); // Set New font
		}	

		if(oG2d.m_bIsScr2Scr != 1) 
		{
			for(k=0; k<((uFontOffset-4+3)/4); k++) // Generate next n word of data for the font (continue)
			{
				uHostData = uTempData = 0;
				for (j=0+4*(k+1); j<4+4*(k+1); j++) // Upto font height(height: 15)
				{
					uTempData = (u32)ucOneLetter[j];
					uHostData |= (uTempData<< (24 - 8*(j%4)));
				}
				if (oG2d.m_bIsScr2Scr==1 && oG2d.m_bIsBitBlt ==0) break;
				else if (oG2d.m_bIsBitBlt) {
					Outp32(rG2D_CMDR3, uHostData);
				}	
				else {
					Outp32(rG2D_CMDR5, uHostData);
				}	
			}
#ifdef INTERRUPT_MODE
				while(!bG2dDone);
				bG2dDone=0;
#endif
		}


		uPosX = uPosX2; //  Set left top X coordinate of next letter
		uPosX2 = uPosX + oG2d.m_uFontWidth - 1; // Set rigth bottom X coordiante of next letter
	}
}
*/

void G2D_PutString(u32 uPosX, u32 uPosY, char* cStr)
{
	u8* ucOneLetter;
	u32 uHostData = 0x0;
	u32 uTempData = 0x0;
	int i, j, k;

	u32 uPosX2, uPosY2;
	u32 uFontOffset;
	u32 uAddressOffset;
	
	// Only support 8X15 font size
	// All font size will be implemented, but now only support 8X15 font size
	if(oG2d.m_uFontWidth==8 && oG2d.m_uFontHeight==8)
		uFontOffset = 8;
	else if(oG2d.m_uFontWidth==8 && oG2d.m_uFontHeight==15)
		uFontOffset = 15;
	else if(oG2d.m_uFontWidth==8 && oG2d.m_uFontHeight==16)
		uFontOffset = 16;	
	
	uPosX2 = uPosX + oG2d.m_uFontWidth - 1;
	uPosY2 = uPosY + oG2d.m_uFontHeight - 1;

	G2D_CheckFifo(17);


	for (i=0; i<strlen(cStr); i++)
	{
		Outp32(rG2D_COORD0_X, uPosX); // COORD_0: LeftTop
		Outp32(rG2D_COORD0_Y, uPosY);
		Outp32(rG2D_COORD1_X, uPosX2); // COORD_1: RightBottom
		Outp32(rG2D_COORD1_Y, uPosY2);


		//ucOneLetter = oG2d.m_upFontType + (cStr[i]-START_ASCII)*15; // get start address of chosen letter in font8x15
		ucOneLetter = oG2d.m_upFontType + (cStr[i]-START_ASCII)*uFontOffset; // get start address of chosen letter in font8x15

		uHostData = uTempData = 0;

		for (j=0; j<4; j++) // Generate of first 1 word of data for a font (start)
		{
			uTempData = (u32)ucOneLetter[j];
			uHostData |= (uTempData << (24 - 8*j));
		}

		if (oG2d.m_bIsScr2Scr==1) {
			uAddressOffset=((oG2d.m_uFontWidth/8*oG2d.m_uFontHeight+3)/4)*4;			
			G2D_SetCMDR7(oG2d.m_uFontAddr+(cStr[i]-START_ASCII)*uAddressOffset);
#ifdef INTERRUPT_MODE
			while(!bG2dDone);
			bG2dDone=0;
#endif							
		}
		else {
			Outp32(rG2D_CMDR4, uHostData); // Set New font
		}	

		if(oG2d.m_bIsScr2Scr != 1) 
		{
			for(k=0; k<((uFontOffset-4+3)/4); k++) // Generate next n word of data for the font (continue)
			{
				uHostData = uTempData = 0;
				for (j=0+4*(k+1); j<4+4*(k+1); j++) // Upto font height(height: 15)
				{
					uTempData = (u32)ucOneLetter[j];
					uHostData |= (uTempData<< (24 - 8*(j%4)));
				}
				Outp32(rG2D_CMDR5, uHostData);
			}
#ifdef INTERRUPT_MODE
				while(!bG2dDone);
				bG2dDone=0;
#endif
		}


		uPosX = uPosX2; //  Set left top X coordinate of next letter
		uPosX2 = uPosX + oG2d.m_uFontWidth - 1; // Set rigth bottom X coordiante of next letter
	}
}


// If bIsHost == 0, Host to screen text drawing
// else if bIsHost == 1, Memory to screen text drawing
void G2D_Printf(u32 uPosX, u32 uPosY, const char* cpFmt, ...)
{
	va_list eAp;
	char cString[256];

	va_start(eAp, cpFmt);
	vsprintf(cString, cpFmt, eAp);
	G2D_PutString(uPosX, uPosY, cString);
	va_end(eAp);
}






//Inserted or Modified by Boaz.kim//

void G2D_Init(void) // Adding
{

	u32 uLcdWidth, uLcdHeight;
	CSPACE eBgBpp;
	LCD_WINDOW eBgWin;
	u32 uLcdFbAddr;

	eBgWin=WIN0;
//	eBgBpp=RGB24;
	eBgBpp=RGB16;


	//uLcdFbAddr = _DRAM_BaseAddress+0x04000000;
	uLcdFbAddr = CODEC_MEM_ST;

#ifdef SILICON
	#if (LCD_MODULE_TYPE== LTV350QV_RGB)
		UART_Printf("Selected LCD Module Type: LTV350QV_RGB\n");
		uLcdWidth=320;
		uLcdHeight=240;	
	#elif (LCD_MODULE_TYPE == LTS222QV_CPU)
		UART_Printf("Selected LCD Module Type: LTS222QV_CPU\n");
		uLcdWidth=240;
		uLcdHeight=320;	
	#else
		Assert(0);
	#endif
#endif


	LCD_SetPort();
	LCD_InitLDI(MAIN);

	LCD_SetPort();
	LCD_InitLDI(MAIN);
	LCD_InitBase();
	LCD_InitWin(eBgBpp, uLcdWidth, uLcdHeight, 0, 0, uLcdWidth, uLcdHeight, 0, 0, uLcdFbAddr, eBgWin, false);	
	LCD_SetWinOnOff(1, eBgWin);
	LCD_GetFrmSz(&uLcdWidth, &uLcdHeight, eBgWin);
	GLIB_InitInstance(uLcdFbAddr, uLcdWidth, uLcdHeight, eBgBpp);
	GLIB_DrawPattern(uLcdWidth, uLcdHeight);
	GLIB_PutLine(0, 0, 0, uLcdHeight-1, C_BLUE);

	LCD_Start();	
	

	UART_Printf("lcdwidht=%d, lcdheight=%d\n", uLcdWidth, uLcdHeight);
	G2D_InitSetting(uLcdFbAddr, eBgBpp, uLcdWidth, uLcdHeight, 0, 0, uLcdWidth, uLcdHeight);


}

void G2D_InitSetting(u32 uFbStAddr, CSPACE eBpp, u32 uCwMaxHSz, u32 uCwMaxVSz,
                                                  u32 uX1_Cw, u32 uY1_Cw, u32 uX2_Cw, u32 uY2_Cw) // modification
{
	Assert(uCwMaxHSz <= 2040); // Max horizontal size of clipping window should be 2048.
	Assert(uCwMaxVSz <= 2040); // Max vertical size of clipping window should be 2048.


	// Initialize color
	switch(eBpp) {
		case RGB16:
			oG2d.m_uColorVal[G2D_BLACK] = 0x0;
			oG2d.m_uColorVal[G2D_RED]   = 0x1f<<11;
			oG2d.m_uColorVal[G2D_GREEN] = 0x3f<<5;
			oG2d.m_uColorVal[G2D_BLUE]  = 0x1f<<0;
			oG2d.m_uColorVal[G2D_WHITE] = 0xffff;
			oG2d.m_uBytes =2;
			break;
		case RGBA16: // 15-bpp, The name of ARGB8 should be changed to proper name.
			oG2d.m_uColorVal[G2D_BLACK] = 0x0;
			oG2d.m_uColorVal[G2D_RED]   = (0x1f<<11);
			oG2d.m_uColorVal[G2D_GREEN] =(0x1f<<6);
			oG2d.m_uColorVal[G2D_BLUE]  =(0x1f<<1);
			oG2d.m_uColorVal[G2D_WHITE] =0xffffe;
			oG2d.m_uBytes =2;
			break;
		case ARGB16: // 15-bpp, The name of ARGB8 should be changed to proper name.
			oG2d.m_uColorVal[G2D_BLACK] = 0x0;
			oG2d.m_uColorVal[G2D_RED]   = (0x1f<<10);
			oG2d.m_uColorVal[G2D_GREEN] =(0x1f<<5);
			oG2d.m_uColorVal[G2D_BLUE]  =(0x1f<<0);
			oG2d.m_uColorVal[G2D_WHITE] =0x7fff;
			oG2d.m_uBytes =2;
			break;						
		case RGB24:
		case ARGB24:
			oG2d.m_uColorVal[G2D_BLACK] = 0x0;
			oG2d.m_uColorVal[G2D_RED]   = 0xff0000;
			oG2d.m_uColorVal[G2D_GREEN] = 0xff00;
			oG2d.m_uColorVal[G2D_BLUE]  = 0xff;
			oG2d.m_uColorVal[G2D_WHITE] = 0xffffff;
			oG2d.m_uBytes =4;
			break;
		case RGBX24:
		case RGBA24:		
			oG2d.m_uColorVal[G2D_BLACK] = 0x0;
			oG2d.m_uColorVal[G2D_RED]   = 0xff000000;
			oG2d.m_uColorVal[G2D_GREEN] = 0xff0000;
			oG2d.m_uColorVal[G2D_BLUE]  = 0xff00;
			oG2d.m_uColorVal[G2D_WHITE] = 0xffffff00;
			oG2d.m_uBytes =4;
			break;			
		default:	
			oG2d.m_uColorVal[G2D_BLACK] = 0x0;
			oG2d.m_uColorVal[G2D_RED]   = 0x1f<<11;
			oG2d.m_uColorVal[G2D_GREEN] = 0x3f<<5;
			oG2d.m_uColorVal[G2D_BLUE]  = 0x1f<<0;
			oG2d.m_uColorVal[G2D_WHITE] = 0xffff;
			oG2d.m_uBytes =4;
			break;
	}
	oG2d.m_uFbStAddr = uFbStAddr;

	oG2d.m_uColorVal[G2D_YELLOW] = (oG2d.m_uColorVal[G2D_RED] | oG2d.m_uColorVal[G2D_GREEN]);
	oG2d.m_uColorVal[G2D_CYAN] = (oG2d.m_uColorVal[G2D_GREEN] | oG2d.m_uColorVal[G2D_BLUE]);
	oG2d.m_uColorVal[G2D_MAGENTA] = (oG2d.m_uColorVal[G2D_RED] | oG2d.m_uColorVal[G2D_BLUE]);

	oG2d.m_uHoriRes = uCwMaxHSz;
	oG2d.m_uVertRes = uCwMaxVSz; //new in v2.0

	//oG2d.m_uMaxDx = uCwMaxHSz - 1;
	//oG2d.m_uMaxDy = uCwMaxVSz - 1;

	oG2d.m_uMaxDx = uCwMaxHSz;
	oG2d.m_uMaxDy = uCwMaxVSz;

	oG2d.m_uCwX1 = uX1_Cw;
	oG2d.m_uCwY1 = uY1_Cw;

	oG2d.m_uCwX2 = uX2_Cw - 1;
	oG2d.m_uCwY2 = uY2_Cw - 1;

	//oG2d.m_uCwX2 = uX2_Cw;
	//oG2d.m_uCwY2 = uY2_Cw;
	

	oG2d.m_upFontType = (u8 *)font8x15;
	oG2d.m_uFontWidth = 8;
	oG2d.m_uFontHeight = 15;

	oG2d.m_eBpp = eBpp;

//	oG2d.m_bIsBitBlt = false;
	oG2d.m_bIsBitBlt = true;


	oG2d.m_bIsScr2Scr = false;
	oG2d.m_uFontAddr = (uFbStAddr&0xff000000)+0x800000;

	G2D_DisableEffect(); // Disable per-pixel/per-plane alpha blending and fading
	G2D_SetColorKeyOff();
	G2D_InitRegs(eBpp);
}

void G2D_ClearFrameEcolor(G2D_COLOR eColor, u32 uPosX, u32 uPosY) // modification
{
	G2D_ClearFrame(oG2d.m_uColorVal[eColor],  uPosX, uPosY);
}

void G2D_ClearFrame(u32 uColor, u32 uPosX, u32 uPosY) //modification
{
	int i;

	u16 uTmpColor;

	if(oG2d.m_uBytes == 2)
		uTmpColor = uColor & 0xffff;

	for (i=uPosX+(oG2d.m_uMaxDx+1)*uPosY; i<(oG2d.m_uMaxDx+1)*(oG2d.m_uMaxDy+1); i++)
	{
		if(oG2d.m_uBytes == 4) // RGB24
			Outp32(oG2d.m_uFbStAddr+i*oG2d.m_uBytes, uColor);
		else if(oG2d.m_uBytes == 2) // RGB16
			Outp16(oG2d.m_uFbStAddr+i*oG2d.m_uBytes, uTmpColor);
		else
			Assert(0); // Unsupported RGB Format!
	}

}


void G2D_PutPixelEcolor(u32 uPosX, u32 uPosY, G2D_COLOR eColor) //modification
{
	G2D_PutPixel(uPosX, uPosY, oG2d.m_uColorVal[eColor]);
}

#if 0
void G2D_PutPixel(u32 uPosX, u32 uPosY, u32 uColor) //modification
{
	volatile unsigned i;
	volatile unsigned val=0x80000000;
//	G2D_CheckFifo(17);
	G2D_CheckFifo(24);

//	UART_Printf("0x%x\n", Inp32_0(rG2D_INT_PEND_REG));

    Outp32(rG2D_COORD0_X, uPosX);
    Outp32(rG2D_COORD0_Y, uPosY);
    Outp32(rG2D_FG_COLOR, uColor);

    Outp32(rG2D_CMDR0, G2D_REND_POINT_BIT);

//	for(i=0; i<10000; i++);

//	UART_Printf("0x%x\n", Inp32_0(rG2D_INT_PEND_REG));
	while(!((Inp32(rG2D_INT_PEND_REG)&(0x3<<9))==(0x3<<9)));	//UART_Printf("0x%x\n", Inp32_0(rG2D_INT_PEND_REG));
//	UART_Printf("0x%x\n", Inp32_0(rG2D_INT_PEND_REG));
	Outp32(rG2D_INT_PEND_REG, val|(3<<9));
	Outp32(rG2D_INT_PEND_REG, val);
	UART_Printf("0x%x\n", Inp32(rG2D_INT_PEND_REG));	
}
#elif 1
void G2D_PutPixel(u32 uPosX, u32 uPosY, u32 uColor) //modification
{
	volatile unsigned i;
	G2D_CheckFifo(17);


	Outp32(rG2D_COORD0_X, uPosX);
	Outp32(rG2D_COORD0_Y, uPosY);
	Outp32(rG2D_FG_COLOR, uColor);

	Outp32(rG2D_CMDR0, G2D_REND_POINT_BIT);
#ifdef INTERRUPT_MODE
	while(!bG2dDone) ;
	bG2dDone=0;
#endif
}
#else
void G2D_PutPixel(u32 uPosX, u32 uPosY, u32 uColor) //modification
{
	volatile unsigned i;
	G2D_CheckFifo(17);


	Outp32(rG2D_COORD0_X, uPosX);
	Outp32(rG2D_COORD0_Y, uPosY);
	Outp32(rG2D_FG_COLOR, uColor);

	Outp32(rG2D_CMDR0, G2D_REND_POINT_BIT);
}

#endif


void G2D_PutLineEcolor(u16 usPosX1, u16 usPosY1, u16 usPosX2, u16 usPosY2, G2D_COLOR eColor, u8 bIsDrawLastPoint) //modification
{
	G2D_PutLine(usPosX1, usPosY1, usPosX2, usPosY2, oG2d.m_uColorVal[eColor], bIsDrawLastPoint);
}

void G2D_PutLine(u16 usPosX1, u16 usPosY1, u16 usPosX2, u16 usPosY2, u32 uColor, u8 bIsDrawLastPoint) //modification
{
    int nMajorCoordX;
    u16 uHSz, uVSz;
    int i;
    int nIncr=0;
	u32 uCmdRegVal;

	G2D_CheckFifo(17);


    Outp32(rG2D_COORD0_X, usPosX1);
    Outp32(rG2D_COORD0_Y, usPosY1);
    Outp32(rG2D_COORD2_X, usPosX2);
    Outp32(rG2D_COORD2_Y, usPosY2);

    uVSz = ABS(usPosY1 - usPosY2);
    uHSz = ABS(usPosX1 - usPosX2);

    nMajorCoordX = (uHSz>=uVSz);

	if(nMajorCoordX)
	{
		for (i=0; i<12; i++)
		{
	    	uVSz <<= 1;
	    	nIncr <<= 1;
	    	if (uVSz >= uHSz)
	    	{
				nIncr = nIncr | 1;
				uVSz -= uHSz;
	    	}
		}
		nIncr = (nIncr + 1) >> 1;
		if (usPosY1 > usPosY2)
		{
	    	nIncr = (~nIncr) + 1; // 2's complement
		}
    }
	else
	{
		for (i=0; i<12; i++)
		{
	    	uHSz <<= 1;
	    	nIncr <<= 1;
	    	if (uHSz >= uVSz)
	    	{
				nIncr = nIncr | 1;
				uHSz -= uVSz;
	    	}
		}
		nIncr = (nIncr + 1) >> 1;
		if (usPosX1 > usPosX2)
		{
	    	nIncr = (~nIncr) + 1; // 2's complement
		}
    }

    Outp32(rG2D_FG_COLOR, uColor);

	uCmdRegVal = 0;

	if(nMajorCoordX)
	{
		Outp32(rG2D_Y_INCR, nIncr);
		uCmdRegVal =
			(bIsDrawLastPoint == true) ? (G2D_REND_LINE_BIT | G2D_MAJOR_COORD_X_BIT & G2D_DRAW_LAST_POINT_BIT) :
			(G2D_REND_LINE_BIT | G2D_MAJOR_COORD_X_BIT | G2D_NOT_DRAW_LAST_POINT_BIT);
		Outp32(rG2D_CMDR0, uCmdRegVal);
#ifdef INTERRUPT_MODE
		while(!bG2dDone);
		bG2dDone=0;
#endif

	}
	else
	{
		Outp32(rG2D_X_INCR, nIncr);
		uCmdRegVal =
			(bIsDrawLastPoint == true) ? (G2D_REND_LINE_BIT | G2D_MAJOR_COORD_Y_BIT & G2D_DRAW_LAST_POINT_BIT) :
			(G2D_REND_LINE_BIT | G2D_MAJOR_COORD_X_BIT | G2D_NOT_DRAW_LAST_POINT_BIT);
		Outp32(rG2D_CMDR0, uCmdRegVal);
#ifdef INTERRUPT_MODE
		while(!bG2dDone);
		bG2dDone=0;
#endif

	}
}

void G2D_PutRectangleEcolor(u32 uStPosX, u32 uStPosY, u32 uWidth, u32 uHeight, G2D_COLOR eColor) //modification
{
	G2D_PutRectangle(uStPosX,  uStPosY,  uWidth, uHeight, oG2d.m_uColorVal[eColor]);
}

void G2D_PutRectangle(u32 uStPosX, u32 uStPosY, u32 uWidth, u32 uHeight, u32 uColor) //modification
{

	u32 uEndPosX = uStPosX + uWidth - 1;
	u32 uEndPosY = uStPosY + uHeight - 1;

	G2D_PutLine(uStPosX,  uStPosY,  uEndPosX, uStPosY,  uColor, true);
	G2D_PutLine(uEndPosX, uStPosY,  uEndPosX, uEndPosY, uColor, true);
	G2D_PutLine(uStPosX,  uEndPosY, uEndPosX, uEndPosY, uColor, true);
	G2D_PutLine(uStPosX,  uStPosY,  uStPosX,  uEndPosY, uColor, true);
}

void G2D_FillRectangleEcolor(u32 uStPosX, u32 uStPosY, u32 uWidth , u32 uHeight, G2D_COLOR eColor) //modification
{
	G2D_FillRectangle(uStPosX,  uStPosY, uWidth,   uHeight, oG2d.m_uColorVal[eColor]);
}

void G2D_FillRectangle(u32 uStPosX, u32 uStPosY, u32 uWidth, u32 uHeight, u32 uColor) //modification
{
	int i;
	u32 uEndPosX = uStPosX + uWidth - 1;
	u32 uEndPosY = uStPosY + uHeight - 1;

	for(i=uStPosY; i<=uEndPosY; i++)
		G2D_PutLine(uStPosX, i, uEndPosX, i, uColor, true);
}

void G2D_SetRop(u32 uRopVal)
{
	G2D_CheckFifo(17);
	Outp32(rG2D_ROP, (Inp32(rG2D_ROP)&=~0xff) | uRopVal);
}

// Set Ternary Raster Operation
// Only support 7 raster operation (most used Rop)
void G2D_SetRopEtype(G2D_ROP_TYPE eRopType)
{
	u32 uRopRegVal;
	u32 uRopVal;

	G2D_CheckFifo(17);

	uRopRegVal=Inp32(rG2D_ROP);

	uRopVal =
		(eRopType == ROP_SRC_ONLY) ? G2D_ROP_SRC_ONLY :
		(eRopType == ROP_DST_ONLY) ? G2D_ROP_DST_ONLY :
		(eRopType == ROP_3RD_OPRND_ONLY) ? G2D_ROP_3RD_OPRND_ONLY :
		(eRopType == ROP_SRC_AND_DST) ? G2D_ROP_SRC_AND_DST:
		(eRopType == ROP_SRC_OR_3RD_OPRND) ? G2D_ROP_SRC_OR_3RD_OPRND :
		(eRopType == ROP_SRC_AND_3RD_OPRND) ? G2D_ROP_SRC_AND_3RD_OPRND :
		(eRopType == ROP_SRC_XOR_3RD_OPRND) ? G2D_ROP_SRC_XOR_3RD_OPRND :
		(eRopType == ROP_SRC_OR_DST) ? G2D_ROP_SRC_OR_DST : 
		(eRopType == ROP_DST_OR_3RD) ? G2D_ROP_DST_OR_3RD_OPRND : G2D_ROP_SRC_ONLY;

	uRopRegVal = (uRopRegVal&(~0xff))|uRopVal;
	Outp32(rG2D_ROP, uRopRegVal);
}


void G2D_RegisterPattern(u16* uspPattern)
{
	int i;

	G2D_CheckFifo(17);
	if (uspPattern != (u16*)0)
	{
		for(i=0; i<32; i++)
		{
			Outp32( rG2D_PATTERN_ADDR+(i*4), (uspPattern[2*i+1]<<16) | uspPattern[2*i+0]);
		}
	}
	else
		Assert(0); // There is no pattern.
}

void G2D_Set3rdOperand(G2D_OPERAND3 e3rdOp)
{

	u32 u3rdOpSel =
		(e3rdOp == G2D_OPERAND3_PAT) ? G2D_OPERAND3_PAT_BIT :
		(e3rdOp == G2D_OPERAND3_FG) ? G2D_OPERAND3_FG_BIT :	 0xffffffff;

	if (u3rdOpSel == 0xffffffff) 
		Assert(0); // UnSupported Third Operand!

	G2D_CheckFifo(17);

	Outp32(rG2D_ROP, (Inp32(rG2D_ROP) &= ~(0x1<<13)) | u3rdOpSel);
}


void G2D_SetRotationMode(u32 uRotationType)
{
	
	G2D_CheckFifo(17);
	Outp32(rG2D_ROTATE, (Inp32(rG2D_ROTATE) & ~0x3f)|(uRotationType));	
}

void G2D_SetRotationOrg(u16 usRotOrgX, u16 usRotOrgY)
{
	G2D_CheckFifo(17);

	Outp16(rG2D_ROT_OC_X, usRotOrgX);
	Outp16(rG2D_ROT_OC_Y, usRotOrgY);
}

void G2D_SetScreenSize(u32 uMaxX, u32 uMaxY)
{
	G2D_CheckFifo(17);

	Outp32(rG2D_SC_HORI_RES, (uMaxX&0x3ff));	
	Outp32(rG2D_SC_VERT_RES, (uMaxY&0x3ff));
}

void G2D_SetSourceSize(u32 uHori, u32 uVert)
{
	G2D_CheckFifo(17);

	Outp32(rG2D_SRC_HORI_RES, (uHori&0x3ff));	
	Outp32(rG2D_SRC_VERT_RES, (uVert&0x3ff));	
}

void G2D_SetClippingWindow(u32 uLeftTopX, u32 uLeftTopY, u32 uRightBottomX, u32 uRightBottomY)
{

	G2D_CheckFifo(17);

	Outp32(rG2D_CW_LT_X, uLeftTopX);	
	Outp32(rG2D_CW_LT_Y, uLeftTopY);
	Outp32(rG2D_CW_RB_X, uRightBottomX);
	Outp32(rG2D_CW_RB_Y, uRightBottomY);	
}

void G2D_SetPatternMemory(u32 uCoordX, u32 uCoordY, u16 usColor)
{

	u32 uOffsetAddress;
	u32 uData;

	G2D_CheckFifo(17);
	
	Assert(uCoordX <= 7);
	
	Assert(uCoordY <= 7);

	uOffsetAddress = uCoordY*8*2 + (uCoordX/2)*4;	
	uData=Inp32(rG2D_PATTERN_ADDR + uOffsetAddress);
	uData = (uData&~(0xffff<<(16*(uCoordX%2))))|(usColor<<(16*(uCoordX%2)));
	Outp32(rG2D_PATTERN_ADDR + uOffsetAddress, uData);	
}

void G2D_GetPatternMemory(u32 uCoordX, u32 uCoordY, u16 *usColor)
{

	u32 uOffsetAddress;
	u32 uData;

	G2D_CheckFifo(17);
	
	Assert(uCoordX <= 7);
	
	Assert(uCoordY <= 7);

	uOffsetAddress = uCoordY*8*2 + (uCoordX/2)*4;	
	uData=Inp32(rG2D_PATTERN_ADDR + uOffsetAddress);

	if((uCoordX%2)==0) *usColor= (u16)(uData&0xffff);
	else *usColor= (u16)((uData&0xffff0000)>>16);
	
}

void G2D_SetPatternOffset(u8 ucPatOffsetX, u8 ucPatOffsetY)
{

	Assert(ucPatOffsetX<=7);
	Assert(ucPatOffsetY<=7);

	G2D_CheckFifo(17);
	Outp32(rG2D_PATOFF_X, ucPatOffsetX);
	Outp32(rG2D_PATOFF_Y, ucPatOffsetY);
}


void G2D_SetCoordinateDrawingText(u32 uStartX, u32 uStartY, u32 uEndX, u32 uEndY)
{

	G2D_CheckFifo(17);

	Outp32(rG2D_COORD0_X, uStartX);
	Outp32(rG2D_COORD0_Y, uStartY);
	Outp32(rG2D_COORD1_X, uEndX);
	Outp32(rG2D_COORD1_Y, uEndY);	
}

void G2D_SetFirstText(u32 uFirstText)
{
	G2D_CheckFifo(17);

	Outp32(rG2D_CMDR4, uFirstText);
}

void G2D_SetNextText(u32 uNextText)
{
	G2D_CheckFifo(17);

	Outp32(rG2D_CMDR5, uNextText);
}

void G2D_SetCMDR7(u32 uTestDrawingFbAddr)
{
	G2D_CheckFifo(17);

	Outp32(rG2D_CMDR7, uTestDrawingFbAddr);
}

void G2D_SetFontType(u32 uFontType, u32 *uFontWidth, u32 *uFontHeight)
{

	switch(uFontType)
	{
		case 0:
			oG2d.m_upFontType = (u8 *)chr_font8X8;
			oG2d.m_uFontWidth = 8;
			oG2d.m_uFontHeight = 8;
			*uFontWidth=8;
			*uFontHeight=8;
			break;
		case 1:
			oG2d.m_upFontType = (u8 *)font8x15;
			oG2d.m_uFontWidth = 8;
			oG2d.m_uFontHeight = 15;
			*uFontWidth=8;
			*uFontHeight=15;
			break;		
		case 2:
			oG2d.m_upFontType = (u8 *)chr_font8X16T;
			oG2d.m_uFontWidth = 8;
			oG2d.m_uFontHeight = 15;
			*uFontWidth=8;
			*uFontHeight=15;			
			break;
		case 3:
			oG2d.m_upFontType = (u8 *)chr_font8X16;
			oG2d.m_uFontWidth = 8;
			oG2d.m_uFontHeight = 16;
			*uFontWidth=8;
			*uFontHeight=16;			
			break;	
		default:
			oG2d.m_upFontType = (u8 *)chr_font8X8;
			oG2d.m_uFontWidth = 8;
			oG2d.m_uFontHeight = 8;
			*uFontWidth=8;
			*uFontHeight=8;			
			break;
	}
}

void G2D_FillMemoryForTextDrawing(u32 uFontType, u32 uTextDrawingFbAddr)
{
	u8* ucOneLetter;	
	u32 uHostData = 0x0;
	u32 uTempData = 0x0;
	u32 i,j,k;
	
	u32 uTotalSizeOfFont;
	u32 uAddressOffset;

	switch(uFontType) {
		case 0:
			oG2d.m_upFontType = (u8 *)chr_font8X8;
			oG2d.m_uFontWidth = 8;
			oG2d.m_uFontHeight = 8;
			uTotalSizeOfFont=sizeof(chr_font8X8);
			break;
		case 1:
			oG2d.m_upFontType = (u8 *)font8x15;
			oG2d.m_uFontWidth = 8;
			oG2d.m_uFontHeight = 15;
			uTotalSizeOfFont=sizeof(font8x15);
			break;
		case 2:
			oG2d.m_upFontType = (u8 *)chr_font8X16T;
			oG2d.m_uFontWidth = 8;
			oG2d.m_uFontHeight = 15;
			uTotalSizeOfFont=sizeof(chr_font8X16T);		
			break;
		case 3:
			oG2d.m_upFontType = (u8 *)chr_font8X16;
			oG2d.m_uFontWidth = 8;
			oG2d.m_uFontHeight = 16;		
			uTotalSizeOfFont=sizeof(chr_font8X16);
			break;	
		default:
			oG2d.m_upFontType = (u8 *)chr_font8X8;
			oG2d.m_uFontWidth = 8;
			oG2d.m_uFontHeight = 8;
			uTotalSizeOfFont=sizeof(chr_font8X8);	
			break;			
	}

//	*uFontSize = oG2d.m_uFontHeight;
/*
	for(i=0; i< uTotalSizeOfFont/4; i++) {
		ucOneLetter = oG2d.m_upFontType + i*4; // get start address of chosen letter in font8x15
		for (j=0; j<4; j++) // Generate 1-word of data!!
		{
			uTempData = (u32)ucOneLetter[j];
			uHostData |= (uTempData << (24 - 8*j));
		}
		*(unsigned int *)(uTextDrawingFbAddr+i*4)=uHostData;
		uHostData=0;
	}	
*/	
	// oG2d.m_ufontWidth might be multiple of 8.
	uAddressOffset=((oG2d.m_uFontWidth/8*oG2d.m_uFontHeight+3)/4)*4;
	for(i=0; i< (uTotalSizeOfFont)/(oG2d.m_uFontWidth/8*oG2d.m_uFontHeight); i++) {
		ucOneLetter = oG2d.m_upFontType + i*oG2d.m_uFontWidth/8*oG2d.m_uFontHeight; 
		for (j=0; j<(oG2d.m_uFontWidth/8*oG2d.m_uFontHeight+3)/4; j++) // Generate 1-word of data!!
		{
			for (k=0; k<4; k++) // Generate 1-word of data!!
			{
				uTempData = (u32)ucOneLetter[j*4+k];
				uHostData |= (uTempData << (24 - 8*k));
			}
			*(unsigned int *)(uTextDrawingFbAddr+i*uAddressOffset+j*4)=uHostData;
			uHostData=0;
		}

	}

}

void G2D_SetCoordinateSrcBlock(u32 uStartX, u32 uStartY, u32 uEndX, u32 uEndY)
{

	G2D_CheckFifo(17);

	Outp32(rG2D_COORD0_X, uStartX);
	Outp32(rG2D_COORD0_Y, uStartY);
	Outp32(rG2D_COORD1_X, uEndX);
	Outp32(rG2D_COORD1_Y, uEndY);	
}

void G2D_SetCoordinateDstBlock(u32 uStartX, u32 uStartY, u32 uEndX, u32 uEndY)
{

	G2D_CheckFifo(17);

	Outp32(rG2D_COORD2_X, uStartX);
	Outp32(rG2D_COORD2_Y, uStartY);
	Outp32(rG2D_COORD3_X, uEndX);
	Outp32(rG2D_COORD3_Y, uEndY);	
}

void G2D_SetFirstBitBLTData(u32 uFirstData)
{
	G2D_CheckFifo(17);

	Outp32(rG2D_CMDR2, uFirstData);
}

void G2D_SetNextBitBLTData(u32 uNextData)
{
	G2D_CheckFifo(17);

	Outp32(rG2D_CMDR3, uNextData);
}

void G2D_SetFirstBitBLTEData(G2D_COLOR eFirstData)
{

	G2D_SetFirstBitBLTData(oG2d.m_uColorVal[eFirstData]);
}

void G2D_SetNextBitBLTEData(G2D_COLOR eNextData)
{
	G2D_SetNextBitBLTData(oG2d.m_uColorVal[eNextData]);
}

void G2D_SetStencilKey(u32 uIsInvOn, u32 uIsAOn, u32 uIsROn, u32 uIsGOn, u32 uIsBOn)
{	
	Outp32(rG2D_COLORKEY_CNTL, ((uIsInvOn&1)<<4)|((uIsROn&1)<<3)|((uIsGOn&1)<<2)|((uIsBOn&1)<<1)|(uIsAOn&1));
}

void G2D_SetStencilMinMax(u32 uAMin, u32 uAMax, u32 uRMin, u32 uRMax, u32 uGMin, u32 uGMax, u32 uBMin, u32 uBMax, CSPACE eBpp)
{

	switch(eBpp)
	{
		case RGB16:
			Outp32(rG2D_COLORKEY_DR_MIN, ((uRMin<<3)<<16)|((uGMin<<2)<<8)|(uBMin<<3));
			Outp32(rG2D_COLORKEY_DR_MAX, ((uRMax<<3)<<16)|((uGMax<<2)<<8)|(uBMax<<3));
			break;
		case RGBA16:
			Outp32(rG2D_COLORKEY_DR_MIN, ((uAMin&0xff)<<24)|((uRMin&0xff)<<16)|((uGMin&0xff)<<8)|(uBMin&0xff));
			Outp32(rG2D_COLORKEY_DR_MAX, ((uAMax&0xff)<<24)|((uRMax&0xff)<<16)|((uGMax&0xff)<<8)|(uBMax&0xff));
			break;
		case ARGB16:
			Outp32(rG2D_COLORKEY_DR_MIN, ((uAMin&0xff)<<24)|((uRMin&0xff)<<16)|((uGMin&0xff)<<8)|(uBMin&0xff));
			Outp32(rG2D_COLORKEY_DR_MAX, ((uAMax&0xff)<<24)|((uRMax&0xff)<<16)|((uGMax&0xff)<<8)|(uBMax&0xff));
			break;
		case RGB24:
		case RGBX24:		
			Outp32(rG2D_COLORKEY_DR_MIN, ((uRMin&0xff)<<16)|((uGMin&0xff)<<8)|(uBMin&0xff));
			Outp32(rG2D_COLORKEY_DR_MAX, ((uRMax&0xff)<<16)|((uGMax&0xff)<<8)|(uBMax&0xff));
			break;
		case RGBA24:
		case ARGB24:
			Outp32(rG2D_COLORKEY_DR_MIN, ((uAMin&0xff)<<24)|((uRMin&0xff)<<16)|((uGMin&0xff)<<8)|(uBMin&0xff));
			Outp32(rG2D_COLORKEY_DR_MAX, ((uAMax&0xff)<<24)|((uRMax&0xff)<<16)|((uGMax&0xff)<<8)|(uBMax&0xff));
			break;
		default:
			Assert(0);
			break;									
	}

}

void G2D_GetFrmSz(u32 *uG2dWidth, u32 *uG2dHeight)
{
	u32 uMaxScreenVal;

	uMaxScreenVal=Inp32(rG2D_SC_RES);
	//*uG2dWidth = (uMaxScreenVal&0x7ff)+1;
	//*uG2dHeight = ((uMaxScreenVal>>16)&0x7ff)+1;

	*uG2dWidth = (uMaxScreenVal&0x7ff);
	*uG2dHeight = ((uMaxScreenVal>>16)&0x7ff);
}

void G2D_GetFbAddr(u32 *uG2dFbAddr)
{
	//*uG2dFbAddr= Inp32_0(rG2D_FB_BA);
	//G2dFbAddr = oG2d.m_uFbStAddr;
	*uG2dFbAddr= Inp32(rG2D_DEST_BASE_ADDR);	
}
/*
void G2D_GetBppMode(CSPACE *eBpp)
{
	u32 uBppVal;

	uBppVal=Inp32_0(rG2D_COLOR_MODE);
	switch(uBppVal&0xf) {
		case 1:
			*eBpp=ARGB8; //15-bpp
			break;
		case 2:
			*eBpp=RGB16; //16-bpp
			break;
		case 4:
			*eBpp=RGB18; // 18-bpp
			break;
		case 8:
			*eBpp=RGB24; // 24-bpp
			break; 
		default:
			*eBpp=RGB16; //16-bpp
			break;
	}		
}
*/
void G2D_GetSrcBppMode(CSPACE *eBpp)
{
	u32 uBppVal;

	uBppVal=Inp32(rG2D_SRC_COLOR_MODE);
	if (!(uBppVal&G2D_COLOR_YUV_MODE)) //means RGB mode
	{
		switch(uBppVal&0x7) {
			case 0:
				*eBpp=RGB16; //16-bpp
				break;
			case 1:
				*eBpp=RGBA16; //16-bpp
				break;
			case 2:
				*eBpp=ARGB16; //16-bpp
				break;
			case 3:
				*eBpp=RGBA24; // 24-bpp
				break;
			case 4:
				*eBpp=ARGB24; // 24-bpp
				break; 
			case 5:
				*eBpp=RGB24; // 24-bpp
				break; 
			case 6:
				*eBpp=RGBX24; // 24-bpp
				break; 
			default:
				*eBpp=RGB24; //24-bpp
				break;
		}	
	}
}

void G2D_GetDestBppMode(CSPACE *eBpp)
{
	u32 uBppVal;

	uBppVal=Inp32(rG2D_DEST_COLOR_MODE);
	switch(uBppVal&0x7) {
		case 0:
			*eBpp=RGB16; //16-bpp
			break;
		case 1:
			*eBpp=RGBA16; //16-bpp
			break;
		case 2:
			*eBpp=ARGB16; //16-bpp
			break;
		case 3:
			*eBpp=RGBA24; // 24-bpp
			break;
		case 4:
			*eBpp=ARGB24; // 24-bpp
			break; 
		case 5:
			*eBpp=RGB24; // 24-bpp
			break; 
		case 6:
			*eBpp=RGBX24; // 24-bpp
			break; 
		default:
			*eBpp=RGB24; //24-bpp
			break;
	}			
}

void G2D_SetColorExpansionMethod(u8 bIsScr2Scr)
{
	oG2d.m_bIsScr2Scr  = (bIsScr2Scr) ? 1 :	0;
}


void G2D_SetFontAddr(u32 uFontAddr)
{
	oG2d.m_uFontAddr= uFontAddr;
}

void G2D_InterruptEnable(void)
{
	//Outp32(rG2D_INTEN, 0x1<<9);//Enable Interrupt
	Outp32(rG2D_INTEN, 0x1<<10);//Enable Interrupt
	Outp32(rG2D_INTC_PEND, 0x80000000);
	bG2dDone=0;
}

void G2D_InterruptDisable(void)
{
	//Outp32(rG2D_INTEN, (Inp32_0(rG2D_INTEN)&~(1<<9)));//Disable Interrupt
	Outp32(rG2D_INTEN, (Inp32(rG2D_INTEN)&~(1<<10)));//Disable Interrupt
}

void G2D_IntEnSelectable(u32 uWhichInt)
{
	Outp32(rG2D_INTEN, uWhichInt);//Enable Interrupt
	Outp32(rG2D_INTC_PEND, 0x80000000);
	bG2dDone=0;
}

void G2D_IntDisSelectable(u32 uWhichInt)
{

	Outp32(rG2D_INTEN, (Inp32(rG2D_INTEN)&~(uWhichInt)));//Disable Interrupt
}

void G2D_WaitForInterrupt(void)
{
	while(!bG2dDone);
	bG2dDone=0;
}		

void __irq Isr_G2d(void)
//void Isr_G2d(void)
{
	bG2dDone=1;	
	
	//Outp32(rG2D_INT_PEND_REG, ((1<<31)|(1<<9)));
	Outp32(rG2D_INTC_PEND, ((1<<31)|(1<<10)));
	Outp32(rG2D_INTC_PEND, (u32)(1<<31)); // Victor gave us a guidance such this line to clear pending.

//	UART_Printf("done\n");
	INTC_ClearVectAddr();
}
void __irq Isr_G2dProfile(void)
//void Isr_G2dProfile(void)
{
#ifdef PROFILE
	volatile float uSpendTime; //msec
	PWM_ReadTime(&uSpendTime);
	//UART_Printf("%6f %10.6f\n", (float)(uSpendTime/1000000.), (float)(1/(uSpendTime/1000000.)));
	UART_Printf("%10.6f\n", uSpendTime);
#endif
	bG2dDone=1;
	Outp32(rG2D_INTC_PEND, ((1<<31)|(1<<9)));
	Outp32(rG2D_INTC_PEND, (u32)(1<<31)); // Victor gave us a guidance such this line to clear pending.
//	UART_Printf("done\n");
	INTC_ClearVectAddr();
}

void __irq Isr_G2dDe(void)
{
	bG2dDone=1;	
	
	Outp32(rG2D_INTC_PEND, ((1<<31)|(7<<8)|(1<<0)));
	Outp32(rG2D_INTC_PEND, (u32)(1<<31)); // Victor gave us a guidance such this line to clear pending.

	UART_Printf("Int\n");
	INTC_ClearVectAddr();
	
}

void G2D_SetEndian(u8 bEndian)
{
	Outp32(rG2D_ENDIAN, (Inp32(rG2D_ENDIAN) & ~(1<<4))|(bEndian<<4));
}



void G2D_DrawImage(CSPACE eBpp)
{
	u32 i,j,k;
	if(eBpp==RGB16) {
		k=0;
		for(j=0; j<320; j++)
			for(i=0; i<240; i++)
// boaz.kim 070903
//				G2D_PutPixel(i, j, sun320240_16bpp[k++]);
				G2D_PutPixel(i, j, sun240320_16bpp[k++]);

	}
	else if(eBpp==RGB24) {
		k=0;
		for(j=0; j<320; j++)
			for(i=0; i<240; i++)
// boaz.kim 070903				
//				G2D_PutPixel(i, j, sun320240_24bpp[k++]);
				G2D_PutPixel(i, j, sun240320_24bpp[k++]);

	} else;


}

void G2D_RopOperation(u8 bSrcData, u8 bDstData, u8 bThirdData, u8 ucRopVal, u8 *bOut)
{
	if((bSrcData==0) && (bDstData==0) && (bThirdData ==0)) *bOut=(u8)((ucRopVal>>0)&0x1);
	else if((bSrcData==0 )&& (bDstData==0) && (bThirdData ==1)) *bOut=(u8)((ucRopVal>>1)&0x1);
	else if((bSrcData==0) && (bDstData==1) && (bThirdData ==0)) *bOut=(u8)((ucRopVal>>2)&0x1);
	else if((bSrcData==0) && (bDstData==1) && (bThirdData ==1)) *bOut=(u8)((ucRopVal>>3)&0x1);
	else if((bSrcData==1) && (bDstData==0) && (bThirdData ==0)) *bOut=(u8)((ucRopVal>>4)&0x1);
	else if((bSrcData==1) && (bDstData==0) && (bThirdData ==1)) *bOut=(u8)((ucRopVal>>5)&0x1);
	else if((bSrcData==1) && (bDstData==1) && (bThirdData ==0)) *bOut=(u8)((ucRopVal>>6)&0x1);
	else if((bSrcData==1) && (bDstData==1) && (bThirdData ==1)) *bOut=(u8)((ucRopVal>>7)&0x1);
	else Assert(0);
}

void G2D_RopOut(u32 uSrcData, u32 uDstData, u32 uThirdData, u8 ucRopVal, CSPACE eBpp, u32 *uRopOut)
{

	u32 i;
	u32 uDataMask;
	u32 uDataSize;
	u8 bSrcData, bDstData, bThirdData, bOutData;
	
	if(eBpp==RGB16) {
		uDataMask=0xffff;	
		uDataSize=16;
	}
	else if(eBpp==RGB18) {
		uDataMask=0x3ffff;
		uDataSize=18;		
	}
	else if(eBpp==RGB24) {
		uDataMask=0xffffff;
		uDataSize=24;
	}
	else Assert(0);

	*uRopOut = 0;
	for(i=0; i<uDataSize; i++) {
		bSrcData=(u8)(((uSrcData&uDataMask)>>i)&1);
		bDstData=(u8)(((uDstData&uDataMask)>>i)&1);
		bThirdData=(u8)(((uThirdData&uDataMask)>>i)&1);
		G2D_RopOperation(bSrcData, bDstData, bThirdData, ucRopVal, &bOutData);
		*uRopOut |= (bOutData << i);
	}
}

void G2D_ConvertEcolorToRaw(G2D_COLOR eColor, u32 *uRawColor) 
{
	*uRawColor=oG2d.m_uColorVal[eColor];
}

void G2D_GetFbData(u32 uX, u32 uY, u32 uG2dWidth, u32 uG2dFbAddr, CSPACE eBpp, u32 *uFbData)
{

	switch(eBpp)
	{
		case RGB16:
		case RGBA16:
		case ARGB16:
			*uFbData=Inp16(uG2dFbAddr+(uY*uG2dWidth+uX)*2);		
			break;
		case ARGB24:
		case RGBA24:
		case RGB24:
		case RGBX24:
			*uFbData=Inp32(uG2dFbAddr+(uY*uG2dWidth+uX)*4);		
			break;
		default:
			*uFbData=Inp32(uG2dFbAddr+(uY*uG2dWidth+uX)*4);		
			break;
	}

}

void G2D_SetFbData(u32 uX, u32 uY, u32 uG2dWidth, u32 uG2dFbAddr, CSPACE eBpp, u32 uFbData)
	 
{

	switch(eBpp)
	{
		case RGB16:
		case RGBA16:
		case ARGB16:
			Outp16(uG2dFbAddr+(uY*uG2dWidth+uX)*2, uFbData);
			break;
		case ARGB24:
		case RGBA24:
		case RGB24:
		case RGBX24:
			Outp32(uG2dFbAddr+(uY*uG2dWidth+uX)*4, uFbData);
			break;
		default:
			Outp32(uG2dFbAddr+(uY*uG2dWidth+uX)*4, uFbData);
			break;
	}

}


void G2D_GetPatternOffset(u8 *ucPatOffsetX, u8 *ucPatOffsetY)
{
	*ucPatOffsetX=Inp32(rG2D_PATOFF_X);
	*ucPatOffsetY=Inp32(rG2D_PATOFF_Y);
}

// It needs to be modified according to V2.0.
void G2D_ConvertPatternToRaw(u16 uThirdDataOrg, CSPACE eBpp, u32 *uThirdDataConverted)
{
	u32 uRed, uGreen, uBlue;
	switch(eBpp) {
		case RGB16:
			*uThirdDataConverted=uThirdDataOrg;
			break;
		case RGB18:
			uRed=(((uThirdDataOrg>>11)&0x1f)<<(12+1));
			uGreen=(((uThirdDataOrg>>5)&0x3f)<<6);
			uBlue=((uThirdDataOrg&0x1f)<<(0+1));
			*uThirdDataConverted=(uRed|uGreen|uBlue);
			if(*uThirdDataConverted&0x02000) *uThirdDataConverted|=0x01000;
			if(*uThirdDataConverted&0x00040) *uThirdDataConverted=*uThirdDataConverted;
			if(*uThirdDataConverted&0x00002) *uThirdDataConverted|=0x1;			
			break;
		case RGB24:
			uRed=(((uThirdDataOrg>>11)&0x1f)<<(16+3));
			uGreen=(((uThirdDataOrg>>5)&0x3f)<<(8+2));
			uBlue=((uThirdDataOrg&0x1f)<<(0+3));
			*uThirdDataConverted=(uRed|uGreen|uBlue);
			if(*uThirdDataConverted&0x00080000) *uThirdDataConverted|=0x00070000;
			if(*uThirdDataConverted&0x00000400) *uThirdDataConverted|=0x00000300;
			if(*uThirdDataConverted&0x00000008) *uThirdDataConverted|=0x00000007;			
			break;
		default:
			*uThirdDataConverted=uThirdDataOrg;
			break;

	}

}

#if 0
void G2D_BlendingOut(u32 uSrcData, u32 uDstData, u8 ucAlphaVal, u8 bFading, u8 ucFadingOffset, u32 *uBlendingOut)
{

	u32 uSrcRed, uSrcGreen, uSrcBlue;
	u32 uDstRed, uDstGreen, uDstBlue;
	u32 uBldRed, uBldGreen, uBldBlue;	
	
	uSrcRed= (uSrcData & 0x00ff0000)>>16;  // Mask R
	uSrcGreen = (uSrcData & 0x0000ff00)>>8;	 // Mask G
	uSrcBlue = uSrcData & 0x000000ff;		 // Mask B

	uDstRed = (uDstData & 0x00ff0000)>>16; // Mask R
	uDstGreen = (uDstData & 0x0000ff00)>>8;  // Mask G
	uDstBlue = uDstData & 0x000000ff;		 // Mask B

	if(bFading) {
		uBldRed= ((uSrcRed*(ucAlphaVal+1))>>8) + ucFadingOffset; // R output
		uBldGreen= ((uSrcGreen*(ucAlphaVal+1))>>8) + ucFadingOffset; // G output
		uBldBlue= ((uSrcBlue*(ucAlphaVal+1)>>8)) + ucFadingOffset; // B output
		if(uBldRed>=256) uBldRed=255;
		if(uBldGreen>=256) uBldGreen=255;
		if(uBldBlue>=256) uBldBlue=255;
	}
	else {
		uBldRed= ((uSrcRed*(ucAlphaVal+1)) + (uDstRed*(256-ucAlphaVal)))>>8; // R output
		uBldGreen= ((uSrcGreen*(ucAlphaVal+1)) + (uDstGreen*(256-ucAlphaVal)))>>8; // G output
		uBldBlue= ((uSrcBlue*(ucAlphaVal+1)) + (uDstBlue*(256-ucAlphaVal)))>>8; // B output
	}

	*uBlendingOut = (uBldRed<<16) | (uBldGreen<<8) | uBldBlue;
}
#else
void G2D_BlendingOut(u32 uSrcData, u32 uDstData, u8 ucAlphaVal, u8 bFading, u8 ucFadingOffset, u32 *uBlendingOut)
{

	u32 uSrcRed, uSrcGreen, uSrcBlue;
	u32 uDstRed, uDstGreen, uDstBlue;
	u32 uBldRed, uBldGreen, uBldBlue;	
	
	uSrcRed= (uSrcData & 0x00ff0000)>>16;  // Mask R
	uSrcGreen = (uSrcData & 0x0000ff00)>>8;	 // Mask G
	uSrcBlue = uSrcData & 0x000000ff;		 // Mask B

	uDstRed = (uDstData & 0x00ff0000)>>16; // Mask R
	uDstGreen = (uDstData & 0x0000ff00)>>8;  // Mask G
	uDstBlue = uDstData & 0x000000ff;		 // Mask B

	if(bFading) {
		uBldRed= ((uSrcRed*(ucAlphaVal+1))>>8) + ucFadingOffset; // R output
		uBldGreen= ((uSrcGreen*(ucAlphaVal+1))>>8) + ucFadingOffset; // G output
		uBldBlue= ((uSrcBlue*(ucAlphaVal+1)>>8)) + ucFadingOffset; // B output
		if(uBldRed>=256) uBldRed=255;
		if(uBldGreen>=256) uBldGreen=255;
		if(uBldBlue>=256) uBldBlue=255;
	}
	else {
		uBldRed= ((uSrcRed*(ucAlphaVal+1)) + (uDstRed*(255-ucAlphaVal)))>>8; // R output
		uBldGreen= ((uSrcGreen*(ucAlphaVal+1)) + (uDstGreen*(255-ucAlphaVal)))>>8; // G output
		uBldBlue= ((uSrcBlue*(ucAlphaVal+1)) + (uDstBlue*(255-ucAlphaVal)))>>8; // B output
	}

	*uBlendingOut = (uBldRed<<16) | (uBldGreen<<8) | uBldBlue;
}

#endif


void G2d_Convert24bpp(u32 uSrcData, CSPACE eBpp, u8 bSwap, u32 *uConvertedData)
{

	u32 uRed, uGreen, uBlue;

#if 0	
	switch(eBpp) {
		case RGB16 : // 16 bit color mode
			if(bSwap == 1) {
				uRed = uSrcData & 0x0000f800;
				uGreen = uSrcData & 0x000007e0;
				uBlue = uSrcData & 0x0000001f;

				*uConvertedData = uRed<<8 | uGreen<<5 | uBlue<<3;
			}
			else {
				uRed = uSrcData & 0xf8000000;
				uGreen = uSrcData & 0x07e00000;
				uBlue = uSrcData & 0x001f0000;

				*uConvertedData = uRed>>8 | uGreen>>11 | uBlue>>13;
			}
			break;	
		case  RGBA16: // 15 bit color mode
			uRed = uSrcData & 0x0000f800;  // R
			uGreen = uSrcData & 0x000007c0;  // G
			uBlue = uSrcData & 0x0000003e;  // B
			*uConvertedData = uRed<<8 | uGreen<<5 | uBlue<<2; // SUM
			break;
		case  ARGB16: // 15 bit color mode
			uRed = uSrcData & 0x00007c00;  // R
			uGreen = uSrcData & 0x000003e0;  // G
			uBlue = uSrcData & 0x0000001f;  // B
			*uConvertedData = uRed<<9 | uGreen<<6 | uBlue<<3; // SUM
			break;
		case RGBA24 :
		case ARGB24 :
		case RGB24  :
		case RGBX24 :
			*uConvertedData = uSrcData;
			break;
		default:
			*uConvertedData = uSrcData;
			break;				
	}
#else
	switch(eBpp) {
		case RGB16 : // 16 bit color mode
			uRed = (uSrcData & 0x0000f800)>>8;
			uGreen = (uSrcData & 0x000007e0)>>3;
			uBlue = (uSrcData & 0x0000001f)<<3;
			if(uRed & 0x80) uRed |= 0x7;
			if(uGreen & 0x80) uGreen |= 0x3;
			if(uBlue & 0x80) uBlue |= 0x7;
			*uConvertedData = uRed<<16 | uGreen<<8 | uBlue;
			break;	
		case  RGBA16: // 15 bit color mode
			uRed = uSrcData & 0x0000f800;  // R
			uGreen = uSrcData & 0x000007c0;  // G
			uBlue = uSrcData & 0x0000003e;  // B
			*uConvertedData = uRed<<8 | uGreen<<5 | uBlue<<2; // SUM
			break;
		case  ARGB16: // 15 bit color mode
			uRed = uSrcData & 0x00007c00;  // R
			uGreen = uSrcData & 0x000003e0;  // G
			uBlue = uSrcData & 0x0000001f;  // B
			*uConvertedData = uRed<<9 | uGreen<<6 | uBlue<<3; // SUM
			break;
		case RGBA24 :
		case ARGB24 :
		case RGB24  :
		case RGBX24 :
			*uConvertedData = uSrcData;
			break;
		default:
			*uConvertedData = uSrcData;
			break;				
	}
#endif	 
} 


void G2D_GetRotateCoordinate(u32 uDstX, u32 uDstY, u32 uOrgX, u32 uOrgY, u32 uRType, u32 *uRsltX, u32 *uRsltY) 
{

	switch(uRType) {
		case  1 : // No Rotate. bypass.
			*uRsltX = uDstX;
			*uRsltY = uDstY;
			break;
		case  2 : // 90 degree Rotation
			*uRsltX = uOrgX + uOrgY - uDstY;
			*uRsltY = uDstX - uOrgX + uOrgY;			
			break;
		case  4 : // 180 degree Rotation
			*uRsltX = 2*uOrgX - uDstX;
			*uRsltY = 2*uOrgY - uDstY;
			break;
		case  8 : // 270 degree Rotation
			*uRsltX = uDstY + uOrgX - uOrgY;
			*uRsltY = uOrgX + uOrgY - uDstX;
			break;
		case 16 : // X-flip
			*uRsltX = uDstX;
			*uRsltY = 2*uOrgY - uDstY;
			break;
		case 32 : // Y-flip
			*uRsltX = 2*uOrgX - uDstX;
			*uRsltY = uDstY;		
			break;
		default :
			Assert(0);
			break;
	}
}


void G2D_SetFifoIntLevel(u8 ucFifoIntLevel)
{
	G2D_CheckFifo(17);
	
	Outp8(rG2D_FIFO_INTC, ucFifoIntLevel);
	
}



// Interrupt TEST!!!!
void G2D_PutPixelForInterrupt(u32 uPosX, u32 uPosY, u32 uColor)
{
	volatile unsigned i;
	G2D_CheckFifo(17);


	Outp32(rG2D_COORD0_X, uPosX);
	Outp32(rG2D_COORD0_Y, uPosY);
	Outp32(rG2D_FG_COLOR, uColor);

	Outp32(rG2D_CMDR0, G2D_REND_POINT_BIT);

	while(!bG2dDone);
	bG2dDone=0;
//	UART_Printf("rG2D_INT_PEND_REG:0x%x\n", rG2D_INT_PEND_REG);
}

void G2D_PutPixelForInterruptHard(u32 uG2dWidth, u32 uG2dHeight, u32 uColor)
{
	u32 i;
	
	Outp32(rG2D_COORD0_X, 0);
	Outp32(rG2D_COORD0_Y, 0);
	Outp32(rG2D_FG_COLOR, uColor);

	for(i=0; i<uG2dWidth*uG2dHeight; i++)
		Outp32(rG2D_CMDR0, G2D_REND_POINT_BIT);
}

/*
void G2D_IntEnableForDeEngineFinish(void)
{
	uIntCnt=0; bG2dDone=0;
	Outp32(rG2D_INTEN, (Inp32_0(rG2D_INTEN)&~(0x7<<8))|(1<<10));
	Outp32(rG2D_INT_PEND_REG, (0x80000000|7<<8));
	Outp32(rG2D_INT_PEND_REG, 0x80000000);
}
void G2D_IntEnableForCmdFinish(void)
{
	uIntCnt=0; bG2dDone=0;
	Outp32(rG2D_INTEN, (Inp32_0(rG2D_INTEN)&~(0x7<<8))|(1<<9));
	Outp32(rG2D_INT_PEND_REG, 0x80000000);
}
void G2D_IntEnableForOverflow(u8 bFifo, u8 ucFifoLevel)
{
	uIntCnt=0;
	if(bFifo) {
//		Outp32(rG2D_INTEN, (Inp32_0(rG2D_INTEN)&~(0x7<<8))|(1<<8)|1);
		Outp32(rG2D_INTEN, (Inp32_0(rG2D_INTEN)&~(0x7<<8))|1);
		Outp32(rG2D_FIFO_INT_CON, ucFifoLevel);
	}	
	else
		Outp32(rG2D_INTEN, (Inp32_0(rG2D_INTEN)&~(0x7<<8))|(1<<8));
	Outp32(rG2D_INT_PEND_REG, 0x80000000);
}
void G2D_InterruptDisAll(void)
{
	UART_Printf("The total number of interrupts occured:[Expected:%d], [Real:%d]\n",  (oG2d.m_uMaxDx+1)*(oG2d.m_uMaxDy+1),uIntCnt);
	uIntCnt=0;
	Outp32(rG2D_INTEN, Inp32_0(rG2D_INTEN)&~((3<<8)|1));
	
}
*/

//void __irq Isr_G2dInt(void)
void Isr_G2dInt(void)
{
	volatile unsigned uPendVal;

	bG2dDone=1;
	uIntCnt++;
	uPendVal=Inp32(rG2D_INTC_PEND);
	switch((uPendVal>>8)&0x7) {
		case 1:
			Outp32(rG2D_INTC_PEND, ((1<<31)|(1<<8)));
			break;
		case 2:
			Outp32(rG2D_INTC_PEND, ((1<<31)|(1<<9)));
			break;
		case 4:
			Outp32(rG2D_INTC_PEND, ((1<<31)|(1<<10)));
			break;
		default:
//			Outp32(rG2D_INT_PEND_REG, ((1<<31)|(0x7<<8)));
			if(uPendVal&1) 	Outp32(rG2D_INTC_PEND, ((1<<31)|(1<<0)));
			else	UART_Printf("No Pending Bit!\n");
			break;
	}
	
	Outp32(rG2D_INTC_PEND, (u32)(1<<31)); // Victor gave us a guidance such this line to clear pending.
	INTC_ClearVectAddr();
}

