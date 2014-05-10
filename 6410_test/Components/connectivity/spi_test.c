#include <stdio.h>
#include "string.h"
#include "def.h"
#include "option.h"
#include "library.h"
#include "system.h"
#include "spi.h"
#include "sysc.h"
#include "timer.h"

// Test Vector.
#define SPI_BUFFER_SIZE 0x100000			// Reserved Buffer Size.
#define SPI_SPEED 20000000	// 50Mhz.			// Transfer Speed, that is limit to clock source.
#define SPI_DATA_COUNT (0x4)				// max Buffer size.
#define CLK_SOURCE SPI_PCLK					// SPI Clock Source.
#define TEST_CHANNEL0 0						// SPI Test Peripheral 0
#define TEST_CHANNEL1 1						// SPI Test Peripheral 1
#define SPI_TEST_CPOL	SPI_ACTIVE_LOW		// SPI Test polarity
#define SPI_TEST_CHANNEL_WIDTH SPI_BYTE	// SPI Channel Size
#define SPI_TEST_BUS_WIDTH SPI_WORD		// SPI Bus Size
#define SPI_TEST_DMA_TYPE SPI_DMA_SINGLE	// SPI DMA Mode
#define SPI_TEST_FORMAT SPI_FORMAT_A		// SPI Test Phase
// When polling mode test, must using board to board test.
#define SPI_TEST_TRANSFER_MODE SPI_DMA_MODE	// SPI transfer mode selection.
#define SPI_MIN_ERROR_CNT	0
//////////
// File Name : SPI_getBuffer
// File Description : make buffer on non-cache area.
// Input : index 
// Output : NONE.
// Version : 
u8 * SPI_getBuffer(int index, u8 sequence, u8 clear) {
	u8* result, *p;
	int buffer_size = SPI_BUFFER_SIZE;
	int i;
	u8 reqNum;
	
	result = (u8*)(CODEC_MEM_ST + index * buffer_size);
	
	if ( clear == TRUE ) {
		memset( result, 0, buffer_size );
	}
	p=result;
	if ( sequence == TRUE ) {
		reqNum = (u8)(rand() & 0xFF);	// auto offset.//youngbo.song
		for(i=buffer_size;i>0;i--) {
			*p++=reqNum++;
		}
	}
	return result;
}


//////////
// File Name : SPI_CompareData
// File Description : data compare function.
// Input : source data1, source data2, data length
// Output : NONE.
// Version : 
int SPI_CompareData(u8* data1, u8* data2, u32 bytes) {
	u32 i;
	int errorCnt=0;
	// find gargage offset.
	for(i=0;i<SPI_MIN_ERROR_CNT;i++) {
		if( *data1 == *(data2+i) ) {
			UART_Printf( "Garbage Offset : [%d]\n" , i );
			data2=data2+i;
			break;
		}
	}
	for (i=0; i<bytes; i++)
	{
		if(*data1 != *data2 )
		{
			if (errorCnt==0)
			{
				UART_Printf("%08x=%02x <-> %08x=%02x\n", data1, *data1, data2, *data2);
			}
			errorCnt++;
		}
		data1++;
		data2++;
	}
	if(errorCnt==0) {
		UART_Printf("Data Compare Ok\n");
	}
	else {
		UART_Printf( "Error Count: %d\n", errorCnt );
	}
	return errorCnt;
}

//////////
// File Name : callback
// File Description : call back function for non-blocking implement.
// Input : SPI_channel
// Output : NONE.
// Version : 
void callback(SPI_channel * ch) {
	UART_Printf( "Callback Exe : tx:%d, rx:%d\n", ch->m_uTxRemainDataSize, ch->m_uRxRemainDataSize );
}

// Test Case 0 - DMA, Interrrupt Only.
u8 SPI_MasterFullDuplex(void) {
	SPI_channel *ch1, *ch0;
	int DataCnt;
	u32 time = 0;
	u8* uMTxBufAddr = (u8*)SPI_getBuffer(0, TRUE, TRUE);
	u8* uMRxBufAddr = (u8*)SPI_getBuffer(1, FALSE, TRUE);
	u8* uSTxBufAddr = (u8*)SPI_getBuffer(2, TRUE, TRUE);
	u8* uSRxBufAddr = (u8*)SPI_getBuffer(3, FALSE, TRUE);

	DataCnt = SPI_DATA_COUNT;

// =========== SPI1 Slave Rx/Tx Setting =========== 
	ch1 = SPI_open( TEST_CHANNEL1, SPI_SLAVE, SPI_TEST_CPOL, SPI_TEST_FORMAT, CLK_SOURCE, SPI_SPEED, SPI_TEST_CHANNEL_WIDTH, SPI_TEST_BUS_WIDTH, SPI_TEST_DMA_TYPE );
	SPI_transaction( ch1, SPI_DMA_MODE, uSTxBufAddr, uSRxBufAddr, DataCnt, callback);

// =========== SPI0 Master Rx/Tx Setting =========== 
	ch0 = SPI_open( TEST_CHANNEL0, SPI_MASTER, SPI_TEST_CPOL, SPI_TEST_FORMAT, CLK_SOURCE, SPI_SPEED, SPI_TEST_CHANNEL_WIDTH, SPI_TEST_BUS_WIDTH, SPI_TEST_DMA_TYPE );
	StartTimer(0);
	SPI_transaction( ch0, SPI_TEST_TRANSFER_MODE, uMTxBufAddr, uMRxBufAddr, DataCnt, NULL);
	time = StopTimer(0);
	UART_Printf( "BPS = %f, time:%d\n", ((float)DataCnt)/((float)time), time );
	while ( ch1->m_uTxRemainDataSize != 0 );
	while ( ch1->m_uRxRemainDataSize != 0 );
	SPI_close( ch0 );
	SPI_close( ch1 );

//	UART_Printf( "master to slave\n");
	if ( SPI_CompareData( uMTxBufAddr, uSRxBufAddr, DataCnt ) > SPI_MIN_ERROR_CNT ) {
		UART_Printf ( "ERROR 1\n");
		return FALSE;
	}
//	UART_Printf( "slave to master\n");
	if ( SPI_CompareData( uSTxBufAddr, uMRxBufAddr, DataCnt ) > SPI_MIN_ERROR_CNT ) {
		UART_Printf ( "ERROR 2\n");
		return FALSE;
	}
	return TRUE;
}

// Test Case 1 - DMA, Interrupt Only
u8 SPI_SlaveFullDuplex(void) {
	SPI_channel *ch1, *ch0;
	int DataCnt;
	u32 time = 0;
	u8* uMTxBufAddr = (u8*)SPI_getBuffer(0, TRUE, TRUE);
	u8* uMRxBufAddr = (u8*)SPI_getBuffer(1, FALSE, TRUE);
	u8* uSTxBufAddr = (u8*)SPI_getBuffer(2, TRUE, TRUE);
	u8* uSRxBufAddr = (u8*)SPI_getBuffer(3, FALSE, TRUE);

	DataCnt = SPI_DATA_COUNT;

// =========== SPI1 Slave Rx/Tx Setting =========== 
	ch0 = SPI_open( TEST_CHANNEL0, SPI_SLAVE, SPI_TEST_CPOL, SPI_TEST_FORMAT, CLK_SOURCE, SPI_SPEED, SPI_TEST_CHANNEL_WIDTH, SPI_TEST_BUS_WIDTH, SPI_TEST_DMA_TYPE );
	SPI_transaction( ch0, SPI_TEST_TRANSFER_MODE, uSTxBufAddr, uSRxBufAddr, DataCnt, callback);

// =========== SPI0 Master Rx/Tx Setting =========== 
	ch1 = SPI_open( TEST_CHANNEL1, SPI_MASTER, SPI_TEST_CPOL, SPI_TEST_FORMAT, CLK_SOURCE, SPI_SPEED, SPI_TEST_CHANNEL_WIDTH, SPI_TEST_BUS_WIDTH, SPI_TEST_DMA_TYPE );
	StartTimer(0);
	SPI_transaction( ch1, SPI_DMA_MODE, uMTxBufAddr, uMRxBufAddr, DataCnt, NULL);
	time = StopTimer(0);
	UART_Printf( "BPS = %f, time:%d\n", ((float)DataCnt)/((float)time), time );
	while ( ch0->m_uTxRemainDataSize != 0 );
	while ( ch0->m_uRxRemainDataSize != 0 );
	SPI_close( ch0 );
	SPI_close( ch1 );

//	UART_Printf( "master to slave\n");
	if ( SPI_CompareData( uMTxBufAddr, uSRxBufAddr, DataCnt ) > SPI_MIN_ERROR_CNT ) {
		UART_Printf ( "ERROR1\n");
		return FALSE;
	}
//	UART_Printf( "slave to master\n");
	if ( SPI_CompareData( uSTxBufAddr, uMRxBufAddr, DataCnt ) > SPI_MIN_ERROR_CNT ) {
		UART_Printf( "ERROR2\n");
		return FALSE;
	}
	return TRUE;
	
}

// Test Case 2 - DMA, Interrupt, Polling - Support
u8 SPI_MasterTxOnly(void) {
	SPI_channel *ch0, *ch1;
	int DataCnt;
	u32 time=0;
	u8* uTxBufAddr = (u8*)SPI_getBuffer(0, TRUE, TRUE);
	u8* uRxBufAddr = (u8*)SPI_getBuffer(2, FALSE, TRUE);

	DataCnt = SPI_DATA_COUNT;
// =========== SPI1 Slave Rx Setting =========== 
	ch1 = SPI_open( TEST_CHANNEL1, SPI_SLAVE, SPI_TEST_CPOL, SPI_TEST_FORMAT, CLK_SOURCE, SPI_SPEED, SPI_TEST_CHANNEL_WIDTH, SPI_TEST_BUS_WIDTH, SPI_TEST_DMA_TYPE );
	SPI_transaction( ch1, SPI_DMA_MODE, NULL, uRxBufAddr, DataCnt, callback);
	
// =========== SPI0 Master Tx Setting =========== 
	ch0 = SPI_open( TEST_CHANNEL0, SPI_MASTER, SPI_TEST_CPOL, SPI_TEST_FORMAT, CLK_SOURCE, SPI_SPEED, SPI_TEST_CHANNEL_WIDTH, SPI_TEST_BUS_WIDTH, SPI_TEST_DMA_TYPE );
	StartTimer(0);
	SPI_transaction( ch0, SPI_TEST_TRANSFER_MODE, uTxBufAddr, NULL, DataCnt, NULL);
	time = StopTimer(0);
	UART_Printf( "BPS = %f, time:%d\n", ((float)DataCnt)/((float)time), time );
	
	while ( ch1->m_uRxRemainDataSize != 0 );
	SPI_close( ch0 );
	SPI_close( ch1 );

	if ( SPI_CompareData( uTxBufAddr, uRxBufAddr, DataCnt ) > 0 ) {
		return FALSE;
	}
	return TRUE;
}

// Test Case 3 - DMA, Interrupt Only.
u8 SPI_SlaveRxOnly(void) {
	SPI_channel * ch0, * ch1;
	int DataCnt;
	u32 time = 0;
	u8* uTxBufAddr = (u8*)SPI_getBuffer(0, TRUE, TRUE);
	u8* uRxBufAddr = (u8*)SPI_getBuffer(2, FALSE, TRUE);
	
	DataCnt = SPI_DATA_COUNT;
 
// =========== SPI0 Slave Rx Setting =========== 
	ch0 = SPI_open( TEST_CHANNEL0, SPI_SLAVE, SPI_TEST_CPOL, SPI_TEST_FORMAT, CLK_SOURCE, SPI_SPEED, SPI_TEST_CHANNEL_WIDTH, SPI_TEST_BUS_WIDTH, SPI_TEST_DMA_TYPE );
	SPI_transaction( ch0, SPI_TEST_TRANSFER_MODE, NULL, uRxBufAddr, DataCnt, callback);
// =========== SPI1 Master Tx Setting =========== 
	ch1 = SPI_open( TEST_CHANNEL1, SPI_MASTER, SPI_TEST_CPOL, SPI_TEST_FORMAT, CLK_SOURCE, SPI_SPEED, SPI_TEST_CHANNEL_WIDTH, SPI_TEST_BUS_WIDTH, SPI_TEST_DMA_TYPE );
	StartTimer(0);
	SPI_transaction( ch1, SPI_DMA_MODE, uTxBufAddr, NULL, DataCnt, NULL);
	time = StopTimer(0);
	UART_Printf( "BPS = %f, time:%d\n", ((float)DataCnt)/((float)time), time );
	while(ch0->m_uRxRemainDataSize!= 0);
	
	SPI_close( ch0 );
	SPI_close( ch1 );

	if ( SPI_CompareData( uTxBufAddr, uRxBufAddr, DataCnt ) > 0 ) {
		return FALSE;
	}
	return TRUE;
}

// Test Case 4 - DMA, Interrupt, Polling - Support
u8 SPI_MasterRxOnly(void) {
	SPI_channel * ch0, *ch1; 
	int DataCnt;
	u8* uTxBufAddr = (u8*)SPI_getBuffer(0, TRUE, TRUE);
	u8* uRxBufAddr = (u8*)SPI_getBuffer(2, FALSE, TRUE);
	
	DataCnt = SPI_DATA_COUNT;
	
// =========== SPI1 Slave Tx Setting =========== 
	ch1 = SPI_open( TEST_CHANNEL1, SPI_SLAVE, SPI_TEST_CPOL, SPI_TEST_FORMAT, CLK_SOURCE, SPI_SPEED, SPI_TEST_CHANNEL_WIDTH, SPI_TEST_BUS_WIDTH, SPI_TEST_DMA_TYPE );
	SPI_transaction( ch1, SPI_DMA_MODE, uTxBufAddr, NULL, DataCnt, callback);
	
// =========== SPI0 Master Rx Setting =========== 
	ch0 = SPI_open( TEST_CHANNEL0, SPI_MASTER, SPI_TEST_CPOL, SPI_TEST_FORMAT, CLK_SOURCE, SPI_SPEED, SPI_TEST_CHANNEL_WIDTH, SPI_TEST_BUS_WIDTH, SPI_TEST_DMA_TYPE );
	SPI_transaction( ch0, SPI_TEST_TRANSFER_MODE, NULL, uRxBufAddr, DataCnt, NULL);

	while( ch1->m_uTxRemainDataSize != 0 );
	SPI_close( ch0 );
	SPI_close( ch1 );
	if ( SPI_CompareData( uTxBufAddr, uRxBufAddr, DataCnt ) > 0 ) {
		return FALSE;
	}
	return TRUE;
}

// Test Case 5 - DMA, Interrupt Only.
u8 SPI_SlaveTxOnly(void) {
	SPI_channel * ch0, *ch1;
	int DataCnt;
	u8* uTxBufAddr = (u8*)SPI_getBuffer(0, TRUE, TRUE);
	u8* uRxBufAddr = (u8*)SPI_getBuffer(2, FALSE, TRUE);
	
	DataCnt = SPI_DATA_COUNT;

//=========== SPI0 Slave Tx Setting ===========
	ch0 = SPI_open( TEST_CHANNEL0, SPI_SLAVE, SPI_TEST_CPOL, SPI_TEST_FORMAT, CLK_SOURCE, SPI_SPEED, SPI_TEST_CHANNEL_WIDTH, SPI_TEST_BUS_WIDTH, SPI_TEST_DMA_TYPE );
	SPI_transaction( ch0, SPI_TEST_TRANSFER_MODE, uTxBufAddr, NULL, DataCnt, callback);

// =========== SPI1 Master Rx Setting =========== 
	ch1 = SPI_open( TEST_CHANNEL1, SPI_MASTER, SPI_TEST_CPOL, SPI_TEST_FORMAT, CLK_SOURCE, SPI_SPEED, SPI_TEST_CHANNEL_WIDTH, SPI_TEST_BUS_WIDTH, SPI_TEST_DMA_TYPE );
	SPI_transaction( ch1, SPI_DMA_MODE, NULL, uRxBufAddr, DataCnt, NULL);

	while(ch0->m_uTxRemainDataSize!=0);
	
	SPI_close( ch0 );
	SPI_close( ch1 );
	if ( SPI_CompareData( uTxBufAddr, uRxBufAddr, DataCnt ) > 0 ) {
		return FALSE;
	}
	return TRUE;
}


u32 MTx[] = { 0x55550000, 0x55550000, 0x55550000, 0x55550000 };
u32 STx[] = { 0xaaaa0000, 0xaaaa0000, 0xaaaa0000, 0xaaaa0000 };
u32 MRx[] = { 0, 0, 0, 0 };
u32 SRx[] = { 0, 0, 0, 0 };

// Test Case 6
u8 SPI_SwapMasterTxOnly(void) {
	SPI_channel *ch1, *ch0;
	int DataCnt;

	DataCnt = sizeof(MTx);

// =========== SPI1 Slave Rx/Tx Setting =========== 
	ch1 = SPI_open( TEST_CHANNEL1, SPI_SLAVE, SPI_TEST_CPOL, SPI_TEST_FORMAT, CLK_SOURCE, SPI_SPEED, SPI_BYTE, SPI_BYTE, SPI_DMA_SINGLE );
	SPI_setSwapMode( ch1, SPI_BIT_SWAP, SPI_NO_SWAP);
	SPI_transaction( ch1, SPI_DMA_MODE, (u8*)STx, (u8*)SRx, DataCnt, callback);

// =========== SPI0 Master Rx/Tx Setting =========== 
	ch0 = SPI_open( TEST_CHANNEL0, SPI_MASTER, SPI_TEST_CPOL, SPI_TEST_FORMAT, CLK_SOURCE, SPI_SPEED, SPI_BYTE, SPI_BYTE, SPI_DMA_SINGLE );
	ch0->m_bIsAutoChipSelection = TRUE;
	SPI_setSwapMode( ch0, SPI_BIT_SWAP, SPI_NO_SWAP);
	SPI_transaction( ch0, SPI_TEST_TRANSFER_MODE, (u8*)MTx, (u8*)MRx, DataCnt, NULL);
	while ( ch1->m_uTxRemainDataSize != 0 );
	while ( ch1->m_uRxRemainDataSize != 0 );
	SPI_close( ch0 );
	SPI_close( ch1 );

	UART_Printf( "master to slave\n");
	if ( SPI_CompareData( (u8*)MTx, (u8*)SRx, DataCnt ) > 16 ) {
		return FALSE;
	}
	UART_Printf( "slave to master\n");
	if ( SPI_CompareData( (u8*)STx, (u8*)MRx, DataCnt ) > 16 ) {
		return FALSE;
	}
	return TRUE;
}


//////////
// File Name : SPI_BtoBMasterFullDuplex
// File Description : Board to Board Test using Master fullduplex mode.
// Input : NONE
// Output : success or fail.
// Version : 
u8 SPI_BtoBMasterFullDuplex(void) {
	SPI_channel *ch;
	int DataCnt;
	u8* uMTxBufAddr = (u8*)SPI_getBuffer(0, TRUE, TRUE);
	u8* uMRxBufAddr = (u8*)SPI_getBuffer(1, FALSE, TRUE);

	DataCnt = SPI_DATA_COUNT;
	UART_Printf( "SPI_BtoBMasterFullDuplex\n");
// =========== SPI0 Slave Rx/Tx Setting =========== 
	ch = SPI_open( TEST_CHANNEL0, SPI_MASTER, SPI_TEST_CPOL, SPI_TEST_FORMAT, CLK_SOURCE, SPI_SPEED, SPI_TEST_CHANNEL_WIDTH, SPI_TEST_BUS_WIDTH, SPI_TEST_DMA_TYPE );
	SPI_transaction( ch, SPI_DMA_MODE, NULL, uMRxBufAddr, DataCnt, NULL);
	SPI_close( ch );

	if ( SPI_CompareData( uMTxBufAddr, uMRxBufAddr, DataCnt ) > 0 ) {
		return FALSE;
	}
	return TRUE;
}

//////////
// File Name : SPI_BtoBSlaveFullDuplex
// File Description : Board to Board Test using Slave fullduplex mode.
// Input : NONE
// Output : success or fail.
// Version : 
u8 SPI_BtoBSlaveFullDuplex(void) {
	SPI_channel *ch;
	int DataCnt;
	u8* uSTxBufAddr = (u8*)SPI_getBuffer(0, TRUE, TRUE);
	u8* uSRxBufAddr = (u8*)SPI_getBuffer(1, FALSE, TRUE);

	DataCnt = SPI_DATA_COUNT;

// =========== SPI0 Slave Rx/Tx Setting =========== 
	UART_Printf( "SPI_BtoBSlaveFullDuplex\n");
	ch = SPI_open( TEST_CHANNEL0, SPI_SLAVE, SPI_TEST_CPOL, SPI_TEST_FORMAT, CLK_SOURCE, SPI_SPEED, SPI_TEST_CHANNEL_WIDTH, SPI_TEST_BUS_WIDTH, SPI_TEST_DMA_TYPE );
	SPI_transaction( ch, SPI_DMA_MODE, uSTxBufAddr, NULL, DataCnt, NULL);
	SPI_close( ch );

	if ( SPI_CompareData( uSTxBufAddr, uSRxBufAddr, DataCnt ) > 0 ) {
		return FALSE;
	}
	return TRUE;
}

//////////
// File Name : SPI_AgingTest
// File Description : Loopback Aging test.
// Input : NONE
// Output : NONE.
// Version : 
void SPI_AgingTest(void) {
	int i;
	int errorCnt=0;
	for (i=0;i<0x200000;i++) {
		if ( SPI_MasterFullDuplex() == FALSE ) {
			errorCnt++;
		}
		if ( SPI_SlaveFullDuplex() == FALSE ) {
			errorCnt++;
		}
		if ( errorCnt > 0 ) {
			UART_Printf("------------------------ErrorCnt : [%d]------------------- \n ", errorCnt );
		}
	}
}

//////////
// File Name : SPI_AgingPollingTest
// File Description : Loopback Aging Test using Polling mode.
// Input : NONE
// Output : NONE.
// Version : 
void SPI_AgingPollingTest(void) {
	int i;
	int errorCnt=0;
	for (i=0;i<0x200000;i++) {
		if ( SPI_MasterFullDuplex() == FALSE ) {
			errorCnt++;
		}
		if ( SPI_SlaveFullDuplex() == FALSE ) {
			errorCnt++;
		}
		if ( errorCnt > 0 ) {
			UART_Printf("------------------------ErrorCnt : [%d]------------------- \n ", errorCnt );
		}
	}
}

testFuncMenu func_HSPI_test[]=
{
	(void(*)(void))SPI_MasterFullDuplex, 		"B2B Master Full Duplex   ",	
	(void(*)(void))SPI_SlaveFullDuplex,			"B2B Slave Full Duplex    ",
	(void(*)(void))SPI_MasterTxOnly,			"B2B Master Tx only       ",
	(void(*)(void))SPI_SlaveRxOnly,   			"B2B Slave Rx only         ",
	(void(*)(void))SPI_MasterRxOnly,			"B2B Master Rx only       ",
	(void(*)(void))SPI_SlaveTxOnly,   			"B2B Slave Tx only         ",	
	(void(*)(void))SPI_SwapMasterTxOnly,		"SPI_SwapMasterTxOnly ",
	(void(*)(void))SPI_BtoBMasterFullDuplex,	"SPI_BtoBMasterFullDuplex",
	(void(*)(void))SPI_BtoBSlaveFullDuplex,	"SPI_BtoBSlaveFullDuplex",
	(void(*)(void))SPI_AgingTest,				"SPI_AgingTest           ",
	(void(*)(void))SPI_AgingPollingTest,		"SPI_AgingPollingTest   ",
//	SPI_ErrorInterrutStaus,		"SPI_ErrorInterrutStaus   ",
	NULL,NULL
};

//////////
// File Name : SPI_Test
// File Description : High-speed SPI Test function.
// Input : NONE
// Output : NONE.
// Version : 
void SPI_Test(void)
{  
	int i;

	// 1. SPI EPLL Source-Setting.
//	*((volatile u32*)0x7E00F01C) &= ~(0xF<<14);	// Clear
	//     Channel 1
//	*((volatile u32*)0x7E00F01C) |= (0x0<<16);
	//     Channel 0
//	*((volatile u32*)0x7E00F01C) |= (0x0<<14);

	// 2. SPI EPLL Ratio-Setting.
//	*((volatile u32*)0x7E00F028) &= ~(0xFF<<0);	// Clear
	// Channel 1
//	*((volatile u32*)0x7E00F028) |= (0x0<<4);
	// Channel 0
//	*((volatile u32*)0x7E00F028) |= (0x0<<0);

	// 3. SPI PCLK-Gate
	//     Channel 0
//	*((volatile u32*)0x7E00F034) &= ~(0x1<<21);
	//     Channel 1
//	*((volatile u32*)0x7E00F034) &= ~(0x1<<22);

	// 4. SPI SCLK- Gate
	//     SCLK_SPI1_48
//	*((volatile u32*)0x7E00F038) &= ~(0x1<<23);
	//     SCLK_SPI0_48
//	*((volatile u32*)0x7E00F038) &= ~(0x1<<22);
	//     SCLK_SPI1
//	*((volatile u32*)0x7E00F038) &= ~(0x1<<21);
	//     SCLK_SPI0
//	*((volatile u32*)0x7E00F038) &= ~(0x1<<20);
	
	while(1)
	{
		i=0;
		UART_Printf("\n\n");
		while(1)
		{   //display menu
	   		UART_Printf("%2d:%s", i, func_HSPI_test[i].desc);
	    	i++;
	    	if((func_HSPI_test[i].func)==NULL)
	    	{
				UART_Printf("\n");
				break;
	    	}
	    	if((i%2)==0)
	    	UART_Printf("\n");
		}

		UART_Printf("\nSelect (\"-1\" to exit) : ");
		i = UART_GetIntNum();
		if(i==-1) 
	   		break;		// return.
		if( (i<((sizeof(func_HSPI_test)-1)/8)) )	// select and execute...
	    	( (void (*)(void)) (func_HSPI_test[i].func) )();
	}
	
}
