/*******************************************************************************
 *
 *	ViP model for FIMGSE
 *		version 1.0
 *
 *	Dragon.c
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
 *	2006. 1. 24	by cheolkyoo.kim
 *
 *  Description
 *
 *	$RCSfile: Dragon.cpp,v $
 *	$Revision: 1.6 $
 *	$Author: cheolkyoo.kim $
 *	$Date: 2006/05/08 05:34:29 $
 *	$Locker:  $
 *
 *	$Source: C:/CVS/CVSrepository/FIMG-3DSE_SW/fimg3dse_fpga/fimg3d/src/Dragon.cpp,v $
 *	$State: Exp $
 *	$Log: Dragon.cpp,v $
 *	Revision 1.6  2006/05/08 05:34:29  cheolkyoo.kim
 *	no message
 *	
 *	Revision 1.5  2006/04/13 11:54:26  cheolkyoo.kim
 *	no message
 *
 *	Revision 1.4  2006/04/08 03:50:29  inhosens.lee
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
 ******************************************************************************/
/****************************************************************************
 *  INCLUDES
 ****************************************************************************/
#include "Fimg3DTest.h"


#include "Models.vsa.h"
#include "Models.psa.h"
//#include "Dragon.h"

/****************************************************************************
 *  DEFINES
 ****************************************************************************/

int Dragon(void)

{
	//unsigned int i;
	unsigned int nNumOfVertices;
	unsigned int nNumofData;
	const unsigned int *pVS;
	const unsigned int *pPS;
	//unsigned int *pMatrix;
	//unsigned int ConstFloatAddr;

	Matrix4 matMVP(-1.673033f, 0.0f,      0.448288f, -0.138564f,
				-0.224144f, 1.5f,     -0.836516f, -0.692820f,
				 0.224593f, 0.501001f, 0.838191f,  0.551301f,
				 0.224144f, 0.5f,      0.836516f,  0.75f);

	float lightVector[8] =
	{
		  0.0f,      0.0f,    0.0f,   0.0f, // ambient color
		 -1.558f,  -1.516f,   0.775f, 0.258f // light position
	};


	pVS = &Models_vsa[0];
	pPS = &Models_psa[0];

    if (fglLoadVShader(pVS) != FGL_ERR_NO_ERROR) {
		return ERROR;
    }

    if (fglLoadPShader(pPS) != FGL_ERR_NO_ERROR) {
		return ERROR;
    }

/*
    ConstFloatAddr = FGVS_CFLOAT_SADDR;
    ConstFloatAddr = matMVP.WriteMatrix4(ConstFloatAddr);

	pMatrix = (unsigned int *)&lightVector[0];
	for(i=0; i < 8; i++)
	{
		WRITEREG(ConstFloatAddr, *pMatrix);
		pMatrix++;
		ConstFloatAddr += 4;
	}
*/	
	unsigned int Offset = fglWriteVertexShaderConstFloat(0, 16, matMVP.m[0]);
	Offset = fglWriteVertexShaderConstFloat(Offset, 8, lightVector);
	

	drvsys_clear_buf((unsigned int*)FIMG_DEPTH_BUFFER, CLEAR_SCREEN_SIZE, 0xFFFFFFFF);
	drvsys_clear_buf((unsigned int*)(FIMG_COLOR_BUFFER), CLEAR_SCREEN_SIZE, 0x0);


	fglFlush(FGL_PIPESTATE_ALL);	// Pipeline status


/*
	// Vertex Shader SFR set
	WRITEREG(FGVS_ATTRIB_NUM, 0x00020003); // Attribute counter of VS output & input
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

	// Raster Engine SFR set
	//WRITEREG(FGRA_BACKFACE_CULL, 0x00000008); // Back-face culling control(enable, CCW-front, Culling back)

	// Per-fragment Unit SFR set
	//WRITEREG(FGPF_DEPTHBUF_ADDR, FIMG_DEPTH_BUFFER); // Depth/Stencil buffer base address
	//WRITEREG(FGPF_DEPTH, 0x00000005); // Depth test LESS function & enable set
	fglSetZBufBaseAddr(FIMG_DEPTH_BUFFER);
	fglSetDepthParams(FGL_COMP_LESS);
	fglEnablePerFragUnit(FGL_PF_DEPTH, FGL_TRUE);

	// Pawn Model test main
	{
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
		
		//nNumOfVertices = nNumTrisDragon * 3;
		//nNumofData = nNumAttributesDragon * nNumOfVertices;
        //const int nNumTrisDragon = 5104;
        //const int nNumAttributesDragon = 12;

		nNumOfVertices = 5104 * 3;
		nNumofData = 12 * nNumOfVertices;

		unsigned int uiDummy = 0xFFFFFFFF;
		fglSendToFIFO(4, &nNumOfVertices);
		fglSendToFIFO(4, &uiDummy);
		
		fglSysTransferToPort(
								/*(unsigned int *)Dragon_Data,*/
								(unsigned int *)FIMG_GEOMETRY_MEMORY,
								/*(volatile unsigned int *)(FGHI_FIFO_ENTRY),*/
								nNumofData
							 );

		fglFlush(FGL_PIPESTATE_ALL);	// Pipeline status

		// Cache flush
		//fglSysCacheFlush();
		fglClearCache(FGL_CACHECTL_FLUSH_ZCACHE | FGL_CACHECTL_FLUSH_CCACHE);

		//fglSysDelay(100);

		drvsys_clear_buf((unsigned int*)FIMG_DEPTH_BUFFER, 0x4B000, 0);
//		drvsys_clear_buf((unsigned int*)(FIMG_COLOR_BUFFER), CLEAR_SCREEN_SIZE, 0);


 	}

	fglEnablePerFragUnit(FGL_PF_DEPTH, FGL_FALSE);
    return NO_ERROR;
}


