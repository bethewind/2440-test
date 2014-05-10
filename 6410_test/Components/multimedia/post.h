/*----------------------------------------------------------------------
 *
 * Filename: post.h
 *
 * Contents: Declaration of "class POST"
 *
 * Authors: Dharma
 *
 * Notes:
 *
 * Copyright (c) 2003 SAMSUNG Electronics.
 *
 *----------------------------------------------------------------------
 */

#ifndef __POST_H__
#define __POST_H__

#define MAX_FRM_BUF_NUM   (30)

//Interrupt Level
#define POST_EDGE_INT		(0)
#define POST_LEVEL_INT		(1)

#define POST_CLKVAL_ALWAYS	(0)
#define POST_CLKVAL_StOfFRAME	(1)

#define POST_PORT		(0)
#define SCALER_PORT		(1)

typedef enum
{
	POST_DMA, POST_FIFO
} POST_PATH;

typedef enum
{
	ONE_SHOT, FREE_RUN
} POST_RUN_MODE;

typedef enum
{
	POST_SW_TRIG, POST_HW_TRIG
} POST_TRIG_MODE;

typedef enum
{
	POST_FRAME_END, POST_FIELD_END
} POST_BUFFER_CHANGE;

typedef enum
{
	POST_PROGRESSIVE_MODE, POST_INTERLACE_MODE
} POST_SCAN_MODE;

typedef enum
{
	RGB, YUV
} POST_FIFO_IF;

typedef enum
{
	POST_A, POST_B //tv scaler
} POST_CH;

typedef enum
{
	HCLK = 0, PLL_EXT_MOUTEPLL = 1, PLL_EXT_FINEPLL = 2, PLL_EXT_DOUTMPLL= 3, EXT_27MHZ = 4
} POST_CLK_SRC;

typedef enum
{
	SCALER_TVOUT, SCALER_FIMD_WIN1, SCALER_FIMD_WIN2
} SCALER_PATH;

typedef struct
{
	u32  m_uBaseAddr;
	u32  m_uModeRegValue;
	CSPACE m_eSrcCSpace, m_eDstCSpace;
	u8 m_bFreeRunMode;

	u32  m_uLastFrmBufIdx;
	u32  m_uSrcFrmStAddr[MAX_FRM_BUF_NUM];
	u32  m_uSrcStY[MAX_FRM_BUF_NUM], m_uSrcStCb[MAX_FRM_BUF_NUM], m_uSrcStCr[MAX_FRM_BUF_NUM];
	u32  m_uSrcEndY[MAX_FRM_BUF_NUM], m_uSrcEndCb[MAX_FRM_BUF_NUM], m_uSrcEndCr[MAX_FRM_BUF_NUM];

	u32  m_uStPosY, m_uEndPosY;
	u32  m_uStPosCb, m_uStPosCr, m_uEndPosCb, m_uEndPosCr;
	u32  m_uStPosRgb, m_uEndPosRgb;
	u32  m_uOutStPosCb, m_uOutStPosCr, m_uOutEndPosCb, m_uOutEndPosCr;
} POST;

void POST_InitCh(POST_CH eCh, POST *sCh);

void POST_InitIpForDmaInDmaOut(
	u32 uSrcWidth, u32 uSrcHeight, u32 uSrcFrmSt, CSPACE eSrcCSpace,
	u32 uDstWidth, u32 uDstHeight, u32 uDstFrmSt, CSPACE eDstCSpace,
	u32 uSrcFrmBufNum, u8 bIsDoubleBuf, POST_RUN_MODE eMode, POST *sCh
	); // DMA In Path + DMA Out Path

void POST_InitIpForDmaInFifoOut(
	u32 uSrcWidth, u32 uSrcHeight, u32 uSrcFrmSt, CSPACE eSrcCSpace,
	u32 uDstWidth, u32 uDstHeight, POST_FIFO_IF eFifoIf, u32 uSrcFrmBufNum, POST *sCh
	); // DMA In path + Local Out Path

void POST_InitIpForFifoInDmaOut(
	u32 uSrcWidth, u32 uSrcHeight,
	u32 uDstWidth, u32 uDstHeight, u32 uDstFrmSt, CSPACE eDstCSpace,
	u8 bIsDoubleBuf, POST_RUN_MODE eMode, POST *sCh
	); // Local In path + DMA Out Path

void POST_InitIpForFifoInFifoOut(
	u32 uSrcWidth, u32 uSrcHeight,
	u32 uDstWidth, u32 uDstHeight, POST_FIFO_IF ePostIf, POST_RUN_MODE eMode, POST *sCh
	);	// Local In path + Local Out Path

void POST_InitIp1(
	u32 uSrcFullWidth,	u32 uSrcFullHeight,
	u32 uSrcStartX,		u32 uSrcStartY,
	u32 uSrcWidth,		u32 uSrcHeight,
	u32 uSrcFrmSt, 		CSPACE eSrcCSpace,
	u32 uDstFullWidth,	u32 uDstFullHeight,
	u32 uDstStartX,		u32 uDstStartY,
	u32 uDstWidth,		u32 uDstHeight,
	u32 uDstFrmSt, 		CSPACE eDstCSpace,
	u32 uSrcFrmBufNum, u8 bIsDoubleBuf, POST_RUN_MODE eMode,
	POST_PATH eInPath, POST_PATH eOutPath, POST *sCh
	);

void POST_InitIp(	u32 uSrcWidth, u32 uSrcHeight, u32 uSrcFrmSt, CSPACE eSrcCSpace,
	u32 uDstWidth, u32 uDstHeight, u32 uDstFrmSt, CSPACE eDstCSpace,
	u32 uSrcFrmBufNum, u8 bIsDoubleBuf, POST_RUN_MODE eMode, POST *sCh
	);
void POST_InitIp1(
	u32 uSrcFullWidth,	u32 uSrcFullHeight,
	u32 uSrcStartX,		u32 uSrcStartY,
	u32 uSrcWidth,		u32 uSrcHeight,
	u32 uSrcFrmSt, 		CSPACE eSrcCSpace,
	u32 uDstFullWidth,	u32 uDstFullHeight,
	u32 uDstStartX,		u32 uDstStartY,
	u32 uDstWidth,		u32 uDstHeight,
	u32 uDstFrmSt, 		CSPACE eDstCSpace,
	u32 uSrcFrmBufNum, u8 bIsDoubleBuf,
	POST_RUN_MODE eMode, POST_PATH eInPath, POST_PATH eOutPath, POST *sCh
	);

void POST_StartProcessing(POST *sCh);
void POST_StartProcessing1(u32 uSrcFrmIdx, u32 uDstBufIdx, POST *sCh);
void POST_GetSrcStAddr(u32 uSrcFrmBufIdx, u32 *uStAddr, POST *sCh);
void POST_SetCLKVALUpdate(u32 uUpdate, POST *sCh);
u8 POST_IsProcessingDone(POST *sCh);
void POST_ClearPending(POST *sCh);
u8 POST_IsPendingEnabled(POST *sCh);
void POST_SetNextFrameStAddr(u32 uNxtSrcFrmSt, u32 uNxtDstFrmSt, POST *sCh); // memory path
void POST_StopProcessingOfFreeRun(POST *sCh);
void POST_DisableChangeNextFrame(POST *sCh);
void POST_SetInterlaceModeOnOff(int onoff, POST *sCh);
void POST_SetInFifoMode(POST_PATH eInPostPath, POST *sCh);
u8 POST_IsFreeRunDone(POST *sCh);
void POST_SetClkSrc(POST_CLK_SRC eClkSrc, POST *sCh);
void POST_SetDataPath(POST_PATH eInPath, POST_PATH eOutPath, POST *sCh);
void POST_SetDataFormat(	CSPACE eSrcCSpace, CSPACE eDstCSpace, POST_PATH eInPath, POST_PATH eOutPath, POST *sCh,
								u32 *uPixelSzIn, u32 *uPixelSzOut);
void POST_SetScaler(u32 uSrcWidth, u32 uSrcHeight, u32 uDstWidth, u32 uDstHeight, POST *sCh);
void POST_SetDMA(	u32 uSrcFullWidth, u32 uSrcFullHeight, u32 uSrcStartX, u32 uSrcStartY, u32 uSrcWidth, u32 uSrcHeight, u32 uSrcFrmSt, CSPACE eSrcCSpace,
						u32 uDstFullWidth, u32 uDstFullHeight, u32 uDstStartX, u32 uDstStartY, u32 uDstWidth, u32 uDstHeight, u32 uDstFrmSt, CSPACE eDstCSpace,
						u32 uSrcFrmBufNum,  POST_PATH eInPath, POST_PATH eOutPath, POST *sCh,
						u32 uPixelSzIn, u32 uPixelSzOut);
void POST_SetAutoLoadEnable(POST_RUN_MODE eMode, POST *sCh);
void POST_EnableInterrupt(u32 uIntLevel, POST *sCh);
void POST_DisableInterrupt(POST *sCh);
void POST_CalcurateScaleOffset(	u32 uSrcWidth, u32 uSrcHeight, u32 uDstWidth, u32 uDstHeight, POST *sCh, 
									u32 *uHOffset, u32 *uVOffset, u32 *uHRatio, u32 *uVRatio, CSPACE eSrcCSpace);

void POST_SetClockDivide(u32 uDivider, POST *sCh);
u32  POST_GetClockDivide(POST *sCh);
u8 POST_CheckAutoLoadEnable(POST *sCh);
void POST_SetScalerforTVout(u32 uSrcImgSizeX, u32 uSrcImgSizeY, u32 uSrcFrameAddr, CSPACE eSrcCSpace, 
								u32 uDstImgSizeX, u32 uDstImgSizeY, POST *sCh);
void POST_SetFifoOutPath(SCALER_PATH ePath, POST *sCh);								

 
#endif /*__POST_H__*/ 
