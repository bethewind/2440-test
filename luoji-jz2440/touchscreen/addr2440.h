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
#define WTDAT (*(volatile unsigned long*)0x53000004)
#define WTCNT (*(volatile unsigned long*)0x53000008)
// sdram
#define BWSCON (*(volatile unsigned long*)0x48000000)
//gpio
#define GPFCON (*(volatile unsigned long*)0x56000050)
#define GPFDAT (*(volatile unsigned long*)0x56000054)
#define EXTINT0 (*(volatile unsigned long*)0x56000088)
#define GPHCON (*(volatile unsigned long*)0x56000070)
#define rGPBCON (*(volatile unsigned long*)0x56000010)
#define rGPBDAT (*(volatile unsigned long*)0x56000014)
#define rGPBUP (*(volatile unsigned long*)0x56000018)
#define rGPCCON (*(volatile unsigned long*)0x56000020)
#define rGPCDAT (*(volatile unsigned long*)0x56000024)
#define rGPCUP (*(volatile unsigned long*)0x56000028)
#define rGPDCON (*(volatile unsigned long*)0x56000030)
#define rGPDDAT (*(volatile unsigned long*)0x56000034)
#define rGPDUP (*(volatile unsigned long*)0x56000038)
#define rGPECON (*(volatile unsigned long*)0x56000040)
#define rGPEDAT (*(volatile unsigned long*)0x56000044)
#define rGPEUP (*(volatile unsigned long*)0x56000048)
#define rGPGCON (*(volatile unsigned long*)0x56000060)
#define rGPGDAT (*(volatile unsigned long*)0x56000064)
#define rGPGUP (*(volatile unsigned long*)0x56000068)
// interrupt
#define INTMSK (*(volatile unsigned long*)0x4A000008)
#define INTOFFSET (*(volatile unsigned long *)0x4A000014)
#define SRCPND (*(volatile unsigned long *)0x4A000000)
#define INTPND (*(volatile unsigned long *)0x4A000010)
#define SUBSRCPND (*(volatile unsigned long *)0X4A000018)
#define INTSUBMSK (*(volatile unsigned long *)0X4A00001C)
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
// timer
#define rTCFG0     (*(volatile unsigned *)0x51000000)
#define rTCFG1     (*(volatile unsigned *)0x51000004)
#define rTCON     (*(volatile unsigned *)0x51000008)
#define rTCNTB0     (*(volatile unsigned *)0x5100000C)
#define rTCMPB0     (*(volatile unsigned *)0x51000010)
// rtc
#define rRTCCON    (*(volatile unsigned char *)0x57000040)  //RTC control
#define rTICNT     (*(volatile unsigned char *)0x57000044)  //Tick time count
#define rRTCALM    (*(volatile unsigned char *)0x57000050)  //RTC alarm control
#define rALMSEC    (*(volatile unsigned char *)0x57000054)  //Alarm second
#define rALMMIN    (*(volatile unsigned char *)0x57000058)  //Alarm minute
#define rALMHOUR   (*(volatile unsigned char *)0x5700005c)  //Alarm Hour
#define rALMDATE   (*(volatile unsigned char *)0x57000060)  //Alarm date  // edited by junon
#define rALMMON    (*(volatile unsigned char *)0x57000064)  //Alarm month
#define rALMYEAR   (*(volatile unsigned char *)0x57000068)  //Alarm year
#define rRTCRST    (*(volatile unsigned char *)0x5700006c)  //RTC round reset
#define rBCDSEC    (*(volatile unsigned char *)0x57000070)  //BCD second
#define rBCDMIN    (*(volatile unsigned char *)0x57000074)  //BCD minute
#define rBCDHOUR   (*(volatile unsigned char *)0x57000078)  //BCD hour
#define rBCDDATE   (*(volatile unsigned char *)0x5700007c)  //BCD date  //edited by junon
#define rBCDDAY    (*(volatile unsigned char *)0x57000080)  //BCD day   //edited by junon
#define rBCDMON    (*(volatile unsigned char *)0x57000084)  //BCD month
#define rBCDYEAR   (*(volatile unsigned char *)0x57000088)  //BCD year
// IIC
#define rIICCON     (*(volatile unsigned *)0x54000000)  //IIC control
#define rIICSTAT    (*(volatile unsigned *)0x54000004)  //IIC status
#define rIICADD     (*(volatile unsigned *)0x54000008)  //IIC address
#define rIICDS      (*(volatile unsigned *)0x5400000c)  //IIC data shift
#define rIICLC      (*(volatile unsigned *)0x54000010)  //IIC multi-master line control
// LCD CONTROLLER
#define LCDCON1     (*(volatile unsigned long *)0x4d000000) //LCD control 1
#define LCDCON2     (*(volatile unsigned long *)0x4d000004) //LCD control 2
#define LCDCON3     (*(volatile unsigned long *)0x4d000008) //LCD control 3
#define LCDCON4     (*(volatile unsigned long *)0x4d00000c) //LCD control 4
#define LCDCON5     (*(volatile unsigned long *)0x4d000010) //LCD control 5
#define LCDSADDR1   (*(volatile unsigned long *)0x4d000014) //STN/TFT Frame buffer start address 1
#define LCDSADDR2   (*(volatile unsigned long *)0x4d000018) //STN/TFT Frame buffer start address 2
#define LCDSADDR3   (*(volatile unsigned long *)0x4d00001c) //STN/TFT Virtual screen address set
#define REDLUT      (*(volatile unsigned long *)0x4d000020) //STN Red lookup table
#define GREENLUT    (*(volatile unsigned long *)0x4d000024) //STN Green lookup table
#define BLUELUT     (*(volatile unsigned long *)0x4d000028) //STN Blue lookup table
#define DITHMODE    (*(volatile unsigned long *)0x4d00004c) //STN Dithering mode
#define TPAL        (*(volatile unsigned long *)0x4d000050) //TFT Temporary palette
#define LCDINTPND   (*(volatile unsigned long *)0x4d000054) //LCD Interrupt pending
#define LCDSRCPND   (*(volatile unsigned long *)0x4d000058) //LCD Interrupt source
#define LCDINTMSK   (*(volatile unsigned long *)0x4d00005c) //LCD Interrupt mask
#define LPCSEL      (*(volatile unsigned long *)0x4d000060) //LPC3600 Control
#define PALETTE     0x4d000400                              //Palette start address
// IIS
#define rIISCON  (*(volatile unsigned *)0x55000000) //IIS Control
#define rIISMOD  (*(volatile unsigned *)0x55000004) //IIS Mode
#define rIISPSR  (*(volatile unsigned *)0x55000008) //IIS Prescaler
#define rIISFCON (*(volatile unsigned *)0x5500000c) //IIS FIFO control
#define rIISFIFO  (*(volatile unsigned short *)0x55000010)    //IIS FIFO entry
// ADC
#define rADCCON    (*(volatile unsigned *)0x58000000)   //ADC control
#define rADCTSC    (*(volatile unsigned *)0x58000004)   //ADC touch screen control
#define rADCDLY    (*(volatile unsigned *)0x58000008)   //ADC start or Interval Delay
#define rADCDAT0   (*(volatile unsigned *)0x5800000c)   //ADC conversion data 0
#define rADCDAT1   (*(volatile unsigned *)0x58000010)   //ADC conversion data 1
#define rADCUPDN   (*(volatile unsigned *)0x58000014)   //Stylus Up/Down interrupt status





#endif /* ADDR2440_H_ */
