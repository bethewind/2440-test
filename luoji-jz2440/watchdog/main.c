/*
 * main.c
 *
 *  Created on: 2014-2-21
 *      Author: cyj
 */
#include "2440lib.h"
#include "addr2440.h"
#include "init2440.h"

#define GPF4_MASK (3 << (2 * 4))
#define GPF0_MASK (3 << (2 * 0))

#define GPF4_CON (1 << (2 * 4)) // out
#define GPF0_CON (2 << (2 * 0)) // Eint

// Timer input clock frequence = PCLK/(prescaler + 1)/diver value
// Timer0 = 50MHZ/(99 + 1)/16 = 31250HZ
void initWatchdog() {
	INTMSK &= ~(1<<9); // INT_TIMER0 interrupt enable
	INTSUBMSK &= ~(1<<13); // INT_TIMER0 interrupt enable
    SRCPND |= 1<<9;
    INTPND |= 1<<9;
    SUBSRCPND |= 1<<13;

	WTCON = (99<<8);
	WTCON |= (1<<5);
	WTCON |= (1<<2);
	WTDAT = 31250;
	WTCNT = 31250;

}

void EINT_Handle() {
    if (INTOFFSET == 9) {
        Uart_SendString("watchdog\n");
        if (GPFDAT &(1<<4))
            GPFDAT &= ~(1<<4);      // LED1点亮
        else
            GPFDAT |= (1<<4);

        SRCPND |= 1<<9;
        INTPND |= 1<<9;
        SUBSRCPND |= 1<<13;
    }
}

int main() {
	init_clock();
	init_uart();

	Uart_SendString("Hellocyj");
	//rGPBCON = 0x2; // GPB0 = TOUt0

    GPFCON &= ~(GPF4_MASK); //清0
    GPFCON |= (GPF4_CON); //赋值
    GPFDAT |= (1 << 4);

    initWatchdog();



    while (1);


    return 0;
}
