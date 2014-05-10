#ifndef __JPEG_H__
#define __JPEG_H__


typedef enum
{
	JPEG_420, JPEG_422, JPEG_444, JPEG_411, JPEG_400
} JPEG_TYPE;

typedef enum
{
	ERR_ENC_OR_DEC,OK_HD_PARSING=0x08,ERR_HD_PARSING1,ERR_HD_PARSING2,OK_ENC_OR_DEC=0x40,
} JPEG_IRQSTATUS;
// Check whether the processing is done normally for the one step decoding.

typedef enum
{
	JPEG_DONE=0x0, JPEG_RUNNING=0x1,
} JPEG_STATUS;


typedef enum
{
	HEADER, BODY, HEADER_N_BODY
} JPEG_DEC_MODE;

typedef struct
{
	u32 m_uJpgHSz, m_uJpgVSz;
	u32 m_uJpgConReg;

	u8 m_bIsHeaderParsed;
} JPEG;

void JPEG_Init(void);
void JPEG_Reset(void);
void JPEG_Wait_Done(void);

void JPEG_GetJpegType(char * pType);
void JPEG_GetStreamLen(u32* uSize);
void JPEG_GetWidthAndHeight(u32* hsz, u32* vsz);

void JPEG_ReadAndClearStatus(JPEG_STATUS* eStatus, JPEG_IRQSTATUS* eIrqStatus);
void JPEG_SetNextFrameStartAddr(u32 uAddr);


void JPEG_InitRegsForEncoding(	u32 uHSz, u32 uVsz, u32 uSrcAddr, CSPACE eRawType,
									u32 uDstAddr, JPEG_TYPE eJpgType, u8 bIsOnTheFly, u8 bIsMotion);
void JPEG_InitRegsForDecoding(	u32 uSrcAddr, u32 uDstAddr,
									JPEG_DEC_MODE eMode, u8 bIncremental, u8 bIsMotion);

void JPEG_StartParsingHeader(u32 uJpgAddr);
void JPEG_StartDecodingBody(u32 uRawAddr, u8 bIsIncremental);
void JPEG_StartDecodingOneFrame(u32 uJpgAddr, u32 uRawAddr, u8 bIsIncremental);
void JPEG_StartEncodingOneFrame(u16 usHSz, u16 usVSz, u32 uSrcAddr, CSPACE eRawType,
										u32 uDestAddr, JPEG_TYPE eJpgType);
	
#endif // __JPEG_H__
