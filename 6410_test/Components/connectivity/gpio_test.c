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
*	File Name : Gpio_test.c
*  
*	File Description : This file implements the functons for Gpio & Eint test.
*
*	Author : Wonjoon Jang
*	Dept. : AP Development Team
*	Created Date : 2007/01/02
*	Version : 0.1 
* 
*	History
*	- Created(wonjoon.jang 2007/01/02)
*  
**************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "def.h"
#include "option.h"
#include "library.h"
#include "sfr6410.h"
#include "system.h"
#include "sysc.h"
#include "intc.h"

#include "gpio.h"

void Test_GPIO(void);

extern u32  g_IntCnt;


//////////
// Function Name : EintSW_Test
// Function Description : SMDK6410 EINT SW Test
// Input : 	None
// Output :	None 
// Version : v0.1
void EintSW_Test(void)
{

	u32  usel;
	u32 uEintNo, uFltWidth ;
	u32  uIntType, uFltType;

	UART_Printf("[External Interrupt Test ]\n");
	UART_Printf("\nSelect EINT SW : 0:EINT9, 1:EINT10, 2:EINT11	: ");
	usel=UART_GetIntNum();
	UART_Printf("\n");

	GPIO_Init();
	
	switch(usel)
	{
		case 0: 
				UART_Printf("Selected EINT9 SW..... \n");
				uEintNo = 9;
				GPIO_EINT0ClrPend(uEintNo);
				INTC_SetVectAddr(NUM_EINT1,  Isr_Eint9);		// NUM_EINT1 : Eint[11:4]
				INTC_Enable(NUM_EINT1);
			     	break;
               
		case 1: 
				UART_Printf("Selected EINT10 SW ..... \n");
			     	uEintNo = 10;
			     	GPIO_EINT0ClrPend(uEintNo);
				INTC_SetVectAddr(NUM_EINT1, Isr_Eint10);		// NUM_EINT1 : Eint[11:4]
				INTC_Enable(NUM_EINT1);
			     	break;
		case 2: 
				UART_Printf("Selected EINT11 SW ..... \n");
				uEintNo = 11;
				GPIO_EINT0ClrPend(uEintNo);
				INTC_SetVectAddr(NUM_EINT1,  Isr_Eint11);		// NUM_EINT1 : Eint[11:4]
				INTC_Enable(NUM_EINT1);
			     	break;
			     	
		default : Assert(0);
	}
   
	
	UART_Printf("\nSelect Int. Type : 0:Low Level,  1:High Level,  2:Falling Edge,  3:Rising Edge, 4:Both Edge	: ");
	usel=UART_GetIntNum();
	UART_Printf("\n");

	switch(usel)
	{
		case 0: 
				uIntType= Low_Level;
			     	break;       
		case 1: 
				uIntType= High_Level;
			     	break;
		case 2: 
				uIntType= Falling_Edge;
			     	break;
		case 3: 
				uIntType=Rising_Edge;
			     	break;
	     	case 4: 
				uIntType=Both_Edge;
			     	break;
		default : Assert(0);
	}

	UART_Printf("\nSelect Filter Type : 0:Filter Disable,  1: Delay Filter,  2:Digital Filter	: ");
	usel=UART_GetIntNum();
	UART_Printf("\n");

	switch(usel)
	{
		case 0: 
				uFltType= eDisFLT;
			     	break;       
		case 1: 
				uFltType= eDLYFLT;
			     	break;
		case 2: 
				uFltType= eDIGFLT;
				UART_Printf("\nEnter Filter Width [1~0x3F] :    ");
				uFltWidth = UART_GetIntNum();
				UART_Printf("\n");			
			     	break;
		default : Assert(0);
	}


	GPIO_SetEint0(uEintNo, uIntType, (FLT_eTYPE)uFltType, uFltWidth);
	GPIO_EINT0DisMask(uEintNo);

	g_IntCnt = 0;
	UART_Printf("Press the EINT buttons or Press any key to exit.\n");


	UART_Getc();
	UART_Printf("EINT interrupt is occurred %d.\n", g_IntCnt);


	GPIO_EINT0EnMask(uEintNo);
	INTC_Disable(NUM_EINT1);
	
}


///////////////////////////////////////////////////////////////////////////////////
////////////////////                    GPIO Main Test                  /////////////////////////// 
///////////////////////////////////////////////////////////////////////////////////

const testFuncMenu gpio_menu[] =
{
		0,                      				"Exit",
		EintSW_Test,				"Eint9,10,11 Test",
		0,0
};

void Test_GPIO(void)
{
	u32 i;
	s32 uSel;

	UART_Printf("[GPIO_Test]\n\n");
	
	while(1)
	{
		for (i=0; (u32)(gpio_menu[i].desc)!=0; i++)
			UART_Printf("%2d: %s\n", i, gpio_menu[i].desc);

		UART_Printf("\nSelect the function to test : ");
		uSel =UART_GetIntNum();
		UART_Printf("\n");
		if(uSel == -1) 
			break;
		if (uSel==0)
			break;
		if (uSel>=0 && uSel<(sizeof(gpio_menu)/8-1))
			(gpio_menu[uSel].func) ();
	}
}	



