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
*	File Name : i2smulti.c
*  
*	File Description : This file implements basic operation of IIS Multi Channel Interface
*
*	Author : Sung-Hyun,Na
*	Dept. : AP Development Team
*	Created Date : 2008/02/29
*	Version : 0.1 
* 
*	History
*	- Created(Sunghyun,Na 2008/02/29)
* 
#include "library.h"
#include "sfr6400.h"
#include "intc.h"

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
#include "audio.h"

#include "i2s.h"
#include "i2smulti.h"
#include "dma.h"

#define S3C6410

static	DMAC 			oI2SMULTIDma;
volatile 	u32				g_uI2SMULTIRecDone;
volatile 	u32				g_uI2SMULTIPlayDone;
volatile 	u32*			g_u2SMULTIPlayAddr;
extern 	I2SMULTI_Infor	g_oI2SMULTIInfor[2]; 

extern	u32				g_MPLL, g_PCLK;
extern 	I2S_TestMode 	g_TestMode;

/*---------------------------------- Init Functions  ---------------------------------*/
//////////
// Function Name : I2SMULTI_Init
// Function Description : This function initializes a certain I2SMULTI Port.
// Input : 	uPort - I2SMULTI Port Number 
// Output : 	TRUE - Memory Device is reset
//			FALSE - Memory Device is not reset because of ERROR
// Version : v0.1

u8 	I2SMULTI_Init(AUDIO_PORT ePort)
{
	I2SMULTI_Infor* p_eInfor;
	p_eInfor = I2SMULTI_GetI2SMULTIInfor(ePort);
	if(ePort == AUDIO_PORT0)
	{
		//I2SMULTI Operation
		p_eInfor->m_eOpMode 			= Master;
		p_eInfor->m_eWordLength			= Word24;
		p_eInfor->m_eIFMode 			= TXRXBoth;
		p_eInfor->m_eLRCLKPolarity 		= RightHigh;
		p_eInfor->m_eDataFormat 			= I2SFormat;
		p_eInfor->m_CLKSRC 				= I2SMUL_EPLL_OUT;
		p_eInfor->m_dSamplingFreq 		= 44100;
		p_eInfor->m_eRFS 				= RFS_768fs;
		p_eInfor->m_eBFS 				= BFS_48fs;	
		p_eInfor->m_eChannel1Discard		= NoDiscard;
		p_eInfor->m_eChannel2Discard		= NoDiscard;
		p_eInfor->m_eDataChannel			= ALLTX;
		
		// using DMAC0
		#ifdef I2SMULTI_NORMAL_DMA		
		p_eInfor->m_uNumDma			= NUM_DMA0;			
		p_eInfor->m_eDmaUnit			= DMA0;			
		#else	// I2SMULTI_SECURE_DMA
		p_eInfor->m_uNumDma			= NUM_SDMA0;		
		p_eInfor->m_eDmaUnit			= SDMA0;		
		#endif
		
		p_eInfor->m_eDmaCh				= DMA_A;
		p_eInfor->m_uI2SMULTIRxFifoAddr	= I2SMULTI_BASE + rI2SMULTIRXD;
		p_eInfor->m_uI2SMULTITxFifoAddr	= I2SMULTI_BASE + rI2SMULTITXD;
		p_eInfor->m_uPcmDataSize			= I2SMULTI_RECORD_LENGTH;
		p_eInfor->m_uPlayDataSize			= (I2SMULTI_RECORD_LENGTH * p_eInfor->m_eDataChannel);//(3072 * 1024-0x20 - (0x2000 * 12));//0x15c4800;// //I2SMULTI_RECORD_LENGTH * 9;// //
		p_eInfor->m_uRecBufferStartAddr 	= (u32 *)0x5100002c;		
		p_eInfor->m_uRecBufferStartAddr1 	= (u32 *)(0x5100002c + I2SMULTI_RECORD_LENGTH);		
		p_eInfor->m_uRecBufferStartAddr2 	= (u32 *)(0x5100002c + I2SMULTI_RECORD_LENGTH*2);		
		p_eInfor->m_uPlayBufferStartAddr	= (u32 *)0x5100002c;//(u32 *)malloc((I2SMULTI_RECORD_LENGTH * 6));		
		
	}	
	else
	{
		return FALSE;
	}	
	I2SMULTIMOD_PutData(ePort, p_eInfor);		
	return TRUE;
}

I2SMULTI_Infor* I2SMULTI_GetI2SMULTIInfor(AUDIO_PORT rhs_ePort)
{
	if (rhs_ePort == AUDIO_PORT0)
		return &(g_oI2SMULTIInfor[AUDIO_PORT0]);
	else 
		return &(g_oI2SMULTIInfor[AUDIO_PORT0]);
}



void I2SMULTI_Sel_OperationMode(AUDIO_PORT ePort, I2SMULTI_Infor * rhs_pInfor)
{
	u32 uSelection;
	UART_Printf("Select Opeation Mode\n");
	UART_Printf("1. Maseter[D]		2. Slave\n");
	uSelection = UART_GetIntNum();

	if ( uSelection == 2)
		rhs_pInfor->m_eOpMode = Slave;
	else 
		rhs_pInfor->m_eOpMode = Master;			
	
}

void I2SMULTI_Sel_WordLength(AUDIO_PORT ePort, I2SMULTI_Infor * rhs_pInfor)
{
	u32 uSelection;
	UART_Printf("Select Word Length\n");
	UART_Printf("1. 16bit[D]		2. 8bit		3. 24bit\n");
	uSelection = UART_GetIntNum();

	if (uSelection == 2)
		rhs_pInfor->m_eWordLength = Word8;
	else if (uSelection == 3) 
		rhs_pInfor->m_eWordLength = Word24;
	else
		rhs_pInfor->m_eWordLength = Word16;
}

void I2SMULTI_Sel_Interface(AUDIO_PORT ePort, I2SMULTI_Infor * rhs_pInfor)
{
	u32 uSelection;
	UART_Printf("Select Interface\n");
	UART_Printf("1. TX_RX Simultaneously[D]		2. TX Only		3. RX Only\n");
	uSelection = UART_GetIntNum();

	if (uSelection == 2)
		rhs_pInfor->m_eIFMode = TXOnly;
	else if (uSelection == 3) 
		rhs_pInfor->m_eIFMode = RXOnly;
	else
		rhs_pInfor->m_eIFMode = TXRXBoth;
}

void I2SMULTI_Sel_LRCLKPolarity(AUDIO_PORT ePort, I2SMULTI_Infor * rhs_pInfor)
{
	u32 uSelection;
	UART_Printf("Select LRCLK Polarity\n");
	UART_Printf("1. Right High[D]		2. Left High\n");
	uSelection = UART_GetIntNum();

	if (uSelection == 2)
		rhs_pInfor->m_eLRCLKPolarity = LeftHigh;
	else 
		rhs_pInfor->m_eLRCLKPolarity = RightHigh;
	
}

void I2SMULTI_Sel_SerialDataFormat(AUDIO_PORT ePort, I2SMULTI_Infor * rhs_pInfor)
{
	u32 uSelection;
	UART_Printf("Select Serial Data Format\n");
	UART_Printf("1. IIS Format[D]		2. MSB Justified	3. LSB Justified\n");
	uSelection = UART_GetIntNum();

	if (uSelection == 2)
		rhs_pInfor->m_eDataFormat = MSBJustified;
	else if (uSelection == 3) 
		rhs_pInfor->m_eDataFormat = LSBJustified;
	else
		rhs_pInfor->m_eDataFormat = I2SFormat;
}


void I2SMULTI_Sel_CLKSource(AUDIO_PORT ePort, I2SMULTI_Infor * rhs_pInfor)
{
	u32 uSelection;
	UART_Printf("Select Serial Data Format\n");
	UART_Printf("1. EPLL[D]		2. MPLL		3. Oscillator(Fin of Syscon)\n");
	UART_Printf("4. PCLK		5. External Codec CLK		6. PCM1 External CLK\n");
	uSelection = UART_GetIntNum();

	if (uSelection == 2)
		rhs_pInfor->m_CLKSRC = I2SMUL_MPLL_OUT;
	else if (uSelection == 3) 
		rhs_pInfor->m_CLKSRC = I2SMUL_SYSTEM_FIN;
	else	 if (uSelection == 4) 
		rhs_pInfor->m_CLKSRC = I2SMUL_PCLK_SRC;
	else	 if (uSelection == 5) 
		rhs_pInfor->m_CLKSRC = I2SMUL_EXTERNALCDCLK;
	else	 if (uSelection == 6) 
		rhs_pInfor->m_CLKSRC = I2SMUL_PCMEXTCLK1;	
	else	 
		rhs_pInfor->m_CLKSRC = I2SMUL_EPLL_OUT;
	
	I2SMULTI_SelCLKAUDIO(ePort, rhs_pInfor->m_CLKSRC);
}

void I2SMULTI_Sel_RFS(AUDIO_PORT ePort, I2SMULTI_Infor * rhs_pInfor)
{
	u32 uSelection;
	UART_Printf("Select LRCLK Length(RFS)\n");
	UART_Printf("1. 16bit(256fs)[D]		2. 32bit(512fs)		3. 12bit(384fs)	4. 24bit(768fs)\n");
	uSelection = UART_GetIntNum();

	if (uSelection == 2)
		rhs_pInfor->m_eRFS = RFS_512fs;
	else if (uSelection == 3) 
		rhs_pInfor->m_eRFS = RFS_384fs;
	else	 if (uSelection == 4) 
		rhs_pInfor->m_eRFS = RFS_768fs;
	else	 
		rhs_pInfor->m_eRFS = RFS_256fs;
}

void I2SMULTI_Sel_BFS(AUDIO_PORT ePort, I2SMULTI_Infor * rhs_pInfor)
{
	u32 uSelection;
	UART_Printf("Select Effective Data Length(BFS)\n");
	UART_Printf("1. 16bit(32fs)[D]		2. 24bit(48fs)		3. 8bit(16fs)		4. 12bit(24fs)\n");
	uSelection = UART_GetIntNum();

	if (uSelection == 2)
		rhs_pInfor->m_eBFS = BFS_48fs;
	else if (uSelection == 3) 
		rhs_pInfor->m_eBFS = BFS_16fs;
	else	 if (uSelection == 4) 
		rhs_pInfor->m_eBFS = BFS_24fs;
	else	 
		rhs_pInfor->m_eBFS = BFS_32fs;
}
	
void I2SMULTI_SamplingFreq(AUDIO_PORT ePort, I2SMULTI_Infor * rhs_pInfor)
{
	
	u32 uSelection;
	UART_Printf("Select Sampling Frequency[KHz]\n");
	UART_Printf("1. 8KHz		2. 11.025KHz		3. 16KHz		4. 22.05KHz\n");
	UART_Printf("5. 32KHz		6. 44.1KHz		7. 48KHz[D]		8. 64KHz \n");
	UART_Printf("9. 88.2Khz		10.96KHz			11. 192KHz\n"); 
	uSelection = UART_GetIntNum();

	if (uSelection == 1)
		rhs_pInfor->m_dSamplingFreq = 8000;
	else if (uSelection == 2) 
		rhs_pInfor->m_dSamplingFreq = 11025;
	else	 if (uSelection == 3) 
		rhs_pInfor->m_dSamplingFreq = 16000;
	else	 if (uSelection == 3) 
		rhs_pInfor->m_dSamplingFreq = 16000;
	else	 if (uSelection == 4) 
		rhs_pInfor->m_dSamplingFreq = 22050;
	else	 if (uSelection == 5) 
		rhs_pInfor->m_dSamplingFreq = 32000;
	else	 if (uSelection == 6) 
		rhs_pInfor->m_dSamplingFreq = 44100;
	else	 if (uSelection == 8) 
		rhs_pInfor->m_dSamplingFreq = 64000;
	else	 if (uSelection == 9) 
		rhs_pInfor->m_dSamplingFreq = 88200;
	else	 if (uSelection == 10) 
		rhs_pInfor->m_dSamplingFreq = 96000;
	else	 if (uSelection == 11) 
		rhs_pInfor->m_dSamplingFreq = 192000;
	else	 
		rhs_pInfor->m_dSamplingFreq = 48000;

	
}

void I2SMULTI_Sel_Discard1(AUDIO_PORT rhs_ePort, I2SMULTI_Infor * rhs_pInfor)
{
	u8 ucSel;
	UART_Printf("Select channel 1 Discard Bit Length\n");
	UART_Printf("1. No Discard[D]	2.Bit 0 ~ 15	3. Bit 16 ~ 31\n");
	ucSel = UART_GetIntNum();
	if ( ucSel == 2)
		rhs_pInfor->m_eChannel1Discard = Discard15to0;
	else if (ucSel == 3)
		rhs_pInfor->m_eChannel1Discard = Discard31to16;
	else
		rhs_pInfor->m_eChannel1Discard = NoDiscard;
	
}

void I2SMULTI_Sel_Discard2(AUDIO_PORT rhs_ePort, I2SMULTI_Infor * rhs_pInfor)
{
	u8 ucSel;
	UART_Printf("Select channel 1 Discard Bit Length\n");
	UART_Printf("1. No Discard[D]	2.Bit 0 ~ 15	3. Bit 16 ~ 31\n");
	ucSel = UART_GetIntNum();
	if ( ucSel == 2)
		rhs_pInfor->m_eChannel2Discard = Discard15to0;
	else if (ucSel == 3)
		rhs_pInfor->m_eChannel2Discard = Discard31to16;
	else
		rhs_pInfor->m_eChannel2Discard = NoDiscard;
	
}
void I2SMULTI_Sel_EnableCh(AUDIO_PORT rhs_ePort, I2SMULTI_Infor * rhs_pInfor)
{
	u8 ucSel;
	UART_Printf("Select Enable TX Channel\n");
	UART_Printf("1. TX0 only[D]	2.TX0 & TX1\n");
	UART_Printf("3. TX0 & TX2	4. All TX\n");
	ucSel = UART_GetIntNum();
	if ( ucSel == 2)
	{
		rhs_pInfor->m_eDataChannel = TX1;
		rhs_pInfor->m_uPlayDataSize = rhs_pInfor->m_uPcmDataSize * 2;		
	}
	else if (ucSel == 3)
	{
		rhs_pInfor->m_eDataChannel = TX2;
		rhs_pInfor->m_uPlayDataSize = rhs_pInfor->m_uPcmDataSize * 2;

	}
	else if (ucSel == 4)
	{
		rhs_pInfor->m_eDataChannel = ALLTX;
		rhs_pInfor->m_uPlayDataSize = rhs_pInfor->m_uPcmDataSize * 3;
	}
	else
	{
		rhs_pInfor->m_eDataChannel = TX0;
		rhs_pInfor->m_uPlayDataSize = rhs_pInfor->m_uPcmDataSize;
	}
	
}

//////////
// Function Name : I2SMULTI_InitPort
// Function Description : 
//   This function Initialize ports as I2SMULTI.
// Input : uPort - I2SMULTI port number
// Output : NONE
// Version : 
void I2SMULTI_InitPort(AUDIO_PORT ePort)
{
	switch(ePort)
	{
		case AUDIO_PORT0:
			GPIO_SetFunctionAll(eGPIO_C, 0x50550000, 0x0);
			GPIO_SetPullUpDownAll(eGPIO_C, 0x0);			// pull-up/down disable

			GPIO_SetFunctionAll(eGPIO_H, 0x55000000, 0x55);
			GPIO_SetPullUpDownAll(eGPIO_H, 0x0);			// pull-up/down disable
			break;
		default :
			GPIO_SetFunctionAll(eGPIO_C, 0x50550000, 0x0);
			GPIO_SetPullUpDownAll(eGPIO_C, 0x0);			// pull-up/down disable

			GPIO_SetFunctionAll(eGPIO_H, 0x55000000, 0x55);
			GPIO_SetPullUpDownAll(eGPIO_H, 0x0);			// pull-up/down disable
			break;
	}
}

//////////
// Function Name : I2SMULTI_ReturnPort
// Function Description : 
//   This function Initialize ports as default GPIO(input).
// Input : uPort - I2SMULTI port number
// Output : NONE
// Version : 
void I2SMULTI_ReturnPort(AUDIO_PORT ePort)
{
	switch(ePort)
	{
		case AUDIO_PORT0 :
			GPIO_SetFunctionAll(eGPIO_C, 0x0, 0x0);
			GPIO_SetFunctionAll(eGPIO_H, 0x0, 0x55);
			break;		
		default :
			GPIO_SetFunctionAll(eGPIO_C, 0x0, 0x0);
			GPIO_SetFunctionAll(eGPIO_H, 0x0, 0x55);
			break;
	}
}


void I2SMULTI_CDCLKOut(AUDIO_PORT rhs_ePort, I2SMULTI_Infor* rhs_pInfor)
{	
	u32 uPRSVal, uTemp;

	I2SMULTI_SelCLKAUDIO(rhs_ePort, rhs_pInfor->m_CLKSRC);
	uPRSVal = I2SMULTI_GetPRSVal(rhs_ePort, rhs_pInfor);
	I2SMULTIPSR_SetPrescaler(rhs_ePort, uPRSVal);	
	
	//Transfer data enable  
	//Set -Up IISCON of I2SMULTI
	uTemp = I2SMULTI_Inp32(rI2SMULTICON);
	uTemp |=I2SMULTI_Enable_I2SIF;			//Enable I2SMULTII/F
	uTemp &=  ~I2SMULTI_Enable_TX_DMA & ~I2SMULTI_Pause_TX_DMA & ~I2SMULTI_Pause_TX_IF & ~I2SMULTI_Enable_TX_DMA & ~I2SMULTI_Pause_RX_DMA & ~I2SMULTI_Pause_RX_IF & ~I2SMULTI_Enable_Underrun_Intr;		//Disable Pause RX DMA, RX I/F
	I2SMULTI_Outp32(rI2SMULTICON, uTemp);
}


void I2SMULTI_SetEpllCDCLKOut(AUDIO_PORT rhs_ePort, I2SMULTI_Infor* rhs_pInfor, u32 uSampleRate)
{
	u32 uTemp;
	double dEPLLCLK = I2SMULTI_SetEPLLClk((double) uSampleRate);
	u32 uPRSVal = I2SMULTI_GetPRSVal2(rhs_ePort, rhs_pInfor, (u32) dEPLLCLK, uSampleRate);
	I2SMULTIPSR_SetPrescaler(rhs_ePort, uPRSVal);
	I2SMULTI_SelCLKAUDIO(rhs_ePort, I2SMUL_EPLL_OUT);							//Epll Out

	//Transfer data enable  
	//Set -Up IISCON of I2S
	uTemp = I2SMULTI_Inp32(rI2SMULTICON);
	uTemp |=I2SMULTI_Enable_I2SIF;			//Enable I2SMULTII/F
	uTemp &=  ~I2SMULTI_Enable_TX_DMA & ~I2SMULTI_Pause_TX_DMA & ~I2SMULTI_Pause_TX_IF & ~I2SMULTI_Enable_TX_DMA & ~I2SMULTI_Pause_RX_DMA & ~I2SMULTI_Pause_RX_IF & ~I2SMULTI_Enable_Underrun_Intr;		//Disable Pause RX DMA, RX I/F
	I2SMULTI_Outp32(rI2SMULTICON, uTemp);
}

void	I2SMULTI_DataInDMAMode(AUDIO_PORT ePort, I2SMULTI_Infor * rhs_pInfor)
{
	u32 uTemp = 0;
	u32 uRegValue;	
	u32 ucChar;
	rhs_pInfor->m_eDreqSrc = DMA0_HSI_I2SV40_RX;			//DMA Request Source : DMA_I2SMULTI_RX DMA_I2S0_RX
	rhs_pInfor->m_eDmaId = eSEL_HSI_RX;

	#ifdef I2SMULTI_NORMAL_DMA
    	SYSC_SelectDMA(rhs_pInfor->m_eDmaId, 1);
	#else
	SYSC_SelectDMA(rhs_pInfor->m_eDmaId, 0);
	#endif
	//IRQ Setting
	INTC_SetVectAddr(rhs_pInfor->m_uNumDma ,ISR_I2SMULTI_DMARecDone);		
	INTC_Enable(rhs_pInfor->m_uNumDma);
	// DMA Setting
	// Secure DMA Set : Not Support SDMA
	DMAC_InitCh((rhs_pInfor->m_eDmaUnit) , rhs_pInfor->m_eDmaCh, &oI2SMULTIDma);
	DMACH_ClearIntPending(&oI2SMULTIDma);
	DMACH_ClearErrIntPending(&oI2SMULTIDma);
	DMACH_Setup(rhs_pInfor->m_eDmaCh, 0x0, (u32)rhs_pInfor->m_uI2SMULTIRxFifoAddr, true,
				(u32)rhs_pInfor->m_uRecBufferStartAddr, false, WORD, I2SMULTI_RECORD_LENGTH / 4, DEMAND, 			//I2SMULTI_RECORD_LENGTH
				(DREQ_SRC)(rhs_pInfor->m_eDreqSrc), MEM, SINGLE, &oI2SMULTIDma);
	DMACH_Start(&oI2SMULTIDma);
	
	//Transfer data enable  
	//Set -Up IISCON of I2SMULTI
	I2SMULTIFIC_AutoFlush(ePort, RXOnly);					//FIFO Flush	
	uTemp = I2SMULTI_Inp32(rI2SMULTICON);		
	uTemp |= I2SMULTI_Enable_I2SIF;							//Enable I2SMULTI Interface
	uTemp &= ~I2SMULTI_Pause_RX_IF & ~I2SMULTI_Enable_Underrun_Intr;
	I2SMULTI_Outp32(rI2SMULTICON, uTemp);				//RX FIFO empty check	
	do 
	{
		uRegValue = I2SMULTI_Inp32(rI2SMULTICON);
	} while ( uRegValue & (I2SMULTI_Status_RX_FIFO_EMPTY));	
	
	uTemp |= I2SMULTI_Enable_RX_DMA;						//Enable RX DMA
	uTemp &= ~I2SMULTI_Pause_RX_DMA;						//Disable Pause RX DMA
	I2SMULTI_Outp32(rI2SMULTICON, uTemp);

	#if 1
	UART_Printf("DMA Pause : P, DMA Play O\n");
	UART_Printf("RX Interface Pause : L, Interface Play : K\n");
	UART_Printf("I2SMULTI Interface Pause : I, Play : U\n");		
	while(g_uI2SMULTIRecDone ==0)    	
	{
        	
		ucChar = UART_Getc();
		 if ((ucChar == 'p') | (ucChar == 'P'))
		{
			I2SMULTICON_PauseDMA(ePort, RXOnly, PAUSE);
			UART_Printf("DMA PAUSE!!\n");				
		}
		else if ((ucChar == 'o') | (ucChar == 'O'))
		{
			I2SMULTICON_PauseDMA(ePort, RXOnly, PLAY);
			UART_Printf("DMA PLAY!!\n");
		}
		else if ((ucChar == 'l') | (ucChar == 'L'))
		{
			I2SMULTICON_PauseIF(ePort, RXOnly, PAUSE);
			UART_Printf("RX Interface PAUSE!!\n");
		}
		else if ((ucChar == 'k') | (ucChar == 'K'))
		{
			I2SMULTICON_PauseIF(ePort, RXOnly, PLAY);
			UART_Printf("RX Interface PLAY!!\n");
		}
		else if ((ucChar == 'i') | (ucChar == 'I'))
		{
			I2SMULTICON_ActiveIF(ePort,  false);
			UART_Printf("Interface Off\n");
		}
		else if ((ucChar == 'u') | (ucChar == 'U'))
		{
			I2SMULTICON_ActiveIF(ePort,  true);
			UART_Printf("Interface On\n");
		}			
    	}
	#else
	while(g_uI2SMULTIRecDone ==0)    	
	{
		UART_Printf(".");
		Delay(3000);
	}
	
	#endif
	 g_uI2SMULTIRecDone = 0; 	


	// DMA stop
    	DMACH_Stop(&oI2SMULTIDma);	
	//Disble Interrupt 
    	INTC_Disable(rhs_pInfor->m_uNumDma);
	//Disable RXDMA and Pause RX Interface
	uTemp = I2SMULTI_Inp32(rI2SMULTICON);
	uTemp &= ~ I2SMULTI_Enable_RX_DMA;
	uTemp |=  I2SMULTI_Pause_RX_IF;
	I2SMULTI_Outp32(rI2SMULTICON, uTemp);
	UART_Printf("Rec Done!!\n");	

	I2SMULTI_MakeChannel(rhs_pInfor);
	//I2SMULTI_MergeChannel(rhs_pInfor);
	//I2SMULTI_MergeSerial(rhs_pInfor);
	//I2SMULTI_MergeEachCh(rhs_pInfor);	
	//I2SMULTI_MergeEachCh2(rhs_pInfor);
	//I2SMULTI_MergeEachCh3(rhs_pInfor);
}

void	I2SMULTI_DataInDMAStop(AUDIO_PORT ePort, I2SMULTI_Infor * rhs_pInfor)
{
	u32 uTemp;
	// DMA stop
    	DMACH_Stop(&oI2SMULTIDma);	
	//Disble Interrupt 
    	INTC_Disable(rhs_pInfor->m_uNumDma);
	//Disable RXDMA and Pause RX Interface
	uTemp = I2SMULTI_Inp32(rI2SMULTICON);
	uTemp &= ~ I2SMULTI_Enable_RX_DMA;
	uTemp |=  I2SMULTI_Pause_RX_IF;
	I2SMULTI_Outp32(rI2SMULTICON, uTemp);

	I2SMULTIFIC_AutoFlush(ePort, TXRXBoth);					//FIFO Flush	
}


void I2SMULTI_DataOutDMA(AUDIO_PORT ePort, I2SMULTI_Infor * rhs_pInfor)
{
	u32 uTemp = 0;
	u32 uRegValue;
	u8 ucChar=0;
	
	//IRQ Setting
	rhs_pInfor->m_eDmaId	= eSEL_HSI_TX;
	rhs_pInfor->m_eDreqSrc	= DMA0_HSI_I2SV40_TX;
	if ( g_TestMode == I2S_SINGLE)	
	{
		INTC_SetVectAddr(rhs_pInfor->m_uNumDma ,ISR_I2SMULTI_DMAPlayDone);
		INTC_Enable(rhs_pInfor->m_uNumDma);	
		INTC_SetVectAddr(NUM_I2S, ISR_I2SMULTI_UnderRunAC);
		INTC_Enable(NUM_I2S);
	}
	else
	{
		INTC_SetVectAddr(rhs_pInfor->m_uNumDma ,ISR_I2SMULTI_DMAPlayDone);
		INTC_Enable(rhs_pInfor->m_uNumDma);	
	}
	uTemp = I2SMULTI_Inp32(rI2SMULTICON);
	uTemp &= ~ I2SMULTI_Enable_TX_DMA;
	uTemp |=  I2SMULTI_Pause_TX_IF;
	I2SMULTI_Outp32(rI2SMULTICON, uTemp);

	//DMA Setting
	#ifdef	I2SMULTI_NORMAL_DMA
	SYSC_SelectDMA(rhs_pInfor->m_eDmaId, 1);
	#else
	SYSC_SelectDMA(rhs_pInfor->m_eDmaId, 0);
	#endif

	DMAC_InitCh((rhs_pInfor->m_eDmaUnit) , rhs_pInfor->m_eDmaCh, &oI2SMULTIDma);
	DMACH_ClearIntPending(&oI2SMULTIDma);
	DMACH_ClearErrIntPending(&oI2SMULTIDma);	
	DMACH_Setup(rhs_pInfor->m_eDmaCh, false, (u32)rhs_pInfor->m_uPlayBufferStartAddr, false,
			(u32)rhs_pInfor->m_uI2SMULTITxFifoAddr, true, WORD, ((u32) rhs_pInfor->m_uPlayDataSize) / 4, DEMAND, 
			MEM, (DREQ_SRC)(rhs_pInfor->m_eDreqSrc), SINGLE, &oI2SMULTIDma);
	DMACH_Start(&oI2SMULTIDma);
	
	I2SMULTIFIC_AutoFlush(ePort, TXOnly);					//FIFO Flush	
	
	uTemp |= I2SMULTI_Enable_TX_DMA | I2SMULTI_Enable_Underrun_Intr;	
	uTemp &= ~I2SMULTI_Pause_TX_DMA; 									// Disable Underrun Interrupt, Pause RX DMA										
	I2SMULTI_Outp32( rI2SMULTICON, uTemp);	
	do 
	{
		uRegValue = I2SMULTI_Inp32(rI2SMULTICON);
	} while ((uRegValue & (I2SMULTI_Status_TX0_FIFO_EMPTY) & (I2SMULTI_Status_TX1_FIFO_EMPTY) & (I2SMULTI_Status_TX2_FIFO_EMPTY)) != 0);			//TX FIFO Full check	
	//Transfer data enable  
	//Set -Up IISCON of I2SMULTI
	uTemp &= ~I2SMULTI_Pause_TX_IF &~I2SMULTI_Pause_RX_IF; 			// Disable Underrun Interrupt, Pause RX DMA
	uTemp |= I2SMULTI_Enable_I2SIF;									// Enable RX DMA,IIS I/F
	I2SMULTI_Outp32(rI2SMULTICON, uTemp);
	
	if (g_TestMode == I2S_SINGLE)	
	{
		UART_Printf("\nIf you want to exit, Press the 'x' key.\n");
		UART_Printf("TX DMA Pause : P, TX DMA Play O\n");
		UART_Printf("TX Interface Pause : L, TX Interface Play : K\n");
		UART_Printf("I2SMULTI Port Interface Pause : I, I2SMULTI Port Interface Pause : U\n");
		while(1)
		{		
			ucChar = UART_Getc();
			//memcpy((void *) 0x55000000, 0x0, 0x10000);
			if((ucChar == 'x') | (ucChar == 'X')) 
				break;
			else if ((ucChar == 'p') | (ucChar == 'P'))
			{
				I2SMULTICON_PauseDMA(ePort, TXOnly, PAUSE);
				UART_Printf("TX DMA PAUSE!!\n");				
			}
			else if ((ucChar == 'o') | (ucChar == 'O'))
			{
				I2SMULTICON_PauseDMA(ePort, TXOnly, PLAY);
				UART_Printf("TX DMA PLAY!!\n");
			}
			else if ((ucChar == 'l') | (ucChar == 'L'))
			{
				I2SMULTICON_PauseIF(ePort, TXOnly, PAUSE);
				UART_Printf("TX Interface PAUSE!!\n");
			}
			else if ((ucChar == 'k') | (ucChar == 'K'))
			{
				I2SMULTICON_PauseIF(ePort, TXOnly, PLAY);
				UART_Printf("TX Interface PLAY!!\n");
			}
			else if ((ucChar == 'i') | (ucChar == 'I'))
			{
				I2SMULTICON_ActiveIF(ePort,  false);
				UART_Printf("Interface Off\n");
			}
			else if ((ucChar == 'u') | (ucChar == 'U'))
			{
				I2SMULTICON_ActiveIF(ePort,  true);
				UART_Printf("Interface On\n");
			}
		} 
		
		uTemp &= ~I2SMULTI_Enable_Underrun_Intr; 									// Disable Underrun Interrupt, Pause RX DMA										
		I2SMULTI_Outp32(rI2SMULTICON, uTemp);	
	}
}

void	I2SMULTI_DataOutDMAStop(AUDIO_PORT ePort, I2SMULTI_Infor * rhs_pInfor)
{
	u32 uTemp;
	
    	DMACH_Stop(&oI2SMULTIDma);			// DMA stop
	
    	INTC_Disable(rhs_pInfor->m_uNumDma);		//Disble Interrupt 
	
	uTemp = I2SMULTI_Inp32(rI2SMULTICON);	//Disable IISCON
	uTemp &= ~ I2SMULTI_Enable_TX_DMA;
	uTemp |=  I2SMULTI_Pause_TX_IF;
	I2SMULTI_Outp32(rI2SMULTICON, uTemp);
	I2SMULTIFIC_AutoFlush(ePort, TXRXBoth);	//FIFO Flush	
}

void	I2SMULTI_DataBypassDMA(AUDIO_PORT ePort, I2SMULTI_Infor * rhs_pInfor)
{
	u32 uTemp = 0;
	
	rhs_pInfor->m_eDreqSrc		= DMA0_HSI_I2SV40_TX;
	rhs_pInfor->m_eDreqDst		= DMA0_HSI_I2SV40_RX;
	//IRQ Setting
	INTC_SetVectAddr(rhs_pInfor->m_uNumDma ,ISR_I2SMULTI_DMABypass);
	
	
	INTC_Enable(rhs_pInfor->m_uNumDma );	
	uTemp = I2SMULTI_Inp32(rI2SMULTICON);
	uTemp &= ~ I2SMULTI_Enable_TX_DMA;
	uTemp |=  I2SMULTI_Pause_TX_IF | I2SMULTI_Pause_RX_IF;
	I2SMULTI_Outp32(rI2SMULTICON, uTemp);

	//DMA Setting
	
	DMAC_InitCh((rhs_pInfor->m_eDmaUnit) , rhs_pInfor->m_eDmaCh, &oI2SMULTIDma);
	DMACH_ClearIntPending(&oI2SMULTIDma);
	DMACH_ClearErrIntPending(&oI2SMULTIDma);
	
	//DMACH_ClearErrIntPending(&oI2SMULTIDma);	
	
	DMACH_Setup(rhs_pInfor->m_eDmaCh, 0x0, (u32)rhs_pInfor->m_uI2SMULTIRxFifoAddr, true,
				(u32)rhs_pInfor->m_uI2SMULTITxFifoAddr, true, WORD, I2SMULTI_RECORD_LENGTH/4, DEMAND, 
				(DREQ_SRC)(rhs_pInfor->m_eDreqSrc), (DREQ_SRC)(rhs_pInfor->m_eDreqDst), SINGLE, &oI2SMULTIDma);
	DMACH_Start(&oI2SMULTIDma);
	
	I2SMULTIFIC_AutoFlush(ePort, TXRXBoth);							//FIFO Flush		
	
	uTemp |= I2SMULTI_Enable_TX_DMA | I2SMULTI_Enable_RX_DMA;	
	uTemp &= ~I2SMULTI_Pause_TX_DMA & ~I2SMULTI_Pause_RX_DMA; 		// Disable Underrun Interrupt, Pause RX DMA										
	I2SMULTI_Outp32( rI2SMULTICON, uTemp);	
	
	//Transfer data enable  
	//Set -Up IISCON of I2SMULTI
	uTemp &= ~I2SMULTI_Pause_TX_IF & ~I2SMULTI_Pause_RX_IF ; 			// Disable Underrun Interrupt, Pause RX DMA
	uTemp |= I2SMULTI_Enable_I2SIF;										// Enable RX DMA,IIS I/F
	I2SMULTI_Outp32( rI2SMULTICON, uTemp);
	UART_Printf("Bypass Start\n");
		
}


void I2SMULTI_DataInPolling(AUDIO_PORT rhs_ePort, I2SMULTI_Infor * rhs_pInfor)
{
	s32 uPcmDataSize;
	u32 uRegValue = 0;
	u32* uRecBuf= rhs_pInfor->m_uRecBufferStartAddr;
	u32 uFIFOTriggerLevel = 0x8;					//for Debug	
	u32 uLoop, uLoopCnt;

	//Assert(uPcmDataSize%4 == 0 && uPcmDataSize >= 4);
	
	UART_Printf("Now Record...\n");
	switch(rhs_pInfor->m_eDataChannel)
	{
		case TX1:
		case TX2:
			uLoop = 2;
			break;
		case ALLTX:
			uLoop = 3;
			break;
		case TX0:
		default:
			uLoop = 1;
			break;
	}
	for (uLoopCnt = 0; uLoopCnt < uLoop; uLoopCnt++)
	{
		uPcmDataSize = rhs_pInfor->m_uPcmDataSize;
		if (uLoopCnt == 0)
		{
			uRecBuf= rhs_pInfor->m_uRecBufferStartAddr;
			UART_Printf("Front Left/Right Rec!!\n");				
		}
		else if (uLoopCnt == 1)
		{
			uRecBuf= rhs_pInfor->m_uRecBufferStartAddr1;
			UART_Printf("Rear Left/Right Rec!!\n");
		}
		else if (uLoopCnt == 2)
		{
			uRecBuf= rhs_pInfor->m_uRecBufferStartAddr2;
			UART_Printf("Center & Woofer Rec!!\n");		
		}
		
		
		I2SMULTIFIC_AutoFlush(rhs_ePort,  RXOnly);		//FIFO Flush
		//I2SMULTICON_SetIFMode(rhs_ePort, RXOnly);
		I2SMULTICON_ActiveIF(rhs_ePort, ACTIVE);		//IIS IF ACTIVE
		I2SMULTICON_PauseIF(rhs_ePort, RXOnly,  PLAY);	//I2SMULTI RX IF no Pause
		
		
		while( uPcmDataSize > 0 )
		{
			do 
			{
				uRegValue = I2SMULTI_Inp32(rI2SMULTIFIC);
			} while ((uRegValue & (I2SMULTI_Status_RX_FIFO_CNT)) < uFIFOTriggerLevel);		// Rx FIFO Level check
			
			while(1)
			{
				*uRecBuf++ = I2SMULTI_Inp32(rI2SMULTIRXD);			//Data Transfer
				uPcmDataSize -= 4;
				uRegValue = I2SMULTI_Inp32(rI2SMULTICON);
				if (uRegValue & (I2SMULTI_Status_RX_FIFO_EMPTY) )
					break;
			}
		}	
		
		UART_Printf("Data Receive Done\n");	
		I2SMULTICON_PauseIF(rhs_ePort, RXOnly, PAUSE);				//RX IF Pause
		I2SMULTICON_ActiveIF(rhs_ePort, INACTIVE);					//IF Inactive	
		I2SMULTIFIC_AutoFlush(rhs_ePort, RXOnly);					//FIFO FLUSH
	}
	
	if (rhs_pInfor->m_eWordLength == Word24)
	{
		switch(rhs_pInfor->m_eDataChannel)
		{
			case TX1:
			case TX2:
				Parser_32to24(rhs_pInfor->m_uRecBufferStartAddr, I2SMULTI_RECORD_LENGTH, (u32 *)0x53400000);
				Parser_32to24(rhs_pInfor->m_uRecBufferStartAddr1, I2SMULTI_RECORD_LENGTH, (u32 *)0x53600000);
				break;
			case ALLTX:
				Parser_32to24(rhs_pInfor->m_uRecBufferStartAddr, I2SMULTI_RECORD_LENGTH, (u32 *)0x53400000);
				Parser_32to24(rhs_pInfor->m_uRecBufferStartAddr1, I2SMULTI_RECORD_LENGTH, (u32 *)0x53600000);
				Parser_32to24(rhs_pInfor->m_uRecBufferStartAddr2, I2SMULTI_RECORD_LENGTH, (u32 *)0x53800000);
				break;
			case TX0:
			default:
				Parser_32to24(rhs_pInfor->m_uRecBufferStartAddr, I2SMULTI_RECORD_LENGTH, (u32 *)0x53400000);
				break;
		}
		
		
	}
	//I2SMULTI_MergeChannel(rhs_pInfor);
	I2SMULTI_MakeChannel(rhs_pInfor);
	
}



void I2SMULTI_DataOutPolling(AUDIO_PORT rhs_ePort, I2SMULTI_Infor * rhs_pInfor)
{
	u32 *uRecBuf = rhs_pInfor->m_uPlayBufferStartAddr;
	s32 uPlayDataSize = rhs_pInfor->m_uPlayDataSize;
	u32 uRegValue = 0;
	u32 uFIFOTriggerLevel = 0x8;
	
	UART_Printf("Now Play...\n");	
	I2SMULTIFIC_AutoFlush(rhs_ePort,  TXOnly);		//FIFO Flush	
	
	
	switch(rhs_pInfor->m_eDataChannel)
	{
		case TX1:
			while(1)
			{
				uRegValue = *((uRecBuf)++);
				I2SMULTI_Outp32(rI2SMULTITXD, uRegValue);	//Data Transfer		
				uPlayDataSize -= 4;
				uRegValue = I2SMULTI_Inp32(rI2SMULTICON);
				if (uRegValue & (I2SMULTI_Status_TX0_FIFO_FULL | I2SMULTI_Status_TX1_FIFO_FULL) )
					break;
			}					
			break;
		case TX2:
			while(1)
			{
				uRegValue = *((uRecBuf)++);
				I2SMULTI_Outp32(rI2SMULTITXD, uRegValue);	//Data Transfer		
				uPlayDataSize -= 4;
				uRegValue = I2SMULTI_Inp32(rI2SMULTICON);
				if (uRegValue & (I2SMULTI_Status_TX0_FIFO_FULL | I2SMULTI_Status_TX2_FIFO_FULL) )
					break;
			}					
			break;
		case ALLTX:
			while(1)
			{
				uRegValue = *((uRecBuf)++);
				I2SMULTI_Outp32(rI2SMULTITXD, uRegValue);	//Data Transfer		
				uPlayDataSize -= 4;
				uRegValue = I2SMULTI_Inp32(rI2SMULTICON);
				if (uRegValue & (I2SMULTI_Status_TX0_FIFO_FULL | I2SMULTI_Status_TX1_FIFO_FULL | I2SMULTI_Status_TX2_FIFO_FULL) )
					break;
			}				
			break;
		case TX0:
		default:
			while(1)
			{
				uRegValue = *((uRecBuf)++);
				I2SMULTI_Outp32(rI2SMULTITXD, uRegValue);	//Data Transfer		
				uPlayDataSize -= 4;
				uRegValue = I2SMULTI_Inp32(rI2SMULTICON);
				if (uRegValue & (I2SMULTI_Status_TX0_FIFO_FULL) )
					break;
			}
			break;
			
	}		
	
	//INTC_SetVectAddr(NUM_I2S ,ISR_I2SMULTI_Underrun);	
	//INTC_Enable(NUM_I2S);
	//I2SMULTICON_EnableIntr(rhs_ePort, ACTIVE);		//Enable TX FIFO Under Run Interrupt 
	UART_Printf("FIFO CNT is 0x%x\n",(I2SMULTI_Inp32(rI2SMULTIFIC)&I2SMULTI_Status_TX0_FIFO_CNT) >> 8);
	
	I2SMULTICON_ActiveIF(rhs_ePort, ACTIVE);		//IIS IF ACTIVE
	I2SMULTICON_PauseIF(rhs_ePort, TXOnly,  PLAY);	//I2SMULTI RX IF no Pause

	
	
	#if 0
	while(uPcmDataSize > 0)
	{
		do 
		{
			uRegValue = I2SMULTI_Inp32( rI2SMULTICON);
		} while ( uRegValue&(I2SMULTI_Status_TX_FIFO_FULL));		// TX FIFO empty check
		uRegValue = *uRecBuf++;
		 I2SMULTI_Outp32( rI2SMULTITXD, uRegValue);		//Data Transfer		
		uPcmDataSize -= 4;
	}
	#else
	switch(rhs_pInfor->m_eDataChannel)
	{
		case TX1:
			while(uPlayDataSize > 0)
			{
				while(1)
				{
					uRegValue = *((uRecBuf)++);
					I2SMULTI_Outp32(rI2SMULTITXD, uRegValue);							//Data Transfer		
					uPlayDataSize -= 4;
					//uRegValue = I2SMULTI_Inp32(rI2SMULTICON);
					if (I2SMULTI_Inp32(rI2SMULTICON) & (I2SMULTI_Status_TX0_FIFO_FULL | I2SMULTI_Status_TX1_FIFO_FULL) )
						break;
				}		
				do 
				{
					uRegValue = I2SMULTI_Inp32(rI2SMULTIFIC);
					//uRegValue1 = I2SMULTI_Inp32(rI2SMULTICON);
					//if ( uRegValue1 & I2SMULTI_Clear_Underrun_Intr)
					//	UART_Printf("Under Run!!\n");
				} while ( (((uRegValue & (I2SMULTI_Status_TX0_FIFO_CNT)) >> 8) > uFIFOTriggerLevel ) && (((uRegValue & (I2SMULTI_Status_TX1_FIFO_CNT)) >> 16) > uFIFOTriggerLevel ) );		// TX0 FIFO Level check				
			}
			break;
		case TX2:
			while(uPlayDataSize > 0)
			{
				while(1)
				{
					uRegValue = *((uRecBuf)++);
					I2SMULTI_Outp32(rI2SMULTITXD, uRegValue);							//Data Transfer		
					uPlayDataSize -= 4;
					//uRegValue = I2SMULTI_Inp32(rI2SMULTICON);
					if (I2SMULTI_Inp32(rI2SMULTICON) & (I2SMULTI_Status_TX0_FIFO_FULL | I2SMULTI_Status_TX2_FIFO_FULL) )
						break;
				}		
				do 
				{
					uRegValue = I2SMULTI_Inp32(rI2SMULTIFIC);
					//uRegValue1 = I2SMULTI_Inp32(rI2SMULTICON);
					//if ( uRegValue1 & I2SMULTI_Clear_Underrun_Intr)
					//	UART_Printf("Under Run!!\n");
				} while ( (((uRegValue & (I2SMULTI_Status_TX0_FIFO_CNT)) >> 8) > uFIFOTriggerLevel ) && (((uRegValue & (I2SMULTI_Status_TX2_FIFO_CNT)) >> 24) > uFIFOTriggerLevel ) );		// TX0 FIFO Level check				
			}
			break;		
		case ALLTX:
			while(uPlayDataSize > 0)
			{
				while(1)
				{
					uRegValue = *((uRecBuf)++);
					I2SMULTI_Outp32(rI2SMULTITXD, uRegValue);							//Data Transfer		
					uPlayDataSize -= 4;
					//uRegValue = I2SMULTI_Inp32(rI2SMULTICON);
					if (I2SMULTI_Inp32(rI2SMULTICON) & (I2SMULTI_Status_TX0_FIFO_FULL | I2SMULTI_Status_TX1_FIFO_FULL | I2SMULTI_Status_TX2_FIFO_FULL) )
						break;
				}		
				do 
				{
					uRegValue = I2SMULTI_Inp32(rI2SMULTIFIC);
					//uRegValue1 = I2SMULTI_Inp32(rI2SMULTICON);
					//if ( uRegValue1 & I2SMULTI_Clear_Underrun_Intr)
					//	UART_Printf("Under Run!!\n");
				} while ( (((uRegValue & (I2SMULTI_Status_TX0_FIFO_CNT)) >> 8) > uFIFOTriggerLevel ) && (((uRegValue & (I2SMULTI_Status_TX1_FIFO_CNT)) >> 16) > uFIFOTriggerLevel ) && (((uRegValue & (I2SMULTI_Status_TX2_FIFO_CNT)) >> 24) > uFIFOTriggerLevel ) );		// TX0 FIFO Level check				
			}
			break;
		case TX0:
		default:
			while(uPlayDataSize > 0)
			{
				while(1)
				{
					uRegValue = *((uRecBuf)++);
					I2SMULTI_Outp32(rI2SMULTITXD, uRegValue);							//Data Transfer		
					uPlayDataSize -= 4;
					//uRegValue = I2SMULTI_Inp32(rI2SMULTICON);
					if (I2SMULTI_Inp32(rI2SMULTICON) & (I2SMULTI_Status_TX0_FIFO_FULL) )
						break;
				}		
				do 
				{
					//uRegValue = I2SMULTI_Inp32(rI2SMULTIFIC);
					//uRegValue1 = I2SMULTI_Inp32(rI2SMULTICON);
					//if ( uRegValue1 & I2SMULTI_Clear_Underrun_Intr)
					//	UART_Printf("Under Run!!\n");
				} while (((I2SMULTI_Inp32(rI2SMULTIFIC) & (I2SMULTI_Status_TX0_FIFO_CNT)) >> 8) > uFIFOTriggerLevel);		// TX0 FIFO Level check				
			}
			break;
	}
	#endif
	I2SMULTICON_PauseIF(rhs_ePort, TXOnly, PAUSE);				//RX IF Pause
	I2SMULTICON_ActiveIF(rhs_ePort, INACTIVE);					//IF Inactive	

	UART_Printf("Play Done~\n");	
	
	//INTC_Disable(INT_IIS);
	//I2SMULTICON_EnableIntr(rhs_ePort, INACTIVE);
	//rhs_pInfor->m_uRecBufferStartAddr = uRecBuf;
	
	I2SMULTIFIC_AutoFlush(rhs_ePort, TXOnly);					//FIFO FLUSH
	
}	

void I2SMULTI_DataBypassPolling(AUDIO_PORT rhs_ePort, I2SMULTI_Infor * rhs_pInfor)
{
	//s32 uPcmDataSize = 0xf0000;
	u32 uRegValue = 0;
	u32 uFIFOTriggerLevel = 0x8;
	u32 uPollingDelay = 0x1000000;

	
	

	//Assert(uPcmDataSize%4 == 0 && uPcmDataSize >= 4);
	
	UART_Printf("Now Record...\n");	
	I2SMULTIFIC_AutoFlush(rhs_ePort,  TXRXBoth);		//FIFO Flush	
	I2SMULTICON_ActiveIF(rhs_ePort, ACTIVE);			//IIS IF ACTIVE
	I2SMULTICON_PauseIF(rhs_ePort, TXRXBoth,  PLAY);	//I2SMULTI RX IF no Pause
	#if 0
	while( uPcmDataSize > 0 )
	{
		do 
		{
			uRegValue = I2SMULTI_Inp32( rI2SMULTICON);
		} while (uRegValue&(I2SMULTI_Status_RX_FIFO_EMPTY));		// Rx FIFO empty check
		UART_Printf("%d\t",(uRegValue & (I2SMULTI_Status_RX_FIFO_CNT)));
		uRegValue	= I2SMULTI_Inp32( rI2SMULTIRXD);		//Data Transfer		
		*uRecBuf++ = uRegValue;
		uPcmDataSize -= 4;		
	}
	#else
	do
	{
		do 
		{
			uRegValue = I2SMULTI_Inp32( rI2SMULTIFIC);
		} while ((uRegValue & (I2SMULTI_Status_RX_FIFO_CNT)) < uFIFOTriggerLevel);		// Rx FIFO Level check
		
		while(1)
		{
			uRegValue	= I2SMULTI_Inp32( rI2SMULTIRXD);						//Data Transfer		
			I2SMULTI_Outp32( rI2SMULTITXD,uRegValue);
			uRegValue = I2SMULTI_Inp32( rI2SMULTICON);
			if (uRegValue & (I2SMULTI_Status_RX_FIFO_EMPTY) )
				break;
		}
		uPollingDelay--;
	}while( uPollingDelay != 0 );
	#endif
	UART_Printf("Data Receive Done\n");	
	I2SMULTICON_PauseIF(rhs_ePort, RXOnly, PAUSE);				//RX IF Pause
	I2SMULTICON_ActiveIF(rhs_ePort, INACTIVE);					//IF Inactive	
	I2SMULTIFIC_AutoFlush(rhs_ePort, RXOnly);					//FIFO FLUSH
}	

void I2SMULTI_DataOutIntr(AUDIO_PORT rhs_ePort, I2SMULTI_Infor * rhs_pInfor)
{
	u32 uTemp;

	INTC_ClearVectAddr();
	I2SMUlTICON_ClrIntr();
	
	//Set Interrupt Controller 	
	INTC_SetVectAddr(NUM_I2S ,ISR_I2SMULTI_Underrun);		
	INTC_Enable(NUM_I2S);
	
	g_uI2SMULTIPlayDone = 0;	
	g_u2SMULTIPlayAddr  = rhs_pInfor->m_uPlayBufferStartAddr;
	UART_Printf("Play!\n");

	I2SMULTIFIC_AutoFlush(rhs_ePort, TXOnly);					//FIFO Flush	
	uTemp = I2SMULTI_Inp32(rI2SMULTICON);		
	uTemp |= I2SMULTI_Enable_I2SIF;				
	uTemp &= ~I2SMULTI_Enable_TX_DMA & ~I2SMULTI_Pause_TX_IF;
	I2SMULTI_Outp32(rI2SMULTICON, uTemp);
	
	I2SMULTICON_EnableIntr(rhs_ePort, ACTIVE);			//Enable I2S Interface & RX Overrun Intr' 

	for (uTemp = 0; uTemp < 8; uTemp++)
		I2SMULTI_Outp32(rI2SMULTITXD, *(g_u2SMULTIPlayAddr));  
	
	while(1)
	{
		if ( g_uI2SMULTIPlayDone == 1)
			break;
	}

	INTC_Disable(NUM_I2S);
	uTemp = I2SMULTI_Inp32(rI2SMULTICON);
	uTemp |= I2SMULTI_Pause_TX_IF;
	uTemp &= ~I2SMULTI_Enable_Underrun_Intr & ~I2SMULTI_Enable_I2SIF;
	I2SMULTI_Outp32(rI2SMULTICON, uTemp);

}

/*---------------------------------- ISR Routines ---------------------------------*/
//////////
// Function Name : ISR_DMA1INTRecDone
// Function Description : This function implements ISR of DMA Record Interrupt.
// Input : 	NONE
// Output : 	NONE
// Version : v0.1
void __irq ISR_I2SMULTI_DMARecDone(void)
{
	
	INTC_Disable(g_oI2SMULTIInfor[AUDIO_PORT0].m_uNumDma);

	// Interrupt Clear
	DMACH_ClearIntPending(&oI2SMULTIDma);
	DMACH_ClearErrIntPending(&oI2SMULTIDma);
	
	INTC_ClearVectAddr();
	g_uI2SMULTIRecDone = 1;

	I2SMULTICON_ActiveDMA(AUDIO_PORT0, RXOnly, INACTIVE);
	I2SMULTICON_ActiveIF(AUDIO_PORT0, INACTIVE);
	//INTC_Enable(g_oI2SMULTIInfor[AUDIO_PORT0].m_uNumDma);	
	
	
	UART_Printf("\nRec Done\n");
#if 0

#endif
	
}


void __irq ISR_I2SMULTI_Underrun(void)
{
	INTC_Disable(NUM_I2S);	
	UART_Printf(".");

	do{
		I2SMULTI_Outp32(rI2SMULTITXD,*(g_u2SMULTIPlayAddr++));
	}while(!(I2SMULTI_Inp32(rI2SMULTICON) & I2SMULTI_Status_TX0_FIFO_FULL));

	I2SMUlTICON_ClrIntr();
	INTC_ClearVectAddr();	

	if ( (u32) g_u2SMULTIPlayAddr >= ((u32) g_oI2SMULTIInfor[AUDIO_PORT0].m_uPlayBufferStartAddr + (u32) g_oI2SMULTIInfor[AUDIO_PORT0].m_uPlayDataSize))
		g_uI2SMULTIPlayDone = 1;
	else
		INTC_Enable(NUM_I2S);	
}

//////////
// Function Name : ISR_DMA1INTPlayDone
// Function Description : This function implements ISR of DMA Play Interrupt.
// Input : 	NONE
// Output : 	NONE
// Version : v0.1

void __irq ISR_I2SMULTI_DMALoopBackDone(void)
{
	I2SMULTI_Infor * pInfor = I2SMULTI_GetI2SMULTIInfor(AUDIO_PORT0);
		
	INTC_Disable(pInfor->m_uNumDma);
	I2SMULTICON_ActiveIF(AUDIO_PORT0, INACTIVE);
	// Interrupt Clear
	DMACH_ClearIntPending(&oI2SMULTIDma);
	DMACH_ClearErrIntPending(&oI2SMULTIDma);
	
	I2SMULTIFIC_AutoFlush(AUDIO_PORT0, TXOnly);	
	DMACH_Start(&oI2SMULTIDma);
	
	
	UART_Printf("TX DMA Done ~~~\n");	
	INTC_ClearVectAddr();
}

void __irq ISR_I2SMULTI_DMAPlayDone(void)
{
	I2SMULTI_Infor * pInfor = I2SMULTI_GetI2SMULTIInfor(AUDIO_PORT0);
		
	INTC_Disable(pInfor->m_uNumDma);
	I2SMULTICON_ActiveIF(AUDIO_PORT0, INACTIVE);
	// Interrupt Clear
	DMACH_ClearIntPending(&oI2SMULTIDma);
	DMACH_ClearErrIntPending(&oI2SMULTIDma);
	#if 0
	DMAC_Start((u32)pInfor->m_uPlayBufferStartAddr, 0, (u32)pInfor->m_uI2SMULTITxFifoAddr, 1,  
	 			WORD, pInfor->m_uPcmDataSize/4, DEMAND, MEM, 0xe,  SINGLE, &oI2SMULTIDma);
	#else
	//DMACH_Setup(pInfor->m_eDmaCh, false, (u32)pInfor->m_uPlayBufferStartAddr, false,
	//		(u32)pInfor->m_uI2SMULTITxFifoAddr, true, WORD, I2SMULTI_RECORD_LENGTH/4, DEMAND, 
	//		MEM, (DREQ_SRC)(pInfor->m_eDreqSrc), SINGLE, &oI2SMULTIDma);	
	
	DMACH_Setup(pInfor->m_eDmaCh, false, (u32)pInfor->m_uPlayBufferStartAddr, false,
			(u32)pInfor->m_uI2SMULTITxFifoAddr, true, WORD, ((u32) pInfor->m_uPlayDataSize) / 4, DEMAND, 
			MEM, (DREQ_SRC)(pInfor->m_eDreqSrc), SINGLE, &oI2SMULTIDma);

	I2SMULTIFIC_AutoFlush(AUDIO_PORT0, TXOnly);	
	DMACH_Start(&oI2SMULTIDma);
	
	
	#endif
	UART_Printf("TX DMA Done ~~~\n");
	DMACH_Start(&oI2SMULTIDma);	
	I2SMULTICON_ActiveIF(AUDIO_PORT0, ACTIVE);
	INTC_Enable(pInfor->m_uNumDma);
	INTC_ClearVectAddr();
}

void __irq ISR_I2SMULTI_DMABypass(void)
{
	INTC_Disable(g_oI2SMULTIInfor[AUDIO_PORT0].m_uNumDma);

	// Interrupt Clear
	DMACH_ClearIntPending(&oI2SMULTIDma);
	
	//DMACH_ClearErrIntPending(&oI2SMULTIDma);	
	
	DMACH_Setup(g_oI2SMULTIInfor[AUDIO_PORT0].m_eDmaCh, 0x0, (u32)g_oI2SMULTIInfor[AUDIO_PORT0].m_uI2SMULTIRxFifoAddr, true,
				(u32)g_oI2SMULTIInfor[AUDIO_PORT0].m_uI2SMULTITxFifoAddr, true, WORD, I2SMULTI_RECORD_LENGTH/4, DEMAND, 
				(DREQ_SRC)(g_oI2SMULTIInfor[AUDIO_PORT0].m_eDreqSrc), DMA0_HSI_I2SV40_TX, SINGLE, &oI2SMULTIDma);

	DMACH_Start(&oI2SMULTIDma);	
	INTC_Enable(g_oI2SMULTIInfor[AUDIO_PORT0].m_uNumDma);	
	INTC_ClearVectAddr();	
#if 0

#endif
	
}

void __irq ISR_I2SMULTI_UnderRunAC(void)
{
	INTC_Disable(NUM_I2S);
	if ( I2S_Inp32(AUDIO_PORT0, 0x00) & I2SMULTI_Clear_Underrun_Intr )
	{
		UART_Printf("\nI2S Port0 TX Under Run!!\n");
		I2SCON_ClrUnderIntr(AUDIO_PORT0);
	}
	if (I2S_Inp32(AUDIO_PORT1, 0x00) & I2SMULTI_Clear_Underrun_Intr)
	{
		UART_Printf("\nI2S Port1 TX Under Run!!\n");
		I2SCON_ClrUnderIntr(AUDIO_PORT1);
	}
	if (I2SMULTI_Inp32(rI2SMULTICON) & I2SMULTI_Clear_Underrun_Intr)
	{
		UART_Printf("\nI2S Multichannel TX Under Run!!\n");
		I2SMUlTICON_ClrIntr();
	}
	
	INTC_ClearVectAddr();	
	INTC_Enable(NUM_I2S);

}

/*---------------------------------- APIs of rI2SMULTICON ---------------------------------*/
//////////
// Function Name : I2SMULTICON_SetIFMode
// Function Description : 
//   This function implements selection of Tx/Rx Transfer mode
// Input : 	AUDIO_PORT		- I2SMULTI controller number
//			I2SMULTI_IFMode		- Tx/Rx mode
// Output : NONE
// Version : v0.1

void I2SMULTICON_SetIFMode(AUDIO_PORT ePort, I2S_IFMode eIFMode)
{
	u32 uRegValue = 0;	

	uRegValue = I2SMULTI_Inp32(rI2SMULTICON); 
	// rI2SMULTICON : TxDMAPause(6), RxDMAPause(5), TxCHPause(4), RxCHPause(3), TxDMAActive(2), RxDMAActive(1)
	switch(eIFMode)
	{
		case TXOnly:
			uRegValue |= I2SMULTI_Enable_TX_DMA;
			uRegValue &= ~I2SMULTI_Enable_TX_DMA & ~I2SMULTI_Pause_TX_IF & ~I2SMULTI_Pause_TX_DMA;			
			break;
		case RXOnly :
			uRegValue |= I2SMULTI_Enable_RX_DMA;
			uRegValue &= ~I2SMULTI_Enable_RX_DMA & ~I2SMULTI_Pause_RX_IF & ~I2SMULTI_Pause_RX_DMA;			
			break;
		case TXRXBoth : 
			uRegValue |= I2SMULTI_Enable_TX_DMA | I2SMULTI_Enable_RX_DMA;
			uRegValue &= ~I2SMULTI_Enable_TX_DMA & ~I2SMULTI_Pause_TX_IF & ~I2SMULTI_Enable_TX_DMA & ~I2SMULTI_Pause_TX_IF & ~I2SMULTI_Pause_TX_DMA & ~I2SMULTI_Pause_RX_DMA;						
			break;
		default :
			break;
	}	
	I2SMULTI_Outp32( rI2SMULTICON, (uRegValue | I2SMULTI_Enable_I2SIF));	
}

void I2SMULTICON_ActiveIF(AUDIO_PORT ePort, u8 ucActive)
{
	u32 uRegValue = 0;	

	uRegValue = I2SMULTI_Inp32(rI2SMULTICON); 	
	if ( ucActive == true)
		uRegValue |= I2SMULTI_Enable_I2SIF;
	else
		uRegValue &= ~I2SMULTI_Enable_I2SIF;
	I2SMULTI_Outp32( rI2SMULTICON, uRegValue);	
}

//////////
// Function Name : I2SMULTICON_ActiveDMA
// Function Description : 
//   This function implements activation/deactivation of I2SMULTI DMA.
// Input : 	ePort		- I2SMULTI controller number
//			eXferMode	- Tx/Rx mode
// Output : NONE
// Version : v0.1
void I2SMULTICON_ActiveDMA(AUDIO_PORT ePort, I2S_IFMode eIFMode, u8 ucActive)
{
	u32 uRegValue = 0;	

	uRegValue = I2SMULTI_Inp32(rI2SMULTICON);
	switch(eIFMode)
	{
		case TXOnly:
			if (ucActive == ACTIVE)
				uRegValue |= I2SMULTI_Enable_TX_DMA;
			else if (ucActive == INACTIVE)
				uRegValue &= ~I2SMULTI_Enable_TX_DMA;
			break;
		case RXOnly:
			if (ucActive == ACTIVE)
				uRegValue |= I2SMULTI_Enable_RX_DMA;
			else if (ucActive == INACTIVE)
				uRegValue &= ~I2SMULTI_Enable_RX_DMA;
			break;
		case TXRXBoth:
			if (ucActive == ACTIVE)
				uRegValue &= ~I2SMULTI_Enable_TX_DMA | I2SMULTI_Enable_RX_DMA;
			else if (ucActive == INACTIVE)
				uRegValue &= ~I2SMULTI_Enable_TX_DMA & ~I2SMULTI_Enable_RX_DMA;
			break;	
	}
	I2SMULTI_Outp32( rI2SMULTICON, uRegValue);	
}

//////////
// Function Name : I2SMULTICON_PauseDMA
// Function Description : 
//   This function implements pause/resume of I2SMULTI Channel.
// Input : 	ePort		- I2SMULTI controller number
//			eXferMode	- Tx/Rx mode
// Output : NONE
// Version : v0.1
void I2SMULTICON_PauseDMA(AUDIO_PORT ePort, I2S_IFMode eIFMode, u8 ucPause)
{
	u32 uRegValue = 0;	

	uRegValue = I2SMULTI_Inp32(rI2SMULTICON);
	switch(eIFMode)
	{
		case TXOnly:
			if (ucPause == PAUSE)
				uRegValue |= I2SMULTI_Pause_TX_DMA;
			else if (ucPause == PLAY)
				uRegValue &= ~I2SMULTI_Pause_TX_DMA;
			break;
		case RXOnly:
			if (ucPause == PAUSE)
				uRegValue |= I2SMULTI_Pause_RX_DMA;
			else if (ucPause == PLAY)
				uRegValue &= ~I2SMULTI_Pause_RX_DMA;
			break;
		case TXRXBoth:
			if (ucPause == PAUSE)
				uRegValue &= ~I2SMULTI_Pause_TX_DMA | I2SMULTI_Pause_RX_DMA;
			else if (ucPause == PLAY)
				uRegValue &= ~I2SMULTI_Pause_TX_DMA & ~I2SMULTI_Pause_RX_DMA;
			break;	
	}
	I2SMULTI_Outp32( rI2SMULTICON, uRegValue);	
}

//////////
// Function Name : I2SMULTICON_PauseIF
// Function Description : 
//   This function implements activation/deactivation of I2SMULTI.
// Input : 	ePort		- I2SMULTI controller number
//			eXferMode	- Tx/Rx mode
// Output : NONE
// Version : v0.1
void I2SMULTICON_PauseIF(AUDIO_PORT ePort, I2S_IFMode eIFMode, u8 ucPause)
{
	u32 uRegValue = 0;	

	uRegValue = I2SMULTI_Inp32(rI2SMULTICON);
	switch(eIFMode)
	{
		case TXOnly:
			if (ucPause == PAUSE)
				uRegValue |= I2SMULTI_Pause_TX_IF;
			else if (ucPause == PLAY)
				uRegValue &= ~I2SMULTI_Pause_TX_IF;
			break;
		case RXOnly:
			if (ucPause == PAUSE)
				uRegValue |= I2SMULTI_Pause_RX_IF;
			else if (ucPause == PLAY)
				uRegValue &= ~I2SMULTI_Pause_RX_IF;
			break;
		case TXRXBoth:
			if (ucPause == PAUSE)
				uRegValue &= ~I2SMULTI_Pause_TX_IF | I2SMULTI_Pause_RX_IF;
			else if (ucPause == PLAY)
				uRegValue &= ~I2SMULTI_Pause_TX_IF & ~I2SMULTI_Pause_RX_IF;
			break;	
	}

	I2SMULTI_Outp32( rI2SMULTICON, uRegValue);	
}

void I2SMULTICON_EnableIntr(AUDIO_PORT rhs_ePort, u32 rhs_uActive)
{
	u32 uRegValue = 0;
	uRegValue =  I2SMULTI_Inp32(rI2SMULTICON);
	if (rhs_uActive == ACTIVE)
		uRegValue |= I2SMULTI_Enable_Underrun_Intr;		
		
	else 	
		uRegValue &= ~I2SMULTI_Enable_Underrun_Intr;		
	
	I2SMULTI_Outp32(rI2SMULTICON, uRegValue);	
}

void I2SMUlTICON_ClrIntr(void)
{
	u32 uRegValue = 0;
	uRegValue =  I2SMULTI_Inp32(rI2SMULTICON);
	uRegValue |= I2SMULTI_Clear_Underrun_Intr;
	I2SMULTI_Outp32(rI2SMULTICON, uRegValue);
	
}
/*---------------------------------- APIs of rI2SMULTIMOD ---------------------------------*/
void I2SMULTIMOD_PutData(AUDIO_PORT ePort, I2SMULTI_Infor * rhs_pInfor)
{
	u32 uTemp;
	
	uTemp = (rhs_pInfor->m_eChannel2Discard << 20) | (rhs_pInfor->m_eChannel1Discard << 18) | (rhs_pInfor->m_eDataChannel << 16) | 
				(rhs_pInfor->m_eWordLength << 13) | (rhs_pInfor->m_eOpMode << 11 ) | 
				(rhs_pInfor->m_eIFMode << 8) | (rhs_pInfor->m_eLRCLKPolarity  << 7) | (rhs_pInfor->m_eDataFormat << 5) |
				(rhs_pInfor->m_eRFS << 3) | (rhs_pInfor->m_eBFS << 1);
	
	if(rhs_pInfor->m_CLKSRC == I2SMUL_PCLK_SRC)
		uTemp &= ~ (1 << 12) & ~(1 << 10);	
	else if (rhs_pInfor->m_CLKSRC == I2SMUL_EXTERNALCDCLK)
		uTemp |= (1 << 12) |( 1 << 10);		
	else 	
		uTemp |= (0 << 12) | ( 1 << 10);
	
	I2SMULTI_Outp32(rI2SMULTIMOD, uTemp);
}

/*---------------------------------- APIs of rI2SMULTIFIC ---------------------------------*/
//////////
// Function Name : I2SMULTIFIC_AutoFlush
// Function Description : 
//   This function implements the I2SMULTI FIFO Flush.
// Input : ePort 		- I2SMULTI port number
//		 eIFMode		- I2SMULTI Interface
// Output : NONE
// Version : v0.1
void I2SMULTIFIC_AutoFlush(AUDIO_PORT ePort, I2S_IFMode eIFMode)
{
	u32 uRegValue = 0;	

	// FIFO Flush
	uRegValue = I2SMULTI_Inp32(rI2SMULTIFIC);

	if ( eIFMode == TXOnly )
	{
		uRegValue |= I2SMULTI_Flush_TX_FIFO;
	}
	else if ( eIFMode == RXOnly )
	{
		uRegValue |= I2SMULTI_Flush_RX_FIFO;
	}
	else if ( eIFMode == TXRXBoth )
	{
		uRegValue |= I2SMULTI_Flush_TX_FIFO | I2SMULTI_Flush_RX_FIFO;
	}
	I2SMULTI_Outp32( rI2SMULTIFIC, uRegValue);
	Delay(1000);
	I2SMULTI_Outp32( rI2SMULTIFIC, 0);
}

/*---------------------------------- APIs of rI2SMULTIPSR ---------------------------------*/
//////////
// Function Name : I2SMULTIPSR_SetPrescaler
// Function Description : 
//   This function implements the I2SMULTI Clock Prescaler.
// Input : ePort 		- I2SMULTI controller number
//			ucPSRActiveFlag	- Pre-scaler Active
//			usPSRDivValue	- Pre-scaler division value.
// Output : NONE
// Version : v0.1
void I2SMULTIPSR_SetPrescaler(AUDIO_PORT ePort, u32 rhs_uPrsVal)
{
	u32 uTemp = 0;
	uTemp= (I2SMULTI_Enable_Prescaler) | (rhs_uPrsVal << 8);
	I2SMULTI_Outp32( rI2SMULTIPSR, uTemp);
}
void I2SMULTIPSR_ActivePRS(AUDIO_PORT ePort, u8 uActive)
{
	u32 uTemp;
	uTemp = I2SMULTI_Inp32(rI2SMULTIPSR);
	if (uActive == ACTIVE)
		uTemp |= (I2SMULTI_Enable_Prescaler);
	else
		uTemp &= ~(I2SMULTI_Enable_Prescaler);
	I2SMULTI_Outp32( rI2SMULTIPSR, uTemp);
}
void I2SMULTIPSR_PutPRSValue(AUDIO_PORT ePort, u32 rhs_uPrsVal)
{
	u32 uTemp;
	uTemp = I2SMULTI_Inp32(rI2SMULTIPSR) & ~(0x3f << 8);	
	uTemp |=(rhs_uPrsVal << 8);
	I2SMULTI_Outp32( rI2SMULTIPSR, uTemp);
}

/*---------------------------------- APIs of rI2SMULTITXD ---------------------------------*/

/*---------------------------------- APIs of rI2SMULTIRXD ---------------------------------*/



/*---------------------------------- APIs of Clock ---------------------------------*/
//Selection CLK
void I2SMULTI_SelCLKAUDIO(AUDIO_PORT ePort, I2SMULTI_CLKSRC eClkSrc)
{
	
	s32 uClkSrc, uEpllCon0;

	uClkSrc = Inp32SYSC(0x10C);
	uEpllCon0 = Inp32SYSC(0x14);

	if(eClkSrc == I2SMUL_EPLL_OUT) 
	{
		Outp32SYSC(0x14, uEpllCon0 |(1<<31));
		Delay(100);		
	}	
	Outp32SYSC(0x10C, uClkSrc & ~(0x7 << 0) | (eClkSrc << 0) );		
}

u32 I2SMULTI_GetPRSVal(AUDIO_PORT ePort, I2SMULTI_Infor * rhs_eInfor)
{
	double dVclkSrc, dTempVal;
	double dTempCDCLK;
	u32 uDivAudio;
	
	if(rhs_eInfor->m_CLKSRC == I2SMUL_EPLL_OUT)
		dVclkSrc = I2SMULTI_SetEPLL(ePort); 	//should be changed according to your system clock condition : 8800000*3.35
	else if(rhs_eInfor->m_CLKSRC == I2SMUL_PCLK_SRC)
		dVclkSrc = 66660000;			//should be changed according to your system clock condition
	else if(rhs_eInfor->m_CLKSRC == I2SMUL_MPLL_OUT)
		dVclkSrc = I2SMULTI_GetMPLL(ePort); 	//should be changed according to your system clock condition : 266000000
	else if(rhs_eInfor->m_CLKSRC == I2SMUL_SYSTEM_FIN)
		dVclkSrc =  12000000; 	//should be changed according to your system clock condition
	else if(rhs_eInfor->m_CLKSRC == I2SMUL_EXTERNALCDCLK)
		dVclkSrc = I2SMULTI_EXTERNALCLK;		
	else if(rhs_eInfor->m_CLKSRC == I2SMUL_PCMEXTCLK1)
		dVclkSrc = I2SMULTI_PCMEXTECLK;	//should be changed according to your system clock condition
			
	uDivAudio = (Inp32SYSC(0x24) >> 24 )& 0xf;			//0x24 : rCLK_DIV2
	dVclkSrc = dVclkSrc / (uDivAudio + 1);
	
	if (rhs_eInfor->m_eRFS == RFS_512fs)
		dTempCDCLK = rhs_eInfor->m_dSamplingFreq * 512;
	else if (rhs_eInfor->m_eRFS == RFS_256fs)
		dTempCDCLK = rhs_eInfor->m_dSamplingFreq * 256;
	else if (rhs_eInfor->m_eRFS == RFS_384fs)
		dTempCDCLK = rhs_eInfor->m_dSamplingFreq * 384;
	else if (rhs_eInfor->m_eRFS == RFS_768fs)
		dTempCDCLK = rhs_eInfor->m_dSamplingFreq * 768;
	
	dTempVal = dVclkSrc/dTempCDCLK;		
	
	if ( ((int)(dTempVal*10) % 10) < 5)		//Round off 
		dTempVal = dTempVal-1;
	
	UART_Printf("Source CLK is %3.3fMHz\n", dVclkSrc / 1000000);	
	UART_Printf("User Sets IISCDCLK is %3.3fMHz\n",(double)(dTempCDCLK/1000000));
	UART_Printf("Prescaler Value is 0x%x\n", (u32) dTempVal);	
	UART_Printf("IISCDCLK is %3.3fMHz\n",(dVclkSrc/(((int)dTempVal)+1))/1000000);
	
	return (u32)(dTempVal);
	
}


u32 I2SMULTI_GetPRSVal2(AUDIO_PORT ePort, I2SMULTI_Infor * rhs_eInfor, u32 uSourceCLK, u32 uTargetCLK)
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
// Function Name : I2SMULTI_SetEPLL
// Function Description : 
//   This function selects the EPLL source as I2SMULTI's master clock.
// Input : ePort - I2SMULTI port  number
// Output : NONE
// Version : 
// Example :
//	FOUTepll = 84.6666MHz
//	Value of Prescaler in I2SMULTI block : 4
//		--> I2SMULTICDCLK : 84.6666/(4+1) = 16.93333MHz
//	RFS	: 384fs ( I2SMULTICDCLK/384 = 44.09KHz)
//	BFS : 48fs (RFS*48 = 2.116MHz)
double I2SMULTI_SetEPLL(AUDIO_PORT ePort)
{
	double fFOUTEpll, fDivdValue, fTemp;
	u8 ucSel = 0;
	u16 uPdiv, uMdiv, uSdiv, uKdiv;
	u32 uEPLLCON0, uEPLLCON1, uDivdValue;	
	if (g_oI2SMULTIInfor[ePort].m_eRFS == RFS_512fs)
		fFOUTEpll = g_oI2SMULTIInfor[ePort].m_dSamplingFreq * 512;
	else if (g_oI2SMULTIInfor[ePort].m_eRFS == RFS_256fs)
		fFOUTEpll = g_oI2SMULTIInfor[ePort].m_dSamplingFreq * 256;
	else if (g_oI2SMULTIInfor[ePort].m_eRFS == RFS_384fs)
		fFOUTEpll = g_oI2SMULTIInfor[ePort].m_dSamplingFreq * 384;
	else if (g_oI2SMULTIInfor[ePort].m_eRFS == RFS_768fs)
		fFOUTEpll = g_oI2SMULTIInfor[ePort].m_dSamplingFreq * 768;
	
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

	uEPLLCON0 = Inp32SYSC(0x14);			//rEPLL_CON0
	uEPLLCON1 = Inp32SYSC(0x18);			//rEPLL_CON1
	uDivdValue = Inp32SYSC(0x1c);		//rCLK_SRC
	uEPLLCON0 &=  ~(0xff << 16) & ~( 0x3f << 8 ) & ~(0x7);
	uEPLLCON1 &= ~(0xffff);	
	switch(ucSel)
	{
		case 0:				//73.728MHz
			uMdiv = 24;
			uKdiv = 37748;
			uPdiv = 1;	
			uSdiv = 2;
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
	Outp32SYSC(0x14, uEPLLCON0);			//rEPLL_CON0
	Outp32SYSC(0x18, uEPLLCON1);			//rEPLL_CON0
	Outp32SYSC(0x1c, uDivdValue);			//rCLK_SRC
	Delay(1000);	
	//SYSC_CtrlCLKOUT(eCLKOUT_EPLLOUT,  0);
	GPIO_SetFunctionEach(eGPIO_F, eGPIO_14, 3);
	GPIO_SetPullUpDownEach(eGPIO_F, eGPIO_14, 0);
	return fFOUTEpll;
}

double I2SMULTI_SetEPLLClk(double EpllClk)
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



double I2SMULTI_GetEPLL(AUDIO_PORT ePort)
{
	u8 muxEpll;
	u16 uPdiv, uMdiv, uSdiv, uKdiv;
	double dTemp;
	
	//SYSC_CtrlCLKOUT(eCLKOUT_EPLLOUT,  0);
	GPIO_SetFunctionEach(eGPIO_F, eGPIO_14, 3);
	GPIO_SetPullUpDownEach(eGPIO_F, eGPIO_14, 0);
	
	muxEpll = (Inp32SYSC(0x1c) >> 2) & 0x1;				//rCLK_SRC
	uMdiv = (Inp32SYSC(0x14) >> 16) & 0xff;				//rEPLL_CON0
	uPdiv = (Inp32SYSC(0x14) >> 8) & 0x3f;				//rEPLL_CON0
	uSdiv = (Inp32SYSC(0x14) >> 0) & 0x7;					//rEPLL_CON0
	uKdiv =( (Inp32SYSC(0x18)) & 0xffff);					//rEPLL_CON1
	dTemp = (double)(((FIN/((double)uPdiv * (1 << uSdiv)))*((double) uMdiv+(double) ((double)uKdiv / 65536))));
	if (muxEpll == 0x1)
		return dTemp;
	else
		return FIN;	
}

double I2SMULTI_GetMPLL(AUDIO_PORT ePort)
{
	u8 muxMpll;
	u16 uPdiv, uMdiv, uSdiv;
	u16 uDivMPLL;
	double temp;

	muxMpll = (Inp32SYSC(0x1c) >> 1) & 0x1;			//rCLK_SRC
	uMdiv = (Inp32SYSC(0x10) >> 16) & 0x3ff;			//rMPLL_CON
	uPdiv = (Inp32SYSC(0x10) >> 8) & 0x3f;			//rMPLL_CON
	uSdiv = (Inp32SYSC(0x10) >> 0) & 0x7;			//rMPLL_CON
	uDivMPLL = (Inp32SYSC(0x20) >> 4) & 0x1;		//rCLK_DIV0
	
	temp = ((FIN/((double)uPdiv * (1 << uSdiv)))*((double) uMdiv));
	temp = temp  / (uDivMPLL+1);
	if (muxMpll == 0x1)
		return temp;
	else
		return (FIN/(uDivMPLL+1));	
}

u32 I2SMULTI_MakeChannel(I2SMULTI_Infor* rhs_pInfor)
{
	u32* pPlayAddr = rhs_pInfor->m_uPlayBufferStartAddr;
	u32* pRecAddr = rhs_pInfor->m_uRecBufferStartAddr;
	u32 uDataBuffer0=0, uDataBuffer1=0;
	s32 uDataLen = (s32) rhs_pInfor->m_uPcmDataSize;
	u32 uCnt0, uLoopCnt=0;

	switch(rhs_pInfor->m_eDataChannel)
	{
		case TX0:
			uLoopCnt = 1;
			break;
		case TX1:
		case TX2:
			uLoopCnt = 2;
			break;
		case ALLTX:
			uLoopCnt = 3;
			break;
		default: 
			return FALSE;	
	}
	if ( rhs_pInfor->m_eWordLength == Word24)
	{
		do
		{
			uDataBuffer0 = *pRecAddr++;
			uDataBuffer1 = *pRecAddr++;
			for (uCnt0=0; uCnt0 < uLoopCnt; uCnt0++)
			{
				*pPlayAddr++ = uDataBuffer0;
				*pPlayAddr++ = uDataBuffer1;
			}
			uDataLen -= 8;
		}while(uDataLen>=0);	
	}
	else
	{
		do
		{
			uDataBuffer0 = *pRecAddr++;
			for (uCnt0=0; uCnt0 < uLoopCnt; uCnt0++)
			{
				*pPlayAddr++ = uDataBuffer0;				
			}
			uDataLen -= 4;
		}while(uDataLen>=0);	
	}
	return TRUE;
}

void I2SMULTI_MergeChannel(I2SMULTI_Infor * rhs_pInfor)
{
	u32* pPlayAddr = rhs_pInfor->m_uPlayBufferStartAddr;
	u32* pRecAddr0 = rhs_pInfor->m_uRecBufferStartAddr;
	u32* pRecAddr1 = rhs_pInfor->m_uRecBufferStartAddr1;
	u32* pRecAddr2 = rhs_pInfor->m_uRecBufferStartAddr2;
	u32 uSize = I2SMULTI_RECORD_LENGTH;	
	PCMWordLength eWord =rhs_pInfor->m_eWordLength;
	uSize = (uSize /4) *4;
	switch(rhs_pInfor->m_eDataChannel)
	{
		case TX0:
			do{
				*(pPlayAddr++) = *(pRecAddr0++);
				uSize -= 4;
			}while(uSize != 0); 
			rhs_pInfor->m_uPcmDataSize = I2SMULTI_RECORD_LENGTH;
			break;
		case TX1:
		case TX2:
			do{
				*(pPlayAddr++) = *(pRecAddr0++);
				if (eWord == Word24)
					*(pPlayAddr++) = *(pRecAddr0++);
					
				*(pPlayAddr++) = *(pRecAddr1++);
				if (eWord == Word24)
					*(pPlayAddr++) = *(pRecAddr1++);
				uSize -= 4;
			}while(uSize != 0); 	
			rhs_pInfor->m_uPcmDataSize = I2SMULTI_RECORD_LENGTH * 2;
			break;			
		case ALLTX:
			do{
				*(pPlayAddr++) = *(pRecAddr0++);
				if (eWord == Word24)
					*(pPlayAddr++) = *(pRecAddr0++);
					
				*(pPlayAddr++) = *(pRecAddr1++);
				if (eWord == Word24)
					*(pPlayAddr++) = *(pRecAddr1++);
					
				*(pPlayAddr++) = *(pRecAddr2++);
				if (eWord == Word24)
					*(pPlayAddr++) = *(pRecAddr2++);
				uSize -= 4;
			}while(uSize != 0);
			rhs_pInfor->m_uPlayDataSize = I2SMULTI_RECORD_LENGTH * 3;
			break;
		default:
			break;
	}
	
}

void I2SMULTI_MergeSerial(I2SMULTI_Infor* rhs_pInfor)
{
	u32* pPlayAddr = rhs_pInfor->m_uPlayBufferStartAddr;
	u32* pRecAddr0 = rhs_pInfor->m_uRecBufferStartAddr;
	u32* pRecAddr1 = rhs_pInfor->m_uRecBufferStartAddr1;
	u32* pRecAddr2 = rhs_pInfor->m_uRecBufferStartAddr2;
	u32 uSize = rhs_pInfor->m_uPcmDataSize;
	PCMWordLength eWord =rhs_pInfor->m_eWordLength;
	s32 	iCnt;	
	
	uSize = (uSize /4) *4;
	switch(rhs_pInfor->m_eDataChannel)
	{
		case TX0:
			do{
				if (eWord == Word24)
				{
					*(pPlayAddr++) = *(pRecAddr0++);
					*(pPlayAddr++) = *(pRecAddr0++);												
					uSize -= 8;
				}							
				else 
				{
					*(pPlayAddr++) = *(pRecAddr0++);												
					uSize -= 4;
				}				
			}while(uSize != 0); 
			rhs_pInfor->m_uPlayDataSize = rhs_pInfor->m_uPcmDataSize * 3;
			break;
		case TX1:
		case TX2:
			for (iCnt = 0; iCnt < 2; iCnt++)
			{
				switch(iCnt)
				{
					case 0:
						do{
							if (eWord == Word24)
							{
								*(pPlayAddr++) = *(pRecAddr0++);
								*(pPlayAddr++) = *(pRecAddr0++);
								*(pPlayAddr++) = 0x0000;
								*(pPlayAddr++) = 0x0000;								
								uSize -= 8;
							}							
							else 
							{
								*(pPlayAddr++) = *(pRecAddr0++);
								*(pPlayAddr++) = 0x0000;								
								uSize -= 4;
							}							
						}while(uSize != 0); 	
						break;
					case 1:
						do{
							
							if (eWord == Word24)
							{
								*(pPlayAddr++) = 0x0000;
								*(pPlayAddr++) = 0x0000;
								*(pPlayAddr++) = *(pRecAddr1++);
								*(pPlayAddr++) = *(pRecAddr1++);								
								uSize -= 8;

							}							
							else 
							{
								*(pPlayAddr++) = 0x0000;
								*(pPlayAddr++) = *(pRecAddr1++);								
								uSize -= 4;
							}							
							
						}while(uSize != 0); 
						break;
				}
				uSize = rhs_pInfor->m_uPcmDataSize;				
			}
			rhs_pInfor->m_uPlayDataSize = rhs_pInfor->m_uPcmDataSize * 6;
			break;			
		case ALLTX:
			for (iCnt = 0; iCnt < 3; iCnt++)
			{
				switch(iCnt)
				{
					case 0:
						do{
							if (eWord == Word24)
							{
								*(pPlayAddr++) = *(pRecAddr0++);
								*(pPlayAddr++) = *(pRecAddr0++);
								*(pPlayAddr++) = 0x0000;
								*(pPlayAddr++) = 0x0000;
								*(pPlayAddr++) = 0x0000;
								*(pPlayAddr++) = 0x0000;
								uSize -= 8;

							}							
							else 
							{
								*(pPlayAddr++) = *(pRecAddr0++);
								*(pPlayAddr++) = 0x0000;
								*(pPlayAddr++) = 0x0000;
								uSize -= 4;

							}
						}while(uSize != 0);
						break;
					case 1:
						do{
							if (eWord == Word24)
							{
								*(pPlayAddr++) = 0x0000;
								*(pPlayAddr++) = 0x0000;
								*(pPlayAddr++) = *(pRecAddr1++);
								*(pPlayAddr++) = *(pRecAddr1++);
								*(pPlayAddr++) = 0x0000;
								*(pPlayAddr++) = 0x0000;
								uSize -= 8;

							}							
							else 
							{
								*(pPlayAddr++) = 0x0000;
								*(pPlayAddr++) = *(pRecAddr1++);
								*(pPlayAddr++) = 0x0000;
								uSize -= 4;
							}							
						}while(uSize != 0);
						break;
					case 2:
						
						do{
							if (eWord == Word24)
							{
								*(pPlayAddr++) = 0x0000;
								*(pPlayAddr++) = 0x0000;
								*(pPlayAddr++) = 0x0000;
								*(pPlayAddr++) = 0x0000;
								*(pPlayAddr++) = *(pRecAddr2++);
								*(pPlayAddr++) = *(pRecAddr2++);
								uSize -= 8;

							}							
							else 
							{
								*(pPlayAddr++) = 0x0000;
								*(pPlayAddr++) = 0x0000;
								*(pPlayAddr++) = *(pRecAddr2++);
								uSize -= 4;
							}							
						}while(uSize != 0);
						break;
				}
				uSize = rhs_pInfor->m_uPcmDataSize;
			}
			
			rhs_pInfor->m_uPlayDataSize = rhs_pInfor->m_uPcmDataSize * 9;
			
			break;
		default:
			break;
	}
}

void I2SMULTI_MergeEachCh(I2SMULTI_Infor* rhs_pInfor)
{
	u32* pPlayAddr = rhs_pInfor->m_uPlayBufferStartAddr;
	u32* pRecAddr0 = rhs_pInfor->m_uRecBufferStartAddr;
	u32* pRecAddr1 = rhs_pInfor->m_uRecBufferStartAddr1;
	u32* pRecAddr2 = rhs_pInfor->m_uRecBufferStartAddr2;
	u32 uSize = rhs_pInfor->m_uPcmDataSize;
	PCMWordLength eWord =rhs_pInfor->m_eWordLength;
	s32 	iCnt;	
	
	uSize = (uSize /4) *4;
	switch(rhs_pInfor->m_eDataChannel)
	{
		case TX0:
			do{
				if (eWord == Word24)
				{
					*(pPlayAddr++) = *(pRecAddr0++);
					*(pPlayAddr++) = *(pRecAddr0++);												
					uSize -= 8;
				}							
				else 
				{
					*(pPlayAddr++) = *(pRecAddr0++);												
					uSize -= 4;
				}				
			}while(uSize != 0); 
			if (eWord == Word24)
				rhs_pInfor->m_uPlayDataSize = rhs_pInfor->m_uPcmDataSize * 2;
			else
				rhs_pInfor->m_uPlayDataSize = rhs_pInfor->m_uPcmDataSize;
			break;
		case TX1:
		case TX2:
			for (iCnt = 0; iCnt < 2; iCnt++)
			{
				switch(iCnt)
				{
					case 0:
						do
						{
							
							if (eWord == Word24)
							{
								if ( uSize > (rhs_pInfor->m_uPcmDataSize) / 2)
								{
									*(pPlayAddr++) = *(pRecAddr0++);
									*(pPlayAddr++) = 0x0000;
									*(pPlayAddr++) = 0x0000;
									*(pPlayAddr++) = 0x0000;
								}
								else
								{
									*(pPlayAddr++) = 0x0000;
									*(pPlayAddr++) = *(pRecAddr0++);
									*(pPlayAddr++) = 0x0000;
									*(pPlayAddr++) = 0x0000;

								}
								*(pRecAddr0++);
								uSize -= 8;
							}							
							else 
							{
								if ( uSize > (rhs_pInfor->m_uPcmDataSize) / 2)
								{
									*(pPlayAddr++) = *(pRecAddr0++) & 0xffff0000;
									*(pPlayAddr++) = 0x0000;								
									
								}
								else
								{
									*(pPlayAddr++) = *(pRecAddr0++) & 0xffff;
									*(pPlayAddr++) = 0x0000;

								}
								uSize -= 4;
							}							
						}while(uSize != 0); 	
						break;
					case 1:
						do{
							
							if (eWord == Word24)
							{
								if ( uSize > (rhs_pInfor->m_uPcmDataSize) / 2)
								{
									*(pPlayAddr++) = 0x0000;
									*(pPlayAddr++) = 0x0000;
									*(pPlayAddr++) = *(pRecAddr1++);
									*(pPlayAddr++) = 0x0000;		
								}
								else
								{
									*(pPlayAddr++) = 0x0000;
									*(pPlayAddr++) = 0x0000;
									*(pPlayAddr++) = 0x0000;
									*(pPlayAddr++) = *(pRecAddr1++);		

								}
								*(pRecAddr1++);	
								uSize -= 8;	
							}							
							else 
							{
								if ( uSize > (rhs_pInfor->m_uPcmDataSize) / 2)
								{
									*(pPlayAddr++) = 0x0000;
									*(pPlayAddr++) = *(pRecAddr1++) & 0xffff0000;		
								}
								else
								{
									*(pPlayAddr++) = 0x0000;
									*(pPlayAddr++) = *(pRecAddr1++) & 0xffff;
								}
								uSize -= 4;
							}							
						}while(uSize != 0); 
						break;
				}
				uSize = rhs_pInfor->m_uPcmDataSize;				
			}
			rhs_pInfor->m_uPlayDataSize = rhs_pInfor->m_uPcmDataSize * 4;
			break;			
		case ALLTX:
			for (iCnt = 0; iCnt < 3; iCnt++)
			{
				switch(iCnt)
				{
					case 0:
						do{
							if (eWord == Word24)
							{
								if ( uSize > (rhs_pInfor->m_uPcmDataSize) / 2)
								{
									*(pPlayAddr++) = *(pRecAddr0++);									
									*(pPlayAddr++) = 0x0000;
									*(pPlayAddr++) = 0x0000;
									*(pPlayAddr++) = 0x0000;
									*(pPlayAddr++) = 0x0000;
									*(pPlayAddr++) = 0x0000;
								
								}
								else
								{
									*(pPlayAddr++) = 0x0000;
									*(pPlayAddr++) = *(pRecAddr0++);
									*(pPlayAddr++) = 0x0000;
									*(pPlayAddr++) = 0x0000;
									*(pPlayAddr++) = 0x0000;
									*(pPlayAddr++) = 0x0000;
								}
								*(pRecAddr0++);
								uSize -= 8;
							}							
							else 
							{
								if ( uSize > (rhs_pInfor->m_uPcmDataSize) / 2)
								{
									*(pPlayAddr++) = *(pRecAddr0++) & 0xffff0000;
									*(pPlayAddr++) = 0x0000;
									*(pPlayAddr++) = 0x0000;
								}
								else
								{
									*(pPlayAddr++) = *(pRecAddr0++) & 0xffff;
									*(pPlayAddr++) = 0x0000;
									*(pPlayAddr++) = 0x0000;

								}
								uSize -= 4;

							}
						}while(uSize != 0);
						break;
					case 1:
						do{
							if (eWord == Word24)
							{
								if ( uSize > (rhs_pInfor->m_uPcmDataSize) / 2)
								{
									*(pPlayAddr++) = 0x0000;
									*(pPlayAddr++) = 0x0000;
									*(pPlayAddr++) = *(pRecAddr1++);
									*(pPlayAddr++) = 0x0000;
									*(pPlayAddr++) = 0x0000;
									*(pPlayAddr++) = 0x0000;
								}
								else 
								{
									*(pPlayAddr++) = 0x0000;
									*(pPlayAddr++) = 0x0000;
									*(pPlayAddr++) = 0x0000;
									*(pPlayAddr++) = *(pRecAddr1++);
									*(pPlayAddr++) = 0x0000;
									*(pPlayAddr++) = 0x0000;

								}
								*(pRecAddr1++);
								uSize -= 8;

							}							
							else 
							{
								if ( uSize > (rhs_pInfor->m_uPcmDataSize) / 2)
								{
									*(pPlayAddr++) = 0x0000;
									*(pPlayAddr++) = *(pRecAddr1++) & 0xffff0000;
									*(pPlayAddr++) = 0x0000;
								}

								else 
								{
									*(pPlayAddr++) = 0x0000;
									*(pPlayAddr++) = *(pRecAddr1++) & 0xffff;
									*(pPlayAddr++) = 0x0000;
								}
								uSize -= 4;
							}							
						}while(uSize != 0);
						break;
					case 2:
						
						do{
							if (eWord == Word24)
							{
								if ( uSize > (rhs_pInfor->m_uPcmDataSize) / 2)
								{
									*(pPlayAddr++) = 0x0000;
									*(pPlayAddr++) = 0x0000;
									*(pPlayAddr++) = 0x0000;
									*(pPlayAddr++) = 0x0000;
									*(pPlayAddr++) = *(pRecAddr2++);
									*(pPlayAddr++) = 0x0000;
								}
								else 
								{
									*(pPlayAddr++) = 0x0000;
									*(pPlayAddr++) = 0x0000;
									*(pPlayAddr++) = 0x0000;
									*(pPlayAddr++) = 0x0000;
									*(pPlayAddr++) = 0x0000;
									*(pPlayAddr++) = *(pRecAddr2++);
								}
								*(pRecAddr2++);
								uSize -= 8;

							}							
							else 
							{
								if ( uSize > (rhs_pInfor->m_uPcmDataSize) / 2)
								{
									*(pPlayAddr++) = 0x0000;
									*(pPlayAddr++) = 0x0000;
									*(pPlayAddr++) = *(pRecAddr2++) & 0xffff0000;
								}
								else
								{
									*(pPlayAddr++) = 0x0000;
									*(pPlayAddr++) = 0x0000;
									*(pPlayAddr++) = *(pRecAddr2++) & 0xffff;
								}
								uSize -= 4;
							}							
						}while(uSize != 0);
						break;
				}
				uSize = rhs_pInfor->m_uPcmDataSize;
			}			
			rhs_pInfor->m_uPlayDataSize = rhs_pInfor->m_uPcmDataSize * 9;
			
			break;
		default:
			break;
	}
}

void I2SMULTI_MergeEachCh2(I2SMULTI_Infor* rhs_pInfor)
{
	u32* pPlayAddr = rhs_pInfor->m_uPlayBufferStartAddr;	
	
	u32* pRecAddr0 = rhs_pInfor->m_uRecBufferStartAddr;
	u32* pRecAddr1 = rhs_pInfor->m_uRecBufferStartAddr1;
	u32* pRecAddr2 = rhs_pInfor->m_uRecBufferStartAddr2;
	
	u32 uSize = rhs_pInfor->m_uPcmDataSize;
	PCMWordLength eWord =rhs_pInfor->m_eWordLength;
	s32 	iCnt;	
	u32 uWoofer = 0;
	u32 uCnt2;
	
	uSize = (uSize /4) *4;
	switch(rhs_pInfor->m_eDataChannel)
	{
		case TX0:
			do{
				if (eWord == Word24)
				{
					*(pPlayAddr++) = *(pRecAddr0++);
					*(pPlayAddr++) = *(pRecAddr0++);												
					uSize -= 8;
				}							
				else 
				{
					*(pPlayAddr++) = *(pRecAddr0++);												
					uSize -= 4;
				}				
			}while(uSize != 0); 
			rhs_pInfor->m_uPlayDataSize = rhs_pInfor->m_uPcmDataSize * 3;
			break;
		case TX1:
		case TX2:
			for (iCnt = 0; iCnt < 2; iCnt++)
			{
				switch(iCnt)
				{
					case 0:
						do
						{
							
							if (eWord == Word24)
							{
								if ( uSize > (rhs_pInfor->m_uPcmDataSize) / 2)
								{
									*(pPlayAddr++) = *(pRecAddr0++);
									*(pPlayAddr++) = 0x0000;
									*(pPlayAddr++) = 0x0000;
									*(pPlayAddr++) = 0x0000;
								}
								else
								{
									*(pPlayAddr++) = 0x0000;
									*(pPlayAddr++) = *(pRecAddr0++);
									*(pPlayAddr++) = 0x0000;
									*(pPlayAddr++) = 0x0000;

								}
								*(pRecAddr0++);
								uSize -= 8;
							}							
							else 
							{
								if ( uSize > (rhs_pInfor->m_uPcmDataSize) / 2)
								{
									*(pPlayAddr++) = *(pRecAddr0++) & 0xffff0000;
									*(pPlayAddr++) = 0x0000;								
									
								}
								else
								{
									*(pPlayAddr++) = *(pRecAddr0++) & 0xffff;
									*(pPlayAddr++) = 0x0000;

								}
								uSize -= 4;
							}							
						}while(uSize != 0); 	
						break;
					case 1:
						do{
							
							if (eWord == Word24)
							{
								if ( uSize > (rhs_pInfor->m_uPcmDataSize) / 2)
								{
									*(pPlayAddr++) = 0x0000;
									*(pPlayAddr++) = 0x0000;
									*(pPlayAddr++) = *(pRecAddr1++);
									*(pPlayAddr++) = 0x0000;		
								}
								else
								{
									*(pPlayAddr++) = 0x0000;
									*(pPlayAddr++) = 0x0000;
									*(pPlayAddr++) = 0x0000;
									*(pPlayAddr++) = *(pRecAddr1++);		

								}
								*(pRecAddr1++);	
								uSize -= 8;	
							}							
							else 
							{
								if ( uSize > (rhs_pInfor->m_uPcmDataSize) / 2)
								{
									*(pPlayAddr++) = 0x0000;
									*(pPlayAddr++) = *(pRecAddr1++) & 0xffff0000;		
								}
								else
								{
									*(pPlayAddr++) = 0x0000;
									*(pPlayAddr++) = *(pRecAddr1++) & 0xffff;
								}
								uSize -= 4;
							}							
						}while(uSize != 0); 
						break;
				}
				uSize = rhs_pInfor->m_uPcmDataSize;				
			}
			rhs_pInfor->m_uPlayDataSize = rhs_pInfor->m_uPcmDataSize * 4;
			break;			
		case ALLTX:
			for (iCnt = 0; iCnt < 10; iCnt++)
			{
				switch(iCnt)
				{
					case 0:
					case 9:
						do{
							if (eWord == Word24)
							{							
								uWoofer = *(pRecAddr0++);								
								*(pPlayAddr++) = 0x0000;
								*(pPlayAddr++) = 0x0000;
								*(pPlayAddr++) = 0x0000;
								*(pPlayAddr++) = uWoofer & 0xffff;
								*(pPlayAddr++) = uWoofer;
								*(pPlayAddr++) = 0x0000;	
								for (uCnt2 = 0; uCnt2 < 5; uCnt2++)
								{
									*(pRecAddr0++);
								}
								uSize -= (4 * uCnt2);
							}							
							else 
							{
							}
						}while(uSize != 0);
						break;
					case 1:
					case 8:
						do{
							if (eWord == Word24)
							{
								uWoofer = *(pRecAddr0++);								
								*(pPlayAddr++) = 0x0000;
								*(pPlayAddr++) = 0x0000;
								*(pPlayAddr++) = uWoofer;
								*(pPlayAddr++) = uWoofer & 0xffff;
								*(pPlayAddr++) = 0x0000;
								*(pPlayAddr++) = 0x0000;								
								for (uCnt2 = 0; uCnt2 < 5; uCnt2++)
								{
									*(pRecAddr0++);
								}
								uSize -= (4 * uCnt2);
							}							
							else 
							{
							}							
						}while(uSize != 0);
						break;
					case 2:
					case 7:						
						do{
							if (eWord == Word24)
							{
								uWoofer = *(pRecAddr1++);
								
								*(pPlayAddr++) = uWoofer;
								*(pPlayAddr++) = 0x0000;
								*(pPlayAddr++) = 0x0000;
								*(pPlayAddr++) = uWoofer & 0xffff;
								*(pPlayAddr++) = 0x0000;
								*(pPlayAddr++) = 0x0000;	
								
								for (uCnt2 = 0; uCnt2 < 5; uCnt2++)
								{
									*(pRecAddr0++);
								}
								uSize -= (4 * uCnt2);

							}							
							else 
							{								
							}							
						}while(uSize != 0);
						break;
					case 3:
					case 6:
						do{
							if (eWord == Word24)
							{
								uWoofer = *(pRecAddr1++);
								
								*(pPlayAddr++) = 0x0000;
								*(pPlayAddr++) = uWoofer;
								*(pPlayAddr++) = 0x0000;
								*(pPlayAddr++) = uWoofer & 0xffff;
								*(pPlayAddr++) = 0x0000;
								*(pPlayAddr++) = 0x0000;									
								
								for (uCnt2 = 0; uCnt2 < 5; uCnt2++)
								{
									*(pRecAddr0++);
								}
								uSize -= (4 * uCnt2);

							}							
							else 
							{								
							}							
						}while(uSize != 0);						
						break;
					case 4:
					case 5:
						do{
							if (eWord == Word24)
							{
								uWoofer = *(pRecAddr2++);
								
								*(pPlayAddr++) = 0x0000;
								*(pPlayAddr++) = 0x0000;
								*(pPlayAddr++) = 0x0000;
								*(pPlayAddr++) = uWoofer & 0xffff;
								*(pPlayAddr++) = 0x0000;
								*(pPlayAddr++) = uWoofer;									
								
								for (uCnt2 = 0; uCnt2 < 5; uCnt2++)
								{
									*(pRecAddr0++);
								}
								uSize -= (4 * uCnt2);
							}							
							else 
							{
							}
						}while(uSize != 0);								
						break;				
				}
				uSize = rhs_pInfor->m_uPcmDataSize;
			}			
			rhs_pInfor->m_uPlayDataSize = rhs_pInfor->m_uPcmDataSize * 9;
			
			break;
		default:
			break;
	}
}

void I2SMULTI_MergeEachCh3(I2SMULTI_Infor* rhs_pInfor)
{
	u32* pPlayAddr = rhs_pInfor->m_uPlayBufferStartAddr;
	u32* pRecAddr0 = rhs_pInfor->m_uRecBufferStartAddr;
	u32* pRecAddr1 = rhs_pInfor->m_uRecBufferStartAddr1;
	u32* pRecAddr2 = rhs_pInfor->m_uRecBufferStartAddr2;
	u32 uSize = rhs_pInfor->m_uPcmDataSize;
	s32 uDataVol = uSize % 0xffff;
	PCMWordLength eWord =rhs_pInfor->m_eWordLength;
	s32 	iCnt;	
	u32 uWoofer;
	
	uSize = (uSize /4) *4;
	switch(rhs_pInfor->m_eDataChannel)
	{
		case TX0:
			do{
				if (eWord == Word24)
				{
					*(pPlayAddr++) = *(pRecAddr0++);
					*(pPlayAddr++) = *(pRecAddr0++);												
					uSize -= 8;
				}							
				else 
				{
					*(pPlayAddr++) = *(pRecAddr0++);												
					uSize -= 4;
				}				
			}while(uSize != 0); 
			rhs_pInfor->m_uPlayDataSize = rhs_pInfor->m_uPcmDataSize * 3;
			break;
		case TX1:
		case TX2:
			for (iCnt = 0; iCnt < 2; iCnt++)
			{
				switch(iCnt)
				{
					case 0:
						do
						{
							
							if (eWord == Word24)
							{
								if ( uSize > (rhs_pInfor->m_uPcmDataSize) / 2)
								{
									*(pPlayAddr++) = *(pRecAddr0++);
									*(pPlayAddr++) = 0x0000;
									*(pPlayAddr++) = 0x0000;
									*(pPlayAddr++) = 0x0000;
								}
								else
								{
									*(pPlayAddr++) = 0x0000;
									*(pPlayAddr++) = *(pRecAddr0++);
									*(pPlayAddr++) = 0x0000;
									*(pPlayAddr++) = 0x0000;

								}
								*(pRecAddr0++);
								uSize -= 8;
							}							
							else 
							{
								if ( uSize > (rhs_pInfor->m_uPcmDataSize) / 2)
								{
									*(pPlayAddr++) = *(pRecAddr0++) & 0xffff0000;
									*(pPlayAddr++) = 0x0000;								
									
								}
								else
								{
									*(pPlayAddr++) = *(pRecAddr0++) & 0xffff;
									*(pPlayAddr++) = 0x0000;

								}
								uSize -= 4;
							}							
						}while(uSize != 0); 	
						break;
					case 1:
						do{
							
							if (eWord == Word24)
							{
								if ( uSize > (rhs_pInfor->m_uPcmDataSize) / 2)
								{
									*(pPlayAddr++) = 0x0000;
									*(pPlayAddr++) = 0x0000;
									*(pPlayAddr++) = *(pRecAddr1++);
									*(pPlayAddr++) = 0x0000;		
								}
								else
								{
									*(pPlayAddr++) = 0x0000;
									*(pPlayAddr++) = 0x0000;
									*(pPlayAddr++) = 0x0000;
									*(pPlayAddr++) = *(pRecAddr1++);		

								}
								*(pRecAddr1++);	
								uSize -= 8;	
							}							
							else 
							{
								if ( uSize > (rhs_pInfor->m_uPcmDataSize) / 2)
								{
									*(pPlayAddr++) = 0x0000;
									*(pPlayAddr++) = *(pRecAddr1++) & 0xffff0000;		
								}
								else
								{
									*(pPlayAddr++) = 0x0000;
									*(pPlayAddr++) = *(pRecAddr1++) & 0xffff;
								}
								uSize -= 4;
							}							
						}while(uSize != 0); 
						break;
				}
				uSize = rhs_pInfor->m_uPcmDataSize;				
			}
			rhs_pInfor->m_uPlayDataSize = rhs_pInfor->m_uPcmDataSize * 4;
			break;			
		case ALLTX:
			for (iCnt = 0; iCnt < 3; iCnt++)
			{
				switch(iCnt)
				{
					case 0:
						do{
							if (eWord == Word24)
							{
								if ( uSize > (rhs_pInfor->m_uPcmDataSize) / 2)
								{
									*(pPlayAddr++) = 0x0000;							
									*(pPlayAddr++) = 0x0000;
									*(pPlayAddr++) = 0x0000;
									*(pPlayAddr++) = 0x0000;
									*(pPlayAddr++) = 0x0000;
									*(pPlayAddr++) = *(pRecAddr0++);								
								}
								else
								{
									*(pPlayAddr++) = 0x0000;
									*(pPlayAddr++) = *(pRecAddr0++);
									*(pPlayAddr++) = 0x0000;
									*(pPlayAddr++) = 0x0000;
									*(pPlayAddr++) = 0x0000;
									*(pPlayAddr++) = 0x0000;
								}
								*(pRecAddr0++);
								uSize -= 8;
							}							
							else 
							{
								if ( uSize > (rhs_pInfor->m_uPcmDataSize) / 2)
								{
									*(pPlayAddr++) = *(pRecAddr0++) & 0xffff0000;
									*(pPlayAddr++) = 0x0000;
									*(pPlayAddr++) = 0x0000;
								}
								else
								{
									*(pPlayAddr++) = *(pRecAddr0++) & 0xffff;
									*(pPlayAddr++) = 0x0000;
									*(pPlayAddr++) = 0x0000;

								}
								uSize -= 4;

							}
						}while(uSize != 0);
						break;
					case 1:
						do{
							if (eWord == Word24)
							{
								if ( uSize > (rhs_pInfor->m_uPcmDataSize) / 2)
								{
									*(pPlayAddr++) = 0x0000;
									*(pPlayAddr++) = 0x0000;
									*(pPlayAddr++) = *(pRecAddr1++);
									*(pPlayAddr++) = 0x0000;
									*(pPlayAddr++) = 0x0000;
									*(pPlayAddr++) = 0x0000;
								}
								else 
								{
									*(pPlayAddr++) = *(pRecAddr1++);
									*(pPlayAddr++) = 0x0000;
									*(pPlayAddr++) = 0x0000;
									*(pPlayAddr++) = 0x0000;
									*(pPlayAddr++) = 0x0000;
									*(pPlayAddr++) = 0x0000;

								}
								*(pRecAddr1++);
								uSize -= 8;

							}							
							else 
							{
								if ( uSize > (rhs_pInfor->m_uPcmDataSize) / 2)
								{
									*(pPlayAddr++) = 0x0000;
									*(pPlayAddr++) = *(pRecAddr1++) & 0xffff0000;
									*(pPlayAddr++) = 0x0000;
								}

								else 
								{
									*(pPlayAddr++) = 0x0000;
									*(pPlayAddr++) = *(pRecAddr1++) & 0xffff;
									*(pPlayAddr++) = 0x0000;
								}
								uSize -= 4;
							}							
						}while(uSize != 0);
						break;
					case 2:
						
						do{
							if (eWord == Word24)
							{
								uWoofer = *(pRecAddr2++);
								if ( uSize > (rhs_pInfor->m_uPcmDataSize) / 2)
								{
									*(pPlayAddr++) = 0x0000;
									*(pPlayAddr++) = 0x0000;
									*(pPlayAddr++) = 0x0000;
									*(pPlayAddr++) = 0x0000;
									*(pPlayAddr++) = uWoofer;
									*(pPlayAddr++) = 0x0000;
								}
								else 
								{
									*(pPlayAddr++) = uWoofer;
									*(pPlayAddr++) = uWoofer;
									*(pPlayAddr++) = uWoofer;
									*(pPlayAddr++) = uWoofer;
									*(pPlayAddr++) = uWoofer;
									*(pPlayAddr++) = uWoofer;
								}
								*(pRecAddr2++);
								uSize -= 8;

							}							
							else 
							{
								if ( uSize > (rhs_pInfor->m_uPcmDataSize) / 2)
								{
									*(pPlayAddr++) = 0x0000;
									*(pPlayAddr++) = 0x0000;
									*(pPlayAddr++) = *(pRecAddr2++) & 0xffff0000;
								}
								else
								{
									*(pPlayAddr++) = 0x0000;
									*(pPlayAddr++) = 0x0000;
									*(pPlayAddr++) = *(pRecAddr2++) & 0xffff;
								}
								uSize -= 4;
							}							
						}while(uSize != 0);
						break;
				}
				uSize = rhs_pInfor->m_uPcmDataSize;
			}			
			rhs_pInfor->m_uPlayDataSize = rhs_pInfor->m_uPcmDataSize * 9;
			
			break;
		default:
			break;
	}
}


/*
void I2SMULTI_MergeEachCh4(I2SMULTI_Infor* rhs_pInfor)
{
	s32* pPlayAddr = (s32) rhs_pInfor->m_uPlayBufferStartAddr;
	s32* pRecAddr  =(s32)  rhs_pInfor->m_uRecBufferStartAddr;	
	s32 uSize = rhs_pInfor->m_uPcmDataSize;	
	
	u32 	uIndicator = 0;	
	s32 uPcmData[2];
	u32 uCnt;

	switch(rhs_pInfor->m_eDataChannel)
	{
		case TX0:
			do
			{

			}while(uSize !=0)
			break;
		case TX1:
		case TX2:
			break;			
		case ALLTX:
			do
			{
				if ( rhs_pInfor->m_eWordLength == Word24)
				{
					for (uCnt = 0; uCnt < 2; uCnt++)				// Load Data
					{
						uPcmData[uCnt] = *(pRecAddr++);
						uSize -= 4;
					}
					
					if((rhs_pInfor->m_uPcmDataSize / uSize) == 0)						// Phase 0
					{
						*(pPlayAddr++) = uPcmData;
						*(pPlayAddr++) = 0x0;
						*(pPlayAddr++) = 0x0;
						*(pPlayAddr++) = 0x0;
						*(pPlayAddr++) = 0x0;
						*(pPlayAddr++) = 0x0;
					}
					else if((uIndicator % 6) == 1)					// Phase 1
					{
					}
					else if((uIndicator % 6) == 2)					// Center
					{
					}
					else if((uIndicator % 6) == 3)					// SUB(Woofer)
					{
					}
					else if((uIndicator % 6) == 4)					// Rear Right
					{
					}
					else if((uIndicator % 6) == 5)					// Rear Left
					{
					}
					
					
					
				}
				
				else
				{
					for ( uIndicator = 0; uIndicator < 3; uIndicator++)
						*(pRecAddr0++) = uPcmData;
				}
				uIndicator++;
				
			}while(uSize !=0)
			break;
	}

	
}
*/
