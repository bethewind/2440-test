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
*	File Name : Timer_test.c
*  
*	File Description : This file implements the API functons for Timer (PWM, WDT) test.
*
*	Author : Woojin.Kim
*	Dept. : AP Development Team
*	Created Date : 2006/12/27
*	Version : 0.1 
* 
*	History
*	- Created(Woojin.Kim 2006/12/27)
*     - Added more test function ( Prescaler,MUX234) (Woojin.Kim 2007/02/12)
*  
**************************************************************************************/


/* Include Files */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "timer.h"
#include "def.h"
#include "option.h"
#include "library.h"
#include "sfr6410.h"
#include "system.h"
#include "sysc.h"
#include "intc.h"
#include "gpio.h"
#include "dma.h"

	extern volatile u32 g_uIntCounterOS;
	extern volatile u32 g_uIntCounter0;
	extern volatile u32 g_uIntCounter1;
	extern volatile u32 g_uIntCounter2;
	extern volatile u32 g_uIntCounter3;
	extern volatile u32 g_uIntCounter4;
//-daedoo	
//	extern volatile u32 g_uIntCounterWT;
	extern volatile u32 g_PWMDmaDone;
	static DMAC oPWMDmac;
	volatile u32 g_TimerSel;
	
void DelayfrTimer_Test(void);
void Timer_Test(void);
void TIMER_UserSelect(void);
void TIMER_Setting(void);
void TIMER_Int(void);
void TIMER_Prescaler1(void);
void TIMER_MUX234(void);
void TIMER_Dma(void);
//-daedoo
//void WDT_IntReq(void);
//void WDT_Reset(void);
void StartandStopTimer_Test(void);
void TIMER_OSREQUEST(void);

const testFuncMenu g_aTIMERTestFunc[] =
{	


      TIMER_OSREQUEST,			"Timer OS issue test",
	TIMER_UserSelect,				"Timer User's Configure",
	TIMER_Setting,				"Timer Normal    ",
	TIMER_Int,					"Timer Interrupt ",
	TIMER_Prescaler1,				"Prescaler 1 test",
	TIMER_MUX234,				"MUX 2,3,4 test",
	TIMER_Dma,				"Timer DMA       ",
//-daedoo	
//	WDT_IntReq,					"WDT Interrupt   ",
//	WDT_Reset,					"WDT Reset       ",
	DelayfrTimer_Test,			"Delay Function ",
	StartandStopTimer_Test,		"StartandStop test-Observation SFR",
	
	0,0
};


void Timer_Test(void)
{
	u32 uCountFunc=0;
	s32 iSel=0;




	
	while(1)
	{

	UART_Printf("\n\n================== Timer Function Test =====================\n\n");

		for (uCountFunc=0; (u32)(g_aTIMERTestFunc[uCountFunc].desc)!=0; uCountFunc++)
			UART_Printf("%2d: %s\n", uCountFunc, g_aTIMERTestFunc[uCountFunc].desc);

		UART_Printf("\nSelect the function to test : ");
		iSel =UART_GetIntNum();
		UART_Printf("\n");
		if(iSel == -1) 
			break;

		if (iSel>=0 && iSel<(sizeof(g_aTIMERTestFunc)/8-1))
			(g_aTIMERTestFunc[iSel].func) ();
	}
}	


void TIMER_OSREQUEST(void)
{
	PWM_stopAll();	// stop timers

	g_uIntCounterOS = 0;
	
	INTC_SetVectAddr(NUM_TIMER0,Isr_TIMEROS);
	INTC_Enable(NUM_TIMER0);
	
	PWM_Select(0,255,4,30000,15000,0,0,0,1,0,1);


	while(g_uIntCounterOS<20)
		{
			if(UART_Getc() == 'x')
			break;
			else if(g_uIntCounterOS>20)
			break;
		}

	INTC_Disable(NUM_TIMER0);

}


////////////
// File Name : TIMER_UserSelect
// File Description : Check Timer setting individually
// Input : user select
// Output : pulse through TOUT[0],[1] pin on B'd
// Version : 
void TIMER_UserSelect(void)
{


	PWM_UserSelect();
	
}

////////////
// File Name : TIMER_Setting
// File Description : Check Timer function which is 'Prescaler' 'Divider' 'TCNTB&TCMPB' 'Inverter' 'Deadzone'
// Input : user select
// Output : pulse through TOUT[0],[1] pin on B'd
// Version : 
void TIMER_Setting(void)
{
	float fTimerclk;
	float fTimerfreq;
	s32 sTimer;
	u32 uSelect;
	u32 uCount;
	u32 uTCNTB;
	u32 uTCMPB;
	u32 uTemp;

while(1)
{

	UART_Printf("[ Timer Test Setting ]\n");	
	UART_Printf(" Select Timer [0~4] :");
	sTimer = UART_GetIntNum();
	if ( sTimer ==-1)
		{
			UART_Printf("\n");
			break;
		}
	else

		GPIO_SetFunctionEach(eGPIO_F,eGPIO_13,2);		//PWM Ext
		GPIO_SetFunctionEach(eGPIO_F,eGPIO_14,2);		//PWM Tout0
		GPIO_SetFunctionEach(eGPIO_F,eGPIO_15,2);		//PWM Tout1
	
		UART_Printf("\n");

	while(1)
		{
		UART_Printf("What do you want to check?\n");
		UART_Printf("0.Prescaler Value	1. Divider Value	2. TCNTB & TCMPB\n");
		UART_Printf("3.Inverter		4. Deadzone		5. Clear all timers\n");
		UART_Printf("[0~4] :");
		uSelect = UART_GetIntNum();

		switch(uSelect)
			{
				case 0:
					uCount = 1;
					while(uCount<256)
						{
						PWM_Select(sTimer,uCount,0,2000,1000,0,0,0,1,0,0);					

						UART_Printf("Timer = %d\n",sTimer);
						UART_Printf("nPCLK = %d , uPrescaler = %d, uDivider = %d dd= %d\n",g_PCLK,uCount,0,(1<<(0)));
						fTimerclk = (1/(float)((float)g_PCLK/((float)uCount)/(1<<(0))))*2000*1000;
						fTimerfreq = 1/fTimerclk;
						UART_Printf("Timer Clock = %f msec , Timer Frequency = %f Khz\n\n",fTimerclk,fTimerfreq);

						
						uCount = uCount +1;
						UART_Printf("Check PWM out (by Pinout)!\n");

						
						UART_Getc();
						}

					break;

				case 1:
					uCount = 0;
					while(uCount <5)
						{

						PWM_Select(sTimer,1,uCount,2000,1000,0,0,0,1,0,0);		

						UART_Printf("Timer = %d\n",sTimer);
						UART_Printf("nPCLK = %d , uPrescaler = %d, uDivider = %d \n",g_PCLK,1,(1<<(uCount)));

						fTimerclk = (1/(float)((float)g_PCLK/((float)1)/(1<<(uCount))))*2000*1000;
						fTimerfreq = 1/fTimerclk;
						UART_Printf("Timer Clock = %f msec , Timer Frequency = %f Khz\n\n",fTimerclk,fTimerfreq);
						
						uCount = uCount + 1;
						UART_Printf("Check PWM out (by Pinout)! \n");

						UART_Getc();
						}
					break;

				case 2:
					UART_Printf("Select TCNTB value set mode \n");
					UART_Printf("1. Lower than half of 2^31         2.above half of 2^31      : ");
					uTemp = UART_GetIntNum();
					UART_Printf("\n");

					if (uTemp == 2)
						{
						UART_Printf("Select TCNTB \n");
						UART_Printf("1. 2^31(max)     2. 3/4 of Max   3.half of Max           :");
						uTemp = UART_GetIntNum();
						if( uTemp == 2)
							{
							uTCNTB = 0xffffffff;
							uTCNTB =(u32)((u32)(uTCNTB/4)*3);
							}
						else if (uTemp == 3)
							{
							uTCNTB = 0xffffffff;
							uTCNTB = (u32)(uTCNTB/2);
							}
						else
							uTCNTB = 0xffffffff;

						UART_Printf("nTCMPB: ");
						uTCMPB = UART_GetIntNum();

						PWM_Select(sTimer,2,0,uTCNTB,uTCMPB,0,0,0,1,0,0);					

						UART_Printf("Timer = %d, TCNTB=%ul, nTCMPB=%ul\n",sTimer,(unsigned)uTCNTB,(unsigned)uTCMPB);
						UART_Printf("nPCLK = %d , uPrescaler = %d, uDivider = %d\n",g_PCLK,2,(1<<(0)));
						fTimerclk = (1/(float)((float)g_PCLK/((float)2)/(1<<(0))))*uTCNTB;
						fTimerfreq = 1/fTimerclk;
						UART_Printf("Timer Clock = %d sec , Timer Frequency = %d hz\n\n",(unsigned)fTimerclk,(unsigned)fTimerfreq);

						UART_Printf("\nCheck PWM out (by Pinout)! \n");

						UART_Getc();
						PWM_stopAll();
						UART_Getc();						
						break;

						

						}
					else
						{
						UART_Printf("TCNTB : ");
						uTCNTB=UART_GetIntNum();
						UART_Printf("\nTCMPB : ");
						uTCMPB=UART_GetIntNum();
						PWM_Select(sTimer,1,0,uTCNTB,uTCMPB,0,0,0,1,0,0);					

						UART_Printf("Timer = %d\n",sTimer);
						UART_Printf("nPCLK = %d , uPrescaler = %d, uDivider = %d\n",g_PCLK,1,(1<<(0)));
						fTimerclk = (1/(float)((float)g_PCLK/((float)1)/(1<<(0))))*uTCNTB*1000;
						fTimerfreq = 1/fTimerclk;
						UART_Printf("Timer Clock = %f msec , Timer Frequency = %f Khz\n\n",fTimerclk,fTimerfreq);

						UART_Printf("\nCheck PWM out (by Pinout)! \n");
						UART_Getc();
						PWM_stopAll();
						UART_Getc();			
						break;
						}
				case 3:
					PWM_Select(sTimer,1,0,2000,1500,0,0,0,1,0,0);					
					UART_Printf("## Inverter off mode ##\n");
					UART_Printf("\nCheck PWM out (by Pinout)! \n");

					UART_Printf("Timer = %d\n",sTimer);
					UART_Printf("nPCLK = %d , uPrescaler = %d, uDivider = %d\n",g_PCLK,1,(1<<(0)));
					fTimerclk = (1/(float)((float)g_PCLK/((float)1)/(1<<(0))))*2000*1000;
					fTimerfreq = 1/fTimerclk;
					UART_Printf("Timer Clock = %f msec , Timer Frequency = %f Khz\n\n",fTimerclk,fTimerfreq);
					UART_Printf("Negative Pulse Width : 1/4, Positive Pulse Width: 3/4\n");
					
					UART_Getc();
					
					PWM_Select(sTimer,1,0,2000,1500,0,0,0,1,1,0);					

					UART_Printf("## Inverter ON mode ##\n");
					UART_Printf("\nCheck PWM out (by Pinout)! \n");

					UART_Printf("Timer = %d\n",sTimer);
					UART_Printf("nPCLK = %d , uPrescaler = %d, uDivider = %d\n",g_PCLK,1,(1<<(0)));
					fTimerclk = (1/(float)((float)g_PCLK/((float)1)/(1<<(0))))*2000*1000;
					fTimerfreq = 1/fTimerclk;
					UART_Printf("Timer Clock = %f msec , Timer Frequency = %f Khz\n\n",fTimerclk,fTimerfreq);
					UART_Printf("Negative Pulse Width : 3/4, Positive Pulse Width: 1/4\n");
					
					UART_Getc();
					break;
					
				case 4:
					UART_Printf("Deadzone length? [0~255] :");
					uCount = UART_GetIntNum();
					PWM_Select(sTimer,1,0,2000,1000,1,uCount,0,1,0,0);										

					UART_Printf("Timer = %d\n",sTimer);
					UART_Printf("nPCLK = %d , uPrescaler = %d, uDivider = %d\n",g_PCLK,1,(1<<(0)));
					fTimerclk = (1/(float)((float)g_PCLK/((float)1)/(1<<(0))))*2000*1000;
					fTimerfreq = 1/fTimerclk;
					UART_Printf("Timer Clock = %f msec , Timer Frequency = %f Khz\n\n",fTimerclk,fTimerfreq);
					
					UART_Printf("\nCheck PWM out (by Pinout)! \n");
					UART_Getc();
					break;

				case 5:
					PWM_stopAll();
					break;
				default:
					break;
			}

			break;
		}

	}
	GPIO_SetFunctionEach(eGPIO_F,eGPIO_14,0);
	GPIO_SetFunctionEach(eGPIO_F,eGPIO_15,0);	
}



////////////
// File Name : TIMER_Int
// File Description : Check Timer Autoreload & oneshot Interrupt
// Input : NONE
// Output : NONE
// Version : 

void TIMER_Int(void)
{
   g_uIntCounter0 =0;   
   g_uIntCounter1 =0;   
   g_uIntCounter2 =0;   
   g_uIntCounter3 =0;   
   g_uIntCounter4 =0;		

	PWM_stopAll();	// stop timers
	
	INTC_SetVectAddr(NUM_TIMER0,Isr_TIMER0);
	INTC_SetVectAddr(NUM_TIMER1,Isr_TIMER1);
	INTC_SetVectAddr(NUM_TIMER2,Isr_TIMER2);
	INTC_SetVectAddr(NUM_TIMER3,Isr_TIMER3);	
	INTC_SetVectAddr(NUM_TIMER4,Isr_TIMER4);	
	INTC_Enable(NUM_TIMER0);
	INTC_Enable(NUM_TIMER1);
	INTC_Enable(NUM_TIMER2);
	INTC_Enable(NUM_TIMER3);
	INTC_Enable(NUM_TIMER4);
	
	
    UART_Printf("\n[Timer 0,1,2,3,4 Interrupt Test]\n\n");

    //Dead zone=0,Prescaler1=255(0x0f),Prescaler0=255(0x0f)
    //All interrupt,Mux4=1/2,Mux3=1/4,Mux2=1/8,Mux1=1/16,Mux0=1/16

	PWM_Select(0,255,4,30000,15000,0,0,0,1,0,1);
	PWM_Select(1,255,3,30000,15000,0,0,0,1,0,1);
	PWM_Select(2,255,2,30000,15000,0,0,0,1,0,1);
	PWM_Select(3,255,1,30000,15000,0,0,0,1,0,1);	
	PWM_Select(4,255,0,30000,15000,0,0,0,1,0,1);	




    while(1)
    {
	if(g_uIntCounter4 == 16)
	break;
    }
	PWM_stopAll();	 //Stop Timers
   
    if(g_uIntCounter4==16 && g_uIntCounter3==8 && g_uIntCounter2==4 
		&& g_uIntCounter1==2 && g_uIntCounter0==1)
	    {
      		  UART_Printf("Timer 0,1,2,3,4 Auto-reload Interrupt Test => OK!\n");
	    }        
    else
	    {
	        UART_Printf("Timer 0,1,2,3,4 Auto-reload Interrupt Test => Fail............\n");
	    }    
    	UART_Printf("Timer0: %d (=1)\nTimer1: %d (=2)\nTimer2: %d (=4)\nTimer3: %d (=8)\nTimer4: %d (=16)\n",
        g_uIntCounter0,g_uIntCounter1,g_uIntCounter2,g_uIntCounter3,g_uIntCounter4);        
	


// OneShot Test //
  	g_uIntCounter0 = 0;g_uIntCounter1 = 0;g_uIntCounter2 = 0;g_uIntCounter3 = 0;g_uIntCounter4 = 0;

    //T0,1,2,3,4= One-shot,Inverter off,No operation,Dead zone disable,Start
	PWM_Select(4,255,0,30000,15000,0,0,0,0,0,1);	  	
	PWM_Select(3,255,1,30000,15000,0,0,0,0,0,1);
	PWM_Select(2,255,2,30000,15000,0,0,0,0,0,1);
	PWM_Select(1,255,3,30000,15000,0,0,0,0,0,1);
	PWM_Select(0,255,4,30000,15000,0,0,0,0,0,1);


	while(g_uIntCounter0 == 0);     

	PWM_stopAll(); //One-shot, Inverter off, No operation, Dead zone disable, Stop
   
    if(g_uIntCounter4==1 && g_uIntCounter3==1 && g_uIntCounter2==1 
		&& g_uIntCounter1==1 && g_uIntCounter0==1)
   	 {
    		UART_Printf("Timer 0,1,2,3,4 One-shot Interrupt Test => OK!\n");
	 }        
    else
	 {
       	 UART_Printf("Timer 0,1,2,3,4 One-shot Interrupt Test => Fail............\n");
	 }    

	UART_Printf("Timer0: %d (=1)\nTimer1: %d (=1)\nTimer2: %d (=1)\nTimer3: %d (=1)\nTimer4: %d (=1)\n", g_uIntCounter0,g_uIntCounter1,g_uIntCounter2,g_uIntCounter3,g_uIntCounter4);        


	// Timer0,1,2,3,4 Interrupt Service is masked //
	INTC_Disable(NUM_TIMER4);
	INTC_Disable(NUM_TIMER3);
	INTC_Disable(NUM_TIMER2);
	INTC_Disable(NUM_TIMER1);
	INTC_Disable(NUM_TIMER0);
	
	UART_Printf("Press any key\n");      
	while(!UART_Getc());       //Key board press check      

}



////////////
// File Name : TIMER_Prescaler1
// File Description : Check Prescaler1 values by Comparing Timer0 & Timer2's Interrupt 
//			(SMDK6400 doesn't have TOUT[2],[3],[4] pin out therefore used Interrupt check)
// Input : NONE
// Output : NONE
// Version : 

void TIMER_Prescaler1(void)
{
	u32 uPrescaler;
	u32 uCnt;

	PWM_stopAll();	// stop timers
	INTC_SetVectAddr(NUM_TIMER0,Isr_TIMER0);
	INTC_SetVectAddr(NUM_TIMER2,Isr_TIMER2);	
	INTC_Enable(NUM_TIMER0);
	INTC_Enable(NUM_TIMER2);

	UART_Printf("Prescaler1 test start\n\n");
	uCnt = 0;
	uPrescaler = 0;
	
	while(uPrescaler<255)
		{

		UART_Printf("test Prescaler value = %d   ",uPrescaler);
		
		PWM_Select(2,uPrescaler,4,10000,5000,0,0,0,1,0,1);	
		PWM_Select(0,uPrescaler,4,10000,5000,0,0,0,1,0,1);	  	

		while (g_uIntCounter0 == 20);
		PWM_stopAll();
		
		if (g_uIntCounter0 == g_uIntCounter2)
 			UART_Printf("Prescaler 1 test ==> OK!\n");
		else
			{
			UART_Printf("Prescaler 1 test ==> Fail........%d, %d\n",g_uIntCounter0,g_uIntCounter2);
		
			uCnt++;
			}
		uPrescaler = uPrescaler + 50;

		g_uIntCounter0 =0;
		g_uIntCounter2=0;
		
		}
#if 0
		uPrescaler = 255;
		UART_Printf("test Prescaler value = %d   ",uPrescaler);
		
		PWM_Select(2,uPrescaler,4,10000,5000,0,0,0,1,0,1);	
		PWM_Select(0,uPrescaler,4,10000,5000,0,0,0,1,0,1);	  	

		while (g_uIntCounter0 == 20);
		PWM_stopAll();
		
		if (g_uIntCounter0 == g_uIntCounter2)
 			UART_Printf("Prescaler 1 test ==> OK!\n");
		else
			{
			UART_Printf("Prescaler 1 test ==> Fail........\n");
			uCnt++;
			}
#endif


	if (uCnt >0)
		UART_Printf("Total Prescaler 1 test Fail\n");
	else
		UART_Printf("Total Prescaler 1 test Success!!\n\n");
	

	UART_Printf("When you confirmed Timer 0, 1 's Prescaler value(Prescaler0) , this result is confirmable!!\n\n");
	
	INTC_Disable(NUM_TIMER2);
	INTC_Disable(NUM_TIMER0);

}


////////////
// File Name : TIMER_MUX234
// File Description : Check MUX2,3,4 values by Comparing with Timer0 Interrupt 
//			(SMDK6400 doesn't have TOUT[2],[3],[4] pin out therefore use Interrupt check)
// Input : NONE
// Output : NONE
// Version : 
void TIMER_MUX234(void)
{
	u32 uSelMUX;
	u32 uMux;
	u32 uCnt;
	
	UART_Printf("Which MUX do you want to check?\n");
	UART_Printf("MUX[2~4]  :");
	uSelMUX = UART_GetIntNum();

	PWM_stopAll();	// stop timers
	INTC_SetVectAddr(NUM_TIMER0,Isr_TIMER0);
	INTC_SetVectAddr(NUM_TIMER2,Isr_TIMER2);
	INTC_SetVectAddr(NUM_TIMER3,Isr_TIMER3);	
	INTC_SetVectAddr(NUM_TIMER4,Isr_TIMER4);	
	INTC_Enable(NUM_TIMER0);
	INTC_Enable(NUM_TIMER2);
	INTC_Enable(NUM_TIMER3);
	INTC_Enable(NUM_TIMER4);

	uCnt = 0;
	
	for(uMux=0;uMux<5;uMux++)
		{

	  	g_uIntCounter0 = 0;
		g_uIntCounter2 = 0;
		g_uIntCounter3 = 0;
		g_uIntCounter4 = 0;		

		PWM_Select(uSelMUX,255,uMux,3000,1500,0,0,0,1,0,1);	
		PWM_Select(0,255,0,3000,1500,0,0,0,1,0,1);	  	


		switch(uSelMUX)
			{
			case 2:
				
				while(!(g_uIntCounter0 == 16));     
					PWM_stopAll(); //One-shot, Inverter off, No operation, Dead zone disable, Stop
			   
			    	if( (g_uIntCounter2== (0x10>>uMux)) && g_uIntCounter0==16)
			   		 {
			    			UART_Printf("Timer 2  MUX2 1/%d Test => OK!\n",(1<<uMux));
					 }        
			    	else
					 {
			       	 	UART_Printf("Timer 2  MUX2 1/%d Test => Fail............\n",(1<<uMux));
						uCnt++;
					 }    

				UART_Printf("Timer0: %d (=16)\n Timer%d: %d (=%d)\n", g_uIntCounter0,uSelMUX,g_uIntCounter2,(0x10>>uMux));        
				break;

			case 3:

				while(!(g_uIntCounter0 == 16));     
				PWM_stopAll(); //One-shot, Inverter off, No operation, Dead zone disable, Stop
			   
			    	if(g_uIntCounter3==(0x10>>uMux) && g_uIntCounter0==16)
			   		 {
			    			UART_Printf("Timer 3 MUX3 1/%d Test => OK!\n",(1<<uMux));
					 }        
			    	else
					 {
			       	 	UART_Printf("Timer 3 MUX3 1/%d Test => Fail............\n",(1<<uMux));
						uCnt++;						
					 }    

				UART_Printf("Timer0: %d (=16)\n Timer%d: %d (=%d)\n", g_uIntCounter0,uSelMUX,g_uIntCounter3,(0x10>>uMux));        
				break;

			case 4:

				while(!(g_uIntCounter0 == 16));     
				PWM_stopAll(); //One-shot, Inverter off, No operation, Dead zone disable, Stop
			   
			    	if(g_uIntCounter4==(0x10>>uMux) && g_uIntCounter0==16)
			   		 {
			    			UART_Printf("Timer 4 MUX4 1/%d Test => OK!\n",(1<<uMux));
					 }        
			    	else
					 {
			       	 	UART_Printf("Timer 4 MUX4 1/%d Test => Fail............\n",(1<<uMux));
						uCnt++;						
					 }    

				UART_Printf("Timer0: %d (=16)\n  Timer%d: %d (=%d)\n", g_uIntCounter0,uSelMUX,g_uIntCounter4,(0x10>>uMux));        
				break;

			default :
				break;
			
			}


		}


	// Timer0,1,2,3,4 Interrupt Service is masked //
		INTC_Disable(NUM_TIMER4);
		INTC_Disable(NUM_TIMER3);
		INTC_Disable(NUM_TIMER2);
		INTC_Disable(NUM_TIMER0);

	if (uCnt >0)
		UART_Printf("\nMUX %d test Fail\n",uSelMUX);
	else
		UART_Printf("\nMUX %d test Success!!\n",uSelMUX);
	

	UART_Printf("When you confirmed Timer 0's Divider value , this result is confirmable!!\n\n");

	
}


////////////
// File Name : Isr_TimerDMA
// File Description : for Timer's DMA request Interrupt Service Routine
//                           In this test, we transmitted rTCMPB0 value from memory by DMA
// Input : NONE
// Output : NONE
// Version : 
void __irq Isr_TimerDMA(void)
{
	u32 uRegTmp;
	u32 uRegTmp2;


	INTC_Disable(NUM_DMA1);

	DMACH_Stop(&oPWMDmac);
	UART_Printf ("DMA request %d              rTCMPB value changed :%d\n",g_PWMDmaDone,PWM_ReadrTCMPB(3));
	g_PWMDmaDone--;	

	// Interrupt Clear
	DMACH_ClearErrIntPending(&oPWMDmac);
  	DMACH_ClearIntPending(&oPWMDmac);

	uRegTmp = DMACH_ReadDstAddr(&oPWMDmac);
	uRegTmp2 = DMACH_ReadSrcAddr(&oPWMDmac);			// next SrcAddress
	uRegTmp2 +=4;
	DMACH_AddrSetup(DMA_A,uRegTmp2,uRegTmp,&oPWMDmac);
	DMACH_SetTransferSize(1,&oPWMDmac);					// set TransferSize
	DMACH_Start(&oPWMDmac);

	INTC_Enable(NUM_DMA1);
	INTC_ClearVectAddr();

}


////////////
// File Name : Comp32
// File Description : This function is for Timer DMA request test. Comparing Tx/Rx memory data
// Input : a0 [Tx mem addr.]  a1[Rx mem addr.]  words[length]
// Output : false/true
// Version : 
u8 Comp32(u32 a0, u32 a1, u32 words)
{
	u32* pD0 = (u32 *)a0;
	u32* pD1 = (u32 *)a1;
	u8 ret = true;
	u32  ecnt = 0;
	u32 i;

	for (i=0; i<words; i++)
	{
		if (*pD0 != *pD1) 
		{
			ret = false;
			UART_Printf(" %08x=%08x <-> %08x=%08x\n", pD0, *pD0, pD1, *pD1);
			ecnt++;
		}
		pD0++;
		pD1++;
	}

	if (ret == false)
		UART_Printf("\n");

	return ret;
}


////////////
// File Name : TIMER_Dma
// File Description : This is test function for Timer DMA request which transmit rTCMPB value from MEM to PWM regisnter
// Input : NONE
// Output : NONE
// Version : 
void TIMER_Dma(void)
{
	// SMDK6400 _DRAM_BaseAddress + 0x800_0000;
	u32 uTxBuffAddr = _DRAM_BaseAddress + 0x1000000;
	u32 uCnt;
	u32 uDataCnts;
	//u32 uTimerSel;

	SYSC_SelectDMA(eSEL_PWM, 1);
	
     	DMAC_InitCh(DMA1, DMA_A, &oPWMDmac);
	INTC_SetVectAddr(NUM_DMA1,  Isr_TimerDMA);
	INTC_Enable(NUM_DMA1);

	DMACH_ClearErrIntPending(&oPWMDmac);
  	DMACH_ClearIntPending(&oPWMDmac);

       UART_Printf("\nTimer Select [0~4] :   ");
	g_TimerSel=UART_GetIntNum();
	
       UART_Printf("\nSelect Transfer Size [1~150] :   ");
	uDataCnts=UART_GetIntNum();
	UART_Printf("\n");

	g_PWMDmaDone = uDataCnts;
	   	
	// Set up the tx buf.
	for (uCnt = 0; uCnt<uDataCnts; uCnt++)
		*((u32 *)(uTxBuffAddr)+uCnt) = (u32)(uCnt*200);
	
	
        // Channel, LLI_Address, SrcAddr, Src Type, DstAddr, Dst Type, Transfer Width, Transfer Size, OpMode(DEMAND), Src Req, Dst Req, Burst
        // Channel Set-up
	DMACH_Setup(DMA_A, 0x0, uTxBuffAddr, 0, 0x7f006034, 1, WORD,1, DEMAND, MEM,DMA1_PWM,SINGLE, &oPWMDmac);


        // Enable DMA
	DMACH_Start(&oPWMDmac);

	//Start Timer0 with DMA request
	//PWM_Select(u32 uTimer,u32 uPrescaler,u32 uDivider,u32 uTCNTB,u32 uTCMPB,u32 uEnDz,u32 uDzlen,u32 uEnDMA,u32 uAutoreload,u32 uEnInverter,u32 uEnInt)
	PWM_Select(g_TimerSel,50,4,30000,15000,0,0,1,1,0,1);
	Outp32(rTINT_CSTAT, 0x0);
	
	while(g_PWMDmaDone>0);

	PWM_stopAll();
	DMACH_Stop(&oPWMDmac);
	INTC_Disable(NUM_DMA1);

}

//-daedoo
#if 0
////////////
// File Name : WDT_IntReq
// File Description : This function checks WDT Interrupt by changing division factor, prescaler value & WTDAT
// Input : NONE
// Output : NONE
// Version : 
void WDT_IntReq(void)
{
	s32 sDiv;
	u32 uPrescaler;
	u32 uWTdat;
	
	// WDT ISR addr Initialization // 

	INTC_SetVectAddr(NUM_WDT,Isr_WDT);	
	g_uIntCounterWT = 0;

	//f_watchdog = PCLK / (Prescaler value + 1 ) / Division_factor
	UART_Printf("[WatchDog Timer Interrupt Request Test]\n");

	/* WDT INTMSK Enable */
	INTC_Enable(NUM_WDT);


	/* WDT Interrupt Test */

	// 1. Clock division_factor test
	UART_Printf("\n1. Clock division factor test\n");
	sDiv = 0;
	uPrescaler = 0;
	uWTdat = 50000;
	while(sDiv <= 3)
		{
	g_uIntCounterWT = 0;		
	UART_Printf("\nClock Division Factor: %d(dec), Prescaler: %d(dec)\n",(1<<(sDiv+4)),uPrescaler+1);
	WDT_operate(0,1,(u32)sDiv,1,uPrescaler,uWTdat,uWTdat);
	while(g_uIntCounterWT != 10);
	sDiv++;
		}
#if 1
	// 2. Clock Prescaler_factor test
	UART_Printf("\n2. Prescaler value test\n");

	uPrescaler = 0;
	uWTdat = 5000;
	while( uPrescaler <= 60)
		{
		sDiv =1;
		g_uIntCounterWT = 0;
		UART_Printf("\nClock Division Factor: %d(dec), Prescaler: %d(dec)\n",(1<<(sDiv+4)),uPrescaler);
		WDT_operate(0,1,(u32)sDiv,1,uPrescaler,uWTdat,uWTdat);
		while(g_uIntCounterWT <10);
		uPrescaler=uPrescaler+20;
		}

	
	while( uPrescaler <= 0xff)
		{
		sDiv =0;
		g_uIntCounterWT = 0;
		UART_Printf("\nClock Division Factor: %d(dec), Prescaler: %d(dec)\n",(1<<(sDiv+4)),uPrescaler);
		WDT_operate(0,1,(u32)sDiv,1,uPrescaler,uWTdat,uWTdat);
		while(g_uIntCounterWT <10);
		uPrescaler=uPrescaler+20;
		}


	sDiv =0;
	uPrescaler = 255;
	uWTdat = 5000;
	g_uIntCounterWT = 0;
	UART_Printf("\nClock Division Factor: %d(dec), Prescaler: %d(dec)\n",(1<<(sDiv+4)),uPrescaler);
	WDT_operate(0,1,(u32)sDiv,1,uPrescaler,uWTdat,uWTdat);
	while(g_uIntCounterWT != 10);

#endif
	
	// 2. WTDAT value test
	UART_Printf("\n3. WTDAT test\n");
	sDiv =1;
	uPrescaler = 0;
	uWTdat = 0xffff;

	g_uIntCounterWT = 0;
	UART_Printf("\nWTDAT: %d(dec)\n",uWTdat);
	WDT_operate(0,1,(u32)sDiv,1,uPrescaler,uWTdat,uWTdat);
	while(g_uIntCounterWT != 10);

	uWTdat = 0xfff0;
	g_uIntCounterWT = 0;
	UART_Printf("\nWTDAT: %d(dec)\n",uWTdat);
	WDT_operate(0,1,(u32)sDiv,1,uPrescaler,uWTdat,uWTdat);
	while(g_uIntCounterWT != 10);

	uWTdat = 0xff00;
	g_uIntCounterWT = 0;
	UART_Printf("\nWTDAT: %d(dec)\n",uWTdat);
	WDT_operate(0,1,(u32)sDiv,1,uPrescaler,uWTdat,uWTdat);
	while(g_uIntCounterWT != 10);

	uWTdat = 0xf000;
	g_uIntCounterWT = 0;
	UART_Printf("\nWTDAT: %d(dec)\n",uWTdat);
	WDT_operate(0,1,(u32)sDiv,1,uPrescaler,uWTdat,uWTdat);
	while(g_uIntCounterWT != 10);


	sDiv =3;
	uPrescaler = 100;
	uWTdat = 1000;

	g_uIntCounterWT = 0;
	UART_Printf("\nWTDAT: %d(dec)\n",uWTdat);
	WDT_operate(0,1,(u32)sDiv,1,uPrescaler,uWTdat,uWTdat);
	while(g_uIntCounterWT != 10);

	

	/* WDT Reset Test */
    UART_Printf("\nI will restart after 2 sec.\n");
	g_uIntCounterWT = 0;
	WDT_operate(1,1,0,1,100,31250,31250);    


 
	while(!UART_GetKey());
	INTC_Disable(NUM_WDT);


}


////////////
// File Name : WDT_Rest
// File Description : This function checks watchdog reset
// Input : NONE
// Output : NONE
// Version : 
void WDT_Reset(void)
{
    
	// WDT ISR addr Initialization // 

	INTC_SetVectAddr(NUM_WDT,Isr_WDT);	
	g_uIntCounterWT = 0;

	UART_Printf("[WatchDog Timer Reset Test]\n");

	INTC_Enable(NUM_WDT);
	
	// 1. Clock division_factor 128
	UART_Printf("\nClock Division Factor: 1(dec), Prescaler: 100(dec)\n");
	WDT_operate(0,1,0,1,100,15625,15625);

    UART_Printf("WDT Reset after wdt interrupt happened 5 times. \n");	   

	while(g_uIntCounterWT != 5); // variable
 
    // WDT reset enable
    UART_Printf("\nI will restart after 2 sec.\n");
	g_uIntCounterWT = 0;
	WDT_operate(1,1,0,1,100,15625,15625);    
	
    
    while(!UART_GetKey());
	INTC_Disable(NUM_WDT);

}
#endif

////////////
// File Name : DelayfrTimer_Test
// File Description : This function checks DelayfrTimer function
// Input : NONE
// Output : NONE
// Version : 
void DelayfrTimer_Test(void)
{
	u32 cMea;
	s32 uInput;

	while(1)
	{
		UART_Printf("What do you want to have as a measurement of delay? \n");
		UART_Printf("[0: micro sec      1:milli sec]            : ");
		cMea = UART_GetIntNum();
		UART_Printf("\n");			
		UART_Printf("How much delay do you want to have?  \n");
		UART_Printf("ex) 1 = 1msec, 100 = 100msec :          ");
		uInput = UART_GetIntNum();
		UART_Printf("\n");	
		if(uInput== -1) 
			break;
				
		DelayfrTimer((eDelay_M)cMea,uInput);
		UART_Printf("This is %dmsec delayed time!!\n\n",uInput);
		
	}
			
}

////////////
// File Name : DelayfrTimer_Test
// File Description : This function checks Start and Stop Timer function with DelayfrTimer
// Input : NONE
// Output : NONE
// Version : 
void StartandStopTimer_Test(void)
{
	u32 uVal, uTimer;
//	u32 uCn;

	UART_Printf(" Select Timer [0~4] :");
	uTimer = UART_GetIntNum();
	
	StartTimer(uTimer);

	DelayfrTimer(milli,1000);
//	for(uCn=0;uCn<5000;uCn++);

   	uVal = StopTimer(uTimer);
	UART_Printf("Timer%d %d microsecond has passed!\n",uTimer,uVal);
	

}






