/**************************************************************************************
* 
*	Project Name : S3C6400 Validation
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
*	File Name : i2s_test.c
*  
*	File Description : This file implements i2s test functions.
*
*	Author : Sunil,Roe
*	Dept. : AP Development Team
*	Created Date : 2006/12/26
*	Version : 0.2 
* 
*	History
*	- 0. Created(Sunil,Roe 2006/12/26)
*	- 1. Revision for S3C6410 (Sung-Hyun, Na 2008/02/15)
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
#include "i2s.h"

#include "nand.h"





/*-----------------------------------Test Functions ------------------------------------*/
AUDIO_PORT I2S_ChangePort(void);
void I2S_Configuration(AUDIO_PORT rhs_ePort);
void I2S_LoopBackTest(AUDIO_PORT rhs_ePort);
void I2S_LoopBackAgigTest(void);

void I2S_PCMInPolling(AUDIO_PORT rhs_ePort);
void I2S_PCMOutPolling(AUDIO_PORT rhs_ePort);
void I2S_PCMDataInOutPolling(AUDIO_PORT rhs_ePort);
void I2S_PCMDataBypassPolling(AUDIO_PORT rhs_ePort);

void I2S_PCMInDMA(AUDIO_PORT);
void I2S_PCMOutDMA(AUDIO_PORT);
void I2S_PCMDataInOutDMA(AUDIO_PORT);
void I2S_PCMDataBypassDMA(AUDIO_PORT rhs_ePort);

void I2S_PCMDataInIntr(AUDIO_PORT rhs_ePort);
void I2S_PCMDataOutIntr(AUDIO_PORT rhs_ePort);
void I2S_PCMDataInOutIntr(AUDIO_PORT rhs_ePort);


void I2S_AutoBFSRFSBLCTest(AUDIO_PORT rhs_ePort);


extern void Test_CLKOUT(void) ;
extern void SYSCT_Clock(void);
const I2StestFuncMenu g_aI2STestFunc[] =
{
	//I2S Function Test Item
	0,									"Exit\n",
	I2S_Configuration,					"I2S Configuration\n",	
	
	I2S_PCMInPolling,					"Polling Mode Data In . ",
	I2S_PCMOutPolling,				"Polling Mode Data Out.",
	I2S_PCMDataInOutPolling,			"Polling Mode Data In/Data Out.",
	I2S_PCMDataBypassPolling,		"Polling Mode Bypass\n",	
	
	I2S_PCMInDMA,					"DMA Mode Data In.",	
	I2S_PCMOutDMA,					"DMA Mode Data Out.",
	I2S_PCMDataInOutDMA,			"DMA Mode Data In/Out",
	I2S_PCMDataBypassDMA,			"DMA Mode Bypass\n",	

	I2S_PCMDataInIntr,				"Interrupt Mode(RX Over Run) Data In.",	
	I2S_PCMDataOutIntr,				"Interrupt Mode(TX Under Run) Data Out.",
	I2S_PCMDataInOutIntr,				"Interrupt Mode Data In/Out",

	I2S_LoopBackTest,					"LoopBack Mode. ",
	I2S_LoopBackAgigTest,				"LoopBack Aging Test\n",
	
//	Test_CLKOUT,						"Test CLKOUT",
//	SYSCT_Clock,						"Test SysClock\n",
	I2S_AutoBFSRFSBLCTest,			"Auto Test For BFS, RFS and word length",	
    	0,0
};
// global variables

I2S_Infor		g_oI2SInfor[AUDIO_NUM];

extern NAND_oInform NAND_Inform[NAND_CONNUM];
extern volatile	 u32		g_uI2SRecDone[I2S_CONNUM];
extern volatile u32		g_uPlayDone[I2S_CONNUM];
I2S_TestMode 		g_TestMode = I2S_SINGLE;

extern volatile u32		g_iSeed, g_iNum, g_iARMCLK_DIVN_R, g_iHCLKx2_DIVN_R, g_iPCLK_DIVN_R;
void I2S_Test(void)
{
	u32 uCountFunc=0;
	s32 iSel=0;
	double dTemp;
	AUDIO_PORT eSelPort;
	u32 uLoopBackMode = 0;

	//TestSFR();
	eSelPort = AUDIO_PORT0;

	I2S_Init(AUDIO_PORT0);
	I2S_InitPort(AUDIO_PORT0);
	
	I2S_Init(AUDIO_PORT1);
	I2S_InitPort(AUDIO_PORT1);

	while(1)
	{	
		UART_Printf("================================================================================\n");
		if ( uLoopBackMode == 1) 
				UART_Printf("Loop Back Mode");
		for (uCountFunc = 0; uCountFunc < I2S_CONNUM; uCountFunc++)
		{
			
			UART_Printf("Port : %d\t",uCountFunc);			
			
			UART_Printf("Operation Mode : ");
			if ( g_oI2SInfor[uCountFunc].m_eOpMode ==  Master)
				UART_Printf("Master\t");
			else 
				UART_Printf("Slave\t");
			
			UART_Printf("PCM Word Length : ");
			if ( g_oI2SInfor[uCountFunc].m_eWordLength ==  Word8)
				UART_Printf("8bit\n");
			else if ( g_oI2SInfor[uCountFunc].m_eWordLength ==  Word16)
				UART_Printf("16bit\n");
			else 
				UART_Printf("24bit\n");

			UART_Printf("Interface Data Format : ");
			if ( g_oI2SInfor[uCountFunc].m_eDataFormat==  I2SFormat)
				UART_Printf("I2S Format\t");
			else if ( g_oI2SInfor[uCountFunc].m_eDataFormat==  MSBJustified)
				UART_Printf("MSBJustified(Left)\t");
			else if ( g_oI2SInfor[uCountFunc].m_eDataFormat==  LSBJustified)
				UART_Printf("LSBJustified(Right)\t");

			
			UART_Printf("LRCLK Polarity : ");
			if ( g_oI2SInfor[uCountFunc].m_eLRCLKPolarity==  LeftHigh)
				UART_Printf("Left High\n");
			else if ( g_oI2SInfor[uCountFunc].m_eLRCLKPolarity==  RightHigh)
				UART_Printf("Right High\n");	

			UART_Printf("I2S Clock Source : ");
			if ( g_oI2SInfor[uCountFunc].m_CLKSRC==  I2S_MOUT_EPLL)
				UART_Printf("EPLL Out\t");
			else if ( g_oI2SInfor[uCountFunc].m_CLKSRC==  I2S_PCLK)
				UART_Printf("PCLK\t");
			else if ( g_oI2SInfor[uCountFunc].m_CLKSRC==  I2S_DOUT_MPLL)
				UART_Printf("MPLL Out\t");
			else if ( g_oI2SInfor[uCountFunc].m_CLKSRC==  I2S_FIN)
				UART_Printf("System External CLK\t");
			else if ( g_oI2SInfor[uCountFunc].m_CLKSRC==  I2S_EXTERNALCDCLK)
				UART_Printf("I2S External CLK\t");
			dTemp = (double)(g_oI2SInfor[uCountFunc].m_dSamplingFreq)/1000;
			UART_Printf("Sampling Frequency : %3.1fKHz\n",dTemp);
			UART_Printf("RFS : ");
			if ( g_oI2SInfor[uCountFunc].m_eRFS==  RFS_256fs)
				UART_Printf("256fs\t");
			else if ( g_oI2SInfor[uCountFunc].m_eRFS==  RFS_512fs)
				UART_Printf("512fs\t");
			else if ( g_oI2SInfor[uCountFunc].m_eRFS==  RFS_384fs)
				UART_Printf("384fs\t");
			else if ( g_oI2SInfor[uCountFunc].m_eRFS==  RFS_768fs)
				UART_Printf("768fs\t");	

			UART_Printf("BFS : ");
			if ( g_oI2SInfor[uCountFunc].m_eBFS==  BFS_32fs)
				UART_Printf("32fs\t");
			else if ( g_oI2SInfor[uCountFunc].m_eBFS==  BFS_48fs)
				UART_Printf("48fs\t");
			else if ( g_oI2SInfor[uCountFunc].m_eBFS==  BFS_16fs)
				UART_Printf("16fs\t");
			else if ( g_oI2SInfor[uCountFunc].m_eBFS==  BFS_24fs)
				UART_Printf("24fs\t");
			UART_Printf("\n\n");
			
		}
				
		UART_Printf("====================== I2S Function Test =======================================\n\n");
		UART_Printf("Current Control Port : Port %d\n",eSelPort);
		for (uCountFunc=0; (u32)(g_aI2STestFunc[uCountFunc].desc)!=0; uCountFunc++)
			UART_Printf("%2d: %s\n", uCountFunc, g_aI2STestFunc[uCountFunc].desc);	
		UART_Printf("\n%2d: Change Port\n",uCountFunc++);		
		UART_Printf("================================================================================\n");			
		UART_Printf("\nSelect the function to test : ");
		
		iSel =UART_GetIntNum();		
		UART_Printf("\n");
		
		if(iSel == 0) 
			break;
		if (iSel>0 && iSel<(sizeof(g_aI2STestFunc)/8-1))
			(g_aI2STestFunc[iSel].func) (eSelPort);
		else if ( iSel == (sizeof(g_aI2STestFunc)/8-1))			
			eSelPort = I2S_ChangePort();	
	}	
	I2S_ReturnPort(AUDIO_PORT0);
	I2S_ReturnPort(AUDIO_PORT1);	
}


AUDIO_PORT I2S_ChangePort(void)
{	
	AUDIO_PORT eSelPort;
	
	UART_Printf("Which Port do you controll?\n");
	UART_Printf("0. Port 0	1. Port 1 \n");
	eSelPort = (AUDIO_PORT)UART_GetIntNum();
	if (eSelPort == 1)
		return AUDIO_PORT1;
	else
		return AUDIO_PORT0;	
}



void I2S_Configuration(AUDIO_PORT rhs_ePort)
{
	u32 uCountFunc=0;
	s32 iSel=0;	
	const I2StestFuncMenu aI2SConfigFunc[] =
	{
		//I2S Function Test Item
		I2S_SelOperationMode,		"Operation Mode ",	
		I2SMOD_ChangeWordLength,	"PCM Word Length(BLC) ",	
		I2SMOD_ChangeInterface,		"Open/Close Interface ",	
		I2S_Sel_LRCLKPolarity,			"LRCLK Polarity.",	
		I2S_Sel_SerialDataFormat,		"Interface Data Format",
		I2S_Sel_CLKSource,			"I2S CLK Source",
		I2S_SamplingFreq,				"Sampling Frequency(LRCLK Freq).",		
		I2S_Sel_RFS,					"LRCLK Length(RFS)",
		I2S_Sel_BFS,					"Effective Data Length(BFS)",		
	    	0,0
	};
	UART_Printf("\n\n================== I2S Configuration =====================\n\n");
	while(1)
	{
		for (uCountFunc=0; (u32)(aI2SConfigFunc[uCountFunc].desc)!=0; uCountFunc++)
			UART_Printf("%2d: %s\n", uCountFunc, aI2SConfigFunc[uCountFunc].desc);

		UART_Printf("\nSelect the function to test : ");
		iSel =UART_GetIntNum();
		UART_Printf("\n");
		if(iSel == -1)
		{
			I2SMOD_PutData(rhs_ePort, I2S_GetI2SInfor(rhs_ePort));
			break;
		}
		(aI2SConfigFunc[iSel].func) (rhs_ePort);
		
	}
}

void I2S_PCMInPolling(AUDIO_PORT rhs_ePort)
{
	if ((g_oI2SInfor[rhs_ePort].m_CLKSRC != I2S_EXTERNALCDCLK) && (g_oI2SInfor[rhs_ePort].m_eOpMode == Master)) 
	{
		I2S_CDCLKOut(rhs_ePort);											
	}
	else if ((g_oI2SInfor[rhs_ePort].m_CLKSRC == I2S_FIN) && (g_oI2SInfor[rhs_ePort].m_eOpMode == Slave)) 
	{
		I2S_CDCLKOut(rhs_ePort);										//12MHz Out
	}
	else if ((g_oI2SInfor[rhs_ePort].m_CLKSRC != I2S_EXTERNALCDCLK) && (g_oI2SInfor[rhs_ePort].m_CLKSRC != I2S_FIN) && (g_oI2SInfor[rhs_ePort].m_eOpMode == Slave)) 
	{
		if ( g_oI2SInfor[rhs_ePort].m_dSamplingFreq % 11025)
			I2S_SetEpllCDCLKOut(rhs_ePort, I2S_USERCLKOUT2);					// 11.2896MHz Out
		else
			I2S_SetEpllCDCLKOut(rhs_ePort, I2S_USERCLKOUT1);					// 12.288MHz Out
	}
	
	#if (I2S_CODEC == WM8753)
	WM8753_CodecInitPCMIn(g_oI2SInfor[rhs_ePort].m_eDataFormat, (u32) g_oI2SInfor[rhs_ePort].m_dSamplingFreq, 
						g_oI2SInfor[rhs_ePort].m_eOpMode,  g_oI2SInfor[rhs_ePort].m_eWordLength, LINEIN, SMDK_I2S);
	#elif (I2S_CODEC == WM8990)
	WM8990_CodecInitPCMIn(g_oI2SInfor[rhs_ePort].m_eDataFormat, (u32) g_oI2SInfor[rhs_ePort].m_dSamplingFreq, 
						g_oI2SInfor[rhs_ePort].m_eOpMode,  g_oI2SInfor[rhs_ePort].m_eWordLength, LINEIN, SMDK_I2S);
	#elif (I2S_CODEC == WM8580)
	WM8580_CodecInitPCMIn(g_oI2SInfor[rhs_ePort].m_eDataFormat, (u32) g_oI2SInfor[rhs_ePort].m_dSamplingFreq, g_oI2SInfor[rhs_ePort].m_eOpMode,  g_oI2SInfor[rhs_ePort].m_eWordLength, LINEIN, SMDK_I2S);
	#endif
	
	I2SMOD_SetTXR(rhs_ePort, RXOnly);
	I2S_DataInPolling(rhs_ePort);
}

void I2S_PCMOutPolling(AUDIO_PORT rhs_ePort)
{
	if ((g_oI2SInfor[rhs_ePort].m_CLKSRC != I2S_EXTERNALCDCLK) && (g_oI2SInfor[rhs_ePort].m_eOpMode == Master)) 
	{
		I2S_CDCLKOut(rhs_ePort);											
	}
	else if ((g_oI2SInfor[rhs_ePort].m_CLKSRC == I2S_FIN) && (g_oI2SInfor[rhs_ePort].m_eOpMode == Slave)) 
	{
		I2S_CDCLKOut(rhs_ePort);										//12MHz Out
	}
	else if ((g_oI2SInfor[rhs_ePort].m_CLKSRC != I2S_EXTERNALCDCLK) && (g_oI2SInfor[rhs_ePort].m_CLKSRC != I2S_FIN) && (g_oI2SInfor[rhs_ePort].m_eOpMode == Slave)) 
	{
		if ( g_oI2SInfor[rhs_ePort].m_dSamplingFreq % 11025)
			I2S_SetEpllCDCLKOut(rhs_ePort, I2S_USERCLKOUT2);					// 11.2896MHz Out
		else
			I2S_SetEpllCDCLKOut(rhs_ePort, I2S_USERCLKOUT1);					// 12.288MHz Out
	}
	
	#if (I2S_CODEC == WM8753)
		WM8753_CodecInitPCMOut(g_oI2SInfor[rhs_ePort].m_eDataFormat, (u32) g_oI2SInfor[rhs_ePort].m_dSamplingFreq, 
								g_oI2SInfor[rhs_ePort].m_eOpMode,  g_oI2SInfor[rhs_ePort].m_eWordLength, SMDK_I2S);
	#elif (I2S_CODEC == WM8990)
		WM8990_CodecInitPCMOut(g_oI2SInfor[rhs_ePort].m_eDataFormat, (u32) g_oI2SInfor[rhs_ePort].m_dSamplingFreq, 
								g_oI2SInfor[rhs_ePort].m_eOpMode,  g_oI2SInfor[rhs_ePort].m_eWordLength, SMDK_I2S);
	#elif (I2S_CODEC == WM8580)
	WM8580_CodecInitPCMOut(g_oI2SInfor[rhs_ePort].m_eDataFormat, (u32) g_oI2SInfor[rhs_ePort].m_dSamplingFreq, 
								g_oI2SInfor[rhs_ePort].m_eOpMode,  g_oI2SInfor[rhs_ePort].m_eWordLength, SMDK_I2S);
	#endif	
	I2SMOD_SetTXR(rhs_ePort, TXOnly);
	I2S_DataOutPolling(rhs_ePort);
}

void I2S_PCMInDMA(AUDIO_PORT rhs_ePort)
{
	UART_Printf("Supply Sound to I2S CODEC via Line In Connector.\n");
	UART_Printf("Press any key to record.\n");
	UART_Getc();
	UART_Printf("Recording...\n");
	
	if ((g_oI2SInfor[rhs_ePort].m_CLKSRC != I2S_EXTERNALCDCLK) && (g_oI2SInfor[rhs_ePort].m_eOpMode == Master)) 
	{
		I2S_CDCLKOut(rhs_ePort);											
	}
	else if ((g_oI2SInfor[rhs_ePort].m_CLKSRC == I2S_FIN) && (g_oI2SInfor[rhs_ePort].m_eOpMode == Slave)) 
	{
		I2S_CDCLKOut(rhs_ePort);										//12MHz Out
	}
	else if ((g_oI2SInfor[rhs_ePort].m_CLKSRC != I2S_EXTERNALCDCLK) && (g_oI2SInfor[rhs_ePort].m_CLKSRC != I2S_FIN) && (g_oI2SInfor[rhs_ePort].m_eOpMode == Slave)) 
	{
		if ( g_oI2SInfor[rhs_ePort].m_dSamplingFreq % 11025)
			I2S_SetEpllCDCLKOut(rhs_ePort, I2S_USERCLKOUT2);					// 11.2896MHz Out
		else
			I2S_SetEpllCDCLKOut(rhs_ePort, I2S_USERCLKOUT1);					// 12.288MHz Out
	}
	
	#if (I2S_CODEC == WM8753)
		WM8753_CodecInitPCMIn(g_oI2SInfor[rhs_ePort].m_eDataFormat, (u32) g_oI2SInfor[rhs_ePort].m_dSamplingFreq, 
								g_oI2SInfor[rhs_ePort].m_eOpMode,  g_oI2SInfor[rhs_ePort].m_eWordLength, LINEIN, SMDK_I2S);
	#elif (I2S_CODEC == WM8990)
		WM8990_CodecInitPCMIn(g_oI2SInfor[rhs_ePort].m_eDataFormat, (u32) g_oI2SInfor[rhs_ePort].m_dSamplingFreq, 
								g_oI2SInfor[rhs_ePort].m_eOpMode,  g_oI2SInfor[rhs_ePort].m_eWordLength, LINEIN, SMDK_I2S);
	#elif (I2S_CODEC == WM8580)
	WM8580_CodecInitPCMIn(g_oI2SInfor[rhs_ePort].m_eDataFormat, (u32) g_oI2SInfor[rhs_ePort].m_dSamplingFreq, 
								g_oI2SInfor[rhs_ePort].m_eOpMode, g_oI2SInfor[rhs_ePort].m_eWordLength, LINEIN, SMDK_I2S);
	#endif	
	I2SMOD_SetTXR(rhs_ePort, RXOnly);
	I2S_DataInDMAStart(rhs_ePort);
	I2S_DataInDMAStop(rhs_ePort);
}
void I2S_PCMOutDMA(AUDIO_PORT	rhs_ePort)
{
	UART_Printf("\nPlay Wave File.\n");
	
	// Data From NAND
	#ifdef I2SFROMNAND
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
	NAND_ReadMultiPage(0, 1000, 0, (u8 *)0x51000000, 21690392);
	#endif 

	
	if ((g_oI2SInfor[rhs_ePort].m_CLKSRC != I2S_EXTERNALCDCLK) && (g_oI2SInfor[rhs_ePort].m_eOpMode == Master)) 
	{
		I2S_CDCLKOut(rhs_ePort);											
	}
	else if ((g_oI2SInfor[rhs_ePort].m_CLKSRC == I2S_FIN) && (g_oI2SInfor[rhs_ePort].m_eOpMode == Slave)) 
	{
		I2S_CDCLKOut(rhs_ePort);										//12MHz Out
	}
	else if ((g_oI2SInfor[rhs_ePort].m_CLKSRC != I2S_EXTERNALCDCLK) && (g_oI2SInfor[rhs_ePort].m_CLKSRC != I2S_FIN) && (g_oI2SInfor[rhs_ePort].m_eOpMode == Slave)) 
	{
		if ( g_oI2SInfor[rhs_ePort].m_dSamplingFreq % 11025)
			I2S_SetEpllCDCLKOut(rhs_ePort, I2S_USERCLKOUT2);			// 11.2896MHz Out
		else
			I2S_SetEpllCDCLKOut(rhs_ePort, I2S_USERCLKOUT1);			// 12.288MHz Out
	}
	
	#if (I2S_CODEC == WM8753)
		WM8753_CodecInitPCMOut(g_oI2SInfor[rhs_ePort].m_eDataFormat, (u32) g_oI2SInfor[rhs_ePort].m_dSamplingFreq, 
								g_oI2SInfor[rhs_ePort].m_eOpMode,  g_oI2SInfor[rhs_ePort].m_eWordLength, SMDK_I2S);
	#elif (I2S_CODEC == WM8990)
		WM8990_CodecInitPCMOut(g_oI2SInfor[rhs_ePort].m_eDataFormat, (u32) g_oI2SInfor[rhs_ePort].m_dSamplingFreq,	
								g_oI2SInfor[rhs_ePort].m_eOpMode,  g_oI2SInfor[rhs_ePort].m_eWordLength, SMDK_I2S);
	#elif (I2S_CODEC == WM8580)
		WM8580_CodecInitPCMOut(g_oI2SInfor[rhs_ePort].m_eDataFormat, (u32) g_oI2SInfor[rhs_ePort].m_dSamplingFreq, 
								g_oI2SInfor[rhs_ePort].m_eOpMode,  g_oI2SInfor[rhs_ePort].m_eWordLength, SMDK_I2S);
	#endif
	I2SMOD_SetTXR(rhs_ePort, TXOnly);
	I2S_DataOutDMAStart(rhs_ePort);
	I2S_DataOutDMAStop(rhs_ePort);
}
void I2S_PCMDataInOutDMA(AUDIO_PORT rhs_ePort)
{
	UART_Printf("Supply Sound to I2S CODEC via Line In Connector.\n");
	UART_Printf("Press any key to record.\n");
	UART_Getc();
	UART_Printf("Recording...\n");	
	
	if ((g_oI2SInfor[rhs_ePort].m_CLKSRC != I2S_EXTERNALCDCLK) && (g_oI2SInfor[rhs_ePort].m_eOpMode == Master)) 
	{
		I2S_CDCLKOut(rhs_ePort);											
	}
	else if ((g_oI2SInfor[rhs_ePort].m_CLKSRC == I2S_FIN) && (g_oI2SInfor[rhs_ePort].m_eOpMode == Slave)) 
	{
		I2S_CDCLKOut(rhs_ePort);										//12MHz Out
	}
	else if ((g_oI2SInfor[rhs_ePort].m_CLKSRC != I2S_EXTERNALCDCLK) && (g_oI2SInfor[rhs_ePort].m_CLKSRC != I2S_FIN) && (g_oI2SInfor[rhs_ePort].m_eOpMode == Slave)) 
	{
		if ( g_oI2SInfor[rhs_ePort].m_dSamplingFreq % 11025)
			I2S_SetEpllCDCLKOut(rhs_ePort, I2S_USERCLKOUT2);					// 11.2896MHz Out
		else
			I2S_SetEpllCDCLKOut(rhs_ePort, I2S_USERCLKOUT1);					// 12.288MHz Out
	}
	
	#if (I2S_CODEC == WM8753)
		WM8753_CodecInitPCMIn(g_oI2SInfor[rhs_ePort].m_eDataFormat, (u32) g_oI2SInfor[rhs_ePort].m_dSamplingFreq, 
								g_oI2SInfor[rhs_ePort].m_eOpMode,  g_oI2SInfor[rhs_ePort].m_eWordLength, LINEIN, SMDK_I2S);
	#elif (I2S_CODEC == WM8990)
		WM8990_CodecInitPCMIn(g_oI2SInfor[rhs_ePort].m_eDataFormat, (u32) g_oI2SInfor[rhs_ePort].m_dSamplingFreq,
								g_oI2SInfor[rhs_ePort].m_eOpMode,  g_oI2SInfor[rhs_ePort].m_eWordLength, LINEIN, SMDK_I2S);
	#elif (I2S_CODEC == WM8580)
		WM8580_CodecInitPCMIn(g_oI2SInfor[rhs_ePort].m_eDataFormat, (u32) g_oI2SInfor[rhs_ePort].m_dSamplingFreq, 
								g_oI2SInfor[rhs_ePort].m_eOpMode, g_oI2SInfor[rhs_ePort].m_eWordLength, LINEIN, SMDK_I2S);
	
	#endif
	I2SMOD_SetTXR(rhs_ePort, RXOnly);
	I2S_DataInDMAStart(rhs_ePort);
	I2S_DataInDMAStop(rhs_ePort);

	UART_Printf("\nListen to Sound via Speak Out Connector.\n");
	UART_Printf("Press any key to play.\n");
	UART_Getc();	
	
	#if (I2S_CODEC == WM8753)
		WM8753_CodecInitPCMOut(g_oI2SInfor[rhs_ePort].m_eDataFormat, (u32) g_oI2SInfor[rhs_ePort].m_dSamplingFreq, 
								g_oI2SInfor[rhs_ePort].m_eOpMode,  g_oI2SInfor[rhs_ePort].m_eWordLength, SMDK_I2S);
	#elif (I2S_CODEC == WM8990)
		WM8990_CodecInitPCMOut(g_oI2SInfor[rhs_ePort].m_eDataFormat, (u32) g_oI2SInfor[rhs_ePort].m_dSamplingFreq,	
								g_oI2SInfor[rhs_ePort].m_eOpMode,  g_oI2SInfor[rhs_ePort].m_eWordLength, SMDK_I2S);
	#endif
	I2SMOD_SetTXR(rhs_ePort, TXOnly);
	I2S_DataOutDMAStart(rhs_ePort);
	I2S_DataOutDMAStop(rhs_ePort);
	
}




void I2S_PCMDataInOutPolling(AUDIO_PORT rhs_ePort)
{
	UART_Printf("Supply Sound to I2S CODEC via Line In Connector.\n");
	UART_Printf("Press any key to record.\n");
	UART_Getc();
	
	if ((g_oI2SInfor[rhs_ePort].m_CLKSRC != I2S_EXTERNALCDCLK) && (g_oI2SInfor[rhs_ePort].m_eOpMode == Master)) 
	{
		I2S_CDCLKOut(rhs_ePort);											
	}
	else if ((g_oI2SInfor[rhs_ePort].m_CLKSRC == I2S_FIN) && (g_oI2SInfor[rhs_ePort].m_eOpMode == Slave)) 
	{
		I2S_CDCLKOut(rhs_ePort);										//12MHz Out
	}
	else if ((g_oI2SInfor[rhs_ePort].m_CLKSRC != I2S_EXTERNALCDCLK) && (g_oI2SInfor[rhs_ePort].m_CLKSRC != I2S_FIN) && (g_oI2SInfor[rhs_ePort].m_eOpMode == Slave)) 
	{
		if ( g_oI2SInfor[rhs_ePort].m_dSamplingFreq % 11025)
			I2S_SetEpllCDCLKOut(rhs_ePort, I2S_USERCLKOUT2);					// 11.2896MHz Out
		else
			I2S_SetEpllCDCLKOut(rhs_ePort, I2S_USERCLKOUT1);					// 12.288MHz Out
	}
	
	#if (I2S_CODEC == WM8753)
		WM8753_CodecInitPCMIn(g_oI2SInfor[rhs_ePort].m_eDataFormat, (u32) g_oI2SInfor[rhs_ePort].m_dSamplingFreq, 
								g_oI2SInfor[rhs_ePort].m_eOpMode,  g_oI2SInfor[rhs_ePort].m_eWordLength, LINEIN, SMDK_I2S);
	#elif (I2S_CODEC == WM8990)
		WM8990_CodecInitPCMIn(g_oI2SInfor[rhs_ePort].m_eDataFormat, (u32) g_oI2SInfor[rhs_ePort].m_dSamplingFreq,
								g_oI2SInfor[rhs_ePort].m_eOpMode,  g_oI2SInfor[rhs_ePort].m_eWordLength, LINEIN,SMDK_I2S);
	#elif (I2S_CODEC == WM8580)
		WM8580_CodecInitPCMIn(g_oI2SInfor[rhs_ePort].m_eDataFormat, (u32) g_oI2SInfor[rhs_ePort].m_dSamplingFreq, 
								g_oI2SInfor[rhs_ePort].m_eOpMode, g_oI2SInfor[rhs_ePort].m_eWordLength, LINEIN, SMDK_I2S);
	
	#endif
	I2SMOD_SetTXR(rhs_ePort, RXOnly);
	I2S_DataInPolling(rhs_ePort);

	UART_Printf("\nListen to Sound via Speak Out Connector.\n");
	UART_Printf("Press any key to play.\n");
	UART_Getc();
	#if (I2S_CODEC == WM8753)
		WM8753_CodecInitPCMOut(g_oI2SInfor[rhs_ePort].m_eDataFormat, (u32) g_oI2SInfor[rhs_ePort].m_dSamplingFreq, 
								g_oI2SInfor[rhs_ePort].m_eOpMode,  g_oI2SInfor[rhs_ePort].m_eWordLength, SMDK_I2S);
	#elif (I2S_CODEC == WM8990)
		WM8990_CodecInitPCMOut(g_oI2SInfor[rhs_ePort].m_eDataFormat, (u32) g_oI2SInfor[rhs_ePort].m_dSamplingFreq, 
								g_oI2SInfor[rhs_ePort].m_eOpMode,  g_oI2SInfor[rhs_ePort].m_eWordLength, SMDK_I2S);
	#elif (I2S_CODEC == WM8580)
		WM8580_CodecInitPCMOut(g_oI2SInfor[rhs_ePort].m_eDataFormat, (u32) g_oI2SInfor[rhs_ePort].m_dSamplingFreq, 
								g_oI2SInfor[rhs_ePort].m_eOpMode,  g_oI2SInfor[rhs_ePort].m_eWordLength, SMDK_I2S);
	#endif
	I2SMOD_SetTXR(rhs_ePort, TXOnly);
	I2S_DataOutPolling(rhs_ePort);		
}
void I2S_PCMDataBypassPolling(AUDIO_PORT rhs_ePort)
{
	if ((g_oI2SInfor[rhs_ePort].m_CLKSRC != I2S_EXTERNALCDCLK) && (g_oI2SInfor[rhs_ePort].m_eOpMode == Master)) 
	{
		I2S_CDCLKOut(rhs_ePort);											
	}
	else if ((g_oI2SInfor[rhs_ePort].m_CLKSRC == I2S_FIN) && (g_oI2SInfor[rhs_ePort].m_eOpMode == Slave)) 
	{
		I2S_CDCLKOut(rhs_ePort);										//12MHz Out
	}
	else if ((g_oI2SInfor[rhs_ePort].m_CLKSRC != I2S_EXTERNALCDCLK) && (g_oI2SInfor[rhs_ePort].m_CLKSRC != I2S_FIN) && (g_oI2SInfor[rhs_ePort].m_eOpMode == Slave)) 
	{
		if ( g_oI2SInfor[rhs_ePort].m_dSamplingFreq % 11025)
			I2S_SetEpllCDCLKOut(rhs_ePort, I2S_USERCLKOUT2);					// 11.2896MHz Out
		else
			I2S_SetEpllCDCLKOut(rhs_ePort, I2S_USERCLKOUT1);					// 12.288MHz Out
	}

	#if (I2S_CODEC == WM8753)
		WM8753_CodecInitPCMOut(g_oI2SInfor[rhs_ePort].m_eDataFormat, (u32) g_oI2SInfor[rhs_ePort].m_dSamplingFreq, 
										g_oI2SInfor[rhs_ePort].m_eOpMode,  g_oI2SInfor[rhs_ePort].m_eWordLength, SMDK_I2S);
	#elif (I2S_CODEC == WM8990)
		WM8990_CodecInitPCMOut(g_oI2SInfor[rhs_ePort].m_eDataFormat, (u32) g_oI2SInfor[rhs_ePort].m_dSamplingFreq,
										g_oI2SInfor[rhs_ePort].m_eOpMode,  g_oI2SInfor[rhs_ePort].m_eWordLength, SMDK_I2S);
	#elif (I2S_CODEC == WM8580)
		WM8580_CodecInitPCMOut(g_oI2SInfor[rhs_ePort].m_eDataFormat, (u32) g_oI2SInfor[rhs_ePort].m_dSamplingFreq, 
								g_oI2SInfor[rhs_ePort].m_eOpMode,  g_oI2SInfor[rhs_ePort].m_eWordLength, SMDK_I2S);
	#endif
	I2S_DataBypassPolling(rhs_ePort);
}

void I2S_PCMDataBypassDMA(AUDIO_PORT rhs_ePort)
{
	if ((g_oI2SInfor[rhs_ePort].m_CLKSRC != I2S_EXTERNALCDCLK) && (g_oI2SInfor[rhs_ePort].m_eOpMode == Master)) 
	{
		I2S_CDCLKOut(rhs_ePort);											
	}
	else if ((g_oI2SInfor[rhs_ePort].m_CLKSRC == I2S_FIN) && (g_oI2SInfor[rhs_ePort].m_eOpMode == Slave)) 
	{
		I2S_CDCLKOut(rhs_ePort);										// 12MHz Out
	}
	else if ((g_oI2SInfor[rhs_ePort].m_CLKSRC != I2S_EXTERNALCDCLK) && (g_oI2SInfor[rhs_ePort].m_CLKSRC != I2S_FIN) && (g_oI2SInfor[rhs_ePort].m_eOpMode == Slave)) 
	{
		if ( g_oI2SInfor[rhs_ePort].m_dSamplingFreq % 11025)
			I2S_SetEpllCDCLKOut(rhs_ePort, I2S_USERCLKOUT2);			// 11.2896MHz Out
		else
			I2S_SetEpllCDCLKOut(rhs_ePort, I2S_USERCLKOUT1);			// 12.288MHz Out
	}
	
	WM8753_CodecInitPCMOut(g_oI2SInfor[rhs_ePort].m_eDataFormat, (u32) g_oI2SInfor[rhs_ePort].m_dSamplingFreq, 
								g_oI2SInfor[rhs_ePort].m_eOpMode,  g_oI2SInfor[rhs_ePort].m_eWordLength, SMDK_I2S);
	I2S_DataBypassDMA(rhs_ePort);
}

void I2S_PCMDataInIntr(AUDIO_PORT rhs_ePort)
{
	UART_Printf("Supply Sound to I2S CODEC via Line In Connector.\n");
	UART_Printf("Press any key to record.\n");
	UART_Getc();
	
	if ((g_oI2SInfor[rhs_ePort].m_CLKSRC != I2S_EXTERNALCDCLK) && (g_oI2SInfor[rhs_ePort].m_eOpMode == Master)) 
	{
		I2S_CDCLKOut(rhs_ePort);											
	}
	else if ((g_oI2SInfor[rhs_ePort].m_CLKSRC == I2S_FIN) && (g_oI2SInfor[rhs_ePort].m_eOpMode == Slave)) 
	{
		I2S_CDCLKOut(rhs_ePort);										//12MHz Out
	}
	else if ((g_oI2SInfor[rhs_ePort].m_CLKSRC != I2S_EXTERNALCDCLK) && (g_oI2SInfor[rhs_ePort].m_CLKSRC != I2S_FIN) && (g_oI2SInfor[rhs_ePort].m_eOpMode == Slave)) 
	{
		if ( g_oI2SInfor[rhs_ePort].m_dSamplingFreq % 11025)
			I2S_SetEpllCDCLKOut(rhs_ePort, I2S_USERCLKOUT2);			// 11.2896MHz Out
		else
			I2S_SetEpllCDCLKOut(rhs_ePort, I2S_USERCLKOUT1);			// 12.288MHz Out
	}

	#if (I2S_CODEC == WM8753)
		WM8753_CodecInitPCMIn(g_oI2SInfor[rhs_ePort].m_eDataFormat, (u32) g_oI2SInfor[rhs_ePort].m_dSamplingFreq, 
								g_oI2SInfor[rhs_ePort].m_eOpMode,  g_oI2SInfor[rhs_ePort].m_eWordLength, LINEIN, SMDK_I2S);
	#elif (I2S_CODEC == WM8990)
		WM8990_CodecInitPCMIn(g_oI2SInfor[rhs_ePort].m_eDataFormat, (u32) g_oI2SInfor[rhs_ePort].m_dSamplingFreq,
								g_oI2SInfor[rhs_ePort].m_eOpMode,  g_oI2SInfor[rhs_ePort].m_eWordLength, LINEIN,SMDK_I2S);
	#elif (I2S_CODEC == WM8580)
		WM8580_CodecInitPCMIn(g_oI2SInfor[rhs_ePort].m_eDataFormat, (u32) g_oI2SInfor[rhs_ePort].m_dSamplingFreq, 
								g_oI2SInfor[rhs_ePort].m_eOpMode, g_oI2SInfor[rhs_ePort].m_eWordLength, LINEIN, SMDK_I2S);
	
	#endif
	I2SMOD_SetTXR(rhs_ePort, RXOnly);
	I2S_DataInIntr(rhs_ePort);

}

void I2S_PCMDataOutIntr(AUDIO_PORT rhs_ePort)
{
	UART_Printf("\nListen to Sound via Speak Out Connector.\n");
	UART_Printf("Press any key to play.\n");
	UART_Getc();
	
	if ((g_oI2SInfor[rhs_ePort].m_CLKSRC != I2S_EXTERNALCDCLK) && (g_oI2SInfor[rhs_ePort].m_eOpMode == Master)) 
	{
		I2S_CDCLKOut(rhs_ePort);											
	}
	else if ((g_oI2SInfor[rhs_ePort].m_CLKSRC == I2S_FIN) && (g_oI2SInfor[rhs_ePort].m_eOpMode == Slave)) 
	{
		I2S_CDCLKOut(rhs_ePort);										//12MHz Out
	}
	else if ((g_oI2SInfor[rhs_ePort].m_CLKSRC != I2S_EXTERNALCDCLK) && (g_oI2SInfor[rhs_ePort].m_CLKSRC != I2S_FIN) && (g_oI2SInfor[rhs_ePort].m_eOpMode == Slave)) 
	{
		if ( g_oI2SInfor[rhs_ePort].m_dSamplingFreq % 11025)
			I2S_SetEpllCDCLKOut(rhs_ePort, I2S_USERCLKOUT2);			// 11.2896MHz Out
		else
			I2S_SetEpllCDCLKOut(rhs_ePort, I2S_USERCLKOUT1);			// 12.288MHz Out
	}

	#if (I2S_CODEC == WM8753)
		WM8753_CodecInitPCMOut(g_oI2SInfor[rhs_ePort].m_eDataFormat, (u32) g_oI2SInfor[rhs_ePort].m_dSamplingFreq, 
										g_oI2SInfor[rhs_ePort].m_eOpMode,  g_oI2SInfor[rhs_ePort].m_eWordLength, SMDK_I2S);
	#elif (I2S_CODEC == WM8990)
		WM8990_CodecInitPCMOut(g_oI2SInfor[rhs_ePort].m_eDataFormat, (u32) g_oI2SInfor[rhs_ePort].m_dSamplingFreq,
										g_oI2SInfor[rhs_ePort].m_eOpMode,  g_oI2SInfor[rhs_ePort].m_eWordLength, SMDK_I2S);
	#elif (I2S_CODEC == WM8580)
		WM8580_CodecInitPCMOut(g_oI2SInfor[rhs_ePort].m_eDataFormat, (u32) g_oI2SInfor[rhs_ePort].m_dSamplingFreq, 
								g_oI2SInfor[rhs_ePort].m_eOpMode,  g_oI2SInfor[rhs_ePort].m_eWordLength, SMDK_I2S);
	#endif
	I2SMOD_SetTXR(rhs_ePort, TXOnly);
	I2S_DataOutIntr(rhs_ePort);
}

void I2S_PCMDataInOutIntr(AUDIO_PORT rhs_ePort)
{

	UART_Printf("Supply Sound to I2S CODEC via Line In Connector.\n");
	UART_Printf("Press any key to record.\n");
	UART_Getc();	
	
	if ((g_oI2SInfor[rhs_ePort].m_CLKSRC != I2S_EXTERNALCDCLK) && (g_oI2SInfor[rhs_ePort].m_eOpMode == Master)) 
	{
		I2S_CDCLKOut(rhs_ePort);											
	}
	else if ((g_oI2SInfor[rhs_ePort].m_CLKSRC == I2S_FIN) && (g_oI2SInfor[rhs_ePort].m_eOpMode == Slave)) 
	{
		I2S_CDCLKOut(rhs_ePort);										//12MHz Out
	}
	else if ((g_oI2SInfor[rhs_ePort].m_CLKSRC != I2S_EXTERNALCDCLK) && (g_oI2SInfor[rhs_ePort].m_CLKSRC != I2S_FIN) && (g_oI2SInfor[rhs_ePort].m_eOpMode == Slave)) 
	{
		if ( g_oI2SInfor[rhs_ePort].m_dSamplingFreq % 11025)
			I2S_SetEpllCDCLKOut(rhs_ePort, I2S_USERCLKOUT2);			// 11.2896MHz Out
		else
			I2S_SetEpllCDCLKOut(rhs_ePort, I2S_USERCLKOUT1);			// 12.288MHz Out
	}
	
	#if (I2S_CODEC == WM8753)
		WM8753_CodecInitPCMIn(g_oI2SInfor[rhs_ePort].m_eDataFormat, (u32) g_oI2SInfor[rhs_ePort].m_dSamplingFreq, 
								g_oI2SInfor[rhs_ePort].m_eOpMode,  g_oI2SInfor[rhs_ePort].m_eWordLength, LINEIN, SMDK_I2S);
	#elif (I2S_CODEC == WM8990)
		WM8990_CodecInitPCMIn(g_oI2SInfor[rhs_ePort].m_eDataFormat, (u32) g_oI2SInfor[rhs_ePort].m_dSamplingFreq,
								g_oI2SInfor[rhs_ePort].m_eOpMode,  g_oI2SInfor[rhs_ePort].m_eWordLength, LINEIN,SMDK_I2S);
	#elif (I2S_CODEC == WM8580)
		WM8580_CodecInitPCMIn(g_oI2SInfor[rhs_ePort].m_eDataFormat, (u32) g_oI2SInfor[rhs_ePort].m_dSamplingFreq, 
								g_oI2SInfor[rhs_ePort].m_eOpMode, g_oI2SInfor[rhs_ePort].m_eWordLength, LINEIN, SMDK_I2S);
	
	#endif

	I2S_DataInIntr(rhs_ePort);

	UART_Printf("\nListen to Sound via Speak Out Connector.\n");
	UART_Printf("Press any key to play.\n");
	UART_Getc();
	#if (I2S_CODEC == WM8753)
		WM8753_CodecInitPCMOut(g_oI2SInfor[rhs_ePort].m_eDataFormat, (u32) g_oI2SInfor[rhs_ePort].m_dSamplingFreq, 
										g_oI2SInfor[rhs_ePort].m_eOpMode,  g_oI2SInfor[rhs_ePort].m_eWordLength, SMDK_I2S);
	#elif (I2S_CODEC == WM8990)
		WM8990_CodecInitPCMOut(g_oI2SInfor[rhs_ePort].m_eDataFormat, (u32) g_oI2SInfor[rhs_ePort].m_dSamplingFreq,
										g_oI2SInfor[rhs_ePort].m_eOpMode,  g_oI2SInfor[rhs_ePort].m_eWordLength, SMDK_I2S);
	#endif
	I2S_DataOutIntr(rhs_ePort);


}

void I2S_AutoBFSRFSBLCTest(AUDIO_PORT rhs_ePort)
{
	I2S_LRCLKLength			eTempRFS = g_oI2SInfor[rhs_ePort].m_eRFS;
	I2S_EffectiveLength		eTempBFS = g_oI2SInfor[rhs_ePort].m_eBFS;
	PCMWordLength			eTempBLC = g_oI2SInfor[rhs_ePort].m_eWordLength;
	g_oI2SInfor[rhs_ePort].m_eRFS			= RFS_256fs;
	
	
	
	do				//Change the RFS
	{	
		g_oI2SInfor[rhs_ePort].m_eBFS 		= BFS_32fs;
		do			//Change the BFS
		{	
			g_oI2SInfor[rhs_ePort].m_eWordLength = Word16;
			do		//Change the BLC
			{
				I2SMOD_PutData(rhs_ePort, I2S_GetI2SInfor(rhs_ePort));
				UART_Printf("Test Condition\n");
				UART_Printf("RFS : %d\t",(u32) g_oI2SInfor[rhs_ePort].m_eRFS);UART_Printf("BFS: %d\t",(u32) g_oI2SInfor[rhs_ePort].m_eBFS);
				UART_Printf("Word Length : %d\n",(u32) g_oI2SInfor[rhs_ePort].m_eWordLength);
				UART_Printf("Press any key to Test.\n");	
				UART_Getc();
				
				UART_Printf("Supply Sound to I2S CODEC via Line In Connector.\n");
				if ((g_oI2SInfor[rhs_ePort].m_CLKSRC != I2S_EXTERNALCDCLK) && (g_oI2SInfor[rhs_ePort].m_eOpMode == Master)) 
				{
					I2S_CDCLKOut(rhs_ePort);											
				}
				else if ((g_oI2SInfor[rhs_ePort].m_CLKSRC == I2S_FIN) && (g_oI2SInfor[rhs_ePort].m_eOpMode == Slave)) 
				{
					I2S_CDCLKOut(rhs_ePort);										//12MHz Out
				}
				else if ((g_oI2SInfor[rhs_ePort].m_CLKSRC != I2S_EXTERNALCDCLK) && (g_oI2SInfor[rhs_ePort].m_CLKSRC != I2S_FIN) && (g_oI2SInfor[rhs_ePort].m_eOpMode == Slave)) 
				{
					if ( g_oI2SInfor[rhs_ePort].m_dSamplingFreq % 11025)
						I2S_SetEpllCDCLKOut(rhs_ePort, I2S_USERCLKOUT2);					// 11.2896MHz Out
					else
						I2S_SetEpllCDCLKOut(rhs_ePort, I2S_USERCLKOUT1);					// 12.288MHz Out
				}
				WM8753_CodecInitPCMIn(g_oI2SInfor[rhs_ePort].m_eDataFormat, (u32) g_oI2SInfor[rhs_ePort].m_dSamplingFreq, 
										g_oI2SInfor[rhs_ePort].m_eOpMode,  g_oI2SInfor[rhs_ePort].m_eWordLength, LINEIN, SMDK_I2S);
				I2S_DataInDMAStart(rhs_ePort);
				I2S_DataInDMAStop(rhs_ePort);

				UART_Printf("\nListen to Sound via Speak Out Connector.\n");
				UART_Printf("Press any key to play.\n");	
				
				
				WM8753_CodecInitPCMOut(g_oI2SInfor[rhs_ePort].m_eDataFormat, (u32) g_oI2SInfor[rhs_ePort].m_dSamplingFreq, 
										g_oI2SInfor[rhs_ePort].m_eOpMode,  g_oI2SInfor[rhs_ePort].m_eWordLength, SMDK_I2S);
				I2S_DataOutDMAStart(rhs_ePort);
				I2S_DataOutDMAStop(rhs_ePort);

				if ( (u32) g_oI2SInfor[rhs_ePort].m_eWordLength < (u32)  Word8)
					(u32)(g_oI2SInfor[rhs_ePort].m_eWordLength)++;
				else 
					break;				
				
			}while(1);	

			if ( (u32) g_oI2SInfor[rhs_ePort].m_eBFS < (u32) BFS_24fs)
				(u32) g_oI2SInfor[rhs_ePort].m_eBFS++;
			else 
				break;
			
		}while(1);	

		if ( (u32) g_oI2SInfor[rhs_ePort].m_eRFS < (u32) RFS_768fs)
				(u32) g_oI2SInfor[rhs_ePort].m_eRFS++;
		else 
			break;
		
	}while(1);

	g_oI2SInfor[rhs_ePort].m_eRFS 		= eTempRFS;
	g_oI2SInfor[rhs_ePort].m_eBFS 		= eTempBFS;
	g_oI2SInfor[rhs_ePort].m_eWordLength = eTempBLC;
	
}


/*---------------------------------- Test Functions -------------------------------*/
//////////
// Function Name : I2S_LoopBackTest
// Function Description : 
//   This function implements I2S Loopback Test function.

//	To execute this test, SMDK6410 must be modified.
//	
//			J13						J13
//		pin3  (BCLK)			pin3  (BCLK)
//		pin7  (CDCLK)			pin7  (CDCLK)  --> These signal should be connected to R249.(Ext.Clock)
//		pin11(LRCK)				pin11(LRCK)
//		pin17(DI)				pin21(DO)
//		pin21(DO)				pin17(DI)
//		
//		Master - AUDIO_PORT0 (Master sends LRCK and BCLK to Slave port.)
//		Slave  - AUDIO_PORT1
//		CDCLK - External Clock
// Input : AUDIO_PORT : DO NOT USE
// Output : NONE
// Version : 


void I2S_LoopBackTest(AUDIO_PORT rhs_ePort)
{
	u32 uSel;
	AUDIO_PORT MasterPort, SlavePort;
	I2S_Infor TempInfor0 = g_oI2SInfor[AUDIO_PORT0], TempInfor1 = g_oI2SInfor[AUDIO_PORT1];
	u32 *pTXDataAddr, *pRXDataAddr;
	s32 uTransferLen = g_oI2SInfor[MasterPort].m_uPcmDataSize;
	//Set Port Mode
	while(1)
	{
		g_TestMode = I2S_DUAL;
		UART_Printf("Loop Back Mode?\n");
		UART_Printf("0. Single Port Mode		1. Loop Back Mode.\n");
		if (UART_GetIntNum() == 1)
		{			
			UART_Printf("Select Master Port\n");
			UART_Printf("0. Port0	1. Port1\n");
			uSel = UART_GetIntNum();
			if (uSel == 0)
			{
				UART_Printf("Master Port : 0, Slave Port : 1\n");
				MasterPort = AUDIO_PORT0;
				SlavePort = AUDIO_PORT1;
			}
			else
			{
				UART_Printf("Master Port : 1, Slave Port : 0\n");
				MasterPort = AUDIO_PORT1;
				SlavePort = AUDIO_PORT0;
			}
			UART_Printf("Data From Master Port to Slave Port\n");
			UART_Printf("Press Any key to start\n");
			UART_Getc();
			
			g_oI2SInfor[MasterPort].m_eOpMode 	= Master;
			g_oI2SInfor[SlavePort].m_eOpMode 	= Slave;
			
			g_oI2SInfor[SlavePort].m_eWordLength	= g_oI2SInfor[MasterPort].m_eWordLength;
			g_oI2SInfor[SlavePort].m_eDataFormat	= g_oI2SInfor[MasterPort].m_eDataFormat;
			g_oI2SInfor[SlavePort].m_eLRCLKPolarity	= g_oI2SInfor[MasterPort].m_eLRCLKPolarity;
			g_oI2SInfor[SlavePort].m_eRFS 		= g_oI2SInfor[MasterPort].m_eRFS;
			g_oI2SInfor[SlavePort].m_eBFS			= g_oI2SInfor[MasterPort].m_eBFS;
			
			I2SMOD_PutData(MasterPort, &(g_oI2SInfor[MasterPort]));
			I2SMOD_PutData(SlavePort, &(g_oI2SInfor[SlavePort]));
			
			I2S_CDCLKOut(MasterPort);
			
			I2SMOD_SetTXR(MasterPort, TXOnly);
			I2SMOD_SetTXR(SlavePort, RXOnly);
			// DMA Start
			I2S_DataOutDMAStart(MasterPort);
			I2S_DataInDMAStart(SlavePort);			

			g_uI2SRecDone[SlavePort]= 0;
			while(g_uI2SRecDone[SlavePort] == 0)
			{
				UART_Printf(".");
        			Delay(3000);
			}			
			 g_uI2SRecDone[SlavePort] = 0; 
			 
			I2S_DataInDMAStop(SlavePort);	
			I2S_DataOutDMAStop(MasterPort);
			
			I2SFIC_AutoFlush(MasterPort, TXRXBoth);
			I2SFIC_AutoFlush(SlavePort, TXRXBoth);
			// Data Test 
			pTXDataAddr = g_oI2SInfor[MasterPort].m_uPlayBufferStartAddr;
			pRXDataAddr = g_oI2SInfor[SlavePort].m_uRecBufferStartAddr;
			uTransferLen = g_oI2SInfor[MasterPort].m_uPcmDataSize;
			
			while(uTransferLen != 0)
			{
				if ( *pTXDataAddr++ == *pRXDataAddr++ );
				else
				{
					UART_Printf("Error!!\n");
					UART_Printf("Address : 0x%x\n", pRXDataAddr);
				}
				uTransferLen -= 4;
			}			
		}
		else
			break;
	}	
	g_TestMode = I2S_SINGLE;
	g_oI2SInfor[AUDIO_PORT0] = TempInfor0;
	g_oI2SInfor[AUDIO_PORT1] = TempInfor1;
}

void I2S_LoopBackAgigTest(void)
{
	u32 uErrorCnt = 0;
	u32 uCnt;
	AUDIO_PORT MasterPort, SlavePort, TempPort;
	I2S_Infor TempInfor0 = g_oI2SInfor[AUDIO_PORT0], TempInfor1 = g_oI2SInfor[AUDIO_PORT1];
	u32 *pTXDataAddr, *pRXDataAddr;
	s32 uTransferLen = g_oI2SInfor[MasterPort].m_uPcmDataSize;
	//Set Port Mode

	g_TestMode = I2S_DUAL;	

	MasterPort = AUDIO_PORT0;
	SlavePort = AUDIO_PORT1;
	while(1)
	{
		UART_Printf("Master Port : %d\n",MasterPort);
		UART_Printf("Slave Port : %d\n",SlavePort);
		g_oI2SInfor[MasterPort].m_eOpMode 	= Master;
		g_oI2SInfor[SlavePort].m_eOpMode 	= Slave;
		
		g_oI2SInfor[SlavePort].m_eWordLength	= g_oI2SInfor[MasterPort].m_eWordLength;
		g_oI2SInfor[SlavePort].m_eDataFormat	= g_oI2SInfor[MasterPort].m_eDataFormat;
		g_oI2SInfor[SlavePort].m_eLRCLKPolarity	= g_oI2SInfor[MasterPort].m_eLRCLKPolarity;
		g_oI2SInfor[SlavePort].m_eRFS 		= g_oI2SInfor[MasterPort].m_eRFS;
		g_oI2SInfor[SlavePort].m_eBFS			= g_oI2SInfor[MasterPort].m_eBFS;
		
		I2SMOD_PutData(MasterPort, &(g_oI2SInfor[MasterPort]));
		I2SMOD_PutData(SlavePort, &(g_oI2SInfor[SlavePort]));
		
		I2S_CDCLKOut(MasterPort);
		
		I2SMOD_SetTXR(MasterPort, TXOnly);
		I2SMOD_SetTXR(SlavePort, RXOnly);
		// DMA Start
		I2S_DataOutDMAStart(MasterPort);
		I2S_DataInDMAStart(SlavePort);			

		g_uI2SRecDone[SlavePort]= 0;
		uCnt = 0;
		while(g_uI2SRecDone[SlavePort] == 0)
		{
			UART_Printf(".");
    			Delay(1000);
			uCnt++;
			#if 0
			if ( uCnt == 30)
			{
				I2SCON_ActiveDMA(SlavePort, RXOnly, INACTIVE);
				UART_Printf("RX DMA PAUSE!!\n");		
				
				I2SCON_ActiveDMA(MasterPort, TXOnly, INACTIVE);
				UART_Printf("TX DMA PAUSE!!\n");					
			}
			else if ( uCnt == 100 )
			{
				I2SCON_ActiveDMA(SlavePort, RXOnly, ACTIVE);
				UART_Printf("RX DMA PLAY!!\n");				
				I2SCON_ActiveDMA(MasterPort, TXOnly, ACTIVE);
				UART_Printf("TX DMA PLAY!!\n");
				
				
			}
			#endif
		}	
		 g_uI2SRecDone[SlavePort] = 0; 

		 // Data Test 
		pTXDataAddr = g_oI2SInfor[MasterPort].m_uPlayBufferStartAddr;
		pRXDataAddr = g_oI2SInfor[SlavePort].m_uRecBufferStartAddr;
		uTransferLen = g_oI2SInfor[MasterPort].m_uPcmDataSize;
		UART_Printf("Master TX, Slave RX\n");
		while(uTransferLen != 0)
		{
			if ( *pTXDataAddr++ == *pRXDataAddr++ );
			else
			{
				UART_Printf("Error NO : %d\n",uErrorCnt++);
				UART_Printf("Address : 0x%x\n", pRXDataAddr);
			}
			uTransferLen -= 4;
		}
		 
		I2S_DataInDMAStop(SlavePort);	
		I2S_DataOutDMAStop(MasterPort);

		I2SMOD_SetTXR(MasterPort, RXOnly);
		I2SMOD_SetTXR(SlavePort, TXOnly);
		// DMA Start
		I2S_DataInDMAStart(MasterPort);
		I2S_DataOutDMAStart(SlavePort);
					

		g_uI2SRecDone[MasterPort]= 0;
		uCnt = 0;
		while(g_uI2SRecDone[MasterPort] == 0)
		{
			UART_Printf(".");
    			Delay(1000);
			uCnt++;
			#if 0
			if ( uCnt == 30)
			{
				I2SCON_ActiveDMA(SlavePort, RXOnly, INACTIVE);
				UART_Printf("RX DMA PAUSE!!\n");		
				
				I2SCON_ActiveDMA(MasterPort, TXOnly, INACTIVE);
				UART_Printf("TX DMA PAUSE!!\n");					
			}
			else if ( uCnt == 100 )
			{
				I2SCON_ActiveDMA(SlavePort, RXOnly, ACTIVE);
				UART_Printf("RX DMA PLAY!!\n");				
				I2SCON_ActiveDMA(MasterPort, TXOnly, ACTIVE);
				UART_Printf("TX DMA PLAY!!\n");
				
				
			}
			#endif
		}	
		 g_uI2SRecDone[MasterPort] = 0; 
		 
		I2S_DataInDMAStop(MasterPort);	
		I2S_DataOutDMAStop(SlavePort);
		
		I2SFIC_AutoFlush(MasterPort, TXRXBoth);
		I2SFIC_AutoFlush(SlavePort, TXRXBoth);
		// Data Test 
		pTXDataAddr = g_oI2SInfor[SlavePort].m_uPlayBufferStartAddr;
		pRXDataAddr = g_oI2SInfor[MasterPort].m_uRecBufferStartAddr;
		uTransferLen = g_oI2SInfor[MasterPort].m_uPcmDataSize;
		*pRXDataAddr++;
		*pRXDataAddr++;
		UART_Printf("Master RX, Slave TX\n");
		while(uTransferLen != 0)
		{
			if ( *pTXDataAddr++ == *pRXDataAddr++ );
			else
			{
				UART_Printf("Error NO : %d\n",uErrorCnt++);
				UART_Printf("Address : 0x%x\n", pRXDataAddr);
			}
			uTransferLen -= 4;
		}	
		TempPort = MasterPort;
		MasterPort = SlavePort;
		SlavePort = TempPort;		
	}
	
	g_TestMode = I2S_SINGLE;
	g_oI2SInfor[AUDIO_PORT0] = TempInfor0;
	g_oI2SInfor[AUDIO_PORT1] = TempInfor1;

}
