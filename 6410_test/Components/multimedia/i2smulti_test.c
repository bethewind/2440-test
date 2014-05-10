/**************************************************************************************
* 
*	Project Name : S3C6410 Validation
*
*	Copyright 2008 by Samsung Electronics, Inc.
*	All rights reserved.
*
*	Project Description :
*		This software is only for validating functions of the S3C6410.
*		Anybody can use this software without our permission.
*  
*--------------------------------------------------------------------------------------
* 
*	File Name : i2smulti_test.c
*  
*	File Description : This file implements I2S Multi Channel test functions.
*
*	Author : Sung-Hyun,Na
*	Dept. : AP Development Team
*	Created Date : 2008/02/01
*	Version : 0.1 
* 
*	History
*	- Created(Sung-Hyun,Na 2008/02/01)
*   
**************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "def.h"
#include "option.h"
#include "library.h"
#include "sfr6410.h"
#include "system.h"
#include "sysc.h"
#include "gpio.h"
#include "dma.h"
#include "intc.h"
#include "iic.h"

#include "i2S.h"
#include "pcm.h"
#include "i2smulti.h"
#include "audio.h"



#include "nand.h"

/*-----------------------------------Test Functions ------------------------------------*/
void I2SMULTI_Configuration(AUDIO_PORT rhs_ePort, I2SMULTI_Infor * rhs_pInfor);

void I2SMULTI_PCMInPolling(AUDIO_PORT rhs_ePort, I2SMULTI_Infor * rhs_pInfor);
void I2SMULTI_PCMOutPolling(AUDIO_PORT rhs_ePort, I2SMULTI_Infor * rhs_pInfor);
void I2SMULTI_PCMDataInOutPolling(AUDIO_PORT rhs_ePort, I2SMULTI_Infor * rhs_pInfor);
void I2SMULTI_PCMDataBypassPolling(AUDIO_PORT rhs_ePort, I2SMULTI_Infor * rhs_pInfor);


void I2SMULTI_PCMInDMA(AUDIO_PORT, I2SMULTI_Infor * rhs_pInfor);
void I2SMULTI_PCMOutDMA(AUDIO_PORT, I2SMULTI_Infor * rhs_pInfor);
void I2SMULTI_PCMDataInOutDMA(AUDIO_PORT, I2SMULTI_Infor * rhs_pInfor);
void I2SMULTI_PCMDataBypassDMA(AUDIO_PORT rhs_ePort, I2SMULTI_Infor * rhs_pInfor);

void I2SMULTI_DataOutIntrMode(AUDIO_PORT rhs_ePort, I2SMULTI_Infor * rhs_pInfor);

void I2SMULTI_DataParser24to32(AUDIO_PORT rhs_ePort, I2SMULTI_Infor * rhs_pInfor);
void I2SMULTI_DataParserWaveTo32Fifo(AUDIO_PORT rhs_ePort, I2SMULTI_Infor * rhs_pInfor);
void I2SMULTI_Decompress32to24(AUDIO_PORT rhs_ePort, I2SMULTI_Infor * rhs_pInfor);
void I2SMULTI_LoopBack(AUDIO_PORT rhs_ePort, I2SMULTI_Infor * rhs_pInfor);
extern void Test_CLKOUT(void) ;
extern void SYSCT_Clock(void);

// global variables

I2SMULTI_Infor		g_oI2SMULTIInfor[AUDIO_NUM];

extern I2S_Infor		g_oI2SInfor[AUDIO_NUM];
extern I2S_TestMode 	g_TestMode;

extern volatile	u32		g_uI2SRecDone[I2S_CONNUM];
extern NAND_oInform NAND_Inform[NAND_CONNUM];


extern volatile u32		g_iSeed, g_iNum, g_iARMCLK_DIVN_R, g_iHCLKx2_DIVN_R, g_iPCLK_DIVN_R;
void I2SMULTI_Test(void)
{
	u32 uCountFunc=0;
	s32 iSel=0;
	double dTemp;
	AUDIO_PORT eSelPort;

	
	const I2SMULTItestFuncMenu g_aI2SMULTITestFunc[] =
	{
		//I2SMULTI Function Test Item
		0,											"Exit\n",
		
		I2SMULTI_Configuration, 					"I2SMULTI Configuration\n",	

		I2SMULTI_PCMInPolling,					"PCM Data In Polling Mode. ",
		I2SMULTI_PCMOutPolling, 					"PCM Data Out Polling Mode.", 
		I2SMULTI_PCMDataInOutPolling,			"I2SMULTI Polling In/Out\n",
		
		I2SMULTI_PCMInDMA,						"PCM Data In DMA Mode.",	
		I2SMULTI_PCMOutDMA, 					"PCM DATA Out DMA Mode.",	
		I2SMULTI_PCMDataInOutDMA,				"I2SMULTI DMA In/Out.\n",

		I2SMULTI_DataOutIntrMode,				"Interrupt Mode Test\n",

		I2SMULTI_LoopBack,						"Loop Back Test",
		
		I2SMULTI_DataParser24to32,				"Binary File Parse(24 to 32)",
		I2SMULTI_DataParserWaveTo32Fifo,		"Get Wave File Information",
		I2SMULTI_Decompress32to24,				"Decompress 32bit to 24bit PCM",

		//Test_CLKOUT,								"Test CLKOUT",
		//SYSCT_Clock,								"Test SysClock\n",
		0,0
	};

	//TestSFR();	
	eSelPort = AUDIO_PORT0;

	

	I2SMULTI_Init(AUDIO_PORT0);
	I2SMULTI_InitPort(AUDIO_PORT0);	
	while(1)
	{	
		UART_Printf("\n================================================================================\n");
		for (uCountFunc = 0; uCountFunc < I2SMULTI_CONNUM; uCountFunc++)
		{
			UART_Printf("Port : %d\t",uCountFunc);
			
			UART_Printf("Operation Mode : ");
			if ( g_oI2SMULTIInfor[uCountFunc].m_eOpMode ==  Master)
				UART_Printf("Master\t");
			else 
				UART_Printf("Slave\t");
			
			UART_Printf("PCM Word Length : ");
			if ( g_oI2SMULTIInfor[uCountFunc].m_eWordLength ==  Word8)
				UART_Printf("8bit\n");
			else if ( g_oI2SMULTIInfor[uCountFunc].m_eWordLength ==  Word16)
				UART_Printf("16bit\n");
			else 
				UART_Printf("24bit\n");

			UART_Printf("Interface Data Format : ");
			if ( g_oI2SMULTIInfor[uCountFunc].m_eDataFormat ==  I2SFormat)
				UART_Printf("I2SMULTI Format\t");
			else if ( g_oI2SMULTIInfor[uCountFunc].m_eDataFormat==  MSBJustified)
				UART_Printf("MSBJustified(Left)\t");
			else if ( g_oI2SMULTIInfor[uCountFunc].m_eDataFormat==  LSBJustified)
				UART_Printf("LSBJustified(Right)\t");

			
			UART_Printf("LRCLK Polarity : ");
			if ( g_oI2SMULTIInfor[uCountFunc].m_eLRCLKPolarity==  LeftHigh)
				UART_Printf("Left High\n");
			else if ( g_oI2SMULTIInfor[uCountFunc].m_eLRCLKPolarity==  RightHigh)
				UART_Printf("Right High\n");	

			UART_Printf("I2SMULTI Clock Source : ");
			if ( g_oI2SMULTIInfor[uCountFunc].m_CLKSRC==  I2SMUL_EPLL_OUT)
				UART_Printf("EPLL Out\t");
			else if ( g_oI2SMULTIInfor[uCountFunc].m_CLKSRC==  I2SMUL_PCLK_SRC)
				UART_Printf("PCLK\t");
			else if ( g_oI2SMULTIInfor[uCountFunc].m_CLKSRC==  I2SMUL_MPLL_OUT)
				UART_Printf("MPLL Out\t");
			else if ( g_oI2SMULTIInfor[uCountFunc].m_CLKSRC==  I2SMUL_SYSTEM_FIN)
				UART_Printf("System External CLK\t");
			else if ( g_oI2SMULTIInfor[uCountFunc].m_CLKSRC==  I2SMUL_EXTERNALCDCLK)
				UART_Printf("I2SMULTI External CLK\t");
			else if ( g_oI2SMULTIInfor[uCountFunc].m_CLKSRC==  I2SMUL_PCMEXTCLK1)
				UART_Printf("I2SMULTI External CLK\t");
			dTemp = (double)(g_oI2SMULTIInfor[uCountFunc].m_dSamplingFreq)/1000;
			UART_Printf("Sampling Frequency : %3.1fKHz\n",dTemp);
			UART_Printf("RFS : ");
			if ( g_oI2SMULTIInfor[uCountFunc].m_eRFS==  RFS_256fs)
				UART_Printf("256fs\t");
			else if ( g_oI2SMULTIInfor[uCountFunc].m_eRFS==  RFS_512fs)
				UART_Printf("512fs\t");
			else if ( g_oI2SMULTIInfor[uCountFunc].m_eRFS==  RFS_384fs)
				UART_Printf("384fs\t");
			else if ( g_oI2SMULTIInfor[uCountFunc].m_eRFS==  RFS_768fs)
				UART_Printf("768fs\t");	

			UART_Printf("BFS : ");
			if ( g_oI2SMULTIInfor[uCountFunc].m_eBFS==  BFS_32fs)
				UART_Printf("32fs\n");
			else if ( g_oI2SMULTIInfor[uCountFunc].m_eBFS==  BFS_48fs)
				UART_Printf("48fs\n");
			else if ( g_oI2SMULTIInfor[uCountFunc].m_eBFS==  BFS_16fs)
				UART_Printf("16fs\n");
			else if ( g_oI2SMULTIInfor[uCountFunc].m_eBFS==  BFS_24fs)
				UART_Printf("24fs\n");

			UART_Printf("TX Channel: ");
			if ( g_oI2SMULTIInfor[uCountFunc].m_eDataChannel==  TX0)
				UART_Printf("TX0\n");
			else if ( g_oI2SMULTIInfor[uCountFunc].m_eDataChannel==  TX1)
			{
				UART_Printf("TX0, TX1\t\t");
				UART_Printf("TX1 Channel: ");
				if ( g_oI2SMULTIInfor[uCountFunc].m_eChannel1Discard==  NoDiscard)
					UART_Printf("No Discard\n");
				else if ( g_oI2SMULTIInfor[uCountFunc].m_eChannel1Discard==  Discard15to0)
					UART_Printf("0 to 15 Discard\n");
				else if ( g_oI2SMULTIInfor[uCountFunc].m_eChannel1Discard==  Discard31to16)
					UART_Printf("16 to 31 Discard\n");				
			}
			else if ( g_oI2SMULTIInfor[uCountFunc].m_eDataChannel==  TX2)
			{
				UART_Printf("TX0, TX2\t\t");
				UART_Printf("TX2 Channel: ");
				if ( g_oI2SMULTIInfor[uCountFunc].m_eChannel2Discard==  NoDiscard)
					UART_Printf("No Discard\n");
				else if ( g_oI2SMULTIInfor[uCountFunc].m_eChannel2Discard==  Discard15to0)
					UART_Printf("0 to 15 Discard\n");
				else if ( g_oI2SMULTIInfor[uCountFunc].m_eChannel2Discard==  Discard31to16)
					UART_Printf("16 to 31 Discard\n");	
			}
			else if ( g_oI2SMULTIInfor[uCountFunc].m_eDataChannel==  ALLTX)
			{
				UART_Printf("TX0, TX1, TX2\t");
				UART_Printf("TX1 Channel: ");
				if ( g_oI2SMULTIInfor[uCountFunc].m_eChannel1Discard==  NoDiscard)
					UART_Printf("No Discard\n");
				else if ( g_oI2SMULTIInfor[uCountFunc].m_eChannel1Discard==  Discard15to0)
					UART_Printf("0 to 15 Discard\n");
				else if ( g_oI2SMULTIInfor[uCountFunc].m_eChannel1Discard==  Discard31to16)
					UART_Printf("16 to 31 Discard\n");
					
				UART_Printf("TX2 Channel: ");
				if ( g_oI2SMULTIInfor[uCountFunc].m_eChannel2Discard==  NoDiscard)
					UART_Printf("No Discard\n");
				else if ( g_oI2SMULTIInfor[uCountFunc].m_eChannel2Discard==  Discard15to0)
					UART_Printf("0 to 15 Discard\n");
				else if ( g_oI2SMULTIInfor[uCountFunc].m_eChannel2Discard==  Discard31to16)
					UART_Printf("16 to 31 Discard\n");	
			}				
			UART_Printf("\n");
			
		}
				
		UART_Printf("====================== I2SMULTI Function Test =======================================\n\n");
		UART_Printf("Current Control Port : Port %d\n",eSelPort);
		for (uCountFunc=0; (u32)(g_aI2SMULTITestFunc[uCountFunc].desc)!=0; uCountFunc++)
			UART_Printf("%2d: %s\n", uCountFunc, g_aI2SMULTITestFunc[uCountFunc].desc);	
		UART_Printf("%2d: Change Port\n",uCountFunc++);
		UART_Printf("================================================================================\n");			
		UART_Printf("\nSelect the function to test : ");
		iSel =UART_GetIntNum();
		UART_Printf("\n");
		if(iSel == 0) 
			break;
		if (iSel>0 && iSel<(sizeof(g_aI2SMULTITestFunc)/8-1))
			(g_aI2SMULTITestFunc[iSel].func) (eSelPort, I2SMULTI_GetI2SMULTIInfor(eSelPort));		
	}	
	I2SMULTI_ReturnPort(AUDIO_PORT0);
}


void I2SMULTI_Configuration(AUDIO_PORT rhs_ePort, I2SMULTI_Infor * rhs_pInfor)
{
	u32 uCountFunc=0;
	s32 iSel=0;	
	const I2SMULTItestFuncMenu aI2SMULTIConfigFunc[] =
	{
		//I2SMULTI Function Test Item
		I2SMULTI_Sel_OperationMode,		"Operation Mode ",	
		I2SMULTI_Sel_WordLength,		"PCM Word Length(BLC) ",	
		I2SMULTI_Sel_Interface,			"Open/Close Interface ",	
		I2SMULTI_Sel_LRCLKPolarity,		"LRCLK Polarity.",	
		I2SMULTI_Sel_SerialDataFormat,	"Interface Data Format",
		I2SMULTI_Sel_CLKSource,			"I2SMULTI CLK Source",
		I2SMULTI_SamplingFreq,			"Sampling Frequency(LRCLK Freq).",		
		I2SMULTI_Sel_RFS,					"LRCLK Length(RFS)",
		I2SMULTI_Sel_BFS,					"Effective Data Length(BFS)",
		I2SMULTI_Sel_EnableCh,			"Enable Channel",
		I2SMULTI_Sel_Discard1,			"Channel 1 Discard",
		I2SMULTI_Sel_Discard2,			"Channel 2 Discard",
		
	    	0,0
	};
	UART_Printf("\n\n================== I2SMULTI Configuration =====================\n\n");
	while(1)
	{
		for (uCountFunc=0; (u32)(aI2SMULTIConfigFunc[uCountFunc].desc)!=0; uCountFunc++)
			UART_Printf("%2d: %s\n", uCountFunc, aI2SMULTIConfigFunc[uCountFunc].desc);

		UART_Printf("\nSelect the function to test : ");
		iSel =UART_GetIntNum();
		UART_Printf("\n");
		if(iSel == -1)
		{
			I2SMULTIMOD_PutData(rhs_ePort, rhs_pInfor);
			break;
		}
		(aI2SMULTIConfigFunc[iSel].func) (rhs_ePort, rhs_pInfor);
		
	}
}

void I2SMULTI_PCMInPolling(AUDIO_PORT rhs_ePort, I2SMULTI_Infor * rhs_pInfor)
{

	WM8580_CodecInitPCMIn(rhs_pInfor->m_eDataFormat, (u32) rhs_pInfor->m_dSamplingFreq, rhs_pInfor->m_eOpMode,  rhs_pInfor->m_eWordLength, LINEIN, SMDK_I2SMULTI);
	if ((rhs_pInfor->m_CLKSRC != I2SMUL_EXTERNALCDCLK) && (rhs_pInfor->m_CLKSRC != I2SMUL_PCMEXTCLK1) && (rhs_pInfor->m_eOpMode == Master)) 
	{
		I2SMULTI_CDCLKOut(rhs_ePort, rhs_pInfor);										
	}
	else if ((rhs_pInfor->m_CLKSRC == I2SMUL_SYSTEM_FIN) && (rhs_pInfor->m_eOpMode == Slave)) 
	{
		I2SMULTI_CDCLKOut(rhs_ePort, rhs_pInfor);						//12MHz Out
	}
	else if ((rhs_pInfor->m_CLKSRC != I2SMUL_EXTERNALCDCLK) && (rhs_pInfor->m_CLKSRC != I2SMUL_PCMEXTCLK1) && (rhs_pInfor->m_CLKSRC != I2SMUL_SYSTEM_FIN) && (rhs_pInfor->m_eOpMode == Slave)) 
	{
		#if 0
		if ( rhs_pInfor->m_dSamplingFreq % 11025)
			I2SMULTI_SetEpllCDCLKOut(rhs_ePort, rhs_pInfor, I2SMULTI_USERCLKOUT2);		// 11.2896MHz Out
		else
			I2SMULTI_SetEpllCDCLKOut(rhs_ePort, rhs_pInfor, I2SMULTI_USERCLKOUT1);		// 12.288MHz Out
		#else
		WM8580_CodecCLKOut((u32) rhs_pInfor->m_dSamplingFreq);	
		#endif		
	}
	else if (rhs_pInfor->m_CLKSRC == I2SMUL_PCMEXTCLK1) 				// I2S0CLKOUT and PCM1EXCLK IN
	{
		I2S_Init(AUDIO_PORT1);
		I2S_InitPort(AUDIO_PORT1);
		I2S_CDCLKOut(AUDIO_PORT1);	

		PCM_InitPort(AUDIO_PORT0);
		PCM_SelCLKAUDIO(AUDIO_PORT0, PCM_PCMCDCLK);
	}
	I2SMULTI_DataInPolling(rhs_ePort, rhs_pInfor);
}

void I2SMULTI_PCMOutPolling(AUDIO_PORT rhs_ePort, I2SMULTI_Infor * rhs_pInfor)
{
	if ((rhs_pInfor->m_CLKSRC != I2SMUL_EXTERNALCDCLK) && (rhs_pInfor->m_CLKSRC != I2SMUL_PCMEXTCLK1) && (rhs_pInfor->m_eOpMode == Master)) 
	{
		I2SMULTI_CDCLKOut(rhs_ePort, rhs_pInfor);										
	}
	else if ((rhs_pInfor->m_CLKSRC == I2SMUL_SYSTEM_FIN) && (rhs_pInfor->m_eOpMode == Slave)) 
	{
		I2SMULTI_CDCLKOut(rhs_ePort, rhs_pInfor);						//12MHz Out
	}	
	else if (rhs_pInfor->m_CLKSRC == I2SMUL_PCMEXTCLK1) 				// I2S0CLKOUT and PCM1EXCLK IN
	{
		I2S_Init(AUDIO_PORT0);
		I2S_InitPort(AUDIO_PORT0);
		I2S_CDCLKOut(AUDIO_PORT0);	
		PCM_InitPort(AUDIO_PORT1);
		PCM_SelCLKAUDIO(AUDIO_PORT1, PCM_PCMCDCLK);
	}
	
	WM8580_CodecInitPCMOut(rhs_pInfor->m_eDataFormat, (u32) rhs_pInfor->m_dSamplingFreq, rhs_pInfor->m_eOpMode,  rhs_pInfor->m_eWordLength, SMDK_I2SMULTI);
	I2SMULTI_DataOutPolling(rhs_ePort, rhs_pInfor);
}


void I2SMULTI_PCMDataInOutPolling(AUDIO_PORT rhs_ePort, I2SMULTI_Infor * rhs_pInfor)
{
	UART_Printf("Supply Sound to I2SMULTI CODEC via Line In Connector.\n");
	UART_Printf("Press any key to record.\n");
	UART_Getc();
	
	if ((rhs_pInfor->m_CLKSRC != I2SMUL_EXTERNALCDCLK) && (rhs_pInfor->m_CLKSRC != I2SMUL_PCMEXTCLK1) && (rhs_pInfor->m_eOpMode == Master)) 
	{
		I2SMULTI_CDCLKOut(rhs_ePort, rhs_pInfor);										
	}
	else if ((rhs_pInfor->m_CLKSRC == I2SMUL_SYSTEM_FIN) && (rhs_pInfor->m_eOpMode == Slave)) 
	{
		I2SMULTI_CDCLKOut(rhs_ePort, rhs_pInfor);						//12MHz Out
	}
	else if ((rhs_pInfor->m_CLKSRC != I2SMUL_EXTERNALCDCLK) && (rhs_pInfor->m_CLKSRC != I2SMUL_PCMEXTCLK1) && (rhs_pInfor->m_CLKSRC != I2SMUL_SYSTEM_FIN) && (rhs_pInfor->m_eOpMode == Slave)) 
	{
		if ( rhs_pInfor->m_dSamplingFreq % 11025)
			I2SMULTI_SetEpllCDCLKOut(rhs_ePort, rhs_pInfor, I2SMULTI_USERCLKOUT2);		// 11.2896MHz Out
		else
			I2SMULTI_SetEpllCDCLKOut(rhs_ePort, rhs_pInfor, I2SMULTI_USERCLKOUT1);		// 12.288MHz Out
	}
	else if (rhs_pInfor->m_CLKSRC == I2SMUL_PCMEXTCLK1) 				// I2S0CLKOUT and PCM1EXCLK IN
	{
		I2S_Init(AUDIO_PORT0);
		I2S_InitPort(AUDIO_PORT0);
		I2S_CDCLKOut(AUDIO_PORT0);	
		PCM_InitPort(AUDIO_PORT1);
		PCM_SelCLKAUDIO(AUDIO_PORT1, PCM_PCMCDCLK);
	}
	
	WM8580_CodecInitPCMIn(rhs_pInfor->m_eDataFormat, (u32) rhs_pInfor->m_dSamplingFreq, rhs_pInfor->m_eOpMode,  rhs_pInfor->m_eWordLength, LINEIN, SMDK_I2SMULTI);
	I2SMULTI_DataInPolling(rhs_ePort, rhs_pInfor);

	UART_Printf("\nListen to Sound via Speak Out Connector.\n");
	UART_Printf("Press any key to play.\n");
	UART_Getc();

	WM8580_CodecInitPCMOut(rhs_pInfor->m_eDataFormat, (u32) rhs_pInfor->m_dSamplingFreq, rhs_pInfor->m_eOpMode,  rhs_pInfor->m_eWordLength, SMDK_I2SMULTI);
	I2SMULTI_DataOutPolling(rhs_ePort, rhs_pInfor);
	
	
}


void I2SMULTI_PCMInDMA(AUDIO_PORT rhs_ePort, I2SMULTI_Infor * rhs_pInfor)
{
	UART_Printf("Supply Sound to I2SMULTI CODEC via Line In Connector.\n");
	UART_Printf("Press any key to record.\n");
	UART_Getc();
	UART_Printf("Recording...\n");	
	WM8580_CodecInitPCMIn(rhs_pInfor->m_eDataFormat, (u32) rhs_pInfor->m_dSamplingFreq, rhs_pInfor->m_eOpMode,  rhs_pInfor->m_eWordLength, LINEIN, SMDK_I2SMULTI);
	if ((rhs_pInfor->m_CLKSRC != I2SMUL_EXTERNALCDCLK) && (rhs_pInfor->m_CLKSRC != I2SMUL_PCMEXTCLK1) && (rhs_pInfor->m_eOpMode == Master)) 
	{
		I2SMULTI_CDCLKOut(rhs_ePort, rhs_pInfor);										
	}
	else if ((rhs_pInfor->m_CLKSRC == I2SMUL_SYSTEM_FIN) && (rhs_pInfor->m_eOpMode == Slave)) 
	{
		I2SMULTI_CDCLKOut(rhs_ePort, rhs_pInfor);						//12MHz Out
	}
	else if ((rhs_pInfor->m_CLKSRC != I2SMUL_EXTERNALCDCLK) && (rhs_pInfor->m_CLKSRC != I2SMUL_PCMEXTCLK1) && (rhs_pInfor->m_CLKSRC != I2SMUL_SYSTEM_FIN) && (rhs_pInfor->m_eOpMode == Slave)) 
	{
		#if 0
		if ( rhs_pInfor->m_dSamplingFreq % 11025)
			I2SMULTI_SetEpllCDCLKOut(rhs_ePort, rhs_pInfor, I2SMULTI_USERCLKOUT2);		// 11.2896MHz Out
		else
			I2SMULTI_SetEpllCDCLKOut(rhs_ePort, rhs_pInfor, I2SMULTI_USERCLKOUT1);		// 12.288MHz Out
		#else
		WM8580_CodecCLKOut((u32) rhs_pInfor->m_dSamplingFreq);	
		#endif		
	}	
	else if (rhs_pInfor->m_CLKSRC == I2SMUL_PCMEXTCLK1) 				// I2S0CLKOUT and PCM1EXCLK IN
	{
		I2S_Init(AUDIO_PORT0);
		I2S_InitPort(AUDIO_PORT0);
		I2S_CDCLKOut(AUDIO_PORT0);	
		PCM_InitPort(AUDIO_PORT1);	
	}	
	I2SMULTI_DataInDMAMode(rhs_ePort, rhs_pInfor);
	if ( rhs_pInfor->m_eWordLength == Word24)
		Parser_32to24(rhs_pInfor->m_uRecBufferStartAddr, rhs_pInfor->m_uPcmDataSize, (u32 *)0x53400000);	
}
void I2SMULTI_PCMOutDMA(AUDIO_PORT rhs_ePort, I2SMULTI_Infor * rhs_pInfor)
{
	UART_Printf("\nPlay Wave File.\n");

	#if DATAFROMNAND
	// MODEM bootloader copy to DPSRAM
	NAND_Inform[0].uNandType = NAND_Normal8bit;
	NAND_Inform[0].uAddrCycle = 4;
	NAND_Inform[0].uBlockNum = 4096;
	NAND_Inform[0].uPageNum = 32;
	NAND_Inform[0].uPageSize = NAND_PAGE_512;
	NAND_Inform[0].uSpareSize = NAND_SPARE_16;
	NAND_Inform[0].uECCtest = 0;
	NAND_Inform[0].uTacls =30;
	NAND_Inform[0].uTwrph0 = 35+30;	//Pad delay : about 10ns
	NAND_Inform[0].uTwrph1 = 15+35;	//tWH : 15ns
	NAND_Inform[0].uSpareECC =0;
	NAND_Inform[0].uSpareECCtest  =0;
	
	NAND_Init(0);
	NAND_ReadMultiPage(0, 1000, 0, (u8 *)0x52500000, 31690392);
	#endif
	
	if ((rhs_pInfor->m_CLKSRC != I2SMUL_EXTERNALCDCLK) && (rhs_pInfor->m_CLKSRC != I2SMUL_PCMEXTCLK1) && (rhs_pInfor->m_eOpMode == Master)) 
	{
		I2SMULTI_CDCLKOut(rhs_ePort, rhs_pInfor);										
	}
	else if ((rhs_pInfor->m_CLKSRC == I2SMUL_SYSTEM_FIN) && (rhs_pInfor->m_eOpMode == Slave)) 
	{
		I2SMULTI_CDCLKOut(rhs_ePort, rhs_pInfor);						//12MHz Out
	}
	else if ((rhs_pInfor->m_CLKSRC != I2SMUL_EXTERNALCDCLK) && (rhs_pInfor->m_CLKSRC != I2SMUL_PCMEXTCLK1) && (rhs_pInfor->m_CLKSRC != I2SMUL_SYSTEM_FIN) && (rhs_pInfor->m_eOpMode == Slave)) 
	{
		#if 0
		if ( rhs_pInfor->m_dSamplingFreq % 11025)
			I2SMULTI_SetEpllCDCLKOut(rhs_ePort, rhs_pInfor, I2SMULTI_USERCLKOUT2);		// 11.2896MHz Out
		else
			I2SMULTI_SetEpllCDCLKOut(rhs_ePort, rhs_pInfor, I2SMULTI_USERCLKOUT1);		// 12.288MHz Out
		#else
		WM8580_CodecCLKOut((u32) rhs_pInfor->m_dSamplingFreq);	
		#endif
	}	
	else if (rhs_pInfor->m_CLKSRC == I2SMUL_PCMEXTCLK1) 				// I2S0CLKOUT and PCM1EXCLK IN
	{
		I2S_Init(AUDIO_PORT0);
		I2S_InitPort(AUDIO_PORT0);
		I2S_CDCLKOut(AUDIO_PORT0);	
		PCM_InitPort(AUDIO_PORT1);	
	}
	WM8580_CodecInitPCMOut(rhs_pInfor->m_eDataFormat, (u32) rhs_pInfor->m_dSamplingFreq, rhs_pInfor->m_eOpMode,  rhs_pInfor->m_eWordLength, SMDK_I2SMULTI);
	I2SMULTI_DataOutDMA(rhs_ePort, rhs_pInfor);
	I2SMULTI_DataOutDMAStop(rhs_ePort, rhs_pInfor);
}
void I2SMULTI_PCMDataInOutDMA(AUDIO_PORT rhs_ePort, I2SMULTI_Infor * rhs_pInfor)
{
	u32 uCnt;
	for (uCnt= 0; uCnt < 2;uCnt++)
	{
		if ((rhs_pInfor->m_CLKSRC != I2SMUL_EXTERNALCDCLK) && (rhs_pInfor->m_CLKSRC != I2SMUL_PCMEXTCLK1) && (rhs_pInfor->m_eOpMode == Master)) 
		{
			I2SMULTI_CDCLKOut(rhs_ePort, rhs_pInfor);										
		}
		else if ((rhs_pInfor->m_CLKSRC == I2SMUL_SYSTEM_FIN) && (rhs_pInfor->m_eOpMode == Slave)) 
		{
			I2SMULTI_CDCLKOut(rhs_ePort, rhs_pInfor);						//12MHz Out
		}
		else if ((rhs_pInfor->m_CLKSRC != I2SMUL_EXTERNALCDCLK) && (rhs_pInfor->m_CLKSRC != I2SMUL_PCMEXTCLK1) && (rhs_pInfor->m_CLKSRC != I2SMUL_SYSTEM_FIN) && (rhs_pInfor->m_eOpMode == Slave)) 
		{
			#if 0
			if ( rhs_pInfor->m_dSamplingFreq % 11025)
				I2SMULTI_SetEpllCDCLKOut(rhs_ePort, rhs_pInfor, I2SMULTI_USERCLKOUT2);		// 11.2896MHz Out
			else
				I2SMULTI_SetEpllCDCLKOut(rhs_ePort, rhs_pInfor, I2SMULTI_USERCLKOUT1);		// 12.288MHz Out
			#else
			WM8580_CodecCLKOut((u32) rhs_pInfor->m_dSamplingFreq);	
			#endif		
		}	
		else if (rhs_pInfor->m_CLKSRC == I2SMUL_PCMEXTCLK1) 				// I2S0CLKOUT and PCM1EXCLK IN
		{
			I2S_Init(AUDIO_PORT0);
			I2S_InitPort(AUDIO_PORT0);
			I2S_CDCLKOut(AUDIO_PORT0);	
			PCM_InitPort(AUDIO_PORT1);
			PCM_SelCLKAUDIO(AUDIO_PORT1, PCM_PCMCDCLK);
		}
			if ( uCnt == 0 )
		{
			UART_Printf("Supply Sound to I2SMULTI CODEC via Line In Connector.\n");
			UART_Printf("Press any key to record.\n");
			UART_Getc();
			UART_Printf("Recording...\n");			
			WM8580_CodecInitPCMIn(rhs_pInfor->m_eDataFormat, (u32) rhs_pInfor->m_dSamplingFreq, rhs_pInfor->m_eOpMode,  rhs_pInfor->m_eWordLength, LINEIN, SMDK_I2SMULTI);
			I2SMULTI_DataInDMAMode(rhs_ePort, rhs_pInfor);
			I2SMULTI_DataInDMAStop(rhs_ePort, rhs_pInfor);
		}
		else
		{
			UART_Printf("\nListen to Sound via Speak Out Connector.\n");
			UART_Printf("Press any key to play.\n");
			UART_Getc();			
			WM8580_CodecInitPCMOut(rhs_pInfor->m_eDataFormat, (u32) rhs_pInfor->m_dSamplingFreq, rhs_pInfor->m_eOpMode,  rhs_pInfor->m_eWordLength, SMDK_I2SMULTI);
			I2SMULTI_DataOutDMA(rhs_ePort, rhs_pInfor);
			I2SMULTI_DataOutDMAStop(rhs_ePort, rhs_pInfor);
		}
	}	
}

void I2SMULTI_DataOutIntrMode(AUDIO_PORT rhs_ePort, I2SMULTI_Infor * rhs_pInfor)
{
	UART_Printf("\nPlay Wave File.\n");

	if ((rhs_pInfor->m_CLKSRC != I2SMUL_EXTERNALCDCLK) && (rhs_pInfor->m_CLKSRC != I2SMUL_PCMEXTCLK1) && (rhs_pInfor->m_eOpMode == Master)) 
	{
		I2SMULTI_CDCLKOut(rhs_ePort, rhs_pInfor);										
	}
	else if ((rhs_pInfor->m_CLKSRC == I2SMUL_SYSTEM_FIN) && (rhs_pInfor->m_eOpMode == Slave)) 
	{
		I2SMULTI_CDCLKOut(rhs_ePort, rhs_pInfor);						//12MHz Out
	}
	else if ((rhs_pInfor->m_CLKSRC != I2SMUL_EXTERNALCDCLK) && (rhs_pInfor->m_CLKSRC != I2SMUL_PCMEXTCLK1) && (rhs_pInfor->m_CLKSRC != I2SMUL_SYSTEM_FIN) && (rhs_pInfor->m_eOpMode == Slave)) 
	{
		if ( rhs_pInfor->m_dSamplingFreq % 11025)
			I2SMULTI_SetEpllCDCLKOut(rhs_ePort, rhs_pInfor, I2SMULTI_USERCLKOUT2);		// 11.2896MHz Out
		else
			I2SMULTI_SetEpllCDCLKOut(rhs_ePort, rhs_pInfor, I2SMULTI_USERCLKOUT1);		// 12.288MHz Out
	}	
	else if (rhs_pInfor->m_CLKSRC == I2SMUL_PCMEXTCLK1) 				// I2S0CLKOUT and PCM1EXCLK IN
	{
		I2S_Init(AUDIO_PORT0);
		I2S_InitPort(AUDIO_PORT0);
		I2S_CDCLKOut(AUDIO_PORT0);	
		PCM_InitPort(AUDIO_PORT1);
		PCM_SelCLKAUDIO(AUDIO_PORT1, PCM_PCMCDCLK);
	}
	WM8580_CodecInitPCMOut(rhs_pInfor->m_eDataFormat, (u32) rhs_pInfor->m_dSamplingFreq, rhs_pInfor->m_eOpMode,  rhs_pInfor->m_eWordLength, SMDK_I2SMULTI);
	I2SMULTI_DataOutIntr(rhs_ePort, rhs_pInfor);
}

void I2SMULTI_DataParser24to32(AUDIO_PORT rhs_ePort, I2SMULTI_Infor * rhs_pInfor)
{	
	u32 uStartAddr, uSize, uEndAddr;

	UART_Printf("\n Enter the Start Address of Source File\n");
	uStartAddr = UART_GetIntNum();

	UART_Printf("\n Enter the Size of Source File\n");
	uSize = UART_GetIntNum();

	UART_Printf("\n Enter the TartetAddress of Source File\n");
	uEndAddr = UART_GetIntNum();
	
	Parser_24to32((u32 *)uStartAddr, uSize, (u32 *)uEndAddr);
}


void I2SMULTI_DataParserWaveTo32Fifo(AUDIO_PORT rhs_ePort, I2SMULTI_Infor * rhs_pInfor)
{
	u32 uStartAddr;
	u8  *uTotch, *uBitperch;
	u32 *uSize, *p_uWaveStartAddr, *uSampleRate;

	UART_Printf("\n Enter the Start Address of WAVE File(Default : 0x51000000)\n");
	uStartAddr = UART_GetIntNum();

	if ( uStartAddr <= 0x50000000 || uStartAddr >= 0x54000000 )
		uStartAddr = 0x51000000;	
	
	p_uWaveStartAddr = Wave_Parser(uStartAddr, uTotch, uSampleRate, uBitperch, uSize);

	UART_Printf("\nStart Address : 0x%x\n",uStartAddr);
	UART_Printf("End Address : 0x%x\n",*p_uWaveStartAddr);
	UART_Printf("Totch : %d\n",*uTotch);
	UART_Printf("SampleRate : %d\n",*uSampleRate);
	UART_Printf("Bitperch : %d\n",*uBitperch);
	UART_Printf("Size : %d\n",*uSize);	

}
void I2SMULTI_Decompress32to24(AUDIO_PORT rhs_ePort, I2SMULTI_Infor * rhs_pInfor)
{
	u32 uStartAddr,uSize, uEndAddr;

	UART_Printf("\n Enter the Start Address of WAVE File\n");
	uStartAddr = UART_GetIntNum();

	UART_Printf("\n Enter the Size of Source File\n");
	uSize = UART_GetIntNum();

	UART_Printf("\n Enter the TartetAddress of Source File\n");
	uEndAddr = UART_GetIntNum();

	Decomp32to24((u32 *)uStartAddr, uSize, (u32 *)uEndAddr);
}

void I2SMULTI_LoopBack(AUDIO_PORT rhs_ePort, I2SMULTI_Infor * rhs_pInfor)
{
	I2S_Infor TempInfor0 = g_oI2SInfor[AUDIO_PORT0], TempInfor1 = g_oI2SInfor[AUDIO_PORT1];
	I2S_Infor *pTempInfor;
	u32 *pTXDataAddr, *pRXDataAddr[3];
	s32 uTransferLen = rhs_pInfor->m_uPcmDataSize;
	u32 uCnt;
	u32 uCMP[6];
	
	u32 uPcmDataSize;
	u32 uRegValue;
	
	g_TestMode = I2S_DUAL;
	I2S_Init(AUDIO_PORT0);
	I2S_InitPort(AUDIO_PORT0);
	
	I2S_Init(AUDIO_PORT1);
	I2S_InitPort(AUDIO_PORT1);
	while(1)
	{
		UART_Printf("Data From Master Port to Slave Port\n");
		UART_Printf("Press Any key to start\n");
		UART_Getc();
		
		I2SMULTI_CDCLKOut(rhs_ePort, rhs_pInfor);				
		
		pTempInfor = &(g_oI2SInfor[AUDIO_PORT0]);
		for (uCnt = 0; uCnt < 2; uCnt++)
		{
			pTempInfor->m_eOpMode 	= Slave;
			pTempInfor->m_eWordLength	= rhs_pInfor->m_eWordLength;
			pTempInfor->m_eDataFormat	= rhs_pInfor->m_eDataFormat;
			pTempInfor->m_eLRCLKPolarity	= rhs_pInfor->m_eLRCLKPolarity;
			pTempInfor->m_eRFS 			= rhs_pInfor->m_eRFS;
			pTempInfor->m_eBFS			= rhs_pInfor->m_eBFS;
			pRXDataAddr[uCnt] 			= pTempInfor->m_uRecBufferStartAddr;
			
			I2SMOD_PutData((AUDIO_PORT) uCnt, pTempInfor);
			I2SMOD_SetTXR((AUDIO_PORT) uCnt, RXOnly);
			//I2SMOD_SetTXR((AUDIO_PORT) uCnt, RXOnly);
			//DMA Start
						
			g_uI2SRecDone[(AUDIO_PORT) uCnt]= 0;
			pTempInfor = &(g_oI2SInfor[AUDIO_PORT1]);
		}

		I2SFIC_AutoFlush(AUDIO_PORT0, TXRXBoth);						//FIFO Flush
		I2SFIC_AutoFlush(AUDIO_PORT1, TXRXBoth);						//FIFO Flush
		I2SMULTIFIC_AutoFlush(rhs_ePort, TXRXBoth);					//FIFO Flush	
		
		//pRXDataAddr[2] = (u32 * )malloc(0x8000);
		//uPcmDataSize = 0x8000;
		I2SMULTI_DataOutDMA(rhs_ePort, rhs_pInfor);
		
		I2S_DataInDMAStart(AUDIO_PORT0);
		I2S_DataInDMAStart(AUDIO_PORT1);
		
		while((g_uI2SRecDone[AUDIO_PORT0] == 0) && (g_uI2SRecDone[AUDIO_PORT1] == 0))
		{
			#if 0
			while( uPcmDataSize > 0 )
			{
				do 
				{
					uRegValue = I2SMULTI_Inp32(rI2SMULTIFIC);
				} while ((uRegValue & (I2SMULTI_Status_RX_FIFO_CNT)) < 8);		// Rx FIFO Level check
				
				while(1)
				{
					*(pRXDataAddr[2])++ = I2SMULTI_Inp32(rI2SMULTIRXD);			//Data Transfer
					uPcmDataSize -= 4;
					uRegValue = I2SMULTI_Inp32(rI2SMULTICON);
					if (uRegValue & (I2SMULTI_Status_RX_FIFO_EMPTY) )
						break;
				}
			}	
			#else
			Delay(3000);
			UART_Printf(".");
			#endif
		}		 
		 
		I2S_DataInDMAStop(AUDIO_PORT0);	
		I2S_DataInDMAStop(AUDIO_PORT1);
		I2SMULTI_DataOutDMAStop(rhs_ePort, rhs_pInfor);
		
		I2SFIC_AutoFlush(AUDIO_PORT0, TXRXBoth);
		I2SFIC_AutoFlush(AUDIO_PORT1, TXRXBoth);
		I2SMULTIFIC_AutoFlush(rhs_ePort,  TXRXBoth);		//FIFO Flush	
		// Data Test 
		pTXDataAddr = rhs_pInfor->m_uPlayBufferStartAddr;		
		uTransferLen = rhs_pInfor->m_uPcmDataSize;
		
		while(uTransferLen != 0)
		{
			#if 0
			uCMP[0] = *pTXDataAddr++ - *pRXDataAddr[0]++; 
			uCMP[1] = *pTXDataAddr++ - *pRXDataAddr[0]++ ;
			uCMP[2] = *pTXDataAddr++ - *pRXDataAddr[1]++; 
			uCMP[3] = *pTXDataAddr++ - *pRXDataAddr[1]++; 
			uCMP[4] = *pTXDataAddr++ ; 
			uCMP[5] = *pTXDataAddr++ ; 
			if ( !uCMP[0] && !uCMP[1] && !uCMP[2] && !uCMP[3]);
			{
				UART_Printf("Error!!\n");
				UART_Printf("Address : 0x%x\n", pTXDataAddr);
			}
			//else if ( *pTXDataAddr++ == *pRXDataAddr[2]++ );
			//else if ( *pTXDataAddr++ == *pRXDataAddr[2]++ );
			#endif
			uTransferLen -= 4 * 6;
		}
		//free(pRXDataAddr[2]);
		UART_Printf("End LoopBack Test Mode?\n");
		UART_Printf("0. No		1. Yes\n");
		if (UART_GetIntNum() == 1)
			break;
	}
	I2S_ReturnPort(AUDIO_PORT0);
	I2S_ReturnPort(AUDIO_PORT1);
	g_oI2SInfor[AUDIO_PORT0] = TempInfor0;
	g_oI2SInfor[AUDIO_PORT1] = TempInfor1;
}




