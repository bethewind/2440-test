/*******************************************************************************
 *
 *	ViP model for FIMGSE
 *		version 1.0
 *
 *	Lighting.c
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
 *  Description -
 *    Minimum primitive engine diagnostic tests:
 *
 *	$RCSfile: Lighting.cpp,v $
 *	$Revision: 1.6 $
 *	$Author: cheolkyoo.kim $
 *	$Date: 2006/05/08 05:34:28 $
 *	$Locker:  $
 *
 *	$Source: C:/CVS/CVSrepository/FIMG-3DSE_SW/fimg3dse_fpga/fimg3d/src/Lighting.cpp,v $
 *	$State: Exp $
 *	$Log: Lighting.cpp,v $
 *	Revision 1.6  2006/05/08 05:34:28  cheolkyoo.kim
 *	no message
 *	
 *	Revision 1.5  2006/04/13 11:54:26  cheolkyoo.kim
 *	no message
 *
 *	Revision 1.4  2006/04/08 03:52:10  inhosens.lee
 *	updated with fimg_ra code
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


#include "Lighting.vsa.h"
#include "mov_v2o.psa.h"

/****************************************************************************
 *  DEFINES
 ****************************************************************************/
//static unsigned int nNumTrisCube1 = 12;
//static unsigned int nNumAttributesCube1 = 12;

int Lighting(void)

{
	unsigned int nNumOfVertices;
	unsigned int nNumofData;
	//unsigned int ConstFloatAddr;
	unsigned int nNumTrisCube1 = 12;
	unsigned int nNumAttributesCube1 = 12;

	float Cube_Data[] =
	{
		//  x      y      z      w      r      g      b      a      nx     ny     nz
		// FRONT
		-0.2f, -0.2f,  0.2f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		 0.2f, -0.2f,  0.2f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		-0.2f,  0.2f,  0.2f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		 0.2f, -0.2f,  0.2f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		-0.2f,  0.2f,  0.2f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		 0.2f,  0.2f,  0.2f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		// BACK
		-0.2f, -0.2f, -0.2f,  1.0f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,  0.0f, -1.0f,  1.0f,
		-0.2f,  0.2f, -0.2f,  1.0f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,  0.0f, -1.0f,  1.0f,
		 0.2f, -0.2f, -0.2f,  1.0f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,  0.0f, -1.0f,  1.0f,
		-0.2f,  0.2f, -0.2f,  1.0f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,  0.0f, -1.0f,  1.0f,
		 0.2f, -0.2f, -0.2f,  1.0f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,  0.0f, -1.0f,  1.0f,
		 0.2f,  0.2f, -0.2f,  1.0f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,  0.0f, -1.0f,  1.0f,
		// LEFT
		-0.2f, -0.2f,  0.2f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,
		-0.2f,  0.2f,  0.2f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,
		-0.2f, -0.2f, -0.2f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,
		-0.2f,  0.2f,  0.2f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,
		-0.2f, -0.2f, -0.2f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,
		-0.2f,  0.2f, -0.2f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,
		// RIGHT
		 0.2f, -0.2f, -0.2f,  1.0f,  1.0f,  1.0f,  0.0f,  1.0f, -1.0f,  0.0f,  0.0f,  1.0f,
		 0.2f,  0.2f, -0.2f,  1.0f,  1.0f,  1.0f,  0.0f,  1.0f, -1.0f,  0.0f,  0.0f,  1.0f,
		 0.2f, -0.2f,  0.2f,  1.0f,  1.0f,  1.0f,  0.0f,  1.0f, -1.0f,  0.0f,  0.0f,  1.0f,
		 0.2f,  0.2f, -0.2f,  1.0f,  1.0f,  1.0f,  0.0f,  1.0f, -1.0f,  0.0f,  0.0f,  1.0f,
		 0.2f, -0.2f,  0.2f,  1.0f,  1.0f,  1.0f,  0.0f,  1.0f, -1.0f,  0.0f,  0.0f,  1.0f,
		 0.2f,  0.2f,  0.2f,  1.0f,  1.0f,  1.0f,  0.0f,  1.0f, -1.0f,  0.0f,  0.0f,  1.0f,
		// TOP
		-0.2f,  0.2f,  0.2f,  1.0f,  1.0f,  0.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f,  1.0f,
		 0.2f,  0.2f,  0.2f,  1.0f,  1.0f,  0.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f,  1.0f,
		-0.2f,  0.2f, -0.2f,  1.0f,  1.0f,  0.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f,  1.0f,
		 0.2f,  0.2f,  0.2f,  1.0f,  1.0f,  0.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f,  1.0f,
		-0.2f,  0.2f, -0.2f,  1.0f,  1.0f,  0.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f,  1.0f,
		 0.2f,  0.2f, -0.2f,  1.0f,  1.0f,  0.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f,  1.0f,
		// BOTTOM
		-0.2f, -0.2f,  0.2f,  1.0f,  0.0f,  1.0f,  1.0f,  1.0f,  0.0f, -1.0f,  0.0f,  1.0f,
		-0.2f, -0.2f, -0.2f,  1.0f,  0.0f,  1.0f,  1.0f,  1.0f,  0.0f, -1.0f,  0.0f,  1.0f,
		 0.2f, -0.2f,  0.2f,  1.0f,  0.0f,  1.0f,  1.0f,  1.0f,  0.0f, -1.0f,  0.0f,  1.0f,
		-0.2f, -0.2f, -0.2f,  1.0f,  0.0f,  1.0f,  1.0f,  1.0f,  0.0f, -1.0f,  0.0f,  1.0f,
		 0.2f, -0.2f,  0.2f,  1.0f,  0.0f,  1.0f,  1.0f,  1.0f,  0.0f, -1.0f,  0.0f,  1.0f,
		 0.2f, -0.2f, -0.2f,  1.0f,  0.0f,  1.0f,  1.0f,  1.0f,  0.0f, -1.0f,  0.0f,  1.0f
	};

	Matrix4 matMVP(1.707107f,  0.0f,       1.707107f, 0.0f,
			1.207107f,  1.707107f, -1.207107f, 0.0f,
			0.501001f, -0.708522f, -0.501001f, 1.302803f,
			0.5f,      -0.707107f, -0.5f,      1.5f);

	if (fglLoadVShader(Lighting_vsa) != FGL_ERR_NO_ERROR)
	{
		return ERROR;
	}

	if (fglLoadPShader(mov_v2o_psa) != FGL_ERR_NO_ERROR)
	{
		return ERROR;
	}

	
	fglWriteVertexShaderConstFloat(0, 16, matMVP.m[0]);

	drvsys_clear_buf((unsigned int*)FIMG_DEPTH_BUFFER, CLEAR_SCREEN_SIZE, 0x00FFFFFF);
	drvsys_clear_buf((unsigned int*)(FIMG_COLOR_BUFFER), CLEAR_SCREEN_SIZE, 0x0);


	fglFlush(FGL_PIPESTATE_ALL);	// Pipeline status


/*
	// Vertex Shader SFR set
	WRITEREG(FGVS_ATTRIB_NUM, 0x00030003); // Attribute counter of VS input & output
	WRITEREG(FGVS_CONFIG, 0x00000001); // Configuration register

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

    fglVSSetAttribNum(3);
    fglPSSetAttributeNum(1);

	// Per-fragment Unit SFR set
	//WRITEREG(FGPF_DEPTHBUF_ADDR, FIMG_DEPTH_BUFFER); // Depth/Stencil buffer base address
	//WRITEREG(FGPF_DEPTH, 0x00000005); // Depth test control
	// Depth/Stencil buffer base address
	fglSetZBufBaseAddr(FIMG_DEPTH_BUFFER);
	fglSetDepthParams(FGL_COMP_LESS);
	fglEnablePerFragUnit(FGL_PF_DEPTH, FGL_TRUE);

	// Primitive Engine SRF Set
	FGL_Vertex Vtx;
	Vtx.prim = FGL_PRIM_TRIANGLES;
	Vtx.enablePointSize = FGL_FALSE;
	Vtx.numVSOut = 1;
	Vtx.shadeModel = FGL_SHADING_SMOOTH;
	Vtx.colorAttribIdx = 0;
	
	fglSetVertex(&Vtx); // Vertex context register

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
	HIAttr.numComp = 4;
	HIAttr.srcW = FGL_ATTRIB_ORDER_4TH;
	HIAttr.srcZ = FGL_ATTRIB_ORDER_3RD;
	HIAttr.srcY = FGL_ATTRIB_ORDER_2ND;
	HIAttr.srcX = FGL_ATTRIB_ORDER_1ST;	
	fglSetAttribute(0, &HIAttr);
	fglSetAttribute(1, &HIAttr);
	HIAttr.bEndFlag = FGL_TRUE;
	fglSetAttribute(2, &HIAttr);

	nNumOfVertices = nNumTrisCube1 * 3;
	nNumofData = nNumAttributesCube1 * nNumOfVertices;

	unsigned int uiDummy = 0xFFFFFFFF;
	fglSendToFIFO(4, &nNumOfVertices);
	fglSendToFIFO(4, &uiDummy);

	fglSysTransferToPort(
							(unsigned int *)Cube_Data,
							/*(volatile unsigned int *)(FGHI_FIFO_ENTRY),*/
							nNumofData
						 );

	fglFlush(FGL_PIPESTATE_ALL);	// Pipeline status

	// Cache flush
	//fglSysCacheFlush();
	fglClearCache(FGL_CACHECTL_FLUSH_ZCACHE | FGL_CACHECTL_FLUSH_CCACHE);


	//fglSysDelay(50);

		drvsys_clear_buf((unsigned int*)FIMG_DEPTH_BUFFER, CLEAR_SCREEN_SIZE, 0);
//		drvsys_clear_buf((unsigned int*)(FIMG_COLOR_BUFFER), CLEAR_SCREEN_SIZE, 0);


	fglEnablePerFragUnit(FGL_PF_DEPTH, FGL_FALSE);
	return NO_ERROR;
}
