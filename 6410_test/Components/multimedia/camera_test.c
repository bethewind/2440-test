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
*	File Name : camera_test.c
*  
*	File Description : Camera Test Function 
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

#include <string.h>
#include <stdio.h>

#include "system.h"
#include "lcd.h"
#include "camera.h"
#include "glib.h"
#include "intc.h"
#include "option.h"
#include "library.h"
#include "sfr6410.h"
#include "sysc.h"
#include "gpio.h"
#include "Smile.h"
#include "Smile800480_24bpp3.h"
#include "Smile800480_24bpp2.h"
#include "post.h"

static POST oPost;
static POST oSc;

static CSPACE eSrcDataFmt; // For source data format of direct path
static CSPACE eDstDataFmt; // For destinaion data format of codec path
static CSPACE eLcdBpp;

static IMAGE_EFFECT   	eImgEffect;
static FLIP_DIR 	    eFlipDir;
static ROT_DEG 	     	eRotDeg;
static CAMIF_INOUT		eInputPath;
static PROCESS_PATH     eProcessPath;
static CAMIF_INOUT		eOutputPath;

static u32 uSrcCroppedHsz, uSrcCroppedVsz;
static u32 uSrcWidth, uSrcHeight;
static u32 uArbitraryCbValue, uArbitraryCrValue;

static u32 uLcdFbAddr;

static volatile u32 uFbAddr1 = 0x53000000;
static volatile u32 uFbAddr2 = 0x54000000;
static volatile u32 uFbAddr3 = 0x55000000;
static volatile u32 uFbAddr4 = 0x56000000;

static volatile u8 bPreviewDone;
static volatile u8 bCodecDone;


static volatile u32 uFramec;


static CSPACE eYuvFmt=YCBYCR;
static u32 uHFrame=480;
static u32 uVFrame=640;
static ITU_R_STANDARD eVideoStandard = BT601;
static CSPACE uImgResolution;
static u32 uImgXsize;
static u32 uImgYsize;
#if 0 
const static u32 uImgAddr = CODEC_MEM_ST+(CODEC_MEM_LMT-CODEC_MEM_ST)/4; 	//0x61000000
const static u32 uImgAddr2 = CODEC_MEM_ST+3*(CODEC_MEM_LMT-CODEC_MEM_ST)/4;  //0x63000000
const static u32 uImgAddr3 = CODEC_MEM_ST+4*(CODEC_MEM_LMT-CODEC_MEM_ST)/4;  //0x63000000
#else
const static u32 uImgAddr = 0x53000000;
const static u32 uImgAddr2 = 0x54000000;
const static u32 uImgAddr3 = 0x55000000;
#endif

static u32 uGlibStAddr;
static u32 uConvertAddr;

// for Last IRQ Test
volatile u32 uCamPreviewStatus, uCamPreviewDataValid, uCamPreviewCptCount;

volatile u32 uCamCodecStatus, uCamCodecDataValid, uCamCodecCptCount;
#define CAPTURE_STOPPED			(0)
#define CAPTURING					(1)
#define CAPTURE_STOP_ISSUED		(2)
#define CAPTURE_LASTIRQ_ISSUED	(3)
#define CAPTURE_LAST_IRQ			(4)

//////////
// Function Name : isr_previewdone
// Function Description : Preview Path ISR 
// Input : NONE
// Output : NONE
// Version : v0.1
void __irq Isr_PreviewDone(void)
{
	LCD_Trigger();
	bPreviewDone = true;
	UART_Printf("P");
	CAMERA_SetClearPreviewInt();
	INTC_ClearVectAddr();		
}


//////////
// Function Name : isr_4previewdone
// Function Description : Preview Path ISR 
// Input : NONE
// Output : NONE
// Version : v0.1
void __irq Isr_4PreviewDone(void)
{
	u32 uFrameNumber;
	LCD_Trigger();
	bPreviewDone = true;
	UART_Printf("P");
	CAMERA_GetFrameNum(P_PATH, &uFrameNumber);

	switch ( (uFrameNumber-2) )
	{
		case 0:
			LCD_InitWin(eLcdBpp, 320, 240, 0, 0, 320, 240, 0, 0, uFbAddr1, WIN0, false);
			break;
		case 1:
			LCD_InitWin(eLcdBpp, 320, 240, 0, 0, 320, 240, 0, 0, uFbAddr2, WIN0, false);
			break;
		case 2:
			LCD_InitWin(eLcdBpp, 320, 240, 0, 0, 320, 240, 0, 0, uFbAddr3, WIN0, false);
			break;
		case 3:
			LCD_InitWin(eLcdBpp, 320, 240, 0, 0, 320, 240, 0, 0, uFbAddr4, WIN0, false);
			break;
		default :
			break;
	}

	CAMERA_SetClearPreviewInt();
	INTC_ClearVectAddr();		
}






//////////
// Function Name : isr_Codecdone
// Function Description : Codec Path ISR 
// Input : NONE
// Output : NONE
// Version : v0.1
void __irq Isr_CodecDone(void)
{
	LCD_Trigger();
	UART_Printf("C");
	bCodecDone = true;
	CAMERA_SetClearCodecInt();
	INTC_ClearVectAddr();
}


//////////
// Function Name : isr_previewdone
// Function Description : Preview Path ISR 
// Input : NONE
// Output : NONE
// Version : v0.1
void __irq Isr_MSDMAPreviewDone(void)
{
	LCD_Trigger();
	bPreviewDone = true;
	UART_Printf("PMS");
	CAMERA_StartMSDmaPreviewPath();
	CAMERA_SetClearPreviewInt();
	INTC_ClearVectAddr();		
}


//////////
// Function Name : isr_previewdone
// Function Description : Preview Path ISR 
// Input : NONE
// Output : NONE
// Version : v0.1
void __irq Isr_MSDMACodecDone(void)
{
	LCD_Trigger();
	bCodecDone = true;
	UART_Printf("CMS");
	CAMERA_StartMSDmaCodecPath();
	CAMERA_SetClearCodecInt();
	INTC_ClearVectAddr();		
}


//////////
// Function Name : isr_LastIRQDone
// Function Description : Preview Path Last IRQ Test
// Input : NONE
// Output : NONE
// Version : v0.1
void __irq Isr_LastIRQDone(void)
{
	UART_Printf("L");

	switch(uCamPreviewStatus) {

	case CAPTURE_STOPPED	:					//	It will never happen.
			break;
			
	case CAPTURING	:
			break;

	case CAPTURE_STOP_ISSUED	:
			CAMERA_EnablePreviewLastIRQ();
			uCamPreviewStatus	=	CAPTURE_LASTIRQ_ISSUED;
			break;

	case CAPTURE_LASTIRQ_ISSUED	:
			CAMERA_DisableImageCapture();
			CAMERA_DisableImageCapturePreview();
			CAMERA_DisablePreviewScaler();
			uCamPreviewStatus	=	CAPTURE_LAST_IRQ;
			break;			

	case CAPTURE_LAST_IRQ	:
			uCamPreviewStatus	=	CAPTURE_STOPPED;
			break;
	}

	if (uCamPreviewCptCount>0)
		uCamPreviewDataValid	=	1;
				
	uCamPreviewCptCount++;	
	CAMERA_SetClearPreviewInt();
	INTC_ClearVectAddr();		
	
}


static void CAMERAT_MakeImage(void)
{
	u32 sel;
	u32 sel_image;
	u32 i;

	UART_Printf("What do you want to display Image format, RGB or YUV ?\n");
	UART_Printf("[0] : RGB\n");
	UART_Printf("[1] : YUV\n");
	UART_Printf("Select number : ");
	UART_Printf("\n");
		
	sel = UART_GetIntNum();

	if (sel == 1)
	{
		UART_Printf("\nSelect Image..\n");
		UART_Printf("[0] : YCbCr420 QCIF [176x144] Non-Interleave\n");
		UART_Printf("[1] : YCbCr420 QVGA [320x240] Non-Interleave\n");
		UART_Printf("[2] : YCbCr420 CIF  [352x288] Non-Interleave\n");
		UART_Printf("[3] : YCbCr420 VGA  [640x480] Non-Interleave\n");
		UART_Printf("[4] : YCbCr420 WIDE [720x480] Non-Interleave\n");
		UART_Printf("[5] : YCbCr420 XGA  [1024x768] Non-Interleave\n");
		UART_Printf("[6] : YCbCr420 HD   [1280x960] Non-Interleave\n");
		UART_Printf("[7] : YCbCr420 QXGA   [2048x1536] Non-Interleave\n");
		UART_Printf("[8] : YCbCr422 QCIF [176x144] Interleave\n");
		UART_Printf("[9] : YCbCr422 QVGA [320x240] Interleave\n");
		UART_Printf("[10] : YCbCr422 CIF  [352x288] Interleave\n");
		UART_Printf("[11]: YCbCr422 VGA  [640x480] Interleave\n");
		UART_Printf("[12]: YCbCr422 WIDE [720x480] Interleave\n");
		UART_Printf("[13]: YCbCr422 XGA  [1024x768] Interleave\n");
		UART_Printf("[14]: YCbCr422 HD   [1280x960] Interleave\n");
		UART_Printf("[15]: YCbCr422 QXGA   [2048x1536] Interleave\n");
		UART_Printf("Select number : ");
		UART_Printf("\n");

		sel_image = UART_GetIntNum();

		if (sel_image >= 7)
		{
			uImgResolution = YCBYCR;
			while(true)
			{
			 	u32 uParam, x;

				UART_Printf("[0] Exit \n");
				UART_Printf("[1] Select YUV422 Ordering format \n");
				UART_Printf("Select number : ");
				UART_Printf("\n");
				uParam = UART_GetIntNum();

				if (uParam == 0)
					break;
				else
				{
					UART_Printf(" Ordering format (1: YCbYCr  2: YCrYCb  3: CbYCrY  4: CrYCbY) \n");
					UART_Printf("Select number : ");
					UART_Printf("\n");

					x = UART_GetIntNum();
					if (x == 1)
						uImgResolution = YCBYCR;
					else if (x == 2)
						uImgResolution = YCRYCB;
					else if (x == 3)
						uImgResolution = CBYCRY;
					else
						uImgResolution = CRYCBY;
				}

			}
			
		}
		else 
			uImgResolution = YC420;
		
		switch (sel_image % 8)
		{
			case 0:
				uImgXsize = 176, uImgYsize = 144;
				break;
			case 1:
				uImgXsize = 320, uImgYsize = 240;
				break;
			case 2:
				uImgXsize = 352, uImgYsize = 288;
				break;
			case 3:
				uImgXsize = 640, uImgYsize = 480;
				break;
			case 4:
				uImgXsize = 720, uImgYsize = 480;
				break;
			case 5:
				uImgXsize = 1024, uImgYsize = 768;
				break;
			case 6:
				uImgXsize = 1280, uImgYsize = 960;
				break;
			case 7:
				uImgXsize = 2048, uImgYsize = 1536;
				break;
		}

		// Image Buffer clear
		for (i = 0; i<uImgXsize*uImgYsize*4; i++)
		*(u8 *)(uImgAddr+i) = 0;

		for (i = 0; i<uImgXsize*uImgYsize*4; i++)
		*(u8 *)(uLcdFbAddr+i) = 0;
	
		uGlibStAddr = uImgAddr + uImgXsize*uImgYsize*2;

		GLIB_InitInstance(uGlibStAddr, uImgXsize, uImgYsize, RGB24);
		GLIB_DrawPattern(uImgXsize, uImgYsize);

	#if 0
		CAMERA_InitDmaInPath(uImgXsize, uImgYsize, uGlibStAddr, RGB24, 640, 480, uImgAddr3, RGB24, FLIP_NO, ROT_0, C_PATH);
		CAMERA_StartDmaInPath();
		while(!bCodecDone);
		CAMERA_WaitMSDMAC();		
		CAMERA_StopDmaInPath();


		CAMERA_InitDmaInPath(640, 480, uImgAddr3, RGB24, uImgXsize, uImgYsize, uImgAddr, uImgResolution, FLIP_NO, ROT_0, C_PATH);
		CAMERA_StartDmaInPath();
		while(!bCodecDone);
		CAMERA_WaitMSDMAC();		
		CAMERA_StopDmaInPath();
	#else
		POST_InitIp(uImgXsize, uImgYsize, uGlibStAddr, RGB24, uImgXsize, uImgYsize, uImgAddr, uImgResolution,
			1, false, ONE_SHOT, &oPost);
		POST_StartProcessing(&oPost);

		while (!POST_IsProcessingDone(&oPost));
	#endif
	
	}
	else
	{
		u32 sel_image;

		UART_Printf("\nSelect Image\n");
		UART_Printf("[0] : RGB16 QCIF [176x144]\n");
		UART_Printf("[1] : RGB16 QVGA [320x240]\n");
		UART_Printf("[2] : RGB16 CIF  [352x288]\n");
		UART_Printf("[3] : RGB16 VGA  [640x480]\n");
		UART_Printf("[4] : RGB16 WIDE [720x480]\n");
		UART_Printf("[5] : RGB16 XGA  [1024x768]\n");
		UART_Printf("[6] : RGB16 HD   [1280x960]\n");
		UART_Printf("[7] : RGB24 QCIF [176x144]\n");
		UART_Printf("[8] : RGB24 QVGA [320x240]\n");
		UART_Printf("[9] : RGB24 CIF  [352x288]\n");
		UART_Printf("[10]: RGB24 VGA  [640x480]\n");
		UART_Printf("[11]: RGB24 WIDE [720x480]\n");
		UART_Printf("[12]: RGB24 XGA  [1024x768]\n");
		UART_Printf("[13]: RGB24 HD   [1280x960]\n");
		UART_Printf("Select number : ");
		UART_Printf("\n");

		sel_image=UART_GetIntNum();

		if (sel_image >= 7)
			uImgResolution = RGB24;
		else
			uImgResolution = RGB16;

		switch (sel_image % 7)
		{
			case 0:
				uImgXsize = 176, uImgYsize = 144;
				break;
			case 1:
				uImgXsize = 320, uImgYsize = 240;
				break;
			case 2:
				uImgXsize = 352, uImgYsize = 288;
				break;
			case 3:
				uImgXsize = 640, uImgYsize = 480;
				break;
			case 4:
				uImgXsize = 720, uImgYsize = 480;
				break;
			case 5:
				uImgXsize = 1024, uImgYsize = 768;
				break;
			case 6:
				uImgXsize = 1280, uImgYsize = 960;
				break;
		}

			// Image Buffer clear
		for (i = 0; i<uImgXsize*uImgYsize*4; i++)
		*(u8 *)(uImgAddr+i) = 0;

		for (i = 0; i<uImgXsize*uImgYsize*4; i++)
		*(u8 *)(uLcdFbAddr+i) = 0;

		GLIB_InitInstance(uImgAddr, uImgXsize, uImgYsize, uImgResolution);
		GLIB_DrawPattern(uImgXsize, uImgYsize);

	}
}






//////////
// Function Name : CAMERAT_DisplayParam
// Function Description : Test condition display function
// Input : NONE
// Output : NONE
// Version : v0.1
static void CAMERAT_DisplayParam(void)
{
	const char* cpDstDataFmtStr;
	const char* cpMemSrcDataFmtStr;
	const char* cpImgEffectStr;
	const char* cpFlipDirStr;
	const char* cpRotStr;
	const char* cpLcdBppStr;
	u32 uSrcHSz, uSrcVSz;
	
	if (eOutputPath==FIFO)
		cpDstDataFmtStr = ((eDstDataFmt == RGB16)||(eDstDataFmt == RGB18)||(eDstDataFmt == RGB24)) ? "RGB 24Bpp" : "YUV444" ;
	else
		cpDstDataFmtStr =(eDstDataFmt == RGB16) ? "RGB 16Bpp" :
			(eDstDataFmt == RGB18) ? "RGB 18Bpp" :
			(eDstDataFmt == RGB24) ? "RGB 24Bpp" :
			(eDstDataFmt == YCBYCR) ? "YCbYCr422_Intlv" :
			(eDstDataFmt == YCRYCB) ? "YCrYCb422_intlv" :
			(eDstDataFmt == CBYCRY) ? "CbYCrY422_Intlv" :
			(eDstDataFmt == CRYCBY) ? "CrYCbY422_Intlv" :
			(eDstDataFmt == YC422) ? "YC422" :
			(eDstDataFmt == YC420) ? "YC420" :"error";
	if (cpDstDataFmtStr == "error")
		Assert(0);

	cpMemSrcDataFmtStr =(eSrcDataFmt == YCBYCR) ? "YCbYCr422_Intlv" :
		(eSrcDataFmt == YCRYCB) ? "YCrYCb422_intlv" :
		(eSrcDataFmt == CBYCRY) ? "CbYCrY422_Intlv" :
		(eSrcDataFmt == CRYCBY) ? "CrYCbY422_Intlv" :
		(eSrcDataFmt == YC422) ? "YC422" :
		(eSrcDataFmt == YC420) ? "YC420" :
		(eSrcDataFmt == RGB16) ? "RGB 16Bpp" :
		(eSrcDataFmt == RGB18) ? "RGB 18Bpp" :
		(eSrcDataFmt == RGB24) ? "RGB 24Bpp" :"error";
	if (cpMemSrcDataFmtStr == "error")
		Assert(0);

	cpImgEffectStr =(eImgEffect == BYPASS) ? "No Image Effect" :
		(eImgEffect == ARBITRARY_CBCR) ? "Arbitrary Cb/Cr" :
		(eImgEffect == NEGATIVE) ? "Negative" :
		(eImgEffect == ART_FREEZE) ? "Art Freeze" :
		(eImgEffect == EMBOSSING) ? "Embossing" :
		(eImgEffect == SILHOUETTE) ? "Silhouette" : "error";
	if (cpImgEffectStr == "error")
		Assert(0);

	cpFlipDirStr =(eFlipDir == FLIP_NO) ? "No Flip" :
		(eFlipDir == FLIP_X) ? "X-Axis Flip" :
		(eFlipDir == FLIP_Y) ? "Y-Axis Flip" :
		(eFlipDir == FLIP_XY) ? "XY-Axis Flip" : "error";
	if (cpFlipDirStr == "error")
		Assert(0);

	cpRotStr = (eRotDeg == ROT_0) ? "No rotation" :
		(eRotDeg == ROT_90) ? "Rotation 90" :
		(eRotDeg == ROT_180) ? "Rotation 180" :
		(eRotDeg == ROT_270) ? "Rotation 270" : "error";
	if (cpRotStr == "error")
		Assert(0);

	cpLcdBppStr = (eLcdBpp == RGB16) ? "RGB 16Bpp" :
		(eLcdBpp == RGB18) ? "RGB 18Bpp" :  "RGB 24Bpp";

	
	UART_Printf("\n\nBasic info:\n");
	UART_Printf("\t- Lcd input bpp: %s\n", cpLcdBppStr);
	UART_Printf("\t- Output data format: %s\n", cpDstDataFmtStr);
	if (eInputPath==FIFO)
	{
		CAMERA_GetSrcImgSz(&uSrcHSz, &uSrcVSz);
	}
	else
		uSrcHSz = uSrcWidth, uSrcVSz=uSrcHeight;
	UART_Printf("Features info:\n");
	UART_Printf(" \t- Flip : %s\n", cpFlipDirStr);
	UART_Printf(" \t- Rotation : %s\n", cpRotStr);
	if (eImgEffect == ARBITRARY_CBCR)
		UART_Printf("\t- Image effect=Arbitrary CbCr, Cb=%d, Cr=%d\n", uArbitraryCbValue, uArbitraryCrValue);
	else
		UART_Printf(" \t- Image effect: %s\n", cpImgEffectStr);

	UART_Printf("Source image info.\n");
	if (eInputPath==DMA)
		UART_Printf("\t- Memory Input Data Format: %s\n", cpMemSrcDataFmtStr);
	UART_Printf(" \t- Source width: %d\t - Source height: %d\n", uSrcHSz, uSrcVSz);
	UART_Printf(" \t- Cropped width: %d\t - Cropped height: %d\n", uSrcCroppedHsz, uSrcCroppedVsz);

}

//////////
// Function Name : CAMERAT_TestTestPattern
// Function Description : Function of Test Pattern Test
// Input : NONE
// Output : NONE
// Version : v0.1
static void CAMERAT_TestTestPattern(void)
{
	u32 uIndex;
	u32 uLcdHsz, uLcdVsz;
	TEST_PATTERN eTestPattern ;
	for (uIndex = 0 ; uIndex<3 ; uIndex++)
	{
		switch(uIndex)
		{
			case 0:
				eTestPattern = VERTICAL_INCR;
				break;
			case 1:
				eTestPattern = COLOR_BAR;	
				break;
			case 2:
				eTestPattern = HORIZONTAL_INCR;	
				break;
			default:
				break;
	 	}

		eProcessPath = C_PATH;
		// 1. Initialize interrupt
		//=======================================
		INTC_SetVectAddr(NUM_CAMIF_P, Isr_PreviewDone);
		INTC_SetVectAddr(NUM_CAMIF_C, Isr_CodecDone);	
		INTC_Enable(NUM_CAMIF_P);
		INTC_Enable(NUM_CAMIF_C);

		// 2. Initialize lcd
		//=======================================
		LCD_InitDISPC(eLcdBpp, uLcdFbAddr, WIN0, false);
		LCD_GetFrmSz(&uLcdHsz, &uLcdVsz, WIN0);	
		LCD_SetWinOnOff(1, WIN0);
		LCD_Start();

		// 3. Initialize camera. run the process of preview/codec path
		//=============================================
		CAMERA_InitSensor();
		if (eProcessPath==P_PATH)
		{
			CAMERA_InitPreviewPath(uLcdHsz, uLcdVsz, uLcdFbAddr, eLcdBpp, FLIP_NO, ROT_0);
			CAMERA_EnableTestMode(eTestPattern);
			CAMERA_StartPreviewPath();
			UART_Printf("\nHit Any Key to Stop the test!\n");
			UART_Getc(); 
			CAMERA_StopPreviewPath();
		}
		else
		{
			CAMERA_InitCodecPath(uLcdHsz, uLcdVsz, uLcdFbAddr, eLcdBpp, FLIP_NO, ROT_0);
			CAMERA_EnableTestMode(eTestPattern);
			CAMERA_StartCodecPath(0);
			UART_Printf("\nHit Any Key to Stop the test!\n");
			UART_Getc();
			CAMERA_StopCodecPath();
		}

		INTC_Disable(NUM_CAMIF_P);
		INTC_Disable(NUM_CAMIF_C);	
	}
	
}


//////////
// Function Name : CAMERAT_TestZoomInOut
// Function Description : Function of ZoomIn/Out Test(Preview Path)
// Input : NONE
// Output : NONE
// Version : v0.1
static void CAMERAT_TestZoomInOut_Preview(void)
{
	u32 uLcdHsz, uLcdVsz;
	u32 uSrcHSz, uSrcVSz;
	char cChar;
	u32 uCurHsz;
	u32 uCurVsz;
	u32 uSrcStX;
	u32 uSrcStY;
	u32 uHStep;
	u32 uVStep;
	int nX;
	
	// 1. Initialize interrupt 
	//=======================================
	INTC_SetVectAddr(NUM_CAMIF_P, Isr_PreviewDone);
	INTC_Enable(NUM_CAMIF_P);
	
	// 2. Initialize lcd
	//=======================================
	LCD_InitDISPC(eLcdBpp, uLcdFbAddr, WIN0, false);
	LCD_GetFrmSz(&uLcdHsz, &uLcdVsz, WIN0);	
	LCD_SetWinOnOff(1, WIN0);
	LCD_Start();

	// 3. Initialize camera and variables of the fimc, run the zoom in/out process
	//=======================================

	CAMERA_InitSensor();
	CAMERA_GetSrcImgSz(&uSrcHSz, &uSrcVSz);

	uCurHsz = uSrcHSz;
	uCurVsz = uSrcVSz;
	uSrcStX = 0;
	uSrcStY = 0;
	uHStep = 16;
	uVStep = 16*uSrcVSz/uSrcHSz;
	eFlipDir = FLIP_NO;
	eRotDeg = ROT_0;

	while (true)
	{
		CAMERA_InitPreviewPath2(uSrcStX, uSrcStY, uCurHsz, uCurVsz,
			uLcdHsz, uLcdVsz, uLcdFbAddr, eLcdBpp, eFlipDir, eRotDeg);
		CAMERA_StartPreviewPath();
		UART_Printf("\nHit Any Key to Stop preview!\n\n");
		UART_Getc();
		CAMERA_StopPreviewPath();

		UART_Printf("\n 0 : Exit\n");
		UART_Printf(" 1 : Zoom In\n");
		UART_Printf(" 2 : Zoom Out\n");

		UART_Printf("Hit Key: ");
		nX = UART_GetIntNum();
		UART_Printf("%c\n", cChar);

		if (nX == 1)
		{
			uSrcStX += uHStep;
			uSrcStY += uVStep;

			if ((uCurHsz >= 16+2*uSrcStX)||(uCurVsz>= 8+2*uSrcStY))
			{
				uCurHsz -= 2*uSrcStX;
				uCurVsz -= 2*uSrcStY;
			}
			else
			{
				uSrcStX -= uHStep;
				uSrcStY -= uVStep;
			}
		}
		else if (nX == 2)
		{
			if ((uCurHsz+2*uSrcStX)<=uSrcHSz && (uCurVsz+2*uSrcStY)<=uSrcVSz)
			{
				uCurHsz += 2*uSrcStX;
				uCurVsz += 2*uSrcStY;
			}

			if (uSrcStX>=uHStep && uSrcStY>=uVStep)
			{
				uSrcStX -= uHStep;
				uSrcStY -= uVStep;
			}
		}
		else if (nX == 0)
		{
			break;
		}
		else
			UART_Printf("Invaild Input! Retry It!!\n");

		UART_Printf("Src Cropping Start XSz: %d, Src Cropping Start YSz: %d\n", uSrcStX, uSrcStY);
		UART_Printf("Src Cropping HSz: %d, Src Cropping VSz: %d\n", uCurHsz, uCurVsz);
	}
	INTC_Disable(NUM_CAMIF_P);
}

//////////
// Function Name : CAMERAT_TestZoomInOut_Codec
// Function Description : Function of ZoomIn/Out Test at Codec Path
// Input : NONE
// Output : NONE
// Version : v0.1
static void CAMERAT_TestZoomInOut_Codec(void)
{
	u32 uLcdHsz, uLcdVsz;
	u32 uSrcHSz, uSrcVSz;
	char cChar;
	u32 uCurHsz;
	u32 uCurVsz;
	u32 uSrcStX;
	u32 uSrcStY;
	u32 uHStep;
	u32 uVStep;
	int nX;
	
	// 1. Initialize interrupt 
	//=======================================
	INTC_SetVectAddr(NUM_CAMIF_C, Isr_CodecDone);
	INTC_Enable(NUM_CAMIF_C);
	
	// 2. Initialize lcd
	//=======================================
	LCD_InitDISPC(eLcdBpp, uLcdFbAddr, WIN0, false);
	LCD_GetFrmSz(&uLcdHsz, &uLcdVsz, WIN0);	
	LCD_SetWinOnOff(1, WIN0);
	LCD_Start();

	// 3. Initialize camera and variables of the fimc, run the zoom in/out process
	//=======================================

	CAMERA_InitSensor();
	CAMERA_GetSrcImgSz(&uSrcHSz, &uSrcVSz);


	uCurHsz = uSrcHSz;
	uCurVsz = uSrcVSz;
	uSrcStX = 0;
	uSrcStY = 0;
	uHStep = 16;
	uVStep = 16*uSrcVSz/uSrcHSz;
	eFlipDir = FLIP_NO;
	eRotDeg = ROT_0;

	while (true)
	{
		CAMERA_InitCodecPath0(uSrcStX, uSrcStY, uCurHsz, uCurVsz, uLcdHsz, uLcdVsz, 0, 0, uLcdHsz, uLcdVsz, 
			uLcdFbAddr, uLcdFbAddr, eLcdBpp, eFlipDir, eRotDeg);
		CAMERA_StartCodecPath(0);
		UART_Printf("\nHit Any Key to Stop Codec!\n\n");
		UART_Getc();
		CAMERA_StopCodecPath();

		UART_Printf("\n 0 : Exit\n");
		UART_Printf(" 1 : Zoom In\n");
		UART_Printf(" 2 : Zoom Out\n");

		UART_Printf("Hit Key: ");
		nX = UART_GetIntNum();
		UART_Printf("%c\n", cChar);

		if (nX == 1)
		{
			uSrcStX += uHStep;
			uSrcStY += uVStep;

			if ((uCurHsz >= 16+2*uSrcStX)||(uCurVsz>= 8+2*uSrcStY))
			{
				uCurHsz -= 2*uSrcStX;
				uCurVsz -= 2*uSrcStY;
			}
			else
			{
				uSrcStX -= uHStep;
				uSrcStY -= uVStep;
			}
		}
		else if (nX == 2)
		{
			if ((uCurHsz+2*uSrcStX)<=uSrcHSz && (uCurVsz+2*uSrcStY)<=uSrcVSz)
			{
				uCurHsz += 2*uSrcStX;
				uCurVsz += 2*uSrcStY;
			}

			if (uSrcStX>=uHStep && uSrcStY>=uVStep)
			{
				uSrcStX -= uHStep;
				uSrcStY -= uVStep;
			}
		}
		else if (nX == 0)
		{
			break;
		}
		else
			UART_Printf("Invaild Input! Retry It!!\n");

		UART_Printf("Src Cropping Start XSz: %d, Src Cropping Start YSz: %d\n", uSrcStX, uSrcStY);
		UART_Printf("Src Cropping HSz: %d, Src Cropping VSz: %d\n", uCurHsz, uCurVsz);
	}
	INTC_Disable(NUM_CAMIF_C);
}




//////////
// Function Name : CAMERAT_TestPreviewPath_Simple
// Function Description : Basic Function of Preview Path Camera In DMA Out
// Input : 	None
// Output :    	None
static void CAMERAT_TestPreviewPath_Simple(void)
{
	u32 uLcdHsz, uLcdVsz;
	u32 uLcdHsz2, uLcdVsz2;
	u32 i,j,k;
	u32 uLcdFbAddr2;
	// 1. Initialize interrupt process
	//=======================================
	INTC_SetVectAddr(NUM_CAMIF_P, Isr_PreviewDone);
	INTC_Enable(NUM_CAMIF_P);

	// 2. Initialize lcd
	//=======================================
	*(volatile unsigned *)0x7410800c=0x0; // Modem I/F Bypass Control. It should be 0,
	LCD_InitDISPC(eLcdBpp, uLcdFbAddr, WIN0, false);
	LCD_GetFrmSz(&uLcdHsz, &uLcdVsz, WIN0);	
	LCD_SetWinOnOff(1, WIN0);
	GLIB_InitInstance(uLcdFbAddr, uLcdHsz, uLcdVsz, eLcdBpp);
	k=0;
	for(i=0; i<480; i++)
		for(j=0; j<800; j++)
			GLIB_PutPixel2(j, i, pSmile_3[k++]);

	LCD_GetFbEndAddr(&uLcdFbAddr2, WIN0);
	LCD_InitWin(eLcdBpp, 320, 240, 0, 0, 320, 240, 0, 0, uLcdFbAddr2, WIN1, false);
	LCD_SetWinOnOff(1, WIN1);
	LCD_Start();
	UART_Printf("\nHit any key to start Camera simple test!\n");
	UART_Getc();

	// 3. Initialize camera and run the  process of preview path.
	//========================================
	CAMERA_InitSensor();
	uLcdHsz2 = 320; 
	uLcdVsz2 = 240;
	CAMERA_InitPreviewPath(uLcdHsz2, uLcdVsz2, uLcdFbAddr2, eLcdBpp, FLIP_NO,ROT_0);		
	
	CAMERA_StartPreviewPath();
	UART_Printf("\nHit any key to stop it!\n");
	UART_Getc();
	CAMERA_StopPreviewPath();
	INTC_Disable(NUM_CAMIF_P);
	
	LCD_SetAllWinOnOff(0);
	LCD_Stop();
	UART_Printf("\nLCD OFF\n");
}


static void CAMERAT_TestPreviewPath_Rot(void)
{
	u32 uLcdHsz, uLcdVsz;
	u32 uLcdHsz2, uLcdVsz2;
	u32 i,j,k;
	u32 uLcdFbAddr2;
	
	// 1. Initialize interrupt process
	//=======================================	
	INTC_SetVectAddr(NUM_CAMIF_P, Isr_PreviewDone);
	INTC_Enable(NUM_CAMIF_P);
	bPreviewDone = false;

	// 2. Initialize lcd
	//=======================================
	*(volatile unsigned *)0x7410800c=0x0; // Modem I/F Bypass Control. It should be 0,
	LCD_InitDISPC(eLcdBpp, uLcdFbAddr, WIN0, false);
	LCD_GetFrmSz(&uLcdHsz, &uLcdVsz, WIN0);	
	LCD_SetWinOnOff(1, WIN0);
	GLIB_InitInstance(uLcdFbAddr, uLcdHsz, uLcdVsz, eLcdBpp);
	k=0;
	for(i=0; i<480; i++)
		for(j=0; j<800; j++)
			GLIB_PutPixel2(j, i, pSmile_3[k++]);

	LCD_GetFbEndAddr(&uLcdFbAddr2, WIN0);
	LCD_InitWin(eLcdBpp, 320, 240, 0, 0, 320, 240, 0, 0, uLcdFbAddr2, WIN1, false);
	LCD_SetWinOnOff(1, WIN1);
	LCD_Start();
	UART_Printf("\nHit any key to start Camera simple test!\n");
	UART_Getc();
	
	CAMERA_InitSensor();
	uLcdHsz2 = 320; 
	uLcdVsz2 = 240;
	CAMERA_InitPreviewPath(uLcdHsz2, uLcdVsz2, uLcdFbAddr2, eLcdBpp, FLIP_NO,ROT_90);		
	
	CAMERA_StartPreviewPath();
	UART_Printf("\nHit any key to stop it!\n");
	UART_Getc();
	CAMERA_StopPreviewPath();
	INTC_Disable(NUM_CAMIF_P);
	
	LCD_SetAllWinOnOff(0);
	LCD_Stop();
	UART_Printf("\nLCD OFF\n");

}




//////////
// Function Name : CAMERAT_TestPreviewPath_LastIRQ
// Function Description : Basic Function of Preview Path Camera In DMA Out( Using Last IRQ)
// Input : 	None
// Output :    	None
static void CAMERAT_TestPreviewPath_LastIRQ(void)
{
	u32 uLcdHsize, uLcdVsize;
	u32 uLcdHsize2, uLcdVsize2;
	u32 uLcdFbAddr2;
	u32 i,j,k;
	
	uCamPreviewStatus = CAPTURE_STOPPED;
	uCamPreviewCptCount = 0;
	uCamPreviewDataValid = 0 ;
	
	// 1. Initialize interrupt process
	//=======================================
	INTC_SetVectAddr(NUM_CAMIF_P, Isr_LastIRQDone);
	INTC_Enable(NUM_CAMIF_P);

	// 2. Initialize lcd
	//=======================================
	LCD_InitDISPC(eLcdBpp, uLcdFbAddr, WIN0, false);
	LCD_GetFrmSz(&uLcdHsize, &uLcdVsize, WIN0);	
	LCD_SetWinOnOff(1, WIN0);
	GLIB_InitInstance(uLcdFbAddr, uLcdHsize, uLcdVsize, eLcdBpp);
	k=0;
	for(i=0; i<480; i++)
		for(j=0; j<800; j++)
			GLIB_PutPixel2(j, i, pSmile_3[k++]);

	LCD_GetFbEndAddr(&uLcdFbAddr2, WIN0);
	LCD_InitWin(eLcdBpp, 320, 240, 0, 0, 320, 240, 0, 0, uLcdFbAddr2, WIN1, false);
	LCD_SetWinOnOff(1, WIN1);

	LCD_Start();
	
	// 3. Initialize camera and run the  process of preview path.
	//========================================
	CAMERA_InitSensor();
	uLcdHsize2 = 320;
	uLcdVsize2 = 240;
	CAMERA_InitPreviewPath(uLcdHsize2, uLcdVsize2, uLcdFbAddr2, eLcdBpp,FLIP_NO,ROT_0);	
	CAMERA_StartPreviewPath();

	uCamPreviewCptCount = 0 ; 
	uCamPreviewStatus = CAPTURING;
	
	UART_Printf("\nLast IRQ Enable Setting Preview Path Start!\n");

	while( UART_GetKey()==0 )
	{
		if( uCamPreviewDataValid)
		{
			uCamPreviewDataValid = 0 ; 
		}
	}

	uCamPreviewStatus = CAPTURE_STOP_ISSUED;
	
	while( !(uCamPreviewStatus == CAPTURE_STOPPED) );

	UART_Printf("\nPreview Capture Count ==%d\n", uCamPreviewCptCount);
	
	CAMERA_StopPreviewPath();

	INTC_Disable(NUM_CAMIF_P);

	LCD_SetAllWinOnOff(0);
	LCD_Stop();
	UART_Printf("\nLCD OFF\n");
}

//////////
// Function Name : CAMERAT_TestPreviewPath_Complex
// Function Description : This Function is Complex(ScanlineOffset) Test.. 
// Input		None
// Output :    	None
static void CAMERAT_TestPreviewPath_Complex(void)
{
//	u32 uBufAddr0, uBufAddr1;
	u32 uLcdHsz, uLcdVsz;
	u32 uLcdHsz2, uLcdVsz2;
	u32 uDstHsz, uDstVsz;
	u32 uDisplayStartX, uDisplayStartY;
	u32 uSrcCropStartX, uSrcCropStartY;
	u32 i,j,k;
	u32 uLcdFbAddr2;

	// 1. Initalize interrupt
	//=====================================
	INTC_Enable(NUM_CAMIF_P);
	INTC_SetVectAddr(NUM_CAMIF_P, Isr_PreviewDone);
	bPreviewDone = false;

	// 2. Initialize lcd with trigger mode
	//=====================================
	LCD_InitDISPC(eLcdBpp, uLcdFbAddr, WIN0, false);
	LCD_GetFrmSz(&uLcdHsz, &uLcdVsz, WIN0);	
	LCD_SetWinOnOff(1, WIN0);
	GLIB_InitInstance(uLcdFbAddr, uLcdHsz, uLcdVsz, eLcdBpp);
	k=0;
	for(i=0; i<480; i++)
		for(j=0; j<800; j++)
			GLIB_PutPixel2(j, i, pSmile_3[k++]);

	LCD_GetFbEndAddr(&uLcdFbAddr2, WIN0);	
	LCD_InitWin(eLcdBpp, 320, 240, 0, 0, 320, 240, 0, 0, uLcdFbAddr2, WIN1, false);
	LCD_SetWinOnOff(1, WIN1);

	LCD_Start();
	
	// 3. Initialize the camera and run the process of preview path
	//===========================================
	uDstHsz = 160, uDstVsz = 120, uDisplayStartX = 16, uDisplayStartY = 12;
	uSrcCropStartX = 0, uSrcCropStartY = 0;
	// uSrcCroppedHsz = 1600,uSrcCroppedVsz = 1200;
	// uSrcCropStartX = 8, uSrcCropStartY = 24,uSrcCroppedHsz = 144,uSrcCroppedVsz = 176;
	// eFlipDir = FLIP_NO, eRotDeg = ROT_90;
	uLcdHsz2 = 320, uLcdVsz2 = 240;
	CAMERA_InitSensor();
	CAMERA_InitPreviewPath0(uSrcCropStartX, uSrcCropStartY, uSrcCroppedHsz, uSrcCroppedVsz,
		uLcdHsz2, uLcdVsz2, uDisplayStartX, uDisplayStartY, uDstHsz, uDstVsz,
		uLcdFbAddr2, uLcdFbAddr2, eLcdBpp, eFlipDir, eRotDeg);
		
	if (eImgEffect == ARBITRARY_CBCR)
		CAMERA_SetArbitraryCbCr(uArbitraryCbValue, uArbitraryCrValue);
	else
		CAMERA_SetImageEffect(eImgEffect);

	CAMERAT_DisplayParam();	
	CAMERA_StartPreviewPath();
	UART_Printf("\nHit any key to stop it!\n");
	UART_Getc();
	CAMERA_StopPreviewPath();
	INTC_Disable(NUM_CAMIF_P);

	LCD_SetAllWinOnOff(0);
	LCD_Stop();
	UART_Printf("\nLCD OFF\n");	
}



//////////
// Function Name : CAMERAT_TestPreviewPath_AllParams
// Function Description : Preview Path All Parameter Test
// Input : 	None
// Output :    	None
// Test case = 5 + 4 + 5 + 4 + 1 = 19ea
static void CAMERAT_TestPreviewPath_AllParams(void)
{
	// 1. Set the case group of the input parameters.
	//================================
	u32 uSrcCropSzList[5][2] = {{192,260}, {560, 400},  {144, 176}, {160, 320}, {800, 600} };
	IMAGE_EFFECT eImgEffectList[6] = {BYPASS, ARBITRARY_CBCR, NEGATIVE, ART_FREEZE, EMBOSSING, SILHOUETTE};
	u32 uArbitraryCbCrList[5][2] = {{0, 0}, {128, 128}, {255, 255}, {0, 255}, {255, 0}};
	FLIP_DIR eFlipDirList[4] = { FLIP_X, FLIP_Y, FLIP_XY, FLIP_NO};
 	u32 uSrcCropIdx;
	u32 uImgEffectIdx;
	u32 uArbitraryIdx;
	u32 uFlipIdx;

	// 3. Change the case and test all the case
	//=================================
	for (uSrcCropIdx=0; uSrcCropIdx<5; uSrcCropIdx++)  // Scaler up,down, bypass = 5 EA (2*2 + 1)

	{
		uSrcCroppedHsz = uSrcCropSzList[uSrcCropIdx][0];
		uSrcCroppedVsz = uSrcCropSzList[uSrcCropIdx][1];

		UART_Printf("\nHit any key to test\n");
		UART_Getc();
		CAMERAT_TestPreviewPath_Complex();
	}

	
	for (uImgEffectIdx=2; uImgEffectIdx<6; uImgEffectIdx++) // Image effect = 4 EA
	{
		eImgEffect = eImgEffectList[uImgEffectIdx];

		UART_Printf("\nHit any key to test\n");
		UART_Getc();
		CAMERAT_TestPreviewPath_Complex();
	}

	eImgEffect = ARBITRARY_CBCR;
	for (uArbitraryIdx=0; uArbitraryIdx<5; uArbitraryIdx++) // Arbitrary Cb,Cr = 5 EA
	{
		uArbitraryCbValue = uArbitraryCbCrList[uArbitraryIdx][0];
		uArbitraryCrValue = uArbitraryCbCrList[uArbitraryIdx][1];

		UART_Printf("\nHit any key to test\n");
		UART_Getc();
		CAMERAT_TestPreviewPath_Complex();
	}

	eImgEffect = BYPASS;
	for (uFlipIdx=0; uFlipIdx<4; uFlipIdx++) // Flip = 4EA
	{
		eFlipDir = eFlipDirList[uFlipIdx];

		UART_Printf("\nHit any key to test\n");
		UART_Getc();
		CAMERAT_TestPreviewPath_Complex();
	}

	eImgEffect = BYPASS;		// Rotation Test 1 EA
	eFlipDir = FLIP_NO;
	eRotDeg = ROT_90;
	UART_Printf("\nHit any key to test\n");
	UART_Getc();
	CAMERAT_TestPreviewPath_Complex();

	eImgEffect = BYPASS;
	eFlipDir = FLIP_NO;
	eRotDeg = ROT_0;

}


//////////
// Function Name : CAMERAT_TestPreviewPathToFifoOut_Simple
// Function Description : camera input and fifo out test function
//                                  Preveiw->FIFO, the DstFmt = RGB24(fixed)
//                                  For Cam FIFO out, win1 is used for p_path, win2 is used for c_path
// Input : 	None
// Output :    	None

static void CAMERAT_TestPreviewPathToFifoOut_Simple(void)
{
	u32 uLcdHsz, uLcdVsz;
	u32 i,j,k;
	u32 uLcdHsz2, uLcdVsz2;
	u32 uLcdFbAddr2;

	// 1. Initialize interrupt
	//================================
	INTC_Enable(NUM_CAMIF_P);
	INTC_SetVectAddr(NUM_CAMIF_P, Isr_PreviewDone);
//	bPreviewDone = false;

	// 2. Initialize lcd
	//========================================
	#if 1 
	LCD_InitBase();
	/*
	LCD_InitDISPC(eLcdBpp, uLcdFbAddr, WIN0, false);
	LCD_GetFrmSz(&uLcdHsz, &uLcdVsz, WIN0);	
	LCD_SetWinOnOff(1, WIN0);

	GLIB_InitInstance(uLcdFbAddr, uLcdHsz, uLcdVsz, eLcdBpp);
	k=0;
	for(i=0; i<480; i++)
		for(j=0; j<800; j++)
			GLIB_PutPixel2(j, i, pSmile_3[k++]);

	LCD_GetFbEndAddr(&uLcdFbAddr2, WIN0);
	*/
	LCD_InitWin(eLcdBpp, 320, 240, 0, 0, 320, 240, 0, 0, uLcdFbAddr2, WIN1, false);
	LCD_InitWinForFifoIn2(IN_CIM, WIN1, LOCALIN_RGB, 320, 240);	
	LCD_SetWinOnOff(1, WIN1);
	LCD_Start();
	#else	
	LCD_InitDISPC(eLcdBpp, uLcdFbAddr, WIN1, false);
	LCD_InitWinForFifoIn(IN_CIM, WIN1);
	LCD_SetWinOnOff(1, WIN1);
	LCD_Start();
	#endif
	
	UART_Printf("\nHit any key to start Camera simple test!\n");
	UART_Getc();

	// 3. Initialize camera and run the process of fifo-input  fifo-output p_path
	//===============================================
	CAMERA_InitSensor();
	uLcdHsz2 = 320;
	uLcdVsz2 = 240;
	
	CAMERA_InitPreviewPathToFifoOut(uLcdHsz2, uLcdVsz2, eLcdBpp);
	CAMERA_StartPreviewPath();
	UART_Printf("\nHit any key to stop it!\n");
	UART_Getc();
	while(!bPreviewDone);

	CAMERA_StopPreviewPath();
	INTC_Disable(NUM_CAMIF_P);

	// LCD Off 
	LCD_SetAllWinOnOff(0);
	LCD_Stop();

}



//////////
// Function Name : CAMERAT_TestPreviewPathToFifoOut_Complex
// Function Description : camera input and fifo out test function
//                                 Fifo out mode doesn't support the scan line offset. No Rotation, No Flip in fifo in&fifo out mode.
// Input : 	None
// Output :    	None

static void CAMERAT_TestPreviewPathToFifoOut_Complex(void)
{
	u32 uLcdHsz, uLcdVsz;
	u32 uSrcCropStartX, uSrcCropStartY;
	u32 uLCDInputFormat;
	
	
	// 1. Initialize interrupt
	//================================
	INTC_Enable(NUM_CAMIF_P);
	INTC_SetVectAddr(NUM_CAMIF_P, Isr_PreviewDone);


	if ( eDstDataFmt == RGB16 || eDstDataFmt == RGB18 || eDstDataFmt == RGB24 )
	{
		uLCDInputFormat = LOCALIN_RGB ;
	}
 	else 
 	{
		uLCDInputFormat = LOCALIN_YCbCr;
 	}


	// 2. Initialize lcd
	//========================================
	LCD_InitDISPC(eLcdBpp, uLcdFbAddr, WIN1, false);
	LCD_GetFrmSz(&uLcdHsz, &uLcdVsz, WIN1);	
	LCD_InitWinForFifoIn1(IN_CIM, WIN1, (LOCAL_INPUT_COLORSPACE)uLCDInputFormat);	
	LCD_SetWinOnOff(1, WIN1);
	LCD_Start();

	// 3. Initialize camera and variables, run the process of fifo-input  fifo-output p_path
	//==========================================================
	uSrcCropStartX = 0, uSrcCropStartY = 0;
	CAMERA_InitSensor();
	CAMERA_InitPreviewPathToFifoOut0(uSrcCropStartX, uSrcCropStartY, uSrcCroppedHsz, uSrcCroppedVsz, uLcdHsz, uLcdVsz, eDstDataFmt);
	
	if (eImgEffect == ARBITRARY_CBCR)
		CAMERA_SetArbitraryCbCr( uArbitraryCbValue, uArbitraryCrValue);
	else
		CAMERA_SetImageEffect(eImgEffect);
	
	CAMERAT_DisplayParam();
	CAMERA_StartPreviewPath();
	UART_Printf("\nHit any key to stop it!\n");
	UART_Getc();
	CAMERA_StopPreviewPath();
	INTC_Disable(NUM_CAMIF_P);

	// LCD Off 
	LCD_SetAllWinOnOff(0);
	LCD_Stop();
	
}



//////////
// Function Name : CAMERAT_TestPreviewPathToFifoOut_AllParams
// Function Description : Camera Preview Path Test. Basic(No Flip, No output Rotator, No Winoffset, No ScnaLineOffset)
// 					  Test case = 5+4+5 +9 = 23 EA
//                                 Fifo Out mode doesn't support the scan line offset.No Rotation, No Flip in fifo in&fifo out mode.
// Input : 	None
// Output :    	None

static void CAMERAT_TestPreviewPathToFifoOut_AllParams(void)
{
	u32 uSrcCropIdx;
	u32 uImgEffectIdx;
	u32 uArbitraryIdx;
	u32 uDstIndex;
	
	// 1. Set the case group of the input parameters.
	//================================
	u32 uSrcCropSzList[5][2] = {{192,260}, {592, 400},  {176, 208}, {160, 320}, {800, 600}};
	IMAGE_EFFECT eImgEffectList[6] = {BYPASS, ARBITRARY_CBCR, NEGATIVE, ART_FREEZE, EMBOSSING, SILHOUETTE};
	u32 uArbitraryCbCrList[5][2] = {{0, 0}, {128, 128}, {255, 255}, {0, 255}, {255, 0}};
	CSPACE eDstFmtArray[9] = {YCBYCR, YCRYCB, CBYCRY, CRYCBY, YC422, YC420, RGB16,  RGB18, RGB24};

	// 2. Change the case and test all the case
	//==================================
	eDstDataFmt = RGB24;
	for (uSrcCropIdx=0; uSrcCropIdx<5; uSrcCropIdx++) // Test case = 5 EA
	{
		uSrcCroppedHsz = uSrcCropSzList[uSrcCropIdx][0];
		uSrcCroppedVsz = uSrcCropSzList[uSrcCropIdx][1];

		UART_Printf("\nHit any key to test\n");
		UART_Getc();
		CAMERAT_TestPreviewPathToFifoOut_Complex();
	}

	for (uImgEffectIdx=2; uImgEffectIdx<6; uImgEffectIdx++) // Test case = 4 EA
	{
		eImgEffect = eImgEffectList[uImgEffectIdx];

		UART_Printf("\nHit any key to test\n");
		UART_Getc();
		CAMERAT_TestPreviewPathToFifoOut_Complex();
	}

	eImgEffect = ARBITRARY_CBCR;
	
	for (uArbitraryIdx=0; uArbitraryIdx<5; uArbitraryIdx++) // Test case = 5 EA
	{
		uArbitraryCbValue = uArbitraryCbCrList[uArbitraryIdx][0];
		uArbitraryCrValue = uArbitraryCbCrList[uArbitraryIdx][1];

		UART_Printf("\nHit any key to test\n");
		UART_Getc();
		CAMERAT_TestPreviewPathToFifoOut_Complex();
	}

	eImgEffect = BYPASS;
	eRotDeg = ROT_0;
	eFlipDir = FLIP_NO;

	for ( uDstIndex = 0 ; uDstIndex < 9 ; uDstIndex++)
	{
		eDstDataFmt = eDstFmtArray[uDstIndex];
		UART_Printf("\nHit any key to test\n");
		UART_Getc();
		CAMERAT_TestPreviewPathToFifoOut_Complex();
	}

	eImgEffect = BYPASS;
	eRotDeg = ROT_0;
	eFlipDir = FLIP_NO;
}



//////////
// Function Name : CAMERAT_TestCodecPathToFifoOut_Simple
// Function Description : camera input and fifo out test function
//                                  Preveiw->FIFO, the DstFmt = RGB24(fixed)
//                                  For Cam FIFO out, win1 is used for p_path, win2 is used for c_path
// Input : 	None
// Output :    	None

static void CAMERAT_TestCodecPathToFifoOut_Simple(void)
{
	u32 uLcdHsz, uLcdVsz;


	// 1. Initialize interrupt
	//================================
	INTC_Enable(NUM_CAMIF_C);
	INTC_SetVectAddr(NUM_CAMIF_C, Isr_CodecDone);

	// 2. Initialize lcd
	//========================================
	LCD_InitDISPC(eLcdBpp, uLcdFbAddr, WIN2, false);
	LCD_GetFrmSz(&uLcdHsz, &uLcdVsz, WIN2);	
	LCD_InitWinForFifoIn1(IN_CIM, WIN2, LOCALIN_RGB);	
	LCD_SetWinOnOff(1, WIN2);
	LCD_Start();

	// 3. Initialize camera and run the process of fifo-input  fifo-output p_path
	//===============================================
	CAMERA_InitSensor();

	CAMERA_InitCodecPathToFifoOut(uLcdHsz, uLcdVsz, eLcdBpp);

	CAMERA_StartCodecPath(0);
	UART_Printf("\nHit any key to stop it!\n");
	UART_Getc();
	CAMERA_StopCodecPath();
	INTC_Disable(NUM_CAMIF_C);

	// LCD Off 
	LCD_SetWinOnOff(0, WIN2);
	LCD_Stop();

}


//////////
// Function Name : CAMERAT_TestCodecPathToFifoOut_Complex
// Function Description : camera input and fifo out test function
//                                 Fifo out mode doesn't support the scan line offset. No Rotation, No Flip in fifo in&fifo out mode.
// Input : 	None
// Output :    	None

static void CAMERAT_TestCodecPathToFifoOut_Complex(void)
{
	u32 uLcdHsz, uLcdVsz;
	u32 uSrcCropStartX, uSrcCropStartY;
	u32 uLCDInputFormat;
	
	
	// 1. Initialize interrupt
	//================================
	INTC_Enable(NUM_CAMIF_C);
	INTC_SetVectAddr(NUM_CAMIF_C, Isr_CodecDone);

	// 2. Initialize lcd
	//========================================
	if ( eDstDataFmt == RGB16 || eDstDataFmt == RGB18 || eDstDataFmt == RGB24 )
	{
		uLCDInputFormat = LOCALIN_RGB ;
	}
 	else 
 	{
		uLCDInputFormat = LOCALIN_YCbCr;
 	}	
	LCD_InitDISPC(eLcdBpp, uLcdFbAddr, WIN2, false);
	LCD_GetFrmSz(&uLcdHsz, &uLcdVsz, WIN2);	
	LCD_InitWinForFifoIn1(IN_CIM, WIN2, (LOCAL_INPUT_COLORSPACE)uLCDInputFormat);	
	LCD_SetWinOnOff(1, WIN2);
	LCD_Start();

	// 3. Initialize camera and variables, run the process of fifo-input  fifo-output p_path
	//==========================================================

	uSrcCropStartX = 0, uSrcCropStartY = 0;

	CAMERA_InitSensor();
	CAMERA_InitCodecPathToFifoOut0(uSrcCropStartX, uSrcCropStartY, uSrcCroppedHsz, uSrcCroppedVsz, uLcdHsz, uLcdVsz, eDstDataFmt);
	
	if (eImgEffect == ARBITRARY_CBCR)
		CAMERA_SetArbitraryCbCr( uArbitraryCbValue, uArbitraryCrValue);
	else
		CAMERA_SetImageEffect(eImgEffect);
	
	CAMERAT_DisplayParam();
	CAMERA_StartCodecPath(0);
	UART_Printf("\nHit any key to stop it!\n");
	UART_Getc();
	CAMERA_StopCodecPath();
	INTC_Disable(NUM_CAMIF_C);

	// LCD Off 
	LCD_SetAllWinOnOff(0);
	LCD_Stop();
	
}


//////////
// Function Name : CAMERAT_TestCodecPathToFifoOut_AllParams
// Function Description : Camera Preview Path Test. Basic(No Flip, No output Rotator, No Winoffset, No ScnaLineOffset)
// 					  Test case = 5+4+5+9 = 23 EA
//                                 Fifo Out mode doesn't support the scan line offset.No Rotation, No Flip in fifo in&fifo out mode.
// Input : 	None
// Output :    	None

static void CAMERAT_TestCodecPathToFifoOut_AllParams(void)
{
	u32 uSrcCropIdx;
	u32 uImgEffectIdx;
	u32 uArbitraryIdx;
	u32 uDstIndex;

	// 1. Set the case group of the input parameters.
	//================================
	u32 uSrcCropSzList[5][2] = {{320,240}, {560, 400},  {240, 160}, {160, 320}, {800, 600} };
	IMAGE_EFFECT eImgEffectList[6] = {BYPASS, ARBITRARY_CBCR, NEGATIVE, ART_FREEZE, EMBOSSING, SILHOUETTE};
	u32 uArbitraryCbCrList[5][2] = {{0, 0}, {128, 128}, {255, 255}, {0, 255}, {255, 0}};
	CSPACE eDstFmtArray[9] = {YCBYCR, YCRYCB, CBYCRY, CRYCBY, YC422, YC420, RGB16,  RGB18, RGB24};

	// 3. Change the case and test all the case
	//==================================
	eDstDataFmt = RGB24;

	for (uSrcCropIdx=0; uSrcCropIdx<5; uSrcCropIdx++) // Test case = 5 EA
	{
		uSrcCroppedHsz = uSrcCropSzList[uSrcCropIdx][0];
		uSrcCroppedVsz = uSrcCropSzList[uSrcCropIdx][1];

		UART_Printf("\nHit any key to test\n");
		UART_Getc();
		CAMERAT_TestCodecPathToFifoOut_Complex();
	}

	for (uImgEffectIdx=2; uImgEffectIdx<6; uImgEffectIdx++) // Test case = 4 EA
	{
		eImgEffect = eImgEffectList[uImgEffectIdx];

		UART_Printf("\nHit any key to test\n");
		UART_Getc();
		CAMERAT_TestCodecPathToFifoOut_Complex();
	}

	eImgEffect = ARBITRARY_CBCR;
	
	for (uArbitraryIdx=0; uArbitraryIdx<5; uArbitraryIdx++) // Test case = 5 EA
	{
		uArbitraryCbValue = uArbitraryCbCrList[uArbitraryIdx][0];
		uArbitraryCrValue = uArbitraryCbCrList[uArbitraryIdx][1];

		UART_Printf("\nHit any key to test\n");
		UART_Getc();
		CAMERAT_TestCodecPathToFifoOut_Complex();
	}

	eImgEffect = BYPASS;
	eRotDeg = ROT_0;
	eFlipDir = FLIP_NO;

	for ( uDstIndex = 0 ; uDstIndex < 9 ; uDstIndex++)
	{
		eDstDataFmt = eDstFmtArray[uDstIndex];
		UART_Printf("\nHit any key to test\n");
		UART_Getc();
		CAMERAT_TestCodecPathToFifoOut_Complex();
	}

	eImgEffect = BYPASS;
	eRotDeg = ROT_0;
	eFlipDir = FLIP_NO;
	
}




//////////
// Function Name : CAMERAT_TestCodecPath_Simple
// Function Description : Codec Path Simple Test
// Input : 	None
// Output :    	None
static void CAMERAT_TestCodecPath_Simple(void)
{
	u32 uLcdHsz, uLcdVsz;
	u32 i,j,k;

	// 1. Initialize interrupt 
	//=================================
	INTC_Enable(NUM_CAMIF_C);
	INTC_SetVectAddr(NUM_CAMIF_C, Isr_CodecDone);
	bCodecDone = false;

	// 2. Initialize lcd
	//==================================
	*(volatile unsigned *)0x7410800c=0x0; // Modem I/F Bypass Control. It should be 0,
	LCD_InitDISPC(eLcdBpp, uLcdFbAddr, WIN0, false);
	LCD_GetFrmSz(&uLcdHsz, &uLcdVsz, WIN0);	
	LCD_SetWinOnOff(1, WIN0);
	GLIB_InitInstance(uLcdFbAddr, uLcdHsz, uLcdVsz, eLcdBpp);
	k=0;
	for(i=0; i<480; i++)
		for(j=0; j<800; j++)
			GLIB_PutPixel2(j, i, pSmile_2[k++]);

	LCD_Start();
	UART_Printf("\nHit any key to start Camera simple test!\n");
	UART_Getc();
	// 3. Initialize camera and run the process of codec path
	//==================================================
	CAMERA_InitSensor();
	CAMERA_InitCodecPath(uLcdHsz, uLcdVsz, uLcdFbAddr, eLcdBpp, FLIP_NO, ROT_0);

	CAMERA_StartCodecPath(0);
	UART_Printf("\nHit any key to capture frame!\n");
	UART_Getc();
	while(!bCodecDone);
	CAMERA_StopCodecPath();
	INTC_Disable(NUM_CAMIF_C);

	// LCD Off
	LCD_SetAllWinOnOff(0);
	LCD_Stop();
	UART_Printf("\nLCD OFF\n");		
}



//////////
// Function Name : CAMERAT_TestCodecPath_Complex
// Function Description : CodecPath Complex Test Function(ScanLine Offset)
// Input : 	None
// Output :    	None
static void CAMERAT_TestCodecPath_Complex(void)
{
	u32 uLcdHsz, uLcdVsz;
	u32 uDstHsz, uDstVsz;
	u32 uDisplayStartX, uDisplayStartY;
	u32 uSrcCropStartX, uSrcCropStartY;

	
	// 1. Initialize interrupt 
	//=================================
	INTC_Enable(NUM_CAMIF_C);
	INTC_SetVectAddr(NUM_CAMIF_C, Isr_CodecDone);
	bCodecDone = false;

	// 2. Initialize lcd
	//==================================
	LCD_InitDISPC(eLcdBpp, uLcdFbAddr, WIN0, false);
	LCD_GetFrmSz(&uLcdHsz, &uLcdVsz, WIN0);	
	LCD_SetWinOnOff(1, WIN0);
	LCD_Start();

	// 3. Initialize camera and run the process of codec path
	//==================================================
 	uDstHsz = 160, uDstVsz = 120;
	uSrcCropStartX = 0, uSrcCropStartY = 0;
	uDisplayStartX = 80, uDisplayStartY = 60;

	eRotDeg = ROT_0;
	CAMERA_InitSensor();
	CAMERA_InitCodecPath0(uSrcCropStartX, uSrcCropStartY, uSrcCroppedHsz, uSrcCroppedVsz,
		uLcdHsz, uLcdVsz, uDisplayStartX, uDisplayStartY, uDstHsz, uDstVsz, uLcdFbAddr, uLcdFbAddr, eLcdBpp, eFlipDir, eRotDeg);

	if (eImgEffect == ARBITRARY_CBCR)
		CAMERA_SetArbitraryCbCr( uArbitraryCbValue, uArbitraryCrValue);
	else
		CAMERA_SetImageEffect(eImgEffect);
	CAMERAT_DisplayParam();
	CAMERA_StartCodecPath(0);
	UART_Printf("\nHit any key to capture frame!\n");
	UART_Getc();
	while(!bCodecDone);
	CAMERA_StopCodecPath();
	INTC_Disable(NUM_CAMIF_C);

	// LCD Off
	LCD_SetAllWinOnOff(0);
	LCD_Stop();
	UART_Printf("\nLCD OFF\n");		

}


//////////
// Function Name : CAMERAT_TestCodecPath_AllParams
// Function Description : Camera Codec Path Test. 
//                                 Test case = 5+4+5+4 = 18 EA
// Input : 	None
// Output :    	None
static void CAMERAT_TestCodecPath_AllParams(void)
{
	// 1. Set the case of the input parameters.
	//================================
	u32 uSrcCropSzList[5][2] = {{192,240}, {600, 448},  {160, 200}, {176, 360}, {800, 600}};
	IMAGE_EFFECT eImgEffectList[6] = {BYPASS, ARBITRARY_CBCR, NEGATIVE, ART_FREEZE, EMBOSSING, SILHOUETTE};
	u32 uArbitraryCbCrList[5][2] = {{0, 0}, {128, 128}, {255, 255}, {0, 255}, {255, 0}};
	FLIP_DIR eFlipDirList[4] = {FLIP_X, FLIP_Y, FLIP_XY, FLIP_NO};
	u32 uSrcCropIdx;
	u32 uImgEffectIdx;
	u32 uArbitraryIdx;
	u32 uFlipIdx;

	// 2. Change the case and test all the case of codec path.
	//=============================================
	for (uSrcCropIdx=0; uSrcCropIdx<5; uSrcCropIdx++) // Scaler up,down, bypass = 5 EA (2*2 + 1)
	{
		uSrcCroppedHsz = uSrcCropSzList[uSrcCropIdx][0];
		uSrcCroppedVsz = uSrcCropSzList[uSrcCropIdx][1];

		UART_Printf("\nHit any key to test\n");
		Getc();
		CAMERAT_TestCodecPath_Complex();
	}

 	for (uImgEffectIdx=2; uImgEffectIdx<6; uImgEffectIdx++) // Image effect = 4 EA
	{
		eImgEffect = eImgEffectList[uImgEffectIdx];

		UART_Printf("\nHit any key to test\n");
		Getc();
		CAMERAT_TestCodecPath_Complex();
	}

 	eImgEffect = ARBITRARY_CBCR;
	for (uArbitraryIdx=0; uArbitraryIdx<5; uArbitraryIdx++) // Arbitrary Cb,Cr = 5 EA
	{
		uArbitraryCbValue = uArbitraryCbCrList[uArbitraryIdx][0];
		uArbitraryCrValue = uArbitraryCbCrList[uArbitraryIdx][1];

		UART_Printf("\nHit any key to test\n");
		Getc();
		CAMERAT_TestCodecPath_Complex();
	}

	eImgEffect = BYPASS;
	for (uFlipIdx=0; uFlipIdx<4; uFlipIdx++) // Flip = 4 EA
	{
		eFlipDir = eFlipDirList[uFlipIdx];

		UART_Printf("\nHit any key to test\n");
		Getc();
		CAMERAT_TestCodecPath_Complex();
	}
	eImgEffect = BYPASS;
	eFlipDir = FLIP_NO;
	eRotDeg = ROT_0;

}


//////////
// Function Name : CAMERAT_TestMultiCaptureOfCodecPath
// Function Description : Capture Count Function Test
// Input : 	None
// Output :    	None
static void CAMERAT_TestMultiCaptureOfCodecPath(void)
{
	u32 uLcdHsz, uLcdVsz;
	u32 uCapNum;
	u32 uBufCnt;
	
	// 0. Interrupt Setting 
	//=====================================
	INTC_Enable(NUM_CAMIF_C);
	INTC_SetVectAddr(NUM_CAMIF_C, Isr_CodecDone);	
	bCodecDone = false;

	// 1. Initialize lcd
	//=====================================
	LCD_InitDISPC(eLcdBpp, uLcdFbAddr, WIN0, false);
	LCD_GetFrmSz(&uLcdHsz, &uLcdVsz, WIN0);	
	LCD_SetWinOnOff(1, WIN0);
	LCD_Start();

	// 2. Set the process mode(interrupt  or polling mode) and captured pic number
	//================================================
	UART_Printf("\nHow many pictures ? : ");
	uCapNum = UART_GetIntNum();

	// 3. Initialize camera and run the process of codec path
	//===================================
	uBufCnt = 0;
 	CAMERA_InitSensor();
	CAMERA_InitCodecPath(uLcdHsz, uLcdVsz, uLcdFbAddr, eLcdBpp, FLIP_NO, ROT_0);
//	CAMERA_InitCodecPath(800, 600, uLcdFbAddr, YC420, FLIP_NO, ROT_0);	
	UART_Printf("\nCapture count = %d. \n", uCapNum);
	CAMERA_StartCodecPath(uCapNum);

	if (uCapNum==0)
	{
		UART_Printf("Hit any key to capture it!\n");
		while(!bCodecDone);
		bCodecDone = false;
		while(GetKey() == 0);
		CAMERA_StopCodecPath();
	}
	else
	{
		while(1)
		{	
			while(!bCodecDone);
			bCodecDone = false;
			uBufCnt++;
			if (uBufCnt==uCapNum)
				break;
		}
		CAMERA_StopCodecPath();
	}

	INTC_Disable(NUM_CAMIF_C);

#if 0 
	// LCD Off
	LCD_SetAllWinOnOff(0);
	LCD_Stop();
	UART_Printf("\nLCD OFF\n");	
#endif

}

//////////
// Function Name : CAMERAT_TestMultiCaptureOfPreviewPath
// Function Description : Capture Count Function Test(Preview Path)
// Input : 	None
// Output :    	None
static void CAMERAT_TestMultiCaptureOfPreviewPath(void)
{
	u32 uLcdHsz, uLcdVsz;
	u32 uCapNum;
	u32 uBufCnt;
	
	// 0. Interrupt Setting 
	//=====================================
	INTC_Enable(NUM_CAMIF_P);
	INTC_SetVectAddr(NUM_CAMIF_P, Isr_PreviewDone);	
	bPreviewDone = false;

	// 1. Initialize lcd
	//=====================================
	LCD_InitDISPC(eLcdBpp, uLcdFbAddr, WIN0, false);
	LCD_GetFrmSz(&uLcdHsz, &uLcdVsz, WIN0);	
	LCD_SetWinOnOff(1, WIN0);
	LCD_Start();

	// 2. Set the process mode(interrupt  or polling mode) and captured pic number
	//================================================
	UART_Printf("\nHow many pictures ? : ");
	uCapNum = UART_GetIntNum();

	// 3. Initialize camera and run the process of codec path
	//===================================
	uBufCnt = 0;
 	CAMERA_InitSensor();
	CAMERA_InitPreviewPath(uLcdHsz, uLcdVsz, uLcdFbAddr, eLcdBpp, FLIP_NO, ROT_0);
	UART_Printf("\nCapture count = %d. \n", uCapNum);
	CAMERA_StartPreviewPathCPT(uCapNum);


	if (uCapNum==0)
	{
		UART_Printf("Hit any key to capture it!\n");
		while(!bPreviewDone);
		bPreviewDone = false;
		while(GetKey() == 0);
		CAMERA_StopPreviewPath();
	}
	else
	{
		while(1)
		{	
			while(!bPreviewDone);
			bPreviewDone = false;
			uBufCnt++;
			if (uBufCnt==uCapNum)
				break;
		}
		CAMERA_StopPreviewPath();
	}
	INTC_Disable(NUM_CAMIF_P);

#if 0 
	// LCD Off
	LCD_SetAllWinOnOff(0);
	LCD_Stop();
	UART_Printf("\nLCD OFF\n");	
#endif	

}

//////////
// Function Name : CAMERAT_TestDmaInPath_PreviewPath_Simple
// Function Description : MSMDA Path Basic Test In Preview Path
// Input : 	None
// Output :    	None
static void CAMERAT_TestDmaInPath_PreviewPath_Simple(void)
{
	u32 uLcdHsz, uLcdVsz;
	u32 uWIN0FbEndAddr;
	u32 uLcdHsz2, uLcdVsz2;	

	// 1. Initialize interrupt
	//==============================
	INTC_Enable(NUM_CAMIF_P);
	INTC_SetVectAddr(NUM_CAMIF_P, Isr_MSDMAPreviewDone);
	bPreviewDone = false;

	uLcdHsz2 = 320;
	uLcdVsz2 = 240;
	// 2. Initialize lcd
	//================================
//	LCD_InitDISPC(eLcdBpp, uLcdFbAddr, WIN0, false);
	LCD_InitBase();
	LCD_InitWin(eLcdBpp, uLcdHsz2, uLcdVsz2, 0, 0, uLcdHsz2, uLcdVsz2, 0, 0, uLcdFbAddr, WIN0, false);
//	LCD_InitDISPC(uLcdHsz2, uLcdVsz2, eLcdBpp, uLcdFbAddr, WIN0, false);
	LCD_GetFrmSz(&uLcdHsz, &uLcdVsz, WIN0);	
	LCD_GetFbEndAddr(&uWIN0FbEndAddr, WIN0);
	LCD_SetWinOnOff(1, WIN0);
	LCD_Start();

	uLcdHsz = 800;
	uLcdVsz = 400;
	// 3. Create the input picture 
	//=================================
	UART_Printf("\nDma input -> dma output in p_path\n");
	GLIB_InitInstance(uWIN0FbEndAddr, uLcdHsz, uLcdVsz, eLcdBpp);
	GLIB_DrawPattern(uLcdHsz, uLcdVsz);

	// 4. Run the process of dma-input path
	//=============================
	CAMERA_InitDmaInPath(uLcdHsz, uLcdVsz, uWIN0FbEndAddr, eLcdBpp, uLcdHsz2, uLcdVsz2, uLcdFbAddr, eLcdBpp, FLIP_NO, ROT_0, P_PATH);

	CAMERAT_DisplayParam();
	CAMERA_StartDmaInPath();
	UART_Printf("\nCheck Iamge and Get Any Key!\n");
	UART_Getc();
	while(!bPreviewDone);

	CAMERA_WaitMSDMAP();	
	CAMERA_StopDmaInPath();

	INTC_Disable(NUM_CAMIF_P);

	// LCD Off
	LCD_SetAllWinOnOff(0);
	LCD_Stop();
	UART_Printf("\nLCD OFF\n");	
}


//////////
// Function Name : CAMERAT_TestDmaInPath_CodecPath_Simple
// Function Description : MSDMA Path Basic Test In Codec Path
// Input : 	None
// Output :    	None
static void CAMERAT_TestDmaInPath_CodecPath_Simple(void)
{
	u32 uLcdHsz, uLcdVsz;
	u32 uWIN0FbEndAddr;

	// 1. Initialize interrupt
	//==============================
	INTC_Enable(NUM_CAMIF_C);
	INTC_SetVectAddr(NUM_CAMIF_C, Isr_MSDMACodecDone);
	bCodecDone = false;

	// 2. Initialize lcd
	//================================
	LCD_InitDISPC(eLcdBpp, uLcdFbAddr, WIN0, false);
	LCD_GetFrmSz(&uLcdHsz, &uLcdVsz, WIN0);	
	LCD_GetFbEndAddr(&uWIN0FbEndAddr, WIN0);
	LCD_SetWinOnOff(1, WIN0);
	LCD_Start();

	// 2. Create the input picture 
	//=================================
	UART_Printf("Dma input -> dma output in c_path\n");
	GLIB_InitInstance(uWIN0FbEndAddr, uLcdHsz, uLcdVsz, eLcdBpp);
	GLIB_DrawPattern(uLcdHsz, uLcdVsz);

	// 4. Run the process of dma-input path
	//=============================
	CAMERA_InitDmaInPath(uLcdHsz, uLcdVsz, uWIN0FbEndAddr, eLcdBpp, uLcdHsz, uLcdVsz, uLcdFbAddr, eLcdBpp, FLIP_NO, ROT_0, C_PATH);
	CAMERAT_DisplayParam();
	CAMERA_StartDmaInPath();
	UART_Printf("\nCheck Image and stop Test Get any Key!\n");
	UART_Getc();
	while(!bCodecDone);
	CAMERA_WaitMSDMAC();		
	CAMERA_StopDmaInPath();

	INTC_Disable(NUM_CAMIF_C);
	// LCD Off
	LCD_SetAllWinOnOff(0);
	LCD_Stop();
	UART_Printf("\nLCD OFF\n");		

}


//////////
// Function Name : CAMERAT_TestDmaInPath_Complex
// Function Description : DMA in (C/P Path) DMA out (C/P Path)
// Input : 	None
// Output :    	None
static void CAMERAT_TestDmaInPath_Complex(void)
{
	u32 uLcdHsz, uLcdVsz;
	u32 uWIN0FbEndAddr;
	u32 uSrcAddr;
	u32 uDstHsz, uDstVsz;
	u32 uDisplayStartX, uDisplayStartY;
	u32 uSrcCropStartX, uSrcCropStartY;
	
    // 1. Initialize interrupt
    //==============================
	INTC_Enable(NUM_CAMIF_P);
	INTC_Enable(NUM_CAMIF_C);
	INTC_SetVectAddr(NUM_CAMIF_P, Isr_MSDMAPreviewDone);
	INTC_SetVectAddr(NUM_CAMIF_C, Isr_MSDMACodecDone);
	bPreviewDone = false;
	bCodecDone = false;

	// 2. Initialize lcd
	//================================
	LCD_InitDISPC(eLcdBpp, uLcdFbAddr, WIN0, false);
	LCD_GetFrmSz(&uLcdHsz, &uLcdVsz, WIN0);	
	LCD_GetFbEndAddr(&uWIN0FbEndAddr, WIN0);
	LCD_SetWinOnOff(1, WIN0);
	LCD_Start();

	// 3. Create the input picture 
	//=================================
	uSrcAddr = uWIN0FbEndAddr+uLcdHsz*uLcdVsz*4;
	GLIB_InitInstance(uWIN0FbEndAddr, uLcdHsz, uLcdVsz, eLcdBpp);
	GLIB_DrawPattern(uLcdHsz, uLcdVsz);

	CAMERA_InitDmaInPath(uLcdHsz, uLcdVsz, uWIN0FbEndAddr, eLcdBpp,
		uSrcWidth, uSrcHeight, uSrcAddr, eSrcDataFmt, FLIP_NO, ROT_0, C_PATH);
//	CAMERA_InitDmaInPath(uLcdHsz, uLcdVsz, uWIN0FbEndAddr, eLcdBpp,
//		uSrcWidth, uSrcHeight, uSrcAddr, eSrcDataFmt, FLIP_NO, ROT_0, P_PATH);

	CAMERA_StartDmaInPath();
	while(!bCodecDone);
	bCodecDone = false;
	CAMERA_WaitMSDMAC();	
//	while(!bPreviewDone);
//	bPreviewDone = false;
//	CAMERA_WaitMSDMAP();	

	CAMERA_StopDmaInPath();


	// 4. Run the process of dma-input path
	//=============================
	eDstDataFmt = eLcdBpp;
//	uCodecBufAddr = uSrcAddr+uSrcWidth*uSrcHeight*4;
	uDstHsz = 160, uDstVsz = 120;
	uDisplayStartX = 80,	uDisplayStartY = 60;
	uSrcCropStartX = 0,	uSrcCropStartY = 0;

//	eRotDeg = ROT_0;
	CAMERA_InitDmaInPath0(uSrcWidth, uSrcHeight, uSrcCropStartX, uSrcCropStartY, uSrcCroppedHsz, uSrcCroppedVsz, uSrcAddr, uSrcAddr,eSrcDataFmt,
		uLcdHsz, uLcdVsz, uDisplayStartX, uDisplayStartY, uDstHsz, uDstVsz, uLcdFbAddr,uLcdFbAddr, eDstDataFmt, eFlipDir, eRotDeg, eProcessPath);

	if (eImgEffect == ARBITRARY_CBCR)
	{
		CAMERA_SetImageEffect(eImgEffect);
		CAMERA_SetArbitraryCbCr( uArbitraryCbValue, uArbitraryCrValue);
	}
	else
		CAMERA_SetImageEffect(eImgEffect);

	CAMERAT_DisplayParam();

	CAMERA_StartDmaInPath();

	if (eProcessPath==P_PATH)
	{
		while(!bPreviewDone);
		CAMERA_WaitMSDMAP();	
	}
	else
	{
		while(!bCodecDone);
		CAMERA_WaitMSDMAC();	
	}

	CAMERA_StopDmaInPath();

	INTC_Disable(NUM_CAMIF_C);
	INTC_Disable(NUM_CAMIF_P);
	
}


//////////
// Function Name : CAMERAT_TestDmaInPath_AllParams
// Function Description : DMA In / DMA Out Test Function 5+4+5+4+1+9 = 28 EA Test
// Input : 	None
// Output :    	None
static void CAMERAT_TestDmaInPath_AllParams(void)
{
	int nI;
	int nSrcCropIdx;
	int nImgEffectIdx;
	int nArbitraryIdx;
//	int nDstIdx;
	int nSrcIdx;
	u32 uFlipIdx;
	
	// 1. Set the case of the input parameters.
	//================================
	u32 uSrcCropSzList[5][2] = {{144, 160}, {112, 176},  {80, 144}, {176, 192}, {640, 480}};
	IMAGE_EFFECT eImgEffectList[6] = {BYPASS, ARBITRARY_CBCR, NEGATIVE, ART_FREEZE, EMBOSSING, SILHOUETTE};
	u32 uArbitraryCbCrList[5][2] = {{0, 0}, {128, 128}, {255, 255}, {0, 255}, {255, 0}};
	FLIP_DIR eFlipDirList[5] = { FLIP_NO, FLIP_X, FLIP_Y, FLIP_XY, FLIP_NO};
	CSPACE eSrcFmtArray[9] = {YCBYCR, YCRYCB, CBYCRY, CRYCBY,  YC422, YC420, RGB16, RGB18, RGB24 };

	// 2. Change the case and test all the case
	//=================================
	eInputPath = DMA;
	for (nI=0; nI<2; nI++)
	{
		eProcessPath = (nI==0) ? P_PATH : C_PATH;
		eRotDeg = ROT_0;
		eImgEffect = BYPASS;
		eFlipDir = FLIP_NO;
		
		UART_Printf("\n============================================================");
		UART_Printf("\n=================> Process Path  0(Preview) 1(Codec) =%d\n", eProcessPath);			
		UART_Printf("============================================================\n");
		
		for (nSrcCropIdx=0; nSrcCropIdx<5; nSrcCropIdx++) // Scaler up,down, bypass = 5 EA (2*2 + 1)
		{
			uSrcCroppedHsz = uSrcCropSzList[nSrcCropIdx][0];
			uSrcCroppedVsz = uSrcCropSzList[nSrcCropIdx][1];

			UART_Printf("\nHit any key to test\n");
			UART_Getc();
			// LCD Off
			LCD_SetAllWinOnOff(0);
			LCD_Stop();
			UART_Printf("\nLCD OFF\n");	
			CAMERAT_TestDmaInPath_Complex();
		}
		
		for (nImgEffectIdx=2; nImgEffectIdx<6; nImgEffectIdx++) // Image effect = 4 EA
		{
			eImgEffect = eImgEffectList[nImgEffectIdx];

			UART_Printf("\nHit any key to test\n");
			UART_Getc();
			// LCD Off
			LCD_SetAllWinOnOff(0);
			LCD_Stop();
			UART_Printf("\nLCD OFF\n");	
			
			CAMERAT_TestDmaInPath_Complex();
		}
		
		eImgEffect = ARBITRARY_CBCR;
		for (nArbitraryIdx=0; nArbitraryIdx<5; nArbitraryIdx++) // Arbitrary Cb,Cr = 5 EA
		{
			uArbitraryCbValue = uArbitraryCbCrList[nArbitraryIdx][0];
			uArbitraryCrValue = uArbitraryCbCrList[nArbitraryIdx][1];

			UART_Printf("\nHit any key to test\n");
			UART_Getc();
			// LCD Off
			LCD_SetAllWinOnOff(0);
			LCD_Stop();
			UART_Printf("\nLCD OFF\n");	
			
			CAMERAT_TestDmaInPath_Complex();
		}

		eImgEffect = BYPASS;
		eRotDeg = ROT_0 ; 
		eSrcDataFmt = RGB16;
				
		//eSrcDataFmt = eSrcFmtArray[8-nDstIdx];
		for ( uFlipIdx=0; uFlipIdx<5; uFlipIdx++)
		{
			eFlipDir = eFlipDirList[uFlipIdx];
			UART_Printf("\nHit any key to test\n");
			UART_Getc();
			// LCD Off
			LCD_SetAllWinOnOff(0);
			LCD_Stop();
			UART_Printf("\nLCD OFF\n");	
			
			CAMERAT_TestDmaInPath_Complex();
		}

		eImgEffect = BYPASS;
		eFlipDir = FLIP_NO;

		if ( eProcessPath == P_PATH )
		{
			eRotDeg = ROT_90;
			UART_Printf("\nHit any key to tets\n");
			UART_Getc();
			// LCD Off
			LCD_SetAllWinOnOff(0);
			LCD_Stop();
			UART_Printf("\nLCD OFF\n");	
			
			CAMERAT_TestDmaInPath_Complex();
		}

		eImgEffect = BYPASS;
		eFlipDir = FLIP_NO;
		eRotDeg = ROT_0 ; 
		
		for ( nSrcIdx=0; nSrcIdx<9; nSrcIdx++)
		{
			eSrcDataFmt = eSrcFmtArray[nSrcIdx];
			UART_Printf("\nHit any key to test\n");
			UART_Getc();
			// LCD Off
			LCD_SetAllWinOnOff(0);
			LCD_Stop();
			UART_Printf("\nLCD OFF\n");	
			
			CAMERAT_TestDmaInPath_Complex();
		}

		eImgEffect = BYPASS;
		eFlipDir = FLIP_NO;
		eRotDeg = ROT_0; 
	}
	
}




//////////
// Function Name : CAMERAT_TestDmaInPathToFifoOut_Simple
// Function Description : MSMDA In Path and FIFO Out Path Basic Test. (Preview / Codec Path)
// Input : 	None
// Output :    	None
static void CAMERAT_TestDmaInPathToFifoOut_Simple(void)
{
	u32 uLcdHsz, uLcdVsz;
	u32 uWIN1FbEndAddr;
	u32 i,j,k;
#if 0 // Preview Path Test
	// 0. Interrupt Setting : For MSDMA.....
	//================================	
	INTC_Enable(NUM_CAMIF_P);
	INTC_SetVectAddr(NUM_CAMIF_P, Isr_MSDMAPreviewDone);
	bPreviewDone = false;

	// 1. Initialize lcd
	//================================
	LCD_InitDISPC(eLcdBpp, uLcdFbAddr, WIN1, false);
	LCD_GetFrmSz(&uLcdHsz, &uLcdVsz, WIN1);
	LCD_GetFbEndAddr(&uWIN1FbEndAddr, WIN1);	
	LCD_InitWinForFifoIn(IN_CIM, WIN1);
	LCD_SetWinOnOff(1, WIN1);
	LCD_Start();

	// 2. Create the input picture 
	//=================================
	UART_Printf("Dma Input -> dma output in Preview_path\n");
	GLIB_InitInstance(uWIN1FbEndAddr, uLcdHsz, uLcdVsz, eLcdBpp);
	#if 0
	GLIB_DrawPattern(uLcdHsz, uLcdVsz);
	#else
		k=0;
	for(i=0; i<240; i++)
		for(j=0; j<320; j++)
			GLIB_PutPixel2(j, i, pSmile_3[k++]);
	#endif

	// 3. Run the process of dma-input fifo-output path
	//=======================================
	eDstDataFmt = RGB24;
	CAMERA_InitDmaInPathToFifoOut(uLcdHsz, uLcdVsz, uWIN1FbEndAddr, eLcdBpp,
		uLcdHsz, uLcdVsz, eLcdBpp, ROT_0, P_PATH);

	uSrcCroppedHsz = uLcdHsz;
	uSrcCroppedVsz = uLcdVsz;
	uSrcHeight = uLcdVsz;
	uSrcWidth = uLcdHsz ; 
	
	CAMERAT_DisplayParam();
	CAMERA_StartDmaInPath();
	UART_Printf("\nHit any key to stop the test!\n");
	UART_Getc();
	while(!bPreviewDone);
	CAMERA_WaitMSDMAP();
	CAMERA_StopDmaInPath();
	INTC_Disable(NUM_CAMIF_P);

	//LCD Off
	//LCD_SetAllWinOnOff(0);
	LCD_SetWinOnOff(0, WIN1);
	LCD_Stop();
	UART_Printf("\nLCD OFF\n");	
#else // Codec Path Test
	// 0. Interrupt Setting : For MSDMA.....
	//================================	
	INTC_Enable(NUM_CAMIF_C);
	INTC_SetVectAddr(NUM_CAMIF_C, Isr_MSDMACodecDone);
	bCodecDone = false;

	// 1. Initialize lcd
	//================================
	LCD_InitDISPC(eLcdBpp, uLcdFbAddr, WIN2, false);
	LCD_GetFrmSz(&uLcdHsz, &uLcdVsz, WIN2);
	LCD_GetFbEndAddr(&uWIN1FbEndAddr, WIN2);	
	LCD_InitWinForFifoIn(IN_CIM, WIN2);
	LCD_SetWinOnOff(1, WIN2);
	LCD_Start();

	// 2. Create the input picture 
	//=================================
	UART_Printf("Dma input -> dma output in Codec_path\n");
	GLIB_InitInstance(uWIN1FbEndAddr, uLcdHsz, uLcdVsz, eLcdBpp);
	#if 0
	GLIB_DrawPattern(uLcdHsz, uLcdVsz);
	#else
		k=0;
	for(i=0; i<480; i++)
		for(j=0; j<800; j++)
			GLIB_PutPixel2(j, i, pSmile_2[k++]);
	#endif

	// 3. Run the process of dma-input fifo-output path
	//=======================================
	eDstDataFmt = RGB24;
	CAMERA_InitDmaInPathToFifoOut(uLcdHsz, uLcdVsz, uWIN1FbEndAddr, eLcdBpp,
		uLcdHsz, uLcdVsz, eLcdBpp, ROT_0, C_PATH);

	uSrcCroppedHsz = uLcdHsz;
	uSrcCroppedVsz = uLcdVsz;
	uSrcHeight = uLcdVsz;
	uSrcWidth = uLcdHsz ; 
	CAMERAT_DisplayParam();
	CAMERA_StartDmaInPath();
	UART_Printf(" Hit any key to stop the test!\n");
	UART_Getc();
	while(!bCodecDone);
	CAMERA_WaitMSDMAC();
	CAMERA_StopDmaInPath();
	INTC_Disable(NUM_CAMIF_C);

	//LCD Off
	//LCD_SetAllWinOnOff(0);
	LCD_SetWinOnOff(0, WIN2);
	LCD_Stop();
	UART_Printf("\nLCD OFF\n");	
#endif
}


//////////
// Function Name : CAMERAT_TestDmaInPathToFifoOut_Complex
// Function Description : 
//                                            Preview path: win1, Codec path: win2
// Input : 	None
// Output :    	None
static void CAMERAT_TestDmaInPathToFifoOut_Complex(void)
{

	u32 uLcdHsz, uLcdVsz;
	u32 uSrcAddr;
	u32 uSrcCropStartX, uSrcCropStartY;
	LCD_WINDOW eWin;
	u32 uLCDInputFormat;

	// 0. Interrupt Setting
	//================================
	INTC_Enable(NUM_CAMIF_P);
	INTC_SetVectAddr(NUM_CAMIF_P, Isr_MSDMAPreviewDone);
	INTC_Enable(NUM_CAMIF_C);
	INTC_SetVectAddr(NUM_CAMIF_C, Isr_MSDMACodecDone);
	bCodecDone = false;
	bPreviewDone = false;
	

	// 1. Initialize lcd
	//================================
   	eWin = (eProcessPath==P_PATH) ? WIN1: WIN2;

	if ( eDstDataFmt == RGB16 || eDstDataFmt == RGB18 || eDstDataFmt == RGB24 )
	{
		uLCDInputFormat = LOCALIN_RGB ;
	}
 	else 
 	{
		uLCDInputFormat = LOCALIN_YCbCr;
 	}
	LCD_InitDISPC(eLcdBpp, uLcdFbAddr, eWin, false);
	LCD_GetFrmSz(&uLcdHsz, &uLcdVsz, eWin);	
//	LCD_GetFbEndAddr(&uWIN1FbEndAddr, WIN1);
	LCD_InitWinForFifoIn1(IN_CIM, eWin, (LOCAL_INPUT_COLORSPACE)uLCDInputFormat);	
	LCD_SetWinOnOff(1, eWin);
	LCD_Start();


	// 2. Create the input picture 
	//=================================
	uSrcAddr = uLcdFbAddr+uLcdHsz*uLcdVsz*4;
	GLIB_InitInstance(uLcdFbAddr, uLcdHsz, uLcdVsz, eLcdBpp);
	GLIB_DrawPattern(uLcdHsz, uLcdVsz);

	CAMERA_InitDmaInPath(uLcdHsz, uLcdVsz, uLcdFbAddr, eLcdBpp, uSrcWidth, uSrcHeight, uSrcAddr, eSrcDataFmt, FLIP_NO, ROT_0, P_PATH);
	CAMERA_StartDmaInPath();
	while(!CAMERA_IsProcessingDone());
	CAMERA_WaitMSDMAP();
	CAMERA_StopDmaInPath();

	// 3. Run the fimc's process of dma-input fifo-output path
	//===================================

	uSrcCropStartX = 0, uSrcCropStartY = 0;

	eRotDeg = ROT_0;

	CAMERA_InitDmaInPathToFifoOut0(uSrcWidth, uSrcHeight, uSrcCropStartX, uSrcCropStartY, uSrcCroppedHsz, uSrcCroppedVsz,
		uSrcAddr, uSrcAddr,eSrcDataFmt, uLcdHsz, uLcdVsz, eDstDataFmt,  eRotDeg, eProcessPath);

	if (eImgEffect == ARBITRARY_CBCR)
	{
		CAMERA_SetImageEffect(eImgEffect);
		CAMERA_SetArbitraryCbCr( uArbitraryCbValue, uArbitraryCrValue);
	}
	else
		CAMERA_SetImageEffect(eImgEffect);
	
	CAMERAT_DisplayParam();
	
	CAMERA_StartDmaInPath();
	UART_Printf("\nHit any key to stop the test!\n");

	UART_Getc();

	if ( eProcessPath == P_PATH)
	{
		while(!bPreviewDone);
		CAMERA_WaitMSDMAP();	
	}
	else
	{
		while(!bCodecDone);
		CAMERA_WaitMSDMAC();	
	}

	CAMERA_StopDmaInPath();
	INTC_Disable(NUM_CAMIF_P);
	INTC_Disable(NUM_CAMIF_C);
	
}


//////////
// Function Name : CAMERAT_TestDmaInPathToFifoOut_AllParams
// Function Description : MSDMA Inpath and FIFO Out Path Test : 5+ 4+ 4+ (9*9) = 94EA *2 = 108 EA
// Input : 	None
// Output :    	None
static void CAMERAT_TestDmaInPathToFifoOut_AllParams(void)
{
	int nI;
	int nSrcCropIdx;
	int nImgEffectIdx;
	int nArbitraryIdx;
	int nSrcIdx;
	int nDstIdx;
	
	// 1. Set the case of the input parameters.
	//================================
	u32 uSrcCropSzList[5][2] = {{176, 112}, {120, 240},  {112, 160}, {192, 144}, {640, 480}};
	IMAGE_EFFECT eImgEffectList[6] = {BYPASS, ARBITRARY_CBCR, NEGATIVE, ART_FREEZE, EMBOSSING, SILHOUETTE};
	u32 uArbitraryCbCrList[5][2] = {{0, 0}, {128, 128}, {255, 255}, {0, 255}, {255, 0}};
	CSPACE eSrcFmtArray[9] = {YCBYCR, YCRYCB, CBYCRY, CRYCBY,  YC422, YC420, RGB16, RGB18, RGB24 };
	CSPACE eDstFmtArray[9] = {YCBYCR, YCRYCB, CBYCRY, CRYCBY,  YC422, YC420, RGB16, RGB18, RGB24 };
	
	// 2. Change the case and test all the case
	//=================================
	eInputPath = DMA;
	eOutputPath = FIFO;
	uSrcWidth = 640;
	uSrcHeight = 480;
	for (nI=0; nI<2; nI++)
	{
		eSrcDataFmt = RGB16;
		eDstDataFmt = RGB24;
		eProcessPath = (nI==0) ? P_PATH : C_PATH;
		eRotDeg = ROT_0;;

		for (nSrcCropIdx=0; nSrcCropIdx<5; nSrcCropIdx++) // Scaler up,down, bypass = 5 EA (2*2 + 1)
		{
			uSrcCroppedHsz = uSrcCropSzList[nSrcCropIdx][0];
			uSrcCroppedVsz = uSrcCropSzList[nSrcCropIdx][1];

			UART_Printf("\nHit any key to test\n");
			UART_Getc();
			CAMERAT_TestDmaInPathToFifoOut_Complex();
		}

		for (nImgEffectIdx=2; nImgEffectIdx<6; nImgEffectIdx++) // Image effect = 4 EA
		{
			eImgEffect = eImgEffectList[nImgEffectIdx];

			UART_Printf("\nHit any key to test\n");
			Getc();
			CAMERAT_TestDmaInPathToFifoOut_Complex();
		}

		eImgEffect = ARBITRARY_CBCR;
		for (nArbitraryIdx=0; nArbitraryIdx<5; nArbitraryIdx++) // Arbitrary Cb,Cr = 5 EA
		{
			uArbitraryCbValue = uArbitraryCbCrList[nArbitraryIdx][0];
			uArbitraryCrValue = uArbitraryCbCrList[nArbitraryIdx][1];

			UART_Printf("\nHit any key to test\n");
			Getc();
			CAMERAT_TestDmaInPathToFifoOut_Complex();
		}

		eImgEffect = BYPASS;

		for ( nDstIdx=0; nDstIdx<9; nDstIdx++)
		{
			eDstDataFmt = eDstFmtArray[nDstIdx];	
			for (nSrcIdx=0; nSrcIdx<9; nSrcIdx++)
			{
				eSrcDataFmt = eSrcFmtArray[nSrcIdx];
				UART_Printf("\n Destinaltion Format = %d\n", eDstDataFmt);
				UART_Printf("Hit any key to test\n");
				UART_Getc();
				CAMERAT_TestDmaInPathToFifoOut_Complex();
			}
		}

	}
}

//////////
// Function Name : CAMERAT_TestScalerMaxSize
// Function Description : Max Scaler Size Test in Preview / Codec Path
// Input : 	None
// Output :    	None
static void CAMERAT_TestScalerMaxSize(void)
{
	u32 uLcdHsz, uLcdVsz;
	u32 uWIN0FbEndAddr;
	u32 uSrcAddr;
	u32 uDstHsz, uDstVsz;
	u32 uDisplayStartX, uDisplayStartY;
	u32 uSrcCropStartX, uSrcCropStartY;
//	u32 uCodecBufAddr;
	
    // 1. Initialize interrupt
    //==============================
	INTC_Enable(NUM_CAMIF_P);
	INTC_Enable(NUM_CAMIF_C);
	INTC_SetVectAddr(NUM_CAMIF_P, Isr_MSDMAPreviewDone);
	INTC_SetVectAddr(NUM_CAMIF_C, Isr_MSDMACodecDone);
	bPreviewDone = false;
	bCodecDone = false;
	eProcessPath = P_PATH;
	eLcdBpp = RGB24;
	
	// 2. Initialize lcd
	//================================
	LCD_InitDISPC(eLcdBpp, uLcdFbAddr, WIN0, false);
	LCD_GetFrmSz(&uLcdHsz, &uLcdVsz, WIN0);	
	LCD_GetFbEndAddr(&uWIN0FbEndAddr, WIN0);
	LCD_SetWinOnOff(1, WIN0);
	LCD_Start();

	// 3. Create the input picture 
	//=================================
#if 0 // Codec Path Confirm
	uSrcAddr = uWIN0FbEndAddr+3200*2400*4;
	GLIB_InitInstance(uWIN0FbEndAddr, 3200, 2400, eLcdBpp);
	GLIB_DrawPattern(3200, 2400);

	CAMERA_InitDmaInPath(3200, 2400, uWIN0FbEndAddr, eLcdBpp,
		1600, 1200, uSrcAddr, eLcdBpp, FLIP_NO, ROT_0, C_PATH);

	CAMERA_StartDmaInPath();
	while(!bCodecDone);
	bCodecDone = false;
	CAMERA_StopDmaInPath();
#else // Preview Path Confirm
	uSrcAddr = uWIN0FbEndAddr+1440*960*4; // D1 Image size check
	GLIB_InitInstance(uWIN0FbEndAddr, 1440, 960, eLcdBpp);
	GLIB_DrawPattern(1440, 960);

	CAMERA_InitDmaInPath(1440, 960, uWIN0FbEndAddr, eLcdBpp,
		720, 480, uSrcAddr, eLcdBpp, FLIP_NO, ROT_0, P_PATH);

	CAMERA_StartDmaInPath();
	while(!bPreviewDone);
	bPreviewDone = false;
	CAMERA_StopDmaInPath();
#endif

	// 4. Run the process of dma-input path
	//=============================
	eDstDataFmt = eLcdBpp;
//	uCodecBufAddr = uSrcAddr+uSrcWidth*uSrcHeight*4;
	uDstHsz = 320, uDstVsz = 240;
	uDisplayStartX = 0,	uDisplayStartY = 0;
	uSrcCropStartX = 0,	uSrcCropStartY = 0;

	eRotDeg = ROT_0;
#if 0 // Codec Path Confirm
	CAMERA_InitDmaInPath0(1600, 1200, uSrcCropStartX, uSrcCropStartY, 1600, 1200, uSrcAddr, uSrcAddr,eLcdBpp,
		uLcdHsz, uLcdVsz, uDisplayStartX, uDisplayStartY, uDstHsz, uDstVsz, uLcdFbAddr,uLcdFbAddr, eLcdBpp, FLIP_NO, ROT_0, eProcessPath);
#else // Preview Path Confirm
	CAMERA_InitDmaInPath0(720, 480, uSrcCropStartX, uSrcCropStartY, 720, 480, uSrcAddr, uSrcAddr,eLcdBpp,
		uLcdHsz, uLcdVsz, uDisplayStartX, uDisplayStartY, uDstHsz, uDstVsz, uLcdFbAddr,uLcdFbAddr, eLcdBpp, FLIP_NO, ROT_0, eProcessPath);
#endif

	CAMERAT_DisplayParam();

	CAMERA_StartDmaInPath();
	if (eProcessPath==P_PATH)
		while(!bPreviewDone);
	else
		while(!bCodecDone);
	CAMERA_StopDmaInPath();

	INTC_Disable(NUM_CAMIF_C);
	INTC_Disable(NUM_CAMIF_P);

}

//////////
// Function Name : CAMERAT_TestBypass_Preview
// Function Description : Bypass Scaler mode Test In Preview Path 
// Input : 	None
// Output :    	None
static void CAMERAT_TestBypass_Preview(void)
{
	u32 uLcdHsz, uLcdVsz;
	u32 uWIN0FbEndAddr;
	u32 uDstHsz, uDstVsz;
	u32 uDisplayStartX, uDisplayStartY;
	u32 uSrcCropStartX, uSrcCropStartY;
	
	bPreviewDone = false;
	bCodecDone = false;
	eProcessPath = P_PATH;
	eLcdBpp = RGB24;

       // 1. Initialize interrupt
  	//==============================
	INTC_Enable(NUM_CAMIF_P);
	INTC_Enable(NUM_CAMIF_C);
	INTC_SetVectAddr(NUM_CAMIF_P, Isr_PreviewDone);
	INTC_SetVectAddr(NUM_CAMIF_C, Isr_MSDMACodecDone);
	
	// 2. Initialize lcd
	//================================
	LCD_InitDISPC(eLcdBpp, uLcdFbAddr, WIN0, false);
	LCD_GetFrmSz(&uLcdHsz, &uLcdVsz, WIN0);	
	LCD_GetFbEndAddr(&uWIN0FbEndAddr, WIN0);
	LCD_SetWinOnOff(1, WIN0);
	LCD_Start();

	// 3. Preview Path Bypass
	//=================================

	CAMERA_InitSensor();
	CAMERA_InitPreviewPath0(0, 0, 800,600, 800, 600, 0, 0, 800, 600, uWIN0FbEndAddr, uWIN0FbEndAddr, YCRYCB, FLIP_NO, ROT_0);
	CAMERA_StartPreviewPath();
	UART_Printf("\nIf You want Capture. press Any key\n");
	UART_Getc();
	while(!bPreviewDone);
	CAMERA_StopPreviewPath();
	bPreviewDone = false;

	// 4. Run the process of dma-input path
	//=============================
	eDstDataFmt = eLcdBpp;
	uDstHsz = 320, uDstVsz = 240;
	uDisplayStartX = 0,	uDisplayStartY = 0;
	uSrcCropStartX = 0,	uSrcCropStartY = 0;
	eRotDeg = ROT_0;

	CAMERA_ResetIp(); //jungil0214
	CAMERA_InitDmaInPath0(800, 600, uSrcCropStartX, uSrcCropStartY, 800, 600, uWIN0FbEndAddr, uWIN0FbEndAddr,YCRYCB,
		uLcdHsz, uLcdVsz, uDisplayStartX, uDisplayStartY, uDstHsz, uDstVsz, uLcdFbAddr,uLcdFbAddr, eLcdBpp, FLIP_NO, ROT_0, C_PATH);

	CAMERAT_DisplayParam();

	CAMERA_StartDmaInPath();
	UART_Printf("\nCheck Image\n");
	UART_Getc();
	while(!bCodecDone);
	CAMERA_StopDmaInPath();

	INTC_Disable(NUM_CAMIF_C);
	INTC_Disable(NUM_CAMIF_P);


}


//////////
// Function Name : CAMERAT_TestBypass_Codec
// Function Description : Scaler Bypass Mode Test In Codec Path
// Input : 	None
// Output :    	None
static void CAMERAT_TestBypass_Codec(void)
{
	u32 uLcdHsz, uLcdVsz;
	u32 uWIN0FbEndAddr;
	u32 uDstHsz, uDstVsz;
	u32 uDisplayStartX, uDisplayStartY;
	u32 uSrcCropStartX, uSrcCropStartY;
	
	bPreviewDone = false;
	bCodecDone = false;
	eProcessPath = C_PATH;
	eLcdBpp = RGB24;

    // 1. Initialize interrupt
    //==============================

	INTC_Enable(NUM_CAMIF_P);
	INTC_Enable(NUM_CAMIF_C);
	INTC_SetVectAddr(NUM_CAMIF_P, Isr_MSDMAPreviewDone);
	INTC_SetVectAddr(NUM_CAMIF_C, Isr_CodecDone);
	
	// 2. Initialize lcd
	//================================
	LCD_InitDISPC(eLcdBpp, uLcdFbAddr, WIN0, false);
	LCD_GetFrmSz(&uLcdHsz, &uLcdVsz, WIN0);	
	LCD_GetFbEndAddr(&uWIN0FbEndAddr, WIN0);
	LCD_SetWinOnOff(1, WIN0);
	LCD_Start();

	// 3. Preview Path Bypass
	//=================================
	CAMERA_InitSensor();
	CAMERA_InitCodecPath0(0, 0, 800,600, 800, 600, 0, 0, 800, 600, uWIN0FbEndAddr, uWIN0FbEndAddr, YCRYCB, FLIP_NO, ROT_0);
	CAMERA_StartCodecPath(0);
	UART_Printf("\nIf You want Capture. press Any key\n");
	UART_Getc();
	while(!bCodecDone);
	CAMERA_StopCodecPath();
	bCodecDone = false;

	// 4. Run the process of dma-input path
	//=============================
	eDstDataFmt = eLcdBpp;
	uDstHsz = 320, uDstVsz = 240;
	uDisplayStartX = 0,	uDisplayStartY = 0;
	uSrcCropStartX = 0,	uSrcCropStartY = 0;
	eRotDeg = ROT_0;

	CAMERA_ResetIp(); //jungil0214
	CAMERA_InitDmaInPath0(800, 600, uSrcCropStartX, uSrcCropStartY, 800, 600, uWIN0FbEndAddr, uWIN0FbEndAddr,YCRYCB,
		uLcdHsz, uLcdVsz, uDisplayStartX, uDisplayStartY, uDstHsz, uDstVsz, uLcdFbAddr,uLcdFbAddr, eLcdBpp, FLIP_NO, ROT_0, P_PATH);

	CAMERAT_DisplayParam();

	CAMERA_StartDmaInPath();
	UART_Printf("\nCheck Image\n");
	UART_Getc();
	while(!bPreviewDone);
	CAMERA_StopDmaInPath();

	INTC_Disable(NUM_CAMIF_C);
	INTC_Disable(NUM_CAMIF_P);

}



/*
//////////
// Function Name : CAMERAT_ReadWriteCameraModule
// Function Description : Camera Module Read / Write Test
// Input : 	None
// Output :    	None
static void CAMERAT_ReadWriteCameraModule(void)
{
	CAMERA_ReadCameraModuleStatus();
}
*/

//////////
// Function Name : CAMERAT_Preview4PingPong
// Function Description : Basic Function of Preview Path Camera In DMA Out
// Input : 	None
// Output :    	None
static void CAMERAT_Preview4PingPong(void)
{
	u32 uLcdHsz, uLcdVsz;
	u32 i,j,k;
	
	// 1. Initialize interrupt process
	//=======================================
	INTC_SetVectAddr(NUM_CAMIF_P, Isr_4PreviewDone);
	INTC_Enable(NUM_CAMIF_P);

	// 2. Initialize lcd
	//=======================================
	*(volatile unsigned *)0x7410800c=0x0; // Modem I/F Bypass Control. It should be 0,
	LCD_InitDISPC(eLcdBpp, uFbAddr1, WIN0, false);
	LCD_GetFrmSz(&uLcdHsz, &uLcdVsz, WIN0);	
	LCD_SetWinOnOff(1, WIN0);

	GLIB_InitInstance(uLcdFbAddr, uLcdHsz, uLcdVsz, eLcdBpp);
	k=0;
	for(i=0; i<240; i++)
		for(j=0; j<320; j++)
			GLIB_PutPixel2(j, i, pSmile[k++]);

	LCD_Start();
	UART_Printf("\nHit any key to start Camera simple test!\n");
	UART_Getc();

	// 3. Initialize camera and run the  process of preview path.
	//========================================
	CAMERA_InitSensor();
	CAMERA_InitPreviewPath_4Buf(uLcdHsz, uLcdVsz, uFbAddr1, uFbAddr2, uFbAddr3, uFbAddr4, eLcdBpp, FLIP_NO, ROT_0);
	
	CAMERA_StartPreviewPath();
	UART_Printf("\nHit any key to stop it!\n");
	UART_Getc();
	CAMERA_StopPreviewPath();
	INTC_Disable(NUM_CAMIF_P);
	
	LCD_SetAllWinOnOff(0);
	LCD_Stop();
	UART_Printf("\nLCD OFF\n");
}

static void CAMERAT_SetParameter(void)
{
	u32 uParam;
	u32 x;

	while (true)
	{
		Disp("[0] Exit\n");
		Disp("[1] Select Process Path\n");
		Disp("[2] Select Image effect\n");
		Disp("[3] Select Rotaion degree\n");
		Disp("Select the setting value : ");
		Disp("\n");
		uParam = UART_GetIntNum();

		if (uParam == 0)
			break;
		switch(uParam)
		{
			case 1:
					UART_Printf(" Select Process Path (1:Preview Path   2:Codec Path) \n");
					x = UART_GetIntNum();
					if(x == 1)
						eProcessPath = P_PATH;
					else
						eProcessPath = C_PATH;
					break;
					
			case 2:
					UART_Printf(" Select Image effect (1:No Image Effect   2:Arbitrary Cb/Cr   3:Negative\n");
					UART_Printf("   4:Art Freeze   5:Embossing   6:Silhouette) \n");
					x = UART_GetIntNum();
					if(x == 1)
						eImgEffect = BYPASS;
					else if (x == 2)
						eImgEffect = ARBITRARY_CBCR;
					else if (x == 3)
						eImgEffect =  NEGATIVE;
					else if (x == 4)
						eImgEffect = ART_FREEZE;
					else if (x == 5)
						eImgEffect = EMBOSSING;
					else
						eImgEffect = SILHOUETTE;
					break;
					
			case 3: 
					UART_Printf(" Rotaion degree (1:No rotation  2:Rotation 90 3:Rotation 180 4:Rotation 270) \n");
					x = UART_GetIntNum();
					if (x == 1)
						eRotDeg = ROT_0;
					else if (x == 2)
						eRotDeg = ROT_90;
					else if (x == 3)
						eRotDeg = ROT_180;
					else
						eRotDeg = ROT_270;
					break;
			default:
					break;
		}
	}
}

static void CAMERAT_TestDisplayOnoToOneImage(void)
{
	u32 uLcdHSz, uLcdVSz;
	u32 i;
	u32 uBytePerPixel;
	u32	uImageBuffAddr = CODEC_MEM_ST+(CODEC_MEM_LMT-CODEC_MEM_ST)/4;

	INTC_Enable(NUM_CAMIF_C);
	INTC_SetVectAddr(NUM_CAMIF_C, Isr_CodecDone);	
	bCodecDone = false;

	if(uImgResolution == RGB16||uImgResolution == YCBYCR||uImgResolution == YCRYCB||uImgResolution == CBYCRY||uImgResolution == CRYCBY)
		uBytePerPixel = 2;
	else if(uImgResolution == RGB18||uImgResolution == RGB24)
		uBytePerPixel = 4;
	else  if(uImgResolution == YC420||uImgResolution == YC422)
		uBytePerPixel = 1;
	else 
		Assert(0);

	for (i = 0; i<uImgXsize*uImgYsize*uBytePerPixel; i++)
		*(u8 *)(uImageBuffAddr+i) = 0;

	LCD_InitDISPC(eLcdBpp, uLcdFbAddr, WIN0, false);
	LCD_GetFrmSz(&uLcdHSz, &uLcdVSz, WIN0);	
	LCD_SetWinOnOff(1, WIN0);
// 	LCD_InitLDI(MAIN);
//	LCD_InitWin(eLcdBpp, uImageBuffAddr, WIN0, false);
//	LCD_SetWinOnOff(1, WIN0);
//	LCD_GetFrmSz(&uLcdHSz, &uLcdVSz, WIN0);

#if 0	
	POST_InitIp(uImgXsize, uImgYsize, uLcdFbAddr, uImgResolution, uLcdHSz, uLcdVSz, uImageBuffAddr, eLcdBpp,
		1, false, ONE_SHOT, &oPost);	
	POST_StartProcessing(&oPost);

	while (POST_IsProcessingDone(&oPost));
#else
	CAMERA_InitDmaInPath(uImgXsize, uImgYsize, uLcdFbAddr, uImgResolution,
		uLcdHSz, uLcdVSz, uImageBuffAddr, eLcdBpp, FLIP_NO, ROT_0, C_PATH);
	CAMERA_StartDmaInPath();
#endif
	LCD_Start();
	while(!bCodecDone);
	UART_Printf("Enter any key to exit it!\n");
	UART_Getc();
	CAMERA_StopDmaInPath();
	
}

static void CAMERAT_TestOneToOneCopyInPreviewPath(void)
{
	u32 uLcdHsz, uLcdVsz;
 	u32 uBytePerPixel;
	u32 i;

	// 1. Initialize interrupt 
	//=================================
	INTC_Enable(NUM_CAMIF_P);
	INTC_SetVectAddr(NUM_CAMIF_P, Isr_PreviewDone);	
	bPreviewDone = false;

	UART_Printf("DMA input -> DMA output (1:1 copy) in p_path\n");
	UART_Printf("Enter Any Key To Test Display\n");
	UART_Getc();
		
	CAMERAT_MakeImage();

	CAMERA_InitDmaInPath(uImgXsize, uImgYsize, uImgAddr, uImgResolution,
		uImgXsize, uImgYsize, uLcdFbAddr, uImgResolution, FLIP_NO, ROT_0, P_PATH);
	CAMERA_OneToOneDataCopy(P_PATH);
	
	CAMERA_StartDmaInPath();
	while(!bPreviewDone);
	CAMERA_WaitMSDMAP();
	CAMERA_StopDmaInPath();

	if(uImgResolution == RGB16||uImgResolution == YCBYCR||uImgResolution == YCRYCB||uImgResolution == CBYCRY||uImgResolution == CRYCBY)
		uBytePerPixel = 2;
	else if(uImgResolution == RGB18||uImgResolution == RGB24)
		uBytePerPixel = 4;
	else  if(uImgResolution == YC420||uImgResolution == YC422)
		uBytePerPixel = 1;
	else 
		Assert(0);
	
	UART_Printf("******>> Compare Count Size  : %d <<******\n",uImgXsize*uImgYsize*uBytePerPixel);
	UART_Printf("uImgXsize: %d, uImgYsize: %d\n\n",uImgXsize, uImgYsize);
	
	if (Compare32(uImgAddr, uLcdFbAddr, uImgXsize*uImgYsize*uBytePerPixel/4))
		UART_Printf(" >> Test Tx&Rx -> Ok << \n");
	else
	{
		UART_Printf(" >>*** Tx-data & Rx-data mismatch ***<< \n");
	}
	
	CAMERAT_TestDisplayOnoToOneImage();
	INTC_Disable(NUM_CAMIF_P);
}



static void CAMERAT_TestOneToOneCopyInCodecPath(void)
{
	u32 uLcdHsz, uLcdVsz;
 	u32 uBytePerPixel;
	u32 i;

	// 1. Initialize interrupt 
	//=================================
	INTC_Enable(NUM_CAMIF_C);
	INTC_SetVectAddr(NUM_CAMIF_C, Isr_CodecDone);	
	bCodecDone = false;

	// 2. Make Image for test
	//=================================
	UART_Printf("DMA input -> DMA output (1:1 copy) in c_path\n\n");
	UART_Printf("Enter Any Key To Test Display\n");
	UART_Getc();
		
	CAMERAT_MakeImage();

	CAMERA_InitDmaInPath(uImgXsize, uImgYsize, uImgAddr, uImgResolution,
		uImgXsize, uImgYsize, uLcdFbAddr, uImgResolution, FLIP_NO, ROT_0, C_PATH);
	CAMERA_OneToOneDataCopy(C_PATH);

	bCodecDone = false;
	CAMERA_StartDmaInPath();
	while(!bCodecDone);
	CAMERA_WaitMSDMAC();
	CAMERA_StopDmaInPath();

	if(uImgResolution == RGB16||uImgResolution == YCBYCR||uImgResolution == YCRYCB||uImgResolution == CBYCRY||uImgResolution == CRYCBY)
		uBytePerPixel = 2;
	else if(uImgResolution == RGB18||uImgResolution == RGB24)
		uBytePerPixel = 4;
	else  if(uImgResolution == YC420||uImgResolution == YC422)
		uBytePerPixel = 1;
	else 
		Assert(0);
	
	UART_Printf("********>> Compare Count Size  : %d\n",uImgXsize*uImgYsize*uBytePerPixel);
	UART_Printf("uImgXsize: %d, uImgYsize: %d\n",uImgXsize, uImgYsize);


	if (Compare32(uImgAddr, uLcdFbAddr, uImgXsize*uImgYsize*uBytePerPixel/4))
		UART_Printf(" >> Test Tx&Rx -> Ok << \n");
	else
	{
		UART_Printf(" >>*** Tx-data & Rx-data mismatch ***<< \n");
	}

	CAMERAT_TestDisplayOnoToOneImage();
	INTC_Disable(NUM_CAMIF_C);
}


static void CAMERAT_SetInterlaceInputParam(void)
{
	u32 uYuvFmt, x;

	while (true)
	{
		UART_Printf("[0] Exit\n");
		UART_Printf("[1] Select order of YUV data\n");
		UART_Printf("[2] Select Source image Size\n");
		UART_Printf("[3] Select ITR-R.601 or 656 interface\n");
		UART_Printf("Select the setting value : ");
		UART_Printf("\n");
		uYuvFmt = UART_GetIntNum();

		if (uYuvFmt == 0)
			break;
		switch(uYuvFmt)
		{
			case 1:
					UART_Printf("Select order of YUV data (1: YCbYCr 2: YCrYCb 3: CbYCrY 4:CrYCbY) \n");
					UART_Printf("Select number : ");
					UART_Printf("\n");
					x = UART_GetIntNum();
					if(x == 1)
						eYuvFmt = YCBYCR;
					else if (x == 2)
						eYuvFmt = YCRYCB;
					else if (x == 3)
						eYuvFmt =  CBYCRY;
					else
						eYuvFmt = CRYCBY;					
					break;
			case 2:
					UART_Printf("Select (H)X(V) (1: 800X480 2: 80X64 )\n"); //   2: 480X640   3: 768X1024) \n");
					UART_Printf("Select number : ");
					UART_Printf("\n");		
					x = UART_GetIntNum();
					if (x == 1)
					{
						#if 1
						uHFrame=800;
						uVFrame=480;
						#else
						uHFrame=64;
						uVFrame=80;
						#endif
					}
					else if (x == 2)
					{
						uHFrame=80;		
						uVFrame=64;
					}
					else
					{
						uHFrame=768;		
						uVFrame=1024;
					}
					break;
			case 3:
					UART_Printf("Select ITR-R.601 or 656 interface (1: ITU-R.601  2: ITU-R.656) \n");
					UART_Printf("Select number : ");
					UART_Printf("\n");
					x = UART_GetIntNum();
					if(x == 1)
						eVideoStandard = CCIR601;
					else
						eVideoStandard = CCIR656;
					break;
			default:
					break;
		}
	}
}


// FIMC => 601:Normal Field=>low  656:Normal Field=>low
// FIMD => 601:Normal Field=>high 656:Normal Field=>lows
static void CAMERAT_TestInterlaceInputMode(void)			
{
	u32 i=0;
	u32 uFgAddr;
	u32 uLcdHsz, uLcdVsz;
	u32 ui,j,k;
	u32 uLcdFbAddr2;
	#if 1
	u32 uHSz=800, uVSz=480;
	#else
	u32 uHSz=80, uVSz=64;
	#endif

	const char* cpItuRFormat;
	const char* cpItuRStandard;
	

	// 1. Initialize interrupt 
	//=================================
	#if 1 
	INTC_Enable(NUM_CAMIF_C);
	INTC_SetVectAddr(NUM_CAMIF_C, Isr_CodecDone);
	bCodecDone = false;
	#else
	INTC_Enable(NUM_CAMIF_P);
	INTC_SetVectAddr(NUM_CAMIF_P, Isr_PreviewDone);
	bCodecDone = false;
	#endif
	
	// 2. Initialize lcd
	//==================================
	*(volatile unsigned *)0x7410800c=0x0; // Modem I/F Bypass Control. It should be 0,
	LCD_InitDISPC(eLcdBpp, uLcdFbAddr, WIN0, false);
	LCD_GetFrmSz(&uLcdHsz, &uLcdVsz, WIN0);	
	LCD_SetWinOnOff(1, WIN0);
	GLIB_InitInstance(uLcdFbAddr, uLcdHsz, uLcdVsz, eLcdBpp);
	k=0;
	for(ui=0; ui<480; ui++)
		for(j=0; j<800; j++)
			GLIB_PutPixel2(j, ui, pSmile_3[k++]);

	LCD_GetFbEndAddr(&uLcdFbAddr2, WIN0);
	LCD_InitWin(eLcdBpp, 320, 240, 0, 0, 320, 240, 0, 0, uLcdFbAddr2, WIN1, false);
	LCD_SetWinOnOff(1, WIN1);
	LCD_Start();
	UART_Printf("\nHit any key to start intelaced test!\n");
	UART_Getc();

	// 3. Initialize lcd
	//========================================
	CAMERA_InitSensor_2();
	CAMERA_InitItuRInterface(eVideoStandard, eYuvFmt, uHSz, uVSz);
//	CAMERA_InitCodecPath(320, 240, uLcdFbAddr, RGB24, FLIP_NO, ROT_0); // codec path setting. 
	CAMERA_InitCodecPath0(0, 0, 800, 480,	320, 240, 0, 0, 320, 240, uLcdFbAddr2, (uLcdFbAddr2+320*4), RGB24, FLIP_NO, ROT_0);	
//	CAMERA_InitPreviewPath0(0, 0, 800, 480, 320, 240, 0, 0, 320, 240, uLcdFbAddr2, (uLcdFbAddr2+320*4), RGB24, FLIP_NO, ROT_0);	
//	CAMERA_SetDstScanOffset(320, 120, 0, 0, uLcdFbAddr2, (uLcdFbAddr2+4));
//	*(unsigned int *)0x78000138 = 0x140; // Line Offset 320 setting.  Codec Offset
	*(unsigned int *)0x7800012c = 0x140; // Line Offset 320 setting.  Preview Offset
	CAMERA_SetScanMode(1); // interlace input setting. 
	CAMERA_SetFieldClk(false, true);
	CAMERA_StartCodecPath(0); // start codec path. 
//	CAMERA_StartPreviewPath(); // start codec path. 
	
	cpItuRStandard =
			(eVideoStandard == CCIR601) ? "ITU-R BT601" :
			(eVideoStandard == CCIR656) ? "ITU-R BT656" : "error";
	if (cpItuRStandard == "error")
	Assert(0);

	cpItuRFormat =
		(eYuvFmt == YCBYCR) ? "YCbYCr422_Intlv" :
		(eYuvFmt == YCRYCB) ? "YCrYCb422_intlv" :
		(eYuvFmt == CBYCRY) ? "CbYCrY422_Intlv" :
		(eYuvFmt == CRYCBY) ? "CrYCbY422_Intlv" : "error";
	if (cpItuRFormat == "error")
		Assert(0); 
			
	UART_Printf("Basic Info:\n");
	UART_Printf("Video Output Standard: %s\n", cpItuRStandard);
	UART_Printf("Source Image Size:\n");
	UART_Printf("\t- Horizontal Size: %d\n", uHFrame);
	UART_Printf("\t- Vertical Size: %d\n", uVFrame);
	UART_Printf("Input Image format: %s\n", cpItuRFormat); 
	UART_Printf("Enter Any Key To Test End\n");
	UART_Getc();

	#if 1
	while(!bCodecDone);
	CAMERA_StopCodecPath();
	#else
	while(!bPreviewDone);
	CAMERA_StopPreviewPath();
	#endif	

	LCD_PerFrameOff();
		
	LCD_Stop();
	
//	CAMERA_SetScanMode(PROGRESSIVE);
	CAMERA_SetScanMode(0);	

#if 0 // Ref Code
	// RGB images are stored at uLcdDstFbAddr.
	GLIB_InitInstance(uLcdFbAddr, uHFrame, uVFrame, eSrcDataFmt);
	#if 1
	GLIB_DrawPattern(uHFrame, uVFrame);
	#else
	GLIB_PutLine(0, 0,uHFrame-1, 0, C_RED);
	GLIB_PutLine(0, 1,uHFrame-1, 1, C_BLUE);
	GLIB_PutLine(0, 2,uHFrame-1, 2, C_RED);
	GLIB_PutLine(0, 3,uHFrame-1, 3, C_BLUE);
	GLIB_PutLine(0, 4,uHFrame-1, 4, C_RED);
	GLIB_PutLine(0, 5,uHFrame-1, 5, C_BLUE);
	#endif





	while(!(i==5))
	{
		LCD_InitBt601And656If(uHFrame, uVFrame);
		LCD_InitWin(eSrcDataFmt, uHFrame, uVFrame, 0, 0, uHFrame, uVFrame,	0, 0, uLcdFbAddr, WIN0, false);
		LCD_InitBt601And656If(eVideoStandard, eYuvFmt);
		LCD_SetScanMode(INTERLACE);
		LCD_SetWinOnOff(1, WIN0);
		LCD_GetFbEndAddr(&uFgAddr, WIN0);
		
#if 1// 601 format test
		if (eVideoStandard == BT601)
		{
			LCD_SetBt601And656CtrlClk(true,false,false,false,false,false,false);
			LCD_SetBT601VsyncDly(100); // Timing is cycle of vclk. odd field cycle.
			// Delay mode: under 64:top,even   over 64:bottom,odd
		}
#endif
		LCD_Start();

#if 1
		Delay(800);
		LCD_PerFrameOff();
		LCD_SetInterlace();
		LCD_Start();
#endif
		CAMERA_InitItuRInterface(eVideoStandard, eYuvFmt, uHFrame, uVFrame);
		CAMERA_SetScanMode(INTERLACE);

		CAMERA_InitPreviewPath(uHSz, uVSz, uFgAddr, eDstDataFmt, FLIP_NO, ROT_0);
		
if (eVideoStandard == BT601)
		CAMERA_SetFieldClk(true, true); // Field port mode, Invert Filed's even field:High
else // BT656
		CAMERA_SetFieldClk(false, true); // Field port mode, Invert Filed's even field:High

//		CAMERA_SelectCam(CAM_B);
		
		bPreviewDone=false;
		CAMERA_StartPreviewPath();

		cpItuRStandard =
			(eVideoStandard == BT601) ? "ITU-R BT601" :
			(eVideoStandard == BT656) ? "ITU-R BT656" : "error";
		if (cpItuRStandard == "error")
			Assert(0);

		cpItuRFormat =
			(eYuvFmt == YCBYCR) ? "YCbYCr422_Intlv" :
			(eYuvFmt == YCRYCB) ? "YCrYCb422_intlv" :
			(eYuvFmt == CBYCRY) ? "CbYCrY422_Intlv" :
			(eYuvFmt == CRYCBY) ? "CrYCbY422_Intlv" : "error";
		if (cpItuRFormat == "error")
			Assert(0); 
			
		UART_Printf("Basic Info:\n");
		UART_Printf("Video Output Standard: %s\n", cpItuRStandard);
		UART_Printf("Source Image Size:\n");
		UART_Printf("\t- Horizontal Size: %d\n", uHFrame);
		UART_Printf("\t- Vertical Size: %d\n", uVFrame);
		UART_Printf("Input Image format: %s\n", cpItuRFormat); 
		UART_Printf("Enter Any Key To Test Display\n");
		UART_Getc();

		while(!bPreviewDone);
		CAMERA_StopPreviewPath();

		LCD_PerFrameOff();
		
		*(volatile unsigned *)0x7410800c=0x0; // Modem I/F Bypass Control. It should be 0,
		LCD_InitDISPC(eDstDataFmt, uLcdFbAddr, WIN0, false);
		LCD_SetWinOnOff(1, WIN0);

		LCD_Start();
		Disp("\nHit the Any key to end:\n");
		UART_Getc();
		
		LCD_Stop();
		i++;
	}
	
	CAMERA_SetScanMode(PROGRESSIVE);
#endif	
}




void * camera_function[][2]=
{
		CAMERAT_TestPreviewPath_Simple,               "Preview path (Simple usage)-OK\n",
		//CAMERAT_TestPreviewPath_Complex,              "Preview path (Complex usage)OK\n",
		CAMERAT_TestPreviewPath_AllParams,            "Preview path (All parameters)-OK\n",
		CAMERAT_TestPreviewPath_Rot,                       "Preview Path (Rotator) -OK\n",
		CAMERAT_TestPreviewPath_LastIRQ,                "Preview Path Last IRQ Test-OK\n\n",

//		CAMERAT_TestPreviewPathToFifoOut_Simple,      "Preview path to FIFO-Out (Simple usage)-3.5\n",
//		CAMERAT_TestPreviewPathToFifoOut_Complex,	  "Preview path to FIFO-Out (Complex usage)OK\n",
//		CAMERAT_TestPreviewPathToFifoOut_AllParams,   "Preview path to FIFO-Out (All parameters)-3.5\n\n",

//		CAMERAT_TestCodecPathToFifoOut_Simple,           "Codec Path to FIFO-out (Simple Usage)-OK\n",
//		CAMERAT_TestCodecPathToFifoOut_Complex,	  "Preview path to FIFO-Out (Complex usage)OK\n",
//		CAMERAT_TestCodecPathToFifoOut_AllParams,   "Codec path to FIFO-Out (All parameters)-OK\n\n",

		CAMERAT_TestCodecPath_Simple,                 "Codec Path (Simple usage)-OK\n",
		//CAMERAT_TestCodecPath_Complex,                "Codec path (Complex usage)OK\n",
		CAMERAT_TestCodecPath_AllParams,              "Codec path (All parameters)-OK\n\n",

		CAMERAT_TestDmaInPath_CodecPath_Simple,                 "MSDMA In path[CodecPath] (Simple usage)-OK\n",
		CAMERAT_TestDmaInPath_PreviewPath_Simple, "MSDMA In Path[Preview Path] (Simple usage)-OK\n",
		//CAMERAT_TestDmaInPath_Complex,	              "MSDMA n path[Both Path] (Complex usage)OK\n",
		CAMERAT_TestDmaInPath_AllParams,              "MSDMA In path[Both Path] (All parameters)-3.5\n\n",

		CAMERAT_TestDmaInPathToFifoOut_Simple,        "DmaIn path to FIFO-Out (Simple usage)-3.5\n",
		//CAMERAT_TestDmaInPathToFifoOut_Complex,       "DmaIn path to FIFO-Out (Complex usage)OK\n",
		CAMERAT_TestDmaInPathToFifoOut_AllParams,     "DmaIn path to FIFO-Out (All parameters)-3.5\n\n",

		CAMERAT_TestTestPattern,                      "Pattern Test(Camera input To DMA Path:Max 1280*1024 Input Size)-OK\n",

		CAMERAT_TestZoomInOut_Preview,	                      "Zoom In/Zoom Out Test[Preview Path]-3.5.\n",
		CAMERAT_TestZoomInOut_Codec,                "Zoom In/Zoom Out Tets[Codec Path]-OK\n",

		CAMERAT_TestScalerMaxSize,                "Codec / Preview Scaler Max Size Check -OK\n\n",

		CAMERAT_TestBypass_Preview,                            "Preview Bypass mode -OK   \n",
		CAMERAT_TestBypass_Codec,                               "Codec Bypass mode  -OK    \n\n",

		CAMERAT_TestMultiCaptureOfCodecPath,               "CaptureCount Test[Codec Path] -OK\n",
		CAMERAT_TestMultiCaptureOfPreviewPath,            "CaptureCount Test[Preview Path] -3.5\n\n",
//		CAMERAT_ReadWriteCameraModule,     		"CameraModuleRad_Write Test -OK\n",		
		CAMERAT_Preview4PingPong,                             "Camera Preview 4Buf-3.5\n\n",

		CAMERAT_SetParameter,						  		"Set Parameter\n",
		CAMERAT_TestOneToOneCopyInCodecPath,          		"Codec Path (One To One Copy)\n",
		CAMERAT_TestOneToOneCopyInPreviewPath,    		    "Preview Path (One To One Copy)\n\n",

		CAMERAT_SetInterlaceInputParam,						"Set Interlace mode Input Parameter\n",
		CAMERAT_TestInterlaceInputMode,						"Test Interlace Input Mode\n",
		

	0,0
};


//////////
// Function Name : CAMERA_Test
// Function Description : This function tests Camera Interface Function 
// Input : 	NONE 
// Output : 	NONE
// Version : v0.1
void CAMERA_Test(void)
{
	int iIndex;

	CAMERA_SFRInit();
	CAMERA_SetPort();
   	CAMERA_ClkSetting(); // Camera Module CLK Setting

	LCD_SetPort();
	LCD_InitLDI(MAIN);

	POST_InitCh(POST_A, &oPost);

	uFramec = 0 ;
	uLcdFbAddr = CODEC_MEM_ST;
	eLcdBpp = RGB24;
	eFlipDir = FLIP_NO;
	eRotDeg = ROT_0;
	uSrcCroppedHsz = 1600;
	uSrcCroppedVsz = 1200;
	eImgEffect = BYPASS;
	eDstDataFmt = RGB24;
	eSrcDataFmt = RGB16;
	eProcessPath = P_PATH;
	eOutputPath = DMA;
	uSrcWidth = 640;
	 uSrcHeight = 480;

	while(1) {
		iIndex=0;	
		UART_Printf("\n\n");
		while(1) {
		    UART_Printf("%2d:%s",iIndex,camera_function[iIndex][1]);
		    iIndex++;
		    if((int)(camera_function[iIndex][0])==0) {
				UART_Printf("\n");
				break;
		    }
//		    if((iIndex%2)==0) UART_Printf("\n");
		}

		UART_Printf("\nSelect(-1 to exit): ");
		iIndex = UART_GetIntNum();
		UART_Printf("\n");

		if(iIndex ==-1) break;
		if(iIndex >=0 && (iIndex < (sizeof(camera_function)/8)-1) ) 
	    	( (void (*)(void)) (camera_function[iIndex][0]) )();
	}
}


