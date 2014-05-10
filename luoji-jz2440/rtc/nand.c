/*
 * init.c
 *
 *  Created on: 2014-3-5
 *      Author: cyj
 */
#include "addr2440.h"

void init_nand();
void nand_reset();
void nand_select();
void nand_deselect();
void write_cmd(unsigned int cmd);
void wait_idle();
unsigned char read_data8();
void write_address(unsigned int address);
void nand_read(unsigned char* dest, unsigned int source, unsigned int size);

void init_nand() {
#define TACLS   0
#define TWRPH0  3
#define TWRPH1  0

	rNFCONF = (TACLS<<12)|(TWRPH0<<8)|(TWRPH1<<4);
	rNFCONT = (1<<4)|(1<<1)|(1<<0);

    nand_reset();
}

void nand_reset() {
    nand_select();
    write_cmd(0xff);
    wait_idle();
    nand_deselect();
}

void nand_select() {
	rNFCONT &= ~(1<<1);
}

void nand_deselect() {
	rNFCONT |= (1<<1);
}

void write_cmd(unsigned int cmd) {
	rNFCMD = cmd;
}

void wait_idle() {
	int i;
	while (!(rNFSTAT&1))
		for(i = 0;i<10;i++);
}

unsigned char read_data8() {
	return rNFDATA8;
}

void write_address(unsigned int addr) {
	//int i;
	int col, page;
    col = addr % 2048;
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

void nand_read(unsigned char* dest, unsigned int source, unsigned int size) {
	int i,j;
    nand_select();
    for (i=source;i<(source+size);) {
    	write_cmd(0x0);
		write_address(i);
		write_cmd(0x30);
		wait_idle();
		for(j=0;j<2048;j++,i++) {
			*dest = read_data8();
			dest++;
		}
    }
    nand_deselect();
}

void nand_read2() {
	nand_read((unsigned char*)0x30000000, 4096, 30*1024);
	//nand_read((unsigned char*)0x31000000, 0, 6096);
}

//unsigned int EraseBlock(unsigned int blocknum) {
//	nand_select();
//	unsigned int pagenum = blocknum * 64;
//	write_cmd(0x60);
//	rNFADDR = pagenum & 0xff;			/* Row Address A12~A19 */
//	rNFADDR = (pagenum >> 8) & 0xff;	/* Row Address A20~A27 */
//	rNFADDR = (pagenum >> 16) & 0x03;	/* Row Address A28~A29 */
//	write_cmd(0xd0);
//	wait_idle();
//	write_cmd(0x70);
//	if (rNFDATA & 0x1) {
//		nand_deselect();
//		return 0;
//	} else {
//		nand_deselect();
//		return 1;
//	}
//}
