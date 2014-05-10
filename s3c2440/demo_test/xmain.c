//====================================================================
// File Name : xmain.c
// Function  : S3C2440 Test Main Menu
// Program   : Junon
// Date      : Nov 29, 2003
// Version   : 0.0
// History
//   0.0 : Programming start (February 20,2002) -> 
//====================================================================

#include <stdlib.h>
#include <string.h>

#include "def.h"
#include "option.h"
#include "2440addr.h"
#include "2440lib.h" // 03.11.27 junon
#include "mmu.h" // 03.11.27 junon

#if USE_MAIN
#include <stdio.h>
#endif


/**************** User test program header file ********************/
#include	"User_Test.h"
#include	"PD6710.h"		// PCMCIA
#include	"int.h"			// Interrupt
#include	"nwait.h"		// nWAIT signal
#include	"stone.h"		// Stepping stone
#include	"cpuspeed.h"	// Cpu speed test.
#include	"Userlib.h"
#include	"power.h"
#include	"nand.h"
#include	"lcd.h"
#include	"camif.h"
#include	"iic.h"
#include	"rtc.h"
#include	"spi.h"
#include	"uart_test.h"
#include	"uart0.h"
#include	"uart1.h"
#include	"uart2.h"
#include	"irda.h"
#include	"Glib.h"
#include	"LCDlib.h"
#include	"Flash.h"
#include	"Am29f800.h"
#include	"strata32.h"

#include	"dma.h"
#include	"sdi.h"
#include	"adcts.h"
#include	"timer.h"

#include	"iis.h"
#include	"ac97.h"	//Added to S3C2440A

#include	"dvstest.h"
#include	"pwr_c.h" // 03.11.27 junon

void Isr_Init(void);
void HaltUndef(void);
void HaltSwi(void);
void HaltPabort(void);
void HaltDabort(void);
void Show_Menu(void);

void Clk0_Enable(int clock_sel);	
void Clk1_Enable(int clock_sel);
void Clk0_Disable(void);
void Clk1_Disable(void);

extern void Calc_Clock(int print_msg);
extern U32 Mdiv, Pdiv, Sdiv, Fclk, Hclk, Pclk, Hdivn, Pdivn, Hclk_Ratio, Pclk_Ratio, Ref_Cnt;

#if 0
void *function[][2]=
{
//User test
	(void *)User_Test,			"User Test       ",
	(void *)Manual_Register_Set,"Manual Reg. Set ",
//Memory
	(void *)Test_PD6710,		"PCMCIA test     ",
	(void *)Test_ISram,			"Stepping stone  ",
	//(void *)Test_WaitPin,			"nWAIT test      ",
	(void *)Test_Nand,			"Nand test       ",
	(void *)ProgramFlash,		"Program Flash   ",
//DMA
	(void *)Test_DMA,			"DMA test        ",
//Interrupt
	(void *)Test_Int,			"Interrupt       ",
//Power & CPU
	//(void *)Test_CpuSpeed,	"Cpu speed       ",
	(void *)Power_Test,			"Power/Clk       ",
//IPs
	(void *)Lcd_Test,			"Lcd test        ",
	(void *)Camera_Test,		"Camera test     ",
	(void *)Spi_Test,			"SPI Test        ",
	(void *)Iic_Test,			"IIC Test        ",
	(void *)Rtc_Test,			"RTC Test        ",
	(void *)IrDA_Test,			"IrDA Test       ",
	(void *)Test_SDI,			"SD test         ",
	(void *)Test_Adc,			"ADC test        ",
	(void *)Test_AdcTs,			"ADC TS test     ",
	(void *)Timer_Test,			"Timer test      ",
	(void *)IIS_Test,			"IIS test        ",
	(void *)AC97_Test,			"AC97 Test	   ",
	(void *)Uart_Test,			"Uart Test       ",
	0,0
};
#endif
// Clock select argument.
#define	UCLK_SEL	(1)
#define	HCLK_SEL	(3)
#define	PCLK_SEL	(4)
#define	DCLK_SEL	(5)
// Hidden
#define	MPLL_SEL	(0)
#define	FCLK_SEL	(2)


//===================================================================

int Main(void)
{
	int i, voltage=120;
	char ch1;
//	unsigned int mpll_val;

	// GPIO port init.
	Port_Init();

	Led_Display(0xf);

	// MMU init. I/D cache on.
//	MMU_Init();

//	__asm("MSR CPSR_c,#0x10");	  //enter usr mode
   
	Led_Display(0x1);

	//////// user can set clock again.	/////////
#if 0
	Delay(0);
	voltage=110;
	Max1718_Set(voltage);		// set proper voltage after clock changed.
	Delay(1);

	#if FIN==12000000	//When 12MHz.
	ChangeUPllValue(56,2,2);		// 48MHz
	for(i=0; i<7; i++);
	ChangeClockDivider(13,12);
	ChangeMPllValue(68,1,1);		//2440x
	#else	// else xtal_in=16.9344MHz.
	ChangeUPllValue(60,4,2);		// 48MHz
	for(i=0; i<7; i++);
	ChangeClockDivider(13,12);
	//ChangeMPllValue(118,2,2);	//265Mhz
	ChangeMPllValue(97,1,2);		//295Mhz
	//ChangeMPllValue(110,3,1);	//398Mhz
	//ChangeMPllValue(120,2,1);	//531Mhz
	#endif
#endif
	/////////////////////////////////////////

	Led_Display(0x2);
	
	Calc_Clock(0);
	UPDATE_REFRESH(Hclk);

	// ISR init
	Isr_Init();
	Uart_Init(0, 115200);
	Uart_Select(0);
	Uart_TxEmpty(0);

	// timer setting for delay 100us unit.
	//Check whether or not the POWER_OFF wake-up.
	Delay(0);	//calibrate Delay()

	 //Turn on LCD.
	PWR_Lcd_Tft_16Bit_240320_On();

	//Check_SleepWakeUp();  //It's needed only for sleep mode test.   


	//Save the wasted power consumption on GPIO.
	//rIISPSR=(2<<5)|(2<<0); //IIS_LRCK=44.1Khz @384fs,PCLK=50Mhz.

	// Set I/O strength control.
	rDSC0 = (0<<31)|(0x3<<8)|(0xff<<0);
//	rDSC0 = (0<<31)|(0x0<<8)|(0x00<<0);
	// nEN_DSC	[31]	: 0:I/O drive strength enable, 1:Disable
	// DSC_ADR	[9:8]	: Addr drive strength, 0:10mA, 1:8mA, 2:6mA, 3:4mA
	// DSC_DATA	[7:0]	: DATA drive strength, 0:12mA, 1:10mA, 2:8mA, 3:6mA
	rDSC1 = (0x3<<28)|(0x3<<26)|(0xfffff<<0);
//	rDSC1 = (0x0<<28)|(0x0<<26)|(0x00000<<0);	
	// DSC_SCK1	[29:28]	: SCLK1, 0:16mA, 1:12mA, 2:8mA, 3:6mA 
	// DSC_SCK0	[27:26]	: SCLK0, 0:16mA, 1:12mA, 2:8mA, 3:6mA 
	// DSC_SCKE	[25:24]	: SCLKE, 0:10mA, 1:8mA, 2:6mA, 3:4mA 
	// DSC_SDR	[23:22]	: nRAS/nCAS, 0:10mA, 1:8mA, 2:6mA, 3:4mA 
	// DSC_NFC	[21:20]	: Nand flash(nFCE,nFRE,nFWE,CLE,ALE), 0:10mA, 1:8mA, 2:6mA, 3:4mA 
	// DSC_BE	[19:18]	: nBE[3:0], 0:10mA, 1:8mA, 2:6mA, 3:4mA 
	// DSC_WOE	[17:16]	: nBE[3:0], 0:10mA, 1:8mA, 2:6mA, 3:4mA 
	// DSC_CS7	[15:14]	: nGCS7, 0:10mA, 1:8mA, 2:6mA, 3:4mA 
	// DSC_CS6	[13:12]	: nGCS6, 0:10mA, 1:8mA, 2:6mA, 3:4mA 
	// DSC_CS5	[11:10]	: nGCS5, 0:10mA, 1:8mA, 2:6mA, 3:4mA 
	// DSC_CS4	[9:8]	: nGCS4, 0:10mA, 1:8mA, 2:6mA, 3:4mA 
	// DSC_CS3	[7:6]	: nGCS3, 0:10mA, 1:8mA, 2:6mA, 3:4mA 
	// DSC_CS2	[5:4]	: nGCS2, 0:10mA, 1:8mA, 2:6mA, 3:4mA 
	// DSC_CS1	[3:2]	: nGCS1, 0:10mA, 1:8mA, 2:6mA, 3:4mA 
	// DSC_CS0	[1:0]	: nGCS0, 0:10mA, 1:8mA, 2:6mA, 3:4mA 
	Uart_Printf("\nuDSC0/1:%x,%x\n", rDSC0, rDSC1);


	/////////////////////////////////////// start testprogram /////////////////////////////////////
	Uart_Printf("\n\n+---------------------------------------------+\n");
	Uart_Printf(    "| S3C2440A Firmware-Test ver 0.03 Jan 2004.   |\n");
	Uart_Printf(    "+---------------------------------------------+\n");
	Uart_Printf("[CPU ID=%xh]\n", rGSTATUS1);
	
	Uart_Printf("[Core voltage: %4.2fV]\n", (float)voltage/100);

	Uart_Printf("[XTAL in = %6.4f]\n", (float)FIN/MEGA);
	Uart_Printf("[Fclk:Hclk:Pclk]=[%4.1f:%4.1f:%4.1f]Mhz\n", (float)Fclk/MEGA, (float)Hclk/MEGA, (float)Pclk/MEGA);
	Uart_Printf("[Uclk=%4.1fMhz]\n", (float)UCLK/MEGA);
	Uart_Printf("\n");

	//Uart_Printf("[rPRIORITY=0x%x]\n", rPRIORITY);
	//rCLKCON = (rCLKCON&0xf) |(0x0<<16)|(0x2<<12)|(0x4<<8)|(0x0<<4); // for test
	//Uart_Printf("[CLKCON=%x]\n", rCLKCON);
	Uart_Printf("[rSTATUS2=0x%x]\n", rGSTATUS2);
	Uart_Printf("[rSTATUS3=0x%x]\n", rGSTATUS3);
	Uart_Printf("[rSTATUS4=0x%x]\n", rGSTATUS4);
	Uart_Printf("[rSRCPND=0x%x]\n", rSRCPND);
	Uart_Printf("[rINTPND=0x%x]\n", rINTPND);
	
	// CLKOUT0/1 select.
	//Uart_Printf("CLKOUT0:MPLL in, CLKOUT1:RTC clock.\n");
	Clk0_Enable(2);	// 0:MPLLin, 1:UPLL, 2:FCLK, 3:HCLK, 4:PCLK, 5:DCLK0
	Clk1_Enable(3);	// 0:MPLLout, 1:UPLL, 2:RTC, 3:HCLK, 4:PCLK, 5:DCLK1	
	//Clk0_Disable();
	//Clk1_Disable();
	
	Uart_Printf("\n\n");
	
///////////////////// menu start ///////////////////////////
	while(1) 
	{
	   	//i = 0;
		Show_Menu();		
		Uart_Printf("\nSelect the function to test : ");
		i = Uart_GetIntNum();
//		ch1 = Uart_Getch();
		Uart_Printf("\n",ch1);
//		i = ch1 - 0x30;
		switch(i)
		{
		   case 1:	User_Test();  //ok
		            break;
		   case 2:	Manual_Register_Set();
		   		    break;
		   case 3:	Test_PD6710();
		   		    break;
		   case 4:	Test_ISram();
		   		    break;
		   case 5:	Test_Nand();
		   		    break;
		   case 6:	ProgramFlash();
		   		    break;
		   case 7:	Test_DMA();
		   		    break;
		   case 8:	Test_Int();	 //ok
		   		    break;
		   case 9:	Power_Test(); //ok
		   		    break;
		   case 10:	Lcd_Test();	  //ok
		   		    break;
		   case 11:	Camera_Test(); //ok
		   		    break;
		   case 12:	Spi_Test();	//ok
		   		    break;
		   case 13:	Iic_Test();	 //ok
		   		    break;
		   case 14:	Rtc_Test();	 //ok
		   		    break;
		   case 15:	IrDA_Test(); //ok
		   		    break;
		   case 16:	Test_SDI();	 //0k
		   		    break;
		   case 17:	Test_Adc();	 //ok
		            break;
		   case 18:	Test_AdcTs();//ok
		            break; 					
		   case 19:	Timer_Test();//ok
		            break; 					
		   case 20:	IIS_Test();	//ok
		            break; 					
		   case 21:	AC97_Test(); //ok
		            break;					
		   case 22:	Uart_Test();//ok
		            break;													  
		   default: break;
		}
	
		Uart_Printf("\n");
	}

} // end of xmain().

//===================================================================
void Isr_Init(void)
{
    pISR_UNDEF  = (unsigned)HaltUndef;
    pISR_SWI    = (unsigned)HaltSwi;
    pISR_PABORT = (unsigned)HaltPabort;
    pISR_DABORT = (unsigned)HaltDabort;
	
    rINTMOD     = 0x0;					 //All=IRQ mode
//    rINTCON=0x5;						   //Non-vectored,IRQ enable,FIQ disable    
    rINTMSK     = BIT_ALLMSK;			  //All interrupt is masked.
//   	pISR_URXD0=(unsigned)Uart0_RxInt;	
//	rINTMSK=~(BIT_URXD0);   //enable UART0 RX Default value=0xffffffff


	//pISR_FIQ,pISR_IRQ must be initialized
   
   // rINTSUBMSK  = BIT_SUB_ALLMSK;		  //All sub-interrupt is masked. <- April 01, 2002 SOP

//    rINTSUBMSK  = ~(BIT_SUB_RXD0);		 //Enable Rx0 Default value=0x7ff
//    rINTMSK     = ~(BIT_UART0);			//Enable UART0 Default value=0xffffffff    
	
//    pISR_UART0=(unsigned)RxInt;			//pISR_FIQ,pISR_IRQ must be initialized
}

//===================================================================
void HaltUndef(void)
{
    Uart_Printf("Undefined instruction exception.\n");
    while(1);
}

//===================================================================
void HaltSwi(void)
{
#if !SEMIHOSTING	
		Uart_Printf("SWI exception!!!\n");
		while(1);
#endif	
}

//===================================================================
void HaltPabort(void)
{
    Uart_Printf("Pabort exception.\n");
    while(1);
}

//===================================================================
void HaltDabort(void)
{
    Uart_Printf("Dabort exception.\n");
    while(1);
}

void Clk0_Enable(int clock_sel)	
{	// 0:MPLLin, 1:UPLL, 2:FCLK, 3:HCLK, 4:PCLK, 5:DCLK0
	rMISCCR = rMISCCR&~(7<<4) | (clock_sel<<4);
	rGPHCON = rGPHCON&~(3<<18) | (2<<18);
}
void Clk1_Enable(int clock_sel)
{	// 0:MPLLout, 1:UPLL, 2:RTC, 3:HCLK, 4:PCLK, 5:DCLK1	
	rMISCCR = rMISCCR&~(7<<8) | (clock_sel<<8);
	rGPHCON = rGPHCON&~(3<<20) | (2<<20);
}
void Clk0_Disable(void)
{
	rGPHCON = rGPHCON&~(3<<18);	// GPH9 Input
}
void Clk1_Disable(void)
{
	rGPHCON = rGPHCON&~(3<<20);	// GPH10 Input
}
void Show_Menu(void)
{
    Uart_Printf("\n================ Menu =================\n");
	Uart_Printf("1 :User Test       \n");
	Uart_Printf("2 :Manual Reg. Set\n");
	Uart_Printf("3 :PCMCIA test \n");
	Uart_Printf("4 :Stepping stone\n");
	Uart_Printf("5 :Nand test\n");
	Uart_Printf("\n");
	Uart_Printf("6 :Program Flash\n");
	Uart_Printf("7 :DMA test \n");
	Uart_Printf("8 :Interrupt\n");
	Uart_Printf("9 :Power/Clk   \n");
	Uart_Printf("10:Lcd test  \n");
	Uart_Printf("\n");
	Uart_Printf("11:Camera test\n");
	Uart_Printf("12:SPI Test \n");
	Uart_Printf("13:IIC Test \n");
	Uart_Printf("14:RTC Test \n");
	Uart_Printf("15:IrDA Test \n");	
	Uart_Printf("16:SD test \n");
	Uart_Printf("17:ADC test \n");
	Uart_Printf("\n");
	Uart_Printf("18:ADC TS test \n");
	Uart_Printf("19:Timer test \n");
	Uart_Printf("20:IIS test \n");
	Uart_Printf("21:AC97 Test\n");
	Uart_Printf("22:Uart Test \n");
	Uart_Printf("\n=============End Menu =================\n");   
}
