/*******************************************************************************
 *
 *	ViP model for FIMGSE
 *		version 1.0
 *
 *	TexFmt2D.cpp
 *
 *	by cheolkyoo.kim
 *	Graphics IP Team in AP, Mobile Solution Development,
 *	System LSI Division, Semiconductor Business,
 *	Samsung Electronics
 *
 *	Copyright (c) 2005 FIMG team
 *
 *	All rights reserved. No part of this program may be reproduced, stored
 *	in a retrieval system, or tranmitted, in any form or by any means,
 *	electronic, mechanical, photocopying, recording, or otherwise, without
 *	the prior written permission of the author.
 *
 *	2006. 9. 20	by cheolkyoo.kim
 *
 *  Description
 *
 *	$RCSfile: $
 *	$Revision: $
 *	$Author: $
 *	$Date: $
 *	$Locker: $
 *
 *	$Source: $
 *	$State: $
 *	$Log: $
 *
 ******************************************************************************/
/****************************************************************************
 *  INCLUDES
 ****************************************************************************/
//#include "Config.h"

#include "Fimg3DTest.h"


#include "TexFmt2D.vsa.h"
#include "TexFmt2D.psa.h"
#include "PaletteTable.h"

/****************************************************************************
 *  DEFINES
 ****************************************************************************/
typedef struct FGL_32BitColorTag {
	unsigned char B;
	unsigned char G;
	unsigned char R;
	unsigned char A;
} FGL_32BitColor;


int TexFmt2D(FGL_TexelFormat format, unsigned int offset)
{
	unsigned int uDataSize;
	FGL_TexUnitParams tuParams;
	unsigned int i;
	
	FGL_32BitColor uColor = { 0, 0, 0, 0 };
	
	float attribute[] =
	{
		// x     y      z      s      t
		-1.f,  1.f,  0.5f,  0.0f,  0.0f,
		 1.f,  1.f,  0.5f,  1.0f,  0.0f,
		-1.f, -1.f,  0.5f,  0.0f,  1.0f,
		 1.f, -1.f,  0.5f,  1.0f,  1.0f
	};


    if (fglLoadVShader(TexFmt2D_vsa) != FGL_ERR_NO_ERROR) {
		return ERROR;
    }

    if (fglLoadPShader(TexFmt2D_psa) != FGL_ERR_NO_ERROR) {
		return ERROR;
    }

	drvsys_clear_buf((unsigned int*)(FIMG_COLOR_BUFFER), CLEAR_SCREEN_SIZE, 0x0);
	fglFlush(FGL_PIPESTATE_ALL);	// Pipeline status

/*
	// Vertex Shader SFR set
	WRITEREG(FGVS_ATTRIB_NUM, 0x00020002); // Attribute counter of VS input & output
	WRITEREG(FGVS_CONFIG, 0x00000001); // Configuration register

	// Pixel Shader SFR set
	WRITEREG(FGPS_EXE_MODE, 0x0); // Pixel shader execution mode control register
	WRITEREG(FGPS_PC_COPY, 0x1); // Copy PSPCS_ADDR value to program counter
	WRITEREG(FGPS_ATTRIB_NUM, 0x1); // Number of attribute of current context
	fglSysPollingMask(FGPS_INBUF_STATUS, 0x0, 0x1); // Status signal of pixel shader input buffer
	WRITEREG(FGPS_EXE_MODE, 0x1); // Pixel shader execution mode control register
*/
    fglVSSetAttribNum(2);
    fglPSSetAttributeNum(1);

    // Texture format rendering test main
    for(unsigned int j=0; j < 4; j++)
    {
		// Primitive Engine SRF Set
		FGL_Vertex Vtx;
		Vtx.prim = FGL_PRIM_TRIANGLE_STRIP;
		Vtx.enablePointSize = FGL_FALSE;
		Vtx.numVSOut = 1;
		Vtx.shadeModel = FGL_SHADING_SMOOTH;
		Vtx.colorAttribIdx = 0;
		
		fglSetVertex(&Vtx); // Vertex context register
		// Raster SFR set
		//fglSetLODControl(0x0000007) );
		fglSetLODRegister(FGL_LODCOEFF_ENABLE_ALL,
					FGL_LODCOEFF_DISABLE,
					FGL_LODCOEFF_DISABLE,
					FGL_LODCOEFF_DISABLE,
					FGL_LODCOEFF_DISABLE,
					FGL_LODCOEFF_DISABLE,
					FGL_LODCOEFF_DISABLE,
					FGL_LODCOEFF_DISABLE);



		// Texture Unit SFR Setting
		tuParams.eType = FGL_TEX_2D;
		tuParams.eColorkey = FGL_CKEY_DISABLE;
		tuParams.bUseExpansion = FGL_FALSE;
		//tuParams.ePaletteFormat = FGL_PALETTE_ARGB8888;
		tuParams.ePaletteFormat = (FGL_PaletteFormat)j;
		tuParams.eFormat = format;
		tuParams.eUMode = FGL_TEX_WRAP_REPEAT;
		tuParams.eVMode = FGL_TEX_WRAP_REPEAT;
		tuParams.bIsNonparametric = FGL_FALSE;
		tuParams.bUseMagFilter = FGL_FALSE;
		tuParams.bUseMinFilter = FGL_FALSE;
		tuParams.eMipMapFilter = FGL_FILTER_DISABLE;
		tuParams.uUSize = 256;
		tuParams.uVSize = 256;
		tuParams.uPSize = 1;

	    // Load pallette table for bpp
        switch((FGL_TexelFormat)format)
        {
        case FGL_TEXEL_1BPP:
    		for(i=0; i<2; i++) 
    		{
       	    	uColor	= *((FGL_32BitColor*)&PaletteTable_2[i]);
 
            	//WRITEREG(FGTU_PALETTE_ADDR, i);
            	fglSetTexPaletteAddr(i);
            	
            	switch(tuParams.ePaletteFormat)
            	{
            	case FGL_PALETTE_ARGB1555:
            		fglSetTexPaletteEntry( (((uColor.A)?1:0)<<15) |
            									   ((uColor.R & 0xF8)<<7) |
            									   ((uColor.G & 0xF8)<<2) |
            									   ((uColor.B & 0xF8)>>3) );
           			break;
            	case FGL_PALETTE_RGB565:
            		fglSetTexPaletteEntry( ((uColor.R & 0xF8)<<8) |
            										((uColor.G & 0xFC)<<3) |
            										((uColor.B & 0xF8)>>3) );
            		break;
            	case FGL_PALETTE_ARGB4444:
            		fglSetTexPaletteEntry( (((unsigned int)uColor.A & 0xF0)<<8) |
            									   (((unsigned int)uColor.R & 0xF0)<<4) |
            									   (((unsigned int)uColor.G & 0xF0)   ) |
            									   (((unsigned int)uColor.B & 0xF0)>>4) );
           			break;
            	case FGL_PALETTE_ARGB8888:
        			fglSetTexPaletteEntry(PaletteTable_2[i]);
        			break;
            	}
    		}
            break;
        case FGL_TEXEL_2BPP:
    		for(i=0; i<4; i++) {
    			fglSetTexPaletteAddr(i); 		  // index
    			fglSetTexPaletteEntry(PaletteTable_4[i]); // paletted color
    		}
            break;
        case FGL_TEXEL_4BPP:
    		for(i=0; i<16; i++) {
    			fglSetTexPaletteAddr(i); 		  // index
    			fglSetTexPaletteEntry(PaletteTable_16[i]); // paletted color
    		}
            break;
        case FGL_TEXEL_8BPP:
    		for(i=0; i<256; i++) {
    			fglSetTexPaletteAddr(i); 		  // index
    			fglSetTexPaletteEntry(PaletteTable_256[i]); // paletted color
    		}
		    break;
       default:
            break;
        }            

		fglSetTexUnitParams(0, &tuParams);
		fglSetTexMipmapLevel(0, FGL_MIPMAP_MIN_LEVEL, 0x0);
		fglSetTexMipmapLevel(0, FGL_MIPMAP_MAX_LEVEL, 0x0);
		fglSetTexBaseAddr(0, (FIMG_TEXTURE_MEMORY+offset));
		
		// Host Interface SFR Set
		FGL_HInterface HInterface;
		HInterface.enableAutoInc = FGL_TRUE;
		HInterface.enableVtxBuffer = FGL_FALSE;
		HInterface.enableVtxCache = FGL_FALSE;	
		HInterface.idxType = FGL_INDEX_DATA_UINT;
		HInterface.numVSOut = 2;
		fglSetHInterface(&HInterface);
		
		FGL_Attribute HIAttr;
		HIAttr.bEndFlag = FGL_FALSE;
		HIAttr.type = FGL_ATTRIB_DATA_FLOAT;
		HIAttr.numComp = 3;
		HIAttr.srcW = FGL_ATTRIB_ORDER_4TH;
		HIAttr.srcZ = FGL_ATTRIB_ORDER_3RD;
		HIAttr.srcY = FGL_ATTRIB_ORDER_2ND;
		HIAttr.srcX = FGL_ATTRIB_ORDER_1ST;	
		fglSetAttribute(0, &HIAttr);
		HIAttr.bEndFlag = FGL_TRUE;
		HIAttr.numComp = 2;
		fglSetAttribute(1, &HIAttr);
		
		unsigned int uiTmpVertices = 4;		
		unsigned int uiDummy = 0xFFFFFFFF;
		fglSendToFIFO(4, &uiTmpVertices);
		fglSendToFIFO(4, &uiDummy);

		uDataSize = sizeof(attribute)/sizeof(attribute[0]);

 		fglSysTransferToPort(
								(unsigned int *)attribute,
								/*(volatile unsigned int *)(FGHI_FIFO_ENTRY),*/
								uDataSize
							 );

		fglFlush(FGL_PIPESTATE_ALL);	// Pipeline status

		// Cache flush
		//fglSysCacheFlush();
		//fglSysCacheClear();
		fglClearCache(FGL_CACHECTL_INIT_ALL);


		fglSysDelay(50);
    }

	fglSetLODControl(0x0);

    return NO_ERROR;
}

