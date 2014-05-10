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
*	File Name : mfc.c
*  
*	File Description : This file implements the API functons for MFC.
*
*	Author : Haksoo,Kim
*	Dept. : AP Development Team
*	Created Date : 2006/12/21
*	Version : 0.1 
* 
*	History
*	- Created(Haksoo,Kim 2006/12/21)
*  
**************************************************************************************/

#include "option.h"
#include "library.h"
#include "sfr6410.h"
#include "mfc.h"

//#include "Prism_S_V131.h" // firmware (80KB)
//#include "Prism_S_V133.h" // firmware (80KB)
//#include "Prism_S_V127_clock_problem_Beta.h"	//firmware (80KB)
//#include "Prism_S_V133_clock_problem_Beta.h"	//firmware (80KB)
#include "Prism_S_V134.h"	//firmware (80KB)

#include <string.h>
#include <stdlib.h>

//#define DBG_MFC
#ifdef DBG_MFC
	#define DbgMfc	Disp
#else
	#define DbgMfc(...)
#endif

#if 0
#define mfcOutp32(addr, data) {UART_Printf("Outp1w(32'h%x, 32'h%x);\n", (addr), (data)); (*(volatile u32 *)(addr) = (data)); } 
#define mfcInp32(addr, data) {((data) = (*(volatile u32 *)(addr))); UART_Printf("Inp1w(32'h%x, 32'h%x);\n", (addr), (data)); }
#else
#define mfcOutp32 Outp32
#define mfcInp32 Inp32
#endif

enum MFC_REG
{
	CODE_RUN           = MFC_BASE+0x000,
		// [0] 1=Start the bit processor, 0=Stop.
	CODE_DN_LOAD       = MFC_BASE+0x004,
		// [15:0]
		// [28:16]
	HOST_INTR          = MFC_BASE+0x008,
		// [0] Write '1' to this bit to request an interrupt to BIT
	BITS_INT_CLEAR     = MFC_BASE+0x00c,
		// [0]
	BITS_INT_STAT      = MFC_BASE+0x010,
		// [0] 1 means that BIT interrupt to the host is asserted.
	BITS_CODE_RESET    = MFC_BASE+0x014,
	BITS_CUR_PC        = MFC_BASE+0x018,

	CODE_BUF_ADDR      = MFC_BASE+0x100,
	WORK_BUF_ADDR      = MFC_BASE+0x104,
	PARA_BUF_ADDR      = MFC_BASE+0x108,
	STREAM_BUF_CTRL    = MFC_BASE+0x10c,
	FRAME_BUF_CTRL     = MFC_BASE+0x110,
	DEC_FUNC_CTRL      = MFC_BASE+0x114,
	WORK_BUF_CTRL      = MFC_BASE+0x11c,

	BITS_RD_PTR0       = MFC_BASE+0x120,
	BITS_WR_PTR0       = MFC_BASE+0x124,
	BITS_RD_PTR1       = MFC_BASE+0x128,
	BITS_WR_PTR1       = MFC_BASE+0x12c,
	BITS_RD_PTR2       = MFC_BASE+0x130,
	BITS_WR_PTR2       = MFC_BASE+0x134,
	BITS_RD_PTR3       = MFC_BASE+0x138,
	BITS_WR_PTR3       = MFC_BASE+0x13c,
	
	BITS_RD_PTR4       = MFC_BASE+0x140,
	BITS_WR_PTR4       = MFC_BASE+0x144,
	BITS_RD_PTR5       = MFC_BASE+0x148,
	BITS_WR_PTR5       = MFC_BASE+0x14c,
	BITS_RD_PTR6       = MFC_BASE+0x150,
	BITS_WR_PTR6       = MFC_BASE+0x154,
	BITS_RD_PTR7       = MFC_BASE+0x158,
	BITS_WR_PTR7       = MFC_BASE+0x15c,
		
	BUSY_FLAG          = MFC_BASE+0x160,
	RUN_CMD            = MFC_BASE+0x164,
	RUN_INDEX          = MFC_BASE+0x168,
	RUN_COD_STD        = MFC_BASE+0x16c,
	INT_ENABLE         = MFC_BASE+0x170,
	INT_REASON         = MFC_BASE+0x174,
	
	// DEC_SEQ_INIT Parameter reg
	// R/W reg
	DEC_SEQ_BIT_BUF_ADDR   = MFC_BASE+0x180,
	DEC_SEQ_BIT_BUF_SIZE   = MFC_BASE+0x184,
	DEC_SEQ_OPTION = MFC_BASE+0x188,
	
	DEC_SEQ_PRO_BUF = MFC_BASE+0x18c,
	DEC_SEQ_TMP_BUF_1 = MFC_BASE+0x190,
	DEC_SEQ_TMP_BUF_2 = MFC_BASE+0x194,
	DEC_SEQ_TMP_BUF_3 = MFC_BASE+0x198,
	DEC_SEQ_TMP_BUF_4 = MFC_BASE+0x19c,
	DEC_SEQ_TMP_BUF_5 = MFC_BASE+0x1a0,
	// R reg
	RET_SEQ_SUCCESS = MFC_BASE+0x1c0,
	RET_DEC_SEQ_SRC_SIZE = MFC_BASE+0x1c4,
	RET_DEC_SEQ_SRC_FRAME_RATE = MFC_BASE+0x1c8,
	RET_DEC_SEQ_FRAME_NEED_COUNT = MFC_BASE+0x1cc,
	RET_DEC_SEQ_FRAME_DELAY = MFC_BASE+0x1d0,
	RET_DEC_SEQ_INFO = MFC_BASE+0x1d4,

	// ENC_SEQ_INIT Parameter reg
	// R/W reg
	ENC_SEQ_BIT_BUF_ADDR = MFC_BASE+0x180,
	ENC_SEQ_BIT_BUF_SIZE = MFC_BASE+0x184,
	ENC_SEQ_OPTION = MFC_BASE+0x188,
	ENC_SEQ_COD_STD = MFC_BASE+0x18c,
	ENC_SEQ_SRC_SIZE = MFC_BASE+0x190,
	ENC_SEQ_SRC_FRAME_RATE = MFC_BASE+0x194,
	ENC_SEQ_MP4_PARA = MFC_BASE+0x198,
	ENC_SEQ_263_PARA = MFC_BASE+0x19c,
	ENC_SEQ_264_PARA = MFC_BASE+0x1a0,
	ENC_SEQ_SLICE_MODE = MFC_BASE+0x1a4,
	ENC_SEQ_GOP_NUM = MFC_BASE+0x1a8,
	ENC_SEQ_RC_PARA = MFC_BASE+0x1ac,
	ENC_SEQ_RC_BUF_SIZE = MFC_BASE+0x1b0,
	ENC_SEQ_INTRA_MB = MFC_BASE+0x1b4,
	ENC_SEQ_FMO = MFC_BASE+0x1b8,
	
	ENC_SEQ_TMP_BUF_1 = MFC_BASE+0x1d0,
	ENC_SEQ_TMP_BUF_2 = MFC_BASE+0x1d4,
	ENC_SEQ_TMP_BUF_3 = MFC_BASE+0x1d8,
	ENC_SEQ_TMP_BUF_4 = MFC_BASE+0x1dc,
	
	// R reg
	//ENC_SEQ_SUCCESS = MFC_BASE+0x1c0, // same as DEC_SEQ_INIT

	// DEC_PIC_RUN Parameter reg
	// R/W reg
	DEC_PIC_ROT_MODE = MFC_BASE+0x180,
	DEC_PIC_ROT_ADDR_Y = MFC_BASE+0x184,
	DEC_PIC_ROT_ADDR_CB = MFC_BASE+0x188,
	DEC_PIC_ROT_ADDR_CR = MFC_BASE+0x18c,
	
	DEC_PIC_DEBLOCK_ADDR_Y = MFC_BASE+0x190,
	DEC_PIC_DEBLOCK_ADDR_CB = MFC_BASE+0x194,
	DEC_PIC_DEBLOCK_ADDR_CR = MFC_BASE+0x198,
	DEC_PIC_ROT_STRIDE = MFC_BASE+0x19c,
	DEC_PIC_CHUNK_SIZE = MFC_BASE+0x1a8,
	DEC_PIC_BB_START = MFC_BASE+0x1ac,
	DEC_PIC_START_BYTE = MFC_BASE+0x1b0,
	
	// R reg
	RET_PIC_FRAME_NUM = MFC_BASE+0x1c0,
	RET_DEC_PIC_INDEX = MFC_BASE+0x1c4,
	RET_DEC_PIC_ERR_MB_NUM = MFC_BASE+0x1c8,
	RET_DEC_PIC_TYPE = MFC_BASE+0x1cc,

	// ENC_PIC_RUN Parameter reg
	// R/W reg
	ENC_PIC_SRC_ADDR_Y = MFC_BASE+0x180,
	ENC_PIC_SRC_ADDR_CB = MFC_BASE+0x184,
	ENC_PIC_SRC_ADDR_CR = MFC_BASE+0x188,
	ENC_PIC_QS = MFC_BASE+0x18c,
	ENC_PIC_ROT_MODE = MFC_BASE+0x190,
	ENC_PIC_OPTION = MFC_BASE+0x194,
	// R reg
	// RET_ENC_PIC_FRAME_NUM = MFC_BASE+0x1c0,  // same as in DEC_PIC_RUN
	RET_ENC_PIC_TYPE = MFC_BASE+0x1c4,
	RET_ENC_PIC_INDEX = MFC_BASE+0x1c8,
	RET_ENC_PIC_SLICE_NUM = MFC_BASE+0x1cc,

	// SET_FRAME_BUFFER Parameter reg
	SET_FRAME_BUF_NUM = MFC_BASE+0x180,
	SET_FRAME_BUF_STRIDE = MFC_BASE+0x184,

	// ENC_HEADER Parameter reg
	ENC_HEADER_CODE = MFC_BASE+0x180,

	// DEC_PARA_SET reg
	DEC_PARA_SET_TYPE = MFC_BASE+0x180,
	DEC_PARA_SET_SIZE = MFC_BASE+0x184,

	// ENC_PARA_SET reg
	// R/W reg
	ENC_PARA_SET_TYPE = MFC_BASE+0x180,
	// R reg
	RET_ENC_PARA_SET_SIZE = MFC_BASE+0x1c0,

	// GET_FW_VER reg
	// R reg
	RET_GET_FW_VER       = MFC_BASE+0x1c0,
	
	SW_RESET              = MFC_BASE+0xe00,

	BITS_CODE_INPUT       = MFC_BASE+0x1000
};

// STREAM_BUF_CTRL (0x10c)
#define STREAM_ENDIAN_LITTLE (0<<0)
#define STREAM_ENDIAN_BIG (1<<0)
#define BUF_STATUS_FULL_EMPTY_CHECK_BIT (0<<1)
#define BUF_STATUS_NO_CHECK_BIT (1<<1)
#define NOT_FLUSH_BUF_IN_ENCODING (0<<2)
#define FLUSH_BUF_IN_ENCODING (1<<2)
#define RESET_BUF_AT_EVERY_ENCODING (2<<2)

// FRAME_BUF_CTRL (0x110)
#define FRAME_MEM_ENDIAN_LITTLE (0<<0)
#define FRAME_MEM_ENDIAN_BIG (1<<0)

// INT_REASON (0x170)
#define INT_DNLOAD_DONE_BIT (1<<0)
#define INT_SEQ_INIT_BIT (1<<1)
#define INT_SEQ_END_BIT (1<<2)
#define INT_PIC_RUN_BIT (1<<3)
#define INT_SET_FRAME_BUF (1<<4)
#define INT_ENC_HEADER (1<<5)
#define INT_ENC_PARA_SET (1<<6)
#define INT_DEC_PARA_SET (1<<7)
#define INT_BIT_BUF_EMPTY_BIT (1<<14)
#define INT_BIT_BUF_FULL_BIT (1<<15)
#define INT_ALL_BIT (0xc0ff)

// DEC_SEQ_OPTION (0x18c)
#define MP4_DBK_DISABLE (0<<0)
#define MP4_DBK_ENABLE (1<<0)
#define REORDER_DISABLE (0<<1)
#define REORDER_ENABLE (1<<1)
#define FILEPLAY_ENABLE (1<<2)
#define FILEPLAY_DISABLE (0<<2)
#define DYNBUFALLOC_ENABLE (1<<3)
#define DYNBUFALLOC_DISABLE (0<<3)

MFC oMfc;

void MFC_InitProcessForDecodingMpeg4(
	u32 uProcessIdx, u32 uStreamBufStAddr, u32 uStreamBufSize,
	u32 uFrameBufStAddr, u8 bDecRotEn, u8 bMp4DeblkEn)
{
	MFC_InitProcessForDecoding(uProcessIdx, MP4_DEC, uStreamBufStAddr, uStreamBufSize,
		uFrameBufStAddr, bDecRotEn, bMp4DeblkEn, false);
}

void MFC_InitProcessForDecodingH264(
	u32 uProcessIdx, u32 uStreamBufStAddr, u32 uStreamBufSize,
	u32 uFrameBufStAddr, u8 bDecRotEn, u8 bH264ReorderEn)
{
	MFC_InitProcessForDecoding(uProcessIdx, AVC_DEC, uStreamBufStAddr, uStreamBufSize,
		uFrameBufStAddr, bDecRotEn, false, bH264ReorderEn);
}

void MFC_InitProcessForDecodingVc1(
	u32 uProcessIdx, u32 uStreamBufStAddr, u32 uStreamBufSize, u32 uFrameBufStAddr, u8 bDecRotEn)
{
	MFC_InitProcessForDecoding(uProcessIdx, VC1_DEC, uStreamBufStAddr, uStreamBufSize,
		uFrameBufStAddr, bDecRotEn, false, false);
}

void MFC_InitProcessForDmbDecoding(
	u32 uProcessIdx, u32 uStreamBufStAddr, u32 uStreamBufSize, u32 uFrameBufStAddr, u8 bDecRotEn)
{
	u32 uStreamBufSizeCeilingToKbMultiple;
	u8 stat;
	float frameRate;
	u32 picX, picY;
	u32 uNumOfRefReconFrame, uFrameBufNum;
	u32 uStride, uHeight;

	Assert(uProcessIdx < MAX_PROCESS_NUM);

	oMfc.m_eCodecMode[uProcessIdx] = AVC_DEC;
	uStreamBufSizeCeilingToKbMultiple = (uStreamBufSize+1023)/1024*1024;

	oMfc.m_uStreamBufStAddr[uProcessIdx] = uStreamBufStAddr;
	oMfc.m_uStreamBufEndAddr[uProcessIdx] = uStreamBufStAddr + uStreamBufSizeCeilingToKbMultiple;
	oMfc.m_uStreamBufByteSize[uProcessIdx] = uStreamBufSizeCeilingToKbMultiple;
		
	oMfc.m_uBitRdPtr[uProcessIdx] = 
		(uProcessIdx == 0) ? BITS_RD_PTR0 :
		(uProcessIdx == 1) ? BITS_RD_PTR1 :
		(uProcessIdx == 2) ? BITS_RD_PTR2 :
		(uProcessIdx == 3) ? BITS_RD_PTR3 :
		(uProcessIdx == 4) ? BITS_RD_PTR4 :
		(uProcessIdx == 5) ? BITS_RD_PTR5 :	
		(uProcessIdx == 6) ? BITS_RD_PTR6 : BITS_RD_PTR7;
	oMfc.m_uBitWrPtr[uProcessIdx] = 
		(uProcessIdx == 0) ? BITS_WR_PTR0 :
		(uProcessIdx == 1) ? BITS_WR_PTR1 :
		(uProcessIdx == 2) ? BITS_WR_PTR2 :
		(uProcessIdx == 3) ? BITS_WR_PTR3 :
		(uProcessIdx == 4) ? BITS_WR_PTR4 :
		(uProcessIdx == 5) ? BITS_WR_PTR5 :			
		(uProcessIdx == 6) ? BITS_WR_PTR6 : BITS_WR_PTR7;		

	mfcOutp32(oMfc.m_uBitRdPtr[uProcessIdx], oMfc.m_uStreamBufStAddr[uProcessIdx]);
	mfcOutp32(oMfc.m_uBitWrPtr[uProcessIdx], oMfc.m_uStreamBufStAddr[uProcessIdx]+uStreamBufSize);	

	mfcOutp32(DEC_SEQ_BIT_BUF_ADDR, oMfc.m_uStreamBufStAddr[uProcessIdx]);
    mfcOutp32(DEC_SEQ_BIT_BUF_SIZE,  oMfc.m_uStreamBufByteSize[uProcessIdx]/1024);    // KB unit

	mfcOutp32(DEC_SEQ_OPTION, MP4_DBK_DISABLE|REORDER_DISABLE);
	oMfc.m_bIsNoMoreStream[uProcessIdx] = false;

	MFC_IssueCmd(uProcessIdx, SEQ_INIT);

	stat = MFC_IsCmdFinished();

	if(stat == false)
	{
		UART_Printf("\n There is an error in the SEQ_INIT result\n");
		return;
	}

	MFC_GetDecSrcFormat(&picX, &picY, &frameRate);
	oMfc.m_uPicX[uProcessIdx] = picX;
	oMfc.m_uPicY[uProcessIdx] = picY;
    UART_Printf("%d x %d @%.2f Hz\n", picX, picY, frameRate);
	Assert(picX > 0);
	Assert(picY > 0);
	
	oMfc.m_bDecRotEn[uProcessIdx] = bDecRotEn;	
	oMfc.m_uRotFrameIdx[uProcessIdx] = 0;
	oMfc.m_uMp4DeblockFrameIdx[uProcessIdx] = 0;
	oMfc.m_uFrameIndex[uProcessIdx] = 0;

	oMfc.m_uFrameDelayCount[uProcessIdx] = mfcInp32(RET_DEC_SEQ_FRAME_DELAY);
	//DbgMfc("Delay Frame num=%d\n", oMfc.m_uFrameDelayCount[uProcessIdx]);

	uNumOfRefReconFrame = 5; // Ref:3 + Recon:2
	oMfc.m_uRefFrameNum[uProcessIdx] = uNumOfRefReconFrame;
	uFrameBufNum = (oMfc.m_bDecRotEn[uProcessIdx]) ? uNumOfRefReconFrame+2 : uNumOfRefReconFrame;
	 
	uStride = (picX%16 == 0) ? picX : (picX+15)/16*16;
	uHeight = (picY%16 == 0) ? picY : (picY+15)/16*16;
	Assert(uStride <= 352);
	Assert(uHeight <= 288);
	MFC_InitDecFrameBuffer(uProcessIdx, uFrameBufNum, uStride, uHeight, uFrameBufStAddr);
			
	MFC_IssueCmdOfSetFrameBuffer(uProcessIdx, uNumOfRefReconFrame, uStride);
}

void MFC_InitProcessForDecoding(
	u32 uProcessIdx, MFC_CODEC_MODE eCodecMode, u32 uStreamBufStAddr, u32 uStreamBufSize,
	u32 uFrameBufStAddr, u8 bDecRotEn, u8 bMp4DeblkEn, u8 bH264ReorderEn)
{
	u32 uStreamBufSizeCeilingToKbMultiple;
	u32 uMp4DecDeblkMode;
	u32 uH264DecReorderMode;
	u8 stat;
	float frameRate;
	u32 picX, picY;
	u32 uNumOfRefReconFrame;
	u32 uStride;
	u32 uHeight;
	u32 uFrameBufNumTemp;
	u32 uFrameBufNum;

	Assert(uProcessIdx < MAX_PROCESS_NUM);
	Assert(eCodecMode == MP4_DEC || eCodecMode == AVC_DEC || eCodecMode == VC1_DEC);

	oMfc.m_eCodecMode[uProcessIdx] = eCodecMode;
	uStreamBufSizeCeilingToKbMultiple = (uStreamBufSize+1023)/1024*1024;

	oMfc.m_uStreamBufStAddr[uProcessIdx] = uStreamBufStAddr;
	oMfc.m_uStreamBufEndAddr[uProcessIdx] = uStreamBufStAddr + uStreamBufSizeCeilingToKbMultiple;
	oMfc.m_uStreamBufByteSize[uProcessIdx] = uStreamBufSizeCeilingToKbMultiple;
		
	oMfc.m_uBitRdPtr[uProcessIdx] = 
		(uProcessIdx == 0) ? BITS_RD_PTR0 :
		(uProcessIdx == 1) ? BITS_RD_PTR1 :
		(uProcessIdx == 2) ? BITS_RD_PTR2 :
		(uProcessIdx == 3) ? BITS_RD_PTR3 :
		(uProcessIdx == 4) ? BITS_RD_PTR4 :
		(uProcessIdx == 5) ? BITS_RD_PTR5 :	
		(uProcessIdx == 6) ? BITS_RD_PTR6 : BITS_RD_PTR7;
	oMfc.m_uBitWrPtr[uProcessIdx] = 
		(uProcessIdx == 0) ? BITS_WR_PTR0 :
		(uProcessIdx == 1) ? BITS_WR_PTR1 :
		(uProcessIdx == 2) ? BITS_WR_PTR2 :
		(uProcessIdx == 3) ? BITS_WR_PTR3 :
		(uProcessIdx == 4) ? BITS_WR_PTR4 :
		(uProcessIdx == 5) ? BITS_WR_PTR5 :			
		(uProcessIdx == 6) ? BITS_WR_PTR6 : BITS_WR_PTR7;		


	mfcOutp32(oMfc.m_uBitRdPtr[uProcessIdx], oMfc.m_uStreamBufStAddr[uProcessIdx]);
	mfcOutp32(oMfc.m_uBitWrPtr[uProcessIdx], oMfc.m_uStreamBufStAddr[uProcessIdx]+uStreamBufSize);	

	mfcOutp32(DEC_SEQ_BIT_BUF_ADDR, oMfc.m_uStreamBufStAddr[uProcessIdx]);
    mfcOutp32(DEC_SEQ_BIT_BUF_SIZE,  oMfc.m_uStreamBufByteSize[uProcessIdx]/1024);    // KB unit

	oMfc.m_bMp4DecDeblkMode[uProcessIdx] = (eCodecMode == MP4_DEC) ? bMp4DeblkEn : false;   
	uMp4DecDeblkMode = (oMfc.m_bMp4DecDeblkMode[uProcessIdx]) ? MP4_DBK_ENABLE : MP4_DBK_DISABLE;
	uH264DecReorderMode = (bH264ReorderEn) ? REORDER_ENABLE : REORDER_DISABLE;
	mfcOutp32(DEC_SEQ_OPTION, uMp4DecDeblkMode|uH264DecReorderMode);

	oMfc.m_bIsNoMoreStream[uProcessIdx] = false;

	MFC_IssueCmd(uProcessIdx, SEQ_INIT);

	stat = MFC_IsCmdFinished();

	if(stat == false)
	{
		UART_Printf("\n There is an error in the SEQ_INIT result\n");
		return;
	}

	//u32 uFrameBufAddr = oMfc.m_uStreamBufEndAddr[uProcessIdx]+STREAM_WR_SIZE;

	MFC_GetDecSrcFormat(&picX, &picY, &frameRate);
	oMfc.m_uPicX[uProcessIdx] = picX;
	oMfc.m_uPicY[uProcessIdx] = picY;
    UART_Printf("%d x %d @%.2f Hz\n", picX, picY, frameRate);
	Assert(picX > 0);
	Assert(picY > 0);
	
	oMfc.m_bDecRotEn[uProcessIdx] = bDecRotEn;	
	oMfc.m_uRotFrameIdx[uProcessIdx] = 0;
	oMfc.m_uMp4DeblockFrameIdx[uProcessIdx] = 0;
	oMfc.m_uFrameIndex[uProcessIdx] = 0;

	// H.263 Annex J deblock is in-loop filter, otherwise deblock is out-loop filter
	MFC_IsDecH263AnnexJOn(uProcessIdx, &oMfc.m_bAnnexJOn[uProcessIdx]); 	
	
	oMfc.m_uFrameDelayCount[uProcessIdx] = mfcInp32(RET_DEC_SEQ_FRAME_DELAY);
	//DbgMfc("Delay Frame num=%d\n", oMfc.m_uFrameDelayCount[uProcessIdx]);

    MFC_GetDecRefFrameNum(uProcessIdx, &uNumOfRefReconFrame);	
	//DbgMfc("num of RefFrame = %d\n", uNumOfRefReconFrame);
	
	uFrameBufNumTemp = (oMfc.m_bDecRotEn[uProcessIdx]) ? uNumOfRefReconFrame+2 : uNumOfRefReconFrame;
	uFrameBufNum = (oMfc.m_bMp4DecDeblkMode[uProcessIdx] && !oMfc.m_bAnnexJOn[uProcessIdx]) ? uFrameBufNumTemp+2 : uFrameBufNumTemp;
	 
	uStride = (picX%16 == 0) ? picX : (picX+15)/16*16;
	uHeight = (picY%16 == 0) ? picY : (picY+15)/16*16;		
	MFC_InitDecFrameBuffer(uProcessIdx, uFrameBufNum, uStride, uHeight, uFrameBufStAddr);
			
	MFC_IssueCmdOfSetFrameBuffer(uProcessIdx, uNumOfRefReconFrame, uStride);
}

void MFC_InitProcessForOneFrameDecoding(
	u32 uProcessIdx, MFC_CODEC_MODE eCodecMode, u32 uStreamBufStAddr, u32 uStreamBufSize,
	u32 uFrameBufStAddr, u8 bDecRotEn, u8 bMp4DeblkEn, u8 bH264ReorderEn, u8 bFilePlayEn, u8 bDynBufAllocEn)
{
	u32 uFilePlayEn;
	u32 uDynBufEn;
	u32 uStreamBufSizeCeilingToKbMultiple;
	u32 uMp4DecDeblkMode;
	u32 uH264DecReorderMode;
	u8 stat;
	float frameRate;
	u32 picX, picY;
	u32 uNumOfRefReconFrame;
	u32 uStride;
	u32 uHeight;
	u32 uFrameBufNumTemp;
	u32 uFrameBufNum;


	Assert(uProcessIdx < MAX_PROCESS_NUM);
	Assert(eCodecMode == MP4_DEC || eCodecMode == AVC_DEC || eCodecMode == VC1_DEC);

	oMfc.m_eCodecMode[uProcessIdx] = eCodecMode;
	uStreamBufSizeCeilingToKbMultiple = (uStreamBufSize+1023)/1024*1024;

	oMfc.m_uStreamBufStAddr[uProcessIdx] = uStreamBufStAddr;
	oMfc.m_uStreamBufEndAddr[uProcessIdx] = uStreamBufStAddr + uStreamBufSizeCeilingToKbMultiple;
	oMfc.m_uStreamBufByteSize[uProcessIdx] = uStreamBufSizeCeilingToKbMultiple;
		
	oMfc.m_uBitRdPtr[uProcessIdx] = 
		(uProcessIdx == 0) ? BITS_RD_PTR0 :
		(uProcessIdx == 1) ? BITS_RD_PTR1 :
		(uProcessIdx == 2) ? BITS_RD_PTR2 :
		(uProcessIdx == 3) ? BITS_RD_PTR3 :
		(uProcessIdx == 4) ? BITS_RD_PTR4 :
		(uProcessIdx == 5) ? BITS_RD_PTR5 :	
		(uProcessIdx == 6) ? BITS_RD_PTR6 : BITS_RD_PTR7;
	oMfc.m_uBitWrPtr[uProcessIdx] = 
		(uProcessIdx == 0) ? BITS_WR_PTR0 :
		(uProcessIdx == 1) ? BITS_WR_PTR1 :
		(uProcessIdx == 2) ? BITS_WR_PTR2 :
		(uProcessIdx == 3) ? BITS_WR_PTR3 :
		(uProcessIdx == 4) ? BITS_WR_PTR4 :
		(uProcessIdx == 5) ? BITS_WR_PTR5 :			
		(uProcessIdx == 6) ? BITS_WR_PTR6 : BITS_WR_PTR7;		


	mfcOutp32(oMfc.m_uBitRdPtr[uProcessIdx], oMfc.m_uStreamBufStAddr[uProcessIdx]);
	mfcOutp32(oMfc.m_uBitWrPtr[uProcessIdx], oMfc.m_uStreamBufStAddr[uProcessIdx]+uStreamBufSize);	

	mfcOutp32(DEC_SEQ_BIT_BUF_ADDR, oMfc.m_uStreamBufStAddr[uProcessIdx]);
    	mfcOutp32(DEC_SEQ_BIT_BUF_SIZE,  oMfc.m_uStreamBufByteSize[uProcessIdx]/1024);    // KB unit

	oMfc.m_bMp4DecDeblkMode[uProcessIdx] = (eCodecMode == MP4_DEC) ? bMp4DeblkEn : false;   
	uMp4DecDeblkMode = (oMfc.m_bMp4DecDeblkMode[uProcessIdx]) ? MP4_DBK_ENABLE : MP4_DBK_DISABLE;
	uH264DecReorderMode = (bH264ReorderEn) ? REORDER_ENABLE : REORDER_DISABLE;
	uFilePlayEn = (bFilePlayEn) ? FILEPLAY_ENABLE : FILEPLAY_DISABLE;
	uDynBufEn = (bDynBufAllocEn) ? DYNBUFALLOC_ENABLE : DYNBUFALLOC_DISABLE;
	mfcOutp32(DEC_SEQ_OPTION, uMp4DecDeblkMode|uH264DecReorderMode|uFilePlayEn|uDynBufEn);

	oMfc.m_bIsNoMoreStream[uProcessIdx] = false;

	MFC_IssueCmd(uProcessIdx, SEQ_INIT);

	stat = MFC_IsCmdFinished();

	if(stat == false)
	{
		UART_Printf("\n There is an error in the SEQ_INIT result\n");
		return;
	}

	MFC_GetDecSrcFormat(&picX, &picY, &frameRate);
	oMfc.m_uPicX[uProcessIdx] = picX;
	oMfc.m_uPicY[uProcessIdx] = picY;
    	DbgMfc("%d x %d @%.2f Hz\n", picX, picY, frameRate);
	Assert(picX > 0);
	Assert(picY > 0);
	
	oMfc.m_bDecRotEn[uProcessIdx] = bDecRotEn;	
	oMfc.m_uRotFrameIdx[uProcessIdx] = 0;
	oMfc.m_uMp4DeblockFrameIdx[uProcessIdx] = 0;
	oMfc.m_uFrameIndex[uProcessIdx] = 0;

	// H.263 Annex J deblock is in-loop filter, otherwise deblock is out-loop filter
	MFC_IsDecH263AnnexJOn(uProcessIdx, &oMfc.m_bAnnexJOn[uProcessIdx]); 	
	
//	mfcInp32(RET_DEC_SEQ_FRAME_DELAY, oMfc.m_uFrameDelayCount[uProcessIdx]);
	oMfc.m_uFrameDelayCount[uProcessIdx] = mfcInp32(RET_DEC_SEQ_FRAME_DELAY);

	DbgMfc("Delay Frame num=%d\n", oMfc.m_uFrameDelayCount[uProcessIdx]);

       MFC_GetDecRefFrameNum(uProcessIdx, &uNumOfRefReconFrame);	
	DbgMfc("num of RefFrame = %d\n", uNumOfRefReconFrame);
	
	uFrameBufNumTemp = (oMfc.m_bDecRotEn[uProcessIdx]) ? uNumOfRefReconFrame+2 : uNumOfRefReconFrame;
	uFrameBufNum = (oMfc.m_bMp4DecDeblkMode[uProcessIdx] && !oMfc.m_bAnnexJOn[uProcessIdx]) ? uFrameBufNumTemp+2 : uFrameBufNumTemp;
	 
	uStride = (picX%16 == 0) ? picX : (picX+15)/16*16;
	uHeight = (picY%16 == 0) ? picY : (picY+15)/16*16;		
	MFC_InitDecFrameBuffer(uProcessIdx, uFrameBufNum, uStride, uHeight, uFrameBufStAddr);
	MFC_IssueCmdOfSetFrameBuffer(uProcessIdx, uNumOfRefReconFrame, uStride);

}

void MFC_InitStreamBufferForOneFrameDecoding(u32 uProcessIdx,  u32 uStreamBufStAddr, u32 uStreamBufSize)
{

	mfcOutp32(DEC_PIC_BB_START,uStreamBufStAddr & 0xfffffffc);
	mfcOutp32(DEC_PIC_START_BYTE,uStreamBufStAddr & 0x3);
	mfcOutp32(DEC_PIC_CHUNK_SIZE,uStreamBufSize);
}

u8 MFC_DecodeOneFrame(u32 uRunIdx, u32* uFrameIdx)
{
	MFC_StartDecodingOneFrame(uRunIdx);
	MFC_WaitForReady();
	return MFC_DoPostProcessingOfDecoding(uRunIdx, false, uFrameIdx);
}

void MFC_StartDecodingOneFrame(u32 uRunIdx)
{
	u32 i, j, uDbkFrameNum=0;
	u32 uRotMode, uRotStride;
	
	if (oMfc.m_bMp4DecDeblkMode[uRunIdx] && !oMfc.m_bAnnexJOn[uRunIdx]) // out-loop filter
	{
		uDbkFrameNum = 2;
		i = oMfc.m_uRefFrameNum[uRunIdx] + oMfc.m_uMp4DeblockFrameIdx[uRunIdx];		
		mfcOutp32(DEC_PIC_DEBLOCK_ADDR_Y, oMfc.m_uFrameAddrY[uRunIdx][i]);
		mfcOutp32(DEC_PIC_DEBLOCK_ADDR_CB, oMfc.m_uFrameAddrCb[uRunIdx][i]);
		mfcOutp32(DEC_PIC_DEBLOCK_ADDR_CR, oMfc.m_uFrameAddrCr[uRunIdx][i]);
	}
	if (oMfc.m_bDecRotEn[uRunIdx]) // out-loop filter
	{		
		j = oMfc.m_uRefFrameNum[uRunIdx] + uDbkFrameNum + oMfc.m_uRotFrameIdx[uRunIdx];
		mfcOutp32(DEC_PIC_ROT_ADDR_Y, oMfc.m_uFrameAddrY[uRunIdx][j]);
		mfcOutp32(DEC_PIC_ROT_ADDR_CB, oMfc.m_uFrameAddrCb[uRunIdx][j]);
		mfcOutp32(DEC_PIC_ROT_ADDR_CR, oMfc.m_uFrameAddrCr[uRunIdx][j]);

		uRotMode = mfcInp32(DEC_PIC_ROT_MODE);
		uRotStride = (uRotMode&1) ? oMfc.m_uPicY[uRunIdx] : oMfc.m_uStride[uRunIdx]; // Rot90 or Rot270
		MFC_SetDecRotStride(uRotStride);
		//DbgMfc("rot stride = %d\n",uRotStride);
	}

	MFC_StartIssuingCmd(uRunIdx, PIC_RUN);
}

u8 MFC_DoPostProcessingOfDecoding(u32 uRunIdx, u8 isEmpty, u32* uFrameIdx)
{
	u32 uDeblockFrameNum = 0;

	if (isEmpty  && oMfc.m_bIsNoMoreStream[uRunIdx] && oMfc.m_uFrameDelayCount[uRunIdx]-- == 0)
	{
		return false;
	}
	
	MFC_GetDecReconFrameIndex(uRunIdx, &oMfc.m_uFrameIndex[uRunIdx]);
	if (oMfc.m_uFrameIndex[uRunIdx] == -1) // 0xffff_ffff
	{
		*uFrameIdx = oMfc.m_uFrameIndex[uRunIdx];
		return false;
	}
	
	uDeblockFrameNum = 0;
	if (oMfc.m_bMp4DecDeblkMode[uRunIdx] && !oMfc.m_bAnnexJOn[uRunIdx])
	{
		oMfc.m_uFrameIndex[uRunIdx] = oMfc.m_uRefFrameNum[uRunIdx] + oMfc.m_uMp4DeblockFrameIdx[uRunIdx];		
		oMfc.m_uMp4DeblockFrameIdx[uRunIdx] = (oMfc.m_uMp4DeblockFrameIdx[uRunIdx]+1)%2;
		uDeblockFrameNum = 2;
	}
	// from 6th f/w (VC-1 path is different from other codec's.)
	if (oMfc.m_bDecRotEn[uRunIdx] && oMfc.m_eCodecMode[uRunIdx] != VC1_DEC)
	{		 
		oMfc.m_uFrameIndex[uRunIdx] = oMfc.m_uRefFrameNum[uRunIdx] + uDeblockFrameNum + oMfc.m_uRotFrameIdx[uRunIdx];		
		oMfc.m_uRotFrameIdx[uRunIdx] = (oMfc.m_uRotFrameIdx[uRunIdx]+1)%2;
	}

	*uFrameIdx = oMfc.m_uFrameIndex[uRunIdx];
	DbgMfc("%d ", oMfc.m_uFrameIndex[uRunIdx]);
	
	return true;
}

void MFC_InitProcessForEncoding(
	u32 uProcessIdx,
	MFC_CODEC_MODE eEncMode,
	u32 uStreamBufStAddr,
	u32 uStreamBufSize,
	u32 uDispWidth,
	u32 uDispHeight,
	u8 bEncRot90En)
{
	MFC_InitProcessForEncoding1(uProcessIdx, eEncMode, uStreamBufStAddr, uStreamBufSize, 
		uDispWidth, uDispHeight, bEncRot90En, false, false, false, false);
}

void MFC_InitProcessForEncoding1(
	u32 uProcessIdx,
	MFC_CODEC_MODE eEncMode,
	u32 uStreamBufStAddr,
	u32 uStreamBufSize,
	u32 uDispWidth,
	u32 uDispHeight,
	u8 bEncRot90En,
	u8 bMbBitReport,
	u8 bSliceInfoReport,
	u8 bAudEnable,
	u8 bSkipDisable)
{
	u32 uRefFrameBufStAddr;
	u32 uRefFrameNum;
    u32 uWidth;
	u32 uHeight;
	u32 i;
	u32 uLongerLength;
	u32 uStride;

	MFC_InitEncSeq(uProcessIdx, eEncMode, uStreamBufStAddr, uStreamBufSize, bEncRot90En,
		bMbBitReport, bSliceInfoReport, bAudEnable, bSkipDisable);
	uRefFrameBufStAddr = oMfc.m_uStreamBufEndAddr[uProcessIdx];

	// No reordering. Double buffer used.
    // FRAME BUFFER SET    
    //      FRAME 0/1 : RECONSTRUCT (Post Processor input)
    oMfc.m_uRefFrameNum[uProcessIdx] = 2;
	uRefFrameNum = oMfc.m_uRefFrameNum[uProcessIdx];
    uWidth = (bEncRot90En) ? uDispHeight : uDispWidth;
	uHeight = (bEncRot90En) ? uDispWidth : uDispHeight;	
    MFC_InitEncFrameBuffer(uProcessIdx,  uRefFrameNum, uWidth, uHeight, uRefFrameBufStAddr);

	// Clear recon frame
	for (i=0; i<uRefFrameNum; i++)
	{
		MFC_ClearFrameBuffer(uProcessIdx, i);
	}	

	// set stride to max(h,w) instead of h
	uLongerLength = (uDispWidth > uDispHeight) ? uDispWidth : uDispHeight;
	uStride = (bEncRot90En) ? uLongerLength : uDispWidth;

	MFC_IssueCmdOfSetFrameBuffer(uProcessIdx, uRefFrameNum, uStride);

}

void MFC_IssueCmdOfSetFrameBuffer(u32 uProcessIdx, u32 uNumOfRefReconFrame, u32 uStride)
{
	u32 i;
	// Reference and Recon frames start addr setting for Command "SET_FRAME_BUF"
	for (i=0; i<uNumOfRefReconFrame; i++)
	{
		mfcOutp32(oMfc.m_uParameterBufStartAddr+i*3*4, oMfc.m_uFrameAddrY[uProcessIdx][i]);
		mfcOutp32(oMfc.m_uParameterBufStartAddr+i*3*4+4, oMfc.m_uFrameAddrCb[uProcessIdx][i]);
		mfcOutp32(oMfc.m_uParameterBufStartAddr+i*3*4+8, oMfc.m_uFrameAddrCr[uProcessIdx][i]);
	}

	mfcOutp32(SET_FRAME_BUF_NUM, uNumOfRefReconFrame);
	mfcOutp32(SET_FRAME_BUF_STRIDE, uStride);
	MFC_IssueCmd(uProcessIdx, SET_FRAME_BUF);
}

void MFC_EncodeOneFrame(u32 uProcessIdx, u32 uAddrOfSrcYFrame, u32* uRecFrameIdx)
{
	MFC_StartEncodingOneFrame(uProcessIdx, uAddrOfSrcYFrame);
    MFC_WaitForReady();
	MFC_DoPostProcessingOfEncoding(uProcessIdx, uRecFrameIdx);
}

void MFC_StartEncodingOneFrame(u32 uProcessIdx, u32 uAddrOfSrcYFrame)
{
	u32 uAddrOfSrcCbFrame, uAddrOfSrcCrFrame;

	mfcOutp32(ENC_PIC_SRC_ADDR_Y, uAddrOfSrcYFrame);
	uAddrOfSrcCbFrame = uAddrOfSrcYFrame+oMfc.m_uStride[uProcessIdx]*oMfc.m_uHeight[uProcessIdx];
	mfcOutp32(ENC_PIC_SRC_ADDR_CB, uAddrOfSrcCbFrame);
	uAddrOfSrcCrFrame = uAddrOfSrcYFrame+oMfc.m_uStride[uProcessIdx]*oMfc.m_uHeight[uProcessIdx]*5/4;
	mfcOutp32(ENC_PIC_SRC_ADDR_CR, uAddrOfSrcCrFrame);

	if (oMfc.m_eCodecMode[uProcessIdx] == MP4_ENC)
	{
		MFC_SetEncPicQs(MP4_ENC, oMfc.m_EncCfg[uProcessIdx].VopQuant);
	}
	else
	{
		MFC_SetEncPicQs(AVC_ENC, oMfc.m_EncCfg[uProcessIdx].PicQpY);
	}

	MFC_StartIssuingCmd(uProcessIdx, PIC_RUN);
}


void MFC_DoPostProcessingOfEncoding(u32 uProcessIdx, u32* uRecFrameIdx)
{
	MFC_GetEncReconFrameIndex(&oMfc.m_uFrameIndex[uProcessIdx]);	
	*uRecFrameIdx = oMfc.m_uFrameIndex[uProcessIdx];
}


void MFC_InitEncSeq(
	u32 uProcessIdx,
	MFC_CODEC_MODE eEncMode,
	u32 uStreamBufStAddr,
	u32 uStreamBufSize,
	u8 bEncRot90En,
	u8 bMbBitReport,
	u8 bSliceInfoReport,
	u8 bAudEnable,
	u8 bSkipDisable)
{
	u32 uSkipDisableBit;
	u32 uMbBitReportBit;
	u32 uSliceInfoReportBit;
	u32 uAudEnableBit;	
	u32 uEncSeqOptionValue;
	u8 stat;

	Assert(uProcessIdx < MAX_PROCESS_NUM);
	Assert(eEncMode == AVC_ENC || eEncMode == MP4_ENC);

	oMfc.m_eCodecMode[uProcessIdx] = eEncMode;

	oMfc.m_uStreamBufStAddr[uProcessIdx] = uStreamBufStAddr;
	oMfc.m_uStreamBufEndAddr[uProcessIdx] = uStreamBufStAddr + uStreamBufSize;
	oMfc.m_uStreamBufByteSize[uProcessIdx] = uStreamBufSize;		

	oMfc.m_uBitRdPtr[uProcessIdx] = 
		(uProcessIdx == 0) ? BITS_RD_PTR0 :
		(uProcessIdx == 1) ? BITS_RD_PTR1 :
		(uProcessIdx == 2) ? BITS_RD_PTR2 :
		(uProcessIdx == 3) ? BITS_RD_PTR3 :
		(uProcessIdx == 4) ? BITS_RD_PTR4 :
		(uProcessIdx == 5) ? BITS_RD_PTR5 :	
		(uProcessIdx == 6) ? BITS_RD_PTR6 : BITS_RD_PTR7;
	oMfc.m_uBitWrPtr[uProcessIdx] = 
		(uProcessIdx == 0) ? BITS_WR_PTR0 :
		(uProcessIdx == 1) ? BITS_WR_PTR1 :
		(uProcessIdx == 2) ? BITS_WR_PTR2 :
		(uProcessIdx == 3) ? BITS_WR_PTR3 :
		(uProcessIdx == 4) ? BITS_WR_PTR4 :
		(uProcessIdx == 5) ? BITS_WR_PTR5 :			
		(uProcessIdx == 6) ? BITS_WR_PTR6 : BITS_WR_PTR7;		
		
    mfcOutp32(oMfc.m_uBitWrPtr[uProcessIdx], oMfc.m_uStreamBufStAddr[uProcessIdx]);
    mfcOutp32(oMfc.m_uBitRdPtr[uProcessIdx], oMfc.m_uStreamBufStAddr[uProcessIdx]+uStreamBufSize);


    // ENCODER PARAMETER SET
    // Qs will be set in the stage of PIC_RUN
    if (oMfc.m_eCodecMode[uProcessIdx] == MP4_ENC)
	{
		// ShortVideoHeader - 0 : MPEG4 SP, 1 : MPEG4 short video header/H.263P3        
		MFC_SetEncCodStd(oMfc.m_EncCfg[uProcessIdx].ShortVideoHeader);		
        mfcOutp32(ENC_SEQ_MP4_PARA, 
			(oMfc.m_EncCfg[uProcessIdx].IntraDcVlcThr << 2) | 
			(oMfc.m_EncCfg[uProcessIdx].RevVlcEn << 1) | 
			oMfc.m_EncCfg[uProcessIdx].DataPartEn);
        mfcOutp32(ENC_SEQ_263_PARA, 
			(oMfc.m_EncCfg[uProcessIdx].AnnexI << 3) | 
			(oMfc.m_EncCfg[uProcessIdx].AnnexJ << 2) | 
			(oMfc.m_EncCfg[uProcessIdx].AnnexK << 1) | 
			oMfc.m_EncCfg[uProcessIdx].AnnexT);		
    } 
	else if (oMfc.m_eCodecMode[uProcessIdx] == AVC_ENC)
	{
		MFC_SetEncCodStd(2); // 2 means H.264 Encoding		
		MFC_SetEnc264Parameter(
			oMfc.m_EncCfg[uProcessIdx].ChromaQpOffset, 
			oMfc.m_EncCfg[uProcessIdx].ConstIntraPredFlag, 
			oMfc.m_EncCfg[uProcessIdx].DisableDeblk,
			oMfc.m_EncCfg[uProcessIdx].DeblkOffsetA,
			oMfc.m_EncCfg[uProcessIdx].DeblkOffsetB);	
		mfcOutp32(ENC_SEQ_FMO, 
			((oMfc.m_EncCfg[uProcessIdx].FmoType)<<5)|
			((oMfc.m_EncCfg[uProcessIdx].NumOfSliceGroup)<<1)|
			((oMfc.m_EncCfg[uProcessIdx].FmoEnable)<<0));
    }
	if (!bEncRot90En)
	{
		MFC_SetEncSrcFormat(oMfc.m_EncCfg[uProcessIdx].PicX, oMfc.m_EncCfg[uProcessIdx].PicY, oMfc.m_EncCfg[uProcessIdx].FrameRate);
	}
	else
	{
		MFC_SetEncSrcFormat(oMfc.m_EncCfg[uProcessIdx].PicY, oMfc.m_EncCfg[uProcessIdx].PicX, oMfc.m_EncCfg[uProcessIdx].FrameRate);
	}
//	DbgMfc("frame rate=%d, picX=%d, picY=%d\n", 
//		oMfc.m_EncCfg[uProcessIdx].FrameRate, oMfc.m_EncCfg[uProcessIdx].PicX, oMfc.m_EncCfg[uProcessIdx].PicY);

    mfcOutp32(ENC_SEQ_SLICE_MODE, 
		(oMfc.m_EncCfg[uProcessIdx].SliceSizeNum << 2) | 
		(oMfc.m_EncCfg[uProcessIdx].SliceSizeMode << 1) | 
		oMfc.m_EncCfg[uProcessIdx].SliceMode);    
	MFC_SetEncGopNum(oMfc.m_EncCfg[uProcessIdx].GopPicNum);
	uSkipDisableBit = (oMfc.m_bSkipDisable[uProcessIdx]) ? 1 : 0;
    mfcOutp32(ENC_SEQ_RC_PARA,
		(uSkipDisableBit << 31) |
		(oMfc.m_EncCfg[uProcessIdx].RcInitDelay << 16) | 
		(oMfc.m_EncCfg[uProcessIdx].RcBitRate << 1) | 
		oMfc.m_EncCfg[uProcessIdx].RcEnable);
    mfcOutp32(ENC_SEQ_RC_BUF_SIZE, oMfc.m_EncCfg[uProcessIdx].RcBufSize);
    mfcOutp32(ENC_SEQ_INTRA_MB, oMfc.m_EncCfg[uProcessIdx].IntraRefreshNum);

    mfcOutp32(ENC_SEQ_BIT_BUF_ADDR, oMfc.m_uStreamBufStAddr[uProcessIdx]);
    mfcOutp32(ENC_SEQ_BIT_BUF_SIZE, uStreamBufSize/1024);    // KB size

	oMfc.m_bMbBitReport[uProcessIdx] = bMbBitReport;  
	oMfc.m_bSliceInfoReport[uProcessIdx] = bSliceInfoReport;
	oMfc.m_bAudEnable[uProcessIdx] = bAudEnable;
	oMfc.m_bSkipDisable[uProcessIdx] = bSkipDisable;

	uMbBitReportBit = (oMfc.m_bMbBitReport[uProcessIdx]) ? (1<<0) : (0<<0);
	uSliceInfoReportBit = (oMfc.m_bSliceInfoReport[uProcessIdx]) ? (1<<1) : (0<<1);
	uAudEnableBit = (oMfc.m_bAudEnable[uProcessIdx]) ? (1<<2) : (0<<2);	
	uEncSeqOptionValue = uAudEnableBit | uSliceInfoReportBit | uMbBitReportBit;
	mfcOutp32(ENC_SEQ_OPTION, uEncSeqOptionValue);

    MFC_IssueCmd(uProcessIdx, SEQ_INIT);


	stat = MFC_IsCmdFinished();

	if(stat == false)
	{
		UART_Printf("\n There is an error in the SEQ_INIT result\n");
		return;
	}
}

void MFC_IssueSeqEndCmd(u32 uRunIdx)
{
    MFC_IssueCmd(uRunIdx, SEQ_END);
}

void MFC_MoveFirmwareToCodeBuf(u32 uFreeBufAddr)
{
    u32 i;
    u32 data;

	//StopBitProcessor();
	MFC_ResetIp();

	// Boot code(1KB) + Codec Firmware (79KB)
    for (i=0 ; i<sizeof(bit_code)/sizeof(bit_code[0]); i+=2)
	{
        data = (bit_code[i] << 16) | bit_code[i+1];	
        Outp32(uFreeBufAddr+(i-0)*2, data);    
    }

	DbgMfc("    CODE_BUF = 0x%x\n",uFreeBufAddr);
	mfcOutp32(CODE_BUF_ADDR, uFreeBufAddr);	    // point at boot code start addr
	oMfc.m_uCodeBufEndAddr = uFreeBufAddr + (sizeof(bit_code)/sizeof(bit_code[0])-0)*2; // 80KB(PRISM) for Code and Coeff ROM table

    // 512 word (Boot Code, 1KB)
    for (i=0; i<512; i++)
	{
        data = bit_code[i];
        Outp32(CODE_DN_LOAD, (i<<16) | data); // i: 13bit addr
    }

}

void MFC_SetParameterBufAddr(u32 uFreeBufAddr)
{
	DbgMfc("  Parameter buf = 0x%x\n",uFreeBufAddr);
	oMfc.m_uParameterBufStartAddr = uFreeBufAddr;
    mfcOutp32(PARA_BUF_ADDR, uFreeBufAddr);	
	oMfc.m_uParameterBufEndAddr = uFreeBufAddr + 8*1024; // 8KB for BIT processor parameter buffer	
}

void MFC_SetStreamBufControlOption(u8 isLittleEndian, u8 isFlowCheckEnable)
{
	u32 uRegData;	
	u32 uEndianMode = (isLittleEndian) ? STREAM_ENDIAN_LITTLE: STREAM_ENDIAN_BIG;
	u32 uStatusCheckMode = (isFlowCheckEnable) ? BUF_STATUS_FULL_EMPTY_CHECK_BIT: BUF_STATUS_NO_CHECK_BIT;	

	uRegData = mfcInp32(STREAM_BUF_CTRL);
	mfcOutp32(STREAM_BUF_CTRL, uRegData&~(0x03)|uStatusCheckMode|uEndianMode);
}

void MFC_SetFrameBufEndianMode(u8 isLittleEndian)
{
	u32 uEndianMode = (isLittleEndian) ? FRAME_MEM_ENDIAN_LITTLE : FRAME_MEM_ENDIAN_BIG;
	mfcOutp32(FRAME_BUF_CTRL, uEndianMode);
}

void MFC_SetUpWorkingBuf(u32 uFreeBufAddr)
{
	MFC_SetWorkBufConfig(false);
	
	DbgMfc("  Working buf = 0x%x\n",uFreeBufAddr);
    mfcOutp32(WORK_BUF_ADDR, uFreeBufAddr);
	
	oMfc.m_uWorkingBufEndAddr = uFreeBufAddr + 1024*1024;  // 1024KB for BIT processor working buffer	
}

// below size unit is KB.
void MFC_SetUpWorkingBuf2(
	u32 uFreeBufAddr, u32 uProcessBufSize, 
	u32 uDecBuf1Size, u32 uDecBuf2Size, u32 uDecBuf3Size, u32 uDecBuf4Size, u32 uDecBuf5Size, 
	u32 uEncBuf1Size, u32 uEncBuf2Size, u32 uEncBuf3Size, u32 uEncBuf4Size)
{
	u32 uBufAddr;

	MFC_SetWorkBufConfig(true);
	
	DbgMfc("  Working buf = 0x%x\n",uFreeBufAddr);
    mfcOutp32(WORK_BUF_ADDR, uFreeBufAddr);
	uBufAddr = uFreeBufAddr + 78*1024;  
	
	MFC_SetProcessBufStAddr(uBufAddr);
	uBufAddr += uProcessBufSize*1024;  
	MFC_SetDecTempBuf1StAddr(uBufAddr);
	uBufAddr += uDecBuf1Size*1024;  
	MFC_SetDecTempBuf2StAddr(uBufAddr);
	uBufAddr += uDecBuf2Size*1024;  
	MFC_SetDecTempBuf3StAddr(uBufAddr);
	uBufAddr += uDecBuf3Size*1024;  
	MFC_SetDecTempBuf4StAddr(uBufAddr);
	uBufAddr += uDecBuf4Size*1024;  
	MFC_SetDecTempBuf5StAddr(uBufAddr);
	uBufAddr += uDecBuf5Size*1024;  
	
	MFC_SetEncTempBuf1StAddr(uBufAddr);
	uBufAddr += uEncBuf1Size*1024;  
	MFC_SetEncTempBuf2StAddr(uBufAddr);
	uBufAddr += uEncBuf2Size*1024;  
	MFC_SetEncTempBuf3StAddr(uBufAddr);
	uBufAddr += uEncBuf3Size*1024;
	MFC_SetEncTempBuf4StAddr(uBufAddr);
	uBufAddr += uEncBuf4Size*1024;
	
	oMfc.m_uWorkingBufEndAddr = uBufAddr;
}

// below size unit is KB.
void MFC_SetUpWorkingBuf1(u32 uFreeBufAddr, MFC_APP_TYPE eAppType)
{
	u32 uBufAddr;
	u32 uBufSizeArray[][10] = {
		{128, 30, 15, 15, 4, 150, 0, 0, 0, 0}, // DMB decoding
		{128, 36, 48, 48, 14, 608, 0, 0, 0, 0}, // Conformance decoding // total = 882 KB
		{32, 36, 48, 48, 14, 16, 36, 48, 200, 256} // Conformance encoding // total =  734 KB
	};
	u32 uIdx = 
		(eAppType == DECODING_DMB) ? 0 : 
		(eAppType == DECODING_CONFORMANCE) ? 1 : 
		(eAppType == ENCODING_WITH_ALL_PARAMETERS) ? 2 : 0xff;

	MFC_SetWorkBufConfig(true);
	
	DbgMfc("  Working buf = 0x%x\n",uFreeBufAddr);
    mfcOutp32(WORK_BUF_ADDR, uFreeBufAddr);
	uBufAddr = uFreeBufAddr + 78*1024;		

	Assert(uIdx != 0xff);	
	
	MFC_SetProcessBufStAddr(uBufAddr);
	uBufAddr += uBufSizeArray[uIdx][0]*1024;  
	MFC_SetDecTempBuf1StAddr(uBufAddr);
	uBufAddr += uBufSizeArray[uIdx][1]*1024;  
	MFC_SetDecTempBuf2StAddr(uBufAddr);
	uBufAddr += uBufSizeArray[uIdx][2]*1024;  
	MFC_SetDecTempBuf3StAddr(uBufAddr);
	uBufAddr += uBufSizeArray[uIdx][3]*1024;  
	MFC_SetDecTempBuf4StAddr(uBufAddr);
	uBufAddr += uBufSizeArray[uIdx][4]*1024;  
	MFC_SetDecTempBuf5StAddr(uBufAddr);
	uBufAddr += uBufSizeArray[uIdx][5]*1024;  
	
	MFC_SetEncTempBuf1StAddr(uBufAddr);
	uBufAddr += uBufSizeArray[uIdx][6]*1024;  
	MFC_SetEncTempBuf2StAddr(uBufAddr);
	uBufAddr += uBufSizeArray[uIdx][7]*1024;  
	MFC_SetEncTempBuf3StAddr(uBufAddr);
	uBufAddr += uBufSizeArray[uIdx][8]*1024;
	MFC_SetEncTempBuf4StAddr(uBufAddr);
	uBufAddr += uBufSizeArray[uIdx][9]*1024;
	
	oMfc.m_uWorkingBufEndAddr = uBufAddr;
}

void MFC_InitBaseForProcesses(u32 uFreeBufAddr)
{
	u32 uBufAddr;
	MFC_SetUpWorkingBuf(uFreeBufAddr);
	MFC_GetWorkingBufEndAddr(&uBufAddr);
	
	MFC_SetParameterBufAddr(uBufAddr);

	MFC_SetStreamBufControlOption(true, true);   // Full/Empty check enable(true) + Little Endian(true)
    MFC_SetFrameBufEndianMode(true);  // Little endian (true)    
    MFC_SetUnmask(INT_MFC_ALL);  // all interrupt enable
    MFC_SetBitstreamEndFlag(false);
}

void MFC_InitBaseForProcesses2(
	u32 uFreeBufAddr, u32 uProcessBufSize, 
	u32 uDecBuf1Size, u32 uDecBuf2Size, u32 uDecBuf3Size, u32 uDecBuf4Size, u32 uDecBuf5Size, 
	u32 uEncBuf1Size, u32 uEncBuf2Size, u32 uEncBuf3Size, u32 uEncBuf4Size)
{
	u32 uBufAddr;

	MFC_SetUpWorkingBuf2(uFreeBufAddr, uProcessBufSize, 
		uDecBuf1Size, uDecBuf2Size, uDecBuf3Size, uDecBuf4Size, uDecBuf5Size, 
		uEncBuf1Size, uEncBuf2Size, uEncBuf3Size, uEncBuf4Size);
	
	MFC_GetWorkingBufEndAddr(&uBufAddr);
	
	MFC_SetParameterBufAddr(uBufAddr);
	
	MFC_SetStreamBufControlOption(true, true);   // Full/Empty check enable(true) + Little Endian(true)
    MFC_SetFrameBufEndianMode(true);  // Little endian (true)    
    MFC_SetUnmask(INT_MFC_ALL);  // all interrupt enable
    MFC_SetBitstreamEndFlag(false);
}

void MFC_InitBaseForProcesses1(u32 uFreeBufAddr, MFC_APP_TYPE eAppType)
{
	u32 uBufAddr;

	MFC_SetUpWorkingBuf1(uFreeBufAddr, eAppType);
	MFC_GetWorkingBufEndAddr(&uBufAddr);
	
	MFC_SetParameterBufAddr(uBufAddr);
	
	MFC_SetStreamBufControlOption(true, true);   // Full/Empty check enable(true) + Little Endian(true)
    MFC_SetFrameBufEndianMode(true);  // Little endian (true)    
    MFC_SetUnmask(INT_MFC_ALL);  // all interrupt enable
    MFC_SetBitstreamEndFlag(false);
}

void MFC_WaitForReady(void)
{
	u32 uRegData;

	do
	{
		uRegData = mfcInp32(BUSY_FLAG);
	} while(uRegData != 0); // 0: ready, 1: busy

}

void MFC_IssueCmd(u32 uProcessIdx, MFC_COMMAND runCmd)
{
	MFC_StartIssuingCmd(uProcessIdx, runCmd);
	MFC_WaitForReady();	
}

void MFC_StartIssuingCmd(u32 uProcessIdx, MFC_COMMAND eCmd)
{
	u32 runStd, uRunCmd;

	MFC_WaitForReady();	 

    mfcOutp32(RUN_INDEX,   uProcessIdx);

	runStd = 
		(oMfc.m_eCodecMode[uProcessIdx] == MP4_DEC) ? 0 :
		(oMfc.m_eCodecMode[uProcessIdx] == MP4_ENC) ? 1 :
		(oMfc.m_eCodecMode[uProcessIdx] == AVC_DEC) ? 2 :
		(oMfc.m_eCodecMode[uProcessIdx] == AVC_ENC) ? 3 :
		(oMfc.m_eCodecMode[uProcessIdx] == VC1_DEC) ? 4 : 0;
	mfcOutp32(RUN_COD_STD, runStd);

	uRunCmd =
		(eCmd == SEQ_INIT) ? 1 :
		(eCmd == SEQ_END) ? 2 :
		(eCmd == PIC_RUN) ? 3 : 
		(eCmd == SET_FRAME_BUF) ? 4 :
		(eCmd == ENCODE_HEADER) ? 5 :
		(eCmd == ENC_PARA_SET) ? 6 :
		(eCmd == DEC_PARA_SET) ? 7 :
		(eCmd == GET_FW_VER) ? 0xf : 0xff;	
	
	//////////
	//s/w work-around(2007.03.21)
	//	: write the busy flag compulsorily 
	//		in order to remove the decoding error when mfcclk is less than pclk
	mfcOutp32(BUSY_FLAG,0x1);
	//////////
    
    mfcOutp32(RUN_CMD, uRunCmd);
}

void MFC_StartBitProcessor(void)
{
	DbgMfc("[ Execution Start... ]\n");
	mfcOutp32(CODE_RUN, 0x1);
}

void MFC_StopBitProcessor(void)
{
	mfcOutp32(CODE_RUN, 0x0);
}

void MFC_ResetProgramCounterOfBitCode(void)
{
	mfcOutp32(BITS_CODE_RESET, 1);
}

void MFC_ResetIp(void)
{
	mfcOutp32(SW_RESET, 0);
	mfcOutp32(SW_RESET, 1);
}

void MFC_GetDecSrcFormat(u32* uWidth, u32* uHeight, float* uFrameRate)
{
	u32 data;	
	u32 uFrameRateDiv;
	u32 uFrameRateRes;

    data = mfcInp32(RET_DEC_SEQ_SRC_SIZE);
    *uWidth = (data >> 10) & 0x3FF;
    *uHeight = (data >>  0) & 0x3FF;

	data = mfcInp32(RET_DEC_SEQ_SRC_FRAME_RATE);
	uFrameRateDiv = (data >> 16) & 0xffff;
	uFrameRateRes = data & 0xffff;
	//DbgMfc("uFrameRateDiv+1=%d, uFrameRateRes=%d\n",uFrameRateDiv+1,uFrameRateRes);
    *uFrameRate = (float)uFrameRateRes / (float)(uFrameRateDiv+1);
}

void MFC_GetEncSrcSize(u32* uWidth, u32* uHeight)
{
	u32 data;	

	data = mfcInp32(ENC_SEQ_SRC_SIZE);
	*uWidth = (data >> 10) & 0x3FF;
	*uHeight = (data >>  0) & 0x3FF;
}

void MFC_GetMp4Info(u8* isDataPartEn, u8* isRevVlcEn, u8* isShortVideoHeader)
{
	u32 data;
	data = mfcInp32(RET_DEC_SEQ_INFO);

	//DbgMfc("getMp4Info()=%d\n",data);
	data = data & 0x7;

	if (data & 4)
	{
		*isShortVideoHeader = true;
		*isDataPartEn = false;
		*isRevVlcEn = false;
	}
	else 
	{
		if (data & 1)
		{
			*isDataPartEn = true;
			*isShortVideoHeader = false;
			if (data & 2)
				*isRevVlcEn = true;
			else
				*isRevVlcEn = false;
		}
		else
		{
			*isDataPartEn = false;
			*isShortVideoHeader = false;
			*isRevVlcEn = false;
		}
	}
}

void MFC_IsDecH263AnnexJOn(u32 runIdx, u8* bAnnexJOn)
{
	if (oMfc.m_eCodecMode[runIdx] == MP4_DEC)
	{
		u32 uRegData;
		uRegData = mfcInp32(RET_DEC_SEQ_INFO);
		*bAnnexJOn = (uRegData&(1<<3)) ? true : false; 
	}
	else
	{
		*bAnnexJOn = false;
	}
}

void MFC_GetDecRefFrameNum(u32 uProcessIdx, u32* uRefFrameNum)
{
	u32 data;

	data = mfcInp32(RET_DEC_SEQ_FRAME_NEED_COUNT);
	*uRefFrameNum = data & 0x1F;
	oMfc.m_uRefFrameNum[uProcessIdx] = *uRefFrameNum;
}

void MFC_GetDispFrameNum(u32 uProcessIdx, u32* uDispFrameNum)
{
	MFC_CODEC_MODE eMode;
	u32 uNumOfFrames;

	Assert(uProcessIdx < MAX_PROCESS_NUM);
	eMode = oMfc.m_eCodecMode[uProcessIdx];
	if (eMode == MP4_DEC || eMode == AVC_DEC || eMode == VC1_DEC)
	{
		uNumOfFrames = (oMfc.m_bMp4DecDeblkMode[uProcessIdx] && !oMfc.m_bAnnexJOn[uProcessIdx]) ? 
			oMfc.m_uRefFrameNum[uProcessIdx]+2 : oMfc.m_uRefFrameNum[uProcessIdx];
		*uDispFrameNum = (oMfc.m_bDecRotEn[uProcessIdx]) ? uNumOfFrames+2 : uNumOfFrames;
	}
	else if (eMode == MP4_ENC || eMode == AVC_ENC)
	{
		*uDispFrameNum = oMfc.m_uRefFrameNum[uProcessIdx];
	}
	else
		Assert(0);
}

void MFC_InitDecFrameBuffer(
	u32 uProcessIdx, 
	u32 uFrameBufNum, 
	u32 uStride, 
	u32 uHeight, 
	u32 uFrameBuf)
{
	u32 k;
	u32 uBufAddr = uFrameBuf;
	u32 i;
	//DbgMfc("addrY=0x%x\n",addrY);

	if(uStride%16!=0)
		UART_Printf("ERROR=>uStride is not multiple of 16\n");
	Assert(uStride%16==0);
	if(uStride>720)
		UART_Printf("ERROR=>uStride=%d(>720)\n",uStride);
	Assert(uStride <= 720);
	if(uHeight>576)
		UART_Printf("ERROR=>uHeight=%d(>576)\n",uHeight);
	Assert(uHeight <= 576);

	oMfc.m_uStride[uProcessIdx] = uStride;
	oMfc.m_uHeight[uProcessIdx] = uHeight;
	
	for (i=0; i<uFrameBufNum; i++)
	{
		oMfc.m_uFrameAddrY[uProcessIdx][i] = uBufAddr;		
		k = uStride * uHeight;
		uBufAddr += k;

		oMfc.m_uFrameAddrCb[uProcessIdx][i] = oMfc.m_uFrameAddrY[uProcessIdx][i] + k;		
		k = (uStride * uHeight) / 4;
		uBufAddr += k;
		
		oMfc.m_uFrameAddrCr[uProcessIdx][i] = oMfc.m_uFrameAddrCb[uProcessIdx][i] + k;		
		uBufAddr += k;
	}
	oMfc.m_uLastFrameBufEndAddr[uProcessIdx] = uBufAddr;
}

void MFC_InitEncFrameBuffer(u32 uProcessIdx, u32 uRefFrameNum, u32 uWidth, u32 uHeight, u32 uRefFrameBufStAddr)
{
	u32 uStride = (uWidth%16 == 0) ? uWidth : (uWidth+15)/16*16;
	u32 uNewHeight = (uHeight%16 == 0) ? uHeight : (uHeight+15)/16*16;	
	u32 k, uBufAddr, i;

	oMfc.m_uStride[uProcessIdx] = uStride;
	oMfc.m_uHeight[uProcessIdx] = uNewHeight;

	uBufAddr = uRefFrameBufStAddr;
	
	// Reference + Reconstruct frame buffer setting
	for (i=0; i<uRefFrameNum; i++)
	{
		oMfc.m_uFrameAddrY[uProcessIdx][i] = uBufAddr;
		k = uStride * uNewHeight;
		uBufAddr += k;

		oMfc.m_uFrameAddrCb[uProcessIdx][i] = oMfc.m_uFrameAddrY[uProcessIdx][i] + k;
		k = (uStride * uNewHeight) / 4;
		uBufAddr += k;
		
		oMfc.m_uFrameAddrCr[uProcessIdx][i] = oMfc.m_uFrameAddrCb[uProcessIdx][i] + k;
		uBufAddr += k;
	}

	oMfc.m_uLastFrameBufEndAddr[uProcessIdx] = uBufAddr;
}


void MFC_ClearFrameBuffer(u32 uProcessIdx, u32 uFrameIdx)
{
	u32 uStride = oMfc.m_uStride[uProcessIdx];
	u32 uHeight = oMfc.m_uHeight[uProcessIdx];
	u32 i;

	Assert(uFrameIdx<MAX_DEC_FRAME);	

	for (i=0; i<uStride*uHeight/4; i++)
		Outp32(oMfc.m_uFrameAddrY[uProcessIdx][uFrameIdx]+i*4, 0);
	for (i=0; i<uStride*uHeight/16; i++)
		Outp32(oMfc.m_uFrameAddrCb[uProcessIdx][uFrameIdx]+i*4, 0x80808080);
	for (i=0; i<uStride*uHeight/16; i++)
		Outp32(oMfc.m_uFrameAddrCr[uProcessIdx][uFrameIdx]+i*4, 0x80808080);

}

void MFC_GetFrameCount(u32* uFrameNum)
{
	*uFrameNum = mfcInp32(RET_PIC_FRAME_NUM);
}


void MFC_GetCurPc(u32* uCurProgCnt)
{
	*uCurProgCnt = mfcInp32(BITS_CUR_PC);
}

void MFC_GetAddrOfYFrame(u32 uProcessIdx, u32 idx, u32* addrY)
{
	Assert(idx<MAX_DEC_FRAME);
	*addrY = oMfc.m_uFrameAddrY[uProcessIdx][idx];
}

void MFC_SetFrameAddr(u32 uProcessIdx, u32 uFrameIdx, u32 uAddrY, u32 uAddrCb, u32 uAddrCr)
{
	oMfc.m_uFrameAddrY[uProcessIdx][uFrameIdx] = uAddrY;
	oMfc.m_uFrameAddrCb[uProcessIdx][uFrameIdx] = uAddrCb;
	oMfc.m_uFrameAddrCr[uProcessIdx][uFrameIdx] = uAddrCr;	
}

void MFC_GetDecReconFrameIndex(u32 uProcessIdx, s32* uDecFrmIdx)
{
	*uDecFrmIdx = mfcInp32(RET_DEC_PIC_INDEX);
}

u8 MFC_IsCmdFinished(void)
{
	u32 uRegData;
	uRegData = mfcInp32(RET_SEQ_SUCCESS);
	return (uRegData == 0) ? false : true;
}

void MFC_GetWrPtr(u32 uRunIdx, u32* uWrPtr)
{
	Assert(uRunIdx<MAX_PROCESS_NUM);
	*uWrPtr = mfcInp32(oMfc.m_uBitWrPtr[uRunIdx]);
}

void MFC_GetRdPtr(u32 uRunIdx, u32* uRdPtr)
{
	Assert(uRunIdx<MAX_PROCESS_NUM);
	*uRdPtr = mfcInp32(oMfc.m_uBitRdPtr[uRunIdx]);
}

void MFC_SetRdPtr(u32 uProcessIdx, u32 uRdPtr)
{
	Assert(uProcessIdx<MAX_PROCESS_NUM);
	mfcOutp32(oMfc.m_uBitRdPtr[uProcessIdx], uRdPtr);
}

void MFC_SetWrPtr(u32 uProcessIdx, u32 uWrPtr)
{
	Assert(uProcessIdx<MAX_PROCESS_NUM);
	mfcOutp32(oMfc.m_uBitWrPtr[uProcessIdx], uWrPtr);
}

void MFC_GetWrPtr2(u32 uRunIdx, u32* uWrPtr)
{
	Assert(uRunIdx<MAX_PROCESS_NUM);
	*uWrPtr = mfcInp32(BITS_WR_PTR0);

}

void MFC_GetRdPtr2(u32 uRunIdx, u32* uRdPtr)
{
	Assert(uRunIdx<MAX_PROCESS_NUM);
	 *uRdPtr = mfcInp32(BITS_RD_PTR0);
}

void MFC_SetWrPtr2(u32 uProcessIdx, u32 uWrPtr)
{
	Assert(uProcessIdx<MAX_PROCESS_NUM);
	mfcOutp32(BITS_WR_PTR0, uWrPtr);
}

void MFC_SetRdPtr2(u32 uProcessIdx, u32 uRdPtr)
{
	Assert(uProcessIdx<MAX_PROCESS_NUM);
	mfcOutp32(BITS_RD_PTR0, uRdPtr);
}

void MFC_GetCodecModeByName(const char* fileExt, u8 bIsDec, MFC_CODEC_MODE* eCodecMode)
{
	int nMode;
	
	if (fileExt[0] == 'M' && fileExt[1] == '4' && fileExt[2] == 'V')
		nMode = 0;
	else if (fileExt[0] == 'm' && fileExt[1] == '4' && fileExt[2] == 'v')
		nMode = 0;
	else if (fileExt[0] == 'm' && fileExt[1] == 'p' && fileExt[2] == '4')
		nMode = 0;
	else if (fileExt[0] == 'M' && fileExt[1] == 'P' && fileExt[2] == '4')
		nMode = 0;
	else if (fileExt[0] == 'c' && fileExt[1] == 'm' && fileExt[2] == 'p')
		nMode = 0;
	else if (fileExt[0] == 'C' && fileExt[1] == 'M' && fileExt[2] == 'P')
		nMode = 0;		
	else if (fileExt[0] == '2' && fileExt[1] == '6' && fileExt[2] == '3')
		nMode = 0;
	else if (fileExt[1] == 0)
		nMode = 0;
	else if (fileExt[0] == 'j' && fileExt[1] == 's' && fileExt[2] == 'v')
		nMode = 1;
	else if (fileExt[0] == 'J' && fileExt[1] == 'S' && fileExt[2] == 'V')
		nMode = 1;
	else if (fileExt[0] == '2' && fileExt[1] == '6' && fileExt[2] == '4')
		nMode = 1;
	else if (fileExt[0] == '2' && fileExt[1] == '6' && fileExt[2] == 'l')
		nMode = 1;
	else if (fileExt[0] == 'h' && fileExt[1] == '2' && fileExt[2] == '6' && fileExt[3] == '4')
		nMode = 1;		
	else if (fileExt[0] == 'r' && fileExt[1] == 'c' && fileExt[2] == 'v')
		nMode = 2;
	else if (fileExt[0] == 'R' && fileExt[1] == 'C' && fileExt[2] == 'V')
		nMode = 2;			
	else
		nMode = 0;
	
	Assert(nMode >= 0 && nMode < 3);
	if (bIsDec) // for decoding
		*eCodecMode = (nMode == 0) ? MP4_DEC : (nMode == 1) ? AVC_DEC : VC1_DEC;
	else // for encoding
		*eCodecMode = (nMode == 0) ? MP4_ENC : (nMode == 1) ? AVC_ENC : (MFC_CODEC_MODE)0xff;
}

void MFC_GetSrcPicSize(u32 uIdx, u32* uWidth, u32* uHeight)
{
	*uWidth = oMfc.m_uPicX[uIdx];
	*uHeight = oMfc.m_uPicY[uIdx];
}

void MFC_GetFrameSize(u32 uIdx, u32* uStride, u32* uHeight)
{
	*uStride = oMfc.m_uStride[uIdx];
	*uHeight = oMfc.m_uHeight[uIdx];
}

// 0 : IPPP... (only first picture is I)
// 1 : IIII... (no P picture)
// 2 : IPIP...
// 3 : IPPIPPIPP...
void MFC_SetEncGopNum(u32 uGopPicNum)
{
	Assert(uGopPicNum <= 60);
	mfcOutp32(ENC_SEQ_GOP_NUM,  uGopPicNum);
}

void MFC_SetEncCodStd(u32 uCodStd)
{
	Assert(uCodStd <= 2);
	mfcOutp32(ENC_SEQ_COD_STD, uCodStd);
}

void MFC_SetEncSrcFormat(u32 uWidth, u32 uHeight, u32 uFrameRate)
{
	Assert(uWidth<1024);
	Assert(uHeight<1024);
	Assert(uFrameRate<64);
	mfcOutp32(ENC_SEQ_SRC_SIZE, (uWidth << 10) | uHeight);
	mfcOutp32(ENC_SEQ_SRC_FRAME_RATE, uFrameRate);
}

void MFC_SetEncPicQs(MFC_CODEC_MODE eCodecMode, u32 uPicQuantStep)
{
	Assert(((eCodecMode == MP4_ENC) && (uPicQuantStep >= 1) && (uPicQuantStep <= 31))
		|| ((eCodecMode == AVC_ENC) && (uPicQuantStep <= 51)) );
	mfcOutp32(ENC_PIC_QS, uPicQuantStep);
}

void MFC_SetEnc264Parameter(
	int uChromaQpOffset, 
	u32 uConstIntraFlag, 
	u32 uDisableDeblk,
	int uDeblkAlphaOffset,
	int uDeblkBetaOffset)
{
	u32 uChroma;
	u32 uDeblkA;
	u32 uDeblkB;

	Assert((uChromaQpOffset >= -12) && (uChromaQpOffset <= 12));
	Assert(uConstIntraFlag < 2);
	Assert(uDisableDeblk < 3);
	Assert((uDeblkAlphaOffset >= -6) && (uDeblkAlphaOffset <= 6));
	Assert((uDeblkBetaOffset >= -6) && (uDeblkBetaOffset <= 6));

	uChroma = (uChromaQpOffset < 0) ? uChromaQpOffset+32 : uChromaQpOffset;
	uDeblkA = (uDeblkAlphaOffset < 0) ? uDeblkAlphaOffset+16 : uDeblkAlphaOffset;
	uDeblkB = (uDeblkBetaOffset < 0) ? uDeblkBetaOffset+16 : uDeblkBetaOffset;
	
	mfcOutp32(ENC_SEQ_264_PARA, 
		(uDeblkB << 12) | 
		(uDeblkA << 8) | 
		(uDisableDeblk << 6) | 
		(uConstIntraFlag << 5) | 
		uChroma);
}

void MFC_SetEncYuvFileCfg(
	u32 uProcessIdx,
	const char* pFileName,
	u32 uNumOfFrame,
	u32 uWidth,
	u32 uHeight,
	u32 uFrameRate)
{
	Assert(uWidth < 1024);
	Assert(uHeight < 1024);
	Assert(uFrameRate < 64);
	
	strncpy(oMfc.m_EncCfg[uProcessIdx].SrcFileName, pFileName, 255);
	oMfc.m_EncCfg[uProcessIdx].NumOfFrame = uNumOfFrame;
	oMfc.m_EncCfg[uProcessIdx].PicX = uWidth;
	oMfc.m_EncCfg[uProcessIdx].PicY = uHeight;
	oMfc.m_EncCfg[uProcessIdx].FrameRate = uFrameRate;
}

void MFC_SetEncGoldenFileCfg(u32 uProcessIdx, const char* pGoldenYuvName, const char* pGoldenStreamName)
{
	strncpy(oMfc.m_EncCfg[uProcessIdx].GoldenYuvFileName, pGoldenYuvName, 255);
	strncpy(oMfc.m_EncCfg[uProcessIdx].GoldenStreamFileName, pGoldenStreamName, 255);
	DbgMfc("gs=%s\n",oMfc.m_EncCfg[uProcessIdx].GoldenStreamFileName);
	DbgMfc("gy=%s\n",oMfc.m_EncCfg[uProcessIdx].GoldenYuvFileName);
}

void MFC_SetEncSliceCfg(
	u32 uProcessIdx,
	u32 uSliceMode,
	u32 uSliceSizeMode,
	u32 uSliceSizeNum)
{
	Assert(uSliceMode < 2);
	Assert(uSliceSizeMode < 2);
	Assert(uSliceSizeNum < 65536);
	
	// Slice structure
	oMfc.m_EncCfg[uProcessIdx].SliceMode = uSliceMode; // (0 : one slice, 1 : multiple slice)
	oMfc.m_EncCfg[uProcessIdx].SliceSizeMode = uSliceSizeMode; // (0 : slice bit number, 1 : slice mb number)
	oMfc.m_EncCfg[uProcessIdx].SliceSizeNum = uSliceSizeNum;    // (bit count or mb number)
}

void MFC_SetEncErrResilienceCfg(u32 uProcessIdx, u32 uIntraRefreshNum)
{
	Assert(uIntraRefreshNum < 65536);
	// error resilience
	oMfc.m_EncCfg[uProcessIdx].IntraRefreshNum = uIntraRefreshNum; // Intra MB Refresh (0 - None, 1 ~ MbNum-1)
}

void MFC_SetEncMpeg4Cfg(
	u32 uProcessIdx,
	u32 uDataPartEn,
	u32 uRevVlcEn,
	u32 uIntraDcVlcThr,
	u32 uVopQuant,
	u32 uGopPicNum)
{
	Assert(uDataPartEn < 2);
	Assert(uRevVlcEn < 2);
	Assert(uIntraDcVlcThr < 8);
	Assert(uVopQuant < 32);
	Assert(uGopPicNum < 61);
	
	oMfc.m_EncCfg[uProcessIdx].VerId = 2;
	oMfc.m_EncCfg[uProcessIdx].DataPartEn = uDataPartEn;
	oMfc.m_EncCfg[uProcessIdx].RevVlcEn = uRevVlcEn;
	oMfc.m_EncCfg[uProcessIdx].IntraDcVlcThr = uIntraDcVlcThr;
	oMfc.m_EncCfg[uProcessIdx].ShortVideoHeader = 0;  // if 0, MPEG4

	oMfc.m_EncCfg[uProcessIdx].VopQuant = uVopQuant;
	oMfc.m_EncCfg[uProcessIdx].GopPicNum = uGopPicNum;    // (0 : IPPPPPP..., 1 : all I, 3 : I,P,P,I,)
}

void MFC_SetEnc263Cfg(
	u32 uProcessIdx,
	u32 uAnnexI,
	u32 uAnnexJ,
	u32 uAnnexK,
	u32 uAnnexT,
	u32 uVopQuant,
	u32 uGopPicNum)
{
	Assert(uAnnexI == 0); // advance intra coding
	Assert(uAnnexJ < 2); // deblocking filter
	Assert(uAnnexK < 2); // slice structure
	Assert(uAnnexT < 2); // modified quantization
	Assert(uVopQuant < 52);
	Assert(uGopPicNum < 61);
	
	oMfc.m_EncCfg[uProcessIdx].AnnexI = uAnnexI;
	oMfc.m_EncCfg[uProcessIdx].AnnexJ = uAnnexJ;
	oMfc.m_EncCfg[uProcessIdx].AnnexK = uAnnexK;
	oMfc.m_EncCfg[uProcessIdx].AnnexT = uAnnexT;
	oMfc.m_EncCfg[uProcessIdx].ShortVideoHeader = 1;  // if 1, H.263. if 0, MPEG4 

	oMfc.m_EncCfg[uProcessIdx].VopQuant = uVopQuant;
	oMfc.m_EncCfg[uProcessIdx].GopPicNum = uGopPicNum;   // (0 : forbidden, 1 : all I, 3 : I,P,P,I,)
}

void MFC_SetEnc264Cfg(
	u32 uProcessIdx,
	u32 uConstIntraPredFlag,
	u32 uDisableDeblk,
	int uDeblkOffsetA,
	int uDeblkOffsetB,
	int uChromaQpOffset,
	u32 uPicQpY,
	u32 uGopPicNum)
{
	Assert(uConstIntraPredFlag < 2);
	Assert(uDisableDeblk < 3);
	Assert(uDeblkOffsetA   >= -6 && uDeblkOffsetA <= 6);
	Assert(uDeblkOffsetB   >= -6 && uDeblkOffsetB <= 6);
	Assert(uChromaQpOffset   >= -12 && uChromaQpOffset <= 12);
	Assert(uPicQpY < 52);
	Assert(uGopPicNum < 61);
	
	oMfc.m_EncCfg[uProcessIdx].ConstIntraPredFlag = uConstIntraPredFlag;

	// deblocking filter
	oMfc.m_EncCfg[uProcessIdx].DisableDeblk = uDisableDeblk;
	oMfc.m_EncCfg[uProcessIdx].DeblkOffsetA = uDeblkOffsetA;
	oMfc.m_EncCfg[uProcessIdx].DeblkOffsetB = uDeblkOffsetB;
	oMfc.m_EncCfg[uProcessIdx].ChromaQpOffset = uChromaQpOffset;
	oMfc.m_EncCfg[uProcessIdx].PicQpY = uPicQpY;
	oMfc.m_EncCfg[uProcessIdx].GopPicNum = uGopPicNum;
}

void MFC_SetEnc264FmoCfg(u32 uProcessIdx, u8 bFmoEnable, u32 uFmoType, u32 uNumOfSliceGroup)
{	
	if (bFmoEnable)
	{
		Assert(uFmoType < 2);
		Assert((uNumOfSliceGroup >= 2) && (uNumOfSliceGroup <= 8));	
	}
	else
	{
		Assert(uNumOfSliceGroup == 1);
	}
	
	oMfc.m_EncCfg[uProcessIdx].FmoEnable = (bFmoEnable) ? 1 : 0;
	oMfc.m_EncCfg[uProcessIdx].NumOfSliceGroup = uNumOfSliceGroup;
	oMfc.m_EncCfg[uProcessIdx].FmoType = uFmoType;
}

void MFC_SetEncRateCtrlCfg(u32 uProcessIdx, u32 uRcEnable, u32 uRcBitRate, u32 uRcInitDelay, u32 uRcBufSize)
{
	Assert(uRcEnable < 2);
	Assert(uRcBitRate < 32768);
	Assert(uRcInitDelay < 65536);
	// rate Control
	oMfc.m_EncCfg[uProcessIdx].RcEnable = uRcEnable;
	oMfc.m_EncCfg[uProcessIdx].RcBitRate = uRcBitRate;
	oMfc.m_EncCfg[uProcessIdx].RcInitDelay = uRcInitDelay;
	oMfc.m_EncCfg[uProcessIdx].RcBufSize = uRcBufSize;		
}


void MFC_GetStreamBufEndAddr(u32 uProcessIdx, u32* uBitBufEndAddr)
{
	*uBitBufEndAddr = oMfc.m_uStreamBufEndAddr[uProcessIdx];
}

void MFC_SetDecRotEn(u8 isRotEn)
{
	u32 uDecRotEnBit = (isRotEn) ? (1<<4) : (0<<4); 
	u32 uRegData;
	uRegData =mfcInp32(DEC_PIC_ROT_MODE);
    mfcOutp32(DEC_PIC_ROT_MODE, uRegData&(0x0f)|uDecRotEnBit);
}

void MFC_SetDecRotationMode(u8 horMirEn, u8 verMirEn, u32 rotDeg)
{	
	u32 uHorMirMode = (horMirEn == true) ? (1<<3) : (0<<3);
	u32 uVerMirMode = (verMirEn == true) ? (1<<2) : (0<<2);
	u32 uRotAng = 
		(rotDeg == 0) ? (0<<0) :
		(rotDeg == 90) ? (1<<0) :
		(rotDeg == 180) ? (2<<0) :
		(rotDeg == 270) ? (3<<0) : (0<<0);
		
	u32 uRegData;

	Assert(rotDeg == 0 || rotDeg == 90 || rotDeg == 180 || rotDeg == 270);

	uRegData = mfcInp32(DEC_PIC_ROT_MODE);
    mfcOutp32(DEC_PIC_ROT_MODE, uRegData&(1<<4)|uHorMirMode|uVerMirMode|uRotAng);
}

void MFC_SetDecRotationMode1(u8 decRotEn, u8 horMirEn, u8 verMirEn, u32 rotDeg)
{	
	u32 uHorMirMode = (horMirEn == true) ? (1<<3) : (0<<3);
	u32 uVerMirMode = (verMirEn == true) ? (1<<2) : (0<<2);
	u32 uRotAng = 
		(rotDeg == 0) ? (0<<0) :
		(rotDeg == 90) ? (1<<0) :
		(rotDeg == 180) ? (2<<0) :
		(rotDeg == 270) ? (3<<0) : (0<<0);
    u32 decRotEnBit = (decRotEn == true) ? (1<<4) : (0<<4);

	Assert(rotDeg == 0 || rotDeg == 90 || rotDeg == 180 || rotDeg == 270);

    mfcOutp32(DEC_PIC_ROT_MODE, decRotEnBit|uHorMirMode|uVerMirMode|uRotAng);
}

void MFC_SetEncRotEn(u8 isRotEn)
{
	u32 uEncRotEnBit = (isRotEn) ? (1<<4) : (0<<4); 
	u32 uRegData;
	uRegData = mfcInp32(ENC_PIC_ROT_MODE);
    mfcOutp32(ENC_PIC_ROT_MODE, uRegData&(0x0f)|uEncRotEnBit);
}

void MFC_SetEncRotationMode(u8 horMirEn, u8 verMirEn, u32 rotDeg)
{	
	u32 uHorMirMode = (horMirEn == true) ? (1<<3) : (0<<3);
	u32 uVerMirMode = (verMirEn == true) ? (1<<2) : (0<<2);
	u32 uRotAng = 
		(rotDeg == 0) ? (0<<0) :
		(rotDeg == 90) ? (1<<0) :
		(rotDeg == 180) ? (2<<0) :
		(rotDeg == 270) ? (3<<0) : (0<<0);
		
	u32 uRegData;

	Assert(rotDeg == 0 || rotDeg == 90 || rotDeg == 180 || rotDeg == 270);

	uRegData = mfcInp32(ENC_PIC_ROT_MODE);
    mfcOutp32(ENC_PIC_ROT_MODE, uRegData&(1<<4)|uHorMirMode|uVerMirMode|uRotAng);
}

void MFC_SetEncRotationMode1(u8 encRotEn, u8 horMirEn, u8 verMirEn, u32 rotDeg)
{
	u32 uHorMirMode = (horMirEn == true) ? (1<<3) : (0<<3);
	u32 uVerMirMode = (verMirEn == true) ? (1<<2) : (0<<2);
	u32 uRotAng = 
		(rotDeg == 0) ? (0<<0) :
		(rotDeg == 90) ? (1<<0) :
		(rotDeg == 180) ? (2<<0) :
		(rotDeg == 270) ? (3<<0) : (0<<0);
    u32 encRotEnBit = (encRotEn == true) ? (1<<4) : (0<<4);

	Assert(rotDeg == 0 || rotDeg == 90 || rotDeg == 180 || rotDeg == 270);

    mfcOutp32(ENC_PIC_ROT_MODE, encRotEnBit|uHorMirMode|uVerMirMode|uRotAng);
}

void MFC_GetEncReconFrameIndex(s32* uEncPicFrmIdx)
{
	*uEncPicFrmIdx = mfcInp32(RET_ENC_PIC_INDEX);
}

void MFC_ClearPending(MFC_INT_FLAG eIntFlag)
{
	mfcOutp32(INT_REASON, 0);
	mfcOutp32(BITS_INT_CLEAR, 1);
}

void MFC_SetUnmask(MFC_INT_FLAG eIntFlag)
{
	u32 uIntDisenBit =
		(eIntFlag == INT_MFC_DNLOAD_DONE) ? INT_DNLOAD_DONE_BIT :
		(eIntFlag == INT_MFC_SEQ_INIT) ? INT_SEQ_INIT_BIT :
		(eIntFlag == INT_MFC_SEQ_END) ? INT_SEQ_END_BIT :
		(eIntFlag == INT_MFC_PIC_RUN) ? INT_PIC_RUN_BIT :
		(eIntFlag == INT_MFC_SET_FRAME_BUF) ? INT_SET_FRAME_BUF :
		(eIntFlag == INT_MFC_ENC_HEADER) ? INT_ENC_HEADER :
		(eIntFlag == INT_MFC_ENC_PARA_SET) ? INT_ENC_PARA_SET :
		(eIntFlag == INT_MFC_DEC_PARA_SET) ? INT_DEC_PARA_SET :	
		(eIntFlag == INT_MFC_BIT_BUF_EMPTY) ? INT_BIT_BUF_EMPTY_BIT :
		(eIntFlag == INT_MFC_BIT_BUF_FULL) ? INT_BIT_BUF_FULL_BIT : 
		(eIntFlag == INT_MFC_ALL) ? INT_ALL_BIT : (0<<0);
	u32 uRegData;
	uRegData = mfcInp32(INT_ENABLE);
	mfcOutp32(INT_ENABLE, uRegData|uIntDisenBit);
}

void MFC_SetMask(MFC_INT_FLAG eIntFlag)
{
	u32 uIntEnBit =
		(eIntFlag == INT_MFC_DNLOAD_DONE) ? INT_DNLOAD_DONE_BIT :
		(eIntFlag == INT_MFC_SEQ_INIT) ? INT_SEQ_INIT_BIT :
		(eIntFlag == INT_MFC_SEQ_END) ? INT_SEQ_END_BIT :
		(eIntFlag == INT_MFC_PIC_RUN) ? INT_PIC_RUN_BIT :
		(eIntFlag == INT_MFC_SET_FRAME_BUF) ? INT_SET_FRAME_BUF :
		(eIntFlag == INT_MFC_ENC_HEADER) ? INT_ENC_HEADER :
		(eIntFlag == INT_MFC_ENC_PARA_SET) ? INT_ENC_PARA_SET :
		(eIntFlag == INT_MFC_DEC_PARA_SET) ? INT_DEC_PARA_SET :				
		(eIntFlag == INT_MFC_BIT_BUF_EMPTY) ? INT_BIT_BUF_EMPTY_BIT :
		(eIntFlag == INT_MFC_BIT_BUF_FULL) ? INT_BIT_BUF_FULL_BIT : 
		(eIntFlag == INT_MFC_ALL) ? INT_ALL_BIT : (0<<0);
	u32 uRegData;
	uRegData = mfcInp32(INT_ENABLE);
	mfcOutp32(INT_ENABLE, uRegData&~uIntEnBit);
}

void MFC_GetIntFlag(MFC_INT_FLAG* eIntFlag)
{
	u32 uIdx;
	u32 uRegData;
	uRegData = mfcInp32(INT_REASON);

	for (uIdx=0; uIdx<16; uIdx++)
	{
		if (uIdx > 7 && uIdx <14)
			continue;
		if (uRegData & (1<<uIdx))		
			break;
	}

	*eIntFlag = 
		(uIdx == 0) ? INT_MFC_DNLOAD_DONE :
		(uIdx == 1) ? INT_MFC_SEQ_INIT :
		(uIdx == 2) ? INT_MFC_SEQ_END :
		(uIdx == 3) ? INT_MFC_PIC_RUN :
		(uIdx == 4) ? INT_MFC_SET_FRAME_BUF :
		(uIdx == 5) ? INT_MFC_ENC_HEADER :
		(uIdx == 6) ? INT_MFC_ENC_PARA_SET :
		(uIdx == 7) ? INT_MFC_DEC_PARA_SET :	
		(uIdx == 14) ? INT_MFC_BIT_BUF_EMPTY : 
		(uIdx == 15) ? INT_MFC_BIT_BUF_FULL : (MFC_INT_FLAG)0xffff;
	
}

void MFC_ResetStreamBufInEncoding(void)
{
	u32 uRegData;

	uRegData = mfcInp32(STREAM_BUF_CTRL);
	mfcOutp32(STREAM_BUF_CTRL, uRegData&~(0x3<<2)|RESET_BUF_AT_EVERY_ENCODING);
}

void MFC_SetFlushingStreamBufModeInEncoding(void)
{
	u32 uRegData;

	uRegData = mfcInp32(STREAM_BUF_CTRL);
	mfcOutp32(STREAM_BUF_CTRL, uRegData&~(0x3<<2)|FLUSH_BUF_IN_ENCODING);
}

void MFC_SetNoFlushingStreamBufModeInEncoding(void)
{
	u32 uRegData;

	uRegData = mfcInp32(STREAM_BUF_CTRL);
	mfcOutp32(STREAM_BUF_CTRL, uRegData&~(0x3<<2)|NOT_FLUSH_BUF_IN_ENCODING);
}

u8 MFC_IsMoreStreamNeeded(u32 uProcessIdx, u32 uSzOfCriterion)
{
	u32 uRdPtr;
	u32 uWrPtr;
	
	MFC_GetRdPtr(uProcessIdx, &uRdPtr);
	MFC_GetWrPtr(uProcessIdx, &uWrPtr);	
	
	if ( ((uWrPtr > uRdPtr) && (uWrPtr - uRdPtr < uSzOfCriterion)) ||
		 ((uWrPtr <= uRdPtr) && (uWrPtr + uSzOfCriterion < uRdPtr)) )
	{				
		return true;
	}
	else
	{
		return false;
	}

}

void MFC_GetFrameDelayCount(u32 uProcessIdx, u32* uDelayCount)
{
	*uDelayCount = oMfc.m_uFrameDelayCount[uProcessIdx];
}

void MFC_SetWordData(u32 uAddr, u32 uWordData)
{
	u32 uRegData;
	u32 uTemp, i;
	u32 uRes = uAddr%4;

	if (uRes == 0)
	{
		Outp32(uAddr, uWordData);
	}
	else
	{
		uRegData = mfcInp32(uAddr-uRes);
//		DbgMfc("before: uRegData=0x%x\n",uRegData);
		uRegData &= ((2<<(uRes*8))-1);
//		DbgMfc("after: uRegData=0x%x\n",uRegData);
		
		for (i=uRes; i<4; i++)
		{
			uTemp = (uWordData>>(8*(i-uRes)))&0xff;
			uRegData |= uTemp<<(8*i);
		}
		Outp32(uAddr-uRes, uRegData);
//		DbgMfc("1:uRegData=0x%x\n",uRegData);
		
		uRegData = 0;
		for (i=0; i<uRes; i++)
		{
			uTemp = (uWordData>>(8*(4-uRes+i)))&0xff;
			uRegData |= uTemp<<(8*i);
		}
		Outp32(uAddr+4-uRes, uRegData);
//		DbgMfc("2:uRegData=0x%x\n",uRegData);
	}
}

void MFC_GetDecErrorMbNum(u32* uErrMbNum)
{
	*uErrMbNum = mfcInp32(RET_DEC_PIC_ERR_MB_NUM);
}

// true: I picture, false: P picture
void MFC_IsEncIntraPic(u8* bIsIpicture)
{
	u32 uPicType;
	uPicType = mfcInp32(RET_ENC_PIC_TYPE);
	*bIsIpicture = (uPicType & 0x1) ? false : true;
}

u8 MFC_GetEncPicSliceNum(u32 uProcessIdx, u32* uSliceNum)
{
	if (oMfc.m_bSliceInfoReport[uProcessIdx])
	{
	#ifdef DBG_MFC
		u32 i, uSlicePtr;
	#endif
		*uSliceNum = mfcInp32(RET_ENC_PIC_SLICE_NUM);
	#ifdef DBG_MFC
		for (i=0; i< *uSliceNum; i++)
		{
			uSlicePtr = mfcInp32(oMfc.m_uParameterBufStartAddr+0x1200+i*4);
			DbgMfc("[%2d] 0x%x\n", i, uSlicePtr);
		}
	#endif
		return true;
	}
	else
	{
		*uSliceNum = 0;
		return false;
	}
}

void MFC_GetEncMbBit(u32 uFreeMemStAddr, u32 uNumOfMbInWidth, u32 uNumOfMbInHeight)
{
	u32 uAddr;
	u32 uData;
	u16 uData16;
	u32 i=0;
	u32 uMbX, uMbY;

	for (uMbY=0; uMbY<uNumOfMbInHeight; uMbY++)
	{
		DbgMfc("[%2d] ", uMbY);
		for (uMbX=0; uMbX<uNumOfMbInWidth; uMbX++)
		{
			uAddr = oMfc.m_uParameterBufStartAddr + (uMbY*64 + uMbX) * 2;  // 16 bit
			uAddr = (uAddr/4)*4; // multiple of 4
			uData = mfcInp32(uAddr);
			uData16 = (uMbX%2 == 0) ? (uData >> 16) & 0xffff : uData & 0xffff;
			DbgMfc("%4d ", uData16);
			*(u16*)(uFreeMemStAddr+i*2) = uData16;
			i++;
		}
		DbgMfc("\n");
	}
}

void MFC_SetEncHeaderCode(MFC_ENC_HEADER_CODE eEncHeaderCode)
{
	u32 uHeaderCode;
	
	switch(eEncHeaderCode)
	{
		// MPEG-4
		case VOL_HEADER:
			uHeaderCode = 0;
			break;
		case VO_HEADER:
			uHeaderCode = 1;
			break;
		case VOS_HEADER:
			uHeaderCode = 2;
			break;
		case GOV_HEADER:
			uHeaderCode = 3;
			break;
		// H.264	
		case SPS_RBSP:
			uHeaderCode = 0;
			break;
		case PPS_RBSP:
			uHeaderCode = 1;
			break;
		default:
			Assert(0);
	}
	mfcOutp32(ENC_HEADER_CODE, uHeaderCode);
}

void MFC_GetEncHeaderCode(u32 uProcessIdx, MFC_ENC_HEADER_CODE* eEncHeaderCode)
{
	u32 uHeaderCode;

	uHeaderCode = mfcInp32(ENC_HEADER_CODE);

	if (oMfc.m_eCodecMode[uProcessIdx] == MP4_ENC)
	{
		*eEncHeaderCode = 
			(uHeaderCode == 0) ? VOL_HEADER :
			(uHeaderCode == 1) ? VO_HEADER :
			(uHeaderCode == 2) ? VOS_HEADER : GOV_HEADER;
	}
	else if (oMfc.m_eCodecMode[uProcessIdx] == AVC_ENC)
	{
		*eEncHeaderCode = (uHeaderCode == 0) ? SPS_RBSP : PPS_RBSP;
	}
	else
	{
		Assert(0);
	}
	
}

void MFC_SetDecParaSetType(MFC_PARA_SET_TYPE eParaSetType)
{
	u32 uParaSetType = (eParaSetType == SEQ_PARA_SET) ? 0 : 1;

	mfcOutp32(DEC_PARA_SET_TYPE, uParaSetType);	
}

void MFC_GetDecParaSetType(MFC_PARA_SET_TYPE* eParaSetType)
{
	u32 uParaSetType;

	uParaSetType = mfcInp32(DEC_PARA_SET_TYPE);	
	*eParaSetType = (uParaSetType == 0) ? SEQ_PARA_SET : PIC_PARA_SET;	
}

void MFC_SetDecParaSetSize(u32 uRbspBytes)
{
	Assert(uRbspBytes < 512);
	mfcOutp32(DEC_PARA_SET_SIZE, uRbspBytes);
}

void MFC_SetEncParaSetType(MFC_PARA_SET_TYPE eParaSetType)
{
	u32 uParaSetType = (eParaSetType == SEQ_PARA_SET) ? 0 : 1;

	mfcOutp32(ENC_PARA_SET_TYPE, uParaSetType);	
}

void MFC_GetEncParaSetType(MFC_PARA_SET_TYPE* eParaSetType)
{
	u32 uParaSetType;

	uParaSetType = mfcInp32(ENC_PARA_SET_TYPE);	
	*eParaSetType = (uParaSetType == 0) ? SEQ_PARA_SET : PIC_PARA_SET;	
}

void MFC_GetEncParaSetSize(u32* uRbspBytes)
{
	*uRbspBytes = mfcInp32(RET_ENC_PARA_SET_SIZE);
	Assert(*uRbspBytes < 512);
}

void MFC_GetFirmwareVersion(u32* uVersion)
{
	u32 uRegData;

	uRegData = mfcInp32(RET_GET_FW_VER);
	Assert(((uRegData>>16)&0xffff) == 0xf202); // product number
	*uVersion = uRegData&0xffff; // versino number (0xMmrr for M.m.rr)
}

void MFC_NotifyNoMoreStream(u32 uProcessIdx)
{
	Assert(uProcessIdx<MAX_PROCESS_NUM);
	
	oMfc.m_bIsNoMoreStream[uProcessIdx] = true;
	MFC_SetBitstreamEndFlag(true);
}

void MFC_GetNoMoreStreamInputed(u32 uProcessIdx, u8* isEnd)
{
	Assert(uProcessIdx<MAX_PROCESS_NUM);
	
	*isEnd = oMfc.m_bIsNoMoreStream[uProcessIdx];
}

void MFC_SetBitstreamEndFlag(u8 isWholeStreamInBuf)
{
	u32 uStreamEnd = (isWholeStreamInBuf) ? 1 : 0;

	mfcOutp32(DEC_FUNC_CTRL, uStreamEnd);
}

void MFC_GetBitstreamEnd(u8* isWholeStreamInBuf)
{
	u32 uStreamEnd;

	uStreamEnd = mfcInp32(DEC_FUNC_CTRL);
	*isWholeStreamInBuf = (uStreamEnd&1) ? true : false;
}

void MFC_SetWorkBufConfig(u8 isEnabled)
{
	u32 uWorkBufConfig = (isEnabled) ? 1 : 0;

	mfcOutp32(WORK_BUF_CTRL, uWorkBufConfig);
}

void MFC_GetWorkBufConfig(u8* isEnabled)
{
	u32 uWorkBufConfig;

	uWorkBufConfig = mfcInp32(WORK_BUF_CTRL);
	*isEnabled = (uWorkBufConfig&1) ? true : false;
}

// not used (MPEG4)
// PS data save buffer (H.264)
// MV direct prediction buffer (VC-1)
void MFC_SetProcessBufStAddr(u32 uProcessBufStAddr)
{
	Assert(uProcessBufStAddr%256 == 0);
	mfcOutp32(DEC_SEQ_PRO_BUF, uProcessBufStAddr);
}

// ACDC prediction buffer (MPEG4)
// Intra prediction Y buffer (H.264)
// ACDC prediction buffer (VC-1)
void MFC_SetDecTempBuf1StAddr(u32 uTempBuf1StAddr)
{
	Assert(uTempBuf1StAddr%256 == 0);
	mfcOutp32(DEC_SEQ_TMP_BUF_1, uTempBuf1StAddr);
}

// Data Partition part1 save buffer (MPEG4)
// Intra prediction Cb buffer (H.264)
// Deblocking buffer (VC-1)
void MFC_SetDecTempBuf2StAddr(u32 uTempBuf2StAddr)
{
	Assert(uTempBuf2StAddr%256 == 0);
	mfcOutp32(DEC_SEQ_TMP_BUF_2, uTempBuf2StAddr);
}

// Data partition part2 save buffer (MPEG4)
// Intra prediction Cr buffer (H.264)
// not used (VC-1)
void MFC_SetDecTempBuf3StAddr(u32 uTempBuf3StAddr)
{
	Assert(uTempBuf3StAddr%256 == 0);
	mfcOutp32(DEC_SEQ_TMP_BUF_3, uTempBuf3StAddr);
}

// not used (MPEG4)
// Slice information save buffer (H.264)
// not-used (VC-1)
void MFC_SetDecTempBuf4StAddr(u32 uTempBuf4StAddr)
{
	Assert(uTempBuf4StAddr%256 == 0);
	mfcOutp32(DEC_SEQ_TMP_BUF_4, uTempBuf4StAddr);
}

// not used (MPEG4)
// Slice save buffer (H.264)
// not used (VC-1)
void MFC_SetDecTempBuf5StAddr(u32 uTempBuf5StAddr)
{
	Assert(uTempBuf5StAddr%256 == 0);
	mfcOutp32(DEC_SEQ_TMP_BUF_5, uTempBuf5StAddr);
}

// ACDC prediction buffer (MPEG4)
// Intra prediction Y buffer (H.264)
void MFC_SetEncTempBuf1StAddr(u32 uTempBuf1StAddr)
{
	Assert(uTempBuf1StAddr%256 == 0);
	mfcOutp32(ENC_SEQ_TMP_BUF_1, uTempBuf1StAddr);
}

// Data Partition part1 save buffer (MPEG4)
// Intra prediction Cb buffer (H.264)
void MFC_SetEncTempBuf2StAddr(u32 uTempBuf2StAddr)
{
	Assert(uTempBuf2StAddr%256 == 0);
	mfcOutp32(ENC_SEQ_TMP_BUF_2, uTempBuf2StAddr);
}

// Data partition part2 save buffer (MPEG4)
// Intra prediction Cr buffer (H.264)
void MFC_SetEncTempBuf3StAddr(u32 uTempBuf3StAddr)
{
	Assert(uTempBuf3StAddr%256 == 0);
	mfcOutp32(ENC_SEQ_TMP_BUF_3, uTempBuf3StAddr);
}

// not used (MPEG4)
// FMO slice group buffer (H.264)
void MFC_SetEncTempBuf4StAddr(u32 uTempBuf4StAddr)
{
	Assert(uTempBuf4StAddr%256 == 0);
	mfcOutp32(ENC_SEQ_TMP_BUF_4, uTempBuf4StAddr);
}

void MFC_SetDeblockAddrY(u32 uDbkAddrY)
{
	mfcOutp32(DEC_PIC_DEBLOCK_ADDR_Y, uDbkAddrY);
}

void MFC_SetDeblockAddrCb(u32 uDbkAddrCb)
{
	mfcOutp32(DEC_PIC_DEBLOCK_ADDR_CB, uDbkAddrCb);
}

void MFC_SetDeblockAddrCr(u32 uDbkAddrCr)
{
	mfcOutp32(DEC_PIC_DEBLOCK_ADDR_CR, uDbkAddrCr);
}

void MFC_SetDecRotStride(u32 uRotStride)
{
	Assert(uRotStride<=720);
	mfcOutp32(DEC_PIC_ROT_STRIDE, uRotStride);
}

void MFC_SetEncRunOption(u8 bPicSkipEn, u8 bIdrPicEn)
{
	u32 uIdrPicEnBit = (bIdrPicEn) ? (1<<1) : (0<<1);
	u32 uPicSkipEnBit = (bPicSkipEn) ? (1<<0) : (0<<0);

	mfcOutp32(ENC_PIC_OPTION, uIdrPicEnBit|uPicSkipEnBit);
}

void MFC_GetCodeBufEndAddr(u32* uEndAddr)
{
	*uEndAddr = oMfc.m_uCodeBufEndAddr;
}

void MFC_GetBaseBufEndAddr(u32* uEndAddr)
{
	*uEndAddr = oMfc.m_uParameterBufEndAddr;
}

void MFC_GetWorkingBufEndAddr(u32* uEndAddr)
{
	*uEndAddr = oMfc.m_uWorkingBufEndAddr;
}

void MFC_GetProcessBufEndAddr(u32 uProcessIdx, u32* uEndAddr)
{
	*uEndAddr = oMfc.m_uLastFrameBufEndAddr[uProcessIdx];
}

void MFC_SetStreamPointer(u32 uProcessIdx, u32 uStreamPtr)
{
	oMfc.m_uStreamPtr[uProcessIdx] = uStreamPtr;
}

void MFC_GetStreamPointer(u32 uProcessIdx, u32* uStreamPtr)
{
	*uStreamPtr = oMfc.m_uStreamPtr[uProcessIdx];
}

void MFC_ParseConfigFileAndSetParameters(u32 uProcessIdx, u32 uConfigFileStAddr)
{
	char* pConfigFileStr = (char*)uConfigFileStAddr;
	char* pch[40];
	u32 j=1, i;
	char* ptr;
	u32 uEncTotalFrameNum;
	u32 uEncWidth;
	u32 uEncHeight;
	u32 uEncFrameRate;
	u32 bIsH263;

	int uEncChromaQpOffset;
	u8 bEncConstIntraFlag;
	u32 uEncDeblkMode;
	int uEncDeblkAOffset;
	int uEncDeblkBOffset;

	u8 bEncAnnexJ;
	u8 bEncAnnexK;
	u8 bEncAnnexT;

	u8 bEncDataPartEn;
	u8 bEncRevVlcEn;
	u32 uEncIntraDcVlcThr;

	u32 uEncPicQs;
	u32 uEncGopNum;
	u8 bEncSliceMode;
	u8 bEncSliceSizeMode;
	u32 uEncSliceSizeNum;

	u8 bEncRcEnable;
	u32 uEncRcBitRate;
	u32 uEncRcInitDelay;
	u32 uEncRcBufSize;	

	u32 uEncIntraRefreshNum;

	u8 bEncFmoEnable;
	u32 uEncFmoType;
	u32 uEncNumOfSliceGroup;

	char* pSrcYuvFileName;
	char* pGoldenYuvFileName;
	char* pGoldenStreamFileName;

	MFC_CODEC_MODE eEncMode;

	pch[0] = strtok(pConfigFileStr, ";-\n");
	//DbgMfc("[0] %s\n",pch[0]);

	while(1)
	{		
		pch[j] = strtok(NULL, "\n");
		if (pch[j] == NULL)
		{
			break;
		}
		else if (*(pch[j]) != ';')
		{
			j++;
		}
	}

	for (i=0; i<j; i++)
	{
		ptr = strchr(pch[i], ' ');
		*ptr = 0;
		ptr = strchr(pch[i], '\t');
		*ptr = 0;
		//DbgMfc("[%d.] %s\n",i,pch[i]);
	}
	*(pch[j]) = 0; // terminate parsing
	
	ptr = strrchr(pch[0], '\\');
	pSrcYuvFileName = (ptr == NULL) ? pch[0] : ptr+1;
	DbgMfc("pSrcYuvFileName=%s\n",pSrcYuvFileName);

	ptr = strrchr(pch[1], '\\');
	pGoldenYuvFileName = (ptr == NULL) ? pch[1] : ptr+1;
	DbgMfc("pGoldenYuvFileName=%s\n",pGoldenYuvFileName);

	ptr = strrchr(pch[2], '\\');
	pGoldenStreamFileName = (ptr == NULL) ? pch[2] : ptr+1;
	DbgMfc("pGoldenStreamFileName=%s\n",pGoldenStreamFileName);

	MFC_GetCodecModeFromName(pGoldenStreamFileName, false, &eEncMode);	
	
	uEncTotalFrameNum = atoi(pch[3]);
	uEncWidth = atoi(pch[4]);
	uEncHeight = atoi(pch[5]);
	uEncFrameRate = atoi(pch[6]);

	if (eEncMode == AVC_ENC) // H.264
	{
		bEncConstIntraFlag = (atoi(pch[7]) == 0) ? false : true;
		uEncDeblkMode = atoi(pch[8]);
		uEncDeblkAOffset = atoi(pch[9]);
		uEncDeblkBOffset = atoi(pch[10]);
		uEncChromaQpOffset = atoi(pch[11]);
		uEncPicQs = atoi(pch[12]);
		uEncGopNum = atoi(pch[13]);

		bEncSliceMode = (atoi(pch[14]) == 0) ? false : true;
		bEncSliceSizeMode = (atoi(pch[15]) == 0) ? false : true;
		uEncSliceSizeNum = atoi(pch[16]);

		bEncRcEnable = (atoi(pch[17]) == 0) ? false : true;
		uEncRcBitRate = atoi(pch[18]);
		uEncRcInitDelay = atoi(pch[19]);
		uEncRcBufSize = atoi(pch[20]);	

		uEncIntraRefreshNum = atoi(pch[21]);	

		// FMO (3)
		if (*(pch[22]) != 0) // FMO support
		{
			bEncFmoEnable = (atoi(pch[22]) == 0) ? false : true;
			uEncFmoType = atoi(pch[23]);
			uEncNumOfSliceGroup = atoi(pch[24]);
			DbgMfc("uEncFmoType=%d, uEncNumOfSliceGroup=%d\n", uEncFmoType, uEncNumOfSliceGroup);
		}
		else
		{
			bEncFmoEnable = false;
			uEncFmoType = 0;
			uEncNumOfSliceGroup = 1;
		}
	}
	else if (eEncMode == MP4_ENC) // MPEG4, H.263
	{
		bEncDataPartEn = (atoi(pch[8]) == 0) ? false : true;
		bEncRevVlcEn = (atoi(pch[9]) == 0) ? false : true;
		uEncIntraDcVlcThr = atoi(pch[10]);
		bIsH263 = (atoi(pch[11]) == 0) ? false : true;
		Assert(atoi(pch[12]) == 0); // annex I disable
		bEncAnnexJ = (atoi(pch[13]) == 0) ? false : true;
		bEncAnnexK = (atoi(pch[14]) == 0) ? false : true;
		bEncAnnexT = (atoi(pch[15]) == 0) ? false : true;

		uEncPicQs = atoi(pch[16]);
		uEncGopNum = atoi(pch[17]);
		bEncSliceMode = (atoi(pch[18]) == 0) ? false : true;
		bEncSliceSizeMode = (atoi(pch[19]) == 0) ? false : true;
		uEncSliceSizeNum = atoi(pch[20]);

		bEncRcEnable = (atoi(pch[21]) == 0) ? false : true;
		uEncRcBitRate = atoi(pch[22]);
		uEncRcInitDelay = atoi(pch[23]);
		uEncRcBufSize = atoi(pch[24]);

		uEncIntraRefreshNum = atoi(pch[25]);
	}
	else
		Assert(0);


	// Set encoding parameters.
	//-----------------------------

	MFC_SetEncYuvFileCfg(uProcessIdx, pSrcYuvFileName, uEncTotalFrameNum, 
		uEncWidth, uEncHeight, uEncFrameRate);
	MFC_SetEncGoldenFileCfg(uProcessIdx, pGoldenYuvFileName, pGoldenStreamFileName);
	MFC_SetEncSliceCfg(uProcessIdx, (bEncSliceMode) ? 1 : 0, (bEncSliceSizeMode) ? 1 : 0, uEncSliceSizeNum);
	MFC_SetEncErrResilienceCfg(uProcessIdx, uEncIntraRefreshNum);

	if (eEncMode == MP4_ENC)
	{
		if (bIsH263)
			MFC_SetEnc263Cfg(uProcessIdx, 0, (bEncAnnexJ) ? 1 : 0, (bEncAnnexK) ? 1 : 0, 
				(bEncAnnexT) ? 1 : 0, uEncPicQs, uEncGopNum);
		else
			MFC_SetEncMpeg4Cfg(uProcessIdx, (bEncDataPartEn) ? 1 : 0, (bEncRevVlcEn) ? 1 : 0, 
				uEncIntraDcVlcThr, uEncPicQs, uEncGopNum);
	}
	else if (eEncMode == AVC_ENC)
	{
		MFC_SetEnc264Cfg(uProcessIdx, (bEncConstIntraFlag) ? 1 : 0, uEncDeblkMode, 
			uEncDeblkAOffset, uEncDeblkBOffset, uEncChromaQpOffset, uEncPicQs, uEncGopNum);
		MFC_SetEnc264FmoCfg(uProcessIdx, bEncFmoEnable, uEncFmoType, uEncNumOfSliceGroup);
	}
	MFC_SetEncRateCtrlCfg(uProcessIdx, (bEncRcEnable) ? 1 : 0, uEncRcBitRate, uEncRcInitDelay, uEncRcBufSize);

}

void MFC_GetCodecModeFromName(const char* pStreamFileName, u8 bIsDec, MFC_CODEC_MODE* eDecMode)
{
	u32 i, uExtIdx;
	u32 length=0;
	int j = -1;
	u32 dotPos[5];  // 5: the number of file extension letters
	char* fileExt;

	for (i=0; i<256; i++)
	{
		if (*(pStreamFileName+i) == 0)
			break;
		length++;
	}

	for (i=0; i<length; i++)
	{
		if (*(pStreamFileName+i) == '.')
		{
			j++;
			dotPos[j] = i;
		}
	}

	if (j == -1)
		fileExt = (char*)0;
	else
	{
		uExtIdx = dotPos[j]+1;
		fileExt = (char*)(pStreamFileName + uExtIdx);
	}
	//DbgMfc("ext = %s\n", fileExt);

	MFC_GetCodecModeByName(fileExt, bIsDec, eDecMode);
}

void MFC_GetGoldenStreamFileName(u32 uProcessIdx, char* pGoldenStreamName)
{
	//pGoldenStreamName = oMfc.m_EncCfg[uProcessIdx].GoldenStreamFileName; 
	strcpy(pGoldenStreamName, oMfc.m_EncCfg[uProcessIdx].GoldenStreamFileName);
}

void MFC_GetGoldenYuvFileName(u32 uProcessIdx, char* pGoldenYuvName)
{
	//pGoldenYuvName = oMfc.m_EncCfg[uProcessIdx].GoldenYuvFileName;
	strcpy(pGoldenYuvName, oMfc.m_EncCfg[uProcessIdx].GoldenYuvFileName);
}

void MFC_GetSrcYuvFileName(u32 uProcessIdx, char* pSrcYuvName)
{
	//pSrcYuvName = oMfc.m_EncCfg[uProcessIdx].SrcFileName;
	strcpy(pSrcYuvName, oMfc.m_EncCfg[uProcessIdx].SrcFileName);
}

void MFC_GetEncPicSizeCfg(u32 uProcessIdx, u32* uPicWidth, u32* uPicHeight)
{
	*uPicWidth = oMfc.m_EncCfg[uProcessIdx].PicX;
	*uPicHeight = oMfc.m_EncCfg[uProcessIdx].PicY;
}

void MFC_GetEncNumOfFramesCfg(u32 uProcessIdx, u32* uNumOfFrames)
{
	*uNumOfFrames = oMfc.m_EncCfg[uProcessIdx].NumOfFrame;
}

void MFC_GetEncFrameRateCfg(u32 uProcessIdx, u32* uFrameRate)
{
	*uFrameRate = oMfc.m_EncCfg[uProcessIdx].FrameRate;
}
