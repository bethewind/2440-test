/*******************************************************************************
 *
 *	ViP model for FIMGSE
 *		version 1.0
 *
 *	Viewport.c
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
 *	2006.  1. 25	by cheolkyoo.kim
 *
 *  Description -
 *
 *	$RCSfile: Viewport.cpp,v $
 *	$Revision: 1.7 $
 *	$Author: cheolkyoo.kim $
 *	$Date: 2006/05/08 05:37:06 $
 *	$Locker:  $
 *
 *	$Source: C:/CVS/CVSrepository/FIMG-3DSE_SW/fimg3dse_fpga/fimg3d/src/Viewport.cpp,v $
 *	$State: Exp $
 *	$Log: Viewport.cpp,v $
 *	Revision 1.7  2006/05/08 05:37:06  cheolkyoo.kim
 *	no message
 *	
 *	Revision 1.6  2006/04/13 11:53:58  cheolkyoo.kim
 *	no message
 *
 *	Revision 1.5  2006/04/08 07:09:33  inhosens.lee
 *	updated with fimg_ra code
 *
 *	Revision 1.4  2006/04/05 04:06:12  cheolkyoo.kim
 *	Replaced ~_XCORD and ~_YCORD to ~_XCOORD and ~_YCOORD.
 *
 *	Revision 1.3  2006/04/05 03:31:23  cheolkyoo.kim
 *	Modify FGPE_VIEWPORT_CENTER_XCOORD to FGPE_VIEWPORT_CENTER_XCOORD
 *
 *	Revision 1.2  2006/03/30 04:27:46  cheolkyoo.kim
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

#include "mov_v2o.vsa.h"
#include "mov_v2o.psa.h"

/****************************************************************************
 *  DEFINES
 ****************************************************************************/


int Viewport(void)

{
	//unsigned int i;
	unsigned int j;
	//unsigned int *pReg;
	FGL_FBCtrlParam fbCtrlParam;

//	unsigned int uDefaultRegVal[11];

	float vertex0[] = {
		-0.5f, -0.5f,  0.5f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,
 		 0.5f, -0.5f,  0.5f,  1.0f,	 0.0f,  1.0f,  0.0f,  1.0f,
		 0.0f,  0.5f,  0.5f,  1.0f,	 0.0f,  0.0f,  1.0f,  1.0f
	};
	float vertex1[] = {
		-1.0f,  1.0f,  0.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
 		-1.0f, -1.0f,  0.0f,  1.0f,	 1.0f,  1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  0.0f,  1.0f,	 1.0f,  1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  0.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
 		 1.0f,  1.0f,  0.0f,  1.0f,	 1.0f,  1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  0.0f,  1.0f,	 1.0f,  1.0f,  1.0f,  1.0f
	};
	
    if (fglLoadVShader(mov_v2o_vsa) != FGL_ERR_NO_ERROR) {
		return ERROR;
    }

    if (fglLoadPShader(mov_v2o_psa) != FGL_ERR_NO_ERROR) {
		return ERROR;
    }

	drvsys_clear_buf((unsigned int*)FIMG_COLOR_BUFFER, CLEAR_SCREEN_SIZE, 0);


	fglFlush(FGL_PIPESTATE_ALL);	// Pipeline status

    	fglVSSetAttribNum(2);
    	fglPSSetAttributeNum(1);

	for(j = 1; j < 2; j++)
	{
		fglFlush(FGL_PIPESTATE_ALL);	// Pipeline status
	    
	    	fbCtrlParam.opaqueAlpha = FGL_FALSE;
    		fbCtrlParam.thresholdAlpha = 0xF2; 
    		fbCtrlParam.constAlpha = 0xF3;
	    	fbCtrlParam.dither = FGL_FALSE;
    		fbCtrlParam.format = FGL_PIXEL_ARGB0888;

		if(j == 0) 
		{
			fglSetViewportParams(FGL_TRUE, 0.0, 0.0, 240.0, 320.0, 320.0);

			// Raster Engine SFR set
			fglSetYClip(0x0, 0x140);
			fglSetXClip(0x0, 0xF0);

			// Per-fragment Unit SFR set
			fglSetFrameBufParams(&fbCtrlParam);
			fglSetColorBufBaseAddr(FIMG_COLOR_BUFFER);
			fglSetFrameBufWidth(0xF0);
		}
		else 
		{
			fglSetViewportParams(FGL_TRUE, -60.0, 240.0, 120.0, 160.0, 320.0);
			
			// Raster Engine SFR set
			fglSetYClip(0x0, 0x140);
			fglSetXClip(0x0, 0xF0);

			// Per-fragment Unit SFR set
			fglSetFrameBufParams(&fbCtrlParam);
			fglSetColorBufBaseAddr(FIMG_COLOR_BUFFER);
			fglSetFrameBufWidth(0xF0);

		}

		// Primitive Engine SRF Set
		FGL_Vertex Vtx;
		Vtx.prim = FGL_PRIM_TRIANGLES;
		Vtx.enablePointSize = FGL_FALSE;
		Vtx.numVSOut = 1;
		Vtx.shadeModel = FGL_SHADING_SMOOTH;
		Vtx.colorAttribIdx = 0;
		
		fglSetVertex(&Vtx);

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
		
		unsigned int uiTmpVertices = 6;		
		unsigned int uiDummy = 0xFFFFFFFF;
		fglSendToFIFO(4, &uiTmpVertices);
		fglSendToFIFO(4, &uiDummy);

		unsigned int uDataSize = sizeof(vertex1)/sizeof(vertex1[0]);

 		fglSysTransferToPort((unsigned int *)vertex1, uDataSize);		

		fglFlush(FGL_PIPESTATE_ALL);	// Pipeline status

		uiTmpVertices = 3;		
		uiDummy = 0xFFFFFFFF;
		fglSendToFIFO(4, &uiTmpVertices);
		fglSendToFIFO(4, &uiDummy);

		uDataSize = sizeof(vertex0)/sizeof(vertex0[0]);

 		fglSysTransferToPort((unsigned int *)vertex0, uDataSize);	

		fglFlush(FGL_PIPESTATE_ALL);	// Pipeline status

		// Cache flush
		fglClearCache(FGL_CACHECTL_FLUSH_ZCACHE | FGL_CACHECTL_FLUSH_CCACHE);

		fglSysDelay(50);
		
	}

    	fglSysInit(0.0, 0.0, 240.0, 320.0, 0.0, 1.0);

	return NO_ERROR;
}

