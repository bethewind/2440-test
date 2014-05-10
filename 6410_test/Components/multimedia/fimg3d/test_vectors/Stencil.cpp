/*******************************************************************************
 *
 *	ViP model for FIMGSE
 *		version 1.0
 *
 *	Stencil.c
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
 *	2005. 12. 26	by cheolkyoo.kim
 *
 *  Description
 *
 *	$RCSfile: Stencil.cpp,v $
 *	$Revision: 1.7 $
 *	$Author: cheolkyoo.kim $
 *	$Date: 2006/05/08 05:37:10 $
 *	$Locker:  $
 *
 *	$Source: C:/CVS/CVSrepository/FIMG-3DSE_SW/fimg3dse_fpga/fimg3d/src/Stencil.cpp,v $
 *	$State: Exp $
 *	$Log: Stencil.cpp,v $
 *	Revision 1.7  2006/05/08 05:37:10  cheolkyoo.kim
 *	no message
 *	
 *	Revision 1.6  2006/04/13 11:54:17  cheolkyoo.kim
 *	no message
 *
 *	Revision 1.5  2006/04/08 04:21:28  inhosens.lee
 *	updated with fimg_ra code
 *
 *	Revision 1.4  2006/04/05 04:06:09  cheolkyoo.kim
 *	Replaced ~_XCORD and ~_YCORD to ~_XCOORD and ~_YCOORD.
 *
 *	Revision 1.3  2006/04/05 02:25:59  cheolkyoo.kim
 *	Some function are replaced by low-level API in the PF block
 *
 *	Revision 1.2  2006/03/30 04:27:44  cheolkyoo.kim
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


#include "stdio.h"
#include "library.h"
#include "mov_v2o.vsa.h"
#include "mov_v2o.psa.h"

/****************************************************************************
 *  DEFINES
 ****************************************************************************/

int Stencil(void)

{
	unsigned int uDataSize;
	//unsigned int regval;
	const unsigned int *pVS;
	const unsigned int *pPS;
	FGL_StencilParam StencilParam;

	float triangle[] =
	{
		/* x      y      z      w      r      g      b      a  */
		-0.2f,  0.1f,  0.5f,  1.0f,  1.0f,  0.0f,  0.0f,  0.5f,
		 0.2f,  0.1f,  0.5f,  1.0f,  1.0f,  0.0f,  0.0f,  0.5f,
		 0.0f, -0.2f,  0.5f,  1.0f,  1.0f,  0.0f,  0.0f,  0.5f
	};

	float rectangle[] =
	{
		 /* x       y      z      w      r      g      b      a  */
		-0.05f,  0.08f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.5f,
		 0.05f,  0.08f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.5f,
		-0.05f, -0.02f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.5f,
		 0.05f, -0.02f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.5f
	};

	pVS = &mov_v2o_vsa[0];
	pPS = &mov_v2o_psa[0];

    if (fglLoadVShader(pVS) != FGL_ERR_NO_ERROR) {
		UART_Printf("\nStencil Test: The LoadVShader fial");
		return ERROR;
    }

    if (fglLoadPShader(pPS) != FGL_ERR_NO_ERROR) {
		UART_Printf("\nStencil Test: The LoadPShader fial");
		return ERROR;
    }

	drvsys_clear_buf((unsigned int*)FIMG_DEPTH_BUFFER, CLEAR_SCREEN_SIZE, 0);
	drvsys_clear_buf((unsigned int*)FIMG_COLOR_BUFFER, CLEAR_SCREEN_SIZE, 0);


	fglFlush(FGL_PIPESTATE_ALL);	// Pipeline status



	// Vertex Shader SFR set
	//WRITEREG(FGVS_ATTRIB_NUM, 0x00020002); // Attribute counter of VS input & output
	//WRITEREG(FGVS_CONFIG, 0x00000001); // Configuration register
    fglVSSetAttribNum(2);
    
	// Pixel Shader SFR set
	//WRITEREG(FGPS_EXE_MODE, 0x00000000); // Pixel shader execution mode control register
	//WRITEREG(FGPS_PC_COPY, 0x00000001); // Copy PSPCS_ADDR value to program counter
	//WRITEREG(FGPS_ATTRIB_NUM, 0x00000001); // Number of attribute of current context
	//fglSysPollingMask(FGPS_INBUF_STATUS, 0x0, 0x00000001); // Status signal of pixel shader input buffer
	//WRITEREG(FGPS_EXE_MODE, 0x00000001); // Pixel shader execution mode control register
    fglPSSetAttributeNum(1);

	// Stencil test main
	{
		// Primitive Engine SRF Set
		FGL_Vertex Vtx;
		Vtx.prim = FGL_PRIM_TRIANGLE_STRIP;
		Vtx.enablePointSize = FGL_FALSE;
		Vtx.numVSOut = 1;
		Vtx.shadeModel = FGL_SHADING_SMOOTH;
		Vtx.colorAttribIdx = 0;
		
		fglSetVertex(&Vtx); // Vertex context register

		// Depth/Stencil buffer base address
		fglSetZBufBaseAddr(FIMG_DEPTH_BUFFER);

		// Stencil control
		//WRITEREG(FGPF_FRONTFACE_STENCIL, 0x6D800003); // Front face stencil test control
		//WRITEREG(FGPF_BACKFACE_STENCIL, 0x6D800003); // Back face stencil test hhcontrol
		StencilParam.face = FGL_FACE_FRONT;
		StencilParam.zpass = FGL_ACT_INCR;
		StencilParam.zfail = FGL_ACT_INCR;
		StencilParam.sfail = FGL_ACT_INCR;
		StencilParam.mask = 0;
		StencilParam.ref = 0;
		StencilParam.mode = FGL_COMP_ALWAYS;


		if(fglSetStencilParams(&StencilParam) != FGL_ERR_NO_ERROR)
		{
			UART_Printf("Stencil Test: The fglSetStencilParams fial");
		}

		StencilParam.face = FGL_FACE_BACK;

		if(fglSetStencilParams(&StencilParam) != FGL_ERR_NO_ERROR)
		{
			UART_Printf("Stencil Test: The fglSetStencilParams fial");
		}

		if(fglEnablePerFragUnit(FGL_PF_STENCIL, FGL_TRUE) != FGL_ERR_NO_ERROR)
		{
			UART_Printf("Stencil Test: The fglEnablePerFragUnit fial");
		}

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
		HIAttr.bEndFlag = FGL_TRUE;
		fglSetAttribute(1, &HIAttr);
		
		// Host interface Set
		unsigned int uiTmpVertices = 4;		
		unsigned int uiDummy = 0xFFFFFFFF;
		fglSendToFIFO(4, &uiTmpVertices);
		fglSendToFIFO(4, &uiDummy);

		uDataSize = sizeof(rectangle)/sizeof(rectangle[0]);

 		fglSysTransferToPort(
								(unsigned int *)rectangle,
								/*(volatile unsigned int *)(FGHI_FIFO_ENTRY),*/
								uDataSize
							 );

		fglFlush(FGL_PIPESTATE_ALL);	// Pipeline status

		// Color Cache flush ?
		//fglSysCCacheFlush();
		fglClearCache(FGL_CACHECTL_FLUSH_CCACHE);

		// Cache flush
		//fglSysCacheFlush();
		fglSysCCacheFlush();

		fglSysDelay(50);

		fglFlush(FGL_PIPESTATE_ALL);	// Pipeline status

		// Primitive Engine SRF Set
		Vtx.prim = FGL_PRIM_TRIANGLES;
		Vtx.enablePointSize = FGL_FALSE;
		Vtx.numVSOut = 1;
		Vtx.shadeModel = FGL_SHADING_SMOOTH;
		Vtx.colorAttribIdx = 0;
		
		fglSetVertex(&Vtx); // Vertex context register

		// Stencil test control register
		//WRITEREG(FGPF_FRONTFACE_STENCIL, 0x0000101F); // Front face stencil test control
		//WRITEREG(FGPF_BACKFACE_STENCIL, 0x0000101F); // Back face stencil test control
		StencilParam.face = FGL_FACE_FRONT;
		StencilParam.zpass = FGL_ACT_KEEP;
		StencilParam.zfail = FGL_ACT_KEEP;
		StencilParam.sfail = FGL_ACT_KEEP;
		StencilParam.mask = 1;
		StencilParam.ref = 1;
		StencilParam.mode = FGL_COMP_NOTEQUAL;

		if(fglSetStencilParams(&StencilParam) != FGL_ERR_NO_ERROR)
		{
			UART_Printf("Stencil Test: The fglSetStencilParams fial");
		}

		StencilParam.face = FGL_FACE_BACK;

		if(fglSetStencilParams(&StencilParam) != FGL_ERR_NO_ERROR)
		{
			UART_Printf("Stencil Test: The fglSetStencilParams fial");
		}

		if(fglEnablePerFragUnit(FGL_PF_STENCIL, FGL_TRUE) != FGL_ERR_NO_ERROR)
		{
			UART_Printf("Stencil Test: The fglEnablePerFragUnit fial");
		}

		uiTmpVertices = 3;
		fglSendToFIFO(4, &uiTmpVertices);
		fglSendToFIFO(4, &uiDummy);

		uDataSize = sizeof(triangle)/sizeof(triangle[0]);

 		fglSysTransferToPort(
								(unsigned int *)triangle,
								/*(volatile unsigned int *)(FGHI_FIFO_ENTRY),*/
								uDataSize
							 );

		fglFlush(FGL_PIPESTATE_ALL);	// Pipeline status

		// Cache flush
		//fglSysCacheFlush();
		fglClearCache(FGL_CACHECTL_FLUSH_ZCACHE | FGL_CACHECTL_FLUSH_CCACHE);

		fglSysDelay(50);

		drvsys_clear_buf((unsigned int*)FIMG_DEPTH_BUFFER, CLEAR_SCREEN_SIZE, 0);
//		drvsys_clear_buf((unsigned int*)(FIMG_COLOR_BUFFER), CLEAR_SCREEN_SIZE, 0);


    }

	if(fglEnablePerFragUnit(FGL_PF_STENCIL, FGL_FALSE) != FGL_ERR_NO_ERROR)
	{
		UART_Printf("Stencil Test: The fglEnablePerFragUnit fial");
	}

	//const char* pFileName_SFR_Dump = "Stencil_SFR.txt";
	//fglDumpContext(pFileName_SFR_Dump);

    return NO_ERROR;
}

