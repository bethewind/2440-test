/**************************************************************************************
* 
*	Project Name : S3C6410 Validation
*
*	Copyright 2006 by Samsung Electronics, Inc.
*	All rights reserved.
*
*	Project Description :
*		This software is only for validating functions of the S3C6410.
*		Anybody can use this software without our permission.
*  
*--------------------------------------------------------------------------------------
* 
*	File Name : power_test.c
*  
*	File Description : This file implements the test functons for the Power Mode
*
*	Author : Wonjoon Jang
*	Dept. : AP Development Team
*	Created Date : 2007/01/10
*	Version : 0.1 
* 
*	History
*	- Created(Wonjoon.jang  2007/01/10)
*  
**************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "option.h"
#include "library.h"
#include "sfr6410.h"
#include "system.h"
#include "gpio.h"
#include "intc.h"

#include "dmc.h"
#include "rtc.h"
//#include "adcts.h"
//#include "lcd.h"
#include "sysc.h"
#include "power_test.h"
#include "timer.h"
#include "keypad.h"
#include "adcts.h"
#include "dma.h"
//#include "hsi.h"
#include "uart.h"

#define	DBG_PRINT			FALSE

u32  g_OnTest ;//= 1;
u32 g_OnTest_DSTOP; //=1;//=1;
u32 g_OnTest_ESLP;//=1;

u32 g_TestCount, g_AlmInt;
u32 g_Inform0;

extern u32 g_DmaDone;
extern u32 g_uIntCounter0 ;   
extern u32 g_uIntCounter1 ;   
extern u32 g_uIntCounter2 ;  
extern u32 g_uIntCounter3 ;  
static DMAC oDmac0_T;
extern u32 g_iSeed;
extern void  Insert_Seed(void);



//////////
// Function Name : InitData_SDRAM
// Function Description : This function initializes data in the DRAM
//					 
// Input : 	check_start - start address, check_size- data size
// Output : 	
//			
// Version : v0.1
void InitData_SDRAM(int check_start, int check_size)
{
    int i;
#if DBG_PRINT
    UART_Printf("[SDRAM Initialization]\n");
    UART_Printf("Fill SDRAM for self-refresh test.\n");
#endif
    for(i=check_start;i<(check_start+check_size);i+=4)
    	*((unsigned int *)i)=i^0x55555555;
    	//*((unsigned int *)i)=0x55555555;
#if DBG_PRINT    
    UART_Printf("Filling SDRAM is completed.\n");
#endif
    
}

//////////
// Function Name : CheckData_SDRAM
// Function Description : This function check data in the DRAM 
//					  
// Input : 	check_start - start address, check_size- data size
// Output : 	
//			
// Version : v0.1
void CheckData_SDRAM(int check_start, int check_size)
{
    int i;
    int error=0;
   #if DBG_PRINT 
    UART_Printf("Check SDRAM for self-refresh test\n");
   #endif
    for(i=check_start;i<(check_start+check_size);i+=4)
    {
    	if(*((unsigned int *)i)!=(i^0x55555555))
    //	if(*((unsigned int *)i)!=(0x55555555))
    	{
    	    UART_Printf("Mem Error:%x=%x(%x)\n",i,*((unsigned int *)i),i^0x55555555);
    	    error++;
    	}
    	if(error>20)
	    break;
    }
    if(error)
    	{
      	UART_Printf("SDRAM self-refresh test:FAILED\n"); 
      	}
    else
    	{
    	 #if DBG_PRINT  
    	UART_Printf("SDRAM self-refresh test:O.K.\n");	
    	 #endif
    	}

}

//////////
// Function Name : Check_AliveSFR
// Function Description : 
//					  
// Input : 	uWorR = 0: Write Alive SFR
//                 uWorR = 1:  Read Alive SFR
// Output : 	
//			
// Version : v0.1
void Check_AliveSFR(u32 uWorR)
{

	if(uWorR == 0)
	{
	
		Outp32SYSC(0x804, 0x80);   		//PWR_CFG		-> 영향없음.
		Outp32SYSC(0x808, 0x10000);		//EINT_MASK
		//Outp32SYSC(0x810,0xFFFFFD00);	//NORMAL_CFG
		Outp32SYSC(0x814,0x20120000);	//STOP_CFG
		Outp32SYSC(0x818,0x1);			//SLEEP_CFG
	
		Outp32SYSC(0x820, 0x2);			//OSC_FREQ
		Outp32SYSC(0x824, 0x2);			//OSC_STABLE
		Outp32SYSC(0x828, 0x2);			//PWR_STABLE
		Outp32SYSC(0x82C, 0x2);			//FPC_STABLE
		Outp32SYSC(0x830, 0xF7777777);	//MTC_STABLE
	
		//Outp32Inform(0, 0x1);			// Information Register
		Outp32Inform(1, 0x2);			// Information Register
		Outp32Inform(2, 0x3);			// Information Register
		Outp32Inform(3, 0x4);			
		Outp32Inform(4, 0x5);
		Outp32Inform(5, 0x6);
		Outp32Inform(6, 0x7);
		//Outp32Inform(7, 0x8);

	}
	else
	{
		
		UART_Printf("	rPWR_CFG = 0x%x \n", Inp32SYSC(0x804));   		//PWR_CFG
		UART_Printf("	rEINT_MASK = 0x%x \n", Inp32SYSC(0x808)); 
		UART_Printf("	rNORMAL_CFG = 0x%x \n", Inp32SYSC(0x810)); 
		UART_Printf("	rSTOP_CFG = 0x%x \n", Inp32SYSC(0x814)); 
		UART_Printf("	rSLEEP_CFG = 0x%x \n", Inp32SYSC(0x818)); 
		UART_Printf("	rOSC_FREQ = 0x%x \n", Inp32SYSC(0x820)); 
		UART_Printf("	rOSC_STABLE = 0x%x \n", Inp32SYSC(0x824)); 
		UART_Printf("	rPWR_STABLE = 0x%x \n", Inp32SYSC(0x828)); 
		UART_Printf("	rFPC_STABLE = 0x%x \n", Inp32SYSC(0x82C)); 
		UART_Printf("	rMTC_STABLE = 0x%x \n", Inp32SYSC(0x830)); 
		UART_Printf("	rInform0	 = 0x%x \n", Inp32SYSC(0xA00)); 
		UART_Printf("	rInform1	 = 0x%x \n", Inp32SYSC(0xA04));  
		UART_Printf("	rInform2	 = 0x%x \n", Inp32SYSC(0xA08)); 
		UART_Printf("	rInform3	 = 0x%x \n", Inp32SYSC(0xA0c)); 
		UART_Printf("	rInform4	 = 0x%x \n", Inp32SYSC(0xA10)); 
		UART_Printf("	rInform5	 = 0x%x \n", Inp32SYSC(0xA14)); 
		UART_Printf("	rInform6	 = 0x%x \n", Inp32SYSC(0xA18)); 
		UART_Printf("	rInform7	 = 0x%x \n", Inp32SYSC(0xA1c)); 

	}

   
}


//////////
// Function Name : Isr_TIMER0
// Function Description : This function is Interrupt Service Routine of PWM Timer0
// Input : NONE
// Output : NONE (increment of g_uIntCounter0 value)
// Version : v0.1

void __irq Isr_TIMER0_Idle(void)
{
	u32 uTmp;
	
	uTmp = Inp32(0x7F006044);
	uTmp = (uTmp & 0x1f)| (1<<5);
	Outp32(0x7F006044,uTmp);       //  Timer0 Interrupt Clear register
//	UART_Printf(".");
    //UART_Printf("Timer0\n");
	g_uIntCounter0++;

    INTC_ClearVectAddr();

}

//////////
// Function Name : Isr_TIMER1
// Function Description : This function is Interrupt Service Routine of PWM Timer0
// Input : NONE
// Output : NONE (increment of g_uIntCounter0 value)
// Version : v0.1
void __irq Isr_TIMER1_Idle(void)
{
 	u32 uTmp;
	
	uTmp = Inp32(0x7F006044);
	uTmp = (uTmp & 0x1f) | (1<<6);
	Outp32(0x7F006044,uTmp);      //  Timer1 Interrupt Clear register
//	UART_Printf("+");
    //UART_Printf("Timer1\n");
    g_uIntCounter1++;
	
    INTC_ClearVectAddr();    

}

//////////
// Function Name : Isr_TIMER2
// Function Description : This function is Interrupt Service Routine of PWM Timer0
// Input : NONE
// Output : NONE (increment of g_uIntCounter0 value)
// Version : v0.1
void __irq Isr_TIMER2_Idle(void)
{
	u32 uTmp;
	
	uTmp = Inp32(0x7F006044);
	uTmp = (uTmp & 0x1f) |(1<<7);
	Outp32(0x7F006044,uTmp);       //  Timer2 Interrupt Clear register
//	UART_Printf("-");
	//UART_Printf("Timer2\n");
	 g_uIntCounter2++;

	INTC_ClearVectAddr();    

}

//////////
// Function Name : Isr_TIMER3
// Function Description : This function is Interrupt Service Routine of PWM Timer0
// Input : NONE
// Output : NONE (increment of g_uIntCounter0 value)
// Version : v0.1
void __irq Isr_TIMER3_Idle(void)
{
	u32 uTmp;
	
	uTmp = Inp32(0x7F006044);
	uTmp = (uTmp & 0x1f) | (1<<8);
	Outp32(0x7F006044,uTmp);       //  Timer3 Interrupt Clear register
 //   UART_Printf("&");
    //UART_Printf("Timer3\n");
    g_uIntCounter3++;
	
    INTC_ClearVectAddr();    
}

void __irq Dma0Done_T(void)
{
    
  	DMACH_ClearIntPending(&oDmac0_T);
 	//rDMAC0IntTCClear = 0xff;
	UART_Printf ("DMA ISR %d\n", g_DmaDone);
	g_DmaDone=1;
	g_DmaDone++;
	INTC_ClearVectAddr();
       //Write_VECTADDR(0x0);
}

void __irq Isr_RTC_AlmWu(void)
{
	//-daedoo
	//RTC_ClearIntP(1);
	RTC_ClearPending(ALARM);
	//uCntAlarm=1;
#if DBG_PRINT
	UART_Printf(" [Wu]RTC alarm test OK\n");
#endif

	INTC_ClearVectAddr();
}


//////////
// Function Name : SetAlarmWakeup
// Function Description : Setting RTC Alarm Int. - after 10sec.
// Input : 	None
// Output :	None 
// Version : v0.1
void SetAlarmWakeup (void)
{
	//-daedoo
	//RTC_ClearIntP(1);	
	RTC_ClearPending(ALARM);
	INTC_SetVectAddr(NUM_RTC_ALARM,Isr_RTC_AlmWu);
	
	//-daedoo
	/*
	RTC_SetCON(0,0,0,0,0,1);
	RTC_SetTime(InitYEAR,InitMONTH,InitDATE,InitDAY,InitHOUR,InitMIN,InitSEC);
	RTC_SetAlmTime(AlmYear,AlmMon,AlmDate,AlmHour,AlmMin,AlmSec+3);
	RTC_SetAlmEn(1,0,0,0,0,0,1);
	RTC_SetCON(0,0,0,0,0,0);
	*/
	RTC_TimeInit(InitYEAR,InitMONTH,InitDATE,InitDAY,InitHOUR,InitMIN,InitSEC);
	RTC_AlarmTimeInit(AlmYear,AlmMon,AlmDate,AlmHour,AlmMin,AlmSec+3);
	RTC_AlarmEnable(true, false, false, false, false, false, true);

#if DBG_PRINT
	UART_Printf("	RTC Alarm is set for wake-up source\n");
#endif
	INTC_Enable(NUM_RTC_ALARM);
	
}

void __irq Isr_RTC_Tick_Wu(void)
{
	
	//GPIO_SetFunctionEach(eGPIO_F, eGPIO_14,3); 	//GPF14[] = 3 : CLKOUT[0]
	//-daedoo
	//RTC_ClearIntP(0);
	RTC_ClearPending(TimeTic);
	
	UART_Printf("[WU] RTC Tick interrupt is occurred .\n");

	INTC_ClearVectAddr();

}


//////////
// Function Name : SetAlarmTick
// Function Description : Setting RTC Alarm Int. - after 10sec.
// Input : 	None
// Output :	None 
// Version : v0.1
void SetAlarmTick (void)
{

	INTC_SetVectAddr(NUM_RTC_TIC, Isr_RTC_Tick_Wu);
	
	// RTC Init.
	//-daedoo
	/*
	RTC_SetCON(0,0,0,0,0,1);
	RTC_SetTime(InitYEAR,InitMONTH,InitDATE,InitDAY,InitHOUR,InitMIN,InitSEC);
	*/
	RTC_TimeInit(InitYEAR,InitMONTH,InitDATE,InitDAY,InitHOUR,InitMIN,InitSEC);
	
#if 0   // 5sec, Tick Clock =128Hz	
	//RTC_SetTickCNT(640);	
	//RTC_SetCON(0,8,0,0,0,0);							// Tick Clock = 128Hz?
       						// 1/128*1280 = 10Sec?
#endif 
	//-daedoo
	/*
	RTC_SetTickCNT(65500);
	RTC_SetCON(0,0,0,0,0,0);	       						
	*/

	RTC_TickCnt(65500);	
	RTC_TickClkSelect(CLK_32768Hz);						


	//RTC_ClearIntP(0);								// Clear Int. Pending
	RTC_ClearPending(TimeTic);
#if 0	
	//RTC_SetCON(1,8,0,0,0,1);
#endif
	//RTC_SetCON(1,0,0,0,0,1);
	RTC_TickTimeEnable(true);
	RTC_Enable(true);

	UART_Printf("	RTC Tick is set for wake-up source\n");

	INTC_Enable(NUM_RTC_TIC);

	
}


//////////
// Function Name : SetEintWakeup
// Function Description : Setting EINT9,10,11 Int. - Falling Edge
// Input : 	None
// Output :	None 
// Version : v0.1
void SetEintWakeup (void)
{
	GPIO_Init();
	
	INTC_SetVectAddr(NUM_EINT1, Isr_WUEint9);

	GPIO_SetPullUpDownEach(eGPIO_N, eGPIO_9, 0x0);  // Pull Down Disable
	GPIO_SetPullUpDownEach(eGPIO_N, eGPIO_11, 0x0);  // Pull Down Disable
	GPIO_SetPullUpDownEach(eGPIO_N, eGPIO_10, 0x0); // Pull Down Disable

	Delay(100);
	//GPIO_SetEint0(9, Low_Level, eDLYFLT, 0);
	GPIO_SetEint0(9, Falling_Edge, eDLYFLT, 0);  			// EVT1 OK

	GPIO_EINT0ClrPend(9);
	GPIO_EINT0ClrPend(10);
	GPIO_EINT0ClrPend(11);

	GPIO_EINT0DisMask(9);
#if DBG_PRINT
	UART_Printf("	EINT9  is set for wake-up source\n");
#endif
	INTC_Enable(NUM_EINT1);
	
}





void __irq	Isr_ADCTS_Wu(void)
{
	UART_Printf("[WU] Touch interrupt is occurred .\n");

	ADCTS_SetADCClearWKInt();
	INTC_ClearVectAddr();
}


//////////
// Function Name : SetADCTSWakeup
// Function Description : 
// Input : 	None
// Output :	None 
// Version : v0.1
void SetADCTSWakeup (void)
{
	u32 uTemp;
	ADCTS_Init();
	ADC_InitADCCON();

	Outp32(0x7E00B000, (Inp32(0x7E00B000)|(1<<2)));
	ADCTS_SetDelay(20);

	uTemp = Inp32(0x7E00B008)|(1<<16);    // RTC Clock
	Outp32(0x7E00B008, uTemp);
	UART_Printf("	Touch Screen  is set for wake-up source\n");

	Outp32(0x7E00B004, 0x90);
	
	ADCTS_SetMeasureMode(eADCTS_MEASURE_FOR_STYLUS);
	Outp32(0x7E00B004, 0xd3);
#if 0	
	UART_Printf("	rADCCON: 0x%x\n", Inp32(0x7E00B000));
	UART_Printf("	rADCTS: 0x%x\n", Inp32(0x7E00B004));
	UART_Printf("	rADCDLY: 0x%x\n", Inp32(0x7E00B008));
#endif	

	INTC_SetVectAddr(NUM_PENDNUP, Isr_ADCTS_Wu);
	INTC_Enable(NUM_PENDNUP);

}



//////////
// Function Name : SetKeypadWakeup
// Function Description : 
// Input : 	None
// Output :	None 
// Version : v0.1
void SetKeypadWakeup (void)
{	
	u32 uGPACONSLP, uGPAPUDSLP;

	//EVT0
	uGPACONSLP = Inp32(0x7F00800C);
	uGPAPUDSLP = Inp32(0x7F008010);

	
	Outp32(0x7E00A004, 0xFF);	
	KEYPAD_Init();

	KEYPAD_SelectINT(1, TRUE);		//Falling Edge
	//KEYPAD_EnableWakeup(3);	// 0x7E00_A000
	KEYPAD_COL(0,0);

	INTC_SetVectAddr(NUM_KEYPAD,Isr_KEYPAD);

	UART_Printf("	Keypad  is set for wake-up source\n");
	INTC_Enable(NUM_KEYPAD);

	// EVT0 
	Outp32(0x7F00800C, uGPACONSLP);
	Outp32(0x7F008010, uGPAPUDSLP);

}



#if 0
//////////
// Function Name : SetHSIWakeup
// Function Description : 
// Input : 	None
// Output :	None 
// Version : v0.1
void SetHSIWakeup (void)
{	
	HSI_SetPort();

	//prepare data
	PrepareTxData();
	PrepareRxData();

	//ISR mapping and interrupt enabling	
	INTC_SetVectAddr(NUM_HSIrx, HSI_Rx_ISR);
	INTC_Enable(NUM_HSIrx);

	HSI_SetForRx(HSI_INT_MODE, HSI_FRAME_MODE, HSI_SINGLE_CHID_MODE, HSI_CHID_3BIT);
	HSI_UnmaskRxInt(0xfb);	// unmask all interrupts except rx-done
	UART_Printf("	HSI  is set for wake-up source\n");
}
#endif

//////////
// Function Name : BatteryFaultEvent
// Function Description : Isr 
// Input : 	None
// Output :	None 
// Version : v0.1
void __irq Isr_WUBATF(void)
{
	SYSC_ClrBATFINT();
	UART_Printf("[WU]Battery Fault is occurred!\n");
	INTC_ClearVectAddr();
}

//////////
// Function Name : BatteryFaultEvent
// Function Description : Isr 
// Input : 	None
// Output :	None 
// Version : v0.1
void __irq Isr_WUBATF1(void)
{
	SYSC_ClrBATFINT();

	Outp32Inform(7, Inp32SYSC(0x90c));
	SYSC_SetCFGWFI(eSLEEP, 0);
	Stop_WFI_Test();
	
	INTC_ClearVectAddr();
}


//////////
// Function Name : Isr_Eintx
// Function Description : EINT ISR Routine
// Input : 	None
// Output :	None 
// Version : v0.1
void __irq Isr_WUEint9(void)
{
          
	GPIO_EINT0ClrPend(9);
	
	//UART_Printf("[WU] EINT9 interrupt is occurred .\n");
      // UART_Printf("0x7f008924= 0x%x\n", Inp32(0x7F008924));
	INTC_ClearVectAddr();  
    
}

void __irq Isr_WUEint10(void)
{
          
	GPIO_EINT0ClrPend(10);
	UART_Printf("[WU]EINT10 interrupt is occurred .\n");

	INTC_ClearVectAddr();
    
}

void __irq Isr_WUEint11(void)
{
          
	GPIO_EINT0ClrPend(11);
	UART_Printf("[WU]EINT11 interrupt is occurred .\n");

	INTC_ClearVectAddr();
    
}


//////////
// Function Name : SetBatteryFaultEvent
// Function Description : Setting Battery Falult Event
// Input : 	None
// Output :	None 
// Version : v0.1
void SetBatteryFaultEvent(BATFLT_eMODE eBATFLT)
{



	if(eBATFLT==eFLT_INT)
	{
		//Outp32SYSC(0x804,0x80); // BATFn_MASK_??
		
		INTC_SetVectAddr(NUM_BATF, Isr_WUBATF);
		SYSC_SetBATF(eFLT_INT, 0);
		INTC_Enable(NUM_BATF);
	}
	if(eBATFLT==eFLT_SLEEP)
	{
		UART_Printf("Inform register is set!\n");
		Outp32Inform(0,0xABCD6400);	
		SYSC_SetBATF(eFLT_SLEEP, 1);	
	}
}

//////////
// Function Name : CheckHCLK_ForPM
// Function Description : Checking & Enable HCLK_GATE[22,21,0]
// Input : 	None
// Output :	None 
// Version : v0.1
void CheckHCLK_ForPM(void)
{
	u32 uRegValue;

	uRegValue = Inp32SYSC(0x30);
	uRegValue = (uRegValue & ~((0x3<<21)|(0x1<<0))) | ((0x3<<21)|(0x1<<0));
	Outp32SYSC(0x30, uRegValue);

	

}

//////////
// Function Name : CheckBLKPWR_ForPM
// Function Description : Checking & Enable NORMAL_CFG
// Input : 	None
// Output :	None 
// Version : v0.1
void CheckBLKPWR_ForPM(void)
{
	u32 uRegValue;

	uRegValue = Inp32SYSC(0x90c) & 0x7F;
	if(uRegValue != 0x7F)
	{
		Outp32SYSC(0x810, 0xFFFFFF00);	// All Sub block Power Enable			
		while(!(Inp32SYSC(0x90C) == 0x7F));
	}
}

//////////
// Function Name : CheckWAKESTAT_ForPM
// Function Description : Checking & Clear Wakeup Status
// Input : 	None
// Output :	None 
// Version : v0.1
void CheckWAKESTAT_ForPM(u32 uPrint)
{
	u32 uRegValue;

	uRegValue =Inp32SYSC(0x908);
	if (uPrint==1)
	{
	UART_Printf("Wake-up Status Register:	0x%x\n",  uRegValue);
	}
	Outp32SYSC(0x908, uRegValue);
}



//////////
// Function Name : IDLET_Baisc
// Function Description : Idle Mode Test - Baisc Test ( 1 Int. Source Enable, Not adapted DVS Scheme)
// Input : 	None
// Output :	None 
// Version : v0.1

void IDLET_Basic (void)
{
	u32 uRegValue;
	
	UART_Printf("[IDLE Mode Baisc Test]\n");
	UART_Printf("After 10 seconds, S3C6410 will wake up by RTC alarm interrupt.\n");
	UART_Printf("S3C6410 will also wake up by EINT9, ADCTS, or BATT_FLT.\n");
	
	
	////////////////////////////////
	// Wake-up Source Setting
	////////////////////////////////
	UART_Printf("Wake-up sourceis set! \n");
	SetEintWakeup();
	SetADCTSWakeup();
	SetAlarmWakeup();
	SetBatteryFaultEvent(eFLT_INT);

	////////////////////////////////
	// Enter Idle Mode
	// 		1. Setting "CFG_STANDBYWFI" to "2'b01"  ( PWR_CFG(0x7E00_F804)
	//          2.  Enter to the STANDBYWFI Command
	////////////////////////////////
	
	UART_Printf("CPU will goes to IDLE Mode \n");
	UART_TxEmpty();

	SYSC_SetCFGWFI(eIDLE,0);		// IDLE,  Clock Disable
	MMU_WaitForInterrupt();


	UART_Printf("Return to Normal Mode \n\n");
	UART_Printf("rWAKEUP_STAT=0x%x\n", Inp32SYSC(0x908));
	uRegValue=Inp32SYSC(0x908);
	Outp32SYSC(0x908,uRegValue);
	UART_Printf("rWAKEUP_STAT=0x%x\n Wake-up Status Clear\n", Inp32SYSC(0x908));

	// Int. Disable
	INTC_Disable(NUM_EINT1);
	INTC_Disable(NUM_RTC_ALARM);
	INTC_Disable(NUM_BATF);
	INTC_Disable(NUM_PENDNUP);

}


//////////
// Function Name : STOPT_Basic 
// Function Description : Stop Mode Test - Baisc Test 
// Input : 	None
// Output :	None 
// Version : v0.1

void STOPT_Basic (void)
{
	u32 uRegValue;
	
	UART_Printf("\n[STOP Mode Basic Test]\n");
	UART_Printf("After 10 seconds, S3C6410 will wake up by RTC alarm interrupt.\n");
	UART_Printf("S3C6410 will also wake up by EINT9, ADCTS, or BATT_FLT.\n");


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
	Outp32Inform(5, 0x0);

	UART_Printf("Wake-up source is set!\n");
	////////////////////////////////
	// Wake-up Source Setting
	//      Wake-up Source Interrupts are must enabled at Stop Mode
	////////////////////////////////
   	SetAlarmWakeup();
	SetEintWakeup();
	Outp32SYSC(0x804, 0x1<<7);			// nBATF Wake-up Mask disable
	SetBatteryFaultEvent(eFLT_INT);
	SetADCTSWakeup();

       /////////////////////////////////
       // 1) Check&Enable HCLK_GATE[22,21,0]
       // 2) Check BLK_PWR_STAT 
       // 3) Check&Clear WAKEUP_STAT
       //////////////////////////////
	CheckHCLK_ForPM();
       CheckBLKPWR_ForPM();
	CheckWAKESTAT_ForPM(1);
	
	UART_Printf("\nCPU will goes to Stop Mode!\n\n");
	UART_TxEmpty();				// 

	////////////////////////////////
	// Enter the Stop Mode
	// 		1. Setting "CFG_STANDBYWFI" to "2'b10"  ( PWR_CFG(0x7E00_F804)
	//          2. Setting "STOP_CFG"(0x7E00_F814) -> ARM_LOGIC/MEM, TOP_LOGIC/MEM ON, OSC_EN ="0"
	//          2.  Enter to the STANDBYWFI Command
	////////////////////////////////
	SYSC_OSCSTAB(1);			
	SYSC_SetSTOPCFG(1, 1, 1, 1);		// Set ARM_LOGIC/MEM, TOP_LOGIC/MEM to "1"
	SYSC_SetCFGWFI(eSTOP, 0);		// 
	MMU_WaitForInterrupt();


	
	////////////////////////////////
	//  Restore  I/O Ports 
	////////////////////////////////
	
/*
	//Restore the port configurations
	UART_Printf("\nI/O Port is restored!\n");
	for(i=0;i<36;i++){
		*( (volatile U32 *)0x56000000 + i)=portStatus[i];
	}
*/
	DisplayLED(0x3);
	UART_Printf("Return to Normal Mode.\n");
	UART_Printf("rWAKEUP_STAT=0x%x\n", Inp32SYSC(0x908));
	uRegValue=Inp32SYSC(0x908);
	Outp32SYSC(0x908,uRegValue);
	UART_Printf("rWAKEUP_STAT=0x%x\n Wake-up Status Clear\n", Inp32SYSC(0x908));
	
	SYSC_RdBLKPWR();

	// Int. Disable
	INTC_Disable(NUM_EINT1);
	INTC_Disable(NUM_RTC_ALARM);
	INTC_Disable(NUM_BATF);
	INTC_Disable(NUM_PENDNUP);

}

//////////
// Function Name : DeepStopStatus
// Function Description : This function  define Deepstop wake-up
// Input :   NONE			
// Output : true , false
// Version : 
u8 DeepStopStatus(void)
{
	u32 uRead0, uRead1;
	
	uRead0=Inp32SYSC(0x804);			// Read PWR_CFG
	uRead1=Inp32SYSC(0x814);			// Read STOP_CFG

       uRead0 = (uRead0 >>5)& 0x3;			// Read CFG_STANDBYWFI
       uRead1 = (uRead1 >>17)&0x1;			// Read ARM_LOGIC

       if ((uRead0==0x2)&&(uRead1 == 0x0))
      	{
      	#if DBG_PRINT
      		UART_Printf("Wake-up from Deep Stop Mode \n");
      	#endif
		return true;
      	}
      	else
		return false;
}


//////////
// Function Name : DSTOPT_Basic 
// Function Description : Stop Mode Test - Baisc Test 
// Input : 	None
// Output :	None 
// Version : v0.1

void DSTOPT_Basic (void)
{
	//u32 uRegValue;
	u32	uDSTOP;

	uDSTOP= DeepStopStatus();
	SYSC_RdRSTSTAT(1);

	if( uDSTOP && !(g_OnTest_DSTOP) )									// must be changed
	{
		
		CheckData_SDRAM(_DRAM_BaseAddress+0x1000000, 0x10000);

		//Check & Clear Wake-up Source 
		SYSC_ClrWKUPSTAT();

		//Read RawInterrupt Status
		UART_Printf("VIC0RawStatus: 0x%x \n", Inp32(0x71200008));
		UART_Printf("VIC1RawStatus: 0x%x \n", Inp32(0x71300008));

		UART_Printf("Deep Stop mode test is done\n");
		g_OnTest_DSTOP = 1;
	}

else			// Entering into Deep Stop Mode
	{
		g_OnTest_DSTOP=0;
	
	UART_Printf("\n[Deep STOP Mode Basic Test]\n");
	UART_Printf("After 10 seconds, S3C6410 will wake up by RTC alarm interrupt.\n");
	UART_Printf("S3C6410 will also wake up by EINT9, ADCTS, or BATT_FLT.\n");


	UART_Printf("Test pattern for SDRAM Self-Refresh is filled!\n");
	InitData_SDRAM(_DRAM_BaseAddress+0x1000000, 0x10000);
	Outp32Inform(5, 0x0);
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


	UART_Printf("Wake-up source is set!\n");
	////////////////////////////////
	// Wake-up Source Setting
	//      Wake-up Source Interrupts are must enabled at Stop Mode
	////////////////////////////////
   	SetAlarmWakeup();
	SetEintWakeup();
	Outp32SYSC(0x804, 0x1<<7);			// nBATF Wake-up Mask disable
	SetBatteryFaultEvent(eFLT_INT);
	SetADCTSWakeup();

       /////////////////////////////////
       // 1) Check&Enable HCLK_GATE[22,21,0]
       // 2) Check BLK_PWR_STAT 
       // 3) Check&Clear WAKEUP_STAT
       //////////////////////////////
	CheckHCLK_ForPM();
       CheckBLKPWR_ForPM();
	CheckWAKESTAT_ForPM(1);
	
	UART_Printf("\nCPU will goes to Deep Stop Mode!\n\n");
	UART_TxEmpty();				// 

	////////////////////////////////
	// Enter the Stop Mode
	// 		1. Setting "CFG_STANDBYWFI" to "2'b10"  ( PWR_CFG(0x7E00_F804)
	//          2. Setting "STOP_CFG"(0x7E00_F814) -> ARM_LOGIC/MEM, TOP_LOGIC/MEM OFF, OSC_EN ="0"
	//          2.  Enter to the STANDBYWFI Command
	////////////////////////////////
	SYSC_OSCSTAB(1);			
	SYSC_SetSTOPCFG(0, 0, 0, 0);		// Set ARM_LOGIC/MEM, TOP_LOGIC/MEM to "1"
	SYSC_SetCFGWFI(eSTOP, 0);		// 
	MMU_WaitForInterrupt();

	UART_Printf("CPU doesn't go to Deep Stop Mode\n");

/*
	//Restore the port configurations
	UART_Printf("\nI/O Port is restored!\n");
	for(i=0;i<36;i++){
		*( (volatile U32 *)0x56000000 + i)=portStatus[i];
	}
*/

	// Int. Disable
	INTC_Disable(NUM_EINT1);
	INTC_Disable(NUM_RTC_ALARM);
	INTC_Disable(NUM_BATF);
	INTC_Disable(NUM_PENDNUP);

	}
	// Int. Disable
	INTC_Disable(NUM_EINT1);
	INTC_Disable(NUM_RTC_ALARM);
	INTC_Disable(NUM_BATF);
	INTC_Disable(NUM_PENDNUP);


}


void ConfigSleepGPIO()
{
	u32 uTTT1, uTTT2, uTTT3;
	
	//UART_Printf("PHY OFF\n");
	uTTT1 = Inp32SYSC(0x900);
	uTTT1 = uTTT1|(1<<14);		
	Outp32SYSC(0x900, uTTT1);	//  USB_PWR_DN_EN
	Outp32(0x7C100000, 0xF);		//  USB_Power Down
	//UART_Printf("rOTHERS(PWR_DN_EN): 0x%x\n",  Inp32SYSC(0x900));
	uTTT1 = Inp32SYSC(0x900);
	uTTT1 = uTTT1 & ~(1<<16);
	Outp32SYSC(0x900, uTTT1);
	//UART_Printf("rOTHERS(USB_SIG_MASK): 0x%x\n",  Inp32SYSC(0x900));
	//UART_Printf("rOTHERS(USB_SIG_MASK): 0x%x\n",  Inp32SYSC(0x900));
	// OTG Power Control....
	
	UART_Printf("rGPA: 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n", Inp32(0x7F008000), Inp32(0x7F008004), Inp32(0x7F008008), Inp32(0x7F00800C),Inp32(0x7F008010));
	UART_Printf("rGPB: 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n", Inp32(0x7F008020), Inp32(0x7F008024), Inp32(0x7F008028), Inp32(0x7F00802C),Inp32(0x7F008030));
	UART_Printf("rGPC: 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n", Inp32(0x7F008040), Inp32(0x7F008044), Inp32(0x7F008048), Inp32(0x7F00804C),Inp32(0x7F008050));
	UART_Printf("rGPD: 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n", Inp32(0x7F008060), Inp32(0x7F008064), Inp32(0x7F008068), Inp32(0x7F00806C),Inp32(0x7F008070));
	UART_Printf("rGPE: 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n", Inp32(0x7F008080), Inp32(0x7F008084), Inp32(0x7F008088), Inp32(0x7F00808C),Inp32(0x7F008090));
	UART_Printf("rGPF: 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n", Inp32(0x7F0080A0), Inp32(0x7F0080A4), Inp32(0x7F0080A8), Inp32(0x7F0080AC),Inp32(0x7F0080B0));
	UART_Printf("rGPG: 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n", Inp32(0x7F0080C0), Inp32(0x7F0080C4), Inp32(0x7F0080C8), Inp32(0x7F0080CC),Inp32(0x7F0080D0));
	UART_Printf("rGPH: 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n", Inp32(0x7F0080E0), Inp32(0x7F0080E4), Inp32(0x7F0080E8), Inp32(0x7F0080EC),Inp32(0x7F0080F0),Inp32(0x7F0080F4));
	UART_Printf("rGPI: 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n", Inp32(0x7F008100), Inp32(0x7F008104), Inp32(0x7F008108), Inp32(0x7F00810C),Inp32(0x7F008110));
	UART_Printf("rGPJ: 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n", Inp32(0x7F008120), Inp32(0x7F008124), Inp32(0x7F008128), Inp32(0x7F00812C),Inp32(0x7F008130));
	UART_Printf("rGPK: 0x%x, 0x%x, 0x%x, 0x%x\n", Inp32(0x7F008800), Inp32(0x7F008804), Inp32(0x7F008808), Inp32(0x7F00880C));
	UART_Printf("rGPL: 0x%x, 0x%x, 0x%x, 0x%x\n", Inp32(0x7F008810), Inp32(0x7F008814), Inp32(0x7F008818), Inp32(0x7F00881C));
	UART_Printf("rGPM: 0x%x, 0x%x, 0x%x\n", Inp32(0x7F008820), Inp32(0x7F008824), Inp32(0x7F008828));
	UART_Printf("rGPN: 0x%x, 0x%x, 0x%x\n", Inp32(0x7F008830), Inp32(0x7F008834), Inp32(0x7F008838));
	UART_Printf("rGPO: 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n", Inp32(0x7F008140), Inp32(0x7F008144), Inp32(0x7F008148), Inp32(0x7F00814C),Inp32(0x7F008150));
	UART_Printf("rGPP: 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n", Inp32(0x7F008160), Inp32(0x7F008164), Inp32(0x7F008168), Inp32(0x7F00816C),Inp32(0x7F008170));
	UART_Printf("rGPQ: 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n", Inp32(0x7F008180), Inp32(0x7F008184), Inp32(0x7F008188), Inp32(0x7F00818C),Inp32(0x7F008190));
	UART_Printf("rSPCON: 0x%x\n", Inp32(0x7F0081A0));
	UART_Printf("rMEMCONSTOP: 0x%x, 0x%x\n", Inp32(0x7F0081B0), Inp32(0x7F0081B4));
	UART_Printf("rMEMCONSLP: 0x%x, 0x%x, 0x%x\n", Inp32(0x7F0081C0), Inp32(0x7F0081C4),Inp32(0x7F0081C8));
	UART_Printf("rMEMDRVCON: 0x%x, 0x%x\n", Inp32(0x7F0081D0), Inp32(0x7F0081D4));
	UART_Printf("rSPCONSLP, SPLEN: 0x%x, 0x%x\n", Inp32(0x7F008880),Inp32(0x7F008930));
	UART_Printf("\n========================================================\n");

///////////////////////////////////////////////////////////////////////////////////////////	
// VDD_MEM0 : GPO, GPP, GPQ Group  =>15~20uA
//                  :
	//Outp32(0x7F008160,  0x0A88AAAA );   // GPPCON
	Outp32(0x7F008160,  0x0);   // GPPCON

	GPIO_SetPullUDSleepAll(eGPIO_O,  0x55555AAA);		// 
	GPIO_SetConRegSleepAll(eGPIO_O, 0xAAAAAAAA);		// Input
	GPIO_SetConRegSleepAll(eGPIO_P, 0x2AAAAAAA);		// Input
	GPIO_SetPullUDSleepAll(eGPIO_P,  0x2AAAAAAA);		// Pull-up
	//GPIO_SetPullUDSleepAll(eGPIO_P,  0x2AAAAAAA);		// Pull-up
	GPIO_SetConRegSleepAll(eGPIO_Q, 0x2AAAA);		// Input
	GPIO_SetPullUDSleepAll(eGPIO_Q,  0x2AAAA);		// Pull-up

	//Outp32(0x7F0081C0, 0x05555252);					// MEM0CONSLP0
	Outp32(0x7F0081C0, 0x05555151);					// MEM0CONSLP0
	//Outp32(0x7F0081C4, 0x2A95555);					// MEM0CONSLP1
	Outp32(0x7F0081C4, 0x2A95555);					// MEM0CONSLP1   Waitn 시방 안된것

	//UART_Printf("rGPOCONSLP: 0x%x\n", Inp32(0x7F00814c));
	//UART_Printf("rGPOPUDSLP: 0x%x\n", Inp32(0x7F008150));
	//UART_Printf("rGPPCONSLP: 0x%x\n", Inp32(0x7F00816C));
	//UART_Printf("rGPPPUDSLP: 0x%x\n", Inp32(0x7F008170));
	//UART_Printf("rGPQCONSLP: 0x%x\n", Inp32(0x7F00818C));
	//UART_Printf("rGPQPUDSLP: 0x%x\n", Inp32(0x7F008190));

///////////////////////////////////////////////////////////////////////////////////////////	
// VDD_MEM1 : GPO, GPP Group
//                  :  Default = 0

///////////////////////////////////////////////////////////////////////////////////////////	
// VDD_EXT : UART, I2C, IrDA, SPI0, Camera, PWM
//                : GPA, GPB, GPC[0:3], GPF
	GPIO_SetConRegSleepAll(eGPIO_A, 0xAAAA);		// Input
	GPIO_SetPullUDSleepAll(eGPIO_A,  0x5555);		// Pull-Down
	GPIO_SetConRegSleepAll(eGPIO_B, 0x2AAA);		// Input
	GPIO_SetPullUDSleepAll(eGPIO_B,  0x1555);		// Pull-Down
	GPIO_SetConRegSleepAll(eGPIO_C, 0xAAAA);	// Input
	GPIO_SetPullUDSleepAll(eGPIO_C,  0xA555);		// Pull-Down,  I2C Pull-up
	GPIO_SetConRegSleepAll(eGPIO_F, 0xAAAAAAAA);		// Input
	GPIO_SetPullUDSleepAll(eGPIO_F,  0x95555555);		// Pull-Down, XPWM_TOUT1 Pull-up

	//UART_Printf("rGPACONSLP: 0x%x\n", Inp32(0x7F00800C));
	//UART_Printf("rGPAPUDSLP: 0x%x\n", Inp32(0x7F008010));
	//UART_Printf("rGPBCONSLP: 0x%x\n", Inp32(0x7F00802C));
	//UART_Printf("rGPBPUDSLP: 0x%x\n", Inp32(0x7F008030));
	//UART_Printf("rGPCCONSLP: 0x%x\n", Inp32(0x7F00804C));
	//UART_Printf("rGPCPUDSLP: 0x%x\n", Inp32(0x7F008050));
	//UART_Printf("rGPFCONSLP: 0x%x\n", Inp32(0x7F0080AC));
	//UART_Printf("rGPFPUDSLP: 0x%x\n", Inp32(0x7F0080B0));

///////////////////////////////////////////////////////////////////////////////////////////	
// VDD_MMC : MMC, SPI1
//                 : GPC[4:7], GPG, GPH
	GPIO_SetConRegSleepAll(eGPIO_G, 0x2AAA);		// Input
	GPIO_SetPullUDSleepAll(eGPIO_G,  0x1555);		// Pull-Down
	GPIO_SetConRegSleepAll(eGPIO_H, 0xAAAAA);		// Input
	GPIO_SetPullUDSleepAll(eGPIO_H,  0x55555);		// Pull-Down

	//UART_Printf("rGPGCONSLP: 0x%x\n", Inp32(0x7F0080CC));
	//UART_Printf("rGPGPUDSLP: 0x%x\n", Inp32(0x7F0080D0));
	//UART_Printf("rGPGCONSLP: 0x%x\n", Inp32(0x7F0080F0));
	//UART_Printf("rGPGPUDSLP: 0x%x\n", Inp32(0x7F0080F4));

///////////////////////////////////////////////////////////////////////////////////////////	
// VDD_PCM : Audio Port0,1 => Leakage 70uA정도 => Tr Switch Issue??
//                 : GPD, GPE

	GPIO_SetConRegSleepAll(eGPIO_D, 0x2AA);		// Input
	GPIO_SetPullUDSleepAll(eGPIO_D,  0x155);		// Pull-Down
	//GPIO_SetConRegSleepAll(eGPIO_E, 0x2AA);		// Input, GPE1 Output "0"
	GPIO_SetConRegSleepAll(eGPIO_E, 0x2A2);		// Input, GPE1 Output "0"
	GPIO_SetPullUDSleepAll(eGPIO_E,  0x151);		// Pull-Down => 0??

	//UART_Printf("rGPDCONSLP: 0x%x\n", Inp32(0x7F00806C));
	//UART_Printf("rGPDPUDSLP: 0x%x\n", Inp32(0x7F008070));
	//UART_Printf("rGPECONSLP: 0x%x\n", Inp32(0x7F00808C));
	//UART_Printf("rGPEPUDSLP: 0x%x\n", Inp32(0x7F008090));

///////////////////////////////////////////////////////////////////////////////////////////	
// VDD_LCD : LCD   => 확인 필요 Output Low => 0, Input Pull-Down GPJ에서... GPJ11,8,9
//                 : GPI, GPJ

	Outp32(0x7410800C, 0x0);		// Bypass OFF
	
	//GPIO_SetConRegSleepAll(eGPIO_I, 0x00000000);		// Output Low
	uTTT1 = Inp32(0x7F008108);		// Back-up GPIPUD
	GPIO_SetConRegSleepAll(eGPIO_I, 0xAAAAAAAA);		// Input
	Outp32(0x7F008108, uTTT1);
	
	uTTT1 = Inp32(0x7F008150);	//Back-up GPOPUDSLP
	uTTT2 = Inp32(0x7F008190);     //Back-up GPQPUDSLP
	uTTT3 = Inp32(0x7F008100);    // Back-up GPICONSLP
	GPIO_SetPullUDSleepAll(eGPIO_I,  0x55555555);		// Pull-Down
	Outp32(0x7F008150, uTTT1);
	Outp32(0x7F008190, uTTT2);
	Outp32(0x7F008100, uTTT3);

	uTTT1 = Inp32(0x7F008128);   // Back-up GPJPUD
	GPIO_SetConRegSleepAll(eGPIO_J, 0xAAAAAA);		// Input
	Outp32(0x7F008128, uTTT1);

	uTTT1 = Inp32(0x7F008170);	//Back-up GPPPUDSLP
	uTTT2 = Inp32(0x7F008120);	//Back-up GPJCON
	GPIO_SetPullUDSleepAll(eGPIO_J,  0x555555);		// Pull-Down   
	Outp32(0x7F008170, uTTT1);
	Outp32(0x7F008120, uTTT2);

	//UART_Printf("rGPICONSLP: 0x%x\n", Inp32(0x7F00810C));
	//UART_Printf("rGPIPUDSLP: 0x%x\n", Inp32(0x7F008110));
	//UART_Printf("rGPJCONSLP: 0x%x\n", Inp32(0x7F00812C));
	//UART_Printf("rGPJPUDSLP: 0x%x\n", Inp32(0x7F008130));


///////////////////////////////////////////////////////////////////////////////////////////	
// VDD_HI    : Host/Modem I/F  => SMDK Default Setting...
//                 : GPK, GPL, GPM
	//uTTT1 = Inp32(0x7F008010);	// Back-up GPAPUDSLP

	//GPIO_SetFunctionAll(eGPIO_K, 0x0, 0x0);   // EVT0, GPKCON0, GPKCON1 => GPACON, GPADAT
	//GPIO_SetFunctionAll(eGPIO_L, 0x0, 0x0);
	//GPIO_SetFunctionAll(eGPIO_M, 0x0, 0x0);

	//Outp32(0x7F008010, uTTT1);

	// Ids_Alive@Sleep EVT0
	GPIO_SetFunctionAll(eGPIO_L, 0x0, 0x0);
	Outp32(0x7F00881c, 0x25555555);		// GPL14 Pull-up, GPL13 Pull dn
	
///////////////////////////////////////////////////////////////////////////////////////////	
// VDD_SYS :  EINT Port, Reset, Clock, JTAG, Others
//                :  GPN[15:0]

	uTTT1 = Inp32(0x7F008030);	//Back-up GPBPUDSLP
	GPIO_SetFunctionAll(eGPIO_N, 0x0, 0x0);
	GPIO_SetPullUpDownAll(eGPIO_N, 0x55A95555);
	Outp32(0x7F008030, uTTT1);

// Others
	Outp32(0x7F008880, 0x1010);	// Reset Out


// Printf All I/O
	UART_Printf("\n========================================================\n");
	UART_Printf("rGPA: 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n", Inp32(0x7F008000), Inp32(0x7F008004), Inp32(0x7F008008), Inp32(0x7F00800C),Inp32(0x7F008010));
	UART_Printf("rGPB: 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n", Inp32(0x7F008020), Inp32(0x7F008024), Inp32(0x7F008028), Inp32(0x7F00802C),Inp32(0x7F008030));
	UART_Printf("rGPC: 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n", Inp32(0x7F008040), Inp32(0x7F008044), Inp32(0x7F008048), Inp32(0x7F00804C),Inp32(0x7F008050));
	UART_Printf("rGPD: 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n", Inp32(0x7F008060), Inp32(0x7F008064), Inp32(0x7F008068), Inp32(0x7F00806C),Inp32(0x7F008070));
	UART_Printf("rGPE: 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n", Inp32(0x7F008080), Inp32(0x7F008084), Inp32(0x7F008088), Inp32(0x7F00808C),Inp32(0x7F008090));
	UART_Printf("rGPF: 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n", Inp32(0x7F0080A0), Inp32(0x7F0080A4), Inp32(0x7F0080A8), Inp32(0x7F0080AC),Inp32(0x7F0080B0));
	UART_Printf("rGPG: 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n", Inp32(0x7F0080C0), Inp32(0x7F0080C4), Inp32(0x7F0080C8), Inp32(0x7F0080CC),Inp32(0x7F0080D0));
	UART_Printf("rGPH: 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n", Inp32(0x7F0080E0), Inp32(0x7F0080E4), Inp32(0x7F0080E8), Inp32(0x7F0080EC),Inp32(0x7F0080F0),Inp32(0x7F0080F4));
	UART_Printf("rGPI: 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n", Inp32(0x7F008100), Inp32(0x7F008104), Inp32(0x7F008108), Inp32(0x7F00810C),Inp32(0x7F008110));
	UART_Printf("rGPJ: 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n", Inp32(0x7F008120), Inp32(0x7F008124), Inp32(0x7F008128), Inp32(0x7F00812C),Inp32(0x7F008130));
	UART_Printf("rGPK: 0x%x, 0x%x, 0x%x, 0x%x\n", Inp32(0x7F008800), Inp32(0x7F008804), Inp32(0x7F008808), Inp32(0x7F00880C));
	UART_Printf("rGPL: 0x%x, 0x%x, 0x%x, 0x%x\n", Inp32(0x7F008810), Inp32(0x7F008814), Inp32(0x7F008818), Inp32(0x7F00881C));
	UART_Printf("rGPM: 0x%x, 0x%x, 0x%x\n", Inp32(0x7F008820), Inp32(0x7F008824), Inp32(0x7F008828));
	UART_Printf("rGPN: 0x%x, 0x%x, 0x%x\n", Inp32(0x7F008830), Inp32(0x7F008834), Inp32(0x7F008838));
	UART_Printf("rGPO: 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n", Inp32(0x7F008140), Inp32(0x7F008144), Inp32(0x7F008148), Inp32(0x7F00814C),Inp32(0x7F008150));
	UART_Printf("rGPP: 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n", Inp32(0x7F008160), Inp32(0x7F008164), Inp32(0x7F008168), Inp32(0x7F00816C),Inp32(0x7F008170));
	UART_Printf("rGPQ: 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n", Inp32(0x7F008180), Inp32(0x7F008184), Inp32(0x7F008188), Inp32(0x7F00818C),Inp32(0x7F008190));
	UART_Printf("rSPCON: 0x%x\n", Inp32(0x7F0081A0));
	UART_Printf("rMEMCONSTOP: 0x%x, 0x%x\n", Inp32(0x7F0081B0), Inp32(0x7F0081B4));
	UART_Printf("rMEMCONSLP: 0x%x, 0x%x, 0x%x\n", Inp32(0x7F0081C0), Inp32(0x7F0081C4),Inp32(0x7F0081C8));
	UART_Printf("rMEMDRVCON: 0x%x, 0x%x\n", Inp32(0x7F0081D0), Inp32(0x7F0081D4));
	UART_Printf("rSPCONSLP, SPLEN: 0x%x, 0x%x\n", Inp32(0x7F008880),Inp32(0x7F008930));
		

}



//////////
// Function Name : SLEEPT_Basic  
// Function Description : Sleep Mode Test - Baisc Test 
// Input : 	None
// Output :	None 
// Version : v0.1
void SLEEPT_Basic (void)
{
	u32 uRstId;	
	
	uRstId = SYSC_RdRSTSTAT(1);
	
	if( ( uRstId == 3 ) && !(g_OnTest) )
	{
		UART_Printf("Wake-up form SLEEP Mode \n");
		CheckData_SDRAM(_DRAM_BaseAddress+0x1000000, 0x10000);

		//Check & Clear Wake-up Source 
		SYSC_ClrWKUPSTAT();
		//Read RawInterrupt Status
		UART_Printf("VIC0RawStatus: 0x%x \n", Inp32(0x71200008));
		UART_Printf("VIC1RawStatus: 0x%x \n", Inp32(0x71300008));

		
		UART_Printf("SLEEP mode test is done\n");
		g_OnTest = 1;
	}
	else			// Entering into SLEEP Mode
	{
		g_OnTest=0;
		
		UART_Printf("[SLEEP Mode Test]\n");
		UART_Printf("S3C6410 will wake-up by EINT9~11 or Other wake-up sources\n");

		UART_Printf("Test pattern for SDRAM Self-Refresh is filled!\n");
		InitData_SDRAM(_DRAM_BaseAddress+0x1000000, 0x10000);

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
   		SetAlarmWakeup();
		SetEintWakeup();
		Outp32SYSC(0x804, 0x1<<7);			// nBATF Wake-up Mask disable
		SetBatteryFaultEvent(eFLT_INT);
	

		UART_Printf("System stablilization counter register!\n");
		////////////////////////////////
		// OSCSTABLE = Clock Control
		// PWRSTABLE = Reset Control
		////////////////////////////////	
		SYSC_OSCSTAB(0x1);			
		SYSC_PWRSTAB(0x1);			

   	  	/////////////////////////////////
   	    	// 1) Check&Enable HCLK_GATE[22,21,0]
      		// 2) Check BLK_PWR_STAT 
	       // 3) Check&Clear WAKEUP_STAT
	       //////////////////////////////
		CheckHCLK_ForPM();
	       CheckBLKPWR_ForPM();
		CheckWAKESTAT_ForPM(1);
	
		UART_Printf("\nCPU will goes to SLEEP Mode!\n\n");
		UART_TxEmpty();				// 
		////////////////////////////////
		// Enter the Stop Mode
		// 		1. Setting "CFG_STANDBYWFI" to "2'b11"  ( PWR_CFG(0x7E00_F804)
		//          2. Setting "SLEEP_CFG"(0x7E00_F818) ->  OSC_EN ="0"
		//          2.  Enter to the STANDBYWFI Command
		////////////////////////////////
				
		SYSC_SetCFGWFI(eSLEEP, 0);		
		MMU_WaitForInterrupt();
		

		UART_Printf("CPU doesn't go to Sleep Mode\n");

	}

}


//////////
// Function Name : ESLEEPT_Basic 
// Function Description : ESleep Mode Test - Baisc Test 
// Input : 	None
// Output :	None 
// Version : v0.1
void ESLEEPT_Basic (void)
{
	u32 uRstId;
	u32 uInform0, uInform7;
	u32 uINTSEL;
		
	UART_Printf("rINFORM0: 0x%x\n", Inp32Inform(0));
	UART_Printf("rINFORM7: 0x%x\n", Inp32Inform(7));

	uInform0 = 0xABCD6400;
	uInform7 = 0x6400ABCD;
	
	uRstId = SYSC_RdRSTSTAT(1);

//	if( ( uRstId == 4 ) && !(g_OnTest) )
	if( ( uRstId == 3 ) && !(g_OnTest) )				// EVT0 
	{
		UART_Printf(" ESLEEP Mode \n");
		CheckData_SDRAM(_DRAM_BaseAddress+0x1000000, 0x10000);

		//Check Information Register Value
		if( (uInform0 !=Inp32Inform(0) )||(uInform7 != Inp32Inform(7)))
		{
			UART_Printf(" Information Register Value is wrong!!! \n");
		}
		else 
		{
			UART_Printf(" Information Register Value is correct!!! \n");
		}

		//Check & Clear Wake-up Source 
		SYSC_ClrWKUPSTAT();
		//Read RawInterrupt Status
		UART_Printf("VIC0RawStatus: 0x%x \n", Inp32(0x71200008));
		UART_Printf("VIC1RawStatus: 0x%x \n", Inp32(0x71300008));

		UART_Printf("SLEEP mode test is done\n");
		g_OnTest_ESLP = 1;
	}
	else			// Entering into SLEEP Mode
	{
		g_OnTest_ESLP=0;
		
		UART_Printf("[ESLEEP Mode Test]\n");
		UART_Printf("Test pattern for SDRAM Self-Refresh is filled!\n");
		InitData_SDRAM(_DRAM_BaseAddress+0x1000000, 0x10000);

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

		// Fill Information Register - for test
		uInform0 = 0xABCD6400;
		uInform7 = 0x6400ABCD;
		Outp32Inform(0,uInform0);
		Outp32Inform(7,uInform7);

		
		UART_Printf("Wake-up source is set!\n");
		////////////////////////////////
		// Wake-up Source Setting
		//      Wake-up Source Interrupts are must enabled at Stop Mode
		////////////////////////////////
   		SetAlarmWakeup();
		SetEintWakeup();
		Outp32SYSC(0x804, 0x1<<7);			// nBATF Wake-up Mask disable
		SetBatteryFaultEvent(eFLT_INT);

		UART_Printf("System stablilization counter register!\n");
		SYSC_OSCSTAB(1);
		SYSC_PWRSTAB(1);

   	  	/////////////////////////////////
   	    	// 1) Check&Enable HCLK_GATE[22,21,0]
      		// 2) Check BLK_PWR_STAT 
	       // 3) Check&Clear WAKEUP_STAT
	       //////////////////////////////
		CheckHCLK_ForPM();
	       CheckBLKPWR_ForPM();
		CheckWAKESTAT_ForPM(1);

		
		UART_Printf("Choose Wake-up Event :  0: Warm Reset,  1: All Sleep Wake-up Source\n");
		uINTSEL = UART_GetIntNum();

		//Outp32SYSC(0x804, 0x80); 	// for test
		SYSC_SetBATF(eFLT_SLEEP, uINTSEL);
		//Outp32SYSC(0x804, 0x18);

		UART_Printf("OTHERs: 0x%x\n", Inp32SYSC(0x900));
		UART_Printf("PWR_CFG: 0x%x\n", Inp32SYSC(0x804));

		
		while(!UART_GetKey())
		{

		DisplayLED(0xC);
		Delay(3000);
		DisplayLED(0xF);
		Delay(3000);

		}

		UART_Printf("CPU doesn't go to Sleep Mode\n");

	}

	

}




//////////
// Function Name : IDLET_CFG 
// Function Description : Idle Mode Test - Validation Test ( 1 Int. Source Enable, Not adapted DVS Scheme)
// Input : 	None
// Output :	None 
// Version : v0.1

void IDLET_CFG (void)
{
	
	u32 uTemp, uRegValue;

	UART_Printf("[IDLE Mode Baisc Test]\n");
	UART_Printf("After 10 seconds, S3C6410 will wake up by RTC alarm interrupt.\n");
	UART_Printf("S3C6410 will also wake up by EINT9, EINT10, EINT11 or BATT_FLT.\n");
	
	uTemp=0;
	
	UART_Printf("Choose Battery Fault Event :  0: Interrupt,  1: ESLEEP\n");
	uTemp = UART_GetIntNum();

	switch(uTemp)
	{
		case 0: 
			SetBatteryFaultEvent(eFLT_INT);
			break;
		case 1:
			InitData_SDRAM(_DRAM_BaseAddress+0x1000000, 0x10000);
			SetBatteryFaultEvent(eFLT_SLEEP);				// IDLE 도중에 들어가지 않는다...:
		       break;
	}


	// Wake-up Source Setting
	UART_Printf("Wake-up sourceis set! \n");
	SetAlarmWakeup();
	SetEintWakeup();
	SetADCTSWakeup();
	//SetAlarmTick();
	
	// For Test(Temp) - Inform Register
	Outp32Inform(0,0xABCD6400);
	
	// Enter Idle Mode
	// CFG_STANDBYWFI = ignore  or  IDLE
	UART_Printf("CPU will goes to IDLE Mode \n");
	//SYSC_SetCFGWFI(eIDLE,1);		// IDLE,  Clock Enable (O.K)
	//SYSC_SetCFGWFI(eIGNORE,1);	// IGNORE,  Clock Enable(O.K)
	SYSC_SetCFGWFI(eIDLE,0);		// IDLE,  Clock Disable
	//Outp32SYSC(0x800, 0x1);			// Fail

       //Test case : bus power down
       SYSC_BLKPwrOffAll();

       //Added case : Clock Off Case
        Outp32SYSC(0x30, 0xFDDFFFFE);	//IROM, MEM0, MFC

#if 0
	//for test => hidden bit test PWR_CFG[1]
	//Outp32SYSC(0x804, 0xB6);			// reset 됨 ??
	// Outp32SYSC(0x804,0x17);			// 반응 없음. 
	//Outp32SYSC(0x804, 0xB0);			// 반응 없음. 
	//Outp32SYSC(0x804, 0xB4);			// 반응 없음.
	//Outp32SYSC(0x804, 0xB2);				// Reset 됨 
	//Outp32SYSC(0x804, 0x8F);
	//Outp32SYSC(0x804, 0x82);
	//UART_Printf("PWR_CFG: 0x%x\n", Inp32SYSC(0x804));
	//while(!UART_GetKey());
#endif

	// Test Case - Bus Transfer Test 	
	DMAC_InitCh(DMA0, DMA_ALL, &oDmac0_T);
	INTC_SetVectAddr(NUM_DMA0,  Dma0Done_T);
	//INTC_Enable(NUM_DMA0);
  	DMACH_Setup(DMA_A, 0x0, 0x52000000, 0, 0x53000000, 0, BYTE, 0x100000, DEMAND, MEM, MEM, SINGLE, &oDmac0_T);
        // Enable DMA
	DMACH_Start(&oDmac0_T);
	
	// Enter Idle Mode
	MMU_WaitForInterrupt();


	UART_Printf("Return to Normal Mode \n\n");
	
	UART_Printf("rWAKEUP_STAT=0x%x\n", Inp32SYSC(0x908));
	uRegValue=Inp32SYSC(0x908);
	Outp32SYSC(0x908,uRegValue);
	UART_Printf("rWAKEUP_STAT=0x%x\n Wake-up Status Clear\n", Inp32SYSC(0x908));
       SYSC_BLKPwrONAll();
       Outp32SYSC(0x30, 0xFFFFFFFF);	//IROM, MEM0, MFC
	SYSC_RdBLKPWR();

	// Int. Disable
	INTC_Disable(NUM_EINT1);
	INTC_Disable(NUM_RTC_ALARM);
	INTC_Disable(NUM_BATF);
	INTC_Disable(NUM_RTC_TIC);

}

//////////
// Function Name : STOPT_CFG  -- ing..
// Function Description : Stop Mode Test - Baisc Test 
// Input : 	None
// Output :	None 
// Version : v0.1
void STOPT_CFG_EVT1 (void)
{
	u32 uTemp;
	u32  uOSCEN;
	//u32 uRegValue2_bk, uRegValue3_bk;
	//u32 uRegValue_SRC, uRegValue_DIV0;
	//u32 uTTT0, uTTT1;
	u32 uARM_Memory,uTop_Memory,uARM_Logic,uTop_Logic, uHidden;
	u32 uOSCTime;
	u32 uTemp0, uTemp2;
	u32 uRet, uDSTOP;


	uDSTOP= DeepStopStatus();
	SYSC_RdRSTSTAT(1);

	if( uDSTOP && !(g_OnTest_DSTOP) )									// must be changed
	{
		
		CheckData_SDRAM(_DRAM_BaseAddress+0x1000000, 0x10000);

		//Check & Clear Wake-up Source 
		SYSC_ClrWKUPSTAT();

		UART_Printf("\nCheck Stepping Stone...\n\n");
		uRet = Compare(0x0C000000, 0x52000000, 0x100);
		if (uRet)
			{
			UART_Printf("Stepping Stone Data Retention Success..\n");
			}
			else
			{
			UART_Printf("Stepping Stone Data Retention  Failed..\n");
			}

		//Read RawInterrupt Status
		UART_Printf("VIC0RawStatus: 0x%x \n", Inp32(0x71200008));
		UART_Printf("VIC1RawStatus: 0x%x \n", Inp32(0x71300008));

		UART_Printf("Deep Stop mode test is done\n");
		g_OnTest_DSTOP = 1;
	}
else			// Entering into Deep Stop Mode
	{
		g_OnTest_DSTOP=0;

	UART_Printf("\n[STOP CFG Test]\n");

//test 2.24  -Operating Mode Change
/*
	SYSC_ChangeMode(eASYNC_MODE);
	SYSC_GetClkInform();
	UART_InitDebugCh(0, 115200);
	UART_Printf("\n\n");
	UART_Printf("ARMCLK: %.2fMHz  HCLKx2: %.2fMHz  HCLK: %.2fMHz  PCLK: %.2fMHz\n",(float)g_ARMCLK/1.0e6, (float)g_HCLKx2/1.0e6, (float)g_HCLK/1.0e6, (float)g_PCLK/1.0e6);
	UART_Printf("SYNC Mode : %d\n", g_SYNCACK);
	UART_Printf("\n");    
*/

	//GPIO_SetFunctionEach(eGPIO_F, eGPIO_14, 1);
	//GPIO_SetDataEach(eGPIO_F, eGPIO_14, 0);

	// User Key Input
	UART_Printf(" Choose Oscillator Enable or Disable :  0 : Disable,  1: Enable \n");
	uOSCEN = UART_GetIntNum();
	UART_Printf(" Choose uARM_Logic :  0 : OFF,  1: ON \n");
	uARM_Logic = UART_GetIntNum();
	UART_Printf(" Choose uARM_Memory :  0 : OFF,  1: ON \n");
	uARM_Memory = UART_GetIntNum();
	UART_Printf(" Choose uTop_Logic :  0 : RET  1: ON \n");
	uTop_Logic = UART_GetIntNum();
	UART_Printf(" Choose uTop_Memory :  0 : OFF,  1: RET\n");
	uTop_Memory = UART_GetIntNum();

	UART_Printf(" Choose Hidden Stop Mode:  0 : Sub Block OFF,  1: Sub Block Enable\n");
	uHidden = UART_GetIntNum();

	UART_Printf(" Oscillator settle-down time:  \n");
	uOSCTime = UART_GetIntNum();


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

	Outp32Inform(5, 0x0);
	UART_Printf("Wake-up source is set!\n");
	////////////////////////////////
	// Wake-up Source Setting
	//      Wake-up Source Interrupts are must enabled at Stop Mode
	////////////////////////////////
   	//SetAlarmWakeup();
	SetEintWakeup();
	SetKeypadWakeup();
	//SetAlarmTick();
	Outp32SYSC(0x804, 0x1<<7);
	SetBatteryFaultEvent(eFLT_INT);
	SetADCTSWakeup();
//	SetHSIWakeup();

	// For Test - Retention Stepping Stone
	UART_Printf("Test pattern for SDRAM Self-Refresh is filled!\n");
	InitData_SDRAM(_DRAM_BaseAddress+0x1000000, 0x10000);
	UART_Printf("\nCopy Stepping Stone...\n\n");
	Copy(0x0C000000, 0x52000000, 0x100);


	//UART_Printf("Oscillator settle-down time is set\n");
	//SYSC_OSCSTAB(1);				// Check...
	
#if 1

	//Test Case - Normal CFG OFF
	SYSC_CtrlNORCFG(eNOR_DOMAINV, 0);
	SYSC_CtrlNORCFG(eNOR_DOMAINI, 0);
	SYSC_CtrlNORCFG(eNOR_DOMAINP, 0);
	SYSC_CtrlNORCFG(eNOR_DOMAINF, 0);
	SYSC_CtrlNORCFG(eNOR_DOMAINS, 0);
	SYSC_CtrlNORCFG(eNOR_DOMAINETM, 1);
	SYSC_CtrlNORCFG(eNOR_IROM, 1);
#endif
	Delay(100);
	SYSC_RdBLKPWR();


	
#if 0
	UART_Printf("PHY OFF\n");
	uTTT1 = Inp32SYSC(0x900);
	uTTT1 = uTTT1|(1<<14);		
	Outp32SYSC(0x900, uTTT1);	//  USB_PWR_DN_EN
	//Outp32(0x7C100000, 0xF);		//  USB_Power Down
	uTTT1 = Inp32(0x7C100004);
	uTTT1 = uTTT1 |(1<<3);
	Outp32(0x7C100004,uTTT1 );			//  
	
	Outp32(0x7C100000, 0xF);		//  USB_Power Down
#endif	
	
	//For Test. (07.02.12- )
	SYSC_OSCSTAB(uOSCTime);			
	SYSC_PWRSTAB(0x1);			
	SYSC_FPCSTAB(0x1);			

	switch(uHidden)
	{
		case 0:
				SYSC_SetSTOPCFG(uARM_Memory, uTop_Memory, uARM_Logic, uTop_Logic);
				uTemp0=Inp32SYSC(0x814);
				//uTemp0=Inp32SYSC(0x814);
				//uTemp0 |=(1<<31);
				//Outp32SYSC(0x814, uTemp0);
				UART_Printf("Normal_CFG: 0x%x\n",Inp32SYSC(0x810));
				UART_Printf("STOP_CFG: 0x%x\n", uTemp0);
				break;
		case 1:
				uTemp0 = Inp32SYSC(0x814);
				uTemp2 = ((1<<16)|(1<<15)|(1<<14)|(1<<13)|(1<<12)|(1<<11)|(1<<10)|(1<<9)); 	// STOP(SUB Enable) -> SLEEP  Fail
				//uTemp2 = ((0<<16)|(0<<15)|(0<<14)|(0<<13)|(0<<12)|(0<<9));
				uTemp0 =  (uTemp0 & ~(0x3FFFFF<<8)) | ((uARM_Memory<<29)|(uTop_Memory<<20)|(uARM_Logic<<17)|(uTop_Logic<<8)|uTemp2);	
				Outp32SYSC(0x814, uTemp0);
				UART_Printf("Normal_CFG: 0x%x\n",Inp32SYSC(0x810));
				UART_Printf("STOP_CFG: 0x%x\n", uTemp0);
				break;
	}

	// for Test  WFI Wake-up 
	//INTC_Disable(NUM_RTC_TIC);
	//INTC_Disable(NUM_RTC_ALARM);
	//INTC_Disable(NUM_EINT1);
	
	//SYSTEM_DisableVIC( );
	//SYSC_SetSTOPCFG(uDeepStop,1, uDeepStop, 0);   // for test 2.12,  TOP Memory ON
	SYSC_SetCFGWFI(eSTOP, uOSCEN);
      UART_Printf("PWR_CFG: 0x%x\n", Inp32SYSC(0x804));
      Delay(10);



       /////////////////////////////////
       // 1) Check&Enable HCLK_GATE[22,21,0]
       // 2) Check BLK_PWR_STAT 
       // 3) Check&Clear WAKEUP_STAT
       //////////////////////////////
	CheckHCLK_ForPM();
       CheckBLKPWR_ForPM();
	CheckWAKESTAT_ForPM(1);
      
	// Normal CFG 
	UART_Printf("\nCPU will goes to Stop Mode!\n\n");
      UART_TxEmpty();	
	Outp32Inform(7, Inp32SYSC(0x90c));

  	MMU_WaitForInterrupt();


	Delay(10);


	DisplayLED(0x3);
	//-daedoo
	//RTC_SetCON(0,0,0,0,0,0);	
	RTC_Enable(false);
	
	UART_Printf("\nCheck Stepping Stone...\n\n");
	uRet = Compare(0x0C000000, 0x52000000, 0x100);
	if (uRet)
			{
			UART_Printf("Stepping Stone Data Retention Success..\n");
			}
			else
			{
			UART_Printf("Stepping Stone Data Retention  Failed..\n");
			}

	
	UART_Printf("Return to Normal Mode.\n");

	UART_Printf("rWAKEUP_STAT=0x%x\n", Inp32SYSC(0x908));
	uTemp=Inp32SYSC(0x908);
	Outp32SYSC(0x908,uTemp);
	UART_Printf("rWAKEUP_STAT=0x%x\n\n Wake-up Status Clear", Inp32SYSC(0x908));
	
	SYSC_RdBLKPWR();

	// Int. Disable
	INTC_Disable(NUM_EINT1);
	INTC_Disable(NUM_RTC_ALARM);
	INTC_Disable(NUM_RTC_TIC);
	
}

	INTC_Disable(NUM_EINT1);
	INTC_Disable(NUM_RTC_ALARM);
	INTC_Disable(NUM_BATF);
	INTC_Disable(NUM_PENDNUP);

}



//////////
// Function Name : SLEEPT_CFG_EVT1  -- ing..
// Function Description : Sleep Mode Test - Baisc Test 
// Input : 	None
// Output :	None 
// Version : v0.1
void SLEEPT_CFG_EVT1 (void)
{
	u32 uRstId;
	//u32 uPortStatus[32];		// must be change
	u32 uInform0, uInform7;
	u32 uTTT1;
		
	UART_Printf("rINFORM0: 0x%x\n", Inp32Inform(0));
	UART_Printf("rINFORM7: 0x%x\n", Inp32Inform(7));

	uInform0 = 0xABCD6400;
	uInform7 = 0x6400ABCD;
	
	uRstId = SYSC_RdRSTSTAT(1);
	
		//SYSC_CtrlNORCFG(eNOR_DOMAINV, 0);
		SYSC_CtrlNORCFG(eNOR_DOMAINI, 1);
		SYSC_CtrlNORCFG(eNOR_DOMAINP, 1);
		SYSC_CtrlNORCFG(eNOR_DOMAINF, 1);
		SYSC_CtrlNORCFG(eNOR_DOMAINS, 1);
		//SYSC_CtrlNORCFG(eNOR_DOMAINETM,0);		// OFF시 Sleep Wake-up Fail...
		SYSC_CtrlNORCFG(eNOR_IROM, 1);

	Delay(10);
	SYSC_RdBLKPWR();
	if( ( uRstId == 3 ) && !(g_OnTest) )
	{
		UART_Printf("Wake-up form SLEEP Mode \n");
		CheckData_SDRAM(_DRAM_BaseAddress+0x1000000, 0x10000);

		//Check Information Register Value
		if( (uInform0 !=Inp32Inform(0) )||(uInform7 != Inp32Inform(7)))
		{
			UART_Printf(" Information Register Value is wrong!!! \n");
		}
		else 
		{
			UART_Printf(" Information Register Value is correct!!! \n");
		}

		//Check & Clear Wake-up Source 
		SYSC_ClrWKUPSTAT();
		//test
		Check_AliveSFR(1);
		
		UART_Printf("SLEEP mode test is done\n");
		g_OnTest = 1;
	}
	else			// Entering into SLEEP Mode
	{
		g_OnTest=0;
		
		UART_Printf("[SLEEP Mode Test]\n");
		UART_Printf("S3C6410 will wake-up by EINT9~11 or Other wake-up sources\n");
				
		
		//Lcd_EnvidOnOff(0);				//Before entering SLEEP mode, LCD must be off.
		ADC_SelectStandbyMode(1);		//ADC Stand-by
		//-daedoo
		//RTC_SetCON(0,0,0,0,0,0);			//RTC SFR R/W Disable.
		RTC_Enable(false);

		// Save the port configurations
		UART_Printf(" I/O Port is stored! \n");
	
		/*
		for (i=0;i<32;i++)
		{
			uPortStatus[i] = *( (volatile u32 *)0x7F008000 + i );
		}
		*/

		

		UART_Printf("Test pattern for SDRAM Self-Refresh is filled!\n");
		InitData_SDRAM(_DRAM_BaseAddress+0x1000000, 0x10000);
		// Fill Information Register - for test
		uInform0 = 0xABCD6400;
		uInform7 = 0x6400ABCD;
		Outp32Inform(0,uInform0);
		Outp32Inform(0,uInform7);

		// ADCCON Standby
		//Outp32(0x7E00B008,0x100FF);
		//Outp32(0x7E00B000, 0x3FC4);

		UART_Printf("Wake-up source is set!\n");
		SetAlarmWakeup();
		SetEintWakeup();
		//SetKeypadWakeup();
		//SetAlarmTick();
		//SetBatteryFaultEvent(eFLT_INT);
		//SetADCTSWakeup();

		//Outp32(0x7E00B000, 0x3FC4);  //ADC Clock Disable
		//Outp32(0x7E00B004, 0x58);
		//Outp32(0x7E00B008, 5	);  //ADC Clock Disable


		//UART_Printf(" I/O Port is set for Sleep Mode \n");
		//ConfigSleepGPIO();
#if 0	// OK		
		Outp32(0x7F008880, 0x1000);   
	
              UART_Printf("rSPCONSLP, SPLEN: 0x%x, 0x%x\n", Inp32(0x7F008880),Inp32(0x7F008930));
#endif              
		UART_Printf("System stablilization counter register!\n");
		SYSC_OSCSTAB(0x1);			
		SYSC_PWRSTAB(0x1);			
		SYSC_FPCSTAB(0x1);	
		//Test Case - Normal CFG OFF
		//SYSC_CtrlNORCFG(eNOR_DOMAINV, 0);
		//SYSC_CtrlNORCFG(eNOR_DOMAINI, 0);
		//SYSC_CtrlNORCFG(eNOR_DOMAINP, 0);
		//SYSC_CtrlNORCFG(eNOR_DOMAINF, 0);
		//SYSC_CtrlNORCFG(eNOR_DOMAINS, 0);
		//SYSC_CtrlNORCFG(eNOR_DOMAINETM, 0);		// OFF시 Sleep Wake-up Fail...
		//SYSC_CtrlNORCFG(eNOR_IROM, 0);
		Delay(100);
		SYSC_RdBLKPWR();
#if 0
		//Outp32(0x7F008880, 0x1000);   
              UART_Printf("rSPCONSLP, SPLEN: 0x%x, 0x%x\n", Inp32(0x7F008880),Inp32(0x7F008930));
#endif    

		SYSC_OSCSTAB(0x8);			
		SYSC_PWRSTAB(0x8);			
		SYSC_FPCSTAB(0x8);	

#if 0	// OK
		Outp32(0x7F008880, 0x1010);   
		Outp32(0x7F008930, 0x2);
		UART_Printf("rSPCONSLP, SPLEN: 0x%x, 0x%x\n", Inp32(0x7F008880),Inp32(0x7F008930));
#endif

		//Add Clock Gating
              //  Outp32SYSC(0x30, 0xFFFFFFFE);	// MFC, MFC Block OFF시 OK
             //  Outp32SYSC(0x30, 0xFDFFFFFF);	// IROM  OK
             //Outp32SYSC(0x30, 0xFFDFFFFF);	// MEM0
		//UART_Printf("HCLKGATE: 0x%x\n", Inp32(0x7E00F030));



		//UART_Getc();
		//UART_Printf("EINTPend:0x%x\n", Inp32(0x7F008924));
		//UART_Getc();

		//test
		//Check_AliveSFR(0);
	
		//USB_OTG_POWER_OFF
		UART_Printf("PHY OFF\n");	
		uTTT1 = Inp32SYSC(0x900);
		uTTT1 = uTTT1|(1<<14);	
		//uTTT1 = uTTT1&~(1<<16)|(1<<14);	
		Outp32SYSC(0x900, uTTT1);	//  USB_PWR_DN_EN
		Outp32(0x7C100000, 0xF);		//  USB_Power Down
		uTTT1 = Inp32(0x7C100004);
		uTTT1 = uTTT1 |(1<<3);
		Outp32(0x7C100004,uTTT1 );	

#if 0
	uRegValue =Inp32(0x7E00F90C);

	UART_Printf("Block Power Status Register	0x%x\n\n\n",  uRegValue);

	UART_Printf("Block Top Power	%d\n",  uRegValue&(0x1<<0));
	UART_Printf("Block V    Power	%d\n",  (uRegValue&(0x1<<1))>>1);
	UART_Printf("Block I	    Power	%d\n",  (uRegValue&(0x1<<2))>>2);	
	UART_Printf("Block P    Power	%d\n",  (uRegValue&(0x1<<3))>>3);	
	UART_Printf("Block F	    Power	%d\n",  (uRegValue&(0x1<<4))>>4);	
	UART_Printf("Block S    Power	%d\n",  (uRegValue&(0x1<<5))>>5);	
	UART_Printf("Block ETM    Power	%d\n",  (uRegValue&(0x1<<6))>>6);	
#endif

		SYSC_RdBLKPWR();
		UART_Printf("\nCPU will goes to SLEEP Mode!\n\n");
		UART_TxEmpty();				// 

	INTC_Disable(NUM_RTC_TIC);
	INTC_Disable(NUM_RTC_ALARM);
	INTC_Disable(NUM_EINT1);
 	
		
		//Outp32SYSC(0x808, 0xFFFFFDFF );   // EINT MASK
		//Outp32SYSC(0x804, 0x61);
		//Outp32SYSC(0x818,1);

		//while(1);
		//Outp32SYSC(0x804,0x1FF80);    // WAKE_UP MASK ? or DisMASK
		// Normal CFG 
#if 0		
		// Reset Out
		Outp32(0x7F008880, 0x1000);   
		//Outp32(0x7F008880, 0x1010);   //OK
              UART_Printf("rSPCONSLP, SPLEN: 0x%x, 0x%x\n", Inp32(0x7F008880),Inp32(0x7F008930));
		//Outp32(0x7F008820, 0x200000);
		Outp32Inform(7, Inp32SYSC(0x90c));
		//UART_Getc();  //Outp32(0x7F008880, 0x1000);   
#endif		

			
		SYSC_SetCFGWFI(eSLEEP, 0); 
			
		MMU_WaitForInterrupt();
	

		UART_Printf("CPU doesn't go to Sleep Mode\n");

	}

	

}



//////////
// Function Name : NORCFG_Test - ing
// Function Description : Block Power Off Test in the Normal Mode
// Input : 	None
// Output :	None 
// Version : v0.1

void NORCFG_Test (void)
{
	
	u32 uTemp;
			
	//Block Power Stablilzation counter
	//[27:24] ETM, [23:20] DOMAIN_S, [19:16] DOMAIN_F, [15:12] DOMAIN_P
	//[11:8] DOMAIN_I, [7:4] DOMAIN_V, [3:0] DOMAIN_TOP
	uTemp = (0xF<<24)|(0xF<<20)|(0xF<<16)|(0xF<<12)|(0xF<<8)|(0xF<<4)|(0xF<<0);
	SYSC_MTCSTAB(uTemp);
	
	UART_Printf("DOMAIN_V Block Power Off- MFC(?)- Press any key \n");
	while(!UART_GetKey());
	SYSC_CtrlNORCFG(eNOR_DOMAINV, 0);
	Delay(100);
	SYSC_RdBLKPWR();

	// Test

	UART_Printf("\nDOMAIN_ETM Block Power Off- ETM - Press any key \n");
	while(!UART_GetKey());
	SYSC_CtrlNORCFG(eNOR_DOMAINETM, 0);
	Delay(100);

	SYSC_RdBLKPWR();

	UART_Printf("\nDOMAIN_ETM Block Power ON- ETM - Press any key \n");
//	while(!UART_GetKey());
	SYSC_CtrlNORCFG(eNOR_DOMAINETM, 1);
	Delay(100);

	SYSC_RdBLKPWR();

	//


	UART_Printf("\nDOMAIN_I Block Power Off- JPEG/CamIF- Press any key \n");
	while(!UART_GetKey());
	SYSC_CtrlNORCFG(eNOR_DOMAINI, 0);
	Delay(100);
	SYSC_RdBLKPWR();

	UART_Printf("\nDOMAIN_P Block Power Off- 2D/TV/Scaler- Press any key \n");
	while(!UART_GetKey());
	SYSC_CtrlNORCFG(eNOR_DOMAINP, 0);
	Delay(100);

	SYSC_RdBLKPWR();
	
	UART_Printf("\nDOMAIN_F Block Power Off- LCD/ROT/Post- Press any key \n");
	while(!UART_GetKey());
	SYSC_CtrlNORCFG(eNOR_DOMAINF, 0);
	Delay(100);

	SYSC_RdBLKPWR();
	
	UART_Printf("\nDOMAIN_S Block Power Off- SDMA/Security- Press any key \n");
	while(!UART_GetKey());
	SYSC_CtrlNORCFG(eNOR_DOMAINS, 0);
	Delay(100);

	SYSC_RdBLKPWR();
	
	UART_Printf("\nDOMAIN_ETM Block Power Off- ETM - Press any key \n");
	while(!UART_GetKey());
	SYSC_CtrlNORCFG(eNOR_DOMAINETM, 0);
	Delay(100);

	SYSC_RdBLKPWR();
	
	UART_Printf("\nIROM Block Power Off-  Press any key \n");
	while(!UART_GetKey());
	SYSC_CtrlNORCFG(eNOR_IROM, 0);
	Delay(100);

	SYSC_RdBLKPWR();



	UART_Printf("========================================== \n");
       UART_Printf("=============     Block Power ON    =============== \n");
	UART_Printf("========================================== \n");


	

	UART_Printf("DOMAIN_V Block Power ON- MFC(?)- Press any key \n");
	while(!UART_GetKey());
	SYSC_CtrlNORCFG(eNOR_DOMAINV, 1);
	Delay(100);

	SYSC_RdBLKPWR();

	UART_Printf("\nDOMAIN_I Block Power ON- JPEG/CamIF- Press any key \n");
	while(!UART_GetKey());
	SYSC_CtrlNORCFG(eNOR_DOMAINI, 1);
	Delay(100);

	SYSC_RdBLKPWR();
     
	UART_Printf("\nDOMAIN_P Block Power ON- 2D/TV/Scaler- Press any key \n");
	while(!UART_GetKey());
	SYSC_CtrlNORCFG(eNOR_DOMAINP, 1);
	Delay(100);

	SYSC_RdBLKPWR();

	UART_Printf("\nDOMAIN_F Block Power ON- LCD/ROT/Post- Press any key \n");
	while(!UART_GetKey());
	SYSC_CtrlNORCFG(eNOR_DOMAINF, 1);
	Delay(100);

	SYSC_RdBLKPWR();

	UART_Printf("\nDOMAIN_S Block Power ON- SDMA/Security- Press any key \n");
	while(!UART_GetKey());
	SYSC_CtrlNORCFG(eNOR_DOMAINS, 1);
	Delay(100);

	SYSC_RdBLKPWR();

	UART_Printf("\nDOMAIN_ETM Block Power ON- ETM - Press any key \n");
	while(!UART_GetKey());
	SYSC_CtrlNORCFG(eNOR_DOMAINETM, 1);
	Delay(100);

	SYSC_RdBLKPWR();

	UART_Printf("\nIROM Block Power ON-  Press any key \n");
	while(!UART_GetKey());
	SYSC_CtrlNORCFG(eNOR_IROM, 1);
	Delay(100);

	SYSC_RdBLKPWR();

}

//////////
// Function Name : CLKGate_Test 
// Function Description : CLKGate_Test in the Normal Mode 
// Input : 	None
// Output :	None 
// Version : v0.1
void CLKGate_Test(void)
{
	
	UART_Printf("HCLK/PCLK/SCLK_Gating Register Test\n");
	UART_Printf("Check the current cunsumption. Type any key to proceed.\n");	
	//Except GPIO,UART0, DMC1, VIC, BUS
    	UART_Printf("rHCLK_GATE: 0x%x, rPCLK_GATE: 0x%x, rSCLK_GATE: 0x%x \n", Inp32SYSC(0x30), Inp32SYSC(0x34), Inp32SYSC(0x38));

       
	while(!UART_GetKey());

	UART_Printf("No.1 : Security off (HCLK[29], SCLK[7]) \n");
	SYSC_CtrlHCLKGate(eHCLK_SECUR, 0);
	SYSC_CtrlSCLKGate(eSCLK_SECUR, 0);	
   	UART_Printf("rHCLK_GATE: 0x%x, rPCLK_GATE: 0x%x, rSCLK_GATE: 0x%x \n", Inp32SYSC(0x30), Inp32SYSC(0x34), Inp32SYSC(0x38));
	
	while(!UART_GetKey());
	
	UART_Printf("No.2 : SDMA1 off (HCLK[28]\n");
	SYSC_CtrlHCLKGate(eHCLK_SDMA1, 0);
	UART_Printf("rHCLK_GATE: 0x%x, rPCLK_GATE: 0x%x, rSCLK_GATE: 0x%x \n", Inp32SYSC(0x30), Inp32SYSC(0x34), Inp32SYSC(0x38));
	
	while(!UART_GetKey());	
	
	UART_Printf("No.3 : SDMA0 off (HCLK[27])\n");
	SYSC_CtrlHCLKGate(eHCLK_SDMA0, 0);
	UART_Printf("rHCLK_GATE: 0x%x, rPCLK_GATE: 0x%x, rSCLK_GATE: 0x%x \n", Inp32SYSC(0x30), Inp32SYSC(0x34), Inp32SYSC(0x38));
	
	while(!UART_GetKey());	
	
	UART_Printf("No.4 : DMA1 off (HCLK[13])\n");
	SYSC_CtrlHCLKGate(eHCLK_DMA1, 0);
	UART_Printf("rHCLK_GATE: 0x%x, rPCLK_GATE: 0x%x, rSCLK_GATE: 0x%x \n", Inp32SYSC(0x30), Inp32SYSC(0x34), Inp32SYSC(0x38));
	
	while(!UART_GetKey());	
	
	UART_Printf("No.5 : DMA0 off (HCLK[12])\n");
	SYSC_CtrlHCLKGate(eHCLK_DMA0, 0 );
	UART_Printf("rHCLK_GATE: 0x%x, rPCLK_GATE: 0x%x, rSCLK_GATE: 0x%x \n", Inp32SYSC(0x30), Inp32SYSC(0x34), Inp32SYSC(0x38));
	
	while(!UART_GetKey());	
	
	UART_Printf("No.6 : USB Host off (HCLK[26], SCLK[30])\n");
	SYSC_CtrlHCLKGate(eHCLK_UHOST, 0);
	SYSC_CtrlSCLKGate(eSCLK_UHOST, 0);
	UART_Printf("rHCLK_GATE: 0x%x, rPCLK_GATE: 0x%x, rSCLK_GATE: 0x%x \n", Inp32SYSC(0x30), Inp32SYSC(0x34), Inp32SYSC(0x38));
	
	while(!UART_GetKey());	
		
	UART_Printf("No.7 : Internal ROM off (HCLK[25])\n");
	SYSC_CtrlHCLKGate(eHCLK_IROM, 0);
	UART_Printf("rHCLK_GATE: 0x%x, rPCLK_GATE: 0x%x, rSCLK_GATE: 0x%x \n", Inp32SYSC(0x30), Inp32SYSC(0x34), Inp32SYSC(0x38));
	
	while(!UART_GetKey());	
	
	UART_Printf("No.8 : DMC0 off (HCLK[23], HCLK[21] )\n");
	SYSC_CtrlHCLKGate(eHCLK_DDR0, 0);
	SYSC_CtrlHCLKGate(eHCLK_DMC0, 0);
	UART_Printf("rHCLK_GATE: 0x%x, rPCLK_GATE: 0x%x, rSCLK_GATE: 0x%x \n", Inp32SYSC(0x30), Inp32SYSC(0x34), Inp32SYSC(0x38));
	
	while(!UART_GetKey());	
	
	UART_Printf("No.9 : USB OTG  off (HCLK[20])\n");
	SYSC_CtrlHCLKGate(eHCLK_OTG, 0);
	UART_Printf("rHCLK_GATE: 0x%x, rPCLK_GATE: 0x%x, rSCLK_GATE: 0x%x \n", Inp32SYSC(0x30), Inp32SYSC(0x34), Inp32SYSC(0x38));
	
	while(!UART_GetKey());	
	
	UART_Printf("No.10 : HSMMC2 off (HCLK[19], SCLK[29],[26])\n");
	SYSC_CtrlHCLKGate(eHCLK_MMC2, 0);
	SYSC_CtrlSCLKGate(eSCLK_MMC2_48,0);
	SYSC_CtrlSCLKGate(eSCLK_MMC2,0);
	UART_Printf("rHCLK_GATE: 0x%x, rPCLK_GATE: 0x%x, rSCLK_GATE: 0x%x \n", Inp32SYSC(0x30), Inp32SYSC(0x34), Inp32SYSC(0x38));
	
	while(!UART_GetKey());	
	
	UART_Printf("No.11 : HSMMC1 off (HCLK[18], SCLK[28], [25])\n");
	SYSC_CtrlHCLKGate(eHCLK_MMC1,0);
	SYSC_CtrlSCLKGate(eSCLK_MMC2_48,0);
	SYSC_CtrlSCLKGate(eSCLK_MMC2,0);
	UART_Printf("rHCLK_GATE: 0x%x, rPCLK_GATE: 0x%x, rSCLK_GATE: 0x%x \n", Inp32SYSC(0x30), Inp32SYSC(0x34), Inp32SYSC(0x38));
	
	while(!UART_GetKey());	
	
	UART_Printf("No.12 : HSMMC0 off (HCLK[17], SCLK[27],[24])\n");
	SYSC_CtrlHCLKGate(eHCLK_MMC0,0);
	SYSC_CtrlSCLKGate(eSCLK_MMC0_48,0);
	SYSC_CtrlSCLKGate(eSCLK_MMC0,0);
	UART_Printf("rHCLK_GATE: 0x%x, rPCLK_GATE: 0x%x, rSCLK_GATE: 0x%x \n", Inp32SYSC(0x30), Inp32SYSC(0x34), Inp32SYSC(0x38));
	
	while(!UART_GetKey());	
	
	UART_Printf("No.13: MDP off (HCLK[16],)\n");
	SYSC_CtrlHCLKGate(eHCLK_MDP,0);
	UART_Printf("rHCLK_GATE: 0x%x, rPCLK_GATE: 0x%x, rSCLK_GATE: 0x%x \n", Inp32SYSC(0x30), Inp32SYSC(0x34), Inp32SYSC(0x38));
	
	while(!UART_GetKey());	
	
	UART_Printf("No.14 : Direct Host I/F off (HCLK[15])\n");
	SYSC_CtrlHCLKGate(eHCLK_Modem, 0);
	UART_Printf("rHCLK_GATE: 0x%x, rPCLK_GATE: 0x%x, rSCLK_GATE: 0x%x \n", Inp32SYSC(0x30), Inp32SYSC(0x34), Inp32SYSC(0x38));
	
	while(!UART_GetKey());	
	
	UART_Printf("No.15 : Indirect Host I/F off (PCLK[14])\n");
	SYSC_CtrlHCLKGate(eHCLK_HOSTIF,0);
	UART_Printf("rHCLK_GATE: 0x%x, rPCLK_GATE: 0x%x, rSCLK_GATE: 0x%x \n", Inp32SYSC(0x30), Inp32SYSC(0x34), Inp32SYSC(0x38));
	
	while(!UART_GetKey());	
	
	UART_Printf("No.16 : JPEG off (HCLK[11], SCLK[1])\n");
	SYSC_CtrlHCLKGate(eHCLK_JPEG,0);
	SYSC_CtrlSCLKGate(eSCLK_JPEG,0);
	UART_Printf("rHCLK_GATE: 0x%x, rPCLK_GATE: 0x%x, rSCLK_GATE: 0x%x \n", Inp32SYSC(0x30), Inp32SYSC(0x34), Inp32SYSC(0x38));
	
	while(!UART_GetKey());	
	
	UART_Printf("No.17 : CAMIF off (HCLK[10], SCLK[2])\n");
	SYSC_CtrlHCLKGate(eHCLK_CAM,0);
	SYSC_CtrlSCLKGate(eSCLK_CAM,0);
	UART_Printf("rHCLK_GATE: 0x%x, rPCLK_GATE: 0x%x, rSCLK_GATE: 0x%x \n", Inp32SYSC(0x30), Inp32SYSC(0x34), Inp32SYSC(0x38));
	
	while(!UART_GetKey());	
	
	UART_Printf("No.18 : SCALER off (HCLK[9],  SCLK[17:16])\n");
	SYSC_CtrlHCLKGate(eHCLK_SCALER,0);
	SYSC_CtrlSCLKGate(eSCLK_SCALER27,0);
	SYSC_CtrlSCLKGate(eSCLK_SCALER,0);
	UART_Printf("rHCLK_GATE: 0x%x, rPCLK_GATE: 0x%x, rSCLK_GATE: 0x%x \n", Inp32SYSC(0x30), Inp32SYSC(0x34), Inp32SYSC(0x38));
	
	while(!UART_GetKey());	
	
	UART_Printf("No.19 : 2D off (HCLK[8])\n");
	SYSC_CtrlHCLKGate(eHCLK_2D,0);
	UART_Printf("rHCLK_GATE: 0x%x, rPCLK_GATE: 0x%x, rSCLK_GATE: 0x%x \n", Inp32SYSC(0x30), Inp32SYSC(0x34), Inp32SYSC(0x38));
	
	while(!UART_GetKey());	
	
	UART_Printf("No.20 : TV off (HCLK[7], SCLK[19:18])\n");
	SYSC_CtrlHCLKGate(eHCLK_TV,0);
	SYSC_CtrlSCLKGate(eSCLK_DAC27,0);
	SYSC_CtrlSCLKGate(eSCLK_TV27,0);
	UART_Printf("rHCLK_GATE: 0x%x, rPCLK_GATE: 0x%x, rSCLK_GATE: 0x%x \n", Inp32SYSC(0x30), Inp32SYSC(0x34), Inp32SYSC(0x38));
	
	while(!UART_GetKey());	
		
	UART_Printf("No.21 : POST0 off (HCLK[5], SCLK[12,10])\n");
	SYSC_CtrlHCLKGate(eHCLK_POST, 0);
	SYSC_CtrlSCLKGate(eSCLK_POST0_27,0);
	SYSC_CtrlSCLKGate(eSCLK_POST0,0);
	UART_Printf("rHCLK_GATE: 0x%x, rPCLK_GATE: 0x%x, rSCLK_GATE: 0x%x \n", Inp32SYSC(0x30), Inp32SYSC(0x34), Inp32SYSC(0x38));
	
	while(!UART_GetKey());	
	
	UART_Printf("No.22 : Rotator off (HCLK[4]\n");
	SYSC_CtrlHCLKGate(eHCLK_ROT,0);
	UART_Printf("rHCLK_GATE: 0x%x, rPCLK_GATE: 0x%x, rSCLK_GATE: 0x%x \n", Inp32SYSC(0x30), Inp32SYSC(0x34), Inp32SYSC(0x38));
	
	while(!UART_GetKey());	
	
	UART_Printf("No.23 : LCD off (HCLK[3], SCLK[15:14])\n");
	SYSC_CtrlHCLKGate(eHCLK_LCD,0);
	SYSC_CtrlSCLKGate(eSCLK_LCD_27,0);
	SYSC_CtrlSCLKGate(eSCLK_LCD,0);
	UART_Printf("rHCLK_GATE: 0x%x, rPCLK_GATE: 0x%x, rSCLK_GATE: 0x%x \n", Inp32SYSC(0x30), Inp32SYSC(0x34), Inp32SYSC(0x38));
	
	while(!UART_GetKey());	
	
	UART_Printf("No.24 :MFC off (HCLK[0], PCLK[0], SCLK[3])\n");
	SYSC_CtrlHCLKGate(eHCLK_MFC,0);
	SYSC_CtrlPCLKGate(ePCLK_MFC,0);
	SYSC_CtrlSCLKGate(eSCLK_MFC,0);
	UART_Printf("rHCLK_GATE: 0x%x, rPCLK_GATE: 0x%x, rSCLK_GATE: 0x%x \n", Inp32SYSC(0x30), Inp32SYSC(0x34), Inp32SYSC(0x38));
	
	while(!UART_GetKey());	
	
	UART_Printf("No.25 : SKEY off (PCLK[24], )\n");
	SYSC_CtrlPCLKGate(ePCLK_SKEY,0);
	UART_Printf("rHCLK_GATE: 0x%x, rPCLK_GATE: 0x%x, rSCLK_GATE: 0x%x \n", Inp32SYSC(0x30), Inp32SYSC(0x34), Inp32SYSC(0x38));
	
	while(!UART_GetKey());	
	
	UART_Printf("No.26 : CHIP ID off (PCLK[23])\n");
	SYSC_CtrlPCLKGate(ePCLK_CHIPID,0);
	UART_Printf("rHCLK_GATE: 0x%x, rPCLK_GATE: 0x%x, rSCLK_GATE: 0x%x \n", Inp32SYSC(0x30), Inp32SYSC(0x34), Inp32SYSC(0x38));
	
	while(!UART_GetKey());	
	
	UART_Printf("No.27 : SPI1 off (PCLK[22], SCLK[23,21])\n");
	SYSC_CtrlPCLKGate(ePCLK_SPI1,0);
	SYSC_CtrlSCLKGate(eSCLK_SPI1_48,0);
	SYSC_CtrlSCLKGate(eSCLK_SPI1,0);
	UART_Printf("rHCLK_GATE: 0x%x, rPCLK_GATE: 0x%x, rSCLK_GATE: 0x%x \n", Inp32SYSC(0x30), Inp32SYSC(0x34), Inp32SYSC(0x38));
	
	while(!UART_GetKey());	

	UART_Printf("No.28 : SPI0 off (PCLK[21], SCLK[22,20])\n");
	SYSC_CtrlPCLKGate(ePCLK_SPI0,0);
	SYSC_CtrlSCLKGate(eSCLK_SPI0_48,0);
	SYSC_CtrlSCLKGate(eSCLK_SPI0,0);
	UART_Printf("rHCLK_GATE: 0x%x, rPCLK_GATE: 0x%x, rSCLK_GATE: 0x%x \n", Inp32SYSC(0x30), Inp32SYSC(0x34), Inp32SYSC(0x38));

	while(!UART_GetKey());		
	
	UART_Printf("No.29 : HSI off (PCLK[20:19])\n");
	SYSC_CtrlPCLKGate(ePCLK_HSITX,0);
	SYSC_CtrlPCLKGate(ePCLK_HSIRX,0);
	UART_Printf("rHCLK_GATE: 0x%x, rPCLK_GATE: 0x%x, rSCLK_GATE: 0x%x \n", Inp32SYSC(0x30), Inp32SYSC(0x34), Inp32SYSC(0x38));
	
	while(!UART_GetKey());		
		
	UART_Printf("No.30 : I2C off (PCLK[17])\n");
	SYSC_CtrlPCLKGate(ePCLK_I2C,0);
	UART_Printf("rHCLK_GATE: 0x%x, rPCLK_GATE: 0x%x, rSCLK_GATE: 0x%x \n", Inp32SYSC(0x30), Inp32SYSC(0x34), Inp32SYSC(0x38));
	
	while(!UART_GetKey());	
	
	UART_Printf("No.31 : I2S1 off (PCLK[16], SCLK[9])\n");
	SYSC_CtrlPCLKGate(ePCLK_I2S1,0);
	SYSC_CtrlSCLKGate(eSCLK_AUDIO1,0);
	UART_Printf("rHCLK_GATE: 0x%x, rPCLK_GATE: 0x%x, rSCLK_GATE: 0x%x \n", Inp32SYSC(0x30), Inp32SYSC(0x34), Inp32SYSC(0x38));
	
	while(!UART_GetKey());	
	
	UART_Printf("No.32 : I2S0 off (PCLK[15], SCLK[8])\n");
	SYSC_CtrlPCLKGate(ePCLK_I2S0,0);
	SYSC_CtrlSCLKGate(eSCLK_AUDIO0,0);
	UART_Printf("rHCLK_GATE: 0x%x, rPCLK_GATE: 0x%x, rSCLK_GATE: 0x%x \n", Inp32SYSC(0x30), Inp32SYSC(0x34), Inp32SYSC(0x38));

	while(!UART_GetKey());		

	UART_Printf("No.33 : AC97 off (PCLK[14])\n");
	SYSC_CtrlPCLKGate(ePCLK_AC97,0);
	UART_Printf("rHCLK_GATE: 0x%x, rPCLK_GATE: 0x%x, rSCLK_GATE: 0x%x \n", Inp32SYSC(0x30), Inp32SYSC(0x34), Inp32SYSC(0x38));

	while(!UART_GetKey());	
	
	UART_Printf("No.34 : TZPC off (PCLK[13])\n");
	SYSC_CtrlPCLKGate(ePCLK_TZPC,0);
	UART_Printf("rHCLK_GATE: 0x%x, rPCLK_GATE: 0x%x, rSCLK_GATE: 0x%x \n", Inp32SYSC(0x30), Inp32SYSC(0x34), Inp32SYSC(0x38));
	
	while(!UART_GetKey());		
		
	UART_Printf("No.35 : TSADC off (PCLK[12])\n");
	SYSC_CtrlPCLKGate(ePCLK_TSADC,0);
	UART_Printf("rHCLK_GATE: 0x%x, rPCLK_GATE: 0x%x, rSCLK_GATE: 0x%x \n", Inp32SYSC(0x30), Inp32SYSC(0x34), Inp32SYSC(0x38));
	
	while(!UART_GetKey());	

	UART_Printf("No.36 : KEYPAD off (PCLK[11])\n");
	SYSC_CtrlPCLKGate(ePCLK_KEYPAD,0);
	UART_Printf("rHCLK_GATE: 0x%x, rPCLK_GATE: 0x%x, rSCLK_GATE: 0x%x \n", Inp32SYSC(0x30), Inp32SYSC(0x34), Inp32SYSC(0x38));
	
	while(!UART_GetKey());	

	UART_Printf("No.37 : IrDA off (PCLK[10], SCLK[6])\n");
	SYSC_CtrlPCLKGate(ePCLK_IRDA,0);
	SYSC_CtrlSCLKGate(eSCLK_IRDA,0);
	UART_Printf("rHCLK_GATE: 0x%x, rPCLK_GATE: 0x%x, rSCLK_GATE: 0x%x \n", Inp32SYSC(0x30), Inp32SYSC(0x34), Inp32SYSC(0x38));
		
	while(!UART_GetKey());	

	UART_Printf("No.38 : PCM1 off (PCLK[9])\n");
	SYSC_CtrlPCLKGate(ePCLK_PCM1,0);
	UART_Printf("rHCLK_GATE: 0x%x, rPCLK_GATE: 0x%x, rSCLK_GATE: 0x%x \n", Inp32SYSC(0x30), Inp32SYSC(0x34), Inp32SYSC(0x38));

	while(!UART_GetKey());	

	UART_Printf("No.39 : PCM0 off (PCLK[8])\n");
	SYSC_CtrlPCLKGate(ePCLK_PCM0,0);
	UART_Printf("rHCLK_GATE: 0x%x, rPCLK_GATE: 0x%x, rSCLK_GATE: 0x%x \n", Inp32SYSC(0x30), Inp32SYSC(0x34), Inp32SYSC(0x38));

	while(!UART_GetKey());	

	UART_Printf("No.40 : PWM off (PCLK[7])\n");
	SYSC_CtrlPCLKGate(ePCLK_PWM,0);
	UART_Printf("rHCLK_GATE: 0x%x, rPCLK_GATE: 0x%x, rSCLK_GATE: 0x%x \n", Inp32SYSC(0x30), Inp32SYSC(0x34), Inp32SYSC(0x38));

	while(!UART_GetKey());	

	UART_Printf("No.41 : RTC off (PCLK[6])\n");
	SYSC_CtrlPCLKGate(ePCLK_RTC,0);
	UART_Printf("rHCLK_GATE: 0x%x, rPCLK_GATE: 0x%x, rSCLK_GATE: 0x%x \n", Inp32SYSC(0x30), Inp32SYSC(0x34), Inp32SYSC(0x38));

	while(!UART_GetKey());	

	UART_Printf("No.42 : WDT off (PCLK[5])\n");
	SYSC_CtrlPCLKGate(ePCLK_WDT,0);
	UART_Printf("rHCLK_GATE: 0x%x, rPCLK_GATE: 0x%x, rSCLK_GATE: 0x%x \n", Inp32SYSC(0x30), Inp32SYSC(0x34), Inp32SYSC(0x38));

	while(!UART_GetKey());	

	UART_Printf("No.43 : UART3 off (PCLK[4])\n");
	SYSC_CtrlPCLKGate(ePCLK_UART3,0);
	UART_Printf("rHCLK_GATE: 0x%x, rPCLK_GATE: 0x%x, rSCLK_GATE: 0x%x \n", Inp32SYSC(0x30), Inp32SYSC(0x34), Inp32SYSC(0x38));

	while(!UART_GetKey());	

	UART_Printf("No.44 : UART2 off (PCLK[3])\n");
	SYSC_CtrlPCLKGate(ePCLK_UART2,0);
	UART_Printf("rHCLK_GATE: 0x%x, rPCLK_GATE: 0x%x, rSCLK_GATE: 0x%x \n", Inp32SYSC(0x30), Inp32SYSC(0x34), Inp32SYSC(0x38));

	while(!UART_GetKey());	

	UART_Printf("No.45 : UART1 off (PCLK[2])\n");
	SYSC_CtrlPCLKGate(ePCLK_UART1,0);
	UART_Printf("rHCLK_GATE: 0x%x, rPCLK_GATE: 0x%x, rSCLK_GATE: 0x%x \n", Inp32SYSC(0x30), Inp32SYSC(0x34), Inp32SYSC(0x38));
						
			
	while(!UART_GetKey());	

	UART_Printf("All CLK Enable\n");
	Outp32SYSC(0x30, 0xffffffff);
	Outp32SYSC(0x34, 0xffffffff);
	Outp32SYSC(0x38, 0xffffffff);

    
			
}


//////////
// Function Name : IDLET_Aging
// Function Description : Idle Mode Test - Baisc Test ( 1 Int. Source Enable, Not adapted DVS Scheme)
// Input : 	None
// Output :	None 
// Version : v0.1

void IDLET_Aging (void)
{
	UART_Printf("[IDLE Mode Aging Test]\n");

	g_uIntCounter0 =0;   
	g_uIntCounter1 =0;   
	g_uIntCounter2 =0;  
	g_uIntCounter3 =0;  

	PWM_stopAll();	// stop timers

	// Timer Setting - 1ms, 0.9ms
	INTC_SetVectAddr(NUM_TIMER0,Isr_TIMER0_Idle);
	INTC_SetVectAddr(NUM_TIMER1,Isr_TIMER1_Idle);
	INTC_SetVectAddr(NUM_TIMER2,Isr_TIMER2_Idle);
	INTC_SetVectAddr(NUM_TIMER3,Isr_TIMER3_Idle);
	INTC_Enable(NUM_TIMER0);
	INTC_Enable(NUM_TIMER1);
	INTC_Enable(NUM_TIMER2);
	INTC_Enable(NUM_TIMER3);

	//PWM_Select(0,255,4,30000,15000,0,0,0,1,0,1);
	//PWM_Select(1,255,3,30000,15000,0,0,0,1,0,1);
	//PWM_Select(2,255,2,30000,15000,0,0,0,1,0,1);
	//PWM_Select(3,255,1,30000,15000,0,0,0,1,0,1);		
	PWM_Select(0,5,4,2500,15000,0,0,0,1,0,1);
	PWM_Select(1,5,4,2450,15000,0,0,0,1,0,1);
	PWM_Select(2,10,2,3000,15000,0,0,0,1,0,1);
	PWM_Select(3,5,1,2500,15000,0,0,0,1,0,1);	

	SYSC_SetCFGWFI(eIDLE,1);	

	while(!UART_GetKey())
	{
	DisplayLED(g_uIntCounter0);
	MMU_WaitForInterrupt();
	DisplayLED(0x0);
	}


	UART_Printf("Return to Normal Mode \n\n");

	// Int. Disable
	PWM_stopAll();	// stop timers
	INTC_Disable(NUM_TIMER3);
	INTC_Disable(NUM_TIMER2);
	INTC_Disable(NUM_TIMER1);
	INTC_Disable(NUM_TIMER0);

}


//////////
// Function Name : SUBBLKLkg_Test - ing
// Function Description : 
// Input : 	None
// Output :	None 
// Version : v0.1

void SUBBLKLkg_Test (void)
{
	
	u32 uTemp;
			
	//Block Power Stablilzation counter
	//[27:24] ETM, [23:20] DOMAIN_S, [19:16] DOMAIN_F, [15:12] DOMAIN_P
	//[11:8] DOMAIN_I, [7:4] DOMAIN_V, [3:0] DOMAIN_TOP
	uTemp = (0xF<<24)|(0xF<<20)|(0xF<<16)|(0xF<<12)|(0xF<<8)|(0xF<<4)|(0xF<<0);
	SYSC_MTCSTAB(uTemp);
	UART_Printf("Press any key  \n");
	while(!UART_GetKey());
	UART_Printf("DOMAIN_V[MFC] : Clock Off  \n");
   	SYSC_CtrlHCLKGate(eHCLK_MFC,0);
	SYSC_CtrlPCLKGate(ePCLK_MFC,0);
	SYSC_CtrlSCLKGate(eSCLK_MFC,0);
	UART_Printf("rHCLK_GATE: 0x%x, rPCLK_GATE: 0x%x, rSCLK_GATE: 0x%x \n", Inp32SYSC(0x30), Inp32SYSC(0x34), Inp32SYSC(0x38));
	while(!UART_GetKey());
		
	UART_Printf("DOMAIN_V Block Power Off- MFC  \n");
   	SYSC_CtrlHCLKGate(eHCLK_MFC,1);
	SYSC_CtrlPCLKGate(ePCLK_MFC,1);
	SYSC_CtrlSCLKGate(eSCLK_MFC,1);

	SYSC_CtrlNORCFG(eNOR_DOMAINV, 0);
	Delay(100);
	SYSC_RdBLKPWR();

	SYSC_CtrlHCLKGate(eHCLK_MFC,0);
	SYSC_CtrlPCLKGate(ePCLK_MFC,0);
	SYSC_CtrlSCLKGate(eSCLK_MFC,0);

	while(!UART_GetKey());
	
	UART_Printf("DOMAIN_I[CAMIF, JPEG] : Clock Off  \n");
	SYSC_CtrlHCLKGate(eHCLK_JPEG,0);
	SYSC_CtrlSCLKGate(eSCLK_JPEG,0);
	SYSC_CtrlHCLKGate(eHCLK_CAM,0);
	SYSC_CtrlSCLKGate(eSCLK_CAM,0);
	UART_Printf("rHCLK_GATE: 0x%x, rPCLK_GATE: 0x%x, rSCLK_GATE: 0x%x \n", Inp32SYSC(0x30), Inp32SYSC(0x34), Inp32SYSC(0x38));
	while(!UART_GetKey());
		
	UART_Printf("DOMAIN_I[CAMIF,JPEG] Block Power Off- MFC  \n");
   	SYSC_CtrlHCLKGate(eHCLK_JPEG,1);
	SYSC_CtrlSCLKGate(eSCLK_JPEG,1);
	SYSC_CtrlHCLKGate(eHCLK_CAM,1);
	SYSC_CtrlSCLKGate(eSCLK_CAM,1);
	
	SYSC_CtrlNORCFG(eNOR_DOMAINI, 0);
	Delay(100);
	SYSC_RdBLKPWR();
	
	SYSC_CtrlHCLKGate(eHCLK_JPEG,0);
	SYSC_CtrlSCLKGate(eSCLK_JPEG,0);
	SYSC_CtrlHCLKGate(eHCLK_CAM,0);
	SYSC_CtrlSCLKGate(eSCLK_CAM,0);

	while(!UART_GetKey());

	UART_Printf("DOMAIN_P[2D,TV,Scaler] : Clock Off  \n");
	SYSC_CtrlHCLKGate(eHCLK_SCALER,0);
	SYSC_CtrlSCLKGate(eSCLK_SCALER27,0);
	SYSC_CtrlSCLKGate(eSCLK_SCALER,0);
	SYSC_CtrlHCLKGate(eHCLK_2D,0);
	SYSC_CtrlHCLKGate(eHCLK_TV,0);
	SYSC_CtrlSCLKGate(eSCLK_DAC27,0);
	SYSC_CtrlSCLKGate(eSCLK_TV27,0);
	
	UART_Printf("rHCLK_GATE: 0x%x, rPCLK_GATE: 0x%x, rSCLK_GATE: 0x%x \n", Inp32SYSC(0x30), Inp32SYSC(0x34), Inp32SYSC(0x38));
	while(!UART_GetKey());
		
	UART_Printf("DOMAIN_P[2D,TV,Scaler] Block Power Off- \n");
	SYSC_CtrlHCLKGate(eHCLK_SCALER,1);
	SYSC_CtrlSCLKGate(eSCLK_SCALER27,1);
	SYSC_CtrlSCLKGate(eSCLK_SCALER,1);
	SYSC_CtrlHCLKGate(eHCLK_2D,1);
	SYSC_CtrlHCLKGate(eHCLK_TV,1);
	SYSC_CtrlSCLKGate(eSCLK_DAC27,1);
	SYSC_CtrlSCLKGate(eSCLK_TV27,1);
	
	SYSC_CtrlNORCFG(eNOR_DOMAINP, 0);
	Delay(100);
	SYSC_RdBLKPWR();
	
	SYSC_CtrlHCLKGate(eHCLK_SCALER,0);
	SYSC_CtrlSCLKGate(eSCLK_SCALER27,0);
	SYSC_CtrlSCLKGate(eSCLK_SCALER,0);
	SYSC_CtrlHCLKGate(eHCLK_2D,0);
	SYSC_CtrlHCLKGate(eHCLK_TV,0);
	SYSC_CtrlSCLKGate(eSCLK_DAC27,0);
	SYSC_CtrlSCLKGate(eSCLK_TV27,0);

	while(!UART_GetKey());

	UART_Printf("DOMAIN_f[ROT,POST,LCD] : Clock Off  \n");
	SYSC_CtrlHCLKGate(eHCLK_POST, 0);
	SYSC_CtrlSCLKGate(eSCLK_POST0_27,0);
	SYSC_CtrlSCLKGate(eSCLK_POST0,0);
	SYSC_CtrlHCLKGate(eHCLK_ROT,0);
	SYSC_CtrlHCLKGate(eHCLK_LCD,0);
	SYSC_CtrlSCLKGate(eSCLK_LCD_27,0);
	SYSC_CtrlSCLKGate(eSCLK_LCD,0);
	UART_Printf("rHCLK_GATE: 0x%x, rPCLK_GATE: 0x%x, rSCLK_GATE: 0x%x \n", Inp32SYSC(0x30), Inp32SYSC(0x34), Inp32SYSC(0x38));
	while(!UART_GetKey());
		
	UART_Printf("DOMAIN_F[ROT,POST,LCD] Block Power Off- \n");
	SYSC_CtrlHCLKGate(eHCLK_POST, 1);
	SYSC_CtrlSCLKGate(eSCLK_POST0_27,1);
	SYSC_CtrlSCLKGate(eSCLK_POST0,1);
	SYSC_CtrlHCLKGate(eHCLK_ROT,1);
	SYSC_CtrlHCLKGate(eHCLK_LCD,1);
	SYSC_CtrlSCLKGate(eSCLK_LCD_27,1);
	SYSC_CtrlSCLKGate(eSCLK_LCD,1);
	
	SYSC_CtrlNORCFG(eNOR_DOMAINF, 0);
	Delay(100);
	SYSC_RdBLKPWR();
	
	SYSC_CtrlHCLKGate(eHCLK_POST, 0);
	SYSC_CtrlSCLKGate(eSCLK_POST0_27,0);
	SYSC_CtrlSCLKGate(eSCLK_POST0,0);
	SYSC_CtrlHCLKGate(eHCLK_ROT,0);
	SYSC_CtrlHCLKGate(eHCLK_LCD,0);
	SYSC_CtrlSCLKGate(eSCLK_LCD_27,0);
	SYSC_CtrlSCLKGate(eSCLK_LCD,0);

	while(!UART_GetKey());

	UART_Printf("DOMAIN_S[SDMA0,SDMA1,Security]  : Clock Off  \n");
	SYSC_CtrlHCLKGate(eHCLK_SDMA1, 0);
	SYSC_CtrlHCLKGate(eHCLK_SDMA0, 0);
	SYSC_CtrlHCLKGate(eHCLK_SECUR, 0);
	SYSC_CtrlSCLKGate(eSCLK_SECUR, 0);
	UART_Printf("rHCLK_GATE: 0x%x, rPCLK_GATE: 0x%x, rSCLK_GATE: 0x%x \n", Inp32SYSC(0x30), Inp32SYSC(0x34), Inp32SYSC(0x38));
	while(!UART_GetKey());
		
	UART_Printf("DOMAIN_S[SDMA0,SDMA1,Security] Block Power Off- \n");
	SYSC_CtrlHCLKGate(eHCLK_SDMA1, 1);
	SYSC_CtrlHCLKGate(eHCLK_SDMA0, 1);
	SYSC_CtrlHCLKGate(eHCLK_SECUR, 1);
	SYSC_CtrlSCLKGate(eSCLK_SECUR, 1);
	
	SYSC_CtrlNORCFG(eNOR_DOMAINS, 0);
	Delay(100);
	SYSC_RdBLKPWR();
	
	SYSC_CtrlHCLKGate(eHCLK_SDMA1, 0);
	SYSC_CtrlHCLKGate(eHCLK_SDMA0, 0);
	SYSC_CtrlHCLKGate(eHCLK_SECUR, 0);
	SYSC_CtrlSCLKGate(eSCLK_SECUR, 0);


	while(!UART_GetKey());

			
	UART_Printf("DOMAIN_ETM Block Power Off- \n");
	
	SYSC_CtrlNORCFG(eNOR_DOMAINETM, 0);
	Delay(100);
	SYSC_RdBLKPWR();

	while(!UART_GetKey());

	UART_Printf("DOMAIN_IROM  : Clock Off  \n");
	SYSC_CtrlHCLKGate(eHCLK_IROM, 0);
	UART_Printf("rHCLK_GATE: 0x%x, rPCLK_GATE: 0x%x, rSCLK_GATE: 0x%x \n", Inp32SYSC(0x30), Inp32SYSC(0x34), Inp32SYSC(0x38));
	while(!UART_GetKey());
		
	UART_Printf("DOMAIN_IROM Block Power Off- \n");
	SYSC_CtrlHCLKGate(eHCLK_IROM, 1);
	
	SYSC_CtrlNORCFG(eNOR_IROM, 0);
	Delay(100);
	SYSC_RdBLKPWR();
	
	SYSC_CtrlHCLKGate(eHCLK_IROM, 0);


	while(!UART_GetKey());
	UART_Printf("Return  \n");

	
	Outp32SYSC(0x30, 0xffffffff);
	Outp32SYSC(0x34, 0xffffffff);
	Outp32SYSC(0x38, 0xffffffff);
	SYSC_BLKPwrONAll();
	UART_Printf("rHCLK_GATE: 0x%x, rPCLK_GATE: 0x%x, rSCLK_GATE: 0x%x \n", Inp32SYSC(0x30), Inp32SYSC(0x34), Inp32SYSC(0x38));
	SYSC_RdBLKPWR();
	
}


//////////
// Function Name : STOPT_CFG  -- ing..
// Function Description : Stop Mode Test - Baisc Test 
// Input : 	None
// Output :	None 
// Version : v0.1
void STOPT_CFG_EVT0 (void)
{
	u32 uTemp;
	u32 i;
	
	u32  uOSCEN ;
	
	u32 uRegValue_SRC, uRegValue_DIV0;
	u32 uTTT0, uTTT1;
	u32 uARM_Memory,uTop_Memory,uARM_Logic,uTop_Logic, uHidden;
	u32 uOSCTime;

	UART_Printf("\n[STOP CFG Test]\n");

	//test 2.24  -Operating Mode Change
/*
	SYSC_ChangeMode(eASYNC_MODE);
	SYSC_GetClkInform();
	UART_InitDebugCh(0, 115200);
	UART_Printf("\n\n");
	UART_Printf("ARMCLK: %.2fMHz  HCLKx2: %.2fMHz  HCLK: %.2fMHz  PCLK: %.2fMHz\n",(float)g_ARMCLK/1.0e6, (float)g_HCLKx2/1.0e6, (float)g_HCLK/1.0e6, (float)g_PCLK/1.0e6);
	UART_Printf("SYNC Mode : %d\n", g_SYNCACK);
	UART_Printf("\n");    
*/

	GPIO_SetFunctionEach(eGPIO_F, eGPIO_14, 1);
	GPIO_SetDataEach(eGPIO_F, eGPIO_14, 0);

	

	//for Test
	//SYSC_DisDBG();
	
	//SYSC_CtrlCLKOUT( eCLKOUT_HCLK, 9); // for clock test, jangwj
	
	//UART_Printf("ADC is in standby \n");
	//ADC_SelectStandbyMode(1);			// rADCCON |= (1<<2)	

	UART_Printf("LCD is turned off \n");
	//Lcd_EnvidOnOff(0);


	
	// User Key Input
	UART_Printf(" Choose Oscillator Enable or Disable :  0 : Disable,  1: Enable \n");
	uOSCEN = UART_GetIntNum();
	UART_Printf(" Choose uARM_Logic :  0 : OFF,  1: ON \n");
	uARM_Logic = UART_GetIntNum();
	UART_Printf(" Choose uARM_Memory :  0 : OFF,  1: ON \n");
	uARM_Memory = UART_GetIntNum();
	UART_Printf(" Choose uTop_Logic :  0 : RET  1: ON \n");
	uTop_Logic = UART_GetIntNum();
	UART_Printf(" Choose uTop_Memory :  0 : OFF,  1: RET\n");
	uTop_Memory = UART_GetIntNum();

	UART_Printf(" Choose Hidden Stop Mode:  0 : Sub Block OFF,  1: Sub Block Enable\n");
	uHidden = UART_GetIntNum();

	UART_Printf(" Oscillator settle-down time:  \n");
	uOSCTime = UART_GetIntNum();

	//UART_Printf("Oscillator settle-down time is set\n");
	//SYSC_OSCSTAB(1);				// Check...

	UART_Printf("Wake-up source is set!\n");

   	//SetAlarmWakeup();
	//SetEintWakeup();
	//SetKeypadWakeup();
	//SetAlarmTick();
	Outp32SYSC(0x804, 0x1<<7);
	SetBatteryFaultEvent(eFLT_INT);
	SetADCTSWakeup();
//	SetHSIWakeup();

	//Wake-up Source Mask...
	//Outp32SYSC(0x804, 1<<12);
	//while(1);

	//Test Case - Normal CFG OFF
#if 0	
	SYSC_CtrlNORCFG(eNOR_DOMAINV, 0);
	SYSC_CtrlNORCFG(eNOR_DOMAINI, 0);
	SYSC_CtrlNORCFG(eNOR_DOMAINP, 0);
	SYSC_CtrlNORCFG(eNOR_DOMAINF, 0);
	SYSC_CtrlNORCFG(eNOR_DOMAINS, 0);
	SYSC_CtrlNORCFG(eNOR_DOMAINETM, 0);
	//SYSC_CtrlNORCFG(eNOR_IROM, 0);
#endif
	Delay(10);
	SYSC_RdBLKPWR();
	UART_Printf("NOR_CFG = 0x%x\n", Inp32SYSC(0x90c));
	//SYSC_CtrlNORCFG(eNOR_DOMAINI, 1);
	//Delay(1000);

	UART_Printf("\nCPU will goes to Stop Mode!\n\n");
	//UART_TxEmpty(1);				// 

	//uTemp=Inp32SYSC(0x900);
	//UART_Printf("Others  Register : 0x%x\n", uTemp);

	 //Change ASYNC Mode
	

	//uTemp=Inp32SYSC(0x900);
	//UART_Printf("Others  Register : 0x%x\n", uTemp);


	//USB_OTG_POWER_OFF
	UART_Printf("PHY OFF\n");
	uTTT1 = Inp32SYSC(0x900);
	uTTT1 = uTTT1|(1<<14);		
	Outp32SYSC(0x900, uTTT1);	//  USB_PWR_DN_EN
	//Outp32(0x7C100000, 0xF);		//  USB_Power Down
	uTTT1 = Inp32(0x7C100004);
	uTTT1 = uTTT1 |(1<<3);
	Outp32(0x7C100004,uTTT1 );			//  
	
	Outp32(0x7C100000, 0xF);		//  USB_Power Down
	
	UART_Printf("rOTHERS(PWR_DN_EN): 0x%x\n",  Inp32SYSC(0x900));
	//uTTT1 = Inp32SYSC(0x900);
	//uTTT1 = uTTT1 & ~(1<<16);
	//Outp32SYSC(0x900, uTTT1);
	UART_Printf("rOTHERS(USB_SIG_MASK): 0x%x\n",  Inp32SYSC(0x900));
	UART_Printf("rOTHERS(USB_SIG_MASK): 0x%x\n",  Inp32SYSC(0x900));
	UART_Printf("rOPHYCLK: 0x%x\n",  Inp32(0x7C100004));

	//UART_Getc();
	

	//Outp32(0x7F008880, 0x1000);	// Reset Out0
	//UART_Getc();
	//Outp32(0x7C100000, 0xF);

	//UART_Getc();
	//For Test. (07.02.12- )
	SYSC_OSCSTAB(uOSCTime);			
	SYSC_PWRSTAB(0x1);			
	SYSC_FPCSTAB(0x1);			

/*
	Outp32(0x70000000,0x999999);

	for (i=0x18000000;i<=0x18001000;i+=4)
	{
	//	*((unsigned int *)i) = i;	
		Outp32(i, i);
	}
*/

	for (i=0;i<=1000;i+=4)
	{
	//	*((unsigned int *)i) = i;	
	//	Outp32(i, i);
	}




	
	//SYSC_SetSTOPCFG(1, uDeepStop, 1, uDeepStop);

	switch(uHidden)
	{
		case 0:
				SYSC_SetSTOPCFG(uARM_Memory, uTop_Memory, uARM_Logic, uTop_Logic);
				break;
		case 1:
				Outp32SYSC(0x814, 0xFFFFFFFF);
				//uTemp0 = Inp32SYSC(0x814);
				//uTemp2 = ((1<<16)|(1<<15)|(1<<14)|(1<<13)|(1<<12)|(1<<9));
				//uTemp0 =  (uTemp0 & ~(0x3FFFFF<<8)) | ((uARM_Memory<<29)|(uTop_Memory<<20)|(uARM_Logic<<17)|(uTop_Logic<<8)|uTemp2);	
				//Outp32SYSC(0x814, uTemp0);
				break;
	}
	
	//SYSC_SetSTOPCFG(uDeepStop,1, uDeepStop, 0);   // for test 2.12,  TOP Memory ON
	SYSC_SetCFGWFI(eSTOP, uOSCEN);

	//for test
	//Outp32SYSC(0x800, 0x2); 

	// Reset Out
	Outp32(0x7F008880, 0x1010);
	// Test Case => External Clock.
/*	
	uRegValue2_bk = Inp32SYSC(0x1C);
	uRegValue3_bk = Inp32SYSC(0x20);

	uTemp = (1<<12)|(1<<9)|(0<<8)|(1<<4)|(0<<0);		// ARM:HCLKx2:HCLK:PCLK = 1:1:2:2
					
	uTemp1 = Inp32SYSC(0x1C);			//Clock Source Register
	uTemp1 = uTemp1 & ~(0x7) | 0x0;

	Outp32SYSC(0x1C, uTemp1);			// Clock Source Change	
	Outp32SYSC(0x20, uTemp);			// Change Divide Value
	SYSC_StopPLL(eAPLL);
	SYSC_StopPLL(eMPLL);
*/	
//test 2.21 ( Clock 1:1:1)

	//UART_Getc();

	// Normal CFG 
	Outp32Inform(7, Inp32SYSC(0x90c));

	// W.A - PLL Lock Time Control					
	Outp32SYSC(0x0, 0x100);
	Outp32SYSC(0x4, 0x100);
	Outp32SYSC(0x8, 0x100);


	uRegValue_SRC = Inp32SYSC(0x1C);
	uRegValue_DIV0 = Inp32SYSC(0x20);

	uTTT0 = (uRegValue_SRC)&~(0x3);
	uTTT1 = ((uRegValue_DIV0)&~(0xFFFF))|((1<<12)|(0<<9)|(1<<8)|(1<<4)|(0<<0));		// ARM:HCLKx2:HCLK:PCLK = 1:1:2:2


	// Clock Source Change
	Outp32SYSC(0x1C, uTTT0);
	Outp32SYSC(0x20, uTTT1);


//	LTC3714_VoltageSet(3, 650);


	//test 2.18
	SYSTEM_DisableIRQ();
	SYSTEM_DisableFIQ();



	Stop_WFI_Test();
//	MMU_WaitForInterrupt();

	
	GPIO_SetDataEach(eGPIO_F, eGPIO_14, 1);
//	LTC3714_VoltageSet(3, 1200);

	Outp32SYSC(0x20, uRegValue_DIV0);
	Outp32SYSC(0x1C,uRegValue_SRC);

	

	Delay(10);
	
	SYSTEM_EnableIRQ();
	SYSTEM_EnableFIQ();
	GPIO_SetDataEach(eGPIO_F, eGPIO_14, 0);


	DisplayLED(0x3);

	


	
	UART_Printf("Return to Normal Mode.\n");

	UART_Printf("rWAKEUP_STAT=0x%x\n", Inp32SYSC(0x908));
	uTemp=Inp32SYSC(0x908);
	Outp32SYSC(0x908,uTemp);
	UART_Printf("rWAKEUP_STAT=0x%x\n\n Wake-up Status Clear", Inp32SYSC(0x908));
	


	// Int. Disable
	INTC_Disable(NUM_EINT1);
	INTC_Disable(NUM_RTC_ALARM);
	INTC_Disable(NUM_RTC_TIC);
	
	

}


//////////
// Function Name : STOPT_Aging  
// Function Description : Stop Mode Test - Baisc Test 
// Input : 	None
// Output :	None 
// Version : v0.1
void STOPT_Aging (void)
{
	u32 uTemp;	
	u32 uTestCount, uRet;


	UART_Printf("\n[STOP CFG Aging Test - Pre Version]\n");

	
	UART_Printf("Oscillator settle-down time is set\n");
	SYSC_OSCSTAB(1);				// Check...

	UART_Printf("Wake-up source is set!\n");
	//SetAlarmWakeup();
	SetEintWakeup();
	//SetKeypadWakeup();
	//SetAlarmTick();
	//Outp32SYSC(0x804, 0x1<<7);
	//SetBatteryFaultEvent(eFLT_INT);
	SetADCTSWakeup();

	Delay(10);
	
	UART_Printf("\nCPU will goes to Stop Mode!\n\n");
      

	while(1)
	{

		DisplayLED(uTestCount);
		if(UART_GetKey()!=0x0) 
			break;
		if(uTestCount>100000) 
			break;
		if ((uTestCount%100)==0)
		{
			DisplayLED(uTestCount/100);
			SYSC_GetClkInform();		
			UART_InitDebugCh(0, 115200);
			Delay(100);
			UART_Printf("\n%d \n", uTestCount);
 			 UART_TxEmpty();
		}

	InitData_SDRAM(_DRAM_BaseAddress+0x1000000, 0x10000);
	Copy(0x0C000000, 0x52000000, 0x100);
	SetAlarmWakeup();

	Delay(100);

	CheckHCLK_ForPM();
       CheckBLKPWR_ForPM();
	CheckWAKESTAT_ForPM(0);
	//SYSC_SetSTOPCFG(1, uDeepStop, 1, uDeepStop);
	SYSC_SetSTOPCFG(1, 1, 1, 1);
	//SYSC_SetSTOPCFG(uDeepStop,1, uDeepStop, 0);   // for test 2.12,  TOP Memory ON
	SYSC_SetCFGWFI(eSTOP, 0);
	MMU_WaitForInterrupt();
	
	//UART_Printf("Return to Normal Mode.\n");

	//UART_Printf("rWAKEUP_STAT=0x%x\n", Inp32SYSC(0x908));
	CheckData_SDRAM(_DRAM_BaseAddress+0x1000000, 0x10000);
	uRet = Compare(0x0C000000, 0x52000000, 0x100);
	if (uRet)
			{
			UART_Printf("Stepping Stone Data Retention Success..\n");
			}
			else
			{
			UART_Printf("Stepping Stone Data Retention  Failed..\n");
			}

	
	uTemp=Inp32SYSC(0x908);
	Outp32SYSC(0x908,uTemp);
	//UART_Printf("rWAKEUP_STAT=0x%x\n\n Wake-up Status Clear", Inp32SYSC(0x908));

	uTestCount++;
	
	}

	// Int. Disable
	INTC_Disable(NUM_EINT1);
	INTC_Disable(NUM_RTC_ALARM);
	INTC_Disable(NUM_RTC_TIC);
	
	

}





//////////
// Function Name : STOPT_CFG  -- ing..
// Function Description : Stop Mode Test - Baisc Test 
// Input : 	None
// Output :	None 
// Version : v0.1
void DSTOPT_Aging (void)
{

	u32 uTemp;
	u32 uOSCTime;
	u32 uRet, uDSTOP;
	u32 uTestCount;

	uDSTOP= DeepStopStatus();
	SYSC_RdRSTSTAT(0);

	if( uDSTOP && !(g_OnTest_DSTOP) )									// must be changed
	{
		
		CheckData_SDRAM(_DRAM_BaseAddress+0x1000000, 0x10000);

		//Check & Clear Wake-up Source 
		//SYSC_ClrWKUPSTAT();
		uTemp=Inp32SYSC(0x908);
		Outp32SYSC(0x908,uTemp);
		//	UART_Printf("\nCheck Stepping Stone...\n\n");
	uRet = Compare(0x0C000000, 0x52000000, 0x100);
	if (uRet)
			{
			//UART_Printf("Stepping Stone Data Retention Success..\n");
			}
			else
			{
			UART_Printf("Stepping Stone Data Retention  Failed..\n");
			}

		DisplayLED(uTestCount);
		SYSC_GetClkInform();		
		UART_InitDebugCh(0, 115200);
		Delay(100);
		UART_Printf("%d : Deep Stop Wake-up Done\n", uTestCount);
		UART_TxEmpty();
		uTestCount++;
		
		//Read RawInterrupt Status
		//UART_Printf("VIC0RawStatus: 0x%x \n", Inp32(0x71200008));
		//UART_Printf("VIC1RawStatus: 0x%x \n", Inp32(0x71300008));

		//UART_Printf("Deep Stop mode test is done\n");
		g_OnTest_DSTOP = 1;
	}
else			// Entering into Deep Stop Mode
	{
		g_OnTest_DSTOP=0;

	//UART_Printf("\n[STOP CFG Test]\n");

//test 2.24  -Operating Mode Change
/*
	SYSC_ChangeMode(eASYNC_MODE);
	SYSC_GetClkInform();
	UART_InitDebugCh(0, 115200);
	UART_Printf("\n\n");
	UART_Printf("ARMCLK: %.2fMHz  HCLKx2: %.2fMHz  HCLK: %.2fMHz  PCLK: %.2fMHz\n",(float)g_ARMCLK/1.0e6, (float)g_HCLKx2/1.0e6, (float)g_HCLK/1.0e6, (float)g_PCLK/1.0e6);
	UART_Printf("SYNC Mode : %d\n", g_SYNCACK);
	UART_Printf("\n");    
*/

	//GPIO_SetFunctionEach(eGPIO_F, eGPIO_14, 1);
	//GPIO_SetDataEach(eGPIO_F, eGPIO_14, 0);

	

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

	Outp32Inform(5, 0x0);
	//UART_Printf("Wake-up source is set!\n");
	////////////////////////////////
	// Wake-up Source Setting
	//      Wake-up Source Interrupts are must enabled at Stop Mode
	////////////////////////////////
   	SetAlarmWakeup();
	SetEintWakeup();
	//SetKeypadWakeup();
	//SetAlarmTick();
	//Outp32SYSC(0x804, 0x1<<7);
	//SetBatteryFaultEvent(eFLT_INT);
	//SetADCTSWakeup();
	//SetHSIWakeup();

	// For Test - Retention Stepping Stone
	//UART_Printf("Test pattern for SDRAM Self-Refresh is filled!\n");
	InitData_SDRAM(_DRAM_BaseAddress+0x1000000, 0x10000);
	//UART_Printf("\nCopy Stepping Stone...\n\n");
	Copy(0x0C000000, 0x52000000, 0x100);


	//UART_Printf("Oscillator settle-down time is set\n");
	//SYSC_OSCSTAB(1);				// Check...
	
#if 1
	//Test Case - Normal CFG OFF
	SYSC_CtrlNORCFG(eNOR_DOMAINV, 0);
	SYSC_CtrlNORCFG(eNOR_DOMAINI, 0);
	SYSC_CtrlNORCFG(eNOR_DOMAINP, 0);
	SYSC_CtrlNORCFG(eNOR_DOMAINF, 0);
	SYSC_CtrlNORCFG(eNOR_DOMAINS, 0);
	SYSC_CtrlNORCFG(eNOR_DOMAINETM, 1);
	SYSC_CtrlNORCFG(eNOR_IROM, 1);
#endif
	Delay(100);
//	SYSC_RdBLKPWR();


	//For Test. (07.02.12- )
	SYSC_OSCSTAB(uOSCTime);			
	SYSC_PWRSTAB(0x1);			
	SYSC_FPCSTAB(0x1);			


	SYSC_SetSTOPCFG(0, 0, 0, 0);
	SYSC_SetCFGWFI(eSTOP, 0);
	
	CheckHCLK_ForPM();
      // CheckBLKPWR_ForPM();
	CheckWAKESTAT_ForPM(0);
      
	// Normal CFG 
	//UART_Printf("\nCPU will goes to Stop Mode!\n\n");
  	
      MMU_WaitForInterrupt();


	Delay(10);


	DisplayLED(0x3);
	//-daedoo
	//RTC_SetCON(0,0,0,0,0,0);	
	RTC_Enable(false);
	


	
	UART_Printf("Return to Normal Mode.\n");

	UART_Printf("rWAKEUP_STAT=0x%x\n", Inp32SYSC(0x908));
	uTemp=Inp32SYSC(0x908);
	Outp32SYSC(0x908,uTemp);
	UART_Printf("rWAKEUP_STAT=0x%x\n\n Wake-up Status Clear", Inp32SYSC(0x908));
	
	SYSC_RdBLKPWR();

	// Int. Disable
	INTC_Disable(NUM_EINT1);
	INTC_Disable(NUM_RTC_ALARM);
	INTC_Disable(NUM_RTC_TIC);
	
}

	INTC_Disable(NUM_EINT1);
	INTC_Disable(NUM_RTC_ALARM);
	INTC_Disable(NUM_BATF);
	INTC_Disable(NUM_PENDNUP);

	
}



void __irq Isr_RTC_Alm1(void)
{
	u32 uBCDDATE, uBCDHOUR, uBCDMIN, uBCDSEC;
	//-daedoo
	//RTC_ClearIntP(1);
	RTC_ClearPending(ALARM);

	uBCDDATE = Inp32(0x7E00507C);
	uBCDHOUR=Inp32(0x7E005078);
	uBCDMIN=Inp32(0x7E005074);
	uBCDSEC=Inp32(0x7E005070);
	g_AlmInt=1;
	UART_Printf("Alarm INT Time:  %x, %x, %x, %x \n", uBCDDATE ,uBCDHOUR,uBCDMIN,uBCDSEC);

	INTC_ClearVectAddr();
}



//////////
// Function Name : SetAlarmWakeup1
// Function Description : Setting RTC Alarm Int. - after 10sec.
// Input : 	None
// Output :	None 
// Version : v0.1
void SetAlarmWakeup1 (u32 uAlmTime)
{
	u32 uBCDDATE, uBCDHOUR, uBCDMIN, uBCDSEC;
	
	INTC_SetVectAddr(NUM_RTC_ALARM,Isr_RTC_Alm1);
	
	//-daedoo
	//RTC_SetCON(0,0,0,0,0,1);
	RTC_Enable(true);
	
	//Read_BCD Data
	uBCDHOUR=Inp32(0x7E005078);
	uBCDMIN=Inp32(0x7E005074);
	uBCDSEC=Inp32(0x7E005070);

	// Read
	uBCDHOUR = ((((uBCDHOUR>>4)&0x3)*10 )+ (uBCDHOUR&0xF));
	uBCDMIN = ((((uBCDMIN>>4)&0x7)*10) + (uBCDMIN&0xF));
	uBCDSEC = ((((uBCDSEC>>4)&0x7)*10) + (uBCDSEC&0xF) );
		
	//-daedoo
	//RTC_SetTime(07,01,01,02,uBCDHOUR,uBCDMIN,uBCDSEC);
	RTC_TimeInit(07,01,01,02,uBCDHOUR,uBCDMIN,uBCDSEC);

	if((uBCDHOUR == 23) && (uBCDMIN == 59)&&(uBCDSEC>55))
	{
		uBCDDATE = 2;
		uBCDHOUR = 0;
		uBCDMIN= 0;
	}
	else
	{
		uBCDDATE = 1;
		if((uBCDMIN == 59)&&(uBCDSEC>55))
		{
			uBCDHOUR = uBCDHOUR + 1;
			uBCDMIN = 0;
		}
		else
		{
			if(uBCDSEC>55)
				{
					uBCDMIN = uBCDMIN +1 ;
					uBCDSEC = 0x1;
				}
			
		}
		
	}

	//-daedoo
	/*
	RTC_SetAlmTime(07,01,uBCDDATE, uBCDHOUR,uBCDMIN,uBCDSEC+uAlmTime);
	RTC_SetAlmEn(1,0,0,0,0,0,1);
	RTC_SetCON(0,0,0,0,0,0);
	*/
	RTC_AlarmTimeInit(07,01,uBCDDATE, uBCDHOUR,uBCDMIN,uBCDSEC+uAlmTime);
	RTC_AlarmEnable(true, false, false, false, false, false, true);
	RTC_Enable(false);



	//UART_Printf("	RTC Alarm is set for wake-up source\n");
	//UART_Printf("Setting Time         : %d, %d, %d, %d \n", uBCDDATE ,uBCDHOUR,uBCDMIN,uBCDSEC);
	UART_Printf("Real Setting Time:  %d, %x, %x, %x \n", uBCDDATE ,Inp32(0x7E005078),Inp32(0x7E005074),Inp32(0x7E005070));
	//UART_Printf("ALR Setting Time: %x, %x, %x, %x \n\n", Inp32(0x7E005060) ,Inp32(0x7E00505C),Inp32(0x7E005058),Inp32(0x7E005054));
	
	g_AlmInt = 0;
	INTC_Enable(NUM_RTC_ALARM);
	
}




// Temp... 
void	PowerMode_Aging(void)
{
	u32	uRandom0, uRandom1;
	u32 uRegValue_SRC,uRegValue_DIV0, uTTT0, uTTT1;
	u32 uTemp, uRstId;


	SYSC_CtrlNORCFG(eNOR_DOMAINV, 0);	// Test Case, MFC Block Power Off

	uRstId = SYSC_RdRSTSTAT(1);
	
	if( ( uRstId == 3 ) && !(g_OnTest) )
	{
		//UART_Printf("Wake-up form SLEEP Mode \n");
		CheckData_SDRAM(_DRAM_BaseAddress+0x1000000, 0x10000);

		//Check & Clear Wake-up Source 
		SYSC_ClrWKUPSTAT();
			
		UART_Printf("SLEEP mode test is done\n");
		g_OnTest = 1;
	}
	else			// Entering into SLEEP Mode
	{
	
	g_OnTest=0;
	g_TestCount++;
	DisplayLED(g_TestCount);

	uRandom0 = rand()%8;
	Insert_Seed();
	uRandom1 = g_iSeed%4;
	
	//UART_Printf("g_iSeed = %d\n", g_iSeed);
	UART_Printf("uRandom1(PM) = %d\n", uRandom1);   // Select Power Mode
	UART_Printf("uRandom0 = %d\n", uRandom0);       // Wake-up Time Select


	InitData_SDRAM(_DRAM_BaseAddress+0x1000000, 0x10000);

	//Set Wake-up Source
	SetAlarmWakeup1(uRandom0+2);
	SetEintWakeup();

	SYSC_OTGSIGDisMSK();		// PHY Signal MASK
	Outp32(0x7C100000, 0xF);		// PHY OFF

	//For Test. (07.02.12- )
	SYSC_OSCSTAB(0x1);			
	SYSC_PWRSTAB(0x1);			
	SYSC_FPCSTAB(0x1);			

	SYSC_CtrlNORCFG(eNOR_DOMAINV, 1);
	Delay(100);


	//PowerMode Random Select - 0: Idle, 1:Stop, 2:DeepStop,3:Sleep
	switch(uRandom1)
	{
		case 0: 
			SYSC_SetCFGWFI(eIDLE,1);
			UART_Printf("Idle Mode\n");
			break;
		case 1: 
			UART_Printf("Stop Mode\n");
			SYSC_SetSTOPCFG(1, 1, 1, 1);
			SYSC_SetCFGWFI(eSTOP, 0);
			break;
		case 2: 
			UART_Printf("D-Stop Mode\n");
			SYSC_SetSTOPCFG(0, 0, 0,0);		// NOR Case
			SYSC_SetCFGWFI(eSTOP, 0);
			break;
		case 3: 
			UART_Printf("Sleep Mode\n");
			SYSC_SetCFGWFI(eSLEEP, 0);
			break;
								
	}

	// Enter Power Mode (W/A)	
	Outp32Inform(7, Inp32SYSC(0x90c));		// Save Sub Block Power Status

	uRegValue_SRC = Inp32SYSC(0x1C);			// Save Clock Source Cntrol register
	uRegValue_DIV0 = Inp32SYSC(0x20);		// Save Clock Divider Register
	

	uTTT0 = (uRegValue_SRC)&~(0x3);			// Use External Clock
	uTTT1 = ((uRegValue_DIV0)&~(0xFFFF))|((1<<12)|(0<<9)|(1<<8)|(1<<4)|(0<<0));		// ARM:HCLKx2:HCLK:PCLK = 1:1:2:2

	// Clock Source Change
	Outp32SYSC(0x1C, uTTT0);
	Outp32SYSC(0x20, uTTT1);

	// W.A - PLL Lock Time Control					
	Outp32SYSC(0x0, 0x100);
	Outp32SYSC(0x4, 0x100);
	Outp32SYSC(0x8, 0x100);

	//test 2.18
	SYSTEM_DisableIRQ();
	SYSTEM_DisableFIQ();

	// Enter Power Mode
	Stop_WFI_Test();
//	MMU_WaitForInterrupt();

	Outp32SYSC(0x20, uRegValue_DIV0);		// Return Clock.
	Outp32SYSC(0x1C,uRegValue_SRC);

	Delay(1000);
	
	SYSTEM_EnableIRQ();
	SYSTEM_EnableFIQ();




	//Wake-up From Idle/Stop
	Delay(100);

	DisplayLED(0x3);
	
	//UART_Printf("Return to Normal Mode.\n");

	//UART_Printf("rWAKEUP_STAT=0x%x\n", Inp32SYSC(0x908));
	uTemp=Inp32SYSC(0x908);
	Outp32SYSC(0x908,uTemp);
	//UART_Printf("rWAKEUP_STAT=0x%x\n\n Wake-up Status Clear", Inp32SYSC(0x908));
	}


	// Int. Disable
	INTC_Disable(NUM_EINT1);
	INTC_Disable(NUM_RTC_ALARM);
	
	

}



void Measure_MTCSTABLE (void)
{
	u32 uCnt, uTCNTB0, uTCNTO0;
		
	
		
		UART_Printf("[MTC Stable Test... ]\n");
		
		UART_Printf("System stablilization counter register!\n");

		GPIO_SetFunctionEach(eGPIO_F, eGPIO_14, 1);
		GPIO_SetDataEach(eGPIO_F, eGPIO_14, 1);
		
		SYSC_OSCFREQ(0xf);			
		SYSC_MTCSTAB(0xFFFFFFFF);

		SYSC_CtrlNORCFG(eNOR_DOMAINV, 0);

		UART_Getc();

		//StartTimer('u');
		uCnt = (u32)(0.000001*g_PCLK);
		GPIO_SetDataEach(eGPIO_F, eGPIO_14, 0);
		//PWM_Select(0,0,0,20000,0,0,0,0,0,0,1);	//timer0,prescaler=1,divider=1/1,autoreload,enInt
		
	
			
		SYSC_CtrlNORCFG(eNOR_DOMAINV, 1);

		while( (Inp32SYSC(0x90C)&0x2)>>1);
		//StopTimer();
	 	//PWM_stop();
		GPIO_SetDataEach(eGPIO_F, eGPIO_14, 1);
	   	//UART_Printf("Time: %d us \n", uTime);
		uTCNTB0 = Inp32(0x7F00600C);
	   	uTCNTO0 = Inp32(0x7F006014);
	   	
		UART_Printf("rTCNTB0: %d , rTCNTO0: %d \n", uTCNTB0, uTCNTO0);
 		UART_Printf("\nTime : %.2f us \n",(float)(uTCNTB0-uTCNTO0)/(float)uCnt);
		
	}

//////////
// Function Name : IDLET_Baisc
// Function Description : Idle Mode Test - Baisc Test ( 1 Int. Source Enable, Not adapted DVS Scheme)
// Input : 	None
// Output :	None 
// Version : v0.1

void IDLE_Current_Measure (void)
{
	UART_Printf("[IDLE Mode Current Measuret]\n");
	
	SetBatteryFaultEvent(eFLT_INT);
	

	// Wake-up Source Setting
	UART_Printf("Wake-up sourceis set! \n");
	//SetAlarmWakeup();
	SetEintWakeup();
	SetADCTSWakeup();

	UART_Printf("Sub Block OFF \n");
	Outp32SYSC(0x810, 0xBFFF4D00);

	UART_Printf("Clock Gating \n");
	//Outp32SYSC(0x30, 0xE550204A);  // MEM0
	//Outp32SYSC(0x30, 0xe570204A);
	Outp32SYSC(0x30, 0xC570204A);
	//Outp32SYSC(0x34,0xFE0450E2);
	//Outp32SYSC(0x38,0xC000C131);
	Outp32SYSC(0x34,0xFE0450C2);
	Outp32SYSC(0x38,0xC0004021);

	UART_Getc();

	UART_Printf("Software Reset \n");
	SYSC_SWRST();
	
	Outp32Inform(7, Inp32SYSC(0x90c));
	SYSC_SetCFGWFI(eIDLE,1);		// IDLE,  Clock Enable (O.K)
	//MMU_WaitForInterrupt();
	Stop_WFI_Test();
	



	//while(g_DmaDone==0);		
	UART_Printf("Return to Normal Mode \n\n");

	// Int. Disable
	INTC_Disable(NUM_EINT1);
	INTC_Disable(NUM_RTC_ALARM);
	INTC_Disable(NUM_BATF);

}

void DStop_Current_Measure (void)
{
	u32 uTemp;
	u32 i;

	u32 uRegValue_SRC, uRegValue_DIV0;
	u32 uTTT0, uTTT1;


	UART_Printf("\n[STOP Current Test]\n");


	GPIO_SetFunctionEach(eGPIO_F, eGPIO_14, 1);
	GPIO_SetDataEach(eGPIO_F, eGPIO_14, 0);

	UART_Printf("Wake-up source is set!\n");
	//SetAlarmWakeup();
	//SetEintWakeup();
	//SetKeypadWakeup();
	//SetAlarmTick();
	Outp32SYSC(0x804, 0x1<<7);
	SetBatteryFaultEvent(eFLT_INT);
	SetADCTSWakeup();


	//USB_OTG_POWER_OFF
	UART_Printf("PHY OFF\n");
	uTTT1 = Inp32SYSC(0x900);
	uTTT1 = uTTT1|(1<<14);		
	Outp32SYSC(0x900, uTTT1);	//  USB_PWR_DN_EN
	//Outp32(0x7C100000, 0xF);		//  USB_Power Down
	//uTTT1 = Inp32(0x7C100004);
	//uTTT1 = uTTT1 |(1<<3);
	//Outp32(0x7C100004,uTTT1 );			//  
	
	Outp32(0x7C100000, 0xF);		//  USB_Power Down
	
	//For Test. (07.02.12- )
	SYSC_OSCSTAB(1);			
	SYSC_PWRSTAB(0x1);			
	SYSC_FPCSTAB(0x1);			

/*
	Outp32(0x70000000,0x999999);

	for (i=0x18000000;i<=0x18001000;i+=4)
	{
	//	*((unsigned int *)i) = i;	
		Outp32(i, i);
	}
*/

	for (i=0;i<=1000;i+=4)
	{
	//	*((unsigned int *)i) = i;	
	//	Outp32(i, i);
	}

	
	SYSC_SetSTOPCFG(0,0,0,0);
	
	
	//SYSC_SetSTOPCFG(uDeepStop,1, uDeepStop, 0);   // for test 2.12,  TOP Memory ON
	SYSC_SetCFGWFI(eSTOP, 0);


	// Normal CFG 
	Outp32Inform(7, Inp32SYSC(0x90c));

	// W.A - PLL Lock Time Control					
	Outp32SYSC(0x0, 0x100);
	Outp32SYSC(0x4, 0x100);
	Outp32SYSC(0x8, 0x100);


	uRegValue_SRC = Inp32SYSC(0x1C);
	uRegValue_DIV0 = Inp32SYSC(0x20);

	uTTT0 = (uRegValue_SRC)&~(0x3);
	uTTT1 = ((uRegValue_DIV0)&~(0xFFFF))|((1<<12)|(0<<9)|(1<<8)|(1<<4)|(0<<0));		// ARM:HCLKx2:HCLK:PCLK = 1:1:2:2


	// Clock Source Change
	Outp32SYSC(0x1C, uTTT0);
	Outp32SYSC(0x20, uTTT1);


	//LTC3714_VoltageSet(3, 650);


	//test 2.18
	SYSTEM_DisableIRQ();
	SYSTEM_DisableFIQ();



	Stop_WFI_Test();
//	MMU_WaitForInterrupt();

	
	GPIO_SetDataEach(eGPIO_F, eGPIO_14, 1);
	//LTC3714_VoltageSet(3, 1200);

	Outp32SYSC(0x20, uRegValue_DIV0);
	Outp32SYSC(0x1C,uRegValue_SRC);

	

	Delay(1000);
	
	SYSTEM_EnableIRQ();
	SYSTEM_EnableFIQ();
	GPIO_SetDataEach(eGPIO_F, eGPIO_14, 0);


	DisplayLED(0x3);
	
	UART_Printf("Return to Normal Mode.\n");

	UART_Printf("rWAKEUP_STAT=0x%x\n", Inp32SYSC(0x908));
	uTemp=Inp32SYSC(0x908);
	Outp32SYSC(0x908,uTemp);
	UART_Printf("rWAKEUP_STAT=0x%x\n\n Wake-up Status Clear", Inp32SYSC(0x908));
	


	// Int. Disable
	INTC_Disable(NUM_EINT1);
	INTC_Disable(NUM_RTC_ALARM);
	INTC_Disable(NUM_RTC_TIC);
	
	

}


void SLEEP_Current_Measure (void)
{
	//u32 uPortStatus[32];		// must be change
	u32 uTTT1;
		
	UART_Printf("[SLEEP Mode Test]\n");
	UART_Printf("S3C6410 will wake-up by EINT9~11 or Other wake-up sources\n");
				
		
		//Lcd_EnvidOnOff(0);				//Before entering SLEEP mode, LCD must be off.
		ADC_SelectStandbyMode(1);		//ADC Stand-by
		//-daedoo
		//RTC_SetCON(0,0,0,0,0,0);			//RTC SFR R/W Disable.
		RTC_Enable(false);

		// Save the port configurations
		UART_Printf(" I/O Port is stored! \n");
	
		/*
		for (i=0;i<32;i++)
		{
			uPortStatus[i] = *( (volatile u32 *)0x7F008000 + i );
		}
		*/

			
		UART_Printf("Wake-up source is set!\n");
		//SetAlarmWakeup();
		//SetEintWakeup();
		//SetKeypadWakeup();
		//SetAlarmTick();
		SetBatteryFaultEvent(eFLT_INT);
		//SetADCTSWakeup();

		//Outp32(0x7E00B000, 0x3FC4);  //ADC Clock Disable
		//Outp32(0x7E00B004, 0x58);
		//Outp32(0x7E00B008, 5	);  //ADC Clock Disable


		UART_Printf(" I/O Port is set for Sleep Mode \n");
		ConfigSleepGPIO();
		

		UART_Printf("System stablilization counter register!\n");
		SYSC_OSCSTAB(0x8);			
		SYSC_PWRSTAB(0x8);			
		SYSC_FPCSTAB(0x8);	

		UART_Printf("Sub Block OFF \n");
		//Outp32SYSC(0x810, 0xBFFF4D00);
		
		//USB_OTG_POWER_OFF
		UART_Printf("PHY OFF\n");
		uTTT1 = Inp32SYSC(0x900);
		uTTT1 = uTTT1|(1<<14);	
		//uTTT1 = uTTT1&~(1<<16)|(1<<14);	
		Outp32SYSC(0x900, uTTT1);	//  USB_PWR_DN_EN
		Outp32(0x7C100000, 0xF);		//  USB_Power Down
		uTTT1 = Inp32(0x7C100004);
		uTTT1 = uTTT1 |(1<<3);
		Outp32(0x7C100004,uTTT1 );	

		UART_Printf("Clock Gating \n");
		//Outp32SYSC(0x30, 0xE550204A);  // MEM0
		//Outp32SYSC(0x30, 0xe570204A);
		//Outp32SYSC(0x30, 0xC7f0204A);
		Outp32SYSC(0x30, 0x01600007);		//MFC??
		//Outp32SYSC(0x34,0xFE0450E2);
		//Outp32SYSC(0x38,0xC000C131);
		Outp32SYSC(0x34,0x00040002);
		Outp32SYSC(0x38,0x00000001);
		
		

		Outp32Inform(7, Inp32SYSC(0x90c));
		//UART_Getc();

		SYSC_SetCFGWFI(eSLEEP, 0);
		
		MMU_WaitForInterrupt();
		//Stop_WFI_Test();

		UART_Printf("CPU doesn't go to Sleep Mode\n");

	
	

}





#if 0
//////////
// Function Name : DEEPSTOP_Basic  -- ing..
// Function Description : Deep Stop Mode Test - Baisc Test 
// Input : 	None
// Output :	None 
// Version : v0.1
void DEEPSTOP_Basic1 (void)
{
	u32 uRstId, i, uTemp;
	u32 uPortStatus[32];		// must be change
	u32 uInform0, uInform7;
	u32 uTTT, uTTT1;
	u32 uOSCEN, uTopMEM, uTopLogic;
	u32 uRegValue_SRC, uRegValue_DIV0,uTTT0;
		
	UART_Printf("rINFORM0: 0x%x\n", Inp32Inform(0));
	UART_Printf("rINFORM7: 0x%x\n", Inp32Inform(7));

	uInform0 = 0xABCD6400;
	


	SYSC_CtrlCLKOUT( eCLKOUT_HCLK, 4); // for clock test, jangwj
	
	uRstId = SYSC_RdRSTSTAT();

	if(  !(g_OnTest_DSTOP) )									// must be changed
	{
		UART_Printf("Wake-up form Deep Stop Mode \n");
		CheckData_SDRAM(_DRAM_BaseAddress+0x1000000, 0x10000);

		//Check Information Register Value
		if( (uInform0 !=Inp32Inform(0) ))
		{
			UART_Printf(" Information Register Value is wrong!!! \n");
		}
		else 
		{
			UART_Printf(" Information Register Value is correct!!! \n");
		}

/*
		//test - Clear PWR_CFG
		uTTT=Inp32SYSC(0x804);
		uTTT= uTTT&~(0x3<<5);
		Outp32SYSC(0x804, uTTT);
*/		
		//Check & Clear Wake-up Source 
		SYSC_ClrWKUPSTAT();

		UART_Printf("Deep Stop mode test is done\n");
		g_OnTest_DSTOP = 1;
	}
	else			// Entering into Deep Stop Mode
	{
		g_OnTest_DSTOP=0;
		
		UART_Printf("[Deep Stop Mode Test]\n");
		UART_Printf("S3C6410 will wake-up by EINT9~11 or Other wake-up sources\n");


		// User Key Input
		UART_Printf(" Choose Oscillator Enable or Disable :  0 : Disable,  1: Enable \n");
		uOSCEN = UART_GetIntNum();
		UART_Printf(" Choose Top Memory :  0 : OFF,  1: ON \n");
		uTopMEM = UART_GetIntNum();
		UART_Printf(" Choose Top Logic :  0 : OFF,  1: ON \n");
		uTopLogic = UART_GetIntNum();
		
		//Lcd_EnvidOnOff(0);				//Before entering SLEEP mode, LCD must be off.
		//ADC_SelectStandbyMode(1);		//ADC Stand-by
		RTC_SetCON(0,0,0,0,0,0);			//RTC SFR R/W Disable.

		// Save the port configurations
		UART_Printf(" I/O Port is stored! \n");
	
		for (i=0;i<32;i++)
		{
			uPortStatus[i] = *( (volatile u32 *)0x7F008000 + i );
		}

		UART_Printf(" I/O Port is set for Stop Mode \n");


		UART_Printf("Test pattern for SDRAM Self-Refresh is filled!\n");
		InitData_SDRAM(_DRAM_BaseAddress+0x1000000, 0x10000);
		// Fill Information Register - for test
		uInform0 = 0xABCD6400;
		
		Outp32Inform(0,uInform0);
		

		UART_Printf("Wake-up source is set!\n");
		//SetAlarmWakeup();
		//SetEintWakeup();
		SetKeypadWakeup();
		//SetAlarmTick();
		//SetBatteryFaultEvent(eFLT_INT);
		//SetADCTSWakeup();

		UART_Printf("System stablilization counter register!\n");
		SYSC_OSCSTAB(1);		// test. 07.03.01
		SYSC_PWRSTAB(1);

		// USB OTG Off
		//SYSC_OTGDisPwr();
		//SYSC_OTGSIGDisMSK();

		uTTT1 = Inp32SYSC(0x900);
		uTTT1 = uTTT1&~(1<<16)|(1<<14);
		
		Outp32(0x7C100000, 0xF);
		Outp32SYSC(0x900, uTTT1);
	
		UART_Printf("\nCPU will goes to Deep Stop Mode!\n\n");
		//UART_TxEmpty(1);				// 

		//Outp32SYSC(0x808, 0xFFFFFDFF );   // EINT MASK
		//Outp32SYSC(0x804, 0x61);
		//Outp32SYSC(0x818,1);

		//while(1);
		//Outp32SYSC(0x804,0x1FF80);    // WAKE_UP MASK ? or DisMASK

		Outp32Inform(7, Inp32SYSC(0x90c));
//		SYSC_SetSTOPCFG(0, uDeepStop, 0, uDeepStop);
		SYSC_SetSTOPCFG(0, uTopMEM, 0, uTopLogic);
		SYSC_SetCFGWFI(eSTOP, uOSCEN);


		// NAND Deep Stop - Work around - 02.21
		Outp32SYSC(0x0, 0x100);
		Outp32SYSC(0x4, 0x100);
		Outp32SYSC(0x8, 0x100);

		//test
		//UART_Printf("PWR_CFG : 0x%x\n", Inp32SYSC(0x804));
		//UART_Printf("STOP_CFG: 0x%x\n", Inp32SYSC(0x814));
		//UART_Printf("OTHERs: 0x%x\n", Inp32SYSC(0x900));			


		
		
		//while(!UART_GetKey());


	uRegValue_SRC = Inp32SYSC(0x1C);
	uRegValue_DIV0 = Inp32SYSC(0x20);

	uTTT0 = (uRegValue_SRC)&~(0x3);
	//uTTT1 = ((uRegValue_DIV0)&~(0xFFFF))|((1<<12)|(1<<9)|(0<<8)|(1<<4)|(0<<0));		// ARM:HCLKx2:HCLK:PCLK = 1:1:2:2
	uTTT1 = ((uRegValue_DIV0)&~(0xFFFF))|((1<<12)|(0<<9)|(1<<8)|(1<<4)|(0<<0));
	// Clock Source Change
	Outp32SYSC(0x1C, uTTT0);
	Outp32SYSC(0x20, uTTT1);

	// PLL OFF
	Outp32SYSC(0xc, 0x0);
	Outp32SYSC(0x10, 0x0);

	Stop_WFI_Test();
		//MMU_WaitForInterrupt();

		UART_Printf("CPU doesn't go to Deep Stop Mode\n");

	}

	

}
#endif

///////////////////////////////////////////////////////////////////////////
// temp
// IDLET_Basic  (O)
// STOPT_Basic (O)
// DSTOPT_Basic - DSTOP&Dormant (O)
// SLEEPT_Basic (O)
// ESLEEPT_Basic (O)
// IDLET_CFG  
// SLEEPT_CFG
// STOPT_CFG_EVT0
// STOPT_CFG_EVT1
// NORCFG_Test
// CLKGate_Test
// SUBBLKLkg_Test
// IDLET_Aging
// STOPT_Aging
// DSTOPT_Aging
// SLEEPT_Aging
// PowerMode_Aging
// 



///////////////////////////////////////////////////////////////////////////////////
////////////////////                   Power Mode  Test                  /////////////////////////// 
///////////////////////////////////////////////////////////////////////////////////

const testFuncMenu powermode_menu[] =
{
	IDLET_Basic,							"Idle Mode Test  - Basic",
	STOPT_Basic,							"Stop Mode Test - Basic",	
	DSTOPT_Basic,							"Deep Stop Mode - Baisc",
	SLEEPT_Basic,							"Sleep Mode Test - Basic",
	ESLEEPT_Basic,							"ESLEEP Mode Test -Baisc",
	
	IDLET_CFG,								"Idle Mode Config",	
	STOPT_CFG_EVT0,						"Stop Mode Test -CFG(EVT0)",	
	STOPT_CFG_EVT1,						"Stop Mode Test -CFG_EVT1",
	SLEEPT_CFG_EVT1,						"Sleep Mode Test - Basic_EVT1",	
	NORCFG_Test,							"Block Power Off Test- In Normal Mode",
	CLKGate_Test,							"Clock Gating Test         ",
	SUBBLKLkg_Test,							"Sub Block Leakage Test",
	
	IDLET_Aging,							"Idle -Aging Test (4 Int. Source)",
	STOPT_Aging,							"Stop Mode Simple Aging Test",
	DSTOPT_Aging,							"Temp Test-DeepStop,",
	
	
	PowerMode_Aging,						"Under developing code",
	Measure_MTCSTABLE,					"Measure...MTC_Stable",
	IDLE_Current_Measure,					"Idle Current Measure",
	DStop_Current_Measure,					"DStop_Current Measure",
	SLEEP_Current_Measure,					"Sleep_Current Measure",

	
	//MeasurePowerConsumption
	0, 0
};

void SYSCT_PowerMode(void)
{
	u32 i;
	s32 uSel;

	UART_Printf("[Power Mode Test]\n\n");
	
	while(1)
	{
		for (i=0; (u32)(powermode_menu[i].desc)!=0; i++)
			UART_Printf("%2d: %s\n", i, powermode_menu[i].desc);

		UART_Printf("\nSelect the function to test : ");
		uSel =UART_GetIntNum();
		UART_Printf("\n");
		if(uSel == -1) 
			break;

		if (uSel>=0 && uSel<(sizeof(powermode_menu)/8-1))
			(powermode_menu[uSel].func) ();
	}
}	

