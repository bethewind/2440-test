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
*	File Name : onenand.h
*  
*	File Description : This file implements the API functons for OneNand controller.
*
*	Author : Heemyung.noh
*	Dept. : AP Development Team
*	Created Date : 2006/11/10
*	Version : 0.1 
* 
*	History
*	- Created(Heemyung.noh 2006/11/10)
*  
**************************************************************************************/
#ifndef __ONENAND_H__
#define __ONENAND_H__


#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

#include "def.h"

#define OND_6400				(0x00)
#define OND_6410				(0x01)
#define OND_DEVICE				OND_6410

// OneNand Transfer Mode Selection
#define OND_DMA					(0x00)
#define OND_POLLING				(0x01)
#define OND_LDM_INST			(0x02)
#define OND_TRANS_MODE		(OND_LDM_INST)

#define OND_LDM_4BURST			(0x00)
#define OND_LDM_8BURST			(0x01)

#if(OND_DEVICE == OND_6410)	
#define OND_LDM_BURST_SIZE	OND_LDM_8BURST
#else
#define OND_LDM_BURST_SIZE	OND_LDM_4BURST
#endif

// OneNand Access Mode Selection
#define 	ONENAND_COMMAND		(0x00)
#define	ONENAND_DIRECT		(0x01)
#define 	ONENAND_ACCESS_MODE	ONENAND_COMMAND

// OneNand Interrupt Mode
#define	ONENAND_POLLING		(0x0)
#define 	ONENAND_INTERRUPT	(0x1)
#define	ONENAND_INTMODE		ONENAND_INTERRUPT

#define 	ONENAND_CONNUM			(2)
#define 	ONENAND_PAGESIZE			(2048)
#define 	ONENAND_SPARESIZE		(64)

#define 	ONENAND_WARMRESET		(1)
#define 	ONENAND_CORERESET		(2)
#define 	ONENAND_HOTRESET			(3)

#define ONDT_CONTINUE				(0)
#define ONDT_BURST4				(1)
#define ONDT_BURST8				(2)
#define ONDT_BURST16				(3)
#define ONDT_BURST32				(4)

#define ONENAND_LOCKTIGHT			(1)
#define ONENAND_LOCK				(2)
#define ONENAND_UNLOCK			(4)

#define ONENAND_WITH_CORRECT			(0x00)
#define ONENAND_WITHOUT_CORRECT		(0x01)

#define 	ONENAND_BOOTRAM_BASE	(0x0000)
#define 	ONENAND_DATARAM0_BASE	(0x0400)
#define 	ONENAND_DATARAM1_BASE	(0x0c00)
#define 	ONENAND_DATARAM_DIRECT	(0x0200*4)		// Map11 command

#define 	ONENAND_SPARERAM00_BASE		(0x10020)
#define 	ONENAND_SPARERAM01_BASE		(0x10030)
#define 	ONENAND_SPARERAM02_BASE		(0x10040)
#define 	ONENAND_SPARERAM03_BASE		(0x10050)

#define 	ONENAND_SPARERAM10_BASE		(0x10060)
#define 	ONENAND_SPARERAM11_BASE		(0x10070)
#define 	ONENAND_SPARERAM12_BASE		(0x10080)
#define 	ONENAND_SPARERAM13_BASE		(0x10090)

#define OND_MULTIERASE_SIZE	(64)

#define OND_MANUFACT_ID			(0xf000*4)
#define OND_DEVICE_ID				(0xf001*4)
#define OND_VER_ID					(0xf002*4)
#define OND_DATA_BUFSIZE			(0xf003*4)
#define OND_BOOT_BUFSIZE			(0xf004*4)
#define OND_AMOUNT_OF_BUFSIZE	(0xf005*4)
#define OND_TECHNOLOGY				(0xf006*4)
#define OND_STARTADDR1				(0xf100*4)
#define OND_STARTADDR2				(0xf101*4)
#define OND_STARTADDR3				(0xf102*4)
#define OND_STARTADDR4				(0xf103*4)
#define OND_STARTADDR8				(0xf107*4)
#define OND_STARTBUF				(0xf200*4)
#define OND_COMMAND				(0xf220*4)
#define OND_SYSCONFIG				(0xf221*4)
#define OND_STATUS					(0xf240*4)
#define OND_INTSTATUS				(0xf241*4)
#define OND_STARTBLOCKADDR		(0xf24c*4)
#define OND_WPROT_STATUS			(0xF24E*4)
#define OND_ECCSTATUS				(0xFF00*4)

#define OND_READINT					(0x80)
#define OND_WRITEINT					(0x40)
#define OND_ERASEINT					(0x20)
#define OND_RESETINT					(0x10)

#define OND_WD_ENABLE				(0)
#define OND_WD_DISABLE				(1)

#define ONDT_NUMOFBAD_BUFFER	(100)

typedef struct ONENAND_Inform
{
	u32	uManufID;
	u32 uDeviceID;
	u32 uDataBufSize; 
	u32 uBootBufSize; 
	u32 uAmountOfBuf; 
	u32 uTechnology;
	u32 uFlashVerID;
	u32 uNumOfBlock;
	u32 uNumOfPage;
	u32 uFlashClock;
	u32 uDbsDfs;
	u32 uCacheReadSupport;
	u32 uTotalBlockWidth;
	u32 uDeviceType;
}
ONENAND_oInform;

typedef struct OneNandT_InterruptFlag
{
	volatile u32 LdFailEccErrInt;
	volatile u32 IntToInt;			
	volatile u32 PgmFailInt;		
	volatile u32 ErsFailInt;	
	volatile u32 LoadCmpInt;		
	volatile u32 PgmCmpInt;		
	volatile u32 ErsCmpInt;		
	volatile u32 BlkRwCmpInt;		
	volatile u32 LockedBlkInt;		
	volatile u32 UnsupCmdInt;		
	volatile u32 IntActInt;		
	volatile u32 RdyActInt;	
	volatile u32 RstCmpInt;			
	volatile u32 CacheOpErrInt;

	volatile u32 IntActInt_Count;
}OneNandT_oInterruptFlag;

typedef enum OneNAND_MODE
{
	eOND_ASYNC, eOND_SYNC_CONT, eOND_SYNC_BURST4, eOND_SYNC_BURST8, eOND_SYNC_BURST16, eOND_SYNC_BURST32
}OneNAND_eMODE;

typedef enum OneNAND_FlashClock
{
	eDiv1_HCLKx2=0, eDiv2_HCLKx2=1, eDiv3_HCLKx2=2, eDiv4_HCLKx2=3
}OneNAND_eFlashClock;

typedef enum OneNAND_LATENCY
{
	eOND_LATENCY3 = 3, 
	eOND_LATENCY4 = 4, 
	eOND_LATENCY5 = 5,
	eOND_LATENCY6 = 6,
	eOND_LATENCY7 = 7,
	eOND_LATENCY8 = 0,
	eOND_LATENCY9 = 1,
	eOND_LATENCY10 = 2
}OneNAND_eLATENCY;

typedef enum OneNAND_INT_ERROR
{
	eOND_LDFAILECCERR = 0x01, 	eOND_INTTO = 0x02, 		eOND_PGMFAIL = 0x04, 		eOND_ERSFAIL = 0x08, 		eOND_LOADCMP = 0x10, 
	eOND_PGMCMP = 0x20, 		eOND_ERSCMP = 0x40, 	eOND_BLKRWCMP = 0x80, 		eOND_LOCKEDBLK = 0x100, 	eOND_UNSUPCMD = 0x200, 	
	eOND_INTACT = 0x400, 		eOND_RDYACT = 0x800, 	eOND_RSTCMP = 0x1000, 		eOND_CACHEOPERR = 0x2000, 	eOND_NOERROR = 0
}OneNAND_eINTERROR;

typedef void (*ONENAND_ISR_Routine) (u32 uController);
typedef struct OneNand_CallBack
{
	ONENAND_ISR_Routine ONENAND_ISR_Func;
}OneNand_sCallBack;

u8 ONENAND_Init(u32 Controller);
#if(OND_DEVICE == OND_6410)	
void ONENAND_SetWatchDog_Reset(u32 Controller, u32 uWatchDog);
#endif
void ONENAND_Reset(u32 Controller, u32 Reset);
void ONENAND_SetLLI(u32 uSrcAddr, u32 uDstAddr);
void ONENAND_SetSyncMode(u32 Controller, OneNAND_eMODE eMode);
void ONENAND_EnableIOBE(u32 Controller, u32 uEnable);
void ONENAND_EnableECCCorrection(u32 Controller, u32 uEnable);
void ONENAND_SetBurstLatency(u32 Controller, OneNAND_eLATENCY eLatency);
void ONENAND_SetFlashClock(u32 Controller, OneNAND_eFlashClock eFlashClock);
void ONENAND_SetAccClock(u32 Controller);
void ONENAND_GetDevInformation(u32 Controller);
u32 ONENAND_GetMemCfg(u32 Controller);
u32 ONENAND_GetBurstLength(u32 Controller);
u32 ONENAND_GetInterruptStatus(u32 Controller);
u32 ONENAND_GetInterruptMask(u32 uController);
u32 ONENAND_GetEccErrStatus(u32 uController);
void ONENAND_SetMemSpace(u32 Controller);
void ONENAND_EnableIntPin(u32 Controller, u32 Enable);
u8 ONENAND_UnlockBlock(u32 uController, u32 uStartBlkAddr, u32 uEndBlkAddr);
OneNAND_eINTERROR ONENAND_UnlockAllBlock(u32 Controller);
u8 ONENAND_LockBlock(u32 Controller, u32 uStartBlkAddr, u32 uEndBlkAddr);
u8 ONENAND_LockTightBlock(u32 Controller, u32 uStartBlkAddr, u32 uEndBlkAddr);
OneNAND_eINTERROR ONENAND_EraseBlock(u32 Controller, u32 uStartBlkAddr, u32 uEndBlkAddr);
OneNAND_eINTERROR ONENAND_EraseVerify(u32 Controller, u32 uBlkAddr);
u8 ONENAND_CopyBack(u32 Controller, u32 uSourceBlkAddr, u8 uSourcePageAddr, u32 uDestinationBlkAddr, u8 uDestinationPageAddr, u32 uPageSize);
u8 ONENAND_WritePage(u32 Controller, u32 uBlkAddr, u8 uPageAddr, u32* aData);
u8 ONENAND_WritePageWithSpare(u32 Controller, u32 uBlkAddr, u8 uPageAddr, u32* aData, u32* pSpare);

u8 ONENAND_ReadPage(u32 Controller, u32 uBlkAddr, u8 uPageAddr, u32* aData);
void ONENAND_LoadToXIPBuffer(u32 Controller, u32 uBlkAddr, u8 uPageAddr);
void ONENAND_WriteFromXIPBuffer(u32 Controller, u32 uBlkAddr, u8 uPageAddr);
void ONENAND_EnableSpareTransfer(u32 Controller, u8 bEnable);
u8 ONENAND_ReadPageWithSpare(u32 Controller, u32 uBlkAddr, u8 uPageAddr, u32* aData, u32 *aSpare);
void ONENAND_ConfigOTPAccess(u32 Controller, u32 uBlkAddr, u8 uPageAddr);
void ONENAND_ConfigSpareAccess(u32 Controller, u32 uBlkAddr, u8 uPageAddr);
void ONENAND_ConfigMainAccess(u32 Controller, u32 uBlkAddr, u8 uPageAddr);
void ONENAND_PipelineReadAhead(u32 Controller, u32 uBlkAddr, u8 uPageAddr, u8 uPageSize);



void ONENAND_WriteIntoDataram(u32 Controller, u32 uAddr, u32 uData);
void ONENAND_ReadOutDataram(u32 Controller, u32 uAddr, u32 *uData);
void ONENAND_WriteIntoFlash(u32 Controller, u32 uBlockAddr, u8 uPageAddr, u32 uData);
void ONENAND_ReadOutFlash(u32 Controller, u32 uBlockAddr, u8 uPageAddr, u32 *uData);
void ONENAND_WriteCmd(u32 Controller, u32 uBlockAddr, u8 uPageSize, u32 uData);
void ONENAND_ReadCmd(u32 Controller, u32 BlockAddr, u8 uPageSize, u32 *uData);
void ONENAND_DirectWrite(u32 Controller, u32 uDeviceAddr, u32 uData);
void ONENAND_DirectRead(u32 Controller, u32 uDeviceAddr, u32 *uData);

void __irq ONENAND_DmaISR(void);
void __irq ONENAND_ISR(void);
void ONENAND_SetISRHandler(void *Ond_Callback);
void ONENAND_EnableInterrupt(u32 uController, OneNAND_eINTERROR eInterrupt);
void ONENAND_AckInterrupt(u32 uController, OneNAND_eINTERROR eInterrupt);
void ONENAND_EnableAllInterrupt(u32 uController);
void ONENAND_DisableInterrupt(u32 uController, OneNAND_eINTERROR eInterrupt);
void ONENAND_DisableAllInterrupt(u32 uController);
void ONENAND_SetSyncMode_1(u32 Controller, u32 uReadMode, u32 uWriteMode, u32 uBurst);
u8 ONENAND_ReadPageForCacheReadWithSpare(u32 Controller, u32 uBlkAddr, u8 uPageAddr, u32* aData, u32 *aSpare, u32 uPageSize);


u8 ONENAND_EraseBlockDirectAccess(u32 uController, u32 uBlock);
u8 ONENAND_EraseMultiBlockDirectAccess(u32 uController, u32 uStartBlock, u32 uEndBlock);
u8 ONENAND_WritePageDirectAccess(u32 uController, u32 uBlock, u32 uPage);
u8 ONENAND_ReadPageDirectAccess(u32 uController, u32 uBlock, u32 uPage);
#if (OND_TRANS_MODE == OND_DMA)
void ONENAND_DirectRead_DMA(u32 Controller, u32 uDeviceAddr, u32 *uData);
void ONENAND_DirectWrite_DMA(u32 Controller, u32 uDeviceAddr, u32 *uData);
#endif

u8 ONENAND_ReadPageForCacheRead(u32 uController, u32 uBlkAddr, u8 uPageAddr, u32* aData, u32 uPageSize);
void ONENAND_PipelineReadAheadDirectAcc(u32 uController, u32 uBlkAddr, u8 uPageAddr, u8 uPageSize, u32 *pData);

void ONENAND_SetMemCfgTest(u32 Controller, u32 uData1, u32 uData2);

#ifdef __cplusplus
}
#endif

#endif //__ONENAND_H__



