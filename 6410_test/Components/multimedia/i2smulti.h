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
*	File Name : i2smulti.h
*  
*	File Description : This file defines the register access function and declares prototypes of i2s funcions
*
*	Author : Sunghyun,Na
*	Dept. : AP Development Team
*	Created Date : 2008/02/01
*	Version : 0.1 
* 
*	History
*	- Created(Sunghyun,Na 2008/02/01)
*  
**************************************************************************************/
#ifndef __I2SMULTI_H__
#define __I2SMULTI_H__
 
  
#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <sysc.h>
#include <dma.h>
#include <timer.h>

#include "i2s.h"
#include "def.h"
#include "audio.h"

#define I2SMULTI_CODEC		WM8580

#define I2SMULTI_NORMAL_DMA					// normal DMA or SDMA Selection.
#define I2SMULTI_EXTERNALCLK	16934400
#define I2SMULTI_PCMEXTECLK		12288000
#define I2SMULTI_USERCLKOUT1	11289600
#define I2SMULTI_USERCLKOUT2	12288000
#define I2SMULTI_RECORD_LENGTH	(0x80000)


#define I2SMULTI_Outp32(offset, x) 			Outp32(I2SMULTI_BASE+offset, x)
#define I2SMULTI_Inp32(offset) 				Inp32(I2SMULTI_BASE+offset)



//Descript I2S SFR

typedef enum 
{
	rI2SMULTICON = 0x0,			// IIS Control Register (R/W)
	rI2SMULTIMOD = 0x4,			// IIS Mode Register (R/W)
	rI2SMULTIFIC = 0x8,			// IIS FIFO Control Register (R/W)
	rI2SMULTIPSR = 0xc,   		// IIS Clock Divider Control Register (R/W)
	rI2SMULTITXD = 0x10,			// IIS Tx Data FIFO (W)
	rI2SMULTIRXD	= 0x14,		// IIS Rx Data FIFO (R)	
}I2SMULTI_SFR;

#define I2SMULTI_Clear_Underrun_Intr			(1 << 17)
#define I2SMULTI_Enable_Underrun_Intr			(1 << 16)
#define I2SMULTI_Status_TX2_FIFO_EMPTY		(1 << 15)
#define I2SMULTI_Status_TX1_FIFO_EMPTY		(1 << 14)
#define I2SMULTI_Status_TX2_FIFO_FULL			(1 << 13)
#define I2SMULTI_Status_TX1_FIFO_FULL			(1 << 12)
#define I2SMULTI_Status_LRCLK					(1 << 11)
#define I2SMULTI_Status_TX0_FIFO_EMPTY		(1 << 10)
#define I2SMULTI_Status_RX_FIFO_EMPTY			(1 <<  9)
#define I2SMULTI_Status_TX0_FIFO_FULL			(1 << 8)
#define I2SMULTI_Status_RX_FIFO_FULL			(1 << 7)
#define I2SMULTI_Pause_TX_DMA				(1 << 6)
#define I2SMULTI_Pause_RX_DMA				(1 << 5)
#define I2SMULTI_Pause_TX_IF					(1 << 4)
#define I2SMULTI_Pause_RX_IF					(1 << 3)
#define I2SMULTI_Enable_TX_DMA				(1 << 2)
#define I2SMULTI_Enable_RX_DMA				(1 << 1)
#define I2SMULTI_Enable_I2SIF					(1 << 0)
#define I2SMULTI_Flush_TX_FIFO				(1 << 15)
#define I2SMULTI_Flush_RX_FIFO				(1 << 7)

#define I2SMULTI_Enable_Prescaler				(1 << 15)

#define I2SMULTI_Status_RX_FIFO_CNT			(0x1f << 0)
#define I2SMULTI_Status_TX0_FIFO_CNT			(0x1f << 8)
#define I2SMULTI_Status_TX1_FIFO_CNT			(0x1f << 16)
#define I2SMULTI_Status_TX2_FIFO_CNT			(0x1f << 24)

typedef enum
{
	NoDiscard 	 = 0,
	Discard15to0	 = 1,
	Discard31to16 = 2
}I2SMULTI_CDD;

typedef enum
{
	TX0 	= 0,
	TX1 	= 1,
	TX2 	= 2,
	ALLTX	= 3
}I2SMULTI_DCE;

typedef enum
{
	I2SMUL_EPLL_OUT = 0,
	I2SMUL_MPLL_OUT = 1,
	I2SMUL_SYSTEM_FIN = 2,
	I2SMUL_EXTERNALCDCLK = 3,

	I2SMUL_PCMEXTCLK1 = 4,
	I2SMUL_PCLK_SRC  = 5,	
}I2SMULTI_CLKSRC;

#define I2SMULTI_CONNUM			(1)

#define ENABLE						(1)
#define DISABLE						(0)

#define ACTIVE						(1)
#define INACTIVE						(0)

#define PAUSE						(1)
#define PLAY							(0)

#define FLUSH						(1)
#define NON_FLUSH					(0)

#define WRDATA						(1)
#define POLLACK     					(2)
#define RDDATA						(3)
#define SETRDADDR					(4)

#define I2SMULTI_DMAC0_REC_COUNT	0x40
#define I2SMULTI_DMAC1_PLAY_COUNT	0x40
#define I2SMULTI_DMA_TRANSFER_SIZE	0xff0
//#define I2S_RECORD_LENGTH			(I2S_DMA_TRANSFER_SIZE*I2S_DMAC0_REC_COUNT*4)
//#define I2S_RECORD_LENGTH			(I2S_DMA_TRANSFER_SIZE*I2S_DMAC0_REC_COUNT*4)/8

typedef struct
{
	//IP Operation
	OPMode					m_eOpMode;
	PCMWordLength			m_eWordLength;
	I2S_IFMode				m_eIFMode;				// Define i2s.h
	I2S_LRCLKPolarity			m_eLRCLKPolarity;			// Define i2s.h
	SerialDataFormat			m_eDataFormat;			// Define audio.h
	I2S_LRCLKLength			m_eRFS;					// Define i2s.h
	I2S_EffectiveLength		m_eBFS;					// Define i2s.h
	I2SMULTI_CDD			m_eChannel1Discard;
	I2SMULTI_CDD			m_eChannel2Discard;
	I2SMULTI_DCE			m_eDataChannel;
	//DMA
	u32 						m_uNumDma;	
	DMASELECT_eID 			m_eDmaId;
	DMA_UNIT 				m_eDmaUnit;
	DMA_CH 				m_eDmaCh;
	u32 						m_uI2SMULTIRxFifoAddr;
	u32 						m_uI2SMULTITxFifoAddr;	
	DREQ_SRC 				m_eDreqSrc;
	DREQ_SRC				m_eDreqDst;
	//Interrupt
	u32 						m_uNumInt;		
	//CLK
	double					m_fSourceCLK;
	double					m_fCodecCLK;
	I2SMULTI_CLKSRC			m_CLKSRC;	
	u32						m_dSamplingFreq;	
	//Codec
	//Address
	u32*					m_uRecBufferStartAddr;
	u32*					m_uRecBufferStartAddr1;
	u32*					m_uRecBufferStartAddr2;
	u32*					m_uPlayBufferStartAddr;
	u32						m_uPcmDataSize;
	u32						m_uPlayDataSize;
	
}I2SMULTI_Infor;

typedef struct I2SMULTItestFuncMenu {
	void (*func)(AUDIO_PORT, I2SMULTI_Infor*); 
	const char *desc;
} I2SMULTItestFuncMenu;

I2SMULTI_Infor* I2SMULTI_GetI2SMULTIInfor(AUDIO_PORT rhs_ePort);


u8	I2SMULTI_Init(AUDIO_PORT ePort);
void I2SMULTI_Sel_OperationMode(AUDIO_PORT, I2SMULTI_Infor *);
void I2SMULTI_Sel_WordLength(AUDIO_PORT, I2SMULTI_Infor *);
void I2SMULTI_Sel_Interface(AUDIO_PORT, I2SMULTI_Infor *);
void I2SMULTI_Sel_LRCLKPolarity(AUDIO_PORT, I2SMULTI_Infor *);
void I2SMULTI_Sel_SerialDataFormat(AUDIO_PORT, I2SMULTI_Infor *);
void I2SMULTI_Sel_CLKSource(AUDIO_PORT, I2SMULTI_Infor *);
void I2SMULTI_Sel_RFS(AUDIO_PORT, I2SMULTI_Infor *);
void I2SMULTI_Sel_BFS(AUDIO_PORT, I2SMULTI_Infor *);
void I2SMULTI_SamplingFreq(AUDIO_PORT, I2SMULTI_Infor *);
void I2SMULTI_Sel_Discard1(AUDIO_PORT rhs_ePort, I2SMULTI_Infor * rhs_pInfor);
void I2SMULTI_Sel_Discard2(AUDIO_PORT rhs_ePort, I2SMULTI_Infor * rhs_pInfor);
void I2SMULTI_Sel_EnableCh(AUDIO_PORT rhs_ePort, I2SMULTI_Infor * rhs_pInfor);

void I2SMULTI_InitPort(AUDIO_PORT);
void I2SMULTI_ReturnPort(AUDIO_PORT);
void I2SMULTI_CDCLKOut(AUDIO_PORT rhs_ePort, I2SMULTI_Infor* rhs_pInfor);
void I2SMULTI_SetEpllCDCLKOut(AUDIO_PORT rhs_ePort, I2SMULTI_Infor* rhs_pInfor, u32 uSampleRate);
void	I2SMULTI_DataInDMAMode(AUDIO_PORT ePort, I2SMULTI_Infor * rhs_pInfor);
void	I2SMULTI_DataInDMAStop(AUDIO_PORT ePort, I2SMULTI_Infor * rhs_pInfor);
void I2SMULTI_DataOutDMA(AUDIO_PORT ePort, I2SMULTI_Infor * rhs_pInfor);
void	I2SMULTI_DataOutDMAStop(AUDIO_PORT ePort, I2SMULTI_Infor * rhs_pInfor);

void	I2SMULTI_DataBypassDMA(AUDIO_PORT ePort, I2SMULTI_Infor * rhs_pInfor);

void I2SMULTI_DataInPolling(AUDIO_PORT rhs_ePort, I2SMULTI_Infor * rhs_pInfor);
void I2SMULTI_DataOutPolling(AUDIO_PORT rhs_ePort, I2SMULTI_Infor * rhs_pInfor);

void I2SMULTI_DataBypassPolling(AUDIO_PORT rhs_ePort, I2SMULTI_Infor * rhs_pInfor);
void I2SMULTI_DataOutIntr(AUDIO_PORT rhs_ePort, I2SMULTI_Infor * rhs_pInfor);


/*---------------------------------- APIs of rI2SMULTICON ---------------------------------*/
void I2SMULTICON_SetIFMode(AUDIO_PORT ePort, I2S_IFMode eIFMode);
void I2SMULTICON_ActiveIF(AUDIO_PORT, u8);
void I2SMULTICON_ActiveDMA(AUDIO_PORT, I2S_IFMode, u8);
void I2SMULTICON_PauseDMA(AUDIO_PORT ePort, I2S_IFMode eIFMode, u8 ucPause);
void I2SMULTICON_PauseIF(AUDIO_PORT ePort, I2S_IFMode eIFMode, u8 ucPause);
void I2SMULTICON_EnableIntr(AUDIO_PORT rhs_ePort, u32 rhs_uActive);
void I2SMUlTICON_ClrIntr(void);

/*---------------------------------- APIs of rI2SMULTIMOD ---------------------------------*/
void I2SMULTIMOD_PutData(AUDIO_PORT ePort, I2SMULTI_Infor * rhs_pInfor);
/*---------------------------------- APIs of rI2SMULTIFIC ---------------------------------*/
void I2SMULTIFIC_AutoFlush(AUDIO_PORT, I2S_IFMode);
/*---------------------------------- APIs of rI2SMULTIPSR ---------------------------------*/
void I2SMULTIPSR_SetPrescaler(AUDIO_PORT ePort, u32 rhs_uPrsVal);
void I2SMULTIPSR_ActivePRS(AUDIO_PORT ePort, u8 uActive);
void I2SMULTIPSR_PutPRSValue(AUDIO_PORT ePort, u32 rhs_uPrsVal);
/*---------------------------------- APIs of rI2SMULTITXD ---------------------------------*/
/*---------------------------------- APIs of rI2SMULTIRXD ---------------------------------*/
/*---------------------------------- ISR Routines ---------------------------------*/
void __irq ISR_I2SMULTI_DMARecDone(void);
void __irq ISR_I2SMULTI_DMAPlayDone(void);
void __irq ISR_I2SMULTI_DMALoopBackDone(void);
void __irq ISR_I2SMULTI_DMABypass(void);
void __irq ISR_I2SMULTI_Underrun(void);
void __irq ISR_I2SMULTI_UnderRunAC(void);

/*---------------------------------- APIs of general I2SMULTI ---------------------------------*/
//void I2SMULTI_Outp32(AUDIO_PORT  ePort, I2SMULTI_SFR offset, u32 x);
//u32  I2SMULTI_Inp32(AUDIO_PORT ePort, I2SMULTI_SFR offset);
//u16  I2SMULTI_Inp16(AUDIO_PORT ePort, I2SMULTI_SFR offset);
/*---------------------------------- APIs of Clock ---------------------------------*/
u32 I2SMULTI_MakeChannel(I2SMULTI_Infor* rhs_pInfor);
void I2SMULTI_MergeChannel(I2SMULTI_Infor * rhs_pInfor);
void I2SMULTI_MergeSerial(I2SMULTI_Infor* rhs_pInfor);
void I2SMULTI_MergeEachCh3(I2SMULTI_Infor* rhs_pInfor);
void I2SMULTI_MergeEachCh2(I2SMULTI_Infor* rhs_pInfor);

void I2SMULTI_MergeEachCh(I2SMULTI_Infor* rhs_pInfor);
void I2SMULTI_SelCLKAUDIO(AUDIO_PORT ePort, I2SMULTI_CLKSRC eClkSrc);
void I2SMULTI_SelectPCLK(u8);
void I2SMULTI_SelectEXTCLK(u8);
double I2SMULTI_SetEPLL(AUDIO_PORT);
double I2SMULTI_SetEPLLClk(double EpllClk);
void I2SMULTI_SelectMPLL(u8);
void I2SMULTI_SelectSysExtCLK(u8);
double I2SMULTI_GetMPLL(AUDIO_PORT ePort);
double I2SMULTI_GetEPLL(AUDIO_PORT ePort);
u32 I2SMULTI_GetPRSVal(AUDIO_PORT ePort, I2SMULTI_Infor * rhs_eInfor);
u32 I2SMULTI_GetPRSVal2(AUDIO_PORT ePort, I2SMULTI_Infor * rhs_eInfor, u32 uSourceCLK, u32 uTargetCLK);


#ifdef __cplusplus
}
#endif

#endif /*__I2SMULTI_H__*/

