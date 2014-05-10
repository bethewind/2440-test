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
*	File Name : sdhc.c
*  
*	File Description : This file implements the API functon for High Speed MMC.
*
*	Author : Youngmin.Kim
*	Dept. : AP Development Team
*	Created Date : 08.OCT.2005
*	Version : 0.2 
* 
*	History
*	 1) 1st Made
*      2) add SDIO, SDHC, CE-ATA interface and code compaction by youngbo.song
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
#include "sdhc.h"
#include "sysc.h"

#undef FEATURE_ASYNC_CLOCK_DOMAIN_SUPPORT
#undef FEATURE_SDHC_PREDEFINED
#undef FEATURE_SDHC_INTERRUPT_CONTROL_MODE
#define FEATURE_SDHC_HIGH_CAPACITY_CARD_SUPPORT
#undef FEATURE_SDHC_HIGH_SPEED_SUPPORT

// HighSpeed mode separator
#define SDHC_MMC_HIGH_SPEED_CLOCK 20000000
#define SDHC_SD_HIGH_SPEED_CLOCK 25000000

//////////
// File Name : SDHC_SetBlockCountReg (Inline Macro)
// File Description : This function set block count register.
// Input : SDHC, block count 
// Output : NONE.
#define SDHC_SetBlockCountReg( sCh, uBlkCnt) \
	Outp16( (sCh)->m_uBaseAddr + SDHC_BLK_COUNT, (uBlkCnt) );

//////////
// File Name : SDHC_SetSystemAddressReg (Inline Macro)
// File Description : This function set DMA start address.
// Input : SDHC, start address.
// Output : NONE.
#define SDHC_SetSystemAddressReg( sCh, SysAddr) \
	Outp32( (sCh)->m_uBaseAddr + SDHC_SYS_ADDR, (SysAddr) );

//////////
// File Name : SDHC_SetBlockSizeReg (Inline Macro)
// File Description : This function set block size and buffer size.
// Input : SDHC, DMA buffer boundary, One block size.
// Output : NONE.
#define SDHC_SetBlockSizeReg( sCh, uDmaBufBoundary, uBlkSize ) \
	Outp16( (sCh)->m_uBaseAddr + SDHC_BLK_SIZE, (((uDmaBufBoundary)<<12)|(uBlkSize)) );

#ifndef FEATURE_ASYNC_CLOCK_DOMAIN_SUPPORT

#ifdef FEATURE_SDHC_INTERRUPT_CONTROL_MODE
//////////
// File Name : SDHC_INT_WAIT_CLEAR (Inline Macro)
// File Description : Interrupt wait and clear.
// Input : SDHC, interrupt bit, timeout loop count 
// Output : NONE.	// 0x7F000000 youngbo.song
#define SDHC_INT_WAIT_CLEAR(sCh,bit,loop) \
	loop=0x7F000000; \
	while ( !((sCh)->m_sNormalStatus & (1<<bit) ) ) { \
		if ( --loop == 0 ) { \
			UART_Printf( "***********Time out Error : bit : %d, Line:%d \n", bit, __LINE__ ); \
			break;	} } \
	(sCh)->m_sNormalStatus &= ~(1<<bit); \
	Outp16( (sCh)->m_uBaseAddr + SDHC_NORMAL_INT_STAT, (1<<bit) ); \
	while( Inp16( (sCh)->m_uBaseAddr + SDHC_NORMAL_INT_STAT ) & (1<<bit) );
#else
//////////
// File Name : SDHC_INT_WAIT_CLEAR (Inline Macro)
// File Description : Interrupt wait and clear.
// Input : SDHC, interrupt bit, timeout loop count 
// Output : NONE.	// 0x7F000000 youngbo.song
#define SDHC_INT_WAIT_CLEAR(sCh,bit,loop) \
	loop=0x7F000000; \
	while ( !(Inp16( (sCh)->m_uBaseAddr + SDHC_NORMAL_INT_STAT ) & (1<<bit) ) ) { \
		if ( --loop == 0 ) { \
			UART_Printf( "***********Time out Error : bit : %d, Line:%d \n", bit, __LINE__ ); \
			break;	} } \
	do { Outp16( (sCh)->m_uBaseAddr + SDHC_NORMAL_INT_STAT, (1<<bit) ); \
	} while( Inp16( (sCh)->m_uBaseAddr + SDHC_NORMAL_INT_STAT ) & (1<<bit) );
#endif

#else

#define SDHC_INT_WAIT_CLEAR(sCh,bit,loop) \
	loop=0x7F000000; \
	while ( !(Inp16( (sCh)->m_uBaseAddr + SDHC_NORMAL_INT_STAT ) & (1<<bit) ) ) { \
		if ( --loop == 0 ) { \
			UART_Printf( "***********Time out Error : bit : %d, Line:%d \n", bit, __LINE__ ); \
			break;	} } \
	Outp16( (sCh)->m_uBaseAddr + SDHC_NORMAL_INT_STAT, (1<<bit) ); \
	if( Inp16( (sCh)->m_uBaseAddr + SDHC_NORMAL_INT_STAT ) & (1<<bit) ) { \
		UART_Printf("Status do not cleared********************\n"); \
	}

#endif


//////////
// File Name : SDHC_INT_CLEAR (Inline Macro)
// File Description : Interrupt clear.
// Input : SDHC, interrupt bit, timeout loop count 
// Output : NONE.	// 0x7F000000 youngbo.song
#define SDHC_ERROR_INT_CLEAR(sCh,bit) \
	Outp16( (sCh)->m_uBaseAddr + SDHC_ERROR_INT_STAT, (1<<bit) ); \
	while( Inp16( (sCh)->m_uBaseAddr + SDHC_ERROR_INT_STAT ) & (1<<bit) );


//////////
// File Name : SDHC_INT_CLEAR (Inline Macro)
// File Description : Interrupt clear.
// Input : SDHC, interrupt bit, timeout loop count 
// Output : NONE.	// 0x7F000000 youngbo.song
#define SDHC_NORMAL_INT_CLEAR(sCh,bit) \
	Outp16( (sCh)->m_uBaseAddr + SDHC_NORMAL_INT_STAT, (1<<bit) ); \
	while( Inp16( (sCh)->m_uBaseAddr + SDHC_NORMAL_INT_STAT ) & (1<<bit) );

//
// [7:6] Command Type
// [5]  Data Present Select
// [4] Command Index Check Enable
// [3] CRC Check Enable
// [1:0] Response Type Select
const unsigned char SDHC_cmd_sfr_data[] = {
	(unsigned char)((0<<4)|(0<<3)|(0<<0)),	// RES_NO_TYPE
	(unsigned char)((1<<4)|(1<<3)|(2<<0)),	// RES_R1_TYPE,
	(unsigned char)((1<<4)|(1<<3)|(3<<0)),	// RES_R1B_TYPE,
	(unsigned char)((0<<4)|(1<<3)|(1<<0)),	// RES_R2_TYPE,
	(unsigned char)((0<<4)|(0<<3)|(2<<0)),	// RES_R3_TYPE,
	(unsigned char)((0<<4)|(0<<3)|(2<<0)),	// RES_R4_TYPE,
	(unsigned char)((1<<4)|(1<<3)|(2<<0)),	// RES_R5_TYPE,
	(unsigned char)((1<<4)|(1<<3)|(2<<0)),	// RES_R6_TYPE,	// check need.
	(unsigned char)((1<<4)|(1<<3)|(2<<0)),	// RES_R7_TYPE,	// check need.
};

SDHC* SDHC_curr_card[SDHC_CHANNEL_CNT];
unsigned int SDHC_global_card_size;
static u8 SDHC_globl_testBuffer[512];


void SDHC_ADMA2ErrorInterruptHandler(SDHC*sCh) {
/*	u32 status;

	status = Inp32(sCh->m_uBaseAddr+SDHC_ADMA_ERROR);

	if( status & SDHC_ADMA_ERROR_STATUS
*/
}


void SDHC_ErrorInterruptHandler(SDHC* sCh) {
	unsigned short status;

	status = sCh->m_sErrorStatus = Inp16(sCh->m_uBaseAddr+SDHC_ERROR_INT_STAT);

	if( status & SDHC_ADMA2_ERROR ) {
		UART_Printf("SDHC_ADMA2_ERROR[%x]\n", Inp32(sCh->m_uBaseAddr+SDHC_ADMA_ERROR));
		SDHC_ERROR_INT_CLEAR(sCh, 9);
	}
	if( status & SDHC_AUTO_CMD12_ERROR ) {
		UART_Printf("SDHC_AUTO_CMD12_ERROR\n");
		SDHC_ERROR_INT_CLEAR(sCh, 8);
	}
	if( status & SDHC_CURRENT_LIMIT_ERROR ) {
		UART_Printf("SDHC_CURRENT_LIMIT_ERROR\n");
		SDHC_ERROR_INT_CLEAR(sCh, 7);
	}
	if( status & SDHC_DATA_END_BIT_ERROR ) {
		UART_Printf("SDHC_DATA_END_BIT_ERROR\n");
		SDHC_ERROR_INT_CLEAR(sCh, 6);
	}
	if( status & SDHC_DATA_CRC_ERROR ) {
		UART_Printf("SDHC_DATA_CRC_ERROR\n");
		SDHC_ERROR_INT_CLEAR(sCh, 5);
	}
	if( status & SDHC_DATA_TIME_OUT_ERROR ) {
		UART_Printf("SDHC_DATA_TIME_OUT_ERROR\n");
		SDHC_ERROR_INT_CLEAR(sCh, 4);
	}
	if( status & SDHC_CMD_INDEX_ERROR ) {
		UART_Printf("SDHC_CMD_INDEX_ERROR\n");
		SDHC_ERROR_INT_CLEAR(sCh, 3);
	}
	if( status & SDHC_CMD_END_BIT_ERROR ) {
		UART_Printf("SDHC_CMD_END_BIT_ERROR\n");
		SDHC_ERROR_INT_CLEAR(sCh, 2);
	}
	if( status & SDHC_CMD_CRC_ERROR ) {
		UART_Printf("SDHC_CMD_CRC_ERROR\n");
		SDHC_ERROR_INT_CLEAR(sCh, 1);
	}
	if( status & SDHC_CMD_TIMEOUT_ERROR ) {
//		UART_Printf("SDHC_CMD_TIMEOUT_ERROR\n");
		SDHC_ERROR_INT_CLEAR(sCh, 0);
	}
}

//////////
// File Name : SDHC_DMAIntHandler
// File Description : This function is interrupt service routine for Common DMA Handling.
// Input : NONE.
// Output : NONE.
void SDHC_InterruptHandler(SDHC * sCh) {
	unsigned short status;

	sCh->m_sNormalStatus |= status  = Inp16(sCh->m_uBaseAddr+SDHC_NORMAL_INT_STAT);

	if ( 0 ) {
		UART_Printf("no status++++++++++++++++++++++++++++++++++++");
		while(1);
	}
//	UART_Printf("normal status : %x\n", status);
	if( status & SDHC_ERROR_INTERRUPT_EN ) {
		SDHC_ErrorInterruptHandler(sCh);
		SDHC_NORMAL_INT_CLEAR(sCh, 15);
	}
	if( status & SDHC_SD_ADDRESS_INT3_EN ) {
		SDHC_NORMAL_INT_CLEAR(sCh, 14 );
	}
	if( status & SDHC_SD_ADDRESS_INT2_EN ) {
		SDHC_NORMAL_INT_CLEAR(sCh, 13 );
	}
	if( status & SDHC_SD_ADDRESS_INT1_EN ) {
		SDHC_NORMAL_INT_CLEAR(sCh, 12 );
	}
	if( status & SDHC_SD_ADDRESS_INT0_EN ) {
		SDHC_NORMAL_INT_CLEAR(sCh, 11 );
	}
	if( status & SDHC_READWAIT_SIG_INT_EN ) {
		SDHC_NORMAL_INT_CLEAR(sCh, 10 );
	}
	if( status & SDHC_CCS_INTERRUPT_STATUS_EN ) {
		SDHC_NORMAL_INT_CLEAR(sCh, 9 );
		sCh->m_uCCSResponse=0;
	}
	if( status & SDHC_CARD_SIG_INT_EN ) {	// SDIO Interrupt.
		UART_Printf("Card Interrupt\n");
		SDHC_NORMAL_INT_CLEAR(sCh, 8 );
	}
	if( status & SDHC_CARD_REMOVAL_SIG_INT_EN ) {
		UART_Printf("Card Removed\n");
		SDHC_NORMAL_INT_CLEAR(sCh, 7 );
	}
	if( status & SDHC_CARD_INSERT_SIG_INT_EN ) {
		UART_Printf("Card Insered\n");
		SDHC_NORMAL_INT_CLEAR(sCh, 6 );
	}
	if( status & SDHC_BUFFER_READREADY_SIG_INT_EN ) {
//		SDHC_NORMAL_INT_CLEAR(sCh, 5 );
		SDHC_ReadOneBlock( sCh );
	}
	if( status & SDHC_BUFFER_WRITEREADY_SIG_INT_EN ) {
//		SDHC_NORMAL_INT_CLEAR(sCh, 4 );
		SDHC_WriteOneBlock( sCh );
	}
	if( status & SDHC_DMA_SIG_INT_EN ) {
		SDHC_NORMAL_INT_CLEAR(sCh, 3 );
		SDHC_SetSystemAddressReg(sCh, Inp32(sCh->m_uBaseAddr+SDHC_SYS_ADDR) );
	}
	if( status & SDHC_BLOCKGAP_EVENT_SIG_INT_EN ) {
		UART_Printf("BlockGap\n");
		SDHC_NORMAL_INT_CLEAR(sCh, 2 );
	}
	if( status & SDHC_TRANSFERCOMPLETE_SIG_INT_EN ) {
		sCh->m_uRemainBlock=0;
		SDHC_NORMAL_INT_CLEAR(sCh, 1 );
	}
	if( status & SDHC_COMMANDCOMPLETE_SIG_INT_EN ) {
		// Command Complete.
		SDHC_NORMAL_INT_CLEAR(sCh, 0 );
	}
}

//////////
// File Name : SDHC_DMADone
// File Description : Interrupt handler for channel 0
// Input : NONE.
// Output : NONE.
void __irq SDHC_ISR0(void) {
	SDHC_InterruptHandler( SDHC_curr_card[SDHC_CHANNEL_0]);
	INTC_ClearVectAddr();
}

//////////
// File Name : SDHC_ISR1
// File Description : Interrupt handler for channel 1
// Input : NONE.
// Output : NONE.
void __irq SDHC_ISR1(void) {
	SDHC_InterruptHandler( SDHC_curr_card[SDHC_CHANNEL_1]);
	INTC_ClearVectAddr();
}

//////////
// File Name : SDHC_ISR2
// File Description : Interrupt handler for channel 2
// Input : NONE.
// Output : NONE.
void __irq SDHC_ISR2(void) {
	SDHC_InterruptHandler( SDHC_curr_card[SDHC_CHANNEL_2]);
	INTC_ClearVectAddr();
}


//////////
// File Name : SDHC_InitCh
// File Description : Initialize channel information.
// Input : SDHC Channel Number, SDHC pointer..
// Output : NONE.
void SDHC_InitCh(SDHC_channel eCh, SDHC *sCh) {
	sCh->m_eChannel = eCh;
	SDHC_curr_card[sCh->m_eChannel]=sCh;	// Pointer...
	if ( eCh == SDHC_CHANNEL_0 ) {		// Channel 0
		sCh->m_uBaseAddr = (u8*)HSMMC0_BASE;
		sCh->m_fIntFn = SDHC_ISR0;
		sCh->m_ucIntChannelNum = NUM_HSMMC0;
	}
	else if ( eCh == SDHC_CHANNEL_1 ) {	// Channel 1
		sCh->m_uBaseAddr = (u8*)HSMMC1_BASE;
		sCh->m_fIntFn = SDHC_ISR1;
		sCh->m_ucIntChannelNum = NUM_HSMMC1;
	}
	else if ( eCh == SDHC_CHANNEL_2 ) {	// channel 2->SPI muxed.
		sCh->m_uBaseAddr = (u8*)HSMMC2_BASE;
		sCh->m_fIntFn = SDHC_ISR2;
		sCh->m_ucIntChannelNum = NUM_SPI1;
	}
	else {
		Assert(0);
	}
}


/**
 * act2  |  act1  | symbol
 *   0    |    0    | NOP
 *   0    |    1    | reserved
 *   1    |    0    | transfer data
 *   1    |    1    | Link
 * valid : valid = 1 -> Indicates this line of descriptor is effective. If valid=0 generate ADMA Error Interrupt and stop ADMA to prevent runaway.
 * end : end = 1 -> Indicates to end of descriptor. The transfer Complete Interrupt is generated when the operation of the descriptor line is completed.
 * Int : Interrupt = 1 -> generates DMA Interrupt When the Operation of the descriptor line is completed.
 */
typedef struct _SDHC_ADMA2_DescriptorEntry {
	u32 attr;
	u32 address;
} SDHC_ADMA2_DescriptorEntry;

SDHC_ADMA2_DescriptorEntry ADMA2List[4096];


void SDHC_GenerateADMA2Descriptor( SDHC* sCh, u32 sysAddr, u16 linkSize, int linkCount) {
	int i;
	
	memset( ADMA2List, 0, sizeof(ADMA2List) );
	for(i=0;i<linkCount;i++) {
		if( i<linkCount-1) {	
			ADMA2List[i].attr = (linkSize<<16) | (0x2<<4) | (0<<2) | (0<<1) | (1<<0);
		}
		else {
			ADMA2List[i].attr = (linkSize<<16) | (0x2<<4) | (1<<2) | (1<<1) | (1<<0);
		}
		ADMA2List[i].address = (u32)(sysAddr + linkSize*i);
	}
	// ADMA2 Mode Selection.
	Outp8( sCh->m_uBaseAddr + SDHC_HOST_CTRL, ( Inp8( sCh->m_uBaseAddr + SDHC_HOST_CTRL ) & ~(0x3<<3) ) | (0x2<<3) );
	// ADMA2 System Address Selection.
	Outp32( sCh->m_uBaseAddr + SDHC_ADMA_SYSTEM_ADDRESS, (u32)ADMA2List);
}


void SDHC_SetDriveStrength(SDHC*sCh, u8 cStrength ) {
	if(cStrength>3) {
		cStrength=3;
	}
	if(sCh->m_eChannel == SDHC_CHANNEL_0 || sCh->m_eChannel == SDHC_CHANNEL_1 ) {
		Outp32( 0x7F0081A0, (Inp32(0x7F0081A0 ) & ~(0x3<<26)) | (cStrength<<26) );	// Data Line
		Outp32( sCh->m_uBaseAddr+SDHC_CONTROL4, cStrength<<16);				// Clock
	}
	else if(sCh->m_eChannel == SDHC_CHANNEL_2) {
		Outp32( 0x7F0081A0, (Inp32(0x7F0081A0 ) & ~(0x3<<26)) | (cStrength<<26) );	// MMC Data Line
//		Outp32( 0x7F0081A0, (Inp32(0x7F0081A0 ) & ~(0x3<<28)) | (3<<28) );	// SPI Data Line
		Outp32( 0x7F0081A0, (Inp32(0x7F0081A0 ) & ~(0x3<<18)) | (cStrength<<18) );	// Clock
	}
}



//////////
// File Name : SDHC_OpenMedia
// File Description : Initialize channel information.
// Input : SDHC Clock Source, SDHC pointer.
// Output : Success or Failure.
u8 SDHC_OpenMedia(SDHC_clockSource eClkSrc, SDHC* sCh)
{
	SDHC_SpeedMode speed;
	u32 uSrcFreq;
	u32 uOperFreq;

	sCh->m_eClockSource = eClkSrc;
	sCh->m_ucHostCtrlReg = 0;
	sCh->m_usClkCtrlReg = 0;
	if ( sCh->m_ucBandwidth == 0 )
		sCh->m_ucBandwidth = 4;		// bit width.

	// GPIO Setting.
	SDHC_SetGPIO(sCh->m_eChannel, sCh->m_ucBandwidth);
	
	if ( eClkSrc == SDHC_HCLK || eClkSrc == 0 ) {
		uSrcFreq = g_HCLK;
	}
	else if ( eClkSrc == SDHC_EPLL ) {
		uSrcFreq = Inp32(0x7E00F014);	// temp
	//	((uSrcFreq>>16)&0xff) //M
	//	((uSrcFreq>>8)&0x3f) //P
	//	(uSrcFreq&0x07) //S
	//   ignore K value
		uSrcFreq = (u32)(((float)FIN)*((uSrcFreq>>16)&0xff)/(((uSrcFreq>>8)&0x3f)*(1<<(uSrcFreq&0x07))));
	}
	else if ( eClkSrc == SDHC_EXTCLK ) {
		// USB Phy On.
		Outp32( 0x7C100000, 0 );
		Outp32( 0X7C100008, 3 );
		Delay(10);
		Outp32( 0X7C100008, 0 );
		
		uSrcFreq = 48000000;	// 48MHz External crystal
	}
	else {
		Assert(FALSE);
	}

	if ( sCh->m_uClockDivision == 0 ) {
		uOperFreq = uSrcFreq;
	}
	else {
		uOperFreq = uSrcFreq / (sCh->m_uClockDivision*2);
	}
	UART_Printf("WorkingFreq = %dMHz\n", uOperFreq/1000000 );
	
	SDHC_ResetController(sCh);

	SDHC_SetDriveStrength(sCh, 3);

	if (!SDHC_IdentifyCard(sCh))
		return FALSE;

	// card Selection
	if ( !SDHC_IssueCommand( sCh, 7, (u32)(sCh->m_uRca<<16), SDHC_CMD_AC_TYPE, SDHC_RES_R1B_TYPE ) )
		return FALSE;

	if ( sCh->m_eCardType == SDHC_MMC_CARD ) {
		SDHC_ReadMMCExtCSD( sCh );
	}

	if (sCh->m_eCardType == SDHC_MMC_CARD || sCh->m_eCardType == SDHC_CE_ATA_CARD ) {
		if (sCh->m_ucSpecVer >= 4) {
			speed = ( uOperFreq > SDHC_MMC_HIGH_SPEED_CLOCK) ? (SDHC_HIGH_SPEED) : (SDHC_NORMAL_SPEED);
			if (!SDHC_SetMmcSpeedMode(speed, sCh)) {
				return FALSE;
			}
		}
		else // old version
			Assert(uOperFreq<=SDHC_MMC_HIGH_SPEED_CLOCK); // Error! Old version MMC card can not support working frequency higher than 20MHz");
	}
	else if ( sCh->m_eCardType == SDHC_SD_CARD ) {
		SDHC_GetSdScr(sCh);
		if (sCh->m_ucSpecVer==1||sCh->m_ucSpecVer==2) {
			speed = ( uOperFreq > SDHC_SD_HIGH_SPEED_CLOCK) ? (SDHC_HIGH_SPEED) : (SDHC_NORMAL_SPEED);
			if (!SDHC_SetSdCardSpeedMode(speed, sCh)) {
				return FALSE;
			}
		}
		else
			Assert(uOperFreq<=SDHC_SD_HIGH_SPEED_CLOCK); // Error! Old version SD card can not support working frequency higher than 25MHz");
	}
	// host controller speed setting.
	speed = ( uOperFreq > SDHC_MMC_HIGH_SPEED_CLOCK) ? (SDHC_HIGH_SPEED) : (SDHC_NORMAL_SPEED);
	SDHC_SetHostCtrlSpeedMode( speed, sCh );

	SDHC_SetSdClockOnOff(0, sCh); // If the sd clock is to be changed, you need to stop sd-clock.
	SDHC_SetSdClock(sCh->m_uClockDivision, sCh, speed, uOperFreq);

	// After a card was selected, then the bus width can be changed.
	if (!SDHC_SetDataTransferWidth( sCh))
		return FALSE;

	if (!SDHC_WaitForCard2TransferState(sCh))
		return FALSE;

	// CMD16 can not be set while card is in programming state.
	if (!SDHC_IssueCommand(sCh, 16, 512, SDHC_CMD_AC_TYPE, SDHC_RES_R1_TYPE ) ) // Set the block size
		return FALSE;

	// youngbo.song
	Outp32( sCh->m_uBaseAddr+SDHC_CONTROL2, Inp32(sCh->m_uBaseAddr+SDHC_CONTROL2)|(1<<8)|(2<<9)|(1<<28));
	return TRUE;
}

//////////
// File Name : SDHC_OpenMediaWithMode
// File Description : SD/MMC Channel initialize with parameter.
// Input : Bus width, Operation mode, Clock source, Clock divider, channel Number, Information Structuer pointer.
// Output : Success or Failure.
u8 SDHC_OpenMediaWithMode(u32 uBusWidth, SDHC_operation eOpMode, SDHC_clockSource eClkSrc,
	u32 uSdClkDivisor, SDHC_channel channel, SDHC* sCh)
{
	sCh->m_eOpMode = eOpMode;
	sCh->m_ucBandwidth = uBusWidth;
	sCh->m_uClockDivision = uSdClkDivisor;

	// channel initialize.
	SDHC_InitCh(channel, sCh);
	
	if ( !SDHC_OpenMedia(eClkSrc, sCh ) )
		return FALSE;

	return TRUE;
}

//////////
// File Name : SDHC_CloseMedia
// File Description : This function close media session.
// Input : SDHC
// Output : NONE.
u8 SDHC_IdentifyCard(SDHC* sCh)
{
	SDHC_Set_InitClock( sCh );	// initial Clock Setting.

//	Outp8( sCh->m_uBaseAddr+SDHC_TIMEOUT_CTRL, 
//		(Inp8(sCh->m_uBaseAddr+SDHC_TIMEOUT_CTRL)&(0xF<<4))|0xe);	// Timeout setting.
	Outp8( sCh->m_uBaseAddr+SDHC_TIMEOUT_CTRL, 
		(Inp8(sCh->m_uBaseAddr+SDHC_TIMEOUT_CTRL)&(0xF<<4))|0xE);	// Timeout setting.
	Outp8( sCh->m_uBaseAddr+SDHC_HOST_CTRL, 0);					// NORMAL Speed mode. and SDMA selected.

	SDHC_SetHostCtrlSpeedMode(SDHC_NORMAL_SPEED, sCh);

	INTC_SetVectAddr( sCh->m_ucIntChannelNum, sCh->m_fIntFn );
	INTC_Enable( sCh->m_ucIntChannelNum );
#ifdef FEATURE_SDHC_INTERRUPT_CONTROL_MODE
	SDHC_SetSdhcInterruptEnable(0x3FF, 0xFF, 0x07F, 0xFF, sCh);
#else
	SDHC_SetSdhcInterruptEnable(0x3F7, 0xFF, 0x0, 0x0, sCh);	// except DMA interrupt
#endif
	// Check card OCR(Operation Condition Register)
	if (SDHC_SetSDOCR(sCh))
		sCh->m_eCardType = SDHC_SD_CARD;
	else if (SDHC_SetMmcOcr(sCh))
		sCh->m_eCardType = SDHC_MMC_CARD;
	else
		return FALSE;

	// Check the attached card and place the card in the IDENT state rHM_RSPREG0
	SDHC_IssueCommand( sCh, 2, 0, SDHC_CMD_BCR_TYPE, SDHC_RES_R2_TYPE );

	UART_Printf("- Product Name : %c%c%c%c%c%c\n",((Inp32(sCh->m_uBaseAddr+SDHC_RSP2)>>24)&0xFF),
		((Inp32(sCh->m_uBaseAddr+SDHC_RSP2)>>16)&0xFF),
		((Inp32(sCh->m_uBaseAddr+SDHC_RSP2)>>8)&0xFF),
		(Inp32(sCh->m_uBaseAddr+SDHC_RSP2)&0xFF),
		((Inp32(sCh->m_uBaseAddr+SDHC_RSP1)>>24)&0xFF),
		((Inp32(sCh->m_uBaseAddr+SDHC_RSP1)>>16)&0xFF));

	// Send RCA(Relative Card Address). It places the card in the STBY state
	sCh->m_uRca= (sCh->m_eCardType==SDHC_MMC_CARD) ? (1): (0);
	SDHC_IssueCommand( sCh, 3, sCh->m_uRca<<16, SDHC_CMD_AC_TYPE, SDHC_RES_R1_TYPE );
	if( sCh->m_eCardType == SDHC_SD_CARD)
	{
		sCh->m_uRca = (Inp32(sCh->m_uBaseAddr+SDHC_RSP0)>>16)&0xFFFF;
		UART_Printf("=>  RCA=0x%x\n", sCh->m_uRca);
	}

	//Send CSD
	SDHC_IssueCommand( sCh, 9, sCh->m_uRca<<16, SDHC_CMD_AC_TYPE, SDHC_RES_R2_TYPE );
	SDHC_DisplayCardInformation(sCh);

	return true;
}

//////////
// File Name : SDHC_CloseMedia
// File Description : This function close media session.
// Input : SDHC
// Output : NONE.
void SDHC_CloseMedia(SDHC* sCh)
{
	SDHC_SetSdClockOnOff(FALSE, sCh); // SDCLK Disable
}

//////////
// File Name : SDHC_WriteOneBlock
// File Description : This function writes one block data by CPU transmission mode.
// Input : SDHC( assert buffer pointer and remain data length.)
// Output : NONE.
void SDHC_WriteOneBlock( SDHC* sCh ) {
	u32* source_Ptr = sCh->m_uBufferPtr;
	int block_size;
	int i;

	block_size = Inp16( (sCh)->m_uBaseAddr + SDHC_BLK_SIZE ) & 0xFFF;
	
	// Wait for buffer write ready... - SDHC_BUFFER_WRITEREADY_STS_INT_EN
	SDHC_INT_WAIT_CLEAR( sCh, 4, i );
	if ( 100000 - i > 1 )
		UART_Printf( "100000 to time:%d\n", 100000-i);
	
	block_size = (block_size+3) >> 2;	// block_size = (block_size+3) / 4;
	for(i=block_size; i>0; i--)	//512/4
	{
		Outp32( sCh->m_uBaseAddr+SDHC_BUF_DAT_PORT, *source_Ptr++);
	}
	sCh->m_uRemainBlock--;
	sCh->m_uBufferPtr = source_Ptr;
}

//////////
// File Name : SDHC_ReadOneBlock
// File Description : This function reads one block data by CPU transmission mode.
// Input : SDHC( assert buffer pointer and remain data length.)
// Output : NONE.
void SDHC_ReadOneBlock( SDHC* sCh ) {
	u32* target_Ptr = sCh->m_uBufferPtr;
	int block_size;
	int i;

	block_size = Inp16( (sCh)->m_uBaseAddr + SDHC_BLK_SIZE ) & 0xFFF;
	
	// Wait for buffer read ready...- SDHC_BUFFER_READREADY_STS_INT_EN
	SDHC_INT_WAIT_CLEAR( sCh, 5, i );
	if ( 100000 - i > 1 )
		UART_Printf( "100000 to time:%d\n", 100000-i);
		
	block_size = (block_size+3) >> 2;	// block_size = (block_size+3) / 4;
	for(i=block_size; i>0; i--)
	{
		*target_Ptr++ = Inp32( sCh->m_uBaseAddr+SDHC_BUF_DAT_PORT );
	}

	sCh->m_uRemainBlock--;
	sCh->m_uBufferPtr = target_Ptr;
}


//////////
// File Name : SDHC_WriteBlocks
// File Description : This function writes user-data common usage.
// Input : start block, block count, source buffer address, SDHC channel
// Output : Success or Failure
u8 SDHC_WriteBlocks(u32 uStBlock, u16 uBlocks, u32 uBufAddr, SDHC* sCh) {
	int ignore;
	if(sCh->m_eTransMode == SDHC_BYTE_MODE)
		uStBlock = uStBlock<<9;	//	 uStBlock * 512;

	sCh->m_uRemainBlock = uBlocks;	// number of blocks.
	sCh->m_uBufferPtr = (u32*)uBufAddr;

	if ( !SDHC_WaitForCard2TransferState( sCh ) )
		return 3;

	SDHC_SetBlockSizeReg(sCh, 7, 512); // Maximum DMA Buffer Size, Block Size
	SDHC_SetBlockCountReg(sCh, uBlocks); // Block Numbers to Write 
	if( sCh->m_eOpMode == SDHC_SDMA_MODE ) {
		SDHC_SetSystemAddressReg(sCh, uBufAddr);// AHB System Address For Write
		SDHC_SetTransferModeReg( (uBlocks==1)?(0):(1), 0, (uBlocks==1)?(0):(1), 1, 1, sCh ); // transfer mode 
	}
	else if( sCh->m_eOpMode == SDHC_ADMA2_MODE ) {
		SDHC_GenerateADMA2Descriptor(sCh, uBufAddr, 1024, uBlocks>>1);
		SDHC_SetTransferModeReg( (uBlocks==1)?(0):(1), 0, (uBlocks==1)?(0):(1), 1, 1, sCh ); // transfer mode 
	}
	else {
		// Interrupt or Polling mode.
		SDHC_SetTransferModeReg( (uBlocks==1)?(0):(1), 0, (uBlocks==1)?(0):(1), 1, 0, sCh ); // transfer mode 
	}

	if (uBlocks == 1) {
		if ( !SDHC_IssueCommand( sCh, 24, uStBlock, SDHC_CMD_ADTC_TYPE, SDHC_RES_R1_TYPE )) {
			return 4;
		}
	}
	else {
		if ( !SDHC_IssueCommand( sCh, 25, uStBlock, SDHC_CMD_ADTC_TYPE, SDHC_RES_R1_TYPE )) {
			return 5;
		}
	}

	if( sCh->m_eOpMode == SDHC_SDMA_MODE || sCh->m_eOpMode == SDHC_ADMA2_MODE ) {
	}
	else if( sCh->m_eOpMode == SDHC_POLLING_MODE ) {
#ifdef FEATURE_SDHC_INTERRUPT_CONTROL_MODE
		Assert("FEATURE_INTERRUPT_CONTROL not support polling mode");
#endif
		while(sCh->m_uRemainBlock != 0 ) {
			SDHC_WriteOneBlock( sCh );
		}
	}
	else if( sCh->m_eOpMode == SDHC_INTERRUPT_MODE ) {
		while(sCh->m_uRemainBlock != 0);
	}
	else {
		Assert("Not support mode");
	}
	// wait for transfer complete.
	SDHC_INT_WAIT_CLEAR( sCh, 1, ignore );
	sCh->m_uRemainBlock = 0;

	return 0;
}


//////////
// File Name : SDHC_ReadBlocks
// File Description : This function reads user-data common usage.
// Input : start block, block count, target buffer address, SDHC channel
// Output : Success or Failure
u8 SDHC_ReadBlocks(u32 uStBlock, u16 uBlocks, u32 uBufAddr, SDHC* sCh)
{
	u32 ignore;
	if(sCh->m_eTransMode == SDHC_BYTE_MODE)
		uStBlock = uStBlock<<9;//*512;
		
	sCh->m_uRemainBlock = uBlocks;
	sCh->m_uBufferPtr=(u32*)uBufAddr;

	if ( !SDHC_WaitForCard2TransferState( sCh ) )
		return 3;

	SDHC_SetBlockSizeReg(sCh, 7, 512); // Maximum DMA Buffer Size, Block Size
	SDHC_SetBlockCountReg(sCh, uBlocks); // Block Numbers to Write

	if ( sCh->m_eOpMode == SDHC_SDMA_MODE ) {
		SDHC_SetSystemAddressReg(sCh, uBufAddr);// AHB System Address For Write
		SDHC_SetTransferModeReg((uBlocks==1)?(0):(1), 1, (uBlocks==1)?(0):(1), 1, 1, sCh ); //Transfer mode setting
	}
	else if ( sCh->m_eOpMode == SDHC_ADMA2_MODE ) {
		SDHC_GenerateADMA2Descriptor(sCh, uBufAddr, 1024, uBlocks>>1);
		SDHC_SetTransferModeReg((uBlocks==1)?(0):(1), 1, (uBlocks==1)?(0):(1), 1, 1, sCh ); //Transfer mode setting
	}
	else {
		SDHC_SetTransferModeReg((uBlocks==1)?(0):(1), 1, (uBlocks==1)?(0):(1), 1, 0, sCh ); //Transfer mode setting
	}

	if (uBlocks == 1) { // CMD17: Single-Read
		if ( !SDHC_IssueCommand( sCh, 17, uStBlock, SDHC_CMD_ADTC_TYPE, SDHC_RES_R1_TYPE )) {
			return 4;
		}
	}
	else { // CMD18: Multi-Read
		if( !SDHC_IssueCommand( sCh, 18, uStBlock, SDHC_CMD_ADTC_TYPE, SDHC_RES_R1_TYPE )) {
			return 5;
		}
	}

	if( sCh->m_eOpMode == SDHC_SDMA_MODE || sCh->m_eOpMode == SDHC_ADMA2_MODE ) {
	}
	else if( sCh->m_eOpMode == SDHC_POLLING_MODE ) {
#ifdef FEATURE_SDHC_INTERRUPT_CONTROL_MODE
		Assert("FEATURE_INTERRUPT_CONTROL not support polling mode");
#endif
		while(sCh->m_uRemainBlock != 0 ) {
			SDHC_ReadOneBlock( sCh );
		}
	}
	else if( sCh->m_eOpMode == SDHC_INTERRUPT_MODE ) {
		while(sCh->m_uRemainBlock != 0);
	}
	else {
		Assert("Not support mode");
	}
	// wait for transfer complete.
	SDHC_INT_WAIT_CLEAR( sCh, 1, ignore );
	sCh->m_uRemainBlock = 0;
	return 0;	// block_cnt * 512
}



//////////
// File Name : SDHC_WaitForCard2TransferState
// File Description : Get error source data. 
// Input : SDHC channel
// Output : Success or Failure
u8 SDHC_WaitForCard2TransferState(SDHC* sCh) {
	u32 uStatus;

	// do until programming status.
	do {
		if ( !SDHC_IssueCommand( sCh, 13, sCh->m_uRca<<16, SDHC_CMD_AC_TYPE, SDHC_RES_R1B_TYPE) ) {
			return FALSE;
		}
		uStatus = (Inp32( sCh->m_uBaseAddr+SDHC_RSP0)>>9) & 0xf;
	} while(uStatus==7||uStatus==6);

	return (uStatus==4) ? TRUE : FALSE;
}

//////////
// File Name : SDHC_ResetController
// File Description : Reset SDHC Controller.
// Input : SDHC channel
// Output : NONE.
void SDHC_ResetController(SDHC* sCh) {
	Outp8( sCh->m_uBaseAddr+SDHC_SOFTWARE_RESET, 0x3);
}

//////////
// File Name : SDHC_IssueCommand
// File Description : This function issue sd/mmc command.
// Input : SDHC, command Index, Argument, Command Type, Response Type
// Output : Command Result.
u8 SDHC_IssueCommand( SDHC* sCh, u16 uCmd, u32 uArg, SDHC_CommandType cType, SDHC_ResponseType rType ) {
	u16 sfrData;
	u32 Loop;

#ifdef FEATURE_ASYNC_CLOCK_DOMAIN_SUPPORT
	while( Inp32( sCh->m_uBaseAddr+SDHC_CONTROL4 ) & 0x1 ) {
		UART_Printf("BUSY");
	}
#endif
	while( Inp32( sCh->m_uBaseAddr+SDHC_PRESENT_STAT ) & 0x1 );	// Check CommandInhibit_CMD

	sfrData = (uCmd<<8) | SDHC_cmd_sfr_data[ rType ];
	if ( cType == SDHC_CMD_ADTC_TYPE ) {
		sfrData |= (1<<5);
	}
	if ( uCmd == 12 ) { // check abort bit when stop command.
		sfrData |= (3<<6);
	}
	// argument setting.
	Outp32( sCh->m_uBaseAddr+SDHC_ARG, uArg);
	
	Outp16( sCh->m_uBaseAddr+SDHC_COMMAND, sfrData);

	// Command Complete. - SDHC_COMMANDCOMPLETE_STS_INT_EN
	SDHC_INT_WAIT_CLEAR( sCh, 0, Loop);

	// Error Status Check - reduce too much error message.
	if ( (Inp16( sCh->m_uBaseAddr+SDHC_NORMAL_INT_STAT ) & (1<<15)) && !(uCmd==1||uCmd==55||uCmd==41) ) {
		UART_Printf("Command = %d, Error Stat = %x\n", Inp16( sCh->m_uBaseAddr+SDHC_COMMAND )>>8,
			Inp16( sCh->m_uBaseAddr+SDHC_ERROR_INT_STAT ) );
		Outp16( sCh->m_uBaseAddr+SDHC_ERROR_INT_STAT, Inp16( sCh->m_uBaseAddr+SDHC_ERROR_INT_STAT ) );
		return FALSE;
	}

	// Busy Check.
	if ( rType == SDHC_RES_R1B_TYPE ) {
		while( Inp32( sCh->m_uBaseAddr+SDHC_PRESENT_STAT ) & (1<<1) );// Check CommandInhibit_DAT
	}
	
	return TRUE;
}

//////////
// File Name : SDHC_GetSdSCR
// File Description : Setting sd card SCR infomation.
// Input : SDHC, temp buffer for gather sd infomation up to 512 byte.
// Output : success of failure.
u8 SDHC_GetSdScr(SDHC* sCh) {
	u32 buffer[2];
	u32 ignore = 0;

	if(!SDHC_IssueCommand( sCh, 16, 8, SDHC_CMD_AC_TYPE, SDHC_RES_R1_TYPE ) )
		return FALSE;

	SDHC_SetBlockSizeReg( sCh, 7, 8);
	SDHC_SetBlockCountReg( sCh, 1);
	SDHC_SetTransferModeReg(0, 1, 0, 0, 0, sCh);
	sCh->m_uRemainBlock = 1;
	sCh->m_uBufferPtr = buffer;

	// CMD55 (For ACMD)
	if (!SDHC_IssueCommand( sCh, 55, sCh->m_uRca<<16, SDHC_CMD_AC_TYPE, SDHC_RES_R1_TYPE ) )
		return FALSE;

	// Acmd51 - Send SCR
	if(!SDHC_IssueCommand( sCh, 51, 0, SDHC_CMD_ADTC_TYPE, SDHC_RES_R1_TYPE ) )
		return FALSE;

#ifdef FEATURE_SDHC_INTERRUPT_CONTROL_MODE
	while( sCh->m_uRemainBlock != 0 )
		ignore++;
#else
	SDHC_ReadOneBlock( sCh );
#endif
	SDHC_INT_WAIT_CLEAR( sCh, 1, ignore );	// SDHC_TRANSFERCOMPLETE_STS_INT_EN

// Transfer mode is determined by capacity register at OCR setting.
//	sCh->m_eTransMode = SDHC_BYTE_MODE;
	if ((*buffer&0xf) == 0x0)
		sCh->m_ucSpecVer = 0; // Version 1.0 ~ 1.01
	else if ((*buffer&0xf) == 0x1)
		sCh->m_ucSpecVer = 1; // Version 1.10, support cmd6
	else if((*buffer&0xf) == 0x2) {
		sCh->m_ucSpecVer = 2; // Version 2.0 support cmd6 and cmd8
//		sCh->m_eTransMode = SDHC_BLOCK_MODE;
	}
	else {
		sCh->m_ucSpecVer = 0; // Error... Deda
	}

	UART_Printf("SDSpecVer=%d\n", sCh->m_ucSpecVer);
	return TRUE;
}


//////////
// File Name : SDHC_ReadMMCExtCSD
// File Description : Read mmc extended CSD Register.
// Input : SDHC channel
// Output : success of failure.
u8 SDHC_ReadMMCExtCSD(SDHC* sCh) {
	u32 S_CMD_SET, uHsTiming, uBusMode;
//	u8 buffer[512]; // - to do - move to another memory. - too much stack.
	u8 *buffer;
	u32 ignore;

//	SDHC_SetSystemAddressReg(sCh, (u32)(&SDHC_globl_testBuffer[0]));// AHB System Address For Write
//	buffer = &SDHC_globl_testBuffer[0];
	
	SDHC_SetBlockSizeReg(sCh, 7, 512); // Maximum DMA Buffer Size, Block Size
	SDHC_SetBlockCountReg(sCh, 1); // Block Numbers to Write
	SDHC_SetTransferModeReg(0, 1, 0, 1, 0, sCh );
	sCh->m_uRemainBlock = 1;
	sCh->m_uBufferPtr = (u32*)SDHC_globl_testBuffer;
	buffer = (u8*)sCh->m_uBufferPtr;
	
	if ( sCh->m_ucSpecVer < 4 ) {
		UART_Printf("SKIP SDHC_ReadMMCExtCSD\n");
		return TRUE;
	}
	if(!SDHC_IssueCommand( sCh, 8, 0, SDHC_CMD_ADTC_TYPE, SDHC_RES_R1_TYPE ))	// cmd8			
		return FALSE;
	
#ifdef FEATURE_SDHC_INTERRUPT_CONTROL_MODE
	while( sCh->m_uRemainBlock != 0 )
		ignore++;
#else
	SDHC_ReadOneBlock( sCh );
#endif
	SDHC_INT_WAIT_CLEAR(sCh, 1, ignore );
	
	S_CMD_SET = buffer[504];
	uHsTiming = buffer[185];
	uBusMode = buffer[183];
	
	UART_Printf( "uHsTiming : %d\n", uHsTiming );
	UART_Printf( "uBusMode : %d\n", uBusMode );
	UART_Printf( "Support Commmand Set : %d\n", buffer[504] );
	UART_Printf( "sector count : %d\n", *((u32*)&buffer[212]) );
	UART_Printf( "min Write performance 8 @52 : %d \n", buffer[210] );
	UART_Printf( "min Read performance 8 @52 : %d \n", buffer[209] );
	UART_Printf( "min Write performance 8 @26 : %d \n", buffer[208] );
	UART_Printf( "min Read performance 8 @26 : %d \n", buffer[207] );
	UART_Printf( "min Write performance 4 @26 : %d \n", buffer[206] );
	UART_Printf( "min Read performance 4 @26 : %d \n", buffer[205] );
	UART_Printf( "Card Type : %d \n", buffer[196] );
	UART_Printf( "CSD structure Version : %d \n", buffer[194] );
	UART_Printf( "CSD Revision : %d\n", buffer[192] );
	UART_Printf( "Command Set : %d\n", buffer[191] );
	UART_Printf( "Command Set Revision : %d\n", buffer[189] );
	UART_Printf( "Power Class :%d \n", buffer[187] );
	UART_Printf( "High Speed Interface Timing : %d\n", buffer[185] );
	UART_Printf( "Bus width timing : %d\n", buffer[183] );
	UART_Printf( "Erased memory content : %d\n", buffer[181] );
	if( buffer[212] | buffer[213] | buffer[214] | buffer[215] ) {
		SDHC_global_card_size = (buffer[212]) | (buffer[213]<<8) | (buffer[214]<<16) | (buffer[215]<<24);
	}
	
	if(S_CMD_SET & (1<<4)) {
		UART_Printf("\n========CE ATA Card detect========\n");
		sCh->m_eCardType = SDHC_CE_ATA_CARD;
	}
	else if(S_CMD_SET & (1<<2))
		UART_Printf("\n========Content Protection SecureMMC Card detect========\n");
	else if(S_CMD_SET & (1<<1))
		UART_Printf("\n========SecureMMC Card detect========\n");
	else if(S_CMD_SET & (1<<0))
		UART_Printf("\n========Standard MMC Card detect========\n");

	return TRUE;
}


//////////
// File Name : SDHC_ClearErrInterruptStatus
// File Description : Clear error interrupt status register.
// Input : SDHC channel
// Output : NONE.
void SDHC_ClearErrInterruptStatus(SDHC* sCh) {
	u16 usSfr, usSfr1;
	usSfr = Inp16(sCh->m_uBaseAddr+SDHC_NORMAL_INT_STAT);

	while (usSfr&(0x1<<15)) {
		usSfr1 = Inp16(sCh->m_uBaseAddr+SDHC_ERROR_INT_STAT);
		Outp16(sCh->m_uBaseAddr+SDHC_ERROR_INT_STAT, usSfr1);
		usSfr = Inp16(sCh->m_uBaseAddr+SDHC_NORMAL_INT_STAT);
	}
}

//////////
// File Name : SDHC_SetMmcOcr
// File Description : Get MMC OCR Register from MMC Card.
// Input : SDHC channel
// Output : success or failure.
u8 SDHC_SetMmcOcr(SDHC* sCh)
{
	u32 i, OCR;

	// Place all cards in the idle state.
	if (!SDHC_IssueCommand( sCh, 0, 0, SDHC_CMD_BC_TYPE, SDHC_RES_NO_TYPE ) )
		return FALSE;

	for (i=0; i<500; i++)
	{
#if 1	// for New Movinand 2007.3.29		
//		SDHC_IssueCommand( sCh, 1, 0x40FF8080, SDHC_CMD_BCR_TYPE, SDHC_RES_R3_TYPE ); // (Ocr:2.7V~3.6V)
		SDHC_IssueCommand( sCh, 1, 0x40FF8000, SDHC_CMD_BCR_TYPE, SDHC_RES_R3_TYPE ); // (Ocr:2.7V~3.6V)
		OCR = Inp32(sCh->m_uBaseAddr+SDHC_RSP0);
#else
		SDHC_IssueCommand( sCh, 1, 0, SDHC_CMD_BCR_TYPE, SDHC_RES_R3_TYPE );
		OCR = Inp32(sCh->m_uBaseAddr+SDHC_RSP0)|(1<<30);
		SDHC_IssueCommand( sCh, 1, OCR, SDHC_CMD_BCR_TYPE, SDHC_RES_R3_TYPE );
#endif
		if (Inp32(sCh->m_uBaseAddr+SDHC_RSP0)&(unsigned int)(0x1<<31))
		{
			if(OCR & (1<<7))
				UART_Printf("Voltage range : 1.65V ~ 1.95V\n");
			if(OCR & (1<<21))
				UART_Printf("Voltage range: 2.7V ~ 3.4V\n");
			else if(OCR & (1<<20))
				UART_Printf("Voltage range: 2.7V ~ 3.3V\n");
			else if(OCR & (1<<19))
				UART_Printf("Voltage range: 2.7V ~ 3.2V\n");
			else if(OCR & (1<<18))
				UART_Printf("Voltage range: 2.7V ~ 3.1V\n");
			else
				continue;

			if ( Inp32(sCh->m_uBaseAddr+SDHC_RSP0) &(unsigned int)(0x1<<30) ) {
				UART_Printf("block mode\n");
				sCh->m_ucSpecVer=4;	// default spec version
				sCh->m_eTransMode=SDHC_BLOCK_MODE;
			}
			else {
				UART_Printf("byte mode\n");
				sCh->m_ucSpecVer=1;	// default spec version
				sCh->m_eTransMode=SDHC_BYTE_MODE;
			}
			sCh->m_eCardType= SDHC_MMC_CARD;
			// If the current card is SD card, then there's time out error, need to be cleared.
			SDHC_ClearErrInterruptStatus(sCh);
			return TRUE;
		}
	}

	// If the current card is SD card, then there's time out error, need to be cleared.
	SDHC_ClearErrInterruptStatus(sCh);
	return false;
}


//////////
// File Name : SDHC_SetSDOCR
// File Description : Get SD OCR Register from SD Card.
// Input : SDHC channel
// Output : success or failure.
u8 SDHC_SetSDOCR(SDHC* sCh)
{
	u32 i, OCR;

	// Place all cards in the idle state.
	if (!SDHC_IssueCommand( sCh, 0, 0, SDHC_CMD_BC_TYPE, SDHC_RES_NO_TYPE ) )
		return FALSE;
#ifdef FEATURE_SDHC_HIGH_CAPACITY_CARD_SUPPORT
	if ( SDHC_IssueCommand( sCh, 8, (0x1<<8)|(0xaa), SDHC_CMD_BCR_TYPE, SDHC_RES_R7_TYPE ) == TRUE ) {
		UART_Printf ( "CMD8 return TRUE\n" );
	}
	// Normal SD has Command Response Error.
	SDHC_ClearErrInterruptStatus(sCh);
#endif

	for(i=0; i<500; i++)
	{
		// CMD55 (For ACMD)
		SDHC_IssueCommand( sCh, 55, 0, SDHC_CMD_AC_TYPE, SDHC_RES_R1_TYPE );
		// (Ocr:2.7V~3.6V)
		SDHC_IssueCommand( sCh, 41, 0x40ff8000, SDHC_CMD_BCR_TYPE, SDHC_RES_R3_TYPE );

		if (Inp32(sCh->m_uBaseAddr+SDHC_RSP0)&(unsigned int)(0x1<<31))
		{
			OCR = Inp32( sCh->m_uBaseAddr+SDHC_RSP0);
			//UART_Printf("\nrHM_RSPREG0=%x",rHM_RSPREG0);
			if(OCR & (1<<7))
				UART_Printf("Voltage range : 1.65V ~ 1.95V\n");
			if(OCR & (1<<21))
				UART_Printf("Voltage range: 2.7V ~ 3.4V\n");
			else if(OCR & (1<<20))
				UART_Printf("Voltage range: 2.7V ~ 3.3V\n");
			else if(OCR & (1<<21))
				UART_Printf("Voltage range: 2.7V ~ 3.4V\n");
			else if(OCR & (1<<23))
				UART_Printf("Voltage range: 2.7V ~ 3.6V\n");

			if(OCR&(0x1<<30)) {
				sCh->m_eTransMode = SDHC_BLOCK_MODE;
				UART_Printf("High Capacity Card\n");
			}
			else {
				sCh->m_eTransMode = SDHC_BYTE_MODE;
				UART_Printf("Byte mode\n");
			}
			// Normal SD has Command Response Error.
			SDHC_ClearErrInterruptStatus(sCh);

			sCh->m_eCardType = SDHC_SD_CARD;
			return TRUE;
		}
	}
	// The current card is MMC card, then there's time out error, need to be cleared.
	SDHC_ClearErrInterruptStatus(sCh);
	return FALSE;
}

void SDHC_Set_InitClock( SDHC* sCh ) {
	Outp32( sCh->m_uBaseAddr+SDHC_CONTROL2, (0x0<<15)|(0x0<<14)|(0x1<<8)|(SDHC_HCLK<<4) );
	Outp32( sCh->m_uBaseAddr+SDHC_CONTROL3, (0<<31) | (0<<23) | (0<<15) | (0<<7) );

	// SDCLK Value Setting + Internal Clock Enable
	Outp16( sCh->m_uBaseAddr+SDHC_CLK_CTRL,
		(Inp16(sCh->m_uBaseAddr+SDHC_CLK_CTRL)&(~(0xff<<8)))|(0x80<<8)|(1<<0) );

	// CheckInternalClockStable
	while (!(Inp16( sCh->m_uBaseAddr+SDHC_CLK_CTRL )&0x2));

#ifdef FEATURE_ASYNC_CLOCK_DOMAIN_SUPPORT
	Outp32( sCh->m_uBaseAddr+SDHC_CONTROL2, Inp32(sCh->m_uBaseAddr+SDHC_CONTROL2) | (1<<31) );
#endif

	SDHC_SetSdClockOnOff( TRUE, sCh);
	UART_Printf("rHM_CONTROL2 = %x\n",Inp32( sCh->m_uBaseAddr+SDHC_CONTROL2 ));
	UART_Printf("rHM_CLKCON = %x\n",Inp16( sCh->m_uBaseAddr+SDHC_CLK_CTRL ));
}


//////////
// File Name : SDHC_SetSDOCR
// File Description : Get SD OCR Register from SD Card.
// Input : SDHC channel
// Output : success or failure.
void SDHC_SetSdClock(u32 uDivisor, SDHC* sCh, SDHC_SpeedMode speed, u32 workingFreq )
{
	if(speed==SDHC_NORMAL_SPEED) {
		Outp32( sCh->m_uBaseAddr+SDHC_CONTROL2, (1<<30)|(0<<15)|(0<<14)|(0x1<<8)|(sCh->m_eClockSource<<4) );
		Outp32( sCh->m_uBaseAddr+SDHC_CONTROL3, (0<<31)|(0<<23)|(0<<15)|(0<<7) );
	}
	else {
		// SD : Setup Time 6ns, Hold Time 2ns
		if ( sCh->m_eCardType == SDHC_SD_CARD || sCh->m_eCardType == SDHC_SDIO_CARD ) {
			Outp32( sCh->m_uBaseAddr+SDHC_CONTROL2, (1<<30)|(0<<15)|(0<<14)|(0x1<<8)|(sCh->m_eClockSource<<4) );
			Outp32( sCh->m_uBaseAddr+SDHC_CONTROL3, (0<<31)|(0<<23)|(0<<15)|(0<<7) );
		}
		// MMC : setup time : 5ns, hold time 5ns
		else if ( sCh->m_eCardType == SDHC_MMC_CARD || sCh->m_eCardType == SDHC_CE_ATA_CARD ) {
			Outp32( sCh->m_uBaseAddr+SDHC_CONTROL2, (1<<30)|(0<<15)|(1<<14)|(0x1<<8)|(sCh->m_eClockSource<<4) );
			Outp32( sCh->m_uBaseAddr+SDHC_CONTROL3, (0<<31)|(0<<23)|(1<<15)|(1<<7) );	// for movinand card.
		}
		else {
			Assert( "Not support card type");
		}
	}

	// SDCLK Value Setting + Internal Clock Enable
	Outp16( sCh->m_uBaseAddr+SDHC_CLK_CTRL,
		(Inp16(sCh->m_uBaseAddr+SDHC_CLK_CTRL)&(~(0xff<<8)))|(uDivisor<<8)|(1<<0) );

	// CheckInternalClockStable
	while (!(Inp16( sCh->m_uBaseAddr+SDHC_CLK_CTRL )&0x2));

#ifdef FEATURE_ASYNC_CLOCK_DOMAIN_SUPPORT
	Outp32( sCh->m_uBaseAddr+SDHC_CONTROL2, Inp32(sCh->m_uBaseAddr+SDHC_CONTROL2) | (1<<31) );
#endif

	SDHC_SetSdClockOnOff( TRUE, sCh);
//	UART_Printf("rHM_CONTROL2 = %x\n",Inp32( sCh->m_uBaseAddr+SDHC_CONTROL2 ));
//	UART_Printf("rHM_CLKCON = %x\n",Inp16( sCh->m_uBaseAddr+SDHC_CLK_CTRL ));
}

//////////
// File Name : SDHC_SetSdClockOnOff
// File Description : On/Off Sd Clock
// Input : On or Off, SDHC Channel
// Output : NONE.
void SDHC_SetSdClockOnOff(u8 uOnOff, SDHC* sCh)
{
	if (uOnOff == FALSE)
	{
		Outp16( sCh->m_uBaseAddr+SDHC_CLK_CTRL,
			Inp16(sCh->m_uBaseAddr+SDHC_CLK_CTRL)&(~(1<<2)) );	// SD Clock disable
	}
	else
 	{
		Outp16( sCh->m_uBaseAddr+SDHC_CLK_CTRL,
			Inp16(sCh->m_uBaseAddr+SDHC_CLK_CTRL)|(1<<2) );		// SD Clock enable
		while ( !( Inp16( sCh->m_uBaseAddr+SDHC_CLK_CTRL ) & (1<<3) ) );// SDHC_clockSource is Stable
	}
}

//////////
// File Name : SDHC_SetDataTransferWidth
// File Description : Set SD/MMC Host and Card data transfer width.
// Input : SDHC Channel
// Output : success or failure.
u8 SDHC_SetDataTransferWidth( SDHC* sCh)
{
	u32 uArg=0;

	SDHC_SetSdhcCardIntEnable(FALSE, sCh); // Disable sd card interrupt

	// bandwidth check
	if ( sCh->m_ucBandwidth == 8 ) {
		if ( sCh->m_eCardType == SDHC_SD_CARD ) {
			sCh->m_ucBandwidth = 4;
		}
	}
	else if ( !(sCh->m_ucBandwidth == 1||sCh->m_ucBandwidth == 4) )
		sCh->m_ucBandwidth = 1;		// default bandwidth = 1

	// SDHC_SD_CARD
	if ( sCh->m_eCardType == SDHC_SD_CARD ) {
		// Application Command.
		if (!SDHC_IssueCommand( sCh, 55, sCh->m_uRca<<16, SDHC_CMD_AC_TYPE, SDHC_RES_R1_TYPE) )
			return false;

		// ACMD6 - Set Bus Width. 0->1bit, 2->4bit
		if( !SDHC_IssueCommand( sCh, 6, (sCh->m_ucBandwidth==1)?(0):(2), SDHC_CMD_AC_TYPE, SDHC_RES_R1_TYPE ) )
			return FALSE;
	}
	else if ( sCh->m_eCardType == SDHC_MMC_CARD || sCh->m_eCardType == SDHC_CE_ATA_CARD  ) {
		if (sCh->m_ucSpecVer >= 4) // It is for a newest MMC Card
		{
			uArg = (3<<24)|(183<<16);
			if (sCh->m_ucBandwidth==1)
				uArg |= (0<<8); //  1-bit bus
			else if (sCh->m_ucBandwidth==4)
				uArg |= (1<<8); //  4-bit bus
			else
				uArg |= (2<<8); // 8-bit bus
			// Refer to p.37~38, p.53~54 & p.83 of "MMC Card system Spec. ver4.0"
			// Refer to Lee's Spec and Add for 8-bit mode
			if( !SDHC_IssueCommand( sCh, 6, uArg, SDHC_CMD_AC_TYPE, SDHC_RES_R1B_TYPE ))
				return FALSE;
		}
		else
			sCh->m_ucBandwidth = 1;	// 1 bit bus.
	}

	// default 1 bit bus mode...
	Outp8( sCh->m_uBaseAddr+SDHC_HOST_CTRL,
		Inp8( sCh->m_uBaseAddr+SDHC_HOST_CTRL)& ~((1<<5)|(1<<1)) );
	if( sCh->m_ucBandwidth == 8 ) {
		Outp8( sCh->m_uBaseAddr+SDHC_HOST_CTRL,
			Inp8( sCh->m_uBaseAddr+SDHC_HOST_CTRL)|(1<<5) ); // 8 bit bus mode.
	}
	else if( sCh->m_ucBandwidth == 4 ) {
		Outp8( sCh->m_uBaseAddr+SDHC_HOST_CTRL,
			Inp8( sCh->m_uBaseAddr+SDHC_HOST_CTRL)|(1<<1) ); // 4 bit bus mode.
	}
	
	SDHC_SetSdhcCardIntEnable(TRUE, sCh);  // Enable sd card interrupt

	return TRUE;
}

//////////
// File Name : SDHC_SetHostCtrlSpeedMode
// File Description : Set SD/MMC Host Speed
// Input : Speed mode, SDHC Channel
// Output : NONE
void SDHC_SetHostCtrlSpeedMode(SDHC_SpeedMode eSpeedMode, SDHC* sCh)
{
// Host controller tx works at the failling edge.
#ifdef FEATURE_SDHC_HIGH_SPEED_SUPPORT
	if ( eSpeedMode == SDHC_HIGH_SPEED )
	{
		Outp8( sCh->m_uBaseAddr+SDHC_HOST_CTRL,
			Inp8(sCh->m_uBaseAddr+SDHC_HOST_CTRL)|(1<<2) );	// High Speed mode.
	}
	else
#endif
	{
		Outp8( sCh->m_uBaseAddr+SDHC_HOST_CTRL,
			Inp8(sCh->m_uBaseAddr+SDHC_HOST_CTRL)&~(1<<2) );	// Normal Speed mode.
	}
}

//////////
// File Name : SDHC_SetTransferModeReg
// File Description : Set host controller transfer mode.
// Input : multi block, read or write, auto command 12 on/off, block count enable or not, DMA enable or not, SDHC Channel
// Output : NONE
void SDHC_SetTransferModeReg(u32 MultiBlk, u32 DataDirection, u32 AutoCmd12En, u32 BlockCntEn, u32 DmaEn, SDHC* sCh)
{
	sCh->m_usTransMode = (u16)((MultiBlk<<5)|(DataDirection<<4)|(AutoCmd12En<<2)|(BlockCntEn<<1)|(DmaEn<<0));
	Outp16( sCh->m_uBaseAddr+SDHC_TRANS_MODE, sCh->m_usTransMode);
}

//////////
// File Name : SDHC_SetSdhcInterruptEnable
// File Description : Setting normal and error interrupt.
// Input : Normal interrupt, Error interrupt, SDHC
// Output : NONE.
void SDHC_SetSdhcInterruptEnable(u16 uNormalIntStatusEn, u16 uErrorIntStatusEn, u16 uNormalIntSigEn, u16 uErrorIntSigEn, SDHC* sCh)
{
	SDHC_ClearErrInterruptStatus(sCh);
	Outp16( sCh->m_uBaseAddr+SDHC_NORMAL_INT_STAT_ENABLE, uNormalIntStatusEn);
	Outp16( sCh->m_uBaseAddr+SDHC_ERROR_INT_STAT_ENABLE, uErrorIntStatusEn);
	Outp16( sCh->m_uBaseAddr+SDHC_NORMAL_INT_SIGNAL_ENABLE, uNormalIntSigEn);
	Outp16( sCh->m_uBaseAddr+SDHC_ERROR_INT_SIGNAL_ENABLE, uErrorIntSigEn);
}

//////////
// File Name : SDHC_SetSdhcCardIntEnable
// File Description : Setting normal and error interrupt.
// Input : SDHC, Normal interrupt, Error interrupt.
// Output : NONE.
void SDHC_SetSdhcCardIntEnable(u8 ucTemp, SDHC* sCh)
{
	u16 usSfr;

	usSfr = Inp16( sCh->m_uBaseAddr+SDHC_NORMAL_INT_STAT_ENABLE);
	usSfr = usSfr & 0xFEFF;
	usSfr |= (ucTemp<<8);
	Outp16( sCh->m_uBaseAddr+SDHC_NORMAL_INT_STAT_ENABLE, usSfr );
}

//////////
// File Name : SDHC_SetSdCardSpeedMode
// File Description : Setting speed mode inside SD card.
// Input : SDHC, SDHC_SpeedMode, temp buffer for operating up to 512 byte.
// Output : NONE.
u8 SDHC_SetSdCardSpeedMode(SDHC_SpeedMode eSpeedMode, SDHC* sCh)
{
	u32 buffer[16];
	u32 uArg = 0;
	u32 ignore;

	// CMD16
	if(!SDHC_IssueCommand( sCh, 16, 64, SDHC_CMD_AC_TYPE, SDHC_RES_R1_TYPE ) ) {
		UART_Printf("CMD16 fail\n");
		return FALSE;
	}

	SDHC_SetBlockSizeReg(sCh, 7, 64);
	SDHC_SetBlockCountReg(sCh, 1);
	SDHC_SetTransferModeReg(0, 1, 0, 0, 0,sCh);
	sCh->m_uRemainBlock = 1;
	sCh->m_uBufferPtr = buffer;

	// High Speed = 1, Normal Speed = 0
	uArg = (0x1<<31)|(0xffff<<8)|(eSpeedMode);

	if( !SDHC_IssueCommand( sCh, 6, uArg, SDHC_CMD_ADTC_TYPE, SDHC_RES_R1_TYPE ) ) {
		UART_Printf("CMD6 fail\n");
		return FALSE;
	}

#ifdef FEATURE_SDHC_INTERRUPT_CONTROL_MODE
	while( sCh->m_uRemainBlock != 0 )
		ignore++;
#else
	SDHC_ReadOneBlock( sCh );
#endif
	SDHC_INT_WAIT_CLEAR( sCh, 1, ignore ); // SDHC_TRANSFERCOMPLETE_STS_INT_EN
	
	if ( buffer[3] & (1<<9) ) { // Function Group 1 <- access mode.
		UART_Printf( "This Media support high speed mode.\n" );
	}
	else {
		UART_Printf( "This Media can't support high speed mode.\n" );
	}
	return TRUE;
}

//////////
// File Name : SDHC_SetMmcSpeedMode
// File Description : Setting speed mode inside MMC card.
// Input : SDHC, SDHC_SpeedMode
// Output : NONE.
u8 SDHC_SetMmcSpeedMode(SDHC_SpeedMode eSpeedMode, SDHC* sCh)
{
	return SDHC_IssueCommand( sCh, 6, (3<<24)|(185<<16)|(eSpeedMode<<8), SDHC_CMD_AC_TYPE, SDHC_RES_R1B_TYPE);
}


//////////
// File Name : SDHC_DisplayCardInformation
// File Description : Display and set card CSD information after CSD command.
// Input : SDHC.
// Output : NONE.
void SDHC_DisplayCardInformation(SDHC * sCh)
{
	u32 CardSize, OneBlockSize;
	
	if(sCh->m_eCardType == SDHC_MMC_CARD)
	{
		sCh->m_ucSpecVer=(Inp32( sCh->m_uBaseAddr+SDHC_RSP3 )>>18)& 0xF;
		
		UART_Printf("=>  m_ucSpecVer=%d\n", sCh->m_ucSpecVer);
	}

	sCh->m_sReadBlockLen = (u16)((Inp32( sCh->m_uBaseAddr+SDHC_RSP2 )>>8) & 0xf);
	sCh->m_sReadBlockPartial = (u16)((Inp32( sCh->m_uBaseAddr+SDHC_RSP2 )>>7) & 0x1);
	sCh->m_sCSize = (u16)(((Inp32( sCh->m_uBaseAddr+SDHC_RSP2 ) & 0x3) << 10) | ((Inp32( sCh->m_uBaseAddr+SDHC_RSP1 ) >> 22) & 0x3ff));
	sCh->m_sCSizeMult = (u16)((Inp32( sCh->m_uBaseAddr+SDHC_RSP1 )>>7)&0x7);
	
	CardSize = ((u32)(1<<sCh->m_sReadBlockLen))*(sCh->m_sCSize+1)*(1<<(sCh->m_sCSizeMult+2))/1048576;
	OneBlockSize = (1<<sCh->m_sReadBlockLen);
	SDHC_global_card_size = ((u32)(1<<sCh->m_sReadBlockLen))*(sCh->m_sCSize+1)*(1<<(sCh->m_sCSizeMult+2))/512;
	
	UART_Printf("READ_BL_LEN: %d\n",sCh->m_sReadBlockLen);
	UART_Printf("READ_BL_PARTIAL: %d\n",sCh->m_sReadBlockPartial);
	UART_Printf("C_SIZE: %d\n",sCh->m_sCSize);
	UART_Printf("C_SIZE_MULT: %d\n",sCh->m_sCSizeMult);

	UART_Printf("One Block Size: %dByte\n",OneBlockSize);
	UART_Printf("Total Card Size: %dMByte\n",CardSize+1);
}


//////////
// File Name : SDHC_SetGPIO
// File Description : Setting GPIO for 6400 high speed MMC.
// Input : Channel Number, Channel Line width.
// Output : success or failure.
u8 SDHC_SetGPIO(SDHC_channel channelNum, int channel_width)
{
	if( channelNum == SDHC_CHANNEL_1 ) {	// Channel 1 ( 1, 4, 8 Channel width )
		GPIO_SetFunctionEach( eGPIO_G, eGPIO_6, 0x3 );	// MMC CDn0 - MMC CDn1, EINT5[6], GPG6
		GPIO_SetFunctionEach( eGPIO_H, eGPIO_0, 0x2 );	// MMC CLK1 - ADDR_CF[0], EINT6[0], GPH0
		GPIO_SetFunctionEach( eGPIO_H, eGPIO_1, 0x2 );	// MMC CMD1 - ADDR_CF[1], EINT6[1], GPH1
		GPIO_SetFunctionEach( eGPIO_H, eGPIO_2, 0x2 );	// MMC DATA1[0] - ADDR_CF[2], EINT6[2], GPH2
		GPIO_SetPullUpDownEach( eGPIO_G, eGPIO_6, 0 );	// MMC CDn0 - MMC CDn1, EINT5[6], GPG6
		GPIO_SetPullUpDownEach( eGPIO_H, eGPIO_0, 0 );	// MMC CLK1 - ADDR_CF[0], EINT6[0], GPH0
		GPIO_SetPullUpDownEach( eGPIO_H, eGPIO_1, 0 );	// MMC CMD1 - ADDR_CF[1], EINT6[1], GPH1
		GPIO_SetPullUpDownEach( eGPIO_H, eGPIO_2, 0 );	// MMC DATA1[0] - ADDR_CF[2], EINT6[2], GPH2
		if ( channel_width == 1 )
			return TRUE;
		GPIO_SetFunctionEach( eGPIO_H, eGPIO_3, 0x2 );	// MMC DATA1[1] - EINT6[3], GPH3
		GPIO_SetFunctionEach( eGPIO_H, eGPIO_4, 0x2 );	// MMC DATA1[2] - EINT6[4], GPH4
		GPIO_SetFunctionEach( eGPIO_H, eGPIO_5, 0x2 );	// MMC DATA1[3] - EINT6[5], GPH5
		GPIO_SetPullUpDownEach( eGPIO_H, eGPIO_3, 0 );	// MMC DATA1[1] - EINT6[3], GPH3
		GPIO_SetPullUpDownEach( eGPIO_H, eGPIO_4, 0 );	// MMC DATA1[2] - EINT6[4], GPH4
		GPIO_SetPullUpDownEach( eGPIO_H, eGPIO_5, 0 );	// MMC DATA1[3] - EINT6[5], GPH5
		if ( channel_width == 4 )
			return TRUE;
		GPIO_SetFunctionEach( eGPIO_H, eGPIO_6, 0x2 );	// MMC DATA1[4] - MMC Data2[0], Key pad COL[6], ADDR_CF[0], EINT6[6]
		GPIO_SetFunctionEach( eGPIO_H, eGPIO_7, 0x2 );	// MMC DATA1[5] - MMC Data2[1], Key pad COL[7], ADDR_CF[1], EINT6[7]
		GPIO_SetFunctionEach( eGPIO_H, eGPIO_8, 0x2 );	// MMC DATA1[6] - MMC Data2[2], ADDR_CF[2], EINT6[8]
		GPIO_SetFunctionEach( eGPIO_H, eGPIO_9, 0x2 );	// MMC DATA1[7] - MMC Data2[3], EINT6[9]
		GPIO_SetPullUpDownEach( eGPIO_H, eGPIO_6, 0 );	// MMC DATA1[4] - MMC Data2[0], Key pad COL[6], ADDR_CF[0], EINT6[6]
		GPIO_SetPullUpDownEach( eGPIO_H, eGPIO_7, 0 );	// MMC DATA1[5] - MMC Data2[1], Key pad COL[7], ADDR_CF[1], EINT6[7]
		GPIO_SetPullUpDownEach( eGPIO_H, eGPIO_8, 0 );	// MMC DATA1[6] - MMC Data2[2], ADDR_CF[2], EINT6[8]
		GPIO_SetPullUpDownEach( eGPIO_H, eGPIO_9, 0 );	// MMC DATA1[7] - MMC Data2[3], EINT6[9]
	}
	else if( channelNum == SDHC_CHANNEL_2 ) {	// Channel 2 ( 1, 4 Channel width )
		GPIO_SetFunctionEach( eGPIO_C, eGPIO_4, 0x3 );	// MMC CMD2 -SPI MISO[1], GPC4
		GPIO_SetFunctionEach( eGPIO_C, eGPIO_5, 0x3 );	// MMC CLK2 -SPI CLK[1], GPC5
		GPIO_SetFunctionEach( eGPIO_H, eGPIO_6, 0x3 );	// MMC Data2[0] - MMC DATA1[4], Key pad COL[6], ADDR_CF[0], EINT6[6]
		GPIO_SetPullUpDownEach( eGPIO_C, eGPIO_4, 0 );	// MMC CMD2 -SPI MISO[1], GPC4
		GPIO_SetPullUpDownEach( eGPIO_C, eGPIO_5, 0 );	// MMC CLK2 -SPI CLK[1], GPC5
		GPIO_SetPullUpDownEach( eGPIO_H, eGPIO_6, 0 );	// MMC Data2[0] - MMC DATA1[4], Key pad COL[6], ADDR_CF[0], EINT6[6]

		// There is no card detection pin at channel-2.
		if ( channel_width == 1 )
			return TRUE;
		GPIO_SetFunctionEach( eGPIO_H, eGPIO_7, 0x3 );	// MMC Data2[1] - MMC DATA1[5], Key pad COL[7], ADDR_CF[1], EINT6[7]
		GPIO_SetFunctionEach( eGPIO_H, eGPIO_8, 0x3 );	// MMC Data2[2] - MMC DATA1[6], ADDR_CF[2], EINT6[8]
		GPIO_SetFunctionEach( eGPIO_H, eGPIO_9, 0x3 );	// MMC Data2[3] - MMC DATA1[7], EINT6[9]
		GPIO_SetPullUpDownEach( eGPIO_H, eGPIO_7, 0);	// MMC Data2[1] - MMC DATA1[5], Key pad COL[7], ADDR_CF[1], EINT6[7]
		GPIO_SetPullUpDownEach( eGPIO_H, eGPIO_8, 0 );	// MMC Data2[2] - MMC DATA1[6], ADDR_CF[2], EINT6[8]
		GPIO_SetPullUpDownEach( eGPIO_H, eGPIO_9, 0 );	// MMC Data2[3] - MMC DATA1[7], EINT6[9]
	}
	else {	// default channel 0
		GPIO_SetFunctionEach( eGPIO_G, eGPIO_0, 0x2 );	// MMC CLK0 - ADDR_CF[0], EINT5[0], GPG0
		GPIO_SetFunctionEach( eGPIO_G, eGPIO_1, 0x2 );	// MMC CMD0 - ADDR_CF[1], EINT5[1], GPG1
		GPIO_SetFunctionEach( eGPIO_G, eGPIO_6, 0x2 );	// MMC CDn0 - MMC CDn1, EINT5[6], GPG6
		GPIO_SetFunctionEach( eGPIO_G, eGPIO_2, 0x2 );	// MMC DATA0[0] - ADDR_CF[2], EINT5[2], GPG2
		GPIO_SetPullUpDownEach( eGPIO_G, eGPIO_0, 0 );	// MMC CLK0 - ADDR_CF[0], EINT5[0], GPG0
		GPIO_SetPullUpDownEach( eGPIO_G, eGPIO_1, 0 );	// MMC CMD0 - ADDR_CF[1], EINT5[1], GPG1
		GPIO_SetPullUpDownEach( eGPIO_G, eGPIO_6, 0 );	// MMC CDn0 - MMC CDn1, EINT5[6], GPG6
		GPIO_SetPullUpDownEach( eGPIO_G, eGPIO_2, 0 );	// MMC DATA0[0] - ADDR_CF[2], EINT5[2], GPG2

		if ( channel_width == 1 )
			return TRUE;
		GPIO_SetFunctionEach( eGPIO_G, eGPIO_3, 0x2 );	// MMC DATA0[1] - EINT5[3], GPG3
		GPIO_SetFunctionEach( eGPIO_G, eGPIO_4, 0x2 );	// MMC DATA0[2] - EINT5[4], GPG4
		GPIO_SetFunctionEach( eGPIO_G, eGPIO_5, 0x2 );	// MMC DATA0[3] - EINT5[5], GPG5
		GPIO_SetPullUpDownEach( eGPIO_G, eGPIO_3, 0 );	// MMC DATA0[1] - EINT5[3], GPG3
		GPIO_SetPullUpDownEach( eGPIO_G, eGPIO_4, 0 );	// MMC DATA0[2] - EINT5[4], GPG4
		GPIO_SetPullUpDownEach( eGPIO_G, eGPIO_5, 0 );	// MMC DATA0[3] - EINT5[5], GPG5
	}
	return TRUE;
}

