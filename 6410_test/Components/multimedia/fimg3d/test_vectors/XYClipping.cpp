/*******************************************************************************
 *
 *	ViP model for FIMGSE
 *		version 1.0
 *
 *	XYClipping.c
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
 *	$Revision: 1.6 $
 *	$Author: cheolkyoo.kim $
 *	$Date: 2006/05/08 05:37:05 $
 *	$Locker:  $
 *
 *	$Source: C:/CVS/CVSrepository/FIMG-3DSE_SW/fimg3dse_fpga/fimg3d/src/XYClipping.cpp,v $
 *	$State: Exp $
 *	$Log: XYClipping.cpp,v $
 *	Revision 1.6  2006/05/08 05:37:05  cheolkyoo.kim
 *	no message
 *	
 *	Revision 1.5  2006/04/13 11:53:58  cheolkyoo.kim
 *	no message
 *
 *	Revision 1.4  2006/04/06 23:03:16  inhosens.lee
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
 *	Revision 1.4  2006/02/09 10:45:55  cheolkyoo.kim
 *	no message
 *
 *	Revision 1.3  2006/02/06 07:07:08  cheolkyoo.kim
 *	no message
 *
 *	Revision 1.2  2006/02/04 02:21:45  cheolkyoo.kim
 *	Change the SFR value of Host interface attribute control register
 *
 *	Revision 1.1  2006/01/02 01:44:11  cheolkyoo.kim
 *	Raster engine code for RTL test vector
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
int XYClipping(void)

{
	unsigned int uDataSize;

	float box_obj0[] =
	{
		/* x      y      z      w      r      g      b      a */
		-1.0f,  0.1f,  0.0f,  1.0f,  1.0f,  0.0f,  0.0f,  0.5f,
		 1.0f,  0.1f,  0.0f,  1.0f,  1.0f,  0.0f,  0.0f,  0.5f,
		-1.0f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,  0.0f,  0.5f,
		 1.0f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,  0.0f,  0.5f
	};

	float box_obj1[] =
	{
		/* x      y      z      w      r      g      b      a */
		-1.0f, -0.1f,  0.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.5f,
		 1.0f, -0.1f,  0.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.5f,
		-1.0f, -0.2f,  0.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.5f,
		 1.0f, -0.2f,  0.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.5f
	};

	float box_obj2[] =
	{
		/* x      y      z      w      r      g      b      a */
		-0.1f,  1.0f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,  0.5f,
		 0.0f,  1.0f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,  0.5f,
		-0.1f, -1.0f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,  0.5f,
		 0.0f, -1.0f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,  0.5f
	};

	float box_obj3[] =
	{
		/* x      y      z      w      r      g      b      a */
		 0.2f,  1.0f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,  0.5f,
		 0.3f,  1.0f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,  0.5f,
		 0.2f, -1.0f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,  0.5f,
		 0.3f, -1.0f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,  0.5f
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

    //unsigned int read_xclip = READREG(FGRA_CLIP_XCORD);
    //unsigned int read_yclip = READREG(FGRA_CLIP_YCORD);

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
		
		// Draw destination color.
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


		fglFlush(FGL_PIPESTATE_ALL);	// Pipeline status


		// Clipping X Y value set
		// Raster Engine SFR set
		fglSetYClip(0x0, 0x0FF);
		fglSetXClip(0x040, 0x0C0);
//		WRITEREG(FGRA_CLIP_YCORD, 0x00FF0000); // y window coordinate clipping
//		WRITEREG(FGRA_CLIP_XCORD, 0x00C00040); // x window coordinate clipping

		fglSendToFIFO(4, &uiTmpVertices);
		fglSendToFIFO(4, &uiDummy);

		uDataSize = sizeof(box_obj1)/sizeof(box_obj1[0]);

 		fglSysTransferToPort(
								(unsigned int *)box_obj1,
								/*(volatile unsigned int *)(FGHI_FIFO_ENTRY),*/
								uDataSize
							 );


		fglFlush(FGL_PIPESTATE_ALL);	// Pipeline status

		// Clipping X Y value set
		// Raster Engine SFR set
		fglSetYClip(0x0, 0x0FF);
		fglSetXClip(0x0, 0x100);
//		WRITEREG(FGRA_CLIP_YCORD, 0x00FF0000); // y window coordinate clipping
//		WRITEREG(FGRA_CLIP_XCORD, 0x01000000); // x window coordinate clipping

		// Draw destination color.
		fglSendToFIFO(4, &uiTmpVertices);
		fglSendToFIFO(4, &uiDummy);

		uDataSize = sizeof(box_obj2)/sizeof(box_obj2[0]);

 		fglSysTransferToPort(
								(unsigned int *)box_obj2,
								/*(volatile unsigned int *)(FGHI_FIFO_ENTRY),*/
								uDataSize
							 );


		fglFlush(FGL_PIPESTATE_ALL);	// Pipeline status

		// Clipping X Y value set
		// Raster Engine SFR set
		fglSetYClip(0x040, 0x0C0);
		fglSetXClip(0x0, 0x100);
//		WRITEREG(FGRA_CLIP_YCORD, 0x00C00040); // y window coordinate clipping
//		WRITEREG(FGRA_CLIP_XCORD, 0x01000000); // x window coordinate clipping

		fglSendToFIFO(4, &uiTmpVertices);
		fglSendToFIFO(4, &uiDummy);

		uDataSize = sizeof(box_obj3)/sizeof(box_obj3[0]);

 		fglSysTransferToPort(
								(unsigned int *)box_obj3,
								/*(volatile unsigned int *)(FGHI_FIFO_ENTRY),*/
								uDataSize
							 );

		fglFlush(FGL_PIPESTATE_ALL);	// Pipeline status


		// Cache flush
		//fglSysCacheFlush();
		fglClearCache(FGL_CACHECTL_FLUSH_ZCACHE | FGL_CACHECTL_FLUSH_CCACHE);

		fglSysDelay(50);

//		drvsys_clear_buf((unsigned int*)(FIMG_COLOR_BUFFER+m_FBOffsize), CLEAR_SCREEN_SIZE, 0x0);


    }

	// Raster Engine SFR initial value set
	fglSetYClip(0x0, 0x140);
	fglSetXClip(0x0, 0xF0);

    return NO_ERROR;
}
