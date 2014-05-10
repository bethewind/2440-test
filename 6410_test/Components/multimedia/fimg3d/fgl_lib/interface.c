/*******************************************************************************
*
* NAME         : fimg_hi.c
* TITLE        : FIMGSE Register-Level API 
* AUTHOR       : Jaeho Kim
* CREATED      : 1 April 2006
*
* COPYRIGHT    : 2005 by Samsung Electronics Limited.
*                All rights reserved.  No part of this software, either
*                material or conceptual may be copied or distributed,
*                transmitted, transcribed, stored in a retrieval system
*                or translated into any human or computer language in any
*                form by any means, electronic, mechanical, manual or
*                other-wise, or disclosed to third parties without the
*                express written permission of Samsung Electronics. 
*                Semiconductor Business, System LSI Division,  Mobile 
*                Solution Development, Graphics IP Team in AP.
*
* DESCRIPTION  : APIs to control host interface
*
* PLATFORM     : ALL
* HISTORY      : 	 
* $RCSfile: fimg_hi.c,v $
* $Revision: 1.5 $
* $Author: cheolkyoo.kim $
* $Date: 2006/04/13 10:56:31 $
* $Locker:  $
* 
* $Source: C:/CVS/CVSrepository/FIMG-3DSE_SW/fimg3dse_fpga/fimg3d/device/fimg_hi.c,v $
* $State: Exp $
* $Log: fimg_hi.c,v $
* Revision 1.5  2006/04/13 10:56:31  cheolkyoo.kim
* Replace FG_TRUE with FGL_TRUE.
*
* Revision 1.4  2006/04/11 06:29:18  im.jaeho.kim
* Added fglDrawNonIndexArrays
*
* Revision 1.3  2006/04/10 05:18:53  im.jaeho.kim
* comments are added
*
* Revision 1.2  2006/04/06 06:27:42  im.jaeho.kim
* no message
*
*
*******************************************************************************/

#include "fgl.h"
#include "register.h"

#ifndef FGHI_FIFO_SIZE
#define FGHI_FIFO_SIZE 32
#endif // FGHI_FIFO_SIZE


/***************************************************************************** 
 * FUNCTIONS: fglGetNumEmptyFIFOSlots
 * SYNOPSIS: This function obtains how many FIFO slots are empty in host interface
 * PARAMETERS: [OUT] pNumSlots: the number of empty slots
 * RETURNS: FGL_ERR_NO_ERROR - if successful.
 * ERRNO:
 *****************************************************************************/

FGL_Error fglGetNumEmptyFIFOSlots(unsigned int *pNumSlots)
{
	//*pNumSlots = READREG(FGHI_FIFO_EMPTY_SPACE);
	READREGP(FGHI_FIFO_EMPTY_SPACE, *pNumSlots);

	return FGL_ERR_NO_ERROR;
}

/***************************************************************************** 
 * FUNCTIONS: fglDrawNonIndexArrays
 * SYNOPSIS: This function sends geometric data to rendering pipeline using non-index scheme.
 * PARAMETERS: [IN] numAttribs: number of input attributes
               [IN] pAttrib: array of input attributes
			   [IN] numVertices: number of vertices
			   [IN] ppData: array of pointers of input data
 * RETURNS: FGL_ERR_NO_ERROR - if successful.
 *          FGL_ERR_INVALID_VALUE - if FGL_Attribute::numComp is greater than 4
 * ERRNO:
 *****************************************************************************/

FGL_Error fglDrawNonIndexArrays(int numAttribs, FGL_Attribute *pAttrib, int numVertices, void **ppData)
{
	int i, j;

	// write the property of input attributes
	
	for(i=0; i<numAttribs; i++)
	{
		fglSetAttribute(i, &(pAttrib[i]));
	}

	// write the number of vertices

	WRITEREGF(FGHI_FIFO_ENTRY, numVertices);
	WRITEREG(FGHI_FIFO_ENTRY, 0xFFFFFFFF);

	for(i=0; i<numVertices; i++)
	{
		for(j=0; j<numAttribs; j++)
		{
			int n;

			if(pAttrib[j].numComp > 4)
				return FGL_ERR_INVALID_VALUE;

			switch(pAttrib[j].type)
			{
			// 1bytes
			case FGL_ATTRIB_DATA_BYTE:
			case FGL_ATTRIB_DATA_UBYTE:
			case FGL_ATTRIB_DATA_NBYTE:
			case FGL_ATTRIB_DATA_NUBYTE:
				{
					unsigned char bits[4];

					bits[0] = bits[1] = bits[2] = bits[3] = 0;
					for(n=0; n<pAttrib[j].numComp; n++)
					{
						bits[n] = ((unsigned char *)(ppData[j]))[pAttrib[j].numComp * i + n];
					}

					fglSendToFIFO(4, bits);

					break;
				}
			// 2bytes
			case FGL_ATTRIB_DATA_SHORT:
			case FGL_ATTRIB_DATA_USHORT:
			case FGL_ATTRIB_DATA_NSHORT:
			case FGL_ATTRIB_DATA_NUSHORT:
				{
					unsigned short bits[4];

					bits[0] = bits[1] = bits[2] = bits[3] = 0;
					for(n=0; n<pAttrib[j].numComp; n++)
					{
						bits[n] = ((unsigned short *)(ppData[j]))[pAttrib[j].numComp * i + n];
					}

					if(pAttrib[j].numComp > 2)
						fglSendToFIFO(8, bits);
					else
						fglSendToFIFO(4, bits);

					break;
				}
			// 4 bytes
			case FGL_ATTRIB_DATA_FIXED:
			case FGL_ATTRIB_DATA_NFIXED:
			case FGL_ATTRIB_DATA_FLOAT:
			case FGL_ATTRIB_DATA_INT:
			case FGL_ATTRIB_DATA_UINT:
			case FGL_ATTRIB_DATA_NINT:
			case FGL_ATTRIB_DATA_NUINT:
				{
					for(n=0; n<pAttrib[j].numComp; n++)
					{
						fglSendToFIFO(4, &(((unsigned int *)(ppData[j]))[pAttrib[j].numComp * i + n]));
					}

					break;
				}
			}
		}
	}

	return FGL_ERR_NO_ERROR;
}



/***************************************************************************** 
 * FUNCTIONS: fglSendToFIFO
 * SYNOPSIS: This function sends data to the 3D rendering pipeline
 * PARAMETERS: [IN] buffer: pointer of input data
 *             [IN] bytes: the total bytes of input data
 * RETURNS: FGL_ERR_NO_ERROR - if successful.
 *          FGL_ERR_UNKNOWN - otherwise
 * ERRNO:
 *****************************************************************************/


FGL_Error fglSendToFIFO(unsigned int bytes, void *buffer)
{
	unsigned int nEmptySpace = 0, nth = 0;
	unsigned char *ptr = buffer, bits[4];
	int i, diff;

	do {

		if(nEmptySpace > FGHI_FIFO_SIZE)
			return FGL_ERR_UNKNOWN;

		if(nEmptySpace == 0)
		{
			do {
				//nEmptySpace = READREG(FGHI_FIFO_EMPTY_SPACE);
				READREGP(FGHI_FIFO_EMPTY_SPACE, nEmptySpace);
			} while(nEmptySpace < 1);
		}

		diff = bytes - nth;
		
		if(diff < 4)
		{
			bits[0] = bits[1] = bits[2] = bits[3] = 0x00;

			for(i=0; i<diff; i++)
				bits[i] = ptr[nth++];

			WRITEREG(FGHI_FIFO_ENTRY, *((unsigned int *)bits));
			nEmptySpace --;
		}
		else
		{
			WRITEREG(FGHI_FIFO_ENTRY, *((unsigned int *)&(ptr[nth])));
			nEmptySpace --;
			nth += 4;
		}

	} while(nth < bytes);
	
	return FGL_ERR_NO_ERROR;
}

/***************************************************************************** 
 * FUNCTIONS: fglSetHInterface
 * SYNOPSIS: This function defines whether vertex buffer, vertex cache and 
 *            auto increment index scheme is active or not.
 *           It also defines data type of transfered index and
 *            the number of output attributes of vertex shader.
 * PARAMETERS: [IN] pHI: pointer of FGL_HInterface
 * RETURNS: FGL_ERR_NO_ERROR - if successful.
 *          FGL_ERR_INVALID_VALUE - if FGL_HInterface::numVSOut is greater than 15
 * ERRNO:
 *****************************************************************************/


FGL_Error fglSetHInterface(FGL_HInterface *pHI)
{
	unsigned int bits = 0;

	FGL_Error error = FGL_ERR_NO_ERROR;

	// enable vertex buffer
	// [31] == TRUE
	if(pHI->enableVtxBuffer == FGL_TRUE)
		bits |= 0x80000000;

	// index data type
	// [25:24]
	switch(pHI->idxType)
	{
	case FGL_INDEX_DATA_UINT: // 00
		break;
	case FGL_INDEX_DATA_USHORT: // 01
		bits |= 0x01000000;
		break;
	case FGL_INDEX_DATA_UBYTE:
		bits |= 0x03000000;
		break;
	}

	// auto increment
	// [16]

	if(pHI->enableAutoInc == FGL_TRUE)
		bits |= 0x00010000;

	// enable vertex cache
	// [4]

	if(pHI->enableVtxCache == FGL_TRUE)
		bits |= 0x00000010;

	// number of VS output
	// [3:0]

	if(pHI->numVSOut > 15)
		error = FGL_ERR_INVALID_VALUE;
	else
		bits |= pHI->numVSOut;

	// write

	WRITEREG(FGHI_HI_CTRL, bits);

	return error;
}

/***************************************************************************** 
 * FUNCTIONS: fglSetIndexOffset
 * SYNOPSIS: This function defines index offset which is used in the auto increment mode
 * PARAMETERS: [IN] offset: index offset value
 * RETURNS: FGL_ERR_NO_ERROR - if successful.
 * ERRNO:
 *****************************************************************************/

FGL_Error fglSetIndexOffset(unsigned int offset)
{
	WRITEREG(FGHI_IDX_OFFSET, offset);

	return FGL_ERR_NO_ERROR;
}

/***************************************************************************** 
 * FUNCTIONS: fglSetVtxBufferAddr
 * SYNOPSIS: This function defines the starting address in vertex buffer,
 *            which are used to send data into vertex buffer
 * PARAMETERS: [IN] address: the starting address
 * RETURNS: FGL_ERR_NO_ERROR - if successful.
 * ERRNO:
 *****************************************************************************/

FGL_Error fglSetVtxBufferAddr(unsigned int addr)
{
	WRITEREG(FGHI_VTXBUF_ADDR, addr);

	return FGL_ERR_NO_ERROR;
}

/***************************************************************************** 
 * FUNCTIONS: fglSendToVtxBuffer
 * SYNOPSIS: The function copies data into vertex buffer.
 * PARAMETERS: [IN] data: data issued into vertex buffer
 * RETURNS: FGL_ERR_NO_ERROR - if successful.
 * ERRNO:
 *****************************************************************************/

FGL_Error fglSendToVtxBuffer(unsigned int data)
{
	WRITEREG(FGHI_VTXBUF_ENTRY, data);

	return FGL_ERR_NO_ERROR;
}

/***************************************************************************** 
 * FUNCTIONS: fglSetAttribute
 * SYNOPSIS: This function specifies the property of attribute
 * PARAMETERS: [IN] attribIdx: the index of attribute, which is in [0-15]
 *             [IN] pAttribInfo: the pointer of FGL_Attribute
 * RETURNS: FGL_ERR_NO_ERROR - if successful.
 *          FGL_ERR_INVALID_VALUE - otherwise
 * ERRNO:
 *****************************************************************************/

FGL_Error fglSetAttribute(unsigned char attribIdx, FGL_Attribute *pAttribInfo)
{
	unsigned int bits = 0;
	unsigned int t, s;

	FGL_Error error = FGL_ERR_NO_ERROR;

	// end
	// [31] == TRUE

	if(pAttribInfo->bEndFlag == FGL_TRUE)
		bits |= 0x80000000;

	// data type
	// [15:12]

	t = pAttribInfo->type;
	bits |= (t << 0xc);

	//  number of components (1~4)
	// [9:8]

	if(pAttribInfo->numComp < 1 || pAttribInfo->numComp > 4)
		error = FGL_ERR_INVALID_VALUE;
	else
	{
		unsigned int c = 0;

		c = pAttribInfo->numComp - 1;
		bits |= (c << 0x8);
	}

	// srcW
	// [7:6]

	s = pAttribInfo->srcW;
	bits |= (s << 0x6);

	// srcZ
	// [5:4]

	s = pAttribInfo->srcZ;
	bits |= (s << 0x4);

	// srcY
	// [3:2]

	s = pAttribInfo->srcY;
	bits |= (s << 0x2);

	// srcX
	// [1:0]

	s = pAttribInfo->srcX;
	bits |= s;

	if(attribIdx > 16)
		error = FGL_ERR_INVALID_VALUE;
	else
	{
		WRITEREG(FGHI_ATTR0 + 0x4 * attribIdx, bits);
	}

	return error;
}

/***************************************************************************** 
 * FUNCTIONS: fglSetVtxBufAttrib
 * SYNOPSIS: This function defines the property of attribute in vertex buffer
 * PARAMETERS: [IN] attribIdx: the index of attribute
 *             [IN] pAttribInfo: the pointer of FGL_VtxBufAttrib
 * RETURNS: FGL_ERR_NO_ERROR - if successful.
 *          FGL_ERR_INVALID_VALUE - otherwise
 * ERRNO:
 *****************************************************************************/

FGL_Error fglSetVtxBufAttrib(unsigned char attribIdx, FGL_VtxBufAttrib *pAttribInfo)
{
	FGL_Error error = FGL_ERR_NO_ERROR;

	unsigned int bits = 0;

	if(attribIdx > 16)
		error = FGL_ERR_INVALID_VALUE;
	else
	{
		// base address

		WRITEREG(FGHI_VTXBUF_BASE0 + 0x4 * attribIdx, pAttribInfo->addr);

		//stride, number of input attribute

		bits |= (pAttribInfo->num & 0x0000ffff);
		bits |= (pAttribInfo->stride << 0x18);

		WRITEREG(FGHI_VTXBUF_CTRL0 + 0x4 * attribIdx, bits);
	}

	return error;
}
