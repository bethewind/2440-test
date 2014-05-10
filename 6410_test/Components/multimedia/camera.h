/*----------------------------------------------------------------------
 *
 * Filename: camif.h
 *
 * Contents: Declaration of "class CAMIF"
 *
 * Abbreviations:
 *
 * Authors: Dharma, Sunny
 *
 * Notes: Camera Interface V3.0
 *
 * Copyright (c) 2006 SAMSUNG Electronics.
 *
 *----------------------------------------------------------------------
 */



#ifndef __CIM_H__
#define __CIM_H__

#include "def.h"
#include "dma.h"
#include "cameram.h"


typedef enum 
{
	BY_PASS, COLOR_BAR, HORIZONTAL_INCR, VERTICAL_INCR
}TEST_PATTERN;

typedef enum 
{
	BYPASS, ARBITRARY_CBCR, NEGATIVE, ART_FREEZE, EMBOSSING, SILHOUETTE
}IMAGE_EFFECT;

typedef enum 
{
	FIFO, DMA
}CAMIF_INOUT;

typedef enum 
{
	P_PATH, C_PATH
}PROCESS_PATH;

typedef enum 
{
	FIMC_FREE_RUN, FIMC_ONE_SHOT
}FIMC_RUN_MODE;

typedef struct
{
	CAMERA m_oCamera;

	CAMIF_INOUT m_eInputPath;
	CAMIF_INOUT m_eOutputPath;
	PROCESS_PATH m_eProcessPath;
	FIMC_RUN_MODE m_eMode;
	CSPACE m_eCamSrcFmt, m_eMemSrcFmt, m_eDstFmt;
	CAM_ATTR m_eCcir;
	u32  m_uIfBits;
	ITU_R_STANDARD  m_eItuR;
	ROT_DEG m_eInRotDeg, m_eOutRotDeg;
	TV_SCAN_MODE		m_eScanMode;
	u8 m_bHighRst, m_bInvPclk, m_bInvVsync, m_bInvHref;
	u32  m_uSrcHsz, m_uSrcVsz;
	u32  m_uDstHsz, m_uDstVsz;

	u8 m_bIsScalerBypass;
	u32  m_uMainScalerCtrl, m_uMSDMACtrl, m_uCigCtrl;
	u32  m_uMainBurstSz;
	u32  m_uSrcAddr0, m_uSrcAddr1;
	
	u32  m_uCptCnt, m_uCptPtr;
	u8  m_bIsCptFrCnt;
} CIM;


void CAMERA_InitPreviewPath(u32 uDstHsz, u32 uDstVsz, u32 uDstAddr, CSPACE eDstFmt,
		FLIP_DIR eFlip, ROT_DEG eRotDeg);
void CAMERAR_InitPreviewPath0(
	u32 uSrcCropStartX, u32 uSrcCropStartY, u32 uSrcCropHsz, u32 uSrcCropVsz, 
	u32 uDisplayHsz, u32 uDisplayVsz, u32 uDstStX, u32 uDstStY, u32 uDstHsz, u32 uDstVsz, 
	u32 uDstAddr0, u32 uDstAddr1, CSPACE eDstFmt, FLIP_DIR eFlip, ROT_DEG eRotDeg);
void CAMERA_InitPreviewPath2(u32 uSrcCropStartX, u32 uSrcCropStartY, u32 uSrcCropHSz, u32 uSrcCropVSz,
		u32 uDstHsz, u32 uDstVsz, u32 uDstAddr,  CSPACE eDstFmt,
		FLIP_DIR eFlip , ROT_DEG eRotDeg );
void CAMERA_InitPreviewPath_1(u32 uDstHsz, u32 uDstVsz, u32 uDstAddr0, u32 uDstAddr1, CSPACE eDstFmt,
		FLIP_DIR eFlip, ROT_DEG eRotDeg);
void CAMERA_SetBasicSfr(u32 uSrcCropStartX, u32 uSrcCropStartY, u32 uSrcCropHsz, u32 uSrcCropVsz,
	u32 uDstHsz, u32 uDstVsz, u32 uDstAddr0, u32 uDstAddr1, CSPACE eDstDataFmt,
	CAMIF_INOUT eInputPath, PROCESS_PATH ePath, CAMIF_INOUT eOutputMode, FLIP_DIR eFlip, ROT_DEG eRotDeg);
void CAMERA_CalcRgbBurstSize(u32* uMainBurstSz, u32* uRemainBurstSz);
void CAMERA_CalcYCbCrBurstSize(  BURST_MODE eWantBurstSz, u32* uYMainBurstSz, u32* uYRemainBurstSz,
		u32* uCMainBurstSz, u32* uCRemainBurstSz);
void CAMERA_InitCodecPath(u32 uDstHsz, u32 uDstVsz, u32 uDstAddr, CSPACE eDstFmt,
		FLIP_DIR eFlip, ROT_DEG eRotDeg);
void CAMERA_InitCodecPath0(u32 uSrcCropStartX, u32 uSrcCropStartY, u32 uSrcCropHsz, u32 uSrcCropVsz, 
		u32 uDisplayHsz, u32 uDisplayVsz, u32 uDstStX, u32 uDstStY, u32 uDstHsz, u32 uDstVsz, 
		u32 uDstAddr0, u32 uDstAddr1, CSPACE eDstFmt, FLIP_DIR eFlip, ROT_DEG eRotDeg);
void CAMERA_InitDmaInPath(u32 uSrcOrgHsz, u32 uSrcOrgVsz, u32 uSrcAddr, CSPACE eSrcFmt,
		u32 uDstHsz, u32 uDstVsz, u32 uDstAddr, CSPACE eDstFmt,
		FLIP_DIR eFlip, ROT_DEG eRotDeg, PROCESS_PATH ePathMode);

void CAMERA_InitDmaInPath0(u32 uSrcOrgHsz, u32 uSrcOrgVsz, u32 uSrcCropStartX, u32 uSrcCropStartY, 
		u32 uSrcCropHsz, u32 uSrcCropVsz, u32 uSrcAddr0, u32 uSrcAddr1, CSPACE eSrcFmt,
		u32 uDisplayHsz, u32 uDisplayVsz, u32 uDstStX, u32 uDstStY, u32 uDstHsz, u32 uDstVsz, 
		u32 uDstAddr0, u32 uDstAddr1, CSPACE eDstFmt, FLIP_DIR eFlip, ROT_DEG eRotDeg, PROCESS_PATH ePathMode);
void CAMERA_InitPreviewPath0(
	u32 uSrcCropStartX, u32 uSrcCropStartY, u32 uSrcCropHsz, u32 uSrcCropVsz, 
	u32 uDisplayHsz, u32 uDisplayVsz, u32 uDstStX, u32 uDstStY, u32 uDstHsz, u32 uDstVsz, 
	u32 uDstAddr0, u32 uDstAddr1, CSPACE eDstFmt, FLIP_DIR eFlip, ROT_DEG eRotDeg);
void CAMERA_InitPreviewPathToFifoOut0(
	u32 uSrcCropStartX, u32 uSrcCropStartY, u32 uSrcCropHsz, u32 uSrcCropVsz, 
	u32 uDstHsz, u32 uDstVsz, CSPACE eDstFmt);
void CAMERA_InitDmaInPathToFifoOut(u32 uSrcOrgHsz, u32 uSrcOrgVsz, u32 uSrcAddr, CSPACE eSrcFmt,
		u32 uDstHsz, u32 uDstVsz, CSPACE eDstFmt,
		ROT_DEG eRotDeg, PROCESS_PATH ePathMode);
void CAMERA_InitDmaInPathToFifoOut0(u32 uSrcOrgHsz, u32 uSrcOrgVsz, u32 uSrcCropStartX, u32 uSrcCropStartY, 
		u32 uSrcCropHsz, u32 uSrcCropVsz, u32 uSrcAddr0, u32 uSrcAddr1, CSPACE eSrcFmt,
		u32 uDstHsz, u32 uDstVsz, CSPACE eDstFmt, 
		ROT_DEG eRotDeg, PROCESS_PATH ePathMode);
void CAMERA_CalcRatioAndShift(u32 uSrcHOrVsz, u32 uDstHOrVsz, u32* uRatio, u32* uShift);
void CAMERA_SetDstScanOffset(u32 uDisplayHsz, u32 uDisplayVsz, u32 uDisplayStartX, u32 uDisplayStartY, u32 uDstAddr0, u32 uDstAddr1);
void CAMERA_StartPreviewPath(void);
void CAMERA_StopPreviewPath(void);
void CAMERA_SetArbitraryCbCr(u32 uCbDegree, u32 uCrDegree);
void CAMERA_SetImageEffect(IMAGE_EFFECT eEffect);
void CAMERA_GetSrcImgSz(u32* uSrcHsz, u32* uSrcVsz);
void CAMERA_InitSensor1(IMG_SIZE eSize, CAM_ATTR eCcir, CSPACE eSrcFmt);
void CAMERA_SetSensorSize(IMG_SIZE eSize);
void CAMERA_GetFrameNum(PROCESS_PATH eProcessPath, u32* uFrmNum);
void CAMERA_StartDmaInPath(void);
void CAMERA_ClearFrameEndStatus(void);
void CAMERA_StopDmaInPath(void);;
void CAMERA_InitPreviewPathToFifoOut(u32 uDstHsz, u32 uDstVsz, CSPACE eDstFmt);
void CAMERA_StartCodecPath(u32 uCptCnt);
void CAMERA_StopCodecPath(void);
void CAMERA_InitSensor(void);
void CAMERA_ResetSensor(void);
void CAMERA_ResetIp(void);
u8 CAMERA_IsProcessingDone(void);
void CAMERA_EnableTestMode(TEST_PATTERN ePattern);
void	CAMERA_SetClearPreviewInt(void);
void	CAMERA_SetClearCodecInt(void);
void CAMERA_ClkSetting(void);
void CAMERA_SFRInit(void);
void CAMERA_EnablePreviewLastIRQ(void);
void CAMERA_DisableImageCapture( void);
void CAMERA_DisableImageCapturePreview( void);
void CAMERA_DisablePreviewScaler( void);
void CAMERA_SetPort(void);
void CAMERA_StartMSDmaCodecPath(void);
void CAMERA_StartMSDmaPreviewPath(void);
void CAMERA_InitCodecPathToFifoOut0(
	u32 uSrcCropStartX, u32 uSrcCropStartY, u32 uSrcCropHsz, u32 uSrcCropVsz, 
	u32 uDstHsz, u32 uDstVsz, CSPACE eDstFmt);
void CAMERA_InitCodecPathToFifoOut(u32 uDstHsz, u32 uDstVsz, CSPACE eDstFmt);

//void CAMERA_ReadCameraModuleStatus(void);
void CAMERA_StartPreviewPathCPT(u32 uCptCnt);


void CAMERA_SetDstScanOffset_4Buf(u32 uDisplayHSz, u32 uDisplayVSz, 
	u32 uDisplayStartX, u32 uDisplayStartY, u32 uDstAddr0, u32 uDstAddr1, u32 uDstAddr2, u32 uDstAddr3 );
void CAMERA_SetBasicSfr_4Buf(u32 uSrcCropStartX, u32 uSrcCropStartY, u32 uSrcCropHsz, u32 uSrcCropVsz,
	u32 uDstHsz, u32 uDstVsz, u32 uDstAddr0, u32 uDstAddr1, u32 uDstAddr2, u32 uDstAddr3,  CSPACE eDstDataFmt,
	CAMIF_INOUT eInputPath, PROCESS_PATH ePath, CAMIF_INOUT eOutputMode, FLIP_DIR eFlip, ROT_DEG eRotDeg);
void CAMERA_InitPreviewPath_4Buf(u32 uDstHsz, u32 uDstVsz, u32 uDstAddr0, u32 uDstAddr1, u32 uDstAddr2, u32 uDstAddr3,  CSPACE eDstFmt, FLIP_DIR eFlip, ROT_DEG eRotDeg);
void CAMERA_InitPreviewPath_4Buf_0(
	u32 uSrcCropStartX, u32 uSrcCropStartY, u32 uSrcCropHsz, u32 uSrcCropVsz, 
	u32 uDisplayHsz, u32 uDisplayVsz, u32 uDstStX, u32 uDstStY, u32 uDstHsz, u32 uDstVsz, 
	u32 uDstAddr0, u32 uDstAddr1, u32 uDstAddr2, u32 uDstAddr3, CSPACE eDstFmt, FLIP_DIR eFlip, ROT_DEG eRotDeg);
void CAMERA_OneToOneDataCopy(PROCESS_PATH eProcessPath);

void CAMERA_InitItuRInterface(ITU_R_STANDARD eItuR, CSPACE eSrcFmt, u32 uSrcHsz, u32 uSrcVsz);


void CAMERA_SetScanMode(TV_SCAN_MODE  eScanMode);
void CAMERA_SetFieldClk( u8 bIsEdgeDlyCnt, u8 bIsNormal);

void CAMERA_InitSensor_2(void);
void CAMERA_WaitMSDMAC(void);
void CAMERA_WaitMSDMAP(void);

#endif //__CIM_H__

