/*******************************************************************************
 *
 *	ViP model for FIMGSE
 *		version 1.0
 *
 *	Blend.c
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
 *	$RCSfile: Blend.cpp,v $
 *	$Revision: 1.7 $
 *	$Author: cheolkyoo.kim $
 *	$Date: 2006/05/08 05:34:30 $
 *	$Locker:  $
 *
 *	$Source: C:/CVS/CVSrepository/FIMG-3DSE_SW/fimg3dse_fpga/fimg3d/src/Blend.cpp,v $
 *	$State: Exp $
 *	$Log: Blend.cpp,v $
 *	Revision 1.7  2006/05/08 05:34:30  cheolkyoo.kim
 *	no message
 *	
 *	Revision 1.6  2006/04/13 11:53:44  cheolkyoo.kim
 *	no message
 *
 *	Revision 1.5  2006/04/07 07:30:58  inhosens.lee
 *	updated with fimg_ra code
 *
 *	Revision 1.4  2006/04/05 04:06:07  cheolkyoo.kim
 *	Replaced ~_XCORD and ~_YCORD to ~_XCOORD and ~_YCOORD.
 *
 *	Revision 1.3  2006/04/05 02:25:59  cheolkyoo.kim
 *	Some function are replaced by low-level API in the PF block
 *
 *	Revision 1.2  2006/03/30 04:27:42  cheolkyoo.kim
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
#include "register.h"
#include "library.h"
#include "mov_v2o.vsa.h"
#include "mov_v2o.psa.h"

/****************************************************************************
 *  DEFINES
 ****************************************************************************/

int Blend(void)

{
	FGL_BlendParam BlendParam;

	float VERTICES[5] = { -0.5f, 0.6666f, 0.0f, 0.0f, 1.0f };

	if (fglLoadVShader(mov_v2o_vsa) != FGL_ERR_NO_ERROR) {
		return ERROR;
	}

	if (fglLoadPShader(mov_v2o_psa) != FGL_ERR_NO_ERROR) {
		return ERROR;
	}

	drvsys_clear_buf((unsigned int*)(FIMG_COLOR_BUFFER), CLEAR_SCREEN_SIZE, 0xFFFFFF00);


	fglFlush(FGL_PIPESTATE_ALL);	// Pipeline status



	// Vertex Shader SFR set
	//WRITEREG(FGVS_ATTRIB_NUM, 0x00020002); // Attribute counter of VS input & output
	//WRITEREG(FGVS_CONFIG, 0x00000001); // Configuration register
	fglVSSetAttribNum(2);

	// Pixel Shader SFR set
	//WRITEREG(FGIB_EXE_MODE, 0x00000000); // Pixel shader execution mode control register
	//WRITEREG(FGIB_PC_COPY, 0x00000001); // Copy PSPCS_ADDR value to program counter
	//WRITEREG(FGIB_ATTRIB_NUM, 0x00000001); // Number of attribute of current context
	//fglSysPollingMask(FGIB_INBUF_STATUS, 0x0, 0x00000001); // Status signal of pixel shader input buffer
	//WRITEREG(FGIB_EXE_MODE, 0x00000001); // Pixel shader execution mode control register
	fglPSSetAttributeNum(1);

	// Blend test main
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
		fglSetPointWidth(50.0f);
		fglSetMinimumPointWidth(1.0f);
		fglSetMaximumPointWidth(1024.0f);

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
		
		BlendParam.dstAlpha = FGL_BLEND_ONE;
		BlendParam.dstColor = FGL_BLEND_ONE;
		BlendParam.srcAlpha = FGL_BLEND_ONE;
		BlendParam.srcColor = FGL_BLEND_ONE;
		BlendParam.constColor = 0xFFFFFFFF;
/*
	 //Colors are added as: (1, 1, 0) + (0, 0, 1) = (1, 1, 1)
	 //  which will produce a white square on a yellow background.
	 glBlendEquation(GL_FUNC_ADD);

	 // Colors are subtracted as: (0, 0, 1) - (1, 1, 0) = (-1, -1, 1)
	 //  which is clamped to (0, 0, 1), producing a blue square on a
	 //  yellow background
	 glBlendEquation(GL_FUNC_SUBTRACT);

	 // Colors are subtracted as: (1, 1, 0) - (0, 0, 1) = (1, 1, -1)
	 //  which is clamed to (1, 1, 0).  This produces yellow for both
	 //  the square and the background.
	 glBlendEquation(GL_FUNC_REVERSE_SUBTRACT);

	 // The minimum of each component is computed, as
	 //  [min(1, 0), min(1, 0), min(0, 1)] which equates to (0, 0, 0).
	 //  This will produce a black square on the yellow background.
	 glBlendEquation(GL_MIN);

	 // The minimum of each component is computed, as
	 //  [max(1, 0), max(1, 0), max(0, 1)] which equates to (1, 1, 1)
	 //  This will produce a white square on the yellow background.
	 glBlendEquation(GL_MAX);
*/		
		for(int i = 0; i < 6; i++)
		{
			unsigned int uiTmpVertices = 1;		
			unsigned int uiDummy = 0xFFFFFFFF;
			fglSendToFIFO(4, &uiTmpVertices);
			fglSendToFIFO(4, &uiDummy);

			unsigned int uDataSize = sizeof(VERTICES)/sizeof(VERTICES[0]);

			fglSysTransferToPorts(
									(unsigned int *)VERTICES,
									(volatile unsigned int *)(FGHI_FIFO_ENTRY),
									uDataSize
								 );
		
		//	fglFlush(FGL_PIPESTATE_ALL_WITH_CCACHE);	// Pipeline status
			fglFlush(FGL_PIPESTATE_ALL);	// Pipeline status
			
			// Per-fragment blend control set
			if(i != 5)
			{
				BlendParam.colorEqua = (FGL_BlendEqua)i;
				BlendParam.alphaEqua = (FGL_BlendEqua)i;

				if(fglEnablePerFragUnit(FGL_PF_BLENDING, FGL_TRUE) != FGL_ERR_NO_ERROR) {
					UART_Printf("\nBlending Test: The fglEnablePerFragUnit failed\n");
				}

				if(fglSetBlendParams(&BlendParam) != FGL_ERR_NO_ERROR) {
					UART_Printf("\nBlending Test: The fglSetBlendParams failed\n");
				}


				if(((i+1) % 2) == 1) {
					VERTICES[0] += 1.0f;
				}
				else {
					VERTICES[0] = -0.5f;
					VERTICES[1] -= 0.666f;
				}
			}
		}


		// Cache flush
		//fglSysCacheFlush();
		//fglClearCache(FGL_CACHECTL_FLUSH_ZCACHE | FGL_CACHECTL_FLUSH_CCACHE);
		fglClearCache(FGL_CACHECTL_FLUSH_CCACHE);

		//fglSysDelay(50);

	}
	
	//drvsys_clear_buf((unsigned int*)(FIMG_COLOR_BUFFER), CLEAR_SCREEN_SIZE, 0xFFFFFFFF);


	if(fglEnablePerFragUnit(FGL_PF_BLENDING, FGL_FALSE) != FGL_ERR_NO_ERROR) {
		UART_Printf("\nBlending Test: The fglEnablePerFragUnit fial\n");
		return ERROR;
	}
	
	return NO_ERROR;
}

