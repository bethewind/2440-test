/******************************************************************************
*
* NAME         : fimg_device.c
* TITLE        : FIMG-3DSE ver1.0 Low Level API 
* AUTHOR       : Thomas, Kim
* CREATED      : 29 Mar 2006
*
* COPYRIGHT    : Copyright(c) 2005 by Samsung Electronics Limited. All rights
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
* DESCRIPTION  : 
*
* PLATFORM     : ALL
* HISTORY      : 29 Mar 2006 created	 
* CVS
* $RCSfile: fimg_pf.c,v $
* $Revision: 1.3 $
* $Author: cheolkyoo.kim $
* $Date: 2006/05/08 05:39:30 $
* $Locker:  $
*       
* $Source: C:/CVS/CVSrepository/FIMG-3DSE_SW/fimg3dse_fpga/fimg3d/device/fimg_pf.c,v $
* $State: Exp $
* $Log: fimg_pf.c,v $
* Revision 1.3  2006/05/08 05:39:30  cheolkyoo.kim
* no message
*
* Revision 1.2  2006/04/13 10:56:31  cheolkyoo.kim
* Replace FG_TRUE with FGL_TRUE.
*
* Revision 1.1  2006/04/05 03:13:16  cheolkyoo.kim
* Initial update per-fragment block's register-level API.
*
*****************************************************************************/

/*****************************************************************************
  INCLUDES 
 *****************************************************************************/
#include "fgl.h"
#include "macros.h"
#include "register.h"

/*****************************************************************************
  DEFINES 
 *****************************************************************************/

#define FGL_PF_UNIT_VALIDATOR		(								\
										FGL_PF_SCISSOR			|	\
										FGL_PF_ALPHA			|	\
										FGL_PF_STENCIL			|	\
										FGL_PF_DEPTH			|	\
										FGL_PF_BLENDING			|	\
										FGL_PF_LOGICALOP			\
									)

#define FGL_COMPARE_FUNC_VALIDATOR	(								    \
									    (1 << FGL_COMP_NEVER)		|	\
										(1 << FGL_COMP_ALWAYS)	    |	\
										(1 << FGL_COMP_LESS)	    |	\
										(1 << FGL_COMP_LEQUAL)		|	\
										(1 << FGL_COMP_EQUAL)		|	\
										(1 << FGL_COMP_GREATER)		|	\
										(1 << FGL_COMP_GEQUAL)		|	\
										(1 << FGL_COMP_NOTEQUAL)		\
									)
/* STENCIL */
#define FGL_FACE_VALIDATOR	        (								    \
									    (1 << FGL_FACE_BACK)    	|	\
										(1 << FGL_FACE_FRONT)    	|	\
										(1 << FGL_FACE_FRONT_AND_BACK)	\
									)


#define FGL_STENCIL_OP_VALIDATOR	(								    \
									    (1 << FGL_ACT_KEEP)			|	\
										(1 << FGL_ACT_ZERO)	    	|	\
										(1 << FGL_ACT_REPLACE)	    |	\
										(1 << FGL_ACT_INCR)			|	\
										(1 << FGL_ACT_DECR)			|	\
										(1 << FGL_ACT_INVERT)		|	\
										(1 << FGL_ACT_INCRWRAP)		|	\
										(1 << FGL_ACT_DECRWRAP)			\
									)

/* BLEND */
#define FGL_BLEND_FUNC_VALIDATOR	(													\
										(1 << FGL_BLEND_ZERO)		           		|	\
										(1 << FGL_BLEND_ONE)	                    |	\
										(1 << FGL_BLEND_SRC_COLOR)               	|	\
										(1 << FGL_BLEND_ONE_MINUS_SRC_COLOR)		|	\
										(1 << FGL_BLEND_DST_COLOR)		        	|	\
										(1 << FGL_BLEND_ONE_MINUS_DST_COLOR)		|   \
										(1 << FGL_BLEND_SRC_ALPHA)		        	|	\
										(1 << FGL_BLEND_ONE_MINUS_SRC_ALPHA)		|	\
										(1 << FGL_BLEND_DST_ALPHA)		        	|   \
										(1 << FGL_BLEND_ONE_MINUS_DST_ALPHA)		|	\
										(1 << FGL_BLEND_CONSTANT_COLOR)		    	|	\
										(1 << FGL_BLEND_ONE_MINUS_CONSTANT_COLOR)	|   \
										(1 << FGL_BLEND_CONSTANT_ALPHA)		    	|	\
										(1 << FGL_BLEND_ONE_MINUS_CONSTANT_ALPHA)	|   \
										(1 << FGL_BLEND_SRC_ALPHA_SATURATE)				\
									)

#define FGL_BLEND_EQUA_VALIDATOR	(										\
										(1 << FGL_EQ_ADD)		    	|	\
									 	(1 << FGL_EQ_SUBTRACT)	    	|	\
									 	(1 << FGL_EQ_REV_SUBTRACT)		|	\
									 	(1 << FGL_EQ_MIN)		    	|	\
									 	(1 << FGL_EQ_MAX)		    		\
									)

/* LOGICAL OP */                                    
#define FGL_LOGIC_OP_VALIDATOR  	(								    \
	                                    (1 << FGL_OP_CLEAR)           |	\
	                                    (1 << FGL_OP_AND)             |	\
	                                    (1 << FGL_OP_AND_REVERSE)     |	\
	                                    (1 << FGL_OP_COPY)            |	\
	                                    (1 << FGL_OP_AND_INVERTED)    |	\
	                                    (1 << FGL_OP_NOOP)            |	\
	                                    (1 << FGL_OP_XOR)             |	\
	                                    (1 << FGL_OP_OR)              |	\
	                                    (1 << FGL_OP_NOR)             |	\
	                                    (1 << FGL_OP_EQUIV)           |	\
	                                    (1 << FGL_OP_INVERT)          |	\
	                                    (1 << FGL_OP_OR_REVERSE)      |	\
	                                    (1 << FGL_OP_COPY_INVERTED)   |	\
	                                    (1 << FGL_OP_OR_INVERTED)     |	\
	                                    (1 << FGL_OP_NAND)            |	\
	                                    (1 << FGL_OP_SET)               \
									)

#define FGL_PIXEL_FORMAT_VALIDATOR	(									\
										(1 << FGL_PIXEL_RGB555)		|	\
										(1 << FGL_PIXEL_RGB565)		|	\
										(1 << FGL_PIXEL_ARGB4444)	|	\
										(1 << FGL_PIXEL_ARGB1555)	|	\
										(1 << FGL_PIXEL_ARGB0888)	|	\
										(1 << FGL_PIXEL_ARGB8888)		\
									) 
 
/* Definitions used to detect enum ranges for expression validation */
/* Per-fragment */
#define PF_UNIT_VALID(enable)										\
		FGL_VALID_BITFIELD(enable, FGL_PF_UNIT_VALIDATOR)

/* Scissor reference value is an 11 bit field */
#define SCISSOR_REF_VALUE_VALID(scissorref)	(scissorref < SCISSOR_MAX_SIZE)

#define ALPHA_COMPARE_FUNC_VALID(compfunc)                          \
		FGL_ENUM_IN_SET(compfunc, FGL_COMPARE_FUNC_VALIDATOR)

/* Alpha reference value is an 8 bit field */
#define ALPHA_REF_VALUE_VALID(alpharef)	(alpharef < 0x100)

/* Stencil reference value is an 8 bit field */
#define STENCIL_REF_VALUE_VALID(stencilref)	(stencilref < 0x100)

#define STENCIL_FACE_VALID(face)									\
		FGL_ENUM_IN_SET(face, FGL_FACE_VALIDATOR)

#define STENCIL_OP_VALID(stencilop)									\
		FGL_ENUM_IN_SET(stencilop, FGL_STENCIL_OP_VALIDATOR)

#define STENCIL_COMPARE_FUNC_VALID(stencilfunc)                     \
        FGL_ENUM_IN_SET(stencilfunc, FGL_COMPARE_FUNC_VALIDATOR)

/* Stencil mask value is an 8 bit field */
#define STENCIL_MASK_VALID(stencilmask)	(stencilmask < 0x100)

#define DEPTH_COMPARE_FUNC_VALID(compfunc)							\
		FGL_ENUM_IN_SET(compfunc, FGL_COMPARE_FUNC_VALIDATOR)

#define BLEND_EQUATION_VALID(equa)                                  \
        FGL_ENUM_IN_SET(equa, FGL_BLEND_EQUA_VALIDATOR)

#define BLEND_SOURCE_FUNC_VALID(srcfunc)							\
		FGL_ENUM_IN_SET(srcfunc, FGL_BLEND_FUNC_VALIDATOR)

#define BLEND_DEST_FUNC_VALID(dstfunc)								\
		FGL_ENUM_IN_SET(dstfunc, FGL_BLEND_FUNC_VALIDATOR)

#define BLEND_SOURCE_ALPHA_FUNC_VALID(srcalphafunc)					\
		FGL_ENUM_IN_SET(srcalphafunc, FGL_BLEND_FUNC_VALIDATOR)

#define BLEND_DEST_ALPHA_FUNC_VALID(dstalphafunc)	                \
        FGL_ENUM_IN_SET(dstalphafunc, FGL_BLEND_FUNC_VALIDATOR)

#define LOGIC_OP_FUNC_VALID(opcode)									\
        FGL_ENUM_IN_SET(opcode, FGL_LOGIC_OP_VALIDATOR)

#define PIXEL_FORMAT_VALID(format)									\
        FGL_ENUM_IN_SET(format, FGL_PIXEL_FORMAT_VALIDATOR)

/* Alpha constant alpha value is an 8 bit field */
#define ALPHA_THRESHOLD_VALUE_VALID(threshold)	(threshold < 0x100)

/* Alpha constant alpha value is an 8 bit field */
#define ALPHA_CONSTANT_VALUE_VALID(constalpha)	(constalpha < 0x100)


/*****************************************************************************
  TYPEDEFS 
 *****************************************************************************/

/*****************************************************************************
  CONSTANT MACRO 
 *****************************************************************************/

/*****************************************************************************
  FUNCTION MACRO 
 *****************************************************************************/

/*****************************************************************************
  TYPEDEFS 
 *****************************************************************************/

/*****************************************************************************
  ENUMS 
 *****************************************************************************/

/*****************************************************************************
 EXTERN 
 *****************************************************************************/

/*****************************************************************************
  NON_STATIC GLOBALS 
 *****************************************************************************/


/*****************************************************************************
  STATIC GLOBALS 
 *****************************************************************************/

/* The single FIMG device */

/* The FIMG device global variable */

/* structure declarations */
/* in some sort of meaningful order but, If defining large numbers of 
   essentially-independent utility functions, consider alphabetical order */

/* Internal Functions */ 

/* Functions */

// Per-fragment Unit Low-level API 

/***************************************************************************** 
 * FUNCTIONS: fglEnablePerFragUnit
 * SYNOPSIS: This function enables and disables various per-fragment capabilities: 
 *           scissor test, alpha test, stencil test, depth test, blending 
 *           and logical operation.
 * PARAMETERS: [IN] unit: a enum value indicating an per-fragment capability. 
 *             [IN] enable: a boolean value to enable or disable.
 * RETURNS: FGL_ERR_NO_ERROR - if successful.
 *          FGL_ERR_INVALID_PARAMETER - unit was not one of the values listed 
 *                                      in the FGL_PerFragUnit
 * ERRNO:   FGL_ERR_NO_ERROR            1	    
 *          FGL_ERR_INVALID_PARAMETER   3
 *****************************************************************************/
FGL_Error
fglEnablePerFragUnit( FGL_PerFragUnit unit, FGL_BOOL enable )
{
	FGL_Error ret = FGL_ERR_UNKNOWN;
	FGL_BOOL bValidUnit = FGL_FALSE;
	unsigned int nUnitEnableVal = 0;
	
	bValidUnit = PF_UNIT_VALID(unit);
	//nUnitEnableVal =0;

	if(bValidUnit)
	{
		switch(unit)
		{
		default:
		case FGL_PF_SCISSOR: 
			READREGP(FGPF_SCISSOR_XCORD, nUnitEnableVal);
			FGL_SET_BITFIELD(nUnitEnableVal, 31:31, enable);
			WRITEREG(FGPF_SCISSOR_XCORD, nUnitEnableVal);
			break;
		case FGL_PF_ALPHA: 
			READREGP(FGPF_ALPHA, nUnitEnableVal);
			FGL_SET_BITFIELD(nUnitEnableVal, 0:0, enable);
			WRITEREG(FGPF_ALPHA, nUnitEnableVal);
			break;
		case FGL_PF_STENCIL: 
			READREGP(FGPF_FRONTFACE_STENCIL, nUnitEnableVal);
			FGL_SET_BITFIELD(nUnitEnableVal, 0:0, enable);
			WRITEREG(FGPF_FRONTFACE_STENCIL, nUnitEnableVal);
			break;
		case FGL_PF_DEPTH: 
			READREGP(FGPF_DEPTH, nUnitEnableVal);
			FGL_SET_BITFIELD(nUnitEnableVal, 0:0, enable);
			WRITEREG(FGPF_DEPTH, nUnitEnableVal);
			break;
		case FGL_PF_BLENDING: 
			READREGP(FGPF_BLEND, nUnitEnableVal);
			FGL_SET_BITFIELD(nUnitEnableVal, 0:0, enable);
			WRITEREG(FGPF_BLEND, nUnitEnableVal);
			break;
		case FGL_PF_LOGICALOP: 
			READREGP(FGPF_LOGIC_OP, nUnitEnableVal);
			FGL_SET_BITFIELD(nUnitEnableVal, 0:0, enable);
			WRITEREG(FGPF_LOGIC_OP, nUnitEnableVal);
			break;
		}
		
		ret = FGL_ERR_NO_ERROR;
	}
	else
	{
		//printf((DBG_ERROR, "Cannot set enable per-fragment units - invalid unit."));
		ret = FGL_ERR_INVALID_PARAMETER;
	}
	
	return ret;	
}

/***************************************************************************** 
 * FUNCTIONS: fglSetScissorParams
 * SYNOPSIS: This function specifies an arbitary screen-aligned rectangle 
 *           outside of which fragments will be discarded.  
 * PARAMETERS: [IN] xMax: the maximum x coordinates of scissor box.
 *             [IN] xMin: the minimum x coordinates of scissor box.
 *             [IN] yMax: the maximum y coordiantes of scissor box.
 *             [IN] yMin: the minimum y coordinates of scissor box.
 * RETURNS: FGL_ERR_NO_ERROR - if successful.
 *          FGL_ERR_INVALID_VALUE - either x or y coordinates was not an 
 *                                  accepted value.
 * ERRNO:   FGL_ERR_NO_ERROR            1	    
 *          FGL_ERR_INVALID_VALUE       7	
 *****************************************************************************/
FGL_Error 
fglSetScissorParams ( 
						unsigned int xMax, 
						unsigned int xMin,
   						unsigned int yMax, 
   						unsigned int yMin
   					)
{
	FGL_Error ret = FGL_ERR_UNKNOWN;
	FGL_BOOL bValidXCoord = FGL_FALSE;
	FGL_BOOL bValidYCoord = FGL_FALSE;
	
	unsigned int nScissorXVal =0;
	unsigned int nScissorYVal =0;
	
	bValidXCoord = SCISSOR_REF_VALUE_VALID(xMax) && SCISSOR_REF_VALUE_VALID(xMin) && (xMax >= xMin);
	bValidYCoord = SCISSOR_REF_VALUE_VALID(yMax) && SCISSOR_REF_VALUE_VALID(yMin) && (yMax >= yMin);

	if (bValidXCoord && bValidYCoord)
	{
		READREGP(FGPF_SCISSOR_XCORD, nScissorXVal);
		FGL_SET_BITFIELD(nScissorXVal, 27:16, xMax);
		FGL_SET_BITFIELD(nScissorXVal, 11:0, xMin);
		WRITEREG(FGPF_SCISSOR_XCORD, nScissorXVal);
		
		FGL_SET_BITFIELD(nScissorYVal, 27:16, yMax);
		FGL_SET_BITFIELD(nScissorYVal, 11:0, yMin);
		WRITEREG(FGPF_SCISSOR_YCORD, nScissorYVal);

		ret = FGL_ERR_NO_ERROR;
	}
	else
	{
		if(!bValidXCoord)
		{
			//printf((DBG_ERROR, "Cannot set scissor test value - invalid x coordinate value"));
		}
		
		if(!bValidYCoord)
		{
			//printf((DBG_ERROR, "Cannot set scissor test value - invalid Y coordinate value"));
		}

		ret = FGL_ERR_INVALID_VALUE;
		
	}

	return ret;
}


/***************************************************************************** 
 * FUNCTIONS: fglSetAlphaParams
 * SYNOPSIS: This function discards a fragment depending on the outcome of a 
 *           comparison between the fragment's alpha value and a constant 
 *           reference value.
 * PARAMETERS:[IN] refAlpha: The reference value to which incoming alpha values
 *                 are compared. This value is clamped to the range 8bit value.
 *            [IN] mode: The alpha comparison function.
 * RETURNS: FGL_ERR_NO_ERROR - if successful.
 *          FGL_ERR_INVALID_VALUE - refAlpha was not an accepted value
 *          FGL_ERR_INVALID_PARAMETER - mode was not an accepted enum value
 * ERRNO:   FGL_ERR_NO_ERROR            1	    
 *          FGL_ERR_INVALID_PARAMETER   2
 *          FGL_ERR_INVALID_VALUE       7	
 *****************************************************************************/
FGL_Error 
fglSetAlphaParams ( unsigned int refAlpha, FGL_CompareFunc mode )
{
	FGL_BOOL bValidAlphaRef = FGL_FALSE;
	FGL_BOOL bValidCompareFunc = FGL_FALSE;
	unsigned int nAlphaVal =0;
	
	bValidAlphaRef = ALPHA_REF_VALUE_VALID(refAlpha);
	bValidCompareFunc = ALPHA_COMPARE_FUNC_VALID(mode);
		
	if (bValidAlphaRef && bValidCompareFunc)
	{
		READREGP(FGPF_ALPHA, nAlphaVal);
		FGL_SET_BITFIELD(nAlphaVal, 11:4, refAlpha);
		FGL_SET_BITFIELD(nAlphaVal, 3:1, mode);
		WRITEREG(FGPF_ALPHA, nAlphaVal);

		return FGL_ERR_NO_ERROR;
	}
	else
	{
		if(!bValidAlphaRef)
		{
			//printf((DBG_ERROR, "Cannot set alpha test control register - invalid reference value."));
			return FGL_ERR_INVALID_VALUE;
		}
		
		if(!bValidCompareFunc)
		{
			//printf((DBG_ERROR, "Cannot set alpha test control register - invalid parameters."));
			return FGL_ERR_INVALID_PARAMETER;
		}
	}
	return FGL_ERR_NO_ERROR;
}

/***************************************************************************** 
 * FUNCTIONS: fglSetStencilParams
 * SYNOPSIS: The stencil test conditionally discards a fragment based on the 
 *           outcome of a comparison between the value in the stencil buffer 
 *           and a reference value.
 * PARAMETERS: [IN] stencilParam: The point argument of FGL_StencilParam
 * RETURNS: FGL_ERR_NO_ERROR - if successful.
 *          FGL_ERR_INVALID_VALUE - The mask and ref of FGL_StencilParam was 
 *                                  not an accepted value .
 *          FGL_ERR_INVALID_PARAMETER - The zpass, zfail, sfail and mode of 
 *                                      FGL_StencilParam was not an accepted 
 *                                      enum value.
 * ERRNO:   FGL_ERR_NO_ERROR            1
 *          FGL_ERR_INVALID_PARAMETER   2
 *          FGL_ERR_INVALID_VALUE       7
 *****************************************************************************/
FGL_Error 
fglSetStencilParams ( pFGL_StencilParam stencilParam )
{
	FGL_BOOL bValidParam = FGL_FALSE;
	unsigned int nStencilVal = 0;
	unsigned int nFrontRegVal = 0;

    bValidParam = ( STENCIL_FACE_VALID(stencilParam->face) & STENCIL_OP_VALID(stencilParam->zpass) & 
                    STENCIL_OP_VALID(stencilParam->zfail) & STENCIL_OP_VALID(stencilParam->sfail) & 
                    STENCIL_MASK_VALID(stencilParam->mask) & STENCIL_REF_VALUE_VALID(stencilParam->ref) & 
                    STENCIL_COMPARE_FUNC_VALID(stencilParam->mode));

    if(bValidParam)
    {
		FGL_SET_BITFIELD(nStencilVal, 31:29, stencilParam->zpass);
		FGL_SET_BITFIELD(nStencilVal, 28:26, stencilParam->zfail);
		FGL_SET_BITFIELD(nStencilVal, 25:23, stencilParam->sfail);
		FGL_SET_BITFIELD(nStencilVal, 19:12, stencilParam->mask);
		FGL_SET_BITFIELD(nStencilVal, 11:4, stencilParam->ref);
		FGL_SET_BITFIELD(nStencilVal, 3:1, stencilParam->mode);

        switch((FGL_Face)(stencilParam->face))
        {
        case FGL_FACE_BACK:
		    WRITEREG(FGPF_BACKFACE_STENCIL, nStencilVal);
            break;
        case FGL_FACE_FRONT:
		    READREGP(FGPF_FRONTFACE_STENCIL, nFrontRegVal);
		    nStencilVal |= (nFrontRegVal & STENCIL_ENABLE_MASK);
		    WRITEREG(FGPF_FRONTFACE_STENCIL, nStencilVal);
            break;
        case FGL_FACE_FRONT_AND_BACK:
		    WRITEREG(FGPF_BACKFACE_STENCIL, nStencilVal);
		    READREGP(FGPF_FRONTFACE_STENCIL, nFrontRegVal);
		    nStencilVal |= (nFrontRegVal & STENCIL_ENABLE_MASK);
		    WRITEREG(FGPF_FRONTFACE_STENCIL, nStencilVal);
            break;
        default:
            return FGL_ERR_INVALID_PARAMETER;
        }
        
		return FGL_ERR_NO_ERROR;
	}
	else
	{
        return FGL_ERR_INVALID_PARAMETER;
	}
}

/***************************************************************************** 
 * FUNCTIONS: fglSetDepthParams
 * SYNOPSIS: This function specifies the value used for depth-buffer comparisons.
 * PARAMETERS: [IN] mode: Specifies the depth-comparison function
 * RETURNS: FGL_ERR_NO_ERROR - if successful.
 *          FGL_ERR_INVALID_PARAMETER - mode was not an accepted enum value.
 * ERRNO:   FGL_ERR_NO_ERROR            1
 *          FGL_ERR_INVALID_PARAMETER   2
 *****************************************************************************/
FGL_Error 
fglSetDepthParams ( FGL_CompareFunc mode )
{
	FGL_BOOL bValidCompFunc = FGL_FALSE;
	unsigned int nDepthVal =0;
	
	bValidCompFunc = DEPTH_COMPARE_FUNC_VALID(mode);
		
	if (bValidCompFunc)
	{
		READREGP(FGPF_DEPTH, nDepthVal);
		FGL_SET_BITFIELD(nDepthVal, 3:1, mode);
		WRITEREG(FGPF_DEPTH, nDepthVal);

		return FGL_ERR_NO_ERROR;
	}
	else
	{
		//printf((DBG_ERROR, "Cannot set depth test register - invalid parameters."));
		return FGL_ERR_INVALID_PARAMETER;
	}
}

/***************************************************************************** 
 * FUNCTIONS: fglSetBlendParams
 * SYNOPSIS: In RGB mode, pixels can be drawn using a function that blends 
 *           the incoming (source) RGBA values with the RGBA values that are 
 *           already in the framebuffer (the destination values). 
 * PARAMETERS: [in] blendParam - the pointer parameter of FGL_BlendParam
 * RETURNS: FGL_ERR_NO_ERROR - if successful
 *          FGL_ERR_INVALID_PARAMETER - the member of struct FGL_BlendParam  
 *                                      was not an accepted enum value.
 * ERRNO:   FGL_ERR_NO_ERROR            1
 *          FGL_ERR_INVALID_PARAMETER   2
 *****************************************************************************/
FGL_Error 
fglSetBlendParams ( pFGL_BlendParam blendParam )
{
    FGL_BOOL bValidParam = FGL_FALSE;
	unsigned int nBlendVal =0;
	
	bValidParam = ( BLEND_EQUATION_VALID(blendParam->colorEqua) &
	                BLEND_EQUATION_VALID(blendParam->alphaEqua) &
	                BLEND_DEST_ALPHA_FUNC_VALID(blendParam->dstAlpha) &
	                BLEND_DEST_FUNC_VALID(blendParam->dstColor) &
	                BLEND_SOURCE_ALPHA_FUNC_VALID(blendParam->srcAlpha) &
	                BLEND_SOURCE_FUNC_VALID(blendParam->srcColor));

	if(bValidParam)
	{
		READREGP(FGPF_BLEND, nBlendVal);
		FGL_SET_BITFIELD(nBlendVal, 22:20, blendParam->alphaEqua);
		FGL_SET_BITFIELD(nBlendVal, 19:17, blendParam->colorEqua);
		FGL_SET_BITFIELD(nBlendVal, 16:13, blendParam->dstAlpha);
		FGL_SET_BITFIELD(nBlendVal, 12:9, blendParam->dstColor);
		FGL_SET_BITFIELD(nBlendVal, 8:5, blendParam->srcAlpha);
		FGL_SET_BITFIELD(nBlendVal, 4:1, blendParam->srcColor);

		WRITEREG(FGPF_BLEND_COLOR, blendParam->constColor);
		WRITEREG(FGPF_BLEND, nBlendVal);

		return FGL_ERR_NO_ERROR;
	}
	else
	{
	    return FGL_ERR_INVALID_PARAMETER;
	}
}


/* thomas-20050921@comment: opengl|es 2.0 not supported */
/***************************************************************************** 
 * FUNCTIONS: fglSetLogicalOpParams
 * SYNOPSIS: A logical operation can be applied the fragment and value stored 
 *           at the corresponding location in the framebuffer; the result 
 *           replaces the current framebuffer value. 
 * PARAMETERS: [in] colorOp - specifies an enumeration value that selects 
 *                            a logical operation.
 * RETURNS: FGL_ERR_NO_ERROR - if successful
 *          FGL_ERR_INVALID_PARAMETER - The colorOp was not an accepted value.
 * ERRNO:   FGL_ERR_NO_ERROR            1
 *          FGL_ERR_INVALID_PARAMETER   2
 *****************************************************************************/
FGL_Error 
fglSetLogicalOpParams ( FGL_LogicalOp colorOp )
{
	unsigned int nLogicOpVal = 0;
	
	if (LOGIC_OP_FUNC_VALID(colorOp))
	{
		READREGP(FGPF_LOGIC_OP, nLogicOpVal);
		FGL_SET_BITFIELD(nLogicOpVal, 8:5, colorOp);
		FGL_SET_BITFIELD(nLogicOpVal, 4:1, colorOp);
		WRITEREG(FGPF_LOGIC_OP, nLogicOpVal);
		return FGL_ERR_NO_ERROR;
	}
	else
	{
		//printf((DBG_ERROR, "Cannot set logical operation color function - invalid colorOp."));
		return FGL_ERR_INVALID_PARAMETER;
	}
}

/***************************************************************************** 
 * FUNCTIONS: fglSetColorBufWriteMask
 * SYNOPSIS: enable and disable writing of frame(color) buffer color components
 * PARAMETERS: [in] r - whether red can or cannot be written into the frame buffer.
 *             [in] g - whether green can or cannot be written into the frame buffer.
 *             [in] b - whether blue can or cannot be written into the frame buffer.
 *             [in] a - whether alpha can or cannot be written into the frame buffer.
 * RETURNS: FGL_ERR_NO_ERROR, if successful
 * ERRNO:   FGL_ERR_NO_ERROR        1
 *****************************************************************************/
FGL_Error 
fglSetColorBufWriteMask ( 
                            FGL_BOOL    r, 
                            FGL_BOOL    g, 
                            FGL_BOOL    b, 
                            FGL_BOOL    a 
                        )
{
	unsigned int nWriteMaskVal = 0;	

	if(r) nWriteMaskVal |= SET_BIT(3);		
	if(g) nWriteMaskVal |= SET_BIT(2);
	if(b) nWriteMaskVal |= SET_BIT(1);	
	if(a) nWriteMaskVal |= SET_BIT(0);
	
	WRITEREG(FGPF_COLOR_MASK, nWriteMaskVal);
	    		
    return FGL_ERR_NO_ERROR;
}


/***************************************************************************** 
 * FUNCTIONS: fglSetStencilBufWriteMask
 * SYNOPSIS: control the front and/or back writing of individual bits 
 *           in the stencil buffer.
 * PARAMETERS: [in] face - specifies whether the front and/or back stencil 
 *                         writemask is updated.
 *             [in] mask - A bit mask to enable and disable writing of individual
 *                         bits in the stencil buffer.
 * RETURNS: FGL_ERR_NO_ERROR - if successful
 *          FGL_ERR_INVALID_PARAMETER - The face and mask were not an accepted value.
 * ERRNO:   FGL_ERR_NO_ERROR            1
 *          FGL_ERR_INVALID_PARAMETER   2
 *****************************************************************************/
#if TARGET_FIMG_VERSION == _FIMG3DSE_VER_1_2_1
FGL_Error 
fglSetStencilBufWriteMask ( FGL_Face face, unsigned int mask )
{
	FGL_BOOL bValidParam = (STENCIL_FACE_VALID(face) & STENCIL_MASK_VALID(mask));
	
    //unsigned int nStencilMaskVal = READREG(FGPF_STENCIL_DEPTH_MASK);
    unsigned int nStencilMaskVal;
    READREGP(FGPF_STENCIL_DEPTH_MASK, nStencilMaskVal);
    	
	if(bValidParam)
	{
        switch((FGL_Face)face)
        {
        case FGL_FACE_BACK:
            FGL_SET_BITFIELD(nStencilMaskVal, 31:24, mask);
		    WRITEREG(FGPF_STENCIL_DEPTH_MASK, nStencilMaskVal);
            break;
        case FGL_FACE_FRONT:
            FGL_SET_BITFIELD(nStencilMaskVal, 23:16, mask);
		    WRITEREG(FGPF_STENCIL_DEPTH_MASK, nStencilMaskVal);
            break;
        case FGL_FACE_FRONT_AND_BACK:
            FGL_SET_BITFIELD(nStencilMaskVal, 23:16, mask);
            FGL_SET_BITFIELD(nStencilMaskVal, 31:24, mask);
		    WRITEREG(FGPF_STENCIL_DEPTH_MASK, nStencilMaskVal);
            break;
        default:
            return FGL_ERR_INVALID_PARAMETER;
        }
        
	    return FGL_ERR_NO_ERROR;	
	}
	else
	{
	    return FGL_ERR_INVALID_PARAMETER;
	}
}
#endif
/***************************************************************************** 
 * FUNCTIONS: fglSetZBufWriteMask
 * SYNOPSIS: enables or disables writing into the depth buffer.
 * PARAMETERS: [in] enable - specifies whether the depth buffer is enabled 
 *                           for writing.
 * RETURNS: FGL_ERR_NO_ERROR - if successful
 * ERRNO:   FGL_ERR_NO_ERROR            1
 *****************************************************************************/
FGL_Error 
fglSetZBufWriteMask ( FGL_BOOL enable )
{
	//unsigned int nDepthMaskVal = READREG(FGPF_STENCIL_DEPTH_MASK);
	unsigned int nDepthMaskVal;
	READREGP(FGPF_STENCIL_DEPTH_MASK, nDepthMaskVal);
   	FGL_SET_BITFIELD(nDepthMaskVal, 0:0, enable);
   	WRITEREG(FGPF_STENCIL_DEPTH_MASK, nDepthMaskVal);
	return FGL_ERR_NO_ERROR;	
}

/***************************************************************************** 
 * FUNCTIONS: fglSetFrameBufParams
 * SYNOPSIS: specifies the value used for frame buffer control.
 * PARAMETERS: [in] fbctrlParam - the pointer parameter of FGL_FBCtrlParam.
 *             opaqueAlpha - after alpha blending, the alpha value is 
 *                           forced to opaque.
 *             thresholdAlpha - specifies an alpha value in the frame buffer 
 *                              ARGB1555 format. 
 *             constAlpha - specifies constant alpha value in the frame 
 *                          buffer ARGB0888 format.
 *             dither - specifies whether the dither is enable or disable.
 *             format - specifies the format used for the frame buffer.
 * RETURNS: FGL_ERR_NO_ERROR - if successful
 *          FGL_ERR_INVALID_PARAMETER - The parameters were not an accepted value.
 * ERRNO:   FGL_ERR_NO_ERROR                1
 *          FGL_ERR_INVALID_PARAMETER       2
 *****************************************************************************/
FGL_Error 
fglSetFrameBufParams ( pFGL_FBCtrlParam fbctrlParam )
{

	FGL_BOOL bValidParam = FGL_FALSE;
	unsigned int nBufCtrlVal =0 ;

    bValidParam = ( ALPHA_THRESHOLD_VALUE_VALID(fbctrlParam->thresholdAlpha) &
                    ALPHA_CONSTANT_VALUE_VALID(fbctrlParam->constAlpha)      &
                    PIXEL_FORMAT_VALID(fbctrlParam->format));

	if(bValidParam)
	{
		FGL_SET_BITFIELD(nBufCtrlVal, 20:20, fbctrlParam->opaqueAlpha);
		
		if((FGL_PixelFormat)fbctrlParam->format == FGL_PIXEL_ARGB1555)
		    FGL_SET_BITFIELD(nBufCtrlVal, 19:12, fbctrlParam->thresholdAlpha);
		
		if((FGL_PixelFormat)fbctrlParam->format == FGL_PIXEL_ARGB0888)
		    FGL_SET_BITFIELD(nBufCtrlVal, 11:4, fbctrlParam->constAlpha);
		
		FGL_SET_BITFIELD(nBufCtrlVal, 3:3, fbctrlParam->dither);
		FGL_SET_BITFIELD(nBufCtrlVal, 2:0, fbctrlParam->format);
		WRITEREG(FGPF_COLORBUF_CTRL, nBufCtrlVal);
		return FGL_ERR_NO_ERROR;
	}
	else
	{
	    return FGL_ERR_INVALID_PARAMETER;
	}
}

/***************************************************************************** 
 * FUNCTIONS: fglSetZBufBaseAddr
 * SYNOPSIS: Depth and Stencil buffer base address
 * PARAMETERS: [in] addr - specifies the value used for stencil/depth buffer 
 *                         address.
 * RETURNS: FGL_ERR_NO_ERROR, if successful
 * ERRNO:   FGL_ERR_NO_ERROR    1
 *****************************************************************************/
FGL_Error 
fglSetZBufBaseAddr ( unsigned int addr )
{
	WRITEREG(FGPF_DEPTHBUF_ADDR, addr);
	return FGL_ERR_NO_ERROR;	
}

/***************************************************************************** 
 * FUNCTIONS: fglSetColorBufBaseAddr
 * SYNOPSIS: color buffer base address
 * PARAMETERS: [in] addr - specifies the value used for frame buffer address.
 * RETURNS: FGL_ERR_NO_ERROR, if successful
 * ERRNO:   FGL_ERR_NO_ERROR    1
 *****************************************************************************/
FGL_Error 
fglSetColorBufBaseAddr ( unsigned int addr )
{
	WRITEREG(FGPF_COLORBUF_ADDR, addr);
	return FGL_ERR_NO_ERROR;	
}

/***************************************************************************** 
 * FUNCTIONS: fglSetFrameBufWidth
 * SYNOPSIS: frame buffer width
 * PARAMETERS: [in] width - specifies the value used for frame buffer width.
 * RETURNS: FGL_ERR_NO_ERROR - if successful
 *          FGL_ERR_INVALID_VALUE - 
 * ERRNO:   FGL_ERR_NO_ERROR        1
 *          FGL_ERR_INVALID_VALUE   7
 *****************************************************************************/
FGL_Error 
fglSetFrameBufWidth ( unsigned int width )
{
	if (width > FGL_ZERO && width <= FGPF_FB_MAX_STRIDE)
	{
		WRITEREG(FGPF_COLORBUF_WIDTH, width);
		return FGL_ERR_NO_ERROR;
	}
	else
	{
		//printf((DBG_ERROR, "Cannot set frame buffer width - invalid width value."));
		return FGL_ERR_INVALID_VALUE;
	}
}

/*-----------------------< End of file >----------------------------- */
