/*
 * main.c
 *
 *  Created on: 2014-2-21
 *      Author: cyj
 */
#include "2440lib.h"
#include "addr2440.h"
#include "nand.h"

#define GPFCON (*(volatile unsigned long*)0x56000050)
#define GPFDAT (*(volatile unsigned long*)0x56000054)

#define GPF4_MASK (3 << (2 * 4))
#define GPF0_MASK (3 << (2 * 0))

#define GPF4_CON (1 << (2 * 4)) // out
#define GPF0_CON (0 << (2 * 0)) // in

unsigned int EraseBlock(unsigned int blocknum) {
	int i;
	rNFCONT &= ~(1<<1);//nand_select();
	unsigned int pagenum = blocknum * 64;
	rNFCMD = 0x60;//write_cmd(0x60);
	rNFADDR = pagenum & 0xff;			/* Row Address A12~A19 */
	rNFADDR = (pagenum >> 8) & 0xff;	/* Row Address A20~A27 */
	rNFADDR = (pagenum >> 16) & 0x03;	/* Row Address A28~A29 */
	rNFCMD = 0xd0;//write_cmd(0xd0);
	//wait_idle();
	while (!(rNFSTAT&1))
		for(i = 0;i<10;i++);
	rNFCMD = 0x70;//write_cmd(0x70);
	if (rNFDATA & 0x1) {
		rNFCONT |= (1<<1);//nand_deselect();
		return 0;
	} else {
		rNFCONT |= (1<<1);//nand_deselect();
		return 1;
	}
}

void write_address5(unsigned int addr) {
	//int i;
	int col, page;
    col = addr & 2048;
	page = addr / 2048;

	rNFADDR = col & 0xff;			/* Column Address A0~A7 */
	//for(i=0; i<10; i++);
	rNFADDR = (col >> 8) & 0x0f; 	/* Column Address A8~A11 */
	//for(i=0; i<10; i++);
	rNFADDR = page & 0xff;			/* Row Address A12~A19 */
	//for(i=0; i<10; i++);
	rNFADDR = (page >> 8) & 0xff;	/* Row Address A20~A27 */
	//for(i=0; i<10; i++);
	rNFADDR = (page >> 16) & 0x03;	/* Row Address A28~A29 */
	//for(i=0; i<10; i++);
}

void nand_write(unsigned int dest, unsigned char* source, unsigned int size) {
	int i,j,k;
	NF_nFCE_L();
    for (i=dest;i<(dest+size);) {
    	NF_CMD(0x80);
		write_address5(i);
		for(k=0; k<10; k++);

		for(j=0;j<2048;j++,i++) {
			NF_WRDATA8(*source);
			source++;
		}
		NF_CMD(0x10);
		NF_WAITRB();
		NF_CMD(0x70);
		if (rNFDATA & 0x1) {
			Uart_SendString("write one page failed\n");
		} else {
			Uart_SendString("write one page successed\n");
		}
    }
    NF_nFCE_H();
}

int main() {
	Uart_SendString("Hellocyj\n");
	if (EraseBlock(0))
		Uart_SendString("Erase success\n");
	else
		Uart_SendString("Erase failed\n");
	//把某位赋0,先把那位清0,再赋值。。赋1直接或。

	nand_write(0, (unsigned char*)0x31000000, 5096);

    GPFCON &= ~(GPF4_MASK | GPF0_MASK); //清0
    GPFCON |= (GPF4_CON | GPF0_CON); //赋值


    while (1) {
    	unsigned long gpfDat = GPFDAT;
        if (gpfDat & 0x00000001) { // 1, 断开  判断某位是否为1
            GPFDAT |= (1 << 4);
        } else {
            GPFDAT &= ~(1 << 4);
        }
    }


    return 0;
}
