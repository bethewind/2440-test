/********************************************************
  NAME: stone.c
  DESC: NAND Internal RAM(stepping stone) test (4KByte)
  HISTORY:
  2002.03.07: ver 0.0
  Rev0.0: July 12, 2002 : Shin, On Pil
  Rev0.1: Ma xx, 2003 : modified for 2440, DonGo.
 ********************************************************/

#include "def.h"
#include "option.h"
#include "2440addr.h"
#include "2440lib.h"
#include "stone.h"

#define	STONE_ADDR	0x40000000
#define	STONE_SIZE	(4*1024)	// 4KB

void Test_ISram(void)
{
    U32 i, j, error=0, loop;
	U32 test_pattern[4] = {0x0, 0x55555555, 0xaaaaaaaa, 0xffffffff};
	U32 *Stone_Addr = (void *)STONE_ADDR;

    Uart_Printf("[Internal Stepping Stone test (%dbyte)]\n", STONE_SIZE);
    Uart_Printf("\nTest Pattern : 0x0 , 0x55555555 , 0xaaaaaaaa , 0xffffffff\n\n");    


	for(loop=0; loop<4; loop++) {
		// write data.
		Uart_Printf("Write [%x]...", test_pattern[loop]);    
	    for(i=0; i<STONE_SIZE/4; i++) *(Stone_Addr+i)=test_pattern[loop];

		// Read data.		
		Uart_Printf("Read...");
	    for(error=0, i=0; i<STONE_SIZE/4; i+=4) {
		    j=*(Stone_Addr+i);
		    if(j!=test_pattern[loop]) {
		        Uart_Printf("Error[AD:%x,Wr:%x,Rd:%x\n",(Stone_Addr+i),test_pattern[loop], j);
		        error++;
		    }
	    }
		if(error==0) Uart_Printf("OK!\n");
		else Uart_Printf("Error(%d)!\n", error);
	}


}
