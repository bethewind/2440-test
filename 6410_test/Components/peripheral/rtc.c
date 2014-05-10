#if 1

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
*	File Name : rtc.c
*  
*	File Description : This file implements the API functons for RTC(Real Time Clock)
*
*	Author : DAEDOO
*	Dept. : AP Development Team
*	Created Date : 2008/03/17
*	Version : 0.1 
* 
*	History"
*	- Created(DAEDOO, 2008/03/17)
*  
**************************************************************************************/

#include "option.h"
#include "library.h"
#include "sfr6410.h"
#include "system.h"
#include "rtc.h"
#include "gpio.h"
#include "intc.h"
//#include "timer.h"


char *Day[8] = {" ","Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday"};


//extern u32 uCntTick; 
//extern u32 uCntAlarm;
/*
void __irq Isr_RTC_Tick(void)
{
	RTC_ClearPending(TimeTic);
	//RTC_ClearPending(0);
	
	uCntTick = true;
	
	INTC_ClearVectAddr();
}



//////////
// Function Name : Isr_RTC_InspectTick
// Function Description : This function is Interrupt Service Routine of RTC Tick Timer for checking CURTICKCNT
// Input : NONE
// Output : NONE
// Version : v0.1

void __irq Isr_RTC_InspectTick(void)
{
	INTC_Disable(NUM_RTC_TIC);
	RTC_ClearPending(TimeTic);
	
	uCntTick = 1;
	UART_Printf("Tick INT Occured!!\n");
	
	INTC_ClearVectAddr();
}

//////////
// Function Name : Isr_RTC_Alm
// Function Description : This function is Interrupt Service Routine of RTC Alarm
// Input : NONE
// Output : NONE
// Version : v0.1

void __irq Isr_RTC_Alm(void)
{
	INTC_Disable(NUM_RTC_ALARM);
	RTC_ClearPending(ALARM);
	
	uCntAlarm=1;
	UART_Printf(" RTC alarm test OK\n");

	INTC_ClearVectAddr();
}
*/

void RTC_ClearPending(u32 uPending)
{
	u32 uTemp;
	//uTemp = uTemp & ~(0x3) |(1<<uPending);	// uSelectP = 0 :Time TIC,		1: ALARM
	uTemp = uTemp & ~(0x3) |(uPending);	// uSelectP = 0 :Time TIC,		1: ALARM
	Outp32(rINTP,uTemp);

}


void RTC_TimeInit(u32 uYear, u32 uMon, u32 uDate, u32 uDay, u32 uHour, u32 uMin, u32 uSec)
{
	u32 Year,Mon,Date,Day,Hour,Min,Sec;

	Year =(((uYear/10)<<4) + (uYear%10) );
	Mon  =(((uMon/10)<<4)+ (uMon%10));
	Date =(((uDate/10)<<4) + (uDate%10) );     	
	Day	 = (uDay%10);	//SUN:1 MON:2 TUE:3 WED:4 THU:5 FRI:6 SAT:7
	Hour =(((uHour/10)<<4) + (uHour%10) );
	Min  =(((uMin/10)<<4)  + (uMin%10)  );
	Sec  =(((uSec/10)<<4)  + (uSec%10)  );     	

	RTC_Enable(true);		

	Outp32(rBCDSEC  ,Sec); 
	Outp32(rBCDMIN  ,Min); 
	Outp32(rBCDHOUR ,Hour);
	Outp32(rBCDDATE ,Date);
	Outp32(rBCDDAY  ,Day); 
	Outp32(rBCDMON  ,Mon); 
	Outp32(rBCDYEAR ,Year);

	RTC_Enable(false);	

}

void RTC_Enable(u8 bData)
{
	u32 uRead;

	uRead = Inp32(rRTCCON);
	Outp32(rRTCCON, uRead&~(1<<0)|(bData));
}

void RTC_TickTimeEnable(u8 bData)
{
	u32 uRead;

	uRead = Inp32(rRTCCON);
	Outp32(rRTCCON, uRead&~(1<<8)|(bData<<8));
}


#if 0
void RTC_TickClkSelect(u ticktimer_clk)
{
	u32 uTickClkSel;

	switch (ticktimer_clk)
	{
		case	CLK_32768Hz:
				uTickClkSel = TICKCLKSEL_32768Hz;
				break;
		case	CLK_16384Hz:				
				uTickClkSel = TICKCLKSEL_32768Hz;
				break;
		case	CLK_16384Hz:
				uTickClkSel = TICKCLKSEL_16384Hz;
				break;
		case	CLK_8192Hz:
				uTickClkSel = TICKCLKSEL_8192Hz;
				break;
		case	CLK_4096Hz:
				uTickClkSel = TICKCLKSEL_4096Hz; 
				break;
		case	CLK_2048Hz:
				uTickClkSel = TICKCLKSEL_2048Hz; 
				break;
		case	CLK_1024Hz:
				uTickClkSel = TICKCLKSEL_1024Hz; 
				break;
		case	CLK_512Hz:
				uTickClkSel = TICKCLKSEL_512Hz;  
				break;
		case	CLK_256Hz:
				uTickClkSel = TICKCLKSEL_256Hz;  
				break;
		case	CLK_128Hz:
				uTickClkSel = TICKCLKSEL_128Hz;  
				break;
		case	CLK_64Hz:
				uTickClkSel = TICKCLKSEL_64Hz;  
				break;
		case	CLK_32Hz:
				uTickClkSel = TICKCLKSEL_32Hz;  
				break;
		case	CLK_16Hz:
				uTickClkSel = TICKCLKSEL_16Hz;   
				break;
		case	CLK_8Hz:
				uTickClkSel = TICKCLKSEL_8Hz;    
				break;				
		case	CLK_4Hz:
				uTickClkSel = TICKCLKSEL_4Hz;    
				break;				
		case	CLK_2Hz:
				uTickClkSel = TICKCLKSEL_2Hz;    
				break;				
		case	CLK_1Hz:
				uTickClkSel = TICKCLKSEL_1Hz;    
				break;
	}

	Outp32(rWTCON, PREVAL(uPreVal) | div_clk);
	//Outp32(rWTDAT, uWdtDat);
	//Outp32(rWTCNT, uWdtDat);
}
#else

void RTC_TickClkSelect(TickTimerClk TICKCLKSEL)
{
	Outp32(rRTCCON, (TICKCLKSEL<<4));
 }

#endif

void RTC_ClkReset(u8 bData)
{
	u32 uRead;

	RTC_Enable(true);
	uRead = Inp32(rRTCCON);
	Outp32(rRTCCON, uRead&~(1<<3)|(bData<<3));
	RTC_Enable(false);
}

void RTC_ClkSelect(u8 bClk)
{
	u32 uRead;

	RTC_Enable(true);		
	uRead = Inp32(rRTCCON);
	Outp32(rRTCCON, uRead&~(1<<2)|(bClk<<2));
	RTC_Enable(false);		
}

void RTC_TickCnt(u32 uTickCnt)
{
	Outp32(rTICCNT, uTickCnt);
}

void RTC_AlarmTimeInit(u32 uYear, u32 uMon, u32 uDate, u32 uHour, u32 uMin, u32 uSec)
{
	u32 Year, Mon, Date, Hour, Min, Sec;
	
    	Year =(((uYear/10)<<4) + (uYear%10));
    	Mon  =(((uMon/10)<<4)+ (uMon%10));
    	Date =(((uDate/10)<<4) + (uDate%10));     	
   	Hour =(((uHour/10)<<4) + (uHour%10));
    	Min  =(((uMin/10)<<4)  + (uMin%10));
    	Sec  =(((uSec/10)<<4)  + (uSec%10));     	

	RTC_Enable(true);		
		
	Outp32(rALMSEC  , Sec); 
	Outp32(rALMMIN  , Min); 
	Outp32(rALMHOUR , Hour);
	Outp32(rALMDATE , Date);
	Outp32(rALMMON  ,Mon); 
	Outp32(rALMYEAR , Year);

	RTC_Enable(false);
	
}

void RTC_AlarmEnable(u8 bAll, u8 bYear, u8 bMon, u8 bDate, u8 bHour, u8 bMin, u8 bSec)
{
	u32 uRead;

	uRead = Inp32(rRTCALM);
	Outp32(rRTCALM , uRead & ~(0x7f)|(bAll<<6)|(bYear<<5)|(bMon<<4)|(bDate<<3)|(bHour<<2)|(bMin<<1)|(bSec<<0));
}

void RTC_Print(void)
{
	u32 uYear,uMonth,uDate,uDay,uHour,uMin,uSec;

	// Check RTC Enable 
	RTC_Enable(true);

	uYear =  Inp32(rBCDYEAR);
	if(uYear == 0x99)
		uYear = 0x1999;
	else
		uYear = 0x2000 + uYear;
	
	uMonth= Inp32(rBCDMON);
	uDate = Inp32(rBCDDATE);
	uHour = Inp32(rBCDHOUR);
	uMin  = Inp32(rBCDMIN);
	uSec  = Inp32(rBCDSEC);
	uDay= Inp32(rBCDDAY);

	RTC_Enable(false);

	UART_Printf("%2x : %2x : %2x  %10s,  %2x/%2x/%4x\n", uHour, uMin, uSec, Day[uDay], uMonth, uDate, uYear);

}

#else

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
*	File Name : rtc.c
*  
*	File Description : This file implements the API functons for RTC(Real Time Clock)
*
*	Author : Woojin.Kim
*	Dept. : AP Development Team
*	Created Date : 2007/01/06
*	Version : 0.1 
* 
*	History"
*	- Created(Woojin.Kim 2007/01/06)
*  
**************************************************************************************/
#include "option.h"
#include "library.h"
#include "sfr6410.h"
#include "system.h"
#include "rtc.h"
#include "gpio.h"
#include "intc.h"
#include "timer.h"

extern u8 *aDay[8];
extern u32 uCntTick;
extern u32 uCntAlarm;


//////////
// Function Name : RTC_SetCon
// Function Description : This function set up RTC
// Input :	uEnTic [0:Disable		1:Enable	]
//			uTicsel [0:2^15hz   ~   15:2^0hz]
//			uRstCLK [0:No Reset	1:Reset]
//			uSelCNT [0:Merge BCD counter	1:Reserved]
//			uSelCLK [0:XTAL divided clock	1:Reserved]
//			uEnRTC [0:Disable		1:Enbalde]
// Output : NONE
// Version : v0.1
void RTC_SetCON(u32 uEnTic,u32 uTicsel,u32 uRstCLK,u32 uSelCNT,u32 uSelCLK,u32 uEnRTC )			//need modify at 6400
{
	u32 uTemp;

	uTemp = (uEnTic<<8) | (uTicsel<<4) |(uRstCLK<<3)|(uSelCNT<<2)|(uSelCLK<<1)|(uEnRTC<<0);
	Outp32(rRTCCON,uTemp);

}


//////////
// Function Name : RTC_SetTime
// Function Description : This function set up RTC time
// Input :	uYear [99:1999	00:2000	2digit dec number]
//			uMon [01~12]
//			uDate [01~31]
//			uDay [SUN:01 MON:02 TUE:03 WED:04 THU:05 FRI:06 SAT:07]
//			uHour [00~23]
//			uMin [00~59]
//			uSec [00~59]
// Output : NONE
// Version : v0.1

void RTC_SetTime(u32 uYear, u32 uMon, u32 uDate, u32 uDay, u32 uHour, u32 uMin, u32 uSec)
{

	u32 Year,Mon,Date,Day,Hour,Min,Sec;
	
    	Year =( ((uYear/10)<<4) + (uYear%10) );
    	Mon  =( ((uMon/10)<<4)+ (uMon%10));
    	Date =( ((uDate/10)<<4) + (uDate%10) );     	
    	Day	 = (uDay%10);	//SUN:1 MON:2 TUE:3 WED:4 THU:5 FRI:6 SAT:7
   	Hour =( ((uHour/10)<<4) + (uHour%10) );
    	Min  =( ((uMin/10)<<4)  + (uMin%10)  );
    	Sec  =( ((uSec/10)<<4)  + (uSec%10)  );     	
		
	Outp32(rBCDSEC  , Sec); 
	Outp32(rBCDMIN  , Min); 
	Outp32(rBCDHOUR , Hour);
	Outp32(rBCDDATE , Date);
	Outp32(rBCDDAY  , Day); 
	Outp32(rBCDMON  ,Mon); 
	Outp32(rBCDYEAR , Year);
	
}


//////////
// Function Name : RTC_Init
// Function Description : This function Initialize RTC time by pre-defined values
// Input : NONE
// Output : NONE
// Version : v0.1

void RTC_Init(void)
{
	RTC_SetCON(0,0,0,0,0,1);
	RTC_SetTime(InitYEAR,InitMONTH,InitDATE,InitDAY,InitHOUR,InitMIN,InitSEC);
	RTC_SetCON(0,0,0,0,0,0);
}


//////////
// Function Name : RTC_SetTickCNT
// Function Description : This function set up RTC Tick Count value
// Input : uTickcnt[16bit tick time count value]
// Output : NONE
// Version : v0.1

void RTC_SetTickCNT(u32 uTickcnt)
{
	Outp32(rTICCNT,uTickcnt);
}


//////////
// Function Name : RTC_Print
// Function Description : This function Print out RTC time through UART
// Input : NONE
// Output : NONE
// Version : v0.1

void RTC_Print(void)
{
	u32 uYear,uMonth,uDate,uDay,uHour,uMin,uSec;


//	uTmpCON = Inp32(rRTCCON);
//	uTmpCON = uTmpCON | 0x1;
//	Outp32(rRTCCON,uTmpCON);		//RTC control Enable

	uYear =  Inp32(rBCDYEAR);
	if(uYear == 0x99)
		uYear = 0x1999;
	else
		uYear = 0x2000 + uYear;
	
	uMonth= Inp32(rBCDMON);
	uDate = Inp32(rBCDDATE);
	uHour = Inp32(rBCDHOUR);
	uMin  = Inp32(rBCDMIN);
	uSec  = Inp32(rBCDSEC);
	uDay= Inp32(rBCDDAY);

//	uTmpCON = Inp32(rRTCCON);
//	uTmpCON = uTmpCON & ~(0x1);
//	Outp32(rRTCCON,uTmpCON);		//RTC control Disable
		
	UART_Printf("%2x : %2x : %2x  %10s,  %2x/%2x/%4x\n",uHour,uMin,uSec,aDay[uDay],uMonth,uDate,uYear);

}


//////////
// Function Name : RTC_PrintCURTICK
// Function Description : This function Print out RTC CURTICK value
// Input : NONE
// Output : NONE
// Version : v0.1

void RTC_PrintCURTICK(void)
{
	u32 uTickCNT=0;


	uTickCNT = (u32)Inp32(rCURTICCNT);
	UART_Printf("CURTICK COUNT : %d\n",uTickCNT);
	
		
}


//////////
// Function Name : RTC_SetAlmTime
// Function Description : This function set up RTC Alarm time
// Input :	uYear [99:1999	00:2000	2digit dec number]
//			uMon [01~12]
//			uDate [01~31]
//			uHour [00~23]
//			uMin [00~59]
//			uSec [00~59]
// Output : NONE
// Version : v0.1

void RTC_SetAlmTime(u32 uYear, u32 uMon, u32 uDate, u32 uHour, u32 uMin, u32 uSec)
{

	u32 Year,Mon,Date,Hour,Min,Sec;
	
    	Year =( ((uYear/10)<<4) + (uYear%10) );
    	Mon  =( ((uMon/10)<<4)+ (uMon%10));
    	Date =( ((uDate/10)<<4) + (uDate%10) );     	
   	Hour =( ((uHour/10)<<4) + (uHour%10) );
    	Min  =( ((uMin/10)<<4)  + (uMin%10)  );
    	Sec  =( ((uSec/10)<<4)  + (uSec%10)  );     	
		
	Outp32(rALMSEC  , Sec); 
	Outp32(rALMMIN  , Min); 
	Outp32(rALMHOUR , Hour);
	Outp32(rALMDATE , Date);
	Outp32(rALMMON  ,Mon); 
	Outp32(rALMYEAR , Year);
	
}


//////////
// Function Name : RTC_SetAlmEn
// Function Description : This function enable RTC Alarm
// Input :	enAlm [0:Disable	1:Enable	(Alarm global enable)]
// Input :	enYear [0:Disable	1:Enable]
//			enMon [0:Disable	1:Enable]
//			enDate [0:Disable	1:Enable]
//			enHour [0:Disable	1:Enable]
//			enMin [0:Disable	1:Enable]
//			enSec [0:Disable	1:Enable]
// Output : NONE
// Version : v0.1

void RTC_SetAlmEn(u32 enAlm, u32 enYear, u32 enMon, u32 enDate, u32 enHour,u32 enMin,u32 enSec)
{
	u32 uTemp;

	uTemp = Inp32(rRTCALM);
	uTemp = uTemp & ~(0x7f) |(enAlm<<6)|(enYear<<5)|(enMon<<4)|(enDate<<3)|(enHour<<2)|(enMin<<1)|(enSec<<0);
	Outp32(rRTCALM,uTemp);
	
}


//////////
// Function Name : RTC_PrintAlm
// Function Description : This function Print out RTC Alarm time through UART
// Input : NONE
// Output : NONE
// Version : v0.1

void RTC_PrintAlm(void)
{

	u32 uYear,uMonth,uDate,uHour,uMin,uSec;


	uYear =  Inp32(rALMYEAR);
	if(uYear == 0x99)
		uYear = 0x1999;
	else
		uYear = 0x2000 + uYear;
	
	uMonth= Inp32(rALMMON);
	uDate = Inp32(rALMDATE);
	uHour = Inp32(rALMHOUR);
	uMin  = Inp32(rALMMIN);
	uSec  = Inp32(rALMSEC);

	UART_Printf("%2x : %2x : %2x  ,  %2x/%2x/%4x\n",uHour,uMin,uSec,uMonth,uDate,uYear);
  	}


//////////
// Function Name : RTC_SetLVD
// Function Description : This function clear RTC Low Voltage Detection value 
// Input : enLVD[0:normal operation	1:Low voltage Detection clear]
// Output : NONE
// Version : v0.1

void RTC_SetLVD(u32 enLVD)
{
	Outp32(rRTCLVD,enLVD);

}


//////////
// Function Name : RTC_PrintLVD
// Function Description : This function Print out RTC Low Voltage Detection value [0:normal operation	1:Low voltage Detection]
// Input : NONE
// Output : NONE
// Version : v0.1

u32 RTC_PrintLVD(void)
{
	u32 uReturn=0;
	uReturn =  Inp32(rRTCLVD);
	return uReturn;
	
}


//////////
// Function Name : RTC_ClearIntP
// Function Description : This function clear RTC Interrupt Pending
// Input : uSelectP [0:Time TIC		1:Alarm]
// Output : NONE
// Version : v0.1

void RTC_ClearIntP(u32 uSelectP)
{
	u32 uTemp;
	uTemp = uTemp & ~(0x3) |(1<<uSelectP);	// uSelectP = 0 :Time TIC,		1: ALARM
	Outp32(rINTP,uTemp);

}


//////////
// Function Name : Isr_RTC_Tick
// Function Description : This function is Interrupt Service Routine of RTC Tick Timer
// Input : NONE
// Output : NONE
// Version : v0.1

void __irq Isr_RTC_Tick(void)
{

	RTC_ClearIntP(0);
	uCntTick = 1;
	INTC_ClearVectAddr();
}



//////////
// Function Name : Isr_RTC_InspectTick
// Function Description : This function is Interrupt Service Routine of RTC Tick Timer for checking CURTICKCNT
// Input : NONE
// Output : NONE
// Version : v0.1

void __irq Isr_RTC_InspectTick(void)
{

	RTC_ClearIntP(0);
	uCntTick = 1;
	UART_Printf("Tick INT Occured!!\n");
	INTC_ClearVectAddr();
}



//////////
// Function Name : Isr_RTC_Alm
// Function Description : This function is Interrupt Service Routine of RTC Alarm
// Input : NONE
// Output : NONE
// Version : v0.1

void __irq Isr_RTC_Alm(void)
{

	RTC_ClearIntP(1);
	uCntAlarm=1;
	UART_Printf(" RTC alarm test OK\n");

	INTC_ClearVectAddr();
}


#endif
