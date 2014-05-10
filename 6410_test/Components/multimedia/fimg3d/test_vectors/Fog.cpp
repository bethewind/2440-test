/*******************************************************************************
 *
 *	ViP model for FIMGSE
 *		version 1.0
 *
 *	Fog.cpp
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
 *	2006. 8. 24  by cheolkyoo.kim
 *
 *  Description
 *
 *
 ******************************************************************************/
/****************************************************************************
 *  INCLUDES
 ****************************************************************************/
#include "Fimg3DTest.h"


#include "stdio.h"
#include "library.h"
#include "Fog.vsa.h"
#include "Fog.psa.h"

//#include "Fog.h"
/****************************************************************************
 *  DEFINES
 ****************************************************************************/
//#define FIMG_TEXTURE_MESH_EARTH    0x63000080
 
int Fog(void)

{
	unsigned int nNumOfVertices;
	unsigned int nNumofData;
	//unsigned int *pMat;
	//unsigned int pVtxShaderParam;

	//bool SwapBackBuffer = FALSE;

    if (fglLoadVShader(Fog_vsa) != FGL_ERR_NO_ERROR) {
		return ERROR;
    }

    if (fglLoadPShader(Fog_psa) != FGL_ERR_NO_ERROR) {
		return ERROR;
    }

	//drvsys_clear_buf((unsigned int*)FIMG_DEPTH_BUFFER, 0x4B000, 0xFFFFFFFF);
	drvsys_clear_buf((unsigned int*)FIMG_DEPTH_BUFFER, CLEAR_SCREEN_SIZE, 0xFFFFFFFF);
	drvsys_clear_buf((unsigned int*)(FIMG_COLOR_BUFFER), CLEAR_SCREEN_SIZE, 0xFFFFFFFF);


	fglFlush(FGL_PIPESTATE_ALL);	// Pipeline status

/*
	// Vertex Shader SFR set
	WRITEREG(FGVS_ATTRIB_NUM, 0x00040003); // Attribute counter of VS output & input
	WRITEREG(FGVS_CONFIG, 0x00000001); // Configuration register

	// Pixel Shader SFR set
	WRITEREG(FGPS_EXE_MODE, 0x0); // Pixel shader execution mode control register
	WRITEREG(FGPS_PC_COPY, 0x1); // Copy PSPCS_ADDR value to program counter
	WRITEREG(FGPS_ATTRIB_NUM, 0x3); // Number of attribute of current context
	fglSysPollingMask(FGPS_INBUF_STATUS, 0x0, 0x1); // Status signal of pixel shader input buffer
	WRITEREG(FGPS_EXE_MODE, 0x1); // Pixel shader execution mode control register
*/
    fglSetVShaderAttribTable(FGL_OUTPUT_ATTRIB_IDX0, 0x03020100);
    fglSetVShaderAttribTable(FGL_OUTPUT_ATTRIB_IDX1, 0x07060504);
    fglSetVShaderAttribTable(FGL_OUTPUT_ATTRIB_IDX2, 0x0B0A0908);

    fglVSSetAttribNum(3);
    fglPSSetAttributeNum(3);

	// Per-fragment Unit SFR set
	// Depth/Stencil buffer base address
	fglSetZBufBaseAddr(FIMG_DEPTH_BUFFER);
	fglSetDepthParams(FGL_COMP_LESS);
	fglEnablePerFragUnit(FGL_PF_DEPTH, FGL_TRUE);

	FGL_TexUnitParams tuParams;
	
#ifdef _FIMG3DSE_VER_1_2
	// Texture Unit SFR Setting
	tuParams.eType = FGL_TEX_2D;
	tuParams.eColorkey = FGL_CKEY_DISABLE;
	tuParams.bUseExpansion = FGL_FALSE;
	tuParams.ePaletteFormat = FGL_PALETTE_ARGB8888;
	tuParams.eFormat = FGL_TEXEL_RGB565;
	tuParams.eUMode = FGL_TEX_WRAP_CLAMP;
	tuParams.eVMode = FGL_TEX_WRAP_CLAMP;
	tuParams.bIsNonparametric = FGL_FALSE;
	tuParams.bUseMagFilter = FGL_TRUE;
	tuParams.bUseMinFilter = FGL_TRUE;
	tuParams.eMipMapFilter = FGL_FILTER_DISABLE;
	tuParams.uUSize = 256;
	tuParams.uVSize = 256;
	tuParams.uPSize = 1;
	
	fglSetTexUnitParams(0, &tuParams);
	fglSetTexBaseAddr(0, FIMG_TEXTURE_MEMORY+0x80);
#else
	WRITEREG(FGTU_TEX0_CTRL, 0x08141A88); 	            // FIMG_TUSFR_TEX0_STATUS
	WRITEREG(FGTU_TEX0_BASE_LEVEL, 0x00000000);         // FIMG_TUSFR_TEX0_BASE_LEVEL
	WRITEREG(FGTU_TEX0_MAX_LEVEL, 0x00000000); 	        // FIMG_TUSFR_TEX0_MAX_LEVEL
    WRITEREG(FGTU_TEX0_BASE_ADDR, FIMG_TEXTURE_MEMORY+0x80);
#endif

    /* Build projection matrix once. */
	Matrix4 matProj, matV, matM, matMVP;
	//matProj.SetAsPerspectiveMatrix(45.0, 0.75, 1.0, 100.0);
	matProj.SetAsPerspectiveMatrix(45.0, 1.67, 1.0, 100.0);
	matV.LookAtMatrix(0.0f, 0.0f, -10.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f);
	matM.Rotate(0, 1, 0, 180);
	//matM.Rotate(0, 1, 0, 90);
	matM.Rotate(1, 0, 0, -40);
	matMVP = matProj * matV * matM;
	
    //pVtxShaderParam = FGVS_CFLOAT_SADDR;
    //pVtxShaderParam = matMVP.WriteMatrix4(pVtxShaderParam);
	//float FogFactor[4] = { 1.25f, -0.125f, 0.0f, 0.0f};
	//float FogFactor[4] = { 1.25f, -0.075f, 0.0f, 0.0f};
	float FogFactor[4] = { 1.25f, -0.095f, 0.0f, 0.0f};
/*
	pMat = (unsigned int *)&FogFactor[0];
	pVtxShaderParam += 8;
	
	WRITEREG(pVtxShaderParam++, *pMat++);
	WRITEREG(pVtxShaderParam++, *pMat++);
	WRITEREG(pVtxShaderParam++, *pMat++);
	WRITEREG(pVtxShaderParam++, *pMat++);
*/
    unsigned int Offset = fglWriteVertexShaderConstFloat(0, 16, matMVP.m[0]);
    Offset += 8;
    Offset = fglWriteVertexShaderConstFloat(Offset, 4, FogFactor);

	
	// Fog test main
	//while(1)
	{
		// Primitive Engine SRF Set
		FGL_Vertex Vtx;
		Vtx.prim = FGL_PRIM_TRIANGLES;
		Vtx.enablePointSize = FGL_FALSE;
		Vtx.numVSOut = 3;
		Vtx.shadeModel = FGL_SHADING_SMOOTH;
		Vtx.colorAttribIdx = 0;
		
		fglSetVertex(&Vtx); // Vertex context register

		// Host Interface SFR Set
		FGL_HInterface HInterface;
		HInterface.enableAutoInc = FGL_TRUE;
		HInterface.enableVtxBuffer = FGL_FALSE;
		HInterface.enableVtxCache = FGL_FALSE;	
		HInterface.idxType = FGL_INDEX_DATA_UINT;
		HInterface.numVSOut = 4;
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
		fglSetAttribute(1, &HIAttr);
		HIAttr.bEndFlag = FGL_TRUE;
		HIAttr.numComp = 2;
		fglSetAttribute(2, &HIAttr);
		
		//const int nNumTrisMeshEarth = 9672;
        //const int nNumAttributesMeshEarth = 8;
		//nNumOfVertices = nNumTrisMeshEarth;
		//nNumofData = nNumAttributesMeshEarth * nNumOfVertices;
		nNumOfVertices = 9672;
		nNumofData = 8 * nNumOfVertices;

		unsigned int uiDummy = 0xFFFFFFFF;
		fglSendToFIFO(4, &nNumOfVertices);
		fglSendToFIFO(4, &uiDummy);

		fglSysTransferToPort(
								/*(unsigned int *)MeshEarth_Data,*/
								(unsigned int *)FIMG_GEOMETRY_MEMORY,
								/*(volatile unsigned int *)(FGHI_FIFO_ENTRY),*/
								nNumofData
							 );


		fglFlush(FGL_PIPESTATE_ALL);	// Pipeline status

		// Cache flush
		//fglSysCacheFlush();
		fglClearCache(FGL_CACHECTL_INIT_ALL);
		//fglSysDelay(50);
 	}
 	
        drvsys_clear_buf((unsigned int*)FIMG_DEPTH_BUFFER, CLEAR_SCREEN_SIZE, 0x0);
		//drvsys_clear_buf((unsigned int*)(FIMG_COLOR_BUFFER), CLEAR_SCREEN_SIZE, 0x0);


    //WRITEREG(FGPF_DEPTH, 0x0);
	if(fglEnablePerFragUnit(FGL_PF_DEPTH, FGL_FALSE) != FGL_ERR_NO_ERROR) {
		UART_Printf("\nDepth buffer test: The fglEnablePerFragUnit fial");
	}
    
    return NO_ERROR;
}


