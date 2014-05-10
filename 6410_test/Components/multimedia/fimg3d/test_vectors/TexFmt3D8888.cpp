/*******************************************************************************
 *
 *	ViP model for FIMGSE
 *		version 1.0
 *
 *	TexFmt3D8888.c
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
 *	2005. 2. 8	by cheolkyoo.kim
 *
 *  Description
 *
 *	$RCSfile: TexFmt3D8888.cpp,v $
 *	$Revision: 1.6 $
 *	$Author: cheolkyoo.kim $
 *	$Date: 2006/05/08 05:37:09 $
 *	$Locker:  $
 *
 *	$Source: C:/CVS/CVSrepository/FIMG-3DSE_SW/fimg3dse_fpga/fimg3d/src/TexFmt3D8888.cpp,v $
 *	$State: Exp $
 *	$Log: TexFmt3D8888.cpp,v $
 *	Revision 1.6  2006/05/08 05:37:09  cheolkyoo.kim
 *	no message
 *	
 *	Revision 1.5  2006/04/13 11:54:07  cheolkyoo.kim
 *	no message
 *
 *	Revision 1.4  2006/04/10 00:55:21  inhosens.lee
 *	updated with fimg_ra and fimg_tu codes
 *
 *	Revision 1.3  2006/04/05 04:06:09  cheolkyoo.kim
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
#include "TexFmt3D.vsa.h"
#include "TexFmt3D.psa.h"

/****************************************************************************
 *  DEFINES
 ****************************************************************************/

int TexFmt3D8888(void)

{
	unsigned int uDataSize;
	const unsigned int *pVS;
	const unsigned int *pPS;
	//unsigned int ConstFloatAddr;
	FGL_TexUnitParams tuParams;

	float attribute[] =
	{
		/* x       y      z      w */
		-2.0f,  2.0f,  0.0f,  1.0f,
		 2.0f,  2.0f,  0.0f,  1.0f,
		-2.0f, -2.0f,  0.0f,  1.0f,
		 2.0f, -2.0f,  0.0f,  1.0f
	};

	Matrix4 matMVP, matMVI;
	matMVP.SetMatrix4(1.00000f, 0.000000f, 0.000000f, 0.000000f,
				0.000000f, 1.00000f, 0.000000f, 0.000000f,
				0.000000f, 0.000000f, -1.00020f, 2.80058f,
				0.000000f, 0.000000f, -1.00000f, 3.00000f);
	matMVI.SetMatrix4(1.0f, 0.0f, 0.0f,  0.0f,
				0.0f, 1.0f, 0.0f,  0.0f,
				0.0f, 0.0f, 1.0f, -3.0f,
				0.0f, 0.0f, 0.0f,  1.0f);
	
	pVS = &TexFmt3D_vsa[0];
	pPS = &TexFmt3D_psa[0];

    if (fglLoadVShader(pVS) != FGL_ERR_NO_ERROR) {
		return ERROR;
    }

    if (fglLoadPShader(pPS) != FGL_ERR_NO_ERROR) {
		return ERROR;
    }
	
	//ConstFloatAddr = FGVS_CFLOAT_SADDR;
	//ConstFloatAddr = matMVP.WriteMatrix4(ConstFloatAddr);
  	//matMVI.WriteMatrix4(ConstFloatAddr, FGL_TRUE);
  	
    unsigned int Offset = fglWriteVertexShaderConstFloat(0, 16, matMVP.m[0]);
    Offset = fglWriteVertexShaderConstFloat(Offset, 16, matMVI.m[0]);
  	

	drvsys_clear_buf((unsigned int*)(FIMG_COLOR_BUFFER), CLEAR_SCREEN_SIZE, 0x0);


	fglFlush(FGL_PIPESTATE_ALL);	// Pipeline status


/*
	// Vertex Shader SFR set
	WRITEREG(FGVS_ATTRIB_NUM, 0x00030001); // Attribute counter of VS input & output
	WRITEREG(FGVS_CONFIG, 0x00000001); // Configuration register
	WRITEREG(FGVS_OUT_ATTRIB_IDX0, 0x03020100); // Output attribute index control

	// Pixel Shader SFR set
	WRITEREG(FGPS_EXE_MODE, 0x00000000); // Pixel shader execution mode control register
	WRITEREG(FGPS_PC_COPY, 0x00000001); // Copy PSPCS_ADDR value to program counter
	WRITEREG(FGPS_ATTRIB_NUM, 0x00000002); // Number of attribute of current context
	fglSysPollingMask(FGPS_INBUF_STATUS, 0x0, 0x00000001); // Status signal of pixel shader input buffer
	WRITEREG(FGPS_EXE_MODE, 0x00000001); // Pixel shader execution mode control register
*/
    fglVSSetAttribNum(1);
    fglPSSetAttributeNum(2);

    // Texture format rendering test main
    {

		// Primitive Engine SRF Set
		FGL_Vertex Vtx;
		Vtx.prim = FGL_PRIM_TRIANGLE_STRIP;
		Vtx.enablePointSize = FGL_FALSE;
		Vtx.numVSOut = 2;
		Vtx.shadeModel = FGL_SHADING_SMOOTH;
		Vtx.colorAttribIdx = 0;
		
		fglSetVertex(&Vtx); // Vertex context register
		// Raster SFR set
		//FGL_DPFAPI( fglSetLODControl(0x0000007) );
		fglSetLODRegister(FGL_LODCOEFF_ENABLE_ALL,
					FGL_LODCOEFF_DISABLE,
					FGL_LODCOEFF_DISABLE,
					FGL_LODCOEFF_DISABLE,
					FGL_LODCOEFF_DISABLE,
					FGL_LODCOEFF_DISABLE,
					FGL_LODCOEFF_DISABLE,
					FGL_LODCOEFF_DISABLE);
//		WRITEREG(FGRA_LOD_CTRL, 0x00000007); // Level of detail attribute1 set

#ifdef _FIMG3DSE_VER_1_2
    	// Texture Unit SFR Setting
    	tuParams.eType = FGL_TEX_3D;
    	tuParams.eColorkey = FGL_CKEY_DISABLE;
    	tuParams.bUseExpansion = FGL_FALSE;
    	tuParams.ePaletteFormat = FGL_PALETTE_ARGB8888;
    	tuParams.eFormat = FGL_TEXEL_ARGB8888;
    	tuParams.eUMode = FGL_TEX_WRAP_CLAMP;
    	tuParams.eVMode = FGL_TEX_WRAP_CLAMP;
    	tuParams.bIsNonparametric = FGL_FALSE;
    	tuParams.bUseMagFilter = FGL_TRUE;
    	tuParams.bUseMinFilter = FGL_TRUE;
       	tuParams.eMipMapFilter = FGL_FILTER_DISABLE;
    	tuParams.uUSize = 128;
    	tuParams.uVSize = 128;
    	tuParams.uPSize = 1;
    	
    	fglSetTexUnitParams(0, &tuParams);
    	fglSetTexBaseAddr(0, FIMG_TEXTURE_MEMORY);
#else
		WRITEREG(FGTU_TEX0_CTRL, 0x18106A77); 	// FIMG_TUSFR_TEX0_STATUS
		WRITEREG(FGTU_TEX0_BASE_LEVEL, 0x00000000); 	// FIMG_TUSFR_TEX0_BASE_LEVEL
		WRITEREG(FGTU_TEX0_MAX_LEVEL, 0x00000000); 	// FIMG_TUSFR_TEX0_MAX_LEVEL
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
		HIAttr.bEndFlag = FGL_TRUE;
		HIAttr.type = FGL_ATTRIB_DATA_FLOAT;
		HIAttr.numComp = 4;
		HIAttr.srcW = FGL_ATTRIB_ORDER_4TH;
		HIAttr.srcZ = FGL_ATTRIB_ORDER_3RD;
		HIAttr.srcY = FGL_ATTRIB_ORDER_2ND;
		HIAttr.srcX = FGL_ATTRIB_ORDER_1ST;	
		fglSetAttribute(0, &HIAttr);
		
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

		fglSysDelay(100);

//		drvsys_clear_buf((unsigned int*)(FIMG_COLOR_BUFFER+m_FBOffsize), CLEAR_SCREEN_SIZE, 0x0);

    }
	fglSetLODControl(0x00000000);


    return NO_ERROR;
}


