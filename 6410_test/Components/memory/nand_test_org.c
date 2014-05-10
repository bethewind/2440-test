/**************************************************************************************
* 
*	Project Name : S3C6410 Validation
*
*	Copyright 2006 by Samsung Electronics, Inc.
*	All rights reserved.
*
*	Project Description :
*		This software is only for validating functions of the S3C6410.
*		Anybody can use this software without our permission.
*  
*--------------------------------------------------------------------------------------
* 
*	File Name : nand_test.c
*  
*	File Description : This file implements the functons for Nand controller test.
*
*	Author : Heemyung.noh
*	Dept. : AP Development Team
*	Created Date : 2006/12/05
*	Version : 0.1 
* 
*	History
*	- Created(Heemyung.noh 2006/12/05)
*  
**************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "def.h"
#include "option.h"
#include "library.h"
#include "sfr6410.h"
#include "system.h"
#include "sysc.h"
#include "nand.h"
#include "timer.h"

//The all bit ecc check of 1page
#define NAND_SLCECC_ALLBIT_CHECK	(TRUE)
#define NAND_ECC_MULTI_ERROR			(0xFFFFFFFF)
#define NAND_ECC_UNCORRECT_ERROR		(0xFFFFFFFF)

extern NAND_oInform	NAND_Inform[NAND_CONNUM];
extern NAND_oEccError NAND_EccError;
u32 g_NandContNum;

static u32 aNANDT_EccError[4][2];		// 4bit error, byte pos & bit pos

//////////
// Function Name : NANDT_PrintErrorType
// Function Description : Print NAND Error Type
// Input : 	eError - Nand Error enumeration variable
// Output :    	None
void NANDT_PrintErrorType(NAND_eERROR eError)
{
	u32 uEccErr0, uEccErr1, uEccErr2;
	
	if(eError & eNAND_1bitEccError)
	{
		if(NAND_Inform[g_NandContNum].uAllBitEccCheck == 0)
			UART_Printf("NAND Error : Main 1 bit ECC Error\n");
		
		NAND_SeekECCErrorPosition(g_NandContNum, &uEccErr0, &uEccErr1, &uEccErr2);

		if(NAND_Inform[g_NandContNum].uNandType == NAND_MLC8bit)
		{
			if(NAND_Inform[g_NandContNum].uAllBitEccCheck == 1)
			{
				aNANDT_EccError[0][0] = (uEccErr0&0x3FF)+(NAND_Inform[g_NandContNum].uMLCECCPageReadSector*NAND_MLC_TRANS_SIZE)
														+ (NAND_Inform[g_NandContNum].uMLCECCPageReadSector*NAND_MLC_ECC_SIZE);
				aNANDT_EccError[0][1] = GetBitPosition(uEccErr2&0xFF);
			}
			else
			{
				UART_Printf("1st Byte Position : %d\n", (uEccErr0&0x3FF));
				UART_Printf("1st Bit Pattern : 0x%02x\n", (uEccErr2&0xFF));
			}
		}
		else
		{
			if(NAND_Inform[g_NandContNum].uAllBitEccCheck == 1)
			{
				aNANDT_EccError[0][0] = (uEccErr0&(0x7FF<<7))>>7;
				aNANDT_EccError[0][1] = (uEccErr0&(0x7<<4))>>4;
			}
			else
			{
				UART_Printf("1st Byte Position : %d\n", (uEccErr0&(0x7FF<<7))>>7);
				UART_Printf("1st Bit Position : %d\n", (uEccErr0&(0x7<<4))>>4);
			}
		}
	}
	if(eError & eNAND_2bitEccError)
	{
		if(NAND_Inform[g_NandContNum].uAllBitEccCheck == 0)
			UART_Printf("NAND Error : Main 2 bit ECC Error\n");
		
		NAND_SeekECCErrorPosition(g_NandContNum, &uEccErr0, &uEccErr1, &uEccErr2);

		if(NAND_Inform[g_NandContNum].uNandType == NAND_MLC8bit)
		{
			if(NAND_Inform[g_NandContNum].uAllBitEccCheck == 1)
			{
				aNANDT_EccError[1][0] = (uEccErr0&0x3FF)+(NAND_Inform[g_NandContNum].uMLCECCPageReadSector*NAND_MLC_TRANS_SIZE)
														+ (NAND_Inform[g_NandContNum].uMLCECCPageReadSector*NAND_MLC_ECC_SIZE);
				aNANDT_EccError[1][1] = GetBitPosition(uEccErr2&0xFF);
				aNANDT_EccError[0][0] = ((uEccErr0&(0x3FF<<16))>>16)+(NAND_Inform[g_NandContNum].uMLCECCPageReadSector*NAND_MLC_TRANS_SIZE)
														+ (NAND_Inform[g_NandContNum].uMLCECCPageReadSector*NAND_MLC_ECC_SIZE);
				aNANDT_EccError[0][1] = GetBitPosition((uEccErr2&(0xFF<<8))>>8);
			}
			else
			{
				UART_Printf("1st Byte Position : %d\n", (uEccErr0&0x3FF));
				UART_Printf("1st Bit Pattern : 0x%02x\n", (uEccErr2&0xFF));
				UART_Printf("2nd Byte Position : %d\n", (uEccErr0&(0x3FF<<16))>>16);
				UART_Printf("2nd Bit Pattern : 0x%02x\n", (uEccErr2&(0xFF<<8))>>8);			
			}
		}
	}	
	if(eError & eNAND_3bitEccError)
	{
		if(NAND_Inform[g_NandContNum].uAllBitEccCheck == 0)
			UART_Printf("NAND Error : Main 3 bit ECC Error\n");
		NAND_SeekECCErrorPosition(g_NandContNum, &uEccErr0, &uEccErr1, &uEccErr2);

		if(NAND_Inform[g_NandContNum].uNandType == NAND_MLC8bit)
		{
			if(NAND_Inform[g_NandContNum].uAllBitEccCheck == 1)
			{
				aNANDT_EccError[2][0] = (uEccErr0&0x3FF)+(NAND_Inform[g_NandContNum].uMLCECCPageReadSector*NAND_MLC_TRANS_SIZE)
														+ (NAND_Inform[g_NandContNum].uMLCECCPageReadSector*NAND_MLC_ECC_SIZE);
				aNANDT_EccError[2][1] = GetBitPosition(uEccErr2&0xFF);
				aNANDT_EccError[1][0] = ((uEccErr0&(0x3FF<<16))>>16)+(NAND_Inform[g_NandContNum].uMLCECCPageReadSector*NAND_MLC_TRANS_SIZE)
														+ (NAND_Inform[g_NandContNum].uMLCECCPageReadSector*NAND_MLC_ECC_SIZE);
				aNANDT_EccError[1][1] = GetBitPosition(((uEccErr2&(0xFF<<8))>>8));
				aNANDT_EccError[0][0] = (uEccErr1&0x3FF)+(NAND_Inform[g_NandContNum].uMLCECCPageReadSector*NAND_MLC_TRANS_SIZE)
														+ (NAND_Inform[g_NandContNum].uMLCECCPageReadSector*NAND_MLC_ECC_SIZE);
				aNANDT_EccError[0][1] = GetBitPosition((uEccErr2&(0xFF<<16))>>16);
			}
			else
			{
				UART_Printf("1st Byte Position : %d\n", (uEccErr0&0x3FF));
				UART_Printf("1st Bit Pattern : 0x%02x\n", (uEccErr2&0xFF));
				UART_Printf("2nd Byte Position : %d\n", (uEccErr0&(0x3FF<<16))>>16);
				UART_Printf("2nd Bit Pattern : 0x%02x\n", (uEccErr2&(0xFF<<8))>>8);		
				UART_Printf("3rd Byte Position : %d\n", (uEccErr1&0x3FF));
				UART_Printf("3rd Bit Pattern : 0x%02x\n", (uEccErr2&(0xFF<<16))>>16);		
			}
		}
	}	
	if(eError & eNAND_4bitEccError)
	{
		if(NAND_Inform[g_NandContNum].uAllBitEccCheck == 0)
			UART_Printf("NAND Error : Main 4 bit ECC Error\n");
		NAND_SeekECCErrorPosition(g_NandContNum, &uEccErr0, &uEccErr1, &uEccErr2);

		if(NAND_Inform[g_NandContNum].uNandType == NAND_MLC8bit)
		{
			if(NAND_Inform[g_NandContNum].uAllBitEccCheck == 1)
			{
				aNANDT_EccError[3][0] = (uEccErr0&0x3FF)+(NAND_Inform[g_NandContNum].uMLCECCPageReadSector*NAND_MLC_TRANS_SIZE)
														+ (NAND_Inform[g_NandContNum].uMLCECCPageReadSector*NAND_MLC_ECC_SIZE);
				aNANDT_EccError[3][1] = GetBitPosition(uEccErr2&0xFF);
				aNANDT_EccError[2][0] = ((uEccErr0&(0x3FF<<16))>>16)+(NAND_Inform[g_NandContNum].uMLCECCPageReadSector*NAND_MLC_TRANS_SIZE)
														+ (NAND_Inform[g_NandContNum].uMLCECCPageReadSector*NAND_MLC_ECC_SIZE);
				aNANDT_EccError[2][1] = GetBitPosition(((uEccErr2&(0xFF<<8))>>8));
				aNANDT_EccError[1][0] = (uEccErr1&0x3FF)+(NAND_Inform[g_NandContNum].uMLCECCPageReadSector*NAND_MLC_TRANS_SIZE)
														+ (NAND_Inform[g_NandContNum].uMLCECCPageReadSector*NAND_MLC_ECC_SIZE);
				aNANDT_EccError[1][1] = GetBitPosition((uEccErr2&(0xFF<<16))>>16);
				aNANDT_EccError[0][0] = ((uEccErr1&(0x3FF<<16))>>16)+(NAND_Inform[g_NandContNum].uMLCECCPageReadSector*NAND_MLC_TRANS_SIZE)
														+ (NAND_Inform[g_NandContNum].uMLCECCPageReadSector*NAND_MLC_ECC_SIZE);
				aNANDT_EccError[0][1] = GetBitPosition((uEccErr2&(u32)(0xFF<<24))>>24);
			}
			else
			{		
				UART_Printf("1st Byte Position : %d\n", (uEccErr0&0x3FF));
				UART_Printf("1st Bit Pattern : 0x%02x\n", (uEccErr2&0xFF));
				UART_Printf("2nd Byte Position : %d\n", (uEccErr0&(0x3FF<<16))>>16);
				UART_Printf("2nd Bit Pattern : 0x%02x\n", (uEccErr2&(0xFF<<8))>>8);		
				UART_Printf("3rd Byte Position : %d\n", (uEccErr1&0x3FF));
				UART_Printf("3rd Bit Pattern : 0x%02x\n", (uEccErr2&(0xFF<<16))>>16);	
				UART_Printf("4th Byte Position : %d\n", (uEccErr1&(0x3FF<<16))>>16);
				UART_Printf("4th Bit Pattern : 0x%02x\n", (uEccErr2&(u32)(0xFF<<24))>>24);
			}
		}
	}	
	if(eError & eNAND_MultiError)
	{
		if(NAND_Inform[g_NandContNum].uAllBitEccCheck == 1)
			aNANDT_EccError[0][0] = NAND_ECC_MULTI_ERROR;
		else
			UART_Printf("NAND Error : Main Multiple ECC Error\n");
	}
	if(eError & eNAND_EccAreaError)
		UART_Printf("NAND Error : Main ECC Area Error\n");
	if(eError & eNAND_UncorrectableError)
	{
		if(NAND_Inform[g_NandContNum].uAllBitEccCheck == 1)
			aNANDT_EccError[0][0] = NAND_ECC_UNCORRECT_ERROR;
		else	
			UART_Printf("NAND Error : Main Uncorrectable ECC Error\n");
	}
	if(eError & eNAND_EtcError)
		UART_Printf("NAND Error : Etc Error\n");
	if(eError & eNAND_Spare1bitEccError)
	{
		if(NAND_Inform[g_NandContNum].uAllBitEccCheck == 0)
			UART_Printf("NAND Error : Spare 1 bit ECC Error\n");

		NAND_SeekECCErrorPosition(g_NandContNum, &uEccErr0, &uEccErr1, &uEccErr2);

		if((NAND_Inform[g_NandContNum].uNandType == NAND_Normal8bit) || (NAND_Inform[g_NandContNum].uNandType == NAND_Advanced8bit))
		{
			if(NAND_Inform[g_NandContNum].uAllBitEccCheck == 1)
			{
				aNANDT_EccError[0][0] = (uEccErr0&(0xF<<21))>>21;
				aNANDT_EccError[0][1] = (uEccErr0&(0x7<<18))>>18;
			}
			else
			{
				UART_Printf("1st Byte Position : %d\n", (uEccErr0&(0xF<<21))>>21);
				UART_Printf("1st Bit Position : %d\n", (uEccErr0&(0x7<<18))>>18);
			}	
		}
	}
	
	if(eError & eNAND_SpareMultiError)
	{
		if(NAND_Inform[g_NandContNum].uAllBitEccCheck == 1)
			aNANDT_EccError[0][0] = NAND_ECC_MULTI_ERROR;
		else
			UART_Printf("NAND Error : Spare Multiple ECC Error\n");
	}
	
	if(eError & eNAND_SpareEccAreaError)
		UART_Printf("NAND Error : Spare ECC Area Error\n");
	if(eError & eNAND_ProgramError)
		UART_Printf("NAND Error : Program Error\n");
	if(eError & eNAND_EraseError)
		UART_Printf("NAND Error : Erase Error\n");
	if(eError & eNAND_InvalidBlock)
		UART_Printf("NAND Error : Invalid Block\n");
}


//////////
// Function Name : NANDT_ReadID
// Function Description : Print NAND Memory Device ID
// Input : 	None
// Output : 	None
void NANDT_ReadID(void)
{
	u32 usId;
	u8 ucMaker, ucDevice, ucThirdByte, ucFourthByte ;

	UART_Printf("[NANDT_ReadID]\n");

	usId = NAND_ReadID(g_NandContNum);
	
	ucMaker = (u8)usId;
	ucDevice = (u8)(usId>>8);
	ucThirdByte = (u8)(usId>>16);
	ucFourthByte = (u8)(usId>>24);
	
	UART_Printf("Maker:0x%02x, Device:0x%02x\n", ucMaker, ucDevice);
	UART_Printf("ucThirdByte:0x%02x, ucFourthByte:0x%02x\n\n", ucThirdByte, ucFourthByte);
}


//////////
// Function Name : NANDT_Reset
// Function Description : generate the NAND Reset
// Input : 	None
// Output : 	None
void NANDT_Reset(void)
{
	UART_Printf("[NANDT_Reset]\n");
	
	UART_Printf("Measure the RESET Operation timing\n");

	NAND_Reset(g_NandContNum);

	UART_Printf("RESET generation complete\n\n");
}


//////////
// Function Name : NANDT_CheckInvalidBlock
// Function Description : Check the Invalid Block of NAND
// Input : 	None
// Output : 	None
void NANDT_CheckInvalidBlock(void)
{
	u32 uBlock;
	NAND_eERROR eError;
	
	UART_Printf("[NANDT_CheckInvalidBlock]\n");

	for(uBlock=0 ; uBlock<NAND_Inform[g_NandContNum].uBlockNum ; uBlock++) //  Read [spre area of first page] per Block number(0~2047)
	{
		eError = NAND_CheckInvalidBlock(g_NandContNum, uBlock);   // Print bad block

		if(eError != eNAND_NoError)
		{
			NANDT_PrintErrorType(eError);
			UART_Printf("Bad Block Number : %d\n", uBlock);
		}
	}

	UART_Printf("\n");
	UART_Printf("[NAND Flash bad block check complete..!!]\n\n");
}


//////////
// Function Name : NANDT_CheckInvalidBlockforAutoTest
// Function Description : Check the Invalid Block of NAND
// Input : 	eTest - Manual test or Auto test selection
//			oAutoVar - Parameters for Auto-Test
// Output : 	None
void NANDT_CheckInvalidBlockforAutoTest(eFunction_Test eTest, oFunctionT_AutoVar oAutoVar)
{
	u32 uBlock;
	NAND_eERROR eError;
	
	for(uBlock=oAutoVar.Test_Parameter[0] ; uBlock<=oAutoVar.Test_Parameter[1] ; uBlock++) //  Read [spre area of first page] per Block number(0~2047)
	{
		eError = NAND_CheckInvalidBlock(g_NandContNum, uBlock);   // Print bad block

		if(eError != eNAND_NoError)
		{
			NANDT_PrintErrorType(eError);
			UART_Printf("Bad Block Number : %d\n", uBlock);
			UART_Getc();
		}
	}
}


//////////
// Function Name : NANDT_ReadPage
// Function Description : Read 1 page data
// Input : 	eTest - Manual test or Auto test selection
//			oAutoVar - Parameters for Auto-Test
// Output : 	None
void NANDT_ReadPage(eFunction_Test eTest, oFunctionT_AutoVar oAutoVar)
{
	u32 i, uBlock, uPage;
	//u8 aBuffer[NAND_PAGE_MAX];
	//u8 aSpareBuffer[NAND_SPARE_MAX];
	u8 *aBuffer, *aSpareBuffer;
	NAND_eERROR eError;

	if(eTest == eTest_Manual)
		UART_Printf("[NANDT_ReadPage]\n");

	aBuffer = (u8 *)malloc(NAND_PAGE_MAX);
	if(aBuffer == 0)
	{
		UART_Printf("Memory Allocation Error...\n");
		UART_Getc();
		return;
	}	
	aSpareBuffer = (u8 *)malloc(NAND_SPARE_MAX);
	if(aSpareBuffer == 0)
	{
		UART_Printf("Memory Allocation Error...\n");
		free(aBuffer);
		UART_Getc();
		return;
	}
	
	for(i=0 ; i<NAND_Inform[g_NandContNum].uPageSize ; i++)
		aBuffer[i] = 0;
	for(i=0 ; i<NAND_Inform[g_NandContNum].uSpareSize ; i++)
		aSpareBuffer[i] = 0xFF;
	
	if(eTest == eTest_Manual)
	{
		UART_Printf("Input the Block Number to read[0~%d]", NAND_Inform[g_NandContNum].uBlockNum-1);
		uBlock = UART_GetIntNum();
		UART_Printf("Input the Page Number to read[0~%d]", NAND_Inform[g_NandContNum].uPageNum-1);
		uPage = UART_GetIntNum();
		UART_Printf("\n");
	}
	else
	{
		uBlock = oAutoVar.Test_Parameter[0];
		uPage = oAutoVar.Test_Parameter[1];
	} 
	
	eError = NAND_ReadPage(g_NandContNum, uBlock, uPage, aBuffer, aSpareBuffer);

	if(eError != eNAND_NoError)
	{
		NANDT_PrintErrorType(eError);
		UART_Printf("Read page Error [%d block  %d page]\n", uBlock, uPage);
		UART_Getc();
	}
	else
	{
		UART_Printf("[Main Area]");
		for(i=0 ; i<NAND_Inform[g_NandContNum].uPageSize ; i++)
		{
			if(i%16==0)
	    			UART_Printf("\n%04xh:",i);
			
        		UART_Printf("%02x ", aBuffer[i]);
		}
		UART_Printf("\n");

		UART_Printf("[Spare Area]");
		for(i=0 ; i<NAND_Inform[g_NandContNum].uSpareSize ; i++)
		{
			if(i%16==0)
	    			UART_Printf("\n%04xh:",i);
			
        		UART_Printf("%02x ", aSpareBuffer[i]);
		}
		UART_Printf("\n\n");
		
		UART_Printf("NAND Read Page : Success\n");
	}
	UART_Printf("\n");

	free(aBuffer);
	free(aSpareBuffer);	
}


//////////
// Function Name : NANDT_WritePage
// Function Description : Write 1 page data
// Input : 	eTest - Manual test or Auto test selection
//			oAutoVar - Parameters for Auto-Test
// Output : 	None
void NANDT_WritePage(eFunction_Test eTest, oFunctionT_AutoVar oAutoVar)
{
	u32 i, uBlock, uPage;
	u8 uOffset;
	//u8 aBuffer[NAND_PAGE_MAX],  aSpareBuffer[NAND_SPARE_MAX];	
	u8 *aBuffer, *aSpareBuffer;
	NAND_eERROR eError;

	if(eTest == eTest_Manual)
		UART_Printf("[NANDT_WritePage]\n");

	if(eTest == eTest_Manual)
	{
		UART_Printf("Input the Block Number to write[0~%d] : ", NAND_Inform[g_NandContNum].uBlockNum-1);
		uBlock = UART_GetIntNum();
		UART_Printf("Input the Page Number to write[0~%d] : ", NAND_Inform[g_NandContNum].uPageNum-1);
		uPage = UART_GetIntNum();
		UART_Printf("Input the srand() offset data : ");
		uOffset = (u8)UART_GetIntNum();
		UART_Printf("\n");
	}
	else
	{
		uBlock = oAutoVar.Test_Parameter[0];
		uPage = oAutoVar.Test_Parameter[1];
		uOffset = oAutoVar.Test_Parameter[2];
	} 
	
	aBuffer = (u8 *)malloc(NAND_PAGE_MAX);
	if(aBuffer == 0)
	{
		UART_Printf("Memory Allocation Error...\n");
		UART_Getc();
		return;
	}	
	aSpareBuffer = (u8 *)malloc(NAND_SPARE_MAX);
	if(aSpareBuffer == 0)
	{
		UART_Printf("Memory Allocation Error...\n");
		free(aBuffer);
		UART_Getc();
		return;
	}

#if 1
	srand(uOffset);
	for(i=0 ; i<NAND_Inform[g_NandContNum].uPageSize ; i++)
	{
		aBuffer[i] = rand()%0xFF;
	}
#else
	for(i=0 ; i<NAND_Inform[g_NandContNum].uPageSize ; i++)
	{
		aBuffer[i] = i;
	}
#endif

	for(i=0 ; i<NAND_Inform[g_NandContNum].uSpareSize ; i++)
		aSpareBuffer[i] = 0xFF;	
	
	eError = NAND_WritePage(g_NandContNum, uBlock, uPage, aBuffer, aSpareBuffer);

	if(eError != eNAND_NoError)
	{
		NANDT_PrintErrorType(eError);
		UART_Getc();
	}
	else
	{
		UART_Printf("[Main Area]");
		for(i=0 ; i<NAND_Inform[g_NandContNum].uPageSize ; i++)
		{
			if(i%16==0)
	    			UART_Printf("\n%04xh:",i);
			
        		UART_Printf("%02x ", aBuffer[i]);
		}
		UART_Printf("\n");
		
		UART_Printf("[Spare Area]");
		for(i=0 ; i<NAND_Inform[g_NandContNum].uSpareSize ; i++)
		{
			if(i%16==0)
	    			UART_Printf("\n%04xh:",i);
			
        		UART_Printf("%02x ", aSpareBuffer[i]);
		}
		UART_Printf("\n\n");
		
		UART_Printf("NAND Write Page : Success\n");
	}
	UART_Printf("\n");
	
	free(aBuffer);
	free(aSpareBuffer);
}


//////////
// Function Name : NANDT_WritePageVerify
// Function Description : Write 1 page data & Verfy
// Input : 	eTest - Manual test or Auto test selection
//			oAutoVar - Parameters for Auto-Test
// Output : 	None
void NANDT_WritePageVerify(eFunction_Test eTest, oFunctionT_AutoVar oAutoVar)
{
	u32 i, uBlock, uPage;
	u8 uOffset;
	//u8 aBuffer[NAND_PAGE_MAX],  aSpareBuffer[NAND_SPARE_MAX];	
	u8 *aBuffer, *aSpareBuffer;
	u8 *pReadBuffer, *pReadSpareBuffer;
	NAND_eERROR eError;
	u8 bError = FALSE;
	
	if(eTest == eTest_Manual)
		UART_Printf("[NANDT_WritePage]\n");

	if(eTest == eTest_Manual)
	{
		UART_Printf("Input the Block Number to write[0~%d] : ", NAND_Inform[g_NandContNum].uBlockNum-1);
		uBlock = UART_GetIntNum();
		UART_Printf("Input the Page Number to write[0~%d] : ", NAND_Inform[g_NandContNum].uPageNum-1);
		uPage = UART_GetIntNum();
		UART_Printf("Input the srand() offset data : ");
		uOffset = (u8)UART_GetIntNum();
		UART_Printf("\n");
	}
	else
	{
		uBlock = oAutoVar.Test_Parameter[0];
		uPage = oAutoVar.Test_Parameter[1];
		uOffset = oAutoVar.Test_Parameter[2];
	} 
	
	aBuffer = (u8 *)malloc(NAND_PAGE_MAX);
	if((aBuffer == 0))
	{
		UART_Printf("Memory Allocation Error...\n");
		UART_Getc();
		return;
	}
	
	aSpareBuffer = (u8 *)malloc(NAND_SPARE_MAX);
	if((aSpareBuffer == 0))
	{
		UART_Printf("Memory Allocation Error...\n");
		free(aBuffer);
		UART_Getc();
		return;
	}
	
	pReadBuffer = (u8 *)malloc(NAND_PAGE_MAX);
	if((pReadBuffer == 0))
	{
		UART_Printf("Memory Allocation Error...\n");
		free(aBuffer);
		free(aSpareBuffer);
		UART_Getc();
		return;
	}
	
	pReadSpareBuffer = (u8 *)malloc(NAND_SPARE_MAX);
	if((pReadSpareBuffer == 0))
	{
		UART_Printf("Memory Allocation Error...\n");
		free(aBuffer);
		free(aSpareBuffer);
		free(pReadBuffer);
		UART_Getc();
		return;
	}
	
#if 1
	srand(uOffset);
	for(i=0 ; i<NAND_Inform[g_NandContNum].uPageSize ; i++)
	{
		aBuffer[i] = rand()%0xFF;
	}
#else
	for(i=0 ; i<NAND_Inform[g_NandContNum].uPageSize ; i++)
	{
		aBuffer[i] = i;
	}
#endif

	for(i=0 ; i<NAND_Inform[g_NandContNum].uSpareSize ; i++)
		aSpareBuffer[i] = 0xFF;	
	
	eError = NAND_WritePage(g_NandContNum, uBlock, uPage, aBuffer, aSpareBuffer);

	if(eError != eNAND_NoError)
	{
		NANDT_PrintErrorType(eError);
		UART_Getc();
		free(aBuffer);
		free(aSpareBuffer);
		free(pReadBuffer);
		free(pReadSpareBuffer);
		return;
	}

	// Read Buffer Clear
	for(i=0 ; i<NAND_Inform[g_NandContNum].uPageSize ; i++)
	{
		pReadBuffer[i] = 0;
	}
	
	eError = NAND_ReadPage(g_NandContNum, uBlock, uPage, pReadBuffer, pReadSpareBuffer);

	if(eError != eNAND_NoError)
	{
		NANDT_PrintErrorType(eError);
		UART_Printf("Read page Error [%d block  %d page]\n", uBlock, uPage);
		UART_Getc();
		free(aBuffer);
		free(aSpareBuffer);
		free(pReadBuffer);
		free(pReadSpareBuffer);
		return;
	}
	else
	{	
		for(i=0 ; i<NAND_Inform[g_NandContNum].uPageSize ; i++)
		{
			if(pReadBuffer[i] != aBuffer[i])
			{
				UART_Printf("Write&Read Verify Error [%d Block : %d Page] :  \n", uBlock, uPage);
				UART_Printf("\t [Write data : Read data] = [0x%08x  : 0x%08x ]  \n", aBuffer[i], pReadBuffer[i]);
				UART_Getc();
				bError = TRUE;
				break;				
			}
		}
	}

	if(eTest == eTest_Manual)
	{
		if(bError == FALSE)
			UART_Printf("Write&Read Verify OK\n");
		UART_Printf("\n");
	}
	else
	{
		UART_Printf(".");
	}
	
	free(aBuffer);
	free(aSpareBuffer);
	free(pReadBuffer);
	free(pReadSpareBuffer);
}


const AutotestFuncMenu nand_ReadWrite_menu[] =
{
	NANDT_WritePage,					"Write Single Page       ",
	NANDT_ReadPage,                			"Read Single Page       ",	
	NANDT_WritePageVerify,				"Write & Verify Single Page",
	0, 0
};


void NANDT_ReadWrite(void)
{
	u32 i;
	s32 uSel;
	oFunctionT_AutoVar oAutoTest;

	UART_Printf("\n[NANDT_ReadWrite]\n");
	
	while(1)
	{
		UART_Printf("\n");
		for (i=0; (u32)(nand_ReadWrite_menu[i].desc)!=0; i++)
			UART_Printf("%2d: %s\n", i, nand_ReadWrite_menu[i].desc);

		UART_Printf("\nSelect the function to test : ");
		uSel =UART_GetIntNum();
		UART_Printf("\n");
		if(uSel == -1) 
			break;

		if (uSel>=0 && uSel<(sizeof(nand_ReadWrite_menu)/8-1))
			(nand_ReadWrite_menu[uSel].func) (eTest_Manual, oAutoTest);
	}
}



//////////
// Function Name : NANDT_EraseSingleBlock
// Function Description : Erase 1 block
// Input : 	eTest - Manual test or Auto test selection
//			oAutoVar - Parameters for Auto-Test
// Output : 	None
void NANDT_EraseSingleBlock(eFunction_Test eTest, oFunctionT_AutoVar oAutoVar)
{
	u32 uBlock;
	NAND_eERROR eError;
	
	if(eTest == eTest_Manual)
		UART_Printf("[NANDT_EraseSingleBlock]\n");

	if(eTest == eTest_Manual)
	{
		UART_Printf("Input the Block Number to erase[0~%d]", NAND_Inform[g_NandContNum].uBlockNum-1);
		uBlock = UART_GetIntNum();
	}
	else
		uBlock = oAutoVar.Test_Parameter[0];
	
	eError = NAND_EraseSingleBlock(g_NandContNum, uBlock);

	if(eError != eNAND_NoError)
	{
		NANDT_PrintErrorType(eError);
		UART_Getc();
	}
	else
	{
		if(eTest == eTest_Manual)
			UART_Printf("NAND Erase Block : Success\n\n");	
	}
}


//////////
// Function Name : NANDT_EraseMultiBlock
// Function Description : Erase Multi block
// Input : 	eTest - Manual test or Auto test selection
//			oAutoVar - Parameters for Auto-Test
// Output : 	None
void NANDT_EraseMultiBlock(eFunction_Test eTest, oFunctionT_AutoVar oAutoVar)
{
	u32 uBlock, uBlockNum;
	NAND_eERROR eError;

	if(eTest == eTest_Manual)
	{
		UART_Printf("[NANDT_EraseMultiBlock]\n");
	}
	
	if(NAND_Inform[g_NandContNum].uNandType == NAND_Normal8bit)	
	{
		if(eTest == eTest_Manual)
		{
			UART_Printf("Input the Start Block Number to erase the multi-block[0~%d]", NAND_Inform[g_NandContNum].uBlockNum-1);
			uBlock = UART_GetIntNum();		
			UART_Printf("Input the Block Number to erase[multiples of 4 : 0~%d]", NAND_Inform[g_NandContNum].uBlockNum-uBlock-1);
			uBlockNum = UART_GetIntNum();
		}	
		else
		{
			uBlock = oAutoVar.Test_Parameter[0];
			uBlockNum = oAutoVar.Test_Parameter[1];
		}
		//make the multiples of 4
		if(uBlockNum%4)
			uBlockNum += (4-(uBlockNum%4));		
	}
	else
	{
		if(eTest == eTest_Manual)
		{
			UART_Printf("Input the Start Block Number to erase the multi-block[multiples of 2 : 0~%d]", NAND_Inform[g_NandContNum].uBlockNum-1);
			uBlock = UART_GetIntNum();
			if(uBlock%2)		//uBlock is must be multiples of 2
				uBlock++;		
			UART_Printf("Input the Block Number to erase[multiples of 2 : 2~%d]", NAND_Inform[g_NandContNum].uBlockNum-uBlock);
			uBlockNum = UART_GetIntNum();
		}	
		else
		{
			uBlock = oAutoVar.Test_Parameter[0];
			uBlockNum = oAutoVar.Test_Parameter[1];
		}
		//make the multiples of 2
		if(uBlockNum%2)
			uBlockNum += (2-(uBlockNum%2));		
	}		
	
	eError = NAND_EraseMultiBlock(g_NandContNum, uBlock, uBlockNum);

	if(eError != eNAND_NoError)
	{
		NANDT_PrintErrorType(eError);
		UART_Getc();
	}
	else
	{
		if(eTest == eTest_Manual)
			UART_Printf("NAND Erase Block[%d ~ %d Block] : Success\n\n", uBlock, uBlock+uBlockNum-1);
	}
}


const AutotestFuncMenu nand_erase_menu[] =
{
	NANDT_EraseSingleBlock,					"Single Block Erase       ",
	NANDT_EraseMultiBlock,                				"Multi Block Erase        ",		
	0, 0
};


void NANDT_Erase(void)
{
	u32 i;
	s32 uSel;
	oFunctionT_AutoVar oAutoTest;

	UART_Printf("\n[NANDT_Erase]\n");
	
	while(1)
	{
		UART_Printf("\n");
		for (i=0; (u32)(nand_erase_menu[i].desc)!=0; i++)
			UART_Printf("%2d: %s\n", i, nand_erase_menu[i].desc);

		UART_Printf("\nSelect the function to test : ");
		uSel =UART_GetIntNum();
		UART_Printf("\n");
		if(uSel == -1) 
			break;

		if (uSel>=0 && uSel<(sizeof(nand_erase_menu)/8-1))
			(nand_erase_menu[uSel].func) (eTest_Manual, oAutoTest);
	}
}


//////////
// Function Name : NANDT_ProgramBinary
// Function Description : Write the Binary file to NAND Memory(ex. Boot Code)
// Input : 	None
// Output : 	None
void NANDT_ProgramBinary(void)
{
	u32 j, uBlock, uPage, uSize;
	u8 *uSourceAddr;
	u32 uWriteBlock, uWritePage, uPageNumToWrite, uRemainedPage;
	u8 bWriteEnd;
	u8 aSpareBuffer[NAND_SPARE_MAX];	
	NAND_eERROR eError;
	
	UART_Printf("[NANDT_ProgramBinary]\n\n");

	uSourceAddr = (u8 *)(_DRAM_BaseAddress+0x00a00000);
	
	UART_Printf("Caution : You must put BINARY file into 0x%08x before programming\n",uSourceAddr);

	UART_Printf("Input the Block Number to write[0~%d]", NAND_Inform[g_NandContNum].uBlockNum-1);
	uBlock = UART_GetIntNum();
	UART_Printf("Input the Page Number to write[0~%d]", NAND_Inform[g_NandContNum].uPageNum-1);
	uPage = UART_GetIntNum();
	UART_Printf("Input the Size to program[Bytes] : ");
	uSize = UART_GetIntNum();

	uPageNumToWrite = uSize/NAND_Inform[g_NandContNum].uPageSize;
	if(uSize%NAND_Inform[g_NandContNum].uPageSize)
		uPageNumToWrite++;	
	uRemainedPage = uPageNumToWrite;
	uWriteBlock = uBlock;
	uWritePage = uPage;
	bWriteEnd = FALSE;
	
	while(1)
	{
		// Invalid Block Check
		eError = NAND_CheckInvalidBlock(g_NandContNum, uWriteBlock);
		while(eError != eNAND_NoError)
		{
			NANDT_PrintErrorType(eError);
			UART_Printf("Block Number: %d\n", uWriteBlock);
			uWriteBlock++;   // for next block
			eError = NAND_CheckInvalidBlock(g_NandContNum, uWriteBlock);
		}

		// Erase Block
		eError = NAND_EraseSingleBlock(g_NandContNum, uWriteBlock);
		while(eError != eNAND_NoError)
		{
			NANDT_PrintErrorType(eError);
			UART_Printf("Block Number: %d\n", uWriteBlock);
			uWriteBlock++;   // for next block
			eError = NAND_CheckInvalidBlock(g_NandContNum, uWriteBlock);
		}		

		// Write Block
		for(j=uWritePage ; j<NAND_Inform[g_NandContNum].uPageNum ; j++)
		{
			eError = NAND_WritePage(g_NandContNum, uWriteBlock, j, uSourceAddr, aSpareBuffer);

			if(eError != eNAND_NoError)
			{
				NANDT_PrintErrorType(eError);
				break;
			}
			
			uRemainedPage--;
			if(uRemainedPage == 0)
			{
				bWriteEnd = TRUE;
				break;
			}
			uSourceAddr += NAND_Inform[g_NandContNum].uPageSize;
		}

		if(bWriteEnd == TRUE)
			break;
		
		uWritePage = 0;
		uWriteBlock++;
		UART_Printf(".");
	}

	UART_Printf("\n");
	UART_Printf("NAND Program Complete\n\n");
}


//////////
// Function Name : NANDT_SoftLock
// Function Description : Soft-lock the NAND Memory
// Input : 	None
// Output : 	None
void NANDT_SoftLock(void)
{
	u32 uStartBlock, uEndBlock;
	NAND_eERROR eError;
	
	UART_Printf("[NANDT_SoftLock]\n\n");

	UART_Printf("Input the Start Block Number to soft-lock[0~%d]", NAND_Inform[g_NandContNum].uBlockNum-1);
	uStartBlock = UART_GetIntNum();
	UART_Printf("Input the End Block Number to soft-lock[%d~%d]", uStartBlock, NAND_Inform[g_NandContNum].uBlockNum-1);
	uEndBlock = UART_GetIntNum();

	eError = NAND_SoftLockingBlock(g_NandContNum, uStartBlock, uEndBlock);

	if(eError != eNAND_NoError)
	{
		UART_Printf("NAND Soft-Lock Block Error.....!!\n\n");
		UART_Getc();
	}
	else
		UART_Printf("NAND Soft-Lock Block Complete[except %d ~ %d Block]\n\n", uStartBlock, uEndBlock);
}


//////////
// Function Name : NANDT_LockTight
// Function Description : Lock-tight the NAND Memory
// Input : 	None
// Output : 	None
void NANDT_LockTight(void)
{
	u32 uStartBlock, uEndBlock;
	NAND_eERROR eError;
	
	UART_Printf("[NANDT_LockTight]\n\n");

	UART_Printf("Input the Start Block Number to lock-tight[0~%d]", NAND_Inform[g_NandContNum].uBlockNum-1);
	uStartBlock = UART_GetIntNum();
	UART_Printf("Input the End Block Number to lock-tight[%d~%d]", uStartBlock, NAND_Inform[g_NandContNum].uBlockNum-1);
	uEndBlock = UART_GetIntNum();

	eError = NAND_LockTightBlock(g_NandContNum, uStartBlock, uEndBlock);

	if(eError != eNAND_NoError)
	{
		UART_Printf("NAND Lock-tight Block Error.....!!\n\n");
		UART_Getc();
	}
	else
		UART_Printf("NAND Lock-tight Block Complete[except %d ~ %d Block]\n\n", uStartBlock, uEndBlock);
}


//////////
// Function Name : NANDT_DisableSoftLock
// Function Description : Disable Soft-lock of the NAND Memory
// Input : 	None
// Output : 	None
void NANDT_DisableSoftLock(void)
{
	NAND_eERROR eError;
	
	UART_Printf("[NANDT_DisableSoftLock]\n\n");

	eError = NAND_UnLockBlock(g_NandContNum);

	if(eError != eNAND_NoError)
	{
		UART_Printf("UnLock the Soft-locked Block Error.....!!\n\n");
		UART_Getc();
	}
	else
		UART_Printf("UnLock the Soft-locked Block Complete\n\n");
}


//////////
// Function Name : NANDT_DisableLockTight
// Function Description : Disable Lock-tight of the NAND Memory
// Input : 	None
// Output : 	None
void NANDT_DisableLockTight(void)
{
	NAND_eERROR eError;
	
	UART_Printf("[NANDT_DisableLockTight]\n\n");

	eError = NAND_UnLockTightBlock(g_NandContNum);

	if(eError != eNAND_NoError)
	{
		UART_Printf("Fail - cleared Lock-tight bit by software\n\n");
		UART_Getc();
	}
	else
		UART_Printf("Success - not cleared Lock-tight bit by software\n\n");
}


const testFuncMenu nandlock_menu[] =
{
	NANDT_SoftLock,					"Soft Lock        ",
	NANDT_LockTight,					"Lock Tight       ",
	NANDT_DisableSoftLock,				"Soft Unlock      ",
	NANDT_DisableLockTight,				"Lock-tight Unlock     ",	
	0, 0
};


void NANDT_LockUnlock(void)
{
	u32 i;
	s32 uSel;
	
	while(1)
	{
		for (i=0; (u32)(nandlock_menu[i].desc)!=0; i++)
			UART_Printf("%2d: %s\n", i, nandlock_menu[i].desc);

		UART_Printf("\nSelect the function to test : ");
		uSel =UART_GetIntNum();
		UART_Printf("\n");
		if(uSel == -1) 
			break;

		if (uSel>=0 && uSel<(sizeof(nandlock_menu)/8-1))
			(nandlock_menu[uSel].func) ();
	}
}


//////////
// Function Name : NANDT_SLCECC
// Function Description : SLC ECC function test
// Input : 	None
// Output : 	None
void NANDT_SLCECC(void)
{
	u32 i, uBlock, uPage, uError, uErrorType;
	u8 aBuffer[NAND_PAGE_MAX];
	u8 aSpareBuffer[NAND_SPARE_MAX];
	NAND_eERROR eError;
	
	UART_Printf("[NANDT_SLCECC]\n");

	if(NAND_Inform[g_NandContNum].uNandType == NAND_MLC8bit)
	{
		UART_Printf("The current Nand Memory is MLC type.....!!\n\n");
		return;
	}
	
	UART_Printf("Input the Block Number to test[0~%d]", NAND_Inform[g_NandContNum].uBlockNum-1);
	uBlock = UART_GetIntNum();
	UART_Printf("Input the Page Number to test[0~%d]", NAND_Inform[g_NandContNum].uPageNum-1);
	uPage = UART_GetIntNum();	
	
	UART_Printf("Input the ECC Error Type\n");
	UART_Printf("1: 1bit error(D),	2: multiple error\n");
	uError = UART_GetIntNum();
	
	switch(uError)
	{
		case 1 :	uErrorType = NAND_ECCERR_1BIT;
				break;
		case 2 :	uErrorType = NAND_ECCERR_MULTI;
				break;
		default :	uErrorType = NAND_ECCERR_1BIT;
				break;				
	}
	
	srand(0);
	for(i=0 ; i<NAND_Inform[g_NandContNum].uPageSize ; i++)
	{
		aBuffer[i] = rand()%0xFF;
	}
	for(i=0 ; i<NAND_Inform[g_NandContNum].uSpareSize ; i++)
		aSpareBuffer[i] = 0xFF;	

	NAND_EccError.uEccErrByte1 = 100;
	NAND_EccError.uEccErrBit1 = 0;
	NAND_EccError.uEccErrByte2 = 200;
	NAND_EccError.uEccErrBit2 = 0;

	UART_Printf("\n");
	UART_Printf("The specific bit ecc check : [%dByte : %dBit]\n", NAND_EccError.uEccErrByte1, NAND_EccError.uEccErrBit1);
	if(uErrorType == NAND_ECCERR_MULTI)
		UART_Printf("The specific bit ecc check : [%dByte : %dBit]\n", NAND_EccError.uEccErrByte2, NAND_EccError.uEccErrBit2);
	UART_Printf("\n");
	
	// Erase Block for test
	UART_Printf("Erase the block ( %d[block] )\n", uBlock);
	eError = NAND_EraseSingleBlock(g_NandContNum, uBlock);
	if(eError != eNAND_NoError)
	{
		NANDT_PrintErrorType(eError);
		UART_Getc();
		return;
	}

	NAND_Inform[g_NandContNum].uECCtest = 1;	
	NAND_Inform[g_NandContNum].uAllBitEccCheck = 0;
	
	// Write the valid data to page 0 ( uBlock[block] 0[page] ) ...for ECC generation
	UART_Printf("Write the valid data to page ( %d[block] %d[page] )\n", uBlock, uPage);
	eError = NAND_WritePage(g_NandContNum, uBlock, uPage, aBuffer, aSpareBuffer);
	if(eError != eNAND_NoError)
	{
		NANDT_PrintErrorType(eError);
		UART_Getc();
		NAND_Inform[g_NandContNum].uECCtest = 0;
		return;
	}

	// Erase Block for test
	UART_Printf("Erase the block ( %d[block] )\n", uBlock);
	eError = NAND_EraseSingleBlock(g_NandContNum, uBlock);
	if(eError != eNAND_NoError)
	{
		NANDT_PrintErrorType(eError);
		UART_Getc();
		NAND_Inform[g_NandContNum].uECCtest = 0;
		return;
	}	
	//uPage++;
	
	// Write the invalid data to page( uBlock[block] 1[page] )....with Invalid Main Data0
	UART_Printf("Write the invalid data to page ( %d[block] %d[page] )\n", uBlock, uPage);
	eError = NAND_WritePageWithInvalidData(g_NandContNum, uBlock, uPage, aBuffer, uErrorType);
	if(eError != eNAND_NoError)
	{
		NANDT_PrintErrorType(eError);
		UART_Getc();
		NAND_Inform[g_NandContNum].uECCtest = 0;
		return;
	}	
	// Read the invalid data to detect the ECC error	
	UART_Printf("Read the invalid data to page ( %d[block] %d[page] )\n", uBlock, uPage);
	eError = NAND_ReadPage(g_NandContNum, uBlock, uPage, aBuffer, aSpareBuffer);
	UART_Printf("\n");
	if(eError != eNAND_NoError)
	{
		NANDT_PrintErrorType(eError);
	}
	
	NAND_Inform[g_NandContNum].uECCtest = 0;

	UART_Printf("SLC Nand ECC test complete\n");
	UART_Printf("\n");	
}


//////////
// Function Name : NANDT_SLCSpareECC
// Function Description : SLC Spare ECC function test
// Input : 	None
// Output : 	None
void NANDT_SLCSpareECC(void)
{
	u32 i, uBlock, uPage, uError, uErrorType;
	u8 aBuffer[NAND_PAGE_MAX];
	u8 aSpareBuffer[NAND_SPARE_MAX];
	NAND_eERROR eError;
	
	UART_Printf("[NANDT_SLCSpareECC]\n");

	if(NAND_Inform[g_NandContNum].uNandType == NAND_MLC8bit)
	{
		UART_Printf("The current Nand Memory is MLC type.....!!\n\n");
		return;
	}
	
	UART_Printf("Input the Block Number to test[0~%d]", NAND_Inform[g_NandContNum].uBlockNum-1);
	uBlock = UART_GetIntNum();
	UART_Printf("Input the Page Number to test[0~%d]", NAND_Inform[g_NandContNum].uPageNum-1);
	uPage = UART_GetIntNum();	
	
	UART_Printf("Input the ECC Error Type\n");
	UART_Printf("1: 1bit error(D),	2: multiple error\n");
	uError = UART_GetIntNum();
	
	switch(uError)
	{
		case 1 :	uErrorType = NAND_ECCERR_1BIT;
				break;
		case 2 :	uErrorType = NAND_ECCERR_MULTI;
				break;
		default :	uErrorType = NAND_ECCERR_1BIT;
				break;				
	}
	
	srand(0);
	for(i=0 ; i<NAND_Inform[g_NandContNum].uPageSize ; i++)
	{
		aBuffer[i] = rand()%0xFF;
	}
	for(i=0 ; i<NAND_Inform[g_NandContNum].uSpareSize ; i++)
		aSpareBuffer[i] = 0xFF;	

	if(NAND_Inform[g_NandContNum].uNandType == NAND_Normal8bit)
	{		
		// Spare Area 0~3 byte : Main ECC Value
		// Spare Area 8~9 byte : Spare ECC Value
		NAND_EccError.uEccErrByte1 = 0;
		NAND_EccError.uEccErrBit1 = 0;
		NAND_EccError.uEccErrByte2 = (NAND_EccError.uEccErrByte1+1)%4;
		NAND_EccError.uEccErrBit2 = 0;
	}
	else if(NAND_Inform[g_NandContNum].uNandType == NAND_Advanced8bit)
	{
		// Spare Area 1~4 byte : Main ECC Value
		// Spare Area 8~9 byte : Spare ECC Value
		NAND_EccError.uEccErrByte1 = 1;
		NAND_EccError.uEccErrBit1 = 0;
		NAND_EccError.uEccErrByte2 = (NAND_EccError.uEccErrByte1%4) + 1;
		NAND_EccError.uEccErrBit2 = 0;
	}
	
	UART_Printf("\n");
	UART_Printf("The specific bit ecc check : [%dByte : %dBit]\n", NAND_EccError.uEccErrByte1, NAND_EccError.uEccErrBit1);
	if(uErrorType == NAND_ECCERR_MULTI)
		UART_Printf("The specific bit ecc check : [%dByte : %dBit]\n", NAND_EccError.uEccErrByte2, NAND_EccError.uEccErrBit2);
	UART_Printf("\n");
	
	// Erase Block for test
	UART_Printf("Erase the block ( %d[block] )\n", uBlock);
	eError = NAND_EraseSingleBlock(g_NandContNum, uBlock);
	if(eError != eNAND_NoError)
	{
		NANDT_PrintErrorType(eError);
		UART_Getc();
		return;
	}

	NAND_Inform[g_NandContNum].uECCtest = 1;	
	NAND_Inform[g_NandContNum].uAllBitEccCheck = 0;
	NAND_Inform[g_NandContNum].uSpareECCtest = 1;	
	
	// Write the valid data to page 0 ( uBlock[block] 0[page] )
	UART_Printf("Write the valid data to page ( %d[block] %d[page] )\n", uBlock, uPage);
	eError = NAND_WritePage(g_NandContNum, uBlock, uPage, aBuffer, aSpareBuffer);
	if(eError != eNAND_NoError)
	{
		NANDT_PrintErrorType(eError);
		UART_Getc();
		NAND_Inform[g_NandContNum].uECCtest = 0;
		NAND_Inform[g_NandContNum].uSpareECCtest = 0;		
		return;
	}


	UART_Printf("Erase the block ( %d[block] )\n", uBlock);
	eError = NAND_EraseSingleBlock(g_NandContNum, uBlock);
	if(eError != eNAND_NoError)
	{
		NANDT_PrintErrorType(eError);
		UART_Getc();
		NAND_Inform[g_NandContNum].uECCtest = 0;
		NAND_Inform[g_NandContNum].uSpareECCtest = 0;		
		return;
	}
	
	// Write the invalid data to page( uBlock[block] 1[page] )
	UART_Printf("Write the invalid data to page ( %d[block] %d[page] )\n", uBlock, uPage);
	eError = NAND_WritePageWithInvalidSpareData(g_NandContNum, uBlock, uPage, aBuffer, uErrorType);
	if(eError != eNAND_NoError)
	{
		NANDT_PrintErrorType(eError);
		UART_Getc();
		NAND_Inform[g_NandContNum].uECCtest = 0;
		NAND_Inform[g_NandContNum].uSpareECCtest = 0;						
		return;
	}

	
	// Read the invalid data to detect the ECC error	
	UART_Printf("Read the invalid data to page ( %d[block] %d[page] )\n", uBlock, uPage);
	eError = NAND_ReadPage(g_NandContNum, uBlock, uPage, aBuffer, aSpareBuffer);
	UART_Printf("\n");
	if(eError != eNAND_NoError)
	{
		NANDT_PrintErrorType(eError);
	}
	
	NAND_Inform[g_NandContNum].uECCtest = 0;
	NAND_Inform[g_NandContNum].uSpareECCtest = 0;	

	UART_Printf("SLC Nand ECC test complete\n");
	UART_Printf("\n");	
}



//////////
// Function Name : NANDT_MLCECC
// Function Description : MLC ECC function test
// Input : 	None
// Output : 	None
void NANDT_MLCECC(void)
{
	u32 i, m, n, uBlock, uPage, uError, uErrorType;
	u8 aBuffer[NAND_PAGE_MAX];
	u8 aSpareBuffer[NAND_SPARE_MAX];
	u8 aReadBuffer[NAND_PAGE_MAX];
	NAND_eERROR eError;
	
	UART_Printf("[NANDT_MLCECC_All]\n");

	if(NAND_Inform[g_NandContNum].uNandType != NAND_MLC8bit)
	{
		UART_Printf("The current Nand Memory is not MLC type.....!!\n\n");
		return;
	}
	
	UART_Printf("Input the Block Number to test[0~%d]", NAND_Inform[g_NandContNum].uBlockNum-1);
	uBlock = UART_GetIntNum();
	UART_Printf("Input the Page Number to test[0~%d]", NAND_Inform[g_NandContNum].uPageNum-1);
	uPage = UART_GetIntNum();	
	
	UART_Printf("Input the ECC Error Type\n");
	UART_Printf("1: 1bit error(D),	2: 2bit error,	3: 3bit error,	4: 4bit error,	5: Uncorrectable\n");
	uError = UART_GetIntNum();
	switch(uError)
	{
		case 1 :	uErrorType = NAND_ECCERR_1BIT;
				
				UART_Printf("Input the First Byte Postion[0 ~ 2075]");
				NAND_EccError.uEccErrByte1 = UART_GetIntNum()%2076;
				UART_Printf("Input the First Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit1 = UART_GetIntNum()%8;

				NAND_Inform[g_NandContNum].uMLCECCPageWriteSector = 
								NAND_EccError.uEccErrByte1/(NAND_MLC_TRANS_SIZE+NAND_MLC_ECC_SIZE);

				NAND_Inform[g_NandContNum].uMLCECCErrBytePos[0] = NAND_EccError.uEccErrByte1 - 
								(NAND_Inform[g_NandContNum].uMLCECCPageWriteSector * (NAND_MLC_TRANS_SIZE+NAND_MLC_ECC_SIZE));
					
				break;
		case 2 :	uErrorType = NAND_ECCERR_2BIT;

				UART_Printf("Input the First Byte Postion[0 ~ 2075]");
				NAND_EccError.uEccErrByte1 = UART_GetIntNum()%2076;
				UART_Printf("Input the First Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit1 = UART_GetIntNum()%8;

				UART_Printf("Input the Second Byte Postion[0 ~ 2075]");
				NAND_EccError.uEccErrByte2 = UART_GetIntNum()%2076;
				UART_Printf("Input the Second Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit2 = UART_GetIntNum()%8;

				NAND_Inform[g_NandContNum].uMLCECCPageWriteSector = 
								NAND_EccError.uEccErrByte1/(NAND_MLC_TRANS_SIZE+NAND_MLC_ECC_SIZE);

				NAND_Inform[g_NandContNum].uMLCECCErrBytePos[0] = NAND_EccError.uEccErrByte1 - 
								(NAND_Inform[g_NandContNum].uMLCECCPageWriteSector * (NAND_MLC_TRANS_SIZE+NAND_MLC_ECC_SIZE));
				NAND_Inform[g_NandContNum].uMLCECCErrBytePos[1] = NAND_EccError.uEccErrByte2 - 
								(NAND_Inform[g_NandContNum].uMLCECCPageWriteSector * (NAND_MLC_TRANS_SIZE+NAND_MLC_ECC_SIZE));
				
				break;
		case 3 :	uErrorType = NAND_ECCERR_3BIT;

				UART_Printf("Input the First Byte Postion[0 ~ 2075]");
				NAND_EccError.uEccErrByte1 = UART_GetIntNum()%2076;
				UART_Printf("Input the First Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit1 = UART_GetIntNum()%8;

				UART_Printf("Input the Second Byte Postion[0 ~ 2075]");
				NAND_EccError.uEccErrByte2 = UART_GetIntNum()%2076;
				UART_Printf("Input the SecondBit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit2 = UART_GetIntNum()%8;

				UART_Printf("Input the Third Byte Postion[0 ~ 2075]");
				NAND_EccError.uEccErrByte3 = UART_GetIntNum()%2076;
				UART_Printf("Input the Third Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit3 = UART_GetIntNum()%8;

				NAND_Inform[g_NandContNum].uMLCECCPageWriteSector = 
								NAND_EccError.uEccErrByte1/(NAND_MLC_TRANS_SIZE+NAND_MLC_ECC_SIZE);

				NAND_Inform[g_NandContNum].uMLCECCErrBytePos[0] = NAND_EccError.uEccErrByte1 - 
								(NAND_Inform[g_NandContNum].uMLCECCPageWriteSector * (NAND_MLC_TRANS_SIZE+NAND_MLC_ECC_SIZE));
				NAND_Inform[g_NandContNum].uMLCECCErrBytePos[1] = NAND_EccError.uEccErrByte2 - 
								(NAND_Inform[g_NandContNum].uMLCECCPageWriteSector * (NAND_MLC_TRANS_SIZE+NAND_MLC_ECC_SIZE));
				NAND_Inform[g_NandContNum].uMLCECCErrBytePos[2] = NAND_EccError.uEccErrByte3 - 
								(NAND_Inform[g_NandContNum].uMLCECCPageWriteSector * (NAND_MLC_TRANS_SIZE+NAND_MLC_ECC_SIZE));
				
				break;
		case 4 :	uErrorType = NAND_ECCERR_4BIT;

				UART_Printf("Input the First Byte Postion[0 ~ 2075]");
				NAND_EccError.uEccErrByte1 = UART_GetIntNum()%2076;
				UART_Printf("Input the First Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit1 = UART_GetIntNum()%8;

				UART_Printf("Input the Second Byte Postion[0 ~ 2075]");
				NAND_EccError.uEccErrByte2 = UART_GetIntNum()%2076;
				UART_Printf("Input the SecondBit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit2 = UART_GetIntNum()%8;

				UART_Printf("Input the Third Byte Postion[0 ~ 2075]");
				NAND_EccError.uEccErrByte3 = UART_GetIntNum()%2076;
				UART_Printf("Input the Third Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit3 = UART_GetIntNum()%8;

				UART_Printf("Input the Fourth Byte Postion[0 ~ 2075]");
				NAND_EccError.uEccErrByte4 = UART_GetIntNum()%2076;
				UART_Printf("Input the Fourth Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit4 = UART_GetIntNum()%8;

				NAND_Inform[g_NandContNum].uMLCECCPageWriteSector = 
								NAND_EccError.uEccErrByte1/(NAND_MLC_TRANS_SIZE+NAND_MLC_ECC_SIZE);

				NAND_Inform[g_NandContNum].uMLCECCErrBytePos[0] = NAND_EccError.uEccErrByte1 - 
								(NAND_Inform[g_NandContNum].uMLCECCPageWriteSector * (NAND_MLC_TRANS_SIZE+NAND_MLC_ECC_SIZE));
				NAND_Inform[g_NandContNum].uMLCECCErrBytePos[1] = NAND_EccError.uEccErrByte2 - 
								(NAND_Inform[g_NandContNum].uMLCECCPageWriteSector * (NAND_MLC_TRANS_SIZE+NAND_MLC_ECC_SIZE));
				NAND_Inform[g_NandContNum].uMLCECCErrBytePos[2] = NAND_EccError.uEccErrByte3 - 
								(NAND_Inform[g_NandContNum].uMLCECCPageWriteSector * (NAND_MLC_TRANS_SIZE+NAND_MLC_ECC_SIZE));
				NAND_Inform[g_NandContNum].uMLCECCErrBytePos[3] = NAND_EccError.uEccErrByte4 - 
								(NAND_Inform[g_NandContNum].uMLCECCPageWriteSector * (NAND_MLC_TRANS_SIZE+NAND_MLC_ECC_SIZE));
				
				break;
		case 5 :	uErrorType = NAND_ECCERR_UNCORRECT;

				UART_Printf("Input the First Byte Postion[0 ~ 2075]");
				NAND_EccError.uEccErrByte1 = UART_GetIntNum()%2076;
				UART_Printf("Input the First Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit1 = UART_GetIntNum()%8;

				UART_Printf("Input the Second Byte Postion[0 ~ 2075]");
				NAND_EccError.uEccErrByte2 = UART_GetIntNum()%2076;
				UART_Printf("Input the SecondBit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit2 = UART_GetIntNum()%8;

				UART_Printf("Input the Third Byte Postion[0 ~ 2075]");
				NAND_EccError.uEccErrByte3 = UART_GetIntNum()%2076;
				UART_Printf("Input the Third Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit3 = UART_GetIntNum()%8;

				UART_Printf("Input the Fourth Byte Postion[0 ~ 2075]");
				NAND_EccError.uEccErrByte4 = UART_GetIntNum()%2076;
				UART_Printf("Input the Fourth Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit4 = UART_GetIntNum()%8;

				UART_Printf("Input the Fifth Byte Postion[0 ~ 2075]");
				NAND_EccError.uEccErrByte5 = UART_GetIntNum()%2076;
				UART_Printf("Input the Fifth Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit5 = UART_GetIntNum()%8;

				NAND_Inform[g_NandContNum].uMLCECCPageWriteSector = 
								NAND_EccError.uEccErrByte1/(NAND_MLC_TRANS_SIZE+NAND_MLC_ECC_SIZE);

				NAND_Inform[g_NandContNum].uMLCECCErrBytePos[0] = NAND_EccError.uEccErrByte1 - 
								(NAND_Inform[g_NandContNum].uMLCECCPageWriteSector * (NAND_MLC_TRANS_SIZE+NAND_MLC_ECC_SIZE));
				NAND_Inform[g_NandContNum].uMLCECCErrBytePos[1] = NAND_EccError.uEccErrByte2 - 
								(NAND_Inform[g_NandContNum].uMLCECCPageWriteSector * (NAND_MLC_TRANS_SIZE+NAND_MLC_ECC_SIZE));
				NAND_Inform[g_NandContNum].uMLCECCErrBytePos[2] = NAND_EccError.uEccErrByte3 - 
								(NAND_Inform[g_NandContNum].uMLCECCPageWriteSector * (NAND_MLC_TRANS_SIZE+NAND_MLC_ECC_SIZE));
				NAND_Inform[g_NandContNum].uMLCECCErrBytePos[3] = NAND_EccError.uEccErrByte4 - 
								(NAND_Inform[g_NandContNum].uMLCECCPageWriteSector * (NAND_MLC_TRANS_SIZE+NAND_MLC_ECC_SIZE));
				NAND_Inform[g_NandContNum].uMLCECCErrBytePos[4] = NAND_EccError.uEccErrByte5 - 
								(NAND_Inform[g_NandContNum].uMLCECCPageWriteSector * (NAND_MLC_TRANS_SIZE+NAND_MLC_ECC_SIZE));
				
				break;
		default : 
				return;
	}
	
	srand(1);
	for(i=0 ; i<NAND_Inform[g_NandContNum].uPageSize ; i++)
	{
		aBuffer[i] = rand()%0xFF;
	}
	for(i=0 ; i<NAND_Inform[g_NandContNum].uSpareSize ; i++)
		aSpareBuffer[i] = 0xFF;	

	for(m=0 ; m<4 ; m++)
		for(n=0 ; n<2 ; n++)
			aNANDT_EccError[m][n] = 0xFFFFFFFE;
		
	UART_Printf("\n");
	
	NAND_Inform[g_NandContNum].uECCtest = 1;	
	NAND_Inform[g_NandContNum].uAllBitEccCheck = 1;
	
	// Erase Block for test
	//UART_Printf("Erase the Block ( %d[block])\n", uBlock);
	eError = NAND_EraseSingleBlock(g_NandContNum, uBlock);
	if(eError != eNAND_NoError)
	{
		NANDT_PrintErrorType(eError);
		NAND_Inform[g_NandContNum].uECCtest = 0;
		NAND_Inform[g_NandContNum].uAllBitEccCheck = 0;
		return;
	}

	// Write the valid data to page ( uBlock[block] 0[page] )
	//UART_Printf("Write the valid data to page for ECC generation ( %d[block] %d[page] )\n", uBlock, uPage);
	NAND_Inform[g_NandContNum].uECCtest = 1;	
	eError = NAND_WritePage(g_NandContNum, uBlock, uPage, aBuffer, aSpareBuffer);
	if(eError != eNAND_NoError)
	{
		NANDT_PrintErrorType(eError);
		NAND_Inform[g_NandContNum].uECCtest = 0;
		NAND_Inform[g_NandContNum].uAllBitEccCheck = 0;
		return;
	}

	//UART_Printf("\n");
	// Test Start......!!
	if(uErrorType == NAND_ECCERR_1BIT)
	{	
		UART_Printf("[%d byte : %d bit]\n", NAND_EccError.uEccErrByte1, NAND_EccError.uEccErrBit1);
	}
	else if(uErrorType == NAND_ECCERR_2BIT)
	{
		UART_Printf("[%d byte : %d bit] & [%d byte : %d bit] \n", 	NAND_EccError.uEccErrByte1, NAND_EccError.uEccErrBit1,
													NAND_EccError.uEccErrByte2, NAND_EccError.uEccErrBit2);
	}
	else if(uErrorType == NAND_ECCERR_3BIT)
	{
		UART_Printf("[%d byte : %d bit] & [%d byte : %d bit] &  [%d byte : %d bit]\n", 	
														NAND_EccError.uEccErrByte1, NAND_EccError.uEccErrBit1,
														NAND_EccError.uEccErrByte2, NAND_EccError.uEccErrBit2,
														NAND_EccError.uEccErrByte3, NAND_EccError.uEccErrBit3);
	}
	else if(uErrorType == NAND_ECCERR_4BIT)
	{
		UART_Printf("[%d byte : %d bit] & [%d byte : %d bit] &  [%d byte : %d bit] &  [%d byte : %d bit]\n", 	
														NAND_EccError.uEccErrByte1, NAND_EccError.uEccErrBit1,
														NAND_EccError.uEccErrByte2, NAND_EccError.uEccErrBit2,
														NAND_EccError.uEccErrByte3, NAND_EccError.uEccErrBit3,
														NAND_EccError.uEccErrByte4, NAND_EccError.uEccErrBit4);
	}
	else if(uErrorType == NAND_ECCERR_UNCORRECT)
	{
		UART_Printf("[%d byte : %d bit] & [%d byte : %d bit] &  [%d byte : %d bit] & [%d byte : %d bit] &  [%d byte : %d bit]\n", 	
														NAND_EccError.uEccErrByte1, NAND_EccError.uEccErrBit1,
														NAND_EccError.uEccErrByte2, NAND_EccError.uEccErrBit2,
														NAND_EccError.uEccErrByte3, NAND_EccError.uEccErrBit3,
														NAND_EccError.uEccErrByte4, NAND_EccError.uEccErrBit4,
														NAND_EccError.uEccErrByte5, NAND_EccError.uEccErrBit5);
	}
	
	// Erase Block for test
	//UART_Printf("Erase the Block ( %d[block])\n", uBlock);
	eError = NAND_EraseSingleBlock(g_NandContNum, uBlock);
	if(eError != eNAND_NoError)
	{
		NANDT_PrintErrorType(eError);
		NAND_Inform[g_NandContNum].uECCtest = 0;
		NAND_Inform[g_NandContNum].uAllBitEccCheck = 0;
		return;
	}

	// Write the invalid data to page( uBlock[block] 1[page] )
	//UART_Printf("Write the INVALID data to page for ECC generation ( %d[block] %d[page] )\n", uBlock, uPage);
	eError = NAND_WritePageWithInvalidData(g_NandContNum, uBlock, uPage, aBuffer, uErrorType);
	if(eError != eNAND_NoError)
	{
		NANDT_PrintErrorType(eError);
		NAND_Inform[g_NandContNum].uECCtest = 0;
		NAND_Inform[g_NandContNum].uAllBitEccCheck = 0;
		return;
	}
					
	// Read the invalid data to detect the ECC error	
	//UART_Printf("Read the INVALID page  ( %d[block] %d[page])\n", uBlock, uPage);
	eError = NAND_ReadPage(g_NandContNum, uBlock, uPage, aReadBuffer, aSpareBuffer);

	NANDT_PrintErrorType(eError);
	if(uErrorType == NAND_ECCERR_1BIT)
	{
		if( (NAND_EccError.uEccErrByte1 != aNANDT_EccError[0][0]) || (NAND_EccError.uEccErrBit1 != aNANDT_EccError[0][1]) )
		{
			UART_Printf("Ecc Error Check Error[%d byte:%d bit] != [%d byte:%d bit] \n", NAND_EccError.uEccErrByte1, NAND_EccError.uEccErrBit1,
																		aNANDT_EccError[0][0], aNANDT_EccError[0][1]);	
			UART_Getc();
			NAND_Inform[g_NandContNum].uECCtest = 0;
			NAND_Inform[g_NandContNum].uAllBitEccCheck = 0;
			return;
		}
	}
	else if(uErrorType == NAND_ECCERR_2BIT)
	{
		if( 	((NAND_EccError.uEccErrByte1 != aNANDT_EccError[0][0]) || (NAND_EccError.uEccErrBit1 != aNANDT_EccError[0][1])) ||
			((NAND_EccError.uEccErrByte2 != aNANDT_EccError[1][0]) || (NAND_EccError.uEccErrBit2 != aNANDT_EccError[1][1])) )
		{
			UART_Printf("Ecc Error Check Error[%d byte:%d bit] != [%d byte:%d bit] \n", NAND_EccError.uEccErrByte1, NAND_EccError.uEccErrBit1,
																		aNANDT_EccError[0][0], aNANDT_EccError[0][1]);	
			UART_Printf("                     [%d byte:%d bit] != [%d byte:%d bit] \n", 	NAND_EccError.uEccErrByte2, NAND_EccError.uEccErrBit2,
																	aNANDT_EccError[1][0], aNANDT_EccError[1][1]);						
			UART_Getc();
			NAND_Inform[g_NandContNum].uECCtest = 0;
			NAND_Inform[g_NandContNum].uAllBitEccCheck = 0;
			return;
		}
	}
	else if(uErrorType == NAND_ECCERR_3BIT)
	{
		if( 	((NAND_EccError.uEccErrByte1 != aNANDT_EccError[0][0]) || (NAND_EccError.uEccErrBit1 != aNANDT_EccError[0][1])) ||
			((NAND_EccError.uEccErrByte2 != aNANDT_EccError[1][0]) || (NAND_EccError.uEccErrBit2 != aNANDT_EccError[1][1])) ||
			((NAND_EccError.uEccErrByte3 != aNANDT_EccError[2][0]) || (NAND_EccError.uEccErrBit3 != aNANDT_EccError[2][1])) )
		{
			UART_Printf("Ecc Error Check Error[%d byte:%d bit] != [%d byte:%d bit] \n", NAND_EccError.uEccErrByte1, NAND_EccError.uEccErrBit1,
																		aNANDT_EccError[0][0], aNANDT_EccError[0][1]);	
			UART_Printf("                     [%d byte:%d bit] != [%d byte:%d bit] \n", 	NAND_EccError.uEccErrByte2, NAND_EccError.uEccErrBit2,
																	aNANDT_EccError[1][0], aNANDT_EccError[1][1]);
			UART_Printf("                     [%d byte:%d bit] != [%d byte:%d bit] \n", 	NAND_EccError.uEccErrByte3, NAND_EccError.uEccErrBit3,
																	aNANDT_EccError[2][0], aNANDT_EccError[2][1]);
			UART_Getc();
			NAND_Inform[g_NandContNum].uECCtest = 0;
			NAND_Inform[g_NandContNum].uAllBitEccCheck = 0;
			return;
		}
	}
	else if(uErrorType == NAND_ECCERR_4BIT)
	{
		if( 	((NAND_EccError.uEccErrByte1 != aNANDT_EccError[0][0]) || (NAND_EccError.uEccErrBit1 != aNANDT_EccError[0][1])) ||
			((NAND_EccError.uEccErrByte2 != aNANDT_EccError[1][0]) || (NAND_EccError.uEccErrBit2 != aNANDT_EccError[1][1])) ||
			((NAND_EccError.uEccErrByte3 != aNANDT_EccError[2][0]) || (NAND_EccError.uEccErrBit3 != aNANDT_EccError[2][1])) ||
			((NAND_EccError.uEccErrByte4 != aNANDT_EccError[3][0]) || (NAND_EccError.uEccErrBit4 != aNANDT_EccError[3][1])) )
		{
			UART_Printf("Ecc Error Check Error[%d byte:%d bit] != [%d byte:%d bit] \n", NAND_EccError.uEccErrByte1, NAND_EccError.uEccErrBit1,
																		aNANDT_EccError[0][0], aNANDT_EccError[0][1]);	
			UART_Printf("                     [%d byte:%d bit] != [%d byte:%d bit] \n", 	NAND_EccError.uEccErrByte2, NAND_EccError.uEccErrBit2,
																	aNANDT_EccError[1][0], aNANDT_EccError[1][1]);
			UART_Printf("                     [%d byte:%d bit] != [%d byte:%d bit] \n", 	NAND_EccError.uEccErrByte3, NAND_EccError.uEccErrBit3,
																	aNANDT_EccError[2][0], aNANDT_EccError[2][1]);
			UART_Printf("                     [%d byte:%d bit] != [%d byte:%d bit] \n", 	NAND_EccError.uEccErrByte4, NAND_EccError.uEccErrBit4,
																	aNANDT_EccError[3][0], aNANDT_EccError[3][1]);
			UART_Getc();
			NAND_Inform[g_NandContNum].uECCtest = 0;
			NAND_Inform[g_NandContNum].uAllBitEccCheck = 0;
			return;
		}
	}
	else if(uErrorType == NAND_ECCERR_UNCORRECT)
	{
		if( aNANDT_EccError[0][0] != NAND_ECC_UNCORRECT_ERROR)
		{
			UART_Printf("Uncorrectable Ecc Error Check Error\n");	
			aNANDT_EccError[0][0] = 0;
			UART_Getc();
			NAND_Inform[g_NandContNum].uECCtest = 0;
			NAND_Inform[g_NandContNum].uAllBitEccCheck = 0;
			return;
		}
		aNANDT_EccError[0][0] = 0;
	}				

	NAND_Inform[g_NandContNum].uECCtest = 0;
	NAND_Inform[g_NandContNum].uAllBitEccCheck = 0;
	
	UART_Printf("MLC Nand ECC test complete & OK...!!\n");
	UART_Printf("\n");	
}


//////////
// Function Name : NANDT_SLCECC_All
// Function Description : This function checks the ECC operation (all bit of SLC Nand Device 1 page)
// Input : 	None
// Output : 	None
void NANDT_SLCECC_All(void)
{
	u32 i, j, uBlock, uPage, uError, uErrorType;
	u8 aBuffer[NAND_PAGE_MAX];
	u8 aSpareBuffer[NAND_SPARE_MAX];
	NAND_eERROR eError;
	u8 aReadBuffer[NAND_PAGE_MAX];
		
	UART_Printf("[NANDT_SLCECC_All]\n");

	if(NAND_Inform[g_NandContNum].uNandType == NAND_MLC8bit)
	{
		UART_Printf("The current Nand Memory is MLC type.....!!\n\n");
		return;
	}
	
	UART_Printf("Input the Block Number to test[0~%d]", NAND_Inform[g_NandContNum].uBlockNum-1);
	uBlock = UART_GetIntNum();
	UART_Printf("Input the Page Number to test[0~%d]", NAND_Inform[g_NandContNum].uPageNum-1);
	uPage = UART_GetIntNum();	
	
	UART_Printf("Input the ECC Error Type\n");
	UART_Printf("1: 1bit error(D),	2: multiple error\n");
	uError = UART_GetIntNum();
	
	switch(uError)
	{
		case 1 :	uErrorType = NAND_ECCERR_1BIT;
				break;
		case 2 :	uErrorType = NAND_ECCERR_MULTI;
				break;
		default :	uErrorType = NAND_ECCERR_1BIT;
				break;				
	}
	
	srand(0);
	for(i=0 ; i<NAND_Inform[g_NandContNum].uPageSize ; i++)
	{
		aBuffer[i] = rand()%0xFF;
	}
	for(i=0 ; i<NAND_Inform[g_NandContNum].uSpareSize ; i++)
		aSpareBuffer[i] = 0xFF;	
		
	UART_Printf("\n");
	UART_Printf("The all bit ecc check of %dBlock:%dPage\n", uBlock, uPage);
	UART_Printf("\n");
		
	NAND_EccError.uEccErrByte1 = 0;
	NAND_EccError.uEccErrBit1 = 0;
	NAND_EccError.uEccErrByte2 = (NAND_EccError.uEccErrByte1+1)%NAND_Inform[g_NandContNum].uPageSize;
	NAND_EccError.uEccErrBit2 = 0;

	NAND_Inform[g_NandContNum].uECCtest = 1;	
	NAND_Inform[g_NandContNum].uAllBitEccCheck = 1;
	
	// uBlock+1[Block] Erase & Write to generate the accurate ECC code(aNand_Spare_Data_Temp[i])
	eError = NAND_EraseSingleBlock(g_NandContNum, uBlock);
	if(eError != eNAND_NoError)
	{
		NANDT_PrintErrorType(eError);
		NAND_Inform[g_NandContNum].uECCtest = 0;
		NAND_Inform[g_NandContNum].uAllBitEccCheck = 0;
		return;
	}
		
	eError = NAND_WritePage(g_NandContNum, uBlock, 0, aBuffer, aSpareBuffer);	// write to 0th page
	if(eError != eNAND_NoError)
	{
		NANDT_PrintErrorType(eError);
		NAND_Inform[g_NandContNum].uECCtest = 0;
		NAND_Inform[g_NandContNum].uAllBitEccCheck = 0;
		return;
	}

	// Test Start......!!
	for(j=0 ; j<NAND_Inform[g_NandContNum].uPageSize ; j++)
	{
		for(i=0 ; i<8 ; i++)
		{
			if(uErrorType == NAND_ECCERR_1BIT)
			{				
				UART_Printf("[%d byte : %d bit]\n", NAND_EccError.uEccErrByte1, NAND_EccError.uEccErrBit1);
			}
			else if(uErrorType == NAND_ECCERR_MULTI)
			{
				UART_Printf("[%d byte : %d bit] & [%d byte : %d bit] \n", 	NAND_EccError.uEccErrByte1, NAND_EccError.uEccErrBit1,
															NAND_EccError.uEccErrByte2, NAND_EccError.uEccErrBit2);
			}
				
			// Erase Block for test
			eError = NAND_EraseSingleBlock(g_NandContNum, uBlock);
			if(eError != eNAND_NoError)
			{
				NANDT_PrintErrorType(eError);
				NAND_Inform[g_NandContNum].uECCtest = 0;
				NAND_Inform[g_NandContNum].uAllBitEccCheck = 0;
				return;
			}

			// Write the invalid data to page( uBlock[block] 1[page] )
			eError = NAND_WritePageWithInvalidData(g_NandContNum, uBlock, uPage, aBuffer, uErrorType);
			if(eError != eNAND_NoError)
			{
				NANDT_PrintErrorType(eError);
				NAND_Inform[g_NandContNum].uECCtest = 0;
				NAND_Inform[g_NandContNum].uAllBitEccCheck = 0;
				return;
			}
				
			// Read the invalid data to detect the ECC error	
			eError = NAND_ReadPage(g_NandContNum, uBlock, uPage, aReadBuffer, aSpareBuffer);

			NANDT_PrintErrorType(eError);
			if(uErrorType == NAND_ECCERR_1BIT)
			{
				if( (NAND_EccError.uEccErrByte1 != aNANDT_EccError[0][0]) || (NAND_EccError.uEccErrBit1 != aNANDT_EccError[0][1]) )
				{
					UART_Printf("Ecc Error Check Error[%d byte:%d bit] != [%d byte:%d bit] \n", NAND_EccError.uEccErrByte1, NAND_EccError.uEccErrBit1,
																				aNANDT_EccError[0][0], aNANDT_EccError[0][1]);	
					UART_Getc();
					NAND_Inform[g_NandContNum].uECCtest = 0;
					NAND_Inform[g_NandContNum].uAllBitEccCheck = 0;
					return;
				}
			}
			else if(uErrorType == NAND_ECCERR_MULTI)
			{
				if( aNANDT_EccError[0][0] != NAND_ECC_MULTI_ERROR)
				{
					UART_Printf("Multi Ecc Error Check Error\n");	
					aNANDT_EccError[0][0] = 0;
					UART_Getc();
					NAND_Inform[g_NandContNum].uECCtest = 0;
					NAND_Inform[g_NandContNum].uAllBitEccCheck = 0;
					return;
				}
				aNANDT_EccError[0][0] = 0;
			}				

			NAND_EccError.uEccErrBit1++;
			NAND_EccError.uEccErrBit2++;		
		}

		NAND_EccError.uEccErrBit1 = 0;
		NAND_EccError.uEccErrBit2 = 0;	
		NAND_EccError.uEccErrByte1++;
		NAND_EccError.uEccErrByte2 = (NAND_EccError.uEccErrByte1+1)%NAND_Inform[g_NandContNum].uPageSize;
	}

	NAND_EccError.uEccErrByte1 = 0;
	NAND_EccError.uEccErrByte2 = 0;
	
	NAND_Inform[g_NandContNum].uECCtest = 0;
	NAND_Inform[g_NandContNum].uAllBitEccCheck = 0;

	UART_Printf("SLC Nand All bit ECC test complete\n");
	UART_Printf("\n");	
}


//////////
// Function Name : NANDT_SLCSpareECC_All
// Function Description : This function checks the ECC operation (all bit of Nand Device Spare Area)
// Input : 	None
// Output : 	None
void NANDT_SLCSpareECC_All(void)
{
	u32 i, j, uBlock, uPage, uError, uErrorType, uFirstEccPos=0;
	u8 aBuffer[NAND_PAGE_MAX];
	u8 aSpareBuffer[NAND_SPARE_MAX];
	NAND_eERROR eError;
	u8 aReadBuffer[NAND_PAGE_MAX];
		
	UART_Printf("[NANDT_SLCSpareECC]\n");

	if(NAND_Inform[g_NandContNum].uNandType == NAND_MLC8bit)
	{
		UART_Printf("The current Nand Memory is MLC type.....!!\n\n");
		return;
	}
	
	UART_Printf("Input the Block Number to test[0~%d]", NAND_Inform[g_NandContNum].uBlockNum-1);
	uBlock = UART_GetIntNum();
	UART_Printf("Input the Page Number to test[0~%d]", NAND_Inform[g_NandContNum].uPageNum-1);
	uPage = UART_GetIntNum();	
	
	UART_Printf("Input the ECC Error Type\n");
	UART_Printf("1: 1bit error(D),	2: multiple error\n");
	uError = UART_GetIntNum();
	
	switch(uError)
	{
		case 1 :	uErrorType = NAND_ECCERR_1BIT;
				break;
		case 2 :	uErrorType = NAND_ECCERR_MULTI;
				break;
		default :	uErrorType = NAND_ECCERR_1BIT;
				break;				
	}
	
	srand(0);
	for(i=0 ; i<NAND_Inform[g_NandContNum].uPageSize ; i++)
	{
		aBuffer[i] = rand()%0xFF;
	}
	for(i=0 ; i<NAND_Inform[g_NandContNum].uSpareSize ; i++)
		aSpareBuffer[i] = 0xFF;	


		
	UART_Printf("\n");
	UART_Printf("The all bit ecc check of [%d Block:%dPage] Spare Area\n", uBlock, uPage);
	UART_Printf("\n");

	if(NAND_Inform[g_NandContNum].uNandType == NAND_Normal8bit)
	{		
		// Spare Area 0~3 byte : Main ECC Value
		// Spare Area 8~9 byte : Spare ECC Value
		NAND_EccError.uEccErrByte1 = 0;
		NAND_EccError.uEccErrBit1 = 0;
		NAND_EccError.uEccErrByte2 = (NAND_EccError.uEccErrByte1+1)%4;
		NAND_EccError.uEccErrBit2 = 0;

		// The first ECC data byte position
		uFirstEccPos = 0;
	}
	else if(NAND_Inform[g_NandContNum].uNandType == NAND_Advanced8bit)
	{
		// Spare Area 1~4 byte : Main ECC Value
		// Spare Area 8~9 byte : Spare ECC Value
		NAND_EccError.uEccErrByte1 = 1;
		NAND_EccError.uEccErrBit1 = 0;
		NAND_EccError.uEccErrByte2 = (NAND_EccError.uEccErrByte1%4) + 1;
		NAND_EccError.uEccErrBit2 = 0;

		// The first ECC data byte position
		uFirstEccPos = 1;			
	}
		
	NAND_Inform[g_NandContNum].uECCtest = 1;
	NAND_Inform[g_NandContNum].uAllBitEccCheck = 1;
	NAND_Inform[g_NandContNum].uSpareECCtest = 0;

	// uBlock+1[Block] Erase & Write to generate the accurate ECC code(aNand_Spare_Data_Temp[i])
	eError = NAND_EraseSingleBlock(g_NandContNum, uBlock);
	if(eError != eNAND_NoError)
	{
		NANDT_PrintErrorType(eError);
		NAND_Inform[g_NandContNum].uECCtest = 0;
		NAND_Inform[g_NandContNum].uAllBitEccCheck = 0;		
		NAND_Inform[g_NandContNum].uSpareECCtest = 0;			
		return;
	}
		
	eError = NAND_WritePage(g_NandContNum, uBlock, 0, aBuffer, aSpareBuffer);	// write to 0th page, ECC generation
	if(eError != eNAND_NoError)
	{
		NANDT_PrintErrorType(eError);
		NAND_Inform[g_NandContNum].uECCtest = 0;
		NAND_Inform[g_NandContNum].uAllBitEccCheck = 0;
		NAND_Inform[g_NandContNum].uSpareECCtest = 0;				
		return;
	}

	NAND_Inform[g_NandContNum].uSpareECCtest = 1;	
		
	// Test Start......!!
	for(j=0 ; j<4 ; j++)	// Main ECC Data : 4 Bytes
	{
		for(i=0 ; i<8 ; i++)	// 8bit
		{
			if(uErrorType == NAND_ECCERR_1BIT)
			{				
				UART_Printf("Spare [%d byte : %d bit] Test\n", NAND_EccError.uEccErrByte1, NAND_EccError.uEccErrBit1);
			}
			else if(uErrorType == NAND_ECCERR_MULTI)
			{
				UART_Printf("Spare[%d byte : %d bit] & [%d byte : %d bit] Test\n", 	NAND_EccError.uEccErrByte1, NAND_EccError.uEccErrBit1,
																NAND_EccError.uEccErrByte2, NAND_EccError.uEccErrBit2);
			}
			
			// Erase Block for test
			eError = NAND_EraseSingleBlock(g_NandContNum, uBlock);
			if(eError != eNAND_NoError)
			{
				NANDT_PrintErrorType(eError);
				NAND_Inform[g_NandContNum].uECCtest = 0;
				NAND_Inform[g_NandContNum].uAllBitEccCheck = 0;
				NAND_Inform[g_NandContNum].uSpareECCtest = 0;						
				return;
			}

			// Write the invalid data to page( uBlock[block] 1[page] )
			eError = NAND_WritePageWithInvalidSpareData(g_NandContNum, uBlock, uPage, aBuffer, uErrorType);
			if(eError != eNAND_NoError)
			{
				NANDT_PrintErrorType(eError);
				NAND_Inform[g_NandContNum].uECCtest = 0;
				NAND_Inform[g_NandContNum].uAllBitEccCheck = 0;
				NAND_Inform[g_NandContNum].uSpareECCtest = 0;						
				return;
			}
			
			// Read the invalid data to detect the ECC error	
			eError = NAND_ReadPage(g_NandContNum, uBlock, uPage, aReadBuffer, aSpareBuffer);

			NANDT_PrintErrorType(eError);
			if(uErrorType == NAND_ECCERR_1BIT)
			{
				if( (NAND_EccError.uEccErrByte1 != (aNANDT_EccError[0][0]+uFirstEccPos)) || (NAND_EccError.uEccErrBit1 != aNANDT_EccError[0][1]) )
				{
					UART_Printf("Ecc Error Check Error[%d byte:%d bit] != [%d byte:%d bit] \n", NAND_EccError.uEccErrByte1, NAND_EccError.uEccErrBit1,
																				aNANDT_EccError[0][0], aNANDT_EccError[0][1]);	
					UART_Getc();
					NAND_Inform[g_NandContNum].uECCtest = 0;
					NAND_Inform[g_NandContNum].uAllBitEccCheck = 0;
					NAND_Inform[g_NandContNum].uSpareECCtest = 0;							
					return;
				}
			}
			else if(uErrorType == NAND_ECCERR_MULTI)
			{
				if( aNANDT_EccError[0][0] != NAND_ECC_MULTI_ERROR)
				{
					UART_Printf("Multi Ecc Error Check Error\n");	
					aNANDT_EccError[0][0] = 0;
					UART_Getc();
					NAND_Inform[g_NandContNum].uECCtest = 0;
					NAND_Inform[g_NandContNum].uAllBitEccCheck = 0;
					NAND_Inform[g_NandContNum].uSpareECCtest = 0;							
					return;
				}
				aNANDT_EccError[0][0] = 0;
			}				

			NAND_EccError.uEccErrBit1++;
			NAND_EccError.uEccErrBit2++;		
		}

		NAND_EccError.uEccErrBit1 = 0;
		NAND_EccError.uEccErrBit2 = 0;	
		NAND_EccError.uEccErrByte1++;
		if(NAND_Inform[g_NandContNum].uNandType == NAND_Normal8bit)
		{
			NAND_EccError.uEccErrByte2 = (NAND_EccError.uEccErrByte1+1)%4;
		}
		else if(NAND_Inform[g_NandContNum].uNandType == NAND_Advanced8bit)
		{			
			NAND_EccError.uEccErrByte2 = (NAND_EccError.uEccErrByte1%4) + 1;
		}			
	}

	NAND_EccError.uEccErrByte1 = 0;
	NAND_EccError.uEccErrByte2 = 0;
	
	NAND_Inform[g_NandContNum].uECCtest = 0;
	NAND_Inform[g_NandContNum].uAllBitEccCheck = 0;
	NAND_Inform[g_NandContNum].uSpareECCtest = 0;	

	UART_Printf("SLC Nand ECC test complete\n");
	UART_Printf("\n");


}


//////////
// Function Name : NANDT_MLCECC_All
// Function Description : This function checks the ECC operation (all bit of MLC Nand Device 1page)
// Input : 	None
// Output : 	None
void NANDT_MLCECC_All(void)
{
	u32 i, j, k, l, m, uExceptCnt, uBlock, uPage, uError, uErrorType;
	u8 aBuffer[NAND_PAGE_MAX];
	u8 aSpareBuffer[NAND_SPARE_MAX];
	u8 aReadBuffer[NAND_PAGE_MAX];
	NAND_eERROR eError;
	
	UART_Printf("[NANDT_MLCECC_All]\n");

	if(NAND_Inform[g_NandContNum].uNandType != NAND_MLC8bit)
	{
		UART_Printf("The current Nand Memory is not MLC type.....!!\n\n");
		return;
	}
	
	UART_Printf("Input the Block Number to test[0~%d]", NAND_Inform[g_NandContNum].uBlockNum-1);
	uBlock = UART_GetIntNum();
	UART_Printf("Input the Page Number to test[0~%d]", NAND_Inform[g_NandContNum].uPageNum-1);
	uPage = UART_GetIntNum();	
	
	UART_Printf("Input the ECC Error Type\n");
	UART_Printf("1: 1bit error(D),	2: 2bit error,	3: 3bit error,	4: 4bit error,	5: Uncorrectable\n");
	uError = UART_GetIntNum();
	switch(uError)
	{
		case 1 :	uErrorType = NAND_ECCERR_1BIT;
				uExceptCnt = 0;
				break;
		case 2 :	uErrorType = NAND_ECCERR_2BIT;
				uExceptCnt = 1;
				break;
		case 3 :	uErrorType = NAND_ECCERR_3BIT;
				uExceptCnt = 2;
				break;
		case 4 :	uErrorType = NAND_ECCERR_4BIT;
				uExceptCnt = 3;
				break;
		case 5 :	uErrorType = NAND_ECCERR_UNCORRECT;
				uExceptCnt = 4;
				break;
		default :	uErrorType = NAND_ECCERR_1BIT;
				uExceptCnt = 0;
				break;				
	}
	uPage = 0;

	
	srand(1);
	for(i=0 ; i<NAND_Inform[g_NandContNum].uPageSize ; i++)
	{
		aBuffer[i] = rand()%0xFF;
	}
	for(i=0 ; i<NAND_Inform[g_NandContNum].uSpareSize ; i++)
		aSpareBuffer[i] = 0xFF;	

	UART_Printf("\n");
	UART_Printf("The all bit ecc check of %dBlock:%dPage\n", uBlock, uPage);
	UART_Printf("\n");
	
	NAND_EccError.uEccErrByte1 = 0;
	NAND_EccError.uEccErrBit1 =0;		// Bit Pattern not Bit Position
	NAND_EccError.uEccErrByte2 = (NAND_EccError.uEccErrByte1+1);//%NAND_MLC_TRANS_SIZE;//NAND_Inform[g_NandContNum].uPageSize;
	NAND_EccError.uEccErrBit2 = 0;
	NAND_EccError.uEccErrByte3 = (NAND_EccError.uEccErrByte2+1);//%NAND_MLC_TRANS_SIZE;//NAND_Inform[g_NandContNum].uPageSize;
	NAND_EccError.uEccErrBit3 = 0;
	NAND_EccError.uEccErrByte4 = (NAND_EccError.uEccErrByte3+1);//%NAND_MLC_TRANS_SIZE;//NAND_Inform[g_NandContNum].uPageSize;
	NAND_EccError.uEccErrBit4 = 0;
	NAND_EccError.uEccErrByte5 = (NAND_EccError.uEccErrByte4+1);//%NAND_MLC_TRANS_SIZE;//NAND_Inform[g_NandContNum].uPageSize;
	NAND_EccError.uEccErrBit5 = 0;	

	NAND_Inform[g_NandContNum].uECCtest = 1;	
	NAND_Inform[g_NandContNum].uAllBitEccCheck = 1;
	
	// Erase Block for test
	eError = NAND_EraseSingleBlock(g_NandContNum, uBlock);
	if(eError != eNAND_NoError)
	{
		NANDT_PrintErrorType(eError);
		NAND_Inform[g_NandContNum].uECCtest = 0;
		NAND_Inform[g_NandContNum].uAllBitEccCheck = 0;
		return;
	}

	// Write the valid data to page 0 ( uBlock[block] 0[page] )
	UART_Printf("Write the valid data to page 0 ( %d[block] %d[page] )\n", uBlock, uPage);
	NAND_Inform[g_NandContNum].uECCtest = 1;	
	eError = NAND_WritePage(g_NandContNum, uBlock, uPage, aBuffer, aSpareBuffer);
	if(eError != eNAND_NoError)
	{
		NANDT_PrintErrorType(eError);
		NAND_Inform[g_NandContNum].uECCtest = 0;
		NAND_Inform[g_NandContNum].uAllBitEccCheck = 0;
		return;
	}

	// Test Start......!!
	for(k=0 ; k<4 ; k++)
	{
		NAND_Inform[g_NandContNum].uMLCECCPageWriteSector = k;

		for(j=0 ; j<(NAND_MLC_TRANS_SIZE+NAND_MLC_ECC_SIZE-uExceptCnt) ; j++)
		{
			for(i=0 ; i<8 ; i++)
			{
				if(uErrorType == NAND_ECCERR_1BIT)
				{	
					NAND_Inform[g_NandContNum].uMLCECCErrBytePos[0] = j;		// for generate the ECC Error in the MLC Spare Area
					UART_Printf("[%d byte : %d bit]\n", NAND_EccError.uEccErrByte1, NAND_EccError.uEccErrBit1);
				}
				else if(uErrorType == NAND_ECCERR_2BIT)
				{
					NAND_Inform[g_NandContNum].uMLCECCErrBytePos[0] = j;
					NAND_Inform[g_NandContNum].uMLCECCErrBytePos[1] = NAND_Inform[g_NandContNum].uMLCECCErrBytePos[0] + 1;
					
					UART_Printf("[%d byte : %d bit] & [%d byte : %d bit] \n", 	NAND_EccError.uEccErrByte1, NAND_EccError.uEccErrBit1,
																NAND_EccError.uEccErrByte2, NAND_EccError.uEccErrBit2);
				}
				else if(uErrorType == NAND_ECCERR_3BIT)
				{
					NAND_Inform[g_NandContNum].uMLCECCErrBytePos[0] = j;
					NAND_Inform[g_NandContNum].uMLCECCErrBytePos[1] = NAND_Inform[g_NandContNum].uMLCECCErrBytePos[0] + 1;
					NAND_Inform[g_NandContNum].uMLCECCErrBytePos[2] = NAND_Inform[g_NandContNum].uMLCECCErrBytePos[1] + 1;
					
					UART_Printf("[%d byte : %d bit] & [%d byte : %d bit] &  [%d byte : %d bit]\n", 	
																	NAND_EccError.uEccErrByte1, NAND_EccError.uEccErrBit1,
																	NAND_EccError.uEccErrByte2, NAND_EccError.uEccErrBit2,
																	NAND_EccError.uEccErrByte3, NAND_EccError.uEccErrBit3);
				}
				else if(uErrorType == NAND_ECCERR_4BIT)
				{
					NAND_Inform[g_NandContNum].uMLCECCErrBytePos[0] = j;
					NAND_Inform[g_NandContNum].uMLCECCErrBytePos[1] = NAND_Inform[g_NandContNum].uMLCECCErrBytePos[0] + 1;
					NAND_Inform[g_NandContNum].uMLCECCErrBytePos[2] = NAND_Inform[g_NandContNum].uMLCECCErrBytePos[1] + 1;
					NAND_Inform[g_NandContNum].uMLCECCErrBytePos[3] = NAND_Inform[g_NandContNum].uMLCECCErrBytePos[2] + 1;
				
					UART_Printf("[%d byte : %d bit] & [%d byte : %d bit] &  [%d byte : %d bit] &  [%d byte : %d bit]\n", 	
																	NAND_EccError.uEccErrByte1, NAND_EccError.uEccErrBit1,
																	NAND_EccError.uEccErrByte2, NAND_EccError.uEccErrBit2,
																	NAND_EccError.uEccErrByte3, NAND_EccError.uEccErrBit3,
																	NAND_EccError.uEccErrByte4, NAND_EccError.uEccErrBit4);
				}
				else if(uErrorType == NAND_ECCERR_UNCORRECT)
				{
					NAND_Inform[g_NandContNum].uMLCECCErrBytePos[0] = j;
					NAND_Inform[g_NandContNum].uMLCECCErrBytePos[1] = NAND_Inform[g_NandContNum].uMLCECCErrBytePos[0] + 1;
					NAND_Inform[g_NandContNum].uMLCECCErrBytePos[2] = NAND_Inform[g_NandContNum].uMLCECCErrBytePos[1] + 1;
					NAND_Inform[g_NandContNum].uMLCECCErrBytePos[3] = NAND_Inform[g_NandContNum].uMLCECCErrBytePos[2] + 1;
					NAND_Inform[g_NandContNum].uMLCECCErrBytePos[4] = NAND_Inform[g_NandContNum].uMLCECCErrBytePos[3] + 1;
					
					UART_Printf("[%d byte : %d bit] & [%d byte : %d bit] &  [%d byte : %d bit] & [%d byte : %d bit] &  [%d byte : %d bit]\n", 	
																	NAND_EccError.uEccErrByte1, NAND_EccError.uEccErrBit1,
																	NAND_EccError.uEccErrByte2, NAND_EccError.uEccErrBit2,
																	NAND_EccError.uEccErrByte3, NAND_EccError.uEccErrBit3,
																	NAND_EccError.uEccErrByte4, NAND_EccError.uEccErrBit4,
																	NAND_EccError.uEccErrByte5, NAND_EccError.uEccErrBit5);
				}
				
				// Erase Block for test
				eError = NAND_EraseSingleBlock(g_NandContNum, uBlock);
				if(eError != eNAND_NoError)
				{
					NANDT_PrintErrorType(eError);
					NAND_Inform[g_NandContNum].uECCtest = 0;
					NAND_Inform[g_NandContNum].uAllBitEccCheck = 0;
					return;
				}

				// Write the invalid data to page( uBlock[block] 1[page] )
				eError = NAND_WritePageWithInvalidData(g_NandContNum, uBlock, uPage, aBuffer, uErrorType);
				if(eError != eNAND_NoError)
				{
					NANDT_PrintErrorType(eError);
					NAND_Inform[g_NandContNum].uECCtest = 0;
					NAND_Inform[g_NandContNum].uAllBitEccCheck = 0;
					return;
				}
					
				// Read the invalid data to detect the ECC error	
				eError = NAND_ReadPage(g_NandContNum, uBlock, uPage, aReadBuffer, aSpareBuffer);

				NANDT_PrintErrorType(eError);
				if(uErrorType == NAND_ECCERR_1BIT)
				{
					if( (NAND_EccError.uEccErrByte1 != aNANDT_EccError[0][0]) || (NAND_EccError.uEccErrBit1 != aNANDT_EccError[0][1]) )
					{
						UART_Printf("Ecc Error Check Error[%d byte:%d bit] != [%d byte:%d bit] \n", NAND_EccError.uEccErrByte1, NAND_EccError.uEccErrBit1,
																					aNANDT_EccError[0][0], aNANDT_EccError[0][1]);	
						UART_Getc();
						//NAND_Inform[g_NandContNum].uECCtest = 0;
						//NAND_Inform[g_NandContNum].uAllBitEccCheck = 0;
						//return;
					}
				}
				else if(uErrorType == NAND_ECCERR_2BIT)
				{
					if( 	((NAND_EccError.uEccErrByte1 != aNANDT_EccError[0][0]) || (NAND_EccError.uEccErrBit1 != aNANDT_EccError[0][1])) ||
						((NAND_EccError.uEccErrByte2 != aNANDT_EccError[1][0]) || (NAND_EccError.uEccErrBit2 != aNANDT_EccError[1][1])) )
					{
						UART_Printf("Ecc Error Check Error[%d byte:%d bit] != [%d byte:%d bit] \n", NAND_EccError.uEccErrByte1, NAND_EccError.uEccErrBit1,
																					aNANDT_EccError[0][0], aNANDT_EccError[0][1]);	
						UART_Printf("                     [%d byte:%d bit] != [%d byte:%d bit] \n", 	NAND_EccError.uEccErrByte2, NAND_EccError.uEccErrBit2,
																				aNANDT_EccError[1][0], aNANDT_EccError[1][1]);						
						UART_Getc();
						//NAND_Inform[g_NandContNum].uECCtest = 0;
						//NAND_Inform[g_NandContNum].uAllBitEccCheck = 0;
						//return;
					}
				}
				else if(uErrorType == NAND_ECCERR_3BIT)
				{
					if( 	((NAND_EccError.uEccErrByte1 != aNANDT_EccError[0][0]) || (NAND_EccError.uEccErrBit1 != aNANDT_EccError[0][1])) ||
						((NAND_EccError.uEccErrByte2 != aNANDT_EccError[1][0]) || (NAND_EccError.uEccErrBit2 != aNANDT_EccError[1][1])) ||
						((NAND_EccError.uEccErrByte3 != aNANDT_EccError[2][0]) || (NAND_EccError.uEccErrBit3 != aNANDT_EccError[2][1])) )
					{
						UART_Printf("Ecc Error Check Error[%d byte:%d bit] != [%d byte:%d bit] \n", NAND_EccError.uEccErrByte1, NAND_EccError.uEccErrBit1,
																					aNANDT_EccError[0][0], aNANDT_EccError[0][1]);	
						UART_Printf("                     [%d byte:%d bit] != [%d byte:%d bit] \n", 	NAND_EccError.uEccErrByte2, NAND_EccError.uEccErrBit2,
																				aNANDT_EccError[1][0], aNANDT_EccError[1][1]);
						UART_Printf("                     [%d byte:%d bit] != [%d byte:%d bit] \n", 	NAND_EccError.uEccErrByte3, NAND_EccError.uEccErrBit3,
																				aNANDT_EccError[2][0], aNANDT_EccError[2][1]);
						UART_Getc();
						//NAND_Inform[g_NandContNum].uECCtest = 0;
						//NAND_Inform[g_NandContNum].uAllBitEccCheck = 0;
						//return;
					}
				}
				else if(uErrorType == NAND_ECCERR_4BIT)
				{
					if( 	((NAND_EccError.uEccErrByte1 != aNANDT_EccError[0][0]) || (NAND_EccError.uEccErrBit1 != aNANDT_EccError[0][1])) ||
						((NAND_EccError.uEccErrByte2 != aNANDT_EccError[1][0]) || (NAND_EccError.uEccErrBit2 != aNANDT_EccError[1][1])) ||
						((NAND_EccError.uEccErrByte3 != aNANDT_EccError[2][0]) || (NAND_EccError.uEccErrBit3 != aNANDT_EccError[2][1])) ||
						((NAND_EccError.uEccErrByte4 != aNANDT_EccError[3][0]) || (NAND_EccError.uEccErrBit4 != aNANDT_EccError[3][1])) )
					{
						UART_Printf("Ecc Error Check Error[%d byte:%d bit] != [%d byte:%d bit] \n", NAND_EccError.uEccErrByte1, NAND_EccError.uEccErrBit1,
																					aNANDT_EccError[0][0], aNANDT_EccError[0][1]);	
						UART_Printf("                     [%d byte:%d bit] != [%d byte:%d bit] \n", 	NAND_EccError.uEccErrByte2, NAND_EccError.uEccErrBit2,
																				aNANDT_EccError[1][0], aNANDT_EccError[1][1]);
						UART_Printf("                     [%d byte:%d bit] != [%d byte:%d bit] \n", 	NAND_EccError.uEccErrByte3, NAND_EccError.uEccErrBit3,
																				aNANDT_EccError[2][0], aNANDT_EccError[2][1]);
						UART_Printf("                     [%d byte:%d bit] != [%d byte:%d bit] \n", 	NAND_EccError.uEccErrByte4, NAND_EccError.uEccErrBit4,
																				aNANDT_EccError[3][0], aNANDT_EccError[3][1]);
						UART_Getc();
						//NAND_Inform[g_NandContNum].uECCtest = 0;
						//NAND_Inform[g_NandContNum].uAllBitEccCheck = 0;
						//return;
					}
				}
				else if(uErrorType == NAND_ECCERR_UNCORRECT)
				{
					if( aNANDT_EccError[0][0] != NAND_ECC_UNCORRECT_ERROR)
					{
						UART_Printf("Uncorrectable Ecc Error Check Error\n");	
						aNANDT_EccError[0][0] = 0;
						UART_Getc();
						//NAND_Inform[g_NandContNum].uECCtest = 0;
						//NAND_Inform[g_NandContNum].uAllBitEccCheck = 0;
						//return;
					}
					aNANDT_EccError[0][0] = 0;
				}				

				for(l=0 ; l<4 ; l++)
					for(m=0 ; m<2 ; m++)
						aNANDT_EccError[l][m] = 0xFFFFFFFE;
					
				// Bit Pattern not Bit Position
				NAND_EccError.uEccErrBit1++;
				NAND_EccError.uEccErrBit2++;	
				NAND_EccError.uEccErrBit3++;	
				NAND_EccError.uEccErrBit4++;	
				NAND_EccError.uEccErrBit5++;	
			}

			NAND_EccError.uEccErrBit1 = 0;
			NAND_EccError.uEccErrBit2 = 0;	
			NAND_EccError.uEccErrBit3 = 0;	
			NAND_EccError.uEccErrBit4 = 0;	
			NAND_EccError.uEccErrBit5 = 0;	
			NAND_EccError.uEccErrByte1++;
			NAND_EccError.uEccErrByte2 = (NAND_EccError.uEccErrByte1+1);//NAND_Inform[g_NandContNum].uPageSize;
			NAND_EccError.uEccErrByte3 = (NAND_EccError.uEccErrByte2+1);//NAND_Inform[g_NandContNum].uPageSize;
			NAND_EccError.uEccErrByte4 = (NAND_EccError.uEccErrByte3+1);//NAND_Inform[g_NandContNum].uPageSize;
			NAND_EccError.uEccErrByte5 = (NAND_EccError.uEccErrByte4+1);//NAND_Inform[g_NandContNum].uPageSize;
		}
		
		NAND_EccError.uEccErrBit1 = 0;
		NAND_EccError.uEccErrBit2 = 0;	
		NAND_EccError.uEccErrBit3 = 0;	
		NAND_EccError.uEccErrBit4 = 0;	
		NAND_EccError.uEccErrBit5 = 0;	
		NAND_EccError.uEccErrByte1 = ((NAND_Inform[g_NandContNum].uMLCECCPageWriteSector+1)*NAND_MLC_TRANS_SIZE)
									+ ((NAND_Inform[g_NandContNum].uMLCECCPageWriteSector+1)*NAND_MLC_ECC_SIZE);
		NAND_EccError.uEccErrByte2 = (NAND_EccError.uEccErrByte1+1);//NAND_Inform[g_NandContNum].uPageSize;
		NAND_EccError.uEccErrByte3 = (NAND_EccError.uEccErrByte2+1);//NAND_Inform[g_NandContNum].uPageSize;
		NAND_EccError.uEccErrByte4 = (NAND_EccError.uEccErrByte3+1);//NAND_Inform[g_NandContNum].uPageSize;
		NAND_EccError.uEccErrByte5 = (NAND_EccError.uEccErrByte4+1);//NAND_Inform[g_NandContNum].uPageSize;
	}

	NAND_Inform[g_NandContNum].uECCtest = 0;
	NAND_Inform[g_NandContNum].uAllBitEccCheck = 0;
	
	UART_Printf("MLC Nand ECC test complete\n");
	UART_Printf("\n");	
}


const testFuncMenu nandecc_menu[] =
{
	NANDT_SLCECC,					"SLC ECC          ",
	NANDT_SLCSpareECC,			"SLC Spare ECC       ",
	NANDT_MLCECC,					"MLC ECC          ",

	NANDT_SLCECC_All,				"SLC All bit ECC          ",
	NANDT_SLCSpareECC_All,			"SLC All bit Spare  ECC      ",
	NANDT_MLCECC_All,				"MLC All bit ECC          ",	
	0, 0
};


void NANDT_ECC(void)
{
	u32 i;
	s32 uSel;
	
	while(1)
	{
		for (i=0; (u32)(nandecc_menu[i].desc)!=0; i++)
			UART_Printf("%2d: %s\n", i, nandecc_menu[i].desc);

		UART_Printf("\nSelect the function to test : ");
		uSel =UART_GetIntNum();
		UART_Printf("\n");
		if(uSel == -1) 
			break;

		if (uSel>=0 && uSel<(sizeof(nandecc_menu)/8-1))
			(nandecc_menu[uSel].func) ();
	}
}


//////////
// Function Name : NANDT_ReadPeformance
// Function Description : This function measures the performance of NAND
// Input : 	eTest - Manual test or Auto test selection
//			oAutoVar - Parameters for Auto-Test
// Output : 	None
void NANDT_ReadPeformance(eFunction_Test eTest, oFunctionT_AutoVar oAutoVar)
{
	u32 i, uBlock, uPage, uSize, uElapsedTime;
	float uPerformance;
	u32 uPageCnt;
	//u8 aBuffer[NAND_PAGE_MAX];
	//u8 aSpareBuffer[NAND_SPARE_MAX];
	u8 *aBuffer, *aSpareBuffer;
	NAND_eERROR eError;

	if(eTest == eTest_Manual)
		UART_Printf("[NANDT_ReadPeformance]\n");

	eError = eNAND_NoError;

	if(eTest == eTest_Manual)
	{
		UART_Printf("Input the Start Block Number to read[0~%d]", NAND_Inform[g_NandContNum].uBlockNum-1);
		uBlock = UART_GetIntNum();
		UART_Printf("Input the Size[unit : MByte(1024x1024)]");
		uSize = UART_GetIntNum();
	}
	else
	{
		uBlock = oAutoVar.Test_Parameter[0];
		uSize = oAutoVar.Test_Parameter[1];
	}
	
	aBuffer = (u8 *)malloc(NAND_PAGE_MAX);
	if(aBuffer == 0)
	{
		UART_Printf("Memory Allocation Error...\n");
		UART_Getc();
		return;
	}	
	aSpareBuffer = (u8 *)malloc(NAND_SPARE_MAX);
	if(aSpareBuffer == 0)
	{
		UART_Printf("Memory Allocation Error...\n");
		free(aBuffer);
		UART_Getc();
		return;
	}

	
	for(i=0 ; i<NAND_Inform[g_NandContNum].uPageSize ; i++)
		aBuffer[i] = 0;
	for(i=0 ; i<NAND_Inform[g_NandContNum].uSpareSize ; i++)
		aSpareBuffer[i] = 0xFF;

	uPageCnt = (uSize*1024*1024)/NAND_Inform[g_NandContNum].uPageSize;	
	uPage = 0;

	NAND_Inform[g_NandContNum].uPerformanceCheck = 1;
	NAND_Inform[g_NandContNum].uAllBitEccCheck = 0;
	NAND_Inform[g_NandContNum].uECCtest = 0;
	NAND_Inform[g_NandContNum].uSpareECCtest = 0;
	
	StartTimer(0);
	for(i=0 ; i<uPageCnt ; i++)
	{
		eError = NAND_ReadPageforPerform(g_NandContNum, uBlock, uPage, aBuffer, aSpareBuffer);

		if(eError != eNAND_NoError)
		{
			NANDT_PrintErrorType(eError);
			UART_Printf("Read page Error [%d block  %d page]\n", uBlock, uPage);
			UART_Getc();
			break;
		}

		uPage++;
		if(uPage == NAND_Inform[g_NandContNum].uPageNum)
		{
			uBlock++;
			uPage = 0;
		}
		
	}
	uElapsedTime = StopTimer(0);
	uPerformance = ((float)(uSize*1000000))/(float)uElapsedTime;

	UART_Printf("**************************************************\n");
	UART_Printf("Transfer size : %d MByte\n", uSize);
	UART_Printf("Elapsed time [usec] = %d\n", uElapsedTime);
	UART_Printf("Performance : %.04f MByte/sec\n", uPerformance);
	UART_Printf("**************************************************\n");
	UART_Printf("\n");

	NAND_Inform[g_NandContNum].uPerformanceCheck = 0;

	free(aBuffer);
	free(aSpareBuffer);
}



//////////
// Function Name : NANDT_WritePeformance
// Function Description : This function measures the performance of NAND
// Input : 	eTest - Manual test or Auto test selection
//			oAutoVar - Parameters for Auto-Test
// Output : 	None
void NANDT_WritePeformance(eFunction_Test eTest, oFunctionT_AutoVar oAutoVar)
{
	u32 i, uBlock, uPage, uSize, uElapsedTime;
	float uPerformance;
	u32 uBlockCnt, uPageCnt;
	//u8 aBuffer[NAND_PAGE_MAX];
	//u8 aSpareBuffer[NAND_SPARE_MAX];
	u8 *aBuffer, *aSpareBuffer;	
	NAND_eERROR eError;

	if(eTest == eTest_Manual)
		UART_Printf("[NANDT_WritePeformance]\n");

	eError = eNAND_NoError;

	if(eTest == eTest_Manual)
	{
		UART_Printf("Input the Start Block Number to write[0~%d]", NAND_Inform[g_NandContNum].uBlockNum-1);
		uBlock = UART_GetIntNum();
		UART_Printf("Input the Size[unit : MByte(1024x1024)]");
		uSize = UART_GetIntNum();
	}
	else
	{
		uBlock = oAutoVar.Test_Parameter[0];
		uSize = oAutoVar.Test_Parameter[1];
	}
	
	aBuffer = (u8 *)malloc(NAND_PAGE_MAX);
	if(aBuffer == 0)
	{
		UART_Printf("Memory Allocation Error...\n");
		UART_Getc();
		return;
	}	
	aSpareBuffer = (u8 *)malloc(NAND_SPARE_MAX);
	if(aSpareBuffer == 0)
	{
		UART_Printf("Memory Allocation Error...\n");
		free(aBuffer);
		UART_Getc();
		return;
	}
	
	for(i=0 ; i<NAND_Inform[g_NandContNum].uPageSize ; i++)
		aBuffer[i] = i;
	for(i=0 ; i<NAND_Inform[g_NandContNum].uSpareSize ; i++)
		aSpareBuffer[i] = 0xFF;

	uPageCnt = (uSize*1024*1024)/NAND_Inform[g_NandContNum].uPageSize;	
	uBlockCnt = uPageCnt/NAND_Inform[g_NandContNum].uPageNum + 1;
	//uEraseBlock = uBlock;
	uPage = 0;

	NAND_Inform[g_NandContNum].uPerformanceCheck = 1;
	NAND_Inform[g_NandContNum].uAllBitEccCheck = 0;
	NAND_Inform[g_NandContNum].uECCtest = 0;
	NAND_Inform[g_NandContNum].uSpareECCtest = 0;
	
	NAND_EraseMultiBlock(g_NandContNum, uBlock, uBlockCnt);
	
	StartTimer(0);
	for(i=0 ; i<uPageCnt ; i++)
	{
		eError = NAND_WritePageforPerform(g_NandContNum, uBlock, uPage, aBuffer, aSpareBuffer);

		if(eError != eNAND_NoError)
		{
			NANDT_PrintErrorType(eError);
			UART_Printf("Read page Error [%d block  %d page]\n", uBlock, uPage);
			UART_Getc();
		}

		uPage++;
		if(uPage == NAND_Inform[g_NandContNum].uPageNum)
		{
			uBlock++;
			uPage = 0;
		}
		
	}
	uElapsedTime = StopTimer(0);
	uPerformance = ((float)(uSize*1000000))/(float)uElapsedTime;

	UART_Printf("**************************************************\n");
	UART_Printf("Transfer size : %d MByte\n", uSize);
	UART_Printf("Elapsed time [usec] = %d\n", uElapsedTime);
	UART_Printf("Performance : %.04f MByte/sec\n", uPerformance);
	UART_Printf("**************************************************\n");
	UART_Printf("\n");

	//NAND_EraseMultiBlock(g_NandContNum, uEraseBlock, uBlockCnt);
	NAND_Inform[g_NandContNum].uPerformanceCheck = 0;

	free(aBuffer);
	free(aSpareBuffer);	
}


const AutotestFuncMenu nand_perform_menu[] =
{
	NANDT_ReadPeformance,			"Read Performance    ",
	NANDT_WritePeformance, 		"Write Performance    ",
	0, 0
};


void NANDT_Performance(void)
{
	u32 i;
	s32 uSel;
	oFunctionT_AutoVar oAutoTest;

	UART_Printf("\n[NANDT_Erase]\n");
	
	while(1)
	{
		UART_Printf("\n");
		for (i=0; (u32)(nand_perform_menu[i].desc)!=0; i++)
			UART_Printf("%2d: %s\n", i, nand_perform_menu[i].desc);

		UART_Printf("\nSelect the function to test : ");
		uSel =UART_GetIntNum();
		UART_Printf("\n");
		if(uSel == -1) 
			break;

		if (uSel>=0 && uSel<(sizeof(nand_perform_menu)/8-1))
			(nand_perform_menu[uSel].func) (eTest_Manual, oAutoTest);
	}
}


void NANDT_FullFunction(void)
{	
	u32 i, j, k, uNumClock;
	u32 uStartTestBlock, uEndTestBlock;
	//u32 uTacls, uTwrph0, uTwrph1, uTemp;
	oFunctionT_AutoVar oAutoTest;
						   //{Sync Mode, APLL, MPLL, HCLKx2 divide, HCLK divide, PCLK divide}
	u32 aCaseOfClock[][6] = { //Sync Mode
#if 0
							{eSYNC_MODE, eAPLL400M, eAPLL200M, 2, 2, 8}, 
							{eSYNC_MODE, eAPLL400M, eAPLL200M, 4, 2, 4},

							{eSYNC_MODE, eAPLL532M, eAPLL266M, 2, 2, 8},
							{eSYNC_MODE, eAPLL532M, eAPLL266M, 4, 2, 4},
#endif
						   	//Async Mode
							{eASYNC_MODE, eAPLL400M, eAPLL400M, 2, 2, 8},
							{eASYNC_MODE, eAPLL400M, eAPLL400M, 4, 2, 4},

							{eASYNC_MODE, eAPLL400M, eAPLL266M, 1, 2, 8},
							{eASYNC_MODE, eAPLL400M, eAPLL266M, 2, 2, 4},

							{eASYNC_MODE, eAPLL400M, eAPLL532M, 2, 2, 8},
							{eASYNC_MODE, eAPLL400M, eAPLL532M, 4, 2, 4},

							{eASYNC_MODE, eAPLL532M, eAPLL400M, 2, 2, 8},
							{eASYNC_MODE, eAPLL532M, eAPLL400M, 4, 2, 4},

							{eASYNC_MODE, eAPLL532M, eAPLL532M, 2, 2, 8},
							{eASYNC_MODE, eAPLL532M, eAPLL532M, 4, 2, 4},
	
							{eASYNC_MODE, eAPLL532M, eAPLL266M, 1, 2, 8},
							{eASYNC_MODE, eAPLL532M, eAPLL266M, 2, 2, 4}
	};
							
	uStartTestBlock = 400;
	uEndTestBlock = 419;
	
	uNumClock = sizeof(aCaseOfClock)/sizeof(aCaseOfClock[0]);

	for(i=0 ; i<uNumClock ; i++)
	{
#if 1	
		if(aCaseOfClock[i][0] == eSYNC_MODE)
		{
			SYSC_ChangeMode(eSYNC_MODE);
			SYSC_ChangeSYSCLK_1((APLL_eOUT)aCaseOfClock[i][1], (APLL_eOUT)aCaseOfClock[i][2], 0, 1, aCaseOfClock[i][5]-1);
			SYSC_SetDIV0(0, 1, aCaseOfClock[i][4]-1, aCaseOfClock[i][3]-1,  aCaseOfClock[i][5]-1, 1, 1,0, 1, 0);

			SYSC_GetClkInform();
			OpenConsole();

			UART_Printf("\n\n");
			UART_Printf("====================================================================\n");
			UART_Printf("Sync Mode : Synchronous\n");
			UART_Printf("ARMCLK : HCLK : PCLK = %d : %.02f : %.02f MHz\n", 
								g_ARMCLK/1000000, (float)g_HCLK/(float)1000000, (float)g_PCLK/(float)1000000);
		}
		else		// Async Mode
		{
			SYSC_ChangeMode(eASYNC_MODE);
			SYSC_ChangeSYSCLK_1((APLL_eOUT)aCaseOfClock[i][1], (APLL_eOUT)aCaseOfClock[i][2], 0, 1, aCaseOfClock[i][5]-1);
			SYSC_SetDIV0(0, 1, aCaseOfClock[i][4]-1, aCaseOfClock[i][3]-1,  aCaseOfClock[i][5]-1, 1, 1,0, 1, 0);

			SYSC_GetClkInform();
			OpenConsole();

			UART_Printf("\n\n");
			UART_Printf("====================================================================\n");
			UART_Printf("Sync Mode : Asynchronous\n");
			UART_Printf("ARMCLK : HCLK : PCLK = %d : %.02f : %.02f MHz (MPLL : %d MHz)\n", 
								g_ARMCLK/1000000, (float)g_HCLK/(float)1000000, (float)g_PCLK/(float)1000000, g_MPLL);			
		}
#endif		
		UART_Printf("\n");

		//Nand Timing Parameter Setting
		NAND_TimingParaSetting(g_NandContNum);

		UART_Printf("--------------------------------------------------------------------\n");
#if 	1			
		UART_Printf("Check Invalid Block\n");
		oAutoTest.Test_Parameter[0] = uStartTestBlock;		//write Block	
		oAutoTest.Test_Parameter[1] = uEndTestBlock;		//write Page
		NANDT_CheckInvalidBlockforAutoTest(eTest_Auto, oAutoTest);
		UART_Printf("Check Invalid Block....Completed[%d ~ %d]\n", uStartTestBlock, uEndTestBlock);

		UART_Printf("Erase Single Block\n");
		for(j=uStartTestBlock ; j<=uEndTestBlock ; j++)
		{
			oAutoTest.Test_Parameter[0] = j;	//erase start block	
			NANDT_EraseSingleBlock(eTest_Auto, oAutoTest);
		}
		UART_Printf("Erase Single Block....Completed [%d ~ %d]\n", uStartTestBlock, uEndTestBlock);

		UART_Printf("Write&Read Verifying");
		for(j=uStartTestBlock ; j<=uEndTestBlock ; j++)
			for(k=0 ; k<NAND_Inform[g_NandContNum].uPageNum ; k++)
			{
				oAutoTest.Test_Parameter[0] = j;		//write Block	
				oAutoTest.Test_Parameter[1] = k;		//write Page
				oAutoTest.Test_Parameter[2] = k;		//random value
				NANDT_WritePageVerify(eTest_Auto, oAutoTest);
				
				if((k!=0) && ((k%8)==7))
					UART_Printf("\b\b\b\b\b\b\b\b");
			}
		UART_Printf("\n");
		UART_Printf(" =>Write&Read Verify OK\n");
		
		UART_Printf("Write Performance\n");
		oAutoTest.Test_Parameter[0] = uStartTestBlock;	//Strat Block
		oAutoTest.Test_Parameter[1] = 5;				//Test Size[MBytes]
		NANDT_WritePeformance(eTest_Auto, oAutoTest);

		UART_Printf("Read Performance\n");
		oAutoTest.Test_Parameter[0] = uStartTestBlock;	//Strat Block
		oAutoTest.Test_Parameter[1] = 5;				//Test Size[MBytes]
		NANDT_ReadPeformance(eTest_Auto, oAutoTest);

		UART_Printf("Erase Multi Block\n");
		oAutoTest.Test_Parameter[0] = uStartTestBlock;					//Erase Strat Block
		oAutoTest.Test_Parameter[1] = uEndTestBlock-uStartTestBlock+1;		//Block Number erased
		NANDT_EraseMultiBlock(eTest_Auto, oAutoTest);
		UART_Printf("Erase Multi Block....Completed [%d ~ %d]\n", uStartTestBlock, uEndTestBlock);
		
#endif			
		UART_Printf("--------------------------------------------------------------------\n");
		UART_Printf("\n");
		
		UART_Printf("====================================================================\n");
	}
}


const testFuncMenu nand8bit_menu[] =
{
	NANDT_ReadID,					"Read ID          ",
	NANDT_Reset,                			"Reset command    ",		
	NANDT_CheckInvalidBlock,		"View Invalid Block   ",
	NANDT_ReadWrite, 				"Page Read&Write   ",
	NANDT_Erase,					"Block Erase      ", 
	NANDT_ProgramBinary,			"Binary program     ",
	NANDT_LockUnlock,				"Lock&Unlock          ",
	NANDT_ECC,						"ECC               ",
	NANDT_Performance,			"Performance    ",
	NANDT_FullFunction,				"Full Function       ",
	0, 0
};



///////////////////////////////////////////////////////////////////////////////////
////////////////////                     Normal 8bit Test                  /////////////////////////// 
///////////////////////////////////////////////////////////////////////////////////

void NandT_Normal8bit(void)
{
	u32 i;
	s32 uSel;

	UART_Printf("\n[NandT_Normal8bit]\n");

	g_NandContNum = 0;
	NAND_Inform[g_NandContNum].uNandType = NAND_Normal8bit;
	NAND_Inform[g_NandContNum].uAddrCycle = 4;
	NAND_Inform[g_NandContNum].uBlockNum = 4096;
	NAND_Inform[g_NandContNum].uPageNum = 32;
	NAND_Inform[g_NandContNum].uPageSize = NAND_PAGE_512;
	NAND_Inform[g_NandContNum].uSpareSize = NAND_SPARE_16;
	NAND_Inform[g_NandContNum].uECCtest = 0;

#if 0
	//K9S1208
	NAND_Inform[g_NandContNum].uTacls = 0;
	NAND_Inform[g_NandContNum].uTwrph0 = 35;
	//NAND_Inform[g_NandContNum].uTwrph0 = 35+10;	//Pad delay : about 10ns
	NAND_Inform[g_NandContNum].uTwrph1 = 10;	
#else
	//K9F1208U0B
	NAND_Inform[g_NandContNum].uTacls = 0;
	NAND_Inform[g_NandContNum].uTwrph0 = 25+10;	//Pad delay : about 10ns
	//NAND_Inform[g_NandContNum].uTwrph1 = 10;
	NAND_Inform[g_NandContNum].uTwrph1 = 15;//+5;	//tWH : 15ns
	
#endif

	NAND_Init(g_NandContNum);
	
	while(1)
	{
		for (i=0; (u32)(nand8bit_menu[i].desc)!=0; i++)
			UART_Printf("%2d: %s\n", i, nand8bit_menu[i].desc);

		UART_Printf("\nSelect the function to test : ");
		uSel =UART_GetIntNum();
		UART_Printf("\n");
		if(uSel == -1) 
			break;

		if (uSel>=0 && uSel<(sizeof(nand8bit_menu)/8-1))
			(nand8bit_menu[uSel].func) ();
	}
}


///////////////////////////////////////////////////////////////////////////////////
///////////////////                     Advanced 8bit Test                  ////////////////////////// 
///////////////////////////////////////////////////////////////////////////////////

void NandT_Advanced8bit(void)
{
	u32 i;
	s32 uSel;

	UART_Printf("\n[NandT_Advanced8bit]\n");

	g_NandContNum = 0;
	NAND_Inform[g_NandContNum].uNandType = NAND_Advanced8bit;
	NAND_Inform[g_NandContNum].uAddrCycle = 5;
	NAND_Inform[g_NandContNum].uBlockNum = 4096;
	NAND_Inform[g_NandContNum].uPageNum = 64;
	NAND_Inform[g_NandContNum].uPageSize = NAND_PAGE_2048;
	NAND_Inform[g_NandContNum].uSpareSize = NAND_SPARE_64;
	NAND_Inform[g_NandContNum].uECCtest = 0;

//	NAND_Inform[g_NandContNum].uTacls = 0;
//	NAND_Inform[g_NandContNum].uTwrph0 = 12+10;	//Pad delay : about 10ns
//	NAND_Inform[g_NandContNum].uTwrph1 = 5;

	NAND_Inform[g_NandContNum].uTacls = 0;
	NAND_Inform[g_NandContNum].uTwrph0 = 12+15;	//Pad delay : about 10ns
	NAND_Inform[g_NandContNum].uTwrph1 = 5;
	
	NAND_Init(g_NandContNum);
	
	while(1)
	{
		for (i=0; (u32)(nand8bit_menu[i].desc)!=0; i++)
			UART_Printf("%2d: %s\n", i, nand8bit_menu[i].desc);

		UART_Printf("\nSelect the function to test : ");
		uSel =UART_GetIntNum();
		UART_Printf("\n");
		if(uSel == -1) 
			break;

		if (uSel>=0 && uSel<(sizeof(nand8bit_menu)/8-1))
			(nand8bit_menu[uSel].func) ();
	}
}

///////////////////////////////////////////////////////////////////////////////////
//////////////////////                    MLC 8bit Test                  //////////////////////////// 
///////////////////////////////////////////////////////////////////////////////////

void NandT_MLC8bit(void)
{
	u32 i;
	s32 uSel;
	UART_Printf("\n[NandT_MLC8bit]\n");

	g_NandContNum = 0;
	NAND_Inform[g_NandContNum].uNandType = NAND_MLC8bit;
	NAND_Inform[g_NandContNum].uAddrCycle = 5;
	NAND_Inform[g_NandContNum].uBlockNum = 8196;
	NAND_Inform[g_NandContNum].uPageNum = 128;
	NAND_Inform[g_NandContNum].uPageSize = NAND_PAGE_2048;
	NAND_Inform[g_NandContNum].uSpareSize = NAND_SPARE_64;
	NAND_Inform[g_NandContNum].uECCtest = 0;

//	NAND_Inform[g_NandContNum].uTacls = 0;
//	NAND_Inform[g_NandContNum].uTwrph0 = 15+10;	//Pad delay : about 10ns
//	NAND_Inform[g_NandContNum].uTwrph1 = 5;

	NAND_Inform[g_NandContNum].uTacls = 0;
	NAND_Inform[g_NandContNum].uTwrph0 = 15+20;	//Pad delay : about 10ns
	//NAND_Inform[g_NandContNum].uTwrph1 = 5;
	NAND_Inform[g_NandContNum].uTwrph1 = 10+15;		//tWH : 10ns + margin
	
	NAND_Init(g_NandContNum);
	
	while(1)
	{
		for (i=0; (u32)(nand8bit_menu[i].desc)!=0; i++)
			UART_Printf("%2d: %s\n", i, nand8bit_menu[i].desc);

		UART_Printf("\nSelect the function to test : ");
		uSel =UART_GetIntNum();
		UART_Printf("\n");
		if(uSel == -1) 
			break;

		if (uSel>=0 && uSel<(sizeof(nand8bit_menu)/8-1))
			(nand8bit_menu[uSel].func) ();
	}
}



///////////////////////////////////////////////////////////////////////////////////
////////////////////                     Nand Main Test                  /////////////////////////// 
///////////////////////////////////////////////////////////////////////////////////

const testFuncMenu nand_menu[] =
{
	NandT_Normal8bit,					"K9F1208(Normal 8bit)",
	NandT_Advanced8bit,				"K9F2G08(Advanced 8bit)",
	NandT_MLC8bit,						"K9HBG08(MLC 8bit)",
	0, 0
};

void NAND_Test(void)
{
	u32 i;
	s32 uSel;

	UART_Printf("[NAND_Test]\n\n");

	//*(u32 *)0x7E00F120 = 0x0f;
	
	while(1)
	{
		for (i=0; (u32)(nand_menu[i].desc)!=0; i++)
			UART_Printf("%2d: %s\n", i, nand_menu[i].desc);

		UART_Printf("\nSelect the function to test : ");
		uSel =UART_GetIntNum();
		UART_Printf("\n");
		if(uSel == -1) 
			break;

		if (uSel>=0 && uSel<(sizeof(nand_menu)/8-1))
			(nand_menu[uSel].func) ();
	}
	
}	

///////////////////////////////////////////////////////////////////////////////////
//////////                     Nand Extra Test Functions for EBI Test                  ///////////////////
///////////////////////////////////////////////////////////////////////////////////
void NANDT_Init(void)
{
	UART_Printf("\n[NandT_Normal8bit]\n");

	g_NandContNum = 0;
	NAND_Inform[g_NandContNum].uNandType = NAND_Normal8bit;
	NAND_Inform[g_NandContNum].uAddrCycle = 4;
	NAND_Inform[g_NandContNum].uBlockNum = 4096;
	NAND_Inform[g_NandContNum].uPageNum = 32;
	NAND_Inform[g_NandContNum].uPageSize = NAND_PAGE_512;
	NAND_Inform[g_NandContNum].uSpareSize = NAND_SPARE_16;
	NAND_Inform[g_NandContNum].uECCtest = 0;

	//K9F1208U0B
	NAND_Inform[g_NandContNum].uTacls = 10;
	NAND_Inform[g_NandContNum].uTwrph0 = 35+10;	//Pad delay : about 10ns
	NAND_Inform[g_NandContNum].uTwrph1 = 15+15;		//tWH : 15ns

	NAND_Init(g_NandContNum);

}


