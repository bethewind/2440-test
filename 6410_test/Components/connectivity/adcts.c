/**************************************************************************************
* 
*	Project Name : S3C6410 Validation
*
*	Copyright 2006~2008 by Samsung Electronics, Inc.
*	All rights reserved.
*
*	Project Description :
*		This software is only for validating functions of the S3C6410.
*		Anybody can use this software without our permission.
*  
*--------------------------------------------------------------------------------------
* 
*	File Name : adcts.c
*  
*	File Description : This file implements the driver functions for ADC/TS.
*
*	Author : Sunil,Roe
*	Dept. : AP Development Team
*	Created Date : 2007/1/2
*	Version : 0.1 
* 
*	History
*	- Created(Sunil,Roe 2007/1/2)
*	- Edited by OnPil,Shin (SOP) on 2008/03/05 ~ 
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
#include "intc.h"
#include "gpio.h"

#include "adcts.h"

#define 	ADCTS			( (volatile oADCTS_REGS *) (g_pADCTSBase))

typedef struct tag_ADCTS_REGS
{
	u32 rADCCON;
	u32 rADCTSC;
	u32 rADCDLY;
	u32 rADCDAT0;
	u32 rADCDAT1;
	u32 rADCUPDN;
	u32 rADCCLRINT;
} 
oADCTS_REGS;

// define global variables
static void *				g_pADCTSBase;
volatile	ADCTS_oInform	g_oADCTSInform;

/*---------------------------------- ISR Routines Definition ---------------------------------*/
//void __irq	ADC_NormalISR(void);
//void __irq	ADCTS_StylusCheckISR(void);
//void __irq	ADCTS_StylusTrackingISR(void);
//void __irq	ADCTS_StylusTrackingISR_Auto(void);

//////////
// Function Name : ADCTS_Init
// Function Description : This function initializes an ADCTS.
// Input : 	NONE 
// Output : 	TRUE - Memory Device is reset
//			FALSE - Memory Device is not reset because of ERROR
// Version : v0.1
u8	ADCTS_Init(void)
{
	u32 uBaseAddress=0;

	uBaseAddress = ADCTS_BASE;
	g_pADCTSBase = (void *)uBaseAddress;
	ADCTS_SetADCClearInt();
	ADCTS_SetADCClearWKInt();
	return TRUE;
}


/*---------------------------------- ISR Routines ---------------------------------*/
//////////
// Function Name : ADC_NormalISR
// Function Description : This function is a ADC interrupt handler of ADC_ConvertADCNormallyInADCInterrupt().
// Input : 	NONE 
// Output : 	NONE
// Version : v0.1
void __irq	ADC_NormalISR(void)
{
	g_oADCTSInform.uADCSampleData 	= ADCTS_GetXPData();
	g_oADCTSInform.ucADCflag		= 1;

	ADCTS_SetADCClearInt();
	INTC_ClearVectAddr();
}

/*
//////////
// Function Name : ADCTS_StylusCheckISR
// Function Description : This function is a pen-down interrupt handler of ADCTS_StylusCheckInPENDNInterrupt().
// Input : 	NONE 
// Output : 	NONE
// Version : v0.1
void __irq ADCTS_StylusCheckISR(void)
{
	if ( ADCTS_GetXPStylusIntState() == STYLUS_DOWN )
	{
		g_oADCTSInform.ucTouchStatus = ADCTS_TOUCH_DOWN;
		UART_Printf("Pen Down Interrupt Occur !!!\n");		
	}		
	else if ( ADCTS_GetXPStylusIntState() == STYLUS_UP )
	{
		g_oADCTSInform.ucTouchStatus = ADCTS_TOUCH_UP;
		UART_Printf("Pen Up Interrupt Occur !!!\n");		
	}

	ADCTS_SetADCClearInt();
	//UART_Printf("Pen Down Interrupt Occur !!!\n");

	ADCTS_SetADCClearWKInt();
	INTC_ClearVectAddr();
}
*/

//////////
// Function Name : ADCTS_StylusTrackingISR
// Function Description : This function is a pen-down interrupt handler of ADCTS_StylusTrackingUsingAutoConversionInPENDNInterruptADCPolling().
// Input : 	NONE 
// Output : 	NONE
// Version : v0.1
void __irq ADCTS_StylusTrackingISR(void)
{
	if ( ADCTS_GetXPStylusIntState() == STYLUS_DOWN )
	{
		g_oADCTSInform.ucTouchStatus = ADCTS_TOUCH_DOWN;
		//UART_Printf("STYLUS_DOWN ADCUPDN (Down=0x1): %x\n",ADCTS_GetRegValue(eADCTS_ADCUPDN));	// Only Test by SOP on 2008/05/19	
	}
	else 
	{
		g_oADCTSInform.ucTouchStatus = ADCTS_TOUCH_UP;
		//UART_Printf("STYLUS_UP ADCUPDN (Up=0x2): %x\n",ADCTS_GetRegValue(eADCTS_ADCUPDN));	// Only Test by SOP on 2008/05/19	
	}

	ADCTS_SetRegValue(eADCTS_ADCUPDN, 0x0);	// Added by SOP on 2008/05/19
	ADCTS_SetADCClearInt();
	ADCTS_SetADCClearWKInt();
	INTC_ClearVectAddr();
}


/*---------------------------------- APIs of rADCCON ---------------------------------*/
//////////
// Function Name : ADC_InitADCCON
// Function Description : This function initializes an ADCCON register.
// Input : 	NONE 
// Output : 	NONE
// Version : v0.1
void ADC_InitADCCON(void)
{
	u32 uADCCONValue=0;

	uADCCONValue = ADCTS_GetRegValue(eADCTS_ADCCON);

#if RESSEL12BIT
	// 12-bit, Enable Prescaler, presclar value, Normal operation, Disabled start by read operation
	uADCCONValue = (1<<16)|(1<<14)|(ADCTS_PRSCVL<<6)|(0<<2)|(0<<1);;
#else
	// 10-bit, Enable Prescaler, presclar value, Normal operation, Disabled start by read operation
	uADCCONValue = (0<<16)|(1<<14)|(ADCTS_PRSCVL<<6)|(0<<2)|(0<<1);;
#endif

	ADCTS_SetRegValue(eADCTS_ADCCON, uADCCONValue);	
}

//////////
// Function Name : ADC_IsEOCFlag
// Function Description : This function requests the result of ADC Conversion.
// Input : 	NONE 
// Output : 	TRUE	- End of A/D conversion
//			FALSE 	- A/D conversion in process
// Version : v0.1
u8 ADC_IsEOCFlag(void)
{
	u32 uADCCONValue=0;
	u8 ucEOCFlag=0;

	uADCCONValue 	= ADCTS_GetRegValue(eADCTS_ADCCON);
	ucEOCFlag 		= (uADCCONValue & 0x8000) >> 15;

	if ( ucEOCFlag == 1 )
		return true;
	else 
		return false;
}

//////////
// Function Name : ADC_EnablePrescaler
// Function Description : This function enables ADC prescaler.
// Input : 	ucSel	- Enable/Disable Selector
//						0 : Disable
//						1 : Enable
// Output : 	NONE
// Version : v0.1
void ADC_EnablePrescaler(u8 ucSel)
{
	u32 uADCCONValue=0;
	u32 uADCCONValue1=0;		// Added by SOP on 2008/05/28	

	uADCCONValue = ADCTS_GetRegValue(eADCTS_ADCCON);
	uADCCONValue1 |= (uADCCONValue & ~(1<<14))|(ucSel<<12);
	ADCTS_SetRegValue(eADCTS_ADCCON, uADCCONValue1);	
}

//////////
// Function Name : ADC_SetPrescaler
// Function Description : This function configures the value of ADC prescaler.
// Input : 	ucPrscvl	- ADC prescaler value (5~255)
// Output : 	NONE
// Version : v0.1
void ADC_SetPrescaler(u8 ucPrscvl)
{
	u32 uADCCONValue=0;
	u32 uADCCONValue1=0;		// Added by SOP on 2008/05/28	

	uADCCONValue 	= ADCTS_GetRegValue(eADCTS_ADCCON);
	uADCCONValue1	|= (uADCCONValue & ~(0xff<<6))|(ucPrscvl<<6);
	ADCTS_SetRegValue(eADCTS_ADCCON, uADCCONValue1);
}

//////////
// Function Name : ADC_SelectADCCh
// Function Description : This function chooses one channel of 8 ADC channels.
// Input : 	ucSel	- number of ADC channel
// Output : 	NONE
// Version : v0.1
void ADC_SelectADCCh(u8 ucSel)
{
	u32 uADCCONValue=0;
	u32 uADCCONValue1=0;		// Added by SOP on 2008/05/28	

	uADCCONValue	= ADCTS_GetRegValue(eADCTS_ADCCON);
	uADCCONValue1	|= ((uADCCONValue & ~(0x7<<3))|(ucSel<<3));	
	ADCTS_SetRegValue(eADCTS_ADCCON, uADCCONValue1);
}

//////////
// Function Name : ADC_SelectStandbyMode
// Function Description : This function chooses 'Standby mode'.
// Input : 	ucSel	- Standby mode select value
//						0 : Normal operation mode
//						1 : Standby mode
// Output : 	NONE
// Version : v0.1
void ADC_SelectStandbyMode(u8 ucSel)
{
	u32 uADCCONValue=0;
	u32 uADCCONValue1=0;		// Added by SOP on 2008/05/28	

	uADCCONValue	= ADCTS_GetRegValue(eADCTS_ADCCON);
	uADCCONValue1	|= (uADCCONValue & ~(1<<2))|(ucSel<<2);
	ADCTS_SetRegValue(eADCTS_ADCCON, uADCCONValue1);
}

//////////
// Function Name : ADC_EnableReadStart
// Function Description : This function enables ADC start by read.
// Input : 	ucSel	- Enable/Disable Selector
//						0 : Disable start by read operation
//						1 : Enable start by read operation
// Output : 	NONE
// Version : v0.1
void ADC_EnableReadStart(u8 ucSel)
{
	u32 uADCCONValue=0;
	u32 uADCCONValue1=0;		// Added by SOP on 2008/05/28	

	uADCCONValue	= ADCTS_GetRegValue(eADCTS_ADCCON);
	uADCCONValue1	|= (uADCCONValue & ~(1<<1))|(ucSel<<1);
	ADCTS_SetRegValue(eADCTS_ADCCON, uADCCONValue1);
}

//////////
// Function Name : ADC_EnableStart
// Function Description : This function enables ADC start by enable.
//					  If READ_START bit is enabled, this value is not valid.
// Input : 	ucSel	- Enable/Disable Selector
//						0 : No operation
//						1 : Enable(A/D conversion starts and this bit is cleared automatically after the start-up.)
// Output : 	NONE
// Version : v0.1
void ADC_EnableStart(u8 ucSel)
{
	u32 uADCCONValue=0;
	u32 uADCCONValue1=0;		// Added by SOP on 2008/05/28	

	uADCCONValue	= ADCTS_GetRegValue(eADCTS_ADCCON);
	uADCCONValue1	|= (uADCCONValue & ~(1<<0))|(ucSel<<0);
	ADCTS_SetRegValue(eADCTS_ADCCON, uADCCONValue1);
}

//////////
// Function Name : ADC_IsAfterStartup
// Function Description : This function requests the result of ADC Enable Start.
// Input : 	NONE 
// Output : 	TRUE	- ADC Enable Started.(ENABLE_START bit of rADCCON is 0)
//			FALSE 	- A/D conversion starts in process
// Version : v0.1
u8 ADC_IsAfterStartup(void)
{
	u32 	uADCCONValue=0;
	u8	ucEnableStartFlag=0;

	uADCCONValue	= ADCTS_GetRegValue(eADCTS_ADCCON);
	ucEnableStartFlag	= (uADCCONValue & 0x1) ;

	if ( ucEnableStartFlag == 0 )
		return true;
	else 
		return false;
}
/*---------------------------------- APIs of rADCTSC ---------------------------------*/
//////////
// Function Name : ADCTS_SetMeasureMode
// Function Description : This function chooses the measurement mode of TouchScreen's position.
// Input : 	ucMeasureMode - X/Y Position Measurement mode of TouchScreen
// Output : 	NONE
// Version : v0.1
void ADCTS_SetMeasureMode(u8 ucMeasureMode)
{
	u32 uADCTSCValue=0;

	uADCTSCValue = ADCTS_GetRegValue(eADCTS_ADCTSC);
	
	switch(ucMeasureMode)
	{
		case eADCTS_MEASURE_NOP :
			//UART_Printf("eADCTS_MEASURE_NOP Mode Setting.\n");
			break;
		case eADCTS_MEASURE_MANUAL_X :
			uADCTSCValue	=	0x69;		// Edited by SOP on 2008/03/05
			//UART_Printf("eADCTS_MEASURE_MANUAL_X Mode Setting.(uADCTSCValue(0x69)= 0x%x)\n",uADCTSCValue);			
			break;
		case eADCTS_MEASURE_MANUAL_Y :
			uADCTSCValue	=	0x9a;		// Edited by SOP on 2008/03/05
			//UART_Printf("eADCTS_MEASURE_MANUAL_Y Mode Setting.(uADCTSCValue(0x9a)= 0x%x)\n",uADCTSCValue);				
			break;
		case eADCTS_MEASURE_AUTO_SEQ :
			uADCTSCValue	=	0x9c;		// Edited by SOP on 2008/03/05
			//UART_Printf("eADCTS_MEASURE_AUTO_SEQ Mode Setting.(uADCTSCValue(0x9c)= 0x%x)\n",uADCTSCValue);			
			break;
		case eADCTS_MEASURE_FOR_STYLUS :
			uADCTSCValue	=	0xd3;
			//UART_Printf("eADCTS_MEASURE_FOR_STYLUS Mode Setting.(uADCTSCValue(0xd3)= 0x%x)\n",uADCTSCValue);			
			break;
	}

	ADCTS_SetRegValue(eADCTS_ADCTSC, uADCTSCValue);
}

//////////
// Function Name : ADCTS_SetStylusDetectMode
// Function Description : This function select the method of  the result of ADC Enable Start.
// Input : 	ucMeasureMode - X/Y Position Measurement mode of TouchScreen
// Output : 	NONE
// Version : v0.1
void ADCTS_SetStylusDetectMode(u8 ucStylusDetectMode)
{
	u32	uADCTSCValue=0;
	u32 uADCTSCValue1=0;		// Added by SOP on 2008/05/28	

	uADCTSCValue	= ADCTS_GetRegValue(eADCTS_ADCTSC);
	uADCTSCValue1	|= (uADCTSCValue& ~(1<<8))|(ucStylusDetectMode<<8);

	ADCTS_SetRegValue(eADCTS_ADCTSC, uADCTSCValue1);
}

/*---------------------------------- APIs of rADCDLY ---------------------------------*/
//////////
// Function Name : ADCTS_SetDelayClkSrc
// Function Description : This function chooses the source of delay Clock.
// Input : 	uClkSrc - Clock Source Selection Number
//				0 - External input clock
//				1 - RTC clock
// Output : 	NONE
// Version : v0.1
void ADCTS_SetDelayClkSrc(u8 uClkSrc)
{
	u32	uADCDLYValue=0;
	u32	uADCDLYValue1=0;		// Added by SOP on 2008/05/28	

	uADCDLYValue	= ADCTS_GetRegValue(eADCTS_ADCDLY);
    	//UART_Printf("\n***  uADCDLYValue	= ADCTS_GetRegValue(eADCTS_ADCDLY) : %08x\n",uADCDLYValue);	// For debugging
	uADCDLYValue1	= (uADCDLYValue& ~(1<<16))|(uClkSrc<<16);	// Error correction by SOP on 04/05/2008	
   	//UART_Printf("***  uADCDLYValue	|= (uADCDLYValue& ~(1<<16))|(uClkSrc<<16) : %08x\n\n",uADCDLYValue);	// For debugging

	ADCTS_SetRegValue(eADCTS_ADCDLY, uADCDLYValue1);	
}

//////////
// Function Name : ADCTS_SetDelay
// Function Description : This function sets ADC start or interval delay.
// Input : 	uDelayNum - Clock Source Selection Number
// Output : 	NONE
// Version : v0.1
void ADCTS_SetDelay(u32 uDelayNum)
{
	u32 uADCDLYValue=0;
	u32 uADCDLYValue1=0;		// Added by SOP on 2008/05/28	

	uADCDLYValue = ADCTS_GetRegValue(eADCTS_ADCDLY);
	uADCDLYValue1 = (uADCDLYValue& ~(0xffff))|(uDelayNum&0xffff);

	ADCTS_SetRegValue(eADCTS_ADCDLY, uADCDLYValue1);	
}

/*---------------------------------- APIs of rADCDATx ---------------------------------*/
//////////
// Function Name : ADCTS_GetXPStylusIntState
// Function Description : This function gets the state of Stylus interrupt (Up/Down).
// Input : 	NONE
// Output : 	ucXPState - Stylus state of XP
// Version : v0.1
u8 ADCTS_GetXPStylusIntState(void)
{
	u32	uADCDAT0Value=0;
	u8	ucXPState=0;

	uADCDAT0Value	= ADCTS_GetRegValue(eADCTS_ADCDAT0);
	ucXPState		= (uADCDAT0Value& (1<<15)) >> 15;

	return ucXPState;
}

//////////
// Function Name : ADCTS_GetYPStylusIntState
// Function Description : This function gets the state of Stylus interrupt (Up/Down).
// Input : 	NONE
// Output : 	ucYPState - Stylus state of YP
// Version : v0.1
u8 ADCTS_GetYPStylusIntState(void)
{
	u32	uADCDAT1Value=0;
	u8	ucYPState=0;

	uADCDAT1Value	= ADCTS_GetRegValue(eADCTS_ADCDAT1);
	ucYPState		= (uADCDAT1Value& (1<<15)) >> 15;

	return ucYPState;
}

//////////
// Function Name : ADCTS_GetXPData
// Function Description : This function gets ADC Conversion data of X-Position.
// Input : 	NONE
// Output : 	(uADCDAT0Value & 0x3ff) - X-Position Conversiondata value(0~3FF)
// Version : v0.1
u32 ADCTS_GetXPData(void)
{
	u32 uADCDAT0Value=0;
	
	uADCDAT0Value = ADCTS_GetRegValue(eADCTS_ADCDAT0);
	return (uADCDAT0Value & 0xfff);		// Support for 12-bit by SOP on 2008/03/05
	//return uADCDAT0Value;				// For only test by SOP on 2008/05/19	
}


//////////
// Function Name : ADCTS_GetYPData
// Function Description : This function gets ADC Conversion data of Y-Position.
// Input : 	NONE
// Output : 	(uADCDAT1Value & 0x3ff) - Y-Position Conversiondata value(0~3FF)
// Version : v0.1
u32 ADCTS_GetYPData(void)
{
	u32 uADCDAT1Value=0;
	
	uADCDAT1Value = ADCTS_GetRegValue(eADCTS_ADCDAT1);
	return (uADCDAT1Value & 0xfff);		// Support for 12-bit by SOP on 2008/03/05
	//return uADCDAT1Value;				// For only test by SOP on 2008/05/19	
}


/*---------------------------------- APIs of INT Clear Registers ---------------------------------*/
//////////
// Function Name : ADCTS_SetADCClearInt
// Function Description : This function clears ADC Interrupt.
// Input : 	NONE
// Output : 	NONE
// Version : v0.1
void ADCTS_SetADCClearInt(void)
{
	u32	uADCCLRINTValue=0;

	uADCCLRINTValue	= ADCTS_GetRegValue(eADCTS_ADCCLRINT);
	uADCCLRINTValue	|= 0;
	ADCTS_SetRegValue(eADCTS_ADCCLRINT, uADCCLRINTValue);	
}

//////////
// Function Name : ADCTS_SetADCClearWKInt
// Function Description : This function clears ADCTS WakeUp Interrupt.
// Input : 	NONE
// Output : 	NONE
// Version : v0.1
void ADCTS_SetADCClearWKInt(void)
{
	u32	uADCCLRINTPNDNUPValue=0;

	uADCCLRINTPNDNUPValue	= ADCTS_GetRegValue(eADCTS_ADCCLRINTPNDNUP);
	uADCCLRINTPNDNUPValue	|= 0;
	ADCTS_SetRegValue(eADCTS_ADCCLRINTPNDNUP, uADCCLRINTPNDNUPValue);	
}

/*---------------------------------- APIs of general ADCTS ---------------------------------*/
//////////
// Function Name : ADCTS_GetRegValue
// Function Description : This function gets the value of ADCTS Registers.
// Input : 	ucADCTSRegId	- the Id of ADCTS Registers
// Output : 	*puADCTSReg	- the value of specified register
// Version : v0.1
u32 ADCTS_GetRegValue(u8 ucADCTSRegId)
{
	volatile u32 *	puADCTSBaseAddr;
	volatile u32 *	puADCTSReg;	

	puADCTSBaseAddr	= &(ADCTS->rADCCON);
	puADCTSReg		= puADCTSBaseAddr + ucADCTSRegId;

	return *puADCTSReg;	
}

//////////
// Function Name : ADCTS_SetRegValue
// Function Description : This function sets the value of ADCTS Registers.
// Input : 	ucADCTSRegId	- the Id of ADCTS Registers
//			uValue			- the value of register
// Output : 	NONE
// Version : v0.1
void ADCTS_SetRegValue(u8 ucADCTSRegId, u32 uValue)
{
	volatile u32 *	puADCTSBaseAddr;
	volatile u32 *	puADCTSReg;	

	puADCTSBaseAddr		= &(ADCTS->rADCCON);
	puADCTSReg			= puADCTSBaseAddr + ucADCTSRegId;

	*puADCTSReg		= uValue;
}


