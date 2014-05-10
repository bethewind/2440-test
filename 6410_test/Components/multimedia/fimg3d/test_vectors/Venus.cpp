/*******************************************************************************
 *
 *	ViP model for FIMGSE
 *		version 1.0
 *
 *	Venus.c
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
 *	2006. 1. 21	by cheolkyoo.kim
 *
 *  Description
 *
 *	$RCSfile: Venus.cpp,v $
 *	$Revision: 1.6 $
 *	$Author: cheolkyoo.kim $
 *	$Date: 2006/05/08 05:37:06 $
 *	$Locker:  $
 *
 *	$Source: C:/CVS/CVSrepository/FIMG-3DSE_SW/fimg3dse_fpga/fimg3d/src/Venus.cpp,v $
 *	$State: Exp $
 *	$Log: Venus.cpp,v $
 *	Revision 1.6  2006/05/08 05:37:06  cheolkyoo.kim
 *	no message
 *	
 *	Revision 1.5  2006/04/13 11:53:58  cheolkyoo.kim
 *	no message
 *
 *	Revision 1.4  2006/04/08 07:09:33  inhosens.lee
 *	updated with fimg_ra code
 *
 *	Revision 1.3  2006/04/05 04:06:12  cheolkyoo.kim
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

#if 1
 
/****************************************************************************
 *  INCLUDES
 ****************************************************************************/

#include "Fimg3DTest.h"

#include "Models.vsa.h"
#include "Models.psa.h"
//#include "Venus.h"

/****************************************************************************
 *  DEFINES
 ****************************************************************************/

int Venus(void)

{
	//unsigned int i;
	unsigned int nNumOfVertices;
	unsigned int nNumofData;
	const unsigned int *pVS;
	const unsigned int *pPS;
	//unsigned int *pMatrix;
	//unsigned int ConstFloatAddr;

	Matrix4 matMVP(1.732051f,  0.0f,        0.0f,        0.0f,
				0.0f, 	    0.0f,        1.732051f,  -9.179870f,
				0.0f, 	    1.002002f,   0.0f,        9.819819f,
				0.0f,       1.0f,        0.0f,       10.0f);

	float lightVector[8] =
	{
		 0.0f,    0.0f,    0.0f,   0.0f, // ambient color
		-0.258f, -0.516f, -0.775f, 0.258f // light position
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

	//WRITEREG(0x00000108, 0xFFFFFF00);


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
	fglSetFaceCullControl(FGL_TRUE, FGL_FALSE/*CCW*/, FGL_FACE_BACK);

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
		
		//nNumOfVertices = nNumTrisVenus * 3;
		//nNumofData = nNumAttributesVenus * nNumOfVertices;
        //const int nNumTrisVenus = 1396;
        //const int nNumAttributesVenus = 12;
		nNumOfVertices = 1396 * 3;
		nNumofData = 12 * nNumOfVertices;

		unsigned int uiDummy = 0xFFFFFFFF;
		fglSendToFIFO(4, &nNumOfVertices);
		fglSendToFIFO(4, &uiDummy);
		
		fglSysTransferToPort(
								/*(unsigned int *)Venus_Data,*/
								(unsigned int *)FIMG_GEOMETRY_MEMORY,
								/*(volatile unsigned int *)(FGHI_FIFO_ENTRY),*/
								nNumofData
							 );

		fglFlush(FGL_PIPESTATE_ALL);	// Pipeline status

		// Cache flush
		//fglSysCacheFlush();
		fglClearCache(FGL_CACHECTL_FLUSH_ZCACHE | FGL_CACHECTL_FLUSH_CCACHE);

		fglSysDelay(50);

		drvsys_clear_buf((unsigned int*)FIMG_DEPTH_BUFFER, 0x4B000, 0);
//		drvsys_clear_buf((unsigned int*)(FIMG_COLOR_BUFFER+m_FBOffsize), CLEAR_SCREEN_SIZE, 0);


 	}

	fglEnablePerFragUnit(FGL_PF_DEPTH, FGL_FALSE);
	fglSetFaceCullControl(FGL_FALSE, FGL_FALSE, FGL_FACE_BACK);

    return NO_ERROR;
}

#else

#include "Fimg3DTest.h"
#include "Models.vsa.h"
#include "Models.psa.h"
//#include "Venus.h"

int Venus(void)

{
	unsigned int nNumOfVertices;
	unsigned int nNumofData;
	const unsigned int *pVS;
	const unsigned int *pPS;
	bool bSwapBackBuffer = false;

	float 	fXAng = 45.0f;
	float 	fYAng = 45.0f;

	Matrix4 matMV, matProj;
	Matrix4 matMVP(1.732051f,  0.0f,        0.0f,        0.0f,
					0.0f, 	 0.0f,        1.732051f,  -9.179870f,
					0.0f, 	 1.002002f,   0.0f,        9.819819f,
					0.0f,       1.0f,        0.0f,       10.0f);


	float lightVector[8] =
	{
		 0.0f,    0.0f,    0.0f,   0.0f, // ambient color
		-0.258f, -0.516f, -0.775f, 0.258f // light position
	};

	pVS = &Models_vsa[0];
	pPS = &Models_psa[0];

    	if (fglLoadVShader(pVS) != FGL_ERR_NO_ERROR)
	{
		return ERROR;
	}

	if (fglLoadPShader(pPS) != FGL_ERR_NO_ERROR)
	{
		return ERROR;
	}

	drvsys_clear_buf((unsigned int*)FIMG_DEPTH_BUFFER, CLEAR_SCREEN_SIZE, 0xFFFFFFFF);
	drvsys_clear_buf((unsigned int*)(FIMG_COLOR_BUFFER), CLEAR_SCREEN_SIZE, 0x0);
	fglFlush(FGL_PIPESTATE_ALL);	// Pipeline status

	fglSetVShaderAttribTable(FGL_OUTPUT_ATTRIB_IDX0, 0x02020100);
	fglSetVShaderAttribTable(FGL_OUTPUT_ATTRIB_IDX1, 0x07060504);
	fglSetVShaderAttribTable(FGL_OUTPUT_ATTRIB_IDX2, 0x0B0A0908);
	
	fglVSSetAttribNum(3);
	fglPSSetAttributeNum(1);

	unsigned int Offset = fglWriteVertexShaderConstFloat(0, 16, matMVP.m[0]);
	Offset = fglWriteVertexShaderConstFloat(Offset, 8, lightVector);


	// Raster Engine SFR set
	fglSetFaceCullControl(FGL_TRUE, FGL_FALSE/*CCW*/, FGL_FACE_BACK);

	fglSetLODRegister(FGL_LODCOEFF_ENABLE_ALL,	FGL_LODCOEFF_ENABLE_ALL,
					FGL_LODCOEFF_DISABLE,	FGL_LODCOEFF_DISABLE,
					FGL_LODCOEFF_DISABLE,	FGL_LODCOEFF_DISABLE,
					FGL_LODCOEFF_DISABLE,	FGL_LODCOEFF_DISABLE);

	// Per-fragment Unit SFR set
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
	
	nNumOfVertices = 1396 * 3;
	nNumofData = 12 * nNumOfVertices;

	unsigned int uiDummy = 0xFFFFFFFF;
	fglSendToFIFO(4, &nNumOfVertices);
	fglSendToFIFO(4, &uiDummy);
	
	fglSysTransferToPort((unsigned int *)FIMG_GEOMETRY_MEMORY, nNumofData);
	fglFlush(FGL_PIPESTATE_ALL);	// Pipeline status

	// Cache flush
	fglClearCache(FGL_CACHECTL_FLUSH_ZCACHE | FGL_CACHECTL_FLUSH_CCACHE);
	fglSysDelay(50);

	//drvsys_clear_buf((unsigned int*)FIMG_DEPTH_BUFFER, 0x4B000, 0);
	// Venus Model test main

	while(1)
	{
		matMV.LoadIdentity();
		matMVP.LoadIdentity();
		matMV.Rotate(1,0,0,fXAng);
		matMV.Rotate(0,1,0,fYAng);
		matMV.Translate(0, 0, -0.2);
		matMVP = matProj*matMV;

	      fglWriteVertexShaderConstFloat(0, 16, matMVP.m[0]);
	
		unsigned int uiDummy = 0xFFFFFFFF;
		fglSendToFIFO(4, &nNumOfVertices);
		fglSendToFIFO(4, &uiDummy);

		fglSysTransferToPort((unsigned int *)FIMG_GEOMETRY_MEMORY, nNumofData);
		fglFlush(FGL_PIPESTATE_ALL);	// Pipeline status
		fglClearCache(FGL_CACHECTL_INIT_ALL);

		fXAng += 5.0f;  // 5 degree
		fYAng += 10.0f;  // 10 degree

		if(fXAng > 360.f) fXAng = 0.0f;
		if(fYAng > 360.f) fYAng = 0.0f;			

		if(bSwapBackBuffer == false) 
		{
			fglSwapBuffer(1);
			drvsys_clear_buf((unsigned int*)FIMG_DEPTH_BUFFER, CLEAR_SCREEN_SIZE, 0xFFFFFFFF);
			drvsys_clear_buf((unsigned int*)(FIMG_COLOR_BUFFER), CLEAR_SCREEN_SIZE, 0xFF000000);
			fglSetColorBufBaseAddr(FIMG_COLOR_BUFFER);
			bSwapBackBuffer = true;
		}
		else
		{	
			fglSwapBuffer(0);
			drvsys_clear_buf((unsigned int*)FIMG_DEPTH_BUFFER, CLEAR_SCREEN_SIZE, 0xFFFFFFFF);
			drvsys_clear_buf((unsigned int*)(FIMG_COLOR_BUFFER+CLEAR_SCREEN_SIZE), CLEAR_SCREEN_SIZE, 0xFF000000);
			fglSetColorBufBaseAddr(FIMG_COLOR_BUFFER+(CLEAR_SCREEN_SIZE));
			bSwapBackBuffer = false;
		} 

 	}

	fglEnablePerFragUnit(FGL_PF_DEPTH, FGL_FALSE);
	fglSetFaceCullControl(FGL_FALSE, FGL_FALSE, FGL_FACE_BACK);

    return NO_ERROR;

}

#endif

