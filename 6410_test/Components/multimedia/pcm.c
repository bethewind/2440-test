/********************************************************************************
* 
*	Project Name : S3C6410 Validation
*
*	Copyright 2008 by Samsung Electronics, Inc.
*	All rights reserved.
*
*	Project Description :
*		This software is only for verifying functions of the S3C6410.
*		Anybody can use this software without our permission.
*  
*--------------------------------------------------------------------------------
* 
*	File Name : PCM_test.c
*  
*	File Description :
*
*	Author	: Sung-Hyun, Na
*	Dept. : AP Development Team
*	Created Date : 2008/02/01
*	Version : 0.2 
* 
*	History
*	- Version 0.1 (2007/03/16)
*	  -> Available with AK2440 PCM Codec.
*   - Version 0.2 (2007/04/
*	  -> Available with 
*********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "def.h"
#include "option.h"
#include "library.h"
#include "sfr6410.h"
#include "system.h"
#include "sysc.h"
#include "intc.h"
#include "gpio.h"
#include "iic.h"
#include "dma.h"

#include "pcm.h"
#include "ac97.h"
#include "i2s.h"

static DMAC g_oPcmDma;

extern u16 g_usAC97Setup;


PCM_Infor 		g_oPCMInfor[AUDIO_NUM];
volatile u16 		g_uPcmRecDone[AUDIO_NUM];
volatile u16 		uPcmPlayDone[AUDIO_NUM];
u16	*			g_PcmDataPointer[AUDIO_NUM];

//////////
// Function Name : PCM_Configure
// Function Desctiption : This fucntion initialize S3C6400  sturct of PCM State.
// Input : None
// Output : ocPCMstate (PCM State Structure)
// Version : 0.0
// Author : Sung-Hyun, Na


void PCM_Init(AUDIO_PORT rhs_ePort)
{
	if(rhs_ePort == AUDIO_PORT0)
	{
		//IP Operation
		g_oPCMInfor[rhs_ePort].m_ePCMClkSrc 			= PCM_MOUT_EPLL;
		g_oPCMInfor[rhs_ePort].m_ePCMMSBPosition 	= AFTER_PCMSYNC_HIGH;
		g_oPCMInfor[rhs_ePort].m_uRXFIFODipstick		= 0x20;
		g_oPCMInfor[rhs_ePort].m_uTXFIFODipstick		= 0x8;
		g_oPCMInfor[rhs_ePort].m_uPCMSClk 			= 768000;
		g_oPCMInfor[rhs_ePort].m_uPCMSync 			= 16000;
		g_oPCMInfor[rhs_ePort].m_eLine				= SMDK_PCM	;
		g_oPCMInfor[rhs_ePort].m_uIntAspects			= RX_FIFO_ALMOST_FULL | TXFIFO_ALMOST_EMPTY;
		
		#ifdef PCM_NORMAL_DMA
		g_oPCMInfor[rhs_ePort].m_uNumDma			= NUM_DMA0;		
		g_oPCMInfor[rhs_ePort].m_eDmaUnit			= DMA0;		
		#else
		g_oPCMInfor[rhs_ePort].m_uNumDma			= NUM_SDMA0;		
		g_oPCMInfor[rhs_ePort].m_eDmaUnit			= SDMA0;	
		#endif		
		g_oPCMInfor[rhs_ePort].m_eDmaCh			= DMA_A;
		
		g_oPCMInfor[rhs_ePort].m_uPcmRxFifoAddr		= PCM0_BASE + rPCMRXFIFO;
		g_oPCMInfor[rhs_ePort].m_uPcmTxFifoAddr		= PCM0_BASE + rPCMTXFIFO;

		g_oPCMInfor[rhs_ePort].m_uRecBufferStartAddr	= (u16 *)PCM_REC_BUF;
		g_oPCMInfor[rhs_ePort].m_uPcmSize			= PCM_REC_LEN;		
		g_oPCMInfor[rhs_ePort].m_uNumInt 			= NUM_PCM0;
		g_oPCMInfor[rhs_ePort].m_uTXFIFODipstick		= 0xa;
		g_oPCMInfor[rhs_ePort].m_uRXFIFODipstick		= 0x9;
		
	}

	else if(rhs_ePort == AUDIO_PORT1)
	{
		g_oPCMInfor[rhs_ePort].m_ePCMClkSrc 			= PCM_MOUT_EPLL;
		g_oPCMInfor[rhs_ePort].m_ePCMMSBPosition 	= AFTER_PCMSYNC_HIGH;
		g_oPCMInfor[rhs_ePort].m_uRXFIFODipstick		= 0x20;
		g_oPCMInfor[rhs_ePort].m_uTXFIFODipstick		= 0x8;
		g_oPCMInfor[rhs_ePort].m_uPCMSClk 			= 768000;
		g_oPCMInfor[rhs_ePort].m_uPCMSync 			= 16000;
		g_oPCMInfor[rhs_ePort].m_eLine				= SMDK_PCM	;
		g_oPCMInfor[rhs_ePort].m_uIntAspects			= RX_FIFO_ALMOST_FULL | TXFIFO_ALMOST_EMPTY;

		#ifdef PCM_NORMAL_DMA
		g_oPCMInfor[rhs_ePort].m_uNumDma			= NUM_DMA1;		
		g_oPCMInfor[rhs_ePort].m_eDmaUnit			= DMA1;	
		#else
		g_oPCMInfor[rhs_ePort].m_uNumDma			= NUM_SDMA1;		
		g_oPCMInfor[rhs_ePort].m_eDmaUnit			= SDMA1;		
		#endif
		
		g_oPCMInfor[rhs_ePort].m_eDmaCh			= DMA_B;
		g_oPCMInfor[rhs_ePort].m_uPcmRxFifoAddr		= PCM1_BASE + rPCMRXFIFO;
		g_oPCMInfor[rhs_ePort].m_uPcmTxFifoAddr		= PCM1_BASE + rPCMTXFIFO;

		g_oPCMInfor[rhs_ePort].m_uRecBufferStartAddr	= (u16 *)PCM_REC_BUF;
		g_oPCMInfor[rhs_ePort].m_uPcmSize			= PCM_REC_LEN;
		g_oPCMInfor[rhs_ePort].m_uNumInt 			= NUM_PCM1;
		g_oPCMInfor[rhs_ePort].m_uTXFIFODipstick		= 0xb;
		g_oPCMInfor[rhs_ePort].m_uRXFIFODipstick		= 0x10;
	}
	
	//Put Data PCMCON
	PCMCTL_PutData(rhs_ePort);		
}



void PCM_Sel_PCMCLKSOURCE(AUDIO_PORT rhs_ePort)
{
	u8 selTemp;
	
	UART_Printf("Select PCM Source CLK\n");
	UART_Printf("0. EPLL[D]		1. MPLL		2. FIN\n");
	UART_Printf("3. PCM_PCMCDCLK		4. PCLK\n");
	selTemp = UART_GetIntNum();
	
	switch(selTemp)
	{
		case 0:
			g_oPCMInfor[rhs_ePort].m_ePCMClkSrc = PCM_MOUT_EPLL;
			I2S_Init(rhs_ePort);
			I2S_SetEPLL( rhs_ePort);
			break;
		case 1:
			g_oPCMInfor[rhs_ePort].m_ePCMClkSrc = PCM_DOUT_MPLL;
			break;
		case 2:
			g_oPCMInfor[rhs_ePort].m_ePCMClkSrc = PCM_FIN_EPLL;
			break;
		case 3:
			g_oPCMInfor[rhs_ePort].m_ePCMClkSrc = PCM_PCMCDCLK;
			break;
		case 4:
			g_oPCMInfor[rhs_ePort].m_ePCMClkSrc = PCM_PCLK;
			break;
		default:
			g_oPCMInfor[rhs_ePort].m_ePCMClkSrc = PCM_MOUT_EPLL;
			break;
	}
	PCM_SelCLKAUDIO(rhs_ePort, g_oPCMInfor[rhs_ePort].m_ePCMClkSrc);
	Delay(1000);	
}

void  PCM_Sel_PCMSCLK(AUDIO_PORT rhs_ePort)
{

	u8 selTemp;
	UART_Printf("Select PCM SCLK\n");
	UART_Printf("0. 128KHz	1. 256KHz[D]		2. 512KHz		3. 768KHz	4. 1.536MHz\n");
	selTemp = UART_GetIntNum();
	
	switch(selTemp)
	{
		case 0:
			g_oPCMInfor[rhs_ePort].m_uPCMSClk = 128000;
			break;
		case 1:
			g_oPCMInfor[rhs_ePort].m_uPCMSClk = 256000;
			break;
		case 2:
			g_oPCMInfor[rhs_ePort].m_uPCMSClk = 512000;
			break;
		case 3:
			g_oPCMInfor[rhs_ePort].m_uPCMSClk = 768000;
			break;
		case 4:
			g_oPCMInfor[rhs_ePort].m_uPCMSClk = 1536000;
			break;
		default:
			g_oPCMInfor[rhs_ePort].m_uPCMSClk = 256000;
			break;
	}	
}

void PCM_Sel_PCMSYNC(AUDIO_PORT rhs_ePort)
{
	u8 selTemp;
	UART_Printf("Select PCM SCLK\n");	

	UART_Printf("0. 8Khz[D]	");	
	if (g_oPCMInfor[rhs_ePort].m_uPCMSClk > 128000)
		UART_Printf("1. 16Khz	");
	if  (g_oPCMInfor[rhs_ePort].m_uPCMSClk > 256000)
		UART_Printf("2. 32Khz	");
	if (g_oPCMInfor[rhs_ePort].m_uPCMSClk > 512000)
		UART_Printf("3. 48Khz	");
	if (g_oPCMInfor[rhs_ePort].m_uPCMSClk > 768000)
		UART_Printf("4. 96Khz	");	
	UART_Printf("\n");	
	selTemp = UART_GetIntNum();
	switch(selTemp)
	{
		case 0:
			g_oPCMInfor[rhs_ePort].m_uPCMSync = 8000;
			break;			
		case 1:
			if (g_oPCMInfor[rhs_ePort].m_uPCMSClk > 128000)
				g_oPCMInfor[rhs_ePort].m_uPCMSync= 16000;
			else 
				g_oPCMInfor[rhs_ePort].m_uPCMSync= 8000;
			break;
		case 2:
			if (g_oPCMInfor[rhs_ePort].m_uPCMSClk > 256000)
				g_oPCMInfor[rhs_ePort].m_uPCMSync = 32000;
			else 
				g_oPCMInfor[rhs_ePort].m_uPCMSync = 8000;
			break;
		case 3:
			if (g_oPCMInfor[rhs_ePort].m_uPCMSClk > 512000)
				g_oPCMInfor[rhs_ePort].m_uPCMSync= 48000;
			else 
				g_oPCMInfor[rhs_ePort].m_uPCMSync= 8000;
			break;
		case 4:
			if (g_oPCMInfor[rhs_ePort].m_uPCMSClk > 768000)
				g_oPCMInfor[rhs_ePort].m_uPCMSync = 96000;
			else 
				g_oPCMInfor[rhs_ePort].m_uPCMSync = 8000;
			break;
		default:
			g_oPCMInfor[rhs_ePort].m_uPCMSync = 8000;
			break;		
	}	
}
	
void PCM_Sel_SyncPosition(AUDIO_PORT rhs_ePort)
{
	u8 selTemp;
	
	UART_Printf("\nSelect PCM data MSB Postion\n");
	UART_Printf("0.DURING_PCMSYNC_HIGH(DSP Mode B)	1.AFTER_PCMSYNC_HIGH(DSP Mode A)[D]\n ");
	selTemp = UART_GetIntNum();

	if (selTemp == 0)
		g_oPCMInfor[rhs_ePort].m_ePCMMSBPosition  = DURING_PCMSYNC_HIGH;
	else 
		g_oPCMInfor[rhs_ePort].m_ePCMMSBPosition  = AFTER_PCMSYNC_HIGH;		
}

void PCM_Sel_Interrupt(AUDIO_PORT rhs_ePort)
{
	u8 ucSel;
	while(1)
	{
		UART_Printf("\nExit : 99\n");
		PCMIRQCTL_GetIRQMode(rhs_ePort);
		UART_Printf("\nChoose Interrupt Source\n");
		ucSel = UART_GetIntNum();
		
		if (ucSel == 99)
			break;
		
		switch(ucSel)
		{
			case 0:
				if ( g_oPCMInfor[rhs_ePort].m_uIntAspects & PCMIRQ_Enable_TransferDone)
					g_oPCMInfor[rhs_ePort].m_uIntAspects &= ~PCMIRQ_Enable_TransferDone;
				else 
					g_oPCMInfor[rhs_ePort].m_uIntAspects |= PCMIRQ_Enable_TransferDone;
				break;

			case 10:
				if ( g_oPCMInfor[rhs_ePort].m_uIntAspects & PCMIRQ_Enable_TX_FIFO_EMPTY)
					g_oPCMInfor[rhs_ePort].m_uIntAspects &= ~PCMIRQ_Enable_TX_FIFO_EMPTY;
				else 
					g_oPCMInfor[rhs_ePort].m_uIntAspects |= PCMIRQ_Enable_TX_FIFO_EMPTY;
				break;

			case 11:
				if ( g_oPCMInfor[rhs_ePort].m_uIntAspects & PCMIRQ_Enable_TX_FIFO_ALMOST_EMPTY)
					g_oPCMInfor[rhs_ePort].m_uIntAspects &= ~PCMIRQ_Enable_TX_FIFO_ALMOST_EMPTY;
				else 
					g_oPCMInfor[rhs_ePort].m_uIntAspects |= PCMIRQ_Enable_TX_FIFO_ALMOST_EMPTY;
				break;

			case 12:
				if ( g_oPCMInfor[rhs_ePort].m_uIntAspects & PCMIRQ_Enable_TX_FIFO_FULL)
					g_oPCMInfor[rhs_ePort].m_uIntAspects &= ~PCMIRQ_Enable_TX_FIFO_FULL;
				else 
					g_oPCMInfor[rhs_ePort].m_uIntAspects |= PCMIRQ_Enable_TX_FIFO_FULL;
				break;
				
			case 13:
				if ( g_oPCMInfor[rhs_ePort].m_uIntAspects & PCMIRQ_Enable_TX_FIFO_ALMOST_FULL)
					g_oPCMInfor[rhs_ePort].m_uIntAspects &= ~PCMIRQ_Enable_TX_FIFO_ALMOST_FULL;
				else 
					g_oPCMInfor[rhs_ePort].m_uIntAspects |= PCMIRQ_Enable_TX_FIFO_ALMOST_FULL;
				break;
				
			case 14:
				if ( g_oPCMInfor[rhs_ePort].m_uIntAspects & PCMIRQ_Enable_TX_FIFO_STARVE_ERROR)
					g_oPCMInfor[rhs_ePort].m_uIntAspects &= ~PCMIRQ_Enable_TX_FIFO_STARVE_ERROR;
				else 
					g_oPCMInfor[rhs_ePort].m_uIntAspects |= PCMIRQ_Enable_TX_FIFO_STARVE_ERROR;
				break;
			case 15:
				if ( g_oPCMInfor[rhs_ePort].m_uIntAspects & PCMIRQ_Enable_TX_FIFO_OVERFLOW_ERROR)
					g_oPCMInfor[rhs_ePort].m_uIntAspects &= ~PCMIRQ_Enable_TX_FIFO_OVERFLOW_ERROR;
				else 
					g_oPCMInfor[rhs_ePort].m_uIntAspects |= PCMIRQ_Enable_TX_FIFO_OVERFLOW_ERROR;
				break;

			case 20:
				if ( g_oPCMInfor[rhs_ePort].m_uIntAspects & PCMIRQ_Enable_RX_FIFO_EMPTY)
					g_oPCMInfor[rhs_ePort].m_uIntAspects &= ~PCMIRQ_Enable_RX_FIFO_EMPTY;
				else 
					g_oPCMInfor[rhs_ePort].m_uIntAspects |= PCMIRQ_Enable_RX_FIFO_EMPTY;
				break;

			case 21:
				if ( g_oPCMInfor[rhs_ePort].m_uIntAspects & PCMIRQ_Enable_RX_FIFO_ALMOST_EMPTY)
					g_oPCMInfor[rhs_ePort].m_uIntAspects &= ~PCMIRQ_Enable_RX_FIFO_ALMOST_EMPTY;
				else 
					g_oPCMInfor[rhs_ePort].m_uIntAspects |= PCMIRQ_Enable_RX_FIFO_ALMOST_EMPTY;
				break;

			case 22:
				if ( g_oPCMInfor[rhs_ePort].m_uIntAspects & PCMIRQ_Enable_RX_FIFO_FULL)
					g_oPCMInfor[rhs_ePort].m_uIntAspects &= ~PCMIRQ_Enable_RX_FIFO_FULL;
				else 
					g_oPCMInfor[rhs_ePort].m_uIntAspects |= PCMIRQ_Enable_RX_FIFO_FULL;
				break;
				
			case 23:
				if ( g_oPCMInfor[rhs_ePort].m_uIntAspects & PCMIRQ_Enable_RX_FIFO_ALMOST_FULL)
					g_oPCMInfor[rhs_ePort].m_uIntAspects &= ~PCMIRQ_Enable_RX_FIFO_ALMOST_FULL;
				else 
					g_oPCMInfor[rhs_ePort].m_uIntAspects |= PCMIRQ_Enable_RX_FIFO_ALMOST_FULL;
				break;
				
			case 24:
				if ( g_oPCMInfor[rhs_ePort].m_uIntAspects & PCMIRQ_Enable_RX_FIFO_STARVE_ERROR)
					g_oPCMInfor[rhs_ePort].m_uIntAspects &= ~PCMIRQ_Enable_RX_FIFO_STARVE_ERROR;
				else 
					g_oPCMInfor[rhs_ePort].m_uIntAspects |= PCMIRQ_Enable_RX_FIFO_STARVE_ERROR;
				break;
			case 25:
				if ( g_oPCMInfor[rhs_ePort].m_uIntAspects & PCMIRQ_Enable_RX_FIFO_OVERFLOW_ERROR)
					g_oPCMInfor[rhs_ePort].m_uIntAspects &= ~PCMIRQ_Enable_RX_FIFO_OVERFLOW_ERROR;
				else 
					g_oPCMInfor[rhs_ePort].m_uIntAspects |= PCMIRQ_Enable_RX_FIFO_OVERFLOW_ERROR;
				break;
				
		}
		
		//PCMIRQCTL_SetIRQ(rhs_ePort);
		//PCMIRQCTL_EnableInt(rhs_ePort, ENABLE);
		PCMOutp32(rhs_ePort, rPCMIRQCTL, g_oPCMInfor[rhs_ePort].m_uIntAspects);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void PCM_InitPort(AUDIO_PORT rhs_ePort)
{
	if (rhs_ePort == AUDIO_PORT0)
	{
		//Set GPD: PCM Port
		GPIO_SetFunctionAll(eGPIO_D, 0x22222, 0); 
		GPIO_SetPullUpDownAll(eGPIO_D, 0x0);	
	}					
	else if (rhs_ePort == AUDIO_PORT1)
	{
		//Set GPE: PCM Port
		GPIO_SetFunctionAll(eGPIO_E, 0x22222, 0); 
		GPIO_SetPullUpDownAll(eGPIO_E, 0x0);				//Pull Down Enable
	}

	Delay(1000);
}

void PCM_CLKOUT(AUDIO_PORT rhs_ePort)
{		
	I2S_Init((AUDIO_PORT)(~rhs_ePort & 0x1));
		
	I2S_InitPort((AUDIO_PORT)((~rhs_ePort) & 0x1));
	I2S_CDCLKOut((AUDIO_PORT)((~rhs_ePort) & 0x1));		 
}

void PCM_CLOSECLKOUT(AUDIO_PORT rhs_ePort)
{
	I2S_ReturnPort((AUDIO_PORT)(~rhs_ePort & 0x1));		
}

void PCM_PCMInDMA(AUDIO_PORT rhs_ePort)
{
	u32 uTempRegister;	
	u32 uSclkDiv, uSyncDiv;
	
	g_uPcmRecDone[rhs_ePort] = 0;
	
	if(rhs_ePort == AUDIO_PORT0)
	{
		g_oPCMInfor[rhs_ePort].m_eDmaId = eSEL_PCM0_RX;
		g_oPCMInfor[rhs_ePort].m_eDmaCh = DMA_A;
		g_oPCMInfor[rhs_ePort].m_eDreqSrc = DMA0_PCM0_RX;
		INTC_SetVectAddr(g_oPCMInfor[rhs_ePort].m_uNumDma, Isr_PCMPORT0_PCMIn_DMADone);
	}
	else if(rhs_ePort == AUDIO_PORT1)
	{
		g_oPCMInfor[rhs_ePort].m_eDmaId = eSEL_PCM1_RX;
		g_oPCMInfor[rhs_ePort].m_eDmaCh = DMA_A;
		g_oPCMInfor[rhs_ePort].m_eDreqSrc = DMA1_PCM1_RX;
		INTC_SetVectAddr(g_oPCMInfor[rhs_ePort].m_uNumDma, Isr_PCMPORT1_PCMIn_DMADone);
	}
		
	//IRQ Setting
	INTC_Enable(g_oPCMInfor[rhs_ePort].m_uNumDma);

	//DMA Setting
	#ifdef PCM_NORMAL_DMA
    	SYSC_SelectDMA(g_oPCMInfor[rhs_ePort].m_eDmaId, 1);
	#else
	SYSC_SelectDMA(g_oPCMInfor[rhs_ePort].m_eDmaId, 0);
	#endif
	
    	DMAC_InitCh(g_oPCMInfor[rhs_ePort].m_eDmaUnit , g_oPCMInfor[rhs_ePort].m_eDmaCh, &g_oPcmDma);

    	DMACH_ClearIntPending(&g_oPcmDma);
	DMACH_ClearErrIntPending(&g_oPcmDma);
	DMACH_Setup(g_oPCMInfor[rhs_ePort].m_eDmaCh, 0x0, g_oPCMInfor[rhs_ePort].m_uPcmRxFifoAddr, true, 
					(u32)g_oPCMInfor[rhs_ePort].m_uRecBufferStartAddr, false, HWORD, g_oPCMInfor[rhs_ePort].m_uPcmSize/4, 
					HANDSHAKE, g_oPCMInfor[rhs_ePort].m_eDreqSrc, MEM, SINGLE, &g_oPcmDma);   	
    	DMACH_Start(&g_oPcmDma);
      
	
	PCM_GetClkValAndClkDir(rhs_ePort, &uSclkDiv, &uSyncDiv);
	PCMCLKCTL_SetDivideLogic(rhs_ePort, uSclkDiv, uSyncDiv);
	
	//Transfer data enable 
	uTempRegister = PCMInp32(rhs_ePort, rPCMCTRL);
	uTempRegister |= RXFIFO_DIPSTICK(0x20) |PCMCTL_EN_RX_DMA |PCMCTL_EN_RX_FIFO |PCMCTL_EN_PCMIF;
	PCMOutp32(rhs_ePort, rPCMCTRL, uTempRegister);

	while(g_uPcmRecDone[rhs_ePort]  ==0)
    	{
        	UART_Printf(".");
        	Delay(3000);
    	}

   	 g_uPcmRecDone[rhs_ePort] = 0;
 
    	DMACH_Stop(&g_oPcmDma);		// DMA stop
    	INTC_Disable(g_oPCMInfor[rhs_ePort].m_uNumDma);

	PCMCLKCTL_ActiveDivideLogic(rhs_ePort, FALSE);
		
	uTempRegister &= ~PCMCTL_EN_PCMIF;
    	PCMOutp32(rhs_ePort, rPCMCTRL, uTempRegister);	   	
  	Delay(1000); 

  	UART_Printf("\nEnd of Record!\n");
}



void PCM_PCMOutDMA(AUDIO_PORT rhs_ePort)
{
	u8 uChar;
	u32 uTempRegister;
	u32 uSclkDiv, uSyncDiv;

	if(rhs_ePort == AUDIO_PORT0)
	{
		g_oPCMInfor[rhs_ePort].m_eDmaId = eSEL_PCM0_TX;
		g_oPCMInfor[rhs_ePort].m_eDmaCh = DMA_B;
		g_oPCMInfor[rhs_ePort].m_eDreqSrc = DMA0_PCM0_TX;
		INTC_SetVectAddr(g_oPCMInfor[rhs_ePort].m_uNumDma, Isr_PCMPORT0_PCMOut_DMADone);			
		INTC_SetVectAddr(NUM_PCM0, ISR_PCM_Port0TXFIFOEr);											// Interrupt for PCM TX FIFO Starve Error		
		INTC_Enable(NUM_PCM0);
	}
	else if(rhs_ePort == AUDIO_PORT1)
	{
		g_oPCMInfor[rhs_ePort].m_eDmaId = eSEL_PCM1_TX;
		g_oPCMInfor[rhs_ePort].m_eDmaCh = DMA_B;
		g_oPCMInfor[rhs_ePort].m_eDreqSrc = DMA1_PCM1_TX;
		INTC_SetVectAddr(g_oPCMInfor[rhs_ePort].m_uNumDma, Isr_PCMPORT1_PCMOut_DMADone);
		INTC_SetVectAddr(NUM_PCM1, ISR_PCM_Port1TXStarveEr);											// Interrupt for PCM TX FIFO Starve Error
		INTC_Enable(NUM_PCM1);
	}
	
	INTC_Enable(g_oPCMInfor[rhs_ePort].m_uNumDma);
	//PCM IRQ Setting
	//PCMIRQCTL_SetIRQMode(rhs_ePort, PCMIRQALL, DISABLE);											// Disable another Interrupt Condition
	//PCMIRQCTL_SetIRQMode(rhs_ePort, TXFIFO_ERROR_STARVE, ENABLE);
	//PCMIRQCTL_SetIRQMode(rhs_ePort, TXFIFO_EMPTY, ENABLE);							
	//PCMIRQCTL_EnableInt(rhs_ePort, ENABLE);															// Enable IRQ Gen'

	//DMA Setting
	#ifdef PCM_NORMAL_DMA
    	SYSC_SelectDMA(g_oPCMInfor[rhs_ePort].m_eDmaId, 1);
	#else
	SYSC_SelectDMA(g_oPCMInfor[rhs_ePort].m_eDmaId, 0);
	#endif
   	
    	DMAC_InitCh(g_oPCMInfor[rhs_ePort].m_eDmaUnit, g_oPCMInfor[rhs_ePort].m_eDmaCh, &g_oPcmDma);

    	DMACH_ClearIntPending(&g_oPcmDma);
	DMACH_ClearErrIntPending(&g_oPcmDma);
	DMACH_Setup(g_oPCMInfor[rhs_ePort].m_eDmaCh, 0x0, (u32)g_oPCMInfor[rhs_ePort].m_uRecBufferStartAddr, false, 
					g_oPCMInfor[rhs_ePort].m_uPcmTxFifoAddr, true, HWORD, g_oPCMInfor[rhs_ePort].m_uPcmSize/4, 
					HANDSHAKE, MEM, g_oPCMInfor[rhs_ePort].m_eDreqSrc , SINGLE, &g_oPcmDma);				// DMA Set-Up
  	
    	DMACH_Start(&g_oPcmDma);																			// DMA Start

	//PCM Clock Setting 	
	PCM_GetClkValAndClkDir(rhs_ePort, &uSclkDiv, &uSyncDiv);
	PCMCLKCTL_SetDivideLogic(rhs_ePort, uSclkDiv, uSyncDiv);
	
	//Transfer data enable  
	uTempRegister = PCMInp32(rhs_ePort, rPCMCTRL);
	uTempRegister |= TXFIFO_DIPSTICK(0x8) |PCMCTL_EN_TX_DMA |PCMCTL_EN_TX_FIFO |PCMCTL_EN_PCMIF & ~PCMCTL_EN_RX_DMA;
	PCMOutp32(rhs_ePort, rPCMCTRL, uTempRegister);
	
	#if (PCM_CODEC == WM9713)
		
	UART_Printf("Codec CMD : 's'	Exit : x\n");
	while(1)
	{
		UART_Printf("0x%x\n",AC97_CodecCmd(READ,0x36,0x0000));
		uChar = UART_Getc();

		if( (uChar == 'x') | (uChar == 'X')) 
			break;

		else if ( (uChar == 's' )| (uChar =='S') )
			AC97_CodecCmdManually();
	} 
	
	#else
	
	UART_Printf("\nIf you want to exit, Press the 'x' key.\n");
	UART_Printf("DMA Disable : P		Enable : O\n");
	UART_Printf("FIFO Disable : L	Enable : K\n");
	UART_Printf("CLK Disable : I		Enable : U\n");
	while(1)
	{
		
		uChar = UART_Getc();
		if( (uChar == 'x') | (uChar == 'X')) 
			break;
		else if ( (uChar == 'o') | (uChar == 'O'))
			PCMCTL_EnableDMA(rhs_ePort, PCM_TX, true);
		else if ( (uChar == 'p') | (uChar == 'P'))
			PCMCTL_EnableDMA(rhs_ePort, PCM_TX, false);
		else if ( (uChar == 'l') | (uChar == 'L'))
			PCMCTL_EnableFIFO(rhs_ePort, PCM_TX, false);
		else if ( (uChar == 'k') | (uChar == 'K'))
			PCMCTL_EnableFIFO(rhs_ePort, PCM_TX, true);
		else if ( (uChar == 'i') | (uChar == 'I'))
			PCMCLKCTL_ActiveDivideLogic(rhs_ePort, false);
		else if ( (uChar == 'u') | (uChar == 'U'))
			PCMCLKCTL_ActiveDivideLogic(rhs_ePort, true);
		
	} 

	#endif

	DMACH_Stop(&g_oPcmDma);		// DMA stop
	INTC_Disable(g_oPCMInfor[rhs_ePort].m_uNumDma);
	INTC_Disable(g_oPCMInfor[rhs_ePort].m_uNumInt);
	
	//PCMIRQCTL_EnableInt(rhs_ePort, FALSE);

	PCMCLKCTL_ActiveDivideLogic(rhs_ePort, FALSE);
	
	uTempRegister &= ~PCMCTL_EN_PCMIF;
    	PCMOutp32(rhs_ePort, rPCMCTRL, uTempRegister);	
}

void PCM_PCMOutDMAStop(AUDIO_PORT rhs_ePort)
{
	u32 uTempRegister= 0;
	DMACH_Stop(&g_oPcmDma);		// DMA stop

	INTC_Disable(g_oPCMInfor[rhs_ePort].m_uNumDma);

	uTempRegister &= ~PCMCTL_EN_PCMIF;
    	PCMOutp32(rhs_ePort, rPCMCTRL, uTempRegister);
	PCMCLKCTL_ActiveDivideLogic(rhs_ePort, false);	
}

void PCM_BypassDMA(AUDIO_PORT rhs_ePort)
{
	u32 uTempRegister;	

	if(rhs_ePort == AUDIO_PORT0)
	{
		g_oPCMInfor[rhs_ePort].m_eDmaId = eSEL_PCM0_TX;
		g_oPCMInfor[rhs_ePort].m_eDmaCh = DMA_B;
		g_oPCMInfor[rhs_ePort].m_eDreqSrc = DMA0_PCM0_TX;
		INTC_SetVectAddr(g_oPCMInfor[rhs_ePort].m_uNumDma, Isr_PCMPORT0_PCMOut_DMADone);
	}
	else if(rhs_ePort == AUDIO_PORT1)
	{
		g_oPCMInfor[rhs_ePort].m_eDmaId = eSEL_PCM1_TX;
		g_oPCMInfor[rhs_ePort].m_eDmaCh = DMA_B;
		g_oPCMInfor[rhs_ePort].m_eDreqSrc = DMA1_PCM1_TX;
		INTC_SetVectAddr(g_oPCMInfor[rhs_ePort].m_uNumDma, Isr_PCMPORT1_PCMOut_DMADone);
	}
	
	//IRQ Setting
	INTC_Enable(g_oPCMInfor[rhs_ePort].m_uNumDma);
	

	//DMA Setting
   	SYSC_SelectDMA(g_oPCMInfor[rhs_ePort].m_eDmaId, 1);
    	DMAC_InitCh(g_oPCMInfor[rhs_ePort].m_eDmaUnit, g_oPCMInfor[rhs_ePort].m_eDmaCh, &g_oPcmDma);

    	DMACH_ClearIntPending(&g_oPcmDma);
	DMACH_ClearErrIntPending(&g_oPcmDma);
	DMACH_Setup(g_oPCMInfor[rhs_ePort].m_eDmaCh, 0x0, (u32)g_oPCMInfor[rhs_ePort].m_uPcmRxFifoAddr, true, 
					g_oPCMInfor[rhs_ePort].m_uPcmTxFifoAddr, true, HWORD, g_oPCMInfor[rhs_ePort].m_uPcmSize/4, 
					HANDSHAKE, MEM, g_oPCMInfor[rhs_ePort].m_eDreqSrc , SINGLE, &g_oPcmDma);	
  	
    	DMACH_Start(&g_oPcmDma);

	//PCM Clock Setting 
	
	//Transfer data enable  
	uTempRegister = PCMInp32(rhs_ePort, rPCMCTRL);
	uTempRegister |= TXFIFO_DIPSTICK(0x8) | PCMCTL_EN_TX_DMA |PCMCTL_EN_TX_FIFO |PCMCTL_EN_RX_DMA |PCMCTL_EN_RX_FIFO |PCMCTL_EN_PCMIF;
	PCMOutp32(rhs_ePort, rPCMCTRL, uTempRegister);		

}

void PCM_PCMInInt(AUDIO_PORT rhs_ePort)
{
	u32 uTempRegister = 0, uIRQRegiset = 0;
	u32 uSclkDiv, uSyncDiv;
		
	g_uPcmRecDone[rhs_ePort] =0; 
	g_PcmDataPointer[rhs_ePort] = g_oPCMInfor[rhs_ePort].m_uRecBufferStartAddr;

	//IRQ Initialization
	//Disable PCM IRQ to ARM
	uIRQRegiset = PCMInp32(rhs_ePort,  rPCMIRQCTL);
	uIRQRegiset &= ~PCMIRQ_Enable_IRQ_TO_ARM;
	PCMOutp32(rhs_ePort, rPCMIRQCTL, uIRQRegiset);
	
	INTC_ClearVectAddr();
	
	if(rhs_ePort == AUDIO_PORT0)
		INTC_SetVectAddr(g_oPCMInfor[rhs_ePort].m_uNumInt, Isr_PCMPORT0_PCMIn);
	
	else if(rhs_ePort == AUDIO_PORT1)
		INTC_SetVectAddr(g_oPCMInfor[rhs_ePort].m_uNumInt, Isr_PCMPORT1_PCMIn);	
	
	INTC_Enable(g_oPCMInfor[rhs_ePort].m_uNumInt);	
	
	PCM_ClearInt(rhs_ePort);														// Clear Interrupt	
	#if 0
	PCMIRQCTL_SetIRQMode(rhs_ePort, PCMIRQALL, false);								// Disable another Interrupt Condition
	PCMIRQCTL_SetIRQMode(rhs_ePort, RX_FIFO_ALMOST_FULL, true);					// Set Irq condition when RX FIFO ALMOST Full	
	PCMIRQCTL_EnableInt(rhs_ePort, ENABLE);										// Enable IRQ Gen'
	#else
	//PCMIRQCTL_SetIRQ(rhs_ePort);												// Set Interrupt Condition	
	PCMIRQCTL_EnableInt(rhs_ePort, TRUE);										// Enable Interrupt 
	#endif

	// PCM CLK and Divider Enable
	PCM_GetClkValAndClkDir(rhs_ePort, &uSclkDiv, &uSyncDiv);
	PCMCLKCTL_SetDivideLogic(rhs_ePort, uSclkDiv, uSyncDiv);	
	
	//Transfer data enable  
	uTempRegister = PCMInp32(rhs_ePort, rPCMCTRL);
	uTempRegister |= RXFIFO_DIPSTICK(0x20) | PCM_RXFIFO_EN  |PCMCTL_EN_PCMIF & ~PCM_TXFIFO_EN & ~PCMCTL_EN_RX_DMA;
	PCMOutp32(rhs_ePort, rPCMCTRL, uTempRegister);
	
	UART_Printf("Now Recoding....\n");
	PCMIRQCTL_GetIRQMode(rhs_ePort);
	while(1)
	{

		if(g_uPcmRecDone[rhs_ePort])
			break;	
	}
	g_uPcmRecDone[rhs_ePort] = 0;
	INTC_Disable(g_oPCMInfor[rhs_ePort].m_uNumInt);
	PCMIRQCTL_EnableInt(rhs_ePort, FALSE);
   	
    	PCMCTL_EnablePCMIF(rhs_ePort,  false);	
	UART_Printf("\nEnd of Record!\n");
}


void PCM_PCMOutInt(AUDIO_PORT rhs_ePort)
{
	u32 uTempRegister, uIRQRegiset = 0;
	u32 uSclkDiv, uSyncDiv;

	uPcmPlayDone[rhs_ePort] =0;	
	g_PcmDataPointer[rhs_ePort] = g_oPCMInfor[rhs_ePort].m_uRecBufferStartAddr;
	

	//IRQ Initialization
	uIRQRegiset = PCMInp32(rhs_ePort,  rPCMIRQCTL);
	uIRQRegiset &= ~PCMIRQ_Enable_IRQ_TO_ARM;
	PCMOutp32(rhs_ePort, rPCMIRQCTL, uIRQRegiset);
	INTC_ClearVectAddr();
	
	if(rhs_ePort == AUDIO_PORT0)
		INTC_SetVectAddr(g_oPCMInfor[rhs_ePort].m_uNumInt, Isr_PCMPORT0_PCMOut);
	
	else if(rhs_ePort == AUDIO_PORT1)
		INTC_SetVectAddr(g_oPCMInfor[rhs_ePort].m_uNumInt, Isr_PCMPORT1_PCMOut);
	INTC_Enable(g_oPCMInfor[rhs_ePort].m_uNumInt);
  
	PCM_ClearInt(rhs_ePort);

	// Interrupt
	#if 0
	PCMIRQCTL_SetIRQMode(rhs_ePort, PCMIRQALL, false);								// Disable another Interrupt Condition
	PCMIRQCTL_SetIRQMode(rhs_ePort, TXFIFO_ALMOST_EMPTY, true);						// Set Irq condition when RX FIFO ALMOST Full
	PCMIRQCTL_EnableInt(rhs_ePort, ENABLE);											// Enable IRQ Gen'
	#else
	//PCMIRQCTL_SetIRQ(rhs_ePort);												// Set Interrupt Condition	
	PCMIRQCTL_EnableInt(rhs_ePort, TRUE);										// Enable Interrupt 
	#endif 
	
	//PCM Clock Setting 
	PCM_GetClkValAndClkDir(rhs_ePort, &uSclkDiv, &uSyncDiv);
	PCMCLKCTL_SetDivideLogic(rhs_ePort, uSclkDiv, uSyncDiv);
	
	uTempRegister = PCMInp32(rhs_ePort, rPCMCTRL);
	uTempRegister = TXFIFO_DIPSTICK(0x14) |PCM_TXFIFO_EN|PCMCTL_EN_PCMIF;
	//Transfer data enable  
	PCMOutp32(rhs_ePort, rPCMCTRL,  uTempRegister);
	UART_Printf("Now Playing....\n");
	PCMIRQCTL_GetIRQMode(rhs_ePort);
	while(1)
	{
		if(uPcmPlayDone[rhs_ePort])
			break;	
	}
	INTC_Disable(g_oPCMInfor[rhs_ePort].m_uNumInt);
	PCMIRQCTL_EnableInt(rhs_ePort, false);
   	
   	PCMCTL_EnablePCMIF(rhs_ePort,  false);	
	//PCM_DisableInt();

	UART_Printf("\nEnd of Play!\n");
}
void PCM_PCMInPolling(AUDIO_PORT rhs_ePort)
{
	u32 uTempRegister = 0, uFIFOCNT = 0;
	u16 uRegValue;
	u16* uRecBuf = g_oPCMInfor[rhs_ePort].m_uRecBufferStartAddr;
	s32 uPcmDataSize = (s32) g_oPCMInfor[rhs_ePort].m_uPcmSize;
	u32 uFIFOTriggerLevel = 0x8;
	u32 uSclkDiv, uSyncDiv;

	//PCM Clock Setting 
	PCM_GetClkValAndClkDir(rhs_ePort, &uSclkDiv, &uSyncDiv);
	PCMCLKCTL_SetDivideLogic(rhs_ePort, uSclkDiv, uSyncDiv);
	
	uTempRegister = PCMInp32(rhs_ePort, rPCMCTRL);
	uTempRegister |= PCM_RXFIFO_EN  |PCMCTL_EN_PCMIF ;
	PCMOutp32(rhs_ePort, rPCMCTRL, uTempRegister);	
	UART_Printf("Now Recoding....\n");

	while( uPcmDataSize > 0 )
	{
		do 
		{
			uFIFOCNT = PCMInp32(rhs_ePort, rPCMFIFOSTAT);
		} while ((uFIFOCNT & (PCMFIFO_Status_RX_FIFO_Count)) < uFIFOTriggerLevel);	// Rx FIFO Level check
		
		while(1)
		{
			*uRecBuf++ = PCMInp16(rhs_ePort, rPCMRXFIFO);	//Data Transfer		
			uPcmDataSize -= 4;
			uRegValue = PCMInp32(rhs_ePort, rPCMFIFOSTAT);
			if (uRegValue & (PCMFIFO_Status_RX_FIFO_EMPTY) )
				break;
		}
	}

	uTempRegister &= ~PCM_RXFIFO_EN & ~PCMCTL_EN_PCMIF;
	PCMOutp32(rhs_ePort, rPCMCTRL, uTempRegister);
	UART_Printf("Rec Done!\n");
}

void PCM_PCMOutPolling(AUDIO_PORT rhs_ePort)
{
	u32 uTempRegister = 0, uFIFOCNT = 0;
	u32 uRegValue;
	u16* uRecBuf = g_oPCMInfor[rhs_ePort].m_uRecBufferStartAddr;
	s32 uPcmDataSize = g_oPCMInfor[rhs_ePort].m_uPcmSize;
	u32 uFIFOTriggerLevel = 0x8;
	u32 uSclkDiv, uSyncDiv;

	//PCM Clock Setting 
	PCM_GetClkValAndClkDir(rhs_ePort, &uSclkDiv, &uSyncDiv);
	PCMCLKCTL_SetDivideLogic(rhs_ePort, uSclkDiv, uSyncDiv);

	uTempRegister |= PCM_TXFIFO_EN  |PCMCTL_EN_PCMIF ;
	PCMOutp32(rhs_ePort, rPCMCTRL, uTempRegister);	
	UART_Printf("Now Playing....\n");

	while( uPcmDataSize > 0 )
	{
		while(1)
		{
			PCMOutp16(rhs_ePort, rPCMTXFIFO, *uRecBuf ++);							//Data Transfer		
			uPcmDataSize -= 4;
			uRegValue = PCMInp32(rhs_ePort, rPCMFIFOSTAT);
			if (uRegValue & (PCMFIFO_Status_TX_FIFO_FULL) )
				break;
		}
		
		do 
		{
			uFIFOCNT = PCMInp32(rhs_ePort, rPCMFIFOSTAT);
		} while ((uFIFOCNT & (PCMFIFO_Status_TX_FIFO_Count)) > uFIFOTriggerLevel);		// Rx FIFO Level check		
		
	}

	uTempRegister &= ~PCM_TXFIFO_EN & ~PCMCTL_EN_PCMIF;
	PCMOutp32(rhs_ePort, rPCMCTRL, uTempRegister);
	UART_Printf("Play Done!\n");
}

/////////////////////////////////////////////// ISR of PCM I/F ///////////////////////////////////////////////

void __irq Isr_PCMPORT0_PCMIn_DMADone(void)
{
	INTC_Disable(g_oPCMInfor[AUDIO_PORT0].m_uNumDma);

	UART_Printf("\nPCM In DMA Done.\n");
	g_uPcmRecDone[AUDIO_PORT0]  = 1;

	DMACH_ClearIntPending(&g_oPcmDma);
	DMACH_ClearErrIntPending(&g_oPcmDma);
	INTC_ClearVectAddr();	
}

void __irq Isr_PCMPORT0_PCMOut_DMADone(void)
{
	INTC_Disable(g_oPCMInfor[AUDIO_PORT0].m_uNumDma);
	DMAC_InitCh(g_oPCMInfor[AUDIO_PORT0].m_eDmaUnit, g_oPCMInfor[AUDIO_PORT0].m_eDmaCh, &g_oPcmDma);
	
	DMACH_ClearIntPending(&g_oPcmDma);
	DMACH_ClearErrIntPending(&g_oPcmDma);
	INTC_ClearVectAddr();	
	

	INTC_Enable(g_oPCMInfor[AUDIO_PORT0].m_uNumDma);

	DMACH_Setup(g_oPCMInfor[AUDIO_PORT0].m_eDmaCh, 0x0, (u32)g_oPCMInfor[AUDIO_PORT0].m_uRecBufferStartAddr, false, 
				g_oPCMInfor[AUDIO_PORT0].m_uPcmTxFifoAddr, true, HWORD, g_oPCMInfor[AUDIO_PORT0].m_uPcmSize/4, 
				HANDSHAKE, MEM, g_oPCMInfor[AUDIO_PORT0].m_eDreqSrc , SINGLE, &g_oPcmDma);	
	DMACH_Start(&g_oPcmDma);

}

void __irq Isr_PCMPORT1_PCMIn_DMADone(void)
{
	INTC_Disable(g_oPCMInfor[AUDIO_PORT1].m_uNumDma);

	UART_Printf("\nPCM In DMA Done.\n");
	g_uPcmRecDone[AUDIO_PORT1]  = 1;

	DMACH_ClearIntPending(&g_oPcmDma);
	DMACH_ClearErrIntPending(&g_oPcmDma);
	INTC_ClearVectAddr();	
}

void __irq Isr_PCMPORT1_PCMOut_DMADone(void)
{
	INTC_Disable(g_oPCMInfor[AUDIO_PORT1].m_uNumDma);
	DMAC_InitCh(g_oPCMInfor[AUDIO_PORT1].m_eDmaUnit, g_oPCMInfor[AUDIO_PORT1].m_eDmaCh, &g_oPcmDma);
	
	DMACH_ClearIntPending(&g_oPcmDma);
	DMACH_ClearErrIntPending(&g_oPcmDma);
	INTC_ClearVectAddr();	
	

	INTC_Enable(g_oPCMInfor[AUDIO_PORT1].m_uNumDma);

	DMACH_Setup(g_oPCMInfor[AUDIO_PORT1].m_eDmaCh, 0x0, (u32)g_oPCMInfor[AUDIO_PORT1].m_uRecBufferStartAddr, false, 
				g_oPCMInfor[AUDIO_PORT1].m_uPcmTxFifoAddr, true, HWORD, g_oPCMInfor[AUDIO_PORT1].m_uPcmSize/4, 
				HANDSHAKE, MEM, g_oPCMInfor[AUDIO_PORT1].m_eDreqSrc , SINGLE, &g_oPcmDma);	
	DMACH_Start(&g_oPcmDma);
}


void __irq Isr_PCMPORT0_PCMIn(void)
{
	u32 i, uPcmFifoStat; 

	INTC_Disable(g_oPCMInfor[AUDIO_PORT0].m_uNumInt);

	INTC_ClearVectAddr();
	PCM_ClearInt(AUDIO_PORT0);
	
	uPcmFifoStat = PCMInp32(AUDIO_PORT0, rPCMFIFOSTAT);
		
	for(i=0; i< ((uPcmFifoStat & 0x3f0)>>4); i++)
	{
		*(g_PcmDataPointer[AUDIO_PORT0]++) = (u16) PCMInp32(AUDIO_PORT0, rPCMRXFIFO);		

		if( (u32) g_PcmDataPointer[AUDIO_PORT0]  >=  ((u32)  g_oPCMInfor[AUDIO_PORT0].m_uRecBufferStartAddr +(u32)  g_oPCMInfor[AUDIO_PORT0].m_uPcmSize) )
			break;	
	}			

	if( (u32) g_PcmDataPointer[AUDIO_PORT0]  >= ( (u32) g_oPCMInfor[AUDIO_PORT0].m_uRecBufferStartAddr + (u32) g_oPCMInfor[AUDIO_PORT0].m_uPcmSize) )
		g_uPcmRecDone[AUDIO_PORT0] = 1;
			
	INTC_Enable(g_oPCMInfor[AUDIO_PORT0].m_uNumInt);

}

void __irq Isr_PCMPORT0_PCMOut(void)
{
	u32 i, uPcmFifoStat; 

	INTC_Disable(g_oPCMInfor[AUDIO_PORT0].m_uNumInt);

	INTC_ClearVectAddr();
	PCM_ClearInt(AUDIO_PORT0);
	
	uPcmFifoStat = PCMInp32(AUDIO_PORT0, rPCMFIFOSTAT);
	
		
	for(i=0; i< (32-((uPcmFifoStat & 0xfc000)>>14)); i++)
	{
		PCMOutp32(AUDIO_PORT0, rPCMTXFIFO, *(g_PcmDataPointer[AUDIO_PORT0]++) );		

		if( (u32) g_PcmDataPointer[AUDIO_PORT0]  >=  ( (u32) g_oPCMInfor[AUDIO_PORT0].m_uRecBufferStartAddr + (u32) g_oPCMInfor[AUDIO_PORT0].m_uPcmSize) )
			break;	
	}			

	if( (u32) g_PcmDataPointer[AUDIO_PORT0]  >=   ((u32) g_oPCMInfor[AUDIO_PORT0].m_uRecBufferStartAddr + (u32) g_oPCMInfor[AUDIO_PORT0].m_uPcmSize) )
		uPcmPlayDone[AUDIO_PORT0] = 1;
			
	INTC_Enable(g_oPCMInfor[AUDIO_PORT0].m_uNumInt);
}

void __irq Isr_PCMPORT1_PCMIn(void)
{
	u32 i, uPcmFifoStat; 

	INTC_Disable(g_oPCMInfor[AUDIO_PORT1].m_uNumInt);

	INTC_ClearVectAddr();
	PCM_ClearInt(AUDIO_PORT1);
	
	uPcmFifoStat = PCMInp32(AUDIO_PORT1, rPCMFIFOSTAT);
		
	for(i=0; i< ((uPcmFifoStat & 0x3f0)>>4); i++)
	{
		*(g_PcmDataPointer[AUDIO_PORT1]++) = (u16) PCMInp32(AUDIO_PORT1, rPCMRXFIFO);		

		if( (u32) g_PcmDataPointer[AUDIO_PORT1]  >=  ((u32)  g_oPCMInfor[AUDIO_PORT1].m_uRecBufferStartAddr +(u32)  g_oPCMInfor[AUDIO_PORT1].m_uPcmSize) )
			break;	
	}			

	if( (u32) g_PcmDataPointer[AUDIO_PORT1]  >= ( (u32) g_oPCMInfor[AUDIO_PORT1].m_uRecBufferStartAddr + (u32) g_oPCMInfor[AUDIO_PORT1].m_uPcmSize) )
		g_uPcmRecDone[AUDIO_PORT1] = 1;
			
	INTC_Enable(g_oPCMInfor[AUDIO_PORT1].m_uNumInt);

}

void __irq Isr_PCMPORT1_PCMOut(void)
{
	u32 i, uPcmFifoStat; 

	INTC_Disable(g_oPCMInfor[AUDIO_PORT1].m_uNumInt);

	INTC_ClearVectAddr();
	PCM_ClearInt(AUDIO_PORT1);
	
	uPcmFifoStat = PCMInp32(AUDIO_PORT1, rPCMFIFOSTAT);
	
		
	for(i=0; i< (32-((uPcmFifoStat & 0xfc000)>>14)); i++)
	{
		PCMOutp32(AUDIO_PORT1, rPCMTXFIFO, *(g_PcmDataPointer[AUDIO_PORT1]++));		

			if( (u32) g_PcmDataPointer[AUDIO_PORT1]  >=  ( (u32) g_oPCMInfor[AUDIO_PORT1].m_uRecBufferStartAddr + (u32) g_oPCMInfor[AUDIO_PORT1].m_uPcmSize) )
			break;	
	}			

	if( (u32) g_PcmDataPointer[AUDIO_PORT1]  >=   ((u32) g_oPCMInfor[AUDIO_PORT1].m_uRecBufferStartAddr + (u32) g_oPCMInfor[AUDIO_PORT1].m_uPcmSize) )
		uPcmPlayDone[AUDIO_PORT1] = 1;
			
	INTC_Enable(g_oPCMInfor[AUDIO_PORT1].m_uNumInt);
}

void __irq ISR_PCM_Port0TXFIFOEr(void)
{
	u32 urPCMIRQSTAT = PCMInp32(AUDIO_PORT0, rPCMIRQSTAT);
	u32 urPCMFIFOSTAT =  PCMInp32(AUDIO_PORT0, rPCMFIFOSTAT);
	INTC_Disable(g_oPCMInfor[AUDIO_PORT0].m_uNumInt);
	
	INTC_ClearVectAddr();
	PCM_ClearInt(AUDIO_PORT0);
	
	if ( urPCMIRQSTAT & PCMIRQ_Status_TX_FIFO_STARVE_ERROR)
	{
		UART_Printf("\n0x%x\n",urPCMIRQSTAT);
		UART_Printf("\n0x%x\n",urPCMFIFOSTAT);
		UART_Printf("Port 0 TX FIFO Starve!!\n");
		DMACH_Setup(g_oPCMInfor[AUDIO_PORT0].m_eDmaCh, 0x0, (u32)g_oPCMInfor[AUDIO_PORT0].m_uRecBufferStartAddr, false, 
				g_oPCMInfor[AUDIO_PORT0].m_uPcmTxFifoAddr, true, HWORD, g_oPCMInfor[AUDIO_PORT0].m_uPcmSize/4, 
				HANDSHAKE, MEM, g_oPCMInfor[AUDIO_PORT0].m_eDreqSrc , SINGLE, &g_oPcmDma);	
		//DMACH_Start(&g_oPcmDma);

	}

	else if ( urPCMIRQSTAT & PCMIRQ_Status_TX_FIFO_EMPTY)
	{
		UART_Printf("Port 0 TX FIFO Empty!!\n");
		//DMACH_Setup(g_oPCMInfor[AUDIO_PORT0].m_eDmaCh, 0x0, (u32)g_oPCMInfor[AUDIO_PORT0].m_uRecBufferStartAddr, false, 
		//		g_oPCMInfor[AUDIO_PORT0].m_uPcmTxFifoAddr, true, HWORD, g_oPCMInfor[AUDIO_PORT0].m_uPcmSize/4, 
		//		HANDSHAKE, MEM, g_oPCMInfor[AUDIO_PORT0].m_eDreqSrc , SINGLE, &g_oPcmDma);	
		DMACH_Start(&g_oPcmDma);
	}
	INTC_Enable(g_oPCMInfor[AUDIO_PORT0].m_uNumInt);

}

void __irq ISR_PCM_Port1TXStarveEr(void)
{
	u32 urPCMIRQSTAT = PCMInp32(AUDIO_PORT1, rPCMIRQSTAT);

	INTC_Disable(g_oPCMInfor[AUDIO_PORT1].m_uNumInt);
	
	INTC_ClearVectAddr();
	PCM_ClearInt(AUDIO_PORT1);

	if ( urPCMIRQSTAT & PCMIRQ_Status_TX_FIFO_STARVE_ERROR)
		UART_Printf("Port 1 TX FIFO Starve!!\n");

	INTC_Enable(g_oPCMInfor[AUDIO_PORT1].m_uNumInt);

}

void __irq ISR_PCM_Port0RXOverflowEr(void)
{
	u32 urPCMIRQSTAT = PCMInp32(AUDIO_PORT0, rPCMIRQSTAT);

	INTC_Disable(g_oPCMInfor[AUDIO_PORT0].m_uNumInt);

	
	INTC_ClearVectAddr();
	PCM_ClearInt(AUDIO_PORT0);
	if ( urPCMIRQSTAT & PCMIRQ_Status_RX_FIFO_OVERFLOW_ERROR)
		UART_Printf("\nPort 0 RX FIFO Overflow!!\n");

	INTC_Enable(g_oPCMInfor[AUDIO_PORT0].m_uNumInt);
}


void __irq ISR_PCM_Port1RXOverflowEr(void)
{
	u32 urPCMIRQSTAT = PCMInp32(AUDIO_PORT1, rPCMIRQSTAT);

	INTC_Disable(g_oPCMInfor[AUDIO_PORT1].m_uNumInt);

	
	INTC_ClearVectAddr();
	PCM_ClearInt(AUDIO_PORT1);
	
	if ( urPCMIRQSTAT & PCMIRQ_Status_RX_FIFO_OVERFLOW_ERROR)
		UART_Printf("\nPort 1 RX FIFO Overflow!!\n");

	INTC_Enable(g_oPCMInfor[AUDIO_PORT1].m_uNumInt);

}
/////////////////////////////////////////////// API of PCM I/F ///////////////////////////////////////////////

void PCMCTL_PutData(AUDIO_PORT rhs_ePort)
{
	u32 urPCMCTL = 0;
	urPCMCTL = PCMInp32(rhs_ePort, rPCMCTRL);
	urPCMCTL &= ~(0x3f << 13) & ~(0xf3 << 7) & ~(0x3 << 3);	
	urPCMCTL |= (g_oPCMInfor[rhs_ePort].m_uTXFIFODipstick & 0x3f) << 13 | (g_oPCMInfor[rhs_ePort].m_uRXFIFODipstick & 0x3f) << 7;
	if (g_oPCMInfor[rhs_ePort].m_ePCMMSBPosition == AFTER_PCMSYNC_HIGH)
		urPCMCTL |= (1 << 4) | (1 << 3);
	else 
		urPCMCTL &= ~(1 << 4) &  ~(1 << 3);		
	PCMOutp32(rhs_ePort, rPCMCTRL, urPCMCTL);
}

void PCMCTL_EnablePCMIF(AUDIO_PORT rhs_ePort,   u8 uActive)
{
	u32 urPCMCTL = 0;
	urPCMCTL = PCMInp32(rhs_ePort, rPCMCTRL);
	if (uActive == true)
		urPCMCTL |= PCMCTL_EN_PCMIF;
	else 
		urPCMCTL &= ~PCMCTL_EN_PCMIF;
	PCMOutp32(rhs_ePort, rPCMCTRL, urPCMCTL);
}

void PCMCTL_EnableDMA(AUDIO_PORT rhs_ePort,  PCM_IFMODE rhs_eIFMode, u8 uActive)
{
	u32 urPCMCTL = 0;
	urPCMCTL = PCMInp32(rhs_ePort, rPCMCTRL);
	if (rhs_eIFMode == PCM_TX)
	{
		if (uActive == true)
			urPCMCTL |= PCMCTL_EN_TX_DMA;
		else 
			urPCMCTL &= ~PCMCTL_EN_TX_DMA;
	}
	else if (rhs_eIFMode == PCM_RX)
	{
		if (uActive == true)
			urPCMCTL |= PCMCTL_EN_RX_DMA;
		else 
			urPCMCTL &= ~PCMCTL_EN_RX_DMA;
	}
	else if (rhs_eIFMode == PCM_TXRX)
	{
		if (uActive == true)
			urPCMCTL |= PCMCTL_EN_TX_DMA | PCMCTL_EN_RX_DMA;
		else 
			urPCMCTL &= ~PCMCTL_EN_TX_DMA & ~PCMCTL_EN_RX_DMA;
	}
	
	PCMOutp32(rhs_ePort, rPCMCTRL, urPCMCTL);
}

void PCMCTL_EnableFIFO(AUDIO_PORT rhs_ePort, PCM_IFMODE rhs_eIFMode, u8 uActive)
{
	u32 urPCMCTL = 0;
	urPCMCTL = PCMInp32(rhs_ePort, rPCMCTRL);
	if (rhs_eIFMode == PCM_TX)
	{
		if (uActive == true)
			urPCMCTL |= PCMCTL_EN_TX_FIFO;
		else 
			urPCMCTL &= ~PCMCTL_EN_TX_FIFO;
	}
	else if (rhs_eIFMode == PCM_RX)
	{
		if (uActive == true)
			urPCMCTL |= PCMCTL_EN_RX_FIFO;
		else 
			urPCMCTL &= ~PCMCTL_EN_RX_FIFO;
	}
	else if (rhs_eIFMode == PCM_TXRX)
	{
		if (uActive == true)
			urPCMCTL |= PCMCTL_EN_TX_FIFO | PCMCTL_EN_RX_FIFO;
		else 
			urPCMCTL &= ~PCMCTL_EN_TX_FIFO & ~PCMCTL_EN_RX_FIFO;
	}
	
	PCMOutp32(rhs_ePort, rPCMCTRL, urPCMCTL);
}



void PCMCLKCTL_SetDivideLogic(AUDIO_PORT rhs_ePort, u32 uSclkDiv, u32  uSyncDiv)
{
	u32 urPCMCLKCTL = 0;
	if (g_oPCMInfor[rhs_ePort].m_ePCMClkSrc == PCM_PCLK)
		urPCMCLKCTL = PCMCLK_EN_DivideLogic | (1 << 18) | (uSclkDiv << 9) | (uSyncDiv << 0);
	else 
		urPCMCLKCTL = PCMCLK_EN_DivideLogic | (0 << 18) | (uSclkDiv << 9) | (uSyncDiv << 0);
	PCMOutp32(rhs_ePort, rPCMCLKCTL, urPCMCLKCTL);
}

void PCMCLKCTL_ActiveDivideLogic(AUDIO_PORT rhs_ePort, u8 uActive)
{
	u32 urPCMCLKCTL = 0;
	urPCMCLKCTL = PCMInp32(rhs_ePort, rPCMCLKCTL);
	if (uActive == true)
		urPCMCLKCTL |= 1 << 19 ;
	else 
		urPCMCLKCTL &= ~(1 << 19);
	PCMOutp32(rhs_ePort, rPCMCLKCTL, urPCMCLKCTL);
}

void PCMCLKCTL_SetSclkDivVal(AUDIO_PORT rhs_ePort, u32 uSclkDiv)
{
	u32 urPCMCLKCTL = 0;
	urPCMCLKCTL = PCMInp32(rhs_ePort, rPCMCLKCTL);
	urPCMCLKCTL &= ~(0xff << 9);
	urPCMCLKCTL |= (uSclkDiv <<9);
	PCMOutp32(rhs_ePort, rPCMCLKCTL, urPCMCLKCTL);
}
void PCMCLKCTL_SetSyncDivVal(AUDIO_PORT rhs_ePort, u32 uSyncDiv)
{
	u32 urPCMCLKCTL = 0;
	urPCMCLKCTL = PCMInp32(rhs_ePort, rPCMCLKCTL);
	urPCMCLKCTL &= ~(0x1ff << 0);
	urPCMCLKCTL |= (uSyncDiv <<0);
	PCMOutp32(rhs_ePort, rPCMCLKCTL, urPCMCLKCTL);
}


void PCMIRQCTL_EnableInt(AUDIO_PORT rhs_ePort, u8 rhs_uActive)
{
	u32 urPCMIRQCTL = 0;
	urPCMIRQCTL = PCMInp32(rhs_ePort, rPCMIRQCTL);
	if (rhs_uActive == true)
	{
		urPCMIRQCTL |= PCMIRQ_Enable_IRQ_TO_ARM;
	}
	else 
	{
		urPCMIRQCTL &= ~PCMIRQ_Enable_IRQ_TO_ARM;
	}    	
		
	PCMOutp32(rhs_ePort, rPCMIRQCTL, (urPCMIRQCTL | g_oPCMInfor[rhs_ePort].m_uIntAspects));
}

void PCMIRQCTL_SetIRQMode(AUDIO_PORT rhs_ePort, PCM_INT rhs_ePcmInt,  u8 rhs_uActive)
{
	u32 urPCMIRQCTL = 0;
	urPCMIRQCTL = PCMInp32(rhs_ePort, rPCMIRQCTL);
	if (rhs_uActive == true)
	{
		if(rhs_ePcmInt == PCMIRQALL)					urPCMIRQCTL |= PCMIRQ_Enable_ALLPCMIRQ;
		if(rhs_ePcmInt == TRANSFER_DONE)				urPCMIRQCTL |= PCMIRQ_Enable_TransferDone;
		
		if (rhs_ePcmInt == TXFIFO_EMPTY)				urPCMIRQCTL |= PCMIRQ_Enable_TX_FIFO_EMPTY;
		if (rhs_ePcmInt == TXFIFO_ALMOST_EMPTY)		urPCMIRQCTL |= PCMIRQ_Enable_TX_FIFO_ALMOST_EMPTY;
		if (rhs_ePcmInt == TXFIFO_FULL)				urPCMIRQCTL |= PCMIRQ_Enable_TX_FIFO_FULL;
		if (rhs_ePcmInt == TXFIFO_ALMOST_FULL)		urPCMIRQCTL |= PCMIRQ_Enable_TX_FIFO_ALMOST_FULL;
		if (rhs_ePcmInt == TXFIFO_ERROR_STARVE)		urPCMIRQCTL |= PCMIRQ_Enable_TX_FIFO_STARVE_ERROR;
		if (rhs_ePcmInt == TXFIFO_ERROR_OVERFLOW)  	urPCMIRQCTL |= PCMIRQ_Enable_TX_FIFO_OVERFLOW_ERROR;
			
		if (rhs_ePcmInt == RXFIFO_EMPTY)				urPCMIRQCTL |= PCMIRQ_Enable_RX_FIFO_EMPTY;
		if (rhs_ePcmInt == RXFIFO_ALMOST_EMPTY)		urPCMIRQCTL |= PCMIRQ_Enable_RX_FIFO_ALMOST_EMPTY;
		if (rhs_ePcmInt == RX_FIFO_FULL)				urPCMIRQCTL |= PCMIRQ_Enable_RX_FIFO_FULL;
		if (rhs_ePcmInt == RX_FIFO_ALMOST_FULL)		urPCMIRQCTL |= PCMIRQ_Enable_RX_FIFO_ALMOST_FULL;
		if (rhs_ePcmInt == RXFIFO_ERROR_STARVE)		urPCMIRQCTL |= PCMIRQ_Enable_RX_FIFO_STARVE_ERROR;
		if (rhs_ePcmInt == RXFIFO_ERROR_OVERFLOW)	urPCMIRQCTL |= PCMIRQ_Enable_RX_FIFO_OVERFLOW_ERROR;
	}
	else 
	{
		if(rhs_ePcmInt == PCMIRQALL)					urPCMIRQCTL &= ~PCMIRQ_Enable_ALLPCMIRQ;
	  	if(rhs_ePcmInt == TRANSFER_DONE)				urPCMIRQCTL &= ~PCMIRQ_Enable_TransferDone;
		
		if (rhs_ePcmInt == TXFIFO_EMPTY)				urPCMIRQCTL &= ~PCMIRQ_Enable_TX_FIFO_EMPTY;
		if (rhs_ePcmInt == TXFIFO_ALMOST_EMPTY)		urPCMIRQCTL &= ~PCMIRQ_Enable_TX_FIFO_ALMOST_EMPTY;
		if (rhs_ePcmInt == TXFIFO_FULL)				urPCMIRQCTL &= ~PCMIRQ_Enable_TX_FIFO_FULL;
		if (rhs_ePcmInt == TXFIFO_ALMOST_FULL)		urPCMIRQCTL &= ~PCMIRQ_Enable_TX_FIFO_ALMOST_FULL;
		if (rhs_ePcmInt == TXFIFO_ERROR_STARVE)		urPCMIRQCTL &= ~PCMIRQ_Enable_TX_FIFO_STARVE_ERROR;
		if (rhs_ePcmInt == TXFIFO_ERROR_OVERFLOW)  	urPCMIRQCTL &= ~PCMIRQ_Enable_TX_FIFO_OVERFLOW_ERROR;
			
		if (rhs_ePcmInt == RXFIFO_EMPTY)				urPCMIRQCTL &= ~PCMIRQ_Enable_RX_FIFO_EMPTY;
		if (rhs_ePcmInt == RXFIFO_ALMOST_EMPTY)		urPCMIRQCTL &= ~PCMIRQ_Enable_RX_FIFO_ALMOST_EMPTY;
		if (rhs_ePcmInt == RX_FIFO_FULL)				urPCMIRQCTL &= ~PCMIRQ_Enable_RX_FIFO_FULL;
		if (rhs_ePcmInt == RX_FIFO_ALMOST_FULL)		urPCMIRQCTL &= ~PCMIRQ_Enable_RX_FIFO_ALMOST_FULL;
		if (rhs_ePcmInt == RXFIFO_ERROR_STARVE)		urPCMIRQCTL &= ~PCMIRQ_Enable_RX_FIFO_STARVE_ERROR;
		if (rhs_ePcmInt == RXFIFO_ERROR_OVERFLOW)	urPCMIRQCTL &= ~PCMIRQ_Enable_RX_FIFO_OVERFLOW_ERROR;
	}
	g_oPCMInfor[rhs_ePort].m_uIntAspects = urPCMIRQCTL;
	PCMOutp32(rhs_ePort, rPCMIRQCTL, urPCMIRQCTL);
}

void PCMIRQCTL_GetIRQMode(AUDIO_PORT rhs_ePort)
{
	u32 urPCMIRQCTL = 0;
	urPCMIRQCTL = PCMInp32(rhs_ePort, rPCMIRQCTL);
	
	UART_Printf("00. Transfer Done : ");	
	if( (urPCMIRQCTL & PCMIRQ_Enable_TransferDone) != 0)
		UART_Printf("O\n");		
	else	
		UART_Printf("X\n");

	UART_Printf("1X. TX FIFO State : \n");
	UART_Printf("10. EMPTY : ");	
	if( (urPCMIRQCTL & PCMIRQ_Enable_TX_FIFO_EMPTY) != 0)
		UART_Printf("O\t");		
	else	
		UART_Printf("X\t");

	UART_Printf("11. ALMOTY EMPTY : ");	
	if( (urPCMIRQCTL & PCMIRQ_Enable_TX_FIFO_ALMOST_EMPTY) != 0)
		UART_Printf("O\t");		
	else	
		UART_Printf("X\t");

	UART_Printf("12. FULL : ");	
	if( (urPCMIRQCTL & PCMIRQ_Enable_TX_FIFO_FULL) != 0)
		UART_Printf("O\t");		
	else	
		UART_Printf("X\t");

	UART_Printf("13. ALMOST FULL : ");	
	if( (urPCMIRQCTL & PCMIRQ_Enable_TX_FIFO_ALMOST_FULL) != 0)
		UART_Printf("O\t");		
	else	
		UART_Printf("X\t");

	UART_Printf("14. STARVE ERROR : ");	
	if( (urPCMIRQCTL & PCMIRQ_Enable_TX_FIFO_STARVE_ERROR) != 0)
		UART_Printf("O\t");		
	else	
		UART_Printf("X\t");

	UART_Printf("15. OVERFLOW ERROR : ");	
	if( (urPCMIRQCTL & PCMIRQ_Enable_TX_FIFO_OVERFLOW_ERROR) != 0)
		UART_Printf("O\n");		
	else	
		UART_Printf("X\n");
	
	UART_Printf("2X. RX FIFO State : \n");
	UART_Printf("20. EMPTY : ");	
	if( (urPCMIRQCTL & PCMIRQ_Enable_RX_FIFO_EMPTY) != 0)
		UART_Printf("O\t");		
	else	
		UART_Printf("X\t");

	
	UART_Printf("21. ALMOST EMPTY : ");	
	if( (urPCMIRQCTL & PCMIRQ_Enable_RX_FIFO_ALMOST_EMPTY) != 0)
		UART_Printf("O\t");		
	else	
		UART_Printf("X\t");

	UART_Printf("22. FULL : ");	
	if( (urPCMIRQCTL & PCMIRQ_Enable_RX_FIFO_FULL) != 0)
		UART_Printf("O\t");		
	else	
		UART_Printf("X\t");

	UART_Printf("23. ALMOST FULL : ");	
	if( (urPCMIRQCTL & PCMIRQ_Enable_RX_FIFO_ALMOST_FULL) != 0)
		UART_Printf("O\t");		
	else	
		UART_Printf("X\t");

	UART_Printf("24. STARVE ERROR:: ");	
	if( (urPCMIRQCTL & PCMIRQ_Enable_RX_FIFO_STARVE_ERROR) != 0)
		UART_Printf("O\t");		
	else	
		UART_Printf("X\t");

	UART_Printf("25. OVERFLOW ERROR:: ");	
	if( (urPCMIRQCTL & PCMIRQ_Enable_RX_FIFO_OVERFLOW_ERROR) != 0)
		UART_Printf("O\n");		
	else	
		UART_Printf("X\n");		
}

void PCM_ClearInt(AUDIO_PORT rhs_ePort)
{
	u32 uPcmClrInt;

	uPcmClrInt = 0x1;
		
	PCMOutp32(rhs_ePort, rPCMCLRINT, uPcmClrInt);
}
void PCM_SelCLKAUDIO(AUDIO_PORT ePort, PCM_CLKSRC eClkSrc)
{
		u32 uClkSrc, uEpllCon0;

		uClkSrc = Inp32SYSC(0x1C);
		uEpllCon0 = Inp32SYSC(0x14);

		if(ePort == AUDIO_PORT0)
		{
			if(eClkSrc == PCM_MOUT_EPLL) 
			{
				Outp32SYSC(0x14, uEpllCon0 |(1<<31));
				Delay(100);
				Outp32SYSC(0x1C, uClkSrc & ~(0x7<<7) |(1<<2));	
			}
			else if (eClkSrc == PCM_DOUT_MPLL) 	Outp32SYSC(0x1C, uClkSrc & ~(0x7<<7)|(1<<7));
			else if (eClkSrc == PCM_FIN_EPLL) 		Outp32SYSC(0x1C, uClkSrc & ~(0x7<<7)|(2<<7));
			else if (eClkSrc == PCM_PCMCDCLK) 	Outp32SYSC(0x1C, uClkSrc & ~(0x7<<7)|(4<<7));
		}

		else if(ePort == AUDIO_PORT1)
		{
			if(eClkSrc == PCM_MOUT_EPLL) 
			{
				Outp32SYSC(0x14, uEpllCon0 |(1<<31));
				Delay(100);
				Outp32SYSC(0x1C, uClkSrc & ~(0x7<<10)|(1<<2));
			}
			else if (eClkSrc == PCM_DOUT_MPLL)	Outp32SYSC(0x1C, uClkSrc & ~(0x7<<10)|(1<<10));
			else if (eClkSrc == PCM_FIN_EPLL) 		Outp32SYSC(0x1C, uClkSrc & ~(0x7<<10)|(2<<10));
			else if (eClkSrc == PCM_PCMCDCLK) 	Outp32SYSC(0x1C, uClkSrc & ~(0x7<<10)|(4<<10));
		}
}


void PCM_GetClkValAndClkDir(AUDIO_PORT rhs_ePort, u32* uSclkDiv, u32* uSyncDiv)
{
	double dTempVal, dVclkSrc, dVclkSync;

//	extern u32		g_MPLL, g_ARMCLK, g_HCLK, g_PCLK, g_MCLK;
	
	if(g_oPCMInfor[rhs_ePort].m_ePCMClkSrc == PCM_MOUT_EPLL)
		dVclkSrc = PCMGetEPLL(); 					//should be changed according to your system clock condition : 8800000*3.35
	else if(g_oPCMInfor[rhs_ePort].m_ePCMClkSrc == PCM_DOUT_MPLL)
		dVclkSrc = PCMGetMPLL(); 					//should be changed according to your system clock condition : 266000000
	else if(g_oPCMInfor[rhs_ePort].m_ePCMClkSrc == PCM_FIN_EPLL)
		dVclkSrc =  FIN; 	//should be changed according to your system clock condition
	else if(g_oPCMInfor[rhs_ePort].m_ePCMClkSrc == PCM_PCMCDCLK)
		dVclkSrc = PCMGetEPLL();//12288000;		// LoopBack From Another Port
	else if(g_oPCMInfor[rhs_ePort].m_ePCMClkSrc == PCM_PCLK)
		dVclkSrc = g_PCLK;							//should be changed according to your system clock condition
		
	
	//UART_Printf("Source CLk : %3.1fKHz\n",dVclkSrc/1000);
	//UART_Printf("User Set PCMSCLK : %3.1fKHz\n",(double)g_oPCMInfor[rhs_ePort].m_uPCMSClk/1000);
	//UART_Printf("User Set PCMSYNC(Sampling Frequency : %3.1fKHz\n",(double)g_oPCMInfor[rhs_ePort].m_uPCMSync/1000);
	
	dTempVal = dVclkSrc/(double)(2*g_oPCMInfor[rhs_ePort].m_uPCMSClk) - 1;

	if ( ((int)(dTempVal*10) % 10) < 5)		//Round off 
		dTempVal = dTempVal-1;
	
	*uSclkDiv = (int)(dTempVal + 1);
	dVclkSync = dVclkSrc/(2 * ( (*uSclkDiv) + 1));
	//UART_Printf("SCLK DIV : %d\n",*uSclkDiv);
	//UART_Printf("Current PCMSCLK : %3.1fKHz\n", dVclkSync/1000);
	

	dTempVal = (double)g_oPCMInfor[rhs_ePort].m_uPCMSClk / (double)g_oPCMInfor[rhs_ePort].m_uPCMSync - 1;

	if ( ((int)(dTempVal*10) % 10) < 5)		//Round off 
		dTempVal = dTempVal-1;

	*uSyncDiv = (int)(dTempVal + 1);
	//UART_Printf("SCLK DIV : %d\n",*uSyncDiv);
	//UART_Printf("Current PCMSCLK : %3.1fKHz\n", (dVclkSync/( ( (*uSyncDiv) + 1) ))/1000);	

 }

double PCMGetEPLL(void)
{
	u8 muxEpll;
	u16 uPdiv, uMdiv, uSdiv, uKdiv;
	double dTemp;	

	SYSC_CtrlCLKOUT(eCLKOUT_EPLLOUT,  0);
	GPIO_SetFunctionEach(eGPIO_F, eGPIO_14, 3);
	GPIO_SetPullUpDownEach(eGPIO_F, eGPIO_14, 0);
	
	muxEpll = (Inp32SYSC(0x01c) >> 2) & 0x1;
	uMdiv = (Inp32SYSC(0x014) >> 16) & 0xff;
	uPdiv = (Inp32SYSC(0x014) >> 8) & 0x3f;
	uSdiv = (Inp32SYSC(0x014) >> 0) & 0x7;
	uKdiv =( (Inp32SYSC(0x018)) & 0xffff);		
	dTemp = (double)(((FIN/((double)uPdiv * (1 << uSdiv)))*((double) uMdiv+(double) ((double)uKdiv / 65536))));
	if (muxEpll == 0x1)
		return dTemp;
	else
		return FIN;	
}

double PCMGetMPLL(void)
{
	u8 muxMpll;
	u16 uPdiv, uMdiv, uSdiv;
	u16 uDivMPLL;
	double temp;
	
	muxMpll = (Inp32SYSC(0x01c) >> 1) & 0x1;
	uMdiv = (Inp32SYSC(0x010) >> 16) & 0x3ff;
	uPdiv = (Inp32SYSC(0x010) >> 8) & 0x3f;
	uSdiv = (Inp32SYSC(0x010) >> 0) & 0x7;
	uDivMPLL = (Inp32SYSC(0x020) >> 4) & 0x1;	
	
	temp = ((FIN/((double)uPdiv * (1 << uSdiv)))*((double) uMdiv));
	temp = temp  / (uDivMPLL+1);
	if (muxMpll == 0x1)
		return temp;
	else
		return (FIN/(uDivMPLL+1));	
}

void PCMOutp32(AUDIO_PORT rhs_ePort, PCM_SFR offset, u32 x) 
{
	if (rhs_ePort == AUDIO_PORT0)
		Outp32(PCM0_BASE+offset, x);
	else
		Outp32(PCM1_BASE+offset, x);
}

void PCMOutp16(AUDIO_PORT rhs_ePort, PCM_SFR offset, u32 x) 
{
	if (rhs_ePort == AUDIO_PORT0)
		Outp16(PCM0_BASE+offset, x);
	else
		Outp16(PCM1_BASE+offset, x);
}

u32  PCMInp32(AUDIO_PORT rhs_ePort, PCM_SFR offset)
{
	if (rhs_ePort == AUDIO_PORT0)
		return Inp32(PCM0_BASE+offset);
	else 
		return Inp32(PCM1_BASE+offset);
}

u16 PCMInp16(AUDIO_PORT rhs_ePort, PCM_SFR offset)
{
	if (rhs_ePort == AUDIO_PORT0)
		return Inp16(PCM0_BASE+offset);
	else
		return Inp16(PCM1_BASE+offset);
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PCM_CODEC_IICWrite(u32 uSlaveAddr, u32 uAddr, u8 uData)
{
	u32 i;
	u8 uI2cData[2];
	s32 uI2cDataCount;
	u32 uI2cPt;	

	#if ( PCM_CODEC == AK2430)
		

		uI2cData[0]   = (unsigned char)uAddr;
		uI2cData[1]   = uData;
		uI2cDataCount = 2;
		uI2cPt = 0;

		IICOutp32(rIICCON0, (1<<7) | (0<<6) | (1<<5) | (0xf)); //Enable ACK, Prescaler IICCLK=PCLK/16, Enable interrupt, Transmit clock value Tx clock=IICCLK/16
		IICOutp32(rIICSTAT0, 0x10);	//IIC bus data output enable(Rx/Tx)
		IICOutp32(rIICLC0, (1<<2)|(1)); 	//Filter enable, 5 clocks SDA output delay    //

		//Data Write Phase
		IICOutp32(rIICDS0, uSlaveAddr); //0xa0
		IICOutp32(rIICSTAT0, 0xf0);      

		while(uI2cDataCount != -1)
		{
			if(IICInp32(rIICCON0)& 0x10)
			{                  

				if((uI2cDataCount--)==0)
				{
					IICOutp32(rIICSTAT0, 0xd0);	//stop MasTx condition 
					IICOutp32(rIICCON0, 0xaf);   //resumes IIC operation.

					Delay(1);  //wait until stop condtion is in effect.
					break;    
				}

			IICOutp32(rIICDS0, uI2cData[uI2cPt++]); 

			for(i=0;i<10;i++);	//for setup time until rising edge of IICSCL
				IICOutp32(rIICCON0, 0xaf); 	//resumes IIC operation.

			}      
		}

		IICOutp32(rIICSTAT0, 0xd0); 	//Master Tx condition, Stop(Write), Output Enable 
		IICOutp32(rIICCON0, 0xaf);	//Resumes IIC operation. 

		Delay(1);  //Wait until stop condtion is in effect.      
	
	
	#elif ( PCM_CODEC == WM8753)	

		uI2cData[0]   = (unsigned char)uAddr;
		uI2cData[1]   = uData;
		uI2cDataCount = 2;
		uI2cPt = 0;

		IICOutp32(rIICCON0, (1<<7) | (0<<6) | (1<<5) | (0xf)); 	//Enable ACK, Prescaler IICCLK=PCLK/16, Enable interrupt, Transmit clock value Tx clock=IICCLK/16
		IICOutp32(rIICSTAT0, 0x10);							//IIC bus data output enable(Rx/Tx)
		IICOutp32(rIICLC0, (1<<2)|(3)); 						//Filter enable, 15 clocks SDA output delay    //

		//Data Write Phase
		IICOutp32(rIICDS0, uSlaveAddr); 	//0xa0
		IICOutp32(rIICSTAT0, 0xf0);      

		while(uI2cDataCount != -1)
		{
			if(IICInp32(rIICCON0)& 0x10)
			{                  

				if((uI2cDataCount--)==0)
				{
					IICOutp32(rIICSTAT0, 0xd0);		//stop MasTx condition 
					IICOutp32(rIICCON0, 0xaf);  		//resumes IIC operation.

					Delay(1);  //wait until stop condtion is in effect.
					break;    
				}

				IICOutp32(rIICDS0, uI2cData[uI2cPt++]); 

				for(i=0;i<10;i++);	//for setup time until rising edge of IICSCL
					IICOutp32(rIICCON0, 0xaf); 	//resumes IIC operation.

			}      
		}

		IICOutp32(rIICSTAT0, 0xd0); 	//Master Tx condition, Stop(Write), Output Enable 
		IICOutp32(rIICCON0, 0xaf);	//Resumes IIC operation. 

		Delay(1);  //Wait until stop condtion is in effect.      
	
	
	#else
		Assert(0);
	
	#endif

}


u8 PCM_CODEC_IICRead(u32 uSlaveAddr, u8 uAddr)
{
	u8  uI2cData;
	s32 uI2cDataCount;
	u32 i;

	#if ( PCM_CODEC == AK2430)	

		uI2cData  = uAddr;
		uI2cDataCount = 1;

		//Register Address Write Phase
		IICOutp32(rIICCON,0 (1<<7) | (0<<6) | (1<<5) | (0<<4) | (0xf));//Enable ACK, Prescaler IICCLK=PCLK/16, Enable interrupt, Transmit clock value Tx clock=IICCLK/16
		IICOutp32(rIICSTAT0, 0x10); //IIC bus data output enable(Rx/Tx)	
		IICOutp32(rIICLC0, (1<<2)|(1)); //Filter enable, 15 clocks SDA output delay    		

		IICOutp32(rIICDS0, uSlaveAddr);
		IICOutp32(rIICSTAT0, 0xf0);	//MasTx,Start 


		while(uI2cDataCount!=-1)
		{
			if(IICInp32(rIICCON0) & 0x10) 
			{	
				//Tx/Rx Interrupt Enable
				if((uI2cDataCount--)==0)
				{
					break;                
				}
				IICOutp32(rIICDS0, uI2cData);
				for(i=0;i<10;i++); 	//for setup time until rising edge of IICSCL
					IICOutp32(rIICCON0, 0xaf);	//resumes IIC operation.	
			}
		}   

		uI2cDataCount = 1;
		IICOutp32(rIICCON0, (1<<7) | (0<<6) | (1<<5) | (0<<4) | (0xf));

		//Register Data Read Phase 
		IICOutp32(rIICDS0, uSlaveAddr);

		IICOutp32(rIICSTAT0, 0xb0);	//Master Rx,Start

		while(uI2cDataCount!=-1)
		{
			if(IICInp32(rIICCON0) & 0x10)	//Interrupt pending 
			{
				if((uI2cDataCount--)==0)
				{
					uI2cData = IICInp32(rIICDS0);

					IICOutp32(rIICSTAT0, 0x90);		//Stop MasRx condition 
					IICOutp32(rIICCON0, 0xaf);       //Resumes IIC operation.

					Delay(1);                    //Wait until stop condtion is in effect.
					//Too long time... 
					//The pending bit will not be set after issuing stop condition.
					break;    
				}

				if((uI2cDataCount)==0)
					IICOutp32(rIICCON0, 0x2f);		//Resumes IIC operation with NOACK. 
				else 
					IICOutp32(rIICCON0, 0xaf);		//Resumes IIC operation with ACK

			}
		}


		return  uI2cData;
	

	#elif ( PCM_CODEC == WM8753)	
	
		uI2cData  = uAddr;
		uI2cDataCount = 1;

		//Register Address Write Phase
		IICOutp32(rIICCON0, (1<<7) | (0<<6) | (1<<5) | (0<<4) | (0xf));//Enable ACK, Prescaler IICCLK=PCLK/16, Enable interrupt, Transmit clock value Tx clock=IICCLK/16
		IICOutp32(rIICSTAT0, 0x10); //IIC bus data output enable(Rx/Tx)	
		IICOutp32(rIICLC0, (1<<2)|(1)); //Filter enable, 15 clocks SDA output delay    		

		IICOutp32(rIICDS0, uSlaveAddr);
		IICOutp32(rIICSTAT0, 0xf0);	//MasTx,Start 


		while(uI2cDataCount!=-1)
		{
			if(IICInp32(rIICCON0) & 0x10) 
			{	//Tx/Rx Interrupt Enable
				if((uI2cDataCount--)==0)
				{
					break;                
				}
					IICOutp32(rIICDS0, uI2cData);
					for(i=0;i<10;i++); 	//for setup time until rising edge of IICSCL
						IICOutp32(rIICCON0, 0xaf);	//resumes IIC operation.	
				}
		}   

		uI2cDataCount = 1;
		IICOutp32(rIICCON0, (1<<7) | (0<<6) | (1<<5) | (0<<4) | (0xf));

		//Register Data Read Phase 
		IICOutp32(rIICDS0, uSlaveAddr);
		IICOutp32(rIICSTAT0, 0xb0);	//Master Rx,Start

		while(uI2cDataCount!=-1)
		{
			if(IICInp32(rIICCON0) & 0x10)	//Interrupt pending 
			{
				if((uI2cDataCount--)==0)
				{
					uI2cData = IICInp32(rIICDS0);

					IICOutp32(rIICSTAT0, 0x90);		//Stop MasRx condition 
					IICOutp32(rIICCON0, 0xaf);       //Resumes IIC operation.

					Delay(1);                    //Wait until stop condtion is in effect.
					//Too long time... 
					//The pending bit will not be set after issuing stop condition.
					break;    
				}

				if((uI2cDataCount)==0)
					IICOutp32(rIICCON0, 0x2f);		//Resumes IIC operation with NOACK. 
				else 
					IICOutp32(rIICCON0, 0xaf);		//Resumes IIC operation with ACK
	
			}
		}

		return  uI2cData;
	
	
	#else	
		Assert(0);
	#endif


}

//////////
// Function Name : 
// Function Desctiption : 
// Input : Sync Rate, MSB Position, SCLK, CLKSRC
// Output : None
// Version : 0.0
// Author : Yoh-Han Lee
 

void PCM_CodecInitPCMIn(u32 eSync, u32 eSclk, SerialDataFormat eMSBPos,  PCM_CLKSRC eClkSrc)
{
	#if ( PCM_CODEC_NAME == AK2430)
	
	if(eSclk == 128000)
		uCodecSet = 0x0;
	else if(eSclk == 256000)
		uCodecSet = 0x1;
	else if(eSclk == 512000)
		uCodecSet = 0x2;

	PCM_CODEC_IICWrite(0x9e, 0x18, 0x07);	//MSTCLK_I = 19.2MHz

	if(eClkSrc ==PCM_PCMCDCLK)
		PCM_CODEC_IICWrite(0x9e, 0x1B, 0x0);	//PCM Codec Master Mode
	else
		PCM_CODEC_IICWrite(0x9e, 0x1B, 0x6);	//PCM Codec Slave Mode

	PCM_CODEC_IICWrite(0x9e, 0x1E, uCodecSet);	//DLCK = 128K,256kHz, or 512K, 14bit linear code (2's complement  format)

	PCM_CODEC_IICWrite(0x9e, 0x0D, 0x5);	//0 dB
	PCM_CODEC_IICWrite(0x9e, 0x06, 0x40);	//POP Mute Off

	PCM_CODEC_IICWrite(0x9e, 0x03, 0x01);	//CKI Buff On other blocks Off
	Delay(1);

	PCM_CODEC_IICWrite(0x9e, 0x03, 0x09);	//PLL2, CKI Buff On and other blocks Off
	Delay(300);

	PCM_CODEC_IICWrite(0x9e, 0x05, 0x10);	//TXSUM On and other blocks Off
	PCM_CODEC_IICWrite(0x9e, 0x05, 0x30);	//PCM Codec, TX Sum On and other blocks Off
	Delay(50);

	PCM_CODEC_IICWrite(0x9e, 0x0B, 0x11);	//sw_COI, sw_TX1 Close and other sw Open
	PCM_CODEC_IICWrite(0x9e, 0x05, 0x35);	//MIC AMP1, MIC BIAS1, PCM CODEC, TX SUM On and other bolcks OFF
	Delay(1600);
	
	#elif ( PCM_CODEC_NAME == WM9713)
	
	if(AC97_InitACLINK())
	{

		AC97_CodecCmd(WRITE,0x26, 0x4f00);		// Enable PR5(Internal Clock, AC-link I/F)
		AC97_CodecCmd(WRITE,0x26, 0x4700);		// Enable PR3(VREF, I/P PGA's, DAC's, ADC's, Mixer, O/P's)

		AC97_CodecCmd(WRITE,0x3C, 0xfbff);		// Enable MBIAS generator

		Delay(1000);
		
	#if 1
		AC97_CodecCmd(WRITE,0x26, 0x4300);		// Enable I/P PGA's nad Mixers
		AC97_CodecCmd(WRITE,0x3C, 0xfbcf);		// Enable ADC L 0xfbcf

		AC97_CodecCmd(WRITE,0x26, 0x4200);		// Enable Stereo ADC 
		AC97_CodecCmd(WRITE,0x26, 0x0200);		// Enable PR6 (O/P PGA's)

		//AC97_CodecCmd(WRITE,0x3E, 0xffbf);		// Enable LINE L PGA
		
		if (AUDIOIN == LINEIN)
			AC97_CodecCmd(WRITE,0x3E, 0xffaf);		// Enable LINE L/R PGA
		else if (AUDIOIN == MICIN)
			AC97_CodecCmd(WRITE,0x3E, 0xfff0);		// Enable MIC L/R PGA
		
		AC97_CodecCmd(WRITE,0x2A,0x1);			//Variable Rate Enable	
		UART_Printf("VRA Enable(1)/Disable(0): 0x%x\n",(0x1&AC97_CodecCmd(READ,0x2A,0x0001)));
		AC97_CodecCmd(WRITE,0x32, 0x1f40);		// ADC SR = 8kHz
		//AC97_CodecCmd(WRITE,0x14, 0xfe52);		// Record Mux Source Selection: LINE L/R			//11 111 11 0 0 1 010 010	 =  1111 1110 0101 0010
		if (AUDIOIN == LINEIN)
		{
			AC97_CodecCmd(WRITE,0x14, 0xfe52);		// Record Mux Source Selection: LINE L/R	 0xfe12
		}
		else if (AUDIOIN == MICIN)
		{
			AC97_CodecCmd(WRITE,0x22, 0x4040);		//MIC Preamp Contorl : MIC2A only, 12dB, MICBias
			AC97_CodecCmd(WRITE,0x10, 0x68);			//Unmute MIC routing : unmute MICA, MICA only 
			AC97_CodecCmd(WRITE,0x14, 0xfe00);		//Record Mux Source Selection: MICA
		}

		AC97_CodecCmd(WRITE,0x12, 0x1010);
		AC97_CodecCmd(WRITE,0x04,0x0707);
		AC97_CodecCmd(WRITE,0x12, 0x1010);		// Unmute ADC and Set ADC Recoding Volume
		
		//AC97_CodecCmd(WRITE,0x5C, 0x0008);	// ADC High-Pass Filter On -> this function remove noise where is no input
		//AC97_CodecCmd(WRITE,0x5A, 0x0020);	// HPF Corner Freq.
		
		//AC97_CodecCmd(WRITE,0x60, 0xf0f0); 		//ALC control 1111 1111 1111 1111
		AC97_CodecCmd(WRITE,0x62, 0x04fbf);		//ALC On : Upper 2 bytes are relate on ALC and Lower 2 bytes are relate on Noise Gate 
		

		AC97_CodecCmd(WRITE,0x12, 0x3f00);		// Unmute ADC and Set ADC Recoding Volume	

		if (eMSBPos == DURING_PCMSYNC_HIGH)	
			AC97_CodecCmd(WRITE,0x36, 0xa6b3);	// PCM IF=DSP B type, 16Bit, Slave Mode, Fs=8kHz		1010 0110 1110 0011
		else 										//AFTER_PCMSYNC_HIGH = Dsp  Mode A
			AC97_CodecCmd(WRITE,0x36, 0xa6a3);	// PCM IF=DSP A type, 16Bit, Slave Mode, Fs=8kHz		1010 0110 1010 0011
		//AC97_CodecCmd(WRITE,0x44, 0x0b80);	// PCM CLK = 8kHz									0000 1011 1000 0000
		AC97_CodecCmd(WRITE,0x4C, 0xffde);			// GPIO config. For PCM IF (Slave mode) 

	#else
		AC97_CodecCmd(WRITE,0x42, 0x0000);
		AC97_CodecCmd(WRITE,0x26, 0x4300);
		AC97_CodecCmd(WRITE,0x3c, 0xebde);
		AC97_CodecCmd(WRITE,0x26, 0x4200);
		AC97_CodecCmd(WRITE,0x26, 0x020d);
		AC97_CodecCmd(WRITE,0x3e, 0xdfef);
		AC97_CodecCmd(WRITE,0x32, 0x1f40);
		AC97_CodecCmd(WRITE,0x08, 0xdf40);
		AC97_CodecCmd(WRITE,0x12, 0x0000);
		AC97_CodecCmd(WRITE,0x14, 0xfe1b);
		AC97_CodecCmd(WRITE,0x18, 0xff20);
		AC97_CodecCmd(WRITE,0x1c, 0x8000);
		if (g_oPCMState.PCMMSBPosition == DURING_PCMSYNC_HIGH)	
			AC97_CodecCmd(WRITE,0x36, 0xa6e3);		// PCM IF=DSP B type, 16Bit, Slave Mode, Fs=8kHz		1010 0110 1110 0011
		else 															//AFTER_PCMSYNC_HIGH = Dsp  Mode A
			AC97_CodecCmd(WRITE,0x36, 0xa6a3);		// PCM IF=DSP A type, 16Bit, Slave Mode, Fs=8kHz		1010 0110 1010 0011
		AC97_CodecCmd(WRITE,0x44, 0x0b80);		// PCM CLK = 8kHz									0000 1011 1000 0000
		AC97_CodecCmd(WRITE,0x4C, 0xffde);		// GPIO config. For PCM IF (Slave mode) 
	#endif				
	}

	#elif ( PCM_CODEC_NAME == WM8753)
	
	//PCM_CODEC_IICWrite(0x34, 0x3e, 0x00);	//Reset
	PCM_CODEC_IICWrite(0x34, 0x1f, 0x00);	//Reset

	PCM_CODEC_IICWrite(0x34, 0x02, 0x08);	//DAC Control Mute
	PCM_CODEC_IICWrite(0x34, 0x04, 0x00);	//ADC Control
	
	if (eMSBPos == DURING_PCMSYNC_HIGH)
		PCM_CODEC_IICWrite(0x34, 0x07, 0xb3);	//PCM Audio I/F
	else 
		PCM_CODEC_IICWrite(0x34, 0x07, 0xa3);	//PCM Audio I/F
		
	PCM_CODEC_IICWrite(0x34, 0x08, 0x0a);	//HiFi Audio I/F
	PCM_CODEC_IICWrite(0x34, 0x0a, 0x00);	//Interface Control << Voice Codec >>

	//PCM_CODEC_IICWrite(0x34, 0x0c, 0x34);	//Sync Rate Control 
	if (eSync == 8000)
		PCM_CODEC_IICWrite(0x34, 0x0c, 0x0c);	//Sync Rate Control 8KHz
	else if (eSync == 16000)     
		PCM_CODEC_IICWrite(0x34, 0x0c, 0x14);	//Sync Rate Control 16KHz 
	else if (eSync == 32000)
		PCM_CODEC_IICWrite(0x34, 0x0c, 0x18);	//Sync Rate Control 32KHz 
	else if (eSync == 48000)
		PCM_CODEC_IICWrite(0x34, 0x0c, 0x00);	//Sync Rate Control 48KHz 
	else if (eSync == 96000)	
		PCM_CODEC_IICWrite(0x34, 0x0c, 0x1e);	//Sync Rate Control 96KHz 
		
	PCM_CODEC_IICWrite(0x34, 0x0e, 0x97); 	//Sample Rate Control (2), Don't care in case of slave mode
	PCM_CODEC_IICWrite(0x34, 0x10, 0xff); 	//Left DAC Volume
	PCM_CODEC_IICWrite(0x34, 0x12, 0xff); 	//Right DAC Volume
	PCM_CODEC_IICWrite(0x34, 0x18, 0x7b); 	//ALC 1
	PCM_CODEC_IICWrite(0x34, 0x1a, 0x00); 	//ALC 2
	PCM_CODEC_IICWrite(0x34, 0x1c, 0x32); 	//ALC 3
	PCM_CODEC_IICWrite(0x34, 0x1e, 0x00); 	//Noise Gate 
	PCM_CODEC_IICWrite(0x34, 0x21, 0xc3); 	//Left ADC Volume
	PCM_CODEC_IICWrite(0x34, 0x23, 0xc3); 	//Right ADC Volume
	PCM_CODEC_IICWrite(0x34, 0x24, 0xc0); 	//Additional Control
	PCM_CODEC_IICWrite(0x34, 0x26, 0x00); 	//Three D Control
	PCM_CODEC_IICWrite(0x34, 0x28, 0xc0); 	//Power Management(1) << Power up VMID[50K ohm]; VREF >>
	PCM_CODEC_IICWrite(0x34, 0x2a, 0x0c); 	//Power Management(2) << Power up ADCL/R >>
	PCM_CODEC_IICWrite(0x34, 0x2c, 0x00); 	//Power Management(3)
	PCM_CODEC_IICWrite(0x34, 0x2e, 0x00); 	//Power Management(4)
	PCM_CODEC_IICWrite(0x34, 0x30, 0x00); 	//ID Register
	PCM_CODEC_IICWrite(0x34, 0x32, 0x00); 	//Int. Polarty
	PCM_CODEC_IICWrite(0x34, 0x34, 0x00);	//Int. Enable
	PCM_CODEC_IICWrite(0x34, 0x36, 0x00);	//GPIO Control (1)
	PCM_CODEC_IICWrite(0x34, 0x38, 0x00);	//GPIO Control (2)

	PCM_CODEC_IICWrite(0x34, 0x40, 0x55);	//Record Mix (1)
	PCM_CODEC_IICWrite(0x34, 0x42, 0x05);	//Record Mix (2)
	PCM_CODEC_IICWrite(0x34, 0x44, 0x50);	//Left Out Mix(1)
	PCM_CODEC_IICWrite(0x34, 0x46, 0x55);	//Left Out Mix(2)
	PCM_CODEC_IICWrite(0x34, 0x48, 0x50);	//Rigth Out Mix(1)
	PCM_CODEC_IICWrite(0x34, 0x4a, 0x55);	//Right Out Mix(2)
	PCM_CODEC_IICWrite(0x34, 0x4c, 0x50);	//Mono Out Mix(1)
	PCM_CODEC_IICWrite(0x34, 0x4e, 0x55);	//Mono Out Mix(2)

	PCM_CODEC_IICWrite(0x34, 0x50, 0x79);	//LOUT1 Volume
	PCM_CODEC_IICWrite(0x34, 0x52, 0x79);	//ROUT1 Volume
	PCM_CODEC_IICWrite(0x34, 0x54, 0x79);	//LOUT2 Volume
	PCM_CODEC_IICWrite(0x34, 0x56, 0x79);	//ROUT2 Volume
	PCM_CODEC_IICWrite(0x34, 0x58, 0x79);	//MONO Out
	PCM_CODEC_IICWrite(0x34, 0x5a, 0x00);	//Output Control
	PCM_CODEC_IICWrite(0x34, 0x5c, 0x05);	//ADC Input Mode << Set L/R ADC input select to Line 1/2 '01' >>
	PCM_CODEC_IICWrite(0x34, 0x5e, 0x00);	//Input Control (1)
	PCM_CODEC_IICWrite(0x34, 0x60, 0x00);	//Input Control (2)
	PCM_CODEC_IICWrite(0x34, 0x62, 0x97);	//Left Input Volume
	PCM_CODEC_IICWrite(0x34, 0x64, 0x97);	//Right Input Volume
	PCM_CODEC_IICWrite(0x34, 0x66, 0x00);	//MIC Bias Comp Control

	PCM_CODEC_IICWrite(0x34, 0x68, 0x04);	//Clock Control << Clock for Voice Codec = MCLK or PLL1
	PCM_CODEC_IICWrite(0x34, 0x6a, 0x00);	//PLL1 Control (1)
	PCM_CODEC_IICWrite(0x34, 0x6c, 0x83);	//PLL1 Control (2)
	PCM_CODEC_IICWrite(0x34, 0x6e, 0x24);	//PLL1 Control (3)
	PCM_CODEC_IICWrite(0x34, 0x71, 0xba);	//PLL1 Control (4)

	PCM_CODEC_IICWrite(0x34, 0x72, 0x00);	//PLL2 Control (1)
	PCM_CODEC_IICWrite(0x34, 0x74, 0x83);	//PLL2 Control (2)
	PCM_CODEC_IICWrite(0x34, 0x76, 0x24);	//PLL2 Control (3)
	PCM_CODEC_IICWrite(0x34, 0x79, 0xba);	//PLL2 Control (4)

	PCM_CODEC_IICWrite(0x34, 0x7a, 0x00);	//Bias Control
	PCM_CODEC_IICWrite(0x34, 0x7e, 0x00);	//Additional Control
	
	#endif
}

void PCM_CodecExitPCMIn(AUDIO_PORT rhs_ePort)
{

	#if (PCM_CODEC_NAME == AK2430)
	PCM_CODEC_IICWrite(0x9e, 0x05, 0x30);	//MIC Amp1, MIC Bias1 Off	
	Delay(1300);
	PCM_CODEC_IICWrite(0x9e, 0x0B, 0x00);	//sw_COI, sw_TX1 Open	
	PCM_CODEC_IICWrite(0x9e, 0x05, 0x00);	//TX SUM, PCM Codec Off
	
	#elif ( PCM_CODEC_NAME == WM9713)
	AC97_CodecCmd(WRITE,0x3E, 0xffff);		//Disable LINE L/R output PGA's
	AC97_CodecCmd(WRITE,0x26, 0xff00);		//Disable I/P PGA's nad Mixers	
	
	#endif
}


void PCM_CodecInitPCMOut(u32 eSync, u32 eSclk, SerialDataFormat eMSBPos,  PCM_CLKSRC eClkSrc)
{

	
	#if (PCM_CODEC == AK2430)
	

	if(eSclk == 128000)
		uCodecSet = 0x0;
	else if(eSclk == 256000)
		uCodecSet = 0x1;
	else if(eSclk == 512000)
		uCodecSet = 0x2;

	PCM_CODEC_IICWrite(0x9e, 0x18, 0x07);		//MSTCLK_I = 19.2MHz

	if(eClkSrc ==PCM_PCMCDCLK)
		PCM_CODEC_IICWrite(0x9e, 0x1B, 0x0);		//PCM Codec Master Mode
	else
		PCM_CODEC_IICWrite(0x9e, 0x1B, 0x6);		//PCM Codec Slave Mode
        	
	PCM_CODEC_IICWrite(0x9e, 0x1E, uCodecSet);	//DLCK = 128K,256kHz, or 512K, 14bit linear code (2's complement  format)
	PCM_CODEC_IICWrite(0x9e, 0x0C, 0x00);		//0 dB
	PCM_CODEC_IICWrite(0x9e, 0x10, 0x1F);		//three dB
	PCM_CODEC_IICWrite(0x9e, 0x11, 0x1F);  		 //three dB
	PCM_CODEC_IICWrite(0x9e, 0x06, 0x40);		//POP Mute Off     
	PCM_CODEC_IICWrite(0x9e, 0x03, 0x01);		//CKI Buff On other blocks Off
	Delay(1); 
	PCM_CODEC_IICWrite(0x9e, 0x03, 0x09);		//PLL2, CKI Buff On and other blocks Off
	Delay(300);
	PCM_CODEC_IICWrite(0x9e, 0x04, 0x01);		//RXSUM On and other blocks Off
	PCM_CODEC_IICWrite(0x9e, 0x05, 0x20);		//PCM Codec On and other blocks Off
	Delay(50);
	PCM_CODEC_IICWrite(0x9e, 0x07, 0x20);		//sw_COL Close and other sw Open
	PCM_CODEC_IICWrite(0x9e, 0x08, 0x20);		//sw_COR Close and othr sw Open
	PCM_CODEC_IICWrite(0x9e, 0x09, 0x14);		//sw_EPL, sw_EPR Amp output
	PCM_CODEC_IICWrite(0x9e, 0x04, 0x31);		//EPL/R Amp(RXSUM) On and other blocks Off
	Delay(1600);
	
	#elif ( PCM_CODEC == WM9713)
	
	if(AC97_InitACLINK())
	{
		AC97_CodecCmd(WRITE,0x26, 0x4f00);		// Enable PR5(Internal Clock, AC-link I/F)
		AC97_CodecCmd(WRITE,0x26, 0x4700);		// Enable PR3(VREF, I/P PGA's, DAC's, ADC's, Mixer, O/P's)

		AC97_CodecCmd(WRITE,0x3C, 0xfbff);		// Enable MBIAS generator
		Delay(1000);

		AC97_CodecCmd(WRITE,0x26, 0x4300);		// Enable I/P PGA's nad Mixers
		AC97_CodecCmd(WRITE,0x3C, 0xebf3);		// Enable VXDAC, L/R HP
		AC97_CodecCmd(WRITE,0x26, 0x4100);		// Enable DAC
		AC97_CodecCmd(WRITE,0x26, 0x0100);		// Enable PR6 (O/P PGA's)
		AC97_CodecCmd(WRITE,0x3E, 0xf9ff);		// Enable HPL/R output PGA's

		AC97_CodecCmd(WRITE,0x1C, 0x00a0);		// HPL/R PGA input select: HPMIXL/R
		AC97_CodecCmd(WRITE,0x04, 0x8a8a);		// Set HL/R Volume = -6dB

		AC97_CodecCmd(WRITE,0x2A,0x1);			//Variable Rate Enable	
		UART_Printf("VRA Enable(1)/Disable(0): 0x%x\n",(0x1&AC97_CodecCmd(READ,0x2A,0x0001)));
		AC97_CodecCmd(WRITE,0x2E, 0x1f40);		// AUXDAC = 8kHz

		AC97_CodecCmd(WRITE,0x12,0x8000);		//Disable ADC because it makes noise when ADC is turn on
		AC97_CodecCmd(WRITE,0x18, 0x02aa);		// Unmute VXDAC to HPMIXL/R
		AC97_CodecCmd(WRITE,0x04, 0x0f0f);		// Unmute HPL/R

		if (eSync == DURING_PCMSYNC_HIGH)			//DURING_PCMSYNC_HIGH = Dsp  Mode B
			AC97_CodecCmd(WRITE,0x36, 0xa6b3);		// PCM IF=DSP B type, 16Bit, Slave Mode, Fs=8kHz		1010 0110 0110 0011
		else 															//AFTER_PCMSYNC_HIGH = Dsp  Mode A
			AC97_CodecCmd(WRITE,0x36, 0xa6a3);		// PCM IF=DSP A type, 16Bit, Slave Mode, Fs=8kHz		1010 0110 0010 0011
		AC97_CodecCmd(WRITE,0x44, 0x0b80);		// PCM CLK = 8kHz
		AC97_CodecCmd(WRITE,0x4C, 0xffde);		// GPIO config. For PCM IF (Slave mode) 
	}
	

	#elif ( PCM_CODEC == WM8753)
	
	PCM_CODEC_IICWrite(0x34, 0x1f, 0x00);	//Reset

	PCM_CODEC_IICWrite(0x34, 0x28, 0xd0);	//PWR Management(1) << Power up VMID[50Kohm]; VREF; VDAC >>
	PCM_CODEC_IICWrite(0x34, 0x2e, 0x03);	//PWR Management(4) << Power up L/RMIX >>
	Delay(1000);
	
	PCM_CODEC_IICWrite(0x34, 0x02, 0x08);	//DAC Control Mute
	PCM_CODEC_IICWrite(0x34, 0x04, 0x00);	//ADC Control
	
	if (eSync == DURING_PCMSYNC_HIGH)
		PCM_CODEC_IICWrite(0x34, 0x07, 0xb3);	//PCM Audio I/F Mode B
	else 
		PCM_CODEC_IICWrite(0x34, 0x07, 0xa3);	//PCM Audio I/F Mode A
	PCM_CODEC_IICWrite(0x34, 0x08, 0x0a);	//HiFi Audio I/F
	
	PCM_CODEC_IICWrite(0x34, 0x0a, 0x33);	//Interface Control << Voice Codec >>
	
	if (eSync == 8000)
		PCM_CODEC_IICWrite(0x34, 0x0c, 0x0c);	//Sync Rate Control 8KHz 
	else if (eSync == 16000)     
		PCM_CODEC_IICWrite(0x34, 0x0c, 0x14);	//Sync Rate Control 16KHz 
	else if (eSync == 32000)
		PCM_CODEC_IICWrite(0x34, 0x0c, 0x18);	//Sync Rate Control 32KHz
	else if (eSync == 48000)
		PCM_CODEC_IICWrite(0x34, 0x0c, 0x00);	//Sync Rate Control 48KHz
	else if (eSync == 96000)	
		PCM_CODEC_IICWrite(0x34, 0x0c, 0x1e);	//Sync Rate Control 96KHz
		
	PCM_CODEC_IICWrite(0x34, 0x0e, 0x97); 	//Sample Rate Control (2), Don't care in case of slave mode
	PCM_CODEC_IICWrite(0x34, 0x10, 0xff); 	//Left DAC Volume
	PCM_CODEC_IICWrite(0x34, 0x12, 0xff); 	//Right DAC Volume
	PCM_CODEC_IICWrite(0x34, 0x14, 0x0f); 	//Bass Control
	PCM_CODEC_IICWrite(0x34, 0x16, 0x0f); 	//Treble Control
	PCM_CODEC_IICWrite(0x34, 0x18, 0x7b); 	//ALC 1
	PCM_CODEC_IICWrite(0x34, 0x1a, 0x00); 	//ALC 2
	PCM_CODEC_IICWrite(0x34, 0x1c, 0x32); 	//ALC 3
	PCM_CODEC_IICWrite(0x34, 0x1e, 0x00); 	//Noise Gate 
	PCM_CODEC_IICWrite(0x34, 0x21, 0xc3); 	//Left ADC Volume
	PCM_CODEC_IICWrite(0x34, 0x23, 0xc3); 	//Right ADC Volume
	PCM_CODEC_IICWrite(0x34, 0x24, 0xc0); 	//Additional Control
	PCM_CODEC_IICWrite(0x34, 0x26, 0x00); 	//Three D Control
	PCM_CODEC_IICWrite(0x34, 0x2a, 0x00); 	//Power Management(2) 
	PCM_CODEC_IICWrite(0x34, 0x30, 0x00); 	//ID Register
	PCM_CODEC_IICWrite(0x34, 0x32, 0x00); 	//Int. Polarty
	PCM_CODEC_IICWrite(0x34, 0x34, 0x00);	//Int. Enable
	PCM_CODEC_IICWrite(0x34, 0x36, 0x00);	//GPIO Control (1)
	PCM_CODEC_IICWrite(0x34, 0x38, 0x00);	//GPIO Control (2)
	PCM_CODEC_IICWrite(0x34, 0x40, 0x55);	//Record Mix (1)
	PCM_CODEC_IICWrite(0x34, 0x42, 0x05);	//Record Mix (2)
	PCM_CODEC_IICWrite(0x34, 0x44, 0x50);	//Left Out Mix(1)
	PCM_CODEC_IICWrite(0x34, 0x47, 0x55);	//Left Out Mix(2) << Set the VXD2LO bit (Voice DAC to Left Output >>
	PCM_CODEC_IICWrite(0x34, 0x48, 0x50);	//Rigth Out Mix(1)
	PCM_CODEC_IICWrite(0x34, 0x4b, 0x55);	//Right Out Mix(2) << Set the VXD2RO bit (Voice DAC to Right Output) >>
	PCM_CODEC_IICWrite(0x34, 0x4c, 0x50);	//Mono Out Mix(1)
	PCM_CODEC_IICWrite(0x34, 0x4e, 0x55);	//Mono Out Mix(2)
	PCM_CODEC_IICWrite(0x34, 0x51, 0x79);	//LOUT1 Volume << Set Left OUtput 1 Volume Update bit to '1' & Volume Level to Default >>
	PCM_CODEC_IICWrite(0x34, 0x53, 0x79);	//ROUT1 Volume << Set Right OUtput 1 Volume Update bit to '1' & Volume Level to Default >>
	PCM_CODEC_IICWrite(0x34, 0x54, 0x79);	//LOUT2 Volume
	PCM_CODEC_IICWrite(0x34, 0x56, 0x79);	//ROUT2 Volume
	PCM_CODEC_IICWrite(0x34, 0x58, 0x79);	//MONO Out
	PCM_CODEC_IICWrite(0x34, 0x5a, 0x00);	//Output Control
	PCM_CODEC_IICWrite(0x34, 0x5c, 0x00);	//ADC Input Mode 
	PCM_CODEC_IICWrite(0x34, 0x5e, 0x00);	//Input Control (1)
	PCM_CODEC_IICWrite(0x34, 0x60, 0x00);	//Input Control (2)
	PCM_CODEC_IICWrite(0x34, 0x62, 0x97);	//Left Input Volume
	PCM_CODEC_IICWrite(0x34, 0x64, 0x97);	//Right Input Volume
	PCM_CODEC_IICWrite(0x34, 0x66, 0x00);	//MIC Bias Comp Control
	PCM_CODEC_IICWrite(0x34, 0x68, 0x04);	//Clock Control << Clock for Voice Codec = MCLK or PLL1
	PCM_CODEC_IICWrite(0x34, 0x6a, 0x00);	//PLL1 Control (1)
	PCM_CODEC_IICWrite(0x34, 0x6c, 0x83);	//PLL1 Control (2)
	PCM_CODEC_IICWrite(0x34, 0x6e, 0x24);	//PLL1 Control (3)
	PCM_CODEC_IICWrite(0x34, 0x71, 0xba);	//PLL1 Control (4)
	PCM_CODEC_IICWrite(0x34, 0x72, 0x00);	//PLL2 Control (1)
	PCM_CODEC_IICWrite(0x34, 0x74, 0x83);	//PLL2 Control (2)
	PCM_CODEC_IICWrite(0x34, 0x76, 0x24);	//PLL2 Control (3)
	PCM_CODEC_IICWrite(0x34, 0x79, 0xba);	//PLL2 Control (4)
	PCM_CODEC_IICWrite(0x34, 0x7a, 0x00);	//Bias Control
	PCM_CODEC_IICWrite(0x34, 0x7e, 0x00);	//Additional Control
	PCM_CODEC_IICWrite(0x34, 0x2d, 0x80); 	//Power Management(3) <<Power up L/ROUT1 outputs >>
		
	
	#endif
}



void PCM_CodecExitPCMOut(void)
{
	#if ( PCM_CODEC == AK2430)
	
		PCM_CODEC_IICWrite(0x9e, 0x04, 0x01);	//EPL/R Amp Off		
		Delay(1300);

		PCM_CODEC_IICWrite(0x9e, 0x07, 0x00);	//sw_COL Close and other sw Open
	   	PCM_CODEC_IICWrite(0x9e, 0x08, 0x00);	//sw_COR Close and othr sw Open
	   	PCM_CODEC_IICWrite(0x9e, 0x09, 0x00);	//sw_EPL, sw_EPR Amp output

		PCM_CODEC_IICWrite(0x9e, 0x04, 0x00);	//RXSUM Off	
		PCM_CODEC_IICWrite(0x9e, 0x05, 0x00);	//PCM Codec Off
	

	#elif ( PCM_CODEC == WM9713)
	
		AC97_CodecCmd(WRITE,0x3E, 0xffff);		//Disable HPL/R output PGA's
		AC97_CodecCmd(WRITE,0x26, 0xff00);		//Disable I/P PGA's nad Mixers
	
	#endif
	
}
