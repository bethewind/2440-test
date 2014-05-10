/*******************************************************************************
 *
 *	ViP model for FIMGSE
 *		version 1.0
 *
 *	TexMipMapFilter.c
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
 *	2006. 1. 20	by cheolkyoo.kim
 *
 *  Description
 *
 *	$RCSfile: TexMipMapFilter.cpp,v $
 *	$Revision: 1.6 $
 *	$Author: cheolkyoo.kim $
 *	$Date: 2006/05/08 05:37:07 $
 *	$Locker:  $
 *
 *	$Source: C:/CVS/CVSrepository/FIMG-3DSE_SW/fimg3dse_fpga/fimg3d/src/TexMipMapFilter.cpp,v $
 *	$State: Exp $
 *	$Log: TexMipMapFilter.cpp,v $
 *	Revision 1.6  2006/05/08 05:37:07  cheolkyoo.kim
 *	no message
 *	
 *	Revision 1.5  2006/04/13 11:53:57  cheolkyoo.kim
 *	no message
 *
 *	Revision 1.4  2006/04/10 05:34:49  inhosens.lee
 *	updated with fimg_ra and fimg_tu codes
 *
 *	Revision 1.3  2006/04/05 04:06:11  cheolkyoo.kim
 *	Replaced ~_XCORD and ~_YCORD to ~_XCOORD and ~_YCOORD.
 *
 *	Revision 1.2  2006/03/30 04:27:45  cheolkyoo.kim
 *	Thomas-20060330@comment: Add to code using directive WIN32_VIP for VIP
 *
 *	Revision 1.1  2006/03/10 08:29:07  cheolkyoo.kim
 *	Initial import of FIMG-3DSE_SW package
 *
 *
 ******************************************************************************/
/****************************************************************************
 *  INCLUDES
 ****************************************************************************/
//#include "Debug.h"

#include "Fimg3DTest.h"


#include "tex_bypass_vsa.h"
#include "tex_bypass_psa.h"

/****************************************************************************
 *  DEFINES
 ****************************************************************************/

int TexMipMapFilter(void)

{
	unsigned int uDataSize;
	unsigned int j;
	FGL_TexUnitParams tuParams;

	float attribute0[] =
	{
		/* x       y      z      w      r      g      b      a 		s      t      r      q*/
		-1.0f,  0.3f, -0.5f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.4f,  0.3f, -0.5f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,
		-1.0f, -0.3f, -0.5f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,  1.0f,
		-0.4f, -0.3f, -0.5f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,  0.0f,  1.0f
	};

	float attribute1[] =
	{
		/* x       y      z      w      r      g      b      a 		s      t      r      q*/
		-0.3f,  0.3f, -0.5f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 0.3f,  0.3f, -0.5f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,
		-0.3f, -0.3f, -0.5f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,  1.0f,
		 0.3f, -0.3f, -0.5f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,  0.0f,  1.0f
	};

	float attribute2[] =
	{
		/* x       y      z      w      r      g      b      a 		s      t      r      q*/
		 0.4f,  0.3f, -0.5f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 1.0f,  0.3f, -0.5f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,
		 0.4f, -0.3f, -0.5f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,  1.0f,
		 1.0f, -0.3f, -0.5f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,  0.0f,  1.0f
	};

    if (fglLoadVShader(tex_bypass_vsa) != FGL_ERR_NO_ERROR) {
		return ERROR;
    }

    if (fglLoadPShader(tex_bypass_psa) != FGL_ERR_NO_ERROR) {
		return ERROR;
    }

	drvsys_clear_buf((unsigned int*)(FIMG_COLOR_BUFFER), CLEAR_SCREEN_SIZE, 0x0);


	fglFlush(FGL_PIPESTATE_ALL);	// Pipeline status


/*
	// Vertex Shader SFR set
	WRITEREG(FGVS_ATTRIB_NUM, 0x00030003); // Attribute counter of VS input & output
	WRITEREG(FGVS_CONFIG, 0x00000001); // Configuration register
	WRITEREG(FGVS_OUT_ATTRIB_IDX0, 0x02020100); // Output attribute index control

	// Pixel Shader SFR set
	WRITEREG(FGPS_EXE_MODE, 0x00000000); // Pixel shader execution mode control register
	WRITEREG(FGPS_PC_COPY, 0x00000001); // Copy PSPCS_ADDR value to program counter
	WRITEREG(FGPS_ATTRIB_NUM, 0x00000002); // Number of attribute of current context
	fglSysPollingMask(FGPS_INBUF_STATUS, 0x0, 0x00000001); // Status signal of pixel shader input buffer
	WRITEREG(FGPS_EXE_MODE, 0x00000001); // Pixel shader execution mode control register
*/
    fglSetVShaderAttribTable(FGL_OUTPUT_ATTRIB_IDX0, 0x02020100);
    fglVSSetAttribNum(3);
    fglPSSetAttributeNum(2);

    // Texture format rendering test main

	// Primitive Engine SRF Set
	FGL_Vertex Vtx;
	Vtx.prim = FGL_PRIM_TRIANGLE_STRIP;
	Vtx.enablePointSize = FGL_FALSE;
	Vtx.numVSOut = 2;
	Vtx.shadeModel = FGL_SHADING_SMOOTH;
	Vtx.colorAttribIdx = 0;
	
	fglSetVertex(&Vtx); // Vertex context register
	// Raster SFR set
	//FGL_DPFAPI( fglSetLODControl(0x00000038) );
	fglSetLODRegister(FGL_LODCOEFF_DISABLE,
					FGL_LODCOEFF_ENABLE_ALL,
					FGL_LODCOEFF_DISABLE,
					FGL_LODCOEFF_DISABLE,
					FGL_LODCOEFF_DISABLE,
					FGL_LODCOEFF_DISABLE,
					FGL_LODCOEFF_DISABLE,
					FGL_LODCOEFF_DISABLE);

//	WRITEREG(FGRA_LOD_CTRL, 0x00000038); // Level of detail attribute1 set

#ifdef _FIMG3DSE_VER_1_2
	// Texture Unit SFR Setting
	tuParams.eType = FGL_TEX_2D;
	tuParams.eColorkey = FGL_CKEY_DISABLE;
	tuParams.bUseExpansion = FGL_FALSE;
	tuParams.ePaletteFormat = FGL_PALETTE_ARGB8888;
	tuParams.eFormat = FGL_TEXEL_RGB565;
	tuParams.eUMode = FGL_TEX_WRAP_REPEAT;
	tuParams.eVMode = FGL_TEX_WRAP_REPEAT;
	tuParams.bIsNonparametric = FGL_FALSE;
	tuParams.bUseMagFilter = FGL_TRUE;
	tuParams.bUseMinFilter = FGL_TRUE;
	tuParams.eMipMapFilter = FGL_FILTER_NEAREST;
	tuParams.uUSize = 256;
	tuParams.uVSize = 256;
	tuParams.uPSize = 1;
	
	fglSetTexUnitParams(0, &tuParams);
	fglSetTexBaseAddr(0, FIMG_TEXTURE_MEMORY);
#else
	WRITEREG(FGTU_TEX0_CTRL, 0x08151088); 	// FIMG_TUSFR_TEX0_STATUS
	WRITEREG(FGTU_TEX0_BASE_LEVEL, 0x00000000); 	// FIMG_TUSFR_TEX0_BASE_LEVEL
	WRITEREG(FGTU_TEX0_MAX_LEVEL, 0x00000008); 	// FIMG_TUSFR_TEX0_MAX_LEVEL
	WRITEREG(FGTU_TEX0_BASE_ADDR, FIMG_TEXTURE_MEMORY); 	// FIMG_TUSFR_TEX0_BASE_ADDRESS
#endif
	// Host Interface SFR Set
	FGL_HInterface HInterface;
	HInterface.enableAutoInc = FGL_TRUE;
	HInterface.enableVtxBuffer = FGL_FALSE;
	HInterface.enableVtxCache = FGL_FALSE;	
	HInterface.idxType = FGL_INDEX_DATA_UINT;
	HInterface.numVSOut = 3;
	fglSetHInterface(&HInterface);
	
	FGL_Attribute HIAttr;
	HIAttr.bEndFlag = FGL_FALSE;
	HIAttr.type = FGL_ATTRIB_DATA_FLOAT;
	HIAttr.numComp = 4;
	HIAttr.srcW = FGL_ATTRIB_ORDER_4TH;
	HIAttr.srcZ = FGL_ATTRIB_ORDER_3RD;
	HIAttr.srcY = FGL_ATTRIB_ORDER_2ND;
	HIAttr.srcX = FGL_ATTRIB_ORDER_1ST;	
	fglSetAttribute(0, &HIAttr);
	fglSetAttribute(1, &HIAttr);
	HIAttr.bEndFlag = FGL_TRUE;
	fglSetAttribute(2, &HIAttr);

    for (j=0; j<2; j++)
    {
		//tuParams.bUseMapFilter = j;
		//tuParams.mipFilter = FGL_FILTER_DISABLE; // 0x0
		//FGL_DPFAPI( fglSetTexUnitParams(0, &tuParams) );
    	tuParams.bUseMagFilter = (FG_BOOL)j;
    	tuParams.bUseMinFilter = (FG_BOOL)j;
    	tuParams.eMipMapFilter = (FGL_MipMapFilter)0;
		fglSetTexUnitParams(0, &tuParams);

		//fglSysModifyReg(FGTU_TEX0_CTRL, 0x0000000F, (j<<TEXCTRL_MAG_FILTER_SHIFT) | 
		//                (j<<TEXCTRL_MIN_FILTER_SHIFT) | (0<<TEXCTRL_MIPMAP_FILTER_SHIFT));

		unsigned int uiTmpVertices = 4;		
		unsigned int uiDummy = 0xFFFFFFFF;
		fglSendToFIFO(4, &uiTmpVertices);
		fglSendToFIFO(4, &uiDummy);

		uDataSize = sizeof(attribute0)/sizeof(attribute0[0]);

 		fglSysTransferToPort(
								(unsigned int *)attribute0,
								/*(volatile unsigned int *)(FGHI_FIFO_ENTRY),*/
								uDataSize
							 );

		fglFlush(FGL_PIPESTATE_ALL);	// Pipeline status


		//tuParams.bUseMapFilter = j;
		//tuParams.mipFilter = FGL_FILTER_NEAREST; // 0x1
		//FGL_DPFAPI( fglSetTexUnitParams(0, &tuParams) );
    	tuParams.bUseMagFilter = (FG_BOOL)j;
    	tuParams.bUseMinFilter = (FG_BOOL)j;
    	tuParams.eMipMapFilter = (FGL_MipMapFilter)1;
		fglSetTexUnitParams(0, &tuParams);
/*
		fglSysModifyReg(FGTU_TEX0_CTRL, 0x0000000F, (j<<TEXCTRL_MAG_FILTER_SHIFT) | 
		                (j<<TEXCTRL_MIN_FILTER_SHIFT) | (1<<TEXCTRL_MIPMAP_FILTER_SHIFT));
*/		
		fglSendToFIFO(4, &uiTmpVertices);
		fglSendToFIFO(4, &uiDummy);

		uDataSize = sizeof(attribute1)/sizeof(attribute1[0]);

 		fglSysTransferToPort(
								(unsigned int *)attribute1,
								/*(volatile unsigned int *)(FGHI_FIFO_ENTRY),*/
								uDataSize
							 );

		fglFlush(FGL_PIPESTATE_ALL);	// Pipeline status


		//tuParams.bUseMapFilter = j;
		//tuParams.mipFilter = FGL_FILTER_LINEAR; // 0x2
		//FGL_DPFAPI( fglSetTexUnitParams(0, &tuParams) );
    	tuParams.bUseMagFilter = (FG_BOOL)j;
    	tuParams.bUseMinFilter = (FG_BOOL)j;
    	tuParams.eMipMapFilter = (FGL_MipMapFilter)2;
		fglSetTexUnitParams(0, &tuParams);
/*		
		fglSysModifyReg(FGTU_TEX0_CTRL, 0x0000000F, (j<<TEXCTRL_MAG_FILTER_SHIFT) | 
		                (j<<TEXCTRL_MIN_FILTER_SHIFT) | (2<<TEXCTRL_MIPMAP_FILTER_SHIFT));
*/		
		fglSendToFIFO(4, &uiTmpVertices);
		fglSendToFIFO(4, &uiDummy);

		uDataSize = sizeof(attribute2)/sizeof(attribute2[0]);

 		fglSysTransferToPort(
								(unsigned int *)attribute2,
								/*(volatile unsigned int *)(FGHI_FIFO_ENTRY),*/
								uDataSize
							 );

		fglFlush(FGL_PIPESTATE_ALL);	// Pipeline status

		// Cache flush
		//fglSysCacheFlush();
		//fglSysCacheClear();
		fglClearCache(FGL_CACHECTL_INIT_ALL);

		fglSysDelay(50);

//		drvsys_clear_buf((unsigned int*)(FIMG_COLOR_BUFFER+m_FBOffsize), CLEAR_SCREEN_SIZE, 0x0);


    }
	fglSetLODControl(0x00000000);


    return NO_ERROR;
}

