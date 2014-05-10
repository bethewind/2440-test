/*******************************************************************************
*
* NAME         : fimg_pe.c
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
* DESCRIPTION  : APIs to control primitive engine
*
* PLATFORM     : ALL
* HISTORY      : 	 
* $RCSfile: fimg_pe.c,v $
* $Revision: 1.5 $
* $Author: cheolkyoo.kim $
* $Date: 2006/05/03 05:31:48 $
* $Locker:  $
* 
* $Source: C:/CVS/CVSrepository/FIMG-3DSE_SW/fimg3dse_fpga/fimg3d/device/fimg_pe.c,v $
* $State: Exp $
* $Log: fimg_pe.c,v $
* Revision 1.5  2006/05/03 05:31:48  cheolkyoo.kim
* no message
*
* Revision 1.4  2006/04/13 10:56:31  cheolkyoo.kim
* Replace FG_TRUE with FGL_TRUE.
*
* Revision 1.3  2006/04/10 05:19:00  im.jaeho.kim
* comments are added
*
* Revision 1.2  2006/04/06 06:27:47  im.jaeho.kim
* no message
*
*
*******************************************************************************/

#include "fgl.h"
#include "register.h"

/***************************************************************************** 
 * FUNCTIONS: fglSetVertex
 * SYNOPSIS: This function specifies the type of primitive, the number of 
 *            input attributes of vertex shader and shading model.
 * PARAMETERS: [IN] pVtx: the pointer of FGL_Vertex strucutre
 * RETURNS: FGL_ERR_NO_ERROR - if successful.
 *          FGL_ERR_INVALID_VALUE - if FGL_Vertex::numVSOut is greater than 9
 * ERRNO:
 *****************************************************************************/
FGL_Error 
fglSetVertex( FGL_Vertex *pVtx )
{
	FGL_Error error = FGL_ERR_NO_ERROR;

	unsigned int bits = 0;

	// primitive

	switch(pVtx->prim)
	{
	case FGL_PRIM_TRIANGLES: 	// [26] bit = 1
		bits |= 0x04000000;
		break;
	case FGL_PRIM_TRIANGLE_FAN: // [25] bit = 1
		bits |= 0x02000000;
		break;
	case FGL_PRIM_TRIANGLE_STRIP: // [24] bit = 1
		bits |= 0x01000000;
		break;
	case FGL_PRIM_LINES: 	 	// [23] bit = 1
		bits |= 0x00800000;
		break;
	case FGL_PRIM_LINE_LOOP: 	// [22] bit = 1
		bits |= 0x00400000;
		break;
	case FGL_PRIM_LINE_STRIP: 	// [21] bit = 1
		bits |= 0x00200000;
		break;
	case FGL_PRIM_POINTS: 	  	// [20] bit = 1
		bits |= 0x00100000;
		break;
	case FGL_PRIM_POINT_SPRITE: // [19] bit = 1
		bits |= 0x00080000;
		break;
	}
	if(pVtx->enablePointSize)
		bits |= 0x00040000;	

	// vout: range is [0~9]

	if(pVtx->numVSOut > 9)
		error = FGL_ERR_INVALID_VALUE;
	else
		bits |= (pVtx->numVSOut << 0xa);
	
	// shading model
	if(pVtx->shadeModel == FGL_SHADING_FLAT)
	{
		bits |= 0x00000200;
		if(pVtx->colorAttribIdx > 8)
			error = FGL_ERR_INVALID_VALUE;
		else
			bits |= (0x1 << pVtx->colorAttribIdx);
	}
	
	WRITEREG(FGPE_VTX_CONTEXT, bits);

	return error;
}

/***************************************************************************** 
 * FUNCTIONS: fglSetViewportParams
 * SYNOPSIS: This function specifies the viewport parameters.
 * PARAMETERS: [IN] bYFlip: true if you want y-flipped window coordindate
 *                          false, otherwise
 *             [IN] x0, y0: origin of viewport in window coordindate system
 *             [IN] px, py: width and height of viewport in terms of pixel
 *             [IN] H: height of window in terms of pixel
 * RETURNS: FGL_ERR_NO_ERROR - if successful.
 * ERRNO:
 *****************************************************************************/

FGL_Error fglSetViewportParams(FG_BOOL bYFlip, float x0, float y0, float px, float py, float H)
{
	// local variable declaration

	float half_px = px * 0.5f;
	float half_py = py * 0.5f;

	// ox: x-coordinate of viewport center
	float ox = x0 + half_px;
	// oy: y-coordindate of viewport center
	float oy;

	// ox
	WRITEREGF(FGPE_VIEWPORT_CENTER_XCOORD, ox);

	// oy
	if(bYFlip == FGL_TRUE)
		oy = (H - y0) - half_py;
	else
		oy = y0 + half_py;

	WRITEREGF(FGPE_VIEWPORT_CENTER_YCOORD, oy);

	// half of viewport width

	WRITEREGF(FGPE_VIEWPORT_HALF_WIDTH, half_px);

	// half of viewport height

	if(bYFlip == FGL_TRUE)
	{
		float t = -half_py;
		WRITEREGF(FGPE_VIEWPORT_HALF_HEIGHT, t);
	}
	else
		WRITEREGF(FGPE_VIEWPORT_HALF_HEIGHT, half_py);

	return FGL_ERR_NO_ERROR;
}

/***************************************************************************** 
 * FUNCTIONS: fglSetDepthRange
 * SYNOPSIS: This function defines an encoding for z-coordinate that's performed
 *            during the viewport transformation.
 * PARAMETERS: [IN] n: near value ( n should be in [0, 1])
 *             [IN] f: far value (f should be in [0, 1])
 * RETURNS: FGL_ERR_NO_ERROR - if successful.
 *          FGL_ERR_INVALID_VALUE - if near/far value is not in [0, 1]
 * ERRNO:
 *****************************************************************************/

FGL_Error fglSetDepthRange(float n, float f)
{
	float half_distance = (f - n) * 0.5f;
	float center = (f + n) * 0.5f;

	if(n < 0.0f || n > 1.0f) return FGL_ERR_INVALID_VALUE;
	if(f < 0.0f || f > 1.0f) return FGL_ERR_INVALID_VALUE;

	WRITEREGF(FGPE_DEPTH_HALF_DISTANCE, half_distance);
	WRITEREGF(FGPE_DEPTH_CENTER, center);

	return FGL_ERR_NO_ERROR;
}
