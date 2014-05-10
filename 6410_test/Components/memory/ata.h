/**************************************************************************************
* 
*	Project Name : S3C6400 Validation
*
*	Copyright 2006 by Samsung Electronics, Inc.
*	All rights reserved.
*
*	Project Description :
*		This software is only for validating functions of the S3C6400.
*		Anybody can use this software without our permission.
*  
*--------------------------------------------------------------------------------------
* 
*	File Name : ata.h
*  
*	File Description : This file defines the register access function
*						and declares prototypes of adcts funcions
*
*	Author : Sunil,Roe
*	Dept. : AP Development Team
*	Created Date : 2007/1/26
*	Version : 0.1 
* 
*	History
*	- Created(Sunil,Roe 2007/1/26)
*  
**************************************************************************************/

#ifndef __ATA6400_H__
#define __ATA6400_H__

#ifdef __cplusplus
extern "C" 
{
#endif

#include <stdio.h>

#define	ENABLE					1
#define	DISABLE					0

#define	MASK					1
#define	UNMASK					0

#define	RESET					1
#define	NORESET				0

#define ATA_INT_NUM			5
#define	ATA_CONNUM			1

#define 	ATA_CON0				0
//#define 	ATA_CON1			1

#define ATA_BASE	(CFCON_BASE+0x1900)

#define ATA_SECTORSIZE				512

typedef enum ATA_SLOT_SEL
{
	eATA_MASTER	=0,
	eATA_SLAVE		=1,
}
eATA_SLOT_SEL;


typedef enum ATA_MODE_6400
{
	eATA_MODE_PIO		= 0,
	eATA_MODE_PIOCPU	= 0,
	eATA_MODE_PIODMA	= 1,
	eATA_MODE_UDMA	= 2,
	eATA_MODE_NONE	= 3,
}
eATA_MODE_6400;

typedef enum ATA_PIOMODE
{
	eATA_PIO0	= 0,
	eATA_PIO1	= 1,
	eATA_PIO2	= 2,
	eATA_PIO3	= 3,
	eATA_PIO4	= 4,	
}
eATA_PIOMODE;

typedef enum ATA_UDMAMODE
{
	eATA_UDMA0	= 0,
	eATA_UDMA1	= 1,
	eATA_UDMA2	= 2,
	eATA_UDMA3	= 3,
	eATA_UDMA4	= 4,	
}
eATA_UDMAMODE;

typedef enum ATA_INT_SRC_6400
{
	eATA_INT_XFER_DONE	= 0,
	eATA_INT_UDMA_HOLD	= 1,
	eATA_INT_IRQ			= 2,
	eATA_INT_TBUF_FULL		= 3,
	eATA_INT_SBUF_EMPTY	= 4,
}
eATA_INT_SRC_6400;

typedef enum ATA_Id
{
	eATA_CONTROL			= 0,		// ATA enable and clock down status
	eATA_STATUS			= 1,		// ATA status
	eATA_COMMAND			= 2,		// ATA command
	eATA_SWRST			= 3,		// ATA software reset
	eATA_IRQ				= 4,		// ATA interrupt sources
	eATA_IRQ_MASK			= 5,		// ATA interrupt mask
	eATA_CFG				= 6,		// ATA configuration for ATA interface
	rReserved1				= 7,     	
	rReserved2				= 8,     	
	rReserved3				= 9,     	
	rReserved4				= 10,     	
	eATA_PIO_TIME			= 11,		// ATA PIO timing
	eATA_UDMA_TIME		= 12,		// ATA UDMA timing
	eATA_XFR_NUM			= 13,		// ATA transfer number
	eATA_XFR_CNT			= 14,		// ATA current transfer count
	eATA_TBUF_START		= 15,		// ATA start address of track buffer
	eATA_TBUF_SIZE		= 16,		// ATA size of track buffer
	eATA_SBUF_START		= 17,		// ATA start address of source buffer
	eATA_SBUF_SIZE		= 18,		// ATA size of source buffer
	eATA_CADR_TBUF		= 19,		// ATA current write address of track buffer
	eATA_CADR_SBUF		= 20,		// ATA current read address of source buffer
	eATA_PIO_DTR			= 21,		// ATA PIO device data register
	eATA_PIO_FED			= 22,		// ATA PIO device Feature/Error register
	eATA_PIO_SCR			= 23,		// ATA PIO sector count register
	eATA_PIO_LLR			= 24,		// ATA PIO device LBA Low register
	eATA_PIO_LMR			= 25,		// ATA PIO device LBA middle register
	eATA_PIO_LHR			= 26,		// ATA PIO device LBA high register
	eATA_PIO_DVR			= 27,		// ATA PIO device register
	eATA_PIO_CSD			= 28,		// ATA PIO device command/status register
	eATA_PIO_DAD			= 29,		// ATA PIO device control/alternate status register
	eATA_PIO_READY		= 30,		// ATA PIO read data from device data register
	eATA_PIO_RDATA		= 31,		// ATA PIO read data from device data register
	rReserved5				= 32,     	
	rReserved6				= 33,     	
	rReserved7				= 34,     	
	rReserved8				= 35,     	
	eBUS_FIFO_STATUS		= 36,		// ATA internal AHB FIFO status
	eATA_FIFO_STATUS		= 37,		// ATA internal ATA FIFO status	
}
eATA_Id;

/*---------------------------------- Init Function ---------------------------------*/
/*---------------------------------- ATA_CONTROL Register ---------------------------------*/
/*---------------------------------- ATA_STATUS Register ---------------------------------*/
#define STATUS_DEVICE_BUSY	0x80
#define STATUS_DATA_REQUEST	0x58
#define STATUS_ERR				0x1
typedef enum ATA_XFR_STATUS
{
	eATA_XFR_STATUS_DEVICE_BUSY		= 0x80,
	eATA_XFR_STATUS_DATA_REQUEST	= 0x58,
	eATA_XFR_STATUS_ERR				= 0x01,
}
eATA_XFR_STATUS;

/*---------------------------------- ATA_COMMAND Register ---------------------------------*/
typedef enum ATA_XFR_CMD
{
	eATA_XFR_CMD_STOP		= 0,
	eATA_XFR_CMD_START		= 1,
	eATA_XFR_CMD_ABORT		= 2,
	eATA_XFR_CMD_CONTINUE	= 3,
}
eATA_XFR_CMD;
/*---------------------------------- ATA_SWRST Register ---------------------------------*/
/*---------------------------------- ATA_IRQ Register ---------------------------------*/
/*---------------------------------- ATA_IRQ_MASK Register ---------------------------------*/
typedef enum ATA_IRQ_SRC
{
	eATA_IRQ_SBUF_EMPTY		= 0x10,
	eATA_IRQ_TBUF_FULL		= 0x08,
	eATA_IRQ_ATADEV_IRQ		= 0x04,
	eATA_IRQ_UDMA_HOLD		= 0x02,
	eATA_IRQ_XFR_DONE		= 0x01,
	eATA_IRQ_ALL				= 0x1f,
}
eATA_IRQ_SRC;

/*---------------------------------- ATA_CFG Register ---------------------------------*/
typedef enum ATA_ENDIAN
{
	eATA_ENDIAN_LITTLE	= 0,
	eATA_ENDIAN_BIG		= 1,
}
eATA_ENDIAN;

typedef enum ATA_DMA_DIR
{
	eATA_DMA_READ_DATA	= 0,
	eATA_DMA_WRITE_DATA	= 1,
}
eATA_DMA_DIR;

/*---------------------------------- ATA_PIO_TIME Register ---------------------------------*/
/*---------------------------------- ATA_UDMA_TIME Register ---------------------------------*/
/*---------------------------------- ATA_XFR_NUM Register ---------------------------------*/


typedef struct ATA_Inform
{
	eATA_MODE_6400	eAtaMode;
	u32				uMaxMultiple;
	u32				uCurrentMultiple;
	u32				uMaxSectors;
	eATA_PIOMODE	eMaxPioMode;
	eATA_PIOMODE	eCurrentPioMode;
	eATA_UDMAMODE	eMaxUdmaMode;
	eATA_UDMAMODE	eCurrentUdmaMode;
	u8				bIsXferDone;
	u8				bIsDone;

	u32*			puATAWriteBuf;
	u32*			puATAReadBuf;
/*
	u32		PCCARD_DATA;
	u32		PCCARD_FEATURE;
	u32		PCCARD_SECTOR;
	u32		PCCARD_LOWLBA;
	u32		PCCARD_MIDLBA;
	u32		PCCARD_HIGHLBA;
	u32		PCCARD_DEVICE;
	u32		PCCARD_STATUS;
	u32		PCCARD_COMMAND;
*/	
}
ATA_oInform;

u8	ATA_Init(u8, u8);
void ATA_InitBuffer(u8);
/*---------------------------------- Functions for Implement ATA Test functions ---------------------------------*/
u8 ATA_WriteBlocks(u8 ucCon, u32 uStBlock, u32 uBlocks, u32 uBufAddr, eATA_SLOT_SEL eSlotsel);
u8 ATA_ReadBlocks(u8 ucCon, u32 uStBlock, u32 uBlocks, u32 uBufAddr, eATA_SLOT_SEL eSlotsel);
void ATA_IdentifyDevice(u8 ucCon,eATA_SLOT_SEL eSlotsel);
void ATA_IdentifyDeviceSlotSel(u8 ucCon,eATA_SLOT_SEL eSlotsel);
void ATA_TestResetAll(u8 ucCon);
void ATA_SetPIOMode(u8 ucCon, eATA_PIOMODE ePioMode, eATA_SLOT_SEL eSlotSel);
void ATA_ChangeATAMode(u8 ucCon);
void ATA_SetUdmaMode(u8 ucCon, eATA_UDMAMODE eUdmaMode, eATA_SLOT_SEL eSlotSel);
u8 ATA_FindInterruptRequest(u8 ucCon, u32* uNthBit);
void ATA_ClearPending(u8 ucCon, u32 uSrcInt);
void ATA_ClearAllInterrupt(u8 ucCon);
u8 ATA_IsDmaDone(u8 ucCon);
u8 ATA_IsTBufFullContinue(u8 ucCon);
u8 ATA_IsSBufEmptyContinue(u8 ucCon);
u8 ATA_StartWritingBlocks(u8 ucCon, u32 uStBlock, u32 uBlocks, u32 uBufAddr);
u8 ATA_StartReadingBlocks(u8 ucCon, u32 uStBlock, u32 uBlocks, u32 uBufAddr);
u8 ATA_StartWritingSectors(u8 ucCon, u32 uStBlock, u32 uBlocks, u32 uBufAddr, eATA_SLOT_SEL eSlotsel);
u8 ATA_StartReadingSectors(u8 ucCon, u32 uStBlock, u32 uBlocks, u32 uBufAddr, eATA_SLOT_SEL eSlotSel);
void ATA_SetTransferCommand(u8 ucCon, eATA_XFR_CMD eXfrCmd);
/*---------------------------------- APIs of ATA_CONTROL Registers ---------------------------------*/
void ATA_SetEnable(u8 ucCon, u8 ucEnFlag);
u8 *ATA_GetModeName(eATA_MODE_6400);

void ATA_TestUDmaMode_Int_for_EBI(void);
void ATA_TestPDmaMode_Int_for_EBI(void);
u8 ATA_OpenMedia(u8 ucCon, eATA_MODE_6400 eAtaMode, eATA_SLOT_SEL eSlotsel);
u8 ATA_CloseMedia(u8);
void ATA_SetRst(u8 , u8 );
#ifdef __cplusplus
}
#endif


#endif // __ATA6400_H__

