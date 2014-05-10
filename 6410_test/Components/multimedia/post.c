/*----------------------------------------------------------------------
 *
 * Filename: post.cpp
 *
 * Contents: Implemention of "class POST"
 *
 * Authors: Dhrama
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

#define PostOutp32(offset, x) Outp32(sCh->m_uBaseAddr+offset, x)
#define PostInp32(offset, x)  x = Inp32(sCh->m_uBaseAddr+offset)

//POSTENVID
#define POST_START              (0x1U<<31) // khlee
#define POST_DISABLE            (0x0U<<31) 

//MODE Control register
#define AUTOLOAD_ENABLE         (0x1<<14)
#define POST_INT_ENABLE         (0x1<<7)
#define POST_PENDING            (0x1<<6)
#define IRQ_LEVEL               (0x1<<5)
#define H_CLK_INPUT              (0x0<<2)
#define EXT_CLK_0_INPUT          (0X1<<2)
#define EXT_CLK_1_INPUT          (0x3<<2)

//MODE Control register 2
#define ADDR_CHANGE_ENABLE      (0x0<<4)
#define ADDR_CHANGE_DISABLE     (0x1<<4)
#define CHANGE_AT_FIELD_END     (0x0<<3)
#define CHANGE_AT_FRAME_END     (0x1<<3)
#define SOFTWARE_TRIGGER        (0x0<<0)
#define HARDWARE_TRIGGER        (0x1<<0)

#define TSCLK_FREQ              (27*1000000); // TV Encoder input clk.


static POST_CLK_SRC ePost_ClkSrc;
static POST_CLK_SRC eScaler_ClkSrc;
static u32 Post_ClkDivider = 1;		// default, divide 2

enum POST_REGS
{
	POST_MODE_CTRL              = 0x00,
	PRESCALE_RATIO              = 0x04,
	PRESCALE_IMG_SIZE           = 0x08,
	SRC_IMG_SIZE                = 0x0C,
	MAIN_SCALE_RATIO_H          = 0x10,
	MAIN_SCALE_RATIO_V          = 0x14,
	DST_IMG_SIZE                = 0x18,
	PRESCALE_SHFACTOR           = 0x1C,
	ADDR_START_Y                = 0x20,
	ADDR_START_CB               = 0x24,
	ADDR_START_CR               = 0x28,
	ADDR_START_RGB              = 0x2C,
	ADDR_END_Y                  = 0x30,
	ADDR_END_CB                 = 0x34,
	ADDR_END_CR                 = 0x38,
	ADDR_END_RGB                = 0x3C,
	OFFSET_Y                    = 0x40,
	OFFSET_CB                   = 0x44,
	OFFSET_CR                   = 0x48,
	OFFSET_RGB                  = 0x4C,
	EXT_FB                      = 0x50, // Reserved in V2.4
	IN_FIFO_STATUS              = 0x50, // Special in v2.3

	// Added in v2.2
	NEXT_ADDR_START_Y           = 0X54,
	NEXT_ADDR_START_CB          = 0x58,
	NEXT_ADDR_START_CR          = 0x5C,
	NEXT_ADDR_START_RGB         = 0x60,
	NEXT_ADDR_END_Y             = 0x64,
	NEXT_ADDR_END_CB            = 0x68,
	NEXT_ADDR_END_CR            = 0x6C,
	NEXT_ADDR_END_RGB           = 0x70,

	// Added in v2.3/v2.4
	ADDR_START_OUT_CB           = 0x74,
	ADDR_START_OUT_CR           = 0x78,
	ADDR_END_OUT_CB             = 0x7c,
	ADDR_END_OUT_CR             = 0x80,
	OFFSET_OUT_CB               = 0x84,
	OFFSET_OUT_CR               = 0x88,
	NEXT_ADDR_START_OUT_CB      = 0x8C,
	NEXT_ADDR_START_OUT_CR      = 0x90,
	NEXT_ADDR_END_OUT_CB        = 0x94,
	NEXT_ADDR_END_OUT_CR        = 0x98,
	POST_START_VIDEO            = 0x9c,	// khlee
	POST_MODE_CTRL_2            = 0xA0
};


//////////
// Function Name : POST_InitCh
// Function Description : Set the Register Base Address
// Input : 	eCh - POST Channel Number
//			sCh - POST structure base address
// Output : 	None
void POST_InitCh(POST_CH eCh, POST *sCh)
{
	if ( eCh == POST_A )
		sCh->m_uBaseAddr = POST0_BASE;
	else if ( eCh == POST_B )
		sCh->m_uBaseAddr = TVSCALER_BASE;
	else
		Assert(0);

	ePost_ClkSrc = HCLK;
	eScaler_ClkSrc = HCLK;
}

//////////
// Function Name : POST_InitIpForDmaInDmaOut
// Function Description : Initialize the POST(DMA to DMA)
// Input : 	uSrcWidth - Source Image Width
//			uSrcHeight - Source Image Height
//			uSrcFrmSt - Base Address of the Source Image 
//			eSrcCSpace - Color Space ot the Source Image
//			uDstWidth - Source Image Width
//			uDstHeight - Source Image Height
//			uDstFrmSt - Base Address of the Source Image 
//			eDstCSpace - Color Space ot the Source Image
//			uSrcFrmBufNum - Frame buffer number
//			bIsDoubleBuf - FALSE in POST
//			eMode - POST running mode(ONE_SHOT or FREE_RUN)
//			sCh - POST structure base address
// Output : 	None
void POST_InitIpForDmaInDmaOut(
	u32 uSrcWidth, u32 uSrcHeight, u32 uSrcFrmSt, CSPACE eSrcCSpace,
	u32 uDstWidth, u32 uDstHeight, u32 uDstFrmSt, CSPACE eDstCSpace,
	u32 uSrcFrmBufNum, u8 bIsDoubleBuf, POST_RUN_MODE eMode, POST *sCh
	)
{
	POST_InitIp1(	uSrcWidth, uSrcHeight, 0, 0, uSrcWidth, uSrcHeight, uSrcFrmSt, eSrcCSpace,
					uDstWidth, uDstHeight, 0, 0, uDstWidth, uDstHeight, uDstFrmSt, eDstCSpace,
					uSrcFrmBufNum, bIsDoubleBuf, eMode, POST_DMA, POST_DMA,	sCh
	);
}

//////////
// Function Name : POST_InitIpForDmaInFifoOut
// Function Description : Initialize the POST(DMA to FIFO)
// Input : 	uSrcWidth - Source Image Width
//			uSrcHeight - Source Image Height
//			uSrcFrmSt - Base Address of the Source Image 
//			eSrcCSpace - Color Space ot the Source Image
//			uDstWidth - Source Image Width
//			uDstHeight - Source Image Height
//			eFifoIf - FIFO output image format
//			uSrcFrmBufNum - Frame buffer number
//			sCh - POST structure base address
// Output : 	None
void POST_InitIpForDmaInFifoOut(
	u32 uSrcWidth, u32 uSrcHeight, u32 uSrcFrmSt, CSPACE eSrcCSpace,
	u32 uDstWidth, u32 uDstHeight, POST_FIFO_IF eFifoIf, u32 uSrcFrmBufNum, POST *sCh
	)
{
	CSPACE eDstCSpace =
		(eFifoIf == RGB) ? RGB24 :
		(eFifoIf == YUV) ? YCBYCR : (CSPACE)0xffffffff;

	Assert(eDstCSpace != 0xffffffff);

	POST_InitIp1(	uSrcWidth, uSrcHeight, 0, 0, uSrcWidth, uSrcHeight, uSrcFrmSt, eSrcCSpace,
					uDstWidth, uDstHeight, 0, 0, uDstWidth, uDstHeight, 0, 			eDstCSpace, 
					uSrcFrmBufNum, false, FREE_RUN, POST_DMA, POST_FIFO, sCh);
}

//////////
// Function Name : POST_InitIpForFifoInDmaOut
// Function Description : Initialize the POST(FIFO to DMA)
// Input : 	uSrcWidth - Source Image Width
//			uSrcHeight - Source Image Height
//			uDstWidth - Source Image Width
//			uDstHeight - Source Image Height
//			uDstFrmSt - Base Address of the Source Image 
//			eDstCSpace - Color Space ot the Source Image
//			bIsDoubleBuf - FALSE in POST
//			eMode - POST running mode(ONE_SHOT or FREE_RUN)
//			sCh - POST structure base address
// Output : 	None
void POST_InitIpForFifoInDmaOut(
	u32 uSrcWidth, u32 uSrcHeight, 
	u32 uDstWidth, u32 uDstHeight, u32 uDstFrmSt, CSPACE eDstCSpace, u8 bIsDoubleBuf,
	POST_RUN_MODE eMode, POST *sCh)
{
	POST_InitIp1(	uSrcWidth, uSrcHeight, 0, 0, uSrcWidth, uSrcHeight, 0, (CSPACE)0,
					uDstWidth, uDstHeight, 0, 0, uDstWidth, uDstHeight, uDstFrmSt, 
					eDstCSpace,	0, bIsDoubleBuf, eMode, POST_FIFO, POST_DMA, sCh);
}

//////////
// Function Name : POST_InitIpForFifoInFifoOut
// Function Description : Initialize the POST(FIFO to FIFO)
// Input : 	uSrcWidth - Source Image Width
//			uSrcHeight - Source Image Height
//			uDstWidth - Source Image Width
//			uDstHeight - Source Image Height
//			eFifoIf - FIFO output image format
//			eMode - POST running mode(ONE_SHOT or FREE_RUN)
//			sCh - POST structure base address
// Output : 	None
void POST_InitIpForFifoInFifoOut(
	u32 uSrcWidth, u32 uSrcHeight,
	u32 uDstWidth, u32 uDstHeight, POST_FIFO_IF eFifoIf, POST_RUN_MODE eMode, POST *sCh)
{
	CSPACE eDstCSpace =
		(eFifoIf == RGB) ? RGB24 :
		(eFifoIf == YUV) ? YCBYCR : (CSPACE)0xffffffff;

	Assert(eDstCSpace != 0xffffffff);
	
	POST_InitIp1(	uSrcWidth, uSrcHeight, 0, 0, uSrcWidth, uSrcHeight, 0, (CSPACE)0,
					uDstWidth, uDstHeight, 0, 0, uDstWidth, uDstHeight, 0, eDstCSpace,
					0, false, eMode, POST_FIFO, POST_FIFO, sCh);
}


//////////
// Function Name : POST_StartProcessing
// Function Description : Start the POST processing
// Input : 	sCh - POST structure base address
// Output : 	None
void POST_StartProcessing(POST *sCh)
{
	POST_StartProcessing1(0, 0, sCh);
}


//////////
// Function Name : POST_StartProcessing1
// Function Description : Start the POST processing
// Input : 	uSrcFrmIdx - Source Frame Index	
//			sCh - POST structure base address
// Output : 	None
void POST_StartProcessing1(u32 uSrcFrmIdx, u32 uDstBufIdx, POST *sCh)
{
	u8 isInDMAMode = ( sCh->m_uModeRegValue & (1U<<31) ) ? false : true;
	
	if ( isInDMAMode   && uSrcFrmIdx != 0 ) //if DMA input (N custom buffer)
	{
		Assert( uSrcFrmIdx <= sCh->m_uLastFrmBufIdx );
		
		// Set reg.s ref. of Current Y
		PostOutp32( ADDR_START_Y, sCh->m_uSrcStY[uSrcFrmIdx] );
		PostOutp32( ADDR_END_Y, sCh->m_uSrcEndY[uSrcFrmIdx] );

		if ( sCh->m_eSrcCSpace == YC420 )
		// Set reg.s ref. of Current Cb
		{
			PostOutp32( ADDR_START_CB, sCh->m_uSrcStCb[uSrcFrmIdx] );
			PostOutp32( ADDR_END_CB, sCh->m_uSrcEndCb[uSrcFrmIdx] );

			// Set reg.s ref. of Current Cr
			PostOutp32( ADDR_START_CR, sCh->m_uSrcStCr[uSrcFrmIdx] );
			PostOutp32( ADDR_END_CR, sCh->m_uSrcEndCr[uSrcFrmIdx] );
		}
	}

	PostOutp32( POST_START_VIDEO, POST_START ); // khlee
}


//////////
// Function Name : POST_SetDataPath
// Function Description : Set the POST proceesing path
// Input : 	eInPath - Data path of the source image
//			eOutPath - Data path of the desitination image
//			sCh - POST structure base address
// Output : 	None
void POST_SetDataPath(POST_PATH eInPath, POST_PATH eOutPath, POST *sCh)
{
	//rb1004
	PostInp32(POST_MODE_CTRL, sCh->m_uModeRegValue);
	
	sCh->m_uModeRegValue &= ~(0x1<<12); // 0: progressive mode, 1: interlace mode
	if ( eInPath == POST_FIFO )
		sCh->m_uModeRegValue |= (0x1U<<31);
	else if (eInPath == POST_DMA )
		sCh->m_uModeRegValue &= ~(0x1U<<31);
	if ( eOutPath == POST_FIFO )
		sCh->m_uModeRegValue |= (0x1<<13);
	else if (eOutPath == POST_DMA )
		sCh->m_uModeRegValue &= ~(0x1<<13);
	PostOutp32( POST_MODE_CTRL, sCh->m_uModeRegValue );
}


//////////
// Function Name : POST_SetDataFormat
// Function Description : Set the POST proceesing data format
// Input : 	eSrcCSpace - Color Space ot the Source Image
//			eDstCSpace - Color Space ot the Destination Image
//			eInPath - Data path of the source image
//			eOutPath - Data path of the desitination image
//			sCh - POST structure base address
//			uPixelSzIn - Source image Pixel byte size[bytes]
//			uPixelSzOut - Destination image Pixel byte size[bytes]
// Output : 	None
void POST_SetDataFormat(	CSPACE eSrcCSpace, CSPACE eDstCSpace, POST_PATH eInPath, POST_PATH eOutPath, POST *sCh,
								u32 *uPixelSzIn, u32 *uPixelSzOut)
{
	//u32 uPixelSzIn, uPixelSzOut;
	//rb1004
	PostInp32(POST_MODE_CTRL, sCh->m_uModeRegValue);
	
	sCh->m_uModeRegValue |= (0x1<<16); // R2YSel = 1;
	sCh->m_uModeRegValue |= (0x2<<10); // Wide = 2'b10;
	
	if ( eInPath == POST_DMA )
	{
		if( eSrcCSpace == YC420 )  
		{
			sCh->m_uModeRegValue &= ~((0x1<<3)|(0x1<<2));
			sCh->m_uModeRegValue |= (0x1<<8)|(0x1<<1);
			*uPixelSzIn = 1;
		}
		else if ( eSrcCSpace == CRYCBY )  
		//else if ( eSrcCSpace == YCBYCR )  
		{
			sCh->m_uModeRegValue &= ~((0x1<<15)|(0x1<<8)|(0x1<<3)|(0x1<<0));
			sCh->m_uModeRegValue |= (0x1<<2)|(0x1<<1);
			*uPixelSzIn = 2;
		}		
		else if ( eSrcCSpace == CBYCRY )  
		//else if ( eSrcCSpace == YCRYCB )  
		{
			sCh->m_uModeRegValue &= ~((0x1<<8)|(0x1<<3)|(0x1<<0));
			sCh->m_uModeRegValue |= (0x1<<15)|(0x1<<2)|(0x1<<1);
			*uPixelSzIn = 2;
		}		
		else if ( eSrcCSpace == YCRYCB )  
		//else if ( eSrcCSpace == CBYCRY )  
		{
			sCh->m_uModeRegValue &= ~((0x1<<15)|(0x1<<8)|(0x1<<3));
			sCh->m_uModeRegValue |= (0x1<<2)|(0x1<<1)|(0x1<<0);
			*uPixelSzIn = 2;
		}		
		else if ( eSrcCSpace == YCBYCR )  
		//else if ( eSrcCSpace == CRYCBY )  
		{
			sCh->m_uModeRegValue &= ~((0x1<<8)|(0x1<<3));
			sCh->m_uModeRegValue |= (0x1<<15)|(0x1<<2)|(0x1<<1)|(0x1<<0);	
			*uPixelSzIn = 2;
		}
		else if ( eSrcCSpace == RGB24 )  
		{
			sCh->m_uModeRegValue &= ~(0x1<<8);
			sCh->m_uModeRegValue |=  (0x1<<3)|(0x1<<2)|(0x1<<1);
			*uPixelSzIn = 4;
		}
		else if ( eSrcCSpace == RGB16 )  
		{
			sCh->m_uModeRegValue &= ~((0x1<<8)|(0x1<<1));
			sCh->m_uModeRegValue |=  (0x1<<3)|(0x1<<2);
			*uPixelSzIn = 2;
		}

	}
	else if ( eInPath == POST_FIFO )
	{
		// Do nothing
 	}
	if ( eOutPath == POST_DMA )
	{
		if( eDstCSpace == YC420 )  
		{
			sCh->m_uModeRegValue &= ~(0x1<<18);
			sCh->m_uModeRegValue |= (0x1<<17);
			*uPixelSzOut = 1;
		}
		else if ( eDstCSpace == CRYCBY ) 
		//else if ( eDstCSpace == YCBYCR ) 
		{
			sCh->m_uModeRegValue &= ~((0x1<<20)|(0x1<<19)|(0x1<<18)|(0x1<<17));
			*uPixelSzOut = 2;
		}		
		else if ( eDstCSpace == CBYCRY )  
		//else if ( eDstCSpace == YCRYCB )  
		{
			sCh->m_uModeRegValue &= ~((0x1<<19)|(0x1<<18)|(0x1<<17));
			sCh->m_uModeRegValue |= (0x1<<20);
			*uPixelSzOut = 2;
		}		
		else if ( eDstCSpace == YCRYCB )  
		//else if ( eDstCSpace == CBYCRY )  
		{
			sCh->m_uModeRegValue &= ~((0x1<<20)|(0x1<<18)|(0x1<<17));
			sCh->m_uModeRegValue |= (0x1<<19);
			*uPixelSzOut = 2;
		}		
		else if ( eDstCSpace == YCBYCR )  
		//else if ( eDstCSpace == CRYCBY )  
		{
			sCh->m_uModeRegValue &= ~((0x1<<18)|(0x1<<17));
			sCh->m_uModeRegValue |= (0x1<<20)|(0x1<<19);	
			*uPixelSzOut = 2;
		}
		else if ( eDstCSpace == RGB24 )  
		{
			sCh->m_uModeRegValue |= (0x1<<18)|(0x1<<4);
			*uPixelSzOut = 4;
		}
		else if ( eDstCSpace == RGB16 )  
		{
			sCh->m_uModeRegValue &= ~(0x1<<4);
			sCh->m_uModeRegValue |= (0x1<<18);
			*uPixelSzOut = 2;
		}
	}
	else if ( eOutPath == POST_FIFO )
	{
		if ( eDstCSpace == RGB24 ) // Actually, RGB24 means a RGB30 of FIFO interface
		{
			sCh->m_uModeRegValue |= (0x1<<18)|(0x1<<13); // OutRGB,OutRGBFormat
			//m_uModeRegValue &= ~(0x1<<17);
		}
		else if ( eDstCSpace == YCBYCR ) // Actually, YCBYCR means a YUV444 of FIFO interface
		{
			sCh->m_uModeRegValue |= (0x1<<13);
			sCh->m_uModeRegValue &= ~(0x1<<18)|(0x1<<17);
		}
		else
			Assert(0);
	}

	PostOutp32( POST_MODE_CTRL, sCh->m_uModeRegValue );
}


//////////
// Function Name : POST_SetScaler
// Function Description : Set the POST proceesing scaling factor
// Input : 	uSrcWidth - Source Image Width
//			uSrcHeight - Source Image Height
//			uDstWidth - Source Image Width
//			uDstHeight - Source Image Height
//			sCh - POST structure base address
// Output : 	None
void POST_SetScaler(u32 uSrcWidth, u32 uSrcHeight, u32 uDstWidth, u32 uDstHeight, POST *sCh)
{
	u32 uPreHratio, uPreVratio, uHshift, uVshift, uShFactor;
	u32 uPreDstWidth, uPreDstHeight, uDx, uDy;
	
	uPreHratio=0, uPreVratio=0, uHshift=0, uVshift=0, uShFactor=0;
	uPreDstWidth=0, uPreDstHeight=0, uDx=0, uDy=0;

	Assert( !(uSrcWidth >= (uDstWidth<<6)) );
	Assert( !(uSrcHeight >= (uDstHeight<<6)) );
	
	if ( uSrcWidth >= (uDstWidth<<5) )
		uPreHratio = 32, uHshift= 5;
	else if ( uSrcWidth >= (uDstWidth<<4) )
		uPreHratio = 16, uHshift = 4;
	else if ( uSrcWidth >= (uDstWidth<<3) )
		uPreHratio = 8, uHshift = 3;
	else if ( uSrcWidth >= (uDstWidth<<2) )
		uPreHratio = 4, uHshift = 2;
	else if ( uSrcWidth >= (uDstWidth<<1) )
		uPreHratio = 2, uHshift = 1;
	else
		uPreHratio = 1, uHshift = 0;
	
	uPreDstWidth = uSrcWidth / uPreHratio;
	uDx = (uSrcWidth<<8) / (uDstWidth<<uHshift);

	if ( uSrcHeight >= (uDstHeight<<5) )
		uPreVratio = 32, uVshift= 5;
	else if ( uSrcHeight >= (uDstHeight<<4) )
		uPreVratio = 16, uVshift = 4;
	else if ( uSrcHeight >= (uDstHeight<<3) )
		uPreVratio = 8, uVshift = 3;
	else if ( uSrcHeight >= (uDstHeight<<2) )
		uPreVratio = 4, uVshift = 2;
	else if ( uSrcHeight >= (uDstHeight<<1) )
		uPreVratio = 2, uVshift = 1;
	else
		uPreVratio = 1, uVshift = 0;
	
	uPreDstHeight = uSrcHeight / uPreVratio;
	uDy = (uSrcHeight<<8) / (uDstHeight<<uVshift);
	uShFactor = 10 - (uHshift + uVshift);

	//rb1004
	//SRC_Width = 4*Prescale_H_Ratio
	//SRC_Height = 2*Prescale_V_Ratio
	Assert( !(uSrcWidth%(4*uPreHratio)));
	Assert( !(uSrcHeight%(2*uPreVratio)));
	Assert( uPreDstWidth   <= 2048 || uPreDstHeight <= 2048);
	
	PostOutp32( PRESCALE_RATIO, ((uPreVratio<<7)|(uPreHratio<<0)) ); //prescale
	PostOutp32( PRESCALE_IMG_SIZE, ((uPreDstHeight<<12)|(uPreDstWidth<<0)) );
	PostOutp32( PRESCALE_SHFACTOR, uShFactor ); //shift factor
	PostOutp32( MAIN_SCALE_RATIO_H, uDx ); //main scale
	PostOutp32( MAIN_SCALE_RATIO_V, uDy );

	PostOutp32( SRC_IMG_SIZE, (((uSrcHeight&0xfff)<<12)|((uSrcWidth&0xfff)<<0))); //image size
	//PostOutp32( SRC_IMG_SIZE, ((uSrcHeight<<12)|(uSrcWidth<<0)) ); 
	PostOutp32( DST_IMG_SIZE, ((uDstHeight<<12)|(uDstWidth<<0)) );
}


//////////
// Function Name : POST_SetDMA
// Function Description : Set the POST proceesing about the DMA path
// Input :	uSrcFullWidth - Source Image Full Width(Virtual screen size)
//			uSrcFullHeight - Source Image Full Height(Virtual screen size)
//			uSrcStartX - Source Image Start width offset
//			uSrcStartY - Source Image Start height offset
//			uSrcWidth - Source Image Width
//			uSrcHeight - Source Image Height
//			uSrcFrmSt - Base Address of the Source Image 
//			eSrcCSpace - Color Space ot the Source Image
//			uDstFullHeight - Source Image Full Width(Virtual screen size)
//			uSrcFullHeight - Source Image Full Height(Virtual screen size)
//			uDstStartX - Source Image Start width offset
//			uDstStartY - Source Image Start height offset
//			uDstWidth - Source Image Width
//			uDstHeight - Source Image Height
//			uDstFrmSt - Base Address of the Source Image 
//			eDstCSpace - Color Space ot the Source Image
//			uSrcFrmBufNum - Frame buffer number
//			eInPath - Data path of the source image
//			eOutPath - Data path of the desitination image
//			sCh - POST structure base address
//			uPixelSzIn - Source image Pixel byte size[bytes]
//			uPixelSzOut - Destination image Pixel byte size[bytes]
// Output : 	None
void POST_SetDMA(	u32 uSrcFullWidth, u32 uSrcFullHeight, u32 uSrcStartX, u32 uSrcStartY, u32 uSrcWidth, u32 uSrcHeight, u32 uSrcFrmSt, CSPACE eSrcCSpace,
						u32 uDstFullWidth, u32 uDstFullHeight, u32 uDstStartX, u32 uDstStartY, u32 uDstWidth, u32 uDstHeight, u32 uDstFrmSt, CSPACE eDstCSpace,
						u32 uSrcFrmBufNum,  POST_PATH eInPath, POST_PATH eOutPath, POST *sCh,
						u32 uPixelSzIn, u32 uPixelSzOut)
{
	u32 uOffsetRgb, uDstStRgb, uDstEndRgb;
	u32 uOutSrcStCb;
	u32 uOutSrcEndCb;
	u32 uOutSrcStCr;
	u32 uOutSrcEndCr;
	
	sCh->m_uLastFrmBufIdx = uSrcFrmBufNum - 1;
	sCh->m_uSrcStY[0]=0;
	sCh->m_uSrcEndY[0]=0;
	sCh->m_uSrcStCb[0]=0;
	sCh->m_uSrcEndCb[0]=0;
	sCh->m_uSrcStCr[0]=0;
	sCh->m_uSrcEndCr[0]=0;
	if ( eInPath == POST_DMA )
	{	
		u32 uOffsetY = (uSrcFullWidth - uSrcWidth)*uPixelSzIn;
		u32 i;
		u32 uOffsetCb, uOffsetCr;

		sCh->m_uStPosY = (uSrcFullWidth*uSrcStartY+uSrcStartX)*uPixelSzIn;
		sCh->m_uEndPosY = uSrcWidth*uSrcHeight*uPixelSzIn + uOffsetY*(uSrcHeight-1);
		sCh->m_uSrcFrmStAddr[0] = uSrcFrmSt;
		sCh->m_uSrcStY[0] = uSrcFrmSt + sCh->m_uStPosY;
		sCh->m_uSrcEndY[0]= sCh->m_uSrcStY[0]+ sCh->m_uEndPosY;
		PostOutp32( ADDR_START_Y, sCh->m_uSrcStY[0] );
		PostOutp32( OFFSET_Y, uOffsetY );
		PostOutp32( ADDR_END_Y, sCh->m_uSrcEndY[0] );

		for ( i=1; i<uSrcFrmBufNum; i++ )
		{
			sCh->m_uSrcFrmStAddr[i] = sCh->m_uSrcFrmStAddr[i-1] + uSrcFullWidth*uSrcFullHeight*uPixelSzIn;
			//sCh->m_uSrcStY[i] = sCh->m_uSrcEndY[i-1] + sCh->m_uStPosY;
			sCh->m_uSrcStY[i] = sCh->m_uSrcFrmStAddr[i] + sCh->m_uStPosY;	//rb1004????
			sCh->m_uSrcEndY[i] = sCh->m_uSrcStY[i] + sCh->m_uEndPosY;
		}

		if ( eSrcCSpace == YC420 )
		{
			uOffsetCb = uOffsetCr = ((uSrcFullWidth-uSrcWidth)/2)*uPixelSzIn;
			sCh->m_uStPosCb = uSrcFullWidth*uSrcFullHeight*1
				+ (uSrcFullWidth*uSrcStartY/2 + uSrcStartX)/2*1;
			//sCh->m_uEndPosCb = uSrcWidth/2*uSrcHeight/2*uPixelSzIn + (uSrcHeight/2-1)*uOffsetCr;
			sCh->m_uEndPosCb = uSrcWidth/2*uSrcHeight/2*uPixelSzIn + (uSrcHeight/2-1)*uOffsetCb;		//rb1004
			sCh->m_uStPosCr = uSrcFullWidth*uSrcFullHeight*1 + uSrcFullWidth*uSrcFullHeight/4*1
				+ (uSrcFullWidth*uSrcStartY/2 + uSrcStartX)/2*1;
			//sCh->m_uEndPosCr = uSrcWidth/2*uSrcHeight/2*uPixelSzIn + (uSrcHeight/2-1)*uOffsetCb;
			sCh->m_uEndPosCr = uSrcWidth/2*uSrcHeight/2*uPixelSzIn + (uSrcHeight/2-1)*uOffsetCr; 	//rb1004
			sCh->m_uSrcStCb[0] = uSrcFrmSt + sCh->m_uStPosCb;
			sCh->m_uSrcEndCb[0] = sCh->m_uSrcStCb[0] + sCh->m_uEndPosCb;
			sCh->m_uSrcStCr[0] = uSrcFrmSt + sCh->m_uStPosCr;
			sCh->m_uSrcEndCr[0] = sCh->m_uSrcStCr[0] + sCh->m_uEndPosCr;
			PostOutp32( ADDR_START_CB, sCh->m_uSrcStCb[0] );
			PostOutp32( OFFSET_CB, uOffsetCr );
			PostOutp32( ADDR_END_CB, sCh->m_uSrcEndCb[0] );
			PostOutp32( ADDR_START_CR, sCh->m_uSrcStCr[0] );
			PostOutp32( OFFSET_CR, uOffsetCb );
			PostOutp32( ADDR_END_CR, sCh->m_uSrcEndCr[0] );
			for ( i=1; i<uSrcFrmBufNum; i++ )
			{
				sCh->m_uSrcFrmStAddr[i] = sCh->m_uSrcFrmStAddr[i-1] + uSrcFullWidth*uSrcFullHeight*3/2;
 				sCh->m_uSrcStY[i] = sCh->m_uSrcFrmStAddr[i] + sCh->m_uStPosY;
				sCh->m_uSrcEndY[i] = sCh->m_uSrcStY[i] + sCh->m_uEndPosY;
  				sCh->m_uSrcStCb[i] = sCh->m_uSrcFrmStAddr[i] + sCh->m_uStPosCb;
				sCh->m_uSrcEndCb[i] = sCh->m_uSrcStCb[i] + sCh->m_uEndPosCb;
 				sCh->m_uSrcStCr[i] = sCh->m_uSrcFrmStAddr[i] + sCh->m_uStPosCr;
				sCh->m_uSrcEndCr[i] = sCh->m_uSrcStCr[i] + sCh->m_uEndPosCr;
			}
		}
	}
	if ( eOutPath == POST_DMA )
	{
		uOffsetRgb = (uDstFullWidth - uDstWidth)*uPixelSzOut;
		sCh->m_uStPosRgb = (uDstFullWidth*uDstStartY + uDstStartX)*uPixelSzOut;
		sCh->m_uEndPosRgb = uDstWidth*uDstHeight*uPixelSzOut + uOffsetRgb*(uDstHeight-1);
		uDstStRgb = uDstFrmSt + sCh->m_uStPosRgb;
		uDstEndRgb = uDstStRgb + sCh->m_uEndPosRgb;
		PostOutp32( ADDR_START_RGB, uDstStRgb );
		PostOutp32( OFFSET_RGB, uOffsetRgb );
		PostOutp32( ADDR_END_RGB, uDstEndRgb );

		if ( eDstCSpace == YC420 )
		{
			u32 uOutOffsetCb, uOutOffsetCr;
			uOutOffsetCb = uOutOffsetCr= ((uDstFullWidth-uDstWidth)/2)*uPixelSzOut;
			sCh->m_uOutStPosCb = uDstFullWidth*uDstFullHeight*1
				+ (uDstFullWidth*uDstStartY/2 + uDstStartX)/2*1;
			sCh->m_uOutEndPosCb = uDstWidth/2*uDstHeight/2*uPixelSzOut + (uDstHeight/2-1)*uOutOffsetCr;
			//sCh->m_uOutEndPosCb = uDstWidth/2*uDstHeight/2*uPixelSzOut + (uDstHeight/2-1)*uOutOffsetCb;		//rb1004
			sCh->m_uOutStPosCr = uDstFullWidth*uDstFullHeight*1 + uDstFullWidth*uDstFullHeight/4*1
				+ (uDstFullWidth*uDstStartY/2 + uDstStartX)/2*1;
			sCh->m_uOutEndPosCr = uDstWidth/2*uDstHeight/2*uPixelSzOut + (uDstHeight/2-1)*uOutOffsetCb;
			//sCh->m_uOutEndPosCr = uDstWidth/2*uDstHeight/2*uPixelSzOut + (uDstHeight/2-1)*uOutOffsetCr;		//rb1004

			uOutSrcStCb = uDstFrmSt + sCh->m_uOutStPosCb;
			uOutSrcEndCb = uOutSrcStCb + sCh->m_uOutEndPosCb;
			uOutSrcStCr = uDstFrmSt + sCh->m_uOutStPosCr;
			uOutSrcEndCr = uOutSrcStCr + sCh->m_uOutEndPosCr;
			PostOutp32( ADDR_START_OUT_CB, uOutSrcStCb );
			PostOutp32( OFFSET_OUT_CB, uOutOffsetCb );
			PostOutp32( ADDR_END_OUT_CB, uOutSrcEndCb );
			PostOutp32( ADDR_START_OUT_CR, uOutSrcStCr );
			PostOutp32( OFFSET_OUT_CR, uOutOffsetCr );
			PostOutp32( ADDR_END_OUT_CR, uOutSrcEndCr );
		}
	}
}


//////////
// Function Name : POST_SetAutoLoadEnable
// Function Description : Set the AutoLoad Mode
// Input : 	eMode - AutoLoad Mode (Per Frame mode or Freerun mode)
//			sCh - POST structure base address
// Output : 	None
void POST_SetAutoLoadEnable(POST_RUN_MODE eMode, POST *sCh)
{
	//rb1004
	PostInp32(POST_MODE_CTRL, sCh->m_uModeRegValue);
	
	if ( eMode == FREE_RUN )
	{
		sCh->m_uModeRegValue |= AUTOLOAD_ENABLE; 
	}
	else if( eMode == ONE_SHOT )
	{
		sCh->m_uModeRegValue &= ~AUTOLOAD_ENABLE; 
	}	
	PostOutp32( POST_MODE_CTRL, sCh->m_uModeRegValue );
}


//////////
// Function Name : POST_EnableInterrupt
// Function Description : Set the Interrupt mode
// Input : 	uIntLevel - Interrupt Mode (Edge Interrupt or Level Interrupt)
//			sCh - POST structure base address
// Output : 	None
void POST_EnableInterrupt(u32 uIntLevel, POST *sCh)
{
	//rb1004
	PostInp32(POST_MODE_CTRL, sCh->m_uModeRegValue);
	
	if(uIntLevel == POST_EDGE_INT)
		sCh->m_uModeRegValue &= ~IRQ_LEVEL;
	else if(uIntLevel == POST_LEVEL_INT)
		sCh->m_uModeRegValue |= IRQ_LEVEL;

	sCh->m_uModeRegValue |= POST_INT_ENABLE;
	PostOutp32( POST_MODE_CTRL, sCh->m_uModeRegValue );
}


//////////
// Function Name : POST_DisableInterrupt
// Function Description : Disable Interrupt
// Input : 	sCh - POST structure base address
// Output : 	None
void POST_DisableInterrupt(POST *sCh)
{
	//rb1004
	PostInp32(POST_MODE_CTRL, sCh->m_uModeRegValue);
	
	sCh->m_uModeRegValue &= ~POST_INT_ENABLE;
	PostOutp32( POST_MODE_CTRL, sCh->m_uModeRegValue );
}


//////////
// Function Name : POST_InitIp
// Function Description : Initialize the POST in the External Application(Virtual screen size = Image size)
// Input :	uSrcWidth - Source Image Width
//			uSrcHeight - Source Image Height
//			uSrcFrmSt - Base Address of the Source Image 
//			eSrcCSpace - Color Space ot the Source Image
//			uDstWidth - Source Image Width
//			uDstHeight - Source Image Height
//			uDstFrmSt - Base Address of the Source Image 
//			eDstCSpace - Color Space ot the Source Image
//			uSrcFrmBufNum - Frame buffer number
//			bIsDoubleBuf - Double buffer
//			eMode - POST running mode(PER_FRAME or FREE_RUN)
//			sCh - POST structure base address
// Output : 	None
void POST_InitIp(
	u32 uSrcWidth, u32 uSrcHeight, u32 uSrcFrmSt, CSPACE eSrcCSpace,
	u32 uDstWidth, u32 uDstHeight, u32 uDstFrmSt, CSPACE eDstCSpace,
	u32 uSrcFrmBufNum, u8 bIsDoubleBuf, POST_RUN_MODE eMode, POST *sCh
	)
{
	POST_InitIp1(
		uSrcWidth, uSrcHeight, 0, 0, uSrcWidth, uSrcHeight, uSrcFrmSt, eSrcCSpace,
		uDstWidth, uDstHeight, 0, 0, uDstWidth, uDstHeight, uDstFrmSt, eDstCSpace,
		uSrcFrmBufNum, bIsDoubleBuf, eMode, POST_DMA, POST_DMA,	sCh
	);
}


//////////
// Function Name : POST_SetDMA
// Function Description : Initialize the POST in the External Application(Virtual screen size != Image size)
// Input :	uSrcFullWidth - Source Image Full Width(Virtual screen size)
//			uSrcFullHeight - Source Image Full Height(Virtual screen size)
//			uSrcStartX - Source Image Start width offset
//			uSrcStartY - Source Image Start height offset
//			uSrcWidth - Source Image Width
//			uSrcHeight - Source Image Height
//			uSrcFrmSt - Base Address of the Source Image 
//			eSrcCSpace - Color Space ot the Source Image
//			uDstFullHeight - Source Image Full Width(Virtual screen size)
//			uSrcFullHeight - Source Image Full Height(Virtual screen size)
//			uDstStartX - Source Image Start width offset
//			uDstStartY - Source Image Start height offset
//			uDstWidth - Source Image Width
//			uDstHeight - Source Image Height
//			uDstFrmSt - Base Address of the Source Image 
//			eDstCSpace - Color Space ot the Source Image
//			uSrcFrmBufNum - Frame buffer number
//			bIsDoubleBuf - Double buffer
//			eMode - POST running mode(PER_FRAME or FREE_RUN)
//			eInPath - Data path of the source image
//			eOutPath - Data path of the desitination image
//			sCh - POST structure base address
// Output : 	None
void POST_InitIp1(
	u32 uSrcFullWidth, u32 uSrcFullHeight, u32 uSrcStartX, u32 uSrcStartY, u32 uSrcWidth, u32 uSrcHeight, u32 uSrcFrmSt, CSPACE eSrcCSpace,
	u32 uDstFullWidth, u32 uDstFullHeight, u32 uDstStartX, u32 uDstStartY, u32 uDstWidth, u32 uDstHeight, u32 uDstFrmSt, CSPACE eDstCSpace,
	u32 uSrcFrmBufNum, u8 bIsDoubleBuf, POST_RUN_MODE eMode, POST_PATH eInPath, POST_PATH eOutPath, POST *sCh	)
{
	u32 uPostModeCtrl2;
	u32 uPixelSzIn, uPixelSzOut;

 	Assert( !bIsDoubleBuf ); // hojinz.kim
  	Assert( uSrcFullWidth   <= 4096 || uSrcFullHeight <= 4096);
	Assert( uDstFullWidth   <= 2048 || uDstFullHeight <= 2048);
	Assert( uSrcFullWidth >= uSrcStartX + uSrcWidth );
	Assert( uSrcFullHeight >= uSrcStartY + uSrcHeight );
	Assert( uDstFullWidth >= uDstStartX + uDstWidth );
	Assert( uDstFullHeight >= uDstStartY + uDstHeight );
	if(eSrcCSpace == YC420)
	{
		Assert( !(uSrcWidth%8) );
		Assert( !((uSrcFullWidth-uSrcWidth)%8) );
		//Assert( !(uSrcHeight%4) );	//rb1004 delete
	}
	else if((eSrcCSpace == RGB24) || (eSrcCSpace == ARGB24) )
	{
		//Nothing
	}	
	else
	{
		Assert( !(uSrcWidth%2) );
		Assert( !((uSrcFullWidth-uSrcWidth)%2) );
		//Assert( !(uSrcHeight%2) );	//rb1004 delete
	}		
	
//	m_uBaseAddr = POST_BASE; //+joon : POST class constructor has not been called, so base address has not been set. So, joon put here

	sCh->m_uModeRegValue = 0;
	sCh->m_eSrcCSpace = eSrcCSpace;
	sCh->m_eDstCSpace = eDstCSpace;

	PostOutp32( POST_START_VIDEO, POST_DISABLE );
	
	//uPostModeCtrl2 = ADDR_CHANGE_ENABLE |CHANGE_AT_FRAME_END |SOFTWARE_TRIGGER;
	uPostModeCtrl2 = ADDR_CHANGE_DISABLE |CHANGE_AT_FRAME_END |SOFTWARE_TRIGGER;
	PostOutp32( POST_MODE_CTRL_2, uPostModeCtrl2 );
	
	//--->SetClock ... rb1004
	//POST_SetClock(eOutPath, sCh);
	if(sCh->m_uBaseAddr == POST0_BASE)
	{
		POST_SetClkSrc(ePost_ClkSrc, sCh);
	}
	else
	{
		POST_SetClkSrc(eScaler_ClkSrc, sCh);
	}
	//--->SetDataPath
	POST_SetDataPath(eInPath, eOutPath, sCh);

	//--->SetDataFormat
	POST_SetDataFormat(eSrcCSpace, eDstCSpace, eInPath, eOutPath, sCh, &uPixelSzIn, &uPixelSzOut);

	//--->SetScaler
	POST_SetScaler(uSrcWidth, uSrcHeight, uDstWidth, uDstHeight, sCh);

	//--->SetDMA
	POST_SetDMA(	uSrcFullWidth, uSrcFullHeight, uSrcStartX, uSrcStartY, uSrcWidth, uSrcHeight, uSrcFrmSt, eSrcCSpace,
					uDstFullWidth, uDstFullHeight, uDstStartX, uDstStartY, uDstWidth, uDstHeight, uDstFrmSt, eDstCSpace,
					uSrcFrmBufNum,  eInPath, eOutPath, sCh,
					uPixelSzIn, uPixelSzOut);

	//-->SetFreeRun
	POST_SetAutoLoadEnable(eMode, sCh);

	//POST_SetCLKVALUpdate(POST_CLKVAL_StOfFRAME, sCh);
	
	//-->Enable Interrupt
	//POST_EnableInterrupt(POST_LEVEL_INT, sCh);

 }



//////////
// Function Name : POST_GetSrcStAddr
// Function Description : Get the Start Address
// Input : 	uSrcFrmBufIdx - Source Frame Buffer Index	
//			sCh - POST structure base address
// Output : 	uStAddr - Source Frame Buffer Address	
void POST_GetSrcStAddr(u32 uSrcFrmBufIdx, u32 *uStAddr, POST *sCh)
{
	Assert(uSrcFrmBufIdx <= sCh->m_uLastFrmBufIdx);

	*uStAddr = sCh->m_uSrcFrmStAddr[uSrcFrmBufIdx];
}


//////////
// Function Name : POST_SetCLKVALUpdate
// Function Description : Select CLKVAL_F update timing control
// Input :	uUpdate - 	POST_CLKVAL_ALWAYS 	: always,   
//						POST_CLKVAL_StOfFRAME 	: Start of a frame
//			sCh - POST structure base address
// Output : 	None
void POST_SetCLKVALUpdate(u32 uUpdate, POST *sCh)
{
	u32 uModeCtrlVal;
	PostInp32(POST_MODE_CTRL, uModeCtrlVal);
	uModeCtrlVal = (uModeCtrlVal & ~(1<<30)) | (uUpdate<<30);

	PostOutp32(POST_MODE_CTRL, uModeCtrlVal);
}



//////////
// Function Name : POST_IsProcessingDone
// Function Description : Check if the processing is done
// Input : 	sCh - POST structure base address
// Output : 	true - processing is done
//			false - processing is running
u8 POST_IsProcessingDone(POST *sCh)
{
	u32 uModeCtrlVal;
	PostInp32(POST_START_VIDEO, uModeCtrlVal);
	return ((uModeCtrlVal&(POST_START))    ? false : true);
}


//////////
// Function Name : POST_ClearPending
// Function Description : Interrupt Pending Bit clear
// Input : 	sCh - POST structure base address
// Output : 	None
void POST_ClearPending(POST *sCh)
{
	//rb1004
	PostInp32(POST_MODE_CTRL, sCh->m_uModeRegValue);
	
	PostOutp32( POST_MODE_CTRL, sCh->m_uModeRegValue & ~POST_PENDING );
}


//////////
// Function Name : POST_IsPendingEnabled
// Function Description : Check whether the Pending bit is set or not
// Input : 	sCh - POST structure base address
// Output : 	true - Pending bit is set
//			false - Pending bit is clear
u8 POST_IsPendingEnabled(POST *sCh)
{
	u32 uModeCtrl;

	PostInp32( POST_MODE_CTRL, uModeCtrl );

	if ( uModeCtrl & (POST_PENDING) )
		return true;
	else
		return false;
}

//////////
// Function Name : POST_SetNextFrameStAddr
// Function Description : Set the Next Frame DMA Start/End Address Register
// Input : 	uNxtSrcFrmSt - The Start Address of the next source frame
//			uNxtDstFrmSt - The Start Address of the next destination frame
//			sCh - POST structure base address
// Output : 	None
void POST_SetNextFrameStAddr(u32 uNxtSrcFrmSt, u32 uNxtDstFrmSt, POST *sCh) // DMA path
{
 	u32 uNxtSrcStY, uNxtSrcEndY;
	u32 uNxtSrcStCb, uNxtSrcEndCb, uNxtSrcStCr, uNxtSrcEndCr;
	u32 uNxtDstStRgb, uNxtDstEndRgb;
	u32 uOutNxtDstStCb, uOutNxtDstEndCb, uOutNxtDstStCr, uOutNxtDstEndCr;
	u32 uModeCtrl;

	uNxtSrcStY = uNxtSrcFrmSt + sCh->m_uStPosY;	//sCh->m_uStPosY = (uSrcFullWidth*uSrcStartY+uSrcStartX)*uPixelSzIn;
	uNxtSrcEndY = uNxtSrcStY + sCh->m_uEndPosY;	//sCh->m_uEndPosY = uSrcWidth*uSrcHeight*uPixelSzIn + uOffsetY*(uSrcHeight-1);
	
	if ( sCh->m_eSrcCSpace == YC420 )
	{
		uNxtSrcStCb = uNxtSrcFrmSt + sCh->m_uStPosCb;
		uNxtSrcEndCb = uNxtSrcStCb + sCh->m_uEndPosCb;
		uNxtSrcStCr = uNxtSrcFrmSt + sCh->m_uStPosCr;
		uNxtSrcEndCr = uNxtSrcStCr + sCh->m_uEndPosCr;
	}
	
	uNxtDstStRgb = uNxtDstFrmSt + sCh->m_uStPosRgb;
	uNxtDstEndRgb = uNxtDstStRgb + sCh->m_uEndPosRgb;
	
	if ( sCh->m_eDstCSpace  == YC420 )
	{
		uOutNxtDstStCb = uNxtDstFrmSt + sCh->m_uOutStPosCb;
		uOutNxtDstEndCb = uOutNxtDstStCb + sCh->m_uOutStPosCb;
		uOutNxtDstStCr = uNxtDstFrmSt + sCh->m_uOutStPosCr;
		uOutNxtDstEndCr = uOutNxtDstStCr + sCh->m_uOutStPosCr;
	}
	
	uModeCtrl=0;
	PostInp32(POST_MODE_CTRL_2, uModeCtrl);
	uModeCtrl |= (0x1<<4);
	PostOutp32(POST_MODE_CTRL_2, uModeCtrl); 	
	
	PostOutp32(NEXT_ADDR_START_Y, uNxtSrcStY);
	PostOutp32(NEXT_ADDR_END_Y, uNxtSrcEndY);
	if ( sCh->m_eSrcCSpace == YC420 )
	{
		PostOutp32(NEXT_ADDR_START_CB, uNxtSrcStCb);
		PostOutp32(NEXT_ADDR_END_CB, uNxtSrcEndCb);
		PostOutp32(NEXT_ADDR_START_CR, uNxtSrcStCr);
		PostOutp32(NEXT_ADDR_END_CR, uNxtSrcEndCr);
	}

	PostOutp32(NEXT_ADDR_START_RGB, uNxtDstStRgb);
	PostOutp32(NEXT_ADDR_END_RGB, uNxtDstEndRgb);
	if ( sCh->m_eDstCSpace  == YC420 )
	{
		PostOutp32(NEXT_ADDR_START_OUT_CB, uOutNxtDstStCb);
		PostOutp32(NEXT_ADDR_END_OUT_CB, uOutNxtDstEndCb);
		PostOutp32(NEXT_ADDR_START_OUT_CR, uOutNxtDstStCr);
		PostOutp32(NEXT_ADDR_END_OUT_CR, uOutNxtDstEndCr);
	}

	uModeCtrl &= ~(0x1<<4);
	PostOutp32(POST_MODE_CTRL_2, uModeCtrl);
}



void POST_SetFifoOutPath(SCALER_PATH ePath, POST *sCh)
{
	u32 uPostModeCtrl2;
	if(sCh->m_uBaseAddr == TVSCALER_BASE)
	{
		PostInp32( POST_MODE_CTRL_2, uPostModeCtrl2 );
		
		uPostModeCtrl2 = (uPostModeCtrl2&~(3<<5)) | (ePath<<5);
		PostOutp32( POST_MODE_CTRL_2, uPostModeCtrl2 );
	}
}

//////////
// Function Name : POST_DisableChangeNextFrame
// Function Description : Disable Address change(Next Address Change)
// Input : 	sCh - POST structure base address
// Output : 	None
void POST_DisableChangeNextFrame(POST *sCh)
{
	u32 uModeCtrl;
	
	PostInp32(POST_MODE_CTRL_2, uModeCtrl);
	uModeCtrl |= (0x1<<4);
	PostOutp32(POST_MODE_CTRL_2, uModeCtrl);
}


//////////
// Function Name : POST_StopProcessingOfFreeRun
// Function Description : Stop the Post Free-Run Mode
// Input : 	sCh - POST structure base address
// Output : 	None
void POST_StopProcessingOfFreeRun(POST *sCh)
{
	PostInp32(POST_MODE_CTRL, sCh->m_uModeRegValue);
	sCh->m_uModeRegValue &= ~(1<<14);
	
	PostOutp32(POST_MODE_CTRL, sCh->m_uModeRegValue);
}



//////////
// Function Name : POST_CheckAutoLoadEnable
// Function Description : Check the Post Auto-Load Enable bit
// Input : 	sCh - POST structure base address
// Output : 	None
u8 POST_CheckAutoLoadEnable(POST *sCh)
{
	PostInp32(POST_MODE_CTRL, sCh->m_uModeRegValue);
	if(sCh->m_uModeRegValue & (1<<14))
		return TRUE;
	else
		return FALSE;
}


//////////
// Function Name : POST_SetInterlaceModeOnOff
// Function Description : Set the output scal method selection register only when FIFO mode
// Input : 	onoff - 1 : Interace scan
//				    0 : Progressive scan
//			sCh - POST structure base address
// Output : 	None
void POST_SetInterlaceModeOnOff(int onoff, POST *sCh)
{
	Assert(onoff == 0 || onoff == 1);

	PostInp32(POST_MODE_CTRL, sCh->m_uModeRegValue);

	if (onoff == 1)
		sCh->m_uModeRegValue |= (1<<12);
	else
		sCh->m_uModeRegValue &= ~(1<<12);

	PostOutp32(POST_MODE_CTRL, sCh->m_uModeRegValue);
}


//////////
// Function Name : POST_SetInFifoMode
// Function Description : Set POST input path to eInPostPath
// Input : 	eInPostPath - Input path
//			sCh - POST structure base address
// Output : 	None
void POST_SetInFifoMode(POST_PATH eInPostPath, POST *sCh)
{
	PostInp32(POST_MODE_CTRL, sCh->m_uModeRegValue);
	
	if (eInPostPath == POST_FIFO)
		sCh->m_uModeRegValue |= (1U<<31);
	else
		sCh->m_uModeRegValue &= ~(1U<<31);

	PostOutp32(POST_MODE_CTRL, sCh->m_uModeRegValue);
}

//////////
// Function Name : POST_IsFreeRunDone
// Function Description : Check if the processing is done
// Input : 	sCh - POST structure base address
// Output : 	true - processing is done
//			false - processing is running
u8 POST_IsFreeRunDone(POST *sCh)
{
	u32 uTmp;

	PostInp32(POST_START_VIDEO, uTmp);
	return (((uTmp & (POST_START)) == 0x0) ? true : false);

}


//////////
// Function Name : POST_SetClkSrc
// Function Description : Set the clock for processing
// Input : 	eClkSrc - Video clock source
//			sCh - POST structure base address
// Output : 	None
void POST_SetClkSrc(POST_CLK_SRC eClkSrc, POST *sCh)
{
	u32 uPostPortSelect;
	
	PostInp32(POST_MODE_CTRL, sCh->m_uModeRegValue);

	if(sCh->m_uBaseAddr == POST0_BASE)
		uPostPortSelect = POST_PORT;
	else
		uPostPortSelect = SCALER_PORT;
	
	if((eClkSrc == HCLK))
	{
		if(g_HCLK>66000000)
		{
			sCh->m_uModeRegValue &= ~(0x7F<<23);
			sCh->m_uModeRegValue |= (Post_ClkDivider<<24);		// CLKVAL_F, Clock Source/2
			sCh->m_uModeRegValue |= (1<<23);		// Divided by CLKVAL_F
		}
		else
		{
			sCh->m_uModeRegValue &= ~(0x7F<<23);
		}
		sCh->m_uModeRegValue = (sCh->m_uModeRegValue & ~(0x3<<21)) | (0<<21);
	}
	else if(eClkSrc == PLL_EXT_MOUTEPLL)
	{
		SYSC_SetPLL(eEPLL, 32, 1, 2, 0);		// EPLL => 96MHz
		if(uPostPortSelect == POST_PORT)
			SYSC_ClkSrc(eLCD_MOUTEPLL);
		else
			SYSC_ClkSrc(eSCALER_MOUTEPLL);
		SYSC_ClkSrc(eEPLL_FOUT);

		sCh->m_uModeRegValue &= ~(0x7F<<23);
		sCh->m_uModeRegValue |= (Post_ClkDivider<<24);		// CLKVAL_F, Clock Source/2
		sCh->m_uModeRegValue |= (1<<23);		// Divided by CLKVAL_F	

		sCh->m_uModeRegValue = (sCh->m_uModeRegValue & ~(0x3<<21)) | (1<<21);
	}
	else if(eClkSrc == PLL_EXT_FINEPLL)
	{
		if(uPostPortSelect == POST_PORT)
			SYSC_ClkSrc(eLCD_FINEPLL);
		else
			SYSC_ClkSrc(eSCALER_FINEPLL);
		//SYSC_ClkSrc(eEPLL_FIN);

		sCh->m_uModeRegValue &= ~(0x7F<<23);	//for clear CLKVAL_F
		sCh->m_uModeRegValue = (sCh->m_uModeRegValue & ~(0x3<<21)) | (1<<21);
	}
	else if(eClkSrc == PLL_EXT_DOUTMPLL)
	{
		if(uPostPortSelect == POST_PORT)
			SYSC_ClkSrc(eLCD_DOUTMPLL);
		else
			SYSC_ClkSrc(eSCALER_DOUTMPLL);
		SYSC_ClkSrc(eMPLL_FOUT);

		sCh->m_uModeRegValue &= ~(0x7F<<23);
		sCh->m_uModeRegValue |= (Post_ClkDivider<<24);		// CLKVAL_F, Clock Source/2
		sCh->m_uModeRegValue |= (1<<23);		// Divided by CLKVAL_F		

		sCh->m_uModeRegValue = (sCh->m_uModeRegValue & ~(0x3<<21)) | (1<<21);
	}
	else		//27MHz Ext Clock Input
	{
		sCh->m_uModeRegValue &= ~(0x7F<<23);
		sCh->m_uModeRegValue = (sCh->m_uModeRegValue & ~(0x3<<21)) | (3<<21);
	}
	
	PostOutp32(POST_MODE_CTRL, sCh->m_uModeRegValue);

	if(uPostPortSelect == POST_PORT)
		ePost_ClkSrc = eClkSrc;
	else
		eScaler_ClkSrc = eClkSrc;
}


//////////
// Function Name : POST_SetClockDivide
// Function Description : Set the clock divider for processing
// Input : 	uDivider - Divide value
//			sCh - POST structure base address
// Output : 	None
void POST_SetClockDivide(u32 uDivider, POST *sCh)
{
	Assert(uDivider >= 1);

	Post_ClkDivider = uDivider&0x3F;
	PostInp32(POST_MODE_CTRL, sCh->m_uModeRegValue);
	sCh->m_uModeRegValue = (sCh->m_uModeRegValue & ~(0x3F<<24)) | (Post_ClkDivider<<24) | (1<<23);   
	PostOutp32(POST_MODE_CTRL, sCh->m_uModeRegValue);
	
}


//////////
// Function Name : POST_GetClockDivide
// Function Description : Set the clock divider for processing
// Input : 	uDivider - Divide value
//			sCh - POST structure base address
// Output : 	None
u32  POST_GetClockDivide(POST *sCh)
{
	return Post_ClkDivider;
}


//////////
// Function Name : POST_CalcurateScaleOffset
// Function Description : Calcurate the Scale Ratio to satisfy the word boundary constraints
// Input : 	uSrcWidth - Source Image Width
//			uSrcHeight - Source Image Height
//			uDstWidth - Source Image Width
//			uDstHeight - Source Image Height
//			sCh - POST structure base address
// Output :	uHOffset - Horizontal Offset Value
//			uVOffset - Vetical Offset Value
//			uHRatio - Horizontal Ratio Value
//			uVRatio - Vetical Ratio Value
void POST_CalcurateScaleOffset(	u32 uSrcWidth, u32 uSrcHeight, u32 uDstWidth, u32 uDstHeight, POST *sCh, 
									u32 *uHOffset, u32 *uVOffset, u32 *uHRatio, u32 *uVRatio, CSPACE eSrcCSpace)
{
	u32 uPreHratio, uPreVratio;
	
	Assert( !(uSrcWidth >= (uDstWidth<<6)) );
	Assert( !(uSrcHeight >= (uDstHeight<<6)) );
	
	if ( uSrcWidth >= (uDstWidth<<5) )
		uPreHratio = 32;
	else if ( uSrcWidth >= (uDstWidth<<4) )
		uPreHratio = 16;
	else if ( uSrcWidth >= (uDstWidth<<3) )
		uPreHratio = 8;
	else if ( uSrcWidth >= (uDstWidth<<2) )
		uPreHratio = 4;
	else if ( uSrcWidth >= (uDstWidth<<1) )
		uPreHratio = 2;
	else
		uPreHratio = 1;

	if ( uSrcHeight >= (uDstHeight<<5) )
		uPreVratio = 32;
	else if ( uSrcHeight >= (uDstHeight<<4) )
		uPreVratio = 16;
	else if ( uSrcHeight >= (uDstHeight<<3) )
		uPreVratio = 8;
	else if ( uSrcHeight >= (uDstHeight<<2) )
		uPreVratio = 4;
	else if ( uSrcHeight >= (uDstHeight<<1) )
		uPreVratio = 2;
	else
		uPreVratio = 1;

	if(eSrcCSpace == YC420)
	{
		*uHOffset = 4*uPreHratio;
		if(*uHOffset < 8)
			*uHOffset = 8;
	}
	else
		*uHOffset = 4*uPreHratio;
	*uVOffset = 2*uPreVratio;
	*uHRatio = uPreHratio;
	*uVRatio = uPreVratio;
}


void POST_SetScalerforTVout(u32 uSrcImgSizeX, u32 uSrcImgSizeY, u32 uSrcFrameAddr, CSPACE eSrcCSpace, 
								u32 uDstImgSizeX, u32 uDstImgSizeY, POST *sCh)
{
	u32 uWidth, uHeight;

	uWidth = uDstImgSizeX;
	uHeight = uDstImgSizeY;
	
	POST_InitCh(POST_B, sCh);

	POST_InitIp1(	uSrcImgSizeX, uSrcImgSizeY, 0, 0, uSrcImgSizeX, uSrcImgSizeY, uSrcFrameAddr, eSrcCSpace,
					uWidth*2, uHeight, 0, 0, uWidth*2, uHeight, 0, YCBYCR,
					1, FALSE, FREE_RUN, POST_DMA, POST_FIFO, sCh);
	POST_SetInterlaceModeOnOff(1, sCh);
	POST_SetNextFrameStAddr(uSrcFrameAddr, 0, sCh);
	POST_StartProcessing(sCh);
}

