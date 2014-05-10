
#include "vsparser.h"

//---------------------------------------------------------
// Function Name : u8 Vsparser(MFC_CODEC_MODE eDecType, u32  uBufferStartAddr, u32 uBufferByteSize, u8 bIsHeader,  //inputs
// u32 * puFrameStartAddr, u32 *puFrameSize, u32* puNumOfFrames) //outputs
//
// Function Description :
// This function parses H.264, H.263, MPEG4 and  VC-1  streams and returns the physical start address and size of  1 frame.
//
//
// Input : None
// Output : 
//---------------------------------------------------------
u8 Vsparser(MFC_CODEC_MODE eDecType, u32  uBufferStartAddr, u32 uBufferByteSize, u8 bIsHeader,  //inputs
	u32 * puFrameStartAddr, u32 *puFrameSize, u32* puNumOfFrames, u8* bIsMpeg4ShortHeader) //outputs
{
	
	u32 i;
	u32	uTemp32;
	u8 *pucSrcMem;
	u8 bIsFrameCorrect;
	
	switch(eDecType)
	{
		case VC1_DEC:	
			
			//  VC-1 stream file parsing: Begin
			//=============================================================================================
		 	pucSrcMem = (u8*) uBufferStartAddr;
			*puFrameStartAddr = uBufferStartAddr;
			if(bIsHeader == true)
			{
				//Inp32(pucSrcMem, uTemp32);
				uTemp32 = Inp32(pucSrcMem);
		 		
		 		if ( ((uTemp32 >>24 ) &0x000000ff) != (u32) 0x000000C5)
		 		{
					UART_Printf(" This file is not VC-1 stream file! Choose *.rcv file \n");
					return false;
		 		}
				uTemp32 = uTemp32 & 0x00ffffff; 		 // lower 24 bit is the number of frames
				UART_Printf(" This file has %d frames \n", uTemp32);
 				*puFrameSize = 9*4; 					// Vc-1 Sequence Header meta data  has 9* 4 byte length.
				*puNumOfFrames = uTemp32;
				if(*puFrameSize > uBufferByteSize)
				{
					return false;
				}
				return true;
			}
			else
			{
				uTemp32 = Inp32Not4ByteAlign((u32) pucSrcMem);
				Assert( ( (uTemp32 & 0xff000000) == 0x80000000) ||  ((uTemp32 & 0xff000000) == 0x00000000));
				uTemp32 = uTemp32 & 0x00ffffff;
				*puFrameSize = 2*4+uTemp32; // Vc-1 Frame Layer header  has 2* 4 byte length.
				if(*puFrameSize > uBufferByteSize)
				{
					return false;
				}
				return true;
			}
	
			case AVC_DEC:
	
				break;
	
			case MP4_DEC:
	
				//  MPEG4  stream file parsing: Begin
				//=============================================================================================
		
			 	pucSrcMem = (u8*) uBufferStartAddr;
				if(bIsHeader == true)
				{
					*puFrameStartAddr = (u32) pucSrcMem;
					//Inp32(pucSrcMem, uTemp32); //FPGA
					uTemp32 = Inp32(pucSrcMem); //SMDK
					uTemp32 = ((uTemp32 &0xff) <<24)
							|(((uTemp32 >>8) &0xff) <<16)
							|(((uTemp32 >>16) &0xff) <<8)
							|((uTemp32 >>24) &0xff);
					uTemp32 = uTemp32 & 0xfffffc00;
					if(uTemp32 != 0x00008000)  // not short header
					{
						*bIsMpeg4ShortHeader = false;
						// find start_code 0x000001		
						for(i= 1; i < uBufferByteSize - *puFrameStartAddr + uBufferStartAddr ;i++)
						{
						if ( (*(pucSrcMem)  ==  0x00) && (*(pucSrcMem+1)  ==  0x00)&& (*(pucSrcMem+2)  ==  0x01)&& (*(pucSrcMem+3)  ==  0xb6)) 
							{ 
							*puFrameSize = (u32) (pucSrcMem) -  (u32) (*puFrameStartAddr);
							return true;
							}					
						pucSrcMem++;
						}
					}
					else 
					{
						*bIsMpeg4ShortHeader = true;
						// find short_header 22'b0000_0000_0000_00000_1000_00		
						for(i= 1; i < uBufferByteSize;i++)
						{
						if ( (*(pucSrcMem)  ==  0x00) && (*(pucSrcMem+1)  ==  0x00)&& ((*(pucSrcMem+2)&0xfc)  ==  0x80)) 
						{ 
							*puFrameStartAddr = (u32) pucSrcMem;
							pucSrcMem++;
							break;
						}
						pucSrcMem++;
						}
						for(i= 1; i < uBufferByteSize - *puFrameStartAddr + uBufferStartAddr ;i++)
						{
							if ( (*(pucSrcMem)  ==  0x00) && (*(pucSrcMem+1)  ==  0x00)&& ((*(pucSrcMem+2)&0xfc)  ==  0x80)) 
							{ 
								*puFrameSize = (u32) pucSrcMem - *puFrameStartAddr;
								return true;
							}					
						pucSrcMem++;
						}
					}
					return false;
				}
				else   
				{
					bIsFrameCorrect = false;
					if(* bIsMpeg4ShortHeader == false)
					{
						// find start_code 0x000001		
						for(i= 1; i < uBufferByteSize;i++)
						{
							if ( (*(pucSrcMem)  ==  0x00) && (*(pucSrcMem+1)  ==  0x00)&& (*(pucSrcMem+2)  ==  0x01)&& (*(pucSrcMem+3)  ==  0xb6)) 
							{ 
								*puFrameStartAddr = (u32) pucSrcMem;
								pucSrcMem++;
								break;
							}					
							pucSrcMem++;
						}
		
						for(i= 1; i < uBufferByteSize - *puFrameStartAddr + uBufferStartAddr ;i++)
						{
							if ( (*(pucSrcMem)  ==  0x00) && (*(pucSrcMem+1)  ==  0x00)&& (*(pucSrcMem+2)  ==  0x01)&& (*(pucSrcMem+3)  ==  0xb6)) 
							{ 
								*puFrameSize = (u32) pucSrcMem - *puFrameStartAddr;
								bIsFrameCorrect = true;
								break;
							}					
							pucSrcMem++;
						}
						return bIsFrameCorrect;
					}
					else 
					{
						// find short_header 22'b0000_0000_0000_00000_1000_00		
		
						for(i= 1; i < uBufferByteSize;i++)
						{
							if ( (*(pucSrcMem)  ==  0x00) && (*(pucSrcMem+1)  ==  0x00)&& ((*(pucSrcMem+2)&0xfc)  ==  0x80)) 
							{ 
								*puFrameStartAddr = (u32) pucSrcMem;
								pucSrcMem++;
								break;
							}
							pucSrcMem++;
						}
					
						for(i= 1; i < uBufferByteSize - *puFrameStartAddr + uBufferStartAddr ;i++)
						{
							if ( (*(pucSrcMem)  ==  0x00) && (*(pucSrcMem+1)  ==  0x00)&& ((*(pucSrcMem+2)&0xfc)  ==  0x80)) 
							{ 
								*puFrameSize = (u32) pucSrcMem -*puFrameStartAddr;
								bIsFrameCorrect = true;
								break;
							}					
							pucSrcMem++;
						}
						return bIsFrameCorrect;
					}
				}
				//=============================================================================================
				//  MPEG4 stream file parsing: End
		
			default:
				return	false;
	}
	
	return false;		
}
