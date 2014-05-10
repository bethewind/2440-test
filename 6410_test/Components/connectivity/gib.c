/*-------------------------------------------------------------------
 * File Name : gps.c
 * Contents  : CGX5500 test functions 
 *
 * History: 
 * 	1) 2007,JAN,23th	Original, CellGuide
 * 	2) 2007,JAN,23th	CellGuide - added TCXO test, reset and snap test
 *
 * Notes:
 *   1. Don't remove any author name & revision history
 *   2. 6410 - used DMA Channel 1
 *
 * Copyright (c) 2007 CellGuide LTD.
 *--------------------------------------------------------------------
 */

#include "system.h"
#include "dma.h"
#include "gib.h"
#include "gpio.h"

#ifdef FPGA_6410
#include "syscon.h"
#define	GPADAT			(GPIO_A_BASE+0x04)
#endif


#ifdef Silicon_6410
#include "sfr6410.h"
#include "intc.h"
#include "gpio.h"
#include "library.h"
#define	GPPDAT			(0x7F008164)
#define	GPPCON_GIB	(0x7F008174)
#endif


#define	TCXO_TEST_COUNT	1000
char *snapBuffer=(char *)0x52000000;

volatile u32 uAutoErrCnt = 0;

static unsigned int gbOccurTCInt = false ; //Transfer Complete
static DMAC oGIBDmac1;

typedef enum {
	CGX5500_REG_READ_ONLY, CGX5500_REG_WRITE_ONLY, CGX5500_REG_READ_WRITE, CGX5500_REG_SPECIAL
} TCgRegMode;


typedef struct {
	const char *mnemonic;
	u32 offset;
	u32 defaultValue;
	u32 readMask;
	u32 writeMask;
	TCgRegMode mode;
} TCgRegDef;


TCgRegDef regmap[] = {
	// Mnemonic		  offset		default	read mask	write mask		mode  
	{ "CONLSB",			0x0000, 0x20200A01, 0x3EF00F8F, 0x3EF00F8F, CGX5500_REG_READ_WRITE},
	/*{ "CONLSB",			0x0000, 0xffffffff, 0x3EF00F8F, 0x3EF00F8F, CGX5500_REG_READ_WRITE},  */
	{ "CONMSB",			0x0004, 0x0000000B, 0x0000F0FF, 0x0000F0FF, CGX5500_REG_SPECIAL},
	{ "NCOSTEP",			0x0008, 0x00ECE605 ,0x00FFFFFF, 0x00FFFFFF, CGX5500_REG_READ_WRITE}, 
	{ "WINSEL",			0x000c, 0x00000F14, 0x00001F1F, 0x00001F1F, CGX5500_REG_READ_WRITE},  
	{ "INPTHRESLSB",		0x0010, 0xC0006000, 0xFFFFFFFF, 0xFFFFFFFF, CGX5500_REG_READ_WRITE}, 
	{ "INPTHRESMSB",		0x0014, 0x000007FF, 0x00001FFF, 0x00001FFF, CGX5500_REG_READ_WRITE}, 
	{ "QUADTHRESLSB",	0x0018, 0xC0006000, 0xFFFFFFFF, 0xFFFFFFFF, CGX5500_REG_READ_WRITE}, 
	{ "QUADTHRESMSB",	0x001c, 0x000007FF, 0x00001FFF, 0x00001FFF, CGX5500_REG_READ_WRITE}, 
	{ "CYCLEINMS",		0x0020, 0X07BFA641, 0x0FFFFFFF, 0x0FFFFFFF, CGX5500_REG_READ_WRITE}, 
	{ "TIMERSTARTLSB",  	0x0024, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, CGX5500_REG_READ_WRITE}, 	
	{ "TIMERSTARTMSB",  	0x0028, 0x00000400, 0x00001FFF, 0x00001FFF, CGX5500_REG_READ_WRITE}, 
	{ "GATEWIDTH",		0x002c, 0x00000000, 0x0FFFFFFF, 0x0FFFFFFF, CGX5500_REG_READ_WRITE}, 
	{ "SNAPBUFDATALSB", 	0x0030, 0x0F008000, 0xFFFFFFFF, 0xFFFFFFFF, CGX5500_REG_READ_WRITE}, 
	{ "SNAPBUFDATAMSB", 	0x0034, 0x00000000, 0x00000003, 0x00000003, CGX5500_REG_READ_WRITE}, 
	{ "CAPTTIMERLSB",	0x0038, 0x00000000, 0x0FFFFFFF, 0x00000000, CGX5500_REG_READ_ONLY},  
	{ "CAPTTIMERMSB",	0x003c, 0x00000000, 0x00001FFF, 0x00001FFF, CGX5500_REG_SPECIAL},  
	{ "RFINITGPO",		0x0040, 0x00000000, 0x00000007, 0x00000007, CGX5500_REG_READ_WRITE}, 
	{ "VER"   ,			0x0044, 0x02120106, 0xFFFFFFFF, 0x00000000, CGX5500_REG_READ_ONLY},  
	{ "SYNCCOUNTLSB",	0x0048, 0x00000000, 0xFFFFFFFF, 0x00000000, CGX5500_REG_READ_ONLY},  
	{ "SYNCCOUNTMSB",	0x004c, 0x00000000, 0x000000FF, 0x00000000, CGX5500_REG_READ_ONLY},  
	{ "NORMCORLSB",		0x0050, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, CGX5500_REG_READ_WRITE}, 
	{ "NORMCORMSB",		0x0054, 0x00000000, 0x0000FFFF, 0x0000FFFF, CGX5500_REG_READ_WRITE}, 
	/*{ "RTCTIMERVAL",		0x005c, 0x00000000, 0xFFFFFFFF, 0x00000000, CGX5500_REG_READ_ONLY},*/
	{ "BSTRACKINFO",		0x0060, 0x00000000, 0x000FFFFF, 0x000FFFFF, CGX5500_REG_READ_WRITE}, 
	{ "BSTRACKINTCNT",  	0x0064, 0x80000000, 0xFFFFFFFF, 0x00000000, CGX5500_REG_READ_ONLY}, 	
	{ "BSTRACKRESID",	0x0068, 0x00000000, 0xFFFFFFFF ,0x00000000, CGX5500_REG_READ_ONLY}, 
	{ "SNAPDATA",		0x0070, 0x00000000, 0xFFFFFFFF, 0x00000000, CGX5500_REG_SPECIAL}, 
	{ "RAWINIT",			0x0080, 0x00000000, 0x0000001F, 0x00000000, CGX5500_REG_READ_ONLY}, 
	{ "INTSRC",			0x0084, 0x00000000, 0x0000001F, 0x00000000, CGX5500_REG_READ_ONLY}, 
	{ "INTEN",			0x0088, 0x00000000, 0x0000001F, 0x0000001F, CGX5500_REG_READ_WRITE}, 
	{ "INTCLR",			0x008c, 0x00000000, 0x00000000, 0x0000001F, CGX5500_REG_WRITE_ONLY}, 
	{ "SATREAD",			0x0090, 0x00000000, 0x800000FC, 0x800000FC, CGX5500_REG_READ_WRITE}, 
	{ "SATREADDATA",	0x0094, 0x00000000, 0xFFFFFFFF, 0x00000000, CGX5500_REG_READ_ONLY}, 
	{ "SAT0NCODATALSB", 	0x0100, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, CGX5500_REG_SPECIAL}, 
	{ "SAT1NCODATALSB", 	0x0104, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, CGX5500_REG_SPECIAL}, 
	{ "SAT2NCODATALSB", 	0x0108, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, CGX5500_REG_SPECIAL}, 
	{ "SAT3NCODATALSB", 	0x010c, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, CGX5500_REG_SPECIAL}, 
	{ "SAT4NCODATALSB", 	0x0110, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, CGX5500_REG_SPECIAL}, 
	{ "SAT5NCODATALSB", 	0x0114, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, CGX5500_REG_SPECIAL}, 
	{ "SAT6NCODATALSB", 	0x0118, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, CGX5500_REG_SPECIAL}, 
	{ "SAT7NCODATALSB", 	0x011c, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, CGX5500_REG_SPECIAL}, 
	{ "SAT8NCODATALSB", 	0x0120, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, CGX5500_REG_SPECIAL}, 
	{ "SAT9NCODATALSB", 	0x0124, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, CGX5500_REG_SPECIAL}, 
	{ "SAT10NCODATALSB",0x0128, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, CGX5500_REG_SPECIAL}, 
	{ "SAT11NCODATALSB",0x012c, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, CGX5500_REG_SPECIAL}, 
	{ "SAT12NCODATALSB",0x0130, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, CGX5500_REG_SPECIAL}, 
	{ "SAT13NCODATALSB",0x0134, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, CGX5500_REG_SPECIAL}, 
	{ "SAT14NCODATALSB",0x0138, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, CGX5500_REG_SPECIAL}, 
	{ "SAT15NCODATALSB",0x013c, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, CGX5500_REG_SPECIAL}, 
	{ "SAT0NCODATAMSB", 0x0140, 0x00000000, 0x0000FFFF, 0x0000FFFF, CGX5500_REG_SPECIAL}, 
	{ "SAT1NCODATAMSB", 0x0144, 0x00000000, 0x0000FFFF, 0x0000FFFF, CGX5500_REG_SPECIAL}, 
	{ "SAT2NCODATAMSB", 0x0148, 0x00000000, 0x0000FFFF, 0x0000FFFF, CGX5500_REG_SPECIAL}, 
	{ "SAT3NCODATAMSB", 0x014c, 0x00000000, 0x0000FFFF, 0x0000FFFF, CGX5500_REG_SPECIAL}, 
	{ "SAT4NCODATAMSB", 0x0150, 0x00000000, 0x0000FFFF, 0x0000FFFF, CGX5500_REG_SPECIAL}, 
	{ "SAT5NCODATAMSB", 0x0154, 0x00000000, 0x0000FFFF, 0x0000FFFF, CGX5500_REG_SPECIAL}, 
	{ "SAT6NCODATAMSB", 0x0158, 0x00000000, 0x0000FFFF, 0x0000FFFF, CGX5500_REG_SPECIAL}, 
	{ "SAT7NCODATAMSB", 0x015c, 0x00000000, 0x0000FFFF, 0x0000FFFF, CGX5500_REG_SPECIAL}, 
	{ "SAT8NCODATAMSB", 0x0160, 0x00000000, 0x0000FFFF, 0x0000FFFF, CGX5500_REG_SPECIAL}, 
	{ "SAT9NCODATAMSB", 0x0164, 0x00000000, 0x0000FFFF, 0x0000FFFF, CGX5500_REG_SPECIAL}, 
	{ "SAT10NCODATAMSB",0x0168, 0x00000000, 0x0000FFFF, 0x0000FFFF, CGX5500_REG_SPECIAL}, 
	{ "SAT11NCODATAMSB",0x016c, 0x00000000, 0x0000FFFF, 0x0000FFFF, CGX5500_REG_SPECIAL}, 
	{ "SAT12NCODATAMSB",0x0170, 0x00000000, 0x0000FFFF, 0x0000FFFF, CGX5500_REG_SPECIAL}, 
	{ "SAT13NCODATAMSB",0x0174, 0x00000000, 0x0000FFFF, 0x0000FFFF, CGX5500_REG_SPECIAL}, 
	{ "SAT14NCODATAMSB",0x0178, 0x00000000, 0x0000FFFF, 0x0000FFFF, CGX5500_REG_SPECIAL}, 
	{ "SAT15NCODATAMSB",0x017c, 0x00000000, 0x0000FFFF, 0x0000FFFF, CGX5500_REG_SPECIAL}, 
	{ "SAT0GOLDDATA",	0x0180, 0x00000000, 0x001FFFFF, 0x001FFFFF, CGX5500_REG_SPECIAL}, 
	{ "SAT1GOLDDATA",	0x0184, 0x00000000, 0x001FFFFF, 0x001FFFFF, CGX5500_REG_SPECIAL}, 
	{ "SAT2GOLDDATA",	0x0188, 0x00000000, 0x001FFFFF, 0x001FFFFF, CGX5500_REG_SPECIAL}, 
	{ "SAT3GOLDDATA",	0x018c, 0x00000000, 0x001FFFFF, 0x001FFFFF, CGX5500_REG_SPECIAL}, 
	{ "SAT4GOLDDATA",	0x0190, 0x00000000, 0x001FFFFF, 0x001FFFFF, CGX5500_REG_SPECIAL}, 
	{ "SAT5GOLDDATA",	0x0194, 0x00000000, 0x001FFFFF, 0x001FFFFF, CGX5500_REG_SPECIAL}, 
	{ "SAT6GOLDDATA",	0x0198, 0x00000000, 0x001FFFFF, 0x001FFFFF, CGX5500_REG_SPECIAL}, 
	{ "SAT7GOLDDATA",	0x019c, 0x00000000, 0x001FFFFF, 0x001FFFFF, CGX5500_REG_SPECIAL}, 
	{ "SAT8GOLDDATA",	0x01a0, 0x00000000, 0x001FFFFF, 0x001FFFFF, CGX5500_REG_SPECIAL}, 
	{ "SAT9GOLDDATA",	0x01a4, 0x00000000, 0x001FFFFF, 0x001FFFFF, CGX5500_REG_SPECIAL}, 
	{ "SAT10GOLDDATA",	0x01a8, 0x00000000, 0x001FFFFF, 0x001FFFFF, CGX5500_REG_SPECIAL}, 
	{ "SAT11GOLDDATA",	0x01ac, 0x00000000, 0x001FFFFF, 0x001FFFFF, CGX5500_REG_SPECIAL}, 
	{ "SAT12GOLDDATA",	0x01b0, 0x00000000, 0x001FFFFF, 0x001FFFFF, CGX5500_REG_SPECIAL}, 
	{ "SAT13GOLDDATA",	0x01b4, 0x00000000, 0x001FFFFF, 0x001FFFFF, CGX5500_REG_SPECIAL}, 
	{ "SAT14GOLDDATA",	0x01b8, 0x00000000, 0x001FFFFF, 0x001FFFFF, CGX5500_REG_SPECIAL}, 
	{ "SAT15GOLDDATA",	0x01bc, 0x00000000, 0x001FFFFF, 0x001FFFFF, CGX5500_REG_SPECIAL}, 
	{ "SAT0MIGDIR",		0x01c0, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, CGX5500_REG_SPECIAL}, 
	{ "SAT1MIGDIR",		0x01c4, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, CGX5500_REG_SPECIAL}, 
	{ "SAT2MIGDIR",		0x01c8, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, CGX5500_REG_SPECIAL}, 
	{ "SAT3MIGDIR",		0x01cc, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, CGX5500_REG_SPECIAL}, 
	{ "SAT4MIGDIR",		0x01d0, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, CGX5500_REG_SPECIAL}, 
	{ "SAT5MIGDIR",		0x01d4, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, CGX5500_REG_SPECIAL}, 
	{ "SAT6MIGDIR",		0x01d8, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, CGX5500_REG_SPECIAL}, 
	{ "SAT7MIGDIR",		0x01dc, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, CGX5500_REG_SPECIAL}, 
	{ "SAT8MIGDIR",		0x01e0, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, CGX5500_REG_SPECIAL}, 
	{ "SAT9MIGDIR",		0x01e4, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, CGX5500_REG_SPECIAL}, 
	{ "SAT10MIGDIR",		0x01e8, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, CGX5500_REG_SPECIAL}, 
	{ "SAT11MIGDIR",		0x01ec, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, CGX5500_REG_SPECIAL}, 
	{ "SAT12MIGDIR",		0x01f0, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, CGX5500_REG_SPECIAL}, 
	{ "SAT13MIGDIR",		0x01f4, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, CGX5500_REG_SPECIAL}, 
	{ "SAT14MIGDIR",		0x01f8, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, CGX5500_REG_SPECIAL}, 
	{ "SAT15MIGDIR",		0x01fc, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, CGX5500_REG_SPECIAL}, 
	{ NULL }
	};


typedef struct {
	const char *mnemonic;
	u32 value;
} TCgRefPattern;


TCgRefPattern regpatterns[] = {
	{"all one", 0xFFFFFFFF},
	{"all zero", 0},
	{"AA", 0xAAAAAAAA},
	{"55", 0x55555555},
	{ NULL }
	};


void __irq Isr_GIBDmaDone(void)
{
#ifdef FPGA_6410
	INTC_Mask(INT_DMA1);
	INTC_ClearPending(INT_DMA1);

	DMAC_ClearIntPending(&oGIBDmac1);		
	gbOccurTCInt = true;

	INTC_Unmask(INT_DMA1);
	Disp("\t 6410 !!! \n\n");
	Disp("\t Occur Terminal Count Interrupt!!! \n\n");
#endif

#ifdef Silicon_6410
	DMACH_ClearIntPending(&oGIBDmac1);		
	gbOccurTCInt = true;

	Disp("\t 6410 !!! \n\n");
	Disp("\t Occur Terminal Count Interrupt!!! \n\n");
	INTC_ClearVectAddr();
#endif
}



//call at "GIB_test.c" only
// [1]_1
void GIB_TestCoreReset(void)
{
	u32 i;
	GIB_CoreReset();
	for(i = 0; (regmap[i].mnemonic != NULL); i++) 
	{
		if((regmap[i].writeMask != 0) && (regmap[i].readMask != 0) && (regmap[i].mode!= CGX5500_REG_SPECIAL))
			GIB_regWrite(i, 0xFFFFFFFF & regmap[i].writeMask);
 	}	
	GIB_CoreReset();
	GIB_TestRegistersDefaultValue();	
}


// call at "GIB_test.c", "GIB_TestCoreReset()"
// [2]_1
void GIB_TestRegistersDefaultValue(void)
{
	u32 i;

	GIB_CoreReset();
	GIB_TimerReset();
		
	Disp("\n\t -> Start Registers \"Default Values\" Test.");
	// Test register accessibility
	for(i = 0; (regmap[i].mnemonic != NULL); i++) 
	{
		if((regmap[i].readMask != 0)  && (regmap[i].mode != CGX5500_REG_SPECIAL))
			GIB_regRead(i, regmap[i].defaultValue & regmap[i].readMask);// read default value		
 	}	
	Disp("\n\t -> End Registers \"Default Values\" Test.\n");
}



// call at "GIB_test.c" only
// [2]_2
void GIB_TestRegistersWrite(void)
{
	u32  i, pattern;

	Disp("\n\t -> Start Registers \"Write\" Test.");

	for(pattern = 0; regpatterns[pattern].mnemonic != NULL; pattern++)
	{
		for(i = 0; (regmap[i].mnemonic != NULL) ; i++) 
		{
			if((regmap[i].writeMask != 0) && (regmap[i].readMask != 0) && (regmap[i].mode != CGX5500_REG_SPECIAL))
			{
				GIB_regWrite(i, regpatterns[pattern].value); 
			}
 		}	
	}
	Disp("\n\t -> End Registers \"Write\" Test.");
}


void GIB_regWrite(int aIndex, u32 aValue)
{
	REG32(GIB_BASE + regmap[aIndex].offset) = aValue & regmap[aIndex].writeMask; //write
	GIB_regRead(aIndex, aValue & regmap[aIndex].readMask); //(after write) compare to read
}


//read and compare!!
void GIB_regRead(int aIndex, u32 aExpected)
{
	volatile u32 reg;

	reg = REG32(GIB_BASE + regmap[aIndex].offset); //Register value save to 'reg'
	reg &= regmap[aIndex].readMask;
	if (reg != aExpected)
		Disp("\n[!] Invalid Value [#%02dth] \"%s\" [0x%04X]\t[got 0x%08X <--> expected 0x%08X]\n",
					aIndex,regmap[aIndex].mnemonic,regmap[aIndex].offset, reg, aExpected);
}








// TCXO
// call at "GIB_test.c" only
// [1]_2
void GIB_TestTcxoReset(void)
{
	u32 lo, lo1;
	u32 hi, hi1;
	
	GIB_CoreReset(); 

	GIB_TimerReset(); 
	REG32(GIB_BASE + 0x004) = 0x00000050;
	Delay(10);
	REG32(GIB_BASE + 0x003C) = 0;  
	lo = REG32(GIB_BASE + 0x0038); 
	hi = REG32(GIB_BASE + 0x003C); 

	GIB_TimerReset(); 
	REG32(GIB_BASE + 0x004) = 0x00000050;
	Delay(5);
	REG32(GIB_BASE + 0x003C) = 0;  
	lo1 = REG32(GIB_BASE + 0x0038); 
	hi1 = REG32(GIB_BASE + 0x003C); 

	Disp("Tcxo Reset [%s] !!!\n",(lo > lo1) ? "OK" : "NG");
	Disp("\t\t  Value of \'GIB_BASE+0x0038\' before TimerReset  [0x%08X]\n",lo ,GIB_BASE+0x0038);
	Disp("\t\t  Value of \'GIB_BASE+0x0038\' after  TimerReset  [0x%08X]\n",lo1,GIB_BASE+0x0038);
} 



// TCXO
// call at "GIB_test.c"
// [3]_
void GIB_TestTcxoCounter(void)
{
	u32 lo, hi;
	u32 lo1, hi1;
	u32 startValLow, endValLow;
	int i;
	long dif;
			
	Disp("Start Testing TCXO Counter\n");
	
	GIB_CoreReset();
	GIB_TimerReset();
	
 	// check TCXO counter
	REG32(GIB_BASE + 0x0004) = ((1<<4) | (1<<6));  // Enable TCXO Counter 
	
	Delay(10);
	REG32(GIB_BASE + 0x0004) = ((0<<4) | (0<<6));  // Diable TCXO Counter
	REG32(GIB_BASE + 0x003C) = 0;  // Capture TCXO Counter
	 
	lo = REG32(GIB_BASE + 0x0038); // Read Low Value (32-bit)		
	hi = REG32(GIB_BASE + 0x003C); // Read High Value (18 bit)
	startValLow = lo; // first reg val


	REG32(GIB_BASE + 0x0004) = ((1<<4) | (1<<6));  // Enable TCXO Counter 

	for(i = 0; i < TCXO_TEST_COUNT; i++)
	{
	}
	REG32(GIB_BASE + 0x0004) = ((0<<4) | (0<<6));  // Diable TCXO Counter 
	
	REG32(GIB_BASE + 0x003C) = 0;  // Capture TCXO Counter
	lo = REG32(GIB_BASE + 0x0038); // Read Low Value (32-bit)		
	hi = REG32(GIB_BASE + 0x003C); // Read High Value (18 bit)
	endValLow = lo; // last reg val

	Disp("Start Test : [%d], End Test [%d], Cycles [%d] diff [%d]\n", startValLow, endValLow, TCXO_TEST_COUNT, (endValLow - startValLow));
	Disp("End Testing TCXO Counter\n");
}



// call at "GIB_test.c"
// [7]_
void GIB_TestSnap1(u32 aLengthMs, u32 aTestMode)
{	
	unsigned long lo, hi;
	unsigned long i, j, intReg;
	unsigned long lengthBytes = aLengthMs *1024 / 4;

	GIB_CoreReset();
	GIB_TimerReset();

	//Normal DMA selection from SYSCON -> Joon , Later SYSCON API need to be used
#ifdef FPGA_6410
	SCON_EnableSdmaInput(FALSE);
	//Disp("\n Interrupt Mode..\n");
	//DMAC_ClearIntPending(&oGIBDmac1);	
	//INTC_Init();
	//INTC_SetVectAddr(INT_DMA1, Isr_GIBDmaDone);
//error
	//INTC_Enable(INT_DMA1);
#endif

#ifdef Silicon_6410
	Disp("\n Interrupt Mode..\n");
	//INTC_Init()
	INTC_SetHandler(NUM_DMA1, Isr_GIBDmaDone);
	INTC_Enable(NUM_DMA1);
#endif

	Disp("Request [%d] Bytes\n", lengthBytes);

	//Clear Interrupt 
	REG32(GIB_BASE + 0x008C) = 0xFFFFFFFF; 
	intReg  = REG32(GIB_BASE + 0x0080);// Read IntErrorupt Status
	if (intReg) Disp("Error! Interrupt not Cleared [0x%08X]\n", intReg);

	// Clear snap buffer
	memset(snapBuffer, 0, lengthBytes*4);

	// setup CGX5500 for snap
	//REG32(GIB_BASE + 0x0008) = 0x000004ed; // mainNCOStep for test mode real input = 1 
	REG32(GIB_BASE + 0x0008) = 0x00C00502; // mainNCOStep for freq 16368000 drift 0.794
	//REG32(GIB_BASE + 0x0030) = 0x0F000000 + (lengthBytes / 16); 
	REG32(GIB_BASE + 0x0030) = 0x04000000 + (lengthBytes / 4); 
	REG32(GIB_BASE + 0x0034) = 0x00000000;
	REG32(GIB_BASE + 0x0020) = 0x09C18F00;
	REG32(GIB_BASE + 0x0000) = 0x20200203;//DMA Request: Low	
	//REG32(GIB_BASE + 0x0000) = 0x20200283;//DMA Request: High
	// set 'testmode' or 'reamode'
	//REG32(GIB_BASE + 0x0004) = 0x0000807A + (aTestMode ? 0x2000 : 0x0000);		//1010 0000 0111 1010
	REG32(GIB_BASE + 0x0004) = 0x0000807A + (1 ? 0x2000 : 0x0000);		//1010 0000 0111 1010
	// read current TCXO count
	REG32(GIB_BASE + 0x003c) = 0x00000000;
	lo = REG32(GIB_BASE + 0x0038); 
	hi = REG32(GIB_BASE + 0x003C); 
	// setup snap to begin in 1 second
	REG32(GIB_BASE + 0x0024) = lo + 16368000 * SNAP_DELAY_SEC;
	REG32(GIB_BASE + 0x0028) = 0x00000000;
	
	//REG32(GIB_BASE + 0x0004) = 0x0000807A + (aTestMode ? 0x2000 : 0x0000);
	REG32(GIB_BASE + 0x0004) = 0x0000807A + (1 ? 0x2000 : 0x0000);



#ifdef FPGA_6410
	DMAC_InitCh(DMA1, DMA_A, &oGIBDmac1); 	// prepare DMA for transfer
	DMAC_Start(
		GIB_BASE + 0x70, // Source address : CGX5500
		1, // fixed
		(u32)snapBuffer, 
		0, // increment
		WORD, // size : WORD/HWORD
		lengthBytes/4, 
		HANDSHAKE, 
		DMA_GIB,  // Source is CGX5500, (6410: DMA1,DMA_GIB), (6430:DMA0,7¹ø)
		0, //MEM, 
		BURST4, 
		&oGIBDmac1);
#endif

#ifdef Silicon_6410
	DMAC_InitCh(DMA1, (DMA_CH)DMA_A, &oGIBDmac1);

	DMACH_Setup(
		(DMA_CH)DMA_A,
		0x0,
		(u32)(GIB_BASE + 0x70),
		0x01, //fix
		(u32)snapBuffer,
		0x00, //incre
		WORD,
		lengthBytes/4,
		DEMAND,
		DMA1_GIB,
		MEM,
		BURST4,
		&oGIBDmac1);
	DMACH_Start(&oGIBDmac1);	
#endif


#ifdef FPGA_6410
	while (!DMAC_IsTransferDone(&oGIBDmac1)); //polling mode
#endif

#ifdef Silicon_6410
	//while (!DMACH_IsTransferDone(&oGIBDmac1)); //polling mode
#endif


	//error
	while(!gbOccurTCInt); //Interrupt mode

	Disp("\n Interrupt Mode!! \n"); //Interrupt mode
	Disp("DMA transfer done. Press \"ENTER\" key...\n");
	UART_GetIntNum();


	// check Interrupt
	intReg  = REG32(GIB_BASE + 0x0080);	
	if((intReg & 0x00000002) == 0) 
		Disp("Error! No 'Start' Interrupt! [0x%08X]\n", intReg);
	if((intReg & 0x00000004) == 0) 
		Disp("Error! No 'End' Interrupt! [0x%08X]\n", intReg);

	//Clear Interrupt 
	REG32(GIB_BASE + 0x008C) = 0x00000002;
	intReg  = REG32(GIB_BASE + 0x0080);	
	if((intReg & 0x00000002) != 0) 
		Disp("Error! 'Start' Interrupt not Cleared! [0x%08X]\n", intReg);
	REG32(GIB_BASE + 0x008C) = 0x00000004;
	intReg  = REG32(GIB_BASE + 0x0080);	
	if((intReg & 0x00000004) != 0) 
		Disp("Error! 'End' Interrupt not Cleared! [0x%08X]\n", intReg);

	// print snap data
	Disp("Raw snap length in bytes = 0x[%08X]\n", lengthBytes);
	for(i = 0 ; i < lengthBytes ; i += (WORDS_IN_LINE * sizeof(int))){
		Disp("[%08X] ", i);
		for(j = 0 ; j < WORDS_IN_LINE * sizeof(int); j += sizeof(int))
			Disp("%08X ", GIB_ReversBits(*(unsigned long*)(snapBuffer + i + j)));
		Disp("\n");
		}
}




// call at "GIB_test.c"
// [5]_
void GIB_TestOverrunInterrupt(void)
{
	unsigned long lengthBytes = 128 *1024 / 4;
	volatile unsigned long intReg = 0;
	unsigned long lo, hi;

	GIB_CoreReset();
	GIB_TimerReset();

	// Clear snap buffer
	memset(snapBuffer, 0, lengthBytes);

	//Enable Overrun Interrupt
		// Interrupt Enable Reg
	REG32(GIB_BASE + 0x0088) |= 0x00000010;

	// setup CGX5500 for snap
		// NCO Step Reg
	REG32(GIB_BASE + 0x0008) = 0x000004ed;
		// Snap Size Reg
	REG32(GIB_BASE + 0x0030) = 0x0F000000 + (lengthBytes / 16); 
	REG32(GIB_BASE + 0x0034) = 0x00000000;
		// TCXO ticks in second * 10 Reg
	REG32(GIB_BASE + 0x0020) = 0x09C18F00;
		// Control LSB Reg
	REG32(GIB_BASE + 0x0000) = 0x20200203;
		// Control MSB Reg
	REG32(GIB_BASE + 0x0004) = 0x0000A07A;
		//TCXO counter capture MSB Reg
	REG32(GIB_BASE + 0x003c) = 0x00000000;
	lo = REG32(GIB_BASE + 0x0038); 
	hi = REG32(GIB_BASE + 0x003C); 	
		// Snap start count LSB Reg
	REG32(GIB_BASE + 0x0024) = lo + 16368000 * SNAP_DELAY_SEC;
		// Snap start count MSB Reg
	REG32(GIB_BASE + 0x0028) = 0x00000000;
		// Control MSB Reg
	REG32(GIB_BASE + 0x0004) = 0x0000A07A;
 
	Disp("Press \"ENTER\" key\n");

	UART_GetIntNum();		

		//Raw interrupts Reg
	intReg  = REG32(GIB_BASE + 0x0080);
	Disp("Interrupt Reg [0x0080] : 0x%08X || Overrun Interrupt Test %s\n", 
		intReg, ((intReg & 0x00000010) > 0) ? "OK" : "NG");

	//Clear Interrupt Overrun
	REG32(GIB_BASE + 0x008C) = 0x00000010;

		//Raw interrupts Reg
	intReg  = REG32(GIB_BASE + 0x0080);
	Disp("Interrupt Reg [0x0080] : 0x%08X || Overrun Interrupt Test %s\n", 
		intReg, ((intReg & 0x00000010) == 0) ? "OK" : "NG");

	GIB_CoreReset();
}



// FPGA or Silicon
// call at "GIB_test.c"
// [6]_1
void GIB_InitGpio(void)
{
	#ifdef FPGA_6410
		GPIO_SetFunctionEach(eGPIO_A, eGPIO_4, 0x1);
		GPIO_SetFunctionEach(eGPIO_A, eGPIO_5, 0x1);
		GPIO_SetFunctionEach(eGPIO_A, eGPIO_6, 0x1);
		GPIO_SetFunctionEach(eGPIO_A, eGPIO_7, 0x1);
	#endif

	#ifdef Silicon_6410
		GPIO_SetFunctionEach(eGPIO_P, eGPIO_8, 0x0);
		GPIO_SetFunctionEach(eGPIO_P, eGPIO_9, 0x0);
		GPIO_SetFunctionEach(eGPIO_P, eGPIO_10, 0x0);
	
		GPIO_SetFunctionEach(eGPIO_P, eGPIO_11, 0x1);
		GPIO_SetFunctionEach(eGPIO_P, eGPIO_12, 0x1);
		GPIO_SetFunctionEach(eGPIO_P, eGPIO_13, 0x1);
		GPIO_SetFunctionEach(eGPIO_P, eGPIO_14, 0x1);

		REG32(GPPCON_GIB) = 0x03;
			//GPPCON_GIB[1]=1=TcxoClk_CER = GPP[10] should receive small TcxoCLK sig.
			//GPPCON_GIB[0]=1=SEL_GIB = gpio setting.
	#endif
}




// FPGA or Silicon
// call at "GIB_test.c"
// [6]_2
void GIB_InitRf(void)
{
	int i,value,clk;
		u32 initData[4] = {0x00000202, 0x00010020, 0x00020002, 0x00030000}; // Don't modifiy about value	
	i=0;value=0;clk=0;
	
	#ifdef FPGA_6410
		for(i = 0; i < 4; i++)
		{
			value = initData[i];
			REG32(GPADAT) &= ~(0x1<<5); //Latch down 

			for(clk = 18; (--clk >= 0);)	
			{
				REG32(GPADAT) &= ~(0x1<<7); //CLK down 
				REG32(GPADAT) &= ~(0x1<<6);
				REG32(GPADAT) |= ((value & 0x1) <<6);			
				REG32(GPADAT) |= (0x1<<7); //CLK up				
				value >>= 1;			
			}			
			REG32(GPADAT) |= (0x1<<5); //Latch up 			
		}
	#endif

	#ifdef Silicon_6410
		for(i = 0; i < 4; i++)
		{
			value = initData[i];
			REG32(GPPDAT) &= ~(0x1<<12); //Latch down 

			for(clk = 18; (--clk >= 0);)	
			{
				REG32(GPPDAT) &= ~(0x1<<13); //CLK down 
				REG32(GPPDAT) &= ~(0x1<<14); //data_in
				REG32(GPPDAT) |= ((value & 0x1) <<14); //data_in			
				REG32(GPPDAT) |= (0x1<<13); //CLK up				
				value >>= 1;			
			}			
			REG32(GPPDAT) |= (0x1<<12); //Latch up 			
		}
	#endif
}



//used "SCON_Reset()" function in Syscon.c 
void GIB_CoreReset(void) 
{
#ifdef FPGA_6410
	SCON_Reset(RST_GIB_CORE);
#endif

#ifdef Silicon_6410
	u32 reg;
	reg=REG32(0x7E00F114); // GIBRST_BASE == (SYSCON_BASE + 0x114)
	reg|= (1<<16);			//bit [16] : Software Reset for GIB Core
	REG32(0x7E00F114) = reg; //Make LOW
	Delay(10);
    
 	reg&=~(1<<16);			
	REG32(0x7E00F114) = reg; //Make HIGH	
	Delay(10);
#endif
}




//used "SCON_Reset()" function in Syscon.c 
void GIB_TimerReset(void)
{
#ifdef FPGA_6410
	SCON_Reset(RST_GIB_TIMER);
#endif

#ifdef Silicon_6410
	u32 reg;
	reg=REG32(0x7E00F114); // GIBRST_BASE == (SYSCON_BASE + 0x114)	
	reg|=(1<<17);			   //bit [17] : Software Reset for GIB Timer
	REG32(0x7E00F114) = reg; //Make LOW
	Delay(10);

	reg&=~(1<<17);
	REG32(0x7E00F114) = reg; //Make HIGH
	Delay(10);
#endif	
}

unsigned long GIB_ReversBits(unsigned long SnapData)
{
	SnapData = (SnapData & 0x0000FFFF) << 16 | (SnapData >> 16) & 0x0000FFFF;
	return SnapData;
}


u32 GIB_AutoTestRegistersDefaultValue(void)
{
	u32 i;
	volatile u32 reg;

	GIB_CoreReset();
	GIB_TimerReset();
		
	// Test register accessibility
	for(i = 0; (regmap[i].mnemonic != NULL); i++) 
	{
		if((regmap[i].readMask != 0)  && (regmap[i].mode != CGX5500_REG_SPECIAL))
		{
			GIB_AutoregRead(i, regmap[i].defaultValue & regmap[i].readMask);// read default value	
		}	
 	}	
	Disp("\n-> End Registers \"Default Values\" Test.\n");

	return uAutoErrCnt;
}

void GIB_AutoregRead(int aIndex, u32 aExpected)
{
	volatile u32 reg;


	reg = REG32(GIB_BASE + regmap[aIndex].offset); //Register value save to 'reg'
	reg &= regmap[aIndex].readMask;
	if (reg != aExpected)
	{
		Disp("\nInvalid Value [#%02dth] \"%s\" [0x%04X]\n[got 0x%08X <--> expected 0x%08X]\n",
					aIndex,regmap[aIndex].mnemonic,regmap[aIndex].offset, reg, aExpected);
		uAutoErrCnt++;
	}	

//	return uAutoErrCnt;
}


