/*******************************************************************************
 *
 *	ViP model for FIMGSE
 *		version 1.0
 *
 *	Scissor.c
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
 *	$RCSfile: Scissor.cpp,v $
 *	$Revision: 1.7 $
 *	$Author: cheolkyoo.kim $
 *	$Date: 2006/05/08 05:34:27 $
 *	$Locker:  $
 *
 *	$Source: C:/CVS/CVSrepository/FIMG-3DSE_SW/fimg3dse_fpga/fimg3d/src/Scissor.cpp,v $
 *	$State: Exp $
 *	$Log: Scissor.cpp,v $
 *	Revision 1.7  2006/05/08 05:34:27  cheolkyoo.kim
 *	no message
 *	
 *	Revision 1.6  2006/04/13 11:54:17  cheolkyoo.kim
 *	no message
 *
 *	Revision 1.5  2006/04/08 04:17:23  inhosens.lee
 *	updated with fimg_ra code
 *
 *	Revision 1.4  2006/04/05 04:06:09  cheolkyoo.kim
 *	Replaced ~_XCORD and ~_YCORD to ~_XCOORD and ~_YCOORD.
 *
 *	Revision 1.3  2006/04/05 02:25:59  cheolkyoo.kim
 *	Some function are replaced by low-level API in the PF block
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


#include "stdio.h"
#include "library.h"
#include "mov_v2o.vsa.h"
#include "mov_v2o.psa.h"

/****************************************************************************
 *  DEFINES
 ****************************************************************************/
int Scissor(void)
{
	unsigned int uDataSize;

	float attribute[] = {
		/*  x      y      z      w      r      g      b      a */
		-0.3f,  0.3f,  0.5f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
		 0.3f,  0.3f,  0.5f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
		-0.3f, -0.3f,  0.5f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
		 0.3f, -0.3f,  0.5f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f
	};

	float attribute0[] = {
		-0.3f,  0.3f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 0.3f,  0.3f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.3f, -0.3f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 0.3f, -0.3f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f
	};


    if (fglLoadVShader(mov_v2o_vsa) != FGL_ERR_NO_ERROR) 
    {
		UART_Printf("Scissor Test: The LoadVShader fial");
		return ERROR;
    }

    if (fglLoadPShader(mov_v2o_psa) != FGL_ERR_NO_ERROR) 
    {
		UART_Printf("Scissor Test: The LoadPShader fial");
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
	
	//fglPSSetPCRange(unsigned int start, unsigned int end )
    fglPSSetAttributeNum(1);

	// Scissor test main
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

		uDataSize = sizeof(attribute)/sizeof(attribute[0]);

 		fglSysTransferToPort(
								(unsigned int *)attribute,
								/*(volatile unsigned int *)(FGHI_FIFO_ENTRY),*/
								uDataSize
							 );

		fglFlush(FGL_PIPESTATE_ALL);	// Pipeline status

		// Scissor control
		//WRITEREG(FGPF_SCISSOR_XCORD, 0x08880078); // X
		//WRITEREG(FGPF_SCISSOR_YCORD, 0x00880078); // Y

		// Scissor control
		//if(fglSetScissorParams(136, 2048, 136, 120) != FGL_ERR_NO_ERROR)
		if(fglSetScissorParams(130, 110, 170, 150) != FGL_ERR_NO_ERROR)
		{
			UART_Printf("Scissor Test: The fglSetScissorParams fial");
		}

		if(fglEnablePerFragUnit(FGL_PF_SCISSOR, FGL_TRUE) != FGL_ERR_NO_ERROR)
		{
			UART_Printf("Scissor Test: The fglEnablePerFragUnit fial");
		}

		fglSendToFIFO(4, &uiTmpVertices);
		fglSendToFIFO(4, &uiDummy);

		uDataSize = sizeof(attribute0)/sizeof(attribute0[0]);

 		fglSysTransferToPort(
								(unsigned int *)attribute0,
								/*(volatile unsigned int *)(FGHI_FIFO_ENTRY),*/
								uDataSize
							 );

		fglFlush(FGL_PIPESTATE_ALL);	// Pipeline status

		// Cache flush
		//fglSysCacheFlush();
		fglClearCache(FGL_CACHECTL_FLUSH_ZCACHE | FGL_CACHECTL_FLUSH_CCACHE);

		fglSysDelay(50);

    }

	// Scissor test control default value set
	if(fglEnablePerFragUnit(FGL_PF_SCISSOR, FGL_FALSE) != FGL_ERR_NO_ERROR)
	{
		UART_Printf("Scissor Test: The fglEnablePerFragUnit fial");
	}

    return NO_ERROR;
}
