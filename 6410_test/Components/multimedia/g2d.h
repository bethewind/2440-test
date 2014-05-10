/**************************************************************************************
* 
*	Project Name : S3C6400 Validation
*
*	Copyright 2006 by Samsung Electronics, Inc.
*	All rights reserved.
*
*	Project Description :
*		This software is only for validating functions of the S3C6400.
*		Anybody can use this software without our permission.
*  
*--------------------------------------------------------------------------------------
* 
*	File Name : g2d.h
*  
*	File Description : This is header file for g2d.c
*
*	Author : Sekwang Kim
*	Dept. : AP Development Team
*	Created Date : 2007/01/25
*	Version : 0.1a 
* 
*	History
*	- Created by Jang JiHyun, Lee YunSoo
* 	- Modified by Sekwang Kim(2007/01/25)
*  
**************************************************************************************/


#ifndef __G2D_H__
#define __G2D_H__

#ifdef __cplusplus
extern "C" {
#endif

//#define PROFILE
//#define MSGPRT
//#define WITHOUT
//#define WITHROTATION
//#define WITHBLENDING
#define WITHALL
//#define DATACOMP

#define INTERRUPT_MODE

#define G2D_MAX_WIDTH				(2040)
#define G2D_MAX_HEIGHT				(2040)

#define G2D_ROTATION_0_DEG_BIT	        (0x1<<0)
#define G2D_ROTATION_90_DEG_BIT	        (0x1<<1)
#define G2D_ROTATION_180_DEG_BIT        (0x1<<2)
#define G2D_ROTATION_270_DEG_BIT        (0x1<<3)

#define G2D_NO_FLIP                     (0x1<<0)
#define G2D_X_FLIP                      (0x1<<4)
#define G2D_Y_FLIP                      (0x1<<5)

// ROP_REG (0x410)
#define G2D_OPERAND3_PAT_BIT            (0<<13)
#define G2D_OPERAND3_FG_BIT             (1<<13)

#define G2D_TRANSPARENT_BIT		(1<<9)
#define G2D_OPAQUE_BIT			(0<<9)

#define G2D_NO_ALPHA_BIT                (0<<10)
#define G2D_PP_ALPHA_SOURCE_BIT         (1<<10)
#define G2D_ALPHA_BIT                   (2<<10)
//#define G2D_PP_ALPHA_THIRD_BIT          (3<<10)
#define G2D_FADING_BIT                  (4<<10)

#define G2D_ROP_SRC_ONLY                (0xf0)
#define G2D_ROP_3RD_OPRND_ONLY          (0xaa)
#define G2D_ROP_DST_ONLY			(0xcc)
#define G2D_ROP_SRC_OR_DST              (0xfc)
#define G2D_ROP_SRC_OR_3RD_OPRND        (0xfa)
#define G2D_ROP_SRC_AND_DST             (0xc0) //(pat==1)? src:dst
#define G2D_ROP_SRC_AND_3RD_OPRND       (0xa0)
#define G2D_ROP_SRC_XOR_3RD_OPRND       (0x5a)
#define G2D_ROP_DST_OR_3RD_OPRND       (0xee)



#define G2D_COLOR_YUV_NARROW		(0x1<<4)
#define G2D_COLOR_YUV_WIDE			(0x0<<4)

#define G2D_COLOR_YUV_MODE			(0x1<<3)
#define G2D_COLOR_RGB_MODE			(0x0<<3)

#define G2D_COLOR_RGB_565           (0x0<<0)
#define G2D_COLOR_RGBA_5551         (0x1<<0)
#define G2D_COLOR_ARGB_1555         (0x2<<0)
#define G2D_COLOR_RGBA_8888         (0x3<<0)
#define G2D_COLOR_ARGB_8888         (0x4<<0)
#define G2D_COLOR_XRGB_8888         (0x5<<0)
#define G2D_COLOR_RGBX_8888         (0x6<<0)

// CMD0_REG (Line) (0x100)
#define G2D_REND_POINT_BIT              (1<<0)
#define G2D_REND_LINE_BIT               (1<<1)
#define G2D_MAJOR_COORD_X_BIT           (1<<8)
#define G2D_MAJOR_COORD_Y_BIT           (0<<8)
#define G2D_NOT_DRAW_LAST_POINT_BIT     (1<<9)
#define G2D_DRAW_LAST_POINT_BIT         ~(0<<9)

// CMD1_REG (BitBlt) (0x104)
#define G2D_STRETCH_BITBLT_BIT          (1<<1)
#define G2D_NORMAL_BITBLT_BIT           (1<<0)

#define ABS(v)                          (((v)>=0) ? (v):(-(v)))
#define START_ASCII                     (0x20)
#define FIFO_NUM			32
#define OPAQUE_ENABLE                   (0<<9)

// Set fading and alpha value
#define FADING_OFFSET_DISABLE           (0x0<<8)
#define ALPHA_VALUE_DISABLE             (0xff<<0)

#define G2D_DE_STATUS_FA_BIT            (1<<9)

#define FONT8BY8			(0)
#define FONT8BY15			(1)
#define FONT8BY16T			(2)
#define FONT8BY16			(3)

#define HOST2SCREEN			(0)
#define SCREEN2SCREEN		(1)

#define G2D_INT_CURR_CMD		(1<<10)
#define G2D_INT_ALL_CMD			(1<<9)
#define G2D_INT_OVERFLOW		(1<<8)
#define G2D_INT_FIFO_LEVEL		(1<<0)

#define SRCBUF				0
#define DSTBUF				1


typedef enum
{
	ROP_DST_ONLY,
	ROP_SRC_ONLY, 
	ROP_3RD_OPRND_ONLY,
	ROP_SRC_AND_DST,
	ROP_SRC_AND_3RD_OPRND,
	ROP_SRC_OR_DST,
	ROP_SRC_OR_3RD_OPRND,	
	ROP_DST_OR_3RD,
	ROP_SRC_XOR_3RD_OPRND

} G2D_ROP_TYPE;

typedef enum
{
	G2D_OPERAND3_PAT, 
	G2D_OPERAND3_FG 
} G2D_OPERAND3;

typedef enum
{
	G2D_NO_ALPHA_MODE,
	G2D_PP_ALPHA_SOURCE_MODE,
	G2D_ALPHA_MODE,
	G2D_FADING_MODE
} G2D_ALPHA_BLENDING_MODE;

typedef enum
{
	G2D_BLACK = 0, G2D_RED = 1, G2D_GREEN = 2, G2D_BLUE = 3, G2D_WHITE = 4, 
	G2D_YELLOW = 5, G2D_CYAN = 6, G2D_MAGENTA = 7
} G2D_COLOR;

typedef struct
{
	u32  m_uFbStAddr;
	u32  m_uSrcStAddr; //new in v2.0
	u32  m_uHoriRes; // Source Horizontal Resolution
	u32  m_uVertRes; // Source Vertical Resolution //new in v2.0

	//  Max Window Size of clipping window
	u32  m_uMaxDx;
	u32  m_uMaxDy;

	// Coordinate (X, Y) of clipping window
	u32  m_uCwX1, m_uCwY1;
	u32  m_uCwX2, m_uCwY2;

	u32  m_uFgColor;
	u32  m_uBgColor;
	u32  m_uBlueScreenColor;
	u32  m_uColorVal[8];

	// Reference to Raster operation
	u32  m_uRopVal; // Raster operation value
	u32  m_uAlphaBlendMode;
	u32  m_uTransparentMode;
	u32  m_u3rdOprndSel;

	// Reference to alpha value
	u32  m_uFadingOffsetVal;
	u32  m_uAlphaVal;

	// Reference to image rotation
	u32  m_uRotOrgX, m_uRotOrgY;
	u32  m_uRotAngle;

	// reference to pattern of bitblt
	u32  m_uPatternOffsetX, m_uPatternOffsetY;

	u32  m_uBytes;

	u8   m_ucAlphaVal;
	u8 m_bIsAlphaCall;
	
	// true: BitBlt enable in Host-To-Screen Font Drawing
	// false: BitBlt disable in Host-To-Screen Font Drawing	
	u8 m_bIsBitBlt;

	u32 m_uFontAddr;
	u8 m_bIsScr2Scr;
	
	CSPACE m_eBpp;

	// N_24X24, B_24X24, N_16X16, T_8X16, N_8X8, N_8X15
	u8*  m_upFontType;
	u32  m_uFontWidth, m_uFontHeight;
} G2D;

extern volatile u8 bG2dDone;
extern volatile unsigned uIntCnt;

void G2D_InitIp(u32 uFbStAddr, CSPACE eBpp);
void G2D_InitIp1(u32 uFbStAddr, CSPACE eBpp, u32 uCwMaxHSz, u32 uCwMaxVSz);
void G2D_InitIp2(u32 uFbStAddr, CSPACE eBpp, u32 uCwMaxHSz, u32 uCwMaxVSz,
	u32 uX1_Cw, u32 uY1_Cw, u32 uX2_Cw, u32 uY2_Cw);

void G2D_Reset(void);
void G2D_WaitForIdleStatus(void);

void G2D_SetRopEtype(G2D_ROP_TYPE eRopType);
void G2D_SetRop(u32 uRopVal);

void G2D_SetRotationOrgX(u16 usRotOrgX);
void G2D_SetRotationOrgY(u16 usRotOrgY);	

void G2D_SetAlphaMode(G2D_ALPHA_BLENDING_MODE eMode);		
void G2D_SetAlphaValue(u8 ucAlphaVal);
void G2D_SetFadingValue(u8 ucFadeVal);

void G2D_EnablePlaneAlphaBlending(u8 ucAlphaVal);
void G2D_DisablePlaneAlphaBlending(void);

void G2D_EnablePixelAlphaBlending(void); // Only Support 24bpp 
void G2D_DisablePixelAlphaBlending(void); // Only Support 24bpp 

void G2D_EnableFadding(u8 ucFadingVal);
void G2D_DisableFadding(void);
	
void G2D_ClearFrameEcolor(G2D_COLOR eColor, u32 uPosX, u32 uPosY);	
void G2D_ClearFrame(u32 uColor, u32 uPosX, u32 uPosY);

void G2D_PutPixelEcolor(u32 uPosX, u32 uPosY, G2D_COLOR eColor);
void G2D_PutPixel(u32 uPosX, u32 uPosY, u32 uColor);

void G2D_PutLineEcolor(u16 uPosX1, u16 uPosY1, u16 uPosX2, u16 uPosY2, G2D_COLOR eColor, u8 bIsDrawLastPoint);
void G2D_PutLine(u16 uPosX1, u16 uPosY1, u16 uPosX2, u16 uPosY2, u32 uColor, u8 bIsDrawLastPoint);

void G2D_PutRectangleEcolor(u32 uStPosX, u32 uStPosY, u32 uWidth, u32 uHeight, G2D_COLOR eColor);
void G2D_PutRectangle(u32 uStPosX, u32 uStPosY, u32 uWidth, u32 uHeight, u32 uColor);

void G2D_FillRectangleEcolor(u32 uStPosX, u32 uStPosY, u32 uWidth, u32 uHeight, G2D_COLOR eColor);
void G2D_FillRectangle(u32 uStPosX, u32 uStPosY, u32 uWidth, u32 uHeight, u32 uColor);

void G2D_DrawPattern(void);
void G2D_DrawPattern1(u32 uPosX, u32 uPosY, u32 uHsz, u32 uVsz);

void G2D_BitBlt(u16 uSrcStX, u16 uSrcStY, u16 uSrcEndX, u16 uSrcEndY,
	u16 uDstStX, u16 uDstStY, u16 uDstEndX, u16 uDstEndY, u8 bIsStretch);

void G2D_RotateImage(u16 usSrcX1, u16 usSrcY1, u16 usSrcX2, u16 usSrcY2,
	u16 usDestX1, u16 usDestY1, ROT_DEG eRotDegree);

void G2D_RotateWithBitBlt(u16 usSrcX1, u16 usSrcY1, u16 usSrcX2, u16 usSrcY2,
	u16 usDestX1, u16 usDestY1, ROT_DEG eRotDegree);

void G2D_SetTransparentMode(u8 bIsTransparent, G2D_COLOR eBsColor);
void G2D_SetTransparentMode1(u8 bIsTransparent, u32 uBsColor);

void G2D_SetColorKeyOn(G2D_COLOR eColor);
void G2D_SetColorKeyOn1(u32 uColorKey);
void G2D_SetColorKeyOff(void);

void G2D_RegisterPattern(u16* uspPattern);
void G2D_SetPatternOffset(u8 ucPatOffsetX, u8 ucPatOffsetY);
void G2D_SetPatternOffsetX(u8 ucPatOffsetX);
void G2D_SetPatternOffsetY(u8 ucPatOffsetY);

void G2D_Set3rdOperand(G2D_OPERAND3 e3rdOp);
void G2D_SetFgEcolor(G2D_COLOR eFgColor);
void G2D_SetFgColor(u32 uFgColor);
void G2D_SetBgEcolor(G2D_COLOR eBgColor);
void G2D_SetBgColor(u32 uBgColor);
void G2D_SetFbAddr(u32 uFbAddr, u32 uWhichBuffer);
void G2D_SetBppMode(CSPACE eBpp, u32 uWhichBuffer);



// Reference to font
void G2D_SetFontColor(G2D_COLOR eFontColor, G2D_COLOR eBgColor, u8 bIsTransparent);
void G2D_SetFontColor1(u32 uFontColor, u32 uBgColor, u8 bIsTransparent);
void G2D_Printf(u32 uPosX, u32 uPosY, const char* cpFmt, ...);

//protected:
	
void G2D_InitRegs(CSPACE eBpp);
void G2D_SetXYIncrFormat(u32 uDividend, u32 uDivisor, u32* uResult);
void G2D_CheckFifo(u32 uEmptyFifo);	
	
void G2D_GetRotationOrgXY(u16 usSrcX1, u16 usSrcY1, u16 usSrcX2, u16 usSrcY2,
	u16 usDestX1, u16 usDestY1, ROT_DEG eRotDegree, u16* usOrgX, u16* usOrgY);

void G2D_PutString(u32 uPosX, u32 uPosY, char* cStr);

void G2D_DisableEffect(void);


//Inserted or Modified by Boaz.kim//

void G2D_Init(void);
void G2D_InitSetting(u32 uFbStAddr, CSPACE eBpp, u32 uCwMaxHSz, u32 uCwMaxVSz, u32 uX1_Cw, u32 uY1_Cw, u32 uX2_Cw, u32 uY2_Cw);



void G2D_SetRotationMode(u32 uRotationType);
void G2D_SetRotationOrg(u16 usRotOrgX, u16 usRotOrgY);
void G2D_SetClippingWindow(u32 uLeftTopX, u32 uLeftTopY, u32 uRightBottomX, u32 uRightBottomY);
void G2D_SetScreenSize(u32 uMaxX, u32 uMaxY);
void G2D_SetPatternMemory(u32 uCoordX, u32 uCoordY, u16 usColor);
void G2D_GetPatternMemory(u32 uCoordX, u32 uCoordY, u16 *usColor);
void G2D_SetCoordinateDrawingText(u32 uStartX, u32 uStartY, u32 uEndX, u32 uEndY);
void G2D_SetFirstText(u32 uFirstText);
void G2D_SetNextText(u32 uNextText);
void G2D_SetCMDR7(u32 uTestDrawingFbAddr);
void G2D_SetFontType(u32 uFontType, u32 *uFontWidth, u32 *uFontHeight);
void G2D_FillMemoryForTextDrawing(u32 uFontType, u32 uTextDrawingFbAddr);
void G2D_SetCoordinateSrcBlock(u32 uStartX, u32 uStartY, u32 uEndX, u32 uEndY);
void G2D_SetCoordinateDstBlock(u32 uStartX, u32 uStartY, u32 uEndX, u32 uEndY);
void G2D_SetFirstBitBLTData(u32 uFirstData);
void G2D_SetNextBitBLTData(u32 uNextData);
void G2D_SetFirstBitBLTEData(G2D_COLOR eFirstData);
void G2D_SetNextBitBLTEData(G2D_COLOR eNextData);
void G2D_SetStencilKey(u32 uIsInvOn, u32 uIsAOn, u32 uIsROn, u32 uIsGOn, u32 uIsBOn);
void G2D_SetStencilMinMax(u32 uAMin, u32 uAMax, u32 uRMin, u32 uRMax, u32 uGMin, u32 uGMax, u32 uBMin, u32 uBMax, CSPACE eBpp);
void G2D_GetFrmSz(u32 *uG2dWidth, u32 *uG2dHeight);
void G2D_GetFbAddr(u32 *uG2dFbAddr);

//void G2D_GetBppMode(CSPACE *eBpp);
void G2D_GetSrcBppMode(CSPACE *eBpp); //New in v2.0
void G2D_GetDestBppMode(CSPACE *eBpp); //New in v2.0

void G2D_SetColorExpansionMethod(u8 bIsScr2Scr);
void G2D_SetHost2ScrBitBLT(u8 bIsHost2Scr);
void G2D_SetFontAddr(u32 uFontAddr);
void G2D_InterruptEnable(void);
void G2D_InterruptDisable(void);
void G2D_IntEnSelectable(u32 uWhichInt);
void G2D_IntDisSelectable(u32 uWhichInt);

//void __irq Isr_G2d(void);
//void __irq Isr_G2dProfile(void);
void __irq Isr_G2d(void);
void __irq Isr_G2dProfile(void);
void G2D_DrawImage(CSPACE eBpp);
void G2D_RopOperation(u8 bSrcData, u8 bDstData, u8 bThirdData, u8 ucRopVal, u8 *bOut);
void G2D_RopOut(u32 uSrcData, u32 uDstData, u32 uThirdData, u8 ucRopVal, CSPACE eBpp, u32 *uRopOut);
void G2D_ConvertEcolorToRaw(G2D_COLOR eColor, u32 *uRawColor) ;
void G2D_GetFbData(u32 uX, u32 uY, u32 uG2dWidth, u32 uG2dFbAddr, CSPACE eBpp, u32 *uFbData);
void G2D_SetFbData(u32 uX, u32 uY, u32 uG2dWidth, u32 uG2dFbAddr, CSPACE eBpp, u32 uFbData);
void G2D_GetPatternOffset(u8 *ucPatOffsetX, u8 *ucPatOffsetY);
void G2D_ConvertPatternToRaw(u16 uThirdDataOrg, CSPACE eBpp, u32 *uThirdDataConverted);
void G2D_BlendingOut(u32 uSrcData, u32 uDstData, u8 ucAlphaVal, u8 bFading, u8 ucFadingOffset, u32 *uBlendingOut);
void G2d_Convert24bpp(u32 uSrcData, CSPACE eBpp, u8 bSwap, u32 *uConvertedData);
void G2D_SetEndian(u8 bEndian);
void G2D_GetRotateCoordinate(u32 uDstX, u32 uDstY, u32 uOrgX, u32 uOrgY, u32 uRType, u32 *uRsltX, u32 *uRsltY);
void G2D_SetFifoIntLevel(u8 ucFifoIntLevel);


void G2D_SetSourceSize(u32 uHori, u32 uVert);


//void G2D_IntEnableForDeEngineFinish(void);
//void G2D_IntEnableForCmdFinish(void);
//void G2D_IntEnableForOverflow(u8 bFifo, u8 ucFifoLevel);	
//void G2D_InterruptDisAll(void);
void G2D_PutPixelForInterrupt(u32 uPosX, u32 uPosY, u32 uColor);
void G2D_PutPixelForInterruptHard(u32 uG2dWidth, u32 uG2dHeight, u32 uColor);
//void __irq Isr_G2dInt(void);
void Isr_G2dInt(void);
void G2D_WaitForInterrupt(void);









#ifdef __cplusplus
}
#endif

#endif /*__G2D_H__*/
