/*
 * init.c
 *
 *  Created on: 2014-3-5
 *      Author: cyj
 */
#define WTCON (*(volatile unsigned long*)0x53000000)
#define BWSCON (*(volatile unsigned long*)0x48000000)


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
            0x008C07A3      ,// REFRESH
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
