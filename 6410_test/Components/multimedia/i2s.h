/**************************************************************************************
* 
*	Project Name : S3C6410 Validation
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
*	File Name : i2s.h
*  
*	File Description : This file defines the register access function
*						and declares prototypes of i2s funcions
*
*	Author : Sung-Hyun, Na
*	Dept. : AP Development Team
*	Created Date : 2008/03/04
*	Version : 0.1 
* 
*	History
*	- Created(Sung-Hyun, Na 2008/03/04)
*  
**************************************************************************************/
#ifndef __I2S_H__
#define __I2S_H__
 
  
#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <sysc.h>
#include <dma.h>
#include <timer.h>
#include "def.h"
#include "audio.h"

#define I2S_CODEC			WM8580

#define I2S_EXTERNALCLK		12288000
#define I2S_USERCLKOUT1		11289600
#define I2S_USERCLKOUT2		12288000
#define I2S_RECORD_LENGTH	0xffff0

#define I2S_NORMAL_DMA					// normal DMA or SDMA Selection.


//Descript I2S SFR

typedef enum 
{
	rI2SCON = 0x0,		// IIS Control Register (R/W)
	rI2SMOD = 0x4,		// IIS Mode Register (R/W)
	rI2SFIC = 0x8,		// IIS FIFO Control Register (R/W)
	rI2SPSR = 0xc,   		// IIS Clock Divider Control Register (R/W)
	rI2STXD = 0x10,		// IIS Tx Data FIFO (W)
	rI2SRXD	= 0x14,		// IIS Rx Data FIFO (R)	
}I2S_SFR;

#if 0 					//Struct for Regitster Read/Write Test
REGINFO		sRegInfo[] = 
{
	{"rI2S0CON					", I2S0_BASE+rI2SCON, 19, RW, DPDB, 0, 0x00000E00},   	
	{"rI2S0MOD					", I2S0_BASE+rI2SMOD, 14, RW, DPDB, 0, 0x00000000},   
	{"rI2S0FIC					", I2S0_BASE+rI2SFIC, 15, RW, DPDB, 0, 	0x00000000},   
	{"rI2S0PSR					", I2S0_BASE+rI2SPSR, 15, RW, DPDB, 0, 0x00000000},   
	{"rI2S0TXD					", I2S0_BASE+rI2STXD, 31, WO, DPDB, 0, 0x00000000},   
	{"rI2S0RXD					", I2S0_BASE+rI2SRXD, 31, RO, DPDB, 0, 0x00000000},   
		
	{"rI2S1CON					", I2S1_BASE+rI2SCON,  19, RW, DPDB, 0, 0x00000E00},   
	{"rI2S1MOD					", I2S1_BASE+rI2SMOD, 14, RO, DPDB, 0, 0x00000000},   
	{"rI2S1FIC					", I2S1_BASE+rI2SFIC, 15, RW, DPDB, 0, 0x00000000},   
	{"rI2S1PSR					", I2S1_BASE+rI2SPSR, 15, RW, DPDB, 0, 0x00000000},   
	{"rI2S1TXD					", I2S1_BASE+rI2STXD, 31, WO, DPDB, 0, 0x00000000},   
	{"rI2S1RXD					", I2S1_BASE+rI2SRXD, 31, RO, DPDB, 0, 0x00000000},   
};
#endif 
// EN : Enable
// DA : disable
// CLR : Clrear
// ST : Status
// PA : Pause (~Enable)
// FL : Flush

#define I2S_CLR_OverrunIntr			(1 << 19)
#define I2S_EN_OverrunIntr				(1 << 18)
#define I2S_CLR_UnderrunIntr			(1 << 17)
#define I2S_EN_UnderrunIntr			(1 << 16)

#define I2S_ST_LRCLK					(1 << 11)
#define I2S_ST_TX_FIFO_EMPTY			(1 << 10)
#define I2S_Status_RX_FIFO_EMPTY		(1 <<  9)
#define I2S_Status_TX_FIFO_FULL		(1 << 8)
#define I2S_Status_RX_FIFO_FULL		(1 << 7)
#define I2S_Status_RX_FIFO_CNT			(0x1f << 0)
#define I2S_Status_TX_FIFO_CNT			(0x1f << 8)
#define I2S_Pause_TX_DMA				(1 << 6)
#define I2S_Pause_RX_DMA				(1 << 5)
#define I2S_Pause_TX_IF				(1 << 4)
#define I2S_Pause_RX_IF				(1 << 3)
#define I2S_Enable_TX_DMA			(1 << 2)
#define I2S_Enable_RX_DMA			(1 << 1)
#define I2S_Enable_I2SIF				(1 << 0)
#define I2S_Flush_TX_FIFO				(1 << 15)
#define I2S_Flush_RX_FIFO				(1 << 7)
#define I2S_Enable_Prescaler			(1 << 15)

typedef enum
{
	Internal_CLK = 0,
	External_CLK = 1,
}I2S_CDCLKCon;

typedef enum
{
	I2S_MOUT_EPLL = 0,
	I2S_DOUT_MPLL = 1,
	I2S_FIN = 2,
	I2S_EXTERNALCDCLK = 3,	
	I2S_PCLK = 4,	
}I2S_CLKSRC;

typedef enum
{
	TXOnly = 0,
	RXOnly = 1,
	TXRXBoth = 2,
}I2S_IFMode;

typedef enum
{
	RightHigh = 0,
	LeftHigh = 1,
}I2S_LRCLKPolarity;

typedef enum
{
	RFS_256fs = 0,		//8bit stereo
	RFS_512fs = 1,		//16bit stereo
	RFS_384fs = 2,		//12bit stereo
	RFS_768fs = 3, 		//24bit stereo
}I2S_LRCLKLength;		//RFS

typedef enum
{
	BFS_32fs = 0,		//16bit
	BFS_48fs = 1,		//24bit
	BFS_16fs = 2, 	//8bit
	BFS_24fs = 3,		//12bit
}I2S_EffectiveLength;		//BFS


typedef enum
{
	I2S_TX,
	I2S_RX,
}I2S_IntrSource;

#define ENABLE						(1)
#define DISABLE						(0)

#define ACTIVE						(1)
#define INACTIVE						(0)

#define PAUSE						(1)
#define PLAY							(0)

#define FLUSH						(1)
#define NON_FLUSH					(0)

#define I2S_CONNUM					(2)

#define WRDATA						(1)
#define POLLACK     					(2)
#define RDDATA						(3)
#define SETRDADDR					(4)

#define I2S_MODE_PLAY				0
#define I2S_MODE_RECORD_MICIn 		1
#define I2S_MODE_RECORD_LineIn		2

#define I2S_STATUS_PLAY				0
#define I2S_STATUS_RECORD			1

#define I2S_DMAC0_REC_COUNT		0x40
#define I2S_DMAC1_PLAY_COUNT		0x40
#define I2S_DMA_TRANSFER_SIZE		0xff0


#define I2S_I2C_BUF_SIZE				0x20
#define I2S_I2C_WRDATA				(1)
#define I2S_I2C_POLLACK 				(2)
#define I2S_I2C_RDDATA				(3)
#define I2S_I2C_SETRDADDR			(4)

//#define IICOutp32(offset, x) Outp32(offset, x)
//#define IICInp32(offset) Inp32(offset)

typedef struct
{
	//IP Operation
	OPMode				m_eOpMode;
	PCMWordLength		m_eWordLength;
	I2S_IFMode			m_eIFMode;
	I2S_LRCLKPolarity		m_eLRCLKPolarity;
	SerialDataFormat		m_eDataFormat;
	I2S_LRCLKLength		m_eRFS;
	I2S_EffectiveLength	m_eBFS;
	//DMA
	u32 					m_uNumDma;
	DMASELECT_eID 		m_eDmaId;
	DMA_UNIT 			m_eDmaUnit;
	DMA_CH 			m_eDmaCh;
	u32 					m_uI2SRxFifoAddr;
	u32 					m_uI2STxFifoAddr;	
	DREQ_SRC 			m_eDreqSrc;
	DREQ_SRC			m_eDreqDst;
	//Interrupt
	u32 					m_uNumInt;		
	//CLK
	double				m_fSourceCLK;
	double				m_fCodecCLK;
	I2S_CLKSRC			m_CLKSRC;	
	u32					m_dSamplingFreq;	
	//Codec
	//Address
	u32*				m_uRecBufferStartAddr;
	u32*				m_uPlayBufferStartAddr;
	u32					m_uPcmDataSize;
	
}I2S_Infor;

typedef enum
{
	I2S_SINGLE,
	I2S_DUAL,
}I2S_TestMode;

typedef struct I2StestFuncMenu {
	void (*func)(AUDIO_PORT); 
	const char *desc;
} I2StestFuncMenu;


u32 I2S_Init(AUDIO_PORT);


void I2S_InitPort(AUDIO_PORT);
void I2S_SetCDCLKPort(AUDIO_PORT);
void I2S_ReturnPort(AUDIO_PORT);
// V0.1
void I2S_CDCLKOut(AUDIO_PORT rhs_ePort);
void I2S_SetEpllCDCLKOut(AUDIO_PORT rhs_ePort, u32 uSamplerate);



// V0.2
void	I2S_DataInDMAStart(AUDIO_PORT ePort);
void	I2S_DataInDMAStop(AUDIO_PORT ePort);
void I2S_DataOutDMAStart(AUDIO_PORT ePort);
void	I2S_DataOutDMAStop(AUDIO_PORT ePort);

u32	I2S_DataBypassDMA(AUDIO_PORT ePort);

void I2S_DataInIntr(AUDIO_PORT rhs_ePort);
void I2S_DataOutIntr(AUDIO_PORT rhs_ePort);

void I2S_DataInPolling(AUDIO_PORT rhs_ePort);
void I2S_DataOutPolling(AUDIO_PORT rhs_ePort);
void I2S_DataBypassPolling(AUDIO_PORT rhs_ePort);


/*---------------------------------- APIs of rI2SCON ---------------------------------*/
void I2SCON_SetIFMode(AUDIO_PORT ePort, I2S_IFMode eIFMode);
void I2SCON_ActiveIF(AUDIO_PORT, u8);
void I2SCON_ActiveDMA(AUDIO_PORT, I2S_IFMode, u8);
void I2SCON_PauseDMA(AUDIO_PORT ePort, I2S_IFMode eIFMode, u8 ucPause);
void I2SCON_PauseIF(AUDIO_PORT ePort, I2S_IFMode eIFMode, u8 ucPause);
void I2SCON_EnableIntr(AUDIO_PORT rhs_ePort, I2S_IntrSource rhs_eISource, u8 ucActive);
void I2SCON_ClrOverIntr(AUDIO_PORT rhs_ePort);
void I2SCON_ClrUnderIntr(AUDIO_PORT rhs_ePort);
/*---------------------------------- APIs of rI2SMOD ---------------------------------*/
void I2SMOD_PutData(AUDIO_PORT ePort, I2S_Infor* p_ePort);
void I2SMOD_SetTXR(AUDIO_PORT ePort, I2S_IFMode eIFMode);
void I2S_SelOperationMode(AUDIO_PORT);
void I2SMOD_ChangeWordLength(AUDIO_PORT);
void I2SMOD_SetWordLength(AUDIO_PORT ePort, PCMWordLength eWordL);
void I2SMOD_ChangeInterface(AUDIO_PORT);
void I2S_Sel_LRCLKPolarity(AUDIO_PORT);
void I2S_Sel_SerialDataFormat(AUDIO_PORT);
void I2S_Sel_CLKSource(AUDIO_PORT);
void I2S_Sel_RFS(AUDIO_PORT);
void I2S_Sel_BFS(AUDIO_PORT);
void I2S_SamplingFreq(AUDIO_PORT);
/*---------------------------------- APIs of rI2SFIC ---------------------------------*/
void I2SFIC_AutoFlush(AUDIO_PORT, I2S_IFMode);
/*---------------------------------- APIs of rI2SPSR ---------------------------------*/
void I2SPSR_SetPrescaler(AUDIO_PORT ePort, u32 rhs_uPrsVal);
void I2SPSR_ActivePRS(AUDIO_PORT ePort, u8 uActive);
void I2SPSR_PutPRSValue(AUDIO_PORT ePort, u32 rhs_uPrsVal);
/*---------------------------------- APIs of rI2STXD ---------------------------------*/
/*---------------------------------- APIs of rI2SRXD ---------------------------------*/
/*---------------------------------- ISR Routines ---------------------------------*/
void __irq ISR_I2SPort0_TXUnderrun(void);
void __irq ISR_I2SPort1_TXUnderrun(void);
void __irq ISR_I2SPort0_RXOverrun(void);
void __irq ISR_I2SPort1_RXOverrun(void);

void __irq ISR_Port0DMARecDone(void);
void __irq ISR_Port1DMARecDone(void);
void __irq ISR_Port0DMAPlayDone(void);
void __irq ISR_Port1DMAPlayDone(void);
void __irq ISR_Port0DMABypass(void);
void __irq ISR_Port1DMABypass(void);
void __irq ISR_Port0DMAPlayLoopBackDone(void);
void __irq ISR_Port1DMAPlayLoopBackDone(void);

void __irq ISR_I2SOverRun(void);
void __irq ISR_I2SUnderRun(void);
/*---------------------------------- APIs of general I2S ---------------------------------*/
void I2S_Outp32(AUDIO_PORT  ePort, I2S_SFR offset, u32 x);
u32  I2S_Inp32(AUDIO_PORT ePort, I2S_SFR offset);
u16  I2S_Inp16(AUDIO_PORT ePort, I2S_SFR offset);
/*---------------------------------- APIs of Clock ---------------------------------*/
void I2S_SelCLKAUDIO(AUDIO_PORT ePort, I2S_CLKSRC eClkSrc);
double I2S_SetEPLL(AUDIO_PORT);
double I2S_SetEPLLClk(double EpllClk);
double I2S_GetMPLL(AUDIO_PORT ePort);
double I2S_GetEPLL(AUDIO_PORT ePort);
u32 I2S_GetPRSVal(AUDIO_PORT);
u32 I2S_GetPRSVal2(AUDIO_PORT ePort, u32 uSourceCLK, u32 uTargetCLK);
I2S_Infor* I2S_GetI2SInfor(AUDIO_PORT rhs_ePort);

#ifdef __cplusplus
}
#endif

#endif /*__I2S_H__*/



