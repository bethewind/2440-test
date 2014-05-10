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
*	File Name : sdhc_test.c
*  
*	File Description : This file implements the API functon for High Speed MMC.
*
*	Author : Youngbo.song
*	Dept. : AP Development Team
*	Created Date : 08.JAN.2007
*	Version : 0.1
* 
*	History
*	 1) 1st Made
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
#include "fat.h"
#include "timer.h"

#define SDHC_TEST_CHANNEL SDHC_CHANNEL_0
#define SDHC_MOVINAND_BOOTLOAD_ADDRESS 0x50500000

//static INTC oIntc;
SDHC SDHC_descriptor;


/*
 * print setting values.
 */
void SDHC_DispCurrParams(void)
{
	SDHC* card = &SDHC_descriptor;
	UART_Printf("\n");

	if (card->m_eClockSource== SDHC_HCLK)
		UART_Printf(" eClkSrc = SDHC_HCLK,");
	else if (card->m_eClockSource == SDHC_EPLL)
		UART_Printf(" eClkSrc = SDHC_EPLL,");
	else if (card->m_eClockSource == SDHC_EXTCLK)
		UART_Printf("eClkSrc = SDHC_EXTCLK,");
	else
		UART_Printf("No Clock Selected.");
	
	UART_Printf(" uClkDiv = %d,", card->m_uClockDivision);
	UART_Printf(" uBusWidth = %d", card->m_ucBandwidth);
	UART_Printf(" uStBlock = %d, uBlocks = %d, ", card->m_uStartBlockPos, card->m_uOneBlockSize);

	if ( card->m_eOpMode == SDHC_POLLING_MODE )
		UART_Printf("SDHC_POLLING_MODE, ");
	else if ( card->m_eOpMode == SDHC_INTERRUPT_MODE )
		UART_Printf("SDHC_INTERRUPT_MODE, ");
	else if ( card->m_eOpMode == SDHC_SDMA_MODE )
		UART_Printf("SDHC_SDMA_MODE, ");
	else
		UART_Printf( "NO Operation Mode, " );
	UART_Printf("\n");
}


//////////
// File Name : SDHC_getBuffer
// File Description : get from mmc_test_buffer  from non-cachable area.
// Input : buffer index(per 1MB), is sequence data, clear buffer.
// Output : buffer pointer.
u32* SDHC_getBuffer(int index, u8 sequence, u8 clear) {
	u32* result, *p;
	int buffer_size = 0x100000;
	int i;
	int reqNum;
	
	result = (u32*)(0x51000000 + index * buffer_size);
	
	if ( clear == TRUE ) {
		memset( result, 0, buffer_size );
	}
	p=result;
	if ( sequence == TRUE ) {
		reqNum = (int)(rand() & 0xfffffff);	// auto offset.
		for(i=buffer_size>>2;i>0;i--) {
			*p++=reqNum++;
			//*p++ = (i&1)?(0x55555555):(0xaaaaaaaa);
		}
	}
	return result;
}

//////////
// File Name : SDHC_DataCompare
// File Description : data compare function.
// Input : source1 pointer, source2 pointer, compare size( byte size ).
// Output : NONE.
u8 SDHC_DataCompare(u32* source1, u32* source2, u32 bytes)
{	
	u32  ErrCnt = 0;
	u32 i;

	bytes = bytes >> 2;	// consider u32 type.
	for (i=0; i<bytes; i++) {
		if (*source1 != *source2) {
			if ( ErrCnt < 5 ) {
				UART_Printf("%08x=%08x <-> %08x=%08x\n", source1, *source1, source2, *source2);
			}
			ErrCnt++;
		}
		source1++;
		source2++;
	}

	if(ErrCnt == 0) {
		return TRUE;
	}
	UART_Printf("Total Error cnt = %d\n",ErrCnt);
	return FALSE;
}


/**
 * Single Card write-read test.
 */
void SDHC_ComplexRWTest(void)
{
	SDHC* oSdhc;
	u32* uSrcAddr = SDHC_getBuffer(0, TRUE, TRUE);
	u32* uDstAddr = SDHC_getBuffer(1, FALSE, TRUE);
	u32 startAddress, blockSize;
	u8 result;
	
	oSdhc = &SDHC_descriptor;

	blockSize = oSdhc->m_uOneBlockSize;
	startAddress = oSdhc->m_uStartBlockPos;

//	Outp32( oSdhc->m_uBaseAddr+SDHC_CONTROL2, Inp32(oSdhc->m_uBaseAddr+SDHC_CONTROL2)|
//		(1<<6)|(1<<3)|(1<<2)|(1<<1)|(1<<0) );

	if (!SDHC_OpenMediaWithMode(oSdhc->m_ucBandwidth, oSdhc->m_eOpMode, oSdhc->m_eClockSource,
								oSdhc->m_uClockDivision, oSdhc->m_eChannel, oSdhc)) {
		UART_Printf(" SDHC_OpenMediaWithMode() failed !!!\n");
		return;
	}

	result = SDHC_WriteBlocks(startAddress, blockSize, (u32)uSrcAddr, oSdhc);
	if (result == 0)
		UART_Printf(" Writing done\n");
	else
		UART_Printf(" Writing failed !!%d\n", result);

	result = SDHC_ReadBlocks(startAddress, blockSize, (u32)uDstAddr, oSdhc);
	if (result == 0)
		UART_Printf(" Reading done\n" );
	else
		UART_Printf(" Reading failed !!%d\n", result);

	SDHC_CloseMedia(oSdhc);

	if (SDHC_DataCompare( uSrcAddr, uDstAddr, blockSize<<7))
		UART_Printf(" Compare Ok\n");
	else
		UART_Printf(" Mismatches !!\n");
}




/**
 * Single Card write-read test.
 */
void SDHC_ComplexRWTest_FullMemory(void)
{
	SDHC* oSdhc;
	u32* uSrcAddr = SDHC_getBuffer(0, TRUE, TRUE);
	u32* uDstAddr = SDHC_getBuffer(1, FALSE, TRUE);
	u32 startAddress, blockSize;
	u8 result;
	int i;
	
	oSdhc = &SDHC_descriptor;

	blockSize = oSdhc->m_uOneBlockSize;
	startAddress = 0;

//	Outp32( oSdhc->m_uBaseAddr+SDHC_CONTROL2, Inp32(oSdhc->m_uBaseAddr+SDHC_CONTROL2)|
//		(1<<6)|(1<<3)|(1<<2)|(1<<1)|(1<<0) );

	for(i=0;i<10000;i++) {

		if (!SDHC_OpenMediaWithMode(oSdhc->m_ucBandwidth, oSdhc->m_eOpMode, oSdhc->m_eClockSource,
									oSdhc->m_uClockDivision, oSdhc->m_eChannel, oSdhc)) {
			UART_Printf(" SDHC_OpenMediaWithMode() failed !!!\n");
			break;
		}

		result = SDHC_WriteBlocks(startAddress, blockSize, (u32)uSrcAddr, oSdhc);
		if (result == 0)
			UART_Printf(" Writing done\n");
		else {
			UART_Printf(" Writing failed !!%d\n", result);
			break;
		}

		result = SDHC_ReadBlocks(startAddress, blockSize, (u32)uDstAddr, oSdhc);
		if (result == 0)
			UART_Printf(" Reading done\n" );
		else {
			UART_Printf(" Reading failed !!%d\n", result);
			break;
		}

		SDHC_CloseMedia(oSdhc);

		if (SDHC_DataCompare( uSrcAddr, uDstAddr, blockSize<<7))
			UART_Printf(" Compare Ok\n");
		else {
			UART_Printf(" Mismatches !!\n");
			break;
		}
		startAddress = startAddress + blockSize;
	}
	while(1);
}



//////////
void SDHC_MoviBootloaderCopy(void) {
	SDHC* oSdhc;
	u32* tempPtr;
	extern unsigned int SDHC_global_card_size;
	unsigned int *source_addr;
	// get 4MB.
	tempPtr = SDHC_getBuffer(3, FALSE, TRUE);
	tempPtr = SDHC_getBuffer(2, FALSE, TRUE);
	tempPtr = SDHC_getBuffer(1, FALSE, TRUE);
	tempPtr = SDHC_getBuffer(0, FALSE, TRUE);

	oSdhc = &SDHC_descriptor;

	if ( !SDHC_OpenMediaWithMode(oSdhc->m_ucBandwidth, oSdhc->m_eOpMode, oSdhc->m_eClockSource,
								oSdhc->m_uClockDivision, oSdhc->m_eChannel, oSdhc)) {
		UART_Printf(" OpenMedia() failed !!!\n");
		return;
	}

	// OS Code.
	source_addr = (unsigned int*)0x50600000;
	// write bootloader2
	if ( SDHC_WriteBlocks(SDHC_global_card_size-(0x8000+18), 0x8000, (u32)source_addr, oSdhc) != 0 ) {
		UART_Printf( "bootloader 2 loading fail\n");
	}
	else {
		UART_Printf( "bootloader 2 loading success\n");
	}
	// read bootloader2
	if ( SDHC_ReadBlocks(SDHC_global_card_size-(0x8000+18), 0x8000, (u32)tempPtr, oSdhc ) != 0 ) {
		UART_Printf( "bootloader 2 reading fail\n");
	}
	else {
		UART_Printf( "bootloader 2 reading success\n");
	}
	// compare bootloader2
	if ( memcmp( (void*)tempPtr, source_addr, 512*0x8000 ) == 0 ) {
		UART_Printf( "compare success\n" );
	}
	else {
		UART_Printf( "OS Compare Error... \n ");
	}

	
	// BL2
	source_addr = (unsigned int*)0x50500000;
	// write bootloader2
	if ( SDHC_WriteBlocks(SDHC_global_card_size-18, 18, (u32)source_addr, oSdhc) != 0 ) {
		UART_Printf( "bootloader 2 loading fail\n");
	}
	else {
		UART_Printf( "bootloader 2 loading success\n");
	}
	// read bootloader2
	if ( SDHC_ReadBlocks(SDHC_global_card_size-18, 18, (u32)tempPtr, oSdhc ) != 0) {
		UART_Printf( "bootloader 2 reading fail\n");
	}
	else {
		UART_Printf( "bootloader 2 reading success\n");
	}
	// compare bootloader2
	if ( memcmp( tempPtr, source_addr, 512*18 ) == 0 ) {
		UART_Printf( "compare success\n" );
	}
	else {
		UART_Printf( "Bootloader 2 Compare Error... \n ");
	}
	


	// close media
	SDHC_CloseMedia(oSdhc);
}

void SDHC_SetStartBlock(void)
{
	int x;

	UART_Printf("Enter the starting block to test :\n");
	x = UART_GetIntNum();

	if (x != -1) {
		SDHC_descriptor.m_uStartBlockPos = x;
	}

	UART_Printf( "start Block addresss : %d\n" , SDHC_descriptor.m_uStartBlockPos );
}

void SDHC_SetBusWidth(void)
{
	u32 x;

	UART_Printf(" Enter the bus width (1, 4, 8): ");
	x = UART_GetIntNum();

	if (x == 1 || x == 4 || x == 8) {
		SDHC_descriptor.m_ucBandwidth = x;
	}
	UART_Printf ( "bus width : %d\n", SDHC_descriptor.m_ucBandwidth );
}
 


void SDHC_SetClockSource(void)
{
	u8 x;

	UART_Printf("Enter the clk source (1=HCLK, 2=EPLL, 3=EXTCLK) : ");
	x = UART_GetIntNum();

	if (x == 2) {
		SDHC_descriptor.m_eClockSource = SDHC_EPLL;
		UART_Printf ( "SDHC_EPLL select\n");
	}
	else if (x == 3) {
		SDHC_descriptor.m_eClockSource = SDHC_EXTCLK;
		UART_Printf ( "SDHC_EXTCLK select\n");
	}
	else {	// x==1 default
		SDHC_descriptor.m_eClockSource = SDHC_HCLK;
		UART_Printf ( "SDHC_HCLK select\n");
	}
}


void SDHC_SetSdClkDivisor(void)
{
	u32 x;

	UART_Printf("Enter the clk divisor ( 1,2,4,8,16,32,64,128,256) : ");
	x = UART_GetIntNum();

	if  (x==0||x == 1||x == 2||x == 4||x == 8||x == 16||x == 32||x == 64||x == 128||x == 256) {
		SDHC_descriptor.m_uClockDivision = x;
	}
	UART_Printf( "Clock divisor : %d\n", SDHC_descriptor.m_uClockDivision );
}


void SDHC_SetNumOfBlocks(void)
{
	int num;

	UART_Printf("Enter the num. of blocks to test : ");
	num = UART_GetIntNum();

	if (num != -1) {
		SDHC_descriptor.m_uOneBlockSize = num;
	}
	UART_Printf("Block size = %d\n" , SDHC_descriptor.m_uOneBlockSize );
}

void SDHC_SetOperationMode(void)
{
	int sel;

	UART_Printf(" 1: SDHC_POLLING_MODE\n");
	UART_Printf(" 2: SDHC_INTERRUPT_MODE\n");
	UART_Printf(" 3: SDHC_SDMA_MODE\n");
	UART_Printf(" 4: SDHC_ADMA2_MODE\n");
	UART_Printf("Select the op mode : ");

	sel = UART_GetIntNum();

	if (sel == 1) {
		SDHC_descriptor.m_eOpMode = SDHC_POLLING_MODE;
		UART_Printf( "SDHC_POLLING_MODE\n");
	}
	else if (sel == 2) {
		SDHC_descriptor.m_eOpMode = SDHC_INTERRUPT_MODE;
		UART_Printf( "SDHC_INTERRUPT_MODE\n");
	}
	else if (sel == 3) {
		SDHC_descriptor.m_eOpMode = SDHC_SDMA_MODE;
		UART_Printf( "SDHC_SDMA_MODE\n");
	}
	else { //sel == 4
		SDHC_descriptor.m_eOpMode = SDHC_ADMA2_MODE;
		UART_Printf( "SDHC_ADMA2_MODE\n");
	}
}

void SDHC_SetChannel(void) {
	int sel;

	UART_Printf(" 1: Channel 0\n");
	UART_Printf(" 2: Channel 1\n");
	UART_Printf(" 3: Channel 2\n");
	UART_Printf("Select the channel: ");

	sel = UART_GetIntNum();

	if (sel == 1) {
		SDHC_descriptor.m_eChannel = SDHC_CHANNEL_0;
		UART_Printf( "SDHC_CHANNEL_0\n");
	}
	else if (sel == 2) {
		SDHC_descriptor.m_eChannel = SDHC_CHANNEL_1;
		UART_Printf( "SDHC_CHANNEL_1\n");
	}
	else if (sel == 3) {
		SDHC_descriptor.m_eChannel = SDHC_CHANNEL_2;
		UART_Printf( "SDHC_CHANNEL_2\n");
	}
	else {
		SDHC_descriptor.m_eChannel = SDHC_CHANNEL_1;
		UART_Printf( "default : SDHC_CHANNEL_1\n");
	}
}


///////////////////////Internal Test Only/////////////////////////////////////////////////////////
typedef struct _SFR_type {
	u32 address;
	u32 size;
} SFR_type;

SFR_type sd_mmc_sfr[] = {
	{SDHC_SYS_ADDR,			31},
	{SDHC_BLK_SIZE,				15},
	{SDHC_BLK_COUNT,			16}, //16-bit SFR
	{SDHC_ARG,					32},
	{SDHC_TRANS_MODE,			10}, //16-bit SFR
	{SDHC_COMMAND,			14}, //16-bit SFR
	{SDHC_BUF_DAT_PORT,		0}, 
	{SDHC_HOST_CTRL,			6}, // 8-bit SFR
	{SDHC_PWR_CTRL,			4}, // 8-bit SFR
	{SDHC_BLOCKGAP_CTRL,		4},// 8-bit SFR
	{SDHC_WAKEUP_CTRL,			3},// 8-bit SFR
	{SDHC_CLK_CTRL,			16}, //16-bit SFR
	{SDHC_TIMEOUT_CTRL,		4}, // 8-bit SFR
	{SDHC_SOFTWARE_RESET,		3},// 8-bit SFR
	{SDHC_NORMAL_INT_STAT,		0},//16-bit SFR
	{SDHC_ERROR_INT_STAT,		0},//16-bit SFR
	{SDHC_NORMAL_INT_STAT_ENABLE,		15},//16-bit SFR
	{SDHC_ERROR_INT_STAT_ENABLE,		10},//16-bit SFR
	{SDHC_NORMAL_INT_SIGNAL_ENABLE,	15}, //16-bit SFR
	{SDHC_ERROR_INT_SIGNAL_ENABLE,		10},//16-bit SFR
	{SDHC_AUTO_CMD12_ERR_STAT,		0}, //16-bit SFR
	{SDHC_CAPA,						27},
	{SDHC_MAX_CURRENT_CAPA,			24},
//	{SDHC_MONITOR						= 0x4C,
	{SDHC_CONTROL2,					32},
		// [15] Feedback clock used for Tx Data/Command logic.
		// [14] Feedback clock used for Rx Data/Command logic.
		// [13] Select card detection signal. 0=nSDCD, 1=DAT[3].
		// [11] CE-ATA I/F mode. 1=Enable, 0=Disable.
		// [8]  SDCLK Hold enable. 
	{SDHC_CONTROL3,					32},
	{SDHC_CONTROL4	,					18},
	{SDHC_FORCE_EVENT_CMD12_ERROR,	0},	// 16-bit SFR
	{SDHC_FORCE_EVENT_NOR_ERROR,		0},	// 16-bit SFR
	{SDHC_ADMA_ERROR,					0},	// 32-bit SFR
	{SDHC_ADMA_SYSTEM_ADDRESS,		32},	// 32-bit SFR
	{SDHC_HOST_CONTROLLER_VERSION,	16}  //16-bit SFR
};

void SDHC_PrintSFRErr( SFR_type* sfr, u32 in, u32 out ) {
	UART_Printf("address[%2x] (%08x)->(%08x)\n", sfr->address, in, out);
}

void SDHC_SFR_Test(void) {
	int i,j;
	SDHC* sCh;
	int sfr_count;

	sCh = &SDHC_descriptor;
	sCh->m_uBaseAddr = (u8*)0x7C400000;

	sfr_count = sizeof(sd_mmc_sfr)/sizeof(SFR_type);

	for(i=0;i<sfr_count;i++) {
		if(sd_mmc_sfr[i].size<=8) {
			for(j=0;j<sd_mmc_sfr[i].size;j++) {
				Outp8( sCh->m_uBaseAddr + sd_mmc_sfr[i].address, (1<<j) );
				if( Inp8( sCh->m_uBaseAddr + sd_mmc_sfr[i].address ) != (1<<j) ) {
					SDHC_PrintSFRErr( &sd_mmc_sfr[i], (1<<j), Inp8( sCh->m_uBaseAddr + sd_mmc_sfr[i].address ) );
				}
			}
		}
		else if(sd_mmc_sfr[i].size<=16) {
			for(j=0;j<sd_mmc_sfr[i].size;j++) {
				Outp16( sCh->m_uBaseAddr + sd_mmc_sfr[i].address, (1<<j) );
				if( Inp16( sCh->m_uBaseAddr + sd_mmc_sfr[i].address ) != (1<<j) ) {
					SDHC_PrintSFRErr( &sd_mmc_sfr[i], (1<<j), Inp16( sCh->m_uBaseAddr + sd_mmc_sfr[i].address ) );
				}
			}
		}
		else {
			for(j=0;j<sd_mmc_sfr[i].size;j++) {
				Outp32( sCh->m_uBaseAddr + sd_mmc_sfr[i].address, (1<<j) );
				if( Inp32( sCh->m_uBaseAddr + sd_mmc_sfr[i].address ) != (1<<j) ) {
					SDHC_PrintSFRErr( &sd_mmc_sfr[i], (1<<j), Inp32( sCh->m_uBaseAddr + sd_mmc_sfr[i].address ) );
				}
			}
		}
	}
}


///////////////////////Internal Test Only/////////////////////////////////////////////////////////

void SDMMC_Test(void)
{
	int i,sel;

	const testFuncMenu menu[]=
	{
		SDHC_ComplexRWTest,			"SDHC_ComplexRWTest",
		SDHC_SetBusWidth,				"Set uBusWidth",
		SDHC_SetStartBlock,				"Set uStBlock",
		SDHC_SetNumOfBlocks,			"Set uBlocks", 
		SDHC_SetClockSource,			"Set eClkSrc",
		SDHC_SetSdClkDivisor,			"Set uClkDiv",
		SDHC_SetOperationMode,			"Set Operation Mode",
		SDHC_SetChannel,				"Set Channel",
		SDHC_MoviBootloaderCopy,		"SDHC_MoviBootloaderCopy",
		SDHC_ComplexRWTest_FullMemory,	"SDHC_ComplexRWTest_FullMemory",
		SDHC_SFR_Test,					"SDHC_SFR_Test",
		0,0
	};
	
	// EPLL_Setting.
	SYSC_SetLockTime( eEPLL, 300);
	SYSC_CtrlCLKOUT(eCLKOUT_EPLLOUT, 0);
//	SYSC_SetPLL(eEPLL, 100, 3, 2, 0);		// EPLL => 100MHz
	SYSC_SetPLL(eEPLL, 66, 3, 2, 0);		// EPLL => 66MHz
	Outp32( 0x7E00F01C, Inp32(0x7E00F01C) | 7 );

	// Turn on USB Clock.
	*((volatile u32*)0x7E00F900) |= (1<<16);

	// 1. MMC EPLL - source Setting.
//	*((volatile u32*)0x7E00F01C) &= ~(0x3F<<18);
	//     Channel 0
//	*((volatile u32*)0x7E00F01C) |= (0x3<<18);
	//     Channel 1
//	*((volatile u32*)0x7E00F01C) |= (0x3<<20);
	//     Channel 2
//	*((volatile u32*)0x7E00F01C) |= (0x3<<22);

	// 2. MMC EPLL - ratio setting.
//	*((volatile u32*)0x7E00F024) &= ~(0xFFF<<0);
	//     Channel 0
//	*((volatile u32*)0x7E00F024) |= (0x2<<0);
	//     Channel 1
//	*((volatile u32*)0x7E00F024) |= (0x2<<4);
	//     Channel 2
//	*((volatile u32*)0x7E00F024) |= (0x2<<8);

	// 3. MMC HCLK-Gate
	//     Channel 0
//	*((volatile u32*)0x7E00F030) &= ~(0x1<<17);
	//     Channel 1
//	*((volatile u32*)0x7E00F030) &= ~(0x1<<18);
	//     Channel 2
//	*((volatile u32*)0x7E00F030) &= ~(0x1<<19);

	// 4. MMC SCLK Gate
	//     SCLK_MMC2_48
//	*((volatile u32*)0x7E00F038) &= ~(0x1<<29);	// USB Clock Gate
	//     SCLK_MMC1_48
//	*((volatile u32*)0x7E00F038) &= ~(0x1<<28);	// USB Clock Gate
	//     SCLK_MMC0_48
//	*((volatile u32*)0x7E00F038) &= ~(0x1<<27);	// USB Clock Gate
	//     SCLK_MMC2
//	*((volatile u32*)0x7E00F038) &= ~(0x1<<26);	// EPLL Source Gate
	//     SCLK_MMC1
//	*((volatile u32*)0x7E00F038) &= ~(0x1<<25);	// EPLL Source Gate
	//     SCLK_MMC0
//	*((volatile u32*)0x7E00F038) &= ~(0x1<<24);	// EPLL Source Gate

	// initial value.
	SDHC_descriptor.m_eChannel = SDHC_TEST_CHANNEL;
	SDHC_descriptor.m_eClockSource = SDHC_HCLK;
	SDHC_descriptor.m_eOpMode = SDHC_SDMA_MODE;
	SDHC_descriptor.m_uStartBlockPos =1000;// start Block address.
	SDHC_descriptor.m_ucBandwidth = 4;	// bandwidth.
	SDHC_descriptor.m_uClockDivision = 2;	// clock division
	SDHC_descriptor.m_uOneBlockSize = 2048;	// block size.

	while(1)
	{
		UART_Printf("\n ----------------------------------------------\n");
		for (i=0; (int)(menu[i].desc)!=0; i++) {
			UART_Printf("%2d: %s\n", i, menu[i].desc);
		}

		SDHC_DispCurrParams();

		UART_Printf("\nSelect the function to test : ");
		sel = UART_GetIntNum();
		UART_Printf("\n");
		if(sel==-1) {
	   		break;		// return.
		}
		if (sel>=0 && sel<(sizeof(menu)/8-1)) {
			(menu[sel].func)();
		}
	}
}


