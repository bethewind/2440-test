/*******************************************************************************
 *
 *	ViP model for FIMGSE
 *		version 1.0
 *
 *	DisplaceVtx.c
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
 *	2006. 1. 25  by cheolkyoo.kim
 *
 *  Description -
 *    Minimum primitive engine diagnostic tests:
 *
 *	$RCSfile: DisplaceVtx.cpp,v $
 *	$Revision: 1.6 $
 *	$Author: cheolkyoo.kim $
 *	$Date: 2006/05/08 05:34:29 $
 *	$Locker:  $
 *
 *	$Source: C:/CVS/CVSrepository/FIMG-3DSE_SW/fimg3dse_fpga/fimg3d/src/DisplaceVtx.cpp,v $
 *	$State: Exp $
 *	$Log: DisplaceVtx.cpp,v $
 *	Revision 1.6  2006/05/08 05:34:29  cheolkyoo.kim
 *	no message
 *	
 *	Revision 1.5  2006/04/13 11:54:26  cheolkyoo.kim
 *	no message
 *
 *	Revision 1.4  2006/04/11 01:12:01  inhosens.lee
 *	updated with fimg_ra and fimg_tu codes
 *
 *	Revision 1.3  2006/04/05 04:06:08  cheolkyoo.kim
 *	Replaced ~_XCORD and ~_YCORD to ~_XCOORD and ~_YCOORD.
 *
 *	Revision 1.2  2006/03/30 04:27:43  cheolkyoo.kim
 *	Thomas-20060330@comment: Add to code using directive WIN32_VIP for VIP
 *
 *	Revision 1.1  2006/03/10 08:29:08  cheolkyoo.kim
 *	Initial import of FIMG-3DSE_SW package
 *
 *
 ******************************************************************************/
/****************************************************************************
 *  INCLUDES
 ****************************************************************************/
#include "Fimg3DTest.h"

#include "library.h"
#include "Displacement.vsa.h"
#include "Displacement.psa.h"

/****************************************************************************
 *  DEFINES
 ****************************************************************************/
int DisplaceVtx(void)

{
	//unsigned int i;
	unsigned int nNumOfVertices;
	unsigned int nNumofData;
	//unsigned int *pMatrix;
	float CurAngle;
	unsigned int Offset = 0;
	FGL_TexUnitParams tuParams;

	float Identity[16] =
	{
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
	Matrix4 matMVP;
	matMVP.SetMatrix4(1.0f,  0.0f,       0.0f,      -4.0f,
			     	  0.0f,  0.813733f, -0.581238f, -1.16248f,
					  0.0f, -0.581354f, -0.813896f,  6.77623f,
					  0.0f, -0.581238f, -0.813733f,  6.97486f);

	float eyePosition[4] = {10.0f, 10.0f, 10.0f, 1.0f};
	float lightDirection[4] = {0.258f, 0.516f, 0.775f, 0.258f};

	if (fglLoadVShader(UART_Printflacement_vsa) != FGL_ERR_NO_ERROR)
	{
		return ERROR;
	}

	if (fglLoadPShader(UART_Printflacement_psa) != FGL_ERR_NO_ERROR)
	{
		return ERROR;
	}

	Offset = fglWriteVertexShaderConstFloat(0, 16, matMVP.m[0]);
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

	// Raster Engine SFR set
	fglSetFaceCullControl(FGL_TRUE, 0/* CCW */, FGL_FACE_BACK);

	// Per-fragment SFR set
	fglSetZBufBaseAddr(FIMG_DEPTH_BUFFER);
	fglSetDepthParams(FGL_COMP_NOTEQUAL);
	fglEnablePerFragUnit(FGL_PF_DEPTH, FGL_TRUE);

	CurAngle = 0.f;
	{
		fglWriteVertexShaderConstFloat(Offset,  4, &CurAngle);
		CurAngle += 30.0f;

		// Primitive Engine SRF Set
		FGL_Vertex Vtx;
		Vtx.prim = FGL_PRIM_TRIANGLES;
		Vtx.enablePointSize = FGL_FALSE;
		Vtx.numVSOut = 2;
		Vtx.shadeModel = FGL_SHADING_SMOOTH;
		Vtx.colorAttribIdx = 0;
		
		fglSetVertex(&Vtx); // Vertex context register
		// Texture Unit SFR Setting
		//FGL_DPFAPI( fglSetLODControl(0x000001C0) );
		fglSetLODRegister(FGL_LODCOEFF_DISABLE,
					FGL_LODCOEFF_DISABLE,
					FGL_LODCOEFF_ENABLE_ALL,
					FGL_LODCOEFF_DISABLE,
					FGL_LODCOEFF_DISABLE,
					FGL_LODCOEFF_DISABLE,
					FGL_LODCOEFF_DISABLE,
					FGL_LODCOEFF_DISABLE);

    	// Texture Unit SFR Setting
		tuParams.eType = FGL_TEX_2D;
	    	tuParams.eColorkey = FGL_CKEY_DISABLE;
    		tuParams.bUseExpansion = FGL_FALSE;
	    	tuParams.ePaletteFormat = FGL_PALETTE_ARGB8888;
	    	tuParams.eFormat = FGL_TEXEL_ARGB8888;
	    	tuParams.eUMode = FGL_TEX_WRAP_REPEAT;
    		tuParams.eVMode = FGL_TEX_WRAP_REPEAT;
	    	tuParams.bIsNonparametric = FGL_FALSE;
    		tuParams.bUseMagFilter = FGL_FALSE;
	    	tuParams.bUseMinFilter = FGL_FALSE;    
	    	tuParams.eMipMapFilter = FGL_FILTER_DISABLE;
	    	tuParams.uUSize = 256;
	    	tuParams.uVSize = 256;
	    	tuParams.uPSize = 1;
    	
	    	fglSetTexUnitParams(0, &tuParams);
    		fglSetTexBaseAddr(0, FIMG_TEXTURE_MEMORY);

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
		
		nNumOfVertices = 450 * 3;
		nNumofData = 16 * nNumOfVertices;

		unsigned int uiDummy = 0xFFFFFFFF;
		fglSendToFIFO(4, &nNumOfVertices);
		fglSendToFIFO(4, &uiDummy);

		fglSysTransferToPort((unsigned int *)FIMG_GEOMETRY_MEMORY, nNumofData);

		fglFlush(FGL_PIPESTATE_ALL);	// Pipeline status

		// Cache flush
		fglClearCache(FGL_CACHECTL_INIT_ALL);
	}

	fglSetFaceCullControl(FGL_FALSE, FGL_FALSE, FGL_FACE_BACK);
	fglSetLODControl(0x0);
	fglEnablePerFragUnit(FGL_PF_DEPTH, FGL_FALSE);
	
	return NO_ERROR;
}


