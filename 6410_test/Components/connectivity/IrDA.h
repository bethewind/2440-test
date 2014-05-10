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
*	File Name : irDA.h
*  
*	File Description : This file declares prototypes of IrDA API funcions.
*
*	Author : Woojin.Kim
*	Dept. : AP Development Team
*	Created Date : 2007/01/25
*	Version : 0.1 
* 
*	History"
*	- Created(Woojin.Kim 2007/01/25)
*  
**************************************************************************************/

#ifndef __IrDA_H_
#define __IrDA_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "sfr6410.h"
#include "def.h"

enum IrDA_SFR
{
	rIrDA_CNT	=IRDA_BASE+0x00,
	rIrDA_MDR	=IRDA_BASE+0x04,
	rIrDA_CNF	=IRDA_BASE+0x08,
	rIrDA_IER	=IRDA_BASE+0x0C,
	rIrDA_IIR	=IRDA_BASE+0x10,
	rIrDA_LSR	= IRDA_BASE+0x14,
	rIrDA_FCR	= IRDA_BASE+0x18,
	rIrDA_PLR	= IRDA_BASE+0x1C,
	rIrDA_RBR	= IRDA_BASE+0x20,
	rIrDA_TXNO	= IRDA_BASE+0x24,
	rIrDA_RXNO	= IRDA_BASE+0x28,
	rIrDA_TXFLL	= IRDA_BASE+0x2C,
	rIrDA_TXFLH	= IRDA_BASE+0x30,
	rIrDA_RXFLL	= IRDA_BASE+0x34,
	rIrDA_RXFLH	= IRDA_BASE+0x38,
	rIrDA_INTCLR	= IRDA_BASE+0x3C
};


typedef enum eIrDA_Error
{
	No_Error 				= 0x00,
	CRC_Error			= 0x01,
	PHY_Error			= 0x02,
	Frame_Error 			= 0x04,
	RxOverRun_Error		= 0x08,
	TxUnderRun_Error		= 0x10
}eIrDA_Error;


typedef enum eIrDA_ClkSrc
{
	Ext48Mhz 	= 1,
	MPLL		= 2,
	EPLL		= 3
	
}eIrDA_ClkSrc;


/*
#define MemoryRx		(CODEC_MEM_ST+0x1000)
#define MemoryTx		(CODEC_MEM_ST+0x2000)
#define MemoryCk		(CODEC_MEM_ST+0x3000)
#define MemoryCk1	(CODEC_MEM_ST+0x4000)
*/

#define MemoryRx		(0x51100000)
#define MemoryTx		(0x51200000)
#define MemoryCk		(0x51300000)
#define MemoryCk1	(0x51400000)



#define IrDA_FIFOSIZE	1		// 0 : 16, 1 : 64 bytes
#define IrDA_FIFOENB	1		// 1 : Enbale IrDA Tx/Rx FIFO

//====================================================================
// IrDA Contol Register (IrDA_CNT)
// This value only valid MIR mode.

#define	IrDA_LOOP_MODE		0	// 0 : Normal, 1 : Loop mode for core test
#define	IrDA_SEND_SIP		1	// 1 : Send SIP pulse

//====================================================================
// IrDA Mode Definition Register (IrDA_MDR)

// -----------------------
// IrDA Test Configuration
// -----------------------
#define	PREAMBLE_VALUE		0
// number of preembles (FIR mode only)
// PREAMBLE		number of preambles
// 00b			16						 ->> fixed by spec
// 01b			04                                     ->> doesn't work well
// 10b			08
// 11b			32

#define	STARTFLAG_VALUE	3                 
// number of start flags (MIR mode only)
// START_FLAG	number of start flags
// minimum value 3 required !!!

#define	RXFL_VALUE			0xFFFF
#define	RXFL_DMAVALUE		0xFFFF
// Number of RX MAX Number


#define 	IrDAtoDMA_RX_SIZE		16	
#define 	DMA_RxBurstSize			3		// 3: 16burst, 4: 32burst, 5: 64burst
#define	RXTR_VALUE				0
#define	RXTR_DMAVALUE			1
// RX FIFO trigger level	
// 00b			01|01
// 01b			04|16
// 10b			08|32
// 11b			14|56

#define IrDA_TxBUFLEN		0xFFFF		//Maximum about 64 reconmand 16
//#define IrDA_TxBUFLEN		16		//Maximum about 64 reconmand 16
#define	TXFL_VALUE		0xffff
//#define	TXFL_VALUE		128
#define	TXFL_DMAVALUE		2048
// Number of TX data (Payload Size)

#define	TXTR_VALUE		2
// TX FIFO trigger level
// 00b			Reserved
// 01b			12|48
// 10b			08|32
// 11b			02|08

#if 0 //0: IrDA_DMAnonFIFO,   1:IrDA_DMAFIFOTEST 
#define	TXTR_DMAVALUE			1
#define 	DMA_TxBurstSize			4  		// 3: 16burst, 4: 32burst, 5: 64burst,  must DMA_TxBurstSize > IrDA FIFO txNo Size(64 - FIFO Threshold level)
#define 	DMAtoIrDA_TX_SIZE		128
#else
#define	TXTR_DMAVALUE			2
#define   DMA_TxBurstSize		4		// 3: 16burst, 4: 32burst, 5: 64burst,  must DMA_TxBurstSize > IrDA FIFO txNo Size(64 - FIFO Threshold level)
#define 	DMAtoIrDA_TX_SIZE		32
#endif




void IrDA_Reset(void);
void IrDA_SetPort(u8 SelPort);
void IrDA_ReturnPort(void);
void IrDA_ClkSrc(eIrDA_ClkSrc ClkSrc);
void IrDA_Init(u32 uMODE, u32 uPREAMBLE, u32 uSTARTFLAG, u32 uRXTXFL, u32 uRXTXTRIG);
void IrDA_IntClear(void);
void IrDA_EnRx(u32 uEn);
void IrDA_EnTx(u32 uEn);
void IrDA_SetIER(u32 LB, u32 ErrInd, u32 TxUnder, u32 LBdetect, u32 RxOver, u32 LBread, u32 TxFbelow, u32 RxFover);
u32 IrDA_ReadLSR(void);

void IrDA_EnInt(u32 uEn);
void IrDA_EnDMA(u32 uTxRx);

void Reg_Dump(void);

void __irq Isr_IrDA_Int_Rx(void);
eIrDA_Error __Isr_IrDA_Sub_RxErr(void);
void __irq Isr_IrDA_Int_Tx(void);



void Irda_Fifo_Int_Rx(u32 MODE, u32 PREAMBLE, u32 STARTFLAG, u32 RXFL, u32 RXTRIG);
void Irda_Fifo_Int_Tx(u32 MODE,u32 PREAMBLE,u32 STARTFLAG,u32 TXFL,u32 TXTRIG);


void __irq Isr_IrDA_Dma_RxDone(void);
void Init_Irda_Dma_Rx(void);
void __irq Isr_IrDA_Dma_TxDone(void);
void Init_Irda_Dma_Tx(void);

#ifdef __cplusplus
}
#endif

#endif/*__IrDA_H_*/
