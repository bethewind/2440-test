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
#define DMAMODE

void init_uart_interrupt() {
	INTMSK &= ~(1<<28); //
	INTSUBMSK &= ~(1<<0); //
	SUBSRCPND |= 1<<0;
    SRCPND |= 1<<28;
    INTPND |= 1<<28;


}

void init_dma_uart() {
	UCON0 = 0x9;

	INTMSK &= ~(1<<17); //
    SRCPND |= 1<<17;
    INTPND |= 1<<17;
}

void dma_M2Uart(unsigned int srcAddr,unsigned int dstAddr,unsigned int tc) {
    rDISRC0=srcAddr;
    rDISRCC0=(0<<1)|(0<<0); // inc,AHB
    rDIDST0=dstAddr;
    rDIDSTC0=(0<<2) | (1<<1)|(1<<0); // inc,uart
    rDCON0=tc|((unsigned int)1<<31)|(0<<30)|(1<<29)|(0<<28)|(0<<27)|\
    		(1<<24)|(1<<23)|(1<<22)|(0<<20);
    rDMASKTRIG0=(1<<1); //DMA on
}

void EINT_Handle() {
	unsigned long offset = INTOFFSET;
    if (offset == 28) {
#ifndef DMAMODE
        Uart_SendString("uart\n");
        Uart_SendByte(URXH0+1);
#endif
        if (GPFDAT &(1<<4))
            GPFDAT &= ~(1<<4);      // LED1点亮
        else
            GPFDAT |= (1<<4);

        SUBSRCPND |= 1<<0;
    } else if (offset == 17) {
        if (GPFDAT &(1<<4))
            GPFDAT &= ~(1<<4);      // LED1点亮
        else
            GPFDAT |= (1<<4);


    }
    SRCPND |= 1<<offset;
    INTPND |= 1<<offset;
}

int main() {
	disable_watchdog();
	init_clock();
	init_uart();
#ifndef DMAMODE
	Uart_SendString("Hellocyj");
#endif
	//rGPBCON = 0x2; // GPB0 = TOUt0

    GPFCON &= ~(GPF4_MASK); //清0
    GPFCON |= (GPF4_CON); //赋值
    GPFDAT |= (1 << 4);
#ifndef DMAMODE
    init_uart_interrupt();
#else
    init_dma_uart();
    char* src = "HelloDma";
    dma_M2Uart((unsigned int)src,0x50000020/*UTXH0*/, 8);
#endif



    while (1);


    return 0;
}
