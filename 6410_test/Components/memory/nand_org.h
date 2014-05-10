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
*	File Name : nand.h
*  
*	File Description : This file implements the API functons for Nand controller.
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
#ifndef __NAND_H__
#define __NAND_H__


#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

#include "def.h"

#define 	NAND_CONNUM			(1)

// Nand device type
#define NAND_Normal8bit			(0)
#define NAND_Advanced8bit		(1)
#define NAND_MLC8bit			(2)

// Nand Page size
#define NAND_PAGE_512			(512)
#define NAND_PAGE_2048			(2048)
#define NAND_PAGE_MAX			(2048)

// Nand Spare Area Size
#define NAND_SPARE_16			(16)
#define NAND_SPARE_64			(64)
#define NAND_SPARE_MAX		(64)

// Nand data transfer mode selection
#define POLLING_TRANSFER		(0)
#define DMA_TRANSFER			(1)
#define NAND_TRANSFER_MODE	DMA_TRANSFER

// Nand Interupt Source
#define NAND_4BITECC_ENC		(1<<7)
#define NAND_4BITECC_DEC		(1<<6)
#define NAND_ILLEGAL_ACCESS	(1<<5)
#define NAND_RnB_TRANS			(1<<4)

// MLC Type Nand Transfer Size(512byte ECC)
#define NAND_MLC_TRANS_SIZE	(512)
#define NAND_MLC_ECC_SIZE		(7)

// ECC Error Type
#define NAND_ECCERR_1BIT		(1)
#define NAND_ECCERR_2BIT		(2)
#define NAND_ECCERR_3BIT		(3)
#define NAND_ECCERR_4BIT		(4)
#define NAND_ECCERR_UNCORRECT		(5)
#define NAND_ECCERR_MULTI		(2)
#define NAND_ECCERR_AREA		(3)

//MLC Spare area ecc generation
#define MLC_SPARE_ECC			(1)

typedef struct NAND_Inform
{
	u32	uNandType;
	u32 uAddrCycle;
	u32 uBlockNum;
	u32 uPageNum;
	u32 uPageSize;
	u32 uSpareSize;
	u32 uECCtest;
	u32 uSpareECCtest;
	u32 uTacls;
	u32 uTwrph0;
	u32 uTwrph1;
	u32 uAllBitEccCheck;
	u32 uMLCECCPageWriteSector;			// 512byte * 4sector for ECC generation
	u32 uMLCECCPageReadSector;		// 512byte * 4sector for ECC generation
	u32 uMLCECCErrBytePos[5];
	u32 uPerformanceCheck;
}
NAND_oInform;

typedef struct NAND_EccError
{
	u32	uEccErrByte1;
	u32 uEccErrBit1;
	u32	uEccErrByte2;
	u32 uEccErrBit2;
	u32	uEccErrByte3;
	u32 uEccErrBit3;
	u32	uEccErrByte4;
	u32 uEccErrBit4;
	u32	uEccErrByte5;
	u32 uEccErrBit5;	
}
NAND_oEccError;

typedef enum NAND_ERROR
{
	eNAND_NoError = 0x00, 	eNAND_1bitEccError = 0x01, 	eNAND_2bitEccError = 0x02, eNAND_3bitEccError = 0x04, eNAND_4bitEccError = 0x08,
	eNAND_MultiError = 0x10, 	eNAND_EccAreaError = 0x20, 	eNAND_UncorrectableError = 0x40, 	eNAND_EtcError = 0x80, 	eNAND_Spare1bitEccError = 0x100,
	eNAND_SpareMultiError = 0x200,	eNAND_SpareEccAreaError = 0x400, eNAND_ProgramError = 0x800, eNAND_EraseError = 0x1000, 	eNAND_InvalidBlock = 0x2000
}NAND_eERROR;


/******************************************/
/*          Variable Define               */
/******************************************/
#define ID_K9S1208V0M		(0xec76)
#define ID_K9F2G08U0M		(0xecda)

//Nand Lock Mode selection
#define NANDLOCK_DIS	0
#define NANDLOCK_SOFT	1
#define NANDLOCK_TIGHT	2

#define ECC512B			0
#define ECC2KB			1
#define ECCSIZE			ECC2KB

/******************************************/
/*          Enumeration Data              */
/******************************************/
typedef enum apNF_eEccAreaError
{
  apNF_NOERR 		= 0,
  apNF_1bINT_ERR 	= 1,
  apNF_MULTI_ERR 	= 2,
  apNF_AREA_ERR 	= 3
}apNF_eEccAreaError;




/******************************************/
/*          Function Define               */
/******************************************/
#define NF_MECC_UnLock(Controller)         	{NAND(Controller)->rNFCONT&=~(1<<7);}
#define NF_MECC_Lock(Controller)         		{NAND(Controller)->rNFCONT|=(1<<7);}
#define NF_SECC_UnLock(Controller)         	{NAND(Controller)->rNFCONT&=~(1<<6);}
#define NF_SECC_Lock(Controller)        	 	{NAND(Controller)->rNFCONT|=(1<<6);}
#define NF_CMD(Controller, cmd)				{NAND(Controller)->rNFCMMD=cmd;}
#define NF_ADDR(Controller, addr)			{NAND(Controller)->rNFADDR=addr;}	
#define NF_nCS1_L(Controller)				{NAND(Controller)->rNFCONT&=~(1<<2);}
#define NF_nCS1_H(Controller)				{NAND(Controller)->rNFCONT|=(1<<2);}
#define NF_nFCE_L(Controller)				{NAND(Controller)->rNFCONT&=~(1<<1);}
#define NF_nFCE_H(Controller)				{NAND(Controller)->rNFCONT|=(1<<1);}
#define NF_RSTECC(Controller)				{NAND(Controller)->rNFCONT|=(1<<5)|(1<<4);}
#define NF_RDDATA(Controller) 				(NAND(Controller)->rNFDATA)
#define NF_RDDATA8(Controller) 			((u8)(*(u8*)(&NAND(Controller)->rNFDATA)) )
#define NF_RDDATA16(Controller) 			((u16)(*(u16*)(&NAND(Controller)->rNFDATA)) )
#define NF_WRDATA(Controller, data) 		{NAND(Controller)->rNFDATA=data;}
#define NF_WRDATA8(Controller, data) 		{*(u8*)(&NAND(Controller)->rNFDATA)=data;}

// RnB Signal
#define NF_CLEAR_RnB(Controller)    		{NAND(Controller)->rNFSTAT |= (1<<4);}	// Have write '1' to clear this bit.
#define NF_DETECT_RnB(Controller)    		{while(!(NAND(Controller)->rNFSTAT&(1<<4)));}


u8 NAND_Init(u32 Controller);
void NAND_GetDurationValue(u32 Controller, u32 *uTacls, u32 *uTwrph0, u32 *uTwrph1);
void NAND_TimingParaSetting(u32 Controller);

u32 NAND_ReadID(u32 Controller);
void NAND_Reset(u32 Controller);
NAND_eERROR NAND_CheckInvalidBlock(u32 Controller, u32 uBlock);
NAND_eERROR NAND_CheckECCError(u32 Controller);
void NAND_SeekECCErrorPosition(u32 Controller, u32 *uEccErr0, u32 *uEccErr1, u32 *uEccErr2);
NAND_eERROR NAND_ReadPageSLC(u32 Controller, u32 uBlock, u32 uPage, u8 *pBuffer, u8 *pSpareBuffer);
NAND_eERROR NAND_ReadPageMLC(u32 Controller, u32 uBlock, u32 uPage, u8 *pBuffer, u8 *pSpareBuffer);
NAND_eERROR NAND_ReadPage(u32 Controller, u32 uBlock, u32 uPage, u8 *pBuffer, u8 *pSpareBuffer);
NAND_eERROR NAND_WritePageSLC(u32 Controller, u32 uBlock, u32 uPage, u8 *pBuffer, u8 *aSpareBuffer);
NAND_eERROR NAND_WritePageMLC(u32 Controller, u32 uBlock, u32 uPage, u8 *pBuffer, u8 *aSpareBuffer);
NAND_eERROR NAND_WritePage(u32 Controller, u32 uBlock, u32 uPage, u8 *pBuffer, u8 *aSpareBuffer);
NAND_eERROR NAND_EraseSingleBlock(u32 Controller, u32 uBlock);
NAND_eERROR NAND_EraseMultiBlock(u32 Controller, u32 uBlock, u32 uBlockNum);
NAND_eERROR NAND_SoftLockingBlock(u32 Controller, u32 uStartBlock, u32 uEndBlock);
NAND_eERROR NAND_LockTightBlock(u32 Controller, u32 uStartBlock, u32 uEndBlock);
NAND_eERROR NAND_UnLockBlock(u32 Controller);
NAND_eERROR NAND_UnLockTightBlock(u32 Controller);
NAND_eERROR NAND_WritePageSLCWithInvalidData(u32 Controller, u32 uBlock, u32 uPage, u8 *pBuffer, u32 uErrorType);
NAND_eERROR NAND_WritePageMLCWithInvalidData(u32 Controller, u32 uBlock, u32 uPage, u8 *pBuffer, u32 uErrorType);
NAND_eERROR NAND_WritePageWithInvalidData(u32 Controller, u32 uBlock, u32 uPage, u8 *pBuffer, u32 uErrorType);
NAND_eERROR NAND_WritePageSLCWithInvalidSpareData(u32 Controller, u32 uBlock, u32 uPage, u8 *pBuffer, u32 uErrorType);
NAND_eERROR NAND_WritePageMLCWithInvalidSpareData(u32 Controller, u32 uBlock, u32 uPage, u8 *pBuffer, u32 uErrorType);
NAND_eERROR NAND_WritePageWithInvalidSpareData(u32 Controller, u32 uBlock, u32 uPage, u8 *pBuffer, u32 uErrorType);
NAND_eERROR NAND_ReadPageSLCforNormal(u32 Controller, u32 uBlock, u32 uPage, u8 *pBuffer, u8 *pSpareBuffer);
NAND_eERROR NAND_ReadPageSLCforAdvance(u32 Controller, u32 uBlock, u32 uPage, u8 *pBuffer, u8 *pSpareBuffer);
NAND_eERROR NAND_ReadPageforPerform(u32 Controller, u32 uBlock, u32 uPage, u8 *pBuffer, u8 *pSpareBuffer);
NAND_eERROR NAND_WritePageSLCforNormal(u32 Controller, u32 uBlock, u32 uPage, u8 *pBuffer, u8 *aSpareBuffer);
NAND_eERROR NAND_WritePageSLCforAdvance(u32 Controller, u32 uBlock, u32 uPage, u8 *pBuffer, u8 *aSpareBuffer);
NAND_eERROR NAND_WritePageforPerform(u32 Controller, u32 uBlock, u32 uPage, u8 *pBuffer, u8 *pSpareBuffer);
NAND_eERROR NAND_ReadMultiPage(u32 Controller, u32 uStartBlock, u32 uStartPage, u8 *pBuffer, u32 uSize);
void NAND_ReadPageSLCSetup(u32 Controller, u32 uBlock, u32 uPage);
void NAND_WritePageSLCSetup(u32 Controller, u32 uBlock, u32 uPage);
NAND_eERROR NAND_WritePageSLCClose(u32 Controller, u8 *pSpareBuffer);

void __irq NAND_ISR0(void);
void __irq NAND_DmaISR(void);
u32 NAND_ReadNFCONRegister(u32 Controller);

#ifdef __cplusplus
}
#endif

#endif //__ONENAND_H__



