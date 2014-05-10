/******************************************************************************
*
* NAME         : fimg_tu.c
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
*	$Revision: 1.7 $
*	$Author: inhosens.lee $
*	$Date: 2006/04/14 00:18:34 $
*	$Locker:  $
*
*	$Source: C:/CVS/CVSrepository/FIMG-3DSE_SW/fimg3dse_fpga/fimg3d/device/fimg_tu.c,v $
*	$State: Exp $
*	$Log: fimg_tu.c,v $
*	Revision 1.7  2006/04/14 00:18:34  inhosens.lee
*	modified some mistypings
*
*	Revision 1.6  2006/04/13 10:56:31  cheolkyoo.kim
*	Replace FG_TRUE with FGL_TRUE.
*
*	Revision 1.5  2006/04/11 06:44:36  inhosens.lee
*	function descriptions are added
*
*	Revision 1.4  2006/04/06 06:57:09  inhosens.lee
*	Descriptions of register-level API of texture unit
*
*
*****************************************************************************/
#include "config.h"
#include "register.h"
#include "macros.h"
#include "fgl.h"
//#include "string.h"
/*****************************************************************************
  DEFINES
 *****************************************************************************/

#define FGL_TEXTURE_TYPE_VALIDATOR  (								\
										(1 << FGL_TEX_2D)	    |	\
										(1 << FGL_TEX_CUBE)	    |	\
										(1 << FGL_TEX_3D)	    	\
									)

#define FGL_COLOR_KEY_VALIDATOR	(									\
									(1 << FGL_CKEY_DISABLE)	    |	\
									(1 << FGL_CKEY_SEL1)	    |	\
									(1 << FGL_CKEY_DISABLE2)    |	\
									(1 << FGL_CKEY_SEL2)		    \
								)

#define FGL_PALETTE_FORMAT_VALIDATOR (									\
										(1 << FGL_PALETTE_ARGB1555)	|	\
										(1 << FGL_PALETTE_RGB565  )	|	\
										(1 << FGL_PALETTE_ARGB4444)	|	\
										(1 << FGL_PALETTE_ARGB8888)		\
									 )

#define FGL_TEXTURE_FORMAT_VALIDATOR (									\
										(1 << FGL_TEXEL_ARGB1555)	|	\
										(1 << FGL_TEXEL_RGB565  )	|	\
										(1 << FGL_TEXEL_ARGB4444)	|	\
										(1 << FGL_TEXEL_DEPTH24 )	|	\
										(1 << FGL_TEXEL_IA88    )	|	\
										(1 << FGL_TEXEL_I8      )	|	\
										(1 << FGL_TEXEL_ARGB8888)	|	\
										(1 << FGL_TEXEL_1BPP    )	|	\
										(1 << FGL_TEXEL_2BPP    )	|	\
										(1 << FGL_TEXEL_4BPP    )	|	\
										(1 << FGL_TEXEL_8BPP    )	|	\
										(1 << FGL_TEXEL_S3TC    )	|	\
										(1 << FGL_TEXEL_Y1VY0U  )	|	\
										(1 << FGL_TEXEL_VY1UY0  )	|	\
										(1 << FGL_TEXEL_Y1UY0V  )	|	\
										(1 << FGL_TEXEL_UY1VY0  )		\
									)

#define FGL_WRAP_MODE_VALIDATOR	(									\
									(1 << FGL_TEX_WRAP_REPEAT )	|	\
									(1 << FGL_TEX_WRAP_FLIP   )	|	\
									(1 << FGL_TEX_WRAP_CLAMP  )		\
								)

#define FGL_MIPMAP_FILTER_VALIDATOR	(								    \
									    (1 << FGL_FILTER_DISABLE)	|	\
									    (1 << FGL_FILTER_NEAREST)	|	\
										(1 << FGL_FILTER_LINEAR)	   	\
									)


#define TEXTURE_TYPE_VALID(type)                            \
		FGL_ENUM_IN_SET(type, FGL_TEXTURE_TYPE_VALIDATOR)

#define COLOR_KEY_VALID(key)								\
		FGL_ENUM_IN_SET(key, FGL_COLOR_KEY_VALIDATOR)

#define PALETTE_FORMAT_VALID(format)						\
		FGL_ENUM_IN_SET(format, FGL_PALETTE_FORMAT_VALIDATOR)

#define TEXTURE_FORMAT_VALID(format)						\
		FGL_ENUM_IN_SET(format, FGL_TEXTURE_FORMAT_VALIDATOR)

#define WRAP_MODE_VALID(mode)								\
		FGL_ENUM_IN_SET(mode, FGL_WRAP_MODE_VALIDATOR)

#define MIPMAP_FILTER_VALID(filter)                         \
		FGL_ENUM_IN_SET(filter, FGL_MIPMAP_FILTER_VALIDATOR)

/*****************************************************************************
 * FUNCTIONS: fglSetTexUnitParams
 *
 * 			refer to
 * 			drvsys_write_reg(FGTU_TSTA#, unsigned int)
 *
 * SYNOPSIS: This function sets various texture parameters
 * 			 for each texture unit.
 * 			 	(type, texture half decimation factor control,
 * 			 	texture decimation factor, color key selection,
 * 			 	texture value expansion method, bilinear filter register,
 * 			 	MIPMAP control register, texture format,
 * 			 	u address mode, v address mode, u size, v size)
 * PARAMETERS: [IN]	unsigned int unit: texture unit ( 0~7 )
 *			   [IN]	FGL_TexUnitParams params: texture parameter structure
 * 			 		 	typedef struct FGL_TexUnitParamsTag
 * 			 		 	{
 * 			 		 		FGL_TexType 		type;
 * 			 		 		FG_BOOL	 			bOnHalfDecimation;
 * 			 		 		FGL_TexDeciFactor 	deciFactor;
 * 			 		 		FGL_CKeySel 		ckey;
 * 			 		 		FG_BOOL 			bTexExp;
 * 			 		 		FG_BOOL				bUseMapFilter;
 * 			 		 		FGL_TexFilter 		mipFilter;
 * 			 		 		FGL_TexelFormat 	format;
 * 			 		 		FGL_TexWrapMode		uMode;
 * 			 		 		FGL_TexWrapMode		vMode;
 * 			 		 		FGL_TexSize 		uSize;
 * 			 		 		FGL_TexSize 		vSize;
 * 			 		 	} FGL_TexUnitParams;
 * RETURNS: FGL_ERR_NO_ERROR - if successful.
 *          FGL_ERR_INVALID_SIZE - if texture u or v size is invalid
 *          FGL_ERR_INVALID_VALUE - if specified parameters are invalid
 * ERRNO: FGL_ERR_INVALID_SIZE = 6
 * 		  FGL_ERR_INVALID_VALUE = 7
 *****************************************************************************/
FGL_Error
fglSetTexUnitParams(unsigned int unit, FGL_TexUnitParams *params)
{
	//FGL_Error ret = FGL_ERR_UNKNOWN;
	FGL_Error ret = FGL_ERR_NO_ERROR;
	unsigned int uUSize = params->uUSize;
	unsigned int uVSize = params->uVSize;
	unsigned int uPSize = params->uPSize;
	unsigned int uMipMapLevel = 0;
	unsigned int uIndex = 0;
	unsigned int uTexMipMapSize = 0;
	unsigned int *pTexRegBaseAddr;// = FIMG_NULL;
	unsigned int TexRegs[17] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	unsigned int *pTexRegVal = (unsigned int *)TexRegs;

	unsigned int uCheckSize, i;

	switch(unit)
	{
	case 0:
		pTexRegBaseAddr = (unsigned int *)FGTU_TEX0_CTRL;
		break;
	case 1:
		pTexRegBaseAddr = (unsigned int *)FGTU_TEX1_CTRL;
		break;
	case 2:
		pTexRegBaseAddr = (unsigned int *)FGTU_TEX2_CTRL;
		break;
	case 3:
		pTexRegBaseAddr = (unsigned int *)FGTU_TEX3_CTRL;
		break;
	case 4:
		pTexRegBaseAddr = (unsigned int *)FGTU_TEX4_CTRL;
		break;
	case 5:
		pTexRegBaseAddr = (unsigned int *)FGTU_TEX5_CTRL;
		break;
	case 6:
		pTexRegBaseAddr = (unsigned int *)FGTU_TEX6_CTRL;
		break;
	case 7:
		pTexRegBaseAddr = (unsigned int *)FGTU_TEX7_CTRL;
		break;
	default:
		return FGL_ERR_INVALID_VALUE;
	}

    if (TEXTURE_TYPE_VALID(params->eType)
			&& COLOR_KEY_VALID(params->eColorkey)
			&& PALETTE_FORMAT_VALID(params->ePaletteFormat)
			&& TEXTURE_FORMAT_VALID(params->eFormat)
			&& WRAP_MODE_VALID(params->eUMode)
			&& WRAP_MODE_VALID(params->eVMode) 
//			&& WRAP_MODE_VALID(params->ePMode)
			&& MIPMAP_FILTER_VALID(params->eMipMapFilter)) {
        TexRegs[uIndex] = ((params->eType) << TEXCTRL_TEXTURE_TYPE_SHIFT)
			| ((params->eColorkey) << TEXCTRL_COLOR_KEY_SHIFT)
			| ((params->ePaletteFormat) << TEXCTRL_PALETTE_FORMAT_SHIFT)
			| ((params->eFormat) << TEXCTRL_TEXTURE_FORMAT_SHIFT)
			| ((params->eUMode) << TEXCTRL_U_WRAPMODE_SHIFT)
			| ((params->eVMode) << TEXCTRL_V_WRAPMODE_SHIFT);
//			| ((params->ePMode) << TEXCTRL_P_WRAPMODE_SHIFT);
        
        if(params->bUseExpansion)
            TexRegs[uIndex] |= (1 << TEXCTRL_EXPANSION_SHIFT);
        
        if(params->bIsNonparametric)
            TexRegs[uIndex] |= (1 << TEXCTRL_NONPARAMETRIC_SHIFT);
        
        if(params->bUseMagFilter)
            TexRegs[uIndex] |= (1 << TEXCTRL_MAG_FILTER_SHIFT);
        
        if(params->bUseMinFilter)
            TexRegs[uIndex] |= (1 << TEXCTRL_MIN_FILTER_SHIFT);
        
		TexRegs[uIndex] |= ((params->eMipMapFilter) << TEXCTRL_MIPMAP_FILTER_SHIFT);
        uIndex++;
    }
    else
    {
        uIndex++;
        ret = FGL_ERR_INVALID_VALUE;
    }

    if(uUSize >= FGL_MIN_TEXTURE_WIDTH && uUSize <= FGL_MAX_TEXTURE_WIDTH)
        TexRegs[uIndex++] = uUSize;
    else
    {
        uIndex++;
        ret = FGL_ERR_INVALID_SIZE;
    }

    if(uVSize >= FGL_MIN_TEXTURE_HEIGHT && uVSize <= FGL_MAX_TEXTURE_HEIGHT)
        TexRegs[uIndex++] = uVSize;
    else
    {
        uIndex++;
        ret = FGL_ERR_INVALID_SIZE;
    }

    if(uPSize >= FGL_MIN_TEXTURE_DEPTH && uPSize <= FGL_MAX_TEXTURE_DEPTH)
        TexRegs[uIndex++] = uPSize;
    else
    {
        uIndex++;
        ret = FGL_ERR_INVALID_SIZE;
    }

	if(params->eMipMapFilter)
	{

		uTexMipMapSize = (unsigned int)(uUSize * uVSize);
		TexRegs[uIndex++] = uTexMipMapSize;

#if 0
		while(FIMG_MAX(uUSize, uVSize) > 1 )
		{
			++uMipMapLevel;
			uTexMipMapSize += (unsigned int)((uUSize >>= 1)*(uVSize >>= 1));
			TexRegs[uIndex++] = uTexMipMapSize;
		}
#else
		uCheckSize = uUSize > uVSize ? uUSize : uVSize;
		uCheckSize >>= 1;
		for(i = 0; uCheckSize > 0 && i < 11; i++) 
		{
    		++uMipMapLevel;
			uUSize = (uUSize <= 1) ? 1 : (uUSize >> 1);
			uVSize = (uVSize <= 1) ? 1 : (uVSize >> 1);
			uCheckSize >>= 1;
			uTexMipMapSize += uUSize * uVSize;
    		TexRegs[uIndex++] = uTexMipMapSize;
    	}
#endif
    	TexRegs[16] = uMipMapLevel;
	}

#if 1
	__asm
	{
    	LDMIA pTexRegVal!,{v1-v8}
    	STMIA pTexRegBaseAddr!,{v1-v8}
    	LDMIA pTexRegVal!,{v1-v8}
    	STMIA pTexRegBaseAddr!,{v1-v8}
	}
    WRITEREG(pTexRegBaseAddr, *pTexRegVal);
#else
		WRITEREG(pTexRegBaseAddr++, *(pTexRegVal++));
		WRITEREG(pTexRegBaseAddr++, *(pTexRegVal++));
		WRITEREG(pTexRegBaseAddr++, *(pTexRegVal++));
		WRITEREG(pTexRegBaseAddr++, *(pTexRegVal++));
		WRITEREG(pTexRegBaseAddr++, *(pTexRegVal++));
		WRITEREG(pTexRegBaseAddr++, *(pTexRegVal++));
		WRITEREG(pTexRegBaseAddr++, *(pTexRegVal++));
		WRITEREG(pTexRegBaseAddr++, *(pTexRegVal++));
		WRITEREG(pTexRegBaseAddr++, *(pTexRegVal++));
		WRITEREG(pTexRegBaseAddr++, *(pTexRegVal++));
		WRITEREG(pTexRegBaseAddr++, *(pTexRegVal++));
		WRITEREG(pTexRegBaseAddr++, *(pTexRegVal++));
		WRITEREG(pTexRegBaseAddr++, *(pTexRegVal++));
		WRITEREG(pTexRegBaseAddr++, *(pTexRegVal++));
		WRITEREG(pTexRegBaseAddr++, *(pTexRegVal++));
		WRITEREG(pTexRegBaseAddr++, *(pTexRegVal++));
		WRITEREG(pTexRegBaseAddr++, *(pTexRegVal++));
#endif

	return ret;
}

/*****************************************************************************
 * FUNCTIONS: fglSetTexStatusParams
 *
 * 			refer to
 * 			drvsys_write_reg(FGTU_TSTA#, unsigned int)
 *
 * SYNOPSIS: This function sets various texture parameters
 * 			 for each texture unit.
 * 			 	(type, color key selection, texture msb expansion method,
 * 			 	palette format, texture format, 
 * 			 	u address mode, v address mode,
 * 			 	parametric or nonparametric addressing,
 * 			 	and mag., min. and mipmap filter)
 * PARAMETERS: [IN]	unsigned int unit: texture unit ( 0~7 )
 *			   [IN]	FGL_TexStatusParams params: texture parameter structure
 * 			 		 	typedef struct FGL_TexStatusParamsTag
 * 			 		 	{
 * 			 		 		FGL_TexType 		eType;
 * 			 		 		FGL_CKeySel 		eColorKey;
 * 			 		 		FG_BOOL	 			bUseExpansion;
 * 			 		 		FGL_PaletteFormat 	ePaletteFormat;
 * 			 		 		FGL_TexelFormat 	eFormat;
 * 			 		 		FGL_TexWrapMode		eUMode;
 * 			 		 		FGL_TexWrapMode		eVMode;
 * 			 		 		FG_BOOL 			bIsNonparametric;
 * 			 		 		FG_BOOL				bUseMagFilter;
 * 			 		 		FG_BOOL				bUseMinFilter;
 * 			 		 		FGL_TexFilter 		eMipMapFilter;
 * 			 		 	} FGL_TexUnitParams;
 * RETURNS: FGL_ERR_NO_ERROR - if successful.
 *          FGL_ERR_INVALID_VALUE - if specified parameters are invalid
 * ERRNO: FGL_ERR_INVALID_VALUE = 7
 *****************************************************************************/

FGL_Error
fglSetTexStatusParams(unsigned int unit, FGL_TexStatusParams *params)
{
	unsigned int texStatusR;
	unsigned int *pTexRegBaseAddr;

	switch(unit)
	{
	case 0:
		pTexRegBaseAddr = (unsigned int *)FGTU_TEX0_CTRL;
		break;
	case 1:
		pTexRegBaseAddr = (unsigned int *)FGTU_TEX1_CTRL;
		break;
	case 2:
		pTexRegBaseAddr = (unsigned int *)FGTU_TEX2_CTRL;
		break;
	case 3:
		pTexRegBaseAddr = (unsigned int *)FGTU_TEX3_CTRL;
		break;
	case 4:
		pTexRegBaseAddr = (unsigned int *)FGTU_TEX4_CTRL;
		break;
	case 5:
		pTexRegBaseAddr = (unsigned int *)FGTU_TEX5_CTRL;
		break;
	case 6:
		pTexRegBaseAddr = (unsigned int *)FGTU_TEX6_CTRL;
		break;
	case 7:
		pTexRegBaseAddr = (unsigned int *)FGTU_TEX7_CTRL;
		break;
	default:
		return FGL_ERR_INVALID_VALUE;
	}

	if (!TEXTURE_TYPE_VALID(params->eType)
						|| !COLOR_KEY_VALID(params->eColorKey)
//						|| params->bUseExpansion > 1
						|| !PALETTE_FORMAT_VALID(params->ePaletteFormat)
						|| !TEXTURE_FORMAT_VALID(params->eFormat)
						|| !WRAP_MODE_VALID(params->eUMode)
						|| !WRAP_MODE_VALID(params->eVMode)
//						|| !WRAP_MODE_VALID(params->ePMode)
//						|| params->bIsNonparametric > 1
//						|| params->bUseMagFilter > 1
//						|| params->bUseMinFilter > 1
						|| !MIPMAP_FILTER_VALID(params->eMipMapFilter))
	{
		return FGL_ERR_INVALID_VALUE;
	}

	texStatusR = ((params->eType) << TEXCTRL_TEXTURE_TYPE_SHIFT)
				| ((params->eColorKey) << TEXCTRL_COLOR_KEY_SHIFT)
				| ((params->bUseExpansion) << TEXCTRL_EXPANSION_SHIFT)
				| ((params->ePaletteFormat) << TEXCTRL_PALETTE_FORMAT_SHIFT)
				| ((params->eFormat) << TEXCTRL_TEXTURE_FORMAT_SHIFT)
				| ((params->eUMode) << TEXCTRL_U_WRAPMODE_SHIFT)
				| ((params->eVMode) << TEXCTRL_V_WRAPMODE_SHIFT)
//				| ((params->ePMode) << TEXCTRL_P_WRAPMODE_SHIFT)
				| ((params->bIsNonparametric) << TEXCTRL_NONPARAMETRIC_SHIFT)
				| ((params->bUseMagFilter) << TEXCTRL_MAG_FILTER_SHIFT)
				| ((params->bUseMinFilter) << TEXCTRL_MIN_FILTER_SHIFT)
				| (params->eMipMapFilter);

	WRITEREG(pTexRegBaseAddr, texStatusR);
		
	return FGL_ERR_NO_ERROR;
}

/*****************************************************************************
 * FUNCTIONS: fglSetTexUSize
 *
 * 			refer to
 * 			drvsys_write_reg(FGTU_USIZE#, unsigned int)
 *
 * SYNOPSIS: This function sets a texture u size
 * 			 for each texture unit.
 * PARAMETERS: [IN]	unsigned int unit: texture unit ( 0~7 )
 *			   [IN]	unsigned int uSize: texture u size
 * RETURNS: FGL_ERR_NO_ERROR - if successful.
 *          FGL_ERR_INVALID_SIZE - if texture size is invalid
 *          FGL_ERR_INVALID_VALUE - if specified unit is invalid
 * ERRNO: FGL_ERR_INVALID_SIZE = 6
 *        FGL_ERR_INVALID_VALUE = 7
 *****************************************************************************/
FGL_Error
fglSetTexUSize(unsigned int unit, unsigned int uSize)
{
	unsigned int *pTexRegBaseAddr;

	if (uSize > MAXVALUE_TU_TEX_SIZE)
	{
		return FGL_ERR_INVALID_SIZE;
	}

	switch(unit)
	{
	case 0:
		pTexRegBaseAddr = (unsigned int *)FGTU_TEX0_USIZE;
		break;
	case 1:
		pTexRegBaseAddr = (unsigned int *)FGTU_TEX1_USIZE;
		break;
	case 2:
		pTexRegBaseAddr = (unsigned int *)FGTU_TEX2_USIZE;
		break;
	case 3:
		pTexRegBaseAddr = (unsigned int *)FGTU_TEX3_USIZE;
		break;
	case 4:
		pTexRegBaseAddr = (unsigned int *)FGTU_TEX4_USIZE;
		break;
	case 5:
		pTexRegBaseAddr = (unsigned int *)FGTU_TEX5_USIZE;
		break;
	case 6:
		pTexRegBaseAddr = (unsigned int *)FGTU_TEX6_USIZE;
		break;
	case 7:
		pTexRegBaseAddr = (unsigned int *)FGTU_TEX7_USIZE;
		break;
	default:
		return FGL_ERR_INVALID_VALUE;
	}

	WRITEREG(pTexRegBaseAddr, uSize);
		
	return FGL_ERR_NO_ERROR;
}

/*****************************************************************************
 * FUNCTIONS: fglSetTexVSize
 *
 * 			refer to
 * 			drvsys_write_reg(FGTU_VSIZE#, unsigned int)
 *
 * SYNOPSIS: This function sets a texture v size
 * 			 for each texture unit.
 * PARAMETERS: [IN]	unsigned int unit: texture unit ( 0~7 )
 *			   [IN]	unsigned int vSize: texture v size
 * RETURNS: FGL_ERR_NO_ERROR - if successful.
 *          FGL_ERR_INVALID_SIZE - if texture size is invalid
 *          FGL_ERR_INVALID_VALUE - if specified unit is invalid
 * ERRNO: FGL_ERR_INVALID_SIZE = 6
 *        FGL_ERR_INVALID_VALUE = 7
 *****************************************************************************/
FGL_Error
fglSetTexVSize(unsigned int unit, unsigned int vSize)
{
	unsigned int *pTexRegBaseAddr;

	if (vSize > MAXVALUE_TU_TEX_SIZE)
	{
		return FGL_ERR_INVALID_SIZE;
	}

	switch(unit)
	{
	case 0:
		pTexRegBaseAddr = (unsigned int *)FGTU_TEX0_VSIZE;
		break;
	case 1:
		pTexRegBaseAddr = (unsigned int *)FGTU_TEX1_VSIZE;
		break;
	case 2:
		pTexRegBaseAddr = (unsigned int *)FGTU_TEX2_VSIZE;
		break;
	case 3:
		pTexRegBaseAddr = (unsigned int *)FGTU_TEX3_VSIZE;
		break;
	case 4:
		pTexRegBaseAddr = (unsigned int *)FGTU_TEX4_VSIZE;
		break;
	case 5:
		pTexRegBaseAddr = (unsigned int *)FGTU_TEX5_VSIZE;
		break;
	case 6:
		pTexRegBaseAddr = (unsigned int *)FGTU_TEX6_VSIZE;
		break;
	case 7:
		pTexRegBaseAddr = (unsigned int *)FGTU_TEX7_VSIZE;
		break;
	default:
		return FGL_ERR_INVALID_VALUE;
	}

	WRITEREG(pTexRegBaseAddr, vSize);
		
	return FGL_ERR_NO_ERROR;
}

/*****************************************************************************
 * FUNCTIONS: fglSetTexPSize
 *
 * 			refer to
 * 			drvsys_write_reg(FGTU_PSIZE#, unsigned int)
 *
 * SYNOPSIS: This function sets a texture p size
 * 			 for each texture unit.
 * PARAMETERS: [IN]	unsigned int unit: texture unit ( 0~7 )
 *			   [IN]	unsigned int pSize: texture p size
 * RETURNS: FGL_ERR_NO_ERROR - if successful.
 *          FGL_ERR_INVALID_SIZE - if texture size is invalid
 *          FGL_ERR_INVALID_VALUE - if specified unit is invalid
 * ERRNO: FGL_ERR_INVALID_SIZE = 6
 *        FGL_ERR_INVALID_VALUE = 7
 *****************************************************************************/
FGL_Error
fglSetTexPSize(unsigned int unit, unsigned int pSize)
{
	unsigned int *pTexRegBaseAddr;

	if (pSize > MAXVALUE_TU_TEX_SIZE)
	{
		return FGL_ERR_INVALID_SIZE;
	}

	switch(unit)
	{
	case 0:
		pTexRegBaseAddr = (unsigned int *)FGTU_TEX0_PSIZE;
		break;
	case 1:
		pTexRegBaseAddr = (unsigned int *)FGTU_TEX1_PSIZE;
		break;
	case 2:
		pTexRegBaseAddr = (unsigned int *)FGTU_TEX2_PSIZE;
		break;
	case 3:
		pTexRegBaseAddr = (unsigned int *)FGTU_TEX3_PSIZE;
		break;
	case 4:
		pTexRegBaseAddr = (unsigned int *)FGTU_TEX4_PSIZE;
		break;
	case 5:
		pTexRegBaseAddr = (unsigned int *)FGTU_TEX5_PSIZE;
		break;
	case 6:
		pTexRegBaseAddr = (unsigned int *)FGTU_TEX6_PSIZE;
		break;
	case 7:
		pTexRegBaseAddr = (unsigned int *)FGTU_TEX7_PSIZE;
		break;
	default:
		return FGL_ERR_INVALID_VALUE;
	}

	WRITEREG(pTexRegBaseAddr, pSize);
		
	return FGL_ERR_NO_ERROR;
}

/*****************************************************************************
 * FUNCTIONS: fglCalculateMipmapOffset
 *
 * 			refer to
 * 			drvsys_write_reg(FGTU_TEX#_L#_OFFSET, unsigned int)
 *
 * SYNOPSIS: This function sets a texture mipmap offset
 * 			 for each texture unit.
 * PARAMETERS: [IN]	unsigned int unit: texture unit ( 0~7 )
 *			   [IN]	unsigned int uSize: texture u size
 *			   [IN]	unsigned int vSize: texture v size
 * RETURNS: FGL_ERR_NO_ERROR - if successful.
 *          FGL_ERR_INVALID_SIZE - if texture size is invalid
 *          FGL_ERR_INVALID_VALUE - if specified unit is invalid
 * ERRNO: FGL_ERR_INVALID_SIZE = 6
 *        FGL_ERR_INVALID_VALUE = 7
 *****************************************************************************/

FGL_Error
fglCalculateMipmapOffset(
					unsigned int unit,
					unsigned int uUSize,
					unsigned int uVSize
		)
{
	unsigned int uTexMipMapSize;
	unsigned int uMipMapLevel;
	unsigned int TexRegs[10];
	unsigned int uCheckSize;
	unsigned int uIndex = 0;
	
	unsigned int* pTexRegBaseAddr;
//	unsigned int* pTexRegVal = TexRegs;
	
	unsigned int i;

	if ((uUSize > MAXVALUE_TU_TEX_SIZE) || (uVSize > MAXVALUE_TU_TEX_SIZE))
	{
		return FGL_ERR_INVALID_SIZE;
	}

	switch(unit)
	{
	case 0:
		pTexRegBaseAddr = (unsigned int *)FGTU_TEX0_L1_OFFSET;
		break;
	case 1:
		pTexRegBaseAddr = (unsigned int *)FGTU_TEX1_L1_OFFSET;
		break;
	case 2:
		pTexRegBaseAddr = (unsigned int *)FGTU_TEX2_L1_OFFSET;
		break;
	case 3:
		pTexRegBaseAddr = (unsigned int *)FGTU_TEX3_L1_OFFSET;
		break;
	case 4:
		pTexRegBaseAddr = (unsigned int *)FGTU_TEX4_L1_OFFSET;
		break;
	case 5:
		pTexRegBaseAddr = (unsigned int *)FGTU_TEX5_L1_OFFSET;
		break;
	case 6:
		pTexRegBaseAddr = (unsigned int *)FGTU_TEX6_L1_OFFSET;
		break;
	case 7:
		pTexRegBaseAddr = (unsigned int *)FGTU_TEX7_L1_OFFSET;
		break;
	default:
		return FGL_ERR_INVALID_VALUE;
	}


	uTexMipMapSize = (unsigned int)(uUSize * uVSize);

	TexRegs[uIndex] = uTexMipMapSize;

	WRITEREG(pTexRegBaseAddr++, TexRegs[uIndex++]);

	uCheckSize = uUSize > uVSize ? uUSize : uVSize;
	uCheckSize >>= 1;
	for(i = 0; uCheckSize > 0 && i < 11; i++) 
	{
   		++uMipMapLevel;
		uUSize = (uUSize <= 1) ? 1 : (uUSize >> 1);
		uVSize = (uVSize <= 1) ? 1 : (uVSize >> 1);
		uCheckSize >>= 1;
		uTexMipMapSize += uUSize * uVSize;
   		TexRegs[uIndex] = uTexMipMapSize;
		WRITEREG(pTexRegBaseAddr++, TexRegs[uIndex++]);
   	}

#if 0
#if 1
	__asm
	{
	LDMIA pTexRegVal!,{v1-v8}
	STMIA pTexRegBaseAddr!,{v1-v8}
	LDMIA pTexRegVal!,{v1-v8}
	STMIA pTexRegBaseAddr!,{v1-v8}
	}
    WRITEREG(pTexRegBaseAddr, *pTexRegVal);
#else
		WRITEREG(pTexRegBaseAddr++, *(pTexRegVal++));
		WRITEREG(pTexRegBaseAddr++, *(pTexRegVal++));
		WRITEREG(pTexRegBaseAddr++, *(pTexRegVal++));
		WRITEREG(pTexRegBaseAddr++, *(pTexRegVal++));
		WRITEREG(pTexRegBaseAddr++, *(pTexRegVal++));
		WRITEREG(pTexRegBaseAddr++, *(pTexRegVal++));
		WRITEREG(pTexRegBaseAddr++, *(pTexRegVal++));
		WRITEREG(pTexRegBaseAddr++, *(pTexRegVal++));
		WRITEREG(pTexRegBaseAddr++, *(pTexRegVal++));
		WRITEREG(pTexRegBaseAddr++, *(pTexRegVal++));
		WRITEREG(pTexRegBaseAddr++, *(pTexRegVal++));
		WRITEREG(pTexRegBaseAddr++, *(pTexRegVal++));
		WRITEREG(pTexRegBaseAddr++, *(pTexRegVal++));
		WRITEREG(pTexRegBaseAddr++, *(pTexRegVal++));
		WRITEREG(pTexRegBaseAddr++, *(pTexRegVal++));
		WRITEREG(pTexRegBaseAddr++, *(pTexRegVal++));
		WRITEREG(pTexRegBaseAddr++, *(pTexRegVal++));
#endif
#endif

	return FGL_ERR_NO_ERROR;
}


/*****************************************************************************
 * FUNCTIONS: fglSetTexMipmapLevel
 *
 * 			refer to
 * 			drvsys_write_reg(FGTU_TEX#_BASE_LEVEL, unsigned int)
 * 			drvsys_write_reg(FGTU_TEX#_MAX_LEVEL, unsigned int)
 *
 * SYNOPSIS: This function sets texture base level or max. level of mipmap
 * 			 for each texture unit.
 * PARAMETERS: [IN]	unsigned int unit: texture unit ( 0~7 )
 *			   [IN] FGL_MipmapLevel level: base level or max. level
 * 			 		 	typedef enum FGL_MipmapLevelTag {
 * 			 		 		FGL_MIPMAP_BASE_LEVEL,
 * 			 		 		FGL_MIPMAP_MAX_LEVEL
 * 			 		 	} FGL_MipmapLevel;
 * 			   [IN]	unsigned int value: 4 bits value for specified level
 * RETURNS: FGL_ERR_NO_ERROR - if successful
 *          FGL_ERR_INVALID_VALUE - if specified level or unit is invalid
 *          							or value is over the range
 * ERRNO: FGL_ERR_INVALID_VALUE = 7
 *****************************************************************************/
FGL_Error
fglSetTexMipmapLevel (
                                    unsigned int    unit,
	   			                    FGL_MipmapLevel level, 
	   			                    unsigned int    value
	   			               )
{
    unsigned int *pRegAddr = 0;

	if (((unsigned int)level > FGL_MIPMAP_MAX_LEVEL)
			|| (value > MAXVALUE_TU_MIPMAP_LEVEL))
	{
		return FGL_ERR_INVALID_VALUE;
	}

//	pRegAddr = (unsigned int *)(FGTU_TEX0_CTRL + unit * BITINTERVAL_TU_UNIT);

	switch(unit)
	{
	case 0:
		pRegAddr = (unsigned int *)((level==FGL_MIPMAP_MIN_LEVEL)?
					FGTU_TEX0_MIN_LEVEL:FGTU_TEX0_MAX_LEVEL);
		break;
	case 1:
		pRegAddr = (unsigned int *)((level==FGL_MIPMAP_MIN_LEVEL)?
					FGTU_TEX1_MIN_LEVEL:FGTU_TEX1_MAX_LEVEL);
		break;
	case 2:
		pRegAddr = (unsigned int *)((level==FGL_MIPMAP_MIN_LEVEL)?
					FGTU_TEX2_MIN_LEVEL:FGTU_TEX2_MAX_LEVEL);
		break;
	case 3:
		pRegAddr = (unsigned int *)((level==FGL_MIPMAP_MIN_LEVEL)?
					FGTU_TEX3_MIN_LEVEL:FGTU_TEX3_MAX_LEVEL);
		break;
	case 4:
		pRegAddr = (unsigned int *)((level==FGL_MIPMAP_MIN_LEVEL)?
					FGTU_TEX4_MIN_LEVEL:FGTU_TEX4_MAX_LEVEL);
		break;
	case 5:
		pRegAddr = (unsigned int *)((level==FGL_MIPMAP_MIN_LEVEL)?
					FGTU_TEX5_MIN_LEVEL:FGTU_TEX5_MAX_LEVEL);
		break;
	case 6:
		pRegAddr = (unsigned int *)((level==FGL_MIPMAP_MIN_LEVEL)?
					FGTU_TEX6_MIN_LEVEL:FGTU_TEX6_MAX_LEVEL);
		break;
	case 7:
		pRegAddr = (unsigned int *)((level==FGL_MIPMAP_MIN_LEVEL)?
					FGTU_TEX7_MIN_LEVEL:FGTU_TEX7_MAX_LEVEL);
		break;
	default:
		return FGL_ERR_INVALID_VALUE;
	}

	WRITEREG(pRegAddr, value);        
	return FGL_ERR_NO_ERROR;
       
}

/*****************************************************************************
 * FUNCTIONS: fglSetTexBaseAddr
 *
 * 			refer to
 * 			drvsys_write_reg(FGTU_TEX#_BASE_ADDR, unsigned int)
 *
 * SYNOPSIS: This function sets texture base address
 * 			 for each texture unit.
 * PARAMETERS: [IN]	unsigned int unit: texture unit ( 0~7 )
 * 			   [IN]	unsigned int addr: base address
 * RETURNS: FGL_ERR_NO_ERROR - if successful
 *          FGL_ERR_INVALID_VALUE - if specified unit is invalid
 * ERRNO: FGL_ERR_INVALID_VALUE = 7
 *****************************************************************************/
FGL_Error
fglSetTexBaseAddr(unsigned int unit, unsigned int addr)
{
    unsigned int *pTexRegBaseAddr;// = 0;

	switch(unit)
	{
	case 0:
		pTexRegBaseAddr = (unsigned int *)FGTU_TEX0_BASE_ADDR;
		break;
	case 1:
		pTexRegBaseAddr = (unsigned int *)FGTU_TEX1_BASE_ADDR;
		break;
	case 2:
		pTexRegBaseAddr = (unsigned int *)FGTU_TEX2_BASE_ADDR;
		break;
	case 3:
		pTexRegBaseAddr = (unsigned int *)FGTU_TEX3_BASE_ADDR;
		break;
	case 4:
		pTexRegBaseAddr = (unsigned int *)FGTU_TEX4_BASE_ADDR;
		break;
	case 5:
		pTexRegBaseAddr = (unsigned int *)FGTU_TEX5_BASE_ADDR;
		break;
	case 6:
		pTexRegBaseAddr = (unsigned int *)FGTU_TEX6_BASE_ADDR;
		break;
	case 7:
		pTexRegBaseAddr = (unsigned int *)FGTU_TEX7_BASE_ADDR;
		break;
	default:
		return FGL_ERR_INVALID_VALUE;   
	}

	WRITEREG(pTexRegBaseAddr, addr);        
	return FGL_ERR_NO_ERROR;
}

/*****************************************************************************
 * FUNCTIONS: fglSetTexColorKey
 *
 * 			refer to
 * 			drvsys_write_reg(FGTU_COLOR_KEY#, unsigned int)
 *
 * SYNOPSIS: This function sets 3D color key 1 or 2 register
 * PARAMETERS: [IN]	unsigned int unit: 3D color key register ( 0~1 )
 *			   [IN] FGL_TexCKey ckey: 3D color key value structure
 * 			 		 	typedef struct FGL_TexCKeyTag
 * 			 		 	{
 * 			 		 		unsigned char r;
 * 			 		 		unsigned char g;
 * 			 		 		unsigned char b;
 * 			 		 	} FGL_TexCKey;
 * RETURNS: FGL_ERR_NO_ERROR - if successful
 *          FGL_ERR_INVALID_VALUE - if specified unit is invalid
 * ERRNO: FGL_ERR_INVALID_VALUE = 7
 *****************************************************************************/
FGL_Error
fglSetTexColorKey(unsigned int unit, FGL_TexCKey ckey)
{

	unsigned int uR;
	unsigned int uG;
	unsigned int uB;

	unsigned int uPort;
	unsigned int uX;

	if (unit == 0)
	{
		uPort = FGTU_COLOR_KEY1;
	}
	else if (unit == 1)
	{
		uPort = FGTU_COLOR_KEY2;
	}
	else
	{
		return FGL_ERR_INVALID_VALUE;
	}
	uR = (unsigned int) ckey.r;
	uG = (unsigned int) ckey.g;
	uB = (unsigned int) ckey.b;

	uX = (uR << BITSHIFT_TU_CK_RED)
		| (uG << BITSHIFT_TU_CK_GREEN)
		| uB;

	WRITEREG(uPort, uX);

	return FGL_ERR_NO_ERROR;
}

/*****************************************************************************
 * FUNCTIONS: fglSetTexColorKeyYUV
 *
 * 			refer to
 * 			drvsys_write_reg(FGTU_TEX_COLOR_KEY_YUV, unsigned int)
 *
 * SYNOPSIS: This function sets 3D color key YUV register
 * PARAMETERS: [IN]	unsigned char g: color key green value
 * 			   [IN]	unsigned char b: color key blue value
 * RETURNS: FGL_ERR_NO_ERROR - It'll always succeed
 * ERRNO:
 *****************************************************************************/
FGL_Error
fglSetTexColorKeyYUV(unsigned char g, unsigned char b)
{
	unsigned int uG = (unsigned int)g;
	unsigned int uB = (unsigned int)b;

	WRITEREG(FGTU_COLOR_KEY_YUV, (uG << BITSHIFT_TU_CK_GREEN) | uB);
	return FGL_ERR_NO_ERROR;
}

/*****************************************************************************
 * FUNCTIONS: fglSetTexColorKeyMask
 *
 * 			refer to
 * 			drvsys_write_reg(FGTU_COLOR_KEY_MASK, unsigned int)
 *
 * SYNOPSIS: This function sets 3D color key mask register
 * PARAMETERS: [IN]	unsigned char bitsToMask: 3D color key mask value
 * RETURNS: FGL_ERR_NO_ERROR - if successful
 *          FGL_ERR_INVALID_VALUE - if specified mask is invalid
 * ERRNO: FGL_ERR_INVALID_VALUE = 7
 *****************************************************************************/
FGL_Error
fglSetTexColorKeyMask(unsigned char bitsToMask)
{
	if (bitsToMask > MAXVALUE_TU_CK_MASK)
	{
		return FGL_ERR_INVALID_VALUE;
	}

	WRITEREG(FGTU_COLOR_KEY_MASK, bitsToMask);

	return FGL_ERR_NO_ERROR;
}

/*****************************************************************************
 * FUNCTIONS: fglSetTexPaletteAddr
 *
 * 			refer to
 * 			drvsys_write_reg(FGTU_PALETTE_ADDR, unsigned int)
 *
 * SYNOPSIS: This function sets palette address for texturing
 * PARAMETERS: [IN]	unsigned int addr:  8-bit palette address
 * RETURNS: FGL_ERR_NO_ERROR - if successful
 *          FGL_ERR_INVALID_VALUE - if specified address is invalid
 * ERRNO: FGL_ERR_INVALID_VALUE = 7
 *****************************************************************************/
FGL_Error
fglSetTexPaletteAddr(unsigned int addr)
{
	if (addr > MAXVALUE_TU_PAL_ADDR)
	{
		return FGL_ERR_INVALID_VALUE;
	}

	WRITEREG(FGTU_PALETTE_ADDR, addr);

	return FGL_ERR_NO_ERROR;
}

/*****************************************************************************
 * FUNCTIONS: fglSetTexPaletteEntry
 *
 * 			refer to
 * 			drvsys_write_reg(FGTU_PALETTE_ENTRY, unsigned int)
 *
 * SYNOPSIS: This function sets palette entry for texturing
 * PARAMETERS: [IN]	unsigned int entry:  palette data in
 * RETURNS: FGL_ERR_NO_ERROR - It'll always succeed
 * ERRNO:
 *****************************************************************************/
FGL_Error
fglSetTexPaletteEntry(unsigned int entry)
{
	WRITEREG(FGTU_PALETTE_ENTRY, entry);

	return FGL_ERR_NO_ERROR;
}

/*****************************************************************************
 * FUNCTIONS: fglSetVtxTexUnitParams
 *
 * 			refer to
 * 			drvsys_write_reg(FGTU_VTXTEX#_CTRL, unsigned int)
 *
 * SYNOPSIS: This function sets vertex texture status register
 * 			 for each vertex texture unit.
 * PARAMETERS: [IN]	unsigned int unit: vertex texture unit ( 0~3 )
 *			   [IN] FGL_VtxTexStatusReg vts: vertex texture status structure
 * 			 		 	typedef struct FGL_VtxTexStatusRegTag
 * 			 		 	{
 * 			 		 		FGL_TexWrapMode uMode;
 * 			 		 		FGL_TexWrapMode vMode;
 * 			 		 		FGL_TexSize 	uSize;
 * 			 		 		FGL_TexSize 	vSize;
 * 			 		 	} FGL_VtxTexStatusReg;
 * RETURNS: FGL_ERR_NO_ERROR - if successful
 *          FGL_ERR_INVALID_SIZE - if vertex texture u or v size is invalid
 *          FGL_ERR_INVALID_VALUE - if specified unit or modes are invalid
 * ERRNO: FGL_ERR_INVALID_SIZE = 6
 * 		  FGL_ERR_INVALID_VALUE = 7
 *****************************************************************************/
FGL_Error
fglSetVtxTexUnitParams(unsigned int unit, FGL_VtxTexStatusReg *vts)
{
	unsigned int uUSize = (unsigned int)vts->uSize;
	unsigned int uVSize = (unsigned int)vts->vSize;

	unsigned int uUMode;
	unsigned int uVMode;

	unsigned int uPort;
	unsigned int uX;

	if ( (uUSize > FGL_TEX_SIZE_2048P) || (uVSize > FGL_TEX_SIZE_2048P) )
	{
		return FGL_ERR_INVALID_SIZE;
	}

	uUMode = (unsigned int)vts->uMode;
	uVMode = (unsigned int)vts->vMode;

	if ((uUMode > FGL_TEX_WRAP_CLAMP) || (uVMode > FGL_TEX_WRAP_CLAMP))
	{
		return FGL_ERR_INVALID_VALUE;
	}

	switch (unit)
	{
	case 0:
		uPort = FGTU_VTXTEX0_CTRL;
		break;
	case 1:
		uPort = FGTU_VTXTEX1_CTRL;
		break;
	case 2:
		uPort = FGTU_VTXTEX2_CTRL;
		break;
	case 3:
		uPort = FGTU_VTXTEX3_CTRL;
		break;
	default:
		return FGL_ERR_INVALID_VALUE;
	}

	uX = (uUMode << BITSHIFT_TU_VTX_UMODE)
		| (uVMode << BITSHIFT_TU_VTX_VMODE)
		| (uUSize << BITSHIFT_TU_VTX_USIZE)
		| uVSize;

	WRITEREG(uPort, uX);

	return FGL_ERR_NO_ERROR;
}


/*****************************************************************************
 * FUNCTIONS: fglSetVtxTexBaseAddr
 *
 * 			refer to
 * 			drvsys_write_reg(FGTU_TEX#_BASE_LEVEL, unsigned int)
 * 			drvsys_write_reg(FGTU_TEX#_MAX_LEVEL, unsigned int)
 *
 * SYNOPSIS: This function sets vertex texture base address
 * 			 for each vertex texture unit.
 * PARAMETERS: [IN]	unsigned int unit: vertex texture unit ( 0~3 )
 * 			   [IN]	unsigned int addr: vertex texture base address
 * RETURNS: FGL_ERR_NO_ERROR - if successful
 *          FGL_ERR_INVALID_VALUE - if specified unit is invalid
 * ERRNO: FGL_ERR_INVALID_VALUE = 7
 *****************************************************************************/
FGL_Error
fglSetVtxTexBaseAddr(unsigned int unit, unsigned int addr)
{
	unsigned int uPort;

	switch (unit)
	{
	case 0:
		uPort = FGTU_VTXTEX0_BASE_ADDR;
		break;
	case 1:
		uPort = FGTU_VTXTEX1_BASE_ADDR;
		break;
	case 2:
		uPort = FGTU_VTXTEX2_BASE_ADDR;
		break;
	case 3:
		uPort = FGTU_VTXTEX3_BASE_ADDR;
		break;
	default:
		return FGL_ERR_INVALID_VALUE;
	}

	WRITEREG(uPort, addr);

	return FGL_ERR_NO_ERROR;
}


