/*******************************************************************************
 *
 *	ViP model for FIMGSE
 *		version 1.0
 *
 *	ProgramCtrl.c
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
 *	2006. 1. 26  by cheolkyoo.kim
 *
 *  Description
 *
 *	$RCSfile: ProgramCtrl.cpp,v $
 *	$Revision: 1.6 $
 *	$Author: cheolkyoo.kim $
 *	$Date: 2006/05/08 05:34:28 $
 *	$Locker:  $
 *
 *	$Source: C:/CVS/CVSrepository/FIMG-3DSE_SW/fimg3dse_fpga/fimg3d/src/ProgramCtrl.cpp,v $
 *	$State: Exp $
 *	$Log: ProgramCtrl.cpp,v $
 *	Revision 1.6  2006/05/08 05:34:28  cheolkyoo.kim
 *	no message
 *	
 *	Revision 1.5  2006/04/13 11:54:17  cheolkyoo.kim
 *	no message
 *
 *	Revision 1.4  2006/04/10 05:34:49  inhosens.lee
 *	updated with fimg_ra and fimg_tu codes
 *
 *	Revision 1.3  2006/04/05 04:06:09  cheolkyoo.kim
 *	Replaced ~_XCORD and ~_YCORD to ~_XCOORD and ~_YCOORD.
 *
 *	Revision 1.2  2006/03/30 04:27:44  cheolkyoo.kim
 *	Thomas-20060330@comment: Add to code using directive WIN32_VIP for VIP
 *
 *	Revision 1.1  2006/03/10 08:29:08  cheolkyoo.kim
 *	Initial import of FIMG-3DSE_SW package
 *
 *
 ******************************************************************************/
/****************************************************************************
 *  INCLUDES
 ****************************************************************************/
#include "Fimg3DTest.h"


#include "ProgramCtrl.vsa.h"
#include "ProgramCtrl.psa.h"

/****************************************************************************
 *  DEFINES
 ****************************************************************************/
int ProgramCtrl(void)
{
 	unsigned int uDataSize;
	unsigned int j;
	FGL_TexUnitParams tuParams;

	float attributes[6][20] =
	{
		/*      x       y     z  	 s      t   */
		{   -1.0f,  1.0f,  0.5f,  0.0f,  0.0f,
    		 0.0f,  1.0f,  0.5f,  1.0f,  0.0f,
    		-1.0f,  0.3f,  0.5f,  0.0f,  1.0f,
    		 0.0f,  0.3f,  0.5f,  1.0f,  1.0f  },

		{    0.0f,  1.0f,  0.5f,  0.0f,  0.0f,
    		 1.0f,  1.0f,  0.5f,  1.0f,  0.0f,
    		 0.0f,  0.3f,  0.5f,  0.0f,  1.0f,
    		 1.0f,  0.3f,  0.5f,  1.0f,  1.0f  },

		{   -1.0f,  0.3f,  0.5f,  0.0f,  0.0f,
    		 0.0f,  0.3f,  0.5f,  1.0f,  0.0f,
    		-1.0f, -0.4f,  0.5f,  0.0f,  1.0f,
    		 0.0f, -0.4f,  0.5f,  1.0f,  1.0f  },

		{    0.0f,  0.3f,  0.5f,  0.0f,  0.0f,
    		 1.0f,  0.3f,  0.5f,  1.0f,  0.0f,
    		 0.0f, -0.4f,  0.5f,  0.0f,  1.0f,
    		 1.0f, -0.4f,  0.5f,  1.0f,  1.0f  },

		{   -1.0f, -0.4f,  0.5f,  0.0f,  0.0f,
    		 0.0f, -0.4f,  0.5f,  1.0f,  0.0f,
    		-1.0f, -1.0f,  0.5f,  0.0f,  1.0f,
    		 0.0f, -1.0f,  0.5f,  1.0f,  1.0f  },

		{    0.0f, -0.4f,  0.5f,  0.0f,  0.0f,
    		 1.0f, -0.4f,  0.5f,  1.0f,  0.0f,
    		 0.0f, -1.0f,  0.5f,  0.0f,  1.0f,
    		 1.0f, -1.0f,  0.5f,  1.0f,  1.0f  }
	};


	unsigned int PSAddrTable[] =
	{  /* Start addr, End Addr */
		 0,  2,
		 3,  6,
		 7, 12,
		13, 18,
		19, 21,
		22, 22
	};

    if (fglLoadVShader(ProgramCtrl_vsa) != FGL_ERR_NO_ERROR) {
		return ERROR;
    }

    if (fglLoadPShader(ProgramCtrl_psa) != FGL_ERR_NO_ERROR) {
		return ERROR;
    }

	drvsys_clear_buf((unsigned int*)(FIMG_COLOR_BUFFER), CLEAR_SCREEN_SIZE, 0x0);
	fglFlush(FGL_PIPESTATE_ALL);	// Pipeline status
/*
	// Vertex Shader SFR set
	WRITEREG(FGVS_ATTRIB_NUM, 0x00020002); // Attribute counter of VS input & output
	WRITEREG(FGVS_CONFIG, 0x1); // Configuration register
	WRITEREG(FGVS_OUT_ATTRIB_IDX0, 0x03020100); // Output attribute index control

	// Pixel Shader SFR set
	WRITEREG(FGPS_EXE_MODE, 0x0); // Pixel shader execution mode control register
	WRITEREG(FGPS_PC_COPY, 0x1); // Copy PSPCS_ADDR value to program counter
	WRITEREG(FGPS_ATTRIB_NUM, 0x1); // Number of attribute of current context
	fglSysPollingMask(FGPS_INBUF_STATUS, 0x0, 0x1); // Status signal of pixel shader input buffer
	WRITEREG(FGPS_EXE_MODE, 0x1); // Pixel shader execution mode control register
*/
    fglSetVShaderAttribTable(FGL_OUTPUT_ATTRIB_IDX0, 0x03020100);
    fglSetVShaderAttribTable(FGL_OUTPUT_ATTRIB_IDX1, 0x07060504);
    fglSetVShaderAttribTable(FGL_OUTPUT_ATTRIB_IDX2, 0x0B0A0908);
    fglVSSetAttribNum(2);
    fglPSSetAttributeNum(1);

    // Texture format rendering test main
    {
		// Primitive Engine SRF Set
		FGL_Vertex Vtx;
		Vtx.prim = FGL_PRIM_TRIANGLE_STRIP;
		Vtx.enablePointSize = FGL_FALSE;
		Vtx.numVSOut = 1;
		Vtx.shadeModel = FGL_SHADING_SMOOTH;
		Vtx.colorAttribIdx = 0;
		
		fglSetVertex(&Vtx); // Vertex context register
		// Raster SFR set
		//fglSetLODControl(0x00000038) );
		fglSetLODRegister(FGL_LODCOEFF_DISABLE,
					FGL_LODCOEFF_ENABLE_ALL,
					FGL_LODCOEFF_DISABLE,
					FGL_LODCOEFF_DISABLE,
					FGL_LODCOEFF_DISABLE,
					FGL_LODCOEFF_DISABLE,
					FGL_LODCOEFF_DISABLE,
					FGL_LODCOEFF_DISABLE);

    	// Texture Unit SFR Setting
    	tuParams.eType = FGL_TEX_2D;
    	tuParams.eColorkey = FGL_CKEY_DISABLE;
    	tuParams.bUseExpansion = FGL_FALSE;
    	tuParams.ePaletteFormat = FGL_PALETTE_ARGB8888;
    	tuParams.eFormat = FGL_TEXEL_ARGB8888;
    	tuParams.eUMode = FGL_TEX_WRAP_REPEAT;
    	tuParams.eVMode = FGL_TEX_WRAP_REPEAT;
    	tuParams.bIsNonparametric = FGL_FALSE;
    	//tuParams.bUseMagFilter = FGL_TRUE;
    	//tuParams.bUseMinFilter = FGL_TRUE;
    	tuParams.bUseMagFilter = FGL_FALSE;
    	tuParams.bUseMinFilter = FGL_FALSE;
     	tuParams.eMipMapFilter = FGL_FILTER_NEAREST;
    	//tuParams.bUseMinMap = FGL_FALSE;
    	//tuParams.eMipMapFilter = FGL_FILTER_DISABLE;
    	tuParams.uUSize = 256;
    	tuParams.uVSize = 256;
    	tuParams.uPSize = 1;
// Sampler0   
    	fglSetTexUnitParams(0, &tuParams);
    	fglSetTexBaseAddr(0, FIMG_TEXTURE_MEMORY);
//Sampler1
		fglSetTexUnitParams(1, &tuParams);
		fglSetTexBaseAddr(1, FIMG_TEXTURE_MEMORY + 0x55554);

		for (j = 0; j < 6; j++)
		{
/*
			WRITEREG(FGPS_EXE_MODE, 0x0);			    // Pixel shader execution mode control register
			WRITEREG(FGPS_PC_START, PSAddrTable[j*2]);		// Start address of pixel shader program
			WRITEREG(FGPS_PC_END, PSAddrTable[j*2+1]);	// End address of pixel shader program
			WRITEREG(FGPS_PC_COPY, 0x1);			    // Copy PSPCS_ADDR value to program counter
	        WRITEREG(FGPS_ATTRIB_NUM, 0x1);			
			fglSysPollingMask(FGPS_INBUF_STATUS, 0x0, 0x1);// Status signal of pixel shader input buffer
			WRITEREG(FGPS_EXE_MODE, 0x1);			    // Pixel shader execution mode control register
*/
            fglPSSetPCRange(PSAddrTable[j*2], PSAddrTable[j*2+1]);
            fglPSSetAttributeNum(1);

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
			HIAttr.numComp = 2;
			HIAttr.bEndFlag = FGL_TRUE;
			fglSetAttribute(1, &HIAttr);
			
			unsigned int uiTmpVertices = 4;		
			unsigned int uiDummy = 0xFFFFFFFF;
			fglSendToFIFO(4, &uiTmpVertices);
			fglSendToFIFO(4, &uiDummy);

			uDataSize = sizeof(attributes[0])/sizeof(attributes[0][0]);

	 		fglSysTransferToPort(
									(unsigned int *)attributes[j],
									/*(volatile unsigned int *)(FGHI_FIFO_ENTRY),*/
									uDataSize
								 );

			fglFlush(FGL_PIPESTATE_ALL);	// Pipeline status
		}

		// Cache flush
		//fglSysCacheFlush();
		//fglSysCacheClear();
		fglClearCache(FGL_CACHECTL_INIT_ALL);

		fglSysDelay(100);
    }
    
	fglSetLODControl(0x0);

    return NO_ERROR;
}

