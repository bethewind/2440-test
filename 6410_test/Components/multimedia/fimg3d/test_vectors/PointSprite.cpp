/*******************************************************************************
 *
 *	ViP model for FIMGSE
 *		version 1.0
 *
 *	PointSprite.c
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
 *	2006. 3. 24	by cheolkyoo.kim
 *
 *  Description
 *
 *	$Revision:  $
 *	$Author:  $
 *	$Date:  $
 *	$Locker:  $
 *
 *	$Source:  $
 *	$State:  $
 *	$Log:  $
 ******************************************************************************/
/****************************************************************************
 *  INCLUDES
 ****************************************************************************/
#include "Fimg3DTest.h"


#include "PointSprite.vsa.h"
#include "PointSprite.psa.h"
//#include "PointSprite.h"

/****************************************************************************
 *  DEFINES
 ****************************************************************************/
//#define SPRITE_TEX_BADDR    0x63000080

int PointSprite(void)

{
	FGL_TexUnitParams   tuParams;
	FGL_BlendParam      BlendParam;
	
    if (fglLoadVShader(PointSprite_vsa) != FGL_ERR_NO_ERROR) {
		return ERROR;
    }

    if (fglLoadPShader(PointSprite_psa) != FGL_ERR_NO_ERROR) {
		return ERROR;
    }

	drvsys_clear_buf((unsigned int*)(FIMG_COLOR_BUFFER), CLEAR_SCREEN_SIZE, 0x0);


	fglFlush(FGL_PIPESTATE_ALL);	// Pipeline status
	
	Matrix4 matMV, matProj, matMVP;
	//matProj.SetAsPerspectiveMatrix(45.0, 0.75, 1.0, 20.0);	
	matProj.SetAsPerspectiveMatrix(45.0, 1.67, 1.0, 20.0);
	
	/* Build projection matrix once. */
/*
	// Vertex Shader SFR set
	WRITEREG(FGVS_ATTRIB_NUM, 0x00040003); // Attribute counter of VS input & output
	WRITEREG(FGVS_CONFIG, 0x00000001); // Configuration register
	WRITEREG(FGVS_OUT_ATTRIB_IDX0, 0x03020100); // Output attribute index control

	// Pixel Shader SFR set
	WRITEREG(FGPS_EXE_MODE, 0x00000000); // Pixel shader execution mode control register
	WRITEREG(FGPS_PC_COPY, 0x00000001); // Copy PSPCS_ADDR value to program counter
	WRITEREG(FGPS_ATTRIB_NUM, 0x00000002); // Number of attribute of current context
	fglSysPollingMask(FGPS_INBUF_STATUS, 0x0, 0x1); // Status signal of pixel shader input buffer
	WRITEREG(FGPS_EXE_MODE, 0x1); // Pixel shader execution mode control register
*/
    fglSetVShaderAttribTable(FGL_OUTPUT_ATTRIB_IDX0, 0x03020100);
    fglSetVShaderAttribTable(FGL_OUTPUT_ATTRIB_IDX1, 0x07060504);
    fglSetVShaderAttribTable(FGL_OUTPUT_ATTRIB_IDX2, 0x0B0A0908);
   
    fglVSSetAttribNum(3);
    fglPSSetAttributeNum(2);

	
    // Point Sprite test main
    {
		matMV.Translate(0., 0., -4.5);
		matMVP = matProj * matMV;
	    //unsigned int pShaderParam = 0x6FD14000;
	    //matMVP.WriteMatrix4(pShaderParam);
	    
	    fglWriteVertexShaderConstFloat(0, 16, matMVP.m[0]);

		// Point sprite coordinates replace register set
		//attribute 1 will be replaced with texture coord.
		fglSetCoordReplace(1);

		// Point min/max size set
		fglSetMinimumPointWidth(1.0f);
		fglSetMaximumPointWidth(1024.0f);

		//point size set
		//WRITEREGF(FGRA_POINT_WIDTH, 1.0f);
		fglSetPointWidth(1.0f);

		// Primitive Engine SRF Set
		FGL_Vertex Vtx;
		Vtx.prim = FGL_PRIM_POINT_SPRITE;
		Vtx.enablePointSize = FGL_TRUE;
		Vtx.numVSOut = 3;
		Vtx.shadeModel = FGL_SHADING_SMOOTH;
		Vtx.colorAttribIdx = 0;
		
		fglSetVertex(&Vtx); // Vertex context register 
		// Raster SFR set
		// Level of detail attribute1 set
		fglSetLODRegister(FGL_LODCOEFF_DISABLE,
					FGL_LODCOEFF_ENABLE_ALL,
					FGL_LODCOEFF_DISABLE,
					FGL_LODCOEFF_DISABLE,
					FGL_LODCOEFF_DISABLE,
					FGL_LODCOEFF_DISABLE,
					FGL_LODCOEFF_DISABLE,
					FGL_LODCOEFF_DISABLE);

		// Blend set
		//WRITEREG(FGPF_BLEND, 0x000022CD);
		BlendParam.colorEqua = FGL_EQ_ADD;
        BlendParam.alphaEqua = FGL_EQ_ADD;
    	BlendParam.dstAlpha = FGL_BLEND_ONE;
		BlendParam.dstColor = FGL_BLEND_ONE;
		BlendParam.srcAlpha = FGL_BLEND_SRC_ALPHA;
		BlendParam.srcColor = FGL_BLEND_SRC_ALPHA;
		BlendParam.constColor = 0x0;
		fglSetBlendParams(&BlendParam);
		fglEnablePerFragUnit(FGL_PF_BLENDING, FGL_TRUE);

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
		//tuParams.bUseMagFilter = FGL_FALSE;
		//tuParams.bUseMinFilter = FGL_FALSE;
		tuParams.eMipMapFilter = FGL_FILTER_DISABLE;
		tuParams.uUSize = 256;
		tuParams.uVSize = 256;
		tuParams.uPSize = 1;
		fglSetTexUnitParams(0, &tuParams);
		fglSetTexBaseAddr(0, FIMG_TEXTURE_MEMORY+0x80);

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
		HIAttr.numComp = 1;
		fglSetAttribute(1, &HIAttr);
		HIAttr.bEndFlag = FGL_TRUE;
		HIAttr.numComp = 4;
		fglSetAttribute(2, &HIAttr);
		
		//unsigned int nNumOfData = sizeof(PointSprite_Data)/sizeof(float);
		//unsigned int nNumOfVertices = nNumOfData/8;
		unsigned int nNumOfData = 800;
		unsigned int nNumOfVertices = 100;
		
		unsigned int uiDummy = 0xFFFFFFFF;
		fglSendToFIFO(4, &nNumOfVertices);
		fglSendToFIFO(4, &uiDummy);

		fglSysTransferToPort(
								/*(unsigned int *)PointSprite_Data,*/
								(unsigned int *)FIMG_GEOMETRY_MEMORY,
								/*(volatile unsigned int *)(FGHI_FIFO_ENTRY),*/
								nNumOfData
							 );


		fglFlush(FGL_PIPESTATE_ALL);	// Pipeline status
		
		//fglSysCacheFlush();
		fglClearCache(FGL_CACHECTL_INIT_ALL);
		fglSysDelay(50);
    }

/*
	if(m_SwapBackBuffer == FALSE) 
	{
		fglSwapBuffer(0);
		drvsys_clear_buf((unsigned int*)(m_FrontFBAddr+m_FBOffsize), CLEAR_SCREEN_SIZE, 0x00000000);
		fglSetColorBufBaseAddr(m_FrontFBAddr);
		m_SwapBackBuffer = TRUE;
	}
	else
	{
		fglSwapBuffer(1);
		drvsys_clear_buf((unsigned int*)(m_BackFBAddr+m_FBOffsize), CLEAR_SCREEN_SIZE, 0x00000000);
		fglSetColorBufBaseAddr(m_BackFBAddr);
		m_SwapBackBuffer = FALSE;
	} 
*/

    
	//const char* pFileName_SFR_Dump = "PointSprite_SFRA.txt";
	//fglDumpContext(pFileName_SFR_Dump);
    
	fglSetLODControl(0x0);
    fglEnablePerFragUnit(FGL_PF_BLENDING, FGL_FALSE);

    return NO_ERROR;
}

