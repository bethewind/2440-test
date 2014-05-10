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
*	File Name : camera.c
*  
*	File Description : Camera API Function 
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

#include "def.h"
#include "system.h"
#include "lcd.h"
#include "camera.h"
#include "cameram.h"
//#include "CameraModule.h"
#include "glib.h"
#include "intc.h"
#include "option.h"
#include "library.h"
#include "sfr6410.h"
#include "sysc.h"
#include "gpio.h"


#define CAMERAR 		 ( ( volatile oCAMERA_REGS * ) CAMERA_pBase )

static void *CAMERA_pBase;


typedef struct tag_CAMERA_REGS
{
	u32 rCISRCFMT; 				// 0x7800_0000			Camera Input Sorce Format
	u32 rCIWDOFST;				// 0x7800_0004			Window Offset Register
	u32 rCIGCTRL;				// 0x7800_0008			Golbal Control Register
	u32 rreserved0;
	u32 rreserved1;
	u32 rCIDOWFST2;			// 0x7800_0014			Window Offset Register 2
	u32 rCICOYSA1;				// 0x7800_0018			Interleace YCbCr, Non Interleave Y, RGB : 1st Frame Start Address for Codec DMA
	u32 rCICOYSA2;				// 0x7800_001C			Interleace YCbCr, Non Interleave Y, RGB : 2st Frame Start Address for Codec DMA
	u32 rCICOYSA3;				// 0x7800_0020			Interleace YCbCr, Non Interleave Y, RGB : 3st Frame Start Address for Codec DMA
	u32 rCICOYSA4;				// 0x7800_0024			Interleace YCbCr, Non Interleave Y, RGB : 4st Frame Start Address for Codec DMA
	u32 rCICOCBSA1;				// 0x7800_0028 		Cb 1st Frame Start Address for Codec DMA
	u32 rCICOCBSA2;				// 0x7800_002C			Cb 2st Frame Start Address for Codec DMA
	u32 rCICOCBSA3;				// 0x7800_0030			Cb 3st Frame Start Address for Codec DMA
	u32 rCICOCBSA4;				// 0x7800_0034			Cb 4st Frame Start Address for Codec DMA
	u32 rCICOCRSA1;				// 0x7800_0038			Cr 1st Frame Start Address for Codec DMA
	u32 rCICOCRSA2;				// 0x7800_003C			Cr 2st Frame Start Address for Codec DMA
	u32 rCICOCRSA3;				// 0x7800_0040			Cr 3st Frame Start Address for Codec DMA
	u32 rCICOCRSA4;				// 0x7800_0044			Cr 4st Frame Start Address for Codec DMA
	u32 rCICOTRGFMT;			// 0x7800_0048			Targer Image Format of Codec DMA
	u32 rCICOCTRL;				// 0x7800_004C			Codec DMA Control Related
	u32 rCICOSCPRERATIO;		// 0x7800_0050			Codec Pre_Scaler Ratio Control
	u32 rCICOSCPREDST;			// 0x7800_0054			Codec Pre_Scaler Destination Format
	u32 rCICOSCCTRL;				// 0x7800_0058			Codec Main_Scaler Control
	u32 rCICOTAREA;				// 0x7800_005C			Codec Pre_Scaler Destination Format(Area)
	u32 rreserved3;
	u32 rCICOSTATUS;				// 0x7800_0064			Codec Path Status
	u32 rreserved4;
	u32 rCIPRYSA1;				// 0x7800_006C			Interleace YCbCr, Non Interleave Y, RGB : 1st Frame Start Address for Preview DMA
	u32 rCIPRYSA2;				// 0x7800_0070			Interleace YCbCr, Non Interleave Y, RGB : 2st Frame Start Address for Preview DMA
	u32 rCIPRYSA3;				// 0x7800_0074			Interleace YCbCr, Non Interleave Y, RGB : 3st Frame Start Address for Preview DMA
	u32 rCIPRYSA4;				// 0x7800_0078			Interleace YCbCr, Non Interleave Y, RGB : 4st Frame Start Address for Preview DMA
	u32 rCIPRCBSA1;				// 0x7800_007C			Cb, 1st Frame Start Address for Preview DMA
	u32 rCIPRCBSA2;				// 0x7800_0080			Cb, 2st Frame Start Address for Preview DMA
	u32 rCIPRCBSA3;				// 0x7800_0084			Cb, 3st Frame Start Address for Preview DMA
	u32 rCIPRCBSA4;				// 0x7800_0088			Cb, 4st Frame Start Address for Preview DMA
	u32 rCIPRCRSA1;				// 0x7800_008C			Cr, 1st Frame Start Address for Preview DMA
	u32 rCIPRCRSA2;				// 0x7800_0090			Cr, 2st Frame Start Address for Preview DMA
	u32 rCIPRCRSA3;				// 0x7800_0094			Cr, 3st Frame Start Address for Preview DMA
	u32 rCIPRCRSA4;				// 0x7800_0098			Cr, 4st Frame Start Address for Preview DMA
	u32 rCIPRTRGFMT;				// 0x7800_009C			Target Image For Preview DMA
	u32 rCIPRCTRL;				// 0x7800_00A0			Preview DMA Control Releated
	u32 rCIPRSCPRERATIO;		// 0x7800_00A4			Preview Pre_Scaler Ratio Control
	u32 rCIPRSCPREDST;			// 0x7800_00A8			Preview Pre_Scaler Destination Format
	u32 rCIPRSCCTRL;				// 0x7800_00AC		Preview Main Scaler Control
	u32 rCIPRTAREA;				// 0x7800_00B0			Preview Pre_Scaler Destination Format(Area)
	u32 rreserved5;
	u32 rCIPRSTATUS;				// 0x7800_00B8			Preview Path Status
	u32 rreserved6;
	u32 rCIIMGCPT;				// 0x7800_00C0			Image Capture Enable Command
	u32 rCICPTSEQ;				// 0x7800_00C4			Camera Image Capture Sequence Related
	u32 rreserved7;
	u32 rreserved8;
	u32 rCIIMGEFF;				// 0x7800_00D0		Image Effects Related
	u32 rMSCOY0SA;				// 0x7800_00D4		MSDMA Y0 Start Address Related
	u32 rMSCOCB0SA;				// 0x7800_00D8		MSDMA Cb0 Start Address Related
	u32 rMSCOCR0SA;				// 0x7800_00DC		MSDMA Cr0 Start Address Related
	u32 rMSCOY0END;				// 0x7800_00E0			MSDMA Y0 End Address Related
	u32 rMSCOCB0END;			// 0x7800_00E4			MSDMA Cb0 End Address Related
	u32 rMSCOCR0END;			// 0x7800_00E8			MSDMA Cr0 End Address Related
	u32 rMSCOYOFF;				// 0x7800_00EC			MSDMA Y Offset Related
	u32 rMSCOCBOFF;				// 0x7800_00F0			MSDMA Cb Offset Related
	u32 rMSCOCROFF; 				// 0x7800_00F4			MSDMA Cr Offset Related
	u32 rMSCOWIDTH;				// 0x7800_00F8			MSDMA Source Image Width Related
	u32 rMSCOCTRL;				// 0x7800_00FC			MSDMA for Codec Control Register
	u32 rMSPRY0SA;				// 0x7800_0100			MSDMA Y0 Start Address Related(Preview)
	u32 rMSPRCB0SA;				// 0x7800_0104 		MSDMA Cb0 Start Address Related(Preview)
	u32 rMSPRCR0SA;				// 0x7800_0108			MSDMA Cr0 Start Address Related(Preview)
	u32 rMSPRY0END;				// 0x7800_010C			MSDMA Y0 End Address Related(Preview)
	u32 rMSPRCB0END;			// 0x7800_0110			MSDMA Cb0 End Address Related(Preview)
	u32 rMSPRCR0END;			// 0x7800_0114			MSDMA Cr0 End Address Related(Preview)
	u32 rMSPRYOFF;				// 0x7800_0118			MSDMA Y Offset Related(Preview)
	u32 rMSPRCBOFF;				// 0x7800_011C			MSDMA Cb Offset Related(Preview)
	u32 rMSPRCROFF;				// 0x7800_0120			MSDMA Cr Offset Related(Preview)
	u32 rMSPRWIDTH;				// 0x7800_0124			MSDMA Source Image Width RElated(Preview)
	u32 rCIMSCTRL;				// 0x7800_0128			MSDMA Control Register for Preview
	u32 rCICOSCOSY;				// 0x7800_012C			Codec Scan Line Y Offset Related
	u32 rCICOSCOSCB;				// 0x7800_0130			Codec Scan Line Cb Offset Related
	u32 rCICOSCOSCR;				// 0x7800_0134			Codec Scan Line Cr Offset Related
	u32 rCIPRSCOSY;				// 0x7800_0138			Preview Scan Line Y Offset Related
	u32 rCIPRSCOSCB;				// 0x7800_013C 		Preview Scan Line Cb Offset Related
	u32 rCIPRSCOSCR;				// 0x7800_0140			Preview Scna Line Cr Offset Related
	
}oCAMERA_REGS;

CIM oCim;
static volatile u32 CAMTYPE;	

#define SCALER_BYPASS_MAX_HSIZE 4096
#define SCALER_MAX_HSIZE_P 720
#define SCALER_MAX_HSIZE_C 1600
#define OUTPUT_MAX_HSIZE_ROT_RGB_P 320
#define OUTPUT_MAX_HSIZE_ROT_RGB_C 800
#define INPUT_MAX_HSIZE_ROT_P 160
#define INPUT_MAX_VSIZE_ROT_P 120
#define INPUT_MAX_HSIZE_ROT_C 720
#define INPUT_MAX_VSIZE_ROT_C 576


/*
//////////
// Function Name : CAMERA_ReadCameraModuleStatus 
// Function Description : Camerar Module 1.3M(SXGA) 2M(UXGA) IIC Initial Setting Value Write / Read
// Input : 	
// Output :    	None
void CAMERA_ReadCameraModuleStatus(void)
{

#if (CAM_MODEL == CAM_S5K3AA)
	oCim.m_eCcir = CCIR656; 
	oCim.m_eCamSrcFmt = CBYCRY;
	CAMTYPE = 0;
	CAMERA_Read_Write(CAMTYPE,oCim.m_eCcir, oCim.m_eCamSrcFmt, VGA);
#elif (CAM_MODEL == CAM_S5K3BA) ///
	oCim.m_eCcir = CCIR601;
	oCim.m_eCamSrcFmt = YCRYCB; 
	CAMTYPE = 1;
	CAMERA_Read_Write(CAMTYPE,oCim.m_eCcir, oCim.m_eCamSrcFmt, SUB_SAMPLING0);
#else
	Assert(0);
#endif

}
*/

//////////
// Function Name : CAMERA_ClkSetting
// Function Description : Camera CLK output setting. using HCLKx2
// Input : 	None
// Output :    	None
void CAMERA_ClkSetting(void)
{
	u32 uCamCLKDiver;
	u32 uHCLKx2;

	SYSC_GetClkInform();
	UART_Printf("\n----------------------------------------------------------------------------------------\n");
	UART_Printf("ARMCLK: %.2fMHz  HCLKx2: %.2fMHz  HCLK: %.2fMHz  PCLK: %.2fMHz\n",(float)g_ARMCLK/1.0e6, (float)g_HCLKx2/1.0e6, (float)g_HCLK/1.0e6, (float)g_PCLK/1.0e6);	

	uHCLKx2 = (int)(g_HCLKx2/1000000);

	switch ( uHCLKx2)
	{
		case 200:
			UART_Printf("  CAMCLK Source is HCLKx2 = 200 Mhz       \n");
			UART_Printf("  CAMCLK = 200 / (1+7) = 25Mhz Setting    \n");
			uCamCLKDiver = Inp32SYSC(0x20);
			uCamCLKDiver = ( uCamCLKDiver & ~(0xf<<20)) | (7<<20); // 200 Mhz / (9+1) = 25Mhz... 
//			uCamCLKDiver = ( uCamCLKDiver & ~(0xf<<20)) | (12<<20); // 200 Mhz / (9+1) = 20Mhz... 
			Outp32SYSC(0x20, uCamCLKDiver);
			break;
		case 266:
			UART_Printf("  CAMCLK Source is HCLKx2 = 266 Mhz        =");
			UART_Printf("  CAMCLK = 266 / (1+9) = 26Mhz Setting     =");
			uCamCLKDiver = Inp32SYSC(0x20);
			uCamCLKDiver = ( uCamCLKDiver & ~(0xf<<20)) | (10<<20); // 266 Mhz / (9+1) = 26Mhz... 
			Outp32SYSC(0x20, uCamCLKDiver);
			break;
		default:
			UART_Printf("Check HCLKx2 is 200 or 266Mhz!!\n");
			Assert(0);
			break;
	}
	
#if 0	
	// CamCLK Source is HCLK
	UART_Printf("\n===============================");
	UART_Printf("\n=  CAMCLK Source is HCLKx2 = 200 Mhz        =");
	UART_Printf("\n=  CAMCLK = 200 / (1+9) = 20Mhz Setting     =");
	UART_Printf("\n===============================");
	uCamCLKDiver = Inp32SYSC(0x20);
	uCamCLKDiver = ( uCamCLKDiver & ~(0xf<<20)) | (9<<20); // 200 Mhz / (9+1) = 20Mhz... 
	Outp32SYSC(0x20, uCamCLKDiver);
#endif
}


//////////
// Function Name : CAMERA_InitSensor
// Function Description : Camera Moudle Init
// Input : 	None
// Output :    	None
void CAMERA_InitSensor(void)
{
	// 1. Reset sensor
	//==========================================
	CAMERA_ResetSensor(); 
	Delay(5000);
	// 2. Initalize the member variables and initalize the camera model.
	//==========================================
	oCim.m_uIfBits = 8;
#if (CAM_MODEL == CAM_S5K3AA)
	oCim.m_bInvPclk = true, 
	oCim.m_bInvVsync = false, 
	oCim.m_bInvHref = false;
//	oCim.m_uSrcHsz = 640, oCim.m_uSrcVsz = 480;
	oCim.m_uSrcHsz = 1280, oCim.m_uSrcVsz = 1024;	
	oCim.m_eCcir = CCIR656; 
	oCim.m_eCamSrcFmt = CBYCRY;
//	oCim.m_eCamSrcFmt = YCBYCR;	
//	oCim.m_eCamSrcFmt = YCRYCB;	
	CAMTYPE = 0;
//	CAMERA_InitS5K3AAE(oCim.m_eCcir, oCim.m_eCamSrcFmt, SXGA);	
	CAMERA_InitS5K3AAE_VGA();	
//	CAMERA_InitS5K3AAE(oCim.m_eCcir, oCim.m_eCamSrcFmt, SXGA);	

#elif (CAM_MODEL == CAM_S5K3BA) ///
	oCim.m_bInvPclk = false,
	oCim.m_bInvVsync = true,
	oCim.m_bInvHref = false;
//	oCim.m_uSrcHsz = 1600, oCim.m_uSrcVsz = 1200;
//	oCim.m_uSrcHsz = 1280, oCim.m_uSrcVsz = 1024;	
	oCim.m_uSrcHsz = 800, oCim.m_uSrcVsz = 600;	
//	oCim.m_uSrcHsz = 320, oCim.m_uSrcVsz = 240;	
	oCim.m_eCcir = CCIR601;
	oCim.m_eCamSrcFmt = YCBYCR; 
	CAMTYPE = 1;
//	CAMERA_InitS5K4BA_2();
	CAMERA_InitS5K3BAF(oCim.m_eCcir, oCim.m_eCamSrcFmt, SUB_SAMPLING2);	

#elif (CAM_MODEL == CAM_S5K4BA) ///
	/*
	oCim.m_bInvPclk = false,
	oCim.m_bInvVsync = true,
	oCim.m_bInvHref = false;
	*/
	oCim.m_bInvPclk = false,
	oCim.m_bInvVsync = true,
	oCim.m_bInvHref = false;

//	oCim.m_uSrcHsz = 800, oCim.m_uSrcVsz = 600;
//	oCim.m_uSrcHsz = 1280, oCim.m_uSrcVsz = 1024;	
	oCim.m_uSrcHsz = 1600, oCim.m_uSrcVsz = 1200;	
	oCim.m_eCcir = CCIR601;
	oCim.m_eCamSrcFmt = CBYCRY; 
//	oCim.m_eCamSrcFmt = YCRYCB; 
	CAMTYPE = 1;
//	CAMERA_InitS5K4BA(oCim.m_eCcir, oCim.m_eCamSrcFmt, SUB_SAMPLING2);
//	CAMERA_InitS5K4BAF(oCim.m_eCcir, oCim.m_eCamSrcFmt, SUB_SAMPLING2);	
	CAMERA_InitS5K4BAF2(oCim.m_eCcir, oCim.m_eCamSrcFmt, SUB_SAMPLING2);	
#elif (CAM_MODEL == CAM_A3AFX_VGA) ///
	oCim.m_bInvPclk = false,
	oCim.m_bInvVsync = true,
	oCim.m_bInvHref = false;
	oCim.m_uSrcHsz = 320, oCim.m_uSrcVsz = 240;
//	oCim.m_uSrcHsz = 176, oCim.m_uSrcVsz = 144;	
	oCim.m_eCcir = CCIR601;
	oCim.m_eCamSrcFmt = YCBYCR; 	
	CAMERA_InitA3AFX_QVGA_20FR();
//	CAMERA_InitA3AFX_QVGA_15FR();	
//	CAMERA_InitA3AFX_QCIF_30FR();
//	CAMERA_InitA3AFX_QCIF_15FR();


#elif (CAM_MODEL == CAM_S5K4CA) ///
	#if 1 
	oCim.m_bInvPclk = false,
	oCim.m_bInvVsync = true,
	oCim.m_bInvHref = false;
	oCim.m_uSrcHsz = 2048, oCim.m_uSrcVsz = 1536;
//	oCim.m_uSrcHsz = 176, oCim.m_uSrcVsz = 144;	
	oCim.m_eCcir = CCIR601;
	oCim.m_eCamSrcFmt = CBYCRY; 	
	CAMERA_Init_K5K4CA();
	#else
	oCim.m_bInvPclk = false,
	oCim.m_bInvVsync = true,
	oCim.m_bInvHref = false;
	oCim.m_eCcir = CCIR656;
	oCim.m_uSrcHsz = 2048, oCim.m_uSrcVsz = 1536;
//	oCim.m_uCamInImgHsz = 2048, oCim.m_uCamInImgVsz = 1536;
	oCim.m_eCamSrcFmt = CBYCRY;
	oCim.m_eCamMode = ITU;
	CAMERA_InitS5K4CAGX(oCim.m_eItuR, oCim.m_eCamSrcFmt, oCim.m_eCamMode);
	#endif


#else
	Assert(0);
#endif

}
	

//////////
// Function Name : CAMERA_InitSensor1
// Function Description : This Function use at JPEG Test. 
// Input : 	IMG_SIZE eSize(Image Size), CAM_ATTR eCcir(Source YCBCR Format), CSPACE eSrcFmt(Source Format)
// Output :    	None
void CAMERA_InitSensor1(IMG_SIZE eSize, CAM_ATTR eCcir, CSPACE eSrcFmt)
{
	SUB_SAMPLING eSub; 
	// 1. Reset sensor
	//==========================================
	CAMERA_ResetSensor();

	// 2. Initalize the member variables and initalize the camera model.
	//==========================================
	oCim.m_uIfBits = 8;
	oCim.m_eCcir = eCcir;
	oCim.m_eCamSrcFmt = eSrcFmt;

	if (eSize == UXGA)
		oCim.m_uSrcHsz = 1600, oCim.m_uSrcVsz = 1200; 
	else if (eSize == SXGA)
		oCim.m_uSrcHsz = 1280, oCim.m_uSrcVsz = 1024; 
	else if (eSize == SVGA)
		oCim.m_uSrcHsz = 800, oCim.m_uSrcVsz = 600; 
	else if (eSize == VGA)
		oCim.m_uSrcHsz = 640, oCim.m_uSrcVsz = 480; 
	else if (eSize == QVGA)
		oCim.m_uSrcHsz = 320, oCim.m_uSrcVsz = 240; 
	else if (eSize == QQVGA)
		oCim.m_uSrcHsz = 160, oCim.m_uSrcVsz = 120; 
	else if (eSize == CIF)
		oCim.m_uSrcHsz = 352, oCim.m_uSrcVsz = 288; 
	else if (eSize == QCIF)
		oCim.m_uSrcHsz = 172, oCim.m_uSrcVsz = 144; 
	else
	{
		Assert(0);
	}

#if (CAM_MODEL == CAM_S5K3AA)

	Assert( oCim.m_eCcir == CCIR656);
	oCim.m_bInvPclk = true, 
	oCim.m_bInvVsync = false,
	oCim.m_bInvHref = false;
	oCim.m_eCamSrcFmt = CBYCRY;
	
//	CAMERA_InitS5K3AAE(oCim.m_eCcir, oCim.m_eCamSrcFmt, SXGA);

#elif (CAM_MODEL == CAM_S5K4AAF)
	oCim.m_bInvPclk = false,
	oCim.m_bInvVsync = false,
	oCim.m_bInvHref = false;
	oCim.m_eCamSrcFmt = YCRYCB;  // change camif setting instead of cam module.

	oCim.m_oCamera.InitS5K4AAF(oCim.m_eCcir, oCim.m_eCamSrcFmt, eSize);
#elif (CAM_MODEL == CAM_S5K3BA)

	Assert( oCim.m_eCcir == CCIR601);
	oCim.m_bInvPclk = false, 
	oCim.m_bInvVsync = true, 
	oCim.m_bInvHref = false;
	oCim.m_eCamSrcFmt = YCRYCB;


	switch(eSize)
	{
		case UXGA:case SXGA:
			eSub = SUB_SAMPLING0;
			oCim.m_uSrcHsz = 1600; oCim.m_uSrcVsz = 1200;
			break;
		case SVGA:case VGA:
			eSub = SUB_SAMPLING2;
			oCim.m_uSrcHsz = 800; oCim.m_uSrcVsz = 600;
			break;
		case QVGA:case QQVGA:case CIF:case QCIF:
			eSub = SUB_SAMPLING4;
			oCim.m_uSrcHsz = 400; oCim.m_uSrcVsz = 300;
			break;
		default: 
			Assert(0);
			break;
	}
	CAMERA_InitS5K3BAF(oCim.m_eCcir, oCim.m_eCamSrcFmt, eSub);
#elif (CAM_MODEL == CAM_OV7620)
	oCim.m_bInvPclk = false, 
	oCim.m_bInvVsync = false, 
	oCim.m_bInvHref = false;
	oCim.m_uSrcHsz = 640, oCim.m_uSrcVsz = 480;
	oCim.m_eCamSrcFmt = YCBYCR;

	CAMERA_InitOV7620(oCim.m_eCcir, oCim.m_uIfBits, false, oCim.m_eCamSrcFmt);
#else
	Assert(0);
#endif
}

void CAMERA_InitSensor_2(void)
{
	// 1. Reset sensor
	//==========================================
	CAMERA_ResetSensor(); 
	Delay(5000);
	// 2. Initalize the member variables and initalize the camera model.
	//==========================================
	oCim.m_uIfBits = 8;
	oCim.m_bInvPclk = true,
	oCim.m_bInvVsync = false,
	oCim.m_bInvHref = TRUE;
	oCim.m_uSrcHsz = 800, oCim.m_uSrcVsz = 480;
	oCim.m_eCcir = CCIR601;
	oCim.m_eCamSrcFmt = YCBYCR; 
	CAMTYPE = 1;

}



//////////
// Function Name : CAMERA_SetSensorSize
// Function Description : this function use at JPEG Test.
// Input : 	IMG_SIZE eSize(Image Size)
// Output :    	None
void CAMERA_SetSensorSize(IMG_SIZE eSize)
{
	SUB_SAMPLING eSub; 

	// 1. Set the oCim.m_uSrcHsz & oCim.m_uSrcVsz according to eSize
	//=======================================
	if (eSize == UXGA)
		oCim.m_uSrcHsz = 1600, oCim.m_uSrcVsz = 1200; 
	else if (eSize == SXGA)
		oCim.m_uSrcHsz = 1280, oCim.m_uSrcVsz = 1024;
	else if (eSize == SVGA)
		oCim.m_uSrcHsz = 800, oCim.m_uSrcVsz = 600; 
	else if (eSize == VGA)
		oCim.m_uSrcHsz = 640, oCim.m_uSrcVsz = 480; 
	else if (eSize == QVGA)
		oCim.m_uSrcHsz = 320, oCim.m_uSrcVsz = 240; 
	else if (eSize == QQVGA)
		oCim.m_uSrcHsz = 160, oCim.m_uSrcVsz = 120; 
	else if (eSize == CIF)
		oCim.m_uSrcHsz = 352, oCim.m_uSrcVsz = 288; 
	else if (eSize == QCIF)
		oCim.m_uSrcHsz = 172, oCim.m_uSrcVsz = 144; 
	else
	{
		Assert(0);
	}

	// 2. Set the size in camera according to the camera model and eSize.
	//=============================================
#if (CAM_MODEL == CAM_S5K4AAF)
	oCim.m_eCamSrcFmt = (eSize == SXGA) ? YCBYCR : YCRYCB;
	oCim.m_oCamera.SetSizeInS5K4AAF(eSize);
#elif (CAM_MODEL == CAM_S5K3BA)

	switch(eSize)
	{
		case UXGA:case SXGA:
			eSub = SUB_SAMPLING0;
			oCim.m_uSrcHsz = 1600; oCim.m_uSrcVsz = 1200;
			break;
		case SVGA:case VGA:
			eSub = SUB_SAMPLING2;
			oCim.m_uSrcHsz = 800; oCim.m_uSrcVsz = 600; 
			break;
		case QVGA:case QQVGA:case CIF:case QCIF:
			eSub = SUB_SAMPLING4;
			oCim.m_uSrcHsz = 400; oCim.m_uSrcVsz = 300;
			break;
		default: 
			Assert(0);
			break;
	}
	CAMERA_SetSizeInS5K3BAF(eSub);
#elif (CAM_MODEL == CAM_OV7620)

#elif (CAM_MODEL == CAM_S5K3AA)
	CAMERA_SetSizeInS5K3AAE(eSize);
#else
	Assert(0);
#endif
}




//////////
// Function Name : CAMERA_ResetIp
// Function Description : Camera Interface reset function
// 					In ITU601 Case, ITU601_656n: 1 -> SwRst: 1 -> SwRst: 0
// 					In ITU656 Case, ITU601_656n: 1 -> SwRst: 1 -> SwRst: 0 -> ITU601_656n: 0
// Camera interface SW Reset Sequence
// Input : 	None
// Output :    	None
void CAMERA_ResetIp(void)
{
	u32 uSrcFmtRegVal = 0;
	u32 uCtrlRegVal = 0;
	
	uSrcFmtRegVal |= (1U<<31);
	Outp32(&CAMERAR->rCISRCFMT, uSrcFmtRegVal);

	uCtrlRegVal = Inp32(&CAMERAR->rCIGCTRL);
	//Inp32(&CAMERAR->rCIGCTRL, uCtrlRegVal); // Extremely important!! If deleted, cam module will be reset.
	uCtrlRegVal |= (1U<<31);
	Outp32(&CAMERAR->rCIGCTRL, uCtrlRegVal);

	uCtrlRegVal &= ~(1U<<31);
	Outp32(&CAMERAR->rCIGCTRL, uCtrlRegVal);

	if(oCim.m_eCcir == CCIR656)
	{
		uSrcFmtRegVal &= ~(1U<<31);
		Outp32(&CAMERAR->rCISRCFMT, uSrcFmtRegVal);
	}
}



//////////
// Function Name : CAMERA_ResetSencor
// Function Description : Camera Module reset function
// Input : 	None
// Output :    	None
void CAMERA_ResetSensor(void)
{
	u32 uCIGCTRL;
	u32 uDelay;
	
#if (CAM_MODEL == CAM_OV7620)
	oCim.m_bHighRst = true;
//#elif (CAM_MODEL == CAM_S5K3AA || CAM_MODEL == CAM_S5K3BA || CAM_MODEL == CAM_S5K4AAF) // jungil
#elif (CAM_MODEL == CAM_S5K3AA || CAM_MODEL == CAM_S5K3BA ||CAM_MODEL == CAM_S5K4BA || CAM_MODEL == CAM_S5K4AAF || CAM_MODEL == CAM_A3AFX_VGA || CAM_MODEL == CAM_S5K4CA)
	oCim.m_bHighRst = false; 
#else
	Assert(0);
#endif 

	uCIGCTRL = Inp32(&CAMERAR->rCIGCTRL);
	if (oCim.m_bHighRst)
	{
		uCIGCTRL |= (1<<30);
		Outp32(&CAMERAR->rCIGCTRL, uCIGCTRL);
		for(uDelay = 0 ; uDelay<1000000 ; uDelay++);
		uCIGCTRL &= ~(1<<30);
		Outp32(&CAMERAR->rCIGCTRL, uCIGCTRL);		
		for(uDelay = 0 ; uDelay<1000000 ; uDelay++);		
		
	}
	else
	{
		uCIGCTRL &= ~(1<<30);
		Outp32(&CAMERAR->rCIGCTRL, uCIGCTRL);		
		for(uDelay = 0 ; uDelay<1000000 ; uDelay++);		
		uCIGCTRL |= (1<<30);
		Outp32(&CAMERAR->rCIGCTRL, uCIGCTRL);		
		for(uDelay = 0 ; uDelay<1000000 ; uDelay++);		
	}

}



//////////
// Function Name : CAMERA_SetBasicSft
// Function Description : Camera Interface(Preview/Codec) Basic SFR setting function
// Input : 	u32 uSrcCropStartX(Windoe Offset X1), u32 uSrcCropStartY(Window Offset Y1), u32 uSrcCropHsz(Wanted Source Crop H Size)
//                 u32 uSrcCropVsz(wanted soure V Size), u32 uDstHsz(Destination H Size), u32 uDstVsz(Destination VSzie)
//                 u32 uDstAddr0(Dstination DMA Address 0), u32 uDstAddr1(Destination DMA Address 1), CSPACE eDstDataFmt(Wanted Data Format),
//	              CAMIF_INOUT eInputPath(Input Data source), PROCESS_PATH ePath(Process Path), CAMIF_INOUT eOutputMode(output mode: DMA/FIFO)
//                , FLIP_DIR eFlip(Flip Mode Selection), ROT_DEG eRotDeg(Rotation mode Selection)
// Output :    	None
void CAMERA_SetBasicSfr(u32 uSrcCropStartX, u32 uSrcCropStartY, u32 uSrcCropHsz, u32 uSrcCropVsz,
	u32 uDstHsz, u32 uDstVsz, u32 uDstAddr0, u32 uDstAddr1, CSPACE eDstDataFmt,
	CAMIF_INOUT eInputPath, PROCESS_PATH ePath, CAMIF_INOUT eOutputMode, FLIP_DIR eFlip, ROT_DEG eRotDeg)
{
	u32 uSrcHOffset1, uSrcVOffset1, uSrcHOffset2, uSrcVOffset2;
	u32 uSrcCroppingHSz, uSrcCroppingVSz;
	u32 uFlip;
	u8 bIsHScaleUp;
	u8 bIsVScaleUp;	
	u32 uRemainBurstLen=0;
	u32 uCMainBurstLen =0;
	u32 uCRemainBurstLen = 0;
	u32 uPreHRatio, uPreVRatio;
	u32 uMainHRatio, uMainVRatio; 
	u32 uShiftFactor;
	u32 uPreDstHSz, uPreDstVSz;
	u32 uHShift, uVShift;
	u32 uLimitHsz;
	u32 uInputIntlvOrder=0;
	u32 uWinOfsEn;
	int nSrcFmt = 0;
	int nSrcYCbCrOrder;
	int nOutputIntlvOrder = 0;
	int nOutRGBFMTPr = 3 ;
	int nOutFormatPr;
	u32 uOutCbCrSz;
	u32 uOutYSz;
	u32 uTargetFmt;
	u32 uDMAControl;
	u32 uPreScalerCtrl1;
	u32 uPreScalerCtrl2;

	Assert(oCim.m_uSrcHsz != 0 && oCim.m_uSrcVsz != 0);

	// 1. Set input path, output path, process path and destination format 
	//=================================================
	oCim.m_eInputPath = eInputPath;
	oCim.m_eProcessPath = ePath;
	oCim.m_eOutputPath = eOutputMode;

	if (oCim.m_eOutputPath==DMA)
	{
		Assert(eDstDataFmt ==YCBYCR || eDstDataFmt==YCRYCB ||eDstDataFmt ==CRYCBY ||eDstDataFmt==CBYCRY
			|| eDstDataFmt==RGB16 || eDstDataFmt==RGB18 ||eDstDataFmt==RGB24 
			|| eDstDataFmt==YC420 || eDstDataFmt==YC422 );
		oCim.m_eDstFmt = eDstDataFmt;
	}
	else 
	{
		oCim.m_eDstFmt = eDstDataFmt; // In fifo output path
	}

	// 2. Compute the horizontal offset and vertical offset, check whether offset size and source size meet the limitation
	//=========================================================================
	uSrcHOffset1 = uSrcCropStartX;
	uSrcVOffset1 = uSrcCropStartY;

	uSrcHOffset2 = oCim.m_uSrcHsz - (uSrcCropHsz + uSrcHOffset1);
	uSrcVOffset2 = oCim.m_uSrcVsz - (uSrcCropVsz + uSrcVOffset1);

	Assert((uSrcHOffset1%2 == 0)); // manual p24 Window horizontal offset1 should be 2's multiple!
	Assert((uSrcHOffset2%2 == 0)); // manual p27 Window horizontal offset2 should be 2's multiple!
	Assert(oCim.m_uSrcHsz >= uSrcHOffset1 + uSrcCropHsz); // Invid horizontal size of original source or cropped window or start coordinate X
	Assert(oCim.m_uSrcVsz >= uSrcVOffset1 + uSrcCropVsz); // Invid vertical size of original source or cropped window or start coordinate Y
	if (oCim.m_eInputPath==FIFO)
	{
		Assert(oCim.m_uSrcHsz%8 == 0 && oCim.m_uSrcHsz >= 16 && oCim.m_uSrcVsz >= 8); // manual p23
	}

	// 3. Set input & output rotator degree and flip
	//=============================================
	oCim.m_eInRotDeg = ROT_0;
	oCim.m_eOutRotDeg = eRotDeg;
	if (eRotDeg==ROT_180 || eRotDeg==ROT_270)
	{
		Assert(eFlip==FLIP_NO);
	}
	if (eRotDeg==ROT_180)
	{
		eRotDeg = ROT_0;
		eFlip=FLIP_XY;
	}
	if(eRotDeg==ROT_270)
	{
		eRotDeg = ROT_90;
		eFlip=FLIP_XY;
	}
	uFlip = (eFlip == FLIP_X) ? 1 : (eFlip == FLIP_Y) ? 2 : (eFlip == FLIP_XY) ? 3 : 0;
	if (oCim.m_eOutputPath==FIFO)
	{ 
		Assert(uFlip == FLIP_NO);
		if (oCim.m_eInputPath==DMA && oCim.m_eMemSrcFmt==YC420)//just YC420 support input-rotation
			oCim.m_eInRotDeg = eRotDeg;
		else
		{
			Assert(eRotDeg== ROT_0);
		}
	}
	else
		oCim.m_eOutRotDeg = eRotDeg;

	// 4. Set destination size, check whether the destination size meet the limitation.
	//======================================================
	if (oCim.m_eOutRotDeg==ROT_0)
	{
		oCim.m_uDstHsz = uDstHsz;
		oCim.m_uDstVsz = uDstVsz;
	}
	else if (oCim.m_eOutRotDeg==ROT_90 )
	{
		oCim.m_uDstHsz = uDstVsz;
		oCim.m_uDstVsz = uDstHsz;
	}
	else
	{
		Assert(0);
	}

/*
if (oCim.m_eInputPath==FIFO) // Manual p44 
	{
		Assert(oCim.m_uDstHsz <= oCim.m_uSrcHsz); // Horiztonal size of destnation should be less than horizontal size of source
		Assert(oCim.m_uDstVsz <= oCim.m_uSrcVsz); // Vertical size of destnation should be less than the vertical size of source
	}
*/

	// 5. Set cropping size, check whether the cropping size meet the limitation.
	//====================================================
	if (oCim.m_eInRotDeg==ROT_0)
	{
		uSrcCroppingHSz = uSrcCropHsz;
		uSrcCroppingVSz = uSrcCropVsz;
	}
	else if (oCim.m_eInRotDeg==ROT_90 )
	{
		uSrcCroppingHSz = uSrcCropVsz;
		uSrcCroppingVSz = uSrcCropHsz;
	}
	else
	{
		Assert(0);
	}
	// manual p25 Source corpping horizontal size should be 8's multiple and minimum 16!
	Assert(uSrcCroppingHSz%8 == 0 && uSrcCroppingHSz >= 16); 


	// 6. Check whether destination size meet the limitation.
	//===================================================
	if (oCim.m_eOutputPath==DMA) // manual p31, p44
	{
		if (oCim.m_eOutRotDeg==ROT_0)
		{
			Assert(oCim.m_uDstHsz%16 == 0 && oCim.m_uDstHsz >= 16 && oCim.m_uDstVsz >= 4); 
		}
		else if(oCim.m_eOutRotDeg==ROT_90)
		{
			if (oCim.m_eProcessPath==P_PATH)
			{
				if ((oCim.m_eDstFmt==RGB24|| oCim.m_eDstFmt == RGB18) && oCim.m_uDstHsz>OUTPUT_MAX_HSIZE_ROT_RGB_P/2)
				{
					Assert(oCim.m_uDstVsz%4 ==0 ); // When output is RGB24 & h_width>160, Target vertical size should be 4's multiple!
				}
				else
				{
					Assert(oCim.m_uDstVsz%8 ==0); // When set rotation, Target vertical size should be 8's multiple!
				}
			}
			else //oCim.m_eProcessPath==C_PATH
			{
				if ((oCim.m_eDstFmt==RGB24|| oCim.m_eDstFmt == RGB18) && oCim.m_uDstHsz>OUTPUT_MAX_HSIZE_ROT_RGB_C/2)
				{
					Assert(oCim.m_uDstVsz%4 == 0); // When output is RGB24 & TargheHsize>400, Target vertical size should be 4's multiple!
				}
				else
				{
					Assert(oCim.m_uDstVsz%8 ==0); // When set rotation, Target vertical size should be 8's multiple!
				}
			}
		}
		else
		{
			Assert(0);
		}
	}

	// 7. Set bIsHScaleUp/bIsVScaleUp and oCim.m_bIsScalerBypass
	//===========================================
	bIsHScaleUp = (uSrcCroppingHSz <= oCim.m_uDstHsz) ? true : false; // Horizontal scale up/down - 1: Up, 0: Down
	bIsVScaleUp = (uSrcCroppingVSz <= oCim.m_uDstVsz) ? true : false; // Vertical scale up/donw - 1: up, 0: down

	if (oCim.m_eInputPath==FIFO && oCim.m_eOutputPath==DMA)
	{
		if (uSrcCroppingHSz==oCim.m_uDstHsz && uSrcCroppingVSz==oCim.m_uDstVsz)
		{
			if (oCim.m_eDstFmt==RGB16 || oCim.m_eDstFmt==RGB18 || oCim.m_eDstFmt==RGB24)
				oCim.m_bIsScalerBypass = false;
			else
				oCim.m_bIsScalerBypass = true;
		}
		else
			oCim.m_bIsScalerBypass = false;
	}
	else
		oCim.m_bIsScalerBypass = false;

	// 8. Check whether oCim.m_bIsScalerBypass, oCim.m_eOutRotDeg and destination size meet the limitation
	//================================================================
	if (oCim.m_bIsScalerBypass)
	{
		Assert(oCim.m_eOutRotDeg == ROT_0); // When ScalerBypass, can not support rotator!
	}
		
	if (oCim.m_eInputPath==DMA)
	{
		Assert(oCim.m_bIsScalerBypass!=true); // Memory input can not support ScalerBypass!
	}

	if (oCim.m_eProcessPath==P_PATH) // The max horizontal size.-----page 3.
	{
		if (oCim.m_eOutRotDeg==ROT_0 )
		{
			if (oCim.m_bIsScalerBypass) 
			{
				Assert(oCim.m_uDstHsz <= SCALER_BYPASS_MAX_HSIZE); //In Pre-view Path, if scaler bypass, the DstHsize should <4096!
			}
			else 
			{
				Assert(oCim.m_uDstHsz <= SCALER_MAX_HSIZE_P); //In Pre-view Path, if scaler is used, the DstHsize should <640!
			}
		}
		else if (oCim.m_eOutRotDeg==ROT_90 ) 
		{	 
			// In Pre-view Path, if rotation & output is RGB format, the DstHsize should <320! 
				// if rotation & output is not RGB format, the DstHsize should <160!
			if (oCim.m_eDstFmt==RGB16||oCim.m_eDstFmt==RGB18||oCim.m_eDstFmt==RGB24)
			{
				Assert(oCim.m_uDstHsz <= OUTPUT_MAX_HSIZE_ROT_RGB_P);
			}
			else 
			{
				Assert(oCim.m_uDstHsz <= OUTPUT_MAX_HSIZE_ROT_RGB_P/2);
			}
		}
		else
		{
			Assert(0);
		}

		if (oCim.m_eInRotDeg==ROT_90 )
		{
			Assert(uSrcCropHsz<=INPUT_MAX_HSIZE_ROT_P&& uSrcCropVsz<=INPUT_MAX_VSIZE_ROT_P);
		}
		
		if (oCim.m_bIsScalerBypass)//In Pre-view Path, if scaler bypass, the scaler input horizontal size should <4096!
		{
			Assert(uSrcCroppingHSz <= SCALER_BYPASS_MAX_HSIZE); 
		}
		
	} 
	else // oCim.m_eProcessPath == C_PATH
	{ 
		if (oCim.m_eOutRotDeg==ROT_0 )
		{
			if (oCim.m_bIsScalerBypass)
			{
				Assert(oCim.m_uDstHsz <= SCALER_BYPASS_MAX_HSIZE); //In Codec Path, if scaler bypass, the DstHsize should <4096!
			}
			else
			{
				Assert(oCim.m_uDstHsz <= SCALER_MAX_HSIZE_C); // In Codec Path, if scaler is used, the DstHsize should <1600!
			}
		}
		else if (oCim.m_eOutRotDeg==ROT_90 )
		{
			//In Codec Path, if rotation & output is RGB format, the DstHsize should <800!
				// if rotation & output is not RGB format, the DstHsize should <400!
			if (oCim.m_eDstFmt==RGB16 || oCim.m_eDstFmt==RGB18 || oCim.m_eDstFmt==RGB24)
			{
				Assert(oCim.m_uDstHsz <= OUTPUT_MAX_HSIZE_ROT_RGB_C); 
			}
			else 
			{
				Assert(oCim.m_uDstHsz <= OUTPUT_MAX_HSIZE_ROT_RGB_C/2); 
			}
		}
		else
		{
			Assert(0);
		}

		if (oCim.m_eInRotDeg==ROT_90 )
		{
			Assert(uSrcCropHsz<=INPUT_MAX_HSIZE_ROT_C && uSrcCropVsz<=INPUT_MAX_VSIZE_ROT_C);
		}

		if (oCim.m_bIsScalerBypass) //In Codec Path, if scaler bypass, the scaler input horizontal size should <4096!
		{
			Assert(uSrcCroppingHSz <= SCALER_BYPASS_MAX_HSIZE); 
		}
	}

	// 9. Compute the burst size
	//======================================

	if ( oCim.m_eDstFmt == RGB16 || oCim.m_eDstFmt==RGB18 || oCim.m_eDstFmt == RGB24)
		CAMERA_CalcRgbBurstSize(&oCim.m_uMainBurstSz, &uRemainBurstLen);
	else// YCbCr format
		CAMERA_CalcYCbCrBurstSize( BURST16, &oCim.m_uMainBurstSz, &uRemainBurstLen, &uCMainBurstLen, &uCRemainBurstLen); 

	// 10. Calculate the pre-scaler ratio and shift factor
	//=======================================

	if (!oCim.m_bIsScalerBypass)
	{

		
		CAMERA_CalcRatioAndShift(uSrcCroppingHSz, oCim.m_uDstHsz, &uPreHRatio, &uHShift);
		CAMERA_CalcRatioAndShift(uSrcCroppingVSz, oCim.m_uDstVsz, &uPreVRatio, &uVShift);
		
		uShiftFactor = 10 - (uHShift + uVShift);
		uPreDstHSz = uSrcCroppingHSz/uPreHRatio;
		uPreDstVSz = uSrcCroppingVSz/uPreVRatio;
		if (oCim.m_eProcessPath==P_PATH)
		{
			Assert(uPreDstHSz <= SCALER_MAX_HSIZE_P); //In Pre-view Path, if use scaler, the scaler input horizontal size should <640!
		}
		else
		{
			Assert(uPreDstHSz <= SCALER_MAX_HSIZE_C); //In Codec Path, if use scaler, the scaler input horizontal size should <1600!
		}

		uMainHRatio = (uSrcCroppingHSz<<8)/(oCim.m_uDstHsz<<uHShift);
		uMainVRatio = (uSrcCroppingVSz<<8)/(oCim.m_uDstVsz<<uVShift); 

		uLimitHsz = (oCim.m_eProcessPath==P_PATH) ? SCALER_MAX_HSIZE_P: SCALER_MAX_HSIZE_C;
		Assert((int)(uSrcHOffset1+uSrcHOffset2) >= (int)(oCim.m_uSrcHsz - uLimitHsz*uPreHRatio)); // Unsupported horizontal offset size
		Assert((uSrcCroppingHSz % (4*uPreHRatio) == 0)); //Unsupported horizontal size of cropped window
		
		if (!bIsHScaleUp) // Scaler down
		{
			Assert(uSrcCroppingVSz % (2*uPreVRatio) == 0); //Unsupported vertical size of cropped window
		}
			
		if (oCim.m_eDstFmt==YC420)
		{
			Assert(uSrcCroppingVSz%2==0 && uSrcCroppingVSz >= 8); // Crop Vsize must be an even number and minimum 8!
		}
		
	}
	else
	{
		uPreHRatio = 1, uPreVRatio = 1, uMainHRatio=1, uMainVRatio=1;
		uShiftFactor = 10, uPreDstHSz =uSrcCroppingHSz, uPreDstVSz=uSrcCroppingVSz;
	}

	// 11. Set the register of the source 
	//====================================
	oCim.m_uCigCtrl = ((oCim.m_bHighRst ? 0 : 1)<<30) 	// Camera A reset
		|(1<<29)						 		// Select Camera - 1: Cam A, 0: Cam B
		|(BY_PASS<<27) 							// Test Pattern - 00: BY_PASS 01: COLOR_BAR 10: HORIZONTAL INCR 11: VERTICAL INCR
		|((oCim.m_bInvPclk ? 1: 0)<<26) 				// 1:inverse PCLK
		|((oCim.m_bInvVsync ? 1: 0)<<25)				// 1:inverse VSYNC
		|((oCim.m_bInvHref ? 1: 0)<<24) 				// 1:inverse HREF
		|(0<<22) 								// Overflow Interrupt - 1: Enable, 0: Disalbe 
		|(1<<20)								// Level Interrupt Enable
		|(1<<21); 								// Href Mask - 1: Mask Out Href during Vsync High, 0: No Mask
	Outp32(&CAMERAR->rCIGCTRL, oCim.m_uCigCtrl);

	if (oCim.m_eInputPath==FIFO) //Camera input
	{ 

		if (oCim.m_uIfBits == 8)
		{
			uInputIntlvOrder = 
				(oCim.m_eCamSrcFmt == YCBYCR) ? 0 :
				(oCim.m_eCamSrcFmt == YCRYCB) ? 1 :
				(oCim.m_eCamSrcFmt == CBYCRY) ? 2 :
				(oCim.m_eCamSrcFmt == CRYCBY) ? 3 : 0;
		}
		else // oCim.m_uIfBits == 16
		{
			uInputIntlvOrder = (oCim.m_eCamSrcFmt== YCBYCR) ? 0 : (oCim.m_eCamSrcFmt == YCRYCB) ? 1 : 0xffffffff;

			if (uInputIntlvOrder == 0xffffffff)
			{
				Assert(0); // Unsupported YCbYCr 422 order in CCIR601 16 bit mode
			}
		}
		
		Outp32(&CAMERAR->rCISRCFMT, 						// Set source format reg.
			((oCim.m_eCcir == CCIR601 ? 1 : 0)<<31) 	// 0: CCIR656, 1: CCIR601
			|(0<<30) 							// UVOffset - 1: +128, 0:+0 (normally used) 
			|((oCim.m_uIfBits == 16 ? 1 : 0)<<29)	// CCIR601 Bits - 1: 16bits, 0: 8bits
			|(oCim.m_uSrcHsz<<16) 
			|(uInputIntlvOrder<<14) 
			|(oCim.m_uSrcVsz<<0));

		uWinOfsEn = ((uSrcHOffset1==0)&&(uSrcHOffset2==0))? 0: 1 ;
		Outp32(&CAMERAR->rCIWDOFST, 			// Set window offset register 
			(uWinOfsEn<<31) 		// window offset - 1: Enable, 0: No Offset
			|(uSrcHOffset1<<16) 	// Window horizontal offset1 
			|(uSrcVOffset1<<0)); 	// Window Vertical offset1 
				
		Outp32(&CAMERAR->rCIDOWFST2, (uSrcHOffset2<<16)|uSrcVOffset2<<0); // window horizontal offset2 and window vertical offset4 
	
		Outp32(&CAMERAR->rCIIMGCPT, 0);		 // Image Cqtpure enable 

		Outp32(&CAMERAR->rCICPTSEQ, 0xffffffff); // set capture sequence reg. 
		
		if (oCim.m_eProcessPath==P_PATH)
		{
			Outp32(&CAMERAR->rCIMSCTRL, 0);
		}

		if (oCim.m_eProcessPath==C_PATH)
		{
			Outp32(&CAMERAR->rMSCOCTRL, 0);
		}

	}
	else // When input come from memory, the address can be computed by the starty_addr, startcr_addr, endy_addr....
	{
		Outp32(&CAMERAR->rCIIMGCPT, 0); // Image Cqtpure enable 

		nSrcYCbCrOrder = (oCim.m_eMemSrcFmt ==YCBYCR) ? 3 :
				(oCim.m_eMemSrcFmt ==YCRYCB) ? 1 :
				(oCim.m_eMemSrcFmt ==CBYCRY) ? 2 : 0;

		if (oCim.m_eMemSrcFmt == RGB16||oCim.m_eMemSrcFmt == RGB18||oCim.m_eMemSrcFmt == RGB24)
			nSrcFmt = 3;
		else if (oCim.m_eMemSrcFmt == YC420)
			nSrcFmt = 0;
		else if (oCim.m_eMemSrcFmt == YC422)
			nSrcFmt = 1;
		else
			nSrcFmt = 2;
		oCim.m_uMSDMACtrl = (nSrcYCbCrOrder<<4)|(1<<3)|(nSrcFmt<<1); // Set MSDMA controller
		if (oCim.m_eProcessPath==P_PATH)
			Outp32(&CAMERAR->rCIMSCTRL, oCim.m_uMSDMACtrl);
		else
			Outp32(&CAMERAR->rMSCOCTRL, oCim.m_uMSDMACtrl);
	}

	// 12. Set the register of the output 
	//===============================================

	nOutFormatPr = (oCim.m_eDstFmt== YC420) ? 0 :(oCim.m_eDstFmt== YC422) ? 1 :
		((oCim.m_eDstFmt==RGB16)||(oCim.m_eDstFmt==RGB18)||(oCim.m_eDstFmt == RGB24)) ? 3 : 2;

	if (oCim.m_eOutputPath==DMA)
	{
		if (nOutFormatPr==3)
			nOutRGBFMTPr = 
				(oCim.m_eDstFmt ==RGB16) ? 0 : 
				(oCim.m_eDstFmt ==RGB18) ? 1 : 
				(oCim.m_eDstFmt ==RGB24) ? 2 : 3;
		else if (nOutFormatPr==2)
			nOutputIntlvOrder = 
				(oCim.m_eDstFmt == YCBYCR) ? 0 : 
				(oCim.m_eDstFmt == YCRYCB) ? 1 : 
				(oCim.m_eDstFmt == CBYCRY) ? 2 : 3; 
		else
			{}
	}
	

	uOutYSz = (oCim.m_uDstHsz*oCim.m_uDstVsz);
	if (oCim.m_eDstFmt == YC422)
		uOutCbCrSz = (oCim.m_uDstHsz/2*oCim.m_uDstVsz)*1;
	else if (oCim.m_eDstFmt == YC420)
		uOutCbCrSz = (oCim.m_uDstHsz/2*oCim.m_uDstVsz/2)*1; 
	else
		uOutCbCrSz = 0;
	
	uTargetFmt = ((oCim.m_eInRotDeg==ROT_90? 1 : 0)<<31)	// 1: Input Rotate clockwize 90. 0: Input Rotator bypass.
		|(nOutFormatPr<<29) 
		|((oCim.m_uDstHsz&0x1fff)<<16)						// Horizontal size of destination 
		|(uFlip<<14) 									// 0: Normal, 1: X-axis mirror, 2: Y-axis mirror, 3: XY-axis(180 degrees) mirror
		|((oCim.m_eOutRotDeg==ROT_90 ? 1 : 0)<<13) 			// 1: Output Rotate clockwize 90. 0: Output Rotator bypass.
		|((oCim.m_uDstVsz&0x1fff)<<0);						// veritcal size of destination

	uDMAControl = (oCim.m_uMainBurstSz<<19)				// Main burst of Y length
		|(uRemainBurstLen<<14) 							// Remain burst of Y length
		|(uCMainBurstLen<<9) 							// Main burst of Cb/Cr length
		|(uCRemainBurstLen<<4)							// Remain burst of Cb/Cr length
		|(0<<2) 										// 1: Enable last IRQ at the end of frame capture, 0: normal
		|(nOutputIntlvOrder<<0); 						// YCbYCr_interleave order

	uPreScalerCtrl1 = (uShiftFactor<<28) 			// shift factor for preview
		|(uPreHRatio<<16) 								// Horizontal ratio of preview pre-scaler
		|(uPreVRatio<<0); 								// Vertical ratio of preview pre-scaler

	uPreScalerCtrl2 = (uPreDstHSz<<16) 				// Destination width for preview pre-scaler
		|(uPreDstVSz<<0); 								// Destination height for preview pre-scaler

	oCim.m_uMainScalerCtrl = ((oCim.m_bIsScalerBypass?1:0)<<31) 	// Pre-view scaler bypass
		|(bIsHScaleUp<<30)								// Horizontal salce Up/Down flag - 1: Up, 0: Down
		|(bIsVScaleUp<<29)								// Vertical salce Up/Down flag - 1: Up, 0: Down 
		|(1U<<28)										// YCbCr data dynamic range selection for csc(RGB2YCBCR)
		|(1U<<27) 										// YCbCr data dynamic range selection for csc(YCBCR2RGB)
		|(uMainHRatio<<16) 
		|(3<<13)
		|(nOutRGBFMTPr<<11)
		|(uMainVRatio<<0);

	if (oCim.m_eProcessPath==P_PATH)
	{
		if (oCim.m_bIsScalerBypass != true)
		{
			Outp32(&CAMERAR->rCIPRSCPRERATIO, uPreScalerCtrl1); 	// set preview pre-scaler control reg. 1
			Outp32(&CAMERAR->rCIPRSCPREDST, uPreScalerCtrl2); 	// set preview pre-scaler control reg. 2
		}
		Outp32(&CAMERAR->rCIPRTRGFMT, uTargetFmt); 				// Set target format reg
//		Outp32(&CAMERAR->rCIPRCTRL, uDMAControl|(1<<2));			// set DMA control reg, enable LastIRQEn
		
		if ((oCim.m_eOutputPath==DMA)) 						// Pre-view and DMAInput path
		{
			Outp32(&CAMERAR->rCIPRSCCTRL, oCim.m_uMainScalerCtrl); 	// set preview main-scaler control reg.
			Outp32(&CAMERAR->rCIPRTAREA, oCim.m_uDstVsz*oCim.m_uDstHsz); 	// set preview DMA target area reg.
		
			switch(oCim.m_eDstFmt)
			{
				case RGB24:
				case RGB18:
				case RGB16:
				case YCBYCR:
				case YCRYCB:
				case CBYCRY:
				case CRYCBY:
					Outp32(&CAMERAR->rCIPRYSA1, uDstAddr0);
					Outp32(&CAMERAR->rCIPRYSA2, uDstAddr1);
					Outp32(&CAMERAR->rCIPRYSA3, uDstAddr0);
					Outp32(&CAMERAR->rCIPRYSA4, uDstAddr1);
					break;
				case YC422:
				case YC420: 
					Outp32(&CAMERAR->rCIPRYSA1, uDstAddr0);
					Outp32(&CAMERAR->rCIPRCBSA1, uDstAddr0 + uOutYSz);
					Outp32(&CAMERAR->rCIPRCRSA1, uDstAddr0 + uOutYSz + uOutCbCrSz);
						
					Outp32(&CAMERAR->rCIPRYSA2, uDstAddr1);
					Outp32(&CAMERAR->rCIPRCBSA2, uDstAddr1 + uOutYSz);
					Outp32(&CAMERAR->rCIPRCRSA2, uDstAddr1 + uOutYSz + uOutCbCrSz);
						
					Outp32(&CAMERAR->rCIPRYSA3, uDstAddr0);
					Outp32(&CAMERAR->rCIPRCBSA3, uDstAddr0 + uOutYSz);
					Outp32(&CAMERAR->rCIPRCRSA3, uDstAddr0 + uOutYSz + uOutCbCrSz);
					
					Outp32(&CAMERAR->rCIPRYSA4, uDstAddr1);
					Outp32(&CAMERAR->rCIPRCBSA4, uDstAddr1 + uOutYSz);
					Outp32(&CAMERAR->rCIPRCRSA4, uDstAddr1 + uOutYSz + uOutCbCrSz);
					break;
				default:
					Assert(0); //Unsupported output data format
			}
		}
		else // fifo output
		{
			oCim.m_uMainScalerCtrl |= 1<<26;
			Outp32(&CAMERAR->rCIPRSCCTRL, oCim.m_uMainScalerCtrl);
		}
		Outp32(&CAMERAR->rCIPRSCOSY, 0); // set preview scan line offset reg.
		Outp32(&CAMERAR->rCIPRSCOSCB, 0); 
		Outp32(&CAMERAR->rCIPRSCOSCR, 0); 
		Outp32(&CAMERAR->rCIPRCTRL, uDMAControl&0xFFFFFFFB);	// Clear LastIRQEn
	}
	else if (oCim.m_eProcessPath==C_PATH)
	{
		if (oCim.m_bIsScalerBypass != true)
		{
			Outp32(&CAMERAR->rCICOSCPRERATIO, uPreScalerCtrl1);		// set preview pre-scaler control reg. 1
			Outp32(&CAMERAR->rCICOSCPREDST, uPreScalerCtrl2);		// set preview pre-scaler control reg. 2
		}
		Outp32(&CAMERAR->rCICOTRGFMT, uTargetFmt);					// Set target format reg
//		Outp32(&CAMERAR->rCICOCTRL, uDMAControl|(1<<2));				// set DMA control reg, enable LastIRQEn
		
		if (oCim.m_eOutputPath==DMA)
		{
			Outp32(&CAMERAR->rCICOTAREA, oCim.m_uDstVsz*oCim.m_uDstHsz); 		// set preview DMA target area reg.
			Outp32(&CAMERAR->rCICOSCCTRL, oCim.m_uMainScalerCtrl);		// set preview main-scaler control reg.

			switch(oCim.m_eDstFmt)
			{
				case RGB24:
				case RGB18:
				case RGB16:
				case YCBYCR:
				case YCRYCB:
				case CBYCRY:
				case CRYCBY:
					Outp32(&CAMERAR->rCICOYSA1, uDstAddr0);
					Outp32(&CAMERAR->rCICOYSA2, uDstAddr1);
					Outp32(&CAMERAR->rCICOYSA3, uDstAddr0);
					Outp32(&CAMERAR->rCICOYSA4, uDstAddr1);
					break;
				case YC422:
				case YC420: 
					Outp32(&CAMERAR->rCICOYSA1, uDstAddr0);
					Outp32(&CAMERAR->rCICOCBSA1, uDstAddr0 + uOutYSz);
					Outp32(&CAMERAR->rCICOCRSA1, uDstAddr0 + uOutYSz + uOutCbCrSz);
						
					Outp32(&CAMERAR->rCICOYSA2, uDstAddr1);
					Outp32(&CAMERAR->rCICOCBSA2, uDstAddr1 + uOutYSz);
					Outp32(&CAMERAR->rCICOCRSA2, uDstAddr1 + uOutYSz + uOutCbCrSz);
						
					Outp32(&CAMERAR->rCICOYSA3, uDstAddr0);
					Outp32(&CAMERAR->rCICOCBSA3, uDstAddr0 + uOutYSz);
					Outp32(&CAMERAR->rCICOCRSA3, uDstAddr0 + uOutYSz + uOutCbCrSz);
					
					Outp32(&CAMERAR->rCICOYSA4, uDstAddr1);
					Outp32(&CAMERAR->rCICOCBSA4, uDstAddr1 + uOutYSz);
					Outp32(&CAMERAR->rCICOCRSA4, uDstAddr1 + uOutYSz + uOutCbCrSz);
					break;
				default:
					Assert(0); //Unsupported output data format
			}
		}
		else // Codec and fifo path
		{
			oCim.m_uMainScalerCtrl |= 1<<26;
			Outp32(&CAMERAR->rCICOSCCTRL, oCim.m_uMainScalerCtrl);
		} 

		Outp32(&CAMERAR->rCICOSCOSY, 0); // set codec scan line offset reg.
		Outp32(&CAMERAR->rCICOSCOSCB, 0); 
		Outp32(&CAMERAR->rCICOSCOSCR, 0); 
		Outp32(&CAMERAR->rCICOCTRL, uDMAControl&0xFFFFFFFB);	// Clear LastIRQEn
	}
	else
		Assert(0);
	
	// 12.Set image effect reg. 
	Outp32(&CAMERAR->rCIIMGEFF, 0);
	
}


//////////
// Function Name : CAMERA_InitPreviewPath0
// Function Description : Connection function Test -> SetBasicSFR Function
// Fifo input p_path dma output
// Input : 	
//	u32 uSrcCropStartX(Window Offset X1), u32 uSrcCropStartY(Window Offset X2), u32 uSrcCropHsz(Wanted Soruce Crop H Size), u32 uSrcCropVsz(Wanted Source Crop V Size), 
//	u32 uDisplayHsz(wanted display H size), u32 uDisplayVsz(wanted display V size), u32 uDstStX(display start x), u32 uDstStY(display Start Y), u32 uDstHsz(Destination H Size), u32 uDstVsz(Destination V Size), 
//	u32 uDstAddr0(Destination Address 0), u32 uDstAddr1(Destination Address 1), CSPACE eDstFmt(Destination Format), FLIP_DIR eFlip(FLIP mode), ROT_DEG eRotDeg(Rotation Mode)
// Output :    	None
void CAMERA_InitPreviewPath0(
	u32 uSrcCropStartX, u32 uSrcCropStartY, u32 uSrcCropHsz, u32 uSrcCropVsz, 
	u32 uDisplayHsz, u32 uDisplayVsz, u32 uDstStX, u32 uDstStY, u32 uDstHsz, u32 uDstVsz, 
	u32 uDstAddr0, u32 uDstAddr1, CSPACE eDstFmt, FLIP_DIR eFlip, ROT_DEG eRotDeg)
{
	CAMERA_ResetIp();

	CAMERA_SetBasicSfr(uSrcCropStartX, uSrcCropStartY, uSrcCropHsz, uSrcCropVsz, 
		uDstHsz, uDstVsz, uDstAddr0, uDstAddr1, eDstFmt, 
		FIFO, P_PATH, DMA, eFlip, eRotDeg);

	if (uDstHsz < uDisplayHsz && uDstVsz < uDisplayVsz)
		CAMERA_SetDstScanOffset(uDisplayHsz, uDisplayVsz, uDstStX, uDstStY, uDstAddr0, uDstAddr1);
	else if (uDstHsz > uDisplayHsz || uDstVsz > uDisplayVsz)
		{
			Assert(0);
		}
	else
		{}
}


//////////
// Function Name : CAMERA_InitPreviewPathToFifoOut0
// Function Description : 
// 					Fifo input p_path fifo output 
// Input : 	
//	u32 uSrcCropStartX(Window offset X1), u32 uSrcCropStartY(Window offset Y1), u32 uSrcCropHsz(Wanted source crop H size), u32 uSrcCropVsz(Wanted source srop V size), 
//	u32 uDstHsz(Destination H size), u32 uDstVsz(Destination V size), CSPACE eDstFmt(Destination format)
// Output :    	None
void CAMERA_InitPreviewPathToFifoOut0(
	u32 uSrcCropStartX, u32 uSrcCropStartY, u32 uSrcCropHsz, u32 uSrcCropVsz, 
	u32 uDstHsz, u32 uDstVsz, CSPACE eDstFmt)
{
	CAMERA_ResetIp();

	CAMERA_SetBasicSfr(uSrcCropStartX, uSrcCropStartY, uSrcCropHsz, uSrcCropVsz, 
		uDstHsz, uDstVsz, 0, 0, eDstFmt, FIFO, P_PATH, FIFO, FLIP_NO, ROT_0);
}



//////////
// Function Name : CAMERA_InitCodecPath0
// Function Description : camera input DMA codec path out function 
// Fifo input c_path dma output
// Input : 	
//		u32 uSrcCropStartX(Window offset X1), u32 uSrcCropStartY(Window offset Y1), u32 uSrcCropHsz(Wanted source H size), u32 uSrcCropVsz(Wanted source V size), 
//		u32 uDisplayHsz(Wanted display H size), u32 uDisplayVsz(Wanted display V size), u32 uDstStX(Display start X), u32 uDstStY(Display start Y), u32 uDstHsz(Destination H size), u32 uDstVsz(Destination V size), 
//		u32 uDstAddr0(Destination address 0), u32 uDstAddr1(Destination Address 1), CSPACE eDstFmt(Destination format), FLIP_DIR eFlip(FLIP mode), ROT_DEG eRotDeg(Rotation mode)
// Output :    	None
void CAMERA_InitCodecPath0(u32 uSrcCropStartX, u32 uSrcCropStartY, u32 uSrcCropHsz, u32 uSrcCropVsz, 
		u32 uDisplayHsz, u32 uDisplayVsz, u32 uDstStX, u32 uDstStY, u32 uDstHsz, u32 uDstVsz, 
		u32 uDstAddr0, u32 uDstAddr1, CSPACE eDstFmt, FLIP_DIR eFlip, ROT_DEG eRotDeg)
{
	CAMERA_ResetIp();

	CAMERA_SetBasicSfr(uSrcCropStartX, uSrcCropStartY, uSrcCropHsz, uSrcCropVsz,
		uDstHsz, uDstVsz, uDstAddr0, uDstAddr1, eDstFmt, 
		FIFO, C_PATH, DMA, eFlip, eRotDeg);
	
	if (uDstHsz < uDisplayHsz && uDstVsz < uDisplayVsz)
		CAMERA_SetDstScanOffset(uDisplayHsz, uDisplayVsz, uDstStX, uDstStY, uDstAddr0, uDstAddr1);
	else if (uDstHsz > uDisplayHsz || uDstVsz > uDisplayVsz)
	{
		Assert(0);
	}
	else
		{};
}


//////////
// Function Name : CAMERA_InitDmaInpath0
// Function Description : this function is Dma input image out through output DMA
// 					Dma input dma output
// Input : 	
//		u32 uSrcOrgHsz(Sourcr image H size), u32 uSrcOrgVsz(Source image V size), u32 uSrcCropStartX(window offset X1), u32 uSrcCropStartY(Window offset Y1), 
//		u32 uSrcCropHsz(Wanted source H size), u32 uSrcCropVsz(Wanted source V size), u32 uSrcAddr0(Source address 0), u32 uSrcAddr1(Source address 1), CSPACE eSrcFmt(Source format),
//		u32 uDisplayHsz, u32 uDisplayVsz, u32 uDstStX, u32 uDstStY, u32 uDstHsz, u32 uDstVsz, 
//		u32 uDstAddr0(Destination address 0), u32 uDstAddr1(Destination address 1), CSPACE eDstFmt(Destination format), FLIP_DIR eFlip(FLIP mode), ROT_DEG eRotDeg(Rotation mode), PROCESS_PATH ePathMode(Process mode)
// Output :    	None
void CAMERA_InitDmaInPath0(u32 uSrcOrgHsz, u32 uSrcOrgVsz, u32 uSrcCropStartX, u32 uSrcCropStartY, 
		u32 uSrcCropHsz, u32 uSrcCropVsz, u32 uSrcAddr0, u32 uSrcAddr1, CSPACE eSrcFmt,
		u32 uDisplayHsz, u32 uDisplayVsz, u32 uDstStX, u32 uDstStY, u32 uDstHsz, u32 uDstVsz, 
		u32 uDstAddr0, u32 uDstAddr1, CSPACE eDstFmt, FLIP_DIR eFlip, ROT_DEG eRotDeg, PROCESS_PATH ePathMode)
{
	u32 uSrcStartY0, uSrcStartCb0,uSrcStartCr0;
	u32 uSrcEndY0, uSrcEndCb0, uSrcEndCr0;
	u32 uSrcStartY1, uSrcStartCb1,uSrcStartCr1;
	u32 uSrcOffsetY, uSrcOffsetCbCr;
	u32 uBytesPerPixel;
	int nVRatio;
	u32 uMSSz;

	// 1. Set the source size, source data format and source address
	//==============================================
	oCim.m_uSrcHsz = uSrcOrgHsz;
	oCim.m_uSrcVsz = uSrcOrgVsz;
	oCim.m_eMemSrcFmt = eSrcFmt;
	oCim.m_uSrcAddr0 = uSrcAddr0;
	oCim.m_uSrcAddr1 = uSrcAddr1;
	oCim.m_eProcessPath = ePathMode; // jungil 0223
	
	// 2. Set the basic sfr
	//===============================================
	CAMERA_SetBasicSfr(uSrcCropStartX, uSrcCropStartY, uSrcCropHsz, uSrcCropVsz, uDstHsz, uDstVsz,
		uDstAddr0, uDstAddr1, eDstFmt, DMA, ePathMode, DMA, eFlip, eRotDeg);

	// 3. Compute the start address of source
	//===============================================
	uBytesPerPixel = ((oCim.m_eMemSrcFmt == YC420)||(oCim.m_eMemSrcFmt == YC422)) ? 1 :
		((oCim.m_eMemSrcFmt == YCBYCR)||(oCim.m_eMemSrcFmt == YCRYCB)||(oCim.m_eMemSrcFmt == CBYCRY)
		 ||(oCim.m_eMemSrcFmt == CRYCBY)||(oCim.m_eMemSrcFmt == RGB16)) ? 2 : 4;

	uSrcOffsetY = (uSrcOrgHsz - uSrcCropHsz)*uBytesPerPixel;
	
	uSrcStartY0 = oCim.m_uSrcAddr0 + (uSrcOrgHsz*uSrcCropStartY + uSrcCropStartX)*uBytesPerPixel;
	uSrcEndY0 = uSrcStartY0 + uSrcCropHsz*uSrcCropVsz*uBytesPerPixel + uSrcOffsetY*(uSrcCropVsz-1);
	uSrcStartY1 = oCim.m_uSrcAddr1 + (uSrcOrgHsz*uSrcCropStartY + uSrcCropStartX)*uBytesPerPixel;

	Assert(uSrcStartY0%4 == 0); //Start addr must be word aligned
	Assert(uSrcStartY1%4 == 0); //Start addr must be word aligned
	
	if ((oCim.m_eMemSrcFmt == YC420)||(oCim.m_eMemSrcFmt == YC422))
	{
		uSrcOffsetCbCr = (uSrcOrgHsz - uSrcCropHsz)*1/2;
		nVRatio=(oCim.m_eMemSrcFmt == YC420)? 2 : 1;

		uSrcStartCb0 = oCim.m_uSrcAddr0+ uSrcOrgHsz*uSrcOrgVsz
			+ uSrcOrgHsz/2*uSrcCropStartY/nVRatio + uSrcCropStartX/2;
		uSrcStartCr0 = oCim.m_uSrcAddr0 + uSrcOrgHsz*uSrcOrgVsz + uSrcOrgHsz/2*uSrcOrgVsz/nVRatio
			+ uSrcOrgHsz/2*uSrcCropStartY/nVRatio + uSrcCropStartX/2;
		uSrcEndCb0 = uSrcStartCb0 + uSrcCropHsz/2*uSrcCropVsz/nVRatio + uSrcOffsetCbCr*(uSrcCropVsz/nVRatio-1);
		uSrcEndCr0 = uSrcStartCr0 + uSrcCropHsz/2*uSrcCropVsz/nVRatio + uSrcOffsetCbCr*(uSrcCropVsz/nVRatio-1);

		uSrcStartCb1 = oCim.m_uSrcAddr1+ uSrcOrgHsz*uSrcOrgVsz
			+ uSrcOrgHsz/2*uSrcCropStartY/nVRatio + uSrcCropStartX/2;
		uSrcStartCr1 = oCim.m_uSrcAddr1 + uSrcOrgHsz*uSrcOrgVsz + uSrcOrgHsz/2*uSrcOrgVsz/nVRatio
			+ uSrcOrgHsz/2*uSrcCropStartY/nVRatio + uSrcCropStartX/2;
		Assert(uSrcStartCb0%4 == 0); //Start addr must be word aligned
		Assert(uSrcStartCb1%4 == 0); //Start addr must be word aligned
		Assert(uSrcStartCr0%4 == 0); //Start addr must be word aligned
		Assert(uSrcStartCr1%4 == 0); //Start addr must be word aligned
	} 
	else
	{
		uSrcOffsetCbCr = 0;
		uSrcStartCb0 = 0;
		uSrcEndCb0 = 0;
		uSrcStartCr0 = 0;
		uSrcEndCr0 = 0;
		uSrcStartCb1 = 0;
		uSrcStartCr1 = 0;
	}

	// 4. Set the register of source start address and image size
	//==========================================
	uMSSz = (uSrcCropVsz<<16)|(uSrcCropHsz);
	if (oCim.m_eProcessPath==P_PATH)
	{
		Outp32(&CAMERAR->rMSPRWIDTH, uMSSz); // Set MSDMA source image size

		Outp32(&CAMERAR->rCIPRYSA1, uDstAddr0);
		Outp32(&CAMERAR->rCIPRYSA2, uDstAddr1); 
		Outp32(&CAMERAR->rCIPRYSA3, uDstAddr0);
		Outp32(&CAMERAR->rCIPRYSA4, uDstAddr1); 
		Outp32(&CAMERAR->rMSPRY0SA, uSrcStartY0);
		Outp32(&CAMERAR->rMSPRY0END, uSrcEndY0);
		Outp32(&CAMERAR->rMSPRCB0SA, uSrcStartCb0);
		Outp32(&CAMERAR->rMSPRCB0END, uSrcEndCb0); 
		Outp32(&CAMERAR->rMSPRCR0SA, uSrcStartCr0);
		Outp32(&CAMERAR->rMSPRCR0END, uSrcEndCr0);
		Outp32(&CAMERAR->rMSPRYOFF, uSrcOffsetY); 
		Outp32(&CAMERAR->rMSPRCBOFF, uSrcOffsetCbCr);
		Outp32(&CAMERAR->rMSPRCROFF, uSrcOffsetCbCr);

	}
	else 
	{
		Outp32(&CAMERAR->rMSCOWIDTH, uMSSz);// Set MSDMA source image size

		Outp32(&CAMERAR->rCICOYSA1, uDstAddr0);
		Outp32(&CAMERAR->rCICOYSA2, uDstAddr1); 
		Outp32(&CAMERAR->rCICOYSA3, uDstAddr0);
		Outp32(&CAMERAR->rCICOYSA4, uDstAddr1); 
		Outp32(&CAMERAR->rMSCOY0SA, uSrcStartY0); // Set SrcAddr0 Y, Cb, Cr
		Outp32(&CAMERAR->rMSCOY0END, uSrcEndY0);
		Outp32(&CAMERAR->rMSCOCB0SA, uSrcStartCb0);
		Outp32(&CAMERAR->rMSCOCB0END, uSrcEndCb0); 
		Outp32(&CAMERAR->rMSCOCR0SA, uSrcStartCr0);
		Outp32(&CAMERAR->rMSCOCR0END, uSrcEndCr0); 
		Outp32(&CAMERAR->rMSCOYOFF, uSrcOffsetY); // Set Offset of Y, Cb, Cr
		Outp32(&CAMERAR->rMSCOCBOFF, uSrcOffsetCbCr);
		Outp32(&CAMERAR->rMSCOCROFF, uSrcOffsetCbCr);
	}

	// 5. Set scan line offset 
	//===========================================
	if (uDstHsz < uDisplayHsz && uDstVsz < uDisplayVsz)
		CAMERA_SetDstScanOffset(uDisplayHsz, uDisplayVsz, uDstStX, uDstStY, uDstAddr0, uDstAddr1);
	else if (uDstHsz > uDisplayHsz || uDstVsz > uDisplayVsz)
		{
			Assert(0);
		}
	else
		{};
}




//////////
// Function Name : CAMERA_InitDmaInPathToFifoOut0
// Function Description : 
// 					Dma input fifo output
// Input : 	
//		u32 uSrcOrgHsz(Source image H size), u32 uSrcOrgVsz(Source image V size), u32 uSrcCropStartX(Window offset X1), u32 uSrcCropStartY(Window offset Y1), 
//		u32 uSrcCropHsz(Wanted crop H size), u32 uSrcCropVsz(Wanted crop V size), u32 uSrcAddr0(Source image address 0), u32 uSrcAddr1(Source image address 1), CSPACE eSrcFmt(Source image format),
//		u32 uDstHsz(Destination image H size), u32 uDstVsz(Destination image V size), CSPACE eDstFmt(Destination image format), 
//		ROT_DEG eRotDeg(Rotation mode), PROCESS_PATH ePathMode(Process path)
// Output :    	None
void CAMERA_InitDmaInPathToFifoOut0(u32 uSrcOrgHsz, u32 uSrcOrgVsz, u32 uSrcCropStartX, u32 uSrcCropStartY, 
		u32 uSrcCropHsz, u32 uSrcCropVsz, u32 uSrcAddr0, u32 uSrcAddr1, CSPACE eSrcFmt,
		u32 uDstHsz, u32 uDstVsz, CSPACE eDstFmt, 
		ROT_DEG eRotDeg, PROCESS_PATH ePathMode)
{
	u32 uSrcStartY0, uSrcStartCb0,uSrcStartCr0;
	u32 uSrcEndY0, uSrcEndCb0, uSrcEndCr0;
	u32 uSrcStartY1, uSrcStartCb1,uSrcStartCr1;
	u32 uSrcOffsetY, uSrcOffsetCbCr;
	u32 uBytesPerPixel;
	int nVRatio;
	u32 uMSSz;

		
	// 1. Set the source size, source data format and source address
	//==========================================
	oCim.m_uSrcHsz = uSrcOrgHsz;
	oCim.m_uSrcVsz = uSrcOrgVsz;
	oCim.m_eMemSrcFmt = eSrcFmt;
	oCim.m_uSrcAddr0 = uSrcAddr0;
	oCim.m_uSrcAddr1 = uSrcAddr1;
	oCim.m_eProcessPath = ePathMode; // jungil
	
	// 2. Set the basic sfr
	//===========================================
	CAMERA_SetBasicSfr(uSrcCropStartX, uSrcCropStartY, uSrcCropHsz, uSrcCropVsz, uDstHsz, uDstVsz,
		0, 0, eDstFmt, DMA, ePathMode, FIFO, FLIP_NO, eRotDeg);

	// 3. Compute the start address of source
	//===========================================
	uBytesPerPixel = (oCim.m_eMemSrcFmt == YC420||oCim.m_eMemSrcFmt == YC422) ? 1 :
		(oCim.m_eMemSrcFmt == YCBYCR||oCim.m_eMemSrcFmt == YCRYCB||oCim.m_eMemSrcFmt == CBYCRY
		 ||oCim.m_eMemSrcFmt == CRYCBY||oCim.m_eMemSrcFmt == RGB16) ? 2 : 4;
	
	uSrcOffsetY = (uSrcOrgHsz - uSrcCropHsz)*uBytesPerPixel;
	
	uSrcStartY0 = oCim.m_uSrcAddr0 + (uSrcOrgHsz*uSrcCropStartY + uSrcCropStartX)*uBytesPerPixel;
	uSrcEndY0 = uSrcStartY0 + uSrcCropHsz*uSrcCropVsz*uBytesPerPixel + uSrcOffsetY*(uSrcCropVsz-1);
	uSrcStartY1 = oCim.m_uSrcAddr1 + (uSrcOrgHsz*uSrcCropStartY + uSrcCropStartX)*uBytesPerPixel;

	Assert(uSrcStartY0%4 == 0); //Start addr must be word aligned
	Assert(uSrcStartY1%4 == 0); //Start addr must be word aligned

	if (oCim.m_eMemSrcFmt == YC420||oCim.m_eMemSrcFmt == YC422)
	{
		uSrcOffsetCbCr = (uSrcOrgHsz - uSrcCropHsz)*1/2;
		nVRatio=(oCim.m_eMemSrcFmt == YC420)? 2 : 1;

		uSrcStartCb0 = oCim.m_uSrcAddr0+ uSrcOrgHsz*uSrcOrgVsz
			+ uSrcOrgHsz/2*uSrcCropStartY/nVRatio + uSrcCropStartX/2;
		uSrcStartCr0 = oCim.m_uSrcAddr0 + uSrcOrgHsz*uSrcOrgVsz + uSrcOrgHsz/2*uSrcOrgVsz/nVRatio
			+ uSrcOrgHsz/2*uSrcCropStartY/nVRatio + uSrcCropStartX/2;
		uSrcEndCb0 = uSrcStartCb0 + uSrcCropHsz/2*uSrcCropVsz/nVRatio + uSrcOffsetCbCr*(uSrcCropVsz/nVRatio-1);
		uSrcEndCr0 = uSrcStartCr0 + uSrcCropHsz/2*uSrcCropVsz/nVRatio + uSrcOffsetCbCr*(uSrcCropVsz/nVRatio-1);

		uSrcStartCb1 = oCim.m_uSrcAddr1+ uSrcOrgHsz*uSrcOrgVsz
			+ uSrcOrgHsz/2*uSrcCropStartY/nVRatio + uSrcCropStartX/2;
		uSrcStartCr1 = oCim.m_uSrcAddr1 + uSrcOrgHsz*uSrcOrgVsz + uSrcOrgHsz/2*uSrcOrgVsz/nVRatio
			+ uSrcOrgHsz/2*uSrcCropStartY/nVRatio + uSrcCropStartX/2;
		Assert(uSrcStartCb0%4 == 0); //Start addr must be word aligned
		Assert(uSrcStartCb1%4 == 0); //Start addr must be word aligned
		Assert(uSrcStartCr0%4 == 0); //Start addr must be word aligned
		Assert(uSrcStartCr1%4 == 0); //Start addr must be word aligned
	} 
	else
	{
		uSrcOffsetCbCr = 0;
		uSrcStartCb0 = 0;
		uSrcEndCb0 = 0;
		uSrcStartCr0 = 0;
		uSrcEndCr0 = 0;
		uSrcStartCb1 = 0;
		uSrcStartCr1 = 0;
	}

	// 4. Set the register of source start address and image size
	//===========================================
	uMSSz = (uSrcCropVsz<<16)|(uSrcCropHsz);
	if (oCim.m_eProcessPath==P_PATH)
	{
		Outp32(&CAMERAR->rMSPRWIDTH, uMSSz); // Set MSDMA source image size

		Outp32(&CAMERAR->rCIPRYSA1, 0);
		Outp32(&CAMERAR->rCIPRYSA2, 0); 
		Outp32(&CAMERAR->rCIPRYSA3, 0);
		Outp32(&CAMERAR->rCIPRYSA4, 0); 
		Outp32(&CAMERAR->rMSPRY0SA, uSrcStartY0); // Set SrcAddr0 Y, Cb, Cr
		Outp32(&CAMERAR->rMSPRY0END, uSrcEndY0);
		Outp32(&CAMERAR->rMSPRCB0SA, uSrcStartCb0);
		Outp32(&CAMERAR->rMSPRCB0END, uSrcEndCb0); 
		Outp32(&CAMERAR->rMSPRCR0SA, uSrcStartCr0);
		Outp32(&CAMERAR->rMSPRCR0END, uSrcEndCr0);
		Outp32(&CAMERAR->rMSPRYOFF, uSrcOffsetY); // Set Offset of Y, Cb, Cr 
		Outp32(&CAMERAR->rMSPRCBOFF, uSrcOffsetCbCr);
		Outp32(&CAMERAR->rMSPRCROFF, uSrcOffsetCbCr);
	}
	else 
	{
		Outp32(&CAMERAR->rMSCOWIDTH, uMSSz);// Set MSDMA source image size

		Outp32(&CAMERAR->rCICOYSA1, 0);
		Outp32(&CAMERAR->rCICOYSA2, 0); 
		Outp32(&CAMERAR->rCICOYSA3, 0);
		Outp32(&CAMERAR->rCICOYSA4, 0); 
		Outp32(&CAMERAR->rMSCOY0SA, uSrcStartY0); // Set SrcAddr0 Y, Cb, Cr
		Outp32(&CAMERAR->rMSCOY0END, uSrcEndY0);
		Outp32(&CAMERAR->rMSCOCB0SA, uSrcStartCb0);
		Outp32(&CAMERAR->rMSCOCB0END, uSrcEndCb0); 
		Outp32(&CAMERAR->rMSCOCR0SA, uSrcStartCr0);
		Outp32(&CAMERAR->rMSCOCR0END, uSrcEndCr0); 
		Outp32(&CAMERAR->rMSCOYOFF, uSrcOffsetY); // Set Offset of Y, Cb, Cr
		Outp32(&CAMERAR->rMSCOCBOFF, uSrcOffsetCbCr);
		Outp32(&CAMERAR->rMSCOCROFF, uSrcOffsetCbCr);
	}
}



//////////
// Function Name : CAMERA_StartPreviewPath
// Function Description : this function : start preview path . 
// Input : 	None
// Output :    	None
void CAMERA_StartPreviewPath(void)
{ 
	u32 uCptEnRegVal;

	// 1. Set p_path main-scaler control register
	//===========================================
	if (oCim.m_bIsScalerBypass!= true)
	{
		oCim.m_uMainScalerCtrl |= (1<<15); // Enable main scaler
		Outp32(&CAMERAR->rCIPRSCCTRL, oCim.m_uMainScalerCtrl);
	}

	// 2. Set image capture register
	//============================================
	uCptEnRegVal = 0; // Image Capture Enable Reg.
	uCptEnRegVal |=(1U<<31)|(1U<<29); // Enable global capture and preview scaler
	uCptEnRegVal &=~(1U<<30);
	uCptEnRegVal &= ~(0x3<<25);
	if (oCim.m_bIsScalerBypass==true) // If bypass-scaler is set,
		uCptEnRegVal &= ~(1<<29); // ImgCptEn_PrSc must be clear
	uCptEnRegVal &= ~(0xffff<<10); // Cclear uCptEnRegVal[25:10]
	Outp32(&CAMERAR->rCIIMGCPT, uCptEnRegVal); 

}


//////////
// Function Name : CAMERA_StartCodecPath
// Function Description : Starts Codec path . 
// Input : 	u32 uCptCnt(Capture count)
// Output :    	None
void CAMERA_StartCodecPath(u32 uCptCnt)
{

	u32 uCptPtr;
	u32 uCptEnRegVal = 0; // Image Capture Enable Reg.

	
	Assert(uCptCnt < 256); //Number of frame counts must be less than 256
	
	// 1. Set c_path main-scaler control register
	//==================================
	if (oCim.m_bIsScalerBypass!= true)
	{
		oCim.m_uMainScalerCtrl |= (1<<15); // Enable main scaler
		Outp32(&CAMERAR->rCICOSCCTRL, oCim.m_uMainScalerCtrl); 
	}

	// 2. Set image capture register
	//==================================
	if (uCptCnt>0)
	{
		Outp32(&CAMERAR->rCICPTSEQ, 0xffffffff); // Apply capture sequence 
		uCptPtr = 0;
		uCptEnRegVal |= (1<<25)|(uCptPtr<<19)|(1<<18)|(uCptCnt<<10) ; // Enable global capture and preview scaler 
	}
	uCptEnRegVal |=(1U<<31); // Enable global capture 
	uCptEnRegVal &= ~(1U<<29); // Disable preview scaler
	if (oCim.m_bIsScalerBypass==true) // if bypass-scaler is set,
		uCptEnRegVal &= ~(1U<<30); // ImgCptEn_CoSc must be clear
	else
		uCptEnRegVal |=(1U<<30); // Enable codec scaler
	Outp32(&CAMERAR->rCIIMGCPT, uCptEnRegVal);

}

//////////
// Function Name : CAMERA_StartDmaInPath
// Function Description : DMA in path start
// Input : 	None
// Output :    	None
void CAMERA_StartDmaInPath(void)
{
	u32 uCptEnRegVal = 0; // Image Capture Enable Reg.
	int nSrcRGBOrder;

	// 1. compute the rgb/YCbYCr order of the source data
	//====================================
	nSrcRGBOrder = (oCim.m_eMemSrcFmt ==RGB16) ? 0 :
		(oCim.m_eMemSrcFmt ==RGB18) ? 1 :
		(oCim.m_eMemSrcFmt ==RGB24) ? 2 : 3;

	// 2. Set the corresponding register(main-scaler control register, MSDMA control register and, image caputre register )
	//=============================================================================
	oCim.m_uMainScalerCtrl &=~(0x3<<13); // Clear the bits
	oCim.m_uMainScalerCtrl |= (nSrcRGBOrder<<13);
		
	if (oCim.m_eOutputPath==DMA)
		CAMERA_ClearFrameEndStatus();
	
	if (oCim.m_eProcessPath==P_PATH)
	{
		Outp32(&CAMERAR->rCIPRSCCTRL, oCim.m_uMainScalerCtrl);

		uCptEnRegVal &= ~(1U<<31|1U<<30); // Disable global capture and codec scaler
		uCptEnRegVal |=(1U<<29); // Enable preview scaler

		if (oCim.m_bIsScalerBypass!= true)
		{
			oCim.m_uMainScalerCtrl |= (1<<15); // Enable main scaler
			Outp32(&CAMERAR->rCIPRSCCTRL, oCim.m_uMainScalerCtrl);
		}
		else 
			uCptEnRegVal &= ~(1<<29); // ImgCptEn_PrSc must be clear

		Outp32(&CAMERAR->rCIIMGCPT, uCptEnRegVal); 
		Outp32(&CAMERAR->rCIMSCTRL, oCim.m_uMSDMACtrl|(1<<0));
	}
	else //if (oCim.m_eProcessPath==C_PATH)
	{
		Outp32(&CAMERAR->rCICOSCCTRL, oCim.m_uMainScalerCtrl);

		uCptEnRegVal &= ~(1U<<31|1U<<29); // Disable global capture and preview scaler
		uCptEnRegVal |=(1U<<30); // Enable codec scaler

		if (oCim.m_bIsScalerBypass!= true)
		{
			oCim.m_uMainScalerCtrl |= (1<<15); // Enable main scaler
			Outp32(&CAMERAR->rCICOSCCTRL, oCim.m_uMainScalerCtrl); 
		}
		else
			uCptEnRegVal &= ~(1<<30); // ImgCptEn_PrSc must be clear

		Outp32(&CAMERAR->rCIIMGCPT, uCptEnRegVal); 
		Outp32(&CAMERAR->rMSCOCTRL, oCim.m_uMSDMACtrl|(1<<0));
	}
}



//////////
// Function Name : CAMERA_StartMSDmaPreviewPath
// Function Description : Camera Input Sorce MSDMA Start(Preview)
// Input : 	None
// Output :    	None
void CAMERA_StartMSDmaPreviewPath(void)
{
	Outp32(&CAMERAR->rCIMSCTRL, oCim.m_uMSDMACtrl|(1<<0));
}


//////////
// Function Name : CAMERA_StartMSDmaCodecPath
// Function Description : Camera Input Sorce MSDMA Start(Preview)
// Input : 	None
// Output :    	None
void CAMERA_StartMSDmaCodecPath(void)
{
	Outp32(&CAMERAR->rMSCOCTRL, oCim.m_uMSDMACtrl|(1<<0));
}

//////////
// Function Name : CAMERA_IsProcessingDone
// Function Description : 
// Input : 	None
// Output :    	None
u8 CAMERA_IsProcessingDone(void)
{
	u32 uSfr;
	u32 uResult;
	
	if (oCim.m_eProcessPath==P_PATH)
	{
		uSfr = Inp32(&CAMERAR->rCIPRSTATUS);
		uResult = (uSfr>>19)&0x1;
		return (u8)uResult;
	}
	else
	{
		uSfr = Inp32(&CAMERAR->rCICOSTATUS);
		return( ((uSfr>>17)&0x01) ? true : false ); 
	}
}


//////////
// Function Name : CAMERA_ClearFrameEndStatus
// Function Description : 
// Input : 	None
// Output :    	None
void CAMERA_ClearFrameEndStatus(void)
{
	if (oCim.m_eProcessPath==P_PATH)
		Outp32(&CAMERAR->rCIPRSTATUS, 0);
	else
		Outp32(&CAMERAR->rCICOSTATUS, 0);
}

//////////
// Function Name : CAMERA_StopPreviewPath
// Function Description : Stop Preview path operation
// Input : 	None
// Output :    	None
void CAMERA_StopPreviewPath(void)
{
	oCim.m_uMainScalerCtrl &= ~(1<<15); // Disable main scaler
	// jungil0214
	oCim.m_uMainScalerCtrl &= ~(1<<31); // Disable bypass 

	Outp32(&CAMERAR->rCIPRSCCTRL, oCim.m_uMainScalerCtrl);
	Outp32(&CAMERAR->rCIIMGCPT, 0);
}

//////////
// Function Name : CAMERA_StopCodecPath
// Function Description : Stop Codec path operation
// Input : 	None
// Output :    	None
void CAMERA_StopCodecPath(void)
{
	oCim.m_uMainScalerCtrl &= ~(1<<15); // Disable main scaler
	// jungil0214
	oCim.m_uMainScalerCtrl &= ~(1<<31); // Disable bypass 
	Outp32(&CAMERAR->rCICOSCCTRL, oCim.m_uMainScalerCtrl);
	Outp32(&CAMERAR->rCIIMGCPT, 0);
	
}

//////////
// Function Name : CAMERA_StopDmainPath
// Function Description : stop dma in path operation
// Input : 	None
// Output :    	None
void CAMERA_StopDmaInPath(void)
{
//	u32 uImgCptRegVal;
//	u32 uMSDMACtrl;
//	u32 uSfr;

	oCim.m_uMainScalerCtrl &= ~(1<<15); // Disable main scaler

	if (oCim.m_eProcessPath==P_PATH)
	{
		Outp32(&CAMERAR->rCIPRSCCTRL, oCim.m_uMainScalerCtrl);
		Outp32(&CAMERAR->rCIIMGCPT, 0);
	}
	else //oCim.m_eProcessPath==C_PATH
	{
		Outp32(&CAMERAR->rCICOSCCTRL, oCim.m_uMainScalerCtrl);
		Outp32(&CAMERAR->rCIIMGCPT, 0);
	}
}



//////////
// Function Name : CAMERA_SetDstScanOffset
// Function Description : Scan Line Offset setting function 
// Input : 	
//		u32 uDisplayHSz(Wanted display H size), u32 uDisplayVSz(Wanted display V size), 
//		u32 uDisplayStartX(Display start X), u32 uDisplayStartY(Display start Y), u32 uDstAddr0(Destination address 0 ), u32 uDstAddr1(Destination address 1)
// Output :    	None
void CAMERA_SetDstScanOffset(u32 uDisplayHSz, u32 uDisplayVSz, 
	u32 uDisplayStartX, u32 uDisplayStartY, u32 uDstAddr0, u32 uDstAddr1)
{ 
	u32 uOutYSz, uBytePerPixrl;
	u32 uInitYOffset;
	u32 uLineYOffset;
	u32 uScanYOff;
	u32 uInitCrOffset, uLineCrOffset, uInitCbOffset, uLineCbOffset, uScanCrOff ,uScanCbOff ;


	
	// 1. Compute oCim.m_uDstHsz, uOutYSz and uBytePerPixrl
	//===================================================
	oCim.m_uDstHsz = (oCim.m_eOutRotDeg==ROT_90) ? oCim.m_uDstVsz: oCim.m_uDstHsz;

	uOutYSz = uDisplayHSz*uDisplayVSz;
	uBytePerPixrl = (oCim.m_eDstFmt == RGB18||oCim.m_eDstFmt == RGB24)? 4:
		 (oCim.m_eDstFmt == YC422||oCim.m_eDstFmt == YC420) ? 1 : 2;

	// 2. Move the start point form (0,0) to (0, uDisplayStartY), then compute the start address again.
	//=============================================================
	if (oCim.m_eProcessPath==P_PATH&&oCim.m_eOutputPath==DMA)
	{
		Outp32(&CAMERAR->rCIPRYSA1, uDstAddr0 + uDisplayHSz*uDisplayStartY*uBytePerPixrl);
		Outp32(&CAMERAR->rCIPRYSA2, uDstAddr1 + uDisplayHSz*uDisplayStartY*uBytePerPixrl);
		Outp32(&CAMERAR->rCIPRYSA3, uDstAddr0 + uDisplayHSz*uDisplayStartY*uBytePerPixrl);
		Outp32(&CAMERAR->rCIPRYSA4, uDstAddr1 + uDisplayHSz*uDisplayStartY*uBytePerPixrl);
		
		if (oCim.m_eDstFmt==YC422)
		{
			Outp32(&CAMERAR->rCIPRCBSA1, uDstAddr0 + uOutYSz + uDisplayHSz*uDisplayStartY/2);
			Outp32(&CAMERAR->rCIPRCRSA1, uDstAddr0 + uOutYSz + uOutYSz/2 + uDisplayHSz*uDisplayStartY/2);
			Outp32(&CAMERAR->rCIPRCBSA2, uDstAddr1 + uOutYSz + uDisplayHSz*uDisplayStartY/2);
			Outp32(&CAMERAR->rCIPRCRSA2, uDstAddr1 + uOutYSz + uOutYSz/2 + uDisplayHSz*uDisplayStartY/2);
			Outp32(&CAMERAR->rCIPRCBSA3, uDstAddr0 + uOutYSz + uDisplayHSz*uDisplayStartY/2);
			Outp32(&CAMERAR->rCIPRCRSA3, uDstAddr0 + uOutYSz + uOutYSz/2 + uDisplayHSz*uDisplayStartY/2);
			Outp32(&CAMERAR->rCIPRCBSA4, uDstAddr1 + uOutYSz + uDisplayHSz*uDisplayStartY/2);
			Outp32(&CAMERAR->rCIPRCRSA4, uDstAddr1 + uOutYSz + uOutYSz/2 + uDisplayHSz*uDisplayStartY/2);
		}
		else if (oCim.m_eDstFmt==YC420)
		{
			Outp32(&CAMERAR->rCIPRCBSA1, uDstAddr0 + uOutYSz + uDisplayHSz*uDisplayStartY/4);
			Outp32(&CAMERAR->rCIPRCRSA1, uDstAddr0 + uOutYSz + uOutYSz/4 + uDisplayHSz*uDisplayStartY/4);
			Outp32(&CAMERAR->rCIPRCBSA2, uDstAddr1 + uOutYSz + uDisplayHSz*uDisplayStartY/4);
			Outp32(&CAMERAR->rCIPRCRSA2, uDstAddr1 + uOutYSz + uOutYSz/4 + uDisplayHSz*uDisplayStartY/4);
			Outp32(&CAMERAR->rCIPRCBSA3, uDstAddr0 + uOutYSz + uDisplayHSz*uDisplayStartY/4);;
			Outp32(&CAMERAR->rCIPRCRSA3, uDstAddr0 + uOutYSz + uOutYSz/4 + uDisplayHSz*uDisplayStartY/4);
			Outp32(&CAMERAR->rCIPRCBSA4, uDstAddr1 + uOutYSz + uDisplayHSz*uDisplayStartY/4);
			Outp32(&CAMERAR->rCIPRCRSA4, uDstAddr1 + uOutYSz + uOutYSz/4 + uDisplayHSz*uDisplayStartY/4);
		}
		else
		{
			Outp32(&CAMERAR->rCIPRCBSA1, 0);
			Outp32(&CAMERAR->rCIPRCRSA1, 0);
			Outp32(&CAMERAR->rCIPRCBSA2, 0);
			Outp32(&CAMERAR->rCIPRCRSA2, 0);
			Outp32(&CAMERAR->rCIPRCBSA3, 0);
			Outp32(&CAMERAR->rCIPRCRSA3, 0);
			Outp32(&CAMERAR->rCIPRCBSA4, 0);
			Outp32(&CAMERAR->rCIPRCRSA4, 0);
		}
	}
	
	if (oCim.m_eProcessPath==C_PATH&&oCim.m_eOutputPath==DMA)
	{
		Outp32(&CAMERAR->rCICOYSA1, uDstAddr0+uDisplayHSz*uDisplayStartY*uBytePerPixrl);
		Outp32(&CAMERAR->rCICOYSA2, uDstAddr1+uDisplayHSz*uDisplayStartY*uBytePerPixrl);
		Outp32(&CAMERAR->rCICOYSA3, uDstAddr0+uDisplayHSz*uDisplayStartY*uBytePerPixrl);
		Outp32(&CAMERAR->rCICOYSA4, uDstAddr1+uDisplayHSz*uDisplayStartY*uBytePerPixrl);
		if (oCim.m_eDstFmt==YC422)
		{
			Outp32(&CAMERAR->rCICOCBSA1, uDstAddr0 + uOutYSz + uDisplayHSz*uDisplayStartY/2);
			Outp32(&CAMERAR->rCICOCRSA1, uDstAddr0 + uOutYSz + uOutYSz/2 + uDisplayHSz*uDisplayStartY/2);
			Outp32(&CAMERAR->rCICOCBSA2, uDstAddr1 + uOutYSz + uDisplayHSz*uDisplayStartY/2);
			Outp32(&CAMERAR->rCICOCRSA2, uDstAddr1 + uOutYSz + uOutYSz/2 + uDisplayHSz*uDisplayStartY/2);
			Outp32(&CAMERAR->rCICOCBSA3, uDstAddr0 + uOutYSz + uDisplayHSz*uDisplayStartY/2);
			Outp32(&CAMERAR->rCICOCRSA3, uDstAddr0 + uOutYSz + uOutYSz/2 + uDisplayHSz*uDisplayStartY/2);
			Outp32(&CAMERAR->rCICOCBSA4, uDstAddr1 + uOutYSz + uDisplayHSz*uDisplayStartY/2);
			Outp32(&CAMERAR->rCICOCRSA4, uDstAddr1 + uOutYSz + uOutYSz/2 + uDisplayHSz*uDisplayStartY/2);
		}
		else if (oCim.m_eDstFmt==YC420)
		{
			Outp32(&CAMERAR->rCICOCBSA1, uDstAddr0 + uOutYSz + uDisplayHSz*uDisplayStartY/4);
			Outp32(&CAMERAR->rCICOCRSA1, uDstAddr0 + uOutYSz + uOutYSz/4 + uDisplayHSz*uDisplayStartY/4);
			Outp32(&CAMERAR->rCICOCBSA2, uDstAddr1 + uOutYSz + uDisplayHSz*uDisplayStartY/4);
			Outp32(&CAMERAR->rCICOCRSA2, uDstAddr1 + uOutYSz + uOutYSz/4 + uDisplayHSz*uDisplayStartY/4);
			Outp32(&CAMERAR->rCICOCBSA3, uDstAddr0 + uOutYSz + uDisplayHSz*uDisplayStartY/4);;
			Outp32(&CAMERAR->rCICOCRSA3, uDstAddr0 + uOutYSz + uOutYSz/4 + uDisplayHSz*uDisplayStartY/4);
			Outp32(&CAMERAR->rCICOCBSA4, uDstAddr1 + uOutYSz + uDisplayHSz*uDisplayStartY/4);
			Outp32(&CAMERAR->rCICOCRSA4, uDstAddr1 + uOutYSz + uOutYSz/4 + uDisplayHSz*uDisplayStartY/4);
		}
		else
		{
			Outp32(&CAMERAR->rCICOCBSA1, 0);
			Outp32(&CAMERAR->rCICOCRSA1, 0);
			Outp32(&CAMERAR->rCICOCBSA2, 0);
			Outp32(&CAMERAR->rCICOCRSA2, 0);
			Outp32(&CAMERAR->rCICOCBSA3, 0);
			Outp32(&CAMERAR->rCICOCRSA3, 0);
			Outp32(&CAMERAR->rCICOCBSA4, 0);
			Outp32(&CAMERAR->rCICOCRSA4, 0);
		}
	}

	// 3. Compute scan line offset y, cb and cr.
	//===================================================
	uDisplayStartY = 0;
	uInitYOffset = (uDisplayHSz*uDisplayStartY + uDisplayStartX);
	uLineYOffset = (uDisplayHSz - oCim.m_uDstHsz);

	uScanYOff=(uInitYOffset<<16)|(uLineYOffset<<0);

	
	if (oCim.m_eDstFmt==YC422||oCim.m_eDstFmt==YC420)
	{
		Assert((uInitYOffset%4 == 0) && (uLineYOffset%4 == 0)); //Initial Y offset must be word boundary alignment!
		
		if (oCim.m_eDstFmt==YC422) 
			uInitCbOffset = uInitCrOffset = (uDisplayHSz)*uDisplayStartY + uDisplayStartX;
		else//YC420
			uInitCbOffset = uInitCrOffset = (uDisplayHSz)*uDisplayStartY/2 + uDisplayStartX;

		uLineCbOffset = uLineCrOffset =(uDisplayHSz - oCim.m_uDstHsz );
		uScanCbOff = (uInitCbOffset<<16)|(uLineCbOffset<<0);
		uScanCrOff = (uInitCrOffset<<16)|(uLineCrOffset<<0);
		Assert((uInitCrOffset%8 == 0)); // Initial Cr offset must be word boundary alignment!
	}
	else 
	{
		uScanCrOff = uScanCbOff = 0;
		if (oCim.m_eDstFmt!=RGB24&&oCim.m_eDstFmt!=RGB18)
		{
			Assert((uInitYOffset%2==0)); // Initial Y offset must be word boundary alignment!
			Assert((uLineYOffset%2==0)); // Line Y offset must be word boundary alignment!
		}
	}
	
	// 4. Set the register of scan line offset
	//===========================================================
	if (oCim.m_eProcessPath==P_PATH&&oCim.m_eOutputPath==DMA) // FIFO mode doesn't support the scan line offset.
	{
		Outp32(&CAMERAR->rCIPRSCOSY, uScanYOff);
		Outp32(&CAMERAR->rCIPRSCOSCB, uScanCbOff);
		Outp32(&CAMERAR->rCIPRSCOSCR, uScanCrOff);
	}
	else if (oCim.m_eProcessPath==C_PATH&&oCim.m_eOutputPath==DMA)
	{
		Outp32(&CAMERAR->rCICOSCOSY, uScanYOff);
		Outp32(&CAMERAR->rCICOSCOSCB, uScanCbOff);
		Outp32(&CAMERAR->rCICOSCOSCR, uScanCrOff);
	}
	else
	{
		Outp32(&CAMERAR->rCIPRSCOSY, 0);
		Outp32(&CAMERAR->rCIPRSCOSCB, 0);
		Outp32(&CAMERAR->rCIPRSCOSCR, 0);
		Outp32(&CAMERAR->rCICOSCOSY, 0);
		Outp32(&CAMERAR->rCICOSCOSCB, 0);
		Outp32(&CAMERAR->rCICOSCOSCR, 0);
	}
}




//////////
// Function Name : CAMERA_EnabelTestMode
// Function Description :  Test Pattern Test start
// Input : 	TEST_PATTERN ePattern(Test Pattern mode)
// Output :    	None
void CAMERA_EnableTestMode(TEST_PATTERN ePattern)
{
	Assert (oCim.m_eCcir == CCIR601);
	Assert (oCim.m_uSrcHsz<=1280); // Supported max size: 1280*1024
	Assert (oCim.m_uSrcVsz<=1024);

	oCim.m_uCigCtrl |= (ePattern<<27);
	Outp32(&CAMERAR->rCIGCTRL, oCim.m_uCigCtrl);
}

//////////
// Function Name : CAMERA_SetImageEffect
// Function Description : Image Effect function start
// Input :      IMAGE_EFFECT eEffect(Image Effect Mode)
// Output :    	None
void CAMERA_SetImageEffect(IMAGE_EFFECT eEffect )
{
	int nIE_AFTER_SC = (oCim.m_bIsScalerBypass) ? 0: 1;

	Outp32(&CAMERAR->rCIIMGEFF, 		// Set image effect reg.
		(1U<<31) 			// Enable image effect in pre-view path.
		|(1U<<30) 			// Enable image effect in codec path.
		|(nIE_AFTER_SC<<29) // Image effect location: 1:After scalling 0:Before scalling.
		|(eEffect<<26)		// 0: Bypass, 1: Arbitrary Cb/Cr, 2: Negative, 3: Art freeze, 4: Embossing 5: Silhouette
		|(0<<13) 			// PAT_Cb
		|(0<<0) );			// PAT_Cr
}

//////////
// Function Name : CAMERA_SetArbitaryCbCr
// Function Description : Image Effect ARBITARY functin setting
// Input : 	u32 uCbDegree(Cb Degree value), u32 uCrDegree(Cr Degree Value)
// Output :    	None
void CAMERA_SetArbitraryCbCr(u32 uCbDegree, u32 uCrDegree)
{
	int nIE_AFTER_SC = (oCim.m_bIsScalerBypass) ? 0: 1;

	Outp32(&CAMERAR->rCIIMGEFF, 		// Set image effect reg.
		(1U<<31) 			// Enable image effect in pre-view path.
		|(1U<<30)			// Enable image effect in codec path.
		|(nIE_AFTER_SC<<29) // Image effect location: 1:After scalling 0:Before scalling.
		|(1<<26) 			// 0: Bypass, 1: Arbitrary Cb/Cr, 2: Negative, 3: Art freeze, 4: Embossing 5: Silhouette
		|(uCbDegree<<13)	// PAT_Cb
		|(uCrDegree<<0) );	// PAT_Cr
}


//////////
// Function Name : CAMERA_CalcRgbBurstSize
// Function Description : RGB format Burst size Calcurate
// Input : 	u32* uMainBurstSz(main burst length), u32* uRemainBurstSz(remained burst length)
// Output :    	None
void CAMERA_CalcRgbBurstSize(u32* uMainBurstSz, u32* uRemainBurstSz)
{ 
	// 1. Set destination size which computed by word
	//=================================================
	u32 uWordSz = (oCim.m_eDstFmt == RGB16) ? oCim.m_uDstHsz/2 : oCim.m_uDstHsz/1;

	// 2. When the output rotator degree=90, calculate main burst size and remain burst size
	//=========================================================
	if (oCim.m_eOutRotDeg==ROT_90 )
	{ 
		if (oCim.m_eProcessPath==C_PATH)
		{
			Assert(oCim.m_uDstHsz <= OUTPUT_MAX_HSIZE_ROT_RGB_C); // overstep the max.horizontal size can support 
		}
		else
		{
			Assert(oCim.m_uDstHsz <= OUTPUT_MAX_HSIZE_ROT_RGB_P); // overstep the max.horizontal size can support 
		}
				
		if (oCim.m_eDstFmt == RGB16 )
		{
			*uMainBurstSz = 4;
			*uRemainBurstSz = uWordSz % *uMainBurstSz;
			*uRemainBurstSz = (*uRemainBurstSz == 0) ? *uMainBurstSz : *uRemainBurstSz;
			
			Assert(*uRemainBurstSz == 4); // Unsupported reamin burst size
		}
		else
		{
			if (((oCim.m_eProcessPath==C_PATH)&&(oCim.m_uDstHsz>400))||((oCim.m_eProcessPath==P_PATH)&&(oCim.m_uDstHsz>160)))
			{
				*uMainBurstSz = 4;
				*uRemainBurstSz = uWordSz % *uMainBurstSz;
				*uRemainBurstSz = (*uRemainBurstSz == 0) ? *uMainBurstSz : *uRemainBurstSz;
				
				Assert(*uRemainBurstSz == 4); // Unsupported reamin burst size
			}
			else 
			{
				*uMainBurstSz = 8;
				*uRemainBurstSz = uWordSz % *uMainBurstSz;
				*uRemainBurstSz = (*uRemainBurstSz == 0) ? *uMainBurstSz : *uRemainBurstSz;
				if ((*uRemainBurstSz!=4)&&(*uRemainBurstSz!=8))
				{
					*uMainBurstSz = 4;
					*uRemainBurstSz = uWordSz % *uMainBurstSz;
					
					Assert(*uRemainBurstSz == 4); // Unsupported remain burst size
				}
			}
		}
	}

	// 3. If the output rotator degree=0, calculate main burst size and remain burst size
	//========================================================
	else if (oCim.m_eOutRotDeg==ROT_0 )
	{
		*uMainBurstSz = 16;
		*uRemainBurstSz = uWordSz % *uMainBurstSz;
		*uRemainBurstSz = (*uRemainBurstSz == 0) ? *uMainBurstSz : *uRemainBurstSz;
		
		if ((*uRemainBurstSz!=4)&&(*uRemainBurstSz!=8)&&(*uRemainBurstSz!=16))
		{
			*uMainBurstSz = 8;
			*uRemainBurstSz = uWordSz % *uMainBurstSz;
			*uRemainBurstSz = (*uRemainBurstSz == 0) ? *uMainBurstSz : *uRemainBurstSz;
			
			if ((*uRemainBurstSz!=4)&&(*uRemainBurstSz!=8))
			{
				*uMainBurstSz = 4;
				*uRemainBurstSz = uWordSz % *uMainBurstSz;
				*uRemainBurstSz = (*uRemainBurstSz == 0) ? *uMainBurstSz : *uRemainBurstSz;
				
				Assert(*uRemainBurstSz == 4); // Unsupported reamin burst size
			}
		}
	}
	else
	{
		Assert(0);
	}
}




//////////
// Function Name : CAMERA_CalcYCbCrBurstSize
// Function Description : calculation ycbcr burst length
// Input : 	
//		BURST_MODE eWantBurstSz(Wanted burst mode size), 
// 		u32* uYMainBurstSz(Y Main burst length), u32* uYRemainBurstSz(Y Remained burst length), u32* uCMainBurstSz(C Main burst length), u32* uCRemainBurstSz(C Remained burst length)
// Output :    	None
void CAMERA_CalcYCbCrBurstSize( BURST_MODE eWantBurstSz, 
	u32* uYMainBurstSz, u32* uYRemainBurstSz, u32* uCMainBurstSz, u32* uCRemainBurstSz)
{
	u32 uWantedRemainBurstSz; 
	u32 uWordSz;
	u32 uWantedBurstSz;
	// 1. Set destination size which expressed by word and wanted burst size
	//================================================
	uWordSz = (oCim.m_eDstFmt == YC420 || oCim.m_eDstFmt == YC422) ? oCim.m_uDstHsz/4 : oCim.m_uDstHsz/2;
	uWantedBurstSz =(eWantBurstSz == BURST16) ? 16 :
		(eWantBurstSz == BURST8) ? 8 :
		(eWantBurstSz == BURST4) ? 4 : 4;

	// 2. When the output rotator degree=90, calculate main burst size and remain burst size
	//=============================================================
	if (oCim.m_eOutRotDeg==ROT_90)
	{
		if (oCim.m_eProcessPath==C_PATH)
		{
			Assert(oCim.m_uDstHsz <= OUTPUT_MAX_HSIZE_ROT_RGB_C/2); // overstep the max.horizontal size can support 
		}
		else
		{
			Assert(oCim.m_uDstHsz <= OUTPUT_MAX_HSIZE_ROT_RGB_P/2); // overstep the max.horizontal size can support 
		}
			
		Assert(oCim.m_uDstHsz%8 == 0); // The targetvsize should be 8's mutiple
		
		if ( oCim.m_eDstFmt == YCBYCR||oCim.m_eDstFmt == YCRYCB||oCim.m_eDstFmt == CRYCBY||oCim.m_eDstFmt == CBYCRY) 
		{
			uWantedBurstSz = 4;
			*uYMainBurstSz = 2;
			uWantedRemainBurstSz = uWordSz % uWantedBurstSz;
			uWantedRemainBurstSz = (uWantedRemainBurstSz == 0) ? uWantedBurstSz : uWantedRemainBurstSz;
			*uYRemainBurstSz = uWantedRemainBurstSz/2; 
		}
		else// Not Interleave
		{
			*uYMainBurstSz = 2;
			*uYRemainBurstSz = 2;
			*uCMainBurstSz = 1;
			*uCRemainBurstSz = 1;
		}

	}

	// 3. If the output rotator degree=0, calculate main burst size and remain burst size
	//========================================================
	else if (oCim.m_eOutRotDeg==ROT_0 )
	{
		uWantedRemainBurstSz = uWordSz % uWantedBurstSz;
		
		if (uWantedRemainBurstSz == 0)
			uWantedRemainBurstSz = uWantedBurstSz;
		
		if ( oCim.m_eDstFmt == YCBYCR || oCim.m_eDstFmt == YCRYCB || oCim.m_eDstFmt == CRYCBY || oCim.m_eDstFmt == CBYCRY) // if interleave ...
		{
			if (uWantedBurstSz == 8 && uWantedRemainBurstSz != 8 && uWantedRemainBurstSz != 4)
			{
				uWantedBurstSz = 4;
				uWantedRemainBurstSz = uWordSz % uWantedBurstSz;
				uWantedRemainBurstSz = (uWantedRemainBurstSz == 0) ? uWantedBurstSz : uWantedRemainBurstSz;
			}
			else if (uWantedBurstSz == 16)
			{
				*uYMainBurstSz = 8, *uCMainBurstSz = 4;

				if (uWantedRemainBurstSz == 16)
					*uYRemainBurstSz = 8, *uCRemainBurstSz = 4;
				else if (uWantedRemainBurstSz == 8)
					*uYRemainBurstSz = 4, *uCRemainBurstSz = 2; 
				else if (uWantedRemainBurstSz == 4)
					*uYRemainBurstSz = 2, *uCRemainBurstSz = 1; 
				else
				{
					Assert(0); // Unsupported burst size!
				}
			}
			else if (uWantedBurstSz == 8)
			{
				*uYMainBurstSz = 4, *uCMainBurstSz = 2;

				if (uWantedRemainBurstSz == 8)
					*uYRemainBurstSz = 4, *uCRemainBurstSz = 2; 
				else if (uWantedRemainBurstSz == 4)
					*uYRemainBurstSz = 2, *uCRemainBurstSz = 1; 
				else
				{
					Assert(0); // Unsupported burst size!
				}
			}
			else if (uWantedBurstSz == 4)
			{
				*uYMainBurstSz = 2, *uCMainBurstSz = 1;

				if (uWantedRemainBurstSz == 4)
					*uYRemainBurstSz = 2, *uCRemainBurstSz = 1; 
				else
				{
					Assert(0); // Unsupported burst size!
				}
			}
			else
			{
				Assert(0); // Unsupported burst size!
			}
		}
		else
		{
			if (uWantedBurstSz == 16 && uWantedRemainBurstSz != 16 && uWantedRemainBurstSz != 8 && uWantedRemainBurstSz != 4)
			{
				uWantedBurstSz = 8; 
				uWantedRemainBurstSz = uWordSz % uWantedBurstSz;
				uWantedRemainBurstSz = (uWantedRemainBurstSz == 0) ? uWantedBurstSz : uWantedRemainBurstSz;
			}
			if (uWantedBurstSz == 8 && uWantedRemainBurstSz != 8 && uWantedRemainBurstSz != 4)
			{
				uWantedBurstSz = 4; 
				uWantedRemainBurstSz = uWordSz % uWantedBurstSz;
				uWantedRemainBurstSz = (uWantedRemainBurstSz == 0) ? uWantedBurstSz : uWantedRemainBurstSz;
			}
			if (uWantedBurstSz == 4)
			{
				Assert(uWantedRemainBurstSz == 4);
			}
			
			*uYMainBurstSz = uWantedBurstSz;
			*uYRemainBurstSz = uWantedRemainBurstSz;

			*uCMainBurstSz = *uYMainBurstSz/2;
			*uCRemainBurstSz = *uYRemainBurstSz/2;
		}
	}
	else
	{
		Assert(0);
	}
}





//////////
// Function Name : CAMERA_CalcRationAndShift
// Function Description : calculation H/V ratio and shift
// Input : 	u32 uSrcHOrVS(source image siez), u32 uDstHOrVSz(Destination size), u32* uRatio(H/V ratio), u32* uShift(H/V shift)
// Output :    	None
void CAMERA_CalcRatioAndShift(u32 uSrcHOrVSz, u32 uDstHOrVSz, u32* uRatio, u32* uShift)
{
	Assert(uSrcHOrVSz <= 64*uDstHOrVSz); 
	
	if (uSrcHOrVSz>=32*uDstHOrVSz)
		*uRatio=32, *uShift=5;
	else if (uSrcHOrVSz>=16*uDstHOrVSz)
		*uRatio=16, *uShift=4;
	else if (uSrcHOrVSz>=8*uDstHOrVSz)
		*uRatio=8, *uShift=3;
	else if (uSrcHOrVSz>=4*uDstHOrVSz)
		*uRatio=4, *uShift=2;
	else if (uSrcHOrVSz>=2*uDstHOrVSz)
		*uRatio=2, *uShift=1;
	else
		*uRatio=1, *uShift=0;
}


//////////
// Function Name : CAMERA_GetSrcImgSz
// Function Description : Source image size H/V clac..
// Input : 	u32* uSrcHSz(source H size), u32* uSrcVSz(source V size)
// Output :    	None
void CAMERA_GetSrcImgSz(u32* uSrcHSz, u32* uSrcVSz)
{
	*uSrcHSz = oCim.m_uSrcHsz;
	*uSrcVSz = oCim.m_uSrcVsz;
}


//////////
// Function Name : CAMERA_InitPreviewPath
// Function Description : Init Preview path setting
// Input : 	
// 		u32 uDstHsz(destination H size), u32 uDstVsz(Destination V size), u32 uDstAddr(Destination address), CSPACE eDstFmt(Destination format),
//		FLIP_DIR eFlip(Flip mode), ROT_DEG eRotDeg(Retation mode)
// Output :    	None
void CAMERA_InitPreviewPath(u32 uDstHsz, u32 uDstVsz, u32 uDstAddr, CSPACE eDstFmt,
		FLIP_DIR eFlip, ROT_DEG eRotDeg)
{
	CAMERA_InitPreviewPath0(0, 0, oCim.m_uSrcHsz, oCim.m_uSrcVsz,
		uDstHsz, uDstVsz, 0, 0, uDstHsz, uDstVsz,
		uDstAddr, uDstAddr, eDstFmt, eFlip, eRotDeg);
}

//////////
// Function Name : CAMERA_ResetCameraModule
// Function Description : Init Preview path test. for tvencoder test using
// Input : 	
//		u32 uDstHsz(Destinatin image H size), u32 uDstVsz(Destinatin image V size), u32 uDstAddr0(Destination address 0 ) , u32 uDstAddr1(Destinatino address 1), CSPACE eDstFmt(Destination format),
//		FLIP_DIR eFlip(Flip mode), ROT_DEG eRotDeg(Rotation mode)
// Output :    	None
void CAMERA_InitPreviewPath_1(u32 uDstHsz, u32 uDstVsz, u32 uDstAddr0, u32 uDstAddr1, CSPACE eDstFmt, FLIP_DIR eFlip, ROT_DEG eRotDeg)
{
	CAMERA_InitPreviewPath0(0, 0, oCim.m_uSrcHsz, oCim.m_uSrcVsz,
		uDstHsz, uDstVsz, 0, 0, uDstHsz, uDstVsz,
		uDstAddr0, uDstAddr1, eDstFmt, eFlip, eRotDeg);
}



//////////
// Function Name : CAMERA_InitCodecPath
// Function Description : Initialize codec path operation
//				 Fifo input c_path dma output
// Input : 	
//		u32 uDstHsz(Destination H size), u32 uDstVsz(Destination V size), u32 uDstAddr(Destination address), CSPACE eDstFmt(Destination Format),
//		FLIP_DIR eFlip(Flip mode), ROT_DEG eRotDeg(Rotation mode)
// Output :    	None
void CAMERA_InitCodecPath(u32 uDstHsz, u32 uDstVsz, u32 uDstAddr, CSPACE eDstFmt,
		FLIP_DIR eFlip, ROT_DEG eRotDeg)
{
	CAMERA_InitCodecPath0(0, 0, oCim.m_uSrcHsz, oCim.m_uSrcVsz,
		uDstHsz, uDstVsz, 0, 0, uDstHsz, uDstVsz,
		uDstAddr, uDstAddr, eDstFmt, eFlip, eRotDeg);
}


//////////
// Function Name : CAMERA_initDmaInPath
// Function Description : initialize DMA In Path 
// Input : 	
//		u32 uSrcOrgHsz(Original surce image h size), u32 uSrcOrgVsz(Original source V size), u32 uSrcAddr(Sourceimage address), CSPACE eSrcFmt(Source image format)
//		u32 uDstHsz(Destination h size), uDstVsz(Destination V size), u32 uDstAddr(Destination address), CSPACE eDstFmt(Destination format),
//		FLIP_DIR eFlip(Flip mode), ROT_DEG eRotDeg(Rotation mode), PROCESS_PATH ePathMode(Proces path)
// Output :    	None
void CAMERA_InitDmaInPath(u32 uSrcOrgHsz, u32 uSrcOrgVsz, u32 uSrcAddr, CSPACE eSrcFmt,
		u32 uDstHsz, u32 uDstVsz, u32 uDstAddr, CSPACE eDstFmt,
		FLIP_DIR eFlip, ROT_DEG eRotDeg, PROCESS_PATH ePathMode)
{
	CAMERA_InitDmaInPath0(uSrcOrgHsz, uSrcOrgVsz, 0, 0,
		uSrcOrgHsz, uSrcOrgVsz, uSrcAddr, uSrcAddr, eSrcFmt,
		uDstHsz, uDstVsz, 0, 0, uDstHsz, uDstVsz,
		uDstAddr, uDstAddr, eDstFmt, eFlip, eRotDeg, ePathMode);
}

//////////
// Function Name : CAMERA_InitPreviewPathToFifoOut
// Function Description : initialize Preview-FIFOout operation 
// Input : 	u32 uDstHsz(Destination H size), u32 uDstVsz(Destination V size), CSPACE eDstFmt(Destinatino format)
// Output :    	None
// Fifo input p_path fifo output
void CAMERA_InitPreviewPathToFifoOut(u32 uDstHsz, u32 uDstVsz, CSPACE eDstFmt)
{
	CAMERA_InitPreviewPathToFifoOut0(0, 0, oCim.m_uSrcHsz, oCim.m_uSrcVsz, uDstHsz, uDstVsz, eDstFmt);
}

//////////
// Function Name : CAMERA_InitDmaInPathToFifoOut
// Function Description : Initialize DMA in path Fifo out operation
// Input : 	
//		u32 uSrcOrgHsz(Origianl source image H size), u32 uSrcOrgVsz(Original image V size), u32 uSrcAddr(Source image address), CSPACE eSrcFmt(Source image format),
//		u32 uDstHsz(Destination H size), u32 uDstVsz(Destination V size), CSPACE eDstFmt(Destination format),
//		ROT_DEG eRotDeg(Rotation mode), PROCESS_PATH ePathMode(Process mode)
// Output :    	None
void CAMERA_InitDmaInPathToFifoOut(u32 uSrcOrgHsz, u32 uSrcOrgVsz, u32 uSrcAddr, CSPACE eSrcFmt,
		u32 uDstHsz, u32 uDstVsz, CSPACE eDstFmt,
		ROT_DEG eRotDeg, PROCESS_PATH ePathMode)
{
	CAMERA_InitDmaInPathToFifoOut0(uSrcOrgHsz, uSrcOrgVsz, 0, 0,
		uSrcOrgHsz, uSrcOrgVsz, uSrcAddr, uSrcAddr, eSrcFmt,
		uDstHsz, uDstVsz, eDstFmt,
		eRotDeg, ePathMode);
}




//////////
// Function Name : CAMERA_InitPreviewPath2
// Function Description : Init previewpath operation 
// Input : 	
//		u32 uSrcCropStartX(Window offset X1), u32 uSrcCropStartY(Window offset Y1), u32 uSrcCropHSz(Source crop H size), u32 uSrcCropVSz(source crop V size),
//		u32 uDstHsz(Destinatino H size), u32 uDstVsz(Destination V size), u32 uDstAddr(Destination address),  CSPACE eDstFmt(Destination format),
//		FLIP_DIR eFlip(flip mode) , ROT_DEG eRotDeg(rotation mode) 
// Output :    	None
void CAMERA_InitPreviewPath2(u32 uSrcCropStartX, u32 uSrcCropStartY, u32 uSrcCropHSz, u32 uSrcCropVSz,
		u32 uDstHsz, u32 uDstVsz, u32 uDstAddr,  CSPACE eDstFmt,
		FLIP_DIR eFlip , ROT_DEG eRotDeg )
{
	CAMERA_InitPreviewPath0(uSrcCropStartX, uSrcCropStartY, uSrcCropHSz, uSrcCropVSz,
		uDstHsz, uDstVsz, 0, 0, uDstHsz, uDstVsz,
		uDstAddr, uDstAddr, eDstFmt, eFlip, eRotDeg);
}

//////////
// Function Name : CAMERA_GetFrameNum
// Function Description : get capture frame number
// Input : 	PROCESS_PATH eProcessPath(Process path), u32* uFrmNum(Frame number)
// Output :    	None
void CAMERA_GetFrameNum(PROCESS_PATH eProcessPath, u32* uFrmNum)
{
	u32 uSfr;
	
	if (eProcessPath==P_PATH)
	{
		uSfr = Inp32(&CAMERAR->rCIPRSTATUS);
		*uFrmNum = (uSfr&(0x3<<26))>>26;
	}
	else
	{
		uSfr = Inp32(&CAMERAR->rCICOSTATUS);
		*uFrmNum = (uSfr&(0x3<<26))>>26;
	}
}


//////////
// Function Name : CAMERA_SetclearPreviewInt
// Function Description : Level interrupt bit clear in preview path
// Input : 	None
// Output :    	None
void	CAMERA_SetClearPreviewInt(void)
{
	u32 uTemp;
	uTemp = Inp32(&CAMERAR->rCIGCTRL);
	uTemp = uTemp | (0x1<<18);
	Outp32(&CAMERAR->rCIGCTRL, uTemp);
}

//////////
// Function Name : CAMERA_SetClearCodecInt
// Function Description : Codec level interrupt bit clear
// Input		None
// Output :    	None
void	CAMERA_SetClearCodecInt(void)
{
	u32 uTemp;
	uTemp = Inp32(&CAMERAR->rCIGCTRL);
	uTemp = uTemp | (0x1<<19); // Codec Level Interrupt Bit Clear
	Outp32(&CAMERAR->rCIGCTRL, uTemp);
}


//////////
// Function Name : CAMERA_SFRInit
// Function Description : Camera SFR baseaddress initialize.
// Input : 	None
// Output :    	None
void CAMERA_SFRInit(void)
{
	u32 uBaseAddress;

	uBaseAddress = CAMERA_BASE;
	CAMERA_pBase = (void *)uBaseAddress;
}

//////////
// Function Name : CAMERA_EnablePreviewLastIRQ
// Function Description : Preview Path Last IRQ optin Enable
// Input : 	None
// Output :    	None
void CAMERA_EnablePreviewLastIRQ(void)
{
	u32 uTemp;
	uTemp = Inp32(&CAMERAR->rCIPRCTRL);
	uTemp |= (1<<2);
	Outp32(&CAMERAR->rCIPRCTRL, uTemp);
}

//////////
// Function Name : CAMERA_DisableImageCapture
// Function Description : Disable image Capture option 
// Input : 	None
// Output :    	None
void CAMERA_DisableImageCapture( void)
{
	u32 uTemp;
	uTemp = Inp32(&CAMERAR->rCIIMGCPT);
	uTemp &= ~(1<<31);
	Outp32(&CAMERAR->rCIIMGCPT, uTemp);
}

//////////
// Function Name : CAMERA_DisableImageCapturePreview
// Function Description : Preview Path Image Capture Disable
// Input : 	None
// Output :    	None
void CAMERA_DisableImageCapturePreview( void)
{
	u32 uTemp;
	uTemp = Inp32(&CAMERAR->rCIIMGCPT);
	uTemp &= ~(1<<29);
	Outp32(&CAMERAR->rCIIMGCPT, uTemp);
}

//////////
// Function Name : CAMERA_DisablePreviewScaler
// Function Description :  Preview Path Scaler Disable
// Input : 	None
// Output :    	None
void CAMERA_DisablePreviewScaler( void)
{
	u32 uTemp;
	uTemp = Inp32(&CAMERAR->rCIPRSCCTRL);
	uTemp &= ~(1<<15);
	Outp32(&CAMERAR->rCIPRSCCTRL, uTemp);
}



//////////
// Function Name : CAMERA_SetPort
// Function Description :  Camera Interface GPIO Setting. 
// Input : 	None
// Output :    	None
void CAMERA_SetPort(void)
{
	GPIO_SetFunctionAll( eGPIO_F, 0x2aaaaaa, 0); // Camera GPIO Setting : rGPFCON = 0x2aa aaaa
	GPIO_SetPullUpDownAll(eGPIO_F, 0);
}

//////////
// Function Name : CAMERA_InitCodecPathToFifoOut
// Function Description : Initialize Codec Path FIFOout operation 
// Input : 	u32 uDstHsz(Destination H size), u32 uDstVsz(Destination V size), CSPACE eDstFmt(Destinatino format)
// Output :    	None
// Fifo input p_path fifo output
void CAMERA_InitCodecPathToFifoOut(u32 uDstHsz, u32 uDstVsz, CSPACE eDstFmt)
{
	CAMERA_InitCodecPathToFifoOut0(0, 0, oCim.m_uSrcHsz, oCim.m_uSrcVsz, uDstHsz, uDstVsz, eDstFmt);
}

//////////
// Function Name : CAMERA_InitPreviewPathToFifoOut0
// Function Description : 
// 					Fifo input Codec path fifo output 
// Input : 	
//	u32 uSrcCropStartX(Window offset X1), u32 uSrcCropStartY(Window offset Y1), u32 uSrcCropHsz(Wanted source crop H size), u32 uSrcCropVsz(Wanted source srop V size), 
//	u32 uDstHsz(Destination H size), u32 uDstVsz(Destination V size), CSPACE eDstFmt(Destination format)
// Output :    	None
void CAMERA_InitCodecPathToFifoOut0(
	u32 uSrcCropStartX, u32 uSrcCropStartY, u32 uSrcCropHsz, u32 uSrcCropVsz, 
	u32 uDstHsz, u32 uDstVsz, CSPACE eDstFmt)
{
	CAMERA_ResetIp();

	CAMERA_SetBasicSfr(uSrcCropStartX, uSrcCropStartY, uSrcCropHsz, uSrcCropVsz, 
		uDstHsz, uDstVsz, 0, 0, eDstFmt, FIFO, C_PATH, FIFO, FLIP_NO, ROT_0);
}

//////////
// Function Name : CAMERA_Start Preview Path
// Function Description : Starts Preview path . 
// Input : 	u32 uCptCnt(Capture count)
// Output :    	None
void CAMERA_StartPreviewPathCPT(u32 uCptCnt)
{

	u32 uCptPtr;
	u32 uCptEnRegVal = 0; // Image Capture Enable Reg.

	
	Assert(uCptCnt < 256); //Number of frame counts must be less than 256
	
	// 1. Set c_path main-scaler control register
	//==================================
	if (oCim.m_bIsScalerBypass!= true)
	{
		oCim.m_uMainScalerCtrl |= (1<<15); // Enable main scaler
		Outp32(&CAMERAR->rCIPRSCCTRL, oCim.m_uMainScalerCtrl); 
	}

	// 2. Set image capture register
	//==================================
	if (uCptCnt>0)
	{
		Outp32(&CAMERAR->rCICPTSEQ, 0xffffffff); // Apply capture sequence 
		uCptPtr = 0;
		uCptEnRegVal |= (1<<24)|(uCptPtr<<19)|(1<<18)|(uCptCnt<<10) ; // Enable global capture and preview scaler 
	}
	uCptEnRegVal |=(1U<<31); // Enable global capture 
	uCptEnRegVal &= ~(1U<<30); // Disable Codec scaler
	if (oCim.m_bIsScalerBypass==true) // if bypass-scaler is set,
		uCptEnRegVal &= ~(1U<<29); // ImgCptEn_PrSc must be clear
	else
		uCptEnRegVal |=(1U<<29); // Enable Preview scaler
	Outp32(&CAMERAR->rCIIMGCPT, uCptEnRegVal);

}

//////////
// Function Name : CAMERA_SetBasicSfr_4Buf
// Function Description : Camera Interface(Preview/Codec) Basic SFR setting function with 4buffer pingpong memory
// Input : 	u32 uSrcCropStartX(Windoe Offset X1), u32 uSrcCropStartY(Window Offset Y1), u32 uSrcCropHsz(Wanted Source Crop H Size)
//                 u32 uSrcCropVsz(wanted soure V Size), u32 uDstHsz(Destination H Size), u32 uDstVsz(Destination VSzie)
//                 u32 uDstAddr0(Dstination DMA Address 0), u32 uDstAddr1(Destination DMA Address 1), u32 uDstAddr2(Dstination DMA Address 2), u32 uDstAddr3(Destination DMA Address 3),CSPACE eDstDataFmt(Wanted Data Format),
//	              CAMIF_INOUT eInputPath(Input Data source), PROCESS_PATH ePath(Process Path), CAMIF_INOUT eOutputMode(output mode: DMA/FIFO)
//                , FLIP_DIR eFlip(Flip Mode Selection), ROT_DEG eRotDeg(Rotation mode Selection)
// Output :    	None
void CAMERA_SetBasicSfr_4Buf(u32 uSrcCropStartX, u32 uSrcCropStartY, u32 uSrcCropHsz, u32 uSrcCropVsz,
	u32 uDstHsz, u32 uDstVsz, u32 uDstAddr0, u32 uDstAddr1, u32 uDstAddr2, u32 uDstAddr3,  CSPACE eDstDataFmt,
	CAMIF_INOUT eInputPath, PROCESS_PATH ePath, CAMIF_INOUT eOutputMode, FLIP_DIR eFlip, ROT_DEG eRotDeg)
{
	u32 uSrcHOffset1, uSrcVOffset1, uSrcHOffset2, uSrcVOffset2;
	u32 uSrcCroppingHSz, uSrcCroppingVSz;
	u32 uFlip;
	u8 bIsHScaleUp;
	u8 bIsVScaleUp;	
	u32 uRemainBurstLen=0;
	u32 uCMainBurstLen =0;
	u32 uCRemainBurstLen = 0;
	u32 uPreHRatio, uPreVRatio;
	u32 uMainHRatio, uMainVRatio; 
	u32 uShiftFactor;
	u32 uPreDstHSz, uPreDstVSz;
	u32 uHShift, uVShift;
	u32 uLimitHsz;
	u32 uInputIntlvOrder=0;
	u32 uWinOfsEn;
	int nSrcFmt = 0;
	int nSrcYCbCrOrder;
	int nOutputIntlvOrder = 0;
	int nOutRGBFMTPr = 3 ;
	int nOutFormatPr;
	u32 uOutCbCrSz;
	u32 uOutYSz;
	u32 uTargetFmt;
	u32 uDMAControl;
	u32 uPreScalerCtrl1;
	u32 uPreScalerCtrl2;

	Assert(oCim.m_uSrcHsz != 0 && oCim.m_uSrcVsz != 0);

	// 1. Set input path, output path, process path and destination format 
	//=================================================
	oCim.m_eInputPath = eInputPath;
	oCim.m_eProcessPath = ePath;
	oCim.m_eOutputPath = eOutputMode;

	if (oCim.m_eOutputPath==DMA)
	{
		Assert(eDstDataFmt ==YCBYCR || eDstDataFmt==YCRYCB ||eDstDataFmt ==CRYCBY ||eDstDataFmt==CBYCRY
			|| eDstDataFmt==RGB16 || eDstDataFmt==RGB18 ||eDstDataFmt==RGB24 
			|| eDstDataFmt==YC420 || eDstDataFmt==YC422 );
		oCim.m_eDstFmt = eDstDataFmt;
	}
	else 
	{
		oCim.m_eDstFmt = eDstDataFmt; // In fifo output path
	}

	// 2. Compute the horizontal offset and vertical offset, check whether offset size and source size meet the limitation
	//=========================================================================
	uSrcHOffset1 = uSrcCropStartX;
	uSrcVOffset1 = uSrcCropStartY;

	uSrcHOffset2 = oCim.m_uSrcHsz - (uSrcCropHsz + uSrcHOffset1);
	uSrcVOffset2 = oCim.m_uSrcVsz - (uSrcCropVsz + uSrcVOffset1);

	Assert((uSrcHOffset1%2 == 0)); // manual p24 Window horizontal offset1 should be 2's multiple!
	Assert((uSrcHOffset2%2 == 0)); // manual p27 Window horizontal offset2 should be 2's multiple!
	Assert(oCim.m_uSrcHsz >= uSrcHOffset1 + uSrcCropHsz); // Invid horizontal size of original source or cropped window or start coordinate X
	Assert(oCim.m_uSrcVsz >= uSrcVOffset1 + uSrcCropVsz); // Invid vertical size of original source or cropped window or start coordinate Y
	if (oCim.m_eInputPath==FIFO)
	{
		Assert(oCim.m_uSrcHsz%8 == 0 && oCim.m_uSrcHsz >= 16 && oCim.m_uSrcVsz >= 8); // manual p23
	}

	// 3. Set input & output rotator degree and flip
	//=============================================
	oCim.m_eInRotDeg = ROT_0;
	oCim.m_eOutRotDeg = eRotDeg;
	if (eRotDeg==ROT_180 || eRotDeg==ROT_270)
	{
		Assert(eFlip==FLIP_NO);
	}
	if (eRotDeg==ROT_180)
	{
		eRotDeg = ROT_0;
		eFlip=FLIP_XY;
	}
	if(eRotDeg==ROT_270)
	{
		eRotDeg = ROT_90;
		eFlip=FLIP_XY;
	}
	uFlip = (eFlip == FLIP_X) ? 1 : (eFlip == FLIP_Y) ? 2 : (eFlip == FLIP_XY) ? 3 : 0;
	if (oCim.m_eOutputPath==FIFO)
	{ 
		Assert(uFlip == FLIP_NO);
		if (oCim.m_eInputPath==DMA && oCim.m_eMemSrcFmt==YC420)//just YC420 support input-rotation
			oCim.m_eInRotDeg = eRotDeg;
		else
		{
			Assert(eRotDeg== ROT_0);
		}
	}
	else
		oCim.m_eOutRotDeg = eRotDeg;

	// 4. Set destination size, check whether the destination size meet the limitation.
	//======================================================
	if (oCim.m_eOutRotDeg==ROT_0)
	{
		oCim.m_uDstHsz = uDstHsz;
		oCim.m_uDstVsz = uDstVsz;
	}
	else if (oCim.m_eOutRotDeg==ROT_90 )
	{
		oCim.m_uDstHsz = uDstVsz;
		oCim.m_uDstVsz = uDstHsz;
	}
	else
	{
		Assert(0);
	}

	if (oCim.m_eInputPath==FIFO) // Manual p44 
	{
		Assert(oCim.m_uDstHsz <= oCim.m_uSrcHsz); // Horiztonal size of destnation should be less than horizontal size of source
		Assert(oCim.m_uDstVsz <= oCim.m_uSrcVsz); // Vertical size of destnation should be less than the vertical size of source
	}

	// 5. Set cropping size, check whether the cropping size meet the limitation.
	//====================================================
	if (oCim.m_eInRotDeg==ROT_0)
	{
		uSrcCroppingHSz = uSrcCropHsz;
		uSrcCroppingVSz = uSrcCropVsz;
	}
	else if (oCim.m_eInRotDeg==ROT_90 )
	{
		uSrcCroppingHSz = uSrcCropVsz;
		uSrcCroppingVSz = uSrcCropHsz;
	}
	else
	{
		Assert(0);
	}
	// manual p25 Source corpping horizontal size should be 8's multiple and minimum 16!
	Assert(uSrcCroppingHSz%8 == 0 && uSrcCroppingHSz >= 16); 


	// 6. Check whether destination size meet the limitation.
	//===================================================
	if (oCim.m_eOutputPath==DMA) // manual p31, p44
	{
		if (oCim.m_eOutRotDeg==ROT_0)
		{
			Assert(oCim.m_uDstHsz%16 == 0 && oCim.m_uDstHsz >= 16 && oCim.m_uDstVsz >= 4); 
		}
		else if(oCim.m_eOutRotDeg==ROT_90)
		{
			if (oCim.m_eProcessPath==P_PATH)
			{
				if ((oCim.m_eDstFmt==RGB24|| oCim.m_eDstFmt == RGB18) && oCim.m_uDstHsz>OUTPUT_MAX_HSIZE_ROT_RGB_P/2)
				{
					Assert(oCim.m_uDstVsz%4 ==0 ); // When output is RGB24 & h_width>160, Target vertical size should be 4's multiple!
				}
				else
				{
					Assert(oCim.m_uDstVsz%8 ==0); // When set rotation, Target vertical size should be 8's multiple!
				}
			}
			else //oCim.m_eProcessPath==C_PATH
			{
				if ((oCim.m_eDstFmt==RGB24|| oCim.m_eDstFmt == RGB18) && oCim.m_uDstHsz>OUTPUT_MAX_HSIZE_ROT_RGB_C/2)
				{
					Assert(oCim.m_uDstVsz%4 == 0); // When output is RGB24 & TargheHsize>400, Target vertical size should be 4's multiple!
				}
				else
				{
					Assert(oCim.m_uDstVsz%8 ==0); // When set rotation, Target vertical size should be 8's multiple!
				}
			}
		}
		else
		{
			Assert(0);
		}
	}

	// 7. Set bIsHScaleUp/bIsVScaleUp and oCim.m_bIsScalerBypass
	//===========================================
	bIsHScaleUp = (uSrcCroppingHSz <= oCim.m_uDstHsz) ? true : false; // Horizontal scale up/down - 1: Up, 0: Down
	bIsVScaleUp = (uSrcCroppingVSz <= oCim.m_uDstVsz) ? true : false; // Vertical scale up/donw - 1: up, 0: down

	if (oCim.m_eInputPath==FIFO && oCim.m_eOutputPath==DMA)
	{
		if (uSrcCroppingHSz==oCim.m_uDstHsz && uSrcCroppingVSz==oCim.m_uDstVsz)
		{
			if (oCim.m_eDstFmt==RGB16 || oCim.m_eDstFmt==RGB18 || oCim.m_eDstFmt==RGB24)
				oCim.m_bIsScalerBypass = false;
			else
				oCim.m_bIsScalerBypass = true;
		}
		else
			oCim.m_bIsScalerBypass = false;
	}
	else
		oCim.m_bIsScalerBypass = false;

	// 8. Check whether oCim.m_bIsScalerBypass, oCim.m_eOutRotDeg and destination size meet the limitation
	//================================================================
	if (oCim.m_bIsScalerBypass)
	{
		Assert(oCim.m_eOutRotDeg == ROT_0); // When ScalerBypass, can not support rotator!
	}
		
	if (oCim.m_eInputPath==DMA)
	{
		Assert(oCim.m_bIsScalerBypass!=true); // Memory input can not support ScalerBypass!
	}

	if (oCim.m_eProcessPath==P_PATH) // The max horizontal size.-----page 3.
	{
		if (oCim.m_eOutRotDeg==ROT_0 )
		{
			if (oCim.m_bIsScalerBypass) 
			{
				Assert(oCim.m_uDstHsz <= SCALER_BYPASS_MAX_HSIZE); //In Pre-view Path, if scaler bypass, the DstHsize should <4096!
			}
			else 
			{
				Assert(oCim.m_uDstHsz <= SCALER_MAX_HSIZE_P); //In Pre-view Path, if scaler is used, the DstHsize should <640!
			}
		}
		else if (oCim.m_eOutRotDeg==ROT_90 ) 
		{	 
			// In Pre-view Path, if rotation & output is RGB format, the DstHsize should <320! 
				// if rotation & output is not RGB format, the DstHsize should <160!
			if (oCim.m_eDstFmt==RGB16||oCim.m_eDstFmt==RGB18||oCim.m_eDstFmt==RGB24)
			{
				Assert(oCim.m_uDstHsz <= OUTPUT_MAX_HSIZE_ROT_RGB_P);
			}
			else 
			{
				Assert(oCim.m_uDstHsz <= OUTPUT_MAX_HSIZE_ROT_RGB_P/2);
			}
		}
		else
		{
			Assert(0);
		}

		if (oCim.m_eInRotDeg==ROT_90 )
		{
			Assert(uSrcCropHsz<=INPUT_MAX_HSIZE_ROT_P&& uSrcCropVsz<=INPUT_MAX_VSIZE_ROT_P);
		}
		
		if (oCim.m_bIsScalerBypass)//In Pre-view Path, if scaler bypass, the scaler input horizontal size should <4096!
		{
			Assert(uSrcCroppingHSz <= SCALER_BYPASS_MAX_HSIZE); 
		}
		
	} 
	else // oCim.m_eProcessPath == C_PATH
	{ 
		if (oCim.m_eOutRotDeg==ROT_0 )
		{
			if (oCim.m_bIsScalerBypass)
			{
				Assert(oCim.m_uDstHsz <= SCALER_BYPASS_MAX_HSIZE); //In Codec Path, if scaler bypass, the DstHsize should <4096!
			}
			else
			{
				Assert(oCim.m_uDstHsz <= SCALER_MAX_HSIZE_C); // In Codec Path, if scaler is used, the DstHsize should <1600!
			}
		}
		else if (oCim.m_eOutRotDeg==ROT_90 )
		{
			//In Codec Path, if rotation & output is RGB format, the DstHsize should <800!
				// if rotation & output is not RGB format, the DstHsize should <400!
			if (oCim.m_eDstFmt==RGB16 || oCim.m_eDstFmt==RGB18 || oCim.m_eDstFmt==RGB24)
			{
				Assert(oCim.m_uDstHsz <= OUTPUT_MAX_HSIZE_ROT_RGB_C); 
			}
			else 
			{
				Assert(oCim.m_uDstHsz <= OUTPUT_MAX_HSIZE_ROT_RGB_C/2); 
			}
		}
		else
		{
			Assert(0);
		}

		if (oCim.m_eInRotDeg==ROT_90 )
		{
			Assert(uSrcCropHsz<=INPUT_MAX_HSIZE_ROT_C && uSrcCropVsz<=INPUT_MAX_VSIZE_ROT_C);
		}

		if (oCim.m_bIsScalerBypass) //In Codec Path, if scaler bypass, the scaler input horizontal size should <4096!
		{
			Assert(uSrcCroppingHSz <= SCALER_BYPASS_MAX_HSIZE); 
		}
	}

	// 9. Compute the burst size
	//======================================

	if ( oCim.m_eDstFmt == RGB16 || oCim.m_eDstFmt==RGB18 || oCim.m_eDstFmt == RGB24)
		CAMERA_CalcRgbBurstSize(&oCim.m_uMainBurstSz, &uRemainBurstLen);
	else// YCbCr format
		CAMERA_CalcYCbCrBurstSize( BURST16, &oCim.m_uMainBurstSz, &uRemainBurstLen, &uCMainBurstLen, &uCRemainBurstLen); 

	// 10. Calculate the pre-scaler ratio and shift factor
	//=======================================

	if (!oCim.m_bIsScalerBypass)
	{

		
		CAMERA_CalcRatioAndShift(uSrcCroppingHSz, oCim.m_uDstHsz, &uPreHRatio, &uHShift);
		CAMERA_CalcRatioAndShift(uSrcCroppingVSz, oCim.m_uDstVsz, &uPreVRatio, &uVShift);
		
		uShiftFactor = 10 - (uHShift + uVShift);
		uPreDstHSz = uSrcCroppingHSz/uPreHRatio;
		uPreDstVSz = uSrcCroppingVSz/uPreVRatio;
		if (oCim.m_eProcessPath==P_PATH)
		{
			Assert(uPreDstHSz <= SCALER_MAX_HSIZE_P); //In Pre-view Path, if use scaler, the scaler input horizontal size should <640!
		}
		else
		{
			Assert(uPreDstHSz <= SCALER_MAX_HSIZE_C); //In Codec Path, if use scaler, the scaler input horizontal size should <1600!
		}

		uMainHRatio = (uSrcCroppingHSz<<8)/(oCim.m_uDstHsz<<uHShift);
		uMainVRatio = (uSrcCroppingVSz<<8)/(oCim.m_uDstVsz<<uVShift); 

		uLimitHsz = (oCim.m_eProcessPath==P_PATH) ? SCALER_MAX_HSIZE_P: SCALER_MAX_HSIZE_C;
		Assert((int)(uSrcHOffset1+uSrcHOffset2) >= (int)(oCim.m_uSrcHsz - uLimitHsz*uPreHRatio)); // Unsupported horizontal offset size
		Assert((uSrcCroppingHSz % (4*uPreHRatio) == 0)); //Unsupported horizontal size of cropped window
		
		if (!bIsHScaleUp) // Scaler down
		{
			Assert(uSrcCroppingVSz % (2*uPreVRatio) == 0); //Unsupported vertical size of cropped window
		}
			
		if (oCim.m_eDstFmt==YC420)
		{
			Assert(uSrcCroppingVSz%2==0 && uSrcCroppingVSz >= 8); // Crop Vsize must be an even number and minimum 8!
		}
		
	}
	else
	{
		uPreHRatio = 1, uPreVRatio = 1, uMainHRatio=1, uMainVRatio=1;
		uShiftFactor = 10, uPreDstHSz =uSrcCroppingHSz, uPreDstVSz=uSrcCroppingVSz;
	}

	// 11. Set the register of the source 
	//====================================
	oCim.m_uCigCtrl = ((oCim.m_bHighRst ? 0 : 1)<<30) 	// Camera A reset
		|(1<<29)						 		// Select Camera - 1: Cam A, 0: Cam B
		|(BY_PASS<<27) 							// Test Pattern - 00: BY_PASS 01: COLOR_BAR 10: HORIZONTAL INCR 11: VERTICAL INCR
		|((oCim.m_bInvPclk ? 1: 0)<<26) 				// 1:inverse PCLK
		|((oCim.m_bInvVsync ? 1: 0)<<25)				// 1:inverse VSYNC
		|((oCim.m_bInvHref ? 1: 0)<<24) 				// 1:inverse HREF
		|(0<<22) 								// Overflow Interrupt - 1: Enable, 0: Disalbe 
		|(1<<20)								// Level Interrupt Enable
		|(1<<21); 								// Href Mask - 1: Mask Out Href during Vsync High, 0: No Mask
	Outp32(&CAMERAR->rCIGCTRL, oCim.m_uCigCtrl);

	if (oCim.m_eInputPath==FIFO) //Camera input
	{ 

		if (oCim.m_uIfBits == 8)
		{
			uInputIntlvOrder = 
				(oCim.m_eCamSrcFmt == YCBYCR) ? 0 :
				(oCim.m_eCamSrcFmt == YCRYCB) ? 1 :
				(oCim.m_eCamSrcFmt == CBYCRY) ? 2 :
				(oCim.m_eCamSrcFmt == CRYCBY) ? 3 : 0;
		}
		else // oCim.m_uIfBits == 16
		{
			uInputIntlvOrder = (oCim.m_eCamSrcFmt== YCBYCR) ? 0 : (oCim.m_eCamSrcFmt == YCRYCB) ? 1 : 0xffffffff;

			if (uInputIntlvOrder == 0xffffffff)
			{
				Assert(0); // Unsupported YCbYCr 422 order in CCIR601 16 bit mode
			}
		}
		
		Outp32(&CAMERAR->rCISRCFMT, 						// Set source format reg.
			((oCim.m_eCcir == CCIR601 ? 1 : 0)<<31) 	// 0: CCIR656, 1: CCIR601
			|(0<<30) 							// UVOffset - 1: +128, 0:+0 (normally used) 
			|((oCim.m_uIfBits == 16 ? 1 : 0)<<29)	// CCIR601 Bits - 1: 16bits, 0: 8bits
			|(oCim.m_uSrcHsz<<16) 
			|(uInputIntlvOrder<<14) 
			|(oCim.m_uSrcVsz<<0));

		uWinOfsEn = ((uSrcHOffset1==0)&&(uSrcHOffset2==0))? 0: 1 ;
		Outp32(&CAMERAR->rCIWDOFST, 			// Set window offset register 
			(uWinOfsEn<<31) 		// window offset - 1: Enable, 0: No Offset
			|(uSrcHOffset1<<16) 	// Window horizontal offset1 
			|(uSrcVOffset1<<0)); 	// Window Vertical offset1 
				
		Outp32(&CAMERAR->rCIDOWFST2, (uSrcHOffset2<<16)|uSrcVOffset2<<0); // window horizontal offset2 and window vertical offset4 
	
		Outp32(&CAMERAR->rCIIMGCPT, 0);		 // Image Cqtpure enable 

		Outp32(&CAMERAR->rCICPTSEQ, 0xffffffff); // set capture sequence reg. 
		
		if (oCim.m_eProcessPath==P_PATH)
		{
			Outp32(&CAMERAR->rCIMSCTRL, 0);
		}

		if (oCim.m_eProcessPath==C_PATH)
		{
			Outp32(&CAMERAR->rMSCOCTRL, 0);
		}

	}
	else // When input come from memory, the address can be computed by the starty_addr, startcr_addr, endy_addr....
	{
		Outp32(&CAMERAR->rCIIMGCPT, 0); // Image Cqtpure enable 

		nSrcYCbCrOrder = (oCim.m_eMemSrcFmt ==YCBYCR) ? 3 :
				(oCim.m_eMemSrcFmt ==YCRYCB) ? 1 :
				(oCim.m_eMemSrcFmt ==CBYCRY) ? 2 : 0;

		if (oCim.m_eMemSrcFmt == RGB16||oCim.m_eMemSrcFmt == RGB18||oCim.m_eMemSrcFmt == RGB24)
			nSrcFmt = 3;
		else if (oCim.m_eMemSrcFmt == YC420)
			nSrcFmt = 0;
		else if (oCim.m_eMemSrcFmt == YC422)
			nSrcFmt = 1;
		else
			nSrcFmt = 2;
		oCim.m_uMSDMACtrl = (nSrcYCbCrOrder<<4)|(1<<3)|(nSrcFmt<<1); // Set MSDMA controller
		if (oCim.m_eProcessPath==P_PATH)
			Outp32(&CAMERAR->rCIMSCTRL, oCim.m_uMSDMACtrl);
		else
			Outp32(&CAMERAR->rMSCOCTRL, oCim.m_uMSDMACtrl);
	}

	// 12. Set the register of the output 
	//===============================================

	nOutFormatPr = (oCim.m_eDstFmt== YC420) ? 0 :(oCim.m_eDstFmt== YC422) ? 1 :
		((oCim.m_eDstFmt==RGB16)||(oCim.m_eDstFmt==RGB18)||(oCim.m_eDstFmt == RGB24)) ? 3 : 2;

	if (oCim.m_eOutputPath==DMA)
	{
		if (nOutFormatPr==3)
			nOutRGBFMTPr = 
				(oCim.m_eDstFmt ==RGB16) ? 0 : 
				(oCim.m_eDstFmt ==RGB18) ? 1 : 
				(oCim.m_eDstFmt ==RGB24) ? 2 : 3;
		else if (nOutFormatPr==2)
			nOutputIntlvOrder = 
				(oCim.m_eDstFmt == YCBYCR) ? 0 : 
				(oCim.m_eDstFmt == YCRYCB) ? 1 : 
				(oCim.m_eDstFmt == CBYCRY) ? 2 : 3; 
		else
			{}
	}
	

	uOutYSz = (oCim.m_uDstHsz*oCim.m_uDstVsz);
	if (oCim.m_eDstFmt == YC422)
		uOutCbCrSz = (oCim.m_uDstHsz/2*oCim.m_uDstVsz)*1;
	else if (oCim.m_eDstFmt == YC420)
		uOutCbCrSz = (oCim.m_uDstHsz/2*oCim.m_uDstVsz/2)*1; 
	else
		uOutCbCrSz = 0;
	
	uTargetFmt = ((oCim.m_eInRotDeg==ROT_90? 1 : 0)<<31)	// 1: Input Rotate clockwize 90. 0: Input Rotator bypass.
		|(nOutFormatPr<<29) 
		|((oCim.m_uDstHsz&0x1fff)<<16)						// Horizontal size of destination 
		|(uFlip<<14) 									// 0: Normal, 1: X-axis mirror, 2: Y-axis mirror, 3: XY-axis(180 degrees) mirror
		|((oCim.m_eOutRotDeg==ROT_90 ? 1 : 0)<<13) 			// 1: Output Rotate clockwize 90. 0: Output Rotator bypass.
		|((oCim.m_uDstVsz&0x1fff)<<0);						// veritcal size of destination

	uDMAControl = (oCim.m_uMainBurstSz<<19)				// Main burst of Y length
		|(uRemainBurstLen<<14) 							// Remain burst of Y length
		|(uCMainBurstLen<<9) 							// Main burst of Cb/Cr length
		|(uCRemainBurstLen<<4)							// Remain burst of Cb/Cr length
		|(0<<2) 										// 1: Enable last IRQ at the end of frame capture, 0: normal
		|(nOutputIntlvOrder<<0); 						// YCbYCr_interleave order

	uPreScalerCtrl1 = (uShiftFactor<<28) 			// shift factor for preview
		|(uPreHRatio<<16) 								// Horizontal ratio of preview pre-scaler
		|(uPreVRatio<<0); 								// Vertical ratio of preview pre-scaler

	uPreScalerCtrl2 = (uPreDstHSz<<16) 				// Destination width for preview pre-scaler
		|(uPreDstVSz<<0); 								// Destination height for preview pre-scaler

	oCim.m_uMainScalerCtrl = ((oCim.m_bIsScalerBypass?1:0)<<31) 	// Pre-view scaler bypass
		|(bIsHScaleUp<<30)								// Horizontal salce Up/Down flag - 1: Up, 0: Down
		|(bIsVScaleUp<<29)								// Vertical salce Up/Down flag - 1: Up, 0: Down 
		|(1U<<28)										// YCbCr data dynamic range selection for csc(RGB2YCBCR)
		|(1U<<27) 										// YCbCr data dynamic range selection for csc(YCBCR2RGB)
		|(uMainHRatio<<16) 
		|(3<<13)
		|(nOutRGBFMTPr<<11)
		|(uMainVRatio<<0);

	if (oCim.m_eProcessPath==P_PATH)
	{
		if (oCim.m_bIsScalerBypass != true)
		{
			Outp32(&CAMERAR->rCIPRSCPRERATIO, uPreScalerCtrl1); 	// set preview pre-scaler control reg. 1
			Outp32(&CAMERAR->rCIPRSCPREDST, uPreScalerCtrl2); 	// set preview pre-scaler control reg. 2
		}
		Outp32(&CAMERAR->rCIPRTRGFMT, uTargetFmt); 				// Set target format reg
//		Outp32(&CAMERAR->rCIPRCTRL, uDMAControl|(1<<2));			// set DMA control reg, enable LastIRQEn
		
		if ((oCim.m_eOutputPath==DMA)) 						// Pre-view and DMAInput path
		{
			Outp32(&CAMERAR->rCIPRSCCTRL, oCim.m_uMainScalerCtrl); 	// set preview main-scaler control reg.
			Outp32(&CAMERAR->rCIPRTAREA, oCim.m_uDstVsz*oCim.m_uDstHsz); 	// set preview DMA target area reg.
		
			switch(oCim.m_eDstFmt)
			{
				case RGB24:
				case RGB18:
				case RGB16:
				case YCBYCR:
				case YCRYCB:
				case CBYCRY:
				case CRYCBY:
					Outp32(&CAMERAR->rCIPRYSA1, uDstAddr0);
					Outp32(&CAMERAR->rCIPRYSA2, uDstAddr1);
					Outp32(&CAMERAR->rCIPRYSA3, uDstAddr2);
					Outp32(&CAMERAR->rCIPRYSA4, uDstAddr3);
					break;
				case YC422:
				case YC420: 
					Outp32(&CAMERAR->rCIPRYSA1, uDstAddr0);
					Outp32(&CAMERAR->rCIPRCBSA1, uDstAddr0 + uOutYSz);
					Outp32(&CAMERAR->rCIPRCRSA1, uDstAddr0 + uOutYSz + uOutCbCrSz);
						
					Outp32(&CAMERAR->rCIPRYSA2, uDstAddr1);
					Outp32(&CAMERAR->rCIPRCBSA2, uDstAddr1 + uOutYSz);
					Outp32(&CAMERAR->rCIPRCRSA2, uDstAddr1 + uOutYSz + uOutCbCrSz);
						
					Outp32(&CAMERAR->rCIPRYSA3, uDstAddr2);
					Outp32(&CAMERAR->rCIPRCBSA3, uDstAddr2 + uOutYSz);
					Outp32(&CAMERAR->rCIPRCRSA3, uDstAddr2 + uOutYSz + uOutCbCrSz);
					
					Outp32(&CAMERAR->rCIPRYSA4, uDstAddr3);
					Outp32(&CAMERAR->rCIPRCBSA4, uDstAddr3 + uOutYSz);
					Outp32(&CAMERAR->rCIPRCRSA4, uDstAddr3 + uOutYSz + uOutCbCrSz);
					break;
				default:
					Assert(0); //Unsupported output data format
			}
		}
		else // fifo output
		{
			oCim.m_uMainScalerCtrl |= 1<<26;
			Outp32(&CAMERAR->rCIPRSCCTRL, oCim.m_uMainScalerCtrl);
		}
		Outp32(&CAMERAR->rCIPRSCOSY, 0); // set preview scan line offset reg.
		Outp32(&CAMERAR->rCIPRSCOSCB, 0); 
		Outp32(&CAMERAR->rCIPRSCOSCR, 0); 
		Outp32(&CAMERAR->rCIPRCTRL, uDMAControl&0xFFFFFFFB);	// Clear LastIRQEn
	}
	else if (oCim.m_eProcessPath==C_PATH)
	{
		if (oCim.m_bIsScalerBypass != true)
		{
			Outp32(&CAMERAR->rCICOSCPRERATIO, uPreScalerCtrl1);		// set preview pre-scaler control reg. 1
			Outp32(&CAMERAR->rCICOSCPREDST, uPreScalerCtrl2);		// set preview pre-scaler control reg. 2
		}
		Outp32(&CAMERAR->rCICOTRGFMT, uTargetFmt);					// Set target format reg
//		Outp32(&CAMERAR->rCICOCTRL, uDMAControl|(1<<2));				// set DMA control reg, enable LastIRQEn
		
		if (oCim.m_eOutputPath==DMA)
		{
			Outp32(&CAMERAR->rCICOTAREA, oCim.m_uDstVsz*oCim.m_uDstHsz); 		// set preview DMA target area reg.
			Outp32(&CAMERAR->rCICOSCCTRL, oCim.m_uMainScalerCtrl);		// set preview main-scaler control reg.

			switch(oCim.m_eDstFmt)
			{
				case RGB24:
				case RGB18:
				case RGB16:
				case YCBYCR:
				case YCRYCB:
				case CBYCRY:
				case CRYCBY:
					Outp32(&CAMERAR->rCICOYSA1, uDstAddr0);
					Outp32(&CAMERAR->rCICOYSA2, uDstAddr1);
					Outp32(&CAMERAR->rCICOYSA3, uDstAddr2);
					Outp32(&CAMERAR->rCICOYSA4, uDstAddr3);
					break;
				case YC422:
				case YC420: 
					Outp32(&CAMERAR->rCICOYSA1, uDstAddr0);
					Outp32(&CAMERAR->rCICOCBSA1, uDstAddr0 + uOutYSz);
					Outp32(&CAMERAR->rCICOCRSA1, uDstAddr0 + uOutYSz + uOutCbCrSz);
						
					Outp32(&CAMERAR->rCICOYSA2, uDstAddr1);
					Outp32(&CAMERAR->rCICOCBSA2, uDstAddr1 + uOutYSz);
					Outp32(&CAMERAR->rCICOCRSA2, uDstAddr1 + uOutYSz + uOutCbCrSz);
						
					Outp32(&CAMERAR->rCICOYSA3, uDstAddr2);
					Outp32(&CAMERAR->rCICOCBSA3, uDstAddr2 + uOutYSz);
					Outp32(&CAMERAR->rCICOCRSA3, uDstAddr2 + uOutYSz + uOutCbCrSz);
					
					Outp32(&CAMERAR->rCICOYSA4, uDstAddr3);
					Outp32(&CAMERAR->rCICOCBSA4, uDstAddr3 + uOutYSz);
					Outp32(&CAMERAR->rCICOCRSA4, uDstAddr3 + uOutYSz + uOutCbCrSz);
					break;
				default:
					Assert(0); //Unsupported output data format
			}
		}
		else // Codec and fifo path
		{
			oCim.m_uMainScalerCtrl |= 1<<26;
			Outp32(&CAMERAR->rCICOSCCTRL, oCim.m_uMainScalerCtrl);
		} 

		Outp32(&CAMERAR->rCICOSCOSY, 0); // set codec scan line offset reg.
		Outp32(&CAMERAR->rCICOSCOSCB, 0); 
		Outp32(&CAMERAR->rCICOSCOSCR, 0); 
		Outp32(&CAMERAR->rCICOCTRL, uDMAControl&0xFFFFFFFB);	// Clear LastIRQEn
	}
	else
		Assert(0);
	
	// 12.Set image effect reg. 
	Outp32(&CAMERAR->rCIIMGEFF, 0);
	
}

//////////
// Function Name : CAMERA_ResetCameraModule
// Function Description : Init Preview path test. for tvencoder test using
// Input : 	
//		u32 uDstHsz(Destinatin image H size), u32 uDstVsz(Destinatin image V size), u32 uDstAddr0(Destination address 0 ) , u32 uDstAddr1(Destinatino address 1), CSPACE eDstFmt(Destination format),
//		FLIP_DIR eFlip(Flip mode), ROT_DEG eRotDeg(Rotation mode)
// Output :    	None
void CAMERA_InitPreviewPath_4Buf(u32 uDstHsz, u32 uDstVsz, u32 uDstAddr0, u32 uDstAddr1, u32 uDstAddr2, u32 uDstAddr3,  CSPACE eDstFmt, FLIP_DIR eFlip, ROT_DEG eRotDeg)
{
	CAMERA_InitPreviewPath_4Buf_0(0, 0, oCim.m_uSrcHsz, oCim.m_uSrcVsz,
		uDstHsz, uDstVsz, 0, 0, uDstHsz, uDstVsz,
		uDstAddr0, uDstAddr1, uDstAddr2, uDstAddr3, eDstFmt, eFlip, eRotDeg);
}

//////////
// Function Name : CAMERA_InitPreviewPath0
// Function Description : Connection function Test -> SetBasicSFR Function
// Fifo input p_path dma output
// Input : 	
//	u32 uSrcCropStartX(Window Offset X1), u32 uSrcCropStartY(Window Offset X2), u32 uSrcCropHsz(Wanted Soruce Crop H Size), u32 uSrcCropVsz(Wanted Source Crop V Size), 
//	u32 uDisplayHsz(wanted display H size), u32 uDisplayVsz(wanted display V size), u32 uDstStX(display start x), u32 uDstStY(display Start Y), u32 uDstHsz(Destination H Size), u32 uDstVsz(Destination V Size), 
//	u32 uDstAddr0(Destination Address 0), u32 uDstAddr1(Destination Address 1), CSPACE eDstFmt(Destination Format), FLIP_DIR eFlip(FLIP mode), ROT_DEG eRotDeg(Rotation Mode)
// Output :    	None
void CAMERA_InitPreviewPath_4Buf_0(
	u32 uSrcCropStartX, u32 uSrcCropStartY, u32 uSrcCropHsz, u32 uSrcCropVsz, 
	u32 uDisplayHsz, u32 uDisplayVsz, u32 uDstStX, u32 uDstStY, u32 uDstHsz, u32 uDstVsz, 
	u32 uDstAddr0, u32 uDstAddr1, u32 uDstAddr2, u32 uDstAddr3, CSPACE eDstFmt, FLIP_DIR eFlip, ROT_DEG eRotDeg)
{
	CAMERA_ResetIp();

	CAMERA_SetBasicSfr_4Buf(uSrcCropStartX, uSrcCropStartY, uSrcCropHsz, uSrcCropVsz, 
		uDstHsz, uDstVsz, uDstAddr0, uDstAddr1, uDstAddr2, uDstAddr3, eDstFmt, 
		FIFO, P_PATH, DMA, eFlip, eRotDeg);

	if (uDstHsz < uDisplayHsz && uDstVsz < uDisplayVsz)
		CAMERA_SetDstScanOffset_4Buf(uDisplayHsz, uDisplayVsz, uDstStX, uDstStY, uDstAddr0, uDstAddr1, uDstAddr2, uDstAddr3);
	else if (uDstHsz > uDisplayHsz || uDstVsz > uDisplayVsz)
		{
			Assert(0);
		}
	else
		{}
}

//////////
// Function Name : CAMERA_SetDstScanOffset
// Function Description : Scan Line Offset setting function 
// Input : 	
//		u32 uDisplayHSz(Wanted display H size), u32 uDisplayVSz(Wanted display V size), 
//		u32 uDisplayStartX(Display start X), u32 uDisplayStartY(Display start Y), u32 uDstAddr0(Destination address 0 ), u32 uDstAddr1(Destination address 1)
// Output :    	None
void CAMERA_SetDstScanOffset_4Buf(u32 uDisplayHSz, u32 uDisplayVSz, 
	u32 uDisplayStartX, u32 uDisplayStartY, u32 uDstAddr0, u32 uDstAddr1, u32 uDstAddr2, u32 uDstAddr3 )
{ 
	u32 uOutYSz, uBytePerPixrl;
	u32 uInitYOffset;
	u32 uLineYOffset;
	u32 uScanYOff;
	u32 uInitCrOffset, uLineCrOffset, uInitCbOffset, uLineCbOffset, uScanCrOff ,uScanCbOff ;


	
	// 1. Compute oCim.m_uDstHsz, uOutYSz and uBytePerPixrl
	//===================================================
	oCim.m_uDstHsz = (oCim.m_eOutRotDeg==ROT_90) ? oCim.m_uDstVsz: oCim.m_uDstHsz;

	uOutYSz = uDisplayHSz*uDisplayVSz;
	uBytePerPixrl = (oCim.m_eDstFmt == RGB18||oCim.m_eDstFmt == RGB24)? 4:
		 (oCim.m_eDstFmt == YC422||oCim.m_eDstFmt == YC420) ? 1 : 2;

	// 2. Move the start point form (0,0) to (0, uDisplayStartY), then compute the start address again.
	//=============================================================
	if (oCim.m_eProcessPath==P_PATH&&oCim.m_eOutputPath==DMA)
	{
		Outp32(&CAMERAR->rCIPRYSA1, uDstAddr0 + uDisplayHSz*uDisplayStartY*uBytePerPixrl);
		Outp32(&CAMERAR->rCIPRYSA2, uDstAddr1 + uDisplayHSz*uDisplayStartY*uBytePerPixrl);
		Outp32(&CAMERAR->rCIPRYSA3, uDstAddr2 + uDisplayHSz*uDisplayStartY*uBytePerPixrl);
		Outp32(&CAMERAR->rCIPRYSA4, uDstAddr3 + uDisplayHSz*uDisplayStartY*uBytePerPixrl);
		
		if (oCim.m_eDstFmt==YC422)
		{
			Outp32(&CAMERAR->rCIPRCBSA1, uDstAddr0 + uOutYSz + uDisplayHSz*uDisplayStartY/2);
			Outp32(&CAMERAR->rCIPRCRSA1, uDstAddr0 + uOutYSz + uOutYSz/2 + uDisplayHSz*uDisplayStartY/2);
			Outp32(&CAMERAR->rCIPRCBSA2, uDstAddr1 + uOutYSz + uDisplayHSz*uDisplayStartY/2);
			Outp32(&CAMERAR->rCIPRCRSA2, uDstAddr1 + uOutYSz + uOutYSz/2 + uDisplayHSz*uDisplayStartY/2);
			Outp32(&CAMERAR->rCIPRCBSA3, uDstAddr2 + uOutYSz + uDisplayHSz*uDisplayStartY/2);
			Outp32(&CAMERAR->rCIPRCRSA3, uDstAddr2 + uOutYSz + uOutYSz/2 + uDisplayHSz*uDisplayStartY/2);
			Outp32(&CAMERAR->rCIPRCBSA4, uDstAddr3 + uOutYSz + uDisplayHSz*uDisplayStartY/2);
			Outp32(&CAMERAR->rCIPRCRSA4, uDstAddr3 + uOutYSz + uOutYSz/2 + uDisplayHSz*uDisplayStartY/2);
		}
		else if (oCim.m_eDstFmt==YC420)
		{
			Outp32(&CAMERAR->rCIPRCBSA1, uDstAddr0 + uOutYSz + uDisplayHSz*uDisplayStartY/4);
			Outp32(&CAMERAR->rCIPRCRSA1, uDstAddr0 + uOutYSz + uOutYSz/4 + uDisplayHSz*uDisplayStartY/4);
			Outp32(&CAMERAR->rCIPRCBSA2, uDstAddr1 + uOutYSz + uDisplayHSz*uDisplayStartY/4);
			Outp32(&CAMERAR->rCIPRCRSA2, uDstAddr1 + uOutYSz + uOutYSz/4 + uDisplayHSz*uDisplayStartY/4);
			Outp32(&CAMERAR->rCIPRCBSA3, uDstAddr2 + uOutYSz + uDisplayHSz*uDisplayStartY/4);;
			Outp32(&CAMERAR->rCIPRCRSA3, uDstAddr2 + uOutYSz + uOutYSz/4 + uDisplayHSz*uDisplayStartY/4);
			Outp32(&CAMERAR->rCIPRCBSA4, uDstAddr3 + uOutYSz + uDisplayHSz*uDisplayStartY/4);
			Outp32(&CAMERAR->rCIPRCRSA4, uDstAddr3 + uOutYSz + uOutYSz/4 + uDisplayHSz*uDisplayStartY/4);
		}
		else
		{
			Outp32(&CAMERAR->rCIPRCBSA1, 0);
			Outp32(&CAMERAR->rCIPRCRSA1, 0);
			Outp32(&CAMERAR->rCIPRCBSA2, 0);
			Outp32(&CAMERAR->rCIPRCRSA2, 0);
			Outp32(&CAMERAR->rCIPRCBSA3, 0);
			Outp32(&CAMERAR->rCIPRCRSA3, 0);
			Outp32(&CAMERAR->rCIPRCBSA4, 0);
			Outp32(&CAMERAR->rCIPRCRSA4, 0);
		}
	}
	
	if (oCim.m_eProcessPath==C_PATH&&oCim.m_eOutputPath==DMA)
	{
		Outp32(&CAMERAR->rCICOYSA1, uDstAddr0+uDisplayHSz*uDisplayStartY*uBytePerPixrl);
		Outp32(&CAMERAR->rCICOYSA2, uDstAddr1+uDisplayHSz*uDisplayStartY*uBytePerPixrl);
		Outp32(&CAMERAR->rCICOYSA3, uDstAddr2+uDisplayHSz*uDisplayStartY*uBytePerPixrl);
		Outp32(&CAMERAR->rCICOYSA4, uDstAddr3+uDisplayHSz*uDisplayStartY*uBytePerPixrl);
		if (oCim.m_eDstFmt==YC422)
		{
			Outp32(&CAMERAR->rCICOCBSA1, uDstAddr0 + uOutYSz + uDisplayHSz*uDisplayStartY/2);
			Outp32(&CAMERAR->rCICOCRSA1, uDstAddr0 + uOutYSz + uOutYSz/2 + uDisplayHSz*uDisplayStartY/2);
			Outp32(&CAMERAR->rCICOCBSA2, uDstAddr1 + uOutYSz + uDisplayHSz*uDisplayStartY/2);
			Outp32(&CAMERAR->rCICOCRSA2, uDstAddr1 + uOutYSz + uOutYSz/2 + uDisplayHSz*uDisplayStartY/2);
			Outp32(&CAMERAR->rCICOCBSA3, uDstAddr2 + uOutYSz + uDisplayHSz*uDisplayStartY/2);
			Outp32(&CAMERAR->rCICOCRSA3, uDstAddr2 + uOutYSz + uOutYSz/2 + uDisplayHSz*uDisplayStartY/2);
			Outp32(&CAMERAR->rCICOCBSA4, uDstAddr3 + uOutYSz + uDisplayHSz*uDisplayStartY/2);
			Outp32(&CAMERAR->rCICOCRSA4, uDstAddr3 + uOutYSz + uOutYSz/2 + uDisplayHSz*uDisplayStartY/2);
		}
		else if (oCim.m_eDstFmt==YC420)
		{
			Outp32(&CAMERAR->rCICOCBSA1, uDstAddr0 + uOutYSz + uDisplayHSz*uDisplayStartY/4);
			Outp32(&CAMERAR->rCICOCRSA1, uDstAddr0 + uOutYSz + uOutYSz/4 + uDisplayHSz*uDisplayStartY/4);
			Outp32(&CAMERAR->rCICOCBSA2, uDstAddr1 + uOutYSz + uDisplayHSz*uDisplayStartY/4);
			Outp32(&CAMERAR->rCICOCRSA2, uDstAddr1 + uOutYSz + uOutYSz/4 + uDisplayHSz*uDisplayStartY/4);
			Outp32(&CAMERAR->rCICOCBSA3, uDstAddr2 + uOutYSz + uDisplayHSz*uDisplayStartY/4);;
			Outp32(&CAMERAR->rCICOCRSA3, uDstAddr2 + uOutYSz + uOutYSz/4 + uDisplayHSz*uDisplayStartY/4);
			Outp32(&CAMERAR->rCICOCBSA4, uDstAddr3 + uOutYSz + uDisplayHSz*uDisplayStartY/4);
			Outp32(&CAMERAR->rCICOCRSA4, uDstAddr3 + uOutYSz + uOutYSz/4 + uDisplayHSz*uDisplayStartY/4);
		}
		else
		{
			Outp32(&CAMERAR->rCICOCBSA1, 0);
			Outp32(&CAMERAR->rCICOCRSA1, 0);
			Outp32(&CAMERAR->rCICOCBSA2, 0);
			Outp32(&CAMERAR->rCICOCRSA2, 0);
			Outp32(&CAMERAR->rCICOCBSA3, 0);
			Outp32(&CAMERAR->rCICOCRSA3, 0);
			Outp32(&CAMERAR->rCICOCBSA4, 0);
			Outp32(&CAMERAR->rCICOCRSA4, 0);
		}
	}

	// 3. Compute scan line offset y, cb and cr.
	//===================================================
	uDisplayStartY = 0;
	uInitYOffset = (uDisplayHSz*uDisplayStartY + uDisplayStartX);
	uLineYOffset = (uDisplayHSz - oCim.m_uDstHsz);

	uScanYOff=(uInitYOffset<<16)|(uLineYOffset<<0);

	
	if (oCim.m_eDstFmt==YC422||oCim.m_eDstFmt==YC420)
	{
		Assert((uInitYOffset%4 == 0) && (uLineYOffset%4 == 0)); //Initial Y offset must be word boundary alignment!
		
		if (oCim.m_eDstFmt==YC422) 
			uInitCbOffset = uInitCrOffset = (uDisplayHSz)*uDisplayStartY + uDisplayStartX;
		else//YC420
			uInitCbOffset = uInitCrOffset = (uDisplayHSz)*uDisplayStartY/2 + uDisplayStartX;

		uLineCbOffset = uLineCrOffset =(uDisplayHSz - oCim.m_uDstHsz );
		uScanCbOff = (uInitCbOffset<<16)|(uLineCbOffset<<0);
		uScanCrOff = (uInitCrOffset<<16)|(uLineCrOffset<<0);
		Assert((uInitCrOffset%8 == 0)); // Initial Cr offset must be word boundary alignment!
	}
	else 
	{
		uScanCrOff = uScanCbOff = 0;
		if (oCim.m_eDstFmt!=RGB24&&oCim.m_eDstFmt!=RGB18)
		{
			Assert((uInitYOffset%2==0)); // Initial Y offset must be word boundary alignment!
			Assert((uLineYOffset%2==0)); // Line Y offset must be word boundary alignment!
		}
	}
	
	// 4. Set the register of scan line offset
	//===========================================================
	if (oCim.m_eProcessPath==P_PATH&&oCim.m_eOutputPath==DMA) // FIFO mode doesn't support the scan line offset.
	{
		Outp32(&CAMERAR->rCIPRSCOSY, uScanYOff);
		Outp32(&CAMERAR->rCIPRSCOSCB, uScanCbOff);
		Outp32(&CAMERAR->rCIPRSCOSCR, uScanCrOff);
	}
	else if (oCim.m_eProcessPath==C_PATH&&oCim.m_eOutputPath==DMA)
	{
		Outp32(&CAMERAR->rCICOSCOSY, uScanYOff);
		Outp32(&CAMERAR->rCICOSCOSCB, uScanCbOff);
		Outp32(&CAMERAR->rCICOSCOSCR, uScanCrOff);
	}
	else
	{
		Outp32(&CAMERAR->rCIPRSCOSY, 0);
		Outp32(&CAMERAR->rCIPRSCOSCB, 0);
		Outp32(&CAMERAR->rCIPRSCOSCR, 0);
		Outp32(&CAMERAR->rCICOSCOSY, 0);
		Outp32(&CAMERAR->rCICOSCOSCB, 0);
		Outp32(&CAMERAR->rCICOSCOSCR, 0);
	}
}



void CAMERA_OneToOneDataCopy(PROCESS_PATH eProcessPath)
{
	u32 uSfr;
	
	if (eProcessPath==P_PATH)
	{
		Assert(oCim.m_uSrcHsz <= SCALER_MAX_HSIZE_P); 
		uSfr = Inp32(&CAMERAR->rCIPRSCCTRL);
//		Inp32(&CAMERAR->rCIPRSCCTRL, uSfr);
		uSfr |= (1<<9);
		Outp32(&CAMERAR->rCIPRSCCTRL, uSfr);
 	}
	else
	{
		Assert(oCim.m_uSrcHsz <= SCALER_MAX_HSIZE_C); 
//		Inp32(&CAMERAR->rCICOSCCTRL, uSfr);
		uSfr = Inp32(&CAMERAR->rCICOSCCTRL);
		uSfr |= (1<<9);
		Outp32(&CAMERAR->rCICOSCCTRL, uSfr);
 	}

}

void CAMERA_InitItuRInterface(ITU_R_STANDARD eItuR, CSPACE eSrcFmt, u32 uSrcHsz, u32 uSrcVsz)
{
	oCim.m_uIfBits = 8;
	oCim.m_eCamSrcFmt= eSrcFmt;
	
	if (eItuR == BT601)
	{
		oCim.m_bInvPclk  = false,
		oCim.m_bInvVsync = false,
		oCim.m_bInvHref  = false;
		oCim.m_uSrcHsz = uSrcHsz, oCim.m_uSrcVsz = uSrcVsz;  
		oCim.m_eItuR = BT601;
	}
	else if (eItuR == BT656)
	{
		oCim.m_bInvPclk  = false,
		oCim.m_bInvVsync = false,
		oCim.m_bInvHref  = false;  
		oCim.m_uSrcHsz = uSrcHsz, oCim.m_uSrcVsz = uSrcVsz;  
		oCim.m_eItuR = BT656;
	}
	else
	 	Assert(0);
}

void CAMERA_SetScanMode(TV_SCAN_MODE eScanMode)
{
	u32 uCiGCtrl;

	oCim.m_eScanMode = eScanMode;

	uCiGCtrl = Inp32(&CAMERAR->rCIGCTRL);
//	ciInp32(rCIGCTRL, uCiGCtrl);

	uCiGCtrl &= ~(1<<0);
	
	if(oCim.m_eScanMode == PROGRESSIVE)
		Outp32(&CAMERAR->rCIGCTRL, uCiGCtrl);
	else	// interlace mode
		Outp32(&CAMERAR->rCIGCTRL, uCiGCtrl|(5<<0));
}


void CAMERA_SetFieldClk( u8 bIsEdgeDlyCnt, u8 bIsNormal) // true:Edge delay count mode, false:Field port mode
{
	u32 uCiGCtrl;

	uCiGCtrl = Inp32(&CAMERAR->rCIGCTRL);
//	ciInp32(CIGCTRL, uCiGCtrl);
	uCiGCtrl &=~ (3<<1);

	if(bIsEdgeDlyCnt)
	{
		if(oCim.m_eItuR == BT656)	
			Assert(0);
		if(oCim.m_eItuR == BT601)	
		{
			if (bIsNormal)
				uCiGCtrl &=~ (1<<1);
			else	// Invert Field clock
				uCiGCtrl |= (1<<1);
		}
	}

	else	// Field mode
	{
			if (bIsNormal)
				uCiGCtrl |= (0x2<<1);
			else	// Invert Field clock
				uCiGCtrl |= (0x3<<1);
	}

	Outp32(&CAMERAR->rCIGCTRL, uCiGCtrl);
}





//////////
// Function Name : CAMERA_WaitMSDMAC
// Function Description : Wait MSDMA Operation for Codec Path
// Input : 	
// Output :    	None
void CAMERA_WaitMSDMAC(void)
{
	u32 uIndex0;
	while(1)
	{
		uIndex0 = Inp32(&CAMERAR->rMSCOCTRL);
		if ( (uIndex0&0x1) == 0 ) break;
	}
//	while( ( (*(unsigned int*)0x78000128)&0x1 != 0 ) );
}

//////////
// Function Name : CAMERA_WaitMSDMAP
// Function Description : Wait MSDMA Operation for Preivew Path
// Input : 	
// Output :    	None
void CAMERA_WaitMSDMAP(void)
{
	u32 uIndex0;
	while(1)
	{
		uIndex0 = Inp32(&CAMERAR->rCIMSCTRL);
		if ( (uIndex0&0x1) == 0 ) break;
	}
}







