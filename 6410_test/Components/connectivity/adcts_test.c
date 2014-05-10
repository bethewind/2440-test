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
*	File Name : adcts_test.c
*  
*	File Description : This file implements adcts test functions.
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
#include "gpio.h"

#include "adcts.h"
#include "intc.h"
#include "sysc.h"

//#include "autotest_api.h"	//api for autotest

#define rADCTSC				(ADCTS_BASE+0x004)

#define rINFORM0			(SYSCON_BASE+0xA00)
#define rINFORM1			(SYSCON_BASE+0xA04)
#define rINFORM2			(SYSCON_BASE+0xA08)
#define rINFORM3			(SYSCON_BASE+0xA0c)

extern volatile	ADCTS_oInform	g_oADCTSInform;
u32 ADC_Auto_ISR=0;		// Added by SOP on 2008/05/10

void ADCTS_Test(void);
u8 ADCTS_AutoTest(void);		// Added by SOP on 2008/05/10

void ADC_ConvertADCNormallyInPolling(void);
void ADC_ConvertADCNormallyInADCInterrupt(void);
void ADC_CheckADCLinearity(void);
void ADCTS_ConvertXYPosSeparatelyInPolling(void);
void ADCTS_ConvertXYPosSequentiallyInPolling(void);
void ADCTS_StylusCheckInPENUPDNInterrupt(void);
//void ADCTS_StylusTrackingUsingAutoConvPENDNIntADCPolling(void);
void ADCTS_CLKDIVTest(void);
void ADCTS_CLKGateTest(void);			// Added by SOP on 2008/04/04
void ADCTS_STOPCurrent (void);			// Added by SOP on 2008/04/07
void ADCTS_SLEEPCurrent (void);		// Added by SOP on 2008/04/07

void ADCTS_CLKDIV(u32 uHCLKx2_DIVN_R, u32 uPCLK_DIVN_R);


const testFuncMenu g_aADCTSTestFunc[] =
{
	// ADC
	ADC_ConvertADCNormallyInPolling,							"ADC Normal Conversion Mode by Polling",
	ADC_ConvertADCNormallyInADCInterrupt,						"ADC Normal Conversion Mode by Interrupt",
	ADC_CheckADCLinearity,										"ADC Linearity Check\n",

	// Touch Screen
	ADCTS_ConvertXYPosSeparatelyInPolling,						"ADCTS Separate Conversion Mode by Polling",
	ADCTS_ConvertXYPosSequentiallyInPolling,					"ADCTS Auto(Sequencial) Conversion Mode by Polling",
	ADCTS_StylusCheckInPENUPDNInterrupt,						"ADCTS Auto(Sequencial) Conversion Mode by Interrupt",	
	//ADCTS_StylusTrackingUsingAutoConvPENDNIntADCPolling,		"ADCTS Stylus Tracking Test using PenDown Interrupt In Auto(Sequencial) Conversion Mode(& ADC Polling)",
	ADCTS_CLKDIVTest,											"ADCTS Clock Divider Test",
	ADCTS_CLKGateTest,											"ADCTS Clock Gating (On/Off) Test\n",
	ADCTS_STOPCurrent,											"ADCTS STOP Mode Current Measurement Test",
	ADCTS_SLEEPCurrent,										"ADCTS SLEEP Mode Current Measurement Test",
	ADCTS_AutoTest,											"ADCTS Auto Test",	
    	0,0
};

void ADCTS_Test(void)
{
	u32 uCountFunc=0;
	s32 iSel=0;

	ADCTS_Init();

	while(1)
	{
	
#if RESSEL12BIT
		UART_Printf("\n\n================== ADC/TS Function Test (12-bit) [2008/05/28] =====================\n\n");	
#else
		UART_Printf("\n\n================== ADC/TS Function Test (10-bit) [2008/05/28] =====================\n\n");	
#endif

		for (uCountFunc=0; (u32)(g_aADCTSTestFunc[uCountFunc].desc)!=0; uCountFunc++)
			UART_Printf("%2d: %s\n", uCountFunc, g_aADCTSTestFunc[uCountFunc].desc);

		UART_Printf("\nSelect the function to test : ");
		iSel =UART_GetIntNum();
		UART_Printf("\n");
		if(iSel == -1) 
			break;

		if (iSel>=0 && iSel<(sizeof(g_aADCTSTestFunc)/8-1))
			(g_aADCTSTestFunc[iSel].func) ();
	}
}	

//////////
// Function Name : ADC_ConvertADCNormallyInPolling
// Function Description : This function tests ADC Normal Conversion in Polling mode.
// Input : 	NONE 
// Output : 	NONE
// Version : v0.1
void ADC_ConvertADCNormallyInPolling(void)
{
	u8	uADCCh;
	u32 uCnt	;
	u32 ucLoop = 10;

#if RESSEL12BIT
	UART_Printf("\n[ ADC Normal Conversion Mode Test(ADC Polling) - 12-bit ]\n\n");    
#else
	UART_Printf("\n[ ADC Normal Conversion Mode Test(ADC Polling) - 10-bit ]\n\n"); 
#endif
	
	UART_Printf("ADC conv. freq.= %d Hz\n\n",(u32)(g_PCLK/(ADCTS_PRSCVL+1.)));
	ADC_InitADCCON();

	for(uCnt=0 ; uCnt<ucLoop; uCnt++) 
	{
		for(uADCCh=0 ; uADCCh<8; uADCCh++) 
		{
			ADC_SelectADCCh(uADCCh);	//	Channel setup
			ADC_EnableStart(ENABLE);

			while (! ADC_IsAfterStartup() ) ;		//	Wait for begin sampling
			while (! (ADC_IsEOCFlag()));			//	Wait for the EOC

			ADC_EnableStart(DISABLE);				
			UART_Printf("AIN[%d]:%04d   ", uADCCh, ADCTS_GetXPData());
		}
		UART_Printf("\n");		
	}
}

//////////
// Function Name : ADC_ConvertADCNormallyInADCInterrupt
// Function Description : This function tests ADC Normal Conversion in Interrupt mode.
// Input : 	NONE 
// Output : 	NONE
// Version : v0.1
void ADC_ConvertADCNormallyInADCInterrupt(void)
{
	u8	ucADCCh = 0;
	u32 uCnt	;
	u32 ucLoop = 10;	
    
#if RESSEL12BIT
	UART_Printf("\n[ ADC Normal Conversion Mode Test(ADC Interrupt) - 12-bit ]\n\n");    
#else
	UART_Printf("\n[ ADC Normal Conversion Mode Test(ADC Interrupt) - 10-bit ]\n\n"); 
#endif
   
	UART_Printf("ADC conv. freq.= %d(Hz)\n\n",(u32)(g_PCLK/(ADCTS_PRSCVL+1.)));
	ADC_InitADCCON();
	INTC_SetVectAddr(NUM_ADC, ADC_NormalISR);
	INTC_Enable(NUM_ADC);

	for(uCnt=0 ; uCnt<ucLoop; uCnt++) 
	{
		for(ucADCCh = 0 ;ucADCCh<8; ucADCCh++) 
		{
			ADC_SelectADCCh(ucADCCh);	//	Channel setup
			g_oADCTSInform.ucADCflag = 0;
			ADC_EnableStart(ENABLE);
			
			while(g_oADCTSInform.ucADCflag == 0);

			UART_Printf("AIN[%d]:%04d   ",ucADCCh, g_oADCTSInform.uADCSampleData);
		}
		UART_Printf("\n");
	}
	INTC_Disable(NUM_ADC);
}



//////////
// Function Name : ADC_CheckADCLinearity
// Function Description : This function tests ADC Conversion Linearity.
// Input : 	NONE 
// Output : 	NONE
// Version : v0.1
void ADC_CheckADCLinearity(void)
{
	u8	ucADCChannelCnt 	= 0;
	u32	ucLoopCnt			= 1;
	u32	uTotADCValue		= 0;
	//u32	ucLoopMax			= 1000;
	u32	ucLoopMax			= 100000;	

#if RESSEL12BIT
	UART_Printf("\n[ ADC Linearity Check - 12-bit ]\n\n");    
#else
	UART_Printf("\n[ ADC Linearity Check - 10-bit ]\n\n"); 
#endif
		
	UART_Printf("Push any key to exit!!!\n");    
	UART_Printf("ADC conv. freq.= %d Hz\n",(u32)(g_PCLK/(ADCTS_PRSCVL+1.)));

	while(!GetKey()) 
	{
		ucLoopCnt 	 = 1;
		uTotADCValue	 = 0;

		ADC_InitADCCON();
		
		UART_Printf("\nInput ADC Channel No. ( 0 ~ 7 ) [%d] : ",ucADCChannelCnt);
	 	ucADCChannelCnt = UART_GetIntNum(); 

		if ( ucADCChannelCnt > 7 )
			break;

		while(ucLoopCnt < (ucLoopMax+1))
		{
			ADC_SelectADCCh(ucADCChannelCnt);	//	Channel setup
			ADC_EnableStart(ENABLE);

			while (! ADC_IsAfterStartup() ) ;		//	Wait for begin sampling

			while (! (ADC_IsEOCFlag()));			//	Wait for the EOC

			uTotADCValue += ADCTS_GetXPData();
			//UART_Printf("Loop: %d ,  AIN [%d] : %04d \n",ucLoopCnt, ucADCChannelCnt, (u32)(uTotADCValue/ucLoopCnt));			
			ucLoopCnt++;
		}
		UART_Printf("------------------------------------------------------------- \n");
		UART_Printf("Loop: %d ,  AIN [%d] Average: %04d \n",ucLoopCnt-1, ucADCChannelCnt, (u32)(uTotADCValue/(ucLoopCnt-1)));
	}
}


//////////
// Function Name : ADCTS_ConvertXYPosSeparatelyInPolling
// Function Description : This function tests ADCTS Manual Conversion of X/Y Position in Polling mode.
// Input : 	NONE 
// Output : 	NONE
// Version : v0.1
void ADCTS_ConvertXYPosSeparatelyInPolling(void)
{
	u32 uXDat = 0;
	u32 uYDat = 0;

#if RESSEL12BIT
	UART_Printf("\n[ ADCTS Separate Conversion Mode by Polling - 12-bit ]\n\n");    
#else
	UART_Printf("\n[ ADCTS Separate Conversion Mode by Polling - 10-bit ]\n\n"); 
#endif
   
   	while(GetKey()==0) 
	{
		//------- Get X-Position Data -------------
		ADCTS_SetMeasureMode(eADCTS_MEASURE_MANUAL_X);
		ADC_InitADCCON();
		ADC_SelectADCCh(5);		// Channel Setup
		ADC_EnableStart(ENABLE);

		while(! (ADC_IsAfterStartup() )) ;	//	Wait for begin sampling
		while (! (ADC_IsEOCFlag()));		//	Wait for the EOC

		uXDat	= ADCTS_GetXPData();

		//------- Get Y-Position Data -------------
		ADCTS_SetMeasureMode(eADCTS_MEASURE_MANUAL_Y);
		ADC_InitADCCON();
		ADC_SelectADCCh(7);		// Channel Setup
		ADC_EnableStart(ENABLE);

		while(! (ADC_IsAfterStartup() )) ;	//	Wait for begin sampling
		while (! (ADC_IsEOCFlag()));		//	Wait for the EOC

		uYDat	= ADCTS_GetYPData();

		UART_Printf("X: %04d  , Y: %04d\n",uXDat,uYDat);
	}
}

//////////
// Function Name : ADCTS_ConvertXYPosSequentiallyInPolling
// Function Description : This function tests ADCTS Auto Sequential Conversion of X/Y Position in Polling mode.
// Input : 	NONE 
// Output : 	NONE
// Version : v0.1
void ADCTS_ConvertXYPosSequentiallyInPolling(void)
{
	u32 uXDat = 0;
	u32 uYDat = 0;

#if RESSEL12BIT
	UART_Printf("\n[ ADCTS Auto(Sequencial) Conversion Mode by Polling - 12-bit ]\n\n");    
#else
	UART_Printf("\n[ ADCTS Auto(Sequencial) Conversion Mode by Polling - 10-bit ]\n\n"); 
#endif
	
   	while(GetKey()==0) 
	{
		ADCTS_SetMeasureMode(eADCTS_MEASURE_AUTO_SEQ);
		ADC_InitADCCON();
		ADC_EnableStart(ENABLE);

		while(! (ADC_IsAfterStartup() )) ;	//	Wait for begin sampling
		while (! (ADC_IsEOCFlag()));		//	Wait for the EOC

		uXDat	= ADCTS_GetXPData();
		uYDat	= ADCTS_GetYPData();

		UART_Printf("X: %04d  , Y: %04d\n",uXDat,uYDat);
   	}
}

//////////
// Function Name : ADCTS_StylusCheckInPENUPDNInterrupt
// Function Description : This function tests ADCTS Stylus Check Using PenDown Interrupt.
// Input : 	NONE 
// Output : 	NONE
// Version : v0.1
#if 1
void ADCTS_StylusCheckInPENUPDNInterrupt(void)
{
	u32 uXDat = 0;
	u32 uYDat = 0;
	
	g_oADCTSInform.ucTouchStatus = ADCTS_TOUCH_INIT;
	ADC_InitADCCON();
	//ADCTS_SetDelay(50000);		// ADCDLY[15:0]
	//ADCTS_SetDelay(10000);		// ADCDLY[15:0]	
	//ADCTS_SetDelay(255);		// ADCDLY[15:0], Reset Default Value = 0xff (255)	
	//ADCTS_SetDelay(5000);		// ADCDLY[15:0]
	//ADCTS_SetDelay(1000);		// ADCDLY[15:0]	
	ADCTS_SetDelay(500);		// ADCDLY[15:0]		

	//ADCTS_SetDelayClkSrc(0);	// 0: External input clock (XXTI or XEXTCLK), 1: RTC clock
	ADCTS_SetDelayClkSrc(1);	// 0: External input clock (XXTI or XEXTCLK), 1: RTC clock		

#if RESSEL12BIT
	UART_Printf("\n[ ADCTS Auto(Sequencial) Conversion Mode by Interrupt - 12-bit ]\n\n");    
#else
	UART_Printf("\n[ ADCTS Auto(Sequencial) Conversion Mode by Interrupt - 10-bit ]\n\n"); 
#endif
	
	ADCTS_SetMeasureMode(eADCTS_MEASURE_FOR_STYLUS);
	Delay(10000);
	INTC_SetVectAddr(NUM_PENDNUP, ADCTS_StylusTrackingISR);	
	INTC_Enable(NUM_PENDNUP);

    	UART_Printf("\nPress any key to exit!!!\n");
	UART_Printf("Stylus Down, please...... \n");

#if 0	
	while(!GetKey())
	{
		if (g_oADCTSInform.ucTouchStatus == ADCTS_TOUCH_DOWN)
		{
    			UART_Printf("*** Stylus Downed.\n");	// Added by SOP on 2008/04/02	
		if ( ADCTS_GetXPStylusIntState() == STYLUS_DOWN)
				ADCTS_SetMeasureMode(eADCTS_MEASURE_FOR_STYLUS); 

				ADCTS_SetStylusDetectMode(STYLUS_UP);
				Delay(300);			
		}
		else if (g_oADCTSInform.ucTouchStatus == ADCTS_TOUCH_UP)
		{
    			UART_Printf("*** Stylus Uped.\n");		// Added by SOP on 2008/04/02			
			if ( ADCTS_GetXPStylusIntState() == STYLUS_UP )
				ADCTS_SetMeasureMode(eADCTS_MEASURE_FOR_STYLUS);
		}
	}
#else					
	while(!GetKey())
	{
		if (g_oADCTSInform.ucTouchStatus == ADCTS_TOUCH_DOWN)
		{
    			//UART_Printf("*** Stylus Downed.\n");	// Added by SOP on 2008/04/02	
			if ( ADCTS_GetXPStylusIntState() == STYLUS_DOWN)
				ADCTS_SetMeasureMode(eADCTS_MEASURE_AUTO_SEQ); 

				ADC_EnableStart(ENABLE);

				while(! (ADC_IsAfterStartup() )) ;	//	Wait for begin sampling
				while (! (ADC_IsEOCFlag()));		//	Wait for the EOC

				uXDat	= ADCTS_GetXPData();
				uYDat	= ADCTS_GetYPData();

				UART_Printf("X: %04d  , Y: %04d\n",uXDat,uYDat);
				ADCTS_SetMeasureMode(eADCTS_MEASURE_FOR_STYLUS);				
				ADCTS_SetStylusDetectMode(STYLUS_UP);
				Delay(300);						
		}
		else if (g_oADCTSInform.ucTouchStatus == ADCTS_TOUCH_UP)
		{
    			//UART_Printf("*** Stylus Uped.\n");		// Added by SOP on 2008/04/02			
			if ( ADCTS_GetXPStylusIntState() == STYLUS_UP )
			{
				//UART_Printf("ADCDAT X: %x  , Y: %x  , ADCUPDN: %x\n",ADCTS_GetXPData(),ADCTS_GetYPData(),ADCTS_GetRegValue(eADCTS_ADCUPDN));	// Only Test by SOP on 2008/05/19								
				ADCTS_SetMeasureMode(eADCTS_MEASURE_FOR_STYLUS);
			}				
		}
	}

#endif
	
	INTC_Disable(NUM_PENDNUP);
	UART_Printf("\n\n--- ADCTS Auto(Sequencial) Conversion Mode by Interrupt test End.\n");		
}

#else				// Only for Test by SOP on 2008/05/19
void ADCTS_StylusCheckInPENUPDNInterrupt(void)
{
	u32 uXDat = 0;
	u32 uYDat = 0;
	
	g_oADCTSInform.ucTouchStatus = ADCTS_TOUCH_INIT;
	ADC_InitADCCON();
	ADCTS_SetDelay(500);		// ADCDLY[15:0]		

	//ADCTS_SetDelayClkSrc(0);	// 0: External input clock (XXTI or XEXTCLK), 1: RTC clock
	ADCTS_SetDelayClkSrc(1);	// 0: External input clock (XXTI or XEXTCLK), 1: RTC clock		

#if RESSEL12BIT
	UART_Printf("\n[ ADCTS Auto(Sequencial) Conversion Mode by Interrupt - 12-bit ]\n\n");    
#else
	UART_Printf("\n[ ADCTS Auto(Sequencial) Conversion Mode by Interrupt - 10-bit ]\n\n"); 
#endif
	
	ADCTS_SetMeasureMode(eADCTS_MEASURE_FOR_STYLUS);
	Delay(10000);
	INTC_SetVectAddr(NUM_PENDNUP, ADCTS_StylusTrackingISR);	
	INTC_Enable(NUM_PENDNUP);

    	UART_Printf("\nPress any key to exit!!!\n");
	UART_Printf("Stylus Down, please...... \n");

	while(!GetKey())
	{
		if (g_oADCTSInform.ucTouchStatus == ADCTS_TOUCH_UP)
		{
    			//UART_Printf("*** Stylus Downed.\n");	// Added by SOP on 2008/04/02	
			if ( ADCTS_GetXPStylusIntState() == STYLUS_UP)
				ADCTS_SetMeasureMode(eADCTS_MEASURE_AUTO_SEQ); 

				ADC_EnableStart(ENABLE);

				while(! (ADC_IsAfterStartup() )) ;	//	Wait for begin sampling
				while (! (ADC_IsEOCFlag()));		//	Wait for the EOC

				uXDat	= ADCTS_GetXPData();
				uYDat	= ADCTS_GetYPData();

				UART_Printf("X: %04d  , Y: %04d\n",uXDat,uYDat);
				//UART_Printf("X: 0x%x  , Y: 0x%x  , ADCUPDN: %x\n",uXDat,uYDat,ADCTS_GetRegValue(eADCTS_ADCUPDN));	// Only Test by SOP on 2008/05/19
				
				ADCTS_SetMeasureMode(eADCTS_MEASURE_FOR_STYLUS);				
				ADCTS_SetStylusDetectMode(STYLUS_DOWN);
				Delay(300);						
		}
		else if (g_oADCTSInform.ucTouchStatus == ADCTS_TOUCH_DOWN) 
		{
    			//UART_Printf("*** Stylus Uped.\n");		// Added by SOP on 2008/04/02			
			if ( ADCTS_GetXPStylusIntState() == STYLUS_DOWN )
			{
				UART_Printf("ADCDAT X: %x  , Y: %x  , ADCUPDN: %x\n",ADCTS_GetXPData(),ADCTS_GetYPData(),ADCTS_GetRegValue(eADCTS_ADCUPDN));	// Only Test by SOP on 2008/05/19					
				ADCTS_SetMeasureMode(eADCTS_MEASURE_FOR_STYLUS);
			}				
		}
	}
	
	INTC_Disable(NUM_PENDNUP);
	UART_Printf("\n\n--- ADCTS Auto(Sequencial) Conversion Mode by Interrupt test End.\n");		
}
#endif


/*
//////////
// Function Name : ADCTS_StylusTrackingUsingAutoConvPENDNIntADCPolling
// Function Description : This function tests ADCTS Auto Sequential Conversion of X/Y Position in Polling mode Using PenDown Interrupt.
// Input : 	NONE 
// Output : 	NONE
// Version : v0.1
void ADCTS_StylusTrackingUsingAutoConvPENDNIntADCPolling(void)
{
	u32 uXDat = 0;
	u32 uYDat = 0;

#if RESSEL12BIT
	UART_Printf("\n\n[ ADCTS Stylus Tracking Test using PenDown Interrupt In Auto(Sequencial) Conversion Mode (& ADC Polling) - 12bit ]\n");
#else
	UART_Printf("\n\n[ ADCTS Stylus Tracking Test using PenDown Interrupt In Auto(Sequencial) Conversion Mode (& ADC Polling) - 10-bit ]\n");
#endif

	g_oADCTSInform.ucTouchStatus = ADCTS_TOUCH_INIT;
	ADC_InitADCCON();
	//ADCTS_SetDelay(50000);		// ADCDLY[15:0]
	//ADCTS_SetDelay(10000);		// ADCDLY[15:0]	
	//ADCTS_SetDelay(255);		// ADCDLY[15:0], Reset Default Value = 0xff (255)	
	ADCTS_SetDelay(5000);		// ADCDLY[15:0]
	//ADCTS_SetDelay(1000);		// ADCDLY[15:0]	
	//ADCTS_SetDelay(500);		// ADCDLY[15:0]		

	//ADCTS_SetDelayClkSrc(0);	// 0: External input clock (XXTI or XEXTCLK), 1: RTC clock
	//ADCTS_SetDelayClkSrc(1);	// 0: External input clock (XXTI or XEXTCLK), 1: RTC clock		

	ADCTS_SetMeasureMode(eADCTS_MEASURE_FOR_STYLUS);

	Delay(10000);

	INTC_SetVectAddr(NUM_PENDNUP, ADCTS_StylusTrackingISR);
	INTC_Enable(NUM_PENDNUP);

    	UART_Printf("\nPress any key to exit!!!\n");
    	UART_Printf("\nStylus Down, please...... \n");

	while(!GetKey())
	{
		if (g_oADCTSInform.ucTouchStatus == ADCTS_TOUCH_DOWN)
		{
    			//UART_Printf("*** Stylus Downed.\n");	// Added by SOP on 2008/04/02			
			while( ADCTS_GetXPStylusIntState() == STYLUS_DOWN )
			{
    				//UART_Printf("*** Stylus Downed.\n");	// Added by SOP on 2008/04/02			
				//ADC_InitADCCON();
				ADCTS_SetMeasureMode(eADCTS_MEASURE_AUTO_SEQ);

				ADC_EnableStart(ENABLE);

				while(! (ADC_IsAfterStartup() )) ;	//	Wait for begin sampling
				while (! (ADC_IsEOCFlag()));		//	Wait for the EOC

				uXDat	= ADCTS_GetXPData();
				uYDat	= ADCTS_GetYPData();

				UART_Printf("X: %04d  , Y: %04d\n",uXDat,uYDat);
				ADCTS_SetMeasureMode(eADCTS_MEASURE_FOR_STYLUS);
				ADCTS_SetStylusDetectMode(STYLUS_UP);
				//Delay(300);
				Delay(100);				
			}
		}
		else if (g_oADCTSInform.ucTouchStatus == ADCTS_TOUCH_UP)
		{
    			//UART_Printf("*** Stylus Uped.\n");		// Added by SOP on 2008/04/02			
			if ( ADCTS_GetXPStylusIntState() == STYLUS_UP )
				ADCTS_SetMeasureMode(eADCTS_MEASURE_FOR_STYLUS);
		}
		//Delay(500);		// For test by SOP 04/05/2008		
	}
	INTC_Disable(NUM_PENDNUP);
	UART_Printf("\n\n--- ADCTS touch screen Tracking test End.\n");	
}
*/

//////////
// Function Name : ADCTS_CLKDIVTest
// Function Description : This function tests ADCTS Auto Sequential Conversion of X/Y Position in Polling mode Using PenDown Interrupt.
// Input : 	NONE 
// Output : 	NONE
// Version : v0.1
void ADCTS_CLKDIVTest(void)
{

#if RESSEL12BIT
	UART_Printf("\n\n[ ADCTS Clock Divider Test - 12bit ]\n");
#else
	UART_Printf("\n\n[ ADCTS Clock Divider Test - 10bit ]\n");
#endif

	ClockInform();

	ADCTS_CLKDIV(6,  5);		// If ARMCLK=532MHz, HCLKX2=76.00MHz, HCLK=38.00MHz, PCLK=12.67MHz
	Pause();
	ADCTS_CLKDIV(6,  1);		// If ARMCLK=532MHz, HCLKX2=76.00MHz, HCLK=38.00MHz, PCLK=38.00MHz
	Pause();	
	ADCTS_CLKDIV(1, 13);	// If ARMCLK=532MHz, HCLKX2=266.00MHz, HCLK=133.00MHz, PCLK=19.00MHz
	Pause();	
	ADCTS_CLKDIV(7,  7);		// If ARMCLK=532MHz, HCLKX2=66.50MHz, HCLK=33.25MHz, PCLK=8.31MHz
	Pause();	
	ADCTS_CLKDIV(7, 11);	// If ARMCLK=532MHz, HCLKX2=66.50MHz, HCLK=33.25MHz, PCLK=5.54MHz
	Pause();	
	ADCTS_CLKDIV(1,  5);		// If ARMCLK=532MHz, HCLKX2=266.00MHz, HCLK=133.00MHz, PCLK=44.33MHz
	Pause();
	ADCTS_CLKDIV(6, 11);	// If ARMCLK=532MHz, HCLKX2=76.00MHz, HCLK=38.00MHz, PCLK=6.33MHz	
	Pause();	
	
	//SYSC_SetDIV0(uAratio, uMratio, uHratio, uHx2ratio, uPratio, uONDratio, uSECUratio, uCAMratio, uJPEGratio, uMFCratio );
	SYSC_SetDIV0(0, 1, 1, 1,  3, 1, 1,0, 1, 0);
	UART_InitDebugCh(0, 115200);
	Delay(100);
	ClockInform();

	UART_Printf("\n\n--- ADCTS Clock Divider test End.\n");	
}

//////////
// Function Name : ADCTS_CLKDIV
// Function Description : This function is Clock Divider control.
// Input : HCLKX2, PCLK Divider Value
// Output : None
// Version : v0.1
void ADCTS_CLKDIV(u32 uHCLKx2_DIVN_R, u32 uPCLK_DIVN_R)
{
	u32 uXDat = 0;
	u32 uYDat = 0;

	//SYSC_SetDIV0(uAratio, uMratio, uHratio, uHx2ratio, uPratio, uONDratio, uSECUratio, uCAMratio, uJPEGratio, uMFCratio );
	SYSC_SetDIV0(0, 1, 1, uHCLKx2_DIVN_R,  uPCLK_DIVN_R, 1, 1,0, 1, 0);
	UART_InitDebugCh(0, 115200);
	Delay(100);

	UART_Printf("\nHCLKX2 Divide Value: %d ,  PCLK Divider Value: %d\n", uHCLKx2_DIVN_R, uPCLK_DIVN_R);
	ClockInform();

	g_oADCTSInform.ucTouchStatus = ADCTS_TOUCH_INIT;
	ADC_InitADCCON();
	ADCTS_SetDelay(50000);

	ADCTS_SetMeasureMode(eADCTS_MEASURE_FOR_STYLUS);

	Delay(10000);

	INTC_SetVectAddr(NUM_PENDNUP, ADCTS_StylusTrackingISR);
	INTC_Enable(NUM_PENDNUP);

    	UART_Printf("\nPress any key to exit!!!\n");
    	UART_Printf("\nStylus Down, please...... \n");

	while(!GetKey())
	{
		if (g_oADCTSInform.ucTouchStatus == ADCTS_TOUCH_DOWN)
		{
			while( ADCTS_GetXPStylusIntState() == STYLUS_DOWN )
			{
				ADC_InitADCCON();
				ADCTS_SetMeasureMode(eADCTS_MEASURE_AUTO_SEQ);

				ADC_EnableStart(ENABLE);

				while(! (ADC_IsAfterStartup() )) ;	//	Wait for begin sampling
				while (! (ADC_IsEOCFlag()));		//	Wait for the EOC

				uXDat	= ADCTS_GetXPData();
				uYDat	= ADCTS_GetYPData();

				UART_Printf("X: %04d  , Y: %04d\n",uXDat,uYDat);
				ADCTS_SetMeasureMode(eADCTS_MEASURE_FOR_STYLUS);
				ADCTS_SetStylusDetectMode(STYLUS_UP);
				Delay(300);
			}
		}
		else if (g_oADCTSInform.ucTouchStatus == ADCTS_TOUCH_UP)
		{
			if ( ADCTS_GetXPStylusIntState() == STYLUS_UP )
				ADCTS_SetMeasureMode(eADCTS_MEASURE_FOR_STYLUS);
		}
	}
	INTC_Disable(NUM_PENDNUP);
}


//////////
// Function Name : ADCTS_CLKGateTest
// Function Description : This function tests is Clock gating control.
// Input : NONE
// Output : None
// Version : v0.1
void ADCTS_CLKGateTest(void)			// Added by SOP on 2008/04/04
{
	UART_Printf("[ ADCTS Clock Gating Control Test ]\n");
	ClockInform();

	ADCTS_CLKDIV(6,  5);		// If ARMCLK=532MHz, HCLKX2=76.00MHz, HCLK=38.00MHz, PCLK=12.67MHz
	UART_Printf("\nTSADC OFF (PCLK_GATE[12])\n");
	SYSC_CtrlPCLKGate(ePCLK_TSADC, 0);
	UART_Printf("rHCLK_GATE: 0x%x, rPCLK_GATE: 0x%x, rSCLK_GATE: 0x%x \n", Inp32SYSC(0x30), Inp32SYSC(0x34), Inp32SYSC(0x38));
	Pause();

	ADCTS_CLKDIV(1,  5);	// If ARMCLK=532MHz, HCLKX2=266.00MHz, HCLK=133.00MHz, PCLK=44.33MHz
	Pause();

	UART_Printf("\nTSADC ON (PCLK_GATE[12])\n");
	SYSC_CtrlPCLKGate(ePCLK_TSADC, 1);
	UART_Printf("rHCLK_GATE: 0x%x, rPCLK_GATE: 0x%x, rSCLK_GATE: 0x%x \n", Inp32SYSC(0x30), Inp32SYSC(0x34), Inp32SYSC(0x38));

	ADCTS_CLKDIV(6,  1);		// If ARMCLK=532MHz, HCLKX2=76.00MHz, HCLK=38.00MHz, PCLK=38.00MHz
	
	SYSC_SetDIV0(0, 1, 1, 1,  3, 1, 1,0, 1, 0);
	UART_InitDebugCh(0, 115200);
	Delay(100);
	ClockInform();
}


//////////
// Function Name : ADCTS_STOPCurrent
// Function Description : ADCTS Stop Mode Current Test 
// Input : 	None
// Output :	None 
// Version : v0.1
void ADCTS_STOPCurrent (void)			// Added by SOP on 04/07/2008
{
	u32 uRegValue, uCnt;

	u32 uPclkGate, uHclkGate, uSclkGate;
	u32 uNormalCfg, uStopCfg, uStopMemCfg;
	u32 uADCTSC[18] = {0x58, 0xd0, 0x70, 0x28, 0x20, 0x68, 0xc8, 0xf8, 0xf0, 0x98, 0x8, 0x0, 0xd3, 0xd8, 0x18, 0x78, 0x48, 0x50};
	
	UART_Printf("\n[ ADCTS STOP Mode Current Test ]\n");
	UART_Printf("S3C6410 will also wake up by RTC Alarm (After 10sec).\n");

	for(uCnt=0;uCnt<18;uCnt++)
	{
		UART_Printf("\nTest pattern for SDRAM Self-Refresh is filled!\n");
		SYSC_InitData_SDRAM(_DRAM_BaseAddress+0x1000000, 0x10000);

		UART_Printf("Wake-up source is set!\n");
		//--------------------------------
		// Wake-up Source Setting Wake-up Source Interrupts are must enabled at Stop Mode
		//--------------------------------
		SYSC_SetAlarmWakeup();		
		SYSC_SetADCTSWakeup();

		SYSC_OSCSTAB(1);			

		SYSC_SetSTOPCFG(1, 1, 1, 1);		// Set ARM_LOGIC/MEM, TOP_LOGIC/MEM to "1"
		SYSC_SetSTOPMEMCFG(1, 1, 1, 1, 1, 1, 1);	

		Outp32(rADCTSC, uADCTSC[uCnt]);
		UART_Printf("ADCTSC Register Setting Value: 0x%x\n",Inp32(rADCTSC));
		
		UART_Printf("CPU will go to Stop Mode!\n");
		UART_TxEmpty();

		SYSC_SetCFGWFI(eSTOP, 0);
		MMU_WaitForInterrupt();
		
		UART_Printf("\nCPU will go to Normal Mode!\n");
		SYSC_ClrWKUPSTAT();	
		SYSC_CheckData_SDRAM(_DRAM_BaseAddress+0x1000000, 0x10000);
		Pause();		
	}
	
	INTC_Disable(NUM_RTC_ALARM);
	INTC_Disable(NUM_PENDNUP);
}



//////////
// Function Name : ADCTS_SLEEPCurrent
// Function Description : Sleep Mode ADCTS Current Test - Baisc Test 
// Input : 	None
// Output :	None 
// Version : v0.1
void ADCTS_SLEEPCurrent (void)			// Added by SOP on 2008/04/07
{
	u32 uRstId;	
	u32 ucnt, uCount;
	u32 uADCTSC[18] = {0x58, 0xd0, 0x70, 0x28, 0x20, 0x68, 0xc8, 0xf8, 0xf0, 0x98, 0x8, 0x0, 0xd3, 0xd8, 0x18, 0x78, 0x48, 0x50};
	//u32 uADCTSC = 0x70;	
	
	UART_Printf("\n[ ADCTS SLEEP Mode Current Test ]\n");
	UART_Printf("S3C6410 will also wake up by RTC Alarm (After 10sec).\n");

	UART_Printf("[%d]%d-th Test!\n", Inp32(rINFORM1), Inp32(rINFORM2));

	uRstId = SYSC_RdRSTSTAT(1);

	if( Inp32(rINFORM2)==0 )
	{
		UART_Printf("Test pattern for SDRAM Self-Refresh is filled!\n");
		SYSC_InitData_SDRAM(_DRAM_BaseAddress+0x1000000, 0x10000);
	}
	else if((Inp32(rINFORM2)!=0) ) 
	{
		UART_Printf("Wake-up form Sleep Mode \n");
		SYSC_CheckData_SDRAM(_DRAM_BaseAddress+0x1000000, 0x10000);
		SYSC_ClrWKUPSTAT();
		Outp32(rINFORM3, 0x0);
	}	

	ucnt=Inp32(rINFORM2);
	ucnt++;
	
	if(ucnt==0xffffffff) 
	{
		ucnt=0;
		Outp32(rINFORM1, (Inp32(rINFORM1))++);
	}	
	Outp32(rINFORM2, ucnt);	

	if(Inp32(rINFORM0)>=18) 
	{
		Outp32(rINFORM0, 0x0);
		Outp32(rADCTSC, uADCTSC[ Inp32(rINFORM0)]);
		UART_Printf("*** rINFORM0 Register Count Value: %d\n",Inp32(rINFORM0));
		Outp32(rINFORM0, ++Inp32(rINFORM0));	
	}		
	else
	{
		UART_Printf("*** rINFORM0 Register Count Value: %d\n",Inp32(rINFORM0));		
		Outp32(rADCTSC, uADCTSC[ Inp32(rINFORM0)]);		
		Outp32(rINFORM0, ++Inp32(rINFORM0));	
	}			

	// ADC & RTC OFF		
	ADC_SelectStandbyMode(1);		//ADC Stand-by
	UART_Printf("ADCTSC Register Setting Value: 0x%x\n",Inp32(rADCTSC));	
	//-daedoo
	//RTC_SetCON(0,0,0,0,0,0);			//RTC SFR R/W Disable.
	RTC_Enable(false);
	
	UART_Printf("Wake-up source is set!\n");
	//--------------------------------
	// Wake-up Source Setting, Wake-up Source Interrupts are must enabled at Stop Mode
	//--------------------------------
	SYSC_SetAlarmWakeup();
	UART_Printf("System stablilization counter register!\n");
	//--------------------------------
	// OSCSTABLE = Clock Control,  PWRSTABLE = Reset Control
	//--------------------------------	
	SYSC_OSCSTAB(0x1);			
	SYSC_PWRSTAB(0x1);			

	//--------------------------------
	// Enter the Stop Mode
	// 		1. Setting "CFG_STANDBYWFI" to "2'b11"  ( PWR_CFG(0x7E00_F804)
	//          2. Setting "SLEEP_CFG"(0x7E00_F818) ->  OSC_EN ="0"
	//          2.  Enter to the STANDBYWFI Command
	//--------------------------------
	UART_Printf("PWRCFG:0x%x, EINTMASK:0x%x, NORMALCFG:0x%x, STOPCFG:0x%x\n", Inp32SYSC(0x804),Inp32SYSC(0x808),Inp32SYSC(0x810),Inp32SYSC(0x814));
	UART_Printf(" SLEEPCFG:0x%x, STOPMEMCFG:0x%x, OTHERS:0x%x, BLKPWRSTAT:0x%x\n",Inp32SYSC(0x818),Inp32SYSC(0x81c),Inp32SYSC(0x900),Inp32SYSC(0x90c)); 
	UART_Printf("\nCPU will go to SLEEP Mode!\n\n");
	UART_TxEmpty(); 
	Outp32(rINFORM3, 0xDEADDEAD);
	SYSC_SetCFGWFI(eSLEEP, 0);		
	MMU_WaitForInterrupt();
	
	UART_Printf("CPU doesn't go to Sleep Mode\n");

	Pause();
}

//////////
// Function Name : ADCTS_StylusTrackingISR_Auto
// Function Description : This function is a pen-down interrupt handler of ADCTS_StylusTrackingUsingAutoConversionInPENDNInterruptADCPolling().
// Input : 	NONE 
// Output : 	NONE
// Version : v0.1
void __irq	ADCTS_StylusTrackingISR_Auto(void)
{
	if ( ADCTS_GetXPStylusIntState() == STYLUS_DOWN )
		g_oADCTSInform.ucTouchStatus = ADCTS_TOUCH_DOWN;
	else 
		g_oADCTSInform.ucTouchStatus = ADCTS_TOUCH_UP;

	ADCTS_SetADCClearInt();
	ADCTS_SetADCClearWKInt();
	INTC_ClearVectAddr();
	ADC_Auto_ISR =1;
}

//////////
// Function Name : ADCTS_AutoTest
// Function Description : ADCTS Auto Test function 
// Input : 	None
// Output :	None 
// Version : v0.1
u8 ADCTS_AutoTest(void)			// Added by SOP on 2008/05/10
{
#if RESSEL12BIT
	u32 uXDat = 4095;
	u32 uYDat = 4095;	
#else
	u32 uXDat = 1023;
	u32 uYDat = 1023;	
#endif		
	ADCTS_Init();

	g_oADCTSInform.ucTouchStatus = ADCTS_TOUCH_INIT;
	ADC_InitADCCON();
	ADCTS_SetDelay(500);

	//ADCTS_SetDelayClkSrc(0);	// 0: External input clock (XXTI or XEXTCLK), 1: RTC clock
	ADCTS_SetDelayClkSrc(1);	// 0: External input clock (XXTI or XEXTCLK), 1: RTC clock	

#if RESSEL12BIT
		UART_Printf("\n\n[ ADCTS Auto Test (12-bit) ]\n\n");	
#else
		UART_Printf("\n\n[ ADCTS Auto Test (10-bit) ]\n\n");	
#endif	

	ADCTS_SetMeasureMode(eADCTS_MEASURE_FOR_STYLUS);
	Delay(10000);
	INTC_SetVectAddr(NUM_PENDNUP, ADCTS_StylusTrackingISR_Auto);
	INTC_Enable(NUM_PENDNUP);

	UART_Printf("\nPress any key to exit!!!\n");
	UART_Printf("\nStylus Down left bottom area(corner) please...... \n");
	
	//AUTO_preparing(-1,"After closing this message, Please Stylus Down bottom left corner area......");

	while(ADC_Auto_ISR == 0);

	if (g_oADCTSInform.ucTouchStatus == ADCTS_TOUCH_DOWN)
	{
		if ( ADCTS_GetXPStylusIntState() == STYLUS_DOWN)
			ADCTS_SetMeasureMode(eADCTS_MEASURE_AUTO_SEQ); 

			ADC_EnableStart(ENABLE);

			while(! (ADC_IsAfterStartup() )) ;	//	Wait for begin sampling
			while (! (ADC_IsEOCFlag()));		//	Wait for the EOC

			uXDat	= ADCTS_GetXPData();
			uYDat	= ADCTS_GetYPData();

			UART_Printf("X: %04d  , Y: %04d\n",uXDat,uYDat);
			ADCTS_SetMeasureMode(eADCTS_MEASURE_FOR_STYLUS);				
			ADCTS_SetStylusDetectMode(STYLUS_UP);
			Delay(300);	
	}
	else if (g_oADCTSInform.ucTouchStatus == ADCTS_TOUCH_UP)
	{
		if ( ADCTS_GetXPStylusIntState() == STYLUS_UP )
			ADCTS_SetMeasureMode(eADCTS_MEASURE_FOR_STYLUS);
	}
		
	INTC_Disable(NUM_PENDNUP);
	UART_Printf("\n\nADCTS Touch Screen Tracking Test End\n");

#if RESSEL12BIT
	if ((uXDat > 2000) || (uYDat >2000))
#else		
	if ((uXDat > 500) || (uYDat >500))		
#endif		
	{
		UART_Printf("\n*** Failed ADCTS Auto Test !!!\n");	
		return false;
	}		
	else
	{
		UART_Printf("\nPassed ADCTS Auto Test.\n");	
		return true;
	}	
}
