#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "def.h"
#include "option.h"
#include "2440addr.h"
#include "2440lib.h"
#include "userlib.h"

#include "lcdlib.h"
#include "glib.h"
#include "lcd.h"

#include "dvstest.h"

#include "user_test2.h"

extern void Clk0_Enable(int clock_sel);
extern void Calc_Clock(int print_msg);
extern void Clk1_Enable(int clock_sel);
extern U32 Mdiv, Pdiv, Sdiv, Fclk, Hclk, Pclk, Hdivn, Pdivn, Hclk_Ratio, Pclk_Ratio, Ref_Cnt;

////////////////////////////// user_test2() start///////////////////////////////
void User_Test2(void)
{
	Uart_Printf("\nUser_Test2.\n");	

	
}

////////////////////////////////// dvs test ////////////////////////////////////////
extern void CLKDIV124(void);
extern void CLKDIV144(void);
#define BIT_DVS_ON	(1<<12)
#define DVS_VOLTL	80
#define DVS_VOLTH	120



void DvsTimer_Setting(void);
void __irq DvsTimer0_intr(void);


void Firm_DVS_Onoff(void)
{
	volatile int i, n, cnt=0;
	
	Uart_Printf("DVS test.\n");

	rBANKSIZE = (rBANKSIZE & ~(3<<4)) | (0<<4) | (1<<7);	//SCKE_EN, SCLK_EN = disable.

	Uart_Printf("Change core speed to 400MHz.\n");
	#if FIN==12000000
		ChangeClockDivider(13, 12);
		#if CPU2440A==1
		ChangeMPllValue(127,2,1);	// 406MHz
		#else
		ChangeMPllValue(127,2,0);	// 406MHz
		#endif
	#else	// 16.9344Mhz
	ChangeClockDivider(13, 12);
		#if CPU2440A==1
		ChangeMPllValue(110,3,1);	// 400MHz
		#else
		ChangeMPllValue(110,3,0);	// 400MHz
		#endif
	#endif
	
	Calc_Clock(1);
	
	UPDATE_REFRESH(Hclk);
	Uart_Init(Pclk, 115200);

	Uart_Printf("Check Clkout0:FCLK, Clkout1:HCLK.\n");
	// Clkout0: FCLK.	
	Clk0_Enable(2);	// 0:MPLLin, 1:UPLL, 2:FCLK, 3:HCLK, 4:PCLK, 5:DCLK0
	// Clkout1: HCLK.
	Clk1_Enable(3);	// 0:MPLLout, 1:UPLL, 2:RTC, 3:HCLK, 4:PCLK, 5:DCLK1	
	
	DvsTimer_Setting();
	
	Uart_Printf("start.\n");

	while(1) {
#if 1
		// 1. Entering Idle mode ...
		// (1) Strengthen HCLK.
		rCAMDIVN = (rCAMDIVN & ~(3<<8)) | (1<<8);	// 1:3:6 -> 1:6:12
		rCLKDIVN &= ~(1<<0);						// 1:6:12 -> 1:6:6

		// (2) Set DVS flag
		rCAMDIVN |= BIT_DVS_ON;					// (1<<12)
		// -> the FCLK will be the same as HCLK(67Mhz).

		// (3) Drop the core voltage
		Max1718_Set(DVS_VOLTL);


		// (4) t_DVS, delay while dropping the core voltage...
		//for(i=0; i<2000; i++);		// SMDK2440: 28us/0.1V.

		// (5) set Idle bit.
		// timer setting...
		rTCNTB0 = 30;
		rTCON = 1<<3| 1<<1;	// load timer.
		rTCON = 1<<3| 1<<0;	// start timer.

		rCLKCON |= 1<<2;	// Enter Idle mode.
		for(i=0;i<10;i++);

		//...
		//... in Idle mode...
		//...

		// 2. Wake up from Idle mode...
		rCLKCON&=~(1<<2);		// must clear this bit after wake-up from idle mode.
		rTCON = 0;

		//(1) Return Core voltage
		Max1718_Set(DVS_VOLTH);
		Led_Display(0x6);

		// (2) t_DVS, delay while dropping the core voltage...
		for(i=0; i<2000; i++);		// SMDK2440: 28us/0.1V.
		Led_Display(0x7);

		// (3) Return Hclk
		rCLKDIVN |= (1<<0);						// 1:6:6 -> 1:6:12
		rCAMDIVN = (rCAMDIVN & ~(3<<8));			// 1:6:12 -> 1:3:6

		if(!(cnt++%10)) Uart_Printf("cnt:%d.\n", cnt);
		//Delay(10);
#else
		// Only for CLKDIV test.
		// * Strengthen HCLK.
		//rCAMDIVN = (rCAMDIVN & ~(3<<8)) | (1<<8);	// 1:3:6 -> 1:6:12
		//rCLKDIVN &= ~(1<<0);						// 1:6:12 -> 1:6:6
		rCLKDIVN=(rCLKDIVN&~(7<<0))|(7<<0);	// for bug of 2440x, 1:3:6
		rCLKDIVN=(rCLKDIVN&~(7<<0))|(5<<0);	// for 2440x, 1:4:8
		//CLKDIV144();
		Led_Display(1);
		// * Drop the core voltage
		Max1718_Set(DVS_VOLTL);
		// * set Idle bit.
		rTCNTB0 = 30;		// Timer setting for wake-up from idle mode.
		rTCON = 1<<3| 1<<1;	// load timer.
		rTCON = 1<<3| 1<<0;	// start timer.

		rCLKCON |= 1<<2;	// Enter Idle mode.
		for(i=0;i<10;i++);

		//...
		//... in Idle mode...
		//...
		
		// 2. Wake up from Idle mode...
		rCLKCON&=~(1<<2);		// must clear this bit after wake-up from idle mode.
		rTCON = 0;


		//* Return Core voltage
		Max1718_Set(DVS_VOLTH);
		// * t_DVS, delay while dropping the core voltage...
		for(i=0; i<2000; i++);		// SMDK2440: 28us/0.1V.

		// * Return Hclk
		//rCLKDIVN |= (1<<0);						// 1:6:6 -> 1:6:12
		//rCAMDIVN = (rCAMDIVN & ~(3<<8));			// 1:6:12 -> 1:3:6
		rCLKDIVN=(rCLKDIVN&~(7<<0))|(3<<0);	// for 2440x, 1:2:4
		//CLKDIV124();
		Led_Display(0);

		Delay(200);
#endif
		if(Uart_GetKey()==ESC_KEY) break;
	}
}



void DvsTimer_Setting(void)
{
//	float cnt_val=0.0;

    Uart_Printf("[ Timer 0 Interrupt setting]\n");

	// Uart_Printf("[Timer interval: %4.1f, %4.1f, %4.1f, %4.1f[ms].\n", \
	// timer_val_arr[0]*0.081, timer_val_arr[1]*0.081, timer_val_arr[2]*0.081, timer_val_arr[3]*0.081);
	// 296, 617, 802, 1284 -> 24ms, 50ms, 65ms, 104ms for eac

	rGPBCON = rGPBCON& ~(0x03) | 0x2;
	rCLKCON |= 1<<8;	// PWM timer PCLK clock enable.

	pISR_TIMER0 = (int)DvsTimer0_intr;
	// Timer interupt start.
	rINTMSK &= ~BIT_TIMER0;
	
	rTCFG0 = 0xff;		// Prescaler0 = 0xff=256.
	// Timer input clock freq = PCLK/(Prescaler0+1)/divider value.
	// where, PCLK = 50.7MHz

	rTCFG1 = 0<<20 | 3<<0;	// MUX0 = 1/16

	// Timer input clock frequency = PCLK/(prescaler value+1)/(divider value)
	// 1clock = 81us.
	rTCNTB0 = 300;// 296, 617, 802, 1284 -> 24ms, 50ms, 65ms, 104ms for each.
	rTCMPB0 = 0;

	// Load timer cnt value
	rTCON = 1<<3| 1<<1;

	// Timer start.
	//rTCON = 1<<3| 1<<0;

}

void __irq DvsTimer0_intr(void)
{
	ClearPending(BIT_TIMER0)
}



void DVS_Onoff_Man(void)
{
//	int i=0; 
	char c;
	

	Uart_Printf("Change core speed to 400MHz.\n");
	#if FIN==12000000
		ChangeClockDivider(13, 12);
		#if CPU2440A==1
		ChangeMPllValue(127,2,1);	// 406MHz
		#else
		ChangeMPllValue(127,2,0);	// 406MHz
		#endif
	#else	// 16.9344Mhz
	ChangeClockDivider(13, 12);
		#if CPU2440A==1
		ChangeMPllValue(110,3,1);	// 400MHz
		#else
		ChangeMPllValue(110,3,0);	// 400MHz
		#endif
	#endif
	
	Calc_Clock(1);
	
	UPDATE_REFRESH(Hclk);
	Uart_Init(Pclk, 115200);

	Uart_Printf("Check Clkout0:FCLK, Clkout1:HCLK.\n");
	// Clkout0: FCLK.	
	Clk0_Enable(2);
	// Clkout1: HCLK.
	Clk1_Enable(3);

	Uart_Printf("Press key to on/off dvs_on bit.\n");
	do {
		c=Uart_Getch();
		rCAMDIVN |= BIT_DVS_ON;
		Uart_Printf("DVS=ON(0x%d).\n", rCAMDIVN);
		
		c=Uart_Getch();
		rCAMDIVN &= ~BIT_DVS_ON;
		Uart_Printf("DVS=OFF(0x%d).\n", rCAMDIVN);
	} while(c!='q');

}

