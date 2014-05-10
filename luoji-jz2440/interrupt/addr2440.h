/*
 * addr2440.h
 *
 *  Created on: 2014-3-11
 *      Author: cyj
 */

#ifndef ADDR2440_H_
#define ADDR2440_H_

// watchdog
#define WTCON (*(volatile unsigned long*)0x53000000)
// sdram
#define BWSCON (*(volatile unsigned long*)0x48000000)
//gpio
#define GPFCON (*(volatile unsigned long*)0x56000050)
#define GPFDAT (*(volatile unsigned long*)0x56000054)
#define EXTINT0 (*(volatile unsigned long*)0x56000088)
// interrupt
#define INTMSK (*(volatile unsigned long*)0X4A000008)
#define INTOFFSET (*(volatile unsigned long *)0x4A000014)
#define SRCPND (*(volatile unsigned long *)0x4A000000)
#define INTPND (*(volatile unsigned long *)0x4A000010)



#endif /* ADDR2440_H_ */
