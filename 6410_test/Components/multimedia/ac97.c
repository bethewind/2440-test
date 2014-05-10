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
*	Created Date : 2007/03/09
*	Version : 0.2
* 
*	History
*	- Version 0.1 (Y.H.Lee, 2007/03/09)
*	  -> Only available with STAC9767 AC97 Codec.
*   - Version 0.2 (Y.H.Lee, 2007/04/04)
*     -> Working with WM9713 AC97 Codec.
*
********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "def.h"
#include "option.h"
#include "library.h"
#include "sfr6410.h"
#include "system.h"
#include "intc.h"
#include "gpio.h"
#include "dma.h"
#include "sysc.h"
#include "timer.h"

#include "ac97.h"
#include "audio.h"

static DMAC oAc97Dma;

static volatile u8 g_uCodecReadyIrq;
static volatile u16 uAc97RecDone;
static volatile u16 uAc97PlayDone;
static volatile u16 g_usAC97Setup;

volatile u32 * uRecBuffer;
volatile u32 * uEndRecBuffer;

extern AC97_Infor 		g_AC97Infor;

u8 AC97_Init(void)
{
	AC97_Infor* p_ePort;
	p_ePort = AC97_GetAC97Infor(AUDIO_PORT0);
	
	//IP Operation 
	p_ePort->m_ePCMOutIntMode		= 	PCMOUT_THRESHOLD_INT;
	p_ePort->m_ePCMInIntMode		= 	PCMIN_THRESHOLD_INT;	
	p_ePort->m_eMICInIntMode		= 	MICIN_THRESHOLD_INT;
	//Sampling Frequency
	p_ePort->m_uSamplingFreq			=	48000;	
	//Recoding & Playing Buffer Address
	p_ePort->m_uPCMInStartAddr		=	(u32 *)0x5100002c;	
	p_ePort->m_uPCMOutStartAddr		=	(u32 *)0x5100002c;
	
	p_ePort->m_uMICInStartAddr		=	(u16 *)0x5100002c;
	p_ePort->m_uMICOutStartAddr		=	(u16 *)0x5100002c;
	p_ePort->m_uPcmDataSize			=	AC97_REC_LEN;	

	#ifdef AC97_NORMAL_DMA
	p_ePort->m_uNumDma			= 	NUM_DMA1;
	p_ePort->m_eDmaUnit				=	DMA1;
	#else
	p_ePort->m_uNumDma			= 	NUM_SDMA1;
	p_ePort->m_eDmaUnit				=	SDMA1;
	#endif 
	p_ePort->m_eDmaCh				=	DMA_A;
	g_AC97Infor.m_uOutputVolume	=	0x0101;
	
	return TRUE;
}

AC97_Infor* AC97_GetAC97Infor(AUDIO_PORT rhs_ePort)
{
	 return &(g_AC97Infor);
}

void AC97_SetPort(AUDIO_PORT ePort)
{
	if (ePort == AUDIO_PORT0)
	{
		//Set GPD: AC97 Port
		GPIO_SetFunctionAll(eGPIO_D, 0x44444, 0); 
		GPIO_SetPullUpDownAll(eGPIO_D, 0x0);	
	}					
	else if (ePort == AUDIO_PORT1)
	{
		//Set GPE: AC97 Port
		GPIO_SetFunctionAll(eGPIO_E, 0x44444, 0); 
		GPIO_SetPullUpDownAll(eGPIO_E, 0x0);	
	}
	Delay(1000);	
}

void AC97_ClosePort(AUDIO_PORT ePort)
{
	switch(ePort)
	{
		case AUDIO_PORT0 :
			GPIO_SetFunctionAll(eGPIO_D, 0x0, 0x0);
			break;
		case AUDIO_PORT1 :
			GPIO_SetFunctionAll(eGPIO_E, 0x0, 0x0);
			break;
		default :
			GPIO_SetFunctionAll(eGPIO_D, 0x0, 0x0);
			break;
	}
	Delay(1000);	
}

void AC97_SelectSamplingRate(AC97_Infor* rhs_pInfor)
{
	u32 i;

	UART_Printf("\nSelect ADC/DAC Rate\n");
	UART_Printf("0:8KHz, 1:22.05kHz, 2:44.1kHz, 3:48kHz\n");
	i = UART_GetIntNum();	

	if(i==0)
	{
		rhs_pInfor->m_uSamplingFreq = 8000;
	}
	else if(i==1)
	{
		rhs_pInfor->m_uSamplingFreq = 22100;
	}
	else if(i==2)
	{
		rhs_pInfor->m_uSamplingFreq = 44100;
	}
	else 
	{
		rhs_pInfor->m_uSamplingFreq = 48000;
	}	
}

void AC97_SelectIntrMode(AC97_Infor * rhs_pInfor)
{	
	u32 uSelTemp;
	UART_Printf("Select Channel\n");
	UART_Printf("0. PCM Out	1. PCM In	2.MIC In\n");
	uSelTemp = UART_GetIntNum();
	UART_Printf("Select Interrupt Mode\n");
	UART_Printf("0: Half[D]	1: Full\n");
	switch(uSelTemp)
	{
		case 0:
			uSelTemp = UART_GetIntNum();
			if (uSelTemp == 1)
				rhs_pInfor->m_ePCMOutIntMode = PCMOUT_UNDERRUN_INT;
			else
				rhs_pInfor->m_ePCMOutIntMode = PCMOUT_THRESHOLD_INT;
			break;
		case 1:
			uSelTemp = UART_GetIntNum();
			if (uSelTemp == 1)
				rhs_pInfor->m_ePCMInIntMode = PCMIN_OVERRUN_INT;
			else
				rhs_pInfor->m_ePCMInIntMode = PCMIN_THRESHOLD_INT;
			break;
		case 2:
		default:
			uSelTemp = UART_GetIntNum();
			if (uSelTemp == 1)
				rhs_pInfor->m_eMICInIntMode = MICIN_OVERRUN_INT;
			else
				rhs_pInfor->m_eMICInIntMode = MICIN_THRESHOLD_INT;
			break;
	}	
}


void AC97_PCMInINT(AC97_Infor* rhs_pInfor)
{
	uRecBuffer = rhs_pInfor->m_uPCMInStartAddr;
	uEndRecBuffer = (u32 *)( (u32) rhs_pInfor->m_uPCMInStartAddr + rhs_pInfor->m_uPcmDataSize);
    
	//Initial Interrupt Controller
	uAc97RecDone =0;
	INTC_ClearVectAddr();
	INTC_SetVectAddr(NUM_AC97, Isr_AC97_PCMIn);
	INTC_Enable(NUM_AC97);

	// Set AC97 Controller to generate IRQ
	
	AC97_ClearInt(rhs_pInfor->m_ePCMInIntMode);
	AC97_EnableInt(rhs_pInfor->m_ePCMInIntMode);

	
	
	// Transfer data enable  using AC-Link
	AC97_SetTransferCh(PCM_IN, PIO);	
	while(1)
	{
	      if(uAc97RecDone == 1)
			break;	
	}	
	
	AC97_SetTransferCh(PCM_IN, OFF);					//Disable Transfer PCM Out data on AC - Link
	
	AC97_DisableInt(rhs_pInfor->m_ePCMInIntMode);		// Set AC97 Controller to generate IRQ
	
	
	INTC_Disable(NUM_AC97);							// Disable Interrupt Controller 

	UART_Printf("\nEnd of Record!\n");	
}


void AC97_PCMOutINT(AC97_Infor* rhs_pInfor)
{
	uRecBuffer = rhs_pInfor->m_uPCMOutStartAddr;
	uEndRecBuffer = (u32 *)((u32)rhs_pInfor->m_uPCMOutStartAddr + rhs_pInfor->m_uPcmDataSize);
    
	//IRQ Initialization
	uAc97PlayDone =0;
	INTC_ClearVectAddr();
	INTC_SetVectAddr(NUM_AC97, Isr_AC97_PCMOut);
	INTC_Enable(NUM_AC97);	
	
	// Set AC97 Controller to generate IRQ
	
	AC97_ClearInt(rhs_pInfor->m_ePCMOutIntMode);
	AC97_EnableInt(rhs_pInfor->m_ePCMOutIntMode);
	
	// Transfer data enable  using AC-Link
	AC97_SetTransferCh(PCM_OUT, PIO);	
	
	while(1)
	{
	      if(uAc97PlayDone == 1)
			break;	
	}
	//Disable Transfer PCM Out data on AC - Link
	AC97_SetTransferCh(PCM_OUT, OFF);	
	// Set AC97 Controller to generate IRQ
	
	AC97_DisableInt(rhs_pInfor->m_ePCMOutIntMode);	
		
	// Disable Interrupt Controller
	INTC_Disable(NUM_AC97);
	UART_Printf("\nEnd of Play!\n");
}


void AC97_MICInINT(AC97_Infor* rhs_pInfor)
{
	uRecBuffer = (u32 *) rhs_pInfor->m_uMICInStartAddr;
	uEndRecBuffer = (u32 *)((u32)rhs_pInfor->m_uMICInStartAddr + rhs_pInfor->m_uPcmDataSize);

	//IRQ Initialization
	uAc97RecDone =0;
	INTC_ClearVectAddr();
	INTC_SetVectAddr(NUM_AC97, Isr_AC97_MICIn);
	INTC_Enable(NUM_AC97);

	// Set AC97 Controller to generate IRQ	
	AC97_ClearInt(rhs_pInfor->m_eMICInIntMode);
	AC97_EnableInt(rhs_pInfor->m_eMICInIntMode);
	
	// Transfer data enable  using AC-Link
	AC97_SetTransferCh(MIC_IN, PIO);	

	while(1)
	{
		if(uAc97RecDone == 1)
			break;	
	}
	// Set AC97 Controller to generate IRQ
	AC97_SetTransferCh(MIC_IN, OFF);
	
	AC97_DisableInt(rhs_pInfor->m_eMICInIntMode);
	
	// Disable Interrupt Controller
	INTC_Disable(NUM_AC97);
	UART_Printf("\nEnd of Record!\n");	

}

void AC97_PCMInDMA(AC97_Infor* rhs_pInfor)
{
	uAc97RecDone =0;		
	
	rhs_pInfor->m_eDmaId = eSEL_AC_PCMIN;
	rhs_pInfor->m_eDreqSrc = DMA1_AC_PCMin;
	
	//Set DMA
	INTC_SetVectAddr(rhs_pInfor->m_uNumDma, Isr_AC97_PCMIn_DMADone);
	INTC_Enable(rhs_pInfor->m_uNumDma);
	
	#ifdef AC97_NORMAL_DMA					// NORMAL DMA	
	SYSC_SelectDMA(rhs_pInfor->m_eDmaId, 1);	
	#else										// Secure DMA
	SYSC_SelectDMA(rhs_pInfor->m_eDmaId, 0);	
	#endif
	
	DMAC_InitCh(rhs_pInfor->m_eDmaUnit, rhs_pInfor->m_eDmaCh, &oAc97Dma);
	DMACH_ClearIntPending(&oAc97Dma);
	DMACH_ClearErrIntPending(&oAc97Dma);
	DMACH_Setup(rhs_pInfor->m_eDmaCh, false, (u32)(AC97_BASE+rACPCMDATA), true,
					(u32)rhs_pInfor->m_uPCMInStartAddr, false, WORD, (rhs_pInfor->m_uPcmDataSize)/4, HANDSHAKE, rhs_pInfor->m_eDreqSrc, MEM, SINGLE, &oAc97Dma);	
	
	DMACH_Start(&oAc97Dma);				// Start DMA	

	AC97_SetTransferCh(PCM_IN, DMA);		// Enable AC- Link to transfer PCM data 

	while(uAc97RecDone ==0)
	{
		UART_Printf(".");
		Delay(3000);
	}
	uAc97RecDone = 0;
	
	
	DMACH_Stop(&oAc97Dma);				// DMA stop
				
	INTC_Disable(rhs_pInfor->m_uNumDma);	
	
	AC97_SetTransferCh(PCM_IN, OFF);		//AC97 PCM In Channel Finish 

	UART_Printf("\nEnd of Record!\n");
}


void AC97_PCMOutDMA(AC97_Infor* rhs_pInfor)
{
	u32 uKeyinput;

	g_usAC97Setup = 0;

	rhs_pInfor->m_eDmaId = eSEL_AC_PCMOUT;
	rhs_pInfor->m_eDreqSrc = DMA1_AC_PCMout;
	
	INTC_SetVectAddr(rhs_pInfor->m_uNumDma, Isr_AC97_PCMOut_DMADone);
	INTC_Enable(rhs_pInfor->m_uNumDma);

	//Set DMA
	#ifdef AC97_NORMAL_DMA	
	SYSC_SelectDMA(rhs_pInfor->m_eDmaId, 1);	
	#else
	SYSC_SelectDMA(rhs_pInfor->m_eDmaId, 0);	
	#endif

	DMAC_InitCh(rhs_pInfor->m_eDmaUnit, rhs_pInfor->m_eDmaCh, &oAc97Dma);
	DMACH_ClearIntPending(&oAc97Dma);
	DMACH_ClearErrIntPending(&oAc97Dma);
	
	DMACH_Setup(rhs_pInfor->m_eDmaCh, 0x0, (u32)rhs_pInfor->m_uPCMOutStartAddr, false, 
					(u32)(AC97_BASE+rACPCMDATA), true, WORD, (rhs_pInfor->m_uPcmDataSize)/4, HANDSHAKE, MEM, rhs_pInfor->m_eDreqSrc, SINGLE, &oAc97Dma);

	// Start DMA				
	DMACH_Start(&oAc97Dma);

	// Enable to transfer PCM data on AC- Link 
	AC97_SetTransferCh(PCM_OUT, DMA);
	
	while(1)
	{
		uKeyinput = UART_Getc();
				
		if( (uKeyinput == 'x') | (uKeyinput == 'X')) 
			break;
		
		else if ((uKeyinput == 'd') | (uKeyinput == 'D') | (uKeyinput == 'u') | (uKeyinput == 'U'))
			AC97_SetOutputVolume(uKeyinput);
		
		else if ((uKeyinput == 's') | (uKeyinput == 'S'))
			AC97_CodecCmdManually();					
	} 
	
	//Disable to Transfer PCM Data on AC -Link
	AC97_SetTransferCh(PCM_OUT, OFF);
	
	// Stop DMA
	DMACH_Stop(&oAc97Dma);							
	INTC_Disable(rhs_pInfor->m_uNumDma);
	

	UART_Printf("\nEnd of Play!\n");	
}

void AC97_MICInDMA(AC97_Infor* rhs_pInfor)
{
	
	rhs_pInfor->m_eDmaId = eSEL_AC_MICIN;
	rhs_pInfor->m_eDreqSrc = DMA1_AC_MICin;
	

	INTC_SetVectAddr(rhs_pInfor->m_uNumDma, Isr_AC97_MICIn_DMADone);
	INTC_Enable(rhs_pInfor->m_uNumDma);

	// Set DMA
	#ifdef AC97_NORMAL_DMA	
	SYSC_SelectDMA(rhs_pInfor->m_eDmaId, 1);	
	#else	
	SYSC_SelectDMA(rhs_pInfor->m_eDmaId, 0);	
	#endif

	DMAC_InitCh(rhs_pInfor->m_eDmaUnit, rhs_pInfor->m_eDmaCh, &oAc97Dma);	
	DMACH_ClearIntPending(&oAc97Dma);
	DMACH_ClearErrIntPending(&oAc97Dma);
	DMACH_Setup(rhs_pInfor->m_eDmaCh, 0x0, (u32)(AC97_BASE+rACMICDATA), true, 
					(u32)rhs_pInfor->m_uMICInStartAddr, false, HWORD, (rhs_pInfor->m_uPcmDataSize)/4, HANDSHAKE, rhs_pInfor->m_eDreqSrc, MEM, SINGLE, &oAc97Dma);
	
	DMACH_Start(&oAc97Dma);						// Strat DMA
	
	AC97_SetTransferCh(MIC_IN, DMA);				// Enable to transfer MIC In data on AC- Link 
	while(uAc97RecDone ==0)
	{
		UART_Printf(".");
		Delay(3000);
	}
	
	AC97_SetTransferCh(MIC_IN, OFF);				// Disable to Transfer MIC In Data on AC -Link	
	DMACH_Stop(&oAc97Dma);						// Stop DMA
	INTC_Disable(rhs_pInfor->m_uNumDma);
	uAc97RecDone = 0;	
	UART_Printf("\nEnd of Record!\n");
}


void AC97_MICOutDMA(AC97_Infor* rhs_pInfor)
{
	u32 uKeyinput;	
	
	rhs_pInfor->m_eDmaId = eSEL_AC_PCMOUT;
	rhs_pInfor->m_eDreqSrc = DMA1_AC_PCMout;	
	INTC_SetVectAddr(rhs_pInfor->m_uNumDma, Isr_AC97_MICOut_DMADone);
	INTC_Enable(rhs_pInfor->m_uNumDma);
	// DMA Setting
	#ifdef AC97_NORMAL_DMA	
	SYSC_SelectDMA(rhs_pInfor->m_eDmaId, 1);
	#else
	SYSC_SelectDMA(rhs_pInfor->m_eDmaId, 0);
	#endif
	DMAC_InitCh(rhs_pInfor->m_eDmaUnit, rhs_pInfor->m_eDmaCh, &oAc97Dma);
	
	DMACH_ClearIntPending(&oAc97Dma);
	DMACH_ClearErrIntPending(&oAc97Dma);
	DMACH_Setup(rhs_pInfor->m_eDmaCh, 0x0, (u32)rhs_pInfor->m_uMICOutStartAddr, false, 
					(u32)(AC97_BASE+rACPCMDATA), true, HWORD, (rhs_pInfor->m_uPcmDataSize)/4, HANDSHAKE, MEM, rhs_pInfor->m_eDreqSrc, SINGLE, &oAc97Dma);

	// Strat DMA
	DMACH_Start(&oAc97Dma);

	// Transfer data enable  using AC-Link
	AC97_SetTransferCh(PCM_OUT, DMA);
	
	while(1)
	{
		uKeyinput = UART_Getc();

		if( (uKeyinput == 'x') | (uKeyinput == 'X')) 
			break;
		
		else if  ((uKeyinput == 'u') | (uKeyinput == 'U') | (uKeyinput == 'd') | (uKeyinput == 'D') )
			AC97_SetOutputVolume(uKeyinput);
		
		else if ((uKeyinput == 's') | (uKeyinput == 'S'))
			AC97_CodecCmdManually();
		
	}     
	
	// Stop DMA
	DMACH_Stop(&oAc97Dma);		
	INTC_Disable(rhs_pInfor->m_uNumDma);	

	//Disable Transfer PCM Out data on AC - Link
	AC97_SetTransferCh(PCM_OUT, OFF);

	UART_Printf("\nEnd of Play!\n");	
}





//Interrupt Service Routine
void __irq Isr_AC97_CodecReady(void)
{ 
	u32 uState;
	
	uState= Inp32(AC97_BASE+rACGLBSTAT); 
	INTC_Disable(NUM_AC97);
	AC97_DisableInt(CODEC_READY_INT);		//Sholud be located after uState= Inp32(AC97_BASE+rACGLBSTAT); 	
	
	if ( (uState & AC97_EnableInt_CodecReady))
	{
		g_uCodecReadyIrq = 1;
		UART_Printf("Codec Ready!\n");	
	}
	
	AC97_ClearInt(CODEC_READY_INT);
	INTC_ClearVectAddr();
}

void __irq Isr_AC97_PCMIn(void)
{
	u32 uGlbStatus, i, urACPCMADDR; 
	
	uGlbStatus= Inp32(AC97_BASE+rACGLBSTAT);
	urACPCMADDR = Inp32(AC97_BASE+rACPCMADDR);
	INTC_Disable(NUM_AC97);
	AC97_DisableInt(g_AC97Infor.m_ePCMInIntMode);	
	
	if (uGlbStatus & AC97_EnableInt_ThresholdPCMIn)
	{
		for(i=0; i<PCM_IN_TRIGGER; i++)
		{
			
			*(uRecBuffer++) = Inp32(AC97_BASE+rACPCMDATA);			

			if(uRecBuffer >= uEndRecBuffer) 			
				break;	
			
		}			
	}

	else if ( uGlbStatus & AC97_EnableInt_OverrunPCMIn)
	{
		for(i=0; i<16; i++)
		{
			*(uRecBuffer++) = Inp32(AC97_BASE+rACPCMDATA);			

			if(uRecBuffer >= uEndRecBuffer)  
				break;	
		}			
	}
	
	INTC_ClearVectAddr();
	AC97_ClearInt(g_AC97Infor.m_ePCMInIntMode);
	
	if(uRecBuffer >= uEndRecBuffer) 
	{
		UART_Printf("0x%8x\n",urACPCMADDR);
		uAc97RecDone =1;	
	}
	
	else
	{	
		AC97_EnableInt(g_AC97Infor.m_ePCMInIntMode);
		INTC_Enable(NUM_AC97);
	}	
}

void __irq Isr_AC97_PCMOut(void)
{
	u32 uGlbStatus, i, urACPCMADDR; 
	uGlbStatus= Inp32(AC97_BASE+rACGLBSTAT);
	urACPCMADDR = Inp32(AC97_BASE+rACPCMADDR);
	INTC_Disable(NUM_AC97);
	AC97_DisableInt(g_AC97Infor.m_ePCMOutIntMode);	
	
	if (uGlbStatus & AC97_EnableInt_ThresholdPCMOut)
	{
		for(i=0; i<PCM_OUT_TRIGGER; i++)
		{	
			AC97Outp32(rACPCMDATA, *(uRecBuffer++));	

			if(uRecBuffer >= uEndRecBuffer)  			
				
				break;	
			
		}		
	}
	else if (uGlbStatus & AC97_EnableInt_UnderrunPCMOut)
	{
		for(i=0; i<16; i++)
		{	
			AC97Outp32(rACPCMDATA, *(uRecBuffer++));	

			if(uRecBuffer >= uEndRecBuffer)  
				break;	
		}		
	}
	AC97_ClearInt(g_AC97Infor.m_ePCMOutIntMode);
	INTC_ClearVectAddr();
	
	if(uRecBuffer >= uEndRecBuffer) 
	{
		UART_Printf("0x%8x\n",urACPCMADDR);
		uAc97PlayDone =1;
	}
	else
	{
		AC97_EnableInt(g_AC97Infor.m_ePCMOutIntMode);
		INTC_Enable(NUM_AC97);
	}
}

void __irq Isr_AC97_MICIn(void)
{
	u32 uGlbStatus, i,uTemp; 
	u32 uTrigerLevel;
	
	uGlbStatus= Inp32(AC97_BASE+rACGLBSTAT);			//When Disable Interrupt of IP, it is cleared.
	INTC_Disable(NUM_AC97);
	AC97_DisableInt(g_AC97Infor.m_eMICInIntMode);
	
	if (uGlbStatus & AC97_EnableInt_ThresholdMICIn)
		uTrigerLevel = PCM_IN_TRIGGER;
		
	else if (uGlbStatus & AC97_EnableInt_OverrunMICIn)
		uTrigerLevel = 16;	
	
	for(i=0; i<uTrigerLevel; i++)
	{
		uTemp = Inp32(AC97_BASE+rACMICDATA);
		uTemp = (uTemp << 16) | uTemp; 
		*(uRecBuffer++) = uTemp;				

		if(uRecBuffer >= uEndRecBuffer)  
			break;	
	}	
	AC97_ClearInt(g_AC97Infor.m_eMICInIntMode);
	INTC_ClearVectAddr();

	if(uRecBuffer >= uEndRecBuffer) 
		uAc97RecDone =1;
	else
	{
		UART_Printf(".");
		AC97_EnableInt(g_AC97Infor.m_eMICInIntMode);
		INTC_Enable(NUM_AC97);
	}
}

void __irq Isr_AC97_PCMIn_DMADone(void)
{
	AC97_Infor* p_Infor = AC97_GetAC97Infor(AUDIO_PORT0);
	
	INTC_Disable(p_Infor->m_uNumDma);
	UART_Printf("\nPCM In DMA Done.\n");
	uAc97RecDone = 1;

	DMACH_ClearIntPending(&oAc97Dma);
	DMACH_ClearErrIntPending(&oAc97Dma);
	INTC_ClearVectAddr();	
}

void __irq Isr_AC97_PCMOut_DMADone(void)
{
	AC97_Infor* p_Infor = AC97_GetAC97Infor(AUDIO_PORT0);
	
	INTC_Disable(p_Infor->m_uNumDma);

	if (g_usAC97Setup == 0)
		UART_Printf("\n~~~\n");
	DMAC_InitCh(p_Infor->m_eDmaUnit, p_Infor->m_eDmaCh, &oAc97Dma);
	
	DMACH_ClearIntPending(&oAc97Dma);
	DMACH_ClearErrIntPending(&oAc97Dma);
	INTC_ClearVectAddr();	

	INTC_Enable(p_Infor->m_uNumDma);

	DMACH_Setup(p_Infor->m_eDmaCh, 0x0, (u32)g_AC97Infor.m_uPCMOutStartAddr, false, 
					(u32)(AC97_BASE+rACPCMDATA), true, WORD, (g_AC97Infor.m_uPcmDataSize)/4, HANDSHAKE, MEM, p_Infor->m_eDreqSrc, SINGLE, &oAc97Dma);
	DMACH_Start(&oAc97Dma);
}

void __irq Isr_AC97_MICIn_DMADone(void)
{
	AC97_Infor* p_Infor = AC97_GetAC97Infor(AUDIO_PORT0);
	
	INTC_Disable(p_Infor->m_uNumDma);

	UART_Printf("\nMIC In DMA Done.\n");
	uAc97RecDone = 1;

	DMACH_ClearIntPending(&oAc97Dma);
	DMACH_ClearErrIntPending(&oAc97Dma);
	INTC_ClearVectAddr();	
}
void __irq Isr_AC97_MICOut_DMADone(void)
{
	AC97_Infor* p_Infor = AC97_GetAC97Infor(AUDIO_PORT0);
	
	INTC_Disable(p_Infor->m_uNumDma);

	if (g_usAC97Setup == 0)
		UART_Printf("\n~~~\n");
	DMAC_InitCh(p_Infor->m_eDmaUnit, p_Infor->m_eDmaCh, &oAc97Dma);
	
	DMACH_ClearIntPending(&oAc97Dma);
	DMACH_ClearErrIntPending(&oAc97Dma);
	INTC_ClearVectAddr();	

	INTC_Enable(p_Infor->m_uNumDma);

	DMACH_Setup(p_Infor->m_eDmaCh, 0x0, (u32)g_AC97Infor.m_uMICOutStartAddr, false, 
					(u32)(AC97_BASE+rACPCMDATA), true, HWORD, (g_AC97Infor.m_uPcmDataSize)/4, HANDSHAKE, MEM, p_Infor->m_eDreqSrc, SINGLE, &oAc97Dma);
	DMACH_Start(&oAc97Dma);
}

void AC97_SetTransferCh(AC97_CHANNEL eCh, AC97_TRANSFER_MODE eTransferMode)
{
	u32 uEnTransferCh;
    
   	uEnTransferCh= Inp32(AC97_BASE+rACGLBCTRL);

	if(eCh == PCM_OUT)
	{
		uEnTransferCh =
			(eTransferMode == PIO) ? (uEnTransferCh |(1<<12)) :
			(eTransferMode == DMA) ? (uEnTransferCh |(2<<12)) : (uEnTransferCh & ~(3<<12));
	}
	else if(eCh == PCM_IN)	
	{
		uEnTransferCh =
			(eTransferMode == PIO) ? (uEnTransferCh |(1<<10)) :
			(eTransferMode == DMA) ? (uEnTransferCh |(2<<10)) : (uEnTransferCh & ~(3<<10));
	}
	else if(eCh == MIC_IN)	
	{
		uEnTransferCh =
			(eTransferMode == PIO) ? (uEnTransferCh |(1<<8)) :
			(eTransferMode == DMA) ? (uEnTransferCh |(2<<8)) : (uEnTransferCh & ~(3<<8));
	}

	uEnTransferCh |= (3<<2); 
		
	AC97Outp32(rACGLBCTRL, uEnTransferCh); 
	Delay(10);	

	//AC97_ControllerState();
}

void AC97_ClearInt(AC97_INT eInt)
{
	u32 uInt;
    
    uInt= Inp32(AC97_BASE+rACGLBCTRL);

    if(eInt == CODEC_READY_INT)
		AC97Outp32(rACGLBCTRL, uInt | AC97_ClearInt_CodecReady);
	else if (eInt == PCMOUT_UNDERRUN_INT)
		AC97Outp32(rACGLBCTRL, uInt | AC97_ClearInt_UnderrunPCMOut);
	else if (eInt == PCMIN_OVERRUN_INT)
		AC97Outp32(rACGLBCTRL, uInt | AC97_ClearInt_OverrunPCMIn);
	else if (eInt == MICIN_OVERRUN_INT)	
		AC97Outp32(rACGLBCTRL, uInt | AC97_ClearInt_OverrunMICIn);
	else if (eInt == PCMOUT_THRESHOLD_INT)	
		AC97Outp32(rACGLBCTRL, uInt | AC97_ClearInt_ThresholdPCMOut);
	else if (eInt == PCMIN_THRESHOLD_INT)	
		AC97Outp32(rACGLBCTRL, uInt | AC97_ClearInt_ThresholdPCMIn);
	else if (eInt == MICIN_THRESHOLD_INT)
		AC97Outp32(rACGLBCTRL, uInt | AC97_ClearInt_ThresholdMICIn);
}

void AC97_EnableInt(AC97_INT eInt)
{
	u32 uInt;
    
    uInt= Inp32(AC97_BASE+rACGLBCTRL);

	if(eInt == CODEC_READY_INT)
		uInt |= AC97_EnableInt_CodecReady;

	else if (eInt == PCMOUT_UNDERRUN_INT)
		uInt |= AC97_EnableInt_UnderrunPCMOut;

	else if (eInt == PCMIN_OVERRUN_INT)
		uInt |= AC97_EnableInt_OverrunPCMIn;

	else if (eInt == MICIN_OVERRUN_INT)	
		uInt |= AC97_EnableInt_OverrunMICIn;

	else if (eInt == PCMOUT_THRESHOLD_INT)	
		uInt |= AC97_EnableInt_ThresholdPCMOut;

	else if (eInt == PCMIN_THRESHOLD_INT)	
		uInt |= AC97_EnableInt_ThresholdPCMIn;	

	else if (eInt == MICIN_THRESHOLD_INT)
		uInt |= AC97_EnableInt_ThresholdMICIn;

	AC97Outp32(rACGLBCTRL, uInt);
	
}

void AC97_DisableInt(AC97_INT eInt)
{
	u32 uInt;
    
    uInt= Inp32(AC97_BASE+rACGLBCTRL);

    if(eInt == CODEC_READY_INT)
		AC97Outp32(rACGLBCTRL, uInt & ~AC97_EnableInt_CodecReady);
	else if (eInt == PCMOUT_UNDERRUN_INT)
		AC97Outp32(rACGLBCTRL, uInt & ~AC97_EnableInt_UnderrunPCMOut);
	else if (eInt == PCMIN_OVERRUN_INT)
		AC97Outp32(rACGLBCTRL, uInt & ~AC97_EnableInt_OverrunPCMIn);
	else if (eInt == MICIN_OVERRUN_INT)	
		AC97Outp32(rACGLBCTRL, uInt & ~AC97_EnableInt_OverrunPCMIn);
	else if (eInt == PCMOUT_THRESHOLD_INT)	
		AC97Outp32(rACGLBCTRL, uInt & ~AC97_EnableInt_ThresholdPCMOut);
	else if (eInt == PCMIN_THRESHOLD_INT)	
		AC97Outp32(rACGLBCTRL, uInt & ~AC97_EnableInt_ThresholdPCMOut);
	else if (eInt == MICIN_THRESHOLD_INT)
		AC97Outp32(rACGLBCTRL, uInt & ~AC97_EnableInt_ThresholdPCMOut);
}



u8 AC97_InitACLINK(void)
{
	u32 i=0;

	UART_Printf("\nAC97 Initialization...\n");	       
    //Codec Ready Check using Codec Ready Interrupt
    //Cold Reset 
	AC97_ColdReset();

	g_uCodecReadyIrq =0;	

	INTC_SetVectAddr(NUM_AC97, Isr_AC97_CodecReady);
	INTC_Enable(NUM_AC97);
	AC97_EnableInt(CODEC_READY_INT);

	while(!g_uCodecReadyIrq)
	{
	 	UART_Printf(".");
		Delay(3000);
      	i++;
         	
       	if(i==20)
			break;
	}	
	UART_Printf("\n");
	if(i>=20)
	{
		UART_Printf("\nAC97 codec is not ready.");
		UART_Printf("\nCheck on connection between AP and AC97 CODEC.\n");
		UART_Printf("\nBye. ");		
		return false;
	}
	else
	{
		return true;
	}
}



void AC97_WarmReset(void)
{
	u32 uGlobalCon;
	
	UART_Printf("\n=>Warm Reset\n");
	AC97_CodecCmd(READ,0x26,0x0); 						//To avoid issuing unwanted command to Codec after warm reset 

	uGlobalCon = Inp32(AC97_BASE+rACGLBCTRL) & ~(0xf);		//except Transfer Data, AC-Link On, Warm Reset and cold Reset bits

	uGlobalCon |= AC97_Enable_Warm_Reset;					//Wakeup Reset  Bit Set
	AC97Outp32(rACGLBCTRL, uGlobalCon);
	Delay(1000);												//wait 1us for warm reset
	AC97_ControllerState();
	
	uGlobalCon &= ~AC97_Enable_Warm_Reset;					// Wakeup Reset Bit Clear
	AC97Outp32(rACGLBCTRL, uGlobalCon);
	AC97_ControllerState();
	Delay(AC97_DebugDelay);

	uGlobalCon |= AC97_Enable_ACLINK;						// AC - Link On	
	AC97Outp32(rACGLBCTRL, uGlobalCon);
	AC97_ControllerState();
	Delay(AC97_DebugDelay);
	
	uGlobalCon |= AC97_Enable_Transfer_ACLINK;				//  Data Transfer Enable
	AC97Outp32(rACGLBCTRL, uGlobalCon);
	Delay(AC97_DebugDelay);
	AC97_ControllerState();
	
	//AC97_CodecCmd(WRITE,0x00,0x683F);		
	UART_Printf("AC97 Codec Powerdown Ctrl/Stat Reg. Value (at 0x26): 0x%x\n", AC97_CodecCmd(READ,0x26,0x0000));	
}

void AC97_ColdReset(void)
{
	u32 uGlobalCon;
		
	UART_Printf("\n=>Cold Reset\n");
	//uGlobalCon = Inp32(AC97_BASE+rACGLBCTRL);

	uGlobalCon = AC97_Enable_Cold_Reset;			//Cold Reset : Assert RESET# Pin
	AC97Outp32(rACGLBCTRL, uGlobalCon);
	Delay(AC97_DebugDelay);	
	AC97_ControllerState();
	
	uGlobalCon &= ~AC97_Enable_Cold_Reset;		//Cold Reset : De-Assert RESET# Pin
	AC97Outp32(rACGLBCTRL, uGlobalCon);
	Delay(AC97_DebugDelay);
	AC97_ControllerState();	

	#if (AC97_CODEC== WM9713)
		AC97_WarmReset();
	#endif
	
	AC97_ControllerState();	
	uGlobalCon |= (1<<2);
	AC97Outp32(rACGLBCTRL, uGlobalCon);
	AC97_ControllerState();
	Delay(AC97_DebugDelay);
	
	uGlobalCon |= (1<<3);
	AC97Outp32(rACGLBCTRL, uGlobalCon);
	AC97_ControllerState();
	
	//UART_Printf("AC97 Codec Powerdown Ctrl/Stat Reg. Value (at 0x26): 0x%x\n", AC97_CodecCmd(READ,0x26,0x0000));	

}

void AC97_CodecInitPD(void)
{
	#if (AC97_CODEC == STAC9767)	
	UART_Printf("\nSoft Reset\n");
	AC97_CodecCmd(WRITE,0x00,0x683F);		//Codec Soft Reset : 16bit In/Out 	
	UART_Printf("AC97 Codec 0x26 Reg.: 0x%x\n\n", AC97_CodecCmd(READ,0x26,0x0000));
	#endif	
}

u16 AC97_CodecCmd(AC97_CMD eCmd, u8 uCmdOffset, u16 uCmdData)
{
	u32 uState;
	u16 uCodecStat;	

	if(eCmd == WRITE)
	{
		AC97Outp32(rACCODECCMD, (0<<23) |(uCmdOffset << 16) |(uCmdData << 0));
		Delay(AC97_CMDWriteDelay);														//Delay more than 1/48Khz(20.83us)
		return 0;
	}
	else if (eCmd == READ) 
	{
		AC97Outp32(rACCODECCMD, (1<<23) |(uCmdOffset << 16) |(uCmdData << 0));
		Delay(AC97_CMDReadDelay);														//Delay more than 1/48Khz(20.83us)
		
		uState= Inp32(AC97_BASE+rACCODECSTAT);
		uCodecStat = (u16)(uState & 0xFFFF);
		return uCodecStat;
	}
	else
		return 0;
}

void AC97_CodecCmdManually(void)
{
	AC97_CMD	eCmd;
	u16	uCmdData;
	u8	uCmdOffset;
	u32 uTemp;
	
	g_usAC97Setup = 1;
	UART_Printf("\n0. Write	1. Read\n");
	uTemp = UART_GetIntNum();
	if (uTemp == 0)
		eCmd = WRITE;
	else 
		eCmd = READ;
	
	UART_Printf("\nInput Codec Register(HEXA)\n");
	uCmdOffset = UART_GetIntNum();
	
	UART_Printf("\nInput Cmd(HEXA)\n");
	uCmdData = UART_GetIntNum();

	uTemp = AC97_CodecCmd(eCmd, uCmdOffset, uCmdData);

	if (eCmd == READ)
		UART_Printf("Read Register : 0x%x,	Value : 0x%x\n",uCmdOffset,uTemp);
	else
		UART_Printf("Write Register : 0x%x,	Value : 0x%x\n",uCmdOffset,AC97_CodecCmd(READ, uCmdOffset, 0x0000));

	g_usAC97Setup = 0;
}

void AC97_ControllerState(void)
{
	u32 uState;

	uState= Inp32(AC97_BASE+rACGLBSTAT);

	if((uState & 0x7) == 0)
		UART_Printf("AC97 Controller State: Idle\n"); 
	else if ((uState & 0x7) == 1)
		UART_Printf("AC97 Controller State: Init\n"); 
	else if ((uState & 0x7) == 2)
		UART_Printf("AC97 Controller State: Ready\n"); 
	else if ((uState & 0x7) == 3)	
		UART_Printf("AC97 Controller State: Active\n"); 
	else if ((uState & 0x7) == 4)	
		UART_Printf("AC97 Controller State: LP\n"); 
	else if ((uState & 0x7) == 5)	
		UART_Printf("AC97 Controller State: Warm\n"); 
}

void AC97_InitCodecPCMIn( u16 uAc97Fs)
{
	//u32 uAc97cmd;
	
	if (AC97_CODEC== WM9713)
	{

	 	AC97_CodecCmd(WRITE,0x26, 0x4f00);		// Enable PR5(Internal Clock, AC-link I/F)
		AC97_CodecCmd(WRITE,0x26, 0x4700);		// Enable PR3(VREF, I/P PGA's, DAC's, ADC's, Mixer, O/P's)

	 	//UART_Printf("AC97 Codec 0x26 Reg.: 0x%x\n\n", AC97_CodecCmd(READ,0x26,0x0000));
		AC97_CodecCmd(WRITE,0x3c, 0xfbff);			// Enable MBIAS generator
		Delay(1000);
		
		AC97_CodecCmd(WRITE,0x26, 0x4300);		// Enable PR2(I/P PGA's and mixers)
		AC97_CodecCmd(WRITE,0x3C, 0xfbc3);		// Enable ADC L/R					//fbf3
		AC97_CodecCmd(WRITE,0x26, 0x4200);		// Enable Stereo ADC 				//4100
		AC97_CodecCmd(WRITE,0x26, 0x0200);		// Enable PR6 (O/P PGA's)			3c fb33
		AC97_CodecCmd(WRITE,0x3E, 0xff9f);			// Enable LINE L/R PGA				26 0100
																				//3e f9ff	
		
		////////////////////////////////////////////////////////
		// codec setting to hear sound while PCM encoding 
		//uAc97cmd = AC97_CodecCmd(READ,0x14, 0x0000);
		//uAc97cmd =  uAc97cmd &  ~(3 << 14) & (~ (5 << 11));
		//AC97_CodecCmd(WRITE,0x14, 0x1612);
		//AC97_CodecCmd(WRITE,0x1c, 0x00a0);
		//AC97_CodecCmd(WRITE,0x04,0x0707);
		//UART_Printf("%x\n",AC97_CodecCmd(READ,0x14, 0x0000));	
		//UART_Printf("%x\n",AC97_CodecCmd(READ,0x0a, 0x0000));
		////////////////////////////////////////////////////////

		AC97_CodecCmd(WRITE,0x2A,0x1);		//Variable Rate Enable	
		UART_Printf("VRA Enable(1)/Disable(0): 0x%x\n",(0x1&AC97_CodecCmd(READ,0x2A,0x0001)));

		if(uAc97Fs==8000)
		{
			//ADC Sampling frequency 8kHz
			AC97_CodecCmd(WRITE,0x32,0x1f40);	
		}			
		
		else if(uAc97Fs==44100)
		{
			//ADC Sampling frequency 44.1kHz
			AC97_CodecCmd(WRITE,0x32,0xac44);
		}
		else if(uAc97Fs==22050)
		{
			//ADC Sampling frequency 22.05kHz
			AC97_CodecCmd(WRITE,0x32,0x5622);	 	
		}
		else if(uAc97Fs==48000)
		{
			//ADC Sampling frequency 48kHz
			AC97_CodecCmd(WRITE,0x32,0xbb80);
		}
		UART_Printf("\nAC97 Codec 0x32 Reg.: 0x%x\n\n", AC97_CodecCmd(READ,0x32,0x0000));

		AC97_CodecCmd(WRITE,0x14, 0xfe12);		// Record Mux Source Selection: LINE L/R
		AC97_CodecCmd(WRITE,0x1c, 0x00a0);
		AC97_CodecCmd(WRITE,0x04,0x0303);
		AC97_CodecCmd(WRITE,0x12, 0x1010);		// Unmute ADC and Set ADC Recoding Volume
	}
	
	
}


void AC97_InitCodecPCMOut(u16 uAc97Fs)
{	
	if (AC97_CODEC== STAC9767)
	{
		AC97_CodecCmd(WRITE,0x00,0x683F);		//codec soft reset 	
		
		AC97_CodecCmd(WRITE,0x2A,0x0001);	//variable rate enable	
		//UART_Printf("\nVRA Enable(1)/Disable(0): 0x%x\n", (0x1&AC97_CodecCmd(READ,0x2A,0x0001)));

		if(uAc97Fs==8000){
		//DAC Sampling frequency 8kHz
		AC97_CodecCmd(WRITE,0x2C,0x1f40);
		}
		else if(uAc97Fs==48000){
		//DAC Sampling frequency 48kHz
		AC97_CodecCmd(WRITE,0x2C,0xbb80);
		}
		else if(uAc97Fs==44100){
		//DAC Sampling frequency 44.1kHz
		AC97_CodecCmd(WRITE,0x2C,0xac44);
		}
		else if(uAc97Fs==22050){
		//DAC Sampling frequency 22.05kHz
		AC97_CodecCmd(WRITE,0x2C,0x5622);		
		}

		AC97_CodecCmd(WRITE,0x26, (1<<8));	// all power on except ADC blcok
		UART_Printf("AC97 Codec 0x26 Reg.: 0x%x\n\n", AC97_CodecCmd(READ,0x26,0x0000));

		AC97_CodecCmd(WRITE,0x18,0x0000);		// PCM out volume on
		AC97_CodecCmd(WRITE,0x20,0x0000);		// general purpose
		AC97_CodecCmd(WRITE,0x04,0x1A1A);	// Aux out(HP out) volume on
		
		g_AC97Infor.m_uOutputVolume = AC97_CodecCmd(READ,0x04,0x00000);	//HP out volume 
	}
	else if (AC97_CODEC== WM9713)	
	{
	 	AC97_CodecCmd(WRITE,0x26, 0x4f00);	// Enable PR5(Internal Clock, AC-link I/F)
		AC97_CodecCmd(WRITE,0x26, 0x4700);	// Enable PR3(VREF, I/P PGA's, DAC's, ADC's, Mixer, O/P's)

	 	//UART_Printf("AC97 Codec 0x26 Reg.: 0x%x\n\n", AC97_CodecCmd(READ,0x26,0x0000));
		AC97_CodecCmd(WRITE,0x3c, 0xfbff);		// Enable MBIAS generator
		Delay(1000);

		AC97_CodecCmd(WRITE,0x26, 0x4300);	// Enable PR2(I/P PGA's and mixers)
		AC97_CodecCmd(WRITE,0x3C, 0xfbf3);		// Enable HPL/R Mixer
		AC97_CodecCmd(WRITE,0x26, 0x4100);	// Enable PR1(Stereo DAC)
		AC97_CodecCmd(WRITE,0x3C, 0xfb33);	// Enable DAC L/R
		AC97_CodecCmd(WRITE,0x26, 0x0100);	// Enable PR6 (O/P PGA's)
		AC97_CodecCmd(WRITE,0x3E, 0xf9ff);		// Enable PR6 (O/P PGA's)

		AC97_CodecCmd(WRITE,0x2A,0x1);		//Variable Rate Enable	
		UART_Printf("VRA Enable(1)/Disable(0): 0x%x\n",(0x1&AC97_CodecCmd(READ,0x2A,0x0001)));

		if(uAc97Fs==8000){
		//DAC Sampling frequency 8kHz
		AC97_CodecCmd(WRITE,0x2C,0x1f40);
		}
		else if(uAc97Fs==48000){
		//DAC Sampling frequency 48kHz
		AC97_CodecCmd(WRITE,0x2C,0xbb80);
		}
		else if(uAc97Fs==44100){
		//DAC Sampling frequency 44.1kHz
		AC97_CodecCmd(WRITE,0x2C,0xac44);
		}
		else if(uAc97Fs==22050){
		//DAC Sampling frequency 22.05kHz
		AC97_CodecCmd(WRITE,0x2C,0x5622);		
		}
		UART_Printf("\nAC97 Codec 0x2C Reg.: 0x%x\n\n", AC97_CodecCmd(READ,0x2C,0x0000));

		AC97_CodecCmd(WRITE,0x12,0x8000);		//Disable ADC because it makes noise when ADC is turn on
		AC97_CodecCmd(WRITE,0x1c, 0x00a0);	// HPL/R PGA input select HPMIXL/R
		AC97_CodecCmd(WRITE,0x04,0x0707);		// Set HPL/R Volume 
		g_AC97Infor.m_uOutputVolume = AC97_CodecCmd(READ,0x04,0x00000);	//HP out volume 
		AC97_CodecCmd(WRITE,0x0c,0x6808);		// Unmute DAC to HP mixer path
		AC97_CodecCmd(WRITE,0x04,0x0A0A);	// Unmute HPL/R
		
	}
}

void AC97_InitCodecMICIn(u16 uAc97Fs)
{
	if (AC97_CODEC== STAC9767)
	{
		AC97_CodecCmd(WRITE,0x00,0x683F);		//codec soft reset 	
		
		AC97_CodecCmd(WRITE,0x2A,0x0001);		//variable rate enable	
		UART_Printf("VRA Enable(1)/Disable(0): 0x%x\n",(0x1&AC97_CodecCmd(READ,0x2A,0x0001)));

		if(uAc97Fs==8000){
		//ADC Sampling frequency 8kHz
		AC97_CodecCmd(WRITE,0x32,0x1f40);	
		}
		if(uAc97Fs==48000){
		//ADC Sampling frequency 48kHz
		AC97_CodecCmd(WRITE,0x32,0xbb80);	
		}
		else if(uAc97Fs==44100){
		//ADC Sampling frequency 44.1kHz
		AC97_CodecCmd(WRITE,0x32,0xac44);
		}
		else if(uAc97Fs==22050){
		//ADC Sampling frequency 22.05kHz
		AC97_CodecCmd(WRITE,0x32,0x5622);	 	
		}
		
		AC97_CodecCmd(WRITE,0x26,(1<<9));		//all power on except DAC Block
		UART_Printf("\nAC97 Codec 0x26 Reg.: 0x%x\n\n", AC97_CodecCmd(READ,0x26,0x0000));
		
		AC97_CodecCmd(WRITE,0x20,0x0000);		//MIC1 Selected
		AC97_CodecCmd(WRITE,0x6e,0x0024);		//ADC Input Slot => left slot6, right slot9, MIC GAIN VAL =1 
		AC97_CodecCmd(WRITE,0x0e,0x0040);		//BOOSTEN =1
		AC97_CodecCmd(WRITE,0x1a,0x0000);		//Left, Right => MIC
		AC97_CodecCmd(WRITE,0x1c,0xff);		
		AC97_CodecCmd(WRITE,0x78,0x0001);		//ADC HPF Bypass
		
		//AC97_CodecCmd(READ,0x1c,0x0000);	    //Record Volume
	}
	else if (AC97_CODEC== WM9713)
	{

	 	AC97_CodecCmd(WRITE,0x26, 0x4f00);		// Enable PR5(Internal Clock, AC-link I/F)
		AC97_CodecCmd(WRITE,0x26, 0x4700);		// Enable PR3(VREF, I/P PGA's, DAC's, ADC's, Mixer, O/P's)
		AC97_CodecCmd(WRITE,0x3c, 0xfbff);			// Enable MBIAS generator
		Delay(1000);		
		AC97_CodecCmd(WRITE,0x26, 0x4300);		// Enable PR2(I/P PGA's and mixers)
		AC97_CodecCmd(WRITE,0x3C, 0xfbcf);			// Enable ADC L/R
		AC97_CodecCmd(WRITE,0x26, 0x4200);		// Enable Stereo ADC 
		AC97_CodecCmd(WRITE,0x26, 0x0200);		// Enable PR6 (O/P PGA's)

		AC97_CodecCmd(WRITE,0x3E, 0xff9f);			// Enable LINE L/R PGA
		//AC97_CodecCmd(WRITE,0x3E, 0xfff0);		// Enable MIC L/R PGA			
		
		AC97_CodecCmd(WRITE,0x2A,0x1);		//Variable Rate Enable	
		UART_Printf("VRA Enable(1)/Disable(0): 0x%x\n",(0x1&AC97_CodecCmd(READ,0x2A,0x0001)));

		if(uAc97Fs==8000){
		//ADC Sampling frequency 8kHz
		AC97_CodecCmd(WRITE,0x32,0x1f40);	
		}
		else if(uAc97Fs==48000){
		//ADC Sampling frequency 48kHz
		AC97_CodecCmd(WRITE,0x32,0xbb80);	
		}
		else if(uAc97Fs==44100){
		//ADC Sampling frequency 44.1kHz
		AC97_CodecCmd(WRITE,0x32,0xac44);
		}
		else if(uAc97Fs==22050){
		//ADC Sampling frequency 22.05kHz
		AC97_CodecCmd(WRITE,0x32,0x5622);	 	
		}
		
		UART_Printf("\nAC97 Codec 0x32 Reg.: 0x%x\n\n", AC97_CodecCmd(READ,0x32,0x0000));
		AC97_CodecCmd(WRITE,0x5C, 0x2);			//ADC Slot Mapping: Left(Slot 6), Right(Slot 9)
		//LINE IN
		AC97_CodecCmd(WRITE,0x14, 0xfe12);		// Record Mux Source Selection: LINE L/R	
	
		//MIC IN
		//AC97_CodecCmd(WRITE,0x22, 0x4040);		//MIC Preamp Contorl : MIC2A only, 12dB, MICBias
		//AC97_CodecCmd(WRITE,0x10, 0x68);			//Unmute MIC routing : unmute MICA, MICA only 
		//AC97_CodecCmd(WRITE,0x14, 0xfe00);		//Record Mux Source Selection: MICA
	

		AC97_CodecCmd(WRITE,0x12, 0x1010);		// Unmute ADC and Set ADC Recoding Volume
	}
}

u8 AC97_SetOutputVolume(u8 uUpDownVolume)
{
	if( ( uUpDownVolume == 'u') | (uUpDownVolume == 'U') )
	{	
		if ((s16) g_AC97Infor.m_uOutputVolume <= 0x0000) 
		{
			UART_Printf("\nLimit Volume Range!");
			return false;
		} 
		else 
		{
			g_AC97Infor.m_uOutputVolume -= 0x0101;
			AC97_CodecCmd(WRITE,0x04, g_AC97Infor.m_uOutputVolume);		// PCM out Volume Up
			UART_Printf("\nSet Level (In AC97 Codec 04h Reg.): 0x%x", g_AC97Infor.m_uOutputVolume);
			UART_Printf("\nHeadphone Volume Level (In AC97 Codec 04h Reg.): 0x%x", AC97_CodecCmd(READ, 0x04,0));
			return true;
		}
	}
				
	if ( ( uUpDownVolume == 'd') | (uUpDownVolume == 'D') ) 
	{	
		if (AC97_CODEC== STAC9767)
		{	
			if (g_AC97Infor.m_uOutputVolume == 0x1F1F) 
			{
				UART_Printf("\nLimit Volume Range!");
				return false;
			}
			else 
			{
				g_AC97Infor.m_uOutputVolume += 0x0101;
				AC97_CodecCmd(WRITE,0x04, g_AC97Infor.m_uOutputVolume);		// PCM out Volume Down
				UART_Printf("\nSet Level (In AC97 Codec 04h Reg.): 0x%x", g_AC97Infor.m_uOutputVolume);
				UART_Printf("\nHeadphone Volume Level (In AC97 Codec 04h Reg.): 0x%x", AC97_CodecCmd(READ, 0x04,0));
				return true;
			}
		}
		else if (AC97_CODEC== WM9713)	
		{	
			if ((s16) g_AC97Infor.m_uOutputVolume >= 0x3F3F)
			{
				UART_Printf("\nLimit Volume Range!");
				return false;
			}
				
			else 
			{
				g_AC97Infor.m_uOutputVolume += 0x0101;
				AC97_CodecCmd(WRITE,0x04, g_AC97Infor.m_uOutputVolume);		// PCM out Volume Down
				UART_Printf("\nSet Level (In AC97 Codec 04h Reg.): 0x%x", g_AC97Infor.m_uOutputVolume);
				UART_Printf("\nHeadphone Volume Level (In AC97 Codec 04h Reg.): 0x%x", AC97_CodecCmd(READ, 0x04,0));
				return true;
			}
		}		
	}
	
	return false;
}

void AC97_ExitCodecPCMOut(void)
{
	//DACs off
	UART_Printf("\n\n=>DACs off PR1\n");
	AC97_CodecCmd((AC97_CMD)0,0x26,(1<<8)|(1<<9));
	AC97_ControllerState();
	UART_Printf("AC97 Codec Powerdown Ctrl/Stat 0x26 Reg.: 0x%x\n", AC97_CodecCmd(READ,0x26,0x0000));	

	//Analog off
	UART_Printf("\n=>Analog off PR2\n");
	AC97_CodecCmd((AC97_CMD)0,0x26,(1<<8)|(1<<9)|(1<<10));
	AC97_ControllerState();
	UART_Printf("AC97 Codec Powerdown Ctrl/Stat 0x26 Reg.: 0x%x\n", AC97_CodecCmd(READ,0x26,0x0000));	

	//Digital I/F off
	UART_Printf("\n=>Digital I/F off PR4\n");
	AC97_CodecCmd((AC97_CMD)0,0x26,(1<<8)|(1<<9)|(1<<10)|(1<<12));
	AC97_ControllerState();
	UART_Printf("AC97 Codec Powerdown Ctrl/Stat 0x26 Reg.: 0x%x\n", AC97_CodecCmd(READ,0x26,0x0000));	
}

void AC97_ExitCodecPCMIn(u16 uDACsOff)
{
	//ADCs off
	UART_Printf("\n\n=>ADCs off PR0\n");
	AC97_CodecCmd(WRITE,0x26,(1<<8));
	AC97_ControllerState();
	UART_Printf("AC97 Codec Powerdown Ctrl/Stat 0x26 Reg.: 0x%x\n", AC97_CodecCmd(READ,0x26,0x0000));

	if(uDACsOff == 1)
	{
		//DACs off
		UART_Printf("\n\n=>DACs off PR1\n");
		AC97_CodecCmd(WRITE,0x26,(1<<8)|(1<<9));
		AC97_ControllerState();
		UART_Printf("AC97 Codec Powerdown Ctrl/Stat 0x26 Reg.: 0x%x\n", AC97_CodecCmd(READ,0x26,0x0000));
	}
	//Analog off
	UART_Printf("\n=>Analog off PR2\n");
	AC97_CodecCmd(WRITE,0x26,(1<<8)|(1<<9)|(1<<10));
	AC97_ControllerState();
	UART_Printf("AC97 Codec Powerdown Ctrl/Stat 0x26 Reg.: 0x%x\n", AC97_CodecCmd(READ,0x26,0x0000));	

	//Digital I/F off
	UART_Printf("\n=>Digital I/F off PR4\n");
	AC97_CodecCmd(WRITE,0x26,(1<<8)|(1<<9)|(1<<10)|(1<<12));
	AC97_ControllerState();
	UART_Printf("AC97 Codec Powerdown Ctrl/Stat 0x26 Reg.: 0x%x\n", AC97_CodecCmd(READ,0x26,0x0000));
}


void AC97_ExitCodecMICIn(u16 uDACsOff)
{
	//ADCs off
	UART_Printf("\n\n=>ADCs off PR0\n");
	AC97_CodecCmd((AC97_CMD)0,0x26,(1<<8));
	AC97_ControllerState();
	UART_Printf("AC97 Codec Powerdown Ctrl/Stat 0x26 Reg.: 0x%x\n", AC97_CodecCmd(READ,0x26,0x0000));

	if(uDACsOff == 1)
	{
		//DACs off
		UART_Printf("\n\n=>DACs off PR1\n");
		AC97_CodecCmd((AC97_CMD)0,0x26,(1<<8)|(1<<9));
		AC97_ControllerState();
		UART_Printf("AC97 Codec Powerdown Ctrl/Stat 0x26 Reg.: 0x%x\n", AC97_CodecCmd(READ,0x26,0x0000));
	}
	
	//Analog off
	UART_Printf("\n=>Analog off PR2\n");
	AC97_CodecCmd((AC97_CMD)0,0x26,(1<<8)|(1<<9)|(1<<10));
	AC97_ControllerState();
	UART_Printf("AC97 Codec Powerdown Ctrl/Stat 0x26 Reg.: 0x%x\n", AC97_CodecCmd(READ,0x26,0x0000));	

	//Digital I/F off
	UART_Printf("\n=>Digital I/F off PR4\n");
	AC97_CodecCmd((AC97_CMD)0,0x26,(1<<8)|(1<<9)|(1<<10)|(1<<12));
	AC97_ControllerState();
	UART_Printf("AC97 Codec Powerdown Ctrl/Stat 0x26 Reg.: 0x%x\n", AC97_CodecCmd(READ,0x26,0x0000));	
}



