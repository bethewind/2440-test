/**
 * glib.cpp
 *
 * @Person Involved dharma			
 * @version         1.0
 */

#include <stdarg.h>
#include <string.h>
#include <stdio.h>

#include "system.h"
#include "library.h"
#include "glib.h"
#include "glib_font.h"

#define CC24_16(c)	         ( ((((c)>>19)&0x1f)<<11)|((((c)>>10)&0x3f)<< 5)|((((c)>>3)&0x1f)<<0) )

#define START_ASCII           (0x20)
#define FONT_START_LINE       (2)
#define FONT_END_LINE         (14)

#define ARGB8_ALPHA_MASK     ~(1<<7)
#define ARGB16_ALPHA_MASK    ~(1<<15)
#define ARGB24_ALPHA_MASK    ~(1<<24)

GLIB oGlib;

void GLIB_Init(u32 uFbAddr, u32 uHSize, u32 uVSize, CSPACE eBpp)
{
	GLIB_InitInstance(uFbAddr, uHSize, uVSize, eBpp);
}

void GLIB_ClearFrame(COLOR color) 
{ 
	if (oGlib.m_uPalFlag == 0)
		GLIB_ClearFrame2(oGlib.m_uColorValue[color]); 
	else
		GLIB_ClearPalFrame(color);
}

void GLIB_ClearFrame3(COLOR color, ALPHA_SEL eSel)
{ 
	GLIB_ClearFrame4(oGlib.m_uColorValue[color], eSel); 
}

void GLIB_GetColorValue(COLOR color, u32 *value)
{ 
	*value =  oGlib.m_uColorValue[color]; 
}

void GLIB_PutPixel(u32 x, u32 y, COLOR color) 
{
	if (oGlib.m_uPalFlag == 0)
		GLIB_PutPixel2(x, y, oGlib.m_uColorValue[color]); 
	else
		GLIB_PutPalData(x, y, color);
}

void GLIB_PutLine(int x1, int y1, int x2, int y2, COLOR color) 
{
	if (oGlib.m_uPalFlag == 0)
		GLIB_PutLine2(x1, y1, x2, y2, oGlib.m_uColorValue[color]);
	else
		GLIB_PutPalLine(x1, y1, x2, y2, color);
}

void GLIB_PutLine3(int nX1, int nY1, int nX2, int nY2, COLOR eColor, ALPHA_SEL eSel)
{
	GLIB_PutLine4(nX1, nY1, nX2, nY2, oGlib.m_uColorValue[eColor], eSel);
}

void GLIB_PutRectangle(int x1, int y1, int dx, int dy, COLOR color) 
{ 
	if (oGlib.m_uPalFlag == 0)
		GLIB_PutRectangle2(x1, y1, dx, dy, oGlib.m_uColorValue[color]); 
	else
		GLIB_PutPalRectangle(x1, y1, dx, dy, color); 
}

void GLIB_PutRectangle3(int x1, int y1, int dx, int dy, COLOR color, ALPHA_SEL eSel) 
{
	GLIB_PutRectangle4(x1, y1, dx, dy, oGlib.m_uColorValue[color], eSel); 	
}

void GLIB_FillRectangle(int x1, int y1, int dx, int dy, COLOR color)
{
	if (oGlib.m_uPalFlag == 0)
		GLIB_FillRectangle2(x1, y1, dx, dy, oGlib.m_uColorValue[color]);
	else
		GLIB_FillPalRectangle(x1, y1, dx, dy, color);		
}

void GLIB_FillRectangle3(int x1, int y1, int dx, int dy, COLOR color, ALPHA_SEL eSel)
{
	GLIB_FillRectangle4(x1, y1, dx, dy, oGlib.m_uColorValue[color], eSel);	
}

void GLIB_GetDimension(u32 *x, u32 *y)
{
	*x = oGlib.m_uHsz;
	*y = oGlib.m_uVsz;
}

void GLIB_InitInstance(u32 uFbAddr, u32 uHSize, u32 uVSize, CSPACE eBpp)
{
	oGlib.m_uHsz = uHSize;
	oGlib.m_uVsz = uVSize;
	oGlib.m_uBpp =
		(eBpp == RGB8 || eBpp == ARGB8) ? 8 :
		(eBpp == RGB16 || eBpp == ARGB16) ? 16 :
		(eBpp == RGB24 || eBpp == ARGB24) ? 24 : 24;

	oGlib.m_uPalDataBpp =
		(eBpp == PAL1) ? 1 :
		(eBpp == PAL2) ? 2 :
		(eBpp == PAL4) ? 4 :
		(eBpp == PAL8) ? 8 : 8;	

	oGlib.m_eCSpace = eBpp;
			
	oGlib.m_pFrameBuf = (void*)uFbAddr;
	oGlib.m_uFrameBuf = uFbAddr; /// added in GJ

	oGlib.m_uPalFlag = 0;
	oGlib.m_uCnt = 0;
	oGlib.m_ucPreData = 0;

	oGlib.m_uPrePosX = 0;
	oGlib.m_uPrePosY = 0;
	
	if (eBpp == RGB8)
	{
		oGlib.m_uColorValue[C_BLACK] = 0x00;
		oGlib.m_uColorValue[C_RED]   = 0x07<<5;
		oGlib.m_uColorValue[C_GREEN] = 0x07<<2;
		oGlib.m_uColorValue[C_BLUE]  = 0x03;
		oGlib.m_uColorValue[C_WHITE] = 0xff;
	}
	// A:1 - R:2 - G:3 - B:2
	// A's init value: 0 (A is Alpha value select bit)
	// if A is 0 then alpha_0 value of LCDC(above V3.2) is selected, else alpha_1 value is selected
	else if (eBpp == ARGB8) 
	{
		oGlib.m_uColorValue[C_BLACK] = 0x00;
		oGlib.m_uColorValue[C_RED] = 0x3<<5;
		oGlib.m_uColorValue[C_GREEN] = 0x7<<2;
		oGlib.m_uColorValue[C_BLUE] = 0x3;
		oGlib.m_uColorValue[C_WHITE] = 0x7f;
	}
	else if (eBpp == RGB16)
	{
		oGlib.m_uColorValue[C_BLACK] = 0x0;
		oGlib.m_uColorValue[C_RED]   = 0x1f<<11;
		oGlib.m_uColorValue[C_GREEN] = 0x3f<<5;
		oGlib.m_uColorValue[C_BLUE]  = 0x1f<<0;
		oGlib.m_uColorValue[C_WHITE] = 0xffff;
	}
	else if (eBpp == ARGB16) // A:1 - R:5 - G:5 - B:5 and A's init value: 0
	{
		oGlib.m_uColorValue[C_BLACK] = 0x0;
		oGlib.m_uColorValue[C_RED]   = 0x1f<<10;
		oGlib.m_uColorValue[C_GREEN] = 0x1f<<5;
		oGlib.m_uColorValue[C_BLUE]  = 0x1f<<0;
		oGlib.m_uColorValue[C_WHITE] = 0x7fff;
	}
	else if (eBpp == RGB24)
	{
		oGlib.m_uColorValue[C_BLACK] = 0x0;
		oGlib.m_uColorValue[C_RED]   = 0xff0000;
		oGlib.m_uColorValue[C_GREEN] = 0xff00;
		oGlib.m_uColorValue[C_BLUE]  = 0xff;
		oGlib.m_uColorValue[C_WHITE] = 0xffffff;
	}
	else if (eBpp == ARGB24) // A:1 - R:8 - G:8 - B:7 and A's init value: 0
	{
		oGlib.m_uColorValue[C_BLACK] = 0x0;
		oGlib.m_uColorValue[C_RED]   = 0xff<<15;
		oGlib.m_uColorValue[C_GREEN] = 0xff<<7;
		oGlib.m_uColorValue[C_BLUE]  = 0x7f;
		oGlib.m_uColorValue[C_WHITE] = 0x7fffff;
	}
	
	oGlib.m_uColorValue[C_YELLOW] = (oGlib.m_uColorValue[C_RED] | oGlib.m_uColorValue[C_GREEN]);
	oGlib.m_uColorValue[C_CYAN] = (oGlib.m_uColorValue[C_GREEN] | oGlib.m_uColorValue[C_BLUE]);
	oGlib.m_uColorValue[C_MAGENTA] = (oGlib.m_uColorValue[C_RED] | oGlib.m_uColorValue[C_BLUE]);		

	GLIB_InitFont();
}

void GLIB_SetPalDataFormat(CSPACE ePalDataFormat)
{
	oGlib.m_uPalFlag = 1;
	
	if (oGlib.m_uPalDataBpp == 1)
	{
		if(ePalDataFormat == RGB8)
		{
			oGlib.m_uColorValue[C_BLACK] = 0x00;
			oGlib.m_uColorValue[C_RED]   = 0x07<<5;
		}
		else if(ePalDataFormat == RGB16)
		{
			oGlib.m_uColorValue[C_BLACK] = 0x0;
			oGlib.m_uColorValue[C_RED]   = 0x1f<<11;			
		}
		else if(ePalDataFormat == RGB24)
		{
			oGlib.m_uColorValue[C_BLACK] = 0x0;
			oGlib.m_uColorValue[C_RED] = 0xff0000;					
		}
		else
		{
			Assert(0);
		}
	}
	else if (oGlib.m_uPalDataBpp == 2)
	{
		if(ePalDataFormat == RGB8)
		{
			oGlib.m_uColorValue[C_BLACK] = 0x00;
			oGlib.m_uColorValue[C_RED]   = 0x07<<5;
			oGlib.m_uColorValue[C_GREEN] = 0x07<<2;
			oGlib.m_uColorValue[C_BLUE]  = 0x03;			
		}
		else if(ePalDataFormat == RGB16)
		{
			oGlib.m_uColorValue[C_BLACK] = 0x0;
			oGlib.m_uColorValue[C_RED]   = 0x1f<<11;
			oGlib.m_uColorValue[C_GREEN] = 0x3f<<5;
			oGlib.m_uColorValue[C_BLUE]  = 0x1f<<0;
		}
		else if(ePalDataFormat == RGB24)
		{
			oGlib.m_uColorValue[C_BLACK] = 0x0;
			oGlib.m_uColorValue[C_RED]   = 0xff0000;
			oGlib.m_uColorValue[C_GREEN] = 0xff00;
			oGlib.m_uColorValue[C_BLUE]  = 0xff;
		}
		else
		{
			Assert(0);
		}
		
	}
	else if ( (oGlib.m_uPalDataBpp == 4) || (oGlib.m_uPalDataBpp == 8) )
	{
		if(ePalDataFormat == RGB8)
		{
			oGlib.m_uColorValue[C_BLACK] = 0x00;
			oGlib.m_uColorValue[C_RED]   = 0x07<<5;
			oGlib.m_uColorValue[C_GREEN] = 0x07<<2;
			oGlib.m_uColorValue[C_BLUE]  = 0x03;
			oGlib.m_uColorValue[C_WHITE] = 0xff;
		}
		else if(ePalDataFormat == RGB16)
		{
			oGlib.m_uColorValue[C_BLACK] = 0x0;
			oGlib.m_uColorValue[C_RED]   = 0x1f<<11;
			oGlib.m_uColorValue[C_GREEN] = 0x3f<<5;
			oGlib.m_uColorValue[C_BLUE]  = 0x1f<<0;
			oGlib.m_uColorValue[C_WHITE] = 0xffff;
		}
		else if(ePalDataFormat == RGB24)
		{
			oGlib.m_uColorValue[C_BLACK] = 0x0;
			oGlib.m_uColorValue[C_RED]   = 0xff0000;
			oGlib.m_uColorValue[C_GREEN] = 0xff00;
			oGlib.m_uColorValue[C_BLUE]  = 0xff;
			oGlib.m_uColorValue[C_WHITE] = 0xffffff;
		}
		else
		{
			Assert(0);
		}
	}
	else
	{
		Assert(0);
	}

	oGlib.m_uColorValue[C_YELLOW] = (oGlib.m_uColorValue[C_RED] | oGlib.m_uColorValue[C_GREEN]);
	oGlib.m_uColorValue[C_CYAN] = (oGlib.m_uColorValue[C_GREEN] | oGlib.m_uColorValue[C_BLUE]);
	oGlib.m_uColorValue[C_MAGENTA] = (oGlib.m_uColorValue[C_RED] | oGlib.m_uColorValue[C_BLUE]);		
	
}

void GLIB_GetPalDataFormat(COLOR eColorIdx, u32 *uColorVal)
{
	*uColorVal = oGlib.m_uColorValue[eColorIdx];
}

void GLIB_PutPixel2(u32 x, u32 y, u32 cvalue)
{
	if (oGlib.m_uBpp == 8)
	{
		*((u8 *)(oGlib.m_pFrameBuf)+oGlib.m_uHsz*(y)+(x)) = (u8)cvalue;
	}
	else if (oGlib.m_uBpp == 16)
	{
		*((u16 *)(oGlib.m_pFrameBuf)+oGlib.m_uHsz*(y)+(x)) = (u16)cvalue;

	}
	else if (oGlib.m_uBpp == 18 || oGlib.m_uBpp == 24)
	{
		*((u32 *)(oGlib.m_pFrameBuf)+oGlib.m_uHsz*(y)+(x)) = cvalue;
	}
}

void GLIB_PutPalData(int nPosX, int nPosY, u32 uData)
{
	if (oGlib.m_uPalDataBpp == 8)
	{
		*((u8 *)(oGlib.m_pFrameBuf)+oGlib.m_uHsz*(nPosY)+(nPosX)) = (u8)uData;		
	}
	else if(oGlib.m_uPalDataBpp != 8)
	{
		Assert(0);
	}
}

void GLIB_PutRectangle2(int x1, int y1, int dx, int dy, u32 cvalue)
{
	u32 x2 = x1+dx-1;
	u32 y2 = y1+dy-1;
	
	GLIB_PutLine2(x1,y1,x2,y1,cvalue);
	GLIB_PutLine2(x2,y1,x2,y2,cvalue);
	GLIB_PutLine2(x1,y2,x2,y2,cvalue);
	GLIB_PutLine2(x1,y1,x1,y2,cvalue);
}

void GLIB_PutRectangle4(int x1, int y1, int dx, int dy, u32 cvalue, ALPHA_SEL eSel)
{
	u32 x2 = x1+dx-1;
	u32 y2 = y1+dy-1;
	
	GLIB_PutLine4(x1,y1,x2,y1,cvalue, eSel);
	GLIB_PutLine4(x2,y1,x2,y2,cvalue, eSel);
	GLIB_PutLine4(x1,y2,x2,y2,cvalue, eSel);
	GLIB_PutLine4(x1,y1,x1,y2,cvalue, eSel);
}

void GLIB_PutPalRectangle(int nPosX1, int nPosY1, int nHSz, int nVSz, u32 uData)
{
	u32 uPosX2 = nPosX1+nHSz-1;
	u32 uPosY2 = nPosY1+nVSz-1;

	GLIB_PutPalLine(nPosX1, nPosY1, uPosX2, nPosY1, uData);
	GLIB_PutPalLine(uPosX2, nPosY1, uPosX2, uPosY2, uData);
	GLIB_PutPalLine(nPosX1, uPosY2, uPosX2, uPosY2, uData);
	GLIB_PutPalLine(nPosX1, nPosY1, nPosX1, uPosY2, uData);
}

void GLIB_FillRectangle2(int x1, int y1, int dx, int dy, u32 cvalue)
{
	int i;
	u32 x2 = x1+dx-1;
	u32 y2 = y1+dy-1;
	
	for(i=y1;i<=y2;i++)
		GLIB_PutLine2(x1,i,x2,i,cvalue);
}

void GLIB_FillRectangle4(int x1, int y1, int dx, int dy, u32 cvalue, ALPHA_SEL eSel)
{
	int i;
	u32 x2 = x1+dx-1;
	u32 y2 = y1+dy-1;
	
	for(i=y1;i<=y2;i++)
		GLIB_PutLine4(x1, i, x2, i, cvalue, eSel);
}

void GLIB_FillPalRectangle(int nPosX1, int nPosY1, int nHSz, int nVSz, u32 uData)
{
	int i;
	u32 uPosX2 = nPosX1+nHSz-1;
	u32 uPosY2 = nPosY1+nVSz-1;
	
	for(i=nPosY1;i<=uPosY2;i++)
		GLIB_PutPalLine(nPosX1,i,uPosX2,i,uData);
}
// LCD display is flipped vertically
// But, think the algorithm by mathematics point.
//   3 I 2
//   4 I 1
//   --+--   <-8 octants  mathematical cordinate
//   5 I 8
//   6 I 7
void GLIB_PutLine2(int x1, int y1, int x2, int y2, u32 cvalue)
{

	int dx,dy,e;
	dx=x2-x1;
	dy=y2-y1;

	if(dx>=0)
	{
		if(dy >= 0) // dy>=0
		{
			if(dx>=dy) // 1/8 octant
			{
				e=dy-dx/2;
				while(x1<=x2)
				{
					GLIB_PutPixel2(x1,y1,cvalue);
					if(e>0){y1+=1;e-=dx;}
					x1+=1;
					e+=dy;
				}
			}
			else		// 2/8 octant
			{
				e=dx-dy/2;
				while(y1<=y2)
				{
					GLIB_PutPixel2(x1,y1,cvalue);
					if(e>0){x1+=1;e-=dy;}
					y1+=1;
					e+=dx;
				}
			}
		}
		else		   // dy<0
		{
			dy=-dy;   // dy=abs(dy)

			if(dx>=dy) // 8/8 octant
			{
				e=dy-dx/2;
				while(x1<=x2)
				{
					GLIB_PutPixel2(x1,y1,cvalue);
					if(e>0){y1-=1;e-=dx;}
					x1+=1;
					e+=dy;
				}
			}
			else		// 7/8 octant
			{
				e=dx-dy/2;
				while(y1>=y2)
				{
					GLIB_PutPixel2(x1,y1,cvalue);
					if(e>0){x1+=1;e-=dy;}
					y1-=1;
					e+=dx;
				}
			}
		}
	}
	else //dx<0
	{
		dx=-dx;		//dx=abs(dx)
		if(dy >= 0) // dy>=0
		{
			if(dx>=dy) // 4/8 octant
			{
				e=dy-dx/2;
				while(x1>=x2)
				{
					GLIB_PutPixel2(x1,y1,cvalue);
					if(e>0){y1+=1;e-=dx;}
					x1-=1;
					e+=dy;
				}
			}
			else		// 3/8 octant
			{
				e=dx-dy/2;
				while(y1<=y2)
				{
					GLIB_PutPixel2(x1,y1,cvalue);
					if(e>0){x1-=1;e-=dy;}
					y1+=1;
					e+=dx;
				}
			}
		}
		else		   // dy<0
		{
			dy=-dy;   // dy=abs(dy)

			if(dx>=dy) // 5/8 octant
			{
				e=dy-dx/2;
				while(x1>=x2)
				{
					GLIB_PutPixel2(x1,y1,cvalue);
					if(e>0){y1-=1;e-=dx;}
					x1-=1;
					e+=dy;
				}
			}
			else		// 6/8 octant
			{
				e=dx-dy/2;
				while(y1>=y2)
				{
					GLIB_PutPixel2(x1,y1,cvalue);
					if(e>0){x1-=1;e-=dy;}
					y1-=1;
					e+=dx;
				}
			}
		}
	}
}

void GLIB_PutLine4(int nX1, int nY1, int nX2, int nY2, u32 uCValue, ALPHA_SEL eSel)
{
	int dx,dy,e;
	u32 uTmpColor;
	
	u32 uAlphaVal = (eSel == ALPHA_0) ? 0 : 1;

	dx=nX2-nX1;
	dy=nY2-nY1;

#if 0
	uTmpColor &=
		(oGlib.m_eCSpace == ARGB8) ? (ARGB8_ALPHA_MASK & uAlphaVal<<7 | uCValue) :
		(oGlib.m_eCSpace == ARGB16) ? (ARGB16_ALPHA_MASK & uAlphaVal<<15 | uCValue) :
		(oGlib.m_eCSpace == ARGB24) ? (ARGB24_ALPHA_MASK & uAlphaVal<<24 | uCValue) : 0xffffffff;
#else
	uTmpColor =
		(oGlib.m_eCSpace == ARGB8) ? (uAlphaVal<<7 | uCValue) :
		(oGlib.m_eCSpace == ARGB16) ? (uAlphaVal<<15 | uCValue) :
		(oGlib.m_eCSpace == ARGB24) ? (uAlphaVal<<23 | uCValue) : 0xffffffff;

#endif

	if (uTmpColor == 0xffffffff)
		Assert(0);

	if(dx>=0)
	{
		if(dy >= 0) // dy>=0
		{
			if(dx>=dy) // 1/8 octant
			{
				e=dy-dx/2;
				while(nX1<=nX2)
				{
					GLIB_PutPixel2(nX1,nY1,uTmpColor);
					if(e>0){nY1+=1;e-=dx;}
					nX1+=1;
					e+=dy;
				}
			}
			else		// 2/8 octant
			{
				e=dx-dy/2;
				while(nY1<=nY2)
				{
					GLIB_PutPixel2(nX1,nY1,uTmpColor);
					if(e>0){nX1+=1;e-=dy;}
					nY1+=1;
					e+=dx;
				}
			}
		}
		else		   // dy<0
		{
			dy=-dy;   // dy=abs(dy)

			if(dx>=dy) // 8/8 octant
			{
				e=dy-dx/2;
				while(nX1<=nX2)
				{
					GLIB_PutPixel2(nX1,nY1,uTmpColor);
					if(e>0){nY1-=1;e-=dx;}
					nX1+=1;
					e+=dy;
				}
			}
			else		// 7/8 octant
			{
				e=dx-dy/2;
				while(nY1>=nY2)
				{
					GLIB_PutPixel2(nX1,nY1,uTmpColor);
					if(e>0){nX1+=1;e-=dy;}
					nY1-=1;
					e+=dx;
				}
			}
		}
	}
	else //dx<0
	{
		dx=-dx;		//dx=abs(dx)
		if(dy >= 0) // dy>=0
		{
			if(dx>=dy) // 4/8 octant
			{
				e=dy-dx/2;
				while(nX1>=nX2)
				{
					GLIB_PutPixel2(nX1,nY1,uTmpColor);
					if(e>0){nY1+=1;e-=dx;}
					nX1-=1;
					e+=dy;
				}
			}
			else		// 3/8 octant
			{
				e=dx-dy/2;
				while(nY1<=nY2)
				{
					GLIB_PutPixel2(nX1,nY1,uTmpColor);
					if(e>0){nX1-=1;e-=dy;}
					nY1+=1;
					e+=dx;
				}
			}
		}
		else		   // dy<0
		{
			dy=-dy;   // dy=abs(dy)

			if(dx>=dy) // 5/8 octant
			{
				e=dy-dx/2;
				while(nX1>=nX2)
				{
					GLIB_PutPixel2(nX1,nY1,uTmpColor);
					if(e>0){nY1-=1;e-=dx;}
					nX1-=1;
					e+=dy;
				}
			}
			else		// 6/8 octant
			{
				e=dx-dy/2;
				while(nY1>=nY2)
				{
					GLIB_PutPixel2(nX1,nY1,uTmpColor);
					if(e>0){nX1-=1;e-=dy;}
					nY1-=1;
					e+=dx;
				}
			}
		}
	}
}

void GLIB_PutPalLine(int nPosX1, int nPosY1, int nPosX2, int nPosY2, u32 uData)
{

	int dx,dy,e;
	dx=nPosX2-nPosX1;
	dy=nPosY2-nPosY1;

	if(dx>=0)
	{
		if(dy >= 0) // dy>=0
		{
			if(dx>=dy) // 1/8 octant
			{
				e=dy-dx/2;
				while(nPosX1<=nPosX2)
				{
					GLIB_PutPalData(nPosX1,nPosY1,uData);
					if(e>0){nPosY1+=1;e-=dx;}
					nPosX1+=1;
					e+=dy;
				}
			}
			else		// 2/8 octant
			{
				e=dx-dy/2;
				while(nPosY1<=nPosY2)
				{
					GLIB_PutPalData(nPosX1,nPosY1,uData);
					if(e>0){nPosX1+=1;e-=dy;}
					nPosY1+=1;
					e+=dx;
				}
			}
		}
		else		   // dy<0
		{
			dy=-dy;   // dy=abs(dy)

			if(dx>=dy) // 8/8 octant
			{
				e=dy-dx/2;
				while(nPosX1<=nPosX2)
				{
					GLIB_PutPalData(nPosX1,nPosY1,uData);
					if(e>0){nPosY1-=1;e-=dx;}
					nPosX1+=1;
					e+=dy;
				}
			}
			else		// 7/8 octant
			{
				e=dx-dy/2;
				while(nPosY1>=nPosY2)
				{
					GLIB_PutPalData(nPosX1,nPosY1,uData);
					if(e>0){nPosX1+=1;e-=dy;}
					nPosY1-=1;
					e+=dx;
				}
			}
		}
	}
	else //dx<0
	{
		dx=-dx;		//dx=abs(dx)
		if(dy >= 0) // dy>=0
		{
			if(dx>=dy) // 4/8 octant
			{
				e=dy-dx/2;
				while(nPosX1>=nPosX2)
				{
					GLIB_PutPalData(nPosX1,nPosY1,uData);
					if(e>0){nPosY1+=1;e-=dx;}
					nPosX1-=1;
					e+=dy;
				}
			}
			else		// 3/8 octant
			{
				e=dx-dy/2;
				while(nPosY1<=nPosY2)
				{
					GLIB_PutPalData(nPosX1,nPosY1,uData);
					if(e>0){nPosX1-=1;e-=dy;}
					nPosY1+=1;
					e+=dx;
				}
			}
		}
		else		   // dy<0
		{
			dy=-dy;   // dy=abs(dy)

			if(dx>=dy) // 5/8 octant
			{
				e=dy-dx/2;
				while(nPosX1>=nPosX2)
				{
					GLIB_PutPalData(nPosX1,nPosY1,uData);
					if(e>0){nPosY1-=1;e-=dx;}
					nPosX1-=1;
					e+=dy;
				}
			}
			else		// 6/8 octant
			{
				e=dx-dy/2;
				while(nPosY1>=nPosY2)
				{
					GLIB_PutPalData(nPosX1,nPosY1,uData);
					if(e>0){nPosX1-=1;e-=dy;}
					nPosY1-=1;
					e+=dx;
				}
			}
		}
	}
}

void GLIB_ClearFrame2(u32 c)
{
	int i, j;
	
	for(j=0; j<oGlib.m_uVsz; j++)
		for(i=0; i<oGlib.m_uHsz; i++)
			GLIB_PutPixel2(i, j, c);

}

void GLIB_ClearFrame4(u32 uCValue, ALPHA_SEL eSel)
{
	int i, j;

	u32 uAlphaVal = (eSel == ALPHA_0) ? 0 : 1;

#if 0
	u32 uTmpColor =
		(oGlib.m_eCSpace == ARGB8) ? (ARGB8_ALPHA_MASK & uAlphaVal<<7 | uCValue) :
		(oGlib.m_eCSpace == ARGB16) ? (ARGB16_ALPHA_MASK & uAlphaVal<<15 | uCValue) :
		(oGlib.m_eCSpace == ARGB24) ? (ARGB24_ALPHA_MASK & uAlphaVal<<24 | uCValue) : 0xffffffff;
#else
	u32 uTmpColor =
		(oGlib.m_eCSpace == ARGB8) ? (uAlphaVal<<7 | uCValue) :
		(oGlib.m_eCSpace == ARGB16) ? (uAlphaVal<<15 | uCValue) :
		(oGlib.m_eCSpace == ARGB24) ? (uAlphaVal<<23 | uCValue) : 0xffffffff;

#endif
	if (uTmpColor == 0xffffffff)
		
		Assert(0);

	
	for(j=0; j<oGlib.m_uVsz; j++)
	for(i=0; i<oGlib.m_uHsz; i++)
		GLIB_PutPixel2(i, j, uTmpColor);
}

void GLIB_ClearPalFrame(u32 uData)
{
	int i, j;
//	u32 uCnt = 0;
	for(j=0; j<oGlib.m_uVsz; j++)
		for(i=0; i<oGlib.m_uHsz; i++)
			GLIB_PutPalData(i, j, uData);

}

void GLIB_InitFont(void)
{
	oGlib.m_pFont8x15 = (u8 *)font8x15;

	GLIB_SetFontColor(C_WHITE, C_BLACK, 0);
}

void GLIB_SetFontColor(COLOR eFontColor, COLOR eBgColor, u8 bIsBgTransparent)
{
	oGlib.m_fontColor = eFontColor;
	oGlib.m_bgColor = eBgColor;
	oGlib.m_bIsBgTransparent = bIsBgTransparent;
}

void GLIB_Printf(u32 x, u32 y, const char *fmt,...)
{
	va_list ap;
	char string[256];

	va_start(ap,fmt);
	vsprintf(string,fmt,ap);
	GLIB_PutString(x,y,string);
	va_end(ap);
}

void GLIB_PutString(u32 x, u32 y, char *str)
{
	int i,j,k;
	int x1=x;
	int oneLine;
	u8 *oneLetter;

	for(i=0;i<strlen(str);i++)
	{
		oneLetter = oGlib.m_pFont8x15+(str[i]-START_ASCII)*15;
	
		for(j=0; j<=(FONT_END_LINE-FONT_START_LINE); j++)
		{
			oneLine=oneLetter[j+FONT_START_LINE];
			for(k=0;k<8;k++)
			{
				if(oneLine&(1<<k))
					GLIB_PutPixel(x1+7-k,y+j,oGlib.m_fontColor);
				else  if(!oGlib.m_bIsBgTransparent)
					GLIB_PutPixel(x1+7-k,y+j,oGlib.m_bgColor);
			}
		}
		x1=x1+8;
	}
}

// Below code will be modified on modifying g2d(g2d class use this function)
#if 0
void GLIB::MakeColor(u32 uRed, u32 uGreen, u32 uBlue, u32& color)
{
	if (oGlib.m_uBpp == 15) {
		color = ((uRed&0x1f)<<10) | ((uGreen&0x1f)<<5) | (uBlue&0x1f);
	} else if (oGlib.m_uBpp == 16) {
		color = ((uRed&0x1f)<<11) | ((uGreen&0x3f)<<5) | (uBlue&0x1f);	
	} else if (oGlib.m_uBpp == 18) {
		color = ((uRed&0x3f)<<12) | ((uGreen&0x3f)<<6) | (uBlue&0x3f);
	} else if (oGlib.m_uBpp == 24) {
		color = ((uRed&0xff)<<16) | ((uGreen&0xff)<<8) | (uBlue&0xff);
	} else {
		Assert(0);
	}
}
#else
void GLIB_MakeColor(u32 uRed, u32 uGreen, u32 uBlue, u32 *color)
{
	if (oGlib.m_uBpp == 16) {
		*color = ((uRed&0x1f)<<11) | ((uGreen&0x3f)<<5) | (uBlue&0x1f);	
	} else if (oGlib.m_uBpp == 18) {
		*color = ((uRed&0x3f)<<12) | ((uGreen&0x3f)<<6) | (uBlue&0x3f);
	} else if (oGlib.m_uBpp == 24) {
		*color = ((uRed&0xff)<<16) | ((uGreen&0xff)<<8) | (uBlue&0xff);
	} else {
		Assert(0);
	}
}
#endif

void GLIB_SetAlpha(COLOR eColor, u32 uAlpha)
{
	u32 idx = (u32)eColor;
	oGlib.m_uColorValue[idx] = oGlib.m_uColorValue[idx]&(0x00ffffff)|((uAlpha&0x0f)<<24);
}

void GLIB_DrawPattern(u32 uWidth, u32 uHeight)
{
	u32 uSubWidth, uSubHeight;

	// uWidth and uHeight are multiple of eight
	Assert( !((uWidth%8)  || (uHeight%8)) );
	
	GLIB_ClearFrame(C_WHITE);

	// Draw Color Bar
	uSubWidth = uWidth/8;
	uSubHeight = uHeight/2;

	GLIB_FillRectangle(0, 0, uSubWidth, uSubHeight, C_RED);
	GLIB_FillRectangle(uSubWidth*1, 0, uSubWidth, uSubHeight, C_GREEN);
	GLIB_FillRectangle(uSubWidth*2, 0, uSubWidth, uSubHeight, C_BLUE);	
	GLIB_FillRectangle(uSubWidth*3, 0, uSubWidth, uSubHeight, C_CYAN);		
	GLIB_FillRectangle(uSubWidth*4, 0, uSubWidth, uSubHeight, C_MAGENTA);			
	GLIB_FillRectangle(uSubWidth*5, 0, uSubWidth, uSubHeight, C_YELLOW);				
	GLIB_FillRectangle(uSubWidth*6, 0, uSubWidth, uSubHeight, C_BLACK);
	GLIB_FillRectangle(uSubWidth*7, 0, uSubWidth, uSubHeight, C_WHITE);

	// Draw Pattern

	uSubWidth = uWidth/2;
	GLIB_PutRectangle(0, uSubHeight, uWidth, uSubHeight, C_RED);
	GLIB_PutLine(0, uSubHeight, uWidth-1, uHeight-1, C_BLACK);
	GLIB_PutLine(uWidth, uSubHeight, 0, uHeight-1, C_MAGENTA);	

	uSubWidth = uWidth/2;
	GLIB_PutLine(uSubWidth, uSubHeight, uSubWidth, uHeight-1, C_GREEN);		

	uSubHeight = uHeight*3/4;
	GLIB_PutLine(0, uSubHeight, uWidth-1, uSubHeight, C_BLUE);
}

void GLIB_DrawPattern2(u32 uWidth, u32 uHeight)
{
	u32 uSubHSz, uSubVSz;

	Assert( !((uWidth%8)  || (uHeight%8)) );

	uSubHSz = uWidth/2;
	uSubVSz = uHeight/4;
	GLIB_FillRectangle(0, 0, uSubHSz, uSubVSz, C_GREEN);
	GLIB_FillRectangle(uSubHSz, 0, uSubHSz, uSubVSz, C_BLUE);
	GLIB_FillRectangle(0, uSubVSz, uSubHSz, uSubVSz, C_WHITE);
	GLIB_FillRectangle(uSubHSz, uSubVSz, uSubHSz, uSubVSz, C_YELLOW);
	
	uSubHSz = uWidth/4;
	uSubVSz = uHeight/2;
	GLIB_FillRectangle(0, uSubVSz, uSubHSz, uSubVSz, C_MAGENTA);
	GLIB_FillRectangle(uSubHSz*1, uSubVSz, uSubHSz, uSubVSz, C_YELLOW);
	GLIB_FillRectangle(uSubHSz*2, uSubVSz, uSubHSz, uSubVSz, C_CYAN);	
	GLIB_FillRectangle(uSubHSz*3, uSubVSz, uSubHSz, uSubVSz, C_WHITE);		

	uSubHSz = uWidth/2;
	uSubVSz = uHeight/2;
	GLIB_PutLine(uSubHSz, 0, 0, uSubVSz-1, C_BLACK);
	GLIB_PutLine(uSubHSz, 0, uWidth-1, uSubVSz-1, C_BLACK);
	GLIB_PutLine(0, uSubVSz-1, uSubHSz-1, uHeight-1, C_BLACK);
	GLIB_PutLine(uSubHSz-1, uHeight-1, uWidth-1, uSubVSz-1, C_BLACK);

	GLIB_PutLine(0, 0, uWidth-1, uHeight-1, C_RED);
	GLIB_PutLine(uWidth-1, 0, 0, uHeight-1, C_GREEN);
	GLIB_PutRectangle(0, 0, uWidth, uHeight, C_RED);
}
