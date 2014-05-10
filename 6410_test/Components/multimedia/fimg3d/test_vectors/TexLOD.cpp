/*******************************************************************************
 *
 *	ViP model for FIMGSE
 *		version 1.0
 *
 *	TexLOD.c
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
 *	2005. 1. 17	by cheolkyoo.kim
 *
 *  Description
 *
 *	$RCSfile: TexLOD.cpp,v $
 *	$Revision: 1.6 $
 *	$Author: cheolkyoo.kim $
 *	$Date: 2006/05/08 05:37:07 $
 *	$Locker:  $
 *
 *	$Source: C:/CVS/CVSrepository/FIMG-3DSE_SW/fimg3dse_fpga/fimg3d/src/TexLOD.cpp,v $
 *	$State: Exp $
 *	$Log: TexLOD.cpp,v $
 *	Revision 1.6  2006/05/08 05:37:07  cheolkyoo.kim
 *	no message
 *	
 *	Revision 1.5  2006/04/13 11:54:08  cheolkyoo.kim
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

#include "Fimg3DTest.h"

//#include "Debug.h"

#include "TexLod.vsa.h"
#if TARGET_FIMG_VERSION == _FIMG3DSE_VER_1_2
#include "TexLod_v1_2.psa.h"
#else
#include "TexLod.psa.h"
#endif

/****************************************************************************
 *  DEFINES
 ****************************************************************************/
//#define FIMG_TEXTURE_LOD    0x63000080

int TexLOD(void)

{
	unsigned int uDataSize;
	//unsigned int uFloatAddr;
	float fAngle = 0.0f;
	Matrix4 matMVP, matMV, matProj;
	FGL_TexUnitParams tuParams;

	float plan[] =
	{
		// x       y      z      s      t 
		-1.0f,  1.0f,  0.0f,  0.0f,  0.0f,
		 1.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		-1.0f, -1.0f,  0.0f,  0.0f,  1.0f,
		 1.0f, -1.0f,  0.0f,  1.0f,  1.0f
	};


    if (fglLoadVShader(TexLod_vsa) != FGL_ERR_NO_ERROR) {
		return ERROR;
    }

    if (fglLoadPShader(TexLod_psa) != FGL_ERR_NO_ERROR) {
		return ERROR;
    }

	/* Build projection matrix once. */
	//matProj.SetAsPerspectiveMatrix(45.0, 0.75, 1.0, 100.0);
	matProj.SetAsPerspectiveMatrix(45, 1.67, 1.0, 20.0);

	drvsys_clear_buf((unsigned int*)(FIMG_COLOR_BUFFER), CLEAR_SCREEN_SIZE, 0x0);


	fglFlush(FGL_PIPESTATE_ALL);	// Pipeline status


/*
	// Vertex Shader SFR set
	WRITEREG(FGVS_ATTRIB_NUM, 0x00020002); // Attribute counter of VS input & output
	WRITEREG(FGVS_CONFIG, 0x00000001); // Configuration register
    WRITEREG(FGVS_OUT_ATTRIB_IDX0, 0x02020100); // Output attribute index control
    
	// Pixel Shader SFR set
	WRITEREG(FGPS_EXE_MODE, 0x00000000); // Pixel shader execution mode control register
	WRITEREG(FGPS_PC_COPY, 0x00000001); // Copy PSPCS_ADDR value to program counter
	WRITEREG(FGPS_ATTRIB_NUM, 0x00000001); // Number of attribute of current context
	fglSysPollingMask(FGPS_INBUF_STATUS, 0x0, 0x00000001); // Status signal of pixel shader input buffer
	WRITEREG(FGPS_EXE_MODE, 0x00000001); // Pixel shader execution mode control register
*/
    fglSetVShaderAttribTable(FGL_OUTPUT_ATTRIB_IDX0, 0x03020100);
    fglSetVShaderAttribTable(FGL_OUTPUT_ATTRIB_IDX1, 0x07060504);
    fglSetVShaderAttribTable(FGL_OUTPUT_ATTRIB_IDX2, 0x0B0A0908);
    fglVSSetAttribNum(2);
    fglPSSetAttributeNum(1);

	// Primitive Engine SRF Set
	FGL_Vertex Vtx;
	Vtx.prim = FGL_PRIM_TRIANGLE_STRIP;
	Vtx.enablePointSize = FGL_FALSE;
	Vtx.numVSOut = 1;
	Vtx.shadeModel = FGL_SHADING_SMOOTH;
	Vtx.colorAttribIdx = 0;
	
	fglSetVertex(&Vtx); // Vertex context register
	// Raster SFR set
	//WRITEREG(FGRA_LOD_CTRL, 0x00000007); // Level of detail attribute1 set
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
	tuParams.ePaletteFormat = FGL_PALETTE_ARGB8888;
	tuParams.eFormat = FGL_TEXEL_RGB565;
	tuParams.eUMode = FGL_TEX_WRAP_REPEAT;
	tuParams.eVMode = FGL_TEX_WRAP_REPEAT;
	tuParams.bIsNonparametric = FGL_FALSE;
	tuParams.bUseMagFilter = FGL_FALSE;
	tuParams.bUseMinFilter = FGL_FALSE;
//	tuParams.bUseMagFilter = FGL_TRUE;
//	tuParams.bUseMinFilter = FGL_TRUE;
	//tuParams.eMipMapFilter = FGL_FILTER_LINEAR;
	//tuParams.eMipMapFilter = FGL_FILTER_NEAREST;
	tuParams.eMipMapFilter = FGL_FILTER_NEAREST;
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
		
    //cheolkyoo-20070427@ADD Watch Point
    //fglProbeWatchPoints();
	//const char* pFileName_SFR_Dump = "TexLOD.txt";
	//fglDumpContext(pFileName_SFR_Dump);
	
    // Texture format rendering test main
    //for(j = 0; j < 36; j++)
    {
    	fAngle = 140.0f;        
    	matMV.Rotate(0, 1, 0, fAngle);
    	matMV.Translate(0, 0, -3.50);
    	matMVP = matProj * matMV;
		//fAngle += 10.f;

	    //uFloatAddr = FGVS_CFLOAT_SADDR;
	    //matMVP.WriteMatrix4(uFloatAddr);
	    
	    fglWriteVertexShaderConstFloat(0, 16, matMVP.m[0]);
   	    
		unsigned int uiTmpVertices = 4;		
		unsigned int uiDummy = 0xFFFFFFFF;
		fglSendToFIFO(4, &uiTmpVertices);
		fglSendToFIFO(4, &uiDummy);

		uDataSize = sizeof(plan)/sizeof(plan[0]);

 		fglSysTransferToPort(
								(unsigned int *)plan,
								/*(volatile unsigned int *)(FGHI_FIFO_ENTRY),*/
								uDataSize
							 );

		fglFlush(FGL_PIPESTATE_ALL);	// Pipeline status

		// Cache flush
		//fglSysCacheFlush();
		//fglSysCacheClear();
		//lClearCache(FGL_CACHECTL_FLUSH_CCACHE);
//		fglSysDelay(50);
		//fglClearCache(FGL_CACHECTL_FLUSH_CCACHE0 | FGL_CACHECTL_FLUSH_CCACHE1);
		//fglClearCache(FGL_CACHECTL_CLEAR_TCACHE);
		//fglClearCache(FGL_CACHECTL_CLEAR_TCACHE0 | FGL_CACHECTL_CLEAR_TCACHE1);
		//fglClearCache(FGL_CACHECTL_INIT_ALL);
		//fglClearCache(FGL_CACHECTL_FLUSH_CCACHE | FGL_CACHECTL_CLEAR_TCACHE0 | FGL_CACHECTL_CLEAR_TCACHE1);
		fglClearCache(FGL_CACHECTL_FLUSH_CCACHE | FGL_CACHECTL_CLEAR_TCACHE);

		fglSysDelay(100);

		//drvsys_clear_buf((unsigned int*)(FIMG_COLOR_BUFFER+m_FBOffsize), CLEAR_SCREEN_SIZE, 0x0);


    }

    fglSetLODControl(0x0);
    
    return NO_ERROR;
}
