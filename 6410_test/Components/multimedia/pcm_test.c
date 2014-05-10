/********************************************************************************
* 
*	Project Name : S3C6400 Validation
*
*	Copyright 2006 by Samsung Electronics, Inc.
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
*	File Description : Test for PCM I/F 
*
*	Author	: Sung-Hyun, Na
*	Dept. : AP Development Team
*	Created Date : 2008/03/03
*	Version : 0.1 
* 
*	History
*	- Version 0.1 (2007/03/16)
*	  -> Available with AK2440 PCM Codec.
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
#include "gpio.h"
#include "intc.h"
#include "dma.h"

#include "ac97.h"
#include "pcm.h"
#include "i2s.h"
#include "audio.h"

void PCM_Test(void);
AUDIO_PORT PCM_SelectPort(void);
static void PCMSet(AUDIO_PORT);
static void PCMCodecRegWR(AUDIO_PORT);
static void PCM_DataInDMA(AUDIO_PORT);
static void PCM_DataOutDMA(AUDIO_PORT);
static void PCMInOutDMA(AUDIO_PORT);
static void PCMInOutInt(AUDIO_PORT);
static void PCM_BypassTestDMA(AUDIO_PORT rhs_ePort);
static void PCMInAC97Out(AUDIO_PORT);
static void AC97InPCMOut(AUDIO_PORT);
static void PCMInOutPolling(AUDIO_PORT);
static void PCM_InterruptTest(AUDIO_PORT rhs_ePort);

extern PCM_Infor 		g_oPCMInfor[AUDIO_NUM];

volatile u32 g_isrTest = 0;

void PCM_Test(void)
{	
	const PCMtestFuncMenu pcm_function[]=
	{
		0,         					"Exit\n",
		PCMSet,					"Configuration\n",		

		
		PCMInOutPolling,			"PCM In/Out Polling\n",
		PCMInOutInt,				"PCM In/Out Interrupt Test\n",
		
		PCM_DataInDMA,			"DMA Mode Test for Data In(Recoding)",
		PCM_DataOutDMA,			"DMA Mode Test for Data Out(Playing)",
		PCMInOutDMA,			"PCM In/Out DMA Test\n",

		PCM_BypassTestDMA,		"Bypass DMA Mode",

		PCMCodecRegWR,			"PCM CODEC Register Write/Read Test\n",			
		PCMInAC97Out,			"PCM IN/AC97 OUT Test",
		AC97InPCMOut,			"AC97 IN/PCM OUT Test\n",

		PCM_InterruptTest,			"Interrupt Test\n",
		0,					0
	};
	u32 uCountFunc = 0;	
	AUDIO_PORT ePCMPort = AUDIO_PORT0;

	//Test SFR
	//TestSFR();
	
	PCM_Init(AUDIO_PORT0);
	PCM_Init(AUDIO_PORT1);	
	
	while (1)
	{
		u32 i, nSel;
		 
		PCM_InitPort(ePCMPort);
		UART_Printf("\n");

		UART_Printf("================================================================================\n");	
		UART_Printf("PCM Configuration\n");
		for (uCountFunc = 0; uCountFunc < AUDIO_NUM; uCountFunc++)
		{	
			UART_Printf("\n");
			UART_Printf("Port : %d\t",uCountFunc);				

			UART_Printf("Interface Data Format : ");
			if ( g_oPCMInfor[uCountFunc].m_ePCMMSBPosition==  AFTER_PCMSYNC_HIGH)
				UART_Printf("After SYNC High Format \n");
			else if ( g_oPCMInfor[uCountFunc].m_ePCMMSBPosition==  DURING_PCMSYNC_HIGH)
				UART_Printf("During SYNC High Format \n");			
			

			UART_Printf("Clock Source : ");
			if ( g_oPCMInfor[uCountFunc].m_ePCMClkSrc==  PCM_MOUT_EPLL)
				UART_Printf("EPLL Out\n");
			else if ( g_oPCMInfor[uCountFunc].m_ePCMClkSrc==  PCM_PCLK)
				UART_Printf("PCLK\n");
			else if ( g_oPCMInfor[uCountFunc].m_ePCMClkSrc==  PCM_MOUT_EPLL)
				UART_Printf("MPLL Out\n");
			else if ( g_oPCMInfor[uCountFunc].m_ePCMClkSrc==  PCM_FIN_EPLL)
				UART_Printf("System External CLK\n");
			else if ( g_oPCMInfor[uCountFunc].m_ePCMClkSrc==  PCM_PCMCDCLK)
				UART_Printf("PCM External CLK\n");
			
			UART_Printf("PCMSCLK Frequency : %3.1fKHz\t",(double)(g_oPCMInfor[uCountFunc].m_uPCMSClk)/1000);
			
			UART_Printf("PCMSYNC Frequency : %3.1fKHz\n",(double)(g_oPCMInfor[uCountFunc].m_uPCMSync)/1000);
			
			
		}		
		UART_Printf("================================================================================\n");	
		UART_Printf("Current PCM Port : %d\n",ePCMPort);
		
		for (i=0; (int)(pcm_function[i].desc)!=0; i++)
		{
			UART_Printf("%2d: %s\n", i,pcm_function[i].desc);
		}
		UART_Printf("%2d: PCM Port Selection\n",i++);
				
		UART_Printf("\nSelect the function to test : ");

		nSel = UART_GetIntNum();

		UART_Printf("\n");

		if (nSel == 0)
			break;
		if (nSel>0 && nSel<(sizeof(pcm_function)/8-1))
			 (pcm_function[nSel].func)(ePCMPort);
		else if ( nSel == (sizeof(pcm_function)/8-1) )
			ePCMPort = PCM_SelectPort();
	}
	
}

AUDIO_PORT PCM_SelectPort(void)
{
	u8 selTemp;
	
	
	UART_Printf("Select PCM PORT\n");
	UART_Printf("0. PORT0[D]			1. PORT1\n");
	selTemp = UART_GetIntNum();
	
	if (selTemp == 1)
	{	
		return AUDIO_PORT0;
	}
	else
	{	
		return AUDIO_PORT1;
	}	
}

static void PCMSet(AUDIO_PORT rhs_ePort)
{
	const PCMtestFuncMenu pcmset_function[]=
	{
		0,         						"Exit\n",
		PCM_Sel_PCMCLKSOURCE,		"PCMCLK Source Selecte",
		PCM_Sel_PCMSCLK,			"PCMSCLK Frequency",
		PCM_Sel_PCMSYNC,			"PCMSYNC Frequency(Sampling rate)",
		PCM_Sel_SyncPosition,			"MSB Position",
		PCM_Sel_Interrupt,				"Interrupt Condition",
		
		0,							 0
	};
	while (1)
	{
		u32 i, nSel;
		UART_Printf("\n");
		for (i=0; (int)(pcmset_function[i].desc)!=0; i++)
		{
			UART_Printf("%2d: %s\n", i,pcmset_function[i].desc);
		}
		UART_Printf("\nSelect the function : ");
		nSel = UART_GetIntNum();
		UART_Printf("\n");
		if (nSel == 0)
		{
			PCMCTL_PutData(rhs_ePort);
			break;
		}
		if (nSel>0 && nSel<(sizeof(pcmset_function)/8-1))
			 (pcmset_function[nSel].func)(rhs_ePort);
	}
}

static void PCMCodecRegWR(AUDIO_PORT rhs_ePort)
{
	u32 i;
/////////////////////////////////////////////////////////////////////
	#if (PCM_CODEC == AK2430)	
		UART_Printf("\nIIC Test of AK2430 PCM Codec.\n");
		UART_Printf("\nReg. Addr.	Reg. Data\n");	

		//for(i=0x2; i<0x1e; i++)
		//	UART_Printf("%2x		%2x \n", i, PCM_CODEC_IICRead(0x9e, i));		

		for(i=0;i<32;i++)
		{
			PCM_CODEC_IICWrite(0x9e, 0x15, i);
			//UART_Printf("MASTERVOL: %2x \n", PCM_CODEC_IICRead(0x9e, 0x15));		
		}
	
/////////////////////////////////////////////////////////////////////
	#elif (PCM_CODEC == WM9713)
		u32 uOutVol;
		UART_Printf("\nRegister W/R Test of WM9713 PCM Codec.\n");
		if(AC97_InitACLINK())
		{	
			for(i=0; i<32; i++)
			{
				AC97_CodecCmd(WRITE,0x04,i);		// Set HPL/R Volume 
				uOutVol = AC97_CodecCmd(READ,0x04,0x00000);
				UART_Printf("HPL/R Volume: %2x \n", uOutVol);
			}
		}
	
/////////////////////////////////////////////////////////////////////
	#elif (PCM_CODEC == WM8753)
	
		UART_Printf("\nIIC Test of WM8753 PCM Codec.\n");
		for(i=0; i<128; i++)
		{
			PCM_CODEC_IICWrite(0x34, 0x58, i);
			UART_Printf(".");
			Delay(1000);
		} 
		UART_Printf("\n");
	
/////////////////////////////////////////////////////////////////////	
	#elif (PCM_CODEC == WM8990)
	
		UART_Printf("\nIIC Test of WM8753 PCM Codec.\n");
		for(i=0; i<128; i++)
		{
			PCM_CODEC_IICWrite(0x34, 0x58, i);
			UART_Printf(".");
			Delay(1000);
		} 
		UART_Printf("\n");	
/////////////////////////////////////////////////////////////////////	
	#else
		Assert(0);
	#endif
	
}

static void PCM_DataInPolling(AUDIO_PORT rhs_ePort)
{
	UART_Printf("Supply Sound to PCM CODEC via Line In Connector.\n");
   	UART_Printf("Press any key to record.\n");
    	UART_Getc();	
	
	#if (PCM_CODEC == WM8753)
	// I2SCDCLK Out for Master Clock of WM8753 using another audio port
	I2S_Init((AUDIO_PORT)(~rhs_ePort & 0x1));		
	I2S_InitPort((AUDIO_PORT)((~rhs_ePort) & 0x1));
	I2S_CDCLKOut((AUDIO_PORT)((~rhs_ePort) & 0x1));
	// Codec Initialization for WM8753
	WM8753_CodecInitPCMIn(g_oPCMInfor[rhs_ePort].m_ePCMMSBPosition, g_oPCMInfor[rhs_ePort].m_uPCMSync, Master, Word16,  LINEIN, g_oPCMInfor[rhs_ePort].m_eLine);
	#elif (PCM_CODEC == WM8990)	
	if ( rhs_ePort == AUDIO_PORT0 )				// WM8990
	{
		// I2SCDCLK Out for Master Clock of WM8990 using another audio port
		I2S_Init(rhs_ePort);		
		I2S_SetCDCLKPort(rhs_ePort);
		I2S_CDCLKOut(rhs_ePort);
		// Codec Initialization for WM8990
		WM8990_CodecInitPCMIn(g_oPCMInfor[rhs_ePort].m_ePCMMSBPosition, g_oPCMInfor[rhs_ePort].m_uPCMSync, Master, Word16,  LINEIN, g_oPCMInfor[rhs_ePort].m_eLine);
	}
	else
		Assert(0);
	#elif (PCM_CODEC == WM9713)
	// Codec Initialization for WM9713
	WM9713_CodecInitPCMIn(g_oPCMInfor[rhs_ePort].m_ePCMMSBPosition, g_oPCMInfor[rhs_ePort].m_uPCMSync, Master, Word16,  LINEIN, g_oPCMInfor[rhs_ePort].m_eLine);
	#elif (PCM_CODEC == WM8580)
	// I2SMULCDCLK Out for Master Clock of WM8580 using i2s multi channel port
	
	//I2SMULTI_Init(AUDIO_PORT0);
	//I2SMULTI_InitPort(AUDIO_PORT0);		
	//I2SMULTI_CDCLKOut(rhs_ePort, rhs_pInfor);	
	
	I2S_Init(rhs_ePort);
	I2S_SetCDCLKPort(rhs_ePort);
	I2S_CDCLKOut(rhs_ePort);
	WM8580_CodecInitPCMIn(g_oPCMInfor[rhs_ePort].m_ePCMMSBPosition, g_oPCMInfor[rhs_ePort].m_uPCMSync, Master, Word16, LINEIN, g_oPCMInfor[rhs_ePort].m_eLine);
	#else
	Assert(0);
	#endif
	
	PCM_PCMInPolling(rhs_ePort);	
}

static void PCM_DataOutPolling(AUDIO_PORT rhs_ePort)
{
	UART_Printf("\nListen to Sound via Speak Out Connector.\n");
	UART_Printf("Press any key to play.\n");
    	UART_Getc();	

	#if (PCM_CODEC == WM8753)		
	WM8753_CodecInitPCMOut(g_oPCMInfor[rhs_ePort].m_ePCMMSBPosition, g_oPCMInfor[rhs_ePort].m_uPCMSync, Master, Word16, g_oPCMInfor[rhs_ePort].m_eLine);
	#elif (PCM_CODEC == WM8580)
	WM8580_CodecInitPCMOut(g_oPCMInfor[rhs_ePort].m_ePCMMSBPosition, g_oPCMInfor[rhs_ePort].m_uPCMSync, Master, Word16, g_oPCMInfor[rhs_ePort].m_eLine);
	#else
	Assert(0);
	#endif
	PCM_PCMOutPolling(rhs_ePort);
}



static void PCMInOutPolling(AUDIO_PORT rhs_ePort)
{
	UART_Printf("Supply Sound to PCM CODEC via Line In Connector.\n");
   	UART_Printf("Press any key to record.\n");
    	UART_Getc();	
	
	#if (PCM_CODEC == WM8753)
	// I2SCDCLK Out for Master Clock of WM8753 using another audio port
	I2S_Init((AUDIO_PORT)(~rhs_ePort & 0x1));		
	I2S_InitPort((AUDIO_PORT)((~rhs_ePort) & 0x1));
	I2S_CDCLKOut((AUDIO_PORT)((~rhs_ePort) & 0x1));
	// Codec Initialization for WM8753
	WM8753_CodecInitPCMIn(g_oPCMInfor[rhs_ePort].m_ePCMMSBPosition, g_oPCMInfor[rhs_ePort].m_uPCMSync, Master, Word16,  LINEIN, g_oPCMInfor[rhs_ePort].m_eLine);
	#elif (PCM_CODEC == WM8990)
	// I2SCDCLK Out for Master Clock of WM8990 using another audio port
	I2S_Init((AUDIO_PORT)(~rhs_ePort & 0x1));		
	I2S_InitPort((AUDIO_PORT)((~rhs_ePort) & 0x1));
	I2S_CDCLKOut((AUDIO_PORT)((~rhs_ePort) & 0x1));	
	// Codec Initialization for WM8990
	WM8990_CodecInitPCMIn(g_oPCMInfor[rhs_ePort].m_ePCMMSBPosition, g_oPCMInfor[rhs_ePort].m_uPCMSync, Master, Word16,  LINEIN, g_oPCMInfor[rhs_ePort].m_eLine);
	#elif (PCM_CODEC == WM9713)
	// Codec Initialization for WM9713
	WM9713_CodecInitPCMIn(g_oPCMInfor[rhs_ePort].m_ePCMMSBPosition, g_oPCMInfor[rhs_ePort].m_uPCMSync, Master, Word16,  LINEIN, g_oPCMInfor[rhs_ePort].m_eLine);
	#elif (PCM_CODEC == WM8580)
	// I2SMULCDCLK Out for Master Clock of WM8580 using i2s multi channel port
	
	//I2SMULTI_Init(AUDIO_PORT0);
	//I2SMULTI_InitPort(AUDIO_PORT0);		
	//I2SMULTI_CDCLKOut(rhs_ePort, rhs_pInfor);	
	
	I2S_Init(rhs_ePort);
	I2S_SetCDCLKPort(rhs_ePort);
	I2S_CDCLKOut(rhs_ePort);
	WM8580_CodecInitPCMIn(g_oPCMInfor[rhs_ePort].m_ePCMMSBPosition, g_oPCMInfor[rhs_ePort].m_uPCMSync, Master, Word16, LINEIN, g_oPCMInfor[rhs_ePort].m_eLine);
	#else
	Assert(0);
	#endif
	
	PCM_PCMInPolling(rhs_ePort);

	UART_Printf("\nListen to Sound via Speak Out Connector.\n");
	UART_Printf("Press any key to play.\n");
    	UART_Getc();
	

	#if (PCM_CODEC == WM8753)		
	WM8753_CodecInitPCMOut(g_oPCMInfor[rhs_ePort].m_ePCMMSBPosition, g_oPCMInfor[rhs_ePort].m_uPCMSync, Master, Word16, g_oPCMInfor[rhs_ePort].m_eLine);
	#elif (PCM_CODEC == WM8580)
	WM8580_CodecInitPCMOut(g_oPCMInfor[rhs_ePort].m_ePCMMSBPosition, g_oPCMInfor[rhs_ePort].m_uPCMSync, Master, Word16, g_oPCMInfor[rhs_ePort].m_eLine);
	#else
	Assert(0);
	#endif
	PCM_PCMOutPolling(rhs_ePort);
}


static void PCM_DataInDMA(AUDIO_PORT rhs_ePort)
{
	UART_Printf("Supply Sound to PCM CODEC via Line In Connector.\n");
    	UART_Printf("Press any key to record.\n");    
    	UART_Getc();
		
   	UART_Printf("Recording...\n");
		

	#if (PCM_CODEC == WM8753)	
	// I2SCDCLK Out for Master Clock of WM8753 using another audio port
	I2S_Init((AUDIO_PORT)(~rhs_ePort & 0x1));		
	I2S_InitPort((AUDIO_PORT)((~rhs_ePort) & 0x1));
	I2S_CDCLKOut((AUDIO_PORT)((~rhs_ePort) & 0x1));
	// Codec Initialization for WM8753
	WM8753_CodecInitPCMIn(g_oPCMInfor[rhs_ePort].m_ePCMMSBPosition, g_oPCMInfor[rhs_ePort].m_uPCMSync, Master, Word16, LINEIN, SMDK_PCM);//set external codec
	#elif (PCM_CODEC == WM8990)
	if ( rhs_ePort == AUDIO_PORT0)
	{
		// I2SCDCLK Out for Master Clock of WM8990 using another audio port
		I2S_Init(rhs_ePort);		
		I2S_SetCDCLKPort(rhs_ePort);
		I2S_CDCLKOut(rhs_ePort);
		// Codec Initialization for WM8990
		WM8990_CodecInitPCMIn(g_oPCMInfor[rhs_ePort].m_ePCMMSBPosition, g_oPCMInfor[rhs_ePort].m_uPCMSync, Master, Word16, LINEIN, SMDK_I2S);//set external codec
	}
	else
	{
		UART_Printf("This path is not available for recoding!!\n");
		Assert(0);
	}
	#elif (PCM_CODEC == WM9713)
	WM9713_CodecInitPCMIn(g_oPCMInfor[rhs_ePort].m_ePCMMSBPosition, g_oPCMInfor[rhs_ePort].m_uPCMSync, Master, Word16, LINEIN, SMDK_PCM);//set external codec
	#elif (PCM_CODEC == WM8580)
	
	#if(WM8580MASTERCLK == CDCLK_I2SMULTI)
	I2SMULTI_Init(AUDIO_PORT0);
	I2SMULTI_InitPort(AUDIO_PORT0);	
	I2SMULTI_CDCLKOut(AUDIO_PORT0, I2SMULTI_GetI2SMULTIInfor(AUDIO_PORT0));
	
	#elif(WM8580MASTERCLK == CDCLK_I2SCDCLK)
	I2S_Init(rhs_ePort);
	I2S_SetCDCLKPort(rhs_ePort);
	I2S_CDCLKOut(rhs_ePort);
	#endif		
	WM8580_CodecInitPCMIn(g_oPCMInfor[rhs_ePort].m_ePCMMSBPosition, g_oPCMInfor[rhs_ePort].m_uPCMSync, Master, Word16, LINEIN, SMDK_PCM);
	#else
	Assert(0);
	#endif

	PCM_PCMInDMA(rhs_ePort);	//set internal interface controller	
}


static void PCM_DataOutDMA(AUDIO_PORT rhs_ePort)
{
	UART_Printf("\nListen to Sound via Speak Out Connector.\n");
	UART_Printf("Press any key to play.\n");
    	UART_Getc();   
			
	#if (PCM_CODEC == WM8753)
	// I2SCDCLK Out for Master Clock of WM8753 using another audio port
	I2S_Init((AUDIO_PORT)(~rhs_ePort & 0x1));		
	I2S_InitPort((AUDIO_PORT)((~rhs_ePort) & 0x1));
	I2S_CDCLKOut((AUDIO_PORT)((~rhs_ePort) & 0x1));
	// Codec Initialization for WM8753
	WM8753_CodecInitPCMOut(g_oPCMInfor[rhs_ePort].m_ePCMMSBPosition, g_oPCMInfor[rhs_ePort].m_uPCMSync, Master, Word16, SMDK_PCM);
	#elif (PCM_CODEC == WM8990)
	// I2SCDCLK Out for Master Clock of WM8990 using another audio port
	I2S_Init(AUDIO_PORT0);		
	I2S_SetCDCLKPort(AUDIO_PORT0);
	I2S_CDCLKOut(AUDIO_PORT0);
	// Codec Initialization for WM8990
	if ( rhs_ePort == AUDIO_PORT0)
		WM8990_CodecInitPCMOut(g_oPCMInfor[rhs_ePort].m_ePCMMSBPosition, g_oPCMInfor[rhs_ePort].m_uPCMSync, Master, Word16, SMDK_I2S);//set external codec
	else 
		WM8990_CodecInitPCMOut(g_oPCMInfor[rhs_ePort].m_ePCMMSBPosition, g_oPCMInfor[rhs_ePort].m_uPCMSync, Master, Word16, SMDK_PCM);//set external codec
	#elif (PCM_CODEC == WM8580)
	#if(WM8580MASTERCLK == CDCLK_I2SMULTI)
	I2SMULTI_Init(AUDIO_PORT0);
	I2SMULTI_InitPort(AUDIO_PORT0);	
	I2SMULTI_CDCLKOut(AUDIO_PORT0, I2SMULTI_GetI2SMULTIInfor(AUDIO_PORT0));	
	#elif(WM8580MASTERCLK == CDCLK_I2SCDCLK)
	I2S_Init(rhs_ePort);
	I2S_SetCDCLKPort(rhs_ePort);
	I2S_CDCLKOut(rhs_ePort);
	#endif		
	WM8580_CodecInitPCMOut(g_oPCMInfor[rhs_ePort].m_ePCMMSBPosition, g_oPCMInfor[rhs_ePort].m_uPCMSync, Master, Word16, SMDK_PCM);
	#else
	Assert(0);
	#endif 	
	PCM_PCMOutDMA(rhs_ePort);		
	UART_Printf("\nEnd of Play!\n");		
}


static void PCMInOutDMA(AUDIO_PORT rhs_ePort)
{
	UART_Printf("Supply Sound to PCM CODEC via Line In Connector.\n");
    	UART_Printf("Press any key to record.\n");    
    	UART_Getc();
		
   	UART_Printf("Recording...\n");
		

	#if (PCM_CODEC == WM8753)	
	PCM_CLKOUT(rhs_ePort);
	WM8753_CodecInitPCMIn(g_oPCMInfor[rhs_ePort].m_ePCMMSBPosition, g_oPCMInfor[rhs_ePort].m_uPCMSync, Master, Word16, LINEIN, SMDK_PCM);//set external codec
	#elif (PCM_CODEC == WM8990)
	if ( rhs_ePort == AUDIO_PORT0)
	{
		WM8990_CodecInitPCMIn(g_oPCMInfor[rhs_ePort].m_ePCMMSBPosition, g_oPCMInfor[rhs_ePort].m_uPCMSync, Master, Word16, LINEIN, SMDK_I2S);//set external codec
	}
	else
	{
		UART_Printf("This path is not available for recoding!!\n");
		Assert(0);
	}
	#elif (PCM_CODEC == WM9713)
	WM9713_CodecInitPCMIn(g_oPCMInfor[rhs_ePort].m_ePCMMSBPosition, g_oPCMInfor[rhs_ePort].m_uPCMSync, Master, Word16, LINEIN, SMDK_PCM);//set external codec
	#elif (PCM_CODEC == WM8580)
	
	#if(WM8580MASTERCLK == CDCLK_I2SMULTI)
	I2SMULTI_Init(AUDIO_PORT0);
	I2SMULTI_InitPort(AUDIO_PORT0);	
	I2SMULTI_CDCLKOut(AUDIO_PORT0, I2SMULTI_GetI2SMULTIInfor(AUDIO_PORT0));
	
	#elif(WM8580MASTERCLK == CDCLK_I2SCDCLK)
	I2S_Init(rhs_ePort);
	I2S_SetCDCLKPort(rhs_ePort);
	I2S_CDCLKOut(rhs_ePort);
	#endif		
	WM8580_CodecInitPCMIn(g_oPCMInfor[rhs_ePort].m_ePCMMSBPosition, g_oPCMInfor[rhs_ePort].m_uPCMSync, Master, Word16, LINEIN, SMDK_PCM);
	#else
	Assert(0);
	#endif

	PCM_PCMInDMA(rhs_ePort);	//set internal interface controller

	UART_Printf("\nListen to Sound via Speak Out Connector.\n");
	UART_Printf("Press any key to play.\n");
    	UART_Getc();   
			
	#if (PCM_CODEC == WM8753)		
	WM8753_CodecInitPCMOut(g_oPCMInfor[rhs_ePort].m_ePCMMSBPosition, g_oPCMInfor[rhs_ePort].m_uPCMSync, Master, Word16, SMDK_PCM);
	#elif (PCM_CODEC == WM8990)
	if ( rhs_ePort == AUDIO_PORT0)
		WM8990_CodecInitPCMOut(g_oPCMInfor[rhs_ePort].m_ePCMMSBPosition, g_oPCMInfor[rhs_ePort].m_uPCMSync, Master, Word16, SMDK_I2S);//set external codec
	else 
		WM8990_CodecInitPCMOut(g_oPCMInfor[rhs_ePort].m_ePCMMSBPosition, g_oPCMInfor[rhs_ePort].m_uPCMSync, Master, Word16, SMDK_PCM);//set external codec
	#elif (PCM_CODEC == WM8580)
	WM8580_CodecInitPCMOut(g_oPCMInfor[rhs_ePort].m_ePCMMSBPosition, g_oPCMInfor[rhs_ePort].m_uPCMSync, Master, Word16, SMDK_PCM);
	#else
	Assert(0);
	#endif 	
	PCM_PCMOutDMA(rhs_ePort);		
	UART_Printf("\nEnd of Play!\n");		
}

static void PCM_BypassTestDMA(AUDIO_PORT rhs_ePort)
{
	UART_Printf("Supply Sound to PCM CODEC via Line In Connector.\n");
	UART_Printf("\nListen to Sound via Speak Out Connector.\n");
    	UART_Printf("Press any key to record.\n");    
    	UART_Getc();
		
   	PCM_CLKOUT(rhs_ePort);		
			
	#if (PCM_CODEC == WM8753)		
		WM8753_CodecInitPCMOut(g_oPCMInfor[rhs_ePort].m_ePCMMSBPosition, g_oPCMInfor[rhs_ePort].m_uPCMSync, Master, Word16, SMDK_PCM);
	#elif (PCM_CODEC == WM8990)
		if ( rhs_ePort == AUDIO_PORT0)
			WM8990_CodecInitPCMOut(g_oPCMInfor[rhs_ePort].m_ePCMMSBPosition, g_oPCMInfor[rhs_ePort].m_uPCMSync, Master, Word16, SMDK_I2S);//set external codec
		else 
			WM8990_CodecInitPCMOut(g_oPCMInfor[rhs_ePort].m_ePCMMSBPosition, g_oPCMInfor[rhs_ePort].m_uPCMSync, Master, Word16, SMDK_PCM);//set external codec
	#else
	Assert(0);
	#endif 
	PCM_PCMOutDMA(rhs_ePort);	
		
	UART_Printf("\nEnd of Play!\n");
	
	PCM_CodecExitPCMOut();	
}

static void PCMInOutInt(AUDIO_PORT rhs_ePort)
{
	UART_Printf("Supply Sound to PCM CODEC via Line In Connector.\n");
   	UART_Printf("Press any key to record.\n");
    	UART_Getc();
   	UART_Printf("Recording...\n");		
   	
	
	
	#if (PCM_CODEC == WM8753)

		
		PCM_CLKOUT(rhs_ePort);
		WM8753_CodecInitPCMIn(g_oPCMInfor[rhs_ePort].m_ePCMMSBPosition, g_oPCMInfor[rhs_ePort].m_uPCMSync, Master, Word16,  LINEIN, g_oPCMInfor[rhs_ePort].m_eLine);
		
	#elif (PCM_CODEC == WM8990)
		PCM_CLKOUT(rhs_ePort);
		WM8990_CodecInitPCMIn(g_oPCMInfor[rhs_ePort].m_ePCMMSBPosition, g_oPCMInfor[rhs_ePort].m_uPCMSync, Master, Word16,  LINEIN, SMDK_I2S);		
	#elif (PCM_CODEC == WM9713)
		WM9713_CodecInitPCMIn(g_oPCMInfor[rhs_ePort].m_ePCMMSBPosition, g_oPCMInfor[rhs_ePort].m_uPCMSync, Master, Word16,  LINEIN, g_oPCMInfor[rhs_ePort].m_eLine);
	#else
	Assert(0);
	#endif
	
	PCM_PCMInInt(rhs_ePort);
	PCM_CodecExitPCMIn(rhs_ePort);

	UART_Printf("\nListen to Sound via Speak Out Connector.\n");
	UART_Printf("Press any key to play.\n");
    	UART_Getc();
	UART_Printf("Play...\n");
	#if (PCM_CODEC == WM8753)
	PCM_CLKOUT(rhs_ePort);
	WM8753_CodecInitPCMOut(g_oPCMInfor[rhs_ePort].m_ePCMMSBPosition, g_oPCMInfor[rhs_ePort].m_uPCMSync, Master, Word16, g_oPCMInfor[rhs_ePort].m_eLine);
	#else
	Assert(0);
	#endif
	PCM_PCMOutInt(rhs_ePort);		
	PCM_CodecExitPCMOut();	
}

void __irq ISR_PCM_INTCHECK(void)
{
	u32 uSclkDiv, uSyncDiv;
	u32 uFIFOStat, uRXFIFOLev, uTXFIFOLev;
	
	PCM_ClearInt(AUDIO_PORT0);
	PCM_ClearInt(AUDIO_PORT1);
	INTC_ClearVectAddr();
	UART_Printf("\nInterrupt!!\n");
	Disp("Before Clearing\n");
	Disp("Port 0(Int)\n");
	UART_Printf("IRQCTL : 	0x%x\t",PCMInp32(AUDIO_PORT0, rPCMIRQCTL));
	UART_Printf("IRQSTAT : 	0x%x\t",PCMInp32(AUDIO_PORT0, rPCMIRQSTAT));
	uFIFOStat = PCMInp32(AUDIO_PORT0, rPCMFIFOSTAT);
	uRXFIFOLev = (uFIFOStat & 0x3f0) >> 4;
	uTXFIFOLev = (uFIFOStat & 0xfc000) >> 14;
	UART_Printf("FIFOSTAT :	0x%x\n",uFIFOStat);
	UART_Printf("RXFIFO Level :	0x%x\n", uRXFIFOLev);
	UART_Printf("TXFIFO Level :	0x%x\n", uTXFIFOLev);
	Disp("Port 1(Int)\n");
	UART_Printf("IRQCTL : 	0x%x\t",PCMInp32(AUDIO_PORT1, rPCMIRQCTL));
	UART_Printf("IRQSTAT : 	0x%x\t",PCMInp32(AUDIO_PORT1, rPCMIRQSTAT));
	uFIFOStat = PCMInp32(AUDIO_PORT1, rPCMFIFOSTAT);
	uRXFIFOLev = (uFIFOStat & 0x3f0) >> 4;
	uTXFIFOLev = (uFIFOStat & 0xfc000) >> 14;
	UART_Printf("FIFOSTAT :	0x%x\n",uFIFOStat);
	UART_Printf("RXFIFO Level :	0x%x\n", uRXFIFOLev);
	UART_Printf("TXFIFO Level :	0x%x\n", uTXFIFOLev);
	//service
	//PCM Clock Setting 
	//PCM_CLKOUT(AUDIO_PORT0);
	//PCM_GetClkValAndClkDir(AUDIO_PORT0, &uSclkDiv, &uSyncDiv);
	//PCMCLKCTL_SetDivideLogic(AUDIO_PORT0, uSclkDiv, uSyncDiv);
	
	INTC_Disable(g_oPCMInfor[AUDIO_PORT0].m_uNumInt);	
	PCMIRQCTL_EnableInt(AUDIO_PORT0, false);
	PCMIRQCTL_SetIRQMode(AUDIO_PORT0, PCMIRQALL, false);						// Disable another Interrupt Condition
	INTC_Disable(g_oPCMInfor[AUDIO_PORT1].m_uNumInt);
	PCMIRQCTL_EnableInt(AUDIO_PORT1, false);
	PCMIRQCTL_SetIRQMode(AUDIO_PORT1, PCMIRQALL, false);						// Disable another Interrupt Condition
	INTC_ClearVectAddr();
	g_isrTest = 1;

	PCMCLKCTL_ActiveDivideLogic(AUDIO_PORT0, DISABLE);	

	Disp("After Clearing\n");
	Disp("Port 0(Int)\n");
	UART_Printf("IRQCTL : 	0x%x\t",PCMInp32(AUDIO_PORT0, rPCMIRQCTL));
	UART_Printf("IRQSTAT : 	0x%x\t",PCMInp32(AUDIO_PORT0, rPCMIRQSTAT));
	UART_Printf("FIFOSTAT :	0x%x\n",PCMInp32(AUDIO_PORT0, rPCMFIFOSTAT));
	Disp("Port 1(Int)\n");
	UART_Printf("IRQCTL : 	0x%x\t",PCMInp32(AUDIO_PORT1, rPCMIRQCTL));
	UART_Printf("IRQSTAT : 	0x%x\t",PCMInp32(AUDIO_PORT1, rPCMIRQSTAT));
	UART_Printf("FIFOSTAT :	0x%x\n",PCMInp32(AUDIO_PORT1, rPCMFIFOSTAT));	
}

static void PCM_InterruptTest(AUDIO_PORT rhs_ePort)
{
	u32 uTempRegister;
	u32 uSclkDiv, uSyncDiv;
	u32 uFIFOCNT = 0;
	u32 uRegValue;
	u16* uRecBuf = g_oPCMInfor[rhs_ePort].m_uRecBufferStartAddr;
	s32 uPcmDataSize = g_oPCMInfor[rhs_ePort].m_uPcmSize;
	u32 uFIFOTriggerLevel = 0x8;	
	u32 uCnt;
	u32 uFIFOStat, uRXFIFOLev, uTXFIFOLev;

	PCMCTL_EnablePCMIF(rhs_ePort, false);
	PCMCTL_EnableFIFO(rhs_ePort, PCM_TXRX, false);
	PCMIRQCTL_EnableInt(rhs_ePort, false);  
	PCMIRQCTL_SetIRQMode(rhs_ePort, PCMIRQALL, false);						// Disable another Interrupt Condition
	PCM_ClearInt(rhs_ePort);
	
	UART_Printf("\nPress any key to Start Test.\n");
	UART_Getc();
	
	
	INTC_ClearVectAddr();	
	INTC_SetVectAddr(g_oPCMInfor[rhs_ePort].m_uNumInt, ISR_PCM_INTCHECK);
		
	//PCM Clock Setting 
	PCM_CLKOUT(rhs_ePort);
	PCM_GetClkValAndClkDir(rhs_ePort, &uSclkDiv, &uSyncDiv);
	PCMCLKCTL_SetDivideLogic(rhs_ePort, uSclkDiv, uSyncDiv);
	PCMCLKCTL_ActiveDivideLogic(rhs_ePort, ENABLE);	
	PCMIRQCTL_SetIRQMode(rhs_ePort, RXFIFO_ALMOST_EMPTY, true);					// Set Irq condition when RX FIFO ALMOST Full	
	PCMCTL_EnableFIFO(rhs_ePort, PCM_RX, ENABLE);	
	PCMCTL_EnablePCMIF(rhs_ePort, ENABLE);	
	#if 0
	for (uCnt = 0;uCnt<15;uCnt++)
		PCMOutp16(rhs_ePort, rPCMRXFIFO,(uCnt));	
	#elif 1
	Delay(2000);
	PCMCTL_EnablePCMIF(rhs_ePort, DISABLE);
	#endif
	Disp("Test Condition\n");
	Disp("Port 0\n");
	UART_Printf("IRQCTL : 	0x%x\t",PCMInp32(AUDIO_PORT0, rPCMIRQCTL));
	UART_Printf("IRQSTAT : 	0x%x\t",PCMInp32(AUDIO_PORT0, rPCMIRQSTAT));
	uFIFOStat = PCMInp32(AUDIO_PORT0, rPCMFIFOSTAT);
	uRXFIFOLev = (uFIFOStat & 0x3f0) >> 4;
	uTXFIFOLev = (uFIFOStat & 0xfc000) >> 14;
	UART_Printf("FIFOSTAT :	0x%x\n",uFIFOStat);
	UART_Printf("RXFIFO Level :	0x%x\n", uRXFIFOLev);
	UART_Printf("TXFIFO Level :	0x%x\n", uTXFIFOLev);
	Disp("Port 1\n");
	UART_Printf("IRQCTL : 	0x%x\t",PCMInp32(AUDIO_PORT1, rPCMIRQCTL));
	UART_Printf("IRQSTAT : 	0x%x\t",PCMInp32(AUDIO_PORT1, rPCMIRQSTAT));
	uFIFOStat = PCMInp32(AUDIO_PORT0, rPCMFIFOSTAT);
	uRXFIFOLev = (uFIFOStat & 0x3f0) >> 4;
	uTXFIFOLev = (uFIFOStat & 0xfc000) >> 14;
	UART_Printf("FIFOSTAT :	0x%x\n",uFIFOStat);
	UART_Printf("RXFIFO Level :	0x%x\n", uRXFIFOLev);
	UART_Printf("TXFIFO Level :	0x%x\n", uTXFIFOLev);
	
	g_isrTest = 0;
	PCMIRQCTL_EnableInt(rhs_ePort, ENABLE);
	INTC_Enable(g_oPCMInfor[rhs_ePort].m_uNumInt);	
	while(!(g_isrTest))
	{
		#if 0
		*uRecBuf = PCMInp16(rhs_ePort, rPCMTXFIFO);	//Data Transfer
		#elif 0
		PCMOutp16(rhs_ePort, rPCMTXFIFO,*uRecBuf);		
		#elif 1
		*uRecBuf++ = PCMInp16(rhs_ePort, rPCMRXFIFO);	//Data Transfer		
		#elif 0
		PCMOutp16(rhs_ePort, rPCMRXFIFO,*uRecBuf);		
		#endif
		UART_Printf("RXFIFO Level :	0x%x\n", ((PCMInp32(rhs_ePort, rPCMFIFOSTAT) & 0x3f0) >> 4));
	}
	PCM_ClearInt(rhs_ePort);
	INTC_ClearVectAddr();
	PCMIRQCTL_EnableInt(rhs_ePort, false);  
	INTC_Disable(g_oPCMInfor[rhs_ePort].m_uNumInt);	
	PCMCTL_EnablePCMIF(rhs_ePort, false);
	PCMCTL_EnableFIFO(rhs_ePort, PCM_TXRX, false);
	PCMIRQCTL_EnableInt(rhs_ePort, false);  
	PCMIRQCTL_SetIRQMode(rhs_ePort, PCMIRQALL, false);						// Disable another Interrupt Condition
	PCM_ClearInt(rhs_ePort);
	PCMCLKCTL_ActiveDivideLogic(rhs_ePort, FALSE);	
	UART_Printf("\nTest Done\n");
}

static void PCMInAC97Out(AUDIO_PORT rhs_ePort)
{
	UART_Printf("This Test is available when you use WM9713 Codec(AC97)\n");
	Delay(5000);
	
	#if (PCM_CODEC == WM9713)
	
		UART_Printf("PCM and AC97 operation are DMA mode\n");
		UART_Printf("Press any key to Continue.\n");
	    	UART_Getc();
		//Encoding by PCM ADC	
		PCM_CodecInitPCMIn(g_oPCMInfor[rhs_ePort].m_uPCMSync, g_oPCMInfor[rhs_ePort].m_ePCMMSBPosition, g_oPCMInfor[rhs_ePort].PCMSClk, g_oPCMInfor[rhs_ePort].PCMClkSrc);			//set external codec
		PCM_PCMInDMA(rhs_ePort);					//set internal interface controller
		PCM_CodecExitPCMIn(rhs_ePort);			//set DMA 
		//Decoding by AC97 DAC
		if(AC97_InitACLINK())
		{
			AC97_InitCodecPCMOut(g_oPCMInfor[rhs_ePort].m_uPCMSync);
			AC97_MICOutDMA(g_oPCMInfor[rhs_ePort].m_uRecBufferStartAddr, g_oPCMInfor[rhs_ePort].m_uPcmSize);
		}
		
	#else
		UART_Printf("Present Codec is not WM9713(AC97) Codec!!\n");
	#endif
}

static void AC97InPCMOut(AUDIO_PORT rhs_ePort)
{
	UART_Printf("This Test is available when you use WM9713 Codec(AC97)\n");
	Delay(5000);
	
	#if (PCM_CODEC == WM9713)
		UART_Printf("PCM and AC97 operation are DMA mode\n");
		UART_Printf("Press any key to Continue.\n");
	    	UART_Getc();
		
		if(AC97_InitACLINK())
		{	
			//Encoding by AC97 ADC
			AC97_InitCodecMICIn(g_oPCMInfor[rhs_ePort].m_uPCMSync);
			AC97_MICInDMA(g_oPCMInfor[rhs_ePort].m_uRecBufferStartAddr, g_oPCMInfor[rhs_ePort].m_uPcmSize);
			//Decoding by PCM DAC
			PCM_CodecInitPCMOut(g_oPCMInfor[rhs_ePort].m_uPCMSync, g_oPCMInfor[rhs_ePort].m_ePCMMSBPosition, g_oPCMInfor[rhs_ePort].m_uPCMSClk, g_oPCMInfor[rhs_ePort].PCMClkSrc);							 
			PCM_PCMOutDMA(g_oPCMInfor[rhs_ePort].m_uRecBufferStartAddr, g_oPCMInfor[rhs_ePort].m_uPcmSize);	
		}
	
	#else
		UART_Printf("Present Codec is not WM9713(AC97) Codec!!\n");
	
	#endif
}


