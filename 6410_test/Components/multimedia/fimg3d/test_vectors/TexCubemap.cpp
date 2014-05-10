/*******************************************************************************
 *
 *	ViP model for FIMGSE
 *		version 1.0
 *
 *	TexCubemapTest.c
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
 *	2006. 3. 24	by cheolkyoo.kim
 *
 *  Description
 *
 *	$Revision:  $
 *	$Author:  $
 *	$Date:  $
 *	$Locker:  $
 *
 *	$Source:  $
 *	$State:  $
 *	$Log:  $
 ******************************************************************************/

/****************************************************************************
 *  INCLUDES
 ****************************************************************************/
#include <math.h>

#include "Fimg3DTest.h"


#include "stdio.h"
#include "library.h"
#include "TexCubemap.vsa.h"
#include "TexCubemap.psa.h"
//#include "Cubemap.h"

/****************************************************************************
 *  DEFINES
 ****************************************************************************/


void transposeMatrix(float *in, float *out)
{
	out[0] = in[0]; out[1] = in[4]; out[2] = in[8]; out[3] = in[12];
	out[4] = in[1]; out[5] = in[5]; out[6] = in[9]; out[7] = in[13];
	out[8] = in[2]; out[9] = in[6]; out[10] = in[10]; out[11] = in[14];
	out[12] = in[3]; out[13] = in[7]; out[14] = in[11]; out[15] = in[15];
}

int TexCubemap(void)

{     
	Matrix4 matMV, matProj, matMVP, matNormal, matTranslate, matRotate;
	   
	if (fglLoadVShader(TexCubemap_vsa) != FGL_ERR_NO_ERROR) {
		return ERROR;
	}

	if (fglLoadPShader(TexCubemap_psa) != FGL_ERR_NO_ERROR) {
		return ERROR;
	}

	drvsys_clear_buf((unsigned int*)(FIMG_DEPTH_BUFFER), CLEAR_SCREEN_SIZE, 0xFFFFFFFF);
	drvsys_clear_buf((unsigned int*)(FIMG_COLOR_BUFFER), CLEAR_SCREEN_SIZE, 0x0);


	fglFlush(FGL_PIPESTATE_ALL);	// Pipeline status
	

	fglSetZBufBaseAddr(FIMG_DEPTH_BUFFER);
	
	if(fglSetDepthParams(FGL_COMP_LESS) != FGL_ERR_NO_ERROR) {
		UART_Printf("\nDepth buffer test: The fglSetDepthParams failed");
	}

	if(fglEnablePerFragUnit(FGL_PF_DEPTH, FGL_TRUE) != FGL_ERR_NO_ERROR) {
		UART_Printf("\nDepth buffer test: The fglEnablePerFragUnit failed");
	}
		
	/* Build projection matrix once. */
	//matProj.SetAsPerspectiveMatrix(45.0, 0.75, 1.0, 20.0);
	matProj.SetAsPerspectiveMatrix(45.0, 1.67, 1.0, 20.0);

	fglVSSetAttribNum(2);
	fglVSSetPCRange(0, 12);
	fglPSSetAttributeNum(1);


	{
		matTranslate.Translate(0, 0, -5.5);
		matRotate.Rotate(0, 1, 0, 180);
		matMV = matTranslate * matRotate;
		matMVP = matProj * matMV;
		matRotate.Transpose();

		//unsigned int uFloatAddr = FGVS_CFLOAT_SADDR;
		//uFloatAddr = matMVP.WriteMatrix4(uFloatAddr);
		//uFloatAddr = matRotate.WriteMatrix4(uFloatAddr);

        unsigned int Offset = fglWriteVertexShaderConstFloat(0, 16, matMVP.m[0]);
        Offset = fglWriteVertexShaderConstFloat(Offset, 16, matRotate.m[0]);


		Matrix4 matTmp;
		matTmp = matMV.Inverse();
		matNormal = matTmp.Transpose();
		//matNormal.WriteMatrix4(uFloatAddr, true);

        Offset = fglWriteVertexShaderConstFloat(Offset,  16, matNormal.m[0]);

		// Primitive Engine SRF Set
		FGL_Vertex Vtx;
		Vtx.prim = FGL_PRIM_TRIANGLES;
		Vtx.enablePointSize = FGL_FALSE;
		Vtx.numVSOut = 1;
		Vtx.shadeModel = FGL_SHADING_SMOOTH;
		Vtx.colorAttribIdx = 0;
		
		fglSetVertex(&Vtx);
 
        FGL_TexUnitParams tuParams = 
        {
          	FGL_TEX_CUBE,           /*FGL_TexType*/
        	FGL_CKEY_DISABLE,       /*FGL_CKeySel*/
        	FGL_TRUE,               /*bUseExpansion*/
        	FGL_PALETTE_ARGB8888,   /*FGL_PaletteFormat*/
        	FGL_TEXEL_ARGB8888,     /*FGL_TexelFormat*/
        	FGL_TEX_WRAP_CLAMP,     /*FGL_TexWrapMode*/
        	FGL_TEX_WRAP_CLAMP,     /*FGL_TexWrapMode*/
        	FGL_FALSE,              /*bIsParamatric*/
        	FGL_TRUE,               /*bUseMagFilter*/
        	FGL_TRUE,               /*bUseMinFilter*/
        	FGL_FILTER_NEAREST,     /*eMipMapFilter*/
        	135,                    /**/
        	110,                    /**/
        	1                       /**/
        };

		fglSetTexUnitParams(0, &tuParams);
		fglSetTexBaseAddr(0, FIMG_TEXTURE_MEMORY);

		//WRITEREG(FGRA_LOD_CTRL, 7);
		fglSetLODRegister(FGL_LODCOEFF_ENABLE_ALL,
					FGL_LODCOEFF_DISABLE,
					FGL_LODCOEFF_DISABLE,
					FGL_LODCOEFF_DISABLE,
					FGL_LODCOEFF_DISABLE,
					FGL_LODCOEFF_DISABLE,
					FGL_LODCOEFF_DISABLE,
					FGL_LODCOEFF_DISABLE);

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
		
        //const int nNumTrisCube = 1024;
        //const int nNumAttributesCube = 6;
		//unsigned int uiTmpVertices = 3 * nNumTrisCube;		
		//unsigned int nNumOfData = 3 * nNumTrisCube * nNumAttributesCube;
		unsigned int nNumOfVertices = 3 * 1024;		
		unsigned int nNumOfData = nNumOfVertices * 6;
		
		unsigned int uiDummy = 0xFFFFFFFF;
		fglSendToFIFO(4, &nNumOfVertices);
		fglSendToFIFO(4, &uiDummy);

		fglSysTransferToPort(
								/*(unsigned int *)Cubemap_Data,*/
								(unsigned int *)FIMG_GEOMETRY_MEMORY,
								/*(volatile unsigned int *)(FGHI_FIFO_ENTRY),*/
								nNumOfData
							 );

		fglFlush(FGL_PIPESTATE_ALL);	// Pipeline status
	}
	
	//fglSysCacheFlush();
	//fglSysCacheClear();
	fglClearCache(FGL_CACHECTL_INIT_ALL);

	fglSysDelay(50);

	//drvsys_clear_buf((unsigned int*)(FIMG_COLOR_BUFFER+m_FBOffsize), CLEAR_SCREEN_SIZE, 0x0);


	//const char* pFileName_SFR_Dump = "SFR_Cubemap.txt";
	//fglDumpContext(pFileName_SFR_Dump);
    //WRITEREG(FGVS_IN_ATTRIB_IDX0, 0x03020100);
    //WRITEREG(FGVS_IN_ATTRIB_IDX1, 0x07060504);

	fglSetLODControl(0x0);
	fglEnablePerFragUnit(FGL_PF_DEPTH, FGL_FALSE);
    	
	return NO_ERROR;
}

