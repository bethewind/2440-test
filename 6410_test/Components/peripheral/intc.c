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
*	File Name : intc.c
*  
*	File Description : This file implements the API functons for interrupt controller.
*
*	Author : Haksoo,Kim
*	Dept. : AP Development Team
*	Created Date : 2006/11/08
*	Version : 0.1 
* 
*	History
*	- Created(Haksoo,Kim 2006/11/08)
*     - Added Software Interrupt API (wonjoon.jang 2007/01/17)
*  
**************************************************************************************/

#include <stdio.h>

#include "option.h"
#include "library.h"
#include "sfr6410.h"
#include "system.h"
#include "intc.h"


// VIC0
#define	rVIC0IRQSTATUS			(VIC0_BASE + 0x00)
#define	rVIC0FIQSTATUS			(VIC0_BASE + 0x04)
#define	rVIC0RAWINTR			(VIC0_BASE + 0x08)
#define	rVIC0INTSELECT			(VIC0_BASE + 0x0c)
#define	rVIC0INTENABLE			(VIC0_BASE + 0x10)
#define	rVIC0INTENCLEAR			(VIC0_BASE + 0x14)
#define	rVIC0SOFTINT			(VIC0_BASE + 0x18)
#define	rVIC0SOFTINTCLEAR		(VIC0_BASE + 0x1c)
#define	rVIC0PROTECTION			(VIC0_BASE + 0x20)
#define	rVIC0SWPRIORITYMASK		(VIC0_BASE + 0x24)
#define	rVIC0PRIORITYDAISY		(VIC0_BASE + 0x28)

#define rVIC0VECTADDR			(VIC0_BASE + 0x100)

#define rVIC0VECPRIORITY		(VIC0_BASE + 0x200)

#define rVIC0ADDR				(VIC0_BASE + 0xf00)
#define rVIC0PERID0				(VIC0_BASE + 0xfe0)
#define rVIC0PERID1				(VIC0_BASE + 0xfe4)
#define rVIC0PERID2				(VIC0_BASE + 0xfe8)
#define rVIC0PERID3				(VIC0_BASE + 0xfec)
#define rVIC0PCELLID0			(VIC0_BASE + 0xff0)
#define rVIC0PCELLID1			(VIC0_BASE + 0xff4)
#define rVIC0PCELLID2			(VIC0_BASE + 0xff8)
#define rVIC0PCELLID3			(VIC0_BASE + 0xffc)

// VIC1
#define	rVIC1IRQSTATUS			(VIC1_BASE + 0x00)
#define	rVIC1FIQSTATUS			(VIC1_BASE + 0x04)
#define	rVIC1RAWINTR			(VIC1_BASE + 0x08)
#define	rVIC1INTSELECT			(VIC1_BASE + 0x0c)
#define	rVIC1INTENABLE			(VIC1_BASE + 0x10)
#define	rVIC1INTENCLEAR			(VIC1_BASE + 0x14)
#define	rVIC1SOFTINT			(VIC1_BASE + 0x18)
#define	rVIC1SOFTINTCLEAR		(VIC1_BASE + 0x1c)
#define	rVIC1PROTECTION			(VIC1_BASE + 0x20)
#define	rVIC1SWPRIORITYMASK		(VIC1_BASE + 0x24)
#define	rVIC1PRIORITYDAISY		(VIC1_BASE + 0x28)

#define rVIC1VECTADDR			(VIC1_BASE + 0x100)

#define rVIC1VECPRIORITY		(VIC1_BASE + 0x200)

#define rVIC1ADDR				(VIC1_BASE + 0xf00)
#define rVIC1PERID0				(VIC1_BASE + 0xfe0)
#define rVIC1PERID1				(VIC1_BASE + 0xfe4)
#define rVIC1PERID2				(VIC1_BASE + 0xfe8)
#define rVIC1PERID3				(VIC1_BASE + 0xfec)
#define rVIC1PCELLID0			(VIC1_BASE + 0xff0)
#define rVIC1PCELLID1			(VIC1_BASE + 0xff4)
#define rVIC1PCELLID2			(VIC1_BASE + 0xff8)
#define rVIC1PCELLID3			(VIC1_BASE + 0xffc)


//////////
// Function Name : INTC_Init
// Function Description : This function initializes interrupt controller
// Input : NONE 
// Output : NONE
// Version : 
void INTC_Init(void)
{
#if (VIC_MODE==0)	
	u32 i;
	
	for(i=0;i<32;i++)
		Outp32(rVIC0VECTADDR+4*i, i);
	
	for(i=0;i<32;i++)
		Outp32(rVIC1VECTADDR+4*i, i+32);
#endif
	Outp32(rVIC0INTENCLEAR, 0xffffffff);
	Outp32(rVIC1INTENCLEAR, 0xffffffff);

	Outp32(rVIC0INTSELECT, 0x0);
	Outp32(rVIC1INTSELECT, 0x0);

	INTC_ClearVectAddr();

	return;
}

//////////
// Function Name : INTC_Enable
// Function Description : This function sets the corresponding bit of interrupt enable register
// Input : intNum, interrupt number to be enabled
// Output : NONE
// Version : 
void INTC_Enable(u32 intNum)
{
	u32 temp;

	if(intNum<32)
	{
		temp = Inp32(rVIC0INTENABLE);
		temp |= (1<<intNum);
		Outp32(rVIC0INTENABLE, temp);
	}
	else
	{
		temp = Inp32(rVIC1INTENABLE);
		temp |= (1<<(intNum-32));
		Outp32(rVIC1INTENABLE, temp);	
	}

	return;
}

//////////
// Function Name : INTC_Disable
// Function Description : This function sets the corresponding bit of interrupt enable clear register
// Input : intNum, interrupt number to be disabled
// Output : NONE
// Version : 
void INTC_Disable(u32 intNum)
{
	u32 temp;

	if(intNum<32)
	{
		temp = Inp32(rVIC0INTENCLEAR);
		temp |= (1<<intNum);
		Outp32(rVIC0INTENCLEAR, temp);
	}
	else
	{
		temp = Inp32(rVIC1INTENCLEAR);
		temp |= (1<<(intNum-32));
		Outp32(rVIC1INTENCLEAR, temp);	
	}

	return;
}

//////////
// Function Name : INTC_ReadIntSrc
// Function Description : This function reads the interrupt source number
// Input : NONE
// Output : interrupt source number
// Version : 
u32 INTC_ReadIntSrc(void)
{
	u32 temp;
	
	temp = Inp32(rVIC0ADDR);
	
	return temp;
}

//////////
// Function Name : INTC_SetHandler
// Function Description : This function registers ISR to the corresponding number
// Input :	intNum, interrupt number to register
//			handler, interrupt handler
// Output : NONE
// Version : 
extern void (*IntHandlerTable[INT_LIMIT])(void) __irq;
void INTC_SetHandler(u32 intNum, void (*handler)() __irq)
{
	IntHandlerTable[intNum] = handler;
	
	return;
}

//////////
// Function Name : INTC_SetVectAddr
// Function Description : This function writes ISR to the corresponding vector address register
// Input :	intNum, interrupt number to register
//			handler, interrupt handler
// Output : NONE
// Version : 
void INTC_SetVectAddr(u32 intNum, void (*handler)(void) __irq)
{	
#if (VIC_MODE == 1)
	if(intNum<32)	//VIC0
	{
		Outp32(rVIC0VECTADDR+4*intNum, (unsigned)handler);
	}
	else	//VIC1
	{
		Outp32(rVIC1VECTADDR+4*(intNum-32), (unsigned)handler);
	}
#else
	INTC_SetHandler(intNum, handler);
#endif	
	return;
}


//////////
// Function Name : INTC_ClearVectAddr
// Function Description : This function clears the vector address register
// Input : NONE
// Output : NONE
// Version : 
void INTC_ClearVectAddr(void)
{
	Outp32(rVIC0ADDR, 0);
	Outp32(rVIC1ADDR, 0);
	
	return;
}


//////////
// Function Name : INTC_SetSoftInt
// Function Description : This function sets the Software interrupt register
// Input : intNum, interrupt number to be enabled
// Output : NONE
// Version : 
void INTC_SetSoftInt(u32 intNum)
{
	u32 temp;

	if(intNum<32)
	{
		temp = Inp32(rVIC0SOFTINT);
		temp |= (1<<intNum);
		Outp32(rVIC0SOFTINT, temp);
	}
	else
	{
		temp = Inp32(rVIC1SOFTINT);
		temp |= (1<<(intNum-32));
		Outp32(rVIC1SOFTINT, temp);	
	}

	return;
}

//////////
// Function Name : INTC_ClearSoftInt
// Function Description : This function sets the Software interrupt register
// Input : intNum, interrupt number to be enabled
// Output : NONE
// Version : 
void INTC_ClearSoftInt(u32 intNum)
{
	u32 temp;

	if(intNum<32)
	{
		
		temp = (1<<intNum);
		Outp32(rVIC0SOFTINTCLEAR, temp);
	}
	else
	{
		
		temp = (1<<(intNum-32));
		Outp32(rVIC1SOFTINTCLEAR, temp);	
	}

	return;
}


//////////
// File Name : INTC_GetVicIrqStatus
// File Description : This function gets the IRQ Status register value
// Input : uController - VIC controller number
// Output : VICIRQSTATUS register value
// Version : 
u32 INTC_GetVicIrqStatus(u32 uController)
{
	if(uController == 0)
		return	Inp32(rVIC0IRQSTATUS);
	else if(uController == 1)
		return 	Inp32(rVIC1IRQSTATUS);

	return 0;
}



