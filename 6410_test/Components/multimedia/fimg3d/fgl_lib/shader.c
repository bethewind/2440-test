/*******************************************************************************
*
* NAME         : fimg_shader.c
* TITLE        : FIMGSE Low Level API 
* AUTHOR       : Thomas, Kim
* CREATED      : 30 Aug 2005
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
* DESCRIPTION  : Declares types for FIMG shader and functions to manipulate them.
*
* PLATFORM     : ALL
* HISTORY      : 	 
* $RCSfile: fimg_shader.c,v $
* $Revision: 1.4 $
* $Author: cheolkyoo.kim $
* $Date: 2006/05/03 05:30:03 $
* $Locker:  $
* 
* $Source: C:/CVS/CVSrepository/FIMG-3DSE_SW/fimg3dse_fpga/fimg3d/device/fimg_shader.c,v $
* $State: Exp $
* $Log: fimg_shader.c,v $
* Revision 1.4  2006/05/03 05:30:03  cheolkyoo.kim
* no message
*
* Revision 1.3  2006/04/13 10:56:31  cheolkyoo.kim
* Replace FG_TRUE with FGL_TRUE.
*
* Revision 1.2  2006/04/05 02:22:39  cheolkyoo.kim
* outw and inw are repalced by WRITEREG and READREG
*
* Revision 1.1  2006/03/10 08:29:18  cheolkyoo.kim
* Initial import of FIMG-3DSE_SW package
*
*******************************************************************************/
#include "fgl.h"
#include "register.h"
#include "macros.h"

 
/*****************************************************************************
  DEFINES
 *****************************************************************************/

typedef enum FGL_ExecuteModeTag
{
	FGL_HOST_ACCESS_MODE = 0,
	FGL_PS_EXECUTE_MODE
} FGL_ExecuteMode;

typedef enum FGL_DeclareAttribTag
{
	FGL_ATRBDEF_POSITION  = 0x10,
	FGL_ATRBDEF_NORMAL    = 0x20,
	FGL_ATRBDEF_PCOLOR    = 0x40,
	FGL_ATRBDEF_SCOLOR    = 0x41,
	FGL_ATRBDEF_TEXTURE0  = 0x80,
	FGL_ATRBDEF_TEXTURE1  = 0x81,
	FGL_ATRBDEF_TEXTURE2  = 0x82,
	FGL_ATRBDEF_TEXTURE3  = 0x83,
	FGL_ATRBDEF_TEXTURE4  = 0x84,
	FGL_ATRBDEF_TEXTURE5  = 0x85,
	FGL_ATRBDEF_TEXTURE6  = 0x86,
	FGL_ATRBDEF_TEXTURE7  = 0x87,
	FGL_ATRBDEF_POINTSIZE = 0x1,
	FGL_ATRBDEF_USERDEF0  = 0x2,
	FGL_ATRBDEF_USERDEF1  = 0x3,
	FGL_ATRBDEF_USERDEF2  = 0x4,
	FGL_ATRBDEF_USERDEF3  = 0x5
} FGL_DeclareAttrib;

/*****************************************************************************
  DEFINES
 *****************************************************************************/


/*****************************************************************************
  MACROS
 *****************************************************************************/
/* Vertex/Pixel shader PC range value is the maximum 512 slots */
#define PROGRAM_COUNT_VALUE_VALID(range)	(range < 0x200)
/* The number of attributes are the maximum 10 in the vertex shader */
#define VS_MAX_ATTRIB_NUM_VALID(num)		((0 < num) && (num <= 10))
/* The number of attributes are the maximum 9 in the pixel shader */
#define PS_MAX_ATTRIB_NUM_VALID(num)		((0 < num) && (num <= 9))
 
 
/******************************************************************************
   Local function
 ******************************************************************************/

/* Local function prototypes */

static FGL_Error 
_PSExecuteMode ( 
					FGL_ExecuteMode exeMode 
			   );

static unsigned int 
_SearchAttribTable (
						unsigned int 		*pAttribTable,
						unsigned int		tableSize,
						FGL_DeclareAttrib	dclAttribName						
   				   );
   				   
   				   
/******************************************************************************
   Exported Functions
 ******************************************************************************/

// Vertex Shader Register-level API

/*****************************************************************************
 * FUNCTIONS: fglVSSetPCRange
 * SYNOPSIS: This function specifies a start and end address of vetex shader  
 *            program.
 * PARAMETERS: [IN] start: the start program count of vertex shader program.
 *             [IN] end: the end program count of vertex shader program.
 * RETURNS: FGL_ERR_NO_ERROR - if successful
 *          FGL_ERR_INVALID_VALUE - invalid start or end program count value.
 * ERRNO:   FGL_ERR_NO_ERROR        1
 *          FGL_ERR_INVALID_VALUE   7
 *****************************************************************************/
FGL_Error
fglVSSetPCRange (
					unsigned int start,
					unsigned int end
				)
{
	FGL_Error ret = FGL_ERR_UNKNOWN;
	FGL_BOOL bValidPCStart = FGL_FALSE;
	FGL_BOOL bValidPCEnd = FGL_FALSE;

	unsigned int nPCRange = 0;

	bValidPCStart = PROGRAM_COUNT_VALUE_VALID(start);
	bValidPCEnd = PROGRAM_COUNT_VALUE_VALID(end);

	if(bValidPCStart && bValidPCEnd && (start <= end))
	{
		FGL_SET_BITFIELD(nPCRange, 24:16, end);
		FGL_SET_BITFIELD(nPCRange,  8:0,  start);
		WRITEREG(FGVS_PC_RANGE, nPCRange);
		WRITEREG(FGVS_CONFIG, PROGRAM_COUNT_COPYOUT); // PC_RANGE vlaue is copied to VS inside

		ret = FGL_ERR_NO_ERROR;
	}
	else
	{
		if(!bValidPCStart)
		{
			//printf((DBG_ERROR, "Cannot set vertex shader pc vlaue - invalid start pc value"));
		}

		if(!bValidPCEnd)
		{
			//printf((DBG_ERROR, "Cannot set vertex shader pc vlaue - invalid end pc value"));
		}

		ret = FGL_ERR_INVALID_VALUE;
	}

	return ret;
}

/*****************************************************************************
 * FUNCTIONS: fglVSSetAttribNum
 * SYNOPSIS: This function specifies the number of attributes of current
 *           vertex shader programs
 * PARAMETERS: [in] outAttribNum: the number of attributes for the vertex
 *             shader output
 *             [in] inAttribNum: the number of attributes for the vertex
 *             shader input
 * RETURNS: FGL_ERR_NO_ERROR, if successful
 *          FGL_ERR_INVALID_VALUE - invalid the number of input or output
 *          attributes.
 * ERRNO:   FGL_ERR_NO_ERROR        1
 *          FGL_ERR_INVALID_VALUE   7
 *****************************************************************************/
FGL_Error fglVSSetAttribNum (unsigned int inAttribNum)
{
	FGL_Error ret = FGL_ERR_UNKNOWN;
	FGL_BOOL bValidInAttrib = FGL_FALSE;

	unsigned nAttribNum = 0;

	bValidInAttrib = VS_MAX_ATTRIB_NUM_VALID(inAttribNum);

	if(bValidInAttrib)
	{
		FGL_SET_BITFIELD(nAttribNum,  3:0,  inAttribNum);
		WRITEREG(FGVS_ATTRIB_NUM, nAttribNum);

		ret = FGL_ERR_NO_ERROR;
	}
	else
	{
		if(!bValidInAttrib)
		{
			//printf((DBG_ERROR, "Cannot set the number of input attrributes - invalid inAttribNum"));
		}
		ret = FGL_ERR_INVALID_VALUE;
	}

	return ret;
}

/*****************************************************************************
 * FUNCTIONS: fglMakeShaderAttribTable
 * SYNOPSIS:  to make a attribute table which is extracted from shader program.
 * PARAMETERS: [in] pVertexShader - the pointer variable of a vertex Program.
 *             [in] pPixelShader -  the pointer variable of a fragment program.
 *             [in] attribTable - the pointer variable pointing in/out attributes
 *                                table.          
 * RETURNS: FGL_ERR_NO_ERROR, if successful
 *          FGL_ERR_INVALID_PARAMETER - the pVertexShader and pPixelShader is NULL.
 *          FGL_ERR_INVALID_VALUE - the table info. is invalid.
 *          FGL_ERR_INVALID_SHADER_CODE - either magic number or shader version  
 *                                        were not an accepted value.
 * ERRNO:   FGL_ERR_NO_ERROR                1
 *          FGL_ERR_INVALID_PARAMETER       2
 *          FGL_ERR_INVALID_VALUE           7
 *          FGL_ERR_INVALID_SHADER_CODE     8
 *****************************************************************************/
FGL_Error
fglMakeShaderAttribTable (
							const unsigned int *pVertexShader,
							const unsigned int *pPixelShader,
							pFGL_ShaderAttribTable attribTable
					     )
{
	FGL_Error ret = FGL_ERR_UNKNOWN;
	FGL_ShaderHeader *pVShaderHeader;
	FGL_ShaderHeader *pPShaderHeader;
	unsigned int *pShaderBody;
	unsigned int *pVSOutTable;
	unsigned int *pPSInTable;
	unsigned int nTableSize;
	unsigned int i;

	if((pVertexShader != FGL_NULL) && (pVertexShader != FGL_NULL))
	{
		if(attribTable != FGL_NULL)
		{

			pVShaderHeader = (FGL_ShaderHeader*)pVertexShader;

			if ( (pVShaderHeader->Magic == VERTEX_SHADER_MAGIC) &&
			     (pVShaderHeader->Version == SHADER_VERSION) )
			{
				nTableSize = pVShaderHeader->InTableSize;
				pShaderBody = (unsigned int*)(&pVShaderHeader[1]);
				pVSOutTable = (unsigned int *)(pShaderBody + nTableSize);
				nTableSize = pVShaderHeader->OutTableSize;

				if( (nTableSize < FGSP_MAX_ATTRIBTBL_SIZE) &&
				    (pVShaderHeader->OutTableSize != 0) )
				{
					attribTable->outAttribTableSize = nTableSize;

					for(i=0; i<nTableSize; i++)
					{
						attribTable->vsOutAttribTable[i] = *pVSOutTable++;
					}
				}
				else
				{
					ret = FGL_ERR_INVALID_VALUE;
				}
			}
			else
			{
				ret = FGL_ERR_INVALID_SHADER_CODE;
			}

			pPShaderHeader = (FGL_ShaderHeader*)pPixelShader;

			if ( (pPShaderHeader->Magic == PIXEL_SHADER_MAGIC) &&
			     (pPShaderHeader->Version == SHADER_VERSION) )
			{
				pPSInTable = (unsigned int*)(&pPShaderHeader[1]);

				nTableSize = pPShaderHeader->InTableSize;

				if( (nTableSize < FGSP_MAX_ATTRIBTBL_SIZE) &&
				    (pPShaderHeader->InTableSize != 0) )
				{
					attribTable->inAttribTableSize = nTableSize;

					for(i=0; i<nTableSize; i++)
					{
						attribTable->psInAttribTable[i] = *pPSInTable++;
					}
				}
				else
				{
					ret = FGL_ERR_INVALID_VALUE;
				}
			}
			else
			{
				ret = FGL_ERR_INVALID_SHADER_CODE;
			}

			attribTable->validTableInfo = FGL_TRUE;
		}
		else
		{
			// printf
			ret = FGL_ERR_INVALID_PARAMETER;
		}
	}
	else
	{
		// printf
		ret = FGL_ERR_INVALID_PARAMETER;
	}

	return ret;
}

/*****************************************************************************
 * FUNCTIONS: fglRemapVShaderOutAttrib
 * SYNOPSIS: This function remap the input attribute index registers for 
 *           flexibility. The N-th input attribute from host is actually read 
 *           from the position indicated by the index looked up from the 
 *           AttribN corresponding to the register number of input register 
 *           in the shader program.
 * PARAMETERS: [in] pShaderAttribTable - the pointer variable pointing in/out 
 *                  attribute table which is extracted from shader program. 
 * RETURNS: FGL_ERR_NO_ERROR, if successful
 *          FGL_ERR_INVALID_PARAMETER - the pointer of pShaderAttribTable is NULL. 
 *          FGL_ERR_INVALID_VALUE - the table info. is invalid.
 * ERRNO:   FGL_ERR_NO_ERROR            1
 *          FGL_ERR_INVALID_PARAMETER   2 
 *          FGL_ERR_INVALID_VALUE       7
 *****************************************************************************/
FGL_Error
fglRemapVShaderOutAttrib (
							pFGL_ShaderAttribTable pShaderAttribTable
						 )
{
	FGL_Error ret = FGL_ERR_UNKNOWN;
	FGL_BOOL bValidInfo = FGL_FALSE;
	unsigned int i;
	unsigned int nInAttribNum = 0;
	unsigned int nOutAttribNum = 0;
	unsigned int *pOutAttribTable;	
	unsigned int *pInAttribTable;
	unsigned int uAttribIndex;	
	unsigned int uIndexCount = 0;
	
	unsigned int OutAttribIndex0 = 0x03020100;
	unsigned int OutAttribIndex1 = 0x07060504;
	unsigned int OutAttribIndex2 = 0x0B0A0908;

	if(pShaderAttribTable != FGL_NULL)
	{
		bValidInfo = pShaderAttribTable->validTableInfo;

		if(bValidInfo)
		{
			nInAttribNum = pShaderAttribTable->inAttribTableSize;
			nOutAttribNum = pShaderAttribTable->outAttribTableSize;
			if((nInAttribNum > 0) && (nOutAttribNum > 0) && (nOutAttribNum > nInAttribNum))
			{
				pOutAttribTable = (unsigned int *)&pShaderAttribTable->vsOutAttribTable[0];
				pInAttribTable = (unsigned int *)&pShaderAttribTable->psInAttribTable[0];
				
				uAttribIndex = _SearchAttribTable(pOutAttribTable, nOutAttribNum, FGL_ATRBDEF_POINTSIZE);
				
				if(uAttribIndex != FGL_ZERO)
				{
					uIndexCount++;
					FGL_SET_BITFIELD(OutAttribIndex0, 11:8, uIndexCount);
				}
				
				for(i = 0; i < nInAttribNum; i++)
				{
					uAttribIndex = _SearchAttribTable(pOutAttribTable, nOutAttribNum, (FGL_DeclareAttrib)pInAttribTable[i]);
					
					if(uAttribIndex != FGL_ZERO)
					{
						uIndexCount++;
						
						switch(uIndexCount)
						{
						case FGSP_ATTRIBUTE_INDEX1:
							FGL_SET_BITFIELD(OutAttribIndex0, 11:8, uAttribIndex);
							break;
						case FGSP_ATTRIBUTE_INDEX2:
							FGL_SET_BITFIELD(OutAttribIndex0, 19:16, uAttribIndex);
							break;
						case FGSP_ATTRIBUTE_INDEX3: 
							FGL_SET_BITFIELD(OutAttribIndex0, 27:24, uAttribIndex);
							break;
						case FGSP_ATTRIBUTE_INDEX4: 
							FGL_SET_BITFIELD(OutAttribIndex1, 3:0, uAttribIndex);
							break;
						case FGSP_ATTRIBUTE_INDEX5: 
							FGL_SET_BITFIELD(OutAttribIndex1, 11:8, uAttribIndex);
							break;
						case FGSP_ATTRIBUTE_INDEX6: 
							FGL_SET_BITFIELD(OutAttribIndex1, 19:16, uAttribIndex);
							break;
						case FGSP_ATTRIBUTE_INDEX7: 
							FGL_SET_BITFIELD(OutAttribIndex1, 27:24, uAttribIndex);
							break;
						case FGSP_ATTRIBUTE_INDEX8: 
							FGL_SET_BITFIELD(OutAttribIndex2, 3:0, uAttribIndex);
							break;
						case FGSP_ATTRIBUTE_INDEX9: 
							FGL_SET_BITFIELD(OutAttribIndex2, 11:8, uAttribIndex);
							break;
						case FGSP_ATTRIBUTE_INDEXA: 
							FGL_SET_BITFIELD(OutAttribIndex2, 19:16, uAttribIndex);
							break;
						case FGSP_ATTRIBUTE_INDEXB: 
							FGL_SET_BITFIELD(OutAttribIndex2, 27:24, uAttribIndex);
							break;
						default:
							break;
						}
					}
					else
					{
						//printf();
						return FGL_ERR_INVALID_VALUE; 	
					}
				}
				
				WRITEREG(FGVS_OUT_ATTRIB_IDX0, OutAttribIndex0); 
				WRITEREG(FGVS_OUT_ATTRIB_IDX1, OutAttribIndex1);
				WRITEREG(FGVS_OUT_ATTRIB_IDX2, OutAttribIndex2);
				
				ret = FGL_ERR_NO_ERROR;
			}
			else
			{
				//printf();
				ret = FGL_ERR_INVALID_VALUE;
			}
		}
		else
		{
			//printf();
			ret = FGL_ERR_INVALID_VALUE;
		}
	}
	else
	{
		//printf();
		ret = FGL_ERR_INVALID_PARAMETER;
	}

	return ret;
}


/*****************************************************************************
 * FUNCTIONS: fglSetVShaderAttribTable
 * SYNOPSIS: This function specifies in/out arrributes order of the vertex  
 *           shader program which vertex shader will be used.
 * PARAMETERS: [in] idx - a index of the attribute table.
 *             [in] value - a value to order attributes.
 * RETURNS: FGL_ERR_NO_ERROR, if successful
 *          FGL_ERR_INVALID_PARAMETER - the idx is not a type of enumeration.
 *          FGL_ERR_INVALID_VALUE - the PC exceeds the range of max attributes
 * ERRNO:   FGL_ERR_NO_ERROR            1
 *          FGL_ERR_INVALID_PARAMETER   2
 *          FGL_ERR_INVALID_VALUE       7
 *****************************************************************************/
FGL_Error
fglSetVShaderAttribTable (
							FGL_AttribTableIdx idx,
							unsigned int value
						 )
{
    int i = 0;
    unsigned int uSubVal = 0;
    FGL_BOOL bValidParam = FGL_TRUE;
    
    for(i = 0; i < 4; i++)
    {
        uSubVal = (value >> (i*FGSP_ATTRIB_IO_IDX_SHIFT)) & FGSP_ATTRIB_IO_IDX_MASK;
        if(uSubVal > FGSP_ATTRIBUTE_INDEXB && bValidParam)
            bValidParam = FGL_FALSE; 
    }
    
    if(bValidParam)
    {
         switch(idx)
        {
    	case FGL_INPUT_ATTRIB_IDX0:
    	    WRITEREG(FGVS_IN_ATTRIB_IDX0, value); 
            break;
    	case FGL_INPUT_ATTRIB_IDX1:
    	    WRITEREG(FGVS_IN_ATTRIB_IDX1, value); 
            break;
    	case FGL_INPUT_ATTRIB_IDX2:
    	    WRITEREG(FGVS_IN_ATTRIB_IDX2, value); 
            break;
    	case FGL_OUTPUT_ATTRIB_IDX0:
    	    WRITEREG(FGVS_OUT_ATTRIB_IDX0, value); 
            break;
    	case FGL_OUTPUT_ATTRIB_IDX1:
    	    WRITEREG(FGVS_OUT_ATTRIB_IDX1, value); 
            break;
    	case FGL_OUTPUT_ATTRIB_IDX2:
    	    WRITEREG(FGVS_OUT_ATTRIB_IDX2, value); 
            break;
        default:
            return FGL_ERR_INVALID_PARAMETER;
        }

        
        return FGL_ERR_NO_ERROR;
   }
   else
   {
        return FGL_ERR_INVALID_VALUE;
   }
    
}


// Fragment Shader Register-level API

/*****************************************************************************
 * FUNCTIONS: fglPSSetPCRange
 * SYNOPSIS: This function specifies the start and end address of the fragment  
 *           shader program which fragment shader will be used.
 * PARAMETERS: [in] start - a start address of the pixel shader program.
 *             [in] end - an end address of the pixel shader program.
 * RETURNS: FGL_ERR_NO_ERROR, if successful
 *          FGL_ERR_INVALID_VALUE - the PC exceeds the range of 512 slots
 * ERRNO:   FGL_ERR_NO_ERROR        1
 *          FGL_ERR_INVALID_VALUE   7
 *****************************************************************************/
FGL_Error
fglPSSetPCRange (unsigned int start, unsigned int end )
{
	FGL_Error ret = FGL_ERR_UNKNOWN;
	FGL_BOOL bValidPCStart = FGL_FALSE;
	FGL_BOOL bValidPCEnd = FGL_FALSE;

	bValidPCStart = PROGRAM_COUNT_VALUE_VALID(start);
	bValidPCEnd = PROGRAM_COUNT_VALUE_VALID(end);

	if(bValidPCStart && bValidPCEnd && (start <= end))
	{
		ret = _PSExecuteMode(FGL_HOST_ACCESS_MODE);
		
		if (ret != FGL_ERR_NO_ERROR)
		{
			//printf((DBG_ERROR, " _PSExecuteMode is fail. - invalid Execute maode parameter"));
			return ret;
		}

		WRITEREG(FGPS_PC_START, start); 			// Start address of pixel shader program
		WRITEREG(FGPS_PC_END, end); 				// End address of pixel shader program
		WRITEREG(FGPS_PC_COPY, PROGRAM_COUNT_COPYOUT);

		ret = _PSExecuteMode(FGL_PS_EXECUTE_MODE);

		if (ret != FGL_ERR_NO_ERROR)
		{
			//printf((DBG_ERROR, " _PSExecuteMode is fail. - invalid Execute maode parameter"));
			return ret;
		}

		ret = FGL_ERR_NO_ERROR;
	}
	else
	{
		if(!bValidPCStart)
		{
			//printf((DBG_ERROR, "Cannot set pixel shader pc vlaue - invalid start pc value"));
		}

		if(!bValidPCEnd)
		{
			//printf((DBG_ERROR, "Cannot set pixel shader pc vlaue - invalid end pc value"));
		}

		ret = FGL_ERR_INVALID_VALUE;
	}

	return ret;
}




/*****************************************************************************
 * FUNCTIONS: fglPSSetAttributeNum
 * SYNOPSIS: this function specifies the value ranged between 1 and 8 according to 
 *           the number of semantics such as color and texture coordinate 
 *           which are transferred to pixel shader.
 * PARAMETERS: [in] attributeNum - the number of attribute for current context.
 * RETURNS: FGL_ERR_NO_ERROR, if successful
 *          FGL_ERR_INVALID_VALUE -the sizes of attribute exceed the maximum number.
 * ERRNO:   FGL_ERR_NO_ERROR        1
 *          FGL_ERR_INVALID_VALUE   7
 *****************************************************************************/
FGL_Error
fglPSSetAttributeNum ( unsigned int attributeNum )
{

	FGL_Error ret = FGL_ERR_UNKNOWN;
	FGL_BOOL bValidNumAttrib = FGL_FALSE;
	unsigned int uInBufStatus;

	bValidNumAttrib = PS_MAX_ATTRIB_NUM_VALID(attributeNum);

	if(bValidNumAttrib)
	{
		ret = _PSExecuteMode(FGL_HOST_ACCESS_MODE);

		if (ret != FGL_ERR_NO_ERROR)
		{
			//printf((DBG_ERROR, " _PSExecuteMode is fail. - invalid Execute maode parameter"));
			return ret;
		}

		WRITEREG(FGPS_ATTRIB_NUM, attributeNum);

		do
		{
			fglPSGetInBufferStatus(&uInBufStatus);
		}
		while(INPUT_BUFFER_READY != uInBufStatus);

		ret = _PSExecuteMode(FGL_PS_EXECUTE_MODE);

		if (ret != FGL_ERR_NO_ERROR)
		{
			//printf((DBG_ERROR, " _PSExecuteMode is fail. - invalid Execute maode parameter"));
			return ret;
		}

		ret = FGL_ERR_NO_ERROR;
	}
	else
	{
		//printf((DBG_ERROR, "Cannot set pixel shader pc vlaue - invalid end pc value"));
		ret = FGL_ERR_INVALID_VALUE;
	}

	return ret;
}

/*****************************************************************************
 * FUNCTIONS: fglPSGetInBufferStatus
 * SYNOPSIS: this function read status register for monitoring fragment shader  
 *           input buffer initialization status.
 * PARAMETERS: [out] ready - the stauts signal which means that PS Input Buffer
 *                           initialization is not completed.
 * RETURNS: FGL_ERR_NO_ERROR, if successful
 * ERRNO:   FGL_ERR_NO_ERROR        1
 *****************************************************************************/
FGL_Error
fglPSGetInBufferStatus ( unsigned int *ready )
{
	FGL_Error ret = FGL_ERR_UNKNOWN;

	unsigned int uInBufStatus = 0;
	READREGP(FGPS_INBUF_STATUS, uInBufStatus);

	if(uInBufStatus & PS_INBUF_STATUS_MASK)
	{
		//printf((DBG_ERROR, "The input buffer of pixel shader is not ready);
		*ready = (unsigned int)INPUT_BUFFER_NOT_READY;
		ret = FGL_ERR_NO_ERROR;
	}
	else
	{
		//printf((DBG_ERROR, "The input buffer of pixel shader is not ready);
		*ready = (unsigned int)INPUT_BUFFER_READY;
		ret = FGL_ERR_NO_ERROR;
	}

	return ret;
}


/*****************************************************************************
 * FUNCTIONS: fglLoadVShader
 * SYNOPSIS: this function uploads a vertex shader program to shader memory   
 *           such as constant integer, float and instruction.
 * PARAMETERS: [in] pShaderCode - the pointer of vertex shader program.
 * RETURNS: FGL_ERR_NO_ERROR, if successful
 *          FGL_ERR_INVALID_VALUE - the program count exceeds the range of 512 slots
 *          FGL_ERR_INVALID_SHADER_CODE - either magic number or shader version  
 *                                        were not an accepted value.
 * ERRNO:   FGL_ERR_NO_ERROR                1
 *          FGL_ERR_INVALID_VALUE           7
 *          FGL_ERR_INVALID_SHADER_CODE     8
 *****************************************************************************/
FGL_Error
fglLoadVShader (const unsigned int *pShaderCode )
{
	unsigned int i;
	unsigned int size;
	unsigned int offset = 0;
	unsigned int AttribIdx = 0;
	unsigned int IdxVal = 0;
	volatile unsigned int *pAddr;
	volatile unsigned int *pShaderData;

	FGL_ShaderHeader *pShaderHeader = (FGL_ShaderHeader*)pShaderCode;
	unsigned int *pShaderBody = (unsigned int*)(&pShaderHeader[1]);

	if ((pShaderHeader->Magic != VERTEX_SHADER_MAGIC) || (pShaderHeader->Version != SHADER_VERSION))
	{
		return FGL_ERR_INVALID_SHADER_CODE;
	}

	if((pShaderHeader->InTableSize != 0) && (pShaderHeader->OutTableSize != 0))
	{
		// TODO
		WRITEREG(FGVS_ATTRIB_NUM, ( (pShaderHeader->InTableSize)<<ATTRIBUTE_NUM_SHIFT)
		                           |(pShaderHeader->OutTableSize));

		pAddr = (unsigned int *)FGVS_IN_ATTRIB_IDX0;
		size = pShaderHeader->InTableSize;
		offset += size;

		while (size & 0xFFFFFFFC)
		{
			//AttribIdx = ((IdxVal++) | ((IdxVal++) << 8) | ((IdxVal++)<<16) | ((IdxVal++) << 24));
			AttribIdx |= (IdxVal++);
			AttribIdx |= ((IdxVal++) << 8);
			AttribIdx |= ((IdxVal++) << 16);
			AttribIdx |= ((IdxVal++) << 24);
			
			WRITEREG(pAddr++, AttribIdx);
			size -= 4;
			AttribIdx = 0;
		}

		for(i=0; i<size; i++)
		{
			AttribIdx |= (IdxVal++)<<(i*8);
		}

		//WRITEREG(pAddr++, AttribIdx);
		WRITEREG(pAddr++, 0x03020100);

		size = pShaderHeader->OutTableSize;
		offset += size;
		IdxVal = 0;

		while (size & 0xFFFFFFFC)
		{
			AttribIdx = ((IdxVal++) | ((IdxVal++) << 8) | ((IdxVal++)<<16) | ((IdxVal++) << 24));
			WRITEREG(pAddr++, AttribIdx);
			size -= 4;
			AttribIdx = 0;
	    }

		for(i=0; i<size; i++)
	    {
			AttribIdx |= (IdxVal++)<<(i*8);
		}

		WRITEREG(pAddr++, AttribIdx);
	}

	if(pShaderHeader->SamTableSize != 0)
	{
		// TODO
		offset += pShaderHeader->SamTableSize;
	}
	

	if(pShaderHeader->InstructSize != 0)
	{ 
		// vertex shader instruction memory start addr.
		pAddr = (unsigned int *)FGVS_INSTMEM_SADDR;
		pShaderData =(unsigned int *)(pShaderBody + offset);
		size = pShaderHeader->InstructSize;
		offset += size;

		if(PROGRAM_COUNT_VALUE_VALID(size))
		{
		// Program counter start/end address setting
			WRITEREG(FGVS_PC_RANGE, ((size >> 2) - 1) << PROGRAM_COUNT_END_SHIFT);
			WRITEREG(FGVS_CONFIG, PROGRAM_COUNT_COPYOUT); // PC_RANGE vlaue is copied to VS inside
			//drvsys_polling_mask(FGVS_CONFIG, 0x0, 0x1);
		}
		else
		{
			//printf((DBG_ERROR, "Cannot set vertex shader pc vlaue - invalid end pc value"));
			return FGL_ERR_INVALID_VALUE;
		}
		
		do
		{
			WRITEREG(pAddr++, *pShaderData++);
		} while(--size != 0);
	} 

	if(pShaderHeader->ConstFloatSize != 0)
	{ 
		// vertex shader float memory start addr.
		pAddr = (unsigned int *)FGVS_CFLOAT_SADDR;
		pShaderData =(unsigned int *)(pShaderBody + offset);
		size = pShaderHeader->ConstFloatSize;
		offset += size;

		do
		{
			WRITEREG(pAddr++, *pShaderData++);
		} while(--size != 0);
	} 

	if(pShaderHeader->ConstIntSize != 0)
	{ 
		// vertex shader integer memory start addr.
		pAddr = (unsigned int *)FGVS_CINT_SADDR;
		pShaderData =(unsigned int *)(pShaderBody + offset);
		size = pShaderHeader->ConstIntSize;
		offset += size;

		do
		{
			WRITEREG(pAddr++, *pShaderData++);
		} while(--size != 0);
	} 

	if(pShaderHeader->ConstBoolSize != 0)
	{ 
		pShaderData =(unsigned int *)(pShaderBody + offset);
		WRITEREG(FGVS_CBOOL_SADDR, *pShaderData);
	} 

	return FGL_ERR_NO_ERROR;
}

/*****************************************************************************
 * FUNCTIONS: fglLoadPShader
 * SYNOPSIS: this function uploads a fragment shader program to shader memory   
 *           such as constant integer, float and instruction.    
 * PARAMETERS: [in] pShaderCode - the pointer of fragment shader program.
 * RETURNS: FGL_ERR_NO_ERROR, if successful
 *          FGL_ERR_INVALID_VALUE - the program count exceeds the range of 512 slots
 *          FGL_ERR_INVALID_SHADER_CODE - either magic number or shader version  
 *                                        were not an accepted value.  
 * ERRNO:   FGL_ERR_NO_ERROR                1
 *          FGL_ERR_INVALID_VALUE           7
 *          FGL_ERR_INVALID_SHADER_CODE     8
 *****************************************************************************/
FGL_Error
fglLoadPShader (const unsigned int *pShaderCode )
{
	FGL_Error ret = FGL_ERR_UNKNOWN;
	FGL_BOOL bValidParam = FGL_FALSE;
	//unsigned int i;
	unsigned int size;
	unsigned int offset = 0;
	unsigned int uAttribNum = 0;
	unsigned int uInBufStatus = 0;
	volatile unsigned int *pAddr;
	volatile unsigned int *pShaderData;

	FGL_ShaderHeader *pShaderHeader = (FGL_ShaderHeader*)pShaderCode;
	unsigned int *pShaderBody = (unsigned int*)(&pShaderHeader[1]);


	if ((pShaderHeader->Magic != PIXEL_SHADER_MAGIC) || (pShaderHeader->Version != SHADER_VERSION))
	{
		return FGL_ERR_INVALID_SHADER_CODE;
	}

	uAttribNum = pShaderHeader->InTableSize;
	bValidParam = PS_MAX_ATTRIB_NUM_VALID(uAttribNum);
	
	if( bValidParam && (uAttribNum != FGL_ZERO) )
	{
		ret = _PSExecuteMode(FGL_HOST_ACCESS_MODE);

		if (ret != FGL_ERR_NO_ERROR)
		{
			//printf((DBG_ERROR, " _PSExecuteMode is fail. - invalid Execute maode parameter"));
			return ret;
		}

		WRITEREG(FGPS_ATTRIB_NUM, uAttribNum);

		do
		{
			fglPSGetInBufferStatus(&uInBufStatus);
		}
		while(INPUT_BUFFER_READY != uInBufStatus);

		ret = _PSExecuteMode(FGL_PS_EXECUTE_MODE);

		if (ret != FGL_ERR_NO_ERROR)
	    {
			//printf((DBG_ERROR, " _PSExecuteMode is fail. - invalid Execute maode parameter"));
			return ret;
		}

		offset += pShaderHeader->InTableSize;
	}

	if(pShaderHeader->OutTableSize != 0)
	{
		// TODO
		offset += pShaderHeader->OutTableSize;
	}

	if(pShaderHeader->SamTableSize != 0)
	{
		// TODO
		offset += pShaderHeader->SamTableSize;
	}
	

	
	if(pShaderHeader->InstructSize != 0)
	{ 
		// vertex shader instruction memory start addr.
		pAddr = (unsigned int *)FGPS_INSTMEM_SADDR;
		pShaderData =(unsigned int *)(pShaderBody + offset);
		size = pShaderHeader->InstructSize;
		offset += size;

		bValidParam = PROGRAM_COUNT_VALUE_VALID(size);

		if(bValidParam)
		{
			ret = _PSExecuteMode(FGL_HOST_ACCESS_MODE);
			
			if (ret != FGL_ERR_NO_ERROR)
			{
				//printf((DBG_ERROR, " _PSExecuteMode is fail. - invalid Execute maode parameter"));
				return ret;
			}

			WRITEREG(FGPS_PC_START, FGL_ZERO);
			WRITEREG(FGPS_PC_END, ((size >> 2) - 1));
			WRITEREG(FGPS_PC_COPY, PROGRAM_COUNT_COPYOUT);

			ret = _PSExecuteMode(FGL_PS_EXECUTE_MODE);

			if (ret != FGL_ERR_NO_ERROR)
			{
				//printf((DBG_ERROR, " _PSExecuteMode is fail. - invalid Execute maode parameter"));
				return ret;
			}

			ret = FGL_ERR_NO_ERROR;
		}
		else
		{
			//printf((DBG_ERROR, "Cannot set pixel shader pc vlaue - invalid end pc value"));
			ret = FGL_ERR_INVALID_VALUE;
		}

		do
		{
			WRITEREG(pAddr++, *pShaderData++);
		} while(--size != 0);

	} 

	if(pShaderHeader->ConstFloatSize != 0)
	{ 
		// vertex shader float memory start addr.
		pAddr = (unsigned int *)FGPS_CFLOAT_SADDR;
		pShaderData =(unsigned int *)(pShaderBody + offset);
		size = pShaderHeader->ConstFloatSize;
		offset += size;

		do
		{
			WRITEREG(pAddr++, *pShaderData++);
		} while(--size != 0);
	} 

	if(pShaderHeader->ConstIntSize != 0)
	{ 
		// vertex shader integer memory start addr.
		pAddr = (unsigned int *)FGPS_CINT_SADDR;
		pShaderData =(unsigned int *)(pShaderBody + offset);
		size = pShaderHeader->ConstIntSize;
		offset += size;

		do
		{
			WRITEREG(pAddr++, *pShaderData++);
		} while(--size != 0);
	} 

	if(pShaderHeader->ConstBoolSize != 0)
	{ 
		pShaderData =(unsigned int *)(pShaderBody + offset);
		WRITEREG(FGPS_CBOOL_SADDR, *pShaderData);
	}

	return FGL_ERR_NO_ERROR;
}


/*****************************************************************************
  INTERNAL FUNCTIONS
 *****************************************************************************/

static FGL_Error 
_PSExecuteMode (
					FGL_ExecuteMode exeMode
			   )
{
	FGL_Error ret = FGL_ERR_UNKNOWN;

	//if(exeMode == (FGL_ExecuteMode)READREG(FGPS_EXE_MODE))
	unsigned int uReadReg;
	READREGP(FGPS_EXE_MODE, uReadReg);
	if(exeMode == (FGL_ExecuteMode)uReadReg)
	{
		return FGL_ERR_NO_ERROR;
	}

	switch(exeMode)
	{
	case FGL_HOST_ACCESS_MODE:

		//if((READREG(FG_PIPELINE_STATUS) & PS_ISNOT_EMPTY_MASK) != FGL_ZERO)
		READREGP(FG_PIPELINE_STATUS, uReadReg);
		if((uReadReg & PS_ISNOT_EMPTY_MASK) != FGL_ZERO)
		{
			//printf((DBG_ERROR, "Pipeline status is not empty"));
			return FGL_ERR_STATUS_BUSY;
		}

		WRITEREG(FGPS_EXE_MODE, HOST_ACCESS_MODE);
		ret = FGL_ERR_NO_ERROR;
		break;

	case FGL_PS_EXECUTE_MODE:

		//if((READREG(FGPS_INBUF_STATUS) & PS_INBUF_STATUS_MASK) != FGL_ZERO)
		READREGP(FGPS_INBUF_STATUS, uReadReg);
		if((uReadReg & PS_INBUF_STATUS_MASK) != FGL_ZERO)
		{
			//printf((DBG_ERROR, "The input buffer of pixel shader is not ready);
			return FGL_ERR_STATUS_BUSY;
		}

		WRITEREG(FGPS_EXE_MODE, PS_EXECUTION_MODE);
		ret = FGL_ERR_NO_ERROR;
		break;

	default:
		//printf((DBG_ERROR, "Cannot set PS execute mode - invalid exeMode"));
		ret = FGL_ERR_INVALID_PARAMETER;
		break;
	}

	return ret;
}


static unsigned int 
_SearchAttribTable (
						unsigned int 		*pAttribTable,
						unsigned int		tableSize,
						FGL_DeclareAttrib	dclAttribName						
   				   )
{
	unsigned int i;
	
	if(pAttribTable != FGL_NULL)
	{
		for(i=0; i < tableSize; i++)
		{
			if(pAttribTable[i] == dclAttribName)
			{
				return i;	
			}
		}
	} 

	return FGL_ZERO;
}

