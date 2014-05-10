/*******************************************************************************
 *
 *	ViP model for FIMGSE
 *		version 1.0
 *
 *	DepthWriteMask.c
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
 *	$RCSfile: DepthWriteMask.cpp,v $
 *	$Revision: 1.6 $
 *	$Author: cheolkyoo.kim $
 *	$Date: 2006/05/08 05:34:29 $
 *	$Locker:  $
 *
 *	$Source: C:/CVS/CVSrepository/FIMG-3DSE_SW/fimg3dse_fpga/fimg3d/src/DepthWriteMask.cpp,v $
 *	$State: Exp $
 *	$Log: DepthWriteMask.cpp,v $
 *	Revision 1.6  2006/05/08 05:34:29  cheolkyoo.kim
 *	no message
 *	
 *	Revision 1.5  2006/04/13 11:53:45  cheolkyoo.kim
 *	no message
 *
 *	Revision 1.4  2006/04/08 03:50:06  inhosens.lee
 *	updated with fimg_ra code
 *
 *	Revision 1.3  2006/04/05 04:06:08  cheolkyoo.kim
 *	Replaced ~_XCORD and ~_YCORD to ~_XCOORD and ~_YCOORD.
 *
 *	Revision 1.2  2006/03/30 04:27:43  cheolkyoo.kim
 *	Thomas-20060330@comment: Add to code using directive WIN32_VIP for VIP
 *
 *	Revision 1.1  2006/03/30 02:23:26  cheolkyoo.kim
 *	Thomas-20060330@comment: Initial commit
 *
 *	Revision 1.1  2006/03/10 08:29:07  cheolkyoo.kim
 *	Initial import of FIMG-3DSE_SW package
 *
 *
 ******************************************************************************/
/****************************************************************************
 *  INCLUDES
 ****************************************************************************/
#include "stdio.h"
#include "Fimg3DTest.h"
#include "library.h"
#include "mov_v2o.vsa.h"
#include "mov_v2o.psa.h"

/****************************************************************************
 *  DEFINES
 ****************************************************************************/

int DepthWriteMask(void)

{
	unsigned int uDataSize;

#if 0
	float box_obj0[] =
	{
		/* x      y      z      w      r      g      b      a */
		-0.1f, -0.1f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  0.5f,
		 0.1f, -0.1f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  0.5f,
		-0.1f,  0.1f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  0.5f,
		 0.1f,  0.1f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  0.5f
	};

	float box_obj1[] =
	{
		/* x      y      z      w      r      g      b      a */
		 0.0f,  0.0f,  0.5f,  1.0f,  1.0f,  1.0f,  0.0f,  0.5f,
		 0.2f,  0.0f,  0.5f,  1.0f,  1.0f,  1.0f,  0.0f,  0.5f,
		 0.0f,  0.2f,  0.5f,  1.0f,  1.0f,  1.0f,  0.0f,  0.5f,
		 0.2f,  0.2f,  0.5f,  1.0f,  1.0f,  1.0f,  0.0f,  0.5f
	};
#endif

    float VERTICES[2][6] = 
    {
        { 0.0f, 0.0f, 0.5f, 1.0f, 1.0f, 1.0f }, 
        { 0.0f, 0.0f, 0.75f, 0.0f, 0.0f, 0.0f }
    };

    if (fglLoadVShader(mov_v2o_vsa) != FGL_ERR_NO_ERROR) {
		return ERROR;
    }
    if (fglLoadPShader(mov_v2o_psa) != FGL_ERR_NO_ERROR) {
		return ERROR;
    }

	drvsys_clear_buf((unsigned int*)FIMG_DEPTH_BUFFER, CLEAR_SCREEN_SIZE, 0xFFFFFFFF);
	drvsys_clear_buf((unsigned int*)(FIMG_COLOR_BUFFER), CLEAR_SCREEN_SIZE, 0x0);


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

	// Depth write mask test main
	{

		// Primitive Engine SRF Set
		FGL_Vertex Vtx;
		Vtx.prim = FGL_PRIM_POINTS;
		Vtx.enablePointSize = FGL_FALSE;
		Vtx.numVSOut = 1;
		Vtx.shadeModel = FGL_SHADING_SMOOTH;
		Vtx.colorAttribIdx = 0;
		
		fglSetVertex(&Vtx); // Vertex context register

		//point size set
		fglSetPointWidth(200.0f);
		fglSetMinimumPointWidth(1.0f);
		fglSetMaximumPointWidth(1024.0f);

		// Depth/Stencil buffer base address
		fglSetZBufBaseAddr(FIMG_DEPTH_BUFFER);
		
		// Depth test function & enable set
		if(fglSetDepthParams(FGL_COMP_LESS) != FGL_ERR_NO_ERROR) {
			UART_Printf("\nDepth buffer test: The fglSetAlphaParams fial\n");
		}

		if(fglEnablePerFragUnit(FGL_PF_DEPTH, FGL_TRUE) != FGL_ERR_NO_ERROR) {
			UART_Printf("\nDepth buffer test: The fglEnablePerFragUnit fial\n");
		}

		fglSetZBufWriteMask(FGL_TRUE);
		
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
		fglSetAttribute(1, &HIAttr);
		
		unsigned int uiTmpVertices = 1;		
		unsigned int uiDummy = 0xFFFFFFFF;
		fglSendToFIFO(4, &uiTmpVertices);
		fglSendToFIFO(4, &uiDummy);

		uDataSize = sizeof(VERTICES)/sizeof(float)/2;

		float *vtx = VERTICES[0];
		fglSysTransferToPort(
								(unsigned int *)vtx,
								/*(volatile unsigned int *)(FGHI_FIFO_ENTRY),*/
								uDataSize
							 );

		fglFlush(FGL_PIPESTATE_ALL);	// Pipeline status
		
		
		fglSetPointWidth(100.0f);
		
		fglSetZBufWriteMask(FGL_FALSE);
		
		fglSendToFIFO(4, &uiTmpVertices);
		fglSendToFIFO(4, &uiDummy);

        vtx = VERTICES[1];
		fglSysTransferToPort(
								(unsigned int *)vtx,
								/*(volatile unsigned int *)(FGHI_FIFO_ENTRY),*/
								uDataSize
							 );

    	fglFlush(FGL_PIPESTATE_ALL);	// Pipeline status
		
	}
#if 0
		// Per-fragment SFR set
		// For Depth Test set to SFR
		fglSysModifyReg(FGPF_DEPTH, 0x0000000E, (5<<1)|0x1); // Depth test mode: greater

		WRITEREG(FGPF_DEPTHBUF_ADDR, FIMG_DEPTH_BUFFER); // Depth/Stencil buffer base address
		WRITEREG(FGPF_DEPTH_MASK, 0x00000001); // Depth buffer write mask

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
		
		unsigned int uiTmpVertices = 4;		
		unsigned int uiDummy = 0xFFFFFFFF;
		fglSendToFIFO(4, &uiTmpVertices);
		fglSendToFIFO(4, &uiDummy);

		uDataSize = sizeof(box_obj0)/sizeof(box_obj0[0]);

		fglSysTransferToPort(
								(unsigned int *)box_obj0,
								/*(volatile unsigned int *)(FGHI_FIFO_ENTRY),*/
								uDataSize
							 );


		fglSysPollingMask(FG_PIPELINE_STATUS, 0x0, 0x00033717); // Pipeline status

		// Per-fragment SFR set.
		WRITEREG(FGPF_DEPTH_MASK, 0x00000000); // Depth buffer write mask
		regval = ((5<<1)|0x1) & 0x0000000E;
		WRITEREG(FGPF_LOGIC_OP, regval);

		fglSendToFIFO(4, &uiTmpVertices);
		fglSendToFIFO(4, &uiDummy);

		uDataSize = sizeof(box_obj1)/sizeof(box_obj1[0]);

		fglSysTransferToPort(
								(unsigned int *)box_obj1,
								/*(volatile unsigned int *)(FGHI_FIFO_ENTRY),*/
								uDataSize
							 );

		fglSysPollingMask(FG_PIPELINE_STATUS, 0x0, 0x00033717); // Pipeline status

		WRITEREG(FGPF_DEPTH, 0x0); // Disable depth test
	}
#endif

	// Cache flush
	//fglSysCacheFlush();
	fglClearCache(FGL_CACHECTL_FLUSH_ZCACHE | FGL_CACHECTL_FLUSH_CCACHE);
	//fglSysDelay(50);
	
	//drvsys_clear_buf((unsigned int*)(FIMG_COLOR_BUFFER), CLEAR_SCREEN_SIZE, 0x0);
	drvsys_clear_buf((unsigned int*)FIMG_DEPTH_BUFFER, CLEAR_SCREEN_SIZE, 0x0);


	if(fglEnablePerFragUnit(FGL_PF_DEPTH, FGL_FALSE) != FGL_ERR_NO_ERROR) {
		UART_Printf("\nDepth buffer test: The fglEnablePerFragUnit fial");
	}

    return NO_ERROR;
}

