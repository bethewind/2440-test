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
*	File Name : AC97.c
*  
*	File Description :
*
*	Author	: Yoh-Han Lee
*	Dept. : AP Development Team
*
*	Version : 0.2
* 
*	History
*	- Version 0.1 (2007/03/09)
*	  -> Only available with STAC9767 AC97 Codec.
*   - Version 0.2 (2007/04/04)
*     -> Working with WM9713 AC97 Codec.
*
********************************************************************************/

#ifndef __AC97_H__
#define __AC97_H__
 
  
#ifdef __cplusplus
extern "C" {
#endif

#include "def.h"
#include "sfr6410.h"
#include "library.h"
#include "gpio.h"
#include "Option.h"
#include "dma.h"
#include "Sysc.h"
#include "audio.h"


#define AC97_CODEC							WM9713
#define AC97_NORMAL_DMA

#define AC97_DebugDelay						100
#define AC97_CMDReadDelay					500
#define AC97_CMDWriteDelay					500

#define AC97_REC_LEN 						0xffff0
#define REC_BUF  							CODEC_MEM_ST	//0x51000000
#define PCM_OUT_TRIGGER 					5
#define PCM_IN_TRIGGER 						5
#define MIC_IN_TRIGGER						5

#define AC97_ClearInt_CodecReady				(1 << 30)
#define AC97_ClearInt_UnderrunPCMOut			(1 << 29)
#define AC97_ClearInt_OverrunPCMIn			(1 << 28)
#define AC97_ClearInt_OverrunMICIn			(1 << 27)
#define AC97_ClearInt_ThresholdPCMOut			(1 << 26)
#define AC97_ClearInt_ThresholdPCMIn			(1 << 25)
#define AC97_ClearInt_ThresholdMICIn			(1 << 24)

#define AC97_EnableInt_CodecReady				(1 << 22)
#define AC97_EnableInt_UnderrunPCMOut		(1 << 21)
#define AC97_EnableInt_OverrunPCMIn			(1 << 20)
#define AC97_EnableInt_OverrunMICIn			(1 << 19)
#define AC97_EnableInt_ThresholdPCMOut		(1 << 18)
#define AC97_EnableInt_ThresholdPCMIn			(1 << 17)
#define AC97_EnableInt_ThresholdMICIn			(1 << 16)

#define AC97_Enable_Transfer_ACLINK			(1 << 3)
#define AC97_Enable_ACLINK					(1 << 2)
#define AC97_Enable_Warm_Reset				(1 << 1)
#define AC97_Enable_Cold_Reset				(1 << 0)

#define AC97_Status_CodecReady				(1 << 22)
#define AC97_Status_UnderrunPCMOut			(1 << 21)
#define AC97_Status_OverrunPCMOIn			(1 << 20)
#define AC97_Status_OverrunMICOut			(1 << 19)
#define AC97_Status_ThresholdPCMOut			(1 << 18)
#define AC97_Status_ThresholdPCMIn			(1 << 17)
#define AC97_Status_ThresholdMICIn			(1 << 16)

enum AC97_SFR
{

	rACGLBCTRL 		= 0x0,		// AC97 Global Control Register (R/W)
	rACGLBSTAT 		= 0x4,		// AC97 Global Status Register (R)
	rACCODECCMD 	= 0x8,		// AC97 Codec Command Register (R/W)
	rACCODECSTAT 	= 0xc,   		// AC97 Codec Status Register (R)
	rACPCMADDR 	= 0x10,		// AC97 PCM Out/In Channel FIFO Address Register (R)
	rACMICADDR		= 0x14,		// AC97 MIC in Channel FIFO Address Register (R)
	rACPCMDATA	 	= 0x18,		// AC97 PCM Out/In Channel FIFO Data Register (R/W)
	rACMICDATA	 	= 0x1c		// AC97 MIC in Channel FIFO Data Register (R/W)
};


#define AC97Outp32(offset, x) Outp32(AC97_BASE+offset, x)
#define AC97Outp16(offset, x) Outp16(AC97_BASE+offset, x)

typedef enum 
{
	PORT0, PORT1
}AC97_PORT;

typedef enum 
{
	CODEC_READY_INT, 
	PCMOUT_UNDERRUN_INT, 
	PCMIN_OVERRUN_INT, 
	MICIN_OVERRUN_INT, 
	PCMOUT_THRESHOLD_INT, 
	PCMIN_THRESHOLD_INT, 
	MICIN_THRESHOLD_INT 
}AC97_INT;

typedef enum 
{
	WRITE, 
	READ
}AC97_CMD;

typedef enum 
{
	PCM_OUT,
	PCM_IN, 
	MIC_IN
}AC97_CHANNEL;

typedef enum 
{
	OFF, 
	PIO, 
	DMA
}AC97_TRANSFER_MODE;

#if 0//For AC97 SFR W/R Test
REGINFO		sAc97RegInfo[] = 
{ 

	{"			", AC97_BASE+0x00, 32, RW, DPDB, 0, 0},   
	{"			", AC97_BASE+0x04, 32, RW, DPDB, 0, 0},
	{"			", AC97_BASE+0x08, 32, RW, DPDB, 0, 0},
	{"			", AC97_BASE+0x10, 32, RW, DPDB, 0, 0},   
	{"			", AC97_BASE+0x14, 32, RW, DPDB, 0, 0},
	{"			", AC97_BASE+0x18, 32, RW, DPDB, 0, 0},
	{"			", AC97_BASE+0x1C, 32, RW, DPDB, 0, 0},   

};
#endif

typedef struct
{
	//IP Operating Information
	AC97_INT			m_ePCMInIntMode;
	AC97_INT			m_ePCMOutIntMode;
	AC97_INT			m_eMICInIntMode;
	//Codec Operating Information
	s32					m_uOutputVolume;
	//CLK
	u32					m_uSamplingFreq;	
	//Address
	u32*				m_uPCMInStartAddr;
	u32*				m_uPCMOutStartAddr;	
	u16*				m_uMICInStartAddr;	
	u16*				m_uMICOutStartAddr;
	u32					m_uPcmDataSize;
	//DMA
	u32 					m_uNumDma;
	DMASELECT_eID 		m_eDmaId;
	DMA_UNIT 			m_eDmaUnit;
	DMA_CH 			m_eDmaCh;	
	DREQ_SRC 			m_eDreqSrc;		
}AC97_Infor;

typedef struct AC97testFuncMenu {
	void (*func)(AC97_Infor *); 
	const char *desc;
} AC97testFuncMenu;

u8 AC97_Init(void);

void AC97_SetPort(AUDIO_PORT ePort);
void AC97_ClosePort(AUDIO_PORT ePort);

AC97_Infor*	AC97_GetAC97Infor(AUDIO_PORT);
void AC97_SelectSamplingRate(AC97_Infor * rhs_pInfor);
void AC97_SelectIntrMode(AC97_Infor * rhs_pInfor);


void AC97_PCMInDMA(AC97_Infor * rhs_pInfor);
void AC97_PCMOutDMA(AC97_Infor * rhs_pInfor);
void AC97_MICInDMA(AC97_Infor * rhs_pInfor);
void AC97_MICOutDMA(AC97_Infor * rhs_pInfor);

void AC97_PCMInINT(AC97_Infor * rhs_pInfor);
void AC97_PCMOutINT(AC97_Infor * rhs_pInfor);
void AC97_MICInINT(AC97_Infor * rhs_pInfor);

u8 AC97_InitACLINK(void);
void AC97_SetTrasferCh(AC97_CHANNEL eCh, AC97_TRANSFER_MODE eTransferMode);
void AC97_WarmReset(void);
void AC97_ColdReset(void);

void AC97_SetTransferCh(AC97_CHANNEL eCh, AC97_TRANSFER_MODE eTransferMode);
void AC97_CodecInitPD(void);
u16 AC97_CodecCmd(AC97_CMD eCmd, u8 uCmdOffset, u16 uCmdData);
void AC97_CodecCmdManually(void);
void AC97_ControllerState(void);

void AC97_EnableInt(AC97_INT eInt);
void AC97_DisableInt(AC97_INT eInt);
void AC97_ClearInt(AC97_INT eInt);


void AC97_InitCodecPCMIn(u16 uAc97Fs);
void AC97_InitCodecPCMOut(u16 uAc97Fs);
void AC97_InitCodecMICIn(u16 uAc97Fs);


u8 AC97_SetOutputVolume(u8 uUpDownVolume);

void AC97_ExitCodecPCMOut(void);
void AC97_ExitCodecPCMIn(u16 uDACsOff);
void AC97_ExitCodecMICIn(u16 uDACsOff);

void __irq Isr_AC97_CodecReady(void);
void __irq Isr_AC97_PCMIn_DMADone(void);
void __irq Isr_AC97_PCMOut_DMADone(void);
void __irq Isr_AC97_MICIn_DMADone(void);
void __irq Isr_AC97_MICOut_DMADone(void);


void __irq Isr_AC97_PCMIn(void);
void __irq Isr_AC97_PCMOut(void);
void __irq Isr_AC97_MICIn(void);

#ifdef __cplusplus
}
#endif

#endif /*__AC97_H__*/

