//====================================================================
// File Name : RTC.c
// Function  : S3C2440 RTC Head file
// Program   : 
// Date      : May 06, 2002
// Version   : 0.1
// History
//   2410 v0.0 : Programming start (March 11, 2002) -> SOP
//   2440 v0.1 : edited by junon
//====================================================================

#include "2440addr.h"
#include "2440lib.h"
#include "RTC.h"

char *day[8] = {" ","Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday"};
volatile int isRtcInt, isInit = 2;              //April 01, 2002 SOP
volatile unsigned int sec_tick;

void * func_rtc_test[][2]=
{	
//									    "0123456789012345" max 15자 로한정하여 comment하세요.
//RTC
		(void *)Test_Rtc_Alarm, 			"RTC Alarm      ",
		(void *)Display_Rtc,						"RTC Display    ", 	 
		(void *)Test_Rtc_Tick,				"RTC Tick       ",
	0,0
};

void Rtc_Test(void)
{
	int i;
	
	Uart_Printf("\n======  RTC Test program start ======\n");
		
	while(1)
	{
		i=0;
		Uart_Printf("\n\n");
		Uart_Printf("1: RTC Alarm      \n");
		Uart_Printf("2: RTC Display \n");
		Uart_Printf("3: RTC Tick  \n");
	

	/*	while(1)
		{   //display menu
			Uart_Printf("%2d:%s",i,func_rtc_test[i][1]);
			i++;
			if((int)(func_rtc_test[i][0])==0)
			{
				Uart_Printf("\n");
				break;
			}
			if((i%4)==0)
			Uart_Printf("\n");
		}		 	*/

		Uart_Printf("\nPress Enter key to exit : ");
		i = Uart_GetIntNum();
		if(i==-1) break;		// return.
		switch (i)
		{
		    case 1:	 Test_Rtc_Alarm();
			         break;
		    case 2:	 Display_Rtc();
			         break;
		    case 3:	 Test_Rtc_Tick();
			         break;
		    default:
			         break;					 					 			
		}	
/*		if(i>=0 && (i<((sizeof(func_rtc_test)-1)/8)) )	// select and execute...
			( (void (*)(void)) (func_rtc_test[i][0]) )();	*/
	}
	
	Uart_Printf("\n====== RTC Test program end ======\n");
}



//=======================================
//Period = (n + 1) / 128 second
//   n : Tick time count value (1~127)
//=======================================
//********************[ Display_Rtc ]*********************************
void Display_Rtc(void)
{
//    int year,tmp;
    int year,tmp,key;                   
    int month,date,weekday,hour,min,sec;

    Uart_Printf("[ Display RTC Test ]\n");
    Uart_Printf("0. RTC Initialize     1. RTC Time Setting    2. Only RTC Display\n\n");
    Uart_Printf("Selet : ");
    key = Uart_GetIntNum();
    Uart_Printf("\n\n");
    
    isInit = key;

    if(isInit == 0)
    {
        Rtc_Init();
        isInit = 2;
    }
    
    else if(isInit == 1)
    {
        Rtc_TimeSet();
        isInit = 2;        
    }

    rRTCCON = 0x01;    //No reset, Merge BCD counters, 1/32768, RTC Control enable
//  Uart_Printf("This test should be excuted once RTC test(Alarm) for RTC initialization\n");
    Uart_Printf("Press any key to exit.\n\n");
    while(!Uart_GetKey())
    {    
        while(1)
        {
				if(rBCDYEAR == 0x99) 
						year = 0x1999;
				else 
						year		= 0x2000 + rBCDYEAR;
				month 	= rBCDMON;
				weekday = rBCDDAY;
				date		= rBCDDATE;
				hour		= rBCDHOUR;
				min 		= rBCDMIN;
				sec 		= rBCDSEC;
				
				if(sec!=tmp)
				{
						tmp = sec;
						break;
				}
		} 		 
		Uart_Printf("%2x : %2x : %2x	%10s,  %2x/%2x/%4x\n",hour,min,sec,day[weekday],month,date,year);
//        Uart_Printf("%2x : %2x : %2x  %9s,  %2x/%2x/%4x     rBCDDATE = %x, weekday= %x\n",hour,min,sec,day[weekday],month,date,year,rBCDDATE,weekday);        
    }
    rRTCCON = 0x0; //No reset, Merge BCD counters, 1/32768, RTC Control disable(for power consumption)
}


//************************[ Test_Rtc_Alarm ]******************************
void Test_Rtc_Alarm(void)  
{
    Uart_Printf("[ RTC Alarm Test for S3C2440 ]\n");

    Rtc_Init();

    rRTCCON  = 0x01;                    //No reset, Merge BCD counters, 1/32768, RTC Control enable
    rALMYEAR = TESTYEAR2 ;
    rALMMON  = TESTMONTH2;
    rALMDATE = TESTDATE2  ;
    rALMHOUR = TESTHOUR2 ;
    rALMMIN  = TESTMIN2  ;
    rALMSEC  = TESTSEC2 + 9; 
	Uart_Printf("After 9 sec, alarm interrupt will occur.. \n");
	
    isRtcInt = 0;
    pISR_RTC = (unsigned int)Rtc_Int;
    rRTCALM  = 0x7f;                    //Global,Year,Month,Day,Hour,Minute,Second alarm enable
    rRTCCON  = 0x0;                     //No reset, Merge BCD counters, 1/32768, RTC Control disable
    rINTMSK  = ~(BIT_RTC);

    while(isRtcInt==0);
    
    rINTMSK = BIT_ALLMSK;
//  rRTCCON = 0x0;  //RTC  Control disable(for power consumption), 1/32768, Normal(merge), No reset
}

//************************[ Rtc_Init ]*********************************
void Rtc_Init(void)
{
    rRTCCON  = rRTCCON  & ~(0xf)  | 0x1;            //No reset, Merge BCD counters, 1/32768, RTC Control enable
    
    rBCDYEAR = rBCDYEAR & ~(0xff) | TESTYEAR;
    rBCDMON  = rBCDMON  & ~(0x1f) | TESTMONTH;
    rBCDDATE = rBCDDATE & ~(0x3f) | TESTDATE;         
    rBCDDAY  = rBCDDAY  & ~(0x7)  | TESTDAY;       //SUN:1 MON:2 TUE:3 WED:4 THU:5 FRI:6 SAT:7
    rBCDHOUR = rBCDHOUR & ~(0x3f) | TESTHOUR;
    rBCDMIN  = rBCDMIN  & ~(0x7f) | TESTMIN;
    rBCDSEC  = rBCDSEC  & ~(0x7f) | TESTSEC;
    
    rRTCCON  = 0x0;             //No reset, Merge BCD counters, 1/32768, RTC Control disable    
}

//************************[ Rtc_TimeSet ]*********************************
void Rtc_TimeSet(void)
{
    int syear,smonth,sdate,shour,smin,ssec;
    int sday;

    Uart_Printf("[ RTC Time Setting ]\n");
    Rtc_Init();         //RTC Initialize
    Uart_Printf("RTC Time Initialized ...\n");
    
    Uart_Printf("Year (Two digit the latest)[0~99] : ");
    syear = Uart_GetIntNum();
    
    Uart_Printf("Month                      [1~12] : ");
    smonth = Uart_GetIntNum();
    
    Uart_Printf("Date                       [1~31] : ");
    sdate = Uart_GetIntNum();
          
    Uart_Printf("\n1:Sunday  2:Monday  3:Thesday  4:Wednesday  5:Thursday  6:Friday  7:Saturday\n");
    Uart_Printf("Day of the week                   : ");
    sday = Uart_GetIntNum();
    
    Uart_Printf("Hour                       [0~23] : ");
    shour = Uart_GetIntNum();
    
    Uart_Printf("Minute                     [0~59] : ");
    smin = Uart_GetIntNum();
    
    Uart_Printf("Second                     [0~59] : ");
    ssec = Uart_GetIntNum();
    
    rRTCCON  = rRTCCON  & ~(0xf)  | 0x1;            //No reset, Merge BCD counters, 1/32768, RTC Control enable
    
    rBCDYEAR = ((syear/10)<<4)+(syear%10);
    rBCDMON  = ((smonth/10)<<4)+(smonth%10);
    rBCDDAY  = sday;           //SUN:1 MON:2 TUE:3 WED:4 THU:5 FRI:6 SAT:7
    rBCDDATE = ((sdate/10)<<4)+(sdate%10);
    rBCDHOUR = ((shour/10)<<4)+(shour%10);
    rBCDMIN  = ((smin/10)<<4)+(smin%10);
    rBCDSEC  = ((ssec/10)<<4)+(ssec%10);
    
	Uart_Printf("%2d : %2d : %2d	%10s,  %2d/%2d/%4d\n",shour,smin,ssec,day[sday],smonth,sdate,syear);
	Uart_Printf("%2x : %2x : %2x	%10s,  %2x/%2x/%4x\n"
		,rBCDHOUR,rBCDMIN,rBCDSEC,day[rBCDDAY],rBCDMON,rBCDDATE,rBCDYEAR);
    rRTCCON  = 0x0;    //No reset, Merge BCD counters, 1/32768, RTC Control disable    
}

//************************[ Test_Rtc_Tick ]****************************
void Test_Rtc_Tick(void)
{
    Uart_Printf("[ RTC Tick interrupt(1 sec) test for S3C2440 ]\n");
    Uart_Printf("Press any key to exit.\n"); 
    Uart_Printf("\n");  
    Uart_Printf("\n");      
    Uart_Printf("   ");    
        
    pISR_TICK = (unsigned)Rtc_Tick;
    sec_tick  = 1;
    rINTMSK   = ~(BIT_TICK); 
    rRTCCON   = 0x0;            //No reset[3], Merge BCD counters[2], BCD clock select XTAL[1], RTC Control disable[0]
    rTICNT    = (1<<7) + 127;   //Tick time interrupt enable, Tick time count value 127
                                //Period = (n + 1) / 128 second    n:Tick time count value(1~127)
    Uart_Getch();
    
    rINTMSK   = BIT_ALLMSK;
    rRTCCON   = 0x0;            //No reset[3], Merge BCD counters[2], BCD clock select XTAL[1], RTC Control disable[0]
}

//-----------------------------------------------------------------------
void __irq EINT0_int(void)
{
    rSRCPND = BIT_EINT0;   
    rINTPND = BIT_EINT0;
    rINTPND;

    rRTCRST = (1<<3) | 3;   //Round second reset enable, over than 30 sec
}

//-----------------------------------------------------------------------
void __irq Rtc_Int(void)
{
    rSRCPND = BIT_RTC;   
    rINTPND = BIT_RTC;
    rINTPND;

    Uart_Printf("RTC Alarm Interrupt O.K.\n");
    isRtcInt = 1;  
}

//---------------------------------------------------------------------
void __irq Rtc_Tick(void)
{
    rSRCPND = BIT_TICK;   
    rINTPND = BIT_TICK;
    rINTPND;

    Uart_Printf("\b\b\b\b\b\b\b%03d sec",sec_tick++);
}
