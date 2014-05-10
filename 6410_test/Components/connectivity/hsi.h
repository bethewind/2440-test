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
*	File Name : mipi_hsi.h
*  
*	File Description : This file declares prototypes of MIPI HSI API funcions and defines some values.
*
*	Author : Haksoo,Kim
*	Dept. : AP Development Team
*	Created Date : 2007/03/03
*	Version : 0.1 
* 
*	History
*	- Created(Haksoo,Kim 2007/03/03)
*  
**************************************************************************************/
 
#ifndef __HSI_H__
#define __HSI_H__


//=====================================================================================
// definitions

typedef enum
{
	HSI_INT_MODE, HSI_DMA_MODE
} HSI_OP_MODE;

typedef enum
{
	HSI_STREAM_MODE, HSI_FRAME_MODE
} HSI_FR_MODE;

typedef enum
{
	HSI_BURST_CHID_MODE, HSI_SINGLE_CHID_MODE
} HSI_CHID_MODE;

typedef enum
{
	HSI_CHID_0BIT, HSI_CHID_1BIT, HSI_CHID_2BIT, HSI_CHID_3BIT
} HSI_CHID_BITS;

typedef enum
{
	HSI_CHID_0, HSI_CHID_1, HSI_CHID_2, HSI_CHID_3, HSI_CHID_4, HSI_CHID_5, HSI_CHID_6, HSI_CHID_7
} HSI_CHID_VALUE;

typedef enum
{
	HSI_TXFIFO_DEPTH	= 32,
	HSI_RXFIFO_DEPTH	= 64
} HSI_FIFO_DEPTH;

typedef enum
{
	TXHOLD_TIMEOUT 		= 0x10,
	TXIDLE_TIMEOUT 		= 0x08,
	TXREQ_TIMEOUT		= 0x04,
	TX_BREAKFRAME_END	= 0x02,
	TXFIFO_EMPTY		= 0x01
} HSI_TX_INTERRUPT;

typedef enum
{
	RX_BREAK_DONE		= 0x80,
	RX_ADDED_CLOCK		= 0x40,
	RX_MISSED_CLOCK		= 0x20,
	RXACK_TIMEOUT		= 0x10,
	RX_BREAKFRAME_ERR	= 0x08,
	RX_DONE				= 0x04,
	RXFIFO_TIMEOUT		= 0x02,
	RXFIFO_FULL			= 0x01
} HSI_RX_INTERRUPT;

typedef enum
{
	HSI_TIMER_OFF, HSI_TIMER_ON
} HSI_TIMER_ONOFF;

typedef enum
{
	HSI_DREQ_FULL		= 0,
	HSI_DREQ_4WORDS		= 1,
	HSI_DREQ_8WORDS		= 2,
	HSI_DREQ_16WORDS	= 3,
} HSI_DMA_RX_TRESH_VAL;

#define HSI_TEST_COUNT		(HSI_RXFIFO_DEPTH*3+8)

//=====================================================================================
// prototypes of API functions
void HSI_SetPort(void);
void HSI_DisableTxDma(void);
void HSI_DisableRxDma(void);
void HSI_SetForTx(HSI_OP_MODE eOpMode, HSI_FR_MODE eFrMode, HSI_CHID_MODE eChidMode, HSI_CHID_BITS eChidBits, HSI_CHID_VALUE eChidValue);
void HSI_SetForRx(HSI_OP_MODE eOpMode, HSI_FR_MODE eFrMode, HSI_CHID_MODE eChidMode, HSI_CHID_BITS eChidBits);
void __irq HSI_Tx_ISR( void);
void __irq HSI_Rx_ISR( void);
void __irq HSI_Dma_ISR(void);
void HSI_SetDmaForTx(u32 uCount);
void HSI_SetDmaForRx(u32 uCount);
void HSI_SetBreakFrameTx(void);
void HSI_ClearBreakFrameTx(void);
void HSI_UnmaskTxInt(u32 uInt);
void HSI_UnmaskRxInt(u32 uInt);
void HSI_WakeupByInt(void);

//=====================================================================================
// extern variables
//extern 

#endif	//#ifndef __HSI_H__
