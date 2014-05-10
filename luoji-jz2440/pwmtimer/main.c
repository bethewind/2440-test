/*
 * main.c
 *
 *  Created on: 2014-2-21
 *      Author: cyj
 */
#include "2440lib.h"
#include "addr2440.h"

#define GPF4_MASK (3 << (2 * 4))
#define GPF0_MASK (3 << (2 * 0))

#define GPF4_CON (1 << (2 * 4)) // out
#define GPF0_CON (2 << (2 * 0)) // Eint

// Timer input clock frequence = PCLK/(prescaler + 1)/diver value
// Timer0 = 50MHZ/(99 + 1)/16 = 31250HZ
void init_timer0() {
    rTCFG0 = 99;
    rTCFG1 = 0x3;
    rTCNTB0 = 31250;
    rTCON |= (1<<1);
    rTCON = 0x9;

}

void EINT_Handle() {
    if (INTOFFSET == 10) {
        Uart_SendString("timer0\n");
        if (GPFDAT &(1<<4))
            GPFDAT &= ~(1<<4);      // LED1点亮
        else
            GPFDAT |= (1<<4);
    }

    SRCPND = 1<<INTOFFSET;
    INTPND = INTPND;
}

int main() {
	Uart_SendString("Hellocyj");
	//rGPBCON = 0x2; // GPB0 = TOUt0

    GPFCON &= ~(GPF4_MASK); //清0
    GPFCON |= (GPF4_CON); //赋值
    GPFDAT |= (1 << 4);
	INTMSK &= ~(1<<10); // INT_TIMER0 interrupt enable
	init_timer0();



    while (1);


    return 0;
}
