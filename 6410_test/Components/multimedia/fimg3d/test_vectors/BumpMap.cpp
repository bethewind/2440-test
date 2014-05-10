/*******************************************************************************
 *
 *	ViP model for FIMGSE
 *		version 1.0
 *
 *	BumpMap.c
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
 *	2006. 1. 27  by cheolkyoo.kim
 *
 *  Description
 *
 *	$RCSfile: BumpMap.cpp,v $
 *	$Revision: 1.6 $
 *	$Author: cheolkyoo.kim $
 *	$Date: 2006/05/08 05:34:30 $
 *	$Locker:  $
 *
 *	$Source: C:/CVS/CVSrepository/FIMG-3DSE_SW/fimg3dse_fpga/fimg3d/src/BumpMap.cpp,v $
 *	$State: Exp $
 *	$Log: BumpMap.cpp,v $
 *	Revision 1.6  2006/05/08 05:34:30  cheolkyoo.kim
 *	no message
 *	
 *	Revision 1.5  2006/04/13 11:53:44  cheolkyoo.kim
 *	no message
 *
 *	Revision 1.4  2006/04/11 01:11:07  inhosens.lee
 *	updated with fimg_ra and fimg_tu codes
 *
 *	Revision 1.3  2006/04/05 04:06:07  cheolkyoo.kim
 *	Replaced ~_XCORD and ~_YCORD to ~_XCOORD and ~_YCOORD.
 *
 *	Revision 1.2  2006/03/30 04:27:42  cheolkyoo.kim
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


#include "BumpMap.vsa.h"
#include "BumpMap.psa.h"
/****************************************************************************
 *  DEFINES
 ****************************************************************************/

int BumpMap(void)

{
	//unsigned int i;
	unsigned int nNumOfVertices;
	unsigned int nNumofData;
	//unsigned int *pMatrix;
	//unsigned int ConstFloatAddr;

	FGL_TexUnitParams tuParams;

	float retangle[] =
	{
		//  x      y     z     w    nx    ny    nz    nw     r     g     b     a     s     t     r     q
		 1.0f,  1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.7f, 0.7f, 0.7f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
		-1.0f,  1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.7f, 0.7f, 0.7f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.7f, 0.7f, 0.7f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,

		 1.0f,  1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.7f, 0.7f, 0.7f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.7f, 0.7f, 0.7f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		 1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.7f, 0.7f, 0.7f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f
	};

	Matrix4 matMVP;

	float Identity[16] =
	{
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	float eyePosition[4] = {0.0f, 0.0f, 5.0f, 1.0f};
	float lightPosition[4] = {0.5f, 0.5f, 3.0f, 0.0f};

    if (fglLoadVShader(BumpMap_vsa) != FGL_ERR_NO_ERROR) {
		return ERROR;
    }

    if (fglLoadPShader(BumpMap_psa) != FGL_ERR_NO_ERROR) {
		return ERROR;
    }

/*
    ConstFloatAddr =(unsigned int)FGVS_CFLOAT_SADDR;
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

	pMatrix = (unsigned int *)&lightPosition[0];
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
    Offset = fglWriteVertexShaderConstFloat(Offset,  4, lightPosition);
	

	drvsys_clear_buf((unsigned int*)(FIMG_COLOR_BUFFER), CLEAR_SCREEN_SIZE, 0x0);


	//fglSysPollingMask(FG_PIPELINE_STATUS, 0x0, 0x0003371F); // Pipeline status
	// Pipeline status
	fglFlush(FGL_PIPESTATE_ALL);	// Pipeline status


/*
	// Vertex Shader SFR set
	WRITEREG(FGVS_ATTRIB_NUM, 0x00040004); // Attribute counter of VS output & input
	WRITEREG(FGVS_CONFIG, 0x00000001); // Configuration register
	WRITEREG(FGVS_OUT_ATTRIB_IDX0, 0x03020100);

	// Pixel Shader SFR set
	WRITEREG(FGIB_EXE_MODE, 0x00000000); // Pixel shader execution mode control register
	WRITEREG(FGIB_PC_COPY, 0x00000001); // Copy PSPCS_ADDR value to program counter
	WRITEREG(FGIB_ATTRIB_NUM, 0x00000003); // Number of attribute of current context
	fglSysPollingMask(FGIB_INBUF_STATUS, 0x0, 0x00000001); // Status signal of pixel shader input buffer
	WRITEREG(FGIB_EXE_MODE, 0x00000001); // Pixel shader execution mode control register
*/
    fglSetVShaderAttribTable(FGL_OUTPUT_ATTRIB_IDX0, 0x03020100);
    fglSetVShaderAttribTable(FGL_OUTPUT_ATTRIB_IDX1, 0x07060504);
    fglSetVShaderAttribTable(FGL_OUTPUT_ATTRIB_IDX2, 0x0B0A0908);
    fglVSSetAttribNum(4);
    fglPSSetAttributeNum(3);


	// Pawn Model test main
	{
		// Primitive Engine SRF Set
		FGL_Vertex Vtx;
		Vtx.prim = FGL_PRIM_TRIANGLES;
		Vtx.enablePointSize = FGL_FALSE;
		Vtx.numVSOut = 3;
		Vtx.shadeModel = FGL_SHADING_SMOOTH;
		Vtx.colorAttribIdx = 0;
		
		fglSetVertex(&Vtx); // Vertex context register

		// Raster SFR set
#if 0
		fglSetLODControl(0x0) );
#else
		fglSetLODRegister(FGL_LODCOEFF_DISABLE,
						FGL_LODCOEFF_DISABLE,
						FGL_LODCOEFF_DISABLE,
						FGL_LODCOEFF_DISABLE,
						FGL_LODCOEFF_DISABLE,
						FGL_LODCOEFF_DISABLE,
						FGL_LODCOEFF_DISABLE,
						FGL_LODCOEFF_DISABLE);
#endif
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
// Sampler0   
    	fglSetTexUnitParams(0, &tuParams);
    	fglSetTexBaseAddr(0, FIMG_TEXTURE_MEMORY);
//Sampler1
		fglSetTexUnitParams(1, &tuParams);
		fglSetTexBaseAddr(1, FIMG_TEXTURE_MEMORY + 0x40000);

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

		//fglSysPollingMask(FG_PIPELINE_STATUS, 0x0, 0x0003371F); // Pipeline status
		// Pipeline status
		fglFlush(FGL_PIPESTATE_ALL);	// Pipeline status

		// Cache flush
		//fglSysCacheFlush();
		fglClearCache(FGL_CACHECTL_INIT_ALL);
		//fglSysDelay(50);
 	}

	//	drvsys_clear_buf((unsigned int*)(FIMG_COLOR_BUFFER), CLEAR_SCREEN_SIZE, 0x0);


	//const char* pFileName_SFR_Dump = "SFR.txt";
	//fglDumpContext(pFileName_SFR_Dump);

    return NO_ERROR;
}


