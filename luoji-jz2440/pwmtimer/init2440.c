/*
 * init.c
 *
 *  Created on: 2014-3-5
 *      Author: cyj
 */
#include "addr2440.h"
#include "2440lib.h"


void disable_watchdog() {
    WTCON = 0;
}

void init_sdram() {
    unsigned long mem_cfg_val[] = {
            0x22011110      ,// BWSCON
            0x00000700      ,// BANKCON0
            0x00000700      ,// BANKCON1
            0x00000700      ,// BANKCON2
            0x00000700      ,// BANKCON3
            0x00000700      ,// BANKCON4
            0x00000700      ,// BANKCON5
            0x00018005      ,// BANKCON6
            0x00018005      ,// BANKCON7
            0x008C04F4      ,// REFRESH HCLK:100M
            0x000000B1      ,// BANKSIZE
            0x00000030      ,// MRSRB6
            0x00000030      ,// MRSRB7
    };

    volatile unsigned long* bWSCON = (volatile unsigned long*)0x48000000;
    int i = 0;
    for (; i < 13; i++) {
        bWSCON[i] = mem_cfg_val[i];
    }
}
#define S3C2440_MPLL_200MHZ     ((0x5c<<12)|(0x01<<4)|(0x02))    // 200M,100M,50M
void init_clock() {
	MPLLCON = S3C2440_MPLL_200MHZ;

	CLKDIVN = 0x3; // 200M,100M,50M

	__asm__(
	    "mrc    p15, 0, r1, c1, c0, 0\n"
	    "orr    r1, r1, #0xc0000000\n"
	    "mcr    p15, 0, r1, c1, c0, 0\n"
	    );
}

void init_uart() {
	GPHCON &= ~(0x0f << 4);
	GPHCON |= (0x0a << 4);

	ULCON0 = 0x3;
	UCON0 = 0x5;
	UFCON0 = 0x0;
	UMCON0 = 0x0;
	UBRDIV0 = 26;

}


