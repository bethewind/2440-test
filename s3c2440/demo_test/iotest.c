/*****************************************
  NAME: power.c
  DESC: SLOW,HOLD mode test
  HISTORY:
  03.23.2002:purnnamu: first release for 2410
  06.02.2002:DonGo: Modified for 2440
 *****************************************/
 
#include "def.h"
#include "option.h"
#include "2440addr.h"
#include "2440lib.h"
#include "2440slib.h" 

#include "iotest.h"

#include "power.h"

#include "memtest.h"


void Test_IO_Strength(void)
{

	int i;
	U32 dsc_data=0, dsc_adr=0, dsc_ctrl=0;
	
	Uart_Printf("Test2... I/O drive strength control test.\n");
	
	Uart_Printf("Current drive strength value(DISC0):%xh\n", rDSC0&0xff);
	
	Uart_Printf("DATA[31:0] dive strength(0:12,1:10,2:8,3:6[mA]) : ");
	i=Uart_GetIntNum();
	switch(i) {
		case 0: dsc_data = 0x00; break;
		case 1: dsc_data = 0x55; break;
		case 2: dsc_data = 0xaa; break;
		case 3: dsc_data = 0xff; break;
		default: dsc_data=0x00; break;
	}
	
	Uart_Printf("ADDR bus dive strength(0:10,1:8,2:6,3:4[mA]) : ");
	dsc_adr=Uart_GetIntNum();


	Uart_Printf("Control and clcok signal dive strength\n");
	Uart_Printf("CLK/Control signal (0:16/10,1:12/8,2:8/6,3:6/4[mA])");
	i=Uart_GetIntNum();
	switch(i) {
		case 0: dsc_ctrl = 0x00000000; break;
		case 1: dsc_ctrl = 0x15555555; break;
		case 2: dsc_ctrl = 0x2aaaaaaa; break;
		case 3: dsc_ctrl = 0x3fffffff; break;
		default: dsc_ctrl=0x00000000; break;
	}
		
	rDSC0 = (rDSC0 & ~(0x83ff<<0)) | (dsc_adr<<8) | dsc_data;

	rDSC1 = dsc_ctrl;
	
	Uart_Printf("rDSC0=%x, rDSC1=%x.\n", rDSC0, rDSC1);

	Uart_Printf("Memory R/W test(y/n)? ");
	if(Uart_Getch()=='y') {
		do {
			Mem_Test(1);
		} while(Uart_GetKey()==0);
	}
}

