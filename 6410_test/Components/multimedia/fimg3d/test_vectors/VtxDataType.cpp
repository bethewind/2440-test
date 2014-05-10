/*******************************************************************************
 *
 *	ViP model for FIMGSE
 *		version 1.0
 *
 *	VtxDataType.c
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
 *	2006.  1. 31 by cheolkyoo.kim
 *
 *  Description -
 *
 *	$RCSfile: VtxDataType.cpp,v $
 *	$Revision: 1.6 $
 *	$Author: cheolkyoo.kim $
 *	$Date: 2006/05/08 05:37:05 $
 *	$Locker:  $
 *
 *	$Source: C:/CVS/CVSrepository/FIMG-3DSE_SW/fimg3dse_fpga/fimg3d/src/VtxDataType.cpp,v $
 *	$State: Exp $
 *	$Log: VtxDataType.cpp,v $
 *	Revision 1.6  2006/05/08 05:37:05  cheolkyoo.kim
 *	no message
 *	
 *	Revision 1.5  2006/04/13 11:53:58  cheolkyoo.kim
 *	no message
 *
 *	Revision 1.4  2006/04/08 07:09:33  inhosens.lee
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
 *
 ******************************************************************************/
/****************************************************************************
 *  INCLUDES
 ****************************************************************************/

#include "Fimg3DTest.h"

#include "VtxDataType.vsa.h"
#include "mov_v2o.psa.h"

/****************************************************************************
 *  DEFINES
 ****************************************************************************/
int VtxDataType(void)

{
	unsigned int i;
	unsigned int j;
//	unsigned int k;
	unsigned int l;
	unsigned int *pVerties;
	unsigned int VertiesTmp;
	unsigned int *pColor;
	unsigned int *pColorTmp;
	unsigned int DwordSize;

	int Vertex2i[] =
	{
		-90, 60,
		-70, 60,
		-90, 40,
		-70, 40
	};

	short Vertex2s[]=
	{
		-90, -40,
		-70, -40,
		-90, -60,
		-70, -60
	};

   	// Set current drawing color to red
	//	               R     G     B
	char  Color3b[4] = {127, 0, 0, 0};
	int   Color3i[4] = {2147483647, 0, 0, 0};
	short Color3s[4] = {32767, 0, 0, 0};
	unsigned char  Color3ub[4] = {255, 0, 0, 0};
	unsigned int   Color3ui[4] = {4294967295LL, 0, 0, 0};
	unsigned short Color3us[4] = {65535, 0, 0, 0};

    if (fglLoadVShader(VtxDataType_vsa) != FGL_ERR_NO_ERROR) {
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


	for(j = 0; j < 2; j++)
	{

		// Primitive Engine SRF Set
		FGL_Vertex Vtx;
		Vtx.prim = FGL_PRIM_TRIANGLE_STRIP;
		Vtx.enablePointSize = FGL_FALSE;
		Vtx.numVSOut = 1;
		Vtx.shadeModel = FGL_SHADING_SMOOTH;
		Vtx.colorAttribIdx = 0;		
		fglSetVertex(&Vtx);					// Vertex context register
		
		FGL_HInterface HInterface;
		HInterface.enableAutoInc = FGL_TRUE;
		HInterface.enableVtxBuffer = FGL_FALSE;
		HInterface.enableVtxCache = FGL_FALSE;	
		HInterface.idxType = FGL_INDEX_DATA_UINT;
		HInterface.numVSOut = 2;
		fglSetHInterface(&HInterface);
		
		FGL_Attribute HIAttr;		
		HIAttr.type = FGL_ATTRIB_DATA_FLOAT;		
		HIAttr.srcW = FGL_ATTRIB_ORDER_4TH;
		HIAttr.srcZ = FGL_ATTRIB_ORDER_3RD;
		HIAttr.srcY = FGL_ATTRIB_ORDER_2ND;
		HIAttr.srcX = FGL_ATTRIB_ORDER_1ST;	
		
		for(l=0; l<6; l++)
		{
			switch(l)
			{
				case 0:
					pColor = (unsigned int *)Color3b;
					HIAttr.bEndFlag = FGL_TRUE;
					HIAttr.type = FGL_ATTRIB_DATA_NBYTE;
					HIAttr.numComp = 4;
					fglSetAttribute(1, &HIAttr);
					DwordSize = 1;
					break;
				case 1:
					pColor = (unsigned int *)Color3i;
					HIAttr.bEndFlag = FGL_TRUE;
					HIAttr.type = FGL_ATTRIB_DATA_NINT;
					HIAttr.numComp = 4;
					fglSetAttribute(1, &HIAttr);
					DwordSize = 4;
					break;
				case 2:
					pColor = (unsigned int *)Color3s;
					HIAttr.bEndFlag = FGL_TRUE;
					HIAttr.type = FGL_ATTRIB_DATA_NSHORT;
					HIAttr.numComp = 4;
					fglSetAttribute(1, &HIAttr);
					DwordSize = 2;
					break;
				case 3:
					pColor = (unsigned int *)Color3ub;
					HIAttr.bEndFlag = FGL_TRUE;
					HIAttr.type = FGL_ATTRIB_DATA_NUBYTE;
					HIAttr.numComp = 4;
					fglSetAttribute(1, &HIAttr);
					DwordSize = 1;
					break;
				case 4:
					pColor = (unsigned int *)Color3ui;
					HIAttr.bEndFlag = FGL_TRUE;
					HIAttr.type = FGL_ATTRIB_DATA_NUINT;
					HIAttr.numComp = 4;
					fglSetAttribute(1, &HIAttr);
					DwordSize = 4;
					break;
				case 5:
					pColor = (unsigned int *)Color3us;
					HIAttr.bEndFlag = FGL_TRUE;
					HIAttr.type = FGL_ATTRIB_DATA_NUSHORT;
					HIAttr.numComp = 4;
					fglSetAttribute(1, &HIAttr);
					DwordSize = 2;
					break;
				default:
					pColor = (unsigned int *)Color3b;
					HIAttr.bEndFlag = FGL_TRUE;
					HIAttr.type = FGL_ATTRIB_DATA_NBYTE;
					HIAttr.numComp = 4;
					fglSetAttribute(1, &HIAttr);
					DwordSize = 1;
					break;
			}

			if(j == 0)
			{
				// Host Interface SFR Set
				HIAttr.bEndFlag = FGL_FALSE;
				HIAttr.type = FGL_ATTRIB_DATA_INT;
				HIAttr.numComp = 2;
				fglSetAttribute(0, &HIAttr);
				//WRITEREG(FGHI_ATTR2, 0x00000000);
				
				unsigned int uiTmpVertices = 4;
				unsigned int uiDummy = 0xFFFFFFFF;

				fglSendToFIFO(4, &uiTmpVertices); // 3 vertices
				fglSendToFIFO(4, &uiDummy);		  // (Non-Index Mode)

				pVerties = (unsigned int *)Vertex2i;

				for(i=0; i<4; i++)
				{
					VertiesTmp = (*pVerties++)+(l*30);
					fglSendToFIFO(4, &VertiesTmp);
					fglSendToFIFO(4, pVerties++);
					pColorTmp = pColor;
					fglSendToFIFO(4 * DwordSize, pColorTmp);					
				}
			}
			else
			{
				// Host Interface SFR Set
				HIAttr.bEndFlag = FGL_FALSE;
				HIAttr.type = FGL_ATTRIB_DATA_SHORT;
				HIAttr.numComp = 2;
				fglSetAttribute(0, &HIAttr);
				//WRITEREG(FGHI_ATTR2, 0x00000000);

				unsigned int uiTmpVertices = 4;
				unsigned int uiDummy = 0xFFFFFFFF;

				fglSendToFIFO(4, &uiTmpVertices); // 3 vertices
				fglSendToFIFO(4, &uiDummy);		  // (Non-Index Mode)

				for(i=0; i<4; i++)
				{
					//ShortVerties = ((unsigned int)Vertex2s[1+i*2]) & 0xFFFF;
					VertiesTmp = ((Vertex2s[0+i*2]+(l*30) & 0xFFFF) | ((((unsigned int)Vertex2s[1+i*2]) & 0xFFFF)<<16));
					fglSendToFIFO(4, &VertiesTmp);
					pColorTmp = pColor;
					fglSendToFIFO(4 * DwordSize, pColorTmp);						
				}
			}

			fglFlush(FGL_PIPESTATE_ALL);	// Pipeline status
		}

	}

	fglFlush(FGL_PIPESTATE_ALL);	// Pipeline status

	// Cache flush
	//fglSysCacheFlush();
	fglClearCache(FGL_CACHECTL_FLUSH_ZCACHE | FGL_CACHECTL_FLUSH_CCACHE);

	fglSysDelay(50);

//	drvsys_clear_buf((unsigned int*)(FIMG_COLOR_BUFFER+m_FBOffsize), CLEAR_SCREEN_SIZE, 0);


	return NO_ERROR;
}

