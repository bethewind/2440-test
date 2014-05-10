/**************************************************************************************
* 
*	Project Name : S3C6410 Validation
*
*	Copyright 2008 by Samsung Electronics, Inc.
*	All rights reserved.
*
*	Project Description :
*		This software is only for validating functions of the S3C6400.
*		Anybody can use this software without our permission.
*  
*--------------------------------------------------------------------------------------
* 
*	File Name : i2s.c
*  
*	File Description : This file implements the WM8753 driver functions for I2S.
*
*	Author : Sung-Hyun,Na
*	Dept. : AP Development Team
*	Created Date : 2008/02/29
*	Version : 0.1 
* 
*	History
*	- Created(Sunghyun,Na 2008/02/01)
*   
**************************************************************************************/

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
#include "iic.h"
#include "audio.h"

#include "i2s.h"

static	DMAC 		oI2SDma[I2S_CONNUM];
volatile	u32			g_uI2SRecDone[I2S_CONNUM];
volatile 	u32			g_uPlayDone[I2S_CONNUM];
extern	I2S_Infor		g_oI2SInfor[I2S_CONNUM];
extern	I2S_TestMode	g_TestMode;
 
// define global variables
void *	g_paI2SBase[I2S_CONNUM];
u32 *	g_pRecAddr[I2S_CONNUM];
u32 *	g_pPlayAddr[I2S_CONNUM];
/*---------------------------------- Init Functions  ---------------------------------*/
//////////
// Function Name : I2S_Init
// Function Description : This function initializes a certain I2S Port.
// Input : 	uPort - I2S Port Number 
// Output : 	TRUE - Memory Device is reset
//				FALSE - Memory Device is not reset because of ERROR
// Version : v0.1

u32 I2S_Init(AUDIO_PORT ePort)
{
	I2S_Infor* p_ePort;
	p_ePort = I2S_GetI2SInfor(ePort);
	
	if(ePort == AUDIO_PORT0)
	{
		// using DMAC0
		#ifdef I2S_NORMAL_DMA		
		p_ePort->m_uNumDma			= NUM_DMA0;					
		p_ePort->m_eDmaUnit				= DMA0;			
		#else	// I2S_SECURE_DMA
		p_ePort->m_uNumDma			= NUM_SDMA0;		
		p_ePort->m_eDmaUnit				= SDMA0;		
		#endif
		p_ePort->m_eDmaCh				= DMA_A;
		p_ePort->m_uI2SRxFifoAddr		= I2S0_BASE + rI2SRXD;
		p_ePort->m_uI2STxFifoAddr		= I2S0_BASE + rI2STXD;

		p_ePort->m_uPlayBufferStartAddr 	= (u32* )0x5100002c;
		p_ePort->m_uRecBufferStartAddr 	= (u32* )0x5100002c;
		
		p_ePort->m_uPcmDataSize			= I2S_RECORD_LENGTH;
		
		//I2S Operation
		p_ePort->m_eOpMode 			= Master;
		p_ePort->m_eWordLength			= Word16;
		p_ePort->m_eIFMode 				= TXRXBoth;
		p_ePort->m_eLRCLKPolarity			= RightHigh;
		p_ePort->m_eDataFormat 			= I2SFormat;
		p_ePort->m_CLKSRC 				= I2S_MOUT_EPLL;
		p_ePort->m_dSamplingFreq 		= 48000;
		p_ePort->m_eRFS 					= RFS_768fs;
		p_ePort->m_eBFS 					= BFS_48fs;
		
		
		
	}
	else if(ePort == AUDIO_PORT1)
	{
		// using DMAC1
		#ifdef I2S_NORMAL_DMA		
		p_ePort->m_uNumDma			= NUM_DMA1;		
		p_ePort->m_eDmaUnit				= DMA1;			
		#else
		p_ePort->m_uNumDma			= NUM_SDMA1;		
		p_ePort->m_eDmaUnit				= SDMA1;		
		#endif
		
		p_ePort->m_eDmaCh				= DMA_B;
		p_ePort->m_uI2SRxFifoAddr		= I2S1_BASE + rI2SRXD;
		p_ePort->m_uI2STxFifoAddr		= I2S1_BASE + rI2STXD;

		p_ePort->m_uPlayBufferStartAddr 	= (u32*) 0x5100002c;
		p_ePort->m_uRecBufferStartAddr 	= (u32*) 0x5100002c;
		
		p_ePort->m_uPcmDataSize			= I2S_RECORD_LENGTH;
		
		//I2S Operation
		p_ePort->m_eOpMode 			= Master;
		p_ePort->m_eWordLength 			= Word16;
		p_ePort->m_eIFMode 				= TXRXBoth;
		p_ePort->m_eLRCLKPolarity			= LeftHigh;
		p_ePort->m_eDataFormat 			= I2SFormat;
		p_ePort->m_CLKSRC 				= I2S_MOUT_EPLL;
		p_ePort->m_dSamplingFreq 		= 48000;
		p_ePort->m_eRFS 					= RFS_768fs;
		p_ePort->m_eBFS 					= BFS_48fs;
	}
	else
	{
		return FALSE;
	}	
	I2SMOD_PutData(ePort, p_ePort);		
	return TRUE;
}

I2S_Infor* I2S_GetI2SInfor(AUDIO_PORT rhs_ePort)
{
	 if (rhs_ePort == AUDIO_PORT1)
		return &(g_oI2SInfor[AUDIO_PORT1]);
	else
		return &(g_oI2SInfor[AUDIO_PORT0]);

}

//////////
// Function Name : I2S_InitPort
// Function Description : 
//   This function Initialize ports as I2S.
// Input : uPort - I2S port number
// Output : NONE
// Version : 
void I2S_InitPort(AUDIO_PORT ePort)
{
	switch(ePort)
	{
		case AUDIO_PORT0:
			GPIO_SetFunctionAll(eGPIO_D, 0x33333, 0x0);
			GPIO_SetPullUpDownAll(eGPIO_D, 0x22222);			// pull-up/down disable
			break;
		case AUDIO_PORT1 :
			GPIO_SetFunctionAll(eGPIO_E, 0x33333, 0x0);
			GPIO_SetPullUpDownAll(eGPIO_E, 0x22222);			// pull-up/down disable
			break;
		default :
			GPIO_SetFunctionAll(eGPIO_D, 0x33333, 0x0);
			GPIO_SetPullUpDownAll(eGPIO_D, 0x22222);			// pull-up/down disable
			break;
	}
}

//////////
// Function Name : I2S_InitPort
// Function Description : 
//   This function Initialize ports as I2S.
// Input : rhs_ePort - I2S port number
// Output : NONE
// Version : 

void I2S_SetCDCLKPort(AUDIO_PORT rhs_ePort)
{
	switch(rhs_ePort)
	{
		case AUDIO_PORT0:
			GPIO_SetFunctionEach(eGPIO_D, eGPIO_1, 0x3);		// Using Only I2SCDCLK OUT PAD		
			break;
		case AUDIO_PORT1 :
			GPIO_SetFunctionEach(eGPIO_E, eGPIO_1, 0x3);		// Using Only I2SCDCLK OUT PAD		
			break;
		default :
			GPIO_SetFunctionEach(eGPIO_D, eGPIO_1, 0x3);		// Using Only I2SCDCLK OUT PAD		
			break;
	}

}

//////////
// Function Name : I2S_ReturnPort
// Function Description : 
//   This function Initialize ports as default GPIO(input).
// Input : uPort - I2S port number
// Output : NONE
// Version : 
void I2S_ReturnPort(AUDIO_PORT ePort)
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
}

void I2S_CDCLKOut(AUDIO_PORT rhs_ePort)
{	
	u32 uPRSVal, uTemp;

	
	I2S_SelCLKAUDIO(rhs_ePort, g_oI2SInfor[rhs_ePort].m_CLKSRC);
	uPRSVal = I2S_GetPRSVal(rhs_ePort);	
	I2SPSR_SetPrescaler(rhs_ePort, uPRSVal);
	
	//Transfer data enable  
	//Set -Up IISCON of I2S
	uTemp = I2S_Inp32(rhs_ePort, rI2SCON);
	uTemp |=I2S_Enable_I2SIF;			//Enable I2SI/F
	uTemp &=  ~I2S_Enable_TX_DMA & ~I2S_Pause_TX_DMA & ~I2S_Pause_TX_IF & ~I2S_Enable_TX_DMA & ~I2S_Pause_RX_DMA & ~I2S_Pause_RX_IF & ~I2S_EN_UnderrunIntr & I2S_EN_OverrunIntr;		//Disable Pause RX DMA, RX I/F
	I2S_Outp32(rhs_ePort,  rI2SCON, uTemp);
}

void I2S_SetEpllCDCLKOut(AUDIO_PORT rhs_ePort, u32 uSampleRate)
{
	u32 uTemp;
	double dEPLLCLK = I2S_SetEPLLClk((double) uSampleRate);
	u32 uPRSVal = I2S_GetPRSVal2(rhs_ePort, (u32) dEPLLCLK, uSampleRate);
	I2SPSR_SetPrescaler(rhs_ePort, uPRSVal);
	I2S_SelCLKAUDIO(rhs_ePort, I2S_MOUT_EPLL);							//Epll Out

	//Transfer data enable  
	//Set -Up IISCON of I2S
	uTemp = I2S_Inp32(rhs_ePort, rI2SCON);
	uTemp |= I2S_Enable_I2SIF;			//Enable I2SI/F
	uTemp &=  ~I2S_Enable_TX_DMA & ~I2S_Pause_TX_DMA & ~I2S_Pause_TX_IF & ~I2S_Enable_TX_DMA & ~I2S_Pause_RX_DMA & ~I2S_Pause_RX_IF & ~I2S_EN_UnderrunIntr & I2S_EN_OverrunIntr;		//Disable Pause RX DMA, RX I/F
	I2S_Outp32(rhs_ePort,  rI2SCON, uTemp);	
}

void I2S_DataInPolling(AUDIO_PORT rhs_ePort)
{
	s32 uPcmDataSize = 0xf0000;
	u32 uRegValue = 0;
	u32* uRecBuf= g_oI2SInfor[rhs_ePort].m_uRecBufferStartAddr;
	u32 uFIFOTriggerLevel = 0x8;					//for Debug	

	Assert(uPcmDataSize%4 == 0 && uPcmDataSize >= 4);
	
	UART_Printf("Now Record...\n");	
	I2SFIC_AutoFlush(rhs_ePort,  RXOnly);		//FIFO Flush
	//I2SCON_SetIFMode(rhs_ePort, RXOnly);
	I2SCON_ActiveIF(rhs_ePort, ACTIVE);			//IIS IF ACTIVE
	I2SCON_PauseIF(rhs_ePort, RXOnly,  PLAY);	//I2S RX IF no Pause

	while( uPcmDataSize > 0 )
	{
		do 
		{
			uRegValue = I2S_Inp32(rhs_ePort, rI2SFIC);
		} while ((uRegValue & (I2S_Status_RX_FIFO_CNT)) < uFIFOTriggerLevel);		// Rx FIFO Level check
		
		while(1)
		{
			uRegValue	= I2S_Inp32(rhs_ePort, rI2SRXD);						//Data Transfer		
			*uRecBuf++ = uRegValue;
			uPcmDataSize -= 4;
			uRegValue = I2S_Inp32(rhs_ePort, rI2SCON);
			if (uRegValue & (I2S_Status_RX_FIFO_EMPTY) )
				break;
		}
	}
	
	UART_Printf("Data Receive Done\n");	
	I2SCON_PauseIF(rhs_ePort, RXOnly, PAUSE);				//RX IF Pause
	I2SCON_ActiveIF(rhs_ePort, INACTIVE);					//IF Inactive	
	I2SFIC_AutoFlush(rhs_ePort, RXOnly);						//FIFO FLUSH

	if (g_oI2SInfor[rhs_ePort].m_eWordLength == Word24)
		Parser_32to24(g_oI2SInfor[rhs_ePort].m_uRecBufferStartAddr, I2S_RECORD_LENGTH, (u32 *)0x53400000);
	
}


void I2S_DataOutPolling(AUDIO_PORT rhs_ePort)
{
	u32 *uRecBuf = g_oI2SInfor[rhs_ePort].m_uRecBufferStartAddr;
	s32 uPcmDataSize = 0xf0000;
	u32 uRegValue = 0;	
	u32 uFIFOTriggerLevel = 0x8;
	
	UART_Printf("Now Play...\n");	
	I2SFIC_AutoFlush(rhs_ePort,  TXOnly);		//FIFO Flush	
	I2SCON_ActiveIF(rhs_ePort, ACTIVE);			//IIS IF ACTIVE
	I2SCON_PauseIF(rhs_ePort, TXOnly,  PLAY);	//I2S RX IF no Pause

	while(uPcmDataSize > 0)
	{
		while(1)
		{
			uRegValue = *uRecBuf++;
			I2S_Outp32(rhs_ePort, rI2STXD, uRegValue);							//Data Transfer		
			uPcmDataSize -= 4;
			uRegValue = I2S_Inp32(rhs_ePort, rI2SCON);
			if (uRegValue & (I2S_Status_TX_FIFO_FULL) )
				break;
		}
		
		do 
		{
			uRegValue = I2S_Inp32(rhs_ePort, rI2SFIC);
		} while ((uRegValue & (I2S_Status_TX_FIFO_CNT)) > uFIFOTriggerLevel);		// TX FIFO Level check
		
	}
	
	UART_Printf("Play Done~\n");	
	I2SCON_PauseIF(rhs_ePort, TXOnly, PAUSE);				//RX IF Pause
	I2SCON_ActiveIF(rhs_ePort, INACTIVE);					//IF Inactive	
	//I2SFIC_AutoFlush(rhs_ePort, TXOnly);					//FIFO FLUSH
}	

void I2S_DataBypassPolling(AUDIO_PORT rhs_ePort)
{
	//s32 uPcmDataSize = 0xf0000;
	u32 uRegValue = 0;
	u32 uFIFOTriggerLevel = 0x8;
	u32 uPollingDelay = 0x10000;

	
	

	//Assert(uPcmDataSize%4 == 0 && uPcmDataSize >= 4);
	
	UART_Printf("By Pass Start...\n");	
	I2SFIC_AutoFlush(rhs_ePort,  TXRXBoth);		//FIFO Flush	
	I2SCON_ActiveIF(rhs_ePort, ACTIVE);			//IIS IF ACTIVE
	I2SCON_PauseIF(rhs_ePort, TXRXBoth,  PLAY);	//I2S RX IF no Pause
	#if 0
	while( uPcmDataSize > 0 )
	{
		do 
		{
			uRegValue = I2S_Inp32(rhs_ePort, rI2SCON);
		} while (uRegValue&(I2S_Status_RX_FIFO_EMPTY));		// Rx FIFO empty check
		UART_Printf("%d\t",(uRegValue & (I2S_Status_RX_FIFO_CNT)));
		uRegValue	= I2S_Inp32(rhs_ePort, rI2SRXD);			//Data Transfer		
		*uRecBuf++ = uRegValue;
		uPcmDataSize -= 4;		
	}
	#else
	do
	{
		do 
		{
			uRegValue = I2S_Inp32(rhs_ePort, rI2SFIC);
		} while ((uRegValue & (I2S_Status_RX_FIFO_CNT)) < uFIFOTriggerLevel);		// Rx FIFO Level check
		
		while(1)
		{
			uRegValue	= I2S_Inp32(rhs_ePort, rI2SRXD);						//Data Transfer		
			I2S_Outp32(rhs_ePort, rI2STXD,uRegValue);
			uRegValue = I2S_Inp32(rhs_ePort, rI2SCON);
			if (uRegValue & (I2S_Status_RX_FIFO_EMPTY) )
				break;
		}
		uPollingDelay--;
	}while( uPollingDelay != 0 );
	#endif
	UART_Printf("By Pass End.\n");	
	I2SCON_PauseIF(rhs_ePort, RXOnly, PAUSE);				//RX IF Pause
	I2SCON_ActiveIF(rhs_ePort, INACTIVE);					//IF Inactive	
	//I2SFIC_AutoFlush(rhs_ePort, RXOnly);						//FIFO FLUSH
}	


void I2S_DataInIntr(AUDIO_PORT rhs_ePort)
{
	u32 uTemp;
	u32 uRegValue;
	//Set Interrupt Controller 
	
	I2SCON_ClrOverIntr(rhs_ePort);	

	UART_Printf("0x%x\n",I2S_Inp32(rhs_ePort, rI2SCON));
	INTC_ClearVectAddr();
	
	if ( rhs_ePort == AUDIO_PORT0)
		INTC_SetVectAddr(NUM_I2S ,ISR_I2SPort0_RXOverrun);	
	else 	
		INTC_SetVectAddr(NUM_I2S ,ISR_I2SPort1_RXOverrun);
	INTC_Enable(NUM_I2S);
	
	g_uI2SRecDone[rhs_ePort] = 0;	
	g_pRecAddr[rhs_ePort] = g_oI2SInfor[rhs_ePort].m_uRecBufferStartAddr;
	
	UART_Printf("Recoding......\n");
	
	I2SFIC_AutoFlush(rhs_ePort, RXOnly);						//FIFO Flush	
	
	uTemp = I2S_Inp32(rhs_ePort, rI2SCON);		
	uTemp |= I2S_Enable_I2SIF;								//Enable I2S Interface & RX Overrun Intr' 
	uTemp &= ~I2S_Enable_RX_DMA & ~I2S_Pause_RX_IF & ~I2S_EN_UnderrunIntr;
	I2S_Outp32(rhs_ePort, rI2SCON, uTemp);

	//do 
	//	{
	//		uRegValue = I2S_Inp32(rhs_ePort, rI2SFIC);
	//	} while ((uRegValue & (I2S_Status_RX_FIFO_CNT)) < uFIFOTriggerLevel);		// Rx FIFO Level check


	for (uTemp = 0; uTemp < 8; uTemp++)
		*(g_pRecAddr[rhs_ePort]) = I2S_Inp32(rhs_ePort, rI2SRXD);			//Data Transfer		
		
	I2SCON_EnableIntr(rhs_ePort, I2S_RX, ACTIVE);

						
	
			
	UART_Printf("0x%x\n",I2S_Inp32(rhs_ePort, rI2SCON));
	while(1)
	{
		
		if ( g_uI2SRecDone[rhs_ePort] == 1)
			break;
	}
	INTC_Disable(NUM_I2S);
	uTemp = I2S_Inp32(rhs_ePort, rI2SCON);
	uTemp |= I2S_Pause_RX_IF;
	uTemp &= ~I2S_EN_OverrunIntr & ~I2S_Enable_I2SIF;
	I2S_Outp32(rhs_ePort, rI2SCON, uTemp);
	
	if (g_oI2SInfor[rhs_ePort].m_eWordLength == Word24)
		Parser_32to24(g_oI2SInfor[rhs_ePort].m_uRecBufferStartAddr, I2S_RECORD_LENGTH, (u32 *)0x53400000);
	
	
}


void I2S_DataOutIntr(AUDIO_PORT rhs_ePort)
{
	u32 uTemp;

	INTC_ClearVectAddr();
	I2SCON_ClrUnderIntr(rhs_ePort);
	
	//Set Interrupt Controller 
	
	if ( rhs_ePort == AUDIO_PORT0)
		INTC_SetVectAddr(NUM_I2S ,ISR_I2SPort0_TXUnderrun);	
	else 	
		INTC_SetVectAddr(NUM_I2S ,ISR_I2SPort1_TXUnderrun);
	INTC_Enable(NUM_I2S);
	
	g_uPlayDone[rhs_ePort] = 0;	
	g_pPlayAddr[rhs_ePort] = g_oI2SInfor[rhs_ePort].m_uPlayBufferStartAddr;
	UART_Printf("Play!\n");

	I2SFIC_AutoFlush(rhs_ePort, TXOnly);					//FIFO Flush	
	uTemp = I2S_Inp32(rhs_ePort, rI2SCON);		
	uTemp |= I2S_Enable_I2SIF;				
	uTemp &= ~I2S_Enable_TX_DMA & ~I2S_Pause_TX_IF & ~I2S_EN_OverrunIntr;
	I2S_Outp32(rhs_ePort, rI2SCON, uTemp);
	
	I2SCON_EnableIntr(rhs_ePort, I2S_TX, ACTIVE);			//Enable I2S Interface & RX Overrun Intr' 

	for (uTemp = 0; uTemp < 8; uTemp++)
		I2S_Outp32(rhs_ePort, rI2STXD, *(g_pRecAddr[rhs_ePort]));  
	
	while(1)
	{
		if ( g_uPlayDone[rhs_ePort] == 1)
			break;
	}

	INTC_Disable(NUM_I2S);
	uTemp = I2S_Inp32(rhs_ePort, rI2SCON);
	uTemp |= I2S_Pause_TX_IF;
	uTemp &= ~I2S_EN_UnderrunIntr & ~I2S_Enable_I2SIF;
	I2S_Outp32(rhs_ePort, rI2SCON, uTemp);

}


void	I2S_DataInDMAStart(AUDIO_PORT ePort)
{
	u32 uTemp = 0;
	u32 uRegValue;
	u8 ucChar=0;
	
	if (ePort == AUDIO_PORT0)
	{
		g_oI2SInfor[ePort].m_eDmaId		= eSEL_I2S0_RX;
		g_oI2SInfor[ePort].m_eDreqSrc		= DMA0_I2S0_RX;
		//IRQ Setting
		INTC_SetVectAddr(g_oI2SInfor[ePort].m_uNumDma ,ISR_Port0DMARecDone);
	}	
	else if ( ePort == AUDIO_PORT1)
	{
		g_oI2SInfor[ePort].m_eDmaId		= eSEL_I2S1_RX;
		g_oI2SInfor[ePort].m_eDreqSrc		= DMA1_I2S1_RX;
		//IRQ Setting
		INTC_SetVectAddr(g_oI2SInfor[ePort].m_uNumDma ,ISR_Port1DMARecDone);
	}	
	g_uI2SRecDone[ePort] = 0;
	INTC_Enable(g_oI2SInfor[ePort].m_uNumDma);
	
	//INTC_SetVectAddr(NUM_I2S, ISR_I2SOverRun);
	//INTC_Enable(NUM_I2S);
	
	//DMA Setting
	#ifdef	I2S_NORMAL_DMA
	SYSC_SelectDMA(g_oI2SInfor[ePort].m_eDmaId, 1);
	#else
	SYSC_SelectDMA(g_oI2SInfor[ePort].m_eDmaId, 0);
	#endif
	
	DMAC_InitCh((g_oI2SInfor[ePort].m_eDmaUnit) , g_oI2SInfor[ePort].m_eDmaCh, &(oI2SDma[ePort]));

	
	DMACH_ClearIntPending(&(oI2SDma[ePort]));
	DMACH_ClearErrIntPending(&(oI2SDma[ePort]));
	DMACH_Setup(g_oI2SInfor[ePort].m_eDmaCh, 0x0, (u32)g_oI2SInfor[ePort].m_uI2SRxFifoAddr, true,
				(u32)g_oI2SInfor[ePort].m_uRecBufferStartAddr, false, WORD, I2S_RECORD_LENGTH/4, DEMAND, 
				(DREQ_SRC)(g_oI2SInfor[ePort].m_eDreqSrc), MEM, SINGLE, &(oI2SDma[ePort]));
	DMACH_Start(&(oI2SDma[ePort]));	
	
	
	
	//Transfer data enable  
	//Set -Up IISCON of I2S
	//I2SFIC_AutoFlush(ePort, RXOnly);						//FIFO Flush	
	uTemp = I2S_Inp32(ePort, rI2SCON);		
	uTemp |= I2S_Enable_I2SIF;			//Enable I2S Interface
	uTemp &= ~I2S_Pause_RX_IF & ~I2S_EN_OverrunIntr;
	I2S_Outp32(ePort,  rI2SCON, uTemp);				
	//RX FIFO empty check	
	do 
	{
		uRegValue = I2S_Inp32(ePort, rI2SCON);
	} while ( uRegValue&(I2S_Status_RX_FIFO_EMPTY));	
	
	//for (uTemp = 0; uTemp < 8; uTemp++)
	//{
	//	*(g_oI2SInfor[ePort].m_uRecBufferStartAddr++) = I2S_Inp32(ePort, rI2SRXD);
	//}
	
	uTemp |= I2S_Enable_RX_DMA;							//Enable RX DMA
	uTemp &= ~I2S_Pause_RX_DMA;						//Disable Pause RX DMA
	I2S_Outp32(ePort,  rI2SCON, uTemp);


	
	
	if ( g_TestMode == I2S_SINGLE )
	{
		UART_Printf("DMA Pause : P, DMA Play O\n");
		UART_Printf("TX Interface Pause : L, Interface Play : K\n");
		UART_Printf("I2S Interface Pause : I, Play : U\n");
		
		while(g_uI2SRecDone[ePort] ==0)    	
		{
	        	
			ucChar = UART_Getc();
			 if ((ucChar == 'p') | (ucChar == 'P'))
			{
				I2SCON_PauseDMA(ePort, RXOnly, PAUSE);
				UART_Printf("DMA PAUSE!!\n");				
			}
			else if ((ucChar == 'o') | (ucChar == 'O'))
			{
				I2SCON_PauseDMA(ePort, RXOnly, PLAY);
				UART_Printf("DMA PLAY!!\n");
			}
			else if ((ucChar == 'l') | (ucChar == 'L'))
			{
				I2SCON_PauseIF(ePort, RXOnly, PAUSE);
				UART_Printf("TX Interface PAUSE!!\n");
			}
			else if ((ucChar == 'k') | (ucChar == 'K'))
			{
				I2SCON_PauseIF(ePort, RXOnly, PLAY);
				UART_Printf("TX Interface PLAY!!\n");
			}
			else if ((ucChar == 'i') | (ucChar == 'I'))
			{
				I2SCON_ActiveIF(ePort,  false);
				UART_Printf("Interface Off\n");
			}
			else if ((ucChar == 'u') | (ucChar == 'U'))
			{
				I2SCON_ActiveIF(ePort,  true);
				UART_Printf("Interface On\n");
			}
			
	    	}
	 g_uI2SRecDone[ePort] = 0; 
	}
	else
	{
	}
	
	//INTC_Disable(NUM_I2S);
	//uTemp = I2S_Inp32(ePort, rI2SCON);		
	//uTemp &= ~I2S_EN_OverrunIntr;			//Enable I2S Interface	
	//I2S_Outp32(ePort,  rI2SCON, uTemp);
}

void I2S_DataInDMAStop(AUDIO_PORT ePort)
{
	u32 uTemp;
	// DMA stop
    	DMACH_Stop(&(oI2SDma[ePort]));	
	//Disble Interrupt 
    	INTC_Disable(g_oI2SInfor[ePort].m_uNumDma);
	//Disable RXDMA and Pause RX Interface
	uTemp = I2S_Inp32(ePort, rI2SCON);
	uTemp &= ~ I2S_Enable_RX_DMA;
	uTemp |=  I2S_Pause_RX_IF;
	I2S_Outp32(ePort,  rI2SCON, uTemp);
	// FIFO FLUSH
	I2SFIC_AutoFlush(ePort, RXOnly);
}


void I2S_DataOutDMAStart(AUDIO_PORT ePort)
{
	u32 uTemp = 0;
	u32 uRegValue;
	u8 ucChar=0;
	if (ePort == AUDIO_PORT0)
	{
		g_oI2SInfor[ePort].m_eDmaId		= eSEL_I2S0_TX;
		g_oI2SInfor[ePort].m_eDreqSrc		= DMA0_I2S0_TX;
		//IRQ Setting
		if ( g_TestMode == I2S_SINGLE )
			INTC_SetVectAddr(g_oI2SInfor[ePort].m_uNumDma ,ISR_Port0DMAPlayDone);
		else if ( g_TestMode == I2S_DUAL )
			INTC_SetVectAddr(g_oI2SInfor[ePort].m_uNumDma ,ISR_Port0DMAPlayLoopBackDone);
	}

	else if ( ePort == AUDIO_PORT1)
	{
		g_oI2SInfor[ePort].m_eDmaId		= eSEL_I2S1_TX;
		g_oI2SInfor[ePort].m_eDreqSrc		= DMA1_I2S1_TX;
		//IRQ Setting
		if (g_TestMode == I2S_SINGLE )
			INTC_SetVectAddr(g_oI2SInfor[ePort].m_uNumDma ,ISR_Port1DMAPlayDone);
		else if (g_TestMode == I2S_DUAL )
			INTC_SetVectAddr(g_oI2SInfor[ePort].m_uNumDma ,ISR_Port1DMAPlayLoopBackDone);
	}
	
	INTC_Enable(g_oI2SInfor[ePort].m_uNumDma);	

	//INTC_SetVectAddr(NUM_I2S, ISR_I2SUnderRun);
	//INTC_Enable(NUM_I2S);
	
	uTemp = I2S_Inp32(ePort, rI2SCON);
	uTemp &= ~ I2S_Enable_TX_DMA;
	uTemp |=  I2S_Pause_TX_IF;
	I2S_Outp32(ePort,  rI2SCON, uTemp);

	//DMA Setting
	#ifdef	I2S_NORMAL_DMA
	SYSC_SelectDMA(g_oI2SInfor[ePort].m_eDmaId, 1);
	#else
	SYSC_SelectDMA(g_oI2SInfor[ePort].m_eDmaId, 0);
	#endif

	DMAC_InitCh((g_oI2SInfor[ePort].m_eDmaUnit) , g_oI2SInfor[ePort].m_eDmaCh, &(oI2SDma[ePort]));
	DMACH_ClearIntPending(&(oI2SDma[ePort]));
	DMACH_ClearErrIntPending(&(oI2SDma[ePort]));	
	if ( g_TestMode == I2S_SINGLE )
	{
		DMACH_Setup(g_oI2SInfor[ePort].m_eDmaCh, false, (u32)g_oI2SInfor[ePort].m_uRecBufferStartAddr, false,
				(u32)g_oI2SInfor[ePort].m_uI2STxFifoAddr, true, WORD, I2S_RECORD_LENGTH/4, DEMAND, 
				MEM, (DREQ_SRC)(g_oI2SInfor[ePort].m_eDreqSrc), SINGLE, &(oI2SDma[ePort]));
	}
	else if (g_TestMode == I2S_DUAL)
	{
		DMACH_Setup(g_oI2SInfor[ePort].m_eDmaCh, false, (u32)g_oI2SInfor[ePort].m_uPlayBufferStartAddr, false,
			(u32)g_oI2SInfor[ePort].m_uI2STxFifoAddr, true, WORD, I2S_RECORD_LENGTH/4, DEMAND, 
			MEM, (DREQ_SRC)(g_oI2SInfor[ePort].m_eDreqSrc), SINGLE, &(oI2SDma[ePort]));
	}
	DMACH_Start(&(oI2SDma[ePort]));
	
	I2SFIC_AutoFlush(ePort, TXOnly);					//FIFO Flush	
	
	uTemp |= I2S_Enable_TX_DMA;	
	uTemp &= ~I2S_Pause_TX_DMA; 					// Disable Underrun Interrupt, Pause RX DMA										
	I2S_Outp32(ePort, rI2SCON, uTemp);	
	do 
	{
		uRegValue = I2S_Inp32(ePort, rI2SCON);
	} while ((uRegValue & (I2S_ST_TX_FIFO_EMPTY)) != 0);			//TX FIFO Full check	
	
	//Transfer data enable  
	//Set -Up IISCON of I2S
	uTemp &= ~I2S_Pause_TX_IF & ~I2S_EN_UnderrunIntr; 				// Disable Underrun Interrupt, Pause RX DMA
	uTemp |= I2S_Enable_I2SIF;										// Enable RX DMA,IIS I/F
	I2S_Outp32(ePort, rI2SCON, uTemp);

	if ( g_TestMode == I2S_SINGLE )
	{
		UART_Printf("\nIf you want to exit, Press the 'x' key.\n");
		UART_Printf("TX DMA Pause : P, TX DMA Play O\n");
		UART_Printf("TX Interface Pause : L, TX Interface Play : K\n");
		UART_Printf("I2S Port Interface Pause : I, I2S Port Interface Pause : U\n");
		while(1)
		{		
			ucChar = UART_Getc();
			if((ucChar == 'x') | (ucChar == 'X')) 
				break;
			else if ((ucChar == 'p') | (ucChar == 'P'))
			{
				I2SCON_PauseDMA(ePort, TXOnly, PAUSE);
				UART_Printf("TX DMA PAUSE!!\n");				
			}
			else if ((ucChar == 'o') | (ucChar == 'O'))
			{
				I2SCON_PauseDMA(ePort, TXOnly, PLAY);
				UART_Printf("TX DMA PLAY!!\n");
			}
			else if ((ucChar == 'l') | (ucChar == 'L'))
			{
				I2SCON_PauseIF(ePort, TXOnly, PAUSE);
				UART_Printf("TX Interface PAUSE!!\n");
			}
			else if ((ucChar == 'k') | (ucChar == 'K'))
			{
				I2SCON_PauseIF(ePort, TXOnly, PLAY);
				UART_Printf("TX Interface PLAY!!\n");
			}
			else if ((ucChar == 'i') | (ucChar == 'I'))
			{
				I2SCON_ActiveIF(ePort,  false);
				UART_Printf("Interface Off\n");
			}
			else if ((ucChar == 'u') | (ucChar == 'U'))
			{
				I2SCON_ActiveIF(ePort,  true);
				UART_Printf("Interface On\n");
			}
		}			
	} 
	
	else
	{
		
	}
	//uTemp &= ~I2S_EN_UnderrunIntr;
	//I2S_Outp32(ePort, rI2SCON, uTemp);
}

void	I2S_DataOutDMAStop(AUDIO_PORT ePort)
{
	u32 uTemp;
	// DMA stop
    	DMACH_Stop(&(oI2SDma[ePort]));	
	//Disble Interrupt 
    	INTC_Disable(g_oI2SInfor[ePort].m_uNumDma);
		
	//Disable IISCON	
	uTemp = I2S_Inp32(ePort, rI2SCON);
	uTemp &= ~ I2S_Enable_TX_DMA;
	uTemp |=  I2S_Pause_TX_IF;
	I2S_Outp32(ePort,  rI2SCON, uTemp);

	// FIFO FLUSH
	I2SFIC_AutoFlush(ePort, TXOnly);
		
}

u32	I2S_DataBypassDMA(AUDIO_PORT ePort)
{
	u32 uTemp = 0;
	u8 ucChar=0;
	if (ePort == AUDIO_PORT0)
	{
		g_oI2SInfor[ePort].m_eDmaId		= eSEL_I2S0_TX;
		g_oI2SInfor[ePort].m_eDreqSrc		= DMA0_I2S0_TX;
		g_oI2SInfor[ePort].m_eDreqDst	= DMA0_I2S0_RX;
		//IRQ Setting
		INTC_SetVectAddr(g_oI2SInfor[ePort].m_uNumDma ,ISR_Port0DMABypass);
	}

	else if ( ePort == AUDIO_PORT1)
	{
		g_oI2SInfor[ePort].m_eDmaId		= eSEL_I2S1_TX;
		g_oI2SInfor[ePort].m_eDreqSrc		= DMA1_I2S1_TX;
		g_oI2SInfor[ePort].m_eDreqDst	= DMA1_I2S1_RX;
		//IRQ Setting
		INTC_SetVectAddr(g_oI2SInfor[ePort].m_uNumDma ,ISR_Port1DMABypass);
	}
	
	INTC_Enable(g_oI2SInfor[ePort].m_uNumDma );	
	uTemp = I2S_Inp32(ePort, rI2SCON);
	uTemp &= ~ I2S_Enable_TX_DMA;
	uTemp |=  I2S_Pause_TX_IF | I2S_Pause_RX_IF;
	I2S_Outp32(ePort,  rI2SCON, uTemp);

	//DMA Setting
	#ifdef	I2S_NORMAL_DMA
	SYSC_SelectDMA(g_oI2SInfor[ePort].m_eDmaId, 1);
	#else
	SYSC_SelectDMA(g_oI2SInfor[ePort].m_eDmaId, 0);
	#endif

	DMAC_InitCh((g_oI2SInfor[ePort].m_eDmaUnit) , g_oI2SInfor[ePort].m_eDmaCh, &(oI2SDma[ePort]));
	DMACH_ClearIntPending(&(oI2SDma[ePort]));
	DMACH_ClearErrIntPending(&(oI2SDma[ePort]));		
	DMACH_Setup(g_oI2SInfor[ePort].m_eDmaCh, 0x0, (u32)g_oI2SInfor[ePort].m_uI2SRxFifoAddr, true,
				(u32)g_oI2SInfor[ePort].m_uI2STxFifoAddr, true, WORD, I2S_RECORD_LENGTH/4, DEMAND, 
				(DREQ_SRC)(g_oI2SInfor[ePort].m_eDreqSrc), (DREQ_SRC)(g_oI2SInfor[ePort].m_eDreqDst), SINGLE, &(oI2SDma[ePort]));
	DMACH_Start(&(oI2SDma[ePort]));
	
	I2SFIC_AutoFlush(ePort, TXRXBoth);							//FIFO Flush		
	
	uTemp |= I2S_Enable_TX_DMA | I2S_Enable_RX_DMA;	
	uTemp &= ~I2S_Pause_TX_DMA & ~I2S_Pause_RX_DMA; 			// Disable Underrun Interrupt, Pause RX DMA										
	I2S_Outp32(ePort, rI2SCON, uTemp);	
	
	//Transfer data enable  
	//Set -Up IISCON of I2S
	uTemp &= ~I2S_Pause_TX_IF & ~I2S_Pause_RX_IF ; 				// Disable Underrun Interrupt, Pause RX DMA
	uTemp |= I2S_Enable_I2SIF;									// Enable RX DMA,IIS I/F
	I2S_Outp32(ePort, rI2SCON, uTemp);
	UART_Printf("Bypass Start\n");

	return true;
		
}


/*---------------------------------- APIs of rI2SCON ---------------------------------*/
//////////
// Function Name : I2SCON_SetIFMode
// Function Description : 
//   This function implements selection of Tx/Rx Transfer mode
// Input : 	AUDIO_PORT		- I2S controller number
//			I2S_IFMode		- Tx/Rx mode
// Output : NONE
// Version : v0.1

void I2SCON_SetIFMode(AUDIO_PORT ePort, I2S_IFMode eIFMode)
{
	u32 uRegValue = 0;	

	uRegValue = I2S_Inp32(ePort, rI2SCON); 
	// rI2SCON : TxDMAPause(6), RxDMAPause(5), TxCHPause(4), RxCHPause(3), TxDMAActive(2), RxDMAActive(1)
	switch(eIFMode)
	{
		case TXOnly:
			uRegValue |= I2S_Enable_TX_DMA;
			uRegValue &= ~I2S_Enable_TX_DMA & ~I2S_Pause_TX_IF & ~I2S_Pause_TX_DMA;			
			break;
		case RXOnly :
			uRegValue |= I2S_Enable_RX_DMA;
			uRegValue &= ~I2S_Enable_RX_DMA & ~I2S_Pause_RX_IF & ~I2S_Pause_RX_DMA;			
			break;
		case TXRXBoth : 
			uRegValue |= I2S_Enable_TX_DMA | I2S_Enable_RX_DMA;
			uRegValue &= ~I2S_Enable_TX_DMA & ~I2S_Pause_TX_IF & ~I2S_Enable_TX_DMA & ~I2S_Pause_TX_IF & ~I2S_Pause_TX_DMA & ~I2S_Pause_RX_DMA;						
			break;
		default :
			break;
	}	
	I2S_Outp32(ePort, rI2SCON, (uRegValue | I2S_Enable_I2SIF));	
}

void I2SCON_ActiveIF(AUDIO_PORT ePort, u8 ucActive)
{
	u32 uRegValue = 0;	

	uRegValue = I2S_Inp32(ePort, rI2SCON); 	
	if ( ucActive == true)
		uRegValue |= I2S_Enable_I2SIF;
	else
		uRegValue &= ~I2S_Enable_I2SIF;
	I2S_Outp32(ePort, rI2SCON, uRegValue);	
}

//////////
// Function Name : I2SCON_ActiveDMA
// Function Description : 
//   This function implements activation/deactivation of I2S DMA.
// Input : 	ePort		- I2S controller number
//			eXferMode	- Tx/Rx mode
// Output : NONE
// Version : v0.1
void I2SCON_ActiveDMA(AUDIO_PORT ePort, I2S_IFMode eIFMode, u8 ucActive)
{
	u32 uRegValue = 0;	

	uRegValue = I2S_Inp32(ePort, rI2SCON);
	switch(eIFMode)
	{
		case TXOnly:
			if (ucActive == ACTIVE)
				uRegValue |= I2S_Enable_TX_DMA;
			else if (ucActive == INACTIVE)
				uRegValue &= ~I2S_Enable_TX_DMA;
			break;
		case RXOnly:
			if (ucActive == ACTIVE)
				uRegValue |= I2S_Enable_RX_DMA;
			else if (ucActive == INACTIVE)
				uRegValue &= ~I2S_Enable_RX_DMA;
			break;
		case TXRXBoth:
			if (ucActive == ACTIVE)
				uRegValue &= ~I2S_Enable_TX_DMA | I2S_Enable_RX_DMA;
			else if (ucActive == INACTIVE)
				uRegValue &= ~I2S_Enable_TX_DMA & ~I2S_Enable_RX_DMA;
			break;	
	}
	I2S_Outp32(ePort, rI2SCON, uRegValue);	
}

//////////
// Function Name : I2SCON_PauseDMA
// Function Description : 
//   This function implements pause/resume of I2S Channel.
// Input : 	ePort		- I2S controller number
//			eXferMode	- Tx/Rx mode
// Output : NONE
// Version : v0.1
void I2SCON_PauseDMA(AUDIO_PORT ePort, I2S_IFMode eIFMode, u8 ucPause)
{
	u32 uRegValue = 0;	

	uRegValue = I2S_Inp32(ePort, rI2SCON);
	switch(eIFMode)
	{
		case TXOnly:
			if (ucPause == PAUSE)
				uRegValue |= I2S_Pause_TX_DMA;
			else if (ucPause == PLAY)
				uRegValue &= ~I2S_Pause_TX_DMA;
			break;
		case RXOnly:
			if (ucPause == PAUSE)
				uRegValue |= I2S_Pause_RX_DMA;
			else if (ucPause == PLAY)
				uRegValue &= ~I2S_Pause_RX_DMA;
			break;
		case TXRXBoth:
			if (ucPause == PAUSE)
				uRegValue &= ~I2S_Pause_TX_DMA | I2S_Pause_RX_DMA;
			else if (ucPause == PLAY)
				uRegValue &= ~I2S_Pause_TX_DMA & ~I2S_Pause_RX_DMA;
			break;	
	}
	I2S_Outp32(ePort, rI2SCON, uRegValue);	
}

//////////
// Function Name : I2SCON_PauseIF
// Function Description : 
//   This function implements activation/deactivation of I2S.
// Input : 	ePort		- I2S controller number
//			eXferMode	- Tx/Rx mode
// Output : NONE
// Version : v0.1
void I2SCON_PauseIF(AUDIO_PORT ePort, I2S_IFMode eIFMode, u8 ucPause)
{
	u32 uRegValue = 0;	

	uRegValue = I2S_Inp32(ePort, rI2SCON);
	switch(eIFMode)
	{
		case TXOnly:
			if (ucPause == PAUSE)
				uRegValue |= I2S_Pause_TX_IF;
			else if (ucPause == PLAY)
				uRegValue &= ~I2S_Pause_TX_IF;
			break;
		case RXOnly:
			if (ucPause == PAUSE)
				uRegValue |= I2S_Pause_RX_IF;
			else if (ucPause == PLAY)
				uRegValue &= ~I2S_Pause_RX_IF;
			break;
		case TXRXBoth:
			if (ucPause == PAUSE)
				uRegValue &= ~I2S_Pause_TX_IF | I2S_Pause_RX_IF;
			else if (ucPause == PLAY)
				uRegValue &= ~I2S_Pause_TX_IF & ~I2S_Pause_RX_IF;
			break;	
	}

	I2S_Outp32(ePort, rI2SCON, uRegValue);	
}

void I2SCON_EnableIntr(AUDIO_PORT rhs_ePort, I2S_IntrSource rhs_eISource, u8 ucActive)
{
	u32 urI2SCON;
	urI2SCON = I2S_Inp32(rhs_ePort, rI2SCON);
	switch(rhs_eISource)
	{
		case I2S_TX:
			if (ucActive == ACTIVE)			
				urI2SCON |= I2S_EN_UnderrunIntr;			
			else 			
				urI2SCON &= ~I2S_EN_UnderrunIntr;			
			break;

		
		case I2S_RX:
			if (ucActive == ACTIVE)
				urI2SCON |= I2S_EN_OverrunIntr;		
			else 
				urI2SCON &= ~I2S_EN_OverrunIntr;
			break;

	}
	I2S_Outp32(rhs_ePort, rI2SCON, urI2SCON);
}

//////////
// Function Name : I2SCON_ClrOverIntr
// Function Description : 
//   This function clears Overrun Interrupt Bit.
// Input : 	rhs_ePort		- I2S controller number//			
// Output : NONE
// Version : v0.1

void I2SCON_ClrOverIntr(AUDIO_PORT rhs_ePort)
{	
	u32 uRegValue;
	uRegValue = I2S_Inp32(rhs_ePort, rI2SCON);
	uRegValue |= I2S_CLR_OverrunIntr;
	I2S_Outp32(rhs_ePort, rI2SCON, uRegValue);
}

//////////
// Function Name : I2SCON_ClrUnderIntr
// Function Description : 
//   This function clears Underrun Interrupt Bit.
// Input : 	rhs_ePort		- I2S controller number//			
// Output : NONE
// Version : v0.1


void I2SCON_ClrUnderIntr(AUDIO_PORT rhs_ePort)
{
	u32 uRegValue;
	uRegValue = I2S_Inp32(rhs_ePort, rI2SCON);
	uRegValue |= I2S_CLR_UnderrunIntr;
	
	I2S_Outp32(rhs_ePort, rI2SCON, uRegValue);
}



/*---------------------------------- APIs of rI2SMOD ---------------------------------*/
void I2SMOD_PutData(AUDIO_PORT ePort, I2S_Infor* p_ePort)
{
	u32 uTemp;
	
	
	if(p_ePort->m_CLKSRC == I2S_PCLK)
	{
		uTemp = (p_ePort->m_eWordLength << 13) | (0 << 12) | (p_ePort->m_eOpMode << 11 ) | ( 0 << 10) |
				(p_ePort->m_eIFMode << 8) | (p_ePort->m_eLRCLKPolarity  << 7) | (p_ePort->m_eDataFormat << 5) |
				(p_ePort->m_eRFS << 3) | (p_ePort->m_eBFS << 1);
		
		
	}
	else if (p_ePort->m_CLKSRC == I2S_EXTERNALCDCLK)
	{
		uTemp = (p_ePort->m_eWordLength << 13) | (1 << 12) | (p_ePort->m_eOpMode << 11 ) | ( 1 << 10) |
				(p_ePort->m_eIFMode << 8) | (p_ePort->m_eLRCLKPolarity << 7) | (p_ePort->m_eDataFormat << 5) |
				(p_ePort->m_eRFS << 3) | (p_ePort->m_eBFS << 1);
	}
	else 
	{
		uTemp = (p_ePort->m_eWordLength << 13) | (0 << 12) | (p_ePort->m_eOpMode << 11 ) | ( 1 << 10) |
				(p_ePort->m_eIFMode << 8) | (p_ePort->m_eLRCLKPolarity << 7) | (p_ePort->m_eDataFormat << 5) |
				(p_ePort->m_eRFS << 3) | (p_ePort->m_eBFS << 1);
	}
	I2S_Outp32(ePort, rI2SMOD, uTemp);
}

void I2SMOD_ChangeWordLength(AUDIO_PORT ePort)
{
	u32 uSelection;
	UART_Printf("Select Word Length\n");
	UART_Printf("1. 16bit[D]		2. 8bit		3. 24bit\n");
	uSelection = UART_GetIntNum();

	if (uSelection == 2)
		g_oI2SInfor[ePort].m_eWordLength = Word8;
	else if (uSelection == 3) 
		g_oI2SInfor[ePort].m_eWordLength = Word24;
	else
		g_oI2SInfor[ePort].m_eWordLength = Word16;
}

void I2SMOD_SetWordLength(AUDIO_PORT ePort, PCMWordLength eWordL)
{
	u32 urI2SMOD = I2S_Inp32(ePort, rI2SMOD) & ~(0x3 << 13);
	I2S_Infor* p_Infor = I2S_GetI2SInfor(ePort);

	p_Infor->m_eWordLength = eWordL;
	urI2SMOD |= ((u32) eWordL << 13);

	I2S_Outp32(ePort, rI2SMOD, urI2SMOD);
}

void I2S_Sel_CLKSource(AUDIO_PORT ePort)
{
	u32 uSelection;
	UART_Printf("Select Serial Data Format\n");
	UART_Printf("1. EPLL[D]		2. MPLL		3. Oscillator(Fin of Syscon)\n");
	UART_Printf("4. PCLK		5. External Codec CLK\n");
	uSelection = UART_GetIntNum();

	if (uSelection == 2)
		g_oI2SInfor[ePort].m_CLKSRC = I2S_DOUT_MPLL;
	else if (uSelection == 3) 
		g_oI2SInfor[ePort].m_CLKSRC = I2S_FIN;
	else	 if (uSelection == 4) 
		g_oI2SInfor[ePort].m_CLKSRC = I2S_PCLK;
	else	 if (uSelection == 5) 
		g_oI2SInfor[ePort].m_CLKSRC = I2S_EXTERNALCDCLK;
	else	 
		g_oI2SInfor[ePort].m_CLKSRC = I2S_MOUT_EPLL;
	
	I2S_SelCLKAUDIO(ePort, g_oI2SInfor[ePort].m_CLKSRC);
}

void I2SMOD_SetCLKSource(AUDIO_PORT ePort, I2S_CLKSRC eCLKSRC)
{
	I2S_Infor* p_Infor = I2S_GetI2SInfor(ePort);
	u32 urI2SMOD = I2S_Inp32(ePort, rI2SMOD) & ~(0x1 << 12);

	p_Infor->m_CLKSRC = eCLKSRC;
	urI2SMOD |= ((u32) eCLKSRC << 12);

	I2S_Outp32(ePort, rI2SMOD, urI2SMOD);
}


void I2SMOD_ChangeInterface(AUDIO_PORT ePort)
{
	u32 uSelection;
	UART_Printf("Select Interface\n");
	UART_Printf("1. TX_RX Simultaneously[D]		2. TX Only		3. RX Only\n");
	uSelection = UART_GetIntNum();

	if (uSelection == 2)
		g_oI2SInfor[ePort].m_eIFMode = TXOnly;
	else if (uSelection == 3) 
		g_oI2SInfor[ePort].m_eIFMode = RXOnly;
	else
		g_oI2SInfor[ePort].m_eIFMode = TXRXBoth;
}


void I2SMOD_SetTXR(AUDIO_PORT ePort, I2S_IFMode eIFMode)
{
	u32 uTemp;
	uTemp= I2S_Inp32(ePort, rI2SMOD);

	uTemp &= ~(0x3 << 8);
	uTemp |= ((u32) eIFMode) << 8;

	I2S_Outp32(ePort, rI2SMOD, uTemp);
	
}

void I2S_SelOperationMode(AUDIO_PORT ePort)
{
	u32 uSelection;
	UART_Printf("Select Opeation Mode\n");
	UART_Printf("1. Maseter[D]		2. Slave\n");
	uSelection = UART_GetIntNum();

	if ( uSelection == 2)
		g_oI2SInfor[ePort].m_eOpMode = Slave;
	else 
		g_oI2SInfor[ePort].m_eOpMode = Master;		
}

void I2S_SetOperationMode(AUDIO_PORT ePort, OPMode eOpMode)
{
	
	
	if ( eOpMode == Slave)	
		g_oI2SInfor[ePort].m_eOpMode = Slave;
	
	else 
		g_oI2SInfor[ePort].m_eOpMode = Master;	
	
	I2SMOD_PutData(ePort, I2S_GetI2SInfor(ePort));		
}


void I2S_Sel_LRCLKPolarity(AUDIO_PORT ePort)
{
	u32 uSelection;
	UART_Printf("Select LRCLK Polarity\n");
	UART_Printf("1. Right High[D]		2. Left High\n");
	uSelection = UART_GetIntNum();

	if (uSelection == 2)
		g_oI2SInfor[ePort].m_eLRCLKPolarity = LeftHigh;
	else 
		g_oI2SInfor[ePort].m_eLRCLKPolarity = RightHigh;
	
}

void I2S_Sel_SerialDataFormat(AUDIO_PORT ePort)
{
	u32 uSelection;
	UART_Printf("Select Serial Data Format\n");
	UART_Printf("1. IIS Format[D]		2. MSB Justified	3. LSB Justified\n");
	uSelection = UART_GetIntNum();

	if (uSelection == 2)
		g_oI2SInfor[ePort].m_eDataFormat = MSBJustified;
	else if (uSelection == 3) 
		g_oI2SInfor[ePort].m_eDataFormat = LSBJustified;
	else
		g_oI2SInfor[ePort].m_eDataFormat = I2SFormat;
}



void I2S_Sel_RFS(AUDIO_PORT ePort)
{
	u32 uSelection;
	UART_Printf("Select LRCLK Length(RFS)\n");
	UART_Printf("1. 16bit(256fs)[D]		2. 32bit(512fs)		3. 12bit(384fs)		4. 24bit(768fs)\n");
	uSelection = UART_GetIntNum();

	if (uSelection == 2)
		g_oI2SInfor[ePort].m_eRFS = RFS_512fs;
	else if (uSelection == 3) 
		g_oI2SInfor[ePort].m_eRFS = RFS_384fs;
	else	 if (uSelection == 4) 
		g_oI2SInfor[ePort].m_eRFS = RFS_768fs;
	else	 
		g_oI2SInfor[ePort].m_eRFS = RFS_256fs;

}

void I2S_Sel_BFS(AUDIO_PORT ePort)
{
	u32 uSelection;
	UART_Printf("Select Effective Data Length(BFS)\n");
	UART_Printf("1. 16bit(32fs)[D]		2. 24bit(48fs)		3. 8bit(16fs)		4. 12bit(24fs)\n");
	uSelection = UART_GetIntNum();

	if (uSelection == 2)
		g_oI2SInfor[ePort].m_eBFS = BFS_48fs;
	else if (uSelection == 3) 
		g_oI2SInfor[ePort].m_eBFS = BFS_16fs;
	else	 if (uSelection == 4) 
		g_oI2SInfor[ePort].m_eBFS = BFS_24fs;
	else	 
		g_oI2SInfor[ePort].m_eBFS = BFS_32fs;

}
	
void I2S_SamplingFreq(AUDIO_PORT ePort)
{
	
	u32 uSelection;
	UART_Printf("Select Sampling Frequency[KHz]\n");
	UART_Printf("1. 8KHz		2. 11.025KHz		3. 16KHz		4. 22.05KHz\n");
	UART_Printf("5. 32KHz		6. 44.1KHz[D]		7. 48KHz		8. 64KHz \n");
	UART_Printf("9. 88.2Khz		10.96KHz		11. 192KHz\n"); 
	uSelection = UART_GetIntNum();

	if (uSelection == 1)
		g_oI2SInfor[ePort].m_dSamplingFreq = 8000;
	else if (uSelection == 2) 
		g_oI2SInfor[ePort].m_dSamplingFreq = 11025;
	else	 if (uSelection == 3) 
		g_oI2SInfor[ePort].m_dSamplingFreq = 16000;
	else	 if (uSelection == 3) 
		g_oI2SInfor[ePort].m_dSamplingFreq = 16000;
	else	 if (uSelection == 4) 
		g_oI2SInfor[ePort].m_dSamplingFreq = 22050;
	else	 if (uSelection == 5) 
		g_oI2SInfor[ePort].m_dSamplingFreq = 32000;
	else	 if (uSelection == 7) 
		g_oI2SInfor[ePort].m_dSamplingFreq = 48000;
	else	 if (uSelection == 8) 
		g_oI2SInfor[ePort].m_dSamplingFreq = 64000;
	else	 if (uSelection == 9) 
		g_oI2SInfor[ePort].m_dSamplingFreq = 88200;
	else	 if (uSelection == 10) 
		g_oI2SInfor[ePort].m_dSamplingFreq = 96000;
	else	 if (uSelection == 11) 
		g_oI2SInfor[ePort].m_dSamplingFreq = 192000;
	else	 
		g_oI2SInfor[ePort].m_dSamplingFreq = 44100;

	
}

/*---------------------------------- APIs of rI2SFIC ---------------------------------*/
//////////
// Function Name : I2SFIC_AutoFlush
// Function Description : 
//   This function implements the I2S FIFO Flush.
// Input : ePort 		- I2S port number
//		 eIFMode		- I2S Interface
// Output : NONE
// Version : v0.1
void I2SFIC_AutoFlush(AUDIO_PORT ePort, I2S_IFMode eIFMode)
{
	u32 uRegValue = 0;	

	// FIFO Flush
	uRegValue = I2S_Inp32(ePort, rI2SFIC);

	if ( eIFMode == TXOnly )
	{
		uRegValue |= I2S_Flush_TX_FIFO;
	}
	else if ( eIFMode == RXOnly )
	{
		uRegValue |= I2S_Flush_TX_FIFO;
	}
	else if ( eIFMode == TXRXBoth )
	{
		uRegValue |= I2S_Flush_TX_FIFO | I2S_Flush_RX_FIFO;
	}
	I2S_Outp32(ePort, rI2SFIC, uRegValue);
	I2S_Outp32(ePort, rI2SFIC, 0);
}

/*---------------------------------- APIs of rI2SPSR ---------------------------------*/
//////////
// Function Name : I2SPSR_SetPrescaler
// Function Description : 
//   This function implements the I2S Clock Prescaler.
// Input : ePort 		- I2S controller number
//			ucPSRActiveFlag	- Pre-scaler Active
//			usPSRDivValue	- Pre-scaler division value.
// Output : NONE
// Version : v0.1
void I2SPSR_SetPrescaler(AUDIO_PORT ePort, u32 rhs_uPrsVal)
{
	u32 uTemp = 0;
	uTemp= (I2S_Enable_Prescaler) | (rhs_uPrsVal << 8);
	I2S_Outp32(ePort, rI2SPSR, uTemp);
}
void I2SPSR_ActivePRS(AUDIO_PORT ePort, u8 uActive)
{
	u32 uTemp;
	uTemp = I2S_Inp32(ePort, rI2SPSR);
	if (uActive == ACTIVE)
		uTemp |= (I2S_Enable_Prescaler);
	else
		uTemp &= ~(I2S_Enable_Prescaler);
	I2S_Outp32(ePort, rI2SPSR, uTemp);
}
void I2SPSR_PutPRSValue(AUDIO_PORT ePort, u32 rhs_uPrsVal)
{
	u32 uTemp;
	uTemp = I2S_Inp32(ePort, rI2SPSR) & ~(0x3f << 8);	
	uTemp |=(rhs_uPrsVal << 8);
	I2S_Outp32(ePort, rI2SPSR, uTemp);
}

/*---------------------------------- APIs of rI2STXD ---------------------------------*/

/*---------------------------------- APIs of rI2SRXD ---------------------------------*/

/*---------------------------------- ISR Routines ---------------------------------*/

void __irq ISR_I2SPort0_RXOverrun(void)
{
	AUDIO_PORT ePort = AUDIO_PORT0;


	INTC_Disable(NUM_I2S);
	
	
	do{
		*(g_pRecAddr[ePort]++) = I2S_Inp32(ePort, rI2SRXD);
	}while(!(I2S_Inp32(ePort, rI2SCON) & I2S_Status_RX_FIFO_EMPTY));
	
	I2SCON_ClrOverIntr(ePort);
	INTC_ClearVectAddr();	
	
	
	if ( (u32) g_pRecAddr[ePort] >= ((u32) g_oI2SInfor[ePort].m_uRecBufferStartAddr + (u32) g_oI2SInfor[ePort].m_uPcmDataSize))
		g_uI2SRecDone[ePort] = 1;
	else
		INTC_Enable(NUM_I2S);
}

void __irq ISR_I2SPort1_RXOverrun(void)
{
	AUDIO_PORT ePort = AUDIO_PORT1;
	INTC_Disable(NUM_I2S);
	
	
	do{
		*(g_pRecAddr[ePort]++) = I2S_Inp32(ePort, rI2SRXD);
	}while(!(I2S_Inp32(ePort, rI2SCON) & I2S_Status_RX_FIFO_EMPTY));
	
	I2SCON_ClrOverIntr(ePort);
	INTC_ClearVectAddr();
	
	
	if ( (u32) g_pRecAddr[ePort] >= (u32) g_oI2SInfor[ePort].m_uRecBufferStartAddr + g_oI2SInfor[ePort].m_uPcmDataSize)
		g_uI2SRecDone[ePort] = 1;
	else
		INTC_Enable(NUM_I2S);

}


void __irq ISR_I2SPort0_TXUnderrun(void)
{
	AUDIO_PORT ePort = AUDIO_PORT0;
	
	INTC_Disable(NUM_I2S);	

	do{
		I2S_Outp32(ePort, rI2STXD,*(g_pPlayAddr[ePort]++));
	}while(!(I2S_Inp32(ePort, rI2SCON) & I2S_Status_TX_FIFO_FULL));
	
	INTC_ClearVectAddr();
	I2SCON_ClrUnderIntr(ePort);

	if ( (u32) g_pPlayAddr[ePort] >= ((u32) g_oI2SInfor[ePort].m_uPlayBufferStartAddr + (u32) g_oI2SInfor[ePort].m_uPcmDataSize))
		g_uPlayDone[ePort] = 1;
	else
		INTC_Enable(NUM_I2S);
}

void __irq ISR_I2SPort1_TXUnderrun(void)
{
	AUDIO_PORT ePort = AUDIO_PORT1;
	INTC_Disable(NUM_I2S);
	

	do{
		I2S_Outp32(ePort, rI2STXD,*(g_pPlayAddr[ePort]++));
	}while(!(I2S_Inp32(ePort, rI2SCON) & I2S_Status_TX_FIFO_FULL));
	
	I2SCON_ClrUnderIntr(ePort);
	INTC_ClearVectAddr();	

	if ( (u32) g_pPlayAddr[ePort] >= ((u32) g_oI2SInfor[ePort].m_uPlayBufferStartAddr + (u32) g_oI2SInfor[ePort].m_uPcmDataSize))
		g_uPlayDone[ePort] = 1;
	else
		INTC_Enable(NUM_I2S);
}

void __irq ISR_I2SOverRun(void)
{
	u32 urI2SCON;
	INTC_Disable(NUM_I2S);

	if ( I2S_Inp32(AUDIO_PORT0, rI2SCON) & I2S_CLR_OverrunIntr )
	{
		UART_Printf("\nI2S Port0 RX Over Run!!\n");
		I2SCON_ClrOverIntr(AUDIO_PORT0);

	}
	if (I2S_Inp32(AUDIO_PORT1, rI2SCON) & I2S_CLR_OverrunIntr)
	{
		UART_Printf("\nI2S Port1 RX Over Run!!\n");
		I2SCON_ClrOverIntr(AUDIO_PORT1);
	}	
	INTC_ClearVectAddr();	

	INTC_Enable(NUM_I2S);
}

void __irq ISR_I2SUnderRun(void)
{
	INTC_Disable(NUM_I2S);
	if ( I2S_Inp32(AUDIO_PORT0, rI2SCON) & I2S_CLR_UnderrunIntr )
	{
		UART_Printf("\nI2S Port0 TX Under Run!!\n");
		I2SCON_ClrUnderIntr(AUDIO_PORT0);
	}
	if (I2S_Inp32(AUDIO_PORT1, rI2SCON) & I2S_CLR_UnderrunIntr)
	{
		UART_Printf("\nI2S Port1 TX Under Run!!\n");
		I2SCON_ClrUnderIntr(AUDIO_PORT1);
	}
	
	INTC_ClearVectAddr();	
	INTC_Enable(NUM_I2S);

}

//////////
// Function Name : ISR_DMA1INTRecDone
// Function Description : This function implements ISR of DMA Record Interrupt.
// Input : 	NONE
// Output : 	NONE
// Version : v0.1
void __irq ISR_Port0DMARecDone(void)
{
	INTC_Disable(g_oI2SInfor[AUDIO_PORT0].m_uNumDma);

	// Interrupt Clear
	DMACH_ClearIntPending(&(oI2SDma[AUDIO_PORT0]));
	DMACH_ClearErrIntPending(&(oI2SDma[AUDIO_PORT0]));
	INTC_ClearVectAddr();
	g_uI2SRecDone[AUDIO_PORT0] = 1;

	I2SCON_ActiveDMA(AUDIO_PORT0, RXOnly, INACTIVE);
	I2SCON_ActiveIF(AUDIO_PORT0, INACTIVE);
	//INTC_Enable(g_oI2SInfor[AUDIO_PORT0].m_uNumDma);	
	
	
	UART_Printf("\nRec Done\n");
}

void __irq ISR_Port1DMARecDone(void)
{
	INTC_Disable(g_oI2SInfor[AUDIO_PORT1].m_uNumDma);

	// Interrupt Clear
	DMACH_ClearIntPending(&(oI2SDma[AUDIO_PORT1]));
	DMACH_ClearErrIntPending(&(oI2SDma[AUDIO_PORT1]));

	I2SCON_ActiveDMA(AUDIO_PORT1, RXOnly, INACTIVE);
	I2SCON_ActiveIF(AUDIO_PORT1, INACTIVE);

	
	UART_Printf("\nRec Done\n");
	INTC_Enable(g_oI2SInfor[AUDIO_PORT1].m_uNumDma);

	g_uI2SRecDone[AUDIO_PORT1] = 1;

	INTC_ClearVectAddr();
}

//////////
// Function Name : ISR_DMA1INTPlayDone
// Function Description : This function implements ISR of DMA Play Interrupt.
// Input : 	NONE
// Output : 	NONE
// Version : v0.1
void __irq ISR_Port0DMAPlayDone(void)
{
	INTC_Disable(g_oI2SInfor[AUDIO_PORT0].m_uNumDma);
	I2SCON_ActiveIF(AUDIO_PORT0, INACTIVE);
	// Interrupt Clear
	DMACH_ClearIntPending(&(oI2SDma[AUDIO_PORT0]));
	DMACH_ClearErrIntPending(&(oI2SDma[AUDIO_PORT0]));
	#if 0
	
	#else
	DMACH_Setup(g_oI2SInfor[AUDIO_PORT0].m_eDmaCh, false, (u32)g_oI2SInfor[AUDIO_PORT0].m_uRecBufferStartAddr, false,
			(u32)g_oI2SInfor[AUDIO_PORT0].m_uI2STxFifoAddr, true, WORD, I2S_RECORD_LENGTH/4, DEMAND, 
			MEM, (DREQ_SRC)(g_oI2SInfor[AUDIO_PORT0].m_eDreqSrc), SINGLE, &(oI2SDma[AUDIO_PORT0]));	
	#endif
	UART_Printf("TX DMA Done ~~~\n");	
	DMACH_Start(&(oI2SDma[AUDIO_PORT0]));	
	I2SCON_ActiveIF(AUDIO_PORT0, ACTIVE);
	INTC_Enable(g_oI2SInfor[AUDIO_PORT0].m_uNumDma);
	INTC_ClearVectAddr();
}

void __irq ISR_Port0DMAPlayLoopBackDone(void)
{
	INTC_Disable(g_oI2SInfor[AUDIO_PORT0].m_uNumDma);
	
	// Interrupt Clear
	DMACH_ClearIntPending(&(oI2SDma[AUDIO_PORT0]));
	DMACH_ClearErrIntPending(&(oI2SDma[AUDIO_PORT0]));
	
	UART_Printf("PLAY DMA Done ~~~\n");
	I2SCON_ActiveDMA(AUDIO_PORT0, TXOnly, INACTIVE);
	I2SCON_ActiveIF(AUDIO_PORT0, INACTIVE);

	INTC_Enable(g_oI2SInfor[AUDIO_PORT0].m_uNumDma);

	INTC_ClearVectAddr();
	
}

void __irq ISR_Port1DMAPlayDone(void)
{
	INTC_Disable(g_oI2SInfor[AUDIO_PORT1].m_uNumDma);
	I2SCON_ActiveIF(AUDIO_PORT1, INACTIVE);

	// Interrupt Clear
	DMACH_ClearIntPending(&(oI2SDma[AUDIO_PORT1]));
	DMACH_ClearErrIntPending(&(oI2SDma[AUDIO_PORT1]));
	#if 0
	
	#else
	DMACH_Setup(g_oI2SInfor[AUDIO_PORT1].m_eDmaCh, false, (u32)g_oI2SInfor[AUDIO_PORT1].m_uRecBufferStartAddr, false,
				(u32)g_oI2SInfor[AUDIO_PORT1].m_uI2STxFifoAddr, true, WORD, I2S_RECORD_LENGTH/4, DEMAND, 
				MEM, (DREQ_SRC)(g_oI2SInfor[AUDIO_PORT1].m_eDreqSrc), SINGLE, &(oI2SDma[AUDIO_PORT1]));
	#endif
	UART_Printf("TX DMA Done ~~~\n");
	DMACH_Start(&(oI2SDma[AUDIO_PORT1]));	
	I2SCON_ActiveIF(AUDIO_PORT1, ACTIVE);
	INTC_Enable(g_oI2SInfor[AUDIO_PORT1].m_uNumDma);
	INTC_ClearVectAddr();
}

void __irq ISR_Port1DMAPlayLoopBackDone(void)
{
	INTC_Disable(g_oI2SInfor[AUDIO_PORT1].m_uNumDma);
	
	// Interrupt Clear
	DMACH_ClearIntPending(&(oI2SDma[AUDIO_PORT1]));
	DMACH_ClearErrIntPending(&(oI2SDma[AUDIO_PORT1]));
	
	UART_Printf("PLAY DMA Done ~~~\n");
	I2SCON_ActiveDMA(AUDIO_PORT1, TXOnly, INACTIVE);
	I2SCON_ActiveIF(AUDIO_PORT1, INACTIVE);

	INTC_Enable(g_oI2SInfor[AUDIO_PORT1].m_uNumDma);

	INTC_ClearVectAddr();
	
}

void __irq ISR_Port0DMABypass(void)
{
	INTC_Disable(g_oI2SInfor[AUDIO_PORT0].m_uNumDma);

	// Interrupt Clear
	DMACH_ClearIntPending(&(oI2SDma[AUDIO_PORT0]));
	DMACH_ClearErrIntPending(&(oI2SDma[AUDIO_PORT0]));	
	
	DMACH_Setup(g_oI2SInfor[AUDIO_PORT0].m_eDmaCh, 0x0, (u32)g_oI2SInfor[AUDIO_PORT0].m_uI2SRxFifoAddr, true,
				(u32)g_oI2SInfor[AUDIO_PORT0].m_uI2STxFifoAddr, true, WORD, I2S_RECORD_LENGTH/4, DEMAND, 
				(DREQ_SRC)(g_oI2SInfor[AUDIO_PORT0].m_eDreqSrc), DMA0_I2S0_RX, SINGLE, &(oI2SDma[AUDIO_PORT0]));

	DMACH_Start(&(oI2SDma[AUDIO_PORT0]));	
	INTC_Enable(g_oI2SInfor[AUDIO_PORT0].m_uNumDma);	
	INTC_ClearVectAddr();	
#if 0

#endif
	
}

void __irq ISR_Port1DMABypass(void)
{
	INTC_Disable(g_oI2SInfor[AUDIO_PORT1].m_uNumDma);

	// Interrupt Clear
	DMACH_ClearIntPending(&(oI2SDma[AUDIO_PORT1]));
	DMACH_ClearErrIntPending(&(oI2SDma[AUDIO_PORT1]));	
	
	DMACH_Setup(g_oI2SInfor[AUDIO_PORT1].m_eDmaCh, 0x0, (u32)g_oI2SInfor[AUDIO_PORT1].m_uI2SRxFifoAddr, true,
				(u32)g_oI2SInfor[AUDIO_PORT1].m_uI2STxFifoAddr, true, WORD, I2S_RECORD_LENGTH/4, DEMAND, 
				(DREQ_SRC)(g_oI2SInfor[AUDIO_PORT1].m_eDreqSrc), DMA1_I2S1_RX, SINGLE, &(oI2SDma[AUDIO_PORT1]));

	DMACH_Start(&(oI2SDma[AUDIO_PORT1]));	
	INTC_Enable(g_oI2SInfor[AUDIO_PORT1].m_uNumDma);	
	INTC_ClearVectAddr();	
#if 0

#endif
	
}



void I2S_Outp32(AUDIO_PORT  rhs_ePort, I2S_SFR rhs_eOffset, u32 rhs_uTemp) 
{
	if (rhs_ePort == AUDIO_PORT0)
		Outp32(I2S0_BASE+rhs_eOffset, rhs_uTemp);
	else
		Outp32(I2S1_BASE+rhs_eOffset, rhs_uTemp);
}

u32  I2S_Inp32(AUDIO_PORT rhs_ePort, I2S_SFR rhs_offset)
{
	if (rhs_ePort == AUDIO_PORT0)
		return Inp32(I2S0_BASE+rhs_offset);
	else 
		return Inp32(I2S1_BASE+rhs_offset);
}

u16 I2S_Inp16(AUDIO_PORT rhs_ePort, I2S_SFR rhs_offset)
{
	if (rhs_ePort == AUDIO_PORT0)
		return Inp16(I2S0_BASE+rhs_offset);
	else
		return Inp16(I2S1_BASE+rhs_offset);
}
/*---------------------------------- APIs of Clock ---------------------------------*/
//Selection CLK
void I2S_SelCLKAUDIO(AUDIO_PORT ePort, I2S_CLKSRC eClkSrc)
{
	u32 uClkSrc, uEpllCon0;

	uClkSrc = Inp32SYSC(0x1C);
	uEpllCon0 = Inp32SYSC(0x14);

	if(ePort == AUDIO_PORT0)
	{
		if(eClkSrc == I2S_MOUT_EPLL) 
		{
			Outp32SYSC(0x14, uEpllCon0 |(1<<31));
			Delay(100);
			Outp32SYSC(0x1C, uClkSrc & ~(0x7<<7) |(1<<2));	
		}
		else if (eClkSrc == I2S_DOUT_MPLL) 		Outp32SYSC(0x1C, uClkSrc & ~(0x7<<7)|(1<<7));
		else if (eClkSrc == I2S_FIN) 				Outp32SYSC(0x1C, uClkSrc & ~(0x7<<7)|(2<<7));
		else if (eClkSrc == I2S_EXTERNALCDCLK) 	Outp32SYSC(0x1C, uClkSrc & ~(0x7<<7)|(3<<7));
	}

	else if(ePort == AUDIO_PORT1)
	{
		if(eClkSrc == I2S_MOUT_EPLL) 
		{
			Outp32SYSC(0x14, uEpllCon0 |(1<<31));
			Delay(100);
			Outp32SYSC(0x1C, uClkSrc & ~(0x7<<10)|(1<<2));
		}
		else if (eClkSrc == I2S_DOUT_MPLL)			Outp32SYSC(0x1C, uClkSrc & ~(0x7<<10)|(1<<10));
		else if (eClkSrc == I2S_FIN) 				Outp32SYSC(0x1C, uClkSrc & ~(0x7<<10)|(2<<10));
		else if (eClkSrc == I2S_EXTERNALCDCLK) 	Outp32SYSC(0x1C, uClkSrc & ~(0x7<<10)|(3<<10));
	}
}

u32 I2S_GetPRSVal(AUDIO_PORT ePort)
{
	double dVclkSrc, dTempVal;
	double dTempCDCLK;
	u32 uDivAudio;
	
	if(g_oI2SInfor[ePort].m_CLKSRC == I2S_MOUT_EPLL)
		dVclkSrc = I2S_SetEPLL(ePort); 	//should be changed according to your system clock condition : 8800000*3.35
	else if(g_oI2SInfor[ePort].m_CLKSRC == I2S_DOUT_MPLL)
		dVclkSrc = I2S_GetMPLL(ePort); 	//should be changed according to your system clock condition : 266000000
	else if(g_oI2SInfor[ePort].m_CLKSRC == I2S_FIN)
		dVclkSrc =  FIN; 	//should be changed according to your system clock condition
	else if(g_oI2SInfor[ePort].m_CLKSRC == I2S_EXTERNALCDCLK)
		dVclkSrc = I2S_EXTERNALCLK;	
	else if(g_oI2SInfor[ePort].m_CLKSRC == I2S_PCLK)
		dVclkSrc = g_PCLK;		//should be changed according to your system clock condition
	uDivAudio = (Inp32SYSC(0x028) >> (8 + (4 * ePort)) )& 0xf;
	dVclkSrc = dVclkSrc / (uDivAudio + 1);
	
	if (g_oI2SInfor[ePort].m_eRFS == RFS_512fs)
		dTempCDCLK = g_oI2SInfor[ePort].m_dSamplingFreq * 512;
	else if (g_oI2SInfor[ePort].m_eRFS == RFS_256fs)
		dTempCDCLK = g_oI2SInfor[ePort].m_dSamplingFreq * 256;
	else if (g_oI2SInfor[ePort].m_eRFS == RFS_384fs)
		dTempCDCLK = g_oI2SInfor[ePort].m_dSamplingFreq * 384;
	else if (g_oI2SInfor[ePort].m_eRFS == RFS_768fs)
		dTempCDCLK = g_oI2SInfor[ePort].m_dSamplingFreq * 768;
	
	dTempVal = dVclkSrc/dTempCDCLK;		
	
	if ( ((int)(dTempVal*10) % 10) < 5)		//Round off 
		dTempVal = dTempVal-1;
	
	UART_Printf("Source CLK is %3.3fMHz\n", dVclkSrc / 1000000);
	if ( g_oI2SInfor[ePort].m_eOpMode == Master)
	{
		UART_Printf("User Sets IISCDCLK is %3.3fMHz\n",(double)(dTempCDCLK/1000000));
		UART_Printf("Prescaler Value is 0x%x\n", (u32) dTempVal);	
		UART_Printf("IISCDCLK is %3.3fMHz\n",(dVclkSrc/(((int)dTempVal)+1))/1000000);
	}
	
	return (u32)(dTempVal);
	
}

u32 I2S_GetPRSVal2(AUDIO_PORT ePort, u32 uSourceCLK, u32 uTargetCLK)
{
	double dVclkSrc, dTempVal;
	double dTempCDCLK;

	dVclkSrc = (double) uSourceCLK;
	dTempCDCLK = (double) uTargetCLK;

	dTempVal = dVclkSrc/dTempCDCLK;		

	if ( ((int)(dTempVal*10) % 10) < 5)		//Round off 
		dTempVal = dTempVal-1;
	
	UART_Printf("Source CLK is %3.3fMHz\n", dVclkSrc / 1000000);
	
	UART_Printf("User Sets IISCDCLK is %3.3fMHz\n",(double)(dTempCDCLK/1000000));
	UART_Printf("Prescaler Value is 0x%x\n", (u32) dTempVal);	
	UART_Printf("IISCDCLK is %3.3fMHz\n",(dVclkSrc/(((int)dTempVal)+1))/1000000);
	
	
	return (u32)(dTempVal);

}
//////////
// Function Name : I2S_SetEPLL
// Function Description : 
//   This function change M,P,S,K Value of EPLL.
// Input : 	ePort -> Sample Frequency, RFS, BFS 
// Output : Output Frequency of EPLL
// Version : 
// Example :
//	
//	RFS	: 384fs ( I2SCDCLK/384 = 44.09KHz)
//	BFS : 48fs (RFS*48 = 2.116MHz)
double I2S_SetEPLL(AUDIO_PORT ePort)
{
	double fFOUTEpll, fDivdValue, fTemp;
	u8 ucSel = 0;
	u16 uPdiv, uMdiv, uSdiv, uKdiv;
	u32 uEPLLCON0, uEPLLCON1, uDivdValue;	
	if (g_oI2SInfor[ePort].m_eRFS == RFS_512fs)
		fFOUTEpll = g_oI2SInfor[ePort].m_dSamplingFreq * 512;
	else if (g_oI2SInfor[ePort].m_eRFS == RFS_256fs)
		fFOUTEpll = g_oI2SInfor[ePort].m_dSamplingFreq * 256;
	else if (g_oI2SInfor[ePort].m_eRFS == RFS_384fs)
		fFOUTEpll = g_oI2SInfor[ePort].m_dSamplingFreq * 384;
	else if (g_oI2SInfor[ePort].m_eRFS == RFS_768fs)
		fFOUTEpll = g_oI2SInfor[ePort].m_dSamplingFreq * 768;
	
	fTemp = 73728000;
	while(1)
	{
		fDivdValue = fTemp  / fFOUTEpll;
		uDivdValue = (u32) fDivdValue;
		fTemp = fDivdValue - (double) uDivdValue;
		if (fTemp < 0.0001 || fTemp > 0.9999)
		{
			fFOUTEpll = fFOUTEpll*fDivdValue;
			UART_Printf("CLKAUDIO%d out is %3.3fMHz\n", ePort, fFOUTEpll/1000000);
			break;
		}
		switch(ucSel)
		{
			case 0:
				fTemp = 67738000;
				ucSel++;
				break;
			case 1:
				fTemp = 49152000;
				ucSel++;
				break;
			case 2:
				fTemp = 45158000;
				ucSel++;
				break;
			case 3:
				fTemp = 32768000;
				ucSel++;
				break;
		}
	}

	uEPLLCON0 = Inp32SYSC(0x014);
	uEPLLCON1 = Inp32SYSC(0x018);
	uDivdValue = Inp32SYSC(0x01c);
	uEPLLCON0 &=  ~(0xff << 16) & ~( 0x3f << 8 ) & ~(0x7);
	uEPLLCON1 &= ~(0xffff);	
	switch(ucSel)
	{
		case 0:				//73.728MHz : 96

			uMdiv = 49;
			uKdiv = 9961;
			uPdiv = 1;	
			uSdiv = 3;
			
			//uMdiv = 24;
			//uKdiv = 37748;
			//uPdiv = 1;	
			//uSdiv = 2;
			break;
		case 1:				//67.738MHz 88.2K
			uMdiv = 45;
			uKdiv = 10398;
			uPdiv = 1;	
			uSdiv = 3;			
			break;
		case 2:				//49.152MHz 64KHz
			uMdiv = 32;
			uKdiv = 50297;
			uPdiv = 2;	
			uSdiv = 2;
			break;
		case 3:				//45.158MHz
			uMdiv = 90;
			uKdiv = 20709;
			uPdiv = 3;	
			uSdiv = 3;
			break;
		case 4:				//32.768MHz
			uMdiv = 21;
			uKdiv = 55400;
			uPdiv = 1;	
			uSdiv = 3;
			break;
	}
	uEPLLCON0 |= (uMdiv << 16) | (uPdiv << 8) | (uSdiv<<0);
	uEPLLCON1 |= (uKdiv << 0);
	uDivdValue |= 1 << 2;
	Outp32SYSC(0x014, uEPLLCON0);
	Outp32SYSC(0x018, uEPLLCON1);
	Outp32SYSC(0x01c, uDivdValue);	
	Delay(1000);	
	SYSC_CtrlCLKOUT(eCLKOUT_EPLLOUT,  0);
	GPIO_SetFunctionEach(eGPIO_F, eGPIO_14, 3);
	GPIO_SetPullUpDownEach(eGPIO_F, eGPIO_14, 0);
	return fFOUTEpll;
}

double I2S_SetEPLLClk(double EpllClk)
{	
	double fFOUTEpll, fDivdValue, fTemp;
	u8 ucSel = 0;
	u16 uPdiv, uMdiv, uSdiv, uKdiv;
	u32 uEPLLCON0, uEPLLCON1, uDivdValue;	

	fFOUTEpll = 73728000;
	while(1)
	{
		fDivdValue = fFOUTEpll  / EpllClk;
		uDivdValue = (u32) fDivdValue;
		fTemp = fDivdValue - (double) uDivdValue;
		if (fTemp < 0.0001 || fTemp > 0.9999)
		{
			UART_Printf("Epll Out is %3.3fMHz\n", fFOUTEpll/1000000);
			break;
		}
		switch(ucSel)
		{
			case 0:
				fFOUTEpll = 67738000;
				ucSel++;
				break;
			case 1:
				fFOUTEpll = 49152000;
				ucSel++;
				break;
			case 2:
				fFOUTEpll = 45158000;
				ucSel++;
				break;
			case 3:
				fFOUTEpll = 32768000;
				ucSel++;
				break;
		}
	}

	uEPLLCON0 = Inp32SYSC(0x014);
	uEPLLCON1 = Inp32SYSC(0x018);
	uDivdValue = Inp32SYSC(0x01c);
	uEPLLCON0 &=  ~(0xff << 16) & ~( 0x3f << 8 ) & ~(0x7);
	uEPLLCON1 &= ~(0xffff);	
	switch(ucSel)
	{
		case 0:				//73.728MHz

			uMdiv = 49;
			uKdiv = 9961;
			uPdiv = 1;	
			uSdiv = 3;
			
			//uMdiv = 24;
			//uKdiv = 37748;
			//uPdiv = 1;	
			//uSdiv = 2;
			break;
		case 1:				//67.738MHz
			uMdiv = 45;
			uKdiv = 10398;
			uPdiv = 1;	
			uSdiv = 3;			
			break;
		case 2:				//49.152MHz
			uMdiv = 32;
			uKdiv = 50297;
			uPdiv = 2;	
			uSdiv = 2;
			break;
		case 3:				//45.158MHz
			uMdiv = 90;
			uKdiv = 20709;
			uPdiv = 3;	
			uSdiv = 3;
			break;
		case 4:				//32.768MHz
			uMdiv = 21;
			uKdiv = 55400;
			uPdiv = 1;	
			uSdiv = 3;
			break;
	}
	uEPLLCON0 |= (uMdiv << 16) | (uPdiv << 8) | (uSdiv<<0);
	uEPLLCON1 |= (uKdiv << 0);
	uDivdValue |= 1 << 2;
	Outp32SYSC(0x014, uEPLLCON0);
	Outp32SYSC(0x018, uEPLLCON1);
	Outp32SYSC(0x01c, uDivdValue);	
	Delay(1000);	
	SYSC_CtrlCLKOUT(eCLKOUT_EPLLOUT,  0);
	GPIO_SetFunctionEach(eGPIO_F, eGPIO_14, 3);
	GPIO_SetPullUpDownEach(eGPIO_F, eGPIO_14, 0);
	return fFOUTEpll;

}

double I2S_GetEPLL(AUDIO_PORT ePort)
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

double I2S_GetMPLL(AUDIO_PORT ePort)
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
