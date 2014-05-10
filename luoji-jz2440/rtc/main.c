/*
 * main.c
 *
 *  Created on: 2014-2-21
 *      Author: cyj
 */
#include "2440lib.h"
#include "addr2440.h"
#include "init2440.h"

#define GPF4_MASK (3 << (2 * 4))
#define GPF0_MASK (3 << (2 * 0))

#define GPF4_CON (1 << (2 * 4)) // out
#define GPF0_CON (2 << (2 * 0)) // Eint

#define TESTYEAR    (0x01)
#define TESTMONTH   (0x12)
#define TESTDATE    (0x31)
#define TESTDAY     (0x02)      //SUN:1 MON:2 TUE:3 WED:4 THU:5 FRI:6 SAT:7
#define TESTHOUR    (0x23)
#define TESTMIN     (0x59)
#define TESTSEC     (0x59)

#define TESTYEAR2   (0x02)
#define TESTMONTH2  (0x01)
#define TESTDATE2   (0x01)
#define TESTDAY2    (0x03)      //SUN:1 MON:2 TUE:3 WED:4 THU:5 FRI:6 SAT:7
#define TESTHOUR2   (0x00)
#define TESTMIN2    (0x00)
#define TESTSEC2    (0x00)

void RTC_Time_Set()
{
    rRTCCON  = (rRTCCON  & ~(0xf))  | 0x1;            //No reset, Merge BCD counters, 1/32768, RTC Control enable

    rBCDYEAR = (rBCDYEAR & ~(0xff)) | TESTYEAR;
    rBCDMON  = (rBCDMON  & ~(0x1f)) | TESTMONTH;
    rBCDDATE = (rBCDDATE & ~(0x3f)) | TESTDATE;
    rBCDDAY  = (rBCDDAY  & ~(0x7))  | TESTDAY;       //SUN:1 MON:2 TUE:3 WED:4 THU:5 FRI:6 SAT:7
    rBCDHOUR = (rBCDHOUR & ~(0x3f)) | TESTHOUR;
    rBCDMIN  = (rBCDMIN  & ~(0x7f)) | TESTMIN;
    rBCDSEC  = (rBCDSEC  & ~(0x7f)) | TESTSEC;

    rRTCCON  = 0x0;             //No reset, Merge BCD counters, 1/32768, RTC Control disable
}

void Display_RTC_Time()
{
    unsigned char year,month,date,day,hour,min,sec;
    RTC_Time_Set();
    rRTCCON = 1 ;       //RTC read and write enable

    year = rBCDYEAR ;       //年
    month  = rBCDMON ;       //月
    date = rBCDDATE ;       //日
    day  = rBCDDAY ;       //星期
    hour = rBCDHOUR ;       //小时
    min  = rBCDMIN ;       //分
    sec  = rBCDSEC ;       //秒

    rRTCCON &= ~1 ;     //RTC read and write disable

    printf("RTCTIME-year:%04x,month:%02x,date:%02x,day:%02x,hour:%02x,min:%02x,sec:%02x\n",year,month,date,day,hour,min,sec);

}

void Test_Rtc_Alarm(void)
{
    RTC_Time_Set();
    rRTCCON  = 0x01;                    //No reset, Merge BCD counters, 1/32768, RTC Control enable
    rALMYEAR = TESTYEAR2 ;
    rALMMON  = TESTMONTH2;
    rALMDATE = TESTDATE2  ;
    rALMHOUR = TESTHOUR2 ;
    rALMMIN  = TESTMIN2  ;
    rALMSEC  = TESTSEC2 + 9;
    rRTCALM  = 0x7f;
    INTMSK  = ~(1<<30);
    rRTCCON &= ~1 ;     //RTC read and write disable
    printf("After 9 sec, alarm interrupt will occur.. \n");
}

void Test_Rtc_Tick(void)
{
    rTICNT = (1<<7) + 127;
    INTMSK  = ~(1<<8);
}

void EINT_Handle() {
    unsigned long offset = INTOFFSET;
    if (offset == 30) {
        Uart_SendString("RTCAlarm\n");
    } else if (offset == 8) {
        Uart_SendString("RTCTick\n");
    }

    SRCPND |= 1<<offset;
    INTPND |= 1<<offset;
}

int main() {

	Uart_SendString("Hellocyj\n");
	//rGPBCON = 0x2; // GPB0 = TOUt0

    GPFCON &= ~(GPF4_MASK); //清0
    GPFCON |= (GPF4_CON); //赋值
    GPFDAT |= (1 << 4);

    Display_RTC_Time();
    //Test_Rtc_Alarm();
    Test_Rtc_Tick();



    while (1);


    return 0;
}
