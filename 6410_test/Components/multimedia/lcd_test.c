/********************************************************************************
* 
*	Project Name : S3C6410 Validation
*
*	Copyright 2006 by Samsung Electronics, Inc.
*	All rights reserved.
*
*	Project Description :
*		This software is only for verifying functions of the S3C6410.
*		Anybody can use this software without our permission.
*  
*--------------------------------------------------------------------------------
* 
*	File Name : lcd_test.c
*  
*	File Description : 
*
*	Author	: Yoh-Han Lee
*	Dept. : AP Development Team
*	Created Date : 2006/12/27
*	Version : 0.2
* 
*	History
*	- Created (Y.H.Lee, 2006/12/27)
*   - Release Ver 0.1 (Y.H.Lee, 2007/03/29)
*   - Release Ver 0.2 ((Y.H.Lee, 2007/06/07) 
*********************************************************************************/

#include <string.h>
#include <stdlib.h>

#include "lcd.h"
#include "def.h"
#include "library.h"
#include "glib.h"
#include "uart.h"
#include "intc.h"
#include "camera.h"
#include "post.h"
#include "Power_test.h"
#include "dma.h"
#include "gpio.h"
#include "sysc.h"

//#include "QVGA_24bpp_Image.h"
//#include "SmileAgain.h"
//#include "autumn.h"
#include "Smile800480_24bpp2.h"
#include "Smile800480_24bpp3.h"
//#include "ganhi240320_RGB24.h"
//#include "240320_kang_image.h"
//#include "Kangjjang-01.h"
//#include "Kangjjang-03.h"
//#include "Entertainment1.h"
//#include "Entertainment22.h"
//#include "320176RGB16.h"
//#include "320-174-RGB16.h"
//#include "kangjjang-320240.h"
//#include "Test_RGB24.h"
//#include "640480_RGB24.h"

static DMAC oDmac0, oDmac1, oDmac2, oDmac3;
//Static Global Variabls
static LCD_WINDOW 	eBgWin, eFgWin;
static LCD_WINDOW 	eFgWinArray_Rev[2], eBgWin_Rev;
static LCD_INT_SYS 	eLcdCpuifInt;
static CSPACE 		eBgBpp;
static CSPACE 		eFgBpp, eFgArgbBpp;
static CSPACE 		ePalDataBpp, ePalBpp;
static CSPACE 		eWinBpp[5];
static CSPACE 		eFgBppArray_Rev[2], eBgBpp_Rev;
	
static u8 bIsRev 	= false;
static u8 bIsAllWin    = false;

static u32 uCount;
static u32 uCmdVal[12];

CPU_AUTO_CMD_RATE 	eCpuAutoCmdRate;
CPU_COMMAND_MODE  	eCpuCmdMode;

static u32 			uLcdFbAddr = LCD_FRAMEBUFFER_ST;
static volatile u8         bPreviewDone, bCodecDone;

static POST 		oPost;
static CSPACE 		eSrcDataFmt ;


static u32 uEINT9IRQ = 0 ; // add. jungil 080407
static u32 uPOSTEND = 0 ; // add. jungil 080407
static u32 uPOSTSTART = 0 ; //add. jungil 080407
static u32 uPOSTDoneIssue = 0 ; 
static u32 WinStopIssue = 0 ;

static u32 uLcdHSz, uLcdVSz;	
static u32 uMidImgHSz, uMidImgVSz;
static u32 uMidStartX, uMidStartY;
static u32 uMidScaledHSz, uMidScaledVSz;
static u32 uImgHSz, uImgVSz;	

static u32 uSimpleTest;

#define uXXXSize 640
#define uYYYSize 480
static CSPACE 		eSrcDataFmt2 = RGB24;

///////////////////////////////////////////////////////////
//LCD Test Top Item 
void LCD_Test(void);
static void TestRgbIf(void);
static void TestCpuIf(void);

//Set Items
static void SetRgbBpp(void);
static void SetArgbBpp(void);
static void SetAllWinBpp(void);
static void SetPaletteBppAndWin(void);
static void SetWinOfFgAndBg(void);
static void SelectReverseWins(void);

//LCD Test Functions
//RGB I/F
static void DisBasicRgb(void);
static void VirtualScreen(void);
static void ColorMap(void);
static void ColorKey(void);
static void BlendingPerPlane(void);
static void BlendingPerPixel(void);
static void DisIntRgb(void);
static void DoubleBuffering(void);
static void AutoDoubleBuffering(void);
static void Palette(void);
static void NonPaletteBpp(void);
static void Swap(void);
static void WvgaDis(void);
static void VDMABurstLength(void);
static void ParallelBGR(void);
static void SerialRGB(void);
static void Interlace(void);
static void EintAliasing(void);
static void GPIOMuxing(void);	
//static void YUV422Out(void);
static void VgaDis(void);
 
//CPU I/F 
static void DisBasicCpu(void);
static void PartialDis(void);
static void AutoNormalManualCmd(void);
static void NormalNAutoCmd(void);
static void ManualRead(void);	
static void DisIntCpuIf(void);
static void Dithering(void);
static void ByPass(void);
static void OutputDataFormat(void);
//static void FrmVsync(void);	

//Camera Local I/F
static void CameraLocalPath(void);

//Post Local I/F
static void PostLocalPath(void);
static void PostSetCscTypeDmaToFifo(void);

///////////////////////////////////////////////////////////
// ISRs for LCD Controller 
void __irq Isr_LCD_I80Int(void);
void __irq Isr_LCD_I80Int1(void);
void __irq Isr_LCD_FrameInt(void);
void __irq Isr_LCD_FifoInt(void);
void __irq Isr_LCD_FifoEmptyInt(void);

//ISR for CAMERA I/F
void __irq Isr_MSDMA_PreviewDone(void);
void __irq Isr_MSDMA_CodecDone(void);

//ISR for External Int.
void __irq Isr_ExtInt9(void);




void __irq Isr_EINT9_Setting(void)
{
	uEINT9IRQ = 1;          
	GPIO_EINT0ClrPend(9);
	INTC_ClearVectAddr();
	POST_InitCh(POST_A, &oPost);
	POST_StopProcessingOfFreeRun(&oPost);	
	while (!POST_IsProcessingDone(&oPost));		
	LCD_SetWinOnOff(0, WIN0); // window 0 is off			
}

void __irq Isr_EINT9_Setting_2(void)
{
	uEINT9IRQ = 1;       
	uPOSTDoneIssue = 1;
	GPIO_EINT0ClrPend(9);
	INTC_ClearVectAddr();
}

void __irq Isr_EINT10_Setting_2(void)
{
//	uPOSTDoneIssue = 1;
//	uPOSTSTART = 1; 
	POST_InitCh(POST_A, &oPost);
	POST_StartProcessing(&oPost);		
//	uPOSTSTART = 0;	
	LCD_SetWinOnOff(1, WIN0);
	GPIO_EINT0ClrPend(10);
	INTC_ClearVectAddr();
}




 void __irq Isr_LCD_FrameCount3(void) // add. jungil test 080407. 
 {
	volatile u32 temp;
	
	INTC_Disable(NUM_LCD1);
	LCD_DisableInt();

	LCD_ClearInt(LCD_FRAME_INT);
	INTC_ClearVectAddr();

	if (WinStopIssue == 1) LCD_SetWinOnOff(0, WIN0);
		
	INTC_Enable(NUM_LCD1);
	LCD_EnableInt();
}






 void __irq Isr_LCD_FrameCount2(void) // add. jungil test 080407. 
 {
	volatile u32 temp;
	
	INTC_Disable(NUM_LCD1);
	LCD_DisableInt();

	//uFrameCount++;

	LCD_ClearInt(LCD_FRAME_INT);
	INTC_ClearVectAddr();

	// Test jungil add. 080407
	if ( uPOSTEND == 1 )
	{
		POST_InitCh(POST_A, &oPost);
		POST_StopProcessingOfFreeRun(&oPost);	
		while (!POST_IsProcessingDone(&oPost));	
		uPOSTEND == 0 ;
	}

	if ( uPOSTSTART == 1 )
	{
		POST_InitCh(POST_A, &oPost);
		POST_StartProcessing(&oPost);		
		uPOSTSTART = 0;
	}
	
	INTC_Enable(NUM_LCD1);
	LCD_EnableInt();
}


 void __irq Isr_POST_Done(void) // add. jungil test 080407. 
 {
	POST_ClearPending(&oPost);
	INTC_ClearVectAddr();
	if ( uPOSTDoneIssue == 1 ) 
	{
		*(unsigned int*)0x77000000 = *(unsigned int*)0x77000000 & ~(0x1<<14); // autoload disable.
		WinStopIssue = 1; 
 	}
}




static void SetWinOfFgAndBg(void)
{
	u32 uBgWinSel, uFgWinSel;
	u8 bIsBgExit = false;
	u8 bIsFgExit = false;

	while (true)
	{
		while(true)
		{
			UART_Printf("[0] Exit\n");
			UART_Printf("[1] WINDOW 0\n");
			UART_Printf("[2] WINDOW 1\n");	
			UART_Printf("[3] WINDOW 2\n");
			UART_Printf("[4] WINDOW 3\n");	
			UART_Printf("[5] WINDOW 4\n");

			UART_Printf("\n");
			UART_Printf(">>Select Window Number Used For BACKGROUND: ");
			uBgWinSel = UART_GetIntNum();

			if (uBgWinSel == 0)
				return;
			else if (uBgWinSel >= 1 && uBgWinSel <= 5)
			{
				eBgWin =
					(uBgWinSel == 1) ? WIN0 :
					(uBgWinSel == 2) ? WIN1 :
					(uBgWinSel == 3) ? WIN2 :
					(uBgWinSel == 4) ? WIN3 : WIN4;

				bIsBgExit = true;
				break;
			}
			else
				UART_Printf("Invalid Input! Retry It!!\n");
		}

		UART_Printf("\n");
		
		while(true)
		{
			UART_Printf("[0] Exit\n");
			UART_Printf("[1] WINDOW 1\n");	
			UART_Printf("[2] WINDOW 2\n");
			UART_Printf("[3] WINDOW 3\n");	
			UART_Printf("[4] WINDOW 4\n");

			UART_Printf("\n");
			UART_Printf(">>Select Window Number Used For FOREGROUND: ");
			uFgWinSel = UART_GetIntNum();

			if (uFgWinSel == 0)
				return;
			else if (uFgWinSel >= 1 && uFgWinSel <= 4)
			{
				eFgWin =
					(uFgWinSel == 1) ? WIN1 :
					(uFgWinSel == 2) ? WIN2 :
					(uFgWinSel == 3) ? WIN3 : WIN4;

				bIsFgExit = true;
				break;
			}
			else
				UART_Printf("Invalid Input! Retry It!!\n");
		}

		if ( (bIsBgExit == true || bIsFgExit == true) && ((int)eBgWin < (int)eFgWin) )
			break;

		if ((int)eBgWin >= (int)eFgWin)
		{
			UART_Printf("Win. Num. Correspond To Foreground Must Be Larger Than That Of Background!\n");
			UART_Printf("\n");
		}
	}
}

static void SetRgbBpp(void)
{
	u32 uSelBpp;

	while (true)
	{
		UART_Printf("[0] Exit\n");
		UART_Printf("[1] RGB16 bpp (R:5 G:6 B:5)\n");
		UART_Printf("[2] RGB24 bpp (R:8 G:8 B:8)\n");

		UART_Printf("\n");
		UART_Printf("> Select Background's Bpp: ");
		uSelBpp = UART_GetIntNum();

		if (uSelBpp == 0)
			return;
		else if (uSelBpp >= 1 && uSelBpp <= 2)
		{
			eBgBpp =  (uSelBpp == 1) ? RGB16 : RGB24;

			break;
		}
		else
			UART_Printf("Invalid Input! Retry It!!\n");
	}

	while (true)
	{
		UART_Printf("[0] Exit\n");
		UART_Printf("[1] RGB16 bpp (R:5 G:6 B:5)\n");
		UART_Printf("[2] RGB24 bpp (R:8 G:8 B:8)\n");
		UART_Printf("[3] 1 bpp (Palette)\n");
		UART_Printf("[4] 2 bpp (palette)\n");	
		UART_Printf("[5] 4 bpp (Palette)\n");
		UART_Printf("[6] 8 bpp (Palette)\n");				

		UART_Printf("\n");
		UART_Printf("> Select Foreground's Bpp: ");
		uSelBpp = UART_GetIntNum();

		if (uSelBpp == 0)
			return;
		else if (uSelBpp >= 1 && uSelBpp <= 2)
		{
			eFgBpp = (uSelBpp == 1) ? RGB16 : RGB24;

			break;
		}
		else
			UART_Printf("Invalid Input! Retry It!!\n");
	}
}

static void SetArgbBpp(void)
{
	u32 uBgBpp, uSelFgArgb;

	while (true) // Set Background's Bpp
	{
		UART_Printf("[0] Exit\n");
		UART_Printf("[1] RGB16 Bpp (R:5 G:6 B:5)\n");
		UART_Printf("[2] RGB24 Bpp (R:8 G:8 B:8)\n");

		UART_Printf("\n");
		UART_Printf("> Select Background's Bpp: ");
		uBgBpp = UART_GetIntNum();

		if (uBgBpp == 0)
			return;
		else if (uBgBpp >= 1 && uBgBpp <=2)
		{
			eBgBpp = (uBgBpp == 1) ? RGB16 : RGB24;
			break;
		}
		else
			UART_Printf("Invalid Input! Retry It!!\n");
	}

	UART_Printf("\n");
	
	while(true) // Set Foreground's Bpp
	{
		UART_Printf("[0] Exit\n");
		UART_Printf("[1] ARGB 16Bpp (A:1 R:5 G:5 B:5)\n");
		UART_Printf("[2] ARGB 24Bpp (A:1 R:8 G:8 B:7)\n");
		UART_Printf("(\'A\' means ALPHA VALUE when PER-PIXEL BLENDING is tested)\n");					
		UART_Printf("This selection is applied for only PER-PIXEL BLENDING test\n");
		UART_Printf("\n");

		UART_Printf("> Select Foreground's Bpp: ");

		uSelFgArgb = UART_GetIntNum();

		if (uSelFgArgb == 0)
			return;
		else if (uSelFgArgb >= 1 && uSelFgArgb <= 2)
		{
			eFgArgbBpp = (uSelFgArgb == 1) ? ARGB16 : ARGB24;
			break;
		}
		else
			UART_Printf("Invalid Input! Retry It!!\n");
	}
	
}


//-----------------------------------------------------------
// Function Name : SetAllWinBpp
//
// Function Desctiption :
//
// Input :	
//
// Output :
//
// Version : 
//------------------------------------------------------------
static void SetAllWinBpp(void)
{
	u32 uSelWin;
	
	while (true)
	{
		UART_Printf("[0] Exit\n");
		UART_Printf("[1] WIN0: RGB24, WIN1: RGB16, WIN2: PAL4, WIN3: PAL2, WIN4: PAL1\n");
		UART_Printf("[2] WIN0: RGB16, WIN1: RGB24, WIN2: PAL1, WIN3: PAL4, WIN4: PAL2\n");
		UART_Printf("[3] WIN0: RGB24, WIN1: RGB24  WIN2: PAL4, WIN3: PAL4, WIN4: PAL1\n");
		UART_Printf("[4] WIN0: RGB16, WIN1: RGB16, WIN2: PAL2, WIN3: PAL1, WIN4: PAL2\n");

		UART_Printf("\n");
		UART_Printf("Select A Combination Of Windows: ");

		uSelWin = UART_GetIntNum();
		UART_Printf("(Data Format Of Palette: RGB16)\n");
		
		if (uSelWin == 0)
			return;
		else if (uSelWin >= 1 && uSelWin <= 4)
		{
			eWinBpp[0] = (uSelWin == 1 || uSelWin == 3) ? RGB24 : RGB16;
			eWinBpp[1] = (uSelWin == 2 || uSelWin == 3) ? RGB24 : RGB16;
			eWinBpp[2] =
				(uSelWin == 1 || uSelWin == 3) ? PAL4 :
				(uSelWin == 4) ? PAL2 : PAL1;
			eWinBpp[3] =
				(uSelWin == 2 || uSelWin == 3) ? PAL4 :
				(uSelWin == 1) ? PAL2 : PAL1;
			eWinBpp[4] = (uSelWin == 2 || uSelWin == 4) ? PAL2 : PAL1;

			ePalDataBpp = RGB16;
			break;
		}
		else
			UART_Printf("Invalid Input! Retry It!!\n");
	}
}

static void SetPaletteBppAndWin(void)
{
	u32 uSelPalDataBpp, uSelPalBpp;
	u32 uSelWin;
	
	while (true)
	{
		UART_Printf("[0] Exit\n");
		UART_Printf("[1] WIN 0\n");
		UART_Printf("[2] WIN 1\n");
		UART_Printf("[3] WIN 2\n");
		UART_Printf("[4] WIN 3\n");
		UART_Printf("[5] WIN 4\n");

		UART_Printf(">>Select Window Number: ");
		
		uSelWin = UART_GetIntNum();

		if (uSelWin == 0)
			return;
		else if (uSelWin >= 1 && uSelWin <= 5)
		{
			eBgWin =
				(uSelWin == 1) ? WIN0 :
				(uSelWin == 2) ? WIN1 :
				(uSelWin == 3) ? WIN2 :
				(uSelWin == 4) ? WIN3 :	WIN4;
			break;
		}
		else 
			UART_Printf("Invalid Input! Retry It!!\n");
		UART_Printf("\n");
	}

	UART_Printf("\n");
	
	while (true)
	{
		UART_Printf("[0] Exit\n");
		UART_Printf("[1] Paltte 1Bpp\n");
		UART_Printf("[2] Paltte 2Bpp\n");

		if (eBgWin != WIN4)
		{
			UART_Printf("[3] Palette 4Bpp\n");

			if (eBgWin == WIN0 || eBgWin == WIN1)
			{
				UART_Printf("[4] Palette 8Bpp\n");
			}
		}
		UART_Printf(">>Select Palette Bpp: ");

		uSelPalBpp = UART_GetIntNum();

		if (uSelPalBpp == 0)
			return;
		else if ( uSelPalBpp >= 1 && uSelPalBpp <= 4)
		{
			if (eBgWin == WIN0 || eBgWin == WIN1)
			{
				ePalBpp =
					(uSelPalBpp == 1) ? PAL1 :
					(uSelPalBpp == 2) ? PAL2 :
					(uSelPalBpp == 3) ? PAL4 : PAL8;
			}
			else if (eBgWin == WIN2 || eBgWin == WIN3)
			{
				ePalBpp =
					(uSelPalBpp == 1) ? PAL1 :
					(uSelPalBpp == 2) ? PAL2 : PAL4;

				ePalDataBpp = RGB16;
				UART_Printf("Data Format Of Palette is set to RGB 16Bpp\n");
			}
			else 
			{
				ePalBpp = (uSelPalBpp == 1) ? PAL1 : PAL2;
				ePalDataBpp = RGB16;
				UART_Printf("Data Format Of Palette is set to RGB 16Bpp\n");				
			}
			
			break;
		}
		else
			UART_Printf("Invalid Input! Retry It!!\n");

		UART_Printf("\n");
	}

	UART_Printf("\n");
	
	if (eBgWin == WIN0 || eBgWin == WIN1)
	{
		while (true)
		{
			UART_Printf("[0] Exit\n");
			UART_Printf("[1] RGB 16Bpp (R:5 G:6 B:5)\n");
			UART_Printf("[2] RGB 24Bpp (R:8 G:8 B:8)\n");

			UART_Printf(">>Select Data Bpp Of Palette: ");

			uSelPalDataBpp = UART_GetIntNum();

			if (uSelPalDataBpp == 0)
				return;
			else if (uSelPalDataBpp >= 1 && uSelPalDataBpp <= 2)
			{
				ePalDataBpp = (uSelPalDataBpp == 1) ? RGB16 : RGB24;
				break;
			}
			else
				UART_Printf("Invalid Input! Retry It!!\n");
		}	
	}	
}

static void SelectReverseWins(void)
{
	u32 uSelWin;
	u32 uSelBpp;
	const char* pcGrndStr;

	while (true)
	{
		UART_Printf("[0] Exit\n");
		UART_Printf("[1] Background: WIN0, Foreground1: WIN2, Foreground2: WIN1\n");
		UART_Printf("[2] Background: WIN1, Foreground1: WIN0, Foreground2: WIN2\n");
		UART_Printf("[3] Background: WIN1, Foreground1: WIN2, Foreground2: WIN0\n");
		UART_Printf("[4] Background: WIN2, Foreground1: WIN0, Foreground2: WIN1\n");
		UART_Printf("[5] Background: WIN2, Foreground1: WIN1, Foreground2: WIN0\n");		

		UART_Printf(">>Select A Combination Of Windows: ");

		uSelWin = UART_GetIntNum();

		if (uSelWin == 0)
			return;
		else if (uSelWin >= 1 || uSelWin <= 5)
		{
			eBgWin_Rev = 
				(uSelWin == 1) ? WIN0 :
				(uSelWin == 2 || uSelWin == 3) ? WIN1 : WIN2;

			eFgWinArray_Rev[0] =
				(uSelWin == 1 || uSelWin == 3) ? WIN2 :
				(uSelWin == 2 || uSelWin == 4) ? WIN0 : WIN1;

			eFgWinArray_Rev[1] =
				(uSelWin == 1 || uSelWin == 4) ? WIN1 :
				(uSelWin == 2) ? WIN2 : WIN0;

			break;
		}
		else
			UART_Printf("Invalid Input! Retry It!!\n");
	}

	UART_Printf("\n");

	while (true)
	{
		int i=0;
		while (i<3)
		{
			pcGrndStr = 
				(i == 0) ? "Background" :
				(i == 1) ? "Foreground1" : "Foreground2";
				
			UART_Printf("[0] Exit\n");
			UART_Printf("[1] RGB 16Bpp\n");
			UART_Printf("[2] RGB 24Bpp\n");
			UART_Printf(">>Select Bpp Of %s: ", pcGrndStr);

			uSelBpp = UART_GetIntNum();

			if (uSelBpp == 0)
				return;
			else if (uSelBpp >= 1 && uSelBpp<= 2)
			{
				if (i==0)
					eBgBpp_Rev = (uSelBpp == 1) ? RGB16 : RGB24;
				else
					eFgBppArray_Rev[i-1] = (uSelBpp == 1) ? RGB16 : RGB24;
				i++;
			}
			else
				UART_Printf("Invalid Input! Retry It!!\n");

			UART_Printf("\n");
		}
		
		if (i==3)
			break;
	}
}

//////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////
// LCD Test Functions
//////////////////////////////////////////////////////////////////////////////////////



static void DisBasicITU656(void)
{
	u32 uFgAddr;
	u32 uHSz, uVSz;
	u32 uTemp;

	u32 i,j,k; // add. jungil
	
	bIsRev = false;
	bIsAllWin = false;
	
	UART_Printf("Enter Any Key To Test Simple Display\n");
	UART_Getc();

	LCD_InitDISPC1(800,480, eBgBpp, uLcdFbAddr, eBgWin, false);
	LCD_SetWinOnOff(1, eBgWin);
	LCD_SetITUInterface_656();
	UART_Printf("Window %d (Background) Is Displayed\n", (int)eBgWin);

	#if 0 // add jungil 
	LCD_GetFrmSz(&uHSz, &uVSz, eBgWin);
	GLIB_InitInstance(uLcdFbAddr, uHSz, uVSz, eBgBpp);
	k=0;
	for(i=0; i<480; i++)
		for(j=0; j<800; j++)
//			GLIB_PutPixel2(j, i, pSmile_3[k++]);
			GLIB_PutPixel2(j, i, pKangjjang01[k++]);		
	#else
	LCD_GetFrmSz(&uHSz, &uVSz, eBgWin);
	GLIB_InitInstance(uLcdFbAddr, uHSz, uVSz, eBgBpp);
	GLIB_DrawPattern(uHSz, uVSz);
	GLIB_PutLine(0, 0, 0, uVSz-1, C_BLUE);
	#endif
	LCD_Start();

	UART_Printf("Do You Want To See Choi Gang Hi ? Yes:1, No:other\n");
	uTemp = UART_GetIntNum();
	
	LCD_SetAllWinOnOff(0);

	#if 0 
	if (uTemp == 1 )
	{
		LCD_InitDISPC(eBgBpp, uLcdFbAddr, eBgWin, false);
		LCD_SetWinOnOff(1, eBgWin);
		LCD_GetFrmSz(&uHSz, &uVSz, eBgWin);
		LCD_Start();
		while(1)
		{
			LCD_InitWin(eBgBpp, uHSz, uVSz, 0, 0, uHSz, uVSz, 0, 0, uLcdFbAddr, eBgWin, false);
			UART_Printf("Window %d (Background) Is Displayed\n", (int)eBgWin);
			GLIB_InitInstance(uLcdFbAddr, uHSz, uVSz, eBgBpp);
			k=0;
			for(i=0; i<480; i++)
				for(j=0; j<800; j++)
					GLIB_PutPixel2(j, i, pKangjjang03[k++]);
			Delay(100000);
			LCD_SetWinOnOff(0, eBgWin);

			LCD_GetFbEndAddr(&uFgAddr, eBgWin);

			LCD_InitWin(eFgBpp, uHSz, uVSz, 0, 0, uHSz, uVSz, 0, 0, uFgAddr, eFgWin, false);
			GLIB_InitInstance(uFgAddr, uHSz, uVSz, eFgBpp);
			k=0;
			for(i=0; i<480; i++)
				for(j=0; j<800; j++)
					GLIB_PutPixel2(j, i, pKangjjang01[k++]);
			UART_Printf("Window %d (Foreground) Is Displayed\n", (int)eFgWin);
			LCD_SetWinOnOff(1, eFgWin);
			Delay(100000);
			LCD_SetWinOnOff(0, eFgWin);
			if (UART_GetKey()) break;		
		}
		UART_Printf("\nIf you want to end this test, press any key\n");
		UART_Getc();
	}
	#endif
}


static void DisBasicITU601(void)
{
	u32 uFgAddr;
	u32 uHSz, uVSz;
	u32 uTemp;

	u32 i,j,k; // add. jungil
	
	bIsRev = false;
	bIsAllWin = false;
	
	UART_Printf("Enter Any Key To Test Simple Display\n");
	UART_Getc();



	LCD_InitDISPC1(800,480, eBgBpp, uLcdFbAddr, eBgWin, false);
	LCD_SetWinOnOff(1, eBgWin);
	LCD_SetITUInterface();
	UART_Printf("Window %d (Background) Is Displayed\n", (int)eBgWin);

	#if 0 // add jungil 
	LCD_GetFrmSz(&uHSz, &uVSz, eBgWin);
	GLIB_InitInstance(uLcdFbAddr, uHSz, uVSz, eBgBpp);
	k=0;
	for(i=0; i<480; i++)
		for(j=0; j<800; j++)
//			GLIB_PutPixel2(j, i, pSmile_3[k++]);
			GLIB_PutPixel2(j, i, pKangjjang01[k++]);		
	#else
	LCD_GetFrmSz(&uHSz, &uVSz, eBgWin);
	GLIB_InitInstance(uLcdFbAddr, uHSz, uVSz, eBgBpp);
	GLIB_DrawPattern(uHSz, uVSz);
	GLIB_PutLine(0, 0, 0, uVSz-1, C_BLUE);
	#endif
	LCD_Start();

	UART_Printf("Do You Want To See Choi Gang Hi ? Yes:1, No:other\n");
	uTemp = UART_GetIntNum();
	
	LCD_SetAllWinOnOff(0);

	#if 0 
	if (uTemp == 1 )
	{
		LCD_InitDISPC(eBgBpp, uLcdFbAddr, eBgWin, false);
		LCD_SetWinOnOff(1, eBgWin);
		LCD_GetFrmSz(&uHSz, &uVSz, eBgWin);
		LCD_Start();
		while(1)
		{
			LCD_InitWin(eBgBpp, uHSz, uVSz, 0, 0, uHSz, uVSz, 0, 0, uLcdFbAddr, eBgWin, false);
			UART_Printf("Window %d (Background) Is Displayed\n", (int)eBgWin);
			GLIB_InitInstance(uLcdFbAddr, uHSz, uVSz, eBgBpp);
			k=0;
			for(i=0; i<480; i++)
				for(j=0; j<800; j++)
					GLIB_PutPixel2(j, i, pKangjjang03[k++]);
			Delay(100000);
			LCD_SetWinOnOff(0, eBgWin);

			LCD_GetFbEndAddr(&uFgAddr, eBgWin);

			LCD_InitWin(eFgBpp, uHSz, uVSz, 0, 0, uHSz, uVSz, 0, 0, uFgAddr, eFgWin, false);
			GLIB_InitInstance(uFgAddr, uHSz, uVSz, eFgBpp);
			k=0;
			for(i=0; i<480; i++)
				for(j=0; j<800; j++)
					GLIB_PutPixel2(j, i, pKangjjang01[k++]);
			UART_Printf("Window %d (Foreground) Is Displayed\n", (int)eFgWin);
			LCD_SetWinOnOff(1, eFgWin);
			Delay(100000);
			LCD_SetWinOnOff(0, eFgWin);
			if (UART_GetKey()) break;		
		}
		UART_Printf("\nIf you want to end this test, press any key\n");
		UART_Getc();
	}
	#endif
}


static void DisBasicRgb(void)
{
	#if 0
	u32 uFgAddr;
	u32 uHSz, uVSz;
	
	u32 i,j,k; // add. jungil
	
	bIsRev = false;
	bIsAllWin = false;
	
	UART_Printf("Enter Any Key To Test Simple Display\n");
	UART_Getc();

		LCD_InitDISPC(eBgBpp, uLcdFbAddr, eBgWin, false);
		LCD_SetWinOnOff(1, eBgWin);
		LCD_GetFrmSz(&uHSz, &uVSz, eBgWin);
		LCD_Start();

	while(1)
	{
		LCD_InitWin(eBgBpp, uHSz, uVSz, 0, 0, uHSz, uVSz, 0, 0, uLcdFbAddr, eBgWin, false);
		UART_Printf("Window %d (Background) Is Displayed\n", (int)eBgWin);
		GLIB_InitInstance(uLcdFbAddr, uHSz, uVSz, eBgBpp);
		k=0;
		for(i=0; i<480; i++)
			for(j=0; j<800; j++)
				GLIB_PutPixel2(j, i, pSmile_2[k++]);
		Delay(10000);
		LCD_SetWinOnOff(0, eBgWin);

		LCD_GetFbEndAddr(&uFgAddr, eBgWin);

		LCD_InitWin(eFgBpp, uHSz, uVSz, 0, 0, uHSz, uVSz, 0, 0, uFgAddr, eFgWin, false);
		GLIB_InitInstance(uFgAddr, uHSz, uVSz, eFgBpp);
		k=0;
		for(i=0; i<480; i++)
			for(j=0; j<800; j++)
				GLIB_PutPixel2(j, i, pSmile_3[k++]);
		UART_Printf("Window %d (Foreground) Is Displayed\n", (int)eFgWin);
		LCD_SetWinOnOff(1, eFgWin);
		Delay(10000);
		LCD_SetWinOnOff(0, eFgWin);
		if (UART_GetKey()) break;		
	}
	UART_Printf("\nIf you want to end this test, press any key\n");
	UART_Getc();

	#else

	u32 uFgAddr;
	u32 uHSz, uVSz;
	u32 uTemp;

	u32 i,j,k; // add. jungil
	u32 uHsize2, uVsize2;
	u32 uBurstWin0, uBurstWin1;
	u32 uVIDCONReg0, uVIDCONReg1;
		
	bIsRev = false;
	bIsAllWin = false;

	eBgBpp = RGB16;
	eFgBpp = RGB16;
	
	UART_Printf("Enter Any Key To Test Simple Display\n");
	UART_Getc();

	LCD_InitDISPC(eBgBpp, uLcdFbAddr, eBgWin, false);
	LCD_SetWinOnOff(1, eBgWin);

	LCD_Start();

	UART_Printf("Window %d (Background) Is Displayed\n", (int)eBgWin);

	#if 0 // add jungil 
	LCD_GetFrmSz(&uHSz, &uVSz, eBgWin);
	GLIB_InitInstance(uLcdFbAddr, uHSz, uVSz, eBgBpp);
	k=0;
	for(i=0; i<480; i++)
		for(j=0; j<800; j++)
			GLIB_PutPixel2(j, i, pSmile_2[k++]);
	#else
	LCD_GetFrmSz(&uHSz, &uVSz, eBgWin);
	GLIB_InitInstance(uLcdFbAddr, uHSz, uVSz, eBgBpp);
	GLIB_DrawPattern(uHSz, uVSz);
	GLIB_PutLine(0, 0, 0, uVSz-1, C_BLUE);
	//LCD_Trigger();	// SW trigger in case of  I80 IF
	#endif
	
	UART_Printf("Enter Any Key To Display Window %d (Foreground)\n", (int)eFgWin);
	UART_Getc();

	LCD_GetFbEndAddr(&uFgAddr, eBgWin);
	
	LCD_InitWin(eFgBpp, uHSz/2, uVSz/2, 0, 0, uHSz/2, uVSz/2, 60, 80, uFgAddr, eFgWin, false);
	LCD_SetWinOnOff(1, eFgWin);

	UART_Printf("Window %d (Foreground) Is Displayed\n", (int)eFgWin);
	LCD_GetFrmSz(&uHSz, &uVSz, eFgWin);
	GLIB_InitInstance(uFgAddr, uHSz, uVSz, eFgBpp);
	GLIB_DrawPattern2(uHSz, uVSz);
	
	GLIB_PutLine(0, 0, 0, uVSz-1, C_BLUE);	
	//LCD_Trigger();	// SW trigger

	UART_Printf("Enter Any Key To Test Case Which Img. Size Is Smaller\n");
	UART_Printf("Than LCD Size In One Window\n");
	UART_Getc();

	// Test case in which img. size is smaller than LCD size in one window.
	LCD_SetAllWinOnOff(0);
	LCD_InitWin(eBgBpp, uHSz, uVSz, 0, 0, uHSz, uVSz, 60, 80, uLcdFbAddr, eBgWin, false);

	LCD_SetWinOnOff(1, eBgWin);
	
	GLIB_InitInstance(uLcdFbAddr, uHSz, uVSz, eBgBpp);
	GLIB_DrawPattern(uHSz, uVSz);
	//LCD_Trigger();

	UART_Printf("\nIf you want to end this test, press any key\n");
	UART_Getc();
	#endif	

	#if 0
	UART_Printf("Do You Want To See Choi Gang Hi ? Yes:1, No:other\n");
	//	UART_Getc();	

	uTemp = UART_GetIntNum();
	if (uTemp == 1 )
	{
		LCD_InitDISPC(eBgBpp, uLcdFbAddr, eBgWin, false);
		LCD_SetWinOnOff(1, eBgWin);
		LCD_GetFrmSz(&uHSz, &uVSz, eBgWin);
		LCD_Start();
		while(1)
		{
			LCD_InitWin(eBgBpp, uHSz, uVSz, 0, 0, uHSz, uVSz, 0, 0, uLcdFbAddr, eBgWin, false);
			UART_Printf("Window %d (Background) Is Displayed\n", (int)eBgWin);
			GLIB_InitInstance(uLcdFbAddr, uHSz, uVSz, eBgBpp);
			k=0;
			for(i=0; i<480; i++)
				for(j=0; j<800; j++)
					GLIB_PutPixel2(j, i, pKangjjang03[k++]);
			Delay(10000);
			LCD_SetWinOnOff(0, eBgWin);

			LCD_GetFbEndAddr(&uFgAddr, eBgWin);

			LCD_InitWin(eFgBpp, uHSz, uVSz, 0, 0, uHSz, uVSz, 0, 0, uFgAddr, eFgWin, false);
			GLIB_InitInstance(uFgAddr, uHSz, uVSz, eFgBpp);
			k=0;
			for(i=0; i<480; i++)
				for(j=0; j<800; j++)
					GLIB_PutPixel2(j, i, pKangjjang01[k++]);
			UART_Printf("Window %d (Foreground) Is Displayed\n", (int)eFgWin);
			LCD_SetWinOnOff(1, eFgWin);
			Delay(10000);
			LCD_SetWinOnOff(0, eFgWin);
			if (UART_GetKey()) break;		
		}
		UART_Printf("\nIf you want to end this test, press any key\n");
		UART_Getc();


	}

	#endif
}

static void VirtualScreen(void) 
{
	u32 uBgWinNum, uFgWinNum;

	u32 uFgAddr;
	u32 uBgHSz, uBgVSz, uFgHSz, uFgVSz, uLcdHSz, uLcdVSz;

	u32 uViewPort_X, uViewPort_Y;
	u32 uOsd_X, uOsd_Y;
	int i;
	
	bIsRev = false;
	bIsAllWin = false;

	uBgWinNum = 
		(eBgWin == WIN0) ? 0 :
		(eBgWin == WIN1) ? 1 :
		(eBgWin == WIN2) ? 2 :
		(eBgWin == WIN3) ? 3 : 4;

	uFgWinNum = 
		(eFgWin == WIN0) ? 0 :
		(eFgWin == WIN1) ? 1 :
		(eFgWin == WIN2) ? 2 :
		(eFgWin == WIN3) ? 3 : 4;

	// The index number of foreground window must be more than that of background window
	if (uBgWinNum >= uFgWinNum)
	{
		UART_Printf("Error! Invalid Background Window Number or Foreground Window Number\n");
		return;
	}

	// Set Background Parameter
	#if (LCD_MODULE_TYPE== LTV350QV_RGB)
		uBgHSz = 320*2;
		uBgVSz = 240*2;
	#elif (LCD_MODULE_TYPE == LTS222QV_CPU)
		uBgHSz = 240*2;
		uBgVSz = 320*2;
	#elif (LCD_MODULE_TYPE== LTP700WV_RGB ||LCD_MODULE_TYPE ==LTE480WV_RGB)
		uBgHSz = 800*2;
		uBgVSz = 480*2;	
	#elif (LCD_MODULE_TYPE== LTV300GV_RGB)
		uBgHSz = 640*2;
		uBgVSz = 480*2;	
	#else
		Assert(0);
	#endif

	UART_Printf("Enter Any Key To Test Virtual Screen\n");
	UART_Getc();
	UART_Printf("\n");

	LCD_InitBase();
	LCD_InitWin(eBgBpp, uBgHSz, uBgVSz, 0, 0, uBgHSz/2, uBgVSz/2, 0, 0, uLcdFbAddr, eBgWin, false);
	
	LCD_SetWinOnOff(1, eBgWin);
	LCD_Start();

	// Draw Background
	GLIB_Init(uLcdFbAddr, uBgHSz, uBgVSz, eBgBpp);
	GLIB_DrawPattern(uBgHSz, uBgVSz);
	GLIB_SetFontColor(C_BLACK, C_WHITE, true);
	GLIB_Printf(100, 160, "BG's WIN %d", (int)eBgWin);
	
	GLIB_SetFontColor(C_RED, C_WHITE, true);	
	GLIB_Printf(340, 160, "BG's WIN %d", (int)eBgWin);	
	
	GLIB_SetFontColor(C_BLACK, C_WHITE, true);	
	GLIB_Printf(340, 480, "BG's WIN %d", (int)eBgWin);		
	GLIB_Printf(100, 480, "BG's WIN %d", (int)eBgWin);	
	LCD_Trigger();

	// Set Foreground Parameter
	
	#if (LCD_MODULE_TYPE== LTV350QV_RGB)
		uFgHSz  = 320/2;
		uFgVSz  = 240/2;
	#elif (LCD_MODULE_TYPE == LTS222QV_CPU)
		uFgHSz  = 240/2;
		uFgVSz  = 320/2;
	#elif (LCD_MODULE_TYPE== LTP700WV_RGB ||LCD_MODULE_TYPE ==LTE480WV_RGB)
		uFgHSz = 800/2;
		uFgVSz = 480/2;		
	#elif (LCD_MODULE_TYPE== LTV300GV_RGB)	
		uFgHSz = 640/2;
		uFgVSz = 480/2;	
	#else
		Assert(0);
	#endif

	uOsd_X = 0;
	uOsd_Y = 0;

	LCD_GetFbEndAddr(&uFgAddr, eBgWin);
	LCD_InitWin(eBgBpp, uFgHSz, uFgVSz, 0, 0, uFgHSz, uFgVSz, uOsd_X, uOsd_Y, uFgAddr, eFgWin, false);

	LCD_SetWinOnOff(1, eFgWin); // Turn on foreground

	// Draw foreground
	GLIB_Init(uFgAddr, uFgHSz, uFgVSz, eBgBpp);
	GLIB_DrawPattern2(uFgHSz, uFgVSz);
	GLIB_SetFontColor(C_BLACK, C_WHITE, true);
	GLIB_Printf(20, 60, "FG's WIN %d", (int)eFgWin);
	LCD_Trigger();

	uViewPort_X = 0;
	uViewPort_Y = 0;
	
	#if (LCD_MODULE_TYPE== LTV350QV_RGB)
		uLcdHSz = 320;
		uLcdVSz = 240;
	#elif (LCD_MODULE_TYPE == LTS222QV_CPU)
		uLcdHSz = 240;
		uLcdVSz = 320;
	#elif (LCD_MODULE_TYPE== LTP700WV_RGB ||LCD_MODULE_TYPE ==LTE480WV_RGB)
		uLcdHSz = 800;
		uLcdVSz = 480;
	#elif (LCD_MODULE_TYPE== LTV300GV_RGB)	
		uLcdHSz = 640;
		uLcdVSz = 480;
	#else
		Assert(0);
	#endif
	
	UART_Printf("ViewPort Moving Test\n");
	UART_Printf("Enter A \'Enter Key\' To Exit Moving Right\n");
	UART_Printf("\n");
	
	//////////////////////////////////////////////////////////////////////
	for (i=0; i<uBgHSz-uLcdHSz; i++)
	{
		UART_Printf("Enter Any Key Except \'Enter Key\' To Move Background RIGHT\n");
		if (UART_Getc() == '\r')
		{
			i = uBgHSz-uLcdHSz-1;
			break;
		}
		
		LCD_SetViewPosOnFrame(i, uViewPort_Y, eBgWin);
		LCD_Trigger();	
				
		UART_Printf("ViewPort's Left Top Coordinate (X, Y) : (%d, %d)\n", i, uViewPort_Y);
		UART_Printf("\n");		
	}
	uViewPort_X = i;
	//////////////////////////////////////////////////////////////////////
	
	UART_Printf("\n");
	UART_Printf("Enter A \'Enter Key\' To Exit Moving Down\n");
	for (i=0; i<uBgVSz - uLcdVSz; i++)
	{
		UART_Printf("Enter Any Key Except \'Enter Key\' To Move Background DOWN\n");
		if (UART_Getc() == '\r')
		{
			i = uBgVSz - uLcdVSz - 1;
			break;
		}
		
		LCD_SetViewPosOnFrame(uViewPort_X, i, eBgWin);
		LCD_Trigger();	
				
		UART_Printf("ViewPort's Left Top Coordinate (X, Y) : (%d, %d)\n", uViewPort_X, i);		
		UART_Printf("\n");		
	}
	uViewPort_Y = i;

	UART_Printf("\n");
	UART_Printf("Enter A \'Enter Key\' To Exit Moving Left\n");
	for (i=uViewPort_X; i>0; i--)
	{
		UART_Printf("Enter Any Key Except \'Enter Key\' To Move Background LEFT\n");	
		if (UART_Getc() == '\r')
			break;

		LCD_SetViewPosOnFrame(uViewPort_X--, uViewPort_Y, eBgWin);
		LCD_Trigger();	
				
		UART_Printf("ViewPort's Left Top Coordinate (X, Y) : (%d, %d)\n", uViewPort_X, uViewPort_Y);		
		UART_Printf("\n");		
	}

	UART_Printf("\n");
	UART_Printf("Enter A \'Enter Key\' To Exit Moving Up\n");
	for (i=uViewPort_Y; i>0; i--)
	{
		UART_Printf("Enter Any Key Except \'Enter Key\' To Move Background UP\n");		
		if (UART_Getc() == '\r')
			break;

		LCD_SetViewPosOnFrame(0, uViewPort_Y--, eBgWin);
		LCD_Trigger();	
			
		UART_Printf("ViewPort's Left Top Coordinate (X, Y) : (0, %d)\n", uViewPort_Y);				
		UART_Printf("\n");		
	}

	UART_Printf("\n");
	UART_Printf("Foreground Moving Test\n");
	UART_Printf("Enter A \'Enter Key\' To Exit Moving Right\n");	
	for(i=0; i<uLcdHSz - uFgHSz; i++)
	{
		UART_Printf("Enter Any Key Except \'Enter Key\' To Move Foreground RIGHT\n");
		if (UART_Getc() == '\r')
		{
			i = uLcdHSz - uFgHSz;
			break;
		}
		
		LCD_SetViewPosOnLcd(i, uOsd_Y, eFgWin);
		LCD_Trigger();	
			
		UART_Printf("Foreground's Left Top Coordinate (X, Y) : (%d, %d)\n", i, uOsd_Y);						
		UART_Printf("\n");				
	}
	uOsd_X = i;
	
	UART_Printf("\n");
	UART_Printf("Enter A \'Enter Key\' To Exit Moving Down\n");
	for(i=0; i<uLcdVSz - uFgVSz; i++)
	{
		UART_Printf("Enter Any Key Except \'Enter Key\' To Move Foreground DOWN\n");
		if (UART_Getc() == '\r')
		{
			i = uLcdVSz - uFgVSz - 1;
			break;
		}
		
		LCD_SetViewPosOnLcd(uOsd_X, i, eFgWin);
		LCD_Trigger();	
			
		UART_Printf("Foreground's Left Top Coordinate (X, Y) : (%d, %d)\n", uOsd_X, i);						
		UART_Printf("\n");				

	}
	uOsd_Y = i;

	UART_Printf("\n");
	UART_Printf("Enter A \'Enter Key\' To Exit Moving Left\n");
	for(i=uOsd_X; i>0; i--)
	{
		UART_Printf("Enter Any Key Except \'Enter Key\' To Move Foreground LEFT\n");
		if (UART_Getc() == '\r')
			break;
		
		LCD_SetViewPosOnLcd(uOsd_X--, uOsd_Y, eFgWin);
		LCD_Trigger();	
				
		UART_Printf("Foreground's Left Top Coordinate (X, Y) : (%d, %d)\n", uOsd_X, uOsd_Y);						
		UART_Printf("\n");				
		
	}

	UART_Printf("\n");
	UART_Printf("Enter A \'Enter Key\' To Exit Moving Up\n");
	for(i=uOsd_Y; i>0; i--)
	{
		UART_Printf("Enter Any Key Except \'Enter Key\' To Move Foreground UP\n");
		if (UART_Getc() == '\r')
			break;
		
		LCD_SetViewPosOnLcd(0, uOsd_Y--, eFgWin);
		LCD_Trigger();	
		
		UART_Printf("Foreground's Left Top Coordinate (X, Y) : (%d, %d)\n", 0, uOsd_Y);						
		UART_Printf("\n");				
	}
}

static void ColorMap(void)
{
	u32 uHSz, uVSz;
	const char* pcColorStr;
	u32 uColorMapVal;
	
	bIsRev = false;
	bIsAllWin = false;
		
	UART_Printf("Enter Any Key To Test Color Map\n");
	UART_Getc();

	LCD_InitDISPC(eBgBpp, uLcdFbAddr, eBgWin, false);	
	LCD_SetWinOnOff(1, eBgWin);

	LCD_Start();
	
	LCD_GetFrmSz(&uHSz, &uVSz, WIN0);
	GLIB_Init(uLcdFbAddr, uHSz, uVSz, eBgBpp);
	GLIB_DrawPattern(uHSz, uVSz);

	GLIB_SetFontColor(C_BLACK, C_WHITE, true);
	GLIB_Printf(40, 80, "WINDOW %d", (int)eBgWin);
	LCD_Trigger();	

	UART_Printf("Enter Any Key To Test Window %d's Color-Map\n", (int)eBgWin);
	UART_Getc();

	uColorMapVal = 
		((int)eBgWin == 0) ? 0x000000ff : // BLUE	
		((int)eBgWin == 1) ? 0x00ff0000 : // RED
		((int)eBgWin == 2) ? 0x0000ff00 : // GREEN
		((int)eBgWin == 3) ? 0x00ffff00 : // YELLOW
		0x00ff00ff; // MAGENTA

	pcColorStr =
		(uColorMapVal == 0x000000ff) ? "BLUE" :		
		(uColorMapVal == 0x00ff0000) ? "RED" :
		(uColorMapVal == 0x0000ff00) ? "GREEN" :
		(uColorMapVal == 0x00ffff00) ? "YELLOW" :
		(uColorMapVal == 0x00ff00ff) ? "MAGENTA" : "NOT DEFINED";

	LCD_SetColorMapOnOff(1, eBgWin);
	LCD_SetColorMapValue(uColorMapVal, eBgWin);
	LCD_Trigger();	
		
	UART_Printf("The Color Of Color-Map's Color : %s\n", pcColorStr);
	UART_Printf("Enter Any Key To Exit This Test\n");
	UART_Getc();
}


static void ColorKey(void)
{

#if 0 
	u32 uFgAddr;
	u32 uHSz, uVSz;

	u32 uKeyAreaColor, uNonKeyAreaColor;
	u32 uColorKey, uMaskKey;
	int k;
	
	bIsRev = false;
	bIsAllWin = false;
	
	if ((int)eBgWin >= (int)eFgWin)
	{
		UART_Printf("Invalid Window Number Correspond to Foreground/ Background!\n");
		return;
	}

	UART_Printf("Enter Any Key To Test Color Key\n");
	UART_Getc();

	LCD_InitDISPC(eBgBpp, uLcdFbAddr, eBgWin, false);	
	LCD_SetWinOnOff(1, eBgWin);

	LCD_Start();
	
	LCD_GetFrmSz(&uHSz,  &uVSz, eBgWin);
	// Draw Pattern by using Glib for Back-ground window 
	GLIB_Init(uLcdFbAddr, uHSz, uVSz, eBgBpp);

	UART_Printf("Window %d (Background) is Displayed\n", (int)eBgWin);
	GLIB_DrawPattern(uHSz, uVSz);
	LCD_Trigger();

	if (eFgBpp == RGB24)
	{
		uColorKey =
			((int)eFgWin == 1) ? 0x0000ff :
			((int)eFgWin == 2) ? 0x00ff00 :
			((int)eFgWin == 3) ? 0xff0000 : 0xff00ff;

		uMaskKey =
			((int)eFgWin == 1) ? 0x00000c :
			((int)eFgWin == 2) ? 0x000c00 :
			((int)eFgWin == 3) ? 0x0c0000 : 0x00000c;

		uKeyAreaColor =
			((int)eFgWin == 1) ? 0x0000fb : // valid color: 0xf3, 0xf7, 0xfb, 0xff
			((int)eFgWin == 2) ? 0x00f300 : // valid color: 0x00f300, 0x00f700, 0x00fb00, 0x00ff00
			((int)eFgWin == 3) ? 0xf70000 : // valid color: 0xf30000, 0xf70000, 0xfb0000, 0xff0000
			0xff00f3; // valid color: 0xff00f3, 0xff00f7, 0xff00fb, 0xff00ff

		uNonKeyAreaColor =
			((int)eFgWin == 1) ? 0x00ff0000 : // RED
			((int)eFgWin == 2) ? 0x000000ff : // BLUE
			((int)eFgWin == 3) ? 0x0000ff00 : // GREEN
			0x0000ffff; // CYAN
	}
	else if (eFgBpp == RGB16)
	{
		// In RGB16(565) Bpp,
		// The Valid bit position of color key value
		// RED : COLVAL[23:19]
		// GREEN : COLVAL[15:10]
		// BLUE : COLVAL[7:3]
		uColorKey =
			((int)eFgWin == 1) ? 0x0000f8 : // BLUE
			((int)eFgWin == 2) ? 0x00fc00 : // GREEN
			((int)eFgWin == 3) ? 0xf80000 : // RED
			0xf800f8; // MAGENTA

		// In RGB16(565) Bpp,
		// Mask Key[18:16] must be 0x7
		// Mask Key[9:8] must be 0x3
		// Mask Key[2:0] must be 0x7 and
		// The valid bit postion of mask key(compare key) value
		// RED : COLVAL[23:19]
		// GREEN : COLVAL[15:10]
		// BLUE : COLVAL[7:3]
		uMaskKey =
			((int)eFgWin == 1) ? 0x070337 :
			((int)eFgWin == 2) ? 0x071b07 :
			((int)eFgWin == 3) ? 0xc70307 : 0x070337;

		uKeyAreaColor =
			((int)eFgWin == 1) ? 0x0019 : // valid color: 0x19, 0x1b, 0x1d, 0x1f
			((int)eFgWin == 2) ? 0x07e0 : // valid color: 0x720, 0x760, 0x790, 0x7e0
			((int)eFgWin == 3) ? 0x7800 : // valid color: 0x3800, 0x7800, 0xc800, 0xf800
			0xf81b; // valid color: 0xf819, 0xf81b, 0xf81d, 0xf81f

		uNonKeyAreaColor =
			((int)eFgWin == 1) ? 0xf800 : // RED
			((int)eFgWin == 2) ? 0x001f : // BLUE
			((int)eFgWin == 3) ? 0x07d0 : // GREEN
			0x07ff; // CYAN
	}

	UART_Printf("Enter Any Key To Display Window %d (Foreground)\n", (int)eFgWin);
	UART_Getc();

	LCD_GetFbEndAddr(&uFgAddr, eBgWin);
	LCD_InitWin(eFgBpp, uHSz, uVSz, 0, 0, uHSz, uVSz, 0, 0, uFgAddr, eFgWin, false);

	LCD_SetWinOnOff(1, eFgWin);
	LCD_GetFrmSz(&uHSz, &uVSz, eFgWin);

	GLIB_Init(uFgAddr, uHSz, uVSz, eFgBpp);
	GLIB_ClearFrame2(uKeyAreaColor);	 // Color key area	
	LCD_Trigger();
	
	GLIB_FillRectangle2(10, 80, 210, 120, uNonKeyAreaColor); // Non-color key area
	LCD_Trigger();
	
	LCD_SetColorKeyOnOff(1, eFgWin);
	
	UART_Printf("Enter Any Key To Test Color Key (FOREGROUND MATCH)\n");
	UART_Getc();
	UART_Printf("\n");
	LCD_SetColorKey(uMaskKey, uColorKey, MATCH_FG_IMAGE, false, eFgWin);
	LCD_Trigger();	


	UART_Printf("Enter Any Key To Test Color Key (BACKGROUDN MATCH)\n");
	UART_Getc();
	UART_Printf("\n");
	LCD_SetColorKey(uMaskKey, uColorKey, MATCH_BG_IMAGE, false, eFgWin);
	LCD_Trigger();	
			
	
	///////////////////////////////////////////////////////////////////
	UART_Printf("Enter Any Key To Test Color Key (FOREGROUND MATCH) + BLENDING\n");
	UART_Getc();
	UART_Printf("\n");
	LCD_SetColorKey(uMaskKey, uColorKey, MATCH_FG_IMAGE, false, eFgWin);
	
	for(k=0; k<0x10; k++)
	{
		LCD_SetAlpha(0x0, 0x0, 0x0, 0x0+k, 0x0+k, 0x0+k, eFgWin);
		LCD_Trigger();	
		UART_Printf("Enter Any Key To Process A Color Key Test: %d\n", k);
		UART_Getc();
	}
	
	LCD_SetAlpha(0x0, 0x0, 0x0, 0x0, 0x0, 0x0, eFgWin);
	LCD_Trigger();	
	
	for(k=0; k<0x10; k++)
	{
		LCD_SetAlpha(0x0+k, 0x0+k, 0x0+k, 0x0, 0x0, 0x0, eFgWin);
		LCD_Trigger();	
		UART_Printf("Enter Any Key To Process A Color Key Test: %d\n", k);
		UART_Getc();
	}
	
	LCD_SetAlpha(0x0, 0x0, 0x0, 0x0, 0x0, 0x0, eFgWin);
	LCD_Trigger();		
	
	for(k=0; k<0x10; k++)
	{
		LCD_SetAlpha(0x0+k, 0x0+k, 0x0+k, 0x0+k, 0x0+k, 0x0+k, eFgWin);
		LCD_Trigger();	
		UART_Printf("Enter Any Key To Process A Color Key Test: %d\n", k);
		UART_Getc();
	}
	LCD_SetAlpha(0x0, 0x0, 0x0, 0x0, 0x0, 0x0, eFgWin);
	LCD_Trigger();		
	/////////////////////////////////////////////////////////////////////


	/////////////////////////////////////////////////////////////////////
	UART_Printf("\n");	
	UART_Printf("Enter Any Key To Test Color Key (BACKGROUND MATCH) + BLENDING\n");
	UART_Getc();
	UART_Printf("\n");

	LCD_SetColorKey(uMaskKey, uColorKey, MATCH_BG_IMAGE, true, eFgWin);
	for(k=0; k<0x10; k++)
	{
		LCD_SetAlpha(0x0, 0x0, 0x0, 0x0+k, 0x0+k, 0x0+k, eFgWin);
		LCD_Trigger();	
		UART_Printf("Enter Any Key To Process A Color Key Test: %d\n", k);
		UART_Getc();
	}
	LCD_SetAlpha(0x0, 0x0, 0x0, 0x0, 0x0, 0x0, eFgWin);
	LCD_Trigger();	

	UART_Printf("Enter Any Key To Per Plane Blending(Using ALPHA 1 VALUE) \n");
	UART_Printf("\n");
	UART_Getc();
	
	for(k=0; k<0x10; k++)
	{
		LCD_SetAlpha(0x0+k, 0x0+k, 0x0+k, 0x0, 0x0, 0x0, eFgWin);			
		LCD_Trigger();			
		UART_Printf("Enter Any Key To Process A Color Key Test: %d\n", k);
		UART_Getc();
	}
	LCD_SetAlpha(0x0, 0x0, 0x0, 0x0, 0x0, 0x0, eFgWin);		
	LCD_Trigger();		
	
	for(k=0; k<0x10; k++)
	{
		LCD_SetAlpha(0x0+k, 0x0+k, 0x0+k, 0x0+k, 0x0+k, 0x0+k, eFgWin);			
		LCD_Trigger();
		UART_Printf("Enter Any Key To Process A Color Key Test: %d\n", k);
		UART_Getc();
	}
	LCD_SetAlpha(0x0, 0x0, 0x0, 0x0, 0x0, 0x0, eFgWin);		
	LCD_Trigger();		
	/////////////////////////////////////////////////////////////////////
#endif
	u32 uFgAddr;
	u32 uHSz, uVSz;

	u32 uKeyAreaColor, uNonKeyAreaColor;
	u32 uColorKey, uMaskKey;
	int k;
	
	bIsRev = false;
	bIsAllWin = false;
	
	if ((int)eBgWin >= (int)eFgWin)
	{
		UART_Printf("Invalid Window Number Correspond to Foreground/ Background!\n");
		return;
	}

	UART_Printf("Enter Any Key To Test Color Key\n");
	UART_Getc();

	LCD_InitDISPC(eBgBpp, uLcdFbAddr, eBgWin, false);	
	LCD_SetWinOnOff(1, eBgWin);

	LCD_Start();
	
	LCD_GetFrmSz(&uHSz,  &uVSz, eBgWin);
	// Draw Pattern by using Glib for Back-ground window 
	GLIB_Init(uLcdFbAddr, uHSz, uVSz, eBgBpp);

	UART_Printf("Window %d (Background) is Displayed\n", (int)eBgWin);
	GLIB_DrawPattern(uHSz, uVSz);
	LCD_Trigger();

	if (eFgBpp == RGB24)
	{
		uColorKey =
			((int)eFgWin == 1) ? 0x0000ff :
			((int)eFgWin == 2) ? 0x00ff00 :
			((int)eFgWin == 3) ? 0xff0000 : 0xff00ff;

		uMaskKey =
			((int)eFgWin == 1) ? 0x00000c :
			((int)eFgWin == 2) ? 0x000c00 :
			((int)eFgWin == 3) ? 0x0c0000 : 0x00000c;

		uKeyAreaColor =
			((int)eFgWin == 1) ? 0x0000fb : // valid color: 0xf3, 0xf7, 0xfb, 0xff
			((int)eFgWin == 2) ? 0x00f300 : // valid color: 0x00f300, 0x00f700, 0x00fb00, 0x00ff00
			((int)eFgWin == 3) ? 0xf70000 : // valid color: 0xf30000, 0xf70000, 0xfb0000, 0xff0000
			0xff00f3; // valid color: 0xff00f3, 0xff00f7, 0xff00fb, 0xff00ff

		uNonKeyAreaColor =
			((int)eFgWin == 1) ? 0x00ff0000 : // RED
			((int)eFgWin == 2) ? 0x000000ff : // BLUE
			((int)eFgWin == 3) ? 0x0000ff00 : // GREEN
			0x0000ffff; // CYAN
	}
	else if (eFgBpp == RGB16)
	{
		// In RGB16(565) Bpp,
		// The Valid bit position of color key value
		// RED : COLVAL[23:19]
		// GREEN : COLVAL[15:10]
		// BLUE : COLVAL[7:3]
		uColorKey =
			((int)eFgWin == 1) ? 0x0000f8 : // BLUE
			((int)eFgWin == 2) ? 0x00fc00 : // GREEN
			((int)eFgWin == 3) ? 0xf80000 : // RED
			0xf800f8; // MAGENTA

		// In RGB16(565) Bpp,
		// Mask Key[18:16] must be 0x7
		// Mask Key[9:8] must be 0x3
		// Mask Key[2:0] must be 0x7 and
		// The valid bit postion of mask key(compare key) value
		// RED : COLVAL[23:19]
		// GREEN : COLVAL[15:10]
		// BLUE : COLVAL[7:3]
		uMaskKey =
			((int)eFgWin == 1) ? 0x070337 :
			((int)eFgWin == 2) ? 0x071b07 :
			((int)eFgWin == 3) ? 0xc70307 : 0x070337;

		uKeyAreaColor =
			((int)eFgWin == 1) ? 0x0019 : // valid color: 0x19, 0x1b, 0x1d, 0x1f
			((int)eFgWin == 2) ? 0x07e0 : // valid color: 0x720, 0x760, 0x790, 0x7e0
			((int)eFgWin == 3) ? 0x7800 : // valid color: 0x3800, 0x7800, 0xc800, 0xf800
			0xf81b; // valid color: 0xf819, 0xf81b, 0xf81d, 0xf81f

		uNonKeyAreaColor =
			((int)eFgWin == 1) ? 0xf800 : // RED
			((int)eFgWin == 2) ? 0x001f : // BLUE
			((int)eFgWin == 3) ? 0x07d0 : // GREEN
			0x07ff; // CYAN
	}

	UART_Printf("Enter Any Key To Display Window %d (Foreground)\n", (int)eFgWin);
	UART_Getc();

	LCD_GetFbEndAddr(&uFgAddr, eBgWin);
	LCD_InitWin(eFgBpp, uHSz, uVSz, 0, 0, uHSz, uVSz, 0, 0, uFgAddr, eFgWin, false);
//	LCD_InitWin(eFgBpp, uHSz/2, uVSz/2, 0, 0, uHSz/2, uVSz/2, 0, 0, uFgAddr, eFgWin, false);	

	LCD_SetWinOnOff(1, eFgWin);
	LCD_GetFrmSz(&uHSz, &uVSz, eFgWin);

	GLIB_Init(uFgAddr, uHSz, uVSz, eFgBpp);
	GLIB_ClearFrame2(uKeyAreaColor);	 // Color key area	
	LCD_Trigger();
	
	GLIB_FillRectangle2(10, 80, 210, 120, uNonKeyAreaColor); // Non-color key area
	LCD_Trigger();
	
	LCD_SetColorKeyOnOff(1, eFgWin);
	
	UART_Printf("Enter Any Key To Test Color Key (FOREGROUND MATCH)\n");
	UART_Getc();
	UART_Printf("\n");
	LCD_SetColorKey(uMaskKey, uColorKey, MATCH_FG_IMAGE, false, eFgWin);
	LCD_Trigger();	


	UART_Printf("Enter Any Key To Test Color Key (BACKGROUDN MATCH)\n");
	UART_Getc();
	UART_Printf("\n");
	LCD_SetColorKey(uMaskKey, uColorKey, MATCH_BG_IMAGE, false, eFgWin);
	LCD_Trigger();	
			
	
	///////////////////////////////////////////////////////////////////
	UART_Printf("Enter Any Key To Test Color Key (FOREGROUND MATCH) + BLENDING\n");
	UART_Getc();
	UART_Printf("\n");
	LCD_SetColorKey(uMaskKey, uColorKey, MATCH_FG_IMAGE, true, eFgWin);
	
	for(k=0; k<0x10; k++)
	{
		LCD_SetAlpha(0x0, 0x0, 0x0, 0x0+k, 0x0+k, 0x0+k, eFgWin);
		LCD_Trigger();	
		UART_Printf("Enter Any Key To Process A Color Key Test: %d\n", k);
		UART_Getc();
	}
	
	LCD_SetAlpha(0x0, 0x0, 0x0, 0x0, 0x0, 0x0, eFgWin);
	LCD_Trigger();	
	
	for(k=0; k<0x10; k++)
	{
		LCD_SetAlpha(0x0+k, 0x0+k, 0x0+k, 0x0, 0x0, 0x0, eFgWin);
		LCD_Trigger();	
		UART_Printf("Enter Any Key To Process A Color Key Test: %d\n", k);
		UART_Getc();
	}
	
	LCD_SetAlpha(0x0, 0x0, 0x0, 0x0, 0x0, 0x0, eFgWin);
	LCD_Trigger();		
	
	for(k=0; k<0x10; k++)
	{
		LCD_SetAlpha(0x0+k, 0x0+k, 0x0+k, 0x0+k, 0x0+k, 0x0+k, eFgWin);
		LCD_Trigger();	
		UART_Printf("Enter Any Key To Process A Color Key Test: %d\n", k);
		UART_Getc();
	}
	LCD_SetAlpha(0x0, 0x0, 0x0, 0x0, 0x0, 0x0, eFgWin);
	LCD_Trigger();		
	/////////////////////////////////////////////////////////////////////


	/////////////////////////////////////////////////////////////////////
	UART_Printf("\n");	
	UART_Printf("Enter Any Key To Test Color Key (BACKGROUND MATCH) + BLENDING\n");
	UART_Getc();
	UART_Printf("\n");

	LCD_SetColorKey(uMaskKey, uColorKey, MATCH_BG_IMAGE, true, eFgWin);
	for(k=0; k<0x10; k++)
	{
		LCD_SetAlpha(0x0, 0x0, 0x0, 0x0+k, 0x0+k, 0x0+k, eFgWin);
		LCD_Trigger();	
		UART_Printf("Enter Any Key To Process A Color Key Test: %d\n", k);
		UART_Getc();
	}
	LCD_SetAlpha(0x0, 0x0, 0x0, 0x0, 0x0, 0x0, eFgWin);
	LCD_Trigger();	

	UART_Printf("Enter Any Key To Per Plane Blending(Using ALPHA 1 VALUE) \n");
	UART_Printf("\n");
	UART_Getc();
	
	for(k=0; k<0x10; k++)
	{
		LCD_SetAlpha(0x0+k, 0x0+k, 0x0+k, 0x0, 0x0, 0x0, eFgWin);			
		LCD_Trigger();			
		UART_Printf("Enter Any Key To Process A Color Key Test: %d\n", k);
		UART_Getc();
	}
	LCD_SetAlpha(0x0, 0x0, 0x0, 0x0, 0x0, 0x0, eFgWin);		
	LCD_Trigger();		
	
	for(k=0; k<0x10; k++)
	{
		LCD_SetAlpha(0x0+k, 0x0+k, 0x0+k, 0x0+k, 0x0+k, 0x0+k, eFgWin);			
		LCD_Trigger();
		UART_Printf("Enter Any Key To Process A Color Key Test: %d\n", k);
		UART_Getc();
	}
	LCD_SetAlpha(0x0, 0x0, 0x0, 0x0, 0x0, 0x0, eFgWin);		
	LCD_Trigger();		
	/////////////////////////////////////////////////////////////////////

}

static void BlendingPerPlane(void)
{
	u32 uFgAddr;
	u32 uHSz, uVSz;
	int j;
	
	int l=0;
	int m=0;
	int n=0;
	
	bIsRev = false;
	bIsAllWin = false;
	
	UART_Printf("Enter Any Key To Test Per Plane Blending\n");
	UART_Getc();

	LCD_InitDISPC(eBgBpp, uLcdFbAddr, eBgWin, false);	
	LCD_SetWinOnOff(1, eBgWin);	
	LCD_Start();
	
	LCD_GetFrmSz(&uHSz,&uVSz, eBgWin);

	// Draw Pattern by using Glib for window 0
	UART_Printf("Window %d(Background) is Displayed\n", (int)eBgWin);	
	UART_Printf("\n");
	GLIB_Init(uLcdFbAddr, uHSz, uVSz, eBgBpp);
	GLIB_DrawPattern(uHSz, uVSz);
	LCD_Trigger();

	UART_Printf("Enter Any Key To Display Window %d\n", (int)eFgWin);
	UART_Getc();

	LCD_GetFbEndAddr(&uFgAddr, eBgWin);
	LCD_InitWin(eFgBpp, uHSz, uVSz, 0, 0, uHSz, uVSz, 0, 0, uFgAddr, eFgWin, false);

	LCD_SetWinOnOff(1, eFgWin);		
	LCD_GetFrmSz(&uHSz, &uVSz, eFgWin);

	// Draw Rectangles by using Glib for window 1
	GLIB_Init(uFgAddr, uHSz, uVSz, eFgBpp);
	GLIB_ClearFrame(C_MAGENTA);
	LCD_Trigger();	
	
	UART_Printf("Window %d(ForeGround) is Displayed\n", (int)eFgWin);		
	UART_Printf("\n");

	UART_Printf("Enter Any Key To Test Per Plane Blending(Using ALPHA 0 VALUE)\n");
	UART_Printf("\n");
	UART_Getc();

	LCD_SetBlendingType(PER_PLANE, ALPHA0_PLANE, eFgWin);
	LCD_SetAlpha(0x0, 0x0, 0x0, 0x0, 0x0, 0x0, eFgWin);

	for (j=0; j<0x10; j++)
	{
		LCD_SetAlpha(0x0+j, 0x0+j, 0x0+j, 0x0, 0x0, 0x0, eFgWin);
		LCD_Trigger();	
		UART_Printf("Enter Any Key To Test Per Plane Blending (Using ALPHA 0 VALUE): %d\n", j);
		UART_Getc();
	}
	
	UART_Printf("Enter Any Key To Per Plane lending(Using ALPHA 1 VALUE) \n");
	UART_Printf("\n");
	UART_Getc();
	
	GLIB_ClearFrame(C_YELLOW);
	LCD_SetBlendingType(PER_PLANE, ALPHA1_PLANE, eFgWin);	
	LCD_SetAlpha(0x0, 0x0, 0x0, 0x0, 0x0, 0x0, eFgWin);		
	LCD_Trigger();
	
	for (j=0; j<0x10; j++)
	{
		
		LCD_SetAlpha(0x0, 0x0, 0x0, 0x0+l, 0x0+m, 0x0+n, eFgWin);
		LCD_Trigger();		

		l++;
		m = (m<0xf) ? m+2 : m;
		n = (n<0xf) ? n+3 : n;

		UART_Printf("Enter Any Key To Test Per Plane Blending (Using ALPHA 1 VALUE): %d\n", j);
		UART_Getc();
	}

}

static void BlendingPerPixel(void)
{
	u32 uFgAddr;
	u32 uHSz, uVSz;
	int j;
	
	bIsRev = false;
	bIsAllWin = false;
	
	UART_Printf("Enter Any Key To Test Per Pixel Blending\n");
	UART_Getc();
	
	LCD_InitDISPC(eBgBpp, uLcdFbAddr, eBgWin, false);
	LCD_SetWinOnOff(1, eBgWin);	
	LCD_GetFrmSz(&uHSz,  &uVSz, eBgWin);
	
	LCD_Start();

	// Draw Pattern by using Glib for window 0
	UART_Printf("Window 0(Background) is Displayed\n");	
	GLIB_Init(uLcdFbAddr, uHSz, uVSz, eBgBpp);
	GLIB_DrawPattern(uHSz, uVSz);
	LCD_Trigger();

	UART_Printf("Enter Any Key To Display Window %d (Foreground)\n", (int)eFgWin);
	UART_Getc();

	LCD_GetFbEndAddr(&uFgAddr, eBgWin);

	LCD_InitWin(eFgArgbBpp, uHSz, uVSz, 0, 0, uHSz, uVSz, 0, 0, uFgAddr, eFgWin, false);

	LCD_SetWinOnOff(1, eFgWin);

	LCD_GetFrmSz(&uHSz, &uVSz, eFgWin);

	LCD_SetBlendingType(PER_PIXEL, AEN_PIXEL, eFgWin);

	GLIB_Init(uFgAddr, uHSz, uVSz, eFgArgbBpp);
	GLIB_SetFontColor(C_BLACK, C_WHITE, true);

	GLIB_ClearFrame3(C_RED, ALPHA_0);
	GLIB_Printf(20, 40, "Alpha 0 is applied");

	GLIB_FillRectangle3(40, 60, 160, 120, C_BLUE, ALPHA_1);
	GLIB_Printf(50, 70, "Alpha 1 is applied");
	LCD_Trigger();		

	UART_Printf("\n");
	UART_Printf("Window %d (Foreground) is Displayed\n", (int)eFgWin);
	UART_Printf("Enter Any Key To Test Per-Pixel Blending\n");
	UART_Getc();

	for (j=0; j<0x10; j++)
	{
		LCD_SetAlpha(0x0+j, 0x0+j, 0x0+j, 0xf-j, 0xf-j, 0xf-j, eFgWin);
		LCD_Trigger();
		UART_Printf("Enter Any Key To Test Per-Pixel Blending: %d\n", j);
		UART_Getc();
	}
}

static void DisIntRgb(void)
{
	u32 i;
	u32 uHSz, uVSz;
	u32 uHSz2, uVSz2;
	u32 uHSz3, uVSz3;	
	u32 uHSz4, uVSz4;		
	FRAME_INT_SEL eFIntSel;
	FIFO_INT_LEVEL eFifoIntLevel;
	FIFO_INT_SEL eFifoIntSel;
	u32 uLcdFbAddr2;
	u32 uLcdFbAddr3;	
	u32 ui,j,k;

	bIsRev = false;
	bIsAllWin = false;

	LCD_InitDISPC(eBgBpp, uLcdFbAddr, eBgWin, false);
	
	LCD_SetWinOnOff(1, eBgWin);
	LCD_GetFrmSz(&uHSz, &uVSz, eBgWin);
	GLIB_InitInstance(uLcdFbAddr, uHSz, uVSz, eBgBpp);
	GLIB_DrawPattern(uHSz, uVSz);


	UART_Printf("\nDisplay Frame Interrupt. \n");
	UART_Printf("Background: WINDOW %d\n", (int)eBgWin);
	
	INTC_SetVectAddr(NUM_LCD1, Isr_LCD_FrameInt);

	for(i=0;i<4;i++)
	{
	       eFIntSel = 
		(i == 0) ? FINT_BACKPORCH :
		(i == 1) ? FINT_VSYNC :			
		(i == 2) ? FINT_ACTIVE : FINT_FRONTPORCH;
	       
		LCD_InitInt(LCD_FRAME_INT, eLcdCpuifInt, FIFO_25, FIFO_INT_WIN0, eFIntSel);
		LCD_EnableInt();
	
		INTC_Enable(NUM_LCD1);

		LCD_Start();
	
		UART_Printf("\nIf you want to stop Frame Int., press any key\n");
		UART_Getc();
	
		INTC_Disable(NUM_LCD1);
		LCD_DisableInt();
	}

	UART_Printf("\nDisplay FIFO Interrupt. \n");
	INTC_SetVectAddr(NUM_LCD0, Isr_LCD_FifoInt);

	UART_Printf("\nIf you want to stop FIFO Int., press any key\n");

	 eFifoIntSel=
		(eBgWin == WIN0) ? FIFO_INT_WIN0 :
		(eBgWin == WIN1) ? FIFO_INT_WIN1 :
		(eBgWin == WIN2) ? FIFO_INT_WIN2 :
		(eBgWin == WIN3) ? FIFO_INT_WIN3 : FIFO_INT_WIN4;

	for(i=0;i<5;i++)
	{
		uCount=0;
		
	       eFifoIntLevel = 
		(i == 0) ? FIFO_25 :
		(i == 1) ? FIFO_50 :		
		(i == 2) ? FIFO_75 :	
		(i == 3) ? FIFO_EMPTY : FIFO_FULL;
	       
		LCD_InitInt(LCD_FIFO_INT, eLcdCpuifInt, eFifoIntLevel, eFifoIntSel, FINT_BACKPORCH);
		LCD_EnableInt();
	
		INTC_Enable(NUM_LCD0);

		LCD_Start();
	
		while(GetKey() == 0)
		{}
		
		INTC_Disable(NUM_LCD0);
		LCD_DisableInt();

		UART_Printf("\n");
	}

} 


static void DoubleBuffering(void)
{
	u32 uDoubleBufAddr;
	u32 uHSz, uVSz;
	LCD_WINDOW eBgWin_Temp;

	bIsRev = false;
	bIsAllWin = false;
	
	// Enable Double Buffer
	UART_Printf("Enter Any Key To Test Simple Double Buffering\n");
	UART_Getc();

	eBgWin_Temp=eBgWin;
	
	eBgWin=WIN0;
	
	LCD_InitDISPC(eBgBpp, uLcdFbAddr, eBgWin, true);
	LCD_SetWinOnOff(1, eBgWin);	
	LCD_GetFrmSz(&uHSz,  &uVSz, eBgWin);

	INTC_SetVectAddr(NUM_LCD1, Isr_LCD_FrameInt);
	LCD_InitInt(LCD_FRAME_INT, eLcdCpuifInt, FIFO_25, FIFO_INT_WIN0, FINT_VSYNC);
	LCD_EnableInt();
	INTC_Enable(NUM_LCD1);

	UART_Printf("Window 0's Double Buffering Test\n");
	UART_Printf("Enter Any Key To Exit Double Buffering Test\n");

	LCD_GetDoubleBufAddr(&uDoubleBufAddr, 0, eBgWin);
	GLIB_Init(uDoubleBufAddr, uHSz, uVSz, eBgBpp);
	//GLIB_ClearFrame(C_YELLOW);
	GLIB_DrawPattern(uHSz, uVSz);
	
	LCD_SetActiveFrameBuf(0, eBgWin);
	GLIB_SetFontColor(C_BLACK, C_YELLOW, false);
	GLIB_Printf(40, 160, "WINDOW 0's Buffer %d", 0);

	LCD_GetDoubleBufAddr(&uDoubleBufAddr, 1, eBgWin);
	GLIB_Init(uDoubleBufAddr, uHSz, uVSz, eBgBpp);
	//GLIB_ClearFrame(C_GREEN);
	GLIB_DrawPattern(uHSz, uVSz);

	LCD_SetActiveFrameBuf(1, eBgWin);
	GLIB_SetFontColor(C_BLACK, C_GREEN, false);
	GLIB_Printf(40, 160, "WINDOW 0's Buffer %d", 1);

	LCD_Start();

	while(GetKey() == 0)
	{
		//LCD_Trigger();
	}

	LCD_SetWinOnOff(0, eBgWin);
	INTC_Disable(NUM_LCD1);
	LCD_DisableInt();

	UART_Printf("\n");	
	UART_Printf("Window 1's Double Buffering Test\n");
	UART_Printf("Enter Any Key To Exit Double Buffering Test\n");
	eBgWin=WIN1;
	
	LCD_InitWin(eBgBpp, uHSz, uVSz, 0, 0, uHSz, uVSz, 0, 0, uLcdFbAddr, eBgWin, true);
	LCD_SetWinOnOff(1, eBgWin);
	LCD_GetFrmSz(&uHSz, &uVSz, eBgWin);

	INTC_SetVectAddr(NUM_LCD1, Isr_LCD_FrameInt);
	LCD_InitInt(LCD_FRAME_INT, eLcdCpuifInt, FIFO_25, FIFO_INT_WIN0, FINT_ACTIVE);
	LCD_EnableInt();
	INTC_Enable(NUM_LCD1);
	

	LCD_GetDoubleBufAddr(&uDoubleBufAddr, 0, eBgWin);
	GLIB_Init(uDoubleBufAddr, uHSz, uVSz, eBgBpp);
	GLIB_ClearFrame(C_WHITE);

	LCD_SetActiveFrameBuf(0, eBgWin);
	GLIB_SetFontColor(C_BLACK, C_WHITE, false);
	GLIB_Printf(40, 160, "WINDOW 1's Buffer %d", 0);

	LCD_GetDoubleBufAddr(&uDoubleBufAddr, 1, eBgWin);
	GLIB_Init(uDoubleBufAddr, uHSz, uVSz, eBgBpp);
	GLIB_ClearFrame(C_BLUE);

	LCD_SetActiveFrameBuf(1, eBgWin);
	GLIB_SetFontColor(C_BLACK, C_BLUE, false);
	GLIB_Printf(40, 160, "WINDOW 1's Buffer %d", 1);

	while(GetKey() == 0)
	{
		//LCD_Trigger();
	}

	LCD_SetWinOnOff(0, eBgWin);
	INTC_Disable(NUM_LCD1);
	LCD_DisableInt();

	LCD_PerFrameOff();

	eBgWin = eBgWin_Temp;
}


static void AutoDoubleBuffering(void)
{
	u32 uDoubleBufAddr;
	u32 uHSz, uVSz;
	u32 uBufferIdx = 0;
	
	bIsRev = false;
	bIsAllWin = false;

	// Enable Double Buffer
	UART_Printf("Enter Any Key To Test Auto Double Buffering\n");
	UART_Getc();
	
 	//// WIN 0 ////	
	
	LCD_InitDISPC(eBgBpp,  uLcdFbAddr, WIN0, true);
	LCD_SetWinOnOff(1, WIN0);	
	LCD_GetFrmSz(&uHSz,  &uVSz, WIN0);

	//LCD_Start();
	
	LCD_GetDoubleBufAddr(&uDoubleBufAddr, 0, WIN0);
	GLIB_Init(uDoubleBufAddr, uHSz, uVSz, eBgBpp);
	GLIB_DrawPattern(uHSz, uVSz);

	LCD_GetDoubleBufAddr(&uDoubleBufAddr, 1, WIN0);
	GLIB_Init(uDoubleBufAddr, uHSz, uVSz, eBgBpp);	
	GLIB_DrawPattern2(uHSz, uVSz);

	LCD_SetActiveFrameBuf(1, WIN0);
	LCD_EnableAutoBuf(1, WIN0); // 0: Normal Double Buffering 1: Auto Double Buffering

	LCD_Start();

	UART_Printf("Enter Any Key To Exit Test\n");
	while (GetKey() == 0)
	{
//		LCD_Trigger();
		LCD_GetBufIdx(&uBufferIdx, WIN0);
		
		UART_Printf("Selected Buffer Index(WIN 0): %d\n", uBufferIdx);
		Delay(1000);
	}

	UART_Printf("\n");
	UART_Printf("Enter Any Key To Test WIN 1's Auto Double Buffering\n");
	UART_Getc();

	LCD_SetWinOnOff(0, WIN0);

 	//// WIN 1 ////
	LCD_InitWin(eBgBpp, uHSz, uVSz, 0, 0, uHSz, uVSz, 0, 0, uLcdFbAddr, WIN1, true);

	LCD_SetWinOnOff(1, WIN1);
	
	uBufferIdx = 0;
	
	LCD_GetDoubleBufAddr(&uDoubleBufAddr, 0, WIN1);
	GLIB_Init(uDoubleBufAddr, uHSz, uVSz, eBgBpp);
	GLIB_DrawPattern(uHSz, uVSz);
	LCD_Trigger();	

	LCD_GetDoubleBufAddr(&uDoubleBufAddr, 1, WIN1);
	GLIB_Init(uDoubleBufAddr, uHSz, uVSz, eBgBpp);	
	GLIB_DrawPattern2(uHSz, uVSz);
	LCD_Trigger();

	LCD_SetActiveFrameBuf(1, WIN1);
	LCD_EnableAutoBuf(1, WIN1); // 0: Normal Double Buffering 1: Auto Double Buffering

	UART_Printf("Enter Any Key To Exit Test\n");
	while (GetKey() == 0)
	{
		LCD_Trigger();
		LCD_GetBufIdx(&uBufferIdx, WIN1);
		UART_Printf("Selected Buffer Index(WIN 1): %d\n", uBufferIdx);
		Delay(1000);
	}

	LCD_SetWinOnOff(0, WIN1);
	LCD_Stop();
}

static void NonPaletteBpp(void)
{
	u32 uHSz, uVSz, uNofPixel;
	u32 uColor[16];
	u32 i;
	
	bIsRev = false;
	bIsAllWin = false;

	uColor[0] = 0x0;				// Black
	uColor[1] = 0x3f<<0; 			// Blue
	uColor[2] = 0x3f<<6; 			// Green
	uColor[3] = (0x3f<<6)|(0x3f<<0);  // Cyan

	uColor[4] = 0x3f<<12; 			// Red
	uColor[5] = (0x3f<<12)|(0x3f<<0); // Magenta
	uColor[6] = (0x3f<<12)|(0x3f<<6); // Yellow
	uColor[7] = 0x3ffff; 			// White

	
	UART_Printf("Enter Any Key To Test 18 bpp.\n");
	UART_Getc();

	LCD_InitDISPC(RGB18, uLcdFbAddr, eBgWin, false);
	LCD_GetFrmSz(&uHSz,  &uVSz, eBgWin);
	LCD_SetWinOnOff(1, eBgWin);

	uNofPixel = uHSz*uVSz;
		
	for (i=0; i<uNofPixel; i++)
	{			
		if ( i < uNofPixel/8) 					  *(u32*)(uLcdFbAddr+i*4) = uColor[0];
		else if ( i >= uNofPixel/8 && i < uNofPixel/8*2)    *(u32*)(uLcdFbAddr+i*4) = uColor[1];
		else if ( i >= uNofPixel/8*2 && i < uNofPixel/8*3) *(u32*)(uLcdFbAddr+i*4) = uColor[2];
		else if ( i >= uNofPixel/8*3 && i < uNofPixel/8*4) *(u32*)(uLcdFbAddr+i*4) = uColor[3];
		else if ( i >= uNofPixel/8*4 && i < uNofPixel/8*5) *(u32*)(uLcdFbAddr+i*4) = uColor[4];
		else if ( i >= uNofPixel/8*5 && i < uNofPixel/8*6) *(u32*)(uLcdFbAddr+i*4) = uColor[5];
		else if ( i >= uNofPixel/8*6 && i < uNofPixel/8*7) *(u32*)(uLcdFbAddr+i*4) = uColor[6];
		else if ( i >= uNofPixel/8*7 && i < uNofPixel/8*8) *(u32*)(uLcdFbAddr+i*4) = uColor[7];	
	}
			
	LCD_Start();

	UART_Printf("Window %d (Background) Is Displayed\n", (int)eBgWin);
	
	UART_Printf("Enter Any Key To Exit.\n");
	UART_Getc();
		
}

static void Palette(void)
{
	u32 uColor[16];
	u32 uHSz, uVSz, uNofPixel;
	
	int i;
	
	bIsRev = false;
	bIsAllWin = false;

	LCD_ChangePaletteClk(1);
	
	UART_Printf("Window 0 Palette RAM Test. \n");
		if(LCD_CheckPaletteRAM(WIN0) == true)
			UART_Printf("Window 0 Palette RAM W/R OK.\n");
		else
			UART_Printf("Window 0 Palette RAM W/R Fail!\n");

	UART_Printf("Window 1 Palette RAM Test. \n");
		if(LCD_CheckPaletteRAM(WIN1) == true)
			UART_Printf("Window 1 Palette RAM W/R OK.\n");
		else
			UART_Printf("Window 1 Palette RAM W/R Fail!\n");

	LCD_ChangePaletteClk(0);	
	
	LCD_InitDISPC(ePalBpp, uLcdFbAddr, eBgWin, false);	
	LCD_SetWinOnOff(1, eBgWin);
	LCD_GetFrmSz(&uHSz, &uVSz, eBgWin);
	
	LCD_SetPaletteCtrl(ePalDataBpp, eBgWin); 
	GLIB_Init(uLcdFbAddr, uHSz, uVSz, ePalBpp);
	GLIB_SetPalDataFormat(ePalDataBpp);

	LCD_Start();
	///////////////////////////////////////////////////////////
	// Palette Data BPP
	if (ePalDataBpp == RGB16)
	{
		uColor[0] = 0x0;	// Black
		uColor[1] = 0xffff; // White
		uColor[2] = 0x1f<<0; // Blue	
		uColor[3] = (0x1f<<11)|(0x3f<<6); // Yellow
		uColor[4] = (0x1f<<11)|(0x1f<<0); // Magenta
		uColor[5] = (0x3f<<6)|(0x1f<<0); // Cyan
		uColor[6] = 0x1f<<11; // Red
		uColor[7] = (0x23<<6)|(0x1f<<0); // Cyan2
		uColor[8] = (0x07<<11)|(0x15<<0); // Magenta1	
		uColor[9] =	(0x18<<11)|(0x17<<6); // Yellow1
		uColor[10] = 0x7777; // Gray	
		uColor[11] = 0x3f<<6; // Green	
		uColor[12] = (0x03<<11)|(0x1f<<0); // Magenta2		
		uColor[13] = (0x1f<<11)|(0x1a<<6); // Yellow2
		uColor[14] = (0x3f<<6)|(0x16<<0); // Cyan2
		uColor[15] = (0x10<<11)|(0x1d<<6)|(0x1f<<0); // Gray2							
	}

	///////////////////////////////////////////////////////////
	// Palette Depth
	if (ePalBpp == PAL8)
	{
		UART_Printf("Palette 8bpp Test. \n");
		
		LCD_ChangePaletteClk(1);
		
		GLIB_GetPalDataFormat(C_RED, &uColor[0]);		
		LCD_SetPaletteData(C_RED, uColor[0], eBgWin);
		
		GLIB_GetPalDataFormat(C_GREEN, &uColor[0]);
		LCD_SetPaletteData(C_GREEN, uColor[0], eBgWin);

		GLIB_GetPalDataFormat(C_BLUE, &uColor[0]);
		LCD_SetPaletteData(C_BLUE, uColor[0], eBgWin);
		
		GLIB_GetPalDataFormat(C_CYAN, &uColor[0]);
		LCD_SetPaletteData(C_CYAN, uColor[0], eBgWin);
		
		GLIB_GetPalDataFormat(C_MAGENTA, &uColor[0]);
		LCD_SetPaletteData(C_MAGENTA, uColor[0], eBgWin);
		
		GLIB_GetPalDataFormat(C_YELLOW, &uColor[0]);
		LCD_SetPaletteData(C_YELLOW, uColor[0], eBgWin);

		GLIB_GetPalDataFormat(C_BLACK, &uColor[0]);
		LCD_SetPaletteData(C_BLACK, uColor[0], eBgWin);
		
		GLIB_GetPalDataFormat(C_WHITE, &uColor[0]);
		LCD_SetPaletteData(C_WHITE, uColor[0], eBgWin);

		LCD_ChangePaletteClk(0);
		
		GLIB_DrawPattern(uHSz, uVSz);

		LCD_Trigger();		
			
	}
	else if (ePalBpp == PAL4)
	{
		UART_Printf("Palette 4bpp Test. \n");
		
		LCD_ChangePaletteClk(1);		

		for (i=0; i<16; i++)
			LCD_SetPaletteData(i, uColor[i], eBgWin);
		
		LCD_ChangePaletteClk(0);


		uNofPixel = uHSz*uVSz/8;
		
		for (i=0; i<uNofPixel; i++)
		{
			
			if ( i < uNofPixel/16) *(u32*)(uLcdFbAddr+i*4) = 0x0;
			else if ( i >= uNofPixel/16 && i < uNofPixel/16*2) *(u32*)(uLcdFbAddr+i*4) = 0x11111111;
			else if ( i >= uNofPixel/16*2 && i < uNofPixel/16*3) *(u32*)(uLcdFbAddr+i*4) = 0x22222222;
			else if ( i >= uNofPixel/16*3 && i < uNofPixel/16*4) *(u32*)(uLcdFbAddr+i*4) = 0x33333333;
			else if ( i >= uNofPixel/16*4 && i < uNofPixel/16*5) *(u32*)(uLcdFbAddr+i*4) = 0x44444444;
			else if ( i >= uNofPixel/16*5 && i < uNofPixel/16*6) *(u32*)(uLcdFbAddr+i*4) = 0x55555555;
			else if ( i >= uNofPixel/16*6 && i < uNofPixel/16*7) *(u32*)(uLcdFbAddr+i*4) = 0x66666666;
			else if ( i >= uNofPixel/16*7 && i < uNofPixel/16*8) *(u32*)(uLcdFbAddr+i*4) = 0x77777777;
			else if ( i >= uNofPixel/16*8 && i < uNofPixel/16*9) *(u32*)(uLcdFbAddr+i*4) = 0x88888888;
			else if ( i >= uNofPixel/16*9 && i < uNofPixel/16*10) *(u32*)(uLcdFbAddr+i*4) = 0x99999999;
			else if ( i >= uNofPixel/16*10 && i < uNofPixel/16*11) *(u32*)(uLcdFbAddr+i*4) = 0xaaaaaaaa;
			else if ( i >= uNofPixel/16*11 && i < uNofPixel/16*12) *(u32*)(uLcdFbAddr+i*4) = 0xbbbbbbbb;
			else if ( i >= uNofPixel/16*12 && i < uNofPixel/16*13) *(u32*)(uLcdFbAddr+i*4) = 0xcccccccc;
			else if ( i >= uNofPixel/16*13 && i < uNofPixel/16*14) *(u32*)(uLcdFbAddr+i*4) = 0xdddddddd;
			else if ( i >= uNofPixel/16*14 && i < uNofPixel/16*15) *(u32*)(uLcdFbAddr+i*4) = 0xeeeeeeee;
			else if ( i >= uNofPixel/16*15 && i < uNofPixel/16*16) *(u32*)(uLcdFbAddr+i*4) = 0xffffffff;
			
		}
			
		LCD_Trigger();

		UART_Printf("Enter Any Key To Exit Current Display.\n");
		UART_Getc();

	}
	else if (ePalBpp == PAL2)
	{
		UART_Printf("Palette 2bpp Test. \n");
		
		LCD_ChangePaletteClk(1);
		for (i=0; i<4; i++)
			LCD_SetPaletteData(i, uColor[i], eBgWin);
		
		LCD_ChangePaletteClk(0);

		uNofPixel = uHSz*uVSz/16;
		
		for (i=0; i<uNofPixel ; i++)
		{
			if ( i < uNofPixel/4) *(u32*)(uLcdFbAddr+i*4) = 0x0;
			else if ( i >= uNofPixel/4 && i < uNofPixel/4*2) *(u32*)(uLcdFbAddr+i*4) = 0x55555555;
			else if ( i >= uNofPixel/4*2 && i < uNofPixel/4*3) *(u32*)(uLcdFbAddr+i*4) = 0xaaaaaaaa;
			else if ( i >= uNofPixel/4*3 && i < uNofPixel/4*4) *(u32*)(uLcdFbAddr+i*4) = 0xffffffff;
		}
		LCD_Trigger();
	}
	else // ePalBpp == 1Bpp
	{
		UART_Printf("Palette 1bpp Test. \n");
		
		LCD_ChangePaletteClk(1);
		for (i=0; i<2; i++)
			LCD_SetPaletteData(i, uColor[i], eBgWin);
		
		LCD_ChangePaletteClk(0);

		uNofPixel = uHSz*uVSz/32;
		
		for (i=0; i<uNofPixel ; i++)
		{
			if ( i < uNofPixel/2) *(u32*)(uLcdFbAddr+i*4) = 0x0;
			else if ( i >= uNofPixel/2 && i < uNofPixel/2*2) *(u32*)(uLcdFbAddr+i*4) = 0xffffffff;
		}
		LCD_Trigger();	
	}

	UART_Printf("Enter Any Key To Exit.\n");
	UART_Getc();
}

static void Swap(void)
{
	u32 uColor[16];
	u32 uHSz, uVSz, uNofPixel;
	u32 uFgAddr;
	LCD_WINDOW eWinTemp0, eWinTemp1;
	CSPACE eCspaceTemp;
	
	int i;

	#if (LCD_MODULE_TYPE== LTV350QV_RGB)	
		uHSz=320;
		uVSz=240;
	
	#elif (LCD_MODULE_TYPE == LTS222QV_CPU)
		uHSz=240;
		uVSz=320;

	#elif (LCD_MODULE_TYPE == LTP700WV_RGB ||LCD_MODULE_TYPE ==LTE480WV_RGB)
		uHSz=800;
		uVSz=480;
		
	#elif (LCD_MODULE_TYPE == LTV300GV_RGB)
		uHSz=640;
		uVSz=480;
	#elif (LCD_MODULE_TYPE == LMS480KC01_RGB)
		uHSz=240;
		uVSz=320;
	#else
		Assert(0);
	#endif

	bIsRev = false;
	bIsAllWin = false;

	eWinTemp0 = eBgWin;
	eWinTemp1 = eFgWin;
	eCspaceTemp = ePalBpp;
	
	eBgWin = WIN2;
	eFgWin = WIN3;
	ePalBpp = PAL4;

	LCD_InitBase();
	LCD_InitWin(ePalBpp, uHSz, uVSz, 0, 0, uHSz, uVSz*3/4, 0, 0, uLcdFbAddr, eBgWin, false);
	
	LCD_SetWinOnOff(1, eBgWin);
	LCD_GetFrmSz(&uHSz, &uVSz, eBgWin);
	
	LCD_SetPaletteCtrl(ePalDataBpp, eBgWin); 

	GLIB_Init(uLcdFbAddr, uHSz, uVSz, ePalBpp);
	GLIB_SetPalDataFormat(ePalDataBpp);

	LCD_Start();
	///////////////////////////////////////////////////////////
	// Palette Data BPP
	if (ePalDataBpp == RGB16)
	{
		uColor[0] = 0x0;	// Black
		uColor[1] = 0xffff; // White
		uColor[2] = 0x1f<<0; // Blue	
		uColor[3] = (0x1f<<11)|(0x3f<<6); // Yellow
		uColor[4] = (0x1f<<11)|(0x1f<<0); // Magenta
		uColor[5] = (0x3f<<6)|(0x1f<<0); // Cyan
		uColor[6] = 0x1f<<11; // Red
		uColor[7] = (0x23<<6)|(0x1f<<0); // Cyan2
		uColor[8] = (0x07<<11)|(0x15<<0); // Magenta1	
		uColor[9] =	(0x18<<11)|(0x17<<6); // Yellow1
		uColor[10] = 0x7777; // Gray	
		uColor[11] = 0x3f<<6; // Green	
		uColor[12] = (0x03<<11)|(0x1f<<0); // Magenta2		
		uColor[13] = (0x1f<<11)|(0x1a<<6); // Yellow2
		uColor[14] = (0x3f<<6)|(0x16<<0); // Cyan2
		uColor[15] = (0x10<<11)|(0x1d<<6)|(0x1f<<0); // Gray2							
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	UART_Printf("Byte Swap Test. \n");
	
	LCD_ChangePaletteClk(1);	

	for (i=0; i<16; i++)
		LCD_SetPaletteData(i, uColor[i], eBgWin);
	
	LCD_ChangePaletteClk(0);


	LCD_SetByteSwap(SWAP_BIT_DISABLE, SWAP_BYTE_ENABLE, SWAP_HALFWORD_DISABLE, eBgWin);

	uNofPixel = uHSz*uVSz/8;
	
	for (i=0; i<uNofPixel; i++)
	{

		if(i<uNofPixel/16*4) *(u32*)(uLcdFbAddr+i*4) = 0x22;
		else if ( i >= uNofPixel/16*4 && i < uNofPixel/16*8) *(u32*)(uLcdFbAddr+i*4) = 0x33;
		else if ( i >= uNofPixel/16*8 && i < uNofPixel/16*12) *(u32*)(uLcdFbAddr+i*4) = 0x44;
		else if ( i >= uNofPixel/16*12 && i < uNofPixel/16*16) *(u32*)(uLcdFbAddr+i*4) = 0x0;
	}

	LCD_GetFbEndAddr(&uFgAddr, eBgWin);

	LCD_InitWin(ePalBpp, uHSz, uVSz, 0, 0, uHSz, uVSz/4, 0, uVSz*3/4, uFgAddr, eFgWin, false);
	LCD_SetWinOnOff(1, eFgWin);
	LCD_GetFrmSz(&uHSz, &uVSz, eFgWin);

	LCD_SetPaletteCtrl(ePalDataBpp, eFgWin); 

	GLIB_Init(uFgAddr, uHSz, uVSz, ePalBpp);
	GLIB_SetPalDataFormat(ePalDataBpp);

	LCD_ChangePaletteClk(1);	

	uNofPixel = uHSz*uVSz/8;
	for (i=0; i<16; i++)
		LCD_SetPaletteData(i, uColor[i], eFgWin);
	
	LCD_ChangePaletteClk(0);
	LCD_SetByteSwap(SWAP_BIT_DISABLE, SWAP_BYTE_DISABLE, SWAP_HALFWORD_DISABLE, eFgWin);

		for (i=0 ; i<uNofPixel/16*4; i++)
	{
		*(u32*)(uFgAddr+i*4) = 0x11000000;
	}

	UART_Printf("Enter Any Key To Exit Current Display. (BYTE SWAP)\n");
	UART_Getc();
	////////////////////////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////////////////////////
	UART_Printf("HW Swap Test. \n");
	LCD_ChangePaletteClk(1);		

	for (i=0; i<16; i++)
		LCD_SetPaletteData(i, uColor[i], eBgWin);
	
	LCD_ChangePaletteClk(0);


	LCD_SetByteSwap(SWAP_BIT_DISABLE, SWAP_BYTE_DISABLE, SWAP_HALFWORD_ENABLE, eBgWin);

	uNofPixel = uHSz*uVSz/8;
	
	for (i=0; i<uNofPixel; i++)
	{

		if(i<uNofPixel/16*4) *(u32*)(uLcdFbAddr+i*4) = 0x22;
		else if ( i >= uNofPixel/16*4 && i < uNofPixel/16*8) *(u32*)(uLcdFbAddr+i*4) = 0x33;
		else if ( i >= uNofPixel/16*8 && i < uNofPixel/16*12) *(u32*)(uLcdFbAddr+i*4) = 0x44;
		else if ( i >= uNofPixel/16*12 && i < uNofPixel/16*16) *(u32*)(uLcdFbAddr+i*4) = 0x0;
	}

	UART_Printf("Enter Any Key To Exit Current Display. (HW SWAP)\n");
	UART_Getc();
	////////////////////////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////////////////////////
	UART_Printf("Bit Swap Test. \n");
	LCD_ChangePaletteClk(1);		

	for (i=0; i<16; i++)
		LCD_SetPaletteData(i, uColor[i], eBgWin);
	
	LCD_ChangePaletteClk(0);

	LCD_SetByteSwap(SWAP_BIT_ENABLE, SWAP_BYTE_DISABLE, SWAP_HALFWORD_DISABLE, eBgWin);

	uNofPixel = uHSz*uVSz/8;
	
	for (i=0; i<uNofPixel; i++)
	{

		if(i<uNofPixel/16*4) *(u32*)(uLcdFbAddr+i*4) = 0x80000000;
		else if ( i >= uNofPixel/16*4 && i < uNofPixel/16*8) *(u32*)(uLcdFbAddr+i*4) = 0x80000000;
		else if ( i >= uNofPixel/16*8 && i < uNofPixel/16*12) *(u32*)(uLcdFbAddr+i*4) = 0x80000000;
		else if ( i >= uNofPixel/16*12 && i < uNofPixel/16*16) *(u32*)(uLcdFbAddr+i*4) = 0x0;
	}

	UART_Printf("Enter Any Key To Exit Current Display. (BIT SWAP)\n");
	UART_Getc();
	////////////////////////////////////////////////////////////////////////////////////////////////////
	
	eBgWin = eWinTemp0;
	eFgWin = eWinTemp1;
	ePalBpp = eCspaceTemp;
}


static void VDMABurstLength(void)
{
	u32 j;
	u32 uHSz, uVSz;
	BURST_LENGTH eBurstLen;
	FIFO_INT_LEVEL eFifoIntLevel;
	FIFO_INT_SEL eFifoIntSel;

	bIsRev = false;
	bIsAllWin = false;

	eFifoIntLevel = FIFO_EMPTY; 
	INTC_SetVectAddr(NUM_LCD0, Isr_LCD_FifoEmptyInt);
	
	for(j=0;j<3;j++)
	{
		if(j==0)
		{
			eBurstLen = BURST_16WORD;
			UART_Printf("\nVDMA Burst 16 Word.");
		}
		else if(j==1) 
		{
			eBurstLen = BURST_8WORD;
			UART_Printf("\nVDMA Burst 8 Word.");
		}
		else if(j==2) 
		{
			eBurstLen = BURST_4WORD;
			UART_Printf("\nVDMA Burst 4 Word.");
		}

		LCD_InitDISPC(eBgBpp, uLcdFbAddr, eBgWin, false);
		LCD_SetBurstLength(eBurstLen, eBgWin);
		LCD_SetWinOnOff(1, eBgWin);
		LCD_GetFrmSz(&uHSz, &uVSz, eBgWin);
		GLIB_InitInstance(uLcdFbAddr, uHSz, uVSz, eBgBpp);
		GLIB_DrawPattern(uHSz, uVSz);	
		
		 eFifoIntSel=
		(eBgWin == WIN0) ? FIFO_INT_WIN0 :
		(eBgWin == WIN1) ? FIFO_INT_WIN1 :
		(eBgWin == WIN2) ? FIFO_INT_WIN2 :
		(eBgWin == WIN3) ? FIFO_INT_WIN3 : FIFO_INT_WIN4;

		//UART_Printf("Background: WINDOW %d\n", (int)eBgWin);

		LCD_InitInt(LCD_FIFO_INT, eLcdCpuifInt, eFifoIntLevel, eFifoIntSel, FINT_BACKPORCH);
		LCD_EnableInt();
		INTC_Enable(NUM_LCD0);
					
		LCD_Start();
		
		while(!(UART_Getc()));
		
		LCD_DisableInt();
		INTC_Disable(NUM_LCD0);
		UART_Printf("\n");
	}	
}


static void WvgaDis(void)
{
	u32 uHSz, uVSz;
	u32 i, j, k;
	
	bIsRev = false;
	bIsAllWin = false;
	
	UART_Printf("Enter Any Key To Test Simple Display\n");
	UART_Getc();

	LCD_InitDISPC(RGB24, uLcdFbAddr, eBgWin, false);
	LCD_GetFrmSz(&uHSz, &uVSz, eBgWin);
	GLIB_InitInstance(uLcdFbAddr, uHSz, uVSz, RGB24);
	LCD_SetWinOnOff(1, eBgWin);
	
	k=0;
	for(i=0; i<uVSz; i++)
	for(j=0; j<uHSz; j++)
		GLIB_PutPixel2(j, i, pSmile_2[k++]);

	/*
	LCD_GetFbEndAddr(&uFbEndAddr, WIN0);
	
	LCD_InitWin(RGB24, uHSz, uVSz, 0, 0, uHSz, uVSz, 0, 0, uFbEndAddr, WIN1, false);
	LCD_SetWinOnOff(1, WIN1);

	LCD_GetFrmSz(&uHSz, &uVSz, WIN1);
	GLIB_InitInstance(uFbEndAddr, uHSz, uVSz, WIN1);
	GLIB_DrawPattern2(uHSz, uVSz);

	LCD_SetBlendingType(PER_PLANE, ALPHA0_PLANE, WIN1);
	LCD_SetAlpha(0x7, 0x7, 0x7, 0x0, 0x0, 0x0, WIN1);
	*/

	LCD_Start();

	UART_Printf("Window %d (Background) Is Displayed\n", (int)eBgWin);

	UART_Printf("Enter Any Key To Exit.\n");
	UART_Getc();

}

static void ParallelBGR(void)
{
	u32 uHSz, uVSz, uNofPixel;
	u32 uColor[16];
	u32 i;
	
	bIsRev = false;
	bIsAllWin = false;

	#if (LCD_MODULE_TYPE== LTV350QV_RGB)
		UART_Printf("Selected LCD Module Type: LTV350QV_RGB\n");
		uHSz=320;
		uVSz=240;
	
	#elif (LCD_MODULE_TYPE == LTS222QV_CPU)
		UART_Printf("Selected LCD Module Type: LTS222QV_CPU\n");
		uHSz=240;
		uVSz=320;

 	#elif (LCD_MODULE_TYPE == LTP700WV_RGB ||LCD_MODULE_TYPE ==LTE480WV_RGB)
		UART_Printf("Selected LCD Module Type: LTP700WV_RGB\n");
		uHSz=800;
		uVSz=480;	

	#elif (LCD_MODULE_TYPE == LTV300GV_RGB)
		UART_Printf("Selected LCD Module Type: LTV300GV_RGB\n");
		uHSz=640;
		uVSz=480;		
	
	#else
		Assert(0);
	#endif

	uColor[0] = 0x0;				// Black
	uColor[1] = 0xff<<0; 			// Blue
	uColor[2] = 0xff<<8; 			// Green
	uColor[3] = (0xff<<8)|(0xff<<0);  // Cyan

	uColor[4] = 0xff<<16; 			// Red
	uColor[5] = (0xff<<16)|(0xff<<0); // Magenta
	uColor[6] = (0xff<<16)|(0xff<<8); // Yellow
	uColor[7] = 0xffffff; 			// White

	UART_Printf("Parallel RGB Mode 24bpp Data Pattern.\n");
	
	LCD_InitBase2((CLK_SRC)LCLK_DOUT_MPLL, PNR_Parallel_RGB, CPU_16BIT);
	LCD_InitWin(RGB24, uHSz, uVSz, 0, 0, uHSz, uVSz, 0, 0, uLcdFbAddr, eBgWin, false);
	
	LCD_GetFrmSz(&uHSz,  &uVSz, eBgWin);
	LCD_SetWinOnOff(1, eBgWin);

	uNofPixel = uHSz*uVSz;
		
	for (i=0; i<uNofPixel; i++)
	{			
		if ( i < uNofPixel/8) 					  *(u32*)(uLcdFbAddr+i*4) = uColor[0];
		else if ( i >= uNofPixel/8 && i < uNofPixel/8*2)    *(u32*)(uLcdFbAddr+i*4) = uColor[1];
		else if ( i >= uNofPixel/8*2 && i < uNofPixel/8*3) *(u32*)(uLcdFbAddr+i*4) = uColor[2];
		else if ( i >= uNofPixel/8*3 && i < uNofPixel/8*4) *(u32*)(uLcdFbAddr+i*4) = uColor[3];
		else if ( i >= uNofPixel/8*4 && i < uNofPixel/8*5) *(u32*)(uLcdFbAddr+i*4) = uColor[4];
		else if ( i >= uNofPixel/8*5 && i < uNofPixel/8*6) *(u32*)(uLcdFbAddr+i*4) = uColor[5];
		else if ( i >= uNofPixel/8*6 && i < uNofPixel/8*7) *(u32*)(uLcdFbAddr+i*4) = uColor[6];
		else if ( i >= uNofPixel/8*7 && i < uNofPixel/8*8) *(u32*)(uLcdFbAddr+i*4) = uColor[7];	
	}
			
	LCD_Start();
	UART_Printf("Window %d (Background) Is Displayed\n", (int)eBgWin);
	
	UART_Printf("Enter any key to test Parallel BGR Mode 24bpp Data Patttern.\n");
	UART_Getc();

	LCD_InitBase2((CLK_SRC)LCLK_DOUT_MPLL, PNR_Parallel_BGR, CPU_16BIT);
	LCD_InitWin(RGB24, uHSz, uVSz, 0, 0, uHSz, uVSz, 0, 0, uLcdFbAddr, eBgWin, false);
	
	LCD_GetFrmSz(&uHSz,  &uVSz, eBgWin);
	LCD_SetWinOnOff(1, eBgWin);

	uNofPixel = uHSz*uVSz;
		
	for (i=0; i<uNofPixel; i++)
	{			
		if ( i < uNofPixel/8) 					  *(u32*)(uLcdFbAddr+i*4) = uColor[0];
		else if ( i >= uNofPixel/8 && i < uNofPixel/8*2)    *(u32*)(uLcdFbAddr+i*4) = uColor[1];
		else if ( i >= uNofPixel/8*2 && i < uNofPixel/8*3) *(u32*)(uLcdFbAddr+i*4) = uColor[2];
		else if ( i >= uNofPixel/8*3 && i < uNofPixel/8*4) *(u32*)(uLcdFbAddr+i*4) = uColor[3];
		else if ( i >= uNofPixel/8*4 && i < uNofPixel/8*5) *(u32*)(uLcdFbAddr+i*4) = uColor[4];
		else if ( i >= uNofPixel/8*5 && i < uNofPixel/8*6) *(u32*)(uLcdFbAddr+i*4) = uColor[5];
		else if ( i >= uNofPixel/8*6 && i < uNofPixel/8*7) *(u32*)(uLcdFbAddr+i*4) = uColor[6];
		else if ( i >= uNofPixel/8*7 && i < uNofPixel/8*8) *(u32*)(uLcdFbAddr+i*4) = uColor[7];	
	}
	LCD_Start();

	UART_Printf("Enter any key to Exit.\n");
	UART_Getc();
}

static void SerialRGB(void)
{
	u32 uHSz, uVSz; 
	u32 i,j,k;
	
	bIsRev = false;
	bIsAllWin = false;
	
	#if (LCD_MODULE_TYPE== LTV350QV_RGB)
		UART_Printf("Selected LCD Module Type: LTV350QV_RGB\n");
		uHSz=320;
		uVSz=240;
	
	#elif (LCD_MODULE_TYPE == LTS222QV_CPU)
		UART_Printf("Selected LCD Module Type: LTS222QV_CPU\n");
		uHSz=240;
		uVSz=320;

	#elif (LCD_MODULE_TYPE == LTP700WV_RGB)
		UART_Printf("Selected LCD Module Type: LTP700WV_RGB\n");
		uHSz=800;
		uVSz=480;	

	#elif (LCD_MODULE_TYPE ==LTE480WV_RGB)
		UART_Printf("Selected LCD Module Type: LTE480WV_RGB\n");
		uHSz=800;
		uVSz=480;	

	#elif (LCD_MODULE_TYPE == LTS222QV_SRGB)	
		UART_Printf("Selected LCD Module Type: LTS222QV_SRGB\n");
		uHSz=240;
		uVSz=320;
	
	#elif (LCD_MODULE_TYPE == LTV300GV_RGB)
		UART_Printf("Selected LCD Module Type: LTV300GV_RGB\n");
		uHSz=640;
		uVSz=480;	
		
	#else
		Assert(0);
	#endif

	
	UART_Printf("Enter Any Key To Test Simple Serial RGB.\n");
	UART_Getc();
	
	LCD_InitBase2((CLK_SRC)LCLK_DOUT_MPLL, PNR_Serial_RGB, CPU_16BIT);
	
	LCD_InitWin(RGB16, uHSz, uVSz, 0, 0, uHSz, uVSz, 0, 0, uLcdFbAddr, eBgWin, false);

	LCD_SetWinOnOff(1, eBgWin);

	LCD_GetFrmSz(&uHSz, &uVSz, eBgWin);
	GLIB_InitInstance(uLcdFbAddr, uHSz, uVSz, RGB16);
	GLIB_DrawPattern(uHSz, uVSz);

	LCD_Start();

	UART_Printf("Window %d (Background) Is Displayed\n", (int)eBgWin);

	UART_Printf("Enter Any Key To Test Simple Serial BGR.\n");
	UART_Getc();

	LCD_InitBase2((CLK_SRC)LCLK_DOUT_MPLL, PNR_Serial_BGR, CPU_16BIT);
	LCD_InitWin(RGB16, uHSz, uVSz, 0, 0, uHSz, uVSz, 0, 0, uLcdFbAddr, eBgWin, false);

	LCD_SetWinOnOff(1, eBgWin);

	LCD_GetFrmSz(&uHSz, &uVSz, eBgWin);
	GLIB_InitInstance(uLcdFbAddr, uHSz, uVSz, RGB16);
	GLIB_DrawPattern(uHSz, uVSz);

	LCD_Start();

	UART_Printf("Enter Any Key To Exit.\n");
	UART_Getc();

#if 0 // serial RGB24 test. jungil 
	LCD_InitBase2((CLK_SRC)LCLK_DOUT_MPLL, PNR_Serial_RGB, CPU_16BIT);
	LCD_InitWin(RGB24, uHSz, uVSz, 0, 0, uHSz, uVSz, 0, 0, uLcdFbAddr, eBgWin, false);
	LCD_SetWinOnOff(1, eBgWin);

	GLIB_InitInstance(uLcdFbAddr, uHSz, uVSz, RGB24);
	k=0;
	for(i=0; i<320; i++)
		for(j=0; j<240; j++)
			GLIB_PutPixel2(j, i, pGanhi_image[k++]);

	LCD_Start();
	UART_Printf("Enter Any Key To Exit.\n");
	UART_Getc();
#endif

}

static void CameraLocalPath(void)
{
	u32 i;
	u32 uLcdHsz, uLcdVsz;
	u32 uWIN1FbEndAddr;
	u32 ui,j,k;
	u32 uLcdFbAddr2;
	u32 uLcdFbAddr3;	
	u32 uXsize, uYsize;
	u32 uSrcStAddr;



	POST_InitCh(POST_A, &oPost);

	uSimpleTest = 1;


	CAMERA_SFRInit();
   	CAMERA_ClkSetting(); // Camera Module CLK Setting

	UART_Printf("\nSelect Camera Local Path.\n");
	UART_Printf("0: Preview, 1: Codec\n");
	i = UART_GetIntNum();

   	if(i==0)
   	{		
		bPreviewDone = false;
	
		LCD_InitDISPC(RGB24, uLcdFbAddr, WIN1, false);
		LCD_GetFrmSz(&uLcdHsz, &uLcdVsz, WIN1);
		LCD_GetFbEndAddr(&uWIN1FbEndAddr, WIN1);	
		LCD_InitWinForFifoIn1(IN_CIM, WIN1, LOCALIN_RGB);
		LCD_SetWinOnOff(1, WIN1);
		LCD_Start();
		
		GLIB_InitInstance(uWIN1FbEndAddr, uLcdHsz, uLcdVsz, RGB24);
		GLIB_DrawPattern(uLcdHsz, uLcdVsz);

		//eDstDataFmt = RGB24;
		INTC_Enable(NUM_CAMIF_P);
		INTC_SetVectAddr(NUM_CAMIF_P, Isr_MSDMA_PreviewDone);
		CAMERA_InitDmaInPathToFifoOut(uLcdHsz, uLcdVsz, uWIN1FbEndAddr, RGB24, uLcdHsz, uLcdVsz, RGB24, ROT_0, P_PATH);
		//CAMERAT_DisplayParam();
		CAMERA_StartDmaInPath();



		UART_Printf("\nPress any key to stop the test!\n");
		UART_Getc();
		
		while(!bPreviewDone);

		CAMERA_StopDmaInPath();
		INTC_Disable(NUM_CAMIF_P);
		
		LCD_SetWinOnOff(0, WIN1);
		LCD_Stop();
   	}
   	else
   	{
		bCodecDone = false;
		
#if 0 // original code max size (lcd size local test)
		LCD_InitDISPC(RGB24, uLcdFbAddr, WIN2, false);
		LCD_GetFrmSz(&uLcdHsz, &uLcdVsz, WIN2);
		LCD_GetFbEndAddr(&uWIN1FbEndAddr, WIN2);	
		LCD_InitWinForFifoIn1(IN_CIM, WIN2, LOCALIN_YCbCr);
		LCD_SetWinOnOff(1, WIN2);
		LCD_Start();

		GLIB_InitInstance(uWIN1FbEndAddr, uLcdHsz, uLcdVsz, RGB24);
		k=0;
			for(ui=0; ui<480; ui++)
				for(j=0; j<800; j++)
					GLIB_PutPixel2(j, ui, pEntertainment1[k++]);
//		GLIB_DrawPattern2(uLcdHsz, uLcdVsz);
#else // For Test add jungil 080321 (osd local path test)
		UART_Printf("H Size Input = \n");
		uXsize = UART_GetIntNum();
	
		UART_Printf("V Size Input = \n");
		uYsize = UART_GetIntNum();


		*(volatile unsigned *)0x7410800c=0x0; // Modem I/F Bypass Control. It should be 0,
		*(volatile unsigned *)0x7e00f024= *(volatile unsigned *)0x7e00f024 | (0x1<<24); // Modem I/F Bypass Control. It should be 0,		
		LCD_InitDISPC(RGB24, uLcdFbAddr, WIN0, false);
		LCD_GetFrmSz(&uLcdHsz, &uLcdVsz, WIN0);	
		LCD_SetWinOnOff(1, WIN0);
		GLIB_InitInstance(uLcdFbAddr, uLcdHsz, uLcdVsz, RGB24);
		k=0;
		for(i=0; i<480; i++)
			for(j=0; j<800; j++)
				GLIB_PutPixel2(j, i, pSmile_3[k++]);
		LCD_GetFbEndAddr(&uLcdFbAddr2, WIN0);

//		LCD_Start();
//		UART_Printf("WIN0 Image [DMA Mode] Display \n");
//		UART_Getc();

		UART_Printf("\nHit any key to start OSD Local Path test[Codec Path]!\n");
		UART_Getc();
		LCD_SetAllWinOnOff(0);

		GLIB_InitInstance(uLcdFbAddr2, uXXXSize, uYYYSize, RGB24);
		k=0;
#if 0 // for test image . delete code size. jungil 080619
		for(i=0; i<uYYYSize; i++){
			for(j=0; j<uXXXSize; j++){
				if ( uXXXSize == 720 )	GLIB_PutPixel2(j, i, pTestRGB24[k++]);
				else if ( uXXXSize == 640 )	GLIB_PutPixel2(j, i, p640480RGB24[k++]);		
				else if ( uXXXSize == 320 )	GLIB_PutPixel2(j, i, pKangjjang320240[k++]);
				}
			}
#endif
		LCD_InitWin(RGB24, uXsize, uYsize, 0, 0, uXsize, uYsize, 0, 0, uLcdFbAddr3, WIN2, false);
//		LCD_InitWinForFifoIn2(IN_CIM, WIN2, LOCALIN_YCbCr, uXsize, uYsize); // Local YCBYCR Format
		LCD_InitWinForFifoIn2(IN_CIM, WIN2, LOCALIN_RGB, uXsize, uYsize);		
//		LCD_SetWinOnOff(1, WIN0);
		LCD_SetWinOnOff(1, WIN2);
		LCD_Start();
#endif
		uImgHSz = uXsize;
		uImgVSz = uYsize;
		uSrcStAddr = 0x52500000;
		eSrcDataFmt = eSrcDataFmt2;

		#if 0 
		POST_InitIpForDmaInDmaOut(320, 240, uLcdFbAddr2, RGB24, 320, 240, uSrcStAddr, eSrcDataFmt, 1, false, ONE_SHOT, &oPost);
		POST_StartProcessing(&oPost);
		while (!POST_IsProcessingDone(&oPost));
		#endif

		INTC_Enable(NUM_CAMIF_C);
		INTC_SetVectAddr(NUM_CAMIF_C, Isr_MSDMA_CodecDone);
		//eDstDataFmt = RGB24;
		CAMERA_InitDmaInPathToFifoOut(uXXXSize, uYYYSize, uLcdFbAddr2, RGB24, uXsize, uYsize, RGB24, ROT_0, C_PATH);
		//CAMERAT_DisplayParam();
		CAMERA_StartDmaInPath();

		UART_Printf("Press any key to stop the test!\n");
		UART_Getc();

		while(!bCodecDone);
		CAMERA_WaitMSDMAC();
		CAMERA_StopDmaInPath();
		INTC_Disable(NUM_CAMIF_C);

		LCD_SetWinOnOff(0, WIN2);
		LCD_Stop();
   	}  
	
}

// One-shot mode isn't supported in local path
// so, Only Free-Run mode is supported in local path
// In local path, scaling isn't supported because LCDC doesn't support that feature, but POST side is able to scale
static void PostLocalPath2(void)
{
	u32 uGlibStAddr, uSrcStAddr;
	u32 i, uTemp_reg;
	u32 uLcdHsz, uLcdVsz, uSRCWidth, uSRCHeight, uSRCHeight_Temp;
	u32 uOffset, uBytesPerPixel;
	u32 uWIN1FbEndAddr;
	u32 ui,j,k;
	u32 uLcdFbAddr2;
	u32 uLcdFbAddr3;	
	u32 uXsize, uYsize;
	u32 rPRESCALE_RATIO = 0x77000004;
	u32 rPRESCALE_SHFACTOR = 0x7700001c;
	u32 rOFFSET_Y = 0x77000040;
	u32 rOFFSET_CB = 0x77000044;
	u32 rOFFSET_CR = 0x77000048;
	u32 rADDRStart_Y = 0x77000020;
	u32 rADDRStart_CB = 0x77000024;
	u32 rADDRStart_CR = 0x77000028;
	u32 rADDREnd_Y = 0x77000030;
	u32 rADDREnd_CB = 0x77000034;
	u32 rADDREnd_CR = 0x77000038;
	u32 rSRCImgSize = 0x7700000c;
	u32 rNxtADDRStart_Y = 0x77000054;
	u32 rNxtADDRStart_CB = 0x77000058;
	u32 rNxtADDRStart_CR = 0x7700005c;
	u32 rNxtADDREnd_Y = 0x77000064;
	u32 rNxtADDREnd_CB = 0x77000068;
	u32 rNxtADDREnd_CR = 0x7700006c;

	
	UART_Printf("Post Simple Dma To Fifo Test.\n");

	POST_InitCh(POST_A, &oPost);

	uSimpleTest = 1;


#if 1 // original code max size (lcd size local test)
	PostSetCscTypeDmaToFifo();
	LCD_InitBase2((CLK_SRC)LCLK_DOUT_MPLL, PNR_Parallel_RGB, CPU_16BIT);
	LCD_InitWinForFifoIn1(IN_POST, WIN0, LOCALIN_RGB);
	LCD_SetWinOnOff(1, WIN0);
	LCD_GetFrmSz(&uLcdHSz, &uLcdVSz, WIN0);
	LCD_Start();


	uImgHSz = uLcdHSz;
	uImgVSz = uLcdVSz;
		
	//uSrcBytesPerPixel = (eSrcDataFmt == RGB16) ? 2 : 4;
	uGlibStAddr = uLcdFbAddr + uLcdHSz*uLcdVSz*4;
	uSrcStAddr = uGlibStAddr + uLcdHSz*uLcdVSz*4;
			
	GLIB_Init(uGlibStAddr, uLcdHSz, uLcdVSz, RGB24);
//	GLIB_DrawPattern(uLcdHSz, uLcdVSz);	
	k=0;
		for(ui=0; ui<480; ui++)
			for(j=0; j<800; j++)
#if 0	// for code size decrease. delete by jungil . for test code. 080619
				GLIB_PutPixel2(j, ui, pEntertainment1[k++]);
#endif 
	POST_InitIpForDmaInDmaOut(uLcdHSz, uLcdVSz, uGlibStAddr, RGB24, uImgHSz, uImgVSz, uSrcStAddr, eSrcDataFmt, 1, false, ONE_SHOT, &oPost);
	POST_StartProcessing(&oPost);
	
	while (!POST_IsProcessingDone(&oPost));
	
	POST_InitIpForDmaInFifoOut(uImgHSz, uImgVSz, uSrcStAddr, eSrcDataFmt, uLcdHSz, uLcdVSz, RGB, 1, &oPost);
	POST_SetNextFrameStAddr(uSrcStAddr, 0, &oPost); // For Local Path
	POST_StartProcessing(&oPost);

	UART_Printf(" Hit any key to stop.\n");
	UART_Getc();
	
	POST_StopProcessingOfFreeRun(&oPost);	
	
	while (!POST_IsProcessingDone(&oPost));		
	
#else // For Test add jungil 080321 (osd local path test)

	UART_Printf("H Size Input = \n");
	uXsize = UART_GetIntNum();
	
	UART_Printf("V Size Input = \n");
	uYsize = UART_GetIntNum();


	*(volatile unsigned *)0x7410800c=0x0; // Modem I/F Bypass Control. It should be 0,
	*(volatile unsigned *)0x7e00f128=0x1; // F_BLOCK QOS Priority High Setting. add. jungil 080327
	
	LCD_InitDISPC(RGB24, uLcdFbAddr, WIN1, false);
	LCD_GetFrmSz(&uLcdHsz, &uLcdVsz, WIN1);	
	LCD_SetWinOnOff(1, WIN1);
	GLIB_InitInstance(uLcdFbAddr, uLcdHsz, uLcdVsz, RGB24);
	k=0;
	for(i=0; i<480; i++)
		for(j=0; j<800; j++)
			GLIB_PutPixel2(j, i, pSmile_3[k++]);
	LCD_GetFbEndAddr(&uLcdFbAddr2, WIN1);
	LCD_Start();
	UART_Printf("WIN1 Inage [DMA Mode] Display \n");
	UART_Getc();

	UART_Printf("\nHit any key to start OSD Local Path test[Post Path]!\n");
	UART_Getc();
	LCD_SetAllWinOnOff(0);

	GLIB_InitInstance(uLcdFbAddr2, uXXXSize, uYYYSize, RGB24);
	k=0;
	for(i=0; i<uYYYSize; i++){
		for(j=0; j<uXXXSize; j++){
				if ( uXXXSize == 720 )	GLIB_PutPixel2(j, i, pTestRGB24[k++]);
				else if ( uXXXSize == 640 )	GLIB_PutPixel2(j, i, p640480RGB24[k++]);		
				else if ( uXXXSize == 320 )	GLIB_PutPixel2(j, i, pKangjjang320240[k++]);
			}
		}

	LCD_InitWin(RGB24, uXsize, uYsize, 0, 0, uXsize, uYsize, 0, 0, uLcdFbAddr3, WIN0, false);
	LCD_InitWinForFifoIn2(IN_POST, WIN0, LOCALIN_RGB, uXsize, uYsize);

	LCD_SetBlendingType(PER_PLANE, ALPHA0_PLANE, WIN1);
	LCD_SetAlpha(0x0, 0x0, 0x0, 0x0, 0x0, 0x0, WIN1);

//	LCD_SetWinOnOff(1, WIN1);
	LCD_SetWinOnOff(1, WIN0);
	LCD_Start();

	uSRCWidth = uXXXSize;
	uSRCHeight = uYYYSize;
	uImgHSz = uXsize;
	uImgVSz = uYsize;
	uSrcStAddr = 0x52500000;
	eSrcDataFmt = eSrcDataFmt2;

	#if 0
	POST_InitIpForDmaInDmaOut(800, 400, uLcdFbAddr2, RGB24, uImgHSz, uImgVSz, uSrcStAddr, eSrcDataFmt, 1, false, ONE_SHOT, &oPost);
	POST_StartProcessing(&oPost);
	while (!POST_IsProcessingDone(&oPost));
	POST_InitIpForDmaInFifoOut(uImgHSz, uImgVSz, uSrcStAddr, eSrcDataFmt, 320, 240, RGB, 1, &oPost);	
	#else
	POST_InitIpForDmaInDmaOut(uSRCWidth, uSRCHeight, uLcdFbAddr2, RGB24, uSRCWidth, uSRCHeight, uSrcStAddr, eSrcDataFmt, 1, false, ONE_SHOT, &oPost);
	POST_StartProcessing(&oPost);
	while (!POST_IsProcessingDone(&oPost));
	POST_InitIpForDmaInFifoOut(uSRCWidth, uSRCHeight, uSrcStAddr, eSrcDataFmt, uImgHSz, uImgVSz, RGB, 1, &oPost);	
	#endif

	POST_SetNextFrameStAddr(uSrcStAddr, 0, &oPost); // For Local Path

	UART_Printf("Input Offset Value = ");
	i = UART_GetIntNum();
	UART_Printf("\n");

//	i=0;

	uBytesPerPixel = (eSrcDataFmt == YC420 ) ? 1:
					(eSrcDataFmt == RGB24 ) ? 4:2;

	//scaling down with line skip 

	uTemp_reg = Inp32( rPRESCALE_RATIO);			//modify prescale Vratio
	uTemp_reg = (uTemp_reg & ~(0x7f<<7)) |(((uTemp_reg>>7) & 0x7f)/(2<<i) <<7);
	Outp32( rPRESCALE_RATIO, uTemp_reg );

	uSRCHeight_Temp = uSRCHeight /(2<<i);
	uTemp_reg = Inp32( rSRCImgSize);				//modify SrcImgSize = original size -skip line
	uTemp_reg = (uTemp_reg & ~(0xfff<<12)) |(uSRCHeight_Temp<<12);
	Outp32( rSRCImgSize, uTemp_reg ); 

	uTemp_reg = Inp32( rPRESCALE_SHFACTOR);		//modify SHfactor
	Outp32( rPRESCALE_SHFACTOR, uTemp_reg+(i+1) ); 


	uOffset = uSRCWidth * ((2<<i)-1) * uBytesPerPixel; 	//modify End Address & Offset value

	if (eSrcDataFmt==YC420)					
	{
		uTemp_reg = Inp32( rADDRStart_CB);
		Outp32(rADDREnd_CB , uTemp_reg + uSRCWidth/2 * uSRCHeight_Temp/2 * uBytesPerPixel + uOffset/2*( uSRCHeight_Temp/2-1));
		uTemp_reg = Inp32( rNxtADDRStart_CB);
		Outp32(rNxtADDREnd_CB , uTemp_reg + uSRCWidth/2 * uSRCHeight_Temp/2 * uBytesPerPixel+ uOffset/2*( uSRCHeight_Temp/2-1));			
		Outp32( rOFFSET_CB, uOffset/2 ); 

		uTemp_reg = Inp32( rADDRStart_CR);
		Outp32(rADDREnd_CR , uTemp_reg + uSRCWidth/2 * uSRCHeight_Temp/2 * uBytesPerPixel+ uOffset/2*( uSRCHeight_Temp/2-1));			
		uTemp_reg = Inp32( rNxtADDRStart_CR);
		Outp32(rNxtADDREnd_CR , uTemp_reg + uSRCWidth/2 * uSRCHeight_Temp/2 * uBytesPerPixel+ uOffset/2*( uSRCHeight_Temp/2-1));			
		Outp32( rOFFSET_CR, uOffset/2 ); 
	}

	uTemp_reg = Inp32( rADDRStart_Y);
	Outp32(rADDREnd_Y , uTemp_reg + uSRCWidth * uSRCHeight_Temp * uBytesPerPixel + uOffset * (uSRCHeight_Temp-1));
	uTemp_reg = Inp32( rNxtADDRStart_Y);
	Outp32(rNxtADDREnd_Y , uTemp_reg + uSRCWidth * uSRCHeight_Temp * uBytesPerPixel + uOffset * (uSRCHeight_Temp-1));
	Outp32( rOFFSET_Y, uOffset ); 

	POST_StartProcessing(&oPost);
	while (!POST_IsProcessingDone(&oPost));

	UART_Printf(" Hit any key to stop.\n");
	UART_Getc();
	
	POST_StopProcessingOfFreeRun(&oPost);	
	
	while (!POST_IsProcessingDone(&oPost));		

#endif
}

// One-shot mode isn't supported in local path
// so, Only Free-Run mode is supported in local path
// In local path, scaling isn't supported because LCDC doesn't support that feature, but POST side is able to scale
static void PostLocalPath(void)
{
	u32 uGlibStAddr, uSrcStAddr;
	u32 i;
	u32 uLcdHsz, uLcdVsz;
	u32 uWIN1FbEndAddr;
	u32 ui,j,k;
	u32 uLcdFbAddr2;
	u32 uLcdFbAddr3;	
	u32 uXsize, uYsize;

	// add jungil 080331 underru test
	FRAME_INT_SEL eFIntSel;
	FIFO_INT_LEVEL eFifoIntLevel;
	FIFO_INT_SEL eFifoIntSel;

	
	UART_Printf("Post Simple Dma To Fifo Test.\n");

	POST_InitCh(POST_A, &oPost);

	uSimpleTest = 1;


#if 1 // original code max size (lcd size local test)
	PostSetCscTypeDmaToFifo();
	LCD_InitBase2((CLK_SRC)LCLK_DOUT_MPLL, PNR_Parallel_RGB, CPU_16BIT);
	LCD_InitWinForFifoIn1(IN_POST, WIN0, LOCALIN_RGB);
	LCD_SetWinOnOff(1, WIN0);
	LCD_GetFrmSz(&uLcdHSz, &uLcdVSz, WIN0);
	LCD_Start();

	uImgHSz = uLcdHSz;
	uImgVSz = uLcdVSz;
		
	//uSrcBytesPerPixel = (eSrcDataFmt == RGB16) ? 2 : 4;
	uGlibStAddr = uLcdFbAddr + uLcdHSz*uLcdVSz*4;
	uSrcStAddr = uGlibStAddr + uLcdHSz*uLcdVSz*4;
			
	GLIB_Init(uGlibStAddr, uLcdHSz, uLcdVSz, RGB24);
	GLIB_DrawPattern(uLcdHSz, uLcdVSz);	
	#if 0
	k=0;
		for(ui=0; ui<480; ui++)
			for(j=0; j<800; j++)
				GLIB_PutPixel2(j, ui, pEntertainment1[k++]);
	#endif
	
	POST_InitIpForDmaInDmaOut(uLcdHSz, uLcdVSz, uGlibStAddr, RGB24, uImgHSz, uImgVSz, uSrcStAddr, eSrcDataFmt, 1, false, ONE_SHOT, &oPost);
	POST_StartProcessing(&oPost);
	
	while (!POST_IsProcessingDone(&oPost));
	
	POST_InitIpForDmaInFifoOut(uImgHSz, uImgVSz, uSrcStAddr, eSrcDataFmt, uLcdHSz, uLcdVSz, RGB, 1, &oPost);
	POST_SetNextFrameStAddr(uSrcStAddr, 0, &oPost); // For Local Path
	POST_StartProcessing(&oPost);

	UART_Printf(" Hit any key to stop.\n");
	UART_Getc();
	
	POST_StopProcessingOfFreeRun(&oPost);	
	
	while (!POST_IsProcessingDone(&oPost));		
	
#else // For Test add jungil 080321 (osd local path test)

	UART_Printf("H Size Input = \n");
	uXsize = UART_GetIntNum();
	
	UART_Printf("V Size Input = \n");
	uYsize = UART_GetIntNum();


	*(volatile unsigned *)0x7410800c=0x0; // Modem I/F Bypass Control. It should be 0,
	*(volatile unsigned *)0x7e00f128=0x1; // F_BLOCK QOS Priority High Setting. add. jungil 080327
	
	LCD_InitDISPC(RGB24, uLcdFbAddr, WIN1, false);
	LCD_GetFrmSz(&uLcdHsz, &uLcdVsz, WIN1);	
	LCD_SetWinOnOff(1, WIN1);
	GLIB_InitInstance(uLcdFbAddr, uLcdHsz, uLcdVsz, RGB24);
	k=0;
	for(i=0; i<480; i++)
		for(j=0; j<800; j++)
			GLIB_PutPixel2(j, i, pSmile_3[k++]);
	LCD_GetFbEndAddr(&uLcdFbAddr2, WIN1);
	LCD_Start();
	UART_Printf("WIN1 Inage [DMA Mode] Display \n");
	UART_Getc();

	UART_Printf("\nHit any key to start OSD Local Path test[Post Path]!\n");
	UART_Getc();
	LCD_SetAllWinOnOff(0);

	GLIB_InitInstance(uLcdFbAddr2, uXXXSize, uYYYSize, RGB24);
	k=0;
	for(i=0; i<uYYYSize; i++){
		for(j=0; j<uXXXSize; j++)
			{
				if ( uXXXSize == 720 )	GLIB_PutPixel2(j, i, pTestRGB24[k++]);
				else if ( uXXXSize == 640 )	GLIB_PutPixel2(j, i, p640480RGB24[k++]);		
				else if ( uXXXSize == 320 )	GLIB_PutPixel2(j, i, pKangjjang320240[k++]);
//			GLIB_PutPixel2(j, i, p320176RGB16[k++]);		
			}
		}

	LCD_InitWin(RGB24, uXsize, uYsize, 0, 0, uXsize, uYsize, 0, 0, uLcdFbAddr3, WIN0, false);
	LCD_InitWinForFifoIn2(IN_POST, WIN0, LOCALIN_RGB, uXsize, uYsize);

	LCD_SetBlendingType(PER_PLANE, ALPHA0_PLANE, WIN1);
	LCD_SetAlpha(0x0, 0x0, 0x0, 0x0, 0x0, 0x0, WIN1);

//	LCD_SetWinOnOff(1, WIN1);
	LCD_SetWinOnOff(1, WIN0);
	LCD_Start();


	uImgHSz = uXsize;
	uImgVSz = uYsize;
	uSrcStAddr = 0x52500000;
	eSrcDataFmt = eSrcDataFmt2;

	#if 0
	POST_InitIpForDmaInDmaOut(800, 400, uLcdFbAddr2, RGB24, uImgHSz, uImgVSz, uSrcStAddr, eSrcDataFmt, 1, false, ONE_SHOT, &oPost);
	POST_StartProcessing(&oPost);
	while (!POST_IsProcessingDone(&oPost));
	POST_InitIpForDmaInFifoOut(uImgHSz, uImgVSz, uSrcStAddr, eSrcDataFmt, 320, 240, RGB, 1, &oPost);	
	#else
	POST_InitIpForDmaInDmaOut(uXXXSize, uYYYSize, uLcdFbAddr2, RGB24, uXXXSize, uYYYSize, uSrcStAddr, eSrcDataFmt, 1, false, ONE_SHOT, &oPost);
	POST_StartProcessing(&oPost);
	while (!POST_IsProcessingDone(&oPost));
	POST_InitIpForDmaInFifoOut(uXXXSize, uYYYSize, uSrcStAddr, eSrcDataFmt, uImgHSz, uImgVSz, RGB, 1, &oPost);	
	#endif

	POST_SetNextFrameStAddr(uSrcStAddr, 0, &oPost); // For Local Path
	POST_StartProcessing(&oPost);


#if 0 // Under run Test add. jungil 080332
			INTC_SetVectAddr(NUM_LCD0, Isr_LCD_FifoInt);

			UART_Printf("\nIf you want to stop FIFO Int., press any key\n");

			 eFifoIntSel=
				(eBgWin == WIN0) ? FIFO_INT_WIN0 :
				(eBgWin == WIN1) ? FIFO_INT_WIN1 :
				(eBgWin == WIN2) ? FIFO_INT_WIN2 :
				(eBgWin == WIN3) ? FIFO_INT_WIN3 : FIFO_INT_WIN4;

				uCount=0;
				
			       eFifoIntLevel = FIFO_EMPTY ;
			       
				LCD_InitInt(LCD_FIFO_INT, eLcdCpuifInt, eFifoIntLevel, eFifoIntSel, FINT_BACKPORCH);
				LCD_EnableInt();
			
				INTC_Enable(NUM_LCD0);

				LCD_Start();
			
				while(GetKey() == 0)
				{}
				
				INTC_Disable(NUM_LCD0);
				LCD_DisableInt();

				UART_Printf("\n");

#endif





	UART_Printf(" Hit any key to stop.\n");
	UART_Getc();
	
	POST_StopProcessingOfFreeRun(&oPost);	
	
	while (!POST_IsProcessingDone(&oPost));		

#endif
}


static void PostLocalPath_Test(void)
{
	u32 uGlibStAddr, uSrcStAddr;
	u32 i;
	u32 uLcdHsz, uLcdVsz;
	u32 uWIN1FbEndAddr;
	u32 ui,j,k;
	u32 uLcdFbAddr2;
	u32 uLcdFbAddr3;	
	u32 uXsize, uYsize;
	u32 uOFFSetX=0, uOFFSetY=0;
	
	// add jungil 080331 underru test
	FRAME_INT_SEL eFIntSel;
	FIFO_INT_LEVEL eFifoIntLevel;
	FIFO_INT_SEL eFifoIntSel;

	
	UART_Printf("Post Simple Dma To Fifo Test.\n");

	POST_InitCh(POST_A, &oPost);
	uSimpleTest = 1;

	UART_Printf("H Size Input = \n");
	uXsize = UART_GetIntNum();
	UART_Printf("V Size Input = \n");
	uYsize = UART_GetIntNum();

	*(volatile unsigned *)0x7410800c=0x0; // Modem I/F Bypass Control. It should be 0,
	*(volatile unsigned *)0x7e00f128=0x1; // F_BLOCK QOS Priority High Setting. add. jungil 080327

	// 01. Interrunpt Enable.
	// EINR 9 Setting
	INTC_SetVectAddr(NUM_EINT1, Isr_EINT9_Setting);
	GPIO_SetPullUpDownEach(eGPIO_N, eGPIO_9, 0x0);  // Pull Down Disable
	Delay(100);
	GPIO_SetEint0(9, Falling_Edge, eDLYFLT, 0);  			// EVT1 OK
	GPIO_EINT0ClrPend(9);
	GPIO_EINT0DisMask(9);
	// LCD VsYNC Setting
	INTC_SetVectAddr(NUM_LCD1, Isr_LCD_FrameCount2);
	LCD_InitInt(LCD_FRAME_INT, eLcdCpuifInt, FIFO_25, FIFO_INT_WIN0, FINT_VSYNC);
	LCD_EnableInt();
	INTC_Enable(NUM_EINT1);
//	INTC_Enable(NUM_LCD1);

	// 02 . Basic Display Settng Win 1, Win0
	LCD_InitDISPC(RGB24, uLcdFbAddr, WIN1, false);
	LCD_GetFrmSz(&uLcdHsz, &uLcdVsz, WIN1);	
	LCD_SetWinOnOff(1, WIN1);
	GLIB_InitInstance(uLcdFbAddr, uLcdHsz, uLcdVsz, RGB24);
	k=0;
	for(i=0; i<480; i++)
		for(j=0; j<800; j++)
			GLIB_PutPixel2(j, i, pSmile_3[k++]);
	LCD_GetFbEndAddr(&uLcdFbAddr2, WIN1);
	LCD_Start();
	UART_Printf("WIN1 Inage [DMA Mode] Display \n");
	UART_Getc();

	UART_Printf("\nHit any key to start OSD Local Path test[Post Path]!\n");
	UART_Getc();
	LCD_SetAllWinOnOff(0);

	GLIB_InitInstance(uLcdFbAddr2, uXXXSize, uYYYSize, RGB24);
	k=0;
	for(i=0; i<uYYYSize; i++){
		for(j=0; j<uXXXSize; j++)
			{
#if 0 // for decrease code size delete. for test code. jungil 080619
				if ( uXXXSize == 720 )	GLIB_PutPixel2(j, i, pTestRGB24[k++]);
				else if ( uXXXSize == 640 )	GLIB_PutPixel2(j, i, p640480RGB24[k++]);		
				else if ( uXXXSize == 320 )	GLIB_PutPixel2(j, i, pKangjjang320240[k++]);
//			GLIB_PutPixel2(j, i, p320176RGB16[k++]);		
#endif
			}
		}

	LCD_InitWin(RGB24, uXsize, uYsize, 0, 0, uXsize, uYsize, 0, 0, uLcdFbAddr3, WIN0, false);
	LCD_InitWinForFifoIn2(IN_POST, WIN0, LOCALIN_RGB, uXsize, uYsize);

	LCD_SetBlendingType(PER_PLANE, ALPHA0_PLANE, WIN1);
	LCD_SetAlpha(0x0, 0x0, 0x0, 0x0, 0x0, 0x0, WIN1);

	LCD_SetWinOnOff(1, WIN1);
	LCD_SetWinOnOff(1, WIN0);
	LCD_Start();

	uImgHSz = uXsize;
	uImgVSz = uYsize;
	uSrcStAddr = 0x52500000;
	eSrcDataFmt = eSrcDataFmt2;

	#if 0
	POST_InitIpForDmaInDmaOut(800, 400, uLcdFbAddr2, RGB24, uImgHSz, uImgVSz, uSrcStAddr, eSrcDataFmt, 1, false, ONE_SHOT, &oPost);
	POST_StartProcessing(&oPost);
	while (!POST_IsProcessingDone(&oPost));
	POST_InitIpForDmaInFifoOut(uImgHSz, uImgVSz, uSrcStAddr, eSrcDataFmt, 320, 240, RGB, 1, &oPost);	
	#else
	POST_InitIpForDmaInDmaOut(uXXXSize, uYYYSize, uLcdFbAddr2, RGB24, uXXXSize, uYYYSize, uSrcStAddr, eSrcDataFmt, 1, false, ONE_SHOT, &oPost);
	POST_StartProcessing(&oPost);
	while (!POST_IsProcessingDone(&oPost));
	POST_InitIpForDmaInFifoOut(uXXXSize, uYYYSize, uSrcStAddr, eSrcDataFmt, uImgHSz, uImgVSz, RGB, 1, &oPost);	
	#endif

	POST_SetNextFrameStAddr(uSrcStAddr, 0, &oPost); // For Local Path
	POST_StartProcessing(&oPost);

	//03. Wait EINT 9 Interrupt
	while (1) 
	{
		if ( uEINT9IRQ == 1 ) 
		{
//			LCD_Stop();
//			uPOSTEND = 1; 
//			if ( ((*(unsigned int*)0x77100004)>>13)&0x3 == 0x0 )
//			{
//				POST_StopProcessingOfFreeRun(&oPost);	
//				while (!POST_IsProcessingDone(&oPost));		
//				LCD_SetWinOnOff(0, WIN0); // window 0 is off	
//				LCD_SetAllWinOnOff(0);
//				LCD_SetWinOnOff(0, WIN0);
//				LCD_Stop();
				POST_InitIpForDmaInFifoOut(uXXXSize, uYYYSize, uSrcStAddr, eSrcDataFmt, uImgHSz, uImgVSz, RGB, 1, &oPost);		
				POST_SetNextFrameStAddr(uSrcStAddr, 0, &oPost); // For Local Path
				POST_StartProcessing(&oPost);
				LCD_InitWin(RGB24, uXsize, uYsize, 0, 0, uXsize, uYsize, uOFFSetX, uOFFSetY, uLcdFbAddr3, WIN0, false);
//				LCD_InitWinForFifoIn2(IN_POST, WIN0, LOCALIN_RGB, uXsize, uYsize);
//				LCD_SetBlendingType(PER_PLANE, ALPHA0_PLANE, WIN1);
//				LCD_SetAlpha(0x0, 0x0, 0x0, 0x0, 0x0, 0x0, WIN1);
	//			uPOSTSTART == 1; 

//				LCD_SetWinOnOff(1, WIN1);
//				LCD_SetWinOnOff(1, WIN0);
//				LCD_Start();
//				break;
			uOFFSetX = uOFFSetX + 2;
			uOFFSetY = uOFFSetY + 2;
			uEINT9IRQ = 0 ;
			if ( uOFFSetX == 200)
			{
				uOFFSetX = 0 ;
				uOFFSetY = 0;
				
			}
				
		}

			
	}
	
	UART_Printf(" Hit any key to stop.\n");
	UART_Getc();

//	}
	INTC_Disable(NUM_LCD1);
	LCD_DisableInt();
	
	POST_StopProcessingOfFreeRun(&oPost);	
	
	while (!POST_IsProcessingDone(&oPost));		

	LCD_SetAllWinOnOff(0);
	LCD_Stop();


			uEINT9IRQ == 0 ;
}

static void PostLocalPath_Test2(void)
{
	u32 uGlibStAddr, uSrcStAddr;
	u32 i;
	u32 uLcdHsz, uLcdVsz;
	u32 uWIN1FbEndAddr;
	u32 ui,j,k;
	u32 uLcdFbAddr2;
	u32 uLcdFbAddr3;	
	u32 uXsize, uYsize;
	u32 uOFFSetX=0, uOFFSetY=0;
	
	// add jungil 080331 underru test
	FRAME_INT_SEL eFIntSel;
	FIFO_INT_LEVEL eFifoIntLevel;
	FIFO_INT_SEL eFifoIntSel;

	
	UART_Printf("Post Simple Dma To Fifo Test.\n");

	POST_InitCh(POST_A, &oPost);
	uSimpleTest = 1;

	UART_Printf("H Size Input = \n");
	uXsize = UART_GetIntNum();
	UART_Printf("V Size Input = \n");
	uYsize = UART_GetIntNum();

	*(volatile unsigned *)0x7410800c=0x0; // Modem I/F Bypass Control. It should be 0,
	*(volatile unsigned *)0x7e00f128=0x1; // F_BLOCK QOS Priority High Setting. add. jungil 080327

	// 01. Interrunpt Enable.
	// EINR 9 Setting
	INTC_SetVectAddr(NUM_EINT1, Isr_EINT9_Setting_2);
	GPIO_SetPullUpDownEach(eGPIO_N, eGPIO_9, 0x0);  // Pull Down Disable
//	INTC_SetVectAddr(NUM_EINT1, Isr_EINT10_Setting_2);
//	GPIO_SetPullUpDownEach(eGPIO_N, eGPIO_10, 0x0);  // Pull Down Disable

	Delay(100);
	GPIO_SetEint0(9, Falling_Edge, eDLYFLT, 0);  			// EVT1 OK
	GPIO_EINT0ClrPend(9);
	GPIO_EINT0DisMask(9);

//	GPIO_SetEint0(10, Falling_Edge, eDLYFLT, 0);  			// EVT1 OK
//	GPIO_EINT0ClrPend(10);
//	GPIO_EINT0DisMask(10);

	// LCD VsYNC Setting
	INTC_SetVectAddr(NUM_LCD1, Isr_LCD_FrameCount3);
	LCD_InitInt(LCD_FRAME_INT, eLcdCpuifInt, FIFO_25, FIFO_INT_WIN0, FINT_BACKPORCH);
	LCD_EnableInt();
	INTC_SetVectAddr(NUM_POST0, Isr_POST_Done);
	POST_EnableInterrupt(POST_LEVEL_INT, &oPost);
	INTC_Enable(NUM_EINT1);
	INTC_Enable(NUM_LCD1);
	INTC_Enable(NUM_POST0);

	// 02 . Basic Display Settng Win 1, Win0
	LCD_InitDISPC(RGB24, uLcdFbAddr, WIN1, false);
	LCD_GetFrmSz(&uLcdHsz, &uLcdVsz, WIN1);	
	LCD_SetWinOnOff(1, WIN1);
	GLIB_InitInstance(uLcdFbAddr, uLcdHsz, uLcdVsz, RGB24);
	k=0;
	for(i=0; i<480; i++)
		for(j=0; j<800; j++)
			GLIB_PutPixel2(j, i, pSmile_3[k++]);
	LCD_GetFbEndAddr(&uLcdFbAddr2, WIN1);

	LCD_Start();

	UART_Printf("\nHit any key to start OSD Local Path test[Post Path]!\n");
	UART_Getc();
	LCD_SetAllWinOnOff(0);

	GLIB_InitInstance(uLcdFbAddr2, uXXXSize, uYYYSize, RGB24);
	k=0;
	for(i=0; i<uYYYSize; i++){
		for(j=0; j<uXXXSize; j++)
			{
#if 0 // for decrese code size. for test code . jungil 080619
				if ( uXXXSize == 720 )	GLIB_PutPixel2(j, i, pTestRGB24[k++]);
				else if ( uXXXSize == 640 )	GLIB_PutPixel2(j, i, p640480RGB24[k++]);		
				else if ( uXXXSize == 320 )	GLIB_PutPixel2(j, i, pKangjjang320240[k++]);
//			GLIB_PutPixel2(j, i, p320176RGB16[k++]);		
#endif
			}
		}

	LCD_InitWin(RGB24, uXsize, uYsize, 0, 0, uXsize, uYsize, 0, 0, uLcdFbAddr3, WIN0, false);
	LCD_InitWinForFifoIn2(IN_POST, WIN0, LOCALIN_RGB, uXsize, uYsize);

	LCD_SetBlendingType(PER_PLANE, ALPHA0_PLANE, WIN1);
	LCD_SetAlpha(0x0, 0x0, 0x0, 0x0, 0x0, 0x0, WIN1);

	LCD_SetWinOnOff(1, WIN1);
	LCD_SetWinOnOff(1, WIN0);
	LCD_Start();

	uImgHSz = uXsize;
	uImgVSz = uYsize;
	uSrcStAddr = 0x52500000;
	eSrcDataFmt = eSrcDataFmt2;

	#if 0
	POST_InitIpForDmaInDmaOut(800, 400, uLcdFbAddr2, RGB24, uImgHSz, uImgVSz, uSrcStAddr, eSrcDataFmt, 1, false, ONE_SHOT, &oPost);
	POST_StartProcessing(&oPost);
	while (!POST_IsProcessingDone(&oPost));
	POST_InitIpForDmaInFifoOut(uImgHSz, uImgVSz, uSrcStAddr, eSrcDataFmt, 320, 240, RGB, 1, &oPost);	
	#else
	POST_InitIpForDmaInDmaOut(uXXXSize, uYYYSize, uLcdFbAddr2, RGB24, uXXXSize, uYYYSize, uSrcStAddr, eSrcDataFmt, 1, false, ONE_SHOT, &oPost);
	POST_StartProcessing(&oPost);
	while (!POST_IsProcessingDone(&oPost));
	POST_InitIpForDmaInFifoOut(uXXXSize, uYYYSize, uSrcStAddr, eSrcDataFmt, uImgHSz, uImgVSz, RGB, 1, &oPost);	
	#endif
	POST_SetNextFrameStAddr(uSrcStAddr, 0, &oPost); // For Local Path
	POST_StartProcessing(&oPost);

	//03. Wait EINT 9 Interrupt
	while (1) 
	{
		if ( (uEINT9IRQ == 1 ) & ( uPOSTDoneIssue == 1) & ( WinStopIssue == 1 ) ) break;
	}
	
	UART_Printf(" Hit any key \n");
	UART_Getc();

	uPOSTDoneIssue = 0;
	WinStopIssue = 0 ;
	uEINT9IRQ =0;

#if 0 
	POST_InitIpForDmaInFifoOut(uXXXSize, uYYYSize, uSrcStAddr, eSrcDataFmt, uImgHSz, uImgVSz, RGB, 1, &oPost);	
	POST_SetNextFrameStAddr(uSrcStAddr, 0, &oPost); // For Local Path
	POST_StartProcessing(&oPost);
#else
	*(unsigned int*)0x77000000 = *(unsigned int*)0x77000000 | (0x1<<14); // autoload Enable.
//	POST_SetNextFrameStAddr(uSrcStAddr, 0, &oPost); // For Local Path
	POST_StartProcessing(&oPost);	
#endif	

	LCD_SetWinOnOff(1,WIN0);

	UART_Printf(" Hit any key to stop.\n");
	UART_Getc();

	

	INTC_Disable(NUM_LCD1);
	INTC_Disable(NUM_EINT1);
	INTC_Disable(NUM_POST0);
	LCD_DisableInt();

	POST_DisableInterrupt(&oPost);
	LCD_SetAllWinOnOff(0);
	LCD_Stop();

	uEINT9IRQ = 0 ;
	uPOSTDoneIssue = 0; 
	WinStopIssue = 0; 
}


static void Interlace(void)
{
	u32 uHSz, uVSz;//, uNofPixel;
	u32 uColor[2];
	u32 i, j;
	
	bIsRev = false;
	bIsAllWin = false;

	#if (LCD_MODULE_TYPE== LTV350QV_RGB)
		UART_Printf("Selected LCD Module Type: LTV350QV_RGB\n");
		uHSz=320;
		uVSz=240;
	
	#elif (LCD_MODULE_TYPE == LTS222QV_CPU)
		UART_Printf("Selected LCD Module Type: LTS222QV_CPU\n");
		uHSz=240;
		uVSz=320;

	#elif (LCD_MODULE_TYPE == LTP700WV_RGB)
		UART_Printf("Selected LCD Module Type: LTP700WV_RGB\n");
		uHSz=800;
		uVSz=480;	

	#elif (LCD_MODULE_TYPE == LTE480WV_RGB)
		UART_Printf("Selected LCD Module Type: LTE480WV_RGB\n");
		uHSz=800;
		uVSz=480;		

	#elif (LCD_MODULE_TYPE == LTV300GV_RGB)
		UART_Printf("Selected LCD Module Type: LTV300GV_RGB\n");
		uHSz=640;
		uVSz=480;	
	
	#else
		Assert(0);
	#endif

	uColor[0] = 0xff<<0; 			// Blue
	uColor[1] = 0xff<<8; 			// Green

	LCD_InitBase2((CLK_SRC)LCLK_DOUT_MPLL, PNR_Parallel_RGB, CPU_16BIT);
	LCD_InitWin(RGB24, uHSz, uVSz, 0, 0, uHSz, uVSz, 0, 0, uLcdFbAddr, eBgWin, false);
	
	LCD_GetFrmSz(&uHSz,  &uVSz, eBgWin);
	LCD_SetWinOnOff(1, eBgWin);

	//uNofPixel = uHSz*uVSz;

	for(j=0; j <uVSz; j=j+2)
	{
		for (i=0; i <uHSz; i++)
			*(u32*)(uLcdFbAddr+uHSz*j*4+i*4) = uColor[0];	
		for (i=0; i <uHSz; i++)
			*(u32*)(uLcdFbAddr+uHSz*(j+1)*4+i*4) = uColor[1];	
	}
			
	LCD_Start();
	
	UART_Printf("Press any key to run interlaced mode.\n");
	UART_Getc();
	
	LCD_SetScanMode(INTERLACE_MODE);

	UART_Printf("Press any key to run progressive mode.\n");
	UART_Getc();
	
	LCD_SetScanMode(PROGRESSIVE_MODE);
	
}


static void EintAliasing(void)
{
	GPIO_Init();

	INTC_SetVectAddr(NUM_EINT1, Isr_ExtInt9);
	//INTC_SetVectAddr(NUM_EINT1, Isr_Eint10);
	//INTC_SetVectAddr(NUM_EINT1, Isr_Eint11);

	GPIO_SetPullUpDownEach(eGPIO_N, eGPIO_11, 0x0);  	// Pull Down Disable
	GPIO_SetPullUpDownEach(eGPIO_N, eGPIO_10, 0x0);		// Pull Down Disable
	GPIO_SetEint0(9, Falling_Edge, eDLYFLT, 0);
	//GPIO_SetEint0(10, Rising_Edge, eDIGFLT, 0x1F);
	//GPIO_SetEint0(11, Rising_Edge, eDIGFLT, 0x1F);	

	GPIO_EINT0ClrPend(9);
	GPIO_EINT0ClrPend(10);
	GPIO_EINT0ClrPend(11);

	GPIO_EINT0DisMask(9);
	//GPIO_EINT0DisMask(10);
	//GPIO_EINT0DisMask(11);

	INTC_Enable(NUM_EINT1);

	//Set LCD GPIO Port
	GPIO_SetFunctionAll(eGPIO_I, 0xaaaaaaaa, 2); 	//GPI[15..0]-> RGB VD[15..0]
	GPIO_SetFunctionAll(eGPIO_J, 0xaaaaaaaa, 2); 	//GPJ[7..0]-> RGB VD[23..16], GPJ[11..8]-> VCLK, VDEN, VSYNC, HSYNC

	UART_Printf("Press Ext Int SW9\n");
	UART_Printf("Press any key to exit\n");

	while(GetKey() == 0)
	{}
	
}

static void GPIOMuxing(void)
{
	u32 i;

	for(i=0; i<16; i++)
	{
		UART_Printf("\nGPI %d Port Test.\n", i);
	
		GPIO_SetFunctionAll(eGPIO_I, 0x00000000, 0); //Input	
		GPIO_SetPullUpDownAll(eGPIO_I, 0x55555555);

		GPIO_SetFunctionEach(eGPIO_I, (GPIO_eBitPos)i, 1); //Output

		UART_Printf("\n Press enter key to exit GPIO toggling\n");
	
		while(GetKey()==0)
		{
			GPIO_SetDataEach(eGPIO_I, (GPIO_eBitPos)i , 1); //High
			Delay(10);	
			GPIO_SetDataEach(eGPIO_I, (GPIO_eBitPos)i , 0); //Low
			Delay(10);	
		}
	}

	for(i=0; i<16; i++)
	{
		UART_Printf("\nGPJ %d Port Test.\n", i);
	
		GPIO_SetFunctionAll(eGPIO_J, 0x00000000, 0); //Input			

		GPIO_SetFunctionEach(eGPIO_J, (GPIO_eBitPos)i, 1); //Output

		UART_Printf("\n Press enter key to exit GPIO toggling\n");
	
		while(GetKey()==0)
		{
			GPIO_SetDataEach(eGPIO_J, (GPIO_eBitPos)i , 1); //High
			Delay(10);	
			GPIO_SetDataEach(eGPIO_J, (GPIO_eBitPos)i , 0); //Low
			Delay(10);	
		}
	}

}

static void VgaDis(void)
{
	u32 uHSz, uVSz;
	u32 i, j, k;
	
	bIsRev = false;
	bIsAllWin = false;
	
	UART_Printf("Enter Any Key To Test Simple Display\n");
	UART_Printf("Include VGA Image File Header \n");
	UART_Getc();

	LCD_InitDISPC(RGB24, uLcdFbAddr, WIN0, false);
	LCD_GetFrmSz(&uHSz, &uVSz, WIN0);
	GLIB_InitInstance(uLcdFbAddr, uHSz, uVSz, RGB24);
	
	k=0;
	for(i=0; i<uVSz; i++)
		for(j=0; j<uHSz; j++)
//			GLIB_PutPixel2(j, i, autumn[k++]);

	LCD_SetWinOnOff(1, WIN0);
	LCD_Start();
	
	//GLIB_DrawPattern2(uHSz, uVSz);
	
	UART_Printf("Enter Any Key To Exit.\n");
	UART_Getc();
}

/*
static void YUV422Out(void)
{
	u32 uHSz, uVSz;
	
	bIsRev = false;
	bIsAllWin = false;

	//Select LCD IF Pin Configure
	GPIO_SetLCDType(eYCbCr);
		
	//Set LCD GPIO Port
	GPIO_SetFunctionAll(eGPIO_I, 0xaaaaaaaa, 2);
	GPIO_SetFunctionAll(eGPIO_J, 0xaaaaaaaa, 2);
	
	UART_Printf("Enter any key to test simple display\n");
	UART_Getc();

	LCD_InitDISPC(eBgBpp, uLcdFbAddr, eBgWin, false);
	LCD_SetWinOnOff(1, eBgWin);

	LCD_Start();

	UART_Printf("Window %d (background) is displayed\n", (int)eBgWin);
	LCD_GetFrmSz(&uHSz, &uVSz, eBgWin);
	GLIB_InitInstance(uLcdFbAddr, uHSz, uVSz, eBgBpp);
	GLIB_DrawPattern(uHSz, uVSz);
	GLIB_PutLine(0, 0, 0, uVSz-1, C_BLUE);
	
	UART_Printf("Enter any key to exit");
	UART_Getc();
	
}
*/


// CPU I/F
static void DisBasicCpu(void)
{
	u32 uFgAddr;
	u32 uHSz, uVSz;	
	bIsRev = false;
	bIsAllWin = false;

	//LCD_SetPort();	
	//LCD_InitLDI(LCD_CPUIF_CS);

	LCD_InitDISPC(eBgBpp, uLcdFbAddr, eBgWin, false);
	LCD_SetWinOnOff(1, eBgWin);

	LCD_Start();	
	UART_Printf("Window %d (Background) Is Displayed\n", (int)eBgWin);
	LCD_GetFrmSz(&uHSz, &uVSz, eBgWin);
	GLIB_InitInstance(uLcdFbAddr, uHSz, uVSz, eBgBpp);
	GLIB_DrawPattern(uHSz, uVSz);
	GLIB_PutLine(0, 0, 0, uVSz-1, C_BLUE);
	LCD_Trigger();

	////////////////////////////////////////////////////////////////////
	UART_Printf("Enter Any Key To Display Window %d (Foreground)\n", (int)eFgWin);
	UART_Getc();

	LCD_GetFbEndAddr(&uFgAddr, eBgWin);
	
	LCD_InitWin(eFgBpp, uHSz/2, uVSz/2, 0, 0, uHSz/2, uVSz/2, 60, 80, uFgAddr, eFgWin, false);
	LCD_SetWinOnOff(1, eFgWin);

	UART_Printf("Window %d (Foreground) Is Displayed\n", (int)eFgWin);
	LCD_GetFrmSz(&uHSz, &uVSz, eFgWin);
	GLIB_InitInstance(uFgAddr, uHSz, uVSz, eFgBpp);
	GLIB_DrawPattern2(uHSz, uVSz);
	GLIB_PutLine(0, 0, 0, uVSz-1, C_BLUE);	

	LCD_Trigger();

	UART_Printf("Enter Any Key To Test Case Which Img. Size Is Smaller\n");
	UART_Printf("Than LCD Size In One Window\n");
	UART_Getc();

	// Test case in which img. size is smaller than LCD size in one window.
	LCD_SetAllWinOnOff(0);
	LCD_InitWin(eBgBpp, uHSz, uVSz, 0, 0, uHSz, uVSz, 60, 80, uLcdFbAddr, eBgWin, false);

	LCD_SetWinOnOff(1, eBgWin);
	
	GLIB_InitInstance(uLcdFbAddr, uHSz, uVSz, eBgBpp);
	GLIB_DrawPattern(uHSz, uVSz);
	LCD_Trigger();

	UART_Printf("\nIf you want to end this test, press any key\n");
	UART_Getc();

	LCD_Stop();
	LCD_SetAllWinOnOff(0);
}



static void PartialDis(void)
{
	u32 uHSz, uVSz;
	u32 uBytes;
	u32 uGlibStAddr;
	u32 i;
	
	bIsRev = false;
	bIsAllWin = false;
	
	#if (LCD_MODULE_TYPE== LTV350QV_RGB)
		UART_Printf("Selected LCD Module Type: LTV350QV_RGB\n");
		uHSz=320;
		uVSz=240;
	
	#elif (LCD_MODULE_TYPE == LTS222QV_CPU)
		UART_Printf("Selected LCD Module Type: LTS222QV_CPU\n");
		uHSz=240;
		uVSz=320;

	#elif (LCD_MODULE_TYPE == LTE480WV_RGB)
		UART_Printf("Selected LCD Module Type: LTE480WV_RGB\n");
		uHSz=800;
		uVSz=480;	

	#elif (LCD_MODULE_TYPE == LTP700WV_RGB)
		UART_Printf("Selected LCD Module Type: LTP700WV_RGB\n");
		uHSz=800;
		uVSz=480;		

	#elif (LCD_MODULE_TYPE == LTS222QV_SRGB)	
		UART_Printf("Selected LCD Module Type: LTS222QV_SRGB\n");
		uHSz=240;
		uVSz=320;
		
	#else
		Assert(0);
	#endif

	for (i = 0; i<480*800; i++)
		*(u8 *)(uLcdFbAddr+i) = 0;
	LCD_InitLDI((CPUIF_LDI)LCD_CPUIF_CS);
	
	
	LCD_InitBase(); 
	LCD_InitWin(eBgBpp, uHSz, uVSz, 0, 0, uHSz, uVSz, 0, 0, uLcdFbAddr, eBgWin, false);
	LCD_SetWinOnOff(1, eBgWin);
	LCD_GetFrmSz(&uHSz, &uVSz, eBgWin);

	LCD_Start();
	
	GLIB_Init(uLcdFbAddr, uHSz, uVSz, eBgBpp);
	GLIB_DrawPattern2(uHSz, uVSz);

	LCD_Trigger();

	uBytes = (eBgBpp == RGB24) ? 4: 2;
	uGlibStAddr = uLcdFbAddr + uHSz*uVSz*uBytes;
	
	GLIB_Init(uGlibStAddr, uHSz, 120, eBgBpp);
	GLIB_DrawPattern(uHSz, 120);

	GLIB_SetFontColor(C_BLACK, C_WHITE, false);
	GLIB_Printf(30, 20, "Partial Display Area.");
	
	UART_Printf("Enter Any Key To Display Partial Area\n");
	UART_Getc();

	//In LCD_LTS222Q_CPU type LDI, Coordinate X must be 0 and Partial HSz must be same to LCD HSz
	LCD_DisplayPartialArea(0, 0, 0, 0, 240, 60, uGlibStAddr, eBgWin);
	LCD_Trigger();	

	UART_Printf("\nIf you want to end this test, press any key\n");
	UART_Getc();

	LCD_Stop();
	LCD_SetAllWinOnOff(0);
}


static void AutoNormalManualCmd(void)
{	
	u32 uHSz, uVSz;
	
	bIsRev = false;
	bIsAllWin = false;

	LCD_InitLDI((CPUIF_LDI)LCD_CPUIF_CS);
	LCD_InitDISPC(eBgBpp, uLcdFbAddr, eBgWin, false);
	LCD_SetWinOnOff(1, eBgWin);
	LCD_GetFrmSz(&uHSz, &uVSz, eBgWin);

	LCD_Start();

	INTC_SetVectAddr(NUM_LCD2, Isr_LCD_I80Int);
	
	GLIB_Init(uLcdFbAddr, uHSz, uVSz, eBgBpp);
	GLIB_DrawPattern(uHSz, uVSz);

	UART_Printf("Auto Command Mode.\n");
	 eCpuCmdMode = LCD_AUTO_CMD;

	// LDI (X, Y) coordinate setting
	uCmdVal[0] = 0x4200;  //R66: X Addr Reg. 
	uCmdVal[1] = 0x4300;  //R67: Y Addr Reg.1
	uCmdVal[2] = 0x4400;  //R68: Y Addr Reg.2
	
	uCmdVal[3] = 0x4200;  
	uCmdVal[4] = 0x4300; 
	uCmdVal[5] = 0x4400;	

	uCmdVal[6] = 0x4200;  
	uCmdVal[7] = 0x4300;  
	uCmdVal[8] = 0x4400; 

	uCmdVal[9] = 0x4200;  
	uCmdVal[10] = 0x4300;  
	uCmdVal[11] = 0x4400;

	LCD_PutCmdToLdi_AutoCmd(uCmdVal, 12, (CPUIF_LDI)LCD_CPUIF_CS);

	LCD_InitInt(LCD_SYSIF_INT, eLcdCpuifInt, FIFO_25, FIFO_INT_WIN0, FINT_VSYNC);
	LCD_EnableInt();
	INTC_Enable(NUM_LCD2);

	eCpuAutoCmdRate = PER_TWO_FRM;
	LCD_SetAutoCmdRate(eCpuAutoCmdRate, (CPUIF_LDI)LCD_CPUIF_CS);

	LCD_Trigger();

	uCount =0;

	while(1)
	{
		if (uCount == ((u32)eCpuAutoCmdRate*8 + 1))
		break;
	}

	INTC_Disable(NUM_LCD2);
	LCD_DisableInt();

	LCD_SetAutoCmdRate(DISABLE_AUTO_FRM, (CPUIF_LDI)LCD_CPUIF_CS);


	/*
	UART_Printf("Enter Any Key to Test Normal Command Mode.\n");
	UART_Getc();

	UART_Printf("Normal Command  Mode.\n");
	 eCpuCmdMode = LCD_NORMAL_CMD;

	// LDI (X, Y) coordinate setting
	uCmdVal[0] = 0x4200;  //R66: X Addr Reg. 
	uCmdVal[1] = 0x4300;  //R67: Y Addr Reg.1
	uCmdVal[2] = 0x4400;	//R68: Y Addr Reg.2
	
	uCmdVal[3] = 0x4200;  
	uCmdVal[4] = 0x4300; 
	uCmdVal[5] = 0x4400;	

	uCmdVal[6] = 0x4200;  
	uCmdVal[7] = 0x4300;  
	uCmdVal[8] = 0x4400; 

	uCmdVal[9] = 0x4200;  
	uCmdVal[10] = 0x4300;  
	uCmdVal[11] = 0x4400;

	LCD_PutCmdToLdi_NormalCmd(uCmdVal, 12, LCD_CPUIF_CS); 

	LCD_InitInt(LCD_SYSIF_INT, eLcdCpuifInt, FIFO_25, FIFO_INT_WIN0, FINT_VSYNC);
	LCD_EnableInt();
	INTC_Enable(NUM_LCD2);

	LCD_Trigger();

	uCount =0;
	
	while(1)
	{
		if (uCount == ((u32)PER_THIRTY_FRM*8 + 1))
		break;
	}

	INTC_Disable(NUM_LCD2);
	LCD_DisableInt();
	

	UART_Printf("Enter Any Key to Test Manual Command Mode.\n");
	UART_Getc();

	UART_Printf("Manual Command  Mode.\n");
	 eCpuCmdMode = LCD_MANUAL_CMD;

	LCD_InitInt(LCD_SYSIF_INT, eLcdCpuifInt, FIFO_25, FIFO_INT_WIN0, FINT_VSYNC);
	LCD_EnableInt();
	INTC_Enable(NUM_LCD2);

	uCmdVal[0] = 0x4400;
	LCD_PutCmdToLdi_ManualCmd(uCmdVal, LCD_CPUIF_CS); 

	LCD_Trigger();

	uCount =0;
	
	while(1)
	{
		if (uCount == ((u32)PER_THIRTY_FRM*8 + 1))
		break;
	}

	INTC_Disable(NUM_LCD2);
	LCD_DisableInt();

	UART_Printf("Enter Any Key to Exit.\n");
	UART_Getc();
	
	uCmdVal[0] = 0x4200;  //R66: X Addr Reg. 
	uCmdVal[1] = 0x4300;  //R67: Y Addr Reg.1
	uCmdVal[2] = 0x4400;	//R68: Y Addr Reg.2
	
	uCmdVal[3] = 0x4200;  
	uCmdVal[4] = 0x4300; 
	uCmdVal[5] = 0x4400;	

	uCmdVal[6] = 0x4200;  
	uCmdVal[7] = 0x4300;  
	uCmdVal[8] = 0x4400; 

	uCmdVal[9] = 0x4200;  
	uCmdVal[10] = 0x4300;  
	uCmdVal[11] = 0x4400;
	LCD_PutCmdToLdi_NormalCmd(uCmdVal, 12, LCD_CPUIF_CS);
	LCD_Trigger();
	*/
	
}

static void NormalNAutoCmd(void)
{
	u32 uHSz, uVSz;
	u32 uModeSel[12];
	
	bIsRev = false;
	bIsAllWin = false;

	//LCD_InitLDI(LCD_CPUIF_CS);
	LCD_InitDISPC(eBgBpp, uLcdFbAddr, eBgWin, false);
	LCD_SetWinOnOff(1, eBgWin);
	LCD_GetFrmSz(&uHSz, &uVSz, eBgWin);

	LCD_Start();
	
	GLIB_Init(uLcdFbAddr, uHSz, uVSz, eBgBpp);
	GLIB_DrawPattern(uHSz, uVSz);

	UART_Printf("Normal Command  Mode.\n");
	 eCpuCmdMode = LCD_NORMAL_CMD;

	// LDI (X, Y) coordinate setting
	uCmdVal[0] = 0x4200;  //R66: X Addr Reg. 
	uCmdVal[1] = 0x4300;  //R67: Y Addr Reg.1
	uCmdVal[2] = 0x4400;	//R68: Y Addr Reg.2
	
	uCmdVal[3] = 0x4200;  
	uCmdVal[4] = 0x4300; 
	uCmdVal[5] = 0x4430;	

	uCmdVal[6] = 0x4200;  
	uCmdVal[7] = 0x4300;  
	uCmdVal[8] = 0x4460; 

	uCmdVal[9] = 0x4200;  
	uCmdVal[10] = 0x4300;  
	uCmdVal[11] = 0x4490;


	uModeSel[0] = LCD_NORMAL_CMD;
	uModeSel[1] = LCD_NORMAL_CMD;
	uModeSel[2] = LCD_NORMAL_CMD;

	uModeSel[3] = LCD_NORMAL_N_AUTO_CMD;
	uModeSel[4] = LCD_NORMAL_N_AUTO_CMD;
	uModeSel[5] = LCD_NORMAL_N_AUTO_CMD;

	uModeSel[6] = LCD_DISABLE_CMD;
	uModeSel[7] = LCD_DISABLE_CMD;
	uModeSel[8] = LCD_DISABLE_CMD;

	uModeSel[9] =  LCD_DISABLE_CMD;
	uModeSel[10] = LCD_DISABLE_CMD;
	uModeSel[11] = LCD_DISABLE_CMD;
	
	LCD_PutDataOrCmdToLdi_SelCmdMode(uCmdVal, 12, uModeSel, (CPUIF_LDI)LCD_CPUIF_CS, true, true);
	LCD_Trigger();	

	UART_Printf("Enter Any Key to Continue. \n");
	UART_Getc();


	uModeSel[0] = LCD_NORMAL_CMD;
	uModeSel[1] = LCD_NORMAL_CMD;
	uModeSel[2] = LCD_NORMAL_CMD;

	uModeSel[3] = LCD_NORMAL_CMD;
	uModeSel[4] = LCD_NORMAL_CMD;
	uModeSel[5] = LCD_NORMAL_CMD;

	uModeSel[6] = LCD_NORMAL_N_AUTO_CMD;
	uModeSel[7] = LCD_NORMAL_N_AUTO_CMD;
	uModeSel[8] = LCD_NORMAL_N_AUTO_CMD;
	
	uModeSel[9] =  LCD_DISABLE_CMD;
	uModeSel[10] = LCD_DISABLE_CMD;
	uModeSel[11] = LCD_DISABLE_CMD;
	
	LCD_PutDataOrCmdToLdi_SelCmdMode(uCmdVal, 12, uModeSel, (CPUIF_LDI)LCD_CPUIF_CS, true, true);
	LCD_Trigger();	


	UART_Printf("Enter Any Key to Continue. \n");
	UART_Getc();


	uModeSel[0] = LCD_NORMAL_CMD;
	uModeSel[1] = LCD_NORMAL_CMD;
	uModeSel[2] = LCD_NORMAL_CMD;

	uModeSel[3] = LCD_NORMAL_CMD;
	uModeSel[4] = LCD_NORMAL_CMD;
	uModeSel[5] = LCD_NORMAL_CMD;

	uModeSel[6] = LCD_NORMAL_CMD;
	uModeSel[7] = LCD_NORMAL_CMD;
	uModeSel[8] = LCD_NORMAL_CMD;

	uModeSel[9] = LCD_NORMAL_N_AUTO_CMD;
	uModeSel[10] = LCD_NORMAL_N_AUTO_CMD;
	uModeSel[11] = LCD_NORMAL_N_AUTO_CMD;
	
	LCD_PutDataOrCmdToLdi_SelCmdMode(uCmdVal, 12, uModeSel, (CPUIF_LDI)LCD_CPUIF_CS, true, true);
	LCD_Trigger();	

	UART_Printf("Enter Any Key to Exit. \n");
	UART_Getc();

	uCmdVal[0] = 0x4200;  //R66: X Addr Reg. 
	uCmdVal[1] = 0x4300;  //R67: Y Addr Reg.1
	uCmdVal[2] = 0x4490;  //R68: Y Addr Reg.2
	
	uCmdVal[3] = 0x4200;  
	uCmdVal[4] = 0x4300; 
	uCmdVal[5] = 0x4430;	

	uCmdVal[6] = 0x4200;  
	uCmdVal[7] = 0x4300;  
	uCmdVal[8] = 0x4460; 

	uCmdVal[9] = 0x4200;  
	uCmdVal[10] = 0x4300;  
	uCmdVal[11] = 0x4400;

	uModeSel[0] = LCD_AUTO_CMD;
	uModeSel[1] = LCD_AUTO_CMD;
	uModeSel[2] = LCD_AUTO_CMD;
	
	uModeSel[3] = LCD_DISABLE_CMD;
	uModeSel[4] = LCD_DISABLE_CMD;
	uModeSel[5] = LCD_DISABLE_CMD;

	uModeSel[6] = LCD_DISABLE_CMD;
	uModeSel[7] = LCD_DISABLE_CMD;
	uModeSel[8] = LCD_DISABLE_CMD;

	uModeSel[9] =  LCD_NORMAL_N_AUTO_CMD;
	uModeSel[10] = LCD_NORMAL_N_AUTO_CMD;
	uModeSel[11] = LCD_NORMAL_N_AUTO_CMD;

	LCD_PutDataOrCmdToLdi_SelCmdMode(uCmdVal, 12, uModeSel, (CPUIF_LDI)LCD_CPUIF_CS, true, false);

	INTC_SetVectAddr(NUM_LCD2, Isr_LCD_I80Int1);
	LCD_InitInt(LCD_SYSIF_INT, eLcdCpuifInt, FIFO_25, FIFO_INT_WIN0, FINT_VSYNC);
	LCD_EnableInt();
	INTC_Enable(NUM_LCD2);

	eCpuAutoCmdRate = PER_TWO_FRM;
	LCD_SetAutoCmdRate(eCpuAutoCmdRate, (CPUIF_LDI)LCD_CPUIF_CS);
	
	LCD_Trigger();	

	uCount =0;
	
	while(1)
	{
		if (uCount == (eCpuAutoCmdRate*2+1))
		break;
	}
	
	LCD_SetAutoCmdRate(DISABLE_AUTO_FRM, (CPUIF_LDI)LCD_CPUIF_CS);
	INTC_Disable(NUM_LCD2);
	LCD_DisableInt();

}

static void ManualRead(void)
{	
	u32 uGlibAddr;
	CSPACE eBgBpp;
	u32 uHSz, uVSz;
	u32 uReadVal;
	u32 i;
	
	bIsRev = false;
	bIsAllWin = false;
	
	uGlibAddr = CODEC_MEM_ST;
	eBgBpp = RGB16; // Can't support 24Bpp
	uHSz = 240;
	uVSz = 320;
	
	GLIB_Init(uGlibAddr, uHSz, uVSz, eBgBpp);
	GLIB_DrawPattern(uHSz, uVSz);

	LCD_InitLDI((CPUIF_LDI)LCD_CPUIF_CS);

	UART_Printf("Enter any key to read GRAM data. \n");
	UART_Getc();

	LCD_InitStartPosOnLcd((CPUIF_LDI)LCD_CPUIF_CS);
	
	for (i=0; i<uHSz*4; i++)
	{
		LCD_PutDataToLdi(*((u16*)(uGlibAddr+i*2)), (CPUIF_LDI)LCD_CPUIF_CS);
		
		LCD_GetDataFromLdi(&uReadVal, (CPUIF_LDI)LCD_CPUIF_CS);
		UART_Printf("%d: 0x%x\n", i, uReadVal);
	}	

	UART_Printf("Enter any key to read reg. value of LDI.\n");
	UART_Getc();

	uCmdVal[0] = 0x4200;
	LCD_PutCmdToLdi_ManualCmd(uCmdVal, (CPUIF_LDI)LCD_CPUIF_CS); 

	LCD_GetCmdFromLdi(&uReadVal, (CPUIF_LDI)LCD_CPUIF_CS);
	UART_Printf("Read Cmd: 0x%x\n", uReadVal);

	uCmdVal[0] = 0x4300;
	LCD_PutCmdToLdi_ManualCmd(uCmdVal, (CPUIF_LDI)LCD_CPUIF_CS); 

	LCD_GetCmdFromLdi(&uReadVal, (CPUIF_LDI)LCD_CPUIF_CS);
	UART_Printf("Read Cmd: 0x%x\n", uReadVal);

	uCmdVal[0] = 0x4400;
	LCD_PutCmdToLdi_ManualCmd(uCmdVal, (CPUIF_LDI)LCD_CPUIF_CS); 

	LCD_GetCmdFromLdi(&uReadVal, (CPUIF_LDI)LCD_CPUIF_CS);
	UART_Printf("Read Cmd: 0x%x\n", uReadVal);
}


static void DisIntCpuIf(void)
{	
	u32 uHSz, uVSz;

	bIsRev = false;
	bIsAllWin = false;	
	
	LCD_InitLDI((CPUIF_LDI)LCD_CPUIF_CS);
	
	LCD_InitDISPC(eBgBpp, uLcdFbAddr, eBgWin, false);
	LCD_SetWinOnOff(1, eBgWin);
	LCD_GetFrmSz(&uHSz, &uVSz, eBgWin);

	LCD_Start();

	INTC_SetVectAddr(NUM_LCD2, Isr_LCD_I80Int1);
	
	GLIB_Init(uLcdFbAddr, uHSz, uVSz, eBgBpp);
	GLIB_DrawPattern2(uHSz, uVSz);

	LCD_InitInt(LCD_SYSIF_INT, eLcdCpuifInt, FIFO_25, FIFO_INT_WIN0, FINT_VSYNC);
	LCD_EnableInt();
	INTC_Enable(NUM_LCD2);

	UART_Printf("Enter a 'Enter key' to exit test. \n");
	LCD_Trigger();
	
	uCount =0;
	
	while(!(UART_Getc() == '\r')){}

	INTC_Disable(NUM_LCD2);
	LCD_DisableInt();

}


static void Dithering(void)
{
	u32 uHSz, uVSz;
	
	bIsRev = false;
	bIsAllWin = false;
	
	UART_Printf("Enter any key to test simple display\n");
	UART_Getc();

	LCD_InitDISPC(eBgBpp, uLcdFbAddr, eBgWin, false);
	LCD_SetWinOnOff(1, eBgWin);

	LCD_Start();

	UART_Printf("Window %d (background) is displayed\n", (int)eBgWin);
	LCD_GetFrmSz(&uHSz, &uVSz, eBgWin);
	GLIB_InitInstance(uLcdFbAddr, uHSz, uVSz, eBgBpp);
	GLIB_DrawPattern(uHSz, uVSz);
	GLIB_PutLine(0, 0, 0, uVSz-1, C_BLUE);
	
	UART_Printf("\nEnter any key to dither 16bit.\n");
	UART_Getc();
	LCD_EnableDithering(1,DITHER_5BIT, DITHER_6BIT, DITHER_5BIT); 

	UART_Printf("\nEnter any key to dither 18bit.\n");
	UART_Getc();
	LCD_EnableDithering(1,DITHER_6BIT, DITHER_6BIT, DITHER_6BIT); 

	UART_Printf("\nEnter any key to dither 24bit.\n");
	UART_Getc();
	LCD_EnableDithering(1,DITHER_8BIT, DITHER_8BIT, DITHER_8BIT); 

	UART_Printf("Enter any key to exit");
	UART_Getc();
}


static void ByPass(void)
{
	LCD_SetPort_ByPass();
}

static void OutputDataFormat(void)
{
	u32 uHSz, uVSz;	
	u32 uColor[16];
	u32 uNofPixel, i, j, k;
	
	bIsRev = false;
	bIsAllWin = false;

	#if(LCD_MODULE_TYPE == LTS222QV_CPU)
		uHSz=240;
		uVSz=320;	
	#else
		Assert(0);
	#endif

	//////////////////////////////////////////////////////////////////////////////
	UART_Printf("\nPress any key to test (18BPP: 18 bit mode)\n");
	UART_Getc();

	uColor[3] = 0x3f<<12; 			// Red
	uColor[2] = (0x3f<<12)|(0x3f<<0); // Magenta
	uColor[1] = (0x3f<<12)|(0x3f<<6); // Yellow
	uColor[0] = 0x3ffff; 			// White
	
	LCD_InitBase2((CLK_SRC)LCLK_DOUT_MPLL, PNR_Parallel_RGB, CPU_18BIT);
	LCD_InitWin(RGB18, uHSz, uVSz, 0, 0, uHSz, uVSz, 0, 0, uLcdFbAddr, eBgWin, false);
	LCD_GetFrmSz(&uHSz,  &uVSz, eBgWin);
	LCD_SetWinOnOff(1, eBgWin);

	uNofPixel = uHSz*uVSz;
		
	for (i=0; i<uNofPixel; i++)
	{			
		if ( i < uNofPixel/8) 					  *(u32*)(uLcdFbAddr+i*4) = uColor[0];
		else if ( i >= uNofPixel/8 && i < uNofPixel/8*2)    *(u32*)(uLcdFbAddr+i*4) = uColor[0];
		else if ( i >= uNofPixel/8*2 && i < uNofPixel/8*3) *(u32*)(uLcdFbAddr+i*4) = uColor[1];
		else if ( i >= uNofPixel/8*3 && i < uNofPixel/8*4) *(u32*)(uLcdFbAddr+i*4) = uColor[1];
		else if ( i >= uNofPixel/8*4 && i < uNofPixel/8*5) *(u32*)(uLcdFbAddr+i*4) = uColor[2];
		else if ( i >= uNofPixel/8*5 && i < uNofPixel/8*6) *(u32*)(uLcdFbAddr+i*4) = uColor[2];
		else if ( i >= uNofPixel/8*6 && i < uNofPixel/8*7) *(u32*)(uLcdFbAddr+i*4) = uColor[3];
		else if ( i >= uNofPixel/8*7 && i < uNofPixel/8*8) *(u32*)(uLcdFbAddr+i*4) = uColor[3];	
	}
			
	LCD_Start();
	LCD_Trigger();
	

	//////////////////////////////////////////////////////////////////////////////
	UART_Printf("\nPress any key to test (16BPP: 8+8 bit mode)\n");
	UART_Getc();
	
	LCD_InitBase2((CLK_SRC)LCLK_DOUT_MPLL, PNR_Parallel_RGB, CPU_8_8BIT);
	LCD_InitWin(RGB16, uHSz, uVSz, 0, 0, uHSz, uVSz, 0, 0, uLcdFbAddr, eBgWin, false);
	LCD_SetWinOnOff(1, eBgWin);

	LCD_Start();	
	LCD_GetFrmSz(&uHSz, &uVSz, eBgWin);
	GLIB_InitInstance(uLcdFbAddr, uHSz, uVSz, eBgBpp);
	GLIB_DrawPattern(uHSz, uVSz);
	GLIB_PutLine(0, 0, 0, uVSz-1, C_BLUE);
	LCD_Trigger();

	
	//////////////////////////////////////////////////////////////////////////////
	UART_Printf("\nPress any key to test (18BPP: 9+9 bit mode)\n");
	UART_Getc();

	LCD_SetCpuOutputFormat(CPU_9_9BIT);
	LCD_InitLDI((CPUIF_LDI)LCD_CPUIF_CS);
	
	uColor[7] = 0x0;				  // Black
	uColor[6] = 0x3f<<0; 			  // Blue
	uColor[5] = 0x3f<<6; 			  // Green
	uColor[4] = (0x3f<<6)|(0x3f<<0);  // Cyan

	uColor[3] = 0x3f<<12; 			  // Red
	uColor[2] = (0x3f<<12)|(0x3f<<0);  // Magenta
	uColor[1] = (0x3f<<12)|(0x3f<<6);  // Yellow
	uColor[0] = 0x3ffff; 			  // White
	
	LCD_InitBase2((CLK_SRC)LCLK_DOUT_MPLL, PNR_Parallel_RGB, CPU_9_9BIT);

	LCD_InitWin(RGB18, uHSz, uVSz, 0, 0, uHSz, uVSz, 0, 0, uLcdFbAddr, eBgWin, false);
	LCD_GetFrmSz(&uHSz,  &uVSz, eBgWin);
	LCD_SetWinOnOff(1, eBgWin);

	uNofPixel = uHSz*uVSz;
		
	for (i=0; i<uNofPixel; i++)
	{			
		if ( i < uNofPixel/8) 					  *(u32*)(uLcdFbAddr+i*4) = uColor[0];
		else if ( i >= uNofPixel/8 && i < uNofPixel/8*2)    *(u32*)(uLcdFbAddr+i*4) = uColor[1];
		else if ( i >= uNofPixel/8*2 && i < uNofPixel/8*3) *(u32*)(uLcdFbAddr+i*4) = uColor[2];
		else if ( i >= uNofPixel/8*3 && i < uNofPixel/8*4) *(u32*)(uLcdFbAddr+i*4) = uColor[3];
		else if ( i >= uNofPixel/8*4 && i < uNofPixel/8*5) *(u32*)(uLcdFbAddr+i*4) = uColor[4];
		else if ( i >= uNofPixel/8*5 && i < uNofPixel/8*6) *(u32*)(uLcdFbAddr+i*4) = uColor[5];
		else if ( i >= uNofPixel/8*6 && i < uNofPixel/8*7) *(u32*)(uLcdFbAddr+i*4) = uColor[6];
		else if ( i >= uNofPixel/8*7 && i < uNofPixel/8*8) *(u32*)(uLcdFbAddr+i*4) = uColor[7];	
	}

	LCD_Start();
	LCD_Trigger();

	//////////////////////////////////////////////////////////////////////////////
	UART_Printf("\nPress any key to test (18BPP: 16+2 bit mode)\n");
	UART_Getc();

	LCD_SetCpuOutputFormat(CPU_16_2BIT);
	LCD_InitLDI((CPUIF_LDI)LCD_CPUIF_CS);
	
	uColor[0] = 0x3ffff;				// White
	uColor[1] = 0x3f<<0; 			// Blue
	uColor[2] = 0x3f<<6; 			// Green
	uColor[3] = (0x3f<<6)|(0x3f<<0);  // Cyan

	uColor[4] = 0x3f<<12; 			// Red
	uColor[5] = (0x3f<<12)|(0x3f<<0); // Magenta
	uColor[6] = (0x3f<<12)|(0x3f<<6); // Yellow
	uColor[7] = 0x0; 			// Black
	
	LCD_InitBase2((CLK_SRC)LCLK_DOUT_MPLL, PNR_Parallel_RGB, CPU_16_2BIT);
	LCD_InitWin(RGB18, uHSz, uVSz, 0, 0, uHSz, uVSz, 0, 0, uLcdFbAddr, eBgWin, false);
	LCD_GetFrmSz(&uHSz,  &uVSz, eBgWin);
	LCD_SetWinOnOff(1, eBgWin);

	uNofPixel = uHSz*uVSz;
		
	for (i=0; i<uNofPixel; i++)
	{			
		if ( i < uNofPixel/8) 					  *(u32*)(uLcdFbAddr+i*4) = uColor[0];
		else if ( i >= uNofPixel/8 && i < uNofPixel/8*2)    *(u32*)(uLcdFbAddr+i*4) = uColor[1];
		else if ( i >= uNofPixel/8*2 && i < uNofPixel/8*3) *(u32*)(uLcdFbAddr+i*4) = uColor[2];
		else if ( i >= uNofPixel/8*3 && i < uNofPixel/8*4) *(u32*)(uLcdFbAddr+i*4) = uColor[3];
		else if ( i >= uNofPixel/8*4 && i < uNofPixel/8*5) *(u32*)(uLcdFbAddr+i*4) = uColor[4];
		else if ( i >= uNofPixel/8*5 && i < uNofPixel/8*6) *(u32*)(uLcdFbAddr+i*4) = uColor[5];
		else if ( i >= uNofPixel/8*6 && i < uNofPixel/8*7) *(u32*)(uLcdFbAddr+i*4) = uColor[6];
		else if ( i >= uNofPixel/8*7 && i < uNofPixel/8*8) *(u32*)(uLcdFbAddr+i*4) = uColor[7];	
	}
			
	LCD_Start();
	LCD_Trigger();
	
	//////////////////////////////////////////////////////////////////////////////
	UART_Printf("\nPress any key to test (24BPP: 16+8 bit mode)\n");
	UART_Getc();
	
	LCD_InitBase2((CLK_SRC)LCLK_DOUT_MPLL, PNR_Parallel_RGB, CPU_16_8BIT);
	LCD_InitWin(RGB24, uHSz, uVSz, 0, 0, uHSz, uVSz, 0, 0, uLcdFbAddr, eBgWin, false);
	LCD_SetWinOnOff(1, eBgWin);
	LCD_GetFrmSz(&uHSz, &uVSz, eBgWin);

	GLIB_Init(uLcdFbAddr, uHSz, uVSz, RGB24);
	GLIB_ClearFrame(C_WHITE);
	LCD_Trigger();

	k=0;
	for(i=0; i<uVSz; i++)
	for(j=0; j<uHSz; j++)
//		GLIB_PutPixel2(j, i, pQVGA_24bpp_Image[k++]);

	LCD_Start();
	LCD_Trigger();

	UART_Printf("Enter any key to exit.\n");
	UART_Getc();

	LCD_Stop();
	LCD_SetAllWinOnOff(0);
	
}



static void Test_FIMD_Underrun(void)
{
	u32 uTemp0;
	u32 uHSz, uVSz;
	u32 i,j,k;
	u32 uTsize, uBurst, uCh;	
	u32 uDataCnts=50000;
	u32	uLLIBaseAddr2 = _DRAM_BaseAddress + 0x00100000;
	u32 uTxBuffAddr2 = _DRAM_BaseAddress + 0x2000000;
	u32 uRxBuffAddr2 = _DRAM_BaseAddress + 0x3000000;

	unsigned int r2,r3, r4, r5, r6, r7, r8,r9, r10;
	unsigned int* r0;
	unsigned int* r1;
	
	uLcdFbAddr = 0x51000000;
	eBgWin = WIN0;
	eBgBpp = RGB24;
	
	// 01. FIMD Display 
	UART_Printf("Enter Any Key To Test Simple Display\n");
	UART_Getc();

	LCD_InitDISPC(eBgBpp, uLcdFbAddr, eBgWin, false);
	LCD_SetWinOnOff(1, eBgWin);
	LCD_GetFrmSz(&uHSz, &uVSz, eBgWin);
	LCD_InitWin(eBgBpp, uHSz, uVSz, 0, 0, uHSz, uVSz, 0, 0, uLcdFbAddr, eBgWin, false);
	UART_Printf("Window %d (Background) Is Displayed\n", (int)eBgWin);
	GLIB_InitInstance(uLcdFbAddr, uHSz, uVSz, eBgBpp);
	LCD_Start();
	k=0;
	for(i=0; i<480; i++)
		for(j=0; j<800; j++)
			GLIB_PutPixel2(j, i, pSmile_2[k++]);

	// 02. DMA Func
	UART_Printf("Selected DMAC 0 ..... \n");
     	DMAC_InitCh(DMA0, DMA_ALL, &oDmac0);

	uCh= DMA_A;
	uTsize = WORD;
	uBurst = BURST128;
	uDataCnts=0x40000; // 1M Byte Transfer

	for (i = 0; i<(uDataCnts*uTsize+16); i++) 	// Clear the rx/tx buf. uRxBuffaddr = 0x53000000, uTxBuffaddr = 0x52000000
	{
		*(u8 *)(uRxBuffAddr2+i) = 0;
		*(u8 *)(uTxBuffAddr2+i) = 0;
	}
	for (i = 0; i<uDataCnts*uTsize; i++) 	// 1. Set up the tx buf.
		*(u8 *)(uTxBuffAddr2+i) = (u8)(i+2)%0xff;

	while(1)
	{
		DMAT_SetLLI(uTxBuffAddr2, uRxBuffAddr2, (DATA_SIZE)uTsize, (BURST_MODE)uBurst, uDataCnts);
//		UART_Printf("LLI Table Composition complete... now, DMA Start!!\n");
	        // Channel, LLI_Address, SrcAddr, Src Type, DstAddr, Dst Type, Transfer Width, Transfer Size, OpMode(DEMAND), Src Req, Dst Req, Burst
		DMACH_Setup((DMA_CH)uCh, uLLIBaseAddr2+0x20, uTxBuffAddr2, 0, uRxBuffAddr2, 0, (DATA_SIZE)uTsize, uDataCnts, DEMAND, MEM, MEM, (BURST_MODE)uBurst, &oDmac0);
		DMACH_Start(&oDmac0);

		/*
		if (CompareDMA(uTxBuffAddr2, uRxBuffAddr2, (DATA_SIZE)uTsize, uDataCnts))
			UART_Printf(" >> Test Tx&Rx -> Ok << \n");
		else
		{
			UART_Printf(" >>*** Tx-data & Rx-data mismatch ***<< \n");
		}
		*/

		// 03. Asm Func // r0 : source address, r1 : destinatino address, r2: source data

		#if 0 
//		r0 = (unsigned int*)(0x55000000);
//		r1 = (unsigned int*)(0x56000000);
//		r2 = 0x51400000;
		__asm 
		{
ROP
			LDMIA r0!, {r3-r10} 
			STMIA r1!, {r3-r10} 
			CMP 	r0, r2
			BNE		ROP
		}
		#endif

	}

	
}





static void FrmVsync(void)
{
	u32 i, uHSz, uVSz;	
	bIsRev = false;
	bIsAllWin = false;

	#if(LCD_MODULE_TYPE == LTS222QV_CPU)
		uHSz=240;
		uVSz=320;	
	#else
		Assert(0);
	#endif
	
	//Configure FRM & Vsync GPIO Port
	GPIO_SetFunctionAll(eGPIO_M, 0x000400, 0x0);

	//Set-up VIDTCON3 Register
	(*(volatile unsigned *)0x7710001c)= 0x0;
	(*(volatile unsigned *)0x7710001c)= (0x1<<31)|(0x1<<29)|(0x1<<8)|(0x1<<0);	

	//Trigger Fame Tansfer of CPU Type LCD
	Disp("\nPress any key to test (16BPP: 8+8 bit mode)\n");
	UART_Getc();
	
	LCD_InitBase2(LCLK_DOUT_MPLL, PNR_Parallel_RGB, CPU_8_8BIT);
	LCD_InitWin(RGB16, uHSz, uVSz, 0, 0, uHSz, uVSz, 0, 0, uLcdFbAddr, eBgWin, false);
	LCD_SetWinOnOff(1, eBgWin);

	LCD_Start();	
	LCD_GetFrmSz(&uHSz, &uVSz, eBgWin);
	GLIB_InitInstance(uLcdFbAddr, uHSz, uVSz, eBgBpp);
	GLIB_DrawPattern(uHSz, uVSz);
	//GLIB_PutLine(0, 0, 0, uVSz-1, C_BLUE);

	LCD_Trigger();

	for(i=0;i<2;i++)
		Delay(1000);

	LCD_Trigger();

	for(i=0;i<2;i++)
		Delay(1000);
	
	LCD_Trigger();

	Disp("Enter any key to exit.\n");
	UART_Getc();

	LCD_Stop();
	LCD_SetAllWinOnOff(0);
	
}


//////////////////////////////////////////////////////////
//ISR
 void __irq Isr_LCD_I80Int(void)
{		
	INTC_Disable(NUM_LCD2);
	LCD_DisableInt();
	
	uCount++;
	UART_Printf("TG Count: %d\n", uCount);
	
	if (uCount == ((u32)eCpuAutoCmdRate*2 + 1))
	{
		uCmdVal[0] = 0x4200;  //R66: X Addr Reg. 
		uCmdVal[1] = 0x4300;  //R67: Y Addr Reg.1
		uCmdVal[2] = 0x4430;  //R68: Y Addr Reg.2
	
		uCmdVal[3] = 0x4200;  
		uCmdVal[4] = 0x4300; 
		uCmdVal[5] = 0x4430;	

		uCmdVal[6] = 0x4200;  
		uCmdVal[7] = 0x4300;  
		uCmdVal[8] = 0x4430;	

		uCmdVal[9] = 0x4200;  
		uCmdVal[10] = 0x4300;  
		uCmdVal[11] = 0x4430;

		if ( eCpuCmdMode == LCD_AUTO_CMD)	LCD_PutCmdToLdi_AutoCmd(uCmdVal, 12, (CPUIF_LDI)LCD_CPUIF_CS);
		else if ( eCpuCmdMode == LCD_NORMAL_CMD) LCD_PutCmdToLdi_NormalCmd(uCmdVal, 12, (CPUIF_LDI)LCD_CPUIF_CS);
		else
		{ 
			uCmdVal[0] = 0x4430;
			LCD_PutCmdToLdi_ManualCmd(uCmdVal, (CPUIF_LDI)LCD_CPUIF_CS); 
		}
	}	
	else if (uCount == ((u32)eCpuAutoCmdRate*4 + 1))
	{
		uCmdVal[0] = 0x4200;  //R66: X Addr Reg. 
		uCmdVal[1] = 0x4300;  //R67: Y Addr Reg.1
		uCmdVal[2] = 0x4460;  //R68: Y Addr Reg.2
	
		uCmdVal[3] = 0x4200;  
		uCmdVal[4] = 0x4300; 
		uCmdVal[5] = 0x4460;	

		uCmdVal[6] = 0x4200;  
		uCmdVal[7] = 0x4300;  
		uCmdVal[8] = 0x4460;	

		uCmdVal[9] = 0x4200;  
		uCmdVal[10] = 0x4300;  
		uCmdVal[11] = 0x4460;	

		if ( eCpuCmdMode == LCD_AUTO_CMD)	LCD_PutCmdToLdi_AutoCmd(uCmdVal, 12, (CPUIF_LDI)LCD_CPUIF_CS);
		else if ( eCpuCmdMode == LCD_NORMAL_CMD) LCD_PutCmdToLdi_NormalCmd(uCmdVal, 12, (CPUIF_LDI)LCD_CPUIF_CS);
		else
		{ 
			uCmdVal[0] = 0x4460;
			LCD_PutCmdToLdi_ManualCmd(uCmdVal, (CPUIF_LDI)LCD_CPUIF_CS); 
		}
	}	
	else if (uCount == ((u32)eCpuAutoCmdRate*6 + 1))
	{
		uCmdVal[0] = 0x4200;  //R66: X Addr Reg. 
		uCmdVal[1] = 0x4300;  //R67: Y Addr Reg.1
		uCmdVal[2] = 0x4490;  //R68: Y Addr Reg.2
	
		uCmdVal[3] = 0x4200;  
		uCmdVal[4] = 0x4300; 
		uCmdVal[5] = 0x4490;	

		uCmdVal[6] = 0x4200;  
		uCmdVal[7] = 0x4300;  
		uCmdVal[8] = 0x4490;	

		uCmdVal[9] = 0x4200;  
		uCmdVal[10] = 0x4300;  
		uCmdVal[11] = 0x4490;	
		
		if ( eCpuCmdMode == LCD_AUTO_CMD)	LCD_PutCmdToLdi_AutoCmd(uCmdVal, 12, (CPUIF_LDI)LCD_CPUIF_CS);
		else if ( eCpuCmdMode == LCD_NORMAL_CMD) LCD_PutCmdToLdi_NormalCmd(uCmdVal, 12, (CPUIF_LDI)LCD_CPUIF_CS);
		else
		{ 
			uCmdVal[0] = 0x4490;
			LCD_PutCmdToLdi_ManualCmd(uCmdVal, (CPUIF_LDI)LCD_CPUIF_CS); 
		}
	}	

	LCD_Trigger();
	
	LCD_ClearInt(LCD_SYSIF_INT);
	INTC_ClearVectAddr();
	
	INTC_Enable(NUM_LCD2);
	LCD_EnableInt();
}


 void __irq Isr_LCD_I80Int1(void)
 {
	
	INTC_Disable(NUM_LCD2);
	LCD_DisableInt();

	UART_Printf("\n I80 Interrupt Routine. \n");

	uCount++;

	LCD_Trigger();

	LCD_ClearInt(LCD_SYSIF_INT);
	INTC_ClearVectAddr();
	
	INTC_Enable(NUM_LCD2);
	LCD_EnableInt();

 }	
  
 void __irq Isr_LCD_FrameInt(void)
 {
	volatile u32 temp;
	
	INTC_Disable(NUM_LCD1);
	LCD_DisableInt();

	UART_Printf("FI.");

	if(eBgWin == WIN0)
	{
		temp=Inp32(LCD_BASE+rWINCON0);

		if(temp & (1<<21))
		LCD_SetActiveFrameBuf(0, WIN0);
		else 
		LCD_SetActiveFrameBuf(1, WIN0);	
	}
	else if(eBgWin == WIN1)
	{
		temp=Inp32(LCD_BASE+rWINCON1);

		if(temp & (1<<21))
		LCD_SetActiveFrameBuf(uCount, WIN1);	
		else 
		LCD_SetActiveFrameBuf(1, WIN1);	
	}

	LCD_ClearInt(LCD_FRAME_INT);
	INTC_ClearVectAddr();
	
	INTC_Enable(NUM_LCD1);
	LCD_EnableInt();
	
}

 void __irq Isr_LCD_FifoInt(void)
{
	u32 uLineCOunt;
	u32 uLineCOunt2;	
	INTC_Disable(NUM_LCD0);
	LCD_DisableInt();

	uCount++;

	if(uCount == 512)
	{
		UART_Printf(".");
		uCount=0;
	}

	#if 0 // active period underrun test
	uLineCOunt = *(unsigned int *) 0x77100004;
	if ( ((uLineCOunt>>13)&0x3) == 0x2) UART_Printf( "%d ", ( (uLineCOunt>>16)&0x7ff) ); 
	#endif 
	
	LCD_ClearInt(LCD_FIFO_INT);
	INTC_ClearVectAddr();
	
	INTC_Enable(NUM_LCD0);
	LCD_EnableInt();
	
}

void __irq Isr_LCD_FifoEmptyInt(void)
{
	INTC_Disable(NUM_LCD0);
	LCD_DisableInt();
	
	UART_Printf("\nFIFO_EMPTY");
	
	LCD_ClearInt(LCD_FIFO_INT);
	INTC_ClearVectAddr();
	
}

//////////////////////////////////////
void __irq Isr_MSDMA_PreviewDone(void)
{
	LCD_Trigger();
	bPreviewDone = true;
	//UART_Printf("PMS");
	CAMERA_StartMSDmaPreviewPath();
	CAMERA_SetClearPreviewInt();
	INTC_ClearVectAddr();		
}

void __irq Isr_MSDMA_CodecDone(void)
{
	LCD_Trigger();
	bCodecDone = true;
	//UART_Printf("CMS");
	CAMERA_StartMSDmaCodecPath();
	CAMERA_SetClearCodecInt();
	INTC_ClearVectAddr();		
}

///////////////////////////////////////
void __irq Isr_ExtInt9(void)
{
    GPIO_EINT0EnMask(9);	//Mask EINT9
    GPIO_SetFunctionAll(eGPIO_J, 0xaaaaaaaa, 2); //GPJ[7..0]-> RGB VD[23..16], GPJ[11..8]-> VCLK, VDEN, VSYNC, HSYNC

	GPIO_EINT0ClrPend(9);
	INTC_ClearVectAddr();  
	
	UART_Printf("EINT9 interrupt is occurred .\n");

	//Set LCD GPIO Port
	GPIO_SetFunctionAll(eGPIO_I, 0xaaaaaaaa, 2); //GPI[15..0]-> RGB VD[15..0]

	GPIO_EINT0DisMask(9);	//Un-mask EINT9
	GPIO_SetFunctionAll(eGPIO_J, 0xaaaaaaaa, 2); //GPJ[7..0]-> RGB VD[23..16], GPJ[11..8]-> VCLK, VDEN, VSYNC, HSYNC	
}


static void PostSetCscTypeDmaToFifo(void)
{
	int nSelSrcDataFmt;

	while (1)
	{
		UART_Printf("\n");
		UART_Printf("[1] RGB16\n");
		UART_Printf("[2] RGB24\n");
		UART_Printf("[3] YCbYCr422 Interleave\n");
		UART_Printf("[4] YCrYCb422 Interleave\n");
		UART_Printf("[5] CbYCrY422 Interleave\n");
		UART_Printf("[6] CrYCbY422 Interleave\n");
		UART_Printf("[7] YUV 420 Non-Interleave\n");
		UART_Printf(">> Enter Source Data Format: ");

		nSelSrcDataFmt = UART_GetIntNum();

		if (nSelSrcDataFmt >= 1 && nSelSrcDataFmt <= 7)
		{
			eSrcDataFmt = 
				(nSelSrcDataFmt == 1) ? RGB16 :
				(nSelSrcDataFmt == 2) ? RGB24 :
				(nSelSrcDataFmt == 3) ? YCBYCR :
				(nSelSrcDataFmt == 4) ? YCRYCB :
				(nSelSrcDataFmt == 5) ? CBYCRY :
				(nSelSrcDataFmt == 6) ? CRYCBY : YC420;
			break;
		}
		else 
			UART_Printf("Invalid Data Format! Retry It!!\n");
	}

	if (eSrcDataFmt == RGB16)
	{
		uImgHSz = 240, uImgVSz = 320;

		//H-Size : Scale down(280->240),   V-Size : Scale down(360->320)
		uMidImgHSz = 360, uMidImgVSz = 480, uMidStartX = 40, uMidStartY = 80;
		uMidScaledHSz = 280, uMidScaledVSz = 360;
	}
	else if (eSrcDataFmt == RGB24)
	{
		uImgHSz = 240, uImgVSz = 320;
		
		//H-Size : Scale up(200->240),   V-Size : Scale up(240->320)
		uMidImgHSz = 360, uMidImgVSz = 480, uMidStartX = 40, uMidStartY = 80;
		uMidScaledHSz = 200, uMidScaledVSz = 240;
	}
	else if (eSrcDataFmt == YCBYCR)
	{
		uImgHSz = 240, uImgVSz = 320;
		
		//H-Size : Scale up(200->240),   V-Size : Scale down(360->320)
		uMidImgHSz = 360, uMidImgVSz = 480, uMidStartX = 160, uMidStartY = 120;
		uMidScaledHSz = 200, uMidScaledVSz = 360;
	}
	else if (eSrcDataFmt == YCRYCB)
	{
		uImgHSz = 240, uImgVSz = 320;

		//H-Size : Scale down(280->240),   V-Size : Scale up(280->320)
		uMidImgHSz = 360, uMidImgVSz = 480, uMidStartX = 0, uMidStartY = 0;
		uMidScaledHSz = 280, uMidScaledVSz = 280;
	}
	else if (eSrcDataFmt == CBYCRY)
	{
		uImgHSz = 240, uImgVSz = 320;

		//H-Size : Scale up(200->240),   V-Size : Scale up(280->320)
		uMidImgHSz = 360, uMidImgVSz = 480, uMidStartX = 40, uMidStartY = 80;
		uMidScaledHSz = 200, uMidScaledVSz = 280;
	}
	else if (eSrcDataFmt == CRYCBY)
	{
		uImgHSz = 240, uImgVSz = 320;

		//H-Size : Scale down(320->240),   V-Size : Scale down(440->320)
		uMidImgHSz = 360, uMidImgVSz = 480, uMidStartX = 40, uMidStartY = 40;
		uMidScaledHSz = 320, uMidScaledVSz = 440;
	}
	else if (eSrcDataFmt == YC420)
	{
		uImgHSz = 240, uImgVSz = 320;

		//H-Size : Scale up(200->240),   V-Size : Scale down(400->320)
		uMidImgHSz = 360, uMidImgVSz = 480, uMidStartX = 40, uMidStartY = 0;
		uMidScaledHSz = 320, uMidScaledVSz = 400;
	}

	UART_Printf("\n");
	if(uSimpleTest == 0)
	{
		UART_Printf("=========    Complex_Test Image Size    =========\n");
		UART_Printf("SrcImgHSz     = %d,       SrcImgVSz     = %d\n",uMidImgHSz,uMidImgVSz);
		UART_Printf("SrcStartX     = %d,         SrcStartY     = %d\n",uMidStartX,uMidStartY);
		UART_Printf("SrcCroppedHSz = %d,       SrcCroppedVSz = %d\n",uMidScaledHSz,uMidScaledVSz);
		UART_Printf("=================================================\n");
		UART_Printf("\n");
	}	
}

static void UnderRun_Test(void)
{
	u32 uFgAddr;
	u32 uHSz, uVSz;
	u32 uTemp;
	u32 i,j,k; // add. jungil

	u32 uWIN0HSize, uWIN1HSize, uWIN2HSize, uWIN3HSize, uWIN4HSize;
	u32 uWIN0VSize, uWIN1VSize, uWIN2VSize, uWIN3VSize, uWIN4VSize;
	u32 uDisplayNum;
	u32 uWIN0Addr, uWIN1Addr, uWIN2Addr, uWIN3Addr, uWIN4Addr;
	u32 uOption;
	u32 uBusSel, uBusPriority, uAHB_CON0;

	u32 uDiver, urVIDCON;

	uWIN0Addr = 0x51000000;
	uWIN1Addr = 0x52000000;
	uWIN2Addr = 0x53000000;
	uWIN3Addr = 0x54000000;
	uWIN4Addr = 0x55000000;
	
	bIsRev = false;
	bIsAllWin = false;
	eBgBpp = RGB24;
	
	UART_Printf("Want Display Window Num = ");
	uDisplayNum = UART_GetIntNum();
	UART_Printf("\n");

	LCD_InitBase();
	switch ( uDisplayNum )
	{
		case 1: 
			UART_Printf("Input H Size = ");
			uWIN0HSize = UART_GetIntNum();
			UART_Printf("\n");
			UART_Printf("Input V Size = ");
			uWIN0VSize = UART_GetIntNum();
			UART_Printf("\n");
			LCD_InitWin(eBgBpp, uWIN0HSize, uWIN0VSize, 0, 0, uWIN0HSize, uWIN0VSize, 0, 0, uWIN0Addr, WIN0, false);
			LCD_GetFrmSz(&uHSz, &uVSz, WIN0);
			GLIB_InitInstance(uWIN0Addr, uHSz, uVSz, eBgBpp);
			GLIB_DrawPattern(uHSz, uVSz);
			uHSz = 0 ; 
			uVSz = 0 ;
			break;
		case 2: 
			UART_Printf("Input H Size @ Win 0 = ");
			uWIN0HSize = UART_GetIntNum();
			UART_Printf("\n");
			UART_Printf("Input V Size @ Win 0 = ");
			uWIN0VSize = UART_GetIntNum();
			UART_Printf("\n");
			UART_Printf("Input H Size @ Win 1 = ");
			uWIN1HSize = UART_GetIntNum();
			UART_Printf("\n");
			UART_Printf("Input V Size @ Win 1 = ");
			uWIN1VSize = UART_GetIntNum();
			UART_Printf("\n");

			LCD_InitWin(eBgBpp, uWIN0HSize, uWIN0VSize, 0, 0, uWIN0HSize, uWIN0VSize, 0, 0, uWIN0Addr, WIN0, false);
			LCD_GetFrmSz(&uHSz, &uVSz, WIN0);
			GLIB_InitInstance(uWIN0Addr, uHSz, uVSz, eBgBpp);
			GLIB_DrawPattern(uHSz, uVSz);
			uHSz = 0 ; 
			uVSz = 0 ;
			LCD_InitWin(eBgBpp, uWIN1HSize, uWIN1VSize, 0, 0, uWIN1HSize, uWIN1VSize, 0, 0, uWIN1Addr, WIN1, false);
			LCD_GetFrmSz(&uHSz, &uVSz, WIN1);
			GLIB_InitInstance(uWIN1Addr, uHSz, uVSz, eBgBpp);
			GLIB_DrawPattern(uHSz, uVSz);
			uHSz = 0 ; 
			uVSz = 0 ;
			break;
		case 3: 
			UART_Printf("Input H Size @ Win 0 = ");
			uWIN0HSize = UART_GetIntNum();
			//uWIN0HSize = 320;
			UART_Printf("\n");
			UART_Printf("Input V Size @ Win 0 = ");
			uWIN0VSize = UART_GetIntNum();
			//uWIN0VSize = 240;
			UART_Printf("\n");
			UART_Printf("Input H Size @ Win 1 = ");
			uWIN1HSize = UART_GetIntNum();
			//uWIN1HSize = 320;
			UART_Printf("\n");
			UART_Printf("Input V Size @ Win 1 = ");
			uWIN1VSize = UART_GetIntNum();
			//uWIN1VSize = 240;
			UART_Printf("\n");
			UART_Printf("Input H Size @ Win 2 = ");
			uWIN2HSize = UART_GetIntNum();
			//uWIN2HSize = 320;
			UART_Printf("\n");
			UART_Printf("Input V Size @ Win 2 = ");
			uWIN2VSize = UART_GetIntNum();
			//uWIN2VSize = 240;
			UART_Printf("\n");

			LCD_InitWin(eBgBpp, uWIN0HSize, uWIN0VSize, 0, 0, uWIN0HSize, uWIN0VSize, 0, 0, uWIN0Addr, WIN0, false);
			LCD_GetFrmSz(&uHSz, &uVSz, WIN0);
			GLIB_InitInstance(uWIN0Addr, uHSz, uVSz, eBgBpp);
			GLIB_DrawPattern(uHSz, uVSz);
			uHSz = 0 ; 
			uVSz = 0 ;
			LCD_InitWin(eBgBpp, uWIN1HSize, uWIN1VSize, 0, 0, uWIN1HSize, uWIN1VSize, 0, 0, uWIN1Addr, WIN1, false);
			LCD_GetFrmSz(&uHSz, &uVSz, WIN1);
			GLIB_InitInstance(uWIN1Addr, uHSz, uVSz, eBgBpp);
			GLIB_DrawPattern(uHSz, uVSz);
			uHSz = 0 ; 
			uVSz = 0 ;
			LCD_InitWin(eBgBpp, uWIN2HSize, uWIN2VSize, 0, 0, uWIN2HSize, uWIN2VSize, 0, 0, uWIN2Addr, WIN2, false);
			LCD_GetFrmSz(&uHSz, &uVSz, WIN2);
			GLIB_InitInstance(uWIN2Addr, uHSz, uVSz, eBgBpp);
			GLIB_DrawPattern(uHSz, uVSz);
			uHSz = 0 ; 
			uVSz = 0 ;
			break;
		case 4:
			UART_Printf("Input H Size @ Win 0 = ");
			uWIN0HSize = UART_GetIntNum();
			UART_Printf("\n");
			UART_Printf("Input V Size @ Win 0 = ");
			uWIN0VSize = UART_GetIntNum();
			UART_Printf("\n");
			UART_Printf("Input H Size @ Win 1 = ");
			uWIN1HSize = UART_GetIntNum();
			UART_Printf("\n");
			UART_Printf("Input V Size @ Win 1 = ");
			uWIN1VSize = UART_GetIntNum();
			UART_Printf("\n");
			UART_Printf("Input H Size @ Win 2 = ");
			uWIN2HSize = UART_GetIntNum();
			UART_Printf("\n");
			UART_Printf("Input V Size @ Win 2 = ");
			uWIN2VSize = UART_GetIntNum();
			UART_Printf("\n");
			UART_Printf("Input H Size @ Win 3 = ");
			uWIN3HSize = UART_GetIntNum();
			UART_Printf("\n");
			UART_Printf("Input V Size @ Win 3 = ");
			uWIN3VSize = UART_GetIntNum();
			UART_Printf("\n");


			LCD_InitWin(eBgBpp, uWIN0HSize, uWIN0VSize, 0, 0, uWIN0HSize, uWIN0VSize, 0, 0, uWIN0Addr, WIN0, false);
			LCD_GetFrmSz(&uHSz, &uVSz, WIN0);
			GLIB_InitInstance(uWIN0Addr, uHSz, uVSz, eBgBpp);
			GLIB_DrawPattern(uHSz, uVSz);
			uHSz = 0 ; 
			uVSz = 0 ;
			LCD_InitWin(eBgBpp, uWIN1HSize, uWIN1VSize, 0, 0, uWIN1HSize, uWIN1VSize, 0, 0, uWIN1Addr, WIN1, false);
			LCD_GetFrmSz(&uHSz, &uVSz, WIN1);
			GLIB_InitInstance(uWIN1Addr, uHSz, uVSz, eBgBpp);
			GLIB_DrawPattern(uHSz, uVSz);
			uHSz = 0 ; 
			uVSz = 0 ;
			LCD_InitWin(eBgBpp, uWIN2HSize, uWIN2VSize, 0, 0, uWIN2HSize, uWIN2VSize, 0, 0, uWIN2Addr, WIN2, false);
			LCD_GetFrmSz(&uHSz, &uVSz, WIN2);
			GLIB_InitInstance(uWIN2Addr, uHSz, uVSz, eBgBpp);
			GLIB_DrawPattern(uHSz, uVSz);
			uHSz = 0 ; 
			uVSz = 0 ;
			LCD_InitWin(eBgBpp, uWIN3HSize, uWIN3VSize, 0, 0, uWIN3HSize, uWIN3VSize, 0, 0, uWIN3Addr, WIN3, false);
			LCD_GetFrmSz(&uHSz, &uVSz, WIN3);
			GLIB_InitInstance(uWIN3Addr, uHSz, uVSz, eBgBpp);
			GLIB_DrawPattern(uHSz, uVSz);
			uHSz = 0 ; 
			uVSz = 0 ;
			break;
		case 5:
			UART_Printf("Input H Size @ Win 0 = ");
			uWIN0HSize = UART_GetIntNum();
			UART_Printf("\n");
			UART_Printf("Input V Size @ Win 0 = ");
			uWIN0VSize = UART_GetIntNum();
			UART_Printf("\n");
			UART_Printf("Input H Size @ Win 1 = ");
			uWIN1HSize = UART_GetIntNum();
			UART_Printf("\n");
			UART_Printf("Input V Size @ Win 1 = ");
			uWIN1VSize = UART_GetIntNum();
			UART_Printf("\n");
			UART_Printf("Input H Size @ Win 2 = ");
			uWIN2HSize = UART_GetIntNum();
			UART_Printf("\n");
			UART_Printf("Input V Size @ Win 2 = ");
			uWIN2VSize = UART_GetIntNum();
			UART_Printf("\n");
			UART_Printf("Input H Size @ Win 3 = ");
			uWIN3HSize = UART_GetIntNum();
			UART_Printf("\n");
			UART_Printf("Input V Size @ Win 3 = ");
			uWIN3VSize = UART_GetIntNum();
			UART_Printf("\n");
			UART_Printf("Input H Size @ Win 4 = ");
			uWIN4HSize = UART_GetIntNum();
			UART_Printf("\n");
			UART_Printf("Input V Size @ Win 4 = ");
			uWIN4VSize = UART_GetIntNum();
			UART_Printf("\n");

			LCD_InitWin(eBgBpp, uWIN0HSize, uWIN0VSize, 0, 0, uWIN0HSize, uWIN0VSize, 0, 0, uWIN0Addr, WIN0, false);
			LCD_GetFrmSz(&uHSz, &uVSz, WIN0);
			GLIB_InitInstance(uWIN0Addr, uHSz, uVSz, eBgBpp);
			GLIB_DrawPattern(uHSz, uVSz);
			uHSz = 0 ; 
			uVSz = 0 ;
			LCD_InitWin(eBgBpp, uWIN1HSize, uWIN1VSize, 0, 0, uWIN1HSize, uWIN1VSize, 0, 0, uWIN1Addr, WIN1, false);
			LCD_GetFrmSz(&uHSz, &uVSz, WIN1);
			GLIB_InitInstance(uWIN1Addr, uHSz, uVSz, eBgBpp);
			GLIB_DrawPattern(uHSz, uVSz);
			uHSz = 0 ; 
			uVSz = 0 ;
			LCD_InitWin(eBgBpp, uWIN2HSize, uWIN2VSize, 0, 0, uWIN2HSize, uWIN2VSize, 0, 0, uWIN2Addr, WIN2, false);
			LCD_GetFrmSz(&uHSz, &uVSz, WIN2);
			GLIB_InitInstance(uWIN2Addr, uHSz, uVSz, eBgBpp);
			GLIB_DrawPattern(uHSz, uVSz);
			uHSz = 0 ; 
			uVSz = 0 ;
			LCD_InitWin(eBgBpp, uWIN3HSize, uWIN3VSize, 0, 0, uWIN3HSize, uWIN3VSize, 0, 0, uWIN3Addr, WIN3, false);
			LCD_GetFrmSz(&uHSz, &uVSz, WIN3);
			GLIB_InitInstance(uWIN3Addr, uHSz, uVSz, eBgBpp);
			GLIB_DrawPattern(uHSz, uVSz);
			uHSz = 0 ; 
			uVSz = 0 ;
			LCD_InitWin(eBgBpp, uWIN4HSize, uWIN4VSize, 0, 0, uWIN4HSize, uWIN4VSize, 0, 0, uWIN4Addr, WIN4, false);
			LCD_GetFrmSz(&uHSz, &uVSz, WIN4);
			GLIB_InitInstance(uWIN4Addr, uHSz, uVSz, eBgBpp);
			GLIB_DrawPattern(uHSz, uVSz);
			uHSz = 0 ; 
			uVSz = 0 ;
			break;
		default :
			break;
	}
	
//	LCD_Start();

	UART_Printf("Bus Master Setting 0: default , 1: Last grant  2: Rotation\n");
	uBusSel = UART_GetIntNum();
	UART_Printf("Bus Master Setting 0: default , 1-6: Prioty setting \n");
 	uBusPriority = UART_GetIntNum();

	uAHB_CON0 = *(unsigned int*)0x7e00f100;
	uAHB_CON0 = (uAHB_CON0 & ~(0x3f) ) |(uBusPriority<<0) |  (uBusSel<<4);
	UART_Printf("REgister Setting = 0x%8x\n", uAHB_CON0 );
	*(unsigned int *)0x7e00f100 = uAHB_CON0 ; 
	
	INTC_SetVectAddr(NUM_LCD0, Isr_LCD_FifoInt);

	UART_Printf("INT Window Setting == ");
	uOption = UART_GetIntNum();
	UART_Printf("\n");
	
	LCD_InitInt(LCD_FIFO_INT, eLcdCpuifInt, FIFO_EMPTY, uOption, FINT_BACKPORCH);
//	LCD_InitInt(LCD_FIFO_INT, eLcdCpuifInt, FIFO_EMPTY, FIFO_INT_WIN1, FINT_BACKPORCH);		
//	LCD_InitInt(LCD_FIFO_INT, eLcdCpuifInt, FIFO_EMPTY, FIFO_INT_WIN2, FINT_BACKPORCH);
//	LCD_InitInt(LCD_FIFO_INT, eLcdCpuifInt, FIFO_EMPTY, FIFO_INT_WIN3, FINT_BACKPORCH);
//	LCD_InitInt(LCD_FIFO_INT, eLcdCpuifInt, FIFO_EMPTY, FIFO_INT_WIN4, FINT_BACKPORCH);
	LCD_EnableInt();
	INTC_Enable(NUM_LCD0);

	UART_Printf("Input Divider Value (4~255)");
	uDiver = UART_GetIntNum();
	UART_Printf("\n");

	urVIDCON = Inp32(0x77100000);
	urVIDCON= ( urVIDCON & (~(0xff<<6)) ) | (uDiver <<6) ; 
	Outp32(0x77100000, urVIDCON);

	


	LCD_Start();
	
	switch ( uDisplayNum )
	{
		case 1:
			LCD_SetWinOnOff(1, WIN0);
			break;
		case 2:
			LCD_SetWinOnOff(1, WIN0);
			LCD_SetWinOnOff(1, WIN1);
			break;
		case 3:
			LCD_SetWinOnOff(1, WIN1);
			LCD_SetWinOnOff(1, WIN1);
			LCD_SetWinOnOff(1, WIN2);
			break;
		case 4:
			LCD_SetWinOnOff(1, WIN0);
			LCD_SetWinOnOff(1, WIN1);
			LCD_SetWinOnOff(1, WIN2);
			LCD_SetWinOnOff(1, WIN3);
			break;
		case 5:
			LCD_SetWinOnOff(1, WIN0);
			LCD_SetWinOnOff(1, WIN1);
			LCD_SetWinOnOff(1, WIN2);
			LCD_SetWinOnOff(1, WIN3);
			LCD_SetWinOnOff(1, WIN4);
			break;
		default:
			break;
	}
	
	UART_Getc();

	LCD_SetWinOnOff(0, WIN0);
	LCD_SetWinOnOff(0, WIN1);
	LCD_SetWinOnOff(0, WIN2);
	LCD_SetWinOnOff(0, WIN3);
	LCD_SetWinOnOff(0, WIN4);
	LCD_Stop();
		
	INTC_Disable(NUM_LCD0);
	LCD_DisableInt();

	UART_Printf("\n");


}





///////////////////////////////////////////////////////////
void LCD_Test(void)
{
	int i=0, nSel;
	char pcBgBppStr[50],  pcBgPalBppStr[50];
	char pcFgBppStr[50], pcFgArgbBppStr[50];

	char pcBgBppStr_Rev[50];
	char pcFgBppStr_Rev[2][50];
	char pcBppStr_All[5][50];

	const testFuncMenu lcd_function[]=
	{
		0,                      "Exit\n",	
		TestRgbIf,				"LCD Controller RGB I/F Test",
		TestCpuIf,				"LCD Controller CPU I/F Test",	

		SetRgbBpp,				"Select RGB16 or 24 bpp for BG and FG",
		SetArgbBpp,				"Select RGB16 or 24 bpp for BG, and ARGB16 or ARGB24 bpp for FG",
		SetAllWinBpp,			"Select Bpp for Win0 ~ Win4	",
		SetPaletteBppAndWin,	"Select Palette Depth and Palette Bpp",
		SetWinOfFgAndBg,		"Select Window for BG and FG",
		SelectReverseWins,		"Select Window for BG, FG1, and FG2, and RGB16 or 24bpp",

		0,							 0
	};

	bIsRev = false;
	bIsAllWin = false;

	eBgWin = WIN0;
	eFgWin = WIN1;
	
	#if (LCD_MODULE_TYPE == LTP700WV_RGB ||LCD_MODULE_TYPE ==LTE480WV_RGB||LCD_MODULE_TYPE ==LMS480KC01_RGB) // add. jungil 
		eBgBpp = RGB24;
		eFgBpp = RGB24;
		eFgArgbBpp = ARGB24;

	#else
		eBgBpp = RGB16;
		eFgBpp = RGB16;
		eFgArgbBpp = ARGB16;
		
	#endif

	nSel = (int) LCD_CPUIF_CS;
	
	if(nSel == 0)
		eLcdCpuifInt = LCD_SYSMAIN_INT;  
	else
		eLcdCpuifInt = LCD_SYSSUB_INT;

	
	ePalBpp = PAL8;
	ePalDataBpp = RGB16;

	eWinBpp[0] = RGB24;
	eWinBpp[1] = RGB16;
	eWinBpp[2] = PAL4;
	eWinBpp[3] = PAL2;
	eWinBpp[4] = PAL1;

	eBgBpp_Rev = RGB16;
	eFgBppArray_Rev[0] = RGB24;
	eFgBppArray_Rev[1] = RGB16;

	eBgWin_Rev = WIN2;
	eFgWinArray_Rev[0] = WIN1;
	eFgWinArray_Rev[1] = WIN0;

	while (1)
	{
		UART_Printf("\n");
		for (i=0; (int)(lcd_function[i].desc)!=0; i++)
		{
			UART_Printf("%2d: %s\n", i,lcd_function[i].desc);
		}

			UART_Printf("========================================================================\n");			

		if (bIsRev == false && bIsAllWin == false)
		{
			ConvertCSpaceToString(eBgBpp, pcBgBppStr);
			ConvertCSpaceToString(ePalBpp, pcBgPalBppStr);
			ConvertCSpaceToString(eFgBpp, pcFgBppStr);			
			ConvertCSpaceToString(eFgArgbBpp, pcFgArgbBppStr);						
			
			UART_Printf("< Currnet Background : WINDOW %d >\n", (int)eBgWin);
			UART_Printf("< Current Foreground : WINDOW %d >\n", (int)eFgWin);

			UART_Printf("< Current Background's RGB BPP : %s >\n", pcBgBppStr);
			UART_Printf("< Current Background's PAL BPP : %s >\n", pcBgPalBppStr);
			UART_Printf("< Current Foreground's RGB BPP : %s >\n", pcFgBppStr);		
			UART_Printf("< Current Foreground's ARGB BPP : %s >\n", pcFgArgbBppStr);	
		}
		else if (bIsRev == true)
		{
			ConvertCSpaceToString(eBgBpp_Rev, pcBgBppStr_Rev);						
			ConvertCSpaceToString(eFgBppArray_Rev[0], pcFgBppStr_Rev[0]);						
			ConvertCSpaceToString(eFgBppArray_Rev[1], pcFgBppStr_Rev[1]);								
			
			UART_Printf("< Currnet Background : WINDOW %d >\n", (int)eBgWin_Rev);
			UART_Printf("< Current Foreground1: WINDOW %d >\n", (int)eFgWinArray_Rev[0]);
			UART_Printf("< Current Foreground2: WINDOW %d >\n", (int)eFgWinArray_Rev[1]);			

			UART_Printf("< Current Background's RGB BPP : %s >\n", pcBgBppStr_Rev);
			UART_Printf("< Current Foreground1's RGB BPP : %s >\n", pcFgBppStr_Rev[0]);		
			UART_Printf("< Current Foreground2's RGB BPP : %s >\n", pcFgBppStr_Rev[1]);					
		}
		else if (bIsAllWin == true)
		{
			int i;
			for (i=0; i<5; i++)
			{
				ConvertCSpaceToString(eWinBpp[i], pcBppStr_All[i]);
				if (i==0)
					UART_Printf("< Currnet Background : WINDOW 0, BPP: %s >\n", pcBppStr_All[i]);
				else
					UART_Printf("< Currnet Foreground%d : WINDOW %d, BPP: %s >\n", i, i, pcBppStr_All[i]);
			}
		}
		
		UART_Printf("========================================================================\n");			
		UART_Printf("\nSelect the function to test : ");

		nSel = UART_GetIntNum();

		UART_Printf("\n");

		if (nSel == 0)
			break;
		if (nSel>0 && nSel<(sizeof(lcd_function)/8-1))
			 (lcd_function[nSel].func)();
	}		
}

 static void TestRgbIf(void)
{
	int i=0, nSel;
	char pcBgBppStr[50], pcBgPalBppStr[50];
	char pcFgBppStr[50], pcFgArgbBppStr[50];

	char pcBgBppStr_Rev[50];
	char pcFgBppStr_Rev[2][50];
	char pcBppStr_All[5][50];

	
	const testFuncMenu lcd_rgbif_function[]=
	{
		0,                          "Exit\n",
			
		DisBasicRgb,		        "Basic Display via RGB IF",
		
		VirtualScreen,		        "Virtual Screen/Soft Scrolling Test",
		ColorMap,					"Color Map Test",
		ColorKey,					"Color Key Test",
		BlendingPerPlane,			"Blending Per Plane Test",
		BlendingPerPixel,			"Blending Per Pixel Test",
		DisIntRgb,					"Display Interrupt Test",
		DoubleBuffering,			"Manual Double Buffering Test (Using Frame Int.)",
		AutoDoubleBuffering, 		"Auto Double Buffering Test",
		Palette,					"Palette Test",
		NonPaletteBpp,				"18bpp(Non-Palette) Test",
		Swap,						"Swap Test",
		VDMABurstLength,			"Video DMA Burst Length Test",
		#if (LCD_MODULE_TYPE == LTP700WV_RGB ||LCD_MODULE_TYPE ==LTE480WV_RGB)
		WvgaDis,					"Display WVGA Image",
		#else if (LCD_MODULE_TYPE == LTV300GV_RGB)
		VgaDis,						"Display VGA Image",
		#endif
		ParallelBGR,				"Simple Parallel BGR Test",
		#if (LCD_MODULE_TYPE == LTS222QV_SRGB)
		SerialRGB,					"Simple Serial RGB/BGR Test",
		#endif
		CameraLocalPath,			"Local Input Test From Camera Preview/Codec Path",
		PostLocalPath,				"Local Input Test From Post Path",
		PostLocalPath2,				"Local Input Test From Post Path_WorkAround",		
		Interlace,					"Interlace Test",
		Dithering,					"Dithering Test",
		EintAliasing,				"Ext Int Aliasing Test",
		GPIOMuxing,					"GPIO Mux Control Test",
		//TestDisplayingAllWin,              "Display All Window",
		SetAllWinBpp,                      "Select Combination of Windows To Display All Window\n",			
		DisBasicITU601,                  "ITU 601 Output Test ",
		DisBasicITU656,                   "ITU 656 Output Test ",
	#if 0 // for test by zeus 080531
		Test_FIMD_Underrun,          "Underrun Test\n",
		PostLocalPath_Test,             "LCD-POST Test\n", // test zeus
		PostLocalPath_Test2,             "LCD-POST Workaround Test\n",		
		UnderRun_Test,                    "Underrun TEst - jugil ", // test zues
		//YUV422Out,				"YUV 422 Out Port Test",
	#endif
		0,							 0
	};

	LCD_SetPort();
	LCD_InitLDI((CPUIF_LDI)LCD_CPUIF_CS);
	
	while (1)
	{
		UART_Printf("\n");
		for (i=0; (int)(lcd_rgbif_function[i].desc)!=0; i++)
		{
			UART_Printf("%2d: %s\n", i, lcd_rgbif_function[i].desc);
		}

		
		UART_Printf("========================================================================\n");			

		if (bIsRev == false && bIsAllWin == false)
		{
			ConvertCSpaceToString(eBgBpp, pcBgBppStr);
			ConvertCSpaceToString(ePalBpp, pcBgPalBppStr);
			ConvertCSpaceToString(eFgBpp, pcFgBppStr);			
			ConvertCSpaceToString(eFgArgbBpp, pcFgArgbBppStr);						
			
			UART_Printf("< Currnet Background : WINDOW %d >\n", (int)eBgWin);
			UART_Printf("< Current Foreground : WINDOW %d >\n", (int)eFgWin);

			UART_Printf("< Current Background's RGB BPP : %s >\n", pcBgBppStr);
			UART_Printf("< Current Background's PAL BPP : %s >\n", pcBgPalBppStr);
			UART_Printf("< Current Foreground's RGB BPP : %s >\n", pcFgBppStr);		
			UART_Printf("< Current Foreground's ARGB BPP : %s >\n", pcFgArgbBppStr);	
		}
		else if (bIsRev == true)
		{
			ConvertCSpaceToString(eBgBpp_Rev, pcBgBppStr_Rev);						
			ConvertCSpaceToString(eFgBppArray_Rev[0], pcFgBppStr_Rev[0]);						
			ConvertCSpaceToString(eFgBppArray_Rev[1], pcFgBppStr_Rev[1]);								
			
			UART_Printf("< Currnet Background : WINDOW %d >\n", (int)eBgWin_Rev);
			UART_Printf("< Current Foreground1: WINDOW %d >\n", (int)eFgWinArray_Rev[0]);
			UART_Printf("< Current Foreground2: WINDOW %d >\n", (int)eFgWinArray_Rev[1]);			

			UART_Printf("< Current Background's RGB BPP : %s >\n", pcBgBppStr_Rev);
			UART_Printf("< Current Foreground1's RGB BPP : %s >\n", pcFgBppStr_Rev[0]);		
			UART_Printf("< Current Foreground2's RGB BPP : %s >\n", pcFgBppStr_Rev[1]);					
		}
		else if (bIsAllWin == true)
		{
			int i;
			for (i=0; i<5; i++)
			{
				ConvertCSpaceToString(eWinBpp[i], pcBppStr_All[i]);
				if (i==0)
					UART_Printf("< Currnet Background : WINDOW 0, BPP: %s >\n", pcBppStr_All[i]);
				else
					UART_Printf("< Currnet Foreground%d : WINDOW %d, BPP: %s >\n", i, i, pcBppStr_All[i]);
			}
		}
		UART_Printf("========================================================================\n");			
		UART_Printf("\nSelect the function to test : ");
		nSel = UART_GetIntNum();
		UART_Printf("\n");

		if (nSel == 0)
		{
			#if (LCD_MODULE_TYPE == LTP700WV_RGB ||LCD_MODULE_TYPE ==LTE480WV_RGB)
				LCD_ExitLTP700WV_RGB();

			#else
				LCD_SetAllWinOnOff(0);
				LCD_Stop();
			#endif
	
			break;
		}
		if (nSel>0 && nSel<(sizeof(lcd_rgbif_function)/8-1))
			 (lcd_rgbif_function[nSel].func)();
	}	
}

static void TestCpuIf(void)
{
	int i=0, nSel;
	char pcBgBppStr[50], pcBgPalBppStr[50];
	char pcFgBppStr[50], pcFgArgbBppStr[50];

	char pcBgBppStr_Rev[50];
	char pcFgBppStr_Rev[2][50];
	char pcBppStr_All[5][50];

	const testFuncMenu lcd_cpuif_function[]=
	{
		0,                          "Exit\n",
		DisBasicCpu,				"Basic Display via CPU IF",
		
		AutoNormalManualCmd,		"Command Test (Auto/Normal/Manual Mode) ",
		NormalNAutoCmd,				"Normal & Auto Command Mode Test ",
		ManualRead,					"Manual Read Test ",
		PartialDis,					"Partial Display",
		DisIntCpuIf,				"Display Interrupt Test (I80)",

		VirtualScreen,				"Virtual Screen/Soft Scrolling Test",
		ColorMap,					"Color Map Test",
		ColorKey,					"Color Key Test",
		BlendingPerPlane,			"Blending Per Plane Test",
		ByPass,						"ByPass",
		OutputDataFormat,			"Output Data Format (Parallel/Split Mode) Test",	
		//FrmVsync,					"FRM & Vsync Signal Test",
		0,						 	0
	};

	LCD_SetPort();
	LCD_InitLDI((CPUIF_LDI)LCD_CPUIF_CS);

	while (1)
	{
		UART_Printf("\n");
		for (i=0; (int)(lcd_cpuif_function[i].desc)!=0; i++)
		{
			UART_Printf("%2d: %s\n", i, lcd_cpuif_function[i].desc);
		}

		
		UART_Printf("========================================================================\n");			

		if (bIsRev == false && bIsAllWin == false)
		{
			ConvertCSpaceToString(eBgBpp, pcBgBppStr);
			ConvertCSpaceToString(ePalBpp, pcBgPalBppStr);
			ConvertCSpaceToString(eFgBpp, pcFgBppStr);			
			ConvertCSpaceToString(eFgArgbBpp, pcFgArgbBppStr);						
			
			UART_Printf("< Currnet Background : WINDOW %d >\n", (int)eBgWin);
			UART_Printf("< Current Foreground : WINDOW %d >\n", (int)eFgWin);

			UART_Printf("< Current Background's RGB BPP : %s >\n", pcBgBppStr);
			UART_Printf("< Current Background's PAL BPP : %s >\n", pcBgPalBppStr);
			UART_Printf("< Current Foreground's RGB BPP : %s >\n", pcFgBppStr);		
			UART_Printf("< Current Foreground's ARGB BPP : %s >\n", pcFgArgbBppStr);	
		}
		else if (bIsRev == true)
		{
			ConvertCSpaceToString(eBgBpp_Rev, pcBgBppStr_Rev);						
			ConvertCSpaceToString(eFgBppArray_Rev[0], pcFgBppStr_Rev[0]);						
			ConvertCSpaceToString(eFgBppArray_Rev[1], pcFgBppStr_Rev[1]);								
			
			UART_Printf("< Currnet Background : WINDOW %d >\n", (int)eBgWin_Rev);
			UART_Printf("< Current Foreground1: WINDOW %d >\n", (int)eFgWinArray_Rev[0]);
			UART_Printf("< Current Foreground2: WINDOW %d >\n", (int)eFgWinArray_Rev[1]);			

			UART_Printf("< Current Background's RGB BPP : %s >\n", pcBgBppStr_Rev);
			UART_Printf("< Current Foreground1's RGB BPP : %s >\n", pcFgBppStr_Rev[0]);		
			UART_Printf("< Current Foreground2's RGB BPP : %s >\n", pcFgBppStr_Rev[1]);					
		}
		else if (bIsAllWin == true)
		{
			int i;
			for (i=0; i<5; i++)
			{
				ConvertCSpaceToString(eWinBpp[i], pcBppStr_All[i]);
				if (i==0)
					UART_Printf("< Currnet Background : WINDOW 0, BPP: %s >\n", pcBppStr_All[i]);
				else
					UART_Printf("< Currnet Foreground%d : WINDOW %d, BPP: %s >\n", i, i, pcBppStr_All[i]);
			}
		}
		UART_Printf("========================================================================\n");			
		UART_Printf("\nSelect the function to test : ");
		nSel = UART_GetIntNum();
		UART_Printf("\n");

		if (nSel == 0)
			break;
		if (nSel>0 && nSel<(sizeof(lcd_cpuif_function)/8-1))
			 (lcd_cpuif_function[nSel].func)();
	}	
	
} 

