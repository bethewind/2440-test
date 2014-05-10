/*
 * FILE : Sram.c 
 * Description: 2420 user defined test program.
 * History:
 *	Nov/xx/2002	for 2420. S.J.Lee
 */

#include "def.h"
#include "option.h"
#include "2440lib.h"
#include "2440addr.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define	SRAM_SADDR	_NONCACHE_STARTADDRESS
#define	SRAM_EADDR	(SRAM_SADDR+ (128*1024))	// Total : 256KB, 0x3100_0000-0x3101_FFFF



void Mem_Test(int Print_msg)
{
	
	U32 addr=0, indata, src_data, i;
	U32 error;
	static int offset=0;
	U16 jump_offset=1;

	
	//Uart_Printf("Check: SRAM Area must be in non-cacheable area!\n");
	//Uart_Printf("SRAM W/R test[%xh-%xh], R/W offset[%d]\n", SRAM_SADDR, SRAM_EADDR-1, offset);
//	for(i=0; i<3; i++) {		// 8/16/32-bit.
	for(i=2; i<3; i++) {		// 8/16/32-bit.

		if(i==0) jump_offset=1;
		else if(i==1) jump_offset=2;
		else if(i==2) jump_offset=4;
		
		///////////////////// Clear Source/Target ////////////////////
		//Uart_Printf("Clear data.\n");
		for(addr=0; (SRAM_SADDR+addr)<SRAM_EADDR; addr+=4) {
			*(U32 *)(SRAM_SADDR+addr) = 0x0;
			if(!(addr%(4*1024))) {
				//Uart_Printf("\b\b\b\b\b\b\b\b\b\b%10x", SRAM_SADDR+addr);
				}
		}
		//Uart_Printf("\b\b\b\b\b\b\b\b\b\b%10x\n", SRAM_SADDR+addr);

		///////////////////// Write ////////////////////
		//Uart_Printf("Write data[%x-%x].\n", SRAM_SADDR, SRAM_EADDR);
		if(Print_msg>0) Uart_Printf("Wr(%d-bit)...", jump_offset*8);
		if(Print_msg>1) Uart_Printf("ADDR:%8x", SRAM_SADDR);
		Led_Display(0x1);
		for(addr=0; (SRAM_SADDR+addr)<SRAM_EADDR; addr+=jump_offset) {
			
			src_data = addr+offset;
		
			switch(jump_offset) {
				case 1:
					*(U8 *)(SRAM_SADDR+addr) = src_data;
				break;
				case 2:
					*(U16 *)(SRAM_SADDR+addr) = src_data;
				break;
				case 4:
					*(U32 *)(SRAM_SADDR+addr) = src_data;
				break;
			}
			if(!(addr%(4*1024))) {
				if(Print_msg>1) Uart_Printf("\b\b\b\b\b\b\b\b%8x", SRAM_SADDR+addr);
				}
		}
		if(Print_msg>1) Uart_Printf("\b\b\b\b\b\b\b\b%8x  ", SRAM_SADDR+addr);

		///////////////////// Verify //////////////////////
		//Uart_Printf("Verify[%x-%x].\n", SRAM_SADDR, SRAM_EADDR);
		if(Print_msg>0) Uart_Printf("Verify...");
		if(Print_msg>1) Uart_Printf("ADDR:%8x", SRAM_SADDR);
		Led_Display(0x2);
		for(error=0, addr=0; (SRAM_SADDR+addr)<SRAM_EADDR; addr+=jump_offset) {
			switch(jump_offset) {
				case 1:
					src_data = (U8)(addr+offset);
					indata = *(U8 *)(SRAM_SADDR+addr);
				break;
				case 2:
					src_data = (U16)(addr+offset);
					indata = *(U16 *)(SRAM_SADDR+addr);
				break;
				case 4:
					src_data = (U32)(addr+offset);
					indata = *(U32 *)(SRAM_SADDR+addr);
				break;
			}

			if(!(addr%(4*1024))) {
				if(Print_msg>1) Uart_Printf("\b\b\b\b\b\b\b\b%8x", SRAM_SADDR+addr);
			}
			
			if(indata != src_data) {
				error++;
				//Uart_Printf("%xH[W:%x, R:%x]\n", addr, src_data, indata);
			}
		}
		if(Print_msg>1) Uart_Printf("\b\b\b\b\b\b\b\b%8x  ", SRAM_SADDR+addr);
		
		if(error!=0) {
			if(Print_msg>0) Uart_Printf("ERROR(%d)...\n\n", error);
		} else {
			if(Print_msg>0) Uart_Printf("OK!\n");
			//Uart_Printf(".");
		}
		Led_Display(0xf);
	}

offset++;
}



