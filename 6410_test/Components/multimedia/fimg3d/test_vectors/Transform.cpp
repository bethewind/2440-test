/*******************************************************************************
 *
 *	ViP model for FIMGSE
 *		version 1.0
 *
 *	Transform.c
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
 *	2006.  1.  26	by cheolkyoo.kim
 *
 *  Description -
 *    Minimum primitive engine diagnostic tests:
 *
 *	$RCSfile: Transform.cpp,v $
 *	$Revision: 1.6 $
 *	$Author: cheolkyoo.kim $
 *	$Date: 2006/05/08 05:37:06 $
 *	$Locker:  $
 *
 *	$Source: C:/CVS/CVSrepository/FIMG-3DSE_SW/fimg3dse_fpga/fimg3d/src/Transform.cpp,v $
 *	$State: Exp $
 *	$Log: Transform.cpp,v $
 *	Revision 1.6  2006/05/08 05:37:06  cheolkyoo.kim
 *	no message
 *	
 *	Revision 1.5  2006/04/13 11:53:57  cheolkyoo.kim
 *	no message
 *
 *	Revision 1.4  2006/04/08 04:33:11  inhosens.lee
 *	updated with fimg_ra code
 *
 *	Revision 1.3  2006/04/05 04:06:11  cheolkyoo.kim
 *	Replaced ~_XCORD and ~_YCORD to ~_XCOORD and ~_YCOORD.
 *
 *	Revision 1.2  2006/03/30 04:27:45  cheolkyoo.kim
 *	Thomas-20060330@comment: Add to code using directive WIN32_VIP for VIP
 *
 *	Revision 1.1  2006/03/10 08:29:07  cheolkyoo.kim
 *	Initial import of FIMG-3DSE_SW package
 *
 *
 ******************************************************************************/

#if 1
  
/****************************************************************************
 *  INCLUDES
 ****************************************************************************/
#include "Fimg3DTest.h"

#include "Transform.vsa.h"
#include "Transform.psa.h"

#define FRAMECOUNT		200
 
int Transform(void)
{	
	int iLoop=0;
	unsigned int nNumOfVertices;
	unsigned int nNumofData;
	//unsigned int ConstFloatAddr;
	bool bSwapBackBuffer = false;
    	unsigned int nNumTrisCube = 12;
	unsigned int nNumAttributesCube = 12;

	FGL_TexUnitParams tuParams;
    
	Matrix4 matMV, matProj, matMVP;
	//float 	fXAng = 0.0f;
	float 	fXAng = 45.0f;
	float 	fYAng = 45.0f;

    
	float Cube_Data[] =
	{
		//  x      y      z     w     r     g     b     a     s     t     r     q
		// FRONT
		-0.3f, -0.3f,  0.3f,  1.f,  1.f,  0.f,  0.f,  0.f,  0.f,  0.f,  0.f,  1.f,
		 0.3f, -0.3f,  0.3f,  1.f,  1.f,  0.f,  0.f,  0.f,  1.f,  0.f,  0.f,  1.f,
		-0.3f,  0.3f,  0.3f,  1.f,  1.f,  0.f,  0.f,  0.f,  0.f,  1.f,  0.f,  1.f,
		 0.3f, -0.3f,  0.3f,  1.f,  1.f,  0.f,  0.f,  0.f,  1.f,  0.f,  0.f,  1.f,
		 0.3f,  0.3f,  0.3f,  1.f,  1.f,  0.f,  0.f,  0.f,  1.f,  1.f,  0.f,  1.f,
		-0.3f,  0.3f,  0.3f,  1.f,  1.f,  0.f,  0.f,  0.f,  0.f,  1.f,  0.f,  1.f,
		// BACK
		-0.3f, -0.3f, -0.3f,  1.f,  0.f,  1.f,  0.f,  0.f,  1.f,  0.f,  0.f,  1.f,
		-0.3f,  0.3f, -0.3f,  1.f,  0.f,  1.f,  0.f,  0.f,  1.f,  1.f,  0.f,  1.f,
		 0.3f, -0.3f, -0.3f,  1.f,  0.f,  1.f,  0.f,  0.f,  0.f,  0.f,  0.f,  1.f,
		-0.3f,  0.3f, -0.3f,  1.f,  0.f,  1.f,  0.f,  0.f,  1.f,  1.f,  0.f,  1.f,
		 0.3f,  0.3f, -0.3f,  1.f,  0.f,  1.f,  0.f,  0.f,  0.f,  1.f,  0.f,  1.f,
		 0.3f, -0.3f, -0.3f,  1.f,  0.f,  1.f,  0.f,  0.f,  0.f,  0.f,  0.f,  1.f,
		// LEFT
		-0.3f, -0.3f,  0.3f,  1.f,  0.f,  0.f,  1.f,  0.f,  1.f,  0.f,  0.f,  1.f,
		-0.3f,  0.3f,  0.3f,  1.f,  0.f,  0.f,  1.f,  0.f,  1.f,  1.f,  0.f,  1.f,
		-0.3f, -0.3f, -0.3f,  1.f,  0.f,  0.f,  1.f,  0.f,  0.f,  0.f,  0.f,  1.f,
		-0.3f,  0.3f,  0.3f,  1.f,  0.f,  0.f,  1.f,  0.f,  1.f,  1.f,  0.f,  1.f,
		-0.3f,  0.3f, -0.3f,  1.f,  0.f,  0.f,  1.f,  0.f,  0.f,  1.f,  0.f,  1.f,
		-0.3f, -0.3f, -0.3f,  1.f,  0.f,  0.f,  1.f,  0.f,  0.f,  0.f,  0.f,  1.f,
		// RIGHT
		 0.3f, -0.3f, -0.3f,  1.f,  1.f,  1.f,  0.f,  0.f,  1.f,  0.f,  0.f,  1.f,
		 0.3f,  0.3f, -0.3f,  1.f,  1.f,  1.f,  0.f,  0.f,  1.f,  1.f,  0.f,  1.f,
		 0.3f, -0.3f,  0.3f,  1.f,  1.f,  1.f,  0.f,  0.f,  0.f,  0.f,  0.f,  1.f,
		 0.3f,  0.3f, -0.3f,  1.f,  1.f,  1.f,  0.f,  0.f,  1.f,  1.f,  0.f,  1.f,
		 0.3f,  0.3f,  0.3f,  1.f,  1.f,  1.f,  0.f,  0.f,  0.f,  1.f,  0.f,  1.f,
		 0.3f, -0.3f,  0.3f,  1.f,  1.f,  1.f,  0.f,  0.f,  0.f,  0.f,  0.f,  1.f,
		// TOP
		-0.3f,  0.3f,  0.3f,  1.f,  1.f,  0.f,  1.f,  0.f,  0.f,  0.f,  0.f,  1.f,
		 0.3f,  0.3f,  0.3f,  1.f,  1.f,  0.f,  1.f,  0.f,  1.f,  0.f,  0.f,  1.f,
		-0.3f,  0.3f, -0.3f,  1.f,  1.f,  0.f,  1.f,  0.f,  0.f,  1.f,  0.f,  1.f,
		 0.3f,  0.3f,  0.3f,  1.f,  1.f,  0.f,  1.f,  0.f,  1.f,  0.f,  0.f,  1.f,
		 0.3f,  0.3f, -0.3f,  1.f,  1.f,  0.f,  1.f,  0.f,  1.f,  1.f,  0.f,  1.f,
		-0.3f,  0.3f, -0.3f,  1.f,  1.f,  0.f,  1.f,  0.f,  0.f,  1.f,  0.f,  1.f,
		// BOTTOM
		-0.3f, -0.3f,  0.3f,  1.f,  0.f,  1.f,  1.f,  0.f,  1.f,  0.f,  0.f,  1.f,
		-0.3f, -0.3f, -0.3f,  1.f,  0.f,  1.f,  1.f,  0.f,  1.f,  1.f,  0.f,  1.f,
		 0.3f, -0.3f,  0.3f,  1.f,  0.f,  1.f,  1.f,  0.f,  0.f,  0.f,  0.f,  1.f,
		-0.3f, -0.3f, -0.3f,  1.f,  0.f,  1.f,  1.f,  0.f,  1.f,  1.f,  0.f,  1.f,
		 0.3f, -0.3f, -0.3f,  1.f,  0.f,  1.f,  1.f,  0.f,  0.f,  1.f,  0.f,  1.f,
		 0.3f, -0.3f,  0.3f,  1.f,  0.f,  1.f,  1.f,  0.f,  0.f,  0.f,  0.f,  1.f

	};

	matProj.SetAsPerspectiveMatrix(60, 800./480., 0.1, 100);

	UART_Printf("Transform Demo Test~~~\n");

	if (fglLoadVShader(Transform_vsa) != FGL_ERR_NO_ERROR)
	{
		return ERROR;
    	}

    	if (fglLoadPShader(Transform_psa) != FGL_ERR_NO_ERROR)
	{
		return ERROR;
    	}
    
	drvsys_clear_buf((unsigned int*)FIMG_DEPTH_BUFFER, CLEAR_SCREEN_SIZE, 0xFFFFFFFF);
	drvsys_clear_buf((unsigned int*)FIMG_COLOR_BUFFER, CLEAR_SCREEN_SIZE, 0x0);

	fglFlush(FGL_PIPESTATE_ALL);	// Pipeline status

	fglSetVShaderAttribTable(FGL_OUTPUT_ATTRIB_IDX0, 0x02020100);
	fglSetVShaderAttribTable(FGL_OUTPUT_ATTRIB_IDX1, 0x07060504);
	fglSetVShaderAttribTable(FGL_OUTPUT_ATTRIB_IDX2, 0x0B0A0908);

	fglVSSetAttribNum(3);
	fglPSSetAttributeNum(2);

	// Raster engine SFR set
	fglSetFaceCullControl(FGL_TRUE, FGL_FALSE/*CCW*/, FGL_FACE_BACK);
	fglSetLODRegister(FGL_LODCOEFF_ENABLE_ALL,	FGL_LODCOEFF_ENABLE_ALL,
						FGL_LODCOEFF_DISABLE,	FGL_LODCOEFF_DISABLE,
						FGL_LODCOEFF_DISABLE,	FGL_LODCOEFF_DISABLE,
						FGL_LODCOEFF_DISABLE,	FGL_LODCOEFF_DISABLE);

	// Per-fragment Unit SFR set
	fglSetZBufBaseAddr(FIMG_DEPTH_BUFFER);
	fglSetDepthParams(FGL_COMP_LESS);
	fglEnablePerFragUnit(FGL_PF_DEPTH, FGL_TRUE);

	// Primitive Engine SRF Set
	FGL_Vertex Vtx;
	Vtx.prim = FGL_PRIM_TRIANGLES;
	Vtx.enablePointSize = FGL_FALSE;
	Vtx.numVSOut = 2;
	Vtx.shadeModel = FGL_SHADING_SMOOTH;
	Vtx.colorAttribIdx = 0;
	fglSetVertex(&Vtx); // Vertex context register

    	// Texture Unit SFR Setting
    	tuParams.eType = FGL_TEX_2D;
    	tuParams.eColorkey = FGL_CKEY_DISABLE;
    	tuParams.bUseExpansion = FGL_FALSE;
    	tuParams.ePaletteFormat = FGL_PALETTE_ARGB8888;
    	tuParams.eFormat = FGL_TEXEL_ARGB1555;
    	tuParams.eUMode = FGL_TEX_WRAP_REPEAT;
    	tuParams.eVMode = FGL_TEX_WRAP_REPEAT;
    	tuParams.bIsNonparametric = FGL_FALSE;
    	tuParams.bUseMagFilter = FGL_TRUE;
    	tuParams.bUseMinFilter = FGL_TRUE;    	
    	//tuParams.bUseMinFilter = FGL_FALSE;    	
    	tuParams.eMipMapFilter = FGL_FILTER_LINEAR;
    	tuParams.uUSize = 256;
    	tuParams.uVSize = 256;
    	tuParams.uPSize = 1;
    	fglSetTexUnitParams(0, &tuParams);
	fglSetTexBaseAddr(0, FIMG_TEXTURE_MEMORY);

	// Host Interface SFR Set
	FGL_HInterface HInterface;
	HInterface.enableAutoInc = FGL_TRUE;
	HInterface.enableVtxBuffer = FGL_FALSE;
	HInterface.enableVtxCache = FGL_FALSE;	
	HInterface.idxType = FGL_INDEX_DATA_UINT;
	HInterface.numVSOut = 3;
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

	nNumOfVertices = nNumTrisCube * 3;
	nNumofData = nNumAttributesCube * nNumOfVertices;

	while(iLoop < FRAMECOUNT)
	{
		matMV.LoadIdentity();
		matMVP.LoadIdentity();
		matMV.Rotate(1,0,0,fXAng);
		matMV.Rotate(0,1,0,fYAng);
		matMV.Translate(0, 0, -1.1);
		matMVP = matProj*matMV;

	      fglWriteVertexShaderConstFloat(0, 16, matMVP.m[0]);
	
		unsigned int uiDummy = 0xFFFFFFFF;
		fglSendToFIFO(4, &nNumOfVertices);
		fglSendToFIFO(4, &uiDummy);

		fglSysTransferToPort((unsigned int *)Cube_Data, nNumofData);

		fglFlush(FGL_PIPESTATE_ALL);	// Pipeline status
		fglClearCache(FGL_CACHECTL_INIT_ALL);

		fXAng += 2.0f;  // 5 degree
		fYAng += 5.0f;  // 10 degree
		
		if(fXAng > 360.f) fXAng = 0.0f;
		if(fYAng > 360.f) fYAng = 0.0f;			

		if(bSwapBackBuffer == false) 
		{
			fglSwapBuffer(1);
			drvsys_clear_buf((unsigned int*)FIMG_DEPTH_BUFFER, CLEAR_SCREEN_SIZE, 0xFFFFFFFF);
			drvsys_clear_buf((unsigned int*)(FIMG_COLOR_BUFFER), CLEAR_SCREEN_SIZE, 0xFF000000);
			fglSetColorBufBaseAddr(FIMG_COLOR_BUFFER);
			bSwapBackBuffer = true;
		}
		else
		{	
			fglSwapBuffer(0);
			drvsys_clear_buf((unsigned int*)FIMG_DEPTH_BUFFER, CLEAR_SCREEN_SIZE, 0xFFFFFFFF);
			drvsys_clear_buf((unsigned int*)(FIMG_COLOR_BUFFER+CLEAR_SCREEN_SIZE), CLEAR_SCREEN_SIZE, 0xFF000000);
			fglSetColorBufBaseAddr(FIMG_COLOR_BUFFER+(CLEAR_SCREEN_SIZE));
			bSwapBackBuffer = false;
		} 

		 iLoop++;
		 
	}

	iLoop = 0;

	fglSetFaceCullControl(FGL_FALSE, FGL_FALSE, FGL_FACE_BACK);
	fglSetLODControl(0x0);
    	fglEnablePerFragUnit(FGL_PF_DEPTH, FGL_FALSE);
		
	return NO_ERROR;
	
}


#elif 1	// touch screen

 
/****************************************************************************
 *  INCLUDES
 ****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "def.h"
#include "option.h"
#include "library.h"
#include "sfr6410.h"
#include "system.h"
#include "intc.h"
#include "gpio.h"

#include "adcts.h"



#include "Fimg3DTest.h"

#include "Transform.vsa.h"
#include "Transform.psa.h"

float uXDat;
float uYDat;

extern volatile	ADCTS_oInform	g_oADCTSInform;

void __irq ADCTS_ISR(void)
{
#if 1	// ORI
	if ( ADCTS_GetXPStylusIntState() == STYLUS_DOWN )
	{
		g_oADCTSInform.ucTouchStatus = ADCTS_TOUCH_DOWN;
	}
	else 
	{
		g_oADCTSInform.ucTouchStatus = ADCTS_TOUCH_UP;
	}
	ADCTS_SetADCClearInt();
	ADCTS_SetADCClearWKInt();
	INTC_ClearVectAddr();
#else

	uXDat	= ADCTS_GetXPData();
	uYDat	= ADCTS_GetYPData();
	UART_Printf("X : %f, Y : %f\n",uXDat,uYDat);

#endif
}

void Capture_XY(void)
{
	//u32 uXDat = 0;
	//u32 uYDat = 0;

	UART_Printf("\n\n[ADCTS touch screen Tracking test.]\n");

	ADCTS_Init();

	g_oADCTSInform.ucTouchStatus	=	ADCTS_TOUCH_INIT;
	ADC_InitADCCON();
	ADCTS_SetDelay(50000);

	ADCTS_SetMeasureMode(eADCTS_MEASURE_FOR_STYLUS);

	Delay(10000);

	INTC_SetVectAddr(NUM_PENDNUP, ADCTS_ISR);
	INTC_Enable(NUM_PENDNUP);

    	UART_Printf("\nPress any key to exit!!!\n");
    	UART_Printf("\nStylus Down, please...... \n");

//	while(!GetKey())
//	{
		if (g_oADCTSInform.ucTouchStatus == ADCTS_TOUCH_DOWN)
		{
			while( ADCTS_GetXPStylusIntState() == STYLUS_DOWN )
			{
				ADC_InitADCCON();
				ADCTS_SetMeasureMode(eADCTS_MEASURE_AUTO_SEQ);

				ADC_EnableStart(ENABLE);

				while(! (ADC_IsAfterStartup() )) ;	//	Wait for begin sampling
				while (! (ADC_IsEOCFlag()));		//	Wait for the EOC

				uXDat	= ADCTS_GetXPData();
				uYDat	= ADCTS_GetYPData();

				UART_Printf("X : %f, Y : %f\n",uXDat,uYDat);
				ADCTS_SetMeasureMode(eADCTS_MEASURE_FOR_STYLUS);
				ADCTS_SetStylusDetectMode(STYLUS_UP);
				Delay(300);
			}
		}
//		else if (g_oADCTSInform.ucTouchStatus == ADCTS_TOUCH_UP)
//		{
//			if ( ADCTS_GetXPStylusIntState() == STYLUS_UP )
//			{
//				ADCTS_SetMeasureMode(eADCTS_MEASURE_FOR_STYLUS);
//			}
//		}
//	}

//	INTC_Disable(NUM_PENDNUP);
	UART_Printf("\n\n[ADCTS touch screen Tracking test End.]\n");	

}

 
int Transform(void)
{	
	unsigned int nNumOfVertices;
	unsigned int nNumofData;
	//unsigned int ConstFloatAddr;
	bool bSwapBackBuffer = false;
    	unsigned int nNumTrisCube = 12;
	unsigned int nNumAttributesCube = 12;

	FGL_TexUnitParams tuParams;
    
	Matrix4 matMV, matProj, matMVP;
	//float 	fXAng = 0.0f;
	// Initial Angle
	float 	fXAng = 45.0f;
	float 	fYAng = 45.0f;
	
	float		fX=0, fY=0;

	#if 1	// touch
	
	UART_Printf("\n\n[ADCTS touch screen Tracking test.]\n");

	ADCTS_Init();

	g_oADCTSInform.ucTouchStatus	=	ADCTS_TOUCH_INIT;
	ADC_InitADCCON();
	ADCTS_SetDelay(50000);

	ADCTS_SetMeasureMode(eADCTS_MEASURE_FOR_STYLUS);

	Delay(10000);

	INTC_SetVectAddr(NUM_PENDNUP, ADCTS_ISR);
	INTC_Enable(NUM_PENDNUP);

    	UART_Printf("\nPress any key to exit!!!\n");
    	UART_Printf("\nStylus Down, please...... \n");

	ADC_InitADCCON();
	ADCTS_SetMeasureMode(eADCTS_MEASURE_AUTO_SEQ);
	ADC_EnableStart(ENABLE);

	if (g_oADCTSInform.ucTouchStatus == ADCTS_TOUCH_DOWN)
	{
		while( ADCTS_GetXPStylusIntState() == STYLUS_DOWN )
		{
			ADC_InitADCCON();
			ADCTS_SetMeasureMode(eADCTS_MEASURE_AUTO_SEQ);
			ADC_EnableStart(ENABLE);

			while(! (ADC_IsAfterStartup() )) ;	//	Wait for begin sampling
			while (! (ADC_IsEOCFlag()));		//	Wait for the EOC

			uXDat	= ADCTS_GetXPData();
			uYDat	= ADCTS_GetYPData();

			UART_Printf("a/d X : %f, a/d Y : %f\n",uXDat,uYDat);
			ADCTS_SetMeasureMode(eADCTS_MEASURE_FOR_STYLUS);
			ADCTS_SetStylusDetectMode(STYLUS_UP);
			Delay(300);
		}
	}
//		else if (g_oADCTSInform.ucTouchStatus == ADCTS_TOUCH_UP)
//		{
//			if ( ADCTS_GetXPStylusIntState() == STYLUS_UP )
//			{
//				ADCTS_SetMeasureMode(eADCTS_MEASURE_FOR_STYLUS);
//			}
//		}

		
	#endif

    
	float Cube_Data[] =
	{
		//  x      y      z     w     r     g     b     a     s     t     r     q
		// FRONT
		-0.3f, -0.3f,  0.3f,  1.f,  1.f,  0.f,  0.f,  0.f,  0.f,  0.f,  0.f,  1.f,
		 0.3f, -0.3f,  0.3f,  1.f,  1.f,  0.f,  0.f,  0.f,  1.f,  0.f,  0.f,  1.f,
		-0.3f,  0.3f,  0.3f,  1.f,  1.f,  0.f,  0.f,  0.f,  0.f,  1.f,  0.f,  1.f,
		 0.3f, -0.3f,  0.3f,  1.f,  1.f,  0.f,  0.f,  0.f,  1.f,  0.f,  0.f,  1.f,
		 0.3f,  0.3f,  0.3f,  1.f,  1.f,  0.f,  0.f,  0.f,  1.f,  1.f,  0.f,  1.f,
		-0.3f,  0.3f,  0.3f,  1.f,  1.f,  0.f,  0.f,  0.f,  0.f,  1.f,  0.f,  1.f,
		// BACK
		-0.3f, -0.3f, -0.3f,  1.f,  0.f,  1.f,  0.f,  0.f,  1.f,  0.f,  0.f,  1.f,
		-0.3f,  0.3f, -0.3f,  1.f,  0.f,  1.f,  0.f,  0.f,  1.f,  1.f,  0.f,  1.f,
		 0.3f, -0.3f, -0.3f,  1.f,  0.f,  1.f,  0.f,  0.f,  0.f,  0.f,  0.f,  1.f,
		-0.3f,  0.3f, -0.3f,  1.f,  0.f,  1.f,  0.f,  0.f,  1.f,  1.f,  0.f,  1.f,
		 0.3f,  0.3f, -0.3f,  1.f,  0.f,  1.f,  0.f,  0.f,  0.f,  1.f,  0.f,  1.f,
		 0.3f, -0.3f, -0.3f,  1.f,  0.f,  1.f,  0.f,  0.f,  0.f,  0.f,  0.f,  1.f,
		// LEFT
		-0.3f, -0.3f,  0.3f,  1.f,  0.f,  0.f,  1.f,  0.f,  1.f,  0.f,  0.f,  1.f,
		-0.3f,  0.3f,  0.3f,  1.f,  0.f,  0.f,  1.f,  0.f,  1.f,  1.f,  0.f,  1.f,
		-0.3f, -0.3f, -0.3f,  1.f,  0.f,  0.f,  1.f,  0.f,  0.f,  0.f,  0.f,  1.f,
		-0.3f,  0.3f,  0.3f,  1.f,  0.f,  0.f,  1.f,  0.f,  1.f,  1.f,  0.f,  1.f,
		-0.3f,  0.3f, -0.3f,  1.f,  0.f,  0.f,  1.f,  0.f,  0.f,  1.f,  0.f,  1.f,
		-0.3f, -0.3f, -0.3f,  1.f,  0.f,  0.f,  1.f,  0.f,  0.f,  0.f,  0.f,  1.f,
		// RIGHT
		 0.3f, -0.3f, -0.3f,  1.f,  1.f,  1.f,  0.f,  0.f,  1.f,  0.f,  0.f,  1.f,
		 0.3f,  0.3f, -0.3f,  1.f,  1.f,  1.f,  0.f,  0.f,  1.f,  1.f,  0.f,  1.f,
		 0.3f, -0.3f,  0.3f,  1.f,  1.f,  1.f,  0.f,  0.f,  0.f,  0.f,  0.f,  1.f,
		 0.3f,  0.3f, -0.3f,  1.f,  1.f,  1.f,  0.f,  0.f,  1.f,  1.f,  0.f,  1.f,
		 0.3f,  0.3f,  0.3f,  1.f,  1.f,  1.f,  0.f,  0.f,  0.f,  1.f,  0.f,  1.f,
		 0.3f, -0.3f,  0.3f,  1.f,  1.f,  1.f,  0.f,  0.f,  0.f,  0.f,  0.f,  1.f,
		// TOP
		-0.3f,  0.3f,  0.3f,  1.f,  1.f,  0.f,  1.f,  0.f,  0.f,  0.f,  0.f,  1.f,
		 0.3f,  0.3f,  0.3f,  1.f,  1.f,  0.f,  1.f,  0.f,  1.f,  0.f,  0.f,  1.f,
		-0.3f,  0.3f, -0.3f,  1.f,  1.f,  0.f,  1.f,  0.f,  0.f,  1.f,  0.f,  1.f,
		 0.3f,  0.3f,  0.3f,  1.f,  1.f,  0.f,  1.f,  0.f,  1.f,  0.f,  0.f,  1.f,
		 0.3f,  0.3f, -0.3f,  1.f,  1.f,  0.f,  1.f,  0.f,  1.f,  1.f,  0.f,  1.f,
		-0.3f,  0.3f, -0.3f,  1.f,  1.f,  0.f,  1.f,  0.f,  0.f,  1.f,  0.f,  1.f,
		// BOTTOM
		-0.3f, -0.3f,  0.3f,  1.f,  0.f,  1.f,  1.f,  0.f,  1.f,  0.f,  0.f,  1.f,
		-0.3f, -0.3f, -0.3f,  1.f,  0.f,  1.f,  1.f,  0.f,  1.f,  1.f,  0.f,  1.f,
		 0.3f, -0.3f,  0.3f,  1.f,  0.f,  1.f,  1.f,  0.f,  0.f,  0.f,  0.f,  1.f,
		-0.3f, -0.3f, -0.3f,  1.f,  0.f,  1.f,  1.f,  0.f,  1.f,  1.f,  0.f,  1.f,
		 0.3f, -0.3f, -0.3f,  1.f,  0.f,  1.f,  1.f,  0.f,  0.f,  1.f,  0.f,  1.f,
		 0.3f, -0.3f,  0.3f,  1.f,  0.f,  1.f,  1.f,  0.f,  0.f,  0.f,  0.f,  1.f

	};

	matProj.SetAsPerspectiveMatrix(60, 800./480., 0.1, 100);

	UART_Printf("Transform Demo Test~~~\n");

	if (fglLoadVShader(Transform_vsa) != FGL_ERR_NO_ERROR)
	{
		return ERROR;
    	}

    	if (fglLoadPShader(Transform_psa) != FGL_ERR_NO_ERROR)
	{
		return ERROR;
    	}
    

#if 0
	Matrix4 matMVP(1.707107f,  0.0f,           1.707107f,   0.0f,
				    1.207107f,  1.707107f,   -1.207107f, 0.0f,
				    0.501001f,  -0.708522f, -0.501001f, 1.302803f,
				    0.5f,      	 -0.707107f, -0.5f,      	    1.5f);

    	fglWriteVertexShaderConstFloat(0, 16, matMVP.m[0]);
#endif


	drvsys_clear_buf((unsigned int*)FIMG_DEPTH_BUFFER, CLEAR_SCREEN_SIZE, 0xFFFFFFFF);
	drvsys_clear_buf((unsigned int*)FIMG_COLOR_BUFFER, CLEAR_SCREEN_SIZE, 0x0);

	fglFlush(FGL_PIPESTATE_ALL);	// Pipeline status

	fglSetVShaderAttribTable(FGL_OUTPUT_ATTRIB_IDX0, 0x02020100);
	fglSetVShaderAttribTable(FGL_OUTPUT_ATTRIB_IDX1, 0x07060504);
	fglSetVShaderAttribTable(FGL_OUTPUT_ATTRIB_IDX2, 0x0B0A0908);

	fglVSSetAttribNum(3);
	fglPSSetAttributeNum(2);

	// Raster engine SFR set
	fglSetFaceCullControl(FGL_TRUE, FGL_FALSE/*CCW*/, FGL_FACE_BACK);
	//FGL_DPFAPI( fglSetLODControl(0x0000003F) );
	fglSetLODRegister(FGL_LODCOEFF_ENABLE_ALL,	FGL_LODCOEFF_ENABLE_ALL,
						FGL_LODCOEFF_DISABLE,	FGL_LODCOEFF_DISABLE,
						FGL_LODCOEFF_DISABLE,	FGL_LODCOEFF_DISABLE,
						FGL_LODCOEFF_DISABLE,	FGL_LODCOEFF_DISABLE);


	// Per-fragment Unit SFR set
	fglSetZBufBaseAddr(FIMG_DEPTH_BUFFER);
	fglSetDepthParams(FGL_COMP_LESS);
	fglEnablePerFragUnit(FGL_PF_DEPTH, FGL_TRUE);

	// Primitive Engine SRF Set
	FGL_Vertex Vtx;
	Vtx.prim = FGL_PRIM_TRIANGLES;
	Vtx.enablePointSize = FGL_FALSE;
	Vtx.numVSOut = 2;
	Vtx.shadeModel = FGL_SHADING_SMOOTH;
	Vtx.colorAttribIdx = 0;
	fglSetVertex(&Vtx); // Vertex context register

    	// Texture Unit SFR Setting
    	tuParams.eType = FGL_TEX_2D;
    	tuParams.eColorkey = FGL_CKEY_DISABLE;
    	tuParams.bUseExpansion = FGL_FALSE;
    	tuParams.ePaletteFormat = FGL_PALETTE_ARGB8888;
    	tuParams.eFormat = FGL_TEXEL_ARGB1555;
    	tuParams.eUMode = FGL_TEX_WRAP_REPEAT;
    	tuParams.eVMode = FGL_TEX_WRAP_REPEAT;
    	tuParams.bIsNonparametric = FGL_FALSE;
    	tuParams.bUseMagFilter = FGL_TRUE;
    	tuParams.bUseMinFilter = FGL_TRUE;    	
    	//tuParams.bUseMinFilter = FGL_FALSE;    	
    	tuParams.eMipMapFilter = FGL_FILTER_LINEAR;
    	tuParams.uUSize = 256;
    	tuParams.uVSize = 256;
    	tuParams.uPSize = 1;
    	fglSetTexUnitParams(0, &tuParams);
	fglSetTexBaseAddr(0, FIMG_TEXTURE_MEMORY);

	// Host Interface SFR Set
	FGL_HInterface HInterface;
	HInterface.enableAutoInc = FGL_TRUE;
	HInterface.enableVtxBuffer = FGL_FALSE;
	HInterface.enableVtxCache = FGL_FALSE;	
	HInterface.idxType = FGL_INDEX_DATA_UINT;
	HInterface.numVSOut = 3;
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

	nNumOfVertices = nNumTrisCube * 3;
	nNumofData = nNumAttributesCube * nNumOfVertices;

	//while(1)
	while(GetKey() == 0)
	{
		matMV.LoadIdentity();
		matMVP.LoadIdentity();
		matMV.Rotate(1,0,0,fXAng);
		matMV.Rotate(0,1,0,fYAng);
		//matMV.Translate(0, 0, -1.1);
		matMV.Translate(fX, fY, -1.1);
		matMVP = matProj*matMV;

	      fglWriteVertexShaderConstFloat(0, 16, matMVP.m[0]);
	
	unsigned int uiDummy = 0xFFFFFFFF;
	fglSendToFIFO(4, &nNumOfVertices);
	fglSendToFIFO(4, &uiDummy);

	fglSysTransferToPort( (unsigned int *)Cube_Data, nNumofData);

	fglFlush(FGL_PIPESTATE_ALL);	// Pipeline status
		fglClearCache(FGL_CACHECTL_INIT_ALL);

#if 1
		//fXAng += 0.0872664f;  // 5 degree
		//fYAng += 0.1745328f;  // 10 degree
		fXAng += 2.0f;  // 5 degree
		fYAng += 5.0f;  // 10 degree
		
		//if(fXAng > 2*PI) fXAng = 0.0f;
		//if(fYAng > 2*PI) fYAng = 0.0f;	
		if(fXAng > 360.f) fXAng = 0.0f;
		if(fYAng > 360.f) fYAng = 0.0f;			
#endif

		if(bSwapBackBuffer == false) 
		{
			#if 0
			fglSwapBuffer(0);
			drvsys_clear_buf((unsigned int*)FIMG_DEPTH_BUFFER, CLEAR_SCREEN_SIZE, 0xFFFFFFFF);
			drvsys_clear_buf((unsigned int*)(0x65000000+(CLEAR_SCREEN_SIZE)), CLEAR_SCREEN_SIZE, 0xFF000000);
			fglSetColorBufBaseAddr(0x65000000);
			bSwapBackBuffer = true;
			#else
			fglSwapBuffer(1);
			drvsys_clear_buf((unsigned int*)FIMG_DEPTH_BUFFER, CLEAR_SCREEN_SIZE, 0xFFFFFFFF);
			drvsys_clear_buf((unsigned int*)(FIMG_COLOR_BUFFER), CLEAR_SCREEN_SIZE, 0xFF000000);
			fglSetColorBufBaseAddr(FIMG_COLOR_BUFFER);
			bSwapBackBuffer = true;
			#endif

		}
		else
		{	
			#if 0	// ori
			fglSwapBuffer(1);
			drvsys_clear_buf((unsigned int*)FIMG_DEPTH_BUFFER, CLEAR_SCREEN_SIZE, 0xFFFFFFFF);
			drvsys_clear_buf((unsigned int*)(0x65000000+(2*CLEAR_SCREEN_SIZE)), CLEAR_SCREEN_SIZE, 0xFF000000);
			fglSetColorBufBaseAddr(0x65000000+(CLEAR_SCREEN_SIZE));
			bSwapBackBuffer = false;

			#else
			fglSwapBuffer(0);
			drvsys_clear_buf((unsigned int*)FIMG_DEPTH_BUFFER, CLEAR_SCREEN_SIZE, 0xFFFFFFFF);
			drvsys_clear_buf((unsigned int*)(FIMG_COLOR_BUFFER+CLEAR_SCREEN_SIZE), CLEAR_SCREEN_SIZE, 0xFF000000);
			fglSetColorBufBaseAddr(FIMG_COLOR_BUFFER+(CLEAR_SCREEN_SIZE));
			bSwapBackBuffer = false;
			#endif
		} 

		#if 1	// TOUCH

		Delay(10);
		//Capture_XY();
		fX = (( uXDat-210)/310)-1;
		fY = ((uYDat-300)/200)-1;
		UART_Printf("X = %.2f, Y = %.2f\n", fX, fY);
		
		#endif
	}


	fglSetFaceCullControl(FGL_FALSE, FGL_FALSE, FGL_FACE_BACK);
	fglSetLODControl(0x0);
    	fglEnablePerFragUnit(FGL_PF_DEPTH, FGL_FALSE);

	//INTC_Disable(NUM_PENDNUP);
		
	return NO_ERROR;
	
}
#endif

