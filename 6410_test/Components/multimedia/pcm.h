/********************************************************************************
* 
*	Project Name : S3C6400 Validation
*
*	Copyright 2006 by Samsung Electronics, Inc.
*	All rights reserved.
*
*	Project Description :
*		This software is only for verifying functions of the S3C6400.
*		Anybody can use this software without our permission.
*  
*--------------------------------------------------------------------------------
* 
*	File Name : pcm.h
*	File Description :
*
*	Author	: Sung-Hyun, Na
*	Dept. : AP Development Team
*
*	Version : 0.2
* 
*	History
*	- Version 0.1 (2007/03/16)
*	  -> Available with AK2440 PCM Codec.
*   - Version 0.2 (2007/04/19)
*     -> Also, working with WM8753 PCM Codec.
*
********************************************************************************/

#ifndef __PCM_H__
#define __PCM_H__
 
  
#ifdef __cplusplus
extern "C" {
#endif

#include "def.h"
#include "option.h"
#include "library.h"
#include "sfr6410.h"
#include "system.h"
#include "sysc.h"
#include "dma.h"
#include "i2s.h"
#include "ac97.h"
#include "audio.h"



#define PCM_CODEC			WM8580
#define PCM_NORMAL_DMA


// Codec Master CLK ON/OFF

#define PCMCTL_EN_TX_DMA						(1 << 6)
#define PCMCTL_EN_RX_DMA						(1 << 5)
#define PCMCTL_EN_TX_FIFO						(1 << 2)
#define PCMCTL_EN_RX_FIFO						(1 << 1)
#define PCMCTL_EN_PCMIF							(1 << 0)
#define PCMCTL_ST_RXFIFODIPSTICK(n)				(((n)&0x3f) <<7)
#define PCMCTL_ST_TXFIFODIPSTICK(n)				(((n)&0x3f) <<13)

#define PCMCLK_EN_DivideLogic						(1 << 19)

#define PCMIRQ_Enable_IRQ_TO_ARM				(1 << 14)
#define PCMIRQ_Enable_ALLPCMIRQ					(0x1fff)
#define PCMIRQ_Enable_TransferDone				(1 << 12)
#define PCMIRQ_Enable_TX_FIFO_EMPTY				(1 << 11)
#define PCMIRQ_Enable_TX_FIFO_ALMOST_EMPTY		(1 << 10)
#define PCMIRQ_Enable_TX_FIFO_FULL				(1 << 9)
#define PCMIRQ_Enable_TX_FIFO_ALMOST_FULL		(1 << 8)
#define PCMIRQ_Enable_TX_FIFO_STARVE_ERROR		(1 << 7)
#define PCMIRQ_Enable_TX_FIFO_OVERFLOW_ERROR	(1 << 6)
#define PCMIRQ_Enable_RX_FIFO_EMPTY				(1 << 5)
#define PCMIRQ_Enable_RX_FIFO_ALMOST_EMPTY		(1 << 4)
#define PCMIRQ_Enable_RX_FIFO_FULL				(1 << 3)
#define PCMIRQ_Enable_RX_FIFO_ALMOST_FULL		(1 << 2)
#define PCMIRQ_Enable_RX_FIFO_STARVE_ERROR		(1 << 1)
#define PCMIRQ_Enable_RX_FIFO_OVERFLOW_ERROR	(1 << 0)

#define PCMIRQ_Status_IRQ_PENDING				(1 << 13)
#define PCMIRQ_Status_TX_FIFO_EMPTY				(1 << 11)
#define PCMIRQ_Status_TX_FIFO_ALMOST_EMPTY		(1 << 10)
#define PCMIRQ_Status_TX_FIFO_FULL				(1 << 9)
#define PCMIRQ_Status_TX_FIFO_ALMOST_FULL		(1 << 8)
#define PCMIRQ_Status_TX_FIFO_STARVE_ERROR		(1 << 7)
#define PCMIRQ_Status_TX_FIFO_OVERFLOW_ERROR	(1 << 6)
#define PCMIRQ_Status_RX_FIFO_EMPTY				(1 << 5)
#define PCMIRQ_Status_RX_FIFO_ALMOST_EMPTY		(1 << 4)
#define PCMIRQ_Status_RX_FIFO_FULL				(1 << 3)
#define PCMIRQ_Status_RX_FIFO_ALMOST_FULL		(1 << 2)
#define PCMIRQ_Status_RX_FIFO_STARVE_ERROR		(1 << 1)
#define PCMIRQ_Status_RX_FIFO_OVERFLOW_ERROR	(1 << 0)

#define PCMFIFO_Status_TX_FIFO_Count				(0x3f << 14)
#define PCMFIFO_Status_TX_FIFO_EMPTY				(1 << 13)
#define PCMFIFO_Status_TX_FIFO_ALMOST_EMPTY		(1 << 12)
#define PCMFIFO_Status_TX_FIFO_FULL				(1 << 11)
#define PCMFIFO_Status_TX_FIFO_ALMOST_FULL		(1 << 10)
#define PCMFIFO_Status_RX_FIFO_Count				(0x3f << 4)
#define PCMFIFO_Status_RX_FIFO_EMPTY				(1 << 3)
#define PCMFIFO_Status_RX_FIFO_ALMOST_EMPTY		(1 << 2)
#define PCMFIFO_Status_RX_FIFO_FULL				(1 << 1)
#define PCMFIFO_Status_RX_FIFO_ALMOST_FULL		(1 << 0)

#define PCM_REC_BUF  				CODEC_MEM_ST	//0x51000000
#define PCM_REC_LEN 				0xfffff

typedef enum 
{
	rPCMCTRL = 0x0,		
	rPCMCLKCTL = 0x4,		
	rPCMTXFIFO = 0x8,		
	rPCMRXFIFO = 0xc,   	
	rPCMIRQCTL = 0x10,		
	rPCMIRQSTAT= 0x14,		
	rPCMFIFOSTAT = 0x18,	
	rPCMCLRINT = 0x20		
}PCM_SFR;



#define IICOutp32(offset, x)	Outp32(offset, x)
#define IICInp32(offset) 		Inp32(offset)

#define PCM_RXFIFO_EN   			(1<<1)
#define PCM_TXFIFO_EN			(1<<2)
#define RX_MSB_POS0				(0<<3)
#define RX_MSB_POS1				(1<<3)
#define TX_MSB_POS0				(0<<4)
#define TX_MSB_POS1				(1<<4)
#define PCM_RX_DMA_EN			(1<<5)
#define PCM_TX_DMA_EN			(1<<6)
#define RXFIFO_DIPSTICK(n)		(((n)&0x3f) <<7)
#define TXFIFO_DIPSTICK(n)		(((n)&0x3f) <<13)

#if 0	//For PCM SFR W/R Test
REGINFO		sPCMRegInfo[] = 
{ 

	{"			", PCM0_BASE+0x00, 32, RW, DPDB, 0, 0},   
	{"			", PCM0_BASE+0x04, 32, RW, DPDB, 0, 0},
	{"			", PCM0_BASE+0x08, 32, RW, DPDB, 0, 0},
	{"			", PCM0_BASE+0x0C, 32, RW, DPDB, 0, 0},
	{"			", PCM0_BASE+0x10, 32, RW, DPDB, 0, 0},   
	{"			", PCM0_BASE+0x14, 32, RW, DPDB, 0, 0},
	{"			", PCM0_BASE+0x18, 32, RW, DPDB, 0, 0},
	{"			", PCM0_BASE+0x20, 32, RW, DPDB, 0, 0},   

	{"			", PCM1_BASE+0x00, 32, RW, DPDB, 0, 0},   
	{"			", PCM1_BASE+0x04, 32, RW, DPDB, 0, 0},
	{"			", PCM1_BASE+0x08, 32, RW, DPDB, 0, 0},
	{"			", PCM1_BASE+0x0C, 32, RW, DPDB, 0, 0},
	{"			", PCM1_BASE+0x10, 32, RW, DPDB, 0, 0},   
	{"			", PCM1_BASE+0x14, 32, RW, DPDB, 0, 0},
	{"			", PCM1_BASE+0x18, 32, RW, DPDB, 0, 0},
	{"			", PCM1_BASE+0x20, 32, RW, DPDB, 0, 0},   

};
#endif

///////////////////////////////////////////////////////////////////////////////////////

//Port Selction
typedef enum
{
	PCM_TX = 0,
	PCM_RX = 1,
	PCM_TXRX = 2
}PCM_IFMODE;
//Clock Source Selction 
typedef enum
{
	PCM_MOUT_EPLL,
	PCM_DOUT_MPLL,
	PCM_FIN_EPLL,
	PCM_PCMCDCLK,
	PCM_PCLK
}PCM_CLKSRC;

typedef enum 
{
	PCMIRQALL				= 0x1fff,
	TRANSFER_DONE			= (1 << 12), 
	TXFIFO_EMPTY			= (1 << 11),
	TXFIFO_ALMOST_EMPTY	= (1 << 10),
	TXFIFO_FULL				= (1 << 9),
	TXFIFO_ALMOST_FULL		= (1 << 8),
	TXFIFO_ERROR_STARVE	= (1 << 7),
	TXFIFO_ERROR_OVERFLOW	= (1 << 6),
	RXFIFO_EMPTY			= (1 << 5),
	RXFIFO_ALMOST_EMPTY	= (1 << 4),
	RX_FIFO_FULL				= (1 << 3),
	RX_FIFO_ALMOST_FULL		= (1 << 2),
	RXFIFO_ERROR_STARVE	= (1 << 1),
	RXFIFO_ERROR_OVERFLOW	= (1 << 0)
}PCM_INT;

typedef enum
{
	Init, Set
}PCM_Configuration;
///////////////////////////////////////////////////////////////////////////////////////

typedef struct
{
	// IP Operation
	u16						m_uTXFIFODipstick;
	u16						m_uRXFIFODipstick;
	PCM_CLKSRC				m_ePCMClkSrc;
	SerialDataFormat			m_ePCMMSBPosition;	
	// DMA
	u32 						m_uNumDma;
	DMASELECT_eID 			m_eDmaId;
	DMA_UNIT 				m_eDmaUnit;
	DMA_CH 				m_eDmaCh;
	u32 						m_uPcmRxFifoAddr;
	u32 						m_uPcmTxFifoAddr;	
	DREQ_SRC 				m_eDreqSrc;
	DREQ_SRC				m_eDreqDst;
	// Interrupt
	u32						m_uIntAspects;
	u32 						m_uNumInt;	
	// CLK
	u32						m_uPCMSClk;	
	u32						m_uPCMSync;	
	// Line
	SMDKLine				m_eLine;	
	// Memory Address
	u16*					m_uRecBufferStartAddr;
	u32						m_uPcmSize;
	u16 *					m_uRecBufferEndAddr;
}PCM_Infor;

typedef struct PCMtestFuncMenu {
	void (*func)(AUDIO_PORT); 
	const char *desc;
} PCMtestFuncMenu;


///////////////////////////////////////////////////////////////////////////////////////

void PCM_Init(AUDIO_PORT rhs_ePort);

void PCM_Sel_PCMCLKSOURCE(AUDIO_PORT rhs_ePort);
void  PCM_Sel_PCMSCLK(AUDIO_PORT rhs_ePort);
void PCM_Sel_PCMSYNC(AUDIO_PORT rhs_ePort);
void PCM_Sel_SyncPosition(AUDIO_PORT rhs_ePort);
void PCM_Sel_Interrupt(AUDIO_PORT rhs_ePort);

void PCM_InitPort(AUDIO_PORT ePort);
void PCM_CLKOUT(AUDIO_PORT rhs_ePort);
void PCM_PCMInDMA(AUDIO_PORT rhs_ePort);
void PCM_PCMOutDMA(AUDIO_PORT rhs_ePort);
void PCM_BypassDMA(AUDIO_PORT rhs_ePort);
void PCM_PCMOutDMAStop(AUDIO_PORT rhs_ePort);
void PCM_PCMInInt(AUDIO_PORT rhs_ePort);
void PCM_PCMOutInt(AUDIO_PORT rhs_ePort);

void PCM_PCMInPolling(AUDIO_PORT rhs_ePort);
void PCM_PCMOutPolling(AUDIO_PORT rhs_ePort);


void __irq Isr_PCMPORT0_PCMIn_DMADone(void);
void __irq Isr_PCMPORT0_PCMOut_DMADone(void);
void __irq Isr_PCMPORT1_PCMIn_DMADone(void);
void __irq Isr_PCMPORT1_PCMOut_DMADone(void);

void __irq Isr_PCMPORT0_PCMIn(void);
void __irq Isr_PCMPORT0_PCMOut(void);
void __irq Isr_PCMPORT1_PCMIn(void);
void __irq Isr_PCMPORT1_PCMOut(void);

void __irq ISR_PCM_Port0TXFIFOEr(void);
void __irq ISR_PCM_Port1TXStarveEr(void);
void __irq ISR_PCM_Port0RXOverflowEr(void);
void __irq ISR_PCM_Port1RXOverflowEr(void);


void PCMCTL_PutData(AUDIO_PORT rhs_ePort);
void PCMCTL_EnablePCMIF(AUDIO_PORT rhs_ePort,   u8 uActive);
void PCMCTL_EnableDMA(AUDIO_PORT rhs_ePort,  PCM_IFMODE rhs_eIFMode, u8 uActive);
void PCMCTL_EnableFIFO(AUDIO_PORT rhs_ePort, PCM_IFMODE rhs_eIFMode, u8 uActive);

void PCMCLKCTL_SetDivideLogic(AUDIO_PORT rhs_ePort, u32 uSclkDiv, u32  uSyncDiv);
void PCMCLKCTL_ActiveDivideLogic(AUDIO_PORT rhs_ePort, u8 uActive);
void PCMCLKCTL_SetSclkDivVal(AUDIO_PORT rhs_ePort, u32 uSclkDiv);
void PCMCLKCTL_SetSyncDivVal(AUDIO_PORT rhs_ePort, u32 uSyncDiv);

void PCMIRQCTL_EnableInt(AUDIO_PORT rhs_ePort, u8 rhs_uActive);



void PCMIRQCTL_SetIRQMode(AUDIO_PORT rhs_ePort, PCM_INT rhs_ePcmInt,  u8 rhs_uActive);
void PCMIRQCTL_GetIRQMode(AUDIO_PORT rhs_ePort);

void PCM_ClearInt(AUDIO_PORT);

void PCM_SelCLKAUDIO(AUDIO_PORT ePort, PCM_CLKSRC eClkSrc);
void PCM_GetClkValAndClkDir(AUDIO_PORT rhs_ePort, u32* uSclkDiv, u32* uSyncDiv);

void PCMOutp32(AUDIO_PORT rhs_ePort, PCM_SFR offset, u32 x);
void PCMOutp16(AUDIO_PORT rhs_ePort, PCM_SFR offset, u32 x);
u32  PCMInp32(AUDIO_PORT rhs_ePort, PCM_SFR offset);
u16 PCMInp16(AUDIO_PORT rhs_ePort, PCM_SFR offset);


void PCM_CodecExitPCMIn(AUDIO_PORT rhs_ePort);
void PCM_CodecInitPCMIn(u32 eSync, u32 eSclk, SerialDataFormat eMSBPos,  PCM_CLKSRC eClkSrc);
void PCM_CodecExitPCMOut(void);
void PCM_CodecInitPCMOut(u32 eSync, u32 eSclk, SerialDataFormat eMSBPos,  PCM_CLKSRC eClkSrc);

void PCM_CODEC_IICWrite(u32 slvAddr, u32 addr, u8 data);
u8 PCM_CODEC_IICRead(u32 slvAddr, u8 addr);

double PCMGetEPLL(void);
double PCMGetMPLL(void);


#ifdef __cplusplus
}
#endif

#endif /*__PCM_H__*/

