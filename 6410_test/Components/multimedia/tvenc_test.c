/**************************************************************************************
* 
*	Project Name : S3C6410 Validation
*
*	Copyright 2007 by Samsung Electronics, Inc.
*	All rights reserved.
*
*	Project Description :
*		This software is only for validating functions of the S3C6410.
*		Anybody can use this software without our permission.
*  
*--------------------------------------------------------------------------------------
* 
*	File Name : tvenc_test.c
*  
*	File Description : This file implements the functons for TV Encoder controller test.
*
*	Author : Heemyung.noh
*	Dept. : AP Development Team
*	Created Date : 2007/01/15
*	Version : 0.1 
* 
*	History
*	- Created(Heemyung.noh 2007/01/15)
*  
**************************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "def.h"
#include "option.h"
#include "library.h"
#include "sfr6410.h"
#include "system.h"
#include "sysc.h"
#include "intc.h"
#include "tvenc.h"
#include "lcd.h"
#include "camera.h"
#include "post.h"
#include "glib.h"

#if 0
#define IMG_CIF_CBAR 	"./images/cif_colorbar.rgb"

//#define IMG_RGB_QCIF	"./images/bmp/QCIF_24bits.bmp"
#define IMG_RGB_QCIF	"./images/bmp/QCIF_24bits.bmp"
#define IMG_RGB_QVGA	"./images/bmp/QVGA_24bits.bmp"
#define IMG_RGB_CIF		"./images/bmp/CIF_24bits.bmp"
#define IMG_RGB_VGA		"./images/bmp/VGA_24bits.bmp"
#define IMG_RGB_WIDE	"./images/bmp/WIDE_24bits.bmp"

#define IMG_420_QVGA	"./images/ycbycr/QVGA_420.yuv"
#define IMG_420_CIF		"./images/ycbycr/CIF_420.yuv"
#define IMG_420_VGA		"./images/ycbycr/VGA_420.yuv"
#define IMG_420_WIDE	"./images/ycbycr/WIDE_420.yuv"

#define IMG_422_QCIF	"./images/ycbycr/QCIF_422.yuv"
#define IMG_422_QVGA	"./images/ycbycr/QVGA_422.yuv"
#define IMG_422_CIF		"./images/ycbycr/CIF_422.yuv"
#define IMG_422_VGA		"./images/ycbycr/VGA_422.yuv"
#define IMG_422_WIDE	"./images/ycbycr/WIDE_422.yuv"
#else	//rb1004
#define IMG_CIF_CBAR 	"D:/MyDocument/AP/S3C6410/Software/MobilePlatform/FW_2nd_1229/images/cif_colorbar.rgb"

#define IMG_RGB_QCIF	"D:/MyDocument/AP/S3C6410/Software/MobilePlatform/FW_2nd_1229/images/bmp/QCIF_24bits.bmp"  
#define IMG_RGB_QVGA	"D:/MyDocument/AP/S3C6410/Software/MobilePlatform/FW_2nd_1229/images/bmp/QVGA_24bits.bmp"
#define IMG_RGB_CIF		"D:/MyDocument/AP/S3C6410/Software/MobilePlatform/FW_2nd_1229/images/bmp/CIF_24bits.bmp"
#define IMG_RGB_VGA	"D:/MyDocument/AP/S3C6410/Software/MobilePlatform/FW_2nd_1229/images/bmp/VGA_24bits.bmp"
#define IMG_RGB_WIDE	"D:/MyDocument/AP/S3C6410/Software/MobilePlatform/FW_2nd_1229/images/bmp/WIDE_24bits.bmp"

#define IMG_420_QVGA	"D:/MyDocument/AP/S3C6410/Software/MobilePlatform/FW_2nd_1229/images/ycbycr/QVGA_420.yuv"
#define IMG_420_CIF		"D:/MyDocument/AP/S3C6410/Software/MobilePlatform/FW_2nd_1229/images/ycbycr/CIF_420.yuv"
#define IMG_420_VGA		"D:/MyDocument/AP/S3C6410/Software/MobilePlatform/FW_2nd_1229/images/ycbycr/VGA_420.yuv"
#define IMG_420_WIDE	"D:/MyDocument/AP/S3C6410/Software/MobilePlatform/FW_2nd_1229/images/ycbycr/WIDE_420.yuv"

#define IMG_422_QCIF	"D:/MyDocument/AP/S3C6410/Software/MobilePlatform/FW_2nd_1229/images/ycbycr/QCIF_422.yuv"
#define IMG_422_QVGA	"D:/MyDocument/AP/S3C6410/Software/MobilePlatform/FW_2nd_1229/images/ycbycr/QVGA_422.yuv"
#define IMG_422_CIF		"D:/MyDocument/AP/S3C6410/Software/MobilePlatform/FW_2nd_1229/images/ycbycr/CIF_422.yuv"
#define IMG_422_VGA		"D:/MyDocument/AP/S3C6410/Software/MobilePlatform/FW_2nd_1229/images/ycbycr/VGA_422.yuv"
#define IMG_422_WIDE	"D:/MyDocument/AP/S3C6410/Software/MobilePlatform/FW_2nd_1229/images/ycbycr/WIDE_422.yuv"
#endif

#define TVSIZE_NTSC		(0)
#define TVSIZE_PAL		(1)
#define TV_MUTE_STEP		(0x8)

static POST oPost;
static POST oSc;

static u32 uConvertAddr;
u32 uImgAddr;
static u32 uGlibStAddr;
static u32 uImgAddr1;

static u32 uImgXsize;
static u32 uImgYsize;
CSPACE uImgResolution;

static const char* pImg_RGB;
static const char* pImg_YUV;

static eTV_CONN_TYPE eConnType;
static TV_STANDARDS eSigType;

static u32 uTvSizeFormat = TVSIZE_NTSC;

typedef struct
{
	const char * pDesc;
	TV_STANDARDS eType;
} TV_TYPE;

typedef enum
{
	eTV_UpPara, eTV_DownPara, eTV_ExitPara
}TVT_UPDOWN;

	
void __irq Isr_FifoUnderrun(void)
{
	UART_Printf("@Isr_FifoUnderrun\n");

	TVENC_ClearUnderrunInt();
	INTC_ClearVectAddr();
}

void __irq Isr_CamPview(void)
{
	u32 uFrmNum;

//	UART_Printf("@Isr_CamPview\n");

	CAMERA_GetFrameNum(P_PATH, &uFrmNum);

	if (uFrmNum == 0 || uFrmNum == 2)
		POST_SetNextFrameStAddr(uImgAddr, 0, &oPost);
	else
		POST_SetNextFrameStAddr(uImgAddr1, 0, &oPost);

	LCD_Trigger();

	//INTC_ClearPending(INT_CAMIF_P);
	CAMERA_SetClearPreviewInt();
	INTC_ClearVectAddr();
}


void __irq Isr_CamPview_Scaler(void)
{
	u32 uFrmNum;
	
	CAMERA_GetFrameNum(P_PATH, &uFrmNum);

	if (uFrmNum == 0 || uFrmNum == 2)
		POST_SetNextFrameStAddr(uImgAddr, 0, &oSc);
	else
		POST_SetNextFrameStAddr(uImgAddr1, 0, &oSc);
	LCD_Trigger();
	
	CAMERA_SetClearPreviewInt();
	INTC_ClearVectAddr();
}


void TVENCT_SelectClockDivider(void)
{
	u32 uPostClockDivide;
	
	UART_Printf("[TVENCT_SelectClockDivider]\n");

	UART_Printf("\n");
	UART_Printf("Select the source clock [0x01 ~ 0x3F] : ");
	
	uPostClockDivide = (u32)UART_GetIntNum();

	POST_SetClockDivide(uPostClockDivide, &oSc);
}


static void TVENCT_MakeImage(void)
{
	u32 sel;
	u32 sel_image;

	uImgAddr = CODEC_MEM_ST;

	UART_Printf("What do you want to display Image format, RGB or YUV ?\n");
	UART_Printf("[0] : RGB\n");
	UART_Printf("[1] : YUV\n");
	UART_Printf("Select number : ");
	sel = UART_GetIntNum();
	//sel = 0;
	
	if (sel == 1)
	{
		UART_Printf("\nSelect Image..\n");
		UART_Printf("[0] : YCbCr420 QCIF [176x144] Non-Interleave\n");
		UART_Printf("[1] : YCbCr420 QVGA [320x240] Non-Interleave\n");
		UART_Printf("[2] : YCbCr420 CIF  [352x288] Non-Interleave\n");
		UART_Printf("[3] : YCbCr420 VGA  [640x480] Non-Interleave\n");
		UART_Printf("[4] : YCbCr420 WIDE [720x480] Non-Interleave\n");
		UART_Printf("[5] : YCbCr422 QCIF [176x144] Interleave\n");
		UART_Printf("[6] : YCbCr422 QVGA [320x240] Interleave\n");
		UART_Printf("[7] : YCbCr422 CIF  [352x288] Interleave\n");
		UART_Printf("[8] : YCbCr422 VGA  [640x480] Interleave\n");
		UART_Printf("[9] : YCbCr422 WIDE [720x480] Interleave\n");
		UART_Printf("Select number : ");
		sel_image = UART_GetIntNum();

		if (sel_image < 4)
			uImgResolution = YC420;
		else
			uImgResolution = YCBYCR;
		
		switch (sel_image % 5)
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
		}

		uGlibStAddr = uImgAddr + uImgXsize*uImgYsize*2;

		GLIB_InitInstance(uGlibStAddr, uImgXsize, uImgYsize, RGB24);
		GLIB_DrawPattern(uImgXsize, uImgYsize);
		
		POST_InitIp(uImgXsize, uImgYsize, uGlibStAddr, RGB24, 
					uImgXsize, uImgYsize, uImgAddr, uImgResolution,
					1, false, ONE_SHOT, &oPost);
		POST_StartProcessing(&oPost);
		while (!POST_IsProcessingDone(&oPost));

	}
	else
	{
		UART_Printf("\nSelect Image\n");
		UART_Printf("[0] : RGB16 QCIF [176x144]\n");
		UART_Printf("[1] : RGB16 QVGA [320x240]\n");
		UART_Printf("[2] : RGB16 CIF  [352x288]\n");
		UART_Printf("[3] : RGB16 VGA  [640x480]\n");
		UART_Printf("[4] : RGB16 WIDE [720x480]\n");
		UART_Printf("[5] : RGB24 QCIF [176x144]\n");
		UART_Printf("[6] : RGB24 QVGA [320x240]\n");
		UART_Printf("[7] : RGB24 CIF  [352x288]\n");
		UART_Printf("[8] : RGB24 VGA  [640x480]\n");
		UART_Printf("[9] : RGB24 WIDE [720x480]\n");
		UART_Printf("[10] : RGB24 WIDE [800x480]\n");
		UART_Printf("[11] : RGB24 WQVGA [480x272]\n");
		UART_Printf("Select number : ");

		sel_image=UART_GetIntNum();
		//sel_image = 1;
		
		if (sel_image >= 5)
			uImgResolution = RGB24;
		else
			uImgResolution = RGB16;

		switch (sel_image)
		{
			case 0:
			case 5:
				uImgXsize = 176, uImgYsize = 144;
				break;
			case 1:
			case 6:
				uImgXsize = 320, uImgYsize = 240;
				break;
			case 2:
			case 7:
				uImgXsize = 352, uImgYsize = 288;
				break;
			case 3:
			case 8:
				uImgXsize = 640, uImgYsize = 480;
				break;
			case 4:
			case 9:
				uImgXsize = 720, uImgYsize = 480;
				break;
			case 10:
				uImgXsize = 800, uImgYsize = 480;
				break;
			case 11:
				uImgXsize = 720, uImgYsize = 360;	
				break;
		}

		GLIB_InitInstance(uImgAddr, uImgXsize, uImgYsize, uImgResolution);
		GLIB_DrawPattern(uImgXsize, uImgYsize);
	}
}


static void TVENCT_GetTVFullSize(u32 *uWidth, u32 *uHeight)
{
	if(uTvSizeFormat == TVSIZE_PAL)
	{
		*uWidth = 720;
		*uHeight = 576;
	}
	else if(uTvSizeFormat == TVSIZE_NTSC)
	{
		*uWidth = 720;
		*uHeight = 480;
	}
	else
	{
		UART_Printf("TV Output Type Error....!!(Press any key)\n");
		UART_Getc();
	}
}


static void TVENCT_SetTVParameter(void)
{
	s32 sSel;
	u8 ucTemp;
	u32 uValue, uSelColor; 
	eMACROPATTERN ePattern;

	while(1)
	{
		UART_Printf("=====================================================\n");
		UART_Printf("[TVENCT_SetTVParameter]\n");
		UART_Printf("\n");
		UART_Printf("0 : MacroVision Enable\n");
		UART_Printf("1 : MacroVision Disable\n");
		UART_Printf("2 : Gamma Control\n");
		UART_Printf("3 : Mute Control\n");
		UART_Printf("4 : HUE Control\n");
		UART_Printf("5 : Contrast Gain Control\n");
		UART_Printf("6 : Cb Gain Control\n");
		UART_Printf("7 : Cr Gain Control\n");
		UART_Printf("8 : Brightness Gain Control\n");	
		UART_Printf("9 : Vertical/Horizontal Active window center\n");	
		UART_Printf("10 : Enhancer Demo Window\n");	
		UART_Printf("11 : Background\n");
		UART_Printf("12 : Sharpness\n");
		UART_Printf("=====================================================\n");
		UART_Printf("Select ('Enter' to exit) : ");	

		sSel = UART_GetIntNum();

		if(sSel == -1)
			break;

		switch(sSel)
		{
			case 0	: 	if( (eSigType == NTSC_M) || (eSigType == NTSC_J) || (eSigType == PAL_M) || (eSigType == NTSC_443) )
							ePattern = eAGC4L;
						else if( (eSigType == PAL_NC) || (eSigType == PAL_N) || (eSigType == PAL_BGHID) )
							ePattern = eP01;
						
						TVENC_EnableMacroVision(eSigType, ePattern);
						UART_Printf("MacroVision is enabled\n");
						break;
						
			case 1	: 	TVENC_DisableMacroVision();
						UART_Printf("MacroVision is disabled\n");
						break;	
						
			case 2 	: 	TVENC_EnableGammaControl(TRUE);
						UART_Printf("\n");
						UART_Printf("'u' : Up,  'd' : Down\n");
						UART_Printf("'q' key to exit\n");
						UART_Printf("\n");
						while(1)
						{	
							uValue = TVENC_GetGammaGain();						
							UART_Printf("Current Gamma gain value[0~3] : %d\n", uValue);
							ucTemp = UART_Getc();
							
							if( (ucTemp == 'u') && (uValue < 3) )
								TVENC_SetGammaGain(uValue+1);
							else if ( (ucTemp == 'd') && (uValue > 0) )
								TVENC_SetGammaGain(uValue-1);
							else if(ucTemp == 'q')
								break;
						}
						TVENC_EnableGammaControl(FALSE);
						break;
						
			case 3	:	TVENC_EnableMuteControl(TRUE);
						UART_Printf("\n");
						UART_Printf("'w' : Mute Y Up,  's' : Mute Y Down\n");
						UART_Printf("'e' : Mute Cb Up,  'd' : Mute Cb Down\n");
						UART_Printf("'r' : Mute Cr Up,  'f' : Mute Cr Down\n");	
						UART_Printf("'q' key to exit\n");
						UART_Printf("\n");

						while(1)
						{	
							u32 uY, uCb, uCr; 
						
							TVENC_GetMuteYCbCr(&uY, &uCb, &uCr);						
							UART_Printf("Current Mute Component value : [ Y:0x%02x ]  [ Cb:0x%02x ]  [ Cr:0x%02x ]\n", uY, uCb, uCr);
							ucTemp = UART_Getc();

							if( (ucTemp == 'w') && (uY < (0xFF-TV_MUTE_STEP)) )
								TVENC_SetMuteYCbCr(uY+TV_MUTE_STEP, uCb, uCr);
							else if( (ucTemp == 's') && (uY >= TV_MUTE_STEP) )
								TVENC_SetMuteYCbCr(uY-TV_MUTE_STEP, uCb, uCr);
							else if( (ucTemp == 'e') && (uCb < (0xFF-TV_MUTE_STEP)) )
								TVENC_SetMuteYCbCr(uY, uCb+TV_MUTE_STEP, uCr);
							else if( (ucTemp == 'd') && (uCb >= TV_MUTE_STEP) )
								TVENC_SetMuteYCbCr(uY, uCb-TV_MUTE_STEP, uCr);
							else if( (ucTemp == 'r') && (uCr < (0xFF-TV_MUTE_STEP)) )
								TVENC_SetMuteYCbCr(uY, uCb, uCr+TV_MUTE_STEP);
							else if( (ucTemp == 'f') && (uCr >= TV_MUTE_STEP) )
								TVENC_SetMuteYCbCr(uY, uCb, uCr-TV_MUTE_STEP);
							else if(ucTemp == 'q')
								break;							
						}
						TVENC_EnableMuteControl(FALSE);
						break;
						
			case 4	:	UART_Printf("\n");
						UART_Printf("'u' : Up,  'd' : Down\n");
						UART_Printf("'q' key to exit\n");
						UART_Printf("\n");
						
						while(1)
						{	
							uValue = TVENC_GetHuePhase();	
							UART_Printf("Current HUE Phase value[0~0xFF] : 0x%02x\n", uValue);
							ucTemp = UART_Getc();
							
							if( (ucTemp == 'u') && (uValue < 0xFF) )
								TVENC_SetHuePhase(uValue+1);
							else if ( (ucTemp == 'd') && (uValue > 0) )
								TVENC_SetHuePhase(uValue-1);
							else if(ucTemp == 'q')
								break;
						}
						break;
			case 5	:	UART_Printf("\n");
						UART_Printf("'u' : Up,  'd' : Down\n");
						UART_Printf("'q' key to exit\n");
						UART_Printf("\n");
						
						while(1)
						{	
							uValue = TVENC_GetContrast();	
							UART_Printf("Current Contrast Gain value[0~0xFF] : 0x%02x\n", uValue);
							ucTemp = UART_Getc();
							
							if( (ucTemp == 'u') && (uValue < 0xFF) )
								TVENC_SetContrast(uValue+1);
							else if ( (ucTemp == 'd') && (uValue > 0) )
								TVENC_SetContrast(uValue-1);
							else if(ucTemp == 'q')
								break;
						}
						break;	
			case 6	:	UART_Printf("\n");
						UART_Printf("'u' : Up,  'd' : Down\n");
						UART_Printf("'q' key to exit\n");
						UART_Printf("\n");
						
						while(1)
						{	
							uValue = TVENC_GetCbGain();	
							UART_Printf("Current Cb Gain value[0~0xFF] : 0x%02x\n", uValue);
							ucTemp = UART_Getc();
							
							if( (ucTemp == 'u') && (uValue < 0xFF) )
								TVENC_SetCbGain(uValue+1);
							else if ( (ucTemp == 'd') && (uValue > 0) )
								TVENC_SetCbGain(uValue-1);
							else if(ucTemp == 'q')
								break;
						}
						break;		
			case 7	:	UART_Printf("\n");
						UART_Printf("'u' : Up,  'd' : Down\n");
						UART_Printf("'q' key to exit\n");
						UART_Printf("\n");
						
						while(1)
						{	
							uValue = TVENC_GetCrGain();	
							UART_Printf("Current Cr Gain value[0~0xFF] : 0x%02x\n", uValue);
							ucTemp = UART_Getc();
							
							if( (ucTemp == 'u') && (uValue < 0xFF) )
								TVENC_SetCrGain(uValue+1);
							else if ( (ucTemp == 'd') && (uValue > 0) )
								TVENC_SetCrGain(uValue-1);
							else if(ucTemp == 'q')
								break;
						}
						break;
						
			case 8	:	UART_Printf("\n");
						UART_Printf("'u' : Up,  'd' : Down\n");
						UART_Printf("'q' key to exit\n");
						UART_Printf("\n");
						
						while(1)
						{	
							uValue = TVENC_GetBright();	
							UART_Printf("Current Brightness Gain value[0~0xFF] : 0x%02x\n", uValue);
							ucTemp = UART_Getc();
							
							if( (ucTemp == 'u') && (uValue < 0xFF) )
								TVENC_SetBright(uValue+1);
							else if ( (ucTemp == 'd') && (uValue > 0) )
								TVENC_SetBright(uValue-1);
							else if(ucTemp == 'q')
								break;
						}
						break;	
						
			case 9	:	UART_Printf("\n");
						UART_Printf("'w' : Vertical Up,  's' : Vertical Down\n");
						UART_Printf("'e' : Horizontal Up,  'd' : Horizontal Down\n");
						UART_Printf("'q' key to exit\n");
						UART_Printf("\n");
						while(1)
						{	
							u32 uVer, uHor;
							
							TVENC_GetActiveWinCenter(&uVer, &uHor);	
							UART_Printf("Current Active Window Center : [%d, %d]\n", uVer, uHor);
							ucTemp = UART_Getc();
							
							if( (ucTemp == 'w') && (uVer < 0x3F) )
								TVENC_SetActiveWinCenter(uVer+1, uHor);
							else if ( (ucTemp == 's') && (uVer > 0) )
								TVENC_SetActiveWinCenter(uVer-1, uHor);
							else if ( (ucTemp == 'e') && (uHor < 0xFF) )
								TVENC_SetActiveWinCenter(uVer, uHor+1);
							else if ( (ucTemp == 'd') && (uHor > 0) )
								TVENC_SetActiveWinCenter(uVer, uHor-1);
							else if(ucTemp == 'q')
								break;
						}
						break;	

			case 10	:	TVENC_EnableEnhancerDemoWindow(TRUE);
						UART_Printf("\n");
						UART_Printf("'w' : Vertical Start Point Up,  	's' : Vertical Start Point Down\n");
						UART_Printf("'e' : Horizontal Start Point Up,  	'd' : Horizontal Start Point Down\n");
						UART_Printf("'r' : Vertical Win_Size Up,  		'f' : Vertical Win_Size Down\n");
						UART_Printf("'t' : Horizontal Win_Size Up,  	'g' : Horizontal Win_Size Down\n");						
						UART_Printf("'q' key to exit\n");
						UART_Printf("\n");
						while(1)
						{	
							u32 uVerSize, uHorSize, uVStart, uHStart;
							
							TVENC_GetEnhancerDemoWindow(&uVerSize, &uVStart, &uHorSize, &uHStart);	
							UART_Printf("Current Demo Window Area : [%d, %d] ~ [%d, %d]\n", uHStart, uVStart, uHorSize, uVerSize);
							ucTemp = UART_Getc();
							
							if( (ucTemp == 'w') && (uVStart < 0x1FF) )
								TVENC_SetEnhancerDemoWindow(uVerSize, uVStart+5, uHorSize, uHStart);
							else if ( (ucTemp == 's') && (uVStart > 0) )
								TVENC_SetEnhancerDemoWindow(uVerSize, uVStart-5, uHorSize, uHStart);
							else if ( (ucTemp == 'e') && (uHStart < 0x7FF) )
								TVENC_SetEnhancerDemoWindow(uVerSize, uVStart, uHorSize, uHStart+5);
							else if ( (ucTemp == 'd') && (uHStart > 0) )
								TVENC_SetEnhancerDemoWindow(uVerSize, uVStart, uHorSize, uHStart-5);
							else if( (ucTemp == 'r') && (uVerSize < 0x1FF) )
								TVENC_SetEnhancerDemoWindow(uVerSize+5, uVStart, uHorSize, uHStart);
							else if ( (ucTemp == 'f') && (uVerSize > 0) )
								TVENC_SetEnhancerDemoWindow(uVerSize-5, uVStart, uHorSize, uHStart);
							else if ( (ucTemp == 't') && (uHorSize < 0x7FF) )
								TVENC_SetEnhancerDemoWindow(uVerSize, uVStart, uHorSize+5, uHStart);
							else if ( (ucTemp == 'g') && (uHorSize > 0) )
								TVENC_SetEnhancerDemoWindow(uVerSize, uVStart, uHorSize-5, uHStart);							
							else if(ucTemp == 'q')
								break;
						}
						//TVENC_SetEnhancerDemoWindow(240, 0, 1440, 0);
						//TVENC_EnableEnhancerDemoWindow(FALSE);
						break;	

			case 11	:	UART_Printf("\n");
						UART_Printf("Select the Background Color[0~7]\n");
						uSelColor = (u32)UART_GetIntNum();
						
						TVENC_EnableBackground(TRUE);
						UART_Printf("\n");
						UART_Printf("'w' : Vertical Start Point Up,  	's' : Vertical Start Point Down\n");
						UART_Printf("'e' : Horizontal Start Point Up,  	'd' : Horizontal Start Point Down\n");
						UART_Printf("'r' : Vertical Visual_Size Up,  	'f' : Vertical Visual_Size Down\n");
						UART_Printf("'t' : Horizontal Visual_Size Up,  	'g' : Horizontal Visual_Size Down\n");						
						UART_Printf("'q' key to exit\n");
						UART_Printf("\n");
						while(1)
						{	
							u32 uOffset, uCurrColor, uVerSize, uHorSize, uVStart, uHStart;

							uOffset = 4;
							TVENC_GetBackground(&uCurrColor, &uHStart, &uVStart, &uHorSize, &uVerSize);	
							UART_Printf("Current Background Color : %d\n", uCurrColor);
							UART_Printf("Current Visual Window Area : [%d, %d] ~ [%d, %d]\n", uHStart*8, uVStart, uHorSize*8, uVerSize);
							ucTemp = UART_Getc();
							
							if( (ucTemp == 'w') && (uVStart < 0xFF) )
								TVENC_SetBackground(uSelColor, uHStart, uVStart+uOffset, uHorSize, uVerSize);
							else if ( (ucTemp == 's') && (uVStart > 0) )
								TVENC_SetBackground(uSelColor, uHStart, uVStart-uOffset, uHorSize, uVerSize);
							else if ( (ucTemp == 'e') && (uHStart < 0xFF) )
								TVENC_SetBackground(uSelColor, uHStart+uOffset, uVStart, uHorSize, uVerSize);
							else if ( (ucTemp == 'd') && (uHStart > 0) )
								TVENC_SetBackground(uSelColor, uHStart-uOffset, uVStart, uHorSize, uVerSize);
							else if( (ucTemp == 'r') && (uVerSize < 0xFF) )
								TVENC_SetBackground(uSelColor, uHStart, uVStart, uHorSize, uVerSize+uOffset);
							else if ( (ucTemp == 'f') && (uVerSize > 0) )
								TVENC_SetBackground(uSelColor, uHStart, uVStart, uHorSize, uVerSize-uOffset);
							else if ( (ucTemp == 't') && (uHorSize < 0xFF) )
								TVENC_SetBackground(uSelColor, uHStart, uVStart, uHorSize+uOffset, uVerSize);
							else if ( (ucTemp == 'g') && (uHorSize > 0) )
								TVENC_SetBackground(uSelColor, uHStart, uVStart, uHorSize-uOffset, uVerSize);							
							else if(ucTemp == 'q')
								break;
						}
						TVENC_SetBackground(1, 0, 0, 180, 240);
						//TVENC_EnableBackground(FALSE);	//default enable
						break;

			case 12	:	UART_Printf("\n");
						UART_Printf("'u' : Up,  'd' : Down\n");
						UART_Printf("'q' key to exit\n");
						UART_Printf("\n");
						
						while(1)
						{	
							uValue = TVENC_GetSharpness();	
							UART_Printf("Current Sharpness Gain value[0~0x3F] : 0x%02x\n", uValue);
							ucTemp = UART_Getc();
							
							if( (ucTemp == 'u') && (uValue < 0x3F) )
								TVENC_SetSharpness(uValue+1);
							else if ( (ucTemp == 'd') && (uValue > 0) )
								TVENC_SetSharpness(uValue-1);
							else if(ucTemp == 'q')
								break;
						}
						break;
						
			default : 	break;

		}
	}


}


static void TVENCT_LoadImageToMemory(void)
{
	u32 uSel;
	u32 uSel_image;

	uImgAddr = CODEC_MEM_ST;

	UART_Printf("What do you want to display Image format, RGB or YUV ?\n");
	UART_Printf("[0] : RGB\n");
	UART_Printf("[1] : YUV\n");
	UART_Printf("[2] : RGB24 CIF COLORBAR\n");
	UART_Printf("Select number : ");
	uSel = UART_GetIntNum();

	if (uSel == 1)
	{
		UART_Printf("Select Image..\n");
		UART_Printf("[0] : YCbCr420 QCIF [176x144] (Not ready)\n");
		UART_Printf("[1] : YCbCr420 QVGA [320x240]\n");
		UART_Printf("[2] : YCbCr420 CIF  [352x288]\n");
		UART_Printf("[3] : YCbCr420 VGA  [640x480]\n");
		UART_Printf("[4] : YCbCr420 WIDE [720x480]\n");
		UART_Printf("[5] : YCbCr422 QCIF [176x144]\n");
		UART_Printf("[6] : YCbCr422 QVGA [320x240]\n");
		UART_Printf("[7] : YCbCr422 CIF  [352x288]\n");
		UART_Printf("[8] : YCbCr422 VGA  [640x480]\n");
		UART_Printf("[9] : YCbCr422 WIDE [720x480]\n");

		uSel = UART_GetIntNum();

		if(uSel > 4)
			uImgResolution = YCBYCR;
		else
			uImgResolution = YC420;

		switch(uSel)
		{
			case 0:
//				pImg_YUV = IMG_420_QCIF; // not ready image
				uImgXsize = 176, uImgYsize = 144;
				break;
			case 1:
				pImg_YUV = IMG_420_QVGA;
				uImgXsize = 320, uImgYsize = 240;
				break;
			case 2:
				pImg_YUV = IMG_420_CIF;
				uImgXsize = 352, uImgYsize = 288;
				break;
			case 3:
				pImg_YUV = IMG_420_VGA;
				uImgXsize = 640, uImgYsize = 480;
				break;
			case 4:
				pImg_YUV = IMG_420_WIDE;
				uImgXsize = 720, uImgYsize = 480;
				break;
			case 5:
				pImg_YUV = IMG_422_QCIF;
				uImgXsize = 176, uImgYsize = 144;
				break;
			case 6:
				pImg_YUV = IMG_422_QVGA;
				uImgXsize = 320, uImgYsize = 240;
				break;
			case 7:
				pImg_YUV = IMG_422_CIF;
				uImgXsize = 352, uImgYsize = 288;
				break;
			case 8:
				pImg_YUV = IMG_422_VGA;
				uImgXsize = 640, uImgYsize = 480;
				break;
			case 9:
				pImg_YUV = IMG_422_WIDE;
				uImgXsize = 720, uImgYsize = 480;
				break;
		}
		LoadFromFile(pImg_YUV, CODEC_MEM_ST);

	}
	else if (uSel == 2)
	{
		uImgResolution = RGB24;

		uImgXsize = 352;
		uImgYsize = 288;

		pImg_RGB = IMG_CIF_CBAR;
		LoadFromFile(pImg_RGB, CODEC_MEM_ST);
	}
	else
	{
		UART_Printf("\nSelect Image\n");
		UART_Printf("[0] : RGB16 QCIF [176x144]\n");
		UART_Printf("[1] : RGB16 QVGA [320x240]\n");
		UART_Printf("[2] : RGB16 CIF  [352x288]\n");
		UART_Printf("[3] : RGB16 VGA  [640x480]\n");
		UART_Printf("[4] : RGB16 WIDE [720x480]\n");
		UART_Printf("[5] : RGB24 QCIF [176x144]\n");
		UART_Printf("[6] : RGB24 QVGA [320x240]\n");
		UART_Printf("[7] : RGB24 CIF  [352x288]\n");
		UART_Printf("[8] : RGB24 VGA  [640x480]\n");
		UART_Printf("[9] : RGB24 WIDE [720x480]\n");
		UART_Printf("Select number : ");

		uSel_image=UART_GetIntNum();

		if (uSel_image > 4)
			uImgResolution = RGB24;
		else
			uImgResolution = RGB16;

		switch (uSel_image % 5)
		{
			case 0:
				uImgXsize = 176, uImgYsize = 144;
				pImg_RGB = IMG_RGB_QCIF;
				break;
			case 1:
				uImgXsize = 320, uImgYsize = 240;
				pImg_RGB = IMG_RGB_QVGA;
				break;
			case 2:
				uImgXsize = 352, uImgYsize = 288;
				pImg_RGB = IMG_RGB_CIF;
				break;
			case 3:
				uImgXsize = 640, uImgYsize = 480;
				pImg_RGB = IMG_RGB_VGA;
				break;
			case 4:
				uImgXsize = 720, uImgYsize = 480;
				pImg_RGB = IMG_RGB_WIDE;
				break;
		}

		uConvertAddr = uImgAddr + uImgXsize*uImgYsize*4;

		LoadFromFile(pImg_RGB, uConvertAddr);

		if(uImgResolution==RGB24)
			ConvertBmpToRgb24bpp(uConvertAddr, uImgAddr, uImgXsize, uImgYsize);
		else if(uImgResolution==RGB16)
			ConvertBmpToRgb16bpp(uConvertAddr, uImgAddr, uImgXsize, uImgYsize);
	}

}




static void TVENCT_SelectTVOutputFormat(void) // TV controller class
{
	u32 uFormatNum, uSelFormat, uSelType;
	u32 i;
	TV_TYPE aTvOutFormat[] =
	{
		"NTSC-M", 				NTSC_M,
		"NTSC-J", 				NTSC_J,
		"PAL-B/D/G/H/I", 		PAL_BGHID,		
//		"PAL-N",    				PAL_N,
		"PAL-M",  				PAL_M,
		"PAL-Nc", 				PAL_NC	
	};

	UART_Printf("\n");
	UART_Printf("0. Composite Out(D)	1. S-Video Out \n");
	UART_Printf("Select the source clock : ");
	
	uSelType = UART_GetIntNum();
	//uSelType = 0;
	
	if(uSelType > 1)
		uSelType = 0;		//Default Composite out
	eConnType = (eTV_CONN_TYPE)uSelType;
	
	uFormatNum = sizeof(aTvOutFormat)/sizeof(TV_TYPE );
	for (i = 0; i < uFormatNum; i++)
		UART_Printf("[%d] : %s\n", i, aTvOutFormat[i].pDesc);
	
	UART_Printf("\nSelect the signal type : ");
	uSelFormat = (u32)UART_GetIntNum();
	//uSelFormat =  0;
	
	UART_Printf("\n");
	if ( (uSelFormat <= uFormatNum) )
		eSigType = aTvOutFormat[uSelFormat].eType;
	
	if( (eSigType == NTSC_M) || (eSigType == NTSC_J) || (eSigType == PAL_M) || (eSigType == NTSC_443) )
		uTvSizeFormat = TVSIZE_NTSC;
	else
		uTvSizeFormat = TVSIZE_PAL;
	
	TVENC_SetTvConMode(eSigType, eConnType);
}


//////////
// Function Name : Scaler_SelectClockDivider
// Function Description : Set the Scaler Clock Divider
// Input : 	None
// Output : 	None
void Scaler_SelectClockDivider(void)
{
	u32 uPostClockDivide;
	
	UART_Printf("[Scaler_SelectClockDivider]\n");

	UART_Printf("\n");
	UART_Printf("Select the source clock [0x01 ~ 0x3F] : ");
	
	uPostClockDivide = (u32)UART_GetIntNum();

	POST_SetClockDivide(uPostClockDivide, &oSc);
}


void TVENCT_Scaler_Enc_FromExtAPI(void)
{
	u32 uImageAddr;

	uImageAddr = CODEC_MEM_ST;
	
	GLIB_InitInstance(uImageAddr, 320, 240, RGB24);
	GLIB_DrawPattern(320, 240);
		
	TVENC_DisplayTVout(NTSC_M, eCOMPOSITE, 720, 480);
	POST_SetScalerforTVout(320, 240, uImageAddr, RGB24, 720, 480, &oSc);

}

extern void ScalerT_SelectClockSource(void);
void TVENCT_Scaler_Enc(void) // post controller initialize
{
	u32 uImgSz, uWidth, uHeight;

	UART_Printf("Select Image Size\n");
	UART_Printf("[0] : Original Image Size\n");
	UART_Printf("[1] : Full Screen size\n");
	UART_Printf("Select number : ");
	uImgSz = UART_GetIntNum();
	//uImgSz = 1;
		
	if(uImgSz)
	{
		TVENCT_GetTVFullSize(&uWidth, &uHeight);
	}
	else
	{
		uWidth = uImgXsize;
		uHeight = uImgYsize;
	}

	INTC_Enable(NUM_TVENC);
	INTC_SetVectAddr(NUM_TVENC, Isr_FifoUnderrun);

	TVENC_SetImageSize(uWidth*2, uHeight);
	TVENC_TurnOnOff(1);

	POST_InitIp1(	uImgXsize, uImgYsize, 0, 0, uImgXsize, uImgYsize, uImgAddr, uImgResolution,
					uWidth*2, uHeight, 0, 0, uWidth*2, uHeight, 0, YCBYCR,
					1, FALSE, FREE_RUN, POST_DMA, POST_FIFO, &oSc);
	POST_SetInterlaceModeOnOff(1, &oSc);
	POST_SetNextFrameStAddr(uImgAddr, 0, &oSc);
	POST_StartProcessing(&oSc);

	TVENCT_SetTVParameter();

	INTC_Disable(NUM_TVENC);

	POST_SetInterlaceModeOnOff(0, &oSc);
	POST_StopProcessingOfFreeRun(&oSc);
	while (!POST_IsProcessingDone(&oSc));

	TVENC_TurnOnOff(0);
}



static void TVENCT_FIMD_Scaler_Enc(void) // post controller initialize
{
	u32 uImgSz, uWidth, uHeight;

	UART_Printf("Select Image Size\n");
	UART_Printf("[0] : Original Image Size\n");
	UART_Printf("[1] : Full Screen size\n");
	UART_Printf("Select number : ");
	uImgSz = UART_GetIntNum();

	if(uImgSz)
	{
		TVENCT_GetTVFullSize(&uWidth, &uHeight);
	}
	else
	{
		uWidth = uImgXsize;
		uHeight = uImgYsize;
	}

	INTC_Enable(NUM_TVENC);
	INTC_SetVectAddr(NUM_TVENC, Isr_FifoUnderrun);
	
	// TV ENCODER 2.0
	TVENC_SetImageSize(uWidth*2, uHeight);
	TVENC_TurnOnOff(1);

	// SCALER 2.5
	POST_InitIpForFifoInFifoOut(	uImgXsize, uImgYsize, 
								uWidth*2, uHeight, YUV, FREE_RUN, &oSc);
	POST_SetInterlaceModeOnOff(1, &oSc);
	POST_StartProcessing(&oSc);

	// FIMD 4.0
	LCD_InitDISPC1(uImgXsize, uImgYsize, uImgResolution, uImgAddr, WIN0, false);
	LCD_SetOutputPath(LCD_TV);
	LCD_Start();

	TVENCT_SetTVParameter();

	INTC_Disable(NUM_TVENC);

	// oScaler die..
	POST_SetInterlaceModeOnOff(0, &oSc);
	POST_StopProcessingOfFreeRun(&oSc);
	while (!POST_IsProcessingDone(&oSc));

	// oLcdc die..
	LCD_Stop();

	// oTvenc die..
	TVENC_TurnOnOff(0);
}


static void TVENCT_POST_FIMD_Scaler_Enc(void)
{
	u32 uImgSz, uWidth, uHeight, uTestItem;
	u32 uLcdHsz, uLcdVsz;

	UART_Printf("Select Image Size\n");
	UART_Printf("[0] : Original Image Size\n");
	UART_Printf("[1] : Full Screen size\n");
	UART_Printf("Select number : ");
	uImgSz = UART_GetIntNum();

	if(uImgSz)
	{
		TVENCT_GetTVFullSize(&uWidth, &uHeight);
	}
	else
	{
		uWidth = uImgXsize;
		uHeight = uImgYsize;
	}

	uTestItem = 1;
	
	INTC_Enable(NUM_TVENC);
	INTC_SetVectAddr(NUM_TVENC, Isr_FifoUnderrun);
	
	// TV ENCODER 2.0
	TVENC_SetImageSize(uWidth*2, uHeight);
	TVENC_TurnOnOff(1);

	// FIMD 4.0
	if(uTestItem == 0)
		LCD_InitBase1(uImgXsize, uImgYsize);
	else if(uTestItem == 1)
		LCD_InitBase();
	LCD_InitWinForFifoIn(IN_POST, WIN0);
	LCD_SetWinOnOff(1, WIN0);	
	LCD_SetOutputPath(LCD_TV);
	LCD_GetFrmSz(&uLcdHsz, &uLcdVsz, WIN0);
	
	//Scaler 2.5	
	if(uTestItem == 0)
		POST_InitIpForFifoInFifoOut(	uImgXsize, uImgYsize, uWidth*2, uHeight, YUV, FREE_RUN, &oSc);
	else if(uTestItem == 1)
		POST_InitIpForFifoInFifoOut(	uLcdHsz, uLcdVsz, uWidth*2, uHeight, YUV, FREE_RUN, &oSc);
	
	POST_SetInterlaceModeOnOff(1, &oSc);
	POST_StartProcessing(&oSc);

	LCD_Start();

	// POST 2.5
	if(uTestItem == 0)
		POST_InitIpForDmaInFifoOut(uImgXsize, uImgYsize, uImgAddr, uImgResolution, 
									uImgXsize, uImgYsize, RGB, 1, &oPost);
	else if(uTestItem == 1)
		POST_InitIpForDmaInFifoOut(uImgXsize, uImgYsize, uImgAddr, uImgResolution, 
									uLcdHsz, uLcdVsz, RGB, 1, &oPost);
	POST_SetNextFrameStAddr(uImgAddr, 0, &oPost);
	POST_StartProcessing(&oPost);

	while(UART_Getc() != 'q')
	{
		u32 uSelclkval;

		UART_Printf("\n");
		UART_Printf("Select the source clock [0x01 ~ 0x3F] : ");
	
		uSelclkval = (u32)UART_GetIntNum();
		LCD_SetClkVal(uSelclkval);
	}

	TVENCT_SetTVParameter();

	INTC_Disable(NUM_TVENC);

	// oPost die..
	POST_StopProcessingOfFreeRun(&oPost);
	Delay(1000);

	// oScaler die..
	POST_SetInterlaceModeOnOff(0, &oSc);
	POST_StopProcessingOfFreeRun(&oSc);
	Delay(1000);

	// oLcdc die..
	LCD_Stop();

	// oTvenc die..
	TVENC_TurnOnOff(0);
}


static void TVENCT_Dual_POST_CPUFIMD_Scaler_Enc(void)
{
	u32 uImgSz, uWidth, uHeight;
	u32 uLcdHsz, uLcdVsz;

	UART_Printf("Select Image Size\n");
	UART_Printf("[0] : Original Image Size\n");
	UART_Printf("[1] : Full Screen size\n");
	UART_Printf("Select number : ");
	uImgSz = UART_GetIntNum();

	uLcdHsz = 240; uLcdVsz = 320;

	if(uImgSz)
	{
		TVENCT_GetTVFullSize(&uWidth, &uHeight);
	}
	else
	{
		uWidth = uLcdHsz;
		uHeight = uLcdVsz;
	}
	
	INTC_Enable(NUM_TVENC);
	INTC_SetVectAddr(NUM_TVENC, Isr_FifoUnderrun);
	
	// TV ENCODER 2.0
	TVENC_SetImageSize(uWidth*2, uHeight);
	TVENC_TurnOnOff(1);

	// POST 2.5
	POST_InitIpForFifoInFifoOut(uLcdHsz, uLcdVsz, uWidth*2, uHeight, YUV, ONE_SHOT, &oSc);
	POST_SetAutoLoadEnable(FREE_RUN, &oSc);
	POST_SetInterlaceModeOnOff(1, &oSc);
	//POST_SetInFifoMode(POST_FIFO, &oSc);
	POST_StartProcessing(&oSc);

	// FIMD 4.0
	//LCD_InitLDI(MAIN);
	LCD_InitBase();
	LCD_InitWinForFifoIn(IN_POST, WIN0);
	LCD_SetWinOnOff(1, WIN0);
	LCD_SetOutputPath(LCD_TVI80F);
	LCD_SetI80Timing(0, 1, 4, 0, MAIN); // cs_setup/wr_setup/wr_act/wr_hold at 40MHz HCLK
	LCD_Start();

	// POST 2.5
	POST_InitIpForDmaInFifoOut(uImgXsize, uImgYsize, uImgAddr, uImgResolution, uLcdHsz, uLcdVsz, RGB, 1, &oPost);
	POST_SetNextFrameStAddr(uImgAddr, 0, &oPost);
	POST_SetAutoLoadEnable(FREE_RUN, &oPost);
	POST_SetInterlaceModeOnOff(0, &oPost);
	//POST_SetInFifoMode(POST_DMA, &oPost);
	POST_StartProcessing(&oPost);

	LCD_Trigger();

	TVENCT_SetTVParameter();
	//UART_Printf("Press any key to end..\n");
	//UART_Getc();

	//INTC_Mask(INT_TVENC0);
	INTC_Disable(NUM_TVENC);
	
	// oPost die..
	POST_StopProcessingOfFreeRun(&oPost);
	Delay(1000);

	// oScaler die..
	POST_SetInterlaceModeOnOff(0, &oSc);
	POST_StopProcessingOfFreeRun(&oSc);
	Delay(1000);

	// oLcdc die..
	LCD_Stop();

	// oTvenc die..
	TVENC_TurnOnOff(0);
}

static void TVENCT_Dual_POST_RGBFIMD_Scaler_Enc(void)
{
	u32 uImgSz, uWidth, uHeight;
	u32 uLcdHsz, uLcdVsz;

	UART_Printf("Select Image Size\n");
	UART_Printf("[0] : Original Image Size\n");
	UART_Printf("[1] : Full Screen size\n");
	UART_Printf("Select number : ");
	uImgSz = UART_GetIntNum();

	// FIMD 4.0
	LCD_InitDISPCForFifoIn(IN_POST, WIN0);
	LCD_GetFrmSz(&uLcdHsz, &uLcdVsz, WIN0);
	LCD_SetWinOnOff(1, WIN0);
	LCD_SetOutputPath(LCD_TVRGB);
	//LCD_SetOutputPath(LCD_RGB);
	//LCD_SetOutputPath(LCD_TV);
	LCD_SetClkVal(4);
	
	if(uImgSz)
	{
		TVENCT_GetTVFullSize(&uWidth, &uHeight);
	}
	else
	{
		uWidth = uImgXsize;
		uHeight = uImgYsize;
	}
	
	// TV ENCODER 2.0
	INTC_Enable(NUM_TVENC);
	INTC_SetVectAddr(NUM_TVENC, Isr_FifoUnderrun);	
	TVENC_SetImageSize(uWidth*2, uHeight);
	TVENC_TurnOnOff(1);

	// POST 2.5
#if 0
	POST_InitIpForFifoInFifoOut(uLcdHsz, uLcdVsz, uWidth*2, uHeight, YUV, ONE_SHOT, &oSc);
	POST_SetAutoLoadEnable(FREE_RUN, &oSc);
#else
	POST_InitIpForFifoInFifoOut(	uLcdHsz, uLcdVsz, 
								uWidth*2, uHeight, YUV, FREE_RUN, &oSc);
#endif
	POST_SetInterlaceModeOnOff(1, &oSc);
	POST_StartProcessing(&oSc);

	LCD_Start();

	// POST 2.5
	POST_InitIpForDmaInFifoOut(uImgXsize, uImgYsize, uImgAddr, uImgResolution, uLcdHsz, uLcdVsz, RGB, 1, &oPost);
	POST_SetNextFrameStAddr(uImgAddr, 0, &oPost);
	POST_SetInterlaceModeOnOff(0, &oPost);
	POST_StartProcessing(&oPost);

	while(UART_Getc() != 'q')
	{
		u32 uSelclkval;

		UART_Printf("\n");
		UART_Printf("Select the source clock [0x01 ~ 0x3F] : ");
	
		uSelclkval = (u32)UART_GetIntNum();
		LCD_SetClkVal(uSelclkval);
	}
	
	TVENCT_SetTVParameter();

	INTC_Disable(NUM_TVENC);
	// oPost die..
	POST_StopProcessingOfFreeRun(&oPost);
	while (!POST_IsProcessingDone(&oPost));	

	// oScaler die..
	POST_SetInterlaceModeOnOff(0, &oSc);
	POST_StopProcessingOfFreeRun(&oSc);
	while (!POST_IsProcessingDone(&oPost));	
	//Delay(1000);

	// oLcdc die..
	LCD_SetWinOnOff(0, WIN0);	
	LCD_Stop();

	// oTvenc die..
	TVENC_TurnOnOff(0);
}

static void TVENCT_DualBuffer_POST_CPUFIMD_Scaler_Enc(void)
{
	u32 uImgSz, uWidth, uHeight;
	u32 uLcdHsz, uLcdVsz;

	uImgAddr = CODEC_MEM_ST;
	uImgAddr1 = CODEC_MEM_LMT;
	
	UART_Printf("Select Image Size\n");
	UART_Printf("[0] : Original Image Size\n");
	UART_Printf("[1] : Full Screen size\n");
	UART_Printf("Select number : ");
	uImgSz = UART_GetIntNum();

	uLcdHsz = 240; uLcdVsz = 320;

	if(uImgSz)
	{
		TVENCT_GetTVFullSize(&uWidth, &uHeight);
	}
	else
	{
		uWidth = uLcdHsz;
		uHeight = uLcdVsz;
	}
	
	//INTC_InitIp();
	
	// FIMC 3.0
	CAMERA_InitSensor();
	CAMERA_InitPreviewPath_1(uLcdHsz, uLcdVsz, uImgAddr, uImgAddr1, YCBYCR, FLIP_NO, ROT_0);
	CAMERA_StartPreviewPath();

	// TV ENCODER 2.0
	TVENC_SetImageSize(uWidth*2, uHeight);
	TVENC_TurnOnOff(1);

	// POST 2.5
	POST_InitIpForFifoInFifoOut(uLcdHsz, uLcdVsz, uWidth*2, uHeight, YUV, ONE_SHOT, &oSc);
	POST_SetAutoLoadEnable(FREE_RUN, &oSc);
	POST_SetInterlaceModeOnOff(1, &oSc);
	POST_SetInFifoMode(POST_FIFO, &oSc);
	POST_StartProcessing(&oSc);

	// FIMD 4.0
	//LCD_InitLDI(MAIN);
	LCD_InitBase();
	LCD_InitWinForFifoIn(IN_POST, WIN0);
	LCD_SetWinOnOff(1, WIN0);
	LCD_SetOutputPath(LCD_TVI80F);
	LCD_SetI80Timing(0, 1, 4, 0, MAIN); // cs_setup/wr_setup/wr_act/wr_hold at 40MHz HCLK
	LCD_Start();

	// POST 2.5
	POST_InitIpForDmaInFifoOut(uLcdHsz, uLcdVsz, uImgAddr, YCBYCR, uLcdHsz, uLcdVsz, RGB, 1, &oPost);
	POST_SetNextFrameStAddr(uImgAddr1, 0, &oPost);
	POST_SetAutoLoadEnable(FREE_RUN, &oPost);
	POST_SetInterlaceModeOnOff(0, &oPost);

	INTC_Enable(NUM_TVENC);
	INTC_Enable(NUM_CAMIF_P);	
	INTC_SetVectAddr(NUM_TVENC, Isr_FifoUnderrun);
	INTC_SetVectAddr(NUM_CAMIF_P, Isr_CamPview);	

	POST_StartProcessing(&oPost);

	TVENCT_SetTVParameter();
	//UART_Printf("Press any key to end..\n");
	//UART_Getc();

	//INTC_Mask(INT_TVENC0);
	//INTC_Mask(INT_CAMIF_P);
	INTC_Disable(NUM_TVENC);
	INTC_Disable(NUM_CAMIF_P);
	
	// oPost die..
	POST_StopProcessingOfFreeRun(&oPost);
	Delay(1000);

	// oScaler die..
	POST_SetInterlaceModeOnOff(0, &oSc);
	POST_StopProcessingOfFreeRun(&oSc);
	Delay(1000);

	// oLcdc die..
	LCD_Stop();

	// oTvenc die..
	TVENC_TurnOnOff(0);

	// oCim die..
	CAMERA_StopPreviewPath();
}

static void TVENCT_DualBuffer_POST_RGBFIMD_Scaler_Enc(void)
{
	u32 uImgSz, uWidth, uHeight;
	u32 uLcdHsz, uLcdVsz;
	CAMIF_INOUT uCamOut;

	uImgAddr = CODEC_MEM_ST;
	uImgAddr1 = CODEC_MEM_LMT;

	UART_Printf("Select Image Size\n");
	UART_Printf("[0] : Original Image Size\n");
	UART_Printf("[1] : Full Screen size\n");
	UART_Printf("Select number : ");
	uImgSz = UART_GetIntNum();

	uCamOut = DMA;
	
	// FIMD 4.0
	//LCD_InitLDI(MAIN);
	LCD_InitBase();
	if(uCamOut == DMA)
	{
		LCD_InitWinForFifoIn(IN_POST, WIN0);
		LCD_SetWinOnOff(1, WIN0);	
	}
	else if(uCamOut == FIFO)
	{
		LCD_InitWinForFifoIn(IN_CIM, WIN1);
		LCD_SetWinOnOff(1, WIN1);
	}
	LCD_GetFrmSz(&uLcdHsz, &uLcdVsz, WIN0);
	LCD_SetOutputPath(LCD_TVRGB);
	LCD_SetClkVal(4);
	//LCD_EnableRgbPort();
	
	if(uImgSz)
	{
		TVENCT_GetTVFullSize(&uWidth, &uHeight);
	}
	else
	{
		uWidth = uLcdHsz;
		uHeight = uLcdVsz;
	}

	// FIMC 3.0
	CAMERA_SFRInit();
	CAMERA_InitSensor();
	if(uCamOut == DMA)
		CAMERA_InitPreviewPath_1(uLcdHsz, uLcdVsz, uImgAddr, uImgAddr1, YCBYCR, FLIP_NO, ROT_0);
	else if(uCamOut == FIFO)	
		CAMERA_InitPreviewPathToFifoOut(uLcdHsz, uLcdVsz, YCBYCR);
	
	CAMERA_StartPreviewPath();
	
	// TV ENCODER 2.0
	TVENC_SetImageSize(uWidth*2, uHeight);
	TVENC_TurnOnOff(1);

	// POST 2.5
#if 0	
	POST_InitIpForFifoInFifoOut(uLcdHsz, uLcdVsz, uWidth*2, uHeight, YUV, ONE_SHOT, &oSc);
	POST_SetAutoLoadEnable(FREE_RUN, &oSc);
	POST_SetInterlaceModeOnOff(1, &oSc);
	POST_SetInFifoMode(POST_FIFO, &oSc);
	POST_StartProcessing(&oSc);
#else
	POST_InitIpForFifoInFifoOut(uLcdHsz, uLcdVsz, uWidth*2, uHeight, YUV, FREE_RUN, &oSc);
	POST_SetInterlaceModeOnOff(1, &oSc);
	POST_StartProcessing(&oSc);
#endif

	LCD_Start();

	//INTC_SetHandlerAndUnmask(INT_TVENC0, Isr_FifoUnderrun);
	//INTC_SetHandlerAndUnmask(INT_CAMIF_P, Isr_CamPview);
	INTC_Enable(NUM_TVENC);	
	INTC_SetVectAddr(NUM_TVENC, Isr_FifoUnderrun);
	INTC_Enable(NUM_CAMIF_P);	
	INTC_SetVectAddr(NUM_CAMIF_P, Isr_CamPview);
	
	// POST 2.5
	if(uCamOut == DMA)
	{
		POST_InitIpForDmaInFifoOut(uLcdHsz, uLcdVsz, uImgAddr, YCBYCR, uLcdHsz, uLcdVsz, RGB, 1, &oPost);
		POST_SetNextFrameStAddr(uImgAddr, 0, &oPost);
		//POST_SetNextFrameStAddr(uImgAddr1, 0, &oPost);	//rb1004
		POST_SetInterlaceModeOnOff(0, &oPost);
		POST_StartProcessing(&oPost);		
	}

	TVENCT_SetTVParameter();

	//INTC_Mask(INT_TVENC0);
	//INTC_Mask(INT_CAMIF_P);
	INTC_Disable(NUM_TVENC);
	INTC_Disable(NUM_CAMIF_P);
	
	// oPost die..
	if(uCamOut == DMA)
	{	
		POST_StopProcessingOfFreeRun(&oPost);
		while (!POST_IsProcessingDone(&oPost));	
	}

	// oScaler die..
	POST_SetInterlaceModeOnOff(0, &oSc);
	POST_StopProcessingOfFreeRun(&oSc);
	Delay(1000);

	// oLcdc die..
	LCD_DisableRgbPort();		//rb1004	
	LCD_Stop();

	// oTvenc die..
	TVENC_TurnOnOff(0);

	// oCim die..
	CAMERA_StopPreviewPath();
}

static void TVENCT_MultiLayer_POST_CPUFIMD_Scaler_Enc(void)
{
	u32 uImgSz, uWidth, uHeight;
	u32 uLcdHsz, uLcdVsz;

	uImgAddr = CODEC_MEM_ST;
	uImgAddr1 = CODEC_MEM_LMT;

	UART_Printf("Select Image Size\n");
	UART_Printf("[0] : Original Image Size\n");
	UART_Printf("[1] : Full Screen size\n");
	UART_Printf("Select number : ");
	uImgSz = UART_GetIntNum();

	uLcdHsz = 240; uLcdVsz = 320;

	if(uImgSz)
	{
		TVENCT_GetTVFullSize(&uWidth, &uHeight);
	}
	else
	{
		uWidth = uLcdHsz;
		uHeight = uLcdVsz;
	}
	
	//INTC_InitIp();

	// FIMC 3.0
	CAMERA_InitSensor();
	CAMERA_InitPreviewPath_1(uLcdHsz, uLcdVsz, uImgAddr, uImgAddr1, YCBYCR, FLIP_NO, ROT_0);
	CAMERA_StartPreviewPath();

	// TV ENCODER 2.0
	TVENC_SetImageSize(uWidth*2, uHeight);
	TVENC_TurnOnOff(1);

	// POST 2.5
	POST_InitIpForFifoInFifoOut(uLcdHsz, uLcdVsz, uWidth*2, uHeight, YUV, ONE_SHOT, &oSc);
	POST_SetAutoLoadEnable(FREE_RUN, &oSc);
	POST_SetInterlaceModeOnOff(1, &oSc);
	POST_SetInFifoMode(POST_FIFO, &oSc);
	POST_StartProcessing(&oSc);

	// FIMD 4.0

	// background
	//LCD_InitLDI(MAIN);
	LCD_InitBase();
	LCD_InitWinForFifoIn(IN_POST, WIN0);
	LCD_SetWinOnOff(1, WIN0);
	LCD_SetOutputPath(LCD_TVI80F);
	LCD_SetI80Timing(0, 1, 4, 0, MAIN); // cs_setup/wr_setup/wr_act/wr_hold at 40MHz HCLK

	// foreground
	LCD_InitWin(RGB24, uLcdHsz/2, uLcdVsz/2, 0, 0, uLcdHsz/2, uLcdVsz/2, uLcdHsz/4, uLcdVsz/4, uImgAddr1+uLcdHsz*uLcdVsz*4, WIN1, false);
	LCD_SetWinOnOff(1, WIN1);

	GLIB_InitInstance(uImgAddr1+uLcdHsz*uLcdVsz*4, uLcdHsz/2, uLcdVsz/2, RGB24);
	GLIB_DrawPattern(uLcdHsz/2, uLcdVsz/2);

	LCD_Start();

	// POST 2.5
	POST_InitIpForDmaInFifoOut(uLcdHsz, uLcdVsz, uImgAddr, YCBYCR, uLcdHsz, uLcdVsz, RGB, 1, &oPost);
	POST_SetNextFrameStAddr(uImgAddr, 0, &oPost);
	POST_SetAutoLoadEnable(FREE_RUN, &oPost);
	POST_SetInterlaceModeOnOff(0, &oPost);
	POST_SetInFifoMode(POST_DMA, &oPost);

	//INTC_SetHandlerAndUnmask(INT_TVENC0, Isr_FifoUnderrun);
	//INTC_SetHandlerAndUnmask(INT_CAMIF_P, Isr_CamPview);
	INTC_Enable(NUM_TVENC);	
	INTC_SetVectAddr(NUM_TVENC, Isr_FifoUnderrun);
	INTC_Enable(NUM_CAMIF_P);	
	INTC_SetVectAddr(NUM_CAMIF_P, Isr_CamPview);
	
	POST_StartProcessing(&oPost);

	LCD_Trigger();

	TVENCT_SetTVParameter();
	//UART_Printf("Press any key to end..\n");
	//UART_Getc();

	//INTC_Mask(INT_TVENC0);
	//INTC_Mask(INT_CAMIF_P);
	INTC_Disable(NUM_TVENC);
	INTC_Disable(NUM_CAMIF_P);

	// oPost die..
	POST_StopProcessingOfFreeRun(&oPost);
	Delay(1000);

	// oScaler die..
	POST_SetInterlaceModeOnOff(0, &oSc);
	POST_StopProcessingOfFreeRun(&oSc);
	Delay(1000);

	// oLcdc die..
	LCD_Stop();

	// oTvenc die..
	TVENC_TurnOnOff(0);

	// oCim die..
	CAMERA_StopPreviewPath();

}


static void TVENCT_MultiLayer_POST_RGBFIMD_Scaler_Enc(void)
{
	u32 uImgSz, uWidth, uHeight;
	u32 uLcdHsz, uLcdVsz;

	UART_Printf("Select Image Size\n");
	UART_Printf("[0] : Original Image Size\n");
	UART_Printf("[1] : Full Screen size\n");
	UART_Printf("Select number : ");
	uImgSz = UART_GetIntNum();

	uImgAddr = CODEC_MEM_ST;
	uImgAddr1 = CODEC_MEM_LMT;	
	//uLcdHsz = 240; uLcdVsz = 320;

	// FIMD 4.0
	//LCD_InitLDI(MAIN);
	LCD_InitBase();
	LCD_InitWinForFifoIn(IN_POST, WIN0);
	LCD_GetFrmSz(&uLcdHsz, &uLcdVsz, WIN0);
	LCD_SetWinOnOff(1, WIN0);
	LCD_SetOutputPath(LCD_TVRGB);
	LCD_SetClkVal(4);
	LCD_EnableRgbPort();
	
	if(uImgSz)
	{
		TVENCT_GetTVFullSize(&uWidth, &uHeight);
	}
	else
	{
		uWidth = uLcdHsz;
		uHeight = uLcdVsz;
	}

	// FIMC 3.0
	CAMERA_SFRInit();	
	CAMERA_InitSensor();
	CAMERA_InitPreviewPath_1(uLcdHsz, uLcdVsz, uImgAddr, uImgAddr1, YCBYCR, FLIP_NO, ROT_0);
	CAMERA_StartPreviewPath();

	// TV ENCODER 2.0
	TVENC_SetImageSize(uWidth*2, uHeight);
	TVENC_TurnOnOff(1);

	// POST 2.5
	POST_InitIpForFifoInFifoOut(uLcdHsz, uLcdVsz, uWidth*2, uHeight, YUV, FREE_RUN, &oSc);
	POST_SetInterlaceModeOnOff(1, &oSc);
	POST_StartProcessing(&oSc);

	// foreground
	LCD_InitWin(RGB24, uLcdHsz/2, uLcdVsz/2, 0, 0, uLcdHsz/2, uLcdVsz/2, uLcdHsz/4, uLcdVsz/4, uImgAddr1+uLcdHsz*uLcdVsz*4, WIN1, false);
	LCD_SetWinOnOff(1, WIN1);

	GLIB_InitInstance(uImgAddr1+uLcdHsz*uLcdVsz*4, uLcdHsz/2, uLcdVsz/2, RGB24);
	GLIB_DrawPattern(uLcdHsz/2, uLcdVsz/2);

	LCD_Start();
	
	// POST 2.5
	POST_InitIpForDmaInFifoOut(uLcdHsz, uLcdVsz, uImgAddr, YCBYCR, uLcdHsz, uLcdVsz, RGB, 1, &oPost);
	POST_SetNextFrameStAddr(uImgAddr1, 0, &oPost);
	POST_SetInterlaceModeOnOff(0, &oPost);

	//INTC_SetHandlerAndUnmask(INT_TVENC0, Isr_FifoUnderrun);
	//INTC_SetHandlerAndUnmask(INT_CAMIF_P, Isr_CamPview);
	INTC_Enable(NUM_TVENC);	
	INTC_SetVectAddr(NUM_TVENC, Isr_FifoUnderrun);
	INTC_Enable(NUM_CAMIF_P);	
	INTC_SetVectAddr(NUM_CAMIF_P, Isr_CamPview);
	
	POST_StartProcessing(&oPost);

	TVENCT_SetTVParameter();

	//INTC_Mask(INT_TVENC0);
	//INTC_Mask(INT_CAMIF_P);
	INTC_Disable(NUM_TVENC);
	INTC_Disable(NUM_CAMIF_P);

	// oPost die..
	POST_StopProcessingOfFreeRun(&oPost);
	while (!POST_IsProcessingDone(&oPost));	

	// oScaler die..
	POST_SetInterlaceModeOnOff(0, &oSc);
	POST_StopProcessingOfFreeRun(&oSc);
	Delay(1000);

	// oLcdc die..
	LCD_DisableRgbPort();		//rb1004	
	LCD_Stop();

	// oTvenc die..
	TVENC_TurnOnOff(0);

	// oCim die..
	CAMERA_StopPreviewPath();
}



void TVENCT_OtherImage_LCDandTV(void)
{
	u32 uImgSz, uWidth, uHeight;
	u32 uLcdHsz, uLcdVsz;
	//CAMIF_INOUT eCamOut;

	uGlibStAddr = CODEC_MEM_ST;
	uImgAddr = CODEC_MEM_LMT;
	uImgAddr1 = CODEC_MEM_LMT + 0x400000;

	UART_Printf("Select Image Size\n");
	UART_Printf("[0] : Original Image Size\n");
	UART_Printf("[1] : Full Screen size\n");
	UART_Printf("Select number : ");
	uImgSz = UART_GetIntNum();

	//uLcdHsz = 320; uLcdVsz = 240;

	// FIMD 4.0
	//LCD_InitLDI(MAIN);
	LCD_InitBase();
	LCD_InitWinForFifoIn(IN_POST, WIN0);	
	LCD_GetFrmSz(&uLcdHsz, &uLcdVsz, WIN0);
	LCD_SetWinOnOff(1, WIN0);	
	LCD_SetOutputPath(LCD_RGB);
	LCD_SetClkVal(4);

	if(uImgSz)
	{
		TVENCT_GetTVFullSize(&uWidth, &uHeight);
	}
	else
	{
		uWidth = uLcdHsz;
		uHeight = uLcdVsz;
	}
	
	// FIMC 3.0
	CAMERA_SFRInit();
	INTC_Enable(NUM_CAMIF_P);	
	INTC_SetVectAddr(NUM_CAMIF_P, Isr_CamPview_Scaler);	
	CAMERA_InitSensor();
	CAMERA_InitPreviewPath_1(uLcdHsz, uLcdVsz, uImgAddr, uImgAddr1, RGB24, FLIP_NO, ROT_0);
	CAMERA_StartPreviewPath();

	//GLIB_InitInstance(uImgAddr, uLcdHsz, uLcdVsz, RGB24);
	//GLIB_DrawPattern2(uLcdHsz, uLcdVsz);

	GLIB_InitInstance(uGlibStAddr, uLcdHsz, uLcdVsz, RGB24);
	GLIB_DrawPattern(uLcdHsz, uLcdVsz);
	
	// POST 2.5
	POST_InitIpForDmaInFifoOut(uLcdHsz, uLcdVsz, uGlibStAddr, RGB24, 
								uLcdHsz, uLcdVsz, RGB, 1, &oPost);
	POST_DisableChangeNextFrame(&oPost);
	POST_StartProcessing(&oPost);

	LCD_Start();
	
	INTC_Enable(NUM_TVENC);	
	INTC_SetVectAddr(NUM_TVENC, Isr_FifoUnderrun);


	// TV ENCODER 2.0
	TVENC_SetImageSize(uWidth*2, uHeight);
	TVENC_TurnOnOff(1);
	
#if 1
	// POST 2.5
	//POST_InitIpForDmaInFifoOut(uLcdHsz, uLcdVsz, uImgAddr, RGB24, 
	//							uWidth*2, uHeight, YUV, 1, &oSc);
	POST_InitIpForDmaInFifoOut(uLcdHsz, uLcdVsz, uImgAddr, RGB24, 
								uWidth*2, uHeight, YUV, 1, &oSc);
	POST_SetNextFrameStAddr(uImgAddr, 0, &oSc);
	//POST_SetNextFrameStAddr(uImgAddr1, 0, &oSc);	//rb1004
	POST_SetInterlaceModeOnOff(1, &oSc);
	POST_StartProcessing(&oSc);		
#else	//for ONE_SHOT test
	POST_InitIp1(	uLcdHsz, uLcdVsz, 0, 0, uLcdHsz, uLcdVsz, uImgAddr, RGB24,
					uWidth*2, uHeight, 0, 0, uWidth*2, uHeight, 0, YCBYCR,
					1, FALSE, ONE_SHOT, POST_DMA, POST_FIFO, &oSc);
	POST_SetInterlaceModeOnOff(1, &oSc);
	POST_SetInFifoMode(POST_DMA, &oSc);	
	POST_StartProcessing(&oSc);

	while(1)
	{
		POST_StartProcessing(&oSc);
		Delay(50);
	}

#endif

	TVENCT_SetTVParameter();

	//INTC_Mask(INT_TVENC0);
	//INTC_Mask(INT_CAMIF_P);
	INTC_Disable(NUM_TVENC);	
	INTC_Disable(NUM_CAMIF_P);	
	
	// oPost die..
	POST_StopProcessingOfFreeRun(&oPost);
	while (!POST_IsProcessingDone(&oPost));	

	// oScaler die..
	POST_SetInterlaceModeOnOff(0, &oSc);
	POST_StopProcessingOfFreeRun(&oSc);
	Delay(1000);

	// oLcdc die..
	//LCDC_DisableRgbPort();		//rb1004	
	LCD_Stop();

	// oTvenc die..
	TVENC_TurnOnOff(0);

	// oCim die..
	CAMERA_StopPreviewPath();
}





////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////           Display Scnario Performance Test           //////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

extern void ScalerT_SelectClockSource(void);
void TVENCT_Scaler_Enc_Perform(void) // post controller initialize
{
	u32 uImgSz, uWidth, uHeight;

	UART_Printf("Select Image Size\n");
	UART_Printf("[0] : Original Image Size\n");
	UART_Printf("[1] : Full Screen size\n");
	UART_Printf("Select number : ");
	uImgSz = UART_GetIntNum();
	ScalerT_SelectClockSource();
		
	if(uImgSz)
	{
		TVENCT_GetTVFullSize(&uWidth, &uHeight);
	}
	else
	{
		uWidth = uImgXsize;
		uHeight = uImgYsize;
	}

	INTC_Enable(NUM_TVENC);
	INTC_SetVectAddr(NUM_TVENC, Isr_FifoUnderrun);

	TVENC_SetImageSize(uWidth*2, uHeight);
	TVENC_TurnOnOff(1);

	POST_InitIp1(	uImgXsize, uImgYsize, 0, 0, uImgXsize, uImgYsize, uImgAddr, uImgResolution,
					uWidth*2, uHeight, 0, 0, uWidth*2, uHeight, 0, YCBYCR,
					1, FALSE, FREE_RUN, POST_DMA, POST_FIFO, &oSc);
	POST_SetInterlaceModeOnOff(1, &oSc);
	POST_SetNextFrameStAddr(uImgAddr, 0, &oSc);
	POST_StartProcessing(&oSc);

	while(UART_Getc() != 'q')
	{
		TVENCT_SelectClockDivider();
	}
	
	TVENCT_SetTVParameter();


	INTC_Disable(NUM_TVENC);

	POST_SetInterlaceModeOnOff(0, &oSc);
	POST_StopProcessingOfFreeRun(&oSc);
	while (!POST_IsProcessingDone(&oSc));

	TVENC_TurnOnOff(0);
}



static void TVENCT_FIMD_Scaler_Enc_Dual_Perform(void) // post controller initialize
{
	u32 uImgSz, uLcdClock, uWidth, uHeight;
	u32 uLcdHsz, uLcdVsz;
	//u32 uCameraHsz, uCameraVsz;
	CLK_SRC eLcdClockSrc;

	UART_Printf("[TVENCT_FIMD_Scaler_Enc_Perform]\n");
	UART_Printf("\n");
	
	UART_Printf("Select TV out Image Size\n");
	UART_Printf("[0] : Original Image Size\n");
	UART_Printf("[1] : Full Screen size\n");
	UART_Printf("Select number : ");
	uImgSz = UART_GetIntNum();

	UART_Printf("\n");
	UART_Printf("Select FIMD Source Clock\n");
	UART_Printf("[0] : HCLK\n");
	UART_Printf("[1] : MPLLout\n");
	UART_Printf("Select number : ");
	uLcdClock = UART_GetIntNum();
	if(uLcdClock == 0)
		eLcdClockSrc = (CLK_SRC)HCLK_SRC;
	else
		eLcdClockSrc = (CLK_SRC)ECLK0_SRC;
	
	ScalerT_SelectClockSource();
	
	uImgAddr = CODEC_MEM_ST;
	uImgAddr1 = CODEC_MEM_LMT;	

	// FIMD 4.0
	LCD_InitDISPC(RGB24, uImgAddr, WIN0, false);
	LCD_GetFrmSz(&uLcdHsz, &uLcdVsz, WIN0);
	LCD_SetWinOnOff(1, WIN0);
	LCD_SetOutputPath(LCD_TVRGB);
	LCD_SetClkSrc(eLcdClockSrc);
	LCD_Start();

	//uCameraHsz = uLcdHsz;
	//uCameraVsz = uLcdVsz;
	
	if(uImgSz)
	{
		TVENCT_GetTVFullSize(&uWidth, &uHeight);
	}
	else
	{
		uWidth = uLcdHsz;
		uHeight = uLcdVsz;
	}

	INTC_SetVectAddr(NUM_TVENC, Isr_FifoUnderrun);
	INTC_Enable(NUM_TVENC);
	INTC_SetVectAddr(NUM_CAMIF_P, Isr_CamPview);
	INTC_Enable(NUM_CAMIF_P);	
	
	// FIMC 3.0
	CAMERA_SFRInit();	
	CAMERA_InitSensor();
	CAMERA_InitPreviewPath_1(uLcdHsz, uLcdVsz, uImgAddr, uImgAddr, RGB24, FLIP_NO, ROT_0);

	// TV ENCODER 2.0
	TVENC_SetImageSize(uWidth*2, uHeight);
	TVENC_TurnOnOff(1);

	// POST 2.5
	POST_InitIpForFifoInFifoOut(uLcdHsz, uLcdVsz, uWidth*2, uHeight, YUV, FREE_RUN, &oSc);
	POST_SetInterlaceModeOnOff(1, &oSc);
	POST_StartProcessing(&oSc);

	// foreground
	LCD_InitWin(RGB24, uLcdHsz, uLcdVsz, 0, 0, uLcdHsz, uLcdVsz, 0, 0, uImgAddr1+uLcdHsz*uLcdVsz*4, WIN1, false);
	LCD_SetAlpha(0x8, 0x8, 0x8, 0x8, 0x8, 0x8, WIN1);
	LCD_SetWinOnOff(1, WIN1);

	GLIB_InitInstance(uImgAddr1+uLcdHsz*uLcdVsz*4, uLcdHsz, uLcdVsz, RGB24);
	GLIB_DrawPattern(uLcdHsz, uLcdVsz);

	LCD_Start();
	CAMERA_StartPreviewPath();
	
	while(UART_Getc() != 'q')
	{
		u32 uSelclkval;

		UART_Printf("\n");
		UART_Printf("Select the source clock [0x01 ~ 0x3F] : ");
	
		uSelclkval = (u32)UART_GetIntNum();
		LCD_SetClkVal(uSelclkval);
	}

	INTC_Disable(NUM_TVENC);
	INTC_Disable(NUM_CAMIF_P);

	// oPost die..
	POST_StopProcessingOfFreeRun(&oPost);
	while (!POST_IsProcessingDone(&oPost));	

	// oScaler die..
	POST_SetInterlaceModeOnOff(0, &oSc);
	POST_StopProcessingOfFreeRun(&oSc);
	while (!POST_IsProcessingDone(&oSc));

	// oLcdc die..
	LCD_Stop();

	// oTvenc die..
	TVENC_TurnOnOff(0);

	// oCim die..
	CAMERA_StopPreviewPath();

}



static void TVENCT_POST_FIMD_Scaler_Enc_Perform(void)
{
	u32 uImgSz, uWidth, uHeight, uTestItem;
	u32 uLcdHsz, uLcdVsz;

	UART_Printf("Select Image Size\n");
	UART_Printf("[0] : Original Image Size\n");
	UART_Printf("[1] : Full Screen size\n");
	UART_Printf("Select number : ");
	uImgSz = UART_GetIntNum();

	if(uImgSz)
	{
		TVENCT_GetTVFullSize(&uWidth, &uHeight);
	}
	else
	{
		uWidth = uImgXsize;
		uHeight = uImgYsize;
	}

	uTestItem = 1;
	
	INTC_Enable(NUM_TVENC);
	INTC_SetVectAddr(NUM_TVENC, Isr_FifoUnderrun);
	
	// TV ENCODER 2.0
	TVENC_SetImageSize(uWidth*2, uHeight);
	TVENC_TurnOnOff(1);

	// FIMD 4.0
	if(uTestItem == 0)
		LCD_InitBase1(uImgXsize, uImgYsize);
	else if(uTestItem == 1)
		LCD_InitBase();
	LCD_InitWinForFifoIn(IN_POST, WIN0);
	LCD_SetWinOnOff(1, WIN0);	
	LCD_SetOutputPath(LCD_TV);
	LCD_GetFrmSz(&uLcdHsz, &uLcdVsz, WIN0);
	
	if(uTestItem == 0)
		POST_InitIpForFifoInFifoOut(	uImgXsize, uImgYsize, uWidth*2, uHeight, YUV, FREE_RUN, &oSc);
	else if(uTestItem == 1)
		POST_InitIpForFifoInFifoOut(	uLcdHsz, uLcdVsz, uWidth*2, uHeight, YUV, FREE_RUN, &oSc);
	
	POST_SetInterlaceModeOnOff(1, &oSc);
	POST_StartProcessing(&oSc);

	LCD_Start();

	// POST 2.5
	if(uTestItem == 0)
		POST_InitIpForDmaInFifoOut(uImgXsize, uImgYsize, uImgAddr, uImgResolution, 
									uImgXsize, uImgYsize, RGB, 1, &oPost);
	else if(uTestItem == 1)
		POST_InitIpForDmaInFifoOut(uImgXsize, uImgYsize, uImgAddr, uImgResolution, 
									uLcdHsz, uLcdVsz, RGB, 1, &oPost);
	POST_SetNextFrameStAddr(uImgAddr, 0, &oPost);
	POST_StartProcessing(&oPost);

	while(UART_Getc() != 'q')
	{
		u32 uSelclkval;

		UART_Printf("\n");
		UART_Printf("Select the source clock [0x01 ~ 0x3F] : ");
	
		uSelclkval = (u32)UART_GetIntNum();
		LCD_SetClkVal(uSelclkval);
	}
	TVENCT_SetTVParameter();

	INTC_Disable(NUM_TVENC);

	// oPost die..
	POST_StopProcessingOfFreeRun(&oPost);
	Delay(1000);

	// oScaler die..
	POST_SetInterlaceModeOnOff(0, &oSc);
	POST_StopProcessingOfFreeRun(&oSc);
	Delay(1000);

	// oLcdc die..
	LCD_Stop();

	// oTvenc die..
	TVENC_TurnOnOff(0);
}


extern void POSTT_SelectClockSource(void);
static void TVENCT_POST_FIMD_Scaler_Enc_Dual_Perform(void)
{
	u32 uImgSz, uLcdClock, uWidth, uHeight;
	u32 uLcdHsz, uLcdVsz;
	u32 uCameraHsz, uCameraVsz;
	CLK_SRC eLcdClockSrc;
	
	UART_Printf("Select Image Size\n");
	UART_Printf("[0] : Original Image Size\n");
	UART_Printf("[1] : Full Screen size\n");
	UART_Printf("Select number : ");
	uImgSz = UART_GetIntNum();

	POSTT_SelectClockSource();
	
	UART_Printf("\n");
	UART_Printf("Select FIMD Source Clock\n");
	UART_Printf("[0] : HCLK\n");
	UART_Printf("[1] : MPLLout\n");
	UART_Printf("Select number : ");
	uLcdClock = UART_GetIntNum();
	if(uLcdClock == 0)
		eLcdClockSrc = (CLK_SRC)HCLK_SRC;
	else
		eLcdClockSrc = (CLK_SRC)ECLK0_SRC;
	
	ScalerT_SelectClockSource();
	
	uImgAddr = CODEC_MEM_ST;
	uImgAddr1 = CODEC_MEM_LMT;	

	// FIMD 4.0
	LCD_InitBase();
	LCD_InitWinForFifoIn(IN_POST, WIN0);
	LCD_GetFrmSz(&uLcdHsz, &uLcdVsz, WIN0);
	LCD_SetWinOnOff(1, WIN0);
	LCD_SetOutputPath(LCD_TVRGB);
	LCD_SetClkSrc(eLcdClockSrc);
	
	uCameraHsz = uLcdHsz;
	uCameraVsz = uLcdVsz;
	
	if(uImgSz)
	{
		TVENCT_GetTVFullSize(&uWidth, &uHeight);
	}
	else
	{
		//uWidth = uLcdHsz;
		//uHeight = uLcdVsz;
		uWidth = 360;
		uHeight = 480;
	}
	
	INTC_SetVectAddr(NUM_TVENC, Isr_FifoUnderrun);
	INTC_Enable(NUM_TVENC);
	INTC_SetVectAddr(NUM_CAMIF_P, Isr_CamPview);
	INTC_Enable(NUM_CAMIF_P);	
	
	// FIMC 3.0
	CAMERA_SFRInit();	
	CAMERA_InitSensor();
	CAMERA_InitPreviewPath_1(uCameraHsz, uCameraVsz, uImgAddr, uImgAddr1, YCBYCR, FLIP_NO, ROT_0);
	CAMERA_StartPreviewPath();
	//GLIB_InitInstance(uImgAddr, uCameraHsz, uCameraVsz, RGB24);
	//GLIB_DrawPattern2(uCameraHsz, uCameraVsz);
	//GLIB_InitInstance(uImgAddr1, uCameraHsz, uCameraVsz, RGB24);
	//GLIB_DrawPattern2(uCameraHsz, uCameraVsz);
	
	// TV ENCODER 2.0
	TVENC_SetImageSize(uWidth*2, uHeight);
	TVENC_TurnOnOff(1);

	// POST 2.5
	POST_InitIpForFifoInFifoOut(uLcdHsz, uLcdVsz, uWidth*2, uHeight, YUV, FREE_RUN, &oSc);
	POST_SetInterlaceModeOnOff(1, &oSc);
	POST_StartProcessing(&oSc);

	// foreground
	LCD_InitWin(RGB24, uLcdHsz, uLcdVsz, 0, 0, uLcdHsz, uLcdVsz, 0, 0, uImgAddr1+uCameraHsz*uCameraVsz*4, WIN1, false);
	LCD_SetAlpha(0x8, 0x8, 0x8, 0x8, 0x8, 0x8, WIN1);
	LCD_SetWinOnOff(1, WIN1);

	GLIB_InitInstance(uImgAddr1+uCameraHsz*uCameraVsz*4, uLcdHsz, uLcdVsz, RGB24);
	GLIB_DrawPattern(uLcdHsz, uLcdVsz);

	LCD_Start();
	
	// POST 2.5
	POST_InitIpForDmaInFifoOut(uCameraHsz, uCameraVsz, uImgAddr, YCBYCR, uLcdHsz, uLcdVsz, RGB, 1, &oPost);
	//POST_InitIpForDmaInFifoOut(uCameraHsz, uCameraVsz, uImgAddr, RGB24, uLcdHsz, uLcdVsz, RGB, 1, &oPost);
	POST_SetNextFrameStAddr(uImgAddr1, 0, &oPost);
	POST_SetInterlaceModeOnOff(0, &oPost);

	POST_StartProcessing(&oPost);

	while(UART_Getc() != 'q')
	{
		u32 uSelclkval;

		UART_Printf("\n");
		UART_Printf("Select the source clock [0x01 ~ 0x3F] : ");
	
		uSelclkval = (u32)UART_GetIntNum();
		LCD_SetClkVal(uSelclkval);
	}
	TVENCT_SetTVParameter();

	INTC_Disable(NUM_TVENC);
	INTC_Disable(NUM_CAMIF_P);

	// oPost die..
	POST_StopProcessingOfFreeRun(&oPost);
	while (!POST_IsProcessingDone(&oPost));	

	// oScaler die..
	POST_SetInterlaceModeOnOff(0, &oSc);
	POST_StopProcessingOfFreeRun(&oSc);
	while (!POST_IsProcessingDone(&oSc));

	// oLcdc die..
	LCD_Stop();

	// oTvenc die..
	TVENC_TurnOnOff(0);

	// oCim die..
	CAMERA_StopPreviewPath();
}



const testFuncMenu tvenc_scenario_menu[]=
{
	TVENCT_Scaler_Enc_Perform,							"TV Only  : [SCALER -> TVenc]",
	TVENCT_POST_FIMD_Scaler_Enc_Perform,				"TV Only  : [POST -> FIMD -> SCALER -> TVenc ]",
	TVENCT_FIMD_Scaler_Enc_Dual_Perform,				"LCD & TV : [FIMD -> SCALER -> TVenc]",
	TVENCT_POST_FIMD_Scaler_Enc_Dual_Perform,		"LCD & TV : [POST -> FIMD -> SCALER -> TVenc]",
	0,0
};

void TVENCT_ScenarioPerform(void)
{
	u32 i;
	s32 sSel;
	
	UART_Printf("\n");
	UART_Printf("[TVENCT_ScenarioPerform]\n");
	
	while(1)
	{
		UART_Printf("\n");
		for (i=0; (int)(tvenc_scenario_menu[i].desc)!=0; i++)
			UART_Printf("%2d: %s\n", i, tvenc_scenario_menu[i].desc);

		UART_Printf("\nSelect the function to test : ");
		sSel = UART_GetIntNum();
		UART_Printf("\n");

		if (sSel == -1)
			break;
		if (sSel>=0 && sSel<(sizeof(tvenc_scenario_menu)/8-1))
			(tvenc_scenario_menu[sSel].func)();
	}
}


const testFuncMenu tvenc_menu[]=
{
	TVENCT_Scaler_Enc,								"TV on [SCALER -> ENCODER]",
	TVENCT_FIMD_Scaler_Enc,						"TV on [FIMD -> SCALER -> ENCODER]",
	TVENCT_POST_FIMD_Scaler_Enc,					"TV on [POST -> FIMD -> SCALER -> ENCODER]",
	TVENCT_Dual_POST_CPUFIMD_Scaler_Enc,			"Dual Display (CPU-LCD + TV) [POST -> FIMD -> SCALER -> ENCODER]",
	TVENCT_Dual_POST_RGBFIMD_Scaler_Enc,			"Dual Display (RGB-LCD + TV) [POST -> FIMD -> SCALER -> ENCODER]",
	TVENCT_DualBuffer_POST_CPUFIMD_Scaler_Enc,	"Double Buffer (CPU-LCD + TV) [POST -> FIMD -> SCALER -> ENCODER]",
	TVENCT_DualBuffer_POST_RGBFIMD_Scaler_Enc,	"Double Buffer (RGB-LCD + TV) [POST -> FIMD -> SCALER -> ENCODER]",
	TVENCT_MultiLayer_POST_CPUFIMD_Scaler_Enc,	"Multi Layer (CPU-LCD + TV) [POST -> FIMD -> SCALER -> ENCODER]",
	TVENCT_MultiLayer_POST_RGBFIMD_Scaler_Enc,	"Multi Layer (RGB-LCD + TV) [POST -> FIMD -> SCALER -> ENCODER]",
	TVENCT_OtherImage_LCDandTV,					"Other image display(RGB-LCD + TV) [POST -> FIMD & SCALER -> ENCODER]",
	TVENCT_MakeImage,								"Make Image",
	TVENCT_LoadImageToMemory,					"Load Image File",	
	TVENCT_SetTVParameter,							"Set Parameter",
	TVENCT_ScenarioPerform,						"Scenario Performance",
	
	0,0
};

	
void TVENC_Test(void)
{
	u32 i;
	s32 sSel;

	TVENCT_SelectTVOutputFormat();
	
	POST_InitCh(POST_A, &oPost);
	POST_InitCh(POST_B, &oSc);

	//Camera Setting
	GPIO_SetFunctionAll( eGPIO_F, 0x2aaaaaa, 0); // Camera GPIO Setting : rGPFCON = 0x2aa aaaa
	GPIO_SetPullUpDownAll(eGPIO_F, 0);
   	CAMERA_ClkSetting(); // Camera Module CLK Setting

	//LCD Setting
	LCD_SetPort();
	LCD_InitLDI(MAIN);
	LCD_Stop();

	//TVENCT_MakeImage();
	//TVENCT_Scaler_Enc();
	
	while(1)
	{
		UART_Printf("\n");
		for (i=0; (int)(tvenc_menu[i].desc)!=0; i++)
			UART_Printf("%2d: %s\n", i, tvenc_menu[i].desc);

		UART_Printf("\nSelect the function to test : ");
		sSel = UART_GetIntNum();
		UART_Printf("\n");

		if (sSel == -1)
			break;
		if (sSel>=0 && sSel<(sizeof(tvenc_menu)/8-1))
			(tvenc_menu[sSel].func)();
	}
}
