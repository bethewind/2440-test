/*******************************************************************************
 *
 *	ViP model for FIMGSE
 *		version 1.0
 *
 *	BurstTransfer.c
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
 *	2006. 1. 31	 by cheolkyoo.kim
 *
 *  Description
 *
 *	$RCSfile: BurstTransfer.cpp,v $
 *	$Revision: 1.6 $
 *	$Author: cheolkyoo.kim $
 *	$Date: 2006/05/08 05:34:30 $
 *	$Locker:  $
 *
 *	$Source: C:/CVS/CVSrepository/FIMG-3DSE_SW/fimg3dse_fpga/fimg3d/src/BurstTransfer.cpp,v $
 *	$State: Exp $
 *	$Log: BurstTransfer.cpp,v $
 *	Revision 1.6  2006/05/08 05:34:30  cheolkyoo.kim
 *	no message
 *	
 *	Revision 1.5  2006/04/13 11:53:44  cheolkyoo.kim
 *	no message
 *
 *	Revision 1.4  2006/04/07 11:00:37  inhosens.lee
 *	updated with fimg_ra code
 *
 *	Revision 1.3  2006/04/05 04:06:07  cheolkyoo.kim
 *	Replaced ~_XCORD and ~_YCORD to ~_XCOORD and ~_YCOORD.
 *
 *	Revision 1.2  2006/03/30 04:27:42  cheolkyoo.kim
 *	Thomas-20060330@comment: Add to code using directive WIN32_VIP for VIP
 *
 *	Revision 1.1  2006/03/10 08:29:08  cheolkyoo.kim
 *	Initial import of FIMG-3DSE_SW package
 *
 ******************************************************************************/
/****************************************************************************
 *  INCLUDES
 ****************************************************************************/
#include "Fimg3DTest.h"

//#include "Debug.h"

#include "Models.vsa.h"
#include "Models.psa.h"
//#include "Bishop.h"

/****************************************************************************
 *  DEFINES
 ****************************************************************************/

int BurstTransfer(void)

{
	unsigned int nNumOfVertices;
	unsigned int nNumofData;
	//unsigned int ConstFloatAddr;

    if (fglLoadVShader(Models_vsa) != FGL_ERR_NO_ERROR) {
		return ERROR;
    }

    if (fglLoadPShader(Models_psa) != FGL_ERR_NO_ERROR) {
		return ERROR;
    }

	Matrix4 matMVP, matProj;
	//matProj.SetAsOrthoMatrix(-12, 12, -16, 16, 0.0, 100.0);
	matProj.SetAsOrthoMatrix(-16, 16, -9, 9, 0.0, 100.0);
	matMVP.Translate(0, 0, -5);
	matMVP = matProj * matMVP;
    //ConstFloatAddr = FGVS_CFLOAT_SADDR;
    //matMVP.WriteMatrix4(ConstFloatAddr);
    
    	fglWriteVertexShaderConstFloat(0, 16, matMVP.m[0]);
	
	drvsys_clear_buf((unsigned int*)FIMG_DEPTH_BUFFER, CLEAR_SCREEN_SIZE, 0xFFFFFFFF);
	drvsys_clear_buf((unsigned int*)FIMG_COLOR_BUFFER, CLEAR_SCREEN_SIZE, 0x0);

	fglFlush(FGL_PIPESTATE_ALL);	// Pipeline status

	fglVSSetAttribNum(3);
	fglPSSetAttributeNum(1);

	// Raster Engine SFR set
	fglSetFaceCullControl(FGL_TRUE, FGL_FALSE/*CCW*/, FGL_FACE_BACK);

	// Per-fragment Unit SFR set
	//WRITEREG(FGPF_DEPTHBUF_ADDR, FIMG_DEPTH_BUFFER); // Depth/Stencil buffer base address
	//WRITEREG(FGPF_DEPTH, 0x00000005); // Depth test LESS function & enable set

	// Depth/Stencil buffer base address
	fglSetZBufBaseAddr(FIMG_DEPTH_BUFFER);
	fglSetDepthParams(FGL_COMP_LESS);
	fglEnablePerFragUnit(FGL_PF_DEPTH, FGL_TRUE);

	// Pawn Model test main
	{
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
		fglSetAttribute(1, &HIAttr);
		HIAttr.bEndFlag = FGL_TRUE;
		fglSetAttribute(2, &HIAttr);
		
		//const int nNumTrisBishop = 496;
        	//const int nNumAttributesBishop = 12;
		//nNumOfVertices = nNumTrisBishop * 3;
		//nNumofData = nNumAttributesBishop * nNumOfVertices;
		nNumOfVertices = 496 * 3;
		nNumofData = 12 * nNumOfVertices;

		unsigned int uiDummy = 0xFFFFFFFF;
		fglSendToFIFO(4, &nNumOfVertices);
		fglSendToFIFO(4, &uiDummy);
		
		fglSysTransferToPort((unsigned int *)FIMG_GEOMETRY_MEMORY, nNumofData);

		fglFlush(FGL_PIPESTATE_ALL);	// Pipeline status

		// Cache flush
		//fglSysCacheFlush();
		fglClearCache(FGL_CACHECTL_FLUSH_ZCACHE | FGL_CACHECTL_FLUSH_CCACHE);

		//fglSysDelay(50);

		drvsys_clear_buf((unsigned int*)FIMG_DEPTH_BUFFER, CLEAR_SCREEN_SIZE, 0);
 	}

	//const char* pFileName_SFR_Dump = "SFR_BURSTTRANSFER.txt";
	//fglDumpContext(pFileName_SFR_Dump);

	fglSetFaceCullControl(FGL_FALSE, FGL_FALSE, FGL_FACE_BACK);
	fglEnablePerFragUnit(FGL_PF_DEPTH, FGL_FALSE);
	
    	return NO_ERROR;
	
}

