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
*	File Name : mfc_test.c
*  
*	File Description : This file implements mfc test functions.
*
*	Author : Haksoo,Kim
*	Dept. : AP Development Team
*	Created Date : 2007/01/xx
*	Version : 0.1 
* 
*	History
*	- Created(Haksoo,Kim 2007/01/xx)
*  
**************************************************************************************/
 
#include "option.h"
#include "library.h"
#include "mfc.h"
#include "sdhc.h"
#include "lcd.h"
#include "post.h"
#include "glib.h"
#include "timer.h"
#include "fat.h"
#include "intc.h"
#include "sysc.h"
#include "vsparser.h"

#include <string.h>
#include <stdlib.h>

#define MFC_MEMORY_BASE		(_DRAM_BaseAddress+0x01000000)
#define	STREAM_BUF_SIZE		(720*576*3)	// >= 720*576*3 (worst case)
#define	DMB_STREAM_BUF_SIZE	(64*1024)	// for DMB decoding

#define USE_SD_STORAGE		TRUE
#define MULTI_DEC_ONE_ENC	TRUE
#define MFC_CLOCK_CHANGE	FALSE

static volatile u8 bMfcDone;
static volatile u8 bPostDone;
static volatile u8 bStreamBufEmpty;

static u8 bH264ReorderEn;

static u8 bDecRotModeEn;
static u32 uDecRotDeg;
static u8 bDecMirHor;
static u8 bDecMirVer;

static MFC_CODEC_MODE eEncMode;
static u8 bIsH263;

static char pSrcYUVFileName[256];

static u32 uEncWidth;
static u32 uEncHeight;
static u32 uEncTotalFrameNum;
#if (USE_SD_STORAGE && MULTI_DEC_ONE_ENC)
static u32 uEncFrameRate;
static u8 bEncSliceMode;
static u8 bEncSliceSizeMode;
static u32 uEncSliceSizeNum;
static u32 uEncIntraRefreshNum;
static u32 uEncGopNum;
static u8 bEncRcEnable;
static u32 uEncRcBitRate;
static u32 uEncRcInitDelay;
static u32 uEncRcBufSize;
static u32 uEncPicQs;

static int uEncChromaQpOffset;
static u32 uEncDeblkMode;
static int uEncDeblkAOffset;
static int uEncDeblkBOffset;

static u8 bEncAnnexJ;
static u8 bEncAnnexK;
static u8 bEncAnnexT;

static u8 bEncDataPartEn;
static u8 bEncRevVlcEn;
static u32 uEncIntraDcVlcThr;
#endif	//#if (MULTI_DEC_ONE_ENC)

static u8 bAllRotating=false;
static u8 bAllRotateFirst=false;

#if (USE_SD_STORAGE)
static SDHC oSdhc;
#endif
static POST oPost;

//////////
// Function Name : Isr_Mfc
// Function Description : MFC ISR
// Input : NONE
// Output : NONE
// Version : 
static void __irq Isr_Mfc(void)
{
	MFC_INT_FLAG eFlag;

	MFC_GetIntFlag(&eFlag);				
	if (eFlag == INT_MFC_BIT_BUF_EMPTY)
	{
		bStreamBufEmpty = true;
		UART_Printf(" empty ");
	}
	MFC_ClearPending(eFlag);
	bMfcDone = true;
	INTC_ClearVectAddr();
}

//////////
// Function Name : Isr_Post
// Function Description : Post Processor ISR defined for MFC test
// Input : NONE
// Output : NONE
// Version : 
static void __irq Isr_Post(void)
{
	POST_ClearPending(&oPost);
	
	bPostDone = true;
	INTC_ClearVectAddr();
}

//////////
// Function Name : SetRotationMode
// Function Description : This function sets rotation mode of decoder
// Input : NONE
// Output : NONE
// Version : 
static void SetRotationMode(void)
{
	int sel;
	
	UART_Printf("MFC Rotation mode ( [1]enable, [2]disable ) : ");
	sel = UART_GetIntNum();
	bDecRotModeEn = (sel == 1) ? true : false;

	if (bDecRotModeEn == true) // rot enable
	{
		UART_Printf("Select Rot degree ([0]0, [1]90, [2]180, [3]270) :");
		sel = UART_GetIntNum();
		Assert(sel >= 0 && sel < 4);
		uDecRotDeg = sel*90;

		UART_Printf("Select Mirror mode ([0]No Mirroring, [1]Mirroring Horizontally [2]Mirroring Vertically [3]Mirroring in both way) :");
		sel = UART_GetIntNum();
		Assert(sel >= 0 && sel < 4);
		bDecMirHor = (sel == 1 || sel == 3) ? true : false;
		bDecMirVer = (sel == 2 || sel == 3) ? true : false;
	}
	else
	{
		uDecRotDeg = 0;
		bDecMirHor = false;
		bDecMirVer = false;		
	}
}

//////////
// Function Name : TestDecoding
// Function Description : This function decodes an stream file and displays the generated YUV
// Input : NONE
// Output : NONE
// Version :
static void TestDecoding(void)
{
	u32 uFreeMemBuf = MFC_MEMORY_BASE;
#if	(USE_SD_STORAGE)
	char pFileName[256];
	u32 uTotalNumOfFiles;
	char fileExt[50];
	static int sel, i;
	const int uBlkSz = 512;
	u8 bFat_FileLoad=false;
#else
	u32 uStreamFileAddr;	
#endif
	const int nReadUnit = STREAM_BUF_SIZE/2;
	static u32 uStreamFileSize;
	u32 uStreamBufStAddr;
	int nReadSize;
	u32 uStreamOffset;
	u32 uFrameBufStAddr;
	u32 uProcessIdx;
	u32 uLcdFbAddr;
	CSPACE eLcdBpp;
	u32 uPicHsz, uPicVsz;
	u32 uDispFrameNum;
	u32 uRotWidth;
	u32 uRotHeight;
	u32 uLcdWidth, uLcdHeight;
	u32 uDecWidth, uDecHeight;
	float fDecTime=0,fPostTime=0,fOtherTime=0;
	double fTotTime,fTotDecTime;
	u32 uFrameIdx;
	u32 frameCount;
	static MFC_CODEC_MODE eDecMode;
	float frameRate;
	u32 picX, picY;
	u8 bRealTimePlay=false;

#if	(!USE_SD_STORAGE)
	// 0. Download the stream file thru USB.
	//-------------------------------------------
	uStreamFileAddr = uFreeMemBuf;
	if((bAllRotating==false)||(bAllRotateFirst==true))
	{
		UART_Printf(" Before testing, download a stream file through USB\n");
		UART_Printf(" If you want to test with previous stream file, press 'x' key\n");
		if(Getc()!='x')	
			uStreamFileSize = DownloadImageThruUsbOtg((u8 *)uStreamFileAddr);
	}
	if (uStreamFileSize == 0)
		return;
	uFreeMemBuf += uStreamFileSize;
	uFreeMemBuf = (uFreeMemBuf + STREAM_WR_SIZE-1)/STREAM_WR_SIZE*STREAM_WR_SIZE;
#endif

	// 1. Move firmware and init base settings.
	//-------------------------------------------
	
	MFC_MoveFirmwareToCodeBuf(uFreeMemBuf);
	MFC_GetCodeBufEndAddr(&uFreeMemBuf);
	MFC_StartBitProcessor();

	MFC_InitBaseForProcesses(uFreeMemBuf);
	MFC_GetBaseBufEndAddr(&uFreeMemBuf);

	// 2. Reserve the stream buffer, & load the first part of a stream
	//-----------------------------------------------------------------

#if USE_SD_STORAGE
	while(!SDHC_OpenMedia(SDHC_HCLK, &oSdhc));
	bFat_FileLoad=FAT_LoadFileSystem(&oSdhc);
	if (!bFat_FileLoad)
		Assert(0);

	if((bAllRotating==false)||(bAllRotateFirst==true))
	{
		if(bFat_FileLoad==false)	//to avoid duplicate display
		{
			FAT_GetTotalNumOfFiles(&uTotalNumOfFiles, &oSdhc);
			for (i=0; i<uTotalNumOfFiles; i++)
			{
				FAT_GetFileName(i, pFileName, &oSdhc);
				UART_Printf("%02d -- %s\n", i, pFileName);
			}
		}

		UART_Printf(" Which file to read ? ");
		sel = UART_GetIntNum();
	}

	Assert(nReadUnit%uBlkSz == 0);
	
	FAT_GetFileSize(sel, (s32 *)&uStreamFileSize, &oSdhc);

	uStreamBufStAddr = uFreeMemBuf;
	uFreeMemBuf += STREAM_BUF_SIZE; // Reserved for stream buf.	
	Assert(uStreamBufStAddr%STREAM_WR_SIZE == 0);
	
	nReadSize = (uStreamFileSize < nReadUnit) ? uStreamFileSize : nReadUnit;
	uStreamOffset = nReadSize;
	if (!FAT_ReadFile4(sel, 0, (nReadSize+uBlkSz-1)/uBlkSz, uStreamBufStAddr, &oSdhc))
		Assert(0);
	
#else
	uStreamBufStAddr = uFreeMemBuf;
	uFreeMemBuf += STREAM_BUF_SIZE; // Reserved for stream buf.
	Assert(uStreamBufStAddr%STREAM_WR_SIZE == 0);
	
	nReadSize = (uStreamFileSize < nReadUnit) ? uStreamFileSize : nReadUnit;
	uStreamOffset = nReadSize;
	memcpy((void *)uStreamBufStAddr, (void *)uStreamFileAddr, nReadSize);
#endif

	// 3. Init a process of MFC
	//--------------------------
#if USE_SD_STORAGE
	FAT_GetFileExt(sel, fileExt, &oSdhc);		
	MFC_GetCodecModeByName(fileExt, true, &eDecMode);
#else
	if((bAllRotating==false)||(bAllRotateFirst==true))
	{
		UART_Printf("\nSelect Decoder mode: \n");
		UART_Printf("[0] MPEG4/H.263\n");
		UART_Printf("[1] H.264\n");
		UART_Printf("[2] VC-1\n");
		eDecMode = (MFC_CODEC_MODE)UART_GetIntNum();
		if(eDecMode>2)
			return;
		eDecMode = (eDecMode==0) ? MP4_DEC : (eDecMode==1) ? AVC_DEC : VC1_DEC ;
	}
#endif

	if((bAllRotating==false)&&(eDecMode != AVC_DEC))
	{
		UART_Printf("\n If you want to play the recon image in real time mode, \n");
		UART_Printf("  press 'y' key [Default : non-real time mode]\n");
			
		if(Getc()=='y')
			bRealTimePlay = true;
		Putc('\n');
	}
	
	uProcessIdx=0;

	bH264ReorderEn = true;
	uFrameBufStAddr = uFreeMemBuf;
	
	if (eDecMode == MP4_DEC)
		MFC_InitProcessForDecodingMpeg4(uProcessIdx, uStreamBufStAddr, STREAM_BUF_SIZE, 			
			uFrameBufStAddr, bDecRotModeEn, true);
	else if (eDecMode == AVC_DEC)
		MFC_InitProcessForDecodingH264(uProcessIdx, uStreamBufStAddr, STREAM_BUF_SIZE, 
			uFrameBufStAddr, bDecRotModeEn, bH264ReorderEn);
	else if (eDecMode == VC1_DEC)
		MFC_InitProcessForDecodingVc1(uProcessIdx, uStreamBufStAddr, STREAM_BUF_SIZE, 
			uFrameBufStAddr, bDecRotModeEn);
	else
		Assert(0);

	MFC_GetProcessBufEndAddr(uProcessIdx, &uFreeMemBuf);
	MFC_SetWrPtr(uProcessIdx, uStreamBufStAddr+nReadSize);
	INTC_SetVectAddr(NUM_MFC,Isr_Mfc);
	INTC_Enable(NUM_MFC);
	
	
	// 4. Init LCDC
	//---------------

	uLcdFbAddr = uFreeMemBuf;
	eLcdBpp = RGB24;	
	LCD_InitDISPC(eLcdBpp, uLcdFbAddr, WIN0, false);
	LCD_GetFbEndAddr(&uFreeMemBuf, WIN0); // Get the free mem buffer addr
	
	LCD_GetFrmSz(&uLcdWidth, &uLcdHeight, WIN0);
	uDecWidth = uLcdWidth;
	uDecHeight = uLcdHeight*9/10;

	GLIB_InitInstance(uLcdFbAddr, uLcdWidth, uLcdHeight, eLcdBpp);
	GLIB_ClearFrame(C_BLUE);
	GLIB_SetFontColor(C_WHITE, C_BLUE, false);
	GLIB_Printf(2, uDecHeight+6, "S3C6410 MFC Decoder");

	
	// 5. Init POST
	//---------------
	MFC_GetSrcPicSize(uProcessIdx, &uPicHsz, &uPicVsz);

	uRotWidth = (uDecRotDeg%180 == 0) ? uPicHsz : uPicVsz;
	uRotHeight = (uDecRotDeg%180 == 0) ? uPicVsz : uPicHsz;
	
	INTC_SetVectAddr(NUM_POST0, Isr_Post);
	INTC_Enable(NUM_POST0);	
	POST_EnableInterrupt(POST_LEVEL_INT, &oPost);
	
	fTotTime=0;
	fTotDecTime=0;
	frameCount=0;
	MFC_GetDecSrcFormat(&picX, &picY, &frameRate);
	
	LCD_Start();
		
	while (GetKey() == 0) // loop until any key is pressed.
	{
		// 6. Decode one frame
		//---------------------

		StartTimer(0);	

		bStreamBufEmpty = false;	
		bMfcDone = false;
		MFC_SetDecRotEn(bDecRotModeEn);
		if (bDecRotModeEn)
			MFC_SetDecRotationMode(bDecMirHor, bDecMirVer, uDecRotDeg);
		MFC_StartDecodingOneFrame(uProcessIdx);
		while(bMfcDone != true);
		
		fDecTime = StopTimer(0);	
		
		fTotTime += fDecTime;
		fTotDecTime += fDecTime;

		if (MFC_DoPostProcessingOfDecoding(uProcessIdx, bStreamBufEmpty, &uFrameIdx) == false)
		{
			while(bPostDone == false);
			break;
		}

		if((bRealTimePlay==true)&&((fPostTime!=0)||(fOtherTime!=0)))
		{
			s32 sDelay=1000000/frameRate-fDecTime-fPostTime-fOtherTime;
			
			if(sDelay>0)
				DelayfrTimer(micro, (u32)sDelay);

			fPostTime = 0;
			fOtherTime = 0;
		}

		// 7. Convert the frame for display
		//----------------------------------

		StartTimer(0);
		
		MFC_GetDispFrameNum(uProcessIdx, &uDispFrameNum);
		
		if (frameCount==0)
			bPostDone = true;
		while(bPostDone == false);
		bPostDone = false;
		
		POST_InitIp1(
			uRotWidth, uRotHeight, 0, 0, uRotWidth, uRotHeight, uFrameBufStAddr, YC420,
			uLcdWidth, uLcdHeight, 0, 0, uDecWidth, uDecHeight, uLcdFbAddr, eLcdBpp,
			uDispFrameNum, false, ONE_SHOT, POST_DMA, POST_DMA, &oPost);
		
		POST_StartProcessing1(uFrameIdx, 0, &oPost);

		LCD_Trigger();

		fPostTime = StopTimer(0);	
		
		fTotTime += fPostTime;
		
		frameCount++;

		// 8. Load stream if necessary
		//------------------------------

		StartTimer(0);

		if (uStreamFileSize == uStreamOffset)
		{					
			MFC_NotifyNoMoreStream(uProcessIdx);
		}
		else if (MFC_IsMoreStreamNeeded(uProcessIdx, nReadUnit))
		{
			if ((uStreamFileSize-uStreamOffset) < nReadUnit) 
				nReadSize = (uStreamFileSize-uStreamOffset);
			else
				nReadSize = nReadUnit;

#if USE_SD_STORAGE
			if (!FAT_ReadFile4(sel, uStreamOffset/uBlkSz, (nReadSize+uBlkSz-1)/uBlkSz, uStreamBufStAddr+uStreamOffset%STREAM_BUF_SIZE, &oSdhc))
				Assert(0);
#else
			memcpy((void *)(uStreamBufStAddr+uStreamOffset%STREAM_BUF_SIZE), (void *)(uStreamFileAddr+uStreamOffset), nReadSize);
#endif
			
			uStreamOffset += nReadSize;
			
			MFC_SetWrPtr(uProcessIdx, uStreamBufStAddr+uStreamOffset%STREAM_BUF_SIZE);
		}
		
		// 9. Display frame rate on the LCD
		//----------------------------------

		if((frameCount%10)==0)
		{
			GLIB_Printf(2, uDecHeight+6, "S3C6410 MFC Decoder(FPS=%.2f @%dx%d)",1000000*frameCount/fTotTime,picX,picY);
		}

		fOtherTime = StopTimer(0);
	}

#if USE_SD_STORAGE
	FAT_UnloadFileSystem(&oSdhc);
	SDHC_CloseMedia(&oSdhc);
#endif	

	UART_Printf("\ntotal decoding time=%.fus, pure FPS=%.2f\n",fTotDecTime,1000000*frameCount/fTotDecTime);
	UART_Printf("total decoding+post_processing time=%.fus\n",fTotTime);
	UART_Printf("FPS=%.2f, average elapsed time= %.2f(ms), total %d frames\n", 
		1000000*frameCount/fTotTime, fTotTime/frameCount/1000, frameCount);
	
	MFC_IssueSeqEndCmd(uProcessIdx);
	MFC_StopBitProcessor();
}

//////////
// Function Name : TestDecodingOneFrameByOneFrame
// Function Description : This function decodes an stream file in line buffer mode
// Input : NONE
// Output : NONE
// Version :
#if	(USE_SD_STORAGE)
#define MAX_FRAME_NUM 1500
static void TestDecodingOneFrameByOneFrame(void)
{
	u32 uFreeMemBuf = CODEC_MEM_ST;
	char pFileName[256];
	u32 uTotalNumOfFiles;
	int sel, i;
	const int uBlkSz = 512;
	const int nReadUnit = STREAM_BUF_SIZE/2;
	s32 uStreamFileSize;
	u32 uStreamBufStAddr;
	char fileExt[50];
	u32 uFrameBufStAddr;
	u32 uProcessIdx =0;
	u32 uLcdFbAddr;
	CSPACE eLcdBpp;
	u32 uPicHsz, uPicVsz;
	u32 uDispFrameNum;
	u32 uRotWidth;
	u32 uRotHeight;
	u32 uLcdWidth, uLcdHeight;
	u32 uDecWidth, uDecHeight;
	float fDecTime=0,fPostTime=0;
	double fTotTime,fTotDecTime;
	u32 picX, picY;
	u32 uFrameIdx;
	u32 frameCount;
	MFC_CODEC_MODE eDecMode;
	float frameRate;
	u32 uStreamBufForSD = CODEC_MEM_ST + 0x02000000;
	u32	uTemp32;
	u32	uTemp32_1 = 0;
	u32 uCopySize=1;
	u8 *pucSrcMem;
	u32 auFrameStartAddr[MAX_FRAME_NUM];
	u32 j;
	u32 uReadAmountByteSize=0; // Stream Buffer Read Amount.
	u8 bFilePlayEn;
	u8 bDynBufAllocEn;
	u8 bMp4DeblkEn;
	u32 uMyFrameIndex;
	u8 bIsFirstFrame;
	u8 bIsCnMSeq=false;
	u32 uNumOfH264IFrame;
	u32 uBufferByteSize;
	u8 bIsHeader;
	u32 uFrameStartAddr;
	u32 uFrameSize;
	u32 uNumOfFrames;
	u8 bIsMpeg4ShrotHeader;
	u8 bIsStreamCorrect = true;
	u8 bFat_FileLoad=false;

	// 1. Move firmware and init base settings.
	//--------------------------------------------------------------------------------
	
	MFC_MoveFirmwareToCodeBuf(uFreeMemBuf);
	MFC_GetCodeBufEndAddr(&uFreeMemBuf);
	MFC_StartBitProcessor();

	MFC_InitBaseForProcesses(uFreeMemBuf);
	MFC_GetBaseBufEndAddr(&uFreeMemBuf);

	// 2. Reserve the stream buffer, & load the first part of a stream
	//--------------------------------------------------------------------------------
	
	while(!SDHC_OpenMedia(SDHC_HCLK, &oSdhc));
	bFat_FileLoad=FAT_LoadFileSystem(&oSdhc);
	if (!bFat_FileLoad)
		Assert(0);

	if(bFat_FileLoad==false)	//to avoid duplicate display
	{
		FAT_GetTotalNumOfFiles(&uTotalNumOfFiles, &oSdhc);
		for (i=0; i<uTotalNumOfFiles; i++)
		{
			FAT_GetFileName(i, pFileName, &oSdhc);
			UART_Printf("%02d -- %s\n", i, pFileName);
		}
	}

	UART_Printf(" Which file to read ? ");
	sel = UART_GetIntNum();

	Assert(nReadUnit%uBlkSz == 0);
	

	FAT_GetFileSize(sel, &uStreamFileSize, &oSdhc);

	uStreamBufStAddr = uFreeMemBuf;
	uFreeMemBuf += STREAM_BUF_SIZE; // Reserved for stream buf.	
	Assert(uStreamBufStAddr%STREAM_WR_SIZE == 0);

	UART_Printf("Stream  Buffer Start Address: 0x%x \n", uStreamBufStAddr);

  	// 3.   Loading Streams into  uStreamBufForSD 
	//--------------------------------------------------------------------------------
	if (!FAT_ReadFile4(sel, 0, (uStreamFileSize+uBlkSz-1)/uBlkSz, uStreamBufForSD, &oSdhc)) //+yktak  Load a Stream from SDMMC into uStreamBufForSD
		Assert(0);

	FAT_GetFileExt(sel, fileExt, &oSdhc);		
	MFC_GetCodecModeByName(fileExt, true, &eDecMode);

	uProcessIdx=0;
	pucSrcMem = (u8 *) uStreamBufForSD;

	switch(eDecMode)
	{
		case VC1_DEC:

			bIsHeader = true;
			uBufferByteSize = 0x10000;
			auFrameStartAddr[0] = uStreamBufForSD;
			Vsparser(eDecMode, auFrameStartAddr[0] , uBufferByteSize,  bIsHeader, 
				&uFrameStartAddr, &uFrameSize, &uNumOfFrames,&bIsMpeg4ShrotHeader);		
			auFrameStartAddr[1] = uFrameStartAddr + uFrameSize;

			bIsHeader =false;
			Vsparser(eDecMode, auFrameStartAddr[1] , uBufferByteSize,  bIsHeader, 
			&uFrameStartAddr, &uFrameSize, &uNumOfFrames,&bIsMpeg4ShrotHeader);		
			auFrameStartAddr[2] = uFrameStartAddr + uFrameSize;
			uCopySize = auFrameStartAddr[2+uTemp32_1]- auFrameStartAddr[0];
			uReadAmountByteSize = uReadAmountByteSize +uCopySize;
			Copy8( auFrameStartAddr[0],uStreamBufStAddr,uCopySize);
			bIsCnMSeq = true;

			break;

		case AVC_DEC:

			//  H.264 stream file parsing: Begin
			//=============================================================================================
			// Structure of H.264 encoded stream (in case 1 frame is composed of single slice).
			// auFrameStartAddr[0] --- auFrameStartAddr[1]-1 	: SPS+PPS+SEI+ 
			// auFrameStartAddr[1] --- auFrameStartAddr[2]-1 	: first I frame
			// auFrameStartAddr[2] --- auFrameStartAddr[3]-1 	: next P-frame
			// ....
			auFrameStartAddr[0] = (u32) pucSrcMem;
			pucSrcMem++;
			uNumOfFrames =1;
			uMyFrameIndex =1;
			uNumOfH264IFrame =0;
			bIsFirstFrame = true;

	 		// Multi Slice parsing!: 1 frame is composed of multi slices.
			// find the first Non-IDR frame
			for(i= 1; i < uStreamFileSize+1;i++)
			{
				if ((*(pucSrcMem)  ==  0x00) && (*(pucSrcMem+1)  ==  0x00)&& (*(pucSrcMem+2)  ==  0x00) 
					&& (*(pucSrcMem+3)  ==  0x01) && ( (*(pucSrcMem+4)&0x1f)  ==  0x01))
				{ 
					auFrameStartAddr[2] = (u32) pucSrcMem;			
					break;
				}
				pucSrcMem++;
			}

			pucSrcMem =(u8*) auFrameStartAddr[0] +1;
			for(i= 1; i < auFrameStartAddr[2] - auFrameStartAddr[0] +1;i++)
			{
				if ((*(pucSrcMem)  ==  0x00) && (*(pucSrcMem+1)  ==  0x00)&& (*(pucSrcMem+2)  ==  0x00) 
					&& (*(pucSrcMem+3)  ==  0x01) && ( (*(pucSrcMem+4)&0x1f)  ==  0x05))
				{ 
					uNumOfH264IFrame++;
					if(uNumOfH264IFrame ==1)
					{
						auFrameStartAddr[1] =  (u32) pucSrcMem;  // the first I frame
					}

				}
				pucSrcMem++;
			}

			// finding the next P frame strat addresses.
			j=3;
			uTemp32 =0;
			if (uNumOfH264IFrame != 1)
			{
				for(i= 1; i < uStreamFileSize - auFrameStartAddr[2] + auFrameStartAddr[0]+1;i++)
				{
					if ((*(pucSrcMem)  ==  0x00) && (*(pucSrcMem+1)  ==  0x00)&& (*(pucSrcMem+2)  ==  0x00) 
						&& (*(pucSrcMem+3)  ==  0x01) && ( (*(pucSrcMem+4)&0x1f)  ==  0x01))
					{ 
						uTemp32++;
						if(uTemp32 == uNumOfH264IFrame)
						{
							auFrameStartAddr[j] = (u32) pucSrcMem;
							j++;
							uTemp32 =0;
							uMyFrameIndex++;
						}
					}
					pucSrcMem++;
				}
				uNumOfFrames = uMyFrameIndex+1;
				UART_Printf(" This file has %d frames \n", uNumOfFrames);
				uCopySize = auFrameStartAddr[2]- auFrameStartAddr[0];
				uReadAmountByteSize = uReadAmountByteSize +uCopySize;
		
				// copy SPS+PPS+SEI+I-frame into Streambuffer 
				Copy8( auFrameStartAddr[0],uStreamBufStAddr,uCopySize);
				break;
			}

			//  1 frame is composed of single slice.
			pucSrcMem = (u8*) uStreamBufForSD;
			for(i= 1; i < uStreamFileSize+1;i++)
			{
				if ((*(pucSrcMem)  ==  0x00) && (*(pucSrcMem+1)  ==  0x00)&& (*(pucSrcMem+2)  ==  0x00) 
					&& (*(pucSrcMem+3)  ==  0x01) &&( ( (*(pucSrcMem+4)&0x1f)  ==  0x01)||( (*(pucSrcMem+4)&0x1f)  ==  0x05)))
				{ 
					if(uMyFrameIndex > MAX_FRAME_NUM -1) { break;}
					auFrameStartAddr[uMyFrameIndex] = (u32) pucSrcMem;
					uMyFrameIndex++;
				}
				pucSrcMem++;
			}
			uNumOfFrames = uMyFrameIndex-1;
			UART_Printf(" This file has %d frames \n", uNumOfFrames);
			uCopySize = auFrameStartAddr[2]- auFrameStartAddr[0];
			uReadAmountByteSize = uReadAmountByteSize +uCopySize;
		
			// copy SPS+PPS+SEI+I-frame into Streambuffer 
			Copy8( auFrameStartAddr[0],uStreamBufStAddr,uCopySize);
			

			break;

		case MP4_DEC:

			//  MPEG4  stream file parsing: Begin
			//=============================================================================================
			// Structure of MPEG4 encoded stream
			// auFrameStartAddr[0] --- auFrameStartAddr[1]-1 	: VOS + VOP layer header
			// auFrameStartAddr[1] --- auFrameStartAddr[2]-1 	: 1st frame
			// ....
			// find start_code

			bIsHeader = true;
			uBufferByteSize = 0x10000;
			Vsparser(eDecMode, uStreamBufForSD , uBufferByteSize,  bIsHeader, 
				&uFrameStartAddr, &uFrameSize, &uNumOfFrames,&bIsMpeg4ShrotHeader);		
			auFrameStartAddr[0] = uFrameStartAddr;
			auFrameStartAddr[1] = uFrameStartAddr + uFrameSize;
			if(bIsMpeg4ShrotHeader == true) 
			{
				bIsFirstFrame = true;
			}
			uCopySize = auFrameStartAddr[1]- auFrameStartAddr[0];
			uReadAmountByteSize = uReadAmountByteSize +uCopySize;
			// copy VOP+VOS Header into Streambuffer 
			Copy8( auFrameStartAddr[0],uStreamBufStAddr,uCopySize);
			break;

			//=============================================================================================
			//  MPEG4 stream file parsing: End

		default:
			return;
	}



	// ##### FramebyFrame Decoding (fileplay mode) Init  : Begin #####
	//=============================================================================================
	uFrameBufStAddr = uFreeMemBuf;
	bFilePlayEn = true;
	bDynBufAllocEn = true;
	bDecRotModeEn = false;
	bH264ReorderEn = false;		// if reordering is enabled, then MFC needs 16 frames to decode in advance.
	bMp4DeblkEn = false;
	
	MFC_InitProcessForOneFrameDecoding(
	uProcessIdx, eDecMode, uStreamBufStAddr,STREAM_BUF_SIZE,
	uFrameBufStAddr, bDecRotModeEn,bMp4DeblkEn, bH264ReorderEn,bFilePlayEn, bDynBufAllocEn);

	MFC_GetProcessBufEndAddr(uProcessIdx, &uFreeMemBuf);
	INTC_SetVectAddr(NUM_MFC,Isr_Mfc);			//SMDK S/W
	INTC_Enable(NUM_MFC);


	
	// 4. Init LCDC
	//-------------------------------------------

	uLcdFbAddr = uFreeMemBuf;
	eLcdBpp = RGB24;	
	LCD_InitDISPC(eLcdBpp, uLcdFbAddr, WIN0, false);
	LCD_GetFbEndAddr(&uFreeMemBuf, WIN0); // Get the free mem buffer addr
	
	LCD_GetFrmSz(&uLcdWidth, &uLcdHeight, WIN0);
	uDecWidth = uLcdWidth;
	uDecHeight = uLcdHeight*9/10;

	GLIB_InitInstance(uLcdFbAddr, uLcdWidth, uLcdHeight, eLcdBpp);
	GLIB_ClearFrame(C_BLUE);
	GLIB_SetFontColor(C_WHITE, C_BLUE, false);
	GLIB_Printf(2, uDecHeight+6, "S3C6410 MFC Decoder");

	
	// 5. Init POST
	//-------------------------------------------
	MFC_GetSrcPicSize(uProcessIdx, &uPicHsz, &uPicVsz);
	MFC_GetDispFrameNum(uProcessIdx, &uDispFrameNum);	

	uRotWidth = (uDecRotDeg%180 == 0) ? uPicHsz : uPicVsz;
	uRotHeight = (uDecRotDeg%180 == 0) ? uPicVsz : uPicHsz;
	
	POST_InitIp1(
		uRotWidth, uRotHeight, 0, 0, uRotWidth, uRotHeight, uFrameBufStAddr, YC420,
		uLcdWidth, uLcdHeight, 0, 0, uDecWidth, uDecHeight, uLcdFbAddr, eLcdBpp,
		uDispFrameNum, false, ONE_SHOT, POST_DMA, POST_DMA, &oPost);
	INTC_SetVectAddr(NUM_POST0, Isr_Post);
	INTC_Enable(NUM_POST0);	
	POST_EnableInterrupt(POST_LEVEL_INT, &oPost);
	
	fTotTime=0;
	fTotDecTime=0;
	frameCount=0;
	MFC_GetDecSrcFormat(&picX, &picY, &frameRate);

	LCD_Start();

		

	// Decode all frames
	//-------------------------------------------
	j = 1; //Current Frame
	while (GetKey() == 0) // loop until any key is pressed.
	{
		// 6. Decode one frame
		//---------------------

		bStreamBufEmpty = false;	
		bMfcDone = false;
		MFC_SetDecRotEn(bDecRotModeEn);
		if (bDecRotModeEn)
		MFC_SetDecRotationMode(bDecMirHor, bDecMirVer, uDecRotDeg);

		switch(eDecMode)
		{
			case VC1_DEC:
				if(bIsCnMSeq == true) 
				{ 
					bIsCnMSeq = false;
					j++;
					j = j+ uTemp32_1;
					break;
				}
				bIsHeader =false;
				uBufferByteSize = 0x10000;
				Vsparser(eDecMode, auFrameStartAddr[j] , uBufferByteSize, bIsHeader, 
					&uFrameStartAddr, &uFrameSize, &uNumOfFrames,&bIsMpeg4ShrotHeader);		
				auFrameStartAddr[j+1] = uFrameStartAddr + uFrameSize;

				uCopySize = auFrameStartAddr[j+1] - auFrameStartAddr[j];
				uReadAmountByteSize = uReadAmountByteSize +uCopySize;
				Copy8( auFrameStartAddr[j],uStreamBufStAddr,uCopySize);	
				MFC_SetWrPtr2(uProcessIdx, uStreamBufStAddr + uCopySize);
				j++;
				break;

			case AVC_DEC:
				if (bIsFirstFrame == true)    // In H.264 case You, should not feed I-frame in the first PIC_RUN
				{
					bIsFirstFrame = false;
					j++;
					break;
				}

				uCopySize = auFrameStartAddr[j+1] - auFrameStartAddr[j];
				uReadAmountByteSize = uReadAmountByteSize +uCopySize;
				Copy8( auFrameStartAddr[j],uStreamBufStAddr,uCopySize);	
				j++;
				break;

			case MP4_DEC:
				if(bIsFirstFrame == true) 
				{ 
					bIsFirstFrame= false;
					break;
				}
				bIsHeader =false;
				uBufferByteSize = 0x10000;
				bIsStreamCorrect = Vsparser(eDecMode, auFrameStartAddr[j] , uBufferByteSize, bIsHeader, 
					&uFrameStartAddr, &uFrameSize, &uNumOfFrames,&bIsMpeg4ShrotHeader);		
				if(bIsStreamCorrect == false) 
				{
					break;
				}
				auFrameStartAddr[j+1] = uFrameStartAddr + uFrameSize;
				
				uCopySize = auFrameStartAddr[j+1] - auFrameStartAddr[j];
				uReadAmountByteSize = uReadAmountByteSize +uCopySize;
				Copy8( auFrameStartAddr[j],uStreamBufStAddr,uCopySize);	
				j++;
				break;
				
			default:
				return;
		}
		
		StartTimer(0);
		
		if(bIsStreamCorrect == false) 
		{
			bIsStreamCorrect = true;
			UART_Printf("Stream End or Stream Error! \n");
			break;
		}

		MFC_InitStreamBufferForOneFrameDecoding(uProcessIdx, uStreamBufStAddr, uCopySize);
		MFC_StartDecodingOneFrame(uProcessIdx);


		while(bMfcDone != true);

		fDecTime = StopTimer(0);	
		fTotTime += fDecTime;
		fTotDecTime += fDecTime;


		if (MFC_DoPostProcessingOfDecoding(uProcessIdx, bStreamBufEmpty, &uFrameIdx) == false)
		break;

		frameCount++;
		if((s32)uFrameIdx==-3)
		continue;


		// 7. Convert the frame for display
		//----------------------------------
		StartTimer(0);
		
		bPostDone = false;
		POST_StartProcessing1(uFrameIdx, 0, &oPost);
		while(bPostDone == false);

		LCD_Trigger();

		fPostTime = StopTimer(0);	
		fTotTime += fPostTime;
		

		// 8. Load stream if necessary
		//------------------------------


		// ##### Check all frames are decoded : start #####
		//=============================================================================================

		if ((uReadAmountByteSize >=  uStreamFileSize)||( frameCount == MAX_FRAME_NUM-2)||( frameCount == uNumOfFrames -1)) 
		{
			UART_Printf("Decode Complete! \n" );
			MFC_NotifyNoMoreStream(uProcessIdx);
			break;
		}

		// 9. Display frame rate on the LCD
		//----------------------------------
		if((frameCount%10)==0)
		{
			UART_Printf("Current Frame number= %d \n", frameCount);
			GLIB_Printf(2, uDecHeight+6, "S3C6410 MFC Decoder(FPS=%.2f @%dx%d)",1000000*frameCount/fTotTime,picX,picY);
		}
	}

	UART_Printf("\ntotal decoding time=%.fus, pure FPS=%.2f\n",fTotDecTime,1000000*frameCount/fTotDecTime);
	UART_Printf("total decoding+post_processing time=%.fus\n",fTotTime);
	UART_Printf("FPS=%.2f, average elapsed time= %.2f(ms), total %d frames\n", 
		1000000*frameCount/fTotTime, fTotTime/frameCount/1000, frameCount);


	MFC_IssueSeqEndCmd(uProcessIdx);
	MFC_StopBitProcessor();

	FAT_UnloadFileSystem(&oSdhc);
	SDHC_CloseMedia(&oSdhc);

}
#endif

//////////
// Function Name : TestDmbDecoding
// Function Description : This function decodes an DMB stream file and displays the generated YUV
// Input : NONE
// Output : NONE
// Version : 
static void TestDmbDecoding(void)
{
	u32 uFreeMemBuf = MFC_MEMORY_BASE;
#if	(USE_SD_STORAGE)
	char pFileName[256];
	u32 uTotalNumOfFiles;
	char fileExt[50];
	int sel, i;
	const int uBlkSz = 512;
	MFC_CODEC_MODE eDecMode;
	u8 bFat_FileLoad=false;
#else
	u32 uStreamFileAddr;
#endif	
	const int nReadUnit = DMB_STREAM_BUF_SIZE/2;
	u32 uStreamFileSize;
	u32 uStreamBufStAddr;
	int nReadSize;
	u32 uStreamOffset;
	u32 uFrameBufStAddr;
	u32 uProcessIdx;
	u32 uLcdFbAddr;
	CSPACE eLcdBpp;
	u32 uPicHsz, uPicVsz;
	u32 uDispFrameNum;
	u32 uRotWidth;
	u32 uRotHeight;
	u32 uLcdWidth, uLcdHeight;
	u32 uFrameIdx;
	u32 frameCount;

#if	(!USE_SD_STORAGE)
	// 0. Download the stream file thru USB.
	//-------------------------------------------
	uStreamFileAddr = uFreeMemBuf;
	UART_Printf(" Before testing, download a stream file through USB\n");
	UART_Printf(" If you want to test with previous stream file, press 'x' key\n");
	if(Getc()!='x')	
		uStreamFileSize = DownloadImageThruUsbOtg((u8 *)uStreamFileAddr);
	if (uStreamFileSize == 0)
		return;
	uFreeMemBuf += uStreamFileSize;
	uFreeMemBuf = (uFreeMemBuf + STREAM_WR_SIZE-1)/STREAM_WR_SIZE*STREAM_WR_SIZE;
#endif

	// 1. Move firmware and init base settings.
	//-------------------------------------------
	
	MFC_MoveFirmwareToCodeBuf(uFreeMemBuf);
	MFC_GetCodeBufEndAddr(&uFreeMemBuf);
	MFC_StartBitProcessor();

	MFC_InitBaseForProcesses1(uFreeMemBuf, DECODING_DMB);
	MFC_GetBaseBufEndAddr(&uFreeMemBuf);

	// 2. Reserve the stream buffer, & load the first part of a stream
	//-----------------------------------------------------------------

#if USE_SD_STORAGE
	while(!SDHC_OpenMedia(SDHC_HCLK, &oSdhc));
	bFat_FileLoad=FAT_LoadFileSystem(&oSdhc);
	if (!bFat_FileLoad)
		Assert(0);
		
	if(bFat_FileLoad==false)	//to avoid duplicate display
	{
		FAT_GetTotalNumOfFiles(&uTotalNumOfFiles, &oSdhc);
		for (i=0; i<uTotalNumOfFiles; i++)
		{
			FAT_GetFileName(i, pFileName, &oSdhc);
			UART_Printf("%02d -- %s\n", i, pFileName);
		}
	}

	UART_Printf(" Which file to read ? ");
	sel = UART_GetIntNum();

	Assert(nReadUnit%uBlkSz == 0);

	FAT_GetFileSize(sel, (s32 *)&uStreamFileSize, &oSdhc);

	uStreamBufStAddr = uFreeMemBuf;
	uFreeMemBuf += DMB_STREAM_BUF_SIZE; // Reserved for stream buf.	
	Assert(uStreamBufStAddr%STREAM_WR_SIZE == 0);
	
	nReadSize = (uStreamFileSize < nReadUnit) ? uStreamFileSize : nReadUnit;
	uStreamOffset = nReadSize;
	if (!FAT_ReadFile4(sel, 0, (nReadSize+uBlkSz-1)/uBlkSz, uStreamBufStAddr, &oSdhc))
		Assert(0);
	
#else
	uStreamBufStAddr = uFreeMemBuf;
	uFreeMemBuf += DMB_STREAM_BUF_SIZE; // Reserved for stream buf.	
	Assert(uStreamBufStAddr%STREAM_WR_SIZE == 0);
	
	nReadSize = (uStreamFileSize < nReadUnit) ? uStreamFileSize : nReadUnit;
	uStreamOffset = nReadSize;
	memcpy((void *)uStreamBufStAddr, (void *)uStreamFileAddr, nReadSize);	
#endif

	// 3. Init a process of MFC
	//--------------------------
#if USE_SD_STORAGE
	FAT_GetFileExt(sel, fileExt, &oSdhc);		
	MFC_GetCodecModeByName(fileExt, true, &eDecMode);
	Assert(eDecMode == AVC_DEC);
#endif	

	uProcessIdx=0;
	
	uFrameBufStAddr = uFreeMemBuf;
	
	MFC_InitProcessForDmbDecoding(uProcessIdx, uStreamBufStAddr, DMB_STREAM_BUF_SIZE, 
			uFrameBufStAddr, bDecRotModeEn);

	MFC_GetProcessBufEndAddr(uProcessIdx, &uFreeMemBuf);
	MFC_SetWrPtr(uProcessIdx, uStreamBufStAddr+nReadSize);
	INTC_SetVectAddr(NUM_MFC,Isr_Mfc);
	INTC_Enable(NUM_MFC);
	
	
	// 4. Init LCDC
	//---------------

	uLcdFbAddr = uFreeMemBuf;
	eLcdBpp = RGB24;	
	LCD_InitDISPC(eLcdBpp, uLcdFbAddr, WIN0, false);
	LCD_GetFbEndAddr(&uFreeMemBuf, WIN0); // Get the free mem buffer addr
	
	// 5. Init POST
	//---------------
	MFC_GetSrcPicSize(uProcessIdx, &uPicHsz, &uPicVsz);
	MFC_GetDispFrameNum(uProcessIdx, &uDispFrameNum);	

	uRotWidth = (uDecRotDeg%180 == 0) ? uPicHsz : uPicVsz;
	uRotHeight = (uDecRotDeg%180 == 0) ? uPicVsz : uPicHsz;
	
	LCD_GetFrmSz(&uLcdWidth, &uLcdHeight, WIN0);

	POST_InitIp(uRotWidth, uRotHeight, uFrameBufStAddr, YC420,
		uLcdWidth, uLcdHeight,uLcdFbAddr, eLcdBpp, uDispFrameNum, false, ONE_SHOT, &oPost);
	INTC_SetVectAddr(NUM_POST0, Isr_Post);
	INTC_Enable(NUM_POST0);
	POST_EnableInterrupt(POST_LEVEL_INT, &oPost);

	frameCount=0;
	
	LCD_Start();
		
	while (GetKey() == 0) // loop until any key is pressed.
	{
		// 6. Decode one frame
		//---------------------
		bStreamBufEmpty = false;	
		bMfcDone = false;
		MFC_SetDecRotEn(bDecRotModeEn);
		if (bDecRotModeEn)
			MFC_SetDecRotationMode(bDecMirHor, bDecMirVer, uDecRotDeg);
		MFC_StartDecodingOneFrame(uProcessIdx);
		while(bMfcDone != true);

		if (MFC_DoPostProcessingOfDecoding(uProcessIdx, bStreamBufEmpty, &uFrameIdx) == false)
			break;

		frameCount++;

		// 7. Convert the frame for display
		//----------------------------------

		bPostDone = false;
		POST_StartProcessing1(uFrameIdx, 0, &oPost);
		while(bPostDone == false);

		LCD_Trigger();

		// 8. Load stream if necessary
		//------------------------------

		if (uStreamFileSize == uStreamOffset)
		{					
			MFC_NotifyNoMoreStream(uProcessIdx);
		}
		else if (MFC_IsMoreStreamNeeded(uProcessIdx, nReadUnit))
		{
			if ((uStreamFileSize-uStreamOffset) < nReadUnit) 
				nReadSize = (uStreamFileSize-uStreamOffset);
			else
				nReadSize = nReadUnit;

#if USE_SD_STORAGE			
			if (!FAT_ReadFile4(sel, uStreamOffset/uBlkSz, (nReadSize+uBlkSz-1)/uBlkSz, uStreamBufStAddr+uStreamOffset%DMB_STREAM_BUF_SIZE, &oSdhc))
				Assert(0);
#else
			memcpy((void *)(uStreamBufStAddr+uStreamOffset%DMB_STREAM_BUF_SIZE), (void *)(uStreamFileAddr+uStreamOffset), nReadSize);
#endif
			
			uStreamOffset += nReadSize;
			
			MFC_SetWrPtr(uProcessIdx, uStreamBufStAddr+uStreamOffset%DMB_STREAM_BUF_SIZE);
		}
	}

#if USE_SD_STORAGE	
	FAT_UnloadFileSystem(&oSdhc);
	SDHC_CloseMedia(&oSdhc);
#endif	
	
	MFC_IssueSeqEndCmd(uProcessIdx);
	MFC_StopBitProcessor();
}

//////////
// Function Name : Compare128
// Function Description : This function compares 2 quad words
// Input : a0, a1, qwords
// Output : ok or err
// Version :
u8 Compare128(u32 a0, u32 a1, u32 qwords)
{
	u32 i;
	
	for(i=0;i<(qwords*16);)
	{
		if(*(u32 *)(a0+i)!=*(u32 *)(a1+i))
			return 0;
		i+=4;
		
	}
	return 1;
}

//////////
// Function Name : FindFrameBufferDiff
// Function Description : This function compares 2 YUV images
// Input : 
// Output : NONE
// Version : 
u8 FindFrameBufferDiff(u32 a0, u32 a1, u32 bytes, u32 uWidth, u32 uHeight, u32* uPeakDiff, u32* uAvgDiff)
{
	u8* pD0 = (u8 *)a0;
	u8* pD1 = (u8 *)a1;
	u8 ret = true;
	u32 uErrCnt = 0;
	u32 uMaxDiff = 0;
	u32 uSumDiff = 0;
	u32 temp2, i;
	u32 uYEndAddr = uWidth*uHeight;
	u32 uCbEndAddr = uWidth*uHeight*5/4;
	u32 uCrEndAddr = uWidth*uHeight*3/2;
	u32 uStride = (uWidth%16 ==0) ? uWidth : (uWidth+15)/16*16;
	u32 uNewHeight = (uHeight%16 ==0) ? uHeight : (uHeight+15)/16*16;
	
	for (i=0; i<bytes; i++)
	{
		if (*pD0 != *pD1) 
		{
			ret = false;			
			temp2 = (*pD0 > *pD1) ? *pD0-*pD1 : *pD1-*pD0;

			if (uMaxDiff < temp2)
				uMaxDiff = temp2;
			uErrCnt++;
			//UART_Printf("%d:0x%x~0x%x: %d-%d ",i,(u32)pD0, (u32)pD1, *pD0,*pD1);
			//UART_Printf("%d: %d-%d ",i, *pD0, *pD1);
		}			

		pD0++;
		if ((i%uWidth == uWidth-1) && i<uYEndAddr)
		{
			pD1 = pD1 + (uStride-uWidth) + 1;
			if (i==uYEndAddr-1)
			{
				pD1 = pD1 + uStride*(uNewHeight-uHeight);
			}
		}
		else if (i>=uYEndAddr && i<uCrEndAddr && ((i-uYEndAddr)%(uWidth/2) == (uWidth/2-1)))
		{
			pD1 = pD1 + (uStride-uWidth)/2 + 1;
			if (i==uCbEndAddr-1)
			{
				pD1 = pD1 + uStride*(uNewHeight-uHeight)/4;
			}
		}
		else
		{
			pD1++;
		}
	}

	if (ret == false)
	{
		Assert(uErrCnt>0);
		//UART_Printf("\nError Count=%d, Max diff=%d, Avg diff=%d\n", uErrCnt, uMaxDiff, uSumDiff/uErrCnt);
		*uPeakDiff = uMaxDiff;
		*uAvgDiff = uSumDiff/uErrCnt;
	}
	else
	{
		*uPeakDiff = 0;
		*uAvgDiff = 0;
	}
	return ret;
}

//////////
// Function Name : CompareFrameBuffers
// Function Description : This function compares 2 YUV images
// Input : 
// Output : NONE
// Version : 
u8 CompareFrameBuffers(
	u32 a0, u32 width, u32 height, 
	u32 a1, u32 dispWidth, u32 dispHeight,
	u32* uMaxAbsDiff, u32* uAvgDiff)
{
	u32 uAbsDiff;
	u8 resultComp=true;
	u8 resultCompY;
	u32 j;
	u8 resultCompCb;
	u32 uCompStAddr;
	u32 uFrameStAddr;
	u8 resultCompCr;

	for (j=0; j<height; j++)
	{
		resultCompY = Compare128(a0+j*width, a1+j*dispWidth, width/16);
		if (!resultCompY)
		{
			FindFrameBufferDiff(a0+j*width, a1+j*dispWidth, width, width,   height, &uAbsDiff, uAvgDiff);			
			if (*uMaxAbsDiff < uAbsDiff)
			{				
				*uMaxAbsDiff = uAbsDiff;								
			}
			resultComp = false;
		}
	}

	uCompStAddr = a0+width*height;
	uFrameStAddr = a1+dispWidth*dispHeight;
	for (j=0; j<height/2; j++)
	{
		resultCompCb = Compare128(uCompStAddr+j*width/2, uFrameStAddr+j*dispWidth/2, width/2/16);
		if (!resultCompCb)
		{
			FindFrameBufferDiff(uCompStAddr+j*width/2, uFrameStAddr+j*dispWidth/2, width/2, width,   height, &uAbsDiff, uAvgDiff);			
			if (*uMaxAbsDiff < uAbsDiff)
			{				
				*uMaxAbsDiff = uAbsDiff;								
			}
			resultComp = false;
		}
	}		

	uCompStAddr = a0+width*height*5/4;
	uFrameStAddr = a1+dispWidth*dispHeight*5/4;
	for (j=0; j<height/2; j++)
	{
		resultCompCr = Compare128(uCompStAddr+j*width/2, uFrameStAddr+j*dispWidth/2, width/2/16);
		if (!resultCompCr)
		{
			FindFrameBufferDiff(uCompStAddr+j*width/2, uFrameStAddr+j*dispWidth/2, width/2, width,   height, &uAbsDiff, uAvgDiff);			
			if (*uMaxAbsDiff < uAbsDiff)
			{				
				*uMaxAbsDiff = uAbsDiff;								
			}
			resultComp = false;
		}
	}
	return resultComp;
}

//////////
// Function Name : TestDecConformanceEach
// Function Description : This function decodes a stream file and then 
//							compares the decoded YUV image with pre-made YUV image
// Input : NONE
// Output : NONE
// Version : 
#if (USE_SD_STORAGE)
static void TestDecConformanceEach(void)
{
	u32 uFreeMemBuf = CODEC_MEM_ST;
	char pFileName[256];
	u32 uTotalNumOfFiles, i;
	int sel;
	s32 nDecFileSize;
	const int nBlkSz = 512;
	const int nReadUnit = STREAM_BUF_SIZE/2;
	int nReadSize;
	u32 uStreamPtr;	
	u32 uStreamBufStAddr;
	u32 selYuv;
	u32 uProcessIdx;
	u32 uFrameBufStAddr;
	u32 picX, picY;
	u32 uWidth, uHeight;
	char fileExt[50];
	u32 uYuvBufAddr;
	u32 uYuvFrameSize;
	u32 uFileOffset;
	u32 uReturnAddr;
	
	u32 uFrameAddrY;
	u32 uAbsDiff;
	u32 uMaxAbsDiff=0;
	u32 uFrameIdx;
	u32 uFrameCount = 0;
	u32 uFrmNum;
	u32 uErrMbNum;
	MFC_CODEC_MODE eDecMode;
	u8 bConformanceError=false;

	// 1. Move firmware and init base settings.
	//-------------------------------------------
	
	MFC_MoveFirmwareToCodeBuf(uFreeMemBuf);
	MFC_GetCodeBufEndAddr(&uFreeMemBuf);
	MFC_StartBitProcessor();
			
	MFC_InitBaseForProcesses1(uFreeMemBuf, DECODING_CONFORMANCE);
	MFC_GetBaseBufEndAddr(&uFreeMemBuf);	

	// 2. Reserve the stream buffer, & load the first part of a stream
	//-----------------------------------------------------------------

	if (!SDHC_OpenMedia(SDHC_HCLK, &oSdhc) || !FAT_LoadFileSystem(&oSdhc))
		Assert(0);

	FAT_GetTotalNumOfFiles(&uTotalNumOfFiles, &oSdhc);
	for (i=0; i<uTotalNumOfFiles; i++)
	{
		FAT_GetFileName(i, pFileName, &oSdhc);
		UART_Printf("%02d -- %s\n", i, pFileName);
	}

	UART_Printf(" Which file to decode ? ");
	sel = UART_GetIntNum();

	FAT_GetFileSize(sel, &nDecFileSize, &oSdhc);
	//UART_Printf("nDecFileSize=%d\n",nDecFileSize);	

	nReadSize = (nDecFileSize < nReadUnit) ? nDecFileSize : nReadUnit;
	uStreamPtr = nReadSize;	
	uStreamBufStAddr = uFreeMemBuf;
	uFreeMemBuf += STREAM_BUF_SIZE;
	if (!FAT_ReadFile4(sel, 0, (nReadSize+nBlkSz-1)/nBlkSz, uStreamBufStAddr, &oSdhc))
		Assert(0);

	FAT_GetFileName(sel, pFileName, &oSdhc);
	if (!FAT_GetFileIndex1(pFileName, "yuv", &selYuv, &oSdhc))
		Assert(0);

	// 3. Init a process of MFC
	//--------------------------
	FAT_GetFileExt(sel, fileExt, &oSdhc);		
	MFC_GetCodecModeByName(fileExt, true, &eDecMode);

	uProcessIdx=0;
	bH264ReorderEn = true;
	bDecRotModeEn = false;

	uFrameBufStAddr = uFreeMemBuf;
	
	if (eDecMode == MP4_DEC)
		MFC_InitProcessForDecodingMpeg4(uProcessIdx, uStreamBufStAddr, STREAM_BUF_SIZE, 			
			uFrameBufStAddr, bDecRotModeEn, false);
	else if (eDecMode == AVC_DEC)
		MFC_InitProcessForDecodingH264(uProcessIdx, uStreamBufStAddr, STREAM_BUF_SIZE, 
			uFrameBufStAddr, bDecRotModeEn, bH264ReorderEn);
	else if (eDecMode == VC1_DEC)
		MFC_InitProcessForDecodingVc1(uProcessIdx, uStreamBufStAddr, STREAM_BUF_SIZE, 
			uFrameBufStAddr, bDecRotModeEn);
	else
		Assert(0);
	MFC_GetProcessBufEndAddr(uProcessIdx, &uFreeMemBuf);
	MFC_SetWrPtr(uProcessIdx, uStreamBufStAddr+nReadSize);
	INTC_SetVectAddr(NUM_MFC,Isr_Mfc);
	INTC_Enable(NUM_MFC);

	MFC_GetSrcPicSize(uProcessIdx, &picX, &picY);
	MFC_GetFrameSize(uProcessIdx, &uWidth, &uHeight);	
		
	// YUV read buffer
	uYuvBufAddr = uFreeMemBuf;
	uYuvFrameSize = picX * picY * 3/2;
	uFileOffset = 0;
	uFreeMemBuf += (uYuvFrameSize/512 + 2) * 512;
	
	uMaxAbsDiff=0;
	uFrameCount = 0;
			
	while (GetKey() == 0)   // exit when any key is pressed.
	{
		// 4. Decode one frame
		//---------------------

		bStreamBufEmpty = false;	
		bMfcDone = false;
		MFC_SetDecRotEn(false);
		MFC_StartDecodingOneFrame(uProcessIdx);
		while(bMfcDone != true);

		if (MFC_DoPostProcessingOfDecoding(uProcessIdx, bStreamBufEmpty, &uFrameIdx) == false)
			break;
		MFC_GetAddrOfYFrame(uProcessIdx, uFrameIdx, &uFrameAddrY);

		uFrameCount++;

		// 5. Load stream if necessary
		//------------------------------

		if (nDecFileSize == uStreamPtr)
		{					
			MFC_NotifyNoMoreStream(uProcessIdx);
		}
		else if (MFC_IsMoreStreamNeeded(uProcessIdx, nReadUnit))
		{
			nReadSize = (nDecFileSize-uStreamPtr < nReadUnit) ? nDecFileSize-uStreamPtr : nReadUnit;
			if (!FAT_ReadFile4(sel, uStreamPtr/nBlkSz, (nReadSize+nBlkSz-1)/nBlkSz, uStreamBufStAddr+uStreamPtr%STREAM_BUF_SIZE, &oSdhc))
				Assert(0);
			uStreamPtr += nReadSize;
			MFC_SetWrPtr(uProcessIdx, uStreamBufStAddr+uStreamPtr%STREAM_BUF_SIZE);
		}
		
		// 6. Read one YUV frame from SD/MMC card
		//----------------------------------------

		if (!FAT_ReadFile6(selYuv, uFileOffset, uYuvFrameSize, uYuvBufAddr, &uReturnAddr, &oSdhc))
			break;

		// 7. Compare the decoded frame with the golden one
		//---------------------------------------------------------

		if (!CompareFrameBuffers(uReturnAddr, picX, picY, uFrameAddrY, uWidth, uHeight, &uMaxAbsDiff, &uAbsDiff))
		{
			MFC_GetFrameCount(&uFrmNum);
			MFC_GetDecErrorMbNum(&uErrMbNum);
			//UART_Printf("%dth frame: max diff=%d, Err MB num=%d\n",uFrmNum, uMaxAbsDiff, uErrMbNum);
			if(uErrMbNum!=0)
			{
				UART_Printf("%dth frame: max diff=%d, Err MB num=%d\n",uFrmNum, uMaxAbsDiff, uErrMbNum);
				bConformanceError=true;
				break;
			}
		}

		uFileOffset += uYuvFrameSize;
		
		if((uFrameCount%10)==0)
			Putc('.');
	}

	// 8. Report the maximum difference
	//----------------------------------

	//UART_Printf("\n::: Max Abs Diff: %d among the %d frames\n", uMaxAbsDiff, uFrameCount);
	
	if(bConformanceError==false)
		UART_Printf("O.K\n");
	else
		UART_Printf("Fail\n");

	FAT_UnloadFileSystem(&oSdhc);
	SDHC_CloseMedia(&oSdhc);	
	
	MFC_IssueSeqEndCmd(uProcessIdx);
	MFC_StopBitProcessor();
	
}

//////////
// Function Name : TestDecConformance
// Function Description : This function decodes several stream files and then 
//							compares the decoded YUV images with pre-made YUV images
// Input : NONE
// Output : NONE
// Version : 
static void TestDecConformance(void)
{
	u32 uFreeMemBuf = CODEC_MEM_ST;
	char pFileName[256];
	int sel;
	s32 nDecFileSize;
	const int nBlkSz = 512;
	const int nReadUnit = STREAM_BUF_SIZE/2;
	int nReadSize;
	u32 uStreamPtr;	
	u32 uStreamBufStAddr;
	u32 selYuv;
	u32 uProcessIdx;
	u32 uFrameBufStAddr;
	u32 picX, picY;
	u32 uWidth, uHeight;
	char fileExt[50];
	u32 uYuvBufAddr;
	u32 uYuvFrameSize;
	u32 uFileOffset;
	u32 uReturnAddr;
	
	u32 uFrameAddrY;
	u32 uAbsDiff;
	u32 uMaxAbsDiff=0;
	u32 uFrameIdx;
	u32 uFrameCount = 0;
	u32 uFrmNum;
	u32 uErrMbNum;
	MFC_CODEC_MODE eDecMode;
	u8 bConformanceError=false;	
	u32 uCount=0, uStartStream=0;
	

	if (!SDHC_OpenMedia(SDHC_HCLK, &oSdhc) || !FAT_LoadFileSystem(&oSdhc))
		Assert(0);
		
	UART_Printf("Select the start stream file\n");
	uStartStream=UART_GetIntNum();
	UART_Printf("Enter the count of stream files\n");
	uCount=UART_GetIntNum();
	for(sel=uStartStream;sel<(uStartStream+uCount);sel++)
	{
		uFreeMemBuf = CODEC_MEM_ST;

		// 1. Move firmware and init base settings.
		//-------------------------------------------
		
		MFC_MoveFirmwareToCodeBuf(uFreeMemBuf);
		MFC_GetCodeBufEndAddr(&uFreeMemBuf);
		MFC_StartBitProcessor();
				
		MFC_InitBaseForProcesses1(uFreeMemBuf, DECODING_CONFORMANCE);
		MFC_GetBaseBufEndAddr(&uFreeMemBuf);	

		// 2. Reserve the stream buffer, & load the first part of a stream
		//-----------------------------------------------------------------	

		FAT_GetFileSize(sel, &nDecFileSize, &oSdhc);
		//UART_Printf("nDecFileSize=%d\n",nDecFileSize);	

		nReadSize = (nDecFileSize < nReadUnit) ? nDecFileSize : nReadUnit;
		uStreamPtr = nReadSize;	
		uStreamBufStAddr = uFreeMemBuf;
		uFreeMemBuf += STREAM_BUF_SIZE;
		if (!FAT_ReadFile4(sel, 0, (nReadSize+nBlkSz-1)/nBlkSz, uStreamBufStAddr, &oSdhc))
			Assert(0);

		FAT_GetFileName(sel, pFileName, &oSdhc);
		if (!FAT_GetFileIndex1(pFileName, "yuv", &selYuv, &oSdhc))
			Assert(0);

		// 3. Init a process of MFC
		//--------------------------
		FAT_GetFileExt(sel, fileExt, &oSdhc);		
		MFC_GetCodecModeByName(fileExt, true, &eDecMode);

		uProcessIdx=0;
		bH264ReorderEn = true;
		bDecRotModeEn = false;

		uFrameBufStAddr = uFreeMemBuf;
		
		if (eDecMode == MP4_DEC)
			MFC_InitProcessForDecodingMpeg4(uProcessIdx, uStreamBufStAddr, STREAM_BUF_SIZE, 			
				uFrameBufStAddr, bDecRotModeEn, false);
		else if (eDecMode == AVC_DEC)
			MFC_InitProcessForDecodingH264(uProcessIdx, uStreamBufStAddr, STREAM_BUF_SIZE, 
				uFrameBufStAddr, bDecRotModeEn, bH264ReorderEn);
		else if (eDecMode == VC1_DEC)
			MFC_InitProcessForDecodingVc1(uProcessIdx, uStreamBufStAddr, STREAM_BUF_SIZE, 
				uFrameBufStAddr, bDecRotModeEn);
		else
			Assert(0);
		MFC_GetProcessBufEndAddr(uProcessIdx, &uFreeMemBuf);
		MFC_SetWrPtr(uProcessIdx, uStreamBufStAddr+nReadSize);
		INTC_SetVectAddr(NUM_MFC,Isr_Mfc);
		INTC_Enable(NUM_MFC);

		MFC_GetSrcPicSize(uProcessIdx, &picX, &picY);
		MFC_GetFrameSize(uProcessIdx, &uWidth, &uHeight);	
			
		// YUV read buffer
		uYuvBufAddr = uFreeMemBuf;
		uYuvFrameSize = picX * picY * 3/2;
		uFileOffset = 0;
		uFreeMemBuf += (uYuvFrameSize/512 + 2) * 512;
		
		uMaxAbsDiff=0;
		uFrameCount = 0;
				
		while (GetKey() == 0)   // exit when any key is pressed.
		{
			// 4. Decode one frame
			//---------------------

			bStreamBufEmpty = false;	
			bMfcDone = false;
			MFC_SetDecRotEn(false);
			MFC_StartDecodingOneFrame(uProcessIdx);
			while(bMfcDone != true);

			if (MFC_DoPostProcessingOfDecoding(uProcessIdx, bStreamBufEmpty, &uFrameIdx) == false)
				break;
			MFC_GetAddrOfYFrame(uProcessIdx, uFrameIdx, &uFrameAddrY);

			uFrameCount++;

			// 5. Load stream if necessary
			//------------------------------

			if (nDecFileSize == uStreamPtr)
			{					
				MFC_NotifyNoMoreStream(uProcessIdx);
			}
			else if (MFC_IsMoreStreamNeeded(uProcessIdx, nReadUnit))
			{
				nReadSize = (nDecFileSize-uStreamPtr < nReadUnit) ? nDecFileSize-uStreamPtr : nReadUnit;
				if (!FAT_ReadFile4(sel, uStreamPtr/nBlkSz, (nReadSize+nBlkSz-1)/nBlkSz, uStreamBufStAddr+uStreamPtr%STREAM_BUF_SIZE, &oSdhc))
					Assert(0);
				uStreamPtr += nReadSize;
				MFC_SetWrPtr(uProcessIdx, uStreamBufStAddr+uStreamPtr%STREAM_BUF_SIZE);
			}
			
			// 6. Read one YUV frame from SD/MMC card
			//----------------------------------------

			if (!FAT_ReadFile6(selYuv, uFileOffset, uYuvFrameSize, uYuvBufAddr, &uReturnAddr, &oSdhc))
				break;

			// 7. Compare the decoded frame with the golden one
			//---------------------------------------------------------

			if (!CompareFrameBuffers(uReturnAddr, picX, picY, uFrameAddrY, uWidth, uHeight, &uMaxAbsDiff, &uAbsDiff))
			{
				MFC_GetFrameCount(&uFrmNum);
				MFC_GetDecErrorMbNum(&uErrMbNum);
				//UART_Printf("%dth frame: max diff=%d, Err MB num=%d\n",uFrmNum, uMaxAbsDiff, uErrMbNum);
				if(uErrMbNum!=0)
				{
					UART_Printf("%dth frame: max diff=%d, Err MB num=%d\n",uFrmNum, uMaxAbsDiff, uErrMbNum);
					bConformanceError=true;
					break;
				}
			}

			uFileOffset += uYuvFrameSize;
			
			if((uFrameCount%10)==0)
				Putc('.');
		}

		// 8. Report the maximum difference
		//----------------------------------

		//UART_Printf("\n::: Max Abs Diff: %d among the %d frames\n", uMaxAbsDiff, uFrameCount);
		
		UART_Printf("\n%s:",pFileName);
		if(bConformanceError==false)
			UART_Printf("O.K\n");
		else
		{
			UART_Printf("Fail\n");
			break;
		}	
		
		MFC_IssueSeqEndCmd(uProcessIdx);
		MFC_StopBitProcessor();
	}

	FAT_UnloadFileSystem(&oSdhc);
	SDHC_CloseMedia(&oSdhc);
	
}
#endif


//////////
// Function Name : TestEncodingParameters
// Function Description : This function encodes a YUV file and then 
//							compares the encoded stream file with pre-made stream file
// Input : NONE
// Output : NONE
// Version : 
#if (USE_SD_STORAGE)
static void TestEncodingParameters(void)
{
	u32 uFreeMemBuf = CODEC_MEM_ST;
	char pFileName[256];
	u32 uTotalNumOfFiles;
	u32 i;
	int sel;
	u32 uConfigFileSize;
	u32 uConfigFileStAddr;
	u32 uProcessIdx;
	u32 uWidth, uHeight;
	u32 uNumOfFrames;
	u32 uFrameRate;
	char pGoldenStreamFileName[256];
	u32 uSrcYuvBufAddr;
	u32 uSrcYuvFrameSizeInBytes;
	s32 selSrcYuv;
	char pSrcYuvFileName[256];
	u32 uEncBufAddr;
	s32 selGoldenStream;
	s32 nGoldenStreamFileSize;
	u32 uGoldenStreamStAddr;
	const int uBlkSz = 512;
	int nReadSize;
	int nGoldenStreamOffset = 0;
	u32 uTrueStAddrOfData;
	u32 uSrcFileOffset=0;	
	u32 uFrameCount=0;
	u32 uEncStreamOffset = 0;
	MFC_CODEC_MODE eMode;		
	u8 bResult = true;
	u8 bIsNoMoreStream;

	// 1. Move firmware and init base settings.
	//-------------------------------------------
	
	MFC_MoveFirmwareToCodeBuf(uFreeMemBuf);
	MFC_GetCodeBufEndAddr(&uFreeMemBuf);		
   	MFC_StartBitProcessor();

	MFC_InitBaseForProcesses1(uFreeMemBuf, ENCODING_WITH_ALL_PARAMETERS);	
	MFC_GetBaseBufEndAddr(&uFreeMemBuf);
	MFC_SetFlushingStreamBufModeInEncoding();

	// 2. Read a config file.
	//------------------------
	
	while(!SDHC_OpenMedia(SDHC_HCLK, &oSdhc));	
	if (!FAT_LoadFileSystem(&oSdhc))
		Assert(0);

	FAT_GetTotalNumOfFiles(&uTotalNumOfFiles, &oSdhc);
	for (i=0; i<uTotalNumOfFiles; i++)
	{
		FAT_GetFileName(i, pFileName, &oSdhc);
		UART_Printf("%02d -- %s\n", i, pFileName);
	}
	
	UART_Printf(" Which config file to read ? ");
	sel = UART_GetIntNum();

	uConfigFileStAddr = uFreeMemBuf;	
	if (!FAT_ReadFile2(sel, uConfigFileStAddr, &uConfigFileSize, &oSdhc))
		Assert(0);

	uFreeMemBuf = (uFreeMemBuf+uConfigFileSize+STREAM_WR_SIZE-1)/STREAM_WR_SIZE*STREAM_WR_SIZE;

	// 3. Parse the config file and set parameters.
	//----------------------------------------------

	uProcessIdx = 0;
	MFC_ParseConfigFileAndSetParameters(uProcessIdx, uConfigFileStAddr);

	MFC_GetEncPicSizeCfg(uProcessIdx, &uWidth, &uHeight);
	MFC_GetEncNumOfFramesCfg(uProcessIdx, &uNumOfFrames);
	MFC_GetEncFrameRateCfg(uProcessIdx, &uFrameRate);

	MFC_GetGoldenStreamFileName(uProcessIdx, pGoldenStreamFileName);
	MFC_GetCodecModeFromName(pGoldenStreamFileName, false, &eMode);	

	// 4. Keep one frame sized memory of the source YUV file.
	//--------------------------------------------------------

	uSrcYuvBufAddr = uFreeMemBuf;
	uSrcYuvFrameSizeInBytes = uWidth * uHeight *3/2;
	uFreeMemBuf += (uSrcYuvFrameSizeInBytes/512 + 2) * 512;
	MFC_GetSrcYuvFileName(uProcessIdx, pSrcYuvFileName);
	FAT_GetFileIndex(pSrcYuvFileName, &selSrcYuv, &oSdhc);
	
	// 5. Init a process of MFC
	//--------------------------

	uEncBufAddr = (uFreeMemBuf+STREAM_WR_SIZE-1)/STREAM_WR_SIZE*STREAM_WR_SIZE;
	MFC_InitProcessForEncoding(
		uProcessIdx, eMode, uEncBufAddr, STREAM_BUF_SIZE, uWidth, uHeight, false);
	MFC_GetProcessBufEndAddr(uProcessIdx, &uFreeMemBuf);
	INTC_SetVectAddr(NUM_MFC,Isr_Mfc);
	INTC_Enable(NUM_MFC);


	// 6. Read the golden stream to compare
	//--------------------------------------

	FAT_GetFileIndex(pGoldenStreamFileName, &selGoldenStream, &oSdhc);

	FAT_GetFileSize(selGoldenStream, &nGoldenStreamFileSize, &oSdhc);
	UART_Printf("golden stream size = %d bytes\n", nGoldenStreamFileSize);
	 
	uGoldenStreamStAddr = uFreeMemBuf;
	uFreeMemBuf += nGoldenStreamFileSize;

	nReadSize = (nGoldenStreamFileSize < STREAM_BUF_SIZE/2) ? nGoldenStreamFileSize : STREAM_BUF_SIZE/2;
	nGoldenStreamOffset = 0;	

	if (!FAT_ReadFile4(selGoldenStream, nGoldenStreamOffset, (nReadSize+uBlkSz-1)/uBlkSz, uGoldenStreamStAddr, &oSdhc))
		Assert(0);	
	nGoldenStreamOffset += nReadSize;

	uSrcFileOffset=0;	
	uFrameCount=0;
	uEncStreamOffset = 0;
		
	bResult = true;
	bIsNoMoreStream = (nReadSize == nGoldenStreamFileSize) ? true : false;
	
	while (GetKey() == 0)
	{	
		// 7. Read one Yuv source frame from SD/MMC card
		//-----------------------------------------------

		if (!FAT_ReadFile6(selSrcYuv, uSrcFileOffset, uSrcYuvFrameSizeInBytes, uSrcYuvBufAddr, &uTrueStAddrOfData, &oSdhc))
			break;
		uSrcFileOffset += uSrcYuvFrameSizeInBytes;

		// 8. Encode one frame
		//---------------------

		bMfcDone = false;
		MFC_SetEncRunOption(false, false);
		MFC_SetEncRotEn(false);
		MFC_StartEncodingOneFrame(uProcessIdx, uTrueStAddrOfData);
		while(bMfcDone != true);		
		
		// 9. Compare the encoded stream with the golden one, Load the golden stream if necessary
		//----------------------------------------------------------------------------------------

		uFrameCount++;
		UART_Printf("%d ",uFrameCount);

		if (!MFC_IsMoreStreamNeeded(uProcessIdx, nReadSize) && !bIsNoMoreStream)
		{
			UART_Printf(" more ");
			if (!Compare128(uEncBufAddr+(uEncStreamOffset%STREAM_BUF_SIZE), 
					uGoldenStreamStAddr+(uEncStreamOffset%STREAM_BUF_SIZE), nReadSize/16))
			{
				bResult = false;
				break;
			}
			uEncStreamOffset += nReadSize;
			
			nReadSize = (nGoldenStreamFileSize-nGoldenStreamOffset < STREAM_BUF_SIZE/2) ? 
				nGoldenStreamFileSize-nGoldenStreamOffset : STREAM_BUF_SIZE/2;
			bIsNoMoreStream = (nReadSize == nGoldenStreamFileSize-nGoldenStreamOffset) ? true : false;
			if (!FAT_ReadFile4(selGoldenStream, nGoldenStreamOffset/uBlkSz, 
				(nReadSize+uBlkSz-1)/uBlkSz, uGoldenStreamStAddr+(nGoldenStreamOffset%STREAM_BUF_SIZE), &oSdhc))
				Assert(0);
			nGoldenStreamOffset += nReadSize;

			MFC_SetRdPtr(uProcessIdx, uEncBufAddr+((nGoldenStreamOffset+STREAM_BUF_SIZE/2)%STREAM_BUF_SIZE));			
		}
			
		if (uFrameCount >= uNumOfFrames)		// end of YUV frame
		{
			UART_Printf("Encoding ended with %d frames\n", uNumOfFrames);
			if (!Compare128(uEncBufAddr+uEncStreamOffset%STREAM_BUF_SIZE, 
					uGoldenStreamStAddr+uEncStreamOffset%STREAM_BUF_SIZE, nReadSize/16))
			{
				bResult = false;
			}			
			UART_Printf("Rate control: %.2fkbps\n", 
				(float)nGoldenStreamFileSize*8/uNumOfFrames*uFrameRate/1000);	
			break;
		}			
	}	

	// 10. Report the result of comparison
	//-------------------------------------

	UART_Printf("\n%s\n\n", bResult ? "Match" : "Mismatch");

	FAT_UnloadFileSystem(&oSdhc);
	SDHC_CloseMedia(&oSdhc);
	
	MFC_IssueSeqEndCmd(uProcessIdx);	
	MFC_StopBitProcessor();
}
#endif

//////////
// Function Name : SetEncMode
// Function Description : This function sets encoder mode
// Input : sSel, file number to select
// Output : NONE
// Version : 
const char *pEncFile[]=
{
	"foreman_qcif_300.yuv",
	"mobile_qcif_300.yuv",
	"foreman_cif_300.yuv",
	"mobile_cif_300.yuv",
	"city_vga_300.yuv",
	"football_sd601_260.yuv",
	"mobile_sd601_260.yuv",
	0
};
static void SetEncMode(s32 *sSel)
{
	int sEncMode=-1;
#if (USE_SD_STORAGE)
	s32 sTmpValue=0;
#else
	int sEncFile=-1;
	u8 i=0;
#endif	

#if USE_SD_STORAGE
	UART_Printf("Select the Encoder File : ");
	sTmpValue = UART_GetIntNum();
	FAT_GetFileName(sTmpValue, pSrcYUVFileName, &oSdhc);
	*sSel = sTmpValue;
	
	UART_Printf("Enter the Encoder Width : ");
	sTmpValue = UART_GetIntNum();
	if(sTmpValue != -1)
	{
		uEncWidth = sTmpValue;
	}
	
	UART_Printf("Enter the Encoder Height : ");
	sTmpValue = UART_GetIntNum();
	if(sTmpValue != -1)
	{
		uEncHeight = sTmpValue;
	}
	
	UART_Printf("Enter the Encoder Total Frame Number : ");
	sTmpValue = UART_GetIntNum();
	if(sTmpValue != -1)
	{
		uEncTotalFrameNum = sTmpValue;
	}
#else
	do
	{
		UART_Printf("Select Encoder File: \n");

		for(i=0;pEncFile[i]!=0;i++)
		{
			UART_Printf("[%d] %s\n",i,pEncFile[i]);
		}
		
		sEncFile = UART_GetIntNum();	
	} while(sEncFile == -1);

	switch(sEncFile)
	{
		default:
		case 0:
			strncpy(pSrcYUVFileName, pEncFile[0], 256);
			uEncWidth = 176;
			uEncHeight = 144;
			uEncTotalFrameNum = 300;
			break;
			
		case 1:
			strncpy(pSrcYUVFileName, pEncFile[1], 256);
			uEncWidth = 176;
			uEncHeight = 144;
			uEncTotalFrameNum = 300;
			break;
			
		case 2:
			strncpy(pSrcYUVFileName, pEncFile[2], 256);
			uEncWidth = 352;
			uEncHeight = 288;
			uEncTotalFrameNum = 300;
			break;
			
		case 3:
			strncpy(pSrcYUVFileName, pEncFile[3], 256);
			uEncWidth = 352;
			uEncHeight = 288;
			uEncTotalFrameNum = 300;
			break;
			
		case 4:
			strncpy(pSrcYUVFileName, pEncFile[4], 256);
			uEncWidth = 640;
			uEncHeight = 480;
			uEncTotalFrameNum = 300;
			break;
			
		case 5:
			strncpy(pSrcYUVFileName, pEncFile[5], 256);
			uEncWidth = 720;
			uEncHeight = 480;
			uEncTotalFrameNum = 260;
			break;
			
		case 6:
			strncpy(pSrcYUVFileName, pEncFile[6], 256);
			uEncWidth = 720;
			uEncHeight = 480;
			uEncTotalFrameNum = 260;
			break;
			
	}
#endif	
	
	do
	{
		UART_Printf("Select Encoder mode: \n");
		UART_Printf("[0] MPEG4\n");
		UART_Printf("[1] H.263\n");
		UART_Printf("[2] H.264\n");
		sEncMode = UART_GetIntNum();		
	} while (sEncMode == -1);

	eEncMode = (sEncMode == 2) ? AVC_ENC : MP4_ENC;
	bIsH263 = (sEncMode == 1) ? true : false;

}

//////////
// Function Name : TestEncoding
// Function Description : This function encodes an YUV file and displays the recon image
// Input : NONE
// Output : NONE
// Version : 
static void TestEncoding(void)
{
	u32 uFreeMemBuf = MFC_MEMORY_BASE;
	u32 uProcessIdx;
	u32 uYuvFrameSize;
	const char* pEncModeName;
	u32 uYuvBufAddr;
	s32 sel;
#if (!USE_SD_STORAGE)
	u32 uYuvFileAddr, uYuvFileSize;	
#endif	
	u32 uTransferSize;
	const int nWriteUnit = STREAM_BUF_SIZE/2;
	u32 uStreamOffset = 0;
	u32 uSaveBufAddr;
	u32 uStreamBufStAddr;
	u32 uLcdFbAddr;
	CSPACE eLcdBpp;
	u32 uLcdWidth, uLcdHeight;
	u32 uDecWidth, uDecHeight;
	u32 uRecFrmAddrY;
	u32 uDispFrameNum;
	u32 uFrameCount=0;
	u32 uSrcYuvFileOffset = 0;
	u32 uWrPtr;
	u32 uTrueStAddrOfData;
	u32 uRecFrameIdx;
	float fEncTime=0;
	double fTotEncTime;
	
#if USE_SD_STORAGE
	// 0. Select the YUV file in the SD card.
	//-------------------------------------------
	while(!SDHC_OpenMedia(SDHC_HCLK, &oSdhc));
	if (!FAT_LoadFileSystem(&oSdhc))
		Assert(0);
		
	SetEncMode(&sel);
	
	if(sel == -1)
	{
		UART_Printf("There is the matched file in the SD card\n");
		return;
	}
#else
	// 0. Download the YUV file thru USB.
	//-------------------------------------------
	SetEncMode(&sel);
	
	uYuvFileAddr = uFreeMemBuf;
	UART_Printf(" Before testing, download \"%s\" file through USB \n", pSrcYUVFileName);
	uYuvFileSize = DownloadImageThruUsbOtg((u8 *)uYuvFileAddr);
	if (uYuvFileSize == 0)
		return;
	uFreeMemBuf += uYuvFileSize;
	uFreeMemBuf = (uFreeMemBuf+STREAM_WR_SIZE-1)/STREAM_WR_SIZE*STREAM_WR_SIZE;
#endif

	// 1. Move firmware and init base settings.
	//-------------------------------------------
	
	MFC_MoveFirmwareToCodeBuf(uFreeMemBuf);
	MFC_GetCodeBufEndAddr(&uFreeMemBuf);	
	MFC_StartBitProcessor();
	
	MFC_InitBaseForProcesses(uFreeMemBuf);
	MFC_GetBaseBufEndAddr(&uFreeMemBuf);
	MFC_SetNoFlushingStreamBufModeInEncoding();

	// 2. Set encoding parameters.
	//-----------------------------

	uProcessIdx = 0;

	MFC_SetEncYuvFileCfg(uProcessIdx, pSrcYUVFileName, uEncTotalFrameNum, uEncWidth, uEncHeight, 30);
	MFC_SetEncSliceCfg(uProcessIdx, 1, 0, 4000);
	MFC_SetEncErrResilienceCfg(uProcessIdx, 0);

	if (eEncMode == MP4_ENC)
	{
		if (bIsH263)
			MFC_SetEnc263Cfg(uProcessIdx, 0, 1, 0, 0, 10, 15);
		else
			MFC_SetEncMpeg4Cfg(uProcessIdx, 0, 0, 0, 10, 15);
	}
	else if (eEncMode == AVC_ENC)
	{
		MFC_SetEnc264Cfg(uProcessIdx, 0, 0, 0, 0, 0, 15, 15);
	}
	MFC_SetEncRateCtrlCfg(uProcessIdx, 1, 128, 0, 0);

	uYuvFrameSize = uEncWidth * uEncHeight * 3/2;	

	pEncModeName =
		(eEncMode == MP4_ENC && bIsH263) ? "H.263" :
		(eEncMode == MP4_ENC && !bIsH263) ? "MPEG4" : "H.264";
	UART_Printf("Source file Name: %s, enc mode: %s, w:%d, h:%d, uEncTotalFrameNum:%d\n",
		pSrcYUVFileName, pEncModeName, uEncWidth,uEncHeight,uEncTotalFrameNum);

	// 3. Keep one frame sized memory and index of the selected YUV file
	//-----------------------------------------------------------------

	uYuvBufAddr = uFreeMemBuf;
	uFreeMemBuf += (uYuvFrameSize/512 + 2) * 512;

	// 4. Init a process of MFC
	//--------------------------
	uFreeMemBuf = (uFreeMemBuf+STREAM_WR_SIZE-1)/STREAM_WR_SIZE*STREAM_WR_SIZE;
	uStreamBufStAddr = uFreeMemBuf;
	MFC_InitProcessForEncoding(
		uProcessIdx, eEncMode, uStreamBufStAddr, STREAM_BUF_SIZE, uEncWidth, uEncHeight, false);
	MFC_GetProcessBufEndAddr(uProcessIdx, &uFreeMemBuf);
	INTC_SetVectAddr(NUM_MFC,Isr_Mfc);
	INTC_Enable(NUM_MFC);	

	// 5. Init LCDC
	//---------------

	uLcdFbAddr = uFreeMemBuf;
	eLcdBpp = RGB24;
	LCD_InitDISPC(eLcdBpp, uLcdFbAddr, WIN0, false);
	LCD_GetFbEndAddr(&uFreeMemBuf, WIN0);

	LCD_GetFrmSz(&uLcdWidth, &uLcdHeight, WIN0);
	uDecWidth = uLcdWidth;
	uDecHeight = uLcdHeight*9/10;

	GLIB_InitInstance(uLcdFbAddr, uLcdWidth, uLcdHeight, eLcdBpp);
	GLIB_ClearFrame(C_BLUE);
	GLIB_SetFontColor(C_WHITE, C_BLUE, false);
	GLIB_Printf(2, uDecHeight+6, "S3C6410 MFC Encoder(%s)",
		(eEncMode == MP4_ENC && bIsH263) ? "H.263" : (eEncMode == MP4_ENC && !bIsH263) ? "MPEG4" : "H.264");

	// 6. Init POST
	//---------------
		
	MFC_GetAddrOfYFrame(uProcessIdx, 0, &uRecFrmAddrY); // Start addr of MFC's output frame buffers
	MFC_GetDispFrameNum(uProcessIdx, &uDispFrameNum);
	
	POST_InitIp1(
		uEncWidth, uEncHeight, 0, 0, uEncWidth, uEncHeight, uRecFrmAddrY, YC420,
		uLcdWidth, uLcdHeight, 0, 0, uDecWidth, uDecHeight, uLcdFbAddr, eLcdBpp,
		uDispFrameNum, false, ONE_SHOT, POST_DMA, POST_DMA, &oPost);
	INTC_SetVectAddr(NUM_POST0, Isr_Post);
	INTC_Enable(NUM_POST0);
	POST_EnableInterrupt(POST_LEVEL_INT, &oPost);

	fTotEncTime=0;
	uFrameCount=0;
	uSrcYuvFileOffset = 0;
	uSaveBufAddr = uFreeMemBuf;
	uFreeMemBuf += STREAM_BUF_SIZE*4;
	
	LCD_Start();
		
	while (GetKey() == 0)
	{
		// 7. Read one frame of YUV image from SD card
		//---------------------------------------------
#if USE_SD_STORAGE
		if (!FAT_ReadFile6(sel, uSrcYuvFileOffset, uYuvFrameSize, uYuvBufAddr, &uTrueStAddrOfData, &oSdhc))
			break;
#else
		if((uSrcYuvFileOffset+uYuvFrameSize)<=uYuvFileSize)
		{
			memcpy((void *)uYuvBufAddr, (void *)(uYuvFileAddr+uSrcYuvFileOffset), uYuvFrameSize);
			uTrueStAddrOfData = uYuvBufAddr;
		}
		else
			break;
#endif
		uSrcYuvFileOffset += uYuvFrameSize;	

		// 8. Encode one frame
		//---------------------

		StartTimer(0);
		
		bMfcDone = false;
		MFC_SetEncRunOption(false, false);
		MFC_StartEncodingOneFrame(uProcessIdx, uTrueStAddrOfData);
		while(bMfcDone != true);

		MFC_DoPostProcessingOfEncoding(uProcessIdx, &uRecFrameIdx);
		
		fEncTime = StopTimer(0);	
		fTotEncTime += fEncTime;

		// 9. Save the encoded stream of the size of the write unit
		//----------------------------------------------------------
		uFrameCount++;
		MFC_GetWrPtr(uProcessIdx, &uWrPtr);

		if (uWrPtr > uStreamBufStAddr+((uStreamOffset+nWriteUnit)%STREAM_BUF_SIZE)+STREAM_WR_SIZE)
		{
			//Copy32(uStreamBufStAddr+(uStreamOffset%STREAM_BUF_SIZE), uSaveBufAddr+uStreamOffset, nWriteUnit/4); 
			memcpy((void *)(uSaveBufAddr+uStreamOffset), (void *)(uStreamBufStAddr+(uStreamOffset%STREAM_BUF_SIZE)), nWriteUnit);

			uStreamOffset += nWriteUnit;			
			MFC_SetRdPtr(uProcessIdx, uWrPtr-STREAM_WR_SIZE); // to avoid the full status
		}

		// 10. Convert the frame for display for verifying
		//-------------------------------------------------
		
		bPostDone = false;
		POST_StartProcessing1(uRecFrameIdx, 0, &oPost);
		while(bPostDone == false);

		LCD_Trigger();

		if (uFrameCount == uEncTotalFrameNum)
		{
			UART_Printf("Encoding is ended with %d frames\n", uFrameCount);
			break;
		}
	}

	MFC_IssueSeqEndCmd(uProcessIdx);

	// As we called MFC_SetNoFlushingStreamBufModeInEncoding(),
	// we need copy operation once more after calling MFC_IssueSeqEndCmd().
	MFC_GetWrPtr(uProcessIdx, &uWrPtr);
	uTransferSize = uWrPtr - (uStreamBufStAddr+(uStreamOffset%STREAM_BUF_SIZE));
	//Copy32(uStreamBufStAddr+(uStreamOffset%STREAM_BUF_SIZE), uSaveBufAddr+uStreamOffset, (uTransferSize+3)/4); 
	memcpy((void *)(uSaveBufAddr+uStreamOffset), (void *)(uStreamBufStAddr+(uStreamOffset%STREAM_BUF_SIZE)), uTransferSize);
	uStreamOffset += uTransferSize;
	
	UART_Printf("FPS=%.2f, average elapsed time= %.2f(ms), total %d frames\n", 
		1000000*uFrameCount/fTotEncTime, fTotEncTime/uFrameCount/1000, uFrameCount);
	
#if USE_SD_STORAGE

	#if 1
	{
		char pEncFileName[256];
		
		UART_Printf("Write down the file name to save the encoded stream to SD/MMC card : \n");
		UART_Printf("(Press just 'Enter' key, if you don't want to save)\n");
		gets(pEncFileName);
		
		if(pEncFileName[0]!='\0')
			FAT_WriteFile(pEncFileName, uStreamOffset, uSaveBufAddr, &oSdhc);
	}
	#endif	

	FAT_UnloadFileSystem(&oSdhc);
	SDHC_CloseMedia(&oSdhc);
	
#else
	UART_Printf("\n Target Address : 0x%08x!!!\n",uSaveBufAddr);
	UART_Printf(" Upload Size : 0x%x!!!\n",uStreamOffset);

	UploadImageThruUsbOtg();
#endif	
	
	MFC_StopBitProcessor();

}

//////////
// Function Name : TestDecodingWithAllRotModes
// Function Description : This function decodes an stream file of all rotation modes and displays the generated YUV
// Input : NONE
// Output : NONE
// Version :
static void TestDecodingWithAllRotModes(void)
{
	u8 bHorMirEn[2] = {false, true};
	u8 bVerMirEn[2] = {false, true};
	u32 uRotDeg[4] = {0,90,180,270};
	u32 i, j, k;

	bH264ReorderEn = false;
	bAllRotating = true;
	bAllRotateFirst = true;

	for (i=0; i<2; i++)
	for (j=0; j<2; j++)
	for (k=0; k<4; k++)
	{
		bDecRotModeEn = true;
		
		bDecMirHor = bHorMirEn[i];
		bDecMirVer = bVerMirEn[j];		
		uDecRotDeg = uRotDeg[k];
		UART_Printf("H mirror:%s, V mirror:=%s, Rot Degree=%d\n",
			(i==0)?"No":"Yes", (j==0)?"No":"Yes", uRotDeg[k]);
		TestDecoding();
		bAllRotateFirst = false;
	}

	bDecRotModeEn = false;
	uDecRotDeg = 0;
	bDecMirHor = false;
	bDecMirVer = false;	
	bAllRotating = false;
	bAllRotateFirst = false;
	
}

//////////
// Function Name : TestEncodingWithAllRotModes
// Function Description : This function encodes an YUV file of all rotation modes and displays the recon image
// Input : NONE
// Output : NONE
// Version :
static void TestEncodingWithAllRotModes(void)
{
	u32 uFreeMemBuf = MFC_MEMORY_BASE;
	u32 uProcessIdx;
	const char* pSrcFileName = "mobile_qcif_300.yuv";
	u32 uYuvFrameSize;
	u32 uYuvBufAddr;
	u32 uLcdFbAddr;
	CSPACE eLcdBpp;	
	s32 sel;
#if (!USE_SD_STORAGE)
	u32 uYuvFileAddr, uYuvFileSize;	
#endif
	u32 uDecWidth;
	u32 uDecHeight;
	u32 uLcdWidth, uLcdHeight;
	u32 uEncBufAddr;
	u8 bEncRotEn = true;
	u8 bHorMirEn[2] = {false, true};
	u8 bVerMirEn[2] = {false, true};
	u32 uRotDeg[4] = {0,90,180,270};
	u32 i, j, k;
	u8 bIsRot90;
	u32 uDispFrameNum;
	u32 uRecFrmAddrY;
	u32 uRotWidth;
	u32 uRotHeight;
	u32 recFrameIdx;
	u32 frameCount=0;
	u32 uFileOffset = 0;
	u32 uTrueStAddrOfData;
	
#if USE_SD_STORAGE
	// 0. Select the YUV file in the SD card.
	//-------------------------------------------
	while(!SDHC_OpenMedia(SDHC_HCLK, &oSdhc));
	if (!FAT_LoadFileSystem(&oSdhc))
		Assert(0);
		
	SetEncMode(&sel);
	
	if(sel == -1)
	{
		UART_Printf("There is the matched file in the SD card\n");
		return;
	}
#else
	// 0. Download the YUV file thru USB.
	//-------------------------------------------
	SetEncMode(&sel);
	
	uYuvFileAddr = uFreeMemBuf;
	UART_Printf(" Before testing, download \"%s\" file through USB \n", pSrcYUVFileName);
	uYuvFileSize = DownloadImageThruUsbOtg((u8 *)uYuvFileAddr);
	if (uYuvFileSize == 0)
		return;
	uFreeMemBuf += uYuvFileSize;
	uFreeMemBuf = (uFreeMemBuf+STREAM_WR_SIZE-1)/STREAM_WR_SIZE*STREAM_WR_SIZE;
#endif

	// 1. Move firmware and init base settings.
	//-------------------------------------------
	
	MFC_MoveFirmwareToCodeBuf(uFreeMemBuf);
	MFC_GetCodeBufEndAddr(&uFreeMemBuf);	
	MFC_StartBitProcessor();

	MFC_InitBaseForProcesses(uFreeMemBuf);
	MFC_GetBaseBufEndAddr(&uFreeMemBuf);

	// 2. Set encoding parameters.
	//-----------------------------

	uProcessIdx = 0;

	MFC_SetEncYuvFileCfg(uProcessIdx, pSrcFileName, uEncTotalFrameNum, uEncWidth, uEncHeight, 30);
	MFC_SetEncSliceCfg(uProcessIdx, 1, 0, 4000);
	MFC_SetEncErrResilienceCfg(uProcessIdx, 0);

	if (eEncMode == MP4_ENC)
	{
		if (bIsH263)
			MFC_SetEnc263Cfg(uProcessIdx, 0, 1, 0, 0, 10, 15);
		else
			MFC_SetEncMpeg4Cfg(uProcessIdx, 0, 0, 0, 10, 15);
	}
	else if (eEncMode == AVC_ENC)
	{
		MFC_SetEnc264Cfg(uProcessIdx, 0, 0, 0, 0, 0, 15, 15);
	}
	MFC_SetEncRateCtrlCfg(uProcessIdx, 1, 128, 0, 0);
	
	uYuvFrameSize = uEncWidth * uEncHeight * 3/2;		

	// 3. Keep one frame sized memory and index of the selected YUV file
	//-------------------------------------------------------------------

	uYuvBufAddr = uFreeMemBuf;
	uFreeMemBuf += (uYuvFrameSize/512 + 2) * 512;

	// 4. Init LCDC
	//---------------
	
	uLcdFbAddr = uFreeMemBuf;
	eLcdBpp = RGB24;	
	LCD_InitDISPC(eLcdBpp, uLcdFbAddr, WIN0, false);
	LCD_GetFbEndAddr(&uFreeMemBuf, WIN0);

	LCD_GetFrmSz(&uLcdWidth, &uLcdHeight, WIN0);
	uDecWidth = uLcdWidth;
	uDecHeight = uLcdHeight*9/10;

	GLIB_InitInstance(uLcdFbAddr, uLcdWidth, uLcdHeight, eLcdBpp);
	GLIB_ClearFrame(C_BLUE);
	GLIB_SetFontColor(C_WHITE, C_BLUE, false);
	GLIB_Printf(
		2, uDecHeight+6, "S3C6410 MFC Rotation Encoder(%s)",
		(eEncMode == MP4_ENC) ? "MPEG4" : (eEncMode == AVC_ENC) ? "H.264" : "UNKNOWN");

	INTC_SetVectAddr(NUM_POST0, Isr_Post);
	INTC_Enable(NUM_POST0);
	POST_EnableInterrupt(POST_LEVEL_INT, &oPost);
	
	uEncBufAddr = uFreeMemBuf;
	
	LCD_Start();

	for (i=0; i<2; i++)
	for (j=0; j<2; j++)
	for (k=0; k<4; k++)
	{
		// 5. Init a process of MFC
		//--------------------------
	
		bIsRot90 = (uRotDeg[k]%180 == 0) ? false : true;
		
		MFC_InitProcessForEncoding(
			uProcessIdx, eEncMode, uEncBufAddr, STREAM_BUF_SIZE,
			uEncWidth, uEncHeight, bIsRot90);
		MFC_GetProcessBufEndAddr(uProcessIdx, &uFreeMemBuf);

		// 6. Init POST
		//---------------
				
		MFC_GetDispFrameNum(uProcessIdx, &uDispFrameNum);

		MFC_GetAddrOfYFrame(uProcessIdx, 0, &uRecFrmAddrY); // Start addr of MFC's output frame buffers
		UART_Printf("H mirror:%s, V mirror:=%s, Rot Degree=%d\n",
			(i==0)?"No":"Yes", (j==0)?"No":"Yes", uRotDeg[k]);
		uRotWidth = (uRotDeg[k]%180 == 0) ? uEncWidth : uEncHeight;
		uRotHeight = (uRotDeg[k]%180 == 0) ? uEncHeight : uEncWidth;
		
		POST_InitIp1(
			uRotWidth, uRotHeight, 0, 0, uRotWidth, uRotHeight,	uRecFrmAddrY, YC420,
			uLcdWidth, uLcdHeight, 0, 0, uDecWidth, uDecHeight, uLcdFbAddr, eLcdBpp,
			uDispFrameNum, false, ONE_SHOT, POST_DMA, POST_DMA, &oPost);

		frameCount=0;
		uFileOffset = 0;
		
		while (GetKey() == 0)
		{
			// 7. Read one frame of YUV image from SD card
			//---------------------------------------------

#if USE_SD_STORAGE
			if (!FAT_ReadFile6(sel, uFileOffset, uYuvFrameSize, uYuvBufAddr, &uTrueStAddrOfData, &oSdhc))
				break;
#else
			if((uFileOffset+uYuvFrameSize)<=uYuvFileSize)
			{
				memcpy((void *)uYuvBufAddr, (void *)(uYuvFileAddr+uFileOffset), uYuvFrameSize);
				uTrueStAddrOfData = uYuvBufAddr;
			}
			else
				break;
#endif
			uFileOffset += uYuvFrameSize;	

			// 8. Encode one frame
			//---------------------

			MFC_SetEncRunOption(false, false);
			MFC_SetEncRotEn(bEncRotEn);
			MFC_SetEncRotationMode(bHorMirEn[i], bVerMirEn[j], uRotDeg[k]);			
			MFC_EncodeOneFrame(uProcessIdx, uTrueStAddrOfData, &recFrameIdx);

			frameCount++;

			// 9. Convert the frame for display for verifying
			//-------------------------------------------------

			bPostDone = false;
			POST_StartProcessing1(recFrameIdx, 0, &oPost);
			while(bPostDone == false);

			LCD_Trigger();
			
			if (frameCount == uEncTotalFrameNum)		// end of YUV frame
				break;
			
		}
		
		MFC_IssueSeqEndCmd(uProcessIdx);
	}
#if USE_SD_STORAGE
	FAT_UnloadFileSystem(&oSdhc);
	SDHC_CloseMedia(&oSdhc);
#endif	
}

//////////
// Function Name : TestMultipleDecoding
// Function Description : This function decodes multiple stream files and displays the generated YUV
// Input : NONE
// Output : NONE
// Version : 
#if (USE_SD_STORAGE)
static void TestMultipleDecoding(void)
{
	u32 uFreeMemBuf = MFC_MEMORY_BASE;
	u32 uLcdFbAddr;
	CSPACE eLcdBpp;
	u32 uLcdWidth, uLcdHeight;
	s32 uFileSize[MAX_PROCESS_NUM];
	u32 uStreamBufStAddr[MAX_PROCESS_NUM];
	u32 uStreamOffset[MAX_PROCESS_NUM];
	int nReadSize[MAX_PROCESS_NUM];
	u32 uFrameBufStAddr[MAX_PROCESS_NUM];
	u32 picX[MAX_PROCESS_NUM];
	u32 picY[MAX_PROCESS_NUM];
	u32 uRotWidth[MAX_PROCESS_NUM];
	u32 uRotHeight[MAX_PROCESS_NUM];	
	MFC_CODEC_MODE eDecMode[MAX_PROCESS_NUM]; 
	u32 sel[MAX_PROCESS_NUM];
	u32 uIdx;
	u32 maxProcessNum;
	const int uBlkSz = 512;
	const int nReadUnit = STREAM_BUF_SIZE/2;
	u32 i,j;
	char pFileName[256];
	u32 uTotalNumOfFiles;
	char fileExt[50];
	u8 bFinishedProcess[MAX_PROCESS_NUM];
	u32 uFinishedProcessNum=0;
	u32 uTotFrame[MAX_PROCESS_NUM];
	u32 uFrameIdx;
	u32 uFrmAddrY;	
	u32 uDispHorizontalDiv;
	u32 uDispVerticalDiv;
	u32 uRotDispHorizontalDiv;
	u32 uRotDispVerticalDiv;
	u32 uRotDispWidth;
	u32 uRotDispHeight;
	u32 xPos;
	u32 yPos;

	// 1. Move firmware and init base settings.
	//-------------------------------------------
	
	MFC_MoveFirmwareToCodeBuf(uFreeMemBuf);
	MFC_GetCodeBufEndAddr(&uFreeMemBuf);	
	MFC_StartBitProcessor();
	
	MFC_InitBaseForProcesses(uFreeMemBuf);
	MFC_GetBaseBufEndAddr(&uFreeMemBuf);

	// 2. Init LCDC
	//---------------

	uLcdFbAddr = uFreeMemBuf;
	eLcdBpp = RGB24;	
	LCD_InitDISPC(eLcdBpp, uLcdFbAddr, WIN0, false);
	LCD_GetFbEndAddr(&uFreeMemBuf, WIN0); // Get the free mem buffer addr
	
	LCD_GetFrmSz(&uLcdWidth, &uLcdHeight, WIN0);
	
	GLIB_InitInstance(uLcdFbAddr, uLcdWidth, uLcdHeight, eLcdBpp);
	GLIB_ClearFrame(C_BLUE);
	GLIB_SetFontColor(C_WHITE, C_BLUE, false);
	GLIB_Printf(2, uLcdHeight*9/10+6, "S3C6410 MFC Multi Codec Test");
	
	INTC_SetVectAddr(NUM_POST0, Isr_Post);
	INTC_Enable(NUM_POST0);	
	POST_EnableInterrupt(POST_LEVEL_INT, &oPost);
	
	INTC_SetVectAddr(NUM_MFC,Isr_Mfc);
	INTC_Enable(NUM_MFC);
	
	UART_Printf("How many processes do you want for decoding[1~%d]?\n", MAX_PROCESS_NUM);
	maxProcessNum = UART_GetIntNum();

	while(!SDHC_OpenMedia(SDHC_HCLK, &oSdhc));
	if (!FAT_LoadFileSystem(&oSdhc))
		Assert(0);

	Assert(nReadUnit%uBlkSz == 0);
	
	for (i=0; i<maxProcessNum; i++)
	{
		uIdx = i;

		// 3. Reserve the stream buffer, & load the first part of a stream
		//-----------------------------------------------------------------	
		uStreamBufStAddr[uIdx] = uFreeMemBuf;
		uFreeMemBuf += STREAM_BUF_SIZE; // Reserved for stream buf.	
		Assert(uStreamBufStAddr[uIdx]%STREAM_WR_SIZE == 0);
			
		FAT_GetTotalNumOfFiles(&uTotalNumOfFiles, &oSdhc);
		for (j=0; j<uTotalNumOfFiles; j++)
		{
			FAT_GetFileName(j, pFileName, &oSdhc);
			UART_Printf("%02d -- %s\n", j, pFileName);
		}

		UART_Printf(" Which file to read ? ");
		sel[uIdx] = UART_GetIntNum();	

		FAT_GetFileSize(sel[uIdx], &uFileSize[uIdx], &oSdhc);
	
		nReadSize[uIdx] = (uFileSize[uIdx] < nReadUnit) ? uFileSize[uIdx] : nReadUnit;
		uStreamOffset[uIdx] = nReadSize[uIdx];
		if (!FAT_ReadFile4(sel[uIdx], 0, (nReadSize[uIdx]+uBlkSz-1)/uBlkSz, uStreamBufStAddr[uIdx], &oSdhc))
			Assert(0);	

		// 4. Init processes of MFC
		//--------------------------		
		FAT_GetFileExt(sel[uIdx], fileExt, &oSdhc);	
		MFC_GetCodecModeByName(fileExt, true, &eDecMode[uIdx]);

		bH264ReorderEn = true;
		uFrameBufStAddr[uIdx] = uFreeMemBuf;
		
		if (eDecMode[uIdx] == MP4_DEC)
			MFC_InitProcessForDecodingMpeg4(uIdx, uStreamBufStAddr[uIdx], STREAM_BUF_SIZE, 			
				uFrameBufStAddr[uIdx], bDecRotModeEn, true);
		else if (eDecMode[uIdx] == AVC_DEC)
			MFC_InitProcessForDecodingH264(uIdx, uStreamBufStAddr[uIdx], STREAM_BUF_SIZE, 
				uFrameBufStAddr[uIdx], bDecRotModeEn, bH264ReorderEn);
		else if (eDecMode[uIdx] == VC1_DEC)
			MFC_InitProcessForDecodingVc1(uIdx, uStreamBufStAddr[uIdx], STREAM_BUF_SIZE, 
				uFrameBufStAddr[uIdx], bDecRotModeEn);
		else
			Assert(0);
		MFC_GetProcessBufEndAddr(uIdx, &uFreeMemBuf);
		uFreeMemBuf = (uFreeMemBuf+STREAM_WR_SIZE-1)/STREAM_WR_SIZE*STREAM_WR_SIZE;
		MFC_SetWrPtr(uIdx, uStreamBufStAddr[uIdx]+nReadSize[uIdx]);		
		
		MFC_GetSrcPicSize(uIdx, &picX[uIdx], &picY[uIdx]);
		uRotWidth[uIdx] = (uDecRotDeg%180 == 0) ? picX[uIdx] : picY[uIdx];
		uRotHeight[uIdx] = (uDecRotDeg%180 == 0) ? picY[uIdx] : picX[uIdx];		
	}

	uFinishedProcessNum=0;
	uDispHorizontalDiv = (maxProcessNum == 1) ? 1 : (maxProcessNum <= 4) ? 2 : (maxProcessNum <= 6) ? 3 : 4;
	uDispVerticalDiv = (maxProcessNum <= 2) ? 1 : 2;
	uRotDispHorizontalDiv = (uDecRotDeg%180 == 0) ? uDispHorizontalDiv : uDispVerticalDiv;
	uRotDispVerticalDiv = (uDecRotDeg%180 == 0) ? uDispVerticalDiv : uDispHorizontalDiv;
	uRotDispWidth = uLcdWidth/uRotDispHorizontalDiv;
	uRotDispHeight = uLcdHeight*9/10/uRotDispVerticalDiv;
	
	
	for (i=0; i<MAX_PROCESS_NUM; i++)
	{
		bFinishedProcess[i] = false;
		uTotFrame[i] = 0;
	}

	LCD_Start();

	while (GetKey() == 0)  // loop until any key is pressed.
	{
		for (i=0; i<maxProcessNum; i++)
		{
			uIdx = i;
			if(bFinishedProcess[uIdx] == false)
			{
				// 5. Decode one frame
				//---------------------
				MFC_SetDecRotEn(bDecRotModeEn);
				if (bDecRotModeEn)
					MFC_SetDecRotationMode(bDecMirHor, bDecMirVer, uDecRotDeg);
				bStreamBufEmpty = false;	
				bMfcDone = false;
				MFC_StartDecodingOneFrame(uIdx);
				while(bMfcDone != true);						
						
				if (MFC_DoPostProcessingOfDecoding(uIdx, bStreamBufEmpty, &uFrameIdx) == false)
				{
					bFinishedProcess[uIdx] = true;
					uFinishedProcessNum++;
					break;
				}
				
				// 6. Init POST & Convert the frame for display
				//----------------------------------------------
				MFC_GetAddrOfYFrame(uIdx, uFrameIdx, &uFrmAddrY);

				xPos = (uDecRotDeg%180 == 0) ? i%uRotDispHorizontalDiv : i/uRotDispVerticalDiv;
				yPos = (uDecRotDeg%180 == 0) ? i/uRotDispHorizontalDiv : uRotDispVerticalDiv-1-i%uRotDispVerticalDiv;				
				POST_InitIp1(
					uRotWidth[uIdx], uRotHeight[uIdx], 0, 0, uRotWidth[uIdx], uRotHeight[uIdx],			
					uFrmAddrY, YC420,
					uLcdWidth, uLcdHeight, uRotDispWidth*xPos, uRotDispHeight*yPos, uRotDispWidth, uRotDispHeight,
					uLcdFbAddr, eLcdBpp, 1, false, ONE_SHOT, POST_DMA, POST_DMA, &oPost);
				bPostDone = false;
				POST_StartProcessing(&oPost);
				while(bPostDone == false);

				LCD_Trigger();

				uTotFrame[i]++;
				
				// 7. Load stream if necessary
				//------------------------------

				if (uFileSize[uIdx] == uStreamOffset[uIdx])
				{					
					MFC_NotifyNoMoreStream(uIdx);
				}
				else if (MFC_IsMoreStreamNeeded(uIdx, nReadUnit))
				{
					if ((uFileSize[uIdx]-uStreamOffset[uIdx]) < nReadUnit) 
						nReadSize[uIdx] = (uFileSize[uIdx]-uStreamOffset[uIdx]);
					else
						nReadSize[uIdx] = nReadUnit;
					
					if (!FAT_ReadFile4(sel[uIdx], uStreamOffset[uIdx]/uBlkSz, 
						(nReadSize[uIdx]+uBlkSz-1)/uBlkSz, uStreamBufStAddr[uIdx]+uStreamOffset[uIdx]%STREAM_BUF_SIZE, &oSdhc))
						Assert(0);
					
					uStreamOffset[uIdx] += nReadSize[uIdx];
					
					MFC_SetWrPtr(uIdx, uStreamBufStAddr[uIdx]+uStreamOffset[uIdx]%STREAM_BUF_SIZE);
				}				
			}					
		}
		if (uFinishedProcessNum == maxProcessNum)
			break;
    }

	FAT_UnloadFileSystem(&oSdhc);
	SDHC_CloseMedia(&oSdhc);
	
	UART_Printf("\n");
	for (i=0; i<maxProcessNum; i++)
	{
		UART_Printf("%d: [%d] frames decoded\n",i,uTotFrame[i]);
		MFC_IssueSeqEndCmd(i);
	}

}
#endif

//////////
// Function Name : TestMultipleDecWithOneEnc
// Function Description : This function decodes multiple stream files and encodes one YUV file
// Input : NONE
// Output : NONE
// Version :
#if (USE_SD_STORAGE && MULTI_DEC_ONE_ENC)
static void TestMultipleDecWithOneEnc(void)
{
	u32 uFreeMemBuf = MFC_MEMORY_BASE;
	u32 uLcdFbAddr;
	CSPACE eLcdBpp;	
	u32 uLcdWidth, uLcdHeight;
	u32 uDecWidth;
	u32 uDecHeight;
	u32 uRunIdx;
	s32 nFileSize[MAX_PROCESS_NUM];
	u32 uStreamBufStAddr[MAX_PROCESS_NUM];
	u32 uStreamOffset[MAX_PROCESS_NUM];
	int nReadSize[MAX_PROCESS_NUM];
	u32 uFrameBufStAddr[MAX_PROCESS_NUM];
	u32 picX[MAX_PROCESS_NUM];
	u32 picY[MAX_PROCESS_NUM];
	u32 uWidth[MAX_PROCESS_NUM];
	u32 uHeight[MAX_PROCESS_NUM];
	MFC_CODEC_MODE eMultiEncMode[MAX_PROCESS_NUM];
	MFC_CODEC_MODE eDecMode[MAX_PROCESS_NUM];
	s32 sel[MAX_PROCESS_NUM];
	s32 sTmpSel;
	int nDecProcessCount;
	int nEncProcessCount;
	const int uBlkSz = 512;
	const int nReadUnit = STREAM_BUF_SIZE/2;
	u32 i,j;
	char fileExt[50];
	char pFileName[256];
	u32 uTotalNumOfFiles;
	u32 uEncIdx;
	u32 uEncDispFrameWidth;
	u32 uEncDispFrameHeight;
	u32 uYuvFrameSize;
	u32 uYuvBufAddr;
	u32 uEncBufAddr;
	u32 recFrameIdx;
	u32 uReturnAddr;
	u32 uFileOffset = 0;
	u8 bFinishedProcess[MAX_PROCESS_NUM];
	u32 uFinishedProcessNum=0;
	u32 uTotFrame[MAX_PROCESS_NUM];
	u32 frameCount;
	u32 uFrameIdx;
	u32 uFrmAddrY;
	u32 maxProcessNum;
	u32 uDispHorizontalDiv;
	u32 uDispVerticalDiv;
	u32 uFrameDispY;

	// 1. Move firmware and init base settings.
	//-------------------------------------------		
	MFC_MoveFirmwareToCodeBuf(uFreeMemBuf);
	MFC_GetCodeBufEndAddr(&uFreeMemBuf);	
	MFC_StartBitProcessor();
	
	MFC_InitBaseForProcesses(uFreeMemBuf);
	MFC_GetBaseBufEndAddr(&uFreeMemBuf);
	MFC_ResetStreamBufInEncoding();    

	// 2. Init LCDC
	//---------------
	uLcdFbAddr = uFreeMemBuf;
	eLcdBpp = RGB24;	
	LCD_InitDISPC(eLcdBpp, uLcdFbAddr, WIN0, false);
	LCD_GetFbEndAddr(&uFreeMemBuf, WIN0);
	
	LCD_GetFrmSz(&uLcdWidth, &uLcdHeight, WIN0);
	uDecWidth = uLcdWidth;
	uDecHeight = uLcdHeight*9/10;

	GLIB_InitInstance(uLcdFbAddr, uLcdWidth, uLcdHeight, eLcdBpp);
	GLIB_ClearFrame(C_BLUE);
	GLIB_SetFontColor(C_WHITE, C_BLUE, false);
	GLIB_Printf(2, uDecHeight+6, "S3C6410 MFC Multi Codec Test");	
	
	INTC_SetVectAddr(NUM_POST0, Isr_Post);
	INTC_Enable(NUM_POST0);	
	POST_EnableInterrupt(POST_LEVEL_INT, &oPost);
	
	INTC_SetVectAddr(NUM_MFC,Isr_Mfc);
	INTC_Enable(NUM_MFC);

	while(!SDHC_OpenMedia(SDHC_HCLK, &oSdhc));
	if (!FAT_LoadFileSystem(&oSdhc))
		Assert(0);
	
	UART_Printf("How many processes do you want for decoding? (1~%d)\n",MAX_PROCESS_NUM-1);
	nDecProcessCount = UART_GetIntNum();
	//UART_Printf("How many processes do you want for encoding? (1~%d)\n", MAX_PROCESS_NUM-nDecProcessCount);
	//nEncProcessCount = GetInt(); // Now support only 1 process
	nEncProcessCount = 1;
	Assert (nDecProcessCount+nEncProcessCount <= MAX_PROCESS_NUM);

	Assert(nReadUnit%uBlkSz == 0);
		
	for (i=0; i<nDecProcessCount; i++)
	{
		uRunIdx = i;

		// 3. Reserve the stream buffer, & load the first part of a stream
		//-----------------------------------------------------------------	
		uStreamBufStAddr[uRunIdx] = uFreeMemBuf;
		uFreeMemBuf += STREAM_BUF_SIZE; // Reserved for stream buf.	
		Assert(uStreamBufStAddr[uRunIdx]%STREAM_WR_SIZE == 0);
			
		FAT_GetTotalNumOfFiles(&uTotalNumOfFiles, &oSdhc);
		for (j=0; j<uTotalNumOfFiles; j++)
		{
			FAT_GetFileName(j, pFileName, &oSdhc);
			UART_Printf("%02d -- %s\n", j, pFileName);
		}

		UART_Printf(" Which file to read for decoding? ");
		sel[i] = UART_GetIntNum();	

		FAT_GetFileSize(sel[uRunIdx], &nFileSize[uRunIdx], &oSdhc);
	
		nReadSize[uRunIdx] = (nFileSize[uRunIdx] < nReadUnit) ? nFileSize[uRunIdx] : nReadUnit;
		uStreamOffset[uRunIdx] = nReadSize[uRunIdx];
		if (!FAT_ReadFile4(sel[uRunIdx], 0, (nReadSize[uRunIdx]+uBlkSz-1)/uBlkSz, uStreamBufStAddr[uRunIdx], &oSdhc))
			Assert(0);	

		// 4. Init processes of MFC
		//--------------------------	
		FAT_GetFileExt(sel[uRunIdx], fileExt, &oSdhc);	
		MFC_GetCodecModeByName(fileExt, true, &eDecMode[uRunIdx]);

		bH264ReorderEn = true;
		uFrameBufStAddr[uRunIdx] = uFreeMemBuf;
		
		if (eDecMode[uRunIdx] == MP4_DEC)
			MFC_InitProcessForDecodingMpeg4(uRunIdx, uStreamBufStAddr[uRunIdx], STREAM_BUF_SIZE, 			
				uFrameBufStAddr[uRunIdx], bDecRotModeEn, true);
		else if (eDecMode[uRunIdx] == AVC_DEC)
			MFC_InitProcessForDecodingH264(uRunIdx, uStreamBufStAddr[uRunIdx], STREAM_BUF_SIZE, 
				uFrameBufStAddr[uRunIdx], bDecRotModeEn, bH264ReorderEn);
		else if (eDecMode[uRunIdx] == VC1_DEC)
			MFC_InitProcessForDecodingVc1(uRunIdx, uStreamBufStAddr[uRunIdx], STREAM_BUF_SIZE, 
				uFrameBufStAddr[uRunIdx], bDecRotModeEn);
		else
			Assert(0);
		MFC_GetProcessBufEndAddr(uRunIdx, &uFreeMemBuf);
		uFreeMemBuf = (uFreeMemBuf+STREAM_WR_SIZE-1)/STREAM_WR_SIZE*STREAM_WR_SIZE;
		MFC_SetWrPtr(uRunIdx, uStreamBufStAddr[uRunIdx]+nReadSize[uRunIdx]);
				
		MFC_GetSrcPicSize(uRunIdx, &picX[uRunIdx], &picY[uRunIdx]);
		MFC_GetFrameSize(uRunIdx, &uWidth[uRunIdx], &uHeight[uRunIdx]);	
	}

	// reading SD card	
	uEncIdx = nDecProcessCount;
	FAT_ReadFileSystemInfo(&oSdhc);
	SetEncMode(&sTmpSel);	
	sel[uEncIdx] = sTmpSel;

	uEncDispFrameWidth = uEncWidth;
	uEncDispFrameHeight = uEncHeight;

	MFC_SetEncYuvFileCfg(uEncIdx, pSrcYUVFileName, uEncTotalFrameNum, uEncWidth, uEncHeight, uEncFrameRate);

	MFC_SetEncSliceCfg(uEncIdx, (bEncSliceMode) ? 1 : 0, (bEncSliceSizeMode) ? 1 : 0, uEncSliceSizeNum);
	MFC_SetEncErrResilienceCfg(uEncIdx, uEncIntraRefreshNum);
	if (eEncMode == MP4_ENC)
	{
		if (bIsH263)
		{
			MFC_SetEnc263Cfg(uEncIdx, 0, (bEncAnnexJ) ? 1 : 0, (bEncAnnexK) ? 1 : 0, (bEncAnnexT) ? 1 : 0, uEncPicQs*2/3, uEncGopNum);
		}
		else
		{
			MFC_SetEncMpeg4Cfg(uEncIdx, (bEncDataPartEn) ? 1 : 0, (bEncRevVlcEn) ? 1 : 0, uEncIntraDcVlcThr, uEncPicQs*2/3, uEncGopNum);
		}		
	}
	else if (eEncMode == AVC_ENC)
	{
		MFC_SetEnc264Cfg(uEncIdx, 0, uEncDeblkMode, uEncDeblkAOffset, uEncDeblkBOffset, uEncChromaQpOffset, uEncPicQs, uEncGopNum);
	}
	MFC_SetEncRateCtrlCfg(uEncIdx, (bEncRcEnable) ? 1 : 0, uEncRcBitRate, uEncRcInitDelay, uEncRcBufSize);

	eMultiEncMode[uEncIdx] = eEncMode;
	picX[uEncIdx] = uEncWidth;
	picY[uEncIdx] = uEncHeight;	
	uYuvFrameSize = picX[uEncIdx] * picY[uEncIdx] * 3/2;
	
	uYuvBufAddr = uFreeMemBuf;
	uFreeMemBuf += (uYuvFrameSize/512 + 2) * 512;

	uEncBufAddr = uFreeMemBuf;
	MFC_InitProcessForEncoding(
		uEncIdx,
		eMultiEncMode[uEncIdx],
		uEncBufAddr,
		STREAM_BUF_SIZE,
		uEncDispFrameWidth,
		uEncDispFrameHeight,
		false);

	MFC_GetProcessBufEndAddr(uEncIdx, &uFreeMemBuf);

	uFileOffset = 0;

	uFinishedProcessNum=0;
	maxProcessNum = nDecProcessCount+nEncProcessCount;
	uDispHorizontalDiv = (maxProcessNum == 1) ? 1 : (maxProcessNum <= 4) ? 2 : (maxProcessNum <= 6) ? 3 : 4;
	uDispVerticalDiv = (maxProcessNum <= 2) ? 1 : 2;		
	
	for (i=0; i<MAX_PROCESS_NUM; i++)
	{
		bFinishedProcess[i] = false;
		uTotFrame[i] = 0;
	}
	
	LCD_Start();
	
	while (GetKey() == 0)   // loop until any key is pressed.
	{
		for (i=0; i<nDecProcessCount; i++)
		{
			uRunIdx = i;
			if(bFinishedProcess[uRunIdx] == false)
			{
				// 5. Decode one frame
				//---------------------			
				MFC_SetDecRotEn(bDecRotModeEn);
				if (bDecRotModeEn)
					MFC_SetDecRotationMode(bDecMirHor, bDecMirVer, uDecRotDeg);
				bStreamBufEmpty = false;
				bMfcDone = false;
				MFC_StartDecodingOneFrame(uRunIdx);
				while(bMfcDone != true);						
						
				if (MFC_DoPostProcessingOfDecoding(uRunIdx, bStreamBufEmpty, &uFrameIdx) == false)
				{
					bFinishedProcess[uRunIdx] = true;
					uFinishedProcessNum++;
					break;
				}
				MFC_GetAddrOfYFrame(uRunIdx, uFrameIdx, &uFrmAddrY);
				
				// 6. Init POST & Convert the frame for display
				//----------------------------------------------				
				POST_InitIp1(
					uWidth[uRunIdx], uHeight[uRunIdx],
					0, 0,
					uWidth[uRunIdx], uHeight[uRunIdx],
					uFrmAddrY, YC420,
					uLcdWidth, uLcdHeight,
					uDecWidth/uDispHorizontalDiv*(i%uDispHorizontalDiv), uDecHeight/uDispVerticalDiv*(i/uDispHorizontalDiv),
					uDecWidth/uDispHorizontalDiv, uDecHeight/uDispVerticalDiv,					
					uLcdFbAddr, eLcdBpp,
					1, false, ONE_SHOT, POST_DMA, POST_DMA, &oPost);
				bPostDone = false;				
				POST_StartProcessing(&oPost);
				while(bPostDone == false);

				LCD_Trigger();

				uTotFrame[i]++;
				
				// 7. Load stream if necessary
				//------------------------------

				if (nFileSize[uRunIdx] == uStreamOffset[uRunIdx])
				{					
					MFC_NotifyNoMoreStream(uRunIdx);
				}
				else if (MFC_IsMoreStreamNeeded(uRunIdx, nReadUnit))
				{
					if ((nFileSize[uRunIdx]-uStreamOffset[uRunIdx]) < nReadUnit) 
						nReadSize[uRunIdx] = (nFileSize[uRunIdx]-uStreamOffset[uRunIdx]);
					else
						nReadSize[uRunIdx] = nReadUnit;
					
					if (!FAT_ReadFile4(sel[uRunIdx], uStreamOffset[uRunIdx]/uBlkSz, 
						(nReadSize[uRunIdx]+uBlkSz-1)/uBlkSz, uStreamBufStAddr[uRunIdx]+uStreamOffset[uRunIdx]%STREAM_BUF_SIZE, &oSdhc))
						Assert(0);
					
					uStreamOffset[uRunIdx] += nReadSize[uRunIdx];
					
					MFC_SetWrPtr(uRunIdx, uStreamBufStAddr[uRunIdx]+uStreamOffset[uRunIdx]%STREAM_BUF_SIZE);
				}						
			}
				
		}

		for (i=nDecProcessCount; i<nDecProcessCount+nEncProcessCount; i++)
		{
			uEncIdx = i;
			if(bFinishedProcess[uEncIdx] == false)
			{
				// 1 frame YUV read from SD card
				if (!FAT_ReadFile6(sel[uEncIdx], uFileOffset, uYuvFrameSize, uYuvBufAddr, &uReturnAddr, &oSdhc))
					break;
				uFileOffset += uYuvFrameSize;	

				MFC_SetEncRunOption(false, false);
				MFC_EncodeOneFrame(uEncIdx, uReturnAddr, &recFrameIdx);
				
				MFC_GetAddrOfYFrame(uEncIdx, recFrameIdx, &uFrameDispY);
						
				bPostDone = false;	
				POST_InitIp1(
					uEncDispFrameWidth, uEncDispFrameHeight,
					0, 0,
					picX[uEncIdx], picY[uEncIdx],			
					uFrameDispY, YC420,
					uLcdWidth, uLcdHeight,
					uDecWidth/uDispHorizontalDiv*(i%uDispHorizontalDiv), uDecHeight/uDispVerticalDiv*(i/uDispHorizontalDiv),
					uDecWidth/uDispHorizontalDiv, uDecHeight/uDispVerticalDiv,
					uLcdFbAddr, eLcdBpp,
					1, false, ONE_SHOT, POST_DMA, POST_DMA, &oPost);
				POST_StartProcessing(&oPost);
				while(bPostDone == false);

				LCD_Trigger();				

				MFC_GetFrameCount(&frameCount);			
	        	if (frameCount == uEncTotalFrameNum)	// end of YUV frame
	        	{
	            	bFinishedProcess[uEncIdx] = true;
					uFinishedProcessNum++;					
	        	}
				
				uTotFrame[i]++;
			}
		}		
			
		if (uFinishedProcessNum == nDecProcessCount+nEncProcessCount)
			break;		    
	}
	
	FAT_UnloadFileSystem(&oSdhc);
	SDHC_CloseMedia(&oSdhc);
	
	UART_Printf("\n");
	for (i=0; i<nDecProcessCount; i++)
	{
		UART_Printf("%d: [%d] frames decoded\n",i,uTotFrame[i]);
		MFC_IssueSeqEndCmd(i);
	}
	for (i=nDecProcessCount; i<nDecProcessCount+nEncProcessCount; i++)
	{
		UART_Printf("%d: [%d] frames encoded\n",i,uTotFrame[i]);
		MFC_IssueSeqEndCmd(i);
	}

}
#endif

//////////
// Function Name : TestDecodingForPerformanceMeasure
// Function Description : This function is made to measure the decoding performance
// Input : NONE
// Output : NONE
// Version : 
static void TestDecodingForPerformanceMeasure(void)
{
	u32 uFreeMemBuf = MFC_MEMORY_BASE;
#if USE_SD_STORAGE	
	char pFileName[256];
	u32 uTotalNumOfFiles, i;
	u8 readResult;
	char fileExt[50];
	int sel;
#endif	
	static s32 uFileSize=0;
	u32 uStreamBufStAddr;
	MFC_CODEC_MODE eDecMode;
	u32 uProcessIdx=0;
	u32 uFrameBufStAddr;
	u32 uFrameIdx;
	float fTime;
	float fTotTime,fTmpTotTime=0;
	u32 frameCount;

	LCD_Stop();
	
	// 1. Load a stream 
	//-------------------

#if (USE_SD_STORAGE)
	while(!SDHC_OpenMedia(SDHC_HCLK, &oSdhc));
	if (!FAT_LoadFileSystem(&oSdhc))
		Assert(0);

	FAT_GetTotalNumOfFiles(&uTotalNumOfFiles, &oSdhc);
	for (i=0; i<uTotalNumOfFiles; i++)
	{
		FAT_GetFileName(i, pFileName, &oSdhc);
		UART_Printf("%02d -- %s\n", i, pFileName);
	}

	UART_Printf(" Which file to read ? ");
	sel = UART_GetIntNum();	

	uFileSize=0;
	uStreamBufStAddr = uFreeMemBuf;
	readResult = FAT_ReadFile1(sel, uStreamBufStAddr, &oSdhc);
	if (readResult)
		FAT_GetFileSize(sel, &uFileSize, &oSdhc);
#else
	uStreamBufStAddr = uFreeMemBuf;
	UART_Printf(" Before testing, download a stream file through USB\n");
	UART_Printf(" If you want to test with previous stream file, press 'x' key\n");
	if(Getc()!='x')	
		uFileSize = DownloadImageThruUsbOtg((u8 *)uStreamBufStAddr);
	if (uFileSize == 0)
		return;
#endif
	uFreeMemBuf += uFileSize;
	uFreeMemBuf = (uFreeMemBuf + STREAM_WR_SIZE-1)/STREAM_WR_SIZE*STREAM_WR_SIZE;

	// 2. Move firmware and init base settings.
	//-------------------------------------------
		
	MFC_MoveFirmwareToCodeBuf(uFreeMemBuf);
	MFC_GetCodeBufEndAddr(&uFreeMemBuf);
	MFC_StartBitProcessor();

	MFC_InitBaseForProcesses(uFreeMemBuf);
	MFC_GetBaseBufEndAddr(&uFreeMemBuf);

	// 3. Init a process of MFC
	//--------------------------
#if USE_SD_STORAGE
	FAT_GetFileExt(sel, fileExt, &oSdhc);
	MFC_GetCodecModeByName(fileExt, true, &eDecMode);
#else
	UART_Printf("\nSelect Decoder mode: \n");
	UART_Printf("[0] MPEG4/H.263\n");
	UART_Printf("[1] H.264\n");
	UART_Printf("[2] VC-1\n");
	eDecMode = (MFC_CODEC_MODE)UART_GetIntNum();
	if(eDecMode>2)
		return;
	eDecMode = (eDecMode==0) ? MP4_DEC : (eDecMode==1) ? AVC_DEC : VC1_DEC ;
	
#endif
	
	uProcessIdx=0;
	uFrameBufStAddr = uFreeMemBuf;
	bH264ReorderEn = true;
	if (eDecMode == MP4_DEC)
		MFC_InitProcessForDecodingMpeg4(uProcessIdx, uStreamBufStAddr, uFileSize, 			
			uFrameBufStAddr, bDecRotModeEn, true);
	else if (eDecMode == AVC_DEC)
		MFC_InitProcessForDecodingH264(uProcessIdx, uStreamBufStAddr, uFileSize, 
			uFrameBufStAddr, bDecRotModeEn, bH264ReorderEn);
	else if (eDecMode == VC1_DEC)
		MFC_InitProcessForDecodingVc1(uProcessIdx, uStreamBufStAddr, uFileSize, 
			uFrameBufStAddr, bDecRotModeEn);
	else
		Assert(0);	
	MFC_GetProcessBufEndAddr(uProcessIdx, &uFreeMemBuf);
	MFC_NotifyNoMoreStream(uProcessIdx);
	INTC_SetVectAddr(NUM_MFC,Isr_Mfc);
	INTC_Enable(NUM_MFC);
		
	fTotTime=0;
	frameCount=0;
	
	while (GetKey() == 0) // loop until any key is pressed.
	{
		// 4. Decode one frame
		//---------------------
		float fTmpTime;

		StartTimer(0);	
		
		bStreamBufEmpty = false;	
		bMfcDone = false;
		MFC_SetDecRotEn(bDecRotModeEn);
		if (bDecRotModeEn)
			MFC_SetDecRotationMode(bDecMirHor, bDecMirVer, uDecRotDeg);
		MFC_StartDecodingOneFrame(uProcessIdx);		
		fTmpTime = StopTimer(0);
		StartTimer(0);
		while(bMfcDone != true);

		fTime = StopTimer(0);
		fTmpTotTime +=	fTime;			
		fTotTime += (fTime+fTmpTime);
		
		if (MFC_DoPostProcessingOfDecoding(uProcessIdx, bStreamBufEmpty, &uFrameIdx) == false)
			break;

		frameCount++;

		if((frameCount%10)==0)
		{
			Putc('.');
		}
		
	}
	UART_Printf("\ntotal decoding time=%.fus\n",fTotTime);
	UART_Printf("FPS=%.2f, average elapsed time= %.2f(ms), total %d frames\n", 
		1000000*frameCount/fTotTime, fTotTime/frameCount/1000, frameCount);
		
	UART_Printf("ARM can be turned off for %.2f(ms)\n",fTmpTotTime);
		
#if USE_SD_STORAGE
	FAT_UnloadFileSystem(&oSdhc);
	SDHC_CloseMedia(&oSdhc);
#endif	

	MFC_IssueSeqEndCmd(uProcessIdx);
	MFC_StopBitProcessor(); 	

}

//////////
// Function Name : TestEncodingForPerformanceMeasure
// Function Description : This function is made to measure the decoding performance
// Input : NONE
// Output : NONE
// Version : 
static void TestEncodingForPerformanceMeasure(void)
{
	u32 uFreeMemBuf = MFC_MEMORY_BASE;
	u32 uProcessIdx;
	u32 uYuvFrameSize;
	const char* pEncModeName;
	u32 uYuvBufAddr;
	s32 sel;
#if (!USE_SD_STORAGE)
	u32 uYuvFileAddr, uYuvFileSize;	
#endif
	u32 uTransferSize;
	const int nWriteUnit = STREAM_BUF_SIZE/2;
	u32 uStreamOffset = 0;
	u32 uSaveBufAddr;
	u32 uStreamBufStAddr;
	u32 uSrcYuvFileOffset = 0;
	u32 uTrueStAddrOfData;
	u32 uRecFrameIdx;
	u32 uWrPtr;
	float fTime;
	float fTotTime;
	u32 frameCount;
	
#if USE_SD_STORAGE
	// 0. Select the YUV file in the SD card.
	//-------------------------------------------
	while(!SDHC_OpenMedia(SDHC_HCLK, &oSdhc));
	if (!FAT_LoadFileSystem(&oSdhc))
		Assert(0);
		
	SetEncMode(&sel);
	
	if(sel == -1)
	{
		UART_Printf("There is the matched file in the SD card\n");
		return;
	}
#else
	// 0. Download the YUV file thru USB.
	//-------------------------------------------
	SetEncMode(&sel);
	
	uYuvFileAddr = uFreeMemBuf;
	UART_Printf(" Before testing, download \"%s\" file through USB \n", pSrcYUVFileName);
	uYuvFileSize = DownloadImageThruUsbOtg((u8 *)uYuvFileAddr);
	if (uYuvFileSize == 0)
		return;
	uFreeMemBuf += uYuvFileSize;
	uFreeMemBuf = (uFreeMemBuf+STREAM_WR_SIZE-1)/STREAM_WR_SIZE*STREAM_WR_SIZE;
#endif

	// 1. Move firmware and init base settings.
	//-------------------------------------------
	
	MFC_MoveFirmwareToCodeBuf(uFreeMemBuf);
	MFC_GetCodeBufEndAddr(&uFreeMemBuf);	
	MFC_StartBitProcessor();
	
	MFC_InitBaseForProcesses(uFreeMemBuf);
	MFC_GetBaseBufEndAddr(&uFreeMemBuf);
	MFC_SetNoFlushingStreamBufModeInEncoding();

	// 2. Set encoding parameters.
	//-----------------------------

	uProcessIdx = 0;

	MFC_SetEncYuvFileCfg(uProcessIdx, pSrcYUVFileName, uEncTotalFrameNum, uEncWidth, uEncHeight, 30);
	MFC_SetEncSliceCfg(uProcessIdx, 1, 0, 4000);
	MFC_SetEncErrResilienceCfg(uProcessIdx, 0);

	if (eEncMode == MP4_ENC)
	{
		if (bIsH263)
			MFC_SetEnc263Cfg(uProcessIdx, 0, 1, 0, 0, 10, 15);
		else
			MFC_SetEncMpeg4Cfg(uProcessIdx, 0, 0, 0, 10, 15);
	}
	else if (eEncMode == AVC_ENC)
	{
		MFC_SetEnc264Cfg(uProcessIdx, 0, 0, 0, 0, 0, 15, 15);
	}
	MFC_SetEncRateCtrlCfg(uProcessIdx, 1, 128, 0, 0);

	uYuvFrameSize = uEncWidth * uEncHeight * 3/2;	

	pEncModeName =
		(eEncMode == MP4_ENC && bIsH263) ? "H.263" :
		(eEncMode == MP4_ENC && !bIsH263) ? "MPEG4" : "H.264";
	UART_Printf("Source file Name: %s, enc mode: %s, w:%d, h:%d, uEncTotalFrameNum:%d\n",
		pSrcYUVFileName, pEncModeName, uEncWidth,uEncHeight,uEncTotalFrameNum);

	// 3. Keep one frame sized memory and index of the selected YUV file
	//-----------------------------------------------------------------

	uYuvBufAddr = uFreeMemBuf;
	uFreeMemBuf += (uYuvFrameSize/512 + 2) * 512;

	// 4. Init a process of MFC
	//--------------------------
	uFreeMemBuf = (uFreeMemBuf+STREAM_WR_SIZE-1)/STREAM_WR_SIZE*STREAM_WR_SIZE;
	uStreamBufStAddr = uFreeMemBuf;
	MFC_InitProcessForEncoding(
		uProcessIdx, eEncMode, uStreamBufStAddr, STREAM_BUF_SIZE, uEncWidth, uEncHeight, false);
	MFC_GetProcessBufEndAddr(uProcessIdx, &uFreeMemBuf);
	INTC_SetVectAddr(NUM_MFC,Isr_Mfc);
	INTC_Enable(NUM_MFC);

	uSrcYuvFileOffset = 0;
	uSaveBufAddr = uFreeMemBuf;
	uFreeMemBuf += STREAM_BUF_SIZE*4;

	fTotTime=0;
	frameCount=0;
		
	while (GetKey() == 0)
	{
		// 5. Read one frame of YUV image from SD card
		//---------------------------------------------
#if USE_SD_STORAGE
		if (!FAT_ReadFile6(sel, uSrcYuvFileOffset, uYuvFrameSize, uYuvBufAddr, &uTrueStAddrOfData, &oSdhc))
			break;
#else
		if((uSrcYuvFileOffset+uYuvFrameSize)<=uYuvFileSize)
		{
			memcpy((void *)uYuvBufAddr, (void *)(uYuvFileAddr+uSrcYuvFileOffset), uYuvFrameSize);
			uTrueStAddrOfData = uYuvBufAddr;
		}
		else
			break;
#endif
		uSrcYuvFileOffset += uYuvFrameSize;	

		// 6. Encode one frame
		//---------------------

		StartTimer(0);
		
		bMfcDone = false;
		MFC_SetEncRunOption(false, false);
		MFC_StartEncodingOneFrame(uProcessIdx, uTrueStAddrOfData);
		while(bMfcDone != true);
		
		fTime = StopTimer(0);
		fTotTime += fTime;

		MFC_DoPostProcessingOfEncoding(uProcessIdx, &uRecFrameIdx);

		// 7. Save the encoded stream of the size of the write unit
		//----------------------------------------------------------
		frameCount++;
		MFC_GetWrPtr(uProcessIdx, &uWrPtr);

		if (uWrPtr > uStreamBufStAddr+((uStreamOffset+nWriteUnit)%STREAM_BUF_SIZE)+STREAM_WR_SIZE)
		{
			memcpy((void *)(uSaveBufAddr+uStreamOffset), (void *)(uStreamBufStAddr+(uStreamOffset%STREAM_BUF_SIZE)), nWriteUnit);

			uStreamOffset += nWriteUnit;			
			MFC_SetRdPtr(uProcessIdx, uWrPtr-STREAM_WR_SIZE); // to avoid the full status
		}

		if((frameCount%10)==0)
		{
			Putc('.');
		}

		if (frameCount == uEncTotalFrameNum)
		{
			UART_Printf("Encoding is ended with %d frames\n", frameCount);
			break;
		}
	}

	MFC_IssueSeqEndCmd(uProcessIdx);
	
	UART_Printf("\ntotal decoding time=%.fus\n",fTotTime);
	UART_Printf("FPS=%.2f, average elapsed time= %.2f(ms), total %d frames\n", 
		1000000*frameCount/fTotTime, fTotTime/frameCount/1000, frameCount);		

	// As we called MFC_SetNoFlushingStreamBufModeInEncoding(),
	// we need copy operation once more after calling MFC_IssueSeqEndCmd().
	MFC_GetWrPtr(uProcessIdx, &uWrPtr);
	uTransferSize = uWrPtr - (uStreamBufStAddr+(uStreamOffset%STREAM_BUF_SIZE));
	memcpy((void *)(uSaveBufAddr+uStreamOffset), (void *)(uStreamBufStAddr+(uStreamOffset%STREAM_BUF_SIZE)), uTransferSize);
	uStreamOffset += uTransferSize;
	
#if USE_SD_STORAGE
	#if 1
	{
		char pEncFileName[256];
		
		UART_Printf("Write down the file name to save the encoded stream to SD/MMC card : \n");
		UART_Printf("(Press just 'Enter' key, if you don't want to save)\n");
		gets(pEncFileName);
		
		if(pEncFileName[0]!='\0')
			FAT_WriteFile(pEncFileName, uStreamOffset, uSaveBufAddr, &oSdhc);
	}
	#endif	

	FAT_UnloadFileSystem(&oSdhc);
	SDHC_CloseMedia(&oSdhc);
	
#else
	UART_Printf("\n Target Address : 0x%08x!!!\n",uSaveBufAddr);
	UART_Printf(" Upload Size : 0x%x!!!\n",uStreamOffset);

	UploadImageThruUsbOtg();
#endif
	
	MFC_StopBitProcessor();

}

//////////
// Function Name : MFC_Test
// Function Description : mfc main test function
// Input : NONE
// Output : NONE
// Version : 
void MFC_Test(void)
{
	int i, sel;
	s32 ucMfcClkRatio=1;
	u32 uTemp32;
	u32 uEPLL_M=50, uEPLL_P=2, uEPLL_S=0;

	const testFuncMenu menu[]=
	{
		SetRotationMode,						"Select Rotation & Mirror mode\n",
		
		TestDecoding,							"Test Decoding",	
		TestDmbDecoding,						"Test DMB Decoding",		
		TestEncoding,							"Test Encoding\n",
		
		TestDecodingWithAllRotModes,			"Test Decoding with all rotation modes",
		TestEncodingWithAllRotModes,			"Test Encoding with all rotation modes\n",
		
#if (USE_SD_STORAGE)
		TestMultipleDecoding,					"Test multiple decoding",
	#if (MULTI_DEC_ONE_ENC)		
		TestMultipleDecWithOneEnc,				"Test multiple decoding with one encoding\n",
	#endif
		TestDecConformanceEach,					"Test decoding conformance(each)",
		TestDecConformance,						"Test decoding conformance",
		TestEncodingParameters,					"Test encoding parameters\n",
		
		TestDecodingOneFrameByOneFrame,			"Test decoding 1 frame by 1 frame\n",
#endif
		TestDecodingForPerformanceMeasure,		"Test Decoding for performance measure",
		TestEncodingForPerformanceMeasure,		"Test Encoding for performance measure",
		0,0
	};
	
	POST_InitCh(POST_A, &oPost);
	LCD_SetPort();
	LCD_InitLDI(MAIN);
#if (USE_SD_STORAGE)	
	SDHC_InitCh(SDHC_CHANNEL_0, &oSdhc);
#endif	

	uTemp32 = Inp32SYSC(0x038);		// read rSCLK_GATE sfr
	uTemp32 &= ~(1<<3);				// disable MFC clock
	Outp32SYSC(0x038,uTemp32);
	
#if (MFC_CLOCK_CHANGE)
	UART_Printf("Select MFC Clock Source(1:HCLKx2, 2:EPLL)\n");
	uTemp32 = UART_GetIntNum();
	if(uTemp32==1)
	{
		uTemp32 = Inp32SYSC(0x01C);		// read rCLK_SRC sfr
		uTemp32 &= ~(0x10<<0);			// set MFC clock source to HCLKx2
		Outp32SYSC(0x01C,uTemp32);
	}
	else if(uTemp32==2)
	{
		SYSC_SetLockTime( eEPLL, 300);
		SYSC_CtrlCLKOUT(eCLKOUT_EPLLOUT, 0);
		SYSC_SetPLL(eEPLL, uEPLL_M, uEPLL_P, uEPLL_S, 0);
		uTemp32 = Inp32SYSC(0x01C);		// read rCLK_SRC sfr
		uTemp32 |= (0x14<<0);			// set MFC clock source to EPLL out
		Outp32SYSC(0x01C,uTemp32);	
	}
	else
	{
		UART_Printf("You entered wrong number!!\n");
		return;
	}
		
	UART_Printf("Enter MfcClkRatio:");	
	if(((ucMfcClkRatio=UART_GetIntNum())==-1)||(ucMfcClkRatio>15))
		ucMfcClkRatio = 1;
#endif
		
	uTemp32 = Inp32SYSC(0x020);		// read rCLK_DIV0 sfr
	uTemp32 = uTemp32 & ~(0xf<<28) | (ucMfcClkRatio<<28);	// set MFC clock division ratio
	Outp32SYSC(0x020,uTemp32);
	ucMfcClkRatio = Inp32SYSC(0x020)>>28;		// read MFC clock division ratio field of rCLK_DIV0 sfr
	
	uTemp32 = Inp32SYSC(0x038);		// read rSCLK_GATE sfr
	uTemp32 |= (1<<3);				// enable MFC clock
	Outp32SYSC(0x038,uTemp32);

	bDecRotModeEn = false;
	uDecRotDeg = 0;
	bDecMirHor = false;
	bDecMirVer = false;
		
	while(1)
	{
		eEncMode = MP4_ENC;
		bIsH263 = false;

		uEncWidth = 176;
		uEncHeight = 144;
		uEncTotalFrameNum = 300;
	#if (USE_SD_STORAGE && MULTI_DEC_ONE_ENC)		
		uEncFrameRate = 30;
		bEncSliceMode = true;
		bEncSliceSizeMode = false;
		uEncSliceSizeNum = 4000;
		uEncIntraRefreshNum = 0;
		uEncGopNum = 15;
		bEncRcEnable = true;
		uEncRcBitRate = 128;
		uEncRcInitDelay = 0;
		uEncRcBufSize = 0;
		uEncPicQs = 15;

		uEncChromaQpOffset = 0;
		uEncDeblkMode = 0;
		uEncDeblkAOffset = 0;
		uEncDeblkBOffset = 0;

		bEncAnnexJ = true;
		bEncAnnexK = false;
		bEncAnnexT = false;

		bEncDataPartEn = false;
		bEncRevVlcEn = false;
		uEncIntraDcVlcThr = 0;
	#endif	
		
		uTemp32 = ((Inp32SYSC(0x01C)&0x10)>>4);		// read MFC clock source field of rCLK_SRC sfr
		if(uTemp32==0)
		{
			UART_Printf(" [MFC Clock : %.1fMHz]\n\n",(float)g_HCLKx2/(ucMfcClkRatio+1)/1000000);
		}
		else
		{
			uTemp32 = (FIN * uEPLL_M / uEPLL_P)>>uEPLL_S;
			UART_Printf(" [MFC Clock : %.1fMHz]\n\n",(float)uTemp32/(ucMfcClkRatio+1)/1000000);
		}		
		
		for (i=0; (int)(menu[i].desc)!=0; i++)
			UART_Printf("%2d: %s\n", i, menu[i].desc);

		UART_Printf("\nSelect the function to test : ");
		sel = UART_GetIntNum();
		UART_Printf("\n");

		if (sel == -1)
			break;
		else if (sel>=0 && sel<(sizeof(menu)/8-1))
			(menu[sel].func)();
	}	
}


