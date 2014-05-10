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
*	File Name : reset_test.c
*  
*	File Description : This file implements the test functons for the Reset Mode
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

#include "rtc.h"
//#include "adcts.h"
//#include "lcd.h"
#include "sysc.h"
#include "power_test.h"
#include "reset_test.h"
#include "timer.h"
#include "dma.h"

#include "post.h"
//#include "rotator.h"
#include "lcd.h"

static DMAC oDmac_0;
static DMAC oDmac_1;
extern volatile int g_DmaDone0;
extern volatile int g_DmaDone1;
extern u32 g_OnTest;

#define LCD_FRAMEBUFFER_0 = _DRAM_BaseAddress + 0x1000000;
#define LCD_FRAMEBUFFER_1 = _DRAM_BaseAddress + 0x2000000;
#define LCD_FRAMEBUFFER_2 = _DRAM_BaseAddress + 0x3000000;
#define LCD_FRAMEBUFFER_3 = _DRAM_BaseAddress + 0x4000000;
#define LCD_FRAMEBUFFER_4 = _DRAM_BaseAddress + 0x5000000;


//////////
// Function Name : Test_SoftReset
// Function Description : This function test software reset 
//					 
// Input : 	
// Output : 	
//			
// Version : v0.1
void Test_SoftReset(void)
{
	u32 uRstId;
	u32 uInform0, uInform1;

	UART_Printf("rINFORM0: 0x%x\n", Inp32Inform(0));
	UART_Printf("rINFORM1: 0x%x\n", Inp32Inform(1));

	uInform0 = 0xABCD6400;
	uInform1 = 0x6400ABCD;

	uRstId = SYSC_RdRSTSTAT(1);
	SYSC_RdBLKPWR();

	if( ( uRstId == 5 ) && !(g_OnTest) )
	{
		UART_Printf("Software Reset- Memory data check \n");
		CheckData_SDRAM(_DRAM_BaseAddress+0x1000000, 0x10000);

		//Check Information Register Value
		if( (uInform0 !=Inp32Inform(0) )||(uInform1 != Inp32Inform(1)))
		{
			UART_Printf(" Information Register Value is wrong!!! \n");
		}
		else 
		{
			UART_Printf(" Information Register Value is correct!!! \n");
		}

		UART_Printf("software reset test is done\n");
		g_OnTest = 1;
		SYSC_BLKPwrONAll();
		Delay(10);
		SYSC_RdBLKPWR();

	}
	else
	{
		UART_Printf("[SoftReset Test]\n");
		InitData_SDRAM(_DRAM_BaseAddress+0x1000000, 0x10000);

              //Added case : bus power down
              SYSC_BLKPwrOffAll();

              //Added case : Clock Off Case
             // Outp32SYSC(0x30, 0xFDDFFFFE);	//IROM, MEM0, MFC
             // Outp32SYSC(0x30, 0xFFFFFFFE);	// MFC, MFC Block OFF시 OK
            //  Outp32SYSC(0x30, 0xFDFFFFFF);	// IROM  OK
             //  Outp32SYSC(0x30, 0xFFDFFFFF);	// MEM0
		UART_Printf("HCLKGATE: 0x%x\n", Inp32(0x7E00F030));
		// Alive Register Write
		Outp32Inform(0, uInform0);
		Outp32Inform(1, uInform1);
		//Outp32(0x7F008880, 0x1000);
              		
		UART_TxEmpty();
		UART_Printf("Now, Soft Reset causes reset on 6400 except SDRAM. \n");   
		SYSC_SWRST();

		while(!UART_GetKey());
	
	}
	    
}



void __irq Dma0Done_Test(void)
{
       u32 uTime;
       
  	DMACH_ClearIntPending(&oDmac_0);
 	//rDMAC0IntTCClear = 0xff;
	uTime = StopTimer(0);
	UART_Printf ("DMAC0 %d\n", uTime);

	g_DmaDone0=1;
	INTC_ClearVectAddr();
       //Write_VECTADDR(0x0);
}

void __irq Dma1Done_Test(void)
{
//	UART_Printf("Current TC1 : %x\n",rDMAC1C0Control);
	   u32 uTime;

	DMACH_ClearIntPending(&oDmac_1);
	//rDMAC1IntTCClear = 0xff;
	uTime = StopTimer(1);
	UART_Printf ("DMAC1 %d\n", uTime);
	g_DmaDone1=1;

	INTC_ClearVectAddr();
}
//////////
// Function Name : Test_WarmReset
// Function Description : This function test software reset 
//					 
// Input : 	
// Output : 	
//			
// Version : v0.1
void Test_WarmReset(void)
{

	u32 uRstId;
	u32 uInform0, uInform1;

	uRstId = SYSC_RdRSTSTAT(1);


	// Check Alive Reg.
	
	// Alive Register

	uInform0 = 0x01234567;
	uInform1 = 0x6400ABCD;

	// For Test
	//WDT_operate(1,0,0,1,100,15625,15625);

	if( ( uRstId == 1 ) && !(g_OnTest) )
	{
		UART_Printf("Warm Reset- Memory data check \n");
		CheckData_SDRAM(_DRAM_BaseAddress+0x1000000, 0x10000);

		//Check Information Register Value
		if( (uInform0 !=Inp32Inform(0) )||(uInform1 != Inp32Inform(1)))
		{
			UART_Printf(" Information Register Value is wrong!!! \n");
		}
		else 
		{
			UART_Printf(" Information Register Value is correct!!! \n");
		}

		UART_Printf("Warm Reset test is done\n");
		g_OnTest = 1;
		SYSC_BLKPwrONAll();
		Delay(10);
		SYSC_RdBLKPWR();

	}
	else
	{
		UART_Printf("[WarmReset Test]\n");
		InitData_SDRAM(_DRAM_BaseAddress+0x1000000, 0x10000);
		// Alive Register Write
		Outp32Inform(0, uInform0);
		Outp32Inform(1, uInform1);

              //Added case : bus power down
              SYSC_BLKPwrOffAll();
		              
		//Block Power Off....
		/*  개별 동작 OK
		SYSC_CtrlNORCFG(eNOR_DOMAINV, 0);
              SYSC_CtrlNORCFG(eNOR_DOMAINI, 0);
              SYSC_CtrlNORCFG(eNOR_DOMAINP, 0);
              SYSC_CtrlNORCFG(eNOR_DOMAINF, 0);
              SYSC_CtrlNORCFG(eNOR_DOMAINS, 0);
              SYSC_CtrlNORCFG(eNOR_DOMAINETM, 0);
              SYSC_CtrlNORCFG(eNOR_IROM, 0);
              */

             // Outp32SYSC(0x810, 0xBFFE0D00);
		//UART_Printf("rNOR_CFG: 0x%x\n", Inp32SYSC(0x810));

              //Added case : Clock Off Case
             // Outp32SYSC(0x30, 0xFDDFFFFE);	
            //  Outp32SYSC(0x30, 0xFFFFFFFE);	// MFC, MFC Block OFF시 OK
            //  Outp32SYSC(0x30, 0xFDFFFFFF);	// IROM  OK
            //  Outp32SYSC(0x30, 0xFFDFFFFF);	// MEM0
		UART_Printf("HCLKGATE: 0x%x\n", Inp32(0x7E00F030));
		
		UART_Printf("Now, Push Warm Reset Botton. \n");   

		while(1)
		{
		// test case 
		DMAC_InitCh(DMA0, DMA_ALL, &oDmac_0);
     		DMAC_InitCh(DMA1, DMA_ALL, &oDmac_1);
		INTC_SetVectAddr(NUM_DMA0,  Dma0Done_Test);
		INTC_SetVectAddr(NUM_DMA1,  Dma1Done_Test);
		INTC_Enable(NUM_DMA0);
		INTC_Enable(NUM_DMA1);

	 	g_DmaDone0=0;
		g_DmaDone1=0;
	
	 	UART_Printf("DMA Start \n");
		// 16MB
		DMACH_Setup(DMA_A, 0x0, 0x51f00000, 0, 0x51f01000, 0, WORD, 0x1000000, DEMAND, MEM, MEM, BURST4, &oDmac_0);
		DMACH_Setup(DMA_A, 0x0, 0x52000000, 0, 0x52001000, 0, WORD, 0x1000000, DEMAND, MEM, MEM, BURST4, &oDmac_1);
       	 // Enable DMA
      	 
		DMACH_Start(&oDmac_0);
		DMACH_Start(&oDmac_1);

		while((g_DmaDone0==0)||(g_DmaDone1==0))							// Int.
		{
			Copy(0x51000000, 0x51800000, 0x1000000);
		}

		}

	
	}
   
}


void Test_WDTReset(void)
{
    
		
	UART_Printf("[WatchDog Timer Reset Test]\n");

	INTC_Enable(NUM_WDT);
	
	// 1. Clock division_factor 128
	UART_Printf("\nClock Division Factor: 1(dec), Prescaler: 100(dec)\n");
	 // WDT reset enable
	UART_Printf("\nI will restart after 2 sec.\n");

	 //-daedoo
	//WDT_operate(1,1,0,1,100,15625,15625);    
	WDT_EnableReset(true);
	WDT_IntEnable(true);
	WDT_Init(100, 0, 15625);
	WDT_Enable(true);

	//Test Case - add SUB Block Off
	SYSC_BLKPwrOffAll();

	//Added case : Clock Off Case
       Outp32(0x7E00F030, 0xFDDFFFFE);
             // Outp32SYSC(0x30, 0xFFFFFFFE);	// MFC, MFC Block OFF시 OK
            // Outp32SYSC(0x30, 0xFDFFFFFF);	// IROM  OK
              //Outp32SYSC(0x30, 0xFFDFFFFF);	// MEM0
	UART_Printf("HCLKGATE: 0x%x\n", Inp32(0x7E00F030));
    
    //while(!UART_GetKey());
    // Test Case - add Bus operation
	while(1)	
	{
		// test case 
		DMAC_InitCh(DMA0, DMA_ALL, &oDmac_0);
     		DMAC_InitCh(DMA1, DMA_ALL, &oDmac_1);
		INTC_SetVectAddr(NUM_DMA0,  Dma0Done_Test);
		INTC_SetVectAddr(NUM_DMA1,  Dma1Done_Test);
		INTC_Enable(NUM_DMA0);
		INTC_Enable(NUM_DMA1);

	 	g_DmaDone0=0;
		g_DmaDone1=0;
	
	 	UART_Printf("DMA Start \n");
		// 16MB
		DMACH_Setup(DMA_A, 0x0, 0x51f00000, 0, 0x51f01000, 0, WORD, 0x1000000, DEMAND, MEM, MEM, BURST4, &oDmac_0);
		DMACH_Setup(DMA_A, 0x0, 0x52000000, 0, 0x52001000, 0, WORD, 0x1000000, DEMAND, MEM, MEM, BURST4, &oDmac_1);
       	 // Enable DMA
      	 
		DMACH_Start(&oDmac_0);
		DMACH_Start(&oDmac_1);

		while((g_DmaDone0==0)||(g_DmaDone1==0))							// Int.
		{
			Copy(0x51000000, 0x51800000, 0x1000000);
		}

	}
    
	//INTC_Disable(NUM_WDT);

}



double VFPtest(double x, double y)
{    return x + y;
}

void Test_VFP(void)
{
   double val1 = 1.0;
    double val2 = 2.0;
    double val3 = 2.22524e-308; //Small, just normalized number
    double val4 = 3.1234e-322; // denormalized number
    //double val4 = 3.1234e-308; // denormalized number
    double res1;
    double res2;
    double res3;
    UART_Printf("VFP Support Code Test\n");
    UART_Printf("=====================\n\n");
    UART_Printf("Non-bouncing calculation of %f + %f\n",val1,val2);
    UART_Printf("The result should be : 3.000000\n");
    res1=VFPtest(val1,val2);
    UART_Printf("The result is        : %f\n\n", res1);
    UART_Printf("Non-bouncing calculation of %e + %e\n",val3,val3);
    UART_Printf("The result should be : 4.45048e-308\n");
    res2=VFPtest(val3,val3);
    UART_Printf("The result is        : %e\n\n", res2);
    UART_Printf("Bouncing calculation of %e + %e\n", val4, val4);
    res3=VFPtest(val4,val4);
    UART_Printf("The result should be : 6.225227e-322\n");
    UART_Printf("The result is        : %e\n\n", res3);

  

}


void Test_Temp(void)
{
	u32 i;

	for(i=0;i<1000;i++)
		{
			UART_Printf("0x%x : 0x%x     ", (0x7F00E000+i*4), Inp32(0x7F00E000+i*4));
		}
  

}


#if 0
void Test_Bus_Prio_M(void)
{
	u32 uMode,uTime, uTest;

	UART_Printf(" Choose Bus Priority :  0 :Fixed_DMA0/DMA1,  1:Fixed_DMA1/DMA0, 2:Last grant, 3:Rotatio \n");
	uMode = UART_GetIntNum();

  	switch (uMode)
	{
	case 0:
		uTest = Inp32SYSC(0x104);
		uTest = uTest & ~(0xFFFF<<16) |( (0x0<<28)|(0x0<<24)|(0x0<<20)|(0x0<<16));
	      Outp32SYSC(0x104, uTest);
	      UART_Printf("rAHB_CON1: 0x%x \n",  Inp32SYSC(0x104));
	      break;

	case 1:
	     	uTest = Inp32SYSC(0x104);
		uTest = uTest & ~(0xFFFF<<16) |( (0x0<<28)|(0x1<<24)|(0x0<<20)|(0x1<<16));
	      Outp32SYSC(0x104, uTest);
	      UART_Printf("rAHB_CON1: 0x%x \n",  Inp32SYSC(0x104));
	      break;

	case 2:
	     	uTest = Inp32SYSC(0x104);
		uTest = uTest & ~(0xFFFF<<16) |( (0x1<<28)|(0x0<<24)|(0x1<<20)|(0x0<<16));
	      Outp32SYSC(0x104, uTest);
	      UART_Printf("rAHB_CON1: 0x%x \n",  Inp32SYSC(0x104));
	      break;

		case 3:
	     	uTest = Inp32SYSC(0x104);
		uTest = uTest & ~(0xFFFF<<16) |( (0x2<<28)|(0x0<<24)|(0x2<<20)|(0x0<<16));
	      Outp32SYSC(0x104, uTest);
	      UART_Printf("rAHB_CON1: 0x%x \n",  Inp32SYSC(0x104));
	      break;      
	      
	      
  	}
	

     	DMAC_InitCh(DMA0, DMA_ALL, &oDmac_0);
     	DMAC_InitCh(DMA1, DMA_ALL, &oDmac_1);
	INTC_SetVectAddr(NUM_DMA0,  Dma0Done_Test);
	INTC_SetVectAddr(NUM_DMA1,  Dma1Done_Test);
	INTC_Enable(NUM_DMA0);
	INTC_Enable(NUM_DMA1);

	 g_DmaDone0=0;
	 g_DmaDone1=0;
	
	 UART_Printf("DMA Start \n");
	// 1MB
	DMACH_Setup(DMA_A, 0x0, 0x51000000, 0, 0x51001000, 0, WORD, 0x1000000, DEMAND, MEM, MEM, SINGLE, &oDmac_0);
	DMACH_Setup(DMA_A, 0x0, 0x52000000, 0, 0x52001000, 0, WORD, 0x1000000, DEMAND, MEM, MEM, SINGLE, &oDmac_1);
        // Enable DMA

  
       StartTimer(0);
	DMACH_Start(&oDmac_0);
       StartTimer(1);
	DMACH_Start(&oDmac_1);
	
	
	while((g_DmaDone0==0)||(g_DmaDone1==0))							// Int.
		{
			Copy(0x51000000, 0x51800000, 0x1000000);
		}
	
	
}


void Test_Bus_Prio_F(void)
{
	u32 uMode,uTime, uTest;
	u32 uHSz, uVSz;


	UART_Printf(" Choose Bus Priority( F ) :  0 :Fixed_000,  1:Fixed_005, 2:Fixed_006, 3:Last Granted  4:Rotatio \n");
	uMode = UART_GetIntNum();

  	switch (uMode)
	{
	case 0:
		uTest = Inp32SYSC(0x100);
		uTest = uTest & ~(0xFF) |( (0x0<<4)|(0x0<<0));
	      Outp32SYSC(0x100, uTest);
	      UART_Printf("rAHB_CON0: 0x%x \n",  Inp32SYSC(0x100));
	      break;

	case 1:
		uTest = Inp32SYSC(0x100);
		uTest = uTest & ~(0xFF) |( (0x0<<4)|(0x5<<0));
	      Outp32SYSC(0x100, uTest);
	      UART_Printf("rAHB_CON0: 0x%x \n",  Inp32SYSC(0x100));
	      break;
	

	case 2:
		uTest = Inp32SYSC(0x100);
		uTest = uTest & ~(0xFF) |( (0x0<<4)|(0x6<<0));
	      Outp32SYSC(0x100, uTest);
	      UART_Printf("rAHB_CON0: 0x%x \n",  Inp32SYSC(0x100));
	      break;
	           

	case 3:
	     	uTest = Inp32SYSC(0x100);
		uTest = uTest & ~(0xFF) |( (0x1<<4)|(0x0<<0));
	      Outp32SYSC(0x100, uTest);
	      UART_Printf("rAHB_CON0: 0x%x \n",  Inp32SYSC(0x100));
	      break;

		case 4:
	     	uTest = Inp32SYSC(0x100);
		uTest = uTest & ~(0xFF) |( (0x2<<4)|(0x0<<0));
	      Outp32SYSC(0x100, uTest);
	      UART_Printf("rAHB_CON1: 0x%x \n",  Inp32SYSC(0x100));
	      break;      
	      
	      
  	}
	



	
	
}

#endif


///////////////////////////////////////////////////////////////////////////////////
////////////////////                   Power Mode  Test                  /////////////////////////// 
///////////////////////////////////////////////////////////////////////////////////

const testFuncMenu reset_menu[] =
{
	Test_SoftReset,							"Software Reset Test",
	Test_WarmReset,						"Warm Reset Test",
	Test_WDTReset,							"WDT Reset Test",
	Test_VFP,								"VFP Test",
	Test_Temp,								"Test Temp",
//Test_Bus_Prio_M,							"Bus Priority_M",

	0, 0
};

void SYSCT_Reset(void)
{
	u32 i;
	s32 uSel;

	UART_Printf("[Reset Mode Test]\n\n");
	
	while(1)
	{
		for (i=0; (u32)(reset_menu[i].desc)!=0; i++)
			UART_Printf("%2d: %s\n", i, reset_menu[i].desc);

		UART_Printf("\nSelect the function to test : ");
		uSel =UART_GetIntNum();
		UART_Printf("\n");
		if(uSel == -1) 
			break;

		if (uSel>=0 && uSel<(sizeof(reset_menu)/8-1))
			(reset_menu[uSel].func) ();
	}
}	

