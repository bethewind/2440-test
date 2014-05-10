/*******************************************************************************
 *
 *	ViP model for FIMGSE
 *		version 1.0
 *
 *	SmallTri.cpp
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
 *	2005. 12. 21	by cheolkyoo.kim
 *
 *  Description
 *
 *	$RCSfile: SmallTri.cpp,v $
 *	$Revision: 1.6 $
 *	$Author: cheolkyoo.kim $
 *	$Date: 2006/05/08 05:34:27 $
 *	$Locker:  $
 *
 *	$Source: C:/CVS/CVSrepository/FIMG-3DSE_SW/fimg3dse_fpga/fimg3d/src/SmallTri.cpp,v $
 *	$State: Exp $
 *	$Log: SmallTri.cpp,v $
 *	Revision 1.6  2006/05/08 05:34:27  cheolkyoo.kim
 *	no message
 *	
 *	Revision 1.5  2006/04/13 11:54:17  cheolkyoo.kim
 *	no message
 *
 *	Revision 1.4  2006/04/08 04:18:34  inhosens.lee
 *	updated with fimg_ra code
 *
 *	Revision 1.3  2006/04/05 04:06:09  cheolkyoo.kim
 *	Replaced ~_XCORD and ~_YCORD to ~_XCOORD and ~_YCOORD.
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


#include "mov_v2o.vsa.h"
#include "mov_v2o.psa.h"

/****************************************************************************
 *  DEFINES
 ****************************************************************************/
int SmallTri(void)
{
	unsigned int uDataSize;

	float object0[] = {
		-0.9f,  0.1f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		-0.7f,  0.1f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		-0.9f, -0.1f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		-0.7f, -0.1f,  0.5f,  1.0f,	 0.0f,  0.0f,  0.0f,  0.0f
	};

	float object1[] = {
		-0.55f,  0.05f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		-0.45f,  0.05f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		-0.55f, -0.05f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		-0.45f, -0.05f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f
	};

	float object2[] = {
		-0.225f,  0.025f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		-0.175f,  0.025f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		-0.225f, -0.025f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		-0.175f, -0.025f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f
	};


	float object3[] = {
		0.0875f,  0.0125f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		0.1125f,  0.0125f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		0.0875f, -0.0125f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		0.1125f, -0.0125f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f
	};

	float object4[] = {
		0.39375f,  0.00625f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		0.40625f,  0.00625f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		0.39375f, -0.00625f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		0.40625f, -0.00625f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f
	};

	float object5[] = {
		0.699999f,  0.000001f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		0.700001f,  0.000001f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		0.699999f, -0.000001f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		0.700001f, -0.000001f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f
	};

    if (fglLoadVShader(mov_v2o_vsa) != FGL_ERR_NO_ERROR) {
		return ERROR;
    }

    if (fglLoadPShader(mov_v2o_psa) != FGL_ERR_NO_ERROR) {
		return ERROR;
    }

	//drvsys_clear_buf((unsigned int*)(FIMG_COLOR_BUFFER), CLEAR_SCREEN_SIZE, 0x07FF07FF);
	drvsys_clear_buf((unsigned int*)(FIMG_COLOR_BUFFER), CLEAR_SCREEN_SIZE, 0x00FFFF00);

	fglFlush(FGL_PIPESTATE_ALL);	// Pipeline status
	
	fglVSSetAttribNum(2);
	fglPSSetAttributeNum(1);

 	// Small Triangle main
 	{

		// Primitive Engine SRF Set
		FGL_Vertex Vtx;
		Vtx.prim = FGL_PRIM_TRIANGLE_STRIP;
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
		HIAttr.bEndFlag = FGL_TRUE;
		fglSetAttribute(1, &HIAttr);
		
		unsigned int uiTmpVertices = 4;		
		unsigned int uiDummy = 0xFFFFFFFF;
		fglSendToFIFO(4, &uiTmpVertices);
		fglSendToFIFO(4, &uiDummy);

		uDataSize = sizeof(object0)/sizeof(object0[0]);

 		fglSysTransferToPort(
								(unsigned int *)object0,
								/*(volatile unsigned int *)(FGHI_FIFO_ENTRY),*/
								uDataSize
							 );

		fglSendToFIFO(4, &uiTmpVertices);
		fglSendToFIFO(4, &uiDummy);

		uDataSize = sizeof(object1)/sizeof(object1[0]);

 		fglSysTransferToPort(
								(unsigned int *)object1,
								/*(volatile unsigned int *)(FGHI_FIFO_ENTRY),*/
								uDataSize
							 );
		fglSendToFIFO(4, &uiTmpVertices);
		fglSendToFIFO(4, &uiDummy);


		uDataSize = sizeof(object2)/sizeof(object2[0]);

 		fglSysTransferToPort(
								(unsigned int *)object2,
								/*(volatile unsigned int *)(FGHI_FIFO_ENTRY),*/
								uDataSize
							 );
		fglSendToFIFO(4, &uiTmpVertices);
		fglSendToFIFO(4, &uiDummy);

		uDataSize = sizeof(object3)/sizeof(object3[0]);

 		fglSysTransferToPort(
								(unsigned int *)object3,
								/*(volatile unsigned int *)(FGHI_FIFO_ENTRY),*/
								uDataSize
							 );
		fglSendToFIFO(4, &uiTmpVertices);
		fglSendToFIFO(4, &uiDummy);

		uDataSize = sizeof(object4)/sizeof(object4[0]);

 		fglSysTransferToPort(
								(unsigned int *)object4,
								/*(volatile unsigned int *)(FGHI_FIFO_ENTRY),*/
								uDataSize
							 );

		fglSendToFIFO(4, &uiTmpVertices);
		fglSendToFIFO(4, &uiDummy);

		uDataSize = sizeof(object5)/sizeof(object5[0]);

 		fglSysTransferToPort(
								(unsigned int *)object5,
								/*(volatile unsigned int *)(FGHI_FIFO_ENTRY),*/
								uDataSize
							 );

		fglFlush(FGL_PIPESTATE_ALL);	// Pipeline status

		// Cache flush
		//fglSysCacheFlush();
		fglClearCache(FGL_CACHECTL_FLUSH_ZCACHE | FGL_CACHECTL_FLUSH_CCACHE);

 	}

    return NO_ERROR;
}
