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
*	File Name : keypad_test.c
*  
*	File Description : This file implements the API functons for Keypad test.
*
*	Author : Sunghyun, Na
*	Dept. : AP Development Team
*	Created Date : 2008/03/01
*	Version : 0.1 
* 
*	History
*	- Created(Woojin.Kim 2008/03/01)
*  
**************************************************************************************/
/* Include Files */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "keypad.h"
#include "def.h"
#include "option.h"
#include "library.h"
#include "sfr6410.h"
#include "system.h"
#include "sysc.h"
#include "intc.h" 

#define rOTHERS				(SYSCON_BASE+0x900)
#define rRST_STAT			(SYSCON_BASE+0x904)
#define rWAKEUP_STAT		(SYSCON_BASE+0x908)
#define rBLK_PWR_STAT		(SYSCON_BASE+0x90c)
#define rINFORM0			(SYSCON_BASE+0xA00)
#define rINFORM1			(SYSCON_BASE+0xA04)
#define rINFORM2			(SYSCON_BASE+0xA08)
#define rINFORM3			(SYSCON_BASE+0xA0c)
#define rINFORM4			(SYSCON_BASE+0xA10)
#define rINFORM5			(SYSCON_BASE+0xA14)
#define rINFORM6			(SYSCON_BASE+0xA18)
#define rINFORM7			(SYSCON_BASE+0xA1c)


void KEYPAD_Test(void);
void KEYPAD_KeyScanTest(KEYPAD_INFOR* rhs_pInfor);
void KEYPAD_Configuration(KEYPAD_INFOR* rhs_pInfor);
void KEYPAD_KeySTOPWakeUpTest(KEYPAD_INFOR rhs_eInfor);
void KEYPAD_KeySLEEPWakeUpTest(KEYPAD_INFOR rhs_eInfor);

volatile KEYPAD_INFOR	g_KEYInfor;

const testFuncKEYPAD g_aKEYPADTestFunc[] =
{	
	0,									"Exit",		
	KEYPAD_Configuration,				"Configuration\n",
	KEYPAD_KeyScanTest,				"KEYPAD Scan Test\n",
	KEYPAD_KeySTOPWakeUpTest,		"Wake Up Test",
	KEYPAD_KeySLEEPWakeUpTest,	"Sleep Wake Upt Test",
	0,0
};


void KEYPAD_Test(void)
{
	u32 uCountFunc=0;
	s32 iSel=0;
	KEYPORT ePort = KeyPort0;
	KEYPAD_INFOR* pInfor;

	
	
	KEYPAD_Init();
	KEYPAD_InitPort(KeyPort0);
	KEYPAD_ClosePort(KeyPort1);
	ePort = KeyPort0;
	pInfor = KEYPAD_GetInfor();
	
	while(1)
	{
		UART_Printf("\n================== KEYPAD Configuration =====================\n\n");

	
		UART_Printf("Key Port : ");
		if( pInfor->m_ePort == KeyPort0 )
			UART_Printf(" 0(HOST I/F)\n");
		else
			UART_Printf(" 1(MMC, EINT)\n");

		UART_Printf("Interrupt Condition :  ");
		if( pInfor->m_eIntSource == Key_Falling )
			UART_Printf(" Falling Edge\t\t");
		else if( pInfor->m_eIntSource == Key_Rising)
			UART_Printf(" Rising Edge\n");
		else
			UART_Printf(" Both Edge\n");

		UART_Printf("Debouce Filter : ");
		if( pInfor->m_eDFilterSource == DF_RTCCLK )
			UART_Printf(" RTC CLK\t\t");
		else
			UART_Printf(" OSC CLK\t\t");

		UART_Printf("Filter Enable : ");
		if( pInfor->m_uFilterEn == true )
			UART_Printf(" Enable\t\t");
		else
			UART_Printf(" Disable\t\t");

		UART_Printf("Filter Value : %d\n",pInfor->m_uFilterVal);

		UART_Printf("Wake UP source : ");
		if( pInfor->m_uWakeup == 1 )
			UART_Printf(" Enable\t\t");
		else
			UART_Printf(" Disable\t\t");		
			
			
		UART_Printf("\n\n================== KEYPAD Function Test =====================\n\n");
	
		for (uCountFunc=0; (u32)(g_aKEYPADTestFunc[uCountFunc].desc)!=0; uCountFunc++)
			UART_Printf("%2d: %s\n", uCountFunc, g_aKEYPADTestFunc[uCountFunc].desc);

		UART_Printf("\nSelect the function to test : ");
		iSel =UART_GetIntNum();
		UART_Printf("\n");
		
		if(iSel == 0) 
			break;
		if (iSel>=0 && iSel<(sizeof(g_aKEYPADTestFunc)/8-1))
			(g_aKEYPADTestFunc[iSel].func) (pInfor);		
	}
}	

//////////
// Function Name : KEYPAD_Configuration
// Function Description : This function change KeyPad Configuration
// Input : KEYPAD_INFOR : Keypad Inforamation
// Output : NONE
// Version : v0.1
void KEYPAD_Configuration(KEYPAD_INFOR* rhs_pInfor)
{
	u32 uCountFunc=0;
	s32 uSel;
	const testFuncKEYPAD aKEYPADConfigFunc[] =
	{
		KEYPAD_SetPort,					"Select KeyPad Port",
		KEYPAD_Sel_Wakeup,				"Wake Up source ",	
		KEYPAD_SetFilterCNT,				"Filter Source Clock Counter",	
		KEYPAD_SetDBFilter,				"Debounce Filter ",	
		KEYPAD_SetINT,					"Interrupt Source",	
		KEYPAD_SelectDFCNT,				"Fiter Counter Value",
			
	    	0,0
	};

	UART_Printf("Selecet Configuration\n");

	UART_Printf("\n\n================== I2S Configuration =====================\n\n");
	while(1)
	{
		for (uCountFunc=0; (u32)(aKEYPADConfigFunc[uCountFunc].desc)!=0; uCountFunc++)
			UART_Printf("%2d: %s\n", uCountFunc, aKEYPADConfigFunc[uCountFunc].desc);

		UART_Printf("\nSelect the function to configure : ");
		uSel =UART_GetIntNum();
		UART_Printf("\n");
		if(uSel == (u32)-1)
		{
			break;
		}
		if (uSel>=0 && uSel<(sizeof(aKEYPADConfigFunc)/8-1))
			(aKEYPADConfigFunc[uSel].func) (rhs_pInfor);		
	}

}


//////////
// Function Name : KEYPADtest
// Function Description : This function test KEYPAD
// Input : NONE
// Output : NONE
// Version : v0.1

void KEYPAD_KeyScanTest(KEYPAD_INFOR* rhs_pInfor)
{
	KEYPAD_COL(0,0);			// Column Low

	INTC_ClearVectAddr();
	KEYPAD_STSCLR(0xff,0xff);	// Key Interrupt Clear

	
	INTC_SetVectAddr(NUM_KEYPAD,Isr_KEYPAD);
	INTC_Enable(NUM_KEYPAD);

	UART_Printf("\nPress Any KeyPad .......\n");
	
	do
	{

	}while(!UART_GetKey());
	INTC_Disable(NUM_KEYPAD);
}

void KEYPAD_KeySTOPWakeUpTest(KEYPAD_INFOR rhs_eInfor)
{
	//Set KEYIF
	u32 uRegValue;

	u32 uPclkGate, uHclkGate, uSclkGate;
	u32 uNormalCfg, uStopCfg, uStopMemCfg;

	//Set Mode Stop 

	UART_Printf("Now go to Stop mode\n");	
	UART_Printf("After 10 seconds, S3C6410 will wake up by KeyIF interrupt.\n");

	////////////////////////////////
	//  Save I/O Ports 
	////////////////////////////////
	// Save the port configurations
	//UART_Printf("I/O Port is stored!\n");
	//for(i=0;i<36;i++) {
	//	portStatus[i]=*( (volatile U32 *)0x56000000 + i);	// 0x5600_0000:GPACON addr.
	//	}
	//UART_Printf("I/O Port is set for Stop Mode!\n");
	//ConfigStopGPIO();
	//ConfigMiscIO_stop();

	UART_Printf("Test pattern for SDRAM Self-Refresh is filled!\n");
	SYSC_InitData_SDRAM(_DRAM_BaseAddress+0x1000000, 0x10000);

	UART_Printf("Wake-up source is set!\n");
	////////////////////////////////
	// Wake-up Source Setting
	//      Wake-up Source Interrupts are must enabled at Stop Mode
	////////////////////////////////
	KEYPAD_COL(0,0);			// Column Low

	INTC_ClearVectAddr();
	KEYPAD_STSCLR(0xff,0xff);	// Key Interrupt Clear

	INTC_SetVectAddr(NUM_KEYPAD,Isr_KEYPAD_WakeUp);
	INTC_Enable(NUM_KEYPAD);

	SYSC_OSCSTAB(1);			
	   
	SYSC_SetSTOPCFG(1, 1, 1, 1);		// Set ARM_LOGIC/MEM, TOP_LOGIC/MEM to "1"
	SYSC_SetSTOPMEMCFG(1, 1, 1, 1, 1, 1, 1);	

	UART_Printf("CPU will go to Stop Mode!\n");
	UART_TxEmpty();
	
	//Set Mode Stop
	SYSC_SetCFGWFI(eSTOP, 0);

	//wait for inerrupt
	MMU_WaitForInterrupt();	

	UART_Printf("nCPU will go to Normal Mode!\n");						//test done
	SYSC_ClrWKUPSTAT();	
	SYSC_CheckData_SDRAM(_DRAM_BaseAddress+0x1000000, 0x10000);
	INTC_Disable(NUM_KEYPAD);	
}

void KEYPAD_KeySLEEPWakeUpTest(KEYPAD_INFOR rhs_eInfor)
{
	u32 uRstId;	
	u32 ucnt;

	uRstId = SYSC_RdRSTSTAT(1);

	if( Inp32(rINFORM2)==0 )
	{
		UART_Printf("Test pattern for SDRAM Self-Refresh is filled!\n");
		SYSC_InitData_SDRAM(_DRAM_BaseAddress+0x1000000, 0x10000);
	}
//	else if((Inp32(rINFORM1)!=0) && !(Inp32(rSTOP_CFG)&1<<17))  {
	else if((Inp32(rINFORM2)!=0) ) {
		UART_Printf("Wake-up form Sleep Mode \n");
		SYSC_CheckData_SDRAM(_DRAM_BaseAddress+0x1000000, 0x10000);
		SYSC_ClrWKUPSTAT();
		Outp32(rINFORM3, 0x0);
	}	

	ucnt=Inp32(rINFORM2);
	ucnt++;
	if(ucnt==0xffffffff) {
		ucnt=0;
		Outp32(rINFORM1, (Inp32(rINFORM1))++);
	}	
	Outp32(rINFORM2, ucnt);	


	// ADC & RTC OFF		
	ADC_SelectStandbyMode(1);		//ADC Stand-by
	//-daedoo
	//RTC_SetCON(0,0,0,0,0,0);			//RTC SFR R/W Disable.
	RTC_Enable(false);
	////////////////////////////////
	//  Save I/O Ports 
	////////////////////////////////
	// Save the port configurations
	//UART_Printf("I/O Port is stored!\n");
	//for(i=0;i<36;i++) {
	//	portStatus[i]=*( (volatile U32 *)0x56000000 + i);	// 0x5600_0000:GPACON addr.
	//	}
	//UART_Printf("I/O Port is set for Stop Mode!\n");
	//ConfigSleepGPIO();
		
	UART_Printf("Wake-up source is set!\n");
	////////////////////////////////
	// Wake-up Source Setting
	//      Wake-up Source Interrupts are must enabled at Stop Mode
	////////////////////////////////
	KEYPAD_COL(0,0);			// Column Low

	INTC_ClearVectAddr();
	KEYPAD_STSCLR(0xff,0xff);	// Key Interrupt Clear

	INTC_SetVectAddr(NUM_KEYPAD,Isr_KEYPAD_WakeUp);
	INTC_Enable(NUM_KEYPAD);

	
	UART_Printf("System stablilization counter register!\n");
	////////////////////////////////
	// OSCSTABLE = Clock Control
	// PWRSTABLE = Reset Control
	////////////////////////////////	
	SYSC_OSCSTAB(0x1);			
	SYSC_PWRSTAB(0x1);	
	SYSC_SetNORCFG((ucnt&(0x1<<0))>>0,(ucnt&(0x1<<1))>>1,(ucnt&(0x1<<2))>>2,(ucnt&(0x1<<3))>>3,(ucnt&(0x1<<4))>>4,(ucnt&(0x1<<5))>>5,(ucnt&(0x1<<6))>>6,(ucnt&(0x1<<7))>>7,(ucnt&(0x1<<8))>>8); 


	////////////////////////////////
	// Enter the Stop Mode
	// 		1. Setting "CFG_STANDBYWFI" to "2'b11"  ( PWR_CFG(0x7E00_F804)
	//          2. Setting "SLEEP_CFG"(0x7E00_F818) ->  OSC_EN ="0"
	//          2.  Enter to the STANDBYWFI Command
	////////////////////////////////

	UART_Printf("PWRCFG:0x%x, EINTMASK:0x%x, NORMALCFG:0x%x, STOPCFG:0x%x\n", Inp32SYSC(0x804),Inp32SYSC(0x808),Inp32SYSC(0x810),Inp32SYSC(0x814));
	UART_Printf(" SLEEPCFG:0x%x, STOPMEMCFG:0x%x, OTHERS:0x%x, BLKPWRSTAT:0x%x\n",Inp32SYSC(0x818),Inp32SYSC(0x81c),Inp32SYSC(0x900),Inp32SYSC(0x90c)); 
	UART_Printf("\nCPU will go to SLEEP Mode!\n\n");
	UART_TxEmpty();				// 
	Outp32(rINFORM3, 0xDEADDEAD);
	SYSC_SetCFGWFI(eSLEEP, 0);		
	MMU_WaitForInterrupt();
	
	UART_Printf("CPU doesn't go to Sleep Mode\n");

	SYSC_ClrWKUPSTAT();	
	SYSC_CheckData_SDRAM(_DRAM_BaseAddress+0x1000000, 0x10000);
	INTC_Disable(NUM_KEYPAD);	
}