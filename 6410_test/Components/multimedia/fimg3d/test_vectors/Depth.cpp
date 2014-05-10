/*******************************************************************************
 *
 *	ViP model for FIMGSE
 *		version 1.0
 *
 *	Depth.c
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
 *	$RCSfile: Depth.cpp,v $
 *	$Revision: 1.7 $
 *	$Author: cheolkyoo.kim $
 *	$Date: 2006/05/08 05:34:29 $
 *	$Locker:  $
 *
 *	$Source: C:/CVS/CVSrepository/FIMG-3DSE_SW/fimg3dse_fpga/fimg3d/src/Depth.cpp,v $
 *	$State: Exp $
 *	$Log: Depth.cpp,v $
 *	Revision 1.7  2006/05/08 05:34:29  cheolkyoo.kim
 *	no message
 *	
 *	Revision 1.6  2006/04/13 11:53:45  cheolkyoo.kim
 *	no message
 *
 *	Revision 1.5  2006/04/08 03:49:11  inhosens.lee
 *	updated with fimg_ra code
 *
 *	Revision 1.4  2006/04/05 04:06:07  cheolkyoo.kim
 *	Replaced ~_XCORD and ~_YCORD to ~_XCOORD and ~_YCOORD.
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

int Depth(void)

{

	unsigned int uDataSize;

#if 0
	float attributes[] =
	{
		0.0f,   0.1f,  0.5f,  1.0f, 1.0f,  0.0f,  0.0f,   1.0f,
		0.1f,   0.0f,  0.5f,  1.0f,	1.0f,  0.0f,  0.0f,   1.0f,
		0.0f,  -0.1f,  0.5f,  1.0f,	1.0f,  0.0f,  0.0f,   1.0f,
		0.05f,  0.1f,  0.7f,  1.0f, 1.0f,  0.94f, 0.24f,  0.3f,
		0.15f,  0.0f,  0.7f,  1.0f,	1.0f,  0.94f, 0.24f,  0.3f,
		0.05f, -0.1f,  0.7f,  1.0f,	1.0f,  0.94f, 0.24f,  0.3f,
		0.1f,   0.1f,  0.1f,  1.0f,	1.0f,  0.24f, 0.068f, 1.0f,
		0.2f,   0.0f,  0.1f,  1.0f,	1.0f,  0.24f, 0.068f, 1.0f,
		0.1f,  -0.1f,  0.1f,  1.0f,	1.0f,  0.24f, 0.068f, 1.0f
	};
#endif

    float VERTICES[60];
    
    for(int i = 0; i < 10; i++)
    {
        VERTICES[i*6]   = -0.9f + ((float)i * 0.2f);           
        VERTICES[i*6+1] = 0.9f;           
        VERTICES[i*6+2] = 1.0f - ((float)i * 0.2f);           
        VERTICES[i*6+3] = 0.1f;           
        VERTICES[i*6+4] = 0.1f;           
        VERTICES[i*6+5] = 0.95f - ((float)i * 0.08f);           
    }

    if (fglLoadVShader(mov_v2o_vsa) != FGL_ERR_NO_ERROR) {
		return ERROR;
    }

    if (fglLoadPShader(mov_v2o_psa) != FGL_ERR_NO_ERROR) {
		return ERROR;
    }

	//drvsys_clear_buf((unsigned int*)FIMG_DEPTH_BUFFER, CLEAR_SCREEN_SIZE, 0xFF7FFFFF);
	drvsys_clear_buf((unsigned int*)FIMG_DEPTH_BUFFER, CLEAR_SCREEN_SIZE, 0xFF800000);
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

	// Depth test main
	{
		// Primitive Engine SRF Set
		FGL_Vertex Vtx;
		Vtx.prim = FGL_PRIM_POINTS;
		Vtx.enablePointSize = FGL_FALSE;
		Vtx.numVSOut = 1;
		Vtx.shadeModel = FGL_SHADING_SMOOTH;
		Vtx.colorAttribIdx = 0;
		
		fglSetVertex(&Vtx); // Vertex context register

		// Point size set
		fglSetPointWidth(24.0f);
		fglSetMinimumPointWidth(1.0f);
		fglSetMaximumPointWidth(1024.0f);

		// Depth/Stencil buffer base address
		fglSetZBufBaseAddr(FIMG_DEPTH_BUFFER);

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
		fglSetAttribute(1, &HIAttr);
		
		for(int j=0; j<8; j++)
		{
			// Depth test function & enable set
			if(fglSetDepthParams((FGL_CompareFunc)j) != FGL_ERR_NO_ERROR) {
			//if(fglSetDepthParams(FGL_COMP_LESS) != FGL_ERR_NO_ERROR) {
				UART_Printf("\nDepth buffer test: The fglSetAlphaParams fial");
			}

			if(fglEnablePerFragUnit(FGL_PF_DEPTH, FGL_TRUE) != FGL_ERR_NO_ERROR) {
				UART_Printf("\nDepth buffer test: The fglEnablePerFragUnit fial");
			}

			unsigned int uiTmpVertices = 0xA;		
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
			
            for(int i = 0; i < 10; i++)
            {
                VERTICES[i*6+1] -= 0.15f;           
            }
    	}
    }

	// Cache flush
	//fglSysCacheFlush();
	fglClearCache(FGL_CACHECTL_FLUSH_ZCACHE | FGL_CACHECTL_FLUSH_CCACHE);
	//fglSysDelay(50);
    
	drvsys_clear_buf((unsigned int*)FIMG_DEPTH_BUFFER, CLEAR_SCREEN_SIZE, 0x0);
	//drvsys_clear_buf((unsigned int*)(FIMG_COLOR_BUFFER), CLEAR_SCREEN_SIZE, 0x0);


	fglEnablePerFragUnit(FGL_PF_DEPTH, FGL_FALSE);

	const char* pFileName_SFR_Dump = "Depth_SFR.txt";
	fglDumpContext(pFileName_SFR_Dump);

    return NO_ERROR;
}

