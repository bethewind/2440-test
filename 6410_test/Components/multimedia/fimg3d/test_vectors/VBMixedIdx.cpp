/*******************************************************************************
 *
 *	ViP model for FIMGSE
 *		version 1.0
 *
 *	VBMixedIdx.c
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
 *	2006. 2. 7  by cheolkyoo.kim
 *
 *  Description -
 *
 *	$RCSfile: VBMixedIdx.cpp,v $
 *	$Revision: 1.7 $
 *	$Author: cheolkyoo.kim $
 *	$Date: 2006/05/08 05:37:06 $
 *	$Locker:  $
 *
 *	$Source: C:/CVS/CVSrepository/FIMG-3DSE_SW/fimg3dse_fpga/fimg3d/src/VBMixedIdx.cpp,v $
 *	$State: Exp $
 *	$Log: VBMixedIdx.cpp,v $
 *	Revision 1.7  2006/05/08 05:37:06  cheolkyoo.kim
 *	no message
 *	
 *	Revision 1.6  2006/04/13 11:53:57  cheolkyoo.kim
 *	no message
 *
 *	Revision 1.5  2006/04/08 07:12:28  inhosens.lee
 *	updated with fimg_ra code
 *
 *	Revision 1.4  2006/04/05 04:06:12  cheolkyoo.kim
 *	Replaced ~_XCORD and ~_YCORD to ~_XCOORD and ~_YCOORD.
 *
 *	Revision 1.3  2006/03/30 04:27:46  cheolkyoo.kim
 *	Thomas-20060330@comment: Add to code using directive WIN32_VIP for VIP
 *
 *	Revision 1.2  2006/03/30 03:25:16  cheolkyoo.kim
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
#include "Diffuse.vsa.h"

/****************************************************************************
 *  DEFINES
 ****************************************************************************/
//static unsigned int nNumTrisCube_HI2 = 12;

int VBMixedIdx(void)

{
	unsigned int i;
	unsigned int j;
	unsigned int nNumOfVertices;
	unsigned int *pVtxBufAddr;
	unsigned int DataSize;
	unsigned int IdxSize;
	//unsigned int ConstFloatAddr;
	unsigned int nEmptySpace;
    unsigned int nNumTrisCube_HI2 = 12;
	
	float CubePos[] =
	{
		//  x      y      z
		// FRONT
		-0.2f, -0.2f,  0.2f,  1.0f,
		 0.2f, -0.2f,  0.2f,  1.0f,
		-0.2f,  0.2f,  0.2f,  1.0f,
		 0.2f, -0.2f,  0.2f,  1.0f,
		-0.2f,  0.2f,  0.2f,  1.0f,
		 0.2f,  0.2f,  0.2f,  1.0f,
		// BACK
		-0.2f, -0.2f, -0.2f,  1.0f,
		-0.2f,  0.2f, -0.2f,  1.0f,
		 0.2f, -0.2f, -0.2f,  1.0f,
		-0.2f,  0.2f, -0.2f,  1.0f,
		 0.2f, -0.2f, -0.2f,  1.0f,
		 0.2f,  0.2f, -0.2f,  1.0f,
		// LEFT
		-0.2f, -0.2f,  0.2f,  1.0f,
		-0.2f,  0.2f,  0.2f,  1.0f,
		-0.2f, -0.2f, -0.2f,  1.0f,
		-0.2f,  0.2f,  0.2f,  1.0f,
		-0.2f, -0.2f, -0.2f,  1.0f,
		-0.2f,  0.2f, -0.2f,  1.0f,
		// RIGHT
		 0.2f, -0.2f, -0.2f,  1.0f,
		 0.2f,  0.2f, -0.2f,  1.0f,
		 0.2f, -0.2f,  0.2f,  1.0f,
		 0.2f,  0.2f, -0.2f,  1.0f,
		 0.2f, -0.2f,  0.2f,  1.0f,
		 0.2f,  0.2f,  0.2f,  1.0f,
	};

	float CubeColor[] =
	{
		// r      g      b
		// FRONT
		1.0f,  0.0f,  0.0f,  1.0f,
		1.0f,  0.0f,  0.0f,  1.0f,
		1.0f,  0.0f,  0.0f,  1.0f,
		1.0f,  0.0f,  0.0f,  1.0f,
		1.0f,  0.0f,  0.0f,  1.0f,
		1.0f,  0.0f,  0.0f,  1.0f,
		// BACK
		0.0f,  1.0f,  0.0f,  1.0f,
		0.0f,  1.0f,  0.0f,  1.0f,
		0.0f,  1.0f,  0.0f,  1.0f,
		0.0f,  1.0f,  0.0f,  1.0f,
		0.0f,  1.0f,  0.0f,  1.0f,
		0.0f,  1.0f,  0.0f,  1.0f,
		// LEFT
		0.0f,  0.0f,  1.0f,  1.0f,
		0.0f,  0.0f,  1.0f,  1.0f,
		0.0f,  0.0f,  1.0f,  1.0f,
		0.0f,  0.0f,  1.0f,  1.0f,
		0.0f,  0.0f,  1.0f,  1.0f,
		0.0f,  0.0f,  1.0f,  1.0f,
		// RIGHT
		1.0f,  1.0f,  0.0f,  1.0f,
		1.0f,  1.0f,  0.0f,  1.0f,
		1.0f,  1.0f,  0.0f,  1.0f,
		1.0f,  1.0f,  0.0f,  1.0f,
		1.0f,  1.0f,  0.0f,  1.0f,
		1.0f,  1.0f,  0.0f,  1.0f,
	};

	float CubeNormal[] =
	{
		// nx     ny     nz
		// FRONT
		 0.0f,  0.0f,  1.0f,  1.0f,
		 0.0f,  0.0f,  1.0f,  1.0f,
		 0.0f,  0.0f,  1.0f,  1.0f,
		 0.0f,  0.0f,  1.0f,  1.0f,
		 0.0f,  0.0f,  1.0f,  1.0f,
		 0.0f,  0.0f,  1.0f,  1.0f,
		// BACK
		 0.0f,  0.0f, -1.0f,  1.0f,
		 0.0f,  0.0f, -1.0f,  1.0f,
		 0.0f,  0.0f, -1.0f,  1.0f,
		 0.0f,  0.0f, -1.0f,  1.0f,
		 0.0f,  0.0f, -1.0f,  1.0f,
		 0.0f,  0.0f, -1.0f,  1.0f,
		// LEFT
		 1.0f,  0.0f,  0.0f,  1.0f,
		 1.0f,  0.0f,  0.0f,  1.0f,
		 1.0f,  0.0f,  0.0f,  1.0f,
		 1.0f,  0.0f,  0.0f,  1.0f,
		 1.0f,  0.0f,  0.0f,  1.0f,
		 1.0f,  0.0f,  0.0f,  1.0f,
		// RIGHT
		-1.0f,  0.0f,  0.0f,  1.0f,
		-1.0f,  0.0f,  0.0f,  1.0f,
		-1.0f,  0.0f,  0.0f,  1.0f,
		-1.0f,  0.0f,  0.0f,  1.0f,
		-1.0f,  0.0f,  0.0f,  1.0f,
		-1.0f,  0.0f,  0.0f,  1.0f,
	};

	float RemainData0[] =
	{
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

	// #24
	float BoxPos[] =
	{
		-0.2f,  -0.2f,  -0.2f,   1.0f, // 0
		-0.2f,  -0.2f,  -0.2f,   1.0f, // 1
		-0.2f,  -0.2f,  -0.2f,   1.0f, // 2
		 0.2f,  -0.2f,  -0.2f,   1.0f, // 3
		 0.2f,  -0.2f,  -0.2f,   1.0f, // 4
		 0.2f,  -0.2f,  -0.2f,   1.0f, // 5
		 0.2f,  -0.2f,   0.2f,   1.0f, // 6
		 0.2f,  -0.2f,   0.2f,   1.0f, // 7
		 0.2f,  -0.2f,   0.2f,   1.0f, // 8
		-0.2f,  -0.2f,   0.2f,   1.0f, // 9
		-0.2f,  -0.2f,   0.2f,   1.0f, // 10
		-0.2f,  -0.2f,   0.2f,   1.0f, // 11
		-0.2f,   0.2f,  -0.2f,   1.0f, // 12
		-0.2f,   0.2f,  -0.2f,   1.0f, // 13
		-0.2f,   0.2f,  -0.2f,   1.0f, // 14
		 0.2f,   0.2f,  -0.2f,   1.0f, // 15
		 0.2f,   0.2f,  -0.2f,   1.0f, // 16
		 0.2f,   0.2f,  -0.2f,   1.0f, // 17
		 0.2f,   0.2f,   0.2f,   1.0f, // 18
		 0.2f,   0.2f,   0.2f,   1.0f, // 19
		 0.2f,   0.2f,   0.2f,   1.0f, // 20
		-0.2f,   0.2f,   0.2f,   1.0f, // 21
	};

	// #24
   float BoxNormal[] =
   {
		 0.0f,  -1.0f,   0.0f,   1.0f,  // 0
		 0.0f,   0.0f,  -1.0f,   1.0f,  // 1
		-1.0f,   0.0f,   0.0f,   1.0f,  // 2
		 0.0f,  -1.0f,   0.0f,   1.0f,  // 3
		 0.0f,   0.0f,  -1.0f,   1.0f,  // 4
		 1.0f,   0.0f,   0.0f,   1.0f,  // 5
		 0.0f,  -1.0f,   0.0f,   1.0f,  // 6
		 1.0f,   0.0f,   0.0f,   1.0f,  // 7
		 0.0f,   0.0f,   1.0f,   1.0f,  // 8
		 0.0f,   0.0f,   1.0f,   1.0f,  // 9
		 0.0f,  -1.0f,   0.0f,   1.0f,  // 10
		-1.0f,   0.0f,   0.0f,   1.0f,  // 11
		-1.0f,   0.0f,   0.0f,   1.0f,  // 12
		 0.0f,   1.0f,   0.0f,   1.0f,  // 13
		 0.0f,   0.0f,  -1.0f,   1.0f,  // 14
		 0.0f,   0.0f,  -1.0f,   1.0f,  // 15
		 0.0f,   1.0f,   0.0f,   1.0f,  // 16
		 1.0f,   0.0f,   0.0f,   1.0f,  // 17
		 1.0f,   0.0f,   0.0f,   1.0f,  // 18
		 0.0f,   0.0f,   1.0f,   1.0f,  // 19
         0.0f,   1.0f,   0.0f,   1.0f,  // 20
		 0.0f,   0.0f,   1.0f,   1.0f,  // 21
	};

	float AppendIdx22[] = {
		-0.2f,   0.2f,   0.2f,   1.0f,  -1.0f,   0.0f,   0.0f,   1.0f };

	float AppendIdx23[] = {
		-0.2f,   0.2f,   0.2f,   1.0f,  0.0f,   1.0f,   0.0f,   1.0f };


	int IdxBox0[] = {  0,  3,  6,    1, 15,  4,    5, 18,  7,    9, 19, 21,    2 };
	unsigned int IdxBox0Short[] = { 0x00030000, 0x00010006, 0x0004000F, 0x00120005,
		                            0x00090007, 0x00150013, 0x00160002 };
	unsigned int IdxBox0Byte[] = { 0x01060300, 0x1205040F, 0x15130907, 0x0D0C1602 };

	int IdxBox1[] = { 12,    13, 20, 16,    0,  6, 10,   1,  14,  15,     5,  17,  18,     9,   8,  19,    2,  11 };
	unsigned int IdxBox1Short[] = { 0x000D000C, 0x00100014, 0x00060000, 0x0001000A,
		                            0x000F000E, 0x00110005, 0x00090012, 0x00130008,
									0x000B0002, 0x000D0016 };
	unsigned int IdxBox1Byte[] = { 0x06001014, 0x0F0E010A, 0x09121105, 0x0B021308, 0x14170D16};		// Fixed by yhim

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

    //ConstFloatAddr = FGVS_CFLOAT_SADDR;
	//matMVP.WriteMatrix4(ConstFloatAddr);
	
	fglWriteVertexShaderConstFloat(0, 16, matMVP.m[0]);	

	drvsys_clear_buf((unsigned int*)FIMG_DEPTH_BUFFER, CLEAR_SCREEN_SIZE, 0x00FFFFFF);
	drvsys_clear_buf((unsigned int*)FIMG_COLOR_BUFFER, CLEAR_SCREEN_SIZE, 0);


	fglFlush(FGL_PIPESTATE_ALL);	// Pipeline status

	fglVSSetAttribNum(3);
	fglPSSetAttributeNum(1);

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
	VtxAttrib.num = 0x18;
	fglSetVtxBufAttrib(0, &VtxAttrib);

	DataSize = sizeof(CubePos)/sizeof(float);

	for(i=0; i<DataSize; i++)
	{
		fglSendToVtxBuffer(*(unsigned int*)&CubePos[i]);	
		pVtxBufAddr++;
	}

	// Attribute 1 for Vertex Buffer
	fglSetVtxBufferAddr((unsigned int)pVtxBufAddr);		// Vetex buffer address
	VtxAttrib.stride = 16;								// Attribute n vertex buffer control
	VtxAttrib.addr = (unsigned int)pVtxBufAddr;			// Vertex buffer attribute0 base address
	VtxAttrib.num = 0x18;
	fglSetVtxBufAttrib(1, &VtxAttrib);

	for(i=0; i<DataSize; i++)
	{
		fglSendToVtxBuffer(*(unsigned int*)&CubeColor[i]);	
		pVtxBufAddr++;
	}

	// Attribute 2 for Vertex Buffer
	fglSetVtxBufferAddr((unsigned int)pVtxBufAddr);		// Vetex buffer address
	VtxAttrib.stride = 16;								// Attribute n vertex buffer control
	VtxAttrib.addr = (unsigned int)pVtxBufAddr;			// Vertex buffer attribute0 base address
	VtxAttrib.num = 0x18;
	fglSetVtxBufAttrib(2, &VtxAttrib);

	for(i=0; i<DataSize; i++)
	{
		fglSendToVtxBuffer(*(unsigned int*)&CubeNormal[i]);	
		pVtxBufAddr++;
	}

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
		nNumOfVertices = nNumTrisCube_HI2 * 3;

		if(j==0)
		{
			HInterface.idxType = FGL_INDEX_DATA_UINT;
			fglSetHInterface(&HInterface);
			
			unsigned int uiIndex = 0x00000000;
			fglSendToFIFO(4, &nNumOfVertices);
			fglSendToFIFO(4, &uiIndex);
		}
		else if(j==1)
		{
			HInterface.idxType = FGL_INDEX_DATA_USHORT;
			fglSetHInterface(&HInterface);
			
			unsigned int uiIndex = 0xFFFF0000;
			fglSendToFIFO(4, &nNumOfVertices);
			fglSendToFIFO(4, &uiIndex);
		}
		else
		{
			HInterface.idxType = FGL_INDEX_DATA_UBYTE;
			HInterface.enableVtxCache = FGL_TRUE;
			fglSetHInterface(&HInterface);
			
			unsigned int uiIndex = 0xFFFFFF00;
			fglSendToFIFO(4, &nNumOfVertices);
			fglSendToFIFO(4, &uiIndex);
		}

		DataSize = sizeof(RemainData0)/sizeof(float);

		fglSysTransferToPort(
								(unsigned int *)RemainData0,
								/*(volatile unsigned int *)(FGHI_FIFO_ENTRY),*/
								DataSize
							 );

		fglFlush(FGL_PIPESTATE_ALL);	// Pipeline status

		// Cache flush
		fglSysCacheFlush();

		fglSysDelay(50);

		drvsys_clear_buf((unsigned int*)FIMG_DEPTH_BUFFER, CLEAR_SCREEN_SIZE, 0x00FFFFFF);
		drvsys_clear_buf((unsigned int*)(FIMG_COLOR_BUFFER), CLEAR_SCREEN_SIZE, 0);

	}

	// HI IdxOffset SFR
	fglSetIndexOffset(0);		    		// Default value
	VtxAttrib.stride = 0;					// Default value
	VtxAttrib.addr = 0;						// Default value
	VtxAttrib.num = 0;
	fglSetVtxBufAttrib(2, &VtxAttrib);

 ///////////////////////////////// Index Transfer ///////////////////////////////////


 	//pVS = &Diffuse_vsa[0];

	if (fglLoadVShader(Diffuse_vsa) != FGL_ERR_NO_ERROR)
	{
		return ERROR;
	}

    //ConstFloatAddr = FGVS_CFLOAT_SADDR;
	//matMVP.WriteMatrix4(ConstFloatAddr);
	
	fglWriteVertexShaderConstFloat(0, 16, matMVP.m[0]);	

 	fglFlush(FGL_PIPESTATE_ALL);	// Pipeline status


/*
	// Vertex Shader SFR set
	WRITEREG(FGVS_ATTRIB_NUM, 0x00020002); // Attribute counter of VS input & output
	WRITEREG(FGVS_CONFIG, 0x00000001); // Configuration register

	// Pixel Shader SFR set
	WRITEREG(FGPS_EXE_MODE, 0x00000000); // Pixel shader execution mode control register
	WRITEREG(FGPS_PC_COPY, 0x00000001); // Copy PSPCS_ADDR value to program counter
	WRITEREG(FGPS_ATTRIB_NUM, 0x00000001); // Number of attribute of current context
	fglSysPollingMask(FGPS_INBUF_STATUS, 0x0, 0x00000001); // Status signal of pixel shader input buffer
	WRITEREG(FGPS_EXE_MODE, 0x00000001); // Pixel shader execution mode control register
*/
    fglVSSetAttribNum(2);
    fglPSSetAttributeNum(1);

 	/*
	 * Transfer the data of geometry to Vertex Buffer
	 */

	pVtxBufAddr = 0;
	
	// Attribute 0 for Vertex Buffer
	fglSetVtxBufferAddr((unsigned int)pVtxBufAddr);		// Vetex buffer address
	VtxAttrib.stride = 16;								// Attribute n vertex buffer control
	VtxAttrib.addr = (unsigned int)pVtxBufAddr;			// Vertex buffer attribute0 base address
	VtxAttrib.num = 0x16;
	fglSetVtxBufAttrib(0, &VtxAttrib);

	DataSize = sizeof(BoxPos)/sizeof(float);

	for(i=0; i<DataSize; i++)
	{
		fglSendToVtxBuffer(*(unsigned int*)&BoxPos[i]);	
		pVtxBufAddr++;
	}

	// Attribute 1 for Vertex Buffer
	// Attribute 0 for Vertex Buffer
	fglSetVtxBufferAddr((unsigned int)pVtxBufAddr);		// Vetex buffer address
	VtxAttrib.stride = 16;								// Attribute n vertex buffer control
	VtxAttrib.addr = (unsigned int)pVtxBufAddr;			// Vertex buffer attribute0 base address
	VtxAttrib.num = 0x16;
	fglSetVtxBufAttrib(1, &VtxAttrib);

	DataSize = sizeof(BoxNormal)/sizeof(float);

	for(i=0; i<DataSize; i++)
	{
		fglSendToVtxBuffer(*(unsigned int*)&BoxNormal[i]);
		pVtxBufAddr++;
	}

	// HI IdxOffset SFR
	fglSetIndexOffset(0);		    // Auto-Increment Mode
	// Primitive Engine SRF Set
	Vtx.prim = FGL_PRIM_TRIANGLES;
	Vtx.enablePointSize = FGL_FALSE;
	Vtx.numVSOut = 1;
	Vtx.shadeModel = FGL_SHADING_SMOOTH;
	Vtx.colorAttribIdx = 0;
	
	fglSetVertex(&Vtx);// Vertex context register
	// Host Interface SFR Set
	HIAttr.bEndFlag = FGL_FALSE;
	HIAttr.type = FGL_ATTRIB_DATA_FLOAT;
	HIAttr.numComp = 4;
	HIAttr.srcW = FGL_ATTRIB_ORDER_4TH;
	HIAttr.srcZ = FGL_ATTRIB_ORDER_3RD;
	HIAttr.srcY = FGL_ATTRIB_ORDER_2ND;
	HIAttr.srcX = FGL_ATTRIB_ORDER_1ST;	
	fglSetAttribute(0, &HIAttr);
	HIAttr.bEndFlag = FGL_TRUE;
	fglSetAttribute(1, &HIAttr);

	HInterface.enableAutoInc = FGL_FALSE;
	HInterface.enableVtxBuffer = FGL_TRUE;
	HInterface.enableVtxCache = FGL_TRUE;	
	HInterface.idxType = FGL_INDEX_DATA_UINT;
	HInterface.numVSOut = 2;	
	fglSetHInterface(&HInterface);
	
	unsigned int uiTmpVertices = 0;
	for(j=0; j<3; j++)
	{
		nNumOfVertices = nNumTrisCube_HI2 * 3;

		if(j==0)
		{
			// Index-Transfer Mode: index type unsigned int
			fglSetHInterface(&HInterface);
			fglSendToFIFO(4, &nNumOfVertices);

			//int IdxBox0[] = {  0,  3,  6,    1, 15,  4,    5, 18,  7,    9$$$, 19, 21,    2 };
			IdxSize = sizeof(IdxBox0)/sizeof(int);
			fglSysTransferToPort(
									(unsigned int *)IdxBox0,
									/*(volatile unsigned int *)(FGHI_FIFO_ENTRY),*/
									IdxSize
								 );
			//	22,
LOOP0:		fglGetNumEmptyFIFOSlots(&nEmptySpace);

			uiTmpVertices = 0x16;
			if(nEmptySpace > 1) {fglSendToFIFO(4, &uiTmpVertices);}
			else goto LOOP0;
			fglSysTransferToPort(
									(unsigned int *)AppendIdx22,
									/*(volatile unsigned int *)(FGHI_FIFO_ENTRY),*/
									0x8
								 );

			//int IdxBox1[] = { 12,    13, 20, 16,    0,  6, 10,   1,  14,  15,     5,  17,  18,     9,   8,  19,    2,  11 };
			IdxSize = sizeof(IdxBox1)/sizeof(int);
			fglSysTransferToPort(
									(unsigned int *)IdxBox1,
									/*(volatile unsigned int *)(FGHI_FIFO_ENTRY),*/
									IdxSize
								 );
			//	22,
LOOP1:		fglGetNumEmptyFIFOSlots(&nEmptySpace);

			uiTmpVertices = 0x16;
			if(nEmptySpace > 1) {fglSendToFIFO(4, &uiTmpVertices);}
			else goto LOOP1;
			fglSysTransferToPort(
									(unsigned int *)AppendIdx22,
									/*(volatile unsigned int *)(FGHI_FIFO_ENTRY),*/
									0x8
								 );
			//	13,
LOOP2:		fglGetNumEmptyFIFOSlots(&nEmptySpace);

			uiTmpVertices = 0xD;
			if(nEmptySpace > 1) {fglSendToFIFO(4, &uiTmpVertices);}
			else goto LOOP2;

			//	23,
LOOP3:		fglGetNumEmptyFIFOSlots(&nEmptySpace);

			uiTmpVertices = 0x17;
			if(nEmptySpace > 1) {fglSendToFIFO(4, &uiTmpVertices);}
			else goto LOOP3;
			fglSysTransferToPort(
									(unsigned int *)AppendIdx23,
									/*(volatile unsigned int *)(FGHI_FIFO_ENTRY),*/
									0x8
								 );
			//	20
LOOP4:		fglGetNumEmptyFIFOSlots(&nEmptySpace);

			uiTmpVertices = 0x14;
			if(nEmptySpace > 1) {fglSendToFIFO(4, &uiTmpVertices);}
			else goto LOOP4;
		}
		else if(j==1)
		{
			// Index-Transfer Mode: index type short
			HInterface.idxType = FGL_INDEX_DATA_USHORT;			
			fglSetHInterface(&HInterface);
			fglSendToFIFO(4, &nNumOfVertices);

			IdxSize = sizeof(IdxBox0Short)/sizeof(unsigned int);
			fglSysTransferToPort(
									(unsigned int *)IdxBox0Short,
									/*(volatile unsigned int *)(FGHI_FIFO_ENTRY),*/
									IdxSize
								 );

			fglSysTransferToPort(
									(unsigned int *)AppendIdx22,
									/*(volatile unsigned int *)(FGHI_FIFO_ENTRY),*/
									0x8
								 );
			IdxSize = sizeof(IdxBox1Short)/sizeof(unsigned int);
			fglSysTransferToPort(
									(unsigned int *)IdxBox1Short,
									/*(volatile unsigned int *)(FGHI_FIFO_ENTRY),*/
									IdxSize
								 );

			fglSysTransferToPort(
									(unsigned int *)AppendIdx22,
									/*(volatile unsigned int *)(FGHI_FIFO_ENTRY),*/
									0x8
								 );

 LOOP5:		fglGetNumEmptyFIFOSlots(&nEmptySpace);
			if(nEmptySpace > 1) 
			{
			    //WRITEREG(FGHI_FIFO_ENTRY, 0x00140017);
			    unsigned int uTmp = 0x00140017;
			    fglSendToFIFO(4, &uTmp);
			}
			else goto LOOP5;
			fglSysTransferToPort(
									(unsigned int *)AppendIdx23,
									/*(volatile unsigned int *)(FGHI_FIFO_ENTRY),*/
									0x8
								 );
		}
		else
		{
			// Index-Transfer Mode: index type unsigned byte
			HInterface.idxType = FGL_INDEX_DATA_UBYTE;			
			fglSetHInterface(&HInterface);
			fglSendToFIFO(4, &nNumOfVertices);

			IdxSize = sizeof(IdxBox0Byte)/sizeof(unsigned int);
			fglSysTransferToPort(
									(unsigned int *)IdxBox0Byte,
									/*(volatile unsigned int *)(FGHI_FIFO_ENTRY),*/
									IdxSize
								 );

			fglSysTransferToPort(
									(unsigned int *)AppendIdx22,
									/*(volatile unsigned int *)(FGHI_FIFO_ENTRY),*/
									0x8
								 );

			IdxSize = sizeof(IdxBox1Byte)/sizeof(unsigned int);
			fglSysTransferToPort(
									(unsigned int *)IdxBox1Byte,
									/*(volatile unsigned int *)(FGHI_FIFO_ENTRY),*/
									IdxSize
								 );

			fglSysTransferToPort(
									(unsigned int *)AppendIdx22,
									/*(volatile unsigned int *)(FGHI_FIFO_ENTRY),*/
									0x8
								 );

			fglSysTransferToPort(
									(unsigned int *)AppendIdx23,
									/*(volatile unsigned int *)(FGHI_FIFO_ENTRY),*/
									0x8
								 );
		}


		fglFlush(FGL_PIPESTATE_ALL);	// Pipeline status

		// Cache flush
		//fglSysCacheFlush();
		fglClearCache(FGL_CACHECTL_FLUSH_ZCACHE | FGL_CACHECTL_FLUSH_CCACHE);

		fglSysDelay(50);

		drvsys_clear_buf((unsigned int*)FIMG_DEPTH_BUFFER, CLEAR_SCREEN_SIZE, 0x00FFFFFF);
//		drvsys_clear_buf((unsigned int*)(FIMG_COLOR_BUFFER+m_FBOffsize), CLEAR_SCREEN_SIZE, 0);


	}

    fglEnablePerFragUnit(FGL_PF_DEPTH, FGL_FALSE);
	// HI IdxOffset SFR
	fglSetIndexOffset(0);		// Default value
	// Attribute 0 for Vertex Buffer
	VtxAttrib.stride = 0;		// Attribute n vertex buffer control
	VtxAttrib.addr = 0;			// Vertex buffer attribute0 base address
	VtxAttrib.num = 0;
	fglSetVtxBufAttrib(2, &VtxAttrib);


	return NO_ERROR;
}
