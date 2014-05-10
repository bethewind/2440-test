/*****************************************
  NAME: pll.c
  DESC: PLL & Maximum frequency test.
  HISTORY:
  03.21.2002:purnnamu: first release for S3C2410X
  06.02.2003:DonGo: Modified for S3C2440X
  Jan.e.2004:DonGo: Modified for S3C2440A
 *****************************************/
#include <math.h>
#include <stdlib.h>

#include "def.h"
#include "option.h"
#include "2440addr.h"
#include "2440lib.h"
#include "2440slib.h" 
#include "pll.h"
#include "memtest.h"

extern Calc_Clock(int print_msg);
extern void Test_IO_Strength(void);
extern U32 Mdiv, Pdiv, Sdiv, Fclk, Hclk, Pclk, Hdivn, Pdivn, Hclk_Ratio, Pclk_Ratio, Ref_Cnt;


void Sram_Test(void);

#if FIN==12000000
volatile U32 Mps_Val[][4]={
	// FCLK,	 M,  P,  S.
	{271500000, 173,  2, 2},
	{304000000,   68,  1, 1},
	{338400000, 133,  3, 1},
	{372000000,   85,  1, 1},
	{406430000, 127,  2, 1},
	{451200000, 180,  3, 1},
	{496000000, 116,  1, 1},
	{532800000, 125,  1, 1},
	{542400000, 218,  3, 1}
};
#else
volatile U32 Mps_Val[][4]={
	// FCLK,	 M,  P,  S.
	{266716800, 118,  2, 2},
	{296352000,   97,  1, 2},
	{331914240,   90,  3, 1},
	{370440000, 167,  6, 1},
	{399651840, 110,  3, 1},
	{431827200,   94,  2, 1},
	/*{470776320, 131,  3, 1},
	{500774400, 199,  5, 1},
	{530611200,   86,  1, 1},
	{541900800, 120,  2, 1}
	*/
};
#endif

int No_Of_Mps_Val=sizeof(Mps_Val)/16;


void Test_PLL(void)
{
	int voltage=120; //temp, 
	int i,j, hdiv_val, pdiv_val;//, fclk;//, mdiv, pdiv, sdiv, sval;
	//U32 fclk;
	U32 prev_fclk;
	U32 mpllcon_bk, clkdivn_bk, camdivn_bk;

	// clock register backup.
	mpllcon_bk = rMPLLCON;
	clkdivn_bk = rCLKDIVN;
	camdivn_bk = rCAMDIVN;
	
	Uart_Printf("[PLL frequency change test 100,000 times]\n");
	//Uart_TxEmpty(1);

	rLOCKTIME=((U32)(1807*0.8)<<12)|((U32)(1807*0.8)<<0); 
	//rLOCKTIME=((U32)(12048)<<16)|((U32)(12048)<<0); 
	// 1807 is 150us at 12Mhz

	j=0;
	while(1) 
	{
		if(Uart_GetKey()==ESC_KEY) 
			break;
		if(j>100000) 
			break;	

		i= j%No_Of_Mps_Val;


		//fclk = Mps_Val[i][0];
		//Uart_Printf("[%d]FCLK=%d [Mps_Val=%xh,%xh,%xh]\n", j, Mps_Val[i][0],Mps_Val[i][1],Mps_Val[i][2],Mps_Val[i][3]);
		Uart_Printf("[%d] ", j);
		Uart_TxEmpty(1);

		if(Mps_Val[i][0]<=350000000)
		{
			hdiv_val=12; 
			pdiv_val=12;
			voltage=110;
		}
		else if(Mps_Val[i][0]<= 440000000) 
		{
			hdiv_val=13; 
			pdiv_val=12;
			voltage=120;
		}
		else
		{
			hdiv_val=14; 
			pdiv_val=12;
			voltage=135;
		}	
		
		Led_Display(j%2);
		//Uart_Printf("[FCLK:HCLK:PCLK=1:%d:%d=", hdiv_val%10, (hdiv_val%10)*(pdiv_val%10));
		//Uart_Printf("%4.1f:%4.1f:%4.1f MHz]\n", (float)fclk/MEGA, (float)fclk/MEGA/(hdiv_val%10), (float)fclk/MEGA/((hdiv_val%10)*(pdiv_val%10)));
		Uart_Printf("Voltage:%4.2f[V]\n", (float)voltage/100);
		Max1718_Set(voltage);
		Delay(10);

		if(prev_fclk<Mps_Val[i][0]) {
			Led_Display(0x1);		
			ChangeClockDivider(hdiv_val, pdiv_val);
			Led_Display(0x2);
			ChangeMPllValue(Mps_Val[i][1],Mps_Val[i][2],Mps_Val[i][3]);
		} 
		else 
		{
			Led_Display(0x2);
			ChangeMPllValue(Mps_Val[i][1],Mps_Val[i][2],Mps_Val[i][3]);
			Led_Display(0x1);		
			ChangeClockDivider(hdiv_val, pdiv_val);
		}
		
		Calc_Clock(1);
		Led_Display(0x4);
		UPDATE_REFRESH(Hclk);
		Led_Display(0x8);

#if 1
		Mem_Test(1);
#endif

		prev_fclk = Mps_Val[i][0];
		j++;
	}  

	Uart_Printf("\nMPLL change test OK.\n");

	Uart_Printf("\nReturn to Previous setting.\n");
	rMPLLCON = mpllcon_bk;
	rCLKDIVN = clkdivn_bk;
	rCAMDIVN = camdivn_bk;
	Calc_Clock(1);

}



// lock time: user lock time setting time[us],
// lock_count: register setting value for wanted locktime.
#define TIME_OF_XTAL	((float)MEGA/(float)FIN)	//us
U32 Calc_Locktime(U32 lock_time)
{
	U32 lock_count=0;

	lock_count = (U32)((float)lock_time/((float)TIME_OF_XTAL));

	return lock_count;
}

void Test_Locktime(void)
{
//	int temp;
	int i;//,j, mdiv, pdiv, sdiv, sval, fclk, hdiv_val, pdiv_val;
	U32 lock_time=300, lock_count=12048;	//Unit is us.
	U32 mpllcon_bk;

	// clock register backup.
	mpllcon_bk = rMPLLCON;

	Uart_Printf("[PLL Lock time test 100,000 times]\n");
	Uart_Printf("Lock time range: 300us - %d [us]\n", (U32)(0xffff*TIME_OF_XTAL));
	Uart_Printf("Input Lock time[us]: ");
	lock_time = Uart_GetIntNum();

	lock_count = Calc_Locktime(lock_time);	// lock_time [us]
	rLOCKTIME=((U32)(lock_count)<<16)|((U32)(lock_count)<<0); 
	Uart_Printf("Locktime register value is %d(%xh).\n", rLOCKTIME&0xffff, rLOCKTIME&0xffff);
	
	Uart_Printf("Clock will be locked after LED7 is ON.\n");

	i=0;
	//ChangeClockDivider(12,12);		// 1:2:4	
	while(1) 
	{
		if(Uart_GetKey()!=0x0) 
			break;
		if(i>100000) 
			break;
		if(i%2) 
		{
			Led_Display(0x8);
			rMPLLCON = mpllcon_bk;
		} 
		else 
		{		
			Led_Display(0x0);
			rMPLLCON = mpllcon_bk;
		}
		//Delay(1000);
		i++;
	}	
	Uart_Printf("[PLL Lock time test %d times end]\n",i-1);
}


void Manual_Change_Pll(void)
{
	U32 pdiv, mdiv, sdiv, sval, hdiv_val, pdiv_val, voltage=120;//	 , fclk
	int i, fclk_sel;
	float ffclk;

	Uart_Printf("[Running change test of M/P/S value]\n");

	for(i=0; i<sizeof(Mps_Val)/16; i++) {
		if(i%2==0) Uart_Printf("\n");
		Uart_Printf("%2d: FCLK=%4.1f [%xh(%d),%xh,%xh]	", i, (float)Mps_Val[i][0]/MEGA,Mps_Val[i][1],Mps_Val[i][1],Mps_Val[i][2],Mps_Val[i][3]);
	}
	Uart_Printf("\n");

	#if 1
	Uart_Printf("Select FCLK No(0-%d): ", sizeof(Mps_Val)/16-1);
	fclk_sel=Uart_GetIntNum();
	mdiv = Mps_Val[fclk_sel][1];
	pdiv = Mps_Val[fclk_sel][2];
	sdiv = Mps_Val[fclk_sel][3];
	#else
	Uart_Printf("Input M vlaue : ");
	mdiv=Uart_GetIntNum();		
	Uart_Printf("Input P vlaue : ");
	pdiv=Uart_GetIntNum();	
	Uart_Printf("Input S vlaue : ");
	sdiv=Uart_GetIntNum();
	#endif

	Uart_Printf("Select FCLK : HCLK(1:1=11,1:2=12,1:3=13,1:4=14,...) : ");
	hdiv_val=Uart_GetIntNum();
	Uart_Printf("Select HCLK : PCLK(1:1=11,1:2=12) : ");
	pdiv_val=Uart_GetIntNum();

	sval=(int)pow(2,sdiv);
	ffclk = (U32) ( (((float)mdiv+8)*FIN*2.0)/(((float)pdiv+2)*(float)sval));	// pll2188x.
	Uart_Printf("Now change PLL value...(%dhz)[mps=%xh(%d), %d, %d]\n", (U32)ffclk, mdiv,mdiv, pdiv, sdiv);

	Uart_Printf("Voltage: ");
	voltage = Uart_GetIntNum();
	Max1718_Set(voltage);
	Delay(10);

	ChangeClockDivider(hdiv_val, pdiv_val);
	ChangeMPllValue(mdiv,pdiv,sdiv);

	Calc_Clock(1);
	UPDATE_REFRESH(Hclk);
	Uart_Init(Pclk, 115200);


	#if 1
	//Uart_Getch();
	Uart_Printf("Memory test (y/n)? ");
	if(Uart_Getch()=='y') {
		while(1) {
			Mem_Test(1);
			if(Uart_GetKey()!=0x0) break;
			//Uart_SendByte((int)'.');
		}
	}
	#endif
	
 }

void Manual_Change_Pll_Iostrength(void)
{

	Manual_Change_Pll();
	Test_IO_Strength();

#if 1
	//Uart_Getch();
	while(1) {
		Mem_Test(1);
	if(Uart_GetKey()!=0x0) break;
	}
#endif
}

void Test_PllOnOff(void)
{
	int i,j;
	unsigned int saveREFRESH;

	Uart_Printf("[PLL On/Off test, 10000 times]\n");
	Uart_TxEmpty(1);	//To avoid being crushed the character

	rLOCKTIME=((U32)(1807*0.8)<<12)|((U32)(1807*0.8)<<0); 
		// 1807 is 150us at 12Mhz
	Uart_Printf("count:                    ");

	for(j=0;j<10000;j++) {
		Led_Display(0x2);
		rCLKSLOW=(1<<7)|(1<<5)|(1<<4)|(0<<0); //FCLK=FIN/1,SLOW mode,MPLL=off,UPLL=off
		
		saveREFRESH=rREFRESH;
		rREFRESH=(1<<23)|(U32)(2048+1-12*7.8); //HCLK = 12MHz, Trp=2clk,Trc=4clk
	
		rCLKSLOW=(1<<7)|(0<<5)|(1<<4)|(0<<0);//PLL on,MPLL=on,UPLL=on
		for(i=0;i<2048/4;i++); //S/W MPLL lock-time
		Led_Display(0x0);
		rCLKSLOW=(1<<7)|(0<<5)|(0<<4)|(0<<0);//NORMAL mode,PLL on,MPLL=on,UPLL=on
	
		rREFRESH=saveREFRESH;
		if(j%500==0)
		{		
		Uart_Printf("\b\b\b\b\b\b%6d", j);
		Uart_TxEmpty(1);	//To avoid being crushed the character		
		}
	}
	Uart_Printf("\b\b\b\b\b\b%6d", j);
}




