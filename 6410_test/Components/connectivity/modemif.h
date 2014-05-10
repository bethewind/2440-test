
//===================================================================
// NAME		: modemif.h	
// DESC		: S3C6410 DPSRAM test 
// History	: 01.03.2007 ver 0.0 by JAE GUEN. SONG
//===================================================================

#ifndef __MODEMIF_H__
#define __MODEMIF_H__

#ifdef __cplusplus
extern "C" {
#endif

#define	MODEM_PRINTF(x...)	UART_Printf(x)
//#define	MODEM_PRINTF(x...)	
/////////////////////
////MODEM I/F SFR////
/////////////////////

#define DPSRAM_BASE_MODEM	(0x10000000)	// nCS0
#define DPSRAM_BASE_AP		(0x74100000)

#define AP_TX_Queue		(0x74101000)
#define AP_RX_Queue		(0x74100000)
#define MSM_TX_Queue	(0x10000000)
#define MSM_RX_Queue	(0x10001000)

#define AP_TX_BUFFER	(0x51000000)
#define AP_RX_BUFFER	(0x51100000)

#define MSM_TX_BUFFER	(0x51000000)
#define MSM_RX_BUFFER	(0x51100000)

#define SLEEP_WAKEUP			(0x10002200)
#define SLEEP_WAKEUP_CLEAR	(0x10002300)

#if 0
#define INT2AP_ADR		0x1ffe	
#define INT2MSM_ADR	0x1ffc	
#else
#define	INT2AP_ADR		0x0FFE
#define	INT2MSM_ADR	0x1FFC	
#endif

//#define DMA_COUNT	256	// MODEM DMA COUNT
#define DMA_COUNT	2	// MODEM DMA COUNT
#define BLK_COUNT	2	// AP 4KB BLOCK COUNT

//#define DPSRAM_TXRX_BUFLEN	0x1000
#define MODEM_TX_BUFLEN	(0x1000-0x10) // 4KB-16B (16Bytes means DMA Request Addresses and Interrupt Reqeust addresses)
//#define MODEM_TX_BUFLEN	0x100 // 4KB
#define AP_TX_BUFLEN		0x3fc

#if 1
#define	DMA_RX_ADDR0	0x0FF8
#define	DMA_RX_ADDR1	0x0FFA
#define	DMA_TX_ADDR0	0x1FF8
#define	DMA_TX_ADDR1	0x1FFA
#else
#define	DMA_TX_ADDR0	0x03FE
#define	DMA_TX_ADDR1	0x07FE
#define	DMA_RX_ADDR0	0x0BFE
#define	DMA_RX_ADDR1	0x0FFE
#endif

enum MODEMIF_SFR
{
	rINT2AP			= MODEMIF_BASE + 0x00,
	rINT2MSM		= MODEMIF_BASE + 0x04,
	rMIFCON		= MODEMIF_BASE + 0x08,
	rMIFPCON		= MODEMIF_BASE + 0x0C,
	rMSMINTCLR		= MODEMIF_BASE + 0x10,
	rDMA_TX_ADR	= MODEMIF_BASE + 0x14,
	rDMA_RX_ADR	= MODEMIF_BASE + 0x18
};

typedef enum 
{
	NOMAL, BYPASS
}LCD_PASS_MODE;

typedef enum
{
	MODEMIF_AP_TX=	0,
	MODEMIF_AP_RX,
	MODEMIF_MODEM_TX,
	MODEMIF_MODEM_RX
}
MODEMIF_XFER_MODE;

typedef enum
{
	MODEMIF_TX_0=0,
	MODEMIF_TX_1,
	MODEMIF_RX_0,
	MODEMIF_RX_1,	
}
MODEMIF_DMA_REQ_NUM;
// MODEM I/F ISR
void __irq Isr_AP2MODEM(void);
void __irq Isr_MODEM2AP(void);

// MODEM I/F Initialize function
void MODEMIF_AP_Init(u8 INT2MSMEN, u8 INT2APEN);
void MODEMIF_MODEM_Init(u8 INT2MSMEN, u8 INT2APEN);

// Interrupt address set function
void MODEMIF_Int2AP(u32 int_add);
void MODEMIF_Int2Modem(u32 int_add);

// Interrupt Clear function
void MODEMIF_IntClear_Int2AP(void);
u16 MODEMIF_IntClear_Int2MSM(void);

// MODEM I/F GPIO port setting function
void MODEMIF_SetPort(void);
void MODEMIF_Set_Modem_Booting_Port(void);

// AP or MODEM booting Reset function
void MODEMIF_MODEM_Reset(void);
void MODEMIF_AP_Reset(void);

// SET MODEM I/F DMA Request Address
void MODEMIF_SET_DMA_REQ_ADDR(MODEMIF_XFER_MODE , MODEMIF_DMA_REQ_NUM , u16 );

// MODEM DMA APIs
void MODEMIF_SET_APDMA(MODEMIF_XFER_MODE eRxTx, MODEMIF_DMA_REQ_NUM eSrcNum, u8 ucEn);
void MODEMIF_SET_DMA_REQ_ADDR(MODEMIF_XFER_MODE eRxTx, MODEMIF_DMA_REQ_NUM eSrcNum, u16 usAddr);

#ifdef __cplusplus
}
#endif

#endif    
//__MODEMIF_H__


