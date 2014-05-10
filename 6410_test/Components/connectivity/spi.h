#ifndef __SPI_H__
#define __SPI_h__

#include "dma.h"


#define SPI_TX_FIFO_SIZE	64
#define SPI_RX_FIFO_SIZE	64

#define SPI_NORMAL_DMA	// normal DMA or SDMA Selection.

#define SPI_INT_TRAILING		(1<<6)
#define SPI_INT_RX_OVERRUN		(1<<5)
#define SPI_INT_RX_UNDERRUN	(1<<4)
#define SPI_INT_TX_OVERRUN		(1<<3)
#define SPI_INT_TX_UNDERRUN	(1<<2)
#define SPI_INT_RX_FIFORDY		(1<<1)
#define SPI_INT_TX_FIFORDY		(1<<0)

#define SPI_INT_TX_UNDERRUN_CLR	(1<<4)
#define SPI_INT_TX_OVERRUN_CLR		(1<<3)
#define SPI_INT_RX_UNDERRUN_CLR	(1<<2)
#define SPI_INT_RX_OVERRUN_CLR		(1<<1)
#define SPI_INT_TRAILING_CLR		(1<<0)

// SPI Transfer mode
typedef enum _SPI_transfer_mode {
	SPI_POLLING_MODE=0,
	SPI_INTERRUPT_MODE,
	SPI_DMA_MODE,
} SPI_transfer_mode;

// SPI Master/Slave Selection
typedef enum _SPI_clock_mode {
	SPI_MASTER=0,
	SPI_SLAVE,
} SPI_clock_mode;

// SPI SFR Control Structure
typedef struct _SPI_SFR {
	u32 ch_cfg;			// 0x00 spi configuration register
	u32 clk_cfg;			// 0x04 clock configuration register
	u32 mode_cfg;		// 0x08 spi fifo control register
	u32 slave_sel;		// 0x0C slave selection signal
	u32 int_enable;		// 0x10 spi interrupt enable register
	u32 spi_status;		// 0x14 spi status register
	u32 spi_tx_data;		// 0x18 spi tx data register
	u32 spi_rx_data;		// 0x1C spi rx data register
	u32 packet_count;	// 0x20 count how many data master gets.
	u32 pending_clr;		// 0x24 pending clear register
	u32 swap_config;		// 0x28 swap config register
	u32 feedback_clk;	// 0x2C feedback clock config register
} SPI_SFR;

// SPI Clock Polarity.
typedef enum _SPI_CPOL {
	SPI_ACTIVE_HIGH=0,
	SPI_ACTIVE_LOW=1,
} SPI_CPOL;

// SPI Clock Phase
typedef enum _SPI_CPHA {
	SPI_FORMAT_A=0,
	SPI_FORMAT_B=1,
} SPI_CPHA;

// SPI Clock Source Selection
typedef enum _SPI_clock_selection {
	SPI_PCLK=0,
	SPI_USB_CLK=1,
	SPI_EPLL_CLK=2,
} SPI_clock_selection;

// SPI Transfer Data Type.
typedef enum _SPI_transfer_data_type {
	SPI_BYTE=0,
	SPI_HWORD=1,
	SPI_WORD=2
} SPI_transfer_data_type;

// SPI Swap mode Selection Structure.
typedef enum _SPI_swapMode {
	SPI_NO_SWAP=0x0,
	SPI_EN_SWAP=0x1,
	SPI_BIT_SWAP=0x3,		// Enable and bit swap.
	SPI_BYTE_SWAP=0x5,	// Enable and byte swap.
	SPI_HWORD_SWAP=0x9	// Enable and hword swap.
} SPI_swapMode;

// SPI Feedback Clock Delay Value.
typedef enum _SPI_feedbackClock {
	SPI_0NS_DELAY=0x0,	
	SPI_3NS_DELAY=0x1,	
	SPI_6NS_DELAY=0x2,
	SPI_9NS_DELAY=0x3,
} SPI_feedbackClock;

// SPI DMA mode Structure.
typedef enum _SPI_DMA_type {
	SPI_DMA_SINGLE=0,
	SPI_DMA_4_BURST,
} SPI_DMA_type;

// SPI Channel Descriptor, that is embedded in kernal.
typedef struct _SPI_Channel {
	SPI_SFR * m_cBase;					// SFR Base Register
	SPI_transfer_mode m_eTransferMode;	// Polling/Interrupt/DMA
	SPI_clock_mode m_eClockMode;		// Master / Slave
	SPI_transfer_data_type m_eChSize;		// Channel Bandwidth
	SPI_transfer_data_type m_eBusSize;		// Bus Bandwidth
	SPI_DMA_type m_eDMAType;			// Single/Burst DMA
	u8 * m_uRxBuf;						// RX buffer pointer
	u8 * m_uTxBuf;						// TX buffer pointer
	u32 m_uRxRemainDataSize;			// Rx remain data size.
	u32 m_uTxRemainDataSize;			// Tx remain data size.
	u16 m_uTraillingCnt;					// trailling count
	u8 m_ucChannelNum;					// channel number
	u8 m_ucTxLevel;						// tx trigger level
	u8 m_ucRxLevel;						// rx trigger level
	u8 m_cPrescaler;					// Prescaler Value;
	u8 m_cChannelNum;					// Channel Number;
	u8 m_bIsAutoChipSelection;			// Auto Chip selection Test.
	SPI_CPOL m_eCPOL;					// CPOL
	SPI_CPHA m_eCPHA;					// CPHA
	SPI_clock_selection m_eClockSource;	// Clock source
	SPI_swapMode m_eRxSwap;			// rx swap config
	SPI_swapMode m_eTxSwap;			// tx swap config
	SPI_feedbackClock m_eFeedbackClock;	// spi feedback clock
	u8 m_ucIntNum;//		= NUM_DMA1;	// store interrupt Number
	u8 m_ucDMACon;// 		= DMA1;		// store Dma number.
	DMAC m_sDMA;						// DMA Descriptor
	void (*m_fISR)(void) __irq;				// interrupt handler.
	void (*m_fDMA)(void) __irq;			// dma interrupt handler.
	void (*m_pCallback)(struct _SPI_Channel *);		// callback function pointer - for generate transfer completed signal
} SPI_channel;


void SPI_pollingProcess( SPI_channel* ch );
void __irq SPI_interruptChannel0( void );
void __irq SPI_interruptChannel1( void );
void __irq SPI_DMADoneChannel0(void );
void __irq SPI_DMADoneChannel1(void );
void SPI_cpuOneTxTransfer( SPI_channel *ch );
void SPI_cpuOneRxTransfer( SPI_channel *ch );
void SPI_transactionDone( SPI_channel * ch );
SPI_channel * SPI_open( u8 channel, SPI_clock_mode master, SPI_CPOL cpol, SPI_CPHA cpha, SPI_clock_selection clock,
						u32 clk_freq, SPI_transfer_data_type ch_size, SPI_transfer_data_type bus_size, SPI_DMA_type dma_type );

int SPI_transaction( SPI_channel * ch, SPI_transfer_mode mode, u8* tx_data, u8* rx_data, u32 size, void (*callbackFn)(SPI_channel *ch) );
void SPI_close( SPI_channel * ch );
void SPI_printStatus( SPI_channel * ch );
void SPI_setBasicRegister( SPI_channel* ch );
void SPI_setFeedbackClock( SPI_channel* ch, SPI_feedbackClock clock );
void SPI_setSwapMode( SPI_channel * ch, SPI_swapMode rx_swap, SPI_swapMode tx_swap );
void SPI_setTxRxTriggerLevel( SPI_channel * ch, u8 rxLevel, u8 txLevel );
void SPI_ErrorCheck(SPI_channel* ch);

// normal SPI setting.
void SPI_Port_Init(void);
void SPI_GPIOPortSet(u8 channel);
void SPI_SetDriveStrength( u8 cStrength );

// Test Only - Between Peripherals DMA Test.
int SPI_PtoPTransaction( SPI_channel * ch, SPI_transfer_mode mode, u32 size, void (*callbackFn)(SPI_channel *ch) );


// LCD Setting Test Jungil 
void SPIInit(void);
void SPIStart(void);
static void SPI_CSControl(int cs);
void SPIWrite(u8 data1,u8 data2,u8 data3);
void SPIWrite_2(u8 data1,u8 data2);


#endif
