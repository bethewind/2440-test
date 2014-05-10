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
*	File Name : hs_spi.c
*  
*	File Description : This file implements the API functon for High Speed SPI.
*
*	Author : Youngbo, Song
*	Dept. : AP Development Team
*	Created Date : 2007/01/29
*	Version : 0.1 
* 
*	History
*	- Created(Youngbo,Song 2007/01/29)
*  
**************************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "def.h"
#include "option.h"
#include "library.h"
#include "sfr6410.h"
#include "system.h"
#include "intc.h"
#include "gpio.h"
#include "sysc.h"
#include "spi.h"

SPI_channel SPI_current_channel[2];

#if 1 // LCD SPI Setting Test Jungil 
enum
{
	CH_CFG=00,
	CLK_CFG=4,
	MODE_CFG=8,
	SLAVE_SLECTION_REG=0xC,
	SPI_INT_EN=0x10,
	SPI_STATUS=0x14,
	SPI_TX_DATA=0x18,
	SPI_RX_DATA=0x1c,
	PACKET_COUNT_REG=0x20,
	PENDING_CLR_REG=0x24,
	SWAP_CFG=0x28,
	FB_CLK_SEL=0x2c
};

#define SPI1Out32(x,y) Outp32(SPI1_BASE+x,y);
#define SPI1Out8(x,y) Outp8(SPI1_BASE+x, y);
#define SPI1In32(x,y) Inp32_SPI(SPI1_BASE+x,y);
#define CPOL 3
#define CPHA 2
#define ENCLK 8
#define CH_TRAN_SIZE 30
#define AUTO_N_MANUAL 1
#define PRESCALER_V 0xff
#endif

//////////
// File Name : SPI_reset
// File Description : This function reset certain spi channel.
// Input : SPI_channel 
// Output : NONE.
// Version : 
void SPI_reset( SPI_channel * ch ) {
	// Register Clear manually
	Outp32( &ch->m_cBase->clk_cfg, 0);
	Outp32( &ch->m_cBase->mode_cfg, 0 );
	Outp32( &ch->m_cBase->slave_sel, 1 );
	Outp32( &ch->m_cBase->packet_count, 0 );
	Outp32( &ch->m_cBase->pending_clr, 0 );
	Outp32( &ch->m_cBase->feedback_clk, 0 );

	// reset with clock delay..
	Outp32( &ch->m_cBase->ch_cfg, Inp32(&ch->m_cBase->ch_cfg) & ~(0x3F) ); // clean register
	Outp32( &ch->m_cBase->ch_cfg, Inp32(&ch->m_cBase->ch_cfg) | (1<<5) );
	Delay(10);

	// release reset signal.
	Outp32( &ch->m_cBase->ch_cfg, Inp32(&ch->m_cBase->ch_cfg) & ~(1<<5) );
}

//////////
// File Name : SPI_channel_Init
// File Description : This function reset certain spi channel.
// Input : SPI_channel 
// Output : NONE.
// Version : 
SPI_channel* SPI_channel_Init( int channel ) {
	SPI_channel* ch = &SPI_current_channel[channel];
	memset ( (void*)ch, 0, sizeof(SPI_channel) );
	ch->m_ucChannelNum = channel;
	if ( channel == 0 ) {
		ch->m_cBase = (SPI_SFR*)SPI0_BASE;
		ch->m_ucIntNum = NUM_SPI0;
		ch->m_fDMA= SPI_DMADoneChannel0;
		ch->m_fISR = SPI_interruptChannel0;
#ifdef SPI_NORMAL_DMA
		ch->m_ucDMACon = DMA0;
		SYSC_SelectDMA( eSEL_SPI0_TX, 1 );	// normal DMA setting.
		SYSC_SelectDMA( eSEL_SPI0_RX, 1 );	// normal DMA setting.
#else
		ch->m_ucDMACon = SDMA0;
		SYSC_SelectDMA( eSEL_SPI0_TX, 0 );	// secure DMA setting.
		SYSC_SelectDMA( eSEL_SPI0_RX, 0 );	// secure DMA setting.
#endif
	}
	else if ( channel == 1 ) {
		ch->m_cBase = (SPI_SFR*)SPI1_BASE;
		ch->m_ucIntNum = NUM_SPI1;

		ch->m_fDMA = SPI_DMADoneChannel1;
		ch->m_fISR = SPI_interruptChannel1;
#ifdef SPI_NORMAL_DMA
		ch->m_ucDMACon = DMA1;
		SYSC_SelectDMA( eSEL_SPI1_TX, 1 );	// normal DMA setting.
		SYSC_SelectDMA( eSEL_SPI1_RX, 1 );	// normal DMA setting.
#else
		ch->m_ucDMACon = SDMA1;
		SYSC_SelectDMA( eSEL_SPI1_TX, 0 );	// secure DMA setting.
		SYSC_SelectDMA( eSEL_SPI1_RX, 0 );	// secure DMA setting.

#endif
	}
	else {
		Assert(0);
	}
	Outp32(&ch->m_cBase->slave_sel, Inp32(&ch->m_cBase->slave_sel) | (1<<0) );		// Chip selection OFF - active LOW.
	SPI_GPIOPortSet(channel);	// channel GPIO setting.
	return ch;
}


void SPI_SetDriveStrength( u8 cStrength ) {
	if(cStrength>3) {
		cStrength=3;
	}
	Outp32( 0x7F0081A0, (Inp32(0x7F0081A0 ) & ~(0x3<<28)) | (cStrength<<28) );	// ALL.
	Outp32( 0x7F0081A0, (Inp32(0x7F0081A0 ) & ~(0x3<<18)) | (cStrength<<18) );	// SPI1 clock and MMC2 Clock
}



//////////
// File Name : SPI_open
// File Description : This function create certain spi channel.
// Input : channel number, SPI_clock_mode, CPOL, CPHA, SPI_clock_selection, clock frequency, channel size, bus size, DMA Type
// Output : SPI_channel
// Version : 
SPI_channel * SPI_open( u8 channel, SPI_clock_mode master, SPI_CPOL cpol, SPI_CPHA cpha, SPI_clock_selection clock,
						u32 clk_freq, SPI_transfer_data_type ch_size, SPI_transfer_data_type bus_size, SPI_DMA_type dma_type )
{
	u8	ucPrescaler;
	SPI_channel* ch = SPI_channel_Init(channel);
	SPI_SetDriveStrength(2);
	SPI_reset( ch );
	// channel.
	ch->m_cChannelNum = channel;
	
	// 1. SPI Channel setting.
	ch->m_eClockMode = master;
	ch->m_eCPOL = cpol;
	ch->m_eCPHA = cpha;

	// 2. Clock Config
	// to do make clock source.
	if ( clock == SPI_USB_CLK ) {
		ucPrescaler = (u8)(((float)48000000)/(float)2/(float)clk_freq - (float)1);
		// USB Phy On.
		Outp32( 0x7C100000, 0 );
		Outp32( 0X7C100008, 3 );
		Delay(10);
		Outp32( 0X7C100008, 0 );
		// youngbo.song Test.
		*((volatile u32*)0x7E00F900) |= (1<<16);	// USB Phi ON.
	}
	else if ( clock == SPI_EPLL_CLK ) {
		ucPrescaler = (u8)(((float)100000000)/(float)2/(float)clk_freq - (float)1);

		// EPLL Setting.
		SYSC_SetLockTime( eEPLL, 300);
		SYSC_CtrlCLKOUT(eCLKOUT_EPLLOUT, 0);
		SYSC_SetPLL(eEPLL, 100, 3, 2, 0);		// EPLL => 100MHz
		Outp32( 0x7E00F01C, Inp32(0x7E00F01C) | 7 );
	}
	else {	//( clock == SPI_PCLK )
		ucPrescaler = (u8)(((float)g_PCLK)/(float)2/(float)clk_freq - (float)1);
	}

	ch->m_eClockSource = clock;
	ch->m_cPrescaler = ucPrescaler;
	
	// 3. Mode Config
	ch->m_eChSize = ch_size;
	ch->m_uTraillingCnt = (u16)0x3ff;	// default Trailling count setting
	ch->m_eBusSize = bus_size;
	ch->m_ucRxLevel = (u8)0x20;		// default setting.
	ch->m_ucTxLevel = (u8)0x20;		// default setting.
	ch->m_eDMAType = dma_type;

	// default swap mode setting.
	SPI_setSwapMode( ch, SPI_NO_SWAP, SPI_NO_SWAP);
	
	// default feed back clock setting.
	SPI_setFeedbackClock( ch, SPI_9NS_DELAY );

	ch->m_bIsAutoChipSelection = TRUE;
	return ch;
}


//////////
// File Name : SPI_setSwapMode
// File Description : This function initializes rx/tx swap mode.
// Input : SPI_channel, SPI_feedbackClock
// Output : NONE
// Version : 
void SPI_setSwapMode( SPI_channel * ch, SPI_swapMode rx_swap, SPI_swapMode tx_swap ) {
	// rx swap mode setting.
	if ( rx_swap == SPI_NO_SWAP ) {
		ch->m_eRxSwap = SPI_NO_SWAP;
	}
	else if(rx_swap == SPI_EN_SWAP ) {	// default case.
		ch->m_eRxSwap = SPI_BYTE_SWAP;
	}
	else {
		ch->m_eRxSwap = rx_swap;
	}
	// tx swap mode setting.
	if ( tx_swap == SPI_NO_SWAP ) {
		ch->m_eTxSwap = SPI_NO_SWAP;
	}
	else if(tx_swap == SPI_EN_SWAP ) {	// default case.
		ch->m_eTxSwap = SPI_BYTE_SWAP;
	}
	else {
		ch->m_eTxSwap = tx_swap;
	}
	// swap mode SFR setting.
	Outp8( &ch->m_cBase->swap_config, (ch->m_eRxSwap << 4) | ch->m_eTxSwap );
}

//////////
// File Name : SPI_setFeedbackClock
// File Description : This function initializes a feedback clock.
// Input : SPI_channel, SPI_feedbackClock
// Output : NONE
// Version : 
void SPI_setFeedbackClock( SPI_channel* ch, SPI_feedbackClock clock ) {
	ch->m_eFeedbackClock = clock;
	Outp8( &ch->m_cBase->feedback_clk, clock );
}

//////////
// File Name : SPI_setTxRxTriggerLevel
// File Description : This function initialize FIFO trigger levels.
// Input : SPI_channel, rx trigger level, tx trigger level
// Output : NONE
// Version : 
void SPI_setTxRxTriggerLevel( SPI_channel * ch, u8 rxLevel, u8 txLevel ) {
	ch->m_ucRxLevel = rxLevel;		// rx trigger level
	ch->m_ucTxLevel = txLevel;		// tx trigger level
}

//////////
// File Name : SPI_setBasicRegister
// File Description : This function initializes a certain spi ch.
// Input : SPI_channel
// Output : NONE
// Version : 
void SPI_setBasicRegister( SPI_channel* ch ) {
	Outp32( &ch->m_cBase->ch_cfg,// (Inp32(&ch->m_cBase->ch_cfg)&~(0x7<<2))|			// Clean Register
									(ch->m_eClockMode<<4)|							// Master/Slave
									(ch->m_eCPOL<<3)|								// CPOL - active high/row
									(ch->m_eCPHA<<2) );								// CPHA - transfer format.

	Outp32( &ch->m_cBase->clk_cfg, (Inp32(&ch->m_cBase->clk_cfg) & ~(0x7ff))|			// clean register.
								(ch->m_eClockSource<<9) |							// Clock setting.
								( ( (ch->m_eClockMode==SPI_MASTER)?(1):(0) )<<8) |		// clock enable
								ch->m_cPrescaler);									// prescaler setting.

	Outp32( &ch->m_cBase->mode_cfg, (Inp32(&ch->m_cBase->mode_cfg)&(u32)(1<<31)) |	// clean register.
														(ch->m_eChSize<<29)|		// channel transfer size.
														(ch->m_uTraillingCnt<<19)|	// trailling count.
														(ch->m_eBusSize<<17)|		// bus transfer size.
														(ch->m_ucRxLevel<<11)|		// Rx trigger level
														(ch->m_ucTxLevel<<5)|		// Tx trigger level
														(ch->m_eDMAType<<0) );		// DMA type
}

//////////
// File Name : SPI_TxDMAInit
// File Description : Initialze DMA Channel for TX.
// Input : SPI_channel , Source Address, Destination Address, Transfer byte Size.
// Output : NONE.
// Version : 
void SPI_TxDMAInit ( SPI_channel * ch, u32 uSrcAddr, u32 uDstAddr, u32 uDataCnt) {
	DATA_SIZE dataSize;
	
	DMAC_InitCh((DMA_UNIT)ch->m_ucDMACon, DMA_A, &ch->m_sDMA);			//  to do.. another..
#ifdef SPI_NORMAL_DMA
	ch->m_ucIntNum = (ch->m_cChannelNum==0)?(NUM_DMA0):(NUM_DMA1);
#else
	ch->m_ucIntNum = (ch->m_cChannelNum==0)?(NUM_SDMA0):(NUM_SDMA1);
#endif
	INTC_SetVectAddr(ch->m_ucIntNum, ch->m_fDMA);
	INTC_Enable(ch->m_ucIntNum);

	// Interrupt Clear
	DMACH_ClearIntPending(&ch->m_sDMA);
	DMACH_ClearErrIntPending(&ch->m_sDMA);

	if ( ch->m_eBusSize == SPI_WORD ) {
		uDataCnt=(uDataCnt+3)>>2;
		dataSize = WORD;
	}
	else if ( ch->m_eBusSize == SPI_HWORD ) {
		uDataCnt=(uDataCnt+1)>>1;
		dataSize = HWORD;
	}
	else {
		dataSize = BYTE;
	}

	DMACH_Setup(DMA_A, 0x0,
		uSrcAddr, FALSE,
		(ch->m_ucChannelNum==0)?(0x7F00B018):(0x7F00C018), TRUE,
		dataSize, uDataCnt, HANDSHAKE,
		MEM,(ch->m_ucChannelNum==0)?(DMA0_SPI0_TX):(DMA1_SPI1_TX),
		(BURST_MODE)ch->m_eDMAType, &ch->m_sDMA);
}

//////////
// File Name : SPI_RxDMAInit
// File Description : Initialze DMA Channel for Rx.
// Input : SPI_channel , Source Address, Destination Address, Transfer byte Size.
// Output : NONE.
// Version : 
void SPI_RxDMAInit(SPI_channel* ch, u32 uSrcAddr, u32 uDstAddr, u32 uDataCnt) {
	DATA_SIZE dataSize;
	DMAC_InitCh((DMA_UNIT)ch->m_ucDMACon, DMA_B, &ch->m_sDMA);			//  to do.. another..
#ifdef SPI_NORMAL_DMA
	ch->m_ucIntNum = (ch->m_cChannelNum==0)?(NUM_DMA0):(NUM_DMA1);
#else
	ch->m_ucIntNum = (ch->m_cChannelNum==0)?(NUM_SDMA0):(NUM_SDMA1);
#endif
	INTC_SetVectAddr(ch->m_ucIntNum, ch->m_fDMA);
	INTC_Enable(ch->m_ucIntNum);

	// Interrupt Clear
	DMACH_ClearIntPending(&ch->m_sDMA);
	DMACH_ClearErrIntPending(&ch->m_sDMA);
	
	if ( ch->m_eBusSize == SPI_WORD ) {
		uDataCnt=(uDataCnt+3)>>2;
		dataSize = WORD;
	}
	else if ( ch->m_eBusSize == SPI_HWORD ) {
		uDataCnt=(uDataCnt+1)>>1;
		dataSize = HWORD;
	}
	else {
		dataSize = BYTE;
	}
	
	DMACH_Setup(DMA_B, 0x0,
		(ch->m_ucChannelNum==0)?(0x7F00B01C):(0x7F00C01C), TRUE,
		uDstAddr, FALSE,
		dataSize, uDataCnt, HANDSHAKE,
		(ch->m_ucChannelNum==0)?(DMA0_SPI0_RX):(DMA1_SPI1_RX), MEM,
		(BURST_MODE)ch->m_eDMAType, &ch->m_sDMA);
}

//////////
// File Name : SPI_interruptTransaction
// File Description : Execute Interrupt Mode .
// Input : SPI_channel 
// Output : success or fail.
// Version : 
u8 SPI_interruptTransaction( SPI_channel * ch) {
	if ( ch->m_eClockMode == SPI_MASTER ) {
		if ( ch->m_uTxBuf == NULL ) {		// master rx only.
			Outp32( &ch->m_cBase->packet_count, (1<<16)|(ch->m_uRxRemainDataSize) );
		}
	}

	Outp32( &ch->m_cBase->int_enable,
		( (ch->m_uTxRemainDataSize!=0) ? (SPI_INT_TX_FIFORDY) : (0) ) |// Tx Buffer Ready
		( (ch->m_uRxRemainDataSize!=0) ? (SPI_INT_TRAILING|SPI_INT_RX_FIFORDY|SPI_INT_RX_OVERRUN|SPI_INT_RX_UNDERRUN) : (0) ) );// Rx buffer Ready.
	Outp32( &ch->m_cBase->pending_clr, 0xffffffff );	// interrupt pending clear.
	ch->m_ucIntNum = (ch->m_ucChannelNum==0)?(NUM_SPI0):(NUM_SPI1);

	if ( ch->m_uRxBuf != 0 ) {
		Outp32( &ch->m_cBase->ch_cfg, Inp32(&ch->m_cBase->ch_cfg)|(1<<1) );	// Rx channel on
	}
	if ( ch->m_uTxBuf != 0 ) {
		Outp32( &ch->m_cBase->ch_cfg, Inp32(&ch->m_cBase->ch_cfg)|(1<<0) );	// Tx channel on
	}

	if ( ch->m_eClockMode == SPI_MASTER ) {
		Outp32( &ch->m_cBase->clk_cfg, Inp32(&ch->m_cBase->clk_cfg) | (1<<8) );		// clock On.
		if ( ch->m_bIsAutoChipSelection == FALSE ) {
			Outp32( &ch->m_cBase->slave_sel, Inp32(&ch->m_cBase->slave_sel) & ~(0x3<<0) );	// Chip selection ON.- active LOW
		}
		else {
			// Auto Chip Selection.
			Outp32( &ch->m_cBase->slave_sel, Inp32(&ch->m_cBase->slave_sel) | (0x1<<1) );
		}
	}
	Outp32( &ch->m_cBase->slave_sel, (Inp32(&ch->m_cBase->slave_sel) & ~(0x3F<<4))|(0<<4) );
	INTC_SetVectAddr( ch->m_ucIntNum, ch->m_fISR);
	INTC_Enable( ch->m_ucIntNum);

	if ( ch->m_pCallback == NULL ) {
		// Until Transfer Done.
		while( ch->m_uTxRemainDataSize != 0 );
		while( ch->m_uRxRemainDataSize != 0 );
		SPI_transactionDone(ch);
	}
	return TRUE;
}

//////////
// File Name : SPI_dmaTransaction
// File Description : Execute DMA Mode .
// Input : SPI_channel 
// Output : success or fail.
// Version : 
u8 SPI_dmaTransaction( SPI_channel * ch) {
	if ( ch->m_eClockMode == SPI_MASTER ) {
		if ( ch->m_uTxBuf == NULL ) {		// master rx only.
			Outp32( &ch->m_cBase->packet_count, (1<<16)|(ch->m_uRxRemainDataSize) );
		}
	}

	if ( ch->m_uRxBuf != 0 ) {
		Outp32( &ch->m_cBase->mode_cfg, Inp32(&ch->m_cBase->mode_cfg) | (1<<2) );	// DMA mode Rx On.
		Outp32( &ch->m_cBase->ch_cfg, Inp32(&ch->m_cBase->ch_cfg)|(1<<1) );	// Rx channel on
		SPI_RxDMAInit(ch, 0, (u32)ch->m_uRxBuf, ch->m_uRxRemainDataSize );	// SPI_RX_DATA
		Outp32( &ch->m_cBase->pending_clr, 0xffffffff);
		DMACH_Start(&ch->m_sDMA);
	}
	if ( ch->m_uTxBuf != 0 ) {
		Outp32( &ch->m_cBase->mode_cfg, Inp32(&ch->m_cBase->mode_cfg) | (1<<1) );	// DMA mode Tx On.
		Outp32( &ch->m_cBase->ch_cfg, Inp32(&ch->m_cBase->ch_cfg)|(1<<0) );	// Tx channel on
		SPI_TxDMAInit(ch, (u32)ch->m_uTxBuf, 0, ch->m_uTxRemainDataSize );	// SPI_TX_DATA
		Outp32( &ch->m_cBase->pending_clr, 0xffffffff);
		DMACH_Start(&ch->m_sDMA);
	}

	if ( ch->m_eClockMode == SPI_MASTER ) {
		Outp32( &ch->m_cBase->clk_cfg, Inp32(&ch->m_cBase->clk_cfg) | (1<<8) );		// clock On.
		if ( ch->m_bIsAutoChipSelection == FALSE ) {
			Outp32( &ch->m_cBase->slave_sel, Inp32(&ch->m_cBase->slave_sel) & ~(0x3<<0) );	// Chip selection ON.- active LOW
		}
		else {
			// Auto Chip Selection.
			Outp32( &ch->m_cBase->slave_sel, Inp32(&ch->m_cBase->slave_sel) | (0x1<<1) );
		}
	}
	Outp32( &ch->m_cBase->slave_sel, (Inp32(&ch->m_cBase->slave_sel) & ~(0x3F<<4))|(0<<4) );

	// Blocking Context, when callback-function pointer is NULL
	if ( ch->m_pCallback == NULL ) {
		// Until Transfer Done.
		while( ch->m_uTxRemainDataSize != 0 );
		while( ch->m_uRxRemainDataSize != 0 );
		DMACH_Stop(&ch->m_sDMA);		 // Stop DMA
		INTC_Disable(ch->m_ucIntNum);		 // Disable DMA Interrupt
		SPI_transactionDone(ch);
	}
	return TRUE;
}

//////////
// File Name : SPI_dmaTransaction
// File Description : Execute Polling Mode .
// Input : SPI_channel 
// Output : success or fail.
// Version : 
u8 SPI_pollingTransaction(SPI_channel * ch ) {
//	SPI_ErrorCheck(ch);
//	SPI_pollingProcess(ch);
	if ( ch->m_eClockMode == SPI_MASTER ) {
		if ( ch->m_uTxBuf == NULL ) {		// master rx only.
			Outp32( &ch->m_cBase->packet_count, (1<<16)|(ch->m_uRxRemainDataSize) );
		}
	}

	if ( ch->m_uRxBuf != 0 ) {
		Outp32( &ch->m_cBase->ch_cfg, Inp32(&ch->m_cBase->ch_cfg)|(1<<1) );	// Rx channel on
	}
	if ( ch->m_uTxBuf != 0 ) {
		Outp32( &ch->m_cBase->ch_cfg, Inp32(&ch->m_cBase->ch_cfg)|(1<<0) );	// Tx channel on
	}
	if ( ch->m_eClockMode == SPI_MASTER ) {
		Outp32( &ch->m_cBase->clk_cfg, Inp32(&ch->m_cBase->clk_cfg) | (1<<8) );		// clock On.
		if ( ch->m_bIsAutoChipSelection == FALSE ) {
			Outp32( &ch->m_cBase->slave_sel, Inp32(&ch->m_cBase->slave_sel) & ~(0x3<<0) );	// Chip selection ON.- active LOW
		}
		else {
			// Auto Chip Selection.
			Outp32( &ch->m_cBase->slave_sel, Inp32(&ch->m_cBase->slave_sel) | (0x1<<1) );
		}
	}
	Outp32( &ch->m_cBase->slave_sel, (Inp32(&ch->m_cBase->slave_sel) & ~(0x3F<<4))|(0<<4) );
	while(ch->m_uTxRemainDataSize != 0 || ch->m_uRxRemainDataSize != 0)
	{
		// do until SPI FIFO Ready
		//while( !(Inp32(&ch->m_cBase->spi_status) & (3<<0)));
		
		if( ch->m_uRxRemainDataSize != 0 ) {
			SPI_cpuOneRxTransfer( ch );
		}
		if( ch->m_uTxRemainDataSize != 0 ) {
			SPI_cpuOneTxTransfer( ch );
		}
	}

	SPI_transactionDone(ch);
	if ( ch->m_pCallback != NULL )
		ch->m_pCallback(ch);
	return TRUE;
}

// youngbo.song
// This function use for periperal DMA Test.
// have to remove, Test Only.
int SPI_PtoPTransaction( SPI_channel * ch, SPI_transfer_mode mode, u32 size, void (*callbackFn)(SPI_channel *ch) ) {
	DATA_SIZE dataSize;
	ch->m_pCallback = callbackFn;
	ch->m_eTransferMode = mode;

	// Do data size alignment
	if ( ch->m_eBusSize == SPI_WORD ) {
		size = (size+3) & ~(0x3);		// word align
	}
	else if ( ch->m_eBusSize == SPI_HWORD ) {
		size = (size+1) & ~(0x1);		// half word align
	}
	
	// renew setting.
	SPI_setBasicRegister( ch );

//	Outp32( &ch->m_cBase->ch_cfg, Inp32(&ch->m_cBase->ch_cfg)&~(3<<0) );		// Tx/Rx channel Off
//	Outp32(&ch->m_cBase->slave_sel, Inp32(&ch->m_cBase->slave_sel) | (1<<0) );	// Chip selection OFF - active LOW.
//	Outp32(&ch->m_cBase->clk_cfg, Inp32(&ch->m_cBase->clk_cfg) & ~(1<<8) );	// clock Off.
	// Auto Chip Selection.
	if ( ch->m_bIsAutoChipSelection == TRUE ) {
		Outp32( &ch->m_cBase->slave_sel, (Inp32(&ch->m_cBase->slave_sel) & ~(0x3F<<4))|(0x0<<4) );
		Outp32( &ch->m_cBase->slave_sel, Inp32(&ch->m_cBase->slave_sel) | (0x1<<1) );
	}

	Outp32( &ch->m_cBase->mode_cfg, Inp32(&ch->m_cBase->mode_cfg) | (1<<2) );	// DMA mode Rx On.
	Outp32( &ch->m_cBase->ch_cfg, Inp32(&ch->m_cBase->ch_cfg)|(1<<1) );	// Rx channel on
	Outp32( &ch->m_cBase->mode_cfg, Inp32(&ch->m_cBase->mode_cfg) | (1<<1) );	// DMA mode Tx On.
	Outp32( &ch->m_cBase->ch_cfg, Inp32(&ch->m_cBase->ch_cfg)|(1<<0) );	// Tx channel on


	DMAC_InitCh((DMA_UNIT)ch->m_ucDMACon, DMA_C, &ch->m_sDMA);			//  to do.. another..
#ifdef SPI_NORMAL_DMA
	ch->m_ucIntNum = (ch->m_cChannelNum==0)?(NUM_DMA0):(NUM_DMA1);
#else
	ch->m_ucIntNum = (ch->m_cChannelNum==0)?(NUM_SDMA0):(NUM_SDMA1);
#endif
	INTC_SetVectAddr(ch->m_ucIntNum, ch->m_fDMA);
	INTC_Enable(ch->m_ucIntNum);

	// Interrupt Clear
	DMACH_ClearIntPending(&ch->m_sDMA);
	DMACH_ClearErrIntPending(&ch->m_sDMA);
	
	if ( ch->m_eBusSize == SPI_WORD ) {
		size=(size+3)>>2;
		dataSize = WORD;
	}
	else if ( ch->m_eBusSize == SPI_HWORD ) {
		size=(size+1)>>1;
		dataSize = HWORD;
	}
	else {
		dataSize = BYTE;
	}
	DMACH_Setup(DMA_C, 0x0,
		(ch->m_ucChannelNum==0)?(0x7F00B01C):(0x7F00C01C), TRUE,	// RX Fifo.
		(ch->m_ucChannelNum==0)?(0x7F00B018):(0x7F00C018), TRUE,	// Tx Fifo
		dataSize, size, HANDSHAKE,
		(ch->m_ucChannelNum==0)?(DMA0_SPI0_RX):(DMA1_SPI1_RX),
		(ch->m_ucChannelNum==0)?(DMA0_SPI0_TX):(DMA1_SPI1_TX),
		(BURST_MODE)ch->m_eDMAType, &ch->m_sDMA);

	Outp32( &ch->m_cBase->pending_clr, 0xffffffff);
	DMACH_Start(&ch->m_sDMA);

	return TRUE;
}

//////////
// File Name : SPI_transaction
// File Description : Transmit Rx/Tx Data
// Input : SPI_channel, selection DMA/Interrupt/polling mode, Tx buffer, Rx buffer, Transfer Size
//           If user want to execte non blocking mode, than set this callback function.
//           when transmission complete, call callback function by interrupt handler.
// Output : transmission size.
// Version : 
int SPI_transaction( SPI_channel * ch, SPI_transfer_mode mode, u8* tx_data, u8* rx_data, u32 size, void (*callbackFn)(SPI_channel *ch) ) {
	ch->m_pCallback = callbackFn;
	ch->m_eTransferMode = mode;

	// Do data size alignment
	if ( ch->m_eBusSize == SPI_WORD ) {
		size = (size+3) & ~(0x3);		// word align
	}
	else if ( ch->m_eBusSize == SPI_HWORD ) {
		size = (size+1) & ~(0x1);		// half word align
	}
	
	// Channel Initialization
	if( tx_data != 0 ) {
		ch->m_uTxRemainDataSize = size;	// Tx Transfer Size.
		ch->m_uTxBuf = tx_data;			// Tx buffer pointer
	}
	else {
		ch->m_uTxRemainDataSize = 0;
		ch->m_uTxBuf = NULL;
	}
	if( rx_data != 0 ) {
		ch->m_uRxRemainDataSize = size;	// Rx Transfer Size.
		ch->m_uRxBuf = rx_data;			// Rx buffer pointer
	}
	else {
		ch->m_uRxRemainDataSize = 0;
		ch->m_uRxBuf = NULL;
	}
	
	// renew setting.
	SPI_setBasicRegister( ch );

//	Outp32( &ch->m_cBase->ch_cfg, Inp32(&ch->m_cBase->ch_cfg)&~(3<<0) );		// Tx/Rx channel Off
//	Outp32(&ch->m_cBase->slave_sel, Inp32(&ch->m_cBase->slave_sel) | (1<<0) );	// Chip selection OFF - active LOW.
//	Outp32(&ch->m_cBase->clk_cfg, Inp32(&ch->m_cBase->clk_cfg) & ~(1<<8) );	// clock Off.

	
	if (mode == SPI_POLLING_MODE ) {
		SPI_pollingTransaction( ch );
	}
	else if ( mode == SPI_INTERRUPT_MODE ) {
		SPI_interruptTransaction( ch );
	}
	else if (mode == SPI_DMA_MODE ) {
		SPI_dmaTransaction( ch );
	}

	return size;
}

//////////
// File Name : SPI_transactionDone
// File Description : This funtion invoked by transmission routine automatically.
// Input : SPI_channel
// Output : NONE.
// Version : 
void SPI_transactionDone( SPI_channel * ch ) {
	if ( ch->m_eClockMode == SPI_MASTER && ch->m_uTxBuf != NULL ) {
//		while ( Inp32(&ch->m_cBase->spi_status) & ( 0x7F << 6) );		// wait Tx FIFO empty.
		while ( !(Inp32(&ch->m_cBase->spi_status) & ( 1<<21 ) ) );		// wait Tx done signal.
	}
//	// Tx condition only.
	if ( ch->m_eClockMode == SPI_MASTER ) {
		if ( ch->m_bIsAutoChipSelection == FALSE ) {
			Outp32(&ch->m_cBase->slave_sel, Inp32(&ch->m_cBase->slave_sel) | (1<<0) );		// Chip selection OFF - active LOW.
			Outp32(&ch->m_cBase->clk_cfg, Inp32(&ch->m_cBase->clk_cfg) & ~(1<<8) );		// clock Off.
		}
	}
//	Outp32( &ch->m_cBase->ch_cfg, Inp32(&ch->m_cBase->ch_cfg)&~(3<<0) );		// Tx/Rx channel Off
}

//////////
// File Name : SPI_close
// File Description : spi_channel close.
// Input : SPI_channel
// Output : NONE.
// Version : 
void SPI_close( SPI_channel * ch ) {
	memset( ch, 0, sizeof (SPI_channel) );
}

//////////
// File Name : SPI_printStatus
// File Description : Print status register for debugging.
// Input : SPI_channel
// Output : NONE.
// Version : 
void SPI_printStatus( SPI_channel * ch ) {
	UART_Printf ( "Tx_done : %d\n", ( Inp32(&ch->m_cBase->spi_status) & (1<<21) )>>21 );
	UART_Printf ( "Trailing byte : %d\n" , ( Inp32(&ch->m_cBase->spi_status) & (1<<20) )>>20 );
	UART_Printf ( "RxFIFOlevel : %d\n", ( Inp32(&ch->m_cBase->spi_status) & (0x7F<<13) )>>13 );
	UART_Printf ( "TxFIFOlevel : %d\n", ( Inp32(&ch->m_cBase->spi_status) & (0x7F<<6) )>>6 );
	UART_Printf ( "RxOver-run err: %d\n" , ( Inp32(&ch->m_cBase->spi_status) & (1<<5) )>>5 );
	UART_Printf ( "RxUnder-run err: %d\n" , ( Inp32(&ch->m_cBase->spi_status) & (1<<4) )>>4 );
	UART_Printf ( "TxOver-run err : %d\n" , ( Inp32(&ch->m_cBase->spi_status) & (1<<3) )>>3 );
	UART_Printf ( "TxUnder-run err: %d\n" , ( Inp32(&ch->m_cBase->spi_status) & (1<<2) )>>2 );
	UART_Printf ( "Rx FIFO Rdy : %d\n" , ( Inp32(&ch->m_cBase->spi_status) & (1<<1) )>>1 );
	UART_Printf ( "Tx FIFO Rdy : %d\n" , ( Inp32(&ch->m_cBase->spi_status) & (1<<0) ) );
}

//////////
// File Name : SPI_cpuOneTxTransfer
// File Description : Transmit tx channel from power by CPU.
// Input : SPI_channel
// Output : NONE.
// Version : 
void SPI_cpuOneTxTransfer( SPI_channel *ch ) {
	int	TxWriteCnt;
	int	i;

	//  Tx buffer ready and Non-zero Tx Data size and Tx-FIFO Ready.
	if ( (ch->m_uTxBuf != NULL) && (ch->m_uTxRemainDataSize > 0) && (Inp32(&ch->m_cBase->spi_status) & (1<<0)) ) {
		TxWriteCnt = SPI_TX_FIFO_SIZE - ((Inp32(&ch->m_cBase->spi_status)>>6) & (0x7F));
		TxWriteCnt = ( ch->m_uTxRemainDataSize < TxWriteCnt ) ? ( ch->m_uTxRemainDataSize ) : (TxWriteCnt);
	}
	else {
		TxWriteCnt = 0;
	}
	
	// TX.
	if ( ch->m_eBusSize == SPI_BYTE ) {
		u8 * ptr = ch->m_uTxBuf;
		for(i=TxWriteCnt;i>0;i--) {
			Outp8(&ch->m_cBase->spi_tx_data, *ptr++);
		}
		ch->m_uTxBuf=ptr;
	}
	else if ( ch->m_eBusSize == SPI_HWORD ) {
		u16 * ptr = (u16*)ch->m_uTxBuf;
		TxWriteCnt = (TxWriteCnt & ~(0x1));	// 2 byte align.
		for(i=TxWriteCnt>>1;i>0;i--) {
			Outp16(&ch->m_cBase->spi_tx_data, *ptr++);
		}
		ch->m_uTxBuf=(u8*)ptr;
	}
	else if ( ch->m_eBusSize == SPI_WORD ) {
		u32 * ptr = (u32*)ch->m_uTxBuf;
		TxWriteCnt = (TxWriteCnt & ~(0x3));	// 4 byte align.
		for(i=TxWriteCnt>>2;i>0;i--) {
			Outp32(&ch->m_cBase->spi_tx_data, *ptr++);
		}
		ch->m_uTxBuf=(u8*)ptr;
	}
	ch->m_uTxRemainDataSize = (ch->m_uTxRemainDataSize - TxWriteCnt < 1 ) ? ( 0 ) : ( ch->m_uTxRemainDataSize - TxWriteCnt );
}

//////////
// File Name : SPI_cpuOneRxTransfer
// File Description : Transmit rx channel from power by CPU.
// Input : SPI_channel
// Output : NONE.
// Version : 
void SPI_cpuOneRxTransfer( SPI_channel *ch ) {
	int	RxReadCnt;
	int	i;

	// Rx Buffer ready and Non-zero Rx data size and Rx-FIFO Read.
//	if ( (ch->m_uRxBuf != NULL) && (ch->m_uRxRemainDataSize > 0 ) && (Inp32(&ch->m_cBase->spi_status) & (1<<1)) ) {
	if ( (ch->m_uRxBuf != NULL) && (ch->m_uRxRemainDataSize > 0 ) && ((Inp32(&ch->m_cBase->spi_status)>>13) & (0x7f)) ) {
		RxReadCnt = (Inp32(&ch->m_cBase->spi_status)>>13) & (0x7f);
		RxReadCnt = ( ch->m_uRxRemainDataSize < RxReadCnt ) ? ( ch->m_uRxRemainDataSize ) : (RxReadCnt);
	}
	else {
		RxReadCnt = 0;
	}

	// Rx.
	if ( ch->m_eBusSize == SPI_BYTE ) {
		u8 * ptr = ch->m_uRxBuf;
		for(i=RxReadCnt;i>0;i--) {
			*ptr++ = Inp8(&ch->m_cBase->spi_rx_data);
		}
		ch->m_uRxBuf=ptr;
	}
	else if ( ch->m_eBusSize == SPI_HWORD ) {
		u16 * ptr = (u16*)ch->m_uRxBuf;
		RxReadCnt = (RxReadCnt & ~(0x1));	// 1 byte align.
		for(i=RxReadCnt>>1;i>0;i--) {
			*ptr++ = Inp16(&ch->m_cBase->spi_rx_data);
		}
		ch->m_uRxBuf=(u8*)ptr;
	}
	else if ( ch->m_eBusSize == SPI_WORD ) {
		u32 * ptr = (u32*)ch->m_uRxBuf;
		RxReadCnt = (RxReadCnt & ~(0x3));	// 4 byte align.
		for(i=RxReadCnt>>2;i>0;i--) {
			*ptr++ = Inp32(&ch->m_cBase->spi_rx_data);
		}
		ch->m_uRxBuf=(u8*)ptr;
	}
	ch->m_uRxRemainDataSize = ( ch->m_uRxRemainDataSize - RxReadCnt < 1 ) ? ( 0 ) : ( ch->m_uRxRemainDataSize - RxReadCnt );
}

//////////
// File Name : SPI_pollingProcess
// File Description : Polling transmission
// Input : SPI_channel
// Output : NONE.
// Version : 
void SPI_pollingProcess( SPI_channel * ch ) {
	if ( ch->m_eClockMode == SPI_MASTER ) {
		if ( ch->m_uTxBuf == NULL ) {		// master rx only.
			Outp32( &ch->m_cBase->packet_count, (1<<16)|(ch->m_uRxRemainDataSize) );
		}
	}

	if ( ch->m_eClockMode == SPI_MASTER ) {
		Outp32( &ch->m_cBase->slave_sel, Inp32(&ch->m_cBase->slave_sel) & ~(1<<0) );	// Chip selection ON.- active LOW
		Outp32( &ch->m_cBase->clk_cfg, Inp32(&ch->m_cBase->clk_cfg) | (1<<8) );		// clock On.
	}
	if ( ch->m_uRxBuf != 0 ) {
		Outp32( &ch->m_cBase->ch_cfg, Inp32(&ch->m_cBase->ch_cfg)|(1<<1) );	// Rx channel on
	}
	if ( ch->m_uTxBuf != 0 ) {
		Outp32( &ch->m_cBase->ch_cfg, Inp32(&ch->m_cBase->ch_cfg)|(1<<0) );	// Tx channel on
	}
	while(ch->m_uTxRemainDataSize != 0 || ch->m_uRxRemainDataSize != 0)
	{
		// do until SPI FIFO Ready
		while( !(Inp32(&ch->m_cBase->spi_status) & (3<<0)));
		
		if( ch->m_uRxRemainDataSize != 0 ) {
			SPI_cpuOneRxTransfer( ch );
		}
		if( ch->m_uTxRemainDataSize != 0 ) {
			SPI_cpuOneTxTransfer( ch );
		}
	}
}

//////////
// File Name : SPI_interruptHandler
// File Description : This function is interrupt handler for transmission and error handling.
// Input : SPI_channel, SPI VIC number.
// Output : NONE.
// Version : 
void SPI_interruptHandler( SPI_channel* ch, int interruptNumber ) {
	u8 spi_status;

	if ( NUM_SPI0 == interruptNumber ) {
//		UART_Printf( "NUM_SPI0\n" );
	}
	else {
//		UART_Printf( "NUM_SPI1\n" );
	}
	// Reset Pending clear.
	Outp8(&ch->m_cBase->pending_clr, 0 );

	// Error inspection
	spi_status = Inp8(&ch->m_cBase->spi_status);
	if( spi_status & (0xf<<2) ) {	
		if(spi_status & SPI_INT_RX_OVERRUN) {
			Outp8(&ch->m_cBase->pending_clr, SPI_INT_RX_OVERRUN_CLR );
			UART_Printf("Rx Overrun Error\n");
		}
		if(spi_status & SPI_INT_RX_UNDERRUN) {
			Outp8(&ch->m_cBase->pending_clr, SPI_INT_RX_UNDERRUN_CLR );
			UART_Printf("Rx Underrun Error\n");
		}
		if(spi_status & SPI_INT_TX_OVERRUN) {
			Outp8(&ch->m_cBase->pending_clr, SPI_INT_TX_OVERRUN_CLR );
			UART_Printf("Tx Overrun Error\n");
		}
		if(spi_status & SPI_INT_TX_UNDERRUN) {
			Outp8(&ch->m_cBase->pending_clr, SPI_INT_TX_UNDERRUN_CLR );
			UART_Printf("Tx Underrun Error\n");
		}
//		ch->m_uTxRemainDataSize = 0;	// force stop.
//		ch->m_uRxRemainDataSize = 0;
	}

	// Tx under run , Over run, RX under run, Over run clear
//	Outp8(&ch->m_cBase->pending_clr, Inp8(&ch->m_cBase->pending_clr)|SPI_INT_RX_UNDERRUN|
//									SPI_INT_TX_OVERRUN|SPI_INT_TX_UNDERRUN|SPI_INT_RX_FIFORDY );

	if( ch->m_uRxRemainDataSize != 0 ) {
		SPI_cpuOneRxTransfer( ch );
	}
	if( ch->m_uTxRemainDataSize != 0 ) {
		SPI_cpuOneTxTransfer( ch );
	}

	if ( ch->m_uTxRemainDataSize == 0 && ch->m_uRxRemainDataSize == 0 ) {
		// transfer done.
		INTC_Disable( interruptNumber );
		if ( ch->m_pCallback != NULL ) {
			SPI_transactionDone(ch);
			ch->m_pCallback(ch);
		}
	}
	else {
		// transfer contine.
	//	INTC_Enable( interruptNumber );
	}
}

//////////
// File Name : SPI_interruptChannel0
// File Description : Interrupt Handler for channel 0
// Input : NONE
// Output : NONE.
// Version : 
void __irq SPI_interruptChannel0( void ) {
	SPI_channel * ch = &SPI_current_channel[0];

	SPI_interruptHandler( ch, NUM_SPI0);
 	INTC_ClearVectAddr();
}

//////////
// File Name : SPI_interruptChannel1
// File Description : Interrupt Handler for channel 1
// Input : NONE
// Output : NONE.
// Version : 
void __irq SPI_interruptChannel1( void ) {
	SPI_channel * ch = &SPI_current_channel[1];

	SPI_interruptHandler( ch, NUM_SPI1);
  	INTC_ClearVectAddr();
}

//////////
// File Name : SPI_DMADoneHandler
// File Description : DMA Done Interrupt Handler for channel 0
// Input : NONE
// Output : NONE.
// Version : 
void SPI_DMADoneHandler( SPI_channel *ch, DMA_CH dmaCh) {
	// which DMA channel between CH_A and CH_B
	if ( dmaCh & DMA_A ) {
		ch->m_uTxRemainDataSize = 0;
//		putchar('A');		// At the CPU power mode, must remove print message for system resource.
	}
	if ( dmaCh & DMA_B ) {
		ch->m_uRxRemainDataSize = 0;
//		putchar('B');		// At the CPU power mode, must remove print message for system resource.
	}
	if ( dmaCh & DMA_C ) {
		ch->m_uTxRemainDataSize = 0;
		ch->m_uRxRemainDataSize = 0;
//		putchar('C');
	}
	
	// done
	if ( ch->m_uTxRemainDataSize == 0 && ch->m_uRxRemainDataSize == 0 ) {
		if ( ch->m_pCallback != NULL ) {
			SPI_transactionDone(ch);
			ch->m_pCallback(ch);
		}
//		INTC_Disable(ch->m_ucIntNum);	// disable interrupt
	}
}

//////////
// File Name : SPI_DMADoneChannel0
// File Description : DMA Done Interrupt Handler for channel 0
// Input : NONE
// Output : NONE.
// Version : 
void __irq SPI_DMADoneChannel0( void ) {
	SPI_channel* ch = &SPI_current_channel[0];
	DMA_CH rxtx = DMACH_GetChannelNumber(&ch->m_sDMA);
	DMACH_ClearIntPending(&ch->m_sDMA);
//	putchar( '0' );	// At the CPU power mode, must remove print message for system resource.
	SPI_DMADoneHandler( ch, rxtx);
 	INTC_ClearVectAddr();
}

//////////
// File Name : SPI_DMADoneChannel1
// File Description : DMA Done Interrupt Handler for channel 1
// Input : NONE
// Output : NONE.
// Version : 
void __irq SPI_DMADoneChannel1( void ) {
	SPI_channel* ch = &SPI_current_channel[1];
	DMA_CH rxtx = DMACH_GetChannelNumber(&ch->m_sDMA);
	DMACH_ClearIntPending(&ch->m_sDMA);
//	putchar( '1' );	// At the CPU power mode, must remove print message for system resource.
	SPI_DMADoneHandler( ch, rxtx);
 	INTC_ClearVectAddr();
}

//////////
// File Name : SPI_ErrorCheck
// File Description : Setting error interrupt handler.
// Input : NONE
// Output : NONE.
// Version : 
void SPI_ErrorCheck(SPI_channel* ch) {
	Outp8(&ch->m_cBase->int_enable, Inp8(&ch->m_cBase->int_enable)|
		(SPI_INT_TRAILING|SPI_INT_RX_OVERRUN|SPI_INT_RX_UNDERRUN|SPI_INT_TX_OVERRUN|
		SPI_INT_TX_UNDERRUN|SPI_INT_RX_FIFORDY|SPI_INT_TX_FIFORDY) );
	INTC_SetVectAddr( ch->m_ucIntNum, ch->m_fISR);
	INTC_Enable( ch->m_ucIntNum);
}

//////////
// File Name : SPI_GPIOPortSet
// File Description : This function set GPIO fit on certain channel.
// Input : NONE
// Output : NONE.
// Version : 
void SPI_GPIOPortSet(u8 channel)
{
	if ( channel == 1 ) {
		GPIO_SetFunctionEach( eGPIO_C, eGPIO_4, 0x2 );	// SPI MISO[1] - EINT2[4], GPC4
		GPIO_SetFunctionEach( eGPIO_C, eGPIO_5, 0x2 );	// SPI CLK[1] - EINT2[5], GPC5
		GPIO_SetFunctionEach( eGPIO_C, eGPIO_6, 0x2 );	// SPI MOSI[1] - EINT2[6], GPC6
		GPIO_SetFunctionEach( eGPIO_C, eGPIO_7, 0x2 );	// SPI CSn[1] - EINT2[7], GPC7
		GPIO_SetPullUpDownEach( eGPIO_C, eGPIO_4, 0 );	// SPI MISO[1] - EINT2[4], GPC4
		GPIO_SetPullUpDownEach( eGPIO_C, eGPIO_5, 0 );	// SPI CLK[1] - EINT2[5], GPC5
		GPIO_SetPullUpDownEach( eGPIO_C, eGPIO_6, 0 );	// SPI MOSI[1] - EINT2[6], GPC6
		GPIO_SetPullUpDownEach( eGPIO_C, eGPIO_7, 0 );	// SPI CSn[1] - EINT2[7], GPC7

	}
	else {	// channel 0
		GPIO_SetFunctionEach( eGPIO_C, eGPIO_0, 0x2 );	// SPI MISO[0] - ADDR_CF[0], Eint2[0], GPC0
		GPIO_SetFunctionEach( eGPIO_C, eGPIO_1, 0x2 );	// SPI CLK[0] - ADDR_CF[1], EINT2[1], GPC1
		GPIO_SetFunctionEach( eGPIO_C, eGPIO_2, 0x2 );	// SPI MOSI[0] - ADDR_CF[2], EINT2[2], GPC2
		GPIO_SetFunctionEach( eGPIO_C, eGPIO_3, 0x2 );	// SPI CSn[0] - EINT2[3], GPC3
		GPIO_SetPullUpDownEach( eGPIO_C, eGPIO_0, 0 );	// SPI MISO[0] - ADDR_CF[0], Eint2[0], GPC0
		GPIO_SetPullUpDownEach( eGPIO_C, eGPIO_1, 0 );	// SPI CLK[0] - ADDR_CF[1], EINT2[1], GPC1
		GPIO_SetPullUpDownEach( eGPIO_C, eGPIO_2, 0 );	// SPI MOSI[0] - ADDR_CF[2], EINT2[2], GPC2
		GPIO_SetPullUpDownEach( eGPIO_C, eGPIO_3, 0 );	// SPI CSn[0] - EINT2[3], GPC3
	}
}

//////LCD_Test///////////////////////////////////////////////////////////////////////////////
static void delayLoop(u32 count) 
{ 
    u32 j; 
    for(j = 0; j < count; j++); 
}


void SPIInit(void)
{
	// Channel 1 Setting
	GPIO_SetFunctionEach( eGPIO_C, eGPIO_4, 0x2 );	// SPI MISO[1] - EINT2[4], GPC4
	GPIO_SetFunctionEach( eGPIO_C, eGPIO_5, 0x2 );	// SPI CLK[1] - EINT2[5], GPC5
	GPIO_SetFunctionEach( eGPIO_C, eGPIO_6, 0x2 );	// SPI MOSI[1] - EINT2[6], GPC6
	GPIO_SetFunctionEach( eGPIO_C, eGPIO_7, 0x2 );	// SPI CSn[1] - EINT2[7], GPC7
	GPIO_SetPullUpDownEach( eGPIO_C, eGPIO_4, 0 );	// SPI MISO[1] - EINT2[4], GPC4
	GPIO_SetPullUpDownEach( eGPIO_C, eGPIO_5, 0 );	// SPI CLK[1] - EINT2[5], GPC5
	GPIO_SetPullUpDownEach( eGPIO_C, eGPIO_6, 0 );	// SPI MOSI[1] - EINT2[6], GPC6
	GPIO_SetPullUpDownEach( eGPIO_C, eGPIO_7, 0 );	// SPI CSn[1] - EINT2[7], GPC7

	SPI1Out32(CH_CFG,(1<<CPOL)|(1<<CPHA));

	SPI1Out32(CLK_CFG,(1<<ENCLK)|PRESCALER_V);
	
	SPI1Out32(MODE_CFG,0);

	SPI1Out32(SLAVE_SLECTION_REG,1);

	SPI1Out32(SPI_INT_EN,0);
}

void SPIStart(void)
{
	volatile u32 tmp;
	SPI1In32(CH_CFG,tmp);
	tmp = tmp | 0x1;
	SPI1Out32(CH_CFG,tmp);
}


static void SPI_CSControl(int cs)
{
	volatile u32 tmp;
	if(cs)
	{
		SPI1In32(SLAVE_SLECTION_REG,tmp);
		tmp |= 1;
		SPI1Out32(SLAVE_SLECTION_REG,tmp);
	}
	else
	{
		SPI1In32(SLAVE_SLECTION_REG,tmp);
		tmp &= ~1;
		SPI1Out32(SLAVE_SLECTION_REG,tmp);
	}
}

void SPIWrite(u8 data1,u8 data2,u8 data3)
{
	SPI_CSControl(0);
	delayLoop(0xffff);
	SPI1Out8(SPI_TX_DATA,data1);
	SPI1Out8(SPI_TX_DATA,data2);
	SPI1Out8(SPI_TX_DATA,data3);
	delayLoop(0x1ffff);
	SPI_CSControl(1);
	delayLoop(0xffff);
}

void SPIWrite_2(u8 data1,u8 data2)
{
	SPI_CSControl(0);
	delayLoop(0xffff);
	SPI1Out8(SPI_TX_DATA,data1);
	SPI1Out8(SPI_TX_DATA,data2);
	delayLoop(0xffff);
	SPI_CSControl(1);
	delayLoop(0xffff);

	
}

static void SPI_CSLOW()
{
	SPI_CSControl(0);
}
static void SPI_CSHIGH()
{
	SPI_CSControl(1);
}

static void SPI_WriteByte()
{

	SPIWrite(0xff, 0, 0);
}
