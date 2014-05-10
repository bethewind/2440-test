/*
 * main.c
 *
 *  Created on: 2014-2-21
 *      Author: cyj
 */
#include "2440lib.h"
#include "addr2440.h"
#include "init2440.h"
#include "lcd.h"

#define GPF4_MASK (3 << (2 * 4))
#define GPF0_MASK (3 << (2 * 0))

#define GPF4_CON (1 << (2 * 4)) // out
#define GPF0_CON (2 << (2 * 0)) // Eint

void EINT_Handle() {
    int i;
    unsigned long offset = INTOFFSET;
    if (offset == 27) {}

    SRCPND |= 1<<offset;
    INTPND |= 1<<offset;
}

int main() {

	Uart_SendString("Hellocyj");
	//rGPBCON = 0x2; // GPB0 = TOUt0

    GPFCON &= ~(GPF4_MASK); //清0
    GPFCON |= (GPF4_CON); //赋值
    GPFDAT |= (1 << 4);

    Test_Lcd_480_272_16bpp();



    while (1);


    return 0;
}
