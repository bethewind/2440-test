
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "sysc.h"
#include "system.h"
#include "intc.h"
#include "jpeg.h"
#include "lcd.h"
#include "camera.h"
#include "glib.h"
#include "post.h"
#include "option.h"
#include "library.h"
#include "sdhc.h"
#include "fat.h"
#include "uart.h"
#include "timer.h"

#define CAPTURED_JPEG      "Captured.jpg"
#define SAVE_JPEG		  "Saved.jpg"
#define YUV_FILE_NAME      "Captured.yuv"

static POST	oPost;
static SDHC   oSdhc;

static CSPACE eLcdBpp;
static u32 uLcdFbAddr, uLcdHSz, uLcdVSz;

static volatile u8 bCamifDone;

static CSPACE eRawImgFormat;
static u32 uRawImgHsz_Enc, uRawImgVsz_Enc, uGlibAddr;
static u32 uRawImgHSz_Dec, uRawImgVSz_Dec, uRawImgAddr;

static u32 uIntCnt;
static u8 bIsOneStep;
static u8 bIsJpgLoaded;

static u32 uJpgSize;
static u32 uJpgAddr;
static JPEG_TYPE eJpgFormat;
static JPEG_IRQSTATUS eJpgIrq;
static JPEG_STATUS eJpgSts;

#define WaitForSet(bDone) while (!bDone)
#define WaitForConversionDone(bDone) while(!bDone)

void __irq Isr_Camif_P(void)
{
	LCD_Trigger();
	//bPreviewDone = true;
	//UART_Printf("P");
	CAMERA_SetClearPreviewInt();
	INTC_ClearVectAddr();		
}

void __irq Isr_Camif_C(void)
{
	#if 0 
	INTC_Disable(NUM_CAMIF_C );
	bCamifDone = true;	
	CAMERA_SetClearCodecInt();
	INTC_Disable(NUM_CAMIF_C );
	INTC_ClearVectAddr();
	#else
//	LCD_Trigger();
//	UART_Printf("C");
	bCamifDone = true;
	CAMERA_SetClearCodecInt();
	INTC_ClearVectAddr();
	#endif
	}

static volatile u8 bJpegDone;

void __irq Isr_Jpeg(void)
{
	u32 uStatus;

	INTC_Disable(NUM_JPEG);

	
	if ( (bIsOneStep == false) || ((bIsOneStep == true)&&(uIntCnt == 1)) )
	{
		JPEG_ReadAndClearStatus(&eJpgSts, &eJpgIrq);

		switch (eJpgIrq)
			{
			case 0x08 : UART_Printf("JPGIRQ : 0x%x   Header parsing OK\n",eJpgIrq); break;	//OK_HD_PARSING
			case 0x00 : UART_Printf("JPGIRQ : 0x%x   Header parsing ERR. Image size and sampling factor value cannot be read\n",eJpgIrq); break;  //ERR_HD_PARSING1
			case 0x40 : UART_Printf("JPGIRQ : 0x%x   Enc or Dec is OK\n",eJpgIrq); break;	//ERR_HD_PARSING2
			case 0x10: UART_Printf("JPGIRQ : 0x%x   finished abnormally & Syntax and Size,Sampling Error occured \n",eJpgIrq); break;			//OK_ENC_OR_DEC
			case 0x18: UART_Printf("JPGIRQ : 0x%x   finished abnormally & compressed file has Syntax error\n",eJpgIrq); break;			//ERR_ENC_OR_DEC
			}

		uIntCnt = 0;
	}

	else if (bIsOneStep == true )
	{


		JPEG_ReadAndClearStatus(&eJpgSts, &eJpgIrq);

		if (eJpgSts == JPEG_DONE)
			{
				switch (eJpgIrq)
					{
					case 0x08 : UART_Printf("JPGIRQ : 0x%x   finished abnormally & image size and sampling factor value can be read\n",eJpgIrq); break;	//OK_HD_PARSING
					case 0x00 : UART_Printf("JPGIRQ : 0x%x   finished abnormally & Image size and sampling factor value cannot be read\n",eJpgIrq); break;  //ERR_HD_PARSING1
					case 0x40 : UART_Printf("JPGIRQ : 0x%x   Enc or Dec is OK\n",eJpgIrq); break;	//ERR_HD_PARSING2
					case 0x10: UART_Printf("JPGIRQ : 0x%x   finished abnormally & Syntax and Size,Sampling Error occured \n",eJpgIrq); break;		//OK_ENC_OR_DEC
					case 0x18: UART_Printf("JPGIRQ : 0x%x   finished abnormally & compressed file has Syntax error \n",eJpgIrq); break;		//ERR_ENC_OR_DEC
					}
			}
	}
	



	//INTC_ClearPending(INT_JPEG);
	bJpegDone = true;
	uIntCnt++;
//	UART_Printf("bJpegDone: %d\n", bJpegDone); 


	INTC_Enable(NUM_JPEG);
	INTC_ClearVectAddr();	
}

static void GetJpegStreamFromPc(void)
{
	#if	 SEMIHOSTING
		const char *pFileName = CAPTURED_JPEG;
		UART_Printf(" Loading %s from PC ...\n", pFileName);
		LoadFromFile1(pFileName, uJpgAddr, &uJpgSize);

		UART_Printf(" Stream size = %d bytes\n", uJpgSize);
		bIsJpgLoaded = true;
	#else
		UART_Printf("Check Semihosting....\n");
	#endif
}

static void GetJpegStreamFromSd(void)
{
#if !SEMIHOSTING
	u32 uTotalNumOfFiles, i;
	int sel;
	char pFileName[50];

	//if (!SDHC_OpenMedia(SDHC_HCLK, &oSdhc) || !FAT_LoadFileSystem(&oSdhc))
	if (!SDHC_OpenMediaWithMode(4, SDHC_POLLING_MODE, SDHC_HCLK, 4, 1, &oSdhc) || !FAT_LoadFileSystem(&oSdhc))

	
		Assert(0);
	
	FAT_GetTotalNumOfFiles(&uTotalNumOfFiles, &oSdhc);
	for (i=0; i<uTotalNumOfFiles; i++)
	{
		FAT_GetFileName(i, pFileName, &oSdhc);
		UART_Printf("%02d -- %s\n", i, pFileName);
	}

	UART_Printf(" Which file to read ? ");
	sel = UART_GetIntNum();
	if (!FAT_ReadFile2(sel, uJpgAddr, &uJpgSize, &oSdhc))
		UART_Printf(" Loading failed ....\n");

	FAT_UnloadFileSystem(&oSdhc);
	SDHC_CloseMedia(&oSdhc);

	UART_Printf(" Stream size = %d bytes\n", uJpgSize);
	bIsJpgLoaded = true;
#endif
}

void TestEncoding(void)
{

	u32 uSelFmt;
	
	bIsOneStep = false;	

	// Input Encoding format
	while (true)
	{
		UART_Printf("\n[0] Exit\n");
		UART_Printf("[1] YUV422 \n");
		UART_Printf("[2] YUV420 \n");
		UART_Printf("\nEnter the encoding format: ");

		uSelFmt = UART_GetIntNum();

		if (uSelFmt == 0)
			return;
		else if (uSelFmt >=1 && uSelFmt <=2)
		{
			eJpgFormat = (uSelFmt == 1) ? JPEG_422: JPEG_420;
			UART_Printf("eJpgFormat = 0x%x\n", eJpgFormat);
			break;
		}
		else
			UART_Printf("Invalid Input! Retry It!!\n");
	}

	// Input Encoding test size
	UART_Printf("\nInput JPEG X size : ");	uRawImgHsz_Enc = UART_GetIntNum();
	UART_Printf("\nInput JPEG Y size : ");	uRawImgVsz_Enc = UART_GetIntNum();

	
	Assert(eRawImgFormat == YCBYCR || eRawImgFormat == RGB16);

	INTC_Init();
	INTC_SetVectAddr(NUM_JPEG, Isr_Jpeg);
	INTC_SetVectAddr(NUM_CAMIF_C, Isr_Camif_C);	
	INTC_Enable(NUM_JPEG);
	INTC_Enable(NUM_CAMIF_C);

	// Draw src. img.
	GLIB_InitInstance(uGlibAddr, uRawImgHsz_Enc, uRawImgVsz_Enc, RGB16);
	GLIB_DrawPattern(uRawImgHsz_Enc, uRawImgVsz_Enc);

	if (eRawImgFormat == YCBYCR)
	{
		POST_InitIp(	uRawImgHSz_Dec, uRawImgVSz_Dec, uRawImgAddr, YCBYCR,
					uLcdHSz, uLcdVSz, uLcdFbAddr, eLcdBpp, 1, false, ONE_SHOT, &oPost);
		bCamifDone = false;
		POST_StartProcessing(&oPost);
		while (!POST_IsFreeRunDone(&oPost));
	}

	uRawImgAddr = (eRawImgFormat == YCBYCR) ? uRawImgAddr : uGlibAddr;

	bJpegDone = false;
	JPEG_StartEncodingOneFrame(uRawImgHsz_Enc, uRawImgVsz_Enc, uRawImgAddr, eRawImgFormat,
						  		 uJpgAddr, eJpgFormat);
	WaitForSet(bJpegDone);

	Assert(eJpgIrq == OK_ENC_OR_DEC);

	JPEG_GetStreamLen(&uJpgSize);
	UART_Printf(" Encoded Stream = %d bytes\n", uJpgSize);
	
}

static void TestOneStepDecoding(void)
{

	u32 temp;
	u32 i;
	
	bIsOneStep = true;

	if (bIsJpgLoaded == false)
	{
		UART_Printf("ERROR! Load a raw image first!!\n");
		return;
	}

	INTC_Init();
	INTC_SetVectAddr(NUM_JPEG, Isr_Jpeg);
	INTC_Enable(NUM_JPEG);

	UART_Printf("timer start~\n");
	StartTimer(0);

	uRawImgAddr = uJpgAddr+((uJpgSize+16)/16)*16;

	for (i = 0; i<(((uJpgSize+16)/16)*16); i++)
	{
		*(u8 *)(uRawImgAddr+i) = 0;
	}
	 
	
	bJpegDone = false;
	JPEG_StartDecodingOneFrame(uJpgAddr, uRawImgAddr, false);
	WaitForSet(bJpegDone);
	//Assert(eJpgIrq == OK_ENC_OR_DEC);
//	JPEG_Wait_Done();

	temp = StopTimer(0);
	UART_Printf("JPEG File H/W Time = %dus\n",temp);
	
	JPEG_GetWidthAndHeight(&uRawImgHSz_Dec, &uRawImgVSz_Dec);
	if ( uRawImgHSz_Dec == 0 || uRawImgVSz_Dec == 0 )
		UART_Printf("Error! Widht or Height is zero\n");
	else 
		UART_Printf(" Hsz = %d, Vsz = %d\n", uRawImgHSz_Dec, uRawImgVSz_Dec);

	UART_Printf("timer start~\n");



	for (i = 0; i<(uJpgSize*8+16); i++)
	{
		*(u8 *)(uLcdFbAddr+i) = 0;
	}
	

//	UART_Getc();

	StartTimer(0);
	POST_InitIp(	uRawImgHSz_Dec, uRawImgVSz_Dec, uRawImgAddr, YCBYCR,
				uLcdHSz, uLcdVSz, uLcdFbAddr, eLcdBpp, 1, false, ONE_SHOT, &oPost);

	POST_StartProcessing(&oPost);
	while (!POST_IsFreeRunDone(&oPost));

	temp = StopTimer(0);
	UART_Printf("JPEG File Resize Time = %dus\n",temp);

	uIntCnt = 0;
	bJpegDone = false;
	
}

static void TestTwoStepDecoding(void)
{
	char pType[30];
	u32 temp;

	bIsOneStep = false;	
	
	if (bIsJpgLoaded == false)
	{
		UART_Printf("ERROR! Load a raw image first !!\n");
		return;
	}

	INTC_Init();
	INTC_SetVectAddr(NUM_JPEG, Isr_Jpeg);
	INTC_SetVectAddr(NUM_CAMIF_C, Isr_Camif_C);
	INTC_Enable(NUM_JPEG);
	INTC_Enable(NUM_CAMIF_C);

	UART_Printf("timer start~\n");
	StartTimer(0);

	// Header Parsing
	bJpegDone = false;
	JPEG_StartParsingHeader(uJpgAddr);
	WaitForSet(bJpegDone);
	
	Assert(eJpgIrq == OK_HD_PARSING);

	JPEG_GetJpegType(pType);
	//UART_Printf(" 1. Sub-Sampling Mode = %s\n", pType);

	JPEG_GetWidthAndHeight(&uRawImgHSz_Dec, &uRawImgVSz_Dec);
	
	if ( uRawImgHSz_Dec == 0 || uRawImgVSz_Dec == 0 )
		UART_Printf("Error! Width or Height is zero\n");
	else
		//UART_Printf(" 2. Resolution : Width = %d, Height = %d\n", uRawImgHSz_Dec, uRawImgVSz_Dec);

	// Decoding body.
	uRawImgAddr = uJpgAddr+((uJpgSize+16)/16)*16;
	//UART_Printf(" uRawImgAddr = 0x%x\n",uRawImgAddr);
	bJpegDone = false;
	JPEG_StartDecodingBody(uRawImgAddr, false);
	WaitForSet(bJpegDone);
	Assert(eJpgIrq == OK_ENC_OR_DEC);



	temp = StopTimer(0);
	UART_Printf("JPEG File Decoding Time = %dus\n",temp);
	
	UART_Printf("After JpegDone\n");
	UART_Printf("timer start~\n");
	StartTimer(0);

	POST_InitIp(	uRawImgHSz_Dec, uRawImgVSz_Dec, uRawImgAddr, YCBYCR,
				uLcdHSz, uLcdVSz, uLcdFbAddr, eLcdBpp, 1, false, ONE_SHOT, &oPost);
	POST_StartProcessing(&oPost);
	while (!POST_IsFreeRunDone(&oPost));

	temp = StopTimer(0);
	UART_Printf("JPEG File Decoding resize Time = %dus\n",temp);

}


static void TestDscSubsys(void)
{
	char cX=0;
	u32 uSrcHsz ,uSrcVsz ;
	u32 uCaptureHsz, uCaptureVsz;
	u32 uCodecAddr = uJpgAddr + 0x500000;
	u32 uBufCnt;
	u32 uCapNum;
	
	CSPACE eDstDataFmt = RGB16;
	IMG_SIZE eCodecSz = SVGA;

	// 1. Initialize LCD
	//========================================
	LCD_InitLDI(MAIN);
	LCD_InitDISPC(eDstDataFmt, uLcdFbAddr, WIN0, false);
	LCD_GetFrmSz(&uLcdHSz, &uLcdVSz, WIN0);
	LCD_Start();

	// 2. Initialize Interrupt
	//========================================
	INTC_Init();
	INTC_SetVectAddr(NUM_JPEG, Isr_Jpeg);
	INTC_SetVectAddr(NUM_CAMIF_C, Isr_Camif_C);	
//	INTC_SetVectAddr(NUM_CAMIF_P, Isr_Camif_P);	
	INTC_Enable(NUM_JPEG);
	INTC_Enable(NUM_CAMIF_C);
//	INTC_Enable(NUM_CAMIF_P);	

	// 3. Initialize camera and run the  process of preview path.
	//========================================
	CAMERA_InitSensor();		//CAMERA_InitSensor1(ePreviewSz, CCIR601, YCBYCR);
	//CAMERA_SetSensorSize(eCodecSz);
	CAMERA_GetSrcImgSz(&uSrcHsz, &uSrcVsz);
	
	uSrcHsz = (uSrcHsz > uLcdHSz) ? uLcdHSz : uSrcHsz;
	uSrcVsz = (uSrcVsz > uLcdVSz) ? uLcdVSz : uSrcVsz;

	CAMERA_InitCodecPath(uSrcHsz, uSrcVsz, uLcdFbAddr, eLcdBpp, FLIP_NO, ROT_0);
	CAMERA_StartCodecPath(0);

//	CAMERA_InitPreviewPath(uSrcHsz, uSrcVsz, uLcdFbAddr, eLcdBpp, FLIP_NO, ROT_0);
//	CAMERA_StartPreviewPath();

	while(1)
	{
		UART_Printf("x = Turn off Camera, s = Capture IMG(Enocoded To JPEG), c = Change IMG Size : ");
		cX = UART_Getc();
		
		UART_Printf("%c\n", cX);

		if(cX == 'x' || cX=='X')
		{
//			CAMERA_StopCodecPath();
//			CAMERA_StopPreviewPath();
			break;
		}
		else if(cX=='s' || cX=='S')
		{
			
			CAMERA_StopCodecPath();
//			CAMERA_StopPreviewPath();

			INTC_Enable(NUM_CAMIF_C);

			CAMERA_ResetSensor(); 
			Delay(5000);
			CAMERA_SetSensorSize(eCodecSz);
			bCamifDone = false;

			ConvertImgSzToNumber(eCodecSz, &uCaptureHsz, &uCaptureVsz);
			eDstDataFmt = YCBYCR;
			CAMERA_InitCodecPath(uCaptureHsz, uCaptureVsz, uCodecAddr,  eDstDataFmt, FLIP_NO, ROT_0);
			uBufCnt=0;
			uCapNum = 10;
			CAMERA_StartCodecPath(uCapNum);
			while(1)
			{	
				while(!bCamifDone);
				bCamifDone = false;
				uBufCnt++;
				if (uBufCnt==uCapNum)
					break;
			}
			CAMERA_StopCodecPath();
			INTC_Disable(NUM_CAMIF_C);
			
			
//			while(!CAMERA_IsProcessingDone());
			//WaitForConversionDone(bCamifDone);			

//			CAMERA_ClearFrameEndStatus();

			UART_Printf("Capturing raw image  ... \n");				
			bJpegDone = false;
			bIsOneStep = false;			
			JPEG_StartEncodingOneFrame(uCaptureHsz, uCaptureVsz, uCodecAddr, eDstDataFmt, uJpgAddr, eJpgFormat);
			WaitForSet(bJpegDone);
			Assert(eJpgIrq == OK_ENC_OR_DEC);
			//JPEG_Wait_Done();

			JPEG_GetStreamLen(&uJpgSize);

#if	 SEMIHOSTING
			UART_Printf("Saving image to file ... \n");
			SaveToFile(CAPTURED_JPEG, uJpgSize, uJpgAddr);
#else
			// Under Construction. SD/MMC will be used.
#endif

			CAMERA_InitCodecPath(uSrcHsz, uSrcVsz, uLcdFbAddr, eLcdBpp, FLIP_NO, ROT_0);
			CAMERA_StartCodecPath(0);


		}
		
		else if(cX=='c' || cX=='C')
		{
			UART_Printf("1 = UXGA, 2 = SXGA, 3 = SVGA, 4 = VGA : ");
			//x = Getc();
//			x = UART_Getc();
			cX = UART_GetIntNum();

//			UART_Printf("%d\n", cX);

			switch (cX)
			{
				case 1: eCodecSz = UXGA; break;
				case 2: eCodecSz = SXGA; break;				
				case 3: eCodecSz = SVGA; break;				
				case 4: eCodecSz = VGA; break;				
				default : eCodecSz = SVGA;

			}
			
//			eCodecSz =  (x=='11') ? UXGA : (x=='22') ? SXGA : (x=='33') ? SVGA : (x=='44') ? VGA : SVGA;
		}
		else
			UART_Printf("Invalid input! Retry!!\n");
	}
	CAMERA_StopCodecPath();
//	CAMERA_StopPreviewPath();

	bIsJpgLoaded = true;

}

static void SaveRawImgToFile(void)
{
#if	 SEMIHOSTING
	SaveToFile(YUV_FILE_NAME, uRawImgHSz_Dec*uRawImgVSz_Dec*2, uRawImgAddr);
#else
	// Under Construction. SD will be used.
#endif
}

static void SaveJpegStreamToFile(void)
{

#if	 SEMIHOSTING
	SaveToFile(SAVE_JPEG, uJpgSize, uJpgAddr);
#else
	// Under Construction. SD will be used.
#endif

}

void TestJpegPerformance(void)
{
	//char pType[30];
	u32 temp;

	bIsOneStep = false;	
	
	if (bIsJpgLoaded == false)
	{
		UART_Printf("ERROR! Load a raw image first !!\n");
		return;
	}

	INTC_Init();
	INTC_SetVectAddr(NUM_JPEG, Isr_Jpeg);
	INTC_SetVectAddr(NUM_CAMIF_C, Isr_Camif_C);
	INTC_Enable(NUM_JPEG);
	INTC_Enable(NUM_CAMIF_C);

	StartTimer(0);
	// Header Parsing
	bJpegDone = false;
	JPEG_StartParsingHeader(uJpgAddr);
	WaitForSet(bJpegDone);
	
	Assert(eJpgIrq == OK_HD_PARSING);

	//JPEG_GetJpegType(pType);
	//UART_Printf(" 1. Sub-Sampling Mode = %s\n", pType);

	JPEG_GetWidthAndHeight(&uRawImgHSz_Dec, &uRawImgVSz_Dec);
	
	if ( uRawImgHSz_Dec == 0 || uRawImgVSz_Dec == 0 )
		UART_Printf("Error! Width or Height is zero\n");
	else
		//UART_Printf(" 2. Resolution : Width = %d, Height = %d\n", uRawImgHSz_Dec, uRawImgVSz_Dec);

	// Decoding body.
	uRawImgAddr = uJpgAddr+((uJpgSize+16)/16)*16;
	//UART_Printf(" uRawImgAddr = 0x%x\n",uRawImgAddr);
	bJpegDone = false;
	JPEG_StartDecodingBody(uRawImgAddr, false);
	WaitForSet(bJpegDone);
	Assert(eJpgIrq == OK_ENC_OR_DEC);

	temp = StopTimer(0);
	UART_Printf("UXGA(1600*1200) JPEG File Decoding Time = %dus\n",temp);
	
	POST_InitIp(
		uRawImgHSz_Dec, uRawImgVSz_Dec, uRawImgAddr, YCBYCR,
		uLcdHSz, uLcdVSz, uLcdFbAddr, eLcdBpp, 1, false, ONE_SHOT, &oPost);
	POST_StartProcessing(&oPost);
	while (!POST_IsFreeRunDone(&oPost));

}

/*
u8 JPEG_AutoTest(void)
{
	char pType[30];
	char pFileName[50];
	
	int sel;
	u32 uTotalNumOfFiles, i;
	u32 compare_size;

	bIsOneStep = false;	

	if (!SDHC_OpenMedia(SDHC_HCLK, &oSdhc) || !FAT_LoadFileSystem(&oSdhc))
		Assert(0);
	
	FAT_GetTotalNumOfFiles(&uTotalNumOfFiles, &oSdhc);
	for (i=0; i<uTotalNumOfFiles; i++)
	{
		FAT_GetFileName(i, pFileName, &oSdhc);
	}
	sel = 0;
	if (!FAT_ReadFile2(sel, uJpgAddr, &uJpgSize, &oSdhc))
		UART_Printf(" Loading failed ....\n");

	FAT_UnloadFileSystem(&oSdhc);
	SDHC_CloseMedia(&oSdhc);

	UART_Printf(" Stream size = %d bytes\n", uJpgSize);
	compare_size = uJpgSize;
	bIsJpgLoaded = true;
	
	if (bIsJpgLoaded == false)
	{
		UART_Printf("ERROR! Load a raw image first !!\n");
		return;
	}

	INTC_Init();
	INTC_SetVectAddr(NUM_JPEG, Isr_Jpeg);
	INTC_Enable(NUM_JPEG);

	// Header Parsing
	bJpegDone = false;
	JPEG_StartParsingHeader(uJpgAddr);
	WaitForSet(bJpegDone);
	
	Assert(eJpgIrq == OK_HD_PARSING);

	JPEG_GetJpegType(pType);
	UART_Printf(" 1. Sub-Sampling Mode = %s\n", pType);

	JPEG_GetWidthAndHeight(&uRawImgHSz_Dec, &uRawImgVSz_Dec);
	
	if ( uRawImgHSz_Dec == 0 || uRawImgVSz_Dec == 0 )
		UART_Printf("Error! Width or Height is zero\n");
	else
		UART_Printf(" 2. Resolution : Width = %d, Height = %d\n", uRawImgHSz_Dec, uRawImgVSz_Dec);

	// Decoding body.
	uRawImgAddr = uJpgAddr+((uJpgSize+16)/16)*16;
	bJpegDone = false;
	JPEG_StartDecodingBody(uRawImgAddr, false);
	WaitForSet(bJpegDone);
	Assert(eJpgIrq == OK_ENC_OR_DEC);
	
	UART_Printf("JPEG Decoding Test Pass!!!\n");

	//UART_Printf("Encoding Start!!!\n");

	////////////////////////////////////////////////////////////
	
	bIsOneStep = false;	

	eJpgFormat = JPEG_422;

	uRawImgHsz_Enc = 1600;
	uRawImgVsz_Enc = 1200;
	
	Assert(eRawImgFormat == YCBYCR || eRawImgFormat == RGB16);

	bJpegDone = false;
	JPEG_StartEncodingOneFrame(uRawImgHsz_Enc, uRawImgVsz_Enc, uRawImgAddr, YCBYCR,
						  		 uJpgAddr, eJpgFormat);
	WaitForSet(bJpegDone);

	Assert(eJpgIrq == OK_ENC_OR_DEC);

	JPEG_GetStreamLen(&uJpgSize);

	if(compare_size == uJpgSize)
	{
		UART_Printf("JPEG Encoding Test Pass!!!\n");
		return true;
	}	
	else
	{
		UART_Printf("Jpeg Encoding Test Fail!!!!\n original size = %d bytes, encoding size = %d bytes\n", compare_size, uJpgSize);
		return false;
	}	

}
*/



u8 JPEG_AutoTest(void)
{
	char pType[30];
//	char pFileName[50];
	const char *pFileName = CAPTURED_JPEG;
	
	int sel, temp;
	u32 uTotalNumOfFiles, i;
	u32 compare_size;

	SDHC_InitCh(SDHC_CHANNEL_0, &oSdhc);

      //---------------  JPEG Basic Setting ----------------------------------------//
	temp = Inp32(0x7e00f020);					// 0x7e00f020 = CLK_DIV0
	Outp32(0x7e00f020, temp |(0x3<<24));			// JPEG_RATIO setting , 4 dividing (266/4 =66.5MHz)

	JPEG_Init();
	eJpgFormat = JPEG_422;
	eLcdBpp = RGB16;


	uLcdFbAddr = (u32)malloc(15000);
	uGlibAddr = (u32)malloc(15000);

//	uLcdFbAddr = CODEC_MEM_ST+0x3000000;
//	uGlibAddr = uLcdFbAddr + 240*320*4;

	
	uJpgAddr = CODEC_MEM_ST;

	eRawImgFormat = RGB16;

	uRawImgAddr = uJpgAddr + 0x1000000;

	uRawImgHsz_Enc = 320;
	uRawImgVsz_Enc = 240;

	bIsJpgLoaded = false;
	bIsOneStep = false;
	uIntCnt = 0;
//------------------------------------------------------------------------------------------//
#if	 SEMIHOSTING

	UART_Printf(" Loading %s from PC ...\n", pFileName);
	LoadFromFile1(pFileName, uJpgAddr, &uJpgSize);

	UART_Printf(" Stream size = %d bytes\n", uJpgSize);
	compare_size = uJpgSize;
	bIsJpgLoaded = true;
#else
	UART_Printf("Check Semihosting....\n");
#endif
//------------------------------------------------------------------------------------------//
	
#if !SEMIHOSTING	

	if (!SDHC_OpenMediaWithMode(4, SDHC_POLLING_MODE, SDHC_HCLK, 4, 1, &oSdhc) || !FAT_LoadFileSystem(&oSdhc))
		Assert(0);

	FAT_GetFileIndex("auto_test_image.jpg", &sel, &oSdhc);
		
	if (!FAT_ReadFile2(sel, uJpgAddr, &uJpgSize, &oSdhc))
		UART_Printf(" Loading failed ....\n");

	FAT_UnloadFileSystem(&oSdhc);
	SDHC_CloseMedia(&oSdhc);

	UART_Printf(" Stream size = %d bytes\n", uJpgSize);
	compare_size = uJpgSize;
	bIsJpgLoaded = true;
#endif

	if (bIsJpgLoaded == false)
	{
		UART_Printf("ERROR! Load a raw image first !!\n");
		return;
	}

	INTC_Init();
	INTC_SetVectAddr(NUM_JPEG, Isr_Jpeg);
	INTC_Enable(NUM_JPEG);

	// Header Parsing
	bJpegDone = false;
	JPEG_StartParsingHeader(uJpgAddr);
	WaitForSet(bJpegDone);
	
	Assert(eJpgIrq == OK_HD_PARSING);

	JPEG_GetJpegType(pType);
	UART_Printf(" 1. Sub-Sampling Mode = %s\n", pType);

	JPEG_GetWidthAndHeight(&uRawImgHSz_Dec, &uRawImgVSz_Dec);
	
	if ( uRawImgHSz_Dec == 0 || uRawImgVSz_Dec == 0 )
		UART_Printf("Error! Width or Height is zero\n");
	else
		UART_Printf(" 2. Resolution : Width = %d, Height = %d\n", uRawImgHSz_Dec, uRawImgVSz_Dec);

	// Decoding body.
	uRawImgAddr = uJpgAddr+((uJpgSize+16)/16)*16;
	bJpegDone = false;
	JPEG_StartDecodingBody(uRawImgAddr, false);
	WaitForSet(bJpegDone);
	Assert(eJpgIrq == OK_ENC_OR_DEC);
	
	UART_Printf("JPEG Decoding Test Pass!!!\n");

	//UART_Printf("Encoding Start!!!\n");

	////////////////////////////////////////////////////////////
	
	bIsOneStep = false;	

	eJpgFormat = JPEG_422;

	uRawImgHsz_Enc = 1600;
	uRawImgVsz_Enc = 1200;
	
	Assert(eRawImgFormat == YCBYCR || eRawImgFormat == RGB16);

	bJpegDone = false;
	JPEG_StartEncodingOneFrame(uRawImgHsz_Enc, uRawImgVsz_Enc, uRawImgAddr, YCBYCR,
						  		 uJpgAddr, eJpgFormat);
	WaitForSet(bJpegDone);

	Assert(eJpgIrq == OK_ENC_OR_DEC);

	JPEG_GetStreamLen(&uJpgSize);

	if(compare_size == uJpgSize)
	{
		UART_Printf("JPEG Encoding Test Pass!!!\n");
		return true;
	}	
	else
	{
		UART_Printf("Jpeg Encoding Test Fail!!!!\n original size = %d bytes, encoding size = %d bytes\n", compare_size, uJpgSize);
		return false;
	}	

}




void JPEG_Test(void)
{
	int i, sel;
	u32 temp;

	const testFuncMenu menu[]=
	{
		0,                                  "Exit",
#ifdef SEMIHOSTING
		GetJpegStreamFromPc,             "Load Jpg File from PC",
#endif		
		GetJpegStreamFromSd,             "Load Jpg File from SD",
		TestEncoding,                      	  "Encoding Image test (Generated Image)",		
		TestOneStepDecoding,               "H/W Decoding test",
		TestTwoStepDecoding,               "Decoding test",
		TestDscSubsys,                      	    "Camera Capture test",
		SaveRawImgToFile,                    "Save Raw Image",
		SaveJpegStreamToFile,               "Save Jpg file to PC",
		TestJpegPerformance,			"JPG Performance Test",
		JPEG_AutoTest,					"Auto Test (JPEG)\n",
		0,0
	};

	//--------------- Basic Peripheral Setting for JPEG Op. --------------------------//
	// JPEG Codec CLK HCLK*2/4	CAMERA_SFRInit();
	CAMERA_SFRInit();
	CAMERA_SetPort();
   	CAMERA_ClkSetting(); 						// Camera Module CLK Setting

	SDHC_InitCh(SDHC_CHANNEL_0, &oSdhc);
	POST_InitCh(POST_A, &oPost);


      //---------------  JPEG Basic Setting ----------------------------------------//
	temp = Inp32(0x7e00f020);					// 0x7e00f020 = CLK_DIV0
	Outp32(0x7e00f020, temp |(0x3<<24));			// JPEG_RATIO setting , 4 dividing (266/4 =66.5MHz)

	JPEG_Init();
	eJpgFormat = JPEG_422;
	eLcdBpp = RGB16;

	//uLcdFbAddr = CODEC_MEM_ST+0x1000000;


	uLcdFbAddr = (u32)malloc(15000);
	uGlibAddr = (u32)malloc(15000);
	
//	uLcdFbAddr = CODEC_MEM_ST+0x3000000;
//	uGlibAddr = uLcdFbAddr + 240*320*4;
	uJpgAddr = CODEC_MEM_ST;

	eRawImgFormat = RGB16;
	//uRawImgAddr = uJpgAddr + 0x500000;
	uRawImgAddr = uJpgAddr + 0x1000000;

	uRawImgHsz_Enc = 320;
	uRawImgVsz_Enc = 240;

	bIsJpgLoaded = false;
	bIsOneStep = false;
	uIntCnt = 0;

	LCD_SetPort();
	LCD_InitLDI(MAIN);
	LCD_InitDISPC(eLcdBpp, uLcdFbAddr, WIN0, false);
	LCD_SetWinOnOff(1, WIN0);
	LCD_Start();
	LCD_GetFrmSz(&uLcdHSz, &uLcdVSz, WIN0);


	while(1)
	{
		UART_Printf("\n");
		for (i=0; (int)(menu[i].desc)!=0; i++) {
			UART_Printf("%2d: %s\n", i, menu[i].desc);
		}

		UART_Printf("\nSelect the function to test : ");
		sel = UART_GetIntNum();
		UART_Printf("\n");

		if (sel == 0)
			break;
		else if (sel>0 && sel<(sizeof(menu)/8-1))
			(menu[sel].func) ();
	}
}
