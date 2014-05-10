/******************************************************************************
*
* NAME         : fimg_ra.c
* TITLE        : FIMG-3DSE ver1.0 Register Level API 
* AUTHOR       : inhosens, Lee
* CREATED      : 06 Apr 2006
*
* COPYRIGHT    : Copyright(c) 2006 by Samsung Electronics Limited. All rights
*                reserved.  No part of this software, either material or 
*                conceptual may be copied or distributed, transmitted, 
*                transcribed, stored in a retrieval system or translated into
*                any human or computer language in any form by any means, 
*                electronic, mechanical, manual or other-wise, or disclosed 
*                to third parties without the express written permission of 
*                Samsung Electronics. Semiconductor Business, System LSI 
*                Division, Mobile Solution Development, Graphics IP Team 
*                in AP.
*
*
*	$Revision: 1.5 $
*	$Author: inhosens.lee $
*	$Date: 2006/04/14 00:18:34 $
*	$Locker:  $
*
*	$Source: C:/CVS/CVSrepository/FIMG-3DSE_SW/fimg3dse_fpga/fimg3d/device/fimg_ra.c,v $
*	$State: Exp $
*	$Log: fimg_ra.c,v $
*	Revision 1.5  2006/04/14 00:18:34  inhosens.lee
*	modified some mistypings
*	
*	Revision 1.4  2006/04/07 04:59:10  inhosens.lee
*	type and name change of a argument in fglSetFaceCullControl
*	
*	Revision 1.3  2006/04/06 06:56:45  inhosens.lee
*	Descriptions of register-level API of Raster engine
*	
*
*****************************************************************************/

#include "fgl.h"
#include "register.h"


/***************************************************************************** 
 * FUNCTIONS: fglSetPixelSamplePos
 *
 * 			refer to
 * 			drvsys_write_reg(FGRA_PIXEL_SAMPOS, (unsigned int) samp)
 *
 * SYNOPSIS: This function controls whether pixel is sampled at the center
 * 			 or corner of the sample
 * PARAMETERS: [IN] FGL_Sample samp: pixel sampling position
 * 			 		 	typedef enum FGL_SampleTag {
 * 			  				FGL_SAMPLE_CENTER,
 * 			  				FGL_SAMPLE_LEFTTOP
 * 			  			} FGL_Sample;
 * RETURNS: FGL_ERR_NO_ERROR - if successful.
 *          FGL_ERR_INVALID_VALUE - if specified sample position is invalid
 * ERRNO: FGL_ERR_INVALID_VALUE = 7
 *****************************************************************************/
FGL_Error
fglSetPixelSamplePos(FGL_Sample samp)
{
	unsigned int uSamp = (unsigned int) samp;
	if(uSamp > (unsigned int)FGL_SAMPLE_LEFTTOP)
	{
		return FGL_ERR_INVALID_VALUE;
	}
	
	WRITEREG(FGRA_PIXEL_SAMPOS, uSamp);

	return FGL_ERR_NO_ERROR;
}

/***************************************************************************** 
 * FUNCTIONS: fglEnableDepthOffset
 *
 * 			refer to
 * 			drvsys_write_reg(FGRA_DEPTH_OFFSET_EN, (FG_BOOL) enable)
 *
 * SYNOPSIS: This function decides to use the depth offset
 *           Note : This function affects polygon, not point and line.
 * PARAMETERS: [IN] FGL_Bool enable: FG_TRUE for use. default value is FG_FALSE 
 * RETURNS: FGL_ERR_NO_ERROR - always
 * ERRNO:
 *****************************************************************************/
FGL_Error
fglEnableDepthOffset(FG_BOOL enable)
{
	WRITEREG(FGRA_DEPTH_OFFSET_EN, enable);

	return FGL_ERR_NO_ERROR;
}

/***************************************************************************** 
 * FUNCTIONS: fglSetDepthOffsetParam
 *
 * 			refer to
 * 				drvsys_write_reg(FGRA_DEPTH_OFFSET_FACTOR, value);
 * 				drvsys_write_reg(FGRA_DEPTH_OFFSET_UNIT, value);
 *
 * SYNOPSIS: This function sets depth offset parameters one by one
 *           Note : This function affects polygon, not point and line.
 * PARAMETERS: [IN] FGL_DepthOffsetParam param: depth offset parameter to be set
 * 	            	typedef enum FGL_DepthOffsetParamTag {
 * 	            		FGL_DEPTH_OFFSET_FACTOR,
 * 	            		FGL_DEPTH_OFFSET_UNITS
 * 	            	} FGL_DepthOffsetParam;
 *             [IN] unsigned int value: specified parameter value
 * RETURNS: FGL_ERR_NO_ERROR - if successful.
 *          FGL_ERR_INVALID_VALUE - if an invalid parameter is specified.
 * ERRNO: FGL_ERR_INVALID_VALUE = 7
 *****************************************************************************/
FGL_Error
fglSetDepthOffsetParam(FGL_DepthOffsetParam param, unsigned int value)
{
	switch (param)
	{
	case (FGL_DEPTH_OFFSET_FACTOR) : 
		WRITEREG(FGRA_DEPTH_OFFSET_FACTOR, value);
		return FGL_ERR_NO_ERROR;
	case (FGL_DEPTH_OFFSET_UNITS) :
		WRITEREG(FGRA_DEPTH_OFFSET_UNIT, value);
		return FGL_ERR_NO_ERROR;
	case (FGL_DEPTH_OFFSET_R) :
		WRITEREG(FGRA_DEPTH_OFFSET_RVAL, value);
		return FGL_ERR_NO_ERROR;
	default :
		return FGL_ERR_INVALID_VALUE;
	}
}

/***************************************************************************** 
 * FUNCTIONS: fglSetFaceCullControl
 *
 * 			refer to
 * 				drvsys_write_reg(FGRA_BACKFACE_CULL,
 * 					((unsigned int)enable)<<3)|(bCCW<<2)|(unsigned int)face);
 *
 * SYNOPSIS: This function controls back-face culling.
 * PARAMETERS: [IN] FG_BOOL enable: FG_TRUE for using back-face cull.
 * 									default is FG_FALSE
 *             [IN] FG_BOOL bCW: FG_TRUE for make a clock-wise face front
 *             					 default is FG_FALSE
 *             [IN] FGL_Face face: culling face
 * 	            	typedef enum FGL_FaceTag {
 * 	            		FGL_FACE_BACK,
 * 	            		FGL_FACE_FRONT,
 * 	            		FGL_FACE_FRON_AND_BACK = 3
 * 	            	} FGL_Face;
 * RETURNS: FGL_ERR_NO_ERROR - if successful.
 *          FGL_ERR_INVALID_VALUE - if parameters exceed the acceptable range,
 * ERRNO: FGL_ERR_INVALID_VALUE = 7
 *****************************************************************************/
FGL_Error
fglSetFaceCullControl(FG_BOOL enable,
	   				FG_BOOL bCW,
				   	FGL_Face face)
{
	unsigned int uFace = (unsigned int) face;

	// uFace != FGL_FACE_RESERVED(2)
	if ( ((uFace > (unsigned int)FGL_FACE_FRONT_AND_BACK)
				|| (uFace == FGL_FACE_RESERVED)) )
	{
		return FGL_ERR_INVALID_VALUE;
	}

	uFace |= (enable << BITSHIFT_RA_FACECULL_ENABLE)
			| (bCW << BITSHIFT_RA_FACECULL_FACEDIR);

	WRITEREG(FGRA_BACKFACE_CULL, uFace);

	return FGL_ERR_NO_ERROR;
}

/***************************************************************************** 
 * FUNCTIONS: fglSetYClip
 *
 * 			refer to
 * 				drvsys_write_reg(FGRA_CLIP_YCORD, (ymax << 16) | ymin)
 *
 * SYNOPSIS: This function sets clipping plan in Y coordinate.
 * PARAMETERS: [IN] unsigned int ymin: Y clipping min. coordinate.
 *             [IN] unsigned int ymax: Y clipping max. coordinate.
 * RETURNS: FGL_ERR_NO_ERROR - if successful.
 *          FGL_ERR_INVALID_VALUE - if parameters exceed the acceptable range,
 * ERRNO: FGL_ERR_INVALID_VALUE = 7
 *****************************************************************************/
FGL_Error
fglSetYClip(unsigned int ymin, unsigned int ymax)
{
	unsigned int value;

	if( (ymin > MAXVALUE_RA_COORD) || (ymax > MAXVALUE_RA_COORD) )
	{
		return FGL_ERR_INVALID_VALUE;
	}

	value = (ymax << BITSHIFT_RA_CLIP_MAX) | ymin;

	WRITEREG(FGRA_CLIP_YCORD, value);

	return FGL_ERR_NO_ERROR;
}

/***************************************************************************** 
 * FUNCTIONS: fglSetLODControl
 *
 * 			refer to
 * 				drvsys_write_reg(FGRA_LOD_CTRL, ctl)
 *
 * SYNOPSIS: This function sets LOD calculation control.
 * PARAMETERS: [IN] unsigned int ctl: 8 lodcon values
 *
 * 					[23:21] {DDY,DDX,LOD} for attribute 7
 * 					[20:18] {DDY,DDX,LOD} for attribute 6
 * 					[17:15] {DDY,DDX,LOD} for attribute 5
 * 					[14:12] {DDY,DDX,LOD} for attribute 4
 * 					[11:9] {DDY,DDX,LOD} for attribute 3
 * 					[8:6] {DDY,DDX,LOD} for attribute 2
 * 					[5:3] {DDY,DDX,LOD} for attribute 1
 * 					[2:01] {DDY,DDX,LOD} for attribute 0
 * RETURNS: FGL_ERR_NO_ERROR - if successful.
 *          FGL_ERR_INVALID_VALUE - if ctl exceeds the acceptable range,
 * ERRNO: FGL_ERR_INVALID_VALUE = 7
 *****************************************************************************/
FGL_Error
fglSetLODControl(unsigned int ctl)
{
	if(ctl > MAXVALUE_RA_LOD)
	{
		return FGL_ERR_INVALID_VALUE;
	}

	WRITEREG(FGRA_LOD_CTRL, ctl);

	return FGL_ERR_NO_ERROR;
}

/***************************************************************************** 
 * FUNCTIONS: fglSetLODRegister
 *
 * 			refer to
 * 				drvsys_write_reg(FGRA_LOD_CTRL, ctl)
 *
 * SYNOPSIS: This function sets LOD control register.
 * PARAMETERS: [IN] FGL_LodCoeff lodCon0 : lodcon value for attribute 0
 *					typedef enum FGL_LodCoeffTag
 *					{
 *						FGL_LODCOEFF_DISABLE,
 *						FGL_LODCOEFF_ENABLE_LOD,
 *						FGL_LODCOEFF_ENABLE_DDX,
 *						FGL_LODCOEFF_ENABLE_DDX_LOD,
 *						FGL_LODCOEFF_ENABLE_DDY,
 *						FGL_LODCOEFF_ENABLE_DDY_LOD,
 *						FGL_LODCOEFF_ENABLE_DDY_DDX,
 *						FGL_LODCOEFF_ENABLE_ALL
 *					} FGL_LodCoeff;
 *				[IN] FGL_LodCoeff lodCon1 : lodcon value for attribute 1
 *				[IN] FGL_LodCoeff lodCon2 : lodcon value for attribute 2
 *				[IN] FGL_LodCoeff lodCon3 : lodcon value for attribute 3
 *				[IN] FGL_LodCoeff lodCon4 : lodcon value for attribute 4
 *				[IN] FGL_LodCoeff lodCon5 : lodcon value for attribute 5
 *				[IN] FGL_LodCoeff lodCon6 : lodcon value for attribute 6
 *				[IN] FGL_LodCoeff lodCon7 : lodcon value for attribute 7
 * RETURNS: FGL_ERR_NO_ERROR - if successful.
 *          FGL_ERR_INVALID_VALUE - if ctl exceeds the acceptable range,
 * ERRNO: FGL_ERR_INVALID_VALUE = 7
 *****************************************************************************/
FGL_Error
fglSetLODRegister(FGL_LodCoeff lodCon0,
					FGL_LodCoeff lodCon1,
					FGL_LodCoeff lodCon2,
					FGL_LodCoeff lodCon3,
					FGL_LodCoeff lodCon4,
					FGL_LodCoeff lodCon5,
					FGL_LodCoeff lodCon6,
					FGL_LodCoeff lodCon7)
{
	unsigned int value;

	if (lodCon0 > FGL_LODCOEFF_ENABLE_ALL
			|| lodCon1 > FGL_LODCOEFF_ENABLE_ALL
			|| lodCon2 > FGL_LODCOEFF_ENABLE_ALL
			|| lodCon3 > FGL_LODCOEFF_ENABLE_ALL
			|| lodCon4 > FGL_LODCOEFF_ENABLE_ALL
			|| lodCon5 > FGL_LODCOEFF_ENABLE_ALL
			|| lodCon6 > FGL_LODCOEFF_ENABLE_ALL
			|| lodCon7 > FGL_LODCOEFF_ENABLE_ALL)
	{
		return FGL_ERR_INVALID_VALUE;
	}

	value = (lodCon7 << BITSHIFT_RA_LODCON7)
			| (lodCon6 << BITSHIFT_RA_LODCON6)
			| (lodCon5 << BITSHIFT_RA_LODCON5)
			| (lodCon4 << BITSHIFT_RA_LODCON4)
			| (lodCon3 << BITSHIFT_RA_LODCON3)
			| (lodCon2 << BITSHIFT_RA_LODCON2)
			| (lodCon1 << BITSHIFT_RA_LODCON1)
			| lodCon0;

	WRITEREG(FGRA_LOD_CTRL, value);

	return FGL_ERR_NO_ERROR;
}

/***************************************************************************** 
 * FUNCTIONS: fglSetXClip
 *
 * 			refer to
 * 				drvsys_write_reg(FGRA_CLIP_XCORD, (xmax << 16) | xmin)
 *
 * SYNOPSIS: This function sets clipping plan in X coordinate.
 * PARAMETERS: [IN] unsigned int xmin: X clipping min. coordinate.
 *             [IN] unsigned int xmax: X clipping max. coordinate.
 * RETURNS: FGL_ERR_NO_ERROR - if successful.
 *          FGL_ERR_INVALID_VALUE - if parameters exceed the acceptable range,
 * ERRNO: FGL_ERR_INVALID_VALUE = 7
 *****************************************************************************/
FGL_Error
fglSetXClip(unsigned int xmin, unsigned int xmax)
{
	unsigned int value;

	if ( (xmin > MAXVALUE_RA_COORD) || (xmax > MAXVALUE_RA_COORD) )
	{
		return FGL_ERR_INVALID_VALUE;
	}

	value = (xmax << BITSHIFT_RA_CLIP_MAX) | xmin;

	WRITEREG(FGRA_CLIP_XCORD, value);

	return FGL_ERR_NO_ERROR;
}

/***************************************************************************** 
 * FUNCTIONS: fglSetPointWidth
 *
 * 			refer to
 * 				drvsys_write_reg(FGRA_PWIDTH, *(unsigned int *)&pWidth)
 *
 * SYNOPSIS: This function sets point width register.
 * PARAMETERS: [IN] float pWidth: Point width.
 * RETURNS: FGL_ERR_NO_ERROR - always.
 *****************************************************************************/
FGL_Error
fglSetPointWidth(float pWidth)
{
	WRITEREGF(FGRA_PWIDTH, pWidth);

	return FGL_ERR_NO_ERROR;
}

/***************************************************************************** 
 * FUNCTIONS: fglSetMinimumPointWidth
 *
 * 			refer to
 * 				drvsys_write_reg(FGRA_PSIZE_MIN, *(unsigned int *)&pWidthMin)
 *
 * SYNOPSIS: This function sets minimum point width register.
 * PARAMETERS: [IN] float pWidthMin: Minimum point width.
 * RETURNS: FGL_ERR_NO_ERROR - always.
 *****************************************************************************/
FGL_Error
fglSetMinimumPointWidth(float pWidthMin)
{
	WRITEREGF(FGRA_PSIZE_MIN, pWidthMin);

	return FGL_ERR_NO_ERROR;
}

/***************************************************************************** 
 * FUNCTIONS: fglSetMaximumPointWidth
 *
 * 			refer to
 * 				drvsys_write_reg(FGRA_PSIZE_MAX, *(unsigned int *)&pWidthMax)
 *
 * SYNOPSIS: This function sets maximum point width register.
 * PARAMETERS: [IN] float pWidthMax: Maximum point width.
 * RETURNS: FGL_ERR_NO_ERROR - always.
 *****************************************************************************/
FGL_Error
fglSetMaximumPointWidth(float pWidthMax)
{
	WRITEREGF(FGRA_PSIZE_MAX, pWidthMax);

	return FGL_ERR_NO_ERROR;
}


/***************************************************************************** 
 * FUNCTIONS: fglSetCoordReplace
 *
 * 			refer to
 * 				drvsys_write_reg(FGRA_COORDREPLACE, (1 << coordReplaceNum))
 *
 * SYNOPSIS: This function is used only in point sprite rendering.
 * 			Only one bit chooses generated texture coordinate for point sprite.
 * PARAMETERS: [IN] unsigned int coordReplaceNum :
 * 						Attribute number for texture coord. of point sprite.
 * RETURNS: FGL_ERR_NO_ERROR - if successful.
 *          FGL_ERR_INVALID_VALUE - if parameters exceed the acceptable range,
 * ERRNO: FGL_ERR_INVALID_VALUE = 7
 *****************************************************************************/
FGL_Error
fglSetCoordReplace(unsigned int coordReplaceNum)
{
	if(coordReplaceNum > MAXVALUE_RA_COODREPLACE)
	{
		return FGL_ERR_INVALID_VALUE;
	}

	
	WRITEREG(FGRA_COORD_REPLACE, (unsigned int)(1 << coordReplaceNum));
	return FGL_ERR_NO_ERROR;
}

/***************************************************************************** 
 * FUNCTIONS: fglSetLineWidth
 *
 * 			refer to
 * 				drvsys_write_reg(FGRA_LWIDTH, *(unsigned int *)&lWidth)
 *
 * SYNOPSIS: This function sets line width register.
 * PARAMETERS: [IN] float lWidth: Line width.
 * RETURNS: FGL_ERR_NO_ERROR - always.
 *****************************************************************************/
FGL_Error
fglSetLineWidth(float lWidth)
{
	WRITEREGF(FGRA_LWIDTH, lWidth);

	return FGL_ERR_NO_ERROR;
}


