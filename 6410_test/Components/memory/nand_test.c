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
//#include "def.h"
//#include "timer.h"
#include "dma.h"


/////////////////////////// 6410 IROM Test ////////////////////////////////////////////////
volatile unsigned char *downPt;
volatile unsigned char *downPt2;
#define DOWNLOAD_ADDRESS (0x55000000)
// chapter7 Nand Flash
#define NANDF_BASE		(0x70200000)
#define rNFCONF			(*(volatile unsigned *)(NANDF_BASE+0x00))		//NAND Flash configuration
#define rNFCONT			(*(volatile unsigned *)(NANDF_BASE+0x04))      	//NAND Flash control
#define rNFCMD			(*(volatile unsigned *)(NANDF_BASE+0x08))     		//NAND Flash command 
#define rNFADDR			(*(volatile unsigned *)(NANDF_BASE+0x0C))     	 	//NAND Flash address
#define rNFDATA			(*(volatile unsigned *)(NANDF_BASE+0x10))      	//NAND Flash data
#define rNFDATA8		(*(volatile unsigned char*)(NANDF_BASE+0x10))      	//NAND Flash data  
#define NFDATA			(NANDF_BASE+0x10)                        
#define rNFMECCD0		(*(volatile unsigned *)(NANDF_BASE+0x14))      	//NAND Flash ECC for Main 
#define rNFMECCD1		(*(volatile unsigned *)(NANDF_BASE+0x18))      	//NAND Flash ECC for Main 
#define rNFSECCD		(*(volatile unsigned *)(NANDF_BASE+0x1C))	  	//NAND Flash ECC for Spare Area
#define rNFSBLK 			(*(volatile unsigned *)(NANDF_BASE+0x20))		//NAND Flash programmable start block address
#define rNFEBLK 			(*(volatile unsigned *)(NANDF_BASE+0x24))	    	//NAND Flash programmable end block address     
#define rNFSTAT 			(*(volatile unsigned *)(NANDF_BASE+0x28))      	//NAND Flash operation status 
#define rNFECCERR0		(*(volatile unsigned *)(NANDF_BASE+0x2C))      	//NAND Flash ECC Error Status for I/O [7:0]
#define rNFECCERR1		(*(volatile unsigned *)(NANDF_BASE+0x30))      	//NAND Flash ECC Error Status for I/O [15:8]
#define rNFMECC0		(*(volatile unsigned *)(NANDF_BASE+0x34))      	//SLC or MLC NAND Flash ECC status
#define rNFMECC1		(*(volatile unsigned *)(NANDF_BASE+0x38))	    	//SLC or MLC NAND Flash ECC status	
#define rNFSECC 			(*(volatile unsigned *)(NANDF_BASE+0x3C))  		//NAND Flash ECC for I/O[15:0]
#define rNFMLCBITPT		(*(volatile unsigned *)(NANDF_BASE+0x40)) 		//NAND Flash 4-bit ECC Error Pattern for data[7:0]
#define rNF8ECCERR0		(*(volatile unsigned *)(NANDF_BASE+0x44)) 		//NAND Flash ECC Error Status register 0
#define rNF8ECCERR1		(*(volatile unsigned *)(NANDF_BASE+0x48)) 		//NAND Flash ECC Error Status register 1
#define rNF8ECCERR2		(*(volatile unsigned *)(NANDF_BASE+0x4c)) 		//NAND Flash ECC Error Status register 2
#define rNFM8ECC0		(*(volatile unsigned *)(NANDF_BASE+0x50)) 		//8Bit ECC Status
#define rNFM8ECC1		(*(volatile unsigned *)(NANDF_BASE+0x54)) 		//8Bit ECC Status
#define rNFM8ECC2		(*(volatile unsigned *)(NANDF_BASE+0x58)) 		//8Bit ECC Status
#define rNFM8ECC3		(*(volatile unsigned *)(NANDF_BASE+0x5c)) 		//8Bit ECC Status
#define rNFMLC8BITPT0	(*(volatile unsigned *)(NANDF_BASE+0x60)) 		//NAND Flash 8-bit ECC Error Pattern register 0 for data[7:0]
#define rNFMLC8BITPT1	(*(volatile unsigned *)(NANDF_BASE+0x64)) 		//NAND Flash 8-bit ECC Error Pattern register 0 for data[7:0]
#define NF_MECC_UnLock2()         	{rNFCONT&=~(1<<7);}
#define NF_MECC_Lock2()         		{rNFCONT|=(1<<7);}

#define NF_SECC_UnLock2()         	{rNFCONT&=~(1<<6);}
#define NF_SECC_Lock2()         		{rNFCONT|=(1<<6);}

#define NF_CMD2(cmd)				{rNFCMD=cmd;}
#define NF_ADDR2(addr)			{rNFADDR=addr;}
	
#define NF_nFCE_L2()				{rNFCONT&=~(1<<1);} 
#define NF_nFCE_H2()				{rNFCONT|=(1<<1);}

#define NF_RSTECC2()				{rNFCONT|=(1<<5|1<<4);}
#define NF_RDDATA2() 				(rNFDATA)
#define NF_RDDATA82() 			((*(volatile unsigned char*)0x70200010) )

#define NF_WRDATA2(data) 		{rNFDATA=data;}
#define NF_WRDATA82(data) 		{rNFDATA8=data;}


#define NF_WAITRB2()    			{while(!(rNFSTAT&(1<<4)));} 
	   							 //wait tWB and check F_RNB pin.
// RnB Signal
#define NF_CLEAR_RB2()    			{rNFSTAT |= (1<<4);}	// Have write '1' to clear this bit.
#define NF_DETECT_RB2()    		{while(!(rNFSTAT&(1<<4)));}



//The all bit ecc check of 1page
#define NAND_SLCECC_ALLBIT_CHECK	(TRUE)
#define NAND_ECC_MULTI_ERROR			(0xFFFFFFFF)
#define NAND_ECC_UNCORRECT_ERROR		(0xFFFFFFFF)

extern NAND_oInform	NAND_Inform[NAND_CONNUM];
extern NAND_oEccError NAND_EccError;
u32 g_NandContNum=0;
extern volatile u32 g_Nand_IllegalAccErrorforTest;
extern volatile u32 gNandAutoTest;
static u32 aNANDT_EccError[4][2];		// 4bit error, byte pos & bit pos

// for DMA test Zeus 070829
#if 0 
volatile int g_DmaDone;
volatile int g_DmaDone0, g_DmaDone1, g_DmaDone2, g_DmaDone3;
#endif
//static DMAC oDmac0, oDmac1, oDmac2, oDmac3;



void NANDT_InsertSeed(void)
{
	u32 uSeed;
	u32 uDate, uHour, uMin, uSec;

	uHour = *(unsigned volatile *)0x7e006078;
	uDate = *(unsigned volatile *)0x7e00607c;
	uMin = *(unsigned volatile *)0x7e006074;
	uSec = *(unsigned volatile *)0x7e006070;
	NAND_Inform[0].uRandomCount++;
	uSeed = (uDate*24*60*60)+(uHour*60*60)+(uMin*60)+uSec+NAND_Inform[0].uRandomCount ;
	srand(uSeed);	
}


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
// Function Name : NANDT_PrintErrorType
// Function Description : Print NAND Error Type
// Input : 	eError - Nand Error enumeration variable
// Output :    	None
void NANDT_PrintErrorType_Sample(NAND_eERROR eError)
{
	u32 uEccErr0, uEccErr1, uEccErr2;
//	u32 uErrorType0;
	UART_Printf("---------------- Error Issued --------------\n");

	switch(NAND_Inform[g_NandContNum].uECCType)
	{
		case ECC1Bit:
			NAND_ReadECCErrorData(g_NandContNum, &uEccErr0, &uEccErr1, &uEccErr2);
			NAND_CheckECCPositoin_8BitAll(g_NandContNum, &uEccErr0, &uEccErr1, &uEccErr2);				
			break;
		case ECC4Bit:
			NAND_ReadECCErrorData(g_NandContNum, &uEccErr0, &uEccErr1, &uEccErr2);
			NAND_CheckECCPositoin(g_NandContNum, &uEccErr0, &uEccErr1, &uEccErr2,eError);			
//			NAND_CorrecrErrorBit_4BitECC(g_NandContNum, &uEccErr0, &uEccErr1, &uEccErr2,eError);
			break;
		case ECC8Bit:
			NAND_ReadECCErrorData(g_NandContNum, &uEccErr0, &uEccErr1, &uEccErr2);
			NAND_CheckECCPositoin(g_NandContNum, &uEccErr0, &uEccErr1, &uEccErr2,eError);
//			NAND_CorrectErrorData();
			break;
		default:
			break;
	}
	UART_Printf("-------------------------------------------\n");		

}


//////////
// Function Name : NANDT_PrintErrorType
// Function Description : Print NAND Error Type
// Input : 	eError - Nand Error enumeration variable
// Output :    	None
void NANDT_PrintErrorType_Sample_8BitALLBit(NAND_eERROR eError)
{
	u32 uEccErr0, uEccErr1, uEccErr2;
//	u32 uErrorType;
	UART_Printf("---------------- Error Issued --------------\n");

	switch(NAND_Inform[g_NandContNum].uECCType)
	{
		case ECC1Bit:
			UART_Printf("Later... ^^\n");
			break;
		case ECC4Bit:
			NAND_ReadECCErrorData(g_NandContNum, &uEccErr0, &uEccErr1, &uEccErr2);
			NAND_CheckECCPositoin_8BitAll(g_NandContNum, &uEccErr0, &uEccErr1, &uEccErr2);
			break;
		case ECC8Bit:
			NAND_ReadECCErrorData(g_NandContNum, &uEccErr0, &uEccErr1, &uEccErr2);
			NAND_CheckECCPositoin_8BitAll(g_NandContNum, &uEccErr0, &uEccErr1, &uEccErr2);
//			NAND_CorrectErrorData();
			break;
		default:
			break;
	}
	UART_Printf("-------------------------------------------\n");		

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
			UART_GetKey();
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
//	eError = NAND_ReadPage_1Bit(g_NandContNum, uBlock, uPage, aBuffer, aSpareBuffer);

	if(eError != eNAND_NoError)
	{
		NANDT_PrintErrorType(eError);
		UART_Printf("Read page Error [%d block  %d page]\n", uBlock, uPage);
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
// Function Name : NANDT_ReadPage_8Bit
// Function Description : Read 1 page data
// Input : 	eTest - Manual test or Auto test selection
//			oAutoVar - Parameters for Auto-Test
// Output : 	None
void NANDT_ReadPage_8Bit(eFunction_Test eTest, oFunctionT_AutoVar oAutoVar)
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
		UART_GetKey();
		return;
	}	
	aSpareBuffer = (u8 *)malloc(NAND_SPARE_MAX);
	if(aSpareBuffer == 0)
	{
		UART_Printf("Memory Allocation Error...\n");
		free(aBuffer);
		UART_GetKey();
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
	
	eError = NAND_ReadPage_8Bit(g_NandContNum, uBlock, uPage, aBuffer, aSpareBuffer);

	if(eError != eNAND_NoError)
	{
		NANDT_PrintErrorType(eError);
		UART_Printf("Read page Error [%d block  %d page]\n", uBlock, uPage);
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
// Function Name : NANDT_ReadPage_8Bit
// Function Description : Read 1 page data
// Input : 	eTest - Manual test or Auto test selection
//			oAutoVar - Parameters for Auto-Test
// Output : 	None
void NANDT_Read2048Page_8Bit(eFunction_Test eTest, oFunctionT_AutoVar oAutoVar)
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
		UART_GetKey();
		return;
	}	
	aSpareBuffer = (u8 *)malloc(NAND_SPARE_MAX);
	if(aSpareBuffer == 0)
	{
		UART_Printf("Memory Allocation Error...\n");
		free(aBuffer);
		UART_GetKey();
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
	
	eError = NAND_Read2048Page_8Bit(g_NandContNum, uBlock, uPage, aBuffer, aSpareBuffer);

	if(eError != eNAND_NoError)
	{
		NANDT_PrintErrorType(eError);
		UART_Printf("Read page Error [%d block  %d page]\n", uBlock, uPage);
		UART_Printf("uMLCECCPageReadSector = %d\n", NAND_Inform[0].uMLCECCPageReadSector );
	#if 1
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
	#endif
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


void NANDT_Read4096Page_8Bit(eFunction_Test eTest, oFunctionT_AutoVar oAutoVar)
{
	u32 i, uBlock, uPage;
	//u8 aBuffer[NAND_PAGE_MAX];
	//u8 aSpareBuffer[NAND_SPARE_MAX];
	u8 *aBuffer, *aSpareBuffer;
	NAND_eERROR eError;
	NAND_Inform[g_NandContNum].uPageSize  = 4096;
	NAND_Inform[g_NandContNum].uSpareSize = 128;

	if(eTest == eTest_Manual)
		UART_Printf("[NANDT_ReadPage]\n");

	aBuffer = (u8 *)malloc(4096);
	if(aBuffer == 0)
	{
		UART_Printf("Memory Allocation Error...\n");
		UART_GetKey();
		return;
	}	
	aSpareBuffer = (u8 *)malloc(128);
	if(aSpareBuffer == 0)
	{
		UART_Printf("Memory Allocation Error...\n");
		free(aBuffer);
		UART_GetKey();
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
	
	eError = NAND_Read4096Page_8Bit(g_NandContNum, uBlock, uPage, aBuffer, aSpareBuffer);

	if(eError != eNAND_NoError)
	{
		NANDT_PrintErrorType(eError);
		UART_Printf("Read page Error [%d block  %d page]\n", uBlock, uPage);
		UART_Printf("uMLCECCPageReadSector = %d\n", NAND_Inform[0].uMLCECCPageReadSector );
	#if 1
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
	#endif
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
// Function Name : NANDT_ReadPage_4Bit
// Function Description : Read 1 page data
// Input : 	eTest - Manual test or Auto test selection
//			oAutoVar - Parameters for Auto-Test
// Output : 	None
void NANDT_ReadPage_4Bit(eFunction_Test eTest, oFunctionT_AutoVar oAutoVar)
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
		UART_GetKey();
		return;
	}	
	aSpareBuffer = (u8 *)malloc(NAND_SPARE_MAX);
	if(aSpareBuffer == 0)
	{
		UART_Printf("Memory Allocation Error...\n");
		free(aBuffer);
		UART_GetKey();
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
	
	eError = NAND_ReadPage_4Bit(g_NandContNum, uBlock, uPage, aBuffer, aSpareBuffer);

	if(eError != eNAND_NoError)
	{
		NANDT_PrintErrorType(eError);
		UART_Printf("Read page Error [%d block  %d page]\n", uBlock, uPage);
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
		aSpareBuffer[i] = 0x0;	
	
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
// Function Name : NANDT_WritePage_4Bit
// Function Description : Write 1 page data
// Input : 	eTest - Manual test or Auto test selection
//			oAutoVar - Parameters for Auto-Test
// Output : 	None
void NANDT_WritePage_4Bit(eFunction_Test eTest, oFunctionT_AutoVar oAutoVar)
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
		UART_GetKey();
		return;
	}	
	aSpareBuffer = (u8 *)malloc(NAND_SPARE_MAX);
	if(aSpareBuffer == 0)
	{
		UART_Printf("Memory Allocation Error...\n");
		free(aBuffer);
		UART_GetKey();
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
		aBuffer[i] = 0xff;
	}
#endif

	for(i=0 ; i<NAND_Inform[g_NandContNum].uSpareSize ; i++)
		aSpareBuffer[i] = 0x0;	
	
	eError = NAND_WritePage_4Bit(g_NandContNum, uBlock, uPage, aBuffer, aSpareBuffer);

	if(eError != eNAND_NoError)
	{
		NANDT_PrintErrorType(eError);
//		UART_GetKey();
		
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
// Function Name : NANDT_WritePage_8bit_NBoot
// Function Description : Write 1 page data
// Input : 	eTest - Manual test or Auto test selection
//			oAutoVar - Parameters for Auto-Test
// Output : 	None
void NANDT_WritePage_8bit_NBoot(eFunction_Test eTest, oFunctionT_AutoVar oAutoVar)
{
	u32 i, uBlock, uPage;
//	u8 uOffset;
	//u8 aBuffer[NAND_PAGE_MAX],  aSpareBuffer[NAND_SPARE_MAX];	
	u8 *aBuffer, *aSpareBuffer;
	u8 *aReadBuffer, *aReadSpareBuffer;
	NAND_eERROR eError;
	u32 uIndex;

	UART_Printf("[NANDT_WritePage]\n");
	UART_Printf("0 Block 0~15Page : 8K Data Format : 0xAAAA5555\n");	

	aBuffer = (u8 *)malloc(NAND_PAGE_MAX);
	if(aBuffer == 0)
	{
		UART_Printf("Memory Allocation Error...\n");
		UART_GetKey();
		return;
	}	
	aSpareBuffer = (u8 *)malloc(NAND_SPARE_MAX);
	if(aSpareBuffer == 0)
	{
		UART_Printf("Memory Allocation Error...\n");
		free(aBuffer);
		UART_GetKey();
		return;
	}

	for ( uIndex = 0 ; uIndex<16 ; uIndex++)
	{

		for(i=0 ; i<NAND_Inform[g_NandContNum].uPageSize ; i++)
		{
			aBuffer[i] = 0xa5;
		}

		for(i=0 ; i<NAND_Inform[g_NandContNum].uSpareSize ; i++)
			aSpareBuffer[i] = 0x0;	

		uBlock = 0 ; 
		uPage = uIndex; 
		
		eError = NAND_WritePage_8Bit(g_NandContNum, uBlock, uPage, aBuffer, aSpareBuffer);

		if(eError != eNAND_NoError)
		{
			NANDT_PrintErrorType(eError);
			
		}
		else
		{
			UART_Printf("Write Page -- Success -- [%2d Block, %4d Page]\n", uBlock, uPage);
		}

		eError = NAND_ReadPage_8Bit(g_NandContNum, uBlock, uPage, aReadBuffer, aReadSpareBuffer);

		if(eError != eNAND_NoError)
		{
			NANDT_PrintErrorType(eError);
			UART_Printf("Read page Error [%d block  %d page]\n", uBlock, uPage);
		}
		else
		{
			UART_Printf("Read page -- Success -- [%2d block  %4d page]\n", uBlock, uPage);
		}

		for(i=0 ; i<NAND_Inform[g_NandContNum].uPageSize ; i++)
		{
			if ( aBuffer[i] != aReadBuffer[i] )
			{
				UART_Printf("Compare Data Error [%d block  %d page]\n", uBlock, uPage);				
				UART_Printf("Check This Condition! and Enter any Key!\n");
				UART_Getc();
			}
		}

		UART_Printf("-------------------------------------------------\n");

	}

	free(aBuffer);
	free(aSpareBuffer);

}


//////////
// Function Name : NANDT_WritePage_8bit
// Function Description : Write 1 page data
// Input : 	eTest - Manual test or Auto test selection
//			oAutoVar - Parameters for Auto-Test
// Output : 	None
void NANDT_WritePage_8bit(eFunction_Test eTest, oFunctionT_AutoVar oAutoVar)
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
		UART_GetKey();
		return;
	}	
	aSpareBuffer = (u8 *)malloc(NAND_SPARE_MAX);
	if(aSpareBuffer == 0)
	{
		UART_Printf("Memory Allocation Error...\n");
		free(aBuffer);
		UART_GetKey();
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
		aBuffer[i] = 0xff;
	}
#endif

	for(i=0 ; i<NAND_Inform[g_NandContNum].uSpareSize ; i++)
		aSpareBuffer[i] = 0x0;	
	
	eError = NAND_WritePage_8Bit(g_NandContNum, uBlock, uPage, aBuffer, aSpareBuffer);

	if(eError != eNAND_NoError)
	{
		NANDT_PrintErrorType(eError);
//		UART_GetKey();
		
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
// Function Name : NANDT_WritePage_8bit
// Function Description : Write 1 page data
// Input : 	eTest - Manual test or Auto test selection
//			oAutoVar - Parameters for Auto-Test
// Output : 	None
void NANDT_Write2048Page_8bit(eFunction_Test eTest, oFunctionT_AutoVar oAutoVar)
{
	u32 i, uBlock, uPage;
	u8 uOffset;
	u8 *aBuffer, *aSpareBuffer;
	NAND_eERROR eError;


	UART_Printf("[NANDT_WritePage]\n");
	UART_Printf("Input the Block Number to write[0~%d] : ", NAND_Inform[g_NandContNum].uBlockNum-1);
	uBlock = UART_GetIntNum();
	UART_Printf("Input the Page Number to write[0~%d] : ", NAND_Inform[g_NandContNum].uPageNum-1);
	uPage = UART_GetIntNum();
	UART_Printf("Input the srand() offset data : ");
	uOffset = (u8)UART_GetIntNum();
	UART_Printf("\n");
	
	aBuffer = (u8 *)malloc(NAND_PAGE_MAX);
	if(aBuffer == 0)
	{
		UART_Printf("Memory Allocation Error...\n");
		UART_GetKey();
		return;
	}	
	aSpareBuffer = (u8 *)malloc(NAND_SPARE_MAX);
	if(aSpareBuffer == 0)
	{
		UART_Printf("Memory Allocation Error...\n");
		free(aBuffer);
		UART_GetKey();
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
		aBuffer[i] = 0x0;
	}
#endif

	for(i=0 ; i<NAND_Inform[g_NandContNum].uSpareSize ; i++)
		aSpareBuffer[i] = 0x0;	
	
	eError = NAND_Write2048Page_8Bit(g_NandContNum, uBlock, uPage, aBuffer, aSpareBuffer);

	if(eError != eNAND_NoError)
	{
		NANDT_PrintErrorType(eError);
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

void NANDT_Write4096Page_8bit(eFunction_Test eTest, oFunctionT_AutoVar oAutoVar)
{
	u32 i, uBlock, uPage;
	u8 uOffset;
	u8 *aBuffer, *aSpareBuffer;
	NAND_eERROR eError;
	NAND_Inform[g_NandContNum].uPageSize = 4096;
	NAND_Inform[g_NandContNum].uSpareSize = 128;

	UART_Printf("[NANDT_WritePage]\n");
	UART_Printf("Input the Block Number to write[0~%d] : ", NAND_Inform[g_NandContNum].uBlockNum-1);
	uBlock = UART_GetIntNum();
	UART_Printf("Input the Page Number to write[0~%d] : ", NAND_Inform[g_NandContNum].uPageNum-1);
	uPage = UART_GetIntNum();
	UART_Printf("Input the srand() offset data : ");
	uOffset = (u8)UART_GetIntNum();
	UART_Printf("\n");
	
	aBuffer = (u8 *)malloc(4096);
	if(aBuffer == 0)
	{
		UART_Printf("Memory Allocation Error...\n");
		UART_GetKey();
		return;
	}	
	aSpareBuffer = (u8 *)malloc(128);
	if(aSpareBuffer == 0)
	{
		UART_Printf("Memory Allocation Error...\n");
		free(aBuffer);
		UART_GetKey();
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
		aBuffer[i] = 0x0;
	}
#endif

	for(i=0 ; i<NAND_Inform[g_NandContNum].uSpareSize ; i++)
		aSpareBuffer[i] = 0x0;	
	
	eError = NAND_Write4096Page_8Bit(g_NandContNum, uBlock, uPage, aBuffer, aSpareBuffer);

	if(eError != eNAND_NoError)
	{
		NANDT_PrintErrorType(eError);
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
void NANDT_WritePageVerify_8BitECC(eFunction_Test eTest, oFunctionT_AutoVar oAutoVar)
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

	NAND_Inform[0].uECCtest == 0;
	eError = NAND_WritePage_8Bit(g_NandContNum, uBlock, uPage, aBuffer, aSpareBuffer);

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

	NAND_Inform[0].uSpareECCtest == 0;
	eError = NAND_ReadPage_8Bit(g_NandContNum, uBlock, uPage, pReadBuffer, pReadSpareBuffer);
	//eError = NAND_ReadPage_1Bit(g_NandContNum, uBlock, uPage, pReadBuffer, pReadSpareBuffer);	

	if(eError != eNAND_NoError)
	{
		NANDT_PrintErrorType(eError);
		UART_Printf("Read page Error [%d block  %d page]\n", uBlock, uPage);
		if(!((eError == eNAND_1bitEccError) || (eError == eNAND_2bitEccError) || (eError == eNAND_3bitEccError) || (eError == eNAND_4bitEccError) ))
		{
			UART_Getc();
		free(aBuffer);
		free(aSpareBuffer);
		free(pReadBuffer);
		free(pReadSpareBuffer);
		return;
	        }
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

//////////
// Function Name : NANDT_WritePageVerify
// Function Description : Write 1 page data & Verfy
// Input : 	eTest - Manual test or Auto test selection
//			oAutoVar - Parameters for Auto-Test
// Output : 	None
void NANDT_WritePageVerify_4096_8BitECC(eFunction_Test eTest, oFunctionT_AutoVar oAutoVar)
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

	NAND_Inform[0].uECCtest == 0;
	eError = NAND_Write4096Page_8Bit(g_NandContNum, uBlock, uPage, aBuffer, aSpareBuffer);

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

	NAND_Inform[0].uSpareECCtest == 0;
	eError = NAND_Read4096Page_8Bit(g_NandContNum, uBlock, uPage, pReadBuffer, pReadSpareBuffer);
	//eError = NAND_ReadPage_1Bit(g_NandContNum, uBlock, uPage, pReadBuffer, pReadSpareBuffer);	

	if(eError != eNAND_NoError)
	{
		NANDT_PrintErrorType(eError);
		UART_Printf("Read page Error [%d block  %d page]\n", uBlock, uPage);
		if(!((eError == eNAND_1bitEccError) || (eError == eNAND_2bitEccError) || (eError == eNAND_3bitEccError) || (eError == eNAND_4bitEccError) ))
		{
			UART_Getc();
		free(aBuffer);
		free(aSpareBuffer);
		free(pReadBuffer);
		free(pReadSpareBuffer);
		return;
	        }
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


//////////
// Function Name : NANDT_WritePageVerify
// Function Description : Write 1 page data & Verfy
// Input : 	eTest - Manual test or Auto test selection
//			oAutoVar - Parameters for Auto-Test
// Output : 	None
void NANDT_WritePageVerify_2048_8BitECC(eFunction_Test eTest, oFunctionT_AutoVar oAutoVar)
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

	NAND_Inform[0].uECCtest == 0;
	eError = NAND_Write2048Page_8Bit(g_NandContNum, uBlock, uPage, aBuffer, aSpareBuffer);

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

	NAND_Inform[0].uSpareECCtest == 0;
	eError = NAND_Read2048Page_8Bit(g_NandContNum, uBlock, uPage, pReadBuffer, pReadSpareBuffer);
	//eError = NAND_ReadPage_1Bit(g_NandContNum, uBlock, uPage, pReadBuffer, pReadSpareBuffer);	

	if(eError != eNAND_NoError)
	{
		NANDT_PrintErrorType(eError);
		UART_Printf("Read page Error [%d block  %d page]\n", uBlock, uPage);
		if(!((eError == eNAND_1bitEccError) || (eError == eNAND_2bitEccError) || (eError == eNAND_3bitEccError) || (eError == eNAND_4bitEccError) ))
		{
			UART_Getc();
		free(aBuffer);
		free(aSpareBuffer);
		free(pReadBuffer);
		free(pReadSpareBuffer);
		return;
	        }
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

	NAND_Inform[0].uECCtest == 0;
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

	NAND_Inform[0].uSpareECCtest == 0;
	eError = NAND_ReadPage(g_NandContNum, uBlock, uPage, pReadBuffer, pReadSpareBuffer);
	//eError = NAND_ReadPage_1Bit(g_NandContNum, uBlock, uPage, pReadBuffer, pReadSpareBuffer);	

	if(eError != eNAND_NoError)
	{
		NANDT_PrintErrorType(eError);
		UART_Printf("Read page Error [%d block  %d page]\n", uBlock, uPage);
		if(!((eError == eNAND_1bitEccError) || (eError == eNAND_2bitEccError) || (eError == eNAND_3bitEccError) || (eError == eNAND_4bitEccError) ))
		{
			UART_Getc();
		free(aBuffer);
		free(aSpareBuffer);
		free(pReadBuffer);
		free(pReadSpareBuffer);
		return;
	        }
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
	NANDT_ReadPage,                			"Read Single Page      ",	
	NANDT_WritePageVerify,				"Write & Verify Single Page     \n",
	NANDT_WritePage_4Bit,                        "[512 PageSize] Write Single Page_4BitECC     ",
	NANDT_ReadPage_4Bit,                         "[512 PageSize] Read Single Page_4BitECC     \n",
	
	NANDT_WritePage_8bit,                        "[512 PageSize] Write Single Page_8BitECC      ",
	NANDT_ReadPage_8Bit,                         "[512 PageSize] Read Single Page_8BitECC       ",
	NANDT_WritePageVerify_8BitECC,         "[512 PageSize] Write Verify Page_8BitECC       \n",
	
	NANDT_Read2048Page_8Bit,                         "[2048PageSize] Read Page_8BitECC       ",
	NANDT_Write2048Page_8bit,                         "[2048PageSize] Write Page_8BitECC       ",
	NANDT_WritePageVerify_2048_8BitECC,        "[2048PageSize] Write Verify 8BitECC       \n",
	
	NANDT_Read4096Page_8Bit,                         "[4096PageSize] Read Page_8BitECC       ",
	NANDT_Write4096Page_8bit,                         "[4096PageSize] Write Page_8BitECC       ",	
	NANDT_WritePageVerify_4096_8BitECC,        "[4096PageSize] Write Verify 8BitECC       \n",	
//	NANDT_WritePage_8bit_NBoot,             "Write 8K Size Data : 0 Block 0~15Page(8K Byte Data Size)      ",

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
//	eError = NAND_EraseSingleBlock_Basic(g_NandContNum, uBlock);	

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
// Function Name : NANDT_EraseSingleBlock_NonValid
// Function Description : Erase 1 block
// Input : 	eTest - Manual test or Auto test selection
//			oAutoVar - Parameters for Auto-Test
// Output : 	None
void NANDT_EraseSingleBlock_NonValid(eFunction_Test eTest, oFunctionT_AutoVar oAutoVar)
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
	
//	eError = NAND_EraseSingleBlock(g_NandContNum, uBlock);
	eError = NAND_EraseSingleBlock_Basic(g_NandContNum, uBlock);	

	if(eError != eNAND_NoError)
	{
		NANDT_PrintErrorType(eError);
		UART_GetKey();
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

void NANDT_EraseMultiBlock_Noinvaid(eFunction_Test eTest, oFunctionT_AutoVar oAutoVar)
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
	
	eError = NAND_EraseMultiBlock_Novaild(g_NandContNum, uBlock, uBlockNum);

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
	NANDT_EraseSingleBlock_NonValid,                   "Non_Check Invalid Data Single block erase      ",
	NANDT_EraseMultiBlock_Noinvaid,                     "Non_Check Invalid Data Multi block erase      ",
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

	uSourceAddr = (u8 *)(_DRAM_BaseAddress+0x01000000);
	
	UART_Printf("Caution : You must put BINARY file into 0x%08x before programming\n",uSourceAddr);

	UART_Printf("Input the Block Number to write[0~%d]", NAND_Inform[g_NandContNum].uBlockNum-1);
	uBlock = UART_GetIntNum();
	if(uBlock == -1)
		uBlock = 0;
	
	UART_Printf("Input the Page Number to write[0~%d]", NAND_Inform[g_NandContNum].uPageNum-1);
	uPage = UART_GetIntNum();
	if(uPage == -1)
		uPage = 0;
	
	UART_Printf("Input the Size to program[Bytes] : ");
	uSize = UART_GetIntNum();
	if(uSize == -1)
	{
		if(NAND_Inform[g_NandContNum].uNandType == NAND_Normal8bit)
			uSize = 0x40000;
		else if(NAND_Inform[g_NandContNum].uNandType == NAND_Advanced8bit)
			uSize = 0x40000;
		else
			uSize = 0x60000;
	}

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
		eError = NAND_EraseSingleBlock_Basic(g_NandContNum, uWriteBlock);
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
//			eError = NAND_WritePage(g_NandContNum, uWriteBlock, j, uSourceAddr, aSpareBuffer);
			eError = NAND_WritePage(g_NandContNum, uWriteBlock, j, uSourceAddr, aSpareBuffer);
//			eError = NAND_WritePage_8Bit(g_NandContNum, uWriteBlock, j, uSourceAddr, aSpareBuffer);
//			eError = NAND_Write2048Page_8BitValid(g_NandContNum, uWriteBlock, j, uSourceAddr, aSpareBuffer);	
//			eError = NAND_Write2048Page_8Bit(g_NandContNum, uWriteBlock, j, uSourceAddr, aSpareBuffer);	
//			eError = NAND_Write4096Page_8BitValid(g_NandContNum, uWriteBlock, j, uSourceAddr, aSpareBuffer);	
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


void NANDT_SoftLock_TEmp(void)
{
	u32 i, uStartBlock, uEndBlock;
	NAND_eERROR eError;
	u8 *aBuffer, *aSpareBuffer;
	u8 bError = FALSE;

	UART_Printf("[NANDT_SoftLock]\n\n");

	
	UART_Printf("Input the Start Block Number to soft-lock[0~%d] : ", NAND_Inform[g_NandContNum].uBlockNum-1);
	uStartBlock = UART_GetIntNum();
	UART_Printf("Input the End Block Number to soft-lock[%d~%d] : ", uStartBlock, NAND_Inform[g_NandContNum].uBlockNum-1);
	uEndBlock = UART_GetIntNum();
	UART_Printf("\n");

	eError = NAND_SoftLockingBlock(g_NandContNum, uStartBlock, uEndBlock);

	if(eError != eNAND_NoError)
	{
		UART_Printf("NAND Soft-Lock Block Error.....!!\n\n");
		UART_Getc();
		return;
	}
	else
	{
		UART_Printf("NAND Soft-Lock Block Complete[except %d ~ %d Block]\n", uStartBlock, uEndBlock);
	}

}



//////////
// Function Name : NANDT_SoftLock
// Function Description : Soft-lock the NAND Memory
// Input : 	eTest - Manual test or Auto test selection
//			oAutoVar - Parameters for Auto-Test
// Output : 	None
void NANDT_SoftLock(eFunction_Test eTest, oFunctionT_AutoVar oAutoVar)
{
	u32 i, uStartBlock, uEndBlock;
	NAND_eERROR eError;
	u8 *aBuffer, *aSpareBuffer;
	u8 bError = FALSE;

	if(eTest == eTest_Manual)
		UART_Printf("[NANDT_SoftLock]\n\n");

	if(eTest == eTest_Manual)
	{
		UART_Printf("Input the Start Block Number to soft-lock[0~%d] : ", NAND_Inform[g_NandContNum].uBlockNum-1);
		uStartBlock = UART_GetIntNum();
		UART_Printf("Input the End Block Number to soft-lock[%d~%d] : ", uStartBlock, NAND_Inform[g_NandContNum].uBlockNum-1);
		uEndBlock = UART_GetIntNum();
		UART_Printf("\n");
	}
	else
	{
		uStartBlock = oAutoVar.Test_Parameter[0];
		uEndBlock = oAutoVar.Test_Parameter[1];
	}

	eError = NAND_SoftLockingBlock(g_NandContNum, uStartBlock, uEndBlock);

	if(eError != eNAND_NoError)
	{
		UART_Printf("NAND Soft-Lock Block Error.....!!\n\n");
		UART_Getc();
		return;
	}
	else
	{
		if(eTest == eTest_Manual)
			UART_Printf("NAND Soft-Lock Block Complete[except %d ~ %d Block]\n", uStartBlock, uEndBlock);
	}

	gNandAutoTest = 1;
	
	if(eTest == eTest_Manual)
		UART_Printf("Erasing to the locked block...\n");
		
	// 0 Block is not included because 0 block is boot area
	for(i=1 ; i<NAND_Inform[g_NandContNum].uBlockNum ; i++)
	{
		g_Nand_IllegalAccErrorforTest = 0;
		
		eError = NAND_EraseSingleBlock(g_NandContNum, i);

		if(eError != eNAND_InvalidBlock)
		{
			if( (i<uStartBlock) || (i>uEndBlock) )
			{
				if(g_Nand_IllegalAccErrorforTest != 1)
				{
					bError = TRUE;
					UART_Printf("NAND Soft-Lock Block.......Erased!! Error..\n");
					UART_Getc();
					break;
				}
			}
			else
			{
				if(g_Nand_IllegalAccErrorforTest == 1)
				{
					bError = TRUE;
					UART_Printf("NAND Unlocked Block.......Not erased!! Error..\n");
					UART_Getc();
					break;
				}		
			}
		}
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
	{
		aBuffer[i] = i;
	}

	if(eTest == eTest_Manual)
		UART_Printf("Writing to the locked block...\n");
	for(i=uStartBlock-2 ; i<=uEndBlock+2 ; i++)		// (uStartBlock-2 ~ uEndBlock+2) Write Test
	{
		g_Nand_IllegalAccErrorforTest = 0;
		NAND_WritePage(g_NandContNum, i, 0, aBuffer, aSpareBuffer);

		if( (i<uStartBlock) || (i>uEndBlock) )
		{
			if(g_Nand_IllegalAccErrorforTest != 1)
			{
				bError = TRUE;
				UART_Printf("NAND Soft-Lock Block.......Written!! Error..\n");
				UART_Getc();
				break;
			}
		}
		else
		{
			if(g_Nand_IllegalAccErrorforTest == 1)
			{
				bError = TRUE;
				UART_Printf("NAND Unlocked Block.......Not written!! Error..\n");
				UART_Getc();
				break;
			}
		}
	}

	// Unlock the Soft-locked block
	if(eTest == eTest_Manual)
		UART_Printf("NAND Unlock the Soft-locked block\n");
	NAND_UnLockBlock(g_NandContNum);
	
	// Erase the written block
	for(i=uStartBlock-2 ; i<uEndBlock+2 ; i++)
	{
		NAND_EraseSingleBlock(g_NandContNum, i);
	}
	
	if(eTest == eTest_Manual)
	{
		if(bError == FALSE)
			UART_Printf("NAND Soft-Lock Block Erase/Write Test....Complete\n");
		else
		{
			UART_Printf("NAND Soft-Lock Block Erase/Write Test....Error\n");
			UART_Getc();
		}
	}

	gNandAutoTest = 0;
	
	free(aBuffer);
	free(aSpareBuffer);	
}


//////////
// Function Name : NANDT_LockTight
// Function Description : Lock-tight the NAND Memory
// Input : 	eTest - Manual test or Auto test selection
//			oAutoVar - Parameters for Auto-Test
// Output : 	None
void NANDT_DisableLockTight(eFunction_Test eTest, oFunctionT_AutoVar oAutoVar);
void NANDT_LockTight(eFunction_Test eTest, oFunctionT_AutoVar oAutoVar)
{
	u32 i, uStartBlock, uEndBlock;
	NAND_eERROR eError;
	u8 *aBuffer, *aSpareBuffer;
	u8 bError = FALSE;

	if(eTest == eTest_Manual)
		UART_Printf("[NANDT_LockTight]\n\n");

	if(eTest == eTest_Manual)
	{
		UART_Printf("Input the Start Block Number to lock-tight[0~%d] : ", NAND_Inform[g_NandContNum].uBlockNum-1);
		uStartBlock = UART_GetIntNum();
		UART_Printf("Input the End Block Number to lock-tight[%d~%d] : ", uStartBlock, NAND_Inform[g_NandContNum].uBlockNum-1);
		uEndBlock = UART_GetIntNum();
		UART_Printf("\n");
	}
	else
	{
		uStartBlock = oAutoVar.Test_Parameter[0];
		uEndBlock = oAutoVar.Test_Parameter[1];
	}		
	
	eError = NAND_LockTightBlock(g_NandContNum, uStartBlock, uEndBlock);

	if(eError != eNAND_NoError)
	{
		UART_Printf("NAND Lock-tight Block Error.....!!\n");
		UART_Getc();
	}
	else
	{
		if(eTest == eTest_Manual)
			UART_Printf("NAND Lock-tight Block Complete[except %d ~ %d Block]\n", uStartBlock, uEndBlock);
	}

	// unlock the lock-tight block -> must be not unlock
	NANDT_DisableLockTight(eTest, oAutoVar);
	
	gNandAutoTest = 1;
	// 0 Block is not included because 0 block is boot area
	for(i=1 ; i<NAND_Inform[g_NandContNum].uBlockNum ; i++)
	{
		g_Nand_IllegalAccErrorforTest = 0;
		eError = NAND_EraseSingleBlock(g_NandContNum, i);

		if(eError != eNAND_InvalidBlock)
		{
			if( (i<uStartBlock) || (i>uEndBlock) )
			{
				if(g_Nand_IllegalAccErrorforTest != 1)
				{
					bError = TRUE;
					UART_Printf("NAND Soft-Lock Block.......Erased!! Error..\n");
					UART_Getc();
					break;
				}
			}
			else
			{
				if(g_Nand_IllegalAccErrorforTest == 1)
				{
					bError = TRUE;
					UART_Printf("NAND Unlocked Block.......Not erased!! Error..\n");
					UART_Getc();
					break;
				}		
			}
		}
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
	{
		aBuffer[i] = i;
	}
	
	for(i=uStartBlock-2 ; i<=uEndBlock+2 ; i++)		// (uStartBlock-2 ~ uEndBlock+2) Write Test
	{
		g_Nand_IllegalAccErrorforTest = 0;
		NAND_WritePage(g_NandContNum, i, 0, aBuffer, aSpareBuffer);

		if( (i<uStartBlock) || (i>uEndBlock) )
		{
			if(g_Nand_IllegalAccErrorforTest != 1)
			{
				bError = TRUE;
				UART_Printf("NAND Soft-Lock Block.......Written!! Error..\n");
				UART_Getc();
				break;
			}
		}
		else
		{
			if(g_Nand_IllegalAccErrorforTest == 1)
			{
				bError = TRUE;
				UART_Printf("NAND Unlocked Block.......Not written!! Error..\n");
				UART_Getc();
				break;
			}
		}
	}

	if(eTest == eTest_Manual)
	{
		if(bError == FALSE)
			UART_Printf("NAND Lock-tight Block Erase/Write Test....Complete\n");
		else
		{
			UART_Printf("NAND Lock-tight Block Block Erase/Write Test....Error\n");
			UART_Getc();
		}
		UART_Printf("\n");
	}

	gNandAutoTest = 0;
	free(aBuffer);
	free(aSpareBuffer);	
}


//////////
// Function Name : NANDT_DisableSoftLock
// Function Description : Disable Soft-lock of the NAND Memory
// Input : 	eTest - Manual test or Auto test selection
//			oAutoVar - Parameters for Auto-Test
// Output : 	None
void NANDT_DisableSoftLock(eFunction_Test eTest, oFunctionT_AutoVar oAutoVar)
{
	u32 i, uStartBlock, uEndBlock;
	NAND_eERROR eError;
	u8 *aBuffer, *aSpareBuffer;
	u8 bError = FALSE;
	
	if(eTest == eTest_Manual)
		UART_Printf("[NANDT_DisableSoftLock]\n\n");

	if(eTest == eTest_Manual)
	{
		UART_Printf("Input the Start Block Number to soft-lock[0~%d] : ", NAND_Inform[g_NandContNum].uBlockNum-1);
		uStartBlock = UART_GetIntNum();
		UART_Printf("Input the End Block Number to soft-lock[%d~%d] : ", uStartBlock, NAND_Inform[g_NandContNum].uBlockNum-1);
		uEndBlock = UART_GetIntNum();
		UART_Printf("\n");
	}
	else
	{
		uStartBlock = oAutoVar.Test_Parameter[0];
		uEndBlock = oAutoVar.Test_Parameter[1];
	}
	
	// Soft-lock 100 ~ 120 block
	eError = NAND_SoftLockingBlock(g_NandContNum, uStartBlock, uEndBlock);

	if(eError != eNAND_NoError)
	{
		UART_Printf("NAND Soft-Lock Block Error.....!!\n\n");
		UART_Getc();
		return;
	}
	else
	{
		if(eTest == eTest_Manual)
			UART_Printf("NAND Soft-Lock Block Complete[except %d ~ %d Block]\n", uStartBlock, uEndBlock);
	}

	// Unlock
	eError = NAND_UnLockBlock(g_NandContNum);

	if(eError != eNAND_NoError)
	{
		UART_Printf("UnLock the Soft-locked Block Error.....!!\n");
		UART_Getc();
	}
	else
	{
		if(eTest == eTest_Manual)
			UART_Printf("UnLock the Soft-locked Block Complete\n");
	}
	
	gNandAutoTest = 1;
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
	{
		aBuffer[i] = i;
	}

	if(eTest == eTest_Manual)
		UART_Printf("Writing to the unlocked block...\n");
	
	for(i=1 ; i<NAND_Inform[g_NandContNum].uBlockNum ; i++)
	{
		g_Nand_IllegalAccErrorforTest = 0;

		if(NAND_CheckInvalidBlock(g_NandContNum, i) != eNAND_NoError)
		{
			NAND_WritePage(g_NandContNum, i, 0, aBuffer, aSpareBuffer);

			if(g_Nand_IllegalAccErrorforTest == 1)
			{
				bError = TRUE;
				UART_Printf("NAND Unlocked Block.......Not written!! Error..\n");
				UART_Getc();
				break;
			}
		}
	}

	if(eTest == eTest_Manual)
		UART_Printf("Erasing to the unlocked block...\n");
	// 0 Block is not included because 0 block is boot area
	for(i=1 ; i<NAND_Inform[g_NandContNum].uBlockNum ; i++)
	{
		g_Nand_IllegalAccErrorforTest = 0;
		eError = NAND_EraseSingleBlock_Basic(g_NandContNum, i);

		if(eError != eNAND_InvalidBlock)
		{
			if(g_Nand_IllegalAccErrorforTest == 1)
			{
				bError = TRUE;
				UART_Printf("NAND Unlocked Block.......Not erased!! Error..\n");
				UART_Getc();
				break;
			}		
		}
	}
	
	if(eTest == eTest_Manual)
	{
		if(bError == FALSE)
			UART_Printf("NAND Soft-Lock Block Erase/Write Test....Complete\n");
		else
		{
			UART_Printf("NAND Soft-Lock Block Erase/Write Test....Error\n");
			UART_Getc();
		}
	}

	gNandAutoTest = 0;
	free(aBuffer);
	free(aSpareBuffer);	
	
}


//////////
// Function Name : NANDT_DisableLockTight
// Function Description : Disable Lock-tight of the NAND Memory
// Input : 	eTest - Manual test or Auto test selection
//			oAutoVar - Parameters for Auto-Test
// Output : 	None
void NANDT_DisableLockTight(eFunction_Test eTest, oFunctionT_AutoVar oAutoVar)
{
	NAND_eERROR eError;

	if(eTest == eTest_Manual)
		UART_Printf("[NANDT_DisableLockTight]\n");

	eError = NAND_UnLockTightBlock(g_NandContNum);

	if(eError != eNAND_NoError)
	{
		UART_Printf("Fail - cleared Lock-tight bit by software\n");
		UART_Getc();
	}
	else
	{
		if(eTest == eTest_Manual)
			UART_Printf("Success - not cleared Lock-tight bit by software\n");
	}
}


//////////
// Function Name : NANDT_SoftLock_8Bit
// Function Description : Soft-lock the NAND Memory
// Input : 	eTest - Manual test or Auto test selection
//			oAutoVar - Parameters for Auto-Test
// Output : 	None
void NANDT_SoftLock_8Bit(eFunction_Test eTest, oFunctionT_AutoVar oAutoVar)
{
	u32 uStartBlock, uEndBlock;
	NAND_eERROR eError;
//	u8 *aBuffer, *aSpareBuffer;
// 	bool bError = FALSE;
//	u32 uTstStartBLK, uTstEndBLK;

	UART_Printf("-------------------------------------\n");
	UART_Printf("[NANDT_SoftLock]\n\n");

	UART_Printf("Input the Start Block Number to soft-lock[0~%d] : ", NAND_Inform[g_NandContNum].uBlockNum-1);
	uStartBlock = UART_GetIntNum();
	UART_Printf("Input the End Block Number to soft-lock[%d~%d] : ", uStartBlock, NAND_Inform[g_NandContNum].uBlockNum-1);
	uEndBlock = UART_GetIntNum();
	UART_Printf("\n");

	eError = NAND_SoftLockingBlock(g_NandContNum, uStartBlock, uEndBlock);

	if(eError != eNAND_NoError)
	{
		UART_Printf("NAND Soft-Lock Block Error.....!!\n\n");
		UART_GetKey();
		return;
	}
	else
	{
		if(eTest == eTest_Manual)
			UART_Printf("NAND Soft-Lock Block Complete[except %d ~ %d Block]\n", uStartBlock, uEndBlock);
	}

	UART_Printf("-------------------------------------\n");
	UART_Printf("[Erasing to the locked block]\n\n");
	NANDT_EraseSingleBlock(eTest_Manual, oAutoVar);

	UART_Printf("-------------------------------------\n");
	UART_Printf("[Write and Read Test]\n");
	UART_Printf("---1. Write Test.\n");
	NANDT_WritePage_8bit(eTest_Manual, oAutoVar);

	UART_Printf("-------------------------------------\n");
	UART_Printf("---2. Read Test.\n");
	NANDT_ReadPage_8Bit(eTest_Manual, oAutoVar);

	UART_Printf("\n-------------------------------------\n");
	UART_Printf("NAND Soft-UnLock Block Complete[except %d ~ %d Block]\n", uStartBlock, uEndBlock);
	NAND_UnLockBlock(g_NandContNum);

	UART_Printf("\n-------------------------------------\n");
	UART_Printf("Write and Read Test\n");
	UART_Printf("---1. Write Test.\n");
	NANDT_WritePage_8bit(eTest_Manual, oAutoVar);

	UART_Printf("---2. Read Test.\n");
	NANDT_ReadPage_8Bit(eTest_Manual, oAutoVar);

	UART_Printf("Soft Lock Read Write Test End.\n");
	UART_Printf("-------------------------------------\n\n\n");	
	

}

//////////
// Function Name : NANDT_TightLock_8Bit
// Function Description : Soft-lock the NAND Memory
// Input : 	eTest - Manual test or Auto test selection
//			oAutoVar - Parameters for Auto-Test
// Output : 	None
void NANDT_TightLock_8Bit(eFunction_Test eTest, oFunctionT_AutoVar oAutoVar)
{
	u32 uStartBlock, uEndBlock;
	NAND_eERROR eError;
//	u8 *aBuffer, *aSpareBuffer;
//	bool bError = FALSE;
//	u32 uTstStartBLK, uTstEndBLK;
	u32 uTemp;

	UART_Printf("-------------------------------------\n");
	UART_Printf("[NANDT_TightLock]\n\n");

	UART_Printf("Input the Start Block Number to Tight-lock[0~%d] : ", NAND_Inform[g_NandContNum].uBlockNum-1);
	uStartBlock = UART_GetIntNum();
	UART_Printf("Input the End Block Number to Tight-lock[%d~%d] : ", uStartBlock, NAND_Inform[g_NandContNum].uBlockNum-1);
	uEndBlock = UART_GetIntNum();
	UART_Printf("\n");

	eError = NAND_LockTightBlock(g_NandContNum, uStartBlock, uEndBlock);

	if(eError != eNAND_NoError)
	{
		UART_Printf("NAND Tight-Lock Block Error.....!!\n\n");
		UART_GetKey();
		return;
	}
	else
	{
		if(eTest == eTest_Manual)
			UART_Printf("NAND Tight-Lock Block Complete[except %d ~ %d Block]\n", uStartBlock, uEndBlock);
	}

	UART_Printf("-------------------------------------\n");
	UART_Printf("[Erasing to the locked block]\n\n");
	NANDT_EraseSingleBlock(eTest_Manual, oAutoVar);

	UART_Printf("-------------------------------------\n");
	UART_Printf("[Write and Read Test]\n");
	UART_Printf("---1. Write Test.\n");
	NANDT_WritePage_8bit(eTest_Manual, oAutoVar);

	UART_Printf("-------------------------------------\n");
	UART_Printf("---2. Read Test.\n");
	NANDT_ReadPage_8Bit(eTest_Manual, oAutoVar);

	UART_Printf("\n-------------------------------------\n");
	UART_Printf("NAND Tight-UnLock Block Complete-> For Test [except %d ~ %d Block]\n", uStartBlock, uEndBlock);
//	NAND_UnLockBlock(g_NandContNum);
	NANDT_DisableLockTight(eTest_Manual, oAutoVar);	

	UART_Printf("\n-------------------------------------\n");
	UART_Printf("Write and Read Test\n");
	UART_Printf("---1. Write Test.\n");
	NANDT_WritePage_8bit(eTest_Manual, oAutoVar);

	UART_Printf("---2. Read Test.\n");
	NANDT_ReadPage_8Bit(eTest_Manual, oAutoVar);

	eError = NAND_LockTightBlock(g_NandContNum, uStartBlock, uEndBlock);

	if(eError != eNAND_NoError)
	{
		UART_Printf("NAND Tight-Lock Block Error.....!!\n\n");
		UART_GetKey();
		return;
	}
	else
	{
		if(eTest == eTest_Manual)
			UART_Printf("NAND Tight-Lock Block Complete[except %d ~ %d Block]\n", uStartBlock, uEndBlock);
	}

	
#if 1
	UART_Printf("-------------------------------------\n\n\n");

	UART_Printf("[Software Reset Issued-NFCON Reset Issued]\n");	
	uTemp = NFCONInp32((unsigned volatile *)0x9e005114);
	uTemp = uTemp | (0x6430);
	NFCONOutp32((unsigned volatile *)0x9e005114, uTemp);
//	*(unsigned volatile *)0x9e005114=(*(unsigned volatile *)0x9e005114)|(0x6430); // SoftWare Reset

	UART_Printf("---1. Write Test.\n");
	NANDT_WritePage_8bit(eTest_Manual, oAutoVar);

	UART_Printf("---2. Read Test.\n");
	NANDT_ReadPage_8Bit(eTest_Manual, oAutoVar);
#endif

	UART_Printf("-------------------------------------\n\n\n");

	UART_Printf("[Sleep mode Entered]\n");
	uTemp = NFCONInp32((unsigned volatile *)0x9e005804); // wake up source is RTC Tick
	uTemp = uTemp & ~ (0x1<<11);
	NFCONOutp32((unsigned volatile *)0x9e005804, uTemp);
//	*(unsigned volatile *)0x9e005804 = ;

	*(unsigned volatile *)0x9e005800 = 0x4; // Enter Sleep mode

	UART_Printf("---1. Write Test.\n");
	NANDT_WritePage_8bit(eTest_Manual, oAutoVar);

	UART_Printf("---2. Read Test.\n");
	NANDT_ReadPage_8Bit(eTest_Manual, oAutoVar);

	UART_Printf("-------------------------------------\n\n\n");
	
	

	

}


const AutotestFuncMenu nandlock_menu[] =
{
	NANDT_SoftLock,					"Soft Lock        ",
	NANDT_LockTight,					"Lock Tight       ",
	NANDT_DisableSoftLock,				"Soft Unlock      ",
	NANDT_DisableLockTight,				"Lock-tight Unlock     ",	
	NANDT_SoftLock_8Bit,                       "[Normal SLC]Soft  Lock Test(8BitECC)     ",
	NANDT_TightLock_8Bit,                     "[Normal SLC]Tight Lock Test(8BitECC)      ",
	NANDT_SoftLock_TEmp,                     "SOft Lock Only",
	0, 0
};


void NANDT_LockUnlock(void)
{
	u32 i;
	s32 uSel;
	oFunctionT_AutoVar oAutoTest;
	
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
			(nandlock_menu[uSel].func) (eTest_Manual, oAutoTest);
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
	//u8 aBuffer[NAND_PAGE_MAX];
	//u8 aSpareBuffer[NAND_SPARE_MAX];
	u8 *aBuffer, *aSpareBuffer;
	NAND_eERROR eError;
	
	UART_Printf("[NANDT_SLCECC]\n");

	if(NAND_Inform[g_NandContNum].uNandType == NAND_MLC8bit)
	{
		UART_Printf("The current Nand Memory is MLC type.....!!\n\n");
		return;
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
	
	if(uErrorType == NAND_ECCERR_1BIT)
	{
		UART_Printf("Input the First Byte Postion[0 ~ 511]");
		NAND_EccError.uEccErrByte1 = UART_GetIntNum()%512;
		UART_Printf("Input the First Bit Postion[0 ~ 7]");
		NAND_EccError.uEccErrBit1 = UART_GetIntNum()%8;	
	}
	else
	{
		UART_Printf("Input the First Byte Postion[0 ~ 511]");
		NAND_EccError.uEccErrByte1 = UART_GetIntNum()%512;
		UART_Printf("Input the First Bit Postion[0 ~ 7]");
		NAND_EccError.uEccErrBit1 = UART_GetIntNum()%8;	

		UART_Printf("Input the Second Byte Postion[0 ~ 511]");
		NAND_EccError.uEccErrByte2 = UART_GetIntNum()%512;
		UART_Printf("Input the Second Bit Postion[0 ~ 7]");
		NAND_EccError.uEccErrBit2 = UART_GetIntNum()%8;		
	}
	
	srand(0);
	for(i=0 ; i<NAND_Inform[g_NandContNum].uPageSize ; i++)
	{
		aBuffer[i] = rand()%0xFF;
	}
	
	for(i=0 ; i<NAND_Inform[g_NandContNum].uSpareSize ; i++)
		aSpareBuffer[i] = 0xFF;	

	UART_Printf("\n");
	UART_Printf("The specific bit ecc check : [%dByte : %dBit]\n", NAND_EccError.uEccErrByte1, NAND_EccError.uEccErrBit1);
	if(uErrorType == NAND_ECCERR_MULTI)
		UART_Printf("The specific bit ecc check : [%dByte : %dBit]\n", NAND_EccError.uEccErrByte2, NAND_EccError.uEccErrBit2);
	UART_Printf("\n");
	
	// Erase Block for test
	UART_Printf("Erase the block ( %d[block] )\n", uBlock);
	eError = NAND_EraseSingleBlock_Basic(g_NandContNum, uBlock);
	if(eError != eNAND_NoError)
	{
		NANDT_PrintErrorType(eError);
		UART_Getc();
		free(aBuffer);
		free(aSpareBuffer);
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
		free(aBuffer);
		free(aSpareBuffer);
		return;
	}

	// Erase Block for test
	UART_Printf("Erase the block ( %d[block] )\n", uBlock);
	eError = NAND_EraseSingleBlock_Basic(g_NandContNum, uBlock);
	if(eError != eNAND_NoError)
	{
		NANDT_PrintErrorType(eError);
		UART_Getc();
		NAND_Inform[g_NandContNum].uECCtest = 0;
		free(aBuffer);
		free(aSpareBuffer);
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
		free(aBuffer);
		free(aSpareBuffer);
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
			
	free(aBuffer);
	free(aSpareBuffer);

	UART_Printf("SLC Nand ECC test complete\n");
	UART_Printf("\n");	
}

//////////
// Function Name : NANDT_1BitECC_SLC
// Function Description : SLC ECC function test
// Input : 	None
// Output : 	None
void NANDT_1BitECC_SLC(void)
{
	u32 i, uBlock, uPage, uError, uErrorType;
	u8 aBuffer[NAND_PAGE_512];
	u8 aSpareBuffer[NAND_SPARE_16];
//	u8 aReadBuffer[NAND_PAGE_512];
	NAND_eERROR eError;
	u32 uTemp;
	u32 uIndex0;

	NAND_EccError.uEccErrBit1 = 0;
	NAND_EccError.uEccErrByte1 = 0;
	NAND_EccError.uEccErrBit2 = 0;
	NAND_EccError.uEccErrByte2 = 0;

	NAND_Inform[g_NandContNum].uECCtest = 0;
	NAND_Inform[g_NandContNum].uSpareECC = 0;						
	NAND_Inform[g_NandContNum].uECCType = ECC1Bit;
	
	UART_Printf("[NAND_1BitECC_BasicTestl]\n");

	// 01. Input wanted 1bit ecc test block
	UART_Printf("Input the Block Number to test[0~%d]", NAND_Inform[g_NandContNum].uBlockNum-1);
	uBlock = UART_GetIntNum();
	UART_Printf("Input the Page Number to test[0~%d]", NAND_Inform[g_NandContNum].uPageNum-1);
	uPage = UART_GetIntNum();	

	// 02. select ecc type
	UART_Printf("Input the ECC Error Type\n");
	UART_Printf("1: 1bit error(D)	2: Multipul Error \n");
	uError = UART_GetIntNum();

	// 03. Input byte positoin and bit position
	switch(uError)
	{
		case 1 :	uErrorType = NAND_ECCERR_1BIT;
				UART_Printf("Input the First Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte1 = UART_GetIntNum()%528;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[0]=NAND_EccError.uEccErrByte1;
				UART_Printf("Input the First Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit1 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[0] = NAND_EccError.uEccErrByte1 ;
				break;
		case 2 :	uErrorType = NAND_ECCERR_2BIT;
				UART_Printf("Input the 1st Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte1 = UART_GetIntNum()%528;
				UART_Printf("Input the 1st Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit1 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[0]=NAND_EccError.uEccErrByte1;
				UART_Printf("Input the 2nd Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte2 = UART_GetIntNum()%528;
				UART_Printf("Input the 2nd Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit2 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[1]=NAND_EccError.uEccErrByte2;
				break;
		default : uErrorType = 0;
			break;
	}

	// 04. Save page and spare data to buffer : aBuffer and aSpareBuffer
	for(i=0 ; i<NAND_Inform[g_NandContNum].uPageSize ; i++)
	{
		aBuffer[i] = 0x0;
	}
	for(i=0 ; i<NAND_Inform[g_NandContNum].uSpareSize ; i++)
		aSpareBuffer[i] = 0x0;	


	UART_Printf("\n");
	
	// 05. Erase Block for test
	UART_Printf("----------------------------------\n");
	UART_Printf("01. Erase the Block ( %d[block])\n", uBlock);
	eError = NAND_EraseSingleBlock_Basic(g_NandContNum, uBlock);
	if(eError != eNAND_NoError)
	{
		NANDT_PrintErrorType_Sample(eError);
		return;
	}

	// 06. Write the valid data to page ( uBlock[block] 0[page] )
	UART_Printf("02. Write the valid data to page for ECC generation ( %d[block] %d[page] )\n", uBlock, uPage);
	NAND_Inform[g_NandContNum].uECCtest = 0;
	NAND_Inform[g_NandContNum].uSpareECC =0;
	eError = NAND_WritePage_1Bit(g_NandContNum, uBlock, uPage, aBuffer, aSpareBuffer);
	if(eError != eNAND_NoError)
	{
		NANDT_PrintErrorType(eError);
		return;
	}

	// 07. Read the valid data from page(uBlock[block] 0[page] )
	UART_Printf("03. Read the valid data in page for ECC generation (%d[block]   %d[Page])\n", uBlock, uPage);
	NAND_Inform[g_NandContNum].uECCtest = 0;
	NAND_Inform[g_NandContNum].uSpareECC = 0;
	eError = NAND_ReadPage_1Bit(g_NandContNum, uBlock, uPage, aBuffer, aSpareBuffer);
	if(eError != eNAND_NoError)
	{
		NANDT_PrintErrorType(eError);
		NAND_Inform[g_NandContNum].uECCtest = 0;
		return;
	}
	else
	{
		#if 1 // For Debug test
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
		#endif
		UART_Printf("NAND Vaild Data Read Page : Success\n");
	}

	// 08. Display test condition
	// Test Start......!!
	UART_Printf("-------------------------------------------\n");	
	UART_Printf("-------------- InValid Data Create   ----------\n");		
	switch (uError)
	{
		case 1: 
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte1, NAND_EccError.uEccErrBit1);
				break;
		case 2: 
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte1, NAND_EccError.uEccErrBit1);
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte2, NAND_EccError.uEccErrBit2);
				break;
		default: break;

	}
	UART_Printf("-------------------------------------------\n");	
	

	// 09. Erase Block for test
	UART_Printf("03. Erase the Block ( %d[block])\n", uBlock);
	eError = NAND_EraseSingleBlock_Basic(g_NandContNum, uBlock);
	if(eError != eNAND_NoError)
	{
		NANDT_PrintErrorType(eError);
		NAND_Inform[g_NandContNum].uECCtest = 0;
		NAND_Inform[g_NandContNum].uAllBitEccCheck = 0;
		return;
	}

	// 10. Create Invalid data

	for ( uIndex0 =0 ; uIndex0<uError ; uIndex0++)
	{
		switch ( uIndex0+1)
		{
			case 1:
				if ( NAND_Inform[g_NandContNum].u8BitECCErrBytePos[uIndex0] < 512 )
				{
					uTemp =	aBuffer[NAND_EccError.uEccErrByte1] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit1);
					aBuffer[NAND_EccError.uEccErrByte1] = uTemp;				
					NAND_Inform[g_NandContNum].uECCtest = 1;
				}
				else
				{
					uTemp =	aSpareBuffer[(NAND_EccError.uEccErrByte1)-512] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit1);
					aSpareBuffer[(NAND_EccError.uEccErrByte1)-512] = uTemp;				
					NAND_Inform[g_NandContNum].uSpareECC = 1;
				}
				break;
			case 2:
				if ( NAND_Inform[g_NandContNum].u8BitECCErrBytePos[uIndex0] < 512 )
				{
					uTemp =	aBuffer[NAND_EccError.uEccErrByte2] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit2);
					aBuffer[NAND_EccError.uEccErrByte2] = uTemp;				
					NAND_Inform[g_NandContNum].uECCtest = 1;					
				}
				else
				{
					uTemp =	aSpareBuffer[(NAND_EccError.uEccErrByte2)-512] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit2);
					aSpareBuffer[(NAND_EccError.uEccErrByte2)-512] = uTemp;				
					NAND_Inform[g_NandContNum].uSpareECC = 1;					
				}
				break;
			default: 
				break;
		}

	}

	// 11. Write Data to NAND Flash
	UART_Printf("04. After invalid data create , write NAND flash\n");
	eError = NAND_WritePage_1Bit(g_NandContNum, uBlock, uPage, aBuffer, aSpareBuffer);
	if(eError != eNAND_NoError)
	{
		NANDT_PrintErrorType(eError);
		NAND_Inform[g_NandContNum].uECCtest = 0;
	}
	else
	{
		#if 1
		UART_Printf("[Main Area]");
		for(i=0 ; i<NAND_Inform[g_NandContNum].uPageSize ; i++)
		{
			if(i%16==0)
	    			UART_Printf("\n%04xh:",i);
			
        		UART_Printf("%02x ", aBuffer[i]);
		}
		UART_Printf("\n");
		
		UART_Printf("[Spare Area: is vaid for Main ECC Test ]");
		for(i=0 ; i<NAND_Inform[g_NandContNum].uSpareSize ; i++)
		{
			if(i%16==0)
	    			UART_Printf("\n%04xh:",i);
			
        		UART_Printf("%02x ", aSpareBuffer[i]);
		}
		UART_Printf("\n\n");
		#endif
		UART_Printf("NAND Invalid Data Write Page : Success\n");
	}

	UART_Printf("\n");

	// 12. Read Data from NAND flash
	UART_Printf("05. Read NAND flash\n");
	eError = NAND_ReadPage_1Bit(g_NandContNum, uBlock, uPage, aBuffer, aSpareBuffer);
	if(eError != eNAND_NoError)
	{

		NANDT_PrintErrorType(eError);
		NAND_Inform[g_NandContNum].uECCtest = 0;
		NAND_Inform[g_NandContNum].uAllBitEccCheck = 0;
		UART_Printf("Read page Error [%d block  %d page]\n", uBlock, uPage);
		UART_Printf("-------------------------------------------\n");	
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

	NAND_Inform[g_NandContNum].uECCtest = 0;
	NAND_Inform[g_NandContNum].uSpareECC = 0 ; 
	
	UART_Printf("SLC Nand 1bit ECC test complete & OK...!!\n");
	UART_Printf("\n");	
}


//////////
// Function Name : NANDT_1BitECC_SLC_All
// Function Description : SLC ECC function test
// Input : 	None
// Output : 	None
void NANDT_1BitECC_SLC_All(void)
{
	u32 i, uBlock, uPage;
	u8 aBuffer[NAND_PAGE_512];
	u8 aSpareBuffer[NAND_SPARE_16];
//	u8 aReadBuffer[NAND_PAGE_512];
	NAND_eERROR eError;
	u32 uTemp;
	u32 uIndex0;
	u32 uByte0, uBit0;

	NAND_Inform[g_NandContNum].uECCtest = 0;
	NAND_Inform[g_NandContNum].uSpareECC = 0;						
	NAND_Inform[g_NandContNum].uECCType = ECC1Bit;
	
	UART_Printf("[NAND_1BitECC_All Bit Testl]\n");

	// 01. Input wanted 1bit ecc test block
	UART_Printf("Input the Block Number to test[0~%d]", NAND_Inform[g_NandContNum].uBlockNum-1);
	uBlock = UART_GetIntNum();
	UART_Printf("Input the Page Number to test[0~%d]", NAND_Inform[g_NandContNum].uPageNum-1);
	uPage = UART_GetIntNum();	

	
	NAND_EccError.uEccErrBit1 = 0;
	NAND_EccError.uEccErrByte1 = 0;
	NAND_EccError.uEccErrBit2 = 0;
	NAND_EccError.uEccErrByte2 = 0;


	for ( uIndex0 = 0 ; uIndex0 < 4128 ; uIndex0++)
	{

		NAND_Inform[g_NandContNum].uECCtest = 0;
		NAND_Inform[g_NandContNum].uSpareECC = 0;	

		#if 0
		if ( uIndex0 == 4097)
		{
			UART_Printf(".............. Check Spare ECC \n");
			UART_Getc();
		}
		#endif
		
		uByte0 = (uIndex0 / 8);
		uBit0 = uIndex0 - ( uByte0 * 8);
		
		NAND_EccError.uEccErrByte1 = uByte0;
		NAND_EccError.uEccErrBit1 = uBit0;
		NAND_Inform[g_NandContNum].u1BitECCErrBytePos[0] = NAND_EccError.uEccErrByte1 ;
 		NAND_Inform[g_NandContNum].uSLCError1bit_Original[0] = uIndex0;

		for(i=0 ; i<NAND_Inform[g_NandContNum].uPageSize ; i++)
		{
			aBuffer[i] = 0x0;
		}

		for(i=0 ; i<NAND_Inform[g_NandContNum].uSpareSize ; i++)
			aSpareBuffer[i] = 0x0;	

		UART_Printf("\n\n");
		
		UART_Printf("----------------------------------\n");
		UART_Printf("01. Erase the Block ( %d[block])\n", uBlock);
		eError = NAND_EraseSingleBlock_Basic(g_NandContNum, uBlock);
		if(eError != eNAND_NoError)
		{
			NANDT_PrintErrorType_Sample(eError);
			return;
		}

		// 06. Write the valid data to page ( uBlock[block] 0[page] )
		UART_Printf("02. Write the valid data to page for ECC generation ( %d[block] %d[page] )\n", uBlock, uPage);
		NAND_Inform[g_NandContNum].uECCtest = 0;
		eError = NAND_WritePage_1Bit(g_NandContNum, uBlock, uPage, aBuffer, aSpareBuffer);
		if(eError != eNAND_NoError)
		{
			NANDT_PrintErrorType(eError);
			return;
		}

		// 07. Read the valid data from page(uBlock[block] 0[page] )
		UART_Printf("03. Read the valid data in page for ECC generation (%d[block]   %d[Page])\n", uBlock, uPage);
		NAND_Inform[g_NandContNum].uECCtest = 0;
		eError = NAND_ReadPage_1Bit(g_NandContNum, uBlock, uPage, aBuffer, aSpareBuffer);
		if(eError != eNAND_NoError)
		{
			NANDT_PrintErrorType(eError);
			NAND_Inform[g_NandContNum].uECCtest = 0;
			return;
		}
		else
		{
			UART_Printf("        -> NAND Vaild Data Read Page : Success\n");
		}

		// 08. Display test condition
		// Test Start......!!
		UART_Printf("------------ InValid Data Create----------\n");		
		UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte1, NAND_EccError.uEccErrBit1);
		UART_Printf("-------------------------------------------\n");	
		

		// 09. Erase Block for test
		UART_Printf("03. Erase the Block ( %d[block])\n", uBlock);
		eError = NAND_EraseSingleBlock_Basic(g_NandContNum, uBlock);
		if(eError != eNAND_NoError)
		{
			NANDT_PrintErrorType(eError);
			NAND_Inform[g_NandContNum].uECCtest = 0;
			NAND_Inform[g_NandContNum].uAllBitEccCheck = 0;
			return;
		}

		// 10. Create Invalid data
		if ( NAND_Inform[g_NandContNum].u1BitECCErrBytePos[0] < 512 )
		{
			uTemp =	aBuffer[NAND_EccError.uEccErrByte1] ;
			uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit1);
			aBuffer[NAND_EccError.uEccErrByte1] = uTemp;				
			NAND_Inform[g_NandContNum].uECCtest = 1;
		}
		else
		{
			uTemp =	aSpareBuffer[(NAND_EccError.uEccErrByte1)-512] ;
			uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit1);
			aSpareBuffer[(NAND_EccError.uEccErrByte1)-512] = uTemp;				
			NAND_Inform[g_NandContNum].uSpareECC = 1;
//			NAND_Inform[g_NandContNum].uSpareECCtest = 1;
//			NAND_Inform[g_NandContNum].uECCtest = 1;
		}

		// 11. Write Data to NAND Flash
		UART_Printf("04. After invalid data create , write NAND flash\n");
		eError = NAND_WritePage_1Bit(g_NandContNum, uBlock, uPage, aBuffer, aSpareBuffer);
		if(eError != eNAND_NoError)
		{
			NANDT_PrintErrorType(eError);
			NAND_Inform[g_NandContNum].uECCtest = 0;
		}
		else
		{
			UART_Printf("        -> NAND Invalid Data Write Page : Success\n");
		}


		// 12. Read Data from NAND flash
		UART_Printf("05. Read NAND flash\n");


		eError = NAND_ReadPage_1Bit(g_NandContNum, uBlock, uPage, aBuffer, aSpareBuffer);
		if(eError != eNAND_NoError)
		{
			NANDT_PrintErrorType_Sample(eError);
			NAND_Inform[g_NandContNum].uECCtest = 0;
			NAND_Inform[g_NandContNum].uAllBitEccCheck = 0;
			UART_Printf("Read page Error [%d block  %d page]\n", uBlock, uPage);
			UART_Printf("-------------------------------------------\n");	
//			UART_Getc();
		}
		else
		{
			#if 1
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
			#endif
			
			UART_Printf("NAND Read Page : Success\n");

		}

	}


	UART_Printf("\n");

	NAND_Inform[g_NandContNum].uECCtest = 0;
	
	UART_Printf("SLC Nand 1bit ECC test complete & OK...!!\n");
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
	eError = NAND_EraseSingleBlock_Basic(g_NandContNum, uBlock);
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
	eError = NAND_EraseSingleBlock_Basic(g_NandContNum, uBlock);
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
	//u8 aBuffer[NAND_PAGE_MAX];
	//u8 aSpareBuffer[NAND_SPARE_MAX];
	//u8 aReadBuffer[NAND_PAGE_MAX];
	u8 *aBuffer;
	u8 *aSpareBuffer;
	u8 *aReadBuffer;
	NAND_eERROR eError;
	
	UART_Printf("[NANDT_MLCECC_All]\n");

	if(NAND_Inform[g_NandContNum].uNandType != NAND_MLC8bit)
	{
		UART_Printf("The current Nand Memory is not MLC type.....!!\n\n");
		return;
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
	aReadBuffer = (u8 *)malloc(NAND_PAGE_MAX);
	if(aReadBuffer == 0)
	{
		UART_Printf("Memory Allocation Error...\n");
		free(aBuffer);
		free(aSpareBuffer);
		UART_Getc();
		return;
	}
	
	UART_Printf("Input the Block Number to test[0~%d]", NAND_Inform[g_NandContNum].uBlockNum-1);
	uBlock = UART_GetIntNum();
	UART_Printf("Input the Page Number to test[0~%d]", NAND_Inform[g_NandContNum].uPageNum-1);
	uPage = UART_GetIntNum();	
	
	UART_Printf("Input the ECC Error Type\n");
	UART_Printf("1: 1bit error(D),	2: 2bit error,	3: 3bit error,	4: 4bit error,	5: Uncorrectable,  6: Uncorrectable(6bit),  7: Uncorrectable(7bit)\n");
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
		case 6 :	uErrorType = NAND_ECCERR_UNCORRECT6BIT;

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

				UART_Printf("Input the Fifth Byte Postion[0 ~ 2075]");
				NAND_EccError.uEccErrByte6 = UART_GetIntNum()%2076;
				UART_Printf("Input the Fifth Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit6 = UART_GetIntNum()%8;
				
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
				NAND_Inform[g_NandContNum].uMLCECCErrBytePos[5] = NAND_EccError.uEccErrByte6 - 
								(NAND_Inform[g_NandContNum].uMLCECCPageWriteSector * (NAND_MLC_TRANS_SIZE+NAND_MLC_ECC_SIZE));
				
				break;
		case 7 :	uErrorType = NAND_ECCERR_UNCORRECT7BIT;

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

				UART_Printf("Input the Fifth Byte Postion[0 ~ 2075]");
				NAND_EccError.uEccErrByte6 = UART_GetIntNum()%2076;
				UART_Printf("Input the Fifth Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit6 = UART_GetIntNum()%8;

				UART_Printf("Input the Fifth Byte Postion[0 ~ 2075]");
				NAND_EccError.uEccErrByte7 = UART_GetIntNum()%2076;
				UART_Printf("Input the Fifth Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit7 = UART_GetIntNum()%8;

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
				NAND_Inform[g_NandContNum].uMLCECCErrBytePos[5] = NAND_EccError.uEccErrByte6 - 
								(NAND_Inform[g_NandContNum].uMLCECCPageWriteSector * (NAND_MLC_TRANS_SIZE+NAND_MLC_ECC_SIZE));
				NAND_Inform[g_NandContNum].uMLCECCErrBytePos[5] = NAND_EccError.uEccErrByte7 - 
								(NAND_Inform[g_NandContNum].uMLCECCPageWriteSector * (NAND_MLC_TRANS_SIZE+NAND_MLC_ECC_SIZE));
				break;				
		default : 
				free(aBuffer);
				free(aSpareBuffer);
				free(aReadBuffer);				
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
	eError = NAND_EraseSingleBlock_Basic(g_NandContNum, uBlock);
	if(eError != eNAND_NoError)
	{
		NANDT_PrintErrorType(eError);
		NAND_Inform[g_NandContNum].uECCtest = 0;
		NAND_Inform[g_NandContNum].uAllBitEccCheck = 0;
		free(aBuffer);
		free(aSpareBuffer);
		free(aReadBuffer);		
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
		free(aBuffer);
		free(aSpareBuffer);
		free(aReadBuffer);
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
	else if(uErrorType == NAND_ECCERR_UNCORRECT6BIT)
	{
		UART_Printf("[%d byte : %d bit] & [%d byte : %d bit] & [%d byte : %d bit] & [%d byte : %d bit] & [%d byte : %d bit] & [%d byte : %d bit]\n", 	
														NAND_EccError.uEccErrByte1, NAND_EccError.uEccErrBit1,
														NAND_EccError.uEccErrByte2, NAND_EccError.uEccErrBit2,
														NAND_EccError.uEccErrByte3, NAND_EccError.uEccErrBit3,
														NAND_EccError.uEccErrByte4, NAND_EccError.uEccErrBit4,
														NAND_EccError.uEccErrByte5, NAND_EccError.uEccErrBit5,
														NAND_EccError.uEccErrByte6, NAND_EccError.uEccErrBit6);
	}
	else //if(uErrorType == NAND_ECCERR_UNCORRECT7BIT)
	{
		UART_Printf("[%d byte : %d bit] & [%d byte : %d bit] &  [%d byte : %d bit] & [%d byte : %d bit] &  [%d byte : %d bit] & [%d byte : %d bit] & [%d byte : %d bit]\n", 	
														NAND_EccError.uEccErrByte1, NAND_EccError.uEccErrBit1,
														NAND_EccError.uEccErrByte2, NAND_EccError.uEccErrBit2,
														NAND_EccError.uEccErrByte3, NAND_EccError.uEccErrBit3,
														NAND_EccError.uEccErrByte4, NAND_EccError.uEccErrBit4,
														NAND_EccError.uEccErrByte5, NAND_EccError.uEccErrBit5,
														NAND_EccError.uEccErrByte6, NAND_EccError.uEccErrBit6,
														NAND_EccError.uEccErrByte7, NAND_EccError.uEccErrBit7);
	}
	// Erase Block for test
	//UART_Printf("Erase the Block ( %d[block])\n", uBlock);
	eError = NAND_EraseSingleBlock_Basic(g_NandContNum, uBlock);
	if(eError != eNAND_NoError)
	{
		NANDT_PrintErrorType(eError);
		NAND_Inform[g_NandContNum].uECCtest = 0;
		NAND_Inform[g_NandContNum].uAllBitEccCheck = 0;
		free(aBuffer);
		free(aSpareBuffer);
		free(aReadBuffer);
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
		free(aBuffer);
		free(aSpareBuffer);
		free(aReadBuffer);
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
			free(aBuffer);
			free(aSpareBuffer);
			free(aReadBuffer);
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
			free(aBuffer);
			free(aSpareBuffer);
			free(aReadBuffer);
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
			free(aBuffer);
			free(aSpareBuffer);
			free(aReadBuffer);
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
			free(aBuffer);
			free(aSpareBuffer);
			free(aReadBuffer);
			return;
		}
	}
	else //if(uErrorType == NAND_ECCERR_UNCORRECT)
	{
		if( aNANDT_EccError[0][0] != NAND_ECC_UNCORRECT_ERROR)
		{
			UART_Printf("Uncorrectable Ecc Error Check Error\n");	
			aNANDT_EccError[0][0] = 0;
			UART_Getc();
			NAND_Inform[g_NandContNum].uECCtest = 0;
			NAND_Inform[g_NandContNum].uAllBitEccCheck = 0;
			free(aBuffer);
			free(aSpareBuffer);
			free(aReadBuffer);
			return;
		}
		aNANDT_EccError[0][0] = 0;
	}				

	NAND_Inform[g_NandContNum].uECCtest = 0;
	NAND_Inform[g_NandContNum].uAllBitEccCheck = 0;
	
	free(aBuffer);
	free(aSpareBuffer);
	free(aReadBuffer);
	
	UART_Printf("MLC Nand ECC test complete & OK...!!\n");
	UART_Printf("\n");	
}

//////////
// Function Name : NANDT_8BitECC_SLC
// Function Description : MLC ECC function test
// Input : 	None
// Output : 	None
void NANDT_8BitECC_SLC_24Byte(void)
{
	u32 i, uBlock, uPage, uError, uErrorType;
	u8 aBuffer[NAND_PAGE_512];
	u8 aSpareBuffer[NAND_SPARE_16];
//	u8 aReadBuffer[NAND_PAGE_512];
	NAND_eERROR eError;
	u32 uTemp;
	u32 uIndex0;


	NAND_EccError.uEccErrBit1 = 0;
	NAND_EccError.uEccErrByte1 = 0;
	NAND_EccError.uEccErrBit2 = 0;
	NAND_EccError.uEccErrByte2 = 0;
	NAND_EccError.uEccErrBit3 = 0;
	NAND_EccError.uEccErrByte3 = 0;
	NAND_EccError.uEccErrBit4 = 0;
	NAND_EccError.uEccErrByte4 = 0;
	NAND_EccError.uEccErrBit5 = 0;
	NAND_EccError.uEccErrByte5 = 0;
	NAND_EccError.uEccErrBit6 = 0;
	NAND_EccError.uEccErrByte6 = 0;
	NAND_EccError.uEccErrBit7 = 0;
	NAND_EccError.uEccErrByte7 = 0;
	NAND_EccError.uEccErrBit8 = 0;
	NAND_EccError.uEccErrByte8 = 0;

	NAND_Inform[g_NandContNum].uECCtest = 0;
	NAND_Inform[g_NandContNum].uECCType = ECC8Bit;
	
	UART_Printf("[NAND_8BitECC_BasicTestl]\n");

	// 01. Input wanted 8bit ecc test block
	UART_Printf("Input the Block Number to test[0~%d]", NAND_Inform[g_NandContNum].uBlockNum-1);
	uBlock = UART_GetIntNum();
	UART_Printf("Input the Page Number to test[0~%d]", NAND_Inform[g_NandContNum].uPageNum-1);
	uPage = UART_GetIntNum();	

	// 02. select ecc type
	UART_Printf("Input the ECC Error Type\n");
	UART_Printf("1: 1bit error(D)	2: 2bit Error	3: 3bit Error\n");
	UART_Printf("4: 4bit error		5: 5bit Error	6: 6bit Error\n");	
	UART_Printf("7: 7bit error		8: 8bit Error\n");
	uError = UART_GetIntNum();

	// 03. Input byte positoin and bit position
	switch(uError)
	{
		case 1 :	uErrorType = NAND_ECCERR_1BIT;
				UART_Printf("Input the First Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte1 = UART_GetIntNum()%528;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[0]=NAND_EccError.uEccErrByte1;
				UART_Printf("Input the First Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit1 = UART_GetIntNum()%8;
//				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[0] = NAND_EccError.uEccErrByte1 ;
				break;
		case 2 :	uErrorType = NAND_ECCERR_2BIT;
				UART_Printf("Input the 1st Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte1 = UART_GetIntNum()%528;
				UART_Printf("Input the 1st Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit1 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[0]=NAND_EccError.uEccErrByte1;
				UART_Printf("Input the 2nd Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte2 = UART_GetIntNum()%528;
				UART_Printf("Input the 2nd Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit2 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[1]=NAND_EccError.uEccErrByte2;
				break;
		case 3 :	uErrorType = NAND_ECCERR_3BIT;
				UART_Printf("Input the 1st Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte1 = UART_GetIntNum()%528;
				UART_Printf("Input the 1st Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit1 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[0]=NAND_EccError.uEccErrByte1;
				UART_Printf("Input the 2nd Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte2 = UART_GetIntNum()%528;
				UART_Printf("Input the 2nd Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit2 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[1]=NAND_EccError.uEccErrByte2;
				UART_Printf("Input the 3th Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte3 = UART_GetIntNum()%528;
				UART_Printf("Input the 3th Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit3 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[2]=NAND_EccError.uEccErrByte3;
				break;
		case 4 :	uErrorType = NAND_ECCERR_4BIT;
				UART_Printf("Input the 1st Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte1 = UART_GetIntNum()%528;
				UART_Printf("Input the 1st Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit1 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[0]=NAND_EccError.uEccErrByte1;
				UART_Printf("Input the 2nd Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte2 = UART_GetIntNum()%528;
				UART_Printf("Input the 2nd Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit2 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[1]=NAND_EccError.uEccErrByte2;
				UART_Printf("Input the 3th Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte3 = UART_GetIntNum()%528;
				UART_Printf("Input the 3th Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit3 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[2]=NAND_EccError.uEccErrByte3;
				UART_Printf("Input the 4th Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte4 = UART_GetIntNum()%528;
				UART_Printf("Input the 4th Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit4 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[3]=NAND_EccError.uEccErrByte4;
				break;
		case 5 :	uErrorType = NAND_ECCERR_5BIT;
				UART_Printf("Input the 1st Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte1 = UART_GetIntNum()%528;
				UART_Printf("Input the 1st Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit1 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[0]=NAND_EccError.uEccErrByte1;
				UART_Printf("Input the 2nd Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte2 = UART_GetIntNum()%528;
				UART_Printf("Input the 2nd Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit2 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[1]=NAND_EccError.uEccErrByte2;
				UART_Printf("Input the 3th Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte3 = UART_GetIntNum()%528;
				UART_Printf("Input the 3th Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit3 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[2]=NAND_EccError.uEccErrByte3;
				UART_Printf("Input the 4th Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte4 = UART_GetIntNum()%528;
				UART_Printf("Input the 4th Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit4 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[3]=NAND_EccError.uEccErrByte4;
				UART_Printf("Input the 5th Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte5 = UART_GetIntNum()%528;
				UART_Printf("Input the 5th Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit5 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[4]=NAND_EccError.uEccErrByte5;
				break;
		case 6 :	uErrorType = NAND_ECCERR_6BIT;
				UART_Printf("Input the 1st Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte1 = UART_GetIntNum()%528;
				UART_Printf("Input the 1st Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit1 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[0]=NAND_EccError.uEccErrByte1;
				UART_Printf("Input the 2nd Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte2 = UART_GetIntNum()%528;
				UART_Printf("Input the 2nd Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit2 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[1]=NAND_EccError.uEccErrByte2;
				UART_Printf("Input the 3th Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte3 = UART_GetIntNum()%528;
				UART_Printf("Input the 3th Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit3 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[2]=NAND_EccError.uEccErrByte3;
				UART_Printf("Input the 4th Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte4 = UART_GetIntNum()%528;
				UART_Printf("Input the 4th Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit4 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[3]=NAND_EccError.uEccErrByte4;
				UART_Printf("Input the 5th Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte5 = UART_GetIntNum()%528;
				UART_Printf("Input the 5th Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit5 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[4]=NAND_EccError.uEccErrByte5;
				UART_Printf("Input the 6th Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte6 = UART_GetIntNum()%528;
				UART_Printf("Input the 6th Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit6 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[5]=NAND_EccError.uEccErrByte6;
				break;
		case 7 :	uErrorType = NAND_ECCERR_7BIT;
				UART_Printf("Input the 1st Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte1 = UART_GetIntNum()%528;
				UART_Printf("Input the 1st Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit1 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[0]=NAND_EccError.uEccErrByte1;
				UART_Printf("Input the 2nd Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte2 = UART_GetIntNum()%528;
				UART_Printf("Input the 2nd Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit2 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[1]=NAND_EccError.uEccErrByte2;
				UART_Printf("Input the 3th Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte3 = UART_GetIntNum()%528;
				UART_Printf("Input the 3th Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit3 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[2]=NAND_EccError.uEccErrByte3;
				UART_Printf("Input the 4th Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte4 = UART_GetIntNum()%528;
				UART_Printf("Input the 4th Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit4 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[3]=NAND_EccError.uEccErrByte4;
				UART_Printf("Input the 5th Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte5 = UART_GetIntNum()%528;
				UART_Printf("Input the 5th Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit5 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[4]=NAND_EccError.uEccErrByte5;
				UART_Printf("Input the 6th Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte6 = UART_GetIntNum()%528;
				UART_Printf("Input the 6th Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit6 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[5]=NAND_EccError.uEccErrByte6;
				UART_Printf("Input the 7th Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte7 = UART_GetIntNum()%528;
				UART_Printf("Input the 7th Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit7 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[6]=NAND_EccError.uEccErrByte7;
				break;
		case 8 :	uErrorType = NAND_ECCERR_8BIT;
				UART_Printf("Input the 1st Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte1 = UART_GetIntNum()%528;
				UART_Printf("Input the 1st Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit1 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[0]=NAND_EccError.uEccErrByte1;
				UART_Printf("Input the 2nd Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte2 = UART_GetIntNum()%528;
				UART_Printf("Input the 2nd Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit2 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[1]=NAND_EccError.uEccErrByte2;
				UART_Printf("Input the 3th Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte3 = UART_GetIntNum()%528;
				UART_Printf("Input the 3th Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit3 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[2]=NAND_EccError.uEccErrByte3;
				UART_Printf("Input the 4th Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte4 = UART_GetIntNum()%528;
				UART_Printf("Input the 4th Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit4 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[3]=NAND_EccError.uEccErrByte4;
				UART_Printf("Input the 5th Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte5 = UART_GetIntNum()%528;
				UART_Printf("Input the 5th Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit5 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[4]=NAND_EccError.uEccErrByte5;
				UART_Printf("Input the 6th Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte6 = UART_GetIntNum()%528;
				UART_Printf("Input the 6th Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit6 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[5]=NAND_EccError.uEccErrByte6;
				UART_Printf("Input the 7th Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte7 = UART_GetIntNum()%528;
				UART_Printf("Input the 7th Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit7 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[6]=NAND_EccError.uEccErrByte7;
				UART_Printf("Input the 8th Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte8 = UART_GetIntNum()%528;
				UART_Printf("Input the 8th Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit8 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[7]=NAND_EccError.uEccErrByte8;
				break;
		default : uErrorType = 0;
			break;
	}

	// 04. Save page and spare data to buffer : aBuffer and aSpareBuffer
	for(i=0 ; i<NAND_Inform[g_NandContNum].uPageSize ; i++)
	{
		aBuffer[i] = 0x0;
	}
	for(i=0 ; i<NAND_Inform[g_NandContNum].uSpareSize ; i++)
		aSpareBuffer[i] = 0x0;	
#if 0 
	for(m=0 ; m<4 ; m++)
		for(n=0 ; n<2 ; n++)
			aNANDT_EccError[m][n] = 0xFFFFFFFE;
#endif

	UART_Printf("\n");
	
	// 05. Erase Block for test
	UART_Printf("----------------------------------\n");
	UART_Printf("01. Erase the Block ( %d[block])\n", uBlock);
	eError = NAND_EraseSingleBlock_Basic(g_NandContNum, uBlock);
	if(eError != eNAND_NoError)
	{
		NANDT_PrintErrorType_Sample(eError);
		return;
	}

	// 06. Write the valid data to page ( uBlock[block] 0[page] )
	UART_Printf("02. Write the valid data to page for ECC generation ( %d[block] %d[page] )\n", uBlock, uPage);
	NAND_Inform[g_NandContNum].uECCtest = 0;
	eError = NAND_WritePage_8Bit_24Byte(g_NandContNum, uBlock, uPage, aBuffer, aSpareBuffer);
	if(eError != eNAND_NoError)
	{
		NANDT_PrintErrorType_Sample(eError);
		return;
	}

	// 07. Read the valid data from page(uBlock[block] 0[page] )
	UART_Printf("03. Read the valid data in page for ECC generation (%d[block]   %d[Page])\n", uBlock, uPage);
	eError = NAND_ReadPage_8Bit(g_NandContNum, uBlock, uPage, aBuffer, aSpareBuffer);
	if(eError != eNAND_NoError)
	{
		NANDT_PrintErrorType_Sample(eError);
		NAND_Inform[g_NandContNum].uECCtest = 0;
		return;
	}
	else
	{
#if 0
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
#endif		
		UART_Printf("NAND Vaild Data Read Page : Success\n");
	}

	// 08. Display test condition
	// Test Start......!!
	UART_Printf("-------------------------------------------\n");	
	UART_Printf("-------------- InValid Data Create   ----------\n");		
	switch (uError)
	{
		case 1: 
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte1, NAND_EccError.uEccErrBit1);
				break;
		case 2: 
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte1, NAND_EccError.uEccErrBit1);
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte2, NAND_EccError.uEccErrBit2);
				break;
		case 3: 
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte1, NAND_EccError.uEccErrBit1);
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte2, NAND_EccError.uEccErrBit2);
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte3, NAND_EccError.uEccErrBit3);
				break;
		case 4: 
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte1, NAND_EccError.uEccErrBit1);
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte2, NAND_EccError.uEccErrBit2);
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte3, NAND_EccError.uEccErrBit3);
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte4, NAND_EccError.uEccErrBit4);
				break;
		case 5: 
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte1, NAND_EccError.uEccErrBit1);
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte2, NAND_EccError.uEccErrBit2);
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte3, NAND_EccError.uEccErrBit3);
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte4, NAND_EccError.uEccErrBit4);
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte5, NAND_EccError.uEccErrBit5);	
				break;
		case 6: 
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte1, NAND_EccError.uEccErrBit1);
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte2, NAND_EccError.uEccErrBit2);
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte3, NAND_EccError.uEccErrBit3);
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte4, NAND_EccError.uEccErrBit4);
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte5, NAND_EccError.uEccErrBit5);	
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte6, NAND_EccError.uEccErrBit6);
				break;
		case 7: 
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte1, NAND_EccError.uEccErrBit1);
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte2, NAND_EccError.uEccErrBit2);
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte3, NAND_EccError.uEccErrBit3);
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte4, NAND_EccError.uEccErrBit4);
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte5, NAND_EccError.uEccErrBit5);	
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte6, NAND_EccError.uEccErrBit6);
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte7, NAND_EccError.uEccErrBit7);
				break;
		case 8: 
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte1, NAND_EccError.uEccErrBit1);
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte2, NAND_EccError.uEccErrBit2);
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte3, NAND_EccError.uEccErrBit3);
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte4, NAND_EccError.uEccErrBit4);
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte5, NAND_EccError.uEccErrBit5);	
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte6, NAND_EccError.uEccErrBit6);
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte7, NAND_EccError.uEccErrBit7);
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte8, NAND_EccError.uEccErrBit8);
				break;

		}
	UART_Printf("-------------------------------------------\n");	
	

	// 09. Erase Block for test
	UART_Printf("03. Erase the Block ( %d[block])\n", uBlock);
	eError = NAND_EraseSingleBlock_Basic(g_NandContNum, uBlock);
	if(eError != eNAND_NoError)
	{
		NANDT_PrintErrorType_Sample(eError);
		NAND_Inform[g_NandContNum].uECCtest = 0;
		NAND_Inform[g_NandContNum].uAllBitEccCheck = 0;
		return;
	}

	// 10. Create Invalid data

	for ( uIndex0 =0 ; uIndex0<uError ; uIndex0++)
	{
		switch ( uIndex0+1)
		{
			case 1:
				if ( NAND_Inform[g_NandContNum].u8BitECCErrBytePos[uIndex0] < 512 )
				{
					uTemp =	aBuffer[NAND_EccError.uEccErrByte1] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit1);
					aBuffer[NAND_EccError.uEccErrByte1] = uTemp;				
				}
				else
				{
					uTemp =	aSpareBuffer[(NAND_EccError.uEccErrByte1)-512] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit1);
					aSpareBuffer[(NAND_EccError.uEccErrByte1)-512] = uTemp;				
				}
				break;
			case 2:
				if ( NAND_Inform[g_NandContNum].u8BitECCErrBytePos[uIndex0] < 512 )
				{
					uTemp =	aBuffer[NAND_EccError.uEccErrByte2] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit2);
					aBuffer[NAND_EccError.uEccErrByte2] = uTemp;				
				}
				else
				{
					uTemp =	aSpareBuffer[(NAND_EccError.uEccErrByte2)-512] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit2);
					aSpareBuffer[(NAND_EccError.uEccErrByte2)-512] = uTemp;				
				}
				break;
			case 3:
				if ( NAND_Inform[g_NandContNum].u8BitECCErrBytePos[uIndex0] < 512 )
				{
					uTemp =	aBuffer[NAND_EccError.uEccErrByte3] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit3);
					aBuffer[NAND_EccError.uEccErrByte3] = uTemp;				
				}
				else
				{
					uTemp =	aSpareBuffer[(NAND_EccError.uEccErrByte3)-512] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit3);
					aSpareBuffer[(NAND_EccError.uEccErrByte3)-512] = uTemp;				
				}
				break;
			case 4:
				if ( NAND_Inform[g_NandContNum].u8BitECCErrBytePos[uIndex0] < 512 )
				{
					uTemp =	aBuffer[NAND_EccError.uEccErrByte4] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit4);
					aBuffer[NAND_EccError.uEccErrByte4] = uTemp;				
				}
				else
				{
					uTemp =	aSpareBuffer[(NAND_EccError.uEccErrByte4)-512] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit4);
					aSpareBuffer[(NAND_EccError.uEccErrByte4)-512] = uTemp;				
				}
				break;
			case 5:
				if ( NAND_Inform[g_NandContNum].u8BitECCErrBytePos[uIndex0] < 512 )
				{
					uTemp =	aBuffer[NAND_EccError.uEccErrByte5] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit5);
					aBuffer[NAND_EccError.uEccErrByte5] = uTemp;				
				}
				else
				{
					uTemp =	aSpareBuffer[(NAND_EccError.uEccErrByte5)-512] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit5);
					aSpareBuffer[(NAND_EccError.uEccErrByte5)-512] = uTemp;				
				}
				break;
			case 6:
				if ( NAND_Inform[g_NandContNum].u8BitECCErrBytePos[uIndex0] < 512 )
				{
					uTemp =	aBuffer[NAND_EccError.uEccErrByte6] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit6);
					aBuffer[NAND_EccError.uEccErrByte6] = uTemp;				
				}
				else
				{
					uTemp =	aSpareBuffer[(NAND_EccError.uEccErrByte6)-512] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit6);
					aSpareBuffer[(NAND_EccError.uEccErrByte6)-512] = uTemp;				
				}
				break;
			case 7:
				if ( NAND_Inform[g_NandContNum].u8BitECCErrBytePos[uIndex0] < 512 )
				{
					uTemp =	aBuffer[NAND_EccError.uEccErrByte7] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit7);
					aBuffer[NAND_EccError.uEccErrByte7] = uTemp;				
				}
				else
				{
					uTemp =	aSpareBuffer[(NAND_EccError.uEccErrByte7)-512] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit7);
					aSpareBuffer[(NAND_EccError.uEccErrByte7)-512] = uTemp;				
				}
				break;
			case 8:
				if ( NAND_Inform[g_NandContNum].u8BitECCErrBytePos[uIndex0] < 512 )
				{
					uTemp =	aBuffer[NAND_EccError.uEccErrByte8] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit8);
					aBuffer[NAND_EccError.uEccErrByte8] = uTemp;				
				}
				else
				{
					uTemp =	aSpareBuffer[(NAND_EccError.uEccErrByte8)-512] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit8);
					aSpareBuffer[(NAND_EccError.uEccErrByte8)-512] = uTemp;				
				}
				break;
			default: 
				break;
		}

	}


	// 11. Write Data to NAND Flash
	UART_Printf("04. After invalid data create , write NAND flash\n");
	NAND_Inform[g_NandContNum].uECCtest = 1;
	eError = NAND_WritePage_8Bit(g_NandContNum, uBlock, uPage, aBuffer, aSpareBuffer);
	if(eError != eNAND_NoError)
	{
		NANDT_PrintErrorType_Sample(eError);
		NAND_Inform[g_NandContNum].uECCtest = 0;
	}
	else
	{
#if 0
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
#endif		
		UART_Printf("NAND Invalid Data Write Page : Success\n");
	}

	UART_Printf("\n");

	// 12. Read Data from NAND flash
	UART_Printf("05. Read NAND flash\n");
	NAND_Inform[g_NandContNum].uECCtest = 1;
	eError = NAND_ReadPage_8Bit(g_NandContNum, uBlock, uPage, aBuffer, aSpareBuffer);
	if(eError != eNAND_NoError)
	{
		NANDT_PrintErrorType_Sample(eError);
		NAND_Inform[g_NandContNum].uECCtest = 0;
		NAND_Inform[g_NandContNum].uAllBitEccCheck = 0;
		UART_Printf("Read page Error [%d block  %d page]\n", uBlock, uPage);
		UART_Printf("-------------------------------------------\n");	
#if 0 
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
#endif		
	}
	else
	{
#if 0
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
#endif		
	}
	UART_Printf("\n");

	
	NAND_Inform[g_NandContNum].uECCtest = 0;
	NAND_Inform[g_NandContNum].uAllBitEccCheck = 0;
	
	UART_Printf("SLC Nand 8bit ECC test complete & OK...!!\n");
	UART_Printf("\n");	
}




//////////
// Function Name : NANDT_8BitECC_SLC
// Function Description : MLC ECC function test
// Input : 	None
// Output : 	None
void NANDT_8BitECC_SLC(void)
{
	u32 i, uBlock, uPage, uError, uErrorType;
	u8 aBuffer[NAND_PAGE_512];
	u8 aSpareBuffer[NAND_SPARE_16];
//	u8 aReadBuffer[NAND_PAGE_512];
	NAND_eERROR eError;
	u32 uTemp;
	u32 uIndex0;


	NAND_EccError.uEccErrBit1 = 0;
	NAND_EccError.uEccErrByte1 = 0;
	NAND_EccError.uEccErrBit2 = 0;
	NAND_EccError.uEccErrByte2 = 0;
	NAND_EccError.uEccErrBit3 = 0;
	NAND_EccError.uEccErrByte3 = 0;
	NAND_EccError.uEccErrBit4 = 0;
	NAND_EccError.uEccErrByte4 = 0;
	NAND_EccError.uEccErrBit5 = 0;
	NAND_EccError.uEccErrByte5 = 0;
	NAND_EccError.uEccErrBit6 = 0;
	NAND_EccError.uEccErrByte6 = 0;
	NAND_EccError.uEccErrBit7 = 0;
	NAND_EccError.uEccErrByte7 = 0;
	NAND_EccError.uEccErrBit8 = 0;
	NAND_EccError.uEccErrByte8 = 0;



	NAND_Inform[g_NandContNum].uECCtest = 0;
	NAND_Inform[g_NandContNum].uECCType = ECC8Bit;
	
	UART_Printf("[NAND_8BitECC_BasicTestl]\n");

	// 01. Input wanted 8bit ecc test block
	UART_Printf("Input the Block Number to test[0~%d]", NAND_Inform[g_NandContNum].uBlockNum-1);
	uBlock = UART_GetIntNum();
	UART_Printf("Input the Page Number to test[0~%d]", NAND_Inform[g_NandContNum].uPageNum-1);
	uPage = UART_GetIntNum();	

	// 02. select ecc type
	UART_Printf("Input the ECC Error Type\n");
	UART_Printf("1: 1bit error(D)	2: 2bit Error	3: 3bit Error\n");
	UART_Printf("4: 4bit error		5: 5bit Error	6: 6bit Error\n");	
	UART_Printf("7: 7bit error		8: 8bit Error\n");
	uError = UART_GetIntNum();

	// 03. Input byte positoin and bit position
	switch(uError)
	{
		case 1 :	uErrorType = NAND_ECCERR_1BIT;
				UART_Printf("Input the First Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte1 = UART_GetIntNum()%528;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[0]=NAND_EccError.uEccErrByte1;
				UART_Printf("Input the First Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit1 = UART_GetIntNum()%8;
//				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[0] = NAND_EccError.uEccErrByte1 ;
				break;
		case 2 :	uErrorType = NAND_ECCERR_2BIT;
				UART_Printf("Input the 1st Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte1 = UART_GetIntNum()%528;
				UART_Printf("Input the 1st Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit1 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[0]=NAND_EccError.uEccErrByte1;
				UART_Printf("Input the 2nd Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte2 = UART_GetIntNum()%528;
				UART_Printf("Input the 2nd Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit2 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[1]=NAND_EccError.uEccErrByte2;
				break;
		case 3 :	uErrorType = NAND_ECCERR_3BIT;
				UART_Printf("Input the 1st Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte1 = UART_GetIntNum()%528;
				UART_Printf("Input the 1st Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit1 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[0]=NAND_EccError.uEccErrByte1;
				UART_Printf("Input the 2nd Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte2 = UART_GetIntNum()%528;
				UART_Printf("Input the 2nd Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit2 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[1]=NAND_EccError.uEccErrByte2;
				UART_Printf("Input the 3th Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte3 = UART_GetIntNum()%528;
				UART_Printf("Input the 3th Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit3 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[2]=NAND_EccError.uEccErrByte3;
				break;
		case 4 :	uErrorType = NAND_ECCERR_4BIT;
				UART_Printf("Input the 1st Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte1 = UART_GetIntNum()%528;
				UART_Printf("Input the 1st Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit1 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[0]=NAND_EccError.uEccErrByte1;
				UART_Printf("Input the 2nd Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte2 = UART_GetIntNum()%528;
				UART_Printf("Input the 2nd Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit2 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[1]=NAND_EccError.uEccErrByte2;
				UART_Printf("Input the 3th Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte3 = UART_GetIntNum()%528;
				UART_Printf("Input the 3th Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit3 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[2]=NAND_EccError.uEccErrByte3;
				UART_Printf("Input the 4th Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte4 = UART_GetIntNum()%528;
				UART_Printf("Input the 4th Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit4 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[3]=NAND_EccError.uEccErrByte4;
				break;
		case 5 :	uErrorType = NAND_ECCERR_5BIT;
				UART_Printf("Input the 1st Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte1 = UART_GetIntNum()%528;
				UART_Printf("Input the 1st Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit1 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[0]=NAND_EccError.uEccErrByte1;
				UART_Printf("Input the 2nd Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte2 = UART_GetIntNum()%528;
				UART_Printf("Input the 2nd Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit2 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[1]=NAND_EccError.uEccErrByte2;
				UART_Printf("Input the 3th Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte3 = UART_GetIntNum()%528;
				UART_Printf("Input the 3th Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit3 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[2]=NAND_EccError.uEccErrByte3;
				UART_Printf("Input the 4th Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte4 = UART_GetIntNum()%528;
				UART_Printf("Input the 4th Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit4 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[3]=NAND_EccError.uEccErrByte4;
				UART_Printf("Input the 5th Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte5 = UART_GetIntNum()%528;
				UART_Printf("Input the 5th Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit5 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[4]=NAND_EccError.uEccErrByte5;
				break;
		case 6 :	uErrorType = NAND_ECCERR_6BIT;
				UART_Printf("Input the 1st Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte1 = UART_GetIntNum()%528;
				UART_Printf("Input the 1st Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit1 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[0]=NAND_EccError.uEccErrByte1;
				UART_Printf("Input the 2nd Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte2 = UART_GetIntNum()%528;
				UART_Printf("Input the 2nd Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit2 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[1]=NAND_EccError.uEccErrByte2;
				UART_Printf("Input the 3th Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte3 = UART_GetIntNum()%528;
				UART_Printf("Input the 3th Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit3 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[2]=NAND_EccError.uEccErrByte3;
				UART_Printf("Input the 4th Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte4 = UART_GetIntNum()%528;
				UART_Printf("Input the 4th Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit4 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[3]=NAND_EccError.uEccErrByte4;
				UART_Printf("Input the 5th Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte5 = UART_GetIntNum()%528;
				UART_Printf("Input the 5th Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit5 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[4]=NAND_EccError.uEccErrByte5;
				UART_Printf("Input the 6th Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte6 = UART_GetIntNum()%528;
				UART_Printf("Input the 6th Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit6 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[5]=NAND_EccError.uEccErrByte6;
				break;
		case 7 :	uErrorType = NAND_ECCERR_7BIT;
				UART_Printf("Input the 1st Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte1 = UART_GetIntNum()%528;
				UART_Printf("Input the 1st Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit1 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[0]=NAND_EccError.uEccErrByte1;
				UART_Printf("Input the 2nd Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte2 = UART_GetIntNum()%528;
				UART_Printf("Input the 2nd Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit2 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[1]=NAND_EccError.uEccErrByte2;
				UART_Printf("Input the 3th Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte3 = UART_GetIntNum()%528;
				UART_Printf("Input the 3th Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit3 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[2]=NAND_EccError.uEccErrByte3;
				UART_Printf("Input the 4th Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte4 = UART_GetIntNum()%528;
				UART_Printf("Input the 4th Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit4 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[3]=NAND_EccError.uEccErrByte4;
				UART_Printf("Input the 5th Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte5 = UART_GetIntNum()%528;
				UART_Printf("Input the 5th Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit5 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[4]=NAND_EccError.uEccErrByte5;
				UART_Printf("Input the 6th Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte6 = UART_GetIntNum()%528;
				UART_Printf("Input the 6th Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit6 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[5]=NAND_EccError.uEccErrByte6;
				UART_Printf("Input the 7th Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte7 = UART_GetIntNum()%528;
				UART_Printf("Input the 7th Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit7 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[6]=NAND_EccError.uEccErrByte7;
				break;
		case 8 :	uErrorType = NAND_ECCERR_8BIT;
				UART_Printf("Input the 1st Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte1 = UART_GetIntNum()%528;
				UART_Printf("Input the 1st Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit1 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[0]=NAND_EccError.uEccErrByte1;
				UART_Printf("Input the 2nd Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte2 = UART_GetIntNum()%528;
				UART_Printf("Input the 2nd Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit2 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[1]=NAND_EccError.uEccErrByte2;
				UART_Printf("Input the 3th Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte3 = UART_GetIntNum()%528;
				UART_Printf("Input the 3th Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit3 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[2]=NAND_EccError.uEccErrByte3;
				UART_Printf("Input the 4th Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte4 = UART_GetIntNum()%528;
				UART_Printf("Input the 4th Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit4 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[3]=NAND_EccError.uEccErrByte4;
				UART_Printf("Input the 5th Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte5 = UART_GetIntNum()%528;
				UART_Printf("Input the 5th Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit5 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[4]=NAND_EccError.uEccErrByte5;
				UART_Printf("Input the 6th Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte6 = UART_GetIntNum()%528;
				UART_Printf("Input the 6th Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit6 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[5]=NAND_EccError.uEccErrByte6;
				UART_Printf("Input the 7th Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte7 = UART_GetIntNum()%528;
				UART_Printf("Input the 7th Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit7 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[6]=NAND_EccError.uEccErrByte7;
				UART_Printf("Input the 8th Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte8 = UART_GetIntNum()%528;
				UART_Printf("Input the 8th Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit8 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[7]=NAND_EccError.uEccErrByte8;
				break;
		default : uErrorType = 0;
			break;
	}

	// 04. Save page and spare data to buffer : aBuffer and aSpareBuffer
	for(i=0 ; i<NAND_Inform[g_NandContNum].uPageSize ; i++)
	{
		aBuffer[i] = 0x0;
	}
	for(i=0 ; i<NAND_Inform[g_NandContNum].uSpareSize ; i++)
		aSpareBuffer[i] = 0x0;	
#if 0 
	for(m=0 ; m<4 ; m++)
		for(n=0 ; n<2 ; n++)
			aNANDT_EccError[m][n] = 0xFFFFFFFE;
#endif

	UART_Printf("\n");
	
	// 05. Erase Block for test
	UART_Printf("----------------------------------\n");
	UART_Printf("01. Erase the Block ( %d[block])\n", uBlock);
	eError = NAND_EraseSingleBlock_Basic(g_NandContNum, uBlock);
	if(eError != eNAND_NoError)
	{
		NANDT_PrintErrorType_Sample(eError);
		return;
	}

	// 06. Write the valid data to page ( uBlock[block] 0[page] )
	UART_Printf("02. Write the valid data to page for ECC generation ( %d[block] %d[page] )\n", uBlock, uPage);
	NAND_Inform[g_NandContNum].uECCtest = 0;
	eError = NAND_WritePage_8Bit(g_NandContNum, uBlock, uPage, aBuffer, aSpareBuffer);	
	if(eError != eNAND_NoError)
	{
		NANDT_PrintErrorType_Sample(eError);
		return;
	}

	// 07. Read the valid data from page(uBlock[block] 0[page] )
	UART_Printf("03. Read the valid data in page for ECC generation (%d[block]   %d[Page])\n", uBlock, uPage);
	eError = NAND_ReadPage_8Bit(g_NandContNum, uBlock, uPage, aBuffer, aSpareBuffer);
	if(eError != eNAND_NoError)
	{
		NANDT_PrintErrorType_Sample(eError);
		NAND_Inform[g_NandContNum].uECCtest = 0;
		return;
	}
	else
	{
#if 0
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
#endif		
		UART_Printf("NAND Vaild Data Read Page : Success\n");
	}

	// 08. Display test condition
	// Test Start......!!
	UART_Printf("-------------------------------------------\n");	
	UART_Printf("-------------- InValid Data Create   ----------\n");		
	switch (uError)
	{
		case 1: 
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte1, NAND_EccError.uEccErrBit1);
				break;
		case 2: 
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte1, NAND_EccError.uEccErrBit1);
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte2, NAND_EccError.uEccErrBit2);
				break;
		case 3: 
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte1, NAND_EccError.uEccErrBit1);
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte2, NAND_EccError.uEccErrBit2);
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte3, NAND_EccError.uEccErrBit3);
				break;
		case 4: 
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte1, NAND_EccError.uEccErrBit1);
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte2, NAND_EccError.uEccErrBit2);
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte3, NAND_EccError.uEccErrBit3);
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte4, NAND_EccError.uEccErrBit4);
				break;
		case 5: 
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte1, NAND_EccError.uEccErrBit1);
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte2, NAND_EccError.uEccErrBit2);
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte3, NAND_EccError.uEccErrBit3);
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte4, NAND_EccError.uEccErrBit4);
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte5, NAND_EccError.uEccErrBit5);	
				break;
		case 6: 
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte1, NAND_EccError.uEccErrBit1);
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte2, NAND_EccError.uEccErrBit2);
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte3, NAND_EccError.uEccErrBit3);
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte4, NAND_EccError.uEccErrBit4);
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte5, NAND_EccError.uEccErrBit5);	
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte6, NAND_EccError.uEccErrBit6);
				break;
		case 7: 
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte1, NAND_EccError.uEccErrBit1);
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte2, NAND_EccError.uEccErrBit2);
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte3, NAND_EccError.uEccErrBit3);
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte4, NAND_EccError.uEccErrBit4);
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte5, NAND_EccError.uEccErrBit5);	
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte6, NAND_EccError.uEccErrBit6);
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte7, NAND_EccError.uEccErrBit7);
				break;
		case 8: 
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte1, NAND_EccError.uEccErrBit1);
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte2, NAND_EccError.uEccErrBit2);
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte3, NAND_EccError.uEccErrBit3);
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte4, NAND_EccError.uEccErrBit4);
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte5, NAND_EccError.uEccErrBit5);	
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte6, NAND_EccError.uEccErrBit6);
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte7, NAND_EccError.uEccErrBit7);
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte8, NAND_EccError.uEccErrBit8);
				break;

		}
	UART_Printf("-------------------------------------------\n");	
	

	// 09. Erase Block for test
	UART_Printf("03. Erase the Block ( %d[block])\n", uBlock);
	eError = NAND_EraseSingleBlock_Basic(g_NandContNum, uBlock);
	if(eError != eNAND_NoError)
	{
		NANDT_PrintErrorType_Sample(eError);
		NAND_Inform[g_NandContNum].uECCtest = 0;
		NAND_Inform[g_NandContNum].uAllBitEccCheck = 0;
		return;
	}

	// 10. Create Invalid data

	for ( uIndex0 =0 ; uIndex0<uError ; uIndex0++)
	{
		switch ( uIndex0+1)
		{
			case 1:
				if ( NAND_Inform[g_NandContNum].u8BitECCErrBytePos[uIndex0] < 512 )
				{
					uTemp =	aBuffer[NAND_EccError.uEccErrByte1] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit1);
					aBuffer[NAND_EccError.uEccErrByte1] = uTemp;				
				}
				else
				{
					uTemp =	aSpareBuffer[(NAND_EccError.uEccErrByte1)-512] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit1);
					aSpareBuffer[(NAND_EccError.uEccErrByte1)-512] = uTemp;				
				}
				break;
			case 2:
				if ( NAND_Inform[g_NandContNum].u8BitECCErrBytePos[uIndex0] < 512 )
				{
					uTemp =	aBuffer[NAND_EccError.uEccErrByte2] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit2);
					aBuffer[NAND_EccError.uEccErrByte2] = uTemp;				
				}
				else
				{
					uTemp =	aSpareBuffer[(NAND_EccError.uEccErrByte2)-512] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit2);
					aSpareBuffer[(NAND_EccError.uEccErrByte2)-512] = uTemp;				
				}
				break;
			case 3:
				if ( NAND_Inform[g_NandContNum].u8BitECCErrBytePos[uIndex0] < 512 )
				{
					uTemp =	aBuffer[NAND_EccError.uEccErrByte3] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit3);
					aBuffer[NAND_EccError.uEccErrByte3] = uTemp;				
				}
				else
				{
					uTemp =	aSpareBuffer[(NAND_EccError.uEccErrByte3)-512] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit3);
					aSpareBuffer[(NAND_EccError.uEccErrByte3)-512] = uTemp;				
				}
				break;
			case 4:
				if ( NAND_Inform[g_NandContNum].u8BitECCErrBytePos[uIndex0] < 512 )
				{
					uTemp =	aBuffer[NAND_EccError.uEccErrByte4] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit4);
					aBuffer[NAND_EccError.uEccErrByte4] = uTemp;				
				}
				else
				{
					uTemp =	aSpareBuffer[(NAND_EccError.uEccErrByte4)-512] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit4);
					aSpareBuffer[(NAND_EccError.uEccErrByte4)-512] = uTemp;				
				}
				break;
			case 5:
				if ( NAND_Inform[g_NandContNum].u8BitECCErrBytePos[uIndex0] < 512 )
				{
					uTemp =	aBuffer[NAND_EccError.uEccErrByte5] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit5);
					aBuffer[NAND_EccError.uEccErrByte5] = uTemp;				
				}
				else
				{
					uTemp =	aSpareBuffer[(NAND_EccError.uEccErrByte5)-512] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit5);
					aSpareBuffer[(NAND_EccError.uEccErrByte5)-512] = uTemp;				
				}
				break;
			case 6:
				if ( NAND_Inform[g_NandContNum].u8BitECCErrBytePos[uIndex0] < 512 )
				{
					uTemp =	aBuffer[NAND_EccError.uEccErrByte6] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit6);
					aBuffer[NAND_EccError.uEccErrByte6] = uTemp;				
				}
				else
				{
					uTemp =	aSpareBuffer[(NAND_EccError.uEccErrByte6)-512] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit6);
					aSpareBuffer[(NAND_EccError.uEccErrByte6)-512] = uTemp;				
				}
				break;
			case 7:
				if ( NAND_Inform[g_NandContNum].u8BitECCErrBytePos[uIndex0] < 512 )
				{
					uTemp =	aBuffer[NAND_EccError.uEccErrByte7] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit7);
					aBuffer[NAND_EccError.uEccErrByte7] = uTemp;				
				}
				else
				{
					uTemp =	aSpareBuffer[(NAND_EccError.uEccErrByte7)-512] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit7);
					aSpareBuffer[(NAND_EccError.uEccErrByte7)-512] = uTemp;				
				}
				break;
			case 8:
				if ( NAND_Inform[g_NandContNum].u8BitECCErrBytePos[uIndex0] < 512 )
				{
					uTemp =	aBuffer[NAND_EccError.uEccErrByte8] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit8);
					aBuffer[NAND_EccError.uEccErrByte8] = uTemp;				
				}
				else
				{
					uTemp =	aSpareBuffer[(NAND_EccError.uEccErrByte8)-512] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit8);
					aSpareBuffer[(NAND_EccError.uEccErrByte8)-512] = uTemp;				
				}
				break;
			default: 
				break;
		}

	}


	// 11. Write Data to NAND Flash
	UART_Printf("04. After invalid data create , write NAND flash\n");
	NAND_Inform[g_NandContNum].uECCtest = 1;
	eError = NAND_WritePage_8Bit(g_NandContNum, uBlock, uPage, aBuffer, aSpareBuffer);
	if(eError != eNAND_NoError)
	{
		NANDT_PrintErrorType_Sample(eError);
		NAND_Inform[g_NandContNum].uECCtest = 0;
	}
	else
	{
#if 0
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
#endif		
		UART_Printf("NAND Invalid Data Write Page : Success\n");
	}

	UART_Printf("\n");

	// 12. Read Data from NAND flash
	UART_Printf("05. Read NAND flash\n");
	NAND_Inform[g_NandContNum].uECCtest = 1;
	eError = NAND_ReadPage_8Bit(g_NandContNum, uBlock, uPage, aBuffer, aSpareBuffer);
	if(eError != eNAND_NoError)
	{
		NANDT_PrintErrorType_Sample(eError);
		NAND_Inform[g_NandContNum].uECCtest = 0;
		NAND_Inform[g_NandContNum].uAllBitEccCheck = 0;
		UART_Printf("Read page Error [%d block  %d page]\n", uBlock, uPage);
		UART_Printf("-------------------------------------------\n");	
#if 0 
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
#endif		
	}
	else
	{
#if 0
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
#endif		
	}
	UART_Printf("\n");

	
	NAND_Inform[g_NandContNum].uECCtest = 0;
	NAND_Inform[g_NandContNum].uAllBitEccCheck = 0;
	
	UART_Printf("SLC Nand 8bit ECC test complete & OK...!!\n");
	UART_Printf("\n");	
}




void NANDT_8BitECC_2048(void)
{
	u32 i,  uBlock, uPage;
	NAND_eERROR eError;
	u8 uOffset;
	u8 *aBuffer, *aSpareBuffer;
	


	UART_Printf("[NANDT_WritePage]\n");
	UART_Printf("Input the Block Number to write[0~%d] : ", NAND_Inform[g_NandContNum].uBlockNum-1);
	uBlock = UART_GetIntNum();
	UART_Printf("Input the Page Number to write[0~%d] : ", NAND_Inform[g_NandContNum].uPageNum-1);
	uPage = UART_GetIntNum();
	UART_Printf("Input the srand() offset data : ");
	uOffset = (u8)UART_GetIntNum();
	UART_Printf("\n");
	
	aBuffer = (u8 *)malloc(NAND_PAGE_MAX);
	if(aBuffer == 0)
	{
		UART_Printf("Memory Allocation Error...\n");
		UART_GetKey();
		return;
	}	
	aSpareBuffer = (u8 *)malloc(NAND_SPARE_MAX);
	if(aSpareBuffer == 0)
	{
		UART_Printf("Memory Allocation Error...\n");
		free(aBuffer);
		UART_GetKey();
		return;
	}

#if 0
	srand(uOffset);
	for(i=0 ; i<NAND_Inform[g_NandContNum].uPageSize ; i++)
	{
		aBuffer[i] = rand()%0xFF;
	}
#else
	for(i=0 ; i<NAND_Inform[g_NandContNum].uPageSize ; i++)
	{
		aBuffer[i] = 0x0;
	}
#endif

	for(i=0 ; i<NAND_Inform[g_NandContNum].uSpareSize ; i++)
		aSpareBuffer[i] = 0x0;	

	NAND_Inform[0].uECCtest = 0;
	eError = NAND_Write2048Page_8Bit(g_NandContNum, uBlock, uPage, aBuffer, aSpareBuffer);

	if(eError != eNAND_NoError)
	{
		NANDT_PrintErrorType(eError);
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

	aBuffer[0] = 0x1;
	aBuffer[3] = 0x0;
	aBuffer[5] = 0x0;
	aBuffer[4] = 0x0;
	aBuffer[7] = 0x0;
	aBuffer[10] = 0x0;
	aBuffer[100] = 0x0;
	aBuffer[13] = 0x0;


	eError = NAND_EraseSingleBlock_Basic(g_NandContNum, uBlock);	

	if(eError != eNAND_NoError)
	{
		NANDT_PrintErrorType(eError);
		UART_GetKey();
	}
	else
	{
		UART_Printf("NAND Erase Block : Success\n\n");	
	}




	NAND_Inform[0].uECCtest = 1;
	eError = NAND_Write2048Page_8Bit(g_NandContNum, uBlock, uPage, aBuffer, aSpareBuffer);

	if(eError != eNAND_NoError)
	{
		NANDT_PrintErrorType(eError);
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


	
	for(i=0 ; i<2048 ; i++)
		aBuffer[i] = 0;
	for(i=0 ; i<64 ; i++)
		aSpareBuffer[i] = 0xFF;

	eError = NAND_Read2048Page_8Bit(g_NandContNum, uBlock, uPage, aBuffer, aSpareBuffer);
	if(eError != eNAND_NoError)
	{
		NANDT_PrintErrorType(eError);
		UART_Printf("Read page Error [%d block  %d page]\n", uBlock, uPage);
		UART_Printf("uMLCECCPageReadSector = %d\n", NAND_Inform[0].uMLCECCPageReadSector );
	#if 1
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
	#endif
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
// Function Name : NANDT_8BitECC_SLC_8Bit
// Function Description : MLC ECC function test
// Input : 	None
// Output : 	None
void NANDT_8BitECC_SLC_8Bit(void)
{
	u32 i, uBlock, uPage,  uErrorType;
	u8 aBuffer[NAND_PAGE_512];
	u8 aSpareBuffer[NAND_SPARE_16];
//	u8 aReadBuffer[NAND_PAGE_512];
	NAND_eERROR eError;
	u32 uTemp;
	u32 uRoop0, uRoop1, uRoop2, uRoop3, uRoop4, uRoop5, uRoop6, uRoop7;
	u32 uIndex0;
	u32 uBit0, uBit1, uBit2, uBit3, uBit4, uBit5, uBit6, uBit7;
	u32 uByte0, uByte1, uByte2, uByte3, uByte4, uByte5, uByte6, uByte7;

	NAND_Inform[g_NandContNum].uECCtest = 0;
	NAND_Inform[g_NandContNum].uECCType = ECC8Bit;
	
	UART_Printf("[NAND_8BitECC_All Bit Testl]\n");
	// 01. Input wanted 8bit ecc test block
	UART_Printf("Input the Block Number to test[0~%d]", NAND_Inform[g_NandContNum].uBlockNum-1);
	uBlock = UART_GetIntNum();
	UART_Printf("Input the Page Number to test[0~%d]", NAND_Inform[g_NandContNum].uPageNum-1);
	uPage = UART_GetIntNum();	

	UART_Printf("--------------------------------------------------------------------------\n");	
	for ( uRoop0 = 0 ; uRoop0 <=4192 ; uRoop0++)
	{
		for ( uRoop1 = uRoop0+1 ; uRoop1<=4193 ; uRoop1++)
		{
			for ( uRoop2=uRoop1+1 ; uRoop2<=4194 ; uRoop2++)
			{
				UART_Printf("Input the Block Number to test[0~%d]", NAND_Inform[g_NandContNum].uBlockNum-1);
				uBlock = UART_GetIntNum();
				UART_Printf("Input the Page Number to test[0~%d]", NAND_Inform[g_NandContNum].uPageNum-1);
				uPage = UART_GetIntNum();	
				for ( uRoop3=uRoop2+1; uRoop3<=4195; uRoop3++)
				{
					for ( uRoop4=uRoop3+1; uRoop4<=4196; uRoop4++)
					{
						for (uRoop5=uRoop4+1; uRoop5<=4197; uRoop5++)
						{
							for(uRoop6=uRoop5+1; uRoop6<=4198; uRoop6++)
							{
								for (uRoop7=uRoop6+1; uRoop7<=4199; uRoop7++)
								{
									uByte0 = (uRoop0/8);
									uTemp = uRoop0-(uByte0*8);
									uBit0 = uTemp;
									
									uByte1 = (uRoop1/8);
									uTemp = uRoop1-(uByte1*8);
									uBit1 = uTemp;
									
									uByte2 = (uRoop2/8);
									uTemp = uRoop2-(uByte2*8);
									uBit2 = uTemp;
									
									uByte3 = (uRoop3/8);
									uTemp = uRoop3-(uByte3*8);
									uBit3 = uTemp;
									
									uByte4 = (uRoop4/8);
									uTemp = uRoop4-(uByte4*8);
									uBit4 = uTemp;
									
									uByte5 = (uRoop5/8);
									uTemp = uRoop5-(uByte5*8);
									uBit5 = uTemp;
									
									uByte6 = (uRoop6/8);
									uTemp = uRoop6-(uByte6*8);
									uBit6 = uTemp;
									
									uByte7 = (uRoop7/8);
									uTemp = uRoop7-(uByte7*8);
									uBit7 = uTemp;

									uErrorType = NAND_ECCERR_8BIT;
									NAND_EccError.uEccErrByte1 = uByte0;
									NAND_EccError.uEccErrBit1 = uBit0;
									NAND_Inform[g_NandContNum].u8BitECCErrBytePos[0]=NAND_EccError.uEccErrByte1;

									NAND_EccError.uEccErrByte2 = uByte1;
									NAND_EccError.uEccErrBit2 = uBit1;
									NAND_Inform[g_NandContNum].u8BitECCErrBytePos[1]=NAND_EccError.uEccErrByte2;
									
									NAND_EccError.uEccErrByte3 = uByte2;
									NAND_EccError.uEccErrBit3 = uBit2;
									NAND_Inform[g_NandContNum].u8BitECCErrBytePos[2]=NAND_EccError.uEccErrByte3;
									
									NAND_EccError.uEccErrByte4 = uByte3;
									NAND_EccError.uEccErrBit4 = uBit3;
									NAND_Inform[g_NandContNum].u8BitECCErrBytePos[3]=NAND_EccError.uEccErrByte4;
									
									NAND_EccError.uEccErrByte5 = uByte4;
									NAND_EccError.uEccErrBit5 = uBit4;
									NAND_Inform[g_NandContNum].u8BitECCErrBytePos[4]=NAND_EccError.uEccErrByte5;
									
									NAND_EccError.uEccErrByte6 = uByte5;
									NAND_EccError.uEccErrBit6 = uBit5;
									NAND_Inform[g_NandContNum].u8BitECCErrBytePos[5]=NAND_EccError.uEccErrByte6;
									
									NAND_EccError.uEccErrByte7 = uByte6;
									NAND_EccError.uEccErrBit7 = uBit6;
									NAND_Inform[g_NandContNum].u8BitECCErrBytePos[6]=NAND_EccError.uEccErrByte7;
									
									NAND_EccError.uEccErrByte8 = uByte7;
									NAND_EccError.uEccErrBit8 = uBit7;
									NAND_Inform[g_NandContNum].u8BitECCErrBytePos[7]=NAND_EccError.uEccErrByte8;

									NAND_Inform[g_NandContNum].uSLCError8bit_Original[0] = uRoop0;
									NAND_Inform[g_NandContNum].uSLCError8bit_Original[1] = uRoop1;
									NAND_Inform[g_NandContNum].uSLCError8bit_Original[2] = uRoop2;
									NAND_Inform[g_NandContNum].uSLCError8bit_Original[3] = uRoop3;
									NAND_Inform[g_NandContNum].uSLCError8bit_Original[4] = uRoop4;
									NAND_Inform[g_NandContNum].uSLCError8bit_Original[5] = uRoop5;
									NAND_Inform[g_NandContNum].uSLCError8bit_Original[6] = uRoop6;
									NAND_Inform[g_NandContNum].uSLCError8bit_Original[7] = uRoop7;

									for(i=0 ; i<NAND_Inform[g_NandContNum].uPageSize ; i++)
									{
										aBuffer[i] = 0x0;
									}
									for(i=0 ; i<NAND_Inform[g_NandContNum].uSpareSize ; i++)
										aSpareBuffer[i] = 0x0;	

									UART_Printf("\n");
									
									// Erase Block
									eError = NAND_EraseSingleBlock_Basic(g_NandContNum, uBlock);
									if(eError != eNAND_NoError)
									{
										NANDT_PrintErrorType_Sample(eError);
										return;
									}

									// Write 1st Block Data. : Vaild Data
									NAND_Inform[g_NandContNum].uECCtest = 0;
									eError = NAND_WritePage_8Bit(g_NandContNum, uBlock, uPage, aBuffer, aSpareBuffer);
									if(eError != eNAND_NoError)
									{
										NANDT_PrintErrorType_Sample(eError);
										return;
									}

									// Read Vaild Data : Invalid Data
									eError = NAND_ReadPage_8Bit(g_NandContNum, uBlock, uPage, aBuffer, aSpareBuffer);
									if(eError != eNAND_NoError)
									{
										NANDT_PrintErrorType_Sample(eError);
										NAND_Inform[g_NandContNum].uECCtest = 0;
										return;
									}
									UART_Printf("\n");
									#if 0 
									// Disaply Test Pattern.
									UART_Printf("-------------------------------------------\n");	
									UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte1, NAND_EccError.uEccErrBit1);
									UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte2, NAND_EccError.uEccErrBit2);
									UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte3, NAND_EccError.uEccErrBit3);
									UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte4, NAND_EccError.uEccErrBit4);
									UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte5, NAND_EccError.uEccErrBit5);	
									UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte6, NAND_EccError.uEccErrBit6);
									UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte7, NAND_EccError.uEccErrBit7);
									UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte8, NAND_EccError.uEccErrBit8);
									UART_Printf("-------------------------------------------\n");	
									#endif

									// Erase Block for test
									eError = NAND_EraseSingleBlock_Basic(g_NandContNum, uBlock);
									if(eError != eNAND_NoError)
									{
										NANDT_PrintErrorType_Sample(eError);
										NAND_Inform[g_NandContNum].uECCtest = 0;
										NAND_Inform[g_NandContNum].uAllBitEccCheck = 0;
										return;
									}
									
									// Create Invalid data
									for ( uIndex0 =0 ; uIndex0<uErrorType; uIndex0++)
									{
										switch ( uIndex0+1)
										{
											case 1:
												if ( NAND_Inform[g_NandContNum].u8BitECCErrBytePos[uIndex0] < 512 )
												{
													uTemp =	aBuffer[NAND_EccError.uEccErrByte1] ;
													uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit1);
													aBuffer[NAND_EccError.uEccErrByte1] = uTemp;				
												}
												else
												{
													uTemp =	aSpareBuffer[(NAND_EccError.uEccErrByte1)-512] ;
													uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit1);
													aSpareBuffer[(NAND_EccError.uEccErrByte1)-512] = uTemp;				
												}
												break;
											case 2:
												if ( NAND_Inform[g_NandContNum].u8BitECCErrBytePos[uIndex0] < 512 )
												{
													uTemp =	aBuffer[NAND_EccError.uEccErrByte2] ;
													uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit2);
													aBuffer[NAND_EccError.uEccErrByte2] = uTemp;				
												}
												else
												{
													uTemp =	aSpareBuffer[(NAND_EccError.uEccErrByte2)-512] ;
													uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit2);
													aSpareBuffer[(NAND_EccError.uEccErrByte2)-512] = uTemp;				
												}
												break;
											case 3:
												if ( NAND_Inform[g_NandContNum].u8BitECCErrBytePos[uIndex0] < 512 )
												{
													uTemp =	aBuffer[NAND_EccError.uEccErrByte3] ;
													uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit3);
													aBuffer[NAND_EccError.uEccErrByte3] = uTemp;				
												}
												else
												{
													uTemp =	aSpareBuffer[(NAND_EccError.uEccErrByte3)-512] ;
													uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit3);
													aSpareBuffer[(NAND_EccError.uEccErrByte3)-512] = uTemp;				
												}
												break;
											case 4:
												if ( NAND_Inform[g_NandContNum].u8BitECCErrBytePos[uIndex0] < 512 )
												{
													uTemp =	aBuffer[NAND_EccError.uEccErrByte4] ;
													uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit4);
													aBuffer[NAND_EccError.uEccErrByte4] = uTemp;				
												}
												else
												{
													uTemp =	aSpareBuffer[(NAND_EccError.uEccErrByte4)-512] ;
													uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit4);
													aSpareBuffer[(NAND_EccError.uEccErrByte4)-512] = uTemp;				
												}
												break;
											case 5:
												if ( NAND_Inform[g_NandContNum].u8BitECCErrBytePos[uIndex0] < 512 )
												{
													uTemp =	aBuffer[NAND_EccError.uEccErrByte5] ;
													uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit5);
													aBuffer[NAND_EccError.uEccErrByte5] = uTemp;				
												}
												else
												{
													uTemp =	aSpareBuffer[(NAND_EccError.uEccErrByte5)-512] ;
													uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit5);
													aSpareBuffer[(NAND_EccError.uEccErrByte5)-512] = uTemp;				
												}
												break;
											case 6:
												if ( NAND_Inform[g_NandContNum].u8BitECCErrBytePos[uIndex0] < 512 )
												{
													uTemp =	aBuffer[NAND_EccError.uEccErrByte6] ;
													uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit6);
													aBuffer[NAND_EccError.uEccErrByte6] = uTemp;				
												}
												else
												{
													uTemp =	aSpareBuffer[(NAND_EccError.uEccErrByte6)-512] ;
													uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit6);
													aSpareBuffer[(NAND_EccError.uEccErrByte6)-512] = uTemp;				
												}
												break;
											case 7:
												if ( NAND_Inform[g_NandContNum].u8BitECCErrBytePos[uIndex0] < 512 )
												{
													uTemp =	aBuffer[NAND_EccError.uEccErrByte7] ;
													uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit7);
													aBuffer[NAND_EccError.uEccErrByte7] = uTemp;				
												}
												else
												{
													uTemp =	aSpareBuffer[(NAND_EccError.uEccErrByte7)-512] ;
													uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit7);
													aSpareBuffer[(NAND_EccError.uEccErrByte7)-512] = uTemp;				
												}
												break;
											case 8:
												if ( NAND_Inform[g_NandContNum].u8BitECCErrBytePos[uIndex0] < 512 )
												{
													uTemp =	aBuffer[NAND_EccError.uEccErrByte8] ;
													uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit8);
													aBuffer[NAND_EccError.uEccErrByte8] = uTemp;				
												}
												else
												{
													uTemp =	aSpareBuffer[(NAND_EccError.uEccErrByte8)-512] ;
													uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit8);
													aSpareBuffer[(NAND_EccError.uEccErrByte8)-512] = uTemp;				
												}
												break;
											default: 
												break;
										}

									}

									// Write Data to NAND Flash
									NAND_Inform[g_NandContNum].uECCtest = 1;
									eError = NAND_WritePage_8Bit(g_NandContNum, uBlock, uPage, aBuffer, aSpareBuffer);
									if(eError != eNAND_NoError)
									{
										NANDT_PrintErrorType_Sample(eError);
										NAND_Inform[g_NandContNum].uECCtest = 0;
									}
									#if 0
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
									#endif
	
//									uTemp=UART_GetIntNum();
//									UART_Printf("Invalid Data REad.......\n");
									
									// Read Data from NAND flash and Check Error Bit
									NAND_Inform[g_NandContNum].uECCtest = 1;
									eError = NAND_ReadPage_8Bit(g_NandContNum, uBlock, uPage, aBuffer, aSpareBuffer);
									if(eError != eNAND_NoError)
									{
										NANDT_PrintErrorType_Sample_8BitALLBit(eError);
										NAND_Inform[g_NandContNum].uECCtest = 0;
										NAND_Inform[g_NandContNum].uAllBitEccCheck = 0;
										UART_Printf("\n\n");
									}
				
								}
								UART_Printf("uRoop0=%4d, uRoop1=%4d, uRoop2=%4d, uRoop3=%4d, uRoop4=%4d, uRoop5=%4d, uRoop6=%4d, uRoop7=%4d, \n", uRoop0, uRoop1, uRoop2, uRoop3, uRoop4, uRoop5, uRoop6, uRoop7);
								UART_Printf("--------------------------------------------------------------------------\n");									
//								uIndex0 = UART_GetIntNum();;
							}
						}
					}
				}
			}
		}
	}

	


	
}


void NANDT_8BitECC_SLC_1bitPattern(u32 uBlock, u32 uPage)
{
	u32 i, uErrorType;
	u8 aBuffer[NAND_PAGE_512];
	u8 aSpareBuffer[NAND_SPARE_16];
//	u8 aReadBuffer[NAND_PAGE_512];
	NAND_eERROR eError;
	u32 uTemp;
	u32 uRoop0;
//	u32 uRoop1, uRoop2, uRoop3, uRoop4, uRoop5, uRoop6, uRoop7;
//	u32 uIndex0, uIndex1;
	u32 uBit0;
// 	u32 uBit1, uBit2, uBit3, uBit4, uBit5, uBit6, uBit7;
	u32 uByte0;
//	u32 uByte1, uByte2, uByte3, uByte4, uByte5, uByte6, uByte7;

	UART_Printf("==================================================\n");
	NAND_Inform[g_NandContNum].u8bitECCPattern = 1; 


	for (uRoop0 = 0 ; uRoop0 <= 4199 ; uRoop0++)
	{
		uByte0 = (uRoop0/8);
		uTemp = uRoop0-(uByte0*8);
		uBit0 = uTemp;

		// 01. Erase blocm to test
		uErrorType = NAND_ECCERR_8BIT;
		NAND_EccError.uEccErrByte1 = uByte0;
		NAND_EccError.uEccErrBit1 = uBit0;
		NAND_Inform[g_NandContNum].u8BitECCErrBytePos[0]=NAND_EccError.uEccErrByte1;
		
		NAND_Inform[g_NandContNum].uSLCError8bit_Original[0] = uRoop0;			
		
		for(i=0 ; i<NAND_Inform[g_NandContNum].uPageSize ; i++)
		{
			aBuffer[i] = 0x0;
		}
		for(i=0 ; i<NAND_Inform[g_NandContNum].uSpareSize ; i++)
			aSpareBuffer[i] = 0x0;	

		UART_Printf("\n");
		
		// Erase Block
		eError = NAND_EraseSingleBlock_Basic(g_NandContNum, uBlock);
		if(eError != eNAND_NoError)
		{
			NANDT_PrintErrorType_Sample(eError);
			return;
		}

		// Write valid data 
		NAND_Inform[g_NandContNum].uECCtest = 0;
		eError = NAND_WritePage_8Bit(g_NandContNum, uBlock, uPage, aBuffer, aSpareBuffer);
		if(eError != eNAND_NoError)
		{
			NANDT_PrintErrorType_Sample(eError);
			return;
		}

		// Read Vaild Data : Invalid Data
		eError = NAND_ReadPage_8Bit(g_NandContNum, uBlock, uPage, aBuffer, aSpareBuffer);
		if(eError != eNAND_NoError)
		{
			NANDT_PrintErrorType_Sample(eError);
			NAND_Inform[g_NandContNum].uECCtest = 0;
			return;
		}

		#if 1 
		// Disaply Test Pattern.
		UART_Printf("-------------------------------------------\n");	
		UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte1, NAND_EccError.uEccErrBit1);
		UART_Printf("-------------------------------------------\n");	
		#endif

		// Erase Block for test
		eError = NAND_EraseSingleBlock_Basic(g_NandContNum, uBlock);
		if(eError != eNAND_NoError)
		{
			NANDT_PrintErrorType_Sample(eError);
			NAND_Inform[g_NandContNum].uECCtest = 0;
			NAND_Inform[g_NandContNum].uAllBitEccCheck = 0;
			return;
		}
		
		// Create Invalid data
		if ( NAND_Inform[g_NandContNum].u8BitECCErrBytePos[0] < 512 )
		{
			uTemp =	aBuffer[NAND_EccError.uEccErrByte1] ;
			uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit1);
			aBuffer[NAND_EccError.uEccErrByte1] = uTemp;				
		}
		else
		{
			uTemp =	aSpareBuffer[(NAND_EccError.uEccErrByte1)-512] ;
			uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit1);
			aSpareBuffer[(NAND_EccError.uEccErrByte1)-512] = uTemp;				
		}

		// Write Data to NAND Flash
		NAND_Inform[g_NandContNum].uECCtest = 1;
		eError = NAND_WritePage_8Bit(g_NandContNum, uBlock, uPage, aBuffer, aSpareBuffer);
		if(eError != eNAND_NoError)
		{
			NANDT_PrintErrorType_Sample(eError);
			NAND_Inform[g_NandContNum].uECCtest = 0;
		}
		#if 0
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
		#endif

		// Read Data from NAND flash and Check Error Bit
		NAND_Inform[g_NandContNum].uECCtest = 1;
		eError = NAND_ReadPage_8Bit(g_NandContNum, uBlock, uPage, aBuffer, aSpareBuffer);
		if(eError != eNAND_NoError)
		{
			NANDT_PrintErrorType_Sample_8BitALLBit(eError);
			NAND_Inform[g_NandContNum].uECCtest = 0;
			NAND_Inform[g_NandContNum].uAllBitEccCheck = 0;
			UART_Printf("\n\n");
		}

	}
}


void NANDT_8BitECC_RandomBitPattern( u32 *uRoop0, u32 *uRoop1, u32 *uRoop2, u32 *uRoop3, u32 *uRoop4, u32 *uRoop5, u32 *uRoop6, u32 *uRoop7 )
{
	u32 uIndex0, uIndex1;
	u32 uTemp0;
	u32 uRandomBit[8];
	u32 uCheckRandom;

	NANDT_InsertSeed();

	uCheckRandom = 1;

	while ( uCheckRandom )
	{

		for ( uIndex0 = 0 ; uIndex0 <8 ; uIndex0++)
		{
			uTemp0 = rand();
			uRandomBit[uIndex0] = uTemp0%4199;
		}

		uCheckRandom = 0 ; 

		for ( uIndex0 = 0 ; uIndex0 < 8 ; uIndex0++)
		{
			for ( uIndex1 = uIndex0+1; uIndex1 < 8 ; uIndex1++)
			{
				if ( uRandomBit[uIndex0] == uRandomBit[uIndex1] )
					uCheckRandom = 1; 
			}
		}
	}

	*uRoop0 = uRandomBit[0];
	*uRoop1 = uRandomBit[1];
	*uRoop2 = uRandomBit[2];
	*uRoop3 = uRandomBit[3];
	*uRoop4 = uRandomBit[4];
	*uRoop5 = uRandomBit[5];
	*uRoop6 = uRandomBit[6];
	*uRoop7 = uRandomBit[7];
}

void NANDT_8BitECC_UnCorrect_RandomBitPattern( u32 *uRoop0, u32 *uRoop1, u32 *uRoop2, u32 *uRoop3, u32 *uRoop4, u32 *uRoop5, u32 *uRoop6, u32 *uRoop7, u32 *uRoop8, u32 *uRoop9 )
{
	u32 uIndex0, uIndex1;
	u32 uTemp0;
	u32 uRandomBit[10];
	u32 uCheckRandom;

	NANDT_InsertSeed();

	uCheckRandom = 1;

	while ( uCheckRandom )
	{

		for ( uIndex0 = 0 ; uIndex0 <10 ; uIndex0++)
		{
			uTemp0 = rand();
			uRandomBit[uIndex0] = uTemp0%4199;
		}

		uCheckRandom = 0 ; 

		for ( uIndex0 = 0 ; uIndex0 < 10 ; uIndex0++)
		{
			for ( uIndex1 = uIndex0+1; uIndex1 < 10 ; uIndex1++)
			{
				if ( uRandomBit[uIndex0] == uRandomBit[uIndex1] )
					uCheckRandom = 1; 
			}
		}
	}

	*uRoop0 = uRandomBit[0];
	*uRoop1 = uRandomBit[1];
	*uRoop2 = uRandomBit[2];
	*uRoop3 = uRandomBit[3];
	*uRoop4 = uRandomBit[4];
	*uRoop5 = uRandomBit[5];
	*uRoop6 = uRandomBit[6];
	*uRoop7 = uRandomBit[7];
	*uRoop8 = uRandomBit[8];
	*uRoop9 = uRandomBit[9];	
}


void NANDT_8BitECC_SLC_AllbitPattern(u32 uBlock, u32 uPage, u32 uBitPattern)
{
	u32 i, uErrorType;
	u8 aBuffer[NAND_PAGE_512];
	u8 aSpareBuffer[NAND_SPARE_16];
//	u8 aReadBuffer[NAND_PAGE_512];
	NAND_eERROR eError;
	u32 uTemp;
	u32 uRoop0, uRoop1, uRoop2, uRoop3, uRoop4, uRoop5, uRoop6, uRoop7;
	u32 uIndex0;
	u32 uBit0, uBit1, uBit2, uBit3, uBit4, uBit5, uBit6, uBit7;
	u32 uByte0, uByte1, uByte2, uByte3, uByte4, uByte5, uByte6, uByte7;

	UART_Printf("==================================================\n");
	NAND_Inform[g_NandContNum].u8bitECCPattern = uBitPattern; 

	// Random Bit Create.
	switch ( NAND_Inform[g_NandContNum].u8bitECCPattern )
	{
		case 1:
			NANDT_8BitECC_RandomBitPattern(&uRoop0, &uRoop1, &uRoop2, &uRoop3, &uRoop4, &uRoop5, &uRoop6, &uRoop7 );
			uByte0 = (uRoop0/8);
			uTemp = uRoop0 - (uByte0*8);
			uBit0 = uTemp;
			break;
		case 2:
			NANDT_8BitECC_RandomBitPattern(&uRoop0, &uRoop1, &uRoop2, &uRoop3, &uRoop4, &uRoop5, &uRoop6, &uRoop7 );
			uByte0 = (uRoop0/8);
			uTemp = uRoop0 - (uByte0*8);
			uBit0 = uTemp;
			uByte1 = (uRoop1/8);
			uTemp = uRoop1 - (uByte1*8);
			uBit1 = uTemp;
			break;
		case 3:
			NANDT_8BitECC_RandomBitPattern(&uRoop0, &uRoop1, &uRoop2, &uRoop3, &uRoop4, &uRoop5, &uRoop6, &uRoop7 );
			uByte0 = (uRoop0/8);
			uTemp = uRoop0 - (uByte0*8);
			uBit0 = uTemp;
			uByte1 = (uRoop1/8);
			uTemp = uRoop1 - (uByte1*8);
			uBit1= uTemp;
			uByte2 = (uRoop2/8);
			uTemp = uRoop2 - (uByte2*8);
			uBit2 = uTemp;
			break;
		case 4:
			NANDT_8BitECC_RandomBitPattern(&uRoop0, &uRoop1, &uRoop2, &uRoop3, &uRoop4, &uRoop5, &uRoop6, &uRoop7 );
			uByte0 = (uRoop0/8);
			uTemp = uRoop0 - (uByte0*8);
			uBit0 = uTemp;
			uByte1 = (uRoop1/8);
			uTemp = uRoop1 - (uByte1*8);
			uBit1= uTemp;
			uByte2 = (uRoop2/8);
			uTemp = uRoop2 - (uByte2*8);
			uBit2 = uTemp;
			uByte3 = (uRoop3/8);
			uTemp = uRoop3 - (uByte3*8);
			uBit3 = uTemp;
			break;
		case 5:
			NANDT_8BitECC_RandomBitPattern(&uRoop0, &uRoop1, &uRoop2, &uRoop3, &uRoop4, &uRoop5, &uRoop6, &uRoop7 );
			uByte0 = (uRoop0/8);
			uTemp = uRoop0 - (uByte0*8);
			uBit0 = uTemp;
			uByte1 = (uRoop1/8);
			uTemp = uRoop1 - (uByte1*8);
			uBit1 = uTemp;
			uByte2 = (uRoop2/8);
			uTemp = uRoop2 - (uByte2*8);
			uBit2 = uTemp;
			uByte3 = (uRoop3/8);
			uTemp = uRoop3 - (uByte3*8);
			uBit3 = uTemp;
			uByte4 = (uRoop4/8);
			uTemp = uRoop4 - (uByte4*8);
			uBit4 = uTemp;
			break;
		case 6:
			NANDT_8BitECC_RandomBitPattern(&uRoop0, &uRoop1, &uRoop2, &uRoop3, &uRoop4, &uRoop5, &uRoop6, &uRoop7 );
			uByte0 = (uRoop0/8);
			uTemp = uRoop0 - (uByte0*8);
			uBit0 = uTemp;
			uByte1 = (uRoop1/8);
			uTemp = uRoop1 - (uByte1*8);
			uBit1 = uTemp;
			uByte2 = (uRoop2/8);
			uTemp = uRoop2 - (uByte2*8);
			uBit2 = uTemp;
			uByte3 = (uRoop3/8);
			uTemp = uRoop3 - (uByte3*8);
			uBit3 = uTemp;
			uByte4 = (uRoop4/8);
			uTemp = uRoop4 - (uByte4*8);
			uBit4 = uTemp;
			uByte5 = (uRoop5/8);
			uTemp = uRoop5 - (uByte5*8);
			uBit5 = uTemp;
			break;
		case 7:
			NANDT_8BitECC_RandomBitPattern(&uRoop0, &uRoop1, &uRoop2, &uRoop3, &uRoop4, &uRoop5, &uRoop6, &uRoop7 );
			uByte0 = (uRoop0/8);
			uTemp = uRoop0 - (uByte0*8);
			uBit0 = uTemp;
			uByte1 = (uRoop1/8);
			uTemp = uRoop1 - (uByte1*8);
			uBit1 = uTemp;
			uByte2 = (uRoop2/8);
			uTemp = uRoop2 - (uByte2*8);
			uBit2 = uTemp;
			uByte3 = (uRoop3/8);
			uTemp = uRoop3 - (uByte3*8);
			uBit3 = uTemp;
			uByte4 = (uRoop4/8);
			uTemp = uRoop4 - (uByte4*8);
			uBit4 = uTemp;
			uByte5 = (uRoop5/8);
			uTemp = uRoop5 - (uByte5*8);
			uBit5 = uTemp;
			uByte6 = (uRoop6/8);
			uTemp = uRoop6 - (uByte6*8);
			uBit6 = uTemp;
			break;
		case 8:
			NANDT_8BitECC_RandomBitPattern(&uRoop0, &uRoop1, &uRoop2, &uRoop3, &uRoop4, &uRoop5, &uRoop6, &uRoop7 );
			uByte0 = (uRoop0/8);
			uTemp = uRoop0 - (uByte0*8);
			uBit0 = uTemp;
			uByte1 = (uRoop1/8);
			uTemp = uRoop1 - (uByte1*8);
			uBit1 = uTemp;
			uByte2 = (uRoop2/8);
			uTemp = uRoop2 - (uByte2*8);
			uBit2 = uTemp;
			uByte3 = (uRoop3/8);
			uTemp = uRoop3 - (uByte3*8);
			uBit3 = uTemp;
			uByte4 = (uRoop4/8);
			uTemp = uRoop4 - (uByte4*8);
			uBit4 = uTemp;
			uByte5 = (uRoop5/8);
			uTemp = uRoop5 - (uByte5*8);
			uBit5 = uTemp;
			uByte6 = (uRoop6/8);
			uTemp = uRoop6 - (uByte6*8);
			uBit6 = uTemp;
			uByte7 = (uRoop7/8);
			uTemp = uRoop7 - (uByte7*8);
			uBit7 = uTemp;
			break;
		case 9:
			NANDT_8BitECC_RandomBitPattern(&uRoop0, &uRoop1, &uRoop2, &uRoop3, &uRoop4, &uRoop5, &uRoop6, &uRoop7 );
			uByte0 = (uRoop0/8);
			uTemp = uRoop0 - (uByte0*8);
			uBit0 = uTemp;
			uByte1 = (uRoop1/8);
			uTemp = uRoop1 - (uByte1*8);
			uBit1 = uTemp;
			uByte2 = (uRoop2/8);
			uTemp = uRoop2 - (uByte2*8);
			uBit2 = uTemp;
			uByte3 = (uRoop3/8);
			uTemp = uRoop3 - (uByte3*8);
			uBit3 = uTemp;
			uByte4 = (uRoop4/8);
			uTemp = uRoop4 - (uByte4*8);
			uBit4 = uTemp;
			uByte5 = (uRoop5/8);
			uTemp = uRoop5 - (uByte5*8);
			uBit5 = uTemp;
			uByte6 = (uRoop6/8);
			uTemp = uRoop6 - (uByte6*8);
			uBit6 = uTemp;
			uByte7 = (uRoop7/8);
			uTemp = uRoop7 - (uByte7*8);
			uBit7 = uTemp;
			break;
		default:
			break;
	}

	if ( NAND_Inform[0].uRandomCount > 999998)
	{
		UART_Printf("Random Counter = %d", NAND_Inform[g_NandContNum].uRandomCount);
		UART_Printf("Roop0= %d, Roop1= %d, Roop2= %d, Roop3= %d\n", uRoop0, uRoop1, uRoop2, uRoop3);
		UART_Printf("Roop4= %d, Roop5= %d, Roop6= %d, Roop7= %d\n", uRoop4, uRoop5, uRoop6, uRoop7);
	}

	uErrorType = NAND_ECCERR_8BIT;

	#if 1 
	// 
	switch ( NAND_Inform[g_NandContNum].u8bitECCPattern)
	{
		case 1:
			NAND_EccError.uEccErrByte1 = uByte0;
			NAND_EccError.uEccErrBit1 = uBit0;
			NAND_Inform[g_NandContNum].u8BitECCErrBytePos[0]=NAND_EccError.uEccErrByte1;
			break;
		case 2:
			NAND_EccError.uEccErrByte1 = uByte0;
			NAND_EccError.uEccErrBit1 = uBit0;
			NAND_Inform[g_NandContNum].u8BitECCErrBytePos[0]=NAND_EccError.uEccErrByte1;
			NAND_EccError.uEccErrByte2 = uByte1;
			NAND_EccError.uEccErrBit2 = uBit1;
			NAND_Inform[g_NandContNum].u8BitECCErrBytePos[1]=NAND_EccError.uEccErrByte2;
			break;
		case 3:
			NAND_EccError.uEccErrByte1 = uByte0;
			NAND_EccError.uEccErrBit1 = uBit0;
			NAND_Inform[g_NandContNum].u8BitECCErrBytePos[0]=NAND_EccError.uEccErrByte1;
			NAND_EccError.uEccErrByte2 = uByte1;
			NAND_EccError.uEccErrBit2 = uBit1;
			NAND_Inform[g_NandContNum].u8BitECCErrBytePos[1]=NAND_EccError.uEccErrByte2;
			NAND_EccError.uEccErrByte3 = uByte2;
			NAND_EccError.uEccErrBit3 = uBit2;
			NAND_Inform[g_NandContNum].u8BitECCErrBytePos[2]=NAND_EccError.uEccErrByte3;
			break;
		case 4:
			NAND_EccError.uEccErrByte1 = uByte0;
			NAND_EccError.uEccErrBit1 = uBit0;
			NAND_Inform[g_NandContNum].u8BitECCErrBytePos[0]=NAND_EccError.uEccErrByte1;
			NAND_EccError.uEccErrByte2 = uByte1;
			NAND_EccError.uEccErrBit2 = uBit1;
			NAND_Inform[g_NandContNum].u8BitECCErrBytePos[1]=NAND_EccError.uEccErrByte2;
			NAND_EccError.uEccErrByte3 = uByte2;
			NAND_EccError.uEccErrBit3 = uBit2;
			NAND_Inform[g_NandContNum].u8BitECCErrBytePos[2]=NAND_EccError.uEccErrByte3;
			NAND_EccError.uEccErrByte4 = uByte3;
			NAND_EccError.uEccErrBit4 = uBit3;
			NAND_Inform[g_NandContNum].u8BitECCErrBytePos[3]=NAND_EccError.uEccErrByte4;
			break;
		case 5:
			NAND_EccError.uEccErrByte1 = uByte0;
			NAND_EccError.uEccErrBit1 = uBit0;
			NAND_Inform[g_NandContNum].u8BitECCErrBytePos[0]=NAND_EccError.uEccErrByte1;
			NAND_EccError.uEccErrByte2 = uByte1;
			NAND_EccError.uEccErrBit2 = uBit1;
			NAND_Inform[g_NandContNum].u8BitECCErrBytePos[1]=NAND_EccError.uEccErrByte2;
			NAND_EccError.uEccErrByte3 = uByte2;
			NAND_EccError.uEccErrBit3 = uBit2;
			NAND_Inform[g_NandContNum].u8BitECCErrBytePos[2]=NAND_EccError.uEccErrByte3;
			NAND_EccError.uEccErrByte4 = uByte3;
			NAND_EccError.uEccErrBit4 = uBit3;
			NAND_Inform[g_NandContNum].u8BitECCErrBytePos[3]=NAND_EccError.uEccErrByte4;
			NAND_EccError.uEccErrByte5 = uByte4;
			NAND_EccError.uEccErrBit5 = uBit4;
			NAND_Inform[g_NandContNum].u8BitECCErrBytePos[4]=NAND_EccError.uEccErrByte5;
			break;
		case 6:
			NAND_EccError.uEccErrByte1 = uByte0;
			NAND_EccError.uEccErrBit1 = uBit0;
			NAND_Inform[g_NandContNum].u8BitECCErrBytePos[0]=NAND_EccError.uEccErrByte1;
			NAND_EccError.uEccErrByte2 = uByte1;
			NAND_EccError.uEccErrBit2 = uBit1;
			NAND_Inform[g_NandContNum].u8BitECCErrBytePos[1]=NAND_EccError.uEccErrByte2;
			NAND_EccError.uEccErrByte3 = uByte2;
			NAND_EccError.uEccErrBit3 = uBit2;
			NAND_Inform[g_NandContNum].u8BitECCErrBytePos[2]=NAND_EccError.uEccErrByte3;
			NAND_EccError.uEccErrByte4 = uByte3;
			NAND_EccError.uEccErrBit4 = uBit3;
			NAND_Inform[g_NandContNum].u8BitECCErrBytePos[3]=NAND_EccError.uEccErrByte4;
			NAND_EccError.uEccErrByte5 = uByte4;
			NAND_EccError.uEccErrBit5 = uBit4;
			NAND_Inform[g_NandContNum].u8BitECCErrBytePos[4]=NAND_EccError.uEccErrByte5;
			NAND_EccError.uEccErrByte6 = uByte5;
			NAND_EccError.uEccErrBit6 = uBit5;
			NAND_Inform[g_NandContNum].u8BitECCErrBytePos[5]=NAND_EccError.uEccErrByte6;
			break;
		case 7:
			NAND_EccError.uEccErrByte1 = uByte0;
			NAND_EccError.uEccErrBit1 = uBit0;
			NAND_Inform[g_NandContNum].u8BitECCErrBytePos[0]=NAND_EccError.uEccErrByte1;
			NAND_EccError.uEccErrByte2 = uByte1;
			NAND_EccError.uEccErrBit2 = uBit1;
			NAND_Inform[g_NandContNum].u8BitECCErrBytePos[1]=NAND_EccError.uEccErrByte2;
			NAND_EccError.uEccErrByte3 = uByte2;
			NAND_EccError.uEccErrBit3 = uBit2;
			NAND_Inform[g_NandContNum].u8BitECCErrBytePos[2]=NAND_EccError.uEccErrByte3;
			NAND_EccError.uEccErrByte4 = uByte3;
			NAND_EccError.uEccErrBit4 = uBit3;
			NAND_Inform[g_NandContNum].u8BitECCErrBytePos[3]=NAND_EccError.uEccErrByte4;
			NAND_EccError.uEccErrByte5 = uByte4;
			NAND_EccError.uEccErrBit5 = uBit4;
			NAND_Inform[g_NandContNum].u8BitECCErrBytePos[4]=NAND_EccError.uEccErrByte5;
			NAND_EccError.uEccErrByte6 = uByte5;
			NAND_EccError.uEccErrBit6 = uBit5;
			NAND_Inform[g_NandContNum].u8BitECCErrBytePos[5]=NAND_EccError.uEccErrByte6;
			NAND_EccError.uEccErrByte7 = uByte6;
			NAND_EccError.uEccErrBit7 = uBit6;
			NAND_Inform[g_NandContNum].u8BitECCErrBytePos[6]=NAND_EccError.uEccErrByte7;
			break;
		case 8:
			NAND_EccError.uEccErrByte1 = uByte0;
			NAND_EccError.uEccErrBit1 = uBit0;
			NAND_Inform[g_NandContNum].u8BitECCErrBytePos[0]=NAND_EccError.uEccErrByte1;
			NAND_EccError.uEccErrByte2 = uByte1;
			NAND_EccError.uEccErrBit2 = uBit1;
			NAND_Inform[g_NandContNum].u8BitECCErrBytePos[1]=NAND_EccError.uEccErrByte2;
			NAND_EccError.uEccErrByte3 = uByte2;
			NAND_EccError.uEccErrBit3 = uBit2;
			NAND_Inform[g_NandContNum].u8BitECCErrBytePos[2]=NAND_EccError.uEccErrByte3;
			NAND_EccError.uEccErrByte4 = uByte3;
			NAND_EccError.uEccErrBit4 = uBit3;
			NAND_Inform[g_NandContNum].u8BitECCErrBytePos[3]=NAND_EccError.uEccErrByte4;
			NAND_EccError.uEccErrByte5 = uByte4;
			NAND_EccError.uEccErrBit5 = uBit4;
			NAND_Inform[g_NandContNum].u8BitECCErrBytePos[4]=NAND_EccError.uEccErrByte5;
			NAND_EccError.uEccErrByte6 = uByte5;
			NAND_EccError.uEccErrBit6 = uBit5;
			NAND_Inform[g_NandContNum].u8BitECCErrBytePos[5]=NAND_EccError.uEccErrByte6;
			NAND_EccError.uEccErrByte7 = uByte6;
			NAND_EccError.uEccErrBit7 = uBit6;
			NAND_Inform[g_NandContNum].u8BitECCErrBytePos[6]=NAND_EccError.uEccErrByte7;
			NAND_EccError.uEccErrByte8 = uByte7;
			NAND_EccError.uEccErrBit8 = uBit7;
			NAND_Inform[g_NandContNum].u8BitECCErrBytePos[7]=NAND_EccError.uEccErrByte8;
			break;
		default :
			break;
	}

	NAND_Inform[g_NandContNum].uSLCError8bit_Original[0] = uRoop0;
	NAND_Inform[g_NandContNum].uSLCError8bit_Original[1] = uRoop1;
	NAND_Inform[g_NandContNum].uSLCError8bit_Original[2] = uRoop2;
	NAND_Inform[g_NandContNum].uSLCError8bit_Original[3] = uRoop3;
	NAND_Inform[g_NandContNum].uSLCError8bit_Original[4] = uRoop4;
	NAND_Inform[g_NandContNum].uSLCError8bit_Original[5] = uRoop5;
	NAND_Inform[g_NandContNum].uSLCError8bit_Original[6] = uRoop6;
	NAND_Inform[g_NandContNum].uSLCError8bit_Original[7] = uRoop7;

	for(i=0 ; i<NAND_Inform[g_NandContNum].uPageSize ; i++)
	{
		aBuffer[i] = 0x0;
	}
	for(i=0 ; i<NAND_Inform[g_NandContNum].uSpareSize ; i++)
		aSpareBuffer[i] = 0x0;	

	UART_Printf("\n");

	
	// Erase Block
	eError = NAND_EraseSingleBlock_Basic(g_NandContNum, uBlock);
	if(eError != eNAND_NoError)
	{
		NANDT_PrintErrorType_Sample(eError);
		return;
	}

	// Write valid data 
	NAND_Inform[g_NandContNum].uECCtest = 0;
	eError = NAND_WritePage_8Bit(g_NandContNum, uBlock, uPage, aBuffer, aSpareBuffer);
	if(eError != eNAND_NoError)
	{
		NANDT_PrintErrorType_Sample(eError);
		return;
	}

	// Read Vaild Data : Invalid Data
	eError = NAND_ReadPage_8Bit(g_NandContNum, uBlock, uPage, aBuffer, aSpareBuffer);
	if(eError != eNAND_NoError)
	{
		NANDT_PrintErrorType_Sample(eError);
		NAND_Inform[g_NandContNum].uECCtest = 0;
		return;
	}

	#if 1 
	// Disaply Test Pattern.
	switch (NAND_Inform[g_NandContNum].u8bitECCPattern )
	{
		case 1:
			UART_Printf("-------------------------------------------\n");	
			UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte1, NAND_EccError.uEccErrBit1);
			UART_Printf("-------------------------------------------\n");	
			break;
		case 2:
			UART_Printf("-------------------------------------------\n");	
			UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte1, NAND_EccError.uEccErrBit1);
			UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte2, NAND_EccError.uEccErrBit2);
			UART_Printf("-------------------------------------------\n");				
			break;
		case 3:
			UART_Printf("-------------------------------------------\n");	
			UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte1, NAND_EccError.uEccErrBit1);
			UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte2, NAND_EccError.uEccErrBit2);
			UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte3, NAND_EccError.uEccErrBit3);
			UART_Printf("-------------------------------------------\n");				
			break;
		case 4:
			UART_Printf("-------------------------------------------\n");	
			UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte1, NAND_EccError.uEccErrBit1);
			UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte2, NAND_EccError.uEccErrBit2);
			UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte3, NAND_EccError.uEccErrBit3);
			UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte4, NAND_EccError.uEccErrBit4);
			UART_Printf("-------------------------------------------\n");				
			break;
		case 5:
			UART_Printf("-------------------------------------------\n");	
			UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte1, NAND_EccError.uEccErrBit1);
			UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte2, NAND_EccError.uEccErrBit2);
			UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte3, NAND_EccError.uEccErrBit3);
			UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte4, NAND_EccError.uEccErrBit4);
			UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte5, NAND_EccError.uEccErrBit5);	
			UART_Printf("-------------------------------------------\n");				
			break;
		case 6:
			UART_Printf("-------------------------------------------\n");	
			UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte1, NAND_EccError.uEccErrBit1);
			UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte2, NAND_EccError.uEccErrBit2);
			UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte3, NAND_EccError.uEccErrBit3);
			UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte4, NAND_EccError.uEccErrBit4);
			UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte5, NAND_EccError.uEccErrBit5);	
			UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte6, NAND_EccError.uEccErrBit6);
			UART_Printf("-------------------------------------------\n");				
			break;
		case 7:
			UART_Printf("-------------------------------------------\n");	
			UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte1, NAND_EccError.uEccErrBit1);
			UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte2, NAND_EccError.uEccErrBit2);
			UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte3, NAND_EccError.uEccErrBit3);
			UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte4, NAND_EccError.uEccErrBit4);
			UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte5, NAND_EccError.uEccErrBit5);	
			UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte6, NAND_EccError.uEccErrBit6);
			UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte7, NAND_EccError.uEccErrBit7);
			UART_Printf("-------------------------------------------\n");				
			break;
		case 8:
			UART_Printf("-------------------------------------------\n");	
			UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte1, NAND_EccError.uEccErrBit1);
			UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte2, NAND_EccError.uEccErrBit2);
			UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte3, NAND_EccError.uEccErrBit3);
			UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte4, NAND_EccError.uEccErrBit4);
			UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte5, NAND_EccError.uEccErrBit5);	
			UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte6, NAND_EccError.uEccErrBit6);
			UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte7, NAND_EccError.uEccErrBit7);
			UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte8, NAND_EccError.uEccErrBit8);
			UART_Printf("-------------------------------------------\n");				
			break;
		default:
			break;
	}
	#endif

	// Erase Block for test
	eError = NAND_EraseSingleBlock_Basic(g_NandContNum, uBlock);
	if(eError != eNAND_NoError)
	{
		NANDT_PrintErrorType_Sample(eError);
		NAND_Inform[g_NandContNum].uECCtest = 0;
		NAND_Inform[g_NandContNum].uAllBitEccCheck = 0;
		return;
	}
	
	// Create Invalid data
	for ( uIndex0 =0 ; uIndex0<uBitPattern ; uIndex0++)
	{
		switch ( uIndex0+1)
		{
			case 1:
				if ( NAND_Inform[g_NandContNum].u8BitECCErrBytePos[uIndex0] < 512 )
				{
					uTemp =	aBuffer[NAND_EccError.uEccErrByte1] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit1);
					aBuffer[NAND_EccError.uEccErrByte1] = uTemp;				
				}
				else
				{
					uTemp =	aSpareBuffer[(NAND_EccError.uEccErrByte1)-512] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit1);
					aSpareBuffer[(NAND_EccError.uEccErrByte1)-512] = uTemp;				
				}
				break;
			case 2:
				if ( NAND_Inform[g_NandContNum].u8BitECCErrBytePos[uIndex0] < 512 )
				{
					uTemp =	aBuffer[NAND_EccError.uEccErrByte2] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit2);
					aBuffer[NAND_EccError.uEccErrByte2] = uTemp;				
				}
				else
				{
					uTemp =	aSpareBuffer[(NAND_EccError.uEccErrByte2)-512] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit2);
					aSpareBuffer[(NAND_EccError.uEccErrByte2)-512] = uTemp;				
				}
				break;
			case 3:
				if ( NAND_Inform[g_NandContNum].u8BitECCErrBytePos[uIndex0] < 512 )
				{
					uTemp =	aBuffer[NAND_EccError.uEccErrByte3] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit3);
					aBuffer[NAND_EccError.uEccErrByte3] = uTemp;				
				}
				else
				{
					uTemp =	aSpareBuffer[(NAND_EccError.uEccErrByte3)-512] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit3);
					aSpareBuffer[(NAND_EccError.uEccErrByte3)-512] = uTemp;				
				}
				break;
			case 4:
				if ( NAND_Inform[g_NandContNum].u8BitECCErrBytePos[uIndex0] < 512 )
				{
					uTemp =	aBuffer[NAND_EccError.uEccErrByte4] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit4);
					aBuffer[NAND_EccError.uEccErrByte4] = uTemp;				
				}
				else
				{
					uTemp =	aSpareBuffer[(NAND_EccError.uEccErrByte4)-512] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit4);
					aSpareBuffer[(NAND_EccError.uEccErrByte4)-512] = uTemp;				
				}
				break;
			case 5:
				if ( NAND_Inform[g_NandContNum].u8BitECCErrBytePos[uIndex0] < 512 )
				{
					uTemp =	aBuffer[NAND_EccError.uEccErrByte5] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit5);
					aBuffer[NAND_EccError.uEccErrByte5] = uTemp;				
				}
				else
				{
					uTemp =	aSpareBuffer[(NAND_EccError.uEccErrByte5)-512] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit5);
					aSpareBuffer[(NAND_EccError.uEccErrByte5)-512] = uTemp;				
				}
				break;
			case 6:
				if ( NAND_Inform[g_NandContNum].u8BitECCErrBytePos[uIndex0] < 512 )
				{
					uTemp =	aBuffer[NAND_EccError.uEccErrByte6] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit6);
					aBuffer[NAND_EccError.uEccErrByte6] = uTemp;				
				}
				else
				{
					uTemp =	aSpareBuffer[(NAND_EccError.uEccErrByte6)-512] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit6);
					aSpareBuffer[(NAND_EccError.uEccErrByte6)-512] = uTemp;				
				}
				break;
			case 7:
				if ( NAND_Inform[g_NandContNum].u8BitECCErrBytePos[uIndex0] < 512 )
				{
					uTemp =	aBuffer[NAND_EccError.uEccErrByte7] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit7);
					aBuffer[NAND_EccError.uEccErrByte7] = uTemp;				
				}
				else
				{
					uTemp =	aSpareBuffer[(NAND_EccError.uEccErrByte7)-512] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit7);
					aSpareBuffer[(NAND_EccError.uEccErrByte7)-512] = uTemp;				
				}
				break;
			case 8:
				if ( NAND_Inform[g_NandContNum].u8BitECCErrBytePos[uIndex0] < 512 )
				{
					uTemp =	aBuffer[NAND_EccError.uEccErrByte8] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit8);
					aBuffer[NAND_EccError.uEccErrByte8] = uTemp;				
				}
				else
				{
					uTemp =	aSpareBuffer[(NAND_EccError.uEccErrByte8)-512] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit8);
					aSpareBuffer[(NAND_EccError.uEccErrByte8)-512] = uTemp;				
				}
				break;
			default: 
				break;
		}

	}


	// Write Data to NAND Flash
	NAND_Inform[g_NandContNum].uECCtest = 1;
	eError = NAND_WritePage_8Bit(g_NandContNum, uBlock, uPage, aBuffer, aSpareBuffer);
	if(eError != eNAND_NoError)
	{
		NANDT_PrintErrorType_Sample(eError);
		NAND_Inform[g_NandContNum].uECCtest = 0;
	}
	#if 0
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
	#endif

	// Read Data from NAND flash and Check Error Bit
	NAND_Inform[g_NandContNum].uECCtest = 1;
	eError = NAND_ReadPage_8Bit(g_NandContNum, uBlock, uPage, aBuffer, aSpareBuffer);
	if(eError != eNAND_NoError)
	{
		NANDT_PrintErrorType_Sample_8BitALLBit(eError);
		NAND_Inform[g_NandContNum].uECCtest = 0;
		NAND_Inform[g_NandContNum].uAllBitEccCheck = 0;
		UART_Printf("\n\n");
	}
#endif

}

void NANDT_8BitECC_SLC_UnCorrect_Pattern(u32 uBlock, u32 uPage, u32 uBitPattern)
{
	u32 i, uErrorType;
	u8 aBuffer[NAND_PAGE_512];
	u8 aSpareBuffer[NAND_SPARE_16];
//	u8 aReadBuffer[NAND_PAGE_512];
	NAND_eERROR eError;
	u32 uTemp;
	u32 uRoop0, uRoop1, uRoop2, uRoop3, uRoop4, uRoop5, uRoop6, uRoop7, uRoop8, uRoop9;
	u32 uIndex0;
	u32 uBit0, uBit1, uBit2, uBit3, uBit4, uBit5, uBit6, uBit7, uBit8, uBit9;
	u32 uByte0, uByte1, uByte2, uByte3, uByte4, uByte5, uByte6, uByte7, uByte8, uByte9;

	UART_Printf("==================================================\n");
	NAND_Inform[g_NandContNum].u8bitECCPattern = uBitPattern; 
	NAND_Inform[g_NandContNum].uUncorrectableTest = 0 ; 
	// Random Bit Create.
	switch ( NAND_Inform[g_NandContNum].u8bitECCPattern )
	{
		case 9:
			NANDT_8BitECC_UnCorrect_RandomBitPattern(&uRoop0, &uRoop1, &uRoop2, &uRoop3, &uRoop4, &uRoop5, &uRoop6, &uRoop7, &uRoop8, &uRoop9 );
			uByte0 = (uRoop0/8);
			uTemp = uRoop0 - (uByte0*8);
			uBit0 = uTemp;
			uByte1 = (uRoop1/8);
			uTemp = uRoop1 - (uByte1*8);
			uBit1 = uTemp;
			uByte2 = (uRoop2/8);
			uTemp = uRoop2 - (uByte2*8);
			uBit2 = uTemp;
			uByte3 = (uRoop3/8);
			uTemp = uRoop3 - (uByte3*8);
			uBit3 = uTemp;
			uByte4 = (uRoop4/8);
			uTemp = uRoop4 - (uByte4*8);
			uBit4 = uTemp;
			uByte5 = (uRoop5/8);
			uTemp = uRoop5 - (uByte5*8);
			uBit5 = uTemp;
			uByte6 = (uRoop6/8);
			uTemp = uRoop6 - (uByte6*8);
			uBit6 = uTemp;
			uByte7 = (uRoop7/8);
			uTemp = uRoop7 - (uByte7*8);
			uBit7 = uTemp;
			uByte8 = (uRoop8/8);
			uTemp = uRoop8 - (uByte8*8);
			uBit8 = uTemp;
			break;
		case 10:
			NANDT_8BitECC_UnCorrect_RandomBitPattern(&uRoop0, &uRoop1, &uRoop2, &uRoop3, &uRoop4, &uRoop5, &uRoop6, &uRoop7, &uRoop8, &uRoop9 );
			uByte0 = (uRoop0/8);
			uTemp = uRoop0 - (uByte0*8);
			uBit0 = uTemp;
			uByte1 = (uRoop1/8);
			uTemp = uRoop1 - (uByte1*8);
			uBit1 = uTemp;
			uByte2 = (uRoop2/8);
			uTemp = uRoop2 - (uByte2*8);
			uBit2 = uTemp;
			uByte3 = (uRoop3/8);
			uTemp = uRoop3 - (uByte3*8);
			uBit3 = uTemp;
			uByte4 = (uRoop4/8);
			uTemp = uRoop4 - (uByte4*8);
			uBit4 = uTemp;
			uByte5 = (uRoop5/8);
			uTemp = uRoop5 - (uByte5*8);
			uBit5 = uTemp;
			uByte6 = (uRoop6/8);
			uTemp = uRoop6 - (uByte6*8);
			uBit6 = uTemp;
			uByte7 = (uRoop7/8);
			uTemp = uRoop7 - (uByte7*8);
			uBit7 = uTemp;
			uByte8 = (uRoop8/8);
			uTemp = uRoop8 - (uByte8*8);
			uBit8 = uTemp;
			uByte9 = (uRoop9/8);
			uTemp = uRoop9 - (uByte9*8);
			uBit9 = uTemp;
			break;
		default:
			break;
	}

	uErrorType = NAND_ECCERR_UNCORRECT;
#if 1 
	// 
	switch ( NAND_Inform[g_NandContNum].u8bitECCPattern)
	{
		case 9:
			NAND_EccError.uEccErrByte1 = uByte0;
			NAND_EccError.uEccErrBit1 = uBit0;
			NAND_Inform[g_NandContNum].u8BitECCErrBytePos[0]=NAND_EccError.uEccErrByte1;
			NAND_EccError.uEccErrByte2 = uByte1;
			NAND_EccError.uEccErrBit2 = uBit1;
			NAND_Inform[g_NandContNum].u8BitECCErrBytePos[1]=NAND_EccError.uEccErrByte2;
			NAND_EccError.uEccErrByte3 = uByte2;
			NAND_EccError.uEccErrBit3 = uBit2;
			NAND_Inform[g_NandContNum].u8BitECCErrBytePos[2]=NAND_EccError.uEccErrByte3;
			NAND_EccError.uEccErrByte4 = uByte3;
			NAND_EccError.uEccErrBit4 = uBit3;
			NAND_Inform[g_NandContNum].u8BitECCErrBytePos[3]=NAND_EccError.uEccErrByte4;
			NAND_EccError.uEccErrByte5 = uByte4;
			NAND_EccError.uEccErrBit5 = uBit4;
			NAND_Inform[g_NandContNum].u8BitECCErrBytePos[4]=NAND_EccError.uEccErrByte5;
			NAND_EccError.uEccErrByte6 = uByte5;
			NAND_EccError.uEccErrBit6 = uBit5;
			NAND_Inform[g_NandContNum].u8BitECCErrBytePos[5]=NAND_EccError.uEccErrByte6;
			NAND_EccError.uEccErrByte7 = uByte6;
			NAND_EccError.uEccErrBit7 = uBit6;
			NAND_Inform[g_NandContNum].u8BitECCErrBytePos[6]=NAND_EccError.uEccErrByte7;
			NAND_EccError.uEccErrByte8 = uByte7;
			NAND_EccError.uEccErrBit8 = uBit7;
			NAND_Inform[g_NandContNum].u8BitECCErrBytePos[7]=NAND_EccError.uEccErrByte8;
			NAND_EccError.uEccErrByte9 = uByte8;
			NAND_EccError.uEccErrBit9 = uBit8;
			NAND_Inform[g_NandContNum].u8BitECCErrBytePos[8]=NAND_EccError.uEccErrByte9;
			break;
		case 10:
			NAND_EccError.uEccErrByte1 = uByte0;
			NAND_EccError.uEccErrBit1 = uBit0;
			NAND_Inform[g_NandContNum].u8BitECCErrBytePos[0]=NAND_EccError.uEccErrByte1;
			NAND_EccError.uEccErrByte2 = uByte1;
			NAND_EccError.uEccErrBit2 = uBit1;
			NAND_Inform[g_NandContNum].u8BitECCErrBytePos[1]=NAND_EccError.uEccErrByte2;
			NAND_EccError.uEccErrByte3 = uByte2;
			NAND_EccError.uEccErrBit3 = uBit2;
			NAND_Inform[g_NandContNum].u8BitECCErrBytePos[2]=NAND_EccError.uEccErrByte3;
			NAND_EccError.uEccErrByte4 = uByte3;
			NAND_EccError.uEccErrBit4 = uBit3;
			NAND_Inform[g_NandContNum].u8BitECCErrBytePos[3]=NAND_EccError.uEccErrByte4;
			NAND_EccError.uEccErrByte5 = uByte4;
			NAND_EccError.uEccErrBit5 = uBit4;
			NAND_Inform[g_NandContNum].u8BitECCErrBytePos[4]=NAND_EccError.uEccErrByte5;
			NAND_EccError.uEccErrByte6 = uByte5;
			NAND_EccError.uEccErrBit6 = uBit5;
			NAND_Inform[g_NandContNum].u8BitECCErrBytePos[5]=NAND_EccError.uEccErrByte6;
			NAND_EccError.uEccErrByte7 = uByte6;
			NAND_EccError.uEccErrBit7 = uBit6;
			NAND_Inform[g_NandContNum].u8BitECCErrBytePos[6]=NAND_EccError.uEccErrByte7;
			NAND_EccError.uEccErrByte8 = uByte7;
			NAND_EccError.uEccErrBit8 = uBit7;
			NAND_Inform[g_NandContNum].u8BitECCErrBytePos[7]=NAND_EccError.uEccErrByte8;
			NAND_EccError.uEccErrByte9 = uByte8;
			NAND_EccError.uEccErrBit9 = uBit8;
			NAND_Inform[g_NandContNum].u8BitECCErrBytePos[8]=NAND_EccError.uEccErrByte9;
			NAND_EccError.uEccErrByte10 = uByte9;
			NAND_EccError.uEccErrBit10 = uBit9;
			NAND_Inform[g_NandContNum].u8BitECCErrBytePos[9]=NAND_EccError.uEccErrByte10;
			break;
		default :
			break;
	}

	NAND_Inform[g_NandContNum].uSLCError8bit_Original[0] = uRoop0;
	NAND_Inform[g_NandContNum].uSLCError8bit_Original[1] = uRoop1;
	NAND_Inform[g_NandContNum].uSLCError8bit_Original[2] = uRoop2;
	NAND_Inform[g_NandContNum].uSLCError8bit_Original[3] = uRoop3;
	NAND_Inform[g_NandContNum].uSLCError8bit_Original[4] = uRoop4;
	NAND_Inform[g_NandContNum].uSLCError8bit_Original[5] = uRoop5;
	NAND_Inform[g_NandContNum].uSLCError8bit_Original[6] = uRoop6;
	NAND_Inform[g_NandContNum].uSLCError8bit_Original[7] = uRoop7;
	NAND_Inform[g_NandContNum].uSLCError8bit_Original[8] = uRoop8;	
	NAND_Inform[g_NandContNum].uSLCError8bit_Original[9] = uRoop9;
	
	for(i=0 ; i<NAND_Inform[g_NandContNum].uPageSize ; i++)
	{
		aBuffer[i] = 0x0;
	}
	for(i=0 ; i<NAND_Inform[g_NandContNum].uSpareSize ; i++)
		aSpareBuffer[i] = 0x0;	

	UART_Printf("\n");

	
	// Erase Block
	eError = NAND_EraseSingleBlock_Basic(g_NandContNum, uBlock);
	if(eError != eNAND_NoError)
	{
		NANDT_PrintErrorType_Sample(eError);
		return;
	}

	// Write valid data 
	NAND_Inform[g_NandContNum].uECCtest = 0;
	eError = NAND_WritePage_8Bit(g_NandContNum, uBlock, uPage, aBuffer, aSpareBuffer);
	if(eError != eNAND_NoError)
	{
		NANDT_PrintErrorType_Sample(eError);
		return;
	}

	// Read Vaild Data : Invalid Data
	eError = NAND_ReadPage_8Bit(g_NandContNum, uBlock, uPage, aBuffer, aSpareBuffer);
	if(eError != eNAND_NoError)
	{
		NANDT_PrintErrorType_Sample(eError);
		NAND_Inform[g_NandContNum].uECCtest = 0;
		return;
	}

	#if 1 
	// Disaply Test Pattern.
	switch (NAND_Inform[g_NandContNum].u8bitECCPattern )
	{
		case 9:
			UART_Printf("-------------------------------------------\n");	
			UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte1, NAND_EccError.uEccErrBit1);
			UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte2, NAND_EccError.uEccErrBit2);
			UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte3, NAND_EccError.uEccErrBit3);
			UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte4, NAND_EccError.uEccErrBit4);
			UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte5, NAND_EccError.uEccErrBit5);	
			UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte6, NAND_EccError.uEccErrBit6);
			UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte7, NAND_EccError.uEccErrBit7);
			UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte8, NAND_EccError.uEccErrBit8);
			UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte9, NAND_EccError.uEccErrBit9);			
			UART_Printf("-------------------------------------------\n");				
			break;
		case 10:
			UART_Printf("-------------------------------------------\n");	
			UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte1, NAND_EccError.uEccErrBit1);
			UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte2, NAND_EccError.uEccErrBit2);
			UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte3, NAND_EccError.uEccErrBit3);
			UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte4, NAND_EccError.uEccErrBit4);
			UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte5, NAND_EccError.uEccErrBit5);	
			UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte6, NAND_EccError.uEccErrBit6);
			UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte7, NAND_EccError.uEccErrBit7);
			UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte8, NAND_EccError.uEccErrBit8);
			UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte9, NAND_EccError.uEccErrBit9);			
			UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte10, NAND_EccError.uEccErrBit10);						
			UART_Printf("-------------------------------------------\n");				
			break;
		default:
			break;
	}
	#endif

	// Erase Block for test
	eError = NAND_EraseSingleBlock_Basic(g_NandContNum, uBlock);
	if(eError != eNAND_NoError)
	{
		NANDT_PrintErrorType_Sample(eError);
		NAND_Inform[g_NandContNum].uECCtest = 0;
		NAND_Inform[g_NandContNum].uAllBitEccCheck = 0;
		return;
	}
	
	// Create Invalid data
	for ( uIndex0 =0 ; uIndex0<uBitPattern ; uIndex0++)
	{
		switch ( uIndex0+1)
		{
			case 1:
				if ( NAND_Inform[g_NandContNum].u8BitECCErrBytePos[uIndex0] < 512 )
				{
					uTemp =	aBuffer[NAND_EccError.uEccErrByte1] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit1);
					aBuffer[NAND_EccError.uEccErrByte1] = uTemp;				
				}
				else
				{
					uTemp =	aSpareBuffer[(NAND_EccError.uEccErrByte1)-512] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit1);
					aSpareBuffer[(NAND_EccError.uEccErrByte1)-512] = uTemp;				
				}
				break;
			case 2:
				if ( NAND_Inform[g_NandContNum].u8BitECCErrBytePos[uIndex0] < 512 )
				{
					uTemp =	aBuffer[NAND_EccError.uEccErrByte2] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit2);
					aBuffer[NAND_EccError.uEccErrByte2] = uTemp;				
				}
				else
				{
					uTemp =	aSpareBuffer[(NAND_EccError.uEccErrByte2)-512] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit2);
					aSpareBuffer[(NAND_EccError.uEccErrByte2)-512] = uTemp;				
				}
				break;
			case 3:
				if ( NAND_Inform[g_NandContNum].u8BitECCErrBytePos[uIndex0] < 512 )
				{
					uTemp =	aBuffer[NAND_EccError.uEccErrByte3] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit3);
					aBuffer[NAND_EccError.uEccErrByte3] = uTemp;				
				}
				else
				{
					uTemp =	aSpareBuffer[(NAND_EccError.uEccErrByte3)-512] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit3);
					aSpareBuffer[(NAND_EccError.uEccErrByte3)-512] = uTemp;				
				}
				break;
			case 4:
				if ( NAND_Inform[g_NandContNum].u8BitECCErrBytePos[uIndex0] < 512 )
				{
					uTemp =	aBuffer[NAND_EccError.uEccErrByte4] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit4);
					aBuffer[NAND_EccError.uEccErrByte4] = uTemp;				
				}
				else
				{
					uTemp =	aSpareBuffer[(NAND_EccError.uEccErrByte4)-512] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit4);
					aSpareBuffer[(NAND_EccError.uEccErrByte4)-512] = uTemp;				
				}
				break;
			case 5:
				if ( NAND_Inform[g_NandContNum].u8BitECCErrBytePos[uIndex0] < 512 )
				{
					uTemp =	aBuffer[NAND_EccError.uEccErrByte5] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit5);
					aBuffer[NAND_EccError.uEccErrByte5] = uTemp;				
				}
				else
				{
					uTemp =	aSpareBuffer[(NAND_EccError.uEccErrByte5)-512] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit5);
					aSpareBuffer[(NAND_EccError.uEccErrByte5)-512] = uTemp;				
				}
				break;
			case 6:
				if ( NAND_Inform[g_NandContNum].u8BitECCErrBytePos[uIndex0] < 512 )
				{
					uTemp =	aBuffer[NAND_EccError.uEccErrByte6] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit6);
					aBuffer[NAND_EccError.uEccErrByte6] = uTemp;				
				}
				else
				{
					uTemp =	aSpareBuffer[(NAND_EccError.uEccErrByte6)-512] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit6);
					aSpareBuffer[(NAND_EccError.uEccErrByte6)-512] = uTemp;				
				}
				break;
			case 7:
				if ( NAND_Inform[g_NandContNum].u8BitECCErrBytePos[uIndex0] < 512 )
				{
					uTemp =	aBuffer[NAND_EccError.uEccErrByte7] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit7);
					aBuffer[NAND_EccError.uEccErrByte7] = uTemp;				
				}
				else
				{
					uTemp =	aSpareBuffer[(NAND_EccError.uEccErrByte7)-512] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit7);
					aSpareBuffer[(NAND_EccError.uEccErrByte7)-512] = uTemp;				
				}
				break;
			case 8:
				if ( NAND_Inform[g_NandContNum].u8BitECCErrBytePos[uIndex0] < 512 )
				{
					uTemp =	aBuffer[NAND_EccError.uEccErrByte8] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit8);
					aBuffer[NAND_EccError.uEccErrByte8] = uTemp;				
				}
				else
				{
					uTemp =	aSpareBuffer[(NAND_EccError.uEccErrByte8)-512] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit8);
					aSpareBuffer[(NAND_EccError.uEccErrByte8)-512] = uTemp;				
				}
				break;
			case 9:
				if ( NAND_Inform[g_NandContNum].u8BitECCErrBytePos[uIndex0] < 512 )
				{
					uTemp =	aBuffer[NAND_EccError.uEccErrByte9] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit9);
					aBuffer[NAND_EccError.uEccErrByte9] = uTemp;				
				}
				else
				{
					uTemp =	aSpareBuffer[(NAND_EccError.uEccErrByte9)-512] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit9);
					aSpareBuffer[(NAND_EccError.uEccErrByte9)-512] = uTemp;				
				}
				break;
			case 10:
				if ( NAND_Inform[g_NandContNum].u8BitECCErrBytePos[uIndex0] < 512 )
				{
					uTemp =	aBuffer[NAND_EccError.uEccErrByte10] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit10);
					aBuffer[NAND_EccError.uEccErrByte10] = uTemp;				
				}
				else
				{
					uTemp =	aSpareBuffer[(NAND_EccError.uEccErrByte10)-512] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit10);
					aSpareBuffer[(NAND_EccError.uEccErrByte10)-512] = uTemp;				
				}
				break;
			default: 
				break;
		}

	}


	// Write Data to NAND Flash
	NAND_Inform[g_NandContNum].uECCtest = 1;
	eError = NAND_WritePage_8Bit(g_NandContNum, uBlock, uPage, aBuffer, aSpareBuffer);
	if(eError != eNAND_NoError)
	{
		NANDT_PrintErrorType_Sample(eError);
		NAND_Inform[g_NandContNum].uECCtest = 0;
	}
	#if 0
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
	#endif

	// Read Data from NAND flash and Check Error Bit
	NAND_Inform[g_NandContNum].uECCtest = 1;
	NAND_Inform[g_NandContNum].uUncorrectableTest = 1;
	eError = NAND_ReadPage_8Bit(g_NandContNum, uBlock, uPage, aBuffer, aSpareBuffer);
	if(eError != eNAND_NoError)
	{
		NANDT_PrintErrorType_Sample_8BitALLBit(eError);
		NAND_Inform[g_NandContNum].uECCtest = 0;
		NAND_Inform[g_NandContNum].uAllBitEccCheck = 0;
		UART_Printf("\n\n");
	}
#endif

}






void NANDT_8BitECC_SLC_AllPattern(void)
{
//	u32 i, m, n, uBlock, uPage, uError, uErrorType;
	u32 uBlock, uPage;
//	u8 aBuffer[NAND_PAGE_512];
//	u8 aSpareBuffer[NAND_SPARE_16];
//	u8 aReadBuffer[NAND_PAGE_512];
//	NAND_eERROR eError;
//	u32 uTemp;
//	u32 uRoop0, uRoop1, uRoop2, uRoop3, uRoop4, uRoop5, uRoop6, uRoop7;
//	u32 uIndex0, uIndex1;
//	u32 uBit0, uBit1, uBit2, uBit3, uBit4, uBit5, uBit6, uBit7;
//	u32 uByte0, uByte1, uByte2, uByte3, uByte4, uByte5, uByte6, uByte7;
	u32 uBitPattern;
	u32 uTestCounter[100]={0, }, uCounter=0, uCounter1=0, uCounter2=0;

	NAND_Inform[g_NandContNum].uECCtest = 0;
	NAND_Inform[g_NandContNum].uECCType = ECC8Bit;
	NAND_Inform[0].uRandomCount=0;
	
	UART_Printf("01. 1Bit all bit Test\n");
	UART_Printf("Input the block number to test[0~%d]", NAND_Inform[g_NandContNum].uBlockNum-1);
	uBlock = UART_GetIntNum();
	UART_Printf("Input the page number to test[0~%d]", NAND_Inform[g_NandContNum].uPageNum-1);
	uPage = UART_GetIntNum();

	//01. 1bit all pattern test.
	NANDT_8BitECC_SLC_1bitPattern( uBlock, uPage);

	//02. Random check. 
	while (1)
	{
		NANDT_InsertSeed();
		uBitPattern = ( (rand()%8) + 1) ; 
		UART_Printf("== Random Test %d Bit Change ==\n", uBitPattern );
		uCounter++;
		NAND_Inform[0].uRandomCount = uCounter;
		if ( uCounter == 1000000 )
		{
			uBlock++;
			uTestCounter[uCounter2] |= (1<<uCounter1); 
			uCounter1++;			
			uCounter = 0; 
		}
		if ( uCounter1 == 1000000 )
		{ 
			uCounter2++;
			uCounter1=0;
		}
		
		UART_Printf("///// Raw Count = %9d \n", uCounter );		
		UART_Printf("///// Test Count[%d] = 0x%8x \n", uCounter2, uTestCounter[uCounter2] );		
		
		NANDT_8BitECC_SLC_AllbitPattern( uBlock, uPage, uBitPattern);
	}
	

}


void NANDT_8BitECC_SLC_Uncorrectable(void)
{
//	u32 i, m, n, uBlock, uPage, uError, uErrorType;
	u32 uBlock, uPage;
//	u8 aBuffer[NAND_PAGE_512];
//	u8 aSpareBuffer[NAND_SPARE_16];
//	u8 aReadBuffer[NAND_PAGE_512];
//	NAND_eERROR eError;
//	u32 uTemp;
//	u32 uRoop0, uRoop1, uRoop2, uRoop3, uRoop4, uRoop5, uRoop6, uRoop7;
//	u32 uIndex0, uIndex1;
//	u32 uBit0, uBit1, uBit2, uBit3, uBit4, uBit5, uBit6, uBit7;
//	u32 uByte0, uByte1, uByte2, uByte3, uByte4, uByte5, uByte6, uByte7;
	u32 uBitPattern;
	u32 uTestCounter[100]={0,  }, uCounter=0, uCounter1=0, uCounter2=0;

	NAND_Inform[g_NandContNum].uECCtest = 0;
	NAND_Inform[g_NandContNum].uECCType = ECC8Bit;
	NAND_Inform[0].uRandomCount=0;
	
	UART_Printf("Input the block number to test[0~%d]", NAND_Inform[g_NandContNum].uBlockNum-1);
	uBlock = UART_GetIntNum();
	UART_Printf("Input the page number to test[0~%d]", NAND_Inform[g_NandContNum].uPageNum-1);
	uPage = UART_GetIntNum();

	//01. 1bit all pattern test.
//	NANDT_8BitECC_SLC_1bitPattern( uBlock, uPage);

	//02. Random check. 
	while (1)
	{
		NANDT_InsertSeed();
		uBitPattern = ( (rand()%2) + 9) ; 
		UART_Printf("== Random Test %d Bit Change ==\n", uBitPattern );
		uCounter++;
		NAND_Inform[0].uRandomCount = uCounter;
		if ( uCounter == 1000000 )
		{
			uBlock++;
			uTestCounter[uCounter2] |= (1<<uCounter1); 
			uCounter1++;			
			uCounter = 0; 
		}
		if ( uCounter1 == 1000000 )
		{ 
			uCounter2++;
			uCounter1=0;
		}
		
		UART_Printf("///// Raw Count = %9d \n", uCounter );		
		UART_Printf("///// Test Count[%d] = 0x%8x \n", uCounter2, uTestCounter[uCounter2] );		
		
		NANDT_8BitECC_SLC_UnCorrect_Pattern( uBlock, uPage, uBitPattern);
	}
	

}





void NANDT_4BitECC_SLC_1bitPattern(u32 uBlock, u32 uPage)
{
	u32 i, uErrorType;
	u8 aBuffer[NAND_PAGE_512];
	u8 aSpareBuffer[NAND_SPARE_16];
//	u8 aReadBuffer[NAND_PAGE_512];
	NAND_eERROR eError;
	u32 uTemp;
	u32 uRoop0;
//	u32 uRoop1, uRoop2, uRoop3, uRoop4, uRoop5, uRoop6, uRoop7;
//	u32 uIndex0, uIndex1;
	u32 uBit0;
//	u32 uBit1, uBit2, uBit3, uBit4, uBit5, uBit6, uBit7;
	u32 uByte0;
//	u32 uByte1, uByte2, uByte3, uByte4, uByte5, uByte6, uByte7;

	UART_Printf("==================================================\n");
	NAND_Inform[g_NandContNum].u4bitECCPattern = 1; 
	

	for (uRoop0 = 0 ; uRoop0 <= 4151 ; uRoop0++)
	{
		uByte0 = (uRoop0/8);
		uTemp = uRoop0-(uByte0*8);
		uBit0 = uTemp;

		// 01. Erase blocm to test
		uErrorType = NAND_ECCERR_8BIT;
		NAND_EccError.uEccErrByte1 = uByte0;
		NAND_EccError.uEccErrBit1 = uBit0;
		NAND_Inform[g_NandContNum].u4BitECCErrBytePos[0]=NAND_EccError.uEccErrByte1;
		
		NAND_Inform[g_NandContNum].uSLCError4bit_Original[0] = uRoop0;			
		
		for(i=0 ; i<NAND_Inform[g_NandContNum].uPageSize ; i++)
		{
			aBuffer[i] = 0x0;
		}
		for(i=0 ; i<NAND_Inform[g_NandContNum].uSpareSize ; i++)
			aSpareBuffer[i] = 0x0;	

		UART_Printf("\n");
		
		// Erase Block
		eError = NAND_EraseSingleBlock_Basic(g_NandContNum, uBlock);
		if(eError != eNAND_NoError)
		{
			NANDT_PrintErrorType_Sample(eError);
			return;
		}

		// Write valid data 
		NAND_Inform[g_NandContNum].uECCtest = 0;
		eError = NAND_WritePage_4Bit(g_NandContNum, uBlock, uPage, aBuffer, aSpareBuffer);
		if(eError != eNAND_NoError)
		{
			NANDT_PrintErrorType_Sample(eError);
			return;
		}

		// Read Vaild Data : Invalid Data
		eError = NAND_ReadPage_4Bit(g_NandContNum, uBlock, uPage, aBuffer, aSpareBuffer);
		if(eError != eNAND_NoError)
		{
			NANDT_PrintErrorType_Sample(eError);
			NAND_Inform[g_NandContNum].uECCtest = 0;
			return;
		}

		#if 1 
		// Disaply Test Pattern.
		UART_Printf("-------------------------------------------\n");	
		UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte1, NAND_EccError.uEccErrBit1);
		UART_Printf("-------------------------------------------\n");	
		#endif

		// Erase Block for test
		eError = NAND_EraseSingleBlock_Basic(g_NandContNum, uBlock);
		if(eError != eNAND_NoError)
		{
			NANDT_PrintErrorType_Sample(eError);
			NAND_Inform[g_NandContNum].uECCtest = 0;
			NAND_Inform[g_NandContNum].uAllBitEccCheck = 0;
			return;
		}
		
		// Create Invalid data
		if ( NAND_Inform[g_NandContNum].u4BitECCErrBytePos[0] < 512 )
		{
			uTemp =	aBuffer[NAND_EccError.uEccErrByte1] ;
			uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit1);
			aBuffer[NAND_EccError.uEccErrByte1] = uTemp;				
		}
		else
		{
			uTemp =	aSpareBuffer[(NAND_EccError.uEccErrByte1)-512] ;
			uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit1);
			aSpareBuffer[(NAND_EccError.uEccErrByte1)-512] = uTemp;				
		}

		// Write Data to NAND Flash
		NAND_Inform[g_NandContNum].uECCtest = 1;
		eError = NAND_WritePage_4Bit(g_NandContNum, uBlock, uPage, aBuffer, aSpareBuffer);
		if(eError != eNAND_NoError)
		{
			NANDT_PrintErrorType_Sample(eError);
			NAND_Inform[g_NandContNum].uECCtest = 0;
		}
		#if 0
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
		#endif

		// Read Data from NAND flash and Check Error Bit
		NAND_Inform[g_NandContNum].uECCtest = 1;
		eError = NAND_ReadPage_4Bit(g_NandContNum, uBlock, uPage, aBuffer, aSpareBuffer);
		if(eError != eNAND_NoError)
		{
			NANDT_PrintErrorType_Sample_8BitALLBit(eError);
			NAND_Inform[g_NandContNum].uECCtest = 0;
			NAND_Inform[g_NandContNum].uAllBitEccCheck = 0;
			UART_Printf("\n\n");
		}

	}
}


void NANDT_4BitECC_RandomBitPattern( u32 *uRoop0, u32 *uRoop1, u32 *uRoop2, u32 *uRoop3)
{
	u32 uIndex0, uIndex1;
	u32 uTemp0;
	u32 uRandomBit[8];
	u32 uCheckRandom;

	NANDT_InsertSeed();

 	uCheckRandom = 1;

	while ( uCheckRandom )
	{
		uCheckRandom = 0 ; 
		
		for ( uIndex0 = 0 ; uIndex0 <4 ; uIndex0++)
		{
			uTemp0 = rand();
			uRandomBit[uIndex0] = uTemp0%4151;
			if ( uRandomBit[uIndex0] == 4144 ) uCheckRandom = 1; 
			if ( uRandomBit[uIndex0] == 4145 ) uCheckRandom = 1; 
			if ( uRandomBit[uIndex0] == 4146 ) uCheckRandom = 1; 
		}



		for ( uIndex0 = 0 ; uIndex0 < 4 ; uIndex0++)
		{
			for ( uIndex1 = uIndex0+1; uIndex1 < 4 ; uIndex1++)
			{
				if ( uRandomBit[uIndex0] == uRandomBit[uIndex1] )
					uCheckRandom = 1; 
			}
		}
	}

	*uRoop0 = uRandomBit[0];
	*uRoop1 = uRandomBit[1];
	*uRoop2 = uRandomBit[2];
	*uRoop3 = uRandomBit[3];
}

void NANDT_4BitECC_SLC_AllbitPattern(u32 uBlock, u32 uPage, u32 uBitPattern)
{
	u32 i ;
	u8 aBuffer[NAND_PAGE_512];
	u8 aSpareBuffer[NAND_SPARE_16];
//	u8 aReadBuffer[NAND_PAGE_512];
	NAND_eERROR eError;
	u32 uTemp;
	u32 uRoop0, uRoop1, uRoop2, uRoop3;
//	u32  uRoop4, uRoop5, uRoop6, uRoop7;
	u32 uIndex0;
	u32 uBit0, uBit1, uBit2, uBit3;
//	u32  uBit4, uBit5, uBit6, uBit7;
	u32 uByte0, uByte1, uByte2, uByte3;
//	u32  uByte4, uByte5, uByte6, uByte7;

	UART_Printf("==================================================\n");
	NAND_Inform[g_NandContNum].u4bitECCPattern = uBitPattern; 

	// Random Bit Create.
	switch ( NAND_Inform[g_NandContNum].u4bitECCPattern )
	{
		case 1:
			NANDT_4BitECC_RandomBitPattern(&uRoop0, &uRoop1, &uRoop2, &uRoop3);
			uByte0 = (uRoop0/8);
			uTemp = uRoop0 - (uByte0*8);
			uBit0 = uTemp;
			break;
		case 2:
			NANDT_4BitECC_RandomBitPattern(&uRoop0, &uRoop1, &uRoop2, &uRoop3);
			uByte0 = (uRoop0/8);
			uTemp = uRoop0 - (uByte0*8);
			uBit0 = uTemp;
			uByte1 = (uRoop1/8);
			uTemp = uRoop1 - (uByte1*8);
			uBit1 = uTemp;
			break;
		case 3:
			NANDT_4BitECC_RandomBitPattern(&uRoop0, &uRoop1, &uRoop2, &uRoop3);
			uByte0 = (uRoop0/8);
			uTemp = uRoop0 - (uByte0*8);
			uBit0 = uTemp;
			uByte1 = (uRoop1/8);
			uTemp = uRoop1 - (uByte1*8);
			uBit1= uTemp;
			uByte2 = (uRoop2/8);
			uTemp = uRoop2 - (uByte2*8);
			uBit2 = uTemp;
			break;
		case 4:
			NANDT_4BitECC_RandomBitPattern(&uRoop0, &uRoop1, &uRoop2, &uRoop3);
			uByte0 = (uRoop0/8);
			uTemp = uRoop0 - (uByte0*8);
			uBit0 = uTemp;
			uByte1 = (uRoop1/8);
			uTemp = uRoop1 - (uByte1*8);
			uBit1= uTemp;
			uByte2 = (uRoop2/8);
			uTemp = uRoop2 - (uByte2*8);
			uBit2 = uTemp;
			uByte3 = (uRoop3/8);
			uTemp = uRoop3 - (uByte3*8);
			uBit3 = uTemp;
			break;
		default:
			break;
	}

//	uErrorType = NAND_ECCERR_8BIT;

	// 
	switch ( NAND_Inform[g_NandContNum].u4bitECCPattern)
	{
		case 1:
			NAND_EccError.uEccErrByte1 = uByte0;
			NAND_EccError.uEccErrBit1 = uBit0;
			NAND_Inform[g_NandContNum].u4BitECCErrBytePos[0]=NAND_EccError.uEccErrByte1;
			break;
		case 2:
			NAND_EccError.uEccErrByte1 = uByte0;
			NAND_EccError.uEccErrBit1 = uBit0;
			NAND_Inform[g_NandContNum].u4BitECCErrBytePos[0]=NAND_EccError.uEccErrByte1;
			NAND_EccError.uEccErrByte2 = uByte1;
			NAND_EccError.uEccErrBit2 = uBit1;
			NAND_Inform[g_NandContNum].u4BitECCErrBytePos[1]=NAND_EccError.uEccErrByte2;
			break;
		case 3:
			NAND_EccError.uEccErrByte1 = uByte0;
			NAND_EccError.uEccErrBit1 = uBit0;
			NAND_Inform[g_NandContNum].u4BitECCErrBytePos[0]=NAND_EccError.uEccErrByte1;
			NAND_EccError.uEccErrByte2 = uByte1;
			NAND_EccError.uEccErrBit2 = uBit1;
			NAND_Inform[g_NandContNum].u4BitECCErrBytePos[1]=NAND_EccError.uEccErrByte2;
			NAND_EccError.uEccErrByte3 = uByte2;
			NAND_EccError.uEccErrBit3 = uBit2;
			NAND_Inform[g_NandContNum].u4BitECCErrBytePos[2]=NAND_EccError.uEccErrByte3;
			break;
		case 4:
			NAND_EccError.uEccErrByte1 = uByte0;
			NAND_EccError.uEccErrBit1 = uBit0;
			NAND_Inform[g_NandContNum].u4BitECCErrBytePos[0]=NAND_EccError.uEccErrByte1;
			NAND_EccError.uEccErrByte2 = uByte1;
			NAND_EccError.uEccErrBit2 = uBit1;
			NAND_Inform[g_NandContNum].u4BitECCErrBytePos[1]=NAND_EccError.uEccErrByte2;
			NAND_EccError.uEccErrByte3 = uByte2;
			NAND_EccError.uEccErrBit3 = uBit2;
			NAND_Inform[g_NandContNum].u4BitECCErrBytePos[2]=NAND_EccError.uEccErrByte3;
			NAND_EccError.uEccErrByte4 = uByte3;
			NAND_EccError.uEccErrBit4 = uBit3;
			NAND_Inform[g_NandContNum].u4BitECCErrBytePos[3]=NAND_EccError.uEccErrByte4;
			break;
		default :
			break;
	}

	NAND_Inform[g_NandContNum].uSLCError4bit_Original[0] = uRoop0;
	NAND_Inform[g_NandContNum].uSLCError4bit_Original[1] = uRoop1;
	NAND_Inform[g_NandContNum].uSLCError4bit_Original[2] = uRoop2;
	NAND_Inform[g_NandContNum].uSLCError4bit_Original[3] = uRoop3;

	for(i=0 ; i<NAND_Inform[g_NandContNum].uPageSize ; i++)
	{
		aBuffer[i] = 0x0;
	}
	for(i=0 ; i<NAND_Inform[g_NandContNum].uSpareSize ; i++)
		aSpareBuffer[i] = 0x0;	

	UART_Printf("\n");
	
	// Erase Block
	eError = NAND_EraseSingleBlock_Basic(g_NandContNum, uBlock);
	if(eError != eNAND_NoError)
	{
		NANDT_PrintErrorType_Sample(eError);
		return;
	}

	// Write valid data 
	NAND_Inform[g_NandContNum].uECCtest = 0;
	eError = NAND_WritePage_4Bit(g_NandContNum, uBlock, uPage, aBuffer, aSpareBuffer);
	if(eError != eNAND_NoError)
	{
		NANDT_PrintErrorType_Sample(eError);
		return;
	}

	// Read Vaild Data : Invalid Data
	eError = NAND_ReadPage_4Bit(g_NandContNum, uBlock, uPage, aBuffer, aSpareBuffer);
	if(eError != eNAND_NoError)
	{
		NANDT_PrintErrorType_Sample(eError);
		NAND_Inform[g_NandContNum].uECCtest = 0;
		return;
	}

	#if 1 
	// Disaply Test Pattern.
	switch (NAND_Inform[g_NandContNum].u4bitECCPattern )
	{
		case 1:
			UART_Printf("-------------------------------------------\n");	
			UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte1, NAND_EccError.uEccErrBit1);
			UART_Printf("-------------------------------------------\n");	
			break;
		case 2:
			UART_Printf("-------------------------------------------\n");	
			UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte1, NAND_EccError.uEccErrBit1);
			UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte2, NAND_EccError.uEccErrBit2);
			UART_Printf("-------------------------------------------\n");				
			break;
		case 3:
			UART_Printf("-------------------------------------------\n");	
			UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte1, NAND_EccError.uEccErrBit1);
			UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte2, NAND_EccError.uEccErrBit2);
			UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte3, NAND_EccError.uEccErrBit3);
			UART_Printf("-------------------------------------------\n");				
			break;
		case 4:
			UART_Printf("-------------------------------------------\n");	
			UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte1, NAND_EccError.uEccErrBit1);
			UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte2, NAND_EccError.uEccErrBit2);
			UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte3, NAND_EccError.uEccErrBit3);
			UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte4, NAND_EccError.uEccErrBit4);
			UART_Printf("-------------------------------------------\n");				
			break;
		default:
			break;
	}
	#endif

	// Erase Block for test
	eError = NAND_EraseSingleBlock_Basic(g_NandContNum, uBlock);
	if(eError != eNAND_NoError)
	{
		NANDT_PrintErrorType_Sample(eError);
		NAND_Inform[g_NandContNum].uECCtest = 0;
		NAND_Inform[g_NandContNum].uAllBitEccCheck = 0;
		return;
	}
	
	// Create Invalid data
	for ( uIndex0 =0 ; uIndex0<uBitPattern ; uIndex0++)
	{
		switch ( uIndex0+1)
		{
			case 1:
				if ( NAND_Inform[g_NandContNum].u4BitECCErrBytePos[uIndex0] < 512 )
				{
					uTemp =	aBuffer[NAND_EccError.uEccErrByte1] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit1);
					aBuffer[NAND_EccError.uEccErrByte1] = uTemp;				
				}
				else
				{
					uTemp =	aSpareBuffer[(NAND_EccError.uEccErrByte1)-512] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit1);
					aSpareBuffer[(NAND_EccError.uEccErrByte1)-512] = uTemp;				
				}
				break;
			case 2:
				if ( NAND_Inform[g_NandContNum].u4BitECCErrBytePos[uIndex0] < 512 )
				{
					uTemp =	aBuffer[NAND_EccError.uEccErrByte2] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit2);
					aBuffer[NAND_EccError.uEccErrByte2] = uTemp;				
				}
				else
				{
					uTemp =	aSpareBuffer[(NAND_EccError.uEccErrByte2)-512] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit2);
					aSpareBuffer[(NAND_EccError.uEccErrByte2)-512] = uTemp;				
				}
				break;
			case 3:
				if ( NAND_Inform[g_NandContNum].u4BitECCErrBytePos[uIndex0] < 512 )
				{
					uTemp =	aBuffer[NAND_EccError.uEccErrByte3] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit3);
					aBuffer[NAND_EccError.uEccErrByte3] = uTemp;				
				}
				else
				{
					uTemp =	aSpareBuffer[(NAND_EccError.uEccErrByte3)-512] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit3);
					aSpareBuffer[(NAND_EccError.uEccErrByte3)-512] = uTemp;				
				}
				break;
			case 4:
				if ( NAND_Inform[g_NandContNum].u4BitECCErrBytePos[uIndex0] < 512 )
				{
					uTemp =	aBuffer[NAND_EccError.uEccErrByte4] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit4);
					aBuffer[NAND_EccError.uEccErrByte4] = uTemp;				
				}
				else
				{
					uTemp =	aSpareBuffer[(NAND_EccError.uEccErrByte4)-512] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit4);
					aSpareBuffer[(NAND_EccError.uEccErrByte4)-512] = uTemp;				
				}
				break;
			default: 
				break;
		}

	}


	// Write Data to NAND Flash
	NAND_Inform[g_NandContNum].uECCtest = 1;
	eError = NAND_WritePage_4Bit(g_NandContNum, uBlock, uPage, aBuffer, aSpareBuffer);
	if(eError != eNAND_NoError)
	{
		NANDT_PrintErrorType_Sample(eError);
		NAND_Inform[g_NandContNum].uECCtest = 0;
	}
	#if 0
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
	#endif

	// Read Data from NAND flash and Check Error Bit
	NAND_Inform[g_NandContNum].uECCtest = 1;
	eError = NAND_ReadPage_4Bit(g_NandContNum, uBlock, uPage, aBuffer, aSpareBuffer);
	if(eError != eNAND_NoError)
	{
		NANDT_PrintErrorType_Sample_8BitALLBit(eError);
		NAND_Inform[g_NandContNum].uECCtest = 0;
		NAND_Inform[g_NandContNum].uAllBitEccCheck = 0;
		UART_Printf("\n\n");
	}

}

void NANDT_4BitECC_SLC_AllPattern(void)
{
	u32 uBlock, uPage;
//	u8 aBuffer[NAND_PAGE_512];
//	u8 aSpareBuffer[NAND_SPARE_16];
//	u8 aReadBuffer[NAND_PAGE_512];
//	NAND_eERROR eError;
//	u32 uTemp;
//	u32 uRoop0, uRoop1, uRoop2, uRoop3, uRoop4, uRoop5, uRoop6, uRoop7;
//	u32 uIndex0, uIndex1;
//	u32 uBit0, uBit1, uBit2, uBit3, uBit4, uBit5, uBit6, uBit7;
//	u32 uByte0, uByte1, uByte2, uByte3, uByte4, uByte5, uByte6, uByte7;
	u32 uBitPattern;
	u32 uTestCounter[100]={0, }, uCounter=0, uCounter1=0, uCounter2=0;

	NAND_Inform[g_NandContNum].uECCtest = 0;
	NAND_Inform[g_NandContNum].uECCType = ECC4Bit;
	NAND_Inform[0].uRandomCount=0;
	
	UART_Printf("01. 1Bit all bit Test\n");
	UART_Printf("Input the block number to test[0~%d]", NAND_Inform[g_NandContNum].uBlockNum-1);
	uBlock = UART_GetIntNum();
	UART_Printf("Input the page number to test[0~%d]", NAND_Inform[g_NandContNum].uPageNum-1);
	uPage = UART_GetIntNum();

	//01. 1bit all pattern test.
	NANDT_4BitECC_SLC_1bitPattern(uBlock, uPage);

	//02. Random check. 
	while (1)
	{
		NANDT_InsertSeed();
		uBitPattern = ( (rand()%4) + 1) ; 
		UART_Printf("== Random Test %d Bit Change ==\n", uBitPattern );
		uCounter++;
		if ( uCounter == 1000000 )
		{
			uBlock++;
			uTestCounter[uCounter2] |= (1<<uCounter1); 
			uCounter1++;			
			uCounter = 0; 
		}
		if ( uCounter1 == 1000000 )
		{ 
			uCounter2++;
			uCounter1=0;
		}
		
		UART_Printf("///// Raw Count = %9d \n", uCounter );		
		UART_Printf("///// Test Count[%d] = 0x%8x \n", uCounter2, uTestCounter[uCounter2] );		
		
		NANDT_4BitECC_SLC_AllbitPattern( uBlock, uPage, uBitPattern);
	}
	

}


//////////
// Function Name : NANDT_4BitECC_SLC
// Function Description : MLC ECC function test
// Input : 	None
// Output : 	None
void NANDT_4BitECC_SLC(void)
{
	u32 i, uBlock, uPage, uError, uErrorType;
	u8 aBuffer[NAND_PAGE_512];
	u8 aSpareBuffer[NAND_SPARE_16];
//	u8 aReadBuffer[NAND_PAGE_512];
	NAND_eERROR eError;
	u32 uTemp;
	u32 uIndex0;


	NAND_EccError.uEccErrBit1 = 0;
	NAND_EccError.uEccErrByte1 = 0;
	NAND_EccError.uEccErrBit2 = 0;
	NAND_EccError.uEccErrByte2 = 0;
	NAND_EccError.uEccErrBit3 = 0;
	NAND_EccError.uEccErrByte3 = 0;
	NAND_EccError.uEccErrBit4 = 0;
	NAND_EccError.uEccErrByte4 = 0;
	NAND_EccError.uEccErrBit5 = 0;
	NAND_EccError.uEccErrByte5 = 0;
	NAND_EccError.uEccErrBit6 = 0;
	NAND_EccError.uEccErrByte6 = 0;
	NAND_EccError.uEccErrBit7 = 0;
	NAND_EccError.uEccErrByte7 = 0;



	NAND_Inform[g_NandContNum].uECCtest = 0;
	NAND_Inform[g_NandContNum].uECCType = ECC4Bit;
	
	UART_Printf("[NAND_4BitECC_BasicTestl]\n");

	// 01. Input wanted 8bit ecc test block
	UART_Printf("Input the Block Number to test[0~%d]", NAND_Inform[g_NandContNum].uBlockNum-1);
	uBlock = UART_GetIntNum();
	UART_Printf("Input the Page Number to test[0~%d]", NAND_Inform[g_NandContNum].uPageNum-1);
	uPage = UART_GetIntNum();	

	// 02. select ecc type
	UART_Printf("Input the ECC Error Type\n");
	UART_Printf("1: 1bit error(D)	2: 2bit Error	3: 3bit Error     4: 4Bit Error     5:Uncorrectabel Error(5Bit)  7: 7Bit(For Test)\n" );
	uError = UART_GetIntNum();

	// 03. Input byte positoin and bit position
	switch(uError)
	{
		case 1 :	uErrorType = NAND_ECCERR_1BIT;
				UART_Printf("Input the First Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte1 = UART_GetIntNum()%528;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[0]=NAND_EccError.uEccErrByte1;
				UART_Printf("Input the First Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit1 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[0] = NAND_EccError.uEccErrByte1 ;
				break;
		case 2 :	uErrorType = NAND_ECCERR_2BIT;
				UART_Printf("Input the 1st Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte1 = UART_GetIntNum()%528;
				UART_Printf("Input the 1st Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit1 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[0]=NAND_EccError.uEccErrByte1;
				UART_Printf("Input the 2nd Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte2 = UART_GetIntNum()%528;
				UART_Printf("Input the 2nd Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit2 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[1]=NAND_EccError.uEccErrByte2;
				break;
		case 3 :	uErrorType = NAND_ECCERR_3BIT;
				UART_Printf("Input the 1st Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte1 = UART_GetIntNum()%528;
				UART_Printf("Input the 1st Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit1 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[0]=NAND_EccError.uEccErrByte1;
				UART_Printf("Input the 2nd Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte2 = UART_GetIntNum()%528;
				UART_Printf("Input the 2nd Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit2 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[1]=NAND_EccError.uEccErrByte2;
				UART_Printf("Input the 3th Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte3 = UART_GetIntNum()%528;
				UART_Printf("Input the 3th Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit3 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[2]=NAND_EccError.uEccErrByte3;
				break;
		case 4 :	uErrorType = NAND_ECCERR_4BIT;
				UART_Printf("Input the 1st Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte1 = UART_GetIntNum()%528;
				UART_Printf("Input the 1st Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit1 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[0]=NAND_EccError.uEccErrByte1;
				UART_Printf("Input the 2nd Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte2 = UART_GetIntNum()%528;
				UART_Printf("Input the 2nd Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit2 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[1]=NAND_EccError.uEccErrByte2;
				UART_Printf("Input the 3th Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte3 = UART_GetIntNum()%528;
				UART_Printf("Input the 3th Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit3 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[2]=NAND_EccError.uEccErrByte3;
				UART_Printf("Input the 4th Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte4 = UART_GetIntNum()%528;
				UART_Printf("Input the 4th Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit4 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[3]=NAND_EccError.uEccErrByte4;
				break;
		case 5 :	uErrorType = NAND_ECCERR_5BIT;
				UART_Printf("Input the 1st Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte1 = UART_GetIntNum()%528;
				UART_Printf("Input the 1st Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit1 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[0]=NAND_EccError.uEccErrByte1;
				UART_Printf("Input the 2nd Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte2 = UART_GetIntNum()%528;
				UART_Printf("Input the 2nd Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit2 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[1]=NAND_EccError.uEccErrByte2;
				UART_Printf("Input the 3th Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte3 = UART_GetIntNum()%528;
				UART_Printf("Input the 3th Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit3 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[2]=NAND_EccError.uEccErrByte3;
				UART_Printf("Input the 4th Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte4 = UART_GetIntNum()%528;
				UART_Printf("Input the 4th Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit4 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[3]=NAND_EccError.uEccErrByte4;
				UART_Printf("Input the 5th Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte5 = UART_GetIntNum()%528;
				UART_Printf("Input the 5th Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit5 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[4]=NAND_EccError.uEccErrByte5;
				break;

		case 7 :	uErrorType = NAND_ECCERR_7BIT;
				UART_Printf("Input the 1st Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte1 = UART_GetIntNum()%528;
				UART_Printf("Input the 1st Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit1 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[0]=NAND_EccError.uEccErrByte1;
				UART_Printf("Input the 2nd Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte2 = UART_GetIntNum()%528;
				UART_Printf("Input the 2nd Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit2 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[1]=NAND_EccError.uEccErrByte2;
				UART_Printf("Input the 3th Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte3 = UART_GetIntNum()%528;
				UART_Printf("Input the 3th Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit3 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[2]=NAND_EccError.uEccErrByte3;
				UART_Printf("Input the 4th Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte4 = UART_GetIntNum()%528;
				UART_Printf("Input the 4th Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit4 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[3]=NAND_EccError.uEccErrByte4;
				UART_Printf("Input the 5th Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte5 = UART_GetIntNum()%528;
				UART_Printf("Input the 5th Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit5 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[4]=NAND_EccError.uEccErrByte5;
				UART_Printf("Input the 6th Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte6 = UART_GetIntNum()%528;
				UART_Printf("Input the 6th Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit6 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[5]=NAND_EccError.uEccErrByte6;
				UART_Printf("Input the 7th Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte7 = UART_GetIntNum()%528;
				UART_Printf("Input the 7th Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit7 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[6]=NAND_EccError.uEccErrByte7;

				break;

		default : uErrorType = 0;
			break;
	}

	// 04. Save page and spare data to buffer : aBuffer and aSpareBuffer
	for(i=0 ; i<NAND_Inform[g_NandContNum].uPageSize ; i++)
	{
		aBuffer[i] = 0x0;
	}
	for(i=0 ; i<NAND_Inform[g_NandContNum].uSpareSize ; i++)
		aSpareBuffer[i] = 0x0;	

	UART_Printf("\n");
	
	// 05. Erase Block for test
	UART_Printf("----------------------------------\n");
	UART_Printf("01. Erase the Block ( %d[block])\n", uBlock);
	eError = NAND_EraseSingleBlock_Basic(g_NandContNum, uBlock);
	if(eError != eNAND_NoError)
	{
		NANDT_PrintErrorType_Sample(eError);
		return;
	}

	// 06. Write the valid data to page ( uBlock[block] 0[page] )
	UART_Printf("02. Write the valid data to page for ECC generation ( %d[block] %d[page] )\n", uBlock, uPage);
	NAND_Inform[g_NandContNum].uECCtest = 0;
	eError = NAND_WritePage_4Bit(g_NandContNum, uBlock, uPage, aBuffer, aSpareBuffer);
	if(eError != eNAND_NoError)
	{
		NANDT_PrintErrorType_Sample(eError);
		return;
	}

	// 07. Read the valid data from page(uBlock[block] 0[page] )
	UART_Printf("03. Read the valid data in page for ECC generation (%d[block]   %d[Page])\n", uBlock, uPage);
	eError = NAND_ReadPage_4Bit(g_NandContNum, uBlock, uPage, aBuffer, aSpareBuffer);
	if(eError != eNAND_NoError)
	{
		NANDT_PrintErrorType_Sample(eError);
		NAND_Inform[g_NandContNum].uECCtest = 0;
		return;
	}
	else
	{
		#if 1
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
		#endif		
		UART_Printf("NAND Vaild Data Read Page : Success\n");
	}

	// 08. Display test condition
	// Test Start......!!
	UART_Printf("-------------------------------------------\n");	
	UART_Printf("-------------- InValid Data Create   ----------\n");		
	switch (uError)
	{
		case 1: 
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte1, NAND_EccError.uEccErrBit1);
				break;
		case 2: 
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte1, NAND_EccError.uEccErrBit1);
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte2, NAND_EccError.uEccErrBit2);
				break;
		case 3: 
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte1, NAND_EccError.uEccErrBit1);
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte2, NAND_EccError.uEccErrBit2);
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte3, NAND_EccError.uEccErrBit3);
				break;
		case 4: 
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte1, NAND_EccError.uEccErrBit1);
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte2, NAND_EccError.uEccErrBit2);
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte3, NAND_EccError.uEccErrBit3);
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte4, NAND_EccError.uEccErrBit4);
				break;
		case 5: 
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte1, NAND_EccError.uEccErrBit1);
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte2, NAND_EccError.uEccErrBit2);
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte3, NAND_EccError.uEccErrBit3);
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte4, NAND_EccError.uEccErrBit4);
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte5, NAND_EccError.uEccErrBit5);	
				break;
		case 7: 
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte1, NAND_EccError.uEccErrBit1);
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte2, NAND_EccError.uEccErrBit2);
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte3, NAND_EccError.uEccErrBit3);
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte4, NAND_EccError.uEccErrBit4);
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte5, NAND_EccError.uEccErrBit5);	
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte6, NAND_EccError.uEccErrBit6);					
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte7, NAND_EccError.uEccErrBit7);	
				break;
		default : break;
		}
	UART_Printf("-------------------------------------------\n");	
	
	// 09. Erase Block for test
	UART_Printf("03. Erase the Block ( %d[block])\n", uBlock);
	eError = NAND_EraseSingleBlock_Basic(g_NandContNum, uBlock);
	if(eError != eNAND_NoError)
	{
		NANDT_PrintErrorType_Sample(eError);
		NAND_Inform[g_NandContNum].uECCtest = 0;
		NAND_Inform[g_NandContNum].uAllBitEccCheck = 0;
		return;
	}

	// 10. Create Invalid data
	for ( uIndex0 =0 ; uIndex0<uError ; uIndex0++)
	{
		switch ( uIndex0+1)
		{
			case 1:
				if ( NAND_Inform[g_NandContNum].u8BitECCErrBytePos[uIndex0] < 512 )
				{
					uTemp =	aBuffer[NAND_EccError.uEccErrByte1] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit1);
					aBuffer[NAND_EccError.uEccErrByte1] = uTemp;				
				}
				else
				{
					uTemp =	aSpareBuffer[(NAND_EccError.uEccErrByte1)-512] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit1);
					aSpareBuffer[(NAND_EccError.uEccErrByte1)-512] = uTemp;				
				}
				break;
			case 2:
				if ( NAND_Inform[g_NandContNum].u8BitECCErrBytePos[uIndex0] < 512 )
				{
					uTemp =	aBuffer[NAND_EccError.uEccErrByte2] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit2);
					aBuffer[NAND_EccError.uEccErrByte2] = uTemp;				
				}
				else
				{
					uTemp =	aSpareBuffer[(NAND_EccError.uEccErrByte2)-512] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit2);
					aSpareBuffer[(NAND_EccError.uEccErrByte2)-512] = uTemp;				
				}
				break;
			case 3:
				if ( NAND_Inform[g_NandContNum].u8BitECCErrBytePos[uIndex0] < 512 )
				{
					uTemp =	aBuffer[NAND_EccError.uEccErrByte3] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit3);
					aBuffer[NAND_EccError.uEccErrByte3] = uTemp;				
				}
				else
				{
					uTemp =	aSpareBuffer[(NAND_EccError.uEccErrByte3)-512] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit3);
					aSpareBuffer[(NAND_EccError.uEccErrByte3)-512] = uTemp;				
				}
				break;
			case 4:
				if ( NAND_Inform[g_NandContNum].u8BitECCErrBytePos[uIndex0] < 512 )
				{
					uTemp =	aBuffer[NAND_EccError.uEccErrByte4] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit4);
					aBuffer[NAND_EccError.uEccErrByte4] = uTemp;				
				}
				else
				{
					uTemp =	aSpareBuffer[(NAND_EccError.uEccErrByte4)-512] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit4);
					aSpareBuffer[(NAND_EccError.uEccErrByte4)-512] = uTemp;				
				}
				break;
			case 5:
				if ( NAND_Inform[g_NandContNum].u8BitECCErrBytePos[uIndex0] < 512 )
				{
					uTemp =	aBuffer[NAND_EccError.uEccErrByte5] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit5);
					aBuffer[NAND_EccError.uEccErrByte5] = uTemp;				
				}
				else
				{
					uTemp =	aSpareBuffer[(NAND_EccError.uEccErrByte5)-512] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit5);
					aSpareBuffer[(NAND_EccError.uEccErrByte5)-512] = uTemp;				
				}
				break;
			case 6:
				if ( NAND_Inform[g_NandContNum].u8BitECCErrBytePos[uIndex0] < 512 )
				{
					uTemp =	aBuffer[NAND_EccError.uEccErrByte6] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit6);
					aBuffer[NAND_EccError.uEccErrByte6] = uTemp;				
				}
				else
				{
					uTemp =	aSpareBuffer[(NAND_EccError.uEccErrByte6)-512] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit6);
					aSpareBuffer[(NAND_EccError.uEccErrByte6)-512] = uTemp;				
				}
				break;
			case 7:
				if ( NAND_Inform[g_NandContNum].u8BitECCErrBytePos[uIndex0] < 512 )
				{
					uTemp =	aBuffer[NAND_EccError.uEccErrByte7] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit7);
					aBuffer[NAND_EccError.uEccErrByte7] = uTemp;				
				}
				else
				{
					uTemp =	aSpareBuffer[(NAND_EccError.uEccErrByte7)-512] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit7);
					aSpareBuffer[(NAND_EccError.uEccErrByte7)-512] = uTemp;				
				}
				break;
			default: 
				break;
		}
	}


	// 11. Write Data to NAND Flash
	UART_Printf("04. After invalid data create , write NAND flash\n");
	NAND_Inform[g_NandContNum].uECCtest = 1;
	eError = NAND_WritePage_4Bit(g_NandContNum, uBlock, uPage, aBuffer, aSpareBuffer);
	if(eError != eNAND_NoError)
	{
		NANDT_PrintErrorType_Sample(eError);
		NAND_Inform[g_NandContNum].uECCtest = 0;
	}
	else
	{
	#if 0
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
	#endif		
	UART_Printf("NAND Invalid Data Write Page : Success\n");
	}

	UART_Printf("\n");

	// 12. Read Data from NAND flash
	UART_Printf("05. Read NAND flash\n");
	NAND_Inform[g_NandContNum].uECCtest = 1;
	eError = NAND_ReadPage_4Bit(g_NandContNum, uBlock, uPage, aBuffer, aSpareBuffer);
	if(eError != eNAND_NoError)
	{
		NANDT_PrintErrorType_Sample(eError);
		NAND_Inform[g_NandContNum].uECCtest = 0;
		NAND_Inform[g_NandContNum].uAllBitEccCheck = 0;
		UART_Printf("Read page Error [%d block  %d page]\n", uBlock, uPage);
		UART_Printf("-------------------------------------------\n");	
#if 1 
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
#endif		
	}
	else
	{
#if 0
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
#endif		
		UART_Printf("NAND Read Page : Success\n");
	}
	UART_Printf("\n");
	
	NAND_Inform[g_NandContNum].uECCtest = 0;
	NAND_Inform[g_NandContNum].uAllBitEccCheck = 0;
	
	UART_Printf("SLC Nand 4bit ECC test complete & OK...!!\n");
	UART_Printf("\n");	
}

//////////
// Function Name : NANDT_4BitECC_SLC
// Function Description : MLC ECC function test
// Input : 	None
// Output : 	None
void NANDT_4BitECC_SLC_Test(void)
{
	u32 i,uBlock, uPage, uError, uErrorType;
	u8 aBuffer[NAND_PAGE_512];
	u8 aSpareBuffer[NAND_SPARE_16];
//	u8 aReadBuffer[NAND_PAGE_512];
	NAND_eERROR eError;
	u32 uTemp;
	u32 uIndex0, uIndex1;


	NAND_EccError.uEccErrBit1 = 0;
	NAND_EccError.uEccErrByte1 = 0;
	NAND_EccError.uEccErrBit2 = 0;
	NAND_EccError.uEccErrByte2 = 0;
	NAND_EccError.uEccErrBit3 = 0;
	NAND_EccError.uEccErrByte3 = 0;
	NAND_EccError.uEccErrBit4 = 0;
	NAND_EccError.uEccErrByte4 = 0;
	NAND_EccError.uEccErrBit5 = 0;
	NAND_EccError.uEccErrByte5 = 0;

	NAND_Inform[g_NandContNum].uECCtest = 0;
	NAND_Inform[g_NandContNum].uECCType = ECC4Bit;
	
	UART_Printf("[NAND_4BitECC_BasicTestl]\n");

	// 01. Input wanted 8bit ecc test block
	UART_Printf("Input the Block Number to test[0~%d]", NAND_Inform[g_NandContNum].uBlockNum-1);
	uBlock = UART_GetIntNum();
	UART_Printf("Input the Page Number to test[0~%d]", NAND_Inform[g_NandContNum].uPageNum-1);
	uPage = UART_GetIntNum();	

	// 02. select ecc type
	UART_Printf("Input the ECC Error Type\n");
	UART_Printf("1: 1bit error(D)	2: 2bit Error	3: 3bit Error     4: 4Bit Error     5:Uncorrectabel Error(5Bit)\n");
	uError = UART_GetIntNum();

	// 03. Input byte positoin and bit position
	switch(uError)
	{
		case 1 :	uErrorType = NAND_ECCERR_1BIT;
				UART_Printf("Input the First Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte1 = UART_GetIntNum()%528;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[0]=NAND_EccError.uEccErrByte1;
				UART_Printf("Input the First Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit1 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[0] = NAND_EccError.uEccErrByte1 ;
				break;
		case 2 :	uErrorType = NAND_ECCERR_2BIT;
				UART_Printf("Input the 1st Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte1 = UART_GetIntNum()%528;
				UART_Printf("Input the 1st Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit1 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[0]=NAND_EccError.uEccErrByte1;
				UART_Printf("Input the 2nd Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte2 = UART_GetIntNum()%528;
				UART_Printf("Input the 2nd Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit2 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[1]=NAND_EccError.uEccErrByte2;
				break;
		case 3 :	uErrorType = NAND_ECCERR_3BIT;
				UART_Printf("Input the 1st Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte1 = UART_GetIntNum()%528;
				UART_Printf("Input the 1st Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit1 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[0]=NAND_EccError.uEccErrByte1;
				UART_Printf("Input the 2nd Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte2 = UART_GetIntNum()%528;
				UART_Printf("Input the 2nd Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit2 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[1]=NAND_EccError.uEccErrByte2;
				UART_Printf("Input the 3th Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte3 = UART_GetIntNum()%528;
				UART_Printf("Input the 3th Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit3 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[2]=NAND_EccError.uEccErrByte3;
				break;
		case 4 :	uErrorType = NAND_ECCERR_4BIT;
				UART_Printf("Input the 1st Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte1 = UART_GetIntNum()%528;
				UART_Printf("Input the 1st Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit1 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[0]=NAND_EccError.uEccErrByte1;
				UART_Printf("Input the 2nd Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte2 = UART_GetIntNum()%528;
				UART_Printf("Input the 2nd Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit2 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[1]=NAND_EccError.uEccErrByte2;
				UART_Printf("Input the 3th Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte3 = UART_GetIntNum()%528;
				UART_Printf("Input the 3th Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit3 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[2]=NAND_EccError.uEccErrByte3;
				UART_Printf("Input the 4th Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte4 = UART_GetIntNum()%528;
				UART_Printf("Input the 4th Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit4 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[3]=NAND_EccError.uEccErrByte4;
				break;
		case 5 :	uErrorType = NAND_ECCERR_5BIT;
				UART_Printf("Input the 1st Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte1 = UART_GetIntNum()%528;
				UART_Printf("Input the 1st Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit1 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[0]=NAND_EccError.uEccErrByte1;
				UART_Printf("Input the 2nd Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte2 = UART_GetIntNum()%528;
				UART_Printf("Input the 2nd Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit2 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[1]=NAND_EccError.uEccErrByte2;
				UART_Printf("Input the 3th Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte3 = UART_GetIntNum()%528;
				UART_Printf("Input the 3th Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit3 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[2]=NAND_EccError.uEccErrByte3;
				UART_Printf("Input the 4th Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte4 = UART_GetIntNum()%528;
				UART_Printf("Input the 4th Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit4 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[3]=NAND_EccError.uEccErrByte4;
				UART_Printf("Input the 5th Byte Postion[0 ~ 527]");
				NAND_EccError.uEccErrByte5 = UART_GetIntNum()%528;
				UART_Printf("Input the 5th Bit Postion[0 ~ 7]");
				NAND_EccError.uEccErrBit5 = UART_GetIntNum()%8;
				NAND_Inform[g_NandContNum].u8BitECCErrBytePos[4]=NAND_EccError.uEccErrByte5;
				break;
		default : uErrorType = 0;
			break;
	}

	// 04. Save page and spare data to buffer : aBuffer and aSpareBuffer
	for(i=0 ; i<NAND_Inform[g_NandContNum].uPageSize ; i++)
	{
		aBuffer[i] = 0x0;
	}
	for(i=0 ; i<NAND_Inform[g_NandContNum].uSpareSize ; i++)
		aSpareBuffer[i] = 0x0;	

	UART_Printf("\n");
	
	// 05. Erase Block for test
	UART_Printf("----------------------------------\n");
	UART_Printf("01. Erase the Block ( %d[block])\n", uBlock);
	eError = NAND_EraseSingleBlock_Basic(g_NandContNum, uBlock);
	if(eError != eNAND_NoError)
	{
		NANDT_PrintErrorType_Sample(eError);
		return;
	}

	// 06. Write the valid data to page ( uBlock[block] 0[page] )
	UART_Printf("02. Write the valid data to page for ECC generation ( %d[block] %d[page] )\n", uBlock, uPage);
	NAND_Inform[g_NandContNum].uECCtest = 0;
	eError = NAND_WritePage_4Bit(g_NandContNum, uBlock, uPage, aBuffer, aSpareBuffer);
	if(eError != eNAND_NoError)
	{
		NANDT_PrintErrorType_Sample(eError);
		return;
	}

	// 07. Read the valid data from page(uBlock[block] 0[page] )
	UART_Printf("03. Read the valid data in page for ECC generation (%d[block]   %d[Page])\n", uBlock, uPage);
	eError = NAND_ReadPage_4Bit(g_NandContNum, uBlock, uPage, aBuffer, aSpareBuffer);
	if(eError != eNAND_NoError)
	{
		NANDT_PrintErrorType_Sample(eError);
		NAND_Inform[g_NandContNum].uECCtest = 0;
		return;
	}
	else
	{
#if 1
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
#endif		
		UART_Printf("NAND Vaild Data Read Page : Success\n");
	}

	// 08. Display test condition
	// Test Start......!!
	UART_Printf("-------------------------------------------\n");	
	UART_Printf("-------------- InValid Data Create   ----------\n");		
	switch (uError)
	{
		case 1: 
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte1, NAND_EccError.uEccErrBit1);
				break;
		case 2: 
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte1, NAND_EccError.uEccErrBit1);
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte2, NAND_EccError.uEccErrBit2);
				break;
		case 3: 
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte1, NAND_EccError.uEccErrBit1);
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte2, NAND_EccError.uEccErrBit2);
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte3, NAND_EccError.uEccErrBit3);
				break;
		case 4: 
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte1, NAND_EccError.uEccErrBit1);
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte2, NAND_EccError.uEccErrBit2);
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte3, NAND_EccError.uEccErrBit3);
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte4, NAND_EccError.uEccErrBit4);
				break;
		case 5: 
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte1, NAND_EccError.uEccErrBit1);
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte2, NAND_EccError.uEccErrBit2);
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte3, NAND_EccError.uEccErrBit3);
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte4, NAND_EccError.uEccErrBit4);
				UART_Printf("[%8d byte : %8d bit]\n", NAND_EccError.uEccErrByte5, NAND_EccError.uEccErrBit5);	
				break;
		default : break;
		}
	UART_Printf("-------------------------------------------\n");	
	
	// 09. Erase Block for test
	UART_Printf("03. Erase the Block ( %d[block])\n", uBlock);
	eError = NAND_EraseSingleBlock_Basic(g_NandContNum, uBlock);
	if(eError != eNAND_NoError)
	{
		NANDT_PrintErrorType_Sample(eError);
		NAND_Inform[g_NandContNum].uECCtest = 0;
		NAND_Inform[g_NandContNum].uAllBitEccCheck = 0;
		return;
	}

	// 10. Create Invalid data

	for ( uIndex0 =0 ; uIndex0<uError ; uIndex0++)
	{
		switch ( uIndex0+1)
		{
			case 1:
				if ( NAND_Inform[g_NandContNum].u8BitECCErrBytePos[uIndex0] < 512 )
				{
					uTemp =	aBuffer[NAND_EccError.uEccErrByte1] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit1);
					aBuffer[NAND_EccError.uEccErrByte1] = uTemp;				
				}
				else
				{
					uTemp =	aSpareBuffer[(NAND_EccError.uEccErrByte1)-512] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit1);
					aSpareBuffer[(NAND_EccError.uEccErrByte1)-512] = uTemp;				
				}
				break;
			case 2:
				if ( NAND_Inform[g_NandContNum].u8BitECCErrBytePos[uIndex0] < 512 )
				{
					uTemp =	aBuffer[NAND_EccError.uEccErrByte2] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit2);
					aBuffer[NAND_EccError.uEccErrByte2] = uTemp;				
				}
				else
				{
					uTemp =	aSpareBuffer[(NAND_EccError.uEccErrByte2)-512] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit2);
					aSpareBuffer[(NAND_EccError.uEccErrByte2)-512] = uTemp;				
				}
				break;
			case 3:
				if ( NAND_Inform[g_NandContNum].u8BitECCErrBytePos[uIndex0] < 512 )
				{
					uTemp =	aBuffer[NAND_EccError.uEccErrByte3] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit3);
					aBuffer[NAND_EccError.uEccErrByte3] = uTemp;				
				}
				else
				{
					uTemp =	aSpareBuffer[(NAND_EccError.uEccErrByte3)-512] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit3);
					aSpareBuffer[(NAND_EccError.uEccErrByte3)-512] = uTemp;				
				}
				break;
			case 4:
				if ( NAND_Inform[g_NandContNum].u8BitECCErrBytePos[uIndex0] < 512 )
				{
					uTemp =	aBuffer[NAND_EccError.uEccErrByte4] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit4);
					aBuffer[NAND_EccError.uEccErrByte4] = uTemp;				
				}
				else
				{
					uTemp =	aSpareBuffer[(NAND_EccError.uEccErrByte4)-512] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit4);
					aSpareBuffer[(NAND_EccError.uEccErrByte4)-512] = uTemp;				
				}
				break;
			case 5:
				if ( NAND_Inform[g_NandContNum].u8BitECCErrBytePos[uIndex0] < 512 )
				{
					uTemp =	aBuffer[NAND_EccError.uEccErrByte5] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit5);
					aBuffer[NAND_EccError.uEccErrByte5] = uTemp;				
				}
				else
				{
					uTemp =	aSpareBuffer[(NAND_EccError.uEccErrByte5)-512] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit5);
					aSpareBuffer[(NAND_EccError.uEccErrByte5)-512] = uTemp;				
				}
				break;
			default: 
				break;
		}
	}


	// 11. Write Data to NAND Flash
	UART_Printf("04. After invalid data create , write NAND flash\n");
	NAND_Inform[g_NandContNum].uECCtest = 1;
	eError = NAND_WritePage_4Bit(g_NandContNum, uBlock, uPage, aBuffer, aSpareBuffer);
	if(eError != eNAND_NoError)
	{
		NANDT_PrintErrorType_Sample(eError);
		NAND_Inform[g_NandContNum].uECCtest = 0;
	}
	else
	{
#if 0
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
#endif		
		UART_Printf("NAND Invalid Data Write Page : Success\n");
	}

	UART_Printf("\n");

	for ( uIndex0 = 0 ; uIndex0 < 4 ; uIndex0++)
	{
		for ( uIndex1 = 0 ; uIndex1 < 2; uIndex1++)
		{
			NAND_Inform[g_NandContNum].u4BitECCCrrectTable[uIndex0][uIndex1] = 0;
		}

	}

	// 12. Read Data from NAND flash
	UART_Printf("05. Read NAND flash\n");
	NAND_Inform[g_NandContNum].uECCtest = 1;
	eError = NAND_ReadPage_4Bit(g_NandContNum, uBlock, uPage, aBuffer, aSpareBuffer);
	if(eError != eNAND_NoError)
	{
		NANDT_PrintErrorType_Sample(eError);
		NAND_Inform[g_NandContNum].uECCtest = 0;
		NAND_Inform[g_NandContNum].uAllBitEccCheck = 0;
		UART_Printf("Read page Error [%d block  %d page]\n", uBlock, uPage);
		UART_Printf("-------------------------------------------\n");	
#if 1 
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
#endif		
	}
	else
	{
#if 0
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
#endif		
	}
	UART_Printf("\n");


	NAND_EccError.uEccErrByte1 = NAND_Inform[g_NandContNum].u4BitECCCrrectTable[0][0];
	NAND_EccError.uEccErrBit1    = NAND_Inform[g_NandContNum].u4BitECCCrrectTable[0][1];
	NAND_EccError.uEccErrByte2 = NAND_Inform[g_NandContNum].u4BitECCCrrectTable[1][0];
	NAND_EccError.uEccErrBit2    = NAND_Inform[g_NandContNum].u4BitECCCrrectTable[1][1];
	NAND_EccError.uEccErrByte3 = NAND_Inform[g_NandContNum].u4BitECCCrrectTable[2][0];
	NAND_EccError.uEccErrBit3    = NAND_Inform[g_NandContNum].u4BitECCCrrectTable[2][1];
	NAND_EccError.uEccErrByte4 = NAND_Inform[g_NandContNum].u4BitECCCrrectTable[3][0];
	NAND_EccError.uEccErrBit4    = NAND_Inform[g_NandContNum].u4BitECCCrrectTable[3][1];


	// 13. Change Correct Erro Bit
	for ( uIndex0 =0 ; uIndex0<uError ; uIndex0++)
	{
		switch ( uIndex0+1)
		{
			case 1:
				if ( NAND_Inform[g_NandContNum].u8BitECCErrBytePos[uIndex0] < 512 )
				{
					uTemp =	aBuffer[NAND_EccError.uEccErrByte1] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit1);
					aBuffer[NAND_EccError.uEccErrByte1] = uTemp;				
				}
				else
				{
					if ( NAND_Inform[g_NandContNum].u8BitECCErrBytePos[uIndex0] != 518 ) 
					{
						uTemp =	aSpareBuffer[(NAND_EccError.uEccErrByte1)-512] ;
						uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit1);
						aSpareBuffer[(NAND_EccError.uEccErrByte1)-512] = uTemp;				
					}
				}
				break;
			case 2:
				if ( NAND_Inform[g_NandContNum].u8BitECCErrBytePos[uIndex0] < 512 )
				{
					uTemp =	aBuffer[NAND_EccError.uEccErrByte2] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit2);
					aBuffer[NAND_EccError.uEccErrByte2] = uTemp;				
				}
				else
				{
					if ( NAND_Inform[g_NandContNum].u8BitECCErrBytePos[uIndex0] != 518 )
					{
						uTemp =	aSpareBuffer[(NAND_EccError.uEccErrByte2)-512] ;
						uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit2);
						aSpareBuffer[(NAND_EccError.uEccErrByte2)-512] = uTemp;				
					}
				}
				break;
			case 3:
				if ( NAND_Inform[g_NandContNum].u8BitECCErrBytePos[uIndex0] < 512 )
				{
					uTemp =	aBuffer[NAND_EccError.uEccErrByte3] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit3);
					aBuffer[NAND_EccError.uEccErrByte3] = uTemp;				
				}
				else
				{
					if ( NAND_Inform[g_NandContNum].u8BitECCErrBytePos[uIndex0] != 518)
					{
						uTemp =	aSpareBuffer[(NAND_EccError.uEccErrByte3)-512] ;
						uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit3);
						aSpareBuffer[(NAND_EccError.uEccErrByte3)-512] = uTemp;				
					}
				}
				break;
			case 4:
				if ( NAND_Inform[g_NandContNum].u8BitECCErrBytePos[uIndex0] < 512 )
				{
					uTemp =	aBuffer[NAND_EccError.uEccErrByte4] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit4);
					aBuffer[NAND_EccError.uEccErrByte4] = uTemp;				
				}
				else
				{
					if ( NAND_Inform[g_NandContNum].u8BitECCErrBytePos[uIndex0] !=518 )
					{
						uTemp =	aSpareBuffer[(NAND_EccError.uEccErrByte4)-512] ;
						uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit4);
						aSpareBuffer[(NAND_EccError.uEccErrByte4)-512] = uTemp;				
					}
				}
				break;
			case 5:
				if ( NAND_Inform[g_NandContNum].u8BitECCErrBytePos[uIndex0] < 512 )
				{
					uTemp =	aBuffer[NAND_EccError.uEccErrByte5] ;
					uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit5);
					aBuffer[NAND_EccError.uEccErrByte5] = uTemp;				
				}
				else
				{
					if ( NAND_Inform[g_NandContNum].u8BitECCErrBytePos[uIndex0] !=518)
					{
						uTemp =	aSpareBuffer[(NAND_EccError.uEccErrByte5)-512] ;
						uTemp = uTemp ^ ( 1<< NAND_EccError.uEccErrBit5);
						aSpareBuffer[(NAND_EccError.uEccErrByte5)-512] = uTemp;				
					}
				}
				break;
			default: 
				break;
		}
	}

	// 11. Write Data to NAND Flash
	UART_Printf("04. After invalid data create , write NAND flash\n");
	NAND_Inform[g_NandContNum].uECCtest = 1;
	eError = NAND_WritePage_4Bit_Test(g_NandContNum, uBlock, uPage, aBuffer, aSpareBuffer);
	if(eError != eNAND_NoError)
	{
		NANDT_PrintErrorType_Sample(eError);
		NAND_Inform[g_NandContNum].uECCtest = 0;
	}
	else
	{
#if 0
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
#endif		
		UART_Printf("NAND Invalid Data Write Page : Success\n");
	}

	UART_Printf("\n");

	for ( uIndex0 = 0 ; uIndex0 < 4 ; uIndex0++)
	{
		for ( uIndex1 = 0 ; uIndex1 < 2; uIndex1++)
		{
			NAND_Inform[g_NandContNum].u4BitECCCrrectTable[uIndex0][uIndex1] = 0;
		}

	}

/*
	// 12. Read Data from NAND flash
	UART_Printf("05. Read NAND flash\n");
	NAND_Inform[g_NandContNum].uECCtest = 1;
	eError = NAND_ReadPage_4Bit(g_NandContNum, uBlock, uPage, aBuffer, aSpareBuffer);
	if(eError != eNAND_NoError)
	{
		NANDT_PrintErrorType_Sample(eError);
		NAND_Inform[g_NandContNum].uECCtest = 0;
		NAND_Inform[g_NandContNum].uAllBitEccCheck = 0;
		UART_Printf("Read page Error [%d block  %d page]\n", uBlock, uPage);
		UART_Printf("-------------------------------------------\n");	
#if 1 
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
#endif		
	}
	else
	{
#if 0
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
#endif		
	}
	UART_Printf("\n");
*/
	
	NAND_Inform[g_NandContNum].uECCtest = 0;
	NAND_Inform[g_NandContNum].uAllBitEccCheck = 0;
	
	UART_Printf("SLC Nand 4bit ECC test complete & OK...!!\n");
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
	//u8 aBuffer[NAND_PAGE_MAX];
	//u8 aSpareBuffer[NAND_SPARE_MAX];
	u8 *aBuffer, *aSpareBuffer;
	NAND_eERROR eError;
	//u8 aReadBuffer[NAND_PAGE_MAX];
	u8 *aReadBuffer;
		
	UART_Printf("[NANDT_SLCECC_All]\n");

	if(NAND_Inform[g_NandContNum].uNandType == NAND_MLC8bit)
	{
		UART_Printf("The current Nand Memory is MLC type.....!!\n\n");
		return;
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
	aReadBuffer = (u8 *)malloc(NAND_PAGE_MAX);
	if(aReadBuffer == 0)
	{
		UART_Printf("Memory Allocation Error...\n");
		free(aBuffer);
		free(aSpareBuffer);
		UART_Getc();
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
	eError = NAND_EraseSingleBlock_Basic(g_NandContNum, uBlock);
	if(eError != eNAND_NoError)
	{
		NANDT_PrintErrorType(eError);
		NAND_Inform[g_NandContNum].uECCtest = 0;
		NAND_Inform[g_NandContNum].uAllBitEccCheck = 0;
		free(aBuffer);
		free(aSpareBuffer);
		free(aReadBuffer);
		return;
	}
		
	eError = NAND_WritePage(g_NandContNum, uBlock, 0, aBuffer, aSpareBuffer);	// write to 0th page
	if(eError != eNAND_NoError)
	{
		NANDT_PrintErrorType(eError);
		NAND_Inform[g_NandContNum].uECCtest = 0;
		NAND_Inform[g_NandContNum].uAllBitEccCheck = 0;
		free(aBuffer);
		free(aSpareBuffer);
		free(aReadBuffer);
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
			eError = NAND_EraseSingleBlock_Basic(g_NandContNum, uBlock);
			if(eError != eNAND_NoError)
			{
				NANDT_PrintErrorType(eError);
				NAND_Inform[g_NandContNum].uECCtest = 0;
				NAND_Inform[g_NandContNum].uAllBitEccCheck = 0;
				free(aBuffer);
				free(aSpareBuffer);
				free(aReadBuffer);
				return;
			}

			// Write the invalid data to page( uBlock[block] 1[page] )
			eError = NAND_WritePageWithInvalidData(g_NandContNum, uBlock, uPage, aBuffer, uErrorType);
			if(eError != eNAND_NoError)
			{
				NANDT_PrintErrorType(eError);
				NAND_Inform[g_NandContNum].uECCtest = 0;
				NAND_Inform[g_NandContNum].uAllBitEccCheck = 0;
				free(aBuffer);
				free(aSpareBuffer);
				free(aReadBuffer);
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
					free(aBuffer);
					free(aSpareBuffer);
					free(aReadBuffer);
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
					free(aBuffer);
					free(aSpareBuffer);
					free(aReadBuffer);
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

	free(aBuffer);
	free(aSpareBuffer);
	free(aReadBuffer);
		
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
	eError = NAND_EraseSingleBlock_Basic(g_NandContNum, uBlock);
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
			eError = NAND_EraseSingleBlock_Basic(g_NandContNum, uBlock);
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
	//u8 aBuffer[NAND_PAGE_MAX];
	//u8 aSpareBuffer[NAND_SPARE_MAX];
	//u8 aReadBuffer[NAND_PAGE_MAX];
	u8 *aBuffer;
	u8 *aSpareBuffer;
	u8 *aReadBuffer;
	NAND_eERROR eError;
	
	UART_Printf("[NANDT_MLCECC_All]\n");

	if(NAND_Inform[g_NandContNum].uNandType != NAND_MLC8bit)
	{
		UART_Printf("The current Nand Memory is not MLC type.....!!\n\n");
		return;
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
	aReadBuffer = (u8 *)malloc(NAND_PAGE_MAX);
	if(aReadBuffer == 0)
	{
		UART_Printf("Memory Allocation Error...\n");
		free(aBuffer);
		free(aSpareBuffer);
		UART_Getc();
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
	eError = NAND_EraseSingleBlock_Basic(g_NandContNum, uBlock);
	if(eError != eNAND_NoError)
	{
		NANDT_PrintErrorType(eError);
		NAND_Inform[g_NandContNum].uECCtest = 0;
		NAND_Inform[g_NandContNum].uAllBitEccCheck = 0;
		free(aBuffer);
		free(aSpareBuffer);
		free(aReadBuffer);
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
		free(aBuffer);
		free(aSpareBuffer);
		free(aReadBuffer);
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
				eError = NAND_EraseSingleBlock_Basic(g_NandContNum, uBlock);
				if(eError != eNAND_NoError)
				{
					NANDT_PrintErrorType(eError);
					NAND_Inform[g_NandContNum].uECCtest = 0;
					NAND_Inform[g_NandContNum].uAllBitEccCheck = 0;
					free(aBuffer);
					free(aSpareBuffer);
					free(aReadBuffer);
					return;
				}

				// Write the invalid data to page( uBlock[block] 1[page] )
				eError = NAND_WritePageWithInvalidData(g_NandContNum, uBlock, uPage, aBuffer, uErrorType);
				if(eError != eNAND_NoError)
				{
					NANDT_PrintErrorType(eError);
					NAND_Inform[g_NandContNum].uECCtest = 0;
					NAND_Inform[g_NandContNum].uAllBitEccCheck = 0;
					free(aBuffer);
					free(aSpareBuffer);
					free(aReadBuffer);
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
	
	free(aBuffer);
	free(aSpareBuffer);
	free(aReadBuffer);
					
	UART_Printf("MLC Nand ECC test complete\n");
	UART_Printf("\n");	
}
#define RAND_COUNT		(0x10000)
#define ECC_ERROR_BIT	(7)
u16 aRandomDataByte[RAND_COUNT][ECC_ERROR_BIT];
u8 aRandomDataBit[RAND_COUNT][ECC_ERROR_BIT];
int SortCompare(const u16 *a, const u16 *b)
{
	return (*a-*b);
}


void NANDT_RandomPosition(u32 rand_val)
{
	u32 i, j, k;
	u16 temp;
//	u16 same_cnt;
	
	srand(rand_val);
	for(i=0 ; i<RAND_COUNT ; i++)
	{
		for(j=0 ; j<ECC_ERROR_BIT ; j++)
		{
			aRandomDataByte[i][j] = rand()%(NAND_MLC_TRANS_SIZE+NAND_MLC_ECC_SIZE-1);
			aRandomDataBit[i][j] = rand()%8;
		}
	}
	
	for(i=0 ; i<RAND_COUNT ; i++)
	{
		qsort(aRandomDataByte[i], ECC_ERROR_BIT, sizeof(u16), SortCompare);
	}
	
	for(i=0 ; i<RAND_COUNT ; i++)
	{	
		for(k=0 ; k<ECC_ERROR_BIT ; k++)
		{
			for(j=0 ; j<ECC_ERROR_BIT ; j++)
			{
				if(aRandomDataByte[i][j] == aRandomDataByte[i][j+1])
				{
					if(aRandomDataBit[i][j] < aRandomDataBit[i][j+1])
					{
						temp = aRandomDataBit[i][j+1];
						aRandomDataBit[i][j+1] = aRandomDataBit[i][j];
						aRandomDataBit[i][j] = temp;
					}
					else if(aRandomDataBit[i][j]  == aRandomDataBit[i][j+1])
					{
						if(aRandomDataBit[i][j+1] == 7)
							aRandomDataBit[i][j+1] = 6;
						aRandomDataBit[i][j] = (aRandomDataBit[i][j+1] + 1)%8;
					}
				}
			}
		}
	}	
}



//////////
// Function Name : NANDT_MLCECC_Random
// Function Description : This function checks the ECC operation (random bit of MLC Nand Device 1page)
// Input : 	None
// Output : 	None
void NANDT_MLCECC_Random(void)
{
	u32 i, k, l, m, uBlock, uPage, uError, uErrorType;
	//u8 aBuffer[NAND_PAGE_MAX];
	//u8 aSpareBuffer[NAND_SPARE_MAX];
	//u8 aReadBuffer[NAND_PAGE_MAX];
	u8 *aBuffer;
	u8 *aSpareBuffer;
	u8 *aReadBuffer;
	NAND_eERROR eError;
	
	UART_Printf("[NANDT_MLCECC_Random]\n");

	if(NAND_Inform[g_NandContNum].uNandType != NAND_MLC8bit)
	{
		UART_Printf("The current Nand Memory is not MLC type.....!!\n\n");
		return;
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
	aReadBuffer = (u8 *)malloc(NAND_PAGE_MAX);
	if(aReadBuffer == 0)
	{
		UART_Printf("Memory Allocation Error...\n");
		free(aBuffer);
		free(aSpareBuffer);
		UART_Getc();
		return;
	}
	
	UART_Printf("Input the Block Number to test[0~%d]", NAND_Inform[g_NandContNum].uBlockNum-1);
	uBlock = UART_GetIntNum();
	UART_Printf("Input the Page Number to test[0~%d]", NAND_Inform[g_NandContNum].uPageNum-1);
	uPage = UART_GetIntNum();	
	
	UART_Printf("Input the ECC Error Type\n");
	UART_Printf("1: 1bit error(D),	2: 2bit error,	3: 3bit error,	4: 4bit error,	5: Uncorrectable(5bit),  6: Uncorrectable(6bit),  7: Uncorrectable(7bit)\n");
	uError = UART_GetIntNum();
	switch(uError)
	{
		case 1 :	uErrorType = NAND_ECCERR_1BIT;
				break;
		case 2 :	uErrorType = NAND_ECCERR_2BIT;
				break;
		case 3 :	uErrorType = NAND_ECCERR_3BIT;
				break;
		case 4 :	uErrorType = NAND_ECCERR_4BIT;
				break;
		case 5 :	uErrorType = NAND_ECCERR_UNCORRECT;
				break;
		case 6 :	uErrorType = NAND_ECCERR_UNCORRECT6BIT;
				break;
		case 7 :	uErrorType = NAND_ECCERR_UNCORRECT7BIT;
				break;
		default :	uErrorType = NAND_ECCERR_1BIT;
				break;				
	}
	uPage = 0;

	
	srand(1);
	for(i=0 ; i<NAND_Inform[g_NandContNum].uPageSize ; i++)
		aBuffer[i] = rand()%0xFF;
	for(i=0 ; i<NAND_Inform[g_NandContNum].uSpareSize ; i++)
		aSpareBuffer[i] = 0xFF;	

	UART_Printf("\n");
	UART_Printf("The all bit ecc check of %dBlock:%dPage\n", uBlock, uPage);
	UART_Printf("\n");

	NAND_Inform[g_NandContNum].uECCtest = 1;	
	NAND_Inform[g_NandContNum].uAllBitEccCheck = 1;
	
	// Erase Block for test
	eError = NAND_EraseSingleBlock(g_NandContNum, uBlock);
	if(eError != eNAND_NoError)
	{
		NANDT_PrintErrorType(eError);
		NAND_Inform[g_NandContNum].uECCtest = 0;
		NAND_Inform[g_NandContNum].uAllBitEccCheck = 0;
		free(aBuffer);
		free(aSpareBuffer);
		free(aReadBuffer);		
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
		free(aBuffer);
		free(aSpareBuffer);
		free(aReadBuffer);	
		return;
	}

	// Test Start......!!
	for(k=0 ; k<4 ; k++)
	{
		NAND_Inform[g_NandContNum].uMLCECCPageWriteSector = k;

		NANDT_RandomPosition(k);
				
		//for(j=0 ; j<(NAND_MLC_TRANS_SIZE+NAND_MLC_ECC_SIZE-uExceptCnt) ; j++)
		{
			for(i=0 ; i<RAND_COUNT ; i++)
			{
				if(uErrorType == NAND_ECCERR_1BIT)
				{	
					NAND_Inform[g_NandContNum].uMLCECCErrBytePos[0] = aRandomDataByte[i][0];

					aRandomDataByte[i][0] += NAND_Inform[g_NandContNum].uMLCECCPageWriteSector*NAND_MLC_ECC_SIZE
						+ NAND_Inform[g_NandContNum].uMLCECCPageWriteSector*NAND_MLC_TRANS_SIZE;
					
					NAND_EccError.uEccErrByte1 = aRandomDataByte[i][0];				
					NAND_EccError.uEccErrBit1 = aRandomDataBit[i][0];

					UART_Printf("Sector : %d [%d byte : %d bit]\n", k, aRandomDataByte[i][0], aRandomDataBit[i][0]);
				}
				else if(uErrorType == NAND_ECCERR_2BIT)
				{
					NAND_Inform[g_NandContNum].uMLCECCErrBytePos[0] = aRandomDataByte[i][0];
					NAND_Inform[g_NandContNum].uMLCECCErrBytePos[1] = aRandomDataByte[i][1];

					aRandomDataByte[i][0] += NAND_Inform[g_NandContNum].uMLCECCPageWriteSector*NAND_MLC_ECC_SIZE
						+ NAND_Inform[g_NandContNum].uMLCECCPageWriteSector*NAND_MLC_TRANS_SIZE;
					aRandomDataByte[i][1] += NAND_Inform[g_NandContNum].uMLCECCPageWriteSector*NAND_MLC_ECC_SIZE
						+ NAND_Inform[g_NandContNum].uMLCECCPageWriteSector*NAND_MLC_TRANS_SIZE;
					
					NAND_EccError.uEccErrByte1 = aRandomDataByte[i][0];
					NAND_EccError.uEccErrByte2 = aRandomDataByte[i][1];					
					NAND_EccError.uEccErrBit1 = aRandomDataBit[i][0];
					NAND_EccError.uEccErrBit2 = aRandomDataBit[i][1];
					
					UART_Printf("Sector : %d [%d byte : %d bit] & [%d byte : %d bit] \n", k,	aRandomDataByte[i][0], aRandomDataBit[i][0],
																	aRandomDataByte[i][1], aRandomDataBit[i][1]);
				}
				else if(uErrorType == NAND_ECCERR_3BIT)
				{
					NAND_Inform[g_NandContNum].uMLCECCErrBytePos[0] = aRandomDataByte[i][0];
					NAND_Inform[g_NandContNum].uMLCECCErrBytePos[1] = aRandomDataByte[i][1];
					NAND_Inform[g_NandContNum].uMLCECCErrBytePos[2] = aRandomDataByte[i][2];

					aRandomDataByte[i][0] += NAND_Inform[g_NandContNum].uMLCECCPageWriteSector*NAND_MLC_ECC_SIZE
						+ NAND_Inform[g_NandContNum].uMLCECCPageWriteSector*NAND_MLC_TRANS_SIZE;
					aRandomDataByte[i][1] += NAND_Inform[g_NandContNum].uMLCECCPageWriteSector*NAND_MLC_ECC_SIZE
						+ NAND_Inform[g_NandContNum].uMLCECCPageWriteSector*NAND_MLC_TRANS_SIZE;
					aRandomDataByte[i][2] += NAND_Inform[g_NandContNum].uMLCECCPageWriteSector*NAND_MLC_ECC_SIZE
						+ NAND_Inform[g_NandContNum].uMLCECCPageWriteSector*NAND_MLC_TRANS_SIZE;
					
					NAND_EccError.uEccErrByte1 = aRandomDataByte[i][0];
					NAND_EccError.uEccErrByte2 = aRandomDataByte[i][1];
					NAND_EccError.uEccErrByte3 = aRandomDataByte[i][2];					
					NAND_EccError.uEccErrBit1 = aRandomDataBit[i][0];
					NAND_EccError.uEccErrBit2 = aRandomDataBit[i][1];
					NAND_EccError.uEccErrBit3 = aRandomDataBit[i][2];
					
					
					UART_Printf("Sector : %d [%d byte : %d bit] & [%d byte : %d bit] &  [%d byte : %d bit]\n", 	k,
																	aRandomDataByte[i][0], aRandomDataBit[i][0],
																	aRandomDataByte[i][1], aRandomDataBit[i][1],
																	aRandomDataByte[i][2], aRandomDataBit[i][2]);
				}
				else if(uErrorType == NAND_ECCERR_4BIT)
				{
					NAND_Inform[g_NandContNum].uMLCECCErrBytePos[0] = aRandomDataByte[i][0];
					NAND_Inform[g_NandContNum].uMLCECCErrBytePos[1] = aRandomDataByte[i][1];
					NAND_Inform[g_NandContNum].uMLCECCErrBytePos[2] = aRandomDataByte[i][2];
					NAND_Inform[g_NandContNum].uMLCECCErrBytePos[3] = aRandomDataByte[i][3];

					aRandomDataByte[i][0] += NAND_Inform[g_NandContNum].uMLCECCPageWriteSector*NAND_MLC_ECC_SIZE
						+ NAND_Inform[g_NandContNum].uMLCECCPageWriteSector*NAND_MLC_TRANS_SIZE;
					aRandomDataByte[i][1] += NAND_Inform[g_NandContNum].uMLCECCPageWriteSector*NAND_MLC_ECC_SIZE
						+ NAND_Inform[g_NandContNum].uMLCECCPageWriteSector*NAND_MLC_TRANS_SIZE;
					aRandomDataByte[i][2] += NAND_Inform[g_NandContNum].uMLCECCPageWriteSector*NAND_MLC_ECC_SIZE
						+ NAND_Inform[g_NandContNum].uMLCECCPageWriteSector*NAND_MLC_TRANS_SIZE;
					aRandomDataByte[i][3] += NAND_Inform[g_NandContNum].uMLCECCPageWriteSector*NAND_MLC_ECC_SIZE
						+ NAND_Inform[g_NandContNum].uMLCECCPageWriteSector*NAND_MLC_TRANS_SIZE;

					NAND_EccError.uEccErrByte1 = aRandomDataByte[i][0];
					NAND_EccError.uEccErrByte2 = aRandomDataByte[i][1];
					NAND_EccError.uEccErrByte3 = aRandomDataByte[i][2];
					NAND_EccError.uEccErrByte4 = aRandomDataByte[i][3];					
					NAND_EccError.uEccErrBit1 = aRandomDataBit[i][0];
					NAND_EccError.uEccErrBit2 = aRandomDataBit[i][1];
					NAND_EccError.uEccErrBit3 = aRandomDataBit[i][2];
					NAND_EccError.uEccErrBit4 = aRandomDataBit[i][3];
				
					UART_Printf("Sector : %d [%d byte : %d bit] & [%d byte : %d bit] &  [%d byte : %d bit] &  [%d byte : %d bit]\n", k,	
																	aRandomDataByte[i][0], aRandomDataBit[i][0],
																	aRandomDataByte[i][1], aRandomDataBit[i][1],
																	aRandomDataByte[i][2], aRandomDataBit[i][2],
																	aRandomDataByte[i][3], aRandomDataBit[i][3]);
				}
				else if(uErrorType == NAND_ECCERR_UNCORRECT)
				{
					NAND_Inform[g_NandContNum].uMLCECCErrBytePos[0] = aRandomDataByte[i][0];
					NAND_Inform[g_NandContNum].uMLCECCErrBytePos[1] = aRandomDataByte[i][1];
					NAND_Inform[g_NandContNum].uMLCECCErrBytePos[2] = aRandomDataByte[i][2];
					NAND_Inform[g_NandContNum].uMLCECCErrBytePos[3] = aRandomDataByte[i][3];
					NAND_Inform[g_NandContNum].uMLCECCErrBytePos[4] = aRandomDataByte[i][4];

					aRandomDataByte[i][0] += NAND_Inform[g_NandContNum].uMLCECCPageWriteSector*NAND_MLC_ECC_SIZE
						+ NAND_Inform[g_NandContNum].uMLCECCPageWriteSector*NAND_MLC_TRANS_SIZE;
					aRandomDataByte[i][1] += NAND_Inform[g_NandContNum].uMLCECCPageWriteSector*NAND_MLC_ECC_SIZE
						+ NAND_Inform[g_NandContNum].uMLCECCPageWriteSector*NAND_MLC_TRANS_SIZE;
					aRandomDataByte[i][2] += NAND_Inform[g_NandContNum].uMLCECCPageWriteSector*NAND_MLC_ECC_SIZE
						+ NAND_Inform[g_NandContNum].uMLCECCPageWriteSector*NAND_MLC_TRANS_SIZE;
					aRandomDataByte[i][3] += NAND_Inform[g_NandContNum].uMLCECCPageWriteSector*NAND_MLC_ECC_SIZE
						+ NAND_Inform[g_NandContNum].uMLCECCPageWriteSector*NAND_MLC_TRANS_SIZE;
					aRandomDataByte[i][4] += NAND_Inform[g_NandContNum].uMLCECCPageWriteSector*NAND_MLC_ECC_SIZE
						+ NAND_Inform[g_NandContNum].uMLCECCPageWriteSector*NAND_MLC_TRANS_SIZE;
					
					NAND_EccError.uEccErrByte1 = aRandomDataByte[i][0];
					NAND_EccError.uEccErrByte2 = aRandomDataByte[i][1];
					NAND_EccError.uEccErrByte3 = aRandomDataByte[i][2];
					NAND_EccError.uEccErrByte4 = aRandomDataByte[i][3];
					NAND_EccError.uEccErrByte5 = aRandomDataByte[i][4];					
					NAND_EccError.uEccErrBit1 = aRandomDataBit[i][0];
					NAND_EccError.uEccErrBit2 = aRandomDataBit[i][1];
					NAND_EccError.uEccErrBit3 = aRandomDataBit[i][2];
					NAND_EccError.uEccErrBit4 = aRandomDataBit[i][3];
					NAND_EccError.uEccErrBit5 = aRandomDataBit[i][4];
					
					UART_Printf("Sector : %d [%d byte : %d bit] & [%d byte : %d bit] &  [%d byte : %d bit] & [%d byte : %d bit] &  [%d byte : %d bit]\n", 	k,
																	aRandomDataByte[i][0], aRandomDataBit[i][0],
																	aRandomDataByte[i][1], aRandomDataBit[i][1],
																	aRandomDataByte[i][2], aRandomDataBit[i][2],
																	aRandomDataByte[i][3], aRandomDataBit[i][3],
																	aRandomDataByte[i][4], aRandomDataBit[i][4]);
				}
				else if(uErrorType == NAND_ECCERR_UNCORRECT6BIT)
				{
					NAND_Inform[g_NandContNum].uMLCECCErrBytePos[0] = aRandomDataByte[i][0];
					NAND_Inform[g_NandContNum].uMLCECCErrBytePos[1] = aRandomDataByte[i][1];
					NAND_Inform[g_NandContNum].uMLCECCErrBytePos[2] = aRandomDataByte[i][2];
					NAND_Inform[g_NandContNum].uMLCECCErrBytePos[3] = aRandomDataByte[i][3];
					NAND_Inform[g_NandContNum].uMLCECCErrBytePos[4] = aRandomDataByte[i][4];
					NAND_Inform[g_NandContNum].uMLCECCErrBytePos[5] = aRandomDataByte[i][5];

					aRandomDataByte[i][0] += NAND_Inform[g_NandContNum].uMLCECCPageWriteSector*NAND_MLC_ECC_SIZE
						+ NAND_Inform[g_NandContNum].uMLCECCPageWriteSector*NAND_MLC_TRANS_SIZE;
					aRandomDataByte[i][1] += NAND_Inform[g_NandContNum].uMLCECCPageWriteSector*NAND_MLC_ECC_SIZE
						+ NAND_Inform[g_NandContNum].uMLCECCPageWriteSector*NAND_MLC_TRANS_SIZE;
					aRandomDataByte[i][2] += NAND_Inform[g_NandContNum].uMLCECCPageWriteSector*NAND_MLC_ECC_SIZE
						+ NAND_Inform[g_NandContNum].uMLCECCPageWriteSector*NAND_MLC_TRANS_SIZE;
					aRandomDataByte[i][3] += NAND_Inform[g_NandContNum].uMLCECCPageWriteSector*NAND_MLC_ECC_SIZE
						+ NAND_Inform[g_NandContNum].uMLCECCPageWriteSector*NAND_MLC_TRANS_SIZE;
					aRandomDataByte[i][4] += NAND_Inform[g_NandContNum].uMLCECCPageWriteSector*NAND_MLC_ECC_SIZE
						+ NAND_Inform[g_NandContNum].uMLCECCPageWriteSector*NAND_MLC_TRANS_SIZE;
					aRandomDataByte[i][5] += NAND_Inform[g_NandContNum].uMLCECCPageWriteSector*NAND_MLC_ECC_SIZE
						+ NAND_Inform[g_NandContNum].uMLCECCPageWriteSector*NAND_MLC_TRANS_SIZE;
					
					NAND_EccError.uEccErrByte1 = aRandomDataByte[i][0];
					NAND_EccError.uEccErrByte2 = aRandomDataByte[i][1];
					NAND_EccError.uEccErrByte3 = aRandomDataByte[i][2];
					NAND_EccError.uEccErrByte4 = aRandomDataByte[i][3];
					NAND_EccError.uEccErrByte5 = aRandomDataByte[i][4];
					NAND_EccError.uEccErrByte6 = aRandomDataByte[i][5];
					NAND_EccError.uEccErrBit1 = aRandomDataBit[i][0];
					NAND_EccError.uEccErrBit2 = aRandomDataBit[i][1];
					NAND_EccError.uEccErrBit3 = aRandomDataBit[i][2];
					NAND_EccError.uEccErrBit4 = aRandomDataBit[i][3];
					NAND_EccError.uEccErrBit5 = aRandomDataBit[i][4];
					NAND_EccError.uEccErrBit6 = aRandomDataBit[i][5];
					
					UART_Printf("Sector : %d [%d byte : %d bit] & [%d byte : %d bit] & [%d byte : %d bit] & [%d byte : %d bit] & [%d byte : %d bit] & [%d byte : %d bit]\n", 	k,
																	aRandomDataByte[i][0], aRandomDataBit[i][0],
																	aRandomDataByte[i][1], aRandomDataBit[i][1],
																	aRandomDataByte[i][2], aRandomDataBit[i][2],
																	aRandomDataByte[i][3], aRandomDataBit[i][3],
																	aRandomDataByte[i][4], aRandomDataBit[i][4],
																	aRandomDataByte[i][5], aRandomDataBit[i][5]);
				}
				else //if(uErrorType == NAND_ECCERR_UNCORRECT7BIT)
				{
					NAND_Inform[g_NandContNum].uMLCECCErrBytePos[0] = aRandomDataByte[i][0];
					NAND_Inform[g_NandContNum].uMLCECCErrBytePos[1] = aRandomDataByte[i][1];
					NAND_Inform[g_NandContNum].uMLCECCErrBytePos[2] = aRandomDataByte[i][2];
					NAND_Inform[g_NandContNum].uMLCECCErrBytePos[3] = aRandomDataByte[i][3];
					NAND_Inform[g_NandContNum].uMLCECCErrBytePos[4] = aRandomDataByte[i][4];
					NAND_Inform[g_NandContNum].uMLCECCErrBytePos[5] = aRandomDataByte[i][5];
					NAND_Inform[g_NandContNum].uMLCECCErrBytePos[6] = aRandomDataByte[i][6];

					aRandomDataByte[i][0] += NAND_Inform[g_NandContNum].uMLCECCPageWriteSector*NAND_MLC_ECC_SIZE
						+ NAND_Inform[g_NandContNum].uMLCECCPageWriteSector*NAND_MLC_TRANS_SIZE;
					aRandomDataByte[i][1] += NAND_Inform[g_NandContNum].uMLCECCPageWriteSector*NAND_MLC_ECC_SIZE
						+ NAND_Inform[g_NandContNum].uMLCECCPageWriteSector*NAND_MLC_TRANS_SIZE;
					aRandomDataByte[i][2] += NAND_Inform[g_NandContNum].uMLCECCPageWriteSector*NAND_MLC_ECC_SIZE
						+ NAND_Inform[g_NandContNum].uMLCECCPageWriteSector*NAND_MLC_TRANS_SIZE;
					aRandomDataByte[i][3] += NAND_Inform[g_NandContNum].uMLCECCPageWriteSector*NAND_MLC_ECC_SIZE
						+ NAND_Inform[g_NandContNum].uMLCECCPageWriteSector*NAND_MLC_TRANS_SIZE;
					aRandomDataByte[i][4] += NAND_Inform[g_NandContNum].uMLCECCPageWriteSector*NAND_MLC_ECC_SIZE
						+ NAND_Inform[g_NandContNum].uMLCECCPageWriteSector*NAND_MLC_TRANS_SIZE;
					aRandomDataByte[i][5] += NAND_Inform[g_NandContNum].uMLCECCPageWriteSector*NAND_MLC_ECC_SIZE
						+ NAND_Inform[g_NandContNum].uMLCECCPageWriteSector*NAND_MLC_TRANS_SIZE;
					aRandomDataByte[i][6] += NAND_Inform[g_NandContNum].uMLCECCPageWriteSector*NAND_MLC_ECC_SIZE
						+ NAND_Inform[g_NandContNum].uMLCECCPageWriteSector*NAND_MLC_TRANS_SIZE;
					
					NAND_EccError.uEccErrByte1 = aRandomDataByte[i][0];
					NAND_EccError.uEccErrByte2 = aRandomDataByte[i][1];
					NAND_EccError.uEccErrByte3 = aRandomDataByte[i][2];
					NAND_EccError.uEccErrByte4 = aRandomDataByte[i][3];
					NAND_EccError.uEccErrByte5 = aRandomDataByte[i][4];
					NAND_EccError.uEccErrByte6 = aRandomDataByte[i][5];
					NAND_EccError.uEccErrByte7 = aRandomDataByte[i][6];
					NAND_EccError.uEccErrBit1 = aRandomDataBit[i][0];
					NAND_EccError.uEccErrBit2 = aRandomDataBit[i][1];
					NAND_EccError.uEccErrBit3 = aRandomDataBit[i][2];
					NAND_EccError.uEccErrBit4 = aRandomDataBit[i][3];
					NAND_EccError.uEccErrBit5 = aRandomDataBit[i][4];
					NAND_EccError.uEccErrBit6 = aRandomDataBit[i][5];
					NAND_EccError.uEccErrBit7 = aRandomDataBit[i][6];
					
					UART_Printf("Sector : %d [%d byte : %d bit] & [%d byte : %d bit] & [%d byte : %d bit] & [%d byte : %d bit] & [%d byte : %d bit] & [%d byte : %d bit] & [%d byte : %d bit]\n", 	k,
																	aRandomDataByte[i][0], aRandomDataBit[i][0],
																	aRandomDataByte[i][1], aRandomDataBit[i][1],
																	aRandomDataByte[i][2], aRandomDataBit[i][2],
																	aRandomDataByte[i][3], aRandomDataBit[i][3],
																	aRandomDataByte[i][4], aRandomDataBit[i][4],
																	aRandomDataByte[i][5], aRandomDataBit[i][5],
																	aRandomDataByte[i][6], aRandomDataBit[i][6]);
				}

					
				// Erase Block for test
				eError = NAND_EraseSingleBlock(g_NandContNum, uBlock);
				if(eError != eNAND_NoError)
				{
					NANDT_PrintErrorType(eError);
					NAND_Inform[g_NandContNum].uECCtest = 0;
					NAND_Inform[g_NandContNum].uAllBitEccCheck = 0;
					free(aBuffer);
					free(aSpareBuffer);
					free(aReadBuffer);	
					return;
				}

				// Write the invalid data to page( uBlock[block] 1[page] )
				eError = NAND_WritePageWithInvalidData(g_NandContNum, uBlock, uPage, aBuffer, uErrorType);
				if(eError != eNAND_NoError)
				{
					NANDT_PrintErrorType(eError);
					NAND_Inform[g_NandContNum].uECCtest = 0;
					NAND_Inform[g_NandContNum].uAllBitEccCheck = 0;
					free(aBuffer);
					free(aSpareBuffer);
					free(aReadBuffer);	
					return;
				}
					
				// Read the invalid data to detect the ECC error	
				eError = NAND_ReadPage(g_NandContNum, uBlock, uPage, aReadBuffer, aSpareBuffer);

				NANDT_PrintErrorType(eError);
				if(uErrorType == NAND_ECCERR_1BIT)
				{
					if( (aRandomDataByte[i][0] != aNANDT_EccError[0][0]) || (aRandomDataBit[i][0] != aNANDT_EccError[0][1]) )
					{
						UART_Printf("Ecc Error Check Error[%d byte:%d bit] != [%d byte:%d bit] \n", aRandomDataByte[i][0], aRandomDataBit[i][0],
																					aNANDT_EccError[0][0], aNANDT_EccError[0][1]);	
						UART_Getc();
						//NAND_Inform[g_NandContNum].uECCtest = 0;
						//NAND_Inform[g_NandContNum].uAllBitEccCheck = 0;
						//return;
					}
				}
				else if(uErrorType == NAND_ECCERR_2BIT)
				{
					if( 	((aRandomDataByte[i][0] != aNANDT_EccError[0][0]) || (aRandomDataBit[i][0] != aNANDT_EccError[0][1])) ||
						((aRandomDataByte[i][1] != aNANDT_EccError[1][0]) || (aRandomDataBit[i][1] != aNANDT_EccError[1][1])) )
					{
						UART_Printf("Ecc Error Check Error[%d byte:%d bit] != [%d byte:%d bit] \n", aRandomDataByte[i][0], aRandomDataBit[i][0],
																					aNANDT_EccError[0][0], aNANDT_EccError[0][1]);	
						UART_Printf("                     [%d byte:%d bit] != [%d byte:%d bit] \n", 	aRandomDataByte[i][1], aRandomDataBit[i][1],
																				aNANDT_EccError[1][0], aNANDT_EccError[1][1]);						
						UART_Getc();
						//NAND_Inform[g_NandContNum].uECCtest = 0;
						//NAND_Inform[g_NandContNum].uAllBitEccCheck = 0;
						//return;
					}
				}
				else if(uErrorType == NAND_ECCERR_3BIT)
				{
					if( 	((aRandomDataByte[i][0] != aNANDT_EccError[0][0]) || (aRandomDataBit[i][0] != aNANDT_EccError[0][1])) ||
						((aRandomDataByte[i][1] != aNANDT_EccError[1][0]) || (aRandomDataBit[i][1] != aNANDT_EccError[1][1])) ||
						((aRandomDataByte[i][2] != aNANDT_EccError[2][0]) || (aRandomDataBit[i][2] != aNANDT_EccError[2][1])) )
					{
						UART_Printf("Ecc Error Check Error[%d byte:%d bit] != [%d byte:%d bit] \n", aRandomDataByte[i][0], aRandomDataBit[i][0],
																					aNANDT_EccError[0][0], aNANDT_EccError[0][1]);	
						UART_Printf("                     [%d byte:%d bit] != [%d byte:%d bit] \n", 	aRandomDataByte[i][1], aRandomDataBit[i][1],
																				aNANDT_EccError[1][0], aNANDT_EccError[1][1]);
						UART_Printf("                     [%d byte:%d bit] != [%d byte:%d bit] \n", 	aRandomDataByte[i][2], aRandomDataBit[i][2],
																				aNANDT_EccError[2][0], aNANDT_EccError[2][1]);
						UART_Getc();
						//NAND_Inform[g_NandContNum].uECCtest = 0;
						//NAND_Inform[g_NandContNum].uAllBitEccCheck = 0;
						//return;
					}
				}
				else if(uErrorType == NAND_ECCERR_4BIT)
				{
					if( 	((aRandomDataByte[i][0] != aNANDT_EccError[0][0]) || (aRandomDataBit[i][0] != aNANDT_EccError[0][1])) ||
						((aRandomDataByte[i][1] != aNANDT_EccError[1][0]) || (aRandomDataBit[i][1] != aNANDT_EccError[1][1])) ||
						((aRandomDataByte[i][2] != aNANDT_EccError[2][0]) || (aRandomDataBit[i][2] != aNANDT_EccError[2][1])) ||
						((aRandomDataByte[i][3] != aNANDT_EccError[3][0]) || (aRandomDataBit[i][3] != aNANDT_EccError[3][1])) )
					{
						UART_Printf("Ecc Error Check Error[%d byte:%d bit] != [%d byte:%d bit] \n", aRandomDataByte[i][0], aRandomDataBit[i][0],
																					aNANDT_EccError[0][0], aNANDT_EccError[0][1]);	
						UART_Printf("                     [%d byte:%d bit] != [%d byte:%d bit] \n", 	aRandomDataByte[i][1], aRandomDataBit[i][1],
																				aNANDT_EccError[1][0], aNANDT_EccError[1][1]);
						UART_Printf("                     [%d byte:%d bit] != [%d byte:%d bit] \n", 	aRandomDataByte[i][2], aRandomDataBit[i][2],
																				aNANDT_EccError[2][0], aNANDT_EccError[2][1]);
						UART_Printf("                     [%d byte:%d bit] != [%d byte:%d bit] \n", 	aRandomDataByte[i][3], aRandomDataBit[i][3],
																				aNANDT_EccError[3][0], aNANDT_EccError[3][1]);
						UART_Getc();
						//NAND_Inform[g_NandContNum].uECCtest = 0;
						//NAND_Inform[g_NandContNum].uAllBitEccCheck = 0;
						//return;
					}
				}
				else //if(uErrorType == NAND_ECCERR_UNCORRECT || uErrorType == NAND_ECCERR_UNCORRECT6BIT || uErrorType == NAND_ECCERR_UNCORRECT7BIT)
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
					
			}
		}
	}

	NAND_Inform[g_NandContNum].uECCtest = 0;
	NAND_Inform[g_NandContNum].uAllBitEccCheck = 0;

	free(aBuffer);
	free(aSpareBuffer);
	free(aReadBuffer);	
	
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
        NANDT_MLCECC_Random, 		"MLC Random bit ECC          \n",
	NANDT_8BitECC_SLC,                      "[Small page nand flash] 8BitECC Basic Test       ",
	NANDT_8BitECC_SLC_24Byte,         "[Small page nand flash] 8BitECC Basic Test(24Byte)     ",
	NANDT_8BitECC_SLC_8Bit,              "[Small page nand flash] 8BitECC 8bit Test    ",
	NANDT_8BitECC_SLC_AllPattern,    "[Small page nand flash] 8BitECC All Bit Pattern Test      ",
	NANDT_8BitECC_SLC_Uncorrectable, "[Small page nand flash] 8BitECC Uncorrectable Error Test     \n",	
	NANDT_1BitECC_SLC,                      "[Small page nand flash] 1BitECC Basic ECC Test      ",
	NANDT_1BitECC_SLC_All,                "[Small page nand flash] 1BitECC All Bit Test       \n",
	NANDT_4BitECC_SLC,                      "[Small page nand flash] 4BitECC Basic ECC Test      ",
	NANDT_4BitECC_SLC_AllPattern,    "[Small page nand flash] 4BitECC All Bit Pattern Test     ",
	NANDT_4BitECC_SLC_Test,             "[Small page nand flash] 4BitECC WorkAround Test      \n",
	NANDT_8BitECC_2048,                     "[2048Page] 8BitECC Basic Test",
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
	u32 i, j, k, uNumClock; 	//, uNumOndMode;
	u32 uStartTestBlock, uEndTestBlock;
	oFunctionT_AutoVar oAutoTest;
						   //{Sync Mode, APLL, MPLL, HCLKx2 divide, HCLK divide, PCLK divide}
	u32 aCaseOfClock[][6] = { //Sync Mode
						   	//Async Mode
#if 1
							{eSYNC_MODE, (APLL_eOUT)eAPLL400M, (APLL_eOUT)eAPLL200M, 2, 2, 8}, 
							{eSYNC_MODE, (APLL_eOUT)eAPLL400M, (APLL_eOUT)eAPLL200M, 4, 2, 4},

							{eSYNC_MODE, (APLL_eOUT)eAPLL532M, (APLL_eOUT)eAPLL266M, 2, 2, 8},
							{eSYNC_MODE, (APLL_eOUT)eAPLL532M, (APLL_eOUT)eAPLL266M, 4, 2, 4},
#endif



							{eASYNC_MODE, (APLL_eOUT)eAPLL400M, (APLL_eOUT)eAPLL400M, 2, 2, 8},
							{eASYNC_MODE, (APLL_eOUT)eAPLL400M, (APLL_eOUT)eAPLL400M, 4, 2, 4},

							{eASYNC_MODE, (APLL_eOUT)eAPLL400M, (APLL_eOUT)eAPLL200M, 1, 2, 8},
							{eASYNC_MODE, (APLL_eOUT)eAPLL400M, (APLL_eOUT)eAPLL200M, 2, 2, 4},

							{eASYNC_MODE, (APLL_eOUT)eAPLL400M, (APLL_eOUT)eAPLL266M, 1, 2, 8},
							{eASYNC_MODE, (APLL_eOUT)eAPLL400M, (APLL_eOUT)eAPLL266M, 2, 2, 4},

							{eASYNC_MODE, (APLL_eOUT)eAPLL400M, (APLL_eOUT)eAPLL532M, 2, 2, 8},
							{eASYNC_MODE, (APLL_eOUT)eAPLL400M, (APLL_eOUT)eAPLL532M, 4, 2, 4},

							{eASYNC_MODE, (APLL_eOUT)eAPLL532M, (APLL_eOUT)eAPLL400M, 2, 2, 8},
							{eASYNC_MODE, (APLL_eOUT)eAPLL532M, (APLL_eOUT)eAPLL400M, 4, 2, 4},

							{eASYNC_MODE, (APLL_eOUT)eAPLL532M, (APLL_eOUT)eAPLL200M, 1, 2, 8},
							{eASYNC_MODE, (APLL_eOUT)eAPLL532M, (APLL_eOUT)eAPLL200M, 2, 2, 4},

							{eASYNC_MODE, (APLL_eOUT)eAPLL532M, (APLL_eOUT)eAPLL532M, 2, 2, 8},
							{eASYNC_MODE, (APLL_eOUT)eAPLL532M, (APLL_eOUT)eAPLL532M, 4, 2, 4},
	
							{eASYNC_MODE, (APLL_eOUT)eAPLL532M, (APLL_eOUT)eAPLL266M, 1, 2, 8},
							{eASYNC_MODE, (APLL_eOUT)eAPLL532M, (APLL_eOUT)eAPLL266M, 2, 2, 4}



							
	};
							
	uStartTestBlock = 100;
	uEndTestBlock = 119;
	
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
		UART_Printf("Write&Read Verify....Completed [%d ~ %d]\n", uStartTestBlock, uEndTestBlock);

#if 1
		UART_Printf("Soft-lock Test\n");
		oAutoTest.Test_Parameter[0] = uStartTestBlock;		
		oAutoTest.Test_Parameter[1] = uEndTestBlock;			
		NANDT_SoftLock(eTest_Auto, oAutoTest);
		UART_Printf("Soft-lock Test....Completed [%d ~ %d]\n", uStartTestBlock, uEndTestBlock);

		UART_Printf("Unlock the Soft-lock Block\n");
		oAutoTest.Test_Parameter[0] = uStartTestBlock;		
		oAutoTest.Test_Parameter[1] = uEndTestBlock;	
		NANDT_DisableSoftLock(eTest_Auto, oAutoTest);
		UART_Printf("Unlock the Soft-lock block....Completed [%d ~ %d]\n", uStartTestBlock, uEndTestBlock);
#endif
		//rb1004
		//lock-tight auto-test is omitted because this test is need to wake-up from sleep mode.
#if 1		
		UART_Printf("Write Performance\n");
		oAutoTest.Test_Parameter[0] = uStartTestBlock;	//Strat Block
		oAutoTest.Test_Parameter[1] = 5;				//Test Size[MBytes]
		NANDT_WritePeformance(eTest_Auto, oAutoTest);

		UART_Printf("Read Performance\n");
		oAutoTest.Test_Parameter[0] = uStartTestBlock;	//Strat Block
		oAutoTest.Test_Parameter[1] = 5;				//Test Size[MBytes]
		NANDT_ReadPeformance(eTest_Auto, oAutoTest);
#endif
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



#if 0 // 6430 FPGA Sleep Wake UP Test . . .. . 
void NANDT_SLEEPWakeup(void)
{
	u32 uTemp;
	
	UART_Printf("-------------------------------------\n\n\n");
	UART_Printf("[Sleep mode Entered]\n");
	uTemp = NFCONInp32((unsigned volatile *)0x9e005804); // wake up source is RTC Tick
	uTemp = uTemp & ~ (0x1<<11);
	NFCONOutp32((unsigned volatile *)0x9e005804, uTemp);
//	*(unsigned volatile *)0x9e005804 = ;
	*(unsigned volatile *)0x9e005800 = 0x4; // Enter Sleep mode
	while(1);
//	UART_Printf("-------------------------------------\n\n\n");

}
#endif

void NANDT_8KStepStone_ReadWrite(void)
{
	u32 uTemp0, uTemp1, uTemp2;
	u32 u8Temp0, u8Temp1;
	
	// 02. Write NAND 0 th block 
//	NANDT_WritePage(eFunction_Test eTest, oFunctionT_AutoVar oAutoVar)


	// 03. Enter Sleep Mode / Stop Mode / Deep-Stop Mode
	UART_Printf("Enter Power mode : 0 - Sleep Mode, 1 - Stop Mode , 2 - Deep Stop Mode\n");
	uTemp0 = UART_GetIntNum();
	switch ( uTemp0 )
	{
		case 0 : // Sleep mode
			SLEEPT_Basic();
			break;
		case 1: // Stop mode
			STOPT_Basic();
			break;
		case 2:
			DSTOPT_Basic();
			break;
		default : 
			break;
	}

	// Stop mode 
	UART_Printf("Check.... check.. \n");
	
	
}

#if 0 // zeus 071212
void ECCFailCount(void) {
	if(globalNandECCfailureCount==0x3c6400E4)
	{
		while(1)
		{
			Err_Disp(1);
		}
	}
	globalNandECCfailureCount=globalNandECCfailureCount+1;
	run();
}


void Err_Disp(int error)
{
	int j, err_type=error; // 1: nand, 0: secure
	
	rGPNCON=rGPNCON&~(0x3<<30)|(0x1<<30);
	
	if (err_type==1)  // Duty ratio 'H'=90%, 'L'=10%
	{
		rGPNDAT|=(1<<15);
		for(j=0; j<900; j++);
		rGPNDAT&=~(1<<15);
		for(j=0; j<100; j++);
		
	}
	else if (err_type==0)	// Duty ratio 'H'=10%, 'L'=90%
	{
		rGPNDAT|=(1<<15);
		for(j=0; j<100; j++);
		rGPNDAT&=~(1<<15);
		for(j=0; j<900; j++);
		
	}
	
}

#endif

void Nand_Reset(void)
{
	NF_nFCE_L2();

	NF_CLEAR_RB2();
	NF_CMD2(0xFF);	//reset command
	NF_DETECT_RB2();
	
	NF_nFCE_H2();

}

void NF8_Init_Adv(void)
{

	rNFCONF = (1<<24)|(7<<12)|(7<<8)|(7<<4)|(1<<3)|(1<<2)|(0<<0);	
	rNFCONT = (0<<17)|(0<<16)|(0<<10)|(0<<9)|(0<<8)|(1<<7)|(1<<6)|(1<<5)|(1<<4)|(0x3<<1)|(1<<0);

//	Nand_Reset();
}

void NF8_Init(void)
{
	rNFCONF = (7<<12)|(7<<8)|(7<<4)|(0<<3)|(1<<2)|(1<<1)|(0<<0);	
	rNFCONT = (0<<18)|(0<<17)|(0<<16)|(0<<10)|(0<<9)|(0<<8)|(0<<7)|(0<<6)|(0x3<<1)|(1<<0);
}

int CorrectECC8Data2(unsigned char *pEncodingBaseAddr)
{	
	unsigned int i,uErrorByte[9];//u32 uErrorByte1, uErrorByte2, uErrorByte3, uErrorByte4, uErrorByte5, uErrorByte6, uErrorByte7, uErrorByte8;	
	unsigned char uErrorBit[9];//u32 uErrorBit1, uErrorBit2, uErrorBit3, uErrorBit4, uErrorBit5, uErrorBit6, uErrorBit7, uErrorBit8;
	unsigned int uErrorType;
	
	// Searching Error Type //몇개냐
	uErrorType = (rNF8ECCERR0>>25)&0xf;
	// Searching Error Byte //몇번째 바이트냐
	uErrorByte[1] = rNF8ECCERR0&0x3ff;
	uErrorByte[2] = (rNF8ECCERR0>>15)&0x3ff;
	
	uErrorByte[3] = (rNF8ECCERR1)&0x3ff;
	uErrorByte[4] = (rNF8ECCERR1>>11)&0x3ff;	
	uErrorByte[5] = (rNF8ECCERR1>>22)&0x3ff;
	
	uErrorByte[6] = (rNF8ECCERR2)&0x3ff;
	uErrorByte[7] = (rNF8ECCERR2>>11)&0x3ff;
	uErrorByte[8] = (rNF8ECCERR2>>22)&0x3ff;
	// Searching Error Bit //
	uErrorBit[1] = rNFMLC8BITPT0&0xff;
	uErrorBit[2] = (rNFMLC8BITPT0>>8)&0xff;
	uErrorBit[3] = (rNFMLC8BITPT0>>16)&0xff;
	uErrorBit[4] = (rNFMLC8BITPT0>>24)&0xff;
	
	uErrorBit[5] = rNFMLC8BITPT1&0xff;
	uErrorBit[6] = (rNFMLC8BITPT1>>8)&0xff;
	uErrorBit[7] = (rNFMLC8BITPT1>>16)&0xff;
	uErrorBit[8] = (rNFMLC8BITPT1>>24)&0xff;
	
	if(!uErrorType) 
		return 1;
	if(uErrorType == 0x9) return 0;
	
	for(i=1;i<=uErrorType ;i++)	
	{
		if(uErrorByte[i] < 512)	
			pEncodingBaseAddr[uErrorByte[i]]^=uErrorBit[i];
		else
		{
			;
		/*
			if(pNandInfo.uPageSize == 512)
			{
			}
			else if(pNandInfo.uPageSize == 2048)
			{
			}
			else if(pNandInfo.uPageSize == 4096)						
			{
				
			}
			*/
		}			
	}
	return 1;
	
}


int NF8_ReadPage(u32 block,u32 page,u8 *buffer) // Normal nand by jcs
{
	int i;
	unsigned int blockPage;
	u8 *bufPt=buffer;
	u32 uPageSize;
	u8 Addr_Cycle, option1;
	u32 uResult;
	
	page=page&0x1f;
	blockPage=(block<<5)+page;
	uPageSize = 512;

	UART_Printf("2 : Address 3 Cycle , 3 : Address 4 Cycle \n");
	UART_Printf("Input Address_Cycle =");
	option1 = UART_GetIntNum();
	
	
   	if (option1==2 )
   	{
   		Addr_Cycle=3;
   	}
   	else if (option1==3 )
   	{
   		Addr_Cycle=4;
   	}
   	else {// default
   		Addr_Cycle = 4;
   	}


	rNFCONF = ( rNFCONF&~(0x3<<23) ) |(0<<30)|(1<<23)|(0x7<<12)|(0x7<<8)|(0x7<<4);
	rNFCONT = ( rNFCONT&~(0x1<<18) ) |(0<<18)|(0<<11)|(0<<10)|(0<<9)|(1<<6)|(1<<0); // Init NFCONT
	rNFSTAT = rNFSTAT|(1<<6)|(1<<7)|(1<<5)|(1<<4);


	NF_MECC_Lock2(); // Main ECC Lock
	NF_nFCE_L2(); // Chip Select Low
	NF_CLEAR_RB2(); // RnB Clear

	NF_CMD2(0x00);	// Read command

	NF_ADDR2(0); // Address Setting
	NF_ADDR2(blockPage&0xff);	// Address Setting
	NF_ADDR2((blockPage>>8)&0xff); // Block & Page num.
	if (Addr_Cycle == 4)
		NF_ADDR2((blockPage>>16)&0xff);

	NF_DETECT_RB2(); // RnB Detect.
	rNFSTAT = rNFSTAT | (1<<4); // RnB Pending Clear

	NF_MECC_UnLock2(); // Main ECC Unlock
	NF_RSTECC2();    // Initialize ECC

	for(i=0;i<uPageSize;i++) 
	{
		*bufPt++=NF_RDDATA82();	// Read Main data
	}

	for(i=0; i<13 ; i++)
	{
	//youngbo.song - Just read.
	//	aNand_Spare_Data[i] = NF_RDDATA8(); // Spare Data Read
		NF_RDDATA82();
	}

	NF_MECC_Lock2(); // Main ECC Lock

	while(!(rNFSTAT&(1<<6))); 	// Check decoding done 
	rNFSTAT = rNFSTAT | (1<<6);   // Decoding done Clear

	NF_nFCE_H2();  // Chip Select is High

	// 03. NAND ECC Data Check 
	while( rNF8ECCERR0 & (u32)(1<<31) ) ; // 8bit ECC Decoding Busy Check.

	// 04. ECC Error bit Detect & Correct
	bufPt=buffer; // Main Data Point Setting.
	uResult = CorrectECC8Data2(bufPt);
	return uResult;

}


int NF8_ReadPage_Adv(unsigned int block,unsigned int page,unsigned char *buffer) // Advanced nand by MGR
{
	int i,j,k;
	unsigned int blockPage, u512PagesPerBlock;

	unsigned char *bufPt=buffer;
	unsigned int page_size;	//uPageSize
	
	unsigned char Addr_Cycle, option2;
	unsigned int uResult[8]={0,};



	page=page&0xff;
	blockPage=(block<<6)+page; // only read page 0 & 1

	UART_Printf("4 : Address 4 Cycle , 5 : Address 5 Cycle, 6: Address 5 Cycle 4096 PageSize \n");
	UART_Printf("Input Address_Cycle =");
	option2 = UART_GetIntNum();
	

	if (option2==4 )
	{
		Addr_Cycle=4;
		page_size=2048;
	}
	else if (option2==5 )
	{
		Addr_Cycle=5;
		page_size=2048;
	}
	else if (option2==6 )
	{
		Addr_Cycle=5;
		page_size=4096;
		blockPage=(block<<7)+page; // only read page 0 & 1
	}
	else
	{	// default - addr:5, page:2048
		Addr_Cycle=5;
		page_size=2048;
	}

	rNFCONF = ( rNFCONF&~(0x3<<23) ) |(0<<30)|(1<<23)|(0x7<<12)|(0x7<<8)|(0x7<<4);
	rNFCONT = ( rNFCONT&~(0x1<<18) ) |(0<<18)|(0<<11)|(0<<10)|(0<<9)|(1<<6)|(1<<0); // Init NFCONT
	rNFSTAT = rNFSTAT|(1<<6)|(1<<7)|(1<<5)|(1<<4);
		
	////////////////////////////////////////////////////////////////////////
	//                                             8K Page Size Read.                                             //
	////////////////////////////////////////////////////////////////////////	
	
	u512PagesPerBlock = (page_size>>9);
	NF_MECC_Lock2(); // Main ECC Lock
	NF_nFCE_L2(); // Chip Select Low
	NF_CLEAR_RB2(); // RnB Clear

	for(i=0; i<u512PagesPerBlock; i++)
	{
		NF_MECC_Lock2(); // Main ECC Lock
		NF_CLEAR_RB2(); // RnB Clear

		if(i==0)
		{
			NF_CMD2(0x00);	// Read command
	
			NF_ADDR2(0); 	
			NF_ADDR2(0); 	
			NF_ADDR2(blockPage&0xff);		//
			NF_ADDR2((blockPage>>8)&0xff);	// Block & Page num.
			if (Addr_Cycle == 5)		NF_ADDR2((blockPage>>16)&0xff);	//
				
			NF_CMD2(0x30);	// 2'nd command
	
			NF_DETECT_RB2(); // RnB Detect.
			rNFSTAT = rNFSTAT | (1<<4); // RnB Pending Clear
		}
		else
		{
			NF_CMD2(0x05); // Random Address Access = 512K start
			NF_ADDR2((512*i)&0xFF);
			NF_ADDR2(((512*i)>>8)&0xFF);
			NF_CMD2(0xE0);
			
		}
		
		NF_MECC_UnLock2(); // Main ECC Unlock
		NF_RSTECC2();    // Initialize ECC

		for(j=0;j<512;j++)  
			*bufPt++=NF_RDDATA82();	// Read Main data

		// Spare Area Address Setting.
		NF_CMD2(0x05);
		NF_ADDR2((page_size+13*i)&0xFF);
		NF_ADDR2(((page_size+13*i)>>8)&0xFF);
		NF_CMD2(0xE0);

		for(k=0; k<13 ; k++)
			NF_RDDATA82(); // Spare Data Read

		NF_MECC_Lock2(); // Main ECC Lock

		while(!(rNFSTAT&(1<<6))); 	// Check decoding done 
		rNFSTAT = rNFSTAT | (1<<6);   // Decoding done Clear

		while( rNF8ECCERR0 & (u32)(1<<31) ) ; // 8bit ECC Decoding Busy Check.
		uResult[i] = CorrectECC8Data2((bufPt-512));
	}

	NF_nFCE_H2();  // Chip Select is High
	return (uResult[0]+uResult[1]+uResult[2]+uResult[3]+uResult[4]+uResult[5]+uResult[6]+uResult[7]);
}


void NANDT_CopyToSRam(void)
{
	//main start
	u32 i, j, page_size, nand_type, option;	//block, blockcopy_count, 
	UART_Printf("\n------------------------------------------------------\n");
	UART_Printf("2 or 3 : 512 page size, 4 or 5 : 2048 page size, 6 : 4096 page size\n");
	UART_Printf("Input Option Number = ");
	option = UART_GetIntNum();
	UART_Printf("------------------------------------------------------\n");
   
	if (option==2 || option==3)
	{
		page_size=512;
		nand_type=0; // normal
	}
	else if (option==4 || option==5)
	{
		page_size=2048;
		nand_type=1; // Advanced
	}
	else//if (option==6)
	{
		page_size=4096;
		nand_type=1; // Advanced
	} 

	downPt=(unsigned char *)(DOWNLOAD_ADDRESS);

	downPt2=(unsigned char *)(DOWNLOAD_ADDRESS); // Zeus 071212

	if(nand_type==0)  // Normal nand
	{
		NF8_Init();   
		for(i=0;i<16;i++)   // 512 pages *16 = 8K 
		{
			if(!NF8_ReadPage(0, i, (unsigned char *)downPt))   
			{

				UART_Printf("Check This Condition ECC Uncorrectable Error\n");
				while(1);
			//	ECCFailCount(); // Zeus.. 071212
			}
			downPt += 512;	    // Next page	
			// -------------------------------------------------
			UART_Printf("[Main Area]");
			for(j=0 ; j<512 ; j++)
			{
				if(j%16==0)
		    			UART_Printf("\n%04xh:",j);
				
	        		UART_Printf("%02x ", *(downPt2+j) );
			}
			UART_Printf("\n");
			UART_Printf("NAND Read Page : Success\n");
			downPt2 += 512; //next page
			// -------------------------------------------------
		}
		#if 0 // zeus 071212
		// secure case.
		if( ((rE_FUSEDEVID >> 15) & 0x1) == 1 ) {	// s3c6410 - default non secure.(0)
			downPt = (unsigned char*)SECURE_KEY_ADDRESS;
			for(i=8;i<12;i++)   // 512 pages *4 = 2K <= Secure Key.
			{
				if(!NF8_ReadPage(0, i, (unsigned char *)downPt))   
				{   
					ECCFailCount();
				}
				downPt += 512;	    // Next page	
			}
		}
		#endif 		
	}
	else if(nand_type==1) // Advanced nand
	{
		NF8_Init();

		if(page_size==4096)	// 4K page 
		{
			for (i=0; i<2;i++) // 4K Pages * 2 = 8K Size
			{
				if(!NF8_ReadPage_Adv(0, 0, (unsigned char *)downPt))   
				{
					UART_Printf("Check This Condition ECC Uncorrectable Error\n");
					while(1);
					// ECCFailCount(); // zeus 071212
				}
				downPt += 4096;
				// -------------------------------------------------
				UART_Printf("[Main Area]");
				for(j=0 ; j<4096 ; j++)
				{
					if(j%16==0)
			    			UART_Printf("\n%04xh:",j);
					
		        		UART_Printf("%02x ", *(downPt2+j) );
				}
				UART_Printf("\n");
				UART_Printf("NAND Read Page : Success\n");
				downPt2 += 4096; //next page
				// -------------------------------------------------				
			}

			#if 0 
			// secure case.
			if( ((rE_FUSEDEVID >> 15) & 0x1) == 1 ) {	// s3c6410 - default non secure.(0)
				downPt = (unsigned char*)SECURE_KEY_ADDRESS;
				if(!NF8_ReadPage_Adv(0, 1, (unsigned char *)downPt))   
				{   
					ECCFailCount();
				}
			}
			#endif
		}
		else if(page_size==2048)
		{
			for(i=0;i<4;i++)   //  2K pages *4
			{
				if(!NF8_ReadPage_Adv(0, i, (unsigned char *)downPt))   
				{   
					UART_Printf("Check This Condition ECC Uncorrectable Error\n");
					while(1);
					// ECCFailCount(); // zeus 071212
				}
				downPt += 2048;	    // Next page	
				// -------------------------------------------------
				UART_Printf("[Main Area]");
				for(j=0 ; j<2048 ; j++)
				{
					if(j%16==0)
			    			UART_Printf("\n%04xh:",j);
					
		        		UART_Printf("%02x ", *(downPt2+j) );
				}
				UART_Printf("\n");
				UART_Printf("NAND Read Page : Success\n");
				downPt2 += 2048; //next page
				// -------------------------------------------------			

			}
			#if 0 
			// secure case.
			if( ((rE_FUSEDEVID >> 15) & 0x1) == 1 ) {	// s3c6410 - default non secure.(0)
				downPt = (unsigned char*)SECURE_KEY_ADDRESS;
				if(!NF8_ReadPage_Adv(0, 2, (unsigned char *)downPt))   
				{   
					ECCFailCount();
				}
			}
			#endif
		}
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
//	NANDT_8KStepStone_ReadWrite,   "Stepping Stone Test ",
//	NANDT_SLEEPWakeup,                       "Sleep Wake Up Booting       ",
	NANDT_CopyToSRam,                         "IROM Code Function      ",
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
	NAND_Inform[g_NandContNum].uSpareECCtest = 0;
#if 0
	//K9S1208
	NAND_Inform[g_NandContNum].uTacls = 0;
	NAND_Inform[g_NandContNum].uTwrph0 = 35;
	//NAND_Inform[g_NandContNum].uTwrph0 = 35+10;	//Pad delay : about 10ns
	NAND_Inform[g_NandContNum].uTwrph1 = 10;	
#else
	//K9F1208U0B
	NAND_Inform[g_NandContNum].uTacls = 10;
	NAND_Inform[g_NandContNum].uTwrph0 = 25+30;	//Pad delay : about 10ns
	//NAND_Inform[g_NandContNum].uTwrph1 = 10;
	NAND_Inform[g_NandContNum].uTwrph1 = 15;	//tWH : 15ns
	
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
//	NAND_Inform[g_NandContNum].uBlockNum = 4096;
//	NAND_Inform[g_NandContNum].uBlockNum = 1024;	
	NAND_Inform[g_NandContNum].uBlockNum = 8096;	// K9K8G08U0A
	NAND_Inform[g_NandContNum].uPageNum = 64;
	NAND_Inform[g_NandContNum].uPageSize = NAND_PAGE_2048;
	NAND_Inform[g_NandContNum].uSpareSize = NAND_SPARE_64;
	NAND_Inform[g_NandContNum].uECCtest = 0;
	NAND_Inform[g_NandContNum].uSpareECCtest = 0;
//	NAND_Inform[g_NandContNum].uTacls = 0;
//	NAND_Inform[g_NandContNum].uTwrph0 = 12+10;	//Pad delay : about 10ns
//	NAND_Inform[g_NandContNum].uTwrph1 = 5;

	NAND_Inform[g_NandContNum].uTacls = 30;
	NAND_Inform[g_NandContNum].uTwrph0 = 12+30;	//Pad delay : about 10ns
	NAND_Inform[g_NandContNum].uTwrph1 = 5+30;
	
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
	NAND_Inform[g_NandContNum].uSpareECCtest = 0;
//	NAND_Inform[g_NandContNum].uTacls = 0;
//	NAND_Inform[g_NandContNum].uTwrph0 = 15+10;	//Pad delay : about 10ns
//	NAND_Inform[g_NandContNum].uTwrph1 = 5;

	NAND_Inform[g_NandContNum].uTacls = 0;
	NAND_Inform[g_NandContNum].uTwrph0 = 15+20;	//Pad delay : about 10ns
	//NAND_Inform[g_NandContNum].uTwrph1 = 5;
	NAND_Inform[g_NandContNum].uTwrph1 = 5;		//tWH : 5ns + margin
	
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

void NandT_Normal8bitSimple(void)
{
	u8 *aBuffer, *aSpareBuffer;

	aBuffer = (u8 *)malloc(NAND_PAGE_MAX);
	aSpareBuffer = (u8 *)malloc(NAND_SPARE_MAX);

	NAND_ReadPageSLC_4BitSimple(0, 0, 0, aBuffer, aSpareBuffer);
		
}

#if 0 
void Isr_Dma0Done(void)
{
 	DMACH_ClearIntPending(&oDmac0);
 	//rDMAC0IntTCClear = 0xff;
	UART_Printf("DMA ISR %d\n", g_DmaDone);
	g_DmaDone=1;
	g_DmaDone++;
	INTC_ClearVectAddr();
       //Write_VECTADDR(0x0);
}
#endif
/*
void DMA_Test(void)
{
	u32 uIndex0, uIndex1, uIndex2;
	u32 uTEmp0, uTemp1, uTemp2;

	u32 uAddress0, uAddress1, uAddress2, uAddress3, uAddress4, uAddress5;

	uAddress0 = 0x51000000;
	uAddress1 = 0x51250000;
	uAddress2 = 0x51500000;
	uAddress3 = 0x51750000;
	uAddress4 = 0x52000000;
	uAddress5 = 0x80200010;	

	// normal DMA 0 chanel Test
	INTC_Init();
     	DMAC_InitCh(DMA0, DMA_ALL, &oDmac0);
	INTC_SetHandler(INT_DMA0, Isr_Dma0Done);
	SYSC_SelectDMA(eSEL_PWM, 1); // Zeus 070830
	INTC_Unmask(INT_DMA0);
//	INTC_SetVectAddr(INT_DMA0,  Isr_Dma0Done);
//	INTC_Enable(INT_DMA0);

	for ( uIndex0 = 0 ; uIndex0 < 5; uIndex0++)
	{
		*(unsigned int*)(uAddress0+uIndex0*4) = 0x12345678;
		*(unsigned int*)(uAddress1+uIndex0*4) = 0x0;
		*(unsigned int*)(0x80200010) = 0x1;
	}
	DMACH_Setup(DMA_A, 0x0, uAddress0, 0, uAddress5, 1, WORD, 5, DEMAND, MEM, DMA1_NAND_TX, SINGLE, &oDmac0);
      	DMACH_Start(&oDmac0);


	UART_Printf("IRQ: 0 / Polling :1\n");
	uTEmp0 = UART_GetIntNum();
	if (uTEmp0 == 0)
	{
		while(g_DmaDone==0);
	}
	else
	{
		while (!DMAC_IsTransferDone(&oDmac0));
	}


	INTC_Disable(INT_DMA0);
    	DMAC_Close(DMA0, DMA_ALL, &oDmac0);
}
*/


///////////////////////////////////////////////////////////////////////////////////
////////////////////                     Nand Main Test                  /////////////////////////// 
///////////////////////////////////////////////////////////////////////////////////

const testFuncMenu nand_menu[] =
{
	NandT_Normal8bit,					"K9F1208(Normal 8bit)",
	NandT_Advanced8bit,				"K9F2G08(Advanced 8bit)",
	NandT_MLC8bit,						"K9HBG08(MLC 8bit)",
//	NandT_Normal8bitSimple,				"K9F1208(Normal 8bit) Simple",
 	//DMA_Test,							"DMA Test",
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
	NAND_Inform[g_NandContNum].uSpareECCtest = 0;
	//K9F1208U0B
	NAND_Inform[g_NandContNum].uTacls = 10;
	NAND_Inform[g_NandContNum].uTwrph0 = 35+10;	//Pad delay : about 10ns
	NAND_Inform[g_NandContNum].uTwrph1 = 15+15;		//tWH : 15ns

	NAND_Init(g_NandContNum);

}


