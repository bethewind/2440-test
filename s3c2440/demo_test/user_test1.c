#include <stdio.h>
#include <string.h>
#include "def.h"
#include "option.h"
#include "2440addr.h"
#include "2440lib.h"

#include "userlib.h"
#include "user_test1.h"
#include "dvstest.h"

// Clock calculation. Current clock information calc.
// Register value : MPLLCON(Mdiv, Pdiv, Sdiv), CLKDIVN(Hdivn, Pdivn).
// Hclk = Fclk/Hclk_Ratio, Pclk = Fclk/Hclk_Ratio/Pclk_Ratio.
// Refresh count(Ref_Cnt) = 2048+1-(Hclk*7.8)
extern void Calc_Clock(int pring_msg);
extern U32 Mdiv, Pdiv, Sdiv, Fclk, Hclk, Pclk, Hdivn, Pdivn, Hclk_Ratio, Pclk_Ratio, Ref_Cnt;

void test1(void);
void Test_IO_Strength(void);


#define	Tacs	0
#define	Tcos	0
#define	Tacc	2
#define	Tcoh	0
#define	Tcah	0


void Sram_timing(void)
{
	volatile U16 i, k;
	U32 bk_bankcon5;
	
	Uart_Printf("Sram test...\n");

	bk_bankcon5 = rBANKCON5;

	// Setting bankcon 5.
	rBWSCON = (rBWSCON&~(0xf<<20)) | (1<<23) | (1<<22) |(1<<20);
	rBANKCON5 = (rBANKCON5&~(0x7ff<<4)) | (Tacs<<13) | (Tcos<<11) | (Tacc<<8) | (Tcoh<<6) | (Tcah<<4) ;

	Uart_Printf("rBANKCON5:%x\n", rBANKCON5);
	for(i=0; ; i++) {
		*(U16 *) 0x28000002 = 1;
		k=*(U16 *) 0x28000002;
		if(Uart_GetKey()) break;
	}

	rBANKCON5 = bk_bankcon5;
	while(1)if(Uart_GetKey()) break;;
	
}

void Dclk_test(void)
{
	U32 bk_dclkcon, bk_gphcon, bk_misccr;
	
	bk_dclkcon = rDCLKCON;
	bk_gphcon = rGPHCON;
	bk_misccr = rMISCCR;
	
	Uart_Printf("DCLK test.\n");
	//rDCLKCON = (1<<24) | (3<<20) | (1<<17) | (1<<16); // Dclk1=Uclk/4 50% duty.
	rDCLKCON = (0<<24) | (1<<20) | (1<<17) | (1<<16);	// Dclk1=Uclk/4 50% duty.
	rGPHCON = (rGPHCON&~(0x3<<20)) | (0x2<<20);
	rMISCCR = (rMISCCR & ~(0x7<<8)) | (0x1<<8);
	Uart_Printf("Press any key to stop.\n");
	Uart_Getch();
	rDCLKCON=bk_dclkcon;
	rGPHCON=bk_gphcon;
	rMISCCR=bk_misccr;
}

////////////////////////////// User_test1() end ///////////////////////////////

void test1(void)
{
//	int i;
	
	Uart_Printf("\nUser_Test1 program...\n");
	
	Uart_Printf("Current Clock information...\n");

	Calc_Clock(1);
	
	Uart_Printf("Register setting value\n");
	Uart_Printf("MDIV:PDIV:SDIV = %x:%x:%x\n", Mdiv, Pdiv, Sdiv);
	Uart_Printf("HDIVN:PDIVN = %x:%x\n", Hdivn, Pdivn);
	
	Uart_Printf("FCLK:HCLK:PCLK = %d:%d:%d kHz\n", Fclk/1000, Hclk/1000, Pclk/1000);

	Uart_Printf("Refresh count :%d(Must be %d)\n", rREFRESH&0x7ff, Ref_Cnt);
	
}

////////////////////////////// User_Test1() start ///////////////////////////////
void User_Test1(void)
{
	Uart_Printf("User test 1.\n");
	test1();
		

}


void ticker_test(void);

////////////////////////// ticker test /////////////////////////////////////
void ticker_test(void)
{
	int divider=3;//, i;
	U32 unit_us, Time;

	Uart_Printf("\nTicker.\n");
	Calc_Clock(0);

	divider=0;
	unit_us = Init_Ticker(divider); //0:16, 1:32, 2: 64, 3:128us divider
	Ticker_Start();
	Delay(1000);//for(i=0; i<100000; i++);
	Time=Ticker_Stop();

	Uart_Printf("time(%x)=%f[sec]\n",Time, (float)Time*(float)unit_us*0.000001);
	Uart_Printf("(Max ticker time: %fsec)\n", (float)unit_us*0.000001*65536);
}

