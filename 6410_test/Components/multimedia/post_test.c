/*----------------------------------------------------------------------
 *
 * Filename: post_test.cpp
 *
 * Contents: Testing application of "class POST"
 *
 * Authors: Dhrama
 *
 * Notes:
 *
 * Copyright (c) 2003 SAMSUNG Electronics.
 *
 *----------------------------------------------------------------------
 */

 #include <stdio.h>

#include "def.h"
#include "option.h"
#include "library.h"
#include "sfr6410.h"
#include "system.h"
#include "sysc.h"
#include "post.h"
#include "post_test.h"
#include "lcd.h"
#include "glib.h"
#include "intc.h"
#include "camera.h"
#include "timer.h"


#if 0

#include "Smile800480_24bpp2.h"
#include "bgcolors216.h"


#define IMG_YC420       "../images/420_240x320result(rotate90).yuv"
#define POST_YUV_FILE_NAME1      "D:/MyWork/MyProject/S3C6410/SW/TestCode/6410_Test_Rev02/Components/multimedia/image/CatureYUV/Captured1.yuv"
#define POST_YUV_FILE_NAME2      "D:/MyWork/MyProject/S3C6410/SW/TestCode/6410_Test_Rev02/Components/multimedia/image/CatureYUV/Captured2.yuv"

#define POST_IMAGE_PATTERN	0
#define POST_IMAGE_HEADER		1
#define POST_IMAGE_SOURCE		POST_IMAGE_PATTERN

#define POST_IMAGE_BMP_SLIDE			(TRUE)

#if (POST_IMAGE_SORCE == POST_IMAGE_HEADER)
#include "Guitar240320_16rgb.h"
#include "Guitar240320_24.h"
#include "guitar240320_YCrYCb422.h"
#include "guitar240320_YCbYCr422.h"
#include "guitar240320_CrYCbY422.h"
#include "guitar240320_CbYCrY422.h"
#include "guitar240320_YCbYCr420.h"
#include "RacingGirl320240.h"
#include "sun320240_16bpp.h"
#include "Yepp320x240.h"
#endif
#endif

//rb1004 test
//#define LCD_INIT_TEST

//static GLIB oGlib;
//static LCDC oLcdc;
static POST oPost;
static POST oSc;

//static INTC oIntc;
//static CIM  oCim;

//static CSPACE eLcdBpp;
static CSPACE eSrcDataFmt;
static CSPACE eMidDataFmt;
static CSPACE eDstDataFmt;

static CSPACE eSrcDataFmt_Yuv;
static CSPACE eDstDataFmt_Yuv;

//static CSPACE eDstDataFmt_FifoOut; // From DMA TO FIFO
//static POST_FIFO_IF ePostFifoIf;

static POST_RUN_MODE ePostRunMode;

//static LCD_LOCAL_IN_DATA_FORMAT eLcdLocalDataFmt;

//static u32 uLcdFbAddr;

static u32 uLcdHSz, uLcdVSz;	
static u32 uSrcStartX, uSrcStartY;
static u32 uSrcCroppedHSz, uSrcCroppedVSz;

static u32 uMidImgHSz, uMidImgVSz;
static u32 uMidStartX, uMidStartY;
static u32 uMidScaledHSz, uMidScaledVSz;

static u32 uMid2ImgHSz, uMid2ImgVSz;
static u32 uMid2StartX, uMid2StartY;
static u32 uMid2ScaledHSz, uMid2ScaledVSz;

static u32 uImgHSz, uImgVSz;	
static u32 uDstStartX, uDstStartY;
static u32 uDstScaledHSz, uDstScaledVSz;

//static u8 bIsR2Y2RFlag = false;

const static u32 uLcdStAddr = CODEC_MEM_ST; 

//static u32 uFrameCnt_Int = 0;
static u32 uNxtAddr1, uNxtAddr2;

//static u32 uIsrCnt;
const static u32 uMaxFrameCnt = 10;

u8 bIsPollingOrInt = true; // true: Polling, false: Int

//rb1004
static u32 uSimpleTest;
static u8 uPostIntOccured;

u8 *Image_Entertain[] = {	"D:/MyWork/MyProject/S3C6410/SW/TestCode/6410_Test_Rev02/Components/multimedia/image/bmp/entertainment1.bmp",
								"D:/MyWork/MyProject/S3C6410/SW/TestCode/6410_Test_Rev02/Components/multimedia/image/bmp/entertainment2.bmp",
								"D:/MyWork/MyProject/S3C6410/SW/TestCode/6410_Test_Rev02/Components/multimedia/image/bmp/entertainment3.bmp",
								"D:/MyWork/MyProject/S3C6410/SW/TestCode/6410_Test_Rev02/Components/multimedia/image/bmp/entertainment4.bmp",
								"D:/MyWork/MyProject/S3C6410/SW/TestCode/6410_Test_Rev02/Components/multimedia/image/bmp/entertainment5.bmp",
								"D:/MyWork/MyProject/S3C6410/SW/TestCode/6410_Test_Rev02/Components/multimedia/image/bmp/entertainment6.bmp",
								"D:/MyWork/MyProject/S3C6410/SW/TestCode/6410_Test_Rev02/Components/multimedia/image/bmp/entertainment7.bmp",
								"D:/MyWork/MyProject/S3C6410/SW/TestCode/6410_Test_Rev02/Components/multimedia/image/bmp/entertainment8.bmp",
								"D:/MyWork/MyProject/S3C6410/SW/TestCode/6410_Test_Rev02/Components/multimedia/image/bmp/entertainment9.bmp",
								"D:/MyWork/MyProject/S3C6410/SW/TestCode/6410_Test_Rev02/Components/multimedia/image/bmp/entertainment10.bmp",
								"D:/MyWork/MyProject/S3C6410/SW/TestCode/6410_Test_Rev02/Components/multimedia/image/bmp/entertainment11.bmp",
								"D:/MyWork/MyProject/S3C6410/SW/TestCode/6410_Test_Rev02/Components/multimedia/image/bmp/entertainment12.bmp",
								"D:/MyWork/MyProject/S3C6410/SW/TestCode/6410_Test_Rev02/Components/multimedia/image/bmp/entertainment13.bmp",
								"D:/MyWork/MyProject/S3C6410/SW/TestCode/6410_Test_Rev02/Components/multimedia/image/bmp/entertainment14.bmp",
								"D:/MyWork/MyProject/S3C6410/SW/TestCode/6410_Test_Rev02/Components/multimedia/image/bmp/entertainment15.bmp",
								"D:/MyWork/MyProject/S3C6410/SW/TestCode/6410_Test_Rev02/Components/multimedia/image/bmp/Illustrate1.bmp",
								"D:/MyWork/MyProject/S3C6410/SW/TestCode/6410_Test_Rev02/Components/multimedia/image/bmp/Illustrate2.bmp",
								"D:/MyWork/MyProject/S3C6410/SW/TestCode/6410_Test_Rev02/Components/multimedia/image/bmp/Illustrate3.bmp",
								"D:/MyWork/MyProject/S3C6410/SW/TestCode/6410_Test_Rev02/Components/multimedia/image/bmp/Illustrate4.bmp",
								"D:/MyWork/MyProject/S3C6410/SW/TestCode/6410_Test_Rev02/Components/multimedia/image/bmp/Illustrate5.bmp",
								"D:/MyWork/MyProject/S3C6410/SW/TestCode/6410_Test_Rev02/Components/multimedia/image/bmp/Illustrate6.bmp",
								"D:/MyWork/MyProject/S3C6410/SW/TestCode/6410_Test_Rev02/Components/multimedia/image/bmp/Illustrate7.bmp",
								"D:/MyWork/MyProject/S3C6410/SW/TestCode/6410_Test_Rev02/Components/multimedia/image/bmp/Illustrate8.bmp",
								"D:/MyWork/MyProject/S3C6410/SW/TestCode/6410_Test_Rev02/Components/multimedia/image/bmp/Illustrate9.bmp"
							};
////////////////////////////////////////////////////////////////////////////
//////////////////                         ISR Routine                         ///////////////////
////////////////////////////////////////////////////////////////////////////

void __irq PostISRDma(void)
{
	uPostIntOccured = TRUE;
	POST_ClearPending(&oPost);
	INTC_ClearVectAddr();
}


/*************************
  Function CSCR2Y ( Color space Convertor RGB to YCbCr)

  Input
  R[7:0], G[7:0], B[7:0]
  Return Value
  CSCR2Y[23:16] : Y
  CSCR2Y[15: 8] : Cb
  CSCR2Y[ 7: 0] : Cr
  Control by R2YCSEL
1: Wide Range
0: Narrow Range
 ************************/
int CSCR2Y_C(int R, int  G, int B, int R2YCSEL)
{
  //int		i;
  int		y;
  int		cb;
  int		cr;
  int		Y;
  int		Cb;
  int		Cr;
  int		R25;
  int		G50;
  int		B09;
  int		R14;
  int		G29;
  int		B43;
  int		R43;
  int		G36;
  int		B07;
  int		r25;
  int		g50;
  int		b09;
  int		r14;
  int		g29;
  int		b43;
  int		r43;
  int		g36;
  int		b07;

 // int             R25_8;
//  int             r25_8;
//int             G50_8;
//  int             g50_8;
//  int             B09_8;
//  int             b09_8;
//  int             R14_8;
 // int             r14_8;
  //int             G29_8;
 // int             g29_8;
//  int             B43_8;
//  int             b43_8;
//  int             R43_8;
//  int             r43_8;
//  int             G36_8;
//  int             g36_8;
//  int             B07_8;
//  int             b07_8;

  int		CSCR2Y;
  int		CSCR2y;


  R25     = (R<<6) + (R<<1);
  r25     = (R<<6) + (R<<3) + (R<<2) + (R);
  G50     = (G) + (G<<7);
  g50     = (G<<7) + (G<<4) + (G<<2) + (G<<1);
  B09     = (B<<4) + (B<<3) + (B);
  b09     = (B<<4) + (B<<3) + (B<<2) + (B);
  R14     = (R<<5) + (R<<2) + (R<<1);
  r14     = (R<<5) + (R<<3) + (R<<2);
  G29     = (G<<6) + (G<<3) + (G<<1);
  g29     = (G<<6) + (G<<4) + (G<<3) - (G);
  B43     = (B<<6) + (B<<5) + (B<<4);
  b43     = (B<<7) + (B<<1) + (B);
  R43     = (R<<6) + (R<<5) + (R<<4);
  r43     = (R<<7) + (R<<1) + (R);
  G36     = (G<<7) - (G<<5) - (G<<1);
  g36     = (G<<7) - (G<<4) - (G<<1);
  B07     = (B<<4) + (B<<1);
  b07     = (B<<4) + (B<<2) + (B);

   Y  =    (R25>>8)  + (G50>>8) + (B09>>8) + 16 ;
   Cb = (- (R14>>8)) - (G29>>8) + (B43>>8) + 128;
   Cr =    (R43>>8)  - (G36>>8) - (B07>>8) + 128;

// for saturation
   if ( Y > 235 ) Y = 235;
   else if ( Y < 16 ) Y = 16;
   else Y = Y;

   if ( Cb > 240 ) Cb = 240;
   else if ( Cb < 16 ) Cb = 16;
   else Cb = Cb;

   if ( Cr > 240 ) Cr = 240;
   else if ( Cr < 16 ) Cr = 16;
   else Cr = Cr;

   y  =    (r25>>8)  + (g50>>8) + (b09>>8);
   cb = (- (r14>>8)) - (g29>>8) + (b43>>8) + 128;
   cr =    (r43>>8)  - (g36>>8) - (b07>>8) + 128;

// for saturation
   if ( y > 255 ) y = 255;
   else if ( y < 0 ) y = 0;
   else y = y;

   if ( cb > 255 ) cb = 255;
   else if ( cb < 0 ) cb = 0;
   else cb = cb;
   
   if ( cr > 255 ) cr = 255;
   else if ( cr < 0 ) cr = 0;
   else cr = cr;

  /* R2YCSEL == 0 */ 
  CSCR2Y = 0x00ffffff & (((0xff&Y)<<16) + ((0xff&Cb)<<8) + ((0xff&Cr)<<0)); 
  /* R2YCSEL == 1 */
  CSCR2y = 0x00ffffff & (((0xff&y)<<16) + ((0xff&cb)<<8) + ((0xff&cr)<<0)); 


  /* for Monitor
     UART_Printf("r25 : %x\n", r25);
     UART_Printf("g50 : %x\n", g50);
     UART_Printf("b09 : %x\n", b09);
     UART_Printf("y : %x\n", y);
     UART_Printf("CSCR2y : %x\n", CSCR2y);
     UART_Printf("R : %x\n", R);
     UART_Printf("G : %x\n", G);
     UART_Printf("B : %x\n", B);
     UART_Printf("Y : %x\n", Y);
     UART_Printf("Cb : %x\n", Cb);
     UART_Printf("Cr : %x\n", Cr);

     UART_Printf("y : %x\n", y);
     UART_Printf("cb : %x\n", cb);
     UART_Printf("cr : %x\n", cr);

     UART_Printf("YCbCr wide: %x\n", CSCR2Y);
     UART_Printf("YCbCr narrow: %x\n", CSCR2y);
   */

  if (R2YCSEL) return CSCR2y;
  else	return CSCR2Y;

}


void CSCY2R (u32 y, u32 cb, u32 cr, u32 Y2RSel, u32  OutRGBFormat)
{
//	unsigned int i;
//	unsigned int horizontal, vertical;
	int s_cb;
	int s_cr;
	int r, g, b;

	if ( Y2RSel == 1 ) 
	{
		if ( y < 64 ) y = 64;
		else if ( y > 940 ) y = 940;
		else y = y;

		if ( cb < 64 ) cb = 64;
		else if ( cb > 960 ) cb = 960;
		else cb = cb;

		if ( cr < 64 ) cr = 64;
		else if ( cr > 960 ) cr = 960;
		else cr = cr;
	}

	if ( Y2RSel == 2 )
	{

		if (cb > 512) 
		{
			cb = cb - 512;
			s_cb = 1;
		} 
		else if ( cb == 0 ) 
		{
			cb = 511;
			s_cb = -1;
		} else 
		{
			cb = 512 - cb;
			s_cb = -1;
		}

		if (cr >= 512) 
		{
			cr = cr - 512;
			s_cr = 1;
		} 
		else if ( cr == 0 ) 
		{
			cr = 511;
			s_cr = -1;
		} 
		else 
		{
			cr = 512 - cr;
			s_cr = -1;
		}
		r = y + s_cr * (((cr<<4) + (cr<<2) + (cr<<1)) >> 4); 
		g = y - s_cr * (((cr<<3) + (cr<<1) + cr) >> 4) - s_cb * (((cb<<4) + (cb<<2) + cb) >> 6);
		b = y + s_cb * (((cb<<2) + (cb<<1) + cb) >> 2);

	}
	else if ( Y2RSel == 1 ) 
	{

		y = y - 64;

		if (cb >= 512) 
		{
			cb = cb - 512;
			s_cb = 1;
		} 
		else 
		{
			cb = 512 - cb;
			s_cb = -1;
		}

		if (cr >= 512) 
		{
			cr = cr - 512;
			s_cr = 1;
		} 
		else 
		{
			cr = 512 - cr;
			s_cr = -1;
		}
		r = (((y<<7) + (y<<4) + (y<<2) + y) >>7 ) + s_cr * (((cr<<4) + (cr<<3) + (cr<<1) + cr) >>4 );
		g = (((y<<7) + (y<<4) + (y<<2) + y) >>7 ) - s_cr * (((cr<<3) + (cr<<2) + cr) >>4 ) - 
		    s_cb * (((cb<<4) + (cb<<3) + cb) >>6 );
		b = (((y<<7) + (y<<4) + (y<<2) + y) >>7 ) + s_cb * ((cb<<1) + (cb>>8));

	}
	else 
	{
		UART_Printf ("Y2R CSC Selection Error\n");
	}

	if ( r > 1023 ) r = 1023;
	else if ( r < 0 ) r = 0;

	if ( g > 1023 ) g = 1023;
	else if ( g < 0 ) g = 0;
	
	if ( b > 1023 ) b = 1023;
	else if ( b < 0 ) b = 0;

	if (OutRGBFormat) {
		UART_Printf ("%3d  %3d  %3d\n", r>>2, g>>2, b>>2);
	}
	else {
		//fprintf ( output_F, "%3d  %3d  %3d\n", (r&0x3e0>>2), (g&0x3f0>>2), (b&0x3e0>>2));
		UART_Printf ("%3d  %3d  %3d\n", (r&0x3e0)>>2, (g&0x3f0)>>2, (b&0x3e0)>>2);
	}
}



#if (POST_IMAGE_SOURCE == POST_IMAGE_HEADER)
static void POSTT_GetImageFromHeader(u32 uFbAddr, u32 uHSize, u32 uVSize, CSPACE eBpp)
{
	u32 i;
	u16 *pBufAddr16;
	u32 *pBufAddr32;

	pBufAddr16 = (u16 *)uFbAddr;
	pBufAddr32 = (u32 *)uFbAddr;

	switch(eBpp)
	{
		case RGB16 : 	for(i=0 ; i<uHSize*uVSize ; i++)
						{
							//*pBufAddr16++ = pGuitar240320_16rgb[i];
							//*pBufAddr16++ = pRacingGirl320240[i];
							*pBufAddr16++ = sun320240_16bpp[i];
						}
						break;
		case RGB24 : 	for(i=0 ; i<uHSize*uVSize ; i++)
						{
							//*pBufAddr32++ = pGuitar240320_24rgb[i];
							//*pBufAddr32++ = pYepp320x240[i];
							*pBufAddr32++ = pSmile_2[i];
						}
						break;
		case YC420 : 	for(i=0 ; i<uHSize*uVSize ; i++)
						{
							*pBufAddr16++ = pGuitar240320_YCbYCr420[i];
						}
						break;
		case YCBYCR : 	for(i=0 ; i<uHSize*uVSize ; i++)
						{
							*pBufAddr16++ = pGuitar240320_YCbYCr422[i];
						}
						break;
		case YCRYCB : 	for(i=0 ; i<uHSize*uVSize ; i++)
						{
							*pBufAddr16++ = pGuitar240320_YCrYCb422[i];
						}
						break;
		case CBYCRY : 	for(i=0 ; i<uHSize*uVSize ; i++)
						{
							*pBufAddr16++ = pGuitar240320_CbYCrY422[i];
						}
						break;
		case CRYCBY : 	for(i=0 ; i<uHSize*uVSize ; i++)
						{
							*pBufAddr16++ = pGuitar240320_CrYCbY422[i];
						}
						break;
		default	: 		break;
	}

}
#endif


static void POSTT_GetImageFromBMP(u32 uFbAddr, u32 uHSize, u32 uVSize, u32 NumOfImage, CSPACE eBpp)
{
	u32 ImageNum;
	u16 *pBufAddr16;
	u32 *pBufAddr32;
	u32 *uBuffer_Temp;

	uBuffer_Temp = (u32 *)malloc(1500000);		//800x480 bmp file
	//uBuffer_Temp = (u32 *)malloc(300000);		//320x240 bmp file
	pBufAddr16 = (u16 *)uFbAddr;
	pBufAddr32 = (u32 *)uFbAddr;
	ImageNum = NumOfImage%(sizeof(Image_Entertain)/4);
	
	switch(eBpp)
	{
		case RGB16 : 	LoadFromFile((const char*)Image_Entertain[ImageNum], (u32)uBuffer_Temp);
						ConvertBmpToRgb16bpp((u32)uBuffer_Temp, (u32)pBufAddr16, uHSize, uVSize);
						break;
		case RGB24 : 	LoadFromFile((const char*)Image_Entertain[ImageNum], (u32)uBuffer_Temp);
						ConvertBmpToRgb24bpp((u32)uBuffer_Temp, (u32)pBufAddr32, uHSize, uVSize);
						break;
		default	: 		break;
	}

	free(uBuffer_Temp);
}


////////////////////////////////////////////////////////////////////////////
//////////////////                       RGB to RGB                           ///////////////////
////////////////////////////////////////////////////////////////////////////

static void POSTT_SetCscTypeDmaToDma_R2R(void)
{
	u32 uSelDmaCscType_R2R;

	while(1)
	{
		UART_Printf("\n");
		UART_Printf("[1] From RGB16 To RGB16\n");
		UART_Printf("[2] From RGB16 To RGB24\n");
		UART_Printf("[3] From RGB24 To RGB16\n");		
		UART_Printf("[4] From RGB24 To RGB24\n");
		UART_Printf("\n");

		UART_Printf(">>Select The Color Space Conversion Type: ");

		uSelDmaCscType_R2R = UART_GetIntNum();

		if (uSelDmaCscType_R2R == 1 || uSelDmaCscType_R2R == 2 || uSelDmaCscType_R2R == 3 || uSelDmaCscType_R2R == 4)
		{
			eSrcDataFmt = 	(uSelDmaCscType_R2R == 1 || uSelDmaCscType_R2R == 2) ? RGB16 : RGB24;
			eDstDataFmt = (uSelDmaCscType_R2R == 1 || uSelDmaCscType_R2R == 3) ? RGB16 : RGB24;

			break;
		}
		else 
			UART_Printf("Invalid Input! Retry It!!\n");
		
	}

	if (eSrcDataFmt == RGB16 && eDstDataFmt == RGB16)
	{
		// Scale up(width, height)
		uImgHSz = 120, 	uImgVSz = 160;
		uSrcStartX = 20, uSrcStartY = 30;
		uSrcCroppedHSz = 88, uSrcCroppedVSz = 100;

		uDstStartX = 40, uDstStartY = 80;
		uDstScaledHSz = 160, uDstScaledVSz = 160;

	}
	else if (eSrcDataFmt == RGB16 && eDstDataFmt == RGB24)
	{
		// Scale up(width), scale down(height)
		uImgHSz = 240,	uImgVSz = 320;
		uSrcStartX = 80, uSrcStartY = 160;
		uSrcCroppedHSz = 120,	uSrcCroppedVSz = 160;

		uDstStartX = 20, uDstStartY = 40;
		uDstScaledHSz = 200, uDstScaledVSz = 120;		
	}
	else if (eSrcDataFmt == RGB24 && eDstDataFmt == RGB16)
	{
		// scale down(width), scale up(height)
		uImgHSz = 360,	uImgVSz = 480;
		uSrcStartX = 30, uSrcStartY = 40;
		uSrcCroppedHSz = 160, uSrcCroppedVSz = 100;

		uDstStartX = 10, uDstStartY = 50;
		uDstScaledHSz = 120, uDstScaledVSz = 190;
	}
	else if (eSrcDataFmt == RGB24 && eDstDataFmt == RGB24)
	{
		// scale down(widht, height)
		uImgHSz = 360,	uImgVSz = 480;
		uSrcStartX = 80, uSrcStartY = 20;
		uSrcCroppedHSz = 168,	uSrcCroppedVSz = 240;

		uDstStartX = 100, uDstStartY = 100;
		uDstScaledHSz = 128, uDstScaledVSz = 140;
	}
	else
	{
		UART_Printf("Error! Unsupported Data Format!\n");
		return;
	}

	UART_Printf("\n");
	if(uSimpleTest == 0)
	{
		UART_Printf("=========    Complex_Test Image Size    =========\n");
		UART_Printf("SrcImgHSz     = %d,       SrcImgVSz     = %d\n",uImgHSz,uImgVSz);
		UART_Printf("SrcStartX     = %d,       SrcStartY     = %d\n",uSrcStartX,uSrcStartY);
		UART_Printf("SrcCroppedHSz = %d,       SrcCroppedVSz = %d\n",uSrcCroppedHSz,uSrcCroppedVSz);
		UART_Printf("DstStartX     = %d,       DstStartY     = %d\n",uDstStartX,uDstStartY);
		UART_Printf("DstScaledHSz  = %d,       DstScaledVSz  = %d\n",uDstScaledHSz,uDstScaledVSz);	
		UART_Printf("=================================================\n");
		UART_Printf("\n");
	}
}


static void POSTT_SimpleDmaToDma_R2R(void)
{
	u32 uLcdHSz, uLcdVSz;
	u32 uGlibStAddr;
	u32 uBytesPerPixel;

	UART_Printf("[POSTT_SimpleDmaToDma_R2R(No Scaling)]\n");

	uSimpleTest = 1;
	POSTT_SetCscTypeDmaToDma_R2R();
	
	LCD_InitDISPC(eDstDataFmt, uLcdStAddr, WIN0, false);
	LCD_Start();
	LCD_SetWinOnOff(1, WIN0);
	LCD_GetFrmSz(&uLcdHSz, &uLcdVSz, WIN0);
	
	// To Clear the LCD Display
	GLIB_Init(uLcdStAddr, uLcdHSz, uLcdVSz, eDstDataFmt);
	GLIB_ClearFrame(C_BLACK);
	
	uBytesPerPixel = (eDstDataFmt == RGB16) ? 2: 4;
	uGlibStAddr = uLcdStAddr + uLcdHSz*uLcdVSz*uBytesPerPixel;

#if (POST_IMAGE_SOURCE == POST_IMAGE_PATTERN)
		GLIB_Init(uGlibStAddr, uLcdHSz, uLcdVSz, eSrcDataFmt);
		GLIB_DrawPattern(uLcdHSz, uLcdVSz);
#elif (POST_IMAGE_SOURCE == POST_IMAGE_HEADER)
		POSTT_GetImageFromHeader(uGlibStAddr, uLcdHSz, uLcdVSz, eSrcDataFmt);
#endif
	POST_InitIpForDmaInDmaOut(	uLcdHSz, uLcdVSz, uGlibStAddr, eSrcDataFmt, 
									uLcdHSz, uLcdVSz, uLcdStAddr, eDstDataFmt,
									1, false, ONE_SHOT, &oPost );
	POST_StartProcessing(&oPost);
	while (!POST_IsProcessingDone(&oPost));	

#if (POST_IMAGE_BMP_SLIDE)	
	{
		u32 uImageNum;
		UART_Printf("Input the any key to display the other image ('q' : quit)\n");
		uImageNum = 0;

		while(UART_Getc() != 'q')
		{
			POSTT_GetImageFromBMP(uGlibStAddr, uLcdHSz, uLcdVSz, uImageNum, eSrcDataFmt);

			// Only CSC operation, not Scaling
			POST_InitIpForDmaInDmaOut(	uLcdHSz, uLcdVSz, uGlibStAddr, eSrcDataFmt, 
											uLcdHSz, uLcdVSz, uLcdStAddr, eDstDataFmt,
											1, false, ONE_SHOT, &oPost );
			POST_StartProcessing(&oPost);
			while (!POST_IsProcessingDone(&oPost));	
			uImageNum++;
		}
	}
#endif	
}


static void POSTT_ComplexIDmaToDma_R2R(void)
{
	u32 uGlibStAddr=0;
	u32 uBytesPerPixel=0;
	u32 uSrcStAddr;

	UART_Printf("[POSTT_ComplexIDmaToDma_R2R]\n");

	uSimpleTest = 0;
	POSTT_SetCscTypeDmaToDma_R2R();
	
	//bIsR2Y2RFlag = false;
	ePostRunMode = ONE_SHOT;
	
	//LCD_InitLDI(MAIN);
#ifdef LCD_INIT_TEST	
	LCD_InitBase();
	LCD_Start();
	LCD_InitWin(eDstDataFmt, uImgHSz, uImgVSz, 0, 0, uImgHSz, uImgVSz, 0, 0, uLcdStAddr, WIN0, false);
#else
	LCD_InitDISPC(eDstDataFmt, uLcdStAddr, WIN0, false);
	LCD_Start();
#endif
	LCD_SetWinOnOff(1, WIN0);
	LCD_GetFrmSz(&uLcdHSz, &uLcdVSz, WIN0);

	// To Clear the LCD Display
	GLIB_Init(uLcdStAddr, uLcdHSz, uLcdVSz, eDstDataFmt);
	GLIB_ClearFrame(C_BLACK);

	uBytesPerPixel = (eDstDataFmt == RGB16) ? 2: 4;
	uGlibStAddr = uLcdStAddr + uLcdHSz*uLcdVSz*uBytesPerPixel;
	uSrcStAddr=0;

	// CSC & Scaling the DrawPattern 1
#if (POST_IMAGE_SOURCE == POST_IMAGE_PATTERN)
	GLIB_Init(uGlibStAddr, uImgHSz, uImgVSz, eSrcDataFmt);
	GLIB_DrawPattern(uImgHSz, uImgVSz);
#elif (POST_IMAGE_SOURCE == POST_IMAGE_HEADER)
	POSTT_GetImageFromHeader(uGlibStAddr, uLcdHSz, uLcdVSz, eSrcDataFmt);
#endif

	POST_InitIp1(	uImgHSz, uImgVSz, uSrcStartX, uSrcStartY, uSrcCroppedHSz, uSrcCroppedVSz, uGlibStAddr, eSrcDataFmt,
					uLcdHSz, uLcdVSz, uDstStartX, uDstStartY, uDstScaledHSz, uDstScaledVSz, uLcdStAddr, eDstDataFmt,
					2, false, ePostRunMode, POST_DMA, POST_DMA, &oPost);

	UART_Printf("Draw Pattern 1(Frame 0)\n");
	POST_StartProcessing1(0, 0, &oPost);
	while (!POST_IsProcessingDone(&oPost));

	UART_Printf("Enter Any Key\n");
	UART_Getc();
	
	// CSC & Scaling the DrawPattern 2	
	POST_GetSrcStAddr(1, &uSrcStAddr, &oPost);
	GLIB_Init(uSrcStAddr, uImgHSz, uImgVSz, eSrcDataFmt);
	GLIB_DrawPattern2(uImgHSz, uImgVSz);

	UART_Printf("Draw Pattern 2(Frame 1)\n");	
	POST_StartProcessing1(1, 0, &oPost);
	while (!POST_IsProcessingDone(&oPost));	
}


static void POSTT_SimpleFreeRunDmaToDma_R2R_Polling(void)
{
	u32 uGlibStAddr=0, uGlibStAddr2=0;
	u32 uBytesPerPixel=0;
	u32 uFrmCnt = 0;

	UART_Printf("[POSTT_SimpleFreeRunDmaToDma_R2R_Polling]\n");

	uSimpleTest = 1;
	POSTT_SetCscTypeDmaToDma_R2R();

	bIsPollingOrInt = TRUE; // Polling

	//LCD_InitLDI(MAIN);
#ifdef LCD_INIT_TEST	
	LCD_InitBase();
	LCD_Start();
	LCD_InitWin(eDstDataFmt, uImgHSz, uImgVSz, 0, 0, uImgHSz, uImgVSz, 0, 0, uLcdStAddr, WIN0, false);
#else
	LCD_InitDISPC(eDstDataFmt, uLcdStAddr, WIN0, false);
	LCD_Start();
#endif
	LCD_SetWinOnOff(1, WIN0);
	LCD_GetFrmSz(&uLcdHSz, &uLcdVSz, WIN0);

	//uBytesPerPixel = (eDstDataFmt == RGB16) ? 2: 4;
	uBytesPerPixel = 4;
	uGlibStAddr = uLcdStAddr + uLcdHSz*uLcdVSz*uBytesPerPixel;
	uGlibStAddr2 = uGlibStAddr + uLcdHSz*uLcdVSz*uBytesPerPixel;

	GLIB_Init(uGlibStAddr, uLcdHSz, uLcdVSz, eSrcDataFmt);
	GLIB_DrawPattern(uLcdHSz, uLcdVSz);

	GLIB_Init(uGlibStAddr2, uLcdHSz, uLcdVSz, eSrcDataFmt);
	GLIB_DrawPattern2(uLcdHSz, uLcdVSz);

	POST_InitIpForDmaInDmaOut(	uLcdHSz, uLcdVSz, uGlibStAddr, eSrcDataFmt, 
									uLcdHSz, uLcdVSz, uLcdStAddr, eDstDataFmt,
									1, false, FREE_RUN, &oPost);
	POST_SetNextFrameStAddr(uGlibStAddr, uLcdStAddr, &oPost);

	uFrmCnt = 0;

	POST_StartProcessing(&oPost);

	while(uFrmCnt<uMaxFrameCnt)
	{
		while (!POST_IsProcessingDone(&oPost));			
		
		if (uFrmCnt%2)
			POST_SetNextFrameStAddr(uGlibStAddr2, uLcdStAddr, &oPost);
		else
			POST_SetNextFrameStAddr(uGlibStAddr, uLcdStAddr, &oPost);
		POST_ClearPending(&oPost);

		DelayfrTimer(milli, 1000);
		uFrmCnt++;
	}

	POST_StopProcessingOfFreeRun(&oPost);
	while (!POST_IsProcessingDone(&oPost));	
	POST_ClearPending(&oPost);

#if (POST_IMAGE_BMP_SLIDE)	
	POSTT_GetImageFromBMP(uGlibStAddr, uLcdHSz, uLcdVSz, 8, eSrcDataFmt);
	POSTT_GetImageFromBMP(uGlibStAddr2, uLcdHSz, uLcdVSz, 9, eSrcDataFmt);

	POST_InitIpForDmaInDmaOut(	uLcdHSz, uLcdVSz, uGlibStAddr, eSrcDataFmt, 
									uLcdHSz, uLcdVSz, uLcdStAddr, eDstDataFmt,
									1, false, FREE_RUN, &oPost);
	POST_SetNextFrameStAddr(uGlibStAddr, uLcdStAddr, &oPost);

	uFrmCnt = 0;

	POST_StartProcessing(&oPost);

	while(uFrmCnt<uMaxFrameCnt)
	{
		while (!POST_IsProcessingDone(&oPost));			
		
		if (uFrmCnt%2)
			POST_SetNextFrameStAddr(uGlibStAddr2, uLcdStAddr, &oPost);
		else
			POST_SetNextFrameStAddr(uGlibStAddr, uLcdStAddr, &oPost);
		POST_ClearPending(&oPost);

		DelayfrTimer(milli, 1000);
		uFrmCnt++;
	}

	POST_StopProcessingOfFreeRun(&oPost);
	while (!POST_IsProcessingDone(&oPost));	
	POST_ClearPending(&oPost);	
#endif	
}


void POSTT_SimpleFreeRunDmaToDma_R2R_Int(void)
{
	u32 uBytesPerPixel=0;
	u32 uPostIsrCount, uPostFrameCount;
	
	UART_Printf("[POSTT_SimpleFreeRunDmaToDma_R2R_Int]\n");

	uSimpleTest = 1;
	POSTT_SetCscTypeDmaToDma_R2R();
	
	bIsPollingOrInt = TRUE; // Int

	//LCD_InitLDI(MAIN);
#ifdef LCD_INIT_TEST	
	LCD_InitBase();
	LCD_Start();
	LCD_InitWin(eDstDataFmt, uImgHSz, uImgVSz, 0, 0, uImgHSz, uImgVSz, 0, 0, uLcdStAddr, WIN0, false);
#else
	LCD_InitDISPC(eDstDataFmt, uLcdStAddr, WIN0, false);
	LCD_Start();
#endif
	LCD_SetWinOnOff(1, WIN0);
	LCD_GetFrmSz(&uLcdHSz, &uLcdVSz, WIN0);

	//uBytesPerPixel = (eDstDataFmt == RGB16) ? 2: 4;
	uBytesPerPixel = 4;
	uNxtAddr1 = uLcdStAddr + uLcdHSz*uLcdVSz*uBytesPerPixel;
	uNxtAddr2 = uNxtAddr1 + uLcdHSz*uLcdVSz*uBytesPerPixel;

	GLIB_Init(uNxtAddr1, uLcdHSz, uLcdVSz, eSrcDataFmt);
	GLIB_DrawPattern(uLcdHSz, uLcdVSz);

	GLIB_Init(uNxtAddr2, uLcdHSz, uLcdVSz, eSrcDataFmt);
	GLIB_DrawPattern2(uLcdHSz, uLcdVSz);

	//Interrupt Setting
	uPostIsrCount = 0;
	uPostFrameCount = 0;
	uPostIntOccured = FALSE;

	INTC_Enable(NUM_POST0);
	INTC_SetVectAddr(NUM_POST0, PostISRDma);
	POST_EnableInterrupt(POST_LEVEL_INT, &oPost);

	POST_InitIpForDmaInDmaOut(	uLcdHSz, uLcdVSz, uNxtAddr1, eSrcDataFmt,
									uLcdHSz, uLcdVSz, uLcdStAddr, eDstDataFmt,
									1, false, FREE_RUN, &oPost);
	POST_SetNextFrameStAddr(uNxtAddr1, uLcdStAddr, &oPost);

	POST_StartProcessing(&oPost);

	while(uPostFrameCount < uMaxFrameCnt)
	{
		if(uPostIntOccured == TRUE)
		{
			uPostIsrCount++;
			if(uPostIsrCount > 200)
			{
				uPostFrameCount++;
				
			 	if (uPostFrameCount % 2)
					POST_SetNextFrameStAddr(uNxtAddr2, uLcdStAddr, &oPost);
				else 
					POST_SetNextFrameStAddr(uNxtAddr1, uLcdStAddr, &oPost);

				if (uPostFrameCount == uMaxFrameCnt)
				{
					POST_StopProcessingOfFreeRun(&oPost);	
					while (!POST_IsProcessingDone(&oPost));	
					uPostIntOccured = FALSE;
					break;
				}
				uPostIsrCount = 0;
			}
			uPostIntOccured = FALSE;
		}
	}

#if (POST_IMAGE_BMP_SLIDE)	
	POSTT_GetImageFromBMP(uNxtAddr1, uLcdHSz, uLcdVSz, 11, eSrcDataFmt);
	POSTT_GetImageFromBMP(uNxtAddr2, uLcdHSz, uLcdVSz, 12, eSrcDataFmt);

	POST_InitIpForDmaInDmaOut(	uLcdHSz, uLcdVSz, uNxtAddr1, eSrcDataFmt,
									uLcdHSz, uLcdVSz, uLcdStAddr, eDstDataFmt,
									1, false, FREE_RUN, &oPost);
	POST_SetNextFrameStAddr(uNxtAddr1, uLcdStAddr, &oPost);

	uPostFrameCount = 0;
	uPostIsrCount = 0;
	POST_StartProcessing(&oPost);

	while(uPostFrameCount<uMaxFrameCnt)
	{
		if(uPostIntOccured == TRUE)
		{
			uPostIsrCount++;
			if(uPostIsrCount > 200)
			{
				uPostFrameCount++;
				
			 	if (uPostFrameCount % 2)
					POST_SetNextFrameStAddr(uNxtAddr2, uLcdStAddr, &oPost);
				else 
					POST_SetNextFrameStAddr(uNxtAddr1, uLcdStAddr, &oPost);

				if (uPostFrameCount == uMaxFrameCnt)
				{
					POST_StopProcessingOfFreeRun(&oPost);	
					while (!POST_IsProcessingDone(&oPost));	
					uPostIntOccured = FALSE;
					break;
				}
				uPostIsrCount = 0;
			}
			uPostIntOccured = FALSE;
		}

	}
#endif	

	POST_DisableInterrupt(&oPost);
	POST_ClearPending(&oPost);	
	INTC_Disable(NUM_POST0);
}



void POSTT_ComplexFreeRunDmaToDma_R2R_Polling(void)
{
	u32 uGlibStAddr, uGlibStAddr2;
	u32 uBytesPerPixel, uFrmCnt;
	
	UART_Printf("[POSTT_ComplexFreeRunDmaToDma_R2R_Polling]\n");

	uSimpleTest = 0;
	POSTT_SetCscTypeDmaToDma_R2R();

	bIsPollingOrInt = TRUE;
	//bIsR2Y2RFlag = FALSE;	
	ePostRunMode = FREE_RUN;
	
	//LCD_InitLDI(MAIN);
#ifdef LCD_INIT_TEST	
	LCD_InitBase();
	LCD_Start();
	LCD_InitWin(eDstDataFmt, uImgHSz, uImgVSz, 0, 0, uImgHSz, uImgVSz, 0, 0, uLcdStAddr, WIN0, false);
#else
	LCD_InitDISPC(eDstDataFmt, uLcdStAddr, WIN0, false);
	LCD_Start();
#endif
	LCD_SetWinOnOff(1, WIN0);
	LCD_GetFrmSz(&uLcdHSz, &uLcdVSz, WIN0);

	// To Clear the LCD Display
	GLIB_Init(uLcdStAddr, uLcdHSz, uLcdVSz, eDstDataFmt);
	GLIB_ClearFrame(C_BLACK);

	uBytesPerPixel = (eDstDataFmt == RGB16) ? 2: 4;
	uGlibStAddr = uLcdStAddr + uLcdHSz*uLcdVSz*uBytesPerPixel;
	uGlibStAddr2 = uGlibStAddr + uImgHSz*uImgVSz*uBytesPerPixel;

	GLIB_Init(uGlibStAddr, uImgHSz, uImgVSz, eSrcDataFmt);
	GLIB_DrawPattern(uImgHSz, uImgVSz);

	GLIB_Init(uGlibStAddr2, uImgHSz, uImgVSz, eSrcDataFmt);
	GLIB_DrawPattern2(uImgHSz, uImgVSz);

	POST_InitIp1(	uImgHSz, uImgVSz, uSrcStartX, uSrcStartY, uSrcCroppedHSz, uSrcCroppedVSz, uGlibStAddr, eSrcDataFmt,
					uLcdHSz, uLcdVSz, uDstStartX, uDstStartY, uDstScaledHSz, uDstScaledVSz, uLcdStAddr, eDstDataFmt, 
					1, false, ePostRunMode, POST_DMA, POST_DMA, &oPost); // src. frm. buf. num: 1, double buffering: false

	POST_SetNextFrameStAddr(uGlibStAddr2, uLcdStAddr, &oPost);
	POST_StartProcessing(&oPost);

	uFrmCnt = 0;

	while(uFrmCnt<uMaxFrameCnt)
	{
		while (!POST_IsProcessingDone(&oPost));			
		
		if (uFrmCnt%2)
			POST_SetNextFrameStAddr(uGlibStAddr, uLcdStAddr, &oPost);
		else
			POST_SetNextFrameStAddr(uGlibStAddr2, uLcdStAddr, &oPost);
		POST_ClearPending(&oPost);

		DelayfrTimer(milli, 500);
		uFrmCnt++;
	}

	POST_StopProcessingOfFreeRun(&oPost);	
	while (!POST_IsProcessingDone(&oPost));	
}


void POSTT_ComplexFreeRunDmaToDma_R2R_Int(void)
{
	u32 uBytesPerPixel=0;
	u32 uPostIsrCount, uPostFrameCount;

	UART_Printf("[POSTT_ComplexFreeRunDmaToDma_R2R_Int]\n");

	uSimpleTest = 0;
	POSTT_SetCscTypeDmaToDma_R2R();
	
	bIsPollingOrInt = false;	
	//bIsR2Y2RFlag = false;	
	ePostRunMode = FREE_RUN;
	
	//LCD_InitLDI(MAIN);
	
#ifdef LCD_INIT_TEST	
	LCD_InitBase();
	LCD_Start();
	LCD_InitWin(eDstDataFmt, uImgHSz, uImgVSz, 0, 0, uImgHSz, uImgVSz, 0, 0, uLcdStAddr, WIN0, false);
#else
	LCD_InitDISPC(eDstDataFmt, uLcdStAddr, WIN0, false);
	LCD_Start();
#endif

	LCD_SetWinOnOff(1, WIN0);

	LCD_GetFrmSz(&uLcdHSz, &uLcdVSz, WIN0);

	// To Clear the LCD Display
	GLIB_Init(uLcdStAddr, uLcdHSz, uLcdVSz, eDstDataFmt);
	GLIB_ClearFrame(C_BLACK);

	uBytesPerPixel = (eDstDataFmt == RGB16) ? 2: 4;

	uNxtAddr1 = uLcdStAddr + uLcdHSz*uLcdVSz*uBytesPerPixel;
	uNxtAddr2 = uNxtAddr1 + uImgHSz*uImgVSz*uBytesPerPixel;

	GLIB_Init(uNxtAddr1, uImgHSz, uImgVSz, eSrcDataFmt);
	GLIB_DrawPattern(uImgHSz, uImgVSz);

	GLIB_Init(uNxtAddr2, uImgHSz, uImgVSz, eSrcDataFmt);
	GLIB_DrawPattern2(uImgHSz, uImgVSz);

	uPostIsrCount = 0;
	uPostFrameCount = 0;
	uPostIntOccured = FALSE;
	//INTC_InitIp();
	//INTC_SetHandlerAndUnmask(INT_POST, PostISRDma);
	INTC_Enable(NUM_POST0);
	INTC_SetVectAddr(NUM_POST0, PostISRDma);
	POST_EnableInterrupt(POST_LEVEL_INT, &oPost);

	POST_InitIp1(	uImgHSz, uImgVSz, uSrcStartX, uSrcStartY, uSrcCroppedHSz, uSrcCroppedVSz, uNxtAddr1, eSrcDataFmt,
					uLcdHSz, uLcdVSz, uDstStartX, uDstStartY, uDstScaledHSz, uDstScaledVSz, uLcdStAddr, eDstDataFmt,
					1, false, ePostRunMode, POST_DMA, POST_DMA, &oPost);

	POST_SetNextFrameStAddr(uNxtAddr2, uLcdStAddr, &oPost);

	POST_StartProcessing(&oPost);

	while(uPostFrameCount < uMaxFrameCnt)
	{
		if(uPostIntOccured == TRUE)
		{
			uPostIsrCount++;
			if(uPostIsrCount == 400)
			{
				uPostFrameCount++;
				
			 	if (uPostFrameCount % 2)
					POST_SetNextFrameStAddr(uNxtAddr2, uLcdStAddr, &oPost);
				else 
					POST_SetNextFrameStAddr(uNxtAddr1, uLcdStAddr, &oPost);

				if (uPostFrameCount == uMaxFrameCnt)
				{
					POST_StopProcessingOfFreeRun(&oPost);	
					while (!POST_IsProcessingDone(&oPost));	
					uPostIntOccured = FALSE;
					break;
				}
				uPostIsrCount = 0;
			}
			uPostIntOccured = FALSE;
		}
	}

	POST_DisableInterrupt(&oPost);
	POST_ClearPending(&oPost);	
	//INTC_Mask(INT_POST);
	INTC_Disable(NUM_POST0);
}


void POSTT_ScaleUpDown_RGB(void)
{
	u32 uGlibStAddr, uGlibStAddr1;
	u32 uBytesPerPixel;
	u32 uScaleOffsetX, uScaleOffsetY;
	u8 uScale;
	u32 uHOffset, uVOffset, uPreHOffset, uPreVOffset, uHRatio, uVRatio;
	s32 nSelDmaCscType_R2R;	

	u32 i;
	u16 *pBufAddr16;
	u32 *pBufAddr32;
	
	UART_Printf("[POSTT_ScaleUpDown_RGB]\n");

	while(1)
	{
		UART_Printf("\n");
		UART_Printf("[1] RGB16\n");
		UART_Printf("[2] RGB24\n");
		UART_Printf("\n");

		UART_Printf(">>Select The Source Image Type: ");

		nSelDmaCscType_R2R = UART_GetIntNum();

		if (nSelDmaCscType_R2R == 1 || nSelDmaCscType_R2R == 2 )
		{
			eSrcDataFmt = (nSelDmaCscType_R2R == 1) ? RGB16 : RGB24;
			eDstDataFmt = (nSelDmaCscType_R2R == 2) ? RGB16 : RGB24;
			break;
		}
		else 
			UART_Printf("Invalid Input! Retry It!!\n");
		
	}

	//bIsR2Y2RFlag = false;
	ePostRunMode = ONE_SHOT;

	//LCD_InitLDI(MAIN);
	LCD_InitDISPC(eSrcDataFmt, uLcdStAddr, WIN0, false);
	LCD_Start();
	LCD_SetWinOnOff(1, WIN0);
	LCD_GetFrmSz(&uLcdHSz, &uLcdVSz, WIN0);

	uImgHSz = uLcdHSz;	
	uImgVSz = uLcdVSz;
	uSrcStartX = 0; 
	uSrcStartY = 0;
	uSrcCroppedHSz = uLcdHSz;	
	uSrcCroppedVSz = uLcdVSz;
	
	uMidImgHSz = 2048;
	uMidImgVSz = 2048;
	uMidStartX = 0;
	uMidStartY = 0;
	uMidScaledHSz = 2048;
	uMidScaledVSz = 2048;
		
	uDstStartX = 0; 
	uDstStartY = 0;
	uDstScaledHSz = uLcdHSz, 
	uDstScaledVSz = uLcdVSz;	
	
	// To Clear the LCD Display
	GLIB_Init(uLcdStAddr, uLcdHSz, uLcdVSz, eSrcDataFmt);
	GLIB_ClearFrame(C_BLACK);

	uBytesPerPixel = (eSrcDataFmt == RGB16) ? 2: 4;
	uGlibStAddr = uLcdStAddr + uLcdHSz*uLcdVSz*uBytesPerPixel;
	uGlibStAddr1 = uGlibStAddr + uMidImgHSz*uMidImgVSz*uBytesPerPixel;
	
	//uSrcStAddr=0;

	// CSC & Scaling the DrawPattern 1

	//GLIB_Init(uGlibStAddr, uImgHSz, uImgVSz, eSrcDataFmt);
	//GLIB_DrawPattern(uImgHSz, uImgVSz);
	POSTT_GetImageFromBMP(uGlibStAddr, uLcdHSz, uLcdVSz, 11, eSrcDataFmt);

#if 0
	switch(eSrcDataFmt)
	{

		case RGB16 : 
					pBufAddr16 = (u16 *)uGlibStAddr;
					
					for(i=0 ; i<uImgHSz*uImgVSz ; i++)
						*pBufAddr16++ = pBgcolors216[i];
						
					break;
							
		case RGB24 : 
					pBufAddr32 = (u32 *)uGlibStAddr;
					
					for(i=0 ; i<uImgHSz*uImgVSz ; i++)
						*pBufAddr32++ = pSmile_2[i];
						
					break;
	}
#endif


	uScaleOffsetX = uMidImgHSz/2;
	uScaleOffsetY = uMidImgVSz/2;
	// generation mid image
	POST_InitIp1(	uImgHSz, uImgVSz, uSrcStartX, uSrcStartY, uSrcCroppedHSz, uSrcCroppedVSz, uGlibStAddr, eSrcDataFmt,
					uMidImgHSz, uMidImgVSz, uMidStartX, uMidStartY, uMidScaledHSz, uMidScaledVSz, uGlibStAddr1, eSrcDataFmt,
					1, false, ePostRunMode, POST_DMA, POST_DMA, &oPost);
	POST_StartProcessing1(0, 0, &oPost);
	while (!POST_IsProcessingDone(&oPost));

	// default display image
	POST_InitIp1(	uMidImgHSz, uMidImgVSz, uMidStartX, uMidStartY, uMidScaledHSz-uScaleOffsetX, uMidScaledVSz-uScaleOffsetY, uGlibStAddr1, eSrcDataFmt,
					uLcdHSz, uLcdVSz, uDstStartX, uDstStartY, uDstScaledHSz, uDstScaledVSz, uLcdStAddr, eSrcDataFmt,
					1, false, ePostRunMode, POST_DMA, POST_DMA, &oPost);
	POST_StartProcessing1(0, 0, &oPost);
	while (!POST_IsProcessingDone(&oPost));

	UART_Printf("=============================================\n");
	UART_Printf("'w' : Vertical Scale-up,  'x' : Vertical Scale-down\n");
	UART_Printf("'d' : Horizontal Scale-up,  'a' : Horizontal Scale-down\n");
	UART_Printf("=============================================\n");
	UART_Printf("Press 'q' key to exit\n");	

	//RGB24 -> Width : multiple of 1,   RGB16 -> Width : multiple of 2
	while((uScale=UART_Getc()) != 'q')
	{
		switch (uScale)
		{
			//Vetical Scale Up
			case 'w' :
			case 'W' :	POST_CalcurateScaleOffset(	uMidScaledHSz-uScaleOffsetX-uPreHOffset, uMidScaledVSz-uScaleOffsetY-uPreVOffset, uDstScaledHSz, uDstScaledVSz, &oPost, 
													&uHOffset, &uVOffset, &uHRatio, &uVRatio, eSrcDataFmt);
						if(uPreVOffset == uVOffset)
						{
							if(uScaleOffsetY < (uMidImgVSz-uPreVOffset))
								 uScaleOffsetY += uPreVOffset;
						}
						else
						{	
							if(uScaleOffsetY < (uMidImgVSz-uVOffset))
								uScaleOffsetY += uVOffset - (uScaleOffsetY%(uVOffset));
							uPreVOffset = uVOffset;
						}
						break;
			//Vetical Scale Down
			case 'x' :
			case 'X' :	POST_CalcurateScaleOffset(	uMidScaledHSz-uScaleOffsetX+uPreHOffset, uMidScaledVSz-uScaleOffsetY+uPreVOffset, uDstScaledHSz, uDstScaledVSz, &oPost, 
													&uHOffset, &uVOffset, &uHRatio, &uVRatio, eSrcDataFmt);
						if(uPreVOffset == uVOffset)
						{
							if(uScaleOffsetY >= uPreVOffset)
								 uScaleOffsetY -= uPreVOffset;
						}
						else
						{	
							if(uScaleOffsetY >= uVOffset)
								uScaleOffsetY -= uVOffset - (uScaleOffsetY%(uVOffset));
							uPreVOffset = uVOffset;
						}
						break;	
			//Horizontal Scale Up	
			case 'd' :
			case 'D' :	POST_CalcurateScaleOffset(	uMidScaledHSz-uScaleOffsetX-uPreHOffset, uMidScaledVSz-uScaleOffsetY-uPreVOffset, uDstScaledHSz, uDstScaledVSz, &oPost, 
													&uHOffset, &uVOffset, &uHRatio, &uVRatio, eSrcDataFmt);
						if(uPreHOffset == uHOffset)
						{
							if(uScaleOffsetX < (uMidImgHSz-uPreHOffset))
								 uScaleOffsetX += uPreHOffset;
						}
						else
						{	
							if(uScaleOffsetX < (uMidImgHSz-uHOffset))
								uScaleOffsetX += uHOffset - (uScaleOffsetX%(uHOffset));
							uPreHOffset = uHOffset;
						}
						break;	
			//Horizontal Scale Down						
			case 'a' :
			case 'A' :	POST_CalcurateScaleOffset(	uMidScaledHSz-uScaleOffsetX+uPreHOffset, uMidScaledVSz-uScaleOffsetY+uPreVOffset, uDstScaledHSz, uDstScaledVSz, &oPost, 
													&uHOffset, &uVOffset, &uHRatio, &uVRatio, eSrcDataFmt);
						if(uPreHOffset == uHOffset)
						{
							if(uScaleOffsetX >= uPreHOffset)
								 uScaleOffsetX -= uPreHOffset;
						}
						else
						{	
							if(uScaleOffsetX >= uHOffset)
								uScaleOffsetX -= uHOffset - (uScaleOffsetX%(uHOffset));
							uPreHOffset = uHOffset;
						}
			default : 	break;
		}

		UART_Printf("(%d*%d) -> (%d*%d)\n",uMidScaledHSz-uScaleOffsetX, uMidScaledVSz-uScaleOffsetY, uDstScaledHSz, uDstScaledVSz);
		
		POST_InitIp1(	uMidImgHSz, uMidImgVSz, uMidStartX, uMidStartY, uMidScaledHSz-uScaleOffsetX, uMidScaledVSz-uScaleOffsetY, uGlibStAddr1, eSrcDataFmt,
						uLcdHSz, uLcdVSz, uDstStartX, uDstStartY, uDstScaledHSz, uDstScaledVSz, uLcdStAddr, eSrcDataFmt,
						1, false, ePostRunMode, POST_DMA, POST_DMA, &oPost);
		POST_StartProcessing1(0, 0, &oPost);
		while (!POST_IsProcessingDone(&oPost));		
	}
	
}





const testFuncMenu r2r_menu[] =
{
	POSTT_SimpleDmaToDma_R2R,                        			"Simple DMA CSC Test From RGB To RGB",
	POSTT_ComplexIDmaToDma_R2R,                       		"Complex DMA CSC Test From RGB To RGB",
	POSTT_SimpleFreeRunDmaToDma_R2R_Polling,          	"Simple FreeRun And CSC Test From RGB To RGB By Using Polling",
	POSTT_SimpleFreeRunDmaToDma_R2R_Int,              	"Simple FreeRun And CSC Test From RGB To RGB By Using Interrupt", 		
	POSTT_ComplexFreeRunDmaToDma_R2R_Polling, 		"Complex FreeRun And CSC Test From RGB To RGB By Using Polling",
	POSTT_ComplexFreeRunDmaToDma_R2R_Int,             	"Complex FreeRun And CSC Test From RGB To RGB By Using Interrupt",		
	POSTT_ScaleUpDown_RGB,							"Scale Up/Down RGB",
	0,                                          0
};


static void POSTT_ConvertRGBToRGB(void)
{
	u32 i; 
	s32 uSel;
	
	UART_Printf("[POSTT_ConvertRGBToRGB]\n");
	
	while(1)
	{
		UART_Printf("\n");
		for (i=0; (int)(r2r_menu[i].desc)!=0; i++)
		{
			UART_Printf("%2d: %s\n", i, r2r_menu[i].desc);
		}

		UART_Printf("\n");
		UART_Printf("Select The Function To Test: ");

		uSel = UART_GetIntNum();
		UART_Printf("\n");

		if (uSel == -1)
			break;
		if (uSel>=0 && uSel<(sizeof(r2r_menu)/8-1))
			(r2r_menu[uSel].func)();
	}
}


////////////////////////////////////////////////////////////////////////////
//////////////////                    RGB to YUV to RGB                    ///////////////////
////////////////////////////////////////////////////////////////////////////


static void POSTT_SetCscTypeDmaToDma_R2Y2R(void)
{
	int nSelDmaCscType_R2Y2R;

	while(1)
	{
		UART_Printf("\n");		
		UART_Printf(" [1] RGB16 -> YCbYCr422_Intlv And YCbYCr422_Intlv -> RGB16\n");
		UART_Printf(" [2] RGB16 -> YCrYCb422_Intlv And YCrYCb422_Intlv -> RGB16\n");		
		UART_Printf(" [3] RGB16 -> CbYCrY422_Intlv And CbYCrY422_Intlv -> RGB16\n");	
		UART_Printf(" [4] RGB16 -> CrYCbY422_Intlv And CrYCbY422_Intlv -> RGB16\n");
		UART_Printf(" [5] RGB16 -> YC420 And YC420 -> RGB16\n");
		UART_Printf("\n");
		UART_Printf(" [6] RGB24 -> YCbYCr422_Intlv And YCbYCr422_Intlv -> RGB24\n");
		UART_Printf(" [7] RGB24 -> YCrYCb422_Intlv And YCrYCb422_Intlv -> RGB24\n");		
		UART_Printf(" [8] RGB24 -> CbYCrY422_Intlv And CbYCrY422_Intlv -> RGB24\n");	
		UART_Printf(" [9] RGB24 -> CrYCbY422_Intlv And CrYCbY422_Intlv -> RGB24\n");
		UART_Printf("[10] RGB24 -> YC420 And YC420 -> RGB24\n");
		UART_Printf("\n");
		UART_Printf("[11] RGB16 -> YCbYCr422_Intlv And YCbYCr422_Intlv -> RGB24\n");
		UART_Printf("[12] RGB16 -> YCrYCb422_Intlv And YCrYCb422_Intlv -> RGB24\n");		
		UART_Printf("[13] RGB16 -> CbYCrY422_Intlv And CbYCrY422_Intlv -> RGB24\n");	
		UART_Printf("[14] RGB16 -> CrYCbY422_Intlv And CrYCbY422_Intlv -> RGB24\n");
		UART_Printf("[15] RGB16 -> YC420 And YC420 -> RGB24\n");
		UART_Printf("\n");
		UART_Printf("[16] RGB24 -> YCbYCr422_Intlv And YCbYCr422_Intlv -> RGB16\n");
		UART_Printf("[17] RGB24 -> YCrYCb422_Intlv And YCrYCb422_Intlv -> RGB16\n");		
		UART_Printf("[18] RGB24 -> CbYCrY422_Intlv And CbYCrY422_Intlv -> RGB16\n");	
		UART_Printf("[19] RGB24 -> CrYCbY422_Intlv And CrYCbY422_Intlv -> RGB16\n");
		UART_Printf("[20] RGB24 -> YC420 And YC420 -> RGB16\n");
		UART_Printf("\n");

		UART_Printf(">> Select The Color Space Conversion Type: ");
		nSelDmaCscType_R2Y2R = UART_GetIntNum();

//		if(	nSelDmaCscType_R2Y2R == 1 || nSelDmaCscType_R2Y2R == 2 || nSelDmaCscType_R2Y2R == 3 || nSelDmaCscType_R2Y2R == 4 ||
//			nSelDmaCscType_R2Y2R == 5 || nSelDmaCscType_R2Y2R == 6 || nSelDmaCscType_R2Y2R == 7 || nSelDmaCscType_R2Y2R == 8 ||
//			nSelDmaCscType_R2Y2R == 9 || nSelDmaCscType_R2Y2R == 10 || nSelDmaCscType_R2Y2R == 11 || nSelDmaCscType_R2Y2R == 12 ||
//			nSelDmaCscType_R2Y2R == 13 || nSelDmaCscType_R2Y2R == 14 || nSelDmaCscType_R2Y2R == 15 || nSelDmaCscType_R2Y2R == 16 ||
//			nSelDmaCscType_R2Y2R == 17 || nSelDmaCscType_R2Y2R == 18 || nSelDmaCscType_R2Y2R == 19 || nSelDmaCscType_R2Y2R == 20)
		if( (nSelDmaCscType_R2Y2R >= 1) && (nSelDmaCscType_R2Y2R <=20) )
		{
			eSrcDataFmt =
				(nSelDmaCscType_R2Y2R == 1 || nSelDmaCscType_R2Y2R == 2 || nSelDmaCscType_R2Y2R == 3 ||
				nSelDmaCscType_R2Y2R == 4 || nSelDmaCscType_R2Y2R == 5 || nSelDmaCscType_R2Y2R == 11 || 
				nSelDmaCscType_R2Y2R == 12 || nSelDmaCscType_R2Y2R == 13 || nSelDmaCscType_R2Y2R == 14 ||
				nSelDmaCscType_R2Y2R == 15) ? RGB16 : RGB24;

			eMidDataFmt =
				(nSelDmaCscType_R2Y2R == 1 || nSelDmaCscType_R2Y2R == 6 ||
				nSelDmaCscType_R2Y2R == 11 || nSelDmaCscType_R2Y2R == 16 ) ? YCBYCR :
				(nSelDmaCscType_R2Y2R == 2 || nSelDmaCscType_R2Y2R == 7 ||
				nSelDmaCscType_R2Y2R == 12 || nSelDmaCscType_R2Y2R == 17 ) ? YCRYCB :
				(nSelDmaCscType_R2Y2R == 3 || nSelDmaCscType_R2Y2R == 8 ||
				nSelDmaCscType_R2Y2R == 13 || nSelDmaCscType_R2Y2R == 18 ) ? CBYCRY :
				(nSelDmaCscType_R2Y2R == 4 || nSelDmaCscType_R2Y2R == 9 ||
				nSelDmaCscType_R2Y2R == 14 || nSelDmaCscType_R2Y2R == 19 ) ? CRYCBY : YC420;

			eDstDataFmt =
				(nSelDmaCscType_R2Y2R == 1 || nSelDmaCscType_R2Y2R == 2 || nSelDmaCscType_R2Y2R == 3 ||
				nSelDmaCscType_R2Y2R == 4 || nSelDmaCscType_R2Y2R == 5 || nSelDmaCscType_R2Y2R == 16 || 
				nSelDmaCscType_R2Y2R == 17 || nSelDmaCscType_R2Y2R == 18 || nSelDmaCscType_R2Y2R == 19 ||
				nSelDmaCscType_R2Y2R == 20) ? RGB16 : RGB24;

			break;
		}
		else
			UART_Printf("Invalid Input! Retry It!!\n");
		
	}
	
	if (eSrcDataFmt == RGB16 && eMidDataFmt == YCBYCR && eDstDataFmt == RGB16)
	{
		uImgHSz = 240, uImgVSz = 320, uSrcStartX = 20, uSrcStartY = 20;
		uSrcCroppedHSz = 160, uSrcCroppedVSz = 240;

		uMidImgHSz = 240, uMidImgVSz = 320, uMidStartX = 60, uMidStartY = 40;
		uMidScaledHSz = 120, uMidScaledVSz = 120;

		uDstStartX = 120, 	uDstStartY = 100, uDstScaledHSz = 104, uDstScaledVSz = 140;		
	}
	else if (eSrcDataFmt == RGB16 && eMidDataFmt == YCRYCB && eDstDataFmt == RGB16)
	{

		uImgHSz = 120, 	uImgVSz = 160, uSrcStartX = 0, uSrcStartY = 40;
		uSrcCroppedHSz = 120, 	uSrcCroppedVSz = 100;

		uMidImgHSz = 240, uMidImgVSz = 320, uMidStartX = 0, uMidStartY = 40;
		uMidScaledHSz = 120, uMidScaledVSz = 120;

		uDstStartX = 0, uDstStartY = 50, uDstScaledHSz = 120, uDstScaledVSz = 110;//160;		
	}
	else if (eSrcDataFmt == RGB16 && eMidDataFmt == CBYCRY && eDstDataFmt == RGB16)	
	{
		uImgHSz = 360, 	uImgVSz = 480,	uSrcStartX = 80, uSrcStartY = 120;
		uSrcCroppedHSz = 104, 	uSrcCroppedVSz = 100;

		uMidImgHSz = 240, uMidImgVSz = 320, uMidStartX = 20, uMidStartY = 40;
		uMidScaledHSz = 160, uMidScaledVSz = 120;

		uDstStartX = 120, uDstStartY = 80, uDstScaledHSz = 80, uDstScaledVSz = 160;		
	}
	else if (eSrcDataFmt == RGB16 && eMidDataFmt == CRYCBY && eDstDataFmt == RGB16)
	{
		uImgHSz = 240, 	uImgVSz = 320, uSrcStartX = 0, uSrcStartY = 20;
		uSrcCroppedHSz = 160, 	uSrcCroppedVSz = 200;

		uMidImgHSz = 120, uMidImgVSz = 160 ,uMidStartX = 10, uMidStartY = 30;
		uMidScaledHSz = 80, uMidScaledVSz = 100;

		uDstStartX = 50, uDstStartY = 60, uDstScaledHSz = 176 ,uDstScaledVSz = 180;		
	}
	else if (eSrcDataFmt == RGB16 && eMidDataFmt == YC420 && eDstDataFmt == RGB16)
	{
		uImgHSz = 240,	uImgVSz = 320, uSrcStartX = 20, uSrcStartY = 20;
		uSrcCroppedHSz = 160, 	uSrcCroppedVSz = 200;

		uMidImgHSz = 120, uMidImgVSz = 160, uMidStartX = 0, uMidStartY = 30;
		uMidScaledHSz = 48, uMidScaledVSz = 100;

		uDstStartX = 50, uDstStartY = 40, uDstScaledHSz = 160, uDstScaledVSz = 200;		
	}
	else if (eSrcDataFmt == RGB24 && eMidDataFmt == YCBYCR && eDstDataFmt == RGB24)
	{
		uImgHSz = 240, 	uImgVSz = 320, uSrcStartX = 20, uSrcStartY = 10;
		uSrcCroppedHSz = 128, 	uSrcCroppedVSz = 128;

		uMidImgHSz = 360, uMidImgVSz = 480, uMidStartX = 60, uMidStartY = 40;
		uMidScaledHSz = 240, uMidScaledVSz = 240;

		uDstStartX = 20, uDstStartY = 80, uDstScaledHSz = 160, uDstScaledVSz = 160;		
	}
	else if (eSrcDataFmt == RGB24 && eMidDataFmt == YCRYCB&& eDstDataFmt == RGB24)
	{
		uImgHSz = 120, 	uImgVSz = 160, uSrcStartX = 20, uSrcStartY = 10;
		uSrcCroppedHSz = 80, uSrcCroppedVSz = 120;

		uMidImgHSz = 240, uMidImgVSz = 320, uMidStartX = 120, uMidStartY = 120;
		uMidScaledHSz = 80, uMidScaledVSz = 160;

		uDstStartX = 20, uDstStartY = 40, uDstScaledHSz = 160, uDstScaledVSz = 200;		
	}
	else if (eSrcDataFmt == RGB24 && eMidDataFmt == CBYCRY&& eDstDataFmt == RGB24)
	{

		uImgHSz = 360, uImgVSz = 480, uSrcStartX = 0, uSrcStartY = 0;
		uSrcCroppedHSz = 360, 	uSrcCroppedVSz = 480;

		uMidImgHSz = 360, uMidImgVSz = 480, uMidStartX = 0, uMidStartY = 0;
		uMidScaledHSz = 360, uMidScaledVSz = 480;

		uDstStartX = 0, uDstStartY = 0, uDstScaledHSz = 320, uDstScaledVSz = 240;		
	}
	else if (eSrcDataFmt == RGB24 && eMidDataFmt == CRYCBY&& eDstDataFmt == RGB24)
	{
		uImgHSz = 240, uImgVSz = 320, uSrcStartX = 80, uSrcStartY = 80;
		uSrcCroppedHSz = 104, uSrcCroppedVSz = 120;

		uMidImgHSz = 360, uMidImgVSz = 480, uMidStartX = 180, uMidStartY = 180;
		uMidScaledHSz = 120, uMidScaledVSz = 240;

		uDstStartX = 20, uDstStartY = 80, uDstScaledHSz = 80, uDstScaledVSz = 160;		
	}
	else if (eSrcDataFmt == RGB24 && eMidDataFmt == YC420 && eDstDataFmt == RGB24)
	{
		uImgHSz = 120, 	uImgVSz = 160, uSrcStartX = 20, uSrcStartY = 80;
		uSrcCroppedHSz = 72, uSrcCroppedVSz = 72;

		uMidImgHSz = 360, uMidImgVSz = 480, uMidStartX = 24, uMidStartY = 20;
		uMidScaledHSz = 320, uMidScaledVSz = 400;

		uDstStartX = 80, uDstStartY = 100, uDstScaledHSz = 120, uDstScaledVSz = 120;		
	}
	else if (eSrcDataFmt == RGB16 && eMidDataFmt == YCBYCR && eDstDataFmt == RGB24)
	{
		uImgHSz = 240, 	uImgVSz = 320, uSrcStartX = 160, 	uSrcStartY = 200;
		uSrcCroppedHSz = 56, uSrcCroppedVSz = 120;

		uMidImgHSz = 120, uMidImgVSz = 160, uMidStartX = 20, uMidStartY = 40;
		uMidScaledHSz = 80, uMidScaledVSz = 80;

		uDstStartX = 20, uDstStartY = 20, uDstScaledHSz = 200, uDstScaledVSz = 220;		
	}
	else if (eSrcDataFmt == RGB16 && eMidDataFmt == YCRYCB && eDstDataFmt == RGB24)
	{
		uImgHSz = 120, 	uImgVSz = 160, uSrcStartX = 20, uSrcStartY = 20;
		uSrcCroppedHSz = 88, uSrcCroppedVSz = 120;

		uMidImgHSz = 120, uMidImgVSz = 480, uMidStartX = 0, uMidStartY = 40;
		uMidScaledHSz = 88, uMidScaledVSz = 280;

		uDstStartX = 10, uDstStartY = 10, uDstScaledHSz = 200,	uDstScaledVSz = 230;		
	}
	else if (eSrcDataFmt == RGB16 && eMidDataFmt == CBYCRY && eDstDataFmt == RGB24)
	{
		uImgHSz = 360, 	uImgVSz = 480, uSrcStartX = 180, 	uSrcStartY = 240;
		uSrcCroppedHSz = 120, 	uSrcCroppedVSz = 240;

		uMidImgHSz = 240, uMidImgVSz = 160, uMidStartX = 20, uMidStartY = 40;
		uMidScaledHSz = 144, uMidScaledVSz = 120;

		uDstStartX = 10, uDstStartY = 10, uDstScaledHSz = 104, uDstScaledVSz = 104;		
	}
	else if (eSrcDataFmt == RGB16 && eMidDataFmt == CRYCBY && eDstDataFmt == RGB24)
	{
		uImgHSz = 240, 	uImgVSz = 320, uSrcStartX = 40, uSrcStartY = 50;
		uSrcCroppedHSz = 184, 	uSrcCroppedVSz = 120;

		uMidImgHSz = 120, uMidImgVSz = 480, uMidStartX = 20, uMidStartY = 40;
		uMidScaledHSz = 88, uMidScaledVSz = 400;

		uDstStartX = 10, uDstStartY = 10, uDstScaledHSz = 160, uDstScaledVSz = 230;		
	}
	else if (eSrcDataFmt == RGB16 && eMidDataFmt == YC420 && eDstDataFmt == RGB24)
	{
		uImgHSz = 120, 	uImgVSz = 160, uSrcStartX = 40, uSrcStartY = 50;
		uSrcCroppedHSz = 40, uSrcCroppedVSz = 88;

		uMidImgHSz = 360, uMidImgVSz = 480, uMidStartX = 56, uMidStartY = 10;
		uMidScaledHSz = 184, uMidScaledVSz = 240;

		uDstStartX = 10, uDstStartY = 10, uDstScaledHSz = 160, uDstScaledVSz = 160;		
	}
	else if (eSrcDataFmt == RGB24 && eMidDataFmt == YCBYCR && eDstDataFmt == RGB16)
	{
		uImgHSz = 240, uImgVSz = 320, uSrcStartX = 60, uSrcStartY = 200;
		uSrcCroppedHSz = 112, 	uSrcCroppedVSz = 96;

		uMidImgHSz = 120, uMidImgVSz = 160, uMidStartX = 20, uMidStartY = 40;
		uMidScaledHSz = 88, uMidScaledVSz = 96;

		uDstStartX = 60, uDstStartY = 80, uDstScaledHSz = 136, uDstScaledVSz = 160;		
	}
	else if (eSrcDataFmt == RGB24 && eMidDataFmt == YCRYCB&& eDstDataFmt == RGB16)
	{
		uImgHSz = 240, uImgVSz = 320, uSrcStartX = 180, uSrcStartY = 60;
		uSrcCroppedHSz = 48, uSrcCroppedVSz = 240;

		uMidImgHSz = 240, uMidImgVSz = 480, uMidStartX = 20, uMidStartY = 40;
		uMidScaledHSz = 184, uMidScaledVSz = 240;

		uDstStartX = 40, uDstStartY = 40, uDstScaledHSz = 184, uDstScaledVSz = 80;		
	}
	else if (eSrcDataFmt == RGB24 && eMidDataFmt == CBYCRY && eDstDataFmt == RGB16)
	{
		uImgHSz = 240, uImgVSz = 320, uSrcStartX = 180, uSrcStartY = 60;
		uSrcCroppedHSz = 48, uSrcCroppedVSz = 240;

		uMidImgHSz = 240, uMidImgVSz = 480, uMidStartX = 20, uMidStartY = 40;
		uMidScaledHSz = 184, uMidScaledVSz = 240;

		uDstStartX = 40, uDstStartY = 40, uDstScaledHSz = 184, uDstScaledVSz = 80;		
	}
	else if (eSrcDataFmt == RGB24 && eMidDataFmt == CRYCBY && eDstDataFmt == RGB16)
	{
		uImgHSz = 360, uImgVSz = 480, uSrcStartX = 40, uSrcStartY = 50;
		uSrcCroppedHSz = 184, 	uSrcCroppedVSz = 120;

		uMidImgHSz = 120, uMidImgVSz = 480, uMidStartX = 20, uMidStartY = 40;
		uMidScaledHSz = 80, uMidScaledVSz = 240;

		uDstStartX = 10, uDstStartY = 10, uDstScaledHSz = 160, uDstScaledVSz = 230;		
		
	}
	else if (eSrcDataFmt == RGB24 && eMidDataFmt == YC420 && eDstDataFmt == RGB16)
	{
		uImgHSz = 240, uImgVSz = 320, uSrcStartX = 120, uSrcStartY = 120;
		uSrcCroppedHSz = 120, 	uSrcCroppedVSz = 120;

		uMidImgHSz = 120, uMidImgVSz = 160, uMidStartX = 24, uMidStartY = 20;
		uMidScaledHSz = 96, uMidScaledVSz = 96;

		uDstStartX = 10, uDstStartY = 10, uDstScaledHSz = 160, uDstScaledVSz = 160;		
	}
	else 
	{
		UART_Printf("Error! Unsupported Data Format!\n");
		return;
	}

	UART_Printf("\n");
	if(uSimpleTest == 0)
	{
		UART_Printf("=========    Complex_Test Image Size    =========\n");
		UART_Printf("SrcImgHSz     = %d,       SrcImgVSz     = %d\n",uImgHSz,uImgVSz);
		UART_Printf("SrcStartX     = %d,       SrcStartY     = %d\n",uSrcStartX,uSrcStartY);
		UART_Printf("SrcCroppedHSz = %d,       SrcCroppedVSz = %d\n",uSrcCroppedHSz,uSrcCroppedVSz);
		UART_Printf("\n");
		UART_Printf("MidImgHSz     = %d,       MidImgVSz     = %d\n",uMidImgHSz,uMidImgVSz);
		UART_Printf("MidStartX     = %d,       MidStartY     = %d\n",uMidStartX,uMidStartY);
		UART_Printf("MidScaledHSz  = %d,       MidScaledVSz  = %d\n",uMidScaledHSz,uMidScaledVSz);
		UART_Printf("\n");
		UART_Printf("DstStartX     = %d,       DstStartY     = %d\n",uDstStartX,uDstStartY);
		UART_Printf("DstScaledHSz  = %d,       DstScaledVSz  = %d\n",uDstScaledHSz,uDstScaledVSz);	
		UART_Printf("=================================================\n");
		UART_Printf("\n");
	}
	
}


// TestXXX_R2Y2R does two steps
// Step 1) Glib Addr(RGB Data Format) -> Post(CSC) -> Post Addr (YUV Data Format) // To verify CSC from RGB to YUV
// Step 2) Post Addr(YUV Data Format) -> Post(CSC) -> LCD Addr (RGB Data Format) // To verify CSC from YUV to RGB
static void POSTT_SimpleDmaToDma_R2Y2R(void)
{
	u32 uGlibStAddr, uPostStAddr;
	u32 uSrcBytesPerPixel, uDstBytesPerPixel;

	UART_Printf("[POSTT_SimpleDmaToDma_R2Y2R]\n");

	uSimpleTest = 1;
	POSTT_SetCscTypeDmaToDma_R2Y2R();
		
	//LCD_InitLDI(MAIN);
#ifdef LCD_INIT_TEST	
	LCD_InitBase();
	LCD_Start();
	LCD_InitWin(eDstDataFmt, uImgHSz, uImgVSz, 0, 0, uImgHSz, uImgVSz, 0, 0, uLcdStAddr, WIN0, false);
#else
	LCD_InitBase();
	LCD_InitDISPC(eDstDataFmt, uLcdStAddr, WIN0, false);
	LCD_GetFrmSz(&uLcdHSz, &uLcdVSz, WIN0);
	//LCD_InitDISPC1(uLcdHSz, uLcdVSz, eDstDataFmt, uLcdStAddr, WIN0, false);
	LCD_Start();
#endif
	LCD_SetWinOnOff(1, WIN0);
	//LCD_GetFrmSz(&uLcdHSz, &uLcdVSz, WIN0);

	uSrcBytesPerPixel = (eSrcDataFmt == RGB16) ? 2 : 4;
	uDstBytesPerPixel = (eDstDataFmt == RGB16) ? 2 : 4;
	uGlibStAddr = uLcdStAddr + uLcdHSz*uLcdVSz*uDstBytesPerPixel;
	uPostStAddr = uGlibStAddr + uLcdHSz*uLcdVSz*uSrcBytesPerPixel;

#if (POST_IMAGE_SOURCE == POST_IMAGE_PATTERN)
	GLIB_Init(uGlibStAddr, uLcdHSz, uLcdVSz, eSrcDataFmt);
	GLIB_DrawPattern(uLcdHSz, uLcdVSz);
#elif (POST_IMAGE_SOURCE == POST_IMAGE_HEADER)
	POSTT_GetImageFromHeader(uGlibStAddr, uLcdHSz, uLcdVSz, eSrcDataFmt);
#endif

	// From RGB To YUV
	UART_Printf("========   RGB -> YUV   =========\n");
	UART_Printf("Source Address : 0x%08x\n", uGlibStAddr);
	UART_Printf("Destination Address : 0x%08x\n", uPostStAddr);
	POST_InitIpForDmaInDmaOut(	uLcdHSz, uLcdVSz, uGlibStAddr, eSrcDataFmt, 
									uLcdHSz, uLcdVSz, uPostStAddr, eMidDataFmt,
									1, FALSE, ONE_SHOT, &oPost);
	//POST_InitIpForDmaInDmaOut(	uImgHSz, uImgVSz, uGlibStAddr, eSrcDataFmt, 
		//							uLcdHSz, uLcdVSz, uPostStAddr, eMidDataFmt,
			//						1, FALSE, ONE_SHOT, &oPost);	
	POST_StartProcessing(&oPost);
	while (!POST_IsProcessingDone(&oPost));	

	//SaveToFile(POST_YUV_FILE_NAME1, uLcdHSz*uLcdVSz*2, uPostStAddr);
	
#if 0	// R->Y Wide & Narrow check
	{
		int R, G, B, YCbCr,i,j;
		for(i=0 ; i<uLcdVSz ; i++)
			for(j=0 ; j<uLcdHSz ; j++)
			{
				R = (*((s32 *)uGlibStAddr + i*uLcdVSz + j) & 0xFF0000)>>16;
				G = (*((s32 *)uGlibStAddr + i*uLcdVSz + j) & 0x00FF00)>>8;
				B = (*((s32 *)uGlibStAddr + i*uLcdVSz + j) & 0x0000FF)>>0;
				YCbCr = CSCR2Y_C(R, G, B, 0);
				UART_Printf("YCbCr : 0x%08x\n", YCbCr);
			}
	}
#endif

	// From YUV To RGB
	UART_Printf("========   YUV -> RGB   =========\n");
	UART_Printf("Source Address : 0x%08x\n", uPostStAddr);
	UART_Printf("Destination Address : 0x%08x\n", uLcdStAddr);	
	POST_InitIpForDmaInDmaOut(	uLcdHSz, uLcdVSz, uPostStAddr, eMidDataFmt, 
									uLcdHSz, uLcdVSz, uLcdStAddr, eDstDataFmt,
									1, FALSE, ONE_SHOT, &oPost);
	POST_StartProcessing(&oPost);
	while (!POST_IsProcessingDone(&oPost));	

#if (POST_IMAGE_BMP_SLIDE)	
	{
		u32 uImageNum;
		UART_Printf("Input the any key to display the other image ('q' : quit)\n");
		uImageNum = 0;
		
		while(UART_Getc() != 'q')
		{
			POSTT_GetImageFromBMP(uGlibStAddr, uLcdHSz, uLcdVSz, uImageNum, eSrcDataFmt);

			// RGB -> YUV
			POST_InitIpForDmaInDmaOut(	uLcdHSz, uLcdVSz, uGlibStAddr, eSrcDataFmt, 
											uLcdHSz, uLcdVSz, uPostStAddr, eMidDataFmt,
											1, FALSE, ONE_SHOT, &oPost);
			POST_StartProcessing(&oPost);
			while (!POST_IsProcessingDone(&oPost));	

			//SaveToFile(POST_YUV_FILE_NAME1, uLcdHSz*uLcdVSz*2, uPostStAddr);
			
			// YUV -> RGB
			POST_InitIpForDmaInDmaOut(	uLcdHSz, uLcdVSz, uPostStAddr, eMidDataFmt, 
											uLcdHSz, uLcdVSz, uLcdStAddr, eDstDataFmt,
											1, FALSE, ONE_SHOT, &oPost);
			POST_StartProcessing(&oPost);
			while (!POST_IsProcessingDone(&oPost));	
			uImageNum++;
		}
	}
#endif	

	UART_Printf("\n");
}


static void POSTT_ComplexDmaToDma_R2Y2R(void)
{
	u32 uGlibStAddr, uPostStAddr;
	u32 uSrcBytesPerPixel, uDstBytesPerPixel;

	UART_Printf("[POSTT_ComplexDmaToDma_R2Y2R]\n");

	uSimpleTest = 0;
	POSTT_SetCscTypeDmaToDma_R2Y2R();
	
	//bIsR2Y2RFlag = true;	
	ePostRunMode = ONE_SHOT;

	//LCD_InitLDI(MAIN);

#ifdef LCD_INIT_TEST	
	LCD_InitBase();
	LCD_Start();
	LCD_InitWin(eDstDataFmt, uImgHSz, uImgVSz, 0, 0, uImgHSz, uImgVSz, 0, 0, uLcdStAddr, WIN0, false);
#else
	LCD_InitDISPC(eDstDataFmt, uLcdStAddr, WIN0, false);
	LCD_Start();
#endif

	LCD_SetWinOnOff(1, WIN0);

	LCD_GetFrmSz(&uLcdHSz, &uLcdVSz, WIN0);

	uSrcBytesPerPixel = (eSrcDataFmt == RGB24) ? 4 : 2;
	uDstBytesPerPixel = (eDstDataFmt == RGB24) ? 4 : 2;

	uGlibStAddr = uLcdStAddr + uLcdHSz*uLcdVSz*uDstBytesPerPixel;
	uPostStAddr = uGlibStAddr + uImgHSz*uImgVSz*uSrcBytesPerPixel;

	GLIB_Init(uLcdStAddr, uLcdHSz, uLcdVSz, eDstDataFmt);
	GLIB_ClearFrame(C_BLACK);

#if (POST_IMAGE_SOURCE == POST_IMAGE_PATTERN)
	GLIB_Init(uGlibStAddr, uImgHSz, uImgVSz, eSrcDataFmt);
	GLIB_DrawPattern(uImgHSz, uImgVSz);
#elif (POST_IMAGE_SOURCE == POST_IMAGE_HEADER)
	POSTT_GetImageFromHeader(uGlibStAddr, uImgHSz, uImgVSz, eSrcDataFmt);
#elif (POST_IMAGE_SOURCE == POST_IMAGE_BMP)
	POSTT_GetImageFromBMP(uGlibStAddr, uLcdHSz, uLcdVSz, 3, eSrcDataFmt);
#endif

	// From RGB To YUV
	UART_Printf("========   RGB -> YUV   =========\n");
	UART_Printf("Source Address : 0x%08x\n", uGlibStAddr);
	UART_Printf("Destination Address : 0x%08x\n", uPostStAddr);	
	POST_InitIp1(	uImgHSz, uImgVSz, uSrcStartX, uSrcStartY, uSrcCroppedHSz, uSrcCroppedVSz, uGlibStAddr, eSrcDataFmt,
					uMidImgHSz, uMidImgVSz, uMidStartX, uMidStartY, uMidScaledHSz, uMidScaledVSz, uPostStAddr, eMidDataFmt,
					1, false, ePostRunMode, POST_DMA, POST_DMA, &oPost);
	POST_StartProcessing(&oPost);
	while (!POST_IsProcessingDone(&oPost));
	
	// From YUV To RGB
	UART_Printf("========   YUV -> RGB   =========\n");
	UART_Printf("Source Address : 0x%08x\n", uPostStAddr);
	UART_Printf("Destination Address : 0x%08x\n", uLcdStAddr);		
	POST_InitIp1(	uMidImgHSz, uMidImgVSz, uMidStartX, uMidStartY, uMidScaledHSz, uMidScaledVSz, uPostStAddr, eMidDataFmt,
					uLcdHSz, uLcdVSz, uDstStartX, uDstStartY, uDstScaledHSz, uDstScaledVSz, uLcdStAddr, eDstDataFmt,
					1, false, ONE_SHOT, POST_DMA, POST_DMA, &oPost);
	POST_StartProcessing(&oPost);
	while (!POST_IsProcessingDone(&oPost));	
}


static void POSTT_SimpleFreeRunDmaToDma_R2Y2R_Polling(void)
{
	u32 uGlibStAddr, uGlibStAddr2, uPostStAddr, uPostStAddr2;
	u32 uSrcBytesPerPixel, uDstBytesPerPixel;
	u32 uFrmCnt;

	UART_Printf("[POSTT_SimpleFreeRunDmaToDma_R2Y2R_Polling]\n");

	uSimpleTest = 1;
	POSTT_SetCscTypeDmaToDma_R2Y2R();
	
	bIsPollingOrInt = TRUE;

	//LCD_InitLDI(MAIN);
#ifdef LCD_INIT_TEST	
	LCD_InitBase();
	LCD_Start();
	LCD_InitWin(eDstDataFmt, uImgHSz, uImgVSz, 0, 0, uImgHSz, uImgVSz, 0, 0, uLcdStAddr, WIN0, false);
#else
	LCD_InitDISPC(eDstDataFmt, uLcdStAddr, WIN0, false);
	LCD_Start();
#endif
	LCD_SetWinOnOff(1, WIN0);
	LCD_GetFrmSz(&uLcdHSz, &uLcdVSz, WIN0);

	uSrcBytesPerPixel = (eSrcDataFmt == RGB16) ? 2 : 4;
	uDstBytesPerPixel = (eDstDataFmt == RGB16) ? 2 : 4;

	uGlibStAddr = uLcdStAddr + uLcdHSz*uLcdVSz*uDstBytesPerPixel;
	uPostStAddr = uGlibStAddr + uLcdHSz*uLcdVSz*uSrcBytesPerPixel;

	uGlibStAddr2 = uPostStAddr + uLcdHSz*uLcdVSz*uDstBytesPerPixel;
	uPostStAddr2 = uGlibStAddr2 + uLcdHSz*uLcdVSz*uSrcBytesPerPixel;

	GLIB_Init(uGlibStAddr, uLcdHSz, uLcdVSz, eSrcDataFmt);
	GLIB_DrawPattern(uLcdHSz, uLcdVSz);

	GLIB_Init(uGlibStAddr2, uLcdHSz, uLcdVSz, eSrcDataFmt);
	GLIB_DrawPattern2(uLcdHSz, uLcdVSz);

	// From RGB to YUV for pattern 1
	POST_InitIpForDmaInDmaOut(uLcdHSz, uLcdVSz, uGlibStAddr, eSrcDataFmt, 
								uLcdHSz, uLcdVSz, uPostStAddr, eMidDataFmt,
								1, FALSE, ONE_SHOT, &oPost);
	POST_StartProcessing(&oPost);
	while (!POST_IsProcessingDone(&oPost));
	UART_Printf("CSC(RGB -> YUV) is finished for pattern 1\n");
	
	// From RGB to YUV for pattern 2
	POST_InitIpForDmaInDmaOut(uLcdHSz, uLcdVSz, uGlibStAddr2, eSrcDataFmt,
								uLcdHSz, uLcdVSz, uPostStAddr2, eMidDataFmt,
								1, FALSE, ONE_SHOT, &oPost);
	POST_StartProcessing(&oPost);
	while (!POST_IsProcessingDone(&oPost));
	UART_Printf("CSC(RGB -> YUV) is fiished for pattern 2\n");
	
	// From YUV To RGB
	POST_InitIpForDmaInDmaOut(uLcdHSz, uLcdVSz, uPostStAddr2, eMidDataFmt,
								uLcdHSz, uLcdVSz, uLcdStAddr, eDstDataFmt,
								1, FALSE, FREE_RUN, &oPost);
	POST_SetNextFrameStAddr(uPostStAddr, uLcdStAddr, &oPost);
	POST_StartProcessing(&oPost);

	uFrmCnt = 0;
	while(uFrmCnt<uMaxFrameCnt)
	{
		while (!POST_IsProcessingDone(&oPost));			
		
		if (uFrmCnt%2)
			POST_SetNextFrameStAddr(uPostStAddr2, uLcdStAddr, &oPost);
		else
			POST_SetNextFrameStAddr(uPostStAddr, uLcdStAddr, &oPost);
		POST_ClearPending(&oPost);

		DelayfrTimer(milli, 1000);
		uFrmCnt++;
	}

	POST_StopProcessingOfFreeRun(&oPost);	
	while (!POST_IsProcessingDone(&oPost));	
	POST_ClearPending(&oPost);	

#if (POST_IMAGE_BMP_SLIDE)	
	POSTT_GetImageFromBMP(uGlibStAddr, uLcdHSz, uLcdVSz, 16, eSrcDataFmt);
	POSTT_GetImageFromBMP(uGlibStAddr2, uLcdHSz, uLcdVSz, 17, eSrcDataFmt);

	// From RGB to YUV for pattern 1
	POST_InitIpForDmaInDmaOut(uLcdHSz, uLcdVSz, uGlibStAddr, eSrcDataFmt, 
								uLcdHSz, uLcdVSz, uPostStAddr, eMidDataFmt,
								1, FALSE, ONE_SHOT, &oPost);
	POST_StartProcessing(&oPost);
	while (!POST_IsProcessingDone(&oPost));
	UART_Printf("CSC(RGB -> YUV) is finished for pattern 1\n");
	
	// From RGB to YUV for pattern 2
	POST_InitIpForDmaInDmaOut(uLcdHSz, uLcdVSz, uGlibStAddr2, eSrcDataFmt,
								uLcdHSz, uLcdVSz, uPostStAddr2, eMidDataFmt,
								1, FALSE, ONE_SHOT, &oPost);
	POST_StartProcessing(&oPost);
	while (!POST_IsProcessingDone(&oPost));
	UART_Printf("CSC(RGB -> YUV) is fiished for pattern 2\n");
	
	// From YUV To RGB
	POST_InitIpForDmaInDmaOut(uLcdHSz, uLcdVSz, uPostStAddr2, eMidDataFmt,
								uLcdHSz, uLcdVSz, uLcdStAddr, eDstDataFmt,
								1, FALSE, FREE_RUN, &oPost);
	POST_SetNextFrameStAddr(uPostStAddr, uLcdStAddr, &oPost);
	POST_StartProcessing(&oPost);

	uFrmCnt = 0;
	while(uFrmCnt<uMaxFrameCnt)
	{
		while (!POST_IsProcessingDone(&oPost));			
		
		if (uFrmCnt%2)
			POST_SetNextFrameStAddr(uPostStAddr2, uLcdStAddr, &oPost);
		else
			POST_SetNextFrameStAddr(uPostStAddr, uLcdStAddr, &oPost);
		POST_ClearPending(&oPost);

		DelayfrTimer(milli, 1000);
		uFrmCnt++;
	}

	POST_StopProcessingOfFreeRun(&oPost);	
	while (!POST_IsProcessingDone(&oPost));	
	POST_ClearPending(&oPost);
#endif		
}


static void POSTT_SimpleFreeRunDmaToDma_R2Y2R_Int(void)
{
	u32 uGlibStAddr, uGlibStAddr2;
	u32 uSrcBytesPerPixel, uDstBytesPerPixel;
	u32 uPostIsrCount, uPostFrameCount;
	
	UART_Printf("[POSTT_SimpleFreeRunDmaToDma_R2Y2R_Int]\n");

	uSimpleTest = 1;
	POSTT_SetCscTypeDmaToDma_R2Y2R();
		
	bIsPollingOrInt = FALSE;

	//LCD_InitLDI(MAIN);
	
#ifdef LCD_INIT_TEST	
	LCD_InitBase();
	LCD_Start();
	LCD_InitWin(eDstDataFmt, uImgHSz, uImgVSz, 0, 0, uImgHSz, uImgVSz, 0, 0, uLcdStAddr, WIN0, false);
#else
	LCD_InitDISPC(eDstDataFmt, uLcdStAddr, WIN0, false);
	LCD_Start();
#endif

	LCD_SetWinOnOff(1, WIN0);

	LCD_GetFrmSz(&uLcdHSz, &uLcdVSz, WIN0);

	uSrcBytesPerPixel = (eSrcDataFmt == RGB16) ? 2 : 4;
	uDstBytesPerPixel = (eDstDataFmt == RGB16) ? 2 : 4;

	uGlibStAddr = uLcdStAddr + uLcdHSz*uLcdVSz*uDstBytesPerPixel;
	uNxtAddr1 = uGlibStAddr + uLcdHSz*uLcdVSz*uSrcBytesPerPixel;

	uGlibStAddr2 = uNxtAddr1 + uLcdHSz*uLcdVSz*uDstBytesPerPixel;
	uNxtAddr2 = uGlibStAddr2 + uLcdHSz*uLcdVSz*uSrcBytesPerPixel;

	GLIB_Init(uGlibStAddr, uLcdHSz, uLcdVSz, eSrcDataFmt);
	GLIB_DrawPattern(uLcdHSz, uLcdVSz);

	GLIB_Init(uGlibStAddr2, uLcdHSz, uLcdVSz, eSrcDataFmt);
	GLIB_DrawPattern2(uLcdHSz, uLcdVSz);

	// From RGB to YUV for pattern 1
	POST_InitIpForDmaInDmaOut(uLcdHSz, uLcdVSz, uGlibStAddr, eSrcDataFmt, 
								uLcdHSz, uLcdVSz, uNxtAddr1, eMidDataFmt,
								1, FALSE, ONE_SHOT, &oPost);
	POST_StartProcessing(&oPost);
	while (!POST_IsProcessingDone(&oPost));
	UART_Printf("CSC(RGB -> YUV) is finished for pattern 1\n");

	// From RGB to YUV for pattern 2
	POST_InitIpForDmaInDmaOut(uLcdHSz, uLcdVSz, uGlibStAddr2, eSrcDataFmt,
								uLcdHSz, uLcdVSz, uNxtAddr2, eMidDataFmt,
								1, FALSE, ONE_SHOT, &oPost);
	POST_StartProcessing(&oPost);
	while (!POST_IsProcessingDone(&oPost));
	UART_Printf("CSC(RGB -> YUV) is finished for pattern 2\n");

	//Interrupt Setting
	uPostIsrCount = 0;
	uPostFrameCount = 0;
	uPostIntOccured = FALSE;
	//INTC_InitIp();
	//INTC_SetHandlerAndUnmask(INT_POST, PostISRDma);
	INTC_Enable(NUM_POST0);
	INTC_SetVectAddr(NUM_POST0, PostISRDma);
	POST_EnableInterrupt(POST_LEVEL_INT, &oPost);
	
	// From YUV To RGB
	POST_InitIpForDmaInDmaOut(uLcdHSz, uLcdVSz, uNxtAddr1, eMidDataFmt,
								uLcdHSz, uLcdVSz, uLcdStAddr, eDstDataFmt, 
								1, FALSE, FREE_RUN, &oPost);
	POST_SetNextFrameStAddr(uNxtAddr2, uLcdStAddr, &oPost);

	POST_StartProcessing(&oPost);
	
	while(uPostFrameCount < uMaxFrameCnt)
	{
		if(uPostIntOccured == TRUE)
		{
			uPostIsrCount++;
			if(uPostIsrCount > 200)  // 1000 is too long
			{
				uPostFrameCount++;
				
			 	if (uPostFrameCount % 2)
					POST_SetNextFrameStAddr(uNxtAddr2, uLcdStAddr, &oPost);
				else 
					POST_SetNextFrameStAddr(uNxtAddr1, uLcdStAddr, &oPost);

				if (uPostFrameCount == uMaxFrameCnt)
				{
					POST_StopProcessingOfFreeRun(&oPost);	
					while (!POST_IsProcessingDone(&oPost));	
					uPostIntOccured = FALSE;
					break;
				}
				uPostIsrCount = 0;
			}
			uPostIntOccured = FALSE;
		}
	}


#if (POST_IMAGE_BMP_SLIDE)	
	POSTT_GetImageFromBMP(uGlibStAddr, uLcdHSz, uLcdVSz, 18, eSrcDataFmt);
	POSTT_GetImageFromBMP(uGlibStAddr2, uLcdHSz, uLcdVSz, 19, eSrcDataFmt);

	// From RGB to YUV for pattern 1
	POST_InitIpForDmaInDmaOut(uLcdHSz, uLcdVSz, uGlibStAddr, eSrcDataFmt, 
								uLcdHSz, uLcdVSz, uNxtAddr1, eMidDataFmt,
								1, FALSE, ONE_SHOT, &oPost);
	POST_StartProcessing(&oPost);
	while (!POST_IsProcessingDone(&oPost));
	UART_Printf("CSC(RGB -> YUV) is finished for pattern 1\n");

	// From RGB to YUV for pattern 2
	POST_InitIpForDmaInDmaOut(uLcdHSz, uLcdVSz, uGlibStAddr2, eSrcDataFmt,
								uLcdHSz, uLcdVSz, uNxtAddr2, eMidDataFmt,
								1, FALSE, ONE_SHOT, &oPost);
	POST_StartProcessing(&oPost);
	while (!POST_IsProcessingDone(&oPost));
	UART_Printf("CSC(RGB -> YUV) is finished for pattern 2\n");

	//Interrupt Setting
	uPostIsrCount = 0;
	uPostFrameCount = 0;
	uPostIntOccured = FALSE;
	//INTC_InitIp();
	//INTC_SetHandlerAndUnmask(INT_POST, PostISRDma);
	INTC_Enable(NUM_POST0);
	INTC_SetVectAddr(NUM_POST0, PostISRDma);
	POST_EnableInterrupt(POST_LEVEL_INT, &oPost);
	
	// From YUV To RGB
	POST_InitIpForDmaInDmaOut(uLcdHSz, uLcdVSz, uNxtAddr1, eMidDataFmt,
								uLcdHSz, uLcdVSz, uLcdStAddr, eDstDataFmt, 
								1, FALSE, FREE_RUN, &oPost);
	POST_SetNextFrameStAddr(uNxtAddr2, uLcdStAddr, &oPost);

	POST_StartProcessing(&oPost);
	
	while(uPostFrameCount < uMaxFrameCnt)
	{
		if(uPostIntOccured == TRUE)
		{
			uPostIsrCount++;
			if(uPostIsrCount > 200)
			{
				uPostFrameCount++;
				
			 	if (uPostFrameCount % 2)
					POST_SetNextFrameStAddr(uNxtAddr2, uLcdStAddr, &oPost);
				else 
					POST_SetNextFrameStAddr(uNxtAddr1, uLcdStAddr, &oPost);

				if (uPostFrameCount == uMaxFrameCnt)
				{
					POST_StopProcessingOfFreeRun(&oPost);	
					while (!POST_IsProcessingDone(&oPost));	
					uPostIntOccured = FALSE;
					break;
				}
				uPostIsrCount = 0;
			}
			uPostIntOccured = FALSE;
		}
	}
#endif	

	POST_DisableInterrupt(&oPost);
	POST_ClearPending(&oPost);	
	//INTC_Mask(INT_POST);			
	INTC_Disable(NUM_POST0);
}


static void POSTT_ComplexFreeRunDmaToDma_R2Y2R_Polling(void)
{
	u32 uGlibStAddr, uGlibStAddr2, uPostStAddr, uPostStAddr2;
	u32 uSrcBytesPerPixel, uDstBytesPerPixel;
	u32 uFrmCnt;

	UART_Printf("[POSTT_ComplexFreeRunDmaToDma_R2Y2R_Polling]\n");

	uSimpleTest = 0;
	POSTT_SetCscTypeDmaToDma_R2Y2R();
	
	bIsPollingOrInt = true;	
	//bIsR2Y2RFlag = true;	
	ePostRunMode = FREE_RUN;
	
	//LCD_InitLDI(MAIN);
#ifdef LCD_INIT_TEST	
	LCD_InitBase();
	LCD_Start();
	LCD_InitWin(eDstDataFmt, uImgHSz, uImgVSz, 0, 0, uImgHSz, uImgVSz, 0, 0, uLcdStAddr, WIN0, false);
#else
	LCD_InitDISPC(eDstDataFmt, uLcdStAddr, WIN0, false);
	LCD_Start();
#endif
	LCD_SetWinOnOff(1, WIN0);
	LCD_GetFrmSz(&uLcdHSz, &uLcdVSz, WIN0);

	uSrcBytesPerPixel = (eSrcDataFmt == RGB24) ? 4 : 2;
	uDstBytesPerPixel = (eDstDataFmt == RGB24) ? 4 : 2;

	uGlibStAddr = uLcdStAddr + uLcdHSz*uLcdVSz*uDstBytesPerPixel;
	uPostStAddr = uGlibStAddr + uImgHSz*uImgVSz*uSrcBytesPerPixel;

	uGlibStAddr2 = uPostStAddr + uMidImgHSz*uMidImgVSz*uDstBytesPerPixel;
	uPostStAddr2 = uGlibStAddr2 + uImgHSz*uImgVSz*uSrcBytesPerPixel;

	// clear the frame
	GLIB_Init(uLcdStAddr, uLcdHSz, uLcdVSz, eDstDataFmt);
	GLIB_ClearFrame(C_BLACK);

	// Draw Pattern 1
	GLIB_Init(uGlibStAddr, uImgHSz, uImgVSz, eSrcDataFmt);
	GLIB_DrawPattern(uImgHSz, uImgVSz);

	// Draw Pattern 2
	GLIB_Init(uGlibStAddr2, uImgHSz, uImgVSz, eSrcDataFmt);
	GLIB_DrawPattern2(uImgHSz, uImgVSz);

	// CSC From RGB To YUV For Pattern 1
	POST_InitIp1(	uImgHSz, uImgVSz, uSrcStartX, uSrcStartY, uSrcCroppedHSz, uSrcCroppedVSz, uGlibStAddr, eSrcDataFmt,
					uMidImgHSz, uMidImgVSz, uMidStartX, uMidStartY, uMidScaledHSz, uMidScaledVSz, uPostStAddr, eMidDataFmt,
					1, FALSE, ONE_SHOT, POST_DMA, POST_DMA, &oPost);
	POST_StartProcessing(&oPost);
	while (!POST_IsProcessingDone(&oPost));
	
	// CSC From RGB To YUV For Pattern 2
	POST_InitIp1(	uImgHSz, uImgVSz, uSrcStartX, uSrcStartY, uSrcCroppedHSz, uSrcCroppedVSz, uGlibStAddr2, eSrcDataFmt,
					uMidImgHSz, uMidImgVSz, uMidStartX, uMidStartY, uMidScaledHSz, uMidScaledVSz, uPostStAddr2, eMidDataFmt,
					1, false, ONE_SHOT, POST_DMA, POST_DMA, &oPost);
	POST_StartProcessing(&oPost);
	while (!POST_IsProcessingDone(&oPost));
	
	// From YUV To RGB
	POST_InitIp1(	uMidImgHSz, uMidImgVSz, uMidStartX, uMidStartY, uMidScaledHSz, uMidScaledVSz, uPostStAddr, eMidDataFmt,
					uLcdHSz, uLcdVSz, uDstStartX, uDstStartY, uDstScaledHSz, uDstScaledVSz, uLcdStAddr, eDstDataFmt, 
					1, false, FREE_RUN, POST_DMA, POST_DMA, &oPost); // src. frm. buf. num: 1, double buffering: disable
	POST_SetNextFrameStAddr(uPostStAddr2, uLcdStAddr, &oPost);
	POST_StartProcessing(&oPost);

	uFrmCnt = 0;
	while(uFrmCnt<uMaxFrameCnt)
	{
		while (!POST_IsProcessingDone(&oPost));			
		
		if (uFrmCnt%2)
			POST_SetNextFrameStAddr(uPostStAddr, uLcdStAddr, &oPost);
		else
			POST_SetNextFrameStAddr(uPostStAddr2, uLcdStAddr, &oPost);
		POST_ClearPending(&oPost);

		DelayfrTimer(milli, 500);
		uFrmCnt++;
	}

	POST_StopProcessingOfFreeRun(&oPost);	
	while (!POST_IsProcessingDone(&oPost));	
}


static void POSTT_ComplexFreeRunDmaToDma_R2Y2R_Int(void)
{
	u32 uGlibStAddr, uGlibStAddr2;
	u32 uSrcBytesPerPixel, uDstBytesPerPixel;
	u32 uPostIsrCount, uPostFrameCount;

	UART_Printf("[POSTT_ComplexFreeRunDmaToDma_R2Y2R_Int]\n");

	uSimpleTest = 0;
	POSTT_SetCscTypeDmaToDma_R2Y2R();
	
	bIsPollingOrInt = false;
	//bIsR2Y2RFlag = true;	
	ePostRunMode = FREE_RUN;

	//LCD_InitLDI(MAIN);
#ifdef LCD_INIT_TEST	
	LCD_InitBase();
	LCD_Start();
	LCD_InitWin(eDstDataFmt, uImgHSz, uImgVSz, 0, 0, uImgHSz, uImgVSz, 0, 0, uLcdStAddr, WIN0, false);
#else
	LCD_InitDISPC(eDstDataFmt, uLcdStAddr, WIN0, false);
	LCD_Start();
#endif
	LCD_SetWinOnOff(1, WIN0);
	LCD_GetFrmSz(&uLcdHSz, &uLcdVSz, WIN0);

	uSrcBytesPerPixel = (eSrcDataFmt == RGB24) ? 4 : 2;
	uDstBytesPerPixel = (eDstDataFmt == RGB24) ? 4 : 2;

	uGlibStAddr = uLcdStAddr + uLcdHSz*uLcdVSz*uDstBytesPerPixel;
	uNxtAddr1 = uGlibStAddr + uImgHSz*uImgVSz*uSrcBytesPerPixel;

	uGlibStAddr2 = uNxtAddr1 + uMidImgHSz*uMidImgVSz*uDstBytesPerPixel;
	uNxtAddr2 = uGlibStAddr2 + uImgHSz*uImgVSz*uSrcBytesPerPixel;

	// clear the frame
	GLIB_Init(uLcdStAddr, uLcdHSz, uLcdVSz, eDstDataFmt);
	GLIB_ClearFrame(C_BLACK);

	// Draw Pattern 1
	GLIB_Init(uGlibStAddr, uImgHSz, uImgVSz, eSrcDataFmt);
	GLIB_DrawPattern(uImgHSz, uImgVSz);

	// Draw Pattern 2
	GLIB_Init(uGlibStAddr2, uImgHSz, uImgVSz, eSrcDataFmt);
	GLIB_DrawPattern2(uImgHSz, uImgVSz);

	// CSC From RGB To YUV For Pattern 1
	POST_InitIp1(	uImgHSz, uImgVSz, uSrcStartX, uSrcStartY, uSrcCroppedHSz, uSrcCroppedVSz, uGlibStAddr, eSrcDataFmt,
					uMidImgHSz, uMidImgVSz, uMidStartX, uMidStartY, uMidScaledHSz, uMidScaledVSz, uNxtAddr1, eMidDataFmt,
					1, false, ONE_SHOT, POST_DMA, POST_DMA, &oPost);
	POST_StartProcessing(&oPost);
	while (!POST_IsProcessingDone(&oPost));
	
	// CSC From RGB To YUV For Pattern 2
	POST_InitIp1(	uImgHSz, uImgVSz, uSrcStartX, uSrcStartY, uSrcCroppedHSz, uSrcCroppedVSz, uGlibStAddr2, eSrcDataFmt,
					uMidImgHSz, uMidImgVSz, uMidStartX, uMidStartY, uMidScaledHSz, uMidScaledVSz, uNxtAddr2, eMidDataFmt,
					1, false, ONE_SHOT, POST_DMA, POST_DMA, &oPost);
	POST_StartProcessing(&oPost);
	while (!POST_IsProcessingDone(&oPost));

	//Interrupt Setting
	uPostIsrCount = 0;
	uPostFrameCount = 0;
	uPostIntOccured = FALSE;
	//INTC_InitIp();
	//INTC_SetHandlerAndUnmask(INT_POST, PostISRDma);
	INTC_Enable(NUM_POST0);
	INTC_SetVectAddr(NUM_POST0, PostISRDma);
	POST_EnableInterrupt(POST_LEVEL_INT, &oPost);
	
	// From YUV To RGB
	POST_InitIp1(	uMidImgHSz, uMidImgVSz, uMidStartX, uMidStartY, uMidScaledHSz, uMidScaledVSz, uNxtAddr1, eMidDataFmt,
					uLcdHSz, uLcdVSz, uDstStartX, uDstStartY, uDstScaledHSz, uDstScaledVSz, uLcdStAddr, eDstDataFmt,
					1, false, FREE_RUN, POST_DMA, POST_DMA, &oPost); // src. frm. buf. num: 1 double buffering: disable
	POST_SetNextFrameStAddr(uNxtAddr2, uLcdStAddr, &oPost);
	POST_StartProcessing(&oPost);

	while(uPostFrameCount < uMaxFrameCnt)
	{
		if(uPostIntOccured == TRUE)
		{
			uPostIsrCount++;
			if(uPostIsrCount > 200)
			{
				uPostFrameCount++;
				
			 	if (uPostFrameCount % 2)
					POST_SetNextFrameStAddr(uNxtAddr2, uLcdStAddr, &oPost);
				else 
					POST_SetNextFrameStAddr(uNxtAddr1, uLcdStAddr, &oPost);

				if (uPostFrameCount == uMaxFrameCnt)
				{
					POST_StopProcessingOfFreeRun(&oPost);	
					while (!POST_IsProcessingDone(&oPost));	
					uPostIntOccured = FALSE;
					break;
				}
				uPostIsrCount = 0;
			}
			uPostIntOccured = FALSE;
		}
	}

	POST_DisableInterrupt(&oPost);
	POST_ClearPending(&oPost);	
	//INTC_Mask(INT_POST);
	INTC_Disable(NUM_POST0);			
}


const testFuncMenu r2y2r_menu[] =
{
	POSTT_SimpleDmaToDma_R2Y2R,                       		"Simple DMA CSC Test \'RGB -> YUV -> RGB\'",
	POSTT_ComplexDmaToDma_R2Y2R,                     		"Complex DMA CSC Test \'RGB -> YUV -> RGB\'",
	POSTT_SimpleFreeRunDmaToDma_R2Y2R_Polling,		"Simple FreeRun And CSC Test \'RGB -> YUV -> RGB\' By Using Polling",						
	POSTT_SimpleFreeRunDmaToDma_R2Y2R_Int,            	"Simple FreeRun And CSC Test \'RGB -> YUV -> RGB\' By Using Interrupt",		
	POSTT_ComplexFreeRunDmaToDma_R2Y2R_Polling,	"Complex FreeRun And CSC Test \'RGB -> YUV -> RGB\' By Using Polling",		
	POSTT_ComplexFreeRunDmaToDma_R2Y2R_Int,       	"Complex FreeRun And CSC Test \'RGB -> YUV -> RGB\' By Using Interrupt",				
	0,                                          0
};


static void POSTT_ConvertRGBToYUVToRGB(void)
{
	u32 i;
	s32 uSel;
			
	while(1)
	{
		UART_Printf("\n");
		for (i=0; (int)(r2y2r_menu[i].desc)!=0; i++)
		{
			UART_Printf("%2d: %s\n", i, r2y2r_menu[i].desc);
		}

		UART_Printf("\n");
		UART_Printf("Select The Function To Test: ");

		uSel = UART_GetIntNum();
		UART_Printf("\n");

		if (uSel == -1)
			break;
		if (uSel>=0 && uSel<(sizeof(r2y2r_menu)/8-1))
			(r2y2r_menu[uSel].func)();
	}
}


////////////////////////////////////////////////////////////////////////////
//////////////////                         YUV to YUV                          ///////////////////
////////////////////////////////////////////////////////////////////////////


static void POSTT_SetCscTypeDma_Y2Y(void)
{
	u32 nSrcDataFmt, nDstDataFmt;

	while (1)
	{
		UART_Printf("\n");
		UART_Printf("[1] YCbYCr422 Interleave\n");
		UART_Printf("[2] YCrYCb422 Interleave\n");
		UART_Printf("[3] CbYCrY422 Interleave\n");
		UART_Printf("[4] CrYCbY422 Interleave\n");
		UART_Printf("[5] YUV420 Non-Interleave\n");			

		UART_Printf("Enter Source Data Format From YUV To YUV: ");
		
		nSrcDataFmt = UART_GetIntNum();

		if (nSrcDataFmt >= 1 && nSrcDataFmt <= 5)
		{
			eSrcDataFmt_Yuv = 
				(nSrcDataFmt == 1) ? YCBYCR :
				(nSrcDataFmt == 2) ? YCRYCB :
				(nSrcDataFmt == 3) ? CBYCRY :
				(nSrcDataFmt == 4) ? CRYCBY : YC420;
				
			break;
		}
		else
			UART_Printf("Invalid Data Format! Retry It!!");
	}

	while (1)
	{
		UART_Printf("\n");
		UART_Printf("[1] YCbYCr422 Interleave\n");
		UART_Printf("[2] YCrYCb422 Interleave\n");
		UART_Printf("[3] CbYCrY422 Interleave\n");
		UART_Printf("[4] CrYCbY422 Interleave\n");
		UART_Printf("[5] YUV420 Non-Interleave\n");			

		UART_Printf("Enter Destination Data Format From YUV To YUV: ");

		nDstDataFmt = UART_GetIntNum();

		if (nDstDataFmt >= 1 && nDstDataFmt <= 5)
		{
			eDstDataFmt_Yuv = 
				(nDstDataFmt == 1) ? YCBYCR :
				(nDstDataFmt == 2) ? YCRYCB :
				(nDstDataFmt == 3) ? CBYCRY :
				(nDstDataFmt == 4) ? CRYCBY : YC420;
			
			break;
		}
		else
			UART_Printf("Invalid Data Format! Retry It!!");
	}

	if (eSrcDataFmt_Yuv == YCBYCR)
	{
		uImgHSz = 240, uImgVSz = 320, uSrcStartX = 0, uSrcStartY = 0;
		uSrcCroppedHSz = 240, uSrcCroppedVSz = 320;
#if 1
		// H-Size : Scale Up, V-Size : Scale Up
		uMidImgHSz = 240, uMidImgVSz = 320, uMidStartX = 60, uMidStartY = 40;
		uMidScaledHSz = 120, uMidScaledVSz = 120;

		uMid2ImgHSz = 240, uMid2ImgVSz = 320, uMid2StartX = 64, uMid2StartY = 64;
		uMid2ScaledHSz = 152, uMid2ScaledVSz = 200;
#else
		// H-Size : Scale Up, V-Size : Scale Up
		uMidImgHSz = 240, uMidImgVSz = 320, uMidStartX = 2, uMidStartY = 2;
		uMidScaledHSz = 120, uMidScaledVSz = 120;

		uMid2ImgHSz = 240, uMid2ImgVSz = 320, uMid2StartX = 8, uMid2StartY = 4;
		uMid2ScaledHSz = 160, uMid2ScaledVSz = 150;
#endif
		uDstStartX = 0, 	uDstStartY = 0, uDstScaledHSz = 320, uDstScaledVSz = 240;
	}
	else if (eSrcDataFmt_Yuv == YCRYCB)
	{
		uImgHSz = 240, uImgVSz = 320, uSrcStartX = 0, uSrcStartY = 0;
		uSrcCroppedHSz = 240, uSrcCroppedVSz = 320;

		// H-Size : Scale Down, V-Size : Scale Down
		uMidImgHSz = 240, uMidImgVSz = 320, uMidStartX = 0, uMidStartY = 0;
		uMidScaledHSz = 240, uMidScaledVSz = 320;

		uMid2ImgHSz = 240, uMid2ImgVSz = 320, uMid2StartX = 16, uMid2StartY = 16;
		uMid2ScaledHSz = 160, uMid2ScaledVSz = 200;
		
		uDstStartX = 0, 	uDstStartY = 0, uDstScaledHSz = 320, uDstScaledVSz = 240;	
	}
	else if (eSrcDataFmt_Yuv == CBYCRY)
	{
		uImgHSz = 240, uImgVSz = 320, uSrcStartX = 0, uSrcStartY = 0;
		uSrcCroppedHSz = 240, uSrcCroppedVSz = 320;

		// H-Size : Scale Up, V-Size : Scale Down
		uMidImgHSz = 240, uMidImgVSz = 320, uMidStartX = 32, uMidStartY = 32;
		uMidScaledHSz = 120, uMidScaledVSz = 200;

		uMid2ImgHSz = 240, uMid2ImgVSz = 320, uMid2StartX = 24, uMid2StartY = 24;
		uMid2ScaledHSz = 200, uMid2ScaledVSz = 120;
		
		uDstStartX = 0, 	uDstStartY = 0, uDstScaledHSz = 320, uDstScaledVSz = 240;	
	}
	else if (eSrcDataFmt_Yuv == CRYCBY)
	{
		uImgHSz = 240, uImgVSz = 320, uSrcStartX = 0, uSrcStartY = 0;
		uSrcCroppedHSz = 240, uSrcCroppedVSz = 320;

		// H-Size : Scale Down, V-Size : Scale Up
		uMidImgHSz = 240, uMidImgVSz = 320, uMidStartX = 0, uMidStartY = 40;
		uMidScaledHSz = 200, uMidScaledVSz = 160;

		uMid2ImgHSz = 240, uMid2ImgVSz = 320, uMid2StartX = 40, uMid2StartY = 40;
		uMid2ScaledHSz = 160, uMid2ScaledVSz = 280;
		
		uDstStartX = 0, 	uDstStartY = 0, uDstScaledHSz = 320, uDstScaledVSz = 240;	}	
	else if (eSrcDataFmt_Yuv == YC420)
	{
		uImgHSz = 240, uImgVSz = 320, uSrcStartX = 0, uSrcStartY = 0;
		uSrcCroppedHSz = 240, uSrcCroppedVSz = 320;

		// H-Size : Scale Down, V-Size : Scale Down
		uMidImgHSz = 240, uMidImgVSz = 320, uMidStartX = 0, uMidStartY = 0;
		uMidScaledHSz = 240, uMidScaledVSz = 320;

		uMid2ImgHSz = 240, uMid2ImgVSz = 320, uMid2StartX = 0, uMid2StartY = 0;
		uMid2ScaledHSz = 160, uMid2ScaledVSz = 200;
		
		uDstStartX = 0, 	uDstStartY = 0, uDstScaledHSz = 320, uDstScaledVSz = 240;	
	}	

	if(uSimpleTest == 0)
	{
		UART_Printf("=========    Complex_Test Image Size    =========\n");
		UART_Printf("MidImgHSz     = %d,       MidImgVSz     = %d\n",uMidImgHSz,uMidImgVSz);
		UART_Printf("MidStartX     = %d,       MidStartY     = %d\n",uMidStartX,uMidStartY);
		UART_Printf("MidCroppedHSz = %d,       MidCroppedVSz = %d\n",uMidScaledHSz,uMidScaledVSz);
		UART_Printf("\n");
		UART_Printf("Mid2StartX     = %d,       Mid2StartY     = %d\n",uMid2StartX,uMid2StartY);
		UART_Printf("Mid2ScaledHSz  = %d,       Mid2ScaledVSz  = %d\n",uMid2ScaledHSz,uMid2ScaledVSz);	
		UART_Printf("=================================================\n");
		UART_Printf("\n");
	}	
}


static void POSTT_SimpleDmaToDma_Y2Y(void)
{
	u32 uGlibStAddr, uSrcStAddr, uDstStAddr;

	UART_Printf("[POSTT_SimpleDmaToDma_Y2Y]\n");

	uSimpleTest = 1;
	POSTT_SetCscTypeDma_Y2Y();

	eDstDataFmt = RGB24;
	//LCD_InitLDI(MAIN);
#ifdef LCD_INIT_TEST	
	LCD_InitBase();
	LCD_Start();
	LCD_InitWin(eDstDataFmt, uImgHSz, uImgVSz, 0, 0, uImgHSz, uImgVSz, 0, 0, uLcdStAddr, WIN0, false);
#else
	LCD_InitDISPC(eDstDataFmt, uLcdStAddr, WIN0, false);
	LCD_Start();
#endif
	LCD_SetWinOnOff(1, WIN0);
	LCD_GetFrmSz(&uLcdHSz, &uLcdVSz, WIN0);

	uImgHSz = uLcdHSz; 
	uImgVSz = uLcdVSz;
	uDstScaledHSz = uLcdHSz; 
	uDstScaledVSz = uLcdVSz;
	
	uGlibStAddr = uLcdStAddr + uImgHSz*uImgVSz*4;
	uSrcStAddr = uGlibStAddr + uImgHSz*uImgVSz*4;
	uDstStAddr = uSrcStAddr + uImgHSz*uImgVSz*4;

	GLIB_Init(uGlibStAddr, uImgHSz, uImgVSz, RGB24);
	GLIB_DrawPattern(uImgHSz, uImgVSz);

	// From RGB24 To YUV
	POST_InitIpForDmaInDmaOut(	uImgHSz, uImgVSz, uGlibStAddr, RGB24, 
									uImgHSz, uImgVSz, uSrcStAddr, eSrcDataFmt_Yuv,
									1, FALSE, ONE_SHOT, &oPost);
	POST_StartProcessing(&oPost);
	while (!POST_IsProcessingDone(&oPost));

	SaveToFile(POST_YUV_FILE_NAME1, uLcdHSz*uLcdVSz*2, uSrcStAddr);

	// From YUV To YUV
	UART_Printf("========   YUV -> YUV   =========\n");
	UART_Printf("Source Address : 0x%08x\n", uSrcStAddr);
	UART_Printf("Destination Address : 0x%08x\n", uDstStAddr);	
	POST_InitIpForDmaInDmaOut(	uImgHSz, uImgVSz, uSrcStAddr, eSrcDataFmt_Yuv, 
									uImgHSz, uImgVSz, uDstStAddr, eDstDataFmt_Yuv,
									1, FALSE, ONE_SHOT, &oPost);
	POST_StartProcessing(&oPost);
	while (!POST_IsProcessingDone(&oPost));

	SaveToFile(POST_YUV_FILE_NAME2, uLcdHSz*uLcdVSz*2, uDstStAddr);

	// From YUV To RGB24
	POST_InitIpForDmaInDmaOut(	uImgHSz, uImgVSz, uDstStAddr, eDstDataFmt_Yuv, 
									uImgHSz, uImgVSz, uLcdStAddr, eDstDataFmt,
									1, FALSE, ONE_SHOT, &oPost);
	POST_StartProcessing(&oPost);
	while (!POST_IsProcessingDone(&oPost));
	
#if (POST_IMAGE_BMP_SLIDE)	
	{
		u32 uImageNum;
		
		UART_Printf("Input the any key to display the other image ('q' : quit)\n");
		uImageNum = 0;
		while(UART_Getc() != 'q')
		{
			POSTT_GetImageFromBMP(uGlibStAddr, uLcdHSz, uLcdVSz, uImageNum, RGB24);

			// From RGB24 To YUV
			POST_InitIpForDmaInDmaOut(	uImgHSz, uImgVSz, uGlibStAddr, RGB24, 
											uImgHSz, uImgVSz, uSrcStAddr, eSrcDataFmt_Yuv,
											1, FALSE, ONE_SHOT, &oPost);
			POST_StartProcessing(&oPost);
			while (!POST_IsProcessingDone(&oPost));

			SaveToFile(POST_YUV_FILE_NAME1, uLcdHSz*uLcdVSz*2, uSrcStAddr);
			
			// From YUV To YUV
			POST_InitIpForDmaInDmaOut(	uImgHSz, uImgVSz, uSrcStAddr, eSrcDataFmt_Yuv, 
											uImgHSz, uImgVSz, uDstStAddr, eDstDataFmt_Yuv,
											1, FALSE, ONE_SHOT, &oPost);
			POST_StartProcessing(&oPost);
			while (!POST_IsProcessingDone(&oPost));

			SaveToFile(POST_YUV_FILE_NAME2, uLcdHSz*uLcdVSz*2, uDstStAddr);
			
			// From YUV To RGB24
			POST_InitIpForDmaInDmaOut(	uImgHSz, uImgVSz, uDstStAddr, eDstDataFmt_Yuv, 
											uImgHSz, uImgVSz, uLcdStAddr, eDstDataFmt,
											1, FALSE, ONE_SHOT, &oPost);
			POST_StartProcessing(&oPost);
			while (!POST_IsProcessingDone(&oPost));
			uImageNum++;
		}
	}
#endif	

}


static void POSTT_ComplexIDmaToDma_Y2Y(void)
{
	u32 uGlibStAddr1, uSrcStAddr, uDstStAddr;
//	u32 uBytesPerPixel;

	UART_Printf("[POSTT_ComplexIDmaToDma_Y2Y]\n");

	uSimpleTest = 0;
	POSTT_SetCscTypeDma_Y2Y();

	eDstDataFmt = RGB24;
	//LCD_InitLDI(MAIN);
#ifdef LCD_INIT_TEST	
	LCD_InitBase();
	LCD_Start();
	LCD_InitWin(eDstDataFmt, uImgHSz, uImgVSz, 0, 0, uImgHSz, uImgVSz, 0, 0, uLcdStAddr, WIN0, false);
#else
	LCD_InitDISPC(eDstDataFmt, uLcdStAddr, WIN0, false);
	LCD_Start();
#endif
	LCD_SetWinOnOff(1, WIN0);
	LCD_GetFrmSz(&uLcdHSz, &uLcdVSz, WIN0);
	
	//uGlibStAddr1 = uLcdStAddr + uImgHSz*uImgVSz*4;
	uGlibStAddr1 = uLcdStAddr + uLcdHSz*uLcdVSz*4;
	uSrcStAddr = uGlibStAddr1 + uImgHSz*uImgVSz*4;
	uDstStAddr = uSrcStAddr + uMidImgHSz*uMidImgVSz*4;
	
	// To Clear the LCD Display
	GLIB_Init(uLcdStAddr, uLcdHSz, uLcdVSz, RGB24);
	GLIB_ClearFrame(C_BLACK);

	// To Clear the uDstStAddr
	GLIB_Init(uDstStAddr, uMid2ImgHSz, uMid2ImgVSz, RGB24);
	GLIB_ClearFrame(C_BLACK);	//YCbCr Data 0x00000000 -> RGB24 Data 0x00008100(not Block color)
	
#if (POST_IMAGE_SOURCE == POST_IMAGE_PATTERN)
	GLIB_Init(uGlibStAddr1, uImgHSz, uImgVSz, RGB24);
	GLIB_DrawPattern(uImgHSz, uImgVSz);
#elif (POST_IMAGE_SOURCE == POST_IMAGE_HEADER)
	POSTT_GetImageFromHeader(uGlibStAddr1, 240, 320, RGB24);
#endif

	// From RGB24 To YUV
	POST_InitIpForDmaInDmaOut(	uImgHSz, uImgVSz, uGlibStAddr1, RGB24, 
									uMidImgHSz, uMidImgVSz, uSrcStAddr, eSrcDataFmt_Yuv,
									1, FALSE, ONE_SHOT, &oPost);
	POST_StartProcessing(&oPost);
	while (!POST_IsProcessingDone(&oPost));

	// From YUV To YUV
	UART_Printf("========   YUV -> YUV   =========\n");
	UART_Printf("Source Address : 0x%08x\n", uSrcStAddr);
	UART_Printf("Destination Address : 0x%08x\n", uDstStAddr);	
	POST_InitIp1(	uMidImgHSz, uMidImgVSz, uMidStartX, uMidStartY, uMidScaledHSz, uMidScaledVSz, uSrcStAddr, eSrcDataFmt_Yuv,
					uMid2ImgHSz, uMid2ImgVSz, uMid2StartX, uMid2StartY, uMid2ScaledHSz, uMid2ScaledVSz, uDstStAddr, eDstDataFmt_Yuv,
					1, FALSE, ONE_SHOT, POST_DMA, POST_DMA, &oPost);
	
	POST_StartProcessing(&oPost);
	while (!POST_IsProcessingDone(&oPost));

	// From YUV To RGB24
#if 0	
	POST_InitIp1(	uMid2ImgHSz, uMid2ImgVSz, uMid2StartX, uMid2StartY, uMid2ScaledHSz, uMid2ScaledVSz, uDstStAddr, eDstDataFmt_Yuv,
					uImgHSz, uImgVSz, 0, 0, uImgHSz, uImgVSz, uLcdStAddr, RGB24,
					1, FALSE, ONE_SHOT, POST_DMA, POST_DMA, &oPost);	
#else
	POST_InitIpForDmaInDmaOut(	uMid2ImgHSz, uMid2ImgVSz, uDstStAddr, eDstDataFmt_Yuv, 
									uLcdHSz, uLcdVSz, uLcdStAddr, eDstDataFmt,
									1, FALSE, ONE_SHOT, &oPost);
#endif
	POST_StartProcessing(&oPost);
	while (!POST_IsProcessingDone(&oPost));
}

static void POSTT_ScaleUpDown_YUV(void)
{
	u32 uGlibStAddr1, uSrcStAddr, uDstStAddr;
	u32 uScaleOffsetX, uScaleOffsetY;
	s32 nSelDmaCscType_Y2Y;
	u32 uHOffset, uVOffset, uPreHOffset, uPreVOffset, uHRatio, uVRatio;	
	u8 uScale;	

	u32 i;
	u32 *pBufAddr32;

	UART_Printf("[POSTT_ScaleUpDown_YUV]\n");

	while(1)
	{
		UART_Printf("\n");
		UART_Printf("[1] YCbYCr422\n");
		UART_Printf("[2] YCrYCb422\n");
		UART_Printf("[3] CbYCrY422\n");
		UART_Printf("[4] CrYCbY422\n");
		UART_Printf("[5] YCbCr420\n");		
		UART_Printf("\n");

		UART_Printf(">>Select The Source Image Type: ");

		nSelDmaCscType_Y2Y = UART_GetIntNum();

		if (nSelDmaCscType_Y2Y >= 1 && nSelDmaCscType_Y2Y <= 5)
		{
			eSrcDataFmt_Yuv = 
				(nSelDmaCscType_Y2Y == 1) ? YCBYCR :
				(nSelDmaCscType_Y2Y == 2) ? YCRYCB :
				(nSelDmaCscType_Y2Y == 3) ? CBYCRY :
				(nSelDmaCscType_Y2Y == 4) ? CRYCBY : YC420;
#if 0
			if(nSelDmaCscType_Y2Y < 5)
				uOffset = 4;		//SRC_Width & DST_Width must be the multiple of 2 (YCbYCr422)
			else
				uOffset = 8;		//SRC_Width & DST_Width must be the multiple of 8 (YCbYCr420)
#endif
			break;
		}
		else 
			UART_Printf("Invalid Input! Retry It!!\n");
		
	}

	//LCD_InitLDI(MAIN);
	LCD_InitDISPC(RGB24, uLcdStAddr, WIN0, false);
	LCD_Start();
	LCD_SetWinOnOff(1, WIN0);
	LCD_GetFrmSz(&uLcdHSz, &uLcdVSz, WIN0);
	
	uImgHSz = uLcdHSz;	
	uImgVSz = uLcdVSz;
	uSrcStartX = 0; 
	uSrcStartY = 0;
	uSrcCroppedHSz = uLcdHSz;	
	uSrcCroppedVSz = uLcdVSz;
	
	uMidImgHSz = 2048;
	uMidImgVSz = 2048;
	uMidStartX = 0;
	uMidStartY = 0;
	uMidScaledHSz = 2048;
	uMidScaledVSz = 2048;
		
	uDstStartX = 0; 
	uDstStartY = 0;
	uDstScaledHSz = uLcdHSz, 
	uDstScaledVSz = uLcdVSz;	
	
	//uGlibStAddr1 = uLcdStAddr + uImgHSz*uImgVSz*4;
	uGlibStAddr1 = uLcdStAddr + uLcdHSz*uLcdVSz*4;
	uSrcStAddr = uGlibStAddr1 + uImgHSz*uImgVSz*4;
	uDstStAddr = uSrcStAddr + uMidImgHSz*uMidImgVSz*4;
	
	// To Clear the LCD Display
	GLIB_Init(uLcdStAddr, uLcdHSz, uLcdVSz, RGB24);
	GLIB_ClearFrame(C_BLACK);

	// To Clear the uDstStAddr
	//GLIB_Init(uDstStAddr, uMid2ImgHSz, uMid2ImgVSz, RGB24);
	//GLIB_ClearFrame(C_BLACK);	//YCbCr Data 0x00000000 -> RGB24 Data 0x00008100(not Block color)
	
#if 0
	// To Make Test Image
	pBufAddr32 = (u32 *)uGlibStAddr1;
					
	for(i=0 ; i<uImgHSz*uImgVSz ; i++)
			*pBufAddr32++ = pSmile_2[i];
#endif 
		
	//GLIB_Init(uGlibStAddr1, uImgHSz, uImgVSz, RGB24);
	//GLIB_DrawPattern(uImgHSz, uImgVSz);
	POSTT_GetImageFromBMP(uGlibStAddr1, uImgHSz, uImgVSz, 20, RGB24);
	
	// From RGB24 To YUV(360*480)
	POST_InitIp1(	uImgHSz, uImgVSz, uSrcStartX, uSrcStartY, uSrcCroppedHSz, uSrcCroppedVSz, uGlibStAddr1, RGB24,
					uMidImgHSz, uMidImgVSz, uMidStartX, uMidStartY, uMidScaledHSz, uMidScaledVSz, uSrcStAddr, eSrcDataFmt_Yuv,
					1, FALSE, ONE_SHOT, POST_DMA, POST_DMA, &oPost);
	POST_StartProcessing(&oPost);
	while (!POST_IsProcessingDone(&oPost));

	uScaleOffsetX = uMidImgHSz/2;
	uScaleOffsetY = uMidScaledVSz/2;
	// From YUV To RGB..Display default image
	POST_InitIp1(	uMidImgHSz, uMidImgVSz, uMidStartX, uMidStartY, uMidScaledHSz-uScaleOffsetX, uMidScaledVSz-uScaleOffsetY, uSrcStAddr, eSrcDataFmt_Yuv,
					uLcdHSz, uLcdVSz, uDstStartX, uDstStartY, uLcdHSz, uLcdVSz, uLcdStAddr, RGB24,
					1, FALSE, ONE_SHOT, POST_DMA, POST_DMA, &oPost);
	POST_StartProcessing(&oPost);
	while (!POST_IsProcessingDone(&oPost));

	UART_Printf("=============================================\n");
	UART_Printf("'w' : Vertical Scale-up,  'x' : Vertical Scale-down\n");
	UART_Printf("'d' : Horizontal Scale-up,  'a' : Horizontal Scale-down\n");
	UART_Printf("=============================================\n");
	UART_Printf("Press 'q' key to exit\n");	
	while((uScale=UART_Getc()) != 'q')
	{
		switch (uScale)
		{
			//Vetical Scale Up
			case 'w' :
			case 'W' :	POST_CalcurateScaleOffset(	uMidScaledHSz-uScaleOffsetX-uPreHOffset, uMidScaledVSz-uScaleOffsetY-uPreVOffset, uDstScaledHSz, uDstScaledVSz, &oPost, 
													&uHOffset, &uVOffset, &uHRatio, &uVRatio, eSrcDataFmt_Yuv);
						if(uPreVOffset == uVOffset)
						{
							if(uScaleOffsetY < (uMidImgVSz-uPreVOffset))
								 uScaleOffsetY += uPreVOffset;
						}
						else
						{	
							if(uScaleOffsetY < (uMidImgVSz-uVOffset))
								uScaleOffsetY += uVOffset - (uScaleOffsetY%(uVOffset));
							uPreVOffset = uVOffset;
						}
						break;
			//Vetical Scale Down
			case 'x' :
			case 'X' :	POST_CalcurateScaleOffset(	uMidScaledHSz-uScaleOffsetX+uPreHOffset, uMidScaledVSz-uScaleOffsetY+uPreVOffset, uDstScaledHSz, uDstScaledVSz, &oPost, 
													&uHOffset, &uVOffset, &uHRatio, &uVRatio, eSrcDataFmt_Yuv);
						if(uPreVOffset == uVOffset)
						{
							if(uScaleOffsetY >= uPreVOffset)
								 uScaleOffsetY -= uPreVOffset;
						}
						else
						{	
							if(uScaleOffsetY >= uVOffset)
								uScaleOffsetY -= uVOffset - (uScaleOffsetY%(uVOffset));
							uPreVOffset = uVOffset;
						}
						break;	
			//Horizontal Scale Up	
			case 'd' :
			case 'D' :	POST_CalcurateScaleOffset(	uMidScaledHSz-uScaleOffsetX-uPreHOffset, uMidScaledVSz-uScaleOffsetY-uPreVOffset, uDstScaledHSz, uDstScaledVSz, &oPost, 
													&uHOffset, &uVOffset, &uHRatio, &uVRatio, eSrcDataFmt_Yuv);
						if(uPreHOffset == uHOffset)
						{
							if(uScaleOffsetX < (uMidImgHSz-uPreHOffset))
								 uScaleOffsetX += uPreHOffset;
						}
						else
						{	
							if(uScaleOffsetX < (uMidImgHSz-uHOffset))
								uScaleOffsetX += uHOffset - (uScaleOffsetX%(uHOffset));
							uPreHOffset = uHOffset;
						}
						break;	
			//Horizontal Scale Down						
			case 'a' :
			case 'A' :	POST_CalcurateScaleOffset(	uMidScaledHSz-uScaleOffsetX+uPreHOffset, uMidScaledVSz-uScaleOffsetY+uPreVOffset, uDstScaledHSz, uDstScaledVSz, &oPost, 
													&uHOffset, &uVOffset, &uHRatio, &uVRatio, eSrcDataFmt_Yuv);
						if(uPreHOffset == uHOffset)
						{
							if(uScaleOffsetX >= uPreHOffset)
								 uScaleOffsetX -= uPreHOffset;
						}
						else
						{	
							if(uScaleOffsetX >= uHOffset)
								uScaleOffsetX -= uHOffset - (uScaleOffsetX%(uHOffset));
							uPreHOffset = uHOffset;
						}							
			default : 	break;
		}
		
		UART_Printf("(%d*%d) -> (%d*%d)\n",uMidScaledHSz-uScaleOffsetX, uMidScaledVSz-uScaleOffsetY, uDstScaledHSz, uDstScaledVSz);

		// Scaling from YUV to YUV
		POST_InitIp1(	uMidImgHSz, uMidImgVSz, uMidStartX, uMidStartY, uMidScaledHSz-uScaleOffsetX, uMidScaledVSz-uScaleOffsetY, uSrcStAddr, eSrcDataFmt_Yuv,
						uLcdHSz, uLcdVSz, uDstStartX, uDstStartY, uLcdHSz, uLcdVSz, uDstStAddr, eSrcDataFmt_Yuv,
						1, FALSE, ONE_SHOT, POST_DMA, POST_DMA, &oPost);
		POST_StartProcessing1(0, 0, &oPost);
		while (!POST_IsProcessingDone(&oPost));

		// CSC from YUV to RGB to display
		POST_InitIp1(	uLcdHSz, uLcdVSz, uDstStartX, uDstStartY, uLcdHSz, uLcdVSz, uDstStAddr, eSrcDataFmt_Yuv,
						uLcdHSz, uLcdVSz, uDstStartX, uDstStartY, uLcdHSz, uLcdVSz, uLcdStAddr, RGB24,
						1, FALSE, ONE_SHOT, POST_DMA, POST_DMA, &oPost);
		POST_StartProcessing1(0, 0, &oPost);
		while (!POST_IsProcessingDone(&oPost));		
	}
}


const testFuncMenu y2y_menu[] =
{
	POSTT_SimpleDmaToDma_Y2Y,                         			"Simple DMA CSC Test From YUV To YUV",
	POSTT_ComplexIDmaToDma_Y2Y,						"Complex DMA CSC Test From YUV To YUV",
	POSTT_ScaleUpDown_YUV,							"Scale Up/Down YUV",
	0,                                          0
};


static void POSTT_ConvertYUVToYUV(void)
{
	u32 i;
	s32 uSel;
			
	while(1)
	{
		UART_Printf("\n");
		for (i=0; (int)(y2y_menu[i].desc)!=0; i++)
		{
			UART_Printf("%2d: %s\n", i, y2y_menu[i].desc);
		}

		UART_Printf("\n");
		UART_Printf("Select The Function To Test: ");

		uSel = UART_GetIntNum();
		UART_Printf("\n");

		if (uSel == -1)
			break;
		if (uSel>=0 && uSel<(sizeof(y2y_menu)/8-1))
			(y2y_menu[uSel].func)();
	}
}


////////////////////////////////////////////////////////////////////////////
//////////////////                         DMA to DMA                        ///////////////////
////////////////////////////////////////////////////////////////////////////


const testFuncMenu dma2dma_menu[] =
{
	POSTT_ConvertRGBToRGB,                        				"Convert RGB to RGB",
	POSTT_ConvertRGBToYUVToRGB,                       			"Convert RGB to YUV to RGB",
	POSTT_ConvertYUVToYUV,                         				"Convert YUV to YUV",
	0,                                          0
};


static void POSTT_ConvertDMAToDMAPath(void)
{
	u32 i;
	s32 uSel;
			
	while(1)
	{
		UART_Printf("\n");
		for (i=0; (int)(dma2dma_menu[i].desc)!=0; i++)
		{
			UART_Printf("%2d: %s\n", i, dma2dma_menu[i].desc);
		}

		UART_Printf("\n");
		UART_Printf("Select The Function To Test: ");

		uSel = UART_GetIntNum();
		UART_Printf("\n");

		if (uSel == -1)
			break;
		if (uSel>=0 && uSel<(sizeof(dma2dma_menu)/8-1))
			(dma2dma_menu[uSel].func)();
	}
}



////////////////////////////////////////////////////////////////////////////
//////////////////                         DMA to FIFO                        ///////////////////
////////////////////////////////////////////////////////////////////////////


static void POSTT_SetCscTypeDmaToFifo(void)
{
	int nSelSrcDataFmt;

	while (1)
	{
		UART_Printf("\n");
		UART_Printf("[1] RGB16\n");
		UART_Printf("[2] RGB24\n");
		UART_Printf("[3] YCbYCr422 Interleave\n");
		UART_Printf("[4] YCrYCb422 Interleave\n");
		UART_Printf("[5] CbYCrY422 Interleave\n");
		UART_Printf("[6] CrYCbY422 Interleave\n");
		UART_Printf("[7] YUV 420 Non-Interleave\n");
		UART_Printf(">> Enter Source Data Format: ");

		nSelSrcDataFmt = UART_GetIntNum();

		if (nSelSrcDataFmt >= 1 && nSelSrcDataFmt <= 7)
		{
			eSrcDataFmt = 
				(nSelSrcDataFmt == 1) ? RGB16 :
				(nSelSrcDataFmt == 2) ? RGB24 :
				(nSelSrcDataFmt == 3) ? YCBYCR :
				(nSelSrcDataFmt == 4) ? YCRYCB :
				(nSelSrcDataFmt == 5) ? CBYCRY :
				(nSelSrcDataFmt == 6) ? CRYCBY : YC420;
			break;
		}
		else 
			UART_Printf("Invalid Data Format! Retry It!!\n");
	}
#if 0 // jihyun
	while (true)
	{
		UART_Printf("\n");
		UART_Printf("[0] Exit\n");
		UART_Printf("[1] YUV444 (Y:10, CB:10 CR: 10) Interleave\n");
		UART_Printf("[2] RGB30 (R:10 G:10 B:10)\n");
		UART_Printf(">> Enter Destination Data Format: ");
		
		nSelDstDataFmt = UART_GetIntNum();

		if (nSelDstDataFmt == 0)
			return;
		else if (nSelDstDataFmt >= 1 && nSelDstDataFmt <= 2)
		{
			ePostFifoIf = 	(nSelDstDataFmt == 1) ? YUV : RGB;
			break;
		}
		else
			UART_Printf("Invalid Data Format! Retry It!!\n");
	}
#endif	

	if (eSrcDataFmt == RGB16)
	{
		uImgHSz = 240, uImgVSz = 320;

		//H-Size : Scale down(280->240),   V-Size : Scale down(360->320)
		uMidImgHSz = 360, uMidImgVSz = 480, uMidStartX = 40, uMidStartY = 80;
		uMidScaledHSz = 280, uMidScaledVSz = 360;
	}
	else if (eSrcDataFmt == RGB24)
	{
		uImgHSz = 240, uImgVSz = 320;
		
		//H-Size : Scale up(200->240),   V-Size : Scale up(240->320)
		uMidImgHSz = 360, uMidImgVSz = 480, uMidStartX = 40, uMidStartY = 80;
		uMidScaledHSz = 200, uMidScaledVSz = 240;
	}
	else if (eSrcDataFmt == YCBYCR)
	{
		uImgHSz = 240, uImgVSz = 320;
		
		//H-Size : Scale up(200->240),   V-Size : Scale down(360->320)
		uMidImgHSz = 360, uMidImgVSz = 480, uMidStartX = 160, uMidStartY = 120;
		uMidScaledHSz = 200, uMidScaledVSz = 360;
	}
	else if (eSrcDataFmt == YCRYCB)
	{
		uImgHSz = 240, uImgVSz = 320;

		//H-Size : Scale down(280->240),   V-Size : Scale up(280->320)
		uMidImgHSz = 360, uMidImgVSz = 480, uMidStartX = 0, uMidStartY = 0;
		uMidScaledHSz = 280, uMidScaledVSz = 280;
	}
	else if (eSrcDataFmt == CBYCRY)
	{
		uImgHSz = 240, uImgVSz = 320;

		//H-Size : Scale up(200->240),   V-Size : Scale up(280->320)
		uMidImgHSz = 360, uMidImgVSz = 480, uMidStartX = 40, uMidStartY = 80;
		uMidScaledHSz = 200, uMidScaledVSz = 280;
	}
	else if (eSrcDataFmt == CRYCBY)
	{
		uImgHSz = 240, uImgVSz = 320;

		//H-Size : Scale down(320->240),   V-Size : Scale down(440->320)
		uMidImgHSz = 360, uMidImgVSz = 480, uMidStartX = 40, uMidStartY = 40;
		uMidScaledHSz = 320, uMidScaledVSz = 440;
	}
	else if (eSrcDataFmt == YC420)
	{
		uImgHSz = 240, uImgVSz = 320;

		//H-Size : Scale up(200->240),   V-Size : Scale down(400->320)
		uMidImgHSz = 360, uMidImgVSz = 480, uMidStartX = 40, uMidStartY = 0;
		uMidScaledHSz = 320, uMidScaledVSz = 400;
	}

	UART_Printf("\n");
	if(uSimpleTest == 0)
	{
		UART_Printf("=========    Complex_Test Image Size    =========\n");
		UART_Printf("SrcImgHSz     = %d,       SrcImgVSz     = %d\n",uMidImgHSz,uMidImgVSz);
		UART_Printf("SrcStartX     = %d,         SrcStartY     = %d\n",uMidStartX,uMidStartY);
		UART_Printf("SrcCroppedHSz = %d,       SrcCroppedVSz = %d\n",uMidScaledHSz,uMidScaledVSz);
		UART_Printf("=================================================\n");
		UART_Printf("\n");
	}	
}


//////////
// Function Name : POSTT_SelectClockDivider
// Function Description : Set POST Clock Divider
// Input : 	None
// Output : 	None
void POSTT_SelectClockDivider(void)
{
	u32 uPostClockDivide;
	
	UART_Printf("[POSTT_SelectClockDivider]\n");

	UART_Printf("\n");
	UART_Printf("Select the source clock [0x01 ~ 0x3F] : ");
	
	uPostClockDivide = (u32)UART_GetIntNum();

	POST_SetClockDivide(uPostClockDivide, &oPost);
}



// One-shot mode isn't supported in local path
// so, Only Free-Run mode is supported in local path
// In local path, scaling isn't supported because LCDC doesn't support that feature, but POST side is able to scale
static void POSTT_SimpleDmaToFifo(void)
{
	u32 uGlibStAddr, uSrcStAddr;
	//u32 uSrcBytesPerPixel;

	UART_Printf("[POSTT_SimpleDmaToFifo]\n");

	uSimpleTest = 1;
	POSTT_SetCscTypeDmaToFifo();

#if 1		// Post -> FIMD WIN0 FIFO mode	
	//LCD_InitDISPCForFifoIn(IN_POST, WIN0);
	LCD_InitBase2((CLK_SRC)ECLK0_SRC, PNR_Parallel_RGB, CPU_16BIT);
	LCD_InitWinForFifoIn1(IN_POST, WIN0, LOCALIN_RGB);
	LCD_SetWinOnOff(1, WIN0);
	LCD_GetFrmSz(&uLcdHSz, &uLcdVSz, WIN0);
	LCD_Start();
#else		// Scaler -> FIMD WIN1 FIFO mode
	//LCD_InitDISPCForFifoIn(IN_POST, WIN0);
	LCD_InitBase2((CLK_SRC)ECLK0_SRC, PNR_Parallel_RGB, CPU_16BIT);
	LCD_InitWinForFifoIn1(IN_POST, WIN1, LOCALIN_RGB);
	LCD_SetWinOnOff(1, WIN1);
	LCD_GetFrmSz(&uLcdHSz, &uLcdVSz, WIN1);
	LCD_Start();
#endif

	uImgHSz = uLcdHSz;
	uImgVSz = uLcdVSz;
		
	//uSrcBytesPerPixel = (eSrcDataFmt == RGB16) ? 2 : 4;
	uGlibStAddr = uLcdStAddr + uLcdHSz*uLcdVSz*4;
	uSrcStAddr = uGlibStAddr + uLcdHSz*uLcdVSz*4;
			
	GLIB_Init(uGlibStAddr, uLcdHSz, uLcdVSz, RGB24);
	GLIB_DrawPattern(uLcdHSz, uLcdVSz);	

	POST_InitIpForDmaInDmaOut(	uLcdHSz, uLcdVSz, uGlibStAddr, RGB24, 
									uImgHSz, uImgVSz, uSrcStAddr, eSrcDataFmt,
									1, false, ONE_SHOT, &oPost);
	POST_StartProcessing(&oPost);
	while (!POST_IsProcessingDone(&oPost));
	
	POST_InitIpForDmaInFifoOut(uImgHSz, uImgVSz, uSrcStAddr, eSrcDataFmt, 
								uLcdHSz, uLcdVSz, RGB, 1, &oPost);
	POST_SetNextFrameStAddr(uSrcStAddr, 0, &oPost); // For Local Path
	POST_StartProcessing(&oPost);


#if 0
	while(1)
	{
		POSTT_SelectClockDivider();
		
		POST_StartProcessing(&oPost);
		while (!POST_IsProcessingDone(&oPost));

		UART_Printf("Another value test : Press any key...('q' : quit)\n");
		if(UART_Getc()=='q')
			break;
	}
	
	UART_Printf("Press Any Key..!\n");
	UART_Getc();

#endif
			
#if (POST_IMAGE_BMP_SLIDE)	
	{
		u32 uImageNum;
		UART_Printf("Input the any key to display the other image ('q' : quit)\n");
		uImageNum = 0;

		while(UART_Getc() != 'q')
		{
			POST_StopProcessingOfFreeRun(&oPost);	
			while (!POST_IsProcessingDone(&oPost));
			POSTT_GetImageFromBMP(uGlibStAddr, uLcdHSz, uLcdVSz, uImageNum, RGB24);
			
			POST_InitIpForDmaInDmaOut(	uLcdHSz, uLcdVSz, uGlibStAddr, RGB24, 
											uImgHSz, uImgVSz, uSrcStAddr, eSrcDataFmt,
											1, false, ONE_SHOT, &oPost);
			POST_StartProcessing(&oPost);
			while (!POST_IsProcessingDone(&oPost));
			
			POST_InitIpForDmaInFifoOut(uImgHSz, uImgVSz, uSrcStAddr, eSrcDataFmt, 
										uLcdHSz, uLcdVSz, RGB, 1, &oPost);
			POST_SetNextFrameStAddr(uSrcStAddr, 0, &oPost); // For Local Path
			POST_StartProcessing(&oPost);
			uImageNum++;
		}
	}
#endif	

	POST_StopProcessingOfFreeRun(&oPost);	
	while (!POST_IsProcessingDone(&oPost));	
}


static void POSTT_SimpleFreeRunDmaToFifo_Int(void)
{
	u32 uGlibStAddr, uPostStAddr, uGlibStAddr2, uPostStAddr2;
	CSPACE eTmpDataFmt = PAL1;
	u32 uPostIsrCount, uPostFrameCount;
	
	UART_Printf("[POSTT_SimpleFreeRunDmaToFifo_Int]\n");

	uSimpleTest = 1;
	POSTT_SetCscTypeDmaToFifo();
	
	bIsPollingOrInt = FALSE;	
	//bIsR2Y2RFlag = FALSE;	
	ePostRunMode = FREE_RUN;

//	eLcdLocalDataFmt = (eDstDataFmt_FifoOut == RGB30) ? LOCAL_IN_RGB : LOCAL_IN_YCBYCR;

	// RGB24 bpp as LCD input data format is only supported in Local path
	//LCD_InitLDI(MAIN);
	LCD_InitDISPCForFifoIn(IN_POST, WIN0);
	LCD_GetFrmSz(&uLcdHSz, &uLcdVSz, WIN0);
	LCD_SetWinOnOff(1, WIN0);
	LCD_Start();

	uImgHSz = uLcdHSz;
	uImgVSz = uLcdVSz;

	// For DisplayCurParam() func.
	uSrcCroppedHSz = uLcdHSz;
	uSrcCroppedVSz = uLcdVSz;
	uSrcStartX = 0;
	uSrcStartY = 0;

	uDstScaledHSz = uLcdHSz; 
	uDstScaledVSz = uLcdVSz;
	uDstStartX = 0;
	uDstStartY = 0;

	// Data format of src.: YCbYCr/CbYCrY422 intlv or YC420. so, src. img. have a 2bytes/pixel
	// Data Format of Dest. : YUV444 or RGB24. so, dest img. have a 4bytes/pixel
	uGlibStAddr = uLcdStAddr + uLcdHSz*uLcdVSz*4;
	uGlibStAddr2 = uGlibStAddr + uImgHSz*uImgVSz*4;

	// To generate image by using Glib, allocate memory for converting from RGB24 to YUV
	if (	eSrcDataFmt == YCBYCR || eSrcDataFmt == CBYCRY ||
		eSrcDataFmt == YCRYCB || eSrcDataFmt == CRYCBY ||		
		eSrcDataFmt == YC420)
	{
		uPostStAddr = uGlibStAddr2 + uImgHSz*uImgVSz*4;		
		uPostStAddr2 = uPostStAddr + uImgHSz*uImgVSz*4;		
		
		eTmpDataFmt = RGB24;
	}
	else
	{
		eTmpDataFmt = eSrcDataFmt;
	}

	GLIB_Init(uGlibStAddr, uImgHSz, uImgVSz, eTmpDataFmt);
	GLIB_DrawPattern(uImgHSz, uImgVSz);

	GLIB_Init(uGlibStAddr2, uImgHSz, uImgVSz, eTmpDataFmt);
	GLIB_DrawPattern2(uLcdHSz, uLcdVSz);
	
	if (	eSrcDataFmt == YCBYCR || eSrcDataFmt == CBYCRY ||
		eSrcDataFmt == YCRYCB || eSrcDataFmt == CRYCBY ||		
		eSrcDataFmt == YC420)
	{
		// Color space conversion from RGB24 to YUV for generated image by using GLIB
		POST_InitIpForDmaInDmaOut(	uImgHSz, uImgVSz, uGlibStAddr, eTmpDataFmt, 
										uImgHSz, uImgVSz, uPostStAddr, eSrcDataFmt,
										1, FALSE, ONE_SHOT, &oPost);
		POST_StartProcessing(&oPost);
		while (!POST_IsProcessingDone(&oPost));

		POST_InitIpForDmaInDmaOut(	uImgHSz, uImgVSz, uGlibStAddr2, eTmpDataFmt, 
										uImgHSz, uImgVSz, uPostStAddr2, eSrcDataFmt,
										1, FALSE, ONE_SHOT, &oPost);
		POST_StartProcessing(&oPost);
		while (!POST_IsProcessingDone(&oPost));

		uNxtAddr1 = uPostStAddr;
		uNxtAddr2 = uPostStAddr2;
	}
	else
	{
		uNxtAddr1 = uGlibStAddr;
		uNxtAddr2 = uGlibStAddr2;
	}

	//Interrupt Setting
	uPostIsrCount = 0;
	uPostFrameCount = 0;
	uPostIntOccured = FALSE;

	INTC_Enable(NUM_POST0);
	INTC_SetVectAddr(NUM_POST0, PostISRDma);
	POST_EnableInterrupt(POST_LEVEL_INT, &oPost);
	
	// Init For Local Path
	POST_InitIpForDmaInFifoOut(	uImgHSz, uImgVSz, uNxtAddr1, eSrcDataFmt, 
								uLcdHSz, uLcdVSz, RGB, 1, &oPost);
	POST_SetNextFrameStAddr(uNxtAddr2, 0, &oPost); // For Local Path
	POST_StartProcessing(&oPost);

	while(uPostFrameCount < uMaxFrameCnt)
	{
		if(uPostIntOccured == TRUE)
		{
			uPostIsrCount++;
			if(uPostIsrCount == 30)
			{
				uPostFrameCount++;
				
			 	if (uPostFrameCount % 2)
					POST_SetNextFrameStAddr(uNxtAddr1, 0, &oPost);
				else 
					POST_SetNextFrameStAddr(uNxtAddr2, 0, &oPost);

				if (uPostFrameCount == uMaxFrameCnt)
				{
					POST_StopProcessingOfFreeRun(&oPost);	
					while (!POST_IsProcessingDone(&oPost));	
					uPostIntOccured = FALSE;
					break;
				}
				uPostIsrCount = 0;
			}
			uPostIntOccured = FALSE;
		}
	}

	POST_DisableInterrupt(&oPost);
	POST_ClearPending(&oPost);	
	//INTC_Mask(INT_POST);	
	INTC_Disable(NUM_POST0);
}



static void POSTT_ComplexFreeRunDmaToFifo_Int(void)
{
	u32 uGlibStAddr, uPostStAddr, uGlibStAddr2, uPostStAddr2;
//	CSPACE eTmpDataFmt = PAL1;
	u32 uPostIsrCount, uPostFrameCount;
	
	UART_Printf("[POSTT_ComplexFreeRunDmaToFifo_Int]\n");

	uSimpleTest = 0;
	POSTT_SetCscTypeDmaToFifo();
	
	bIsPollingOrInt = FALSE;	
	//bIsR2Y2RFlag = FALSE;	
	ePostRunMode = FREE_RUN;
	eDstDataFmt = RGB24;

//	eLcdLocalDataFmt = (eDstDataFmt_FifoOut == RGB30) ? LOCAL_IN_RGB : LOCAL_IN_YCBYCR;

	// RGB24 bpp as LCD input data format is only supported in Local path
	//LCD_InitLDI(MAIN);
	LCD_InitDISPCForFifoIn(IN_POST, WIN0);
	LCD_GetFrmSz(&uLcdHSz, &uLcdVSz, WIN0);
	LCD_SetWinOnOff(1, WIN0);
	LCD_Start();

	//uBytesPerPixel = (eDstDataFmt == RGB16) ? 2: 4;
	
	// Data format of src.: YCbYCr/CbYCrY422 intlv or YC420. so, src. img. have a 2bytes/pixel
	// Data Format of Dest. : YUV444 or RGB24. so, dest img. have a 4bytes/pixel
	uGlibStAddr = uLcdStAddr + uLcdHSz*uLcdVSz*4;
	uGlibStAddr2 = uGlibStAddr + uImgHSz*uImgVSz*4;

	if( eSrcDataFmt == RGB24 )
	{
		uPostStAddr = uGlibStAddr2 + uImgHSz*uImgVSz*4;		
		uPostStAddr2 = uPostStAddr + uMidImgHSz*uMidImgVSz*4;		//RGB24	
		//eTmpDataFmt = RGB24;
	}	
	else
	{
		uPostStAddr = uGlibStAddr2 + uImgHSz*uImgVSz*4;		
		uPostStAddr2 = uPostStAddr + uMidImgHSz*uMidImgVSz*2;		//RGB16/YCbCr422/YCbCr420
		//eTmpDataFmt = RGB24;
	}

	GLIB_Init(uGlibStAddr, uImgHSz, uImgVSz, RGB24);
	GLIB_DrawPattern(uImgHSz, uImgVSz);

	GLIB_Init(uGlibStAddr2, uImgHSz, uImgVSz, RGB24);
	GLIB_DrawPattern2(uImgHSz, uImgVSz);

	// Color space conversion from RGB24 to YUV for generated image by using GLIB
	POST_InitIp1(	uImgHSz, uImgVSz, 0, 0, uImgHSz, uImgVSz, uGlibStAddr, RGB24,
					uMidImgHSz, uMidImgVSz, 0, 0, uMidImgHSz, uMidImgVSz, uPostStAddr, eSrcDataFmt,
					1, FALSE, ONE_SHOT, POST_DMA, POST_DMA, &oPost);
	//POST_InitIpForDmaInDmaOut(	uImgHSz, uImgVSz, uGlibStAddr, RGB24, 
	//								uImgHSz, uImgVSz, uPostStAddr, eSrcDataFmt,
	//								1, FALSE, ONE_SHOT, &oPost);
	POST_StartProcessing(&oPost);
	while (!POST_IsProcessingDone(&oPost));
	
	POST_InitIp1(	uImgHSz, uImgVSz, 0, 0, uImgHSz, uImgVSz, uGlibStAddr2, RGB24,
					uMidImgHSz, uMidImgVSz, 0, 0, uMidImgHSz, uMidImgVSz, uPostStAddr2, eSrcDataFmt,
					1, FALSE, ONE_SHOT, POST_DMA, POST_DMA, &oPost);
	//POST_InitIpForDmaInDmaOut(	uImgHSz, uImgVSz, uGlibStAddr2, RGB24, 
	//								uImgHSz, uImgVSz, uPostStAddr2, eSrcDataFmt,
	//								1, FALSE, ONE_SHOT, &oPost);
	POST_StartProcessing(&oPost);
	while (!POST_IsProcessingDone(&oPost));

	uNxtAddr1 = uPostStAddr;
	uNxtAddr2 = uPostStAddr2;

	//Interrupt Setting
	uPostIsrCount = 0;
	uPostFrameCount = 0;
	uPostIntOccured = FALSE;
	
	INTC_Enable(NUM_POST0);
	INTC_SetVectAddr(NUM_POST0, PostISRDma);
	POST_EnableInterrupt(POST_LEVEL_INT, &oPost);

	// Init For Local Path
	//POST_InitIp1(	uMidImgHSz, uMidImgVSz, uDstStartX, uDstStartY, uDstScaledHSz, uDstScaledVSz, uNxtAddr1, eSrcDataFmt,
	//				uLcdHSz, uLcdVSz, 0, 0, uLcdHSz, uLcdVSz, 0, RGB24,
	//				1, FALSE, FREE_RUN, POST_DMA, POST_FIFO, &oPost);	
	POST_InitIp1(	uMidImgHSz, uMidImgVSz, uMidStartX, uMidStartY, uMidScaledHSz, uMidScaledVSz, uNxtAddr1, eSrcDataFmt,
					uLcdHSz, uLcdVSz, 0, 0, uLcdHSz, uLcdVSz, 0, RGB24,
					1, FALSE, FREE_RUN, POST_DMA, POST_FIFO, &oPost);		
	//POST_InitIpForDmaInFifoOut(	uLcdHSz, uLcdVSz, uNxtAddr1, eSrcDataFmt, 
	//							uLcdHSz, uLcdVSz, RGB, 1, &oPost);
	POST_SetNextFrameStAddr(uNxtAddr2, 0, &oPost); // For Local Path
	POST_StartProcessing(&oPost);

	while(uPostFrameCount < uMaxFrameCnt)
	{
		if(uPostIntOccured == TRUE)
		{
			uPostIsrCount++;
			if(uPostIsrCount == 30)
			{
				uPostFrameCount++;
				
			 	if (uPostFrameCount % 2)
					POST_SetNextFrameStAddr(uNxtAddr1, 0, &oPost);
				else 
					POST_SetNextFrameStAddr(uNxtAddr2, 0, &oPost);

				if (uPostFrameCount == uMaxFrameCnt)
				{
					POST_StopProcessingOfFreeRun(&oPost);	
					while (!POST_IsProcessingDone(&oPost));	
					uPostIntOccured = FALSE;
					break;
				}
				uPostIsrCount = 0;
			}
			uPostIntOccured = FALSE;
		}
	}

	POST_DisableInterrupt(&oPost);
	POST_ClearPending(&oPost);	
	//INTC_Mask(INT_POST);	
	INTC_Disable(NUM_POST0);
}



const testFuncMenu dma2fifo_menu[] =
{
	POSTT_SimpleDmaToFifo,                                  				"Simple FreeRun Local CSC Test ",		
	POSTT_SimpleFreeRunDmaToFifo_Int,                 			"Simple FreeRun Local CSC Test By Using Interrupt",
	POSTT_ComplexFreeRunDmaToFifo_Int,                 				"Complex FreeRun Local CSC Test By Using Interrupt",
	0,                                          0
};


static void POSTT_ConvertDMAToFIFOPath(void)
{
	u32 i;
	s32 uSel;
			
	while(1)
	{
		UART_Printf("\n");
		for (i=0; (int)(dma2fifo_menu[i].desc)!=0; i++)
		{
			UART_Printf("%2d: %s\n", i, dma2fifo_menu[i].desc);
		}

		UART_Printf("\n");
		UART_Printf("Select The Function To Test: ");

		uSel = UART_GetIntNum();
		UART_Printf("\n");

		if (uSel == -1)
			break;
		if (uSel>=0 && uSel<(sizeof(dma2fifo_menu)/8-1))
			(dma2fifo_menu[uSel].func)();
	}
}



////////////////////////////////////////////////////////////////////////////
//////////////////                    Post Clock Selection                   ///////////////////
////////////////////////////////////////////////////////////////////////////

//////////
// Function Name : POSTT_SelectClockSource
// Function Description : Set POST Clock source
// Input : 	None
// Output : 	None

void POSTT_SelectClockSource(void)
{
	u32 uPostClockSel, uPLLExtClock; 
	POST_CLK_SRC uPLLExtSrc;

	UART_Printf("\n");	
	UART_Printf("[POSTT_SelectClockSource]\n");
	UART_Printf("0. HCLK(D)	1. PLL Ext Clock	2. 27MHz Ext Clock\n");
	UART_Printf("Select the source clock : ");
	
	uPostClockSel = (u32)UART_GetIntNum();
		
	if(uPostClockSel == 1)
	{
		UART_Printf("\n");
		UART_Printf("0. MOUTepll(D)	1. DOUTmpll	2. FINepll\n");
		UART_Printf("Select the PLL Ext Clock source clock : ");

		uPLLExtClock = (u32)UART_GetIntNum();

		switch(uPLLExtClock)
		{
			case 0 :	uPLLExtSrc = PLL_EXT_MOUTEPLL;
					break;
			case 1 :	uPLLExtSrc = PLL_EXT_DOUTMPLL;
					break;
			case 2 :	uPLLExtSrc = PLL_EXT_FINEPLL;
					break;
			default :  uPLLExtSrc = PLL_EXT_MOUTEPLL;
					  break;
		}
	}

	switch(uPostClockSel)
	{
		case 0 :	POST_SetClkSrc(HCLK, &oPost);
				break;
		case 1 :	POST_SetClkSrc(uPLLExtSrc, &oPost);
				break;
		case 2 :	POST_SetClkSrc(EXT_27MHZ, &oPost);
				break;
		default :	POST_SetClkSrc(HCLK, &oPost);
				break;
	}
}


////////////////////////////////////////////////////////////////////////////
//////////////////                    POST Convert Image                   ///////////////////
////////////////////////////////////////////////////////////////////////////
#if 0
static void POSTT_ConvertImage(void)
{
	u32 uHSz, uVSz;
	CSPACE eBpp = RGB24;
	u32 uFbStAddr = CODEC_MEM_ST;
	u32 uImgAddr;

	UART_Printf("[POSTT_ConvertImage]\n");

	uHSz = 240, uVSz = 320;
	
	uImgAddr = uFbStAddr + uHSz*uVSz*4;

	//LCD_InitLDI(MAIN);

#ifdef LCD_INIT_TEST	
	LCD_InitBase();
	LCD_Start();
	LCD_InitWin(eBpp, uImgHSz, uImgVSz, 0, 0, uImgHSz, uImgVSz, 0, 0, uFbStAddr, WIN0, false);
#else
	LCD_InitDISPC(eBpp, uFbStAddr, WIN0, false);
	LCD_Start();
#endif
	LCD_SetWinOnOff(1, WIN0);

	// Load Image.
	LoadFromFile(IMG_YC420, uImgAddr);

	// Post.
	POST_InitIpForDmaInDmaOut(uHSz, uVSz, uImgAddr, YC420, uHSz, uVSz, uFbStAddr, RGB24,
		1, false, ONE_SHOT, &oPost);
	POST_StartProcessing(&oPost);

}
#endif

////////////////////////////////////////////////////////////////////////////
//////////////////                       POST Async Clock                   ///////////////////
////////////////////////////////////////////////////////////////////////////
static void POSTT_ClockTest(	CLK_SRC eLcdClkSrc, u32 uLcdClkDiv, POST_CLK_SRC ePostClkSrc,
								u32 *uGlibStAddr0, u32 *uGlibStAddr1, u32 uNumOfImage)
{
	u32 uImageNum;

	UART_Printf("\n");
	if(eLcdClkSrc == SRC_HCLK)
		UART_Printf("1. Current LCD Clock : HCLK\n");
	else if(eLcdClkSrc ==  SRC_MOUT_EPLL)
			UART_Printf("2. Current LCD Clock : PLL_EXT_MOUTEPLL\n");
	else if(eLcdClkSrc ==  SRC_FIN_EPLL)
			UART_Printf("3. Current LCD Clock : PLL_EXT_FINEPLL\n");
	else if(eLcdClkSrc ==  SRC_DOUT_MPLL)
			UART_Printf("4. Current LCD Clock : PLL_EXT_DOUTMPLL\n");	
	else //(eLcdClkSrc == ECLK1_SRC)
		UART_Printf("5. Current LCD Clock : EXT_27MHz\n");
	
	if(ePostClkSrc == HCLK)
		UART_Printf("1. Current POST Clock : HCLK\n");
	else if(ePostClkSrc == PLL_EXT_MOUTEPLL)
		UART_Printf("2. Current POST Clock : PLL_EXT_MOUTEPLL\n");
	else if(ePostClkSrc == PLL_EXT_FINEPLL)
		UART_Printf("3. Current POST Clock : PLL_EXT_FINEPLL\n");
	else	 if(ePostClkSrc == PLL_EXT_DOUTMPLL)
		UART_Printf("4. Current POST Clock : PLL_EXT_DOUTMPLL\n");
	else	 //(ePostClkSrc == EXT_27MHZ)
		UART_Printf("5. Current POST Clock : EXT_27MHz\n");

	if(eLcdClkSrc ==  SRC_MOUT_EPLL)
		{
			if( (ePostClkSrc == PLL_EXT_FINEPLL) || (ePostClkSrc == PLL_EXT_DOUTMPLL) )
			{
				UART_Printf("===>This combination is not supported\n");
				return;
			}
		}
	else if(eLcdClkSrc ==  SRC_FIN_EPLL)
	{//FIN_EPLL(12MHz) can't be used on 800*480 LCD because it need 24MHz VCLK.
			if( (ePostClkSrc == PLL_EXT_MOUTEPLL) || (ePostClkSrc == PLL_EXT_DOUTMPLL) )
			{
				UART_Printf("===>This combination is not supported\n");
				return;
			}
		}
	else if(eLcdClkSrc ==  SRC_DOUT_MPLL)
		{
			if( (ePostClkSrc == PLL_EXT_MOUTEPLL) || (ePostClkSrc == PLL_EXT_FINEPLL) )
			{
				UART_Printf("===>This combination is not supported\n");	
				return;
			}
		}

	LCD_InitDISPCForFifoIn(IN_POST, WIN0);
//	LCD_InitBase2((CLK_SRC)ECLK0_SRC, PNR_Parallel_RGB, CPU_16BIT);
	//LCD_InitWinForFifoIn1(IN_POST, WIN0, LOCALIN_YCbCr);

	if(eLcdClkSrc == SRC_MOUT_EPLL)
	{
		SYSC_SetPLL(eEPLL, 32, 1, 2, 0);
			LCD_SelClkSrcFromSYSCON(MOUT_EPLL);
		SYSC_ClkSrc(eEPLL_FOUT);
	}
	else if(eLcdClkSrc == SRC_FIN_EPLL)
			LCD_SelClkSrcFromSYSCON(FIN_EPLL);
	else if(eLcdClkSrc == SRC_DOUT_MPLL)
			LCD_SelClkSrcFromSYSCON(DOUT_MPLL);
	
	LCD_SetClkVal(uLcdClkDiv);
	LCD_SetClkSrc(eLcdClkSrc);	
	
	LCD_SetWinOnOff(1, WIN0);
	LCD_GetFrmSz(&uLcdHSz, &uLcdVSz, WIN0);

	LCD_Start();

	POST_InitIpForDmaInFifoOut(uLcdHSz, uLcdVSz, (u32)uGlibStAddr0, RGB24, uLcdHSz, uLcdVSz, RGB, 1, &oPost);
	//POST_InitIpForDmaInFifoOut(uLcdHSz, uLcdVSz, (u32)uGlibStAddr0, RGB24, uLcdHSz, uLcdVSz, YUV, 1, &oPost);
	POST_SetNextFrameStAddr((u32)uGlibStAddr0, 0, &oPost); // For Local Path
	POST_SetClkSrc(ePostClkSrc, &oPost);
	POST_StartProcessing(&oPost);
	Delay(10000);

	uImageNum = 0;
#if 0	// Post On/Off repeat
	while(uImageNum<uNumOfImage)
	{	
		POST_StopProcessingOfFreeRun(&oPost);	
		while (!POST_IsProcessingDone(&oPost));	
		
		//DelayfrTimer(milli, 1);
		while(POST_CheckAutoLoadEnable(&oPost));
		while (!POST_IsProcessingDone(&oPost));
		
		POST_InitIpForDmaInFifoOut(uLcdHSz, uLcdVSz, (u32)uGlibStAddr1[uImageNum], RGB24, 
									uLcdHSz, uLcdVSz, RGB, 1, &oPost);
		POST_SetNextFrameStAddr((u32)uGlibStAddr1[uImageNum], 0, &oPost); // For Local Path
		POST_StartProcessing(&oPost);
		DelayfrTimer(milli, 500);
		uImageNum++;
	}
#else		// only update the Post Next Address
	while(uImageNum<uNumOfImage)
	{	
		POST_SetNextFrameStAddr((u32)uGlibStAddr1[uImageNum], 0, &oPost); // For Local Path
		Delay(10000);
		uImageNum++;
	}
#endif	
	POST_StopProcessingOfFreeRun(&oPost);	
	while (!POST_IsProcessingDone(&oPost));
	
}


static void POSTT_SyncAsyncClk(void)
{
	u32 *uGlibStAddr0;
	u32 i, j, uNumOfTestImage, uCount;
	u32 *uGlibStAddr1[30];
	
#if (LCD_MODULE_TYPE ==LTE480WV_RGB)
	u32 eLcdClk[5][2] = {	{SRC_HCLK, 4},
						{SRC_MOUT_EPLL, 3},
						{SRC_FIN_EPLL, 0},
						{SRC_DOUT_MPLL, 4},
						{SRC_27M, 0}};

#elif (LCD_MODULE_TYPE== LTV350QV_RGB)
	u32 eLcdClk[5][2] = { 	{SRC_HCLK, 21},
						{SRC_MOUT_EPLL, 21},
						{SRC_FIN_EPLL, 1},
						{SRC_DOUT_MPLL, 21},
						{SRC_27M, 5}};
#endif	

	POST_CLK_SRC ePostClk[5] = { HCLK, PLL_EXT_MOUTEPLL, PLL_EXT_FINEPLL, PLL_EXT_DOUTMPLL, EXT_27MHZ};

	uNumOfTestImage = sizeof(Image_Entertain)/4;
	uCount = 0;
	
	LCD_InitDISPCForFifoIn(IN_POST, WIN0);
	LCD_SetClkSrc((CLK_SRC)ECLK1_SRC);
	LCD_SetClkVal(5);	
	LCD_SetWinOnOff(1, WIN0);
	LCD_GetFrmSz(&uLcdHSz, &uLcdVSz, WIN0);

	uGlibStAddr0 = (u32 *)malloc(uLcdHSz*uLcdVSz*4);

	UART_Printf("Downloading the Image....\n");
	for(i=0 ; i<uNumOfTestImage ; i++)
	{
		uGlibStAddr1[i] = (u32 *)malloc(uLcdHSz*uLcdVSz*4);
		POSTT_GetImageFromBMP((u32)uGlibStAddr1[i], uLcdHSz, uLcdVSz, i, RGB24);
	}
	
	GLIB_Init((u32)uGlibStAddr0, uLcdHSz, uLcdVSz, RGB24);
	GLIB_DrawPattern(uLcdHSz, uLcdVSz);	

#if 1
	while(!UART_GetKey())
	{
		for(i=0 ; i<5 ; i++)
		{
			for(j=0 ; j<5 ; j++)
			{
				POSTT_ClockTest(	(CLK_SRC)eLcdClk[i][0],  eLcdClk[i][1], ePostClk[j], 
									(u32 *)uGlibStAddr0, (u32 *)uGlibStAddr1, uNumOfTestImage);
			}
		}
		uCount++;
		UART_Printf("\n");
		UART_Printf("Count : %d\n", uCount);
	}
#else
	{
		u32 uLcdClock, uPostClock;
	
		while(UART_Getc() != 'q')
		{
			UART_Printf(">>Select The LCD Clock[0~4]");
			uLcdClock = UART_GetIntNum();
			UART_Printf(">>Select The POST Clock[0~4]");
			uPostClock = UART_GetIntNum();
			
			POSTT_ClockTest(	(CLK_SRC)eLcdClk[uLcdClock][0],  eLcdClk[uLcdClock][1], eLcdClk[uLcdClock][2], 
									ePostClk[uPostClock], (u32 *)uGlibStAddr0, (u32 *)uGlibStAddr1, uNumOfTestImage);
		}
	}
#endif

	for(i=0 ; i<uNumOfTestImage ; i++)
	{
		free(uGlibStAddr1[i]);
	}
	
}


////////////////////////////////////////////////////////////////////////////
//////////////////                       POST Test Main                      ///////////////////
////////////////////////////////////////////////////////////////////////////


const testFuncMenu post_menu[] =
{
	POSTT_ConvertDMAToDMAPath,                        			"DMA to DMA",
	POSTT_ConvertDMAToFIFOPath,                                  	"DMA to FIFO",		
//	POSTT_AboveAll,                                   					"Test Above All",   
	POSTT_SyncAsyncClk,								"Sync & Async Clock Test",
	POSTT_SelectClockSource,							"Clock Source Selection",
	POSTT_SelectClockDivider,							"Clock Divider Selection",	
	0,                                          0
};
	
void POST_Test(void)
{
	u32 i;
	s32 uSel;

	UART_Printf("[POST_Test]\n");

	POST_InitCh(POST_A, &oPost);
	POST_InitCh(POST_B, &oSc);
	//POST_InitCh(POST_B, &oPost);
	//POST_InitCh(POST_A, &oSc);

	//LCD Setting
	LCD_SetPort();
	LCD_InitLDI(MAIN);	
	
	while(1)
	{
		UART_Printf("\n");
		for (i=0; (int)(post_menu[i].desc)!=0; i++)
		{
			UART_Printf("%2d: %s\n", i, post_menu[i].desc);
		}

		UART_Printf("\n");
		UART_Printf("Select The Function To Test: ");

		uSel = UART_GetIntNum();
		UART_Printf("\n");

		if (uSel == -1)
			break;
		if (uSel>=0 && uSel<(sizeof(post_menu)/8-1))
			(post_menu[uSel].func)();
	}

	LCD_Stop();
}
