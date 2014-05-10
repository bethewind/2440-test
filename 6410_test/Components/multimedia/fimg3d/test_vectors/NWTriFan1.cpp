/*******************************************************************************
 *
 *	ViP model for FIMGSE
 *		version 1.0
 *
 *	NWTriFan1.c
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
 *	$RCSfile: NWTriFan1.cpp,v $
 *	$Revision: 1.6 $
 *	$Author: cheolkyoo.kim $
 *	$Date: 2006/05/08 05:34:28 $
 *	$Locker:  $
 *
 *	$Source: C:/CVS/CVSrepository/FIMG-3DSE_SW/fimg3dse_fpga/fimg3d/src/NWTriFan1.cpp,v $
 *	$State: Exp $
 *	$Log: NWTriFan1.cpp,v $
 *	Revision 1.6  2006/05/08 05:34:28  cheolkyoo.kim
 *	no message
 *	
 *	Revision 1.5  2006/04/13 11:54:17  cheolkyoo.kim
 *	no message
 *
 *	Revision 1.4  2006/04/08 04:00:10  inhosens.lee
 *	updated with fimg_ra code
 *
 *	Revision 1.3  2006/04/05 04:06:08  cheolkyoo.kim
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

int NWTriFan1(void)

{
	unsigned int uDataSize;

	static const float attributes[] =
	{
		//  x      y      z      w      r      g      b      a
		 0.0f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,
		-0.3f, -0.5f, -0.5f,  1.0f,  0.0f,  1.0f,  0.0f,  1.0f,
		 0.3f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.2f, -1.2f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f, -1.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		-0.0f,  0.5f,  0.9f, -1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
		-0.0f, -0.5f,  0.3f, -1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
		 0.5f,  0.2f,  1.2f,  1.0f,  0.0f,  1.0f,  0.0f,  1.0f,
		 0.3f,  0.5f,  1.2f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		-0.3f,  0.5f,  0.5f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f
	};

    if (fglLoadVShader(mov_v2o_vsa) != FGL_ERR_NO_ERROR) {
		return ERROR;
    }

    if (fglLoadPShader(mov_v2o_psa) != FGL_ERR_NO_ERROR) {
		return ERROR;
    }

	drvsys_clear_buf((unsigned int*)(FIMG_COLOR_BUFFER), CLEAR_SCREEN_SIZE, 0x0);


	fglFlush(FGL_PIPESTATE_ALL);	// Pipeline status


/*
	// Vertex Shader SFR set
	WRITEREG(FGVS_ATTRIB_NUM, 0x00020002); // Attribute counter of VS input & output
	WRITEREG(FGVS_CONFIG, 0x00000001); // Configuration register

	// Pixel Shader SFR set
	WRITEREG(FGPS_EXE_MODE, 0x00000000); // Pixel shader execution mode control register
	WRITEREG(FGPS_PC_COPY, 0x00000001); // Copy PSPCS_ADDR value to program counter
	WRITEREG(FGPS_ATTRIB_NUM, 0x00000001); // Number of attribute of current context
	fglSysPollingMask(FGPS_INBUF_STATUS, 0x0, 0x00000001); // Status signal of pixel shader input buffer
	WRITEREG(FGPS_EXE_MODE, 0x00000001); // Pixel shader execution mode control register
*/
    fglVSSetAttribNum(2);
    fglPSSetAttributeNum(1);

	//for(j = 0; j < 2; j++)
	{
		// Primitive Engine SFR Set
		FGL_Vertex Vtx;
		Vtx.prim = FGL_PRIM_TRIANGLE_FAN;
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
		
		unsigned int uiTmpVertices = 0xA;		
		unsigned int uiDummy = 0xFFFFFFFF;
		fglSendToFIFO(4, &uiTmpVertices);
		fglSendToFIFO(4, &uiDummy);

		uDataSize = sizeof(attributes)/sizeof(attributes[0]);

		fglSysTransferToPort(
								(unsigned int *)attributes,
								/*(volatile unsigned int *)(FGHI_FIFO_ENTRY),*/
								uDataSize
							 );

		fglFlush(FGL_PIPESTATE_ALL);	// Pipeline status

		// Cache flush
		//fglSysCacheFlush();
		fglClearCache(FGL_CACHECTL_FLUSH_ZCACHE | FGL_CACHECTL_FLUSH_CCACHE);

		fglSysDelay(50);

		drvsys_clear_buf((unsigned int*)(FIMG_COLOR_BUFFER), CLEAR_SCREEN_SIZE, 0x39E739E7);

	}

    return NO_ERROR;
}
