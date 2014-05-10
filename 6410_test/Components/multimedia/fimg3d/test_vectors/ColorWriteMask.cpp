/*******************************************************************************
 *
 *	ViP model for FIMGSE
 *		version 1.0
 *
 *	ColorWriteMask.c
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
 *	$RCSfile: ColorWriteMask.cpp,v $
 *	$Revision: 1.7 $
 *	$Author: cheolkyoo.kim $
 *	$Date: 2006/05/08 05:34:30 $
 *	$Locker:  $
 *
 *	$Source: C:/CVS/CVSrepository/FIMG-3DSE_SW/fimg3dse_fpga/fimg3d/src/ColorWriteMask.cpp,v $
 *	$State: Exp $
 *	$Log: ColorWriteMask.cpp,v $
 *	Revision 1.7  2006/05/08 05:34:30  cheolkyoo.kim
 *	no message
 *	
 *	Revision 1.6  2006/04/13 11:53:45  cheolkyoo.kim
 *	no message
 *
 *	Revision 1.5  2006/04/08 00:46:17  inhosens.lee
 *	updated with fimg_ra code
 *
 *	Revision 1.4  2006/04/05 04:06:07  cheolkyoo.kim
 *	Replaced ~_XCORD and ~_YCORD to ~_XCOORD and ~_YCOORD.
 *
 *	Revision 1.3  2006/03/30 08:25:50  cheolkyoo.kim
 *	Thomas-20060330@comment: initial commit
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

int ColorWriteMask(void)

{
	unsigned int uDataSize;
	FGL_BlendParam BlendParam;
	
    float VERTICES[5] = { -0.75f, 0.75f, 1.0f, 1.0f, 1.0f };
	
    if (fglLoadVShader(mov_v2o_vsa) != FGL_ERR_NO_ERROR) {
		return ERROR;
    }

    if (fglLoadPShader(mov_v2o_psa) != FGL_ERR_NO_ERROR) {
		return ERROR;
    }

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
		
// Software workaround for FIMG-3DSE 
	BlendParam.dstAlpha = FGL_BLEND_ZERO;
	BlendParam.dstColor = FGL_BLEND_ZERO;
	BlendParam.srcAlpha = FGL_BLEND_ONE;
	BlendParam.srcColor = FGL_BLEND_ONE;
	BlendParam.colorEqua = FGL_EQ_ADD;
	BlendParam.alphaEqua = FGL_EQ_ADD;
	BlendParam.constColor = 0xFFFFFFFF;

	if(fglEnablePerFragUnit(FGL_PF_BLENDING, FGL_TRUE) != FGL_ERR_NO_ERROR) {
		UART_Printf("\nBlending Test: The fglEnablePerFragUnit fial\n");
	}

	if(fglSetBlendParams(&BlendParam) != FGL_ERR_NO_ERROR) {
		UART_Printf("\nBlending Test: The fglSetBlendParams fial\n");
	}


	// Color write mask test main
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
		//WRITEREGF(FGRA_POINT_SIZE_MIN, 1.0f);
		//WRITEREGF(FGRA_POINT_SIZE_MAX, 1024.0f);
		//WRITEREGF(FGRA_POINT_WIDTH, 50.0f);
		fglSetMinimumPointWidth(1.0f);
		fglSetMaximumPointWidth(1024.0f);
		fglSetPointWidth(50.0f);
		
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
		HIAttr.numComp = 2;
		HIAttr.srcW = FGL_ATTRIB_ORDER_4TH;
		HIAttr.srcZ = FGL_ATTRIB_ORDER_3RD;
		HIAttr.srcY = FGL_ATTRIB_ORDER_2ND;
		HIAttr.srcX = FGL_ATTRIB_ORDER_1ST;	
		fglSetAttribute(0, &HIAttr);
		HIAttr.bEndFlag = FGL_TRUE;
		HIAttr.numComp = 3;
		fglSetAttribute(1, &HIAttr);
		
		unsigned int uiTmpVertices = 1;		
		unsigned int uiDummy = 0xFFFFFFFF;
		for(int i = 0; i < 16; i++)
		{
			// Per-fragment color buffer mask register
            fglSetColorBufWriteMask((i & 0x1), (i & 0x2), (i & 0x4), (i & 0x8));
			
			fglSendToFIFO(4, &uiTmpVertices);
			fglSendToFIFO(4, &uiDummy);

			uDataSize = sizeof(VERTICES)/sizeof(VERTICES[0]);

			fglSysTransferToPort(
									(unsigned int *)VERTICES,
									/*(volatile unsigned int *)(FGHI_FIFO_ENTRY),*/
									uDataSize
								 );

			fglFlush(FGL_PIPESTATE_ALL);	// Pipeline status

		    if(((i+1) % 4) == 0) {
		        VERTICES[0] = -0.75f;
		        VERTICES[1] -= 0.5f;
		    }
		    else {
		        VERTICES[0] += 0.5f;
		    }
		}
    }

	// Cache flush
	//fglSysCacheFlush();
	fglClearCache(FGL_CACHECTL_FLUSH_ZCACHE | FGL_CACHECTL_FLUSH_CCACHE);

	//fglSysDelay(50);
    
	//drvsys_clear_buf((unsigned int*)(FIMG_COLOR_BUFFER), CLEAR_SCREEN_SIZE, 0xFFFFFFFF);

    
    fglSetColorBufWriteMask(FGL_FALSE, FGL_FALSE, FGL_FALSE, FGL_FALSE);

	if(fglEnablePerFragUnit(FGL_PF_BLENDING, FGL_FALSE) != FGL_ERR_NO_ERROR) {
		UART_Printf("\nBlending Test: The fglEnablePerFragUnit fial\n");
	}

    return NO_ERROR;
}

