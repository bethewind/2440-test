/**************************************************************************************
* 
*	Project Name : S3C6410 Validation
*
*	Copyright 2006 by Samsung Electronics, Inc.
*	All rights reserved.
*
*	Project Description :
*		This software is only for validating functions of the S3C6410.
*		Anybody can use this software without our permission.
*  
*--------------------------------------------------------------------------------------
* 
*	File Name : sblock.c
*  
*	File Description : This file implements the API functons for Security subsystem.
*				    This Test code is oriented by SA Verification Code
*
*	Author : Wonjoon,Jang
*	Dept. : AP Development Team
*	Created Date : 2007/01/18
*	Version : 0.1 
* 
*	History
*	- Created(wonjoon.jang 2007/01/18)
*  
**************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "option.h"
#include "library.h"
#include "sfr6410.h"
#include "system.h"
#include "intc.h"
#include "gpio.h"

#include "sysc.h"
#include "dma.h"
#include "sblock.h"
#include "sblock_para.h"


	// Define SFR
	//DMA & Interrupt (AHB-Rx)
#define	rDNI_CFG					(DnI_BASE + 0x00)

	//FIFO Rx(AHB-Tx)
#define	rFIFO_RX_CTRL				(RX_SFR_BASE+0x00)
#define	rFIFO_RX_MSG_LENGTH		(RX_SFR_BASE+0x04)
#define	rFIFO_RX_BLK_SIZE			(RX_SFR_BASE+0x08)
#define	rFIFO_RX_DST_ADDR			(RX_SFR_BASE+0x0C)
#define	rFIFO_RX_MSG_LENGTH_CNT	(RX_SFR_BASE+0x10)
#define	rFIFO_RX_WR_BUF			(RX_SFR_BASE+0x40)		// 0x40~0x80,  32X 4B

	//FIFO RX DMA Addr  = RX_FIFO_BASE + 0x40
#define	rSDMA1_RX_FIFO			(RX_FIFO_BASE+0x40)

	//FIFO Tx(AHB-Rx)
#define	rFIFO_TX_CTRL				(TX_SFR_BASE+0x00)
#define	rFIFO_TX_MSG_LENGTH		(TX_SFR_BASE+0x04)
#define	rFIFO_TX_BLK_SIZE			(TX_SFR_BASE+0x08)
#define	rFIFO_TX_SRC_ADDR			(TX_SFR_BASE+0x0C)
#define	rFIFO_TX_MSG_LENGTH_CNT	(TX_SFR_BASE+0x10)
#define	rFIFO_TX_RD_BUF			(TX_SFR_BASE+0x40)		// 0x40~0x80,  32X4B

	//FIFO TX DMA Address = TX_FIFO_BASE + 0x40
#define	rSDMA1_TX_FIFO			(TX_FIFO_BASE+0x40)

	//AES (AHB-Rx)
#define	rAES_RX_CTRL				(AES_RX_BASE+0x00)
#define	rAES_RX_DIN1				(AES_RX_BASE+0x10)
#define	rAES_RX_DIN2				(AES_RX_BASE+0x14)
#define	rAES_RX_DIN3				(AES_RX_BASE+0x18)
#define	rAES_RX_DIN4				(AES_RX_BASE+0x1C)
#define	rAES_RX_DOUT1				(AES_RX_BASE+0x20)
#define	rAES_RX_DOUT2				(AES_RX_BASE+0x24)
#define	rAES_RX_DOUT3				(AES_RX_BASE+0x28)
#define	rAES_RX_DOUT4				(AES_RX_BASE+0x2C)
#define	rAES_RX_KEY1				(AES_RX_BASE+0x80)
#define	rAES_RX_KEY2				(AES_RX_BASE+0x84)
#define	rAES_RX_KEY3				(AES_RX_BASE+0x88)
#define	rAES_RX_KEY4				(AES_RX_BASE+0x8C)
#define	rAES_RX_KEY5				(AES_RX_BASE+0x90)
#define	rAES_RX_KEY6				(AES_RX_BASE+0x94)
#define	rAES_RX_KEY7				(AES_RX_BASE+0x98)
#define	rAES_RX_KEY8				(AES_RX_BASE+0x9C)
#define	rAES_RX_IV1				(AES_RX_BASE+0xA0)
#define	rAES_RX_IV2				(AES_RX_BASE+0xA4)
#define	rAES_RX_IV3				(AES_RX_BASE+0xA8)
#define	rAES_RX_IV4				(AES_RX_BASE+0xAC)
#define	rAES_RX_CTR1				(AES_RX_BASE+0xB0)
#define	rAES_RX_CTR2				(AES_RX_BASE+0xB4)
#define	rAES_RX_CTR3				(AES_RX_BASE+0xB8)
#define	rAES_RX_CTR4				(AES_RX_BASE+0xBC)

	//AES (AHB-Tx)
#define	rAES_TX_DOUT1				(AES_TX_BASE+0x20)
#define	rAES_TX_DOUT2				(AES_TX_BASE+0x24)
#define	rAES_TX_DOUT3				(AES_TX_BASE+0x28)
#define	rAES_TX_DOUT4				(AES_TX_BASE+0x2C)

	//DES&3DES (AHB-Rx)
#define	rDES_RX_CTRL				(DES_RX_BASE+0x00)
#define	rDES_RX_KEY1_0				(DES_RX_BASE+0x10)
#define	rDES_RX_KEY1_1				(DES_RX_BASE+0x14)
#define	rDES_RX_KEY2_0				(DES_RX_BASE+0x18)
#define	rDES_RX_KEY2_1				(DES_RX_BASE+0x1C)
#define	rDES_RX_KEY3_0				(DES_RX_BASE+0x20)
#define	rDES_RX_KEY3_1				(DES_RX_BASE+0x24)
#define	rDES_RX_IN0				(DES_RX_BASE+0x40)
#define	rDES_RX_IN1				(DES_RX_BASE+0x44)
#define	rDES_RX_OUT0				(DES_RX_BASE+0x48)
#define	rDES_RX_OUT1				(DES_RX_BASE+0x4C)
#define	rDES_RX_IV0				(DES_RX_BASE+0x50)
#define	rDES_RX_IV1				(DES_RX_BASE+0x54)

	//DES&3DES (AHB-Tx)
#define	rDES_TX_OUT0				(DES_TX_BASE+0x48)
#define	rDES_TX_OUT1				(DES_TX_BASE+0x4C)

	//SHA1 & PRNG (AHB-Rx)
#define	rHASH_RX_CTRL				(HASH_RX_BASE+0x00)
#define	rHASH_RX_DAT				(HASH_RX_BASE+0x04)		// 0x04~0x20  8X4 B
#define	rPRNG_SEED_DAT1			(HASH_RX_BASE+0x08)
#define	rPRNG_SEED_DAT2			(HASH_RX_BASE+0x0C)
#define	rPRNG_SEED_DAT3			(HASH_RX_BASE+0x10)
#define	rPRNG_SEED_DAT4			(HASH_RX_BASE+0x14)
#define	rPRNG_SEED_DAT5			(HASH_RX_BASE+0x18)
#define	rPRNG_SEED_DAT6			(HASH_RX_BASE+0x1C)
#define	rPRNG_SEED_DAT7			(HASH_RX_BASE+0x20)
#define	rPRNG_SEED_DAT8			(HASH_RX_BASE+0x24)
#define	rPRNG_SEED_DAT9			(HASH_RX_BASE+0x28)
#define	rPRNG_SEED_DAT10			(HASH_RX_BASE+0x2C)
#define	rHASH_RX_STAT				(HASH_RX_BASE+0x30)
#define	rHASH_OUTPUT_01			(HASH_RX_BASE+0x34)
#define	rHASH_OUTPUT_02			(HASH_RX_BASE+0x38)
#define	rHASH_OUTPUT_03			(HASH_RX_BASE+0x3C)
#define	rHASH_OUTPUT_04			(HASH_RX_BASE+0x40)
#define	rHASH_OUTPUT_05			(HASH_RX_BASE+0x44)
#define	rHASH_OUTPUT_06			(HASH_RX_BASE+0x48)
#define	rHASH_OUTPUT_07			(HASH_RX_BASE+0x4C)
#define	rHASH_OUTPUT_08			(HASH_RX_BASE+0x50)
#define	rHASH_OUTPUT_09			(HASH_RX_BASE+0x54)
#define	rHASH_OUTPUT_10			(HASH_RX_BASE+0x58)
#define	rHASH_RX_IV1				(HASH_RX_BASE+0x70)
#define	rHASH_RX_IV2				(HASH_RX_BASE+0x74)
#define	rHASH_RX_IV3				(HASH_RX_BASE+0x78)
#define	rHASH_RX_IV4				(HASH_RX_BASE+0x7C)
#define	rHASH_RX_IV5				(HASH_RX_BASE+0x80)
#define	rPRE_MSG_LENGTH1			(HASH_RX_BASE+0x84)
#define	rPRE_MSG_LENGTH2			(HASH_RX_BASE+0x88)

	//SHA1 & PRNG (AHB-Tx)
#define	rHASH_TX_OUT1				(HASH_TX_BASE+0x34)
#define	rHASH_TX_OUT2				(HASH_TX_BASE+0x38)
#define	rHASH_TX_OUT3				(HASH_TX_BASE+0x3C)
#define	rHASH_TX_OUT4				(HASH_TX_BASE+0x40)
#define	rHASH_TX_OUT5				(HASH_TX_BASE+0x44)

#define	rPRNG_TX_OUT1				(HASH_TX_BASE+0x34)
#define	rPRNG_TX_OUT2				(HASH_TX_BASE+0x38)
#define	rPRNG_TX_OUT3				(HASH_TX_BASE+0x3C)
#define	rPRNG_TX_OUT4				(HASH_TX_BASE+0x40)
#define	rPRNG_TX_OUT5				(HASH_TX_BASE+0x44)
#define	rPRNG_TX_OUT6				(HASH_TX_BASE+0x48)
#define	rPRNG_TX_OUT7				(HASH_TX_BASE+0x4C)
#define	rPRNG_TX_OUT8				(HASH_TX_BASE+0x50)
#define	rPRNG_TX_OUT9				(HASH_TX_BASE+0x54)
#define	rPRNG_TX_OUT10			( HASH_TX_BASE+0x58)

// rHASH_CONTROL
// [Data_Selection]
#define HASH_DATA_TEXT					(0<<2)
#define HASH_DATA_KEY					(1<<2)
// [USE_IV]
#define HASH_CONST_HASH				(0<<8)
#define HASH_CONST_IV					(1<<8)

#define DATA_SELECTION					(1<<2)
#define HASH_START						(1<<3)
#define HASH_INPUT_FINISHED			(1<<4)
#define SEED_SET_ENABLE				(1<<5)


SBLK oSblk;
DMAC oSdma1_RX;
DMAC oSdma1_TX;

//u32 g_pSrcAddr ;
#define g_SrcNCNBAddr 0x53000000 //NCNB
#define g_SrcCBAddr 0x51000000 //CB

extern u32 g_AES_Key;

/*
#define SDMA_TCOUT_INT_ENABLE		(1u<<31)

#define SDMA_DST_INC				(1<<27)
#define SDMA_SRC_INC				(1<<26)

#define SDMA_DST_TRANS_MASTER1		(0<<25)
#define SDMA_DST_TRANS_MASTER2		(1<<25)
#define SDMA_SRC_TRANS_MASTER1		(0<<24)
#define SDMA_SRC_TRANS_MASTER2		(1<<24)

#define SDMA_DST_TRANS_DWIDTH_9BIT	(0<<21)	
#define SDMA_DST_TRANS_DWIDTH_16BIT	(1<<21)	
#define SDMA_DST_TRANS_DWIDTH_32BIT	(2<<21)

#define SDMA_SRC_TRANS_DWIDTH_9BIT	(0<<18)	
#define SDMA_SRC_TRANS_DWIDTH_16BIT	(1<<18)	
#define SDMA_SRC_TRANS_DWIDTH_32BIT	(2<<18)

#define SDMA_DST_BURST_SIZE_1		(0<<15)	
#define SDMA_DST_BURST_SIZE_4		(1<<15)	
#define SDMA_DST_BURST_SIZE_8		(2<<15)	
#define SDMA_DST_BURST_SIZE_16		(3<<15)	
#define SDMA_DST_BURST_SIZE_32		(4<<15)	
#define SDMA_DST_BURST_SIZE_64		(5<<15)	
#define SDMA_DST_BURST_SIZE_128		(6<<15)	
#define SDMA_DST_BURST_SIZE_256		(7<<15)

#define SDMA_SRC_BURST_SIZE_1		(0<<12)	
#define SDMA_SRC_BURST_SIZE_4		(1<<12)	
#define SDMA_SRC_BURST_SIZE_8		(2<<12)	
#define SDMA_SRC_BURST_SIZE_16		(3<<12)	
#define SDMA_SRC_BURST_SIZE_32		(4<<12)	
#define SDMA_SRC_BURST_SIZE_64		(5<<12)	
#define SDMA_SRC_BURST_SIZE_128		(6<<12)	
#define SDMA_SRC_BURST_SIZE_256		(7<<12)

#define SDMA_TCOUT_INT_MASK			(1<<15)
#define SDMA_INT_ERROR_MASK			(1<<14)

#define SDMA_FLOW_MEM2MEM			(0<<11)
#define SDMA_FLOW_MEM2PERI			(1<<11)
#define SDMA_FLOW_PERI2MEM			(2<<11)
#define SDMA_FLOW_PERI2PERI			(3<<11)

#define SDMA_DST_PERI_SECURE_RX		(14<<6)
#define SDMA_SRC_PERI_SECURE_TX		(15<<1)
*/


//////////
// Function Name : SBLK_Reset
// Function Description : This function  reset control register of the security sub blocks
// Input : NONE 
// Output : NONE
// Version : 
void SBLK_Reset(void)
{
  	Outp32(rAES_RX_CTRL, 0x0);
  	Outp32(rDES_RX_CTRL, 0x0);
  	Outp32(rHASH_RX_CTRL, 0x0);
  	Outp32(rFIFO_RX_CTRL, 0x4);
  	Outp32(rFIFO_TX_CTRL, 0x4);
}


//////////
// Function Name : SBLK_Init
// Function Description : This function  initialize security sub blocks
// Input : 				eSblkType :  AES, DES, HASH
//						eOperMode : ECB, CBC, CTR, HMAC, SHA1, PRNG
//						eDirSel	 :   ENC, DEC
// Output : NONE
// Version : 
void SBLK_Init(SBLK_TYPE eSblkType, OPER_MODE eOperMode, DIR_SEL eDirSel)
{
	u32 uReg;
	u32 *pSrc;
	int i;

	oSblk.m_eSblkType = eSblkType;
	oSblk.m_eOperMode = eOperMode;

	SBLK_Reset();

	if (oSblk.m_eSblkType == AES)
	{
		Copy((u32)uAesKey, rAES_RX_KEY1, 4); 			// install aes key
		Copy((u32)uAesIV, rAES_RX_IV1, 4); 			// install aes iv
		Copy((u32)uAesInitCounter, rAES_RX_CTR1, 4); 	// install ctr data 

		uReg = (eDirSel == ENC)? (0<<3) : (1<<3);
		if (oSblk.m_eOperMode == ECB)
			uReg |= (1<<4);
		else if (oSblk.m_eOperMode == CBC)
			uReg |= (2<<4);
		else if (oSblk.m_eOperMode == CTR)
			uReg |= (3<<4);
		else
			Assert(0);
		Outp32(rAES_RX_CTRL, uReg);
	}
	else if (oSblk.m_eSblkType == DES)
	{
		Copy((u32)uDesKey, rDES_RX_KEY1_0, 6); 		// install des key
		Copy((u32)uDesIV, rDES_RX_IV0, 2); 			// install des iv

		uReg = ((eDirSel == ENC)? (0<<2) : (1<<2))|(0<<5)|(1<<1);
		if (oSblk.m_eOperMode == ECB)
			uReg |= (1<<3);
		else if (oSblk.m_eOperMode == CBC)
			uReg |= (2<<3);
		else
			Assert(0);
		Outp32(rDES_RX_CTRL, uReg);
	}
	else if (oSblk.m_eSblkType == HASH)
	{
		if (oSblk.m_eOperMode == HMAC)
		{
			Outp32(rHASH_RX_CTRL, HASH_START|(1<<2)|(0<<0)); // start + key mode

			pSrc = (u32 *)uHmacKey;
			for(i=0; i<16; i++)
				*(u32 *)rHASH_RX_DAT = *pSrc++;

			Outp32(rHASH_RX_CTRL, (0<<2)); // text mode
		}
		else if (oSblk.m_eOperMode == SHA1)
		{
			Outp32(rHASH_RX_CTRL, HASH_START|(1<<0));
		}
		else if(oSblk.m_eOperMode == PRNG)
		{
			Outp32(rHASH_RX_CTRL, SEED_SET_ENABLE|HASH_START|(2<<0));
			Copy((u32)uPrngSeed, rPRNG_SEED_DAT1, 10);
		}
		else
		{
			Assert(0);
		}
	}

	Outp32(rDNI_CFG, 0xFFFFFF00);
}

//////////
// Function Name : SBLK_SetFifo
// Function Description : This function  set Fifo information
// Input : 				uRxMsgLen : [31:0]
//						uRxBlkSz : [15:0]
//						uTxMsgLen : [31:0]
//						uTxBlkSz : [15:0]
//						eRxLValid : LASTBYTE_1ST, LASTBYTE_2ND, LASTBYTE_3RD, LASTBYTE_4TH ( SHA-1/PRNG)
// Output : NONE
// Version : 
void SBLK_SetFifo(u32 uRxMsgLen, u32 uRxBlkSz, u32 uTxMsgLen, u32 uTxBlkSz, LVALID_BYTE eRxLValid)
{
	u32 uReg;

	if (oSblk.m_eSblkType == AES)
	{
		Outp32(rFIFO_RX_MSG_LENGTH, uRxMsgLen);
		Outp32(rFIFO_RX_BLK_SIZE, uRxBlkSz);
		Outp32(rFIFO_RX_DST_ADDR, rAES_RX_DIN1);

		Outp32(rFIFO_TX_MSG_LENGTH, uTxMsgLen);
		Outp32(rFIFO_TX_BLK_SIZE, uTxBlkSz);
		Outp32(rFIFO_TX_SRC_ADDR, rAES_TX_DOUT1);
	}
	else if (oSblk.m_eSblkType == DES)
	{
		Outp32(rFIFO_RX_MSG_LENGTH, uRxMsgLen);
		Outp32(rFIFO_RX_BLK_SIZE, uRxBlkSz);
		Outp32(rFIFO_RX_DST_ADDR, rDES_RX_IN0);

		Outp32(rFIFO_TX_MSG_LENGTH, uTxMsgLen);
		Outp32(rFIFO_TX_BLK_SIZE, uTxBlkSz);
		Outp32(rFIFO_TX_SRC_ADDR, rDES_TX_OUT0);
	}
	else if (oSblk.m_eSblkType == HASH)
	{
		Outp32(rFIFO_RX_MSG_LENGTH, uRxMsgLen);
		uReg = 
			((eRxLValid == LASTBYTE_1ST) ? (0<<16) :
			(eRxLValid == LASTBYTE_2ND) ? (1<<16) :
			(eRxLValid == LASTBYTE_3RD) ? (2<<16) : (3<<16)) |
			(uRxBlkSz<<0);
	
		Outp32(rFIFO_RX_BLK_SIZE, uReg);
		Outp32(rFIFO_RX_DST_ADDR, rHASH_RX_DAT);

		Outp32(rFIFO_TX_MSG_LENGTH, uTxMsgLen);
		Outp32(rFIFO_TX_BLK_SIZE, uTxBlkSz);
		Outp32(rFIFO_TX_SRC_ADDR, rHASH_TX_OUT1);
	}
	else
		Assert(0);
}

//////////
// Function Name : SBLK_StartFifo
// Function Description : This function  FIFO Start
// Input :   NONE				
// Output : NONE
// Version : 
void SBLK_StartFifo(void)
{
	if (oSblk.m_eSblkType == AES)
	{
		Outp32(rFIFO_RX_CTRL, 0x39);		// FRx_Dest_Module= AES, FRx_Host_Rd_En, FRx_Host_Wr_En,  FRx_Sync_Tx, FRx_Start
		Outp32(rFIFO_TX_CTRL, 0x31);		//  FRx_Dest_Module= AES, FTx_Host_Rd_En, FTx_Host_Wr_En, FTx_Start
	}
	else if (oSblk.m_eSblkType == DES)
	{
		Outp32(rFIFO_RX_CTRL, 0x79);		// FRx_Dest_Module= DES, FRx_Host_Rd_En, FRx_Host_Wr_En,  FRx_Sync_Tx, FRx_Start
		Outp32(rFIFO_TX_CTRL, 0x71);		//  FRx_Dest_Module= AES, FTx_Host_Rd_En, FTx_Host_Wr_En, FTx_Start
	}
	else
	{
		Outp32(rFIFO_RX_CTRL, 0xb9);		// FRx_Dest_Module= SHA, FRx_Host_Rd_En, FRx_Host_Wr_En,  FRx_Sync_Tx, FRx_Start
		Outp32(rFIFO_TX_CTRL, 0xb1);		//  FRx_Dest_Module= SHA, FTx_Host_Rd_En, FTx_Host_Wr_En, FTx_Start
		
	}
}

//////////
// Function Name : SBLK_GetAvailableRxFifoSize
// Function Description : This function  read FRx_Wd2Write filed of the FIFO_RX_CTRL register.
// Input :   *uSize				
// Output : NONE
// Version : 
void SBLK_GetAvailableRxFifoSize(u32* uSize)
{
	u32 uRead;

	uRead = Inp32(rFIFO_RX_CTRL);

	//UART_Printf("rFIFO_RX_CTRL(GetAva):0x%x\n", uRead);
	uRead = (uRead>>16)&0xff;
	*uSize = uRead;
}

//////////
// Function Name : SBLK_PutDataToRxFifo
// Function Description : This function  put data to Rx FIFO from source address.
// Input :   uSrcAddr, uSize				
// Output : NONE
// Version : 
void SBLK_PutDataToRxFifo(u32 uSrcAddr, u32 uSize)
{	
	u32 i, uRemainedSize, uInputSize;
	u32 *pSrcAddr;

	
	pSrcAddr = (u32 *)uSrcAddr;
	uRemainedSize = uSize;

	while(uRemainedSize > 0)
	{
		SBLK_GetAvailableRxFifoSize(&uInputSize);

		if (uInputSize >= uRemainedSize)
		{
			uInputSize = uRemainedSize;
			uRemainedSize = 0;
		}
		else
		{
			uRemainedSize -= uInputSize;
		}

		for(i=0; i<uInputSize; i++)
			Outp32(rFIFO_RX_WR_BUF, *pSrcAddr++);
	}
}

//////////
// Function Name : SBLK_GetDataFromTxFifo
// Function Description : This function  get data from Tx FIFO to destination address.
// Input :   uDstAddr, uSize				
// Output : NONE
// Version : 
void SBLK_GetDataFromTxFifo(u32 uDstAddr, u32 uSize)
{
	u32 i;
	u32 *uPtrSrc, *uPtrDst;

	uPtrSrc = (u32 *)rFIFO_TX_RD_BUF;
	uPtrDst = (u32 *)uDstAddr;
	
	for(i=0; i<uSize; i++)
		*uPtrDst++ = *uPtrSrc++;
}

//////////
// Function Name : SBLK_IsFifoTransDone
// Function Description : This function   read "Done" field of the FIFO Tx Control Register.
// Input :   NONE			
// Output : true , false
// Version : 
u8 SBLK_IsFifoTransDone(void)
{
	u32 uRead;//, uRead0;

	//Delay(10);			// printf문 삽입시 필요 함.
	uRead=Inp32(rFIFO_TX_CTRL);
	//uRead0=Inp32(rFIFO_RX_CTRL); // For Test
	//UART_Printf("rFIFO_RX_CTRL : 0x%x\n", uRead);		// Test 3.17
	//UART_Printf("rDES_RX_CTRL : 0x%x\n", Inp32(0x7D200000));

	//For Test
	//SBLK_DispFifoRxCtrl(uRead0);
	//SBLK_DispFifoTxCtrl(uRead);
	
	if((uRead>>25)&1)
		return true;
	else
		return false;
}

//////////
// Function Name : SBLK_ClearIntPending
// Function Description : This function  clear int. pending of the SDMA1 Int.
// Input :   uTXnRX ( 1: Tx,  0: Rx)			
// Output : NONE
// Version : 
void SBLK_ClearIntPending(u32 uTXnRX)
{
	u32 uRead;
	
	if(uTXnRX == 1)
	{
		uRead = (1<<7);
		//DMACH_ClearIntPending(&oSdma1_TX);
	}
	else if (uTXnRX == 0)
	{
		uRead = (1<<6);
		//DMACH_ClearIntPending(&oSdma1_RX);
	}
	Outp32(0x7DC00008, uRead);

}

//////////
// Function Name : SBLK_ClearIntPending
// Function Description : This function  set SDMA1 
// Input :   uRxSrcAddr,uRxSize,uTxDstAddr,uTxSize			
// Output : NONE
// Version : 
void SBLK_SetSDMA(u32 uRxSrcAddr, u32 uRxSize, u32 uTxDstAddr, u32 uTxSize)
{
	u32 uReg;


	if (!(uRxSize%4)) uReg = 4<<1;
	else uReg = 0<<1;

	if (!(uTxSize%4)) uReg |= 4<<5;
	else uReg |= 0<<5;

 /*
	if (!(uRxSize%4)) uReg = 3<<1;
	else uReg = 0<<1;

	if (!(uTxSize%4)) uReg |= 3<<5;
	else uReg |= 0<<5;
*/

	Outp32(rDNI_CFG, 0xFFFFFF11|uReg);

	
	// Secure DMA input Selection
	SYSC_SelectDMA(eSEL_SECU_RX, 0);		// Secure Rx "SDMA1"
	SYSC_SelectDMA(eSEL_SECU_TX, 0);		// Secure Tx "SDMA1"
	DMAC_InitCh(SDMA1, DMA_G, &oSdma1_RX);	// SDMA1 Initialize
	DMAC_InitCh(SDMA1, DMA_H, &oSdma1_TX);

	Outp32(0x7DC00034, 0x0);					// SDMA1 DMA_SYNC


	// Rx Source Setting - using Channel 6 ( You can select any channel 0 to 7)
	// eChannel, LLI, SrcAddr, SrcFixed, DestAddr, DstFixed, eDataSize, DataCnt, eOpMode, eSrcReq, eDstReq, eBurstMode, DMAC
	//DMACH_Setup(DMA_G,  0x0,  uRxSrcAddr, 0, rSDMA1_RX_FIFO, 1, WORD, uRxSize, DEMAND, MEM, SDMA1_SECU_RX,  BURST8, &oSdma1_RX);
	DMACH_Setup(DMA_G,  0x0,  uRxSrcAddr, 0, rSDMA1_RX_FIFO, 1, WORD, uRxSize, DEMAND, MEM, SDMA1_SECU_RX,  BURST16, &oSdma1_RX);
	//DMACH_Setup(DMA_G,  0x0,  uRxSrcAddr, 0, rSDMA1_RX_FIFO, 1, WORD, uRxSize, DEMAND, MEM, SDMA1_SECU_RX,  BURST4, &oSdma1_RX);
	//DMACH_Setup(DMA_G,  0x0,  uRxSrcAddr, 0, rSDMA1_RX_FIFO, 1, WORD, uRxSize, DEMAND, MEM, SDMA1_SECU_RX,  SINGLE, &oSdma1_RX);
	
	// Tx Destination Setting - using Channel 7 (Tx)
	//DMACH_Setup(DMA_H,  0x0,  rSDMA1_TX_FIFO, 1, uTxDstAddr, 0, WORD, uTxSize, DEMAND, SDMA1_SECU_TX, MEM,  BURST8, &oSdma1_TX);	
	DMACH_Setup(DMA_H,  0x0,  rSDMA1_TX_FIFO, 1, uTxDstAddr, 0, WORD, uTxSize, DEMAND, SDMA1_SECU_TX, MEM,  BURST16, &oSdma1_TX);
	//DMACH_Setup(DMA_H,  0x0,  rSDMA1_TX_FIFO, 1, uTxDstAddr, 0, WORD, uTxSize, DEMAND, SDMA1_SECU_TX, MEM,  BURST4, &oSdma1_TX);	
	//DMACH_Setup(DMA_H,  0x0,  rSDMA1_TX_FIFO, 1, uTxDstAddr, 0, WORD, uTxSize, DEMAND, SDMA1_SECU_TX, MEM,  SINGLE, &oSdma1_TX);	

}

//////////
// Function Name : SBLK_StartSDMA
// Function Description : This function  start SDMA1
// Input :   NONE			
// Output : NONE
// Version : 
void SBLK_StartSDMA(void)
{

	DMACH_Start(&oSdma1_RX);
	DMACH_Start(&oSdma1_TX);

}

//////////
// Function Name : SBLK_PutDataToInReg
// Function Description : This function  
// Input :   uSrcAddr,	uSize,	eLValid		
// Output : NONE
// Version : 
void SBLK_PutDataToInReg(u32 uSrcAddr, u32 uSize, LVALID_BYTE eLValid)
{
	u32 i, uReg;
	u32 *pSrcAddr, *pDstAddr;

	pSrcAddr = (u32 *)uSrcAddr;

	if (oSblk.m_eSblkType == AES)
	{
		pDstAddr = (u32 *)rAES_RX_DIN1;
		for (i=uSize; i>0; i--)
			Outp32(pDstAddr++, *pSrcAddr++);
	}
	else if (oSblk.m_eSblkType == DES)
	{
		pDstAddr = (u32 *)rDES_RX_IN0;
		for (i=uSize; i>0; i--)
			Outp32(pDstAddr++, *pSrcAddr++);
	}
	else
	{
		for (i=uSize; i>0; i--)
		{
			if(i == 1)
			{
				uReg =
					((eLValid == LASTBYTE_1ST)? (0<<6) :
					(eLValid == LASTBYTE_2ND)? (1<<6) :
					(eLValid == LASTBYTE_3RD)? (2<<6) : (3<<6)) |
					HASH_INPUT_FINISHED |
					((oSblk.m_eOperMode == HMAC)? (0<<0) :
					(oSblk.m_eOperMode == SHA1)? (1<<0) : (2<<0));

				Outp32(rHASH_RX_CTRL, uReg); // inform hash of last data input
			}
			Outp32(rHASH_RX_DAT, *pSrcAddr++);
		}
	}
}

//////////
// Function Name : SBLK_StartByManual
// Function Description : This function  
// Input :   NONE	
// Output : NONE
// Version : 
void SBLK_StartByManual(void)
{
	u32 uRead;

	if (oSblk.m_eSblkType == AES)
	{
		uRead = Inp32(rAES_RX_CTRL);
		uRead |= (1<<0);
		Outp32(rAES_RX_CTRL, uRead);
	}
	else if (oSblk.m_eSblkType == DES)
	{
		uRead=Inp32(rDES_RX_CTRL);
		uRead |= (1<<0);
		Outp32(rDES_RX_CTRL, uRead);
	}
	else
	{
		uRead=Inp32(rHASH_RX_CTRL);
		uRead |= (1<<3);
		Outp32(rHASH_RX_CTRL, uRead);
	}
}


//////////
// Function Name : SBLK_GetDataFromOutReg
// Function Description : This function  
// Input :   uDstAddr, 	uSize
// Output : NONE
// Version :
void SBLK_GetDataFromOutReg(u32 uDstAddr, u32 uSize)
{
	u32 i;
	u32 *pSrc, *pDst;

	pDst = (u32 *)uDstAddr;

	if (oSblk.m_eSblkType == AES)
		pSrc = (u32 *)rAES_RX_DOUT1;
	else if (oSblk.m_eSblkType == DES)
		pSrc = (u32 *)rDES_RX_OUT0;
	else if (oSblk.m_eSblkType == HASH)
	{
		if (oSblk.m_eOperMode == PRNG)
			pSrc = (u32 *)rPRNG_TX_OUT1;
		else
			pSrc = (u32 *)rHASH_OUTPUT_01;
	}

	for(i=0; i<uSize; i++)
		*pDst++ = *pSrc++;
}

void SBLK_GetDataFromOutReg_Test(u32 uDstAddr, u32 uSize)
{
	u32 i;
	u32 *pSrc, *pDst;

	pDst = (u32 *)uDstAddr;

	if (oSblk.m_eSblkType == AES)
		pSrc = (u32 *)rAES_RX_DOUT1;
	else if (oSblk.m_eSblkType == DES)
		pSrc = (u32 *)rDES_RX_OUT0;
	else if (oSblk.m_eSblkType == HASH)
	{
		if (oSblk.m_eOperMode == PRNG)
			pSrc = (u32 *)rPRNG_TX_OUT1;
		else
			pSrc = (u32 *)rHASH_TX_OUT1;
	}

	for(i=0; i<uSize; i++)
		*pDst++ = *pSrc++;
}



//////////
// Function Name : SBLK_IsOutputReady
// Function Description : This function  
// Input :   NONE
// Output : true, false
// Version :
u8 SBLK_IsOutputReady(void)
{
	u32 uRead, uRet;

	if (oSblk.m_eSblkType == AES)
	{
		uRead=Inp32(rAES_RX_CTRL);
		uRet = (uRead>>10)&1;
	}
	else if (oSblk.m_eSblkType == DES)
	{
		uRead=Inp32(rDES_RX_CTRL);
		uRet = (uRead>>7)&1;
	}
	else if (oSblk.m_eSblkType == HASH)
	{
		uRead=Inp32(rHASH_RX_STAT);
		if (oSblk.m_eOperMode == PRNG)
			uRet = (uRead>>2)&1;
		else
			uRet = (uRead>>0)&1;
	}

	if(uRet)
		return true;
	else
		return false;
}

 void SBLK_DispFifoRxCtrl(u32 uFRx_Ctrl)

{

    //FIFO-RX

    UART_Printf("++++++++START FIFO-RX Ctrl Value++++++++++++++++++++++++++++++++++++++++++++++++++\n\n");

    UART_Printf("FRx_Start = %d    => It should be zero when  FIFO starts Transfer.\n",(uFRx_Ctrl>>0) & 0x1);

    UART_Printf("FRx_ERROR_En  = %d    => Enables ERROR response via HRESP port when host tries to access FIFO-RX.\n",(uFRx_Ctrl>>1) & 0x1);

    UART_Printf("          And Access is not enabled by FRx_Ctrl[4] 0r [5].\n");

    UART_Printf("FRx_Reset = %d    => Enables ERROR response via HRESP port when host tries to access FIFO-RX.\n",(uFRx_Ctrl>>2) & 0x1);

    UART_Printf("FRx_Sync_Tx   = %d    => Waits for FIFO_TX Done.\n",(uFRx_Ctrl>>3) & 0x1);

    UART_Printf("FRx_Host_Wr_En    = %d    => Enables Host write to FRx_WrBuf.\n",(uFRx_Ctrl>>4) & 0x1);

    UART_Printf("FRx_Host_Rd_En    = %d    => Enables Host read from FRx_Ctrl[31:16] and FRx_MLenCnt.\n",(uFRx_Ctrl>>5) & 0x1);

    UART_Printf("FRx_Dest_Module   = %d    => Destination Module Selection 0:AES, 1:DES, 2:SHA-1/PRNG,3:NA.\n",(uFRx_Ctrl>>6) & 0x3);

    UART_Printf("FRx_Wd2Read   = %d    => Number of words that can be read from FIFO Memory(FRx_WrBuf\n",(uFRx_Ctrl>>8) & 0xff);

    UART_Printf("FRx_Wd2Write  = %d    => Number of words that can be written to FIFO Memory(FRx_WrBuf\n",(uFRx_Ctrl>>16) & 0xff);

    UART_Printf("FRx_Running   = %d    => Set to 1 if FIFO-RX is transfering data to the destination or waiting for destination input beffer ready\n",(uFRx_Ctrl>>24) & 0x1);

    UART_Printf("          Set to 1 when FRx_Start bit resets to 0. ReadONly\n");

    UART_Printf("FRx_Done  = %d    => Set to 1 if FIFO-RX finished transfering data to the destination\n",(uFRx_Ctrl>>25) & 0x1);

    UART_Printf("FRx_Empty = %d    => Set to 1 if FIFO Buffer(FRx_WrBuf) is Empty\n",(uFRx_Ctrl>>26) & 0x1);

    UART_Printf("FRx_Full  = %d    => Set to 1 if FIFO Buffer(FRx_WrBuf) is Full\n",(uFRx_Ctrl>>27) & 0x1);

    UART_Printf("++++++++END FIFO-RX Ctrl Value++++++++++++++++++++++++++++++++++++++++++++++++++\n\n");

}

void SBLK_DispFifoTxCtrl(u32 uFTx_Ctrl)

{

    //FIFO-TX

    UART_Printf("++++++++START FIFO-TX Ctrl Value++++++++++++++++++++++++++++++++++++++++++++++++++\n\n");

    UART_Printf("FTx_Start = %d    => It should be zero when  FIFO starts Transfer.\n",(uFTx_Ctrl>>0) & 0x1);

    UART_Printf("FTx_ERROR_En  = %d    => Enables ERROR response via HRESP port when host tries to access FIFO-TX.\n",(uFTx_Ctrl>>1) & 0x1);

    UART_Printf("          And Access is not enabled by FTx_Ctrl[4] 0r [5].\n");

    UART_Printf("FTx_Reset = %d    => Enables ERROR response via HRESP port when host tries to access FIFO-TX.\n",(uFTx_Ctrl>>2) & 0x1);

    UART_Printf("FTx_Reserved  = %d    => Reserved.\n",(uFTx_Ctrl>>3) & 0x1);

    UART_Printf("FTx_Host_Wr_En    = %d    => Enables Host write to FRx_WrBuf.\n",(uFTx_Ctrl>>4) & 0x1);

    UART_Printf("FTx_Host_Rd_En    = %d    => Enables Host read from FTx_Ctrl[31:16] and FTx_MLenCnt.\n",(uFTx_Ctrl>>5) & 0x1);

    UART_Printf("FTx_Src_Module    = %d    => Destination Module Selection 0:AES, 1:DES, 2:SHA-1/PRNG,3:NA.\n",(uFTx_Ctrl>>6) & 0x3);

    UART_Printf("FTx_Wd2Write  = %d    => Number of words that can be read from FIFO Memory(FTx_WrBuf\n",(uFTx_Ctrl>>8) & 0xff);

    UART_Printf("FTx_Wd2Read   = %d    => Number of words that can be written to FIFO Memory(FTx_WrBuf\n",(uFTx_Ctrl>>16) & 0xff);

    UART_Printf("FTx_Running   = %d    => Set to 1 if FIFO-TX is transfering data to the destination or waiting for destination input beffer ready\n",(uFTx_Ctrl>>24) & 0x1);

    UART_Printf("          Set to 1 when FRx_Start bit resets to 0. ReadONly\n");

    UART_Printf("FTx_Done  = %d    => Set to 1 if FIFO-TX finished transfering data to the destination\n",(uFTx_Ctrl>>25) & 0x1);

    UART_Printf("FTx_Empty = %d    => Set to 1 if FIFO Buffer(FTx_WrBuf) is Empty\n",(uFTx_Ctrl>>26) & 0x1);

    UART_Printf("FTx_Full  = %d    => Set to 1 if FIFO Buffer(FTx_WrBuf) is Full\n",(uFTx_Ctrl>>27) & 0x1);

    UART_Printf("++++++++END FIFO-TX Ctrl Value++++++++++++++++++++++++++++++++++++++++++++++++++\n\n");

}

void SBLK_Init_Test(SBLK_TYPE eSblkType, OPER_MODE eOperMode, DIR_SEL eDirSel)
{
	u32 uReg;
	u32 *pSrc;
	int i;
	int uSel, uSel1;

	oSblk.m_eSblkType = eSblkType;
	oSblk.m_eOperMode = eOperMode;

	SBLK_Reset();

	if (oSblk.m_eSblkType == AES)
	{
	UART_Printf(" Choose AES Key Select :  0 : 128bit,  1:192bit,  2:256bits, 3: Counter  \n");
	uSel = UART_GetIntNum();

	 switch(uSel)
	 	{
		case 0:
	 		Copy((u32)uAesKey, rAES_RX_KEY1, 4); 			// install aes key
			Copy((u32)uAesIV, rAES_RX_IV1, 4); 			// install aes iv
			Copy((u32)uAesInitCounter, rAES_RX_CTR1, 4); 	// install ctr data 


			uReg = (eDirSel == ENC)? (0<<3) : (1<<3);
			if (oSblk.m_eOperMode == ECB)
				uReg |= (1<<4);
			else if (oSblk.m_eOperMode == CBC)
				uReg |= (2<<4);
			else if (oSblk.m_eOperMode == CTR)
				uReg |= (3<<4);
			else
				Assert(0);
			Outp32(rAES_RX_CTRL, uReg);
			break;

		case 1:
	 		Copy((u32)uAesKey, rAES_RX_KEY1, 4); 			// install aes key
	 		Copy((u32)uAesKey, rAES_RX_KEY5, 2); 			// install aes key
			Copy((u32)uAesIV, rAES_RX_IV1, 4); 			// install aes iv
			Copy((u32)uAesInitCounter, rAES_RX_CTR1, 4); 	// install ctr data 


			uReg = (eDirSel == ENC)? (0<<3) : (1<<3);
			uReg |= (1<<1);
			if (oSblk.m_eOperMode == ECB)
				uReg |= (1<<4);
			else if (oSblk.m_eOperMode == CBC)
				uReg |= (2<<4);
			else if (oSblk.m_eOperMode == CTR)
				uReg |= (3<<4);
			else
				Assert(0);
			Outp32(rAES_RX_CTRL, uReg);
			break;
		
		case 2:
	 		Copy((u32)uAesKey, rAES_RX_KEY1, 4); 			// install aes key
	 		Copy((u32)uAesKey, rAES_RX_KEY5, 4); 			// install aes key
			Copy((u32)uAesIV, rAES_RX_IV1, 4); 			// install aes iv
			Copy((u32)uAesInitCounter, rAES_RX_CTR1, 4); 	// install ctr data 


			uReg = (eDirSel == ENC)? (0<<3) : (1<<3);
			uReg |= (2<<1);
			if (oSblk.m_eOperMode == ECB)
				uReg |= (1<<4);
			else if (oSblk.m_eOperMode == CBC)
				uReg |= (2<<4);
			else if (oSblk.m_eOperMode == CTR)
				uReg |= (3<<4);
			else
				Assert(0);
			Outp32(rAES_RX_CTRL, uReg);
			break;

		case 3:
	 		Copy((u32)uAesKey, rAES_RX_KEY1, 4); 			// install aes key
	 		Copy((u32)uAesIV, rAES_RX_IV1, 4); 			// install aes iv
			Copy((u32)uAesInitCounter, rAES_RX_CTR1, 4); 	// install ctr data 
			UART_Printf(" Choose Counter :  0 : 16bit,  1:32bit,  2:64bits  \n");
			uSel1 = UART_GetIntNum();

			uReg = (eDirSel == ENC)? (0<<3) : (1<<3);
			uReg |= (uSel1<<6);
			if (oSblk.m_eOperMode == ECB)
				uReg |= (1<<4);
			else if (oSblk.m_eOperMode == CBC)
				uReg |= (2<<4);
			else if (oSblk.m_eOperMode == CTR)
				uReg |= (3<<4);
			else
				Assert(0);
			Outp32(rAES_RX_CTRL, uReg);
			UART_Printf("rAES_RX_CRT 0x%x\n", Inp32(0x7D100000));
			break;



	 	}


		
	}
	else if (oSblk.m_eSblkType == DES)
	{
		Copy((u32)uDesKey, rDES_RX_KEY1_0, 6); 		// install des key
		Copy((u32)uDesIV, rDES_RX_IV0, 2); 			// install des iv

		uReg = ((eDirSel == ENC)? (0<<2) : (1<<2))|(1<<5)|(1<<1);		// TDES
		if (oSblk.m_eOperMode == ECB)
			uReg |= (1<<3);
		else if (oSblk.m_eOperMode == CBC)
			uReg |= (2<<3);
		else
			Assert(0);
		Outp32(rDES_RX_CTRL, uReg);
	}
	else if (oSblk.m_eSblkType == HASH)
	{
		if (oSblk.m_eOperMode == HMAC)
		{
			Outp32(rHASH_RX_CTRL, HASH_START|(1<<2)|(0<<0)); // start + key mode

			pSrc = (u32 *)uHmacKey;
			for(i=0; i<16; i++)
				*(u32 *)rHASH_RX_DAT = *pSrc++;

			Outp32(rHASH_RX_CTRL, (0<<2)); // text mode
		}
		else if (oSblk.m_eOperMode == SHA1)
		{
			Outp32(rHASH_RX_CTRL, HASH_START|(1<<0));
		}
		else if(oSblk.m_eOperMode == PRNG)
		{
			Outp32(rHASH_RX_CTRL, SEED_SET_ENABLE|HASH_START|(2<<0));
			Copy((u32)uPrngSeed, rPRNG_SEED_DAT1, 10);
		}
		else
		{
			Assert(0);
		}
	}

	Outp32(rDNI_CFG, 0xFFFFFF00);
}


void SBLK_Init_Test2(SBLK_TYPE eSblkType, OPER_MODE eOperMode, DIR_SEL eDirSel)
{
	u32 uReg;
	u32 *pSrc;
	int i;

	oSblk.m_eSblkType = eSblkType;
	oSblk.m_eOperMode = eOperMode;

	SBLK_Reset();

	if (oSblk.m_eSblkType == AES)
	{
		

	 switch(g_AES_Key)
	 	{
		case 0:
	 		Copy((u32)uAesKey, rAES_RX_KEY1, 4); 			// install aes key
			Copy((u32)uAesIV, rAES_RX_IV1, 4); 			// install aes iv
			Copy((u32)uAesInitCounter, rAES_RX_CTR1, 4); 	// install ctr data 


			uReg = (eDirSel == ENC)? (0<<3) : (1<<3);
			if (oSblk.m_eOperMode == ECB)
				uReg |= (1<<4);
			else if (oSblk.m_eOperMode == CBC)
				uReg |= (2<<4);
			else if (oSblk.m_eOperMode == CTR)
				uReg |= (3<<4);
			else
				Assert(0);
			Outp32(rAES_RX_CTRL, uReg);

			//UART_Printf("Selected 128 Key Option \n");
			break;

		case 1:
	 		Copy((u32)uAesKey192, rAES_RX_KEY1, 6); 			// install aes key
	 		Copy((u32)uAesIV, rAES_RX_IV1, 4); 			// install aes iv
			Copy((u32)uAesInitCounter, rAES_RX_CTR1, 4); 	// install ctr data 


			uReg = (eDirSel == ENC)? (0<<3) : (1<<3);
			uReg |= (1<<1);
			if (oSblk.m_eOperMode == ECB)
				uReg |= (1<<4);
			else if (oSblk.m_eOperMode == CBC)
				uReg |= (2<<4);
			else if (oSblk.m_eOperMode == CTR)
				uReg |= (3<<4);
			else
				Assert(0);
			Outp32(rAES_RX_CTRL, uReg);
			//UART_Printf("Selected 196 Key Option \n");
			break;
		
		case 2:
	 		Copy((u32)uAesKey256, rAES_RX_KEY1, 8); 			// install aes key
	 		Copy((u32)uAesIV, rAES_RX_IV1, 4); 			// install aes iv
			Copy((u32)uAesInitCounter, rAES_RX_CTR1, 4); 	// install ctr data 


			uReg = (eDirSel == ENC)? (0<<3) : (1<<3);
			uReg |= (2<<1);
			if (oSblk.m_eOperMode == ECB)
				uReg |= (1<<4);
			else if (oSblk.m_eOperMode == CBC)
				uReg |= (2<<4);
			else if (oSblk.m_eOperMode == CTR)
				uReg |= (3<<4);
			else
				Assert(0);
			Outp32(rAES_RX_CTRL, uReg);
			//UART_Printf("Selected 256 Key Option \n");
			break;

	 	}


		
	}
	else if (oSblk.m_eSblkType == DES)
	{
		Copy((u32)uDesKey, rDES_RX_KEY1_0, 6); 		// install des key
		Copy((u32)uDesIV, rDES_RX_IV0, 2); 			// install des iv

		uReg = ((eDirSel == ENC)? (0<<2) : (1<<2))|(1<<5)|(1<<1);		// TDES
		if (oSblk.m_eOperMode == ECB)
			uReg |= (1<<3);
		else if (oSblk.m_eOperMode == CBC)
			uReg |= (2<<3);
		else
			Assert(0);
		Outp32(rDES_RX_CTRL, uReg);
	}
	else if (oSblk.m_eSblkType == HASH)
	{
		if (oSblk.m_eOperMode == HMAC)
		{
			Outp32(rHASH_RX_CTRL, HASH_START|(1<<2)|(0<<0)); // start + key mode

			pSrc = (u32 *)uHmacKey;
			for(i=0; i<16; i++)
				*(u32 *)rHASH_RX_DAT = *pSrc++;

			Outp32(rHASH_RX_CTRL, (0<<2)); // text mode
		}
		else if (oSblk.m_eOperMode == SHA1)
		{
			Outp32(rHASH_RX_CTRL, HASH_START|(1<<0));
		}
		else if(oSblk.m_eOperMode == PRNG)
		{
			Outp32(rHASH_RX_CTRL, SEED_SET_ENABLE|HASH_START|(2<<0));
			Copy((u32)uPrngSeed, rPRNG_SEED_DAT1, 10);
		}
		else
		{
			Assert(0);
		}
	}

	Outp32(rDNI_CFG, 0xFFFFFF00);
}

void SBLK_Init_Test3(SBLK_TYPE eSblkType, OPER_MODE eOperMode, DIR_SEL eDirSel)
{
	u32 uReg;
	u32 *pSrc;
	int i;

	oSblk.m_eSblkType = eSblkType;
	oSblk.m_eOperMode = eOperMode;

	SBLK_Reset();

	if (oSblk.m_eSblkType == AES)
	{
		

	 switch(g_AES_Key)
	 	{
		case 0:
	 		Copy((u32)uAesKey, rAES_RX_KEY1, 4); 			// install aes key
			Copy((u32)uAesIV, rAES_RX_IV1, 4); 			// install aes iv
			Copy((u32)uAesInitCounter_, rAES_RX_CTR1, 4); 	// install ctr data 


			uReg = (eDirSel == ENC)? (0<<3) : (1<<3);
			if (oSblk.m_eOperMode == ECB)
				uReg |= (1<<4);
			else if (oSblk.m_eOperMode == CBC)
				uReg |= (2<<4);
			else if (oSblk.m_eOperMode == CTR)
				uReg |= (3<<4);
			else
				Assert(0);
			Outp32(rAES_RX_CTRL, uReg);

			//UART_Printf("Selected 128 Key Option \n");
			break;

		case 1:
	 		Copy((u32)uAesKey192, rAES_RX_KEY1, 6); 			// install aes key
	 		Copy((u32)uAesIV, rAES_RX_IV1, 4); 			// install aes iv
			Copy((u32)uAesInitCounter_+0x10, rAES_RX_CTR1, 4); 	// install ctr data 


			uReg = (eDirSel == ENC)? (0<<3) : (1<<3);
			uReg |= (1<<1);
			uReg |= (1<<6);			// 32bit
			if (oSblk.m_eOperMode == ECB)
				uReg |= (1<<4);
			else if (oSblk.m_eOperMode == CBC)
				uReg |= (2<<4);
			else if (oSblk.m_eOperMode == CTR)
				uReg |= (3<<4);
			else
				Assert(0);
			Outp32(rAES_RX_CTRL, uReg);
			
			//UART_Printf("Selected 196 Key Option \n");
			break;
		
		case 2:
	 		Copy((u32)uAesKey256, rAES_RX_KEY1, 8); 			// install aes key
	 		Copy((u32)uAesIV, rAES_RX_IV1, 4); 			// install aes iv
			Copy((u32)uAesInitCounter_+0x30, rAES_RX_CTR1, 4); 	// install ctr data 


			uReg = (eDirSel == ENC)? (0<<3) : (1<<3);
			uReg |= (2<<1);
			uReg |= (2<<6);			// 64bit
			if (oSblk.m_eOperMode == ECB)
				uReg |= (1<<4);
			else if (oSblk.m_eOperMode == CBC)
				uReg |= (2<<4);
			else if (oSblk.m_eOperMode == CTR)
				uReg |= (3<<4);
			else
				Assert(0);
			Outp32(rAES_RX_CTRL, uReg);
			//UART_Printf("Selected 256 Key Option \n");
			break;

	 	}


		
	}
	else if (oSblk.m_eSblkType == DES)
	{
		Copy((u32)uDesKey, rDES_RX_KEY1_0, 6); 		// install des key
		Copy((u32)uDesIV, rDES_RX_IV0, 2); 			// install des iv

		uReg = ((eDirSel == ENC)? (0<<2) : (1<<2))|(1<<5)|(1<<1);		// TDES
		if (oSblk.m_eOperMode == ECB)
			uReg |= (1<<3);
		else if (oSblk.m_eOperMode == CBC)
			uReg |= (2<<3);
		else
			Assert(0);
		Outp32(rDES_RX_CTRL, uReg);
	}
	else if (oSblk.m_eSblkType == HASH)
	{
		if (oSblk.m_eOperMode == HMAC)
		{
			Outp32(rHASH_RX_CTRL, HASH_START|(1<<2)|(0<<0)); // start + key mode

			pSrc = (u32 *)uHmacKey;
			for(i=0; i<16; i++)
				*(u32 *)rHASH_RX_DAT = *pSrc++;

			Outp32(rHASH_RX_CTRL, (0<<2)); // text mode
		}
		else if (oSblk.m_eOperMode == SHA1)
		{
			Outp32(rHASH_RX_CTRL, HASH_START|(1<<0));
		}
		else if(oSblk.m_eOperMode == PRNG)
		{
			Outp32(rHASH_RX_CTRL, SEED_SET_ENABLE|HASH_START|(2<<0));
			Copy((u32)uPrngSeed, rPRNG_SEED_DAT1, 10);
		}
		else
		{
			Assert(0);
		}
	}

	Outp32(rDNI_CFG, 0xFFFFFF00);
}

void SBLK_Init_T(SBLK_TYPE eSblkType, OPER_MODE eOperMode, DIR_SEL eDirSel)
{
	u32 uReg;
	u32 *pSrc;
	int i;

	oSblk.m_eSblkType = eSblkType;
	oSblk.m_eOperMode = eOperMode;

	SBLK_Reset();

	if (oSblk.m_eSblkType == AES)
	{
		Copy((u32)uAesKey, rAES_RX_KEY1, 4); 			// install aes key
		Copy((u32)uAesIV, rAES_RX_IV1, 4); 			// install aes iv
		Copy((u32)uAesInitCounter, rAES_RX_CTR1, 4); 	// install ctr data 

		uReg = (eDirSel == ENC)? (0<<3) : (1<<3);
		if (oSblk.m_eOperMode == ECB)
			uReg |= (1<<4);
		else if (oSblk.m_eOperMode == CBC)
			uReg |= (2<<4);
		else if (oSblk.m_eOperMode == CTR)
			uReg |= (3<<4);
		else
			Assert(0);
		Outp32(rAES_RX_CTRL, uReg);
	}
	else if (oSblk.m_eSblkType == DES)
	{
		Copy((u32)uDesKey, rDES_RX_KEY1_0, 6); 		// install des key
		Copy((u32)uDesIV, rDES_RX_IV0, 2); 			// install des iv

		uReg = ((eDirSel == ENC)? (0<<2) : (1<<2))|(0<<5)|(1<<1);
		if (oSblk.m_eOperMode == ECB)
			uReg |= (1<<3);
		else if (oSblk.m_eOperMode == CBC)
			uReg |= (2<<3);
		else
			Assert(0);
		Outp32(rDES_RX_CTRL, uReg);
	}
	else if (oSblk.m_eSblkType == HASH)
	{
		if (oSblk.m_eOperMode == HMAC)
		{
			Outp32(rHASH_RX_CTRL, HASH_START|(1<<2)|(0<<0)); // start + key mode

			pSrc = (u32 *)uHmacKey;
			for(i=0; i<16; i++)
				*(u32 *)rHASH_RX_DAT = *pSrc++;

			Outp32(rHASH_RX_CTRL, (0<<2)); // text mode
		}
		else if (oSblk.m_eOperMode == SHA1)
		{
			Outp32(rHASH_RX_CTRL, HASH_START|(1<<8)|(1<<0));
		}
		else if(oSblk.m_eOperMode == PRNG)
		{
			Outp32(rHASH_RX_CTRL, SEED_SET_ENABLE|HASH_START|(2<<0));
			Copy((u32)uPrngSeed, rPRNG_SEED_DAT1, 10);
		}
		else
		{
			Assert(0);
		}
	}

	Outp32(rDNI_CFG, 0xFFFFFF00);
}


void SBLK_PutDataToInReg1(u32 uSrcAddr, u32 uSize, LVALID_BYTE eLValid)
{
	u32 i, uReg;
	u32 *pSrcAddr, *pDstAddr;

	pSrcAddr = (u32 *)uSrcAddr;

	if (oSblk.m_eSblkType == AES)
	{
		pDstAddr = (u32 *)rAES_RX_DIN1;
		for (i=uSize; i>0; i--)
			Outp32(pDstAddr++, *pSrcAddr++);
	}
	else if (oSblk.m_eSblkType == DES)
	{
		pDstAddr = (u32 *)rDES_RX_IN0;
		for (i=uSize; i>0; i--)
			Outp32(pDstAddr++, *pSrcAddr++);
	}
	else
	{
		UART_Printf("SHA1 Intermediate\n");
		Outp32(rHASH_RX_CTRL, 0x109);
		for (i=uSize; i>0; i--)
		{
		//Outp32(rHASH_RX_CTRL, 0x09);	
			if(i == 1)
			{
				uReg =
					((eLValid == LASTBYTE_1ST)? (0<<6) :
					(eLValid == LASTBYTE_2ND)? (1<<6) :
					(eLValid == LASTBYTE_3RD)? (2<<6) : (3<<6)) |
					HASH_INPUT_FINISHED |
					((oSblk.m_eOperMode == HMAC)? (0<<0) :
					(oSblk.m_eOperMode == SHA1)? (1<<0) : (2<<0));

				uReg |= (1<<8);	
				UART_Printf("RX_CTRL = 0x%x\n",uReg);
				
				Outp32(rHASH_RX_CTRL, 0xD1);
				//Outp32(rHASH_RX_CTRL, uReg); // inform hash of last data input
			}
			Outp32(rHASH_RX_DAT, *pSrcAddr++);
			
		}
	}
}

