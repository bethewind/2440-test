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
*	File Name : clock_test.c
*  
*	File Description : This file implements the test functons for the clock module
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

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "option.h"
#include "library.h"
#include "sfr6410.h"
#include "system.h"
#include "gpio.h"
#include "intc.h"

#include "rtc.h"
#include "timer.h"
//#include "adcts.h"
//#include "lcd.h"
#include "sysc.h"
#include "power_test.h"
#include "clock_test.h"
#include "dmc_library.h"
//#include "dma.h"


extern volatile u32 g_TestCount;

volatile u32         g_RTC_ALM, g_TestErr, g_TestEnd;
//
volatile u32		g_iSeed, g_iNum, g_iARMCLK_DIVN_R, g_iHCLKx2_DIVN_R, g_iPCLK_DIVN_R;
volatile u32		g_iRandom_PMS;
volatile u32		g_iRoop = 0 , g_iRoop1 = 0, g_iRoop2 = 0, g_iRoop3 = 0, g_iRoop4=0, g_iRoop5=0, g_iRoop6=0, g_iRoop7 =0 ;
volatile u32		g_iPvalue, g_iMvalue, g_iSvalue; 
volatile u32		g_iCompare_0 = 0, g_iCompare_1 = 0, g_iSum = 0;
volatile u32		g_iAPLL,  g_iAPLL_0, g_iMPLL;
volatile u32		g_iFCLK;
volatile u32		g_NumPLL=22;
//volatile u32 iDivn[16][16][16] = 0;
volatile u32 iDivn[16][16][16] = {0,};
volatile u32 iCompare_R[32] = { 0x7ff, 0x7ff, 0x7ff, 0x7ff, 0x7ff, 0x7ff, 0x7ff, 0x7ff, 0x7ff, 0x7ff, 0x7ff, 0x7ff, 0x7ff, 0x7ff,0x7ff, 0x7ff,
	                                          0x7ff, 0x7ff, 0x7ff, 0x7ff, 0x7ff, 0x7ff, 0x7ff, 0x7ff, 0x7ff, 0x7ff, 0x7ff, 0x7ff, 0x7ff, 0x7ff, 0x7ff, 0x7ff};
/*
volatile u32 iCompare_R[32] = { 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 
                                                  0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,0xffffffff, 0xffffffff,
                                                  0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,0xffffffff, 0xffffffff,
                                                  0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,0xffffffff, 0xffffffff};
*/
/*
volatile u32 iCompare_PMS[18] = 
{
	0x3ffff, 0x3ffff, 0x3ffff, 0x3ffff, 0x3ffff, 0x3ffff, 0x3ffff, 0x3ffff, 0x3ffff, 0x3ffff, 0x3ffff, 0x3ffff, 0x3ffff, 0x3ffff, 0x3ffff, 0x3ffff, 0x3ffff, 0x3ffff

};
*/
volatile u32 iCompare_PMS[27] = 
{
	0x7ffffff, 0x7ffffff, 0x7ffffff, 0x7ffffff, 0x7ffffff, 0x7ffffff, 0x7ffffff, 0x7ffffff, 0x7ffffff, 
	0x7ffffff, 0x7ffffff, 0x7ffffff, 0x7ffffff, 0x7ffffff, 0x7ffffff, 0x7ffffff, 0x7ffffff, 0x7ffffff, 
	0x7ffffff, 0x7ffffff, 0x7ffffff, 0x7ffffff, 0x7ffffff, 0x7ffffff, 0x7ffffff, 0x7ffffff, 0x7ffffff 
};



//////////
// Function Name : MemoryRWTest
// Function Description : This function test Read and Write to memory area
//					 
// Input : 	
// Output : 	
//			
// Version : v0.1
 void MemoryRWTest(void)
{
  	//int i;
 	u32 data;
    	u32 memError=0;
    	unsigned int *pt;
  	volatile u32 _Test_Start_Address;
 	volatile u32 _Test_End_Address;    
    	//
    	// memory test
    	//
    
    	_Test_Start_Address = _DRAM_BaseAddress + 0x3000000 ;
    	_Test_End_Address   = _DRAM_BaseAddress + 0x3010000;
	
    
    	UART_Printf("Memory Test(%xh-%xh):WR",_Test_Start_Address,_Test_End_Address);
    
    	pt=(unsigned int *)_Test_Start_Address;
    	while((unsigned int)pt<_Test_End_Address)
    	{
       	  //*pt=(unsigned int)pt;
         	//data = (unsigned int)pt+((unsigned int)pt<<17);
         	data = (unsigned int)pt;
         	*pt=data;
         	//*pt=0xaaaaaaaa;
         	pt++;
    	}
     
    	UART_Printf("\b\bRD");
    	pt=(unsigned int *)_Test_Start_Address;
            
    	while((unsigned int)pt<_Test_End_Address)
    	{
       	data=*pt;
      		if(data!=(unsigned int)pt)
      		//if(data != 0xaaaaaaaa)
      		{
              		memError=1;
             
          		//    UART_Printf("\b\bFAIL:0x%x=0x%x\n",i,data);
              		break;
      		}
         		*pt=0xaaaaaaaa;
         		pt++;
    	}
     
     		if(memError==0)UART_Printf("\b\bO.K.\n");

}



//////////
// Function Name : MemoryRWTest
// Function Description : This function test Read and Write to memory area
//					 
// Input : 	
// Output : 	
//			
// Version : v0.1
 u8 MemoryRWTest1(u32 a0, u32 a1)
{
  	//int i;
 	u32 data;
	u32 memError=0;
	unsigned int *pt;
  	volatile u32 _Test_Start_Address;
 	volatile u32 _Test_End_Address;    
	//
	// memory test
	//

	_Test_Start_Address = a0 ;
	_Test_End_Address   = a1 ;


	//UART_Printf("Memory Test(%xh-%xh):WR",_Test_Start_Address,_Test_End_Address);

	pt=(unsigned int *)_Test_Start_Address;
	// init Data
	while((unsigned int)pt<_Test_End_Address)
	{
		data = 0;
		*pt=data;
		pt++;
	}

	pt=(unsigned int *)_Test_Start_Address;
	while((unsigned int)pt<_Test_End_Address)
	{
		data = (unsigned int)pt;
		*pt=data;
		pt++;
	}
     
	pt=(unsigned int *)_Test_Start_Address;

	while((unsigned int)pt<_Test_End_Address)
	{
		data=*pt;
		if(data!=(unsigned int)pt) {
			memError = false;
			return memError;
		}
		pt++;
	}
	return memError;

}


#if 0

//////////
// File Name : Test_Locktime
// File Description : This function is for the PLL Lock Time Test
//				  APLL&MPLL = 100us,  EPLL = 300us
// Input : 				
//
//						
// Output : NONE
// Version : 

void Test_Locktime(void)
{
	u32 uPLLType;
	u32 uLockTime =300;
	u32 uTestCount;
	u32 uCONReg;
	u32 uTestType, uLockTime0;
	u32 uLockDetect0, uLockDetect1;

	SYSC_ChangeMode(eASYNC_MODE);
	//SYSC_ChangeSYSCLK_1(eAPLL667M, eAPLL266M ,  0, 0, 3);			// 667MHz, 1:4:8, ASync.. HCLKx2 = 0 
	SYSC_ChangeSYSCLK(eAPLL532M, eAPLL266M ,  0, 0, 3);
	SYSC_GetClkInform();
	UART_InitDebugCh(0, 115200);
	
	UART_Printf("[Lock time test ]\n");
       UART_Printf("\nSelect PLL : 0:APLL, 1:MPLL, 2:EPLL	: ");

      uPLLType = UART_GetIntNum();
	
       UART_Printf("Lock time range:APLL, MPLL(100us), (EPLL)300us\n");
     	UART_Printf("Input Lock time[us]: ");
     	
	uLockTime = UART_GetIntNum();
	// CLK Output PAD Enable
	GPIO_SetFunctionEach(eGPIO_F, eGPIO_14, 3);

	
       switch(uPLLType)
	{
		case 0: 
			UART_Printf("[APLL Lock time test 100,000 times]\n");
			UART_Printf("         Change Sync. Mode\n");
			SYSC_ChangeMode(eSYNC_MODE);
			SYSC_GetClkInform();
			UART_InitDebugCh(0, 115200);
			SYSC_SetLockTime( eAPLL, uLockTime);
			SYSC_CtrlCLKOUT(eCLKOUT_HCLK, 9);
			uLockTime0 = Inp32SYSC(0x0);
			break;

		case 1: 
			UART_Printf("[MPLL Lock time test 100,000 times]\n");
			SYSC_ChangeMode(eASYNC_MODE);
			SYSC_GetClkInform();
			UART_InitDebugCh(0, 115200);
			SYSC_SetLockTime( eMPLL, uLockTime);
			SYSC_CtrlCLKOUT(eCLKOUT_HCLK, 9);
			uLockTime0 = Inp32SYSC(0x4);
			break;

		case 2: 
			UART_Printf("[EPLL Lock time test 100,000 times]\n");
			SYSC_SetLockTime( eEPLL, uLockTime);
			SYSC_CtrlCLKOUT(eCLKOUT_EPLLOUT, 9);
			uLockTime0 = Inp32SYSC(0x8);
			break;
		}


      UART_Printf("\nSelect Test Type : 0:Check Lock Time, 1:for Change PLL Test	: ");
      uTestType = UART_GetIntNum();

	if(uTestType == 0)
	{
		UART_Printf("[Check Lock Time - Press any Key]\n");
			
		GPIO_SetPullUpDownEach(eGPIO_N, eGPIO_9,0); 	// Pull Down Disable
		GPIO_SetFunctionEach(eGPIO_N, eGPIO_9, 1);		// Output Pad
		//GPIO_SetDataEach(eGPIO_N, eGPIO_9,1);
		Outp32(0x7f008834, 0xFFF);

		GPIO_SetPullUpDownEach(eGPIO_F, eGPIO_15, 0);
		GPIO_SetFunctionEach(eGPIO_F,eGPIO_15,1);
		GPIO_SetDataEach(eGPIO_F,eGPIO_15,1);
		
		while(!UART_GetKey());	

	
		
		//GPIO_SetDataEach(eGPIO_N, eGPIO_9,0);
		//Outp32(0x7f008834, 0x0);
		GPIO_SetDataEach(eGPIO_F,eGPIO_15,0);
		//Delay(10);
		//SYSC_SetPLL(eMPLL, 400, 3, 3,0);			// EVT0
		SYSC_SetPLL(eAPLL, 400, 3, 2,0);		      // 500MHz for PLL Change test  => fail (from 400,3,2)
		//SYSC_SetPLL(eMPLL, 400, 3, 3,0);
		Delay(10);
		GPIO_SetDataEach(eGPIO_F,eGPIO_15,1);
		Outp32(0x7f008834, 0xFFF);
	//	GPIO_SetDataEach(eGPIO_N, eGPIO_9,1);
	//	Outp32SYSC(0xC,0x82140601);				//532MHz,  for Test 07.02.14
	//	Outp32SYSC(0xC,0x81900302);

		//Outp32SYSC(0x1C, 0x3);					// Clock Source Change => Fail....

		//while(!UART_GetKey());				// for Test 07.02.14
		SYSC_GetClkInform();
		UART_InitDebugCh(0, 115200);
		
		uLockDetect0 = SYSC_RdLockDetect(eAPLL);
		uLockDetect1= SYSC_RdLockDetect(eMPLL);
  
	UART_Printf("\n\n");
	UART_Printf("***************************************************\n");
	UART_Printf("ARMCLK: %.2fMHz  HCLKx2: %.2fMHz  HCLK: %.2fMHz  PCLK: %.2fMHz\n",(float)g_ARMCLK/1.0e6, (float)g_HCLKx2/1.0e6, (float)g_HCLK/1.0e6, (float)g_PCLK/1.0e6);
	UART_Printf("APLL Lock :  %d,  MPLL_Lock: %d\n", uLockDetect0, uLockDetect1);
	UART_Printf("\n");    
       UART_Printf(" Press any Key\n");
		while(!UART_GetKey());	
		
		//GPIO_SetDataEach(eGPIO_N, eGPIO_9,0);
		//Outp32(0x7f008834, 0x0);
		GPIO_SetDataEach(eGPIO_F,eGPIO_15,0);
		//Delay(1000);
		//SYSC_SetPLL(eMPLL, 400, 3, 3,0);
		SYSC_SetPLL(eAPLL, 532, 3, 2, 0);		      // EVT1
		//SYSC_SetPLL(eMPLL, 400, 3, 3,0);			// for test
		Delay(10);
		GPIO_SetDataEach(eGPIO_F,eGPIO_15,1);
		
		Outp32(0x7f008834, 0xFFF);
	//	GPIO_SetDataEach(eGPIO_N, eGPIO_9,1);
	//	Outp32SYSC(0xC,0x82140601);				//532MHz,  for Test 07.02.14
	//	Outp32SYSC(0xC,0x81900302);

		//Outp32SYSC(0x1C, 0x3);					// Clock Source Change => Fail....

		//while(!UART_GetKey());				// for Test 07.02.14
		//SYSC_GetClkInform();
		UART_InitDebugCh(0, 115200);
		
		uLockDetect0 = SYSC_RdLockDetect(eAPLL);
		uLockDetect1= SYSC_RdLockDetect(eMPLL);
  
	UART_Printf("\n\n");
	UART_Printf("***************************************************\n");
	UART_Printf("*       S3C6410 - Test firmware v0.1              *\n");
	UART_Printf("***************************************************\n");
	UART_Printf("System ID : Revision [%d], Pass [%d]\n", g_System_Revision, g_System_Pass);
	UART_Printf("ARMCLK: %.2fMHz  HCLKx2: %.2fMHz  HCLK: %.2fMHz  PCLK: %.2fMHz\n",(float)g_ARMCLK/1.0e6, (float)g_HCLKx2/1.0e6, (float)g_HCLK/1.0e6, (float)g_PCLK/1.0e6);
	UART_Printf("APLL Lock :  %d,  MPLL_Lock: %d\n", uLockDetect0, uLockDetect1);
	UART_Printf("\n");    
	

		
	}
	else if(uTestType == 1)
	{
		UART_Printf("[Lock time test 100,000 times]\n");
	while(1) 
	{
		if(UART_GetKey()!=0x0) 
			break;
		if(uTestCount>100000) 
			break;
		if ((uTestCount%1000)==0)
		{

			DisplayLED(uTestCount%1000);
			if(uPLLType == 0)
				{ uCONReg = Inp32SYSC(0xC);}
			else if(uPLLType == 1)
				{uCONReg = Inp32SYSC(0x10);}
			else if(uPLLType == 2)
				{uCONReg = Inp32SYSC(0x14);}
			
			SYSC_GetClkInform();
			UART_InitDebugCh(0, 115200);
			UART_Printf("\n%d, PLL CON Reg: 0x%x\n", uTestCount, uCONReg);

		}
		if(uTestCount%2) 
		{
			//DisplayLED(0x8);

			if(uPLLType == 0 ) 
			{
				SYSC_SetPLL(eAPLL, 400, 3, 2, 0);	 	// APLL => 400MHz
			}
			else if (uPLLType == 1)
			{
				SYSC_SetPLL(eMPLL, 800, 6, 3, 0);		// MPLL => 200MHz
			}

			else if (uPLLType == 2 )
			{
				SYSC_SetPLL(eEPLL, 32, 1, 2, 0);		// EPLL => 96MHz
			}
			
			Delay(10);

			
			
		} 
		else 
		{		
			//DisplayLED(0x3);
			if(uPLLType == 0 ) 
			{
				SYSC_SetPLL(eAPLL, 532, 3, 2, 0);	 	// APLL => 532MHz, EVT1
			}
			else if (uPLLType == 1)
			{
				SYSC_SetPLL(eMPLL, 532, 3, 3, 0);		// MPLL => 266MHz, EVT1
			}

			else if (uPLLType == 2 )
			{
				SYSC_SetPLL(eEPLL, 100, 3, 2, 0);		// EPLL => 100MHz
			}

			Delay(10);
			
		}
		
		uTestCount++;
	}	
	}
	SYSC_ChangeMode(eASYNC_MODE);
	SYSC_GetClkInform();
	UART_InitDebugCh(0, 115200);
	Delay(10);

	
	UART_Printf("Lock Time : 0x%x\n", uLockTime0 );
	UART_Printf("[PLL Lock time test %d times end]\n",uTestCount-1);

	SYSC_ChangeMode(eASYNC_MODE);
	//SYSC_ChangeSYSCLK_1(eAPLL667M, eAPLL266M ,  0, 0, 3);			// 667MHz, 1:4:8, ASync.. HCLKx2 = 0 
	SYSC_ChangeSYSCLK(eAPLL532M, eAPLL266M ,  0, 0, 3);
	SYSC_GetClkInform();
	UART_InitDebugCh(0, 115200);

	// Default Value로 변경 추가 ? 
	
}

#endif

//////////
// File Name : Test_Locktime
// File Description : This function is for the PLL Lock Time Test
//				  APLL&MPLL = 100us,  EPLL = 300us
// Input : 				
//
//						
// Output : NONE
// Version : 0.1 by boaz

void Test_Locktime(void)
{
	u32 uPLLType;
	u32 uLockTime =300;
	u32 uTestCount;
	u32 uCONReg;
	u32 uTestType, uLockTime0;
	u32 uLockDetect0, uLockDetect1;

	SYSC_ChangeMode(eASYNC_MODE);
	//SYSC_ChangeSYSCLK_1(eAPLL667M, eAPLL266M ,  0, 0, 3);			// 667MHz, 1:4:8, ASync.. HCLKx2 = 0 
	SYSC_ChangeSYSCLK(eAPLL532M, eAPLL266M ,  0, 0, 3);
	SYSC_GetClkInform();
	UART_InitDebugCh(0, 115200);
	
	UART_Printf("[Lock time test ]\n");
       UART_Printf("\nSelect PLL : 0:APLL, 1:MPLL, 2:EPLL	: ");

      uPLLType = UART_GetIntNum();
	
       UART_Printf("Lock time range:APLL, MPLL(100us), (EPLL)300us\n");
     	UART_Printf("Input Lock time[us]: ");
     	
	uLockTime = UART_GetIntNum();
	// CLK Output PAD Enable
	GPIO_SetFunctionEach(eGPIO_F, eGPIO_14, 3);

	
       switch(uPLLType)
	{
		case 0: 
			UART_Printf("[APLL Lock time test 100,000 times]\n");
			UART_Printf("         Change Sync. Mode\n");
			SYSC_ChangeMode(eSYNC_MODE);
			SYSC_GetClkInform();
			UART_InitDebugCh(0, 115200);
			SYSC_SetLockTime( eAPLL, uLockTime);
			SYSC_CtrlCLKOUT(eCLKOUT_HCLK, 1);
			uLockTime0 = Inp32SYSC(0x0);
			break;

		case 1: 
			UART_Printf("[MPLL Lock time test 100,000 times]\n");
			SYSC_ChangeMode(eASYNC_MODE);
			SYSC_GetClkInform();
			UART_InitDebugCh(0, 115200);
			SYSC_SetLockTime( eMPLL, uLockTime);
			SYSC_CtrlCLKOUT(eCLKOUT_HCLK, 1);
			uLockTime0 = Inp32SYSC(0x4);
			break;

		case 2: 
			UART_Printf("[EPLL Lock time test 100,000 times]\n");
			SYSC_SetLockTime( eEPLL, uLockTime);
			SYSC_ClkSrc(eEPLL_FOUT);
			SYSC_ClkSrc(eMMC0_MOUTEPLL);
			SYSC_SetDIV(eMMC0_RATIO, 1);
			SYSC_SDHC_Clock_On(0);
			SYSC_CtrlCLKOUT(eCLKOUT_EPLLOUT, 1);
			uLockTime0 = Inp32SYSC(0x8);
			break;
	}


	uTestCount=0;
	UART_Printf("[Lock time test 100,000 times]\n");
	while(1) 
	{
		if(UART_GetKey()) 
			break;
		if(uTestCount>100000) 
			break;
		if ((uTestCount%1000)==0)
		{
			DisplayLED(uTestCount%1000);
			if(uPLLType == 0) uCONReg = Inp32SYSC(0xC);
			else if(uPLLType == 1) uCONReg = Inp32SYSC(0x10);
			else if(uPLLType == 2) uCONReg = Inp32SYSC(0x14);
			
			SYSC_GetClkInform();
			UART_InitDebugCh(0, 115200);
			UART_Printf("\n%d, PLL CON Reg: 0x%x\n", uTestCount, uCONReg);

		}
		if(uTestCount%2) 
		{
			if(uPLLType == 0 ) 
			{
				SYSC_SetPLL(eAPLL, 400, 3, 2, 0);	 	// APLL => 400MHz
			}
			else if (uPLLType == 1)
			{
				SYSC_SetPLL(eMPLL, 400, 3, 3, 0);		// MPLL => 200MHz
			}
			else if (uPLLType == 2 )
			{
				SYSC_SetPLL(eEPLL, 32, 1, 2, 0);		// EPLL => 96MHz
			}			
			Delay(10);			
		} 
		else 
		{		
			//DisplayLED(0x3);
			if(uPLLType == 0 ) 
			{
				SYSC_SetPLL(eAPLL, 266, 3, 1, 0);	 	// APLL => 532MHz, EVT1
			}
			else if (uPLLType == 1)
			{
				SYSC_SetPLL(eMPLL, 266, 3, 2, 0);		// MPLL => 266MHz, EVT1
			}
			else if (uPLLType == 2 )
			{
				SYSC_SetPLL(eEPLL, 32, 1, 3, 0);		// EPLL => 48MHz
			}
			Delay(10);			
		}		
		uTestCount++;

	}
	SYSC_ChangeMode(eASYNC_MODE);
	SYSC_GetClkInform();
	UART_InitDebugCh(0, 115200);
	Delay(10);

	
	UART_Printf("Lock Time : 0x%x\n", uLockTime0 );
	UART_Printf("[PLL Lock time test %d times end]\n",uTestCount-1);

	SYSC_ChangeMode(eASYNC_MODE);
	//SYSC_ChangeSYSCLK_1(eAPLL667M, eAPLL266M ,  0, 0, 3);			// 667MHz, 1:4:8, ASync.. HCLKx2 = 0 
	SYSC_ChangeSYSCLK(eAPLL532M, eAPLL266M ,  0, 0, 3);
	SYSC_GetClkInform();
	UART_InitDebugCh(0, 115200);
	// Default Value로 변경 추가 ? 
	
}


//////////
// File Name : Test_ManualChangePLL
// File Description : This function is for the PLL operating 
//				 
// Input : 			
//
//						
// Output : NONE
// Version : 
void Test_ManualChangePLL(void)  
{

	u32 uPLLType,   uRegValue0_bk, uRegValue1_bk;
	u32 uPval, uMval, uSval, uKval=0;
	u32 i;
	u32 uWA_EVT0, uFreq;
		
	UART_Printf("[PLL Manual Change - This is only for test..]\n");
	// Current Clock Information
	SYSC_GetClkInform();
	UART_Printf("Current Clock Information \n");
	UART_Printf("ARMCLK: %.2fMHz  HCLKx2: %.2fMHz  HCLK: %.2fMHz  PCLK: %.2fMHz\n",(float)g_ARMCLK/1.0e6, (float)g_HCLKx2/1.0e6, (float)g_HCLK/1.0e6, (float)g_PCLK/1.0e6);
	UART_Printf("============================ \n");
       UART_Printf("\nSelect PLL : 0:APLL, 1:MPLL, 2:EPLL	     : ");
       uPLLType = UART_GetIntNum();

	UART_Printf("\nEnter the  Mdiv Value( A&MPLL: 56~1023,  EPLL: 13~255) :  ");
       uMval = UART_GetIntNum();

	UART_Printf("\nEnter the  Pdiv Value( 1~63) :  ");
       uPval = UART_GetIntNum();
       
	UART_Printf("\nEnter the  Sdiv Value( 0~5) :  ");
       uSval = UART_GetIntNum();

       if(uPLLType == 2)
      	{
		UART_Printf("\nEnter the  Kdiv Value( 0 ~ 65535) :  ");
       	uKval = UART_GetIntNum();
      	
      	}
       
	// CLK Output PAD Enable
	

	switch(uPLLType)
	{
		case 0: 
			uFreq = (((FIN>>uSval)/uPval)*uMval)/1.0e6;
			
			UART_Printf("[Selected APLL: %.2fMHz]\n", (float)uFreq);
			
			// Clock Output Pad is selected APLLOUT
			SYSC_CtrlCLKOUT(eCLKOUT_APLLOUT, 0);
			uRegValue0_bk = Inp32SYSC(0xC);	// APLL_CON
			uWA_EVT0=Inp32SYSC(0x10);		// MPLL_CON
			Outp32SYSC(0x10, uWA_EVT0);			// W.A : PLL Change 07.02.14
			SYSC_SetPLL(eAPLL, uMval, uPval, uSval, uKval);
			
			Delay(100);
			break;

		case 1: 
			UART_Printf("Selected MPLL & Selected Async Mode\n");
			SYSC_ChangeMode(eASYNC_MODE);
			// Clock Output pad is selected HCLK		
			SYSC_CtrlCLKOUT(eCLKOUT_HCLK, 0);
			uRegValue0_bk = Inp32SYSC(0x10);	//MPLL_CON

			SYSC_SetPLL(eMPLL, uMval, uPval, uSval, uKval);
			Delay(100);
			break;

		case 2: 
			UART_Printf("[Selected APLL]\n");
			// Clock Output Pad is selected APLLOUT
			SYSC_CtrlCLKOUT(eCLKOUT_EPLLOUT, 0);
			uRegValue0_bk = Inp32SYSC(0x14);	// EPLL_CON0
			uRegValue1_bk = Inp32SYSC(0x18);	// EPLL_CON1
				
			SYSC_SetPLL(eEPLL, uMval, uPval, uSval, uKval);
			Delay(100);
			break;
	}

	
	SYSC_GetClkInform();
	UART_InitDebugCh(0, 115200);
	Delay(10);
	UART_Printf("Current Clock Information \n");
	UART_Printf("ARMCLK: %.2fMHz  HCLKx2: %.2fMHz  HCLK: %.2fMHz  PCLK: %.2fMHz\n",(float)g_ARMCLK/1.0e6, (float)g_HCLKx2/1.0e6, (float)g_HCLK/1.0e6, (float)g_PCLK/1.0e6);
	UART_Printf("============================ \n");

	

	UART_Printf("press 'q' key to exit\n");	
	while(UART_GetKey()!='q')
	{
		DisplayLED(3);
		for(i=0;i<3000000;i++);
		DisplayLED(9);
		for(i=0;i<3000000;i++);
	}

	// Return the pre test value
	if(uPLLType == 0 ) 
	{
		Outp32SYSC(0x10, uWA_EVT0);					// W.A PLL Change 07.02.14
		Outp32SYSC(0xC, uRegValue0_bk);	 

		SYSC_GetClkInform();
		UART_InitDebugCh(0, 115200);
	}
	else if (uPLLType == 1)
	{
		Outp32SYSC(0x10, uRegValue0_bk);	
		Delay(10);
		SYSC_GetClkInform();
		UART_InitDebugCh(0, 115200);
	}
	else if (uPLLType == 2 )
	{
		Outp32SYSC(0x18, uRegValue1_bk);
		Outp32SYSC(0x14, uRegValue0_bk);
	}
	
	
	
}



//////////
// File Name : Test_OnOffPll
// File Description : This function is for the PLL On/OFF
//				 
// Input : 			
//
//						
// Output : NONE
// Version : 
#if 0
void Test_OnOffPll(void)
{
	u32 uOPType,   uRegValue0_bk, uRegValue1_bk, uRegValue2_bk, uRegValue3_bk;
	u32 uLockTime;
	u32 i,j, uTemp, uTemp1, uTestCount;


	UART_Printf("[PLL On/Off test, 10000 times]\n");

	//Choose Operating Mode.
	UART_Printf("\nChoose System Operating Mode : 0:Sync Mode, 1:Async Mode	     : ");
       uOPType = UART_GetIntNum();

	//CLK_OUT Pad => HCLK
	SYSC_CtrlCLKOUT(eCLKOUT_HCLK, 9);

	uLockTime = 100;	
	SYSC_SetLockTime(eAPLL, uLockTime);
	SYSC_SetLockTime(eMPLL, uLockTime);
	// CLK Output PAD Enable
	GPIO_SetFunctionEach(eGPIO_F, eGPIO_14, 3);

	switch(uOPType)
	{
		case 0: 
			UART_Printf("[Selected Sync. Mode Operation]\n");
			SYSC_ChangeMode(eSYNC_MODE);

			uRegValue0_bk = Inp32SYSC(0xC);			// APLL_CON
			uRegValue1_bk = Inp32SYSC(0x10);			// MPLL CON
			uRegValue2_bk = Inp32SYSC(0x1C);			// Clock Source Register
			uRegValue3_bk = Inp32SYSC(0x20);			// Clock Divider value
			
			break;

		case 1: 
			UART_Printf("Selected Async Mode Operation\n");
			//SYSC_ChangeMode(eASYNC_MODE);

			uRegValue0_bk = Inp32SYSC(0xC);			// APLL_CON
			uRegValue1_bk = Inp32SYSC(0x10);			// MPLL CON
			uRegValue2_bk = Inp32SYSC(0x1C);			//Clock Source Register
			uRegValue3_bk = Inp32SYSC(0x20);			// Clock Divider value
			break;

	}


	uTestCount = 0;

	while(1) 
		{
		
		if(UART_GetKey()!=0x0) 
			break;
		if(uTestCount>100000) 
			break;

//		if ((uTestCount%100)==0)
		if ((uTestCount%10)==0)
		{
			DisplayLED(uTestCount/100);
			SYSC_GetClkInform();		
			UART_InitDebugCh(0, 115200);
			Delay(10);
			UART_Printf("\n%d \n", uTestCount);

		}
			
	// ON&OFF Test
	//for(i=0;i<10000;i++) {
		//isplayLED(0x2);

				
		uTemp = Inp32SYSC(0x20);
		uTemp = (1<<12)|(0<<9)|(1<<8)|(1<<4)|(0<<0);		// ARM:HCLKx2:HCLK:PCLK = 1:1:2:2
					
		uTemp1 = Inp32SYSC(0x1C);			//Clock Source Register
		uTemp1 = uTemp1 & ~(0x7) | 0x0;

		Outp32SYSC(0x1C, uTemp1);			// Clock Source Change	
		Outp32SYSC(0x20, uTemp);			// Change Divide Value
		SYSC_StopPLL(eAPLL);
		SYSC_StopPLL(eMPLL);


		Delay(10);

		//DisplayLED(0x8);
		SYSC_SetPLL(eAPLL, 400, 3, 2, 0);	 	// APLL => 400MHz
		SYSC_SetPLL(eMPLL, 800, 6, 3, 0);		// MPLL => 200MHz
		Delay(100);
		
		Outp32SYSC(0x20, uRegValue3_bk);			// Change Divide Value
		Outp32SYSC(0x1C, uRegValue2_bk);			// Clock Source Change	

		GPIO_SetFunctionEach(eGPIO_F, eGPIO_14, 3);

		SYSC_GetClkInform();		
		UART_InitDebugCh(0, 115200);		
		MemoryRWTest();
		
		uTestCount++;
		Delay(100);

		GPIO_SetFunctionEach(eGPIO_F, eGPIO_14, 1);
  		
	}



	Outp32SYSC(0xC, uRegValue0_bk);
	Outp32SYSC(0x10, uRegValue1_bk);
	Outp32SYSC(0x20, uRegValue3_bk);
	Outp32SYSC(0x1C, uRegValue2_bk);
	
	
}

#endif

void __irq Isr_RTC_Tick1(void)
{
	//-daedoo
	//RTC_ClearIntP(0);
	RTC_ClearPending(TimeTic);
	g_RTC_ALM=1;
	

	INTC_ClearVectAddr();
}


void Test_OnOffPll(void)
{
	u32 uOPType,   uRegValue0_bk, uRegValue1_bk, uRegValue2_bk, uRegValue3_bk;
	u32 uLockTime;
	u32 uTemp, uTemp1, uTestCount;
	u32 uTestTime, uTestErr;


	UART_Printf("[PLL On/Off test, 10000 times]\n");

	//Choose Operating Mode.
	UART_Printf("\nChoose System Operating Mode : 0:Sync Mode, 1:Async Mode	     : ");
       uOPType = UART_GetIntNum();

	//CLK_OUT Pad => HCLK
	SYSC_CtrlCLKOUT(eCLKOUT_APLLOUT, 9);
	

	uLockTime = 100;	
	SYSC_SetLockTime(eAPLL, uLockTime);
	SYSC_SetLockTime(eMPLL, uLockTime);
	// Test Point....
	GPIO_SetFunctionEach(eGPIO_N, eGPIO_9, 1);	// EINT9 : Output pad

	switch(uOPType)
	{
		case 0: 
			UART_Printf("[Selected Sync. Mode Operation]\n");
			SYSC_ChangeMode(eSYNC_MODE);

			uRegValue0_bk = Inp32SYSC(0xC);			// APLL_CON
			uRegValue1_bk = Inp32SYSC(0x10);			// MPLL CON
			uRegValue2_bk = Inp32SYSC(0x1C);			// Clock Source Register
			uRegValue3_bk = Inp32SYSC(0x20);			// Clock Divider value
			
			break;

		case 1: 
			UART_Printf("Selected Async Mode Operation\n");
			//SYSC_ChangeMode(eASYNC_MODE);

			uRegValue0_bk = Inp32SYSC(0xC);			// APLL_CON
			uRegValue1_bk = Inp32SYSC(0x10);			// MPLL CON
			uRegValue2_bk = Inp32SYSC(0x1C);			//Clock Source Register
			uRegValue3_bk = Inp32SYSC(0x20);			// Clock Divider value
			break;

	}


	uTestCount = 0;
	uTestErr=0;
	INTC_SetVectAddr(NUM_RTC_TIC,Isr_RTC_Tick1);
	INTC_Enable(NUM_RTC_TIC);

	uTemp = Inp32SYSC(0x20);
	uTemp = (1<<12)|(0<<9)|(1<<8)|(1<<4)|(0<<0);		// ARM:HCLKx2:HCLK:PCLK = 1:1:2:2
					
	uTemp1 = Inp32SYSC(0x1C);			//Clock Source Register
	uTemp1 = uTemp1 & ~(0x7) | 0x0;

	
	while(1) 
		{
		
		if(UART_GetKey()!=0x0) 
			break;
		if(uTestCount>100000) 
		{
			UART_Printf("PLL ON/OFF Test - Test Count: %d,  Error: %d\n", uTestCount, uTestErr);
			break;
		}
		if(uTestErr>10000)
		{
			UART_Printf("PLL ON/OFF Fail - Test Count: %d,  Error: %d\n", uTestCount, uTestErr);
			break;	
		}
//		if ((uTestCount%100)==0)
		if ((uTestCount%100)==0)
		{
			DisplayLED(uTestCount/100);
			SYSC_GetClkInform();		
			UART_InitDebugCh(0, 115200);
			Delay(10);
			UART_Printf("Test Count : %d,  Error: %d \n", uTestCount, uTestErr);
			UART_TxEmpty();

		}
			
	// ON&OFF Test
	//for(i=0;i<10000;i++) {
		//isplayLED(0x2);

		g_RTC_ALM =0;		


		Outp32SYSC(0x1C, uTemp1);			// Clock Source Change	
		Outp32SYSC(0x20, uTemp);			// Change Divide Value
		SYSC_StopPLL(eAPLL);
		//Outp32SYSC(0xC, 0x0);				// Temp Test
		//SYSC_StopPLL(eMPLL);
		//GPIO_SetDataEach(eGPIO_N, eGPIO_9, 0);
		
		//UART_Getc();
											// No Delay ~= 0.2ms
		Delay(10);							// Stop Time ~= 34ms
		//Delay(1);							// Stop Time ~=3.5ms
		//Delay(200);							//  Stop Time ~= 667ms
		//Delay(400);
		GPIO_SetDataEach(eGPIO_N, eGPIO_9, 0);
		//DisplayLED(0x8);
		//SYSC_SetPLL(eMPLL, 800, 6, 3, 0);		// MPLL => 200MHz
		//SYSC_SetPLL(eAPLL, 400, 3, 2, 0);	 	// APLL => 400MHz	, 1600MHz
		//SYSC_SetPLL(eAPLL, 200, 3, 1, 0);	 	// APLL => 400MHz	, VCO 800MHz
		//SYSC_SetPLL(eAPLL, 400, 2, 3, 0 );		// APLL => 300MHz, VCO 2400MHz
		SYSC_SetPLL(eAPLL, 200, 3, 1, 0 );		// APLL => 300MHz, VCO 2400MHz
		//SYSC_SetPLL(eAPLL, 300, 6, 1, 0);	 	// APLL => 300MHz	, VCO 600MHz
		//SYSC_SetPLL(eAPLL, 800, 6, 2, 0);	 	// APLL => 400MHz
		//SYSC_SetPLL(eAPLL, 144, 1, 2, 0);	 	// APLL => 432MHz
		//SYSC_SetPLL(eAPLL, 284, 2, 2, 0);	 	// APLL => 426MHz, 1704		=> OK??
		//SYSC_SetPLL(eAPLL, 288, 2, 2, 0);	 	// APLL => 432MHz,		1728    => 
		//SYSC_SetPLL(eAPLL, 170, 2, 2, 0);	 	// APLL => 255MHz, VCO=1020MHz	=> OK??
		//SYSC_SetPLL(eAPLL, 230, 2, 2, 0);	 	// APLL => 345MHz, VCO 1380MHz		=> OK??
		//SYSC_SetPLL(eAPLL, 400, 3, 5, 0);	 	// APLL => 200MHz	, 1600MHz
		//SYSC_SetPLL(eAPLL, 504, 6, 5, 0);	 	// APLL => 200MHz	, 1600MHz
		//SYSC_SetPLL(eAPLL, 1000, 10, 2, 0);		// APLL => 300MHz
		/////////////////////////// Test
		//SYSC_SetPLL(eAPLL, 136, 1, 2, 0);

		//SYSC_SetPLL(eAPLL, 400, 1, 4, 0);		// 4800MHz, Fout 300
		//SYSC_SetPLL(eAPLL, 400, 2, 3, 0);		// 2400MHz, Fout 300
		//SYSC_SetPLL(eAPLL, 400, 3, 2, 0);		// 1600MHz, Fout 300
		//SYSC_SetPLL(eAPLL, 400, 4, 2, 0);		// 1200MHz, Fout 300
		//SYSC_SetPLL(eAPLL, 400, 5, 1, 0);		// 1200MHz, Fout 300
		//SYSC_SetPLL(eAPLL, 400, 6, 1, 0);		// 800MHz, Fout 300
		//SYSC_SetPLL(eAPLL, 400, 7, 1, 0);		// 685MHz, Fout 342
		
		///////////////////////  P Divider 6
		//SYSC_SetPLL(eAPLL, 300, 2, 1, 0);		// 800MHz
		//SYSC_SetPLL(eAPLL, 402, 2, 1, 0);		// 800MHz
		//SYSC_SetPLL(eAPLL, 404, 2, 1, 0);		// 800MHz
		//SYSC_SetPLL(eAPLL, 406, 2, 1, 0);		// 800MHz
		//SYSC_SetPLL(eAPLL, 408, 2, 1, 0);		// 800MHz
		//SYSC_SetPLL(eAPLL, 410, 2, 1, 0);		// 800MHz
		//SYSC_SetPLL(eAPLL, 412, 2, 1, 0);		// 800MHz
		//SYSC_SetPLL(eAPLL, 416, 2, 1, 0);		// 800MHz

		

		///////////////////////  P Divider 6
		//SYSC_SetPLL(eAPLL, 400, 6, 1, 0);		// 800MHz
		//SYSC_SetPLL(eAPLL, 402, 6, 1, 0);		// 800MHz
		//SYSC_SetPLL(eAPLL, 404, 6, 1, 0);		// 800MHz
		//SYSC_SetPLL(eAPLL, 406, 6, 1, 0);		// 800MHz
		//SYSC_SetPLL(eAPLL, 408, 6, 1, 0);		// 800MHz
		//SYSC_SetPLL(eAPLL, 410, 6, 1, 0);		// 800MHz
		//SYSC_SetPLL(eAPLL, 412, 6, 1, 0);		// 800MHz
		//SYSC_SetPLL(eAPLL, 416, 6, 1, 0);		// 800MHz

		//SYSC_SetPLL(eAPLL, 200, 2, 2, 0);		// 1200MHz,   M/8=25,  M/(Fin/P)=33.3
		//SYSC_SetPLL(eAPLL, 204, 2, 2, 0);		// 1224MHz,   M/8=25.5,  M/(Fin/P)=34
		//SYSC_SetPLL(eAPLL, 208, 2, 2, 0);		// 1224MHz,   M/8=25.5,  M/(Fin/P)=34

		///////////////////////  M Divider 
		//SYSC_SetPLL(eAPLL, 390, 3, 2, 0);		// 800MHz
		//SYSC_SetPLL(eAPLL, 391, 3, 2, 0);		// 800MHz
		//SYSC_SetPLL(eAPLL, 392, 3, 2, 0);		// 800MHz,  => Fail
		//SYSC_SetPLL(eAPLL, 393, 3, 2, 0);		// 800MHz   => Fail (1/8000)
		//SYSC_SetPLL(eAPLL, 394, 3, 2, 0);		// 800MHz   = PASS
		//SYSC_SetPLL(eAPLL, 395, 3, 2, 0);		// 800MHz    => PASS
		//SYSC_SetPLL(eAPLL, 396, 3, 2, 0);		// 800MHz   => PASS
		//SYSC_SetPLL(eAPLL, 397, 3, 2, 0);		// 800MHz => PASS
		//SYSC_SetPLL(eAPLL, 398, 3, 2, 0);		// 800MHz
		//SYSC_SetPLL(eAPLL, 399, 3, 2, 0);		// 800MHz
		//SYSC_SetPLL(eAPLL, 400, 3, 2, 0);		// 800MHz
		//SYSC_SetPLL(eAPLL, 401, 3, 2, 0);		// 800MHz
		//SYSC_SetPLL(eAPLL, 402, 3, 2, 0);		// 800MHz
		//SYSC_SetPLL(eAPLL, 403, 3, 2, 0);		// 800MHz
		//SYSC_SetPLL(eAPLL, 404, 3, 2, 0);		// 800MHz
		//SYSC_SetPLL(eAPLL, 405, 3, 2, 0);		// 800MHz
		//SYSC_SetPLL(eAPLL, 406, 3, 2, 0);		// 800MHz
		//SYSC_SetPLL(eAPLL, 407, 3, 2, 0);		// 800MHz
		//SYSC_SetPLL(eAPLL, 408, 3, 2, 0);		// 800MHz
		//SYSC_SetPLL(eAPLL, 409, 3, 2, 0);		// 800MHz
		//SYSC_SetPLL(eAPLL, 410, 3, 2, 0);		// 800MHz

		///////////////////////  VCO Range
		//SYSC_SetPLL(eAPLL, 500, 6, 1, 0);	 	// APLL => 500MHz	, 1000MHz	
		//SYSC_SetPLL(eAPLL, 550, 3, 3, 0);	 	// APLL => 		, 2200MHz
		//SYSC_SetPLL(eAPLL, 220, 3, 2, 0);		//  APLL => 220MHz,880MHz
		//SYSC_SetPLL(eAPLL, 225, 3, 1, 0);		//  APLL => 450MHz, 900MHz
		//SYSC_SetPLL(eAPLL, 240, 3, 2, 0);		//  APLL => 240MHz, 960MHz
		//SYSC_SetPLL(eAPLL, 275, 3, 2, 0);		//  APLL => 275MHz, 1100MHz
		//SYSC_SetPLL(eAPLL, 280, 3, 2, 0);		//  APLL => 240MHz,1120MHz
		//SYSC_SetPLL(eAPLL, 284, 3, 2, 0);		//  1136MHz
		//SYSC_SetPLL(eAPLL, 288, 3, 2, 0);		//  1152MHz
		//SYSC_SetPLL(eAPLL, 292, 3, 2, 0);		//  1168MHz
		//SYSC_SetPLL(eAPLL, 296, 3, 2, 0);		//  1184MHz
		//SYSC_SetPLL(eAPLL, 312, 3, 2, 0);		//  1184MHz
		//SYSC_SetPLL(eAPLL, 320, 3, 2, 0);		//  APLL => 320MHz,1280MHz
		//SYSC_SetPLL(eAPLL, 325, 3, 2, 0);		//  APLL => 325MHz, 1300MHz
		//SYSC_SetPLL(eAPLL, 360, 3, 2, 0);		//  APLL => 360MHz,1440MHz
		//SYSC_SetPLL(eAPLL, 375, 3, 2, 0);		//  APLL => 375MHz, 1500MHz
		//SYSC_SetPLL(eAPLL, 390, 3, 2, 0);		//  APLL => 390MHz, 1560MHz
		//SYSC_SetPLL(eAPLL, 395, 3, 2, 0);		//  APLL => 395MHz, 1580MHz
		//SYSC_SetPLL(eAPLL, 399, 3, 2, 0);	 	// APLL => 399MHz	, 1596MHz
		//SYSC_SetPLL(eAPLL, 402, 3, 2, 0);	 	// APLL => 402MHz	, 1608MHz
		//SYSC_SetPLL(eAPLL, 405, 3, 2, 0);		//  APLL => 405MHz, 1620MHz
		//SYSC_SetPLL(eAPLL, 410, 3, 2, 0);		//  APLL => 410MHz, 1640MHz
		//SYSC_SetPLL(eAPLL, 425, 3, 2, 0);		//  APLL => 425MHz, 1700MHz
		//SYSC_SetPLL(eAPLL, 440, 3, 2, 0);		//  APLL => 440MHz,1760MHz
		//SYSC_SetPLL(eAPLL, 480, 3, 2, 0);		//  APLL => 480MHz,1920MHz
		//SYSC_SetPLL(eAPLL, 475, 3, 2, 0);		//  APLL => 475MHz, 1900MHz
		//SYSC_SetPLL(eAPLL, 525, 3, 2, 0);		//  APLL => 525MHz, 2100MHz
		//SYSC_SetPLL(eAPLL, 533, 3, 2, 0);		//  APLL => 533MHz, 2132MHz
		//SYSC_SetPLL(eAPLL, 575, 3, 3, 0);		//  APLL => 533MHz, 2132MHz
		
		/////////////////////// For EVT1 Test 
		//SYSC_SetPLL(eAPLL, 200, 3, 2, 0);	 	// APLL => 200MHz	, 800MHz
		//SYSC_SetPLL(eAPLL, 250, 3, 2, 0);	 	// APLL => 250MHz	, 1000MHz
		//SYSC_SetPLL(eAPLL, 300, 3, 2, 0);	 	// APLL => 300MHz	, 1200MHz
		//SYSC_SetPLL(eAPLL, 350, 3, 2, 0);	 	// APLL => 450MHz	, 1400MHz
		//SYSC_SetPLL(eAPLL, 400, 3, 2, 0);	 	// APLL => 400MHz	, 1600MHz
		//SYSC_SetPLL(eAPLL, 450, 3, 2, 0);	 	// APLL => 450MHz	, 1800MHz
		//SYSC_SetPLL(eAPLL, 500, 3, 2, 0);	 	// APLL => 500MHz	, 2000MHz
		

		
		//Delay(50);							// 1st Test ~=170ms
		//Delay(1);							//  Test = 3ms
		GPIO_SetDataEach(eGPIO_N, eGPIO_9, 1);
		
		Outp32SYSC(0x20, uRegValue3_bk);			// Change Divide Value
		Outp32SYSC(0x1C, uRegValue2_bk);			// Clock Source Change	

		Delay(100);								// 100us
		//UART_Getc();
		
		
		SYSC_GetClkInform();		
		UART_InitDebugCh(0, 115200);		

		//-daedoo
		/*
		RTC_SetCON(0,0,0,0,0,1);
		RTC_SetTime(InitYEAR,InitMONTH,InitDATE,InitDAY,InitHOUR,InitMIN,InitSEC);
		RTC_SetTickCNT(100);	
		RTC_SetCON(0,0,0,0,0,0);							// Tick Clock = 128Hz?
		RTC_ClearIntP(0);	
		*/
		RTC_TimeInit(InitYEAR,InitMONTH,InitDATE,InitDAY,InitHOUR,InitMIN,InitSEC);
		RTC_TickCnt(100);	
		RTC_TickClkSelect(CLK_32768Hz);
		RTC_ClearPending(TimeTic);
		
		StartTimer(0);
		//RTC_SetCON(1,0,0,0,0,1);
		RTC_TickTimeEnable(true);
		RTC_Enable(true);
		
		while(g_RTC_ALM==0);
		uTestTime=StopTimer(0);
		//UART_Printf("%d:  uTestTime = %d \n",uTestCount,  uTestTime);
		//RTC_SetCON(0,0,0,0,0,0);
		RTC_Enable(false);

	#if  1
		//if((uTestTime<4040)||(uTestTime>4100))		//Test Case (504,6,5)
		//if((uTestTime<3190)||(uTestTime>3250))		//Test Case (400,3,5)
		if((uTestTime<3040)||(uTestTime>3150))
		//if((uTestTime<1000)||(uTestTime>1050))			
			{
			  // UART_Printf("\nTest Count : %d,  Error: %d \n", uTestCount, uTestErr);
			    uTestErr++;
			    DisplayLED(0xC);
			    //UART_Getc();
			}
	#endif
	
		//SYSC_GetClkInform();		
		//UART_InitDebugCh(0, 115200);		
		//MemoryRWTest();
		//  UART_Getc();
		uTestCount++;
		Delay(100);
		
		//GPIO_SetFunctionEach(eGPIO_F, eGPIO_14, 1);
  		
	}



	Outp32SYSC(0xC, uRegValue0_bk);
	Outp32SYSC(0x10, uRegValue1_bk);
	Outp32SYSC(0x20, uRegValue3_bk);
	Outp32SYSC(0x1C, uRegValue2_bk);

	SYSC_GetClkInform();		
	UART_InitDebugCh(0, 115200);	
	
	
}

// Temp. code
void Test_OnOffPll_EVT1(void)
{
	u32 uOPType,   uRegValue0_bk, uRegValue1_bk, uRegValue2_bk, uRegValue3_bk;
	u32 uLockTime;
	u32 uTemp, uTemp1, uTestCount,uMval;
	u32 uTestTime, uTestErr;


	UART_Printf("[PLL On/Off test, 10000 times]\n");

	//Choose Operating Mode.
	UART_Printf("\nChoose System Operating Mode : 0:Sync Mode, 1:Async Mode	     : ");
       uOPType = UART_GetIntNum();

	//CLK_OUT Pad => HCLK
	SYSC_CtrlCLKOUT(eCLKOUT_APLLOUT, 9);
	

	uLockTime = 100;	
	SYSC_SetLockTime(eAPLL, uLockTime);
	SYSC_SetLockTime(eMPLL, uLockTime);
	// Test Point....
	GPIO_SetFunctionEach(eGPIO_N, eGPIO_9, 1);	// EINT9 : Output pad

	switch(uOPType)
	{
		case 0: 
			UART_Printf("[Selected Sync. Mode Operation]\n");
			SYSC_ChangeMode(eSYNC_MODE);

			uRegValue0_bk = Inp32SYSC(0xC);			// APLL_CON
			uRegValue1_bk = Inp32SYSC(0x10);			// MPLL CON
			uRegValue2_bk = Inp32SYSC(0x1C);			// Clock Source Register
			uRegValue3_bk = Inp32SYSC(0x20);			// Clock Divider value
			
			break;

		case 1: 
			UART_Printf("Selected Async Mode Operation\n");
			//SYSC_ChangeMode(eASYNC_MODE);

			uRegValue0_bk = Inp32SYSC(0xC);			// APLL_CON
			uRegValue1_bk = Inp32SYSC(0x10);			// MPLL CON
			uRegValue2_bk = Inp32SYSC(0x1C);			//Clock Source Register
			uRegValue3_bk = Inp32SYSC(0x20);			// Clock Divider value
			break;

	}


	uTestCount = 0;
	uTestErr=0;
	INTC_SetVectAddr(NUM_RTC_TIC,Isr_RTC_Tick1);
	INTC_Enable(NUM_RTC_TIC);

	uTemp = Inp32SYSC(0x20);
	uTemp = (1<<12)|(0<<9)|(1<<8)|(1<<4)|(0<<0);		// ARM:HCLKx2:HCLK:PCLK = 1:1:2:2
					
	uTemp1 = Inp32SYSC(0x1C);			//Clock Source Register
	uTemp1 = uTemp1 & ~(0x7) | 0x0;

	
	for(uMval=390; uMval<411; uMval++)
	{
		UART_Printf(" M-Div Value : %d \n", uMval);
		UART_Printf(" ============================ \n");
		UART_TxEmpty();
		uTestCount = 0;
		uTestErr=0;
	
	while(1)
		{
		if(UART_GetKey()!=0x0) 
			break;
		if(uTestCount>5000) 
		{
			UART_Printf("	End of test = M-Div Value : %d\n", uMval);
			UART_Printf("PLL ON/OFF Test - Test Count: %d,  Error: %d\n", uTestCount, uTestErr);
			break;
		}
		if(uTestErr>1000)
		{
			UART_Printf("	M-Div Value : %d\n", uMval);
			UART_Printf("PLL ON/OFF Fail - Test Count: %d,  Error: %d\n", uTestCount, uTestErr);
			break;	
		}
//		if ((uTestCount%100)==0)
		if ((uTestCount%100)==0)
		{
			DisplayLED(uTestCount/100);
			
		}
		if ((uTestCount%500)==0)
		{
			DisplayLED(uTestCount/100);
			SYSC_GetClkInform();		
			UART_InitDebugCh(0, 115200);
			Delay(10);
			UART_Printf("Test Count : %d,  Error: %d \n", uTestCount, uTestErr);
			UART_TxEmpty();

		}
			
	// ON&OFF Test
	//for(i=0;i<10000;i++) {
		//isplayLED(0x2);

		g_RTC_ALM =0;		


		Outp32SYSC(0x1C, uTemp1);			// Clock Source Change	
		Outp32SYSC(0x20, uTemp);			// Change Divide Value
		SYSC_StopPLL(eAPLL);
		//Outp32SYSC(0xC, 0x0);				// Temp Test
		//SYSC_StopPLL(eMPLL);
		//GPIO_SetDataEach(eGPIO_N, eGPIO_9, 0);
		
		//UART_Getc();
											// No Delay ~= 0.2ms
		Delay(10);							// Stop Time ~= 34ms
		//Delay(1);							// Stop Time ~=3.5ms
		//Delay(200);							//  Stop Time ~= 667ms
		//Delay(400);
		GPIO_SetDataEach(eGPIO_N, eGPIO_9, 0);
		//DisplayLED(0x8);
		
		
		///////////////////////  M Divider 
		SYSC_SetPLL(eAPLL, uMval, 3, 2, 0);		// 800MHz
		//SYSC_SetPLL(eAPLL, 392, 3, 2, 0);		// 800MHz,  => Fail
		//SYSC_SetPLL(eAPLL, 393, 3, 2, 0);		// 800MHz   => Fail (1/8000)
		//SYSC_SetPLL(eAPLL, 394, 3, 2, 0);		// 800MHz   = PASS
		//SYSC_SetPLL(eAPLL, 395, 3, 2, 0);		// 800MHz    => PASS
		//SYSC_SetPLL(eAPLL, 396, 3, 2, 0);		// 800MHz   => PASS
		//SYSC_SetPLL(eAPLL, 397, 3, 2, 0);		// 800MHz => PASS
		//SYSC_SetPLL(eAPLL, 398, 3, 2, 0);		// 800MHz
		//SYSC_SetPLL(eAPLL, 399, 3, 2, 0);		// 800MHz
		//SYSC_SetPLL(eAPLL, 400, 3, 2, 0);		// 800MHz
		//SYSC_SetPLL(eAPLL, 401, 3, 2, 0);		// 800MHz
		//SYSC_SetPLL(eAPLL, 402, 3, 2, 0);		// 800MHz
		//SYSC_SetPLL(eAPLL, 403, 3, 2, 0);		// 800MHz
		//SYSC_SetPLL(eAPLL, 404, 3, 2, 0);		// 800MHz
		//SYSC_SetPLL(eAPLL, 405, 3, 2, 0);		// 800MHz
		//SYSC_SetPLL(eAPLL, 406, 3, 2, 0);		// 800MHz
		//SYSC_SetPLL(eAPLL, 407, 3, 2, 0);		// 800MHz
		//SYSC_SetPLL(eAPLL, 408, 3, 2, 0);		// 800MHz
		//SYSC_SetPLL(eAPLL, 409, 3, 2, 0);		// 800MHz
		//SYSC_SetPLL(eAPLL, 410, 3, 2, 0);		// 800MHz

				
		//Delay(50);							// 1st Test ~=170ms
		//Delay(1);							//  Test = 3ms
		GPIO_SetDataEach(eGPIO_N, eGPIO_9, 1);
		
		Outp32SYSC(0x20, uRegValue3_bk);			// Change Divide Value
		Outp32SYSC(0x1C, uRegValue2_bk);			// Clock Source Change	

		Delay(100);								// 100us
		//UART_Getc();
		
		
		SYSC_GetClkInform();		
		UART_InitDebugCh(0, 115200);		

		//-daedoo
		/*
		RTC_SetCON(0,0,0,0,0,1);
		RTC_SetTime(InitYEAR,InitMONTH,InitDATE,InitDAY,InitHOUR,InitMIN,InitSEC);
		RTC_SetTickCNT(100);	
		RTC_SetCON(0,0,0,0,0,0);							// Tick Clock = 128Hz?
		RTC_ClearIntP(0);	
		*/
		RTC_TimeInit(InitYEAR,InitMONTH,InitDATE,InitDAY,InitHOUR,InitMIN,InitSEC);
		RTC_TickCnt(100);	
		RTC_TickClkSelect(CLK_32768Hz);
		RTC_ClearPending(TimeTic);
		
		StartTimer(0);
		
		//RTC_SetCON(1,0,0,0,0,1);
		RTC_TickTimeEnable(true);
		RTC_Enable(true);
		
		while(g_RTC_ALM==0);
		uTestTime=StopTimer(0);
		//UART_Printf("%d:  uTestTime = %d \n",uTestCount,  uTestTime);
		//RTC_SetCON(0,0,0,0,0,0);
		RTC_Enable(false);

	#if  1
		if((uTestTime<3050)||(uTestTime>3150))
		//if((uTestTime<1000)||(uTestTime>1050))			
			{
			  // UART_Printf("\nTest Count : %d,  Error: %d \n", uTestCount, uTestErr);
			    uTestErr++;
			    DisplayLED(0xF);
			   // UART_Getc();
			}
	#endif
	
		//SYSC_GetClkInform();		
		//UART_InitDebugCh(0, 115200);		
		//MemoryRWTest();
		//  UART_Getc();
		uTestCount++;
		Delay(100);
		
		//GPIO_SetFunctionEach(eGPIO_F, eGPIO_14, 1);
  		
	}

	Outp32SYSC(0xC, uRegValue0_bk);
	Outp32SYSC(0x10, uRegValue1_bk);
	Outp32SYSC(0x20, uRegValue3_bk);
	Outp32SYSC(0x1C, uRegValue2_bk);

	SYSC_GetClkInform();		
	UART_InitDebugCh(0, 115200);	

}

/*
	Outp32SYSC(0xC, uRegValue0_bk);
	Outp32SYSC(0x10, uRegValue1_bk);
	Outp32SYSC(0x20, uRegValue3_bk);
	Outp32SYSC(0x1C, uRegValue2_bk);

	SYSC_GetClkInform();		
	UART_InitDebugCh(0, 115200);	
*/	
	
}




//////////
// File Name : Test_ManChangeCLKDivider
// File Description : This function is for the Clock Divider
//				 
// Input : 			
//
//						
// Output : NONE
// Version : 
void Test_ManChangeCLKDivider(void)  
{

	u32 uRegValue0_bk;
	u32 uARatio, uHx2Ratio, uPRatio;
	u32 i, uTestType,uTestCount;
		
	SYSC_CtrlCLKOUT(eCLKOUT_HCLK, 9);	
	UART_Printf("[Clock Dividerl Change - This is only for test..]\n");
	// Current Clock Information
	SYSC_GetClkInform();
	UART_Printf("Current Clock Information \n");
	UART_Printf("ARMCLK: %.2fMHz  HCLKx2: %.2fMHz  HCLK: %.2fMHz  PCLK: %.2fMHz\n",(float)g_ARMCLK/1.0e6, (float)g_HCLKx2/1.0e6, (float)g_HCLK/1.0e6, (float)g_PCLK/1.0e6);
	UART_Printf("============================ \n");


	// Save Clock Divider Value
	uRegValue0_bk = Inp32SYSC(0x20);			// Clock Divider value
	// CLK Output PAD Enable
	GPIO_SetFunctionEach(eGPIO_F, eGPIO_14, 3);
	
	UART_Printf("\nEnter the  APLL_Ratio Value( 0~7) :  ");
       uARatio = UART_GetIntNum();

	UART_Printf("\nEnter the  HCLKx2 Ratio Value(HCLKx2=APLL_OUT/HCLKx2_Ratio)( 0~7) :  ");
       uHx2Ratio = UART_GetIntNum();

	UART_Printf("\nEnter the  PCLK Ratio Value (PCLK=HCLKx2/PCLK_Ratio)(Odd Number 1~15) :  ");
       uPRatio = UART_GetIntNum();

	
      UART_Printf("\nSelect Test Type : 0:1time change, 1:repeat test	: ");
      uTestType = UART_GetIntNum();

	if(uTestType == 0)
	{
		// Debug GPIO Setting
	GPIO_SetPullUpDownEach(eGPIO_F, eGPIO_15, 0);
	GPIO_SetFunctionEach(eGPIO_F,eGPIO_15,1);
	GPIO_SetDataEach(eGPIO_F,eGPIO_15,1);
	
	UART_Printf("\npress any key	\n");
	while(!UART_GetKey());	
	
	GPIO_SetDataEach(eGPIO_F,eGPIO_15,0);
	SYSC_SetDIV0(uARatio, 1, 1, uHx2Ratio,  uPRatio, 1, 1,0, 1, 0);  // MPLL Ratio : 1 , HRatio : 1 (HCLKx2 /2) )

	SYSC_GetClkInform();
	UART_InitDebugCh(0, 115200);

	UART_Printf("Current Clock Information \n");
	UART_Printf("ARMCLK: %.2fMHz  HCLKx2: %.2fMHz  HCLK: %.2fMHz  PCLK: %.2fMHz\n",(float)g_ARMCLK/1.0e6, (float)g_HCLKx2/1.0e6, (float)g_HCLK/1.0e6, (float)g_PCLK/1.0e6);
	UART_Printf("============================ \n");
	UART_Printf("press 'q' key to exit\n");	

	
		while(UART_GetKey()!='q')
		{
		DisplayLED(9);
		for(i=0;i<3000000;i++);
		DisplayLED(6);
		for(i=0;i<3000000;i++);
		}

	}
	else if(uTestType == 1)
	{
		while(1) 
		{
		if(UART_GetKey()!=0x0) 
			break;
		if(uTestCount>100000) 
			break;
		if ((uTestCount%1000)==0)
		{
		
			SYSC_GetClkInform();		
			UART_InitDebugCh(0, 115200);
			Delay(100);
			UART_Printf("\n%d \n", uTestCount);

		}
		if(uTestCount%2) 
		{
			DisplayLED(0x8);
			SYSC_SetDIV0(uARatio, 1, 1, uHx2Ratio,  uPRatio, 1, 1,0, 1, 0);
			Delay(100);			
		} 
		else 
		{		
			DisplayLED(0x3);
			Outp32SYSC(0x20, uRegValue0_bk);
			Delay(100);
		}
		
		uTestCount++;
		}	
	}
	
	// Return the pre test value
	Outp32SYSC(0x20, uRegValue0_bk);

	Delay(100);
	
	SYSC_GetClkInform();
	UART_InitDebugCh(0, 115200);
	
}


//////////
// File Name : Test_CLKOUT
// File Description : This function is for the Clock output
//				 
// Input : 			
//
//						
// Output : NONE
// Version : 6410 0.1
void Test_CLKOUT(void)  
{

	u32 uRegValue0_bk;
	u32 uDiv, uSEL, uDDiv, uDCMP, uDSEL;
	u32 i, uDclk;
		
	UART_Printf("[Clock Output Test - rCLK_OUT  & DCLK ]\n");
	// Current Clock Information
	SYSC_GetClkInform();
	UART_Printf("FoutAPLL: %.2fMHz, ARMCLK: %.2fMHz,  HCLKx2: %.2fMHz,  HCLK: %.2fMHz,  PCLK: %.2fMHz\n", (float)g_APLL/1.0e6, (float)g_ARMCLK/1.0e6, (float)g_HCLKx2/1.0e6, (float)g_HCLK/1.0e6, (float)g_PCLK/1.0e6);
	
	// Save CLK_OUT Register
	uRegValue0_bk = Inp32SYSC(0x2C);			// Back-up CLK_OUT Register Value
	// CLK Output PAD Enable
	GPIO_SetFunctionEach(eGPIO_F, eGPIO_14, 3);
		
	UART_Printf("\n CLK Out Select  : 0:APLL Out/4, 1:EPLL Out, 2:HCLK, 3:CLK48M, 4:CLK27M, 5:RTC, 6: Tick, 7: DOUT, ");
	UART_Printf("\n CLK Out Select  : 8:FIN, 9:FrefAPLL, 10:FfbAPLL/2, 11:FrefMPLL, 12:FfbMPLL/2, 13:FrefEPLL, 14:FfbEPLL/2  ");
       uSEL = UART_GetIntNum();	
	if((uSEL==1)||(uSEL==13)||(uSEL==14)) SYSC_SetPLL(eEPLL, 32, 1, 2, 0);
	if(uSEL==3) ; //Enable USB Clock
	if(uSEL==4 ) Outp32SYSC(0x804, (Inp32SYSC(0x804))|1);
	if(uSEL==5) ;
	if(uSEL==6) {
		//-daedoo
		/*
		RTC_SetCON(0,2,0,0,0,0); 		   //Select Tick Timer Sub Clock
		RTC_SetTickCNT(0xffff);	   
		RTC_SetCON(1,2,0,0,0,1);
		*/
		RTC_TickClkSelect(CLK_8192Hz); 		   //Select Tick Timer Sub Clock
		RTC_TickClkSelect(0xffff);	   
		RTC_TickTimeEnable(true);
		RTC_Enable(true);

	}; //TICK Clock Selection


	if(uSEL == 7 )
	{
		SYSC_CtrlCLKOUT((CLKOUT_eTYPE)uSEL,  0); 	   	
		UART_Printf("\n Choose DCLK Source Clock  : 0:PCLK, 1:48MHz     :  ");
       	uDSEL = UART_GetIntNum();

		if(uDSEL==0)
		{
			uDclk = (unsigned int) ((float)g_PCLK/(float)(uDDiv+1));
		}
		else if(uDSEL==1)
		{
			uDclk = (unsigned int) ((float)(48000000)/(float)(uDDiv+1));
		}			

		UART_Printf("\n change DCLK Value Manually[y or n]:");
		if(UART_Getc()=='y') {
			UART_Printf("\n Enter the DCLKDIV (0~15) \n");
			uDDiv = UART_GetIntNum();
			
			UART_Printf("\n Enter the DCLKCMP (0~15, <DCLKDIV) \n");
			uDCMP = UART_GetIntNum();
			
			SYSC_CtrlDCLK(uDCMP, uDDiv, uDSEL, 1);			
			UART_Printf("\n [DCLK: % 4.1f MHz], [High level duration : %d], [Low level duration : %d]\n", (float) uDclk/1.0e6, uDDiv-uDCMP, uDCMP+1);
		}
		else {
			uDDiv=uDCMP=0;
			UART_Printf("Press any key except 'x' to continue TEST!!\n");
			while(UART_Getc()!='x') {
				SYSC_CtrlDCLK(uDCMP, uDDiv, uDSEL, 1);
				UART_Printf("\n [DCLK: % 4.1f MHz], [High level duration : %d], [Low level duration : %d]\n", (float) uDclk/1.0e6, uDDiv-uDCMP, uDCMP+1);
				UART_Printf("DIVISION: %d, COMPARE: %d\n", uDDiv, uDCMP);
				
				if(uDCMP==0xf) break;
				if(uDCMP==uDDiv) {
					uDDiv++;
					uDCMP=0;
				}
				else if(uDCMP < uDDiv) {
					uDCMP++;
				}				
			}
		}	
	}
	else {

		UART_Printf("\n change Dividing Value Manually[y or n]:");
		if(UART_Getc()=='y') {
			UART_Printf("\nEnter the  CLK_Out Dividing Value(0~15) :  ");
		       uDiv = UART_GetIntNum();
			SYSC_CtrlCLKOUT((CLKOUT_eTYPE)uSEL,  uDiv);		   
		}
		else {
			uDiv=0;
			UART_Printf("\nCLK_Out Dividing Value=%d\n", uDiv+1);
			SYSC_CtrlCLKOUT((CLKOUT_eTYPE)uSEL,  uDiv);		   
			UART_Getc();
			uDiv=1;
			UART_Printf("\nCLK_Out Dividing Value=%d\n", uDiv+1);
			SYSC_CtrlCLKOUT((CLKOUT_eTYPE)uSEL,  uDiv);		 
			UART_Getc();
			uDiv=2;
			UART_Printf("\nCLK_Out Dividing Value=%d\n", uDiv+1);
			SYSC_CtrlCLKOUT((CLKOUT_eTYPE)uSEL,  uDiv);		 
			UART_Getc();
			uDiv=3;
			UART_Printf("\nCLK_Out Dividing Value=%d\n", uDiv+1);
			SYSC_CtrlCLKOUT((CLKOUT_eTYPE)uSEL,  uDiv);		 
			UART_Getc();
			uDiv=4;
			UART_Printf("\nCLK_Out Dividing Value=%d\n", uDiv+1);
			SYSC_CtrlCLKOUT((CLKOUT_eTYPE)uSEL,  uDiv);		 
			UART_Getc();
			uDiv=5;
			UART_Printf("\nCLK_Out Dividing Value=%d\n", uDiv+1);
			SYSC_CtrlCLKOUT((CLKOUT_eTYPE)uSEL,  uDiv);		 
			UART_Getc();
			uDiv=6;
			UART_Printf("\nCLK_Out Dividing Value=%d\n", uDiv+1);
			SYSC_CtrlCLKOUT((CLKOUT_eTYPE)uSEL,  uDiv);		 
			UART_Getc();
			uDiv=7;
			UART_Printf("\nCLK_Out Dividing Value=%d\n", uDiv+1);
			SYSC_CtrlCLKOUT((CLKOUT_eTYPE)uSEL,  uDiv);		 
			UART_Getc();
			uDiv=8;
			UART_Printf("\nCLK_Out Dividing Value=%d\n", uDiv+1);
			SYSC_CtrlCLKOUT((CLKOUT_eTYPE)uSEL,  uDiv);		 
			UART_Getc();
			uDiv=9;
			UART_Printf("\nCLK_Out Dividing Value=%d\n", uDiv+1);
			SYSC_CtrlCLKOUT((CLKOUT_eTYPE)uSEL,  uDiv);		 
			UART_Getc();
			uDiv=10;
			UART_Printf("\nCLK_Out Dividing Value=%d\n", uDiv+1);
			SYSC_CtrlCLKOUT((CLKOUT_eTYPE)uSEL,  uDiv);		 
			UART_Getc();
			uDiv=11;
			UART_Printf("\nCLK_Out Dividing Value=%d\n", uDiv+1);
			SYSC_CtrlCLKOUT((CLKOUT_eTYPE)uSEL,  uDiv);		 
			UART_Getc();
			uDiv=12;
			UART_Printf("\nCLK_Out Dividing Value=%d\n", uDiv+1);
			SYSC_CtrlCLKOUT((CLKOUT_eTYPE)uSEL,  uDiv);		 
			UART_Getc();
			uDiv=13;
			UART_Printf("\nCLK_Out Dividing Value=%d\n", uDiv+1);
			SYSC_CtrlCLKOUT((CLKOUT_eTYPE)uSEL,  uDiv);		 
			UART_Getc();
			uDiv=14;
			UART_Printf("\nCLK_Out Dividing Value=%d\n", uDiv+1);
			SYSC_CtrlCLKOUT((CLKOUT_eTYPE)uSEL,  uDiv);		 
			UART_Getc();
			uDiv=15;
			UART_Printf("\nCLK_Out Dividing Value=%d\n", uDiv+1);
			SYSC_CtrlCLKOUT((CLKOUT_eTYPE)uSEL,  uDiv);		 
		}	

	}
	UART_Printf("press any key to exit\n");
	UART_Getc();

	// Return the pre test value	
	Outp32SYSC(0x2C, uRegValue0_bk);
	GPIO_SetFunctionEach(eGPIO_F, eGPIO_14, 1);
		
	
}


//////////
// File Name : Test_ChangeOPMode
// File Description : This function is for the changing operating mode.
//				 
// Input : 			
//
//						
// Output : NONE
// Version : 

// Only Test Mode.....
void Test_ChangeOPMode(void)  
{
	u32 i, uTestCount;

	UART_Printf("Operating mode Change Test\n");

	SYSC_CtrlCLKOUT( eCLKOUT_APLLOUT, 9); // for clock test, jangwj
	//SYSC_ChangeSYSCLK_1(eAPLL480M, eAPLL266M ,  0, 1,3);	



	SYSC_ChangeMode(eASYNC_MODE);
	SYSC_GetClkInform();
	UART_InitDebugCh(0, 115200);
	UART_Printf("\n\n");
	UART_Printf("ARMCLK: %.2fMHz  HCLKx2: %.2fMHz  HCLK: %.2fMHz  PCLK: %.2fMHz\n",(float)g_ARMCLK/1.0e6, (float)g_HCLKx2/1.0e6, (float)g_HCLK/1.0e6, (float)g_PCLK/1.0e6);
	UART_Printf("SYNC Mode : %d\n", g_SYNCACK);
	UART_Printf("\n");    


	SYSC_ChangeMode(eSYNC_MODE);
	SYSC_GetClkInform();
	UART_InitDebugCh(0, 115200);
	UART_Printf("\n\n");
	UART_Printf("ARMCLK: %.2fMHz  HCLKx2: %.2fMHz  HCLK: %.2fMHz  PCLK: %.2fMHz\n",(float)g_ARMCLK/1.0e6, (float)g_HCLKx2/1.0e6, (float)g_HCLK/1.0e6, (float)g_PCLK/1.0e6);
	UART_Printf("SYNC Mode : %d\n", g_SYNCACK);
	UART_Printf("\n");    

	uTestCount=0;
 
	UART_Printf("press 'q' key to exit\n");	
	while(UART_GetKey()!='q')
	{

		if(UART_GetKey()!=0x0) 
			break;
		if(uTestCount>100000) 
			break;
		if ((uTestCount%1000)==0)
		{
			DisplayLED(uTestCount>>10);
			SYSC_GetClkInform();		
			UART_InitDebugCh(0, 115200);
			Delay(100);
			UART_Printf("\n%d \n", uTestCount);

		}

		//DisplayLED(i%15);
		SYSC_ChangeMode(eASYNC_MODE);
		SYSC_GetClkInform();
		UART_InitDebugCh(0, 115200);

		Delay(10);
		
		SYSC_ChangeMode(eSYNC_MODE);
		SYSC_GetClkInform();
		UART_InitDebugCh(0, 115200);

		uTestCount++;
	
		if ((uTestCount%1000)==0)
		{
				DisplayLED(i>>10);
		}
		
		/*
		DisplayLED(9);
		for(i=0;i<30000000;i++);
		DisplayLED(6);
		for(i=0;i<30000000;i++);
		*/
	}
		
	
}





//////////
// Function Name : Insert_Seed
// Function Description : This function generate random number using  information of the RTC
//					 
// Input : 	
// Output : 	
//			
// Version : v0.1
void Insert_Seed(void)
{
	u32 uBCDDAY, uBCDHOUR, uBCDMIN, uBCDSEC;

	uBCDDAY 	=	Inp32(0x7E005080);
	uBCDHOUR    =	Inp32(0x7E005078);
	uBCDMIN		=	Inp32(0x7E005074);
	uBCDSEC	=	Inp32(0x7E005070);

	UART_Printf("uBCDDAY:0x%x, uBCDHOUR:0x%x, uBCDMIN:0x%x, uBCDSEC:0x%x\n", uBCDDAY, uBCDHOUR, uBCDMIN, uBCDSEC);
	g_iSeed = (uBCDDAY*24*60*60)+(uBCDHOUR*60*60)+(uBCDMIN*60)+uBCDSEC;
	srand(g_iSeed);	
}

#if 0
//////////
// Function Name : Select_PMS_Random
// Function Description : This function generate P,M,S Value.
//					 
// Input : 	
// Output : 	
//			
// Version : v0.1
// PMS Value must be changed~~~
void Select_PMS_Random(void)
{
	g_iRandom_PMS = rand()%18;

	DisplayLED(g_iRoop3/10);
	g_iRoop3++;
	switch ( g_iRandom_PMS )
	{
		case 0:
			g_iPvalue = 6;
			g_iMvalue = 800;
			g_iSvalue = 3;
			g_iCompare_1 = g_iRandom_PMS;
			g_iAPLL = 200;
			break;
		case 1:
			g_iPvalue = 6;
			g_iMvalue = 880;
			g_iSvalue = 3;
			g_iCompare_1 = g_iRandom_PMS;
			g_iAPLL = 220;
			break;
		case 2:
			g_iPvalue = 6;
			g_iMvalue = 960;
			g_iSvalue = 3;
			g_iCompare_1 = g_iRandom_PMS;
			g_iAPLL = 240;
			break;
		case 3:
			g_iPvalue = 3;
			g_iMvalue = 260;
			g_iSvalue = 2;
			g_iCompare_1 = g_iRandom_PMS;
			g_iAPLL = 260;
			break;
		case 4:
			g_iPvalue = 3;
			g_iMvalue = 280;
			g_iSvalue= 2;
			g_iCompare_1 = g_iRandom_PMS;
			g_iAPLL = 280;
			break;			
		case 5:
			g_iPvalue = 3;
			g_iMvalue= 300;
			g_iSvalue = 2;
			g_iCompare_1 = g_iRandom_PMS;
			g_iAPLL = 300;
			break;			
		case 6:
			g_iPvalue = 3;
			g_iMvalue = 320;
			g_iSvalue = 2;
			g_iCompare_1 = g_iRandom_PMS;
			g_iAPLL = 320;
			break;			
		case 7:
			g_iPvalue = 3;
			g_iMvalue= 340;
			g_iSvalue= 2;
			g_iCompare_1 = g_iRandom_PMS;
			g_iAPLL = 340;
			break;			
		case 8:
			g_iPvalue = 3;
			g_iMvalue = 360;
			g_iSvalue = 2;
			g_iCompare_1 = g_iRandom_PMS;
			g_iAPLL = 360;
			break;			
		case 9:
			g_iPvalue = 3;
			g_iMvalue = 380;
			g_iSvalue = 2;
			g_iCompare_1 = g_iRandom_PMS;
			g_iAPLL = 380;
			break;			
		case 10:
			g_iPvalue = 3;
			g_iMvalue = 400;
			g_iSvalue = 2;
			g_iCompare_1 = g_iRandom_PMS;
			g_iAPLL = 400;
			break;			
		case 11:
			g_iPvalue = 3;
			g_iMvalue = 420;
			g_iSvalue = 2;
			g_iCompare_1 = g_iRandom_PMS;
			g_iAPLL = 420;
			break;			
		case 12:
			g_iPvalue = 3;
			g_iMvalue = 440;
			g_iSvalue = 2;
			g_iCompare_1 = g_iRandom_PMS;
			g_iAPLL = 440;
			break;			
		case 13:
			g_iPvalue = 3;
			g_iMvalue = 460;
			g_iSvalue = 2;
			g_iCompare_1 = g_iRandom_PMS;
			g_iAPLL = 460;
			break;			
		case 14:
			g_iPvalue = 3;
			g_iMvalue = 480;
			g_iSvalue = 2;
			g_iCompare_1 = g_iRandom_PMS;
			g_iAPLL = 480;
			break;			
		case 15:
			g_iPvalue = 3;
			g_iMvalue = 500;
			g_iSvalue = 2;
			g_iCompare_1 = g_iRandom_PMS;
			g_iAPLL = 520;
			break;			
		case 16:
			g_iPvalue = 6;
			g_iMvalue = 520;
			g_iSvalue = 1;
			g_iCompare_1 = g_iRandom_PMS;
			g_iAPLL = 520;
			break;			
		case 17:
			g_iPvalue = 6;
			g_iMvalue = 540;
			g_iSvalue = 1;
			g_iCompare_1 = g_iRandom_PMS;
			g_iAPLL = 540;
			break;			
		default :

			break;
	}
}

// PMS Value must be changed~~~
void Select_PMS_Random_M(void)
{
	g_iRandom_PMS = rand()%18;

	DisplayLED(g_iRoop3/10);
	g_iRoop3++;
	switch ( g_iRandom_PMS )
	{
		case 0:
			g_iPvalue = 6;
			g_iMvalue = 800;
			g_iSvalue = 3;
			g_iCompare_1 = g_iRandom_PMS;
			g_iMPLL = 200;
			break;
		case 1:
			g_iPvalue = 6;
			g_iMvalue = 880;
			g_iSvalue = 3;
			g_iCompare_1 = g_iRandom_PMS;
			g_iMPLL = 220;
			break;
		case 2:
			g_iPvalue = 6;
			g_iMvalue = 960;
			g_iSvalue = 3;
			g_iCompare_1 = g_iRandom_PMS;
			g_iMPLL = 240;
			break;
		case 3:
			g_iPvalue = 3;
			g_iMvalue = 260;
			g_iSvalue = 2;
			g_iCompare_1 = g_iRandom_PMS;
			g_iMPLL = 260;
			break;
		case 4:
			g_iPvalue = 3;
			g_iMvalue = 280;
			g_iSvalue= 3;
			g_iCompare_1 = g_iRandom_PMS;
			g_iMPLL = 140;
			break;			
		case 5:
			g_iPvalue = 3;
			g_iMvalue= 300;
			g_iSvalue = 3;
			g_iCompare_1 = g_iRandom_PMS;
			g_iMPLL = 150;
			break;			
		case 6:
			g_iPvalue = 3;
			g_iMvalue = 320;
			g_iSvalue = 3;
			g_iCompare_1 = g_iRandom_PMS;
			g_iMPLL = 160;
			break;			
		case 7:
			g_iPvalue = 3;
			g_iMvalue= 340;
			g_iSvalue= 3;
			g_iCompare_1 = g_iRandom_PMS;
			g_iMPLL = 170;
			break;			
		case 8:
			g_iPvalue = 3;
			g_iMvalue = 360;
			g_iSvalue = 3;
			g_iCompare_1 = g_iRandom_PMS;
			g_iMPLL = 180;
			break;			
		case 9:
			g_iPvalue = 3;
			g_iMvalue = 380;
			g_iSvalue = 3;
			g_iCompare_1 = g_iRandom_PMS;
			g_iMPLL = 190;
			break;			
		case 10:
			g_iPvalue = 3;
			g_iMvalue = 400;
			g_iSvalue = 3;
			g_iCompare_1 = g_iRandom_PMS;
			g_iMPLL = 200;
			break;			
		case 11:
			g_iPvalue = 3;
			g_iMvalue = 420;
			g_iSvalue = 3;
			g_iCompare_1 = g_iRandom_PMS;
			g_iMPLL = 210;
			break;			
		case 12:
			g_iPvalue = 3;
			g_iMvalue = 440;
			g_iSvalue = 3;
			g_iCompare_1 = g_iRandom_PMS;
			g_iMPLL = 220;
			break;			
		case 13:
			g_iPvalue = 3;
			g_iMvalue = 460;
			g_iSvalue = 3;
			g_iCompare_1 = g_iRandom_PMS;
			g_iMPLL = 230;
			break;			
		case 14:
			g_iPvalue = 3;
			g_iMvalue = 480;
			g_iSvalue = 3;
			g_iCompare_1 = g_iRandom_PMS;
			g_iMPLL = 240;
			break;			
		case 15:
			g_iPvalue = 3;
			g_iMvalue = 500;
			g_iSvalue = 3;
			g_iCompare_1 = g_iRandom_PMS;
			g_iMPLL = 260;
			break;			
		case 16:
			g_iPvalue = 6;
			g_iMvalue = 520;
			g_iSvalue = 2;
			g_iCompare_1 = g_iRandom_PMS;
			g_iMPLL = 260;
			break;			
		case 17:
			g_iPvalue = 6;
			g_iMvalue = 532;
			g_iSvalue = 2;
			g_iCompare_1 = g_iRandom_PMS;
			g_iMPLL = 266;
			break;			
		default :

			break;
	}
}

#endif

void SetSafeClockDivider(void)
{
	if(g_iAPLL>532 && g_iAPLL<=800) {
//		DMC_PreChangeSDRAMParameter();
		SYSC_SetDIV(eHCLKX2_RATIO, 2);
		SYSC_SetDIV(eHCLK_RATIO, 1);
	}	
	else if(g_iAPLL>800 &&  g_iAPLL<=1064)	{
//		DMC_PreChangeSDRAMParameter();
		SYSC_SetDIV(eHCLKX2_RATIO, 3);		
		SYSC_SetDIV(eHCLK_RATIO, 1);
	}	
	else if(g_iAPLL>1064 && g_iAPLL<=1330) {
//		DMC_PreChangeSDRAMParameter();
		SYSC_SetDIV(eHCLKX2_RATIO, 4);		
		SYSC_SetDIV(eHCLK_RATIO, 1);		
	}
	else {
		SYSC_SetDIV(eHCLKX2_RATIO, 1);		
		SYSC_SetDIV(eHCLK_RATIO, 1);		
	}	
}


//////////
// Function Name : Select_PMS_Random
// Function Description : This function generate P,M,S Value.
//					 
// Input : 	
// Output : 	
//			
// Version : v0.1
// PMS Value must be changed~~~
void Select_PMS_Random(void)
{
	g_iRandom_PMS = rand()%(g_NumPLL-1);

	DisplayLED(g_iRoop3/10);
	g_iRoop3++;
	switch ( g_iRandom_PMS )
	{
		case 0:
			g_iPvalue = 3;
			g_iMvalue = 400;
			g_iSvalue = 5;
			g_iCompare_1 = g_iRandom_PMS;
			g_iAPLL = 50;
			break;
		case 1:
			g_iPvalue = 3;
			g_iMvalue = 400;
			g_iSvalue = 4;
			g_iCompare_1 = g_iRandom_PMS;
			g_iAPLL = 100;
			break;
		case 2:
			g_iPvalue = 3;
			g_iMvalue = 400;
			g_iSvalue = 3;
			g_iCompare_1 = g_iRandom_PMS;
			g_iAPLL = 200;
			break;
		case 3:
			g_iPvalue = 3;
			g_iMvalue = 266;
			g_iSvalue = 2;
			g_iCompare_1 = g_iRandom_PMS;
			g_iAPLL = 266;
			break;
		case 4:
			g_iPvalue = 3;
			g_iMvalue = 300;
			g_iSvalue= 2;
			g_iCompare_1 = g_iRandom_PMS;
			g_iAPLL = 300;
			break;			
		case 5:
			g_iPvalue = 3;
			g_iMvalue= 340;
			g_iSvalue = 2;
			g_iCompare_1 = g_iRandom_PMS;
			g_iAPLL = 340;
			break;			
		case 6:
			g_iPvalue = 3;
			g_iMvalue = 360;
			g_iSvalue = 2;
			g_iCompare_1 = g_iRandom_PMS;
			g_iAPLL = 360;
			break;			
		case 7:
			g_iPvalue = 3;
			g_iMvalue= 380;
			g_iSvalue= 2;
			g_iCompare_1 = g_iRandom_PMS;
			g_iAPLL = 380;
			break;			
		case 8:
			g_iPvalue = 3;
			g_iMvalue = 400;
			g_iSvalue = 2;
			g_iCompare_1 = g_iRandom_PMS;
			g_iAPLL = 400;
			break;			
		case 9:
			g_iPvalue = 3;
			g_iMvalue = 205;
			g_iSvalue = 1;
			g_iCompare_1 = g_iRandom_PMS;
			g_iAPLL = 410;
			break;			
		case 10:
			g_iPvalue = 3;
			g_iMvalue = 210;
			g_iSvalue = 1;
			g_iCompare_1 = g_iRandom_PMS;
			g_iAPLL = 420;
			break;			
		case 11:
			g_iPvalue = 3;
			g_iMvalue = 215;
			g_iSvalue = 1;
			g_iCompare_1 = g_iRandom_PMS;
			g_iAPLL = 430;
			break;			
		case 12:
			g_iPvalue = 3;
			g_iMvalue = 220;
			g_iSvalue = 1;
			g_iCompare_1 = g_iRandom_PMS;
			g_iAPLL = 440;
			break;			
		case 13:
			g_iPvalue = 3;
			g_iMvalue = 225;
			g_iSvalue = 1;
			g_iCompare_1 = g_iRandom_PMS;
			g_iAPLL = 450;
			break;			
		case 14:
			g_iPvalue = 3;
			g_iMvalue = 230;
			g_iSvalue = 1;
			g_iCompare_1 = g_iRandom_PMS;
			g_iAPLL = 460;
			break;			
		case 15:
			g_iPvalue = 3;
			g_iMvalue = 235;
			g_iSvalue = 1;
			g_iCompare_1 = g_iRandom_PMS;
			g_iAPLL = 470;
			break;			
		case 16:
			g_iPvalue = 3;
			g_iMvalue = 240;
			g_iSvalue = 1;
			g_iCompare_1 = g_iRandom_PMS;
			g_iAPLL = 480;
			break;			
		case 17:
			g_iPvalue = 3;
			g_iMvalue = 245;
			g_iSvalue = 1;
			g_iCompare_1 = g_iRandom_PMS;
			g_iAPLL = 490;
			break;	
		case 18:
			g_iPvalue = 3;
			g_iMvalue = 250;
			g_iSvalue = 1;
			g_iCompare_1 = g_iRandom_PMS;
			g_iAPLL = 500;
			break;			
		case 19:
			g_iPvalue = 3;
			g_iMvalue = 255;
			g_iSvalue = 1;
			g_iCompare_1 = g_iRandom_PMS;
			g_iAPLL = 510;
			break;			
		case 20:
			g_iPvalue = 3;
			g_iMvalue = 266;
			g_iSvalue = 1;
			g_iCompare_1 = g_iRandom_PMS;
			g_iAPLL = 532;
			break;			
		case 21:
			g_iPvalue = 3;
			g_iMvalue = 270;
			g_iSvalue = 1;
			g_iCompare_1 = g_iRandom_PMS;
			g_iAPLL = 540;
			break;			
		case 22:
			g_iPvalue = 3;
			g_iMvalue = 275;
			g_iSvalue = 1;
			g_iCompare_1 = g_iRandom_PMS;
			g_iAPLL = 550;
			break;			
		case 23:
			g_iPvalue = 3;
			g_iMvalue = 280;
			g_iSvalue = 1;
			g_iCompare_1 = g_iRandom_PMS;
			g_iAPLL = 560;
			break;			
		case 24:
			g_iPvalue = 3;
			g_iMvalue = 333;
			g_iSvalue = 1;
			g_iCompare_1 = g_iRandom_PMS;
			g_iAPLL = 666;
			break;			
		case 25:
			g_iPvalue = 3;
			g_iMvalue = 400;
			g_iSvalue = 1;
			g_iCompare_1 = g_iRandom_PMS;
			g_iAPLL = 800;
			break;			
		case 26:
			g_iPvalue = 3;
			g_iMvalue = 225;
			g_iSvalue = 0;
			g_iCompare_1 = g_iRandom_PMS;
			g_iAPLL = 900;
			break;					
		default :
			g_iPvalue = 3;
			g_iMvalue = 225;
			g_iSvalue = 0;
			g_iCompare_1 = g_iRandom_PMS;
			g_iAPLL = 900;
			break;
	}
	UART_Printf("Next Frequency:%dMhz\n", g_iAPLL);
}

// PMS Value must be changed~~~
void Select_PMS_Random_M(void)
{
	g_iRandom_PMS = rand()%18;

	DisplayLED(g_iRoop3/10);
	g_iRoop3++;
	switch ( g_iRandom_PMS )
	{
		case 0:
			g_iPvalue = 6;
			g_iMvalue = 800;
			g_iSvalue = 3;
			g_iCompare_1 = g_iRandom_PMS;
			g_iMPLL = 200;
			break;
		case 1:
			g_iPvalue = 6;
			g_iMvalue = 880;
			g_iSvalue = 3;
			g_iCompare_1 = g_iRandom_PMS;
			g_iMPLL = 220;
			break;
		case 2:
			g_iPvalue = 6;
			g_iMvalue = 960;
			g_iSvalue = 3;
			g_iCompare_1 = g_iRandom_PMS;
			g_iMPLL = 240;
			break;
		case 3:
			g_iPvalue = 3;
			g_iMvalue = 260;
			g_iSvalue = 2;
			g_iCompare_1 = g_iRandom_PMS;
			g_iMPLL = 260;
			break;
		case 4:
			g_iPvalue = 3;
			g_iMvalue = 280;
			g_iSvalue= 3;
			g_iCompare_1 = g_iRandom_PMS;
			g_iMPLL = 140;
			break;			
		case 5:
			g_iPvalue = 3;
			g_iMvalue= 300;
			g_iSvalue = 3;
			g_iCompare_1 = g_iRandom_PMS;
			g_iMPLL = 150;
			break;			
		case 6:
			g_iPvalue = 3;
			g_iMvalue = 320;
			g_iSvalue = 3;
			g_iCompare_1 = g_iRandom_PMS;
			g_iMPLL = 160;
			break;			
		case 7:
			g_iPvalue = 3;
			g_iMvalue= 340;
			g_iSvalue= 3;
			g_iCompare_1 = g_iRandom_PMS;
			g_iMPLL = 170;
			break;			
		case 8:
			g_iPvalue = 3;
			g_iMvalue = 360;
			g_iSvalue = 3;
			g_iCompare_1 = g_iRandom_PMS;
			g_iMPLL = 180;
			break;			
		case 9:
			g_iPvalue = 3;
			g_iMvalue = 380;
			g_iSvalue = 3;
			g_iCompare_1 = g_iRandom_PMS;
			g_iMPLL = 190;
			break;			
		case 10:
			g_iPvalue = 3;
			g_iMvalue = 400;
			g_iSvalue = 3;
			g_iCompare_1 = g_iRandom_PMS;
			g_iMPLL = 200;
			break;			
		case 11:
			g_iPvalue = 3;
			g_iMvalue = 420;
			g_iSvalue = 3;
			g_iCompare_1 = g_iRandom_PMS;
			g_iMPLL = 210;
			break;			
		case 12:
			g_iPvalue = 3;
			g_iMvalue = 440;
			g_iSvalue = 3;
			g_iCompare_1 = g_iRandom_PMS;
			g_iMPLL = 220;
			break;			
		case 13:
			g_iPvalue = 3;
			g_iMvalue = 460;
			g_iSvalue = 3;
			g_iCompare_1 = g_iRandom_PMS;
			g_iMPLL = 230;
			break;			
		case 14:
			g_iPvalue = 3;
			g_iMvalue = 480;
			g_iSvalue = 3;
			g_iCompare_1 = g_iRandom_PMS;
			g_iMPLL = 240;
			break;			
		case 15:
			g_iPvalue = 3;
			g_iMvalue = 500;
			g_iSvalue = 3;
			g_iCompare_1 = g_iRandom_PMS;
			g_iMPLL = 260;
			break;			
		case 16:
			g_iPvalue = 6;
			g_iMvalue = 520;
			g_iSvalue = 2;
			g_iCompare_1 = g_iRandom_PMS;
			g_iMPLL = 260;
			break;			
		case 17:
			g_iPvalue = 6;
			g_iMvalue = 532;
			g_iSvalue = 2;
			g_iCompare_1 = g_iRandom_PMS;
			g_iMPLL = 266;
			break;			
		default :

			break;
	}
}


//////////
// Function Name : Apply_PMS
// Function Description : This function 
//					 
// Input : 	
// Output : 	
//			
// Version : v0.1 - 070228
void Apply_PMS(void)
{
	u32 uRegValue_APLL;
	u32 uRet;
	//u32 uRegValue_DIV0, uRegValue_SRC, uTemp, uTemp1;
	
	//EVT0, Using External Clock.

//	if(SYSC_CheckOpMode()) SetSafeClockDivider();


	uRegValue_APLL =  (u32)(((u32)(0x1<<31))|(g_iMvalue<<16)|(g_iPvalue<<8)|(g_iSvalue<<0));
	
	// Clock Divider Change 1:2:2
	//uRegValue_DIV0 = Inp32SYSC(0x20);
	//uTemp = (uRegValue_DIV0&~(0xFFFF))|(1<<12)|((0<<9)|(1<<8)|(1<<4)|(0<<0));		// ARM:HCLKx2:HCLK:PCLK = 1:1:2:2

	//uRegValue_SRC = Inp32SYSC(0x1C);
	//uTemp1 = (uRegValue_SRC&~(0x7))|0x0;

#if 0	//EVT0
	Outp32SYSC(0x1c, uTemp1);	// Clock Source Change
	Outp32SYSC(0x20, uTemp);	// Change Clock Divider
#endif

	//PLL Change
	Outp32SYSC(0xc, uRegValue_APLL);

	while(((Inp32SYSC(0xc)>>30)&0x1)==0);

#if 0	//EVT0
	// Change Divider Value
	Outp32SYSC(0x20, uRegValue_DIV0);
	// Change Clock SRC
	Outp32SYSC(0x1c, uRegValue_SRC);
#endif

	SYSC_GetClkInform();
	UART_InitDebugCh(0, 115200);
	//if(SYSC_CheckOpMode()) DMC_ChangeSDRAMParameter(1, (u32)g_HCLK/1.0e6);
//	Delay(100);

	uRet = MemoryRWTest1(0x52000000, 0x52100000);

	if(uRet)
		UART_Printf("Memory Read write Test is FAIL!\n");
	else	
		UART_Printf("Memory Read write Test is OK!\n");

 	UART_Printf("====[%d][%d]======= \n", g_iRoop3, g_iRoop6);
	UART_Printf("Current Clock Information \n");
	UART_Printf("ARMCLK: %.2fMHz  HCLKx2: %.2fMHz  HCLK: %.2fMHz  PCLK: %.2fMHz\n",(float)g_ARMCLK/1.0e6, (float)g_HCLKx2/1.0e6, (float)g_HCLK/1.0e6, (float)g_PCLK/1.0e6);
	UART_Printf("============================ \n");
	UART_TxEmpty();
		
}


void Apply_PMS_M(void)
{
	u32 uRegValue_MPLL;
	u32 uRet;
	//u32 uRegValue_DIV0, uRegValue_SRC, uTemp, uTemp1;
	
	//EVT0, Using External Clock.
	

	uRegValue_MPLL =  (u32)(((u32)(0x1<<31))|(g_iMvalue<<16)|(g_iPvalue<<8)|(g_iSvalue<<0));
	
	// Clock Divider Change 1:2:2
	//uRegValue_DIV0 = Inp32SYSC(0x20);
	//uTemp = (uRegValue_DIV0&~(0xFFFF))|(1<<12)|((0<<9)|(1<<8)|(1<<4)|(0<<0));		// ARM:HCLKx2:HCLK:PCLK = 1:1:2:2

	//uRegValue_SRC = Inp32SYSC(0x1C);
	//uTemp1 = (uRegValue_SRC&~(0x7))|0x0;

#if 0	//EVT0
	Outp32SYSC(0x1c, uTemp1);	// Clock Source Change
	Outp32SYSC(0x20, uTemp);	// Change Clock Divider
#endif

	//PLL Change
	Outp32SYSC(0x10, uRegValue_MPLL);

	while(((Inp32SYSC(0x10)>>30)&0x1)==0);

#if 0	//EVT0
	// Change Divider Value
	Outp32SYSC(0x20, uRegValue_DIV0);
	// Change Clock SRC
	Outp32SYSC(0x1c, uRegValue_SRC);
#endif

	SYSC_GetClkInform();
	UART_InitDebugCh(0, 115200);
//	Delay(100);

	uRet = MemoryRWTest1(0x52000000, 0x52100000);

	if(uRet)
		UART_Printf("Memory Read write Test is FAIL!\n");
	else	
		UART_Printf("Memory Read write Test is OK!\n");

 	UART_Printf("============================ \n");
	UART_Printf("Current Clock Information \n");
	UART_Printf("ARMCLK: %.2fMHz  HCLKx2: %.2fMHz  HCLK: %.2fMHz  PCLK: %.2fMHz\n",(float)g_ARMCLK/1.0e6, (float)g_HCLKx2/1.0e6, (float)g_HCLK/1.0e6, (float)g_PCLK/1.0e6);
	UART_Printf("============================ \n");
	UART_TxEmpty();

#if 0
	UART_Printf("============================ \n");
	UART_Printf("Current Clock Information \n");
	UART_Printf("ARMCLK: %.2fMHz  HCLKx2: %.2fMHz  HCLK: %.2fMHz  PCLK: %.2fMHz\n",(float)g_ARMCLK/1.0e6, (float)g_HCLKx2/1.0e6, (float)g_HCLK/1.0e6, (float)g_PCLK/1.0e6);
	UART_Printf("============================ \n");
#endif	
		
}

void Select_Random_DIVN_PMS(void)
{
	//u32 iARMCLK=0, iHCLK=0, iPCLK=0;
	float fARMCLK=0 ,fHCLKx2=0, fHCLK=0, fPCLK=0;
	float fARMCLK_0=0 ,fHCLKx2_0=0, fHCLK_0=0, fPCLK_0=0;	
	u32 u_iHCLK_DIVN_R = 1;
	
	while(1)
	{

		g_iARMCLK_DIVN_R  = ( rand()%16 );
		g_iHCLKx2_DIVN_R    = ( rand()%8 );
		g_iPCLK_DIVN_R    = (( rand()%15 )+1);   // 1~15
		
		fARMCLK = (float)(g_iAPLL   /  (g_iARMCLK_DIVN_R+1));
		fHCLKx2   = (float)(g_iAPLL /  (g_iHCLKx2_DIVN_R+1));
		fHCLK   =(float)(fHCLKx2   / (u_iHCLK_DIVN_R +1));
		fPCLK   = (float)(fHCLKx2  / (g_iPCLK_DIVN_R+1));
		
		
		if ( fARMCLK<=540 & fHCLK <= 133 & fPCLK <= 66.5 )		
		//if ( fARMCLK<=540 & fHCLK <= 133 & fPCLK <= 40 )
		{
		 	g_iRoop ++;
			
			if ( g_iRoop3 >= 2 )
			{
				fARMCLK_0 = (float)(g_iAPLL_0   /  (g_iARMCLK_DIVN_R+1));
				fHCLKx2_0   = (float)(g_iAPLL_0 /  (g_iHCLKx2_DIVN_R+1));
				fHCLK_0	 = (float)(fHCLKx2_0 / (u_iHCLK_DIVN_R+1));
				fPCLK_0   = (float)(fHCLKx2_0   / (g_iPCLK_DIVN_R+1));

				if ( fARMCLK_0 <=540 & fHCLK_0 <= 133 & fPCLK_0 <= 66.5 )		
				{
				 	g_iFCLK = g_iAPLL*1000000;
					UART_Printf("Previous PLL Freauency = %dMhz\n", g_iAPLL_0);
					//UART_TxEmpty(1);
				 	g_iAPLL_0 = g_iAPLL;				
				 	break;
				}
			}
			else 
			{
				g_iAPLL_0 = g_iAPLL;		
				g_iFCLK = g_iAPLL*1000000;
				break;
				
			}
		}
	}
}



void Compare_PMS_Logic(void)
{
	if ( g_iRoop3 >= 2 )
	{	
		iCompare_PMS[g_iCompare_0] = iCompare_PMS[g_iCompare_0] & (~(1<<g_iCompare_1)) ; 
		g_iSum = 0;
		for ( g_iRoop1 = 0 ; g_iRoop1 < g_NumPLL; g_iRoop1++ )
		{
			g_iSum = g_iSum + ((0xffffffff>>(32-g_NumPLL)) &  iCompare_PMS[g_iRoop1]);
		}
		if ( g_iSum == 0 )
		{
			UART_Printf(" \nTest End!!! -> %d Rotation Testing\n " , g_iRoop3 );
			UART_Printf(" \nTest End!!! -> %d th Test \n " , g_iRoop6 );
			g_iRoop6++;

			for ( g_iRoop4 = 0 ; g_iRoop4 < g_NumPLL ; g_iRoop4++ )
			{
				iCompare_PMS[g_iRoop4] = 0x7ffffff;
			}
			//if ( g_iRoop6 == 4 )
			if( g_iRoop6 == 5)
			{
				//UART_Printf(" 3th Test END !! \n");
				UART_Printf(" 4th Test END !! \n");
				g_TestEnd = 1;
				//UART_Getc();	
			}
		}
		g_iCompare_0 = g_iCompare_1;
	}
	else
	{
		g_iCompare_0 = g_iCompare_1;
	}

}





void Return_Condition(void)
{

	u32 uTemp, uTemp1,uRegValue_APLL, uRegValue_DIV0, uRegValue_SRC;
	
	g_iPvalue = 3;
	g_iMvalue = 400;
	g_iSvalue = 2;
	
	g_iARMCLK_DIVN_R = 0;
	g_iHCLKx2_DIVN_R   = 1;
	g_iPCLK_DIVN_R   = 3;


	//SYSC_SetDIV0(g_iARMCLK_DIVN_R, 1, 1, g_iHCLKx2_DIVN_R,  g_iPCLK_DIVN_R, 1, 1,0, 1, 0);
	//SYSC_SetPLL(eAPLL, g_iMvalue, g_iPvalue, g_iSvalue, 0 );	

	// EVT0 - 07.02.28
	uRegValue_APLL =  (u32)(((u32)(0x1<<31))|(g_iMvalue<<16)|(g_iPvalue<<8)|(g_iSvalue<<0));
	
	// Clock Divider Change 1:2:2
	uRegValue_DIV0 = Inp32SYSC(0x20);
	uTemp = (uRegValue_DIV0&~(0xFFFF))|(1<<12)|((0<<9)|(1<<8)|(1<<4)|(0<<0));		// ARM:HCLKx2:HCLK:PCLK = 1:1:2:2
	uRegValue_DIV0 = (uRegValue_DIV0&~(0xFFFF))|(g_iPCLK_DIVN_R<<12)|((g_iHCLKx2_DIVN_R<<9)|(1<<8)|(1<<4)|(g_iARMCLK_DIVN_R<<0));

	uRegValue_SRC = Inp32SYSC(0x1C);
	uTemp1 = (uRegValue_SRC&~(0x7))|0x0;

	Outp32SYSC(0x1c, uTemp1);	// Clock Source Change
	Outp32SYSC(0x20, uTemp);	// Change Clock Divider


	//PLL Change
	Outp32SYSC(0xc, uRegValue_APLL);

	while(((Inp32SYSC(0xc)>>30)&0x1)==0);

	// Change Divider Value
	Outp32SYSC(0x20, uRegValue_DIV0);
	// Change Clock SRC
	Outp32SYSC(0x1c, uRegValue_SRC);
	Delay(100);
	
	SYSC_GetClkInform(); 
	UART_InitDebugCh(0, 115200);
	Delay(100);
	UART_Printf(" Re Back_UP\n");
	
	UART_Printf("============================ \n");
	UART_Printf("Current Clock Information \n");
	UART_Printf("ARMCLK: %.2fMHz  HCLKx2: %.2fMHz  HCLK: %.2fMHz  PCLK: %.2fMHz\n",(float)g_ARMCLK/1.0e6, (float)g_HCLKx2/1.0e6, (float)g_HCLK/1.0e6, (float)g_PCLK/1.0e6);
	UART_Printf("============================ \n");
	
	
	for ( g_iRoop1 = 0 ; g_iRoop1 < 32 ; g_iRoop1++ )
	{
		iCompare_R[g_iRoop1] = 0x7ff;
	}
	for ( g_iRoop4 = 0 ; g_iRoop4 < 18 ; g_iRoop4++ )
	{
		iCompare_PMS[g_iRoop4] = 0x3ffff;
	}
	
	g_iAPLL   = 400;
	g_iAPLL_0 = 400;
	g_iCompare_0 = 0;
	g_iCompare_1 = 0;
	g_iSum = 0;
	g_iRoop = 0;
	g_iRoop1 = 0;
	g_iRoop2 = 0;
	g_iRoop3 = 0;
	g_iRoop4 = 0;
	g_iRoop5 = 0;
	g_iRoop6 = 0;
	g_iRoop7 = 0;
		
	Delay(100);

}

void Select_Random_DIVN(void)
{
	//u32 iARMCLK=0,iHCLKx2=0, iHCLK=0, iPCLK=0;
	float fARMCLK=0 ,fHCLKx2=0, fHCLK=0, fPCLK=0;
	u32 u_iHCLK_DIVN_R=1;
	
	g_iAPLL_0 = g_iAPLL;
	
	while(1)
	{

		//for test
		DisplayLED(g_iRoop/1000);
		if(UART_GetKey()!=0x0) 
			break;
		if(g_iRoop5==10) 
			break;
		/*
		if ((g_iRoop%10000)==0)
		{	
			SYSC_GetClkInform();		
			UART_InitDebugCh(0, 115200);
			Delay(1000);
			UART_Printf("\n%d \n", g_iRoop);
		}
		*/

	
		//g_iARMCLK_DIVN_R  = ( rand()%16 );
		g_iARMCLK_DIVN_R = 0;					// EVT0, Sync Mode...
		g_iHCLKx2_DIVN_R   = ( (rand()%7 )+1 );	// Sync. Min. 1
		g_iPCLK_DIVN_R    = (( rand()%15 )+1);		// PCLK  =< HCLK
		
		//fARMCLK = (float)(g_iAPLL   /  (g_iARMCLK_DIVN_R+1));
		// EVT0 - Sync. Mode
		fARMCLK = (float)(g_iAPLL   /  (0+1));
		fHCLKx2   = (float)(g_iAPLL /  (g_iHCLKx2_DIVN_R+1));
		fHCLK   =(float)(fHCLKx2   / (u_iHCLK_DIVN_R +1));
		fPCLK   = (float)(fHCLKx2  / (g_iPCLK_DIVN_R+1));

		if(((g_iPCLK_DIVN_R%2)==1) &&( fARMCLK<=540 & fHCLK <= 133 & fPCLK <= 66.5 )	)
		//if ( fARMCLK<=540 & fHCLK <= 133 & fPCLK <= 66.5 )		
		{
		 	g_iRoop ++;
			break;  	
		}
	}

	//UART_Printf("Random Choice= HCLKx2 Ratio = %d, PCLK Ratio = %d \n", g_iHCLKx2_DIVN_R, g_iPCLK_DIVN_R);
}

void Compare_Logic_DIVN(void)
{
	if ( g_iRoop == 1 )
	{
		g_iCompare_0 = iDivn[g_iARMCLK_DIVN_R][g_iHCLKx2_DIVN_R][g_iPCLK_DIVN_R];
	}
	else 
	{
		g_iCompare_1 = iDivn[g_iARMCLK_DIVN_R][g_iHCLKx2_DIVN_R][g_iPCLK_DIVN_R] ;
	}

	if ( g_iRoop >= 2 )
	{	
		//org
		iCompare_R[g_iCompare_0] = iCompare_R[g_iCompare_0] & (~(1<<g_iCompare_1)) ; 
		//iCompare_R[g_iCompare_0] = iCompare_R[g_iCompare_0] & (~(1<<(g_iRoop%12))) ; 
		g_iSum = 0;
		for ( g_iRoop1 = 0 ; g_iRoop1 < 32 ;g_iRoop1++ )
		{
			g_iSum = g_iSum + iCompare_R[g_iRoop1];
		}
		if (g_iSum == 0 )
		{
				
			UART_Printf("\n------------------> Test End!!! %d Rotation \n " , g_iRoop );
			g_iRoop5++;
			for ( g_iRoop1 = 0 ; g_iRoop1 < 32 ; g_iRoop1++ )
			{
				iCompare_R[g_iRoop1] = 0x7ff;
			}
			UART_Printf("------------------> %d th Test OK \n", g_iRoop5);
			if ( g_iRoop5 == 10 )
			{
				//UART_Getc();	
				UART_Printf("Change and Retest another Chip\n");
				//g_iRoop5 = 0;
				
			}
		}
		g_iCompare_0 = g_iCompare_1;
	}
	
}

/*
void Data_Compare_Init(void)
{	// ARM_Ratio, HCLKx2 Ratio,  PCLK Ratio
	iDivn[0][0][3] = 0;
	iDivn[1][1][4] = 1;
	iDivn[2][1][5] = 2;
	iDivn[3][2][6] = 3;
	iDivn[4][2][7] = 4;
	iDivn[5][3][8] = 5;
	iDivn[6][3][9] = 6;
	iDivn[7][4][10] = 7;
	iDivn[8][4][11] = 8;
	iDivn[9][5][12] = 9;
	iDivn[10][5][13] = 10;
	iDivn[11][6][14] = 11;
	iDivn[12][6][15] = 12;
	iDivn[13][7][3] = 13;
	iDivn[14][7][4] = 14;
	iDivn[15][8][5] = 15;
	iDivn[0][8][6] = 16;
	iDivn[1][9][7] = 17;
	iDivn[2][9][8] = 18;
	iDivn[3][10][9] = 19;
	iDivn[4][10][10] = 20;
	iDivn[5][11][11] = 21;
	iDivn[6][11][12] = 22;
	iDivn[7][12][13] = 23;
	iDivn[8][12][14] = 24;
	iDivn[9][13][15] = 25;
	iDivn[10][13][15] = 26;
	iDivn[11][14][15] = 27;
	iDivn[12][14][15] = 28;
	iDivn[13][15][15] = 29;
	iDivn[14][0][15] = 30;
	iDivn[15][15][15] = 31;
	
}
*/
// Sync Mode = HCLKx2 (1~15),  PCLK = Odd Number, 
void Data_Compare_Init(void)
{	// ARM_Ratio, HCLKx2 Ratio,  PCLK Ratio
	iDivn[0][1][3] = 0;
	iDivn[0][1][5] = 1;
	iDivn[0][1][7] = 2;
	iDivn[0][1][9] = 3;
	iDivn[0][1][11] = 4;
	iDivn[0][1][13] = 5;
	iDivn[0][1][15] = 6;
	iDivn[0][2][3] = 7;
	iDivn[0][2][5] = 8;
	iDivn[0][2][7] = 9;
	iDivn[0][2][9] = 10;
	iDivn[0][2][11] = 11;
	iDivn[0][2][13] = 12;
	iDivn[0][2][15] = 13;
	iDivn[0][3][1] = 14;
	iDivn[0][3][3] = 15;
	iDivn[0][3][5] = 16;
	iDivn[0][3][7] = 17;
	iDivn[0][4][9] = 18;
	iDivn[0][4][11] = 19;
	iDivn[0][4][13] = 20;
	iDivn[0][4][15] = 21;
	iDivn[0][5][1] = 22;
	iDivn[0][5][3] = 23;
	iDivn[0][5][5] = 24;
	iDivn[0][6][7] = 25;
	iDivn[0][6][9] = 26;
	iDivn[0][6][11] = 27;
	iDivn[0][7][13] = 28;
	iDivn[0][7][15] = 29;
	iDivn[0][7][1] = 30;
	iDivn[0][7][3] = 31;
	
}

//////////
// File Name : Test_PMS_Value_Change
// File Description : This function is for PLL Test.  The PMS Value of the APLL is changed randomly
//				 
// Input : 			
//
//						
// Output : NONE
// Version : V0.1 boaz
void Test_PMS_Value_Change(void)
{
	u32 uSelPLL;
	u32 uOpMode;
	
	Insert_Seed(); 

	UART_Printf("Choose  PLL  [0] APLL,  [1] MPLL :\n");
	uSelPLL = UART_GetIntNum();
	

	if(uSelPLL==0) {
		UART_Printf("Choose  Operating Mode [0] ASYNC Mode,  [1] SYNC Mode: \n");
		uOpMode = UART_GetIntNum();
		if(uOpMode==0) SYSC_ChangeMode(eASYNC_MODE);
		else if(uOpMode==1) SYSC_ChangeMode(eSYNC_MODE);
		else Assert(0);				
	}	

	UART_Printf("Press any key to stop Test\n\n");

	// CLK Output PAD Enable
	
	SYSC_SetLockTime(eAPLL, 300);
	SYSC_SetLockTime(eMPLL, 300);


	switch(uSelPLL)
	{
		case 0:
				SYSC_CtrlCLKOUT( eCLKOUT_APLLOUT, 1); // for clock test, jangwj
				while(!UART_GetKey())
			   	{
					Select_PMS_Random();
					Apply_PMS();
					Compare_PMS_Logic();
			   	}
		 		break;
		case 1:
				SYSC_CtrlCLKOUT( eCLKOUT_HCLK, 1); // for clock test, jangwj
				while(!UART_GetKey())
			   	{
					Select_PMS_Random_M();
					Apply_PMS_M();
					Compare_PMS_Logic();
			   	}
				   		
				break;
	}

		Return_Condition();	
	
}


void Test_PMS_Value_Change_MPLL(void)
{
	Insert_Seed(); 
		
	UART_Printf("\n");
	UART_Printf("If You Start Test, Press Any Key!!!\n");
	UART_Printf("Press 'x' : Stop Test\n\n");
	UART_Getc();

	// CLK Output PAD Enable
	SYSC_CtrlCLKOUT( eCLKOUT_HCLK, 4); // for clock test, jangwj

	SYSC_SetLockTime(eAPLL, 100);
	SYSC_SetLockTime(eMPLL, 100);
	
	while(!UART_GetKey())
   	{
		Select_PMS_Random_M();
		Apply_PMS_M();
		Compare_PMS_Logic();
   	}
   	Return_Condition();
}




//////////
// File Name : Test_Random_DIVN
// File Description : This function is  sub test function for "Test_DIVN_Value_Change"
//				 
// Input : 			
//
//						
// Output : NONE
// Version : 
void Test_Random_DIVN(void)
{

		g_iAPLL = 532;  // for test

		Select_Random_DIVN();
		
		//SYSC_SetDIV0(g_iARMCLK_DIVN_R, 1, 1, g_iHCLKx2_DIVN_R,  g_iPCLK_DIVN_R, 1, 1,0, 1, 0);
		//Sync Mode
		SYSC_SetDIV0(0, 1, 1, g_iHCLKx2_DIVN_R,  g_iPCLK_DIVN_R, 1, 1,0, 1, 0);
		SYSC_GetClkInform(); 
		UART_InitDebugCh(0, 115200);
		Delay(100);
/*
		UART_Printf("HCLKx2 Ratio = %d, PCLK Ratio = %d \n", g_iHCLKx2_DIVN_R, g_iPCLK_DIVN_R);

		UART_Printf("============================ \n");
		UART_Printf("Current Clock Information \n");
		UART_Printf("ARMCLK: %.2fMHz  HCLKx2: %.2fMHz  HCLK: %.2fMHz  PCLK: %.2fMHz\n",(float)g_ARMCLK/1.0e6, (float)g_HCLKx2/1.0e6, (float)g_HCLK/1.0e6, (float)g_PCLK/1.0e6);
		UART_Printf("============================ \n");
*/
		g_iFCLK = g_iAPLL * 1000000;
				
		Compare_Logic_DIVN();
}

//////////
// File Name : Test_DIVISION_Value_Change_ASync
// File Description : This function is for Division Change Test.  The division number will be changed in Sync. mode
//				 
// Input : 			
//
//						
// Output : NONE
// Version : V0.1 boaz
void Test_DIVISION_Value_Change_ASync(void)
{
	u32 uSelDIV;
	u32 uOpMode;
	volatile u16 uCheck[32]={	0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 
							0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
							0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
							0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
							0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
							0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
							0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,							
							0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff};							
	u32 uRandARM, uRandHCLK2, uRandPCLK;
	u32 uDiv;
	u32 uRet;
	u16 usARM, usHCLK2, usHCLK, usPCLK;
	u32 uUnitLoop;
	u32 uAmountLoop;
	u32 uVarNum;
	u32 uShift;
	u32 uSum;
	u32 uLoop;
	
	uUnitLoop=uAmountLoop=0;

	SYSC_ChangeMode(eASYNC_MODE);
	SYSC_CtrlCLKOUT( eCLKOUT_HCLK, 1); 	
	
	Insert_Seed(); 
	UART_InitDebugCh(0, 115200);

	
 // add at 3/19

	DMC_PreChangeSDRAMParameter();

	while(1) {
		DMC_PreChangeSDRAMParameter();

		uRandARM=rand()%16;
		uRandHCLK2=rand()%8;
		uRandPCLK=rand()%8;
		
//		uRandARM = (uUnitLoop%1024)>>6;
//		uRandHCLK2 = (uUnitLoop%64)>>3;
//		uRandPCLK = uUnitLoop%8; 

		uDiv=Inp32SYSC(0x020);
		uDiv &= ~(0xf<<12|0xf<<8|0xf);
		usARM=uRandARM;
		usHCLK2=uRandHCLK2;
		usHCLK=1;					
		usPCLK=uRandPCLK*2+1;
		if(usHCLK2==0 && usPCLK==1) continue;		
		uDiv |= usPCLK<<12|usHCLK2<<9|usHCLK<<8|usARM;
		
		UART_Printf("uDiv:0x%x, ARM:%d, HCLK2:%d, HCLK:%d, PCLK:%d\n", uDiv, usARM, usHCLK2, usHCLK, usPCLK);
		UART_TxEmpty();
		
		Outp32SYSC(0x020, uDiv);

		SYSC_GetClkInform();
		
		DMC_ChangeSDRAMParameter(1, g_HCLK/1000000);


		if ( uUnitLoop >= 2 ) {	
			uVarNum= (uRandHCLK2 & 0x4) ? 16+usARM : usARM;
			uShift=((uRandHCLK2&0x3)<<3) + uRandPCLK;
			uCheck[uVarNum] &= ~(1<<uShift);
			uSum = 0;
			for ( uLoop = 0 ; uLoop < 32; uLoop++ )
				uSum = uSum + uCheck[uLoop];
			if ( uSum == 0 ) {
				uUnitLoop=0;
				uAmountLoop++;
				for ( uLoop = 0 ; uLoop < 32 ; uLoop++ )
					uCheck[uLoop] = 0xffffffff;
			}
		}
		
		DisplayLED(uUnitLoop/10);
		uUnitLoop++;	

		
		uRet = MemoryRWTest1(0x52000000, 0x52001000);

		if(uRet)
			UART_Printf("Memory Read write Test is FAIL!\n");
		else	
			UART_Printf("Memory Read write Test is OK!\n");

	 	UART_Printf("========[%d][%d]============ \n", uUnitLoop, uAmountLoop);
		UART_Printf("Current Clock Information \n");
		UART_Printf("ARMCLK: %.2fMHz  HCLKx2: %.2fMHz  HCLK: %.2fMHz  PCLK: %.2fMHz\n",(float)g_ARMCLK/1.0e6, (float)g_HCLKx2/1.0e6, (float)g_HCLK/1.0e6, (float)g_PCLK/1.0e6);
		UART_Printf("============================ \n");
		UART_TxEmpty();
//		if(uUnitLoop==1024) { uUnitLoop=0; uAmountLoop++;}
		if(UART_GetKey()=='x') UART_Getc();

	}
	
}


#if 0 // 3/18 ok
//////////
// File Name : Test_DIVISION_Value_Change_Sync
// File Description : This function is for Division Change Test.  The division number will be changed in Sync. mode
//				 
// Input : 			
//
//						
// Output : NONE
// Version : V0.1 boaz
void Test_DIVISION_Value_Change_Sync(void)
{
	u32 uSelDIV;
	u32 uOpMode;
	volatile u8 ucCheck[30]={	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
							0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
	u16 usDivider[30]={0x001, 0x011, 0x021, 0x031, 0x041, 0x051, 0x061, 0x071,
		                        0x101, 0x111, 0x121, 0x131, 0x141, 0x151, 0x161, 0x171,
		                        0x221, 0x251,
		                        0x311, 0x331, 0x351, 0x371,
		                        0x441, 0x551, 0x661, 0x771, 0x941, 0xb51, 0xd61, 0xf71};
	u32 uRand0, uRand1;
	u32 uDiv;
	u32 uRet;
	u16 usARM, usHCLK2, usHCLK, usPCLK;
	
	g_iRoop3=0;
	
	Insert_Seed(); 

	SYSC_SetLockTime(eAPLL, 300);
	SYSC_CtrlCLKOUT( eCLKOUT_HCLK, 1); 	
	SYSC_ChangeMode(eSYNC_MODE);	
	SYSC_SetDIV(eHCLKX2_RATIO, 1);
	SYSC_SetPLL(eAPLL, 266, 3, 2, 0);

//	SYSC_SetDIV(eHCLKX2_RATIO, 1);
//	SYSC_SetPLL(eAPLL, 266, 3, 2, 0);
#if 0
	 uDiv=Inp32SYSC(0x020);
	 uDiv &= ~(0xf<<12|0xf<<8|0xf);
	 uDiv |= (1<<12|0<<9|1<<8|0);
	 Outp32SYSC(0x020, uDiv);
#endif

 // add at 3/19

	DMC_PreChangeSDRAMParameter();

//	uRand0=0;
//	uRand1=0;
	while(!UART_GetKey()) {
		DMC_PreChangeSDRAMParameter();

#if 0
		uRand0 = rand()%30;
		uRand1 = rand()%8;
#else
		uRand0 = g_iRoop3/8;
		uRand1 = g_iRoop3%8;
#endif

		uDiv=Inp32SYSC(0x020);
		uDiv &= ~(0xf<<12|0xf<<8|0xf);
/*
		usPCLK=uRand1*2+1;
		usARM=(usDivider[uRand0]&0xf00)>>8;
		usHCLK2=(usDivider[uRand0]&0x70)>>4;
		usHCLK=(usDivider[uRand0]&0x1);
		if(usPCLK == usHCLK) continue;
*/
		usARM=(usDivider[uRand0]&0xf00)>>8;
		usHCLK2=(usDivider[uRand0]&0x70)>>4;
		usHCLK=(usDivider[uRand0]&0x1);
		usPCLK=uRand1*2+1;

		
		uDiv |= usPCLK<<12|usHCLK2<<9|usHCLK<<8|usARM;
		UART_Printf("uDiv:0x%x, ARM:%d, HCLK2:%d, HCLK:%d, PCLK:%d\n", uDiv, usARM, usHCLK2, usHCLK, usPCLK);
		UART_TxEmpty();
		
		Outp32SYSC(0x020, uDiv);

		SYSC_GetClkInform();
		UART_InitDebugCh(0, 115200);
		
		DMC_ChangeSDRAMParameter(1, g_HCLK/1000000);
		
		DisplayLED(g_iRoop3/10);
		g_iRoop3++;

		
		uRet = MemoryRWTest1(0x52000000, 0x52001000);

		if(uRet) {
			UART_Printf("Memory Read write Test is FAIL!\n");
			return;
		}	
		else	
			UART_Printf("Memory Read write Test is OK!\n");

	 	UART_Printf("========[%d][%d]============ \n", g_iRoop3, g_iRoop6);
		UART_Printf("Current Clock Information \n");
		UART_Printf("ARMCLK: %.2fMHz  HCLKx2: %.2fMHz  HCLK: %.2fMHz  PCLK: %.2fMHz\n",(float)g_ARMCLK/1.0e6, (float)g_HCLKx2/1.0e6, (float)g_HCLK/1.0e6, (float)g_PCLK/1.0e6);
		UART_Printf("============================ \n");
		UART_TxEmpty();


		if ( g_iRoop3 >= 2 )
		{	
			ucCheck[uRand0] = ucCheck[uRand0] & (~(1<<uRand1)) ; 
			g_iSum = 0;
			for ( g_iRoop1 = 0 ; g_iRoop1 < 30; g_iRoop1++ )
			{
				g_iSum = g_iSum + ucCheck[g_iRoop1];
			}
			if ( g_iSum == 0 )
			{
				UART_Printf(" \nTest End!!! -> %d Rotation Testing\n " , g_iRoop3 );
				UART_Printf(" \nTest End!!! -> %d th Test \n " , g_iRoop6 );
				g_iRoop6++;

				for ( g_iRoop4 = 0 ; g_iRoop4 < 30 ; g_iRoop4++ )
				{
					ucCheck[g_iRoop4] = 0xff;
				}
				//if ( g_iRoop6 == 4 )
				if( g_iRoop6 == 5)
				{
					//UART_Printf(" 3th Test END !! \n");
					UART_Printf(" 4th Test END !! \n");
					g_TestEnd = 1;
					//UART_Getc();	
				}
			}
		}
		if(g_iRoop3>240) { g_iRoop3=0; g_iRoop6++;}

	}
	
}
#else  // Random Test
//////////
// File Name : Test_DIVISION_Value_Change_Sync
// File Description : This function is for Division Change Test.  The division number will be changed in Sync. mode
//				 
// Input : 			
//
//						
// Output : NONE
// Version : V0.1 boaz
void Test_DIVISION_Value_Change_Sync(void)
{
	u32 uSelDIV;
	u32 uOpMode;
#if 1
	volatile u8 ucCheck[30]={	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
							0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
	u16 usDivider[30]={0x001, 0x011, 0x021, 0x031, 0x041, 0x051, 0x061, 0x071,
		                        0x101, 0x111, 0x121, 0x131, 0x141, 0x151, 0x161, 0x171,
		                        0x221, 0x251,
		                        0x311, 0x331, 0x351, 0x371,
		                        0x441, 0x551, 0x661, 0x771, 0x941, 0xb51, 0xd61, 0xf71};
#else
	volatile u8 ucCheck[26]={	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
							0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
	u16 usDivider[26]={0x001, 0x011, 0x021, 0x031, 0x041, 0x051, 0x061, 0x071,
								0x101, 0x111, 0x121, 0x131, 0x141, 0x151, 0x161, 0x171,
								0x221, 0x251,
								0x311, 0x331, 0x351, 0x371,
								0x441, 0x551, 0x661, 0x771};

#endif

	u32 uRand0, uRand1;
	u32 uDiv;
	u32 uRet;
	u16 usARM, usHCLK2, usHCLK, usPCLK;
	
	g_iRoop3=0;
	
	Insert_Seed(); 

	SYSC_SetLockTime(eAPLL, 300);
	SYSC_CtrlCLKOUT( eCLKOUT_HCLK, 1); 	
	SYSC_ChangeMode(eSYNC_MODE);	
	SYSC_SetDIV(eHCLKX2_RATIO, 1);
//	SYSC_SetPLL(eAPLL, 266, 3, 2, 0);
	 SYSC_SetPLL(eAPLL, 400, 3, 4, 0);


 // add at 3/19

	DMC_PreChangeSDRAMParameter();

//	uRand0=0;
//	uRand1=0;
	while(!UART_GetKey()) {
		DMC_PreChangeSDRAMParameter();

#if 0
	uRand0 = rand()%26;
	uRand1 = rand()%8;
#else
	uRand0 = rand()%30;
	uRand1 = rand()%8;

#endif

#if 0
		uRand0 = g_iRoop3/8;
		uRand1 = g_iRoop3%8;
#endif

		uDiv=Inp32SYSC(0x020);
		uDiv &= ~(0xf<<12|0xf<<8|0xf);
#if 1
		usPCLK=uRand1*2+1;
		usARM=(usDivider[uRand0]&0xf00)>>8;
		usHCLK2=(usDivider[uRand0]&0x70)>>4;
		usHCLK=(usDivider[uRand0]&0x1);
//		if(usPCLK == usHCLK) continue;
		if((usPCLK == 1) && (usHCLK2==0)) continue;
		
#else
		usARM=(usDivider[uRand0]&0xf00)>>8;
		usHCLK2=(usDivider[uRand0]&0x70)>>4;
		usHCLK=(usDivider[uRand0]&0x1);
		usPCLK=uRand1*2+1;
#endif
		
		uDiv |= usPCLK<<12|usHCLK2<<9|usHCLK<<8|usARM;
		UART_Printf("uDiv:0x%x, ARM:%d, HCLK2:%d, HCLK:%d, PCLK:%d\n", uDiv, usARM, usHCLK2, usHCLK, usPCLK);
		UART_TxEmpty();
		
		Outp32SYSC(0x020, uDiv);

		SYSC_GetClkInform();
		UART_InitDebugCh(0, 115200);
		
		DMC_ChangeSDRAMParameter(1, g_HCLK/1000000);
		
		DisplayLED(g_iRoop3/10);
		g_iRoop3++;

		
		uRet = MemoryRWTest1(0x52000000, 0x52001000);

		if(uRet) {
			UART_Printf("Memory Read write Test is FAIL!\n");
			return;
		}	
		else	
			UART_Printf("Memory Read write Test is OK!\n");

	 	UART_Printf("========[%d][%d]============ \n", g_iRoop3, g_iRoop6);
		UART_Printf("Current Clock Information \n");
		UART_Printf("ARMCLK: %.2fMHz  HCLKx2: %.2fMHz  HCLK: %.2fMHz  PCLK: %.2fMHz\n",(float)g_ARMCLK/1.0e6, (float)g_HCLKx2/1.0e6, (float)g_HCLK/1.0e6, (float)g_PCLK/1.0e6);
		UART_Printf("============================ \n");
		UART_TxEmpty();

#if 1
		if ( g_iRoop3 >= 2 )
		{	
			ucCheck[uRand0] = ucCheck[uRand0] & (~(1<<uRand1)) ; 
			g_iSum = 0;
			for ( g_iRoop1 = 0 ; g_iRoop1 < 30; g_iRoop1++ )
			{
				g_iSum = g_iSum + ucCheck[g_iRoop1];
			}
			if ( g_iSum == 0 )
			{
				UART_Printf(" \nTest End!!! -> %d Rotation Testing\n " , g_iRoop3 );
				UART_Printf(" \nTest End!!! -> %d th Test \n " , g_iRoop6 );
				g_iRoop6++;

				for ( g_iRoop4 = 0 ; g_iRoop4 < 30 ; g_iRoop4++ )
				{
					ucCheck[g_iRoop4] = 0xff;
				}
				//if ( g_iRoop6 == 4 )
				if( g_iRoop6 == 5)
				{
					//UART_Printf(" 3th Test END !! \n");
					UART_Printf(" 4th Test END !! \n");
					g_TestEnd = 1;
					//UART_Getc();	
				}
			}
		}
		//if(g_iRoop3>240) { g_iRoop3=0; g_iRoop6++;}
#else
	if ( g_iRoop3 >= 2 )
	{	
		ucCheck[uRand0] = ucCheck[uRand0] & (~(1<<uRand1)) ; 
		g_iSum = 0;
		for ( g_iRoop1 = 0 ; g_iRoop1 < 26; g_iRoop1++ )
		{
			g_iSum = g_iSum + ucCheck[g_iRoop1];
		}
		if ( g_iSum == 0 )
		{
			UART_Printf(" \nTest End!!! -> %d Rotation Testing\n " , g_iRoop3 );
			UART_Printf(" \nTest End!!! -> %d th Test \n " , g_iRoop6 );
			g_iRoop6++;

			for ( g_iRoop4 = 0 ; g_iRoop4 < 26 ; g_iRoop4++ )
			{
				ucCheck[g_iRoop4] = 0xff;
			}
			//if ( g_iRoop6 == 4 )
			if( g_iRoop6 == 5)
			{
				//UART_Printf(" 3th Test END !! \n");
				UART_Printf(" 4th Test END !! \n");
				g_TestEnd = 1;
				//UART_Getc();	
			}
		}
	}
	//if(g_iRoop3>240) { g_iRoop3=0; g_iRoop6++;}

#endif
	}
	
}

#endif



//////////
// File Name : Test_DIVISION_Value_Change_Sync_Sep
// File Description : This function is for Division Change Test.  The division number will be changed in Sync. mode
//				 
// Input : 			
//
//						
// Output : NONE
// Version : V0.1 boaz
void Test_DIVISION_Value_Change_Sync_Sep(void)
{
	u32 uSelDIV;
	u32 uOpMode;
#if 1
	volatile u8 ucCheck[32]={	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
							0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
							0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
							0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
	u16 usDivider[32]={0x001, 0x011, 0x021, 0x031, 0x041, 0x051, 0x061, 0x071,
		                        0x101, 0x111, 0x121, 0x131, 0x141, 0x151, 0x161, 0x171,
		                        0x221, 0x251,
		                        0x311, 0x331, 0x351, 0x371,
		                        0x441, 0x521, 0x551, 0x661, 0x731, 0x771, 0x941, 0xb51, 0xd61, 0xf71};
#else
	volatile u8 ucCheck[26]={	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
							0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
	u16 usDivider[26]={0x001, 0x011, 0x021, 0x031, 0x041, 0x051, 0x061, 0x071,
								0x101, 0x111, 0x121, 0x131, 0x141, 0x151, 0x161, 0x171,
								0x221, 0x251,
								0x311, 0x331, 0x351, 0x371,
								0x441, 0x551, 0x661, 0x771};

#endif

	u32 uRand0, uRand1;
	u32 uDiv;
	u32 uRet;
	u16 usARM, usHCLK2, usHCLK, usPCLK;
	u32 uClkSrc;
	u32 uGo=0;
	
	g_iRoop3=0;
	
	Insert_Seed(); 

	SYSC_SetLockTime(eAPLL, 300);
	SYSC_CtrlCLKOUT( eCLKOUT_HCLK, 1); 	
	SYSC_ChangeMode(eSYNC_MODE);	
	SYSC_SetDIV(eHCLKX2_RATIO, 7);
//	SYSC_SetPLL(eAPLL, 266, 3, 2, 0);
	 SYSC_SetPLL(eAPLL, 400, 3, 4, 0);


 // add at 3/19

	DMC_PreChangeSDRAMParameter();

//	uRand0=0;
//	uRand1=0;
	while(!UART_GetKey()) {
		DMC_PreChangeSDRAMParameter();

		uRand0 = rand()%16;

		uDiv=Inp32SYSC(0x020);
		
		usPCLK=(uDiv&(0xf<<12))>>12;
		usHCLK2=(uDiv&(0x7<<9))>>9;
		usHCLK=(uDiv&(0x1<<8))>>8;		
		
		uDiv &= ~(0xf);
//		usPCLK=uRand1*2+1;
/*
		switch(usARM) {
			case 0:
			case 1:
			case 3:
			case 7:
			case 15:
				uGo=1;
				break;
			case 2:
			case 4:
			case 5:
			case 6:
			case 8:
			case 9:
			case 10:
			case 11:
			case 12:
			case 13:
			case 14:
				uGo=0;
				break;
			default:
				uGo=0;
				break;
		}	
		if(uGo==0) continue;
		else if(uGo==1) usARM=uRand0;
*/
 
		if(uRand0==0) usARM=uRand0;
		else if(uRand0==1) usARM=uRand0;
		else if(uRand0==3) usARM=uRand0;
		else if(uRand0==7) usARM=uRand0;
		else if(uRand0==15) usARM=uRand0;
		else continue;
 

//		usHCLK2=(usDivider[uRand0]&0x70)>>4;
//		usHCLK=(usDivider[uRand0]&0x1);
//		if(usPCLK == usHCLK) continue;
//		if((usPCLK == 1) && (usHCLK2==0)) continue;
//		uDiv |= usPCLK<<12|usHCLK2<<9|usHCLK<<8|usARM;
		uDiv |= usARM;
		UART_Printf("uDiv:0x%x, ARM:%d, HCLK2:%d, HCLK:%d, PCLK:%d\n", uDiv, usARM, usHCLK2, usHCLK, usPCLK);
		UART_TxEmpty();


		///////////  Change Clock Division
		Outp32SYSC(0x020, uDiv);


		SYSC_GetClkInform();
		UART_InitDebugCh(0, 115200);
		
		DMC_ChangeSDRAMParameter(1, g_HCLK/1000000);
		
		DisplayLED(g_iRoop3/10);
		g_iRoop3++;

		
		uRet = MemoryRWTest1(0x52000000, 0x52001000);

		if(uRet) {
			UART_Printf("Memory Read write Test is FAIL!\n");
			return;
		}	
		else	
			UART_Printf("Memory Read write Test is OK!\n");

	 	UART_Printf("========[%d][%d]============ \n", g_iRoop3, g_iRoop6);
		UART_Printf("Current Clock Information \n");
		UART_Printf("ARMCLK: %.2fMHz  HCLKx2: %.2fMHz  HCLK: %.2fMHz  PCLK: %.2fMHz\n",(float)g_ARMCLK/1.0e6, (float)g_HCLKx2/1.0e6, (float)g_HCLK/1.0e6, (float)g_PCLK/1.0e6);
		UART_Printf("============================ \n");
		UART_TxEmpty();

#if 0
		if ( g_iRoop3 >= 2 )
		{	
			ucCheck[uRand0] = ucCheck[uRand0] & (~(1<<uRand1)) ; 
			g_iSum = 0;
			for ( g_iRoop1 = 0 ; g_iRoop1 < 30; g_iRoop1++ )
			{
				g_iSum = g_iSum + ucCheck[g_iRoop1];
			}
			if ( g_iSum == 0 )
			{
				UART_Printf(" \nTest End!!! -> %d Rotation Testing\n " , g_iRoop3 );
				UART_Printf(" \nTest End!!! -> %d th Test \n " , g_iRoop6 );
				g_iRoop6++;

				for ( g_iRoop4 = 0 ; g_iRoop4 < 30 ; g_iRoop4++ )
				{
					ucCheck[g_iRoop4] = 0xff;
				}
				//if ( g_iRoop6 == 4 )
				if( g_iRoop6 == 5)
				{
					//UART_Printf(" 3th Test END !! \n");
					UART_Printf(" 4th Test END !! \n");
					g_TestEnd = 1;
					//UART_Getc();	
				}
			}
		}
		//if(g_iRoop3>240) { g_iRoop3=0; g_iRoop6++;}
//#else
	if ( g_iRoop3 >= 2 )
	{	
		ucCheck[uRand0] = ucCheck[uRand0] & (~(1<<uRand1)) ; 
		g_iSum = 0;
		for ( g_iRoop1 = 0 ; g_iRoop1 < 26; g_iRoop1++ )
		{
			g_iSum = g_iSum + ucCheck[g_iRoop1];
		}
		if ( g_iSum == 0 )
		{
			UART_Printf(" \nTest End!!! -> %d Rotation Testing\n " , g_iRoop3 );
			UART_Printf(" \nTest End!!! -> %d th Test \n " , g_iRoop6 );
			g_iRoop6++;

			for ( g_iRoop4 = 0 ; g_iRoop4 < 26 ; g_iRoop4++ )
			{
				ucCheck[g_iRoop4] = 0xff;
			}
			//if ( g_iRoop6 == 4 )
			if( g_iRoop6 == 5)
			{
				//UART_Printf(" 3th Test END !! \n");
				UART_Printf(" 4th Test END !! \n");
				g_TestEnd = 1;
				//UART_Getc();	
			}
		}
	}
	//if(g_iRoop3>240) { g_iRoop3=0; g_iRoop6++;}

#endif
	}
	
}


void Test_Divider_Manual(void)
{
	u32 uDiv, usPCLK, usARM, usHCLK2, usHCLK;
	u32 uRet;
	
	SYSC_SetLockTime(eAPLL, 300);
	SYSC_CtrlCLKOUT( eCLKOUT_HCLK, 1); 	
	SYSC_ChangeMode(eSYNC_MODE);	
	SYSC_SetDIV(eHCLKX2_RATIO, 1);
	SYSC_SetPLL(eAPLL, 266, 3, 2, 0);	

	DMC_PreChangeSDRAMParameter();


	SYSC_GetClkInform();
	UART_InitDebugCh(0, 115200);
	UART_Printf("ARMCLK: %.2fMHz  HCLKx2: %.2fMHz  HCLK: %.2fMHz  PCLK: %.2fMHz\n",(float)g_ARMCLK/1.0e6, (float)g_HCLKx2/1.0e6, (float)g_HCLK/1.0e6, (float)g_PCLK/1.0e6);
	UART_TxEmpty();	
#if 0
	while(1) {
	usPCLK=1;
	usARM=4;
	usHCLK2=4;
	usHCLK=1;
	UART_Printf("uDiv:0x%x, ARM:%d, HCLK2:%d, HCLK:%d, PCLK:%d\n", uDiv, usARM, usHCLK2, usHCLK, usPCLK);
	UART_TxEmpty();	
	uDiv=Inp32SYSC(0x020);
	uDiv &= ~(0xf<<12|0xf<<8|0xf);
	uDiv |= usPCLK<<12|usHCLK2<<9|usHCLK<<8|usARM;
	Outp32SYSC(0x020, uDiv);
	
	SYSC_GetClkInform();
	UART_InitDebugCh(0, 115200);
	UART_Printf("ARMCLK: %.2fMHz  HCLKx2: %.2fMHz  HCLK: %.2fMHz  PCLK: %.2fMHz\n",(float)g_ARMCLK/1.0e6, (float)g_HCLKx2/1.0e6, (float)g_HCLK/1.0e6, (float)g_PCLK/1.0e6);
	
	usPCLK=9;
	usARM=1;
	usHCLK2=5;
	usHCLK=1;
	uDiv=Inp32SYSC(0x020);
	uDiv &= ~(0xf<<12|0xf<<8|0xf);
	uDiv |= usPCLK<<12|usHCLK2<<9|usHCLK<<8|usARM;
	UART_Printf("uDiv:0x%x, ARM:%d, HCLK2:%d, HCLK:%d, PCLK:%d\n", uDiv, usARM, usHCLK2, usHCLK, usPCLK);
	UART_TxEmpty();
	
	Outp32SYSC(0x020, uDiv);	
	SYSC_GetClkInform();
	UART_InitDebugCh(0, 115200);
	UART_Printf("ARMCLK: %.2fMHz  HCLKx2: %.2fMHz  HCLK: %.2fMHz  PCLK: %.2fMHz\n",(float)g_ARMCLK/1.0e6, (float)g_HCLKx2/1.0e6, (float)g_HCLK/1.0e6, (float)g_PCLK/1.0e6);
	UART_Getc();
	}	
#else

	while(1) {
		DMC_PreChangeSDRAMParameter();		
		usPCLK=1;
		usARM=0;
		usHCLK2=6;
		usHCLK=1;
		UART_Printf("uDiv:0x%x, ARM:%d, HCLK2:%d, HCLK:%d, PCLK:%d\n", uDiv, usARM, usHCLK2, usHCLK, usPCLK);
		UART_TxEmpty(); 
		uDiv=Inp32SYSC(0x020);
		uDiv &= ~(0xf<<12|0xf<<8|0xf);
		uDiv |= usPCLK<<12|usHCLK2<<9|usHCLK<<8|usARM;
		Outp32SYSC(0x020, uDiv);	
		DMC_ChangeSDRAMParameter(1, g_HCLK/1000000);
		
		uRet = MemoryRWTest1(0x52000000, 0x52001000);
		if(uRet)
			UART_Printf("Memory Read write Test is FAIL!\n");
		else	
			UART_Printf("Memory Read write Test is OK!\n"); 	

		UART_Printf("ARMCLK: %.2fMHz  HCLKx2: %.2fMHz  HCLK: %.2fMHz  PCLK: %.2fMHz\n",(float)g_ARMCLK/1.0e6, (float)g_HCLKx2/1.0e6, (float)g_HCLK/1.0e6, (float)g_PCLK/1.0e6);

		DMC_PreChangeSDRAMParameter();		
		usPCLK=7;
		usARM=3;
		usHCLK2=1;
		usHCLK=1;
		uDiv=Inp32SYSC(0x020);
		uDiv &= ~(0xf<<12|0xf<<8|0xf);
		uDiv |= usPCLK<<12|usHCLK2<<9|usHCLK<<8|usARM;
		UART_Printf("uDiv:0x%x, ARM:%d, HCLK2:%d, HCLK:%d, PCLK:%d\n", uDiv, usARM, usHCLK2, usHCLK, usPCLK);
		UART_TxEmpty();

		Outp32SYSC(0x020, uDiv);	
		DMC_ChangeSDRAMParameter(1, g_HCLK/1000000);	
		uRet = MemoryRWTest1(0x52000000, 0x52001000);
		if(uRet)
			UART_Printf("Memory Read write Test is FAIL!\n");
		else	
			UART_Printf("Memory Read write Test is OK!\n"); 
		SYSC_GetClkInform();
		UART_InitDebugCh(0, 115200);
		UART_Printf("ARMCLK: %.2fMHz  HCLKx2: %.2fMHz  HCLK: %.2fMHz  PCLK: %.2fMHz\n",(float)g_ARMCLK/1.0e6, (float)g_HCLKx2/1.0e6, (float)g_HCLK/1.0e6, (float)g_PCLK/1.0e6);
	}


#endif

}


//////////
// File Name : Test_Division_withExternal
// File Description : This function is for Division Change Test.  The division number will be changed in Sync. mode
//				 
// Input : 			
//
//						
// Output : NONE
// Version : V0.1 boaz
void Test_DIVISION_withExternal(void)
{
	u32 uSelDIV;
	u32 uOpMode;
#if 1
	volatile u8 ucCheck[32]={	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
							0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
							0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
							0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
	u16 usDivider[32]={0x001, 0x011, 0x021, 0x031, 0x041, 0x051, 0x061, 0x071,
		                        0x101, 0x111, 0x121, 0x131, 0x141, 0x151, 0x161, 0x171,
		                        0x221, 0x251,
		                        0x311, 0x331, 0x351, 0x371,
		                        0x441, 0x521, 0x551, 0x661, 0x731, 0x771, 0x941, 0xb51, 0xd61, 0xf71};
#else
	volatile u8 ucCheck[26]={	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
							0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
	u16 usDivider[26]={0x001, 0x011, 0x021, 0x031, 0x041, 0x051, 0x061, 0x071,
								0x101, 0x111, 0x121, 0x131, 0x141, 0x151, 0x161, 0x171,
								0x221, 0x251,
								0x311, 0x331, 0x351, 0x371,
								0x441, 0x551, 0x661, 0x771};

#endif

	u32 uRand0, uRand1;
	u32 uDiv;
	u32 uRet;
	u16 usARM, usHCLK2, usHCLK, usPCLK;
	u32 uClkSrc;
	
	g_iRoop3=0;
	
	Insert_Seed(); 

	SYSC_SetLockTime(eAPLL, 300);
	SYSC_CtrlCLKOUT( eCLKOUT_HCLK, 1); 	
	SYSC_ChangeMode(eSYNC_MODE);	
	SYSC_SetDIV(eHCLKX2_RATIO, 1);
	SYSC_SetPLL(eAPLL, 266, 3, 2, 0);
//	 SYSC_SetPLL(eAPLL, 400, 3, 4, 0);


 // add at 3/19

	DMC_PreChangeSDRAMParameter();

//	uRand0=0;
//	uRand1=0;
	while(!UART_GetKey()) {
		DMC_PreChangeSDRAMParameter();

		uRand0 = rand()%32;
		uRand1 = rand()%8;

		uDiv=Inp32SYSC(0x020);
		uDiv &= ~(0xf<<12|0xf<<8|0xf);
		usPCLK=uRand1*2+1;
		usARM=(usDivider[uRand0]&0xf00)>>8;
		usHCLK2=(usDivider[uRand0]&0x70)>>4;
		usHCLK=(usDivider[uRand0]&0x1);
	//	if(usPCLK == usHCLK) continue;
		if((usPCLK == 1) && (usHCLK2==0)) continue;
		uDiv |= usPCLK<<12|usHCLK2<<9|usHCLK<<8|usARM;
		UART_Printf("uDiv:0x%x, ARM:%d, HCLK2:%d, HCLK:%d, PCLK:%d\n", uDiv, usARM, usHCLK2, usHCLK, usPCLK);
		UART_TxEmpty();


		///////////  Change System Clock to External Clock
		uClkSrc = Inp32SYSC(0x01c);
		uClkSrc &= ~(1);
		Outp32SYSC(0x01c, uClkSrc);

		///////////  Change Clock Division
		Outp32SYSC(0x020, uDiv);


		///////////  Change System Clock to PLL Out
		uClkSrc = Inp32SYSC(0x01c);
		uClkSrc |= 1;
		Outp32SYSC(0x01c, uClkSrc);


		SYSC_GetClkInform();
		UART_InitDebugCh(0, 115200);
		
		DMC_ChangeSDRAMParameter(1, g_HCLK/1000000);
		
		DisplayLED(g_iRoop3/10);
		g_iRoop3++;

		
		uRet = MemoryRWTest1(0x52000000, 0x52001000);

		if(uRet) {
			UART_Printf("Memory Read write Test is FAIL!\n");
			return;
		}	
		else	
			UART_Printf("Memory Read write Test is OK!\n");

	 	UART_Printf("========[%d][%d]============ \n", g_iRoop3, g_iRoop6);
		UART_Printf("Current Clock Information \n");
		UART_Printf("ARMCLK: %.2fMHz  HCLKx2: %.2fMHz  HCLK: %.2fMHz  PCLK: %.2fMHz\n",(float)g_ARMCLK/1.0e6, (float)g_HCLKx2/1.0e6, (float)g_HCLK/1.0e6, (float)g_PCLK/1.0e6);
		UART_Printf("============================ \n");
		UART_TxEmpty();

#if 1
		if ( g_iRoop3 >= 2 )
		{	
			ucCheck[uRand0] = ucCheck[uRand0] & (~(1<<uRand1)) ; 
			g_iSum = 0;
			for ( g_iRoop1 = 0 ; g_iRoop1 < 30; g_iRoop1++ )
			{
				g_iSum = g_iSum + ucCheck[g_iRoop1];
			}
			if ( g_iSum == 0 )
			{
				UART_Printf(" \nTest End!!! -> %d Rotation Testing\n " , g_iRoop3 );
				UART_Printf(" \nTest End!!! -> %d th Test \n " , g_iRoop6 );
				g_iRoop6++;

				for ( g_iRoop4 = 0 ; g_iRoop4 < 30 ; g_iRoop4++ )
				{
					ucCheck[g_iRoop4] = 0xff;
				}
				//if ( g_iRoop6 == 4 )
				if( g_iRoop6 == 5)
				{
					//UART_Printf(" 3th Test END !! \n");
					UART_Printf(" 4th Test END !! \n");
					g_TestEnd = 1;
					//UART_Getc();	
				}
			}
		}
		//if(g_iRoop3>240) { g_iRoop3=0; g_iRoop6++;}
#else
	if ( g_iRoop3 >= 2 )
	{	
		ucCheck[uRand0] = ucCheck[uRand0] & (~(1<<uRand1)) ; 
		g_iSum = 0;
		for ( g_iRoop1 = 0 ; g_iRoop1 < 26; g_iRoop1++ )
		{
			g_iSum = g_iSum + ucCheck[g_iRoop1];
		}
		if ( g_iSum == 0 )
		{
			UART_Printf(" \nTest End!!! -> %d Rotation Testing\n " , g_iRoop3 );
			UART_Printf(" \nTest End!!! -> %d th Test \n " , g_iRoop6 );
			g_iRoop6++;

			for ( g_iRoop4 = 0 ; g_iRoop4 < 26 ; g_iRoop4++ )
			{
				ucCheck[g_iRoop4] = 0xff;
			}
			//if ( g_iRoop6 == 4 )
			if( g_iRoop6 == 5)
			{
				//UART_Printf(" 3th Test END !! \n");
				UART_Printf(" 4th Test END !! \n");
				g_TestEnd = 1;
				//UART_Getc();	
			}
		}
	}
	//if(g_iRoop3>240) { g_iRoop3=0; g_iRoop6++;}

#endif
	}
	
}




//////////
// File Name : Test_DIVN_Value_Change
// File Description : This function is for clock divider test.  The clock divider Values  are changed randomly
//				 
// Input : 			
//
//						
// Output : NONE
// Version : 
void Test_DIVN_Value_Change(void)
{

	// CLK Output PAD Enable
	SYSC_CtrlCLKOUT( eCLKOUT_HCLK, 9); // for clock test, jangwj


	UART_Printf("If You Start Test, Press Any Key!!!\n");
	UART_Printf("Press 'x' : Stop Test\n\n");


	UART_Getc();

/*
	while (1)
	{
		UART_Printf("*********** TEST ************\n");
		UART_Printf("Input Operation MPLL Clock (266Mhz or 200Mhz) = ");
		iMPLL = UART_GetIntNum();
		UART_Printf("\n");
		
		if ( iMPLL == 200 )
		{
			Data_Compare_Init();
			break;
		}
		
		if (iMPLL == 266 )
		{
			Data_Compare_Init_266();
			Setting_PMS_266();
			break;
		}
	}
*/	
	Data_Compare_Init();
	Insert_Seed();
		
	while(!UART_GetKey())
   	{
 		Test_Random_DIVN();
   	}	

   	Return_Condition();
}


void Select_PMS_Random_EVT1_1(void)
{
	g_iRandom_PMS = rand()%18;

	DisplayLED(g_iRoop3/10);
	g_iRoop3++;
	switch ( g_iRandom_PMS )
	{
		case 0:
			g_iPvalue = 6;
			g_iMvalue = 500;
			g_iSvalue = 1;
			g_iCompare_1 = g_iRandom_PMS;
			g_iAPLL = 500;						//  VCO = 1000MHz
			break;
		case 1:
			g_iPvalue = 6;
			g_iMvalue = 532;
			g_iSvalue = 1;
			g_iCompare_1 = g_iRandom_PMS;
			g_iAPLL = 532;						// VCO = 1064MHz
			break;
		case 2:
			g_iPvalue = 6;
			g_iMvalue = 550;
			g_iSvalue = 2;
			g_iCompare_1 = g_iRandom_PMS;
			g_iAPLL = 275;					    // VCO = 1100MHz
			break;
		case 3:
			g_iPvalue = 6;
			g_iMvalue = 600;
			g_iSvalue = 2;
			g_iCompare_1 = g_iRandom_PMS;
			g_iAPLL = 300;					// VCO = 1200MHz, for 600MHz
			break;
		case 4:
			g_iPvalue = 6;
			g_iMvalue = 650;
			g_iSvalue= 2;
			g_iCompare_1 = g_iRandom_PMS;
			g_iAPLL = 325;					// VCO = 1300MHz
			break;			
		case 5:
			g_iPvalue = 6;
			g_iMvalue= 700;
			g_iSvalue = 2;
			g_iCompare_1 = g_iRandom_PMS;
			g_iAPLL = 350;					// VCO = 1400MHz
			break;			
		case 6:
			g_iPvalue = 6;
			g_iMvalue = 750;
			g_iSvalue = 2;
			g_iCompare_1 = g_iRandom_PMS;
			g_iAPLL = 375;					// VCO = 1500MHz				
			break;			
		case 7:
			g_iPvalue = 6;
			g_iMvalue= 800;
			g_iSvalue= 2;
			g_iCompare_1 = g_iRandom_PMS;
			g_iAPLL = 400;					// VCO = 1600MHz
			break;			
		case 8:
			g_iPvalue = 3;
			g_iMvalue = 400;
			g_iSvalue = 2;
			g_iCompare_1 = g_iRandom_PMS;
			g_iAPLL = 400;				   // VCO = 1600MHz
			break;			
		case 9:
			g_iPvalue = 3;
			g_iMvalue = 425;
			g_iSvalue = 2;
			g_iCompare_1 = g_iRandom_PMS;
			g_iAPLL = 425;				// VCO = 1700MHz
			break;			
		case 10:
			g_iPvalue = 3;
			g_iMvalue = 450;
			g_iSvalue = 2;
			g_iCompare_1 = g_iRandom_PMS;
			g_iAPLL = 450;				// VCO = 1800MHz
			break;			
		case 11:
			g_iPvalue = 3;
			g_iMvalue = 475;
			g_iSvalue = 2;
			g_iCompare_1 = g_iRandom_PMS;
			g_iAPLL = 475;				// VCO = 1900MHz
			break;			
		case 12:
			g_iPvalue = 3;
			g_iMvalue = 500;
			g_iSvalue = 2;
			g_iCompare_1 = g_iRandom_PMS;
			g_iAPLL = 500;			   // VCO = 2000MHz
			break;			
		case 13:
			g_iPvalue = 3;
			g_iMvalue = 532;
			g_iSvalue = 2;
			g_iCompare_1 = g_iRandom_PMS;
			g_iAPLL = 532;			// VCO = 2128MHz
			break;			
		case 14:
			g_iPvalue = 6;
			g_iMvalue = 667;
			g_iSvalue = 2;
			g_iCompare_1 = g_iRandom_PMS;
			g_iAPLL = 333;			// VCO = 1334MHz,  for 667MHz
			break;			
		case 15:
			g_iPvalue = 2;
			g_iMvalue = 150;
			g_iSvalue = 2;
			g_iCompare_1 = g_iRandom_PMS;
			g_iAPLL = 225;		    // VCO= 900MHz   for 225MHz
			break;			
		case 16:
			g_iPvalue = 3;
			g_iMvalue = 520;
			g_iSvalue = 2;
			g_iCompare_1 = g_iRandom_PMS;
			g_iAPLL = 520;		// VCO = 2080MHz  for 520MHz
			break;			
		case 17:
			g_iPvalue = 3;
			g_iMvalue = 200;
			g_iSvalue = 2;
			g_iCompare_1 = g_iRandom_PMS;
			g_iAPLL = 200;		// VCO=800MHz
			break;			
		default :

			break;
	}
}


void Select_PMS_Random_EVT1_2(void)
{
	g_iRandom_PMS = rand()%18;

	DisplayLED(g_iRoop3/10);
	g_iRoop3++;
	switch ( g_iRandom_PMS )
	{
		case 0:
			g_iPvalue = 3;
			g_iMvalue = 390;
			g_iSvalue = 2;
			g_iCompare_1 = g_iRandom_PMS;
			g_iAPLL = 390;						//  VCO = 1560MHz
			break;
		case 1:
			g_iPvalue = 3;
			g_iMvalue = 532;
			g_iSvalue = 2;
			g_iCompare_1 = g_iRandom_PMS;
			g_iAPLL = 532;						// VCO = 2128MHz
			break;
		case 2:
			g_iPvalue = 6;
			g_iMvalue = 850;
			g_iSvalue = 2;
			g_iCompare_1 = g_iRandom_PMS;
			g_iAPLL = 425;					    // VCO = 1700MHz
			break;
		case 3:
			g_iPvalue = 6;
			g_iMvalue = 900;
			g_iSvalue = 2;
			g_iCompare_1 = g_iRandom_PMS;
			g_iAPLL = 450;					// VCO = 1800MHz, for 600MHz
			break;
		case 4:
			g_iPvalue = 6;
			g_iMvalue = 950;
			g_iSvalue= 2;
			g_iCompare_1 = g_iRandom_PMS;
			g_iAPLL = 475;					// VCO = 1900MHz
			break;			
		case 5:
			g_iPvalue = 6;
			g_iMvalue= 1000;
			g_iSvalue = 2;
			g_iCompare_1 = g_iRandom_PMS;
			g_iAPLL = 500;					// VCO = 2000MHz
			break;			
		case 6:
			g_iPvalue = 3;
			g_iMvalue = 400;
			g_iSvalue = 2;
			g_iCompare_1 = g_iRandom_PMS;
			g_iAPLL = 400;					// VCO = 1600MHz				
			break;			
		case 7:
			g_iPvalue = 6;
			g_iMvalue= 800;
			g_iSvalue= 2;
			g_iCompare_1 = g_iRandom_PMS;
			g_iAPLL = 400;					// VCO = 1600MHz
			break;			
		case 8:
			g_iPvalue = 3;
			g_iMvalue = 400;
			g_iSvalue = 2;
			g_iCompare_1 = g_iRandom_PMS;
			g_iAPLL = 400;				   // VCO = 1600MHz
			break;			
		case 9:
			g_iPvalue = 3;
			g_iMvalue = 425;
			g_iSvalue = 2;
			g_iCompare_1 = g_iRandom_PMS;
			g_iAPLL = 425;				// VCO = 1700MHz
			break;			
		case 10:
			g_iPvalue = 3;
			g_iMvalue = 450;
			g_iSvalue = 2;
			g_iCompare_1 = g_iRandom_PMS;
			g_iAPLL = 450;				// VCO = 1800MHz
			break;			
		case 11:
			g_iPvalue = 3;
			g_iMvalue = 475;
			g_iSvalue = 2;
			g_iCompare_1 = g_iRandom_PMS;
			g_iAPLL = 475;				// VCO = 1900MHz
			break;			
		case 12:
			g_iPvalue = 3;
			g_iMvalue = 500;
			g_iSvalue = 2;
			g_iCompare_1 = g_iRandom_PMS;
			g_iAPLL = 500;			   // VCO = 2000MHz
			break;			
		case 13:
			g_iPvalue = 3;
			g_iMvalue = 532;
			g_iSvalue = 2;
			g_iCompare_1 = g_iRandom_PMS;
			g_iAPLL = 532;			// VCO = 2128MHz
			break;			
		case 14:
			g_iPvalue = 3;
			g_iMvalue = 550;
			g_iSvalue = 3;
			g_iCompare_1 = g_iRandom_PMS;
			g_iAPLL = 275;			// VCO = 2200MHz,  for 550MHz
			break;			
		case 15:
			g_iPvalue = 3;
			g_iMvalue = 480;
			g_iSvalue = 2;
			g_iCompare_1 = g_iRandom_PMS;
			g_iAPLL = 480;		    // VCO= 1920MHz   for 240MHz
			break;			
		case 16:
			g_iPvalue = 3;
			g_iMvalue = 520;
			g_iSvalue = 2;
			g_iCompare_1 = g_iRandom_PMS;
			g_iAPLL = 520;		// VCO = 2080MHz  for 520MHz
			break;			
		case 17:
			g_iPvalue = 3;
			g_iMvalue = 390;
			g_iSvalue = 2;
			g_iCompare_1 = g_iRandom_PMS;
			g_iAPLL = 390;		// VCO=1560MHz
			break;			
		default :

			break;
	}
}



u8 Apply_PMS_Sort(u32  uPLLStopEn)
{
	u32 uRegValue_DIV0, uRegValue_SRC, uTemp, uTemp1, uRegValue_APLL;
	u32 uRet=0;
	
	//EVT0, Using External Clock.
	

	uRegValue_APLL =  (u32)(((u32)(0x1<<31))|(g_iMvalue<<16)|(g_iPvalue<<8)|(g_iSvalue<<0));
	
	// Clock Divider Change 1:2:2
	uRegValue_DIV0 = Inp32SYSC(0x20);
	uTemp = (uRegValue_DIV0&~(0xFFFF))|(1<<12)|((0<<9)|(1<<8)|(1<<4)|(0<<0));		// ARM:HCLKx2:HCLK:PCLK = 1:1:2:2

	uRegValue_SRC = Inp32SYSC(0x1C);
	uTemp1 = (uRegValue_SRC&~(0x7))|0x0;

	if(uPLLStopEn == 1) 
	{

#if 1	//EVT0
	Outp32SYSC(0x1c, uTemp1);	// Clock Source Change
	Outp32SYSC(0x20, uTemp);	// Change Clock Divider
#endif

#if 1		// Check...
	GPIO_SetDataEach(eGPIO_N, eGPIO_9, 0);
	SYSC_StopPLL(eAPLL);

	Delay(10);					// Stop Time 34ms
	GPIO_SetDataEach(eGPIO_N, eGPIO_9, 1);
#endif

	//PLL Change
	Outp32SYSC(0xc, uRegValue_APLL);

	while(((Inp32SYSC(0xc)>>30)&0x1)==0);

#if 1	//EVT0
	// Change Divider Value
	Outp32SYSC(0x20, uRegValue_DIV0);
	// Change Clock SRC
	Outp32SYSC(0x1c, uRegValue_SRC);
#endif
	}
	else
	{
	//PLL Change
	Outp32SYSC(0xc, uRegValue_APLL);
	while(((Inp32SYSC(0xc)>>30)&0x1)==0);
	}
	
	SYSC_GetClkInform();
	UART_InitDebugCh(0, 115200);
	Delay(100);

	uRet = MemoryRWTest1(0x52000000, 0x52010000);

	if(uRet == false)
		{
		#if 1
		UART_Printf("Memory Read write Test is failed\n");
	 	UART_Printf("============================ \n");
		UART_Printf("Current Clock Information \n");
		UART_Printf("ARMCLK: %.2fMHz  HCLKx2: %.2fMHz  HCLK: %.2fMHz  PCLK: %.2fMHz\n",(float)g_ARMCLK/1.0e6, (float)g_HCLKx2/1.0e6, (float)g_HCLK/1.0e6, (float)g_PCLK/1.0e6);
		UART_Printf("============================ \n");
		#endif
	
		}
		 return uRet;
	

#if 0
	UART_Printf("============================ \n");
	UART_Printf("Current Clock Information \n");
	UART_Printf("ARMCLK: %.2fMHz  HCLKx2: %.2fMHz  HCLK: %.2fMHz  PCLK: %.2fMHz\n",(float)g_ARMCLK/1.0e6, (float)g_HCLKx2/1.0e6, (float)g_HCLK/1.0e6, (float)g_PCLK/1.0e6);
	UART_Printf("============================ \n");
#endif	
		
}


void Check_Result (u32 uRet)
{
	u32 uTestTime;

	g_RTC_ALM = 0;
	g_TestCount++;
	
	if(uRet == false)
	{
		DisplayLED(g_iRandom_PMS);
		UART_Printf(" Failed PMS Value = %d,  0x%x \n", g_iRandom_PMS, iCompare_PMS[g_iCompare_0]);
		UART_Getc();
		//break;
	}

#if 1
	INTC_SetVectAddr(NUM_RTC_TIC,Isr_RTC_Tick1);
	INTC_Enable(NUM_RTC_TIC);

	SYSC_GetClkInform();		
	UART_InitDebugCh(0, 115200);		

	//-daedoo
	/*
	RTC_SetCON(0,0,0,0,0,1);
	RTC_SetTime(InitYEAR,InitMONTH,InitDATE,InitDAY,InitHOUR,InitMIN,InitSEC);
	RTC_SetTickCNT(100);								// 3.05ms
	RTC_SetCON(0,0,0,0,0,0);							// 
	RTC_ClearIntP(0);	
	*/

	RTC_TimeInit(InitYEAR,InitMONTH,InitDATE,InitDAY,InitHOUR,InitMIN,InitSEC);
	RTC_TickCnt(100);	
	RTC_TickClkSelect(CLK_32768Hz);					// 3.05ms
	RTC_ClearPending(TimeTic);
		
	StartTimer(0);
	//RTC_SetCON(1,0,0,0,0,1);
	RTC_TickTimeEnable(true);
	RTC_Enable(true);
		
	while(g_RTC_ALM==0);
	uTestTime=StopTimer(0);
	UART_Printf("%d:  uTestTime = %d \n", g_iRandom_PMS,  uTestTime);
	//RTC_SetCON(0,0,0,0,0,0);
	RTC_Enable(false);


	INTC_Disable(NUM_RTC_TIC);
	
	#if  1
		if((uTestTime<3050)||(uTestTime>3150))
		//if((uTestTime<950)||(uTestTime>1100))			// (1000,10,2) => 1030~1050
			{
			   UART_Printf(" Failed PMS Value = %d,  0x%x \n", g_iRandom_PMS, iCompare_PMS[g_iCompare_0]);
			    g_TestErr++;
			    UART_Getc();
			}
	#endif

	if ((g_TestCount%100)==0)
		{
			UART_Printf("Test Count : %d,  Error: %d \n", g_TestCount, g_TestErr);

		}

	
#endif
	
}

void Test_PLL_EVT1_Sort(void)
{
	u32 uSelPLL, uResult,uPLLCtrl;
	
	Insert_Seed(); 

	UART_Printf("Choose  VCO_Range :  0 : 1.0G~2.2GHz,  1: 1.56~2.2GHz \n");

	uSelPLL = UART_GetIntNum();

	UART_Printf("Choose  VCO_Range :  0 : No PLL Stop & PMS Change,  1: PLL Stop & PMS Change \n");

	uPLLCtrl = UART_GetIntNum();

	UART_Printf("\n");
	UART_Printf("If You Start Test, Press Any Key!!!\n");
	UART_Printf("Press 'x' : Stop Test\n\n");
	UART_Getc();

	// CLK Output PAD Enable
	
	SYSC_SetLockTime(eAPLL, 100);
	SYSC_SetLockTime(eMPLL, 100);
	
	// Test Point....
	GPIO_SetFunctionEach(eGPIO_N, eGPIO_9, 1);	// EINT9 : Output pad
	g_TestCount = 0;
	g_TestEnd=0;

	switch(uSelPLL)
	{
		case 0:
				SYSC_CtrlCLKOUT( eCLKOUT_APLLOUT, 9); // for clock test, jangwj
				g_TestErr=0;
				while(!UART_GetKey())
			   	{
					Select_PMS_Random_EVT1_1();
					uResult = Apply_PMS_Sort(uPLLCtrl);
					Compare_PMS_Logic();
					Check_Result(uResult);
					if(g_TestEnd ==1)
					{
						UART_Printf("\nTest Count : %d,  Error: %d \n", g_TestCount, g_TestErr);
						break;
					}
					UART_TxEmpty();
			   	}
		 		break;
		case 1:
				
				SYSC_CtrlCLKOUT( eCLKOUT_APLLOUT, 9); // for clock test, jangwj
				g_TestErr=0;
				while(!UART_GetKey())
			   	{
					Select_PMS_Random_EVT1_2();
					uResult = Apply_PMS_Sort(uPLLCtrl);
					Compare_PMS_Logic();
					Check_Result(uResult);
					if(g_TestEnd ==1)
					{
						UART_Printf("\nTest Count : %d,  Error: %d \n", g_TestCount, g_TestErr);
						break;
					}
					UART_TxEmpty();
			   	}
				   		
				break;
	}

		Return_Condition();	
	
}



///////////////////////////////////////////////////////////////////////////////////
////////////////////                   Power Mode  Test                  /////////////////////////// 
///////////////////////////////////////////////////////////////////////////////////

const testFuncMenu clock_menu[] =
{
	Test_Locktime,						"PLL Lock Time Test",
	Test_ManualChangePLL,				"PLL Change-Manual",	
	Test_OnOffPll,						"PLL ON/OFF Test",
	Test_OnOffPll_EVT1,					"PLL ON/OFF Test - MDiv 390~410",
	Test_ManChangeCLKDivider,			"Clock Divider-Manual",
	Test_CLKOUT,						"Clock Out Test",
	Test_ChangeOPMode,					"Operating Mode Change Test",
	Test_PMS_Value_Change,				"PMS Value Random change",
	Test_PMS_Value_Change_MPLL,		"MPLL PMS Value Random change",
	Test_DIVN_Value_Change,			"Divider Value Random change",
	Test_PLL_EVT1_Sort,					"EVT1 PLL Sorting",
	Test_DIVISION_Value_Change_Sync,	"All Division in Sync Mode",
	Test_DIVISION_Value_Change_ASync, "All Division in Async Mode",
	Test_Divider_Manual,				"Division Manual",
	Test_DIVISION_withExternal,			"Division with External",
	Test_DIVISION_Value_Change_Sync_Sep,	"ARM Division in Sync Mode",
//	Test_WarmReset,						"Warm Reset Test",

	0, 0
};

void SYSCT_Clock(void)
{
	u32 i;
	s32 uSel;

	UART_Printf("[Clock Control Test]\n\n");
	
	while(1)
	{
		for (i=0; (u32)(clock_menu[i].desc)!=0; i++)
			UART_Printf("%2d: %s\n", i, clock_menu[i].desc);

		UART_Printf("\nSelect the function to test : ");
		uSel =UART_GetIntNum();
		UART_Printf("\n");
		if(uSel == -1) 
			break;

		if (uSel>=0 && uSel<(sizeof(clock_menu)/8-1))
			(clock_menu[uSel].func) ();
	}
}	

