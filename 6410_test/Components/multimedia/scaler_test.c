/*----------------------------------------------------------------------
 *
 * Filename: scaler_test.cpp
 *
 * Contents: Testing application of "class POST"
 *
 * Authors: 
 *
 * Notes:
 *
 * Copyright (c) 2003 SAMSUNG Electronics.
 *
 *----------------------------------------------------------------------
 */

 #include <stdio.h>

#include "def.h"
#include "option.h"
#include "library.h"
#include "sfr6410.h"
#include "system.h"
#include "sysc.h"
#include "post.h"
#include "lcd.h"
#include "glib.h"
#include "intc.h"
#include "camera.h"
#include "timer.h"
#include "post_test.h"
#include "tvenc.h"



#if 0
#include "Smile800480_24bpp2.h"
#include "bgcolors216.h"

#endif

static POST oPost;
static POST oSc;

static CSPACE eSrcDataFmt;
static CSPACE eMidDataFmt;
static CSPACE eDstDataFmt;

static CSPACE eSrcDataFmt_Yuv;
static CSPACE eDstDataFmt_Yuv;


static POST_RUN_MODE ePostRunMode;

static u32 uLcdHSz, uLcdVSz;	
static u32 uSrcStartX, uSrcStartY;
static u32 uSrcCroppedHSz, uSrcCroppedVSz;

static u32 uMidImgHSz, uMidImgVSz;
static u32 uMidStartX, uMidStartY;
static u32 uMidScaledHSz, uMidScaledVSz;

static u32 uMid2ImgHSz, uMid2ImgVSz;
static u32 uMid2StartX, uMid2StartY;
static u32 uMid2ScaledHSz, uMid2ScaledVSz;

static u32 uImgHSz, uImgVSz;	
static u32 uDstStartX, uDstStartY;
static u32 uDstScaledHSz, uDstScaledVSz;


const static u32 uLcdStAddr = CODEC_MEM_ST; 

static u32 uNxtAddr1, uNxtAddr2;

const static u32 uMaxFrameCnt = 10;

u8 bScalerIsPollingOrInt = true; // true: Polling, false: Int

static u32 uSimpleTest;
static u8 uScalerIntOccured;

u8 *Scaler_Image_Entertain[] = {	"D:/MyWork/MyProject/S3C6410/SW/TestCode/6410_Test_Rev02/Components/multimedia/image/bmp/entertainment1.bmp",
								"D:/MyWork/MyProject/S3C6410/SW/TestCode/6410_Test_Rev02/Components/multimedia/image/bmp/entertainment2.bmp",
								"D:/MyWork/MyProject/S3C6410/SW/TestCode/6410_Test_Rev02/Components/multimedia/image/bmp/entertainment3.bmp",
								"D:/MyWork/MyProject/S3C6410/SW/TestCode/6410_Test_Rev02/Components/multimedia/image/bmp/entertainment4.bmp",
								"D:/MyWork/MyProject/S3C6410/SW/TestCode/6410_Test_Rev02/Components/multimedia/image/bmp/entertainment5.bmp",
								"D:/MyWork/MyProject/S3C6410/SW/TestCode/6410_Test_Rev02/Components/multimedia/image/bmp/entertainment6.bmp",
								"D:/MyWork/MyProject/S3C6410/SW/TestCode/6410_Test_Rev02/Components/multimedia/image/bmp/entertainment7.bmp",
								"D:/MyWork/MyProject/S3C6410/SW/TestCode/6410_Test_Rev02/Components/multimedia/image/bmp/entertainment8.bmp",
								"D:/MyWork/MyProject/S3C6410/SW/TestCode/6410_Test_Rev02/Components/multimedia/image/bmp/entertainment9.bmp",
								"D:/MyWork/MyProject/S3C6410/SW/TestCode/6410_Test_Rev02/Components/multimedia/image/bmp/entertainment10.bmp",
								"D:/MyWork/MyProject/S3C6410/SW/TestCode/6410_Test_Rev02/Components/multimedia/image/bmp/entertainment11.bmp",
								"D:/MyWork/MyProject/S3C6410/SW/TestCode/6410_Test_Rev02/Components/multimedia/image/bmp/entertainment12.bmp",
								"D:/MyWork/MyProject/S3C6410/SW/TestCode/6410_Test_Rev02/Components/multimedia/image/bmp/entertainment13.bmp",
								"D:/MyWork/MyProject/S3C6410/SW/TestCode/6410_Test_Rev02/Components/multimedia/image/bmp/entertainment14.bmp",
								"D:/MyWork/MyProject/S3C6410/SW/TestCode/6410_Test_Rev02/Components/multimedia/image/bmp/entertainment15.bmp",
								"D:/MyWork/MyProject/S3C6410/SW/TestCode/6410_Test_Rev02/Components/multimedia/image/bmp/Illustrate1.bmp",
								"D:/MyWork/MyProject/S3C6410/SW/TestCode/6410_Test_Rev02/Components/multimedia/image/bmp/Illustrate2.bmp",
								"D:/MyWork/MyProject/S3C6410/SW/TestCode/6410_Test_Rev02/Components/multimedia/image/bmp/Illustrate3.bmp",
								"D:/MyWork/MyProject/S3C6410/SW/TestCode/6410_Test_Rev02/Components/multimedia/image/bmp/Illustrate4.bmp",
								"D:/MyWork/MyProject/S3C6410/SW/TestCode/6410_Test_Rev02/Components/multimedia/image/bmp/Illustrate5.bmp",
								"D:/MyWork/MyProject/S3C6410/SW/TestCode/6410_Test_Rev02/Components/multimedia/image/bmp/Illustrate6.bmp",
								"D:/MyWork/MyProject/S3C6410/SW/TestCode/6410_Test_Rev02/Components/multimedia/image/bmp/Illustrate7.bmp",
								"D:/MyWork/MyProject/S3C6410/SW/TestCode/6410_Test_Rev02/Components/multimedia/image/bmp/Illustrate8.bmp",
								"D:/MyWork/MyProject/S3C6410/SW/TestCode/6410_Test_Rev02/Components/multimedia/image/bmp/Illustrate9.bmp"
							};
////////////////////////////////////////////////////////////////////////////
//////////////////                         ISR Routine                         ///////////////////
////////////////////////////////////////////////////////////////////////////

void __irq ScalerISRDma(void)
{
	uScalerIntOccured = TRUE;
	POST_ClearPending(&oSc);
	INTC_ClearVectAddr();
}



//////////
// Function Name : ScalerT_SelectClockDivider
// Function Description : Set POST Clock Divider
// Input : 	None
// Output : 	None
void ScalerT_SelectClockDivider(void)
{
	u32 uPostClockDivide;
	
	UART_Printf("[ScalerT_SelectClockDivider]\n");

	UART_Printf("\n");
	UART_Printf("Select the source clock [0x01 ~ 0x3F] : ");
	
	uPostClockDivide = (u32)UART_GetIntNum();

	POST_SetClockDivide(uPostClockDivide, &oSc);
}


#if (POST_IMAGE_SOURCE == POST_IMAGE_HEADER)
static void ScalerT_GetImageFromHeader(u32 uFbAddr, u32 uHSize, u32 uVSize, CSPACE eBpp)
{
	u32 i;
	u16 *pBufAddr16;
	u32 *pBufAddr32;

	pBufAddr16 = (u16 *)uFbAddr;
	pBufAddr32 = (u32 *)uFbAddr;

	switch(eBpp)
	{
		case RGB16 : 	for(i=0 ; i<uHSize*uVSize ; i++)
						{
							//*pBufAddr16++ = pGuitar240320_16rgb[i];
							//*pBufAddr16++ = pRacingGirl320240[i];
							*pBufAddr16++ = sun320240_16bpp[i];
						}
						break;
		case RGB24 : 	for(i=0 ; i<uHSize*uVSize ; i++)
						{
							//*pBufAddr32++ = pGuitar240320_24rgb[i];
							//*pBufAddr32++ = pYepp320x240[i];
							*pBufAddr32++ = pSmile_2[i];
						}
						break;
		case YC420 : 	for(i=0 ; i<uHSize*uVSize ; i++)
						{
							*pBufAddr16++ = pGuitar240320_YCbYCr420[i];
						}
						break;
		case YCBYCR : 	for(i=0 ; i<uHSize*uVSize ; i++)
						{
							*pBufAddr16++ = pGuitar240320_YCbYCr422[i];
						}
						break;
		case YCRYCB : 	for(i=0 ; i<uHSize*uVSize ; i++)
						{
							*pBufAddr16++ = pGuitar240320_YCrYCb422[i];
						}
						break;
		case CBYCRY : 	for(i=0 ; i<uHSize*uVSize ; i++)
						{
							*pBufAddr16++ = pGuitar240320_CbYCrY422[i];
						}
						break;
		case CRYCBY : 	for(i=0 ; i<uHSize*uVSize ; i++)
						{
							*pBufAddr16++ = pGuitar240320_CrYCbY422[i];
						}
						break;
		default	: 		break;
	}

}
#endif


static void ScalerT_GetImageFromBMP(u32 uFbAddr, u32 uHSize, u32 uVSize, u32 NumOfImage, CSPACE eBpp)
{
	u32 ImageNum;
	u16 *pBufAddr16;
	u32 *pBufAddr32;
	u32 *uBuffer_Temp;

//	uBuffer_Temp = (u32 *)malloc(300000);		//320x240 bmp file
	uBuffer_Temp = (u32 *)malloc(1500000);		//800x480 bmp file
	pBufAddr16 = (u16 *)uFbAddr;
	pBufAddr32 = (u32 *)uFbAddr;
	ImageNum = NumOfImage%(sizeof(Scaler_Image_Entertain)/4);
	
	switch(eBpp)
	{
		case RGB16 : 	LoadFromFile((const char*)Scaler_Image_Entertain[ImageNum], (u32)uBuffer_Temp);
						ConvertBmpToRgb16bpp((u32)uBuffer_Temp, (u32)pBufAddr16, uHSize, uVSize);
						break;
		case RGB24 : 	LoadFromFile((const char*)Scaler_Image_Entertain[ImageNum], (u32)uBuffer_Temp);
						ConvertBmpToRgb24bpp((u32)uBuffer_Temp, (u32)pBufAddr32, uHSize, uVSize);
						break;
		default	: 		break;
	}

	free(uBuffer_Temp);
}


////////////////////////////////////////////////////////////////////////////
//////////////////                       RGB to RGB                           ///////////////////
////////////////////////////////////////////////////////////////////////////

static void ScalerT_SetCscTypeDmaToDma_R2R(void)
{
	u32 uSelDmaCscType_R2R;

	while(1)
	{
		UART_Printf("\n");
		UART_Printf("[1] From RGB16 To RGB16\n");
		UART_Printf("[2] From RGB16 To RGB24\n");
		UART_Printf("[3] From RGB24 To RGB16\n");		
		UART_Printf("[4] From RGB24 To RGB24\n");
		UART_Printf("\n");

		UART_Printf(">>Select The Color Space Conversion Type: ");

		uSelDmaCscType_R2R = UART_GetIntNum();

		if (uSelDmaCscType_R2R == 1 || uSelDmaCscType_R2R == 2 || uSelDmaCscType_R2R == 3 || uSelDmaCscType_R2R == 4)
		{
			eSrcDataFmt = 	(uSelDmaCscType_R2R == 1 || uSelDmaCscType_R2R == 2) ? RGB16 : RGB24;
			eDstDataFmt = (uSelDmaCscType_R2R == 1 || uSelDmaCscType_R2R == 3) ? RGB16 : RGB24;

			break;
		}
		else 
			UART_Printf("Invalid Input! Retry It!!\n");
		
	}

	if (eSrcDataFmt == RGB16 && eDstDataFmt == RGB16)
	{
		// Scale up(width, height)
		uImgHSz = 120, 	uImgVSz = 160;
		uSrcStartX = 20, uSrcStartY = 30;
		uSrcCroppedHSz = 88, uSrcCroppedVSz = 100;

		uDstStartX = 40, uDstStartY = 80;
		uDstScaledHSz = 160, uDstScaledVSz = 160;

	}
	else if (eSrcDataFmt == RGB16 && eDstDataFmt == RGB24)
	{
		// Scale up(width), scale down(height)
		uImgHSz = 240,	uImgVSz = 320;
		uSrcStartX = 80, uSrcStartY = 160;
		uSrcCroppedHSz = 120,	uSrcCroppedVSz = 160;

		uDstStartX = 20, uDstStartY = 40;
		uDstScaledHSz = 200, uDstScaledVSz = 120;		
	}
	else if (eSrcDataFmt == RGB24 && eDstDataFmt == RGB16)
	{
		// scale down(width), scale up(height)
		uImgHSz = 360,	uImgVSz = 480;
		uSrcStartX = 30, uSrcStartY = 40;
		uSrcCroppedHSz = 160, uSrcCroppedVSz = 100;

		uDstStartX = 10, uDstStartY = 50;
		uDstScaledHSz = 120, uDstScaledVSz = 190;
	}
	else if (eSrcDataFmt == RGB24 && eDstDataFmt == RGB24)
	{
		// scale down(widht, height)
		uImgHSz = 360,	uImgVSz = 480;
		uSrcStartX = 80, uSrcStartY = 20;
		uSrcCroppedHSz = 168,	uSrcCroppedVSz = 240;

		uDstStartX = 100, uDstStartY = 100;
		uDstScaledHSz = 128, uDstScaledVSz = 140;
	}
	else
	{
		UART_Printf("Error! Unsupported Data Format!\n");
		return;
	}

	UART_Printf("\n");
	if(uSimpleTest == 0)
	{
		UART_Printf("=========    Complex_Test Image Size    =========\n");
		UART_Printf("SrcImgHSz     = %d,       SrcImgVSz     = %d\n",uImgHSz,uImgVSz);
		UART_Printf("SrcStartX     = %d,       SrcStartY     = %d\n",uSrcStartX,uSrcStartY);
		UART_Printf("SrcCroppedHSz = %d,       SrcCroppedVSz = %d\n",uSrcCroppedHSz,uSrcCroppedVSz);
		UART_Printf("DstStartX     = %d,       DstStartY     = %d\n",uDstStartX,uDstStartY);
		UART_Printf("DstScaledHSz  = %d,       DstScaledVSz  = %d\n",uDstScaledHSz,uDstScaledVSz);	
		UART_Printf("=================================================\n");
		UART_Printf("\n");
	}
}


static void ScalerT_SimpleDmaToDma_R2R(void)
{
	u32 uLcdHSz, uLcdVSz;
	u32 uGlibStAddr;
	u32 uBytesPerPixel;

	UART_Printf("[ScalerT_SimpleDmaToDma_R2R(No Scaling)]\n");

	uSimpleTest = 1;
	ScalerT_SetCscTypeDmaToDma_R2R();
	
	LCD_InitDISPC(eDstDataFmt, uLcdStAddr, WIN0, false);
	LCD_Start();
	LCD_SetWinOnOff(1, WIN0);
	LCD_GetFrmSz(&uLcdHSz, &uLcdVSz, WIN0);
	
	// To Clear the LCD Display
	GLIB_Init(uLcdStAddr, uLcdHSz, uLcdVSz, eDstDataFmt);
	GLIB_ClearFrame(C_BLACK);
	
	uBytesPerPixel = (eDstDataFmt == RGB16) ? 2: 4;
	uGlibStAddr = uLcdStAddr + uLcdHSz*uLcdVSz*uBytesPerPixel;

#if (POST_IMAGE_SOURCE == POST_IMAGE_PATTERN)
		GLIB_Init(uGlibStAddr, uLcdHSz, uLcdVSz, eSrcDataFmt);
		GLIB_DrawPattern(uLcdHSz, uLcdVSz);
#elif (POST_IMAGE_SOURCE == POST_IMAGE_HEADER)
		ScalerT_GetImageFromHeader(uGlibStAddr, uLcdHSz, uLcdVSz, eSrcDataFmt);
#endif
	POST_InitIpForDmaInDmaOut(	uLcdHSz, uLcdVSz, uGlibStAddr, eSrcDataFmt, 
									uLcdHSz, uLcdVSz, uLcdStAddr, eDstDataFmt,
									1, false, ONE_SHOT, &oSc );
	POST_StartProcessing(&oSc);
	while (!POST_IsProcessingDone(&oSc));	

#if (POST_IMAGE_BMP_SLIDE)	
	{
		u32 uImageNum;
		UART_Printf("Input the any key to display the other image ('q' : quit)\n");
		uImageNum = 0;

		while(UART_Getc() != 'q')
		{
			ScalerT_GetImageFromBMP(uGlibStAddr, uLcdHSz, uLcdVSz, uImageNum, eSrcDataFmt);

			// Only CSC operation, not Scaling
			POST_InitIpForDmaInDmaOut(	uLcdHSz, uLcdVSz, uGlibStAddr, eSrcDataFmt, 
											uLcdHSz, uLcdVSz, uLcdStAddr, eDstDataFmt,
											1, false, ONE_SHOT, &oSc );
			POST_StartProcessing(&oSc);
			while (!POST_IsProcessingDone(&oSc));	
			uImageNum++;
		}
	}
#endif	
}


static void ScalerT_ComplexIDmaToDma_R2R(void)
{
	u32 uGlibStAddr=0;
	u32 uBytesPerPixel=0;
	u32 uSrcStAddr;

	UART_Printf("[ScalerT_ComplexIDmaToDma_R2R]\n");

	uSimpleTest = 0;
	ScalerT_SetCscTypeDmaToDma_R2R();
	
	ePostRunMode = ONE_SHOT;
	
	//LCD_InitLDI(MAIN);
#ifdef LCD_INIT_TEST	
	LCD_InitBase();
	LCD_Start();
	LCD_InitWin(eDstDataFmt, uImgHSz, uImgVSz, 0, 0, uImgHSz, uImgVSz, 0, 0, uLcdStAddr, WIN0, false);
#else
	LCD_InitDISPC(eDstDataFmt, uLcdStAddr, WIN0, false);
	LCD_Start();
#endif
	LCD_SetWinOnOff(1, WIN0);
	LCD_GetFrmSz(&uLcdHSz, &uLcdVSz, WIN0);

	// To Clear the LCD Display
	GLIB_Init(uLcdStAddr, uLcdHSz, uLcdVSz, eDstDataFmt);
	GLIB_ClearFrame(C_BLACK);

	uBytesPerPixel = (eDstDataFmt == RGB16) ? 2: 4;
	uGlibStAddr = uLcdStAddr + uLcdHSz*uLcdVSz*uBytesPerPixel;
	uSrcStAddr=0;

	// CSC & Scaling the DrawPattern 1
#if (POST_IMAGE_SOURCE == POST_IMAGE_PATTERN)
	GLIB_Init(uGlibStAddr, uImgHSz, uImgVSz, eSrcDataFmt);
	GLIB_DrawPattern(uImgHSz, uImgVSz);
#elif (POST_IMAGE_SOURCE == POST_IMAGE_HEADER)
	ScalerT_GetImageFromHeader(uGlibStAddr, uLcdHSz, uLcdVSz, eSrcDataFmt);
#endif

	POST_InitIp1(	uImgHSz, uImgVSz, uSrcStartX, uSrcStartY, uSrcCroppedHSz, uSrcCroppedVSz, uGlibStAddr, eSrcDataFmt,
					uLcdHSz, uLcdVSz, uDstStartX, uDstStartY, uDstScaledHSz, uDstScaledVSz, uLcdStAddr, eDstDataFmt,
					2, false, ePostRunMode, POST_DMA, POST_DMA, &oSc);

	UART_Printf("Draw Pattern 1(Frame 0)\n");
	POST_StartProcessing1(0, 0, &oSc);
	while (!POST_IsProcessingDone(&oSc));

	UART_Printf("Enter Any Key\n");
	UART_Getc();
	
	// CSC & Scaling the DrawPattern 2	
	POST_GetSrcStAddr(1, &uSrcStAddr, &oSc);
	GLIB_Init(uSrcStAddr, uImgHSz, uImgVSz, eSrcDataFmt);
	GLIB_DrawPattern2(uImgHSz, uImgVSz);

	UART_Printf("Draw Pattern 2(Frame 1)\n");	
	POST_StartProcessing1(1, 0, &oSc);
	while (!POST_IsProcessingDone(&oSc));	
}


static void ScalerT_SimpleFreeRunDmaToDma_R2R_Polling(void)
{
	u32 uGlibStAddr=0, uGlibStAddr2=0;
	u32 uBytesPerPixel=0;
	u32 uFrmCnt = 0;

	UART_Printf("[ScalerT_SimpleFreeRunDmaToDma_R2R_Polling]\n");

	uSimpleTest = 1;
	ScalerT_SetCscTypeDmaToDma_R2R();

	bScalerIsPollingOrInt = TRUE; // Polling

	//LCD_InitLDI(MAIN);
#ifdef LCD_INIT_TEST	
	LCD_InitBase();
	LCD_Start();
	LCD_InitWin(eDstDataFmt, uImgHSz, uImgVSz, 0, 0, uImgHSz, uImgVSz, 0, 0, uLcdStAddr, WIN0, false);
#else
	LCD_InitDISPC(eDstDataFmt, uLcdStAddr, WIN0, false);
	LCD_Start();
#endif
	LCD_SetWinOnOff(1, WIN0);
	LCD_GetFrmSz(&uLcdHSz, &uLcdVSz, WIN0);

	//uBytesPerPixel = (eDstDataFmt == RGB16) ? 2: 4;
	uBytesPerPixel = 4;
	uGlibStAddr = uLcdStAddr + uLcdHSz*uLcdVSz*uBytesPerPixel;
	uGlibStAddr2 = uGlibStAddr + uLcdHSz*uLcdVSz*uBytesPerPixel;

	GLIB_Init(uGlibStAddr, uLcdHSz, uLcdVSz, eSrcDataFmt);
	GLIB_DrawPattern(uLcdHSz, uLcdVSz);

	GLIB_Init(uGlibStAddr2, uLcdHSz, uLcdVSz, eSrcDataFmt);
	GLIB_DrawPattern2(uLcdHSz, uLcdVSz);

	POST_InitIpForDmaInDmaOut(	uLcdHSz, uLcdVSz, uGlibStAddr, eSrcDataFmt, 
									uLcdHSz, uLcdVSz, uLcdStAddr, eDstDataFmt,
									1, false, FREE_RUN, &oSc);
	POST_SetNextFrameStAddr(uGlibStAddr, uLcdStAddr, &oSc);

	uFrmCnt = 0;

	POST_StartProcessing(&oSc);

	while(uFrmCnt<uMaxFrameCnt)
	{
		while (!POST_IsProcessingDone(&oSc));			
		
		if (uFrmCnt%2)
			POST_SetNextFrameStAddr(uGlibStAddr2, uLcdStAddr, &oSc);
		else
			POST_SetNextFrameStAddr(uGlibStAddr, uLcdStAddr, &oSc);
		POST_ClearPending(&oSc);

		DelayfrTimer(milli, 1000);
		uFrmCnt++;
	}

	POST_StopProcessingOfFreeRun(&oSc);
	while (!POST_IsProcessingDone(&oSc));	
	POST_ClearPending(&oSc);

#if (POST_IMAGE_BMP_SLIDE)	
	ScalerT_GetImageFromBMP(uGlibStAddr, uLcdHSz, uLcdVSz, 8, eSrcDataFmt);
	ScalerT_GetImageFromBMP(uGlibStAddr2, uLcdHSz, uLcdVSz, 9, eSrcDataFmt);

	POST_InitIpForDmaInDmaOut(	uLcdHSz, uLcdVSz, uGlibStAddr, eSrcDataFmt, 
									uLcdHSz, uLcdVSz, uLcdStAddr, eDstDataFmt,
									1, false, FREE_RUN, &oSc);
	POST_SetNextFrameStAddr(uGlibStAddr, uLcdStAddr, &oSc);

	uFrmCnt = 0;

	POST_StartProcessing(&oSc);

	while(uFrmCnt<uMaxFrameCnt)
	{
		while (!POST_IsProcessingDone(&oSc));			
		
		if (uFrmCnt%2)
			POST_SetNextFrameStAddr(uGlibStAddr2, uLcdStAddr, &oSc);
		else
			POST_SetNextFrameStAddr(uGlibStAddr, uLcdStAddr, &oSc);
		POST_ClearPending(&oSc);

		DelayfrTimer(milli, 1000);
		uFrmCnt++;
	}

	POST_StopProcessingOfFreeRun(&oSc);
	while (!POST_IsProcessingDone(&oSc));	
	POST_ClearPending(&oSc);	
#endif	
}


void ScalerT_SimpleFreeRunDmaToDma_R2R_Int(void)
{
	u32 uBytesPerPixel=0;
	u32 uPostIsrCount, uPostFrameCount;
	
	UART_Printf("[ScalerT_SimpleFreeRunDmaToDma_R2R_Int]\n");

	uSimpleTest = 1;
	ScalerT_SetCscTypeDmaToDma_R2R();
	
	bScalerIsPollingOrInt = TRUE; // Int

	//LCD_InitLDI(MAIN);
#ifdef LCD_INIT_TEST	
	LCD_InitBase();
	LCD_Start();
	LCD_InitWin(eDstDataFmt, uImgHSz, uImgVSz, 0, 0, uImgHSz, uImgVSz, 0, 0, uLcdStAddr, WIN0, false);
#else
	LCD_InitDISPC(eDstDataFmt, uLcdStAddr, WIN0, false);
	LCD_Start();
#endif
	LCD_SetWinOnOff(1, WIN0);
	LCD_GetFrmSz(&uLcdHSz, &uLcdVSz, WIN0);

	//uBytesPerPixel = (eDstDataFmt == RGB16) ? 2: 4;
	uBytesPerPixel = 4;
	uNxtAddr1 = uLcdStAddr + uLcdHSz*uLcdVSz*uBytesPerPixel;
	uNxtAddr2 = uNxtAddr1 + uLcdHSz*uLcdVSz*uBytesPerPixel;

	GLIB_Init(uNxtAddr1, uLcdHSz, uLcdVSz, eSrcDataFmt);
	GLIB_DrawPattern(uLcdHSz, uLcdVSz);

	GLIB_Init(uNxtAddr2, uLcdHSz, uLcdVSz, eSrcDataFmt);
	GLIB_DrawPattern2(uLcdHSz, uLcdVSz);

	//Interrupt Setting
	uPostIsrCount = 0;
	uPostFrameCount = 0;
	uScalerIntOccured = FALSE;

	INTC_Enable(NUM_SCALER);
	INTC_SetVectAddr(NUM_SCALER, ScalerISRDma);
	POST_EnableInterrupt(POST_LEVEL_INT, &oSc);

	POST_InitIpForDmaInDmaOut(	uLcdHSz, uLcdVSz, uNxtAddr1, eSrcDataFmt,
									uLcdHSz, uLcdVSz, uLcdStAddr, eDstDataFmt,
									1, false, FREE_RUN, &oSc);
	POST_SetNextFrameStAddr(uNxtAddr1, uLcdStAddr, &oSc);

	POST_StartProcessing(&oSc);

	while(uPostFrameCount < uMaxFrameCnt)
	{
		if(uScalerIntOccured == TRUE)
		{
			uPostIsrCount++;
			if(uPostIsrCount > 200)
			{
				uPostFrameCount++;
				
			 	if (uPostFrameCount % 2)
					POST_SetNextFrameStAddr(uNxtAddr2, uLcdStAddr, &oSc);
				else 
					POST_SetNextFrameStAddr(uNxtAddr1, uLcdStAddr, &oSc);

				if (uPostFrameCount == uMaxFrameCnt)
				{
					POST_StopProcessingOfFreeRun(&oSc);	
					while (!POST_IsProcessingDone(&oSc));	
					uScalerIntOccured = FALSE;
					break;
				}
				uPostIsrCount = 0;
			}
			uScalerIntOccured = FALSE;
		}
	}

#if (POST_IMAGE_BMP_SLIDE)	
	ScalerT_GetImageFromBMP(uNxtAddr1, uLcdHSz, uLcdVSz, 11, eSrcDataFmt);
	ScalerT_GetImageFromBMP(uNxtAddr2, uLcdHSz, uLcdVSz, 12, eSrcDataFmt);

	POST_InitIpForDmaInDmaOut(	uLcdHSz, uLcdVSz, uNxtAddr1, eSrcDataFmt,
									uLcdHSz, uLcdVSz, uLcdStAddr, eDstDataFmt,
									1, false, FREE_RUN, &oSc);
	POST_SetNextFrameStAddr(uNxtAddr1, uLcdStAddr, &oSc);

	uPostFrameCount = 0;
	uPostIsrCount = 0;
	POST_StartProcessing(&oSc);

	while(uPostFrameCount<uMaxFrameCnt)
	{
		if(uScalerIntOccured == TRUE)
		{
			uPostIsrCount++;
			if(uPostIsrCount > 200)
			{
				uPostFrameCount++;
				
			 	if (uPostFrameCount % 2)
					POST_SetNextFrameStAddr(uNxtAddr2, uLcdStAddr, &oSc);
				else 
					POST_SetNextFrameStAddr(uNxtAddr1, uLcdStAddr, &oSc);

				if (uPostFrameCount == uMaxFrameCnt)
				{
					POST_StopProcessingOfFreeRun(&oSc);	
					while (!POST_IsProcessingDone(&oSc));	
					uScalerIntOccured = FALSE;
					break;
				}
				uPostIsrCount = 0;
			}
			uScalerIntOccured = FALSE;
		}

	}
#endif	

	POST_DisableInterrupt(&oSc);
	POST_ClearPending(&oSc);	
	INTC_Disable(NUM_SCALER);
}



void ScalerT_ComplexFreeRunDmaToDma_R2R_Polling(void)
{
	u32 uGlibStAddr, uGlibStAddr2;
	u32 uBytesPerPixel, uFrmCnt;
	
	UART_Printf("[ScalerT_ComplexFreeRunDmaToDma_R2R_Polling]\n");

	uSimpleTest = 0;
	ScalerT_SetCscTypeDmaToDma_R2R();

	bScalerIsPollingOrInt = TRUE;
	ePostRunMode = FREE_RUN;
	
	//LCD_InitLDI(MAIN);
#ifdef LCD_INIT_TEST	
	LCD_InitBase();
	LCD_Start();
	LCD_InitWin(eDstDataFmt, uImgHSz, uImgVSz, 0, 0, uImgHSz, uImgVSz, 0, 0, uLcdStAddr, WIN0, false);
#else
	LCD_InitDISPC(eDstDataFmt, uLcdStAddr, WIN0, false);
	LCD_Start();
#endif
	LCD_SetWinOnOff(1, WIN0);
	LCD_GetFrmSz(&uLcdHSz, &uLcdVSz, WIN0);

	// To Clear the LCD Display
	GLIB_Init(uLcdStAddr, uLcdHSz, uLcdVSz, eDstDataFmt);
	GLIB_ClearFrame(C_BLACK);

	uBytesPerPixel = (eDstDataFmt == RGB16) ? 2: 4;
	uGlibStAddr = uLcdStAddr + uLcdHSz*uLcdVSz*uBytesPerPixel;
	uGlibStAddr2 = uGlibStAddr + uImgHSz*uImgVSz*uBytesPerPixel;

	GLIB_Init(uGlibStAddr, uImgHSz, uImgVSz, eSrcDataFmt);
	GLIB_DrawPattern(uImgHSz, uImgVSz);

	GLIB_Init(uGlibStAddr2, uImgHSz, uImgVSz, eSrcDataFmt);
	GLIB_DrawPattern2(uImgHSz, uImgVSz);

	POST_InitIp1(	uImgHSz, uImgVSz, uSrcStartX, uSrcStartY, uSrcCroppedHSz, uSrcCroppedVSz, uGlibStAddr, eSrcDataFmt,
					uLcdHSz, uLcdVSz, uDstStartX, uDstStartY, uDstScaledHSz, uDstScaledVSz, uLcdStAddr, eDstDataFmt, 
					1, false, ePostRunMode, POST_DMA, POST_DMA, &oSc); // src. frm. buf. num: 1, double buffering: false

	POST_SetNextFrameStAddr(uGlibStAddr2, uLcdStAddr, &oSc);
	POST_StartProcessing(&oSc);

	uFrmCnt = 0;

	while(uFrmCnt<uMaxFrameCnt)
	{
		while (!POST_IsProcessingDone(&oSc));			
		
		if (uFrmCnt%2)
			POST_SetNextFrameStAddr(uGlibStAddr, uLcdStAddr, &oSc);
		else
			POST_SetNextFrameStAddr(uGlibStAddr2, uLcdStAddr, &oSc);
		POST_ClearPending(&oSc);

		DelayfrTimer(milli, 500);
		uFrmCnt++;
	}

	POST_StopProcessingOfFreeRun(&oSc);	
	while (!POST_IsProcessingDone(&oSc));	
}


void ScalerT_ComplexFreeRunDmaToDma_R2R_Int(void)
{
	u32 uBytesPerPixel=0;
	u32 uPostIsrCount, uPostFrameCount;

	UART_Printf("[ScalerT_ComplexFreeRunDmaToDma_R2R_Int]\n");

	uSimpleTest = 0;
	ScalerT_SetCscTypeDmaToDma_R2R();
	
	bScalerIsPollingOrInt = false;	
	ePostRunMode = FREE_RUN;
	
	//LCD_InitLDI(MAIN);
	
#ifdef LCD_INIT_TEST	
	LCD_InitBase();
	LCD_Start();
	LCD_InitWin(eDstDataFmt, uImgHSz, uImgVSz, 0, 0, uImgHSz, uImgVSz, 0, 0, uLcdStAddr, WIN0, false);
#else
	LCD_InitDISPC(eDstDataFmt, uLcdStAddr, WIN0, false);
	LCD_Start();
#endif

	LCD_SetWinOnOff(1, WIN0);

	LCD_GetFrmSz(&uLcdHSz, &uLcdVSz, WIN0);

	// To Clear the LCD Display
	GLIB_Init(uLcdStAddr, uLcdHSz, uLcdVSz, eDstDataFmt);
	GLIB_ClearFrame(C_BLACK);

	uBytesPerPixel = (eDstDataFmt == RGB16) ? 2: 4;

	uNxtAddr1 = uLcdStAddr + uLcdHSz*uLcdVSz*uBytesPerPixel;
	uNxtAddr2 = uNxtAddr1 + uImgHSz*uImgVSz*uBytesPerPixel;

	GLIB_Init(uNxtAddr1, uImgHSz, uImgVSz, eSrcDataFmt);
	GLIB_DrawPattern(uImgHSz, uImgVSz);

	GLIB_Init(uNxtAddr2, uImgHSz, uImgVSz, eSrcDataFmt);
	GLIB_DrawPattern2(uImgHSz, uImgVSz);

	uPostIsrCount = 0;
	uPostFrameCount = 0;
	uScalerIntOccured = FALSE;
	//INTC_InitIp();
	//INTC_SetHandlerAndUnmask(INT_POST, ScalerISRDma);
	INTC_Enable(NUM_SCALER);
	INTC_SetVectAddr(NUM_SCALER, ScalerISRDma);
	POST_EnableInterrupt(POST_LEVEL_INT, &oSc);

	POST_InitIp1(	uImgHSz, uImgVSz, uSrcStartX, uSrcStartY, uSrcCroppedHSz, uSrcCroppedVSz, uNxtAddr1, eSrcDataFmt,
					uLcdHSz, uLcdVSz, uDstStartX, uDstStartY, uDstScaledHSz, uDstScaledVSz, uLcdStAddr, eDstDataFmt,
					1, false, ePostRunMode, POST_DMA, POST_DMA, &oSc);

	POST_SetNextFrameStAddr(uNxtAddr2, uLcdStAddr, &oSc);

	POST_StartProcessing(&oSc);

	while(uPostFrameCount < uMaxFrameCnt)
	{
		if(uScalerIntOccured == TRUE)
		{
			uPostIsrCount++;
			if(uPostIsrCount == 400)
			{
				uPostFrameCount++;
				
			 	if (uPostFrameCount % 2)
					POST_SetNextFrameStAddr(uNxtAddr2, uLcdStAddr, &oSc);
				else 
					POST_SetNextFrameStAddr(uNxtAddr1, uLcdStAddr, &oSc);

				if (uPostFrameCount == uMaxFrameCnt)
				{
					POST_StopProcessingOfFreeRun(&oSc);	
					while (!POST_IsProcessingDone(&oSc));	
					uScalerIntOccured = FALSE;
					break;
				}
				uPostIsrCount = 0;
			}
			uScalerIntOccured = FALSE;
		}
	}

	POST_DisableInterrupt(&oSc);
	POST_ClearPending(&oSc);	
	//INTC_Mask(INT_POST);
	INTC_Disable(NUM_SCALER);
}


void ScalerT_ScaleUpDown_RGB(void)
{
	u32 uGlibStAddr, uGlibStAddr1;
	u32 uBytesPerPixel;
	u32 uScaleOffsetX, uScaleOffsetY;
	u8 uScale;
	u32 uHOffset, uVOffset, uPreHOffset, uPreVOffset, uHRatio, uVRatio;
	s32 nSelDmaCscType_R2R;	

	u32 i;
	u16 *pBufAddr16;
	u32 *pBufAddr32;

	UART_Printf("[ScalerT_ScaleUpDown_RGB]\n");

	while(1)
	{
		UART_Printf("\n");
		UART_Printf("[1] RGB16\n");
		UART_Printf("[2] RGB24\n");
		UART_Printf("\n");

		UART_Printf(">>Select The Source Image Type: ");

		nSelDmaCscType_R2R = UART_GetIntNum();

		if (nSelDmaCscType_R2R == 1 || nSelDmaCscType_R2R == 2 )
		{
			eSrcDataFmt = (nSelDmaCscType_R2R == 1) ? RGB16 : RGB24;
			eDstDataFmt = (nSelDmaCscType_R2R == 2) ? RGB16 : RGB24;
			break;
		}
		else 
			UART_Printf("Invalid Input! Retry It!!\n");
		
	}

	ePostRunMode = ONE_SHOT;

	//LCD_InitLDI(MAIN);
	LCD_InitDISPC(eSrcDataFmt, uLcdStAddr, WIN0, false);
	LCD_Start();
	LCD_SetWinOnOff(1, WIN0);
	LCD_GetFrmSz(&uLcdHSz, &uLcdVSz, WIN0);

	uImgHSz = uLcdHSz;	
	uImgVSz = uLcdVSz;
	uSrcStartX = 0; 
	uSrcStartY = 0;
	uSrcCroppedHSz = uLcdHSz;	
	uSrcCroppedVSz = uLcdVSz;
	
	uMidImgHSz = 2048;
	uMidImgVSz = 2048;
	uMidStartX = 0;
	uMidStartY = 0;
	uMidScaledHSz = 2048;
	uMidScaledVSz = 2048;
		
	uDstStartX = 0; 
	uDstStartY = 0;
	uDstScaledHSz = uLcdHSz, 
	uDstScaledVSz = uLcdVSz;	
	
	// To Clear the LCD Display
	GLIB_Init(uLcdStAddr, uLcdHSz, uLcdVSz, eSrcDataFmt);
	GLIB_ClearFrame(C_BLACK);

	uBytesPerPixel = (eSrcDataFmt == RGB16) ? 2: 4;
	uGlibStAddr = uLcdStAddr + uLcdHSz*uLcdVSz*uBytesPerPixel;
	uGlibStAddr1 = uGlibStAddr + uMidImgHSz*uMidImgVSz*uBytesPerPixel;
	
	//uSrcStAddr=0;

	// CSC & Scaling the DrawPattern 1

	//GLIB_Init(uGlibStAddr, uImgHSz, uImgVSz, eSrcDataFmt);
	//GLIB_DrawPattern(uImgHSz, uImgVSz);
	ScalerT_GetImageFromBMP(uGlibStAddr, uLcdHSz, uLcdVSz, 11, eSrcDataFmt);


#if 0
	switch(eSrcDataFmt)
	{

		case RGB16 : 
					pBufAddr16 = (u16 *)uGlibStAddr;
					
					for(i=0 ; i<uImgHSz*uImgVSz ; i++)
						*pBufAddr16++ = pBgcolors216[i];
						
					break;
							
		case RGB24 : 
					pBufAddr32 = (u32 *)uGlibStAddr;
					
					for(i=0 ; i<uImgHSz*uImgVSz ; i++)
						*pBufAddr32++ = pSmile_2[i];
						
					break;
	}
#endif

	uScaleOffsetX = uMidImgHSz/2;
	uScaleOffsetY = uMidImgVSz/2;
	// generation mid image
	POST_InitIp1(	uImgHSz, uImgVSz, uSrcStartX, uSrcStartY, uSrcCroppedHSz, uSrcCroppedVSz, uGlibStAddr, eSrcDataFmt,
					uMidImgHSz, uMidImgVSz, uMidStartX, uMidStartY, uMidScaledHSz, uMidScaledVSz, uGlibStAddr1, eSrcDataFmt,
					1, false, ePostRunMode, POST_DMA, POST_DMA, &oSc);
	POST_StartProcessing1(0, 0, &oSc);
	while (!POST_IsProcessingDone(&oSc));

	// default display image
	POST_InitIp1(	uMidImgHSz, uMidImgVSz, uMidStartX, uMidStartY, uMidScaledHSz-uScaleOffsetX, uMidScaledVSz-uScaleOffsetY, uGlibStAddr1, eSrcDataFmt,
					uLcdHSz, uLcdVSz, uDstStartX, uDstStartY, uDstScaledHSz, uDstScaledVSz, uLcdStAddr, eSrcDataFmt,
					1, false, ePostRunMode, POST_DMA, POST_DMA, &oSc);
	POST_StartProcessing1(0, 0, &oSc);
	while (!POST_IsProcessingDone(&oSc));

	UART_Printf("=============================================\n");
	UART_Printf("'w' : Vertical Scale-up,  'x' : Vertical Scale-down\n");
	UART_Printf("'d' : Horizontal Scale-up,  'a' : Horizontal Scale-down\n");
	UART_Printf("=============================================\n");
	UART_Printf("Press 'q' key to exit\n");	

	//RGB24 -> Width : multiple of 1,   RGB16 -> Width : multiple of 2
	while((uScale=UART_Getc()) != 'q')
	{
		switch (uScale)
		{
			//Vetical Scale Up
			case 'w' :
			case 'W' :	POST_CalcurateScaleOffset(	uMidScaledHSz-uScaleOffsetX-uPreHOffset, uMidScaledVSz-uScaleOffsetY-uPreVOffset, uDstScaledHSz, uDstScaledVSz, &oSc, 
													&uHOffset, &uVOffset, &uHRatio, &uVRatio, eSrcDataFmt);
						if(uPreVOffset == uVOffset)
						{
							if(uScaleOffsetY < (uMidImgVSz-uPreVOffset))
								 uScaleOffsetY += uPreVOffset;
						}
						else
						{	
							if(uScaleOffsetY < (uMidImgVSz-uVOffset))
								uScaleOffsetY += uVOffset - (uScaleOffsetY%(uVOffset));
							uPreVOffset = uVOffset;
						}
						break;
			//Vetical Scale Down
			case 'x' :
			case 'X' :	POST_CalcurateScaleOffset(	uMidScaledHSz-uScaleOffsetX+uPreHOffset, uMidScaledVSz-uScaleOffsetY+uPreVOffset, uDstScaledHSz, uDstScaledVSz, &oSc, 
													&uHOffset, &uVOffset, &uHRatio, &uVRatio, eSrcDataFmt);
						if(uPreVOffset == uVOffset)
						{
							if(uScaleOffsetY >= uPreVOffset)
								 uScaleOffsetY -= uPreVOffset;
						}
						else
						{	
							if(uScaleOffsetY >= uVOffset)
								uScaleOffsetY -= uVOffset - (uScaleOffsetY%(uVOffset));
							uPreVOffset = uVOffset;
						}
						break;	
			//Horizontal Scale Up	
			case 'd' :
			case 'D' :	POST_CalcurateScaleOffset(	uMidScaledHSz-uScaleOffsetX-uPreHOffset, uMidScaledVSz-uScaleOffsetY-uPreVOffset, uDstScaledHSz, uDstScaledVSz, &oSc, 
													&uHOffset, &uVOffset, &uHRatio, &uVRatio, eSrcDataFmt);
						if(uPreHOffset == uHOffset)
						{
							if(uScaleOffsetX < (uMidImgHSz-uPreHOffset))
								 uScaleOffsetX += uPreHOffset;
						}
						else
						{	
							if(uScaleOffsetX < (uMidImgHSz-uHOffset))
								uScaleOffsetX += uHOffset - (uScaleOffsetX%(uHOffset));
							uPreHOffset = uHOffset;
						}
						break;	
			//Horizontal Scale Down						
			case 'a' :
			case 'A' :	POST_CalcurateScaleOffset(	uMidScaledHSz-uScaleOffsetX+uPreHOffset, uMidScaledVSz-uScaleOffsetY+uPreVOffset, uDstScaledHSz, uDstScaledVSz, &oSc, 
													&uHOffset, &uVOffset, &uHRatio, &uVRatio, eSrcDataFmt);
						if(uPreHOffset == uHOffset)
						{
							if(uScaleOffsetX >= uPreHOffset)
								 uScaleOffsetX -= uPreHOffset;
						}
						else
						{	
							if(uScaleOffsetX >= uHOffset)
								uScaleOffsetX -= uHOffset - (uScaleOffsetX%(uHOffset));
							uPreHOffset = uHOffset;
						}
			default : 	break;
		}

		UART_Printf("(%d*%d) -> (%d*%d)\n",uMidScaledHSz-uScaleOffsetX, uMidScaledVSz-uScaleOffsetY, uDstScaledHSz, uDstScaledVSz);
		
		POST_InitIp1(	uMidImgHSz, uMidImgVSz, uMidStartX, uMidStartY, uMidScaledHSz-uScaleOffsetX, uMidScaledVSz-uScaleOffsetY, uGlibStAddr1, eSrcDataFmt,
						uLcdHSz, uLcdVSz, uDstStartX, uDstStartY, uDstScaledHSz, uDstScaledVSz, uLcdStAddr, eSrcDataFmt,
						1, false, ePostRunMode, POST_DMA, POST_DMA, &oSc);
		POST_StartProcessing1(0, 0, &oSc);
		while (!POST_IsProcessingDone(&oSc));		
	}
	
}





const testFuncMenu scaler_r2r_menu[] =
{
	ScalerT_SimpleDmaToDma_R2R,                        			"Simple DMA CSC Test From RGB To RGB",
	ScalerT_ComplexIDmaToDma_R2R,                       		"Complex DMA CSC Test From RGB To RGB",
	ScalerT_SimpleFreeRunDmaToDma_R2R_Polling,          	"Simple FreeRun And CSC Test From RGB To RGB By Using Polling",
	ScalerT_SimpleFreeRunDmaToDma_R2R_Int,              	"Simple FreeRun And CSC Test From RGB To RGB By Using Interrupt", 		
	ScalerT_ComplexFreeRunDmaToDma_R2R_Polling, 		"Complex FreeRun And CSC Test From RGB To RGB By Using Polling",
	ScalerT_ComplexFreeRunDmaToDma_R2R_Int,             	"Complex FreeRun And CSC Test From RGB To RGB By Using Interrupt",		
	ScalerT_ScaleUpDown_RGB,							"Scale Up/Down RGB",
	0,                                          0
};


static void ScalerT_ConvertRGBToRGB(void)
{
	u32 i; 
	s32 uSel;
	
	UART_Printf("[ScalerT_ConvertRGBToRGB]\n");
	
	while(1)
	{
		UART_Printf("\n");
		for (i=0; (int)(scaler_r2r_menu[i].desc)!=0; i++)
		{
			UART_Printf("%2d: %s\n", i, scaler_r2r_menu[i].desc);
		}

		UART_Printf("\n");
		UART_Printf("Select The Function To Test: ");

		uSel = UART_GetIntNum();
		UART_Printf("\n");

		if (uSel == -1)
			break;
		if (uSel>=0 && uSel<(sizeof(scaler_r2r_menu)/8-1))
			(scaler_r2r_menu[uSel].func)();
	}
}


////////////////////////////////////////////////////////////////////////////
//////////////////                    RGB to YUV to RGB                    ///////////////////
////////////////////////////////////////////////////////////////////////////


static void ScalerT_SetCscTypeDmaToDma_R2Y2R(void)
{
	int nSelDmaCscType_R2Y2R;

	while(1)
	{
		UART_Printf("\n");		
		UART_Printf(" [1] RGB16 -> YCbYCr422_Intlv And YCbYCr422_Intlv -> RGB16\n");
		UART_Printf(" [2] RGB16 -> YCrYCb422_Intlv And YCrYCb422_Intlv -> RGB16\n");		
		UART_Printf(" [3] RGB16 -> CbYCrY422_Intlv And CbYCrY422_Intlv -> RGB16\n");	
		UART_Printf(" [4] RGB16 -> CrYCbY422_Intlv And CrYCbY422_Intlv -> RGB16\n");
		UART_Printf(" [5] RGB16 -> YC420 And YC420 -> RGB16\n");
		UART_Printf("\n");
		UART_Printf(" [6] RGB24 -> YCbYCr422_Intlv And YCbYCr422_Intlv -> RGB24\n");
		UART_Printf(" [7] RGB24 -> YCrYCb422_Intlv And YCrYCb422_Intlv -> RGB24\n");		
		UART_Printf(" [8] RGB24 -> CbYCrY422_Intlv And CbYCrY422_Intlv -> RGB24\n");	
		UART_Printf(" [9] RGB24 -> CrYCbY422_Intlv And CrYCbY422_Intlv -> RGB24\n");
		UART_Printf("[10] RGB24 -> YC420 And YC420 -> RGB24\n");
		UART_Printf("\n");
		UART_Printf("[11] RGB16 -> YCbYCr422_Intlv And YCbYCr422_Intlv -> RGB24\n");
		UART_Printf("[12] RGB16 -> YCrYCb422_Intlv And YCrYCb422_Intlv -> RGB24\n");		
		UART_Printf("[13] RGB16 -> CbYCrY422_Intlv And CbYCrY422_Intlv -> RGB24\n");	
		UART_Printf("[14] RGB16 -> CrYCbY422_Intlv And CrYCbY422_Intlv -> RGB24\n");
		UART_Printf("[15] RGB16 -> YC420 And YC420 -> RGB24\n");
		UART_Printf("\n");
		UART_Printf("[16] RGB24 -> YCbYCr422_Intlv And YCbYCr422_Intlv -> RGB16\n");
		UART_Printf("[17] RGB24 -> YCrYCb422_Intlv And YCrYCb422_Intlv -> RGB16\n");		
		UART_Printf("[18] RGB24 -> CbYCrY422_Intlv And CbYCrY422_Intlv -> RGB16\n");	
		UART_Printf("[19] RGB24 -> CrYCbY422_Intlv And CrYCbY422_Intlv -> RGB16\n");
		UART_Printf("[20] RGB24 -> YC420 And YC420 -> RGB16\n");
		UART_Printf("\n");

		UART_Printf(">> Select The Color Space Conversion Type: ");
		nSelDmaCscType_R2Y2R = UART_GetIntNum();

//		if(	nSelDmaCscType_R2Y2R == 1 || nSelDmaCscType_R2Y2R == 2 || nSelDmaCscType_R2Y2R == 3 || nSelDmaCscType_R2Y2R == 4 ||
//			nSelDmaCscType_R2Y2R == 5 || nSelDmaCscType_R2Y2R == 6 || nSelDmaCscType_R2Y2R == 7 || nSelDmaCscType_R2Y2R == 8 ||
//			nSelDmaCscType_R2Y2R == 9 || nSelDmaCscType_R2Y2R == 10 || nSelDmaCscType_R2Y2R == 11 || nSelDmaCscType_R2Y2R == 12 ||
//			nSelDmaCscType_R2Y2R == 13 || nSelDmaCscType_R2Y2R == 14 || nSelDmaCscType_R2Y2R == 15 || nSelDmaCscType_R2Y2R == 16 ||
//			nSelDmaCscType_R2Y2R == 17 || nSelDmaCscType_R2Y2R == 18 || nSelDmaCscType_R2Y2R == 19 || nSelDmaCscType_R2Y2R == 20)
		if( (nSelDmaCscType_R2Y2R >= 1) && (nSelDmaCscType_R2Y2R <=20) )
		{
			eSrcDataFmt =
				(nSelDmaCscType_R2Y2R == 1 || nSelDmaCscType_R2Y2R == 2 || nSelDmaCscType_R2Y2R == 3 ||
				nSelDmaCscType_R2Y2R == 4 || nSelDmaCscType_R2Y2R == 5 || nSelDmaCscType_R2Y2R == 11 || 
				nSelDmaCscType_R2Y2R == 12 || nSelDmaCscType_R2Y2R == 13 || nSelDmaCscType_R2Y2R == 14 ||
				nSelDmaCscType_R2Y2R == 15) ? RGB16 : RGB24;

			eMidDataFmt =
				(nSelDmaCscType_R2Y2R == 1 || nSelDmaCscType_R2Y2R == 6 ||
				nSelDmaCscType_R2Y2R == 11 || nSelDmaCscType_R2Y2R == 16 ) ? YCBYCR :
				(nSelDmaCscType_R2Y2R == 2 || nSelDmaCscType_R2Y2R == 7 ||
				nSelDmaCscType_R2Y2R == 12 || nSelDmaCscType_R2Y2R == 17 ) ? YCRYCB :
				(nSelDmaCscType_R2Y2R == 3 || nSelDmaCscType_R2Y2R == 8 ||
				nSelDmaCscType_R2Y2R == 13 || nSelDmaCscType_R2Y2R == 18 ) ? CBYCRY :
				(nSelDmaCscType_R2Y2R == 4 || nSelDmaCscType_R2Y2R == 9 ||
				nSelDmaCscType_R2Y2R == 14 || nSelDmaCscType_R2Y2R == 19 ) ? CRYCBY : YC420;

			eDstDataFmt =
				(nSelDmaCscType_R2Y2R == 1 || nSelDmaCscType_R2Y2R == 2 || nSelDmaCscType_R2Y2R == 3 ||
				nSelDmaCscType_R2Y2R == 4 || nSelDmaCscType_R2Y2R == 5 || nSelDmaCscType_R2Y2R == 16 || 
				nSelDmaCscType_R2Y2R == 17 || nSelDmaCscType_R2Y2R == 18 || nSelDmaCscType_R2Y2R == 19 ||
				nSelDmaCscType_R2Y2R == 20) ? RGB16 : RGB24;

			break;
		}
		else
			UART_Printf("Invalid Input! Retry It!!\n");
		
	}
	
	if (eSrcDataFmt == RGB16 && eMidDataFmt == YCBYCR && eDstDataFmt == RGB16)
	{
		uImgHSz = 240, uImgVSz = 320, uSrcStartX = 20, uSrcStartY = 20;
		uSrcCroppedHSz = 160, uSrcCroppedVSz = 240;

		uMidImgHSz = 240, uMidImgVSz = 320, uMidStartX = 60, uMidStartY = 40;
		uMidScaledHSz = 120, uMidScaledVSz = 120;

		uDstStartX = 120, 	uDstStartY = 100, uDstScaledHSz = 104, uDstScaledVSz = 140;		
	}
	else if (eSrcDataFmt == RGB16 && eMidDataFmt == YCRYCB && eDstDataFmt == RGB16)
	{

		uImgHSz = 120, 	uImgVSz = 160, uSrcStartX = 0, uSrcStartY = 40;
		uSrcCroppedHSz = 120, 	uSrcCroppedVSz = 100;

		uMidImgHSz = 240, uMidImgVSz = 320, uMidStartX = 0, uMidStartY = 40;
		uMidScaledHSz = 120, uMidScaledVSz = 120;

		uDstStartX = 0, uDstStartY = 50, uDstScaledHSz = 120, uDstScaledVSz = 110;//160;		
	}
	else if (eSrcDataFmt == RGB16 && eMidDataFmt == CBYCRY && eDstDataFmt == RGB16)	
	{
		uImgHSz = 360, 	uImgVSz = 480,	uSrcStartX = 80, uSrcStartY = 120;
		uSrcCroppedHSz = 104, 	uSrcCroppedVSz = 100;

		uMidImgHSz = 240, uMidImgVSz = 320, uMidStartX = 20, uMidStartY = 40;
		uMidScaledHSz = 160, uMidScaledVSz = 120;

		uDstStartX = 120, uDstStartY = 80, uDstScaledHSz = 80, uDstScaledVSz = 160;		
	}
	else if (eSrcDataFmt == RGB16 && eMidDataFmt == CRYCBY && eDstDataFmt == RGB16)
	{
		uImgHSz = 240, 	uImgVSz = 320, uSrcStartX = 0, uSrcStartY = 20;
		uSrcCroppedHSz = 160, 	uSrcCroppedVSz = 200;

		uMidImgHSz = 120, uMidImgVSz = 160 ,uMidStartX = 10, uMidStartY = 30;
		uMidScaledHSz = 80, uMidScaledVSz = 100;

		uDstStartX = 50, uDstStartY = 60, uDstScaledHSz = 176 ,uDstScaledVSz = 180;		
	}
	else if (eSrcDataFmt == RGB16 && eMidDataFmt == YC420 && eDstDataFmt == RGB16)
	{
		uImgHSz = 240,	uImgVSz = 320, uSrcStartX = 20, uSrcStartY = 20;
		uSrcCroppedHSz = 160, 	uSrcCroppedVSz = 200;

		uMidImgHSz = 120, uMidImgVSz = 160, uMidStartX = 0, uMidStartY = 30;
		uMidScaledHSz = 48, uMidScaledVSz = 100;

		uDstStartX = 50, uDstStartY = 40, uDstScaledHSz = 160, uDstScaledVSz = 200;		
	}
	else if (eSrcDataFmt == RGB24 && eMidDataFmt == YCBYCR && eDstDataFmt == RGB24)
	{
		uImgHSz = 240, 	uImgVSz = 320, uSrcStartX = 20, uSrcStartY = 10;
		uSrcCroppedHSz = 128, 	uSrcCroppedVSz = 128;

		uMidImgHSz = 360, uMidImgVSz = 480, uMidStartX = 60, uMidStartY = 40;
		uMidScaledHSz = 240, uMidScaledVSz = 240;

		uDstStartX = 20, uDstStartY = 80, uDstScaledHSz = 160, uDstScaledVSz = 160;		
	}
	else if (eSrcDataFmt == RGB24 && eMidDataFmt == YCRYCB&& eDstDataFmt == RGB24)
	{
		uImgHSz = 120, 	uImgVSz = 160, uSrcStartX = 20, uSrcStartY = 10;
		uSrcCroppedHSz = 80, uSrcCroppedVSz = 120;

		uMidImgHSz = 240, uMidImgVSz = 320, uMidStartX = 120, uMidStartY = 120;
		uMidScaledHSz = 80, uMidScaledVSz = 160;

		uDstStartX = 20, uDstStartY = 40, uDstScaledHSz = 160, uDstScaledVSz = 200;		
	}
	else if (eSrcDataFmt == RGB24 && eMidDataFmt == CBYCRY&& eDstDataFmt == RGB24)
	{

		uImgHSz = 360, uImgVSz = 480, uSrcStartX = 0, uSrcStartY = 0;
		uSrcCroppedHSz = 360, 	uSrcCroppedVSz = 480;

		uMidImgHSz = 360, uMidImgVSz = 480, uMidStartX = 0, uMidStartY = 0;
		uMidScaledHSz = 360, uMidScaledVSz = 480;

		uDstStartX = 0, uDstStartY = 0, uDstScaledHSz = 320, uDstScaledVSz = 240;		
	}
	else if (eSrcDataFmt == RGB24 && eMidDataFmt == CRYCBY&& eDstDataFmt == RGB24)
	{
		uImgHSz = 240, uImgVSz = 320, uSrcStartX = 80, uSrcStartY = 80;
		uSrcCroppedHSz = 104, uSrcCroppedVSz = 120;

		uMidImgHSz = 360, uMidImgVSz = 480, uMidStartX = 180, uMidStartY = 180;
		uMidScaledHSz = 120, uMidScaledVSz = 240;

		uDstStartX = 20, uDstStartY = 80, uDstScaledHSz = 80, uDstScaledVSz = 160;		
	}
	else if (eSrcDataFmt == RGB24 && eMidDataFmt == YC420 && eDstDataFmt == RGB24)
	{
		uImgHSz = 120, 	uImgVSz = 160, uSrcStartX = 20, uSrcStartY = 80;
		uSrcCroppedHSz = 72, uSrcCroppedVSz = 72;

		uMidImgHSz = 360, uMidImgVSz = 480, uMidStartX = 24, uMidStartY = 20;
		uMidScaledHSz = 320, uMidScaledVSz = 400;

		uDstStartX = 80, uDstStartY = 100, uDstScaledHSz = 120, uDstScaledVSz = 120;		
	}
	else if (eSrcDataFmt == RGB16 && eMidDataFmt == YCBYCR && eDstDataFmt == RGB24)
	{
		uImgHSz = 240, 	uImgVSz = 320, uSrcStartX = 160, 	uSrcStartY = 200;
		uSrcCroppedHSz = 56, uSrcCroppedVSz = 120;

		uMidImgHSz = 120, uMidImgVSz = 160, uMidStartX = 20, uMidStartY = 40;
		uMidScaledHSz = 80, uMidScaledVSz = 80;

		uDstStartX = 20, uDstStartY = 20, uDstScaledHSz = 200, uDstScaledVSz = 220;		
	}
	else if (eSrcDataFmt == RGB16 && eMidDataFmt == YCRYCB && eDstDataFmt == RGB24)
	{
		uImgHSz = 120, 	uImgVSz = 160, uSrcStartX = 20, uSrcStartY = 20;
		uSrcCroppedHSz = 88, uSrcCroppedVSz = 120;

		uMidImgHSz = 120, uMidImgVSz = 480, uMidStartX = 0, uMidStartY = 40;
		uMidScaledHSz = 88, uMidScaledVSz = 280;

		uDstStartX = 10, uDstStartY = 10, uDstScaledHSz = 200,	uDstScaledVSz = 230;		
	}
	else if (eSrcDataFmt == RGB16 && eMidDataFmt == CBYCRY && eDstDataFmt == RGB24)
	{
		uImgHSz = 360, 	uImgVSz = 480, uSrcStartX = 180, 	uSrcStartY = 240;
		uSrcCroppedHSz = 120, 	uSrcCroppedVSz = 240;

		uMidImgHSz = 240, uMidImgVSz = 160, uMidStartX = 20, uMidStartY = 40;
		uMidScaledHSz = 144, uMidScaledVSz = 120;

		uDstStartX = 10, uDstStartY = 10, uDstScaledHSz = 104, uDstScaledVSz = 104;		
	}
	else if (eSrcDataFmt == RGB16 && eMidDataFmt == CRYCBY && eDstDataFmt == RGB24)
	{
		uImgHSz = 240, 	uImgVSz = 320, uSrcStartX = 40, uSrcStartY = 50;
		uSrcCroppedHSz = 184, 	uSrcCroppedVSz = 120;

		uMidImgHSz = 120, uMidImgVSz = 480, uMidStartX = 20, uMidStartY = 40;
		uMidScaledHSz = 88, uMidScaledVSz = 400;

		uDstStartX = 10, uDstStartY = 10, uDstScaledHSz = 160, uDstScaledVSz = 230;		
	}
	else if (eSrcDataFmt == RGB16 && eMidDataFmt == YC420 && eDstDataFmt == RGB24)
	{
		uImgHSz = 120, 	uImgVSz = 160, uSrcStartX = 40, uSrcStartY = 50;
		uSrcCroppedHSz = 40, uSrcCroppedVSz = 88;

		uMidImgHSz = 360, uMidImgVSz = 480, uMidStartX = 56, uMidStartY = 10;
		uMidScaledHSz = 184, uMidScaledVSz = 240;

		uDstStartX = 10, uDstStartY = 10, uDstScaledHSz = 160, uDstScaledVSz = 160;		
	}
	else if (eSrcDataFmt == RGB24 && eMidDataFmt == YCBYCR && eDstDataFmt == RGB16)
	{
		uImgHSz = 240, uImgVSz = 320, uSrcStartX = 60, uSrcStartY = 200;
		uSrcCroppedHSz = 112, 	uSrcCroppedVSz = 96;

		uMidImgHSz = 120, uMidImgVSz = 160, uMidStartX = 20, uMidStartY = 40;
		uMidScaledHSz = 88, uMidScaledVSz = 96;

		uDstStartX = 60, uDstStartY = 80, uDstScaledHSz = 136, uDstScaledVSz = 160;		
	}
	else if (eSrcDataFmt == RGB24 && eMidDataFmt == YCRYCB&& eDstDataFmt == RGB16)
	{
		uImgHSz = 240, uImgVSz = 320, uSrcStartX = 180, uSrcStartY = 60;
		uSrcCroppedHSz = 48, uSrcCroppedVSz = 240;

		uMidImgHSz = 240, uMidImgVSz = 480, uMidStartX = 20, uMidStartY = 40;
		uMidScaledHSz = 184, uMidScaledVSz = 240;

		uDstStartX = 40, uDstStartY = 40, uDstScaledHSz = 184, uDstScaledVSz = 80;		
	}
	else if (eSrcDataFmt == RGB24 && eMidDataFmt == CBYCRY && eDstDataFmt == RGB16)
	{
		uImgHSz = 240, uImgVSz = 320, uSrcStartX = 180, uSrcStartY = 60;
		uSrcCroppedHSz = 48, uSrcCroppedVSz = 240;

		uMidImgHSz = 240, uMidImgVSz = 480, uMidStartX = 20, uMidStartY = 40;
		uMidScaledHSz = 184, uMidScaledVSz = 240;

		uDstStartX = 40, uDstStartY = 40, uDstScaledHSz = 184, uDstScaledVSz = 80;		
	}
	else if (eSrcDataFmt == RGB24 && eMidDataFmt == CRYCBY && eDstDataFmt == RGB16)
	{
		uImgHSz = 360, uImgVSz = 480, uSrcStartX = 40, uSrcStartY = 50;
		uSrcCroppedHSz = 184, 	uSrcCroppedVSz = 120;

		uMidImgHSz = 120, uMidImgVSz = 480, uMidStartX = 20, uMidStartY = 40;
		uMidScaledHSz = 80, uMidScaledVSz = 240;

		uDstStartX = 10, uDstStartY = 10, uDstScaledHSz = 160, uDstScaledVSz = 230;		
		
	}
	else if (eSrcDataFmt == RGB24 && eMidDataFmt == YC420 && eDstDataFmt == RGB16)
	{
		uImgHSz = 240, uImgVSz = 320, uSrcStartX = 120, uSrcStartY = 120;
		uSrcCroppedHSz = 120, 	uSrcCroppedVSz = 120;

		uMidImgHSz = 120, uMidImgVSz = 160, uMidStartX = 24, uMidStartY = 20;
		uMidScaledHSz = 96, uMidScaledVSz = 96;

		uDstStartX = 10, uDstStartY = 10, uDstScaledHSz = 160, uDstScaledVSz = 160;		
	}
	else 
	{
		UART_Printf("Error! Unsupported Data Format!\n");
		return;
	}

	UART_Printf("\n");
	if(uSimpleTest == 0)
	{
		UART_Printf("=========    Complex_Test Image Size    =========\n");
		UART_Printf("SrcImgHSz     = %d,       SrcImgVSz     = %d\n",uImgHSz,uImgVSz);
		UART_Printf("SrcStartX     = %d,       SrcStartY     = %d\n",uSrcStartX,uSrcStartY);
		UART_Printf("SrcCroppedHSz = %d,       SrcCroppedVSz = %d\n",uSrcCroppedHSz,uSrcCroppedVSz);
		UART_Printf("\n");
		UART_Printf("MidImgHSz     = %d,       MidImgVSz     = %d\n",uMidImgHSz,uMidImgVSz);
		UART_Printf("MidStartX     = %d,       MidStartY     = %d\n",uMidStartX,uMidStartY);
		UART_Printf("MidScaledHSz  = %d,       MidScaledVSz  = %d\n",uMidScaledHSz,uMidScaledVSz);
		UART_Printf("\n");
		UART_Printf("DstStartX     = %d,       DstStartY     = %d\n",uDstStartX,uDstStartY);
		UART_Printf("DstScaledHSz  = %d,       DstScaledVSz  = %d\n",uDstScaledHSz,uDstScaledVSz);	
		UART_Printf("=================================================\n");
		UART_Printf("\n");
	}
	
}


// TestXXX_R2Y2R does two steps
// Step 1) Glib Addr(RGB Data Format) -> Post(CSC) -> Post Addr (YUV Data Format) // To verify CSC from RGB to YUV
// Step 2) Post Addr(YUV Data Format) -> Post(CSC) -> LCD Addr (RGB Data Format) // To verify CSC from YUV to RGB
static void ScalerT_SimpleDmaToDma_R2Y2R(void)
{
	u32 uGlibStAddr, uPostStAddr;
	u32 uSrcBytesPerPixel, uDstBytesPerPixel;

	UART_Printf("[ScalerT_SimpleDmaToDma_R2Y2R]\n");

	uSimpleTest = 1;
	ScalerT_SetCscTypeDmaToDma_R2Y2R();
		
	//LCD_InitLDI(MAIN);
#ifdef LCD_INIT_TEST	
	LCD_InitBase();
	LCD_Start();
	LCD_InitWin(eDstDataFmt, uImgHSz, uImgVSz, 0, 0, uImgHSz, uImgVSz, 0, 0, uLcdStAddr, WIN0, false);
#else
	LCD_InitBase();
	LCD_InitDISPC(eDstDataFmt, uLcdStAddr, WIN0, false);
	LCD_GetFrmSz(&uLcdHSz, &uLcdVSz, WIN0);
	//LCD_InitDISPC1(uLcdHSz, uLcdVSz, eDstDataFmt, uLcdStAddr, WIN0, false);
	LCD_Start();
#endif
	LCD_SetWinOnOff(1, WIN0);
	//LCD_GetFrmSz(&uLcdHSz, &uLcdVSz, WIN0);

	uSrcBytesPerPixel = (eSrcDataFmt == RGB16) ? 2 : 4;
	uDstBytesPerPixel = (eDstDataFmt == RGB16) ? 2 : 4;
	uGlibStAddr = uLcdStAddr + uLcdHSz*uLcdVSz*uDstBytesPerPixel;
	uPostStAddr = uGlibStAddr + uLcdHSz*uLcdVSz*uSrcBytesPerPixel;

#if (POST_IMAGE_SOURCE == POST_IMAGE_PATTERN)
	GLIB_Init(uGlibStAddr, uLcdHSz, uLcdVSz, eSrcDataFmt);
	GLIB_DrawPattern(uLcdHSz, uLcdVSz);
#elif (POST_IMAGE_SOURCE == POST_IMAGE_HEADER)
	ScalerT_GetImageFromHeader(uGlibStAddr, uLcdHSz, uLcdVSz, eSrcDataFmt);
#endif

	// From RGB To YUV
	UART_Printf("========   RGB -> YUV   =========\n");
	UART_Printf("Source Address : 0x%08x\n", uGlibStAddr);
	UART_Printf("Destination Address : 0x%08x\n", uPostStAddr);
	POST_InitIpForDmaInDmaOut(	uLcdHSz, uLcdVSz, uGlibStAddr, eSrcDataFmt, 
									uLcdHSz, uLcdVSz, uPostStAddr, eMidDataFmt,
									1, FALSE, ONE_SHOT, &oSc);
	//POST_InitIpForDmaInDmaOut(	uImgHSz, uImgVSz, uGlibStAddr, eSrcDataFmt, 
		//							uLcdHSz, uLcdVSz, uPostStAddr, eMidDataFmt,
			//						1, FALSE, ONE_SHOT, &oSc);	
	POST_StartProcessing(&oSc);
	while (!POST_IsProcessingDone(&oSc));	

	//SaveToFile(POST_YUV_FILE_NAME1, uLcdHSz*uLcdVSz*2, uPostStAddr);
	
#if 0	// R->Y Wide & Narrow check
	{
		int R, G, B, YCbCr, i, j;
		for(i=0 ; i<uLcdVSz ; i++)
			for(j=0 ; j<uLcdHSz ; j++)
			{
				R = (*((s32 *)uGlibStAddr + i*uLcdVSz + j) & 0xFF0000)>>16;
				G = (*((s32 *)uGlibStAddr + i*uLcdVSz + j) & 0x00FF00)>>8;
				B = (*((s32 *)uGlibStAddr + i*uLcdVSz + j) & 0x0000FF)>>0;
				YCbCr = CSCR2Y_C(R, G, B, 0);
				UART_Printf("YCbCr : 0x%08x\n", YCbCr);
			}
	}
#endif

	// From YUV To RGB
	UART_Printf("========   YUV -> RGB   =========\n");
	UART_Printf("Source Address : 0x%08x\n", uPostStAddr);
	UART_Printf("Destination Address : 0x%08x\n", uLcdStAddr);	
	POST_InitIpForDmaInDmaOut(	uLcdHSz, uLcdVSz, uPostStAddr, eMidDataFmt, 
									uLcdHSz, uLcdVSz, uLcdStAddr, eDstDataFmt,
									1, FALSE, ONE_SHOT, &oSc);
	POST_StartProcessing(&oSc);
	while (!POST_IsProcessingDone(&oSc));	

#if (POST_IMAGE_BMP_SLIDE)	
	{
		u32 uImageNum;
		UART_Printf("Input the any key to display the other image ('q' : quit)\n");
		uImageNum = 0;
		
		while(UART_Getc() != 'q')
		{
			ScalerT_GetImageFromBMP(uGlibStAddr, uLcdHSz, uLcdVSz, uImageNum, eSrcDataFmt);

			// RGB -> YUV
			POST_InitIpForDmaInDmaOut(	uLcdHSz, uLcdVSz, uGlibStAddr, eSrcDataFmt, 
											uLcdHSz, uLcdVSz, uPostStAddr, eMidDataFmt,
											1, FALSE, ONE_SHOT, &oSc);
			POST_StartProcessing(&oSc);
			while (!POST_IsProcessingDone(&oSc));	

			//SaveToFile(POST_YUV_FILE_NAME1, uLcdHSz*uLcdVSz*2, uPostStAddr);
			
			// YUV -> RGB
			POST_InitIpForDmaInDmaOut(	uLcdHSz, uLcdVSz, uPostStAddr, eMidDataFmt, 
											uLcdHSz, uLcdVSz, uLcdStAddr, eDstDataFmt,
											1, FALSE, ONE_SHOT, &oSc);
			POST_StartProcessing(&oSc);
			while (!POST_IsProcessingDone(&oSc));	
			uImageNum++;
		}
	}
#endif	

	UART_Printf("\n");
}


static void ScalerT_ComplexDmaToDma_R2Y2R(void)
{
	u32 uGlibStAddr, uPostStAddr;
	u32 uSrcBytesPerPixel, uDstBytesPerPixel;

	UART_Printf("[ScalerT_ComplexDmaToDma_R2Y2R]\n");

	uSimpleTest = 0;
	ScalerT_SetCscTypeDmaToDma_R2Y2R();
	
	ePostRunMode = ONE_SHOT;

	//LCD_InitLDI(MAIN);

#ifdef LCD_INIT_TEST	
	LCD_InitBase();
	LCD_Start();
	LCD_InitWin(eDstDataFmt, uImgHSz, uImgVSz, 0, 0, uImgHSz, uImgVSz, 0, 0, uLcdStAddr, WIN0, false);
#else
	LCD_InitDISPC(eDstDataFmt, uLcdStAddr, WIN0, false);
	LCD_Start();
#endif

	LCD_SetWinOnOff(1, WIN0);

	LCD_GetFrmSz(&uLcdHSz, &uLcdVSz, WIN0);

	uSrcBytesPerPixel = (eSrcDataFmt == RGB24) ? 4 : 2;
	uDstBytesPerPixel = (eDstDataFmt == RGB24) ? 4 : 2;

	uGlibStAddr = uLcdStAddr + uLcdHSz*uLcdVSz*uDstBytesPerPixel;
	uPostStAddr = uGlibStAddr + uImgHSz*uImgVSz*uSrcBytesPerPixel;

	GLIB_Init(uLcdStAddr, uLcdHSz, uLcdVSz, eDstDataFmt);
	GLIB_ClearFrame(C_BLACK);

#if (POST_IMAGE_SOURCE == POST_IMAGE_PATTERN)
	GLIB_Init(uGlibStAddr, uImgHSz, uImgVSz, eSrcDataFmt);
	GLIB_DrawPattern(uImgHSz, uImgVSz);
#elif (POST_IMAGE_SOURCE == POST_IMAGE_HEADER)
	ScalerT_GetImageFromHeader(uGlibStAddr, uImgHSz, uImgVSz, eSrcDataFmt);
#elif (POST_IMAGE_SOURCE == POST_IMAGE_BMP)
	ScalerT_GetImageFromBMP(uGlibStAddr, uLcdHSz, uLcdVSz, 3, eSrcDataFmt);
#endif

	// From RGB To YUV
	UART_Printf("========   RGB -> YUV   =========\n");
	UART_Printf("Source Address : 0x%08x\n", uGlibStAddr);
	UART_Printf("Destination Address : 0x%08x\n", uPostStAddr);	
	POST_InitIp1(	uImgHSz, uImgVSz, uSrcStartX, uSrcStartY, uSrcCroppedHSz, uSrcCroppedVSz, uGlibStAddr, eSrcDataFmt,
					uMidImgHSz, uMidImgVSz, uMidStartX, uMidStartY, uMidScaledHSz, uMidScaledVSz, uPostStAddr, eMidDataFmt,
					1, false, ePostRunMode, POST_DMA, POST_DMA, &oSc);
	POST_StartProcessing(&oSc);
	while (!POST_IsProcessingDone(&oSc));
	
	// From YUV To RGB
	UART_Printf("========   YUV -> RGB   =========\n");
	UART_Printf("Source Address : 0x%08x\n", uPostStAddr);
	UART_Printf("Destination Address : 0x%08x\n", uLcdStAddr);		
	POST_InitIp1(	uMidImgHSz, uMidImgVSz, uMidStartX, uMidStartY, uMidScaledHSz, uMidScaledVSz, uPostStAddr, eMidDataFmt,
					uLcdHSz, uLcdVSz, uDstStartX, uDstStartY, uDstScaledHSz, uDstScaledVSz, uLcdStAddr, eDstDataFmt,
					1, false, ONE_SHOT, POST_DMA, POST_DMA, &oSc);
	POST_StartProcessing(&oSc);
	while (!POST_IsProcessingDone(&oSc));	
}


static void ScalerT_SimpleFreeRunDmaToDma_R2Y2R_Polling(void)
{
	u32 uGlibStAddr, uGlibStAddr2, uPostStAddr, uPostStAddr2;
	u32 uSrcBytesPerPixel, uDstBytesPerPixel;
	u32 uFrmCnt;

	UART_Printf("[ScalerT_SimpleFreeRunDmaToDma_R2Y2R_Polling]\n");

	uSimpleTest = 1;
	ScalerT_SetCscTypeDmaToDma_R2Y2R();
	
	bScalerIsPollingOrInt = TRUE;

	//LCD_InitLDI(MAIN);
#ifdef LCD_INIT_TEST	
	LCD_InitBase();
	LCD_Start();
	LCD_InitWin(eDstDataFmt, uImgHSz, uImgVSz, 0, 0, uImgHSz, uImgVSz, 0, 0, uLcdStAddr, WIN0, false);
#else
	LCD_InitDISPC(eDstDataFmt, uLcdStAddr, WIN0, false);
	LCD_Start();
#endif
	LCD_SetWinOnOff(1, WIN0);
	LCD_GetFrmSz(&uLcdHSz, &uLcdVSz, WIN0);

	uSrcBytesPerPixel = (eSrcDataFmt == RGB16) ? 2 : 4;
	uDstBytesPerPixel = (eDstDataFmt == RGB16) ? 2 : 4;

	uGlibStAddr = uLcdStAddr + uLcdHSz*uLcdVSz*uDstBytesPerPixel;
	uPostStAddr = uGlibStAddr + uLcdHSz*uLcdVSz*uSrcBytesPerPixel;

	uGlibStAddr2 = uPostStAddr + uLcdHSz*uLcdVSz*uDstBytesPerPixel;
	uPostStAddr2 = uGlibStAddr2 + uLcdHSz*uLcdVSz*uSrcBytesPerPixel;

	GLIB_Init(uGlibStAddr, uLcdHSz, uLcdVSz, eSrcDataFmt);
	GLIB_DrawPattern(uLcdHSz, uLcdVSz);

	GLIB_Init(uGlibStAddr2, uLcdHSz, uLcdVSz, eSrcDataFmt);
	GLIB_DrawPattern2(uLcdHSz, uLcdVSz);

	// From RGB to YUV for pattern 1
	POST_InitIpForDmaInDmaOut(uLcdHSz, uLcdVSz, uGlibStAddr, eSrcDataFmt, 
								uLcdHSz, uLcdVSz, uPostStAddr, eMidDataFmt,
								1, FALSE, ONE_SHOT, &oSc);
	POST_StartProcessing(&oSc);
	while (!POST_IsProcessingDone(&oSc));
	UART_Printf("CSC(RGB -> YUV) is finished for pattern 1\n");
	
	// From RGB to YUV for pattern 2
	POST_InitIpForDmaInDmaOut(uLcdHSz, uLcdVSz, uGlibStAddr2, eSrcDataFmt,
								uLcdHSz, uLcdVSz, uPostStAddr2, eMidDataFmt,
								1, FALSE, ONE_SHOT, &oSc);
	POST_StartProcessing(&oSc);
	while (!POST_IsProcessingDone(&oSc));
	UART_Printf("CSC(RGB -> YUV) is fiished for pattern 2\n");
	
	// From YUV To RGB
	POST_InitIpForDmaInDmaOut(uLcdHSz, uLcdVSz, uPostStAddr2, eMidDataFmt,
								uLcdHSz, uLcdVSz, uLcdStAddr, eDstDataFmt,
								1, FALSE, FREE_RUN, &oSc);
	POST_SetNextFrameStAddr(uPostStAddr, uLcdStAddr, &oSc);
	POST_StartProcessing(&oSc);

	uFrmCnt = 0;
	while(uFrmCnt<uMaxFrameCnt)
	{
		while (!POST_IsProcessingDone(&oSc));			
		
		if (uFrmCnt%2)
			POST_SetNextFrameStAddr(uPostStAddr2, uLcdStAddr, &oSc);
		else
			POST_SetNextFrameStAddr(uPostStAddr, uLcdStAddr, &oSc);
		POST_ClearPending(&oSc);

		DelayfrTimer(milli, 1000);
		uFrmCnt++;
	}

	POST_StopProcessingOfFreeRun(&oSc);	
	while (!POST_IsProcessingDone(&oSc));	
	POST_ClearPending(&oSc);	

#if (POST_IMAGE_BMP_SLIDE)	
	ScalerT_GetImageFromBMP(uGlibStAddr, uLcdHSz, uLcdVSz, 16, eSrcDataFmt);
	ScalerT_GetImageFromBMP(uGlibStAddr2, uLcdHSz, uLcdVSz, 17, eSrcDataFmt);

	// From RGB to YUV for pattern 1
	POST_InitIpForDmaInDmaOut(uLcdHSz, uLcdVSz, uGlibStAddr, eSrcDataFmt, 
								uLcdHSz, uLcdVSz, uPostStAddr, eMidDataFmt,
								1, FALSE, ONE_SHOT, &oSc);
	POST_StartProcessing(&oSc);
	while (!POST_IsProcessingDone(&oSc));
	UART_Printf("CSC(RGB -> YUV) is finished for pattern 1\n");
	
	// From RGB to YUV for pattern 2
	POST_InitIpForDmaInDmaOut(uLcdHSz, uLcdVSz, uGlibStAddr2, eSrcDataFmt,
								uLcdHSz, uLcdVSz, uPostStAddr2, eMidDataFmt,
								1, FALSE, ONE_SHOT, &oSc);
	POST_StartProcessing(&oSc);
	while (!POST_IsProcessingDone(&oSc));
	UART_Printf("CSC(RGB -> YUV) is fiished for pattern 2\n");
	
	// From YUV To RGB
	POST_InitIpForDmaInDmaOut(uLcdHSz, uLcdVSz, uPostStAddr2, eMidDataFmt,
								uLcdHSz, uLcdVSz, uLcdStAddr, eDstDataFmt,
								1, FALSE, FREE_RUN, &oSc);
	POST_SetNextFrameStAddr(uPostStAddr, uLcdStAddr, &oSc);
	POST_StartProcessing(&oSc);

	uFrmCnt = 0;
	while(uFrmCnt<uMaxFrameCnt)
	{
		while (!POST_IsProcessingDone(&oSc));			
		
		if (uFrmCnt%2)
			POST_SetNextFrameStAddr(uPostStAddr2, uLcdStAddr, &oSc);
		else
			POST_SetNextFrameStAddr(uPostStAddr, uLcdStAddr, &oSc);
		POST_ClearPending(&oSc);

		DelayfrTimer(milli, 1000);
		uFrmCnt++;
	}

	POST_StopProcessingOfFreeRun(&oSc);	
	while (!POST_IsProcessingDone(&oSc));	
	POST_ClearPending(&oSc);
#endif		
}


static void ScalerT_SimpleFreeRunDmaToDma_R2Y2R_Int(void)
{
	u32 uGlibStAddr, uGlibStAddr2;
	u32 uSrcBytesPerPixel, uDstBytesPerPixel;
	u32 uPostIsrCount, uPostFrameCount;
	
	UART_Printf("[ScalerT_SimpleFreeRunDmaToDma_R2Y2R_Int]\n");

	uSimpleTest = 1;
	ScalerT_SetCscTypeDmaToDma_R2Y2R();
		
	bScalerIsPollingOrInt = FALSE;

	//LCD_InitLDI(MAIN);
	
#ifdef LCD_INIT_TEST	
	LCD_InitBase();
	LCD_Start();
	LCD_InitWin(eDstDataFmt, uImgHSz, uImgVSz, 0, 0, uImgHSz, uImgVSz, 0, 0, uLcdStAddr, WIN0, false);
#else
	LCD_InitDISPC(eDstDataFmt, uLcdStAddr, WIN0, false);
	LCD_Start();
#endif

	LCD_SetWinOnOff(1, WIN0);

	LCD_GetFrmSz(&uLcdHSz, &uLcdVSz, WIN0);

	uSrcBytesPerPixel = (eSrcDataFmt == RGB16) ? 2 : 4;
	uDstBytesPerPixel = (eDstDataFmt == RGB16) ? 2 : 4;

	uGlibStAddr = uLcdStAddr + uLcdHSz*uLcdVSz*uDstBytesPerPixel;
	uNxtAddr1 = uGlibStAddr + uLcdHSz*uLcdVSz*uSrcBytesPerPixel;

	uGlibStAddr2 = uNxtAddr1 + uLcdHSz*uLcdVSz*uDstBytesPerPixel;
	uNxtAddr2 = uGlibStAddr2 + uLcdHSz*uLcdVSz*uSrcBytesPerPixel;

	GLIB_Init(uGlibStAddr, uLcdHSz, uLcdVSz, eSrcDataFmt);
	GLIB_DrawPattern(uLcdHSz, uLcdVSz);

	GLIB_Init(uGlibStAddr2, uLcdHSz, uLcdVSz, eSrcDataFmt);
	GLIB_DrawPattern2(uLcdHSz, uLcdVSz);

	// From RGB to YUV for pattern 1
	POST_InitIpForDmaInDmaOut(uLcdHSz, uLcdVSz, uGlibStAddr, eSrcDataFmt, 
								uLcdHSz, uLcdVSz, uNxtAddr1, eMidDataFmt,
								1, FALSE, ONE_SHOT, &oSc);
	POST_StartProcessing(&oSc);
	while (!POST_IsProcessingDone(&oSc));
	UART_Printf("CSC(RGB -> YUV) is finished for pattern 1\n");

	// From RGB to YUV for pattern 2
	POST_InitIpForDmaInDmaOut(uLcdHSz, uLcdVSz, uGlibStAddr2, eSrcDataFmt,
								uLcdHSz, uLcdVSz, uNxtAddr2, eMidDataFmt,
								1, FALSE, ONE_SHOT, &oSc);
	POST_StartProcessing(&oSc);
	while (!POST_IsProcessingDone(&oSc));
	UART_Printf("CSC(RGB -> YUV) is finished for pattern 2\n");

	//Interrupt Setting
	uPostIsrCount = 0;
	uPostFrameCount = 0;
	uScalerIntOccured = FALSE;
	//INTC_InitIp();
	//INTC_SetHandlerAndUnmask(INT_POST, ScalerISRDma);
	INTC_Enable(NUM_SCALER);
	INTC_SetVectAddr(NUM_SCALER, ScalerISRDma);
	POST_EnableInterrupt(POST_LEVEL_INT, &oSc);
	
	// From YUV To RGB
	POST_InitIpForDmaInDmaOut(uLcdHSz, uLcdVSz, uNxtAddr1, eMidDataFmt,
								uLcdHSz, uLcdVSz, uLcdStAddr, eDstDataFmt, 
								1, FALSE, FREE_RUN, &oSc);
	POST_SetNextFrameStAddr(uNxtAddr2, uLcdStAddr, &oSc);

	POST_StartProcessing(&oSc);
	
	while(uPostFrameCount < uMaxFrameCnt)
	{
		if(uScalerIntOccured == TRUE)
		{
			uPostIsrCount++;
			if(uPostIsrCount > 200)
			{
				uPostFrameCount++;
				
			 	if (uPostFrameCount % 2)
					POST_SetNextFrameStAddr(uNxtAddr2, uLcdStAddr, &oSc);
				else 
					POST_SetNextFrameStAddr(uNxtAddr1, uLcdStAddr, &oSc);

				if (uPostFrameCount == uMaxFrameCnt)
				{
					POST_StopProcessingOfFreeRun(&oSc);	
					while (!POST_IsProcessingDone(&oSc));	
					uScalerIntOccured = FALSE;
					break;
				}
				uPostIsrCount = 0;
			}
			uScalerIntOccured = FALSE;
		}
	}


#if (POST_IMAGE_BMP_SLIDE)	
	ScalerT_GetImageFromBMP(uGlibStAddr, uLcdHSz, uLcdVSz, 18, eSrcDataFmt);
	ScalerT_GetImageFromBMP(uGlibStAddr2, uLcdHSz, uLcdVSz, 19, eSrcDataFmt);

	// From RGB to YUV for pattern 1
	POST_InitIpForDmaInDmaOut(uLcdHSz, uLcdVSz, uGlibStAddr, eSrcDataFmt, 
								uLcdHSz, uLcdVSz, uNxtAddr1, eMidDataFmt,
								1, FALSE, ONE_SHOT, &oSc);
	POST_StartProcessing(&oSc);
	while (!POST_IsProcessingDone(&oSc));
	UART_Printf("CSC(RGB -> YUV) is finished for pattern 1\n");

	// From RGB to YUV for pattern 2
	POST_InitIpForDmaInDmaOut(uLcdHSz, uLcdVSz, uGlibStAddr2, eSrcDataFmt,
								uLcdHSz, uLcdVSz, uNxtAddr2, eMidDataFmt,
								1, FALSE, ONE_SHOT, &oSc);
	POST_StartProcessing(&oSc);
	while (!POST_IsProcessingDone(&oSc));
	UART_Printf("CSC(RGB -> YUV) is finished for pattern 2\n");

	//Interrupt Setting
	uPostIsrCount = 0;
	uPostFrameCount = 0;
	uScalerIntOccured = FALSE;
	//INTC_InitIp();
	//INTC_SetHandlerAndUnmask(INT_POST, ScalerISRDma);
	INTC_Enable(NUM_SCALER);
	INTC_SetVectAddr(NUM_SCALER, ScalerISRDma);
	POST_EnableInterrupt(POST_LEVEL_INT, &oSc);
	
	// From YUV To RGB
	POST_InitIpForDmaInDmaOut(uLcdHSz, uLcdVSz, uNxtAddr1, eMidDataFmt,
								uLcdHSz, uLcdVSz, uLcdStAddr, eDstDataFmt, 
								1, FALSE, FREE_RUN, &oSc);
	POST_SetNextFrameStAddr(uNxtAddr2, uLcdStAddr, &oSc);

	POST_StartProcessing(&oSc);
	
	while(uPostFrameCount < uMaxFrameCnt)
	{
		if(uScalerIntOccured == TRUE)
		{
			uPostIsrCount++;
			if(uPostIsrCount > 200)
			{
				uPostFrameCount++;
				
			 	if (uPostFrameCount % 2)
					POST_SetNextFrameStAddr(uNxtAddr2, uLcdStAddr, &oSc);
				else 
					POST_SetNextFrameStAddr(uNxtAddr1, uLcdStAddr, &oSc);

				if (uPostFrameCount == uMaxFrameCnt)
				{
					POST_StopProcessingOfFreeRun(&oSc);	
					while (!POST_IsProcessingDone(&oSc));	
					uScalerIntOccured = FALSE;
					break;
				}
				uPostIsrCount = 0;
			}
			uScalerIntOccured = FALSE;
		}
	}
#endif	

	POST_DisableInterrupt(&oSc);
	POST_ClearPending(&oSc);	
	//INTC_Mask(INT_POST);			
	INTC_Disable(NUM_SCALER);
}


static void ScalerT_ComplexFreeRunDmaToDma_R2Y2R_Polling(void)
{
	u32 uGlibStAddr, uGlibStAddr2, uPostStAddr, uPostStAddr2;
	u32 uSrcBytesPerPixel, uDstBytesPerPixel;
	u32 uFrmCnt;

	UART_Printf("[ScalerT_ComplexFreeRunDmaToDma_R2Y2R_Polling]\n");

	uSimpleTest = 0;
	ScalerT_SetCscTypeDmaToDma_R2Y2R();
	
	bScalerIsPollingOrInt = true;	
	ePostRunMode = FREE_RUN;
	
	//LCD_InitLDI(MAIN);
#ifdef LCD_INIT_TEST	
	LCD_InitBase();
	LCD_Start();
	LCD_InitWin(eDstDataFmt, uImgHSz, uImgVSz, 0, 0, uImgHSz, uImgVSz, 0, 0, uLcdStAddr, WIN0, false);
#else
	LCD_InitDISPC(eDstDataFmt, uLcdStAddr, WIN0, false);
	LCD_Start();
#endif
	LCD_SetWinOnOff(1, WIN0);
	LCD_GetFrmSz(&uLcdHSz, &uLcdVSz, WIN0);

	uSrcBytesPerPixel = (eSrcDataFmt == RGB24) ? 4 : 2;
	uDstBytesPerPixel = (eDstDataFmt == RGB24) ? 4 : 2;

	uGlibStAddr = uLcdStAddr + uLcdHSz*uLcdVSz*uDstBytesPerPixel;
	uPostStAddr = uGlibStAddr + uImgHSz*uImgVSz*uSrcBytesPerPixel;

	uGlibStAddr2 = uPostStAddr + uMidImgHSz*uMidImgVSz*uDstBytesPerPixel;
	uPostStAddr2 = uGlibStAddr2 + uImgHSz*uImgVSz*uSrcBytesPerPixel;

	// clear the frame
	GLIB_Init(uLcdStAddr, uLcdHSz, uLcdVSz, eDstDataFmt);
	GLIB_ClearFrame(C_BLACK);

	// Draw Pattern 1
	GLIB_Init(uGlibStAddr, uImgHSz, uImgVSz, eSrcDataFmt);
	GLIB_DrawPattern(uImgHSz, uImgVSz);

	// Draw Pattern 2
	GLIB_Init(uGlibStAddr2, uImgHSz, uImgVSz, eSrcDataFmt);
	GLIB_DrawPattern2(uImgHSz, uImgVSz);

	// CSC From RGB To YUV For Pattern 1
	POST_InitIp1(	uImgHSz, uImgVSz, uSrcStartX, uSrcStartY, uSrcCroppedHSz, uSrcCroppedVSz, uGlibStAddr, eSrcDataFmt,
					uMidImgHSz, uMidImgVSz, uMidStartX, uMidStartY, uMidScaledHSz, uMidScaledVSz, uPostStAddr, eMidDataFmt,
					1, FALSE, ONE_SHOT, POST_DMA, POST_DMA, &oSc);
	POST_StartProcessing(&oSc);
	while (!POST_IsProcessingDone(&oSc));
	
	// CSC From RGB To YUV For Pattern 2
	POST_InitIp1(	uImgHSz, uImgVSz, uSrcStartX, uSrcStartY, uSrcCroppedHSz, uSrcCroppedVSz, uGlibStAddr2, eSrcDataFmt,
					uMidImgHSz, uMidImgVSz, uMidStartX, uMidStartY, uMidScaledHSz, uMidScaledVSz, uPostStAddr2, eMidDataFmt,
					1, false, ONE_SHOT, POST_DMA, POST_DMA, &oSc);
	POST_StartProcessing(&oSc);
	while (!POST_IsProcessingDone(&oSc));
	
	// From YUV To RGB
	POST_InitIp1(	uMidImgHSz, uMidImgVSz, uMidStartX, uMidStartY, uMidScaledHSz, uMidScaledVSz, uPostStAddr, eMidDataFmt,
					uLcdHSz, uLcdVSz, uDstStartX, uDstStartY, uDstScaledHSz, uDstScaledVSz, uLcdStAddr, eDstDataFmt, 
					1, false, FREE_RUN, POST_DMA, POST_DMA, &oSc); // src. frm. buf. num: 1, double buffering: disable
	POST_SetNextFrameStAddr(uPostStAddr2, uLcdStAddr, &oSc);
	POST_StartProcessing(&oSc);

	uFrmCnt = 0;
	while(uFrmCnt<uMaxFrameCnt)
	{
		while (!POST_IsProcessingDone(&oSc));			
		
		if (uFrmCnt%2)
			POST_SetNextFrameStAddr(uPostStAddr, uLcdStAddr, &oSc);
		else
			POST_SetNextFrameStAddr(uPostStAddr2, uLcdStAddr, &oSc);
		POST_ClearPending(&oSc);

		DelayfrTimer(milli, 500);
		uFrmCnt++;
	}

	POST_StopProcessingOfFreeRun(&oSc);	
	while (!POST_IsProcessingDone(&oSc));	
}


static void ScalerT_ComplexFreeRunDmaToDma_R2Y2R_Int(void)
{
	u32 uGlibStAddr, uGlibStAddr2;
	u32 uSrcBytesPerPixel, uDstBytesPerPixel;
	u32 uPostIsrCount, uPostFrameCount;

	UART_Printf("[ScalerT_ComplexFreeRunDmaToDma_R2Y2R_Int]\n");

	uSimpleTest = 0;
	ScalerT_SetCscTypeDmaToDma_R2Y2R();
	
	bScalerIsPollingOrInt = false;
	ePostRunMode = FREE_RUN;

	//LCD_InitLDI(MAIN);
#ifdef LCD_INIT_TEST	
	LCD_InitBase();
	LCD_Start();
	LCD_InitWin(eDstDataFmt, uImgHSz, uImgVSz, 0, 0, uImgHSz, uImgVSz, 0, 0, uLcdStAddr, WIN0, false);
#else
	LCD_InitDISPC(eDstDataFmt, uLcdStAddr, WIN0, false);
	LCD_Start();
#endif
	LCD_SetWinOnOff(1, WIN0);
	LCD_GetFrmSz(&uLcdHSz, &uLcdVSz, WIN0);

	uSrcBytesPerPixel = (eSrcDataFmt == RGB24) ? 4 : 2;
	uDstBytesPerPixel = (eDstDataFmt == RGB24) ? 4 : 2;

	uGlibStAddr = uLcdStAddr + uLcdHSz*uLcdVSz*uDstBytesPerPixel;
	uNxtAddr1 = uGlibStAddr + uImgHSz*uImgVSz*uSrcBytesPerPixel;

	uGlibStAddr2 = uNxtAddr1 + uMidImgHSz*uMidImgVSz*uDstBytesPerPixel;
	uNxtAddr2 = uGlibStAddr2 + uImgHSz*uImgVSz*uSrcBytesPerPixel;

	// clear the frame
	GLIB_Init(uLcdStAddr, uLcdHSz, uLcdVSz, eDstDataFmt);
	GLIB_ClearFrame(C_BLACK);

	// Draw Pattern 1
	GLIB_Init(uGlibStAddr, uImgHSz, uImgVSz, eSrcDataFmt);
	GLIB_DrawPattern(uImgHSz, uImgVSz);

	// Draw Pattern 2
	GLIB_Init(uGlibStAddr2, uImgHSz, uImgVSz, eSrcDataFmt);
	GLIB_DrawPattern2(uImgHSz, uImgVSz);

	// CSC From RGB To YUV For Pattern 1
	POST_InitIp1(	uImgHSz, uImgVSz, uSrcStartX, uSrcStartY, uSrcCroppedHSz, uSrcCroppedVSz, uGlibStAddr, eSrcDataFmt,
					uMidImgHSz, uMidImgVSz, uMidStartX, uMidStartY, uMidScaledHSz, uMidScaledVSz, uNxtAddr1, eMidDataFmt,
					1, false, ONE_SHOT, POST_DMA, POST_DMA, &oSc);
	POST_StartProcessing(&oSc);
	while (!POST_IsProcessingDone(&oSc));
	
	// CSC From RGB To YUV For Pattern 2
	POST_InitIp1(	uImgHSz, uImgVSz, uSrcStartX, uSrcStartY, uSrcCroppedHSz, uSrcCroppedVSz, uGlibStAddr2, eSrcDataFmt,
					uMidImgHSz, uMidImgVSz, uMidStartX, uMidStartY, uMidScaledHSz, uMidScaledVSz, uNxtAddr2, eMidDataFmt,
					1, false, ONE_SHOT, POST_DMA, POST_DMA, &oSc);
	POST_StartProcessing(&oSc);
	while (!POST_IsProcessingDone(&oSc));

	//Interrupt Setting
	uPostIsrCount = 0;
	uPostFrameCount = 0;
	uScalerIntOccured = FALSE;
	//INTC_InitIp();
	//INTC_SetHandlerAndUnmask(INT_POST, ScalerISRDma);
	INTC_Enable(NUM_SCALER);
	INTC_SetVectAddr(NUM_SCALER, ScalerISRDma);
	POST_EnableInterrupt(POST_LEVEL_INT, &oSc);
	
	// From YUV To RGB
	POST_InitIp1(	uMidImgHSz, uMidImgVSz, uMidStartX, uMidStartY, uMidScaledHSz, uMidScaledVSz, uNxtAddr1, eMidDataFmt,
					uLcdHSz, uLcdVSz, uDstStartX, uDstStartY, uDstScaledHSz, uDstScaledVSz, uLcdStAddr, eDstDataFmt,
					1, false, FREE_RUN, POST_DMA, POST_DMA, &oSc); // src. frm. buf. num: 1 double buffering: disable
	POST_SetNextFrameStAddr(uNxtAddr2, uLcdStAddr, &oSc);
	POST_StartProcessing(&oSc);

	while(uPostFrameCount < uMaxFrameCnt)
	{
		if(uScalerIntOccured == TRUE)
		{
			uPostIsrCount++;
			if(uPostIsrCount > 200)
			{
				uPostFrameCount++;
				
			 	if (uPostFrameCount % 2)
					POST_SetNextFrameStAddr(uNxtAddr2, uLcdStAddr, &oSc);
				else 
					POST_SetNextFrameStAddr(uNxtAddr1, uLcdStAddr, &oSc);

				if (uPostFrameCount == uMaxFrameCnt)
				{
					POST_StopProcessingOfFreeRun(&oSc);	
					while (!POST_IsProcessingDone(&oSc));	
					uScalerIntOccured = FALSE;
					break;
				}
				uPostIsrCount = 0;
			}
			uScalerIntOccured = FALSE;
		}
	}

	POST_DisableInterrupt(&oSc);
	POST_ClearPending(&oSc);	
	//INTC_Mask(INT_POST);
	INTC_Disable(NUM_SCALER);			
}


const testFuncMenu scaler_r2y2r_menu[] =
{
	ScalerT_SimpleDmaToDma_R2Y2R,                       		"Simple DMA CSC Test \'RGB -> YUV -> RGB\'",
	ScalerT_ComplexDmaToDma_R2Y2R,                     		"Complex DMA CSC Test \'RGB -> YUV -> RGB\'",
	ScalerT_SimpleFreeRunDmaToDma_R2Y2R_Polling,		"Simple FreeRun And CSC Test \'RGB -> YUV -> RGB\' By Using Polling",						
	ScalerT_SimpleFreeRunDmaToDma_R2Y2R_Int,            	"Simple FreeRun And CSC Test \'RGB -> YUV -> RGB\' By Using Interrupt",		
	ScalerT_ComplexFreeRunDmaToDma_R2Y2R_Polling,	"Complex FreeRun And CSC Test \'RGB -> YUV -> RGB\' By Using Polling",		
	ScalerT_ComplexFreeRunDmaToDma_R2Y2R_Int,       	"Complex FreeRun And CSC Test \'RGB -> YUV -> RGB\' By Using Interrupt",				
	0,                                          0
};


static void ScalerT_ConvertRGBToYUVToRGB(void)
{
	u32 i;
	s32 uSel;
			
	while(1)
	{
		UART_Printf("\n");
		for (i=0; (int)(scaler_r2y2r_menu[i].desc)!=0; i++)
		{
			UART_Printf("%2d: %s\n", i, scaler_r2y2r_menu[i].desc);
		}

		UART_Printf("\n");
		UART_Printf("Select The Function To Test: ");

		uSel = UART_GetIntNum();
		UART_Printf("\n");

		if (uSel == -1)
			break;
		if (uSel>=0 && uSel<(sizeof(scaler_r2y2r_menu)/8-1))
			(scaler_r2y2r_menu[uSel].func)();
	}
}


////////////////////////////////////////////////////////////////////////////
//////////////////                         YUV to YUV                          ///////////////////
////////////////////////////////////////////////////////////////////////////


static void ScalerT_SetCscTypeDma_Y2Y(void)
{
	u32 nSrcDataFmt, nDstDataFmt;

	while (1)
	{
		UART_Printf("\n");
		UART_Printf("[1] YCbYCr422 Interleave\n");
		UART_Printf("[2] YCrYCb422 Interleave\n");
		UART_Printf("[3] CbYCrY422 Interleave\n");
		UART_Printf("[4] CrYCbY422 Interleave\n");
		UART_Printf("[5] YUV420 Non-Interleave\n");			

		UART_Printf("Enter Source Data Format From YUV To YUV: ");
		
		nSrcDataFmt = UART_GetIntNum();

		if (nSrcDataFmt >= 1 && nSrcDataFmt <= 5)
		{
			eSrcDataFmt_Yuv = 
				(nSrcDataFmt == 1) ? YCBYCR :
				(nSrcDataFmt == 2) ? YCRYCB :
				(nSrcDataFmt == 3) ? CBYCRY :
				(nSrcDataFmt == 4) ? CRYCBY : YC420;
				
			break;
		}
		else
			UART_Printf("Invalid Data Format! Retry It!!");
	}

	while (1)
	{
		UART_Printf("\n");
		UART_Printf("[1] YCbYCr422 Interleave\n");
		UART_Printf("[2] YCrYCb422 Interleave\n");
		UART_Printf("[3] CbYCrY422 Interleave\n");
		UART_Printf("[4] CrYCbY422 Interleave\n");
		UART_Printf("[5] YUV420 Non-Interleave\n");			

		UART_Printf("Enter Destination Data Format From YUV To YUV: ");

		nDstDataFmt = UART_GetIntNum();

		if (nDstDataFmt >= 1 && nDstDataFmt <= 5)
		{
			eDstDataFmt_Yuv = 
				(nDstDataFmt == 1) ? YCBYCR :
				(nDstDataFmt == 2) ? YCRYCB :
				(nDstDataFmt == 3) ? CBYCRY :
				(nDstDataFmt == 4) ? CRYCBY : YC420;
			
			break;
		}
		else
			UART_Printf("Invalid Data Format! Retry It!!");
	}

	if (eSrcDataFmt_Yuv == YCBYCR)
	{
		uImgHSz = 240, uImgVSz = 320, uSrcStartX = 0, uSrcStartY = 0;
		uSrcCroppedHSz = 240, uSrcCroppedVSz = 320;
#if 1
		// H-Size : Scale Up, V-Size : Scale Up
		uMidImgHSz = 240, uMidImgVSz = 320, uMidStartX = 60, uMidStartY = 40;
		uMidScaledHSz = 120, uMidScaledVSz = 120;

		uMid2ImgHSz = 240, uMid2ImgVSz = 320, uMid2StartX = 64, uMid2StartY = 64;
		uMid2ScaledHSz = 152, uMid2ScaledVSz = 200;
#else
		// H-Size : Scale Up, V-Size : Scale Up
		uMidImgHSz = 240, uMidImgVSz = 320, uMidStartX = 2, uMidStartY = 2;
		uMidScaledHSz = 120, uMidScaledVSz = 120;

		uMid2ImgHSz = 240, uMid2ImgVSz = 320, uMid2StartX = 8, uMid2StartY = 4;
		uMid2ScaledHSz = 160, uMid2ScaledVSz = 150;
#endif
		uDstStartX = 0, 	uDstStartY = 0, uDstScaledHSz = 320, uDstScaledVSz = 240;
	}
	else if (eSrcDataFmt_Yuv == YCRYCB)
	{
		uImgHSz = 240, uImgVSz = 320, uSrcStartX = 0, uSrcStartY = 0;
		uSrcCroppedHSz = 240, uSrcCroppedVSz = 320;

		// H-Size : Scale Down, V-Size : Scale Down
		uMidImgHSz = 240, uMidImgVSz = 320, uMidStartX = 0, uMidStartY = 0;
		uMidScaledHSz = 240, uMidScaledVSz = 320;

		uMid2ImgHSz = 240, uMid2ImgVSz = 320, uMid2StartX = 16, uMid2StartY = 16;
		uMid2ScaledHSz = 160, uMid2ScaledVSz = 200;
		
		uDstStartX = 0, 	uDstStartY = 0, uDstScaledHSz = 320, uDstScaledVSz = 240;	
	}
	else if (eSrcDataFmt_Yuv == CBYCRY)
	{
		uImgHSz = 240, uImgVSz = 320, uSrcStartX = 0, uSrcStartY = 0;
		uSrcCroppedHSz = 240, uSrcCroppedVSz = 320;

		// H-Size : Scale Up, V-Size : Scale Down
		uMidImgHSz = 240, uMidImgVSz = 320, uMidStartX = 32, uMidStartY = 32;
		uMidScaledHSz = 120, uMidScaledVSz = 200;

		uMid2ImgHSz = 240, uMid2ImgVSz = 320, uMid2StartX = 24, uMid2StartY = 24;
		uMid2ScaledHSz = 200, uMid2ScaledVSz = 120;
		
		uDstStartX = 0, 	uDstStartY = 0, uDstScaledHSz = 320, uDstScaledVSz = 240;	
	}
	else if (eSrcDataFmt_Yuv == CRYCBY)
	{
		uImgHSz = 240, uImgVSz = 320, uSrcStartX = 0, uSrcStartY = 0;
		uSrcCroppedHSz = 240, uSrcCroppedVSz = 320;

		// H-Size : Scale Down, V-Size : Scale Up
		uMidImgHSz = 240, uMidImgVSz = 320, uMidStartX = 0, uMidStartY = 40;
		uMidScaledHSz = 200, uMidScaledVSz = 160;

		uMid2ImgHSz = 240, uMid2ImgVSz = 320, uMid2StartX = 40, uMid2StartY = 40;
		uMid2ScaledHSz = 160, uMid2ScaledVSz = 280;
		
		uDstStartX = 0, 	uDstStartY = 0, uDstScaledHSz = 320, uDstScaledVSz = 240;	}	
	else if (eSrcDataFmt_Yuv == YC420)
	{
		uImgHSz = 240, uImgVSz = 320, uSrcStartX = 0, uSrcStartY = 0;
		uSrcCroppedHSz = 240, uSrcCroppedVSz = 320;

		// H-Size : Scale Down, V-Size : Scale Down
		uMidImgHSz = 240, uMidImgVSz = 320, uMidStartX = 0, uMidStartY = 0;
		uMidScaledHSz = 240, uMidScaledVSz = 320;

		uMid2ImgHSz = 240, uMid2ImgVSz = 320, uMid2StartX = 0, uMid2StartY = 0;
		uMid2ScaledHSz = 160, uMid2ScaledVSz = 200;
		
		uDstStartX = 0, 	uDstStartY = 0, uDstScaledHSz = 320, uDstScaledVSz = 240;	
	}	

	if(uSimpleTest == 0)
	{
		UART_Printf("=========    Complex_Test Image Size    =========\n");
		UART_Printf("MidImgHSz     = %d,       MidImgVSz     = %d\n",uMidImgHSz,uMidImgVSz);
		UART_Printf("MidStartX     = %d,       MidStartY     = %d\n",uMidStartX,uMidStartY);
		UART_Printf("MidCroppedHSz = %d,       MidCroppedVSz = %d\n",uMidScaledHSz,uMidScaledVSz);
		UART_Printf("\n");
		UART_Printf("Mid2StartX     = %d,       Mid2StartY     = %d\n",uMid2StartX,uMid2StartY);
		UART_Printf("Mid2ScaledHSz  = %d,       Mid2ScaledVSz  = %d\n",uMid2ScaledHSz,uMid2ScaledVSz);	
		UART_Printf("=================================================\n");
		UART_Printf("\n");
	}	
}


static void ScalerT_SimpleDmaToDma_Y2Y(void)
{
	u32 uGlibStAddr, uSrcStAddr, uDstStAddr;

	UART_Printf("[ScalerT_SimpleDmaToDma_Y2Y]\n");

	uSimpleTest = 1;
	ScalerT_SetCscTypeDma_Y2Y();

	eDstDataFmt = RGB24;
	//LCD_InitLDI(MAIN);
#ifdef LCD_INIT_TEST	
	LCD_InitBase();
	LCD_Start();
	LCD_InitWin(eDstDataFmt, uImgHSz, uImgVSz, 0, 0, uImgHSz, uImgVSz, 0, 0, uLcdStAddr, WIN0, false);
#else
	LCD_InitDISPC(eDstDataFmt, uLcdStAddr, WIN0, false);
	LCD_Start();
#endif
	LCD_SetWinOnOff(1, WIN0);
	LCD_GetFrmSz(&uLcdHSz, &uLcdVSz, WIN0);

	uImgHSz = uLcdHSz; 
	uImgVSz = uLcdVSz;
	uDstScaledHSz = uLcdHSz; 
	uDstScaledVSz = uLcdVSz;
	
	uGlibStAddr = uLcdStAddr + uImgHSz*uImgVSz*4;
	uSrcStAddr = uGlibStAddr + uImgHSz*uImgVSz*4;
	uDstStAddr = uSrcStAddr + uImgHSz*uImgVSz*4;

	GLIB_Init(uGlibStAddr, uImgHSz, uImgVSz, RGB24);
	GLIB_DrawPattern(uImgHSz, uImgVSz);

	// From RGB24 To YUV
	POST_InitIpForDmaInDmaOut(	uImgHSz, uImgVSz, uGlibStAddr, RGB24, 
									uImgHSz, uImgVSz, uSrcStAddr, eSrcDataFmt_Yuv,
									1, FALSE, ONE_SHOT, &oSc);
	POST_StartProcessing(&oSc);
	while (!POST_IsProcessingDone(&oSc));

	SaveToFile(POST_YUV_FILE_NAME1, uLcdHSz*uLcdVSz*2, uSrcStAddr);

	// From YUV To YUV
	UART_Printf("========   YUV -> YUV   =========\n");
	UART_Printf("Source Address : 0x%08x\n", uSrcStAddr);
	UART_Printf("Destination Address : 0x%08x\n", uDstStAddr);	
	POST_InitIpForDmaInDmaOut(	uImgHSz, uImgVSz, uSrcStAddr, eSrcDataFmt_Yuv, 
									uImgHSz, uImgVSz, uDstStAddr, eDstDataFmt_Yuv,
									1, FALSE, ONE_SHOT, &oSc);
	POST_StartProcessing(&oSc);
	while (!POST_IsProcessingDone(&oSc));

	SaveToFile(POST_YUV_FILE_NAME2, uLcdHSz*uLcdVSz*2, uDstStAddr);

	// From YUV To RGB24
	POST_InitIpForDmaInDmaOut(	uImgHSz, uImgVSz, uDstStAddr, eDstDataFmt_Yuv, 
									uImgHSz, uImgVSz, uLcdStAddr, eDstDataFmt,
									1, FALSE, ONE_SHOT, &oSc);
	POST_StartProcessing(&oSc);
	while (!POST_IsProcessingDone(&oSc));
	
#if (POST_IMAGE_BMP_SLIDE)	
	{
		u32 uImageNum;
		
		UART_Printf("Input the any key to display the other image ('q' : quit)\n");
		uImageNum = 0;
		while(UART_Getc() != 'q')
		{
			ScalerT_GetImageFromBMP(uGlibStAddr, uLcdHSz, uLcdVSz, uImageNum, RGB24);

			// From RGB24 To YUV
			POST_InitIpForDmaInDmaOut(	uImgHSz, uImgVSz, uGlibStAddr, RGB24, 
											uImgHSz, uImgVSz, uSrcStAddr, eSrcDataFmt_Yuv,
											1, FALSE, ONE_SHOT, &oSc);
			POST_StartProcessing(&oSc);
			while (!POST_IsProcessingDone(&oSc));

			SaveToFile(POST_YUV_FILE_NAME1, uLcdHSz*uLcdVSz*2, uSrcStAddr);
			
			// From YUV To YUV
			POST_InitIpForDmaInDmaOut(	uImgHSz, uImgVSz, uSrcStAddr, eSrcDataFmt_Yuv, 
											uImgHSz, uImgVSz, uDstStAddr, eDstDataFmt_Yuv,
											1, FALSE, ONE_SHOT, &oSc);
			POST_StartProcessing(&oSc);
			while (!POST_IsProcessingDone(&oSc));

			SaveToFile(POST_YUV_FILE_NAME2, uLcdHSz*uLcdVSz*2, uDstStAddr);
			
			// From YUV To RGB24
			POST_InitIpForDmaInDmaOut(	uImgHSz, uImgVSz, uDstStAddr, eDstDataFmt_Yuv, 
											uImgHSz, uImgVSz, uLcdStAddr, eDstDataFmt,
											1, FALSE, ONE_SHOT, &oSc);
			POST_StartProcessing(&oSc);
			while (!POST_IsProcessingDone(&oSc));
			uImageNum++;
		}
	}
#endif	

}


static void ScalerT_ComplexIDmaToDma_Y2Y(void)
{
	u32 uGlibStAddr1, uSrcStAddr, uDstStAddr;
//	u32 uBytesPerPixel;

	UART_Printf("[ScalerT_ComplexIDmaToDma_Y2Y]\n");

	uSimpleTest = 0;
	ScalerT_SetCscTypeDma_Y2Y();

	eDstDataFmt = RGB24;
	//LCD_InitLDI(MAIN);
#ifdef LCD_INIT_TEST	
	LCD_InitBase();
	LCD_Start();
	LCD_InitWin(eDstDataFmt, uImgHSz, uImgVSz, 0, 0, uImgHSz, uImgVSz, 0, 0, uLcdStAddr, WIN0, false);
#else
	LCD_InitDISPC(eDstDataFmt, uLcdStAddr, WIN0, false);
	LCD_Start();
#endif
	LCD_SetWinOnOff(1, WIN0);
	LCD_GetFrmSz(&uLcdHSz, &uLcdVSz, WIN0);
	
	//uGlibStAddr1 = uLcdStAddr + uImgHSz*uImgVSz*4;
	uGlibStAddr1 = uLcdStAddr + uLcdHSz*uLcdVSz*4;
	uSrcStAddr = uGlibStAddr1 + uImgHSz*uImgVSz*4;
	uDstStAddr = uSrcStAddr + uMidImgHSz*uMidImgVSz*4;
	
	// To Clear the LCD Display
	GLIB_Init(uLcdStAddr, uLcdHSz, uLcdVSz, RGB24);
	GLIB_ClearFrame(C_BLACK);

	// To Clear the uDstStAddr
	GLIB_Init(uDstStAddr, uMid2ImgHSz, uMid2ImgVSz, RGB24);
	GLIB_ClearFrame(C_BLACK);	//YCbCr Data 0x00000000 -> RGB24 Data 0x00008100(not Block color)
	
#if (POST_IMAGE_SOURCE == POST_IMAGE_PATTERN)
	GLIB_Init(uGlibStAddr1, uImgHSz, uImgVSz, RGB24);
	GLIB_DrawPattern(uImgHSz, uImgVSz);
#elif (POST_IMAGE_SOURCE == POST_IMAGE_HEADER)
	ScalerT_GetImageFromHeader(uGlibStAddr1, 240, 320, RGB24);
#endif

	// From RGB24 To YUV
	POST_InitIpForDmaInDmaOut(	uImgHSz, uImgVSz, uGlibStAddr1, RGB24, 
									uMidImgHSz, uMidImgVSz, uSrcStAddr, eSrcDataFmt_Yuv,
									1, FALSE, ONE_SHOT, &oSc);
	POST_StartProcessing(&oSc);
	while (!POST_IsProcessingDone(&oSc));

	// From YUV To YUV
	UART_Printf("========   YUV -> YUV   =========\n");
	UART_Printf("Source Address : 0x%08x\n", uSrcStAddr);
	UART_Printf("Destination Address : 0x%08x\n", uDstStAddr);	
	POST_InitIp1(	uMidImgHSz, uMidImgVSz, uMidStartX, uMidStartY, uMidScaledHSz, uMidScaledVSz, uSrcStAddr, eSrcDataFmt_Yuv,
					uMid2ImgHSz, uMid2ImgVSz, uMid2StartX, uMid2StartY, uMid2ScaledHSz, uMid2ScaledVSz, uDstStAddr, eDstDataFmt_Yuv,
					1, FALSE, ONE_SHOT, POST_DMA, POST_DMA, &oSc);
	
	POST_StartProcessing(&oSc);
	while (!POST_IsProcessingDone(&oSc));

	// From YUV To RGB24
#if 0	
	POST_InitIp1(	uMid2ImgHSz, uMid2ImgVSz, uMid2StartX, uMid2StartY, uMid2ScaledHSz, uMid2ScaledVSz, uDstStAddr, eDstDataFmt_Yuv,
					uImgHSz, uImgVSz, 0, 0, uImgHSz, uImgVSz, uLcdStAddr, RGB24,
					1, FALSE, ONE_SHOT, POST_DMA, POST_DMA, &oSc);	
#else
	POST_InitIpForDmaInDmaOut(	uMid2ImgHSz, uMid2ImgVSz, uDstStAddr, eDstDataFmt_Yuv, 
									uLcdHSz, uLcdVSz, uLcdStAddr, eDstDataFmt,
									1, FALSE, ONE_SHOT, &oSc);
#endif
	POST_StartProcessing(&oSc);
	while (!POST_IsProcessingDone(&oSc));
}

static void ScalerT_ScaleUpDown_YUV(void)
{
	u32 uGlibStAddr1, uSrcStAddr, uDstStAddr;
	u32 uScaleOffsetX, uScaleOffsetY;
	s32 nSelDmaCscType_Y2Y;
	u32 uHOffset, uVOffset, uPreHOffset, uPreVOffset, uHRatio, uVRatio;	
	u8 uScale;	

	u32 i;
	u32 *pBufAddr32;

	UART_Printf("[ScalerT_ScaleUpDown_YUV]\n");

	while(1)
	{
		UART_Printf("\n");
		UART_Printf("[1] YCbYCr422\n");
		UART_Printf("[2] YCrYCb422\n");
		UART_Printf("[3] CbYCrY422\n");
		UART_Printf("[4] CrYCbY422\n");
		UART_Printf("[5] YCbCr420\n");		
		UART_Printf("\n");

		UART_Printf(">>Select The Source Image Type: ");

		nSelDmaCscType_Y2Y = UART_GetIntNum();

		if (nSelDmaCscType_Y2Y >= 1 && nSelDmaCscType_Y2Y <= 5)
		{
			eSrcDataFmt_Yuv = 
				(nSelDmaCscType_Y2Y == 1) ? YCBYCR :
				(nSelDmaCscType_Y2Y == 2) ? YCRYCB :
				(nSelDmaCscType_Y2Y == 3) ? CBYCRY :
				(nSelDmaCscType_Y2Y == 4) ? CRYCBY : YC420;
#if 0
			if(nSelDmaCscType_Y2Y < 5)
				uOffset = 4;		//SRC_Width & DST_Width must be the multiple of 2 (YCbYCr422)
			else
				uOffset = 8;		//SRC_Width & DST_Width must be the multiple of 8 (YCbYCr420)
#endif
			break;
		}
		else 
			UART_Printf("Invalid Input! Retry It!!\n");
		
	}

	//LCD_InitLDI(MAIN);
	LCD_InitDISPC(RGB24, uLcdStAddr, WIN0, false);
	LCD_Start();
	LCD_SetWinOnOff(1, WIN0);
	LCD_GetFrmSz(&uLcdHSz, &uLcdVSz, WIN0);
	
	uImgHSz = uLcdHSz;	
	uImgVSz = uLcdVSz;
	uSrcStartX = 0; 
	uSrcStartY = 0;
	uSrcCroppedHSz = uLcdHSz;	
	uSrcCroppedVSz = uLcdVSz;
	
	uMidImgHSz = 2048;
	uMidImgVSz = 2048;
	uMidStartX = 0;
	uMidStartY = 0;
	uMidScaledHSz = 2048;
	uMidScaledVSz = 2048;
		
	uDstStartX = 0; 
	uDstStartY = 0;
	uDstScaledHSz = uLcdHSz, 
	uDstScaledVSz = uLcdVSz;	
	
	//uGlibStAddr1 = uLcdStAddr + uImgHSz*uImgVSz*4;
	uGlibStAddr1 = uLcdStAddr + uLcdHSz*uLcdVSz*4;
	uSrcStAddr = uGlibStAddr1 + uImgHSz*uImgVSz*4;
	uDstStAddr = uSrcStAddr + uMidImgHSz*uMidImgVSz*4;
	
	// To Clear the LCD Display
	GLIB_Init(uLcdStAddr, uLcdHSz, uLcdVSz, RGB24);
	GLIB_ClearFrame(C_BLACK);

	// To Clear the uDstStAddr
	//GLIB_Init(uDstStAddr, uMid2ImgHSz, uMid2ImgVSz, RGB24);
	//GLIB_ClearFrame(C_BLACK);	//YCbCr Data 0x00000000 -> RGB24 Data 0x00008100(not Block color)
	
#if 0
	// To Make Test Image
	pBufAddr32 = (u32 *)uGlibStAddr1;
					
	for(i=0 ; i<uImgHSz*uImgVSz ; i++)
			*pBufAddr32++ = pSmile_2[i];
#endif 
	
	//GLIB_Init(uGlibStAddr1, uImgHSz, uImgVSz, RGB24);
	//GLIB_DrawPattern(uImgHSz, uImgVSz);
	ScalerT_GetImageFromBMP(uGlibStAddr1, uImgHSz, uImgVSz, 20, RGB24);
	
	// From RGB24 To YUV(360*480)
	POST_InitIp1(	uImgHSz, uImgVSz, uSrcStartX, uSrcStartY, uSrcCroppedHSz, uSrcCroppedVSz, uGlibStAddr1, RGB24,
					uMidImgHSz, uMidImgVSz, uMidStartX, uMidStartY, uMidScaledHSz, uMidScaledVSz, uSrcStAddr, eSrcDataFmt_Yuv,
					1, FALSE, ONE_SHOT, POST_DMA, POST_DMA, &oSc);
	POST_StartProcessing(&oSc);
	while (!POST_IsProcessingDone(&oSc));

	uScaleOffsetX = uMidImgHSz/2;
	uScaleOffsetY = uMidScaledVSz/2;
	// From YUV To RGB..Display default image
	POST_InitIp1(	uMidImgHSz, uMidImgVSz, uMidStartX, uMidStartY, uMidScaledHSz-uScaleOffsetX, uMidScaledVSz-uScaleOffsetY, uSrcStAddr, eSrcDataFmt_Yuv,
					uLcdHSz, uLcdVSz, uDstStartX, uDstStartY, uLcdHSz, uLcdVSz, uLcdStAddr, RGB24,
					1, FALSE, ONE_SHOT, POST_DMA, POST_DMA, &oSc);
	POST_StartProcessing(&oSc);
	while (!POST_IsProcessingDone(&oSc));

	UART_Printf("=============================================\n");
	UART_Printf("'w' : Vertical Scale-up,  'x' : Vertical Scale-down\n");
	UART_Printf("'d' : Horizontal Scale-up,  'a' : Horizontal Scale-down\n");
	UART_Printf("=============================================\n");
	UART_Printf("Press 'q' key to exit\n");	
	while((uScale=UART_Getc()) != 'q')
	{
		switch (uScale)
		{
			//Vetical Scale Up
			case 'w' :
			case 'W' :	POST_CalcurateScaleOffset(	uMidScaledHSz-uScaleOffsetX-uPreHOffset, uMidScaledVSz-uScaleOffsetY-uPreVOffset, uDstScaledHSz, uDstScaledVSz, &oSc, 
													&uHOffset, &uVOffset, &uHRatio, &uVRatio, eSrcDataFmt_Yuv);
						if(uPreVOffset == uVOffset)
						{
							if(uScaleOffsetY < (uMidImgVSz-uPreVOffset))
								 uScaleOffsetY += uPreVOffset;
						}
						else
						{	
							if(uScaleOffsetY < (uMidImgVSz-uVOffset))
								uScaleOffsetY += uVOffset - (uScaleOffsetY%(uVOffset));
							uPreVOffset = uVOffset;
						}
						break;
			//Vetical Scale Down
			case 'x' :
			case 'X' :	POST_CalcurateScaleOffset(	uMidScaledHSz-uScaleOffsetX+uPreHOffset, uMidScaledVSz-uScaleOffsetY+uPreVOffset, uDstScaledHSz, uDstScaledVSz, &oSc, 
													&uHOffset, &uVOffset, &uHRatio, &uVRatio, eSrcDataFmt_Yuv);
						if(uPreVOffset == uVOffset)
						{
							if(uScaleOffsetY >= uPreVOffset)
								 uScaleOffsetY -= uPreVOffset;
						}
						else
						{	
							if(uScaleOffsetY >= uVOffset)
								uScaleOffsetY -= uVOffset - (uScaleOffsetY%(uVOffset));
							uPreVOffset = uVOffset;
						}
						break;	
			//Horizontal Scale Up	
			case 'd' :
			case 'D' :	POST_CalcurateScaleOffset(	uMidScaledHSz-uScaleOffsetX-uPreHOffset, uMidScaledVSz-uScaleOffsetY-uPreVOffset, uDstScaledHSz, uDstScaledVSz, &oSc, 
													&uHOffset, &uVOffset, &uHRatio, &uVRatio, eSrcDataFmt_Yuv);
						if(uPreHOffset == uHOffset)
						{
							if(uScaleOffsetX < (uMidImgHSz-uPreHOffset))
								 uScaleOffsetX += uPreHOffset;
						}
						else
						{	
							if(uScaleOffsetX < (uMidImgHSz-uHOffset))
								uScaleOffsetX += uHOffset - (uScaleOffsetX%(uHOffset));
							uPreHOffset = uHOffset;
						}
						break;	
			//Horizontal Scale Down						
			case 'a' :
			case 'A' :	POST_CalcurateScaleOffset(	uMidScaledHSz-uScaleOffsetX+uPreHOffset, uMidScaledVSz-uScaleOffsetY+uPreVOffset, uDstScaledHSz, uDstScaledVSz, &oSc, 
													&uHOffset, &uVOffset, &uHRatio, &uVRatio, eSrcDataFmt_Yuv);
						if(uPreHOffset == uHOffset)
						{
							if(uScaleOffsetX >= uPreHOffset)
								 uScaleOffsetX -= uPreHOffset;
						}
						else
						{	
							if(uScaleOffsetX >= uHOffset)
								uScaleOffsetX -= uHOffset - (uScaleOffsetX%(uHOffset));
							uPreHOffset = uHOffset;
						}							
			default : 	break;
		}
		
		UART_Printf("(%d*%d) -> (%d*%d)\n",uMidScaledHSz-uScaleOffsetX, uMidScaledVSz-uScaleOffsetY, uDstScaledHSz, uDstScaledVSz);

		// Scaling from YUV to YUV
		POST_InitIp1(	uMidImgHSz, uMidImgVSz, uMidStartX, uMidStartY, uMidScaledHSz-uScaleOffsetX, uMidScaledVSz-uScaleOffsetY, uSrcStAddr, eSrcDataFmt_Yuv,
						uLcdHSz, uLcdVSz, uDstStartX, uDstStartY, uLcdHSz, uLcdVSz, uDstStAddr, eSrcDataFmt_Yuv,
						1, FALSE, ONE_SHOT, POST_DMA, POST_DMA, &oSc);
		POST_StartProcessing1(0, 0, &oSc);
		while (!POST_IsProcessingDone(&oSc));

		// CSC from YUV to RGB to display
		POST_InitIp1(	uLcdHSz, uLcdVSz, uDstStartX, uDstStartY, uLcdHSz, uLcdVSz, uDstStAddr, eSrcDataFmt_Yuv,
						uLcdHSz, uLcdVSz, uDstStartX, uDstStartY, uLcdHSz, uLcdVSz, uLcdStAddr, RGB24,
						1, FALSE, ONE_SHOT, POST_DMA, POST_DMA, &oSc);
		POST_StartProcessing1(0, 0, &oSc);
		while (!POST_IsProcessingDone(&oSc));		
	}
}


const testFuncMenu scaler_y2y_menu[] =
{
	ScalerT_SimpleDmaToDma_Y2Y,                         			"Simple DMA CSC Test From YUV To YUV",
	ScalerT_ComplexIDmaToDma_Y2Y,						"Complex DMA CSC Test From YUV To YUV",
	ScalerT_ScaleUpDown_YUV,							"Scale Up/Down YUV",
	0,                                          0
};


static void ScalerT_ConvertYUVToYUV(void)
{
	u32 i;
	s32 uSel;
			
	while(1)
	{
		UART_Printf("\n");
		for (i=0; (int)(scaler_y2y_menu[i].desc)!=0; i++)
		{
			UART_Printf("%2d: %s\n", i, scaler_y2y_menu[i].desc);
		}

		UART_Printf("\n");
		UART_Printf("Select The Function To Test: ");

		uSel = UART_GetIntNum();
		UART_Printf("\n");

		if (uSel == -1)
			break;
		if (uSel>=0 && uSel<(sizeof(scaler_y2y_menu)/8-1))
			(scaler_y2y_menu[uSel].func)();
	}
}


////////////////////////////////////////////////////////////////////////////
//////////////////                         DMA to DMA                        ///////////////////
////////////////////////////////////////////////////////////////////////////


const testFuncMenu scaler_dma2dma_menu[] =
{
	ScalerT_ConvertRGBToRGB,                        				"Convert RGB to RGB",
	ScalerT_ConvertRGBToYUVToRGB,                       			"Convert RGB to YUV to RGB",
	ScalerT_ConvertYUVToYUV,                         				"Convert YUV to YUV",
	0,                                          0
};


static void ScalerT_ConvertDMAToDMAPath(void)
{
	u32 i;
	s32 uSel;
			
	while(1)
	{
		UART_Printf("\n");
		for (i=0; (int)(scaler_dma2dma_menu[i].desc)!=0; i++)
		{
			UART_Printf("%2d: %s\n", i, scaler_dma2dma_menu[i].desc);
		}

		UART_Printf("\n");
		UART_Printf("Select The Function To Test: ");

		uSel = UART_GetIntNum();
		UART_Printf("\n");

		if (uSel == -1)
			break;
		if (uSel>=0 && uSel<(sizeof(scaler_dma2dma_menu)/8-1))
			(scaler_dma2dma_menu[uSel].func)();
	}
}



////////////////////////////////////////////////////////////////////////////
//////////////////                         DMA to FIFO                        ///////////////////
////////////////////////////////////////////////////////////////////////////


static void ScalerT_SetCscTypeDmaToFifo(void)
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
#if 0 // jihyun
	while (true)
	{
		UART_Printf("\n");
		UART_Printf("[0] Exit\n");
		UART_Printf("[1] YUV444 (Y:10, CB:10 CR: 10) Interleave\n");
		UART_Printf("[2] RGB30 (R:10 G:10 B:10)\n");
		UART_Printf(">> Enter Destination Data Format: ");
		
		nSelDstDataFmt = UART_GetIntNum();

		if (nSelDstDataFmt == 0)
			return;
		else if (nSelDstDataFmt >= 1 && nSelDstDataFmt <= 2)
		{
			ePostFifoIf = 	(nSelDstDataFmt == 1) ? YUV : RGB;
			break;
		}
		else
			UART_Printf("Invalid Data Format! Retry It!!\n");
	}
#endif	

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


// One-shot mode isn't supported in local path
// so, Only Free-Run mode is supported in local path
// In local path, scaling isn't supported because LCDC doesn't support that feature, but POST side is able to scale
static void ScalerT_SimpleDmaToFifo(void)
{
	u32 uGlibStAddr, uSrcStAddr;
	//u32 uSrcBytesPerPixel;

	UART_Printf("[ScalerT_SimpleDmaToFifo]\n");

	uSimpleTest = 1;
	ScalerT_SetCscTypeDmaToFifo();

#if 0		// Post -> FIMD WIN0 FIFO mode	
	//LCD_InitDISPCForFifoIn(IN_POST, WIN0);
	LCD_InitBase2((CLK_SRC)ECLK0_SRC, PNR_Parallel_RGB, CPU_16BIT);
	LCD_InitWinForFifoIn1(IN_POST, WIN0, LOCALIN_RGB);
	LCD_SetWinOnOff(1, WIN0);
	LCD_GetFrmSz(&uLcdHSz, &uLcdVSz, WIN0);
	LCD_Start();
#else		// Scaler -> FIMD WIN1 FIFO mode
	LCD_InitBase2((CLK_SRC)ECLK0_SRC, PNR_Parallel_RGB, CPU_16BIT);
	LCD_InitWinForFifoIn1(IN_POST, WIN1, LOCALIN_YCbCr);
	LCD_SetWinOnOff(1, WIN1);
	LCD_GetFrmSz(&uLcdHSz, &uLcdVSz, WIN1);
	LCD_Start();
#endif

	uImgHSz = uLcdHSz;
	uImgVSz = uLcdVSz;

	//TVENC_TurnOnOff(1);
	//TVENC_DisplayTVout(NTSC_M, eS_VIDEO, 320, 240);
	//INTC_Disable(NUM_TVENC);
	
	//uSrcBytesPerPixel = (eSrcDataFmt == RGB16) ? 2 : 4;
	uGlibStAddr = uLcdStAddr + uLcdHSz*uLcdVSz*4;
	uSrcStAddr = uGlibStAddr + uLcdHSz*uLcdVSz*4;
			
	GLIB_Init(uGlibStAddr, uLcdHSz, uLcdVSz, RGB24);
	GLIB_DrawPattern(uLcdHSz, uLcdVSz);	

	POST_InitIpForDmaInDmaOut(	uLcdHSz, uLcdVSz, uGlibStAddr, RGB24, 
									uImgHSz, uImgVSz, uSrcStAddr, eSrcDataFmt,
									1, false, ONE_SHOT, &oSc);
	POST_StartProcessing(&oSc);
	while (!POST_IsProcessingDone(&oSc));
	
	POST_InitIpForDmaInFifoOut(uImgHSz, uImgVSz, uSrcStAddr, eSrcDataFmt, 
								uLcdHSz, uLcdVSz, YUV, 1, &oSc);
	POST_SetFifoOutPath(SCALER_FIMD_WIN1, &oSc);
	POST_SetNextFrameStAddr(uSrcStAddr, 0, &oSc); // For Local Path
	POST_StartProcessing(&oSc);


#if 0
	while(1)
	{
		ScalerT_SelectClockDivider();

		POST_StartProcessing(&oPost);
		while (!POST_IsProcessingDone(&oPost));

		UART_Printf("Another value test : Press any key...('q' : quit)\n");
		if(UART_Getc()=='q')
			break;
	}
	
	
	UART_Printf("Press Any Key..!\n");
	UART_Getc();
#endif
			
#if (POST_IMAGE_BMP_SLIDE)	
	{
		u32 uImageNum;
		UART_Printf("Input the any key to display the other image ('q' : quit)\n");
		uImageNum = 0;

		while(UART_Getc() != 'q')
		{
			POST_StopProcessingOfFreeRun(&oSc);	
			while (!POST_IsProcessingDone(&oSc));
			ScalerT_GetImageFromBMP(uGlibStAddr, uLcdHSz, uLcdVSz, uImageNum, RGB24);
			
			POST_InitIpForDmaInDmaOut(	uLcdHSz, uLcdVSz, uGlibStAddr, RGB24, 
											uImgHSz, uImgVSz, uSrcStAddr, eSrcDataFmt,
											1, false, ONE_SHOT, &oSc);
			POST_StartProcessing(&oSc);
			while (!POST_IsProcessingDone(&oSc));
			
			POST_InitIpForDmaInFifoOut(uImgHSz, uImgVSz, uSrcStAddr, eSrcDataFmt, 
										uLcdHSz, uLcdVSz, YUV, 1, &oSc);
			POST_SetNextFrameStAddr(uSrcStAddr, 0, &oSc); // For Local Path
			POST_StartProcessing(&oSc);
			uImageNum++;
		}
	}
#endif	

	POST_StopProcessingOfFreeRun(&oSc);	
	while (!POST_IsProcessingDone(&oSc));

	POST_SetFifoOutPath(SCALER_TVOUT, &oSc);
}


static void ScalerT_SimpleFreeRunDmaToFifo_Int(void)
{
	u32 uGlibStAddr, uPostStAddr, uGlibStAddr2, uPostStAddr2;
	CSPACE eTmpDataFmt = PAL1;
	u32 uPostIsrCount, uPostFrameCount;
	
	UART_Printf("[ScalerT_SimpleFreeRunDmaToFifo_Int]\n");
	UART_Printf("Scaler -> FIMD WIN2 FIFO path\n");

	uSimpleTest = 1;
	ScalerT_SetCscTypeDmaToFifo();
	
	bScalerIsPollingOrInt = FALSE;	
	ePostRunMode = FREE_RUN;

//	eLcdLocalDataFmt = (eDstDataFmt_FifoOut == RGB30) ? LOCAL_IN_RGB : LOCAL_IN_YCBYCR;

	// RGB24 bpp as LCD input data format is only supported in Local path
	//LCD_InitLDI(MAIN);
	LCD_InitBase2((CLK_SRC)ECLK0_SRC, PNR_Parallel_RGB, CPU_16BIT);
	LCD_InitWinForFifoIn1(IN_POST, WIN2, LOCALIN_RGB);
	LCD_GetFrmSz(&uLcdHSz, &uLcdVSz, WIN2);
	LCD_SetWinOnOff(1, WIN2);
	LCD_Start();

	uImgHSz = uLcdHSz;
	uImgVSz = uLcdVSz;

	// For DisplayCurParam() func.
	uSrcCroppedHSz = uLcdHSz;
	uSrcCroppedVSz = uLcdVSz;
	uSrcStartX = 0;
	uSrcStartY = 0;

	uDstScaledHSz = uLcdHSz; 
	uDstScaledVSz = uLcdVSz;
	uDstStartX = 0;
	uDstStartY = 0;

	// Data format of src.: YCbYCr/CbYCrY422 intlv or YC420. so, src. img. have a 2bytes/pixel
	// Data Format of Dest. : YUV444 or RGB24. so, dest img. have a 4bytes/pixel
	uGlibStAddr = uLcdStAddr + uLcdHSz*uLcdVSz*4;
	uGlibStAddr2 = uGlibStAddr + uImgHSz*uImgVSz*4;

	// To generate image by using Glib, allocate memory for converting from RGB24 to YUV
	if (	eSrcDataFmt == YCBYCR || eSrcDataFmt == CBYCRY ||
		eSrcDataFmt == YCRYCB || eSrcDataFmt == CRYCBY ||		
		eSrcDataFmt == YC420)
	{
		uPostStAddr = uGlibStAddr2 + uImgHSz*uImgVSz*4;		
		uPostStAddr2 = uPostStAddr + uImgHSz*uImgVSz*4;		
		
		eTmpDataFmt = RGB24;
	}
	else
	{
		eTmpDataFmt = eSrcDataFmt;
	}

	GLIB_Init(uGlibStAddr, uImgHSz, uImgVSz, eTmpDataFmt);
	GLIB_DrawPattern(uImgHSz, uImgVSz);

	GLIB_Init(uGlibStAddr2, uImgHSz, uImgVSz, eTmpDataFmt);
	GLIB_DrawPattern2(uLcdHSz, uLcdVSz);
	
	if (	eSrcDataFmt == YCBYCR || eSrcDataFmt == CBYCRY ||
		eSrcDataFmt == YCRYCB || eSrcDataFmt == CRYCBY ||		
		eSrcDataFmt == YC420)
	{
		// Color space conversion from RGB24 to YUV for generated image by using GLIB
		POST_InitIpForDmaInDmaOut(	uImgHSz, uImgVSz, uGlibStAddr, eTmpDataFmt, 
										uImgHSz, uImgVSz, uPostStAddr, eSrcDataFmt,
										1, FALSE, ONE_SHOT, &oSc);
		POST_StartProcessing(&oSc);
		while (!POST_IsProcessingDone(&oSc));

		POST_InitIpForDmaInDmaOut(	uImgHSz, uImgVSz, uGlibStAddr2, eTmpDataFmt, 
										uImgHSz, uImgVSz, uPostStAddr2, eSrcDataFmt,
										1, FALSE, ONE_SHOT, &oSc);
		POST_StartProcessing(&oSc);
		while (!POST_IsProcessingDone(&oSc));

		uNxtAddr1 = uPostStAddr;
		uNxtAddr2 = uPostStAddr2;
	}
	else
	{
		uNxtAddr1 = uGlibStAddr;
		uNxtAddr2 = uGlibStAddr2;
	}

	//Interrupt Setting
	uPostIsrCount = 0;
	uPostFrameCount = 0;
	uScalerIntOccured = FALSE;

	INTC_Enable(NUM_SCALER);
	INTC_SetVectAddr(NUM_SCALER, ScalerISRDma);
	POST_EnableInterrupt(POST_LEVEL_INT, &oSc);
	
	// Init For Local Path
	POST_InitIpForDmaInFifoOut(	uImgHSz, uImgVSz, uNxtAddr1, eSrcDataFmt, 
								uLcdHSz, uLcdVSz, RGB, 1, &oSc);
	POST_SetNextFrameStAddr(uNxtAddr2, 0, &oSc); // For Local Path
	POST_SetFifoOutPath(SCALER_FIMD_WIN2, &oSc);
	POST_StartProcessing(&oSc);

	while(uPostFrameCount < uMaxFrameCnt)
	{
		if(uScalerIntOccured == TRUE)
		{
			uPostIsrCount++;
			if(uPostIsrCount == 30)
			{
				uPostFrameCount++;
				
			 	if (uPostFrameCount % 2)
					POST_SetNextFrameStAddr(uNxtAddr1, 0, &oSc);
				else 
					POST_SetNextFrameStAddr(uNxtAddr2, 0, &oSc);

				if (uPostFrameCount == uMaxFrameCnt)
				{
					POST_StopProcessingOfFreeRun(&oSc);	
					while (!POST_IsProcessingDone(&oSc));	
					uScalerIntOccured = FALSE;
					break;
				}
				uPostIsrCount = 0;
			}
			uScalerIntOccured = FALSE;
		}
	}

	POST_DisableInterrupt(&oSc);
	POST_ClearPending(&oSc);	
	//INTC_Mask(INT_POST);	
	INTC_Disable(NUM_SCALER);
	POST_SetFifoOutPath(SCALER_TVOUT, &oSc);
}



static void ScalerT_ComplexFreeRunDmaToFifo_Int(void)
{
	u32 uGlibStAddr, uPostStAddr, uGlibStAddr2, uPostStAddr2;
//	CSPACE eTmpDataFmt = PAL1;
	u32 uPostIsrCount, uPostFrameCount;
	
	UART_Printf("[ScalerT_ComplexFreeRunDmaToFifo_Int]\n");

	uSimpleTest = 0;
	ScalerT_SetCscTypeDmaToFifo();
	
	bScalerIsPollingOrInt = FALSE;	
	ePostRunMode = FREE_RUN;
	eDstDataFmt = RGB24;

//	eLcdLocalDataFmt = (eDstDataFmt_FifoOut == RGB30) ? LOCAL_IN_RGB : LOCAL_IN_YCBYCR;

	// RGB24 bpp as LCD input data format is only supported in Local path
	LCD_InitBase2((CLK_SRC)ECLK0_SRC, PNR_Parallel_RGB, CPU_16BIT);
	LCD_InitWinForFifoIn1(IN_POST, WIN1, LOCALIN_RGB);
	LCD_GetFrmSz(&uLcdHSz, &uLcdVSz, WIN1);
	LCD_SetWinOnOff(1, WIN1);
	LCD_Start();

	//uBytesPerPixel = (eDstDataFmt == RGB16) ? 2: 4;
	
	// Data format of src.: YCbYCr/CbYCrY422 intlv or YC420. so, src. img. have a 2bytes/pixel
	// Data Format of Dest. : YUV444 or RGB24. so, dest img. have a 4bytes/pixel
	uGlibStAddr = uLcdStAddr + uLcdHSz*uLcdVSz*4;
	uGlibStAddr2 = uGlibStAddr + uImgHSz*uImgVSz*4;

	if( eSrcDataFmt == RGB24 )
	{
		uPostStAddr = uGlibStAddr2 + uImgHSz*uImgVSz*4;		
		uPostStAddr2 = uPostStAddr + uMidImgHSz*uMidImgVSz*4;		//RGB24	
		//eTmpDataFmt = RGB24;
	}	
	else
	{
		uPostStAddr = uGlibStAddr2 + uImgHSz*uImgVSz*4;		
		uPostStAddr2 = uPostStAddr + uMidImgHSz*uMidImgVSz*2;		//RGB16/YCbCr422/YCbCr420
		//eTmpDataFmt = RGB24;
	}

	GLIB_Init(uGlibStAddr, uImgHSz, uImgVSz, RGB24);
	GLIB_DrawPattern(uImgHSz, uImgVSz);

	GLIB_Init(uGlibStAddr2, uImgHSz, uImgVSz, RGB24);
	GLIB_DrawPattern2(uImgHSz, uImgVSz);

	// Color space conversion from RGB24 to YUV for generated image by using GLIB
	POST_InitIp1(	uImgHSz, uImgVSz, 0, 0, uImgHSz, uImgVSz, uGlibStAddr, RGB24,
					uMidImgHSz, uMidImgVSz, 0, 0, uMidImgHSz, uMidImgVSz, uPostStAddr, eSrcDataFmt,
					1, FALSE, ONE_SHOT, POST_DMA, POST_DMA, &oSc);
	//POST_InitIpForDmaInDmaOut(	uImgHSz, uImgVSz, uGlibStAddr, RGB24, 
	//								uImgHSz, uImgVSz, uPostStAddr, eSrcDataFmt,
	//								1, FALSE, ONE_SHOT, &oSc);
	POST_StartProcessing(&oSc);
	while (!POST_IsProcessingDone(&oSc));
	
	POST_InitIp1(	uImgHSz, uImgVSz, 0, 0, uImgHSz, uImgVSz, uGlibStAddr2, RGB24,
					uMidImgHSz, uMidImgVSz, 0, 0, uMidImgHSz, uMidImgVSz, uPostStAddr2, eSrcDataFmt,
					1, FALSE, ONE_SHOT, POST_DMA, POST_DMA, &oSc);
	//POST_InitIpForDmaInDmaOut(	uImgHSz, uImgVSz, uGlibStAddr2, RGB24, 
	//								uImgHSz, uImgVSz, uPostStAddr2, eSrcDataFmt,
	//								1, FALSE, ONE_SHOT, &oSc);
	POST_StartProcessing(&oSc);
	while (!POST_IsProcessingDone(&oSc));

	uNxtAddr1 = uPostStAddr;
	uNxtAddr2 = uPostStAddr2;

	//Interrupt Setting
	uPostIsrCount = 0;
	uPostFrameCount = 0;
	uScalerIntOccured = FALSE;
	
	INTC_Enable(NUM_SCALER);
	INTC_SetVectAddr(NUM_SCALER, ScalerISRDma);
	POST_EnableInterrupt(POST_LEVEL_INT, &oSc);

	// Init For Local Path
	//POST_InitIp1(	uMidImgHSz, uMidImgVSz, uDstStartX, uDstStartY, uDstScaledHSz, uDstScaledVSz, uNxtAddr1, eSrcDataFmt,
	//				uLcdHSz, uLcdVSz, 0, 0, uLcdHSz, uLcdVSz, 0, RGB24,
	//				1, FALSE, FREE_RUN, POST_DMA, POST_FIFO, &oSc);	
	POST_InitIp1(	uMidImgHSz, uMidImgVSz, uMidStartX, uMidStartY, uMidScaledHSz, uMidScaledVSz, uNxtAddr1, eSrcDataFmt,
					uLcdHSz, uLcdVSz, 0, 0, uLcdHSz, uLcdVSz, 0, RGB24,
					1, FALSE, FREE_RUN, POST_DMA, POST_FIFO, &oSc);		
	//POST_InitIpForDmaInFifoOut(	uLcdHSz, uLcdVSz, uNxtAddr1, eSrcDataFmt, 
	//							uLcdHSz, uLcdVSz, RGB, 1, &oSc);
	POST_SetNextFrameStAddr(uNxtAddr2, 0, &oSc); // For Local Path
	POST_SetFifoOutPath(SCALER_FIMD_WIN1, &oSc);
	POST_StartProcessing(&oSc);

	while(uPostFrameCount < uMaxFrameCnt)
	{
		if(uScalerIntOccured == TRUE)
		{
			uPostIsrCount++;
			if(uPostIsrCount == 30)
			{
				uPostFrameCount++;
				
			 	if (uPostFrameCount % 2)
					POST_SetNextFrameStAddr(uNxtAddr1, 0, &oSc);
				else 
					POST_SetNextFrameStAddr(uNxtAddr2, 0, &oSc);

				if (uPostFrameCount == uMaxFrameCnt)
				{
					POST_StopProcessingOfFreeRun(&oSc);	
					while (!POST_IsProcessingDone(&oSc));	
					uScalerIntOccured = FALSE;
					break;
				}
				uPostIsrCount = 0;
			}
			uScalerIntOccured = FALSE;
		}
	}

	POST_DisableInterrupt(&oSc);
	POST_ClearPending(&oSc);	
	//INTC_Mask(INT_POST);	
	INTC_Disable(NUM_SCALER);
	POST_SetFifoOutPath(SCALER_TVOUT, &oSc);
}



const testFuncMenu scaler_dma2fifo_menu[] =
{
	ScalerT_SimpleDmaToFifo,                                  				"Simple FreeRun Local CSC Test ",		
	ScalerT_SimpleFreeRunDmaToFifo_Int,                 			"Simple FreeRun Local CSC Test By Using Interrupt",
	ScalerT_ComplexFreeRunDmaToFifo_Int,                 				"Complex FreeRun Local CSC Test By Using Interrupt",
	0,                                          0
};


static void ScalerT_ConvertDMAToFIFOPath(void)
{
	u32 i;
	s32 uSel;
			
	while(1)
	{
		UART_Printf("\n");
		for (i=0; (int)(scaler_dma2fifo_menu[i].desc)!=0; i++)
		{
			UART_Printf("%2d: %s\n", i, scaler_dma2fifo_menu[i].desc);
		}

		UART_Printf("\n");
		UART_Printf("Select The Function To Test: ");

		uSel = UART_GetIntNum();
		UART_Printf("\n");

		if (uSel == -1)
			break;
		if (uSel>=0 && uSel<(sizeof(scaler_dma2fifo_menu)/8-1))
			(scaler_dma2fifo_menu[uSel].func)();
	}
}


////////////////////////////////////////////////////////////////////////////
//////////////////                         FIFO to DMA                        ///////////////////
////////////////////////////////////////////////////////////////////////////


static void ScalerT_FifoToDma(void)
{
	u32 uInImg0=0, uInImg1=0;
	u32 uOutImg=0;

	UART_Printf("[ScalerT_FifoToDma]\n");
	
	eDstDataFmt = RGB24;

	uInImg0 = CODEC_MEM_ST;
	
	//FIMD4.0 Init
	LCD_InitDISPC(eDstDataFmt, uInImg0, WIN0, false);
	LCD_SetWinOnOff(1, WIN0);
	LCD_GetFrmSz(&uLcdHSz, &uLcdVSz, WIN0);
	
	uImgHSz = uLcdHSz;
	uImgVSz = uLcdVSz;
	uDstScaledHSz = uLcdHSz;
	uDstScaledVSz = uLcdVSz;

	uInImg1 = uInImg0 + uImgHSz*uImgVSz*4;
	uOutImg = uInImg1 + uImgHSz*uImgVSz*4;

	// 1. Image 0 Display
	GLIB_Init(uInImg0, uImgHSz, uImgVSz, eDstDataFmt);
	GLIB_DrawPattern(uImgHSz, uImgVSz);
	GLIB_SetFontColor(C_BLACK, C_WHITE, true);
	GLIB_Printf(10, 10, "WIN 0");

	UART_Printf("Enter Any Key To Display Image 1\n");
	UART_Getc();
	LCD_Start();		

	// 2. Image 1 Display
	GLIB_Init(uInImg1, uImgHSz, uImgVSz, eDstDataFmt);
	GLIB_DrawPattern2(uImgHSz, uImgVSz);
	GLIB_SetFontColor(C_BLACK, C_WHITE, true);
	GLIB_Printf(10, 50, "WIN 1");

	UART_Printf("Enter Any Key To Display Image 2\n");
	UART_Getc();

	LCD_InitDISPC(eDstDataFmt, uInImg1, WIN0, false);
	LCD_Start();
	LCD_SetWinOnOff(1, WIN0);

	// 3. Blending image between image0 & image1
	UART_Printf("Enter Any Key To Display Blended Image Between Image0 And Image1\n");
	UART_Getc();
	
	POST_InitIpForFifoInDmaOut(uImgHSz, uImgVSz, uDstScaledHSz, uDstScaledVSz, uOutImg, eDstDataFmt,
								false, ONE_SHOT, &oSc);	// FIFO In, DMA Out
	POST_StartProcessing(&oSc);

	LCD_InitDISPC(eDstDataFmt, uInImg0, WIN0, false);
	LCD_InitDISPC(eDstDataFmt, uInImg1, WIN1, false);
	LCD_SetWinOnOff(1, WIN0);
	LCD_SetWinOnOff(1, WIN1);
//	LCD_SetClkDir(DIRECT_DIR);
	LCD_SetOutputPath(LCD_TVRGB);
	LCD_SetBlendingType(PER_PLANE, ALPHA0_PLANE, WIN1);
	LCD_SetAlpha(7, 7, 7, 0x0, 0x0, 0x0, WIN1);
	LCD_Start();

	while (!POST_IsProcessingDone(&oSc));
	
	UART_Getc();
	LCD_InitDISPC(eDstDataFmt, uOutImg, WIN0, false);
	LCD_SetWinOnOff(1, WIN0);
	LCD_SetWinOnOff(0, WIN1);
	LCD_Start();
}


const testFuncMenu scaler_fifo2dma_menu[] =
{
	ScalerT_FifoToDma,                                  				"Test FIFO TO DMA (FIFO Source: LCDC)",
	0,                                          0
};


static void ScalerT_ConvertFIFOToDMAPath(void)
{
	u32 i; 
	s32 uSel;
			
	while(1)
	{
		UART_Printf("\n");
		for (i=0; (int)(scaler_fifo2dma_menu[i].desc)!=0; i++)
		{
			UART_Printf("%2d: %s\n", i, scaler_fifo2dma_menu[i].desc);
		}

		UART_Printf("\n");
		UART_Printf("Select The Function To Test: ");

		uSel = UART_GetIntNum();
		UART_Printf("\n");

		if (uSel == -1)
			break;
		if (uSel>=0 && uSel<(sizeof(scaler_fifo2dma_menu)/8-1))
			(scaler_fifo2dma_menu[uSel].func)();
	}
}


////////////////////////////////////////////////////////////////////////////
//////////////////                    Post Clock Selection                   ///////////////////
////////////////////////////////////////////////////////////////////////////

//////////
// Function Name : ScalerT_SelectClockSource
// Function Description : Set POST Clock source
// Input : 	None
// Output : 	None

void ScalerT_SelectClockSource(void)
{
	u32 uPostClockSel, uPLLExtClock; 
	POST_CLK_SRC uPLLExtSrc;

	UART_Printf("\n");
	UART_Printf("[ScalerT_SelectClockSource]\n");
	UART_Printf("0. HCLK(D)	1. PLL Ext Clock	2. 27MHz Ext Clock\n");
	UART_Printf("Select the source clock : ");
	
	uPostClockSel = (u32)UART_GetIntNum();
		
	if(uPostClockSel == 1)
	{
		UART_Printf("\n");
		UART_Printf("0. MOUTepll(D)	1. DOUTmpll	2. FINepll\n");
		UART_Printf("Select the PLL Ext Clock source clock : ");

		uPLLExtClock = (u32)UART_GetIntNum();

		switch(uPLLExtClock)
		{
			case 0 :	uPLLExtSrc = PLL_EXT_MOUTEPLL;
					break;
			case 1 :	uPLLExtSrc = PLL_EXT_DOUTMPLL;
					break;
			case 2 :	uPLLExtSrc = PLL_EXT_FINEPLL;
					break;
			default :  uPLLExtSrc = PLL_EXT_MOUTEPLL;
					  break;
		}
	}

	switch(uPostClockSel)
	{
		case 0 :	POST_SetClkSrc(HCLK, &oSc);
				break;
		case 1 :	POST_SetClkSrc(uPLLExtSrc, &oSc);
				break;
		case 2 :	POST_SetClkSrc(EXT_27MHZ, &oSc);
				break;
		default :	POST_SetClkSrc(HCLK, &oSc);
				break;
	}
}


////////////////////////////////////////////////////////////////////////////
//////////////////                    POST Convert Image                   ///////////////////
////////////////////////////////////////////////////////////////////////////
#if 0
static void ScalerT_ConvertImage(void)
{
	u32 uHSz, uVSz;
	CSPACE eBpp = RGB24;
	u32 uFbStAddr = CODEC_MEM_ST;
	u32 uImgAddr;

	UART_Printf("[ScalerT_ConvertImage]\n");

	uHSz = 240, uVSz = 320;
	
	uImgAddr = uFbStAddr + uHSz*uVSz*4;

	//LCD_InitLDI(MAIN);

#ifdef LCD_INIT_TEST	
	LCD_InitBase();
	LCD_Start();
	LCD_InitWin(eBpp, uImgHSz, uImgVSz, 0, 0, uImgHSz, uImgVSz, 0, 0, uFbStAddr, WIN0, false);
#else
	LCD_InitDISPC(eBpp, uFbStAddr, WIN0, false);
	LCD_Start();
#endif
	LCD_SetWinOnOff(1, WIN0);

	// Load Image.
	LoadFromFile(IMG_YC420, uImgAddr);

	// Post.
	POST_InitIpForDmaInDmaOut(uHSz, uVSz, uImgAddr, YC420, uHSz, uVSz, uFbStAddr, RGB24,
		1, false, ONE_SHOT, &oSc);
	POST_StartProcessing(&oSc);

}
#endif

////////////////////////////////////////////////////////////////////////////
//////////////////                       POST Async Clock                   ///////////////////
////////////////////////////////////////////////////////////////////////////
static void ScalerT_ClockTest(	CLK_SRC eLcdClkSrc, u32 uLcdClkDiv, POST_CLK_SRC ePostClkSrc,
								u32 *uGlibStAddr0, u32 *uGlibStAddr1, u32 uNumOfImage)
{
	u32 uNumWin, uImageNum;
	LCD_WINDOW eLcdWin;
	SCALER_PATH eScalerPath;
		
	UART_Printf("\n");
	if(eLcdClkSrc == SRC_HCLK)
		UART_Printf("1. Current LCD Clock : HCLK\n");
	else if(eLcdClkSrc ==  SRC_MOUT_EPLL)
			UART_Printf("2. Current LCD Clock : PLL_EXT_MOUTEPLL\n");
	else if(eLcdClkSrc ==  SRC_FIN_EPLL)
			UART_Printf("3. Current LCD Clock : PLL_EXT_FINEPLL\n");
	else if(eLcdClkSrc ==  SRC_DOUT_MPLL)
			UART_Printf("4. Current LCD Clock : PLL_EXT_DOUTMPLL\n");	
	else //(eLcdClkSrc == ECLK1_SRC)
		UART_Printf("5. Current LCD Clock : EXT_27MHz\n");
	
	if(ePostClkSrc == HCLK)
		UART_Printf("1. Current Scaler Clock : HCLK\n");
	else if(ePostClkSrc == PLL_EXT_MOUTEPLL)
		UART_Printf("2. Current Scaler Clock : PLL_EXT_MOUTEPLL\n");
	else if(ePostClkSrc == PLL_EXT_FINEPLL)
		UART_Printf("3. Current Scaler Clock : PLL_EXT_FINEPLL\n");
	else	 if(ePostClkSrc == PLL_EXT_DOUTMPLL)
		UART_Printf("4. Current Scaler Clock : PLL_EXT_DOUTMPLL\n");
	else	 //(ePostClkSrc == EXT_27MHZ)
		UART_Printf("5. Current Scaler Clock : EXT_27MHz\n");

	eLcdWin = WIN1;
	eScalerPath = SCALER_FIMD_WIN1;
	
	for(uNumWin=0 ; uNumWin<2 ; uNumWin++)
	{
		if(eLcdWin == WIN1)
			UART_Printf("LCD Window : WIN1\n");
		else
			UART_Printf("LCD Window : WIN2\n");

		LCD_InitBase2((CLK_SRC)ECLK0_SRC, PNR_Parallel_RGB, CPU_16BIT);
		LCD_InitWinForFifoIn1(IN_POST, eLcdWin, LOCALIN_RGB);

		if(eLcdClkSrc == SRC_MOUT_EPLL)
			{
				SYSC_SetPLL(eEPLL, 32, 1, 2, 0);
				LCD_SelClkSrcFromSYSCON(MOUT_EPLL);
				SYSC_ClkSrc(eEPLL_FOUT);
			}
		else if(eLcdClkSrc == SRC_FIN_EPLL)
				LCD_SelClkSrcFromSYSCON(FIN_EPLL);
		else if(eLcdClkSrc == SRC_DOUT_MPLL)
				LCD_SelClkSrcFromSYSCON(DOUT_MPLL);

		LCD_SetClkVal(uLcdClkDiv);
		LCD_SetClkSrc(eLcdClkSrc);	
		
		LCD_SetWinOnOff(1, eLcdWin);
		LCD_GetFrmSz(&uLcdHSz, &uLcdVSz, eLcdWin);

		LCD_Start();

		POST_InitIpForDmaInFifoOut(uLcdHSz, uLcdVSz, (u32)uGlibStAddr0, RGB24, uLcdHSz, uLcdVSz, RGB, 1, &oSc);
		POST_SetNextFrameStAddr((u32)uGlibStAddr0, 0, &oSc); // For Local Path
		POST_SetClkSrc(ePostClkSrc, &oSc);
		POST_SetFifoOutPath(eScalerPath, &oSc);
		POST_StartProcessing(&oSc);
		Delay(10000);

		uImageNum = 0;
#if 0	// Post On/Off repeat
		while(uImageNum<uNumOfImage)
		{	
			POST_StopProcessingOfFreeRun(&oSc);	
			while (!POST_IsProcessingDone(&oSc));	
			
			//DelayfrTimer(milli, 1);
			while(POST_CheckAutoLoadEnable(&oSc));
			while (!POST_IsProcessingDone(&oSc));
			
			POST_InitIpForDmaInFifoOut(uLcdHSz, uLcdVSz, (u32)uGlibStAddr1[uImageNum], RGB24, 
										uLcdHSz, uLcdVSz, RGB, 1, &oSc);
			POST_SetNextFrameStAddr((u32)uGlibStAddr1[uImageNum], 0, &oSc); // For Local Path
			POST_StartProcessing(&oSc);
			DelayfrTimer(milli, 500);
			uImageNum++;
		}
#else		// only update the Post Next Address
		while(uImageNum<uNumOfImage)
		{	
			POST_SetNextFrameStAddr((u32)uGlibStAddr1[uImageNum], 0, &oSc); // For Local Path
			Delay(10000);
			uImageNum++;
		}
#endif	
		POST_StopProcessingOfFreeRun(&oSc);	
		while (!POST_IsProcessingDone(&oSc));
		POST_SetFifoOutPath(SCALER_TVOUT, &oSc);

		eLcdWin = WIN2;
		eScalerPath = SCALER_FIMD_WIN2;		
	}
	
}


static void ScalerT_Scaler_FIMD_SyncAsyncClk(void)
{
	u32 *uGlibStAddr0;
	u32 i, j, uNumOfTestImage, uCount;
	u32 *uGlibStAddr1[30];

#if (LCD_MODULE_TYPE ==LTE480WV_RGB)
	u32 eLcdClk[5][2] = {	{SRC_HCLK, 4},
						{SRC_MOUT_EPLL, 3},
						{SRC_FIN_EPLL, 0},
						{SRC_DOUT_MPLL, 4},
						{SRC_27M, 0}};

#elif (LCD_MODULE_TYPE== LTV350QV_RGB)
	u32 eLcdClk[5][2] = { 	{SRC_HCLK, 21},
						{SRC_MOUT_EPLL, 21},
						{SRC_FIN_EPLL, 1},
						{SRC_DOUT_MPLL, 21},
						{SRC_27M, 5}};
#endif	

	POST_CLK_SRC ePostClk[5] = { HCLK, PLL_EXT_MOUTEPLL, PLL_EXT_FINEPLL, PLL_EXT_DOUTMPLL, EXT_27MHZ};

	uNumOfTestImage = sizeof(Scaler_Image_Entertain)/4;
	uCount = 0;
	
	LCD_InitDISPCForFifoIn(IN_POST, WIN0);
	LCD_SetClkSrc((CLK_SRC)ECLK1_SRC);
	LCD_SetClkVal(5);	
	LCD_SetWinOnOff(1, WIN0);
	LCD_GetFrmSz(&uLcdHSz, &uLcdVSz, WIN0);

	uGlibStAddr0 = (u32 *)malloc(uLcdHSz*uLcdVSz*4);

	UART_Printf("Downloading the Image....\n");
	for(i=0 ; i<uNumOfTestImage ; i++)
	{
		uGlibStAddr1[i] = (u32 *)malloc(uLcdHSz*uLcdVSz*4);
		ScalerT_GetImageFromBMP((u32)uGlibStAddr1[i], uLcdHSz, uLcdVSz, i, RGB24);
	}
	
	GLIB_Init((u32)uGlibStAddr0, uLcdHSz, uLcdVSz, RGB24);
	GLIB_DrawPattern(uLcdHSz, uLcdVSz);	

#if 1
	while(!UART_GetKey())
	{
		for(i=0 ; i<5 ; i++)
		{
			for(j=0 ; j<5 ; j++)
			{
				ScalerT_ClockTest(	(CLK_SRC)eLcdClk[i][0],  eLcdClk[i][1],  ePostClk[j], 
									(u32 *)uGlibStAddr0, (u32 *)uGlibStAddr1, uNumOfTestImage);
			}
		}
		uCount++;
		UART_Printf("\n");
		UART_Printf("Count : %d\n", uCount);
	}
#else
	{
		u32 uLcdClock, uPostClock;
	
		while(UART_Getc() != 'q')
		{
			UART_Printf(">>Select The LCD Clock[0~4]");
			uLcdClock = UART_GetIntNum();
			UART_Printf(">>Select The POST Clock[0~4]");
			uPostClock = UART_GetIntNum();
			
			ScalerT_ClockTest(	(CLK_SRC)eLcdClk[uLcdClock][0],  eLcdClk[uLcdClock][1], eLcdClk[uLcdClock][2], 
									ePostClk[uPostClock], (u32 *)uGlibStAddr0, (u32 *)uGlibStAddr1, uNumOfTestImage);
		}
	}
#endif

	free(uGlibStAddr0);
	for(i=0 ; i<uNumOfTestImage ; i++)
	{
		free(uGlibStAddr1[i]);
	}
	
}


extern void __irq Isr_FifoUnderrun(void);
void ScalerT_Scaler_TVEnc_SourceClk_Test(void) // post controller initialize
{
	u32 uSrcWidth, uSrcHeight, uWidth, uHeight;
	u32 i, j, uNumOfTestImage, uCount, uImageNum;
	u32 *uGlibStAddr1[30];
	u32 *uGlibStAddr0;
	POST_CLK_SRC ePostClk[5] = { HCLK, PLL_EXT_MOUTEPLL, PLL_EXT_FINEPLL, PLL_EXT_DOUTMPLL, EXT_27MHZ};

	uSrcWidth = 320;
	uSrcHeight = 240;
	
	uWidth = 720;
	uHeight = 480;

	uNumOfTestImage = sizeof(Scaler_Image_Entertain)/4;
	uCount = 0;
	
	uGlibStAddr0 = (u32 *)malloc(uSrcWidth*uSrcHeight*4);
	
	UART_Printf("Downloading the Image....\n");
	for(i=0 ; i<uNumOfTestImage ; i++)
	{
		uGlibStAddr1[i] = (u32 *)malloc(uSrcWidth*uSrcHeight*4);
		ScalerT_GetImageFromBMP((u32)uGlibStAddr1[i], uSrcWidth, uSrcHeight, i, RGB24);
	}
	
	GLIB_Init((u32)uGlibStAddr0, uSrcWidth, uSrcHeight, RGB24);
	GLIB_DrawPattern(uSrcWidth, uSrcHeight);	

	INTC_SetVectAddr(NUM_TVENC, Isr_FifoUnderrun);

	TVENC_SetImageSize(uWidth*2, uHeight);
		
	while(!UART_GetKey())
	{
		for(j=0 ; j<5 ; j++)
		{
			POST_SetClkSrc(ePostClk[j], &oSc);

			if(ePostClk[j] == HCLK)
				UART_Printf("1. Current POST Clock : HCLK\n");
			else if(ePostClk[j] == PLL_EXT_MOUTEPLL)
				UART_Printf("2. Current POST Clock : PLL_EXT_MOUTEPLL\n");
			else if(ePostClk[j] == PLL_EXT_FINEPLL)
				UART_Printf("3. Current POST Clock : PLL_EXT_FINEPLL  => No operation because of performance(greater than 25MHz)\n");
			else	 if(ePostClk[j] == PLL_EXT_DOUTMPLL)
				UART_Printf("4. Current POST Clock : PLL_EXT_DOUTMPLL\n");
			else	 //(ePostClk[j] == EXT_27MHZ)
				UART_Printf("5. Current POST Clock : EXT_27MHZ\n");

			INTC_Enable(NUM_TVENC);
			TVENC_TurnOnOff(1);
	
			if(ePostClk[j] != PLL_EXT_FINEPLL)
			{
				POST_InitIp1(	uSrcWidth, uSrcHeight, 0, 0, uSrcWidth, uSrcHeight, (u32)uGlibStAddr0, RGB24,
								uWidth*2, uHeight, 0, 0, uWidth*2, uHeight, 0, YCBYCR,
								1, FALSE, FREE_RUN, POST_DMA, POST_FIFO, &oSc);
				POST_SetInterlaceModeOnOff(1, &oSc);
				POST_SetNextFrameStAddr((u32)uGlibStAddr0, 0, &oSc);
				POST_StartProcessing(&oSc);

				Delay(10000);

				uImageNum = 0;			
				while(uImageNum<uNumOfTestImage)
				{	
					POST_SetNextFrameStAddr((u32)uGlibStAddr1[uImageNum], 0, &oSc); // For Local Path
					Delay(10000);
					uImageNum++;
				}

				POST_SetInterlaceModeOnOff(0, &oSc);
				POST_StopProcessingOfFreeRun(&oSc);
				while (!POST_IsProcessingDone(&oSc));

				INTC_Disable(NUM_TVENC);
				TVENC_TurnOnOff(0);
			}
		
		}
		uCount++;
		UART_Printf("\n");
		UART_Printf("Count : %d\n", uCount);
	}

	INTC_Disable(NUM_TVENC);
	TVENC_TurnOnOff(0);

	free(uGlibStAddr0);
	for(i=0 ; i<uNumOfTestImage ; i++)
	{
		free(uGlibStAddr1[i]);
	}	
}



static void ScalerT_Post_FIMD_Scaler_TVEnc(	CLK_SRC eLcdClkSrc, u32 uLcdClkDiv, POST_CLK_SRC ePostClkSrc,
												u32 *uGlibStAddr0, u32 *uGlibStAddr1, u32 uNumOfImage)
{
	u32 uImageNum;

	UART_Printf("\n");
	if(eLcdClkSrc == SRC_HCLK)
		UART_Printf("1. Current LCD Clock : HCLK\n");
	else if(eLcdClkSrc ==  SRC_MOUT_EPLL)
			UART_Printf("2. Current LCD Clock : PLL_EXT_MOUTEPLL\n");
	else if(eLcdClkSrc ==  SRC_FIN_EPLL)
			UART_Printf("3. Current LCD Clock : PLL_EXT_FINEPLL\n");
	else if(eLcdClkSrc ==  SRC_DOUT_MPLL)
			UART_Printf("4. Current LCD Clock : PLL_EXT_DOUTMPLL\n");	
	else //(eLcdClkSrc == ECLK1_SRC)
		UART_Printf("5. Current LCD Clock : EXT_27MHz\n");
	
	if(ePostClkSrc == HCLK)
		UART_Printf("1. Current Scaler Clock : HCLK\n");
	else if(ePostClkSrc == PLL_EXT_MOUTEPLL)
		UART_Printf("2. Current Scaler Clock : PLL_EXT_MOUTEPLL\n");
	else if(ePostClkSrc == PLL_EXT_FINEPLL)
	{
		UART_Printf("3. Current Scaler Clock : PLL_EXT_FINEPLL => No operation because of performance(greater than 25MHz)\n");
		return;
	}
	else	 if(ePostClkSrc == PLL_EXT_DOUTMPLL)
		UART_Printf("4. Current Scaler Clock : PLL_EXT_DOUTMPLL\n");
	else	 //(ePostClkSrc == EXT_27MHZ)
		UART_Printf("5. Current Scaler Clock : EXT_27MHz\n");

	LCD_InitDISPCForFifoIn(IN_POST, WIN0);

	if(eLcdClkSrc == SRC_MOUT_EPLL)
	{
			SYSC_SetPLL(eEPLL, 32, 1, 2, 0);
			LCD_SelClkSrcFromSYSCON(MOUT_EPLL);
			SYSC_ClkSrc(eEPLL_FOUT);
	}	
	else if(eLcdClkSrc == SRC_FIN_EPLL)
			LCD_SelClkSrcFromSYSCON(FIN_EPLL);
	else if(eLcdClkSrc == SRC_DOUT_MPLL)
			LCD_SelClkSrcFromSYSCON(DOUT_MPLL);
	
	LCD_SetWinOnOff(1, WIN0);
	LCD_GetFrmSz(&uLcdHSz, &uLcdVSz, WIN0);
	LCD_SetOutputPath(LCD_TVRGB);
	LCD_SetClkVal(uLcdClkDiv);
	LCD_SetClkSrc(eLcdClkSrc);	
	LCD_Start();

	TVENC_DisplayTVout(NTSC_M, eS_VIDEO, 720, 480);
	
	POST_InitIpForFifoInFifoOut(uLcdHSz, uLcdVSz, 720*2, 480, YUV, FREE_RUN, &oSc);
	POST_SetInterlaceModeOnOff(1, &oSc);
	POST_SetClkSrc(ePostClkSrc, &oSc);
	POST_StartProcessing(&oSc);
	
	POST_InitIpForDmaInFifoOut(uLcdHSz, uLcdVSz, (u32)uGlibStAddr0, RGB24, uLcdHSz, uLcdVSz, RGB, 1, &oPost);
	POST_SetNextFrameStAddr((u32)uGlibStAddr0, 0, &oPost); // For Local Path
	POST_SetClkSrc(HCLK, &oPost);
	POST_StartProcessing(&oPost);
	
	Delay(10000);

	uImageNum = 0;
	while(uImageNum<uNumOfImage)
	{	
		POST_SetNextFrameStAddr((u32)uGlibStAddr1[uImageNum], 0, &oPost); // For Local Path
		Delay(10000);
		uImageNum++;
	}

	INTC_Disable(NUM_TVENC);
	
	POST_StopProcessingOfFreeRun(&oPost);
	while (!POST_IsProcessingDone(&oPost));	

	// oScaler die..
	POST_SetInterlaceModeOnOff(0, &oSc);
	POST_StopProcessingOfFreeRun(&oSc);
	while (!POST_IsProcessingDone(&oSc));

	LCD_Stop();

	TVENC_TurnOnOff(0);
}


void ScalerT_Post_FIMD_Scaler_TVEnc_SourceClk_Test(void)
{
	u32 *uGlibStAddr0;
	u32 i, j, uNumOfTestImage, uCount;
	u32 *uGlibStAddr1[30];

#if (LCD_MODULE_TYPE ==LTE480WV_RGB)
	u32 eLcdClk[5][2] = {	{SRC_HCLK, 1},
						{SRC_MOUT_EPLL, 1},
						{SRC_FIN_EPLL, 0},
						{SRC_DOUT_MPLL, 1},
						{SRC_27M, 0}};

#elif (LCD_MODULE_TYPE== LTV350QV_RGB)
	u32 eLcdClk[5][2] = { 	{SRC_HCLK, 17},
						{SRC_MOUT_EPLL, 17},
						{SRC_FIN_EPLL, 1},
						{SRC_DOUT_MPLL, 17},
						{SRC_27M, 3}};
#endif	

	POST_CLK_SRC ePostClk[5] = { HCLK, PLL_EXT_MOUTEPLL, PLL_EXT_FINEPLL, PLL_EXT_DOUTMPLL, EXT_27MHZ};

	uNumOfTestImage = sizeof(Scaler_Image_Entertain)/4;
	uCount = 0;
	
	LCD_InitDISPCForFifoIn(IN_POST, WIN0);
	LCD_SetClkSrc((CLK_SRC)ECLK1_SRC);
	LCD_SetClkVal(5);	
	LCD_SetWinOnOff(1, WIN0);
	LCD_GetFrmSz(&uLcdHSz, &uLcdVSz, WIN0);

	uGlibStAddr0 = (u32 *)malloc(uLcdHSz*uLcdVSz*4);

	UART_Printf("Downloading the Image....\n");
	for(i=0 ; i<uNumOfTestImage ; i++)
	{
		uGlibStAddr1[i] = (u32 *)malloc(uLcdHSz*uLcdVSz*4);
		ScalerT_GetImageFromBMP((u32)uGlibStAddr1[i], uLcdHSz, uLcdVSz, i, RGB24);
	}
	
	GLIB_Init((u32)uGlibStAddr0, uLcdHSz, uLcdVSz, RGB24);
	GLIB_DrawPattern(uLcdHSz, uLcdVSz);	

	while(!UART_GetKey())
	{
		for(i=0 ; i<5 ; i++)
		{
			for(j=0 ; j<5 ; j++)
			{
				ScalerT_Post_FIMD_Scaler_TVEnc(	(CLK_SRC)eLcdClk[i][0],  eLcdClk[i][1], ePostClk[j], 
													(u32 *)uGlibStAddr0, (u32 *)uGlibStAddr1, uNumOfTestImage);
			}
		}
		uCount++;
		UART_Printf("\n");
		UART_Printf("Count : %d\n", uCount);
	}

	free(uGlibStAddr0);
	for(i=0 ; i<uNumOfTestImage ; i++)
	{
		free(uGlibStAddr1[i]);
	}

}

////////////////////////////////////////////////////////////////////////////
//////////////////                       POST Test Main                      ///////////////////
////////////////////////////////////////////////////////////////////////////


const testFuncMenu scaler_menu[] =
{
	ScalerT_ConvertDMAToDMAPath,                        			"DMA to DMA",
	ScalerT_ConvertDMAToFIFOPath,                                  	"DMA to FIFO",		
	ScalerT_ConvertFIFOToDMAPath,                   			"FIFO to DMA(FIFO Source: LCDC)",
	ScalerT_SelectClockSource,							"Clock Source Selection",
	ScalerT_SelectClockDivider,							"Clock Divider Selection",	
	ScalerT_Scaler_FIMD_SyncAsyncClk,					"Source Clock aging(Scaler -> FIMD)",	
	ScalerT_Scaler_TVEnc_SourceClk_Test,				"Source Clock aging(Scaler -> TVenc)",
	ScalerT_Post_FIMD_Scaler_TVEnc_SourceClk_Test,		"Source Clock aging(Post -> FIMD -> Scaler -> TVenc)",
	0,                                          0
};
	
void Scaler_Test(void)
{
	u32 i;
	s32 uSel;

	UART_Printf("[POST_Test]\n");

	POST_InitCh(POST_A, &oPost);
	POST_InitCh(POST_B, &oSc);

	//LCD Setting
	LCD_SetPort();
	LCD_InitLDI(MAIN);	
	
	while(1)
	{
		UART_Printf("\n");
		for (i=0; (int)(scaler_menu[i].desc)!=0; i++)
		{
			UART_Printf("%2d: %s\n", i, scaler_menu[i].desc);
		}

		UART_Printf("\n");
		UART_Printf("Select The Function To Test: ");

		uSel = UART_GetIntNum();
		UART_Printf("\n");

		if (uSel == -1)
			break;
		if (uSel>=0 && uSel<(sizeof(scaler_menu)/8-1))
			(scaler_menu[uSel].func)();
	}

	LCD_Stop();
}
