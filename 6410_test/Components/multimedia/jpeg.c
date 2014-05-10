/*----------------------------------------------------------------------
 *
 * Filename: jpeg.c
 *
 * Contents:
 *
 * History: 

 	1. 070817
 	    - GetJpegType1 fucntion edited
     	    - Deleted no used  fucntion
 
 * Contributors:
 *
 * Copyright (c) 2003 SAMSUNG Electronics.
 *
 *----------------------------------------------------------------------
 */

#include <string.h>
#include <stdio.h>
#include "system.h"
#include "jpeg.h"
#include "jpeg_tables.h"

#include "option.h"
#include "library.h"
#include "sfr6410.h"

enum JPEG_REG
{
	JPGMOD       = (JPEG_BASE+0x000),
	JPGSTS        = (JPEG_BASE+0x004),
	JPGQHNO     = (JPEG_BASE+0x008),
	JPGDRI        = (JPEG_BASE+0x00C),
	
	JPGY          = (JPEG_BASE+0x010),
	JPGX          = (JPEG_BASE+0x014),
	JPGCNT      = (JPEG_BASE+0x018),
	
	JPGIRQEN    = (JPEG_BASE+0x01C),
	JPGIRQ        = (JPEG_BASE+0x020), // Int Pending Register
	
	JQTBL0        = (JPEG_BASE+0x400),
	JQTBL1        = (JPEG_BASE+0x500),
	JQTBL2        = (JPEG_BASE+0x600),
	JQTBL3        = (JPEG_BASE+0x700),
	
	JHDCTBL0      = (JPEG_BASE+0x800),
	JHDCTBLG0    = (JPEG_BASE+0x840),
	JHACTBL0      = (JPEG_BASE+0x880),
	JHACTBLG0    = (JPEG_BASE+0x8c0),
	JHDCTBL1      = (JPEG_BASE+0xc00),
	JHDCTBLG1    = (JPEG_BASE+0xc40),
	JHACTBL1      = (JPEG_BASE+0xc80),
	JHACTBLG1    = (JPEG_BASE+0xcc0),
	
	JIMGADDR0     = (JPEG_BASE+0x1000),
	JIMGADDR1     = (JPEG_BASE+0x1004),
	JHUFADDR0     = (JPEG_BASE+0x1008),
	JHUFADDR1     = (JPEG_BASE+0x100c),
	
	JSTART        = (JPEG_BASE+0x1010),
	JRSTART      = (JPEG_BASE+0x1014),
	RESET_CON = (JPEG_BASE+0x1018),
	JPG_CON     = (JPEG_BASE+0x101c),
	
	JCOEF1        = (JPEG_BASE+0x1020),
	JCOEF2        = (JPEG_BASE+0x1024),
	JCOEF3        = (JPEG_BASE+0x1028),
	
	JMISC       	    = (JPEG_BASE+0x102c),
	JFRAME_INTV   = (JPEG_BASE+0x1030),
	RESUME        	    = (JPEG_BASE+0x1038),
	
	IMGADDR_END   = (JPEG_BASE+0x103c),	
	HUFADDR_END   = (JPEG_BASE+0x1040),	
	HUFADDR_MNT   = (JPEG_BASE+0x1044)
};

// Coefficients for RGB to YUV
#define COEF1_RGB_2_YUV         0x4d971e
#define COEF2_RGB_2_YUV         0x2c5783
#define COEF3_RGB_2_YUV         0x836e13

#define ENABLE_MOTION_ENC       (0x1<<3)
#define DISABLE_MOTION_ENC      (0x0<<3)

#define ENABLE_MOTION_DEC       (0x1<<0)
#define DISABLE_MOTION_DEC      (0x0<<0)

#define ENABLE_HW_DEC           (0x1<<2)
#define DISABLE_HW_DEC          (0x0<<2)

#define INCREMENTAL_DEC	(0x1<<3)
#define NORMAL_DEC                (0x0<<3)


JPEG oJpg;

void JPEG_Init(void)
{
	oJpg.m_bIsHeaderParsed = false;
}

// It is necessary for decoding
void JPEG_Reset(void)
{


	Outp32(RESET_CON, 0); //0: enable, 1: disable(default value)
	
}

void JPEG_Wait_Done(void)
{
	u32 temp;

	do{
		temp = Inp32(JPGSTS);
		//UART_Printf("jpgsts = 0x%x\n",temp);
	}while(temp);
	
}

void JPEG_GetJpegType(char* pType)
{
	u32 uSampleMode;
	uSampleMode = (Inp32(JPGMOD) & 0x07);

	if (uSampleMode == 0)
		strcpy(pType, "JPEG 444");
	else if (uSampleMode == 1)
		strcpy(pType, "JPEG 422");
	else if (uSampleMode == 2)
		strcpy(pType, "JPEG 420");
	else if (uSampleMode == 3)
		strcpy(pType, "JPEG 400 (Gray)");
	else if (uSampleMode == 6)
		strcpy(pType, "JPEG 411");
	else
		Assert(0);
}

void JPEG_GetStreamLen(u32* uSize)
{
	*uSize = Inp32(JPGCNT);
}

void JPEG_GetWidthAndHeight(u32* hsz, u32* vsz)
{
	*hsz = Inp32(JPGX);
	*vsz = Inp32(JPGY);
}

void JPEG_ReadAndClearStatus(JPEG_STATUS* eStatus, JPEG_IRQSTATUS* eIrqStatus)
{
	u32 uIntEn;
	u32 uStatus;
	u32 uJpgStsReg;
	
	uJpgStsReg = Inp32(JPGSTS);
//	UART_Printf("1: JPGSTS = 0x%x\n",uJpgStsReg);

	*eStatus = uJpgStsReg;

	uStatus = Inp32(JPGIRQ);
		
	uStatus &= ((1<<6)|(1<<4)|(1<<3));

	*eIrqStatus = uStatus;




 }

// Set Next frame start addr of Jpeg data for both encoding and decoding
void JPEG_SetNextFrameStartAddr(u32 addr)
{
	Outp32(JHUFADDR1, addr);
}

void JPEG_InitRegsForEncoding(
	u32 uRawHsz, u32 uRawVsz, u32 uRawAddr, CSPACE eRawType,
	u32 uJpgAddr, JPEG_TYPE uJpgType, u8 bIsOnTheFly, u8 bIsMotion)
{
	int i;

	Assert(eRawType == YCBYCR || eRawType == RGB16);

	Outp32(JPGMOD, (uJpgType == JPEG_422)? (0x1<<0) : (0x2<<0)); // Encoded to yuv422 or yuv420
	Outp32(JPGDRI, 2); // MCU inserts RST marker
	Outp32(JPGQHNO, 0x0);
	Outp32(JPGX, uRawHsz);
	Outp32(JPGY, uRawVsz);
	Outp32(JIMGADDR0, uRawAddr); // Address of input image
	Outp32(JHUFADDR0, uJpgAddr); // Address of JPEG stream
	Outp32(JIMGADDR1, uRawAddr); // Address of input image
	Outp32(JHUFADDR1, uJpgAddr); // next address of motion JPEG stream
	Outp32(JCOEF1, COEF1_RGB_2_YUV); // Coefficient value 1 for RGB to YCbCr
	Outp32(JCOEF2, COEF2_RGB_2_YUV); // Coefficient value 2 for RGB to YCbCr
	Outp32(JCOEF3, COEF3_RGB_2_YUV); // Coefficient value 3 for RGB to YCbCr

	Outp32(JMISC,
		(bIsOnTheFly ? 0 : (eRawType == YCBYCR ? 1 : 2))<<5 |
		(bIsOnTheFly ? 1 : 0)<<2
		);

	Outp32(JPG_CON, 	(bIsMotion ? ENABLE_MOTION_ENC : DISABLE_MOTION_ENC));

//	Outp32(JPGIRQEN, 0x10);
//	Outp32(JPGIRQEN, 1<<4); // Deleted @2006.6.8

	// Quantiazation and Huffman Table setting
	//-----------------------------------------

	for (i=0; i<64; i++)
		Outp32((JQTBL0+i*4), (u32)QTBL0[i]);

	for (i=0; i<64; i++)
		Outp32((JQTBL1+i*4), (u32)std_chrominance_quant_tbl_plus[i]);

	for (i=0; i<16; i++)
		Outp32((JHDCTBL0+i*4), (u32)HDCTBL0[i]);

	for (i=0; i<12; i++)
		Outp32((JHDCTBLG0+i*4), (u32)HDCTBLG0[i]);

	for (i=0; i<16; i++)
		Outp32((JHACTBL0+i*4), (u32)HACTBL0[i]);

	for (i=0; i<162; i++)
		Outp32((JHACTBLG0+i*4), (u32)HACTBLG0[i]);
}

void JPEG_InitRegsForDecoding(
	u32 uSrcAddr, u32 uDstAddr,
	JPEG_DEC_MODE eMode, u8 bIncremental, u8 bIsMotion
	)
{

	u32 uReg = 0;
	u32 uJpgConVal = 0;	
	u32 uMisc = 0;

	if (eMode == HEADER || eMode == HEADER_N_BODY)
	{
		Outp32(JPGMOD, 0x8); // Process mode: Decoding		
		Outp32(JHUFADDR0, uSrcAddr); // Address of compresed input data
		Outp32(JHUFADDR1, uSrcAddr); // Address of compresed input data

//		uReg = Inp32(JPGIRQEN);
//		uReg = uReg & ~(0xf);
//		uReg = uReg | (0x8);

//		uReg = 0x0;
//		Outp32(JPGIRQEN, uReg); // JPGIRQEN[6:3]=For several error conditions @2006.6.8	

		Outp32(JPGIRQEN, 0x1<<3); // JPGIRQEN[6:3]=For several error conditions @2006.6.8	
		//Outp32(JPGIRQEN, 0xf); // JPGIRQEN[6:3]=For several error conditions @2006.6.8	

		if (eMode == HEADER_N_BODY)
		{
			Outp32(JIMGADDR0, uDstAddr); // Address of decompresed image
			Outp32(JIMGADDR1, uDstAddr); // Address of decompresed image
		}


		uJpgConVal = (eMode == HEADER) ? DISABLE_HW_DEC : ENABLE_HW_DEC;
	}
	else // eMode == BODY
	{
		Outp32(JIMGADDR0, uDstAddr); // Address of decompresed image
		Outp32(JIMGADDR1, uDstAddr); // Address of decompresed image
	}


	if (eMode == BODY || eMode == HEADER_N_BODY)
	{
		uJpgConVal |= (bIsMotion == true) ? ENABLE_MOTION_DEC : DISABLE_MOTION_DEC;
		uMisc = (bIncremental == true) ? INCREMENTAL_DEC : NORMAL_DEC;
	}

	Outp32(JPG_CON, uJpgConVal);
	Outp32(JMISC, uMisc);

//	Inp32(JPGIRQ);
//	Inp32(JPGSTS);


}



void JPEG_StartParsingHeader(u32 uJpgAddr)
{
	Assert(uJpgAddr%16 == 0);	
	//UART_Printf("Dec: jpeg=%08x\n", uJpgAddr);

	JPEG_Reset();

	JPEG_InitRegsForDecoding(uJpgAddr, 0, HEADER, false, false);

	Outp32(JSTART, 1);
	
	oJpg.m_bIsHeaderParsed = true;
}

void JPEG_StartDecodingBody(u32 uRawAddr, u8 bIsIncremental)
{
	Assert(oJpg.m_bIsHeaderParsed == true);

	JPEG_InitRegsForDecoding(0, uRawAddr, BODY, bIsIncremental, false);
	Outp32(JRSTART, 1);
}

void JPEG_StartDecodingOneFrame(u32 uJpgAddr, u32 uRawAddr, u8 bIsIncremental)
{
	
	Assert(uJpgAddr%16 == 0);
	
	//UART_Printf(" Dec: jpeg=%08x, raw=%08x\n", uJpgAddr, uRawAddr);

	JPEG_Reset();

	JPEG_InitRegsForDecoding(uJpgAddr, uRawAddr, HEADER_N_BODY, bIsIncremental, false);

	Outp32(JSTART, 1);
}

void JPEG_StartEncodingOneFrame(
	u16 usHSz, u16 usVSz, u32 uSrcAddr, CSPACE eRawType,
	u32 uDestAddr, JPEG_TYPE eJpgType)
{
	Assert(eRawType == YCBYCR || eRawType == RGB16);

	JPEG_Reset();
	JPEG_InitRegsForEncoding(usHSz, usVSz, uSrcAddr, eRawType, uDestAddr, eJpgType, false, false);

	Outp32(JSTART, 0); // start.
}

