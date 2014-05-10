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
*	File Name : rotator_test.c
*  
*	File Description : This file includes the test code for rotator.
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
#include "rotator.h"
#include "def.h"
#include "library.h"
#include "intc.h"
#include "camera.h"
#include "timer.h"


void TestRotatorRGB565(void)
{
	u32 i;
	u16 usRotHorSize, usRotVerSize;
	u32 uRotSrcAddr, uRotDstAddr;
	CSPACE eBpp;
	LCD_WINDOW eWin;
	u32 uLcdWidth, uLcdHeight, uLcdFbAddr0, uLcdFbAddr1;
	ROTATOR_ROT_TYPE eRotType;

	UART_Printf("Rotation Test RGB 565!!\n");
	
	INTC_SetVectAddr(NUM_ROTATOR, Isr_Rotator);
	INTC_Enable(NUM_ROTATOR);
	ROTATOR_IntEnable();	

	LCD_GetFrmSz(&uLcdWidth, &uLcdHeight, WIN0);
	ROTATOR_GetLcdFbAddr(&uLcdFbAddr0, &uLcdFbAddr1);
	ROTATOR_GetFbAddr(&uRotSrcAddr, &uRotDstAddr);
	ROTATOR_GetFrmSz(&usRotHorSize, &usRotVerSize);
	ROTATOR_GetBppMode(&eBpp);


	LCD_Stop();
	eWin=WIN0;
	if(eBpp != RGB16) eBpp=RGB16;
	if(uLcdFbAddr0 != uRotDstAddr) uRotDstAddr=uLcdFbAddr0;	
	ROTATOR_InitLcd(eWin, eBpp);
	GLIB_InitInstance(uLcdFbAddr0, uLcdWidth, uLcdHeight, eBpp);
	GLIB_DrawPattern(uLcdWidth, uLcdHeight);	
	LCD_Start();

	for(i=0; i<5; i++) {
		switch(i) {
			case 0 :
				usRotHorSize=(u16)uLcdHeight;
				usRotVerSize=(u16)uLcdWidth;
				eRotType=ROTATOR_TYPE_ROT_90;
				UART_Printf("Press any key to test 90 Rotation!\n");
				break;
			case 1 :
				usRotHorSize=(u16)uLcdWidth;
				usRotVerSize=(u16)uLcdHeight;
				eRotType=ROTATOR_TYPE_ROT_180;
				UART_Printf("Press any key to test 180 Rotation!\n");
				break;
			case 2 :
				usRotHorSize=(u16)uLcdHeight;
				usRotVerSize=(u16)uLcdWidth;
				eRotType=ROTATOR_TYPE_ROT_270;
				UART_Printf("Press any key to test 270 Rotation!\n");
				break;
			case 3 :
				usRotHorSize=(u16)uLcdWidth;
				usRotVerSize=(u16)uLcdHeight;
				eRotType=ROTATOR_TYPE_FLIP_HOR;
				UART_Printf("Press any key to test X FLIP!\n");
				break;
			case 4 :
				usRotHorSize=(u16)uLcdWidth;
				usRotVerSize=(u16)uLcdHeight;
				eRotType=ROTATOR_TYPE_FLIP_VER;
				UART_Printf("Press any key to test Y FLIP!\n");
				break;
			default :
				break;				
		}	
		UART_Getc();
		ROTATOR_DrawImage(eBpp, usRotHorSize, usRotVerSize, uRotSrcAddr);
		ROTATOR_InitSetting(eBpp, usRotHorSize, usRotVerSize, uRotSrcAddr, uRotDstAddr, eRotType);
		ROTATOR_Start();
		ROTATOR_CheckIntDone(); 


	}	

	INTC_Disable(NUM_2D);
	ROTATOR_IntDisable();

}

void TestRotatorRGB888(void)
{
	u32 i;
	u16 usRotHorSize, usRotVerSize;
	u32 uRotSrcAddr, uRotDstAddr;
	CSPACE eBpp;
	LCD_WINDOW eWin;
	u32 uLcdWidth, uLcdHeight, uLcdFbAddr0, uLcdFbAddr1;
	ROTATOR_ROT_TYPE eRotType;

	UART_Printf("Rotation Test RGB 888!!\n");
	
	INTC_SetVectAddr(NUM_ROTATOR, Isr_Rotator);
	INTC_Enable(NUM_ROTATOR);
	ROTATOR_IntEnable();	

	LCD_GetFrmSz(&uLcdWidth, &uLcdHeight, WIN0);
	ROTATOR_GetLcdFbAddr(&uLcdFbAddr0, &uLcdFbAddr1);
	ROTATOR_GetFbAddr(&uRotSrcAddr, &uRotDstAddr);
	ROTATOR_GetFrmSz(&usRotHorSize, &usRotVerSize);
	ROTATOR_GetBppMode(&eBpp);


	LCD_Stop();
	eWin=WIN0;
	if(eBpp != RGB24) eBpp=RGB24;
	if(uLcdFbAddr0 != uRotDstAddr) uRotDstAddr=uLcdFbAddr0;	
	ROTATOR_InitLcd(eWin, eBpp);
	GLIB_InitInstance(uLcdFbAddr0, uLcdWidth, uLcdHeight, eBpp);
	GLIB_DrawPattern(uLcdWidth, uLcdHeight);	
	LCD_Start();

	for(i=0; i<5; i++) {
		switch(i) {
			case 0 :
				usRotHorSize=(u16)uLcdHeight;
				usRotVerSize=(u16)uLcdWidth;
				eRotType=ROTATOR_TYPE_ROT_90;
				UART_Printf("Press any key to test 90 Rotation!\n");
				break;
			case 1 :
				usRotHorSize=(u16)uLcdWidth;
				usRotVerSize=(u16)uLcdHeight;
				eRotType=ROTATOR_TYPE_ROT_180;
				UART_Printf("Press any key to test 180 Rotation!\n");
				break;
			case 2 :
				usRotHorSize=(u16)uLcdHeight;
				usRotVerSize=(u16)uLcdWidth;
				eRotType=ROTATOR_TYPE_ROT_270;
				UART_Printf("Press any key to test 270 Rotation!\n");
				break;
			case 3 :
				usRotHorSize=(u16)uLcdWidth;
				usRotVerSize=(u16)uLcdHeight;
				eRotType=ROTATOR_TYPE_FLIP_HOR;
				UART_Printf("Press any key to test X FLIP!\n");
				break;
			case 4 :
				usRotHorSize=(u16)uLcdWidth;
				usRotVerSize=(u16)uLcdHeight;
				eRotType=ROTATOR_TYPE_FLIP_VER;
				UART_Printf("Press any key to test Y FLIP!\n");
				break;
			default :
				break;				
		}	
		UART_Getc();
		ROTATOR_DrawImage(eBpp, usRotHorSize, usRotVerSize, uRotSrcAddr);
		ROTATOR_InitSetting(eBpp, usRotHorSize, usRotVerSize, uRotSrcAddr, uRotDstAddr, eRotType);
		ROTATOR_Start();
		ROTATOR_CheckIntDone(); 


	}	

	INTC_Disable(NUM_2D);
	ROTATOR_IntDisable();

}

void TestRotatorYC420(void)
{
	u32 i;
	u16 usRotHorSize, usRotVerSize;
	u32 uRotSrcAddr, uRotDstAddr;
	CSPACE eBpp;
	LCD_WINDOW eWin;
	u32 uLcdWidth, uLcdHeight, uLcdFbAddr0, uLcdFbAddr1;
	u32 uOrgFbAddr;
	ROTATOR_ROT_TYPE eRotType;


	//u32 uLcdWidth, uLcdHeight;
	//LCD_GetFrmSz(&uLcdWidth, &uLcdHeight, 0);
	//LCD_GetFbAddr(&uLcdFbAddr, 0);

	UART_Printf("Rotation Test YC420!!\n");
	
	INTC_SetVectAddr(NUM_ROTATOR, Isr_Rotator);
	INTC_Enable(NUM_ROTATOR);
	ROTATOR_IntEnable();	

	LCD_GetFrmSz(&uLcdWidth, &uLcdHeight, WIN0);
	ROTATOR_GetLcdFbAddr(&uLcdFbAddr0, &uLcdFbAddr1);
	ROTATOR_GetFbAddr(&uRotSrcAddr, &uRotDstAddr);
	ROTATOR_GetFrmSz(&usRotHorSize, &usRotVerSize);
	ROTATOR_GetBppMode(&eBpp);


	LCD_Stop();
	eWin=WIN0;
	ROTATOR_InitLcd(eWin, RGB24);
	GLIB_InitInstance(uLcdFbAddr0, uLcdWidth, uLcdHeight, RGB24);
	GLIB_DrawPattern(uLcdWidth, uLcdHeight);	
	LCD_Start();


	eBpp=YC420;
	uRotDstAddr= _DRAM_BaseAddress + 0x06000000;
	uOrgFbAddr= _DRAM_BaseAddress + 0x06800000;


	for(i=0; i<5; i++) {
		switch(i) {
			case 0 :
				usRotHorSize=(u16)uLcdHeight;
				usRotVerSize=(u16)uLcdWidth;
				eRotType=ROTATOR_TYPE_ROT_90;
				UART_Printf("Press any key to test 90 Rotation!\n");
				break;
			case 1 :
				usRotHorSize=(u16)uLcdWidth;
				usRotVerSize=(u16)uLcdHeight;
				eRotType=ROTATOR_TYPE_ROT_180;
				UART_Printf("Press any key to test 180 Rotation!\n");
				break;
			case 2 :
				usRotHorSize=(u16)uLcdHeight;
				usRotVerSize=(u16)uLcdWidth;
				eRotType=ROTATOR_TYPE_ROT_270;
				UART_Printf("Press any key to test 270 Rotation!\n");
				break;
			case 3 :
				usRotHorSize=(u16)uLcdWidth;
				usRotVerSize=(u16)uLcdHeight;
				eRotType=ROTATOR_TYPE_FLIP_HOR;
				UART_Printf("Press any key to test X FLIP!\n");
				break;
			case 4 :
				usRotHorSize=(u16)uLcdWidth;
				usRotVerSize=(u16)uLcdHeight;
				eRotType=ROTATOR_TYPE_FLIP_VER;
				UART_Printf("Press any key to test Y FLIP!\n");
				break;
			default :
				break;				
		}	
		UART_Getc();
		ROTATOR_DrawImage(RGB24, usRotHorSize, usRotVerSize, uOrgFbAddr);	
		ROTATOR_ConvertColorSpace(RGB24, uOrgFbAddr, usRotHorSize, usRotVerSize, eBpp, uRotSrcAddr);
		ROTATOR_InitSetting(eBpp, usRotHorSize, usRotVerSize, uRotSrcAddr, uRotDstAddr, eRotType);
		ROTATOR_Start();
		ROTATOR_CheckIntDone();
		ROTATOR_ConvertColorSpace(eBpp, uRotDstAddr, uLcdWidth, uLcdHeight, RGB24, uLcdFbAddr0);	
	}	

	INTC_Disable(NUM_2D);
	ROTATOR_IntDisable();

}

void TestRotatorYC422(void)
{
	u32 i;
	u16 usRotHorSize, usRotVerSize;
	u32 uRotSrcAddr, uRotDstAddr;
	CSPACE eBpp;
	LCD_WINDOW eWin;
	u32 uLcdWidth, uLcdHeight, uLcdFbAddr0, uLcdFbAddr1;
	u32 uOrgFbAddr;
	ROTATOR_ROT_TYPE eRotType;


	//u32 uLcdWidth, uLcdHeight;
	//LCD_GetFrmSz(&uLcdWidth, &uLcdHeight, 0);
	//LCD_GetFbAddr(&uLcdFbAddr, 0);

	UART_Printf("Rotation Test YC422!!\n");
	
	INTC_SetVectAddr(NUM_ROTATOR, Isr_Rotator);
	INTC_Enable(NUM_ROTATOR);
	ROTATOR_IntEnable();	

	LCD_GetFrmSz(&uLcdWidth, &uLcdHeight, WIN0);
	ROTATOR_GetLcdFbAddr(&uLcdFbAddr0, &uLcdFbAddr1);
	ROTATOR_GetFbAddr(&uRotSrcAddr, &uRotDstAddr);
	ROTATOR_GetFrmSz(&usRotHorSize, &usRotVerSize);
	ROTATOR_GetBppMode(&eBpp);


	LCD_Stop();
	eWin=WIN0;
	ROTATOR_InitLcd(eWin, RGB24);
	GLIB_InitInstance(uLcdFbAddr0, uLcdWidth, uLcdHeight, RGB24);
	GLIB_DrawPattern(uLcdWidth, uLcdHeight);	
	LCD_Start();


	eBpp=YCRYCB; // for POST Processor, CbYCrY
	uRotDstAddr= _DRAM_BaseAddress + 0x06000000;
	uOrgFbAddr= _DRAM_BaseAddress + 0x06800000;


	for(i=0; i<5; i++) {
		switch(i) {
			case 0 :
				usRotHorSize=(u16)uLcdHeight;
				usRotVerSize=(u16)uLcdWidth;
				eRotType=ROTATOR_TYPE_ROT_90;
				UART_Printf("Press any key to test 90 Rotation!\n");
				break;
			case 1 :
				usRotHorSize=(u16)uLcdWidth;
				usRotVerSize=(u16)uLcdHeight;
				eRotType=ROTATOR_TYPE_ROT_180;
				UART_Printf("Press any key to test 180 Rotation!\n");
				break;
			case 2 :
				usRotHorSize=(u16)uLcdHeight;
				usRotVerSize=(u16)uLcdWidth;
				eRotType=ROTATOR_TYPE_ROT_270;
				UART_Printf("Press any key to test 270 Rotation!\n");
				break;
			case 3 :
				usRotHorSize=(u16)uLcdWidth;
				usRotVerSize=(u16)uLcdHeight;
				eRotType=ROTATOR_TYPE_FLIP_HOR;
				UART_Printf("Press any key to test X FLIP!\n");
				break;
			case 4 :
				usRotHorSize=(u16)uLcdWidth;
				usRotVerSize=(u16)uLcdHeight;
				eRotType=ROTATOR_TYPE_FLIP_VER;
				UART_Printf("Press any key to test Y FLIP!\n");
				break;
			default :
				break;				
		}	
		UART_Getc();
		ROTATOR_DrawImage(RGB24, usRotHorSize, usRotVerSize, uOrgFbAddr);	
		ROTATOR_ConvertColorSpace(RGB24, uOrgFbAddr, usRotHorSize, usRotVerSize, eBpp, uRotSrcAddr);
		ROTATOR_InitSetting(eBpp, usRotHorSize, usRotVerSize, uRotSrcAddr, uRotDstAddr, eRotType);
		ROTATOR_Start();
		ROTATOR_CheckIntDone();
		ROTATOR_ConvertColorSpace(eBpp, uRotDstAddr, uLcdWidth, uLcdHeight, RGB24, uLcdFbAddr0);	
	}	

	INTC_Disable(NUM_2D);
	ROTATOR_IntDisable();

}

void TestRotatorInterruptRGB565(void)
{
	u16 usRotHorSize, usRotVerSize;
	u32 uRotSrcAddr, uRotDstAddr;
	CSPACE eBpp;

	ROTATOR_GetFbAddr(&uRotSrcAddr, &uRotDstAddr);
	ROTATOR_GetFrmSz(&usRotHorSize, &usRotVerSize);
	ROTATOR_GetBppMode(&eBpp);

	UART_Printf("Width:%d, Height:%d\n", usRotHorSize, usRotVerSize);

	ROTATOR_InitSetting(eBpp, usRotHorSize, usRotVerSize, uRotSrcAddr, uRotDstAddr, ROTATOR_TYPE_ROT_90);
	ROTATOR_DrawImage(eBpp, usRotHorSize, usRotVerSize, uRotSrcAddr);
	
	INTC_SetVectAddr(NUM_ROTATOR, Isr_Rotator);
	INTC_Enable(NUM_ROTATOR);
	ROTATOR_IntEnable();	
	
	ROTATOR_Start();
	ROTATOR_CheckIntDone();
	UART_Printf("Rotation with Interrupt is finished!\n");
	
	INTC_Disable(NUM_2D);
	ROTATOR_IntDisable();	

}

void TestRotatorStatusRGB565(void)
{
	u16 usRotHorSize, usRotVerSize;
	u32 uRotSrcAddr, uRotDstAddr;
	CSPACE eBpp;
	volatile u32 uSTATCFG;


	ROTATOR_GetFbAddr(&uRotSrcAddr, &uRotDstAddr);
	ROTATOR_GetFrmSz(&usRotHorSize, &usRotVerSize);
	ROTATOR_GetBppMode(&eBpp);

	UART_Printf("Width:%d, Height:%d\n", usRotHorSize, usRotVerSize);

	ROTATOR_InitSetting(eBpp, usRotHorSize, usRotVerSize, uRotSrcAddr, uRotDstAddr, ROTATOR_TYPE_ROT_90);
	ROTATOR_DrawImage(eBpp, usRotHorSize, usRotVerSize, uRotSrcAddr);
	
	ROTATOR_Start();
	ROTATOR_CheckDone();
	UART_Printf("Rotation without Interrupt is finished!\n");
	
	INTC_Disable(NUM_2D);
	ROTATOR_IntDisable();	

}

void TestRotatorOneMoreJob(void)
{
	u16 usRotHorSize, usRotVerSize;
	u32 uRotSrcAddr0, uRotDstAddr0;
	u32 uRotSrcAddr1, uRotDstAddr1;
	u32 uSTATCFG;
	CSPACE eBpp;
	u32 uWidth, uHeight;
	ROTATOR_ROT_TYPE eRotType;


	UART_Printf("Rotation Audit Test with All Size!!\n");
	
	INTC_SetVectAddr(NUM_ROTATOR, Isr_RotatorOneMoreJob);
	INTC_Enable(NUM_ROTATOR);
	ROTATOR_IntEnable();	

	LCD_Stop();

	uWidth=2048;
	uHeight=2048;
	usRotHorSize=usRotVerSize=2048;
	uRotSrcAddr0=  _DRAM_BaseAddress + 0x01000000;
	uRotDstAddr0= uRotSrcAddr0 + uWidth*uHeight*2;
	uRotSrcAddr1= uRotDstAddr0 + uWidth*uHeight*2;
	uRotDstAddr1= uRotSrcAddr1 + uWidth*uHeight*2;

	eBpp=RGB16;
	eRotType=ROTATOR_TYPE_ROT_90;
	ROTATOR_DrawImage(eBpp, usRotHorSize, usRotVerSize, uRotSrcAddr0);	
	ROTATOR_DrawImage(eBpp, usRotHorSize, usRotVerSize, uRotSrcAddr1);	
	UART_Printf("Setting Rotation\n");			
	ROTATOR_InitSetting(eBpp, usRotHorSize, usRotVerSize, uRotSrcAddr0, uRotDstAddr0, eRotType);
	ROTATOR_Start();

	uSTATCFG=Inp32(0x7720002c); //read STATCFG
	UART_Printf("STATCFG:0x%x\n", uSTATCFG);

	ROTATOR_SetSrcAddr(eBpp, uRotSrcAddr1, usRotHorSize, usRotVerSize);
	ROTATOR_SetDstAddr(eBpp, uRotDstAddr1, usRotHorSize, usRotVerSize);
	ROTATOR_Start();

	uSTATCFG=Inp32(0x7720002c); //read STATCFG
	UART_Printf("STATCFG:0x%x\n", uSTATCFG);

	ROTATOR_CheckIntDone();
	if(!ROTATOR_AuditOperation(eBpp, (u32)usRotHorSize, (u32)usRotVerSize, eRotType, uRotSrcAddr0, uRotDstAddr0)) 
		UART_Printf("Rotation Error 0!\n");
	
	ROTATOR_CheckIntDone();
	if(!ROTATOR_AuditOperation(eBpp, (u32)usRotHorSize, (u32)usRotVerSize, eRotType, uRotSrcAddr1, uRotDstAddr1)) 
		UART_Printf("Rotation Error 1!\n");

	UART_Printf("Test OK!\n");
	INTC_Disable(NUM_2D);
	ROTATOR_IntDisable();

}


void ROTATOR_Test(void)
{
	testFuncMenu rotator_menu[]=
	{
		TestRotatorRGB565,						"RGB 565",		
		TestRotatorRGB888,						"RGB 888",		
		TestRotatorYC420,						"YCbYCr 420",
		TestRotatorYC422,						"YCbYCr 422",
		TestRotatorStatusRGB565,				"Checking Status",
		TestRotatorInterruptRGB565,				"Checking Interrupt",		
		TestRotatorOneMoreJob,					"Rotation with One More Job",
		0,0
	};

	u32 i;
	s32 sSel;


	UART_Printf("[Graphic 2D Test]\n\n");
	
	ROTATOR_Init(ROTATOR_TEST_MODE);	

	while(1)
	{
		for (i=0; (u32)(rotator_menu[i].desc)!=0; i++) {
			UART_Printf("%2d: %s\n", i, rotator_menu[i].desc);
		}

		UART_Printf("\nSelect the function to test : ");
		sSel = UART_GetIntNum();
		UART_Printf("\n");

		if (sSel ==  -1)
			break;
		else if (sSel>=0 && sSel<(sizeof(rotator_menu)/8-1))
			 ( rotator_menu[sSel].func) ();

	}
}
