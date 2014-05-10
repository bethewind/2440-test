/*******************************************************************************
 *
 *	ViP model for FIMGSE
 *		version 1.0
 *
 *	MipmapFmt8888.c
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
 *	$RCSfile: MipmapFmt8888.cpp,v $
 *	$Revision: 1.6 $
 *	$Author: cheolkyoo.kim $
 *	$Date: 2006/05/08 05:34:28 $
 *	$Locker:  $
 *
 *	$Source: C:/CVS/CVSrepository/FIMG-3DSE_SW/fimg3dse_fpga/fimg3d/src/MipmapFmt8888.cpp,v $
 *	$State: Exp $
 *	$Log: MipmapFmt8888.cpp,v $
 *	Revision 1.6  2006/05/08 05:34:28  cheolkyoo.kim
 *	no message
 *	
 *	Revision 1.5  2006/04/13 11:54:16  cheolkyoo.kim
 *	no message
 *
 *	Revision 1.4  2006/04/10 00:55:21  inhosens.lee
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



//#include "Debug.h"

#include "Fimg3DTest.h"


#include "tex_bypass_vsa.h"
#include "tex_bypass_psa.h"


int MipmapFmt8888(void)

{
	unsigned int uDataSize;
	FGL_TexUnitParams tuParams;

	float object0[] = {
		-0.9f,  0.1f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.7f,  0.1f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
		-0.9f, -0.1f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
		-0.7f, -0.1f,  0.5f,  1.0f,	 0.0f,  0.0f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,  1.0f
	};

	float object1[] = {
		-0.55f,  0.05f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.45f,  0.05f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
		-0.55f, -0.05f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
		-0.45f, -0.05f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,  1.0f
	};

	float object2[] = {
		-0.225f,  0.025f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.175f,  0.025f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
		-0.225f, -0.025f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
		-0.175f, -0.025f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,  1.0f
	};


	float object3[] = {
		0.0875f,  0.0125f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		0.1125f,  0.0125f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
		0.0875f, -0.0125f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
		0.1125f, -0.0125f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,  1.0f
	};

	float object4[] = {
		0.39375f,  0.00625f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		0.40625f,  0.00625f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
		0.39375f, -0.00625f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
		0.40625f, -0.00625f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,  1.0f
	};

	float object5[] = {
		0.699999f,  0.000001f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		0.700001f,  0.000001f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
		0.699999f, -0.000001f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
		0.700001f, -0.000001f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,  1.0f
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
		//FGL_DPFAPI( fglSetLODControl(0x00000038) );
		fglSetLODRegister(FGL_LODCOEFF_DISABLE,
						FGL_LODCOEFF_ENABLE_ALL,
						FGL_LODCOEFF_DISABLE,
						FGL_LODCOEFF_DISABLE,
						FGL_LODCOEFF_DISABLE,
						FGL_LODCOEFF_DISABLE,
						FGL_LODCOEFF_DISABLE,
						FGL_LODCOEFF_DISABLE);

//		WRITEREG(FGRA_LOD_CTRL, 0x00000038); // Level of detail attribute1 set

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
    	tuParams.eMipMapFilter = FGL_FILTER_NEAREST;
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
		HIAttr.bEndFlag = FGL_TRUE;
		fglSetAttribute(2, &HIAttr);
		
		unsigned int uiTmpVertices = 4;		
		unsigned int uiDummy = 0xFFFFFFFF;
		fglSendToFIFO(4, &uiTmpVertices);
		fglSendToFIFO(4, &uiDummy);

		uDataSize = sizeof(object0)/sizeof(object0[0]);

		fglSysTransferToPort(
								(unsigned int *)object0,
								/*(volatile unsigned int *)(FGHI_FIFO_ENTRY),*/
								uDataSize
							 );


		fglSendToFIFO(4, &uiTmpVertices);
		fglSendToFIFO(4, &uiDummy);

		uDataSize = sizeof(object1)/sizeof(object1[0]);

		fglSysTransferToPort(
								(unsigned int *)object1,
								/*(volatile unsigned int *)(FGHI_FIFO_ENTRY),*/
								uDataSize
							 );


		fglSendToFIFO(4, &uiTmpVertices);
		fglSendToFIFO(4, &uiDummy);

		uDataSize = sizeof(object2)/sizeof(object2[0]);

		fglSysTransferToPort(
								(unsigned int *)object2,
								/*(volatile unsigned int *)(FGHI_FIFO_ENTRY),*/
								uDataSize
							 );


		fglSendToFIFO(4, &uiTmpVertices);
		fglSendToFIFO(4, &uiDummy);

		uDataSize = sizeof(object3)/sizeof(object3[0]);

		fglSysTransferToPort(
								(unsigned int *)object3,
								/*(volatile unsigned int *)(FGHI_FIFO_ENTRY),*/
								uDataSize
							 );


		fglSendToFIFO(4, &uiTmpVertices);
		fglSendToFIFO(4, &uiDummy);

		uDataSize = sizeof(object4)/sizeof(object4[0]);

		fglSysTransferToPort(
								(unsigned int *)object4,
								/*(volatile unsigned int *)(FGHI_FIFO_ENTRY),*/
								uDataSize
							 );

		fglSendToFIFO(4, &uiTmpVertices);
		fglSendToFIFO(4, &uiDummy);

		uDataSize = sizeof(object5)/sizeof(object5[0]);

		fglSysTransferToPort(
								(unsigned int *)object5,
								/*(volatile unsigned int *)(FGHI_FIFO_ENTRY),*/
								uDataSize
							 );

		fglFlush(FGL_PIPESTATE_ALL);	// Pipeline status

		// Cache flush
		//fglSysCacheFlush();
		//fglSysCacheClear();
		fglClearCache(FGL_CACHECTL_INIT_ALL);

		//fglSysDelay(50);

//		drvsys_clear_buf((unsigned int*)(FIMG_COLOR_BUFFER), CLEAR_SCREEN_SIZE, 0x0);


    }
    
	//const char* pFileName_SFR_Dump = "SFR_Mipmap.txt";
	//fglDumpContext(pFileName_SFR_Dump);

	fglSetLODControl(0x0);
	

    return NO_ERROR;
}

