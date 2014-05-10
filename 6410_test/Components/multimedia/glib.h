#ifndef __GLIB_H__
#define __GLIB_H__

typedef enum
{
	C_BLACK=0, C_RED=1, C_GREEN=2, C_BLUE=3, C_WHITE=4,
	C_YELLOW=5, C_CYAN=6, C_MAGENTA=7
} COLOR;

typedef enum
{
	ALPHA_0, ALPHA_1
} ALPHA_SEL;

void GLIB_Init(u32 uFbAddr, u32 uHSize, u32 uVSize, CSPACE eBpp);
void GLIB_InitInstance(u32 uFbAddr, u32 uHSize, u32 uVSize, CSPACE eBpp);

void GLIB_PutLine(int x1, int y1, int x2, int y2, COLOR color);
void GLIB_PutLine2(int x1, int y1, int x2, int y2, u32 cvalue);
	
void GLIB_PutRectangle(int x1, int y1, int dx, int dy, COLOR color);
void GLIB_PutRectangle2(int x1, int y1, int dx, int dy, u32 cvalue);
void GLIB_FillRectangle(int x1, int y1, int dx, int dy, COLOR color);
void GLIB_FillRectangle2(int x1, int y1, int dx, int dy, u32 cvalue);
void GLIB_ClearFrame(COLOR color);
void GLIB_ClearFrame2(u32 cvalue);
void GLIB_PutPixel(u32 x, u32 y, COLOR color);
void GLIB_PutPixel2(u32 x, u32 y, u32 cvalue);

void GLIB_GetColorValue(COLOR color, u32* value);
void GLIB_Printf(u32 x, u32 y, const char *fmt, ...);
void GLIB_SetFontColor(COLOR fontColor, COLOR bgColor, u8 bIsBgTransparent);
void GLIB_PutString(u32 x, u32 y, char *str);

void GLIB_GetDimension(u32 *x, u32 *y);

void GLIB_MakeColor(u32 uRed, u32 uGreen, u32 uBlue, u32 *color); ///

void GLIB_SetAlpha(COLOR eColor, u32 uAlpha);
	
void GLIB_DrawPattern(u32 uWidth, u32 uHeight);
void GLIB_DrawPattern2(u32 uWidth, u32 uHeight);
	
void GLIB_SetPalDataFormat(CSPACE ePalDataFormat);
void GLIB_GetPalDataFormat(COLOR eColorIdx, u32 *uColorVal);

void GLIB_InitFont(void);
	
void GLIB_PutPalData(int nPosX, int nPosY, u32 uData);
void GLIB_ClearPalFrame(u32 uData);
void GLIB_PutPalLine(int nPosX1, int nPosY1, int nPosX2, int nPosY2, u32 uData);
void GLIB_utPalRectangle(int nPosX1, int nPosY1, int nHSz, int nVSz, u32 uData);
void GLIB_FillPalRectangle(int nPosX1, int nPosY1, int nHSz, int nVSz, u32 uData);
void GLIB_PutPalRectangle(int nPosX1, int nPosY1, int nHSz, int nVSz, u32 uData);

void GLIB_PutLine3(int nX1, int nY1, int nX2, int nY2, COLOR eColor, ALPHA_SEL eSel);
void GLIB_PutLine4(int nX1, int nY1, int nX2, int nY2, u32 uCValue, ALPHA_SEL eSel);
void GLIB_PutRectangle3(int x1, int y1, int dx, int dy, COLOR color, ALPHA_SEL eSel);
void GLIB_PutRectangle4(int x1, int y1, int dx, int dy, u32 cvalue, ALPHA_SEL eSel);
void GLIB_FillRectangle3(int x1, int y1, int dx, int dy, COLOR color, ALPHA_SEL eSel);
void GLIB_FillRectangle4(int x1, int y1, int dx, int dy, u32 cvalue, ALPHA_SEL eSel);
void GLIB_ClearFrame3(COLOR color, ALPHA_SEL eSel);
void GLIB_ClearFrame4(u32 cvalue, ALPHA_SEL eSel);

void GLIB_PutString2(u32 x, u32 y, char *str, ALPHA_SEL eSel);

typedef struct {
	u32  m_uHsz;
	u32  m_uVsz;
	u32  m_uBpp;
	void* m_pFrameBuf;
	u32  m_uFrameBuf;
	u32  m_uColorValue[256];

	u32 m_uPalDataBpp;
	u32 m_uPalFlag; // if uPalFlag == 1 then Palette, else Real Color
	u32 m_uCnt;
	u8 m_ucPreData;
	u8 m_uPrePosX, m_uPrePosY;
	CSPACE m_eCSpace;

	COLOR m_fontColor;
	COLOR m_bgColor;
	u8 m_bIsBgTransparent;
	u8*  m_pFont8x15;
} GLIB;

#endif //__GLIB_H__
