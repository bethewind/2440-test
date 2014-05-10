/*******************************************************************************
 *
 *	ViP model for FIMGSE
 *		version 1.0
 *
 *	Imaging.c
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
 *	2006. 2. 1  by cheolkyoo.kim
 *
 *  Description
 *
 *	$RCSfile: Imaging.cpp,v $
 *	$Revision: 1.6 $
 *	$Author: cheolkyoo.kim $
 *	$Date: 2006/05/08 05:34:29 $
 *	$Locker:  $
 *
 *	$Source: C:/CVS/CVSrepository/FIMG-3DSE_SW/fimg3dse_fpga/fimg3d/src/Imaging.cpp,v $
 *	$State: Exp $
 *	$Log: Imaging.cpp,v $
 *	Revision 1.6  2006/05/08 05:34:29  cheolkyoo.kim
 *	no message
 *	
 *	Revision 1.5  2006/04/13 11:54:26  cheolkyoo.kim
 *	no message
 *
 *	Revision 1.4  2006/04/11 01:12:15  inhosens.lee
 *	updated with fimg_ra and fimg_tu codes
 *
 *	Revision 1.3  2006/04/05 04:06:08  cheolkyoo.kim
 *	Replaced ~_XCORD and ~_YCORD to ~_XCOORD and ~_YCOORD.
 *
 *	Revision 1.2  2006/03/30 04:27:43  cheolkyoo.kim
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
#include "Fimg3DTest.h"

#include "Imaging.vsa.h"
#include "Imaging.psa.h"

/****************************************************************************
 *  DEFINES
 ****************************************************************************/

int Imaging(void)

{
	unsigned int nNumOfVertices;
	unsigned int nNumofData;

	FGL_TexUnitParams tuParams;

	float retangle[] =
	{
		//  x      y     z     w    nx    ny    nz    nw     r     g     b     a     s     t     r     q
		 1.0f,  1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.7f, 0.7f, 0.7f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
		-1.0f,  1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.7f, 0.7f, 0.7f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.7f, 0.7f, 0.7f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,

		 1.0f,  1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.7f, 0.7f, 0.7f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.7f, 0.7f, 0.7f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		 1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.7f, 0.7f, 0.7f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f
	};

	Matrix4 matMVP(2.414214f,  0.0f,       0.0f,       0.0f,
			0.0f,       2.414214f,  0.0f,       0.0f,
			0.0f,       0.0f,      -1.002002f,  3.306807f,
			0.0f,       0.0f,      -1.0f,       3.5f);
	
	float Identity[16] =
	{
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	float eyePosition[4] = {0.0f, 0.0f, 5.0f, 1.0f};
	float lightPosition[4] = {0.258f, 0.516f, 0.775f, 0.258f};
	float GrayScaleWeights[3] = { 0.30f, 0.59f, 0.11f};

	//pVS = &Imaging_vsa[0];
	//pPS = &Imaging_psa[0];

    if (fglLoadVShader(Imaging_vsa) != FGL_ERR_NO_ERROR) {
		return ERROR;
    }

    if (fglLoadPShader(Imaging_psa) != FGL_ERR_NO_ERROR) {
		return ERROR;
    }

    unsigned int Offset = fglWriteVertexShaderConstFloat(0, 16, matMVP.m[0]);
    Offset = fglWriteVertexShaderConstFloat(Offset, 12, Identity);
    Offset = fglWriteVertexShaderConstFloat(Offset,  4, eyePosition);
    Offset = fglWriteVertexShaderConstFloat(Offset,  4, lightPosition);

    fglWriteFragmentShaderConstFloat(0, 3, GrayScaleWeights);



	drvsys_clear_buf((unsigned int*)(FIMG_COLOR_BUFFER), CLEAR_SCREEN_SIZE, 0x0);


	fglFlush(FGL_PIPESTATE_ALL);	// Pipeline status

    fglSetVShaderAttribTable(FGL_OUTPUT_ATTRIB_IDX0, 0x03020100);
    fglSetVShaderAttribTable(FGL_OUTPUT_ATTRIB_IDX1, 0x07060504);
    fglSetVShaderAttribTable(FGL_OUTPUT_ATTRIB_IDX2, 0x0B0A0908);
    fglVSSetAttribNum(4);
    fglPSSetAttributeNum(2);

	// Pawn Model test main
	{
		// Primitive Engine SRF Set
		FGL_Vertex Vtx;
		Vtx.prim = FGL_PRIM_TRIANGLES;
		Vtx.enablePointSize = FGL_FALSE;
		Vtx.numVSOut = 2;
		Vtx.shadeModel = FGL_SHADING_SMOOTH;
		Vtx.colorAttribIdx = 0;
		
		fglSetVertex(&Vtx); // Vertex context register

		// Raster SFR set
		//FGL_DPFAPI( fglSetLODControl(0x0000003F) );
		fglSetLODRegister(FGL_LODCOEFF_ENABLE_ALL,
					FGL_LODCOEFF_ENABLE_ALL,
					FGL_LODCOEFF_DISABLE,
					FGL_LODCOEFF_DISABLE,
					FGL_LODCOEFF_DISABLE,
					FGL_LODCOEFF_DISABLE,
					FGL_LODCOEFF_DISABLE,
					FGL_LODCOEFF_DISABLE);

//		WRITEREG(FGRA_LOD_CTRL, 0x0000003F); // Level of detail attribute1 set
#ifdef _FIMG3DSE_VER_1_2
    	// Texture Unit SFR Setting
    	tuParams.eType = FGL_TEX_2D;
    	tuParams.eColorkey = FGL_CKEY_DISABLE;
    	tuParams.bUseExpansion = FGL_FALSE;
    	tuParams.ePaletteFormat = FGL_PALETTE_ARGB8888;
    	tuParams.eFormat = FGL_TEXEL_ARGB8888;
    	tuParams.eUMode = FGL_TEX_WRAP_REPEAT;
    	tuParams.eVMode = FGL_TEX_WRAP_REPEAT;
    	tuParams.bIsNonparametric = FGL_FALSE;
    	tuParams.bUseMagFilter = FGL_TRUE;
    	tuParams.bUseMinFilter = FGL_TRUE;
    	tuParams.eMipMapFilter = FGL_FILTER_DISABLE;
    	tuParams.uUSize = 256;
    	tuParams.uVSize = 256;
    	tuParams.uPSize = 1;

    	fglSetTexUnitParams(0, &tuParams);
    	fglSetTexBaseAddr(0, FIMG_TEXTURE_MEMORY);
#else
		// Sampler0: Texture0 Unit SFR Setting
		tuParams.type = FGL_TEX_2D;	// 0x1
		tuParams.bOnHalfDecimation = 0x0;	// OFF
		tuParams.deciFactor = FGL_TEX_NO_DECIMATION;	//0x0
		tuParams.ckey = FGL_CKEY_DISABLE;	// 0x0
		tuParams.bTexExp = 0x1;	// 0 Expansion
		tuParams.bUseMapFilter = 0x1;	// bilinear filter
		tuParams.mipFilter = FGL_FILTER_DISABLE; // 0x0
		tuParams.format = FGL_TEXEL_ARGB8888; // 0x6
		tuParams.uMode = FGL_TEX_WRAP_REPEAT; // 0x0
		tuParams.vMode = FGL_TEX_WRAP_REPEAT; // 0x0
		tuParams.uSize = FGL_TEX_SIZE_256P; // 256 pixels 0x8
		tuParams.vSize = FGL_TEX_SIZE_256P; // 256 pixels 0x8

		fglSetTexUnitParams(0, &tuParams) );
		fglSetTexMipmapLevel(0, FGL_MIPMAP_BASE_LEVEL, 0x0) );
		fglSetTexMipmapLevel(0, FGL_MIPMAP_MAX_LEVEL, 0x0) );
		fglSetTexBaseAddr(0, FIMG_TEXTURE_MEMORY) );
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
		fglSetAttribute(2, &HIAttr);
		HIAttr.bEndFlag = FGL_TRUE;
		fglSetAttribute(3, &HIAttr);
		
		nNumOfVertices = 6;
		nNumofData = 16 * nNumOfVertices;

		unsigned int uiDummy = 0xFFFFFFFF;
		fglSendToFIFO(4, &nNumOfVertices);
		fglSendToFIFO(4, &uiDummy);

		fglSysTransferToPort(
								(unsigned int *)retangle,
								/*(volatile unsigned int *)(FGHI_FIFO_ENTRY),*/
								nNumofData
							 );

		fglFlush(FGL_PIPESTATE_ALL);	// Pipeline status

		// Cache flush
		//fglSysCacheFlush();
		fglClearCache(FGL_CACHECTL_INIT_ALL);
		//drvsys_clear_buf((unsigned int*)(FIMG_COLOR_BUFFER), CLEAR_SCREEN_SIZE, 0x0);
 	}

	fglSetLODControl(0x00000000);

    return NO_ERROR;
}


