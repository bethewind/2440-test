/*******************************************************************************
 *
 *	ViP model for FIMGSE
 *		version 1.0
 *
 *	DepthOffset0.c
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
 *	$RCSfile: DepthOffset0.cpp,v $
 *	$Revision: 1.6 $
 *	$Author: cheolkyoo.kim $
 *	$Date: 2006/05/08 05:34:29 $
 *	$Locker:  $
 *
 *	$Source: C:/CVS/CVSrepository/FIMG-3DSE_SW/fimg3dse_fpga/fimg3d/src/DepthOffset0.cpp,v $
 *	$State: Exp $
 *	$Log: DepthOffset0.cpp,v $
 *	Revision 1.6  2006/05/08 05:34:29  cheolkyoo.kim
 *	no message
 *	
 *	Revision 1.5  2006/04/13 11:53:45  cheolkyoo.kim
 *	no message
 *
 *	Revision 1.4  2006/04/07 00:44:47  inhosens.lee
 *	updated with fimg_ra code
 *
 *	Revision 1.3  2006/04/05 04:06:08  cheolkyoo.kim
 *	Replaced ~_XCORD and ~_YCORD to ~_XCOORD and ~_YCOORD.
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
//#include "Debug.h"

#include "Fimg3DTest.h"


#include "mov_v4o.vsa.h"
#include "mov_v2o.psa.h"
//#include "Sphere.h"

/****************************************************************************
 *  DEFINES
 ****************************************************************************/
int DepthOffset(void)
{
	if (fglLoadVShader(mov_v4o_vsa) != FGL_ERR_NO_ERROR) {
		return ERROR;
    }

    if (fglLoadPShader(mov_v2o_psa) != FGL_ERR_NO_ERROR) {
		return ERROR;
    }

	drvsys_clear_buf((unsigned int*)FIMG_DEPTH_BUFFER, CLEAR_SCREEN_SIZE, 0xFFFFFFFF);
	drvsys_clear_buf((unsigned int*)(FIMG_COLOR_BUFFER), CLEAR_SCREEN_SIZE, 0x0);

	fglFlush(FGL_PIPESTATE_ALL);	// Pipeline status

	Matrix4 matMVP, matProj;
	matProj.SetAsOrthoMatrix(-2.4, 2.4, -3.2, 3.2, 0.0, 100.0);
	matMVP.Translate(0, 0, -1);
	matMVP = matProj * matMVP;
    
    //unsigned int ConstFloatAddr = FGVS_CFLOAT_SADDR;
    //ConstFloatAddr = matMVP.WriteMatrix4(ConstFloatAddr);
    
    unsigned int Offset = fglWriteVertexShaderConstFloat(0, 16, matMVP.m[0]);
/*
	// Vertex Shader SFR set
	WRITEREG(FGVS_ATTRIB_NUM, 0x00020004); // Attribute counter of VS output & input
	WRITEREG(FGVS_CONFIG, 0x00000001); // Configuration register
	WRITEREG(FGVS_OUT_ATTRIB_IDX0, 0x03020100);

	// Pixel Shader SFR set
	WRITEREG(FGPS_EXE_MODE, 0x0); // Pixel shader execution mode control register
	WRITEREG(FGPS_PC_COPY, 0x1); // Copy PSPCS_ADDR value to program counter
	WRITEREG(FGPS_ATTRIB_NUM, 0x1); // Number of attribute of current context
	fglSysPollingMask(FGPS_INBUF_STATUS, 0x0, 0x1); // Status signal of pixel shader input buffer
	WRITEREG(FGPS_EXE_MODE, 0x1); // Pixel shader execution mode control register
*/
    fglSetVShaderAttribTable(FGL_OUTPUT_ATTRIB_IDX0, 0x02020100);
    fglSetVShaderAttribTable(FGL_OUTPUT_ATTRIB_IDX1, 0x07060504);
    fglSetVShaderAttribTable(FGL_OUTPUT_ATTRIB_IDX2, 0x0B0A0908);

    fglVSSetAttribNum(4);
    fglPSSetAttributeNum(1);

	// Per-fragment Unit SFR set
	//WRITEREG(FGPF_DEPTHBUF_ADDR, FIMG_DEPTH_BUFFER); // Depth/Stencil buffer base address
	//unsigned int uRegVal = READREG(FGPF_DEPTH);
	//WRITEREG(FGPF_DEPTH, 0x5); // Depth test LESS function & enable set
	fglSetZBufBaseAddr(FIMG_DEPTH_BUFFER);
	fglSetDepthParams(FGL_COMP_LESS);
	//fglSetDepthParams(FGL_COMP_LEQUAL);
	fglEnablePerFragUnit(FGL_PF_DEPTH, FGL_TRUE);


	// Pawn Model test main
	{
/*
    	for(int i=0; i < 4; i++)
    	{
    		WRITEREGF((ConstFloatAddr + (i * 4)), 1.0f);
    	}		
*/    	
    	// cheolkyoo-20070528@modify
    	//float OneVector[4] = { 1.0f, 1.0f, 1.0f, 1.0f};
    	float OneVector[4] = { 1.0f, 0.0f, 0.0f, 1.0f};
	    fglWriteVertexShaderConstFloat(Offset, 4, OneVector);
		

		// Raster engine SFR set
		//glPolygonOffset(0.f, -4.f)
#if 0
		fglSetDepthOffsetParam(FGL_DEPTH_OFFSET_FACTOR, 0x40A00000);
		fglSetDepthOffsetParam(FGL_DEPTH_OFFSET_UNITS, 0x0);
#else
		// cheolkyoo-20070528@modify
		//fglSetDepthOffsetParam(FGL_DEPTH_OFFSET_FACTOR, 0x3F800000);
		fglSetDepthOffsetParam(FGL_DEPTH_OFFSET_FACTOR, 0x0);
		//fglSetDepthOffsetParam(FGL_DEPTH_OFFSET_UNITS, 0x40000000); /* 2.0 */
		//fglSetDepthOffsetParam(FGL_DEPTH_OFFSET_UNITS, 0x3FC00000); /* 1.5 */
		//fglSetDepthOffsetParam(FGL_DEPTH_OFFSET_UNITS, 0x3FA00000); /* 1.25 */
		fglSetDepthOffsetParam(FGL_DEPTH_OFFSET_UNITS, 0x3F800000); /* 1.0 */
		//fglSetDepthOffsetParam(FGL_DEPTH_OFFSET_UNITS, 0x0);
	    // for CellMath
	    //fglSetDepthOffsetParam(FGL_DEPTH_OFFSET_R, 0x34000001);
	    // for DesignWare
	    //fglSetDepthOffsetParam(FGL_DEPTH_OFFSET_R, 0x33C00001);
	    fglSetDepthOffsetParam(FGL_DEPTH_OFFSET_R, 0x34000001);
#endif		
		
		//FGL_DPFAPI( fglSetDepthOffsetParam(FGL_DEPTH_OFFSET_FACTOR, 0x0) );
		//FGL_DPFAPI( fglSetDepthOffsetParam(FGL_DEPTH_OFFSET_UNITS, 0x461C4000) );
//		FGL_DPFAPI( fglSetDepthOffsetParam(FGL_DEPTH_OFFSET_R, 0x33800002) );
		fglEnableDepthOffset(FGL_TRUE);

		
		// Primitive Engine SRF Set
		FGL_Vertex Vtx;
		Vtx.prim = FGL_PRIM_TRIANGLES;
		Vtx.enablePointSize = FGL_FALSE;
		Vtx.numVSOut = 1;
		Vtx.shadeModel = FGL_SHADING_SMOOTH;
		Vtx.colorAttribIdx = 0;
		
		fglSetVertex(&Vtx); // Vertex context register
		
		// Raster SFR Line Width set
		//WRITEREGF(FGRA_LINE_WIDTH, 0.5f);
		fglSetLineWidth(0.5f);

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
		fglSetAttribute(1, &HIAttr);
		fglSetAttribute(2, &HIAttr);
		HIAttr.bEndFlag = FGL_TRUE;
		fglSetAttribute(3, &HIAttr);
		
		//const int nNumTrisSphere = 1024;
        //const int nNumAttributesSphere = 16;
		//unsigned int nNumOfVertices = nNumTrisSphere * 3;
		//unsigned int nNumofData = nNumAttributesSphere * nNumOfVertices;		
		unsigned int nNumOfVertices = 1024 * 3;
		unsigned int nNumofData = 16 * nNumOfVertices;		
		
		unsigned int uiDummy = 0xFFFFFFFF;
		fglSendToFIFO(4, &nNumOfVertices);
		fglSendToFIFO(4, &uiDummy);		

		fglSysTransferToPort(
								/*(unsigned int *)Sphere_Data,*/
								(unsigned int *)FIMG_GEOMETRY_MEMORY,
								/*(volatile unsigned int *)(FGHI_FIFO_ENTRY),*/
								nNumofData
							);


		fglFlush(FGL_PIPESTATE_ALL);	// Pipeline status
/*
    	for(int i=0; i < 4; i++)
    	{
    		WRITEREGF((ConstFloatAddr + (i * 4)), 0.0f);
    	}		
*/        
    	// cheolkyoo-20070528@modify
    	//float ZeroVector[4] = { 0.0f, 0.0f, 0.0f, 0.0f};
    	float ZeroVector[4] = { 0.0f, 0.0f, 1.0f, 1.0f};
	    fglWriteVertexShaderConstFloat(Offset, 4, ZeroVector);
        
        fglEnableDepthOffset(FGL_FALSE);
        
        // Line rendering: line strip
		// cheolkyoo-20070528@modify
		//Vtx.prim = FGL_PRIM_LINE_STRIP;
		Vtx.prim = FGL_PRIM_TRIANGLES;
		Vtx.enablePointSize = FGL_FALSE;
		Vtx.numVSOut = 1;
		Vtx.shadeModel = FGL_SHADING_SMOOTH;
		Vtx.colorAttribIdx = 0;
		
		fglSetVertex(&Vtx); // Vertex context register

		fglSendToFIFO(4, &nNumOfVertices);
		fglSendToFIFO(4, &uiDummy);
		fglSysTransferToPort(
								/*(unsigned int *)Sphere_Data,*/
								(unsigned int *)FIMG_GEOMETRY_MEMORY,
								/*(volatile unsigned int *)(FGHI_FIFO_ENTRY),*/
								nNumofData
							);

		fglFlush(FGL_PIPESTATE_ALL);	// Pipeline status

		// Cache flush
		//fglSysCacheFlush();
		fglClearCache(FGL_CACHECTL_FLUSH_ZCACHE | FGL_CACHECTL_FLUSH_CCACHE);
		//fglSysDelay(50);

		drvsys_clear_buf((unsigned int*)FIMG_DEPTH_BUFFER, CLEAR_SCREEN_SIZE, 0);
	    //drvsys_clear_buf((unsigned int*)(FIMG_COLOR_BUFFER), CLEAR_SCREEN_SIZE, 0);
 	}

	// Depth test Off
	fglEnablePerFragUnit(FGL_PF_DEPTH, FGL_FALSE);
	// Depth offset use disable
	fglEnableDepthOffset(FGL_FALSE);

    return NO_ERROR;
}
