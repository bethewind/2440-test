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
 *
 *	$RCSfile: VBAutoIncre.cpp,v $
 *	$Revision: 1.6 $
 *	$Author: cheolkyoo.kim $
 *	$Date: 2006/05/08 05:37:06 $
 *	$Locker:  $
 *
 *	$Source: C:/CVS/CVSrepository/FIMG-3DSE_SW/fimg3dse_fpga/fimg3d/src/VBAutoIncre.cpp,v $
 *	$State: Exp $
 *	$Log: VBAutoIncre.cpp,v $
 *	Revision 1.6  2006/05/08 05:37:06  cheolkyoo.kim
 *	no message
 *	
 *	Revision 1.5  2006/04/13 11:53:57  cheolkyoo.kim
 *	no message
 *
 *	Revision 1.4  2006/04/08 07:12:28  inhosens.lee
 *	updated with fimg_ra code
 *
 *	Revision 1.3  2006/04/05 04:06:11  cheolkyoo.kim
 *	Replaced ~_XCORD and ~_YCORD to ~_XCOORD and ~_YCOORD.
 *
 *	Revision 1.2  2006/03/30 04:27:46  cheolkyoo.kim
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



#include "Lighting.vsa.h"
#include "mov_v2o.psa.h"

/****************************************************************************
 *  DEFINES
 ****************************************************************************/

int VBAutoIncre(void)

{
    unsigned int nNumTrisCube_HI0 = 12;
	unsigned int i;
	unsigned int j;
	//const unsigned int *pVS;
	//const unsigned int *pPS;
	unsigned int nNumOfVertices;
	unsigned int *pVtxBufAddr;
	unsigned int DataSize;
	//unsigned int ConstFloatAddr;

	float CubePos[] =
	{
		//  x      y      z
		// FRONT
		-0.2f, -0.2f,  0.2f,
		 0.2f, -0.2f,  0.2f,
		-0.2f,  0.2f,  0.2f,
		 0.2f, -0.2f,  0.2f,
		-0.2f,  0.2f,  0.2f,
		 0.2f,  0.2f,  0.2f,
		// BACK
		-0.2f, -0.2f, -0.2f,
		-0.2f,  0.2f, -0.2f,
		 0.2f, -0.2f, -0.2f,
		-0.2f,  0.2f, -0.2f,
		 0.2f, -0.2f, -0.2f,
		 0.2f,  0.2f, -0.2f,
		// LEFT
		-0.2f, -0.2f,  0.2f,
		-0.2f,  0.2f,  0.2f,
		-0.2f, -0.2f, -0.2f,
		-0.2f,  0.2f,  0.2f,
		-0.2f, -0.2f, -0.2f,
		-0.2f,  0.2f, -0.2f,
		// RIGHT
		 0.2f, -0.2f, -0.2f,
		 0.2f,  0.2f, -0.2f,
		 0.2f, -0.2f,  0.2f,
		 0.2f,  0.2f, -0.2f,
		 0.2f, -0.2f,  0.2f,
		 0.2f,  0.2f,  0.2f,
		// TOP
		-0.2f,  0.2f,  0.2f,
		 0.2f,  0.2f,  0.2f,
		-0.2f,  0.2f, -0.2f,
		 0.2f,  0.2f,  0.2f,
		-0.2f,  0.2f, -0.2f,
		 0.2f,  0.2f, -0.2f,
		// BOTTOM
		-0.2f, -0.2f,  0.2f,
		-0.2f, -0.2f, -0.2f,
		 0.2f, -0.2f,  0.2f,
		-0.2f, -0.2f, -0.2f,
		 0.2f, -0.2f,  0.2f,
		 0.2f, -0.2f, -0.2f
	};

	float CubeColor[] =
	{
		// r      g      b
		// FRONT
		1.0f,  0.0f,  0.0f,
		1.0f,  0.0f,  0.0f,
		1.0f,  0.0f,  0.0f,
		1.0f,  0.0f,  0.0f,
		1.0f,  0.0f,  0.0f,
		1.0f,  0.0f,  0.0f,
		// BACK
		0.0f,  1.0f,  0.0f,
		0.0f,  1.0f,  0.0f,
		0.0f,  1.0f,  0.0f,
		0.0f,  1.0f,  0.0f,
		0.0f,  1.0f,  0.0f,
		0.0f,  1.0f,  0.0f,
		// LEFT
		0.0f,  0.0f,  1.0f,
		0.0f,  0.0f,  1.0f,
		0.0f,  0.0f,  1.0f,
		0.0f,  0.0f,  1.0f,
		0.0f,  0.0f,  1.0f,
		0.0f,  0.0f,  1.0f,
		// RIGHT
		1.0f,  1.0f,  0.0f,
		1.0f,  1.0f,  0.0f,
		1.0f,  1.0f,  0.0f,
		1.0f,  1.0f,  0.0f,
		1.0f,  1.0f,  0.0f,
		1.0f,  1.0f,  0.0f,
		// TOP
		1.0f,  0.0f,  1.0f,
		1.0f,  0.0f,  1.0f,
		1.0f,  0.0f,  1.0f,
		1.0f,  0.0f,  1.0f,
		1.0f,  0.0f,  1.0f,
		1.0f,  0.0f,  1.0f,
		// BOTTOM
		0.0f,  1.0f,  1.0f,
		0.0f,  1.0f,  1.0f,
		0.0f,  1.0f,  1.0f,
		0.0f,  1.0f,  1.0f,
		0.0f,  1.0f,  1.0f,
		0.0f,  1.0f,  1.0f
	};

	float CubeNormal[] =
	{
		// nx     ny     nz
		// FRONT
		 0.0f,  0.0f,  1.0f,
		 0.0f,  0.0f,  1.0f,
		 0.0f,  0.0f,  1.0f,
		 0.0f,  0.0f,  1.0f,
		 0.0f,  0.0f,  1.0f,
		 0.0f,  0.0f,  1.0f,
		// BACK
		 0.0f,  0.0f, -1.0f,
		 0.0f,  0.0f, -1.0f,
		 0.0f,  0.0f, -1.0f,
		 0.0f,  0.0f, -1.0f,
		 0.0f,  0.0f, -1.0f,
		 0.0f,  0.0f, -1.0f,
		// LEFT
		 1.0f,  0.0f,  0.0f,
		 1.0f,  0.0f,  0.0f,
		 1.0f,  0.0f,  0.0f,
		 1.0f,  0.0f,  0.0f,
		 1.0f,  0.0f,  0.0f,
		 1.0f,  0.0f,  0.0f,
		// RIGHT
		-1.0f,  0.0f,  0.0f,
		-1.0f,  0.0f,  0.0f,
		-1.0f,  0.0f,  0.0f,
		-1.0f,  0.0f,  0.0f,
		-1.0f,  0.0f,  0.0f,
		-1.0f,  0.0f,  0.0f,
		// TOP
		 0.0f,  1.0f,  0.0f,
		 0.0f,  1.0f,  0.0f,
		 0.0f,  1.0f,  0.0f,
		 0.0f,  1.0f,  0.0f,
		 0.0f,  1.0f,  0.0f,
		 0.0f,  1.0f,  0.0f,
		// BOTTOM
		 0.0f, -1.0f,  0.0f,
		 0.0f, -1.0f,  0.0f,
		 0.0f, -1.0f,  0.0f,
		 0.0f, -1.0f,  0.0f,
		 0.0f, -1.0f,  0.0f,
		 0.0f, -1.0f,  0.0f
	};

	if (fglLoadVShader(Lighting_vsa) != FGL_ERR_NO_ERROR)
	{
		return ERROR;
	}

	if (fglLoadPShader(mov_v2o_psa) != FGL_ERR_NO_ERROR)
	{
		return ERROR;
	}

	Matrix4 matMVP(1.707107f,  0.0f,       1.707107f, 0.0f,
			1.207107f,  1.707107f, -1.207107f, 0.0f,
			0.501001f, -0.708522f, -0.501001f, 1.302803f,
			0.5f,      -0.707107f, -0.5f,      1.5f);
    
    //ConstFloatAddr = FGVS_CFLOAT_SADDR;
    //matMVP.WriteMatrix4(ConstFloatAddr);
    
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
    fglVSSetAttribNum(3);
    fglPSSetAttributeNum(1);

	// Per-fragment Unit SFR set
	//WRITEREG(FGPF_DEPTHBUF_ADDR, FIMG_DEPTH_BUFFER); // Depth/Stencil buffer base address
	//WRITEREG(FGPF_DEPTH, 0x00000005); // Depth test control
	fglSetZBufBaseAddr(FIMG_DEPTH_BUFFER);
	fglSetDepthParams(FGL_COMP_LESS);
	fglEnablePerFragUnit(FGL_PF_DEPTH, FGL_TRUE);


	/*
	 * Transfer the data of geometry to Vertex Buffer
	 */

	pVtxBufAddr = 0;
	FGL_VtxBufAttrib VtxAttrib;
	
	// Attribute 0 for Vertex Buffer
	fglSetVtxBufferAddr((unsigned int)pVtxBufAddr);		// Vetex buffer address
	VtxAttrib.stride = 16;								// Attribute n vertex buffer control
	VtxAttrib.addr = (unsigned int)pVtxBufAddr;			// Vertex buffer attribute0 base address
	VtxAttrib.num = 0x24;
	fglSetVtxBufAttrib(0, &VtxAttrib);
	
	DataSize = (sizeof(CubePos)/sizeof(float))/3;

	for(i=0; i<DataSize; i++)
	{
		fglSendToVtxBuffer(*(unsigned int*)&CubePos[i*3]);		
		pVtxBufAddr++;
		fglSendToVtxBuffer(*(unsigned int*)&CubePos[i*3+1]);
		pVtxBufAddr++;
		fglSendToVtxBuffer(*(unsigned int*)&CubePos[i*3+2]);
		pVtxBufAddr++;
		fglSendToVtxBuffer(0x3F800000);
		pVtxBufAddr++;
	}

	// Attribute 1 for Vertex Buffer
	fglSetVtxBufferAddr((unsigned int)pVtxBufAddr);	// Vetex buffer address
	VtxAttrib.stride = 16;								// Attribute n vertex buffer control
	VtxAttrib.addr = (unsigned int)pVtxBufAddr;			// Vertex buffer attribute0 base address
	VtxAttrib.num = 0x24;								// Attribute n vertex buffer control
	fglSetVtxBufAttrib(1, &VtxAttrib);

	for(i=0; i<DataSize; i++)
	{
		fglSendToVtxBuffer(*(unsigned int*)&CubeColor[i*3]);		
		pVtxBufAddr++;
		fglSendToVtxBuffer(*(unsigned int*)&CubeColor[i*3+1]);
		pVtxBufAddr++;
		fglSendToVtxBuffer(*(unsigned int*)&CubeColor[i*3+2]);
		pVtxBufAddr++;
		fglSendToVtxBuffer(0x3F800000);
		pVtxBufAddr++;
	}

	// Attribute 2 for Vertex Buffer
	fglSetVtxBufferAddr((unsigned int)pVtxBufAddr);	// Vetex buffer address
	VtxAttrib.stride = 16;								// Attribute n vertex buffer control
	VtxAttrib.addr = (unsigned int)pVtxBufAddr;			// Vertex buffer attribute0 base address
	VtxAttrib.num = 0x24;								// Attribute n vertex buffer control
	fglSetVtxBufAttrib(2, &VtxAttrib);

	for(i=0; i<DataSize; i++)
	{
		fglSendToVtxBuffer(*(unsigned int*)&CubeNormal[i*3]);		
		pVtxBufAddr++;
		fglSendToVtxBuffer(*(unsigned int*)&CubeNormal[i*3+1]);
		pVtxBufAddr++;
		fglSendToVtxBuffer(*(unsigned int*)&CubeNormal[i*3+2]);
		pVtxBufAddr++;
		fglSendToVtxBuffer(0x3F800000);
		pVtxBufAddr++;
	}


	// Question
	// HI IdxOffset SFR
	fglSetIndexOffset(1);		    // Auto-Increment Mode

	// Primitive Engine SRF Set
	FGL_Vertex Vtx;
	Vtx.prim = FGL_PRIM_TRIANGLES;
	Vtx.enablePointSize = FGL_FALSE;
	Vtx.numVSOut = 1;
	Vtx.shadeModel = FGL_SHADING_SMOOTH;
	Vtx.colorAttribIdx = 0;
	
	fglSetVertex(&Vtx); // Vertex context register

	// Host Interface SFR Set
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

	FGL_HInterface HInterface;
	HInterface.enableAutoInc = FGL_TRUE;
	HInterface.enableVtxBuffer = FGL_TRUE;
	HInterface.enableVtxCache = FGL_FALSE;	
	HInterface.idxType = FGL_INDEX_DATA_UINT;
	HInterface.numVSOut = 2;
	
	for(j=0; j<3; j++)
	{
		nNumOfVertices = nNumTrisCube_HI0 * 3;

		if(j==0)
		{
			// Auto-Increment Mode: index type unsigned int
			HInterface.idxType = FGL_INDEX_DATA_UINT;
			fglSetHInterface(&HInterface);
			
			unsigned int uiIndex = 0x00000000;
			fglSendToFIFO(4, &nNumOfVertices);
			fglSendToFIFO(4, &uiIndex);
		}
		else if(j==1)
		{
			// Auto-Increment Mode: index type short
			HInterface.idxType = FGL_INDEX_DATA_USHORT;
			fglSetHInterface(&HInterface);
			
			unsigned int uiIndex = 0xFFFF0000;
			fglSendToFIFO(4, &nNumOfVertices);
			fglSendToFIFO(4, &uiIndex);
		}
		else
		{
			// Auto-Increment Mode: index type unsigned byte
			HInterface.idxType = FGL_INDEX_DATA_UBYTE;
			HInterface.enableVtxCache = FGL_TRUE;
			fglSetHInterface(&HInterface);
			
			unsigned int uiIndex = 0xFFFFFF00;
			fglSendToFIFO(4, &nNumOfVertices);
			fglSendToFIFO(4, &uiIndex);
		}

		fglFlush(FGL_PIPESTATE_ALL);	// Pipeline status

		// Cache flush
		//fglSysCacheFlush();
		fglClearCache(FGL_CACHECTL_FLUSH_ZCACHE | FGL_CACHECTL_FLUSH_CCACHE);


		fglSysDelay(50);

		drvsys_clear_buf((unsigned int*)FIMG_DEPTH_BUFFER, CLEAR_SCREEN_SIZE, 0x00FFFFFF);
//		drvsys_clear_buf((unsigned int*)(FIMG_COLOR_BUFFER+m_FBOffsize), CLEAR_SCREEN_SIZE, 0);


	}

	//WRITEREG(FGPF_DEPTH, 0);
    fglEnablePerFragUnit(FGL_PF_DEPTH, FGL_FALSE);

	return NO_ERROR;
}
