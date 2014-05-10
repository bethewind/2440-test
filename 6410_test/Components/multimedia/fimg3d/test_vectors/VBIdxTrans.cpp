/*******************************************************************************
 *
 *	ViP model for FIMGSE
 *		version 1.0
 *
 *	VBIdxTrans.c
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
 *	$RCSfile: VBIdxTrans.cpp,v $
 *	$Revision: 1.6 $
 *	$Author: cheolkyoo.kim $
 *	$Date: 2006/05/08 05:37:06 $
 *	$Locker:  $
 *
 *	$Source: C:/CVS/CVSrepository/FIMG-3DSE_SW/fimg3dse_fpga/fimg3d/src/VBIdxTrans.cpp,v $
 *	$State: Exp $
 *	$Log: VBIdxTrans.cpp,v $
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


#include "Diffuse.vsa.h"
#include "mov_v2o.psa.h"

/****************************************************************************
 *  DEFINES
 ****************************************************************************/
//static unsigned int nNumTrisCube_HI1 = 12;

int VBIdxTrans(void)

{
	unsigned int i;
	unsigned int j;
	//const unsigned int *pVS;
	//const unsigned int *pPS;
	unsigned int nNumOfVertices;
	unsigned int *pVtxBufAddr;
	unsigned int DataSize;
	unsigned int IdxVal;
	//unsigned int ConstFloatAddr;
    unsigned int nNumTrisCube_HI1 = 12;

	// #24
	float CubePos[] =
	{
		-0.2f,  -0.2f,  -0.2f,   1.0f,
		-0.2f,  -0.2f,  -0.2f,   1.0f,
		-0.2f,  -0.2f,  -0.2f,   1.0f,
		 0.2f,  -0.2f,  -0.2f,   1.0f,
		 0.2f,  -0.2f,  -0.2f,   1.0f,
		 0.2f,  -0.2f,  -0.2f,   1.0f,
		 0.2f,  -0.2f,   0.2f,   1.0f,
		 0.2f,  -0.2f,   0.2f,   1.0f,
		 0.2f,  -0.2f,   0.2f,   1.0f,
		-0.2f,  -0.2f,   0.2f,   1.0f,
		-0.2f,  -0.2f,   0.2f,   1.0f,
		-0.2f,  -0.2f,   0.2f,   1.0f,
		-0.2f,   0.2f,  -0.2f,   1.0f,
		-0.2f,   0.2f,  -0.2f,   1.0f,
		-0.2f,   0.2f,  -0.2f,   1.0f,
		 0.2f,   0.2f,  -0.2f,   1.0f,
		 0.2f,   0.2f,  -0.2f,   1.0f,
		 0.2f,   0.2f,  -0.2f,   1.0f,
		 0.2f,   0.2f,   0.2f,   1.0f,
		 0.2f,   0.2f,   0.2f,   1.0f,
		 0.2f,   0.2f,   0.2f,   1.0f,
		-0.2f,   0.2f,   0.2f,   1.0f,
		-0.2f,   0.2f,   0.2f,   1.0f,
		-0.2f,   0.2f,   0.2f,   1.0f
	};

	// #24
   float CubeNormal[] =
   {
		 0.0f,  -1.0f,   0.0f,   1.0f,
		 0.0f,   0.0f,  -1.0f,   1.0f,
		-1.0f,   0.0f,   0.0f,   1.0f,
		 0.0f,  -1.0f,   0.0f,   1.0f,
		 0.0f,   0.0f,  -1.0f,   1.0f,
		 1.0f,   0.0f,   0.0f,   1.0f,
		 0.0f,  -1.0f,   0.0f,   1.0f,
		 1.0f,   0.0f,   0.0f,   1.0f,
		 0.0f,   0.0f,   1.0f,   1.0f,
		 0.0f,   0.0f,   1.0f,   1.0f,
		 0.0f,  -1.0f,   0.0f,   1.0f,
		-1.0f,   0.0f,   0.0f,   1.0f,
		-1.0f,   0.0f,   0.0f,   1.0f,
		 0.0f,   1.0f,   0.0f,   1.0f,
		 0.0f,   0.0f,  -1.0f,   1.0f,
		 0.0f,   0.0f,  -1.0f,   1.0f,
		 0.0f,   1.0f,   0.0f,   1.0f,
		 1.0f,   0.0f,   0.0f,   1.0f,
		 1.0f,   0.0f,   0.0f,   1.0f,
		 0.0f,   0.0f,   1.0f,   1.0f,
		 0.0f,   1.0f,   0.0f,   1.0f,
		 0.0f,   0.0f,   1.0f,   1.0f,
		-1.0f,   0.0f,   0.0f,   1.0f,
		 0.0f,   1.0f,   0.0f,   1.0f
	};

	// #12
	unsigned int IdxBox[] =
	{
		 0,   3,   6,
		 1,  15,   4,
		 5,  18,   7,
		 9,  19,  21,
		 2,  22,  12,
		13,  20,  16,
		 0,   6,  10,
		 1,  14,  15,
		 5,  17,  18,
		 9,   8,  19,
		 2,  11,  22,
		13,  23,  20
	};

	Matrix4 matMVP(1.707107f,  0.0f,       1.707107f, 0.0f,
			1.207107f,  1.707107f, -1.207107f, 0.0f,
			0.501001f, -0.708522f, -0.501001f, 1.302803f,
			0.5f,      -0.707107f, -0.5f,      1.5f);

	if (fglLoadVShader(Diffuse_vsa) != FGL_ERR_NO_ERROR)
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
	drvsys_clear_buf((unsigned int*)(FIMG_COLOR_BUFFER), CLEAR_SCREEN_SIZE, 0);


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
	// Per-fragment SFR set
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
	VtxAttrib.num = 0x18;
	fglSetVtxBufAttrib(0, &VtxAttrib);

	DataSize = sizeof(CubePos)/sizeof(float);

	for(i=0; i<DataSize; i++)
	{
		fglSendToVtxBuffer(*(unsigned int*)&CubePos[i]);
		pVtxBufAddr++;
	}

	// Attribute 1 for Vertex Buffer
	// Attribute 0 for Vertex Buffer
	fglSetVtxBufferAddr((unsigned int)pVtxBufAddr);		// Vetex buffer address
	VtxAttrib.stride = 16;								// Attribute n vertex buffer control
	VtxAttrib.addr = (unsigned int)pVtxBufAddr;			// Vertex buffer attribute0 base address
	VtxAttrib.num = 0x18;
	fglSetVtxBufAttrib(1, &VtxAttrib);

	DataSize = sizeof(CubeNormal)/sizeof(float);

	for(i=0; i<DataSize; i++)
	{
		fglSendToVtxBuffer(*(unsigned int*)&CubeNormal[i]);
		pVtxBufAddr++;
	}


	// Question
	// HI IdxOffset SFR
	fglSetIndexOffset(0);		    // Auto-Increment Mode

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
	HIAttr.bEndFlag = FGL_TRUE;
	fglSetAttribute(1, &HIAttr);

	FGL_HInterface HInterface;
	HInterface.enableAutoInc = FGL_FALSE;
	HInterface.enableVtxBuffer = FGL_TRUE;
	HInterface.enableVtxCache = FGL_TRUE;	
	HInterface.idxType = FGL_INDEX_DATA_UINT;
	HInterface.numVSOut = 2;
	
	for(j=0; j<3; j++)
	{
		nNumOfVertices = nNumTrisCube_HI1 * 3;

		if(j==0)
		{
			// Auto-Increment Mode: index type unsigned int
			HInterface.idxType = FGL_INDEX_DATA_UINT;
			fglSetHInterface(&HInterface);
			
			fglSendToFIFO(4, &nNumOfVertices);		 // 18 vertices
			fglSendToFIFO(4 * nNumOfVertices, IdxBox);			
		}
		else if(j==1)
		{
			// Auto-Increment Mode: index type short
			HInterface.idxType = FGL_INDEX_DATA_USHORT;
			fglSetHInterface(&HInterface);
			fglSendToFIFO(4, &nNumOfVertices);		 // 18 vertices

			for(i=0; i<nNumOfVertices;)
			{
				IdxVal = ( (IdxBox[i] & 0xFFFF) | ((IdxBox[i+1] & 0xFFFF)<<16)) ; // start index (after a dummy vertex) // index = 0, 1, 2, ... , 17
				fglSendToFIFO(4, &IdxVal);
				i += 2;
			}
		}
		else
		{
			// Auto-Increment Mode: index type unsigned byte
			HInterface.idxType = FGL_INDEX_DATA_UBYTE;
			fglSetHInterface(&HInterface);
			fglSendToFIFO(4, &nNumOfVertices);		 // 18 vertices

			for(i=0; i<nNumOfVertices;)
			{
				IdxVal = ( (IdxBox[i] & 0xFF) | ((IdxBox[i+1] & 0xFFFF)<<8) |
				           ((IdxBox[i+2] & 0xFF)<<16) | ((IdxBox[i+3] & 0xFF)<<24)) ; // start index (after a dummy vertex) // index = 0, 1, 2, ... , 17
				fglSendToFIFO(4, &IdxVal);
				i += 4;
			}
		}

		fglFlush(FGL_PIPESTATE_ALL);	// Pipeline status

		// Cache flush
		//fglSysCacheFlush();
		fglClearCache(FGL_CACHECTL_FLUSH_ZCACHE | FGL_CACHECTL_FLUSH_CCACHE);

		fglSysDelay(50);

		drvsys_clear_buf((unsigned int*)FIMG_DEPTH_BUFFER, 0x4B000, 0x00FFFFFF);
		//drvsys_clear_buf((unsigned int*)(FIMG_COLOR_BUFFER+m_FBOffsize), CLEAR_SCREEN_SIZE, 0);


	}

    fglEnablePerFragUnit(FGL_PF_DEPTH, FGL_FALSE);
	return NO_ERROR;
}
