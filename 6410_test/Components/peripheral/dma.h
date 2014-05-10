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
*	File Name : Dma.h
*  
*	File Description : This file implements the API functons for DMA controller.
*
*	Author : Wonjoon Jang
*	Dept. : AP Development Team
*	Created Date : 2006/12/29
*	Version : 0.1 
* 
*	History
*	- Created(wonjoon.jang  2007/12/29)
*  
**************************************************************************************/
#ifndef __DMA_H__
#define __DMA_H__


#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

#include "def.h"

#define 	ONENAND_LLI_BASE		(0x0C000000)

typedef enum
{
	DMA0, DMA1, SDMA0, SDMA1
}  DMA_UNIT;

typedef enum
{
	DMA_A = 1<<0,
	DMA_B = 1<<1,
	DMA_C = 1<<2,
	DMA_D = 1<<3,
	DMA_E = 1<<4,
	DMA_F = 1<<5,
	DMA_G = 1<<6,
	DMA_H = 1<<7,
	DMA_ALL= 0xFF
} DMA_CH;

typedef enum
{
	eINCREMENT	= (u32) 0,
	eFIX		= (u32) 1,
} INC_TYPE;

typedef enum
{
	BYTE		= (u32) 1,
	HWORD		= (u32) 2,
	WORD		= (u32) 4
} DATA_SIZE;

typedef enum
{
	SINGLE		= (u32) 0,
	BURST4		= (u32) 1,
	BURST8		= (u32) 2,
	BURST16	= (u32) 3,
	BURST32	= (u32) 4,
	BURST64	= (u32) 5,
	BURST128	= (u32) 6,
	BURST256	= (u32) 7
} BURST_MODE;


typedef enum
{
	DMA0_UART0_0		= 0x00,	// DMA0, SDMA0
	DMA0_UART0_1		= 0x01,
	DMA0_UART1_0		= 0x02,
	DMA0_UART1_1		= 0x03,
	DMA0_UART2_0		= 0x04,
	DMA0_UART2_1		= 0x05,
	DMA0_UART3_0		= 0x06,
	DMA0_UART3_1		= 0x07,
	DMA0_PCM0_TX		= 0x08,	
	DMA0_PCM0_RX		= 0x09,
	DMA0_I2S0_TX		= 0x0a,	//	10
	DMA0_I2S0_RX		= 0x0b,	//	11
	DMA0_SPI0_TX		= 0x0c,	//	12
	DMA0_SPI0_RX		= 0x0d,	//	13
	DMA0_HSI_I2SV40_TX		= 0x0e,	//	14
	DMA0_HSI_I2SV40_RX		= 0x0f,	//	15

	DMA1_PCM1_TX		= 0x20,	// DMA1, SDMA1
	DMA1_PCM1_RX		= 0x21,
	DMA1_I2S1_TX		= 0x22,
	DMA1_I2S1_RX		= 0x23,
	DMA1_SPI1_TX		= 0x24,
	DMA1_SPI1_RX		= 0x25,
	DMA1_AC_PCMout	= 0x26,
	DMA1_AC_PCMin		= 0x27,
	DMA1_AC_MICin		= 0x28,
	DMA1_PWM			= 0x29,
	DMA1_IrDA			= 0x2a,	//	10
	DMA1_EXTERNAL		= 0x2b,	//	11
	DMA1_GIB			= 0x2c,	//	12
//	DMA1_SW_REQ1		= 0x2d,	//	13	For Test (by Woojin)
	SDMA1_SECU_RX		= 0x2e,	// 	14	SDMA1
	SDMA1_SECU_TX		= 0x2f,	// 	15	SDMA1

	MEM				= 0x14,
	DMA1_NAND_TX		= 0x15,
	DMA1_NAND_RX		= 0x16,
	DMA0_OND_TX		= 0x17,
	DMA0_OND_RX		= 0x18,

	
	DMA0_MODEM_TX	= 0x32,
	DMA0_MODEM_RX	= 0x33,	
	DMA1_MODEM_TX	= 0x34,
	DMA1_MODEM_RX	= 0x35,	


	DMA_MODEM			= 0x62,

	SOFTWARE			= 0x63
} DREQ_SRC;

typedef enum
{
	DEMAND, HANDSHAKE
} DMA_HS_MODE;

typedef enum
{
	AHB_MASTER1 = 0,   // Memory Bus...
	AHB_MASTER2 = 1    // Peripheral Bus...
} DMA_AHB;

typedef struct
{
	u32 m_uChAddr;
	u32 m_uBaseAddr;
} DMAC;




void DMAC_InitCh(DMA_UNIT eUnit, DMA_CH eCh, DMAC *sCh);
void DMAC_Close(DMA_UNIT eUnit, DMA_CH eCh, DMAC *sCh);
void DMACH_Setup(
	DMA_CH eCh, u32 uLLIAddr, u32 uSrcAddr, u8 bSrcFixed, u32 uDstAddr, u8 bDstFixed, DATA_SIZE eDataSz, u32 uDataCnt,
	DMA_HS_MODE eOpMode, DREQ_SRC eSrcReq, DREQ_SRC eDstReq, BURST_MODE eBurstMode, DMAC *sCh);

void DMACH_Stop(DMAC *sCh);
u8 DMACH_IsTransferDone(DMAC *sCh);
u32 DMAC_IntStatus(DMAC *sCh);
u32 DMACH_Configuration(DMAC *sCh);
void DMACH_ClearIntPending(DMAC *sCh);
void DMACH_ClearErrIntPending(DMAC *sCh);
void DMACH_Start(DMAC *sCh);
DMA_CH DMACH_GetChannelNumber(DMAC *sCh);

void DMACH_AddrSetup(DMA_CH eCh,  u32 uSrcAddr, u32 uDstAddr, DMAC *sCh);
u32 DMACH_ReadSrcAddr(DMAC * sCh);
u32 DMACH_ReadDstAddr(DMAC * sCh);
void DMACH_WriteSrcAddr(DMAC *sCh, u32 uSrcAddr);
void DMACH_WriteDstAddr(DMAC *sCh, u32 uDstAddr);
//void DMACH_WriteTransferSize(DMAC *sCh, u32 uSize);
	
void DMACH_SoftBurstReq(DMAC *sCh, DREQ_SRC eSrcReq);
void DMACH_SoftBurstLastReq(DMAC *sCh, DREQ_SRC eSrcReq);
void DMACH_SetTransferSize(u32 uSize, DMAC *sCh);

u32 DMAT_GetBurstSize(BURST_MODE );

void __irq Dma0Done(void);
void __irq Dma1Done(void);
void __irq SDma0Done(void);
void __irq SDma1Done(void);

u8 CompareDMA(u32, u32, DATA_SIZE, u32);
void DMACH_ClearIntPendingrb1004(DMAC *sCh, u32 uPending);

#ifdef __cplusplus
}
#endif

#endif //__DMA_H__



