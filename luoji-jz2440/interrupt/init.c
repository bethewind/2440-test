/*
 * init.c
 *
 *  Created on: 2014-3-5
 *      Author: cyj
 */
#include "addr2440.h"
#define GPF4_MASK (3 << (2 * 4))
#define GPF0_MASK (3 << (2 * 0))

#define GPF4_CON (1 << (2 * 4)) // out
#define GPF0_CON (2 << (2 * 0)) // Eint

void init_led() {
    GPFCON &= ~(GPF4_MASK); //清0
    GPFCON |= (GPF4_CON); //赋值
    GPFDAT |= (1 << 4);
}

void init_interrupt() {
    GPFCON &= ~(GPF0_MASK); //清0
    GPFCON |= (GPF0_CON); //赋值

    //低电平触发
    EXTINT0 &= ~0x7;
    EXTINT0 |= 0x0;

    INTMSK &= ~(1);
}
