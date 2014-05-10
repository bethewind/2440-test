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
*	File Name : g2d_test.c
*  
*	File Description : This file includes the test code for 2D.
*
*	Author : Sekwang Kim
*	Dept. : AP Development Team
*	Created Date : 2007/01/25
*	Version : 0.1a 
* 
*	History
*	- Created by JiHyun Jang , YunSoo Lee 
* 	- Modified by Sekwang Kim(2007/01/25)
*  
**************************************************************************************/


#include "system.h"

#include "lcd.h"
#include "glib.h"
#include "g2d.h"
#include "def.h"
#include "library.h"
#include "intc.h"
#include "timer.h"

#include "register_addr.h"




#define C16_BLUE            (0x1f<<0)
#define C16_GREEN           (0x3f<<5)
#define C16_RED             (0x1f<<11)
#define C16_BLACK           (0x0)
#define C16_WHITE           (0xffff)
#define C16_LIGHT_GRAY      (0xc618)

#define RGB16_IMG_SIZE      240*120*2
#define PATTERN_MEM_ST      (CODEC_MEM_ST+0x80000)


u16 pat8x8[]={
	C16_WHITE,C16_WHITE,C16_WHITE,C16_RED  ,C16_BLUE ,C16_WHITE,C16_WHITE,C16_WHITE,
	C16_WHITE,C16_WHITE,C16_RED  ,C16_RED  ,C16_BLUE ,C16_BLUE ,C16_WHITE,C16_WHITE,
	C16_WHITE,C16_RED  ,C16_RED  ,C16_RED  ,C16_BLUE ,C16_BLUE ,C16_BLUE ,C16_WHITE,
	C16_RED  ,C16_RED  ,C16_RED  ,C16_RED  ,C16_BLUE ,C16_BLUE ,C16_BLUE ,C16_BLUE ,
	C16_GREEN,C16_GREEN,C16_GREEN,C16_GREEN,C16_BLACK,C16_BLACK,C16_BLACK,C16_BLACK,
	C16_WHITE,C16_GREEN,C16_GREEN,C16_GREEN,C16_BLACK,C16_BLACK,C16_BLACK,C16_WHITE,
	C16_WHITE,C16_WHITE,C16_GREEN,C16_GREEN,C16_BLACK,C16_BLACK,C16_WHITE,C16_WHITE,
	C16_WHITE,C16_WHITE,C16_WHITE,C16_GREEN,C16_BLACK,C16_WHITE,C16_WHITE,C16_WHITE};


u16 pat8x8_rectangle[]={
	C16_RED,C16_RED,C16_RED,C16_RED  ,C16_RED ,C16_RED,C16_RED,C16_RED,
	C16_RED,C16_RED,C16_RED,C16_RED  ,C16_RED ,C16_RED,C16_RED,C16_RED,
	C16_RED,C16_RED,C16_GREEN,C16_GREEN,C16_GREEN,C16_GREEN,C16_RED,C16_RED,
	C16_RED,C16_RED,C16_GREEN,C16_GREEN,C16_GREEN,C16_GREEN,C16_RED,C16_RED,
	C16_RED,C16_RED,C16_GREEN,C16_GREEN,C16_GREEN,C16_GREEN,C16_RED,C16_RED,
	C16_RED,C16_RED,C16_GREEN,C16_GREEN,C16_GREEN,C16_GREEN,C16_RED,C16_RED,
	C16_RED,C16_RED,C16_RED,C16_RED  ,C16_RED ,C16_RED,C16_RED,C16_RED,
	C16_RED,C16_RED,C16_RED,C16_RED  ,C16_RED ,C16_RED,C16_RED,C16_RED};

u16 pat8x8_outline[]={
	C16_WHITE,C16_WHITE,C16_WHITE,C16_WHITE  ,C16_WHITE ,C16_WHITE,C16_WHITE,C16_WHITE,
	C16_WHITE,C16_BLACK,C16_BLACK,C16_BLACK,C16_BLACK,C16_BLACK,C16_BLACK,C16_WHITE,
	C16_WHITE,C16_BLACK,C16_BLACK,C16_BLACK,C16_BLACK,C16_BLACK,C16_BLACK,C16_WHITE,
	C16_WHITE,C16_BLACK,C16_BLACK,C16_BLACK,C16_BLACK,C16_BLACK,C16_BLACK,C16_WHITE,
	C16_WHITE,C16_BLACK,C16_BLACK,C16_BLACK,C16_BLACK,C16_BLACK,C16_BLACK,C16_WHITE,
	C16_WHITE,C16_BLACK,C16_BLACK,C16_BLACK,C16_BLACK,C16_BLACK,C16_BLACK,C16_WHITE,
	C16_WHITE,C16_BLACK,C16_BLACK,C16_BLACK,C16_BLACK,C16_BLACK,C16_BLACK,C16_WHITE,
	C16_WHITE,C16_WHITE,C16_WHITE,C16_WHITE  ,C16_WHITE ,C16_WHITE,C16_WHITE,C16_WHITE};
	
u16 pat8x8_white[]={
	C16_WHITE,C16_WHITE,C16_WHITE,C16_WHITE  ,C16_WHITE ,C16_WHITE,C16_WHITE,C16_WHITE,
	C16_WHITE,C16_WHITE,C16_WHITE,C16_WHITE,C16_WHITE,C16_WHITE,C16_WHITE,C16_WHITE,
	C16_WHITE,C16_WHITE,C16_WHITE,C16_WHITE,C16_WHITE,C16_WHITE,C16_WHITE,C16_WHITE,
	C16_WHITE,C16_WHITE,C16_WHITE,C16_WHITE,C16_WHITE,C16_WHITE,C16_WHITE,C16_WHITE,
	C16_WHITE,C16_WHITE,C16_WHITE,C16_WHITE,C16_WHITE,C16_WHITE,C16_WHITE,C16_WHITE,
	C16_WHITE,C16_WHITE,C16_WHITE,C16_WHITE,C16_WHITE,C16_WHITE,C16_WHITE,C16_WHITE,
	C16_WHITE,C16_WHITE,C16_WHITE,C16_WHITE,C16_WHITE,C16_WHITE,C16_WHITE,C16_WHITE,
	C16_WHITE,C16_WHITE,C16_WHITE,C16_WHITE  ,C16_WHITE ,C16_WHITE,C16_WHITE,C16_WHITE};

u16 pat8x8_black[]={
	C16_BLACK,C16_BLACK,C16_BLACK,C16_BLACK  ,C16_BLACK ,C16_BLACK,C16_BLACK,C16_BLACK,
	C16_BLACK,C16_BLACK,C16_BLACK,C16_BLACK,C16_BLACK,C16_BLACK,C16_BLACK,C16_BLACK,
	C16_BLACK,C16_BLACK,C16_BLACK,C16_BLACK,C16_BLACK,C16_BLACK,C16_BLACK,C16_BLACK,
	C16_BLACK,C16_BLACK,C16_BLACK,C16_BLACK,C16_BLACK,C16_BLACK,C16_BLACK,C16_BLACK,
	C16_BLACK,C16_BLACK,C16_BLACK,C16_BLACK,C16_BLACK,C16_BLACK,C16_BLACK,C16_BLACK,
	C16_BLACK,C16_BLACK,C16_BLACK,C16_BLACK,C16_BLACK,C16_BLACK,C16_BLACK,C16_BLACK,
	C16_BLACK,C16_BLACK,C16_BLACK,C16_BLACK,C16_BLACK,C16_BLACK,C16_BLACK,C16_BLACK,
	C16_BLACK,C16_BLACK,C16_BLACK,C16_BLACK  ,C16_BLACK ,C16_BLACK,C16_BLACK,C16_BLACK};	

u16 pat8x8_rgbw[]={
	C16_RED,C16_RED,C16_RED,C16_RED,C16_GREEN,C16_GREEN,C16_GREEN,C16_GREEN,
	C16_RED,C16_RED,C16_RED,C16_RED,C16_GREEN,C16_GREEN,C16_GREEN,C16_GREEN,
	C16_RED,C16_RED,C16_RED,C16_RED,C16_GREEN,C16_GREEN,C16_GREEN,C16_GREEN,
	C16_RED,C16_RED,C16_RED,C16_RED,C16_GREEN,C16_GREEN,C16_GREEN,C16_GREEN,
	C16_BLUE,C16_BLUE,C16_BLUE,C16_BLUE,C16_WHITE,C16_WHITE,C16_WHITE,C16_WHITE,
	C16_BLUE,C16_BLUE,C16_BLUE,C16_BLUE,C16_WHITE,C16_WHITE,C16_WHITE,C16_WHITE,
	C16_BLUE,C16_BLUE,C16_BLUE,C16_BLUE,C16_WHITE,C16_WHITE,C16_WHITE,C16_WHITE,
	C16_BLUE,C16_BLUE,C16_BLUE,C16_BLUE,C16_WHITE,C16_WHITE,C16_WHITE,C16_WHITE};


//extern void TestSFR(void);


void TestPointDrawingSimple(void)
{
	u32 i,j;
	u32 uG2dFbAddr, uG2dWidth, uG2dHeight;
	CSPACE eBpp;

#ifdef INTERRUPT_MODE
	INTC_SetVectAddr(NUM_2D, Isr_G2d);
	INTC_Enable(NUM_2D);
	G2D_InterruptEnable();
#endif

	G2D_GetFrmSz(&uG2dWidth, &uG2dHeight);
	G2D_GetFbAddr(&uG2dFbAddr);	
	G2D_GetSrcBppMode(&eBpp);
		
	UART_Printf("Width=%d, Height=%d, Framebuffer:0x%x\n", uG2dWidth, uG2dHeight, uG2dFbAddr);
	
	G2D_ClearFrameEcolor(G2D_BLACK, 0, 0);
	UART_Printf("Simple Point Drawing, press any key to continue\n");

	for(j=0; j<uG2dHeight; j++)
		for(i=0; i<uG2dWidth; i++)
			if(i<(uG2dWidth/2) && j<(uG2dHeight/2)) G2D_PutPixelEcolor(i, j, G2D_RED);
			else if(i>=(uG2dWidth/2) && (j<uG2dHeight/2)) G2D_PutPixelEcolor(i, j, G2D_GREEN);
			else if(i<(uG2dWidth/2) && j>=(uG2dHeight/2)) G2D_PutPixelEcolor(i, j, G2D_BLUE);
			else	 G2D_PutPixelEcolor(i, j, G2D_WHITE);
	UART_Printf("Red, Green, Bule and White are displayed, press any key to exit\n");
	UART_Getc();

#ifdef INTERRUPT_MODE	
	G2D_InterruptDisable();
	INTC_Disable(NUM_2D);
#endif
	
	G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);	
}

void TestPointDrawingWithRop(void)
{
	u32 i,j,k;
	u32 uG2dFbAddr, uG2dWidth, uG2dHeight;
	CSPACE eBpp;


#ifdef INTERRUPT_MODE
	INTC_SetVectAddr(NUM_2D, Isr_G2d);
	INTC_Enable(NUM_2D);
	G2D_InterruptEnable();
#endif


	G2D_GetFrmSz(&uG2dWidth, &uG2dHeight);
	G2D_GetFbAddr(&uG2dFbAddr);
	G2D_GetSrcBppMode(&eBpp);
	UART_Printf("Width=%d, Height=%d, Framebuffer:0x%x\n", uG2dWidth, uG2dHeight, uG2dFbAddr);

	G2D_ClearFrameEcolor(G2D_YELLOW, 0, 0);
	G2D_RegisterPattern(pat8x8_rgbw);

	

	UART_Printf("Point Drawing with Raster Operation(pattern), press any key to continue\n");
	G2D_Set3rdOperand(G2D_OPERAND3_PAT);
	UART_Getc();	

	for(k=0; k<=8; k++) {
		G2D_SetRopEtype((G2D_ROP_TYPE)(k));
		switch((G2D_ROP_TYPE)k){
			case ROP_DST_ONLY: 
				UART_Printf("Destination Only, press any key to continue\n");
				break;
			case ROP_SRC_ONLY: 
				UART_Printf("Source Only, press any key to continue\n");
				break;
			case ROP_3RD_OPRND_ONLY: 
				UART_Printf("Third Operand Only, press any key to continue\n");
				break;
			case ROP_SRC_AND_DST: 
				UART_Printf("Source AND Destination, press any key to continue\n");
				break;
			case ROP_SRC_AND_3RD_OPRND: 
				UART_Printf("Source AND Third Operand, press any key to continue\n");
				break;
			case ROP_SRC_OR_DST: 
				UART_Printf("Source OR Destination, press any key to continue\n");
				break;
			case ROP_SRC_OR_3RD_OPRND: 
				UART_Printf("Source OR Third Operand, press any key to continue\n");
				break;				
			case ROP_DST_OR_3RD:
				UART_Printf("Destination OR Third Operand, press any key to continue\n");
				break;
			case ROP_SRC_XOR_3RD_OPRND: 
				UART_Printf("Source XOR Third Operand, press any key to continue\n");
				break;
			default:
				break;
		} 

		for(j=0; j<uG2dHeight; j++)
			for(i=0; i<uG2dWidth; i++)
				if(i<(uG2dWidth/2) && j<(uG2dHeight/2)) G2D_PutPixelEcolor(i, j, G2D_RED);
				else if(i>=(uG2dWidth/2) && (j<uG2dHeight/2)) G2D_PutPixelEcolor(i, j, G2D_GREEN);
				else if(i<(uG2dWidth/2) && j>=(uG2dHeight/2)) G2D_PutPixelEcolor(i, j, G2D_BLUE);
				else	 G2D_PutPixelEcolor(i, j, G2D_WHITE);
		UART_Getc();
	}

#if 0 // Open this block when you want to test ROP that get the third operand via foreground color 
	UART_Printf("Point Drawing with Raster Operation(foreground), press any key to continue\n");
	G2D_ClearFrameEcolor(G2D_YELLOW, 0, 0);
	G2D_SetFgColor(G2D_MAGENTA);
	G2D_Set3rdOperand(G2D_OPERAND3_FG);
	G2D_SetRopEtype(ROP_DST_OR_3RD);
	UART_Getc();	

	for(j=0; j<uG2dHeight; j++)
		for(i=0; i<uG2dWidth; i++)
			G2D_PutPixelEcolor(i, j, G2D_MAGENTA);
#endif

#ifdef INTERRUPT_MODE	
	G2D_InterruptDisable();
	INTC_Disable(NUM_2D);
#endif

	G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);

}

void TestPointDrawingWithRotation(void)
{
	u32 i,j;
	u32 uG2dFbAddr, uG2dWidth, uG2dHeight;
	CSPACE eBpp;


#ifdef INTERRUPT_MODE
	INTC_SetVectAddr(NUM_2D, Isr_G2d);
	INTC_Enable(NUM_2D);
	G2D_InterruptEnable();
#endif


	G2D_GetFrmSz(&uG2dWidth, &uG2dHeight);
	G2D_GetFbAddr(&uG2dFbAddr);
	G2D_GetSrcBppMode(&eBpp);
	UART_Printf("Width=%d, Height=%d, Framebuffer:0x%x\n", uG2dWidth, uG2dHeight, uG2dFbAddr);

	G2D_ClearFrameEcolor(G2D_BLACK, 0, 0);
	for(j=0; j<uG2dHeight; j++)
		for(i=0; i<uG2dWidth; i++) {
			if(i==(uG2dWidth/2-1)) G2D_PutPixelEcolor(i, j, G2D_WHITE);
			if(j==(uG2dHeight/2-1)) G2D_PutPixelEcolor(i, j, G2D_WHITE);
		}	

	G2D_SetRotationMode(G2D_ROTATION_0_DEG_BIT);
	for(j=0; j<uG2dHeight; j++)
		for(i=0; i<uG2dWidth; i++) {
			if(i>=((uG2dWidth/2-1)+10) && i<((uG2dWidth/2-1)+50) && j > ((uG2dHeight/2-1)-50) && j<=((uG2dHeight/2-1)-10)) G2D_PutPixelEcolor(i, j, G2D_BLUE);
		}
	UART_Printf("Destination Image is displayed, press any key to test rotation\n");
	UART_Getc();

	G2D_SetRotationMode(G2D_ROTATION_90_DEG_BIT);
	G2D_SetRotationOrg((u16)(uG2dWidth/2-1), (u16)(uG2dHeight/2-1));
	for(j=0; j<uG2dHeight; j++)
		for(i=0; i<uG2dWidth; i++) {
			if(i>=((uG2dWidth/2-1)+10) && i<((uG2dWidth/2-1)+50) && j > ((uG2dHeight/2-1)-50) && j<=((uG2dHeight/2-1)-10)) G2D_PutPixelEcolor(i, j, G2D_RED);
		}
	UART_Printf("90 degree rotation, press any key to test rotation\n");
	UART_Getc();

	G2D_SetRotationMode(G2D_ROTATION_180_DEG_BIT);
	G2D_SetRotationOrg((u16)(uG2dWidth/2-1), (u16)(uG2dHeight/2-1));
	for(j=0; j<uG2dHeight; j++)
		for(i=0; i<uG2dWidth; i++) {
			if(i>=((uG2dWidth/2-1)+10) && i<((uG2dWidth/2-1)+50) && j > ((uG2dHeight/2-1)-50) && j<=((uG2dHeight/2-1)-10)) G2D_PutPixelEcolor(i, j, G2D_GREEN);
		}
	UART_Printf("180 degree rotation, press any key to test rotation\n");
	UART_Getc();

	G2D_SetRotationMode(G2D_ROTATION_270_DEG_BIT);
	G2D_SetRotationOrg((u16)(uG2dWidth/2-1), (u16)(uG2dHeight/2-1));
	for(j=0; j<uG2dHeight; j++)
		for(i=0; i<uG2dWidth; i++) {
			if(i>=((uG2dWidth/2-1)+10) && i<((uG2dWidth/2-1)+50) && j > ((uG2dHeight/2-1)-50) && j<=((uG2dHeight/2-1)-10)) G2D_PutPixelEcolor(i, j, G2D_WHITE);
		}
	UART_Printf("270 degree rotation, press any key to test rotation\n");
	UART_Getc();

	G2D_SetRotationMode(G2D_ROTATION_0_DEG_BIT);
	for(j=0; j<uG2dHeight; j++)
		for(i=0; i<uG2dWidth; i++) {
			if(i>=((uG2dWidth/2-1)+50) && i<((uG2dWidth/2-1)+90) && j > ((uG2dHeight/2-1)-90) && j<=((uG2dHeight/2-1)-50)) G2D_PutPixelEcolor(i, j, G2D_BLUE);
		}
	UART_Printf("Destination Image is displayed, press any key to test rotation\n");
	UART_Getc();	

	G2D_SetRotationMode(G2D_X_FLIP);
	G2D_SetRotationOrg((u16)(uG2dWidth/2-1), (u16)(uG2dHeight/2-1));
	for(j=0; j<uG2dHeight; j++)
		for(i=0; i<uG2dWidth; i++) {
			if(i>=((uG2dWidth/2-1)+50) && i<((uG2dWidth/2-1)+90) && j > ((uG2dHeight/2-1)-90) && j<=((uG2dHeight/2-1)-50)) G2D_PutPixelEcolor(i, j, G2D_YELLOW);
		}
	UART_Printf("X-FLIP, press any key to test rotation\n");
	UART_Getc();

	G2D_SetRotationMode(G2D_Y_FLIP);
	G2D_SetRotationOrg((u16)(uG2dWidth/2-1), (u16)(uG2dHeight/2-1));
	for(j=0; j<uG2dHeight; j++)
		for(i=0; i<uG2dWidth; i++) {
			if(i>=((uG2dWidth/2-1)+50) && i<((uG2dWidth/2-1)+90) && j > ((uG2dHeight/2-1)-90) && j<=((uG2dHeight/2-1)-50)) G2D_PutPixelEcolor(i, j, G2D_CYAN);
		}
	UART_Printf("Y-FLIP, press any key to exit\n");
	UART_Getc();

#ifdef INTERRUPT_MODE	
	G2D_InterruptDisable();
	INTC_Disable(NUM_2D);
#endif

	G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);	
}


void TestPointDrawingWithBlending(void)
{
	u32 i,j;
	u32 uG2dFbAddr, uG2dWidth, uG2dHeight;
	s32  sAlphaVal;	
	CSPACE eBpp;


#ifdef INTERRUPT_MODE
	INTC_SetVectAddr(NUM_2D, Isr_G2d);
	INTC_Enable(NUM_2D);
	G2D_InterruptEnable();
#endif

	G2D_GetFrmSz(&uG2dWidth, &uG2dHeight);
	G2D_GetFbAddr(&uG2dFbAddr);
	G2D_GetSrcBppMode(&eBpp);
	UART_Printf("Width=%d, Height=%d, Framebuffer:0x%x\n", uG2dWidth, uG2dHeight, uG2dFbAddr);

	UART_Printf("press any key to test alpha blending.\n");
	G2D_ClearFrameEcolor(G2D_YELLOW, 0, 0);


	G2D_SetAlphaMode(G2D_ALPHA_MODE);
	G2D_SetAlphaValue(0xff);
	
	for(j=0; j<uG2dHeight; j++)
		for(i=0; i<uG2dWidth; i++)
			if(i<(uG2dWidth/2) && j<(uG2dHeight/2)) G2D_PutPixelEcolor(i, j, G2D_RED);
			else if(i>=(uG2dWidth/2) && (j<uG2dHeight/2)) G2D_PutPixelEcolor(i, j, G2D_GREEN);
			else if(i<(uG2dWidth/2) && j>=(uG2dHeight/2)) G2D_PutPixelEcolor(i, j, G2D_BLUE);
			else	 G2D_PutPixelEcolor(i, j, G2D_WHITE);
			
	UART_Getc();
			
	sAlphaVal=255;

	while(1) {
		sAlphaVal -= 8;
		if(sAlphaVal<=0) sAlphaVal=0;
		G2D_SetAlphaValue((u8)sAlphaVal);
		G2D_ClearFrameEcolor(G2D_YELLOW, 0, 0);
		for(j=0; j<uG2dHeight; j++)
			for(i=0; i<uG2dWidth; i++)
				if(i<(uG2dWidth/2) && j<(uG2dHeight/2)) G2D_PutPixelEcolor(i, j, G2D_RED);
				else if(i>=(uG2dWidth/2) && (j<uG2dHeight/2)) G2D_PutPixelEcolor(i, j, G2D_GREEN);
				else if(i<(uG2dWidth/2) && j>=(uG2dHeight/2)) G2D_PutPixelEcolor(i, j, G2D_BLUE);
				else	 G2D_PutPixelEcolor(i, j, G2D_WHITE);
		UART_Printf("Alpha:%d\n", sAlphaVal);					
		if(sAlphaVal==0) break;
		UART_Getc();
	}

#ifdef INTERRUPT_MODE	
	G2D_InterruptDisable();
	INTC_Disable(NUM_2D);
#endif

	G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);	

}


void TestPointDrawingWithClipping(void)
{
	u32 i,j;
	u32 uG2dFbAddr, uG2dWidth, uG2dHeight;
	u32 uDec;	
	CSPACE eBpp;

#ifdef INTERRUPT_MODE
	INTC_SetVectAddr(NUM_2D, Isr_G2d);
	INTC_Enable(NUM_2D);
	G2D_InterruptEnable();
#endif

	G2D_GetFrmSz(&uG2dWidth, &uG2dHeight);
	G2D_GetFbAddr(&uG2dFbAddr);
	G2D_GetSrcBppMode(&eBpp);
	UART_Printf("Width=%d, Height=%d, Framebuffer:0x%x\n", uG2dWidth, uG2dHeight, uG2dFbAddr);

	
	G2D_ClearFrameEcolor(G2D_BLACK, 0, 0);
	UART_Printf("Simple Point Drawing, press any key to continue\n");

	for(j=0; j<uG2dHeight; j++)
		for(i=0; i<uG2dWidth; i++)
			if(i<(uG2dWidth/2) && j<(uG2dHeight/2)) G2D_PutPixelEcolor(i, j, G2D_RED);
			else if(i>=(uG2dWidth/2) && (j<uG2dHeight/2)) G2D_PutPixelEcolor(i, j, G2D_GREEN);
			else if(i<(uG2dWidth/2) && j>=(uG2dHeight/2)) G2D_PutPixelEcolor(i, j, G2D_BLUE);
			else	 G2D_PutPixelEcolor(i, j, G2D_WHITE);
	UART_Printf("Red, Green, Bule and White are displayed, press any key to test window clipping\n");	
	UART_Getc();

	uDec=0;
	while(1) {
		uDec+=8;
		if((uG2dWidth-2*uDec <=32)||(uG2dHeight-2*uDec <=32)) break;
		G2D_ClearFrameEcolor(G2D_BLACK, 0, 0);
		G2D_SetClippingWindow(0+uDec, 0+uDec, uG2dWidth-uDec, uG2dHeight-uDec);
		UART_Printf("[Clipping]x1=%d,y1=%d,x2=%d,y2=%d\n",uG2dWidth-uDec, uG2dHeight-uDec, uG2dWidth-uDec, uG2dHeight-uDec);
		for(j=0; j<uG2dHeight; j++)
			for(i=0; i<uG2dWidth; i++)
				if(i<(uG2dWidth/2) && j<(uG2dHeight/2)) G2D_PutPixelEcolor(i, j, G2D_RED);
				else if(i>=(uG2dWidth/2) && (j<uG2dHeight/2)) G2D_PutPixelEcolor(i, j, G2D_GREEN);
				else if(i<(uG2dWidth/2) && j>=(uG2dHeight/2)) G2D_PutPixelEcolor(i, j, G2D_BLUE);
				else	 G2D_PutPixelEcolor(i, j, G2D_WHITE);
		UART_Getc();		
	}

#ifdef INTERRUPT_MODE	
	G2D_InterruptDisable();
	INTC_Disable(NUM_2D);
#endif

	G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);

}

void TestPointDrawingWithAll(void)
{
	u32 i,j;
	u32 uG2dFbAddr, uG2dWidth, uG2dHeight;
	CSPACE eBpp;

#ifdef INTERRUPT_MODE
	INTC_SetVectAddr(NUM_2D, Isr_G2d);
	INTC_Enable(NUM_2D);
	G2D_InterruptEnable();
#endif

	G2D_GetFrmSz(&uG2dWidth, &uG2dHeight);
	G2D_GetFbAddr(&uG2dFbAddr);
	G2D_GetSrcBppMode(&eBpp);
	UART_Printf("Width=%d, Height=%d, Framebuffer:0x%x\n", uG2dWidth, uG2dHeight, uG2dFbAddr);

	UART_Printf("Point Drawing with All Operations, press any key to test.\n");
	G2D_ClearFrameEcolor(G2D_YELLOW, 0, 0);	
	G2D_RegisterPattern(pat8x8_outline);
	
	UART_Getc();


	G2D_SetAlphaMode(G2D_ALPHA_MODE);
	G2D_SetAlphaValue(0xcf);
	G2D_SetClippingWindow(uG2dWidth/4, uG2dHeight/4,uG2dWidth*3/4, uG2dHeight*3/4);
	G2D_Set3rdOperand(G2D_OPERAND3_PAT);
	G2D_SetRopEtype(ROP_SRC_AND_3RD_OPRND);
	
	for(j=0; j<uG2dHeight; j++)
		for(i=0; i<uG2dWidth; i++)
			if(i<(uG2dWidth/2) && j<(uG2dHeight/2)) G2D_PutPixelEcolor(i, j, G2D_RED);
			else if(i>=(uG2dWidth/2) && (j<uG2dHeight/2)) G2D_PutPixelEcolor(i, j, G2D_GREEN);
			else if(i<(uG2dWidth/2) && j>=(uG2dHeight/2)) G2D_PutPixelEcolor(i, j, G2D_BLUE);
			else	 G2D_PutPixelEcolor(i, j, G2D_WHITE);
		

	G2D_SetRotationMode(G2D_ROTATION_0_DEG_BIT);	
	G2D_SetRotationOrg((u16)(uG2dWidth/2-1), (u16)(uG2dHeight/2-1));
	for(j=0; j<uG2dHeight; j++)
		for(i=0; i<uG2dWidth; i++) {
			if(i>=((uG2dWidth/2-1)+10) && i<((uG2dWidth/2-1)+50) && j > ((uG2dHeight/2-1)-50) && j<=((uG2dHeight/2-1)-10)) G2D_PutPixelEcolor(i, j, G2D_BLACK);
		}
	G2D_SetRotationMode(G2D_ROTATION_90_DEG_BIT);	
	G2D_SetRotationOrg((u16)(uG2dWidth/2-1), (u16)(uG2dHeight/2-1));
	for(j=0; j<uG2dHeight; j++)
		for(i=0; i<uG2dWidth; i++) {
			if(i>=((uG2dWidth/2-1)+10) && i<((uG2dWidth/2-1)+50) && j > ((uG2dHeight/2-1)-50) && j<=((uG2dHeight/2-1)-10)) G2D_PutPixelEcolor(i, j, G2D_RED);
		} 
	G2D_SetRotationMode(G2D_ROTATION_180_DEG_BIT);	
	G2D_SetRotationOrg((u16)(uG2dWidth/2-1), (u16)(uG2dHeight/2-1));
	for(j=0; j<uG2dHeight; j++)
		for(i=0; i<uG2dWidth; i++) {
			if(i>=((uG2dWidth/2-1)+10) && i<((uG2dWidth/2-1)+50) && j > ((uG2dHeight/2-1)-50) && j<=((uG2dHeight/2-1)-10)) G2D_PutPixelEcolor(i, j, G2D_GREEN);
		} 
	G2D_SetRotationMode(G2D_ROTATION_270_DEG_BIT);	
	G2D_SetRotationOrg((u16)(uG2dWidth/2-1), (u16)(uG2dHeight/2-1));
	for(j=0; j<uG2dHeight; j++)
		for(i=0; i<uG2dWidth; i++) {
			if(i>=((uG2dWidth/2-1)+10) && i<((uG2dWidth/2-1)+50) && j > ((uG2dHeight/2-1)-50) && j<=((uG2dHeight/2-1)-10)) G2D_PutPixelEcolor(i, j, G2D_BLUE);
		} 

	UART_Printf("Press any key to exit!\n");
	UART_Getc();
	
#ifdef INTERRUPT_MODE	
	G2D_InterruptDisable();
	INTC_Disable(NUM_2D);
#endif
	
	G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);

}


void TestLineDrawingSimple(void)
{
	u32 uG2dFbAddr, uG2dWidth, uG2dHeight;	
	CSPACE eBpp;

#ifdef INTERRUPT_MODE
	INTC_SetVectAddr(NUM_2D, Isr_G2d);
	INTC_Enable(NUM_2D);
	G2D_InterruptEnable();
#endif


	G2D_GetFrmSz(&uG2dWidth, &uG2dHeight);
	G2D_GetFbAddr(&uG2dFbAddr);
	G2D_GetSrcBppMode(&eBpp);
	UART_Printf("Width=%d, Height=%d, Framebuffer:0x%x\n", uG2dWidth, uG2dHeight, uG2dFbAddr);
	
	G2D_ClearFrameEcolor(G2D_BLACK, 0, 0);
	UART_Printf("Simple Line Drawing, press any key to continue\n");
	UART_Getc();

	G2D_PutLineEcolor(0,0,uG2dWidth-1, 0, G2D_WHITE, 1);
	G2D_PutLineEcolor(0,0,0, uG2dHeight-1, G2D_WHITE, 1);
	G2D_PutLineEcolor(uG2dWidth-1,0,uG2dWidth-1, uG2dHeight-1, G2D_WHITE, 1);
	G2D_PutLineEcolor(0,uG2dHeight-1,uG2dWidth-1, uG2dHeight-1, G2D_WHITE, 1);
	G2D_PutLineEcolor(0,0,uG2dWidth-1, uG2dHeight-1, G2D_WHITE, 1);
	G2D_PutLineEcolor(0,uG2dHeight-1,uG2dWidth-1, 0, G2D_WHITE, 1);
	
#ifdef INTERRUPT_MODE
	G2D_InterruptDisable();
	INTC_Disable(NUM_2D);
#endif

	G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);

}

void TestLineDrawingWithRop(void)
{
	u32 i,k;
	u32 uG2dFbAddr, uG2dWidth, uG2dHeight;
	CSPACE eBpp;

#ifdef INTERRUPT_MODE
	INTC_SetVectAddr(NUM_2D, Isr_G2d);
	INTC_Enable(NUM_2D);
	G2D_InterruptEnable();
#endif

	G2D_GetFrmSz(&uG2dWidth, &uG2dHeight);
	G2D_GetFbAddr(&uG2dFbAddr);
	G2D_GetSrcBppMode(&eBpp);
	UART_Printf("Width=%d, Height=%d, Framebuffer:0x%x\n", uG2dWidth, uG2dHeight, uG2dFbAddr);
	
	G2D_ClearFrameEcolor(G2D_YELLOW, 0, 0);
	G2D_RegisterPattern(pat8x8_rgbw);

	
	G2D_Set3rdOperand(G2D_OPERAND3_PAT);
	UART_Printf("Point Drawing with Raster Operation, press any key to continue\n");
	UART_Getc();	

	for(k=0; k<=8; k++) {
		G2D_SetRopEtype((G2D_ROP_TYPE)(k));
		switch((G2D_ROP_TYPE)k){
			case ROP_DST_ONLY: 
				UART_Printf("Destination Only, press any key to continue\n");
				break;
			case ROP_SRC_ONLY: 
				UART_Printf("Source Only, press any key to continue\n");
				break;
			case ROP_3RD_OPRND_ONLY: 
				UART_Printf("Third Operand Only, press any key to continue\n");
				break;
			case ROP_SRC_AND_DST: 
				UART_Printf("Source AND Destination, press any key to continue\n");
				break;
			case ROP_SRC_AND_3RD_OPRND: 
				UART_Printf("Source AND Third Operand, press any key to continue\n");
				break;
			case ROP_SRC_OR_DST: 
				UART_Printf("Source OR Destination, press any key to continue\n");
				break;
			case ROP_SRC_OR_3RD_OPRND: 
				UART_Printf("Source OR Third Operand, press any key to continue\n");
				break;				
			case ROP_DST_OR_3RD:
				UART_Printf("Destination OR Third Operand, press any key to continue\n");
				break;
			case ROP_SRC_XOR_3RD_OPRND: 
				UART_Printf("Source XOR Third Operand, press any key to continue\n");
				break;
			default:
				break;
		} 


		for(i=0; i<uG2dHeight; i++)
			if((i%4)==0)
				G2D_PutLineEcolor(0,i,uG2dWidth-1,i,G2D_BLUE,1);
		G2D_PutLineEcolor(0,0,uG2dWidth-1, 0, G2D_RED, 1);
		G2D_PutLineEcolor(0,0,0, uG2dHeight-1, G2D_RED, 1);
		G2D_PutLineEcolor(uG2dWidth-1,0,uG2dWidth-1, uG2dHeight-1, G2D_RED, 1);
		G2D_PutLineEcolor(0,uG2dHeight-1,uG2dWidth-1, uG2dHeight-1, G2D_RED, 1);

		UART_Getc();
	}
 
#if 0 // Open this block when you want to test ROP that get the third operand via foreground color 
	UART_Printf("Point Drawing with Raster Operation(foreground), press any key to continue\n");
	G2D_ClearFrameEcolor(G2D_YELLOW, 0, 0);
	G2D_SetFgColor(G2D_MAGENTA);
	G2D_Set3rdOperand(G2D_OPERAND3_FG);
	G2D_SetRopEtype(ROP_DST_OR_3RD);
	UART_Getc();	

	for(j=0; j<uG2dHeight; j++)
		G2D_PutLineEcolor(0,j,uG2dWidth-1,uG2dHeight-1, G2D_MAGENTA,1);
#endif 	

#ifdef INTERRUPT_MODE	
	G2D_InterruptDisable();
	INTC_Disable(NUM_2D);
#endif

	G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);

}


void TestLineDrawingWithRotation(void)
{
	u32 uG2dFbAddr, uG2dWidth, uG2dHeight;
	CSPACE eBpp;
	

#ifdef INTERRUPT_MODE
	INTC_SetVectAddr(NUM_2D, Isr_G2d);
	INTC_Enable(NUM_2D);
	G2D_InterruptEnable();
#endif


	G2D_GetFrmSz(&uG2dWidth, &uG2dHeight);
	G2D_GetFbAddr(&uG2dFbAddr);
	G2D_GetSrcBppMode(&eBpp);
	UART_Printf("Width=%d, Height=%d, Framebuffer:0x%x\n", uG2dWidth, uG2dHeight, uG2dFbAddr);

	G2D_ClearFrameEcolor(G2D_BLACK, 0, 0);
	G2D_PutLineEcolor(0,uG2dHeight/2-1,uG2dWidth-1, uG2dHeight/2-1, G2D_WHITE, 1);
	G2D_PutLineEcolor(uG2dWidth/2-1,0, uG2dWidth/2-1, uG2dHeight-1,G2D_WHITE, 1);

	G2D_SetRotationMode(G2D_ROTATION_0_DEG_BIT);
	G2D_PutLineEcolor(uG2dWidth/2-1, uG2dHeight/2-1, uG2dWidth/2-1+20, uG2dHeight/2-1-20, G2D_WHITE,1);
	UART_Printf("Destination Image is displayed, press any key to test rotation\n");
	UART_Getc();

	G2D_SetRotationMode(G2D_ROTATION_90_DEG_BIT);
	G2D_SetRotationOrg((u16)(uG2dWidth/2-1), (u16)(uG2dHeight/2-1));
	G2D_PutLineEcolor(uG2dWidth/2-1, uG2dHeight/2-1, uG2dWidth/2-1+20, uG2dHeight/2-1-20, G2D_RED,1);
	UART_Printf("90 degree rotation, press any key to test rotation\n");
	UART_Getc();

	G2D_SetRotationMode(G2D_ROTATION_180_DEG_BIT);
	G2D_SetRotationOrg((u16)(uG2dWidth/2-1), (u16)(uG2dHeight/2-1));
	G2D_PutLineEcolor(uG2dWidth/2-1, uG2dHeight/2-1, uG2dWidth/2-1+20, uG2dHeight/2-1-20, G2D_GREEN,1);
	UART_Printf("180 degree rotation, press any key to test rotation\n");
	UART_Getc();

	G2D_SetRotationMode(G2D_ROTATION_270_DEG_BIT);
	G2D_SetRotationOrg((u16)(uG2dWidth/2-1), (u16)(uG2dHeight/2-1));
	G2D_PutLineEcolor(uG2dWidth/2-1, uG2dHeight/2-1, uG2dWidth/2-1+20, uG2dHeight/2-1-20, G2D_BLUE,1);
	UART_Printf("270 degree rotation, press any key to test rotation\n");
	UART_Getc();

	G2D_SetRotationMode(G2D_ROTATION_0_DEG_BIT);
	G2D_PutLineEcolor(uG2dWidth/2-1+20, uG2dHeight/2-1-20, uG2dWidth/2-1+40, uG2dHeight/2-1, G2D_YELLOW,1);
	UART_Printf("Destination Image is displayed, press any key to test rotation\n");
	UART_Getc();	

	G2D_SetRotationMode(G2D_X_FLIP);
	G2D_SetRotationOrg((u16)(uG2dWidth/2-1), (u16)(uG2dHeight/2-1));
	G2D_PutLineEcolor(uG2dWidth/2-1+20, uG2dHeight/2-1-20, uG2dWidth/2-1+40, uG2dHeight/2-1, G2D_CYAN,1);
	UART_Printf("X-FLIP, press any key to test rotation\n");
	UART_Getc();

	G2D_SetRotationMode(G2D_Y_FLIP);
	G2D_SetRotationOrg((u16)(uG2dWidth/2-1), (u16)(uG2dHeight/2-1));
	G2D_PutLineEcolor(uG2dWidth/2-1+20, uG2dHeight/2-1-20, uG2dWidth/2-1+40, uG2dHeight/2-1, G2D_MAGENTA,1);
	UART_Printf("Y-FLIP, press any key to exit\n");
	UART_Getc();

#ifdef INTERRUPT_MODE	
	G2D_InterruptDisable();
	INTC_Disable(NUM_2D);
#endif

	G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);

}

void TestLineDrawingWithBlending(void)
{
	u32 i;
	u32 uG2dFbAddr, uG2dWidth, uG2dHeight;
	s32  sAlphaVal;
	CSPACE eBpp;


#ifdef INTERRUPT_MODE
	INTC_SetVectAddr(NUM_2D, Isr_G2d);
	INTC_Enable(NUM_2D);
	G2D_InterruptEnable();
#endif


	G2D_GetFrmSz(&uG2dWidth, &uG2dHeight);
	G2D_GetFbAddr(&uG2dFbAddr);
	G2D_GetSrcBppMode(&eBpp);
	UART_Printf("Width=%d, Height=%d, Framebuffer:0x%x\n", uG2dWidth, uG2dHeight, uG2dFbAddr);

	UART_Printf("press any key to test alpha blending.\n");
	G2D_ClearFrameEcolor(G2D_YELLOW, 0, 0);
	UART_Getc();


	G2D_SetAlphaMode(G2D_ALPHA_MODE);
	G2D_SetAlphaValue(0xff);
	
	for(i=0; i<uG2dHeight; i++)
		if((i%4)==0)
			G2D_PutLineEcolor(0,i,uG2dWidth-1,i,G2D_BLUE,1);
	for(i=0; i<uG2dWidth; i++)
		if((i%4)==0)
			G2D_PutLineEcolor(i,0,i,uG2dHeight-1,G2D_BLUE,1);
			
	sAlphaVal=255;
	while(1) {
		sAlphaVal -= 8;
		if(sAlphaVal<=0) sAlphaVal=0;
		G2D_SetAlphaValue((u8)sAlphaVal);
		G2D_ClearFrameEcolor(G2D_YELLOW, 0, 0);
		for(i=0; i<uG2dHeight; i++)
			if((i%4)==0)
				G2D_PutLineEcolor(0,i,uG2dWidth-1,i,G2D_BLUE,1);		
		for(i=0; i<uG2dWidth; i++)
			if((i%4)==0)
				G2D_PutLineEcolor(i,0,i,uG2dHeight-1,G2D_BLUE,1);

		UART_Printf("Alpha:%d\n", sAlphaVal);					
		if(sAlphaVal==0) break;
		UART_Getc();
	}


#ifdef INTERRUPT_MODE	
	G2D_InterruptDisable();
	INTC_Disable(NUM_2D);
#endif

	G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);


}

void TestLineDrawingClipping(void)
{
	u32 uG2dFbAddr, uG2dWidth, uG2dHeight;
	u32 uDec;	
	CSPACE eBpp;

#ifdef INTERRUPT_MODE
	INTC_SetVectAddr(NUM_2D, Isr_G2d);
	INTC_Enable(NUM_2D);
	G2D_InterruptEnable();
#endif


	G2D_GetFrmSz(&uG2dWidth, &uG2dHeight);
	G2D_GetFbAddr(&uG2dFbAddr);
	G2D_GetSrcBppMode(&eBpp);
	UART_Printf("Width=%d, Height=%d, Framebuffer:0x%x\n", uG2dWidth, uG2dHeight, uG2dFbAddr);
	
	G2D_ClearFrameEcolor(G2D_BLACK, 0, 0);
	UART_Printf("Simple Line Drawing, press any key to continue\n");
	UART_Getc();
	
	G2D_PutLineEcolor(0,0,uG2dWidth-1, 0, G2D_WHITE, 1);
	G2D_PutLineEcolor(0,0,0, uG2dHeight-1, G2D_WHITE, 1);
	G2D_PutLineEcolor(uG2dWidth-1,0,uG2dWidth-1, uG2dHeight-1, G2D_WHITE, 1);
	G2D_PutLineEcolor(0,uG2dHeight-1,uG2dWidth-1, uG2dHeight-1, G2D_WHITE, 1);
	G2D_PutLineEcolor(0,0,uG2dWidth-1, uG2dHeight-1, G2D_WHITE, 1);
	G2D_PutLineEcolor(0,uG2dHeight-1,uG2dWidth-1, 0, G2D_WHITE, 1);
	UART_Printf("Lines are displayed, press any key to test window clipping\n");	
	UART_Getc();

	uDec=0;
	while(1){
		uDec+=8;
		if((uG2dWidth-2*uDec <=32)||(uG2dHeight-2*uDec <=32)) break;
		G2D_ClearFrameEcolor(G2D_BLACK, 0, 0);
		G2D_SetClippingWindow(0+uDec, 0+uDec, uG2dWidth-uDec, uG2dHeight-uDec);
		UART_Printf("[Clipping]x1=%d,y1=%d,x2=%d,y2=%d\n",uG2dWidth-uDec, uG2dHeight-uDec, uG2dWidth-uDec, uG2dHeight-uDec);
		G2D_PutLineEcolor(0,0,uG2dWidth-1, 0, G2D_WHITE, 1);
		G2D_PutLineEcolor(0,0,0, uG2dHeight-1, G2D_WHITE, 1);
		G2D_PutLineEcolor(uG2dWidth-1,0,uG2dWidth-1, uG2dHeight-1, G2D_WHITE, 1);
		G2D_PutLineEcolor(0,uG2dHeight-1,uG2dWidth-1, uG2dHeight-1, G2D_WHITE, 1);
		G2D_PutLineEcolor(0,0,uG2dWidth-1, uG2dHeight-1, G2D_WHITE, 1);
		G2D_PutLineEcolor(0,uG2dHeight-1,uG2dWidth-1, 0, G2D_WHITE, 1);
		UART_Getc();
	}
	
#ifdef INTERRUPT_MODE	
	G2D_InterruptDisable();
	INTC_Disable(NUM_2D);
#endif

	G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);

}


void TestLineDrawingWithAll(void)
{
	u32 uG2dFbAddr, uG2dWidth, uG2dHeight;
	CSPACE eBpp;
	

#ifdef INTERRUPT_MODE
	INTC_SetVectAddr(NUM_2D, Isr_G2d);
	INTC_Enable(NUM_2D);
	G2D_InterruptEnable();
#endif


	G2D_GetFrmSz(&uG2dWidth, &uG2dHeight);
	G2D_GetFbAddr(&uG2dFbAddr);
	G2D_GetSrcBppMode(&eBpp);
	UART_Printf("Width=%d, Height=%d, Framebuffer:0x%x\n", uG2dWidth, uG2dHeight, uG2dFbAddr);

	UART_Printf("Point Drawing with All Operations, press any key to test.\n");
	G2D_ClearFrameEcolor(G2D_YELLOW, 0, 0);	
	G2D_RegisterPattern(pat8x8_rgbw);
	UART_Getc();


	G2D_SetAlphaMode(G2D_ALPHA_MODE);
	G2D_SetAlphaValue(0xcf);
	G2D_SetClippingWindow(uG2dWidth/4, uG2dHeight/4,uG2dWidth*3/4, uG2dHeight*3/4);
	G2D_Set3rdOperand(G2D_OPERAND3_PAT);
	G2D_SetRopEtype(ROP_SRC_XOR_3RD_OPRND);

	
	G2D_PutLineEcolor(0,uG2dHeight/2-1,uG2dWidth-1, uG2dHeight/2-1, G2D_BLACK, 1);
	G2D_PutLineEcolor(uG2dWidth/2-1,0, uG2dWidth/2-1, uG2dHeight-1,G2D_BLACK, 1);
	G2D_PutLineEcolor(0,0,uG2dWidth-1, 0, G2D_BLACK, 1);
	G2D_PutLineEcolor(0,0,0, uG2dHeight-1, G2D_BLACK, 1);
	G2D_PutLineEcolor(uG2dWidth-1,0,uG2dWidth-1, uG2dHeight-1, G2D_BLACK, 1);
	G2D_PutLineEcolor(0,uG2dHeight-1,uG2dWidth-1, uG2dHeight-1, G2D_BLACK, 1);
	

	G2D_SetRotationMode(G2D_ROTATION_0_DEG_BIT);	
	G2D_PutLineEcolor(uG2dWidth/2-1, uG2dHeight/2-1, uG2dWidth/2-1+20, uG2dHeight/2-1-20, G2D_BLACK,1);

	G2D_SetRotationMode(G2D_ROTATION_90_DEG_BIT);	
	G2D_SetRotationOrg((u16)(uG2dWidth/2-1), (u16)(uG2dHeight/2-1));
	G2D_PutLineEcolor(uG2dWidth/2-1, uG2dHeight/2-1, uG2dWidth/2-1+20, uG2dHeight/2-1-20, G2D_RED,1);


	G2D_SetRotationMode(G2D_ROTATION_180_DEG_BIT);	
	G2D_SetRotationOrg((u16)(uG2dWidth/2-1), (u16)(uG2dHeight/2-1));
	G2D_PutLineEcolor(uG2dWidth/2-1, uG2dHeight/2-1, uG2dWidth/2-1+20, uG2dHeight/2-1-20, G2D_GREEN,1);

	G2D_SetRotationMode(G2D_ROTATION_270_DEG_BIT);	
	G2D_SetRotationOrg((u16)(uG2dWidth/2-1), (u16)(uG2dHeight/2-1));
	G2D_PutLineEcolor(uG2dWidth/2-1, uG2dHeight/2-1, uG2dWidth/2-1+20, uG2dHeight/2-1-20, G2D_BLUE,1);

	G2D_SetRotationMode(G2D_ROTATION_0_DEG_BIT);
	G2D_PutLineEcolor(uG2dWidth/2-1+20, uG2dHeight/2-1-20, uG2dWidth/2-1+40, uG2dHeight/2-1, G2D_BLACK,1);

	G2D_SetRotationMode(G2D_X_FLIP);
	G2D_SetRotationOrg((u16)(uG2dWidth/2-1), (u16)(uG2dHeight/2-1));
	G2D_PutLineEcolor(uG2dWidth/2-1+20, uG2dHeight/2-1-20, uG2dWidth/2-1+40, uG2dHeight/2-1, G2D_CYAN,1);

	G2D_SetRotationMode(G2D_Y_FLIP);
	G2D_SetRotationOrg((u16)(uG2dWidth/2-1), (u16)(uG2dHeight/2-1));
	G2D_PutLineEcolor(uG2dWidth/2-1+20, uG2dHeight/2-1-20, uG2dWidth/2-1+40, uG2dHeight/2-1, G2D_MAGENTA,1);
 	

	UART_Printf("Press any key to exit!\n");
	UART_Getc();

#ifdef INTERRUPT_MODE	
	G2D_InterruptDisable();
	INTC_Disable(NUM_2D);
#endif

	G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);


}



void TestLineDrawingLastPixel(void)
{
	u32 uG2dFbAddr, uG2dWidth, uG2dHeight;
	u8 IsDrawLastPoint;
	CSPACE eBpp;
	

#ifdef INTERRUPT_MODE
	INTC_SetVectAddr(NUM_2D, Isr_G2d);
	INTC_Enable(NUM_2D);
	G2D_InterruptEnable();
#endif


	G2D_GetFrmSz(&uG2dWidth, &uG2dHeight);
	G2D_GetFbAddr(&uG2dFbAddr);
	G2D_GetSrcBppMode(&eBpp);
	UART_Printf("Width=%d, Height=%d, Framebuffer:0x%x\n", uG2dWidth, uG2dHeight, uG2dFbAddr);
	
	G2D_ClearFrameEcolor(G2D_BLACK, 0, 0);
	G2D_PutLineEcolor(uG2dWidth*3/4, 0, uG2dWidth*3/4, uG2dHeight-1, G2D_RED, 1);

	UART_Printf("Line Drawing with Draw Last Point, press any key to continue\n");
	UART_Getc();
	IsDrawLastPoint=1;
	G2D_PutLineEcolor(0, uG2dHeight/4,uG2dWidth*3/4, uG2dHeight/4, G2D_BLUE, IsDrawLastPoint);
	
	UART_Printf("Line Drawing with Do Not Draw Last Point, press any key to continue\n");
	UART_Getc();
	IsDrawLastPoint=0;
	G2D_PutLineEcolor(0, uG2dHeight*3/4,uG2dWidth*3/4, uG2dHeight*3/4, G2D_BLUE, IsDrawLastPoint);

	UART_Printf("Press any key to exit!\n");
	UART_Getc();
	
#ifdef INTERRUPT_MODE	
	G2D_InterruptDisable();
	INTC_Disable(NUM_2D);
#endif

	G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);

}

void TestColorExpansionSimple(void)
{
	u32 uG2dFbAddr, uG2dWidth, uG2dHeight;
	u32 uFontWidth, uFontHeight;
	u32 uTextDrawingFbAddr;
	CSPACE eBpp;
	

	u8 bIsTransparent=1;
	
#ifdef INTERRUPT_MODE
	INTC_SetVectAddr(NUM_2D, Isr_G2d);
	INTC_Enable(NUM_2D);
	G2D_InterruptEnable();
#endif

	
	G2D_GetFrmSz(&uG2dWidth, &uG2dHeight);
	G2D_GetFbAddr(&uG2dFbAddr);
	G2D_GetSrcBppMode(&eBpp);
	UART_Printf("Width=%d, Height=%d, Framebuffer:0x%x\n", uG2dWidth, uG2dHeight, uG2dFbAddr);

	// Host to Screen Test
	UART_Printf("Simple Host to Screen Color Expansion, press any key to continue");
	G2D_ClearFrameEcolor(G2D_BLACK, 0, 0);
	UART_Getc();
	G2D_SetColorExpansionMethod(HOST2SCREEN);

	G2D_SetFontColor(G2D_RED, G2D_BLACK, bIsTransparent);	
	G2D_SetFontType(FONT8BY8, &uFontWidth, &uFontHeight);	
	G2D_Printf(5, 5, "[2D]H2S Color Expansion");
	UART_Printf("8x8 font is dispayed using color expansion, press any key to continue.\n");
	UART_Getc();

	G2D_SetFontColor(G2D_GREEN, G2D_BLACK, bIsTransparent);	
	G2D_SetFontType(FONT8BY15, &uFontWidth, &uFontHeight);	
	G2D_Printf(5, uG2dHeight*1/4+5, "[2D]H2S Color Expansion");
	UART_Printf("8x15 font is dispayed using color expansion, press any key to continue.\n");
	UART_Getc();

	G2D_SetFontColor(G2D_BLUE, G2D_BLACK, bIsTransparent);	
	G2D_SetFontType(FONT8BY16T, &uFontWidth, &uFontHeight);	
	G2D_Printf(5, uG2dHeight*2/4+5, "[2D]H2S Color Expansion");
	UART_Printf("8x16T font is dispayed using color expansion, press any key to continue.\n");
	UART_Getc();

	G2D_SetFontColor(G2D_WHITE, G2D_BLACK, bIsTransparent);	
	G2D_SetFontType(FONT8BY16, &uFontWidth, &uFontHeight);	
	G2D_Printf(5, uG2dHeight*3/4+5, "[2D]H2S Color Expansion");
	UART_Printf("8x16 font is dispayed using color expansion, press any key to continue.\n");
	UART_Getc();	



	// Screen to Screen Test
	UART_Printf("Simple Screen to Screen Color Expansion, press any key to continue");
	G2D_ClearFrameEcolor(G2D_BLACK, 0, 0);
	UART_Getc();
	
	uTextDrawingFbAddr=(uG2dFbAddr&0xff000000)+0x800000;
	G2D_SetColorExpansionMethod(SCREEN2SCREEN);
	G2D_SetFontAddr(uTextDrawingFbAddr);
	

	G2D_SetFontColor(G2D_RED, G2D_BLACK, bIsTransparent);	
	G2D_FillMemoryForTextDrawing(FONT8BY8, uTextDrawingFbAddr);
	G2D_SetFontType(FONT8BY8, &uFontWidth, &uFontHeight);	
	G2D_Printf(5, 5, "[2D]S2S Color Expansion");
	UART_Printf("8x8 font is dispayed using color expansion, press any key to continue.\n");
	UART_Getc();
	
	G2D_SetFontColor(G2D_GREEN, G2D_BLACK, bIsTransparent);	
	G2D_FillMemoryForTextDrawing(FONT8BY15, uTextDrawingFbAddr);
	G2D_SetFontType(FONT8BY15, &uFontWidth, &uFontHeight);	
	G2D_Printf(5, uG2dHeight*1/4+5, "[2D]S2S Color Expansion");
	UART_Printf("8x15 font is dispayed using color expansion, press any key to continue.\n");
	UART_Getc();

	G2D_SetFontColor(G2D_BLUE, G2D_BLACK, bIsTransparent);	
	G2D_FillMemoryForTextDrawing(FONT8BY16T, uTextDrawingFbAddr);
	G2D_SetFontType(FONT8BY16T, &uFontWidth, &uFontHeight);	
	G2D_Printf(5, uG2dHeight*2/4+5, "[2D]S2S Color Expansion");
	UART_Printf("8x16T font is dispayed using color expansion, press any key to continue.\n");
	UART_Getc();

	G2D_SetFontColor(G2D_WHITE, G2D_BLACK, bIsTransparent);	
	G2D_FillMemoryForTextDrawing(FONT8BY16, uTextDrawingFbAddr);
	G2D_SetFontType(FONT8BY16, &uFontWidth, &uFontHeight);	
	G2D_Printf(5, uG2dHeight*3/4+5, "[2D]S2S Color Expansion");
	UART_Printf("8x16 font is dispayed using color expansion, press any key to exit.\n");
	UART_Getc();
	
#ifdef INTERRUPT_MODE	
	G2D_InterruptDisable();
	INTC_Disable(NUM_2D);
#endif
	
	G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);
  

}


void TestColorExpansionWithRop(void)
{
	u32 k;
	u32 uG2dFbAddr, uG2dWidth, uG2dHeight;
	u32 uFontWidth, uFontHeight;
	u32 uTextDrawingFbAddr;
	CSPACE eBpp;
	

	u8 bIsTransparent=1;

#ifdef INTERRUPT_MODE
	INTC_SetVectAddr(NUM_2D, Isr_G2d);
	INTC_Enable(NUM_2D);
	G2D_InterruptEnable();
#endif

	G2D_GetFrmSz(&uG2dWidth, &uG2dHeight);
	G2D_GetFbAddr(&uG2dFbAddr);
	G2D_GetSrcBppMode(&eBpp);
	UART_Printf("Width=%d, Height=%d, Framebuffer:0x%x\n", uG2dWidth, uG2dHeight, uG2dFbAddr);

	G2D_ClearFrameEcolor(G2D_YELLOW, 0, 0);
	G2D_RegisterPattern(pat8x8_rgbw);
	G2D_Set3rdOperand(G2D_OPERAND3_PAT);


	// Host to Screen Test
	UART_Printf("Host to Screen Color Expansion with ROP, press any key to continue!\n");	
	G2D_SetColorExpansionMethod(HOST2SCREEN);
	G2D_SetFontType(FONT8BY8, &uFontWidth, &uFontHeight);	
	UART_Getc();
	
	for(k=0; k<=8; k++) {
		G2D_SetRopEtype((G2D_ROP_TYPE)(k));
		switch((G2D_ROP_TYPE)k){
			case ROP_DST_ONLY: 
				UART_Printf("Destination Only, press any key to continue\n");
				break;
			case ROP_SRC_ONLY: 
				UART_Printf("Source Only, press any key to continue\n");
				break;
			case ROP_3RD_OPRND_ONLY: 
				UART_Printf("Third Operand Only, press any key to continue\n");
				break;
			case ROP_SRC_AND_DST: 
				UART_Printf("Source AND Destination, press any key to continue\n");
				break;
			case ROP_SRC_AND_3RD_OPRND: 
				UART_Printf("Source AND Third Operand, press any key to continue\n");
				break;
			case ROP_SRC_OR_DST: 
				UART_Printf("Source OR Destination, press any key to continue\n");
				break;
			case ROP_SRC_OR_3RD_OPRND: 
				UART_Printf("Source OR Third Operand, press any key to continue\n");
				break;				
			case ROP_DST_OR_3RD:
				UART_Printf("Destination OR Third Operand, press any key to continue\n");
				break;
			case ROP_SRC_XOR_3RD_OPRND: 
				UART_Printf("Source XOR Third Operand, press any key to continue\n");
				break;
			default:
				break;
		} 

		G2D_SetFontColor(G2D_RED, G2D_BLACK, bIsTransparent);	
		G2D_Printf(5, 5, "[2D]H2S Color Expansion");
		G2D_SetFontColor(G2D_GREEN, G2D_BLACK, bIsTransparent);	
		G2D_Printf(5, uG2dHeight*1/4+5, "[2D]H2S Color Expansion");
		G2D_SetFontColor(G2D_BLUE, G2D_BLACK, bIsTransparent);	
		G2D_Printf(5, uG2dHeight*2/4+5, "[2D]H2S Color Expansion");
		G2D_SetFontColor(G2D_WHITE, G2D_BLACK, bIsTransparent);	
		G2D_Printf(5, uG2dHeight*3/4+5, "[2D]H2S Color Expansion");

		UART_Getc();
	}

	G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);
	G2D_ClearFrameEcolor(G2D_YELLOW, 0, 0);


	// Screen to Screen Test
	
	UART_Printf("Screen to Screen Color Expansion with ROP, press any key to continue!\n");	
	G2D_SetColorExpansionMethod(SCREEN2SCREEN);
	G2D_SetFontType(FONT8BY8, &uFontWidth, &uFontHeight);	
	uTextDrawingFbAddr=(uG2dFbAddr&0xff000000)+0x800000;
	G2D_FillMemoryForTextDrawing(FONT8BY8, uTextDrawingFbAddr);
	G2D_SetFontAddr(uTextDrawingFbAddr);
	G2D_Set3rdOperand(G2D_OPERAND3_PAT);	
	UART_Getc();


	for(k=0; k<=8; k++) {
		G2D_SetRopEtype((G2D_ROP_TYPE)(k));
		switch((G2D_ROP_TYPE)k){
			case ROP_DST_ONLY: 
				UART_Printf("Destination Only, press any key to continue\n");
				break;
			case ROP_SRC_ONLY: 
				UART_Printf("Source Only, press any key to continue\n");
				break;
			case ROP_3RD_OPRND_ONLY: 
				UART_Printf("Third Operand Only, press any key to continue\n");
				break;
			case ROP_SRC_AND_DST: 
				UART_Printf("Source AND Destination, press any key to continue\n");
				break;
			case ROP_SRC_AND_3RD_OPRND: 
				UART_Printf("Source AND Third Operand, press any key to continue\n");
				break;
			case ROP_SRC_OR_DST: 
				UART_Printf("Source OR Destination, press any key to continue\n");
				break;
			case ROP_SRC_OR_3RD_OPRND: 
				UART_Printf("Source OR Third Operand, press any key to continue\n");
				break;				
			case ROP_DST_OR_3RD:
				UART_Printf("Destination OR Third Operand, press any key to continue\n");
				break;
			case ROP_SRC_XOR_3RD_OPRND: 
				UART_Printf("Source XOR Third Operand, press any key to continue\n");
				break;
			default:
				break;
		} 

		G2D_SetFontColor(G2D_RED, G2D_BLACK, bIsTransparent);	
		G2D_Printf(5, 5, "[2D]S2S Color Expansion");
		G2D_SetFontColor(G2D_GREEN, G2D_BLACK, bIsTransparent); 
		G2D_Printf(5, uG2dHeight*1/4+5, "[2D]S2S Color Expansion");
		G2D_SetFontColor(G2D_BLUE, G2D_BLACK, bIsTransparent);	
		G2D_Printf(5, uG2dHeight*2/4+5, "[2D]S2S Color Expansion");
		G2D_SetFontColor(G2D_WHITE, G2D_BLACK, bIsTransparent); 
		G2D_Printf(5, uG2dHeight*3/4+5, "[2D]S2S Color Expansion");

		UART_Getc();
	}

#ifdef INTERRUPT_MODE	
	G2D_InterruptDisable();
	INTC_Disable(NUM_2D);
#endif

	G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);

}


void TestColorExpansionWithRotation(void)
{
	u32 i,j;
	u32 uG2dFbAddr, uG2dWidth, uG2dHeight;
	u32 uFontWidth, uFontHeight;
	u32 uTextDrawingFbAddr;
	CSPACE eBpp;
	

	u8 bIsTransparent=1;
	
#ifdef INTERRUPT_MODE
	INTC_SetVectAddr(NUM_2D, Isr_G2d);
	INTC_Enable(NUM_2D);
	G2D_InterruptEnable();
#endif

	
	G2D_GetFrmSz(&uG2dWidth, &uG2dHeight);
	G2D_GetFbAddr(&uG2dFbAddr);
	G2D_GetSrcBppMode(&eBpp);
	UART_Printf("Width=%d, Height=%d, Framebuffer:0x%x\n", uG2dWidth, uG2dHeight, uG2dFbAddr);


	// Host to Screen Test
	UART_Printf("Simple Host to Screen Color Expansion, press any key to continue");
	G2D_ClearFrameEcolor(G2D_BLACK, 0, 0);
	for(j=0; j<uG2dHeight; j++)
		for(i=0; i<uG2dWidth; i++) {
			if(i==(uG2dWidth/2-1)) G2D_PutPixelEcolor(i, j, G2D_MAGENTA);
			if(j==(uG2dHeight/2-1)) G2D_PutPixelEcolor(i, j, G2D_MAGENTA);
		}
	UART_Getc();	
	G2D_SetColorExpansionMethod(HOST2SCREEN);
	G2D_SetFontType(FONT8BY8, &uFontWidth, &uFontHeight);	
	
	G2D_SetRotationMode(G2D_ROTATION_0_DEG_BIT);
	G2D_SetFontColor(G2D_WHITE, G2D_BLACK, bIsTransparent);	
	G2D_Printf(uG2dWidth/2-1, uG2dHeight/2-1, "ROTATION");
	UART_Printf("8x8 font is dispayed using color expansion, press any key to continue.\n");
	UART_Getc();

	G2D_SetRotationMode(G2D_ROTATION_90_DEG_BIT);
	G2D_SetRotationOrg((u16)(uG2dWidth/2-1), (u16)(uG2dHeight/2-1));
	G2D_SetFontColor(G2D_RED, G2D_BLACK, bIsTransparent);	
	G2D_Printf(uG2dWidth/2-1, uG2dHeight/2-1, "ROTATION");
	UART_Printf("90 degree rotation, press any key to test rotation\n");
	UART_Getc();

	G2D_SetRotationMode(G2D_ROTATION_180_DEG_BIT);
	G2D_SetRotationOrg((u16)(uG2dWidth/2-1), (u16)(uG2dHeight/2-1));
	G2D_SetFontColor(G2D_GREEN, G2D_BLACK, bIsTransparent);	
	G2D_Printf(uG2dWidth/2-1, uG2dHeight/2-1, "ROTATION");
	UART_Printf("180 degree rotation, press any key to test rotation\n");
	UART_Getc();

	G2D_SetRotationMode(G2D_ROTATION_270_DEG_BIT);
	G2D_SetRotationOrg((u16)(uG2dWidth/2-1), (u16)(uG2dHeight/2-1));
	G2D_SetFontColor(G2D_BLUE, G2D_BLACK, bIsTransparent);	
	G2D_Printf(uG2dWidth/2-1, uG2dHeight/2-1, "ROTATION");
	UART_Printf("270 degree rotation, press any key to test rotation\n");
	UART_Getc();	

	G2D_SetRotationMode(G2D_ROTATION_0_DEG_BIT);
	G2D_ClearFrameEcolor(G2D_BLACK, 0, 0);
	for(j=0; j<uG2dHeight; j++)
		for(i=0; i<uG2dWidth; i++) {
			if(i==(uG2dWidth/2-1)) G2D_PutPixelEcolor(i, j, G2D_MAGENTA);
			if(j==(uG2dHeight/2-1)) G2D_PutPixelEcolor(i, j, G2D_MAGENTA);
		}	
	G2D_SetFontColor(G2D_WHITE, G2D_BLACK, bIsTransparent);	
	G2D_Printf(uG2dWidth/2-1, uG2dHeight/2-1, "FLIP");
	UART_Printf("8x8 font is dispayed using color expansion, press any key to continue.\n");
	UART_Getc();

	G2D_SetRotationMode(G2D_X_FLIP);
	G2D_SetRotationOrg((u16)(uG2dWidth/2-1), (u16)(uG2dHeight/2-1));
	G2D_SetFontColor(G2D_RED, G2D_BLACK, bIsTransparent);	
	G2D_Printf(uG2dWidth/2-1, uG2dHeight/2-1, "FLIP");
	UART_Printf("X-flip using color expansion, press any key to continue.\n");
	UART_Getc();

	G2D_SetRotationMode(G2D_Y_FLIP);
	G2D_SetRotationOrg((u16)(uG2dWidth/2-1), (u16)(uG2dHeight/2-1));
	G2D_SetFontColor(G2D_BLUE, G2D_BLACK, bIsTransparent);	
	G2D_Printf(uG2dWidth/2-1, uG2dHeight/2-1, "FLIP");
	UART_Printf("Y-flip using color expansion, press any key to continue.\n");
	UART_Getc();


	// Screen to Screen Test
	UART_Printf("Simple Screen to Screen Color Expansion, press any key to continue");
	G2D_SetRotationMode(G2D_ROTATION_0_DEG_BIT);
	G2D_ClearFrameEcolor(G2D_BLACK, 0, 0);
	for(j=0; j<uG2dHeight; j++)
		for(i=0; i<uG2dWidth; i++) {
			if(i==(uG2dWidth/2-1)) G2D_PutPixelEcolor(i, j, G2D_MAGENTA);
			if(j==(uG2dHeight/2-1)) G2D_PutPixelEcolor(i, j, G2D_MAGENTA);
		}	
	UART_Getc();
	
	uTextDrawingFbAddr=(uG2dFbAddr&0xff000000)+0x800000;
	G2D_SetColorExpansionMethod(SCREEN2SCREEN);	
	G2D_FillMemoryForTextDrawing(FONT8BY8, uTextDrawingFbAddr);
	G2D_SetFontAddr(uTextDrawingFbAddr);
	G2D_SetFontType(FONT8BY8, &uFontWidth, &uFontHeight);	
	G2D_SetRotationMode(G2D_ROTATION_0_DEG_BIT);

	G2D_SetFontColor(G2D_WHITE, G2D_BLACK, bIsTransparent);	
	G2D_Printf(uG2dWidth/2-1, uG2dHeight/2-1, "ROTATION");
	UART_Printf("8x8 font is dispayed using color expansion, press any key to continue.\n");
	UART_Getc();

	G2D_SetFontColor(G2D_RED, G2D_BLACK, bIsTransparent);	
	G2D_SetRotationMode(G2D_ROTATION_90_DEG_BIT);
	G2D_SetRotationOrg((u16)(uG2dWidth/2-1), (u16)(uG2dHeight/2-1));
	G2D_Printf(uG2dWidth/2-1, uG2dHeight/2-1, "ROTATION");
	UART_Printf("90 degree rotation using color expansion, press any key to continue.\n");
	UART_Getc();

	G2D_SetFontColor(G2D_GREEN, G2D_BLACK, bIsTransparent);	
	G2D_SetRotationMode(G2D_ROTATION_180_DEG_BIT);
	G2D_SetRotationOrg((u16)(uG2dWidth/2-1), (u16)(uG2dHeight/2-1));
	G2D_Printf(uG2dWidth/2-1, uG2dHeight/2-1, "ROTATION");
	UART_Printf("180 degree rotation using color expansion, press any key to continue.\n");
	UART_Getc();

	G2D_SetFontColor(G2D_BLUE, G2D_BLACK, bIsTransparent);	
	G2D_SetRotationMode(G2D_ROTATION_270_DEG_BIT);
	G2D_SetRotationOrg((u16)(uG2dWidth/2-1), (u16)(uG2dHeight/2-1));	
	G2D_Printf(uG2dWidth/2-1, uG2dHeight/2-1, "ROTATION");
	UART_Printf("270 degree rotation using color expansion, press any key to continue.\n");
	UART_Getc();	

	G2D_SetRotationMode(G2D_ROTATION_0_DEG_BIT);
	G2D_ClearFrameEcolor(G2D_BLACK, 0, 0);
	for(j=0; j<uG2dHeight; j++)
		for(i=0; i<uG2dWidth; i++) {
			if(i==(uG2dWidth/2-1)) G2D_PutPixelEcolor(i, j, G2D_MAGENTA);
			if(j==(uG2dHeight/2-1)) G2D_PutPixelEcolor(i, j, G2D_MAGENTA);
		}	
	G2D_SetFontColor(G2D_WHITE, G2D_BLACK, bIsTransparent);	
	G2D_Printf(uG2dWidth/2-1, uG2dHeight/2-1, "FLIP");
	UART_Printf("8x8 font is dispayed using color expansion, press any key to continue.\n");
	UART_Getc();

	G2D_SetRotationMode(G2D_X_FLIP);
	G2D_SetRotationOrg((u16)(uG2dWidth/2-1), (u16)(uG2dHeight/2-1));
	G2D_SetFontColor(G2D_RED, G2D_BLACK, bIsTransparent);	
	G2D_Printf(uG2dWidth/2-1, uG2dHeight/2-1, "FLIP");
	UART_Printf("X-flip using color expansion, press any key to continue.\n");
	UART_Getc();

	G2D_SetRotationMode(G2D_Y_FLIP);
	G2D_SetRotationOrg((u16)(uG2dWidth/2-1), (u16)(uG2dHeight/2-1));
	G2D_SetFontColor(G2D_BLUE, G2D_BLACK, bIsTransparent);	
	G2D_Printf(uG2dWidth/2-1, uG2dHeight/2-1, "FLIP");
	UART_Printf("Y-flip using color expansion, press any key to exit.\n");
	UART_Getc(); 


#ifdef INTERRUPT_MODE	
	G2D_InterruptDisable();
	INTC_Disable(NUM_2D);
#endif
	
	G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);
  
}


void TestColorExpansionWithBlending(void) //Not Implemented yet
{
	u32 uG2dFbAddr, uG2dWidth, uG2dHeight;
	u32 uFontWidth, uFontHeight;
	u32 uTextDrawingFbAddr;
	s32  sAlphaVal;
	CSPACE eBpp;
	

	u8 bIsTransparent=1;

#ifdef INTERRUPT_MODE
	INTC_SetVectAddr(NUM_2D, Isr_G2d);
	INTC_Enable(NUM_2D);
	G2D_InterruptEnable();
#endif


	G2D_GetFrmSz(&uG2dWidth, &uG2dHeight);
	G2D_GetFbAddr(&uG2dFbAddr);
	G2D_GetSrcBppMode(&eBpp);
	UART_Printf("Width=%d, Height=%d, Framebuffer:0x%x\n", uG2dWidth, uG2dHeight, uG2dFbAddr);

	G2D_ClearFrameEcolor(G2D_YELLOW, 0, 0);

	// Host to Screen Test
	UART_Printf("Host to Screen Color Expansion with Blending, press any key to continue!\n");	
	G2D_SetColorExpansionMethod(HOST2SCREEN);
	G2D_SetFontType(FONT8BY8, &uFontWidth, &uFontHeight);	

	G2D_SetAlphaMode(G2D_ALPHA_MODE);
	G2D_SetAlphaValue(0xff);
	
	G2D_SetFontColor(G2D_RED, G2D_BLACK, bIsTransparent);	
	G2D_Printf(5, 5, "[2D]H2S Color Expansion");
	G2D_SetFontColor(G2D_GREEN, G2D_BLACK, bIsTransparent); 
	G2D_Printf(5, uG2dHeight*1/4+5, "[2D]H2S Color Expansion");
	G2D_SetFontColor(G2D_BLUE, G2D_BLACK, bIsTransparent);	
	G2D_Printf(5, uG2dHeight*2/4+5, "[2D]H2S Color Expansion");
	G2D_SetFontColor(G2D_WHITE, G2D_BLACK, bIsTransparent); 
	G2D_Printf(5, uG2dHeight*3/4+5, "[2D]H2S Color Expansion");
	UART_Getc();
	
	sAlphaVal=255;
	while(1) {
		sAlphaVal -= 8;
		if(sAlphaVal<=0) sAlphaVal=0;
		G2D_SetAlphaValue((u8)sAlphaVal);
		G2D_ClearFrameEcolor(G2D_YELLOW, 0, 0);

		G2D_SetFontColor(G2D_RED, G2D_BLACK, bIsTransparent);	
		G2D_Printf(5, 5, "[2D]H2S Color Expansion");
		G2D_SetFontColor(G2D_GREEN, G2D_BLACK, bIsTransparent); 
		G2D_Printf(5, uG2dHeight*1/4+5, "[2D]H2S Color Expansion");
		G2D_SetFontColor(G2D_BLUE, G2D_BLACK, bIsTransparent);	
		G2D_Printf(5, uG2dHeight*2/4+5, "[2D]H2S Color Expansion");
		G2D_SetFontColor(G2D_WHITE, G2D_BLACK, bIsTransparent); 
		G2D_Printf(5, uG2dHeight*3/4+5, "[2D]H2S Color Expansion");

		UART_Printf("Alpha:%d\n", sAlphaVal);					
		if(sAlphaVal==0) break;
		UART_Getc();
	}	
	

	UART_Printf("press any key to continue!\n");
	UART_Getc();
	G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);
	G2D_ClearFrameEcolor(G2D_YELLOW, 0, 0);


	// Screen to Screen Test	
	UART_Printf("Screen to Screen Color Expansion with Blending, press any key to continue!\n");	
	G2D_SetColorExpansionMethod(SCREEN2SCREEN);
	G2D_SetFontType(FONT8BY8, &uFontWidth, &uFontHeight);	
	uTextDrawingFbAddr=(uG2dFbAddr&0xff000000)+0x800000;
	G2D_FillMemoryForTextDrawing(FONT8BY8, uTextDrawingFbAddr);
	G2D_SetFontAddr(uTextDrawingFbAddr);

	G2D_SetAlphaMode(G2D_ALPHA_MODE);
	G2D_SetAlphaValue(0xff);

	G2D_SetFontColor(G2D_RED, G2D_BLACK, bIsTransparent);	
	G2D_Printf(5, 5, "[2D]S2S Color Expansion");
	G2D_SetFontColor(G2D_GREEN, G2D_BLACK, bIsTransparent); 
	G2D_Printf(5, uG2dHeight*1/4+5, "[2D]S2S Color Expansion");
	G2D_SetFontColor(G2D_BLUE, G2D_BLACK, bIsTransparent);	
	G2D_Printf(5, uG2dHeight*2/4+5, "[2D]S2S Color Expansion");
	G2D_SetFontColor(G2D_WHITE, G2D_BLACK, bIsTransparent); 
	G2D_Printf(5, uG2dHeight*3/4+5, "[2D]S2S Color Expansion");

	UART_Getc();
	
	sAlphaVal=255;
	while(1) {
		sAlphaVal -= 8;
		if(sAlphaVal<=0) sAlphaVal=0;
		G2D_SetAlphaValue((u8)sAlphaVal);
		G2D_ClearFrameEcolor(G2D_YELLOW, 0, 0);

		G2D_SetFontColor(G2D_RED, G2D_BLACK, bIsTransparent);	
		G2D_Printf(5, 5, "[2D]S2S Color Expansion");
		G2D_SetFontColor(G2D_GREEN, G2D_BLACK, bIsTransparent); 
		G2D_Printf(5, uG2dHeight*1/4+5, "[2D]S2S Color Expansion");
		G2D_SetFontColor(G2D_BLUE, G2D_BLACK, bIsTransparent);	
		G2D_Printf(5, uG2dHeight*2/4+5, "[2D]S2S Color Expansion");
		G2D_SetFontColor(G2D_WHITE, G2D_BLACK, bIsTransparent); 
		G2D_Printf(5, uG2dHeight*3/4+5, "[2D]S2S Color Expansion");

		UART_Printf("Alpha:%d\n", sAlphaVal);					
		if(sAlphaVal==0) break;
		UART_Getc();
	}



#ifdef INTERRUPT_MODE	
	G2D_InterruptDisable();
	INTC_Disable(NUM_2D);
#endif

	G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);
}


void TestColorExpansionWithClipping(void) 
{
	u32 uG2dFbAddr, uG2dWidth, uG2dHeight;
	u32 uFontWidth, uFontHeight;
	u32 uTextDrawingFbAddr;
	u32 uDec;
	CSPACE eBpp;
	

	u8 bIsTransparent=1;

#ifdef INTERRUPT_MODE
	INTC_SetVectAddr(NUM_2D, Isr_G2d);
	INTC_Enable(NUM_2D);
	G2D_InterruptEnable();
#endif

	G2D_GetFrmSz(&uG2dWidth, &uG2dHeight);
	G2D_GetFbAddr(&uG2dFbAddr);
	G2D_GetSrcBppMode(&eBpp);
	UART_Printf("Width=%d, Height=%d, Framebuffer:0x%x\n", uG2dWidth, uG2dHeight, uG2dFbAddr);

	G2D_ClearFrameEcolor(G2D_BLACK, 0, 0);

	// Host to Screen Test
	UART_Printf("Host to Screen Color Expansion with Clipping, press any key to continue!\n");	
	G2D_SetColorExpansionMethod(HOST2SCREEN);
	G2D_SetFontType(FONT8BY8, &uFontWidth, &uFontHeight);	
	
	G2D_SetFontColor(G2D_RED, G2D_BLACK, bIsTransparent);	
	G2D_Printf(0, 5, "[2D]H2S Color Expansion");
	G2D_SetFontColor(G2D_GREEN, G2D_BLACK, bIsTransparent); 
	G2D_Printf(0, uG2dHeight*1/4+5, "[2D]H2S Color Expansion");
	G2D_SetFontColor(G2D_BLUE, G2D_BLACK, bIsTransparent);	
	G2D_Printf(0, uG2dHeight*2/4+5, "[2D]H2S Color Expansion");
	G2D_SetFontColor(G2D_WHITE, G2D_BLACK, bIsTransparent); 
	G2D_Printf(0, uG2dHeight*3/4+5, "[2D]H2S Color Expansion");
	UART_Getc();
	
	uDec=0;
	while(1) {
		uDec+=8;
		if((uG2dWidth-2*uDec <=32)||(uG2dHeight-2*uDec <=32)) break;
		G2D_ClearFrameEcolor(G2D_BLACK, 0, 0);
		G2D_SetClippingWindow(0+uDec, 0+uDec, uG2dWidth-uDec, uG2dHeight-uDec);
		UART_Printf("[Clipping]x1=%d,y1=%d,x2=%d,y2=%d\n",uG2dWidth-uDec, uG2dHeight-uDec, uG2dWidth-uDec, uG2dHeight-uDec);
		G2D_SetFontColor(G2D_RED, G2D_BLACK, bIsTransparent);	
		G2D_Printf(0, 5, "[2D]Host to Screen Color Expansion Test");
		G2D_SetFontColor(G2D_GREEN, G2D_BLACK, bIsTransparent); 
		G2D_Printf(0, uG2dHeight*1/4+5, "[2D]Host to Screen Color Expansion Test");
		G2D_SetFontColor(G2D_BLUE, G2D_BLACK, bIsTransparent);	
		G2D_Printf(0, uG2dHeight*2/4+5, "[2D]Host to Screen Color Expansion Test");
		G2D_SetFontColor(G2D_WHITE, G2D_BLACK, bIsTransparent); 
		G2D_Printf(0, uG2dHeight*3/4+5, "[2D]Host to Screen Color Expansion Test");
		UART_Getc();		
	}


	G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);
	G2D_ClearFrameEcolor(G2D_BLACK, 0, 0);


	// Screen to Screen Test	
	UART_Printf("Screen to Screen Color Expansion with ROP, press any key to continue!\n");	
	G2D_SetColorExpansionMethod(SCREEN2SCREEN);
	G2D_SetFontType(FONT8BY8, &uFontWidth, &uFontHeight);	
	uTextDrawingFbAddr=(uG2dFbAddr&0xff000000)+0x800000;
	G2D_FillMemoryForTextDrawing(FONT8BY8, uTextDrawingFbAddr);
	G2D_SetFontAddr(uTextDrawingFbAddr);

	G2D_SetFontColor(G2D_RED, G2D_BLACK, bIsTransparent);	
	G2D_Printf(0, 5, "[2D]S2S Color Expansion");
	G2D_SetFontColor(G2D_GREEN, G2D_BLACK, bIsTransparent); 
	G2D_Printf(0, uG2dHeight*1/4+5, "[2D]S2S Color Expansion");
	G2D_SetFontColor(G2D_BLUE, G2D_BLACK, bIsTransparent);	
	G2D_Printf(0, uG2dHeight*2/4+5, "[2D]S2S Color Expansion");
	G2D_SetFontColor(G2D_WHITE, G2D_BLACK, bIsTransparent); 
	G2D_Printf(0, uG2dHeight*3/4+5, "[2D]S2S Color Expansion");
	UART_Getc();
	
	uDec=0;
	while(1) {
		uDec+=8;
		if((uG2dWidth-2*uDec <=32)||(uG2dHeight-2*uDec <=32)) break;
		G2D_ClearFrameEcolor(G2D_BLACK, 0, 0);
		G2D_SetClippingWindow(0+uDec, 0+uDec, uG2dWidth-uDec, uG2dHeight-uDec);
		UART_Printf("[Clipping]x1=%d,y1=%d,x2=%d,y2=%d\n",uG2dWidth-uDec, uG2dHeight-uDec, uG2dWidth-uDec, uG2dHeight-uDec);
		G2D_SetFontColor(G2D_RED, G2D_BLACK, bIsTransparent);	
		G2D_Printf(0, 5, "[2D]S2S Color Expansion");
		G2D_SetFontColor(G2D_GREEN, G2D_BLACK, bIsTransparent); 
		G2D_Printf(0, uG2dHeight*1/4+5, "[2D]S2S Color Expansion");
		G2D_SetFontColor(G2D_BLUE, G2D_BLACK, bIsTransparent);	
		G2D_Printf(0, uG2dHeight*2/4+5, "[2D]S2S Color Expansion");
		G2D_SetFontColor(G2D_WHITE, G2D_BLACK, bIsTransparent); 
		G2D_Printf(0, uG2dHeight*3/4+5, "[2D]S2S Color Expansion");
		UART_Getc();		
	}

#ifdef INTERRUPT_MODE	
	G2D_InterruptDisable();
	INTC_Disable(NUM_2D);
#endif

	G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);
}



void TestColorExpansionTransparency(void)
{
	u32 uG2dFbAddr, uG2dWidth, uG2dHeight;
	u32 uFontWidth, uFontHeight;
	u32 uTextDrawingFbAddr;
	CSPACE eBpp;
	

	u8 bIsTransparent=1;
	
#ifdef INTERRUPT_MODE
	INTC_SetVectAddr(NUM_2D, Isr_G2d);
	INTC_Enable(NUM_2D);
	G2D_InterruptEnable();
#endif


	
	G2D_GetFrmSz(&uG2dWidth, &uG2dHeight);
	G2D_GetFbAddr(&uG2dFbAddr);
	G2D_GetSrcBppMode(&eBpp);
	UART_Printf("Width=%d, Height=%d, Framebuffer:0x%x\n", uG2dWidth, uG2dHeight, uG2dFbAddr);

	// Host to Screen Test
	UART_Printf("Simple Host to Screen Color Expansion, press any key to continue");
	G2D_ClearFrameEcolor(G2D_BLACK, 0, 0);
	UART_Getc();
	G2D_SetColorExpansionMethod(HOST2SCREEN);

	bIsTransparent=1;
	G2D_SetFontColor(G2D_RED, G2D_BLUE, bIsTransparent);	
	G2D_SetFontType(FONT8BY8, &uFontWidth, &uFontHeight);	
	G2D_Printf(5, 5, "[2D]H2S Color Expansion");

	bIsTransparent=0;
	G2D_SetFontColor(G2D_GREEN, G2D_MAGENTA, bIsTransparent);	
	G2D_SetFontType(FONT8BY15, &uFontWidth, &uFontHeight);	
	G2D_Printf(5, uG2dHeight*1/4+5, "[2D]H2S Color Expansion");

	bIsTransparent=1;
	G2D_SetFontColor(G2D_BLUE, G2D_WHITE, bIsTransparent);	
	G2D_SetFontType(FONT8BY16T, &uFontWidth, &uFontHeight);	
	G2D_Printf(5, uG2dHeight*2/4+5, "[2D]H2S Color Expansion");

	bIsTransparent=0;
	G2D_SetFontColor(G2D_WHITE, G2D_CYAN, bIsTransparent);	
	G2D_SetFontType(FONT8BY16, &uFontWidth, &uFontHeight);	
	G2D_Printf(5, uG2dHeight*3/4+5, "[2D]H2S Color Expansion");
	UART_Getc();	



	// Screen to Screen Test
	UART_Printf("Simple Screen to Screen Color Expansion, press any key to continue");
	G2D_ClearFrameEcolor(G2D_BLACK, 0, 0);
	UART_Getc();
	
	uTextDrawingFbAddr=(uG2dFbAddr&0xff000000)+0x800000;
	G2D_SetColorExpansionMethod(SCREEN2SCREEN);
	G2D_SetFontAddr(uTextDrawingFbAddr);
	
	bIsTransparent=1;
	G2D_SetFontColor(G2D_RED, G2D_BLUE, bIsTransparent);	
	G2D_FillMemoryForTextDrawing(FONT8BY8, uTextDrawingFbAddr);
	G2D_SetFontType(FONT8BY8, &uFontWidth, &uFontHeight);	
	G2D_Printf(5, 5, "[2D]S2S Color Expansion");
	
	bIsTransparent=0;	
	G2D_SetFontColor(G2D_GREEN, G2D_MAGENTA, bIsTransparent);	
	G2D_FillMemoryForTextDrawing(FONT8BY15, uTextDrawingFbAddr);
	G2D_SetFontType(FONT8BY15, &uFontWidth, &uFontHeight);	
	G2D_Printf(5, uG2dHeight*1/4+5, "[2D]S2S Color Expansion");

	bIsTransparent=1;
	G2D_SetFontColor(G2D_BLUE, G2D_WHITE, bIsTransparent);	
	G2D_FillMemoryForTextDrawing(FONT8BY16T, uTextDrawingFbAddr);
	G2D_SetFontType(FONT8BY16T, &uFontWidth, &uFontHeight);	
	G2D_Printf(5, uG2dHeight*2/4+5, "[2D]S2S Color Expansion");	

	bIsTransparent=0;
	G2D_SetFontColor(G2D_WHITE, G2D_CYAN, bIsTransparent);	
	G2D_FillMemoryForTextDrawing(FONT8BY16, uTextDrawingFbAddr);
	G2D_SetFontType(FONT8BY16, &uFontWidth, &uFontHeight);	
	G2D_Printf(5, uG2dHeight*3/4+5, "[2D]S2S Color Expansion");
	
#ifdef INTERRUPT_MODE	
	G2D_InterruptDisable();
	INTC_Disable(NUM_2D);
#endif

	G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);
  

}


void TestColorExpansionWithAll(void)
{
	u32 i,j;
	u32 uG2dFbAddr, uG2dWidth, uG2dHeight;
	u32 uFontWidth, uFontHeight;
	u32 uTextDrawingFbAddr;
	CSPACE eBpp;
	

	u8 bIsTransparent=1;

#ifdef INTERRUPT_MODE
	INTC_SetVectAddr(NUM_2D, Isr_G2d);
	INTC_Enable(NUM_2D);
	G2D_InterruptEnable();
#endif


	G2D_GetFrmSz(&uG2dWidth, &uG2dHeight);
	G2D_GetFbAddr(&uG2dFbAddr);
	G2D_GetSrcBppMode(&eBpp);
	UART_Printf("Width=%d, Height=%d, Framebuffer:0x%x\n", uG2dWidth, uG2dHeight, uG2dFbAddr);

	UART_Printf("Point Drawing with All Operations, press any key to test.\n");
	G2D_ClearFrameEcolor(G2D_YELLOW, 0, 0);	
	G2D_RegisterPattern(pat8x8_rgbw);
	UART_Getc();


	G2D_SetAlphaMode(G2D_ALPHA_MODE);
	G2D_SetAlphaValue(0xcf);
	G2D_SetClippingWindow(uG2dWidth/4, uG2dHeight/4,uG2dWidth*3/4, uG2dHeight*3/4);
	G2D_Set3rdOperand(G2D_OPERAND3_PAT);
	G2D_SetRopEtype(ROP_SRC_XOR_3RD_OPRND);
	
	G2D_ClearFrameEcolor(G2D_YELLOW, 0, 0);

	// Host to Screen Test
	UART_Printf("Host to Screen Color Expansion with All Operations, press any key to continue!\n");	
	G2D_SetColorExpansionMethod(HOST2SCREEN);
	G2D_SetFontType(FONT8BY8, &uFontWidth, &uFontHeight);	
	
	G2D_SetRotationMode(G2D_ROTATION_0_DEG_BIT);
	G2D_SetFontColor(G2D_WHITE, G2D_BLACK, bIsTransparent);	
	G2D_Printf(uG2dWidth/2-1, uG2dHeight/2-1, "ROTATION TEST ALL");

	G2D_SetRotationMode(G2D_ROTATION_90_DEG_BIT);
	G2D_SetRotationOrg((u16)(uG2dWidth/2-1), (u16)(uG2dHeight/2-1));
	G2D_SetFontColor(G2D_RED, G2D_BLACK, bIsTransparent);	
	G2D_Printf(uG2dWidth/2-1, uG2dHeight/2-1, "ROTATION TEST ALL");

	G2D_SetRotationMode(G2D_ROTATION_180_DEG_BIT);
	G2D_SetRotationOrg((u16)(uG2dWidth/2-1), (u16)(uG2dHeight/2-1));
	G2D_SetFontColor(G2D_GREEN, G2D_BLACK, bIsTransparent);	
	G2D_Printf(uG2dWidth/2-1, uG2dHeight/2-1, "ROTATION TEST ALL");

	G2D_SetRotationMode(G2D_ROTATION_270_DEG_BIT);
	G2D_SetRotationOrg((u16)(uG2dWidth/2-1), (u16)(uG2dHeight/2-1));
	G2D_SetFontColor(G2D_BLUE, G2D_MAGENTA, 0);	
	G2D_Printf(uG2dWidth/2-1, uG2dHeight/2-1, "ROTATION TEST ALL");

	G2D_SetRotationMode(G2D_ROTATION_0_DEG_BIT);
	for(j=0; j<uG2dHeight; j++)
		for(i=0; i<uG2dWidth; i++) {
			if(i==(uG2dWidth/2-1)) G2D_PutPixelEcolor(i, j, G2D_MAGENTA);
			if(j==(uG2dHeight/2-1)) G2D_PutPixelEcolor(i, j, G2D_MAGENTA);
		}	
	G2D_SetFontColor(G2D_WHITE, G2D_BLACK, bIsTransparent);	
	G2D_Printf(uG2dWidth/2-1, uG2dHeight/2-1, "FLIP");

	G2D_SetRotationMode(G2D_X_FLIP);
	G2D_SetRotationOrg((u16)(uG2dWidth/2-1), (u16)(uG2dHeight/2-1));
	G2D_SetFontColor(G2D_RED, G2D_BLACK, bIsTransparent);	
	G2D_Printf(uG2dWidth/2-1, uG2dHeight/2-1, "FLIP");

	G2D_SetRotationMode(G2D_Y_FLIP);
	G2D_SetRotationOrg((u16)(uG2dWidth/2-1), (u16)(uG2dHeight/2-1));
	G2D_SetFontColor(G2D_BLUE, G2D_MAGENTA, 0);	
	G2D_Printf(uG2dWidth/2-1, uG2dHeight/2-1, "FLIP");

	UART_Printf("Press any key to continue.\n");
	UART_Getc();	

	// Screen to Screen Test	
	UART_Printf("Screen to Screen Color Expansion with ROP, press any key to continue!\n");		
	G2D_SetColorExpansionMethod(SCREEN2SCREEN);
	G2D_SetFontType(FONT8BY8, &uFontWidth, &uFontHeight);	
	uTextDrawingFbAddr=(uG2dFbAddr&0xff000000)+0x800000;
	G2D_FillMemoryForTextDrawing(FONT8BY8, uTextDrawingFbAddr);
	G2D_SetFontAddr(uTextDrawingFbAddr);
	G2D_ClearFrameEcolor(G2D_YELLOW, 0, 0);
	UART_Getc();

	G2D_Set3rdOperand(G2D_OPERAND3_PAT);
	G2D_SetRopEtype(ROP_SRC_XOR_3RD_OPRND);

	G2D_SetRotationMode(G2D_ROTATION_0_DEG_BIT);
	G2D_SetFontColor(G2D_WHITE, G2D_BLACK, bIsTransparent);	
	G2D_Printf(uG2dWidth/2-1, uG2dHeight/2-1, "ROTATION TEST ALL");

	G2D_SetRotationMode(G2D_ROTATION_90_DEG_BIT);
	G2D_SetRotationOrg((u16)(uG2dWidth/2-1), (u16)(uG2dHeight/2-1));
	G2D_SetFontColor(G2D_RED, G2D_BLACK, bIsTransparent);	
	G2D_Printf(uG2dWidth/2-1, uG2dHeight/2-1, "ROTATION TEST ALL");

	G2D_SetRotationMode(G2D_ROTATION_180_DEG_BIT);
	G2D_SetRotationOrg((u16)(uG2dWidth/2-1), (u16)(uG2dHeight/2-1));
	G2D_SetFontColor(G2D_GREEN, G2D_BLACK, bIsTransparent);	
	G2D_Printf(uG2dWidth/2-1, uG2dHeight/2-1, "ROTATION TEST ALL");

	G2D_SetRotationMode(G2D_ROTATION_270_DEG_BIT);
	G2D_SetRotationOrg((u16)(uG2dWidth/2-1), (u16)(uG2dHeight/2-1));
	G2D_SetFontColor(G2D_BLUE, G2D_BLACK, bIsTransparent);	
	G2D_Printf(uG2dWidth/2-1, uG2dHeight/2-1, "ROTATION TEST ALL");

	G2D_SetRotationMode(G2D_ROTATION_0_DEG_BIT);
	for(j=0; j<uG2dHeight; j++)
		for(i=0; i<uG2dWidth; i++) {
			if(i==(uG2dWidth/2-1)) G2D_PutPixelEcolor(i, j, G2D_MAGENTA);
			if(j==(uG2dHeight/2-1)) G2D_PutPixelEcolor(i, j, G2D_MAGENTA);
		}	
	G2D_SetFontColor(G2D_WHITE, G2D_BLACK, bIsTransparent);	
	G2D_Printf(uG2dWidth/2-1, uG2dHeight/2-1, "FLIP");

	G2D_SetRotationMode(G2D_X_FLIP);
	G2D_SetRotationOrg((u16)(uG2dWidth/2-1), (u16)(uG2dHeight/2-1));
	G2D_SetFontColor(G2D_RED, G2D_BLACK, bIsTransparent);	
	G2D_Printf(uG2dWidth/2-1, uG2dHeight/2-1, "FLIP");

	G2D_SetRotationMode(G2D_Y_FLIP);
	G2D_SetRotationOrg((u16)(uG2dWidth/2-1), (u16)(uG2dHeight/2-1));
	G2D_SetFontColor(G2D_BLUE, G2D_BLACK, bIsTransparent);	
	G2D_Printf(uG2dWidth/2-1, uG2dHeight/2-1, "FLIP");

	UART_Printf("press any key to exit!\n");

#ifdef INTERRUPT_MODE	
	G2D_InterruptDisable();
	INTC_Disable(NUM_2D);
#endif

	G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);

}

void TestOnScrBitBLTSimple(void)
{
	u32 uG2dFbAddr, uG2dWidth, uG2dHeight;
	CSPACE eBpp;

	u8 uIsStretch=0;

#ifdef INTERRUPT_MODE
	INTC_SetVectAddr(NUM_2D, Isr_G2d);
	INTC_Enable(NUM_2D);
	G2D_InterruptEnable();
#endif
;

	G2D_GetFrmSz(&uG2dWidth, &uG2dHeight);
	G2D_GetFbAddr(&uG2dFbAddr);	
	G2D_GetSrcBppMode(&eBpp);
		
	UART_Printf("Width=%d, Height=%d, Framebuffer:0x%x\n", uG2dWidth, uG2dHeight, uG2dFbAddr);

	G2D_DrawImage(eBpp);

	UART_Printf("BitBLT Test, Press any key to continue!\n");
	UART_Getc();

	// Normal BitBLT
	UART_Printf("[Source Coordinate] x1=%d, y1=%d, x2=%d, y2=%d\n", 5, 5, 34, 34);
	UART_Printf("Press any key to test Normal BitBLT!\n");
	G2D_PutRectangleEcolor(5,5,30,30,G2D_MAGENTA) ;
	G2D_PutLineEcolor(119, 0, 119, 319, G2D_WHITE, true);
	G2D_PutLineEcolor(0, 159, 239, 159, G2D_WHITE, true);
	
	UART_Getc();
	uIsStretch=0;
	G2D_BitBlt(5, 5, 34, 34, uG2dWidth/2-1, uG2dHeight/2-1, uG2dWidth/2-1+29, uG2dHeight/2-1+29,uIsStretch);
	UART_Printf("[Destination Coordinate] x1=%d, y1=%d, x2=%d, y2=%d\n", uG2dWidth/2-1, uG2dHeight/2-1, uG2dWidth/2-1+29, uG2dHeight/2-1+29);
	UART_Printf("Press any key to continue!\n");
	UART_Getc();

	// Stretched BitBLT
	UART_Printf("Stretched BitBLT Test, Press any key to continue!\n");
	UART_Printf("[Source Coordinate] x1=%d, y1=%d, x2=%d, y2=%d\n", 5, 5, 34, 34);
	G2D_PutRectangleEcolor(5,5,30,30,G2D_CYAN) ;
	UART_Getc();
	uIsStretch=1;	
	G2D_BitBlt(5, 5, 34, 34, uG2dWidth/2-1, uG2dHeight/2-1, uG2dWidth/2-1+59, uG2dHeight/2-1+59,uIsStretch);
	UART_Printf("[Destination Coordinate] x1=%d, y1=%d, x2=%d, y2=%d\n", uG2dWidth/2-1, uG2dHeight/2-1, uG2dWidth/2-1+59, uG2dHeight/2-1+59);	
	UART_Printf("Press any key to exit!\n");
	UART_Getc();

	
#ifdef INTERRUPT_MODE	
	G2D_InterruptDisable();
	INTC_Disable(NUM_2D);
#endif

	G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);	
}


void TestOnScrBitBLTWithRop(void)
{
	u32 l;
	u32 uG2dFbAddr, uG2dWidth, uG2dHeight;
	CSPACE eBpp;

	u8 uIsStretch=0;

#ifdef INTERRUPT_MODE
	INTC_SetVectAddr(NUM_2D, Isr_G2d);
	INTC_Enable(NUM_2D);
	G2D_InterruptEnable();
#endif

	G2D_GetFrmSz(&uG2dWidth, &uG2dHeight);
	G2D_GetFbAddr(&uG2dFbAddr);	
	G2D_GetSrcBppMode(&eBpp);
		
	UART_Printf("Width=%d, Height=%d, Framebuffer:0x%x\n", uG2dWidth, uG2dHeight, uG2dFbAddr);

	G2D_DrawImage(eBpp);
	
	UART_Printf("BitBLT Test, Press any key to continue!\n");
	UART_Getc();
	
	G2D_RegisterPattern(pat8x8_rgbw);	
	G2D_Set3rdOperand(G2D_OPERAND3_PAT);
	UART_Printf("Normal BitBLT with Raster Operation, press any key to continue\n");
	UART_Getc();	

	//Normal BitBLT
	uIsStretch=0;
	for(l=0; l<=8; l++) {
		G2D_SetRopEtype((G2D_ROP_TYPE)(l));
		switch((G2D_ROP_TYPE)l){
			case ROP_DST_ONLY: 
				UART_Printf("Destination Only, press any key to continue\n");
				break;
			case ROP_SRC_ONLY: 
				UART_Printf("Source Only, press any key to continue\n");
				break;
			case ROP_3RD_OPRND_ONLY: 
				UART_Printf("Third Operand Only, press any key to continue\n");
				break;
			case ROP_SRC_AND_DST: 
				UART_Printf("Source AND Destination, press any key to continue\n");
				break;
			case ROP_SRC_AND_3RD_OPRND: 
				UART_Printf("Source AND Third Operand, press any key to continue\n");
				break;
			case ROP_SRC_OR_DST: 
				UART_Printf("Source OR Destination, press any key to continue\n");
				break;
			case ROP_SRC_OR_3RD_OPRND: 
				UART_Printf("Source OR Third Operand, press any key to continue\n");
				break;				
			case ROP_DST_OR_3RD:
				UART_Printf("Destination OR Third Operand, press any key to continue\n");
				break;
			case ROP_SRC_XOR_3RD_OPRND: 
				UART_Printf("Source XOR Third Operand, press any key to continue\n");
				break;
			default:
				break;
		} 

		G2D_PutRectangleEcolor(5,5,30,30,G2D_MAGENTA) ;
		G2D_BitBlt(5, 5, 34, 34, uG2dWidth/2-1, uG2dHeight/2-1, uG2dWidth/2-1+29, uG2dHeight/2-1+29,uIsStretch);
		UART_Getc();
		G2D_SetRopEtype(ROP_SRC_ONLY);

		G2D_DrawImage(eBpp);
	}

	G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);	
	G2D_Set3rdOperand(G2D_OPERAND3_PAT);
	UART_Printf("Stretched BitBLT with Raster Operation, press any key to continue\n");
	UART_Getc();	
	//Stretched BitBLT
	uIsStretch=1;
	for(l=0; l<=8; l++) {
		G2D_SetRopEtype((G2D_ROP_TYPE)(l));
		switch((G2D_ROP_TYPE)l){
			case ROP_DST_ONLY: 
				UART_Printf("Destination Only, press any key to continue\n");
				break;
			case ROP_SRC_ONLY: 
				UART_Printf("Source Only, press any key to continue\n");
				break;
			case ROP_3RD_OPRND_ONLY: 
				UART_Printf("Third Operand Only, press any key to continue\n");
				break;
			case ROP_SRC_AND_DST: 
				UART_Printf("Source AND Destination, press any key to continue\n");
				break;
			case ROP_SRC_AND_3RD_OPRND: 
				UART_Printf("Source AND Third Operand, press any key to continue\n");
				break;
			case ROP_SRC_OR_DST: 
				UART_Printf("Source OR Destination, press any key to continue\n");
				break;
			case ROP_SRC_OR_3RD_OPRND: 
				UART_Printf("Source OR Third Operand, press any key to continue\n");
				break;				
			case ROP_DST_OR_3RD:
				UART_Printf("Destination OR Third Operand, press any key to continue\n");
				break;
			case ROP_SRC_XOR_3RD_OPRND: 
				UART_Printf("Source XOR Third Operand, press any key to continue\n");
				break;
			default:
				break;
		} 

		G2D_PutRectangleEcolor(5,5,30,30,G2D_MAGENTA) ;
		G2D_BitBlt(5, 5, 34, 34, uG2dWidth/2-1, uG2dHeight/2-1, uG2dWidth/2-1+59, uG2dHeight/2-1+59,uIsStretch);
		UART_Getc();
		G2D_SetRopEtype(ROP_SRC_ONLY);		

		G2D_DrawImage(eBpp);
		
	}

	
#ifdef INTERRUPT_MODE	
	G2D_InterruptDisable();
	INTC_Disable(NUM_2D);
#endif

	G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);	
}



void TestOnScrBitBLTWithRotation(void)
{
	u32 uG2dFbAddr, uG2dWidth, uG2dHeight;
	CSPACE eBpp;

	u8 uIsStretch=0;

#ifdef INTERRUPT_MODE
	INTC_SetVectAddr(NUM_2D, Isr_G2d);
	INTC_Enable(NUM_2D);
	G2D_InterruptEnable();
#endif

	G2D_GetFrmSz(&uG2dWidth, &uG2dHeight);
	G2D_GetFbAddr(&uG2dFbAddr);	
	G2D_GetSrcBppMode(&eBpp);
		
	UART_Printf("Width=%d, Height=%d, Framebuffer:0x%x\n", uG2dWidth, uG2dHeight, uG2dFbAddr);

	G2D_DrawImage(eBpp);

	UART_Printf("BitBLT Test, Press any key to continue!\n");
	UART_Getc();

	// Normal BitBLT
	UART_Printf("Press any key to test Normal BitBLT!\n");
	G2D_PutRectangleEcolor(5,5,30,30,G2D_MAGENTA) ;
	UART_Getc();
	uIsStretch=0;	
	G2D_BitBlt(5, 5, 34, 34, uG2dWidth/2-1, uG2dHeight/2-1, uG2dWidth/2-1+29, uG2dHeight/2-1+29,uIsStretch);
	UART_Getc();

	UART_Printf("90 degree rotation with BitBLT, Press any key to continue!\n");
	G2D_SetRotationMode(G2D_ROTATION_90_DEG_BIT);
	G2D_SetRotationOrg((u16)(uG2dWidth/2-1), (u16)(uG2dHeight/2-1));
	G2D_BitBlt(5, 5, 34, 34, uG2dWidth/2-1, uG2dHeight/2-1, uG2dWidth/2-1+29, uG2dHeight/2-1+29,uIsStretch);
	UART_Getc();
	UART_Printf("180 degree rotation with BitBLT, Press any key to continue!\n");
	G2D_SetRotationMode(G2D_ROTATION_180_DEG_BIT);
	G2D_SetRotationOrg((u16)(uG2dWidth/2-1), (u16)(uG2dHeight/2-1));
	G2D_BitBlt(5, 5, 34, 34, uG2dWidth/2-1, uG2dHeight/2-1, uG2dWidth/2-1+29, uG2dHeight/2-1+29,uIsStretch);
	UART_Getc();
	UART_Printf("270 degree rotation with BitBLT, Press any key to continue!\n");
	G2D_SetRotationMode(G2D_ROTATION_270_DEG_BIT);
	G2D_SetRotationOrg((u16)(uG2dWidth/2-1), (u16)(uG2dHeight/2-1));
	G2D_BitBlt(5, 5, 34, 34, uG2dWidth/2-1, uG2dHeight/2-1, uG2dWidth/2-1+29, uG2dHeight/2-1+29,uIsStretch);
	UART_Getc();
	UART_Printf("Flip with BitBLT, Press any key to continue!\n");
	G2D_SetRotationMode(G2D_ROTATION_0_DEG_BIT);		
	G2D_DrawImage(eBpp);	
	G2D_PutRectangleEcolor(5,5,30,30,G2D_MAGENTA);
	G2D_BitBlt(5, 5, 34, 34, uG2dWidth/2-1, uG2dHeight/2-1, uG2dWidth/2-1+29, uG2dHeight/2-1+29,uIsStretch);
	UART_Getc();
	UART_Printf("Flip with BitBLT, Press any key to continue!\n");
	G2D_SetRotationMode(G2D_X_FLIP);	
	G2D_SetRotationOrg((u16)(uG2dWidth/2-1), (u16)(uG2dHeight/2-1));
	G2D_BitBlt(5, 5, 34, 34, uG2dWidth/2-1, uG2dHeight/2-1, uG2dWidth/2-1+29, uG2dHeight/2-1+29,uIsStretch);
	UART_Getc();
	UART_Printf("Flip with BitBLT, Press any key to continue!\n");
	G2D_SetRotationMode(G2D_Y_FLIP);	
	G2D_SetRotationOrg((u16)(uG2dWidth/2-1), (u16)(uG2dHeight/2-1));
	G2D_BitBlt(5, 5, 34, 34, uG2dWidth/2-1, uG2dHeight/2-1, uG2dWidth/2-1+29, uG2dHeight/2-1+29,uIsStretch);
	UART_Getc();	
	G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);	


	// Stretched BitBLT
	UART_Printf("Stretched BitBLT Test, Press any key to continue!\n");
	G2D_DrawImage(eBpp);	
	G2D_PutRectangleEcolor(5,5,30,30,G2D_CYAN) ;	
	UART_Getc();
	uIsStretch=1;	
	G2D_BitBlt(5, 5, 34, 34, uG2dWidth/2-1, uG2dHeight/2-1, uG2dWidth/2-1+59, uG2dHeight/2-1+59,uIsStretch);
	UART_Getc();

	UART_Printf("90 degree rotation with BitBLT, Press any key to continue!\n");
	G2D_SetRotationMode(G2D_ROTATION_90_DEG_BIT);
	G2D_SetRotationOrg((u16)(uG2dWidth/2-1), (u16)(uG2dHeight/2-1));
	G2D_BitBlt(5, 5, 34, 34, uG2dWidth/2-1, uG2dHeight/2-1, uG2dWidth/2-1+59, uG2dHeight/2-1+59,uIsStretch);
	UART_Getc();
	UART_Printf("180 degree rotation with BitBLT, Press any key to continue!\n");
	G2D_SetRotationMode(G2D_ROTATION_180_DEG_BIT);
	G2D_SetRotationOrg((u16)(uG2dWidth/2-1), (u16)(uG2dHeight/2-1));
	G2D_BitBlt(5, 5, 34, 34, uG2dWidth/2-1, uG2dHeight/2-1, uG2dWidth/2-1+59, uG2dHeight/2-1+59,uIsStretch);
	UART_Getc();
	UART_Printf("270 degree rotation with BitBLT, Press any key to continue!\n");
	G2D_SetRotationMode(G2D_ROTATION_270_DEG_BIT);
	G2D_SetRotationOrg((u16)(uG2dWidth/2-1), (u16)(uG2dHeight/2-1));
	G2D_BitBlt(5, 5, 34, 34, uG2dWidth/2-1, uG2dHeight/2-1, uG2dWidth/2-1+59, uG2dHeight/2-1+59,uIsStretch);
	UART_Getc();
	UART_Printf("Flip with BitBLT, Press any key to continue!\n");
	G2D_SetRotationMode(G2D_ROTATION_0_DEG_BIT);		
	G2D_DrawImage(eBpp);	
	G2D_PutRectangleEcolor(5,5,30,30,G2D_MAGENTA);	
	G2D_BitBlt(5, 5, 34, 34, uG2dWidth/2-1, uG2dHeight/2-1, uG2dWidth/2-1+59, uG2dHeight/2-1+59,uIsStretch);
	UART_Getc();
	UART_Printf("X-Flip with BitBLT, Press any key to continue!\n");
	G2D_SetRotationMode(G2D_X_FLIP);
	G2D_SetRotationOrg((u16)(uG2dWidth/2-1), (u16)(uG2dHeight/2-1));	
	G2D_BitBlt(5, 5, 34, 34, uG2dWidth/2-1, uG2dHeight/2-1, uG2dWidth/2-1+59, uG2dHeight/2-1+59,uIsStretch);
	UART_Getc();
	UART_Printf("Y-Flip with BitBLT, Press any key to continue!\n");
	G2D_SetRotationMode(G2D_Y_FLIP);
	G2D_SetRotationOrg((u16)(uG2dWidth/2-1), (u16)(uG2dHeight/2-1));	
	G2D_BitBlt(5, 5, 34, 34, uG2dWidth/2-1, uG2dHeight/2-1, uG2dWidth/2-1+59, uG2dHeight/2-1+59,uIsStretch);
	UART_Getc();

/*
	UART_Printf("full X-flip\n");
	G2D_SetRotationMode(G2D_ROTATION_0_DEG_BIT);
	G2D_DrawImage(eBpp);	
	GetChar();
	G2D_SetRotationMode(G2D_X_FLIP);
	G2D_SetRotationOrg(159,119);	
	G2D_BitBlt(0,0,319,239,0,0,319,239,0);
	GetChar();
	
	UART_Printf("full Y-flip\n");
	G2D_SetRotationMode(G2D_ROTATION_0_DEG_BIT);
	G2D_DrawImage(eBpp);	
	GetChar();
	G2D_SetRotationMode(G2D_Y_FLIP);
	G2D_SetRotationOrg(159,119);	
	G2D_BitBlt(0,0,319,239,0,0,319,239,0);
	GetChar();
*/
	
#ifdef INTERRUPT_MODE	
	G2D_InterruptDisable();
	INTC_Disable(NUM_2D);
#endif

	G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);	
}


void TestOnScrBitBLTWithBlending(void)
{
	u32 uG2dFbAddr, uG2dWidth, uG2dHeight;
	CSPACE eBpp;
	s32 sAlphaVal;

	u8 uIsStretch=0;

#ifdef INTERRUPT_MODE
	INTC_SetVectAddr(NUM_2D, Isr_G2d);
	INTC_Enable(NUM_2D);
	G2D_InterruptEnable();
#endif

	G2D_GetFrmSz(&uG2dWidth, &uG2dHeight);
	G2D_GetFbAddr(&uG2dFbAddr);	
	G2D_GetSrcBppMode(&eBpp);
		
	UART_Printf("Width=%d, Height=%d, Framebuffer:0x%x\n", uG2dWidth, uG2dHeight, uG2dFbAddr);
	UART_Printf("BitBLT Test, Press any key to continue!\n");
	UART_Getc();

	G2D_DrawImage(eBpp);

	// Normal BitBLT
	UART_Printf("Normal BitBLT Test with BLENDING, Press any key to continue!\n");
	uIsStretch=0;
	G2D_PutRectangleEcolor(5,5,30,30,G2D_MAGENTA) ;
	UART_Getc();
	
	G2D_SetAlphaMode(G2D_ALPHA_MODE);
	sAlphaVal=255;
	while(1) {
		sAlphaVal -= 8;
		if(sAlphaVal<=0) sAlphaVal=0;
		G2D_SetAlphaValue((u8)sAlphaVal);
		G2D_BitBlt(5, 5, 34, 34, uG2dWidth/2-1, uG2dHeight/2-1, uG2dWidth/2-1+29, uG2dHeight/2-1+29,uIsStretch);
		UART_Printf("Alpha:%d\n", sAlphaVal);					
		if(sAlphaVal==0) break;
		UART_Getc();
		G2D_SetAlphaValue(255);
		G2D_DrawImage(eBpp);		
		G2D_PutRectangleEcolor(5,5,30,30,G2D_MAGENTA) ;
	}	
	G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);	
	UART_Printf("Press any key to continue!\n");
	UART_Getc();
	
	// Stretched BitBLT
	UART_Printf("Stretched BitBLT Test with BLENDING, Press any key to continue!\n");
	uIsStretch=1;
	G2D_DrawImage(eBpp);
	G2D_PutRectangleEcolor(5,5,30,30,G2D_MAGENTA) ;
	UART_Getc();
	G2D_SetAlphaMode(G2D_ALPHA_MODE);
	sAlphaVal=255;
	while(1) {
		sAlphaVal -= 8;
		if(sAlphaVal<=0) sAlphaVal=0;
		G2D_SetAlphaValue((u8)sAlphaVal);
		G2D_BitBlt(5, 5, 34, 34, uG2dWidth/2-1, uG2dHeight/2-1, uG2dWidth/2-1+49, uG2dHeight/2-1+49,uIsStretch);
		UART_Printf("Alpha:%d\n", sAlphaVal);					
		if(sAlphaVal==0) break;
		UART_Getc();
		G2D_SetAlphaValue(255);
		G2D_DrawImage(eBpp);		
		G2D_PutRectangleEcolor(5,5,30,30,G2D_MAGENTA) ;
	}	

	UART_Printf("Press any key to exit!\n");
	UART_Getc();
	
#ifdef INTERRUPT_MODE	
	G2D_InterruptDisable();
	INTC_Disable(NUM_2D);
#endif

	G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);	
}

void TestOnScrBitBLTWithClipping(void)
{
	u32 uG2dFbAddr, uG2dWidth, uG2dHeight;
	CSPACE eBpp;
	u32 uDec;

	u8 uIsStretch=0;

#ifdef INTERRUPT_MODE
	INTC_SetVectAddr(NUM_2D, Isr_G2d);
	INTC_Enable(NUM_2D);
	G2D_InterruptEnable();
#endif

	G2D_GetFrmSz(&uG2dWidth, &uG2dHeight);
	G2D_GetFbAddr(&uG2dFbAddr);	
	G2D_GetSrcBppMode(&eBpp);
		
	UART_Printf("Width=%d, Height=%d, Framebuffer:0x%x\n", uG2dWidth, uG2dHeight, uG2dFbAddr);
	UART_Printf("BitBLT Test, Press any key to continue!\n");
	UART_Getc();

	G2D_DrawImage(eBpp);

	// Normal BitBLT
	UART_Printf("Normal BitBLT Test with Clipping, Press any key to continue!\n");
	uIsStretch=0;
	UART_Getc();
	uDec=0;
	while(1){
		uDec+=8;
		if((uG2dWidth-2*uDec <=32)||(uG2dHeight-2*uDec <=32)) break;
//		G2D_ClearFrameEcolor(G2D_BLACK, 0, 0);
		G2D_SetClippingWindow(0+uDec, 0+uDec, uG2dWidth-uDec, uG2dHeight-uDec);
		UART_Printf("[Clipping]x1=%d,y1=%d,x2=%d,y2=%d\n",uG2dWidth-uDec, uG2dHeight-uDec, uG2dWidth-uDec, uG2dHeight-uDec);
		G2D_BitBlt(0, 0,uG2dWidth/2-1,uG2dHeight/2-1,uG2dWidth/2, uG2dHeight/2,uG2dWidth-1,uG2dHeight-1,uIsStretch);
		G2D_PutRectangleEcolor(0+uDec, 0+uDec, uG2dWidth-2*uDec+1, uG2dHeight-2*uDec+1, G2D_MAGENTA);
		UART_Getc();
		G2D_DrawImage(eBpp);
		
	}
	G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);	
	UART_Printf("Press any key to continue!\n");
	UART_Getc();
	
	// Stretched BitBLT
	UART_Printf("Stretched BitBLT Test with Clipping, Press any key to continue!\n");
	uIsStretch=1;
	G2D_DrawImage(eBpp);
	UART_Getc();
	uDec=0;
	while(1){
		uDec+=8;
		if((uG2dWidth-2*uDec <=32)||(uG2dHeight-2*uDec <=32)) break;
//		G2D_ClearFrameEcolor(G2D_BLACK, 0, 0);
		G2D_SetClippingWindow(0+uDec, 0+uDec, uG2dWidth-uDec, uG2dHeight-uDec);
		UART_Printf("[Clipping]x1=%d,y1=%d,x2=%d,y2=%d\n",uG2dWidth-uDec, uG2dHeight-uDec, uG2dWidth-uDec, uG2dHeight-uDec);
		G2D_BitBlt(0, 0,uG2dWidth/4-1,uG2dHeight/4-1,uG2dWidth/2, uG2dHeight/2,uG2dWidth-1,uG2dHeight-1,uIsStretch);
		G2D_PutRectangleEcolor(0+uDec, 0+uDec, uG2dWidth-2*uDec+1, uG2dHeight-2*uDec+1, G2D_MAGENTA);
		UART_Getc();
		G2D_DrawImage(eBpp);

	}
	

	UART_Printf("Press any key to exit!\n");
	UART_Getc();
	
#ifdef INTERRUPT_MODE	
	G2D_InterruptDisable();
	INTC_Disable(NUM_2D);
#endif

	G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);	
}


void TestOnScrBitBLTWithTransparency(void)
{
	u32 i;
	u32 uG2dFbAddr, uG2dWidth, uG2dHeight;
	CSPACE eBpp;

	u8 uIsStretch=0;
	u8 bIsTransparent=0;

#ifdef INTERRUPT_MODE
	INTC_SetVectAddr(NUM_2D, Isr_G2d);
	INTC_Enable(NUM_2D);
	G2D_InterruptEnable();
#endif


	G2D_GetFrmSz(&uG2dWidth, &uG2dHeight);
	G2D_GetFbAddr(&uG2dFbAddr);	
	G2D_GetSrcBppMode(&eBpp);
		
	UART_Printf("Width=%d, Height=%d, Framebuffer:0x%x\n", uG2dWidth, uG2dHeight, uG2dFbAddr);

	G2D_DrawImage(eBpp);

	UART_Printf("BitBLT Test, Press any key to continue!\n");
	UART_Getc();

	// Normal BitBLT
	UART_Printf("Press any key to test Normal BitBLT!\n");
	for(i=0; i<=uG2dHeight/2-1; i++)
		if(i>=uG2dHeight/8 && i< uG2dHeight*3/8)
			G2D_PutLineEcolor(0,i,uG2dWidth-1,i,G2D_MAGENTA,0) ;
	G2D_PutRectangleEcolor(0, 0, uG2dWidth, uG2dHeight/2, G2D_WHITE);
	UART_Getc();	
	uIsStretch=0;
	
	UART_Printf("Opaque Mode!!\n");	
	bIsTransparent=0;
	G2D_SetTransparentMode(bIsTransparent, G2D_MAGENTA);
	G2D_BitBlt(0, 0, uG2dWidth-1, uG2dHeight/2-1, 0, uG2dHeight/2, uG2dWidth-1, uG2dHeight-1, uIsStretch);
	G2D_PutRectangleEcolor(0, uG2dHeight/2, uG2dWidth, uG2dHeight/2, G2D_RED);
	UART_Printf("Press any key to continue!\n");
	UART_Getc();

	G2D_DrawImage(eBpp);
	for(i=0; i<=uG2dHeight/2-1; i++)
		if(i>=uG2dHeight/8 && i< uG2dHeight*3/8)
			G2D_PutLineEcolor(0,i,uG2dWidth-1,i,G2D_YELLOW,0) ;
	G2D_PutRectangleEcolor(0, 0, uG2dWidth, uG2dHeight/2, G2D_WHITE);
	UART_Getc();	
	UART_Printf("Transparent Mode!!\n");
	bIsTransparent=1;
	G2D_SetTransparentMode(bIsTransparent, G2D_YELLOW);
	G2D_BitBlt(0, 0, uG2dWidth-1, uG2dHeight/2-1, 0, uG2dHeight/2, uG2dWidth-1, uG2dHeight-1, uIsStretch);
	G2D_PutRectangleEcolor(0, uG2dHeight/2, uG2dWidth, uG2dHeight/2, G2D_RED);
	UART_Printf("Press any key to continue!\n");
	UART_Getc();
	G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);	

	// Stretched BitBLT
	UART_Printf("Press any key to test Stretched BitBLT!\n");
	G2D_DrawImage(eBpp);	
	for(i=0; i<=uG2dHeight/2-1; i++)
		if(i>=uG2dHeight/8 && i< uG2dHeight*3/8)
			G2D_PutLineEcolor(0,i,uG2dWidth/2-1,i,G2D_MAGENTA,0) ;
	G2D_PutRectangleEcolor(0, 0, uG2dWidth/2, uG2dHeight/2, G2D_WHITE);
	UART_Getc();	
	uIsStretch=1;
	
	UART_Printf("Opaque Mode!!\n");	
	bIsTransparent=0;
	G2D_SetTransparentMode(bIsTransparent, G2D_MAGENTA);
	G2D_BitBlt(0, 0, uG2dWidth/2-1, uG2dHeight/2-1, 0, uG2dHeight/2, uG2dWidth-1, uG2dHeight-1, uIsStretch);
	G2D_PutRectangleEcolor(0, uG2dHeight/2, uG2dWidth, uG2dHeight/2, G2D_RED);
	UART_Printf("Press any key to continue!\n");
	UART_Getc();

	G2D_DrawImage(eBpp);
	for(i=0; i<=uG2dHeight/2-1; i++)
		if(i>=uG2dHeight/8 && i< uG2dHeight*3/8)
			G2D_PutLineEcolor(0,i,uG2dWidth/2-1,i,G2D_YELLOW,0) ;
	G2D_PutRectangleEcolor(0, 0, uG2dWidth/2, uG2dHeight/2, G2D_WHITE);
	UART_Getc();	
	UART_Printf("Transparent Mode!!\n");
	bIsTransparent=1;
	G2D_SetTransparentMode(bIsTransparent, G2D_YELLOW);
	G2D_BitBlt(0, 0, uG2dWidth/2-1, uG2dHeight/2-1, 0, uG2dHeight/2, uG2dWidth-1, uG2dHeight-1, uIsStretch);
	G2D_PutRectangleEcolor(0, uG2dHeight/2, uG2dWidth, uG2dHeight/2, G2D_RED);
	UART_Printf("Press any key to continue!\n");
	UART_Getc();
	G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);	


#ifdef INTERRUPT_MODE	
	G2D_InterruptDisable();
	INTC_Disable(NUM_2D);
#endif

	G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);	
}


void TestOnScrBitBLTWithAll(void)
{
	u32 i;
	u32 uG2dFbAddr, uG2dWidth, uG2dHeight;
	CSPACE eBpp;

	u8 uIsStretch=0;
	u8 bIsTransparent=0;

#ifdef INTERRUPT_MODE
	INTC_SetVectAddr(NUM_2D, Isr_G2d);
	INTC_Enable(NUM_2D);
	G2D_InterruptEnable();
#endif


	G2D_GetFrmSz(&uG2dWidth, &uG2dHeight);
	G2D_GetFbAddr(&uG2dFbAddr);	
	G2D_GetSrcBppMode(&eBpp);
		
	UART_Printf("Width=%d, Height=%d, Framebuffer:0x%x\n", uG2dWidth, uG2dHeight, uG2dFbAddr);

	G2D_DrawImage(eBpp);

	UART_Printf("BitBLT Test, Press any key to continue!\n");
	UART_Getc();

	// Normal BitBLT
	UART_Printf("Press any key to test Normal BitBLT!\n");
	for(i=10; i<20; i++)
		G2D_PutLineEcolor(0,i,39,i,G2D_MAGENTA,0) ;
	G2D_PutRectangleEcolor(0, 0, 39, 39, G2D_WHITE);
	UART_Getc();	
	uIsStretch=0;
	
	UART_Printf("Opaque Mode!!\n");	
	bIsTransparent=0;	
	G2D_SetAlphaMode(G2D_ALPHA_MODE);
	G2D_SetAlphaValue(0xcf);
	G2D_RegisterPattern(pat8x8_rgbw);		
	G2D_Set3rdOperand(G2D_OPERAND3_PAT);
	
	G2D_SetTransparentMode(bIsTransparent, G2D_MAGENTA);
	G2D_BitBlt(0, 0, 39, 39, uG2dWidth/2-1, uG2dHeight/2-1, uG2dWidth/2-1+39, uG2dHeight/2-1+39, uIsStretch);
//	G2D_PutRectangleEcolor(uG2dWidth/2-1, uG2dHeight/2-1, 40, 40, G2D_RED);
	UART_Printf("Press any key to continue!\n");
	G2D_SetRotationMode(G2D_ROTATION_90_DEG_BIT);
	G2D_SetRotationOrg((u16)(uG2dWidth/2-1), (u16)(uG2dHeight/2-1));
	G2D_SetTransparentMode(bIsTransparent, G2D_MAGENTA);
	G2D_BitBlt(0, 0, 39, 39, uG2dWidth/2-1, uG2dHeight/2-1, uG2dWidth/2-1+39, uG2dHeight/2-1+39, uIsStretch);
//	G2D_PutRectangleEcolor(uG2dWidth/2-1, uG2dHeight/2-1, 40, 40, G2D_RED);
	G2D_SetRopEtype(ROP_SRC_XOR_3RD_OPRND);	
	G2D_SetRotationMode(G2D_ROTATION_180_DEG_BIT);
	G2D_SetClippingWindow(uG2dWidth/2-1-20, uG2dHeight/2-1-20, uG2dWidth/2-1+20, uG2dHeight/2-1+20);
	G2D_SetRotationOrg((u16)(uG2dWidth/2-1), (u16)(uG2dHeight/2-1));
	G2D_SetTransparentMode(bIsTransparent, G2D_MAGENTA);
	G2D_BitBlt(0, 0, 39, 39, uG2dWidth/2-1, uG2dHeight/2-1, uG2dWidth/2-1+39, uG2dHeight/2-1+39, uIsStretch);
//	G2D_PutRectangleEcolor(uG2dWidth/2-1, uG2dHeight/2-1, 40, 40, G2D_RED);
	G2D_SetRopEtype(ROP_SRC_XOR_3RD_OPRND);	
	G2D_SetRotationMode(G2D_ROTATION_270_DEG_BIT);
	G2D_SetClippingWindow(uG2dWidth/2-1-20, uG2dHeight/2-1-20, uG2dWidth/2-1+20, uG2dHeight/2-1+20);
	G2D_SetRotationOrg((u16)(uG2dWidth/2-1), (u16)(uG2dHeight/2-1));
	G2D_SetTransparentMode(bIsTransparent, G2D_MAGENTA);
	G2D_BitBlt(0, 0, 39, 39, uG2dWidth/2-1, uG2dHeight/2-1, uG2dWidth/2-1+39, uG2dHeight/2-1+39, uIsStretch);
//	G2D_PutRectangleEcolor(uG2dWidth/2-1, uG2dHeight/2-1, 40, 40, G2D_RED);
	UART_Printf("Press any key to continue!\n");
	UART_Getc();	
	G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);	

	G2D_DrawImage(eBpp);
	UART_Printf("Transparent Mode!!\n");	
	for(i=10; i<20; i++)
		G2D_PutLineEcolor(0,i,39,i,G2D_MAGENTA,0) ;
	G2D_PutRectangleEcolor(0, 0, 39, 39, G2D_WHITE);
	UART_Getc();	
	
	bIsTransparent=1;	
	G2D_SetAlphaMode(G2D_ALPHA_MODE);
	G2D_SetAlphaValue(0xcf);
	G2D_RegisterPattern(pat8x8_rgbw);	
	G2D_Set3rdOperand(G2D_OPERAND3_PAT);
	
	G2D_SetTransparentMode(bIsTransparent, G2D_MAGENTA);
	G2D_BitBlt(0, 0, 39, 39, uG2dWidth/2-1, uG2dHeight/2-1, uG2dWidth/2-1+39, uG2dHeight/2-1+39, uIsStretch);

	G2D_SetRotationMode(G2D_ROTATION_90_DEG_BIT);
	G2D_SetRotationOrg((u16)(uG2dWidth/2-1), (u16)(uG2dHeight/2-1));
	G2D_SetTransparentMode(bIsTransparent, G2D_MAGENTA);
	G2D_BitBlt(0, 0, 39, 39, uG2dWidth/2-1, uG2dHeight/2-1, uG2dWidth/2-1+39, uG2dHeight/2-1+39, uIsStretch);

	G2D_SetRopEtype(ROP_SRC_XOR_3RD_OPRND);	
	G2D_SetRotationMode(G2D_ROTATION_180_DEG_BIT);
	G2D_SetClippingWindow(uG2dWidth/2-1-20, uG2dHeight/2-1-20, uG2dWidth/2-1+20, uG2dHeight/2-1+20);
	G2D_SetRotationOrg((u16)(uG2dWidth/2-1), (u16)(uG2dHeight/2-1));
	G2D_SetTransparentMode(bIsTransparent, G2D_MAGENTA);
	G2D_BitBlt(0, 0, 39, 39, uG2dWidth/2-1, uG2dHeight/2-1, uG2dWidth/2-1+39, uG2dHeight/2-1+39, uIsStretch);

	G2D_SetRopEtype(ROP_SRC_XOR_3RD_OPRND);	
	G2D_SetRotationMode(G2D_ROTATION_270_DEG_BIT);
	G2D_SetClippingWindow(uG2dWidth/2-1-20, uG2dHeight/2-1-20, uG2dWidth/2-1+20, uG2dHeight/2-1+20);
	G2D_SetRotationOrg((u16)(uG2dWidth/2-1), (u16)(uG2dHeight/2-1));
	G2D_SetTransparentMode(bIsTransparent, G2D_MAGENTA);
	G2D_BitBlt(0, 0, 39, 39, uG2dWidth/2-1, uG2dHeight/2-1, uG2dWidth/2-1+39, uG2dHeight/2-1+39, uIsStretch);
	UART_Printf("Press any key to continue!\n");
	UART_Getc();	
	G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);	 
	


	// Stretched BitBLT
	UART_Printf("Press any key to test Stretched BitBLT!\n");
	UART_Printf("Opaque Mode!!\n");	
	G2D_DrawImage(eBpp);
	for(i=10; i<20; i++)
		G2D_PutLineEcolor(0,i,39,i,G2D_MAGENTA,0) ;
	G2D_PutRectangleEcolor(0, 0, 39, 39, G2D_WHITE);
	UART_Getc();	
	uIsStretch=1;
	
	bIsTransparent=0;	
	G2D_SetAlphaMode(G2D_ALPHA_MODE);
	G2D_SetAlphaValue(0xcf);
	G2D_RegisterPattern(pat8x8_rgbw);	
	G2D_Set3rdOperand(G2D_OPERAND3_PAT);
	
	G2D_SetTransparentMode(bIsTransparent, G2D_MAGENTA);
	G2D_BitBlt(0, 0, 39, 39, uG2dWidth/2-1, uG2dHeight/2-1, uG2dWidth/2-1+79, uG2dHeight/2-1+79, uIsStretch);

	G2D_SetRotationMode(G2D_ROTATION_90_DEG_BIT);
	G2D_SetRotationOrg((u16)(uG2dWidth/2-1), (u16)(uG2dHeight/2-1));
	G2D_SetTransparentMode(bIsTransparent, G2D_MAGENTA);
	G2D_BitBlt(0, 0, 39, 39, uG2dWidth/2-1, uG2dHeight/2-1, uG2dWidth/2-1+79, uG2dHeight/2-1+79, uIsStretch);

	G2D_SetRopEtype(ROP_SRC_XOR_3RD_OPRND);	
	G2D_SetRotationMode(G2D_ROTATION_180_DEG_BIT);
	G2D_SetClippingWindow(uG2dWidth/2-1-40, uG2dHeight/2-1-40, uG2dWidth/2-1+40, uG2dHeight/2-1+40);
	G2D_SetRotationOrg((u16)(uG2dWidth/2-1), (u16)(uG2dHeight/2-1));
	G2D_SetTransparentMode(bIsTransparent, G2D_MAGENTA);
	G2D_BitBlt(0, 0, 39, 39, uG2dWidth/2-1, uG2dHeight/2-1, uG2dWidth/2-1+79, uG2dHeight/2-1+79, uIsStretch);

	G2D_SetRopEtype(ROP_SRC_XOR_3RD_OPRND);	
	G2D_SetRotationMode(G2D_ROTATION_270_DEG_BIT);
	G2D_SetClippingWindow(uG2dWidth/2-1-40, uG2dHeight/2-1-40, uG2dWidth/2-1+40, uG2dHeight/2-1+40);
	G2D_SetRotationOrg((u16)(uG2dWidth/2-1), (u16)(uG2dHeight/2-1));
	G2D_SetTransparentMode(bIsTransparent, G2D_MAGENTA);
	G2D_BitBlt(0, 0, 39, 39, uG2dWidth/2-1, uG2dHeight/2-1, uG2dWidth/2-1+79, uG2dHeight/2-1+79, uIsStretch);

	UART_Printf("Press any key to continue!\n");
	UART_Getc();	
	G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);	

	UART_Printf("Transparent Mode!!\n");	
	G2D_DrawImage(eBpp);
	for(i=10; i<20; i++)
		G2D_PutLineEcolor(0,i,39,i,G2D_MAGENTA,0) ;
	G2D_PutRectangleEcolor(0, 0, 39, 39, G2D_WHITE);
	UART_Getc();	
	uIsStretch=1;	
	bIsTransparent=1;	
	G2D_SetAlphaMode(G2D_ALPHA_MODE);
	G2D_SetAlphaValue(0xcf);
	G2D_RegisterPattern(pat8x8_rgbw);	
	G2D_Set3rdOperand(G2D_OPERAND3_PAT);
	
	G2D_SetTransparentMode(bIsTransparent, G2D_MAGENTA);
	G2D_BitBlt(0, 0, 39, 39, uG2dWidth/2-1, uG2dHeight/2-1, uG2dWidth/2-1+79, uG2dHeight/2-1+79, uIsStretch);

	G2D_SetRotationMode(G2D_ROTATION_90_DEG_BIT);
	G2D_SetRotationOrg((u16)(uG2dWidth/2-1), (u16)(uG2dHeight/2-1));
	G2D_SetTransparentMode(bIsTransparent, G2D_MAGENTA);
	G2D_BitBlt(0, 0, 39, 39, uG2dWidth/2-1, uG2dHeight/2-1, uG2dWidth/2-1+79, uG2dHeight/2-1+79, uIsStretch);

	G2D_SetRopEtype(ROP_SRC_XOR_3RD_OPRND);	
	G2D_SetRotationMode(G2D_ROTATION_180_DEG_BIT);
	G2D_SetClippingWindow(uG2dWidth/2-1-40, uG2dHeight/2-1-40, uG2dWidth/2-1+40, uG2dHeight/2-1+40);
	G2D_SetRotationOrg((u16)(uG2dWidth/2-1), (u16)(uG2dHeight/2-1));
	G2D_SetTransparentMode(bIsTransparent, G2D_MAGENTA);
	G2D_BitBlt(0, 0, 39, 39, uG2dWidth/2-1, uG2dHeight/2-1, uG2dWidth/2-1+79, uG2dHeight/2-1+79, uIsStretch);

	G2D_SetRopEtype(ROP_SRC_XOR_3RD_OPRND);	
	G2D_SetRotationMode(G2D_ROTATION_270_DEG_BIT);
	G2D_SetClippingWindow(uG2dWidth/2-1-40, uG2dHeight/2-1-40, uG2dWidth/2-1+40, uG2dHeight/2-1+40);
	G2D_SetRotationOrg((u16)(uG2dWidth/2-1), (u16)(uG2dHeight/2-1));
	G2D_SetTransparentMode(bIsTransparent, G2D_MAGENTA);
	G2D_BitBlt(0, 0, 39, 39, uG2dWidth/2-1, uG2dHeight/2-1, uG2dWidth/2-1+79, uG2dHeight/2-1+79, uIsStretch);

	UART_Printf("Press any key to continue!\n");
	UART_Getc();	
	
#ifdef INTERRUPT_MODE	
	G2D_InterruptDisable();
	INTC_Disable(NUM_2D);
#endif

	G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);	
}

void TestOffScrBitBLTSimple(void)
{
	u32 uG2dFbAddr, uG2dWidth, uG2dHeight;
	u32 uG2dOffFbAddr, uG2dOffWidth, uG2dOffHeight;
	CSPACE eBpp, eBppOff;

	u8 uIsStretch=0;

#ifdef INTERRUPT_MODE
	INTC_SetVectAddr(NUM_2D, Isr_G2d);
	INTC_Enable(NUM_2D);
	G2D_InterruptEnable();
#endif


	G2D_GetFrmSz(&uG2dWidth, &uG2dHeight);
	G2D_GetFbAddr(&uG2dFbAddr);	
	G2D_GetSrcBppMode(&eBpp);

		
	UART_Printf("Screen Size[Width=%d, Height=%d] Framebuffer:0x%x\n", uG2dWidth, uG2dHeight, uG2dFbAddr);


	
	uG2dOffFbAddr=uG2dFbAddr;
	uG2dOffWidth=uG2dWidth;
	uG2dOffHeight=uG2dHeight;
	eBppOff=eBpp;
	G2D_SetFbAddr(uG2dOffFbAddr, SRCBUF);
	G2D_SetBppMode(eBppOff, SRCBUF);	
	G2D_SetSourceSize(uG2dOffWidth, uG2dOffHeight);
	GLIB_InitInstance(uG2dOffFbAddr, uG2dOffWidth, uG2dOffHeight, eBppOff);
	GLIB_DrawPattern(uG2dOffWidth, uG2dOffHeight);	
	UART_Printf("Off Screen Image[Width:%d, Height=%d] is Displayed !\n", uG2dOffWidth, uG2dOffHeight);
	UART_Printf("Press any key to continue!\n");
	UART_Getc();

	UART_Printf("On Screen Image is Displayed!\n");
	G2D_DrawImage(eBpp);


	uG2dOffFbAddr = CODEC_MEM_ST+0x800000;
	uG2dOffWidth=uG2dWidth/2;
	uG2dOffHeight=uG2dHeight/2;
	eBppOff=RGB16;
	G2D_SetSourceSize(uG2dOffWidth, uG2dOffHeight);
	G2D_SetFbAddr(uG2dOffFbAddr, SRCBUF);
	G2D_SetBppMode(eBppOff, SRCBUF);	
	GLIB_InitInstance(uG2dOffFbAddr, uG2dOffWidth, uG2dOffHeight, eBppOff);
	GLIB_DrawPattern(uG2dOffWidth, uG2dOffHeight);	

	// Normal BitBLT
	UART_Printf("Press any key to test Normal BitBLT!\n");
	UART_Getc();
	uIsStretch=0;
	G2D_BitBlt(0, 0, uG2dOffWidth-1, uG2dOffHeight-1, 0, 0, uG2dWidth/2-1, uG2dHeight/2-1,uIsStretch);
	UART_Printf("Press any key to continue!\n");
	UART_Getc();

	// Stretched BitBLT
	UART_Printf("Stretched BitBLT Test, Press any key to continue!\n");
	UART_Getc();
	uIsStretch=1;	
	G2D_BitBlt(0, 0, uG2dOffWidth-1, uG2dOffHeight-1, 0,0, uG2dWidth-1, uG2dHeight-1,uIsStretch);

	UART_Printf("Press any key to continue!\n");
	UART_Getc();

	
#ifdef INTERRUPT_MODE	
	G2D_InterruptDisable();
	INTC_Disable(NUM_2D);
#endif

	G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);	
}

void TestOffScrBitBLTWithRop(void)
{
	u32 l;
	u32 uG2dFbAddr, uG2dWidth, uG2dHeight;
	u32 uG2dOffFbAddr, uG2dOffWidth, uG2dOffHeight;
	CSPACE eBpp, eBppOff;

	u8 uIsStretch=0;

#ifdef INTERRUPT_MODE
	INTC_SetVectAddr(NUM_2D, Isr_G2d);
	INTC_Enable(NUM_2D);
	G2D_InterruptEnable();
#endif

	G2D_GetFrmSz(&uG2dWidth, &uG2dHeight);
	G2D_GetFbAddr(&uG2dFbAddr);	
	G2D_GetSrcBppMode(&eBpp);
		
	UART_Printf("Screen Size[Width=%d, Height=%d] Framebuffer:0x%x\n", uG2dWidth, uG2dHeight, uG2dFbAddr);


	
	uG2dOffFbAddr = CODEC_MEM_ST+0x800000;
	uG2dOffWidth=uG2dWidth/2;
	uG2dOffHeight=uG2dHeight/2;
	eBppOff=eBpp;
	G2D_SetFbAddr(uG2dOffFbAddr, SRCBUF);	
	G2D_SetBppMode(eBppOff, SRCBUF);
	G2D_SetSourceSize(uG2dOffWidth, uG2dOffHeight);	
	GLIB_InitInstance(uG2dOffFbAddr, uG2dOffWidth, uG2dOffHeight, eBppOff);
	GLIB_DrawPattern(uG2dOffWidth, uG2dOffHeight);	
	UART_Printf("Press any key to continue!\n");
	UART_Getc();

	UART_Printf("On Screen Image is Displayed!\n");
	G2D_DrawImage(eBpp);
	

	G2D_RegisterPattern(pat8x8_rgbw);	
	G2D_Set3rdOperand(G2D_OPERAND3_PAT);
	UART_Printf("Normal BitBLT with Raster Operation, press any key to continue\n");
	UART_Getc();	

	//Normal BitBLT
	uIsStretch=0;
	for(l=0; l<=8; l++) {
		G2D_SetRopEtype((G2D_ROP_TYPE)(l));
		switch((G2D_ROP_TYPE)l){
			case ROP_DST_ONLY: 
				UART_Printf("Destination Only, press any key to continue\n");
				break;
			case ROP_SRC_ONLY: 
				UART_Printf("Source Only, press any key to continue\n");
				break;
			case ROP_3RD_OPRND_ONLY: 
				UART_Printf("Third Operand Only, press any key to continue\n");
				break;
			case ROP_SRC_AND_DST: 
				UART_Printf("Source AND Destination, press any key to continue\n");
				break;
			case ROP_SRC_AND_3RD_OPRND: 
				UART_Printf("Source AND Third Operand, press any key to continue\n");
				break;
			case ROP_SRC_OR_DST: 
				UART_Printf("Source OR Destination, press any key to continue\n");
				break;
			case ROP_SRC_OR_3RD_OPRND: 
				UART_Printf("Source OR Third Operand, press any key to continue\n");
				break;				
			case ROP_DST_OR_3RD:
				UART_Printf("Destination OR Third Operand, press any key to continue\n");
				break;
			case ROP_SRC_XOR_3RD_OPRND: 
				UART_Printf("Source XOR Third Operand, press any key to continue\n");
				break;
			default:
				break;
		} 

		G2D_BitBlt(0,0, uG2dOffWidth-1, uG2dOffHeight-1, 0,0, uG2dWidth/2-1, uG2dHeight/2-1,uIsStretch);
		UART_Getc();
		G2D_SetRopEtype(ROP_SRC_ONLY);

		G2D_DrawImage(eBpp);
	}

	G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);	
	G2D_Set3rdOperand(G2D_OPERAND3_PAT);
	G2D_SetFbAddr(uG2dOffFbAddr, SRCBUF);	
	G2D_SetBppMode(eBppOff, SRCBUF);
	G2D_SetSourceSize(uG2dOffWidth, uG2dOffHeight);		
	UART_Printf("Stretched BitBLT with Raster Operation, press any key to continue\n");
	UART_Getc();	
	//Stretched BitBLT
	uIsStretch=1;
	for(l=0; l<=8; l++) {
		G2D_SetRopEtype((G2D_ROP_TYPE)(l));
		switch((G2D_ROP_TYPE)l){
			case ROP_DST_ONLY: 
				UART_Printf("Destination Only, press any key to continue\n");
				break;
			case ROP_SRC_ONLY: 
				UART_Printf("Source Only, press any key to continue\n");
				break;
			case ROP_3RD_OPRND_ONLY: 
				UART_Printf("Third Operand Only, press any key to continue\n");
				break;
			case ROP_SRC_AND_DST: 
				UART_Printf("Source AND Destination, press any key to continue\n");
				break;
			case ROP_SRC_AND_3RD_OPRND: 
				UART_Printf("Source AND Third Operand, press any key to continue\n");
				break;
			case ROP_SRC_OR_DST: 
				UART_Printf("Source OR Destination, press any key to continue\n");
				break;
			case ROP_SRC_OR_3RD_OPRND: 
				UART_Printf("Source OR Third Operand, press any key to continue\n");
				break;				
			case ROP_DST_OR_3RD:
				UART_Printf("Destination OR Third Operand, press any key to continue\n");
				break;
			case ROP_SRC_XOR_3RD_OPRND: 
				UART_Printf("Source XOR Third Operand, press any key to continue\n");
				break;
			default:
				break;
		} 

		G2D_BitBlt(0, 0, uG2dOffWidth-1, uG2dOffHeight-1, 0, 0, uG2dWidth-1, uG2dHeight/2-1,uIsStretch);
		UART_Getc();
		G2D_SetRopEtype(ROP_SRC_ONLY);		

		G2D_DrawImage(eBpp);
		
	}

	
#ifdef INTERRUPT_MODE	
	G2D_InterruptDisable();
	INTC_Disable(NUM_2D);
#endif

	G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);	
}

void TestOffScrBitBLTWithRotation(void)
{
	u32 uG2dFbAddr, uG2dWidth, uG2dHeight;
	u32 uG2dOffFbAddr, uG2dOffWidth, uG2dOffHeight;
	CSPACE eBpp, eBppOff;

	u8 uIsStretch=0;

#ifdef INTERRUPT_MODE
	INTC_SetVectAddr(NUM_2D, Isr_G2d);
	INTC_Enable(NUM_2D);
	G2D_InterruptEnable();
#endif

	G2D_GetFrmSz(&uG2dWidth, &uG2dHeight);
	G2D_GetFbAddr(&uG2dFbAddr);	
	G2D_GetSrcBppMode(&eBpp);
		
	UART_Printf("Width=%d, Height=%d, Framebuffer:0x%x\n", uG2dWidth, uG2dHeight, uG2dFbAddr);

	uG2dOffFbAddr = CODEC_MEM_ST+0x800000;
	uG2dOffWidth=32;
	uG2dOffHeight=40;
	eBppOff=eBpp;
	G2D_SetFbAddr(uG2dOffFbAddr, SRCBUF);	
	G2D_SetBppMode(eBppOff, SRCBUF);
	G2D_SetSourceSize(uG2dOffWidth, uG2dOffHeight);	
	GLIB_InitInstance(uG2dOffFbAddr, uG2dOffWidth, uG2dOffHeight, eBppOff);
	GLIB_DrawPattern(uG2dOffWidth, uG2dOffHeight);	
	UART_Printf("Press any key to continue!\n");
	UART_Getc();

	G2D_DrawImage(eBpp);

	UART_Printf("BitBLT Test with Rotation, Press any key to continue!\n");
	UART_Getc();

	// Normal BitBLT
	UART_Printf("Normal BitBLT Test, Press any key to continue!\n");
	UART_Getc();
	uIsStretch=0;	
	G2D_BitBlt(0, 0, uG2dOffWidth-1, uG2dOffHeight-1, uG2dWidth/2-1, uG2dHeight/2-1, uG2dWidth/2-1+uG2dOffWidth-1, uG2dHeight/2-1+uG2dOffHeight-1,uIsStretch);
	UART_Getc();

	UART_Printf("90 degree rotation with BitBLT, Press any key to continue!\n");
	G2D_SetRotationMode(G2D_ROTATION_90_DEG_BIT);
	G2D_SetRotationOrg((u16)(uG2dWidth/2-1), (u16)(uG2dHeight/2-1));
	G2D_BitBlt(0, 0, uG2dOffWidth-1, uG2dOffHeight-1, uG2dWidth/2-1, uG2dHeight/2-1, uG2dWidth/2-1+uG2dOffWidth-1, uG2dHeight/2-1+uG2dOffHeight-1,uIsStretch);
	UART_Getc();
	UART_Printf("180 degree rotation with BitBLT, Press any key to continue!\n");
	G2D_SetRotationMode(G2D_ROTATION_180_DEG_BIT);
	G2D_SetRotationOrg((u16)(uG2dWidth/2-1), (u16)(uG2dHeight/2-1));
	G2D_BitBlt(0, 0, uG2dOffWidth-1, uG2dOffHeight-1, uG2dWidth/2-1, uG2dHeight/2-1, uG2dWidth/2-1+uG2dOffWidth-1, uG2dHeight/2-1+uG2dOffHeight-1,uIsStretch);
	UART_Getc();
	UART_Printf("270 degree rotation with BitBLT, Press any key to continue!\n");
	G2D_SetRotationMode(G2D_ROTATION_270_DEG_BIT);
	G2D_SetRotationOrg((u16)(uG2dWidth/2-1), (u16)(uG2dHeight/2-1));
	G2D_BitBlt(0, 0, uG2dOffWidth-1, uG2dOffHeight-1, uG2dWidth/2-1, uG2dHeight/2-1, uG2dWidth/2-1+uG2dOffWidth-1, uG2dHeight/2-1+uG2dOffHeight-1,uIsStretch);
	UART_Getc();
	UART_Printf("Flip with BitBLT, Press any key to continue!\n");
	G2D_SetRotationMode(G2D_ROTATION_0_DEG_BIT);		
	G2D_DrawImage(eBpp);	
	G2D_PutRectangleEcolor(5,5,30,30,G2D_MAGENTA);
	G2D_BitBlt(0, 0, uG2dOffWidth-1, uG2dOffHeight-1, uG2dWidth/2-1, uG2dHeight/2-1, uG2dWidth/2-1+uG2dOffWidth-1, uG2dHeight/2-1+uG2dOffHeight-1,uIsStretch);
	UART_Getc();
	UART_Printf("Flip with BitBLT, Press any key to continue!\n");
	G2D_SetRotationMode(G2D_X_FLIP);	
	G2D_SetRotationOrg((u16)(uG2dWidth/2-1), (u16)(uG2dHeight/2-1));
	G2D_BitBlt(0, 0, uG2dOffWidth-1, uG2dOffHeight-1, uG2dWidth/2-1, uG2dHeight/2-1, uG2dWidth/2-1+uG2dOffWidth-1, uG2dHeight/2-1+uG2dOffHeight-1,uIsStretch);
	UART_Getc();
	UART_Printf("Flip with BitBLT, Press any key to continue!\n");
	G2D_SetRotationMode(G2D_Y_FLIP);	
	G2D_SetRotationOrg((u16)(uG2dWidth/2-1), (u16)(uG2dHeight/2-1));
	G2D_BitBlt(0, 0, uG2dOffWidth-1, uG2dOffHeight-1, uG2dWidth/2-1, uG2dHeight/2-1, uG2dWidth/2-1+uG2dOffWidth-1, uG2dHeight/2-1+uG2dOffHeight-1,uIsStretch);
	UART_Getc();	
	G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);	


	// Stretched BitBLT
	G2D_SetFbAddr(uG2dOffFbAddr, SRCBUF);	
	G2D_SetBppMode(eBppOff, SRCBUF);
	G2D_SetSourceSize(uG2dOffWidth, uG2dOffHeight);		
	UART_Printf("Stretched BitBLT Test, Press any key to continue!\n");
	G2D_DrawImage(eBpp);	
	UART_Getc();
	uIsStretch=1;	
	G2D_BitBlt(0, 0, uG2dOffWidth-1, uG2dOffHeight-1, uG2dWidth/2-1, uG2dHeight/2-1, uG2dWidth/2-1+uG2dOffWidth*2-1, uG2dHeight/2-1+uG2dOffHeight*2-1,uIsStretch);
	UART_Getc();

	UART_Printf("90 degree rotation with BitBLT, Press any key to continue!\n");
	G2D_SetRotationMode(G2D_ROTATION_90_DEG_BIT);
	G2D_SetRotationOrg((u16)(uG2dWidth/2-1), (u16)(uG2dHeight/2-1));
	G2D_BitBlt(0, 0, uG2dOffWidth-1, uG2dOffHeight-1, uG2dWidth/2-1, uG2dHeight/2-1, uG2dWidth/2-1+uG2dOffWidth*2-1, uG2dHeight/2-1+uG2dOffHeight*2-1,uIsStretch);
	UART_Getc();
	UART_Printf("180 degree rotation with BitBLT, Press any key to continue!\n");
	G2D_SetRotationMode(G2D_ROTATION_180_DEG_BIT);
	G2D_SetRotationOrg((u16)(uG2dWidth/2-1), (u16)(uG2dHeight/2-1));
	G2D_BitBlt(0, 0, uG2dOffWidth-1, uG2dOffHeight-1, uG2dWidth/2-1, uG2dHeight/2-1, uG2dWidth/2-1+uG2dOffWidth*2-1, uG2dHeight/2-1+uG2dOffHeight*2-1,uIsStretch);
	UART_Getc();
	UART_Printf("270 degree rotation with BitBLT, Press any key to continue!\n");
	G2D_SetRotationMode(G2D_ROTATION_270_DEG_BIT);
	G2D_SetRotationOrg((u16)(uG2dWidth/2-1), (u16)(uG2dHeight/2-1));
	G2D_BitBlt(0, 0, uG2dOffWidth-1, uG2dOffHeight-1, uG2dWidth/2-1, uG2dHeight/2-1, uG2dWidth/2-1+uG2dOffWidth*2-1, uG2dHeight/2-1+uG2dOffHeight*2-1,uIsStretch);
	UART_Getc();
	UART_Printf("Flip with BitBLT, Press any key to continue!\n");
	G2D_SetRotationMode(G2D_ROTATION_0_DEG_BIT);		
	G2D_DrawImage(eBpp);	
	G2D_PutRectangleEcolor(5,5,30,30,G2D_MAGENTA);	
	G2D_BitBlt(0, 0, uG2dOffWidth-1, uG2dOffHeight-1, uG2dWidth/2-1, uG2dHeight/2-1, uG2dWidth/2-1+uG2dOffWidth*2-1, uG2dHeight/2-1+uG2dOffHeight*2-1,uIsStretch);
	UART_Getc();
	UART_Printf("X-Flip with BitBLT, Press any key to continue!\n");
	G2D_SetRotationMode(G2D_X_FLIP);
	G2D_SetRotationOrg((u16)(uG2dWidth/2-1), (u16)(uG2dHeight/2-1));	
	G2D_BitBlt(0, 0, uG2dOffWidth-1, uG2dOffHeight-1, uG2dWidth/2-1, uG2dHeight/2-1, uG2dWidth/2-1+uG2dOffWidth*2-1, uG2dHeight/2-1+uG2dOffHeight*2-1,uIsStretch);
	UART_Getc();
	UART_Printf("Y-Flip with BitBLT, Press any key to continue!\n");
	G2D_SetRotationMode(G2D_Y_FLIP);
	G2D_SetRotationOrg((u16)(uG2dWidth/2-1), (u16)(uG2dHeight/2-1));	
	G2D_BitBlt(0, 0, uG2dOffWidth-1, uG2dOffHeight-1, uG2dWidth/2-1, uG2dHeight/2-1, uG2dWidth/2-1+uG2dOffWidth*2-1, uG2dHeight/2-1+uG2dOffHeight*2-1,uIsStretch);
	UART_Getc();

/*
	UART_Printf("full X-flip\n");
	G2D_SetRotationMode(G2D_ROTATION_0_DEG_BIT);
	G2D_DrawImage(eBpp);	
	GetChar();
	G2D_SetRotationMode(G2D_X_FLIP);
	G2D_SetRotationOrg(159,119);	
	G2D_BitBlt(0,0,319,239,0,0,319,239,0);
	GetChar();
	
	UART_Printf("full Y-flip\n");
	G2D_SetRotationMode(G2D_ROTATION_0_DEG_BIT);
	G2D_DrawImage(eBpp);	
	GetChar();
	G2D_SetRotationMode(G2D_Y_FLIP);
	G2D_SetRotationOrg(159,119);	
	G2D_BitBlt(0,0,319,239,0,0,319,239,0);
	GetChar();
*/
	
#ifdef INTERRUPT_MODE	
	G2D_InterruptDisable();
	INTC_Disable(NUM_2D);
#endif

	G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);	
}


void TestOffScrBitBLTWithBlending(void)
{
	u32 uG2dFbAddr, uG2dWidth, uG2dHeight;
	u32 uG2dOffFbAddr, uG2dOffWidth, uG2dOffHeight;
	CSPACE eBpp, eBppOff;
	s32 sAlphaVal;

	u8 uIsStretch=0;

#ifdef INTERRUPT_MODE
	INTC_SetVectAddr(NUM_2D, Isr_G2d);
	INTC_Enable(NUM_2D);
	G2D_InterruptEnable();
#endif

	G2D_GetFrmSz(&uG2dWidth, &uG2dHeight);
	G2D_GetFbAddr(&uG2dFbAddr);	
	G2D_GetSrcBppMode(&eBpp);
		
	UART_Printf("Width=%d, Height=%d, Framebuffer:0x%x\n", uG2dWidth, uG2dHeight, uG2dFbAddr);
	UART_Printf("BitBLT Test, Press any key to continue!\n");

	uG2dOffFbAddr = CODEC_MEM_ST+0x800000;
	uG2dOffWidth=uG2dWidth/2;
	uG2dOffHeight=uG2dHeight/2;
	eBppOff=eBpp;
	G2D_SetFbAddr(uG2dOffFbAddr, SRCBUF);	
	G2D_SetBppMode(eBppOff, SRCBUF);
	G2D_SetSourceSize(uG2dOffWidth, uG2dOffHeight);	
	GLIB_InitInstance(uG2dOffFbAddr, uG2dOffWidth, uG2dOffHeight, eBppOff);
	GLIB_DrawPattern(uG2dOffWidth, uG2dOffHeight);	
	UART_Printf("Press any key to continue!\n");
	UART_Getc();
	
	G2D_DrawImage(eBpp);

	// Normal BitBLT
	UART_Printf("Normal BitBLT Test with BLENDING, Press any key to continue!\n");
	uIsStretch=0;
	UART_Getc();
	
	G2D_SetAlphaMode(G2D_ALPHA_MODE);
	sAlphaVal=255;
	while(1) {
		sAlphaVal -= 8;
		if(sAlphaVal<=0) sAlphaVal=0;
		G2D_SetAlphaValue((u8)sAlphaVal);
		G2D_BitBlt(0, 0, uG2dOffWidth-1, uG2dOffHeight-1, 0, 0, uG2dWidth/2-1, uG2dHeight/2-1,uIsStretch);
		UART_Printf("Alpha:%d\n", sAlphaVal);					
		if(sAlphaVal==0) break;
		UART_Getc();
		G2D_SetAlphaValue(255);
		G2D_DrawImage(eBpp);		
	}	

	G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);	
	UART_Printf("Press any key to continue!\n");
	UART_Getc();
	
	// Stretched BitBLT
	UART_Printf("Stretched BitBLT Test with BLENDING, Press any key to continue!\n");
	uIsStretch=1;
	G2D_DrawImage(eBpp);
	G2D_SetFbAddr(uG2dOffFbAddr, SRCBUF);	
	G2D_SetBppMode(eBppOff, SRCBUF);
	G2D_SetSourceSize(uG2dOffWidth, uG2dOffHeight);	
	UART_Getc();
	G2D_SetAlphaMode(G2D_ALPHA_MODE);
	sAlphaVal=255;
	while(1) {
		sAlphaVal -= 8;
		if(sAlphaVal<=0) sAlphaVal=0;
		G2D_SetAlphaValue((u8)sAlphaVal);
		G2D_BitBlt(0, 0, uG2dOffWidth-1, uG2dOffHeight-1, 0, 0, uG2dWidth-1, uG2dHeight/2-1,uIsStretch);
		UART_Printf("Alpha:%d\n", sAlphaVal);					
		if(sAlphaVal==0) break;
		UART_Getc();
		G2D_SetAlphaValue(255);
		G2D_DrawImage(eBpp);		
	}	

	UART_Printf("Press any key to exit!\n");
	UART_Getc();
	
#ifdef INTERRUPT_MODE	
	G2D_InterruptDisable();
	INTC_Disable(NUM_2D);
#endif

	G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);	
}

void TestOffScrBitBLTWithClipping(void)
{
	u32 uG2dFbAddr, uG2dWidth, uG2dHeight;
	u32 uG2dOffFbAddr, uG2dOffWidth, uG2dOffHeight;	
	CSPACE eBpp, eBppOff;
	u32 uDec;

	u8 uIsStretch=0;

#ifdef INTERRUPT_MODE
	INTC_SetVectAddr(NUM_2D, Isr_G2d);
	INTC_Enable(NUM_2D);
	G2D_InterruptEnable();
#endif

	G2D_GetFrmSz(&uG2dWidth, &uG2dHeight);
	G2D_GetFbAddr(&uG2dFbAddr);	
	G2D_GetSrcBppMode(&eBpp);
		
	UART_Printf("Width=%d, Height=%d, Framebuffer:0x%x\n", uG2dWidth, uG2dHeight, uG2dFbAddr);
	UART_Printf("BitBLT Test, Press any key to continue!\n");
	uG2dOffFbAddr = CODEC_MEM_ST+0x800000;
	uG2dOffWidth=uG2dWidth;
	uG2dOffHeight=uG2dHeight;
	eBppOff=eBpp;
	G2D_SetFbAddr(uG2dOffFbAddr, SRCBUF);	
	G2D_SetBppMode(eBppOff, SRCBUF);
	G2D_SetSourceSize(uG2dOffWidth, uG2dOffHeight);	
	GLIB_InitInstance(uG2dOffFbAddr, uG2dOffWidth, uG2dOffHeight, eBppOff);
	GLIB_DrawPattern(uG2dOffWidth, uG2dOffHeight);	
	UART_Printf("Press any key to continue!\n");
	UART_Getc();
	G2D_DrawImage(eBpp);

	// Normal BitBLT
	UART_Printf("Normal BitBLT Test with Clipping, Press any key to continue!\n");
	uIsStretch=0;
	UART_Getc();
	uDec=0;
	while(1){
		uDec+=8;
		if((uG2dWidth-2*uDec <=32)||(uG2dHeight-2*uDec <=32)) break;
		G2D_SetClippingWindow(0+uDec, 0+uDec, uG2dWidth-uDec, uG2dHeight-uDec);
		UART_Printf("[Clipping]x1=%d,y1=%d,x2=%d,y2=%d\n",uG2dWidth-uDec, uG2dHeight-uDec, uG2dWidth-uDec, uG2dHeight-uDec);
		G2D_BitBlt(0, 0,uG2dOffWidth-1,uG2dOffHeight-1,0, 0,uG2dWidth-1,uG2dHeight-1,uIsStretch);
		UART_Getc();
		G2D_DrawImage(eBpp);

	}
	G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);	
	UART_Printf("Press any key to continue!\n");
	UART_Getc();
	
	// Stretched BitBLT
	UART_Printf("Stretched BitBLT Test with Clipping, Press any key to continue!\n");
	uIsStretch=1;
	G2D_DrawImage(eBpp);
	uG2dOffFbAddr = CODEC_MEM_ST+0x800000;
	uG2dOffWidth=uG2dWidth/2;
	uG2dOffHeight=uG2dHeight/2;
	eBppOff=eBpp;
	G2D_SetFbAddr(uG2dOffFbAddr, SRCBUF);	
	G2D_SetBppMode(eBppOff, SRCBUF);
	G2D_SetSourceSize(uG2dOffWidth, uG2dOffHeight);	
	GLIB_InitInstance(uG2dOffFbAddr, uG2dOffWidth, uG2dOffHeight, eBppOff);
	GLIB_DrawPattern(uG2dOffWidth, uG2dOffHeight);		
	UART_Getc();
	uDec=0;
	while(1){
		uDec+=8;
		if((uG2dWidth-2*uDec <=32)||(uG2dHeight-2*uDec <=32)) break;
		G2D_SetClippingWindow(0+uDec, 0+uDec, uG2dWidth-uDec, uG2dHeight-uDec);
		UART_Printf("[Clipping]x1=%d,y1=%d,x2=%d,y2=%d\n",uG2dWidth-uDec, uG2dHeight-uDec, uG2dWidth-uDec, uG2dHeight-uDec);
		G2D_BitBlt(0, 0,uG2dOffWidth-1,uG2dOffHeight-1,0, 0,uG2dWidth-1,uG2dHeight-1,uIsStretch);
		UART_Getc();
		G2D_DrawImage(eBpp);

	}
	

	UART_Printf("Press any key to exit!\n");
	UART_Getc();
	
#ifdef INTERRUPT_MODE	
	G2D_InterruptDisable();
	INTC_Disable(NUM_2D);
#endif

	G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);	
}

void TestOffScrBitBLTWithTransparency(void)
{
	u32 i;
	u32 uG2dFbAddr, uG2dWidth, uG2dHeight;
	u32 uG2dOffFbAddr, uG2dOffWidth, uG2dOffHeight;
	CSPACE eBpp, eBppOff;

	u8 uIsStretch=0;
	u8 bIsTransparent=0;

#ifdef INTERRUPT_MODE
	INTC_SetVectAddr(NUM_2D, Isr_G2d);
	INTC_Enable(NUM_2D);
	G2D_InterruptEnable();
#endif


	G2D_GetFrmSz(&uG2dWidth, &uG2dHeight);
	G2D_GetFbAddr(&uG2dFbAddr);	
	G2D_GetSrcBppMode(&eBpp);
		
	UART_Printf("Width=%d, Height=%d, Framebuffer:0x%x\n", uG2dWidth, uG2dHeight, uG2dFbAddr);
	
	G2D_DrawImage(eBpp);

	UART_Printf("BitBLT Test, Press any key to continue!\n");
	UART_Getc();

	// Normal BitBLT	
	uG2dOffFbAddr = CODEC_MEM_ST+0x800000;
	uG2dOffWidth=uG2dWidth;
	uG2dOffHeight=uG2dHeight;
	eBppOff=eBpp;
	G2D_SetFbAddr(uG2dOffFbAddr, SRCBUF);	
	G2D_SetBppMode(eBppOff, SRCBUF);
	G2D_SetSourceSize(uG2dOffWidth, uG2dOffHeight);	
	GLIB_InitInstance(uG2dOffFbAddr, uG2dOffWidth, uG2dOffHeight, eBppOff);
	GLIB_DrawPattern(uG2dOffWidth, uG2dOffHeight);
		
	UART_Printf("Press any key to test Normal BitBLT!\n");

	GLIB_FillRectangle(0,uG2dOffHeight/8,uG2dWidth,uG2dOffHeight*2/8,G2D_MAGENTA);
	GLIB_FillRectangle(0,uG2dOffHeight*5/8,uG2dWidth,uG2dOffHeight*2/8,G2D_YELLOW);
	UART_Getc();	
	uIsStretch=0;
	
	UART_Printf("Opaque Mode!!\n");	
	bIsTransparent=0;
	G2D_SetTransparentMode(bIsTransparent, G2D_MAGENTA);
	G2D_BitBlt(0, 0, uG2dOffWidth-1, uG2dOffHeight/2-1, 0, 0, uG2dWidth-1, uG2dHeight/2-1, uIsStretch);
	UART_Printf("Transparent Mode!!\n");
	bIsTransparent=1;
	G2D_SetTransparentMode(bIsTransparent, G2D_YELLOW);
	G2D_BitBlt(0, uG2dOffHeight/2, uG2dOffWidth-1, uG2dOffHeight-1, 0, uG2dHeight/2, uG2dWidth-1, uG2dHeight-1, uIsStretch);
	UART_Printf("Press any key to continue!\n");
	UART_Getc();


	// Stretched BitBLT
	UART_Printf("Press any key to test Stretched BitBLT!\n");
	G2D_DrawImage(eBpp);	
	G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);	
	uG2dOffFbAddr = CODEC_MEM_ST+0x800000;
	uG2dOffWidth=uG2dWidth/2;
	uG2dOffHeight=uG2dHeight/2;
	eBppOff=eBpp;
	G2D_SetFbAddr(uG2dOffFbAddr, SRCBUF);	
	G2D_SetBppMode(eBppOff, SRCBUF);
	G2D_SetSourceSize(uG2dOffWidth, uG2dOffHeight);	
	GLIB_InitInstance(uG2dOffFbAddr, uG2dOffWidth, uG2dOffHeight, eBppOff);
	GLIB_DrawPattern(uG2dOffWidth, uG2dOffHeight);	

	GLIB_FillRectangle(0,uG2dOffHeight/8,uG2dWidth/2,uG2dOffHeight*2/8,G2D_YELLOW);
	GLIB_FillRectangle(0,uG2dOffHeight*5/8,uG2dWidth/2,uG2dOffHeight*2/8,G2D_MAGENTA);	
	UART_Getc();
		
	uIsStretch=1;	
	UART_Printf("Opaque Mode!!\n");	
	bIsTransparent=0;
	G2D_SetTransparentMode(bIsTransparent, G2D_YELLOW);
	G2D_BitBlt(0, 0, uG2dOffWidth-1, uG2dOffHeight-1, 0, 0, uG2dWidth-1, uG2dHeight/2-1, uIsStretch);
	UART_Printf("Transparent Mode!!\n");
	bIsTransparent=1;
	G2D_SetTransparentMode(bIsTransparent, G2D_MAGENTA);
	G2D_BitBlt(0, 0, uG2dOffWidth-1, uG2dOffHeight-1, 0, uG2dHeight/2, uG2dWidth-1, uG2dHeight-1, uIsStretch);
	UART_Printf("Press any key to continue!\n");

	UART_Getc();
	G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);	


#ifdef INTERRUPT_MODE	
	G2D_InterruptDisable();
	INTC_Disable(NUM_2D);
#endif

	G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);	
}


void TestOffScrBitBLTWithAll(void)
{
	u32 i;
	u32 uG2dFbAddr, uG2dWidth, uG2dHeight;
	u32 uG2dOffFbAddr, uG2dOffWidth, uG2dOffHeight;
	CSPACE eBpp, eBppOff;

	u8 uIsStretch=0;
	u8 bIsTransparent=0;

#ifdef INTERRUPT_MODE
	INTC_SetVectAddr(NUM_2D, Isr_G2d);
	INTC_Enable(NUM_2D);
	G2D_InterruptEnable();
#endif


	G2D_GetFrmSz(&uG2dWidth, &uG2dHeight);
	G2D_GetFbAddr(&uG2dFbAddr);	
	G2D_GetSrcBppMode(&eBpp);
		
	UART_Printf("Width=%d, Height=%d, Framebuffer:0x%x\n", uG2dWidth, uG2dHeight, uG2dFbAddr);

	G2D_DrawImage(eBpp);
	
	uG2dOffFbAddr = CODEC_MEM_ST+0x800000;
	uG2dOffWidth=32;
	uG2dOffHeight=40;
	eBppOff=eBpp;
	G2D_SetFbAddr(uG2dOffFbAddr, SRCBUF);	
	G2D_SetBppMode(eBppOff, SRCBUF);
	G2D_SetSourceSize(uG2dOffWidth, uG2dOffHeight);	
	GLIB_InitInstance(uG2dOffFbAddr, uG2dOffWidth, uG2dOffHeight, eBppOff);
	GLIB_DrawPattern(uG2dOffWidth, uG2dOffHeight);	

	UART_Printf("BitBLT Test, Press any key to continue!\n");
	UART_Getc();

	// Normal BitBLT
	UART_Printf("Press any key to test Normal BitBLT!\n");
	GLIB_FillRectangle(0,15,32,10,G2D_MAGENTA);
	UART_Getc();	
	uIsStretch=0;
	
	UART_Printf("Opaque Mode!!\n");	
	bIsTransparent=0;	
	G2D_SetAlphaMode(G2D_ALPHA_MODE);
	G2D_SetAlphaValue(0xcf);
	G2D_RegisterPattern(pat8x8_rgbw);		
	G2D_Set3rdOperand(G2D_OPERAND3_PAT);
	
	G2D_SetTransparentMode(bIsTransparent, G2D_MAGENTA);
	G2D_BitBlt(0, 0, uG2dOffWidth-1, uG2dOffHeight-1, uG2dWidth/2-1, uG2dHeight/2-1, uG2dWidth/2-1+uG2dOffWidth-1, uG2dHeight/2-1+uG2dOffHeight-1, uIsStretch);
//	G2D_PutRectangleEcolor(uG2dWidth/2-1, uG2dHeight/2-1, 40, 40, G2D_RED);
	UART_Printf("Press any key to continue!\n");
	G2D_SetRotationMode(G2D_ROTATION_90_DEG_BIT);
	G2D_SetRotationOrg((u16)(uG2dWidth/2-1), (u16)(uG2dHeight/2-1));
	G2D_SetTransparentMode(bIsTransparent, G2D_MAGENTA);
	G2D_BitBlt(0, 0, uG2dOffWidth-1, uG2dOffHeight-1, uG2dWidth/2-1, uG2dHeight/2-1, uG2dWidth/2-1+uG2dOffWidth-1, uG2dHeight/2-1+uG2dOffHeight-1, uIsStretch);
//	G2D_PutRectangleEcolor(uG2dWidth/2-1, uG2dHeight/2-1, 40, 40, G2D_RED);
	G2D_SetRopEtype(ROP_SRC_XOR_3RD_OPRND);	
	G2D_SetRotationMode(G2D_ROTATION_180_DEG_BIT);
	G2D_SetClippingWindow(uG2dWidth/2-1-20, uG2dHeight/2-1-20, uG2dWidth/2-1+20, uG2dHeight/2-1+20);
	G2D_SetRotationOrg((u16)(uG2dWidth/2-1), (u16)(uG2dHeight/2-1));
	G2D_SetTransparentMode(bIsTransparent, G2D_MAGENTA);
	G2D_BitBlt(0, 0, uG2dOffWidth-1, uG2dOffHeight-1, uG2dWidth/2-1, uG2dHeight/2-1, uG2dWidth/2-1+uG2dOffWidth-1, uG2dHeight/2-1+uG2dOffHeight-1, uIsStretch);
//	G2D_PutRectangleEcolor(uG2dWidth/2-1, uG2dHeight/2-1, 40, 40, G2D_RED);
	G2D_SetRopEtype(ROP_SRC_XOR_3RD_OPRND);	
	G2D_SetRotationMode(G2D_ROTATION_270_DEG_BIT);
	G2D_SetClippingWindow(uG2dWidth/2-1-20, uG2dHeight/2-1-20, uG2dWidth/2-1+20, uG2dHeight/2-1+20);
	G2D_SetRotationOrg((u16)(uG2dWidth/2-1), (u16)(uG2dHeight/2-1));
	G2D_SetTransparentMode(bIsTransparent, G2D_MAGENTA);
	G2D_BitBlt(0, 0, uG2dOffWidth-1, uG2dOffHeight-1, uG2dWidth/2-1, uG2dHeight/2-1, uG2dWidth/2-1+uG2dOffWidth-1, uG2dHeight/2-1+uG2dOffHeight-1, uIsStretch);
//	G2D_PutRectangleEcolor(uG2dWidth/2-1, uG2dHeight/2-1, 40, 40, G2D_RED);
	UART_Printf("Press any key to continue!\n");
	UART_Getc();	
	G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);	


	G2D_SetFbAddr(uG2dOffFbAddr, SRCBUF);	
	G2D_SetBppMode(eBppOff, SRCBUF);
	G2D_SetSourceSize(uG2dOffWidth, uG2dOffHeight);	
	G2D_DrawImage(eBpp);
	UART_Printf("Transparent Mode!!\n");	
	UART_Getc();	
	
	bIsTransparent=1;	
	G2D_SetAlphaMode(G2D_ALPHA_MODE);
	G2D_SetAlphaValue(0xcf);
	G2D_RegisterPattern(pat8x8_rgbw);	
	G2D_Set3rdOperand(G2D_OPERAND3_PAT);
	
	G2D_SetTransparentMode(bIsTransparent, G2D_MAGENTA);
	G2D_BitBlt(0, 0, uG2dOffWidth-1, uG2dOffHeight-1, uG2dWidth/2-1, uG2dHeight/2-1, uG2dWidth/2-1+uG2dOffWidth-1, uG2dHeight/2-1+uG2dOffHeight-1, uIsStretch);
	G2D_SetRotationMode(G2D_ROTATION_90_DEG_BIT);
	G2D_SetRotationOrg((u16)(uG2dWidth/2-1), (u16)(uG2dHeight/2-1));
	G2D_SetTransparentMode(bIsTransparent, G2D_MAGENTA);
	G2D_BitBlt(0, 0, uG2dOffWidth-1, uG2dOffHeight-1, uG2dWidth/2-1, uG2dHeight/2-1, uG2dWidth/2-1+uG2dOffWidth-1, uG2dHeight/2-1+uG2dOffHeight-1, uIsStretch);

	G2D_SetRopEtype(ROP_SRC_XOR_3RD_OPRND);	
	G2D_SetRotationMode(G2D_ROTATION_180_DEG_BIT);
	G2D_SetClippingWindow(uG2dWidth/2-1-20, uG2dHeight/2-1-20, uG2dWidth/2-1+20, uG2dHeight/2-1+20);
	G2D_SetRotationOrg((u16)(uG2dWidth/2-1), (u16)(uG2dHeight/2-1));
	G2D_SetTransparentMode(bIsTransparent, G2D_MAGENTA);
	G2D_BitBlt(0, 0, uG2dOffWidth-1, uG2dOffHeight-1, uG2dWidth/2-1, uG2dHeight/2-1, uG2dWidth/2-1+uG2dOffWidth-1, uG2dHeight/2-1+uG2dOffHeight-1, uIsStretch);

	G2D_SetRopEtype(ROP_SRC_XOR_3RD_OPRND);	
	G2D_SetRotationMode(G2D_ROTATION_270_DEG_BIT);
	G2D_SetClippingWindow(uG2dWidth/2-1-20, uG2dHeight/2-1-20, uG2dWidth/2-1+20, uG2dHeight/2-1+20);
	G2D_SetRotationOrg((u16)(uG2dWidth/2-1), (u16)(uG2dHeight/2-1));
	G2D_SetTransparentMode(bIsTransparent, G2D_MAGENTA);
	G2D_BitBlt(0, 0, uG2dOffWidth-1, uG2dOffHeight-1, uG2dWidth/2-1, uG2dHeight/2-1, uG2dWidth/2-1+uG2dOffWidth-1, uG2dHeight/2-1+uG2dOffHeight-1, uIsStretch);
	UART_Printf("Press any key to continue!\n");
	UART_Getc();	
	G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);	 


	G2D_SetFbAddr(uG2dOffFbAddr, SRCBUF);	
	G2D_SetBppMode(eBppOff, SRCBUF);
	G2D_SetSourceSize(uG2dOffWidth, uG2dOffHeight);		
	// Stretched BitBLT
	UART_Printf("Press any key to test Stretched BitBLT!\n");
	UART_Printf("Opaque Mode!!\n");	
	G2D_DrawImage(eBpp);
	UART_Getc();	
	uIsStretch=1;
	
	bIsTransparent=0;	
	G2D_SetAlphaMode(G2D_ALPHA_MODE);
	G2D_SetAlphaValue(0xcf);
	G2D_RegisterPattern(pat8x8_rgbw);	
	G2D_Set3rdOperand(G2D_OPERAND3_PAT);
	
	G2D_SetTransparentMode(bIsTransparent, G2D_MAGENTA);
	G2D_BitBlt(0, 0, uG2dOffWidth-1, uG2dOffHeight-1, uG2dWidth/2-1, uG2dHeight/2-1, uG2dWidth/2-1+uG2dOffWidth*2-1, uG2dHeight/2-1+uG2dOffHeight*2-1, uIsStretch);
	G2D_SetRotationMode(G2D_ROTATION_90_DEG_BIT);
	G2D_SetRotationOrg((u16)(uG2dWidth/2-1), (u16)(uG2dHeight/2-1));
	G2D_SetTransparentMode(bIsTransparent, G2D_MAGENTA);
	G2D_BitBlt(0, 0, uG2dOffWidth-1, uG2dOffHeight-1, uG2dWidth/2-1, uG2dHeight/2-1, uG2dWidth/2-1+uG2dOffWidth*2-1, uG2dHeight/2-1+uG2dOffHeight*2-1, uIsStretch);

	G2D_SetRopEtype(ROP_SRC_XOR_3RD_OPRND);	
	G2D_SetRotationMode(G2D_ROTATION_180_DEG_BIT);
	G2D_SetClippingWindow(uG2dWidth/2-1-40, uG2dHeight/2-1-40, uG2dWidth/2-1+40, uG2dHeight/2-1+40);
	G2D_SetRotationOrg((u16)(uG2dWidth/2-1), (u16)(uG2dHeight/2-1));
	G2D_SetTransparentMode(bIsTransparent, G2D_MAGENTA);
	G2D_BitBlt(0, 0, uG2dOffWidth-1, uG2dOffHeight-1, uG2dWidth/2-1, uG2dHeight/2-1, uG2dWidth/2-1+uG2dOffWidth*2-1, uG2dHeight/2-1+uG2dOffHeight*2-1, uIsStretch);

	G2D_SetRopEtype(ROP_SRC_XOR_3RD_OPRND);	
	G2D_SetRotationMode(G2D_ROTATION_270_DEG_BIT);
	G2D_SetClippingWindow(uG2dWidth/2-1-40, uG2dHeight/2-1-40, uG2dWidth/2-1+40, uG2dHeight/2-1+40);
	G2D_SetRotationOrg((u16)(uG2dWidth/2-1), (u16)(uG2dHeight/2-1));
	G2D_SetTransparentMode(bIsTransparent, G2D_MAGENTA);
	G2D_BitBlt(0, 0, uG2dOffWidth-1, uG2dOffHeight-1, uG2dWidth/2-1, uG2dHeight/2-1, uG2dWidth/2-1+uG2dOffWidth*2-1, uG2dHeight/2-1+uG2dOffHeight*2-1, uIsStretch);

	UART_Printf("Press any key to continue!\n");
	UART_Getc();	
	G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);	

	G2D_SetFbAddr(uG2dOffFbAddr, SRCBUF);	
	G2D_SetBppMode(eBppOff, SRCBUF);
	G2D_SetSourceSize(uG2dOffWidth, uG2dOffHeight);		
	UART_Printf("Transparent Mode!!\n");	
	G2D_DrawImage(eBpp);
	UART_Getc();	
	uIsStretch=1;	
	bIsTransparent=1;	
	G2D_SetAlphaMode(G2D_ALPHA_MODE);
	G2D_SetAlphaValue(0xcf);
	G2D_RegisterPattern(pat8x8_rgbw);	
	G2D_Set3rdOperand(G2D_OPERAND3_PAT);
	
	G2D_SetTransparentMode(bIsTransparent, G2D_MAGENTA);
	G2D_BitBlt(0, 0, uG2dOffWidth-1, uG2dOffHeight-1, uG2dWidth/2-1, uG2dHeight/2-1, uG2dWidth/2-1+uG2dOffWidth*2-1, uG2dHeight/2-1+uG2dOffHeight*2-1, uIsStretch);
	G2D_SetRotationMode(G2D_ROTATION_90_DEG_BIT);
	G2D_SetRotationOrg((u16)(uG2dWidth/2-1), (u16)(uG2dHeight/2-1));
	G2D_SetTransparentMode(bIsTransparent, G2D_MAGENTA);
	G2D_BitBlt(0, 0, uG2dOffWidth-1, uG2dOffHeight-1, uG2dWidth/2-1, uG2dHeight/2-1, uG2dWidth/2-1+uG2dOffWidth*2-1, uG2dHeight/2-1+uG2dOffHeight*2-1, uIsStretch);

	G2D_SetRopEtype(ROP_SRC_XOR_3RD_OPRND);	
	G2D_SetRotationMode(G2D_ROTATION_180_DEG_BIT);
	G2D_SetClippingWindow(uG2dWidth/2-1-40, uG2dHeight/2-1-40, uG2dWidth/2-1+40, uG2dHeight/2-1+40);
	G2D_SetRotationOrg((u16)(uG2dWidth/2-1), (u16)(uG2dHeight/2-1));
	G2D_SetTransparentMode(bIsTransparent, G2D_MAGENTA);
	G2D_BitBlt(0, 0, 39, 39, uG2dWidth/2-1, uG2dHeight/2-1, uG2dWidth/2-1+79, uG2dHeight/2-1+79, uIsStretch);

	G2D_SetRopEtype(ROP_SRC_XOR_3RD_OPRND);	
	G2D_SetRotationMode(G2D_ROTATION_270_DEG_BIT);
	G2D_SetClippingWindow(uG2dWidth/2-1-40, uG2dHeight/2-1-40, uG2dWidth/2-1+40, uG2dHeight/2-1+40);
	G2D_SetRotationOrg((u16)(uG2dWidth/2-1), (u16)(uG2dHeight/2-1));
	G2D_SetTransparentMode(bIsTransparent, G2D_MAGENTA);
	G2D_BitBlt(0, 0, 39, 39, uG2dWidth/2-1, uG2dHeight/2-1, uG2dWidth/2-1+79, uG2dHeight/2-1+79, uIsStretch);

	UART_Printf("Press any key to continue!\n");
	UART_Getc();	
	
#ifdef INTERRUPT_MODE	
	G2D_InterruptDisable();
	INTC_Disable(NUM_2D);
#endif

	G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);	
}

void TestHostToScrBitBLTSimple(void)
{
	u32 uG2dFbAddr, uG2dWidth, uG2dHeight;
	CSPACE eBpp;
	u32 i,j;


#ifdef INTERRUPT_MODE
	INTC_SetVectAddr(NUM_2D, Isr_G2d);
	INTC_Enable(NUM_2D);
	G2D_InterruptEnable();
#endif


	G2D_GetFrmSz(&uG2dWidth, &uG2dHeight);
	G2D_GetFbAddr(&uG2dFbAddr);	
	G2D_GetSrcBppMode(&eBpp);
		
	UART_Printf("Width=%d, Height=%d, Framebuffer:0x%x\n", uG2dWidth, uG2dHeight, uG2dFbAddr);

	G2D_ClearFrameEcolor(G2D_BLACK, 0, 0);

	UART_Printf("Host to Screen BitBLT Test, Press any key to continue!\n");
	UART_Getc();


	G2D_SetCoordinateDstBlock(0, 0, uG2dWidth-1, uG2dHeight-1);
	for(j=0; j<uG2dHeight; j++)
		for(i=0; i<uG2dWidth; i++) 
		{
			if(i<(uG2dWidth/2) && j<(uG2dHeight/2)) 
			{
				if(i==0 && j==0) G2D_SetFirstBitBLTEData(G2D_RED);
				else G2D_SetNextBitBLTEData(G2D_RED);
			}
			else if(i>=(uG2dWidth/2) && (j<uG2dHeight/2)) G2D_SetNextBitBLTEData(G2D_GREEN);
			else if(i<(uG2dWidth/2) && j>=(uG2dHeight/2)) G2D_SetNextBitBLTEData(G2D_BLUE);
			else G2D_SetNextBitBLTEData(G2D_WHITE);
		}			
	
	G2D_WaitForInterrupt();
	UART_Printf("Test is Done!\n");

#ifdef INTERRUPT_MODE	
	G2D_InterruptDisable();
	INTC_Disable(NUM_2D);
#endif

	G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);	
}

void TestHostToScrBitBLTWithRop(void)
{
	u32 l, i, j;
	u32 uG2dFbAddr, uG2dWidth, uG2dHeight;
	CSPACE eBpp;


#ifdef INTERRUPT_MODE
	INTC_SetVectAddr(NUM_2D, Isr_G2d);
	INTC_Enable(NUM_2D);
	G2D_InterruptEnable();
#endif

	G2D_GetFrmSz(&uG2dWidth, &uG2dHeight);
	G2D_GetFbAddr(&uG2dFbAddr);	
	G2D_GetSrcBppMode(&eBpp);
		
	UART_Printf("Width=%d, Height=%d, Framebuffer:0x%x\n", uG2dWidth, uG2dHeight, uG2dFbAddr);

	G2D_ClearFrameEcolor(G2D_BLACK, 0, 0);

	UART_Printf("Host to Screen BitBLT with ROP Test, Press any key to continue!\n");
	UART_Getc();

	
	
	G2D_RegisterPattern(pat8x8_rgbw);	
	G2D_Set3rdOperand(G2D_OPERAND3_PAT);
	UART_Printf("Normal BitBLT with Raster Operation, press any key to continue\n");
	UART_Getc();	


	G2D_SetCoordinateDstBlock(0, 0, uG2dWidth-1, uG2dHeight-1);

	for(l=0; l<=8; l++) {
		G2D_SetRopEtype((G2D_ROP_TYPE)(l));
		switch((G2D_ROP_TYPE)l){
			case ROP_DST_ONLY: 
				UART_Printf("Destination Only, press any key to continue: ");
				break;
			case ROP_SRC_ONLY: 
				UART_Printf("Source Only, press any key to continue: ");
				break;
			case ROP_3RD_OPRND_ONLY: 
				UART_Printf("Third Operand Only, press any key to continue: ");
				break;
			case ROP_SRC_AND_DST: 
				UART_Printf("Source AND Destination, press any key to continue: ");
				break;
			case ROP_SRC_AND_3RD_OPRND: 
				UART_Printf("Source AND Third Operand, press any key to continue: ");
				break;
			case ROP_SRC_OR_DST: 
				UART_Printf("Source OR Destination, press any key to continue: ");
				break;
			case ROP_SRC_OR_3RD_OPRND: 
				UART_Printf("Source OR Third Operand, press any key to continue: ");
				break;				
			case ROP_DST_OR_3RD:
				UART_Printf("Destination OR Third Operand, press any key to continue: ");
				break;
			case ROP_SRC_XOR_3RD_OPRND: 
				UART_Printf("Source XOR Third Operand, press any key to continue: ");
				break;
			default:
				break;
		} 

		for(j=0; j<uG2dHeight; j++)
			for(i=0; i<uG2dWidth; i++) 
			{
				if(i<(uG2dWidth/2) && j<(uG2dHeight/2)) 
				{
					if(i==0 && j==0) G2D_SetFirstBitBLTEData(G2D_RED);
					else G2D_SetNextBitBLTEData(G2D_RED);
				}
				else if(i>=(uG2dWidth/2) && (j<uG2dHeight/2)) G2D_SetNextBitBLTEData(G2D_GREEN);
				else if(i<(uG2dWidth/2) && j>=(uG2dHeight/2)) G2D_SetNextBitBLTEData(G2D_BLUE);
				else G2D_SetNextBitBLTEData(G2D_WHITE);
			}
			
		G2D_WaitForInterrupt();
		UART_Printf("Done!\n");
		UART_Getc();
		G2D_SetRopEtype(ROP_SRC_ONLY);

		G2D_ClearFrameEcolor(G2D_BLACK, 0, 0);
	}

	G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);	


	
#ifdef INTERRUPT_MODE	
	G2D_InterruptDisable();
	INTC_Disable(NUM_2D);
#endif

	G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);	
}

void TestHostToScrBitBLTWithRotation(void)
{
	u32 i;
	u32 uG2dFbAddr, uG2dWidth, uG2dHeight;
	CSPACE eBpp;


#ifdef INTERRUPT_MODE
	INTC_SetVectAddr(NUM_2D, Isr_G2d);
	INTC_Enable(NUM_2D);
	G2D_InterruptEnable();
#endif

	G2D_GetFrmSz(&uG2dWidth, &uG2dHeight);
	G2D_GetFbAddr(&uG2dFbAddr);	
	G2D_GetSrcBppMode(&eBpp);
		
	UART_Printf("Width=%d, Height=%d, Framebuffer:0x%x\n", uG2dWidth, uG2dHeight, uG2dFbAddr);

	G2D_ClearFrameEcolor(G2D_BLACK, 0, 0);
	G2D_PutLineEcolor(uG2dWidth/2-1, 0, uG2dWidth/2-1, uG2dHeight-1, G2D_WHITE, true);
	G2D_PutLineEcolor(0, uG2dHeight/2-1, uG2dWidth-1, uG2dHeight/2-1, G2D_WHITE, true);
	

	UART_Printf("BitBLT Test, Press any key to continue!\n");
	UART_Getc();

	G2D_SetCoordinateDstBlock(uG2dWidth/2, uG2dHeight/2, uG2dWidth/2+29, uG2dHeight/2+29);
	for(i=0; i<(30*30); i++)
		if(i==0) G2D_SetFirstBitBLTEData(G2D_RED);
		else G2D_SetNextBitBLTEData(G2D_RED);
	G2D_WaitForInterrupt();
	UART_Printf("Press any key to TEST!\n");
	UART_Getc();

	UART_Printf("90 degree rotation with BitBLT, Press any key to continue!\n");
	G2D_SetRotationMode(G2D_ROTATION_90_DEG_BIT);
	G2D_SetRotationOrg((u16)(uG2dWidth/2-1), (u16)(uG2dHeight/2-1));
	for(i=0; i<(30*30); i++)
		if(i==0) G2D_SetFirstBitBLTEData(G2D_GREEN);
		else G2D_SetNextBitBLTEData(G2D_GREEN);
	G2D_WaitForInterrupt();
	UART_Getc();
	
	UART_Printf("180 degree rotation with BitBLT, Press any key to continue!\n");
	G2D_SetRotationMode(G2D_ROTATION_180_DEG_BIT);
	G2D_SetRotationOrg((u16)(uG2dWidth/2-1), (u16)(uG2dHeight/2-1));
	for(i=0; i<(30*30); i++)
		if(i==0) G2D_SetFirstBitBLTEData(G2D_BLUE);
		else G2D_SetNextBitBLTEData(G2D_BLUE);
	G2D_WaitForInterrupt();
	UART_Getc();
	
	UART_Printf("270 degree rotation with BitBLT, Press any key to continue!\n");
	G2D_SetRotationMode(G2D_ROTATION_270_DEG_BIT);
	G2D_SetRotationOrg((u16)(uG2dWidth/2-1), (u16)(uG2dHeight/2-1));
	for(i=0; i<(30*30); i++)
		if(i==0) G2D_SetFirstBitBLTEData(G2D_YELLOW);
		else G2D_SetNextBitBLTEData(G2D_YELLOW);
	G2D_WaitForInterrupt();
	UART_Getc();

		
	UART_Printf("Flip with BitBLT, Press any key to continue!\n");
	G2D_SetRotationMode(G2D_ROTATION_0_DEG_BIT);		
	G2D_ClearFrameEcolor(G2D_BLACK, 0, 0);
	G2D_PutLineEcolor(uG2dWidth/2-1, 0, uG2dWidth/2-1, uG2dHeight-1, G2D_WHITE, true);
	G2D_PutLineEcolor(0, uG2dHeight/2-1, uG2dWidth-1, uG2dHeight/2-1, G2D_WHITE, true);

	G2D_SetCoordinateDstBlock(uG2dWidth/2, uG2dHeight/2, uG2dWidth/2+29, uG2dHeight/2+29);
	for(i=0; i<(30*30); i++)
		if(i==0) G2D_SetFirstBitBLTEData(G2D_RED);
		else G2D_SetNextBitBLTEData(G2D_RED);
	G2D_WaitForInterrupt();
	UART_Getc();
	
	UART_Printf("X-Flip with BitBLT, Press any key to continue!\n");
	G2D_SetRotationMode(G2D_X_FLIP);	
	G2D_SetRotationOrg((u16)(uG2dWidth/2-1), (u16)(uG2dHeight/2-1));
	for(i=0; i<(30*30); i++)
		if(i==0) G2D_SetFirstBitBLTEData(G2D_GREEN);
		else G2D_SetNextBitBLTEData(G2D_GREEN);
	G2D_WaitForInterrupt();
	UART_Getc();
	
	UART_Printf("Y-Flip with BitBLT, Press any key to continue!\n");
	G2D_SetRotationMode(G2D_Y_FLIP);	
	G2D_SetRotationOrg((u16)(uG2dWidth/2-1), (u16)(uG2dHeight/2-1));
	for(i=0; i<(30*30); i++)
		if(i==0) G2D_SetFirstBitBLTEData(G2D_BLUE);
		else G2D_SetNextBitBLTEData(G2D_BLUE);
	G2D_WaitForInterrupt();
	
	UART_Getc();	
	G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);	

	
#ifdef INTERRUPT_MODE	
	G2D_InterruptDisable();
	INTC_Disable(NUM_2D);
#endif

	G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);	
}

void TestHostToScrBitBLTWithBlending(void)
{
	u32 uG2dFbAddr, uG2dWidth, uG2dHeight;
	CSPACE eBpp;
	s32 sAlphaVal;
	u32 i,j;


#ifdef INTERRUPT_MODE
	INTC_SetVectAddr(NUM_2D, Isr_G2d);
	INTC_Enable(NUM_2D);
	G2D_InterruptEnable();
#endif

	G2D_GetFrmSz(&uG2dWidth, &uG2dHeight);
	G2D_GetFbAddr(&uG2dFbAddr);	
	G2D_GetSrcBppMode(&eBpp);
		
	UART_Printf("Width=%d, Height=%d, Framebuffer:0x%x\n", uG2dWidth, uG2dHeight, uG2dFbAddr);
	UART_Printf("BitBLT Test, Press any key to continue!\n");
	UART_Getc();

	G2D_DrawImage(eBpp);

	UART_Printf("Host to Screen BitBLT Test with BLENDING, Press any key to continue!\n");
	UART_Getc();
	
	G2D_SetAlphaMode(G2D_ALPHA_MODE);
	sAlphaVal=255;
	while(1) {
		sAlphaVal -= 8;
		if(sAlphaVal<=0) sAlphaVal=0;
		G2D_SetAlphaValue((u8)sAlphaVal);
		G2D_SetCoordinateDstBlock(0, 0, uG2dWidth-1, uG2dHeight-1);
		for(j=0; j<uG2dHeight; j++)
			for(i=0; i<uG2dWidth; i++) 
			{
				if(i<(uG2dWidth/2) && j<(uG2dHeight/2)) 
				{
					if(i==0 && j==0) G2D_SetFirstBitBLTEData(G2D_RED);
					else G2D_SetNextBitBLTEData(G2D_RED);
				}
				else if(i>=(uG2dWidth/2) && (j<uG2dHeight/2)) G2D_SetNextBitBLTEData(G2D_GREEN);
				else if(i<(uG2dWidth/2) && j>=(uG2dHeight/2)) G2D_SetNextBitBLTEData(G2D_BLUE);
				else G2D_SetNextBitBLTEData(G2D_WHITE);
			}			
	
		G2D_WaitForInterrupt();
		UART_Printf("Alpha:%d\n", sAlphaVal);					
		if(sAlphaVal==0) break;
		UART_Getc();
		G2D_SetAlphaValue(255);
		G2D_DrawImage(eBpp);		
	}	
	G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);	
	UART_Printf("Press any key to continue!\n");
	UART_Getc();
	
	
#ifdef INTERRUPT_MODE	
	G2D_InterruptDisable();
	INTC_Disable(NUM_2D);
#endif

	G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);	
}

void TestHostToScrBitBLTWithClipping(void)
{
	u32 uG2dFbAddr, uG2dWidth, uG2dHeight;
	u32 i, j;
	CSPACE eBpp;
	u32 uDec;


#ifdef INTERRUPT_MODE
	INTC_SetVectAddr(NUM_2D, Isr_G2d);
	INTC_Enable(NUM_2D);
	G2D_InterruptEnable();
#endif

	G2D_GetFrmSz(&uG2dWidth, &uG2dHeight);
	G2D_GetFbAddr(&uG2dFbAddr);	
	G2D_GetSrcBppMode(&eBpp);
		
	UART_Printf("Width=%d, Height=%d, Framebuffer:0x%x\n", uG2dWidth, uG2dHeight, uG2dFbAddr);
	UART_Printf("Host to Screen BitBLT with ClippingTest, Press any key to continue!\n");
	G2D_ClearFrameEcolor(G2D_BLACK, 0, 0);
	UART_Getc();


	uDec=0;
	while(1){
		uDec+=8;
		if((uG2dWidth-2*uDec <=32)||(uG2dHeight-2*uDec <=32)) break;
		G2D_SetClippingWindow(0+uDec, 0+uDec, uG2dWidth-uDec, uG2dHeight-uDec);
		G2D_SetCoordinateDstBlock(0, 0, uG2dWidth-1, uG2dHeight-1);
		for(j=0; j<uG2dHeight; j++)
			for(i=0; i<uG2dWidth; i++) 
			{
				if(i<(uG2dWidth/2) && j<(uG2dHeight/2)) 
				{
					if(i==0 && j==0) G2D_SetFirstBitBLTEData(G2D_RED);
					else G2D_SetNextBitBLTEData(G2D_RED);
				}
				else if(i>=(uG2dWidth/2) && (j<uG2dHeight/2)) G2D_SetNextBitBLTEData(G2D_GREEN);
				else if(i<(uG2dWidth/2) && j>=(uG2dHeight/2)) G2D_SetNextBitBLTEData(G2D_BLUE);
				else G2D_SetNextBitBLTEData(G2D_WHITE);
			}			
	
		G2D_WaitForInterrupt();
		UART_Printf("[Clipping]x1=%d,y1=%d,x2=%d,y2=%d\n",uG2dWidth-uDec, uG2dHeight-uDec, uG2dWidth-uDec, uG2dHeight-uDec);
		UART_Getc();
		G2D_ClearFrameEcolor(G2D_BLACK, 0, 0);
		
	}
	G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);	
	UART_Printf("Press any key to continue!\n");
	UART_Getc();
	
#ifdef INTERRUPT_MODE	
	G2D_InterruptDisable();
	INTC_Disable(NUM_2D);
#endif

	G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);	
}


void TestHostToScrBitBLTWithTransparency(void)
{
	u32 i, j;
	u32 uG2dFbAddr, uG2dWidth, uG2dHeight;
	CSPACE eBpp;

	u8 uIsStretch=0;
	u8 bIsTransparent=0;

#ifdef INTERRUPT_MODE
	INTC_SetVectAddr(NUM_2D, Isr_G2d);
	INTC_Enable(NUM_2D);
	G2D_InterruptEnable();
#endif


	G2D_GetFrmSz(&uG2dWidth, &uG2dHeight);
	G2D_GetFbAddr(&uG2dFbAddr);	
	G2D_GetSrcBppMode(&eBpp);
		
	UART_Printf("Width=%d, Height=%d, Framebuffer:0x%x\n", uG2dWidth, uG2dHeight, uG2dFbAddr);

	G2D_DrawImage(eBpp);

	UART_Printf("BitBLT Test, Press any key to continue!\n");
	UART_Getc();

	
	UART_Printf("Opaque Mode!!\n");	
	bIsTransparent=0;
	G2D_SetTransparentMode(bIsTransparent, G2D_MAGENTA);


	G2D_SetCoordinateDstBlock(0, 0, uG2dWidth-1, uG2dHeight-1);
	for(j=0; j<uG2dHeight; j++)
		for(i=0; i<uG2dWidth; i++) 
		{
			if(i<(uG2dWidth/2) && j<(uG2dHeight/2)) 
			{
				if(i==0 && j==0) G2D_SetFirstBitBLTEData(G2D_RED);
				else G2D_SetNextBitBLTEData(G2D_RED);
			}
			else if(i>=(uG2dWidth/2) && (j<uG2dHeight/2)) G2D_SetNextBitBLTEData(G2D_MAGENTA);
			else if(i<(uG2dWidth/2) && j>=(uG2dHeight/2)) G2D_SetNextBitBLTEData(G2D_MAGENTA);
			else G2D_SetNextBitBLTEData(G2D_WHITE);
		}			
	
	G2D_WaitForInterrupt();

	UART_Printf("Press any key to continue!\n");
	UART_Getc();

	G2D_DrawImage(eBpp);
	UART_Printf("Transparent Mode! Press any key to continue!\n");	
	UART_Getc();	
	bIsTransparent=1;
	G2D_SetTransparentMode(bIsTransparent, G2D_YELLOW);
	
	G2D_SetCoordinateDstBlock(0, 0, uG2dWidth-1, uG2dHeight-1);
	for(j=0; j<uG2dHeight; j++)
		for(i=0; i<uG2dWidth; i++) 
		{
			if(i<(uG2dWidth/2) && j<(uG2dHeight/2)) 
			{
				if(i==0 && j==0) G2D_SetFirstBitBLTEData(G2D_RED);
				else G2D_SetNextBitBLTEData(G2D_RED);
			}
			else if(i>=(uG2dWidth/2) && (j<uG2dHeight/2)) G2D_SetNextBitBLTEData(G2D_YELLOW);
			else if(i<(uG2dWidth/2) && j>=(uG2dHeight/2)) G2D_SetNextBitBLTEData(G2D_YELLOW);
			else G2D_SetNextBitBLTEData(G2D_WHITE);
		}			
	
	G2D_WaitForInterrupt();	




	UART_Printf("Press any key to continue!\n");
	UART_Getc();
	G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);	


#ifdef INTERRUPT_MODE	
	G2D_InterruptDisable();
	INTC_Disable(NUM_2D);
#endif

	G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);	
}


void TestPatternMemoryRW(void)
{
	u32 i,j,k;
	u16 usPatternValue;
	u8 bErrorFlag;
	
	UART_Printf("Pattern Memory Write & Read Test!\n");
	bErrorFlag=false;
	for(i=0; i<8; i++)
		for(j=0; j<8; j++)
			for(k=0; k<=0xffff; k++) {
				G2D_SetPatternMemory(j, i, (u16)k);				
				G2D_GetPatternMemory(j, i, &usPatternValue);
				//UART_Printf("%x, %x\n", k, usPatternValue);
				if( (u16)k != usPatternValue) {
					UART_Printf("[%d,%d]Write:%x, Read:%x\n", j,i,k,usPatternValue);
					bErrorFlag=true;
				}	
			}
	if(!bErrorFlag) UART_Printf("Pattern Memory Write & Read Test OK!\n");		
}

void TestPatternMemoryOffset(void)
{
	u32 i,j,k;
	u32 uG2dFbAddr, uG2dWidth, uG2dHeight;
	CSPACE eBpp;


#ifdef INTERRUPT_MODE
	INTC_SetVectAddr(NUM_2D, Isr_G2d);
	INTC_Enable(NUM_2D);
	G2D_InterruptEnable();
#endif


	G2D_GetFrmSz(&uG2dWidth, &uG2dHeight);
	G2D_GetFbAddr(&uG2dFbAddr);	
	G2D_GetSrcBppMode(&eBpp);
		
	UART_Printf("Width=%d, Height=%d, Framebuffer:0x%x\n", uG2dWidth, uG2dHeight, uG2dFbAddr);
	
	UART_Printf("Pattern Memory Offset Test using Pixel Drawing, press any key to continue except 'x'!\n");

	G2D_ClearFrameEcolor(G2D_BLACK, 0, 0);
	G2D_RegisterPattern(pat8x8_rgbw);	
	G2D_Set3rdOperand(G2D_OPERAND3_PAT);
	G2D_SetRopEtype(ROP_3RD_OPRND_ONLY);

	for(k=0; k<64; k++) {
		G2D_SetPatternOffset((k%8),(k/8));
		UART_Printf("OffsetX: %d, OffsetY: %d\n", (k%8),(k/8));
		for(j=0; j<uG2dHeight; j++)
			for(i=0; i<uG2dWidth; i++)
				if(i<(uG2dWidth/2) && j<(uG2dHeight/2)) G2D_PutPixelEcolor(i, j, G2D_RED);
				else if(i>=(uG2dWidth/2) && (j<uG2dHeight/2)) G2D_PutPixelEcolor(i, j, G2D_GREEN);
				else if(i<(uG2dWidth/2) && j>=(uG2dHeight/2)) G2D_PutPixelEcolor(i, j, G2D_BLUE);
				else	 G2D_PutPixelEcolor(i, j, G2D_WHITE);
		if(UART_Getc() == 'x') break;
		G2D_ClearFrameEcolor(G2D_BLACK, 0, 0);
	}
	
#ifdef INTERRUPT_MODE	
	G2D_InterruptDisable();
	INTC_Disable(NUM_2D);
#endif

	G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);	

}


void TestClippingMaxScreen(void)
{
	u32 i,j,k;
	u32 uG2dFbAddr, uG2dWidth, uG2dHeight;
	CSPACE eBpp;
	u32 uRefCoord;
	s32 sCoordX, sCoordY;

#ifdef INTERRUPT_MODE
	INTC_SetVectAddr(NUM_2D, Isr_G2d);
	INTC_Enable(NUM_2D);
	G2D_InterruptEnable();
#endif


	G2D_GetFrmSz(&uG2dWidth, &uG2dHeight);
	G2D_GetFbAddr(&uG2dFbAddr);
	G2D_GetSrcBppMode(&eBpp);
	UART_Printf("Width=%d, Height=%d, Framebuffer:0x%x\n", uG2dWidth, uG2dHeight, uG2dFbAddr);

	
	G2D_ClearFrameEcolor(G2D_BLACK, 0, 0);
	UART_Printf("Clipping with Max Screen, press any key to continue\n");


	G2D_SetClippingWindow(0, 0, uG2dWidth-1, uG2dHeight-1);

	G2D_SetScreenSize(uG2dWidth, uG2dHeight);	
	for(k=0; k<uG2dWidth; k++) {
		G2D_SetClippingWindow(0, 0, uG2dWidth-1-k, uG2dHeight-1);
		G2D_SetScreenSize(uG2dWidth-k, uG2dHeight);
		UART_Printf("Max Screen X:%d.['x' to exit]\n", uG2dWidth-1-k);
		for(j=0; j<uG2dHeight; j++)
			for(i=0; i<uG2dWidth; i++)
				if(i<(uG2dWidth/2) && j<(uG2dHeight/2)) G2D_PutPixelEcolor(i, j, G2D_RED);
				else if(i>=(uG2dWidth/2) && (j<uG2dHeight/2)) G2D_PutPixelEcolor(i, j, G2D_GREEN);
				else if(i<(uG2dWidth/2) && j>=(uG2dHeight/2)) G2D_PutPixelEcolor(i, j, G2D_BLUE);
				else	 G2D_PutPixelEcolor(i, j, G2D_WHITE);
//		if(GetChar() == 'x') break;

//		for(j=0; j<uG2dHeight; j++)
//			for(i=0; i<uG2dWidth; i++)
//				G2D_PutPixelEcolor(i, j, (G2D_COLOR)((i*j)%8));
//		for(j=0; j<uG2dHeight; j++) {
//			for(i=0; i<uG2dWidth; i++)	{
//				G2D_GetRotateCoordinate(i, j, uOrgX, uOrgY, uRType, &uRsltX, &uRsltY);
//					if((uRsltX<uG2dWidth) && (uRsltY<uG2dHeight)) {
//						G2D_ConvertEcolorToRaw((G2D_COLOR)((i*j)%8), &uOrgData);
//						G2d_Convert24bpp(uOrgData, eBpp, 0, &uOrgDataConverted);
//						G2D_GetFbData(uRsltX, uRsltY, uG2dWidth, uG2dFbAddr, eBpp, &uFbData);
//						G2d_Convert24bpp(uFbData, eBpp, 0, &uFbDataConverted);
//						if((uOrgDataConverted&0xffffff) != (uFbDataConverted&0xffffff)) {
//							UART_Printf("BPP Change Error!\n");
//							bErrorFlag=1;
//						}
//					}
//				}
		G2D_ClearFrameEcolor(G2D_BLACK, 0, 0);
	}
	
	G2D_SetScreenSize(uG2dWidth, uG2dHeight);	
	for(k=0; k<uG2dHeight; k++) {
		G2D_SetClippingWindow(0, 0, uG2dWidth-1, uG2dHeight-1-k);
		G2D_SetScreenSize(uG2dWidth, uG2dHeight-k);
		UART_Printf("Max Screen Y:%d.['x' to exit]\n", uG2dHeight-1-k);
		for(j=0; j<uG2dHeight; j++)
			for(i=0; i<uG2dWidth; i++)
				if(i<(uG2dWidth/2) && j<(uG2dHeight/2)) G2D_PutPixelEcolor(i, j, G2D_RED);
				else if(i>=(uG2dWidth/2) && (j<uG2dHeight/2)) G2D_PutPixelEcolor(i, j, G2D_GREEN);
				else if(i<(uG2dWidth/2) && j>=(uG2dHeight/2)) G2D_PutPixelEcolor(i, j, G2D_BLUE);
				else	 G2D_PutPixelEcolor(i, j, G2D_WHITE);
//		if(GetChar() == 'x') break;
		G2D_ClearFrameEcolor(G2D_BLACK, 0, 0);
	}

	uRefCoord=(uG2dWidth>uG2dHeight) ? uG2dWidth : uG2dHeight;
	G2D_SetScreenSize(uG2dWidth, uG2dHeight);	
	for(k=0; k<uRefCoord; k++) {
		sCoordX=uG2dWidth-k;
		sCoordY=uG2dHeight-k;
		if(sCoordX<0) sCoordX=0;
		if(sCoordY<0) sCoordY=0;
		G2D_SetClippingWindow(0, 0, (u32)(sCoordX-1), (u32)(sCoordY-1));		
		G2D_SetScreenSize((u32)sCoordX, (u32)sCoordY);
		UART_Printf("Max Screen X:%d, Max Screen Y:%d.['x' to exit]\n", sCoordX, sCoordY);
		for(j=0; j<uG2dHeight; j++)
			for(i=0; i<uG2dWidth; i++)
				if(i<(uG2dWidth/2) && j<(uG2dHeight/2)) G2D_PutPixelEcolor(i, j, G2D_RED);
				else if(i>=(uG2dWidth/2) && (j<uG2dHeight/2)) G2D_PutPixelEcolor(i, j, G2D_GREEN);
				else if(i<(uG2dWidth/2) && j>=(uG2dHeight/2)) G2D_PutPixelEcolor(i, j, G2D_BLUE);
				else	 G2D_PutPixelEcolor(i, j, G2D_WHITE);
//		if(GetChar() == 'x') break;
		G2D_ClearFrameEcolor(G2D_BLACK, 0, 0);
	}

	UART_Printf("Press any key to exit!\n");
	UART_Getc();

#ifdef INTERRUPT_MODE	
	G2D_InterruptDisable();
	INTC_Disable(NUM_2D);
#endif

	G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);

}


void TestClippingWindow(void)
{
	u32 i,j,k;
	u32 uG2dFbAddr, uG2dWidth, uG2dHeight;
	CSPACE eBpp;
	u32 uRefCoord;
	s32 sCoordX1, sCoordX2, sCoordY1, sCoordY2;

#ifdef INTERRUPT_MODE
	INTC_SetVectAddr(NUM_2D, Isr_G2d);
	INTC_Enable(NUM_2D);
	G2D_InterruptEnable();
#endif


	G2D_GetFrmSz(&uG2dWidth, &uG2dHeight);
	G2D_GetFbAddr(&uG2dFbAddr);
	G2D_GetSrcBppMode(&eBpp);
	UART_Printf("Width=%d, Height=%d, Framebuffer:0x%x\n", uG2dWidth, uG2dHeight, uG2dFbAddr);

	
	G2D_ClearFrameEcolor(G2D_BLACK, 0, 0);
	UART_Printf("Clipping with Window!\n");

	G2D_SetScreenSize(uG2dWidth, uG2dHeight);
	UART_Printf("X-Direction Increment!\n");
	G2D_SetClippingWindow(0, 0, uG2dWidth-1, uG2dHeight-1);
	for(k=0; k<uG2dWidth; k++) {
		UART_Printf("Screen X1:%d.['x' to exit]\n", k);		
		G2D_SetClippingWindow(0+k, 0, uG2dWidth-1, uG2dHeight-1);
		for(j=0; j<uG2dHeight; j++)
			for(i=0; i<uG2dWidth; i++)
				if(i<(uG2dWidth/2) && j<(uG2dHeight/2)) G2D_PutPixelEcolor(i, j, G2D_RED);
				else if(i>=(uG2dWidth/2) && (j<uG2dHeight/2)) G2D_PutPixelEcolor(i, j, G2D_GREEN);
				else if(i<(uG2dWidth/2) && j>=(uG2dHeight/2)) G2D_PutPixelEcolor(i, j, G2D_BLUE);
				else	 G2D_PutPixelEcolor(i, j, G2D_WHITE);
//		if(GetChar() == 'x') break;
		G2D_ClearFrameEcolor(G2D_BLACK, 0, 0);
	}
	
	UART_Printf("Press any key to test X-Direction Increment!\n");
	UART_Getc();	
	G2D_SetClippingWindow(0, 0, uG2dWidth-1, uG2dHeight-1);	
	for(k=0; k<uG2dWidth; k++) {
		UART_Printf("Screen X2:%d.['x' to exit]\n", uG2dWidth-1-k);		
		G2D_SetClippingWindow(0, 0, uG2dWidth-1-k, uG2dHeight-1);
		for(j=0; j<uG2dHeight; j++)
			for(i=0; i<uG2dWidth; i++)
				if(i<(uG2dWidth/2) && j<(uG2dHeight/2)) G2D_PutPixelEcolor(i, j, G2D_RED);
				else if(i>=(uG2dWidth/2) && (j<uG2dHeight/2)) G2D_PutPixelEcolor(i, j, G2D_GREEN);
				else if(i<(uG2dWidth/2) && j>=(uG2dHeight/2)) G2D_PutPixelEcolor(i, j, G2D_BLUE);
				else	 G2D_PutPixelEcolor(i, j, G2D_WHITE);
//		if(GetChar() == 'x') break;
		G2D_ClearFrameEcolor(G2D_BLACK, 0, 0);
	}	
	UART_Printf("Press any key to test X-Direction Increment & Decrement!\n");
	UART_Getc();
	G2D_SetClippingWindow(0, 0, uG2dWidth-1, uG2dHeight-1);	
	for(k=0; k<uG2dWidth; k++) {
		sCoordX1=k;
		sCoordX2=uG2dWidth-1-k;
		if(k> uG2dWidth/2-1) sCoordX1=sCoordX2=uG2dWidth/2;
		UART_Printf("Screen X1:%d, Screen X2:%d.['x' to exit]\n", (u32)sCoordX1, (u32)sCoordX2);		
		G2D_SetClippingWindow((u32)sCoordX1, 0, (u32)sCoordX2, uG2dHeight-1);
		for(j=0; j<uG2dHeight; j++)
			for(i=0; i<uG2dWidth; i++)
				if(i<(uG2dWidth/2) && j<(uG2dHeight/2)) G2D_PutPixelEcolor(i, j, G2D_RED);
				else if(i>=(uG2dWidth/2) && (j<uG2dHeight/2)) G2D_PutPixelEcolor(i, j, G2D_GREEN);
				else if(i<(uG2dWidth/2) && j>=(uG2dHeight/2)) G2D_PutPixelEcolor(i, j, G2D_BLUE);
				else	 G2D_PutPixelEcolor(i, j, G2D_WHITE);
//		if(GetChar() == 'x') break;		
		G2D_ClearFrameEcolor(G2D_BLACK, 0, 0);
	}
	
	UART_Printf("Press any key to test Y-Direction Increment!\n");
	UART_Getc();
	G2D_SetClippingWindow(0, 0, uG2dWidth-1, uG2dHeight-1);
	for(k=0; k<uG2dHeight; k++) {
		UART_Printf("Screen Y1:%d.['x' to exit]\n", k);		
		G2D_SetClippingWindow(0, 0+k, uG2dWidth-1, uG2dHeight-1);
		for(j=0; j<uG2dHeight; j++)
			for(i=0; i<uG2dWidth; i++)
				if(i<(uG2dWidth/2) && j<(uG2dHeight/2)) G2D_PutPixelEcolor(i, j, G2D_RED);
				else if(i>=(uG2dWidth/2) && (j<uG2dHeight/2)) G2D_PutPixelEcolor(i, j, G2D_GREEN);
				else if(i<(uG2dWidth/2) && j>=(uG2dHeight/2)) G2D_PutPixelEcolor(i, j, G2D_BLUE);
				else	 G2D_PutPixelEcolor(i, j, G2D_WHITE);
//		if(GetChar() == 'x') break;
		G2D_ClearFrameEcolor(G2D_BLACK, 0, 0);
	}	
	UART_Printf("Press any key to test Y-Direction Decrement!\n");
	UART_Getc();	
	G2D_SetClippingWindow(0, 0, uG2dWidth-1, uG2dHeight-1);	
	for(k=0; k<uG2dHeight; k++) {
		UART_Printf("Screen Y2:%d.['x' to exit]\n", uG2dHeight-1-k);		
		G2D_SetClippingWindow(0, 0, uG2dWidth-1, uG2dHeight-1-k);
		for(j=0; j<uG2dHeight; j++)
			for(i=0; i<uG2dWidth; i++)
				if(i<(uG2dWidth/2) && j<(uG2dHeight/2)) G2D_PutPixelEcolor(i, j, G2D_RED);
				else if(i>=(uG2dWidth/2) && (j<uG2dHeight/2)) G2D_PutPixelEcolor(i, j, G2D_GREEN);
				else if(i<(uG2dWidth/2) && j>=(uG2dHeight/2)) G2D_PutPixelEcolor(i, j, G2D_BLUE);
				else	 G2D_PutPixelEcolor(i, j, G2D_WHITE);
//		if(GetChar() == 'x') break;
		G2D_ClearFrameEcolor(G2D_BLACK, 0, 0);
	}	
	UART_Printf("Press any key to test Y-Direction Increment & Decrement!\n");
	UART_Getc();
	G2D_SetClippingWindow(0, 0, uG2dWidth-1, uG2dHeight-1);	
	for(k=0; k<uG2dHeight; k++) {
		sCoordY1=k;
		sCoordY2=uG2dHeight-1-k;
		if(k> uG2dHeight/2-1) sCoordY1=sCoordY2=uG2dHeight/2;
		UART_Printf("Screen Y1:%d, Screen Y2:%d.['x' to exit]\n", (u32)sCoordY1, (u32)sCoordY2);
		G2D_SetClippingWindow(0, (u32)sCoordY1, uG2dWidth-1, (u32)sCoordY2);
		for(j=0; j<uG2dHeight; j++)
			for(i=0; i<uG2dWidth; i++)
				if(i<(uG2dWidth/2) && j<(uG2dHeight/2)) G2D_PutPixelEcolor(i, j, G2D_RED);
				else if(i>=(uG2dWidth/2) && (j<uG2dHeight/2)) G2D_PutPixelEcolor(i, j, G2D_GREEN);
				else if(i<(uG2dWidth/2) && j>=(uG2dHeight/2)) G2D_PutPixelEcolor(i, j, G2D_BLUE);
				else	 G2D_PutPixelEcolor(i, j, G2D_WHITE);
//		if(GetChar() == 'x') break;		
		G2D_ClearFrameEcolor(G2D_BLACK, 0, 0);
	}

	UART_Printf("Press any key to test X/Y-Direction Increment & Decrement!\n");
	UART_Getc();	
	G2D_SetClippingWindow(0, 0, uG2dWidth-1, uG2dHeight-1);	
	uRefCoord=(uG2dWidth>uG2dHeight) ? uG2dWidth : uG2dHeight;
	for(k=0; k<uRefCoord; k++) {
		sCoordX1=sCoordY1=k;
		sCoordX2=uG2dWidth-1-k;
		sCoordY2=uG2dHeight-1-k;
		if(k>uG2dWidth/2-1) sCoordX1=sCoordX2=uG2dWidth/2;
		if(k>uG2dHeight/2-1) sCoordY1=sCoordY2=uG2dHeight/2;		
		UART_Printf("Screen X1:%d, Screen X2:%d, Screen Y1:%d, Screen Y2:%d.['x' to exit]\n", (u32)sCoordX1, (u32)sCoordX2, (u32)sCoordY1, (u32)sCoordY2);		
		G2D_SetClippingWindow((u32)sCoordX1, (u32)sCoordY1, (u32)sCoordX2, (u32)sCoordY2);
		for(j=0; j<uG2dHeight; j++)
			for(i=0; i<uG2dWidth; i++)
				if(i<(uG2dWidth/2) && j<(uG2dHeight/2)) G2D_PutPixelEcolor(i, j, G2D_RED);
				else if(i>=(uG2dWidth/2) && (j<uG2dHeight/2)) G2D_PutPixelEcolor(i, j, G2D_GREEN);
				else if(i<(uG2dWidth/2) && j>=(uG2dHeight/2)) G2D_PutPixelEcolor(i, j, G2D_BLUE);
				else	 G2D_PutPixelEcolor(i, j, G2D_WHITE);
//		if(GetChar() == 'x') break;		
		G2D_ClearFrameEcolor(G2D_BLACK, 0, 0);
	}

	UART_Printf("Press any key to exit!\n");
	UART_Getc();

#ifdef INTERRUPT_MODE	
	G2D_InterruptDisable();
	INTC_Disable(NUM_2D);
#endif

	G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);

}

void TestRopAllOperation(void)
{
	u32 i,j,k;
	u32 uG2dFbAddr, uG2dWidth, uG2dHeight;
	CSPACE eBpp;
	u8 ucPatOffsetX, ucPatOffsetY;
	u16 usThirdDataOrg;
	u32 uSrcData, uDstData, uThirdDataConverted, uRopData;
	u8 bErrorFlag=0;


#ifdef INTERRUPT_MODE
	INTC_SetVectAddr(NUM_2D, Isr_G2d);
	INTC_Enable(NUM_2D);
	G2D_InterruptEnable();
#endif


	G2D_GetFrmSz(&uG2dWidth, &uG2dHeight);
	G2D_GetFbAddr(&uG2dFbAddr);
	G2D_GetSrcBppMode(&eBpp);
	UART_Printf("Width=%d, Height=%d, Framebuffer:0x%x\n", uG2dWidth, uG2dHeight, uG2dFbAddr);

	
	G2D_ClearFrameEcolor(G2D_CYAN, 0, 0);
	UART_Printf("ROP with All Operations!\n");

	UART_Printf("Third Operand is PATTERN, press any key to continue!\n");
	G2D_RegisterPattern(pat8x8_rgbw);	
	G2D_Set3rdOperand(G2D_OPERAND3_PAT);
	UART_Getc();

	for(k=239; k<=0xff; k++) {
		G2D_SetRop(k);
		UART_Printf("ROP Value=%d\n", k);
		for(j=0; j<uG2dHeight; j++)
			for(i=0; i<uG2dWidth; i++) {
				G2D_GetFbData(i, j, uG2dWidth, uG2dFbAddr, eBpp, &uDstData); // before execution ROP
				if(i<(uG2dWidth/2) && j<(uG2dHeight/2)) {
					G2D_PutPixelEcolor(i, j, G2D_RED);
					G2D_ConvertEcolorToRaw(G2D_RED, &uSrcData);
				}	
				else if(i>=(uG2dWidth/2) && (j<uG2dHeight/2)) {
					G2D_PutPixelEcolor(i, j, G2D_GREEN);
					G2D_ConvertEcolorToRaw(G2D_GREEN, &uSrcData);
				}	
				else if(i<(uG2dWidth/2) && j>=(uG2dHeight/2)) {
					G2D_PutPixelEcolor(i, j, G2D_BLUE);
					G2D_ConvertEcolorToRaw(G2D_BLUE, &uSrcData);
				}	
				else	 {
					G2D_PutPixelEcolor(i, j, G2D_WHITE);
					G2D_ConvertEcolorToRaw(G2D_WHITE, &uSrcData);
				}
				G2D_GetPatternOffset(&ucPatOffsetX, &ucPatOffsetY);
				G2D_GetPatternMemory((i+ucPatOffsetX)%8, (j+ucPatOffsetY)%8, &usThirdDataOrg);
				G2D_ConvertPatternToRaw(usThirdDataOrg, eBpp, &uThirdDataConverted);
				G2D_RopOut(uSrcData, uDstData, uThirdDataConverted, (u8)k, eBpp, &uRopData);				
				G2D_GetFbData(i, j, uG2dWidth, uG2dFbAddr, eBpp, &uDstData); // after execution ROP
				if((uRopData&0xffffff) != (uDstData&0xffffff)) {
					UART_Printf("ROP Error[%d, %d]\n", i,j);
					bErrorFlag=1;
				}	
			}	
		if(UART_GetKey()) break;				
		G2D_ClearFrameEcolor(G2D_CYAN, 0, 0);
	}


/*	
	G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);
	G2D_ClearFrameEcolor(G2D_MAGENTA, 0, 0);
	UART_Printf("Third Operand is Foreground, press any key to continue!\n");
	G2D_Set3rdOperand(G2D_OPERAND3_FG);
	GetChar();

	uShrinkWidth=20;
	uShrinkHeight=20;
	G2D_SetRop(ROP_SRC_AND_3RD_OPRND);
	for(i=0; i<= 0xffffff; i++) {
		G2D_SetFgColor(i);
		UART_Printf("FG Color=%x\n", i);
		for(j=0; j<uShrinkHeight; j++)
			for(i=0; i<uShrinkWidth; i++) {
				G2D_GetFbData(i, j, uShrinkWidth, uG2dFbAddr, eBpp, &uDstData); // before execution ROP
				if(i<(uShrinkWidth/2) && j<(uShrinkHeight/2)) {
					G2D_PutPixelEcolor(i, j, G2D_RED);
					G2D_ConvertEcolorToRaw(G2D_RED, &uSrcData);
				}	
				else if(i>=(uShrinkWidth/2) && (j<uShrinkHeight/2)) {
					G2D_PutPixelEcolor(i, j, G2D_GREEN);
					G2D_ConvertEcolorToRaw(G2D_GREEN, &uSrcData);
				}	
				else if(i<(uShrinkWidth/2) && j>=(uShrinkHeight/2)) {
					G2D_PutPixelEcolor(i, j, G2D_BLUE);
					G2D_ConvertEcolorToRaw(G2D_BLUE, &uSrcData);
				}	
				else	 {
					G2D_PutPixelEcolor(i, j, G2D_WHITE);
					G2D_ConvertEcolorToRaw(G2D_WHITE, &uSrcData);
				}
				G2D_GetPatternOffset(&ucPatOffsetX, &ucPatOffsetY);
				G2D_GetPatternMemory((i+ucPatOffsetX)%8, (j+ucPatOffsetY)%8, &usThirdDataOrg);
				G2D_ConvertPatternToRaw(usThirdDataOrg, eBpp, &uThirdDataConverted);
				G2D_RopOut(uSrcData, uDstData, uThirdDataConverted, (u8)k, eBpp, &uRopData);				
				G2D_GetFbData(i, j, uG2dWidth, uG2dFbAddr, eBpp, &uDstData); // after execution ROP
				if(uRopData != uDstData) {
					UART_Printf("ROP Error[%d, %d]\n", i,j);
					bErrorFlag=1;
				}	
			}	
		if(UART_GetKey()) break;				
		G2D_ClearFrameEcolor(G2D_CYAN, 0, 0);

	}	
*/
	if(!bErrorFlag) UART_Printf("ROP with All Operation Test OK!!\n");
	UART_Printf("Press any key to exit!\n");
	UART_Getc();

#ifdef INTERRUPT_MODE	
	G2D_InterruptDisable();
	INTC_Disable(NUM_2D);
#endif

	G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);

}

void TestRopWithThirdOperandChange(void)
{
	u32 uG2dFbAddr, uG2dWidth, uG2dHeight;
	CSPACE eBpp;
	u8 bIsStretch=0;

#ifdef INTERRUPT_MODE
	INTC_SetVectAddr(NUM_2D, Isr_G2d);
	INTC_Enable(NUM_2D);
	G2D_InterruptEnable();
#endif


	G2D_GetFrmSz(&uG2dWidth, &uG2dHeight);
	G2D_GetFbAddr(&uG2dFbAddr);
	G2D_GetSrcBppMode(&eBpp);
	UART_Printf("Width=%d, Height=%d, Framebuffer:0x%x\n", uG2dWidth, uG2dHeight, uG2dFbAddr);
	
	G2D_DrawImage(eBpp);
	UART_Printf("Thire Operand Change, press any key to continue!\n");
	UART_Getc();

	UART_Printf("Third Operand=Pattern, ROP= Source OR Third Operand, Press any key to continue\n");
	G2D_RegisterPattern(pat8x8_rgbw);	
	G2D_Set3rdOperand(G2D_OPERAND3_PAT);
	G2D_SetRop(ROP_SRC_OR_3RD_OPRND);
	G2D_BitBlt(0, 0, uG2dWidth-1, uG2dHeight-1, 0, 0, uG2dWidth-1, uG2dHeight-1, bIsStretch);
	UART_Getc();
	G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);

	UART_Printf("Third Operand=Pattern, ROP= Source OR Third Operand, Press any key to continue\n");
	G2D_DrawImage(eBpp);
	G2D_RegisterPattern(pat8x8_outline);	
	G2D_Set3rdOperand(G2D_OPERAND3_PAT);
	G2D_SetRop(ROP_SRC_OR_3RD_OPRND);
	G2D_BitBlt(0, 0, uG2dWidth-1, uG2dHeight-1, 0, 0, uG2dWidth-1, uG2dHeight-1, bIsStretch);	
	UART_Getc();
	G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);

	G2D_DrawImage(eBpp);
	UART_Printf("Thire Operand Change, press any key to continue!\n");
	UART_Getc();

	UART_Printf("Third Operand=FOREGROUND, ROP= Source OR Third Operand, Press any key to continue\n");
	G2D_SetFgEcolor(G2D_MAGENTA);
	G2D_Set3rdOperand(G2D_OPERAND3_FG);
	G2D_SetRop(ROP_SRC_OR_3RD_OPRND);
	G2D_BitBlt(0, 0, uG2dWidth-1, uG2dHeight-1, 0, 0, uG2dWidth-1, uG2dHeight-1, bIsStretch);
	UART_Getc();
	G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);

	UART_Printf("Third Operand=FOREGROUND, ROP= Source OR Third Operand, Press any key to exit\n");
	G2D_DrawImage(eBpp);
	G2D_SetFgEcolor(G2D_CYAN);
	G2D_Set3rdOperand(G2D_OPERAND3_FG);
	G2D_SetRop(ROP_SRC_OR_3RD_OPRND);
	G2D_BitBlt(0, 0, uG2dWidth-1, uG2dHeight-1, 0, 0, uG2dWidth-1, uG2dHeight-1, bIsStretch);	
	UART_Getc();


#ifdef INTERRUPT_MODE	
	G2D_InterruptDisable();
	INTC_Disable(NUM_2D);
#endif

	G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);

}


void TestBlendingWithPerPlane(void)
{
	u32 i,j,k;
	u32 uG2dFbAddr, uG2dWidth, uG2dHeight;
	CSPACE eBpp;
	u32 uSrcDataOrg, uDstDataOrg, uSrcDataConverted, uDstDataConverted, uBlendingOut;
	u8 bErrorFlag=0;

#ifdef INTERRUPT_MODE
	INTC_SetVectAddr(NUM_2D, Isr_G2d);
	INTC_Enable(NUM_2D);
	G2D_InterruptEnable();
#endif

	LCD_Stop();


	G2D_GetFrmSz(&uG2dWidth, &uG2dHeight);
	G2D_GetFbAddr(&uG2dFbAddr);
	G2D_GetSrcBppMode(&eBpp);
	UART_Printf("Width=%d, Height=%d, Framebuffer:0x%x\n", uG2dWidth, uG2dHeight, uG2dFbAddr);

	
	G2D_ClearFrameEcolor(G2D_MAGENTA, 0, 0);
	UART_Printf("Blending with Per Plane, Press any key to continue\n");


	G2D_SetAlphaMode(G2D_ALPHA_MODE);
	
// 071020	GetChar();

	for(k=0; k<=0xff; k++) {
		G2D_SetAlphaValue(k);
		UART_Printf("Alpha Value=%d\n", k);
		for(j=0; j<uG2dHeight; j++)
			for(i=0; i<uG2dWidth; i++) {
				G2D_GetFbData(i, j, uG2dWidth, uG2dFbAddr, eBpp, &uDstDataOrg); // before execution ROP
				if(i<(uG2dWidth/2) && j<(uG2dHeight/2)) {
					G2D_PutPixelEcolor(i, j, G2D_RED);
					G2D_ConvertEcolorToRaw(G2D_RED, &uSrcDataOrg);
				}	
				else if(i>=(uG2dWidth/2) && (j<uG2dHeight/2)) {
					G2D_PutPixelEcolor(i, j, G2D_GREEN);
					G2D_ConvertEcolorToRaw(G2D_GREEN, &uSrcDataOrg);
				}	
				else if(i<(uG2dWidth/2) && j>=(uG2dHeight/2)) {
					G2D_PutPixelEcolor(i, j, G2D_BLUE);
					G2D_ConvertEcolorToRaw(G2D_BLUE, &uSrcDataOrg);
				}	
				else	 {
					G2D_PutPixelEcolor(i, j, G2D_WHITE);
					G2D_ConvertEcolorToRaw(G2D_WHITE, &uSrcDataOrg);
				}
//				G2D_GetPatternOffset(&ucPatOffsetX, &ucPatOffsetY);
//				G2D_GetPatternMemory((i+ucPatOffsetX)%8, (j+ucPatOffsetY)%8, &usThirdDataOrg);
//				G2D_ConvertPatternToRaw(usThirdDataOrg, eBpp, &uThirdDataConverted);
				G2d_Convert24bpp(uSrcDataOrg, eBpp, 1, &uSrcDataConverted);
				G2d_Convert24bpp(uDstDataOrg, eBpp, 1, &uDstDataConverted);
				G2D_BlendingOut(uSrcDataConverted, uDstDataConverted, (u8)k, false, 0, &uBlendingOut);
				//G2D_RopOut(uSrcData, uDstData, uThirdDataConverted, (u8)k, eBpp, &uRopData);				
				G2D_GetFbData(i, j, uG2dWidth, uG2dFbAddr, eBpp, &uDstDataOrg); // after execution ROP
				G2d_Convert24bpp(uDstDataOrg, eBpp, 1, &uDstDataConverted); 
				if((uBlendingOut&0xffffff) != (uDstDataConverted&0xffffff)) {
					UART_Printf("Alpha Error[%d, %d]\n", i,j);
					bErrorFlag=1;
				}	
			}	
		if(UART_GetKey()) break;				
		G2D_ClearFrameEcolor(G2D_MAGENTA, 0, 0);
	}


	if(!bErrorFlag) UART_Printf("Blending with Per Plane Test OK!!\n");
	UART_Printf("Press any key to exit!\n");
// 071020	GetChar();

#ifdef INTERRUPT_MODE	
	G2D_InterruptDisable();
	INTC_Disable(NUM_2D);
#endif

	G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);

}


void TestBlendingWithPerPixel(void)
{
	u32 i,j,k;
	u32 uG2dFbAddr, uG2dWidth, uG2dHeight;
	CSPACE eBpp, eBppBackup;
	u32 uSrcDataOrg, uDstDataOrg, uSrcDataConverted, uDstDataConverted, uBlendingOut;
	u8 bErrorFlag=0;
	u32 uCyanRaw;

#ifdef INTERRUPT_MODE
	INTC_SetVectAddr(NUM_2D, Isr_G2d);
	INTC_Enable(NUM_2D);
	G2D_InterruptEnable();
#endif

	G2D_GetFrmSz(&uG2dWidth, &uG2dHeight);
	G2D_GetFbAddr(&uG2dFbAddr);
	G2D_GetSrcBppMode(&eBpp);
	UART_Printf("Width=%d, Height=%d, Framebuffer:0x%x\n", uG2dWidth, uG2dHeight, uG2dFbAddr);

	eBppBackup=eBpp;
	eBpp=ARGB24;
	G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);
	
	G2D_ClearFrameEcolor(G2D_MAGENTA, 0, 0);
	UART_Printf("Blending with Per Pixel, Press any key to continue!\n");

	G2D_SetAlphaValue(0);
// 071020	GetChar();

	G2D_ConvertEcolorToRaw(G2D_CYAN, &uCyanRaw);

	for(k=0; k<=0xff; k++) {
		UART_Printf("Alpha=%d\n",k);
		G2D_SetAlphaMode(G2D_NO_ALPHA_MODE);
		for(j=0; j<uG2dHeight/2; j++) // Fill Source
			for(i=0; i<uG2dWidth/2; i++) {
				G2D_ConvertEcolorToRaw((G2D_COLOR)(i%8), &uSrcDataOrg);
				G2D_SetFbData(i, j, uG2dWidth, uG2dFbAddr, eBpp, (uSrcDataOrg|(k<<24)));
			}	
			
		for(j=uG2dHeight/2; j<uG2dHeight; j++) // Fill Destination
			for(i=uG2dWidth/2; i<uG2dWidth; i++) G2D_PutPixelEcolor(i, j, G2D_CYAN);

		G2D_SetAlphaMode(G2D_PP_ALPHA_SOURCE_MODE);		
		G2D_BitBlt(0, 0, uG2dWidth/2-1, uG2dHeight/2-1, uG2dWidth/2, uG2dHeight/2, uG2dWidth-1, uG2dHeight-1, 0);

		for(j=0; j<uG2dHeight/2; j++) //Compare
			for(i=0; i<uG2dWidth/2; i++) {
				G2D_ConvertEcolorToRaw((G2D_COLOR)(i%8), &uSrcDataOrg);
				G2d_Convert24bpp(uSrcDataOrg, eBpp, 1, &uSrcDataConverted);
				G2d_Convert24bpp(uCyanRaw, eBpp, 1, &uDstDataConverted);
				G2D_BlendingOut(uSrcDataConverted, uDstDataConverted, (u8)k, false, 0, &uBlendingOut);
				G2D_GetFbData(i+uG2dWidth/2, j+uG2dHeight/2, uG2dWidth, uG2dFbAddr, eBpp, &uDstDataOrg);
				G2d_Convert24bpp(uDstDataOrg, eBpp, 1, &uDstDataConverted);
				if((uBlendingOut&0xffffff) != (uDstDataConverted&0xffffff)) {
					UART_Printf("Blending Error[%d,%d,%d]\n", k, i, j);
				}	
			}
		if(UART_GetKey()) break;
	
	}


	if(!bErrorFlag) UART_Printf("Blending with Per Pixel Test OK!!\n");
	UART_Printf("Press any key to exit!\n");
// 071020	GetChar();

#ifdef INTERRUPT_MODE	
	G2D_InterruptDisable();
	INTC_Disable(NUM_2D);
#endif

	eBpp=eBppBackup;
	G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);

}

void TestFadingWithPerPlane(void)
{
	u32 i,j,k;
	u32 uG2dFbAddr, uG2dWidth, uG2dHeight;
	CSPACE eBpp;
	u32 uSrcDataOrg, uDstDataOrg, uSrcDataConverted, uDstDataConverted, uBlendingOut;
	u8 bErrorFlag=0;
	u8 ucAlphaValue;

#ifdef INTERRUPT_MODE
	INTC_SetVectAddr(NUM_2D, Isr_G2d);
	INTC_Enable(NUM_2D);
	G2D_InterruptEnable();
#endif

	G2D_GetFrmSz(&uG2dWidth, &uG2dHeight);
	G2D_GetFbAddr(&uG2dFbAddr);
	G2D_GetSrcBppMode(&eBpp);
	UART_Printf("Width=%d, Height=%d, Framebuffer:0x%x\n", uG2dWidth, uG2dHeight, uG2dFbAddr);

	
	G2D_ClearFrameEcolor(G2D_MAGENTA, 0, 0);
	UART_Printf("Fading with Per Plane, Press any key to continue\n");


	G2D_SetAlphaMode(G2D_FADING_MODE);
	ucAlphaValue=128;
	G2D_SetAlphaValue(ucAlphaValue);
// 071020	GetChar();

	for(k=0; k<=0xff; k++) {
		G2D_SetFadingValue(k);
		UART_Printf("Fading Value=%d\n", k);
		for(j=0; j<uG2dHeight; j++)
			for(i=0; i<uG2dWidth; i++) {
				G2D_GetFbData(i, j, uG2dWidth, uG2dFbAddr, eBpp, &uDstDataOrg); // before execution ROP
				if(i<(uG2dWidth/2) && j<(uG2dHeight/2)) {
					G2D_PutPixelEcolor(i, j, G2D_RED);
					G2D_ConvertEcolorToRaw(G2D_RED, &uSrcDataOrg);
				}	
				else if(i>=(uG2dWidth/2) && (j<uG2dHeight/2)) {
					G2D_PutPixelEcolor(i, j, G2D_GREEN);
					G2D_ConvertEcolorToRaw(G2D_GREEN, &uSrcDataOrg);
				}	
				else if(i<(uG2dWidth/2) && j>=(uG2dHeight/2)) {
					G2D_PutPixelEcolor(i, j, G2D_BLUE);
					G2D_ConvertEcolorToRaw(G2D_BLUE, &uSrcDataOrg);
				}	
				else	 {
					G2D_PutPixelEcolor(i, j, G2D_WHITE);
					G2D_ConvertEcolorToRaw(G2D_WHITE, &uSrcDataOrg);
				}
//				G2D_GetPatternOffset(&ucPatOffsetX, &ucPatOffsetY);
//				G2D_GetPatternMemory((i+ucPatOffsetX)%8, (j+ucPatOffsetY)%8, &usThirdDataOrg);
//				G2D_ConvertPatternToRaw(usThirdDataOrg, eBpp, &uThirdDataConverted);
				G2d_Convert24bpp(uSrcDataOrg, eBpp, 1, &uSrcDataConverted);
				G2d_Convert24bpp(uDstDataOrg, eBpp, 1, &uDstDataConverted);
				G2D_BlendingOut(uSrcDataConverted, uDstDataConverted, (u8)ucAlphaValue, true, (u8)k, &uBlendingOut);
				//G2D_RopOut(uSrcData, uDstData, uThirdDataConverted, (u8)k, eBpp, &uRopData);				
				G2D_GetFbData(i, j, uG2dWidth, uG2dFbAddr, eBpp, &uDstDataOrg); // after execution ROP
				G2d_Convert24bpp(uDstDataOrg, eBpp, 1, &uDstDataConverted); 
				if((uBlendingOut&0xffffff) != (uDstDataConverted&0xffffff)) {
					UART_Printf("Alpha Error[%d, %d]\n", i,j);
					bErrorFlag=1;
				}	
			}	
		if(UART_GetKey()) break;				
//		G2D_ClearFrameEcolor(G2D_MAGENTA, 0, 0);
	}


	if(!bErrorFlag) UART_Printf("Fading with Per Plane Test OK!!\n");
	UART_Printf("Press any key to exit!\n");
// 071020	GetChar();

#ifdef INTERRUPT_MODE	
	G2D_InterruptDisable();
	INTC_Disable(NUM_2D);
#endif

	G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);

}


void TestChangeBpp(void)
{
	u32 i,j;
	u32 uG2dFbAddr, uG2dWidth, uG2dHeight;
	CSPACE eBpp, eBppBackup;
	u32 uOrgData, uOrgDataConverted;
	u32 uFbData, uFbDataConverted;
	u8 bErrorFlag=0;


#ifdef INTERRUPT_MODE
	INTC_SetVectAddr(NUM_2D, Isr_G2d);
	INTC_Enable(NUM_2D);
	G2D_InterruptEnable();
#endif


	G2D_GetFrmSz(&uG2dWidth, &uG2dHeight);
	G2D_GetFbAddr(&uG2dFbAddr);
	G2D_GetSrcBppMode(&eBpp);
	eBppBackup=eBpp;
	UART_Printf("Width=%d, Height=%d, Framebuffer:0x%x\n", uG2dWidth, uG2dHeight, uG2dFbAddr);

	UART_Printf("Change BPP!\n");

	LCD_Stop();

// RGB565
	eBpp=RGB16;
	bErrorFlag=0;
	G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);

	for(j=0; j<uG2dHeight; j++)
		for(i=0; i<uG2dWidth; i++)
			G2D_PutPixelEcolor(i, j, (G2D_COLOR)((i*j)%8));
		
	for(j=0; j<uG2dHeight; j++)
		for(i=0; i<uG2dWidth; i++)	{
			G2D_ConvertEcolorToRaw((G2D_COLOR)((i*j)%8), &uOrgData);
			G2d_Convert24bpp(uOrgData, eBpp, 1, &uOrgDataConverted);
			G2D_GetFbData(i, j, uG2dWidth, uG2dFbAddr, eBpp, &uFbData);
			G2d_Convert24bpp(uFbData, eBpp, 1, &uFbDataConverted);
			if(uOrgDataConverted != uFbDataConverted) {
				UART_Printf("BPP Change Error!\n");
				bErrorFlag=1;
			}
		if(bErrorFlag) break;			
		}

	if(!bErrorFlag) UART_Printf("16 Bpp RGB565 is OK!\n");


// RGBA 5551
	eBpp=RGBA16;
	bErrorFlag=0;
	G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);

	for(j=0; j<uG2dHeight; j++)
		for(i=0; i<uG2dWidth; i++)
			G2D_PutPixelEcolor(i, j, (G2D_COLOR)((i*j)%8));
		
	for(j=0; j<uG2dHeight; j++)
		for(i=0; i<uG2dWidth; i++)	{
			G2D_ConvertEcolorToRaw((G2D_COLOR)((i*j)%8), &uOrgData);
			G2d_Convert24bpp(uOrgData, eBpp, 1, &uOrgDataConverted);
			G2D_GetFbData(i, j, uG2dWidth, uG2dFbAddr, eBpp, &uFbData);
			G2d_Convert24bpp(uFbData, eBpp, 1, &uFbDataConverted);
			if(uOrgDataConverted != uFbDataConverted) {
				UART_Printf("BPP Change Error!\n");
				bErrorFlag=1;
			}
		if(bErrorFlag) break;			
		}

	if(!bErrorFlag) UART_Printf("16 Bpp RGBA5551 is OK!\n");
	

// RGBA 1555
	eBpp=ARGB16;
	bErrorFlag=0;
	G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);

	for(j=0; j<uG2dHeight; j++)
		for(i=0; i<uG2dWidth; i++)
			G2D_PutPixelEcolor(i, j, (G2D_COLOR)((i*j)%8));
		
	for(j=0; j<uG2dHeight; j++)
		for(i=0; i<uG2dWidth; i++)	{
			G2D_ConvertEcolorToRaw((G2D_COLOR)((i*j)%8), &uOrgData);
			G2d_Convert24bpp(uOrgData, eBpp, 1, &uOrgDataConverted);
			G2D_GetFbData(i, j, uG2dWidth, uG2dFbAddr, eBpp, &uFbData);
			G2d_Convert24bpp(uFbData, eBpp, 1, &uFbDataConverted);
			if(uOrgDataConverted != uFbDataConverted) {
				UART_Printf("BPP Change Error!\n");
				bErrorFlag=1;
			}
		if(bErrorFlag) break;			
		}

	if(!bErrorFlag) UART_Printf("16 Bpp ARGB1555 is OK!\n");
	
// RGBA 8888
	eBpp=RGBA24;
	bErrorFlag=0;
	G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);

	for(j=0; j<uG2dHeight; j++)
		for(i=0; i<uG2dWidth; i++)
			G2D_PutPixelEcolor(i, j, (G2D_COLOR)((i*j)%8));
		
	for(j=0; j<uG2dHeight; j++)
		for(i=0; i<uG2dWidth; i++)	{
			G2D_ConvertEcolorToRaw((G2D_COLOR)((i*j)%8), &uOrgData);
			G2d_Convert24bpp(uOrgData, eBpp, 1, &uOrgDataConverted);
			G2D_GetFbData(i, j, uG2dWidth, uG2dFbAddr, eBpp, &uFbData);
			G2d_Convert24bpp(uFbData, eBpp, 1, &uFbDataConverted);
			if((uOrgDataConverted&0xffffff00) != (uFbDataConverted&0xffffff00)) {
				UART_Printf("BPP Change Error!\n");
				bErrorFlag=1;
			}
		if(bErrorFlag) break;			
		}

	if(!bErrorFlag) UART_Printf("24 Bpp RGBA8888 is OK!\n");	
	
// ARGB 8888
	eBpp=ARGB24;
	bErrorFlag=0;
	G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);

	for(j=0; j<uG2dHeight; j++)
		for(i=0; i<uG2dWidth; i++)
			G2D_PutPixelEcolor(i, j, (G2D_COLOR)((i*j)%8));
		
	for(j=0; j<uG2dHeight; j++)
		for(i=0; i<uG2dWidth; i++)	{
			G2D_ConvertEcolorToRaw((G2D_COLOR)((i*j)%8), &uOrgData);
			G2d_Convert24bpp(uOrgData, eBpp, 1, &uOrgDataConverted);
			G2D_GetFbData(i, j, uG2dWidth, uG2dFbAddr, eBpp, &uFbData);
			G2d_Convert24bpp(uFbData, eBpp, 1, &uFbDataConverted);
			if((uOrgDataConverted&0xffffff) != (uFbDataConverted&0xffffff)) {
				UART_Printf("BPP Change Error!\n");
				bErrorFlag=1;
			}
		if(bErrorFlag) break;			
		}

	if(!bErrorFlag) UART_Printf("24 Bpp ARGB8888 is OK!\n");		


// XRGB 8888
	eBpp=RGB24;
	bErrorFlag=0;
	G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);

	for(j=0; j<uG2dHeight; j++)
		for(i=0; i<uG2dWidth; i++)
			G2D_PutPixelEcolor(i, j, (G2D_COLOR)((i*j)%8));
		
	for(j=0; j<uG2dHeight; j++)
		for(i=0; i<uG2dWidth; i++)	{
			G2D_ConvertEcolorToRaw((G2D_COLOR)((i*j)%8), &uOrgData);
			G2d_Convert24bpp(uOrgData, eBpp, 1, &uOrgDataConverted);
			G2D_GetFbData(i, j, uG2dWidth, uG2dFbAddr, eBpp, &uFbData);
			G2d_Convert24bpp(uFbData, eBpp, 1, &uFbDataConverted);
			if((uOrgDataConverted&0xffffff) != (uFbDataConverted&0xffffff)) {
				UART_Printf("BPP Change Error!\n");
				bErrorFlag=1;
			}
		if(bErrorFlag) break;			
		}

	if(!bErrorFlag) UART_Printf("24 Bpp XRGB8888 is OK!\n");		

// RGBX 8888
	eBpp=RGBX24;
	bErrorFlag=0;
	G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);

	for(j=0; j<uG2dHeight; j++)
		for(i=0; i<uG2dWidth; i++)
			G2D_PutPixelEcolor(i, j, (G2D_COLOR)((i*j)%8));
		
	for(j=0; j<uG2dHeight; j++)
		for(i=0; i<uG2dWidth; i++)	{
			G2D_ConvertEcolorToRaw((G2D_COLOR)((i*j)%8), &uOrgData);
			G2d_Convert24bpp(uOrgData, eBpp, 1, &uOrgDataConverted);
			G2D_GetFbData(i, j, uG2dWidth, uG2dFbAddr, eBpp, &uFbData);
			G2d_Convert24bpp(uFbData, eBpp, 1, &uFbDataConverted);
			if((uOrgDataConverted&0xffffff00) != (uFbDataConverted&0xffffff00)) {
				UART_Printf("BPP Change Error!\n");
				bErrorFlag=1;
			}
		if(bErrorFlag) break;			
		}

	if(!bErrorFlag) UART_Printf("24 Bpp RGBX8888 is OK!\n");



#ifdef INTERRUPT_MODE	
	G2D_InterruptDisable();
	INTC_Disable(NUM_2D);
#endif

	G2D_InitSetting(uG2dFbAddr, eBppBackup, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);	
	LCD_Start();

}


void TestCombinationBpp(void)
{
	u32 i,j;
	u32 uG2dFbAddr, uG2dWidth, uG2dHeight;
	u32 uG2dOffFbAddr, uG2dOffWidth, uG2dOffHeight;	
	CSPACE eBpp, eBppBackup, eBppOff;
	u32 uOrgData, uOrgDataConverted;
	u32 uFbData, uFbDataConverted;
	u8 bErrorFlag=0;


#ifdef INTERRUPT_MODE
	INTC_SetVectAddr(NUM_2D, Isr_G2d);
	INTC_Enable(NUM_2D);
	G2D_InterruptEnable();
#endif


	G2D_GetFrmSz(&uG2dWidth, &uG2dHeight);
	G2D_GetFbAddr(&uG2dFbAddr);
	G2D_GetSrcBppMode(&eBpp);
	eBppBackup=eBpp;
	UART_Printf("Width=%d, Height=%d, Framebuffer:0x%x\n", uG2dWidth, uG2dHeight, uG2dFbAddr);


	LCD_Stop();

	// Source RGB16 Destination RGB24
	UART_Printf("Source: RGB16, Destination: RGB24!\n");	
	//LCDC_InitIp(RGB24, uG2dFbAddr, WIN0, false);
	LCD_InitWin(RGB24, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight, 0, 0, uG2dFbAddr, WIN0, false);	

	G2D_InitSetting(uG2dFbAddr, RGB24, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);	
	G2D_DrawImage(RGB24);
	LCD_Start();
	

	uG2dOffFbAddr = CODEC_MEM_ST+0x800000;
	uG2dOffWidth=uG2dWidth/2;
	uG2dOffHeight=uG2dHeight/2;
	eBppOff=RGB16;
	G2D_SetFbAddr(uG2dOffFbAddr, SRCBUF);	
	G2D_SetBppMode(eBppOff, SRCBUF);
	G2D_SetSourceSize(uG2dOffWidth, uG2dOffHeight);	
	GLIB_InitInstance(uG2dOffFbAddr, uG2dOffWidth, uG2dOffHeight, eBppOff);
	GLIB_DrawPattern(uG2dOffWidth, uG2dOffHeight);	
	
	
	UART_Printf("Press any key to Test!\n");
	UART_Getc();
	
	G2D_BitBlt(0, 0, uG2dOffWidth-1, uG2dOffHeight-1, 0, 0, uG2dWidth/2-1, uG2dHeight/2-1,0);

	UART_Printf("Press any key to Continue!\n");
	UART_Getc();
	
	
	LCD_Stop();
	// Source RGB24 Destination RGB16
	UART_Printf("Source: RGB24, Destination: RGB16!\n");	
//	LCDC_InitIp(RGB16, uG2dFbAddr, WIN0, false);
	LCD_InitWin(RGB24, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight, 0, 0, uG2dFbAddr, WIN0, false);	
	G2D_InitSetting(uG2dFbAddr, RGB16, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);	
	G2D_DrawImage(RGB16);
	LCD_Start();
	

	uG2dOffFbAddr = CODEC_MEM_ST+0x800000;
	uG2dOffWidth=uG2dWidth/2;
	uG2dOffHeight=uG2dHeight/2;
	eBppOff=RGB24;
	G2D_SetFbAddr(uG2dOffFbAddr, SRCBUF);	
	G2D_SetBppMode(eBppOff, SRCBUF);
	G2D_SetSourceSize(uG2dOffWidth, uG2dOffHeight);	
	GLIB_InitInstance(uG2dOffFbAddr, uG2dOffWidth, uG2dOffHeight, eBppOff);
	GLIB_DrawPattern(uG2dOffWidth, uG2dOffHeight);	
	
	
	UART_Printf("Press any key to Test!\n");
	UART_Getc();
	
	G2D_BitBlt(0, 0, uG2dOffWidth-1, uG2dOffHeight-1, 0, 0, uG2dWidth/2-1, uG2dHeight/2-1,0);

	UART_Printf("Press any key to Continue!\n");
	UART_Getc();


#ifdef INTERRUPT_MODE	
	G2D_InterruptDisable();
	INTC_Disable(NUM_2D);
#endif
//	LCDC_InitIp(eBppBackup, uG2dFbAddr, WIN0, false);
	LCD_InitWin(RGB24, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight, 0, 0, uG2dFbAddr, WIN0, false);	
	G2D_InitSetting(uG2dFbAddr, eBppBackup, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);	
	G2D_ClearFrameEcolor(G2D_BLACK, 0, 0);
	LCD_Start();

}



void TestRotationDegree90(void)
{
	u32 i,j;
	u32 uG2dFbAddr, uG2dWidth, uG2dHeight;
	CSPACE eBpp;
	u32 uOrgData, uOrgDataConverted, uFbData, uFbDataConverted;
	u8 bErrorFlag;

	u32 uOrgX, uOrgY, uRType, uRsltX, uRsltY;
	u32 m,n;


#ifdef INTERRUPT_MODE
	INTC_SetVectAddr(NUM_2D, Isr_G2d);
	INTC_Enable(NUM_2D);
	G2D_InterruptEnable();
#endif


	G2D_GetFrmSz(&uG2dWidth, &uG2dHeight);
	G2D_GetFbAddr(&uG2dFbAddr);
	G2D_GetSrcBppMode(&eBpp);
	UART_Printf("Width=%d, Height=%d, Framebuffer:0x%x\n", uG2dWidth, uG2dHeight, uG2dFbAddr);

	G2D_ClearFrameEcolor(G2D_BLACK, 0, 0);
	UART_Printf("Rotation with 90 Degree Test!\n");

	uRType=G2D_ROTATION_90_DEG_BIT;
	G2D_SetRotationMode(uRType);

//	for(m=0; m<uG2dHeight; m++) {
//		for(n=0; n<uG2dWidth; n++) {
//			uOrgX=(u16)n;
//			uOrgY=(u16)m;	
			uOrgX=(u16)(uG2dWidth/2-1);
			uOrgY=(u16)(uG2dHeight/2-1);
			G2D_SetRotationOrg((u16)uOrgX, (u16)uOrgY);
			for(j=0; j<uG2dHeight; j++)
				for(i=0; i<uG2dWidth; i++)
					G2D_PutPixelEcolor(i, j, (G2D_COLOR)((i*j)%8));
				
			for(j=0; j<uG2dHeight; j++) {
				for(i=0; i<uG2dWidth; i++)	{
					G2D_GetRotateCoordinate(i, j, uOrgX, uOrgY, uRType, &uRsltX, &uRsltY);
					if((uRsltX<uG2dWidth) && (uRsltY<uG2dHeight)) {
						G2D_ConvertEcolorToRaw((G2D_COLOR)((i*j)%8), &uOrgData);
						G2d_Convert24bpp(uOrgData, eBpp, 1, &uOrgDataConverted);
						G2D_GetFbData(uRsltX, uRsltY, uG2dWidth, uG2dFbAddr, eBpp, &uFbData);
						G2d_Convert24bpp(uFbData, eBpp, 1, &uFbDataConverted);
						if((uOrgDataConverted&0xffffff) != (uFbDataConverted&0xffffff)) {
							UART_Printf("Rotation Error!\n");
							bErrorFlag=1;
						}
					}
				}
				if(bErrorFlag) break;
			}	
//		}
//		UART_Printf(".");
//	}
	if(!bErrorFlag) UART_Printf("Rotation with 90 Degree Test OK!\n");


#ifdef INTERRUPT_MODE	
	G2D_InterruptDisable();
	INTC_Disable(NUM_2D);
#endif

	G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);	
}


void TestRotationDegree180(void)
{
	u32 i,j;
	u32 uG2dFbAddr, uG2dWidth, uG2dHeight;
	CSPACE eBpp;
	u32 uOrgData, uOrgDataConverted, uFbData, uFbDataConverted;
	u8 bErrorFlag;

	u32 uOrgX, uOrgY, uRType, uRsltX, uRsltY;


#ifdef INTERRUPT_MODE
	INTC_SetVectAddr(NUM_2D, Isr_G2d);
	INTC_Enable(NUM_2D);
	G2D_InterruptEnable();
#endif


	G2D_GetFrmSz(&uG2dWidth, &uG2dHeight);
	G2D_GetFbAddr(&uG2dFbAddr);
	G2D_GetSrcBppMode(&eBpp);
	UART_Printf("Width=%d, Height=%d, Framebuffer:0x%x\n", uG2dWidth, uG2dHeight, uG2dFbAddr);

	G2D_ClearFrameEcolor(G2D_BLACK, 0, 0);
	UART_Printf("Rotation with 180 Degree Test!\n");

	uRType=G2D_ROTATION_180_DEG_BIT;
	G2D_SetRotationMode(uRType);

//	for(m=0; m<uG2dHeight; m++) {
//		for(n=0; n<uG2dWidth; n++) {
//			uOrgX=(u16)n;
//			uOrgY=(u16)m;	
			uOrgX=(u16)(uG2dWidth/2-1);
			uOrgY=(u16)(uG2dHeight/2-1);
			G2D_SetRotationOrg((u16)uOrgX, (u16)uOrgY);
			for(j=0; j<uG2dHeight; j++)
				for(i=0; i<uG2dWidth; i++)
					G2D_PutPixelEcolor(i, j, (G2D_COLOR)((i*j)%8));
				
			for(j=0; j<uG2dHeight; j++) {
				for(i=0; i<uG2dWidth; i++)	{
					G2D_GetRotateCoordinate(i, j, uOrgX, uOrgY, uRType, &uRsltX, &uRsltY);
					if((uRsltX<uG2dWidth) && (uRsltY<uG2dHeight)) {
						G2D_ConvertEcolorToRaw((G2D_COLOR)((i*j)%8), &uOrgData);
						G2d_Convert24bpp(uOrgData, eBpp, 1, &uOrgDataConverted);
						G2D_GetFbData(uRsltX, uRsltY, uG2dWidth, uG2dFbAddr, eBpp, &uFbData);
						G2d_Convert24bpp(uFbData, eBpp, 1, &uFbDataConverted);
						if((uOrgDataConverted&0xffffff) != (uFbDataConverted&0xffffff)) {
							UART_Printf("BPP Change Error!\n");
							bErrorFlag=1;
						}
					}
				}
				if(bErrorFlag) break;	
			}	
//		}
//		UART_Printf(".");
//	}

	if(!bErrorFlag) UART_Printf("Rotation with 180 Degree Test OK!\n");

#ifdef INTERRUPT_MODE	
	G2D_InterruptDisable();
	INTC_Disable(NUM_2D);
#endif

	G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);	
}

void TestRotationDegree270(void)
{
	u32 i,j;
	u32 uG2dFbAddr, uG2dWidth, uG2dHeight;
	CSPACE eBpp;
	u32 uOrgData, uOrgDataConverted, uFbData, uFbDataConverted;
	u8 bErrorFlag;

	u32 uOrgX, uOrgY, uRType, uRsltX, uRsltY;


#ifdef INTERRUPT_MODE
	INTC_SetVectAddr(NUM_2D, Isr_G2d);
	INTC_Enable(NUM_2D);
	G2D_InterruptEnable();
#endif

	G2D_GetFrmSz(&uG2dWidth, &uG2dHeight);
	G2D_GetFbAddr(&uG2dFbAddr);
	G2D_GetSrcBppMode(&eBpp);
	UART_Printf("Width=%d, Height=%d, Framebuffer:0x%x\n", uG2dWidth, uG2dHeight, uG2dFbAddr);

	G2D_ClearFrameEcolor(G2D_BLACK, 0, 0);
	UART_Printf("Rotation with 270 Degree Test!\n");

	uRType=G2D_ROTATION_270_DEG_BIT;
	G2D_SetRotationMode(uRType);

//	for(m=0; m<uG2dHeight; m++) {
//		for(n=0; n<uG2dWidth; n++) {
//			uOrgX=(u16)n;
//			uOrgY=(u16)m;	
			uOrgX=(u16)(uG2dWidth/2-1);
			uOrgY=(u16)(uG2dHeight/2-1);
			G2D_SetRotationOrg((u16)uOrgX, (u16)uOrgY);
			for(j=0; j<uG2dHeight; j++)
				for(i=0; i<uG2dWidth; i++)
					G2D_PutPixelEcolor(i, j, (G2D_COLOR)((i*j)%8));
				
			for(j=0; j<uG2dHeight; j++) {
				for(i=0; i<uG2dWidth; i++)	{
					G2D_GetRotateCoordinate(i, j, uOrgX, uOrgY, uRType, &uRsltX, &uRsltY);
					if((uRsltX<uG2dWidth) && (uRsltY<uG2dHeight)) {
						G2D_ConvertEcolorToRaw((G2D_COLOR)((i*j)%8), &uOrgData);
						G2d_Convert24bpp(uOrgData, eBpp, 1, &uOrgDataConverted);
						G2D_GetFbData(uRsltX, uRsltY, uG2dWidth, uG2dFbAddr, eBpp, &uFbData);
						G2d_Convert24bpp(uFbData, eBpp, 1, &uFbDataConverted);
						if((uOrgDataConverted&0xffffff) != (uFbDataConverted&0xffffff)) {
							UART_Printf("BPP Change Error!\n");
							bErrorFlag=1;
						}
					}
				}
				if(bErrorFlag) break;
			}
//		}
//		UART_Printf(".");
//	}

	if(!bErrorFlag) UART_Printf("Rotation with 270 Degree Test OK!\n");

#ifdef INTERRUPT_MODE	
	G2D_InterruptDisable();
	INTC_Disable(NUM_2D);
#endif

	G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);	
}

void TestRotationXFlip(void)
{
	u32 i,j;
	u32 uG2dFbAddr, uG2dWidth, uG2dHeight;
	CSPACE eBpp;
	u32 uOrgData, uOrgDataConverted, uFbData, uFbDataConverted;
	u8 bErrorFlag;

	u32 uOrgX, uOrgY, uRType, uRsltX, uRsltY;


#ifdef INTERRUPT_MODE
	INTC_SetVectAddr(NUM_2D, Isr_G2d);
	INTC_Enable(NUM_2D);
	G2D_InterruptEnable();
#endif

	G2D_GetFrmSz(&uG2dWidth, &uG2dHeight);
	G2D_GetFbAddr(&uG2dFbAddr);
	G2D_GetSrcBppMode(&eBpp);
	UART_Printf("Width=%d, Height=%d, Framebuffer:0x%x\n", uG2dWidth, uG2dHeight, uG2dFbAddr);

	G2D_ClearFrameEcolor(G2D_BLACK, 0, 0);
	UART_Printf("Rotation with X Flip Test!\n");

	uRType=G2D_X_FLIP;
	G2D_SetRotationMode(uRType);

//	for(m=0; m<uG2dHeight; m++) {
//		for(n=0; n<uG2dWidth; n++) {
//			uOrgX=(u16)n;
//			uOrgY=(u16)m;	
			//uOrgY=(u16)m;	
			uOrgX=(u16)(uG2dWidth/2-1);
			uOrgY=(u16)(uG2dHeight/2-1);
			G2D_SetRotationOrg((u16)uOrgX, (u16)uOrgY);
			for(j=0; j<uG2dHeight; j++)
				for(i=0; i<uG2dWidth; i++)
					G2D_PutPixelEcolor(i, j, (G2D_COLOR)((i*j)%8));
				
			for(j=0; j<uG2dHeight; j++) {
				for(i=0; i<uG2dWidth; i++)	{
					G2D_GetRotateCoordinate(i, j, uOrgX, uOrgY, uRType, &uRsltX, &uRsltY);
					if((uRsltX<uG2dWidth) && (uRsltY<uG2dHeight)) {
						G2D_ConvertEcolorToRaw((G2D_COLOR)((i*j)%8), &uOrgData);
						G2d_Convert24bpp(uOrgData, eBpp, 1, &uOrgDataConverted);
						G2D_GetFbData(uRsltX, uRsltY, uG2dWidth, uG2dFbAddr, eBpp, &uFbData);
						G2d_Convert24bpp(uFbData, eBpp, 1, &uFbDataConverted);
						if((uOrgDataConverted&0xffffff) != (uFbDataConverted&0xffffff)) {
							UART_Printf("BPP Change Error!\n");
							bErrorFlag=1;
						}
					}
				}
				if(bErrorFlag) break;
			}				
//		}
//		UART_Printf(".");
//	}

	if(!bErrorFlag) UART_Printf("Rotation with X Flip Test OK!\n");

#ifdef INTERRUPT_MODE	
	G2D_InterruptDisable();
	INTC_Disable(NUM_2D);
#endif

	G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);	
}

void TestRotationYFlip(void)
{
	u32 i,j;
	u32 uG2dFbAddr, uG2dWidth, uG2dHeight;
	CSPACE eBpp;
	u32 uOrgData, uOrgDataConverted, uFbData, uFbDataConverted;
	u8 bErrorFlag;

	u32 uOrgX, uOrgY, uRType, uRsltX, uRsltY;


#ifdef INTERRUPT_MODE
	INTC_SetVectAddr(NUM_2D, Isr_G2d);
	INTC_Enable(NUM_2D);
	G2D_InterruptEnable();
#endif


	G2D_GetFrmSz(&uG2dWidth, &uG2dHeight);
	G2D_GetFbAddr(&uG2dFbAddr);
	G2D_GetSrcBppMode(&eBpp);
	UART_Printf("Width=%d, Height=%d, Framebuffer:0x%x\n", uG2dWidth, uG2dHeight, uG2dFbAddr);

	G2D_ClearFrameEcolor(G2D_BLACK, 0, 0);
	UART_Printf("Rotation with Y Flip Test!\n");

	uRType=G2D_Y_FLIP;
	G2D_SetRotationMode(uRType);

//	for(m=0; m<uG2dHeight; m++) {
//		for(n=0; n<uG2dWidth; n++) {
//			uOrgX=(u16)n;
//			uOrgY=(u16)m;	
			uOrgX=(u16)(uG2dWidth/2-1);
			uOrgY=(u16)(uG2dHeight/2-1);

			G2D_SetRotationOrg((u16)uOrgX, (u16)uOrgY);
			for(j=0; j<uG2dHeight; j++)
				for(i=0; i<uG2dWidth; i++)
					G2D_PutPixelEcolor(i, j, (G2D_COLOR)((i*j)%8));
				
			for(j=0; j<uG2dHeight; j++) {
				for(i=0; i<uG2dWidth; i++)	{
					G2D_GetRotateCoordinate(i, j, uOrgX, uOrgY, uRType, &uRsltX, &uRsltY);
					if((uRsltX<uG2dWidth) && (uRsltY<uG2dHeight)) {
						G2D_ConvertEcolorToRaw((G2D_COLOR)((i*j)%8), &uOrgData);
						G2d_Convert24bpp(uOrgData, eBpp, 1, &uOrgDataConverted);
						G2D_GetFbData(uRsltX, uRsltY, uG2dWidth, uG2dFbAddr, eBpp, &uFbData);
						G2d_Convert24bpp(uFbData, eBpp, 1, &uFbDataConverted);
						if((uOrgDataConverted&0xffffff) != (uFbDataConverted&0xffffff)) {
							UART_Printf("BPP Change Error!\n");
							bErrorFlag=1;
						}
					}
				}
				if(bErrorFlag) break;
			}				
//		}
//		UART_Printf(".");
//	}

	if(!bErrorFlag) UART_Printf("Rotation with Y Flip Test OK!\n");

#ifdef INTERRUPT_MODE	
	G2D_InterruptDisable();
	INTC_Disable(NUM_2D);
#endif

	G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);	
}

void TestEndianChange(void)
{
	u32 i,j,k,l;
	u32 uG2dFbAddr, uG2dWidth, uG2dHeight;
	CSPACE eBpp, eBppBackup;
	u8 bEndian, bErrorFlag=0;
	u32 uOrgData, uFbData;
	u32 uOrgDataConverted, uFbDataConverted;

#ifdef INTERRUPT_MODE
	INTC_SetVectAddr(NUM_2D, Isr_G2d);
	INTC_Enable(NUM_2D);
	G2D_InterruptEnable();
#endif


	G2D_GetFrmSz(&uG2dWidth, &uG2dHeight);
	G2D_GetFbAddr(&uG2dFbAddr);	
	G2D_GetSrcBppMode(&eBpp);
		
	UART_Printf("Width=%d, Height=%d, Framebuffer:0x%x\n", uG2dWidth, uG2dHeight, uG2dFbAddr);
	
	G2D_ClearFrameEcolor(G2D_BLACK, 0, 0);
	UART_Printf("EndianChange, press any key to continue\n");
	LCD_Stop();

	bEndian=0;
	eBppBackup=eBpp;
	for(l=0; l<2; l++) {
		bEndian = (l) ? 1: 0;
		if(bEndian) UART_Printf("Big Endian Test!\n");
		else if(!bEndian) UART_Printf("Little Endian Test!\n");
		else Assert(0);		
		for(k=0; k<4; k++) {
			if(k==0) { UART_Printf("[15BPP]"); eBpp=ARGB8; }
			else if(k==1) { UART_Printf("[16BPP]"); eBpp=RGB16;}
			else if(k==2) { UART_Printf("[18BPP]"); eBpp=RGB18;}
			else if(k==3) { UART_Printf("[24BPP]"); eBpp=RGB24;}				
			G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);	
			G2D_SetEndian(bEndian);

			for(j=0; j<uG2dHeight; j++)
				for(i=0; i<uG2dWidth; i++)
					G2D_PutPixelEcolor(i, j, (G2D_COLOR)((i*j)%8));

			for(j=0; j<uG2dHeight; j++)
				for(i=0; i<uG2dWidth; i++)	{
					G2D_ConvertEcolorToRaw((G2D_COLOR)((i*j)%8), &uOrgData);
					G2d_Convert24bpp(uOrgData, eBpp, 1, &uOrgDataConverted);
					G2D_GetFbData(i, j, uG2dWidth, uG2dFbAddr, eBpp, &uFbData);
					G2d_Convert24bpp(uFbData, eBpp, 1, &uFbDataConverted);
					if(uOrgDataConverted != uFbDataConverted) {
						UART_Printf("BPP Change Error!\n");
						bErrorFlag=1;
					}	
				}	
			if(!bErrorFlag) UART_Printf(" OK!\n");
		}	
	}	
	
#ifdef INTERRUPT_MODE	
	G2D_InterruptDisable();
	INTC_Disable(NUM_2D);
#endif

	G2D_InitSetting(uG2dFbAddr, eBppBackup, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);	
}


void TestInterruptDrawingEngineFinish(void)
{
	u32 i,j;
	u32 uG2dFbAddr, uG2dWidth, uG2dHeight;
	CSPACE eBpp;


	uIntCnt=0;
	INTC_SetVectAddr(NUM_2D, Isr_G2dInt);
	INTC_Enable(NUM_2D);
	G2D_IntEnSelectable(G2D_INT_CURR_CMD);


	G2D_GetFrmSz(&uG2dWidth, &uG2dHeight);
	G2D_GetFbAddr(&uG2dFbAddr);	
	G2D_GetSrcBppMode(&eBpp);
		
	UART_Printf("Width=%d, Height=%d, Framebuffer:0x%x\n", uG2dWidth, uG2dHeight, uG2dFbAddr);
	
	G2D_ClearFrameEcolor(G2D_BLACK, 0, 0);

	for(j=0; j<uG2dHeight; j++)
		for(i=0; i<uG2dWidth; i++) 
			G2D_PutPixelForInterrupt(i, j, 0);

	UART_Printf("The number of occured interrupt: %d\n", uIntCnt);
	uIntCnt=0;

	G2D_IntDisSelectable(G2D_INT_CURR_CMD);
	INTC_Disable(NUM_2D);

	G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);	
}

void TestInterruptCommandFifoFinish(void)
{
	u32 i,j;
	u32 uG2dFbAddr, uG2dWidth, uG2dHeight;
	CSPACE eBpp;
	

	uIntCnt=0;
	INTC_SetVectAddr(NUM_2D, Isr_G2dInt);
	INTC_Enable(NUM_2D);
	G2D_IntEnSelectable(G2D_INT_ALL_CMD);

	G2D_GetFrmSz(&uG2dWidth, &uG2dHeight);
	G2D_GetFbAddr(&uG2dFbAddr);	
	G2D_GetSrcBppMode(&eBpp);
		
	UART_Printf("Width=%d, Height=%d, Framebuffer:0x%x\n", uG2dWidth, uG2dHeight, uG2dFbAddr);
	
	G2D_ClearFrameEcolor(G2D_BLACK, 0, 0);


	G2D_PutPixelForInterruptHard(uG2dWidth, uG2dHeight, 0xffffff);
	
	UART_Printf("The number of occured interrupt: %d\n", uIntCnt);
	uIntCnt=0;	

	G2D_IntDisSelectable(G2D_INT_ALL_CMD);
	INTC_Disable(NUM_2D);


	G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);	
}


void TestInterruptOverflow(void)
{
	u32 uG2dFbAddr, uG2dWidth, uG2dHeight;
	CSPACE eBpp;


	uIntCnt=0;
	INTC_SetVectAddr(NUM_2D, Isr_G2dInt);
	INTC_Enable(NUM_2D);
	G2D_IntEnSelectable(G2D_INT_OVERFLOW);


	G2D_GetFrmSz(&uG2dWidth, &uG2dHeight);
	G2D_GetFbAddr(&uG2dFbAddr);	
	G2D_GetSrcBppMode(&eBpp);
		
	UART_Printf("Width=%d, Height=%d, Framebuffer:0x%x\n", uG2dWidth, uG2dHeight, uG2dFbAddr);
	
	G2D_ClearFrameEcolor(G2D_BLACK, 0, 0);

	G2D_PutPixelForInterruptHard(uG2dWidth, uG2dHeight, 0xffffff);


	UART_Printf("The number of occured interrupt: %d\n", uIntCnt);
	uIntCnt=0;	

	G2D_IntDisSelectable(G2D_INT_OVERFLOW);
	INTC_Disable(NUM_2D);

	G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);	
}


void TestInterruptFifoLevel(void)
{
	u32 uG2dFbAddr, uG2dWidth, uG2dHeight;
	CSPACE eBpp;

	u32 uFifoLevel;


	G2D_GetFrmSz(&uG2dWidth, &uG2dHeight);
	G2D_GetFbAddr(&uG2dFbAddr);	
	G2D_GetSrcBppMode(&eBpp);
		
	UART_Printf("Width=%d, Height=%d, Framebuffer:0x%x\n", uG2dWidth, uG2dHeight, uG2dFbAddr);
	
	G2D_ClearFrameEcolor(G2D_BLACK, 0, 0);

	UART_Printf("enter fifo level:");
	uFifoLevel=UART_GetIntNum();
	G2D_SetFifoIntLevel((u8)uFifoLevel);


	uIntCnt=0;
	INTC_SetVectAddr(NUM_2D, Isr_G2dInt);
	INTC_Enable(NUM_2D);
	G2D_IntEnSelectable(G2D_INT_FIFO_LEVEL);


	G2D_PutPixelForInterruptHard(uG2dWidth, uG2dHeight, 0xffffff);
	UART_Printf("The number of occured interrupt: %d\n", uIntCnt);
	uIntCnt=0;	


	G2D_IntDisSelectable(G2D_INT_FIFO_LEVEL);
	INTC_Disable(NUM_2D);
	
	G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);	
}


void TestStencilWithPointDrwaing(void)
{
	u32 i,j;
	u32 uG2dFbAddr, uG2dWidth, uG2dHeight;
	CSPACE eBpp;
	u32 uStencilColor;
	u32 uAMin, uAMax, uRMin, uRMax, uGMin, uGMax, uBMin, uBMax;
	u32 uIsInvOn, uIsAOn, uIsROn, uIsGOn, uIsBOn;
	u32 uQWidth, uQHeight;


#ifdef INTERRUPT_MODE
	INTC_SetVectAddr(NUM_2D, Isr_G2d);
	INTC_Enable(NUM_2D);
	G2D_InterruptEnable();
#endif


	G2D_GetFrmSz(&uG2dWidth, &uG2dHeight);
	G2D_GetFbAddr(&uG2dFbAddr);	
	G2D_GetSrcBppMode(&eBpp);
		
	UART_Printf("Width=%d, Height=%d, Framebuffer:0x%x\n", uG2dWidth, uG2dHeight, uG2dFbAddr);
	
	G2D_ClearFrameEcolor(G2D_BLACK, 0, 0);
	uQHeight=uG2dHeight/2;
	uQWidth=uG2dWidth/2;
	UART_Printf("Stencil Test with Point Drawing!\n");

	switch(eBpp) {
		case RGB16:
			uRMin=uGMin=uBMin=20;
			uRMax=uGMax=uBMax=31;
			uStencilColor=(uRMin<<11)|(uGMin<<5)|(uBMin<<0);
			break;	
		case RGBA16:
			uRMin=uGMin=uBMin=20;
			uRMax=uGMax=uBMax=31;
			uStencilColor=(uRMin<<11)|(uGMin<<6)|(uBMin<<1);
			break;
		case ARGB16:
			uRMin=uGMin=uBMin=20;
			uRMax=uGMax=uBMax=31;
			uStencilColor=(uRMin<<10)|(uGMin<<5)|(uBMin<<0);			
			break;
		case RGB24:		
			uRMin=uGMin=uBMin=200;
			uRMax=uGMax=uBMax=255;
			uStencilColor=(uRMax<<16)|(uGMax<<8)|(uBMax<<0);
			break;
		case RGBX24:		
			uRMin=uGMin=uBMin=200;
			uRMax=uGMax=uBMax=255;
			uStencilColor=(uRMax<<24)|(uGMax<<16)|(uBMax<<8);
			break;			
		case ARGB24:		
			uRMin=uGMin=uBMin=uAMin=200;
			uRMax=uGMax=uBMax=uAMax=255;
			uStencilColor=(uAMax<<24)|(uRMax<<16)|(uGMax<<8)|(uBMax<<0);
			break;
		case RGBA24:		
			uRMin=uGMin=uBMin=uAMin=200;
			uRMax=uGMax=uBMax=uAMax=255;
			uStencilColor=(uRMax<<24)|(uGMax<<16)|(uBMax<<8)|(uAMax<<0);
			break;			
		default:
			Assert(0);
			break;
	}


	// With Drawing Point
	for(j=0; j<uQHeight; j++)
		for(i=0; i<uQWidth; i++)
			if(i<(uQWidth/2) && j<(uQHeight/2)) G2D_PutPixelEcolor(i, j, G2D_RED);
			else if(i>=(uQWidth/2) && j<(uQHeight/2)) G2D_PutPixelEcolor(i, j, G2D_GREEN);
			else if(i<(uQWidth/2) && j>=(uQHeight/2)) G2D_PutPixelEcolor(i, j, G2D_BLUE);
			else G2D_PutPixelEcolor(i, j, G2D_MAGENTA);

	UART_Printf("Press any key to test STENCIL with Non-Inverse!\n");
	UART_Getc();
	uIsROn=uIsGOn=uIsBOn=true;
	uIsInvOn=uIsAOn=false;
	uAMin=uAMax=0x0;
	G2D_SetStencilMinMax(uAMin, uAMax, uRMin, uRMax, uGMin, uGMax, uBMin, uBMax, eBpp);
	G2D_SetStencilKey(uIsInvOn, uIsAOn, uIsROn, uIsGOn, uIsBOn);


	for(j=uQHeight; j<uG2dHeight; j++)
		for(i=uQWidth; i<uG2dWidth; i++)
			if(i<(uG2dWidth/2+uQWidth/2) && j<(uG2dHeight/2+uQHeight/2)) G2D_PutPixelEcolor(i, j, G2D_RED);
			else if(i>=(uG2dWidth/2+uQWidth/2) && j<(uG2dHeight/2+uQHeight/2)) G2D_PutPixel(i, j, uStencilColor);
			else if(i<(uG2dWidth/2+uQWidth/2) && j>=(uG2dHeight/2+uQHeight/2)) G2D_PutPixel(i, j, uStencilColor);
			else G2D_PutPixelEcolor(i, j, G2D_MAGENTA);
			
	UART_Printf("Press any key to continue!\n");
	UART_Getc();
			
	G2D_SetStencilKey(0, 0, 0, 0, 0);
	G2D_ClearFrameEcolor(G2D_BLACK, 0, 0);
	// With Drawing Point
	for(j=0; j<uQHeight; j++)
		for(i=0; i<uQWidth; i++)
			if(i<(uQWidth/2) && j<(uQHeight/2)) G2D_PutPixelEcolor(i, j, G2D_RED);
			else if(i>=(uQWidth/2) && j<(uQHeight/2)) G2D_PutPixelEcolor(i, j, G2D_GREEN);
			else if(i<(uQWidth/2) && j>=(uQHeight/2)) G2D_PutPixelEcolor(i, j, G2D_BLUE);
			else G2D_PutPixelEcolor(i, j, G2D_MAGENTA);

	UART_Printf("Press any key to test STENCIL with Inverse!\n");
	UART_Getc();

	uIsROn=uIsGOn=uIsBOn=uIsInvOn=true;
	uIsAOn=false;
	uAMin=uAMax=0x0;
	G2D_SetStencilMinMax(uAMin, uAMax, uRMin, uRMax, uGMin, uGMax, uBMin, uBMax, eBpp);
	G2D_SetStencilKey(uIsInvOn, uIsAOn, uIsROn, uIsGOn, uIsBOn);


	for(j=uQHeight; j<uG2dHeight; j++)
		for(i=uQWidth; i<uG2dWidth; i++)
			if(i<(uG2dWidth/2+uQWidth/2) && j<(uG2dHeight/2+uQHeight/2)) G2D_PutPixelEcolor(i, j, G2D_RED);
			else if(i>=(uG2dWidth/2+uQWidth/2) && j<(uG2dHeight/2+uQHeight/2)) G2D_PutPixel(i, j, uStencilColor);
			else if(i<(uG2dWidth/2+uQWidth/2) && j>=(uG2dHeight/2+uQHeight/2)) G2D_PutPixel(i, j, uStencilColor);
			else G2D_PutPixelEcolor(i, j, G2D_MAGENTA);	


	UART_Printf("Press any key to continue!\n");
	UART_Getc();
	G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);	


	
#ifdef INTERRUPT_MODE	
	G2D_InterruptDisable();
	INTC_Disable(NUM_2D);
#endif

	G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);	
}

void TestStencilWithOnScrBitBLT(void)
{
	u32 i,j;
	u32 uG2dFbAddr, uG2dWidth, uG2dHeight;
	CSPACE eBpp;
	u32 uStencilColor;
	u32 uAMin, uAMax, uRMin, uRMax, uGMin, uGMax, uBMin, uBMax;
	u32 uIsInvOn, uIsAOn, uIsROn, uIsGOn, uIsBOn;
	u32 uQWidth, uQHeight;


#ifdef INTERRUPT_MODE
	INTC_SetVectAddr(NUM_2D, Isr_G2d);
	INTC_Enable(NUM_2D);
	G2D_InterruptEnable();
#endif


	G2D_GetFrmSz(&uG2dWidth, &uG2dHeight);
	G2D_GetFbAddr(&uG2dFbAddr);	
	G2D_GetSrcBppMode(&eBpp);
		
	UART_Printf("Width=%d, Height=%d, Framebuffer:0x%x\n", uG2dWidth, uG2dHeight, uG2dFbAddr);
	
	G2D_ClearFrameEcolor(G2D_BLACK, 0, 0);
	uQHeight=uG2dHeight/2;
	uQWidth=uG2dWidth/2;
	UART_Printf("Stencil Test with Normal BitBLT!\n");

	uAMin=uAMax=0x0;
	switch(eBpp) {
		case RGB16:
			uRMin=uGMin=uBMin=20;
			uRMax=uGMax=uBMax=31;
			uStencilColor=(uRMin<<11)|(uGMin<<5)|(uBMin<<0);
			break;	
		case RGBA16:
			uRMin=uGMin=uBMin=20;
			uRMax=uGMax=uBMax=31;
			uStencilColor=(uRMin<<11)|(uGMin<<6)|(uBMin<<1);
			break;
		case ARGB16:
			uRMin=uGMin=uBMin=20;
			uRMax=uGMax=uBMax=31;
			uStencilColor=(uRMin<<10)|(uGMin<<5)|(uBMin<<0);			
			break;
		case RGB24:		
			uRMin=uGMin=uBMin=200;
			uRMax=uGMax=uBMax=255;
			uStencilColor=(uRMax<<16)|(uGMax<<8)|(uBMax<<0);
			break;
		case RGBX24:		
			uRMin=uGMin=uBMin=200;
			uRMax=uGMax=uBMax=255;
			uStencilColor=(uRMax<<24)|(uGMax<<16)|(uBMax<<8);
			break;			
		case ARGB24:		
			uRMin=uGMin=uBMin=uAMin=200;
			uRMax=uGMax=uBMax=uAMax=255;
			uStencilColor=(uAMax<<24)|(uRMax<<16)|(uGMax<<8)|(uBMax<<0);
			break;
		case RGBA24:		
			uRMin=uGMin=uBMin=uAMin=200;
			uRMax=uGMax=uBMax=uAMax=255;
			uStencilColor=(uRMax<<24)|(uGMax<<16)|(uBMax<<8)|(uAMax<<0);
			break;			
		default:
			Assert(0);
			break;
	}


	for(j=0; j<uQHeight; j++)
		for(i=0; i<uQWidth; i++)
			if(i<(uQWidth/2) && j<(uQHeight/2)) G2D_PutPixelEcolor(i, j, G2D_RED);
			else if(i>=(uQWidth/2) && j<(uQHeight/2)) G2D_PutPixel(i, j, uStencilColor);
			else if(i<(uQWidth/2) && j>=(uQHeight/2)) G2D_PutPixel(i, j, uStencilColor);
			else G2D_PutPixelEcolor(i, j, G2D_MAGENTA);	
			
	UART_Printf("Press any key to test STENCIL with Non-Inverse!\n");
	UART_Getc();
	uIsROn=uIsGOn=uIsBOn=true;
	uIsInvOn=uIsAOn=false;
	if(eBpp == ARGB24) uIsAOn = true;

	G2D_SetStencilMinMax(uAMin, uAMax, uRMin, uRMax, uGMin, uGMax, uBMin, uBMax, eBpp);
	G2D_SetStencilKey(uIsInvOn, uIsAOn, uIsROn, uIsGOn, uIsBOn);

	G2D_BitBlt(0,0,uG2dWidth/2-1,uG2dHeight/2-1,uG2dWidth/2,uG2dHeight/2,uG2dWidth-1,uG2dHeight-1,0);
			

	UART_Printf("Press any key to continue!\n");
	UART_Getc();	
	G2D_SetStencilKey(0, 0, 0, 0, 0);
	G2D_ClearFrameEcolor(G2D_BLACK, 0, 0);


	for(j=0; j<uQHeight; j++)
		for(i=0; i<uQWidth; i++)
			if(i<(uQWidth/2) && j<(uQHeight/2)) G2D_PutPixelEcolor(i, j, G2D_RED);
			else if(i>=(uQWidth/2) && j<(uQHeight/2)) G2D_PutPixel(i, j, uStencilColor);
			else if(i<(uQWidth/2) && j>=(uQHeight/2)) G2D_PutPixel(i, j, uStencilColor);
			else G2D_PutPixelEcolor(i, j, G2D_MAGENTA);	
	UART_Printf("Press any key to test STENCIL with Inverse!\n");
	UART_Getc();

	uIsROn=uIsGOn=uIsBOn=uIsInvOn=true;
	uIsAOn=false;
	if(eBpp == ARGB24) uIsAOn = true;
		
	G2D_SetStencilMinMax(uAMin, uAMax, uRMin, uRMax, uGMin, uGMax, uBMin, uBMax, eBpp);
	G2D_SetStencilKey(uIsInvOn, uIsAOn, uIsROn, uIsGOn, uIsBOn);


	G2D_BitBlt(0,0,uG2dWidth/2-1,uG2dHeight/2-1,uG2dWidth/2,uG2dHeight/2,uG2dWidth-1,uG2dHeight-1,0);
			

	UART_Printf("Press any key to continue!\n");
	UART_Getc();
	
	G2D_SetStencilKey(0, 0, 0, 0, 0);
	G2D_ClearFrameEcolor(G2D_BLACK, 0, 0);
	UART_Printf("Stencil Test with Stretched BitBLT!\n");	

	for(j=0; j<uQHeight/2; j++)
		for(i=0; i<uQWidth/2; i++)
			if(i<(uQWidth/4) && j<(uQHeight/4)) G2D_PutPixelEcolor(i, j, G2D_RED);
			else if(i>=(uQWidth/4) && j<(uQHeight/4)) G2D_PutPixel(i, j, uStencilColor);
			else if(i<(uQWidth/4) && j>=(uQHeight/4)) G2D_PutPixel(i, j, uStencilColor);
			else G2D_PutPixelEcolor(i, j, G2D_MAGENTA);	
			
	UART_Printf("Press any key to test STENCIL with Non-Inverse!\n");
	UART_Getc();
	uIsROn=uIsGOn=uIsBOn=true;
	uIsInvOn=uIsAOn=false;
	if(eBpp == ARGB24) uIsAOn = true;	

	G2D_SetStencilMinMax(uAMin, uAMax, uRMin, uRMax, uGMin, uGMax, uBMin, uBMax, eBpp);
	G2D_SetStencilKey(uIsInvOn, uIsAOn, uIsROn, uIsGOn, uIsBOn);

	G2D_BitBlt(0,0,uG2dWidth/4-1,uG2dHeight/4-1,uG2dWidth/2,uG2dHeight/2,uG2dWidth-1,uG2dHeight-1,1);
			

	UART_Printf("Press any key to continue!\n");
	UART_Getc();	
	G2D_SetStencilKey(0, 0, 0, 0, 0);
	G2D_ClearFrameEcolor(G2D_BLACK, 0, 0);


	for(j=0; j<uQHeight/2; j++)
		for(i=0; i<uQWidth/2; i++)
			if(i<(uQWidth/4) && j<(uQHeight/4)) G2D_PutPixelEcolor(i, j, G2D_RED);
			else if(i>=(uQWidth/4) && j<(uQHeight/4)) G2D_PutPixel(i, j, uStencilColor);
			else if(i<(uQWidth/4) && j>=(uQHeight/4)) G2D_PutPixel(i, j, uStencilColor);
			else G2D_PutPixelEcolor(i, j, G2D_MAGENTA);	
	UART_Printf("Press any key to test STENCIL with Inverse!\n");
	UART_Getc();

	uIsROn=uIsGOn=uIsBOn=uIsInvOn=true;
	uIsAOn=false;
	if(eBpp == ARGB24) uIsAOn = true;	
	G2D_SetStencilMinMax(uAMin, uAMax, uRMin, uRMax, uGMin, uGMax, uBMin, uBMax, eBpp);
	G2D_SetStencilKey(uIsInvOn, uIsAOn, uIsROn, uIsGOn, uIsBOn);


	G2D_BitBlt(0,0,uG2dWidth/4-1,uG2dHeight/4-1,uG2dWidth/2,uG2dHeight/2,uG2dWidth-1,uG2dHeight-1,1);
			

	UART_Printf("Press any key to continue!\n");
	UART_Getc();	
	
	
	G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);	


	
#ifdef INTERRUPT_MODE	
	G2D_InterruptDisable();
	INTC_Disable(NUM_2D);
#endif

	G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);	

}

void TestStencilWithOffScrBitBLT(void)
{
	u32 i,j;
	u32 uG2dFbAddr, uG2dWidth, uG2dHeight;
	u32 uG2dOffFbAddr, uG2dOffWidth, uG2dOffHeight;	
	CSPACE eBpp, eBppOff;
	u32 uStencilColor;
	u32 uAMin, uAMax, uRMin, uRMax, uGMin, uGMax, uBMin, uBMax;
	u32 uIsInvOn, uIsAOn, uIsROn, uIsGOn, uIsBOn;
	u32 uQWidth, uQHeight;


#ifdef INTERRUPT_MODE
	INTC_SetVectAddr(NUM_2D, Isr_G2d);
	INTC_Enable(NUM_2D);
	G2D_InterruptEnable();
#endif


	G2D_GetFrmSz(&uG2dWidth, &uG2dHeight);
	G2D_GetFbAddr(&uG2dFbAddr);	
	G2D_GetSrcBppMode(&eBpp);
		
	UART_Printf("Width=%d, Height=%d, Framebuffer:0x%x\n", uG2dWidth, uG2dHeight, uG2dFbAddr);
	
	G2D_ClearFrameEcolor(G2D_BLACK, 0, 0);
	
	G2D_DrawImage(eBpp);

	UART_Printf("Stencil Test with Normal BitBLT!\n");

	uAMin=uAMax=0x0;
	switch(eBpp) {
		case RGB16:
			uRMin=uGMin=uBMin=20;
			uRMax=uGMax=uBMax=31;
			uStencilColor=(uRMin<<11)|(uGMin<<5)|(uBMin<<0);
			break;	
		case RGBA16:
			uRMin=uGMin=uBMin=20;
			uRMax=uGMax=uBMax=31;
			uStencilColor=(uRMin<<11)|(uGMin<<6)|(uBMin<<1);
			break;
		case ARGB16:
			uRMin=uGMin=uBMin=20;
			uRMax=uGMax=uBMax=31;
			uStencilColor=(uRMin<<10)|(uGMin<<5)|(uBMin<<0);			
			break;
		case RGB24:		
			uRMin=uGMin=uBMin=200;
			uRMax=uGMax=uBMax=255;
			uStencilColor=(uRMax<<16)|(uGMax<<8)|(uBMax<<0);
			break;
		case RGBX24:		
			uRMin=uGMin=uBMin=200;
			uRMax=uGMax=uBMax=255;
			uStencilColor=(uRMax<<24)|(uGMax<<16)|(uBMax<<8);
			break;			
		case ARGB24:		
			uRMin=uGMin=uBMin=uAMin=200;
			uRMax=uGMax=uBMax=uAMax=255;
			uStencilColor=(uAMax<<24)|(uRMax<<16)|(uGMax<<8)|(uBMax<<0);
			break;
		case RGBA24:		
			uRMin=uGMin=uBMin=uAMin=200;
			uRMax=uGMax=uBMax=uAMax=255;
			uStencilColor=(uRMax<<24)|(uGMax<<16)|(uBMax<<8)|(uAMax<<0);
			break;			
		default:
			Assert(0);
			break;
	}

	uG2dOffFbAddr = CODEC_MEM_ST+0x800000;
	uG2dOffWidth=uG2dWidth/2;
	uG2dOffHeight=uG2dHeight/2;
	eBppOff=eBpp;
	G2D_SetFbAddr(uG2dOffFbAddr, SRCBUF);	
	G2D_SetBppMode(eBppOff, SRCBUF);
	G2D_SetSourceSize(uG2dOffWidth, uG2dOffHeight);	
	GLIB_InitInstance(uG2dOffFbAddr, uG2dOffWidth, uG2dOffHeight, eBppOff);
	GLIB_DrawPattern(uG2dOffWidth, uG2dOffHeight);

	for(j=0; j<uG2dOffHeight; j++)
		for(i=0; i<uG2dOffWidth; i++)
			if(i<(uG2dOffWidth/2) && j<(uG2dOffHeight/2)) GLIB_PutPixel(i, j, G2D_RED);
			else if(i>=(uG2dOffWidth/2) && j<(uG2dOffHeight/2)) GLIB_PutPixel2(i, j, uStencilColor);
			else if(i<(uG2dOffWidth/2) && j>=(uG2dOffHeight/2)) GLIB_PutPixel2(i, j, uStencilColor);
			else GLIB_PutPixel(i, j, G2D_MAGENTA);	
			
	UART_Printf("Press any key to test STENCIL with Non-Inverse!\n");
	UART_Getc();
	uIsROn=uIsGOn=uIsBOn=true;
	uIsInvOn=uIsAOn=false;

	G2D_SetStencilMinMax(uAMin, uAMax, uRMin, uRMax, uGMin, uGMax, uBMin, uBMax, eBpp);
	G2D_SetStencilKey(uIsInvOn, uIsAOn, uIsROn, uIsGOn, uIsBOn);

	G2D_BitBlt(0,0,uG2dOffWidth-1,uG2dOffHeight-1,uG2dWidth/2,uG2dHeight/2,uG2dWidth-1,uG2dHeight-1,0);
			

	UART_Printf("Press any key to continue!\n");
	UART_Getc();	
	G2D_SetStencilKey(0, 0, 0, 0, 0);
	G2D_DrawImage(eBpp);


	for(j=0; j<uG2dOffHeight; j++)
		for(i=0; i<uG2dOffWidth; i++)
			if(i<(uG2dOffWidth/2) && j<(uG2dOffHeight/2)) GLIB_PutPixel(i, j, G2D_RED);
			else if(i>=(uG2dOffWidth/2) && j<(uG2dOffHeight/2)) GLIB_PutPixel2(i, j, uStencilColor);
			else if(i<(uG2dOffWidth/2) && j>=(uG2dOffHeight/2)) GLIB_PutPixel2(i, j, uStencilColor);
			else GLIB_PutPixel(i, j, G2D_MAGENTA);
			
	UART_Printf("Press any key to test STENCIL with Inverse!\n");
	UART_Getc();

	uIsROn=uIsGOn=uIsBOn=uIsInvOn=true;
	uIsAOn=false;
	uAMin=uAMax=0x0;
	G2D_SetStencilMinMax(uAMin, uAMax, uRMin, uRMax, uGMin, uGMax, uBMin, uBMax, eBpp);
	G2D_SetStencilKey(uIsInvOn, uIsAOn, uIsROn, uIsGOn, uIsBOn);


	G2D_BitBlt(0,0,uG2dOffWidth-1,uG2dOffHeight-1,uG2dWidth/2,uG2dHeight/2,uG2dWidth-1,uG2dHeight-1,0);
			

	UART_Printf("Press any key to continue!\n");
	UART_Getc();
	
	G2D_SetStencilKey(0, 0, 0, 0, 0);
	G2D_DrawImage(eBpp);
	UART_Printf("Stencil Test with Stretched BitBLT!\n");
	uG2dOffFbAddr = CODEC_MEM_ST+0x800000;
	uG2dOffWidth=40;
	uG2dOffHeight=64;
	eBppOff=eBpp;
	G2D_SetFbAddr(uG2dOffFbAddr, SRCBUF);	
	G2D_SetBppMode(eBppOff, SRCBUF);
	G2D_SetSourceSize(uG2dOffWidth, uG2dOffHeight);	
	GLIB_InitInstance(uG2dOffFbAddr, uG2dOffWidth, uG2dOffHeight, eBppOff);
	GLIB_DrawPattern(uG2dOffWidth, uG2dOffHeight);		

	for(j=0; j<uG2dOffHeight; j++)
		for(i=0; i<uG2dOffWidth; i++)
			if(i<(uG2dOffWidth/2) && j<(uG2dOffHeight/2)) GLIB_PutPixel(i, j, G2D_RED);
			else if(i>=(uG2dOffWidth/2) && j<(uG2dOffHeight/2)) GLIB_PutPixel2(i, j, uStencilColor);
			else if(i<(uG2dOffWidth/2) && j>=(uG2dOffHeight/2)) GLIB_PutPixel2(i, j, uStencilColor);
			else GLIB_PutPixel(i, j, G2D_MAGENTA);
			
	UART_Printf("Press any key to test STENCIL with Non-Inverse!\n");
	UART_Getc();
	uIsROn=uIsGOn=uIsBOn=true;
	uIsInvOn=uIsAOn=false;

	G2D_SetStencilMinMax(uAMin, uAMax, uRMin, uRMax, uGMin, uGMax, uBMin, uBMax, eBpp);
	G2D_SetStencilKey(uIsInvOn, uIsAOn, uIsROn, uIsGOn, uIsBOn);

	G2D_BitBlt(0,0,uG2dOffWidth-1,uG2dOffHeight-1,uG2dWidth/2,uG2dHeight/2,uG2dWidth-1,uG2dHeight-1,1);
			

	UART_Printf("Press any key to continue!\n");
	UART_Getc();	
	G2D_SetStencilKey(0, 0, 0, 0, 0);
	G2D_DrawImage(eBpp);


	for(j=0; j<uG2dOffHeight; j++)
		for(i=0; i<uG2dOffWidth; i++)
			if(i<(uG2dOffWidth/2) && j<(uG2dOffHeight/2)) GLIB_PutPixel(i, j, G2D_RED);
			else if(i>=(uG2dOffWidth/2) && j<(uG2dOffHeight/2)) GLIB_PutPixel2(i, j, uStencilColor);
			else if(i<(uG2dOffWidth/2) && j>=(uG2dOffHeight/2)) GLIB_PutPixel2(i, j, uStencilColor);
			else GLIB_PutPixel(i, j, G2D_MAGENTA);
			
	UART_Printf("Press any key to test STENCIL with Inverse!\n");
	UART_Getc();

	uIsROn=uIsGOn=uIsBOn=uIsInvOn=true;
	uIsAOn=false;
	uAMin=uAMax=0x0;
	G2D_SetStencilMinMax(uAMin, uAMax, uRMin, uRMax, uGMin, uGMax, uBMin, uBMax, eBpp);
	G2D_SetStencilKey(uIsInvOn, uIsAOn, uIsROn, uIsGOn, uIsBOn);


	G2D_BitBlt(0,0,uG2dOffWidth-1,uG2dOffHeight-1,uG2dWidth/2,uG2dHeight/2,uG2dWidth-1,uG2dHeight-1,1);
			

	UART_Printf("Press any key to continue!\n");
	UART_Getc();	
	
	
	G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);	


	
#ifdef INTERRUPT_MODE	
	G2D_InterruptDisable();
	INTC_Disable(NUM_2D);
#endif

	G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);	

}

void TestMaxResolutionWithPointDrawing(void)
{
	u32 i,j,k;
	u32 uG2dFbAddr, uG2dWidth, uG2dHeight, uG2DWidthBackup, uG2dHeightBackup;
	CSPACE eBpp, eBppBackup;
	u8 bErrorFlag=0;
	u32 uOrgData, uFbData;
	u32 uOrgDataConverted, uFbDataConverted;
	u8 ucReadSize;

#ifdef INTERRUPT_MODE
	INTC_SetVectAddr(NUM_2D, Isr_G2d);
	INTC_Enable(NUM_2D);
	G2D_InterruptEnable();
#endif



	G2D_GetFrmSz(&uG2dWidth, &uG2dHeight);
	G2D_GetFbAddr(&uG2dFbAddr);	
	G2D_GetSrcBppMode(&eBpp);
		
	LCD_Stop();


	eBppBackup=eBpp;
	uG2DWidthBackup=uG2dWidth;
	uG2dHeightBackup=uG2dHeight;

	uG2dWidth=G2D_MAX_WIDTH;
	uG2dHeight=G2D_MAX_WIDTH;

	UART_Printf("Width=%d, Height=%d, Framebuffer:0x%x\n", uG2dWidth, uG2dHeight, uG2dFbAddr);
	UART_Printf("Maximum Resolution Test with Point Drawing!\n");
	
	
	for(k=0; k<6; k++) {
		if(k==0) 		{ UART_Printf("[RGB 565]");	eBpp=RGB16; }
		else if(k==1)	{ UART_Printf("[RGBA 5551]");	eBpp=RGBA16;}
		else if(k==2)	{ UART_Printf("[ARGB 1555]");	eBpp=ARGB16;}
		else if(k==3)	{ UART_Printf("[RGBA 8888]");	eBpp=RGBA24;}				
		else if(k==4)	{ UART_Printf("[ARGB 8888]");	eBpp=ARGB24;}
		else if(k==5)	{ UART_Printf("[XRGB 8888]");	eBpp=RGB24;}
		else if(k==6)	{ UART_Printf("[RGBX 8888]");	eBpp=RGBX24;}				
		
		G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);	
		
		for(j=0; j<uG2dHeight; j++)
			for(i=0; i<uG2dWidth; i++)
				G2D_PutPixelEcolor(i, j, (G2D_COLOR)((i*j)%8));
				
		for(j=0; j<uG2dHeight; j++)
			for(i=0; i<uG2dWidth; i++)	{
				G2D_ConvertEcolorToRaw((G2D_COLOR)((i*j)%8), &uOrgData);
				G2d_Convert24bpp(uOrgData, eBpp, 1, &uOrgDataConverted);
				G2D_GetFbData(i, j, uG2dWidth, uG2dFbAddr, eBpp, &uFbData);
				G2d_Convert24bpp(uFbData, eBpp, 1, &uFbDataConverted);
				switch(k)
				{
					case 0:
					case 1:
					case 2:
						if(uOrgDataConverted != uFbDataConverted) {
							UART_Printf("Error!\n");
							bErrorFlag=1;
						}
						break;
					case 3:
					case 6:
						if((uOrgDataConverted&0xffffff00) != (uFbDataConverted&0xffffff00)) {
							UART_Printf("Error!\n");
							bErrorFlag=1;
						}
						break;
					case 4:
					case 5:
						if((uOrgDataConverted&0xffffff) != (uFbDataConverted&0xffffff)) {
							UART_Printf("Error!\n");
							bErrorFlag=1;
						}
						break;					
				}
			}	
		if(!bErrorFlag) UART_Printf(" OK!\n");
	}
		
#ifdef INTERRUPT_MODE	
	G2D_InterruptDisable();
	INTC_Disable(NUM_2D);
#endif

	G2D_InitSetting(uG2dFbAddr, eBppBackup, uG2DWidthBackup, uG2dHeightBackup, 0, 0, uG2DWidthBackup, uG2dHeightBackup);	
}



void TestMaxResolutionWithBitBLT(void)
{
	u32 i,j,k;
	u32 uG2dFbAddr, uG2dWidth, uG2dHeight, uG2DWidthBackup, uG2dHeightBackup;
	CSPACE eBpp, eBppBackup;
	u8 bErrorFlag=0;
	u32 uOrgData, uFbData;
	u32 uOrgDataConverted, uFbDataConverted;
	u8 ucReadSize;

#ifdef INTERRUPT_MODE
	INTC_SetVectAddr(NUM_2D, Isr_G2d);
	INTC_Enable(NUM_2D);
	G2D_InterruptEnable();
#endif


	G2D_GetFrmSz(&uG2dWidth, &uG2dHeight);
	G2D_GetFbAddr(&uG2dFbAddr);	
	G2D_GetSrcBppMode(&eBpp);
		
	LCD_Stop();


	eBppBackup=eBpp;
	uG2DWidthBackup=uG2dWidth;
	uG2dHeightBackup=uG2dHeight;


	uG2dWidth=G2D_MAX_WIDTH;
	uG2dHeight=G2D_MAX_WIDTH;

	UART_Printf("Width=%d, Height=%d, Framebuffer:0x%x\n", uG2dWidth, uG2dHeight, uG2dFbAddr);
	UART_Printf("Maximum Resolution Test with BitBLT!\n");
	
	
	for(k=0; k<6; k++) {
		if(k==0) 		{ UART_Printf("[RGB 565]");	eBpp=RGB16; }
		else if(k==1)	{ UART_Printf("[RGBA 5551]");	eBpp=RGBA16;}
		else if(k==2)	{ UART_Printf("[ARGB 1555]");	eBpp=ARGB16;}
		else if(k==3)	{ UART_Printf("[RGBA 8888]");	eBpp=RGBA24;}				
		else if(k==4)	{ UART_Printf("[ARGB 8888]");	eBpp=ARGB24;}
		else if(k==5)	{ UART_Printf("[XRGB 8888]");	eBpp=RGB24;}
		else if(k==6)	{ UART_Printf("[RGBX 8888]");	eBpp=RGBX24;}				
		
		G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);

		for(j=0; j<uG2dHeight; j++)
			for(i=0; i<uG2dWidth; i++)
				G2D_PutPixelEcolor(i, j, (G2D_COLOR)((i*j)%8));

		G2D_BitBlt(0, 0, uG2dWidth-1, uG2dHeight-1, 0, 0, uG2dWidth-1, uG2dHeight-1, 0);

		for(j=0; j<uG2dHeight; j++)
			for(i=0; i<uG2dWidth; i++)	{
				G2D_ConvertEcolorToRaw((G2D_COLOR)((i*j)%8), &uOrgData);
				G2d_Convert24bpp(uOrgData, eBpp, 1, &uOrgDataConverted);
				G2D_GetFbData(i, j, uG2dWidth, uG2dFbAddr, eBpp, &uFbData);
				G2d_Convert24bpp(uFbData, eBpp, 1, &uFbDataConverted);
				switch(k)
				{
					case 0:
					case 1:
					case 2:
						if(uOrgDataConverted != uFbDataConverted) {
							UART_Printf("Error!\n");
							bErrorFlag=1;
						}
						break;
					case 3:
					case 6:
						if((uOrgDataConverted&0xffffff00) != (uFbDataConverted&0xffffff00)) {
							UART_Printf("Error!\n");
							bErrorFlag=1;
						}
						break;
					case 4:
					case 5:
						if((uOrgDataConverted&0xffffff) != (uFbDataConverted&0xffffff)) {
							UART_Printf("Error!\n");
							bErrorFlag=1;
						}
						break;					
				}	
			}	
		if(!bErrorFlag) UART_Printf(" OK!\n");
	}	

#ifdef INTERRUPT_MODE	
	G2D_InterruptDisable();
	INTC_Disable(NUM_2D);
#endif

	G2D_InitSetting(uG2dFbAddr, eBppBackup, uG2DWidthBackup, uG2dHeightBackup, 0, 0, uG2DWidthBackup, uG2dHeightBackup);	
}


void TestPerformanceWithBitBLT(void)
{
	u32 i,j,k,l;
	u32 uG2dFbAddr, uG2dWidth, uG2dHeight, uG2DWidthBackup, uG2dHeightBackup;
	CSPACE eBpp, eBppBackup;
	u8 bErrorFlag=0;
	u32 uOrgX, uOrgY, uRType;
	u8 ucReadSize;
	s32 sResolution;

#ifdef INTERRUPT_MODE
	INTC_SetVectAddr(NUM_2D, Isr_G2d);
	INTC_Enable(NUM_2D);
	G2D_InterruptEnable();
#endif


	G2D_GetFrmSz(&uG2dWidth, &uG2dHeight);
	G2D_GetFbAddr(&uG2dFbAddr);	
	G2D_GetSrcBppMode(&eBpp); //v2.0, Src & Dest Color mode need to be test separately later.
		
	LCD_Stop();
	UART_Printf("Performance Test, Press any key to continue!\n");
#ifdef WITHOUT
	UART_Printf("BitBLT ONLY!!!\n");
#endif
#ifdef WITHALL
		UART_Printf("BitBLT with ROTATION & BLENDING!!!\n");
#endif
#ifdef WITHROTATION
		UART_Printf("BitBLT with ROTATION!!!\n");
#endif
#ifdef WITHBLENDING
		UART_Printf("BitBLT with BLENDING!!!\n");
#endif



	UART_Getc();

	eBppBackup=eBpp;
	uG2DWidthBackup=uG2dWidth;
	uG2dHeightBackup=uG2dHeight;

//	UART_Printf("Select the Resolution[0:QVGA, 1:WQVGA, 2: VGA, 3: WVGA, 4: SVGA, 5:XGA, 6:SXGA, 7:UXGA, 8:Maximum]:");
//	sResolution=GetInt();

	for(l=0; l<9; l++) {
		sResolution=l;
		switch(sResolution) {
			case 0:			
				uG2dWidth=320;
				uG2dHeight=240;
				break;
			case 1: 		
				uG2dWidth=480;
				uG2dHeight=272;
				break;
			case 2:			
				uG2dWidth=640;
				uG2dHeight=480;
				break;
			case 3:			
				uG2dWidth=800;
				uG2dHeight=480;
				break;
			case 4: 		
				uG2dWidth=800;
				uG2dHeight=600;
				break;
			case 5:			
				uG2dWidth=1024;
				uG2dHeight=768;
				break;
			case 6:			
				uG2dWidth=1280;
				uG2dHeight=1024;
				break;			
			case 7: 		
				uG2dWidth=1600;
				uG2dHeight=1200;
				break;			
			case 8:			
				uG2dWidth=G2D_MAX_WIDTH;
				uG2dHeight=G2D_MAX_HEIGHT;
				break;			
			default:			
				uG2dWidth=G2D_MAX_WIDTH;
				uG2dHeight=G2D_MAX_HEIGHT;
				break;						
		}

#ifdef MSGPRT
		UART_Printf("Width=%d, Height=%d, Framebuffer:0x%x\n", uG2dWidth, uG2dHeight, uG2dFbAddr);
		UART_Printf("Maximum Resolution Test with BitBLT!\n");
#endif		
		
		for(ucReadSize=0; ucReadSize<4; ucReadSize++) {
#ifdef MSGPRT			
			if(ucReadSize==0) UART_Printf("Read Busrt 1 Test!\n");
			else if(ucReadSize==1) UART_Printf("Read Busrt 4 Test!\n");
			else if(ucReadSize==2) UART_Printf("Read Busrt 8 Test!\n");
			else if(ucReadSize==3) UART_Printf("Read Busrt 16 Test!\n");		
			else Assert(0);		
#endif			
			for(k=0; k<4; k++) {
#ifdef MSGPRT				
				if(k==0)  UART_Printf("[15BPP]");
				else if(k==1)  UART_Printf("[16BPP]");
				else if(k==2)  UART_Printf("[18BPP]");
				else if(k==3)  UART_Printf("[24BPP]");
#endif				
				if(k==0)  eBpp=ARGB8; 
				else if(k==1)  eBpp=RGB16;
				else if(k==2)  eBpp=RGB18;
				else if(k==3)  eBpp=RGB24;	

				G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);	
//				G2d_SetReadSize(ucReadSize);

				for(j=0; j<uG2dHeight; j++)
					for(i=0; i<uG2dWidth; i++)
						G2D_PutPixelEcolor(i, j, (G2D_COLOR)((i*j)%8));
					
				INTC_SetVectAddr(NUM_2D, Isr_G2dProfile);
#ifdef WITHROTATION
				uRType=G2D_ROTATION_90_DEG_BIT;
				uOrgX=uG2dWidth/2-1;
				uOrgY=uG2dHeight/2-1;
				G2D_SetRotationMode(uRType);
				G2D_SetRotationOrg((u16)uOrgX, (u16)uOrgY);
#endif
#ifdef WITHBLENDING
				G2D_SetAlphaMode(G2D_ALPHA_MODE);
				G2D_SetAlphaValue(128);
#endif
#ifdef WITHALL
				uRType=G2D_ROTATION_90_DEG_BIT;
				uOrgX=uG2dWidth/2-1;
				uOrgY=uG2dHeight/2-1;
				G2D_SetRotationMode(uRType);
				G2D_SetRotationOrg((u16)uOrgX, (u16)uOrgY);
				G2D_SetAlphaMode(G2D_ALPHA_MODE);
				G2D_SetAlphaValue(128);
#endif
				G2D_BitBlt(0, 0, uG2dWidth-1, uG2dHeight-1, 0, 0, uG2dWidth-1, uG2dHeight-1, 0);

				INTC_SetVectAddr(NUM_2D, Isr_G2d);
		
				for(j=0; j<uG2dHeight; j++)
					for(i=0; i<uG2dWidth; i++)	{
#ifdef DATACOMP 
#ifdef WITHBLENDING
						G2D_ConvertEcolorToRaw((i*j)%8, &uSrcData);
						G2D_ConvertEcolorToRaw((i*j)%8, &uDstData);
						G2d_Convert24bpp(uSrcData, eBpp, 1, &uSrcDataConverted);
						G2d_Convert24bpp(uDstData, eBpp, 1, &uDstDataConverted);						
						G2D_GetFbData(i, j, uG2dWidth, uG2dFbAddr, eBpp, &uFbData);
						G2d_Convert24bpp(uFbData, eBpp, 1, &uFbDataConverted);						
						G2D_BlendingOut(uSrcDataConverted, uDstDataConverted, (u8)128, false, 0, &uBlendingOut);
						if(uBlendingOut != uFbDataConverted) {
							UART_Printf("Blending Error\n");
							bErrorFlag=1;
							}	
#endif
#endif
#ifdef DATACOMP 
#ifdef WITHROTATION
						G2D_GetRotateCoordinate(i, j, uOrgX, uOrgY, uRType, &uRsltX, &uRsltY);
						if((uRsltX<uG2dWidth) && (uRsltY<uG2dHeight)) {
							G2D_ConvertEcolorToRaw((i*j)%8, &uSrcData);
							G2d_Convert24bpp(uSrcData, eBpp, 1, &uSrcDataConverted);		
							G2D_GetFbData(uRsltX, uRsltY, uG2dWidth, uG2dFbAddr, eBpp, &uFbData);
							G2d_Convert24bpp(uFbData, eBpp, 1, &uFbDataConverted);							
							if(uSrcDataConverted != uFbDataConverted) {
								UART_Printf("Rotation Error!\n");
								bErrorFlag=1;
							}	
						}						
#endif
#endif
#ifdef  DATACOMP
#ifdef WITHALL
						G2D_ConvertEcolorToRaw((i*j)%8, &uSrcData);
						G2D_ConvertEcolorToRaw((i*j)%8, &uDstData);
						G2d_Convert24bpp(uSrcData, eBpp, 1, &uSrcDataConverted);
						G2d_Convert24bpp(uDstData, eBpp, 1, &uDstDataConverted);						
						G2D_GetFbData(i, j, uG2dWidth, uG2dFbAddr, eBpp, &uFbData);
						G2d_Convert24bpp(uFbData, eBpp, 1, &uFbDataConverted);						
						G2D_BlendingOut(uSrcDataConverted, uDstDataConverted, (u8)128, false, 0, &uBlendingOut);
						
						G2D_GetRotateCoordinate(i, j, uOrgX, uOrgY, uRType, &uRsltX, &uRsltY);
						if((uRsltX<uG2dWidth) && (uRsltY<uG2dHeight)) {
							G2D_GetFbData(uRsltX, uRsltY, uG2dWidth, uG2dFbAddr, eBpp, &uFbData);
							G2d_Convert24bpp(uFbData, eBpp, 1, &uFbDataConverted);							
							if(uBlendingOut != uFbDataConverted) {
								UART_Printf("Blending & Rotation Error!\n");
								bErrorFlag=1;
							}
						}	

#endif
#endif

					}	

				if(bErrorFlag) {UART_Printf(" Error!\n"); bErrorFlag=0;}

				}
			}	
		}	


#ifdef INTERRUPT_MODE	
	G2D_InterruptDisable();
	INTC_Disable(NUM_2D);
#endif

	G2D_InitSetting(uG2dFbAddr, eBppBackup, uG2DWidthBackup, uG2dHeightBackup, 0, 0, uG2DWidthBackup, uG2dHeightBackup);	
}



void TestPerformanceCS(void)
{
	u32 i,j,k,l;
	u32 uG2dFbAddr, uG2dWidth, uG2dHeight, uG2DWidthBackup, uG2dHeightBackup;
	CSPACE eBpp, eBppBackup;
	u8 bErrorFlag=0;
	u32 uOrgX, uOrgY, uRType;
	u8 ucReadSize;
	s32 sResolution;

#ifdef INTERRUPT_MO
	INTC_SetVectAddr(NUM_2D, Isr_G2d);
	INTC_Enable(NUM_2D);
	G2D_InterruptEnable();
#endif


	G2D_GetFrmSz(&uG2dWidth, &uG2dHeight);
	G2D_GetFbAddr(&uG2dFbAddr);	
	G2D_GetSrcBppMode(&eBpp);
		
	LCD_Stop();
	UART_Printf("Performance Test, Press any key to continue!\n");
#ifdef WITHOUT
	UART_Printf("BitBLT ONLY!!!\n");
#endif
#ifdef WITHALL
		UART_Printf("BitBLT with ROTATION & BLENDING!!!\n");
#endif
#ifdef WITHROTATION
		UART_Printf("BitBLT with ROTATION!!!\n");
#endif
#ifdef WITHBLENDING
		UART_Printf("BitBLT with BLENDING!!!\n");
#endif



	UART_Getc();

	eBppBackup=eBpp;
	uG2DWidthBackup=uG2dWidth;
	uG2dHeightBackup=uG2dHeight;

//	UART_Printf("Select the Resolution[0:QVGA, 1:WQVGA, 2: VGA, 3: WVGA, 4: SVGA, 5:XGA, 6:SXGA, 7:UXGA, 8:Maximum]:");
//	sResolution=GetInt();

	for(l=0; l<9; l++) {
		sResolution=l;
		switch(sResolution) {
			case 0:			
				uG2dWidth=320;
				uG2dHeight=240;
				break;
			case 1: 		
				uG2dWidth=480;
				uG2dHeight=272;
				break;
			case 2:			
				uG2dWidth=640;
				uG2dHeight=480;
				break;
			case 3:			
				uG2dWidth=800;
				uG2dHeight=480;
				break;
			case 4: 		
				uG2dWidth=800;
				uG2dHeight=600;
				break;
			case 5:			
				uG2dWidth=1024;
				uG2dHeight=768;
				break;
			case 6:			
				uG2dWidth=1280;
				uG2dHeight=1024;
				break;			
			case 7: 		
				uG2dWidth=1600;
				uG2dHeight=1200;
				break;			
			case 8:			
				uG2dWidth=G2D_MAX_WIDTH;
				uG2dHeight=G2D_MAX_HEIGHT;
				break;			
			default:			
				uG2dWidth=G2D_MAX_WIDTH;
				uG2dHeight=G2D_MAX_HEIGHT;
				break;						
		}

#ifdef MSGPRT
		UART_Printf("Width=%d, Height=%d, Framebuffer:0x%x\n", uG2dWidth, uG2dHeight, uG2dFbAddr);
		UART_Printf("Maximum Resolution Test with BitBLT!\n");
#endif		
		
		for(ucReadSize=0; ucReadSize<4; ucReadSize++) {
#ifdef MSGPRT			
			if(ucReadSize==0) UART_Printf("Read Busrt 1 Test!\n");
			else if(ucReadSize==1) UART_Printf("Read Busrt 4 Test!\n");
			else if(ucReadSize==2) UART_Printf("Read Busrt 8 Test!\n");
			else if(ucReadSize==3) UART_Printf("Read Busrt 16 Test!\n");		
			else Assert(0);		
#endif			
			for(k=0; k<4; k++) {
#ifdef MSGPRT				
				if(k==0)  UART_Printf("[15BPP]");
				else if(k==1)  UART_Printf("[16BPP]");
				else if(k==2)  UART_Printf("[18BPP]");
				else if(k==3)  UART_Printf("[24BPP]");
#endif				
				if(k==0)  eBpp=ARGB8; 
				else if(k==1)  eBpp=RGB16;
				else if(k==2)  eBpp=RGB18;
				else if(k==3)  eBpp=RGB24;	

				G2D_InitSetting(uG2dFbAddr, eBpp, uG2dWidth, uG2dHeight, 0, 0, uG2dWidth, uG2dHeight);	
//				G2d_SetReadSize(ucReadSize);

				for(j=0; j<uG2dHeight; j++)
					for(i=0; i<uG2dWidth; i++)
						G2D_PutPixelEcolor(i, j, (G2D_COLOR)((i*j)%8));
					
				INTC_SetVectAddr(NUM_2D, Isr_G2dProfile);
#ifdef WITHROTATION
				uRType=G2D_ROTATION_90_DEG_BIT;
				uOrgX=uG2dWidth/2-1;
				uOrgY=uG2dHeight/2-1;
				G2D_SetRotationMode(uRType);
				G2D_SetRotationOrg((u16)uOrgX, (u16)uOrgY);
#endif
#ifdef WITHBLENDING
				G2D_SetAlphaMode(G2D_ALPHA_MODE);
				G2D_SetAlphaValue(128);
#endif
#ifdef WITHALL
				uRType=G2D_ROTATION_90_DEG_BIT;
				uOrgX=uG2dWidth/2-1;
				uOrgY=uG2dHeight/2-1;
				G2D_SetRotationMode(uRType);
				G2D_SetRotationOrg((u16)uOrgX, (u16)uOrgY);
				G2D_SetAlphaMode(G2D_ALPHA_MODE);
				G2D_SetAlphaValue(128);
#endif
				G2D_BitBlt(0, 0, uG2dWidth-1, uG2dHeight-1, 0, 0, uG2dWidth-1, uG2dHeight-1, 0);

				INTC_SetVectAddr(NUM_2D, Isr_G2d);
		
				for(j=0; j<uG2dHeight; j++)
					for(i=0; i<uG2dWidth; i++)	{
#ifdef DATACOMP 
#ifdef WITHBLENDING
						G2D_ConvertEcolorToRaw((i*j)%8, &uSrcData);
						G2D_ConvertEcolorToRaw((i*j)%8, &uDstData);
						G2d_Convert24bpp(uSrcData, eBpp, 1, &uSrcDataConverted);
						G2d_Convert24bpp(uDstData, eBpp, 1, &uDstDataConverted);						
						G2D_GetFbData(i, j, uG2dWidth, uG2dFbAddr, eBpp, &uFbData);
						G2d_Convert24bpp(uFbData, eBpp, 1, &uFbDataConverted);						
						G2D_BlendingOut(uSrcDataConverted, uDstDataConverted, (u8)128, false, 0, &uBlendingOut);
						if(uBlendingOut != uFbDataConverted) {
							UART_Printf("Blending Error\n");
							bErrorFlag=1;
							}	
#endif
#endif
#ifdef DATACOMP 
#ifdef WITHROTATION
						G2D_GetRotateCoordinate(i, j, uOrgX, uOrgY, uRType, &uRsltX, &uRsltY);
						if((uRsltX<uG2dWidth) && (uRsltY<uG2dHeight)) {
							G2D_ConvertEcolorToRaw((i*j)%8, &uSrcData);
							G2d_Convert24bpp(uSrcData, eBpp, 1, &uSrcDataConverted);		
							G2D_GetFbData(uRsltX, uRsltY, uG2dWidth, uG2dFbAddr, eBpp, &uFbData);
							G2d_Convert24bpp(uFbData, eBpp, 1, &uFbDataConverted);							
							if(uSrcDataConverted != uFbDataConverted) {
								UART_Printf("Rotation Error!\n");
								bErrorFlag=1;
							}	
						}						
#endif
#endif
#ifdef  DATACOMP
#ifdef WITHALL
						G2D_ConvertEcolorToRaw((i*j)%8, &uSrcData);
						G2D_ConvertEcolorToRaw((i*j)%8, &uDstData);
						G2d_Convert24bpp(uSrcData, eBpp, 1, &uSrcDataConverted);
						G2d_Convert24bpp(uDstData, eBpp, 1, &uDstDataConverted);						
						G2D_GetFbData(i, j, uG2dWidth, uG2dFbAddr, eBpp, &uFbData);
						G2d_Convert24bpp(uFbData, eBpp, 1, &uFbDataConverted);						
						G2D_BlendingOut(uSrcDataConverted, uDstDataConverted, (u8)128, false, 0, &uBlendingOut);
						
						G2D_GetRotateCoordinate(i, j, uOrgX, uOrgY, uRType, &uRsltX, &uRsltY);
						if((uRsltX<uG2dWidth) && (uRsltY<uG2dHeight)) {
							G2D_GetFbData(uRsltX, uRsltY, uG2dWidth, uG2dFbAddr, eBpp, &uFbData);
							G2d_Convert24bpp(uFbData, eBpp, 1, &uFbDataConverted);							
							if(uBlendingOut != uFbDataConverted) {
								UART_Printf("Blending & Rotation Error!\n");
								bErrorFlag=1;
							}
						}	

#endif
#endif

					}	

				if(bErrorFlag) {UART_Printf(" Error!\n"); bErrorFlag=0;}

				}
			}	
		}	


#ifdef INTERRUPT_MODE	
	G2D_InterruptDisable();
	INTC_Disable(NUM_2D);
#endif

	G2D_InitSetting(uG2dFbAddr, eBppBackup, uG2DWidthBackup, uG2dHeightBackup, 0, 0, uG2DWidthBackup, uG2dHeightBackup);	
}


testFuncMenu line_menu[]=
{
	TestPointDrawingSimple,						"Point Drawing witout Operation",
	TestPointDrawingWithRop,					"Point Drawing with ROP",
	TestPointDrawingWithRotation,				"Point Drawing with Rotation",
	TestPointDrawingWithBlending,				"Point Drawing with Blending",
	TestPointDrawingWithClipping,				"Point Drawing with Clipping",
	TestPointDrawingWithAll,						"Point Drawing with All Operation",
	TestLineDrawingSimple,						"Line Drawing without Operation",
	TestLineDrawingWithRop,						"Line Drawing with ROP",
	TestLineDrawingWithRotation,					"Line Drawing with Rotation",
	TestLineDrawingWithBlending,				"Line Drawing with Blending",
	TestLineDrawingClipping,						"Line Drawing with Clipping",
	TestLineDrawingWithAll,						"Line Drawing with All Operation",
	TestLineDrawingLastPixel,					"Line Drawing Do not Draw Last Pixel",


// CAUTION: The routines below are for only internal validation, It should be eliminated from release version.
	0,0
};

testFuncMenu color_menu[]=
{
	TestColorExpansionSimple,					"Color Expansion without Operation",
	TestColorExpansionWithRop,					"Color Expansion with ROP",
	TestColorExpansionWithRotation,				"Color Expansion with Rotation",	
	TestColorExpansionWithBlending,				"Color Expansion with Blending",
	TestColorExpansionWithClipping,				"Color Expansion with Clipping",
	TestColorExpansionTransparency,				"Color Expansion with Transparency",
	TestColorExpansionWithAll,					"Color Expansion with All Operation",

// CAUTION: The routines below are for only internal validation, It should be eliminated from release version.
	0,0
};


testFuncMenu bitblt_menu[]=
{
	TestOnScrBitBLTSimple,							"On Screen BitBLT without Operation",
	TestOnScrBitBLTWithRop,							"On Screen BitBLT with ROP",
	TestOnScrBitBLTWithRotation,					"On Screen BitBLT with Rotation",
	TestOnScrBitBLTWithBlending,					"On Screen BitBLT with Blending",
	TestOnScrBitBLTWithClipping,					"On Screen BitBLT with Clipping",
	TestOnScrBitBLTWithTransparency,				"On Screen BitBLT with Transparency",
	TestOnScrBitBLTWithAll,							"On Screen BitBLT with All Operation",
	TestOffScrBitBLTSimple,							"Off Screen BitBLT without Operation",
	TestOffScrBitBLTWithRop,						"Off Screen BitBLT with ROP",
	TestOffScrBitBLTWithRotation,					"Off Screen BitBLT with Rotation",
	TestOffScrBitBLTWithBlending,					"Off Screen BitBLT with Blending",
	TestOffScrBitBLTWithClipping,					"Off Screen BitBLT with Clipping",
	TestOffScrBitBLTWithTransparency,				"Off Screen BitBLT with Transparency",
	TestOffScrBitBLTWithAll,						"Off Screen BitBLT with All Operation",	
	TestHostToScrBitBLTSimple,						"Host to Screen BitBLT without Operation",
	TestHostToScrBitBLTWithRop,						"Host to Screen BitBLT with ROP",
	TestHostToScrBitBLTWithRotation,				"Host to Screen BitBLT with Rotation",
	TestHostToScrBitBLTWithBlending,				"Host to Screen BitBLT with Blending",
	TestHostToScrBitBLTWithClipping,				"Host to Screen BitBLT with Clipping",
	TestHostToScrBitBLTWithTransparency,			"Host to Screen BitBLT with Transparency",
	

// CAUTION: The routines below are for only internal validation, It should be eliminated from release version.
	0,0
};


testFuncMenu alpha_menu[]=
{
	TestBlendingWithPerPlane,					"Blending with PerPlane",
	TestBlendingWithPerPixel	,					"Blending with PerPixel",
	TestFadingWithPerPlane,						"Fading with PerPlane",
	0,0
};
testFuncMenu clipping_menu[]=
{
	TestClippingMaxScreen, 						"Clipping with Max Screen",
	TestClippingWindow,	 						"Clipping with Clipping Window",
	0,0
};
testFuncMenu rop_menu[]=
{
	TestRopAllOperation,							"ROP with All Operation",
	TestRopWithThirdOperandChange,				"ROP with Third Operand Change",
	0,0
};
testFuncMenu rotation_menu[]=
{
	TestRotationDegree90,						"Rotate 90 Degree",
	TestRotationDegree180,						"Rotate 180 Degree",
	TestRotationDegree270,						"Rotate 270 Degree",
	TestRotationXFlip,							"Rotate X Flip",
	TestRotationYFlip,							"Rotate Y Flip",	
	0,0
};
testFuncMenu pattern_menu[]=
{
	TestPatternMemoryRW,							"Pattern Memory Write & Read",
	TestPatternMemoryOffset,						"Pattern Memory Offset",
	0,0
};
testFuncMenu bpp_menu[]=
{
	TestChangeBpp,								"Change Color Mode",
	TestCombinationBpp,							"BitBLT with different BPP",

	0,0
};
testFuncMenu stencil_menu[]=
{
	TestStencilWithPointDrwaing,					"Stencil Test with PointDrawing",
	TestStencilWithOnScrBitBLT,						"Stencil Test with On Screen BitBLT",
	TestStencilWithOffScrBitBLT,					"Stencil Test with Off Screen BitBLT",	
	0,0
};
testFuncMenu interrupt_menu[]=
{
	TestInterruptDrawingEngineFinish,		"Interrupt for DE Engine Finish",
	TestInterruptCommandFifoFinish,			"Interrupt for CMD Finish",
	TestInterruptOverflow,					"Interrupt for OVERFLOW",	
	TestInterruptFifoLevel,					"Interrupt for FIFO Level",
	0,0
};
testFuncMenu endianread_menu[]=
{
	//TestReadBurstChange,					"Read Burst Change",
	0,0
};

testFuncMenu maxres_menu[]=
{
	TestMaxResolutionWithPointDrawing,			"Maximum Resolution with Point Drawing",		
	TestMaxResolutionWithBitBLT,					"Maximum Resolution with BitBLT",
	0,0
};

testFuncMenu performance_menu[]=
{
	TestPerformanceWithBitBLT,							"Performance Test",
	TestPerformanceCS,							"Performance Test",		
	0,0
};


void Test_Display(testFuncMenu *menu)
{
	u32 i;
	s32 sSel;
	
	while(1)
	{
		for (i=0; (u32)(menu[i].desc)!=0; i++) {
			UART_Printf("%2d: %s\n", i, menu[i].desc);
		}
		UART_Printf("\nSelect the function to test : ");
		sSel = UART_GetIntNum();
		UART_Printf("\n");
		if (sSel ==  -1)
			break;
		else if (sSel>=0 && sSel<(sizeof(menu)/8-1))
			 ( menu[sSel].func) ();
	}   

}

void Test_LineDrawing(void)				{Test_Display(line_menu);}
void Test_ColorExpansion(void)			{Test_Display(color_menu);}
void Test_BitBlt(void)					{Test_Display(bitblt_menu);}
void Test_AlphaBlending(void)			{Test_Display(alpha_menu);}
void Test_WindowClipping(void)			{Test_Display(clipping_menu);}
void Test_RasterOperation(void)			{Test_Display(rop_menu);}
void Test_Rotation(void)				{Test_Display(rotation_menu);}
void Test_PatternMemroy(void)			{Test_Display(pattern_menu);}
void Test_BppMode(void)					{Test_Display(bpp_menu);}
void Test_Stencil(void)					{Test_Display(stencil_menu);}
void Test_Interrupt(void)				{Test_Display(interrupt_menu);}
void Test_EndianRead(void)				{Test_Display(endianread_menu);}
void Test_MaximumResolution(void)		{Test_Display(maxres_menu);}
void Test_Performance(void)				{Test_Display(performance_menu);}
void Test_Something(void)				
{
	TestMaxResolutionWithBitBLT();
	TestMaxResolutionWithPointDrawing();
	TestBlendingWithPerPlane();
	TestFadingWithPerPlane();		
	TestBlendingWithPerPixel();
}

void G2D_Test(void)
{
	testFuncMenu g2d_menu[]=
	{
		Test_LineDrawing,						"Line Drawing",
		Test_ColorExpansion,					"Color Expansion",
		Test_BitBlt,							"BitBLT",
		Test_AlphaBlending,						"Alpha Blending",
		Test_WindowClipping, 					"Window Clipping",
		Test_RasterOperation, 					"Raster Operation",
		Test_Rotation,							"Rotation",
		Test_PatternMemroy,						"Pattern Memory",


// CAUTION: The routines below are for only internal validation, It should be eliminated from release version.
		TestSFR,								"SFR Test",
		Test_BppMode,							"Bpp Mode",
		Test_Stencil,							"Stencil",
		Test_Interrupt,							"Interrupt",
//		Test_EndianRead,						"Endian & Read",
		Test_MaximumResolution,					"Maximum Resolution",
//		Test_Performance,						"Performance      ",
		Test_Something,							"Test Something",
		0,0

	};

	u32 i;
	s32 sSel;


	UART_Printf("[Graphic 2D Test]\n\n");

	INTC_Init();
	
	G2D_Init();	

	while(1)
	{
		for (i=0; (u32)(g2d_menu[i].desc)!=0; i++) {
			UART_Printf("%2d: %s\n", i, g2d_menu[i].desc);
		}

		UART_Printf("\nSelect the function to test : ");
		sSel = UART_GetIntNum();
		UART_Printf("\n");

		if (sSel ==  -1)
			break;
		else if (sSel>=0 && sSel<(sizeof(g2d_menu)/8-1))
			 ( g2d_menu[sSel].func) ();
	}
}
