//===================================================================
// File Name : Dvstest.c
// Function  : S3C2440A 
// Date      : Feb 02, 2004
// Version   : 0.0
// History
//  0.00: Feb.xx.2004:DonGo: first draft version for DVS.
//===================================================================

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



#define BIT_DVS_ON	(1<<12)
#define DVS_VOLTL	100
#define DVS_VOLTH	120
// regulator voltage out settling time = 28us/0.1V.
// 0.2V:56us, 0.3V:84us, 0.4V:112us, 0.5V:140us
// 0.25V:70us, 0.35V:98us, 0.45V:126us, 0.55V:154us.

extern U32 FCLK1;
extern void CLKDIV124(void);
extern void CLKDIV144(void);
extern void Clk0_Enable(int clock_sel);
extern void Clk0_Enable(int clock_sel);
extern void Clk1_Enable(int clock_sel);
extern void Lcd_Port_Init(void);
extern void Calc_Clock(int print_msg);


U32 Vcount=0, Timer_cnt0=0;
short int Idle_flag=0;	// assume,  0:Normal, 1:Idle.
short int Div_flag=124;
short int intr_flag=0;

// one-timer clock= 81 usec. 
U32 timer_val_arr[4] = {296, 617, 802, 1284};
//U32 timer_val_arr[4] = {1284, 1284, 1284, 1284};
// 296, 617, 802, 1284 -> 24ms, 50ms, 65ms, 104ms for each.

short int timer_select=0;

#define	XADDR	_NONCACHE_STARTADDRESS
#define	IDLE_EN		0	// enable idle at timer interrupt.

extern U32 Mdiv, Pdiv, Sdiv, Fclk, Hclk, Pclk, Hdivn, Pdivn, Hclk_Ratio, Pclk_Ratio, Ref_Cnt;

void Dvs_Test(void)
{
	volatile int i, n;

	Uart_Printf("Dvs test.\n");

	rGPGCON = (rGPGCON & ~(3<<22)) | (1<<22);	// set GPG11 output for idle state.
	
	rBANKSIZE = (rBANKSIZE & ~(3<<4)) | (0<<4) | (1<<7);	//SCKE_EN, SCLK_EN = disable.

	Uart_Printf("Change core speed to 266MHz.\n");
	#if FIN==12000000
	ChangeClockDivider(13, 12);	// 1:3:6
		#if CPU2440A==1
		ChangeMPllValue(127,2,1);	// 406MHz
		#else						// 2440X
		ChangeMPllValue(127,2,0);	// 406MHz
		#endif
	#else	// 16.9344Mhz
		ChangeClockDivider(13, 12);
		#if CPU2440A==1
		ChangeMPllValue(110,3,1);	// 400MHz
		#else						// 2440X
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
	
	
#if ADS10==TRUE	
	srand(0);
#endif
	Led_Display(0);	// clear all leds.
	
	Timer_Setting();
	Set_Lcd_Tft_16Bit_240320_Dvs();

	Uart_Printf("Tcnt, Vcnt, Idle_flag.\n");
	Uart_Printf("%8d,%8d,%1d\n", Timer_cnt0, Vcount, Idle_flag);
	
	while(1) {
		//Uart_Printf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
		Uart_Printf("%8d,%8d,%1d\n", Timer_cnt0, Vcount, Idle_flag);
		for(i=0; i<1024*32; i+=4) {
			//*(U32 *)(XADDR+i) = i;
			//*(U32 *)(XADDR+i);
		}
		if(Uart_GetKey()==ESC_KEY) break;
	}

	rINTMSK = BIT_ALLMSK;
}


////////////////////////////// HCLK change test start  ///////////////////////////////

//========================================================
// Timer Interrupt Request 
//========================================================
void __irq Timer0_intr(void)
{
	int i;
	
	//if(Timer_cnt0%2) Led_Onoff(LED1,ON);
	//else Led_Onoff(LED1,OFF);

	ClearPending(BIT_TIMER0)

	if(rLCDSRCPND & 2) {
		rLCDSRCPND=2; // Clear LCD SUB Interrupt source pending
		rLCDINTPND=2; // Clear LCD SUB Interrupt pending
		ClearPending(BIT_LCD);
	}

#if ADS10==TRUE
	i= (rand()%700) + 61;	// 0.081ms/cnt
#else
	i= 370;	// 0.081ms/cnt -> 370cnt==300ms.
#endif
	
	rTCNTB0 = i;
	//rTCNTB0 = timer_val_arr[Timer_cnt0%4];

	// Load timer cnt value
	rTCON = 1<<3| 1<<1;
	// Timer start.
	rTCON = 1<<3| 1<<0;

	if(Timer_cnt0%2) {
		Idle_flag = 1;	// Idle mode
		rINTMSK &= ~(BIT_LCD);	// unmask.
		#if IDLE_EN==1
			Led_Onoff(LED4, OFF);
			rCLKCON |= 1<<2;	// Enter Idle mode.
		    for(i=0;i<10;i++);
			//wait until S3C2400X enters IDLE mode.
	        //wait EINT0 interrupt or RTC alarm interrupt
	        rCLKCON&=~(1<<2);
		Led_Onoff(LED4, ON);
    		//turn-off IDLE bit. IDLE bit should be turned off after wake-up.
    	#endif
	} else {
		Idle_flag = 0;	// Normal mode
		rINTMSK &= ~(BIT_LCD);	// unmask.
	}

	Timer_cnt0++;
}


void __irq Lcd_Int_Frame_ForDvs(void)
{
	int i;
	
	rINTMSK |= (BIT_LCD);
	
	if((rLCDCON5 & (0x3<<15))) 
	{	// if Not VSYNC period

		Uart_Printf("-");
	} 
	else 
	{		// if in VSYNC period
		if( Idle_flag==1) 
		{	// Idle -> Clkdiv=1:4:4
			rLCDCON1 &= ~(0x1);	// ENVID off.

			#if CPU2440A==1
			rCAMDIVN = (rCAMDIVN & ~(3<<8)) | (1<<8);	// 1:3:6 -> 1:6:12
			rCLKDIVN &= ~(1<<0);						// 1:6:12 -> 1:6:6
			#else 
			CLKDIV144();
			#endif
			#if CPU2440A==1
			rCAMDIVN |= BIT_DVS_ON;		// (1<<12), DVS off.
			rGPGDAT &= ~(1<<11);
			#endif
			Max1718_Set(DVS_VOLTL);
			UPDATE_REFRESH(Hclk/2);
			rLCDCON1 = (rLCDCON1 & ~(0x3ff<<8)) | (4<<8) | 1;	// ENVID on. 5MHz VCLK, 60fps

			Vcount++;
   
			rLCDSRCPND=2; // Clear LCD SUB Interrupt source pending
			rLCDINTPND=2; // Clear LCD SUB Interrupt pending
			ClearPending(BIT_LCD);

			if(rSRCPND & BIT_TIMER0) {
				rSRCPND = BIT_TIMER0;	   //Clear pending bit
				rINTPND = BIT_TIMER0;
			}

			#if IDLE_EN==1
			rCLKCON |= 1<<2;	// Enter Idle mode.
			for(i=0;i<10;i++);
			//wait until S3C2400 enters IDLE mode.	
			//wait EINT0 interrupt or RTC alarm interrupt
			rCLKCON&=~(1<<2);
			//turn-off IDLE bit. IDLE bit should be turned off after wake-up.
	    		#endif
		} 
		else 	// If Normal mode...
		{	
			rLCDCON1 &= ~(0x1);	// ENVID off.

			Max1718_Set(DVS_VOLTH);
			for(i=0; i<2000;i++) rGSTATUS0;

			UPDATE_REFRESH(Hclk);

			#if CPU2440A==1
			rCAMDIVN &= ~BIT_DVS_ON;	// (0<<12), DVS off, FCLK=MPLLout
			rGPGDAT |= (1<<11);
			#endif

			#if CPU2440A==1
			rCLKDIVN |= (1<<0);						// 1:6:6 -> 1:6:12
			rCAMDIVN = (rCAMDIVN & ~(3<<8));			// 1:6:12 -> 1:3:6
			#else
			CLKDIV124();	// Normal -> Clkdiv=1:2:4
			#endif
			rLCDCON1 = (rLCDCON1 & ~(0x3ff<<8)) | (9<<8) | 1;	// ENVID on.5MHz VCLK, 60fps

			Vcount++;
   
			rLCDSRCPND=2; // Clear LCD SUB Interrupt source pending
			rLCDINTPND=2; // Clear LCD SUB Interrupt pending
			ClearPending(BIT_LCD);

			if(rSRCPND & BIT_TIMER0) {
				rSRCPND = BIT_TIMER0;	   //Clear pending bit
				rINTPND = BIT_TIMER0;
			}
		}
	} // endof if((rLCDCON5 & (0x3<<15))) {
}
////////////////////////////// HCLK change test end  ///////////////////////////////

void Set_Lcd_Tft_16Bit_240320_Dvs(void)
{
   // int i,j;//,k;
	
	Uart_Printf("[Set LCD]\n");
	
    Lcd_Port_Init();
    Lcd_Init(MODE_TFT_16BIT_240320);
    Glib_Init(MODE_TFT_16BIT_240320);
	Lcd_Lcc3600Enable(); // Enable LCC3600
    Lcd_PowerEnable(0, 1);
    Lcd_EnvidOnOff(1);
    
    Glib_ClearScr(0, MODE_TFT_16BIT_240320);
    Glib_FilledRectangle(0,0,119,159,0xf800);
    Glib_FilledRectangle(120,0,239,159,0x07e0);
    Glib_FilledRectangle(0,160,119,320,0x001f);
    Glib_FilledRectangle(120,160,239,320,0xffff);
    Glib_FilledRectangle(100,100,150,200,0x5555);

    rLCDCON1 &= ~(0x1);	// ENVID off.
	rLCDCON1 = (rLCDCON1 & ~(0x3ff<<8)) | (9<<8) | 1;	// ENVID on. 5MHz 60fps

	// ON PWREN signal
	rLCDCON5 = (rLCDCON5 & ~(1<<3)) ;	// PWREN disable
	rGPGCON = (rGPGCON & ~(0x3<<8)) | 0x1<<8;	// GPG4, PWR_EN -> Output.
	rGPGDAT&=(~(1<<4)); // GPG4=Low
	Delay(50); // GPG4=Low
	rGPGDAT|=(1<<4); //GPG4=High

	//--------LCD frame interrupt setting ------------------------------------START
	pISR_LCD=(unsigned)Lcd_Int_Frame_ForDvs;
	rINTMSK &= ~(BIT_LCD);
	rLCDINTMSK=(1<<2)|(0<<1)|(1); // 8Words Trigger Level,Unmask Frame int,mask Fifo int
	//--------LCD frame interrupt setting ------------------------------------END

}


void Led_Onoff(int Led_No, int On_Off)
{
	// GPF7  GPF6   GPF5   GPF4
	//nLED_8 nLED4 nLED_2 nLED_1
	if(On_Off==ON) rGPFDAT = (rGPFDAT & ~Led_No);
	else rGPFDAT = (rGPFDAT | Led_No);
}

void Timer_Setting(void)
{
//	float cnt_val=0.0;

    Uart_Printf("[ Timer 0 Interrupt setting]\n");

	// Uart_Printf("[Timer interval: %4.1f, %4.1f, %4.1f, %4.1f[ms].\n", \
	// timer_val_arr[0]*0.081, timer_val_arr[1]*0.081, timer_val_arr[2]*0.081, timer_val_arr[3]*0.081);
	// 296, 617, 802, 1284 -> 24ms, 50ms, 65ms, 104ms for eac

	rGPBCON = rGPBCON& ~(0x03) | 0x2;
	rCLKCON |= 1<<8;	// PWM timer PCLK clock enable.

    pISR_TIMER0 = (int)Timer0_intr;
	// Timer interupt start.
	rINTMSK &= ~BIT_TIMER0;
	
	rTCFG0 = 0xff;		// Prescaler0 = 0xff=256.
	// Timer input clock freq = PCLK/(Prescaler0+1)/divider value.
	// where, PCLK = 50.7MHz

	rTCFG1 = 0<<20 | 3<<0;	// MUX0 = 1/16

	// Timer input clock frequency = PCLK/(prescaler value+1)/(divider value)
	// 1clock = 81us.
    rTCNTB0 = timer_val_arr[0];
    rTCMPB0 = 0;

	// Load timer cnt value
	rTCON = 1<<3| 1<<1;

	// Timer start.
	rTCON = 1<<3| 1<<0;

}


