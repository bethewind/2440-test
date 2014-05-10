/*
 * nand.h
 *
 *  Created on: 2014-3-16
 *      Author: cyj
 */

#ifndef NAND_H_
#define NAND_H_

#include "addr2440.h"

#define NF_CMD(cmd)			{rNFCMD=cmd;}
#define NF_ADDR(addr)		{rNFADDR=addr;}
#define NF_nFCE_L()			{rNFCONT&=~(1<<1);}
#define NF_nFCE_H()			{rNFCONT|=(1<<1);}
#define NF_RDDATA() 		(rNFDATA)
#define NF_RDDATA8() 		((*(volatile unsigned char*)0x4E000010) )
#define NF_WRDATA(data) 	{rNFDATA=data;}
#define NF_WRDATA8(data) 	{rNFDATA8=data;}
#define NF_WAITRB()         {while(!(rNFSTAT&(1<<0)));}           //等待nandflash不忙


#endif /* NAND_H_ */
