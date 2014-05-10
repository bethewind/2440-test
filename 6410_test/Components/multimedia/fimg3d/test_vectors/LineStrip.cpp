/*******************************************************************************
 *
 *	ViP model for FIMGSE
 *		version 1.0
 *
 *	LineStrip.c
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
 *	2005. 3. 22	by cheolkyoo.kim
 *
 *  Description
 *
 *	$RCSfile:  $
 *	$Revision:  $
 *	$Author:  $
 *	$Date:  $
 *	$Locker:  $
 *
 *	$Source:  $
 *	$State:  $
 *	$Log:  $
 ******************************************************************************/
/****************************************************************************
 *  INCLUDES
 ****************************************************************************/
#include "Fimg3DTest.h"


#include "mvp_v2o.vsa.h"
#include "mov_v2o.psa.h"

/****************************************************************************
 *  DEFINES
 ****************************************************************************/


int LineStrip(void)

{    	
	float attributes[] = 
	{
		0.000000f, 0.500000f, 0.5f, 1.0f, 0.4f, 0.1f, 1.0f, 
		0.000000f, 0.500000f, 0.5f, 1.0f, 0.4f, 0.1f, 1.0f, 
		0.000000f, 0.500000f, 0.5f, 1.0f, 0.4f, 0.1f, 1.0f, 
		0.171010f, 0.469846f, 0.5f, 1.0f, 0.4f, 0.1f, 1.0f, 
		0.171010f, 0.469846f, 0.5f, 1.0f, 0.4f, 0.1f, 1.0f, 
		0.321394f, 0.383022f, 0.5f, 1.0f, 0.4f, 0.1f, 1.0f, 
		0.321394f, 0.383022f, 0.5f, 1.0f, 0.4f, 0.1f, 1.0f, 
		0.433013f, 0.250000f, 0.5f, 1.0f, 0.4f, 0.1f, 1.0f, 
		0.433013f, 0.250000f, 0.5f, 1.0f, 0.4f, 0.1f, 1.0f, 
		0.492404f, 0.086824f, 0.5f, 1.0f, 0.4f, 0.1f, 1.0f, 
		0.492404f, 0.086824f, 0.5f, 1.0f, 0.4f, 0.1f, 1.0f, 
		0.492404f, -0.086824f, 0.5f, 1.0f, 0.4f, 0.1f, 1.0f, 
		0.492404f, -0.086824f, 0.5f, 1.0f, 0.4f, 0.1f, 1.0f, 
		0.433013f, -0.250000f, 0.5f, 1.0f, 0.4f, 0.1f, 1.0f, 
		0.433013f, -0.250000f, 0.5f, 1.0f, 0.4f, 0.1f, 1.0f, 
		0.321394f, -0.383022f, 0.5f, 1.0f, 0.4f, 0.1f, 1.0f, 
		0.321394f, -0.383022f, 0.5f, 1.0f, 0.4f, 0.1f, 1.0f, 
		0.171010f, -0.469846f, 0.5f, 1.0f, 0.4f, 0.1f, 1.0f, 
		0.171010f, -0.469846f, 0.5f, 1.0f, 0.4f, 0.1f, 1.0f, 
		0.000000f, -0.500000f, 0.5f, 1.0f, 0.4f, 0.1f, 1.0f, 
		0.000000f, -0.500000f, 0.5f, 1.0f, 0.4f, 0.1f, 1.0f, 
		-0.171010f, -0.469846f, 0.5f, 1.0f, 0.4f, 0.1f, 1.0f, 
		-0.171010f, -0.469846f, 0.5f, 1.0f, 0.4f, 0.1f, 1.0f, 
		-0.321394f, -0.383022f, 0.5f, 1.0f, 0.4f, 0.1f, 1.0f, 
		-0.321394f, -0.383022f, 0.5f, 1.0f, 0.4f, 0.1f, 1.0f, 
		-0.433013f, -0.250000f, 0.5f, 1.0f, 0.4f, 0.1f, 1.0f, 
		-0.433013f, -0.250000f, 0.5f, 1.0f, 0.4f, 0.1f, 1.0f, 
		-0.492404f, -0.086824f, 0.5f, 1.0f, 0.4f, 0.1f, 1.0f, 
		-0.492404f, -0.086824f, 0.5f, 1.0f, 0.4f, 0.1f, 1.0f, 
		-0.492404f, 0.086824f, 0.5f, 1.0f, 0.4f, 0.1f, 1.0f, 
		-0.492404f, 0.086824f, 0.5f, 1.0f, 0.4f, 0.1f, 1.0f, 
		-0.433013f, 0.250000f, 0.5f, 1.0f, 0.4f, 0.1f, 1.0f, 
		-0.433013f, 0.250000f, 0.5f, 1.0f, 0.4f, 0.1f, 1.0f, 
		-0.321394f, 0.383022f, 0.5f, 1.0f, 0.4f, 0.1f, 1.0f, 
		-0.321394f, 0.383022f, 0.5f, 1.0f, 0.4f, 0.1f, 1.0f, 
		-0.171010f, 0.469846f, 0.5f, 1.0f, 0.4f, 0.1f, 1.0f
	};

    if (fglLoadVShader(mvp_v2o_vsa) != FGL_ERR_NO_ERROR) {
		return ERROR;
    }

    if (fglLoadPShader(mov_v2o_psa) != FGL_ERR_NO_ERROR) {
		return ERROR;
    }

	drvsys_clear_buf((unsigned int*)(FIMG_COLOR_BUFFER), CLEAR_SCREEN_SIZE, 0x0);

	fglFlush(FGL_PIPESTATE_ALL);	// Pipeline status
	

	Matrix4 matMV, matProj, matMVP;
	//matProj.SetAsPerspectiveMatrix(45.0, 0.75, 1.0, 20.0);	
	matProj.SetAsPerspectiveMatrix(45, 1.67, 1.0, 20.0);
	/* Build projection matrix once. */
/*
	// Vertex Shader SFR set
	WRITEREG(FGVS_ATTRIB_NUM, 0x00020002); // Attribute counter of VS input & output
	WRITEREG(FGVS_CONFIG, 0x00000001); // Configuration register
	// Pixel Shader SFR set
	WRITEREG(FGPS_EXE_MODE, 0x0); // Pixel shader execution mode control register
	WRITEREG(FGPS_PC_COPY, 0x1); // Copy PSPCS_ADDR value to program counter
	WRITEREG(FGPS_ATTRIB_NUM, 0x1); // Number of attribute of current context
	fglSysPollingMask(FGPS_INBUF_STATUS, 0x0, 0x1); // Status signal of pixel shader input buffer
	WRITEREG(FGPS_EXE_MODE, 0x1); // Pixel shader execution mode control register
*/
    fglVSSetAttribNum(2);
    fglPSSetAttributeNum(1);

	// LineStrip test main
	{		
/*
		makeRotateMatrix(fXAng, 1, 0, 0, rotateMatrix);
		makeRotateMatrix(fYAng, 0, 1, 0, rotateMatrix1);
		multMatrix(rotateMatrix, rotateMatrix, rotateMatrix1);
		multMatrix(modelMatrix, translateMatrix, rotateMatrix);
*/
		matMV.Translate(0., 0., -4.5);
		matMVP = matProj * matMV;
		
	    //unsigned int pShaderParam = FGVS_CFLOAT_SADDR;
	    //matMVP.WriteMatrix4(pShaderParam);
	    
	    fglWriteVertexShaderConstFloat(0, 16, matMVP.m[0]);
		
		// Line Width set
		fglSetLineWidth(10.0f);

		
		// Primitive Engine SRF Set
		FGL_Vertex Vtx;
		Vtx.prim = FGL_PRIM_LINE_STRIP;
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
		HIAttr.numComp = 3;
		HIAttr.srcW = FGL_ATTRIB_ORDER_4TH;
		HIAttr.srcZ = FGL_ATTRIB_ORDER_3RD;
		HIAttr.srcY = FGL_ATTRIB_ORDER_2ND;
		HIAttr.srcX = FGL_ATTRIB_ORDER_1ST;	
		fglSetAttribute(0, &HIAttr);
		HIAttr.bEndFlag = FGL_TRUE;
		HIAttr.numComp = 4;
		fglSetAttribute(1, &HIAttr);
		
		unsigned int nNumOfData = sizeof(attributes)/sizeof(float);
		unsigned int nNumOfVertices = nNumOfData/7;
		unsigned int uiDummy = 0xFFFFFFFF;
		fglSendToFIFO(4, &nNumOfVertices);
		fglSendToFIFO(4, &uiDummy);

		fglSysTransferToPort(
								(unsigned int *)attributes,
								/*(volatile unsigned int *)(FGHI_FIFO_ENTRY),*/
								nNumOfData
							 );

		fglFlush(FGL_PIPESTATE_ALL);	// Pipeline status
		
		//fglSysCacheFlush();
		fglClearCache(FGL_CACHECTL_FLUSH_ZCACHE | FGL_CACHECTL_FLUSH_CCACHE);

		//fglSysDelay(50);
	}

/*
	if(m_SwapBackBuffer == FALSE) 
	{
		fglSwapBuffer(0);
		drvsys_clear_buf((unsigned int*)(m_FrontFBAddr), CLEAR_SCREEN_SIZE, 0x0);
		fglSetColorBufBaseAddr(m_FrontFBAddr);
		m_SwapBackBuffer = TRUE;
	}
	else
	{
		fglSwapBuffer(1);
		drvsys_clear_buf((unsigned int*)(m_BackFBAddr), CLEAR_SCREEN_SIZE, 0x0);
		fglSetColorBufBaseAddr(m_BackFBAddr);
		m_SwapBackBuffer = FALSE;
	} 
*/



    return NO_ERROR;
}
