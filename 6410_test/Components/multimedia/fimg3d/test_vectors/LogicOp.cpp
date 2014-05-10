/*******************************************************************************
 *
 *	ViP model for FIMGSE
 *		version 1.0
 *
 *	LogicOp.c
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
 *	$RCSfile: LogicOp.cpp,v $
 *	$Revision: 1.8 $
 *	$Author: cheolkyoo.kim $
 *	$Date: 2006/05/08 05:34:28 $
 *	$Locker:  $
 *
 *	$Source: C:/CVS/CVSrepository/FIMG-3DSE_SW/fimg3dse_fpga/fimg3d/src/LogicOp.cpp,v $
 *	$State: Exp $
 *	$Log: LogicOp.cpp,v $
 *	Revision 1.8  2006/05/08 05:34:28  cheolkyoo.kim
 *	no message
 *	
 *	Revision 1.7  2006/04/13 11:54:26  cheolkyoo.kim
 *	no message
 *
 *	Revision 1.6  2006/04/08 03:52:46  inhosens.lee
 *	updated with fimg_ra code
 *
 *	Revision 1.5  2006/04/05 04:06:08  cheolkyoo.kim
 *	Replaced ~_XCORD and ~_YCORD to ~_XCOORD and ~_YCOORD.
 *
 *	Revision 1.4  2006/04/05 03:31:23  cheolkyoo.kim
 *	Modify FGPE_VIEWPORT_CENTER_XCOORD to FGPE_VIEWPORT_CENTER_XCOORD
 *
 *	Revision 1.3  2006/04/05 02:25:59  cheolkyoo.kim
 *	Some function are replaced by low-level API in the PF block
 *
 *	Revision 1.2  2006/03/30 04:27:43  cheolkyoo.kim
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


int LogicOp(void)

{
	unsigned int j;
	unsigned int uDataSize;

#if 0
	float box_obj0[] =
	{
		/* x      y      z      w      r      g      b      a */
		-0.1f, -0.1f,  0.0f,  1.0f,  1.0f,  0.0f,  0.0f,  0.5f,
		 0.1f, -0.1f,  0.0f,  1.0f,  1.0f,  0.0f,  0.0f,  0.5f,
		-0.1f,  0.1f,  0.0f,  1.0f,  1.0f,  0.0f,  0.0f,  0.5f,
		 0.1f,  0.1f,  0.0f,  1.0f,  1.0f,  0.0f,  0.0f,  0.5f
	};

	float box_obj1[] =
	{
		/* x      y      z      w      r      g      b      a */
		 0.0f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.5f,
		 0.2f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.5f,
		 0.0f,  0.2f,  0.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.5f,
		 0.2f,  0.2f,  0.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.5f
	};
#endif

    float VERTICES[5] = { -0.75f, 0.75f, 0.93f, 0.67f, 0.34f };

	unsigned int background[] =
	{
		0xFFFFFFFF, 0xF806F806, 0xF81FF81F, 0xFA26FA26, 0xF980F980, 0xFCCCFCCC
	};

    if (fglLoadVShader(mov_v2o_vsa) != FGL_ERR_NO_ERROR) {
		return ERROR;
    }

    if (fglLoadPShader(mov_v2o_psa) != FGL_ERR_NO_ERROR) {
		return ERROR;
    }

	drvsys_clear_buf((unsigned int*)(FIMG_COLOR_BUFFER), CLEAR_SCREEN_SIZE, background[3]);


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

	// LogicOp test main
	{

#if 0
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
#endif
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
		
		for(j = 0; j < 16;  j++)
		{
#if 0
			// Draw destination color.
			unsigned int uiTmpVertices = 4;		
			unsigned int uiDummy = 0xFFFFFFFF;
			fglSendToFIFO(4, &uiTmpVertices);
			fglSendToFIFO(4, &uiDummy);

			uDataSize = sizeof(box_obj0)/sizeof(box_obj0[0]);

			fglSysTransferToPort(
									(unsigned int *)box_obj0,
									(volatile unsigned int *)(FGHI_FIFO_ENTRY),
									uDataSize
								 );

			fglSysPollingMask(FG_PIPELINE_STATUS, 0x0, 0x00033717); // Pipeline status
#endif
			// Draw source color.
			// Logical operation control register
			//fglSysModifyReg(FGPF_LOGIC_OP, 0x000001FF, (j<<1)|(j<<5)|0x1);

			if(fglSetLogicalOpParams((FGL_LogicalOp)j) != FGL_ERR_NO_ERROR)
			{
				UART_Printf("\nLogical Operation Test: The fglSetBlendParams fial");
			}

			if(fglEnablePerFragUnit(FGL_PF_LOGICALOP, FGL_TRUE) != FGL_ERR_NO_ERROR)
			{
				UART_Printf("\nLogical Operation Test: The fglEnablePerFragUnit fial");
			}


#if 0
			unsigned int uiTmpVertices = 4;		
			unsigned int uiDummy = 0xFFFFFFFF;
			fglSendToFIFO(4, &uiTmpVertices);
			fglSendToFIFO(4, &uiDummy);
			
			uDataSize = sizeof(box_obj1)/sizeof(box_obj1[0]);

			fglSysTransferToPort(
									(unsigned int *)box_obj1,
									/*(volatile unsigned int *)(FGHI_FIFO_ENTRY),*/
									uDataSize
								 );
#endif
			unsigned int uiTmpVertices = 1;		
			unsigned int uiDummy = 0xFFFFFFFF;
			fglSendToFIFO(4, &uiTmpVertices);
			fglSendToFIFO(4, &uiDummy);

            uDataSize = sizeof(VERTICES)/sizeof(VERTICES[0]);

			fglSysTransferToPort(
									(unsigned int *)VERTICES,
									/*(volatile unsigned int *)(FGHI_FIFO_ENTRY),*/
									uDataSize
								 );

			fglFlush(FGL_PIPESTATE_ALL);	// Pipeline status

		    if(((j+1) % 4) == 0) {
		        VERTICES[0] = -0.75f;
		        VERTICES[1] -= 0.5f;
		    }
		    else {
		        VERTICES[0] += 0.5f;
		    }


			fglFlush(FGL_PIPESTATE_ALL);	// Pipeline status

			// Cache flush
			//fglSysCacheFlush();
			fglClearCache(FGL_CACHECTL_FLUSH_ZCACHE | FGL_CACHECTL_FLUSH_CCACHE);
		//	fglSysDelay(50);
		}
    }

	//drvsys_clear_buf((unsigned int*)(FIMG_COLOR_BUFFER), CLEAR_SCREEN_SIZE, background[j%6]);


	if(fglEnablePerFragUnit(FGL_PF_LOGICALOP, FGL_FALSE) != FGL_ERR_NO_ERROR)
	{
		UART_Printf("\nLogical Operation Test: The fglEnablePerFragUnit fial");
	}

    return NO_ERROR;
}

