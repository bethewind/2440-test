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
#define GPHCON (*(volatile unsigned long*)0x56000070)
// interrupt
#define INTMSK (*(volatile unsigned long*)0X4A000008)
#define INTOFFSET (*(volatile unsigned long *)0x4A000014)
#define SRCPND (*(volatile unsigned long *)0x4A000000)
#define INTPND (*(volatile unsigned long *)0x4A000010)
//clock
#define MPLLCON (*(volatile unsigned long *)0x4C000004)
#define CLKDIVN (*(volatile unsigned long *)0x4C000014)
//uart
#define ULCON0 (*(volatile unsigned long *)0x50000000)
#define UCON0 (*(volatile unsigned long *)0x50000004)
#define UFCON0 (*(volatile unsigned long *)0x50000008)
#define UMCON0 (*(volatile unsigned long *)0x5000000C)
#define UTRSTAT0            (*(volatile unsigned long *)0x50000010)
#define UTXH0               (*(volatile unsigned char *)0x50000020)
#define URXH0               (*(volatile unsigned char *)0x50000024)
#define UBRDIV0 (*(volatile unsigned long *)0x50000028)

//Nand Flash
#define rNFCONF		(*(volatile unsigned *)0x4E000000)		//NAND Flash configuration
#define rNFCONT		(*(volatile unsigned *)0x4E000004)      //NAND Flash control
#define rNFCMD		(*(volatile unsigned *)0x4E000008)      //NAND Flash command
#define rNFADDR		(*(volatile unsigned *)0x4E00000C)      //NAND Flash address
#define rNFDATA		(*(volatile unsigned *)0x4E000010)      //NAND Flash data
#define rNFDATA8	(*(volatile unsigned char *)0x4E000010)     //NAND Flash data
#define NFDATA		(0x4E000010)      //NAND Flash data address
#define rNFMECCD0	(*(volatile unsigned *)0x4E000014)      //NAND Flash ECC for Main Area
#define rNFMECCD1	(*(volatile unsigned *)0x4E000018)
#define rNFSECCD	(*(volatile unsigned *)0x4E00001C)		//NAND Flash ECC for Spare Area
#define rNFSTAT		(*(volatile unsigned *)0x4E000020)		//NAND Flash operation status
#define rNFESTAT0	(*(volatile unsigned *)0x4E000024)
#define rNFESTAT1	(*(volatile unsigned *)0x4E000028)
#define rNFMECC0	(*(volatile unsigned *)0x4E00002C)
#define rNFMECC1	(*(volatile unsigned *)0x4E000030)
#define rNFSECC		(*(volatile unsigned *)0x4E000034)
#define rNFSBLK		(*(volatile unsigned *)0x4E000038)		//NAND Flash Start block address
#define rNFEBLK		(*(volatile unsigned *)0x4E00003C)		//NAND Flash End block address




#endif /* ADDR2440_H_ */
