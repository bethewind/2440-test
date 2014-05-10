/*
 * main.c
 *
 *  Created on: 2014-2-21
 *      Author: cyj
 */
#include "2440lib.h"
#include "addr2440.h"
#include "init2440.h"
#include "iis.h"
#include "WindowsXP_Wav.h"

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
int i;
	Uart_SendString("Helloiis\n");
	//rGPBCON = 0x2; // GPB0 = TOUt0

    GPFCON &= ~(GPF4_MASK); //清0
    GPFCON |= (GPF4_CON); //赋值
    GPFDAT |= (1 << 4);

    // config 2440 codec interface
    rGPBCON = (1<<8) | (1<<6) | (1<<4);
    rGPBUP = 0x1c;
    // config 2440 iis
    rGPECON = 0x2aa;
    rGPEUP = 0x1f;



    while (1) {
    	playsound(WindowsXP_Wav, sizeof(WindowsXP_Wav));

    	    Uart_SendString("Helloiis over\n");
    	    for (i = 0;i<1000;i++);
    }


    return 0;
}
