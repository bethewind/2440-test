/*****************************************
  NAME: idle.c
  DESC: IDLE mode test
  HISTORY:
  03.23.2002:purnnamu: first release for 2410
  06.02.2003:DonGo: Modified for 2440
 *****************************************/

#include "def.h"
#include "option.h"
#include "2440addr.h"
#include "2440lib.h"
#include "2440slib.h" 

#include "idle.h"


#define TESTYEAR 	(0x00)
#define TESTMONTH 	(0x12)
#define TESTDATE        (0x31)
#define TESTDAY         (0x06)  // SUN:1 MON:2 TUE:3 WED:4 THU:5 FRI:6 SAT:7   
#define TESTHOUR	(0x23)
#define TESTMIN		(0x59)
#define TESTSEC		(0x59)

#define TESTYEAR2	(0x01)
#define TESTMONTH2 	(0x01)
#define TESTDATE2	(0x01)
#define TESTDAY2	(0x07)  // SUN:1 MON:2 TUE:3 WED:4 THU:5 FRI:6 SAT:7   
#define TESTHOUR2	(0x00)
#define TESTMIN2	(0x00)
#define TESTSEC2	(0x00)

extern void SetAlarmWakeUp(void);



static void __irq Eint0Int(void)
 {
    ClearPending(BIT_EINT0);
    Uart_Printf("EINT0 interrupt is occurred for IDLE wake-up.\n");
}

static void __irq AlarmInt(void)
{
    ClearPending(BIT_RTC);
    Uart_Printf("ALARM interrupt is occurred for IDLE wake-up.\n");
}


static volatile int t0cnt,t1cnt;

static void __irq Timer0Int(void)
//Timer0 interrupt will be used to test the IDLE mode hard	
{
    ClearPending(BIT_TIMER0);
    t0cnt++;
}

static void __irq Timer1Int(void)
//Timer1 interrupt will be used to test the IDLE mode hard	
{
    ClearPending(BIT_TIMER1);
    t1cnt++;
}


/**********************
 *   IDLE mode test   *
 **********************/

void Test_IdleMode(void)
{
    int i;
//    int extintMode;

    MMU_SetAsyncBusMode(); 
     	//ARM920T should be in the async bus mode 
    	//because FCLK is used for ARM920T in the async bus mode.

    Uart_Printf("[IDLE Mode Test]\n");
    Uart_Printf("After 10 seconds, S3C2440 will wake up by RTC alarm interrupt.\n");
    Uart_Printf("S3C2440 will also wake up by EINT0.\n");

    //PWR_Lcd_Tft_16Bit_240320_On();
    //PWR_StartIIS();
    //PWR_StartTimer();
    
    Uart_TxEmpty(1);    //Wait until UART0 Tx buffer empty.
    
    rGPFCON=rGPFCON & ~(3<<0)|(2<<0); //PF0=EINT0
    rEXTINT0=rEXTINT0&(7<<0)|(0x2<<0); //EINT0=falling edge triggered
    
    pISR_EINT0=(U32)Eint0Int;
    pISR_RTC=(U32)AlarmInt;

    rSRCPND = BIT_EINT0|BIT_RTC; //to clear the previous pending states
    rINTPND = BIT_EINT0|BIT_RTC;
    
    rINTMSK=~(BIT_EINT0|BIT_RTC);

    SetAlarmWakeUp();
    rRTCCON = 0x0;	// R/W disable, but interrupt will be generated.

    rCLKCON|=(1<<2);  //enter IDLE mode.

    for(i=0;i<10;i++);   
        //wait until S3C2400X enters IDLE mode.	
        //wait EINT0 interrupt or RTC alarm interrupt

    rCLKCON&=~(1<<2);      
    //turn-off IDLE bit. IDLE bit should be turned off after wake-up.
   
    Uart_Printf("Return to Normal Mode.\n");

    rINTMSK=BIT_ALLMSK;
}



void Test_IdleModeHard(void)
{
    int i,j;

    MMU_SetAsyncBusMode(); //ARM920T should be in the async. bus mode.
    
    Uart_Printf("[IDLE Mode Hard Test with Timer0]\n");
    Uart_Printf("S3C2440 will also wake up by EINT0 falling edge.\n");

    Uart_TxEmpty(1);    //Wait until UART0 Tx buffer empty.

    rGPFCON=rGPFCON & ~(3<<0)|(2<<0); //PF0=EINT0
    rEXTINT0=rEXTINT0&(7<<0)|(0x2<<0); //EINT0=falling edge triggered
    
    rSRCPND = BIT_EINT0;
    rINTPND = BIT_EINT0;

    t0cnt=0;t1cnt=0;

    pISR_TIMER0=(U32)Timer0Int; 
    pISR_TIMER1=(U32)Timer1Int;
    pISR_EINT0=(U32)Eint0Int;

    rTCFG0=0x0;		//PRESC01=1,PRESC234=1
    rTCFG1=0x0; 	//TIMER0,1,2,3,4,5= 1/2    
    rTCNTB0=65535;	
    rTCNTB1=2570;
    rTCON=0xa0a;	//T0,T1=manualUpdate,interval mode
    rTCON=0x909;	//Start T0,T1.
    //rTCON=0x00a;
    //rTCON=0x009;
  
    rINTMSK=~(BIT_EINT0|BIT_TIMER0|BIT_TIMER1);
        
    for(i=0;i<10000;i++)
    {
    	rCLKCON|=(1<<2);  //enter IDLE mode.

	//wait until S3C2400X enters IDLE mode.	
	//wait EINT0 interrupt or RTC alarm interrupt
	for(j=0;j<10;j++);   

	rCLKCON&=~(1<<2);      
    	    //turn-off IDLE bit. IDLE bit should be turned off after wake-up.

    	if(i%100==0)Uart_Printf("#");
    }

    rTCON=0x0; //turn the timer off

    rINTMSK=BIT_ALLMSK;
    
    Uart_Printf("\nt0cnt=%d,t1cnt=%d(t0cnt+t1cnt>=10000)\n",t0cnt,t1cnt);
    Uart_Printf("Return to Normal Mode.\n");
}


/*********************
 * MMU IDLE mode test *
 *********************/

void Test_MMUIdleMode(void)
{
    int i;
//    int extintMode;

    MMU_SetAsyncBusMode(); //ARM920T should be in the async. Bus mode.

    Uart_Printf("[MMU IDLE Mode Test]\n");
    Uart_Printf("This routine tests MMU registser7:Wait for interrupt function.\n");
    Uart_Printf("After 10 seconds, S3C2440 will wake up by RTC alarm interrupt.\n");
    Uart_Printf("S3C2440 will also wake up by EINT0.\n");
    Uart_TxEmpty(1);    //Wait until UART0 Tx buffer empty.

    rGPFCON=rGPFCON & ~(3<<0)|(2<<0); //PF0=EINT0
    rEXTINT0=rEXTINT0&(7<<0)|(0x2<<0); //EINT0=falling edge triggered
    
    pISR_EINT0=(U32)Eint0Int;
    pISR_RTC=(U32)AlarmInt;

    rSRCPND = BIT_EINT0|BIT_RTC; //to clear the previous pending states
    rINTPND = BIT_EINT0|BIT_RTC;
    
    rINTMSK=~(BIT_EINT0|BIT_RTC);
    //rINTMSK=~(BIT_RTC);
    //rINTMSK=BIT_ALLMSK;

    SetAlarmWakeUp();
    rRTCCON = 0x0;	// R/W disable, but interrupt will be generated.

    MMU_WaitForInterrupt();
     
    //wait until S3C2400X enters IDLE mode.	
    //wait EINT0 interrupt or RTC alarm interrupt
    for(i=0;i<10;i++);   

    Uart_Printf("Return to Normal Mode.\n");

    rINTMSK=BIT_ALLMSK;
}



