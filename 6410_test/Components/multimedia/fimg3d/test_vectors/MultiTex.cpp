/*******************************************************************************
 *
 *	ViP model for FIMGSE
 *		version 1.0
 *
 *	MultiTex.c
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
 *	2006. 1. 26  by cheolkyoo.kim
 *
 *  Description
 *
 *	$RCSfile: MultiTex.cpp,v $
 *	$Revision: 1.6 $
 *	$Author: cheolkyoo.kim $
 *	$Date: 2006/05/08 05:34:28 $
 *	$Locker:  $
 *
 *	$Source: C:/CVS/CVSrepository/FIMG-3DSE_SW/fimg3dse_fpga/fimg3d/src/MultiTex.cpp,v $
 *	$State: Exp $
 *	$Log: MultiTex.cpp,v $
 *	Revision 1.6  2006/05/08 05:34:28  cheolkyoo.kim
 *	no message
 *	
 *	Revision 1.5  2006/04/13 11:54:17  cheolkyoo.kim
 *	no message
 *
 *	Revision 1.4  2006/04/10 05:34:48  inhosens.lee
 *	updated with fimg_ra and fimg_tu codes
 *
 *	Revision 1.3  2006/04/05 04:06:08  cheolkyoo.kim
 *	Replaced ~_XCORD and ~_YCORD to ~_XCOORD and ~_YCOORD.
 *
 *	Revision 1.2  2006/03/30 04:27:44  cheolkyoo.kim
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


#include "MultiTex.vsa.h"
#include "MultiTex.psa.h"
//#include "Sphere.h"

/****************************************************************************
 *  DEFINES
 ****************************************************************************/

#if 0

int MultiTex(void)

{
	//unsigned int i;
	unsigned int nNumOfVertices;
	unsigned int nNumofData;
	//unsigned int *pMatrix;
	//unsigned int ConstFloatAddr;
//	unsigned int uRegVal;
	FGL_TexUnitParams tuParams;

	float Identity[16] = {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	float eyePosition[4] = {0.0f, 0.0f, 5.0f, 1.0f};

	float lightDirection[4] = {0.1f, 0.5f, 1.0f, 1.0f};

    if (fglLoadVShader(MultiTex_vsa) != FGL_ERR_NO_ERROR) {
		return ERROR;
    }

    if (fglLoadPShader(MultiTex_psa) != FGL_ERR_NO_ERROR) {
		return ERROR;
    }
 
 	Matrix4 matMVP, matProj;
 	matProj.SetAsOrthoMatrix(-2.4, 2.4, -3.2, 3.2, 0.0, 100.0);
 	matMVP.Translate(0, 0, -5);
 	matMVP = matProj * matMVP;
/*
    ConstFloatAddr = FGVS_CFLOAT_SADDR;
    ConstFloatAddr = matMVP.WriteMatrix4(ConstFloatAddr);

	pMatrix = (unsigned int *)&Identity[0];
	for(i=0; i < 12; i++)
	{
		WRITEREG(ConstFloatAddr, *pMatrix);
		pMatrix++;
		ConstFloatAddr += 4;
	}

	pMatrix = (unsigned int *)&eyePosition[0];
	for(i=0; i < 4; i++)
	{
		WRITEREG(ConstFloatAddr, *pMatrix);
		pMatrix++;
		ConstFloatAddr += 4;
	}

	pMatrix = (unsigned int *)&lightDirection[0];
	for(i=0; i < 4; i++)
	{
		WRITEREG(ConstFloatAddr, *pMatrix);
		pMatrix++;
		ConstFloatAddr += 4;
	}
*/	
    unsigned int Offset = fglWriteVertexShaderConstFloat(0, 16, matMVP.m[0]);
    Offset = fglWriteVertexShaderConstFloat(Offset, 12, Identity);
    Offset = fglWriteVertexShaderConstFloat(Offset,  4, eyePosition);
    Offset = fglWriteVertexShaderConstFloat(Offset,  4, lightDirection);
	

	drvsys_clear_buf((unsigned int*)FIMG_DEPTH_BUFFER, CLEAR_SCREEN_SIZE, 0xFFFFFFFF);
	drvsys_clear_buf((unsigned int*)(FIMG_COLOR_BUFFER), CLEAR_SCREEN_SIZE, 0x0);


	fglFlush(FGL_PIPESTATE_ALL);	// Pipeline status


/*
	// Vertex Shader SFR set
	WRITEREG(FGVS_ATTRIB_NUM, 0x00030004); // Attribute counter of VS output & input
	WRITEREG(FGVS_CONFIG, 0x00000001); // Configuration register
	WRITEREG(FGVS_OUT_ATTRIB_IDX0, 0x03020100);

	// Pixel Shader SFR set
	WRITEREG(FGPS_EXE_MODE, 0x00000000); // Pixel shader execution mode control register
	WRITEREG(FGPS_PC_COPY, 0x00000001); // Copy PSPCS_ADDR value to program counter
	WRITEREG(FGPS_ATTRIB_NUM, 0x00000002); // Number of attribute of current context
	fglSysPollingMask(FGPS_INBUF_STATUS, 0x0, 0x00000001); // Status signal of pixel shader input buffer
	WRITEREG(FGPS_EXE_MODE, 0x00000001); // Pixel shader execution mode control register

	// Per-fragment Unit SFR set
	WRITEREG(FGPF_DEPTHBUF_ADDR, FIMG_DEPTH_BUFFER); // Depth/Stencil buffer base address
	uRegVal = READREG(FGPF_DEPTH);
	WRITEREG(FGPF_DEPTH, 0x00000005); // Depth test LESS function & enable set
*/
    fglSetVShaderAttribTable(FGL_OUTPUT_ATTRIB_IDX0, 0x03020100);
    fglSetVShaderAttribTable(FGL_OUTPUT_ATTRIB_IDX1, 0x07060504);
    fglSetVShaderAttribTable(FGL_OUTPUT_ATTRIB_IDX2, 0x0B0A0908);
 
    fglVSSetAttribNum(4);
    fglPSSetAttributeNum(2);

	// Depth/Stencil buffer base address
	fglSetZBufBaseAddr(FIMG_DEPTH_BUFFER);
	fglSetDepthParams(FGL_COMP_LESS);
    fglEnablePerFragUnit(FGL_PF_DEPTH, FGL_TRUE);

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
		//WRITEREG(FGRA_LOD_CTRL, 0x00000038); // Level of detail attribute1 set
		fglSetLODControl(0x0);
//		WRITEREG(FGRA_LOD_CTRL, 0x00000000); // Level of detail attribute1 set
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
    	tuParams.bUseMagFilter = FGL_FALSE;
    	tuParams.bUseMinFilter = FGL_FALSE;
     	tuParams.eMipMapFilter = FGL_FILTER_DISABLE;
    	tuParams.uUSize = 512;
    	tuParams.uVSize = 256;
    	tuParams.uPSize = 1;
// Sampler0   
    	fglSetTexUnitParams(0, &tuParams);
    	fglSetTexBaseAddr(0, FIMG_TEXTURE_MEMORY);
//Sampler1
		fglSetTexUnitParams(1, &tuParams);
		fglSetTexBaseAddr(1, FIMG_TEXTURE_MEMORY + 0x40000);
#else
		// Sampler0: Texture0 Unit SFR Setting
		tuParams.type = FGL_TEX_2D;	// 0x1
		tuParams.bOnHalfDecimation = 0x0;	// OFF
		tuParams.deciFactor = FGL_TEX_NO_DECIMATION;	//0x0
		tuParams.ckey = FGL_CKEY_DISABLE;	// 0x0
		tuParams.bTexExp = 0x1;	// 0 Expansion
		tuParams.bUseMapFilter = 0x0;	// don't use filter
		tuParams.mipFilter = FGL_FILTER_DISABLE; // 0x0
		tuParams.format = FGL_TEXEL_RGB565; // 0x1
		tuParams.uMode = FGL_TEX_WRAP_REPEAT; // 0x0
		tuParams.vMode = FGL_TEX_WRAP_REPEAT; // 0x0
		tuParams.uSize = FGL_TEX_SIZE_512P; // 512 pixels
		tuParams.vSize = FGL_TEX_SIZE_256P; // 256 pixels

		fglSetTexUnitParams(0, &tuParams) );
		fglSetTexMipmapLevel(0, FGL_MIPMAP_BASE_LEVEL, 0x0) );
		fglSetTexMipmapLevel(0, FGL_MIPMAP_MAX_LEVEL, 0x0) );
		fglSetTexBaseAddr(0, FIMG_TEXTURE_MEMORY) );
//		WRITEREG(FGTU_TEX0_CTRL, 0x08101098); 	// FIMG_TUSFR_TEX0_STATUS
//		WRITEREG(FGTU_TEX0_BASE_LEVEL, 0x00000000); 	// FIMG_TUSFR_TEX0_BASE_LEVEL
//		WRITEREG(FGTU_TEX0_MAX_LEVEL, 0x00000000); 	// FIMG_TUSFR_TEX0_MAX_LEVEL
//		WRITEREG(FGTU_TEX0_BASE_ADDR, FIMG_TEXTURE_MEMORY); 	// FIMG_TUSFR_TEX0_BASE_ADDRESS

		// Sampler1: Texture1 Unit SFR Setting
		fglSetTexUnitParams(1, &tuParams) );
		fglSetTexMipmapLevel(1, FGL_MIPMAP_BASE_LEVEL, 0x0) );
		fglSetTexMipmapLevel(1, FGL_MIPMAP_MAX_LEVEL, 0x0) );
		fglSetTexBaseAddr(1, FIMG_TEXTURE_MEMORY + 0x40000) );
//		WRITEREG(FGTU_TEX1_CTRL, 0x08101098); 	// FIMG_TUSFR_TEX0_STATUS
//		WRITEREG(FGTU_TEX1_BASE_LEVEL, 0x00000000); 	// FIMG_TUSFR_TEX0_BASE_LEVEL
//		WRITEREG(FGTU_TEX1_MAX_LEVEL, 0x00000000); 	// FIMG_TUSFR_TEX0_MAX_LEVEL
//		WRITEREG(FGTU_TEX1_BASE_ADDR, (FIMG_TEXTURE_MEMORY + 0x40000)); 	// FIMG_TUSFR_TEX0_BASE_ADDRESS
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

		//nNumOfVertices = nNumTrisSphere * 3;
		//nNumofData = nNumAttributesSphere * nNumOfVertices;
		nNumOfVertices = 1024 * 3;
		nNumofData = 16 * nNumOfVertices;		

		unsigned int uiDummy = 0xFFFFFFFF;
		fglSendToFIFO(4, &nNumOfVertices);
		fglSendToFIFO(4, &uiDummy);
		
		fglSysTransferToPort(
								/*(unsigned int *)Sphere_Data,*/
								(unsigned int *)FIMG_GEOMETRY_MEMORY,
								/*(volatile unsigned int *)(FGHI_FIFO_ENTRY),*/
								nNumofData
							 );

		fglFlush(FGL_PIPESTATE_ALL);	// Pipeline status

		// Cache flush
		// fglSysCacheFlush();
		fglClearCache(FGL_CACHECTL_INIT_ALL);

		fglSysDelay(50);

//		drvsys_clear_buf((unsigned int*)(FIMG_COLOR_BUFFER), CLEAR_SCREEN_SIZE, 0x0);
		drvsys_clear_buf((unsigned int*)FIMG_DEPTH_BUFFER, CLEAR_SCREEN_SIZE, 0x0);

 	}

    fglEnablePerFragUnit(FGL_PF_DEPTH, FGL_FALSE);
    return NO_ERROR;
}

#else



int MultiTex(void)

{
	//unsigned int i;
	unsigned int nNumOfVertices;
	unsigned int nNumofData;
	//unsigned int *pMatrix;
	//unsigned int ConstFloatAddr;
//	unsigned int uRegVal;
	FGL_TexUnitParams tuParams;

	float Identity[16] = {
//		1.0f, 0.0f, 0.0f, 0.0f,
//		0.0f, 1.0f, 0.0f, 0.0f,
//		0.0f, 0.0f, 1.0f, 0.0f,
//		0.0f, 0.0f, 0.0f, 1.0f

		1.2f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.2f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.2f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.2f

	};

	float eyePosition[4] = {0.0f, 0.0f, 3.0f, 1.0f};

	float lightDirection[4] = {0.1f, 0.5f, 1.0f, 1.0f};

    if (fglLoadVShader(MultiTex_vsa) != FGL_ERR_NO_ERROR) {
		return ERROR;
    }

    if (fglLoadPShader(MultiTex_psa) != FGL_ERR_NO_ERROR) {
		return ERROR;
    }
 
 	Matrix4 matMVP, matProj;
 	matProj.SetAsOrthoMatrix(-2.4, 2.4, -3.2, 3.2, 0.0, 100.0);
 	matMVP.Translate(0, 0, -5);
 	matMVP = matProj * matMVP;
	
    unsigned int Offset = fglWriteVertexShaderConstFloat(0, 16, matMVP.m[0]);
    Offset = fglWriteVertexShaderConstFloat(Offset, 12, Identity);
    Offset = fglWriteVertexShaderConstFloat(Offset,  4, eyePosition);
    Offset = fglWriteVertexShaderConstFloat(Offset,  4, lightDirection);
	

	drvsys_clear_buf((unsigned int*)FIMG_DEPTH_BUFFER, CLEAR_SCREEN_SIZE, 0xFFFFFFFF);
	drvsys_clear_buf((unsigned int*)(FIMG_COLOR_BUFFER), CLEAR_SCREEN_SIZE, 0x0);


	fglFlush(FGL_PIPESTATE_ALL);	// Pipeline status


	fglSetVShaderAttribTable(FGL_OUTPUT_ATTRIB_IDX0, 0x03020100);
	fglSetVShaderAttribTable(FGL_OUTPUT_ATTRIB_IDX1, 0x07060504);
	fglSetVShaderAttribTable(FGL_OUTPUT_ATTRIB_IDX2, 0x0B0A0908);
	 
	fglVSSetAttribNum(4);
	fglPSSetAttributeNum(2);

	// Depth/Stencil buffer base address
	fglSetZBufBaseAddr(FIMG_DEPTH_BUFFER);
	fglSetDepthParams(FGL_COMP_LESS);
	fglEnablePerFragUnit(FGL_PF_DEPTH, FGL_TRUE);

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
		//WRITEREG(FGRA_LOD_CTRL, 0x00000038); // Level of detail attribute1 set
		fglSetLODControl(0x0);
//		WRITEREG(FGRA_LOD_CTRL, 0x00000000); // Level of detail attribute1 set
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
    	tuParams.bUseMagFilter = FGL_FALSE;
    	tuParams.bUseMinFilter = FGL_FALSE;
     	tuParams.eMipMapFilter = FGL_FILTER_DISABLE;
    	tuParams.uUSize = 512;
    	tuParams.uVSize = 256;
    	tuParams.uPSize = 1;
// Sampler0   
    	fglSetTexUnitParams(0, &tuParams);
    	fglSetTexBaseAddr(0, FIMG_TEXTURE_MEMORY);
//Sampler1
		fglSetTexUnitParams(1, &tuParams);
		fglSetTexBaseAddr(1, FIMG_TEXTURE_MEMORY + 0x40000);
#else
		// Sampler0: Texture0 Unit SFR Setting
		tuParams.type = FGL_TEX_2D;	// 0x1
		tuParams.bOnHalfDecimation = 0x0;	// OFF
		tuParams.deciFactor = FGL_TEX_NO_DECIMATION;	//0x0
		tuParams.ckey = FGL_CKEY_DISABLE;	// 0x0
		tuParams.bTexExp = 0x1;	// 0 Expansion
		tuParams.bUseMapFilter = 0x0;	// don't use filter
		tuParams.mipFilter = FGL_FILTER_DISABLE; // 0x0
		tuParams.format = FGL_TEXEL_RGB565; // 0x1
		tuParams.uMode = FGL_TEX_WRAP_REPEAT; // 0x0
		tuParams.vMode = FGL_TEX_WRAP_REPEAT; // 0x0
		tuParams.uSize = FGL_TEX_SIZE_512P; // 512 pixels
		tuParams.vSize = FGL_TEX_SIZE_256P; // 256 pixels

		fglSetTexUnitParams(0, &tuParams) );
		fglSetTexMipmapLevel(0, FGL_MIPMAP_BASE_LEVEL, 0x0) );
		fglSetTexMipmapLevel(0, FGL_MIPMAP_MAX_LEVEL, 0x0) );
		fglSetTexBaseAddr(0, FIMG_TEXTURE_MEMORY) );

		// Sampler1: Texture1 Unit SFR Setting
		fglSetTexUnitParams(1, &tuParams) );
		fglSetTexMipmapLevel(1, FGL_MIPMAP_BASE_LEVEL, 0x0) );
		fglSetTexMipmapLevel(1, FGL_MIPMAP_MAX_LEVEL, 0x0) );
		fglSetTexBaseAddr(1, FIMG_TEXTURE_MEMORY + 0x40000) );
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

		//nNumOfVertices = nNumTrisSphere * 3;
		//nNumofData = nNumAttributesSphere * nNumOfVertices;
		nNumOfVertices = 1024 * 3;
		nNumofData = 16 * nNumOfVertices;		

		unsigned int uiDummy = 0xFFFFFFFF;
		fglSendToFIFO(4, &nNumOfVertices);
		fglSendToFIFO(4, &uiDummy);
		
		fglSysTransferToPort(
								/*(unsigned int *)Sphere_Data,*/
								(unsigned int *)FIMG_GEOMETRY_MEMORY,
								/*(volatile unsigned int *)(FGHI_FIFO_ENTRY),*/
								nNumofData
							 );

		fglFlush(FGL_PIPESTATE_ALL);	// Pipeline status

		// Cache flush
		// fglSysCacheFlush();
		fglClearCache(FGL_CACHECTL_INIT_ALL);

		fglSysDelay(50);

//		drvsys_clear_buf((unsigned int*)(FIMG_COLOR_BUFFER), CLEAR_SCREEN_SIZE, 0x0);
		drvsys_clear_buf((unsigned int*)FIMG_DEPTH_BUFFER, CLEAR_SCREEN_SIZE, 0x0);

 	}

    fglEnablePerFragUnit(FGL_PF_DEPTH, FGL_FALSE);
    return NO_ERROR;
}


#endif

