/*************************************************
  NAME: sleep.c
  DESC: Sleep mode test
  HISTORY:
  08.24.2002:purnnamu
	LCDVFn is connected the analog circuit in LCD. 
	So, this signal should be output L.
  05.xx.2003:DonGo modifid for 2440X, Power-off -> Sleep. naming change.
 *************************************************/
 
#include "def.h"
#include "option.h"
#include "2440addr.h"
#include "2440lib.h"
#include "2440slib.h" 

#include "lcdlib.h"
#include "sleep.h"

#define TESTYEAR    (0x00)
#define TESTMONTH   (0x12)
#define TESTDATE    (0x31)
#define TESTDAY	    (0x06)  // SUN:1 MON:2 TUE:3 WED:4 THU:5 FRI:6 SAT:7   
#define TESTHOUR    (0x23)
#define TESTMIN	    (0x59)
#define TESTSEC	    (0x59)

#define TESTYEAR2   (0x01)
#define TESTMONTH2  (0x01)
#define TESTDATE2   (0x01)
#define TESTDAY2    (0x07)  // SUN:1 MON:2 TUE:3 WED:4 THU:5 FRI:6 SAT:7   
#define TESTHOUR2   (0x00)
#define TESTMIN2    (0x00)
#define TESTSEC2    (0x00)


#define CHECK_SDRAM_SELFREFRESH	(1)
#define EXTERNAL_BUS_HOLDER	(1)  
//The SMDK2440X board has external databus buffer with bus hold function.
#define CHOOSE_EINT_TYPE	(1)
#define ALARM_WAKEUP		(1)


void SelectEintType(void);
#define SLEEP_COUNT (*(volatile unsigned int *)0x32000000)

//static void __irq Eint8_23Int_100Hz(void);

void Batt_Sleep(void);

extern void Manual_Register_Set(void);



//================================================================================
static void __irq Eint0Int(void)
{
	ClearPending(BIT_EINT0);
	Uart_Printf("[WU]EINT0 interrupt is occurred for Sleep mode wake-up.\n");
}


//================================================================================
static void __irq Eint8_23Int(void)
{
	rEINTPEND=(1<<11); //EINT11 sub-pending bit.	
	ClearPending(BIT_EINT8_23);
	Uart_Printf("[WU]EINT8_23 interrupt is occurred for Sleep mode wake-up.\n");
}


//================================================================================
/*static void __irq Eint8_23Int_100Hz(void)
{
	rEINTPEND=(1<<11); //EINT11 sub-pending bit.	
	ClearPending(BIT_EINT8_23);
	Uart_Printf("wui\n");
}*/


//================================================================================
static void __irq AlarmInt(void)
{
	Uart_Printf("INTPND:%x, SRCPND:%x \n", rINTPND, rSRCPND);
	ClearPending(BIT_RTC);
	Uart_Printf("[WU]ALARM interrupt is occurred for Sleep mode wake-up.\n");
}


//================================================================================
void Test_InitSDRAM(void)
//Initialize SDRAM for SDRAM self-refresh test.
{
	int i;
	
	Uart_Printf("[SDRAM Initialization]\n");
	Uart_Printf("Fill SDRAM for self-refresh test.\n");
	for(i=_NONCACHE_STARTADDRESS;i<(_NONCACHE_STARTADDRESS+0x400000);i+=4)
		*((U32 *)i)=i^0x55555555;
	Uart_Printf("Filling SDRAM is completed.\n");

	*((volatile U32 *)0x33e00000)=0x0;  //for Initialize the counter in Check_SleepWakeUp_100Hz(). 
}


//================================================================================
void SelectEintType(void)
{

	int extintMode;

	Uart_Printf("1.L-level  2.H-level  3.F-edge  4.R-edge  5.B-edge\n");
	Uart_Printf("Select the EINT0 trigger type:");	


	extintMode=Uart_Getch();

	rGPFCON=rGPFCON & ~(3<<0)|(2<<0); //PF0=EINT0
	
	switch(extintMode)
	{
	case '1':
		rEXTINT0=rEXTINT0&~(7<<0)|(0x0<<0); //EINT0=low level triggered
		Uart_Printf("L-level\n");
		break;
	case '2':
		rEXTINT0=rEXTINT0&~(7<<0)|(0x1<<0); //EINT0=high level triggered
		Uart_Printf("H-level\n");
		break;
	case '3':
		rEXTINT0=rEXTINT0&~(7<<0)|(0x2<<0);  //EINT0=falling edge triggered
		Uart_Printf("F-edge\n");
		break;
	case '4':
		rEXTINT0=rEXTINT0&~(7<<0)|(0x4<<0); //EINT0=rising edge triggered
		Uart_Printf("R-edge\n");
		break;
	case '5':
		rEXTINT0=rEXTINT0&~(7<<0)|(0x6<<0); //EINT0=both edge triggered
		Uart_Printf("B-edge\n");
		break;
	default:
		break;
	}
}


//================================================================================
void ConfigSleepGPIO(void)
{
	// Check point
	// 1) NC pin: input pull-up on 
	// 2) If input is driver externally: input pull-up off
	// 3) If a connected component draws some current: output low.
	// 4) If a connected component draws no current: output high.

	// Follow the configuration order for setting the ports. 
	// 1) setting value(GPnDAT) 
	// 2) setting control register  (GPnCON)
	// 3) configure pull-up resistor(GPnUP)  

	//CAUTION: Below configurations are only for SMDK2440 eva. board
	//*** PORT A GROUP
	// [22:nFCE] [21:nRSTOUT] [20:nFRE] [19:nFWE] [18:ALE] [17:CLE] [16:nGCS5]
	// [15:nGCS4] [14:nGCS3] [13:nGCS2] [12:nGCS1] [11:ADR26] [10:ADR25] [9:ADR24] [8:ADR23]  
	// [7:ADR22] [6:ADR21] [5:ADR20] [4:ADR19] [3:ADR18] [2:ADR17] [1:ADR16] [0:ADR0]
	rGPACON = 0x7fffff; 

	//**** PORT B GROUP
	// [10:nXDREQ0] [9:nXDACK0] [8:nXDREQ1] 
	// [7:nXDACK1] [6:nSS_KBD] [5:nDIS_OFF] [4:L3CLOCK] [3:L3DATA] [2:L3MODE] [1:nIrDATXDEN] [0:Keyboard]
	// * nXDREQ0/1:ext.pD, nSS_KBD:ext.pU, nDIS_OFF:ext.pD, nIrDATXDEN:ext.pU, Keyboard:ext.pU  
	//rGPBDAT = 0x0;
	//rGPBCON = 0x0;  // all input  
	//rGPBUP  = (1<<10)+(1<<8)+(1<<6)+(1<<5)+(1<<1)+(1<<0);//0x563;

	// modified for Voltage regulator /OE signal(GPB10), GPB10=output high.

	//rGPBCON = (1<<20)|(1<<16) | 0x0;  // GPB10=output  

	//rGPBUP  = (0<<10)+(0<<8)+(1<<6)+(1<<5)+(1<<1)+(1<<0);//0x563;


	//*** PORT C GROUP
	// [15:VD7] [14:VD6] [13:VD5] [12:VD4] [11:VD3] [10:VD2] [9:VD1] [8:VD0] 
	// [7:LCD_LPCREVB] [6:LCD_LPCREV] [5:LCD_LPCOE] [4:VM] [3:VFRAME] [2:VLINE] [1:VCLK] [0:LEND]
	// * LCD_LPCREV and LCD_LPCREVB are connected the analog circuit in LCD or board. So, this signal should be output L.
	rGPCDAT = 0x0;
	rGPCCON = (1<<14)+(1<<12);//0x5000;
	rGPCUP  = (1<<7)+(1<<6);//0xc0;  

	//*** PORT D GROUP
	// [15:VD23] [14:VD22] [13:VD21] [12:VD20] [11:VD19] [10:VD18] [9:VD17] [8:VD16] 
	// [7:VD15] [6:VD14] [5:VD13] [4:VD12] [3:VD11] [2:VD10] [1:VD9] [0:VD8]
	// * 5V tol. pad(GPD[10:8]) should be output L 
	// * The input of Maxim IC may have current sink, so the port should have configured output 'H'(GPD0)
	// * The output of Maxim IC drives 'H', so disable the pull-up(GPD1)
	rGPDDAT = (1<<0);//0x1;
	rGPDCON = (1<<20)+(1<<18)+(1<<16)+(1<<0);//0x150001;	
	rGPDUP  = (1<<10)+(1<<9)+(1<<8)+(1<<1);//0x702;

	//*** PORT E GROUP
	// [15:IICSDA] [14:IICSCL] [13:SPICLK] [12:SPIMOSI] [11:SPIMISO] [10:SDATA3] [9:SDDATA2] [8:SDDATA1] 
	// [7:SDDATA0] [6:SDCMD] [5:SDCLK] [4:I2SSDO] [3:I2SSDI] [2:CDCLK] [1:I2SSCLK] [0:I2SLRCK]
	// * 5V tol. pad(GPE[13:11]) should be output L 
	// * The output of CODEC IC drives 'H', so disable the pull-up
	// * IICSDA/IICSCL:ext pU, SDDAT[3:0]/SDCMD:ext pU 
	rGPEDAT = 0x0;
	rGPECON = (1<<26)+(1<<24)+(1<<22);//0x5400000;	
	rGPEUP  = (0xf<<12)+(0xf<<8)+(0x3<<6)+(1<<3);//0xffc8;

	//*** PORT F GROUP
	// [7:nLED_8] [6:nLED_4] [5:nLED_2] [4:nLED_1] [3:nIRQ_PCMCIA] [2:EINT2] [1:KBDINT] [0:EINT0]
	// * GPF[7:4] are connected to 'H' through LED, so make output 'H'
	// * nIRQ_PCMCIA=Hi-Z Before initialize
	// * EINT0 is the wake-up source
	// * EINT2/0:ext pU, KBDINT:Keyboard controller drives high 
	rGPFDAT = (0xf<<4);//0xf0;
	rGPFCON = (0x5<<12)+(0x5<<8)+2;//0x5502;
	rGPFUP  = (0xf<<4)+7;//0xf7;

	//*** PORT G GROUP
	// [15:EINT23] [14:EINT22] [13:EINT21] [12:EINT20] [11:EINT19] [10:nCD_SD] [9:DMAMODE] [8:DMASTART]
	// [7:KBDSPICLK] [6:KBDSPIMOSI] [5:KBDSPIMISO] [4:LCD_PWREN] [3:EINT11] [2:nSS_SPI] [1:IRQ_LAN] [0:IRQ_PCMCIA]
	// * 5V tol. pad(GPG[13:11]) should be output L
	// * GPG[15:13]:NAND boot config, EINT19:ext pU, nCD_SD:ext pU, EINT11:ext pU
	// * The input of Maxim IC may have current sink, so the port should have configured output 'H'(GPG9)
	// * The output of Maxim IC drives 'H', so disable the pull-up(GPG10)
	rGPGDAT = (1<<9);//0x200;
	rGPGCON = (1<<18)+(1<<14)+(1<<12)+(1<<10);//0x45400;
	rGPGUP  = (0xe<<12)+(0xe<<8)+(0xe<<4)+(1<<3);//0xeee8;

	//*** PORT H GROUP
	// [10:CLKOUT1] [9:CLKOUT0] [8:UCLK]
	// [7:nCTS1] [6:nRTS1] [5:RXD1] [4:TXD1] [3:RXD0] [2:TXD0] [1:nRTS0] [0:nCTS0] 
	// * WP_SP:ext pU
	// * The input of Maxim IC may have current sink, so the port should have configured output 'H'(GPH4/2/1)
	// * The output of Maxim IC drives 'H', so disable the pull-up(GPH5/3/0)
	rGPHDAT = (1<<4)+(1<<2)+(1<<1);  
	rGPHCON = (1<<8)+(1<<4)+(1<<2);//0x114; 
	rGPHUP  = (1<<8)+(1<<5)+(1<<3)+1;//0x129;

	//*** PORT J GROUP
	// [12:CAMRESET] [11:CAMCLKOUT] [10:CAMHREF] [9:CAMVS] [8:CAMPCLKIN] 
	// [7:CAMD7] [6:CAMD6] [5:CAMD5] [4:CAMD4] [3:CAMD3] [2:CAMD2] [1:CAMD1] [0:CAMD0]
	// * The output of buffer IC drives 'H', so disable the pull-up(GPJ[10:0])
	rGPJDAT = 0x0;  	
	rGPJCON = 0x0;  // all input
	rGPJUP  = 0x7ff;	

	//External interrupt will be falling edge triggered. 
	rEXTINT0 = 0x22222222;	// EINT[7:0]
	rEXTINT1 = 0x22222222;	// EINT[15:8]
	rEXTINT2 = 0x22222222;	// EINT[23:16]
}


//================================================================================
void SetAlarmWakeUp(void)
{
	rRTCCON = 0x1;	// R/W enable, 1/32768, Normal(merge), No reset

	rBCDYEAR = TESTYEAR;
	rBCDMON  = TESTMONTH;
	rBCDDAY  = TESTDAY;	// SUN:1 MON:2 TUE:3 WED:4 THU:5 FRI:6 SAT:7
	rBCDDATE = TESTDATE;
	rBCDHOUR = TESTHOUR;
	rBCDMIN  = TESTMIN;
	rBCDSEC  = TESTSEC;
	
	rALMYEAR=TESTYEAR2 ;
	rALMMON =TESTMONTH2;
	rALMDATE=TESTDATE2  ;
	rALMHOUR=TESTHOUR2 ;
	rALMMIN =TESTMIN2  ;
	rALMSEC =TESTSEC2+3;

	rRTCALM=0x7f;

	rRTCCON=0x0;   // R/W disable, 1/32768, Normal(merge), No reset
}

//================================================================================
U32 dsc0_bak, dsc1_bak, mslcon_bak;

void ConfigMiscIO(void)
{
	// memory data/control signal pin status definition when sleep.
	mslcon_bak = rMSLCON;
	
	// In the evaluation board, Even though in sleep mode, the devices are all supplied the power.
	//rMSLCON = (0<<11)|(0<<10)|(0<<9)|(0<<8)|(0<<7)|(0<<6)|(0<<5)|(0<<4)|(0<<3)|(0<<2)|(0<<1)|(0<<0);
	rMSLCON = (1<<11)|(1<<10)|(1<<9)|(1<<8)|(1<<7)|(1<<6)|(1<<5)|(1<<4)|(1<<3)|(1<<2)|(1<<1)|(1<<0);
	// Memory interface signal definition when sleep mode.
	//PSC_DATA	[11]	DATA status, 0:output "0" ( All 32-bit data Low drive ), 1:Hi-Z
	//PSC_WAIT	[10]	nWAIT status, 0:output "0", 1: input
	//PSC_RnB	[9]		RnB status, 0:output "0", 1: input
	//PSC_NF	[8]		NAND Flash I/O status, 0:inactive  nFCE,nFRE,nFWE,ALE,CLE = 11100, 1: Hi-Z
	//PSC_SDR	[7]		nSRAS, nSCAS  status, 0:inactive( "11"), 1: Hi-Z
	//PSC_DQM	[6]		DQM[3:0]/nWE[3:0]  status, 0:inactive( "0000"), 1:Hi-Z
	//PSC_OE	[5]		nOE status, 0:inactive( "1"), 1:Hi-Z
	//PSC_WE	[4]		nWE status, 0:inactive( "1"), 1:Hi-Z
	//PSC_GCS0	[3]		nGCS[0] status, 0:inactive( "1"), 1:Hi-Z
	//PSC_GCS[5:1][2]	nGCS[5:1] status, 0:inactive( "1"),  1:Hi-Z
	//PSC_GCS6	[1]		nGCS[6] status, 0:inactive( "1"), 1:Hi-Z
	//PSC_GCS7	[0]		nGCS[7] status, 0:inactive( "1"), 1:Hi-Z

#if 1
	Uart_Printf("Drive strength control.\n");

	dsc0_bak = rDSC0;
	dsc1_bak = rDSC1;

	rDSC0 = ((unsigned int)1<<31)|(3<<8)|(3<<0);
	// nEN_DSC	[31]	Enable Drive strength control, 1:enable
	// DSC_ADR	[9:8]	Address bus drive strength. 0:10mA, 1:8mA, 2:6mA, 3:4mA
	// DSC_DATA	[7:0]	Data bus drive strength, 0:12mA, 1:10mA, 2:8mA, 3:6mA
	rDSC1 = (3<<28)|(3<<26)|(3<24)|(3<<22)|(3<<20)|(3<<18);
	// 0:16mA, 1:12mA, 2:8mA, 3:6mA
	//DSC_SCK1	[29:28]	SCLK1
	//DSC_SCK0	[27:26]	SCLK0
	//DSC_SCKE	[25:24]	SCKE
	//DSC_SDR	[23:22]	nSRAS/nSCAS
	//DSC_NFC	[21:20]	Nand Flash(nFCWE, nFRE, nFWE,CLE,ALE) Control
	//DSC_BE	[19:18]	nBE[3:0]
	//DSC_WOE	[17:16]	nWE/nOE
	//DSC_CS7	[15:14]	nGCS7
	//DSC_CS6	[13:12]	nGCS6
	//DSC_CS5	[11:10]	nGCS5
	//DSC_CS4	[9:8]	nGCS4
	//DSC_CS3	[7:6]	nGCS3
	//DSC_CS2	[5:4]	nGCS2
	//DSC_CS1	[3:2]	nGCS1
	//DSC_CS0	[1:0]	nGCS0
#endif
	
}


//================================================================================
void Check_SleepWakeUp(void)
{
	Uart_Printf("[Sleep related registers]\n");

	Uart_Printf("GSTATUS2: PWRST=%d OFFRST=%d WDTRST=%d\n", 
	(rGSTATUS2&(1<<0))!=0,
	(rGSTATUS2&(1<<1))!=0,
	(rGSTATUS2&(1<<2))!=0);
	Uart_Printf("GSTATUS2:%x, GSTATUS2\n");
	Uart_Printf("GSTATUS3:%x(0x0,0x%x), GSTATUS4=%x(0x0,0xaaaaaaaa)\n",
	rGSTATUS3,(U32)StartPointAfterSleepWakeUp,rGSTATUS4);
	if(rGSTATUS2&(1<<0))
	{
	Uart_Printf("Power On Reset\n\n");
	rGSTATUS2=(1<<0);
	SLEEP_COUNT=0;
	return;
	}
	if(rGSTATUS2&(1<<1))
	{
	Uart_Printf("Sleep Wake-up\n\n");
	rGSTATUS2=(1<<1);
	Uart_Printf("EINTPEND=0x%x\n",rEINTPEND);
	Uart_Printf("SRCPND=0x%x\n",rSRCPND);
	// Added for wake-up 100Hz.
	SLEEP_COUNT = SLEEP_COUNT+1;
	Uart_Printf("SLEEP_COUNT=%d\n", SLEEP_COUNT);
	if(SLEEP_COUNT>1000000) 
	{
		Uart_Printf("\n\nSleep count is %d... any key to continue\n\n", SLEEP_COUNT);
		Uart_Getch();
	}

	Test_SleepMode();	// goto sleep mode again.
	}
	if(rGSTATUS2&(1<<2))
	{
		Uart_Printf("Watchdog Reset\n\n");
	rGSTATUS2=(1<<2);
	return;
	}
}


//================================================================================
void Test_SleepMode(void)
{
   
	int i;
//	U32 portStatus[33+3];
//	int error=0;
//	int mode;
	Uart_Printf("[Sleep Mode Test]\n");

	Uart_Printf("nBATT_FLT pin should be tested.\n");
	
	Uart_Printf("S3C2440 will wake up by EINT0 .\n");

/////////////////////////////////////////// add for battery fault...
#if 1
	Uart_Printf("\nBattery fault interrupt setting.\n");
	Uart_Printf("Detect low-level.\n");

	Uart_Printf("0 : Reset system during nBATT_FLT=0.\n");
	Uart_Printf("4 : System will be wake-up from sleep mode during nBATT_FLT=0.\n");
	Uart_Printf("6 : System will ignore Eint0 signal during nBATT_FLT=0.\n");
	Uart_Printf("7 : Disable Battery function.\n");
	Uart_Printf("Select battery function : ");
	i = Uart_GetIntNum() & 0x7;
	
	rMISCCR = (rMISCCR & ~(0x1f<<20)) | (i<<20);
	// [22:20]
	// if 0, System will be Reset during nBATT_FLT=0.
	// if 4, System will be wake-up from sleep mode during nBATT_FLT=0.
	// if 6, System will ignore nBATT_FLT signal.
	// if 7, Disable Battery function.

	Uart_Printf("rMISCCR:%x\n", rMISCCR);
	pISR_BAT_FLT = (U32)BattFaultInt;
	rSRCPND = BIT_BAT_FLT;
	rINTPND = BIT_BAT_FLT;

	rINTMSK = rINTMSK & ~(BIT_BAT_FLT);	// unmask.

#endif	
/////////////////////////////////////////// add for battery fault...end



#if 0	
	Uart_Printf("Check SDRAM for self-refresh test\n");
	for(i=_NONCACHE_STARTADDRESS;i<(_NONCACHE_STARTADDRESS+0x400000);i+=4)
	{
	//Uart_Printf("%x=%x(%x)\n",i,*((U32 *)i),i^0x55555555);
	if(*((U32 *)i)!=(i^0x55555555))
	{
		Uart_Printf("Mem Error:%x=%x(%x)\n",i,*((U32 *)i),i^0x55555555);
		 error++;
	}
	if(error>20)
		break;
	}
	if(error)
		Uart_Printf("SDRAM self-refresh test:FAILED\n"); 
	else 
	Uart_Printf("SDRAM self-refresh test:O.K.\n");	
#endif

	Uart_TxEmpty(1);	//Wait until UART0 Tx buffer empty.
	   

#if 1	  
	SelectEintType();
#else
	rGPFCON=rGPFCON & ~(3<<0)|(2<<0); //PF0=EINT0
	rEXTINT0=rEXTINT0&~(7<<0)|(2<<0);  //EINT0=falling edge triggered
#endif

#if 1	//test whether or not the interrupt pending bit retain the wake-up source.
	pISR_EINT0=(U32)Eint0Int;
	pISR_EINT8_23=(U32)Eint8_23Int;
	pISR_RTC=(U32)AlarmInt;
	rINTMSK = ~(BIT_EINT0|BIT_EINT8_23|BIT_RTC);
	for(i=0;i<100;i++);

	rSRCPND = BIT_EINT0|BIT_EINT8_23|BIT_RTC;
	rINTPND = BIT_EINT0|BIT_EINT8_23|BIT_RTC;

	rEINTPEND=rEINTPEND;
	rSRCPND=rSRCPND;
	rINTPND=rINTPND;
#endif	 


	rEINTMASK = rEINTMASK&~(1<<11); //SRCPND:EINT8_23 will be set by EINT11 after wake-up.
	rINTMSK = BIT_ALLMSK;
	// NOTE: Any interrupt can't be used in Sleep mode. 
	//	   because SDRAM is in self-refresh mode and ISR code will access SDRAM.

	Uart_Printf("Alarm wake-up (y/n)? ");
	if(Uart_Getch()=='y')
	{
		Uart_Printf("S3C2440 will wake up by RTC alarm(10 sec) or EINT0 .\n");
		SetAlarmWakeUp();
	}


	rRTCCON=0x0;   // R/W disable, 1/32768, Normal(merge), No reset
	rADCCON|=(1<<2);

	rMISCCR|=(1<<12); //USB port0 = suspend
	rMISCCR|=(1<<13); //USB port1 = suspend


	rGSTATUS3=(U32)StartPointAfterSleepWakeUp;	// memory control part in startup code.
	Uart_Printf("set restart address:%x\n", rGSTATUS3);
	rGSTATUS4=0xaaaaaaaa;

	Uart_TxEmpty(1);	//Wait until UART0 Tx buffer empty.
	ConfigSleepGPIO();
	ConfigMiscIO();

	//Max1718_Set(110);

	rMISCCR=rMISCCR|(3<<0); 	// Data[31:0] pull-up disable.

	Lcd_EnvidOnOff(0); //Before entering Sleep mode, LCD must be off	


	//=================================================================
	//  VERY IMPORTANT NOTE 
	//  To enter SLEEP/SLIDLE mode, MMU TLB-fill operation should be prohibited
	//   because MTT table is in SDRAM and SDRAM is in the self-refresh mode.
	//  So, we will fill TLB before entering SDRAM self-refresh 
	//   instead of disabling MMU.
	rREFRESH;//To fill TLB for the special register used in EnterPWDN
	rCLKCON;
	//=================================================================

	

	EnterPWDN(0xffff8); //Sleep mode
	//Never return here. 
}

int batt_flag=0;
//================================================================================
static void __irq BattFaultInt(void)
{
	rINTMSK |= (BIT_BAT_FLT);	// mask.
	ClearPending(BIT_BAT_FLT)

	Uart_Printf("BATT_FLT interrupt occured\n");
	batt_flag=1;
	//Batt_Sleep();

}

//================================================================================
void Test_BattFaultInterrupt(void)
{
	int i;
	
	Uart_Printf("Battery fault interrupt test.\n");
	Uart_Printf("Detect low-level.\n");
/*
	Uart_Printf("0 : Reset system during nBATT_FLT=0.\n");
	Uart_Printf("4 : System will be wake-up from sleep mode during nBATT_FLT=0.\n");
	Uart_Printf("6 : System will ignore Eint0 signal during nBATT_FLT=0.\n");
	Uart_Printf("7 : Disable Battery function.\n");
	Uart_Printf("Select battery function : ");
	//i = Uart_GetIntNum() & 0x7;
*/
	i = 4;
	
	rMISCCR = (rMISCCR & ~(0x1f<<20)) | (i<<20);
	// [22:20]
	// if 0, System will be Reset during nBATT_FLT=0.
	// if 4, System will be wake-up from sleep mode during nBATT_FLT=0.
	// if 6, System will ignore nBATT_FLT signal.
	// if 7, Disable Battery function.

	pISR_BAT_FLT = (U32)BattFaultInt;
	rSRCPND = BIT_BAT_FLT;
	rINTPND = BIT_BAT_FLT;

	rINTMSK = rINTMSK & ~(BIT_BAT_FLT);	// unmask.

		while(batt_flag==0)
		{
			if(Uart_GetKey()){
				break; 
			}
		}	
		batt_flag=0;
		//Batt_Sleep();
		Uart_Printf("exit.\n");
		
}

void Batt_Sleep(void)
{
	int i;
//	U32 portStatus[33+3];
//	int error=0;
//	int mode;
	Uart_Printf("[Sleep Mode Test]\n");

	Uart_Printf("nBATT_FLT pin should be tested.\n");
	
	Uart_Printf("S3C2440 will wake up by EINT0 .\n");

/////////////////////////////////////////// add for battery fault...
#if 1
	Uart_Printf("6 : System will ignore Eint0 signal during nBATT_FLT=0.\n");

	Uart_Printf("rMISCCR:%x\n", rMISCCR);

#endif	
/////////////////////////////////////////// add for battery fault...end

	Uart_TxEmpty(1);	//Wait until UART0 Tx buffer empty.
	   

#if 0	  
	SelectEintType();
#else
	rGPFCON=rGPFCON & ~(3<<0)|(2<<0); //PF0=EINT0
	rEXTINT0=rEXTINT0&~(7<<0)|(2<<0);  //EINT0=falling edge triggered
#endif

#if 1	//test whether or not the interrupt pending bit retain the wake-up source.
	pISR_EINT0=(U32)Eint0Int;
	pISR_EINT8_23=(U32)Eint8_23Int;
	pISR_RTC=(U32)AlarmInt;
	rINTMSK = ~(BIT_EINT0|BIT_EINT8_23|BIT_RTC);
	for(i=0;i<100;i++);

	rSRCPND = BIT_EINT0|BIT_EINT8_23|BIT_RTC;
	rINTPND = BIT_EINT0|BIT_EINT8_23|BIT_RTC;

	rEINTPEND=rEINTPEND;
	rSRCPND=rSRCPND;
	rINTPND=rINTPND;
#endif	 

	rEINTMASK = rEINTMASK&~(1<<11); //SRCPND:EINT8_23 will be set by EINT11 after wake-up.
	rINTMSK = BIT_ALLMSK;
	// NOTE: Any interrupt can't be used in Sleep mode. 
	//	   because SDRAM is in self-refresh mode and ISR code will access SDRAM.

	Uart_Printf("Alarm wake-up (y/n)? ");
	if(Uart_Getch()=='y')
	{
		Uart_Printf("S3C2440 will wake up by RTC alarm(10 sec) or EINT0 .\n");
		SetAlarmWakeUp();
	}

	rRTCCON=0x0;   // R/W disable, 1/32768, Normal(merge), No reset
	rADCCON|=(1<<2);

	rMISCCR|=(1<<12); //USB port0 = suspend
	rMISCCR|=(1<<13); //USB port1 = suspend

	rGSTATUS3=(U32)StartPointAfterSleepWakeUp;	// memory control part in startup code.
	Uart_Printf("set restart address:%x\n", rGSTATUS3);
	rGSTATUS4=0xaaaaaaaa;

	Uart_TxEmpty(1);	//Wait until UART0 Tx buffer empty.
	ConfigSleepGPIO();
	ConfigMiscIO();

	rMISCCR=rMISCCR|(3<<0); 	// Data line pull-up disable.

	Lcd_EnvidOnOff(0); //Before entering Sleep mode, LCD must be off	

	rMISCCR = (rMISCCR & ~(0x7<<20)) | (6<<20);	// set 6.

	//=================================================================
	//  VERY IMPORTANT NOTE 
	//  To enter SLEEP/SLIDLE mode, MMU TLB-fill operation should be prohibited
	//   because MTT table is in SDRAM and SDRAM is in the self-refresh mode.
	//  So, we will fill TLB before entering SDRAM self-refresh 
	//   instead of disabling MMU.
	rREFRESH;//To fill TLB for the special register used in EnterPWDN
	rCLKCON;
	//=================================================================


	EnterPWDN(0xffff8); //Sleep mode
}


/************************ Sleep Mode by 100hz test ************************/
//================================================================================
void Check_SleepWakeUp_100Hz(void)
{
}

//================================================================================

void Test_SleepMode_100Hz(void)
{
}


void Test_StopMode(void)
{
	int i;
	U32 portStatus[33+3];	// +3 is for 2440 GPJ port.
//	int error=0;
//	int mode;
	Uart_Printf("[STOP Mode Test]\n");

	Uart_Printf("S3C2440 will wake up by EINT0 .\n");

	Uart_TxEmpty(1);	//Wait until UART0 Tx buffer empty.

	rMISCCR = (rMISCCR & ~(7<<8) & ~(7<<4)) | (3<<8) | (2<<4);	// CLKSEL0=FCLK, CLKSEL1=HCLK
// Save the port configurations
	for(i=0;i<33;i++) {
		portStatus[i]=*( (volatile U32 *)0x56000000 + i);	// 0x5600_0000:GPACON addr.
	}
	for(i=0;i<3;i++) 
		portStatus[i+33]=*( (volatile U32 *)0x560000d0 + i);	// 0x5600_00d0:GPJCON addr. added ffor 2440

	ConfigSleepGPIO();
	ConfigMiscIO();
   
	//Uart_Printf("S3C2440 will wake up by EINT0 (Falling Edge Triggered).\n");
	rGPFCON=rGPFCON & ~(3<<0)|(2<<0);	//GPF0=EINT0
	rEXTINT0=rEXTINT0&~(7<<0)|(0x2<<0);  //EINT0=falling edge triggered

	//Uart_Printf("S3C2440 will wake up by EINT11 (Falling Edge Triggered).\n");
	rGPGCON=rGPGCON & ~(3<<6)|(2<<6);	//GPG3=EINT11
	rEXTINT1=rEXTINT1&~(7<<12)|(2<<12);  //EINT11=falling edge triggered

	//Uart_Printf("\nNow, I am entering Stop mode.\n");
	Uart_TxEmpty(1);	//Wait until UART0 Tx buffer empty.
	
	pISR_EINT0	= (U32)Eint0Int;
	pISR_EINT8_23 = (U32)Eint8_23Int;
	pISR_RTC	  = (U32)AlarmInt;

	rEINTPEND = (1<<11); // clear EINT11 pending bit in rEINTPEND
	rSRCPND = BIT_EINT0 | BIT_RTC | BIT_EINT8_23;
	rINTPND = BIT_EINT0 | BIT_RTC | BIT_EINT8_23;

	rINTMSK = BIT_ALLMSK;
	//rINTMSK = ~BIT_EINT0;
	// NOTE: Any interrupt can't be used in Sleep mode 
	//	   because SDRAM is in self-refresh mode and ISR code will access SDRAM.


	rRTCCON=0x0;   // R/W disable, 1/32768, Normal(merge), No reset
	rADCCON|=(1<<2);	// ADC stanby.

	rMISCCR|=(1<<12); //USB port0 = suspend
	rMISCCR|=(1<<13); //USB port1 = suspend

	//rMISCCR|=(1<<2); //Previous state at Sleep mode. 
	rMISCCR &= ~(1<<2); //Previous state at STOP(?) mode (???)
					 
	//For SDRAM self-refresh mode, rMISCCR[2] should be 1.

	rMISCCR=rMISCCR|(3<<0); 

	Lcd_EnvidOnOff(0); //Before entering Sleep mode, LCD must be off	
	Delay(1);

	Led_Display(0x5);
	//rCLKSLOW=0x34;  //csh (???)debug
	
	//=================================================================
	//  VERY IMPORTANT NOTE 
	//  To enter STOP/SLIDLE mode, MMU TLB-fill operation should be prohibited
	//   because MTT table is in SDRAM and SDRAM is in the self-refresh mode.
	//  So, we will fill TLB before entering SDRAM self-refresh 
	//   instead of disabling MMU.
	rREFRESH;	//To fill TLB for the special register used in EnterPWDN
	rCLKCON;
	rMISCCR;
	rGPFDAT;
	//=================================================================
	rGPGDAT=rGPGDAT&~(1<<11)|(0<<11);	//GPG11(EINT19)=L

	EnterPWDN(0xffff1); //Sleep mode added for 2440 bit[19] camera.

	rCLKCON=0xffff0;	//Clear STOP_BIT & IDLE_BIT just after exiting Sleep mode.

	//rCLKSLOW=0x4; //csh (???) debug
	Uart_Printf("wake-up\n");

	// Restore I/O drive strength 
	rDSC0 = ((unsigned int)1<<31)|(0<<8)|(0<<0);
	rDSC1 = (0<<28)|(0<<26)|(0<24)|(0<<22)|(0<<20)|(0<<18);

	//Restore the port configurations
	for(i=0;i<33;i++){
		*( (volatile U32 *)0x56000000 + i)=portStatus[i];
	}
	for(i=0;i<3;i++) 	// GPJ port
		*( (volatile U32 *)0x560000d0 + i)=portStatus[i+33];

	Led_Display(0xa);
	Lcd_EnvidOnOff(1); 
	
	Uart_Printf("SRCPND=%x,INTPND=%x\n",rSRCPND,rINTPND);
	rINTMSK =~(BIT_EINT0|BIT_RTC|BIT_EINT8_23);
	for(i=0;i<10;i++); 

	//The Wake-up source is indicated by rSRCPND.
	//The corresponding interrupt wiil be occurred here.

	rINTMSK =BIT_ALLMSK;
	Uart_Printf("Return to Normal Mode.\n");	
}
