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
*	File Name : intc_test.c
*  
*	File Description : This file implements the functons for interrupt controller test.
*
*	Author : Wonjoon Jang
*	Dept. : AP Development Team
*	Created Date : 2007/01/17
*	Version : 0.1 
* 
*	History
*	- Created(wonjoon.jang 2007/01/17)
*  
**************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#include "def.h"
#include "option.h"
#include "library.h"
#include "sfr6410.h"
#include "system.h"
#include "intc.h"
#include "gpio.h"
#include "sysc.h"

void INTC_Test(void);

volatile u32  g_SoftIntNo;
volatile u32  g_IntCnt12, g_IntCnt9;
volatile u32  g_INTOccur;
volatile u32  g_INTTestRst =0;

//////////
// Function Name : Isr_SoftInt
// Function Description : SoftInt ISR Routine
// Input : 	None
// Output :	None 
// Version : v0.1
void __irq Isr_SoftInt(void)
{
          
	u32 utemp, utemp1;

	
	if(g_SoftIntNo<32)
	{
		utemp= Inp32(0x71200000);
		utemp1 = (utemp>>g_SoftIntNo)&0x1;
	}
	else
	{
		utemp= Inp32(0x71300000);
		utemp1 = (utemp>>(g_SoftIntNo-32))&0x1;	
	}

	if(utemp1 == 1)
	{
		g_INTOccur = 1;
		//UART_Printf("Software int. No %d.\n", g_SoftIntNo);
		INTC_ClearSoftInt( g_SoftIntNo);
	}
	else
	{
		g_INTOccur = 0;
		UART_Printf("Software int. No %d. ----- Failed\n", g_SoftIntNo);
		UART_Printf("Int. Status Reg. 0x%x\n",  utemp);
	}

	
	INTC_ClearVectAddr();
    
}



//////////
// Function Name : INTCT_SoftInt
// Function Description : Software Int Test
// Input : 	None
// Output :	None 
// Version : v0.1

void	INTCT_SoftInt(eFunction_Test eTest, oFunctionT_AutoVar oAutoVar)
{
	
	if(eTest == eTest_Manual)
	{
	 	UART_Printf("[Software Int. Test]\n");
		UART_Printf("\nSelect Software Int No. : 	 ");
		 g_SoftIntNo=UART_GetIntNum();
	}
	else
	{
		g_SoftIntNo = oAutoVar.Test_Parameter[0];
	}

	g_INTOccur = 0;

	INTC_SetVectAddr(g_SoftIntNo,  Isr_SoftInt);		// NUM_EINT1 : Eint[11:4]
	INTC_Enable(g_SoftIntNo);
	INTC_SetSoftInt(g_SoftIntNo);

	//while(!UART_GetKey());
	//while(g_INTOccur == 0);
	Delay(100);			// Wait

	if(g_INTOccur ==0)
	{
		g_INTTestRst = 1;
	}


	if(eTest == eTest_Manual)
	{
		if(g_INTTestRst ==0)
	 	UART_Printf("\nSelect Software Int No. :  %d          OK\n ", g_SoftIntNo);
		else
		UART_Printf("\nSelect Software Int No :  %d          FAIL\n ", g_SoftIntNo);	

		g_INTTestRst=0;

	}
	else
	{
		if(g_INTTestRst ==1)
		UART_Printf("\nSelect Software Int No :  %d          FAIL\n ", g_SoftIntNo);
		
	}
	INTC_SetVectAddr(g_SoftIntNo,  0x0);		// NUM_EINT1 : Eint[11:4]
	INTC_Disable(g_SoftIntNo);

}



void __irq Isr_SoftInt0(void)
{
          
	
	UART_Printf("Software int. No %d.\n", 0);
	INTC_ClearSoftInt( 0);
	g_INTOccur =1;
	INTC_ClearVectAddr();
    
}

void __irq Isr_SoftInt1(void)
{
          
	
	UART_Printf("Software int. No %d.\n", 1);
	INTC_ClearSoftInt( 1);
	g_INTOccur=2;
	INTC_ClearVectAddr();
    
}

void __irq Isr_SoftInt2(void)
{
          
	
	UART_Printf("Software int. No %d.\n", 2);
	INTC_ClearSoftInt(2);
	g_INTOccur=3;
	INTC_ClearVectAddr();
    
}

void __irq Isr_SoftInt3(void)
{
          
	
	UART_Printf("Software int. No %d.\n", 3);
	INTC_ClearSoftInt(3);
	g_INTOccur=4;
	INTC_ClearVectAddr();
    
}

//////////
// Function Name : INTCT_SoftInt
// Function Description : Software Int Test
// Input : 	None
// Output :	None 
// Version : v0.1
void	INTCT_SWPriority(eFunction_Test eTest, oFunctionT_AutoVar oAutoVar)
//void	INTCT_SWPriority(void)
{
	u32 uTemp;

	if(eTest == eTest_Manual)
	{
	 	UART_Printf("[Software Int. Test]\n");
	 	UART_Printf("\nSelect Priority Case : 0 : Default,   1:INT1 Priority Level (INT1-2-3-0)	 ");
		uTemp=UART_GetIntNum();
	}
	else
	{
		uTemp = oAutoVar.Test_Parameter[0];
	}
	switch(uTemp)
		{
		case 0:
		break;

		case 1:
			Outp32(0x71200200, 0xf);	// INT0
			Outp32(0x71200204, 0x1);	// INT1
			Outp32(0x71200208, 0x8);	// INT2
			Outp32(0x7120020C, 0x8);	// INT3
			break;
		}

	INTC_SetVectAddr(0,  Isr_SoftInt0);		// NUM_EINT1 : Eint[11:4]
	INTC_SetVectAddr(1,  Isr_SoftInt1);	
	INTC_SetVectAddr(2,  Isr_SoftInt2);	
	INTC_SetVectAddr(3,  Isr_SoftInt3);	
	INTC_Enable(0);
	INTC_Enable(1);
	INTC_Enable(2);
	INTC_Enable(3);
	Outp32(0x71200018, 0xF); 		// Soft INT. 0,1,2,3

	//while(!UART_GetKey());
	Delay(1000);			// Wait 1ms

	if((uTemp == 0 && g_INTOccur  ==4)||(uTemp == 1 && g_INTOccur == 1))
	{	
		g_INTTestRst = 0; 		// Test OK
	}
	else
	{	
		g_INTTestRst = 1;
	}

	if(eTest == eTest_Manual)
	{
		if(g_INTTestRst ==0)
		UART_Printf("\nSoftware Priority Test: %d         OK\n ",g_INTOccur);
		else
		UART_Printf("\nSoftware Priority Test: %d       FAIL\n ",g_INTOccur);
		
	 	g_INTTestRst = 0;
	}
	
	INTC_Disable(0);
	INTC_Disable(1);
	INTC_Disable(2);
	INTC_Disable(3);

	Outp32(0x71200200, 0xf);
	Outp32(0x71200204, 0xf);
	Outp32(0x71200208, 0xf);
	Outp32(0x7120020C, 0xf);

	Outp32(0x7120001c, 0xf);
}



void	INTCT_SWPriorityMask(eFunction_Test eTest, oFunctionT_AutoVar oAutoVar)
//void	INTCT_SWPriority(void)
{
	u32 uTemp;

	if(eTest == eTest_Manual)
	{
	 	UART_Printf("[Software Int. Test]\n");
	 	UART_Printf("\nSelect Priority Case : 0 : 0xF Priority Level Mask,   1:0x2,0x3 Priority Level Mask	 ");
		uTemp=UART_GetIntNum();
	}
	else
	{
		uTemp = oAutoVar.Test_Parameter[0];
	}
	switch(uTemp)
		{
		case 0:
			Outp32(0x71200200, 0xf);
			Outp32(0x71200204, 0x1);
			Outp32(0x71200208, 0x8);
			Outp32(0x7120020C, 0x8);
			Outp32(0x71200024, 0x7FFF);
			
		break;

		case 1:
			Outp32(0x71200200, 0x4);
			Outp32(0x71200204, 0x2);
			Outp32(0x71200208, 0x3);
			Outp32(0x7120020C, 0x1);
			Outp32(0x71200024, 0xFFF3);
			break;
		}

	INTC_SetVectAddr(0,  Isr_SoftInt0);		// NUM_EINT1 : Eint[11:4]
	INTC_SetVectAddr(1,  Isr_SoftInt1);	
	INTC_SetVectAddr(2,  Isr_SoftInt2);	
	INTC_SetVectAddr(3,  Isr_SoftInt3);	
	INTC_Enable(0);
	INTC_Enable(1);
	INTC_Enable(2);
	INTC_Enable(3);
	Outp32(0x71200018, 0xF); 		// Soft INT. 0,1,2,3

	Delay(1000);			// Wait 1ms

	if((uTemp == 0 && g_INTOccur  ==4)||(uTemp == 1 && g_INTOccur == 1))
	{	
		g_INTTestRst = 0; 		// Test OK
	}
	else
	{	
		g_INTTestRst = 1;
		
	}

	if(eTest == eTest_Manual)
	{
		if(g_INTTestRst ==0)
		UART_Printf("\nSoftware Priority Test: %d         OK\n ",g_INTOccur);
		else
		UART_Printf("\nSoftware Priority Test: %d       FAIL\n ",g_INTOccur);
		
	 	g_INTTestRst = 0;
	}
	
	INTC_Disable(0);
	INTC_Disable(1);
	INTC_Disable(2);
	INTC_Disable(3);
	
	Outp32(0x71200200, 0xf);
	Outp32(0x71200204, 0xf);
	Outp32(0x71200208, 0xf);
	Outp32(0x7120020C, 0xf);
	Outp32(0x71200024, 0xFFFF);

	Outp32(0x7120001c, 0xf);


}



void __irq Isr_Eint9_(void)
{
          
	GPIO_EINT0ClrPend(9);
	g_IntCnt9++;
	UART_Printf("EINT9 interrupt is occurred %d.\n", g_IntCnt9);

	INTC_ClearVectAddr();
    
}

void __irq Isr_Eint12(void)
{
          
	GPIO_EINT0ClrPend(12);
	g_IntCnt12++;
	UART_Printf("EINT12 interrupt is occurred %d.\n", g_IntCnt12);

	INTC_ClearVectAddr();
    
}

void	INTCT_VIC0VIC1(eFunction_Test eTest, oFunctionT_AutoVar oAutoVar)
//void	INTCT_VIC0VIC1(void)
{

	if(eTest == eTest_Manual)
	{
	 	UART_Printf("[VIC0&VIC1 Priority Test]\n");
	 	UART_Printf("This Test need SMDK Board revision - EINT12\n");
	 	
	}

	GPIO_Init();

	g_IntCnt9=0;
	g_IntCnt12=0;
	
	INTC_SetVectAddr(NUM_EINT1, Isr_Eint9_);
	INTC_SetVectAddr(NUM_EINT2, Isr_Eint12);
	
	GPIO_SetPullUpDownEach(eGPIO_N, eGPIO_9, 0x0);  // Pull Down Disable
	GPIO_SetPullUpDownEach(eGPIO_N, eGPIO_11, 0x0);  // Pull Down Disable
	GPIO_SetPullUpDownEach(eGPIO_N, eGPIO_10, 0x0); // Pull Down Disable
	GPIO_SetPullUpDownEach(eGPIO_N, eGPIO_12, 0x0);  // Pull Down Disable

	Delay(100);
	GPIO_SetEint0(9, Falling_Edge, eDLYFLT, 0);
	GPIO_SetEint0(12, Falling_Edge, eDLYFLT, 0);
	
	GPIO_EINT0ClrPend(9);
	GPIO_EINT0ClrPend(12);

	GPIO_EINT0DisMask(9);
	GPIO_EINT0DisMask(12);

	INTC_Enable(NUM_EINT1);
	INTC_Enable(NUM_EINT2);

	Outp32(0x71200200, 0xf);  // Test


	while(!UART_GetKey());
	
	INTC_Disable(NUM_EINT1);
	INTC_Disable(NUM_EINT2);


}


const AutotestFuncMenu Intc_man[] =
{
	INTCT_SoftInt,					"Software Interrupt Test",
	INTCT_SWPriority,                          "INT Priority",
	INTCT_SWPriorityMask,			"INT Priority Mask",
	INTCT_VIC0VIC1,					"VIC0&VIC1",
	0, 0
};


void INTCT_Test(void)
{
	u32 i;
	s32 uSel;
	oFunctionT_AutoVar oParameter;
	
	while(1)
	{
		for (i=0; (u32)(Intc_man[i].desc)!=0; i++)
			UART_Printf("%2d: %s\n", i, Intc_man[i].desc);

		UART_Printf("\nSelect the function to test : \n");
		uSel =UART_GetIntNum();
		UART_Printf("\n");
		if(uSel == -1) 
			break;

		if (uSel>=0 && uSel<(sizeof(Intc_man)/8-1))
			(Intc_man[uSel].func) (eTest_Manual, oParameter);
	}

}




///////  Error detecton must be changed.......
void INTCT_FullFunction(void)
{
	u32 i, uNumMode;
	oFunctionT_AutoVar oAutoTest;	
	
	UART_Printf("\n [ INTC Full Function Test ]\n");
	UART_Printf("============================================= \n");
	
	g_INTTestRst =0;

	uNumMode = 64;
	for(i=0; i<uNumMode;i++)
	{
		oAutoTest.Test_Parameter[0] = i;
	 	INTCT_SoftInt(eTest_Auto, oAutoTest );
	}
      
	if(g_INTTestRst ==0)
	 	UART_Printf("		1. Software INT Test. :           PASS\n ");
	else
		UART_Printf("		1. Software INT Test. :          FAIL\n ");

	///////////////  SoftPriority Test  ///////////////////////

	
	g_INTTestRst =0;
	for(i=0; i<2;i++)
	{
		oAutoTest.Test_Parameter[0] = i;
	 	INTCT_SWPriority(eTest_Auto, oAutoTest );
	}

	if(g_INTTestRst ==0)
	 	UART_Printf("		2. Priority  Test. :           PASS\n ");
	else
		UART_Printf("		2. Priority  Test. :          FAIL\n ");
	

}


u8 INTCT_autotest(void)
{
	u32 i, uNumMode;
	oFunctionT_AutoVar oAutoTest;	
	u32 uFault;
	
	
	g_INTTestRst =0;
       uFault = 0;

       ///////////////  SoftINT Test  ///////////////////////
	uNumMode = 64;
	for(i=0; i<uNumMode;i++)
	{
		oAutoTest.Test_Parameter[0] = i;
	 	INTCT_SoftInt(eTest_Auto, oAutoTest );
	}
      
	if(g_INTTestRst ==1) 	uFault++ ;

	///////////////  SoftPriority Test  ///////////////////////

	
	g_INTTestRst =0;
	for(i=0; i<2;i++)
	{
		oAutoTest.Test_Parameter[0] = i;
	 	INTCT_SWPriority(eTest_Auto, oAutoTest );
	}

	if(g_INTTestRst ==1) 	uFault++ ;

	if(uFault == 0)
	{
		UART_Printf("	INTC SoftwareINT&Priority :			PASS \n");
		return true;
	}
	else
	{
		UART_Printf("	INTC SoftwareINT&Priority :			FAIL\n");
		return false;
	}

	
	

}




///////////////////////////////////////////////////////////////////////////////////
////////////////////                   Syscon Main Test                  /////////////////////////// 
///////////////////////////////////////////////////////////////////////////////////

const testFuncMenu intc_menu[] =
{
	INTCT_Test,						"Interrupt Controller Man. Test",

	INTCT_FullFunction,				"Interrupt Controller Full Test",
	0, 0
};

void INTC_Test(void)
{
	u32 i;
	s32 uSel;

	UART_Printf("[SYSC_Test]\n\n");
	
	while(1)
	{
		for (i=0; (u32)(intc_menu[i].desc)!=0; i++)
			UART_Printf("%2d: %s\n", i, intc_menu[i].desc);

		UART_Printf("\nSelect the function to test : ");
		uSel =UART_GetIntNum();
		UART_Printf("\n");
		if(uSel == -1) 
			break;

		if (uSel>=0 && uSel<(sizeof(intc_menu)/8-1))
			(intc_menu[uSel].func) ();
	}
}	


