/*******************************************************************************
 *
 *	ViP model for FIMGSE
 *		version 1.0
 *
 *	BackfaceCull.c
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
 *	$RCSfile: BackfaceCull.cpp,v $
 *	$Revision: 1.6 $
 *	$Author: cheolkyoo.kim $
 *	$Date: 2006/05/08 05:34:30 $
 *	$Locker:  $
 *
 *	$Source: C:/CVS/CVSrepository/FIMG-3DSE_SW/fimg3dse_fpga/fimg3d/src/BackfaceCull.cpp,v $
 *	$State: Exp $
 *	$Log: BackfaceCull.cpp,v $
 *	Revision 1.6  2006/05/08 05:34:30  cheolkyoo.kim
 *	no message
 *
 *	Revision 1.5  2006/04/13 11:53:44  cheolkyoo.kim
 *	no message
 *
 *	Revision 1.4  2006/04/06 08:09:20  inhosens.lee
 *	updated with fimg_ra code
 *
 *	Revision 1.3  2006/04/05 04:06:07  cheolkyoo.kim
 *	Replaced ~_XCORD and ~_YCORD to ~_XCOORD and ~_YCOORD.
 *
 *	Revision 1.2  2006/03/30 04:27:42  cheolkyoo.kim
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
#include <string.h>



//#include "Debug.h"

#include "Fimg3DTest.h"


#include "mov_v2o.vsa.h"
#include "mov_v2o.psa.h"

/****************************************************************************
 *  DEFINES
 ****************************************************************************/

int BackfaceCull(void)

{
//	unsigned int uRegVal;

 	float VERTICES[] = 
 	{
		/*  x       y      r       g       b */
        -1.0f,	 1.0f,	0.2f,	0.2f, 	0.2f,
        -1.0f,	 0.5f,	0.2f,	0.2f, 	0.2f,
        -0.5f,	 1.0f,	0.2f,	0.2f, 	0.2f,
        -0.5f,	 1.0f,	0.2f,	0.2f, 	0.2f,
        -1.0f,	 0.5f,	0.2f,	0.2f, 	0.2f,
        -0.5f,	 0.5f,	0.2f,	0.2f, 	0.2f,

        -0.5f,	 0.5f,	1.0f,	0.0f, 	0.0f,
        -0.5f,	 1.0f,	1.0f,	0.0f, 	0.0f,
         0.0f,	 1.0f,	1.0f,	0.0f, 	0.0f,
        -0.5f,	 0.5f,	1.0f,	0.0f, 	0.0f,
         0.0f,	 1.0f,	1.0f,	0.0f, 	0.0f,
         0.0f,	 0.5f,	1.0f,	0.0f, 	0.0f
 	};

	float VERTEX[60];
	
    float COLOR[6][3] = 
    {
        1.0f, 1.0f, 0.0f, 
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 
        1.0f, 1.0f, 1.0f,
        0.0f, 1.0f, 1.0f            
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
	WRITEREG(FGIB_EXE_MODE, 0x0); // Pixel shader execution mode control register
	WRITEREG(FGIB_PC_COPY, 0x1); // Copy PSPCS_ADDR value to program counter
	WRITEREG(FGIB_ATTRIB_NUM, 0x1); // Number of attribute of current context
	fglSysPollingMask(FGIB_INBUF_STATUS, 0x0, 0x1); // Status signal of pixel shader input buffer
	WRITEREG(FGIB_EXE_MODE, 0x1); // Pixel shader execution mode control register
*/
    fglVSSetAttribNum(2);
    fglPSSetAttributeNum(1);


	//uRegVal = READREG(FGRA_BACKFACE_CULL);

	// Primitive Engine SFR Set
	FGL_Vertex Vtx;
	Vtx.prim = FGL_PRIM_TRIANGLES;
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
	HIAttr.numComp = 2;
	HIAttr.srcW = FGL_ATTRIB_ORDER_4TH;
	HIAttr.srcZ = FGL_ATTRIB_ORDER_3RD;
	HIAttr.srcY = FGL_ATTRIB_ORDER_2ND;
	HIAttr.srcX = FGL_ATTRIB_ORDER_1ST;	
	fglSetAttribute(0, &HIAttr);
	HIAttr.bEndFlag = FGL_TRUE;
	HIAttr.numComp = 3;
	fglSetAttribute(1, &HIAttr);
		
    // Back face cull
    {
		for( int i = 0; i < 2; i++) // loop for front face {CCW, CW}
		{
			memcpy(VERTEX, VERTICES, sizeof(VERTICES));

			if(i == 1)
			{
	            for(int k = 0; k < 12; k++)
                    VERTEX[k*5] += 1.0f; 
			}
			
			for(int j = 0; j < 4; j++) // loop for cull face {FRONT, BACK, FRONT & BACK}
			{
			    
    			if(j == 2)
    			    fglSetFaceCullControl(FGL_FALSE, i /* CCW/CW */, (FGL_Face)(j-1));
    			else
    			    fglSetFaceCullControl(FGL_TRUE, i /* CCW/CW */, (FGL_Face)j);

    			unsigned int uiTmpVertices = 0xC;		
				unsigned int uiDummy = 0xFFFFFFFF;
				fglSendToFIFO(4, &uiTmpVertices);
				fglSendToFIFO(4, &uiDummy);	

    			fglSysTransferToPort(
    									(unsigned int *)VERTEX,
    									/*(volatile unsigned int *)(FGHI_FIFO_ENTRY),*/
    									(sizeof(VERTEX)/sizeof(VERTEX[0]))
    								);

    			fglFlush(FGL_PIPESTATE_ALL);	// Pipeline status
			    
			    for(int k = 0; k < 12; k++)
			    {
			        VERTEX[k*5 + 1] -= 0.5f;
			        
			        if(k < 6)
			        {
    			        VERTEX[k*5 + 2] = COLOR[2*j][0];
    			        VERTEX[k*5 + 3] = COLOR[2*j][1];
    			        VERTEX[k*5 + 4] = COLOR[2*j][2];
			        }
			        else
			        {
    			        VERTEX[k*5 + 2] = COLOR[2*j+1][0];
    			        VERTEX[k*5 + 3] = COLOR[2*j+1][1];
    			        VERTEX[k*5 + 4] = COLOR[2*j+1][2];
			        }
			    }
			}
		}

		// Cache flush
		//fglSysCacheFlush();
		fglClearCache(FGL_CACHECTL_FLUSH_CCACHE);
		//fglSysDelay(50);
    }

    //drvsys_clear_buf((unsigned int*)(FIMG_COLOR_BUFFER), CLEAR_SCREEN_SIZE, 0x0);


	fglSetFaceCullControl(FGL_FALSE, FGL_FALSE, FGL_FACE_BACK);

    return NO_ERROR;
}
