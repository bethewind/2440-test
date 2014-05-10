/********************************************************************************
* 
*	Project Name : S3C6410 Validation
*
*	Copyright 2008 by Samsung Electronics, Inc.
*	All rights reserved.
*
*	Project Description :
*		This software is only for verifying functions of the S3C6400.
*		Anybody can use this software without our permission.
*  
*--------------------------------------------------------------------------------
* 
*	File Name : AC97_test.c
*  
*	File Description :
*
*	Author	: Sung-Hyun, Na
*	Dept. : AP Development Team
*	Created Date : 2008/02/19
*	Version : 0.1
* 
*	History
*	- Version 0.1 (Sung-Hyun.Na, 2008/02/19)
*	  -> Working On SMDK6400 Rev 0.2 Board
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
#include "timer.h"

#include "ac97.h"
#include "audio.h"

//Test Functions
void AC97_Test(void);
void AC97_Configuration(AC97_Infor* rhs_pInfor);
static void AC97_Powerdown(AC97_Infor* rhs_pInfor);
static void AC97_CodecRegisterRW(AC97_Infor* rhs_pInfor);

static void AC97_PCMInOutDMAMODE(AC97_Infor* rhs_pInfor);
static void AC97_PCMInOutIntMODE(AC97_Infor* rhs_pInfor);

static void AC97_MICInDMAMODE(AC97_Infor* rhs_pInfor);
static void AC97_MICInIntMODE(AC97_Infor* rhs_pInfor);


AC97_Infor 		g_AC97Infor;

void AC97_Test(void)
{	
	const AC97testFuncMenu ac97_function[]=
	{
		0,         						"Exit\n",
		AC97_Configuration,			"AC97 Controller Configuration",
		AC97_Powerdown,				"AC97 Power Down Test",
		AC97_CodecRegisterRW,		"AC97 Calculate Delay Time\n",
		AC97_PCMInOutIntMODE,		"AC97 PCM In/Out (via Slot3, 4) Interrupt Mode Test",
		AC97_MICInIntMODE,			"AC97 MIC In (via Slot 6) Interrupt Mode Test\n",
		AC97_PCMInOutDMAMODE,	"AC97 PCM In/Out (via Slot3, 4) DMA Mode Test",
		AC97_MICInDMAMODE,		"AC97 MIC In (via Slot 6) DMA Mode Test",
		
		0,							 0
	};
	AC97_Infor* pInfor;
	AUDIO_PORT ePort = AUDIO_PORT0;
	
	AC97_Init(); 
	AC97_SetPort(AUDIO_PORT0);
	AC97_ClosePort(AUDIO_PORT1);
	Delay(1000);
	pInfor = AC97_GetAC97Infor(AUDIO_PORT0);
	
	while (1)
	{
		u32 i, nSel;
		UART_Printf("===================================================================\n");
		UART_Printf("Port : %d\t",ePort);
		UART_Printf("Sampling Frequency : %3.1fKHz\n",(double)(g_AC97Infor.m_uSamplingFreq)/1000);

		UART_Printf("PCM Out Int source : ");
		if ( g_AC97Infor.m_ePCMOutIntMode == PCMOUT_UNDERRUN_INT)
			UART_Printf("Under Run\n");
		else 
			UART_Printf("Threshold\n");
		
		UART_Printf("PCM In Int Source : ");
		if ( g_AC97Infor.m_ePCMOutIntMode == PCMIN_OVERRUN_INT)
			UART_Printf("Over Run\n");
		else 
			UART_Printf("Threshold\n");
		
		UART_Printf("MIC In Int Source : ");
		if ( g_AC97Infor.m_ePCMOutIntMode == MICIN_OVERRUN_INT)
			UART_Printf("Over Run\n");
		else 
			UART_Printf("Threshold\n");		
		UART_Printf("\n");
		
		UART_Printf("======================== AC97 Tes Function ========================\n\n");
		for (i=0; (int)(ac97_function[i].desc)!=0; i++)
		{
			UART_Printf("%2d: %s\n", i,ac97_function[i].desc);
		}
		UART_Printf("\n%2d: Change Port\n",i++);
		UART_Printf("==================================================================\n");	
		UART_Printf("\nSelect the function to test : ");

		nSel = UART_GetIntNum();

		UART_Printf("\n");

		if (nSel == 0)
			break;
		if (nSel>0 && nSel<(sizeof(ac97_function)/8-1))
			 (ac97_function[nSel].func)(pInfor);
		else if ( nSel == (sizeof(ac97_function)/8-1))
		{	
			if (ePort == AUDIO_PORT0)
			{
				AC97_ClosePort(AUDIO_PORT0);
				AC97_SetPort(AUDIO_PORT1);				
				ePort = AUDIO_PORT1;
			}
			else 
			{
				AC97_ClosePort(AUDIO_PORT1);
				AC97_SetPort(AUDIO_PORT0);				
				ePort = AUDIO_PORT0;
			}
		}
	}
	
}

void AC97_Configuration(AC97_Infor* rhs_pInfor)
{
	u32 uCountFunc=0;
	s32 iSel=0;	
	const AC97testFuncMenu aAC97ConfigFunc[] =
	{
		//AC97 Configuration Item
		AC97_SelectSamplingRate,		"Sampling Frequency(VRA On/Off) ",	
		AC97_SelectIntrMode,			"Interrupt Source ",				
	    	0,0
	};
	UART_Printf("\n\n================== AC97 Configuration =====================\n\n");
	while(1)
	{
		for (uCountFunc=0; (u32)(aAC97ConfigFunc[uCountFunc].desc)!=0; uCountFunc++)
			UART_Printf("%2d: %s\n", uCountFunc, aAC97ConfigFunc[uCountFunc].desc);

		UART_Printf("\nSelect the function to test : ");
		iSel =UART_GetIntNum();
		UART_Printf("\n");
		if(iSel == -1)
		{
			break;
		}
		(aAC97ConfigFunc[iSel].func) (rhs_pInfor);
		
	}
}

static void AC97_Powerdown(AC97_Infor* rhs_pInfor)
{
	u32 uGlobalCon;

	if(AC97_InitACLINK())
	{
		//Normal
	 	UART_Printf("\n=>Normal\n");
	 	AC97_ControllerState();
	 	UART_Printf("AC97 Codec Powerdown Ctrl/Stat Reg. Value (at 0x26): 0x%x\n", AC97_CodecCmd(READ,0x26,0x0000));
		UART_Getc();
	 	
	 	//ADCs off
	 	UART_Printf("\n=>ADCs off PR0\n");
		AC97_CodecCmd(WRITE,0x26,(1<<8));
	 	AC97_ControllerState();
		UART_Printf("AC97 Codec Powerdown Ctrl/Stat Reg. Value (at 0x26): 0x%x\n", AC97_CodecCmd(READ,0x26,0x0000));
	
		UART_Getc();
	 	
	 	//DACs off
	 	UART_Printf("\n=>DACs off PR1\n");
	 	AC97_CodecCmd(WRITE,0x26,(1<<8)|(1<<9));
	 	AC97_ControllerState();
	 	UART_Printf("AC97 Codec Powerdown Ctrl/Stat Reg. Value (at 0x26): 0x%x\n", AC97_CodecCmd(READ,0x26,0x0000));	
		UART_Getc();
	 	
	 	//Analog off
	 	UART_Printf("\n=>Analog off PR2 & PR3\n");
	 	AC97_CodecCmd(WRITE,0x26,(1<<8)|(1<<9)|(1<<10));
	 	AC97_ControllerState();
	 	UART_Printf("AC97 Codec Powerdown Ctrl/Stat Reg. Value (at 0x26): 0x%x\n", AC97_CodecCmd(READ,0x26,0x0000));	
	 	UART_Getc();
	 	
	 	//Digital I/F off
	 	UART_Printf("\n=>Digital I/F off PR4\n");
	 	AC97_CodecCmd(WRITE,0x26,(1<<8)|(1<<9)|(1<<10)|(1<<12));
	 	AC97_ControllerState();
	 
	 	//Shut off AC-Link
	 	UART_Printf("\n=>Shut off AC-Link\n");								//Hold AC-link data
	 	uGlobalCon= Inp32(AC97_BASE+rACGLBCTRL);
	 	//AC97Outp32(rACGLBCTRL, uGlobalCon & ~(1<<2));		//Hold AC-Link Data
	 	AC97Outp32(rACGLBCTRL, uGlobalCon & ~((1<<2)|(1<<3)));	//Don't Hold AC -Link Data
	 	AC97_ControllerState();

	 	UART_Printf("\nPress enter key for Warm Reset\n");
	 	UART_Getc();
		
		//Cold Reset Test;
		AC97_ColdReset();
	
		//Warm Reset
		AC97_WarmReset();


		/*
		//Normal
	 	UART_Printf("\n=>Normal\n");
	 	AC97_ControllerState();
	 	UART_Printf("AC97 Codec Powerdown Ctrl/Stat Reg. Value (at 0x26): 0x%x\n", AC97_CodecCmd(READ,0x26,0x0000));
		UART_Getc();
	 	
	 	//ADCs off
	 	UART_Printf("\n=>ADCs off PR0\n");
		AC97_CodecCmd(WRITE,0x26,(1<<8));
	 	AC97_ControllerState();
		UART_Printf("AC97 Codec Powerdown Ctrl/Stat Reg. Value (at 0x26): 0x%x\n", AC97_CodecCmd(READ,0x26,0x0000));
	
		UART_Getc();
	 	
	 	//DACs off
	 	UART_Printf("\n=>DACs off PR1\n");
	 	AC97_CodecCmd(WRITE,0x26,(1<<8)|(1<<9));
	 	AC97_ControllerState();
	 	UART_Printf("AC97 Codec Powerdown Ctrl/Stat Reg. Value (at 0x26): 0x%x\n", AC97_CodecCmd(READ,0x26,0x0000));	
		UART_Getc();
	 	
	 	//Analog off
	 	UART_Printf("\n=>Analog off PR2 & PR3\n");
	 	AC97_CodecCmd(WRITE,0x26,(1<<8)|(1<<9)|(1<<10));
	 	AC97_ControllerState();
	 	UART_Printf("AC97 Codec Powerdown Ctrl/Stat Reg. Value (at 0x26): 0x%x\n", AC97_CodecCmd(READ,0x26,0x0000));	
	 	UART_Getc();
	 	
	 	//Digital I/F off
	 	UART_Printf("\n=>Digital I/F off PR4\n");
	 	AC97_CodecCmd(WRITE,0x26,(1<<8)|(1<<9)|(1<<10)|(1<<12));
	 	AC97_ControllerState();
	 
	 	//Shut off AC-Link
	 	UART_Printf("\n=>Shut off AC-Link\n");							//Hold AC-link data
	 	uGlobalCon= Inp32(AC97_BASE+rACGLBCTRL);
	 	AC97Outp32(rACGLBCTRL, uGlobalCon & ~(1<<2));
	 	AC97_ControllerState();

	 	UART_Printf("\nPress enter key for Warm Reset\n");
	 	UART_Getc();
	
		//Warm Reset
		AC97_WarmReset();	
		*/
	}
			
}

static void AC97_PCMInOutDMAMODE(AC97_Infor* rhs_pInfor)
{
	if(AC97_InitACLINK())
	{	
		UART_Printf("Supply Sound to AC97 CODEC via Line In Connector.\n");
		UART_Printf("Press any key to record.\n");
		UART_Getc();
		UART_Printf("Recording...\n");	
		
		WM9713_CodecInitPCMIn(ACLinkSlot34, rhs_pInfor->m_uSamplingFreq, Master, Word16, LINEIN, SMDK_AC97);	
		AC97_PCMInDMA(rhs_pInfor);		

		UART_Printf("\nListen to Sound via Speak Out Connector.\n");
		UART_Printf("Press any key to play.\n");
		UART_Getc();
		UART_Printf("\nNow Play...\n");
		UART_Printf("To Volume Up, Press the 'u' key.\n");
		UART_Printf("To Volume Down, Press the 'd' key.\n");
		UART_Printf("To Command  Codec Manually,  Press the 's' key.\n");
		UART_Printf("\nIf you want to exit, Press the 'x' key.\n");
		UART_Printf("Headphone Volume Register = 0x%x\n", rhs_pInfor->m_uOutputVolume);
		
		WM9713_InitPCMOut(ACLinkSlot34, rhs_pInfor->m_uSamplingFreq, Master, Word16, SMDK_AC97);		
		AC97_PCMOutDMA(rhs_pInfor);	
		
	}
	AC97_ExitCodecPCMIn(1);
}

static void AC97_PCMInOutIntMODE(AC97_Infor* rhs_pInfor)
{
	if(AC97_InitACLINK())
	{	
		UART_Printf("Supply Sound to AC97 CODEC via Line In Connector.\n");
	   	UART_Printf("Press any key to record.\n");

		UART_Getc();
	   	UART_Printf("Recording...\n");	
		
		WM9713_CodecInitPCMIn(ACLinkSlot34, rhs_pInfor->m_uSamplingFreq, Master, Word16, LINEIN, SMDK_AC97);			
		AC97_PCMInINT(rhs_pInfor);

		UART_Printf("\nListen to Sound via Speak Out Connector.\n");
		UART_Printf("Press any key to play.\n");
	    	UART_Getc();
	   	UART_Printf("\nNow Play...\n");
		UART_Printf("Headphone Volume Register = 0x%x\n", rhs_pInfor->m_uOutputVolume);
		
		WM9713_InitPCMOut(ACLinkSlot34, rhs_pInfor->m_uSamplingFreq, Master, Word16, SMDK_AC97);
		AC97_PCMOutINT(rhs_pInfor);
				
	}
	AC97_ExitCodecPCMIn(1);
}


static void AC97_MICInIntMODE(AC97_Infor* rhs_pInfor)
{
	if(AC97_InitACLINK())
	{	
		
		UART_Printf("Supply Sound to AC97 CODEC via Line In Connector.\n");
		UART_Printf("Press any key to record.\n");

		UART_Getc();
	   	UART_Printf("Recording...\n");	
		
		WM9713_CodecInitPCMIn(ACLinkSlot6, rhs_pInfor->m_uSamplingFreq, Master, Word16, LINEIN, SMDK_AC97);
		//AC97_InitCodecMICIn(rhs_pInfor->m_uSamplingFreq);
		AC97_MICInINT(rhs_pInfor);

		UART_Printf("\nListen to Sound via Speak Out Connector.\n");
		UART_Printf("Press any key to play.\n");
	    	UART_Getc();
	   	UART_Printf("\nNow Play...\n");
		UART_Printf("Headphone Volume Register = 0x%x\n", rhs_pInfor->m_uOutputVolume);

		WM9713_InitPCMOut(ACLinkSlot34, rhs_pInfor->m_uSamplingFreq, Master, Word16, SMDK_AC97);
		AC97_PCMOutINT(rhs_pInfor);		
			
	}
	AC97_ExitCodecMICIn(1);
}

static void AC97_MICInDMAMODE(AC97_Infor* rhs_pInfor)
{
	if(AC97_InitACLINK())
	{
		UART_Printf("Supply Sound to AC97 CODEC via Line In Connector.\n");
		WM9713_CodecInitPCMIn(ACLinkSlot6, rhs_pInfor->m_uSamplingFreq, Master, Word16, LINEIN, SMDK_AC97);		
		AC97_MICInDMA(rhs_pInfor);

		UART_Printf("\nListen to Sound via Speak Out Connector.\n");
		UART_Printf("Press any key to play.\n");
		UART_Getc();
		UART_Printf("\nNow Play...\n");
		UART_Printf("To Volume Up, Press the 'u' key.\n");
		UART_Printf("To Volume Down, Press the 'd' key.\n");
		UART_Printf("To Command  Codec Manually,  Press the 's' key.\n");
		UART_Printf("\nIf you want to exit, Press the 'x' key.\n");
		UART_Printf("Headphone Volume Register = 0x%x\n", rhs_pInfor->m_uOutputVolume);
		
		WM9713_InitPCMOut(ACLinkSlot34, rhs_pInfor->m_uSamplingFreq, Master, Word16, SMDK_AC97);
		AC97_MICOutDMA(rhs_pInfor);
	}
	AC97_ExitCodecMICIn(1);
}


static void AC97_CodecRegisterRW(AC97_Infor* rhs_pInfor)
{
	u32 uCmdOffset = 0x26;
	u16 uCmdData = 0x4f00;
	u32 uDebugdelay = 1;
	u32 uState = 0;
	u16 uCodecStat;
	
	if(AC97_InitACLINK())
	{
		AC97_CodecCmd(WRITE,0x23,0xf);
		AC97_CodecCmd(READ,0x23,0x0);
		
		while(1)
		{	
			//Write
			AC97Outp32(rACCODECCMD, (0<<23)|(uCmdOffset<<16)|(uCmdData<<0));
			DelayfrTimer(micro,uDebugdelay);			
			
			//Read
			AC97Outp32(rACCODECCMD, (1<<23)|(uCmdOffset<<16)|(0<<0));
			DelayfrTimer(micro,uDebugdelay);

			uState= Inp32(AC97_BASE+rACCODECSTAT);

			uCodecStat = (u16)(uState & 0xFFFF);
			//Delay(uDebugdelay);
			if (uCmdData == uCodecStat)
			{
				UART_Printf("Delay Time is %d\n",uDebugdelay);
				break;
			}
			else 
				uDebugdelay++;			
		}
		
	}
}


