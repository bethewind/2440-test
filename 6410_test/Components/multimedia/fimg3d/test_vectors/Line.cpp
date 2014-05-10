/*******************************************************************************
 *
 *	ViP model for FIMGSE
 *		version 1.0
 *
 *	Line.c
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

int Line(void)

{		
	float attributes[] = 
	{

		#if 1
		// x     y     z      r      g      b     a
		0.0f, 0.0f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		0.0f, 0.800000f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		0.0f, 0.0f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		0.138919f, 0.787846f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		0.0f, 0.0f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		0.273616f, 0.751754f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		0.0f, 0.0f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		0.400000f, 0.692820f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		0.0f, 0.0f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		0.514230f, 0.612836f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		0.0f, 0.0f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		0.612836f, 0.514230f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		0.0f, 0.0f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		0.692820f, 0.400000f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		0.0f, 0.0f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		0.751754f, 0.273616f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		0.0f, 0.0f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		0.787846f, 0.138919f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		0.0f, 0.0f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		0.800000f, 0.0f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		0.0f, 0.0f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		0.787846f, -0.138919f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		0.0f, 0.0f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		0.751754f, -0.273616f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		0.0f, 0.0f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		0.692820f, -0.400000f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		0.0f, 0.0f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		0.612836f, -0.514230f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		0.0f, 0.0f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		0.514230f, -0.612836f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		0.0f, 0.0f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		0.400000f, -0.692820f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		0.0f, 0.0f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		0.273616f, -0.751754f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		0.0f, 0.0f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		0.138919f, -0.787846f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		0.0f, 0.0f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		0.0f, -0.800000f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		0.0f, 0.0f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		-0.138919f, -0.787846f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		0.0f, 0.0f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		-0.273616f, -0.751754f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		0.0f, 0.0f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		-0.400000f, -0.692820f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		0.0f, 0.0f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		-0.514230f, -0.612836f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		0.0f, 0.0f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		-0.612836f, -0.514230f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		0.0f, 0.0f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		-0.692820f, -0.400000f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		0.0f, 0.0f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		-0.751754f, -0.273616f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		0.0f, 0.0f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		-0.787846f, -0.138919f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		0.0f, 0.0f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		-0.800000f, -0.0f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		0.0f, 0.0f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		-0.787846f, 0.138919f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		0.0f, 0.0f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		-0.751754f, 0.273616f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		0.0f, 0.0f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		-0.692820f, 0.400000f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		0.0f, 0.0f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		-0.612836f, 0.514230f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		0.0f, 0.0f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		-0.514230f, 0.612836f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		0.0f, 0.0f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		-0.400000f, 0.692820f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		0.0f, 0.0f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		-0.273616f, 0.751754f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		0.0f, 0.0f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		-0.138919f, 0.787846f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f

		#else

		// x     y     z      r      g      b     a

		0.0f, 0.0f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		0.612836f, -0.514230f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		0.0f, 0.0f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		0.514230f, -0.612836f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		0.0f, 0.0f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		0.400000f, -0.692820f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		0.0f, 0.0f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		0.273616f, -0.751754f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		0.0f, 0.0f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		0.138919f, -0.787846f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		0.0f, 0.0f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		0.0f, -0.800000f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		0.0f, 0.0f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		-0.138919f, -0.787846f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		0.0f, 0.0f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		-0.273616f, -0.751754f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		0.0f, 0.0f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		-0.400000f, -0.692820f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		0.0f, 0.0f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		-0.514230f, -0.612836f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		0.0f, 0.0f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		-0.612836f, -0.514230f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		0.0f, 0.0f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		-0.692820f, -0.400000f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		0.0f, 0.0f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		-0.751754f, -0.273616f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		0.0f, 0.0f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		-0.787846f, -0.138919f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		0.0f, 0.0f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		-0.800000f, -0.0f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		0.0f, 0.0f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		-0.787846f, 0.138919f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		0.0f, 0.0f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		-0.751754f, 0.273616f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		0.0f, 0.0f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		-0.692820f, 0.400000f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		0.0f, 0.0f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		-0.612836f, 0.514230f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		0.0f, 0.0f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		-0.514230f, 0.612836f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		0.0f, 0.0f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		-0.400000f, 0.692820f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		0.0f, 0.0f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		-0.273616f, 0.751754f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		0.0f, 0.0f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f,
		-0.138919f, 0.787846f, 0.5f, 0.75f, 0.75f, 0.75f, 1.0f

		#endif
	};

    if (fglLoadVShader(mvp_v2o_vsa) != FGL_ERR_NO_ERROR) {
		return ERROR;
    }

    if (fglLoadPShader(mov_v2o_psa) != FGL_ERR_NO_ERROR) {
		return ERROR;
    }

	drvsys_clear_buf((unsigned int*)(FIMG_COLOR_BUFFER), CLEAR_SCREEN_SIZE, 0x00000000);

	fglFlush(FGL_PIPESTATE_ALL);	// Pipeline status
	//fglSysPollingMask(0x20000000, 0x0, FGL_PIPESTATE_ALL); // Pipeline status
	
	Matrix4 matMV, matProj, matMVP;	
	/* Build projection matrix once. */
	//matProj.SetAsPerspectiveMatrix(45, 0.75, 1.0, 20.0);
	matProj.SetAsPerspectiveMatrix(45, 1.67, 1.0, 20.0);

	fglVSSetAttribNum(2);
    	fglPSSetAttributeNum(1);

	// Line test main
	{
		matMV.Translate(0, 0, -4.5);
		matMVP = matProj * matMV;
	    
   	    	fglWriteVertexShaderConstFloat(0, 16, matMVP.m[0]);
   	    
		// Line Width set
		fglSetLineWidth(1.123f);
		
		// Primitive Engine SRF Set 
		FGL_Vertex Vtx;
		Vtx.prim = FGL_PRIM_LINES;
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

		fglFlush(FGL_PIPESTATE_ALL);	// Pipeline statuss
		
		//fglSysCacheFlush();
		fglClearCache(FGL_CACHECTL_FLUSH_ZCACHE | FGL_CACHECTL_FLUSH_CCACHE);

		//fglSysDelay(50);
	}

    return NO_ERROR;
}

