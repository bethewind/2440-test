#ifndef __MFC_H__
#define __MFC_H__


typedef enum
{
    MP4_DEC,
    MP4_ENC,
    AVC_DEC,
    AVC_ENC,
    VC1_DEC
} MFC_CODEC_MODE;

typedef enum
{
    SEQ_INIT,
	SEQ_END,
	PIC_RUN,
	SET_FRAME_BUF,
	ENCODE_HEADER,
	ENC_PARA_SET,
	DEC_PARA_SET,
	GET_FW_VER
} MFC_COMMAND;

typedef enum
{
	VOL_HEADER,
	VO_HEADER,
	VOS_HEADER,
	GOV_HEADER,
	SPS_RBSP,
	PPS_RBSP
} MFC_ENC_HEADER_CODE;

typedef enum
{
	SEQ_PARA_SET,
	PIC_PARA_SET
} MFC_PARA_SET_TYPE;

typedef enum
{
	INT_MFC_DNLOAD_DONE,
	INT_MFC_SEQ_INIT,
	INT_MFC_SEQ_END,
	INT_MFC_PIC_RUN,
	INT_MFC_SET_FRAME_BUF,
	INT_MFC_ENC_HEADER,
	INT_MFC_ENC_PARA_SET,
	INT_MFC_DEC_PARA_SET,
	INT_MFC_BIT_BUF_EMPTY,
	INT_MFC_BIT_BUF_FULL,
	INT_MFC_ALL
} MFC_INT_FLAG;

typedef enum
{
	DECODING_DMB,
	DECODING_CONFORMANCE,
	ENCODING_WITH_ALL_PARAMETERS
} MFC_APP_TYPE;

#define MAX_DEC_FRAME      (16+2+2)    // AVC REF 16, ROT 2, Deblock 2
#define MAX_PROCESS_NUM    8
#define	STREAM_WR_SIZE     1024


typedef struct
{
    char SrcFileName[255];
	char GoldenYuvFileName[255];
	char GoldenStreamFileName[255];
    u32     NumOfFrame;
    u32     PicX;
    u32     PicY;
    u32     FrameRate;

    // MPEG4 ONLY
    int     VerId;
    int     DataPartEn;
    int     RevVlcEn;
    int     ShortVideoHeader;
    int     AnnexI;
    int     AnnexJ;
    int     AnnexK;
    int     AnnexT;
    int     IntraDcVlcThr;
    int     VopQuant;

    // H.264 ONLY
    int     ConstIntraPredFlag;
    int     DisableDeblk;
    int     DeblkOffsetA;
    int     DeblkOffsetB;
    int     ChromaQpOffset;
    int     PicQpY;

	u32     FmoEnable;
	u32     NumOfSliceGroup;
	u32     FmoType;

    // COMMON
    int     GopPicNum;
    int     SliceMode;
    int     SliceSizeMode;
    int     SliceSizeNum;

    int     IntraRefreshNum;

    // RC
    int     RcEnable;
    int     RcBitRate;
    int     RcInitDelay;
    int     RcBufSize;
}  ENC_CFG ;

typedef struct
{
	u32 m_uFrameAddrY[MAX_PROCESS_NUM][MAX_DEC_FRAME];
	u32 m_uFrameAddrCb[MAX_PROCESS_NUM][MAX_DEC_FRAME];
	u32 m_uFrameAddrCr[MAX_PROCESS_NUM][MAX_DEC_FRAME];
	u32 m_uLastFrameBufEndAddr[MAX_PROCESS_NUM];

	u32 m_uCodeBufEndAddr;
	u32 m_uParameterBufStartAddr;
	u32 m_uParameterBufEndAddr;
	u32 m_uWorkingBufEndAddr;	

	MFC_CODEC_MODE m_eCodecMode[MAX_PROCESS_NUM];
	u32 m_uPicX[MAX_PROCESS_NUM];
	u32 m_uPicY[MAX_PROCESS_NUM];
	u32 m_uStride[MAX_PROCESS_NUM];
	u32 m_uHeight[MAX_PROCESS_NUM];
	u32 m_uBitRdPtr[MAX_PROCESS_NUM];
	u32 m_uBitWrPtr[MAX_PROCESS_NUM];
	
	u32 m_uStreamBufStAddr[MAX_PROCESS_NUM];
	u32 m_uStreamBufEndAddr[MAX_PROCESS_NUM];
	u32 m_uStreamBufByteSize[MAX_PROCESS_NUM];	
	u8 m_bIsNoMoreStream[MAX_PROCESS_NUM];
	u32 m_uStreamPtr[MAX_PROCESS_NUM];	

	u32 m_uRefFrameNum[MAX_PROCESS_NUM];
	s32 m_uFrameIndex[MAX_PROCESS_NUM];
	u8 m_bMp4DecDeblkMode[MAX_PROCESS_NUM];
	u32 m_uMp4DeblockFrameIdx[MAX_PROCESS_NUM];
	u8 m_bAnnexJOn[MAX_PROCESS_NUM];
	u8 m_bDecRotEn[MAX_PROCESS_NUM];
	u32 m_uRotFrameIdx[MAX_PROCESS_NUM];
	u32 m_uFrameDelayCount[MAX_PROCESS_NUM];

	ENC_CFG m_EncCfg[MAX_PROCESS_NUM];
	u8 m_bMbBitReport[MAX_PROCESS_NUM];
	u8 m_bSliceInfoReport[MAX_PROCESS_NUM];
	u8 m_bAudEnable[MAX_PROCESS_NUM];
	u8 m_bSkipDisable[MAX_PROCESS_NUM];
} MFC;

void MFC_MoveFirmwareToCodeBuf(u32 uFreeBufAddr);
	
void MFC_InitBaseForProcesses(u32 uFreeBufAddr);
void MFC_InitBaseForProcesses1(u32 uFreeBufAddr, MFC_APP_TYPE eAppType);
void MFC_InitBaseForProcesses2(u32 uFreeBufAddr, u32 uProcessBufSize, 
	u32 uDecBuf1Size, u32 uDecBuf2Size, u32 uDecBuf3Size, u32 uDecBuf4Size, u32 uDecBuf5Size, 
	u32 uEncBuf1Size, u32 uEncBuf2Size, u32 uEncBuf3Size, u32 uEncBuf4Size);
	
void MFC_StartBitProcessor(void);
void MFC_StopBitProcessor(void);

void MFC_InitProcessForDecodingMpeg4(
	u32 uProcessIdx, u32 uStreamBufStAddr, u32 uStreamBufSize,
	u32 uFrameBufStAddr, u8 bDecRotEn, u8 bMp4DeblkEn);
void MFC_InitProcessForDecodingH264(
	u32 uProcessIdx, u32 uStreamBufStAddr, u32 uStreamBufSize,
	u32 uFrameBufStAddr, u8 bDecRotEn, u8 bH264ReorderEn);
void MFC_InitProcessForDecodingVc1(
	u32 uProcessIdx, u32 uStreamBufStAddr, u32 uStreamBufSize, u32 uFrameBufStAddr, u8 bDecRotEn);
void MFC_InitProcessForDmbDecoding(
	u32 uProcessIdx, u32 uStreamBufStAddr, u32 uStreamBufSize, u32 uFrameBufStAddr, u8 bDecRotEn);
void MFC_InitStreamBufferForOneFrameDecoding(u32 uProcessIdx,  u32 uStreamBufStAddr, u32 uStreamBufSize);
u8 MFC_DecodeOneFrame(u32 uProcessIdx, u32* uFrameIdx);
void MFC_StartDecodingOneFrame(u32 uProcessIdx);
u8 MFC_DoPostProcessingOfDecoding(u32 uRunIdx, u8 isEmpty, u32* uFrameIdx);
void MFC_InitProcessForOneFrameDecoding(
	u32 uProcessIdx, MFC_CODEC_MODE eCodecMode, u32 uStreamBufStAddr, u32 uStreamBufSize,
	u32 uFrameBufStAddr, u8 bDecRotEn, u8 bMp4DeblkEn, u8 bH264ReorderEn, u8 bFilePlayEn, u8 bDynBufAllocEn);
	
void MFC_InitProcessForEncoding(
	u32 uProcessIdx,
	MFC_CODEC_MODE eEncMode,
	u32 uStreamBufStAddr,
	u32 uStreamBufSize,
	u32 uDispWidth,
	u32 uDispHeight,
	u8 bEncRot90En);
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
	u8 bSkipDisable);

void MFC_EncodeOneFrame(u32 uProcessIdx, u32 uAddrOfSrcYFrame, u32* uRecFrameIdx);	
void MFC_StartEncodingOneFrame(u32 uProcessIdx, u32 uAddrOfSrcYFrame);
void MFC_DoPostProcessingOfEncoding(u32 uProcessIdx, u32* uRecFrameIdx);	

void MFC_IssueSeqEndCmd(u32 uProcessIdx);

// Helper functions
void MFC_GetDecSrcFormat(u32* uWidth, u32* uHeight, float* uFrameRate);
void MFC_GetEncSrcSize(u32* uWidth, u32* uHeight);
void MFC_GetMp4Info(u8* isDataPartEn, u8* isRevVlcEn, u8* isShortVideoHeader);
void MFC_GetDispFrameNum(u32 uRunIdx, u32* uDispFrameNum);
void MFC_GetFrameCount(u32* uFrameNum);
void MFC_GetCurPc(u32* uCurProgCnt);

void MFC_GetAddrOfYFrame(u32 uProcessIdx, u32 idx, u32* addrY);

void MFC_GetWrPtr(u32 uRunIdx, u32* uWrPtr);
void MFC_GetRdPtr(u32 uRunIdx, u32* uRdPtr);
void MFC_SetRdPtr(u32 uProcessIdx, u32 uRdPtr);
void MFC_SetWrPtr(u32 uProcessIdx, u32 uWrPtr);	
void MFC_GetWrPtr2(u32 uRunIdx, u32* uWrPtr);
void MFC_GetRdPtr2(u32 uRunIdx, u32* uRdPtr);
void MFC_SetWrPtr2(u32 uProcessIdx, u32 uWrPtr);
void MFC_SetRdPtr2(u32 uProcessIdx, u32 uRdPtr);
void MFC_ResetProgramCounterOfBitCode(void);
void MFC_ResetIp(void);
void MFC_GetCodecModeByName(const char* fileExt, u8 bIsDec, MFC_CODEC_MODE* eCodecMode);
void MFC_GetSrcPicSize(u32 uIdx, u32* uWidth, u32* uHeight);
void MFC_GetFrameSize(u32 uIdx, u32* uStride, u32* uHeight);
	
void MFC_SetEncYuvFileCfg(
	u32 uProcessIdx, const char* pFileName, u32 uNumOfFrame,
	u32 uWidth, u32 uHeight, u32 uFrameRate);
void MFC_SetEncGoldenFileCfg(u32 uProcessIdx, const char* pGoldenYuvName, const char* pGoldenStreamName);

void MFC_SetEncSliceCfg(u32 uProcessIdx, u32 uSliceMode, u32 uSliceSizeMode, u32 uSliceSizeNum);
void MFC_SetEncErrResilienceCfg(u32 uProcessIdx, u32 uIntraRefreshNum);
void MFC_SetEncMpeg4Cfg(
	u32 uProcessIdx, u32 uDataPartEn, u32 uRevVlcEn, u32 uIntraDcVlcThr, u32 uVopQuant, u32 uGopPicNum);
void MFC_SetEnc263Cfg(
	u32 uProcessIdx, u32 uAnnexI, u32 uAnnexJ, u32 uAnnexK, u32 uAnnexT, u32 uVopQuant, u32 uGopPicNum);
void MFC_SetEnc264Cfg(
	u32 uProcessIdx, u32 uConstIntraPredFlag, u32 uDisableDeblk,
	int uDeblkOffsetA, int uDeblkOffsetB, int uChromaQpOffset,
	u32 uPicQpY, u32 uGopPicNum);
void MFC_SetEnc264FmoCfg(u32 uProcessIdx, u8 bFmoEnable, u32 uFmoType, u32 uNumOfSliceGroup);
void MFC_SetEncRateCtrlCfg(u32 uProcessIdx, u32 uRcEnable, u32 uRcBitRate, u32 uRcInitDelay, u32 uRcBufSize);

void MFC_GetStreamBufEndAddr(u32 uProcessIdx, u32* uBitBufEndAddr);
void MFC_SetDecRotEn(u8 isRotEn);
void MFC_SetDecRotationMode(u8 horMirEn, u8 verMirEn, u32 rotDeg);
void MFC_SetDecRotationMode1(u8 decRotEn, u8 horMirEn, u8 verMirEn, u32 rotDeg);
void MFC_SetEncRotEn(u8 isRotEn);
void MFC_SetEncRotationMode(u8 horMirEn, u8 verMirEn, u32 rotDeg);
void MFC_SetEncRotationMode1(u8 encRotEn, u8 horMirEn, u8 verMirEn, u32 rotDeg);
void MFC_GetEncReconFrameIndex(s32* uEncPicFrmIdx);
void MFC_ClearPending(MFC_INT_FLAG eIntFlag);
void MFC_SetUnmask(MFC_INT_FLAG eIntFlag);
void MFC_SetMask(MFC_INT_FLAG eIntFlag);
void MFC_GetIntFlag(MFC_INT_FLAG* eIntFlag);
void MFC_ResetStreamBufInEncoding(void);
void MFC_SetFlushingStreamBufModeInEncoding(void);
void MFC_SetNoFlushingStreamBufModeInEncoding(void);
u8 MFC_IsMoreStreamNeeded(u32 uProcessIdx, u32 uSzOfCriterion);
void MFC_GetFrameDelayCount(u32 uProcessIdx, u32* uDelayCount);
void MFC_SetWordData(u32 uAddr, u32 uWordData);
void MFC_GetDecErrorMbNum(u32* uErrMbNum);
void MFC_IsEncIntraPic(u8* bIsIpicture);
u8 MFC_GetEncPicSliceNum(u32 uProcessIdx, u32* uSliceNum);
void MFC_GetEncMbBit(u32 uFreeMemStAddr, u32 uNumOfMbInWidth, u32 uNumOfMbInHeight);
void MFC_SetEncHeaderCode(MFC_ENC_HEADER_CODE eEncHeaderCode);
void MFC_GetEncHeaderCode(u32 uProcessIdx, MFC_ENC_HEADER_CODE* eEncHeaderCode);
void MFC_SetDecParaSetType(MFC_PARA_SET_TYPE eParaSetType);
void MFC_GetDecParaSetType(MFC_PARA_SET_TYPE* eParaSetType);
void MFC_SetDecParaSetSize(u32 uRbspBytes);
void MFC_SetEncParaSetType(MFC_PARA_SET_TYPE eParaSetType);
void MFC_GetEncParaSetType(MFC_PARA_SET_TYPE* eParaSetType);
void MFC_GetEncParaSetSize(u32* uRbspBytes);
void MFC_GetFirmwareVersion(u32* uVersion);
void MFC_NotifyNoMoreStream(u32 uProcessIdx);
void MFC_GetNoMoreStreamInputed(u32 uProcessIdx, u8* isEnd);
void MFC_SetBitstreamEndFlag(u8 isWholeStreamInBuf);
void MFC_GetBitstreamEnd(u8* isWholeStreamInBuf);
void MFC_SetWorkBufConfig(u8 isEnabled);
void MFC_GetWorkBufConfig(u8* isEnabled);

void MFC_SetDecRotStride(u32 uRotStride);
void MFC_SetEncRunOption(u8 bPicSkipEn, u8 bIdrPicEn);
void MFC_GetCodeBufEndAddr(u32* uEndAddr);
void MFC_GetBaseBufEndAddr(u32* uEndAddr);
void MFC_GetWorkingBufEndAddr(u32* uEndAddr);
void MFC_GetProcessBufEndAddr(u32 uProcessIdx, u32* uEndAddr);
void MFC_SetStreamPointer(u32 uProcessIdx, u32 uStreamPtr);
void MFC_GetStreamPointer(u32 uProcessIdx, u32* uStreamPtr);
void MFC_IssueCmd(u32 uProcessIdx, MFC_COMMAND runCmd);

void MFC_ParseConfigFileAndSetParameters(u32 uProcessIdx, u32 uConfigFileStAddr);
void MFC_GetCodecModeFromName(const char* pStreamFileName, u8 bIsDec, MFC_CODEC_MODE* eDecMode);
void MFC_GetGoldenStreamFileName(u32 uProcessIdx, char* pGoldenStreamName);
void MFC_GetGoldenYuvFileName(u32 uProcessIdx, char* pGoldenYuvName);
void MFC_GetSrcYuvFileName(u32 uProcessIdx, char* pSrcYuvName);
void MFC_GetEncPicSizeCfg(u32 uProcessIdx, u32* uPicWidth, u32* uPicHeight);
void MFC_GetEncNumOfFramesCfg(u32 uProcessIdx, u32* uNumOfFrames);
void MFC_GetEncFrameRateCfg(u32 uProcessIdx, u32* uFrameRate);
	
//protected:
void MFC_InitProcessForDecoding(
	u32 uProcessIdx, MFC_CODEC_MODE eCodecMode, u32 uStreamBufStAddr, u32 uStreamBufSize,
	u32 uFrameBufStAddr, u8 bDecRotEn, u8 bMp4DeblkEn, u8 bH264ReorderEn);	
void MFC_SetFrameBufEndianMode(u8 isLittleEndian);
void MFC_SetStreamBufControlOption(u8 isLittleEndian, u8 isFlowCheckEnable);
void MFC_SetUpWorkingBuf(u32 uFreeBufAddr);
void MFC_SetUpWorkingBuf1(u32 uFreeBufAddr, MFC_APP_TYPE eAppType);
void MFC_SetUpWorkingBuf2(
	u32 uFreeBufAddr, u32 uProcessBufSize, 
	u32 uDecBuf1Size, u32 uDecBuf2Size, u32 uDecBuf3Size, u32 uDecBuf4Size, u32 uDecBuf5Size, 
	u32 uEncBuf1Size, u32 uEncBuf2Size, u32 uEncBuf3Size, u32 uEncBuf4Size);
void MFC_SetParameterBufAddr(u32 uFreeBufAddr);

void MFC_IssueCmdOfSetFrameBuffer(u32 uProcessIdx, u32 uNumOfRefReconFrame, u32 uStride);
void MFC_InitEncSeq(
	u32 uProcessIdx,
	MFC_CODEC_MODE eEncMode,
	u32 uStreamBufStAddr,
	u32 uStreamBufSize,
	u8 bEncRot90En,
	u8 bMbBitReport,
	u8 bSliceInfoReport,
	u8 bAudEnable,
	u8 bSkipDisable);
void MFC_InitDecFrameBuffer(u32 uProcessIdx, u32 uFrameBufNum, u32 uStride, u32 uHeight, u32 uFrameBuf);
void MFC_InitEncFrameBuffer(u32 uProcessIdx, u32 uRefFrameNum, u32 uWidth, u32 uHeight, u32 uRefFrameBufStAddr);
void MFC_ClearFrameBuffer(u32 uProcessIdx, u32 uFrameIdx);
void MFC_SetFrameAddr(u32 uProcessIdx, u32 uFrameIdx, u32 uAddrY, u32 uAddrCb, u32 uAddrCr);

void MFC_WaitForReady(void);

void MFC_StartIssuingCmd(u32 uProcessIdx, MFC_COMMAND eCmd);
u8 MFC_IsCmdFinished(void);
	
void MFC_IsDecH263AnnexJOn(u32 uProcessIdx, u8* bAnnexJOn);
void MFC_GetDecRefFrameNum(u32 uProcessIdx, u32* uRefFrameNum);
void MFC_GetDecReconFrameIndex(u32 uProcessIdx, s32* uDecFrmIdx);

void MFC_SetEncGopNum(u32 uGopPicNum);
void MFC_SetEncCodStd(u32 uCodStd);
void MFC_SetEncSrcFormat(u32 uWidth, u32 uHeight, u32 uFrameRate);
void MFC_SetEncPicQs(MFC_CODEC_MODE eCodecMode, u32 uPicQuantStep);
void MFC_SetEnc264Parameter(
	int uChromaQpOffset, 
	u32 uConstIntraFlag, 
	u32 uDisableDeblk,
	int uDeblkAlphaOffset,
	int uDeblkBetaOffset);
void MFC_SetProcessBufStAddr(u32 uProcessBufStAddr);
void MFC_SetDecTempBuf1StAddr(u32 uTempBuf1StAddr);
void MFC_SetDecTempBuf2StAddr(u32 uTempBuf2StAddr);
void MFC_SetDecTempBuf3StAddr(u32 uTempBuf3StAddr);
void MFC_SetDecTempBuf4StAddr(u32 uTempBuf4StAddr);
void MFC_SetDecTempBuf5StAddr(u32 uTempBuf5StAddr);
void MFC_SetEncTempBuf1StAddr(u32 uTempBuf1StAddr);
void MFC_SetEncTempBuf2StAddr(u32 uTempBuf2StAddr);
void MFC_SetEncTempBuf3StAddr(u32 uTempBuf3StAddr);
void MFC_SetEncTempBuf4StAddr(u32 uTempBuf4StAddr);
	
void MFC_SetDeblockAddrY(u32 uDbkAddrY);
void MFC_SetDeblockAddrCb(u32 uDbkAddrCb);
void MFC_SetDeblockAddrCr(u32 uDbkAddrCr);

#endif  // __MFC_H__
