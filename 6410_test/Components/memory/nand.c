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
*	File Name : nand.c
*  
*	File Description : This file implements the API functons for Nand controller.
*
*	Author : Heemyung.noh
*	Dept. : AP Development Team
*	Created Date : 2006/12/05
*	Version : 0.1 
* 
*	History
*	- Created(Heemyung.noh 2006/12/05)
*  
**************************************************************************************/

#include <stdio.h>

#include "def.h"
#include "option.h"
#include "library.h"
#include "sfr6410.h"
#include "system.h"
#include "sysc.h"
#include "intc.h"
#include "nand.h"
#include "dma.h"
#include "gpio.h"


#define NAND_REG_BUG		(FALSE)
//#define NAND_EVT1			(TRUE)

#define NAND(__n) 		( ( volatile oNAND_REGS * ) ( NAND_pBase[__n] ) )

typedef struct tag_NAND_REGS
{
	u32 rNFCONF;
	u32 rNFCONT;
	u32 rNFCMMD;
	u32 rNFADDR;
	u32 rNFDATA;
	u32 rNFMECCD0;
	u32 rNFMECCD1;
	u32 rNFSECCD;
	u32 rNFSBLK;
	u32 rNFEBLK;
	u32 rNFSTAT;
	u32 rNFECCERR0;
	u32 rNFECCERR1;
	u32 rNFMECC0;
	u32 rNFMECC1;
	u32 rNFSECC;
	u32 rNFMLCBITPT;
	u32 rNF8ECCERR0;
	u32 rNF8ECCERR1;
	u32 rNF8ECCERR2;
	u32 rNFM8ECC0;
	u32 rNFM8ECC1;
	u32 rNFM8ECC2;
	u32 rNFM8ECC3;
	u32 rNFMLC8BITPT0;
	u32 rNFMLC8BITPT1;
} 
oNAND_REGS;

static void *NAND_pBase[NAND_CONNUM];
static u8 aNand_Spare_Data[NAND_SPARE_MAX] = {	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
												0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
												0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
												0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
static u8 aNand_Spare_Data_Temp[NAND_SPARE_MAX] = {	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
												0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
												0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
												0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

static volatile u32 g_Nand_RnBTransition, g_Nand_4bitEccEncDone, g_Nand_4bitEccDecDone, g_Nand_IllegalAccError;
NAND_oInform NAND_Inform[NAND_CONNUM];
NAND_oEccError NAND_EccError;
volatile u32 g_Nand_IllegalAccErrorforTest;
volatile u32 gNandAutoTest=0;



#if (NAND_TRANSFER_MODE == DMA_TRANSFER)
	static volatile u32 Nand_DmaDone;
#endif
	
DMAC g_oNandDmac0;
DMAC g_oNandDmac1;

//////////
// Function Name : NAND_Init
// Function Description : This function initializes a certain Nand Controller
// Input : 	Controller - Nand Controller Port Number 
// Output : 	TRUE - Memory Device is initialized
//			FALSE - Memory Device is not initialized
u8	NAND_Init(u32 Controller)
{
	u32 uBaseAddress, uTemp;
	u32 uTacls, uTwrph0, uTwrph1;

	if(Controller == 0)
	{
	uBaseAddress = NFCON_BASE;
	}
	else		
	{
		// S3C6410 has 1 Nand Controller
		return FALSE;
	}
	
	NAND_pBase[Controller] = (void *)uBaseAddress;

	INTC_SetVectAddr(NUM_NFC, NAND_ISR0);
	
	NAND_GetDurationValue(Controller, &uTacls, &uTwrph0, &uTwrph1);

// for test
	#if 0 
	uTacls = 7;
	uTwrph0 = 7;
	uTwrph1 =7;
	#endif
	

	if(NAND_Inform[Controller].uNandType == NAND_Normal8bit)
	{
		Outp32(&NAND(Controller)->rNFCONF, (uTacls<<12)|(uTwrph0<<8)|(uTwrph1<<4)|(0<<3)|(1<<2)|((NAND_Inform[Controller].uAddrCycle-3)<<1));
		Outp32(&NAND(Controller)->rNFCONT, (0<<17)|(0<<16)|(0<<10)|(0<<9)|(0<<8)|(1<<7)|(1<<6)|(1<<5)|(1<<4)|(0x3<<1)|(1<<0));
	}
	else if(NAND_Inform[Controller].uNandType == NAND_Advanced8bit)
	{
		Outp32(&NAND(Controller)->rNFCONF, (uTacls<<12)|(uTwrph0<<8)|(uTwrph1<<4)|(1<<3)|(1<<2)|((NAND_Inform[Controller].uAddrCycle-4)<<1));
		Outp32(&NAND(Controller)->rNFCONT, (0<<17)|(0<<16)|(0<<10)|(0<<9)|(0<<8)|(1<<7)|(1<<6)|(1<<5)|(1<<4)|(0x3<<1)|(1<<0));
	}
	else if(NAND_Inform[Controller].uNandType == NAND_MLC8bit)
	{
		Outp32(&NAND(Controller)->rNFCONF, (1<<24)|(uTacls<<12)|(uTwrph0<<8)|(uTwrph1<<4)|(1<<3)|(1<<2)|((NAND_Inform[Controller].uAddrCycle-4)<<1));
		Outp32(&NAND(Controller)->rNFCONT, (0<<17)|(0<<16)|(0<<10)|(0<<9)|(0<<8)|(1<<7)|(1<<6)|(1<<5)|(1<<4)|(0x3<<1)|(1<<0));
	}
	else
	{
		return FALSE;
	}		

	#if 1 
	uTemp = Inp32(&(NAND(Controller)->rNFCONF));
	if(g_HCLK  > 66000000)
		uTemp &= ~(1<<30);
	else
		uTemp |= (1<<30);
	Outp32(&(NAND(Controller)->rNFCONF), uTemp);
	#endif
	

	NAND_Reset(Controller);

#if NAND_TRANSFER_MODE == DMA_TRANSFER
#if 0
		DMAC_InitCh(DMA0, DMA_ALL, &g_oNandDmac1);
		INTC_SetVectAddr(NUM_DMA0,  NAND_DmaISR);
		INTC_Enable(NUM_DMA0);
#else	
		DMAC_InitCh(SDMA0, DMA_ALL, &g_oNandDmac1);
		INTC_SetVectAddr(NUM_SDMA0,  NAND_DmaISR);
		INTC_Enable(NUM_SDMA0);	
#endif		
		//SYSC_SelectDMA(eSEL_PWM, 1);

#endif	

	//GPIO_SetMem0DrvStrength(0xffffffff);
	
	return TRUE;
}


void	NAND_GetDurationValue(u32 Controller, u32 *uTacls, u32 *uTwrph0, u32 *uTwrph1)
{
	u32 uTemp;
	float fTemp;

	uTemp = (NAND_Inform[Controller].uTacls * (g_HCLK/1000))/1000000;
	fTemp = (((float)NAND_Inform[Controller].uTacls * (float)(g_HCLK/1000))/(float)1000000);
	fTemp -= uTemp;

	if(fTemp != (float)0)
		*uTacls = uTemp+1;
	else
		*uTacls = uTemp;

	uTemp = (NAND_Inform[Controller].uTwrph0 * (g_HCLK/1000))/1000000;
	fTemp = (((float)NAND_Inform[Controller].uTwrph0 * (float)(g_HCLK/1000))/(float)1000000);
	fTemp -= uTemp;

	if(fTemp != (float)0)
		*uTwrph0 = uTemp;
	else
	{
		if(uTemp > 0)
			*uTwrph0 = uTemp-1;
		else
			*uTwrph0 = 0;
	}
	
	uTemp = (NAND_Inform[Controller].uTwrph1 * (g_HCLK/1000))/1000000;
	fTemp = (((float)NAND_Inform[Controller].uTwrph1 * (float)(g_HCLK/1000))/(float)1000000);
	fTemp -= uTemp;

	if(fTemp != (float)0)
		*uTwrph1 = uTemp;
	else
	{
		if(uTemp > 0)
			*uTwrph1 = uTemp-1;
		else
			*uTwrph1 = 0;
	}
}
	

//////////
// Function Name : NAND_TimingParaSetting
// Function Description : This function set the Timing Parameter(TACLS, TWRPH0, TWRPH1)
// Input : 	Controller - Nand Controller Port Number 
// Output : 	None
void NAND_TimingParaSetting(u32 Controller)
{
	u32 uTacls, uTwrph0, uTwrph1, uTemp0, uTemp1;

	NAND_GetDurationValue(Controller, &uTacls, &uTwrph0, &uTwrph1);

	uTemp0 = Inp32(&NAND(Controller)->rNFCONF);
	uTemp0 = (uTemp0 & ~((0x07<<12)|(0x07<<8)|(0x07<<4))) | ((uTacls<<12)|(uTwrph0<<8)|(uTwrph1<<4));
//	uTemp0 = (uTemp0 & ~((0x07<<12)|(0x07<<8)|(0x07<<4))) | ((7<<12)|(7<<8)|(7<<4));	
	Outp32(&NAND(Controller)->rNFCONF, uTemp0);

	 if(NAND_Inform[Controller].uNandType == NAND_MLC8bit)
	{
		uTemp1 = Inp32(&(NAND(Controller)->rNFCONF));
		if(g_HCLK  > 66000000)
			uTemp1 &= ~(1<<30);
		else
			uTemp1 |= (1<<30);
		Outp32(&(NAND(Controller)->rNFCONF), uTemp1);
	}

	UART_Printf("Nand Timing Parameter : TACLS(%d Clock), TWRPH0(%d Clock), TWRPH1(%d Clock)\n", uTacls, uTwrph0+1, uTwrph1+1);
}



//////////
// Function Name : NAND_ISR
// Function Description : Nand IRQ routine
// Input : 	None
// Output : 	None
void __irq NAND_ISR0(void)
{
	u32 uIntSource;
	
	// rb1004 : must be modified
	//INT_Disable1(BIT_NUM_NFC);
	INTC_Disable(NUM_NFC);

	uIntSource = Inp32(&NAND(0)->rNFSTAT);

	if(uIntSource & NAND_4BITECC_ENC)
	{
		Outp32(&NAND(0)->rNFSTAT, (1<<7));
		g_Nand_4bitEccEncDone = 1;
	}
	else if(uIntSource & NAND_4BITECC_DEC)
	{
		Outp32(&NAND(0)->rNFSTAT, (1<<6));
		g_Nand_4bitEccDecDone = 1;
	}
	else if(uIntSource & NAND_ILLEGAL_ACCESS)
	{
		Outp32(&NAND(0)->rNFSTAT, (1<<5));
		g_Nand_IllegalAccError = 1;
		g_Nand_IllegalAccErrorforTest = 1;
		if(gNandAutoTest == 0)
		{
			UART_Printf("NAND Illegal Access Error.........!!\n");
			UART_GetKey();
		}
	}
	else if(uIntSource & NAND_RnB_TRANS)
	{
		Outp32(&NAND(0)->rNFSTAT, (1<<4));
		g_Nand_RnBTransition = 1;
	}

	// rb1004 : must be modified
	//INT_Enable1(BIT_NUM_NFC);
	//Write_VECTADDR(0x0);	
	INTC_Enable(NUM_NFC);
	INTC_ClearVectAddr();
}

#if 0 // for FPBA Board
static void Isr_NAND(void)
{
	u32 uIntSource;
	
	INTC_Mask(NUM_NFC);
	INTC_ClearPending(NUM_NFC);

	uIntSource = Inp32(&NAND(0)->rNFSTAT);

	if(uIntSource & NAND_4BITECC_ENC)
	{
		Outp32(&NAND(0)->rNFSTAT, (1<<7));
		g_Nand_4bitEccEncDone = 1;
	}

	if(uIntSource & NAND_4BITECC_DEC)
	{
		Outp32(&NAND(0)->rNFSTAT, (1<<6));
		g_Nand_4bitEccDecDone = 1;
	}

	if(uIntSource & NAND_ILLEGAL_ACCESS)
	{
		Outp32(&NAND(0)->rNFSTAT, (1<<5));
		g_Nand_IllegalAccError = 1;
		g_Nand_IllegalAccErrorforTest = 1;
		if(gNandAutoTest == 0)
		{
			UART_Printf("NAND Illegal Access Error.........!!\n");
			UART_GetKey();
		}
	}

	if(uIntSource & NAND_RnB_TRANS)
	{
		Outp32(&NAND(0)->rNFSTAT, (1<<4));
		g_Nand_RnBTransition = 1;
	}

	INTC_Unmask(NUM_NFC);
	INTC_ClearVectAddr();

	
}
#endif

//////////
// Function Name : NAND_DmaISR
// Function Description : Nand DMA transfer IRQ routine
// Input : 	None
// Output : 	None
#if (NAND_TRANSFER_MODE == DMA_TRANSFER)

	#if 1
void __irq NAND_DmaISR(void) // NonVIC
{
  	DMACH_ClearIntPending(&g_oNandDmac1);

	//UART_Printf ("DMA ISR %d\n", Nand_DmaDone);

	Nand_DmaDone = 1;
	INTC_ClearVectAddr();
}
	#else
static void ISR_NANDDMA(void)
{
	#if 0 
	INTC_Mask(INT_DMA0);
	INTC_ClearPending(INT_DMA0);
	*(unsigned int*)(0x75000008) = 0xf; // DMA Interrupt Clear Normal DMA0
	*(unsigned int*)(0x75000010) = 0xf; // DMA Interrupt Clear Error
	UART_Printf ("DMA ISR %d\n", Nand_DmaDone);
	Nand_DmaDone = true;
	INTC_Unmask(INT_DMA0);
	#else
	INTC_Mask(NUM_SDMA0);
	INTC_ClearPending(NUM_SDMA0);
	*(unsigned int*)(0x7db00008) = 0xf; // DMA Interrupt Clear SDMA0
	*(unsigned int*)(0x7db00010) = 0xf; // DMA Interrupt Clear SDMA0	
	UART_Printf ("DMA ISR %d\n", Nand_DmaDone);
	Nand_DmaDone = true;
	INTC_Unmask(NUM_SDMA0);
	#endif	
}
	#endif
	
#endif

//////////
// Function Name : NAND_ReadID
// Function Description : This function get the ID of external NAND Device
// Input : 	Controller - Nand Controller Port Number 
// Output : 	Nand Memory ID
u32 NAND_ReadID(u32 Controller)
{
	u32 i;
	//u8 ucID1, ucID2, ucID3, ucID4;
	u32 usID;

	NF_nFCE_L(Controller);
	
	NF_CMD(Controller, 0x90);
	NF_ADDR(Controller, 0x0);

	for (i=0; i<10; i++);

#if 0
	ucID1 = (u8)NF_RDDATA8(Controller);	// read 4byte.
	ucID2 = (u8)NF_RDDATA8(Controller);
	ucID3 = (u8)NF_RDDATA8(Controller);
	ucID4 = (u8)NF_RDDATA8(Controller);
	usID = (ucID4<<24)|(ucID3<<16)|(ucID2<<8)|(ucID1<<0);
#else
	usID = NF_RDDATA(Controller);
#endif

	NF_nFCE_H(Controller);
	
	return 	usID;	
}

//////////
// Function Name : NAND_Reset
// Function Description : This function reset the external NAND Device
// Input : 	Controller - Nand Controller Port Number 
// Output : 	None
void NAND_Reset(u32 Controller)
{
    	u32 i;
   
	NF_nFCE_L(Controller);

	NF_CLEAR_RnB(Controller);
	NF_CMD(Controller, 0xFF);	//reset command

	if(NAND_Inform[Controller].uNandType == NAND_Normal8bit)
	{
		for(i=0;i<1000;i++);
	}
	else
	{
		for(i=0;i<10;i++);  //tWB = 100ns. //??????
		NF_DETECT_RnB(Controller);
	}
	NF_nFCE_H(Controller);
}

//////////
// Function Name : NAND_CheckInvalidBlock
// Function Description : This function check the invalid block of external NAND device
// Input : 	Controller - Nand Controller Port Number 
//			uBlock - Check Block
// Output : 	Nand Error Type
NAND_eERROR NAND_CheckInvalidBlock(u32 Controller, u32 uBlock)
{
	u32 uBlockPage, uPageSize;
	u8 ucData;
//	u32 uTemp;
	g_Nand_RnBTransition =0;
		
	INTC_Disable(NUM_NFC);
	
	if(NAND_Inform[Controller].uNandType == NAND_Normal8bit)
	{
		uBlockPage=(uBlock<<5);
		//uPageSize = NAND_PAGE_512;
		uPageSize = NAND_Inform[Controller].uPageSize;
	}
	else if(NAND_Inform[Controller].uNandType == NAND_Advanced8bit)
	{
		uBlockPage=(uBlock<<6);
		//uPageSize = NAND_PAGE_2048;
		uPageSize = NAND_Inform[Controller].uPageSize;
	}
	else if(NAND_Inform[Controller].uNandType == NAND_MLC8bit)
	{
		uBlockPage=(uBlock<<7);
		//uPageSize = NAND_PAGE_2048;
		uPageSize = NAND_Inform[Controller].uPageSize;
	}
	else
		return eNAND_EtcError;

	NF_nFCE_L(Controller);
	NF_CLEAR_RnB(Controller);

	//if(NAND_Inform[Controller].uNandType == NAND_Normal8bit)
	if(NAND_Inform[Controller].uAddrCycle == 4)
	{
		NF_CMD(Controller, 0x50);		 		// Spare array read command
		NF_ADDR(Controller, (uPageSize+5)&0xf);	// 6th byte in the Spare Area
	}
	else	 if(NAND_Inform[Controller].uAddrCycle == 5)
	{
		NF_CMD(Controller, 0x00);				// 1st Read Command
		NF_ADDR(Controller, (uPageSize+0)&0xff);			
		NF_ADDR(Controller, ((uPageSize+0)>>8)&0xff);		// 1st byte in the Spare Area		
	}			

	// Address Page Address : Block = 32Page : Device - 4096 Block = 4096*32Page = 2^17 so, address need 3Cycle. 
	NF_ADDR(Controller, uBlockPage&0xff);	 // The mark of bad block is in 0 page
	NF_ADDR(Controller, (uBlockPage>>8)&0xff);	 // For block number A[24:17]
	NF_ADDR(Controller, (uBlockPage>>16)&0xff);  // For block number A[25]

	if((NAND_Inform[Controller].uNandType == NAND_Advanced8bit) || (NAND_Inform[Controller].uNandType == NAND_MLC8bit) )
		NF_CMD(Controller, 0x30);	// 2'nd command
	 
	NF_DETECT_RnB(Controller);	 // Wait tR(max 12us)
	NF_CLEAR_RnB(Controller);


	ucData=NF_RDDATA8(Controller);

	NF_CMD(Controller, 0x00);	// Define the starting address of the 1st half of the register

	NF_nFCE_H(Controller);    

	if(ucData!=0xff)
	{
		//UART_Printf("[block %d has been marked as a bad block(%x)]\n",uBlock,ucData);
		return eNAND_InvalidBlock;
	}
	else
	{
		return eNAND_NoError;
	}
}



//////////
// Function Name : NAND_CheckECCError
// Function Description : This function check the ECC Error
// Input : 	Controller - Nand Controller Port Number 
// Output :  	ECC Error Type 
NAND_eERROR NAND_CheckECCError(u32 Controller)
{
	u32 uEccError0;
	//u32 uEccError1;
	NAND_eERROR eError;	

	eError = eNAND_NoError;

	if((NAND_Inform[Controller].uNandType == NAND_Normal8bit) || (NAND_Inform[Controller].uNandType == NAND_Advanced8bit) )
	{
		uEccError0 = Inp32(&NAND(Controller)->rNFECCERR0);
		switch (uEccError0 & 0x03)
		{
			case 0x00 : 	eError = eNAND_NoError;
						break;
			case 0x01 : 	eError = eNAND_1bitEccError;
						break;
			case 0x02 : 	eError = eNAND_MultiError;
						break;
			case 0x03 : 	eError = eNAND_EccAreaError;
						break;
		}

		switch ((uEccError0 & 0x0C)>>2)
		{
			case 0x00 : 	eError |= eNAND_NoError;
						break;
			case 0x01 : 	eError |= eNAND_Spare1bitEccError;
						break;
			case 0x02 : 	eError |= eNAND_SpareMultiError;
						break;
			case 0x03 : 	eError |= eNAND_SpareEccAreaError;
						break;
		}	

		if(NAND_Inform[Controller].uPerformanceCheck == 0)
		{
			switch ((uEccError0 & 0x0C)>>2)
			{
				case 0x00 : 	eError |= eNAND_NoError;
							break;
				case 0x01 : 	eError |= eNAND_Spare1bitEccError;
							break;
				case 0x02 : 	eError |= eNAND_SpareMultiError;
							break;
				case 0x03 : 	eError |= eNAND_SpareEccAreaError;
							break;
			}	
		}
	}
	else if(NAND_Inform[Controller].uNandType == NAND_MLC8bit)
	{	
		uEccError0 = Inp32(&NAND(Controller)->rNFECCERR0);
		//uEccError1 = Inp32(&NAND(Controller)->rNFECCERR1);	
		switch ((uEccError0>>26) & 0x07)
		{
			case 0x00 : 	eError = eNAND_NoError;
						break;
			case 0x01 : 	eError = eNAND_1bitEccError;
						break;
			case 0x02 : 	eError = eNAND_2bitEccError;
						break;
			case 0x03 : 	eError = eNAND_3bitEccError;
						break;
			case 0x04 : 	eError = eNAND_4bitEccError;
						break;
			case 0x05 : 	eError = eNAND_UncorrectableError;
						break;
		}
	}

	return eError;

}





//////////
// Function Name : NAND_SeekECCErrorPosition
// Function Description : This function seek the ECC error position
// Input : 	Controller - Nand Controller Port Number 
//			uEccErr0 - the variable address to return the rNFECCERR0 register value
//			uEccErr1 - the variable address to return the rNFECCERR1 register value
//			uEccErr2 - the variable address to return the rNFMLCBITPT register value
// Output : 	ECC error register0/1 & Bit Pattern register value
void NAND_SeekECCErrorPosition(u32 Controller, u32 *uEccErr0, u32 *uEccErr1, u32 *uEccErr2)
{
	if(NAND_Inform[Controller].uNandType == NAND_MLC8bit)
	{
		*uEccErr0 = Inp32(&(NAND(Controller)->rNFECCERR0));
		*uEccErr1 = Inp32(&(NAND(Controller)->rNFECCERR1));
		*uEccErr2 = Inp32(&(NAND(Controller)->rNFMLCBITPT));
	}
	else
	{
		*uEccErr0 = Inp32(&(NAND(Controller)->rNFECCERR0));
	}
}

//////////
// Function Name : NAND_ReadPageSLC
// Function Description : This function read the 1 page data of external SLC Type NAND device
// Input : 	Controller - Nand Controller Port Number 
//			uBlock - Read Block
//			uPage - Read Page
//			pBuffer - Read data buffer
// Output : 	ECC Error Type 
NAND_eERROR NAND_ReadPageSLC(u32 Controller, u32 uBlock, u32 uPage, u8 *pBuffer, u8 *pSpareBuffer)
{
	u32 i, uBlockPage, uPageSize;
	u32 uMecc, uSecc;
	NAND_eERROR eError;	
	u32 *pBufferW;
	u32 uNFCONFreg, uNFCONTreg;
	u32 uTemp0;
			
	pBufferW = (u32 *)pBuffer;			
	g_Nand_RnBTransition = 0;

	// Setting NFCONF Register.
	uNFCONFreg = Inp32(&(NAND(Controller)->rNFCONF));
	uNFCONFreg = uNFCONFreg & (~(0x1<<30)); // HCLK is more than 66Mhz.
	uNFCONFreg = uNFCONFreg & (~(0x3<<23)); // 1bit ECC Select.
	uNFCONFreg = uNFCONFreg | ((7<<12)|(7<<8)|(7<<4)) ; // Timing 7,7,7 setting. add. jungil 
	Outp32(&(NAND(Controller)->rNFCONF), uNFCONFreg ); // NFCONF Register setting. 
	// Setting NFCONT Register.
	uNFCONTreg = Inp32(&(NAND(Controller)->rNFCONT)); // read NFCON
	uNFCONTreg &= ~((1<<13)|(1<<12));		// 4bit ECC encoding/decoding completion Interrupt disable
	uNFCONTreg |= (1<<10) | (1<<9);		// Illegal Access & RnB Transition Interrupt enable
	Outp32(&(NAND(Controller)->rNFCONT), uNFCONTreg); // write setting. NFCONT Register
	Outp32(&(NAND(Controller)->rNFSTAT), (1<<5)|(1<<4)); // Illegal Access & RnB Interrupt Pending bit clear

	INTC_Enable(NUM_NFC); // Interrupt Enable.

	if(NAND_Inform[Controller].uNandType == NAND_Normal8bit)
	{
		uBlockPage=(uBlock<<5) + uPage;
		uPageSize = NAND_Inform[Controller].uPageSize;
	}
	else //if(NAND_Inform[Controller].uNandType == NAND_Advanced8bit)
	{
		uBlockPage=(uBlock<<6) + uPage;
		uPageSize = NAND_Inform[Controller].uPageSize;
	}

	NF_RSTECC(Controller);    // Initialize ECC
	NF_MECC_UnLock(Controller); // Main Data ECC Unlock
    
	NF_nFCE_L(Controller);   // CS Low
	NF_CLEAR_RnB(Controller);

	if(NAND_Inform[Controller].uNandType == NAND_Normal8bit)
	//if(NAND_Inform[Controller].uAddrCycle == 4)
	{
		NF_CMD(Controller, 0x00);	// Read command
		NF_ADDR(Controller, 0); 	
	}
	else // if(NAND_Inform[Controller].uAddrCycle == 5)
	{
		NF_CMD(Controller, 0x00);	// 1st Read command
		NF_ADDR(Controller, 0); 	
		NF_ADDR(Controller, 0); 
	}
	
	NF_ADDR(Controller, uBlockPage&0xff);		//
	NF_ADDR(Controller, (uBlockPage>>8)&0xff);	// Block & Page num.
	NF_ADDR(Controller, (uBlockPage>>16)&0xff);	//

	if(NAND_Inform[Controller].uNandType == NAND_Advanced8bit)
		NF_CMD(Controller, 0x30);	// 2'nd command
	
	NF_DETECT_RnB(Controller);

    	#if NAND_TRANSFER_MODE == POLLING_TRANSFER
	#if 0
		for(i=0 ; i<uPageSize ; i++) 
		{
			*pBuffer++ = NF_RDDATA8(Controller);	// Read one page
		}
	#else
		for(i=0 ; i<uPageSize/4 ; i++) 
		{
			*pBufferW++ = NF_RDDATA(Controller);	// Read one page
		}
	#endif
	#elif NAND_TRANSFER_MODE == DMA_TRANSFER
		Nand_DmaDone = 0;
		DMACH_Setup(DMA_A, 0x0, (u32)(&(NAND(Controller)->rNFDATA)), 1, (u32)pBufferW, 0, WORD, uPageSize/4, DEMAND, DMA1_NAND_TX, MEM, BURST128, &g_oNandDmac1);
			
		DMACH_Start(&g_oNandDmac1);
		while(!Nand_DmaDone);	
	#endif

	NF_MECC_Lock(Controller); // Main Data ECC Stop. ECC Lock

	if(NAND_Inform[Controller].uNandType == NAND_Normal8bit)
	{
		NF_SECC_UnLock(Controller); // Spare ECC Unlock
		
		uMecc = NF_RDDATA(Controller); 	// Main Area ECC Check(1st ~ 4th byte : Main Area ECC Information)


		
		if(NAND_Inform[Controller].uSpareECCtest == 0)
		{
			Outp32(&NAND(Controller)->rNFMECCD0, ((uMecc&0xff00)<<8)|(uMecc&0xff) );
			Outp32(&NAND(Controller)->rNFMECCD1, ((uMecc&0xff000000)>>8)|((uMecc&0xff0000)>>16) );
		}
		else if(NAND_Inform[Controller].uSpareECCtest == 1)
		{
			u32 uTempMecc;			// The correct MECC value is written for SpareECC Test
			uTempMecc = ((u32)aNand_Spare_Data_Temp[3]<<24)|
						((u32)aNand_Spare_Data_Temp[2]<<16)|
						((u32)aNand_Spare_Data_Temp[1]<<8)|
						((u32)aNand_Spare_Data_Temp[0]);		
			Outp32(&NAND(Controller)->rNFMECCD0, ((uTempMecc&0xff00)<<8)|(uTempMecc&0xff) );
			Outp32(&NAND(Controller)->rNFMECCD1, ((uTempMecc&0xff000000)>>8)|((uTempMecc&0xff0000)>>16) );
		}

		NF_SECC_Lock(Controller); // Spare ECC Lock
		
		aNand_Spare_Data[0] = (u8)(uMecc&0xFF);
		aNand_Spare_Data[1] = (u8)((uMecc&0xFF00)>>8);
		aNand_Spare_Data[2] = (u8)((uMecc&0xFF0000)>>16);
		aNand_Spare_Data[3] = (u8)((uMecc&0xFF000000)>>24);
		aNand_Spare_Data[4] = NF_RDDATA8(Controller);		 // Vaild data read 0xff
		
	}
	else if(NAND_Inform[Controller].uNandType == NAND_Advanced8bit)
	{	
		aNand_Spare_Data[0] = NF_RDDATA8(Controller);		// Invalid Block Check data 0xff

		NF_SECC_UnLock(Controller); // Spare ecc unlock
		
		uMecc = NF_RDDATA(Controller);		// Main Area ECC Check

		NF_SECC_Lock(Controller);
		
		if(NAND_Inform[Controller].uSpareECCtest == 0)
		{
			Outp32(&NAND(Controller)->rNFMECCD0, ((uMecc&0xff00)<<8)|(uMecc&0xff) );
			Outp32(&NAND(Controller)->rNFMECCD1, ((uMecc&0xff000000)>>8)|((uMecc&0xff0000)>>16) );
		}
		
		aNand_Spare_Data[1] = (u8)(uMecc&0xFF);
		aNand_Spare_Data[2] = (u8)((uMecc&0xFF00)>>8);
		aNand_Spare_Data[3] = (u8)((uMecc&0xFF0000)>>16);
		aNand_Spare_Data[4] = (u8)((uMecc&0xFF000000)>>24);
	}

	for(i=5; i<NAND_Inform[Controller].uSpareSize ; i++)
	{
		aNand_Spare_Data[i] = NF_RDDATA8(Controller);
	}
	
	for(i=0; i<NAND_Inform[Controller].uSpareSize ; i++)
		*pSpareBuffer++ = aNand_Spare_Data[i];

	uSecc = (aNand_Spare_Data[9]<<8) | (aNand_Spare_Data[8]); 	// Spare Area ECC Check
	
	Outp32(&NAND(Controller)->rNFSECCD, ((uSecc&0xff00)<<8)|(uSecc&0xff) );
		
	NF_nFCE_H(Controller); 

	eError = NAND_CheckECCError(Controller);

	return eError;
}


//////////
// Function Name : NAND_ReadPageMLC
// Function Description : This function read the 1 page data of external MLC Type NAND device
// Input : 	Controller - Nand Controller Port Number 
//			uBlock - Read Block
//			uPage - Read Page
//			pBuffer - Read data buffer
// Output : 	Nand Error Type
NAND_eERROR NAND_ReadPageMLC(u32 Controller, u32 uBlock, u32 uPage, u8 *pBuffer, u8 *pSpareBuffer)
{
	u32 i, uBlockPage, uPageSize, uTemp;
	NAND_eERROR eError;
	u32 *pBufferW;
			
	pBufferW = (u32 *)pBuffer;
	g_Nand_4bitEccDecDone = 0;
	g_Nand_RnBTransition = 0;
	
	
	uTemp = NAND_ReadNFCONRegister(Controller);
	//uTemp |= (1<<12);		// 4bit ECC decoding Interrupt enable
	uTemp |= (1<<12)|(1<<13);		// 4bit ECC decoding/encoding Interrupt enable...because error
	uTemp |= (1<<9)|(1<<10);		// RnB, Illegal Acess Interupt Enable.
	uTemp &= ~(1<<18);		// Decoding 4bit ECC selection
	Outp32(&(NAND(Controller)->rNFCONT), uTemp);

	Outp32(&(NAND(Controller)->rNFSTAT), (1<<6));		// 4bit ECC Decoding finish bit clear
	Outp32(&(NAND(Controller)->rNFSTAT), (1<<5));		// Illegal access Pending clear.
	Outp32(&(NAND(Controller)->rNFSTAT), (1<<4));		// RnB pending clear

	INTC_Enable(NUM_NFC);

	uBlockPage=(uBlock<<7) + uPage;
	uPageSize = NAND_Inform[Controller].uPageSize;

	//////////////////////////
	// Read 0 ~ 511
	//////////////////////////
	NF_RSTECC(Controller);    // Initialize ECC
	NF_MECC_UnLock(Controller);
    
	NF_nFCE_L(Controller);   
	
#if (NAND_REG_BUG == TRUE)
	uTemp = NAND_ReadNFCONRegister(Controller);
	uTemp |= (1<<12)|(1<<13);		// 4bit ECC decoding/encoding Interrupt enable...because error
	Outp32(&(NAND(Controller)->rNFCONT), uTemp);	
#endif

	NF_CLEAR_RnB(Controller);

	NF_CMD(Controller, 0x00);	// 1st Read command

	NF_ADDR(Controller, 0); 	
	NF_ADDR(Controller, 0); 	
	NF_ADDR(Controller, uBlockPage&0xff);		//
	NF_ADDR(Controller, (uBlockPage>>8)&0xff);	// Block & Page num.
	NF_ADDR(Controller, (uBlockPage>>16)&0xff);	//

	NF_CMD(Controller, 0x30);	// 2'nd command

	while(!g_Nand_RnBTransition);
	//NF_DETECT_RnB(Controller);

    	#if NAND_TRANSFER_MODE == POLLING_TRANSFER
	#if 0	
		for(i=0 ; i<NAND_MLC_TRANS_SIZE ; i++) 	// MLC ECC Size : 512bytes
		{
			*pBuffer++ = NF_RDDATA8(Controller);	// Read one page
		}
	#else
		for(i=0 ; i<NAND_MLC_TRANS_SIZE/4 ; i++) 
		{
			*pBufferW++ = NF_RDDATA(Controller);	// Read one page
		}
	#endif		
	#elif NAND_TRANSFER_MODE == DMA_TRANSFER
		Nand_DmaDone = 0;
		DMACH_Setup(DMA_A, 0x0, (u32)(&(NAND(Controller)->rNFDATA)), 1, (u32)pBufferW, 0, WORD, NAND_MLC_TRANS_SIZE/4, DEMAND, DMA1_NAND_TX, MEM, BURST128, &g_oNandDmac1);
		
		DMACH_Start(&g_oNandDmac1);
		while(!Nand_DmaDone);	
		pBufferW += NAND_MLC_TRANS_SIZE/4;
	#endif

	NF_CMD(Controller, 0x05);
	NF_ADDR(Controller, (uPageSize)&0xFF);
	NF_ADDR(Controller, ((uPageSize)>>8)&0xFF);
	NF_CMD(Controller, 0xE0);

	for(i=0 ; i<7 ; i++) 
	{
		aNand_Spare_Data[i] = NF_RDDATA8(Controller);	
		if(NAND_Inform[Controller].uPerformanceCheck == 0)
			*pSpareBuffer++ = aNand_Spare_Data[i];		
	}

	// Check decoding done Polling Method
	//while(!(NAND(Controller)->rNFSTAT&(1<<6)));
	//*(unsigned volatile *)0x80200028 |= (1<<6); // Pending Clear

	while(!g_Nand_4bitEccDecDone); // 6400 Code ISR Using. 
	NF_MECC_Lock(Controller);


	eError = NAND_CheckECCError(Controller);
	if(eError != eNAND_NoError)
	{
		NF_MECC_Lock(Controller);	
		NF_nFCE_H(Controller);
		
		uTemp = NAND_ReadNFCONRegister(Controller);
		uTemp &= ~((1<<12)|(1<<13));		// 4bit ECC decoding Interrupt disable
		Outp32(&(NAND(Controller)->rNFCONT), uTemp);	

		NAND_Inform[Controller].uMLCECCPageReadSector = 0;		//0 ~ 511 Bytes	
		return eError;
	}

	//////////////////////////
	// Read 512 ~ 1023
	//////////////////////////
	g_Nand_4bitEccDecDone = 0;
	NF_RSTECC(Controller);    // Initialize ECC
	NF_MECC_UnLock(Controller);

#if (NAND_REG_BUG == TRUE)
	uTemp = NAND_ReadNFCONRegister(Controller);
	uTemp |= (1<<12)|(1<<13);		// 4bit ECC decoding/encoding Interrupt enable...because error
	Outp32(&(NAND(Controller)->rNFCONT), uTemp);	
#endif

	NF_CMD(Controller, 0x05);
	NF_ADDR(Controller, (512)&0xFF);
	NF_ADDR(Controller, (512>>8)&0xFF);
	NF_CMD(Controller, 0xE0);

    	#if NAND_TRANSFER_MODE == POLLING_TRANSFER
	#if 0	
		for(i=0 ; i<NAND_MLC_TRANS_SIZE ; i++) 	// MLC ECC Size : 512bytes
		{
			*pBuffer++ = NF_RDDATA8(Controller);	// Read one page
		}
	#else
		for(i=0 ; i<NAND_MLC_TRANS_SIZE/4 ; i++) 
		{
			*pBufferW++ = NF_RDDATA(Controller);	// Read one page
		}
	#endif
	#elif NAND_TRANSFER_MODE == DMA_TRANSFER
		Nand_DmaDone = 0;
		DMACH_Setup(DMA_A, 0x0, (u32)(&(NAND(Controller)->rNFDATA)), 1, (u32)pBufferW, 0, WORD, NAND_MLC_TRANS_SIZE/4, DEMAND, DMA1_NAND_TX, MEM, BURST128, &g_oNandDmac1);
		
		DMACH_Start(&g_oNandDmac1);
		while(!Nand_DmaDone);	
		pBufferW += NAND_MLC_TRANS_SIZE/4;
	#endif

	NF_CMD(Controller, 0x05);
	NF_ADDR(Controller, (uPageSize+7)&0xFF);
	NF_ADDR(Controller, ((uPageSize+7)>>8)&0xFF);
	NF_CMD(Controller, 0xE0);

	for(i=7;i<14;i++) 
	{
		aNand_Spare_Data[i] = NF_RDDATA8(Controller);	
		if(NAND_Inform[Controller].uPerformanceCheck == 0)
			*pSpareBuffer++ = aNand_Spare_Data[i];	
	}	

	// Check decoding done Polling Method
	//while(!(NAND(Controller)->rNFSTAT&(1<<6)));
	//*(unsigned volatile *)0x80200028 |= (1<<6); // Pending Clear

	while(!g_Nand_4bitEccDecDone); // 6400 Code ISR Using. 
	NF_MECC_Lock(Controller);	


	eError = NAND_CheckECCError(Controller);
	if(eError != eNAND_NoError)
	{
		NF_MECC_Lock(Controller);	
		NF_nFCE_H(Controller);
		uTemp = NAND_ReadNFCONRegister(Controller);
		uTemp &= ~((1<<12)|(1<<13));		// 4bit ECC decoding Interrupt enable
		Outp32(&(NAND(Controller)->rNFCONT), uTemp);			
		NAND_Inform[Controller].uMLCECCPageReadSector = 1;		//512 ~ 1023 Bytes
		return eError;
	}
	
	//////////////////////////
	// Read 1024 ~ 1535
	//////////////////////////
	g_Nand_4bitEccDecDone = 0;
	NF_RSTECC(Controller);    // Initialize ECC
	NF_MECC_UnLock(Controller);

#if (NAND_REG_BUG == TRUE)
	uTemp = NAND_ReadNFCONRegister(Controller);
	uTemp |= (1<<12)|(1<<13);		// 4bit ECC decoding/encoding Interrupt enable...because error
	Outp32(&(NAND(Controller)->rNFCONT), uTemp);	
#endif

	NF_CMD(Controller, 0x05);
	NF_ADDR(Controller, (512*2)&0xFF);
	NF_ADDR(Controller, ((512*2)>>8)&0xFF);
	NF_CMD(Controller, 0xE0);

    	#if NAND_TRANSFER_MODE == POLLING_TRANSFER
	#if 0	
		for(i=0 ; i<NAND_MLC_TRANS_SIZE ; i++) 	// MLC ECC Size : 512bytes
		{
			*pBuffer++ = NF_RDDATA8(Controller);	// Read one page
		}
	#else
		for(i=0 ; i<NAND_MLC_TRANS_SIZE/4 ; i++) 
		{
			*pBufferW++ = NF_RDDATA(Controller);	// Read one page
		}
	#endif
	#elif NAND_TRANSFER_MODE == DMA_TRANSFER
		Nand_DmaDone = 0;
//		DMACH_Setup(DMA_A, 0x0, (u32)(&(NAND(Controller)->rNFDATA)), 1, (u32)pBufferW, 0, WORD, NAND_MLC_TRANS_SIZE/4, DEMAND, DMA1_NAND_TX, MEM, BURST128, &g_oNandDmac1);
		
//		DMACH_Start(&g_oNandDmac1);
		while(!Nand_DmaDone);	
		pBufferW += NAND_MLC_TRANS_SIZE/4;
	#endif

	NF_CMD(Controller, 0x05);
	NF_ADDR(Controller, (uPageSize+14)&0xFF);
	NF_ADDR(Controller, ((uPageSize+14)>>8)&0xFF);
	NF_CMD(Controller, 0xE0);

	for(i=14;i<21;i++) 
	{
		aNand_Spare_Data[i] = NF_RDDATA8(Controller);	
		if(NAND_Inform[Controller].uPerformanceCheck == 0)
			*pSpareBuffer++ = aNand_Spare_Data[i];	
	}	

	// Check decoding done Polling Method
	//while(!(NAND(Controller)->rNFSTAT&(1<<6)));
	//*(unsigned volatile *)0x80200028 |= (1<<6); // Pending Clear

	while(!g_Nand_4bitEccDecDone); // 6400 Code ISR Using. 
	NF_MECC_Lock(Controller);

	eError = NAND_CheckECCError(Controller);
	if(eError != eNAND_NoError)
	{
		NF_MECC_Lock(Controller);		
		NF_nFCE_H(Controller);
		uTemp = NAND_ReadNFCONRegister(Controller);
		uTemp &= ~(1<<12);		// 4bit ECC decoding Interrupt enable
		Outp32(&(NAND(Controller)->rNFCONT), uTemp);	
		NAND_Inform[Controller].uMLCECCPageReadSector = 2;		//1024 ~ 1535 Bytes
		return eError;
	}
	
	//////////////////////////
	// Read 1536 ~ 2047
	//////////////////////////
	g_Nand_4bitEccDecDone = 0;
	NF_RSTECC(Controller);    // Initialize ECC
	NF_MECC_UnLock(Controller);

#if (NAND_REG_BUG == TRUE)
	uTemp = NAND_ReadNFCONRegister(Controller);
	uTemp |= (1<<12)|(1<<13);		// 4bit ECC decoding/encoding Interrupt enable...because error
	Outp32(&(NAND(Controller)->rNFCONT), uTemp);	
#endif

	NF_CMD(Controller, 0x05);
	NF_ADDR(Controller, (512*3)&0xFF);
	NF_ADDR(Controller, ((512*3)>>8)&0xFF);
	NF_CMD(Controller, 0xE0);

    	#if NAND_TRANSFER_MODE == POLLING_TRANSFER
	#if 0	
		for(i=0 ; i<NAND_MLC_TRANS_SIZE ; i++) 	// MLC ECC Size : 512bytes
		{
			*pBuffer++ = NF_RDDATA8(Controller);	// Read one page
		}
	#else
		for(i=0 ; i<NAND_MLC_TRANS_SIZE/4 ; i++) 
		{
			*pBufferW++ = NF_RDDATA(Controller);	// Read one page
		}
	#endif
	#elif NAND_TRANSFER_MODE == DMA_TRANSFER
		Nand_DmaDone = 0;
		DMACH_Setup(DMA_A, 0x0, (u32)(&(NAND(Controller)->rNFDATA)), 1, (u32)pBufferW, 0, WORD, NAND_MLC_TRANS_SIZE/4, DEMAND, DMA1_NAND_TX, MEM, BURST128, &g_oNandDmac1);
		
		DMACH_Start(&g_oNandDmac1);
		while(!Nand_DmaDone);	
		pBufferW += NAND_MLC_TRANS_SIZE/4;
	#endif

	NF_CMD(Controller, 0x05);
	NF_ADDR(Controller, (uPageSize+21)&0xFF);
	NF_ADDR(Controller, ((uPageSize+21)>>8)&0xFF);
	NF_CMD(Controller, 0xE0);

	for(i=21;i<28;i++) 
	{
		aNand_Spare_Data[i] = NF_RDDATA8(Controller);	
		if(NAND_Inform[Controller].uPerformanceCheck == 0)
			*pSpareBuffer++ = aNand_Spare_Data[i];	
	}	

	// Check decoding done Polling Method
	//while(!(NAND(Controller)->rNFSTAT&(1<<6)));
	//*(unsigned volatile *)0x80200028 |= (1<<6); // Pending Clear

	while(!g_Nand_4bitEccDecDone); // 6400 Code ISR Using. 
	NF_MECC_Lock(Controller);

	eError = NAND_CheckECCError(Controller);
	if(eError != eNAND_NoError)
	{
		NF_MECC_Lock(Controller);		
		NF_nFCE_H(Controller);
		uTemp = NAND_ReadNFCONRegister(Controller);
		uTemp &= ~(1<<12);		// 4bit ECC decoding Interrupt enable
		Outp32(&(NAND(Controller)->rNFCONT), uTemp);		
		NAND_Inform[Controller].uMLCECCPageReadSector = 3;		// 1536 ~ 2047 Bytes
		return eError;
	}

	NF_nFCE_H(Controller);

	uTemp = NAND_ReadNFCONRegister(Controller);
	uTemp &= ~((1<<12)|(1<<13));		// 4bit ECC decoding/encoding Interrupt disable
	Outp32(&(NAND(Controller)->rNFCONT), uTemp);
	
	//	INTC_Mask(NUM_NFC); // FPGA
	INTC_Disable(NUM_NFC);
	
	return eNAND_NoError;
}



//////////
// Function Name : NAND_ReadPage
// Function Description : This function read the 1 page data of external NAND device
// Input : 	Controller - Nand Controller Port Number 
//			uBlock - Read Block
//			uPage - Read Page
//			pBuffer - Read data buffer
// Output : 	Nand Error Type
NAND_eERROR NAND_ReadPage(u32 Controller, u32 uBlock, u32 uPage, u8 *pBuffer, u8 *pSpareBuffer)
{
	NAND_eERROR eError;

	if( (NAND_Inform[Controller].uNandType == NAND_Normal8bit) || (NAND_Inform[Controller].uNandType == NAND_Advanced8bit) )
		eError = NAND_ReadPageSLC(Controller, uBlock, uPage, pBuffer, pSpareBuffer);
	else //if(NAND_Inform[Controller].uNandType == NAND_MLC8bit)
		eError = NAND_ReadPageMLC(Controller, uBlock, uPage, pBuffer, pSpareBuffer);

	return eError;
}



//////////
// Function Name : NAND_ReadMultiPage
// Function Description : This function read the multi page data of external NAND device
// Input : 	Controller - Nand Controller Port Number 
//			uBlock - Read Block
//			uPage - Read Page
//			pBuffer - Read data buffer
// Output : 	Nand Error Type
NAND_eERROR NAND_ReadMultiPage(u32 Controller, u32 uStartBlock, u32 uStartPage, u8 *pBuffer, u32 uSize)
{
	u8 aSpareBuffer[NAND_SPARE_MAX];
	u32 i, uPageCnt, uBlock, uPage;
	NAND_eERROR eError;

	uPageCnt = uSize/NAND_Inform[Controller].uPageSize;
	uBlock = uStartBlock;
	uPage = uStartPage;
	
	for(i=0 ; i<uPageCnt ; i++)
	{
		if( (NAND_Inform[Controller].uNandType == NAND_Normal8bit) || (NAND_Inform[Controller].uNandType == NAND_Advanced8bit) )
			eError = NAND_ReadPageSLC(Controller, uBlock, uPage, pBuffer, aSpareBuffer);
		else
			eError = NAND_ReadPageMLC(Controller, uBlock, uPage, pBuffer, aSpareBuffer);

		if(eError != eNAND_NoError)
			return eError;
		
		pBuffer += NAND_Inform[Controller].uPageSize;
		uPage++;

		if(uPage == NAND_Inform[Controller].uPageNum)
		{
			uBlock++;
			uPage = 0;
		}
	}
	
	return eError;
}



//////////
// Function Name : NAND_WritePageSLC
// Function Description : This function write the 1 page data of external SLC Type NAND device
// Input : 	Controller - Nand Controller Port Number 
//			uBlock - Write Block
//			uPage - Write Page
//			pBuffer - Write data buffer
//			aSpareBuffer - Memory to store the spare data
// Output : 	ECC Error Type 
NAND_eERROR NAND_WritePageSLC(u32 Controller, u32 uBlock, u32 uPage, u8 *pBuffer, u8 *aSpareBuffer)
{
	u32 i, uBlockPage, uPageSize, uSpareSize, uTemp;
	u32 uMecc, uSecc, uStatus;
	NAND_eERROR eError;	
	u32 *pBufferW;
	u32 uNFCONFreg, uNFCONTreg;
	u32 uTemp0;
			
	pBufferW = (u32 *)pBuffer;			
	g_Nand_RnBTransition = 0;

	// Setting NFCONF Register.
	uNFCONFreg = Inp32(&(NAND(Controller)->rNFCONF));
	uNFCONFreg = uNFCONFreg & (~(0x1<<30)); // HCLK is more than 66Mhz.
	uNFCONFreg = uNFCONFreg & (~(0x3<<23)); // 1bit ECC Select.
	uNFCONFreg = uNFCONFreg | ((7<<12)|(7<<8)|(7<<4)) ; // Timing 7,7,7 setting. add. jungil 
	Outp32(&(NAND(Controller)->rNFCONF), uNFCONFreg ); // NFCONF Register setting. 
	// Setting NFCONT Register.
	uNFCONTreg = Inp32(&(NAND(Controller)->rNFCONT)); // read NFCON
	uNFCONTreg &= ~((1<<13)|(1<<12));		// 4bit ECC encoding/decoding completion Interrupt disable
	uNFCONTreg |= (1<<10) | (1<<9);		// Illegal Access & RnB Transition Interrupt enable
	Outp32(&(NAND(Controller)->rNFCONT), uNFCONTreg); // write setting. NFCONT Register
	Outp32(&(NAND(Controller)->rNFSTAT), (1<<5)|(1<<4)); // Illegal Access & RnB Interrupt Pending bit clear

	INTC_Enable(NUM_NFC); // Interrupt Enable
	
	if(NAND_Inform[Controller].uNandType == NAND_Normal8bit)
	{
		uBlockPage=(uBlock<<5) + uPage;
		uPageSize = NAND_Inform[Controller].uPageSize;
		uSpareSize = NAND_Inform[Controller].uSpareSize;
	}
	else //if(NAND_Inform[Controller].uNandType == NAND_Advanced8bit)
	{
		uBlockPage=(uBlock<<6) + uPage;
		uPageSize = NAND_Inform[Controller].uPageSize;
		uSpareSize = NAND_Inform[Controller].uSpareSize;
	}

	NF_RSTECC(Controller);    // Initialize ECC
	NF_MECC_UnLock(Controller);

	NF_nFCE_L(Controller);    
	NF_CLEAR_RnB(Controller);

	NF_CMD(Controller, 0x80);	// Program command
	NF_ADDR(Controller, 0); 
		
	if(NAND_Inform[Controller].uNandType == NAND_Advanced8bit)
	//if(NAND_Inform[Controller].uAddrCycle == 5)
	{
		NF_ADDR(Controller, 0); 
	}
	
	NF_ADDR(Controller, uBlockPage&0xff);		//
	NF_ADDR(Controller, (uBlockPage>>8)&0xff);	// Block & Page num.
	NF_ADDR(Controller, (uBlockPage>>16)&0xff);	//

    	#if NAND_TRANSFER_MODE == POLLING_TRANSFER
		#if 0
			for(i=0 ; i<uPageSize ; i++) 
			{
				NF_WRDATA8(Controller, *pBuffer++);		// Write one page
			}
		#else
			for(i=0 ; i<uPageSize/4 ; i++) 
			{
				NF_WRDATA(Controller, *pBufferW++);	// Read one page
			}
		#endif		
	#elif NAND_TRANSFER_MODE == DMA_TRANSFER
		Nand_DmaDone = 0;
		DMACH_Setup(DMA_A, 0x0, (u32)pBufferW, 0, (u32)(&(NAND(Controller)->rNFDATA)), 1, WORD, uPageSize/4, DEMAND, MEM, DMA1_NAND_RX, BURST128, &g_oNandDmac1);

		DMACH_Start(&g_oNandDmac1);
		while(!Nand_DmaDone);	
	#endif

	NF_MECC_Lock(Controller); 

	g_Nand_IllegalAccError  = 0;

	if(NAND_Inform[Controller].uNandType == NAND_Normal8bit)
	{
		uMecc = Inp32(&NAND(Controller)->rNFMECC0);		// Main Area ECC

		aNand_Spare_Data[0] = (u8)(uMecc&0xFF);
		aNand_Spare_Data[1] = (u8)((uMecc&0xFF00)>>8);
		aNand_Spare_Data[2] = (u8)((uMecc&0xFF0000)>>16);
		aNand_Spare_Data[3] = (u8)((uMecc&0xFF000000)>>24);
		aNand_Spare_Data[5] = (u8)0xFF;			//Marking Valid Block

		NF_SECC_UnLock(Controller); // Spare area data write. and start spare ecc making.

		for(i=0;i<4;i++) 
		{
			NF_WRDATA8(Controller, aNand_Spare_Data[i]);		// Write spare array(Main ECC) 4Byte Main ECC Data
			//NF8_Spare_Data[i]=aNand_Spare_Data[i];
    		} 
	
		NF_SECC_Lock(Controller); // Spare ECC Unlock 

		uSecc = Inp32(&NAND(Controller)->rNFSECC) & 0xFFFF;		// Spare Area ECC

		aNand_Spare_Data[8] = (u8)(uSecc&0xFF); 
		aNand_Spare_Data[9] = (u8)((uSecc&0xFF00)>>8);

		for(i=4 ; i<uSpareSize; i++) 
		{
			NF_WRDATA8(Controller, aNand_Spare_Data[i]);  // Write spare array(Spare ECC and Mark)
			//NF8_Spare_Data[i]=aNand_Spare_Data[i];
		}  

		NF_CLEAR_RnB(Controller);
		NF_CMD(Controller, 0x10);	 // Write 2nd command	
	}
	else if(NAND_Inform[Controller].uNandType == NAND_Advanced8bit)
	{	
		uMecc = Inp32(&NAND(Controller)->rNFMECC0);		// Main Area ECC

		aNand_Spare_Data[0] = (u8)0xFF;				//Marking Valid Block
		aNand_Spare_Data[1] = (u8)(uMecc&0xFF);
		aNand_Spare_Data[2] = (u8)((uMecc&0xFF00)>>8);
		aNand_Spare_Data[3] = (u8)((uMecc&0xFF0000)>>16);
		aNand_Spare_Data[4] = (u8)((uMecc&0xFF000000)>>24);

		NF_WRDATA8(Controller, aNand_Spare_Data[0]);	//Write Marking Valid Block
		
		NF_SECC_UnLock(Controller); // Spare ECC create , Spare Ecc Unlock
		for(i=1;i<5;i++) 
		{
			NF_WRDATA8(Controller, aNand_Spare_Data[i]);	// Write spare array(Main ECC)
			//NF8_Spare_Data[i]=aNand_Spare_Data[i];
    		} 
		NF_SECC_Lock(Controller); // Spare ECC Lock , End

		uSecc = Inp32(&NAND(Controller)->rNFSECC) & 0xFFFF;	// Spare Area ECC

		aNand_Spare_Data[8] = (u8)(uSecc&0xFF);
		aNand_Spare_Data[9] = (u8)((uSecc&0xFF00)>>8);

		for(i=5 ; i<uSpareSize; i++) // Spare ECC and the other data write.
		{
			NF_WRDATA8(Controller, aNand_Spare_Data[i]);  // Write spare array(Spare ECC and Mark)
			//NF8_Spare_Data[i]=aNand_Spare_Data[i];
		}  

		NF_CLEAR_RnB(Controller);
		NF_CMD(Controller, 0x10);	 // Write 2nd command	
	}

	#if 1 // Interupt Mode
	while(!g_Nand_RnBTransition)
	{
		if(g_Nand_IllegalAccError == 1)
		{
			//NAND_Reset(Controller);
			g_Nand_IllegalAccError = 0;
			NF_CMD(Controller, 0x70); // Read Status register
			NF_nFCE_H(Controller);
			
			uNFCONTreg = 0 ; 
			uNFCONTreg = Inp32(&(NAND(Controller)->rNFCONT)); // read NFCONT regster
			uNFCONTreg &= ~((1<<10) | (1<<9));		// Illegal Access & RnB Transition Interrupt disable
			Outp32(&(NAND(Controller)->rNFCONT), uNFCONTreg);
			INTC_Disable(NUM_NFC);
			return eNAND_ProgramError;
		}
	}
	#else // Polling Mode
		NF_DETECT_RnB(Controller);
	#endif

	NF_CMD(Controller, 0x70); //Read Program Status

	while(!((uStatus = NF_RDDATA8(Controller)) & (1<<6)));

	if(uStatus & 0x01)
		eError = eNAND_ProgramError;		// Error in Program
	else
		eError = eNAND_NoError;
	
	NF_nFCE_H(Controller);
	
	uNFCONTreg = 0 ; 
	uNFCONTreg = Inp32(&(NAND(Controller)->rNFCONT)); // read NFCONT regster
	uNFCONTreg &= ~((1<<10) | (1<<9));		// Illegal Access & RnB Transition Interrupt disable
	Outp32(&(NAND(Controller)->rNFCONT), uNFCONTreg);

	if(NAND_Inform[Controller].uECCtest == 1)
	{
		for(i=0 ; i<uSpareSize ; i++)
			aNand_Spare_Data_Temp[i] = aNand_Spare_Data[i];
	}

	for(i=0 ; i<uSpareSize ; i++)
	{
		*aSpareBuffer++ = aNand_Spare_Data[i];
	}
	
	INTC_Disable(NUM_NFC);
	return eError;
}




//////////
// Function Name : NAND_WritePageMLC
// Function Description : This function write the 1 page data of external NAND device
// Input : 	Controller - Nand Controller Port Number 
//			uBlock - Write Block
//			uPage - Write Page
//			pBuffer - Write data buffer
// Output : 	Nand Error Type
NAND_eERROR NAND_WritePageMLC(u32 Controller, u32 uBlock, u32 uPage, u8 *pBuffer, u8 *aSpareBuffer)
{
	u32 i, uBlockPage, uSpareSize, uTemp;
	u32 uMecc0, uMecc1, uStatus;
	//u32 uSecc;
	NAND_eERROR eError;
	u32 *pBufferW;
			
	pBufferW = (u32 *)pBuffer;
	g_Nand_4bitEccEncDone = 0;
	g_Nand_RnBTransition = 0;
	g_Nand_IllegalAccError = 0;
	
	uTemp = NAND_ReadNFCONRegister(Controller);
#if (NAND_REG_BUG == TRUE)
	uTemp |= (1<<18)|(1<<13)|(1<<12)|(1<<10)|(1<<9);		// 4bit ECC encoding& decoding & Illegal Access & RnB Transition Interrupt enable..because error
#else
	uTemp |= (1<<18)|(1<<13)|(1<<10)|(1<<9);		// 4bit ECC encoding & Illegal Access & RnB Transition Interrupt enable
#endif	
	Outp32(&(NAND(Controller)->rNFCONT), uTemp);

	Outp32(&(NAND(Controller)->rNFSTAT), (1<<7));		// 4bit ECC encoding finish bit clear

	INTC_Enable(NUM_NFC);

	uBlockPage=(uBlock<<7) + uPage;
	uSpareSize = NAND_Inform[Controller].uSpareSize;

	//////////////////////////
	// Write 0 ~ 511
	//////////////////////////
	NF_RSTECC(Controller);    // Initialize ECC
	NF_MECC_UnLock(Controller);
    
	NF_nFCE_L(Controller);    
#if (NAND_REG_BUG == TRUE)
	uTemp = NAND_ReadNFCONRegister(Controller);
	uTemp |= (1<<12)|(1<<13);		// 4bit ECC decoding/encoding Interrupt enable...because error
	Outp32(&(NAND(Controller)->rNFCONT), uTemp);	
#endif	
	NF_CLEAR_RnB(Controller);

	NF_CMD(Controller, 0x00);	// 1st Read command..????
	NF_CMD(Controller, 0x80);	// 1st Write command

	NF_ADDR(Controller, 0); 	
	NF_ADDR(Controller, 0); 	
	NF_ADDR(Controller, uBlockPage&0xff);		//
	NF_ADDR(Controller, (uBlockPage>>8)&0xff);	// Block & Page num.
	NF_ADDR(Controller, (uBlockPage>>16)&0xff);	//

    	#if NAND_TRANSFER_MODE == POLLING_TRANSFER
	#if 0
		for(i=0 ; i<NAND_MLC_TRANS_SIZE ; i++) 	// MLC ECC Size : 512bytes
		{
			NF_WRDATA8(Controller, *pBuffer++);		// Write one page
		}
	#else
		for(i=0 ; i<NAND_MLC_TRANS_SIZE/4 ; i++) 
		{
			NF_WRDATA(Controller, *pBufferW++);	// Write one page
		}
	#endif
	#elif NAND_TRANSFER_MODE == DMA_TRANSFER
		Nand_DmaDone = 0;
		DMACH_Setup(DMA_A, 0x0, (u32)pBufferW, 0, (u32)(&(NAND(Controller)->rNFDATA)), 1, WORD, NAND_MLC_TRANS_SIZE/4, DEMAND, MEM, DMA1_NAND_RX, BURST128, &g_oNandDmac1);
			
		DMACH_Start(&g_oNandDmac1);
		while(!Nand_DmaDone);	
	   	pBufferW += NAND_MLC_TRANS_SIZE/4;
	#endif

	NF_MECC_Lock(Controller);

//EVT1 NFCONT[13] bit has no read path....Not check ECC encoding completion
	while(!g_Nand_4bitEccEncDone);

	uMecc0 = Inp32(&(NAND(Controller)->rNFMECC0));
	uMecc1 = Inp32(&(NAND(Controller)->rNFMECC1)) & 0xFFFFFF;

	aNand_Spare_Data[0]=(u8)(uMecc0&0xff);
	aNand_Spare_Data[1]=(u8)((uMecc0>>8) & 0xff);
	aNand_Spare_Data[2]=(u8)((uMecc0>>16) & 0xff);
	aNand_Spare_Data[3]=(u8)((uMecc0>>24) & 0xff);
	aNand_Spare_Data[4]=(u8)(uMecc1&0xff);
	aNand_Spare_Data[5]=(u8)((uMecc1>>8) & 0xff);
	aNand_Spare_Data[6]=(u8)((uMecc1>>16) & 0xff);	

	//////////////////////////
	// Write 512 ~ 1023
	//////////////////////////
	g_Nand_4bitEccEncDone = 0;
	NF_RSTECC(Controller);    // Initialize ECC
	NF_MECC_UnLock(Controller);

#if (NAND_REG_BUG == TRUE)
	uTemp = NAND_ReadNFCONRegister(Controller);
	uTemp |= (1<<12)|(1<<13);		// 4bit ECC decoding/encoding Interrupt enable...because error
	Outp32(&(NAND(Controller)->rNFCONT), uTemp);	
#endif

#if NAND_TRANSFER_MODE == POLLING_TRANSFER
	#if 0
		for(i=0 ; i<NAND_MLC_TRANS_SIZE ; i++) 	// MLC ECC Size : 512bytes
		{
			NF_WRDATA8(Controller, *pBuffer++);		// Write one page
		}
	#else
		for(i=0 ; i<NAND_MLC_TRANS_SIZE/4 ; i++) 
		{
			NF_WRDATA(Controller, *pBufferW++);	// Write one page
		}
	#endif
	#elif NAND_TRANSFER_MODE == DMA_TRANSFER
		Nand_DmaDone = 0;
		DMACH_Setup(DMA_A, 0x0, (u32)pBufferW, 0, (u32)(&(NAND(Controller)->rNFDATA)), 1, WORD, NAND_MLC_TRANS_SIZE/4, DEMAND, MEM, DMA1_NAND_RX, BURST128, &g_oNandDmac1);
			
		DMACH_Start(&g_oNandDmac1);
		while(!Nand_DmaDone);	
	   	pBufferW += NAND_MLC_TRANS_SIZE/4;
	#endif

	NF_MECC_Lock(Controller);
	//EVT1 NFCONT[13] bit has no read path....Not check ECC encoding completion
	while(!g_Nand_4bitEccEncDone);

	uMecc0 = Inp32(&(NAND(Controller)->rNFMECC0));
	uMecc1 = Inp32(&(NAND(Controller)->rNFMECC1)) & 0xFFFFFF;

	aNand_Spare_Data[7]=(u8)(uMecc0&0xff);
	aNand_Spare_Data[8]=(u8)((uMecc0>>8) & 0xff);
	aNand_Spare_Data[9]=(u8)((uMecc0>>16) & 0xff);
	aNand_Spare_Data[10]=(u8)((uMecc0>>24) & 0xff);
	aNand_Spare_Data[11]=(u8)(uMecc1&0xff);
	aNand_Spare_Data[12]=(u8)((uMecc1>>8) & 0xff);
	aNand_Spare_Data[13]=(u8)((uMecc1>>16) & 0xff);
	
	//////////////////////////
	// Write 1024 ~ 1535
	//////////////////////////
	g_Nand_4bitEccEncDone = 0;
	NF_RSTECC(Controller);    // Initialize ECC
	NF_MECC_UnLock(Controller);

#if (NAND_REG_BUG == TRUE)
	uTemp = NAND_ReadNFCONRegister(Controller);
	uTemp |= (1<<12)|(1<<13);		// 4bit ECC decoding/encoding Interrupt enable...because error
	Outp32(&(NAND(Controller)->rNFCONT), uTemp);	
#endif

#if NAND_TRANSFER_MODE == POLLING_TRANSFER
	#if 0
		for(i=0 ; i<NAND_MLC_TRANS_SIZE ; i++) 	// MLC ECC Size : 512bytes
		{
			NF_WRDATA8(Controller, *pBuffer++);		// Write one page
		}
	#else
		for(i=0 ; i<NAND_MLC_TRANS_SIZE/4 ; i++) 
		{
			NF_WRDATA(Controller, *pBufferW++);	// Write one page
		}
	#endif
	#elif NAND_TRANSFER_MODE == DMA_TRANSFER
		Nand_DmaDone = 0;
		DMACH_Setup(DMA_A, 0x0, (u32)pBufferW, 0, (u32)(&(NAND(Controller)->rNFDATA)), 1, WORD, NAND_MLC_TRANS_SIZE/4, DEMAND, MEM, DMA1_NAND_RX, BURST128, &g_oNandDmac1);
			
		DMACH_Start(&g_oNandDmac1);
		while(!Nand_DmaDone);	
	   	pBufferW += NAND_MLC_TRANS_SIZE/4;
	#endif

	NF_MECC_Lock(Controller);
	//EVT1 NFCONT[13] bit has no read path....Not check ECC encoding completion
	while(!g_Nand_4bitEccEncDone);

	uMecc0 = Inp32(&(NAND(Controller)->rNFMECC0));
	uMecc1 = Inp32(&(NAND(Controller)->rNFMECC1)) & 0xFFFFFF;

	aNand_Spare_Data[14]=(u8)(uMecc0&0xff);
	aNand_Spare_Data[15]=(u8)((uMecc0>>8) & 0xff);
	aNand_Spare_Data[16]=(u8)((uMecc0>>16) & 0xff);
	aNand_Spare_Data[17]=(u8)((uMecc0>>24) & 0xff);
	aNand_Spare_Data[18]=(u8)(uMecc1&0xff);
	aNand_Spare_Data[19]=(u8)((uMecc1>>8) & 0xff);
	aNand_Spare_Data[20]=(u8)((uMecc1>>16) & 0xff);
	
	//////////////////////////
	// Write 1536 ~ 2047
	//////////////////////////
	g_Nand_4bitEccEncDone = 0;
	NF_RSTECC(Controller);    // Initialize ECC
	NF_MECC_UnLock(Controller);

#if (NAND_REG_BUG == TRUE)
	uTemp = NAND_ReadNFCONRegister(Controller);
	uTemp |= (1<<12)|(1<<13);		// 4bit ECC decoding/encoding Interrupt enable...because error
	Outp32(&(NAND(Controller)->rNFCONT), uTemp);	
#endif

#if NAND_TRANSFER_MODE == POLLING_TRANSFER
	#if 0
		for(i=0 ; i<NAND_MLC_TRANS_SIZE ; i++) 	// MLC ECC Size : 512bytes
		{
			NF_WRDATA8(Controller, *pBuffer++);		// Write one page
		}
	#else
		for(i=0 ; i<NAND_MLC_TRANS_SIZE/4 ; i++) 
		{
			NF_WRDATA(Controller, *pBufferW++);	// Write one page
		}
	#endif
	#elif NAND_TRANSFER_MODE == DMA_TRANSFER
		Nand_DmaDone = 0;
		DMACH_Setup(DMA_A, 0x0, (u32)pBufferW, 0, (u32)(&(NAND(Controller)->rNFDATA)), 1, WORD, NAND_MLC_TRANS_SIZE/4, DEMAND, MEM, DMA1_NAND_RX, BURST128, &g_oNandDmac1);
			
		DMACH_Start(&g_oNandDmac1);
		while(!Nand_DmaDone);	
	   	pBufferW += NAND_MLC_TRANS_SIZE/4;
	#endif

	NF_MECC_Lock(Controller);
	//EVT1 NFCONT[13] bit has no read path....Not check ECC encoding completion
	while(!g_Nand_4bitEccEncDone);

	uMecc0 = Inp32(&(NAND(Controller)->rNFMECC0));
	uMecc1 = Inp32(&(NAND(Controller)->rNFMECC1)) & 0xFFFFFF;

	aNand_Spare_Data[21]=(u8)(uMecc0&0xff);
	aNand_Spare_Data[22]=(u8)((uMecc0>>8) & 0xff);
	aNand_Spare_Data[23]=(u8)((uMecc0>>16) & 0xff);
	aNand_Spare_Data[24]=(u8)((uMecc0>>24) & 0xff);
	aNand_Spare_Data[25]=(u8)(uMecc1&0xff);
	aNand_Spare_Data[26]=(u8)((uMecc1>>8) & 0xff);
	aNand_Spare_Data[27]=(u8)((uMecc1>>16) & 0xff);

	//////////////////////////
	// Spare Area Write
	//////////////////////////

	for(i=0 ; i<uSpareSize ; i++)
		NF_WRDATA8(Controller, aNand_Spare_Data[i]);

	NF_CMD(Controller, 0x10);	// 2nd Write Command

	while(!g_Nand_RnBTransition)
	{
		if(g_Nand_IllegalAccError == 1)
		{
			//NAND_Reset(Controller);
			g_Nand_IllegalAccError = 0;
			
			NF_CMD(Controller, 0x70);

			NF_nFCE_H(Controller);
			
			uTemp = NAND_ReadNFCONRegister(Controller);
			uTemp &= ~((1<<13)|(1<<12)|(1<<10)|(1<<9));		// 4bit ECC encoding & Illegal Access & RnB Transition Interrupt disable
			Outp32(&(NAND(Controller)->rNFCONT), uTemp);

			INTC_Disable(NUM_NFC);	
			
			return eNAND_ProgramError;
		}
	}
	
	//Read Program Status
	NF_CMD(Controller, 0x70);

	while(!((uStatus = NF_RDDATA8(Controller)) & (1<<6)));
	if(uStatus & 0x01)
		eError = eNAND_ProgramError;		// Error in Program
	else
		eError = eNAND_NoError;

	NF_nFCE_H(Controller);
	
	uTemp = NAND_ReadNFCONRegister(Controller);
	uTemp &= ~((1<<13)|(1<<12)|(1<<10)|(1<<9));		// 4bit ECC encoding & Illegal Access & RnB Transition Interrupt disable
	Outp32(&(NAND(Controller)->rNFCONT), uTemp);

	if(NAND_Inform[Controller].uPerformanceCheck == 0)
	{
		if(NAND_Inform[Controller].uECCtest == 1)
		{
			for(i=0 ; i<uSpareSize ; i++)
				aNand_Spare_Data_Temp[i] = aNand_Spare_Data[i];
		}

		for(i=0 ; i<uSpareSize ; i++)
			*aSpareBuffer++ = aNand_Spare_Data[i];
	}
	
	INTC_Disable(NUM_NFC);
	
	return eError;
}



//////////
// Function Name : NAND_WritePage
// Function Description : This function write the 1 page data of external MLC Type NAND device
// Input : 	Controller - Nand Controller Port Number 
//			uBlock - Write Block
//			uPage - Write Page
//			pBuffer - Write data buffer
// Output : 	Nand Error Type
NAND_eERROR NAND_WritePage(u32 Controller, u32 uBlock, u32 uPage, u8 *pBuffer, u8 *aSpareBuffer)
{
	NAND_eERROR eError;

	if( (NAND_Inform[Controller].uNandType == NAND_Normal8bit) || (NAND_Inform[Controller].uNandType == NAND_Advanced8bit) )
		eError = NAND_WritePageSLC(Controller, uBlock, uPage, pBuffer, aSpareBuffer);
	else //if(NAND_Inform[Controller].uNandType == NAND_MLC8bit)
		eError = NAND_WritePageMLC(Controller, uBlock, uPage, pBuffer, aSpareBuffer);

	return eError;
}


//////////
// Function Name : NAND_EraseSingleBlock
// Function Description : This function erase 1 block data
// Input : 	Controller - Nand Controller Port Number 
//			uBlock - Erase Block
// Output : 	Nand Error Type
NAND_eERROR NAND_EraseSingleBlock(u32 Controller, u32 uBlock)
{
	u32 uTemp, uBlockPage, uStatus;
	NAND_eERROR eError;

	if(NAND_Inform[Controller].uNandType == NAND_Normal8bit)
	{
		uBlockPage=(uBlock<<5);
	}
	else if(NAND_Inform[Controller].uNandType == NAND_Advanced8bit)
	{
		uBlockPage=(uBlock<<6);
	}
	else //if(NAND_Inform[Controller].uNandType == NAND_MLC8bit)
	{
		uBlockPage=(uBlock<<7);
	}

	uTemp = NAND_ReadNFCONRegister(Controller);
	uTemp |= (1<<10) | (1<<9);		// Illegal Access & RnB Transition Interrupt enable
	Outp32(&(NAND(Controller)->rNFCONT), uTemp);
	
	Outp32(&(NAND(Controller)->rNFSTAT), (1<<5)|(1<<4));		// Illegal Access & RnB Interrupt Pending bit clear

	//Invalid Block Check
	
	eError = NAND_CheckInvalidBlock(Controller, uBlock);
	if(eError != eNAND_NoError)
		return eError;

	INTC_Enable(NUM_NFC);
	
	g_Nand_RnBTransition = 0;
	g_Nand_IllegalAccError = 0;

	NF_nFCE_L(Controller);
	NF_CLEAR_RnB(Controller);

	NF_CMD(Controller, 0x60);					// 1st Erase Command

	NF_ADDR(Controller, uBlockPage&0xff);	 		// The mark of bad block is in 0 page
	NF_ADDR(Controller, (uBlockPage>>8)&0xff);	 	// For block number A[24:17]
	NF_ADDR(Controller, (uBlockPage>>16)&0xff);  		// For block number A[25]

	NF_CMD(Controller, 0xd0);								// 2nd Erase command

	while(!g_Nand_RnBTransition)
	{
		if(g_Nand_IllegalAccError == 1)
		{
			//NAND_Reset(Controller);
			g_Nand_IllegalAccError = 0;
			//return eNAND_EraseError;
			eError = eNAND_IllegalAccError;
			NF_CMD(Controller, 0x70);   // Read status command
			
			NF_nFCE_H(Controller);

			uTemp = NAND_ReadNFCONRegister(Controller);
			uTemp &= ~((1<<10)|(1<<9));		// Illegal Access & RnB Transition Interrupt disable
			Outp32(&(NAND(Controller)->rNFCONT), uTemp);

			INTC_Disable(NUM_NFC);
			
			return eError;			
		}
	}

	NF_CMD(Controller, 0x70);   // Read status command

	while(!((uStatus = NF_RDDATA8(Controller)) & (1<<6)));
	if(uStatus & 0x01)
		eError = eNAND_EraseError;		// Error in Program
	else
		eError = eNAND_NoError;
	
//      if (NF_RDDATA8(Controller)&0x1) // Erase error check
//		eError = eNAND_EraseError;
//       else 
//		eError = eNAND_NoError;
	   
	NF_nFCE_H(Controller);

	uTemp = NAND_ReadNFCONRegister(Controller);
	uTemp &= ~((1<<10)|(1<<9));		// Illegal Access & RnB Transition Interrupt disable
	Outp32(&(NAND(Controller)->rNFCONT), uTemp);

	// rb1004 : must be modify...Interrupt Controller
	//INT_Disable1(BIT_NUM_NFC);
//	INTC_Mask(NUM_NFC); // FPGA
	INTC_Disable(NUM_NFC);
	
	return eError;
}

//////////
// Function Name : NAND_EraseMultiBlock
// Function Description : This function erase the multi-block data
// Input : 	Controller - Nand Controller Port Number 
//			uBlock - Erase Block
//			uBlockNum - Erase Block Number
// Output : 	Nand Error Type
NAND_eERROR NAND_EraseMultiBlock_Novaild(u32 Controller, u32 uBlock, u32 uBlockNum)
{
	u32 i, j, uMultiEraseNum, uTemp, uStatus;
	u32 uBlockShift, uBlockPage;
	NAND_eERROR eError;


	eError = eNAND_NoError;
	
	if(NAND_Inform[Controller].uNandType == NAND_Normal8bit)
	{
		uBlockPage=(uBlock<<5);
		uBlockShift = 5;
		uMultiEraseNum = 4;
	}
	else if(NAND_Inform[Controller].uNandType == NAND_Advanced8bit)
	{
		uBlockPage=(uBlock<<6);
		uBlockShift = 6;		
		uMultiEraseNum = 2;
	}
	else //if(NAND_Inform[Controller].uNandType == NAND_MLC8bit)
	{
		uBlockPage=(uBlock<<7);
		uBlockShift = 7;		
		uMultiEraseNum = 2;
	}

	Outp32(&(NAND(Controller)->rNFSTAT), (1<<5)|(1<<4));		// Illegal Access & RnB Interrupt Pending bit clear
	uTemp = Inp32(&(NAND(Controller)->rNFCONT));
	uTemp = uTemp | ((1<<10)|(1<<9));
	Outp32(&(NAND(Controller)->rNFCONT), uTemp);		// Illegal Access & RnB Interrupt Enable.	


	//Invalid Block Check

#if 0
	for(i=uBlock ; i<(uBlock+uBlockNum-1) ; i++)
	{
		eError = NAND_CheckInvalidBlock(Controller, i);
		if(eError != eNAND_NoError)
			return eError;
	}
#endif


	INTC_Enable(NUM_NFC);		

	for(i=0 ; i<uBlockNum ; i+=uMultiEraseNum )
	{
		g_Nand_RnBTransition = 0;
		g_Nand_IllegalAccError = 0;
		
		NF_nFCE_L(Controller);
		NF_CLEAR_RnB(Controller);

		for(j=0 ; j<uMultiEraseNum ; j++)
		{
			NF_CMD(Controller, 0x60);						// 1st Erase Command

			NF_ADDR(Controller, uBlockPage&0xff);	 		
			NF_ADDR(Controller, (uBlockPage>>8)&0xff);	 	
			NF_ADDR(Controller, (uBlockPage>>16)&0xff);  	
			uBlockPage += (1<<uBlockShift);
		}

		NF_CMD(Controller, 0xd0);							// 2nd Erase command

#if 1
		while(!g_Nand_RnBTransition)
		{
			if(g_Nand_IllegalAccError == 1)
			{
				//NAND_Reset(Controller);
				g_Nand_IllegalAccError = 0;
				NF_nFCE_H(Controller);
				uTemp = NAND_ReadNFCONRegister(Controller);
				uTemp &= ~((1<<10)|(1<<9));		// Illegal Access & RnB Transition Interrupt disable
				Outp32(&(NAND(Controller)->rNFCONT), uTemp);

//				INTC_Mask(NUM_NFC);				// FPGA
				INTC_Disable(NUM_NFC);
				return eNAND_EraseError;
			}
		}
#else
		NF_DETECT_RnB(Controller);
#endif


		if(NAND_Inform[Controller].uNandType == NAND_Normal8bit)
		{
			NF_CMD(Controller, 0x71); 						// Read Multi-plane status command	
		}
		else
			NF_CMD(Controller, 0x70);   						// Read status command

		while(!((uStatus = NF_RDDATA8(Controller)) & (1<<6)));

		if(uStatus & 0x01)
		{
			eError = eNAND_EraseError;		// Error in Erase
			NF_nFCE_H(Controller);
			uTemp = NAND_ReadNFCONRegister(Controller);
			uTemp &= ~((1<<10)|(1<<9));		// Illegal Access & RnB Transition Interrupt disable
			Outp32(&(NAND(Controller)->rNFCONT), uTemp);

			INTC_Disable(NUM_NFC);			
			return eError;
		}
		else
			eError = eNAND_NoError;
  
		NF_nFCE_H(Controller);
	}

	uTemp = NAND_ReadNFCONRegister(Controller);
	uTemp &= ~((1<<10)|(1<<9));		// Illegal Access & RnB Transition Interrupt disable
	Outp32(&(NAND(Controller)->rNFCONT), uTemp);

//	INTC_Mask(NUM_NFC); // FPGA
	INTC_Disable(NUM_NFC);
	
	return eError;
}
//////////
// Function Name : NAND_EraseMultiBlock
// Function Description : This function erase the multi-block data
// Input : 	Controller - Nand Controller Port Number 
//			uBlock - Erase Block
//			uBlockNum - Erase Block Number
// Output : 	Nand Error Type
NAND_eERROR NAND_EraseMultiBlock(u32 Controller, u32 uBlock, u32 uBlockNum)
{
	u32 i, j, uMultiEraseNum, uTemp, uStatus;
	u32 uBlockShift, uBlockPage;
	NAND_eERROR eError;


	eError = eNAND_NoError;
	
	if(NAND_Inform[Controller].uNandType == NAND_Normal8bit)
	{
		uBlockPage=(uBlock<<5);
		uBlockShift = 5;
		uMultiEraseNum = 4;
	}
	else if(NAND_Inform[Controller].uNandType == NAND_Advanced8bit)
	{
		uBlockPage=(uBlock<<6);
		uBlockShift = 6;		
		uMultiEraseNum = 2;
	}
	else //if(NAND_Inform[Controller].uNandType == NAND_MLC8bit)
	{
		uBlockPage=(uBlock<<7);
		uBlockShift = 7;		
		uMultiEraseNum = 2;
	}

	Outp32(&(NAND(Controller)->rNFSTAT), (1<<5)|(1<<4));		// Illegal Access & RnB Interrupt Pending bit clear
	uTemp = Inp32(&(NAND(Controller)->rNFCONT));
	uTemp = uTemp | ((1<<10)|(1<<9));
	Outp32(&(NAND(Controller)->rNFCONT), uTemp);		// Illegal Access & RnB Interrupt Enable.	


	//Invalid Block Check

	for(i=uBlock ; i<(uBlock+uBlockNum-1) ; i++)
	{
		eError = NAND_CheckInvalidBlock(Controller, i);
		if(eError != eNAND_NoError)
			return eError;
	}

	INTC_Enable(NUM_NFC);		

	for(i=0 ; i<uBlockNum ; i+=uMultiEraseNum )
	{
		g_Nand_RnBTransition = 0;
		g_Nand_IllegalAccError = 0;
		
		NF_nFCE_L(Controller);
		NF_CLEAR_RnB(Controller);

		for(j=0 ; j<uMultiEraseNum ; j++)
		{
			NF_CMD(Controller, 0x60);						// 1st Erase Command

			NF_ADDR(Controller, uBlockPage&0xff);	 		
			NF_ADDR(Controller, (uBlockPage>>8)&0xff);	 	
			NF_ADDR(Controller, (uBlockPage>>16)&0xff);  	
			uBlockPage += (1<<uBlockShift);
		}

		NF_CMD(Controller, 0xd0);							// 2nd Erase command

#if 1
		while(!g_Nand_RnBTransition)
		{
			if(g_Nand_IllegalAccError == 1)
			{
				//NAND_Reset(Controller);
				g_Nand_IllegalAccError = 0;
				NF_nFCE_H(Controller);
				uTemp = NAND_ReadNFCONRegister(Controller);
				uTemp &= ~((1<<10)|(1<<9));		// Illegal Access & RnB Transition Interrupt disable
				Outp32(&(NAND(Controller)->rNFCONT), uTemp);

//				INTC_Mask(NUM_NFC);				// FPGA
				INTC_Disable(NUM_NFC);
				return eNAND_EraseError;
			}
		}
#else
		NF_DETECT_RnB(Controller);
#endif


		if(NAND_Inform[Controller].uNandType == NAND_Normal8bit)
		{
			NF_CMD(Controller, 0x71); 						// Read Multi-plane status command	
		}
		else
			NF_CMD(Controller, 0x70);   						// Read status command

		while(!((uStatus = NF_RDDATA8(Controller)) & (1<<6)));

		if(uStatus & 0x01)
		{
			eError = eNAND_EraseError;		// Error in Erase
			NF_nFCE_H(Controller);
			uTemp = NAND_ReadNFCONRegister(Controller);
			uTemp &= ~((1<<10)|(1<<9));		// Illegal Access & RnB Transition Interrupt disable
			Outp32(&(NAND(Controller)->rNFCONT), uTemp);

			INTC_Disable(NUM_NFC);			
			return eError;
		}
		else
			eError = eNAND_NoError;
  
		NF_nFCE_H(Controller);
	}

	uTemp = NAND_ReadNFCONRegister(Controller);
	uTemp &= ~((1<<10)|(1<<9));		// Illegal Access & RnB Transition Interrupt disable
	Outp32(&(NAND(Controller)->rNFCONT), uTemp);

//	INTC_Mask(NUM_NFC); // FPGA
	INTC_Disable(NUM_NFC);
	
	return eError;
}


//////////
// Function Name : NAND_SoftLockingBlock
// Function Description : This function lock the blocks, then when you try to write or erase the locked area, the illegal access will be occured
// Input : 	Controller - Nand Controller Port Number 
//			uStartBlock, uEndBlock - Enable Soft-Locking the other blocks except from uStartBlock[block] to uEndBlock-1[block]
// Output : 	Nand Error Type
NAND_eERROR NAND_SoftLockingBlock(u32 Controller, u32 uStartBlock, u32 uEndBlock)
{
	u32 uTemp;

	if(NAND_Inform[Controller].uNandType == NAND_Normal8bit)
	{
		Outp32(&(NAND(Controller)->rNFSBLK), uStartBlock<<5);
		Outp32(&(NAND(Controller)->rNFEBLK), (uEndBlock)<<5);
	}
	else if(NAND_Inform[Controller].uNandType == NAND_Advanced8bit)
	{
		Outp32(&(NAND(Controller)->rNFSBLK), uStartBlock<<6);
		Outp32(&(NAND(Controller)->rNFEBLK), (uEndBlock)<<6);
	}
	else if(NAND_Inform[Controller].uNandType == NAND_MLC8bit)
	{
		Outp32(&(NAND(Controller)->rNFSBLK), uStartBlock<<7);
		Outp32(&(NAND(Controller)->rNFEBLK), (uEndBlock)<<7);
	}
	
	uTemp = NAND_ReadNFCONRegister(Controller);
	uTemp |= (1<<16);		// Enable Soft lock
	Outp32(&(NAND(Controller)->rNFCONT), uTemp);	

	return eNAND_NoError;
}


//////////
// Function Name : NAND_LockTightBlock
// Function Description : This function lock-tight the blocks, then when you try to write or erase the locked area, the illegal access will be occured
//					  You cannot disable lock-tight by software. Only reset or wakeup from sleep mode can make disable lock-tight
// Input : 	Controller - Nand Controller Port Number 
//			uStartBlock, uEndBlock - Enable Lock-tight the other blocks except from uStartBlock[block] to uEndBlock-1[block]
// Output : 	Nand Error Type
NAND_eERROR NAND_LockTightBlock(u32 Controller, u32 uStartBlock, u32 uEndBlock)
{
	u32 uTemp;

	if(NAND_Inform[Controller].uNandType == NAND_Normal8bit)
	{
		Outp32(&(NAND(Controller)->rNFSBLK), uStartBlock<<5);
		Outp32(&(NAND(Controller)->rNFEBLK), (uEndBlock)<<5);
	}
	else if(NAND_Inform[Controller].uNandType == NAND_Advanced8bit)
	{
		Outp32(&(NAND(Controller)->rNFSBLK), uStartBlock<<6);
		Outp32(&(NAND(Controller)->rNFEBLK), (uEndBlock)<<6);
	}
	else if(NAND_Inform[Controller].uNandType == NAND_MLC8bit)
	{
		Outp32(&(NAND(Controller)->rNFSBLK), uStartBlock<<7);
		Outp32(&(NAND(Controller)->rNFEBLK), (uEndBlock)<<7);
	}
	
	uTemp = NAND_ReadNFCONRegister(Controller);
	uTemp |= (1<<17);		// Enable Lock-tight
	Outp32(&(NAND(Controller)->rNFCONT), uTemp);	

	return eNAND_NoError;
}


//////////
// Function Name : NAND_UnLockBlock
// Function Description : This function unlock the soft-locked blocks
// Input : 	Controller - Nand Controller Port Number 
// Output : 	Nand Error Type
NAND_eERROR NAND_UnLockBlock(u32 Controller)
{
	u32 uTemp;

	uTemp = NAND_ReadNFCONRegister(Controller);
	if(!(uTemp & (1<<16)))
		return eNAND_NoError;
	
	Outp32(&(NAND(Controller)->rNFSBLK), 0);
	Outp32(&(NAND(Controller)->rNFEBLK), 0);
	
	//uTemp = NAND_ReadNFCONRegister(Controller);
	uTemp &= ~(1<<16);		// Disable Soft lock
	Outp32(&(NAND(Controller)->rNFCONT), uTemp);	

	uTemp = NAND_ReadNFCONRegister(Controller);

	if(uTemp & (1<<16))
		return eNAND_EtcError;
	
	return eNAND_NoError;
}


//////////
// Function Name : NAND_UnLockTightBlock
// Function Description : This function unlock the lock-tight blocks, but cannot unlock those blocks.  This is for test..!!!
// Input : 	Controller - Nand Controller Port Number 
// Output : 	Nand Error Type
NAND_eERROR NAND_UnLockTightBlock(u32 Controller)
{
	u32 uTemp;

	uTemp = NAND_ReadNFCONRegister(Controller);
	if(!(uTemp & (1<<17)))
		return eNAND_NoError;
		
	Outp32(&(NAND(Controller)->rNFSBLK), 0);
	Outp32(&(NAND(Controller)->rNFEBLK), 0);
	
	//uTemp = NAND_ReadNFCONRegister(Controller);
	uTemp &= ~(1<<17);		// Disable lock-tight. but this bit is not cleared...for test
	Outp32(&(NAND(Controller)->rNFCONT), uTemp);	

	uTemp = NAND_ReadNFCONRegister(Controller);

	if(!(uTemp & (1<<17)))
		return eNAND_EtcError;
	
	return eNAND_NoError;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////                     For ECC Error Test                   //////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

//////////
// Function Name : NAND_WritePageSLCWithInvalidData
// Function Description : This function write the 1 page invalid data test ECC Error
// Input : 	Controller - Nand Controller Port Number 
//			uBlock - Write Block
//			uPage - Write Page
//			pBuffer - Write data buffer
//			uErrorType - ECC Error Type
// Output : 	ECC Error Type 
NAND_eERROR NAND_WritePageSLCWithInvalidData(u32 Controller, u32 uBlock, u32 uPage, u8 *pBuffer, u32 uErrorType)
{
	u32 i, uBlockPage, uPageSize, uSpareSize, uTemp, uStatus;
	u8 *pErrorBuffer;
	NAND_eERROR eError;	

	pErrorBuffer = pBuffer;
	
	uTemp = NAND_ReadNFCONRegister(Controller);
	uTemp &= ~((1<<13)|(1<<12));		// 4bit ECC encoding/decoding completion Interrupt disable
	uTemp |= (1<<10) | (1<<9);		// Illegal Access & RnB Transition Interrupt enable
	Outp32(&(NAND(Controller)->rNFCONT), uTemp);
	
	Outp32(&(NAND(Controller)->rNFSTAT), (1<<5)|(1<<4));		// Illegal Access & RnB Interrupt Pending bit clear

	INTC_Enable(NUM_NFC);
	
	if(NAND_Inform[Controller].uNandType == NAND_Normal8bit)
	{
		uBlockPage=(uBlock<<5) + uPage;
		uPageSize = NAND_Inform[Controller].uPageSize;
		uSpareSize = NAND_Inform[Controller].uSpareSize;
	}
	else //if(NAND_Inform[Controller].uNandType == NAND_Advanced8bit)
	{
		uBlockPage=(uBlock<<6) + uPage;
		uPageSize = NAND_Inform[Controller].uPageSize;
		uSpareSize = NAND_Inform[Controller].uSpareSize;
	}

	//////////////////////////
	// Generate the Error Data
	//////////////////////////
	// The ECC value of valid data(1 page) has in the aNand_Spare_Data_Temp[] array
	if(uErrorType == NAND_ECCERR_1BIT)
	{
		pErrorBuffer[NAND_EccError.uEccErrByte1] ^= (1<<NAND_EccError.uEccErrBit1);		
	}
	else if(uErrorType == NAND_ECCERR_MULTI)
	{
		pErrorBuffer[NAND_EccError.uEccErrByte1] ^= (1<<NAND_EccError.uEccErrBit1);	
		pErrorBuffer[NAND_EccError.uEccErrByte2] ^= (1<<NAND_EccError.uEccErrBit2);	
	}

	//////////////////////////
	// Write 0 ~ uPageSize
	//////////////////////////
	
	NF_RSTECC(Controller);    // Initialize ECC
	NF_MECC_UnLock(Controller);
    
	NF_nFCE_L(Controller);    
	NF_CLEAR_RnB(Controller);

	NF_CMD(Controller, 0x80);	// Write Command
	NF_ADDR(Controller, 0); 
		
	//if(NAND_Inform[Controller].uNandType == NAND_Advanced8bit)
	if(NAND_Inform[Controller].uAddrCycle == 5)
		NF_ADDR(Controller, 0); 
	
	NF_ADDR(Controller, uBlockPage&0xff);		//
	NF_ADDR(Controller, (uBlockPage>>8)&0xff);	// Block & Page num.
	NF_ADDR(Controller, (uBlockPage>>16)&0xff);	//

	for(i=0 ; i<uPageSize/4 ; i++) 
	{
		NF_WRDATA(Controller, *((u32 *)pBuffer));		// Write one page
		pBuffer = pBuffer + 4;
	}

	NF_MECC_Lock(Controller);

	for(i=0 ; i<uSpareSize; i++) 
	{
		NF_WRDATA8(Controller, aNand_Spare_Data_Temp[i]);  		// The ECC value of the valid data
	}

	NF_CLEAR_RnB(Controller);
	
	g_Nand_RnBTransition = 0;	
	NF_CMD(Controller, 0x10);	 // Write 2nd command	

	NF_DETECT_RnB(Controller);
        while(!g_Nand_RnBTransition);
	
	//Read Program Status
	NF_CMD(Controller, 0x70);
	while(!((uStatus = NF_RDDATA8(Controller)) & (1<<6)));

	if(uStatus & 0x01)
		eError = eNAND_ProgramError;		// Error in Program
	else
		eError = eNAND_NoError;

	NF_nFCE_H(Controller);
	
	uTemp = Inp32(&(NAND(Controller)->rNFCONT));
	uTemp &= ~((1<<10) | (1<<9));		// Illegal Access & RnB Transition Interrupt disable
	Outp32(&(NAND(Controller)->rNFCONT), uTemp);
        INTC_Disable(NUM_NFC);
	// restore the valid data
	if(uErrorType == NAND_ECCERR_1BIT)
	{
		pErrorBuffer[NAND_EccError.uEccErrByte1] ^= (1<<NAND_EccError.uEccErrBit1);		
	}
	else if(uErrorType == NAND_ECCERR_MULTI)
	{
		pErrorBuffer[NAND_EccError.uEccErrByte1] ^= (1<<NAND_EccError.uEccErrBit1);	
		pErrorBuffer[NAND_EccError.uEccErrByte2] ^= (1<<NAND_EccError.uEccErrBit2);	
	}
	
	return eError;
}


//////////
// Function Name : NAND_WritePageMLCWithInvalidData
// Function Description : This function write the 1 page invalid data to test ECC Error
// Input : 	Controller - Nand Controller Port Number 
//			uBlock - Write Block
//			uPage - Write Page
//			pBuffer - Write data buffer
//			uErrorType - ECC Error Type
// Output : 	Nand Error Type
NAND_eERROR NAND_WritePageMLCWithInvalidData(u32 Controller, u32 uBlock, u32 uPage, u8 *pBuffer, u32 uErrorType)
{
	u32 i, uBlockPage, uSpareSize, uTemp, uStatus;
	u8 *pErrorBuffer;
	NAND_eERROR eError;
	u32 uDataPos[8]={0, }, uSparePos[8]={0, };
	u32 uMLCECCErrBytePos[8]={0, };
	u32 uMinusSpareArea;
	
	pErrorBuffer = pBuffer;
	g_Nand_IllegalAccError = 0;
	
	uTemp = NAND_ReadNFCONRegister(Controller);
	//uTemp |= (1<<13)|(1<<10)|(1<<9);		// 4bit ECC encoding & Illegal Access & RnB Transition Interrupt enable
	uTemp |= (1<<13)|(1<<12)|(1<<10)|(1<<9);		// 4bit ECC encoding & Illegal Access & RnB Transition Interrupt enable..because error
	uTemp |= (1<<18);		// Encoding 4bit ECC selection
	Outp32(&(NAND(Controller)->rNFCONT), uTemp);

	Outp32(&(NAND(Controller)->rNFSTAT), (1<<7));		// 4bit ECC encoding finish bit clear


	INTC_Enable(NUM_NFC);

	uBlockPage=(uBlock<<7) + uPage;
	uSpareSize = NAND_Inform[Controller].uSpareSize;

	//////////////////////////
	// Generate the Error Data
	//////////////////////////
	// The ECC value of valid data(512bytes) has in the aNand_Spare_Data_Temp[] array
	if(uErrorType == NAND_ECCERR_1BIT)
	{
		uMLCECCErrBytePos[0] = NAND_Inform[Controller].uMLCECCErrBytePos[0];	// 0 ~ (NAND_MLC_TRANS_SIZE + NAND_MLC_ECC_SIZE-uExceptCnt)....0 ~ 518
		uMinusSpareArea = NAND_Inform[Controller].uMLCECCPageWriteSector*NAND_MLC_ECC_SIZE;
	
		uSparePos[0] = NAND_EccError.uEccErrByte1 - ((NAND_Inform[Controller].uMLCECCPageWriteSector+1)*NAND_MLC_TRANS_SIZE) + 1;	
		uDataPos[0] = NAND_EccError.uEccErrByte1 - uMinusSpareArea;
		
		if( uMLCECCErrBytePos[0] < NAND_MLC_TRANS_SIZE)
			pErrorBuffer[uDataPos[0]] ^= (1<<NAND_EccError.uEccErrBit1);
		else
			aNand_Spare_Data_Temp[uSparePos[0]] ^= (1<<NAND_EccError.uEccErrBit1);
	}
	else if(uErrorType == NAND_ECCERR_2BIT)
	{
		uMLCECCErrBytePos[0] = NAND_Inform[Controller].uMLCECCErrBytePos[0];	// 0 ~ (NAND_MLC_TRANS_SIZE + NAND_MLC_ECC_SIZE-uExceptCnt)....0 ~ 518
		uMLCECCErrBytePos[1] = NAND_Inform[Controller].uMLCECCErrBytePos[1];	// 0 ~ (NAND_MLC_TRANS_SIZE + NAND_MLC_ECC_SIZE-uExceptCnt)....0 ~ 518
		uMinusSpareArea = NAND_Inform[Controller].uMLCECCPageWriteSector*NAND_MLC_ECC_SIZE;
		
		uSparePos[0] = NAND_EccError.uEccErrByte1 - ((NAND_Inform[Controller].uMLCECCPageWriteSector+1)*NAND_MLC_TRANS_SIZE) + 1;	
		uDataPos[0] = NAND_EccError.uEccErrByte1 - uMinusSpareArea;

		uSparePos[1] = NAND_EccError.uEccErrByte2 - ((NAND_Inform[Controller].uMLCECCPageWriteSector+1)*NAND_MLC_TRANS_SIZE) + 1;	
		uDataPos[1] = NAND_EccError.uEccErrByte2 - uMinusSpareArea;

		if( uMLCECCErrBytePos[0] < NAND_MLC_TRANS_SIZE)
			pErrorBuffer[uDataPos[0]] ^= (1<<NAND_EccError.uEccErrBit1);
		else
			aNand_Spare_Data_Temp[uSparePos[0]] ^= (1<<NAND_EccError.uEccErrBit1);

		if( uMLCECCErrBytePos[1] < NAND_MLC_TRANS_SIZE)
			pErrorBuffer[uDataPos[1]] ^= (1<<NAND_EccError.uEccErrBit2);
		else	
			aNand_Spare_Data_Temp[uSparePos[1]] ^= (1<<NAND_EccError.uEccErrBit2);
	}
	else if(uErrorType == NAND_ECCERR_3BIT)
	{
		uMLCECCErrBytePos[0] = NAND_Inform[Controller].uMLCECCErrBytePos[0];	// 0 ~ (NAND_MLC_TRANS_SIZE + NAND_MLC_ECC_SIZE-uExceptCnt)....0 ~ 518
		uMLCECCErrBytePos[1] = NAND_Inform[Controller].uMLCECCErrBytePos[1];	// 0 ~ (NAND_MLC_TRANS_SIZE + NAND_MLC_ECC_SIZE-uExceptCnt)....0 ~ 518
		uMLCECCErrBytePos[2] = NAND_Inform[Controller].uMLCECCErrBytePos[2];	// 0 ~ (NAND_MLC_TRANS_SIZE + NAND_MLC_ECC_SIZE-uExceptCnt)....0 ~ 518
		uMinusSpareArea = NAND_Inform[Controller].uMLCECCPageWriteSector*NAND_MLC_ECC_SIZE;
		
		uSparePos[0] = NAND_EccError.uEccErrByte1 - ((NAND_Inform[Controller].uMLCECCPageWriteSector+1)*NAND_MLC_TRANS_SIZE) + 1;	
		uDataPos[0] = NAND_EccError.uEccErrByte1 - uMinusSpareArea;

		uSparePos[1] = NAND_EccError.uEccErrByte2 - ((NAND_Inform[Controller].uMLCECCPageWriteSector+1)*NAND_MLC_TRANS_SIZE) + 1;	
		uDataPos[1] = NAND_EccError.uEccErrByte2 - uMinusSpareArea;

		uSparePos[2] = NAND_EccError.uEccErrByte3 - ((NAND_Inform[Controller].uMLCECCPageWriteSector+1)*NAND_MLC_TRANS_SIZE) + 1;	
		uDataPos[2] = NAND_EccError.uEccErrByte3 - uMinusSpareArea;
		
		if( uMLCECCErrBytePos[0] < NAND_MLC_TRANS_SIZE)
			pErrorBuffer[uDataPos[0]] ^= (1<<NAND_EccError.uEccErrBit1);
		else
			aNand_Spare_Data_Temp[uSparePos[0]] ^= (1<<NAND_EccError.uEccErrBit1);

		if( uMLCECCErrBytePos[1] < NAND_MLC_TRANS_SIZE)
			pErrorBuffer[uDataPos[1]] ^= (1<<NAND_EccError.uEccErrBit2);
		else	
			aNand_Spare_Data_Temp[uSparePos[1]] ^= (1<<NAND_EccError.uEccErrBit2);

		if( uMLCECCErrBytePos[2] < NAND_MLC_TRANS_SIZE)
			pErrorBuffer[uDataPos[2]] ^= (1<<NAND_EccError.uEccErrBit3);
		else	
			aNand_Spare_Data_Temp[uSparePos[2]] ^= (1<<NAND_EccError.uEccErrBit3);
	}
	else if(uErrorType == NAND_ECCERR_4BIT)
	{
		uMLCECCErrBytePos[0] = NAND_Inform[Controller].uMLCECCErrBytePos[0];	// 0 ~ (NAND_MLC_TRANS_SIZE + NAND_MLC_ECC_SIZE-uExceptCnt)....0 ~ 518
		uMLCECCErrBytePos[1] = NAND_Inform[Controller].uMLCECCErrBytePos[1];	// 0 ~ (NAND_MLC_TRANS_SIZE + NAND_MLC_ECC_SIZE-uExceptCnt)....0 ~ 518
		uMLCECCErrBytePos[2] = NAND_Inform[Controller].uMLCECCErrBytePos[2];	// 0 ~ (NAND_MLC_TRANS_SIZE + NAND_MLC_ECC_SIZE-uExceptCnt)....0 ~ 518
		uMLCECCErrBytePos[3] = NAND_Inform[Controller].uMLCECCErrBytePos[3];	// 0 ~ (NAND_MLC_TRANS_SIZE + NAND_MLC_ECC_SIZE-uExceptCnt)....0 ~ 518
		uMinusSpareArea = NAND_Inform[Controller].uMLCECCPageWriteSector*NAND_MLC_ECC_SIZE;
		
		uSparePos[0] = NAND_EccError.uEccErrByte1 - ((NAND_Inform[Controller].uMLCECCPageWriteSector+1)*NAND_MLC_TRANS_SIZE) + 1;	
		uDataPos[0] = NAND_EccError.uEccErrByte1 - uMinusSpareArea;

		uSparePos[1] = NAND_EccError.uEccErrByte2 - ((NAND_Inform[Controller].uMLCECCPageWriteSector+1)*NAND_MLC_TRANS_SIZE) + 1;	
		uDataPos[1] = NAND_EccError.uEccErrByte2 - uMinusSpareArea;

		uSparePos[2] = NAND_EccError.uEccErrByte3 - ((NAND_Inform[Controller].uMLCECCPageWriteSector+1)*NAND_MLC_TRANS_SIZE) + 1;	
		uDataPos[2] = NAND_EccError.uEccErrByte3 - uMinusSpareArea;

		uSparePos[3] = NAND_EccError.uEccErrByte4 - ((NAND_Inform[Controller].uMLCECCPageWriteSector+1)*NAND_MLC_TRANS_SIZE) + 1;	
		uDataPos[3] = NAND_EccError.uEccErrByte4 - uMinusSpareArea;
		
		if( uMLCECCErrBytePos[0] < NAND_MLC_TRANS_SIZE)
			pErrorBuffer[uDataPos[0]] ^= (1<<NAND_EccError.uEccErrBit1);
		else
			aNand_Spare_Data_Temp[uSparePos[0]] ^= (1<<NAND_EccError.uEccErrBit1);

		if( uMLCECCErrBytePos[1] < NAND_MLC_TRANS_SIZE)
			pErrorBuffer[uDataPos[1]] ^= (1<<NAND_EccError.uEccErrBit2);
		else	
			aNand_Spare_Data_Temp[uSparePos[1]] ^= (1<<NAND_EccError.uEccErrBit2);

		if( uMLCECCErrBytePos[2] < NAND_MLC_TRANS_SIZE)
			pErrorBuffer[uDataPos[2]] ^= (1<<NAND_EccError.uEccErrBit3);
		else	
			aNand_Spare_Data_Temp[uSparePos[2]] ^= (1<<NAND_EccError.uEccErrBit3);

		if( uMLCECCErrBytePos[3] < NAND_MLC_TRANS_SIZE)
			pErrorBuffer[uDataPos[3]] ^= (1<<NAND_EccError.uEccErrBit4);
		else	
			aNand_Spare_Data_Temp[uSparePos[3]] ^= (1<<NAND_EccError.uEccErrBit4);
	}
	else if(uErrorType == NAND_ECCERR_UNCORRECT)
	{
		uMLCECCErrBytePos[0] = NAND_Inform[Controller].uMLCECCErrBytePos[0];	// 0 ~ (NAND_MLC_TRANS_SIZE + NAND_MLC_ECC_SIZE-uExceptCnt)....0 ~ 518
		uMLCECCErrBytePos[1] = NAND_Inform[Controller].uMLCECCErrBytePos[1];	// 0 ~ (NAND_MLC_TRANS_SIZE + NAND_MLC_ECC_SIZE-uExceptCnt)....0 ~ 518
		uMLCECCErrBytePos[2] = NAND_Inform[Controller].uMLCECCErrBytePos[2];	// 0 ~ (NAND_MLC_TRANS_SIZE + NAND_MLC_ECC_SIZE-uExceptCnt)....0 ~ 518
		uMLCECCErrBytePos[3] = NAND_Inform[Controller].uMLCECCErrBytePos[3];	// 0 ~ (NAND_MLC_TRANS_SIZE + NAND_MLC_ECC_SIZE-uExceptCnt)....0 ~ 518
		uMLCECCErrBytePos[4] = NAND_Inform[Controller].uMLCECCErrBytePos[4];	// 0 ~ (NAND_MLC_TRANS_SIZE + NAND_MLC_ECC_SIZE-uExceptCnt)....0 ~ 518
		uMinusSpareArea = NAND_Inform[Controller].uMLCECCPageWriteSector*NAND_MLC_ECC_SIZE;
		
		uSparePos[0] = NAND_EccError.uEccErrByte1 - ((NAND_Inform[Controller].uMLCECCPageWriteSector+1)*NAND_MLC_TRANS_SIZE) + 1;	
		uDataPos[0] = NAND_EccError.uEccErrByte1 - uMinusSpareArea;

		uSparePos[1] = NAND_EccError.uEccErrByte2 - ((NAND_Inform[Controller].uMLCECCPageWriteSector+1)*NAND_MLC_TRANS_SIZE) + 1;	
		uDataPos[1] = NAND_EccError.uEccErrByte2 - uMinusSpareArea;

		uSparePos[2] = NAND_EccError.uEccErrByte3 - ((NAND_Inform[Controller].uMLCECCPageWriteSector+1)*NAND_MLC_TRANS_SIZE) + 1;	
		uDataPos[2] = NAND_EccError.uEccErrByte3 - uMinusSpareArea;

		uSparePos[3] = NAND_EccError.uEccErrByte4 - ((NAND_Inform[Controller].uMLCECCPageWriteSector+1)*NAND_MLC_TRANS_SIZE) + 1;	
		uDataPos[3] = NAND_EccError.uEccErrByte4 - uMinusSpareArea;

		uSparePos[4] = NAND_EccError.uEccErrByte5 - ((NAND_Inform[Controller].uMLCECCPageWriteSector+1)*NAND_MLC_TRANS_SIZE) + 1;	
		uDataPos[4] = NAND_EccError.uEccErrByte5 - uMinusSpareArea;
		
		if( uMLCECCErrBytePos[0] < NAND_MLC_TRANS_SIZE)
			pErrorBuffer[uDataPos[0]] ^= (1<<NAND_EccError.uEccErrBit1);
		else
			aNand_Spare_Data_Temp[uSparePos[0]] ^= (1<<NAND_EccError.uEccErrBit1);

		if( uMLCECCErrBytePos[1] < NAND_MLC_TRANS_SIZE)
			pErrorBuffer[uDataPos[1]] ^= (1<<NAND_EccError.uEccErrBit2);
		else	
			aNand_Spare_Data_Temp[uSparePos[1]] ^= (1<<NAND_EccError.uEccErrBit2);

		if( uMLCECCErrBytePos[2] < NAND_MLC_TRANS_SIZE)
			pErrorBuffer[uDataPos[2]] ^= (1<<NAND_EccError.uEccErrBit3);
		else	
			aNand_Spare_Data_Temp[uSparePos[2]] ^= (1<<NAND_EccError.uEccErrBit3);

		if( uMLCECCErrBytePos[3] < NAND_MLC_TRANS_SIZE)
			pErrorBuffer[uDataPos[3]] ^= (1<<NAND_EccError.uEccErrBit4);
		else	
			aNand_Spare_Data_Temp[uSparePos[3]] ^= (1<<NAND_EccError.uEccErrBit4);

		if( uMLCECCErrBytePos[4] < NAND_MLC_TRANS_SIZE)
			pErrorBuffer[uDataPos[4]] ^= (1<<NAND_EccError.uEccErrBit5);
		else	
			aNand_Spare_Data_Temp[uSparePos[4]] ^= (1<<NAND_EccError.uEccErrBit5);
	}
	else if(uErrorType == NAND_ECCERR_UNCORRECT6BIT)
	{
		uMLCECCErrBytePos[0] = NAND_Inform[Controller].uMLCECCErrBytePos[0];	// 0 ~ (NAND_MLC_TRANS_SIZE + NAND_MLC_ECC_SIZE-uExceptCnt)....0 ~ 518
		uMLCECCErrBytePos[1] = NAND_Inform[Controller].uMLCECCErrBytePos[1];	// 0 ~ (NAND_MLC_TRANS_SIZE + NAND_MLC_ECC_SIZE-uExceptCnt)....0 ~ 518
		uMLCECCErrBytePos[2] = NAND_Inform[Controller].uMLCECCErrBytePos[2];	// 0 ~ (NAND_MLC_TRANS_SIZE + NAND_MLC_ECC_SIZE-uExceptCnt)....0 ~ 518
		uMLCECCErrBytePos[3] = NAND_Inform[Controller].uMLCECCErrBytePos[3];	// 0 ~ (NAND_MLC_TRANS_SIZE + NAND_MLC_ECC_SIZE-uExceptCnt)....0 ~ 518
		uMLCECCErrBytePos[4] = NAND_Inform[Controller].uMLCECCErrBytePos[4];	// 0 ~ (NAND_MLC_TRANS_SIZE + NAND_MLC_ECC_SIZE-uExceptCnt)....0 ~ 518
		uMLCECCErrBytePos[5] = NAND_Inform[Controller].uMLCECCErrBytePos[5];	// 0 ~ (NAND_MLC_TRANS_SIZE + NAND_MLC_ECC_SIZE-uExceptCnt)....0 ~ 518

		uMinusSpareArea = NAND_Inform[Controller].uMLCECCPageWriteSector*NAND_MLC_ECC_SIZE;
		
		uSparePos[0] = NAND_EccError.uEccErrByte1 - ((NAND_Inform[Controller].uMLCECCPageWriteSector+1)*NAND_MLC_TRANS_SIZE) + 1;	
		uDataPos[0] = NAND_EccError.uEccErrByte1 - uMinusSpareArea;

		uSparePos[1] = NAND_EccError.uEccErrByte2 - ((NAND_Inform[Controller].uMLCECCPageWriteSector+1)*NAND_MLC_TRANS_SIZE) + 1;	
		uDataPos[1] = NAND_EccError.uEccErrByte2 - uMinusSpareArea;

		uSparePos[2] = NAND_EccError.uEccErrByte3 - ((NAND_Inform[Controller].uMLCECCPageWriteSector+1)*NAND_MLC_TRANS_SIZE) + 1;	
		uDataPos[2] = NAND_EccError.uEccErrByte3 - uMinusSpareArea;

		uSparePos[3] = NAND_EccError.uEccErrByte4 - ((NAND_Inform[Controller].uMLCECCPageWriteSector+1)*NAND_MLC_TRANS_SIZE) + 1;	
		uDataPos[3] = NAND_EccError.uEccErrByte4 - uMinusSpareArea;

		uSparePos[4] = NAND_EccError.uEccErrByte5 - ((NAND_Inform[Controller].uMLCECCPageWriteSector+1)*NAND_MLC_TRANS_SIZE) + 1;	
		uDataPos[4] = NAND_EccError.uEccErrByte5 - uMinusSpareArea;

		uSparePos[5] = NAND_EccError.uEccErrByte6 - ((NAND_Inform[Controller].uMLCECCPageWriteSector+1)*NAND_MLC_TRANS_SIZE) + 1;	
		uDataPos[5] = NAND_EccError.uEccErrByte6 - uMinusSpareArea;
		
		if( uMLCECCErrBytePos[0] < NAND_MLC_TRANS_SIZE)
			pErrorBuffer[uDataPos[0]] ^= (1<<NAND_EccError.uEccErrBit1);
		else
			aNand_Spare_Data_Temp[uSparePos[0]] ^= (1<<NAND_EccError.uEccErrBit1);

		if( uMLCECCErrBytePos[1] < NAND_MLC_TRANS_SIZE)
			pErrorBuffer[uDataPos[1]] ^= (1<<NAND_EccError.uEccErrBit2);
		else	
			aNand_Spare_Data_Temp[uSparePos[1]] ^= (1<<NAND_EccError.uEccErrBit2);

		if( uMLCECCErrBytePos[2] < NAND_MLC_TRANS_SIZE)
			pErrorBuffer[uDataPos[2]] ^= (1<<NAND_EccError.uEccErrBit3);
		else	
			aNand_Spare_Data_Temp[uSparePos[2]] ^= (1<<NAND_EccError.uEccErrBit3);

		if( uMLCECCErrBytePos[3] < NAND_MLC_TRANS_SIZE)
			pErrorBuffer[uDataPos[3]] ^= (1<<NAND_EccError.uEccErrBit4);
		else	
			aNand_Spare_Data_Temp[uSparePos[3]] ^= (1<<NAND_EccError.uEccErrBit4);

		if( uMLCECCErrBytePos[4] < NAND_MLC_TRANS_SIZE)
			pErrorBuffer[uDataPos[4]] ^= (1<<NAND_EccError.uEccErrBit5);
		else	
			aNand_Spare_Data_Temp[uSparePos[4]] ^= (1<<NAND_EccError.uEccErrBit5);
		if( uMLCECCErrBytePos[5] < NAND_MLC_TRANS_SIZE)
			pErrorBuffer[uDataPos[5]] ^= (1<<NAND_EccError.uEccErrBit6);
		else	
			aNand_Spare_Data_Temp[uSparePos[5]] ^= (1<<NAND_EccError.uEccErrBit6);		
	}
	else //if(uErrorType == NAND_ECCERR_UNCORRECT7BIT)
	{
		uMLCECCErrBytePos[0] = NAND_Inform[Controller].uMLCECCErrBytePos[0];	// 0 ~ (NAND_MLC_TRANS_SIZE + NAND_MLC_ECC_SIZE-uExceptCnt)....0 ~ 518
		uMLCECCErrBytePos[1] = NAND_Inform[Controller].uMLCECCErrBytePos[1];	// 0 ~ (NAND_MLC_TRANS_SIZE + NAND_MLC_ECC_SIZE-uExceptCnt)....0 ~ 518
		uMLCECCErrBytePos[2] = NAND_Inform[Controller].uMLCECCErrBytePos[2];	// 0 ~ (NAND_MLC_TRANS_SIZE + NAND_MLC_ECC_SIZE-uExceptCnt)....0 ~ 518
		uMLCECCErrBytePos[3] = NAND_Inform[Controller].uMLCECCErrBytePos[3];	// 0 ~ (NAND_MLC_TRANS_SIZE + NAND_MLC_ECC_SIZE-uExceptCnt)....0 ~ 518
		uMLCECCErrBytePos[4] = NAND_Inform[Controller].uMLCECCErrBytePos[4];	// 0 ~ (NAND_MLC_TRANS_SIZE + NAND_MLC_ECC_SIZE-uExceptCnt)....0 ~ 518
		uMLCECCErrBytePos[5] = NAND_Inform[Controller].uMLCECCErrBytePos[5];	// 0 ~ (NAND_MLC_TRANS_SIZE + NAND_MLC_ECC_SIZE-uExceptCnt)....0 ~ 518
		uMLCECCErrBytePos[6] = NAND_Inform[Controller].uMLCECCErrBytePos[6];	// 0 ~ (NAND_MLC_TRANS_SIZE + NAND_MLC_ECC_SIZE-uExceptCnt)....0 ~ 518

		uMinusSpareArea = NAND_Inform[Controller].uMLCECCPageWriteSector*NAND_MLC_ECC_SIZE;
		
		uSparePos[0] = NAND_EccError.uEccErrByte1 - ((NAND_Inform[Controller].uMLCECCPageWriteSector+1)*NAND_MLC_TRANS_SIZE) + 1;	
		uDataPos[0] = NAND_EccError.uEccErrByte1 - uMinusSpareArea;

		uSparePos[1] = NAND_EccError.uEccErrByte2 - ((NAND_Inform[Controller].uMLCECCPageWriteSector+1)*NAND_MLC_TRANS_SIZE) + 1;	
		uDataPos[1] = NAND_EccError.uEccErrByte2 - uMinusSpareArea;

		uSparePos[2] = NAND_EccError.uEccErrByte3 - ((NAND_Inform[Controller].uMLCECCPageWriteSector+1)*NAND_MLC_TRANS_SIZE) + 1;	
		uDataPos[2] = NAND_EccError.uEccErrByte3 - uMinusSpareArea;

		uSparePos[3] = NAND_EccError.uEccErrByte4 - ((NAND_Inform[Controller].uMLCECCPageWriteSector+1)*NAND_MLC_TRANS_SIZE) + 1;	
		uDataPos[3] = NAND_EccError.uEccErrByte4 - uMinusSpareArea;

		uSparePos[4] = NAND_EccError.uEccErrByte5 - ((NAND_Inform[Controller].uMLCECCPageWriteSector+1)*NAND_MLC_TRANS_SIZE) + 1;	
		uDataPos[4] = NAND_EccError.uEccErrByte5 - uMinusSpareArea;

		uSparePos[5] = NAND_EccError.uEccErrByte6 - ((NAND_Inform[Controller].uMLCECCPageWriteSector+1)*NAND_MLC_TRANS_SIZE) + 1;	
		uDataPos[5] = NAND_EccError.uEccErrByte6 - uMinusSpareArea;

		uSparePos[6] = NAND_EccError.uEccErrByte7 - ((NAND_Inform[Controller].uMLCECCPageWriteSector+1)*NAND_MLC_TRANS_SIZE) + 1;	
		uDataPos[6] = NAND_EccError.uEccErrByte7 - uMinusSpareArea;
		
		if( uMLCECCErrBytePos[0] < NAND_MLC_TRANS_SIZE)
			pErrorBuffer[uDataPos[0]] ^= (1<<NAND_EccError.uEccErrBit1);
		else
			aNand_Spare_Data_Temp[uSparePos[0]] ^= (1<<NAND_EccError.uEccErrBit1);

		if( uMLCECCErrBytePos[1] < NAND_MLC_TRANS_SIZE)
			pErrorBuffer[uDataPos[1]] ^= (1<<NAND_EccError.uEccErrBit2);
		else	
			aNand_Spare_Data_Temp[uSparePos[1]] ^= (1<<NAND_EccError.uEccErrBit2);

		if( uMLCECCErrBytePos[2] < NAND_MLC_TRANS_SIZE)
			pErrorBuffer[uDataPos[2]] ^= (1<<NAND_EccError.uEccErrBit3);
		else	
			aNand_Spare_Data_Temp[uSparePos[2]] ^= (1<<NAND_EccError.uEccErrBit3);

		if( uMLCECCErrBytePos[3] < NAND_MLC_TRANS_SIZE)
			pErrorBuffer[uDataPos[3]] ^= (1<<NAND_EccError.uEccErrBit4);
		else	
			aNand_Spare_Data_Temp[uSparePos[3]] ^= (1<<NAND_EccError.uEccErrBit4);

		if( uMLCECCErrBytePos[4] < NAND_MLC_TRANS_SIZE)
			pErrorBuffer[uDataPos[4]] ^= (1<<NAND_EccError.uEccErrBit5);
		else	
			aNand_Spare_Data_Temp[uSparePos[4]] ^= (1<<NAND_EccError.uEccErrBit5);
		
		if( uMLCECCErrBytePos[5] < NAND_MLC_TRANS_SIZE)
			pErrorBuffer[uDataPos[5]] ^= (1<<NAND_EccError.uEccErrBit6);
		else	
			aNand_Spare_Data_Temp[uSparePos[5]] ^= (1<<NAND_EccError.uEccErrBit6);		

		if( uMLCECCErrBytePos[6] < NAND_MLC_TRANS_SIZE)
			pErrorBuffer[uDataPos[6]] ^= (1<<NAND_EccError.uEccErrBit7);
		else	
			aNand_Spare_Data_Temp[uSparePos[6]] ^= (1<<NAND_EccError.uEccErrBit7);				
	}

	
	//////////////////////////
	// Write 0 ~ 511
	//////////////////////////
	g_Nand_4bitEccEncDone = 0;
	
	NF_RSTECC(Controller);    // Initialize ECC
	NF_MECC_UnLock(Controller);
    
	NF_nFCE_L(Controller);    

#if (NAND_REG_BUG == TRUE)
	uTemp = NAND_ReadNFCONRegister(Controller);
	uTemp |= (1<<12)|(1<<13);		// 4bit ECC decoding/encoding Interrupt enable...because error
	Outp32(&(NAND(Controller)->rNFCONT), uTemp);	
#endif

	NF_CLEAR_RnB(Controller);

	NF_CMD(Controller, 0x00);	// 1st Read command..????
	NF_CMD(Controller, 0x80);	// 1st Write command

	NF_ADDR(Controller, 0); 	
	NF_ADDR(Controller, 0); 	
	NF_ADDR(Controller, uBlockPage&0xff);		//
	NF_ADDR(Controller, (uBlockPage>>8)&0xff);	// Block & Page num.
	NF_ADDR(Controller, (uBlockPage>>16)&0xff);	//

	for(i=0 ; i<NAND_MLC_TRANS_SIZE ; i++) 	// MLC ECC Size : 512bytes
	{
		NF_WRDATA8(Controller, *pBuffer++);		// Write one page
	}

	//NF_MECC_Lock(Controller);

//EVT1 NFCONT[13] bit has no read path....Not check ECC encoding completion
//	while(!g_Nand_4bitEccEncDone);

	//////////////////////////
	// Write 512 ~ 1023
	//////////////////////////
	g_Nand_4bitEccEncDone = 0;
	NF_RSTECC(Controller);    // Initialize ECC

#if (NAND_REG_BUG == TRUE)
	uTemp = NAND_ReadNFCONRegister(Controller);
	uTemp |= (1<<12)|(1<<13);		// 4bit ECC decoding/encoding Interrupt enable...because error
	Outp32(&(NAND(Controller)->rNFCONT), uTemp);	
#endif

	for(i=0 ; i<NAND_MLC_TRANS_SIZE ; i++) 	// MLC ECC Size : 512bytes
	{
		NF_WRDATA8(Controller, *pBuffer++);		// Write one page
	}

//EVT1 NFCONT[13] bit has no read path....Not check ECC encoding completion
//	while(!g_Nand_4bitEccEncDone);

	//////////////////////////
	// Write 1024 ~ 1535
	//////////////////////////
	g_Nand_4bitEccEncDone = 0;
	NF_RSTECC(Controller);    // Initialize ECC

#if (NAND_REG_BUG == TRUE)
	uTemp = NAND_ReadNFCONRegister(Controller);
	uTemp |= (1<<12)|(1<<13);		// 4bit ECC decoding/encoding Interrupt enable...because error
	Outp32(&(NAND(Controller)->rNFCONT), uTemp);	
#endif

	for(i=0 ; i<NAND_MLC_TRANS_SIZE ; i++) 	// MLC ECC Size : 512bytes
	{
		NF_WRDATA8(Controller, *pBuffer++);		// Write one page
	}

//EVT1 NFCONT[13] bit has no read path....Not check ECC encoding completion
//	while(!g_Nand_4bitEccEncDone);
	
	//////////////////////////
	// Write 1536 ~ 2047
	//////////////////////////
	g_Nand_4bitEccEncDone = 0;
	NF_RSTECC(Controller);    // Initialize ECC

#if (NAND_REG_BUG == TRUE)
	uTemp = NAND_ReadNFCONRegister(Controller);
	uTemp |= (1<<12)|(1<<13);		// 4bit ECC decoding/encoding Interrupt enable...because error
	Outp32(&(NAND(Controller)->rNFCONT), uTemp);	
#endif

	for(i=0 ; i<NAND_MLC_TRANS_SIZE ; i++) 	// MLC ECC Size : 512bytes
	{
		NF_WRDATA8(Controller, *pBuffer++);		// Write one page
	}

//EVT1 NFCONT[13] bit has no read path....Not check ECC encoding completion
//	while(!g_Nand_4bitEccEncDone);

	//////////////////////////
	// Spare Area Write
	//////////////////////////	
	g_Nand_RnBTransition = 0;
	
	for(i=0 ; i<uSpareSize ; i++)
		NF_WRDATA8(Controller, aNand_Spare_Data_Temp[i]);		// The ECC value of the valid data

	NF_CMD(Controller, 0x10);	// 2nd Write Command

	while(!g_Nand_RnBTransition)
	{
		if(g_Nand_IllegalAccError == 1)
		{
			//NAND_Reset(Controller);
			g_Nand_IllegalAccError = 0;
			NF_CMD(Controller, 0x70);

			NF_nFCE_H(Controller);
			
			uTemp = NAND_ReadNFCONRegister(Controller);
			uTemp &= ~((1<<13)|(1<<12)|(1<<10)|(1<<9));		// 4bit ECC encoding & Illegal Access & RnB Transition Interrupt disable
			Outp32(&(NAND(Controller)->rNFCONT), uTemp);

			INTC_Disable(NUM_NFC);				
			return eNAND_ProgramError;
		}
	}
	
	//Read Program Status
	NF_CMD(Controller, 0x70);

	while(!((uStatus = NF_RDDATA8(Controller)) & (1<<6)));
	if(uStatus & 0x01)
		eError = eNAND_ProgramError;		// Error in Program
	else
		eError = eNAND_NoError;

	NF_nFCE_H(Controller);
	
	uTemp = NAND_ReadNFCONRegister(Controller);
	uTemp &= ~((1<<13)|(1<<12)|(1<<10)|(1<<9));		// 4bit ECC encoding & Illegal Access & RnB Transition Interrupt disable
	Outp32(&(NAND(Controller)->rNFCONT), uTemp);

	INTC_Disable(NUM_NFC);

	// restore the valid data
	if(uErrorType == NAND_ECCERR_1BIT)
	{
		if( uMLCECCErrBytePos[0] < NAND_MLC_TRANS_SIZE)
			pErrorBuffer[uDataPos[0]] ^= (1<<NAND_EccError.uEccErrBit1);
		else
			aNand_Spare_Data_Temp[uSparePos[0]] ^= (1<<NAND_EccError.uEccErrBit1);
	}
	else if(uErrorType == NAND_ECCERR_2BIT)
	{
		if( uMLCECCErrBytePos[0] < NAND_MLC_TRANS_SIZE)
			pErrorBuffer[uDataPos[0]] ^= (1<<NAND_EccError.uEccErrBit1);
		else
			aNand_Spare_Data_Temp[uSparePos[0]] ^= (1<<NAND_EccError.uEccErrBit1);

		if( uMLCECCErrBytePos[1] < NAND_MLC_TRANS_SIZE)
			pErrorBuffer[uDataPos[1]] ^= (1<<NAND_EccError.uEccErrBit2);
		else	
			aNand_Spare_Data_Temp[uSparePos[1]] ^= (1<<NAND_EccError.uEccErrBit2);	
	}
	else if(uErrorType == NAND_ECCERR_3BIT)
	{
		if( uMLCECCErrBytePos[0] < NAND_MLC_TRANS_SIZE)
			pErrorBuffer[uDataPos[0]] ^= (1<<NAND_EccError.uEccErrBit1);
		else
			aNand_Spare_Data_Temp[uSparePos[0]] ^= (1<<NAND_EccError.uEccErrBit1);

		if( uMLCECCErrBytePos[1] < NAND_MLC_TRANS_SIZE)
			pErrorBuffer[uDataPos[1]] ^= (1<<NAND_EccError.uEccErrBit2);
		else	
			aNand_Spare_Data_Temp[uSparePos[1]] ^= (1<<NAND_EccError.uEccErrBit2);

		if( uMLCECCErrBytePos[2] < NAND_MLC_TRANS_SIZE)
			pErrorBuffer[uDataPos[2]] ^= (1<<NAND_EccError.uEccErrBit3);
		else	
			aNand_Spare_Data_Temp[uSparePos[2]] ^= (1<<NAND_EccError.uEccErrBit3);
	}
	else if(uErrorType == NAND_ECCERR_4BIT)
	{
		if( uMLCECCErrBytePos[0] < NAND_MLC_TRANS_SIZE)
			pErrorBuffer[uDataPos[0]] ^= (1<<NAND_EccError.uEccErrBit1);
		else
			aNand_Spare_Data_Temp[uSparePos[0]] ^= (1<<NAND_EccError.uEccErrBit1);

		if( uMLCECCErrBytePos[1] < NAND_MLC_TRANS_SIZE)
			pErrorBuffer[uDataPos[1]] ^= (1<<NAND_EccError.uEccErrBit2);
		else	
			aNand_Spare_Data_Temp[uSparePos[1]] ^= (1<<NAND_EccError.uEccErrBit2);

		if( uMLCECCErrBytePos[2] < NAND_MLC_TRANS_SIZE)
			pErrorBuffer[uDataPos[2]] ^= (1<<NAND_EccError.uEccErrBit3);
		else	
			aNand_Spare_Data_Temp[uSparePos[2]] ^= (1<<NAND_EccError.uEccErrBit3);

		if( uMLCECCErrBytePos[3] < NAND_MLC_TRANS_SIZE)
			pErrorBuffer[uDataPos[3]] ^= (1<<NAND_EccError.uEccErrBit4);
		else	
			aNand_Spare_Data_Temp[uSparePos[3]] ^= (1<<NAND_EccError.uEccErrBit4);
	}
	else if(uErrorType == NAND_ECCERR_UNCORRECT)
	{
		if( uMLCECCErrBytePos[0] < NAND_MLC_TRANS_SIZE)
			pErrorBuffer[uDataPos[0]] ^= (1<<NAND_EccError.uEccErrBit1);
		else
			aNand_Spare_Data_Temp[uSparePos[0]] ^= (1<<NAND_EccError.uEccErrBit1);

		if( uMLCECCErrBytePos[1] < NAND_MLC_TRANS_SIZE)
			pErrorBuffer[uDataPos[1]] ^= (1<<NAND_EccError.uEccErrBit2);
		else	
			aNand_Spare_Data_Temp[uSparePos[1]] ^= (1<<NAND_EccError.uEccErrBit2);

		if( uMLCECCErrBytePos[2] < NAND_MLC_TRANS_SIZE)
			pErrorBuffer[uDataPos[2]] ^= (1<<NAND_EccError.uEccErrBit3);
		else	
			aNand_Spare_Data_Temp[uSparePos[2]] ^= (1<<NAND_EccError.uEccErrBit3);

		if( uMLCECCErrBytePos[3] < NAND_MLC_TRANS_SIZE)
			pErrorBuffer[uDataPos[3]] ^= (1<<NAND_EccError.uEccErrBit4);
		else	
			aNand_Spare_Data_Temp[uSparePos[3]] ^= (1<<NAND_EccError.uEccErrBit4);

		if( uMLCECCErrBytePos[4] < NAND_MLC_TRANS_SIZE)
			pErrorBuffer[uDataPos[4]] ^= (1<<NAND_EccError.uEccErrBit5);
		else	
			aNand_Spare_Data_Temp[uSparePos[4]] ^= (1<<NAND_EccError.uEccErrBit5);
	}
	else if(uErrorType == NAND_ECCERR_UNCORRECT6BIT)
	{
		if( uMLCECCErrBytePos[0] < NAND_MLC_TRANS_SIZE)
			pErrorBuffer[uDataPos[0]] ^= (1<<NAND_EccError.uEccErrBit1);
		else
			aNand_Spare_Data_Temp[uSparePos[0]] ^= (1<<NAND_EccError.uEccErrBit1);

		if( uMLCECCErrBytePos[1] < NAND_MLC_TRANS_SIZE)
			pErrorBuffer[uDataPos[1]] ^= (1<<NAND_EccError.uEccErrBit2);
		else	
			aNand_Spare_Data_Temp[uSparePos[1]] ^= (1<<NAND_EccError.uEccErrBit2);

		if( uMLCECCErrBytePos[2] < NAND_MLC_TRANS_SIZE)
			pErrorBuffer[uDataPos[2]] ^= (1<<NAND_EccError.uEccErrBit3);
		else	
			aNand_Spare_Data_Temp[uSparePos[2]] ^= (1<<NAND_EccError.uEccErrBit3);

		if( uMLCECCErrBytePos[3] < NAND_MLC_TRANS_SIZE)
			pErrorBuffer[uDataPos[3]] ^= (1<<NAND_EccError.uEccErrBit4);
		else	
			aNand_Spare_Data_Temp[uSparePos[3]] ^= (1<<NAND_EccError.uEccErrBit4);

		if( uMLCECCErrBytePos[4] < NAND_MLC_TRANS_SIZE)
			pErrorBuffer[uDataPos[4]] ^= (1<<NAND_EccError.uEccErrBit5);
		else	
			aNand_Spare_Data_Temp[uSparePos[4]] ^= (1<<NAND_EccError.uEccErrBit5);

		if( uMLCECCErrBytePos[5] < NAND_MLC_TRANS_SIZE)
			pErrorBuffer[uDataPos[5]] ^= (1<<NAND_EccError.uEccErrBit6);
		else	
			aNand_Spare_Data_Temp[uSparePos[5]] ^= (1<<NAND_EccError.uEccErrBit6);		
	}
	else //if(uErrorType == NAND_ECCERR_UNCORRECT7BIT)
	{
		if( uMLCECCErrBytePos[0] < NAND_MLC_TRANS_SIZE)
			pErrorBuffer[uDataPos[0]] ^= (1<<NAND_EccError.uEccErrBit1);
		else
			aNand_Spare_Data_Temp[uSparePos[0]] ^= (1<<NAND_EccError.uEccErrBit1);

		if( uMLCECCErrBytePos[1] < NAND_MLC_TRANS_SIZE)
			pErrorBuffer[uDataPos[1]] ^= (1<<NAND_EccError.uEccErrBit2);
		else	
			aNand_Spare_Data_Temp[uSparePos[1]] ^= (1<<NAND_EccError.uEccErrBit2);

		if( uMLCECCErrBytePos[2] < NAND_MLC_TRANS_SIZE)
			pErrorBuffer[uDataPos[2]] ^= (1<<NAND_EccError.uEccErrBit3);
		else	
			aNand_Spare_Data_Temp[uSparePos[2]] ^= (1<<NAND_EccError.uEccErrBit3);

		if( uMLCECCErrBytePos[3] < NAND_MLC_TRANS_SIZE)
			pErrorBuffer[uDataPos[3]] ^= (1<<NAND_EccError.uEccErrBit4);
		else	
			aNand_Spare_Data_Temp[uSparePos[3]] ^= (1<<NAND_EccError.uEccErrBit4);

		if( uMLCECCErrBytePos[4] < NAND_MLC_TRANS_SIZE)
			pErrorBuffer[uDataPos[4]] ^= (1<<NAND_EccError.uEccErrBit5);
		else	
			aNand_Spare_Data_Temp[uSparePos[4]] ^= (1<<NAND_EccError.uEccErrBit5);

		if( uMLCECCErrBytePos[5] < NAND_MLC_TRANS_SIZE)
			pErrorBuffer[uDataPos[5]] ^= (1<<NAND_EccError.uEccErrBit6);
		else	
			aNand_Spare_Data_Temp[uSparePos[5]] ^= (1<<NAND_EccError.uEccErrBit6);		

		if( uMLCECCErrBytePos[6] < NAND_MLC_TRANS_SIZE)
			pErrorBuffer[uDataPos[6]] ^= (1<<NAND_EccError.uEccErrBit7);
		else	
			aNand_Spare_Data_Temp[uSparePos[6]] ^= (1<<NAND_EccError.uEccErrBit7);
	}
	
	return eError;
}


//////////
// Function Name : NAND_WritePageWithInvalidData
// Function Description : This function write the 1 page invalid data to test ECC Error
// Input : 	Controller - Nand Controller Port Number 
//			uBlock - Write Block
//			uPage - Write Page
//			pBuffer - Write data buffer
//			uErrorType - Error Type to test
// Output : 	Nand Error Type
NAND_eERROR NAND_WritePageWithInvalidData(u32 Controller, u32 uBlock, u32 uPage, u8 *pBuffer, u32 uErrorType)
{
	NAND_eERROR eError;

	if( (NAND_Inform[Controller].uNandType == NAND_Normal8bit) || (NAND_Inform[Controller].uNandType == NAND_Advanced8bit) )
		eError = NAND_WritePageSLCWithInvalidData(Controller, uBlock, uPage, pBuffer, uErrorType);
	else //if(NAND_Inform[Controller].uNandType == NAND_MLC8bit)
		eError = NAND_WritePageMLCWithInvalidData(Controller, uBlock, uPage, pBuffer, uErrorType);

	return eError;
}



//////////
// Function Name : NAND_WritePageSLCWithInvalidSpareData
// Function Description : This function write the 1 page invalid spare data test ECC Error
// Input : 	Controller - Nand Controller Port Number 
//			uBlock - Write Block
//			uPage - Write Page
//			pBuffer - Write data buffer
//			uErrorType - ECC Error Type
// Output : 	ECC Error Type 
NAND_eERROR NAND_WritePageSLCWithInvalidSpareData(u32 Controller, u32 uBlock, u32 uPage, u8 *pBuffer, u32 uErrorType)
{
	u32 i, uBlockPage, uPageSize, uSpareSize, uTemp, uStatus;
	//u8 *pErrorBuffer;
	NAND_eERROR eError;	

	//pErrorBuffer = pBuffer;
	
	uTemp = NAND_ReadNFCONRegister(Controller);
	uTemp &= ~((1<<13)|(1<<12));		// 4bit ECC encoding/decoding completion Interrupt disable
	uTemp |= (1<<10) | (1<<9);		// Illegal Access & RnB Transition Interrupt enable
	Outp32(&(NAND(Controller)->rNFCONT), uTemp);
	
	Outp32(&(NAND(Controller)->rNFSTAT), (1<<5)|(1<<4));		// Illegal Access & RnB Interrupt Pending bit clear

	INTC_Enable(NUM_NFC);
	
	if(NAND_Inform[Controller].uNandType == NAND_Normal8bit)
	{
		uBlockPage=(uBlock<<5) + uPage;
		uPageSize = NAND_Inform[Controller].uPageSize;
		uSpareSize = NAND_Inform[Controller].uSpareSize;
	}
	else //if(NAND_Inform[Controller].uNandType == NAND_Advanced8bit)
	{
		uBlockPage=(uBlock<<6) + uPage;
		uPageSize = NAND_Inform[Controller].uPageSize;
		uSpareSize = NAND_Inform[Controller].uSpareSize;
	}

	//////////////////////////
	// Generate the Error Data
	//////////////////////////
	// Change to invalid data
	if(uErrorType == NAND_ECCERR_1BIT)
	{
		aNand_Spare_Data_Temp[NAND_EccError.uEccErrByte1] ^= (1<<NAND_EccError.uEccErrBit1);		
	}
	else if(uErrorType == NAND_ECCERR_MULTI)
	{
		aNand_Spare_Data_Temp[NAND_EccError.uEccErrByte1] ^= (1<<NAND_EccError.uEccErrBit1);	
		aNand_Spare_Data_Temp[NAND_EccError.uEccErrByte2] ^= (1<<NAND_EccError.uEccErrBit2);	
	}

	//////////////////////////
	// Write 0 ~ uPageSize
	//////////////////////////
	
	NF_RSTECC(Controller);    // Initialize ECC
	NF_MECC_UnLock(Controller);
    
	NF_nFCE_L(Controller);    
	NF_CLEAR_RnB(Controller);

	NF_CMD(Controller, 0x80);	// Read command
	NF_ADDR(Controller, 0); 
		
	//if(NAND_Inform[Controller].uNandType == NAND_Advanced8bit)
	if(NAND_Inform[Controller].uAddrCycle == 5)
		NF_ADDR(Controller, 0); 
	
	NF_ADDR(Controller, uBlockPage&0xff);		//
	NF_ADDR(Controller, (uBlockPage>>8)&0xff);	// Block & Page num.
	NF_ADDR(Controller, (uBlockPage>>16)&0xff);	//

	for(i=0 ; i<uPageSize ; i++) 
	{
		NF_WRDATA8(Controller, *pBuffer++);		// Write one page
	}

	NF_MECC_Lock(Controller);

	for(i=0 ; i<uSpareSize; i++) 
	{
		NF_WRDATA8(Controller, aNand_Spare_Data_Temp[i]);  		// The ECC value of the valid data
	}

	NF_CLEAR_RnB(Controller);
	
	g_Nand_RnBTransition = 0;	
	NF_CMD(Controller, 0x10);	 // Write 2nd command	

	while(!g_Nand_RnBTransition);
	
	//Read Program Status
	NF_CMD(Controller, 0x70);

	while(!((uStatus = NF_RDDATA8(Controller)) & (1<<6)));
	if(uStatus & 0x01)
		eError = eNAND_ProgramError;		// Error in Program
	else
		eError = eNAND_NoError;

	NF_nFCE_H(Controller);
	
	uTemp = NAND_ReadNFCONRegister(Controller);
	uTemp &= ~((1<<10) | (1<<9));		// Illegal Access & RnB Transition Interrupt disable
	Outp32(&(NAND(Controller)->rNFCONT), uTemp);

	// rb1004 : must be modify...Interrupt Controller
	//INT_Disable1(BIT_NUM_NFC);
	INTC_Disable(NUM_NFC);	

	// restore the valid value
	if(uErrorType == NAND_ECCERR_1BIT)
	{
		aNand_Spare_Data_Temp[NAND_EccError.uEccErrByte1] ^= (1<<NAND_EccError.uEccErrBit1);		
	}
	else if(uErrorType == NAND_ECCERR_MULTI)
	{
		aNand_Spare_Data_Temp[NAND_EccError.uEccErrByte1] ^= (1<<NAND_EccError.uEccErrBit1);	
		aNand_Spare_Data_Temp[NAND_EccError.uEccErrByte2] ^= (1<<NAND_EccError.uEccErrBit2);	
	}
	
	return eError;
}


//////////
// Function Name : NAND_WritePageWithInvalidSpareData
// Function Description : This function write the 1 page invalid spare data to test ECC Error
// Input : 	Controller - Nand Controller Port Number 
//			uBlock - Write Block
//			uPage - Write Page
//			pBuffer - Write data buffer
//			uErrorType - Error Type to test
// Output : 	Nand Error Type
NAND_eERROR NAND_WritePageWithInvalidSpareData(u32 Controller, u32 uBlock, u32 uPage, u8 *pBuffer, u32 uErrorType)
{
	NAND_eERROR eError;

	if( (NAND_Inform[Controller].uNandType == NAND_Normal8bit) || (NAND_Inform[Controller].uNandType == NAND_Advanced8bit) )
		eError = NAND_WritePageSLCWithInvalidSpareData(Controller, uBlock, uPage, pBuffer, uErrorType);
	else
		eError = eNAND_EtcError;
	return eError;
}



//////////
// Function Name : NAND_ReadPageSLCforNormal
// Function Description : This function read the data from SLC Normal Nand Device for Performance Measurement
// Input : 	Controller - Nand Controller Port Number 
//			uBlock - Read Block
//			uPage - Read Page
//			pBuffer - Read data buffer
// Output : 	ECC Error Type 
NAND_eERROR NAND_ReadPageSLCforNormal(u32 Controller, u32 uBlock, u32 uPage, u8 *pBuffer, u8 *pSpareBuffer)
{
	u32 uBlockPage, uPageSize;
	//u32 uTemp;
	u32 uMecc;
	NAND_eERROR eError;	
	u32 *pBufferW;
			
	pBufferW = (u32 *)pBuffer;	
	uBlockPage=(uBlock<<5) + uPage;
	uPageSize = NAND_Inform[Controller].uPageSize;
		
	NF_RSTECC(Controller);    // Initialize ECC
	NF_MECC_UnLock(Controller);
    
	NF_nFCE_L(Controller);    
	NF_CLEAR_RnB(Controller);

	NF_CMD(Controller, 0x00);	// Read command
	NF_ADDR(Controller, 0); 	
	NF_ADDR(Controller, uBlockPage&0xff);		//
	NF_ADDR(Controller, (uBlockPage>>8)&0xff);	// Block & Page num.
	NF_ADDR(Controller, (uBlockPage>>16)&0xff);	//

	NF_DETECT_RnB(Controller);

    	#if NAND_TRANSFER_MODE == POLLING_TRANSFER
	#if 0
	{
		u32 i;
	
		for(i=0 ; i<uPageSize ; i++) 
		{
			*pBuffer++ = NF_RDDATA8(Controller);	// Read one page
		}
	}
	#else
	{
		u32 i;
	
		for(i=0 ; i<uPageSize/4 ; i++) 
		{
			*pBufferW++ = NF_RDDATA(Controller);	// Read one page
		}
	}
	#endif
	#elif NAND_TRANSFER_MODE == DMA_TRANSFER
		Nand_DmaDone = 0;
		DMACH_Setup(DMA_A, 0x0, (u32)(&(NAND(Controller)->rNFDATA)), 1, (u32)pBufferW, 0, WORD, uPageSize/4, DEMAND, DMA1_NAND_TX, MEM, BURST128, &g_oNandDmac1);
			
		DMACH_Start(&g_oNandDmac1);
		while(!Nand_DmaDone);	
	#endif

	NF_MECC_Lock(Controller);

//	NF_SECC_UnLock(Controller);
		
	// Main Area ECC Check(1st ~ 4th byte : Main Area ECC Information)
	uMecc = NF_RDDATA(Controller);
		
	Outp32(&NAND(Controller)->rNFMECCD0, ((uMecc&0xff00)<<8)|(uMecc&0xff) );
	Outp32(&NAND(Controller)->rNFMECCD1, ((uMecc&0xff000000)>>8)|((uMecc&0xff0000)>>16) );

//	NF_SECC_Lock(Controller);
		
	NF_nFCE_H(Controller); 

	eError = NAND_CheckECCError(Controller);

	return eError;
}



//////////
// Function Name : NAND_ReadPageSLCforAdvance
// Function Description : This function read the data from SLC Advanced Nand Device for Performance Measurement
// Input : 	Controller - Nand Controller Port Number 
//			uBlock - Read Block
//			uPage - Read Page
//			pBuffer - Read data buffer
// Output : 	ECC Error Type 
NAND_eERROR NAND_ReadPageSLCforAdvance(u32 Controller, u32 uBlock, u32 uPage, u8 *pBuffer, u8 *pSpareBuffer)
{
	u32 uBlockPage, uPageSize;
	//u32 uTemp;
	u32 uMecc;
	NAND_eERROR eError;	
	u32 *pBufferW;
			
	pBufferW = (u32 *)pBuffer;	
	eError = eNAND_NoError;	
	uBlockPage=(uBlock<<6) + uPage;
	uPageSize = NAND_Inform[Controller].uPageSize;
		
	NF_RSTECC(Controller);    // Initialize ECC
	NF_MECC_UnLock(Controller);
    
	NF_nFCE_L(Controller);    
	NF_CLEAR_RnB(Controller);

	NF_CMD(Controller, 0x00);	// 1st Read command
	NF_ADDR(Controller, 0); 	
	NF_ADDR(Controller, 0); 
	
	NF_ADDR(Controller, uBlockPage&0xff);		//
	NF_ADDR(Controller, (uBlockPage>>8)&0xff);	// Block & Page num.
	NF_ADDR(Controller, (uBlockPage>>16)&0xff);	//

	NF_CMD(Controller, 0x30);	// 2'nd command
	
	NF_DETECT_RnB(Controller);

    	#if NAND_TRANSFER_MODE == POLLING_TRANSFER
	#if 0
	{
		u32 i;

		for(i=0 ; i<uPageSize ; i++) 
		{
			*pBuffer++ = NF_RDDATA8(Controller);	// Read one page
		}
	}
	#else
	{
		u32 i;
			
		for(i=0 ; i<uPageSize/4 ; i++) 
		{
			*pBufferW++ = NF_RDDATA(Controller);	// Read one page
		}
	}
	#endif
	#elif NAND_TRANSFER_MODE == DMA_TRANSFER
		Nand_DmaDone = 0;
		DMACH_Setup(DMA_A, 0x0, (u32)(&(NAND(Controller)->rNFDATA)), 1, (u32)pBufferW, 0, WORD, uPageSize/4, DEMAND, DMA1_NAND_TX, MEM, BURST128, &g_oNandDmac1);
			
		DMACH_Start(&g_oNandDmac1);
		while(!Nand_DmaDone);	
	#endif

	NF_MECC_Lock(Controller);

	aNand_Spare_Data[0] = NF_RDDATA8(Controller);		// Invalid Block Check data

//	NF_SECC_UnLock(Controller);
		
	// Main Area ECC Check
	uMecc = NF_RDDATA(Controller);

	if(NAND_Inform[Controller].uSpareECCtest == 0)
	{
		Outp32(&NAND(Controller)->rNFMECCD0, ((uMecc&0xff00)<<8)|(uMecc&0xff) );
		Outp32(&NAND(Controller)->rNFMECCD1, ((uMecc&0xff000000)>>8)|((uMecc&0xff0000)>>16) );
	}
		
//	NF_SECC_Lock(Controller);
	
	NF_nFCE_H(Controller); 

	eError = NAND_CheckECCError(Controller);

	return eError;
}



//////////
// Function Name : NAND_ReadPageMLCforPerform
// Function Description : This function read the data from MLC Nand Device for Performance Measurement
// Input : 	Controller - Nand Controller Port Number 
//			uBlock - Read Block
//			uPage - Read Page
//			pBuffer - Read data buffer
// Output : 	Nand Error Type
NAND_eERROR NAND_ReadPageMLCforPerform(u32 Controller, u32 uBlock, u32 uPage, u8 *pBuffer, u8 *pSpareBuffer)
{
	u32 i, uBlockPage, uPageSize, uTemp;
	NAND_eERROR eError;
	u32 *pBufferW;
			
	pBufferW = (u32 *)pBuffer;
	g_Nand_4bitEccDecDone = 0;
	
	uTemp = NAND_ReadNFCONRegister(Controller);
	//uTemp |= (1<<12);		// 4bit ECC decoding Interrupt enable
	uTemp |= (1<<12)|(1<<13);		// 4bit ECC decoding/encoding Interrupt enable...because error
	uTemp &= ~(1<<18);		// Decoding 4bit ECC selection
	Outp32(&(NAND(Controller)->rNFCONT), uTemp);

	Outp32(&(NAND(Controller)->rNFSTAT), (1<<6));		// 4bit ECC Decoding finish bit clear
	INTC_Enable(NUM_NFC);

	uBlockPage=(uBlock<<7) + uPage;
	uPageSize = NAND_Inform[Controller].uPageSize;

	//////////////////////////
	// Read 0 ~ 511
	//////////////////////////
	NF_RSTECC(Controller);    // Initialize ECC
	NF_MECC_UnLock(Controller);
    
	NF_nFCE_L(Controller);   
	
#if (NAND_REG_BUG == TRUE)
	uTemp = NAND_ReadNFCONRegister(Controller);
	uTemp |= (1<<12)|(1<<13);		// 4bit ECC decoding/encoding Interrupt enable...because error
	Outp32(&(NAND(Controller)->rNFCONT), uTemp);	
#endif

	NF_CLEAR_RnB(Controller);

	NF_CMD(Controller, 0x00);	// 1st Read command

	NF_ADDR(Controller, 0); 	
	NF_ADDR(Controller, 0); 	
	NF_ADDR(Controller, uBlockPage&0xff);		//
	NF_ADDR(Controller, (uBlockPage>>8)&0xff);	// Block & Page num.
	NF_ADDR(Controller, (uBlockPage>>16)&0xff);	//

	NF_CMD(Controller, 0x30);	// 2'nd command
	NF_DETECT_RnB(Controller);

    	#if NAND_TRANSFER_MODE == POLLING_TRANSFER
	#if 0	
		for(i=0 ; i<NAND_MLC_TRANS_SIZE ; i++) 	// MLC ECC Size : 512bytes
		{
			*pBuffer++ = NF_RDDATA8(Controller);	// Read one page
		}
	#else
		for(i=0 ; i<NAND_MLC_TRANS_SIZE/4 ; i++) 
		{
			*pBufferW++ = NF_RDDATA(Controller);	// Read one page
		}
	#endif		
	#elif NAND_TRANSFER_MODE == DMA_TRANSFER
		Nand_DmaDone = 0;
		DMACH_Setup(DMA_A, 0x0, (u32)(&(NAND(Controller)->rNFDATA)), 1, (u32)pBufferW, 0, WORD, NAND_MLC_TRANS_SIZE/4, DEMAND, DMA1_NAND_TX, MEM, BURST128, &g_oNandDmac1);
		
		DMACH_Start(&g_oNandDmac1);
		while(!Nand_DmaDone);	
		pBufferW += NAND_MLC_TRANS_SIZE/4;
	#endif

	//NF_MECC_Lock(Controller);

	NF_CMD(Controller, 0x05);
	NF_ADDR(Controller, (uPageSize+1)&0xFF);
	NF_ADDR(Controller, ((uPageSize+1)>>8)&0xFF);
	NF_CMD(Controller, 0xE0);

	pSpareBuffer++;
	for(i=1 ; i<8 ; i++) 
	{
		aNand_Spare_Data[i] = NF_RDDATA8(Controller);	
		*pSpareBuffer++ = aNand_Spare_Data[i];		
	}

	while(g_Nand_4bitEccDecDone == 0);

	eError = NAND_CheckECCError(Controller);
	if(eError != eNAND_NoError)
	{
		NF_MECC_Lock(Controller);	
		NF_nFCE_H(Controller);
		
		uTemp = NAND_ReadNFCONRegister(Controller);
		uTemp &= ~((1<<12)|(1<<13));		// 4bit ECC decoding Interrupt disable
		Outp32(&(NAND(Controller)->rNFCONT), uTemp);	

		NAND_Inform[Controller].uMLCECCPageReadSector = 0;		//0 ~ 511 Bytes	
		return eError;
	}

	//////////////////////////
	// Read 512 ~ 1023
	//////////////////////////
	g_Nand_4bitEccDecDone = 0;
	NF_RSTECC(Controller);    // Initialize ECC

#if (NAND_REG_BUG == TRUE)
	uTemp = NAND_ReadNFCONRegister(Controller);
	uTemp |= (1<<12)|(1<<13);		// 4bit ECC decoding/encoding Interrupt enable...because error
	Outp32(&(NAND(Controller)->rNFCONT), uTemp);	
#endif

	NF_CMD(Controller, 0x05);
	NF_ADDR(Controller, (512)&0xFF);
	NF_ADDR(Controller, (512>>8)&0xFF);
	NF_CMD(Controller, 0xE0);

    	#if NAND_TRANSFER_MODE == POLLING_TRANSFER
	#if 0	
		for(i=0 ; i<NAND_MLC_TRANS_SIZE ; i++) 	// MLC ECC Size : 512bytes
		{
			*pBuffer++ = NF_RDDATA8(Controller);	// Read one page
		}
	#else
		for(i=0 ; i<NAND_MLC_TRANS_SIZE/4 ; i++) 
		{
			*pBufferW++ = NF_RDDATA(Controller);	// Read one page
		}
	#endif
	#elif NAND_TRANSFER_MODE == DMA_TRANSFER
		Nand_DmaDone = 0;
		DMACH_Setup(DMA_A, 0x0, (u32)(&(NAND(Controller)->rNFDATA)), 1, (u32)pBufferW, 0, WORD, NAND_MLC_TRANS_SIZE/4, DEMAND, DMA1_NAND_TX, MEM, BURST128, &g_oNandDmac1);
		
		DMACH_Start(&g_oNandDmac1);
		while(!Nand_DmaDone);	
		pBufferW += NAND_MLC_TRANS_SIZE/4;
	#endif

	NF_CMD(Controller, 0x05);
	NF_ADDR(Controller, (uPageSize+8)&0xFF);
	NF_ADDR(Controller, ((uPageSize+8)>>8)&0xFF);
	NF_CMD(Controller, 0xE0);

	for(i=8;i<15;i++) 
	{
		aNand_Spare_Data[i] = NF_RDDATA8(Controller);	
		*pSpareBuffer++ = aNand_Spare_Data[i];		
	}	

	while(g_Nand_4bitEccDecDone == 0);

	eError = NAND_CheckECCError(Controller);
	if(eError != eNAND_NoError)
	{
		NF_MECC_Lock(Controller);	
		NF_nFCE_H(Controller);
		uTemp = NAND_ReadNFCONRegister(Controller);
		uTemp &= ~((1<<12)|(1<<13));		// 4bit ECC decoding Interrupt enable
		Outp32(&(NAND(Controller)->rNFCONT), uTemp);			
		NAND_Inform[Controller].uMLCECCPageReadSector = 1;		//512 ~ 1023 Bytes
		return eError;
	}
	
	//////////////////////////
	// Read 1024 ~ 1535
	//////////////////////////
	g_Nand_4bitEccDecDone = 0;
	NF_RSTECC(Controller);    // Initialize ECC

#if (NAND_REG_BUG == TRUE)
	uTemp = NAND_ReadNFCONRegister(Controller);
	uTemp |= (1<<12)|(1<<13);		// 4bit ECC decoding/encoding Interrupt enable...because error
	Outp32(&(NAND(Controller)->rNFCONT), uTemp);	
#endif

	NF_CMD(Controller, 0x05);
	NF_ADDR(Controller, (512*2)&0xFF);
	NF_ADDR(Controller, ((512*2)>>8)&0xFF);
	NF_CMD(Controller, 0xE0);

    	#if NAND_TRANSFER_MODE == POLLING_TRANSFER
	#if 0	
		for(i=0 ; i<NAND_MLC_TRANS_SIZE ; i++) 	// MLC ECC Size : 512bytes
		{
			*pBuffer++ = NF_RDDATA8(Controller);	// Read one page
		}
	#else
		for(i=0 ; i<NAND_MLC_TRANS_SIZE/4 ; i++) 
		{
			*pBufferW++ = NF_RDDATA(Controller);	// Read one page
		}
	#endif
	#elif NAND_TRANSFER_MODE == DMA_TRANSFER
		Nand_DmaDone = 0;
		DMACH_Setup(DMA_A, 0x0, (u32)(&(NAND(Controller)->rNFDATA)), 1, (u32)pBufferW, 0, WORD, NAND_MLC_TRANS_SIZE/4, DEMAND, DMA1_NAND_TX, MEM, BURST128, &g_oNandDmac1);
		
		DMACH_Start(&g_oNandDmac1);
		while(!Nand_DmaDone);	
		pBufferW += NAND_MLC_TRANS_SIZE/4;
	#endif

	NF_CMD(Controller, 0x05);
	NF_ADDR(Controller, (uPageSize+15)&0xFF);
	NF_ADDR(Controller, ((uPageSize+15)>>8)&0xFF);
	NF_CMD(Controller, 0xE0);

	for(i=15;i<22;i++) 
	{
		aNand_Spare_Data[i] = NF_RDDATA8(Controller);	
		*pSpareBuffer++ = aNand_Spare_Data[i];		
	}	

	while(g_Nand_4bitEccDecDone == 0);

	eError = NAND_CheckECCError(Controller);
	if(eError != eNAND_NoError)
	{
		NF_MECC_Lock(Controller);		
		NF_nFCE_H(Controller);
		uTemp = NAND_ReadNFCONRegister(Controller);
		uTemp &= ~(1<<12);		// 4bit ECC decoding Interrupt enable
		Outp32(&(NAND(Controller)->rNFCONT), uTemp);	
		NAND_Inform[Controller].uMLCECCPageReadSector = 2;		//1024 ~ 1535 Bytes
		return eError;
	}
	
	//////////////////////////
	// Read 1536 ~ 2047
	//////////////////////////
	g_Nand_4bitEccDecDone = 0;
	NF_RSTECC(Controller);    // Initialize ECC

#if (NAND_REG_BUG == TRUE)
	uTemp = NAND_ReadNFCONRegister(Controller);
	uTemp |= (1<<12)|(1<<13);		// 4bit ECC decoding/encoding Interrupt enable...because error
	Outp32(&(NAND(Controller)->rNFCONT), uTemp);	
#endif

	NF_CMD(Controller, 0x05);
	NF_ADDR(Controller, (512*3)&0xFF);
	NF_ADDR(Controller, ((512*3)>>8)&0xFF);
	NF_CMD(Controller, 0xE0);

    	#if NAND_TRANSFER_MODE == POLLING_TRANSFER
	#if 0	
		for(i=0 ; i<NAND_MLC_TRANS_SIZE ; i++) 	// MLC ECC Size : 512bytes
		{
			*pBuffer++ = NF_RDDATA8(Controller);	// Read one page
		}
	#else
		for(i=0 ; i<NAND_MLC_TRANS_SIZE/4 ; i++) 
		{
			*pBufferW++ = NF_RDDATA(Controller);	// Read one page
		}
	#endif
	#elif NAND_TRANSFER_MODE == DMA_TRANSFER
		Nand_DmaDone = 0;
		DMACH_Setup(DMA_A, 0x0, (u32)(&(NAND(Controller)->rNFDATA)), 1, (u32)pBufferW, 0, WORD, NAND_MLC_TRANS_SIZE/4, DEMAND, DMA1_NAND_TX, MEM, BURST128, &g_oNandDmac1);
		
		DMACH_Start(&g_oNandDmac1);
		while(!Nand_DmaDone);	
		pBufferW += NAND_MLC_TRANS_SIZE/4;
	#endif

	NF_CMD(Controller, 0x05);
	NF_ADDR(Controller, (uPageSize+22)&0xFF);
	NF_ADDR(Controller, ((uPageSize+22)>>8)&0xFF);
	NF_CMD(Controller, 0xE0);

	while(g_Nand_4bitEccDecDone == 0);

	eError = NAND_CheckECCError(Controller);
	if(eError != eNAND_NoError)
	{
		NF_MECC_Lock(Controller);		
		NF_nFCE_H(Controller);
		uTemp = NAND_ReadNFCONRegister(Controller);
		uTemp &= ~(1<<12);		// 4bit ECC decoding Interrupt enable
		Outp32(&(NAND(Controller)->rNFCONT), uTemp);		
		NAND_Inform[Controller].uMLCECCPageReadSector = 3;		// 1536 ~ 2047 Bytes
		return eError;
	}

	NF_nFCE_H(Controller);

	NF_MECC_Lock(Controller);	

	uTemp = NAND_ReadNFCONRegister(Controller);
	uTemp &= ~((1<<12)|(1<<13));		// 4bit ECC decoding/encoding Interrupt disable
	Outp32(&(NAND(Controller)->rNFCONT), uTemp);
	
	INTC_Disable(NUM_NFC);
	
	return eNAND_NoError;
}




//////////
// Function Name : NAND_ReadPageforPerform
// Function Description : This function read the data from Nand Device for Performance Measurement
// Input : 	Controller - Nand Controller Port Number 
//			uBlock - Read Block
//			uPage - Read Page
//			pBuffer - Read data buffer
// Output : 	Nand Error Type
NAND_eERROR NAND_ReadPageforPerform(u32 Controller, u32 uBlock, u32 uPage, u8 *pBuffer, u8 *pSpareBuffer)
{
	NAND_eERROR eError;

	if(NAND_Inform[Controller].uNandType == NAND_Normal8bit)
		eError = NAND_ReadPageSLCforNormal(Controller, uBlock, uPage, pBuffer, pSpareBuffer);
	else if(NAND_Inform[Controller].uNandType == NAND_Advanced8bit) 
		eError = NAND_ReadPageSLCforAdvance(Controller, uBlock, uPage, pBuffer, pSpareBuffer);
	else //if(NAND_Inform[Controller].uNandType == NAND_MLC8bit)
		eError = NAND_ReadPageMLC(Controller, uBlock, uPage, pBuffer, pSpareBuffer);

	return eError;
}


//////////
// Function Name : NAND_WritePageSLCforNormal
// Function Description : This function write the data to SLC Normal Nand Device for Performance Measurement
// Input : 	Controller - Nand Controller Port Number 
//			uBlock - Write Block
//			uPage - Write Page
//			pBuffer - Write data buffer
//			aSpareBuffer - Memory to store the spare data
// Output : 	ECC Error Type 
NAND_eERROR NAND_WritePageSLCforNormal(u32 Controller, u32 uBlock, u32 uPage, u8 *pBuffer, u8 *aSpareBuffer)
{
	u32 i, uBlockPage, uPageSize, uSpareSize, uTemp;
	u32 uMecc, uSecc, uStatus;
	NAND_eERROR eError;	
	u32 *pBufferW;
			
	pBufferW = (u32 *)pBuffer;
	g_Nand_RnBTransition = 0;

	uTemp = NAND_ReadNFCONRegister(Controller);
	uTemp &= ~((1<<13)|(1<<12));		// 4bit ECC encoding/decoding completion Interrupt disable
	uTemp |= (1<<10) | (1<<9);		// Illegal Access & RnB Transition Interrupt enable
	Outp32(&(NAND(Controller)->rNFCONT), uTemp);
	
	Outp32(&(NAND(Controller)->rNFSTAT), (1<<5)|(1<<4));		// Illegal Access & RnB Interrupt Pending bit clear
	INTC_Enable(NUM_NFC);
	
	uBlockPage=(uBlock<<5) + uPage;
	uPageSize = NAND_Inform[Controller].uPageSize;
	uSpareSize = NAND_Inform[Controller].uSpareSize;

	NF_RSTECC(Controller);    // Initialize ECC
	NF_MECC_UnLock(Controller);
    
	NF_nFCE_L(Controller);    
	NF_CLEAR_RnB(Controller);

	NF_CMD(Controller, 0x80);	// Program command
	NF_ADDR(Controller, 0); 
	NF_ADDR(Controller, uBlockPage&0xff);		//
	NF_ADDR(Controller, (uBlockPage>>8)&0xff);	// Block & Page num.
	NF_ADDR(Controller, (uBlockPage>>16)&0xff);	//

    	#if NAND_TRANSFER_MODE == POLLING_TRANSFER
		for(i=0 ; i<uPageSize/4 ; i++) 
		{
			NF_WRDATA(Controller, *pBufferW++);	// Read one page
		}
	#elif NAND_TRANSFER_MODE == DMA_TRANSFER
		Nand_DmaDone = 0;
		DMACH_Setup(DMA_A, 0x0, (u32)pBufferW, 0, (u32)(&(NAND(Controller)->rNFDATA)), 1, WORD, uPageSize/4, DEMAND, MEM, DMA1_NAND_RX, BURST128, &g_oNandDmac1);
			
		DMACH_Start(&g_oNandDmac1);
		while(!Nand_DmaDone);	
	#endif

	NF_MECC_Lock(Controller);
	g_Nand_IllegalAccError  = 0;

	// Main Area ECC
	uMecc = Inp32(&NAND(Controller)->rNFMECC0);

	aNand_Spare_Data[0] = (u8)(uMecc&0xFF);
	aNand_Spare_Data[1] = (u8)((uMecc&0xFF00)>>8);
	aNand_Spare_Data[2] = (u8)((uMecc&0xFF0000)>>16);
	aNand_Spare_Data[3] = (u8)((uMecc&0xFF000000)>>24);
	aNand_Spare_Data[5] = (u8)0xFF;			//Marking Valid Block

	NF_SECC_UnLock(Controller);

	for(i=0;i<4;i++) 
	{
		NF_WRDATA8(Controller, aNand_Spare_Data[i]);		// Write spare array(Main ECC)
		//NF8_Spare_Data[i]=aNand_Spare_Data[i];
    	} 
	
	NF_SECC_Lock(Controller);

	// Spare Area ECC
	uSecc = Inp32(&NAND(Controller)->rNFSECC) & 0xFFFF;

	aNand_Spare_Data[8] = (u8)(uSecc&0xFF);
	aNand_Spare_Data[9] = (u8)((uSecc&0xFF00)>>8);

	for(i=4 ; i<uSpareSize; i++) 
	{
		NF_WRDATA8(Controller, aNand_Spare_Data[i]);  // Write spare array(Spare ECC and Mark)
		//NF8_Spare_Data[i]=aNand_Spare_Data[i];
	}  

	NF_CLEAR_RnB(Controller);
	NF_CMD(Controller, 0x10);	 // Write 2nd command	
	
	while(!g_Nand_RnBTransition)
	{
		if(g_Nand_IllegalAccError == 1)
		{
			//NAND_Reset(Controller);
			g_Nand_IllegalAccError = 0;
			NF_CMD(Controller, 0x70);

			NF_nFCE_H(Controller);
			
			uTemp = NAND_ReadNFCONRegister(Controller);
			uTemp &= ~((1<<10) | (1<<9));		// Illegal Access & RnB Transition Interrupt disable
			Outp32(&(NAND(Controller)->rNFCONT), uTemp);

			INTC_Disable(NUM_NFC);
			return eNAND_ProgramError;
		}
	}

	//Read Program Status
	NF_CMD(Controller, 0x70);

	while(!((uStatus = NF_RDDATA8(Controller)) & (1<<6)));
	if(uStatus & 0x01)
		eError = eNAND_ProgramError;		// Error in Program
	else
		eError = eNAND_NoError;

	NF_nFCE_H(Controller);
	
	uTemp = NAND_ReadNFCONRegister(Controller);
	uTemp &= ~((1<<10) | (1<<9));		// Illegal Access & RnB Transition Interrupt disable
	Outp32(&(NAND(Controller)->rNFCONT), uTemp);

	INTC_Disable(NUM_NFC);

	return eError;
}



//////////
// Function Name : NAND_WritePageSLCforAdvance
// Function Description : This function write the data to SLC Advanced Nand Device for Performance Measurement
// Input : 	Controller - Nand Controller Port Number 
//			uBlock - Write Block
//			uPage - Write Page
//			pBuffer - Write data buffer
//			aSpareBuffer - Memory to store the spare data
// Output : 	ECC Error Type 
NAND_eERROR NAND_WritePageSLCforAdvance(u32 Controller, u32 uBlock, u32 uPage, u8 *pBuffer, u8 *aSpareBuffer)
{
	u32 i, uBlockPage, uPageSize, uSpareSize, uTemp;
	u32 uMecc, uSecc, uStatus;
	NAND_eERROR eError;	
	u32 *pBufferW;
			
	pBufferW = (u32 *)pBuffer;
	g_Nand_RnBTransition = 0;

	uTemp = NAND_ReadNFCONRegister(Controller);
	uTemp &= ~((1<<13)|(1<<12));		// 4bit ECC encoding/decoding completion Interrupt disable
	uTemp |= (1<<10) | (1<<9);		// Illegal Access & RnB Transition Interrupt enable
	Outp32(&(NAND(Controller)->rNFCONT), uTemp);
	
	Outp32(&(NAND(Controller)->rNFSTAT), (1<<5)|(1<<4));		// Illegal Access & RnB Interrupt Pending bit clear
	INTC_Enable(NUM_NFC);
	
	uBlockPage=(uBlock<<6) + uPage;
	uPageSize = NAND_Inform[Controller].uPageSize;
	uSpareSize = NAND_Inform[Controller].uSpareSize;

	NF_RSTECC(Controller);    // Initialize ECC
	NF_MECC_UnLock(Controller);
    
	NF_nFCE_L(Controller);    
	NF_CLEAR_RnB(Controller);

	NF_CMD(Controller, 0x80);	// Program command
	NF_ADDR(Controller, 0); 
		
	//if(NAND_Inform[Controller].uNandType == NAND_Advanced8bit)
	if(NAND_Inform[Controller].uAddrCycle == 5)
		NF_ADDR(Controller, 0); 
	
	NF_ADDR(Controller, uBlockPage&0xff);		//
	NF_ADDR(Controller, (uBlockPage>>8)&0xff);	// Block & Page num.
	NF_ADDR(Controller, (uBlockPage>>16)&0xff);	//

    	#if NAND_TRANSFER_MODE == POLLING_TRANSFER
		for(i=0 ; i<uPageSize/4 ; i++) 
		{
			NF_WRDATA(Controller, *pBufferW++);	// Read one page
		}
	#elif NAND_TRANSFER_MODE == DMA_TRANSFER
		Nand_DmaDone = 0;
		DMACH_Setup(DMA_A, 0x0, (u32)pBufferW, 0, (u32)(&(NAND(Controller)->rNFDATA)), 1, WORD, uPageSize/4, DEMAND, MEM, DMA1_NAND_RX, BURST128, &g_oNandDmac1);
			
		DMACH_Start(&g_oNandDmac1);
		while(!Nand_DmaDone);	
	#endif

	NF_MECC_Lock(Controller);
	g_Nand_IllegalAccError  = 0;

	// Main Area ECC
	uMecc = Inp32(&NAND(Controller)->rNFMECC0);

	aNand_Spare_Data[0] = (u8)0xFF;				//Marking Valid Block
	aNand_Spare_Data[1] = (u8)(uMecc&0xFF);
	aNand_Spare_Data[2] = (u8)((uMecc&0xFF00)>>8);
	aNand_Spare_Data[3] = (u8)((uMecc&0xFF0000)>>16);
	aNand_Spare_Data[4] = (u8)((uMecc&0xFF000000)>>24);

	NF_WRDATA8(Controller, aNand_Spare_Data[0]);	//Write Marking Valid Block
		
	NF_SECC_UnLock(Controller);
	for(i=1;i<5;i++) 
	{
		NF_WRDATA8(Controller, aNand_Spare_Data[i]);		// Write spare array(Main ECC)
		//NF8_Spare_Data[i]=aNand_Spare_Data[i];
   	} 
	NF_SECC_Lock(Controller);

	// Spare Area ECC
	uSecc = Inp32(&NAND(Controller)->rNFSECC) & 0xFFFF;

	aNand_Spare_Data[8] = (u8)(uSecc&0xFF);
	aNand_Spare_Data[9] = (u8)((uSecc&0xFF00)>>8);

	for(i=5 ; i<uSpareSize; i++) 
	{
		NF_WRDATA8(Controller, aNand_Spare_Data[i]);  // Write spare array(Spare ECC and Mark)
		//NF8_Spare_Data[i]=aNand_Spare_Data[i];
	}  

	NF_CLEAR_RnB(Controller);
	NF_CMD(Controller, 0x10);	 // Write 2nd command	

	while(!g_Nand_RnBTransition)
	{
		if(g_Nand_IllegalAccError == 1)
		{
			//NAND_Reset(Controller);
			g_Nand_IllegalAccError = 0;
			NF_CMD(Controller, 0x70);

			NF_nFCE_H(Controller);
			
			uTemp = NAND_ReadNFCONRegister(Controller);
			uTemp &= ~((1<<10) | (1<<9));		// Illegal Access & RnB Transition Interrupt disable
			Outp32(&(NAND(Controller)->rNFCONT), uTemp);

			INTC_Disable(NUM_NFC);
			return eNAND_ProgramError;
		}
	}

	//Read Program Status
	NF_CMD(Controller, 0x70);

	while(!((uStatus = NF_RDDATA8(Controller)) & (1<<6)));
	if(uStatus & 0x01)
		eError = eNAND_ProgramError;		// Error in Program
	else
		eError = eNAND_NoError;

	NF_nFCE_H(Controller);
	
	uTemp = NAND_ReadNFCONRegister(Controller);
	uTemp &= ~((1<<10) | (1<<9));		// Illegal Access & RnB Transition Interrupt disable
	Outp32(&(NAND(Controller)->rNFCONT), uTemp);

	INTC_Disable(NUM_NFC);

	return eError;
}


//////////
// Function Name : NAND_WritePageforPerform
// Function Description : This function write the data to Nand Device for Performance Measurement
// Input : 	Controller - Nand Controller Port Number 
//			uBlock - Write Block
//			uPage - Write Page
//			pBuffer - Write data buffer
// Output : 	Nand Error Type
NAND_eERROR NAND_WritePageforPerform(u32 Controller, u32 uBlock, u32 uPage, u8 *pBuffer, u8 *pSpareBuffer)
{
	NAND_eERROR eError;

	if(NAND_Inform[Controller].uNandType == NAND_Normal8bit)
		eError = NAND_WritePageSLCforNormal(Controller, uBlock, uPage, pBuffer, pSpareBuffer);
	else if(NAND_Inform[Controller].uNandType == NAND_Advanced8bit)
		eError = NAND_WritePageSLCforAdvance(Controller, uBlock, uPage, pBuffer, pSpareBuffer);
	else //if(NAND_Inform[Controller].uNandType == NAND_MLC8bit)
		eError = NAND_WritePageMLC(Controller, uBlock, uPage, pBuffer, pSpareBuffer);

	return eError;
}



//////////
// Function Name : NAND_ReadNFCONRegister
// Function Description : This function return the NFCON Register Value.....because of the Design Bug in 6400 EVT0
// Input : 	Controller - Nand Controller Port Number 
// Output : 	NFCON Register Value
u32 NAND_ReadNFCONRegister(u32 Controller)
{
	u32 uTemp, uErrorBit;
	
	uTemp = Inp32(&(NAND(Controller)->rNFCONT));

	uErrorBit = (uTemp & (3<<13)) >> 1;
	uTemp &= ~(3<<13);

	return (uTemp | uErrorBit);
}

///////////////////////////////////////////////////////////////////////////////////
//////////                     Nand Extra Test Functions for EBI Test                  ///////////////////
///////////////////////////////////////////////////////////////////////////////////

//////////
// Function Name : NAND_ReadPageSLCSetup
// Function Description : This function read the 1 page data of external SLC Type NAND device
// Input : 	Controller - Nand Controller Port Number 
//			uBlock - Read Block
//			uPage - Read Page
//			pBuffer - Read data buffer
// Output : 	ECC Error Type 
void NAND_ReadPageSLCSetup(u32 Controller, u32 uBlock, u32 uPage)
{
	u32 uBlockPage;
	
	uBlockPage=(uBlock<<5) + uPage;


	NF_RSTECC(Controller);    // Initialize ECC
	NF_MECC_UnLock(Controller);
    
	NF_nFCE_L(Controller);    
	NF_CLEAR_RnB(Controller);

	NF_CMD(Controller, 0x00);	// Read command
	NF_ADDR(Controller, 0); 	
	
	NF_ADDR(Controller, uBlockPage&0xff);		//
	NF_ADDR(Controller, (uBlockPage>>8)&0xff);	// Block & Page num.
	NF_ADDR(Controller, (uBlockPage>>16)&0xff);	//

	NF_DETECT_RnB(Controller);

	return ;
}

//////////
// Function Name : NAND_ReadPageSLCClose
// Function Description : This function read the 1 page data of external SLC Type NAND device
// Input : 	Controller - Nand Controller Port Number 
//			uBlock - Read Block
//			uPage - Read Page
//			pBuffer - Read data buffer
// Output : 	ECC Error Type 
NAND_eERROR NAND_ReadPageSLCClose(u32 Controller, u8 *pSpareBuffer)
{
	u32 i;
	u32 uMecc, uSecc;
	NAND_eERROR eError;	

	NF_MECC_Lock(Controller);

	NF_SECC_UnLock(Controller);
	
	// Main Area ECC Check(1st ~ 4th byte : Main Area ECC Information)
	uMecc = NF_RDDATA(Controller);
	
	if(NAND_Inform[Controller].uSpareECCtest == 0)
	{
		Outp32(&NAND(Controller)->rNFMECCD0, ((uMecc&0xff00)<<8)|(uMecc&0xff) );
		Outp32(&NAND(Controller)->rNFMECCD1, ((uMecc&0xff000000)>>8)|((uMecc&0xff0000)>>16) );
	}
	else if(NAND_Inform[Controller].uSpareECCtest == 1)
	{
		// The correct MECC value is written for SpareECC Test
		u32 uTempMecc;

		uTempMecc = ((u32)aNand_Spare_Data_Temp[3]<<24)|
					((u32)aNand_Spare_Data_Temp[2]<<16)|
					((u32)aNand_Spare_Data_Temp[1]<<8)|
					((u32)aNand_Spare_Data_Temp[0]);		
		Outp32(&NAND(Controller)->rNFMECCD0, ((uTempMecc&0xff00)<<8)|(uTempMecc&0xff) );
		Outp32(&NAND(Controller)->rNFMECCD1, ((uTempMecc&0xff000000)>>8)|((uTempMecc&0xff0000)>>16) );
	}
	
	NF_SECC_Lock(Controller);

	aNand_Spare_Data[0] = (u8)(uMecc&0xFF);
	aNand_Spare_Data[1] = (u8)((uMecc&0xFF00)>>8);
	aNand_Spare_Data[2] = (u8)((uMecc&0xFF0000)>>16);
	aNand_Spare_Data[3] = (u8)((uMecc&0xFF000000)>>24);

	aNand_Spare_Data[4] = NF_RDDATA8(Controller);		 // read 5th byte(dummy)

	for(i=5; i<NAND_Inform[Controller].uSpareSize ; i++)
	{
		aNand_Spare_Data[i] = NF_RDDATA8(Controller);
	}
	
	for(i=0; i<NAND_Inform[Controller].uSpareSize ; i++)
		*pSpareBuffer++ = aNand_Spare_Data[i];

	// Spare Area ECC Check
	uSecc = (aNand_Spare_Data[9]<<8) | (aNand_Spare_Data[8]);
	
	Outp32(&NAND(Controller)->rNFSECCD, ((uSecc&0xff00)<<8)|(uSecc&0xff) );
		
	NF_nFCE_H(Controller); 

	eError = NAND_CheckECCError(Controller);

	return eError;
}

//////////
// Function Name : NAND_WritePageSLCSetup
// Function Description : This function write the 1 page data of external SLC Type NAND device
// Input : 	Controller - Nand Controller Port Number 
//			uBlock - Read Block
//			uPage - Read Page
//			pBuffer - Read data buffer
// Output : 	ECC Error Type 
void NAND_WritePageSLCSetup(u32 Controller, u32 uBlock, u32 uPage)
{
	u32 uBlockPage, uTemp;

	g_Nand_RnBTransition = 0;

	uTemp = NAND_ReadNFCONRegister(Controller);
	uTemp &= ~((1<<13)|(1<<12));		// 4bit ECC encoding/decoding completion Interrupt disable
	uTemp |= (1<<10) | (1<<9);		// Illegal Access & RnB Transition Interrupt enable
	Outp32(&(NAND(Controller)->rNFCONT), uTemp);
	
	Outp32(&(NAND(Controller)->rNFSTAT), (1<<5)|(1<<4));		// Illegal Access & RnB Interrupt Pending bit clear
	INTC_Enable(NUM_NFC);
	
	uBlockPage=(uBlock<<5) + uPage;

	NF_RSTECC(Controller);    // Initialize ECC
	NF_MECC_UnLock(Controller);
    
	NF_nFCE_L(Controller);    
	NF_CLEAR_RnB(Controller);

	NF_CMD(Controller, 0x80);	// Program command
	NF_ADDR(Controller, 0); 
		
	//if(NAND_Inform[Controller].uNandType == NAND_Advanced8bit)
	if(NAND_Inform[Controller].uAddrCycle == 5)
		NF_ADDR(Controller, 0); 
	
	NF_ADDR(Controller, uBlockPage&0xff);		//
	NF_ADDR(Controller, (uBlockPage>>8)&0xff);	// Block & Page num.
	NF_ADDR(Controller, (uBlockPage>>16)&0xff);	//

	return;
}

//////////
// Function Name : NAND_WritePageSLCClose
// Function Description : This function write the 1 page data of external SLC Type NAND device
// Input : 	Controller - Nand Controller Port Number 
//			uBlock - Read Block
//			uPage - Read Page
//			pBuffer - Read data buffer
// Output : 	ECC Error Type 
NAND_eERROR NAND_WritePageSLCClose(u32 Controller, u8 *pSpareBuffer)
{
	u32 i, uSpareSize, uTemp;
	u32 uMecc, uSecc;
	NAND_eERROR eError;	
	
	NF_MECC_Lock(Controller);
	g_Nand_IllegalAccError  = 0;


	// Main Area ECC
	uMecc = Inp32(&NAND(Controller)->rNFMECC0);

	aNand_Spare_Data[0] = (u8)(uMecc&0xFF);
	aNand_Spare_Data[1] = (u8)((uMecc&0xFF00)>>8);
	aNand_Spare_Data[2] = (u8)((uMecc&0xFF0000)>>16);
	aNand_Spare_Data[3] = (u8)((uMecc&0xFF000000)>>24);
	aNand_Spare_Data[5] = (u8)0xFF;			//Marking Valid Block

	NF_SECC_UnLock(Controller);

	for(i=0;i<4;i++) 
	{
		NF_WRDATA8(Controller, aNand_Spare_Data[i]);		// Write spare array(Main ECC)
		//NF8_Spare_Data[i]=aNand_Spare_Data[i];
		} 

	NF_SECC_Lock(Controller);

	// Spare Area ECC
	uSecc = Inp32(&NAND(Controller)->rNFSECC) & 0xFFFF;

	aNand_Spare_Data[8] = (u8)(uSecc&0xFF);
	aNand_Spare_Data[9] = (u8)((uSecc&0xFF00)>>8);

	for(i=4 ; i<uSpareSize; i++) 
	{
		NF_WRDATA8(Controller, aNand_Spare_Data[i]);  // Write spare array(Spare ECC and Mark)
		//NF8_Spare_Data[i]=aNand_Spare_Data[i];
	}  

	NF_CLEAR_RnB(Controller);
	NF_CMD(Controller, 0x10);	 // Write 2nd command	
#if 0
	while(!g_Nand_RnBTransition)
	{
		if(g_Nand_IllegalAccError == 1)
		{
			NAND_Reset(Controller);
			g_Nand_IllegalAccError = 0;
			return eNAND_ProgramError;
		}
	}
#endif
	//Read Program Status
	NF_CMD(Controller, 0x70);
#if 0
	while(!((uStatus = NF_RDDATA8(Controller)) & (1<<6)));
	if(uStatus & 0x01)
		eError = eNAND_ProgramError;		// Error in Program
	else
		eError = eNAND_NoError;
#endif	
//	if(NF_RDDATA8(Controller) & 0x01)
//		eError = eNAND_ProgramError;		// Error in Program
//	else
//		eError = eNAND_NoError;

	NF_nFCE_H(Controller);
	
	uTemp = NAND_ReadNFCONRegister(Controller);
	uTemp &= ~((1<<10) | (1<<9));		// Illegal Access & RnB Transition Interrupt disable
	Outp32(&(NAND(Controller)->rNFCONT), uTemp);

	if(NAND_Inform[Controller].uECCtest == 1)
	{
		for(i=0 ; i<uSpareSize ; i++)
			aNand_Spare_Data_Temp[i] = aNand_Spare_Data[i];
	}

	for(i=0 ; i<uSpareSize ; i++)
	{
		*pSpareBuffer++ = aNand_Spare_Data[i];
	}
	
	INTC_Disable(NUM_NFC);

	return eError;
}




//===========================================================================
//                      jungil made code 8bit ecc related
//===========================================================================


void NAND_WaitForRnbTransition(u32 Controller)
{
	u32 uStat;
	u32 uTemp;

	do {
		uStat = Inp32(&(NAND(Controller)->rNFSTAT));
	} while (!(uStat&1<<4));

	uTemp = 1<<4;
	Outp32(&(NAND(Controller)->rNFSTAT), uTemp); // Clear
	g_Nand_RnBTransition = 1;
}

//////////
// Function Name : NAND_CheckECCError_8Bit
// Function Description : This function check the ECC Error
// Input : 	Controller - Nand Controller Port Number 
// Output :  	ECC Error Type 
NAND_eERROR NAND_CheckECCError_8Bit(u32 Controller)
{
	u32 uEccError0;
//	u32 uEccError1, uEccError2;
//	u32 uTemp;
	//u32 uEccError1;
	NAND_eERROR eError;	

	while( (NAND(Controller)->rNF8ECCERR0&((u32)(1<<31)) ));
	eError = eNAND_NoError;
	uEccError0 = Inp32(&NAND(Controller)->rNF8ECCERR0);
//	uEccError1 = Inp32(&NAND(Controller)->rNF8ECCERR1);
//	uEccError2 = Inp32(&NAND(Controller)->rNF8ECCERR2);

	// Check Free Page
	if ( (NAND(Controller)->rNF8ECCERR0&(1<<29))) UART_Printf(" Free Page\n");

	switch ( (uEccError0>>25) & 0xf)
	{
			case 0x0 : 	eError = eNAND_NoError;
						break;
			case 0x1 : 	eError = eNAND_1bitEccError;
						break;
			case 0x2 : 	eError = eNAND_2bitEccError;
						break;
			case 0x3 : 	eError = eNAND_3bitEccError;
						break;
			case 0x4 : 	eError = eNAND_4bitEccError;
						break;
			case 0x5 : 	eError = eNAND_5bitEccError;
						break;
			case 0x6 : 	eError = eNAND_6bitEccError;
						break;
			case 0x7 : 	eError = eNAND_7bitEccError;
						break;
			case 0x8 : 	eError = eNAND_8bitEccError;
						break;
			case 0x9 : 	eError = eNAND_UncorrectableError;
						break;
			default:	 	break;
			
						
	}

#if 1 // Uncorrectable Error TEst
	if ( NAND_Inform[Controller].uUncorrectableTest == 1)
	{
		if ( eError == eNAND_UncorrectableError ) 
		{
			return eError;
		}
		else
		{
			UART_Printf("Check This Condition : Not UnCorrectable Error \n");
			while(1);
		}
	}
#endif
	return eError;

	
}

//////////
// Function Name : NAND_CheckECCError_4Bit
// Function Description : This function check the ECC Error
// Input : 	Controller - Nand Controller Port Number 
// Output :  	ECC Error Type 
NAND_eERROR NAND_CheckECCError_4Bit(u32 Controller)
{
	u32 uEccError0; 
//	u32 uEccError1, uEccError2;
//	u32 uTemp;
	//u32 uEccError1;
	NAND_eERROR eError;	

	while( (NAND(Controller)->rNFECCERR0&((u32)((1<<31)) )));
	
	eError = eNAND_NoError;

	uEccError0 = Inp32(&NAND(Controller)->rNFECCERR0);
//	uEccError1 = Inp32(&NAND(Controller)->rNFECCERR1);


	// Check Free Page
	if ( (NAND(Controller)->rNF8ECCERR0&(1<<29))) UART_Printf(" Free Page\n");

	switch ( (uEccError0>>26) & 0x7)
	{
			case 0x0 : 	eError = eNAND_NoError;
						break;
			case 0x1 : 	eError = eNAND_1bitEccError;
						break;
			case 0x2 : 	eError = eNAND_2bitEccError;
						break;
			case 0x3 : 	eError = eNAND_3bitEccError;
						break;
			case 0x4 : 	eError = eNAND_4bitEccError;
						break;
			case 0x5 : 	eError = eNAND_UncorrectableError;
						break;
			default:	 	break;					
	}

	return eError;

}

//////////
void NAND_ReadECCErrorData(u32 Controller, u32 *uEccErr0, u32 *uEccErr1, u32 *uEccErr2)
{
	*uEccErr0 = 0 ;
	*uEccErr1 = 0 ;
	*uEccErr2 = 0 ;
	
	switch (NAND_Inform[Controller].uECCType)
	{
		case ECC1Bit:
			*uEccErr0 = Inp32(&(NAND(Controller)->rNFECCERR0));
			break;
		case ECC4Bit:
			*uEccErr0 = Inp32(&(NAND(Controller)->rNFECCERR0));
			*uEccErr1 = Inp32(&(NAND(Controller)->rNFECCERR1));			
			break;
		case ECC8Bit:
			*uEccErr0 = Inp32(&(NAND(Controller)->rNF8ECCERR0));
			*uEccErr1 = Inp32(&(NAND(Controller)->rNF8ECCERR1));			
			*uEccErr2 = Inp32(&(NAND(Controller)->rNF8ECCERR2));						
			break;
		default: break;
	}

}



void NAND_CorrecrErrorBit_4BitECC(u32 Controller, u32 *uEccErr0,  u32 *uEccErr1, u32 *uEccErr2, NAND_eERROR eError)
{
//	u32 u1ECCByte, u2EccByte, u3EccByte, u4EccByte, u5EccByte, u6EccByte, u7EccByte, u8EccByte;
//	u32 u1EccBit, u2EccBit, u3EccBit, u4EccBit, u5EccBit, u6EccBit, u7EccBit, u8EccBit;
	u32 uErrorData0, uErrorData1, uErrorData2;
//	u32 uErrPattern4Bit;
//	u32 uErrPattern8Bit;

	u32 uTemp0, uTemp1, uTemp2;

	uErrorData0 = *uEccErr0;
	uErrorData1 = *uEccErr1;
	uErrorData2 = *uEccErr2;
	uTemp2 = uErrorData2 ; // Comfile Error check by junil 

	if ( NAND_Inform[Controller].uECCType == ECC4Bit )
	{

		if ( eError & eNAND_1bitEccError )
		{
			uTemp0 = uErrorData0 & 0x3ff;
			uTemp1 = Inp32(&(NAND(Controller)->rNFMLCBITPT));
			UART_Printf("1st Error => (%4d)byte, (%8d)bit\n", uTemp0, GetBitPosition(uTemp1&0xff) );
		}
		if ( eError & eNAND_2bitEccError )
		{
			uTemp0 = uErrorData0 & 0x3ff;
			uTemp1 = Inp32(&(NAND(Controller)->rNFMLCBITPT));
			UART_Printf("1st Error => (%4d)byte, (%8d)bit\n", uTemp0, GetBitPosition(uTemp1&0xff) );
			NAND_Inform[Controller].u4BitECCCrrectTable[0][0] = uTemp0;
			NAND_Inform[Controller].u4BitECCCrrectTable[0][1] = GetBitPosition(uTemp1&0xff);
			uTemp0 = (uErrorData0>>16) & 0x3ff;
			uTemp1 = Inp32(&(NAND(Controller)->rNFMLCBITPT));
			UART_Printf("2nd Error => (%4d)byte, (%8d)bit\n", uTemp0,GetBitPosition((uTemp1>>8)&0xff) );
			NAND_Inform[Controller].u4BitECCCrrectTable[1][0] = uTemp0;
			NAND_Inform[Controller].u4BitECCCrrectTable[1][1] = GetBitPosition(uTemp1&0xff);
		}
		if ( eError & eNAND_3bitEccError )
		{
			uTemp0 = uErrorData0 & 0x3ff;
			uTemp1 = Inp32(&(NAND(Controller)->rNFMLCBITPT));
			UART_Printf("1st Error => (%4d)byte, (%8d)bit\n", uTemp0, GetBitPosition(uTemp1&0xff) );
			NAND_Inform[Controller].u4BitECCCrrectTable[0][0] = uTemp0;
			NAND_Inform[Controller].u4BitECCCrrectTable[0][1] = GetBitPosition(uTemp1&0xff);
			uTemp0 = (uErrorData0>>16) & 0x3ff;
			uTemp1 = Inp32(&(NAND(Controller)->rNFMLCBITPT));
			UART_Printf("2nd Error => (%4d)byte, (%8d)bit\n", uTemp0,GetBitPosition((uTemp1>>8)&0xff) );
			NAND_Inform[Controller].u4BitECCCrrectTable[1][0] = uTemp0;
			NAND_Inform[Controller].u4BitECCCrrectTable[1][1] = GetBitPosition((uTemp1>>8)&0xff);
			uTemp0 = uErrorData1 & 0x3ff;
			uTemp1 = Inp32(&(NAND(Controller)->rNFMLCBITPT));
			UART_Printf("3th Error => (%4d)byte, (%8d)bit\n", uTemp0, GetBitPosition((uTemp1>>16)&0xff) );
			NAND_Inform[Controller].u4BitECCCrrectTable[2][0] = uTemp0;
			NAND_Inform[Controller].u4BitECCCrrectTable[2][1] = GetBitPosition((uTemp1>>16)&0xff);
		}
		if ( eError & eNAND_4bitEccError )
		{
			uTemp0 = uErrorData0 & 0x3ff;
			uTemp1 = Inp32(&(NAND(Controller)->rNFMLCBITPT));
			UART_Printf("1st Error => (%4d)byte, (%8x)bit\n", uTemp0, GetBitPosition(uTemp1&0xff) );
			NAND_Inform[Controller].u4BitECCCrrectTable[0][0] = uTemp0;
			NAND_Inform[Controller].u4BitECCCrrectTable[0][1] = GetBitPosition(uTemp1&0xff);
			uTemp0 = (uErrorData0>>16) & 0x3ff;
			uTemp1 = Inp32(&(NAND(Controller)->rNFMLCBITPT));
			UART_Printf("2nd Error => (%4d)byte, (%8x)bit\n", uTemp0,GetBitPosition((uTemp1>>8)&0xff) );
			NAND_Inform[Controller].u4BitECCCrrectTable[1][0] = uTemp0;
			NAND_Inform[Controller].u4BitECCCrrectTable[1][1] = GetBitPosition((uTemp1>>8)&0xff);
			uTemp0 = uErrorData1 & 0x3ff;
			uTemp1 = Inp32(&(NAND(Controller)->rNFMLCBITPT));
			UART_Printf("3th Error => (%4d)byte, (%8x)bit\n", uTemp0, GetBitPosition((uTemp1>>16)&0xff) );
			NAND_Inform[Controller].u4BitECCCrrectTable[2][0] = uTemp0;
			NAND_Inform[Controller].u4BitECCCrrectTable[2][1] = GetBitPosition((uTemp1>>16)&0xff);
			uTemp0 = (uErrorData1>>16) & 0x3ff;
			uTemp1 = Inp32(&(NAND(Controller)->rNFMLCBITPT));
			UART_Printf("4th Error => (%4d)byte, (%8x)bit\n", uTemp0, GetBitPosition((uTemp1>>24)&0xff) );
			NAND_Inform[Controller].u4BitECCCrrectTable[3][0] = uTemp0;
			NAND_Inform[Controller].u4BitECCCrrectTable[3][1] = GetBitPosition((uTemp1>>24)&0xff);
		}
	}

}






void NAND_CheckECCPositoin(u32 Controller, u32 *uEccErr0, u32 *uEccErr1, u32 *uEccErr2, NAND_eERROR eError)
{
//	u32 u1ECCByte, u2EccByte, u3EccByte, u4EccByte, u5EccByte, u6EccByte, u7EccByte, u8EccByte;
//	u32 u1EccBit, u2EccBit, u3EccBit, u4EccBit, u5EccBit, u6EccBit, u7EccBit, u8EccBit;
	u32 uErrorData0, uErrorData1, uErrorData2;
//	u32 uErrPattern4Bit;
//	u32 uErrPattern8Bit;

	u32 uTemp0, uTemp1;

	uErrorData0 = *uEccErr0;
	uErrorData1 = *uEccErr1;
	uErrorData2 = *uEccErr2;

	if ( NAND_Inform[Controller].uECCType == ECC1Bit )
	{
		if ( eError & eNAND_1bitEccError )
		{
			uTemp0 = (uErrorData0>>7) & 0x3ff;
			uTemp1 = (uErrorData0>>4) & 0x7;
			UART_Printf("1st Error => (%4d)byte, (%8x)bit\n", uTemp0, uTemp1);
		}
		if ( eError & eNAND_Spare1bitEccError)
		{
			uTemp0 = (uErrorData0>>7) & 0x3ff;
			uTemp1 = (uErrorData0>>4) & 0x7;
			UART_Printf("1st Error => (%4d)byte, (%8x)bit\n", uTemp0+511, uTemp1);
		}
	}

	if ( NAND_Inform[Controller].uECCType == ECC4Bit )
	{

		if ( eError & eNAND_1bitEccError )
		{
					uTemp0 = uErrorData0 & 0x3ff;
					uTemp1 = Inp32(&(NAND(Controller)->rNFMLCBITPT));
					UART_Printf("1st Error => (%4d)byte, (%8x)bit\n", uTemp0, GetBitPosition(uTemp1&0xff) );
		}
		if ( eError & eNAND_2bitEccError )
		{
					uTemp0 = uErrorData0 & 0x3ff;
					uTemp1 = Inp32(&(NAND(Controller)->rNFMLCBITPT));
					UART_Printf("1st Error => (%4d)byte, (%8x)bit\n", uTemp0, GetBitPosition(uTemp1&0xff) );
					uTemp0 = (uErrorData0>>16) & 0x3ff;
					uTemp1 = Inp32(&(NAND(Controller)->rNFMLCBITPT));
					UART_Printf("2nd Error => (%4d)byte, (%8x)bit\n", uTemp0,GetBitPosition((uTemp1>>8)&0xff) );
		}
		if ( eError & eNAND_3bitEccError )
		{
					uTemp0 = uErrorData0 & 0x3ff;
					uTemp1 = Inp32(&(NAND(Controller)->rNFMLCBITPT));
					UART_Printf("1st Error => (%4d)byte, (%8x)bit\n", uTemp0, GetBitPosition(uTemp1&0xff) );
					uTemp0 = (uErrorData0>>16) & 0x3ff;
					uTemp1 = Inp32(&(NAND(Controller)->rNFMLCBITPT));
					UART_Printf("2nd Error => (%4d)byte, (%8x)bit\n", uTemp0,GetBitPosition((uTemp1>>8)&0xff) );
					uTemp0 = uErrorData1 & 0x3ff;
					uTemp1 = Inp32(&(NAND(Controller)->rNFMLCBITPT));
					UART_Printf("3th Error => (%4d)byte, (%8x)bit\n", uTemp0, GetBitPosition((uTemp1>>16)&0xff) );
		}
		if ( eError & eNAND_4bitEccError )
		{
					uTemp0 = uErrorData0 & 0x3ff;
					uTemp1 = Inp32(&(NAND(Controller)->rNFMLCBITPT));
					UART_Printf("1st Error => (%4d)byte, (%8x)bit\n", uTemp0, GetBitPosition(uTemp1&0xff) );
					uTemp0 = (uErrorData0>>16) & 0x3ff;
					uTemp1 = Inp32(&(NAND(Controller)->rNFMLCBITPT));
					UART_Printf("2nd Error => (%4d)byte, (%8x)bit\n", uTemp0,GetBitPosition((uTemp1>>8)&0xff) );
					uTemp0 = uErrorData1 & 0x3ff;
					uTemp1 = Inp32(&(NAND(Controller)->rNFMLCBITPT));
					UART_Printf("3th Error => (%4d)byte, (%8x)bit\n", uTemp0, GetBitPosition((uTemp1>>16)&0xff) );
					uTemp0 = (uErrorData1>>16) & 0x3ff;
					uTemp1 = Inp32(&(NAND(Controller)->rNFMLCBITPT));
					UART_Printf("4th Error => (%4d)byte, (%8x)bit\n", uTemp0, GetBitPosition((uTemp1>>24)&0xff) );
		}
	}




	if ( NAND_Inform[Controller].uECCType == ECC8Bit )
	{

		if ( eError & eNAND_1bitEccError )
		{
					uTemp0 = uErrorData0 & 0x3ff;
					uTemp1 = Inp32(&(NAND(Controller)->rNFMLC8BITPT0));
					UART_Printf("1st Error => (%4d)byte, (%8x)bit\n", uTemp0, GetBitPosition(uTemp1&0xff) );
		}
		if ( eError & eNAND_2bitEccError )
		{
					uTemp0 = uErrorData0 & 0x3ff;
					uTemp1 = Inp32(&(NAND(Controller)->rNFMLC8BITPT0));
					UART_Printf("1st Error => (%4d)byte, (%8x)bit\n", uTemp0, GetBitPosition(uTemp1&0xff) );
					uTemp0 = (uErrorData0>>15) & 0x3ff;
					uTemp1 = Inp32(&(NAND(Controller)->rNFMLC8BITPT0));
					UART_Printf("2nd Error => (%4d)byte, (%8x)bit\n", uTemp0,GetBitPosition((uTemp1>>8)&0xff) );
		}
		if ( eError & eNAND_3bitEccError )
		{
					uTemp0 = uErrorData0 & 0x3ff;
					uTemp1 = Inp32(&(NAND(Controller)->rNFMLC8BITPT0));
					UART_Printf("1st Error => (%4d)byte, (%8x)bit\n", uTemp0, GetBitPosition(uTemp1&0xff) );
					uTemp0 = (uErrorData0>>15) & 0x3ff;
					uTemp1 = Inp32(&(NAND(Controller)->rNFMLC8BITPT0));
					UART_Printf("2nd Error => (%4d)byte, (%8x)bit\n", uTemp0,GetBitPosition((uTemp1>>8)&0xff) );
					uTemp0 = uErrorData1 & 0x3ff;
					uTemp1 = Inp32(&(NAND(Controller)->rNFMLC8BITPT0));
					UART_Printf("3th Error => (%4d)byte, (%8x)bit\n", uTemp0, GetBitPosition((uTemp1>>16)&0xff) );
		}
		if ( eError & eNAND_4bitEccError )
		{
					uTemp0 = uErrorData0 & 0x3ff;
					uTemp1 = Inp32(&(NAND(Controller)->rNFMLC8BITPT0));
					UART_Printf("1st Error => (%4d)byte, (%8x)bit\n", uTemp0, GetBitPosition(uTemp1&0xff) );
					uTemp0 = (uErrorData0>>15) & 0x3ff;
					uTemp1 = Inp32(&(NAND(Controller)->rNFMLC8BITPT0));
					UART_Printf("2nd Error => (%4d)byte, (%8x)bit\n", uTemp0,GetBitPosition((uTemp1>>8)&0xff) );
					uTemp0 = uErrorData1 & 0x3ff;
					uTemp1 = Inp32(&(NAND(Controller)->rNFMLC8BITPT0));
					UART_Printf("3th Error => (%4d)byte, (%8x)bit\n", uTemp0, GetBitPosition((uTemp1>>16)&0xff) );
					uTemp0 = (uErrorData1>>11) & 0x3ff;
					uTemp1 = Inp32(&(NAND(Controller)->rNFMLC8BITPT0));
					UART_Printf("4th Error => (%4d)byte, (%8x)bit\n", uTemp0, GetBitPosition((uTemp1>>24)&0xff) );
		}
		if ( eError & eNAND_5bitEccError )
		{
					uTemp0 = uErrorData0 & 0x3ff;
					uTemp1 = Inp32(&(NAND(Controller)->rNFMLC8BITPT0));
					UART_Printf("1st Error => (%4d)byte, (%8x)bit\n", uTemp0, GetBitPosition(uTemp1&0xff) );
					uTemp0 = (uErrorData0>>15) & 0x3ff;
					uTemp1 = Inp32(&(NAND(Controller)->rNFMLC8BITPT0));
					UART_Printf("2nd Error => (%4d)byte, (%8x)bit\n", uTemp0,GetBitPosition((uTemp1>>8)&0xff) );
					uTemp0 = uErrorData1 & 0x3ff;
					uTemp1 = Inp32(&(NAND(Controller)->rNFMLC8BITPT0));
					UART_Printf("3th Error => (%4d)byte, (%8x)bit\n", uTemp0, GetBitPosition((uTemp1>>16)&0xff) );
					uTemp0 = (uErrorData1>>11) & 0x3ff;
					uTemp1 = Inp32(&(NAND(Controller)->rNFMLC8BITPT0));
					UART_Printf("4th Error => (%4d)byte, (%8x)bit\n", uTemp0, GetBitPosition((uTemp1>>24)&0xff) );
					uTemp0 = (uErrorData1>>22) & 0x3ff;
					uTemp1 = Inp32(&(NAND(Controller)->rNFMLC8BITPT1));
					UART_Printf("5th Error => (%4d)byte, (%8x)bit\n", uTemp0, GetBitPosition(uTemp1&0xff) );
		}
		if ( eError & eNAND_6bitEccError )
		{
					uTemp0 = uErrorData0 & 0x3ff;
					uTemp1 = Inp32(&(NAND(Controller)->rNFMLC8BITPT0));
					UART_Printf("1st Error => (%4d)byte, (%8x)bit\n", uTemp0, GetBitPosition(uTemp1&0xff) );
					uTemp0 = (uErrorData0>>15) & 0x3ff;
					uTemp1 = Inp32(&(NAND(Controller)->rNFMLC8BITPT0));
					UART_Printf("2nd Error => (%4d)byte, (%8x)bit\n", uTemp0,GetBitPosition((uTemp1>>8)&0xff) );
					uTemp0 = uErrorData1 & 0x3ff;
					uTemp1 = Inp32(&(NAND(Controller)->rNFMLC8BITPT0));
					UART_Printf("3th Error => (%4d)byte, (%8x)bit\n", uTemp0, GetBitPosition((uTemp1>>16)&0xff) );
					uTemp0 = (uErrorData1>>11) & 0x3ff;
					uTemp1 = Inp32(&(NAND(Controller)->rNFMLC8BITPT0));
					UART_Printf("4th Error => (%4d)byte, (%8x)bit\n", uTemp0, GetBitPosition((uTemp1>>24)&0xff) );
					uTemp0 = (uErrorData1>>22) & 0x3ff;
					uTemp1 = Inp32(&(NAND(Controller)->rNFMLC8BITPT1));
					UART_Printf("5th Error => (%4d)byte, (%8x)bit\n", uTemp0, GetBitPosition(uTemp1&0xff) );
					uTemp0 = uErrorData2 & 0x3ff;
					uTemp1 = Inp32(&(NAND(Controller)->rNFMLC8BITPT1));
					UART_Printf("6th Error => (%4d)byte, (%8x)bit\n", uTemp0, GetBitPosition((uTemp1>>8)&0xff) );
		}
		if ( eError & eNAND_7bitEccError )
		{
					uTemp0 = uErrorData0 & 0x3ff;
					uTemp1 = Inp32(&(NAND(Controller)->rNFMLC8BITPT0));
					UART_Printf("1st Error => (%4d)byte, (%8x)bit\n", uTemp0, GetBitPosition(uTemp1&0xff) );
					uTemp0 = (uErrorData0>>15) & 0x3ff;
					uTemp1 = Inp32(&(NAND(Controller)->rNFMLC8BITPT0));
					UART_Printf("2nd Error => (%4d)byte, (%8x)bit\n", uTemp0,GetBitPosition((uTemp1>>8)&0xff) );
					uTemp0 = uErrorData1 & 0x3ff;
					uTemp1 = Inp32(&(NAND(Controller)->rNFMLC8BITPT0));
					UART_Printf("3th Error => (%4d)byte, (%8x)bit\n", uTemp0, GetBitPosition((uTemp1>>16)&0xff) );
					uTemp0 = (uErrorData1>>11) & 0x3ff;
					uTemp1 = Inp32(&(NAND(Controller)->rNFMLC8BITPT0));
					UART_Printf("4th Error => (%4d)byte, (%8x)bit\n", uTemp0, GetBitPosition((uTemp1>>24)&0xff) );
					uTemp0 = (uErrorData1>>22) & 0x3ff;
					uTemp1 = Inp32(&(NAND(Controller)->rNFMLC8BITPT1));
					UART_Printf("5th Error => (%4d)byte, (%8x)bit\n", uTemp0, GetBitPosition(uTemp1&0xff) );
					uTemp0 = uErrorData2 & 0x3ff;
					uTemp1 = Inp32(&(NAND(Controller)->rNFMLC8BITPT1));
					UART_Printf("6th Error => (%4d)byte, (%8x)bit\n", uTemp0, GetBitPosition((uTemp1>>8)&0xff) );
					uTemp0 = (uErrorData2>>11) & 0x3ff;
					uTemp1 = Inp32(&(NAND(Controller)->rNFMLC8BITPT1));
					UART_Printf("7th Error => (%4d)byte, (%8x)bit\n", uTemp0, GetBitPosition((uTemp1>>16)&0xff) );
		}
		if ( eError & eNAND_8bitEccError )
		{
					uTemp0 = uErrorData0 & 0x3ff;
					uTemp1 = Inp32(&(NAND(Controller)->rNFMLC8BITPT0));
					UART_Printf("1st Error => (%4d)byte, (%8x)bit\n", uTemp0, GetBitPosition(uTemp1&0xff) );
					uTemp0 = (uErrorData0>>15) & 0x3ff;
					uTemp1 = Inp32(&(NAND(Controller)->rNFMLC8BITPT0));
					UART_Printf("2nd Error => (%4d)byte, (%8x)bit\n", uTemp0,GetBitPosition((uTemp1>>8)&0xff) );
					uTemp0 = uErrorData1 & 0x3ff;
					uTemp1 = Inp32(&(NAND(Controller)->rNFMLC8BITPT0));
					UART_Printf("3th Error => (%4d)byte, (%8x)bit\n", uTemp0, GetBitPosition((uTemp1>>16)&0xff) );
					uTemp0 = (uErrorData1>>11) & 0x3ff;
					uTemp1 = Inp32(&(NAND(Controller)->rNFMLC8BITPT0));
					UART_Printf("4th Error => (%4d)byte, (%8x)bit\n", uTemp0, GetBitPosition((uTemp1>>24)&0xff) );
					uTemp0 = (uErrorData1>>22) & 0x3ff;
					uTemp1 = Inp32(&(NAND(Controller)->rNFMLC8BITPT1));
					UART_Printf("5th Error => (%4d)byte, (%8x)bit\n", uTemp0, GetBitPosition(uTemp1&0xff) );
					uTemp0 = uErrorData2 & 0x3ff;
					uTemp1 = Inp32(&(NAND(Controller)->rNFMLC8BITPT1));
					UART_Printf("6th Error => (%4d)byte, (%8x)bit\n", uTemp0, GetBitPosition((uTemp1>>8)&0xff) );
					uTemp0 = (uErrorData2>>11) & 0x3ff;
					uTemp1 = Inp32(&(NAND(Controller)->rNFMLC8BITPT1));
					UART_Printf("7th Error => (%4d)byte, (%8x)bit\n", uTemp0, GetBitPosition((uTemp1>>16)&0xff) );
					uTemp0 = (uErrorData2>>22) & 0x3ff;
					uTemp1 = Inp32(&(NAND(Controller)->rNFMLC8BITPT1));
					UART_Printf("8th Error => (%4d)byte, (%8x)bit\n", uTemp0, GetBitPosition((uTemp1>>24)&0xff) );
		}
	}




}


void NAND_CheckECCPositoin_8BitAll(u32 Controller, u32 *uEccErr0, u32 *uEccErr1, u32 *uEccErr2)
{
//	u32 u1ECCByte, u2EccByte, u3EccByte, u4EccByte, u5EccByte, u6EccByte, u7EccByte, u8EccByte;
//	u32 u1EccBit, u2EccBit, u3EccBit, u4EccBit, u5EccBit, u6EccBit, u7EccBit, u8EccBit;
	u32 uErrorData0, uErrorData1, uErrorData2;
//	u32 uErrPattern4Bit;
//	u32 uErrPattern8Bit;

	u32 uIndex, uTemp0, uTemp1, uTemp4;
	u32 uIndex1, uIndex2;
	u32 uTemp3;

	uErrorData0 = *uEccErr0;
	uErrorData1 = *uEccErr1;
	uErrorData2 = *uEccErr2;

	if ( NAND_Inform[Controller].uECCType == ECC1Bit )
	{
		if ( NAND_Inform[Controller].uECCtest == 1 )
		{
			uTemp0 = (uErrorData0>>7) & 0x3ff;
			uTemp1 = (uErrorData0>>4) & 0x7;
			NAND_Inform[Controller].uSLCError1bit_Compare[0] = (uTemp0 *8)+uTemp1;
		}
		if ( NAND_Inform[Controller].uSpareECC == 1 )
		{
			uTemp0 = (uErrorData0>>21) & 0x3ff;
			uTemp1 = (uErrorData0>>18) & 0x7;
			NAND_Inform[Controller].uSLCError1bit_Compare[0] = ( (uTemp0+512)*8 )+uTemp1;
		}

		if (NAND_Inform[Controller].uSLCError1bit_Compare[0] == NAND_Inform[Controller].uSLCError1bit_Original[0])
		{
			UART_Printf("Origianl Bit = %4d Bit ===== Detect Bit = %4d Bit\n", NAND_Inform[Controller].uSLCError1bit_Original[0], NAND_Inform[Controller].uSLCError1bit_Compare[0]);
		}
		else
		{
			UART_Printf("Error Bit Detect is Bad\n");				
			UART_Printf("Origianl Bit = %4d Bit ===== Detect Bit = %4d Bit\n", NAND_Inform[Controller].uSLCError1bit_Original[0], NAND_Inform[Controller].uSLCError1bit_Compare[0]);
			UART_Printf("Check This Condition\n");
			uTemp4=UART_GetIntNum();
		}
	}

	if ( NAND_Inform[Controller].uECCType == ECC4Bit )
	{
		switch (NAND_Inform[Controller].u4bitECCPattern )
		{
			case 1:
				uTemp0 = uErrorData0 & 0x3ff;
				uTemp1 = Inp32(&(NAND(Controller)->rNFMLCBITPT));
				uTemp3 = GetBitPosition(uTemp1&0xff);
				NAND_Inform[Controller].uSLCError4bit_Compare[0]= (uTemp0*8)+uTemp3;
				//UART_Printf("1st Error => (%4d)byte, (%8x)bit\n", uTemp0, GetBitPosition(uTemp1&0xff) );
				break;
			case 2:
				uTemp0 = uErrorData0 & 0x3ff;
				uTemp1 = Inp32(&(NAND(Controller)->rNFMLCBITPT));
				uTemp3 = GetBitPosition(uTemp1&0xff);
				NAND_Inform[Controller].uSLCError4bit_Compare[0]= (uTemp0*8)+uTemp3;
				//		UART_Printf("1st Error => (%4d)byte, (%8x)bit\n", uTemp0, GetBitPosition(uTemp1&0xff) );

				uTemp0 = (uErrorData0>>16) & 0x3ff;
				uTemp1 = Inp32(&(NAND(Controller)->rNFMLCBITPT));
				uTemp3 = GetBitPosition((uTemp1>>8)&0xff);
				NAND_Inform[Controller].uSLCError4bit_Compare[1]= (uTemp0*8)+uTemp3;
				//		UART_Printf("2nd Error => (%4d)byte, (%8x)bit\n", uTemp0,GetBitPosition((uTemp1>>8)&0xff) );
				break;
			case 3:
				uTemp0 = uErrorData0 & 0x3ff;
				uTemp1 = Inp32(&(NAND(Controller)->rNFMLCBITPT));
				uTemp3 = GetBitPosition(uTemp1&0xff);
				NAND_Inform[Controller].uSLCError4bit_Compare[0]= (uTemp0*8)+uTemp3;
				//		UART_Printf("1st Error => (%4d)byte, (%8x)bit\n", uTemp0, GetBitPosition(uTemp1&0xff) );

				uTemp0 = (uErrorData0>>16) & 0x3ff;
				uTemp1 = Inp32(&(NAND(Controller)->rNFMLCBITPT));
				uTemp3 = GetBitPosition((uTemp1>>8)&0xff);
				NAND_Inform[Controller].uSLCError4bit_Compare[1]= (uTemp0*8)+uTemp3;
				//		UART_Printf("2nd Error => (%4d)byte, (%8x)bit\n", uTemp0,GetBitPosition((uTemp1>>8)&0xff) );

				uTemp0 = uErrorData1 & 0x3ff;
				uTemp1 = Inp32(&(NAND(Controller)->rNFMLCBITPT));
				uTemp3 = GetBitPosition((uTemp1>>16)&0xff);
				NAND_Inform[Controller].uSLCError4bit_Compare[2]= (uTemp0*8)+uTemp3;
				//		UART_Printf("3th Error => (%4d)byte, (%8x)bit\n", uTemp0, GetBitPosition((uTemp1>>16)&0xff) );
				break;
			case 4:
				uTemp0 = uErrorData0 & 0x3ff;
				uTemp1 = Inp32(&(NAND(Controller)->rNFMLCBITPT));
				uTemp3 = GetBitPosition(uTemp1&0xff);
				NAND_Inform[Controller].uSLCError4bit_Compare[0]= (uTemp0*8)+uTemp3;
				//		UART_Printf("1st Error => (%4d)byte, (%8x)bit\n", uTemp0, GetBitPosition(uTemp1&0xff) );

				uTemp0 = (uErrorData0>>16) & 0x3ff;
				uTemp1 = Inp32(&(NAND(Controller)->rNFMLCBITPT));
				uTemp3 = GetBitPosition((uTemp1>>8)&0xff);
				NAND_Inform[Controller].uSLCError4bit_Compare[1]= (uTemp0*8)+uTemp3;
				//		UART_Printf("2nd Error => (%4d)byte, (%8x)bit\n", uTemp0,GetBitPosition((uTemp1>>8)&0xff) );

				uTemp0 = uErrorData1 & 0x3ff;
				uTemp1 = Inp32(&(NAND(Controller)->rNFMLCBITPT));
				uTemp3 = GetBitPosition((uTemp1>>16)&0xff);
				NAND_Inform[Controller].uSLCError4bit_Compare[2]= (uTemp0*8)+uTemp3;
				//		UART_Printf("3th Error => (%4d)byte, (%8x)bit\n", uTemp0, GetBitPosition((uTemp1>>16)&0xff) );

				uTemp0 = (uErrorData1>>16) & 0x3ff;
				uTemp1 = Inp32(&(NAND(Controller)->rNFMLCBITPT));
				uTemp3 = GetBitPosition((uTemp1>>24)&0xff);
				NAND_Inform[Controller].uSLCError4bit_Compare[3]= (uTemp0*8)+uTemp3;
				//		UART_Printf("4th Error => (%4d)byte, (%8x)bit\n", uTemp0, GetBitPosition((uTemp1>>24)&0xff) );
				break;
			default:
				break;
		}


		switch ( NAND_Inform[Controller].u4bitECCPattern )
		{
			case 1:
				break;
			case 2:
				for ( uIndex1=0; uIndex1<2; uIndex1++)
				{
					for ( uIndex2=uIndex1+1; uIndex2<2; uIndex2++)
					{
						if ( NAND_Inform[Controller].uSLCError4bit_Compare[uIndex1] > NAND_Inform[Controller].uSLCError4bit_Compare[uIndex2] )
						{
							NAND_Inform[Controller].uSLCError4bit_Temp[0] =  NAND_Inform[Controller].uSLCError4bit_Compare[uIndex1];
							NAND_Inform[Controller].uSLCError4bit_Compare[uIndex1] = NAND_Inform[Controller].uSLCError4bit_Compare[uIndex2];
							NAND_Inform[Controller].uSLCError4bit_Compare[uIndex2] = NAND_Inform[Controller].uSLCError4bit_Temp[0];
						}
					}
				}
				break;
			case 3:
				for ( uIndex1=0; uIndex1<3; uIndex1++)
				{
					for ( uIndex2=uIndex1+1; uIndex2<3; uIndex2++)
					{
						if ( NAND_Inform[Controller].uSLCError4bit_Compare[uIndex1] > NAND_Inform[Controller].uSLCError4bit_Compare[uIndex2] )
						{
							NAND_Inform[Controller].uSLCError4bit_Temp[0] =  NAND_Inform[Controller].uSLCError4bit_Compare[uIndex1];
							NAND_Inform[Controller].uSLCError4bit_Compare[uIndex1] = NAND_Inform[Controller].uSLCError4bit_Compare[uIndex2];
							NAND_Inform[Controller].uSLCError4bit_Compare[uIndex2] = NAND_Inform[Controller].uSLCError4bit_Temp[0];
						}
					}
				}
				break;
			case 4:
				for ( uIndex1=0; uIndex1<4; uIndex1++)
				{
					for ( uIndex2=uIndex1+1; uIndex2<4; uIndex2++)
					{
						if ( NAND_Inform[Controller].uSLCError4bit_Compare[uIndex1] > NAND_Inform[Controller].uSLCError4bit_Compare[uIndex2] )
						{
							NAND_Inform[Controller].uSLCError4bit_Temp[0] =  NAND_Inform[Controller].uSLCError4bit_Compare[uIndex1];
							NAND_Inform[Controller].uSLCError4bit_Compare[uIndex1] = NAND_Inform[Controller].uSLCError4bit_Compare[uIndex2];
							NAND_Inform[Controller].uSLCError4bit_Compare[uIndex2] = NAND_Inform[Controller].uSLCError4bit_Temp[0];
						}
					}
				}
				break;
			default :
				break;
				
		}

		switch ( NAND_Inform[Controller].u4bitECCPattern )
		{
			case 1:
				break;
			case 2:
				for ( uIndex1=0; uIndex1<2; uIndex1++)
				{
					for ( uIndex2=uIndex1+1; uIndex2<2; uIndex2++)
					{
						if ( NAND_Inform[Controller].uSLCError4bit_Original[uIndex1] > NAND_Inform[Controller].uSLCError4bit_Original[uIndex2] )
						{
							NAND_Inform[Controller].uSLCError4bit_Temp[0] =  NAND_Inform[Controller].uSLCError4bit_Original[uIndex1];
							NAND_Inform[Controller].uSLCError4bit_Original[uIndex1] = NAND_Inform[Controller].uSLCError4bit_Original[uIndex2];
							NAND_Inform[Controller].uSLCError4bit_Original[uIndex2] = NAND_Inform[Controller].uSLCError4bit_Temp[0];
						}
					}
				}
				break;
			case 3:
				for ( uIndex1=0; uIndex1<3; uIndex1++)
				{
					for ( uIndex2=uIndex1+1; uIndex2<3; uIndex2++)
					{
						if ( NAND_Inform[Controller].uSLCError4bit_Original[uIndex1] > NAND_Inform[Controller].uSLCError4bit_Original[uIndex2] )
						{
							NAND_Inform[Controller].uSLCError4bit_Temp[0] =  NAND_Inform[Controller].uSLCError4bit_Original[uIndex1];
							NAND_Inform[Controller].uSLCError4bit_Original[uIndex1] = NAND_Inform[Controller].uSLCError4bit_Original[uIndex2];
							NAND_Inform[Controller].uSLCError4bit_Original[uIndex2] = NAND_Inform[Controller].uSLCError4bit_Temp[0];
						}
					}
				}
				break;
			case 4:
				for ( uIndex1=0; uIndex1<4; uIndex1++)
				{
					for ( uIndex2=uIndex1+1; uIndex2<4; uIndex2++)
					{
						if ( NAND_Inform[Controller].uSLCError4bit_Original[uIndex1] > NAND_Inform[Controller].uSLCError4bit_Original[uIndex2] )
						{
							NAND_Inform[Controller].uSLCError4bit_Temp[0] =  NAND_Inform[Controller].uSLCError4bit_Original[uIndex1];
							NAND_Inform[Controller].uSLCError4bit_Original[uIndex1] = NAND_Inform[Controller].uSLCError4bit_Original[uIndex2];
							NAND_Inform[Controller].uSLCError4bit_Original[uIndex2] = NAND_Inform[Controller].uSLCError4bit_Temp[0];
						}
					}
				}
				break;
			default :
				break;
				
		}

		switch ( NAND_Inform[Controller].u4bitECCPattern )
		{
			case 1:
				for ( uIndex=0; uIndex<1 ; uIndex++)
				{
					if ( NAND_Inform[Controller].uSLCError4bit_Compare[uIndex] == NAND_Inform[Controller].uSLCError4bit_Original[uIndex] )
					{
						UART_Printf("%2d Error Bit Detect is Good\n", uIndex);
						UART_Printf("Origianl Bit = %4d Bit ===== Detect Bit = %4d Bit\n", NAND_Inform[Controller].uSLCError4bit_Original[uIndex], NAND_Inform[Controller].uSLCError4bit_Compare[uIndex]);
					}
					else
					{
						UART_Printf("%2d Error Bit Detect is Bad\n", uIndex);				
						UART_Printf("Origianl Bit = %4d Bit ===== Detect Bit = %4d Bit\n", NAND_Inform[Controller].uSLCError4bit_Original[uIndex], NAND_Inform[Controller].uSLCError4bit_Compare[uIndex]);
						UART_Printf("Check This Condition\n");
						uTemp4=UART_GetIntNum();
					}
				}
				break;
			case 2:
				for ( uIndex=0; uIndex<2 ; uIndex++)
				{
					if ( NAND_Inform[Controller].uSLCError4bit_Compare[uIndex] == NAND_Inform[Controller].uSLCError4bit_Original[uIndex] )
					{
						UART_Printf("%2d Error Bit Detect is Good\n", uIndex);
						UART_Printf("Origianl Bit = %4d Bit ===== Detect Bit = %4d Bit\n", NAND_Inform[Controller].uSLCError4bit_Original[uIndex], NAND_Inform[Controller].uSLCError4bit_Compare[uIndex]);
					}
					else
					{
						UART_Printf("%2d Error Bit Detect is Bad\n", uIndex);				
						UART_Printf("Origianl Bit = %4d Bit ===== Detect Bit = %4d Bit\n", NAND_Inform[Controller].uSLCError4bit_Original[uIndex], NAND_Inform[Controller].uSLCError4bit_Compare[uIndex]);
						UART_Printf("Check This Condition\n");
						uTemp4=UART_GetIntNum();
					}
				}
				break;
			case 3:
				for ( uIndex=0; uIndex<3 ; uIndex++)
				{
					if ( NAND_Inform[Controller].uSLCError4bit_Compare[uIndex] == NAND_Inform[Controller].uSLCError4bit_Original[uIndex] )
					{
						UART_Printf("%2d Error Bit Detect is Good\n", uIndex);
						UART_Printf("Origianl Bit = %4d Bit ===== Detect Bit = %4d Bit\n", NAND_Inform[Controller].uSLCError4bit_Original[uIndex], NAND_Inform[Controller].uSLCError4bit_Compare[uIndex]);
					}
					else
					{
						UART_Printf("%2d Error Bit Detect is Bad\n", uIndex);				
						UART_Printf("Origianl Bit = %4d Bit ===== Detect Bit = %4d Bit\n", NAND_Inform[Controller].uSLCError4bit_Original[uIndex], NAND_Inform[Controller].uSLCError4bit_Compare[uIndex]);
						UART_Printf("Check This Condition\n");
						uTemp4=UART_GetIntNum();
					}
				}
				break;
			case 4:
				for ( uIndex=0; uIndex<4 ; uIndex++)
				{
					if ( NAND_Inform[Controller].uSLCError4bit_Compare[uIndex] == NAND_Inform[Controller].uSLCError4bit_Original[uIndex] )
					{
						UART_Printf("%2d Error Bit Detect is Good\n", uIndex);
						UART_Printf("Origianl Bit = %4d Bit ===== Detect Bit = %4d Bit\n", NAND_Inform[Controller].uSLCError4bit_Original[uIndex], NAND_Inform[Controller].uSLCError4bit_Compare[uIndex]);
					}
					else
					{
						UART_Printf("%2d Error Bit Detect is Bad\n", uIndex);				
						UART_Printf("Origianl Bit = %4d Bit ===== Detect Bit = %4d Bit\n", NAND_Inform[Controller].uSLCError4bit_Original[uIndex], NAND_Inform[Controller].uSLCError4bit_Compare[uIndex]);
						UART_Printf("Check This Condition\n");
						uTemp4=UART_GetIntNum();
					}
				}
				break;
			default :
				break;

		}

	}
		

	if ( NAND_Inform[Controller].uECCType == ECC8Bit )
	{
		switch (NAND_Inform[Controller].u8bitECCPattern )
		{
			case 1:
				uTemp0 = uErrorData0 & 0x3ff;
				uTemp1 = Inp32(&(NAND(Controller)->rNFMLC8BITPT0));
				uTemp3 = GetBitPosition(uTemp1&0xff);
				NAND_Inform[Controller].uSLCError8bit_Compare[0]= (uTemp0*8)+uTemp3;
				//UART_Printf("1st Error => (%4d)byte, (%8x)bit\n", uTemp0, GetBitPosition(uTemp1&0xff) );
				break;
			case 2:
				uTemp0 = uErrorData0 & 0x3ff;
				uTemp1 = Inp32(&(NAND(Controller)->rNFMLC8BITPT0));
				uTemp3 = GetBitPosition(uTemp1&0xff);
				NAND_Inform[Controller].uSLCError8bit_Compare[0]= (uTemp0*8)+uTemp3;
				//		UART_Printf("1st Error => (%4d)byte, (%8x)bit\n", uTemp0, GetBitPosition(uTemp1&0xff) );

				uTemp0 = (uErrorData0>>15) & 0x3ff;
				uTemp1 = Inp32(&(NAND(Controller)->rNFMLC8BITPT0));
				uTemp3 = GetBitPosition((uTemp1>>8)&0xff);
				NAND_Inform[Controller].uSLCError8bit_Compare[1]= (uTemp0*8)+uTemp3;
				//		UART_Printf("2nd Error => (%4d)byte, (%8x)bit\n", uTemp0,GetBitPosition((uTemp1>>8)&0xff) );
				break;
			case 3:
				uTemp0 = uErrorData0 & 0x3ff;
				uTemp1 = Inp32(&(NAND(Controller)->rNFMLC8BITPT0));
				uTemp3 = GetBitPosition(uTemp1&0xff);
				NAND_Inform[Controller].uSLCError8bit_Compare[0]= (uTemp0*8)+uTemp3;
				//		UART_Printf("1st Error => (%4d)byte, (%8x)bit\n", uTemp0, GetBitPosition(uTemp1&0xff) );

				uTemp0 = (uErrorData0>>15) & 0x3ff;
				uTemp1 = Inp32(&(NAND(Controller)->rNFMLC8BITPT0));
				uTemp3 = GetBitPosition((uTemp1>>8)&0xff);
				NAND_Inform[Controller].uSLCError8bit_Compare[1]= (uTemp0*8)+uTemp3;
				//		UART_Printf("2nd Error => (%4d)byte, (%8x)bit\n", uTemp0,GetBitPosition((uTemp1>>8)&0xff) );

				uTemp0 = uErrorData1 & 0x3ff;
				uTemp1 = Inp32(&(NAND(Controller)->rNFMLC8BITPT0));
				uTemp3 = GetBitPosition((uTemp1>>16)&0xff);
				NAND_Inform[Controller].uSLCError8bit_Compare[2]= (uTemp0*8)+uTemp3;
				//		UART_Printf("3th Error => (%4d)byte, (%8x)bit\n", uTemp0, GetBitPosition((uTemp1>>16)&0xff) );
				break;
			case 4:
				uTemp0 = uErrorData0 & 0x3ff;
				uTemp1 = Inp32(&(NAND(Controller)->rNFMLC8BITPT0));
				uTemp3 = GetBitPosition(uTemp1&0xff);
				NAND_Inform[Controller].uSLCError8bit_Compare[0]= (uTemp0*8)+uTemp3;
				//		UART_Printf("1st Error => (%4d)byte, (%8x)bit\n", uTemp0, GetBitPosition(uTemp1&0xff) );

				uTemp0 = (uErrorData0>>15) & 0x3ff;
				uTemp1 = Inp32(&(NAND(Controller)->rNFMLC8BITPT0));
				uTemp3 = GetBitPosition((uTemp1>>8)&0xff);
				NAND_Inform[Controller].uSLCError8bit_Compare[1]= (uTemp0*8)+uTemp3;
				//		UART_Printf("2nd Error => (%4d)byte, (%8x)bit\n", uTemp0,GetBitPosition((uTemp1>>8)&0xff) );

				uTemp0 = uErrorData1 & 0x3ff;
				uTemp1 = Inp32(&(NAND(Controller)->rNFMLC8BITPT0));
				uTemp3 = GetBitPosition((uTemp1>>16)&0xff);
				NAND_Inform[Controller].uSLCError8bit_Compare[2]= (uTemp0*8)+uTemp3;
				//		UART_Printf("3th Error => (%4d)byte, (%8x)bit\n", uTemp0, GetBitPosition((uTemp1>>16)&0xff) );

				uTemp0 = (uErrorData1>>11) & 0x3ff;
				uTemp1 = Inp32(&(NAND(Controller)->rNFMLC8BITPT0));
				uTemp3 = GetBitPosition((uTemp1>>24)&0xff);
				NAND_Inform[Controller].uSLCError8bit_Compare[3]= (uTemp0*8)+uTemp3;
				//		UART_Printf("4th Error => (%4d)byte, (%8x)bit\n", uTemp0, GetBitPosition((uTemp1>>24)&0xff) );
				break;
			case 5:
				uTemp0 = uErrorData0 & 0x3ff;
				uTemp1 = Inp32(&(NAND(Controller)->rNFMLC8BITPT0));
				uTemp3 = GetBitPosition(uTemp1&0xff);
				NAND_Inform[Controller].uSLCError8bit_Compare[0]= (uTemp0*8)+uTemp3;
				//		UART_Printf("1st Error => (%4d)byte, (%8x)bit\n", uTemp0, GetBitPosition(uTemp1&0xff) );

				uTemp0 = (uErrorData0>>15) & 0x3ff;
				uTemp1 = Inp32(&(NAND(Controller)->rNFMLC8BITPT0));
				uTemp3 = GetBitPosition((uTemp1>>8)&0xff);
				NAND_Inform[Controller].uSLCError8bit_Compare[1]= (uTemp0*8)+uTemp3;
				//		UART_Printf("2nd Error => (%4d)byte, (%8x)bit\n", uTemp0,GetBitPosition((uTemp1>>8)&0xff) );

				uTemp0 = uErrorData1 & 0x3ff;
				uTemp1 = Inp32(&(NAND(Controller)->rNFMLC8BITPT0));
				uTemp3 = GetBitPosition((uTemp1>>16)&0xff);
				NAND_Inform[Controller].uSLCError8bit_Compare[2]= (uTemp0*8)+uTemp3;
				//		UART_Printf("3th Error => (%4d)byte, (%8x)bit\n", uTemp0, GetBitPosition((uTemp1>>16)&0xff) );

				uTemp0 = (uErrorData1>>11) & 0x3ff;
				uTemp1 = Inp32(&(NAND(Controller)->rNFMLC8BITPT0));
				uTemp3 = GetBitPosition((uTemp1>>24)&0xff);
				NAND_Inform[Controller].uSLCError8bit_Compare[3]= (uTemp0*8)+uTemp3;
				//		UART_Printf("4th Error => (%4d)byte, (%8x)bit\n", uTemp0, GetBitPosition((uTemp1>>24)&0xff) );

				uTemp0 = (uErrorData1>>22) & 0x3ff;
				uTemp1 = Inp32(&(NAND(Controller)->rNFMLC8BITPT1));
				uTemp3 = GetBitPosition(uTemp1&0xff);
				NAND_Inform[Controller].uSLCError8bit_Compare[4]= (uTemp0*8)+uTemp3;
				//		UART_Printf("5th Error => (%4d)byte, (%8x)bit\n", uTemp0, GetBitPosition(uTemp1&0xff) );
				break;
			case 6:
				uTemp0 = uErrorData0 & 0x3ff;
				uTemp1 = Inp32(&(NAND(Controller)->rNFMLC8BITPT0));
				uTemp3 = GetBitPosition(uTemp1&0xff);
				NAND_Inform[Controller].uSLCError8bit_Compare[0]= (uTemp0*8)+uTemp3;
				//		UART_Printf("1st Error => (%4d)byte, (%8x)bit\n", uTemp0, GetBitPosition(uTemp1&0xff) );

				uTemp0 = (uErrorData0>>15) & 0x3ff;
				uTemp1 = Inp32(&(NAND(Controller)->rNFMLC8BITPT0));
				uTemp3 = GetBitPosition((uTemp1>>8)&0xff);
				NAND_Inform[Controller].uSLCError8bit_Compare[1]= (uTemp0*8)+uTemp3;
				//		UART_Printf("2nd Error => (%4d)byte, (%8x)bit\n", uTemp0,GetBitPosition((uTemp1>>8)&0xff) );

				uTemp0 = uErrorData1 & 0x3ff;
				uTemp1 = Inp32(&(NAND(Controller)->rNFMLC8BITPT0));
				uTemp3 = GetBitPosition((uTemp1>>16)&0xff);
				NAND_Inform[Controller].uSLCError8bit_Compare[2]= (uTemp0*8)+uTemp3;
				//		UART_Printf("3th Error => (%4d)byte, (%8x)bit\n", uTemp0, GetBitPosition((uTemp1>>16)&0xff) );

				uTemp0 = (uErrorData1>>11) & 0x3ff;
				uTemp1 = Inp32(&(NAND(Controller)->rNFMLC8BITPT0));
				uTemp3 = GetBitPosition((uTemp1>>24)&0xff);
				NAND_Inform[Controller].uSLCError8bit_Compare[3]= (uTemp0*8)+uTemp3;
				//		UART_Printf("4th Error => (%4d)byte, (%8x)bit\n", uTemp0, GetBitPosition((uTemp1>>24)&0xff) );

				uTemp0 = (uErrorData1>>22) & 0x3ff;
				uTemp1 = Inp32(&(NAND(Controller)->rNFMLC8BITPT1));
				uTemp3 = GetBitPosition(uTemp1&0xff);
				NAND_Inform[Controller].uSLCError8bit_Compare[4]= (uTemp0*8)+uTemp3;
				//		UART_Printf("5th Error => (%4d)byte, (%8x)bit\n", uTemp0, GetBitPosition(uTemp1&0xff) );

				uTemp0 = uErrorData2 & 0x3ff;
				uTemp1 = Inp32(&(NAND(Controller)->rNFMLC8BITPT1));
				uTemp3 =GetBitPosition((uTemp1>>8)&0xff);
				NAND_Inform[Controller].uSLCError8bit_Compare[5]= (uTemp0*8)+uTemp3;
				//		UART_Printf("6th Error => (%4d)byte, (%8x)bit\n", uTemp0, GetBitPosition((uTemp1>>8)&0xff) );
				break;
			case 7:
				uTemp0 = uErrorData0 & 0x3ff;
				uTemp1 = Inp32(&(NAND(Controller)->rNFMLC8BITPT0));
				uTemp3 = GetBitPosition(uTemp1&0xff);
				NAND_Inform[Controller].uSLCError8bit_Compare[0]= (uTemp0*8)+uTemp3;
				//		UART_Printf("1st Error => (%4d)byte, (%8x)bit\n", uTemp0, GetBitPosition(uTemp1&0xff) );

				uTemp0 = (uErrorData0>>15) & 0x3ff;
				uTemp1 = Inp32(&(NAND(Controller)->rNFMLC8BITPT0));
				uTemp3 = GetBitPosition((uTemp1>>8)&0xff);
				NAND_Inform[Controller].uSLCError8bit_Compare[1]= (uTemp0*8)+uTemp3;
				//		UART_Printf("2nd Error => (%4d)byte, (%8x)bit\n", uTemp0,GetBitPosition((uTemp1>>8)&0xff) );

				uTemp0 = uErrorData1 & 0x3ff;
				uTemp1 = Inp32(&(NAND(Controller)->rNFMLC8BITPT0));
				uTemp3 = GetBitPosition((uTemp1>>16)&0xff);
				NAND_Inform[Controller].uSLCError8bit_Compare[2]= (uTemp0*8)+uTemp3;
				//		UART_Printf("3th Error => (%4d)byte, (%8x)bit\n", uTemp0, GetBitPosition((uTemp1>>16)&0xff) );

				uTemp0 = (uErrorData1>>11) & 0x3ff;
				uTemp1 = Inp32(&(NAND(Controller)->rNFMLC8BITPT0));
				uTemp3 = GetBitPosition((uTemp1>>24)&0xff);
				NAND_Inform[Controller].uSLCError8bit_Compare[3]= (uTemp0*8)+uTemp3;
				//		UART_Printf("4th Error => (%4d)byte, (%8x)bit\n", uTemp0, GetBitPosition((uTemp1>>24)&0xff) );

				uTemp0 = (uErrorData1>>22) & 0x3ff;
				uTemp1 = Inp32(&(NAND(Controller)->rNFMLC8BITPT1));
				uTemp3 = GetBitPosition(uTemp1&0xff);
				NAND_Inform[Controller].uSLCError8bit_Compare[4]= (uTemp0*8)+uTemp3;
				//		UART_Printf("5th Error => (%4d)byte, (%8x)bit\n", uTemp0, GetBitPosition(uTemp1&0xff) );

				uTemp0 = uErrorData2 & 0x3ff;
				uTemp1 = Inp32(&(NAND(Controller)->rNFMLC8BITPT1));
				uTemp3 =GetBitPosition((uTemp1>>8)&0xff);
				NAND_Inform[Controller].uSLCError8bit_Compare[5]= (uTemp0*8)+uTemp3;
				//		UART_Printf("6th Error => (%4d)byte, (%8x)bit\n", uTemp0, GetBitPosition((uTemp1>>8)&0xff) );

				uTemp0 = (uErrorData2>>11) & 0x3ff;
				uTemp1 = Inp32(&(NAND(Controller)->rNFMLC8BITPT1));
				uTemp3 = GetBitPosition((uTemp1>>16)&0xff);
				NAND_Inform[Controller].uSLCError8bit_Compare[6]= (uTemp0*8)+uTemp3;
				//		UART_Printf("7th Error => (%4d)byte, (%8x)bit\n", uTemp0, GetBitPosition((uTemp1>>16)&0xff) );
				break;
			case 8:
				uTemp0 = uErrorData0 & 0x3ff;
				uTemp1 = Inp32(&(NAND(Controller)->rNFMLC8BITPT0));
				uTemp3 = GetBitPosition(uTemp1&0xff);
				NAND_Inform[Controller].uSLCError8bit_Compare[0]= (uTemp0*8)+uTemp3;
				//		UART_Printf("1st Error => (%4d)byte, (%8x)bit\n", uTemp0, GetBitPosition(uTemp1&0xff) );

				uTemp0 = (uErrorData0>>15) & 0x3ff;
				uTemp1 = Inp32(&(NAND(Controller)->rNFMLC8BITPT0));
				uTemp3 = GetBitPosition((uTemp1>>8)&0xff);
				NAND_Inform[Controller].uSLCError8bit_Compare[1]= (uTemp0*8)+uTemp3;
				//		UART_Printf("2nd Error => (%4d)byte, (%8x)bit\n", uTemp0,GetBitPosition((uTemp1>>8)&0xff) );

				uTemp0 = uErrorData1 & 0x3ff;
				uTemp1 = Inp32(&(NAND(Controller)->rNFMLC8BITPT0));
				uTemp3 = GetBitPosition((uTemp1>>16)&0xff);
				NAND_Inform[Controller].uSLCError8bit_Compare[2]= (uTemp0*8)+uTemp3;
				//		UART_Printf("3th Error => (%4d)byte, (%8x)bit\n", uTemp0, GetBitPosition((uTemp1>>16)&0xff) );

				uTemp0 = (uErrorData1>>11) & 0x3ff;
				uTemp1 = Inp32(&(NAND(Controller)->rNFMLC8BITPT0));
				uTemp3 = GetBitPosition((uTemp1>>24)&0xff);
				NAND_Inform[Controller].uSLCError8bit_Compare[3]= (uTemp0*8)+uTemp3;
				//		UART_Printf("4th Error => (%4d)byte, (%8x)bit\n", uTemp0, GetBitPosition((uTemp1>>24)&0xff) );

				uTemp0 = (uErrorData1>>22) & 0x3ff;
				uTemp1 = Inp32(&(NAND(Controller)->rNFMLC8BITPT1));
				uTemp3 = GetBitPosition(uTemp1&0xff);
				NAND_Inform[Controller].uSLCError8bit_Compare[4]= (uTemp0*8)+uTemp3;
				//		UART_Printf("5th Error => (%4d)byte, (%8x)bit\n", uTemp0, GetBitPosition(uTemp1&0xff) );

				uTemp0 = uErrorData2 & 0x3ff;
				uTemp1 = Inp32(&(NAND(Controller)->rNFMLC8BITPT1));
				uTemp3 =GetBitPosition((uTemp1>>8)&0xff);
				NAND_Inform[Controller].uSLCError8bit_Compare[5]= (uTemp0*8)+uTemp3;
				//		UART_Printf("6th Error => (%4d)byte, (%8x)bit\n", uTemp0, GetBitPosition((uTemp1>>8)&0xff) );

				uTemp0 = (uErrorData2>>11) & 0x3ff;
				uTemp1 = Inp32(&(NAND(Controller)->rNFMLC8BITPT1));
				uTemp3 = GetBitPosition((uTemp1>>16)&0xff);
				NAND_Inform[Controller].uSLCError8bit_Compare[6]= (uTemp0*8)+uTemp3;
				//		UART_Printf("7th Error => (%4d)byte, (%8x)bit\n", uTemp0, GetBitPosition((uTemp1>>16)&0xff) );

				uTemp0 = (uErrorData2>>22) & 0x3ff;
				uTemp1 = Inp32(&(NAND(Controller)->rNFMLC8BITPT1));
				uTemp3 = GetBitPosition((uTemp1>>24)&0xff);
				NAND_Inform[Controller].uSLCError8bit_Compare[7]= (uTemp0*8)+uTemp3;
				//		UART_Printf("8th Error => (%4d)byte, (%8x)bit\n", uTemp0, GetBitPosition((uTemp1>>24)&0xff) );
				break;
			default:
				break;
		}


		switch ( NAND_Inform[Controller].u8bitECCPattern )
		{
			case 1:
				break;
			case 2:
				for ( uIndex1=0; uIndex1<2; uIndex1++)
				{
					for ( uIndex2=uIndex1+1; uIndex2<2; uIndex2++)
					{
						if ( NAND_Inform[Controller].uSLCError8bit_Compare[uIndex1] > NAND_Inform[Controller].uSLCError8bit_Compare[uIndex2] )
						{
							NAND_Inform[Controller].uSLCError8bit_Temp[0] =  NAND_Inform[Controller].uSLCError8bit_Compare[uIndex1];
							NAND_Inform[Controller].uSLCError8bit_Compare[uIndex1] = NAND_Inform[Controller].uSLCError8bit_Compare[uIndex2];
							NAND_Inform[Controller].uSLCError8bit_Compare[uIndex2] = NAND_Inform[Controller].uSLCError8bit_Temp[0];
						}
					}
				}
				break;
			case 3:
				for ( uIndex1=0; uIndex1<3; uIndex1++)
				{
					for ( uIndex2=uIndex1+1; uIndex2<3; uIndex2++)
					{
						if ( NAND_Inform[Controller].uSLCError8bit_Compare[uIndex1] > NAND_Inform[Controller].uSLCError8bit_Compare[uIndex2] )
						{
							NAND_Inform[Controller].uSLCError8bit_Temp[0] =  NAND_Inform[Controller].uSLCError8bit_Compare[uIndex1];
							NAND_Inform[Controller].uSLCError8bit_Compare[uIndex1] = NAND_Inform[Controller].uSLCError8bit_Compare[uIndex2];
							NAND_Inform[Controller].uSLCError8bit_Compare[uIndex2] = NAND_Inform[Controller].uSLCError8bit_Temp[0];
						}
					}
				}
				break;
			case 4:
				for ( uIndex1=0; uIndex1<4; uIndex1++)
				{
					for ( uIndex2=uIndex1+1; uIndex2<4; uIndex2++)
					{
						if ( NAND_Inform[Controller].uSLCError8bit_Compare[uIndex1] > NAND_Inform[Controller].uSLCError8bit_Compare[uIndex2] )
						{
							NAND_Inform[Controller].uSLCError8bit_Temp[0] =  NAND_Inform[Controller].uSLCError8bit_Compare[uIndex1];
							NAND_Inform[Controller].uSLCError8bit_Compare[uIndex1] = NAND_Inform[Controller].uSLCError8bit_Compare[uIndex2];
							NAND_Inform[Controller].uSLCError8bit_Compare[uIndex2] = NAND_Inform[Controller].uSLCError8bit_Temp[0];
						}
					}
				}
				break;
			case 5:
				for ( uIndex1=0; uIndex1<5; uIndex1++)
				{
					for ( uIndex2=uIndex1+1; uIndex2<5; uIndex2++)
					{
						if ( NAND_Inform[Controller].uSLCError8bit_Compare[uIndex1] > NAND_Inform[Controller].uSLCError8bit_Compare[uIndex2] )
						{
							NAND_Inform[Controller].uSLCError8bit_Temp[0] =  NAND_Inform[Controller].uSLCError8bit_Compare[uIndex1];
							NAND_Inform[Controller].uSLCError8bit_Compare[uIndex1] = NAND_Inform[Controller].uSLCError8bit_Compare[uIndex2];
							NAND_Inform[Controller].uSLCError8bit_Compare[uIndex2] = NAND_Inform[Controller].uSLCError8bit_Temp[0];
						}
					}
				}
				break;
			case 6:
				for ( uIndex1=0; uIndex1<6; uIndex1++)
				{
					for ( uIndex2=uIndex1+1; uIndex2<6; uIndex2++)
					{
						if ( NAND_Inform[Controller].uSLCError8bit_Compare[uIndex1] > NAND_Inform[Controller].uSLCError8bit_Compare[uIndex2] )
						{
							NAND_Inform[Controller].uSLCError8bit_Temp[0] =  NAND_Inform[Controller].uSLCError8bit_Compare[uIndex1];
							NAND_Inform[Controller].uSLCError8bit_Compare[uIndex1] = NAND_Inform[Controller].uSLCError8bit_Compare[uIndex2];
							NAND_Inform[Controller].uSLCError8bit_Compare[uIndex2] = NAND_Inform[Controller].uSLCError8bit_Temp[0];
						}
					}
				}
				break;
			case 7:
				for ( uIndex1=0; uIndex1<7; uIndex1++)
				{
					for ( uIndex2=uIndex1+1; uIndex2<7; uIndex2++)
					{
						if ( NAND_Inform[Controller].uSLCError8bit_Compare[uIndex1] > NAND_Inform[Controller].uSLCError8bit_Compare[uIndex2] )
						{
							NAND_Inform[Controller].uSLCError8bit_Temp[0] =  NAND_Inform[Controller].uSLCError8bit_Compare[uIndex1];
							NAND_Inform[Controller].uSLCError8bit_Compare[uIndex1] = NAND_Inform[Controller].uSLCError8bit_Compare[uIndex2];
							NAND_Inform[Controller].uSLCError8bit_Compare[uIndex2] = NAND_Inform[Controller].uSLCError8bit_Temp[0];
						}
					}
				}
				break;
			case 8:
				for ( uIndex1=0; uIndex1<8; uIndex1++)
				{
					for ( uIndex2=uIndex1+1; uIndex2<8; uIndex2++)
					{
						if ( NAND_Inform[Controller].uSLCError8bit_Compare[uIndex1] > NAND_Inform[Controller].uSLCError8bit_Compare[uIndex2] )
						{
							NAND_Inform[Controller].uSLCError8bit_Temp[0] =  NAND_Inform[Controller].uSLCError8bit_Compare[uIndex1];
							NAND_Inform[Controller].uSLCError8bit_Compare[uIndex1] = NAND_Inform[Controller].uSLCError8bit_Compare[uIndex2];
							NAND_Inform[Controller].uSLCError8bit_Compare[uIndex2] = NAND_Inform[Controller].uSLCError8bit_Temp[0];
						}
					}
				}
				break;
			default :
				break;
				
		}

		switch ( NAND_Inform[Controller].u8bitECCPattern )
		{
			case 1:
				break;
			case 2:
				for ( uIndex1=0; uIndex1<2; uIndex1++)
				{
					for ( uIndex2=uIndex1+1; uIndex2<2; uIndex2++)
					{
						if ( NAND_Inform[Controller].uSLCError8bit_Original[uIndex1] > NAND_Inform[Controller].uSLCError8bit_Original[uIndex2] )
						{
							NAND_Inform[Controller].uSLCError8bit_Temp[0] =  NAND_Inform[Controller].uSLCError8bit_Original[uIndex1];
							NAND_Inform[Controller].uSLCError8bit_Original[uIndex1] = NAND_Inform[Controller].uSLCError8bit_Original[uIndex2];
							NAND_Inform[Controller].uSLCError8bit_Original[uIndex2] = NAND_Inform[Controller].uSLCError8bit_Temp[0];
						}
					}
				}
				break;
			case 3:
				for ( uIndex1=0; uIndex1<3; uIndex1++)
				{
					for ( uIndex2=uIndex1+1; uIndex2<3; uIndex2++)
					{
						if ( NAND_Inform[Controller].uSLCError8bit_Original[uIndex1] > NAND_Inform[Controller].uSLCError8bit_Original[uIndex2] )
						{
							NAND_Inform[Controller].uSLCError8bit_Temp[0] =  NAND_Inform[Controller].uSLCError8bit_Original[uIndex1];
							NAND_Inform[Controller].uSLCError8bit_Original[uIndex1] = NAND_Inform[Controller].uSLCError8bit_Original[uIndex2];
							NAND_Inform[Controller].uSLCError8bit_Original[uIndex2] = NAND_Inform[Controller].uSLCError8bit_Temp[0];
						}
					}
				}
				break;
			case 4:
				for ( uIndex1=0; uIndex1<4; uIndex1++)
				{
					for ( uIndex2=uIndex1+1; uIndex2<4; uIndex2++)
					{
						if ( NAND_Inform[Controller].uSLCError8bit_Original[uIndex1] > NAND_Inform[Controller].uSLCError8bit_Original[uIndex2] )
						{
							NAND_Inform[Controller].uSLCError8bit_Temp[0] =  NAND_Inform[Controller].uSLCError8bit_Original[uIndex1];
							NAND_Inform[Controller].uSLCError8bit_Original[uIndex1] = NAND_Inform[Controller].uSLCError8bit_Original[uIndex2];
							NAND_Inform[Controller].uSLCError8bit_Original[uIndex2] = NAND_Inform[Controller].uSLCError8bit_Temp[0];
						}
					}
				}
				break;
			case 5:
				for ( uIndex1=0; uIndex1<5; uIndex1++)
				{
					for ( uIndex2=uIndex1+1; uIndex2<5; uIndex2++)
					{
						if ( NAND_Inform[Controller].uSLCError8bit_Original[uIndex1] > NAND_Inform[Controller].uSLCError8bit_Original[uIndex2] )
						{
							NAND_Inform[Controller].uSLCError8bit_Temp[0] =  NAND_Inform[Controller].uSLCError8bit_Original[uIndex1];
							NAND_Inform[Controller].uSLCError8bit_Original[uIndex1] = NAND_Inform[Controller].uSLCError8bit_Original[uIndex2];
							NAND_Inform[Controller].uSLCError8bit_Original[uIndex2] = NAND_Inform[Controller].uSLCError8bit_Temp[0];
						}
					}
				}
				break;
			case 6:
				for ( uIndex1=0; uIndex1<6; uIndex1++)
				{
					for ( uIndex2=uIndex1+1; uIndex2<6; uIndex2++)
					{
						if ( NAND_Inform[Controller].uSLCError8bit_Original[uIndex1] > NAND_Inform[Controller].uSLCError8bit_Original[uIndex2] )
						{
							NAND_Inform[Controller].uSLCError8bit_Temp[0] =  NAND_Inform[Controller].uSLCError8bit_Original[uIndex1];
							NAND_Inform[Controller].uSLCError8bit_Original[uIndex1] = NAND_Inform[Controller].uSLCError8bit_Original[uIndex2];
							NAND_Inform[Controller].uSLCError8bit_Original[uIndex2] = NAND_Inform[Controller].uSLCError8bit_Temp[0];
						}
					}
				}
				break;
			case 7:
				for ( uIndex1=0; uIndex1<7; uIndex1++)
				{
					for ( uIndex2=uIndex1+1; uIndex2<7; uIndex2++)
					{
						if ( NAND_Inform[Controller].uSLCError8bit_Original[uIndex1] > NAND_Inform[Controller].uSLCError8bit_Original[uIndex2] )
						{
							NAND_Inform[Controller].uSLCError8bit_Temp[0] =  NAND_Inform[Controller].uSLCError8bit_Original[uIndex1];
							NAND_Inform[Controller].uSLCError8bit_Original[uIndex1] = NAND_Inform[Controller].uSLCError8bit_Original[uIndex2];
							NAND_Inform[Controller].uSLCError8bit_Original[uIndex2] = NAND_Inform[Controller].uSLCError8bit_Temp[0];
						}
					}
				}
				break;
			case 8:
				for ( uIndex1=0; uIndex1<8; uIndex1++)
				{
					for ( uIndex2=uIndex1+1; uIndex2<8; uIndex2++)
					{
						if ( NAND_Inform[Controller].uSLCError8bit_Original[uIndex1] > NAND_Inform[Controller].uSLCError8bit_Original[uIndex2] )
						{
							NAND_Inform[Controller].uSLCError8bit_Temp[0] =  NAND_Inform[Controller].uSLCError8bit_Original[uIndex1];
							NAND_Inform[Controller].uSLCError8bit_Original[uIndex1] = NAND_Inform[Controller].uSLCError8bit_Original[uIndex2];
							NAND_Inform[Controller].uSLCError8bit_Original[uIndex2] = NAND_Inform[Controller].uSLCError8bit_Temp[0];
						}
					}
				}
				break;
			default :
				break;
				
		}

		switch ( NAND_Inform[Controller].u8bitECCPattern )
		{
			case 1:
				for ( uIndex=0; uIndex<1 ; uIndex++)
				{
					if ( NAND_Inform[Controller].uSLCError8bit_Compare[uIndex] == NAND_Inform[Controller].uSLCError8bit_Original[uIndex] )
					{
						UART_Printf("%2d Error Bit Detect is Good\n", uIndex);
						UART_Printf("Origianl Bit = %4d Bit ===== Detect Bit = %4d Bit\n", NAND_Inform[Controller].uSLCError8bit_Original[uIndex], NAND_Inform[Controller].uSLCError8bit_Compare[uIndex]);
					}
					else
					{
						UART_Printf("%2d Error Bit Detect is Bad\n", uIndex);				
						UART_Printf("Origianl Bit = %4d Bit ===== Detect Bit = %4d Bit\n", NAND_Inform[Controller].uSLCError8bit_Original[uIndex], NAND_Inform[Controller].uSLCError8bit_Compare[uIndex]);
						UART_Printf("Check This Condition\n");
						uTemp4=UART_GetIntNum();
					}
				}
				break;
			case 2:
				for ( uIndex=0; uIndex<2 ; uIndex++)
				{
					if ( NAND_Inform[Controller].uSLCError8bit_Compare[uIndex] == NAND_Inform[Controller].uSLCError8bit_Original[uIndex] )
					{
						UART_Printf("%2d Error Bit Detect is Good\n", uIndex);
						UART_Printf("Origianl Bit = %4d Bit ===== Detect Bit = %4d Bit\n", NAND_Inform[Controller].uSLCError8bit_Original[uIndex], NAND_Inform[Controller].uSLCError8bit_Compare[uIndex]);
					}
					else
					{
						UART_Printf("%2d Error Bit Detect is Bad\n", uIndex);				
						UART_Printf("Origianl Bit = %4d Bit ===== Detect Bit = %4d Bit\n", NAND_Inform[Controller].uSLCError8bit_Original[uIndex], NAND_Inform[Controller].uSLCError8bit_Compare[uIndex]);
						UART_Printf("Check This Condition\n");
						uTemp4=UART_GetIntNum();
					}
				}
				break;
			case 3:
				for ( uIndex=0; uIndex<3 ; uIndex++)
				{
					if ( NAND_Inform[Controller].uSLCError8bit_Compare[uIndex] == NAND_Inform[Controller].uSLCError8bit_Original[uIndex] )
					{
						UART_Printf("%2d Error Bit Detect is Good\n", uIndex);
						UART_Printf("Origianl Bit = %4d Bit ===== Detect Bit = %4d Bit\n", NAND_Inform[Controller].uSLCError8bit_Original[uIndex], NAND_Inform[Controller].uSLCError8bit_Compare[uIndex]);
					}
					else
					{
						UART_Printf("%2d Error Bit Detect is Bad\n", uIndex);				
						UART_Printf("Origianl Bit = %4d Bit ===== Detect Bit = %4d Bit\n", NAND_Inform[Controller].uSLCError8bit_Original[uIndex], NAND_Inform[Controller].uSLCError8bit_Compare[uIndex]);
						UART_Printf("Check This Condition\n");
						uTemp4=UART_GetIntNum();
					}
				}
				break;
			case 4:
				for ( uIndex=0; uIndex<4 ; uIndex++)
				{
					if ( NAND_Inform[Controller].uSLCError8bit_Compare[uIndex] == NAND_Inform[Controller].uSLCError8bit_Original[uIndex] )
					{
						UART_Printf("%2d Error Bit Detect is Good\n", uIndex);
						UART_Printf("Origianl Bit = %4d Bit ===== Detect Bit = %4d Bit\n", NAND_Inform[Controller].uSLCError8bit_Original[uIndex], NAND_Inform[Controller].uSLCError8bit_Compare[uIndex]);
					}
					else
					{
						UART_Printf("%2d Error Bit Detect is Bad\n", uIndex);				
						UART_Printf("Origianl Bit = %4d Bit ===== Detect Bit = %4d Bit\n", NAND_Inform[Controller].uSLCError8bit_Original[uIndex], NAND_Inform[Controller].uSLCError8bit_Compare[uIndex]);
						UART_Printf("Check This Condition\n");
						uTemp4=UART_GetIntNum();
					}
				}
				break;
			case 5:
				for ( uIndex=0; uIndex<5 ; uIndex++)
				{
					if ( NAND_Inform[Controller].uSLCError8bit_Compare[uIndex] == NAND_Inform[Controller].uSLCError8bit_Original[uIndex] )
					{
						UART_Printf("%2d Error Bit Detect is Good\n", uIndex);
						UART_Printf("Origianl Bit = %4d Bit ===== Detect Bit = %4d Bit\n", NAND_Inform[Controller].uSLCError8bit_Original[uIndex], NAND_Inform[Controller].uSLCError8bit_Compare[uIndex]);
					}
					else
					{
						UART_Printf("%2d Error Bit Detect is Bad\n", uIndex);				
						UART_Printf("Origianl Bit = %4d Bit ===== Detect Bit = %4d Bit\n", NAND_Inform[Controller].uSLCError8bit_Original[uIndex], NAND_Inform[Controller].uSLCError8bit_Compare[uIndex]);
						UART_Printf("Check This Condition\n");
						uTemp4=UART_GetIntNum();
					}
				}
				break;
			case 6:
				for ( uIndex=0; uIndex<6 ; uIndex++)
				{
					if ( NAND_Inform[Controller].uSLCError8bit_Compare[uIndex] == NAND_Inform[Controller].uSLCError8bit_Original[uIndex] )
					{
						UART_Printf("%2d Error Bit Detect is Good\n", uIndex);
						UART_Printf("Origianl Bit = %4d Bit ===== Detect Bit = %4d Bit\n", NAND_Inform[Controller].uSLCError8bit_Original[uIndex], NAND_Inform[Controller].uSLCError8bit_Compare[uIndex]);
					}
					else
					{
						UART_Printf("%2d Error Bit Detect is Bad\n", uIndex);				
						UART_Printf("Origianl Bit = %4d Bit ===== Detect Bit = %4d Bit\n", NAND_Inform[Controller].uSLCError8bit_Original[uIndex], NAND_Inform[Controller].uSLCError8bit_Compare[uIndex]);
						UART_Printf("Check This Condition\n");
						uTemp4=UART_GetIntNum();
					}
				}
				break;
			case 7:
				for ( uIndex=0; uIndex<7 ; uIndex++)
				{
					if ( NAND_Inform[Controller].uSLCError8bit_Compare[uIndex] == NAND_Inform[Controller].uSLCError8bit_Original[uIndex] )
					{
						UART_Printf("%2d Error Bit Detect is Good\n", uIndex);
						UART_Printf("Origianl Bit = %4d Bit ===== Detect Bit = %4d Bit\n", NAND_Inform[Controller].uSLCError8bit_Original[uIndex], NAND_Inform[Controller].uSLCError8bit_Compare[uIndex]);
					}
					else
					{
						UART_Printf("%2d Error Bit Detect is Bad\n", uIndex);				
						UART_Printf("Origianl Bit = %4d Bit ===== Detect Bit = %4d Bit\n", NAND_Inform[Controller].uSLCError8bit_Original[uIndex], NAND_Inform[Controller].uSLCError8bit_Compare[uIndex]);
						UART_Printf("Check This Condition\n");
						uTemp4=UART_GetIntNum();
					}
				}
				break;
			case 8:
				for ( uIndex=0; uIndex<8 ; uIndex++)
				{
					if ( NAND_Inform[Controller].uSLCError8bit_Compare[uIndex] == NAND_Inform[Controller].uSLCError8bit_Original[uIndex] )
					{
						UART_Printf("%2d Error Bit Detect is Good\n", uIndex);
						UART_Printf("Origianl Bit = %4d Bit ===== Detect Bit = %4d Bit\n", NAND_Inform[Controller].uSLCError8bit_Original[uIndex], NAND_Inform[Controller].uSLCError8bit_Compare[uIndex]);
					}
					else
					{
						UART_Printf("%2d Error Bit Detect is Bad\n", uIndex);				
						UART_Printf("Origianl Bit = %4d Bit ===== Detect Bit = %4d Bit\n", NAND_Inform[Controller].uSLCError8bit_Original[uIndex], NAND_Inform[Controller].uSLCError8bit_Compare[uIndex]);
						UART_Printf("Check This Condition\n");
						uTemp4=UART_GetIntNum();
					}
				}
				break;
			default :
				break;

		}

	}

}

//////////
// Function Name : NAND_ReadPageSLC
// Function Description : This function read the 1 page data of external SLC Type NAND device
// Input : 	Controller - Nand Controller Port Number 
//			uBlock - Read Block
//			uPage - Read Page
//			pBuffer - Read data buffer
// Output : 	ECC Error Type 
NAND_eERROR NAND_ReadPageSLC_BackUp(u32 Controller, u32 uBlock, u32 uPage, u8 *pBuffer, u8 *pSpareBuffer)
{
	u32 i, uBlockPage, uPageSize;
	u32 uMecc0, uSecc;
//	u32 uMecc1,  uVal ;
	NAND_eERROR eError;	
	u32 *pBufferW;
	u32 uTemp;
	u32 uSpareSize, uIndex, uTempMECCSpare;
	
	pBufferW = (u32 *)pBuffer;

	// 01. aNAND_Spare_Data Init. add jungil 0625
	uSpareSize = NAND_Inform[Controller].uSpareSize;	
	for(uIndex=0 ; uIndex<uSpareSize; uIndex++) 
	{
		aNand_Spare_Data[uIndex] = 0x0;
	}  

	// 02. Add. Jungil 0626 NFCONT Setting.
	uTemp = Inp32(&(NAND(Controller)->rNFCONT));
	uTemp &= ~((1<<13)|(1<<12));		// 4bit ECC encoding/decoding completion Interrupt disable
	uTemp |= (1<<10) | (1<<9);		// Illegal Access & RnB Transition Interrupt enable
	Outp32(&(NAND(Controller)->rNFCONT), uTemp);
	uTemp = 0;
	uTemp = Inp32(&(NAND(Controller)->rNFCONF));
	uTemp &= ~(1<<23);		// 4bit ECC encoding/decoding ECC disable
	Outp32(&(NAND(Controller)->rNFCONF), uTemp);

	// 03. Add. jungil 0625	 Interrupt Setting.
	Outp32(&(NAND(Controller)->rNFSTAT), (1<<5)|(1<<4));		// Illegal Access & RnB Interrupt Pending bit clear
	g_Nand_RnBTransition = 0 ;
	INTC_Enable(NUM_NFC);

	// 04. Calculate Page Size Address.
	if(NAND_Inform[Controller].uNandType == NAND_Normal8bit)
	{
		uBlockPage=(uBlock<<5) + uPage;
		uPageSize = NAND_Inform[Controller].uPageSize;
	}
	else //if(NAND_Inform[Controller].uNandType == NAND_Advanced8bit)
	{
		uBlockPage=(uBlock<<6) + uPage;
		uPageSize = NAND_Inform[Controller].uPageSize;
	}

	// 05. Main ECC Unlock
	NF_MECC_UnLock(Controller);
	// 06. Main / Spare ECC Initialize
	NF_RSTECC(Controller);    // Initialize ECC

	// 07. CE Signal is Low. 
	NF_nFCE_L(Controller);
	// 08. Clear RnB Check. 
	NF_CLEAR_RnB(Controller);

	// 08. Command and First Address Setiing. Command 0x00: Read Command. 1st Address: Colume address
	//if(NAND_Inform[Controller].uNandType == NAND_Normal8bit)
	if(NAND_Inform[Controller].uAddrCycle == 4)
	{
		NF_CMD(Controller, 0x00);	// Read command
		NF_ADDR(Controller, 0); 	
	}
	else if(NAND_Inform[Controller].uAddrCycle == 5)
	{
		NF_CMD(Controller, 0x00);	// 1st Read command
		NF_ADDR(Controller, 0); 	
		NF_ADDR(Controller, 0); 
	}

	// 09. Another Address setting. BlockPage address Setting.
	NF_ADDR(Controller, uBlockPage&0xff);		//
	NF_ADDR(Controller, (uBlockPage>>8)&0xff);	// Block & Page num.
	NF_ADDR(Controller, (uBlockPage>>16)&0xff);	//

	if(NAND_Inform[Controller].uNandType == NAND_Advanced8bit)
		NF_CMD(Controller, 0x30);	// 2'nd command


	// 10. Add. jungil 0625 RnB Siganl  Wait
	while(!g_Nand_RnBTransition);
	//NF_DETECT_RnB(Controller);

	// 11. Reading Data : Page Size Data
	#if NAND_TRANSFER_MODE == POLLING_TRANSFER
	#if 0
		for(i=0 ; i<uPageSize ; i++) 
		{
			*pBuffer++ = NF_RDDATA8(Controller);	// Read one page
		}
	#else
		for(i=0 ; i<uPageSize/4 ; i++) 
		{
			*pBufferW++ = NF_RDDATA(Controller);	// Read one page
		}
	#endif
	#elif NAND_TRANSFER_MODE == DMA_TRANSFER
		Nand_DmaDone = 0;
//		DMACH_Setup(DMA_A, 0x0, (u32)(&(NAND(Controller)->rNFDATA)), 1, (u32)pBufferW, 0, WORD, uPageSize/4, DEMAND, DMA1_NAND_TX, MEM, BURST128, &g_oNandDmac1);
			
//		DMACH_Start(&g_oNandDmac1);
		while(!Nand_DmaDone);	
	#endif

	// 12. Main ECC Lock.
	NF_MECC_Lock(Controller);

	if(NAND_Inform[Controller].uNandType == NAND_Normal8bit)
	{
	// 13. Spare ECC Unlock.
		NF_SECC_UnLock(Controller);

	// 14. Read Main ECC Data in Spare Area [0]~[3] byte
		for(i=0; i<4 ; i++)
		{
			aNand_Spare_Data[i] = NF_RDDATA8(Controller);
		}
	// 15. Spare ECC Lock
		NF_SECC_Lock(Controller);
	// 16. Setting. Main ECC Data to ECCD0~1
		uTempMECCSpare = ((u32)aNand_Spare_Data[3]<<24)|
						   ((u32)aNand_Spare_Data[2]<<16)|
   						   ((u32)aNand_Spare_Data[1]<<8)|
						   ((u32)aNand_Spare_Data[0]);		
		Outp32(&NAND(Controller)->rNFMECCD0, ((uTempMECCSpare&0xff00)<<8)|(uTempMECCSpare&0xff) );
		Outp32(&NAND(Controller)->rNFMECCD1, ((uTempMECCSpare&0xff000000)>>8)|((uTempMECCSpare&0xff0000)>>16) );
	}
	else if(NAND_Inform[Controller].uNandType == NAND_Advanced8bit)
	{	
		aNand_Spare_Data[0] = NF_RDDATA8(Controller);		// Invalid Block Check data

		NF_SECC_UnLock(Controller);
		
		// Main Area ECC Check
		uMecc0 = NF_RDDATA(Controller);

		if(NAND_Inform[Controller].uSpareECCtest == 0)
		{
			Outp32(&NAND(Controller)->rNFMECCD0, ((uMecc0&0xff00)<<8)|(uMecc0&0xff) );
			Outp32(&NAND(Controller)->rNFMECCD1, ((uMecc0&0xff000000)>>8)|((uMecc0&0xff0000)>>16) );
		}
		
		NF_SECC_Lock(Controller);

		aNand_Spare_Data[1] = (u8)(uMecc0&0xFF);
		aNand_Spare_Data[2] = (u8)((uMecc0&0xFF00)>>8);
		aNand_Spare_Data[3] = (u8)((uMecc0&0xFF0000)>>16);
		aNand_Spare_Data[4] = (u8)((uMecc0&0xFF000000)>>24);
	}

	// 17. Another Spare ECC Data Reading. [4]~[15] Byte
	for(i=4; i<NAND_Inform[Controller].uSpareSize ; i++)
	{
		aNand_Spare_Data[i] = NF_RDDATA8(Controller);
	}

	// 18. Back Up Spare Data
	for(i=0; i<NAND_Inform[Controller].uSpareSize ; i++)
		*pSpareBuffer++ = aNand_Spare_Data[i];

	// 19. Save Spare ECC Data [8],[9] Block. 
	uSecc = (aNand_Spare_Data[9]<<8) | (aNand_Spare_Data[8]);

	// 20. Setting Spare ECC Data in NFSECCD Register
	Outp32(&NAND(Controller)->rNFSECCD, ((uSecc&0xff00)<<8)|(uSecc&0xff) );

	// 21. CE signal is High		
	NF_nFCE_H(Controller); 

	// 22. Check ECC Error 
	eError = NAND_CheckECCError(Controller);

	return eError;
}

 NAND_eERROR NAND_ReadPageSLC_4BitSimple(u32 Controller, u32 uBlock, u32 uPage, u8 *pBuffer, u8 *pSpareBuffer)
 {
 	u32 i, uBlockPage, uPageSize;
//	u32 uMecc0, uSecc, uMecc1, uMecc2, uMecc3;
	NAND_eERROR eError;	
	u32 *pBufferW;
//	u32 uTemp, uIndex, uVal, uTempMECCSpare;
	u32 uSpareSize;

	NAND_Reset(Controller);
	
	NAND_pBase[Controller] = (void *)NFCON_BASE;

//	*(unsigned volatile *)0x80200000=(1<<30)|(0x1<<23)|(0x7<<12)|(0x7<<8)|(0x7<<4);
	*(unsigned volatile *)0x80200000=(0<<30)|(0x2<<23)|(0x0<<12)|(0x1<<8)|(0x0<<4);	
	*(unsigned volatile *)0x80200004=(1<<7)|(1<<6)|(1<<5)|(1<<4)|(1<<2)|(1<<1)|(1<<0);

	
	pBufferW = (u32 *)pBuffer;

	// 01. aNAND_Spare_Data Init. add jungil 0625
	uSpareSize = 16;	


	uBlockPage=(uBlock<<5) + uPage;
	uPageSize = 512;



//	NF_MECC_Lock(Controller);
	NF_RSTECC(Controller);    // Initialize ECC
	NF_MECC_UnLock(Controller);


	NF_nFCE_L(Controller);
	NF_CLEAR_RnB(Controller);

	NF_CMD(Controller, 0x00);	// Read command
	NF_ADDR(Controller, 0); 	
	NF_ADDR(Controller, uBlockPage&0xff);		//
	NF_ADDR(Controller, (uBlockPage>>8)&0xff);	// Block & Page num.
	NF_ADDR(Controller, (uBlockPage>>16)&0xff);	//


	NF_DETECT_RnB(Controller);

	for(i=0 ; i<uPageSize ; i++) 
	{
		*pBuffer++ = NF_RDDATA8(Controller);	// Read one page
	}

	for(i=0; i<7; i++)
	{
		aNand_Spare_Data[i] = NF_RDDATA8(Controller);
	}
	
	
	// Check decoding done 
	while(!(NAND(Controller)->rNFSTAT&(1<<6)));
	*(volatile unsigned *)0x80200028 |= (1<<6);
	NF_MECC_Lock(Controller);

	NF_nFCE_H(Controller); 

//	eError = NAND_CheckECCError_8Bit(Controller);

	return eError;


 }


NAND_eERROR NAND_WritePageSLC_8BitValid(u32 Controller, u32 uBlock, u32 uPage, u8 *pBuffer, u8 *pSpareBuffer)
 {
 	u32 i, uBlockPage, uPageSize;
	u32 uMecc0, uMecc1, uMecc2, uMecc3, uStatus;
//	u32 uSecc, uTemp0, uIndex, uVal, uTempMECCSpare, uTemp;
	NAND_eERROR eError;	
	u32 *pBufferW;
	u8 *pBufferW8;
	u32 *pSpareBufferW;
	u32 uTemp0;
	u32 uSpareSize;
	u32 uOption0;
	u32 uNFCONTreg, uNFCONFreg;
	
	pBufferW = (u32 *)pBuffer;
	pSpareBufferW = (u32 *)pSpareBuffer;

	uSpareSize = 16;	
	uBlockPage=(uBlock<<5) + uPage;
	uPageSize = 512;

	// Setting NFCONF Register.
	uNFCONFreg = Inp32(&(NAND(Controller)->rNFCONF));
//	uNFCONFreg = ( uNFCONFreg & (~(0x1<<30)) ) | (0x1<<30) ; // 0: HCLK is more than 66Mhz. 1: HCLK is less than 66Mhz
	uNFCONFreg = ( uNFCONFreg & (~(0x1<<30)) ) ; // 0: HCLK is more than 66Mhz. 1: HCLK is less than 66Mhz	
	uNFCONFreg = ( uNFCONFreg & (~(0x3<<23)) ) | (0x1<<23) ; // 8bit ECC Select.
	Outp32(&(NAND(Controller)->rNFCONF), uNFCONFreg ); // NFCONF Register setting. 
	// Setting NFCONT Register.
	uNFCONTreg = Inp32(&(NAND(Controller)->rNFCONT)); // read NFCON
	uNFCONTreg = ( uNFCONTreg &~(0x1<<18) ) | (0x1<<18); // 8bit ecc Decoding enable. 
	uNFCONTreg &= ~((1<<13)|(1<<12));		// 4bit ECC encoding/decoding completion Interrupt disable
//	uNFCONTreg = ( uNFCONTreg & ~(0x1<<12)) | (0x1<<12);	// 4bit ECC decoding completion Interrupt Enable.
//	uNFCONTreg = ( uNFCONTreg & ~(0x1<<13)) | (0x1<<13);	// 4bit ECC Encoding completion Interrupt Enable.

//	uNFCONTreg |= ( (1<<10) | (1<<9) );		// Illegal Access & RnB Transition Interrupt enable
	uNFCONTreg |= ( (1<< 9) );		// Illegal Access & RnB Transition Interrupt enable
	
	Outp32(&(NAND(Controller)->rNFCONT), uNFCONTreg); // write setting. NFCONT Register

	Outp32(&(NAND(Controller)->rNFSTAT), (1<<5)|(1<<4)); // Illegal Access & RnB Interrupt Pending bit clear

	#if 0 // Timing max setting. 7, 7, 7 
	uTemp0 = Inp32(&(NAND(Controller)->rNFCONF));
	uTemp0 = uTemp0 | ((7<<12)|(7<<8)|(7<<4));
	Outp32(&(NAND(Controller)->rNFCONF), uTemp0);	
	#endif

	#if 0 // Interupt Enable.
	INTC_Unmask(NUM_NFC);
	#endif	

	// 3 Address Cycle Setting 
	#if 0 // adress setting test.
	UART_Printf("----------------------------------------------------\n");
	UART_Printf(" 0: 3 Address Cycle , 1: 4 Address Cycle                                \n");
	uOption0 = UART_GetIntNum();
	UART_Printf("----------------------------------------------------\n");
	#else
	uOption0 = 0;
	#endif
	
	NF_MECC_Lock(Controller); // Main ECC Lock
	NF_nFCE_L(Controller); // CE is Low


	NF_CMD(Controller, 0x80);	// Read command
	NF_ADDR(Controller, 0); 	// Address setting. 
	NF_ADDR(Controller, uBlockPage&0xff);		//
	NF_ADDR(Controller, (uBlockPage>>8)&0xff);	// Block & Page num.
	if (uOption0 == 1) NF_ADDR(Controller, (uBlockPage>>16)&0xff);	//

	NF_MECC_UnLock(Controller);
	NF_RSTECC(Controller);    // Initialize ECC	

	for(i=0 ; i<uPageSize ; i++) 
	{
		NF_WRDATA8(Controller, *pBuffer++);
	}

	#if 0 // spec out encoding done. .. . .  
	while(!(NAND(Controller)->rNFSTAT&(1<<7))); // wait encoding done
	*(unsigned volatile *)0x70200028 |= (1<<7); // encoding done clear
	*(unsigned volatile *)0x70200004=(*(unsigned volatile *)0x70200004)|(1<<11); // MLS Stop
	#endif
	
	NF_MECC_Lock(Controller);
	
	if ( NAND_Inform[Controller].uECCtest == 1)
	{
		for ( i=0; i<13; i++)
		{
			aNand_Spare_Data[i] = *pSpareBuffer++;
		}

		for (i=0 ; i <13 ; i++)
		{
			NF_WRDATA8(Controller, aNand_Spare_Data[i]);
		}

		NF_CLEAR_RnB(Controller);
		NF_CMD(Controller, 0x10);	// Read command
		#if 0 // Interupt Using. 
		while(!g_Nand_RnBTransition)
		{
			if(g_Nand_IllegalAccError == 1)
			{
				NAND_Reset(Controller);
				g_Nand_IllegalAccError = 0;
				return eNAND_ProgramError;
			}
		}
		#else
		NF_DETECT_RnB(Controller);
		#endif
	}
	else
	{
		uMecc0 = Inp32(&NAND(Controller)->rNFM8ECC0);
		uMecc1 = Inp32(&NAND(Controller)->rNFM8ECC1); 
		uMecc2 = Inp32(&NAND(Controller)->rNFM8ECC2);
		uMecc3 = Inp32(&NAND(Controller)->rNFM8ECC3);
		aNand_Spare_Data[0] = (u8)(uMecc0&0xFF);
		aNand_Spare_Data[1] = (u8)((uMecc0&0xFF00)>>8);
		aNand_Spare_Data[2] = (u8)((uMecc0&0xFF0000)>>16);
		aNand_Spare_Data[3] = (u8)((uMecc0&0xFF000000)>>24);
		aNand_Spare_Data[4] = (u8)(uMecc1&0xFF);
		aNand_Spare_Data[5] = (u8)((uMecc1&0xFF00)>>8);
		aNand_Spare_Data[6] = (u8)((uMecc1&0xFF0000)>>16);
		aNand_Spare_Data[7] = (u8)((uMecc1&0xFF000000)>>24);
		aNand_Spare_Data[8] = (u8)(uMecc2&0xFF);
		aNand_Spare_Data[9] = (u8)((uMecc2&0xFF00)>>8);
		aNand_Spare_Data[10] = (u8)((uMecc2&0xFF0000)>>16);
		aNand_Spare_Data[11] = (u8)((uMecc2&0xFF000000)>>24);
		aNand_Spare_Data[12] = (u8)(uMecc3&0xFF);
		aNand_Spare_Data[13] = 0xff;
		aNand_Spare_Data[14] = 0xff;
		aNand_Spare_Data[15] = 0xff;

		for (i=0 ; i <13 ; i++)
		{
			NF_WRDATA8(Controller, aNand_Spare_Data[i]);
		}

	#if 0 
		for(i=13; i<16 ; i++)
		{
			NF_WRDATA8(Controller, aNand_Spare_Data[i]);
		}
	#endif		

		NF_CLEAR_RnB(Controller);
		NF_CMD(Controller, 0x10);	// Read command
		#if 0 // Interupt Using. 
		while(!g_Nand_RnBTransition)
		{
			if(g_Nand_IllegalAccError == 1)
			{
				NAND_Reset(Controller);
				g_Nand_IllegalAccError = 0;
				return eNAND_ProgramError;
			}
		}
		#else
		NF_DETECT_RnB(Controller);
		#endif
	}

	NF_CMD(Controller, 0x70);

	while(!((uStatus = NF_RDDATA8(Controller)) & (1<<6))); 	// Check Operation Status Check.

	if(uStatus & 0x01)
		eError = eNAND_ProgramError;		// Error in Program
	else
		eError = eNAND_NoError;
	
	NF_nFCE_H(Controller); 

	if ( NAND_Inform[Controller].uECCtest != 1)
	{
		for ( i = 0 ; i<16 ; i++)
		{
			*pSpareBuffer++ = aNand_Spare_Data[i];

		}
	}

	INTC_Disable(NUM_NFC);
	
	return eError;
 }


NAND_eERROR NAND_Write2048Page_8BitValid(u32 Controller, u32 uBlock, u32 uPage, u8 *pBuffer, u8 *pSpareBuffer)
 {
 	u32 i, uBlockPage, uPageSize;
	u32 uMecc0,uMecc1, uMecc2, uMecc3, uStatus;
//	u32 uSecc,  uTemp, uVal, uTempMECCSpare, uIndex2;
	NAND_eERROR eError;	
	u32 *pBufferW;
	u8 *pBufferW8;
	u32 *pSpareBufferW;
	u32 uTemp0;
	u32 uSpareSize, uIndex0, uIndex1 ;
	u32 uOption1;
	
	pBufferW = (u32 *)pBuffer;
	pSpareBufferW = (u32 *)pSpareBuffer;

	#if 0 // Interupt Enable.
	g_Nand_RnBTransition = 0;
	#endif	

	uSpareSize = 64;	
	uBlockPage=(uBlock<<6) + uPage;
	uPageSize = 2048;

	*(unsigned volatile *)0x70200000=((*(unsigned volatile *)0x70200000)&~(0x3<<23))|(1<<30)|(1<<23); // Clock setting. and 8bit sellect.
	*(unsigned volatile *)0x70200004=(*(unsigned volatile *)0x70200004)|(0x1<<18)|(1<<10)|(1<<9); // Illegal and RnB Interrupt Enable.
	*(unsigned volatile *)0x70200028=(*(unsigned volatile *)0x70200028)|(1<<4); // RnB Pending Clear

	uTemp0 = Inp32(&(NAND(Controller)->rNFCONF));
	uTemp0 = uTemp0 | ((7<<12)|(7<<8)|(7<<4));
	Outp32(&(NAND(Controller)->rNFCONF), uTemp0);	

#if 0 // Interupt Enable.
	INTC_Unmask(NUM_NFC);
#endif	

	#if 1 
	UART_Printf("Address Setting = 4: Address 4 /// 5: Address 5 ");
	uOption1 = UART_GetIntNum();
	NAND_Inform[Controller].uAddrCycle = uOption1;
	#else
	NAND_Inform[Controller].uAddrCycle = 4;
	#endif
	
	//////////////////////////////////////////////////////////////////////
	//                                               0~2048Byte Write                                           //
	//////////////////////////////////////////////////////////////////////  
	
	NF_MECC_Lock(Controller); // Main ECC Lock
	NF_nFCE_L(Controller); // CE is Low

//	if(NAND_Inform[Controller].uAddrCycle == 4)
//	{
//		NF_CMD(Controller, 0x80);	// Write command
//		NF_ADDR(Controller, 0); 	
//	}
//	else if(NAND_Inform[Controller].uAddrCycle == 5)
//	{
		NF_CMD(Controller, 0x80);	// 1st Write command
		NF_ADDR(Controller, 0); 	
		NF_ADDR(Controller, 0); 
//	}
 
	NF_ADDR(Controller, uBlockPage&0xff);		
	NF_ADDR(Controller, (uBlockPage>>8)&0xff);	// Block & Page num.
	if(NAND_Inform[Controller].uAddrCycle == 5) NF_ADDR(Controller, (uBlockPage>>16)&0xff);	//

	for ( uIndex0 = 0 ; uIndex0<4; uIndex0++)
	{
		NF_MECC_UnLock(Controller);
		NF_RSTECC(Controller);    // Initialize ECC	

		for(i=0 ; i<512 ; i++) 
		{
			NF_WRDATA8(Controller, *pBuffer++);
		}

		while(!(NAND(Controller)->rNFSTAT&(1<<7))); // wait encoding done
		*(unsigned volatile *)0x70200028 |= (1<<7); // encoding done clear
		*(unsigned volatile *)0x70200004=(*(unsigned volatile *)0x70200004)|(1<<11); // MLS Stop

		NF_MECC_Lock(Controller);
		
		if ( NAND_Inform[Controller].uECCtest == 1)
		{
			for ( i=(13*uIndex0) ; i<(13+(13*uIndex0)) ; i++)
			{
				aNand_Spare_Data[i] = *pSpareBuffer++;
			}
		}
		else
		{
			uMecc0 = Inp32(&NAND(Controller)->rNFM8ECC0);
			uMecc1 = Inp32(&NAND(Controller)->rNFM8ECC1); 
			uMecc2 = Inp32(&NAND(Controller)->rNFM8ECC2);
			uMecc3 = Inp32(&NAND(Controller)->rNFM8ECC3);

			uIndex1 = (13*uIndex0);
			aNand_Spare_Data[uIndex1+0] = (u8)(uMecc0&0xFF);
			aNand_Spare_Data[uIndex1+1] = (u8)((uMecc0&0xFF00)>>8);
			aNand_Spare_Data[uIndex1+2] = (u8)((uMecc0&0xFF0000)>>16);
			aNand_Spare_Data[uIndex1+3] = (u8)((uMecc0&0xFF000000)>>24);
			aNand_Spare_Data[uIndex1+4] = (u8)(uMecc1&0xFF);
			aNand_Spare_Data[uIndex1+5] = (u8)((uMecc1&0xFF00)>>8);
			aNand_Spare_Data[uIndex1+6] = (u8)((uMecc1&0xFF0000)>>16);
			aNand_Spare_Data[uIndex1+7] = (u8)((uMecc1&0xFF000000)>>24);
			aNand_Spare_Data[uIndex1+8] = (u8)(uMecc2&0xFF);
			aNand_Spare_Data[uIndex1+9] = (u8)((uMecc2&0xFF00)>>8);
			aNand_Spare_Data[uIndex1+10] = (u8)((uMecc2&0xFF0000)>>16);
			aNand_Spare_Data[uIndex1+11] = (u8)((uMecc2&0xFF000000)>>24);
			aNand_Spare_Data[uIndex1+12] = (u8)(uMecc3&0xFF);
		}

	}

	
	//////////////////////////////////////////////////////////////////////
	//                                             Spare Area Write                                             //
	//////////////////////////////////////////////////////////////////////  

	for(i=0 ; i<64 ; i++)
//		NF_WRDATA8(Controller, aNand_Spare_Data[i]);
		NF_WRDATA8(Controller, (0xff)); // Linux Test Spare Data All FF Setting. add. jungil 080403	

	NF_CMD(Controller, 0x10);	// 2nd Write Command

	NF_DETECT_RnB(Controller);

	//Read Program Status
	NF_CMD(Controller, 0x70);

	while(!((uStatus = NF_RDDATA8(Controller)) & (1<<6)));
	// Write Operation Status Check.
	if(uStatus & 0x01)
		eError = eNAND_ProgramError;		// Error in Program
	else
		eError = eNAND_NoError;
	
	NF_nFCE_H(Controller); 

	//INTC_Mask(NUM_NFC); // FPGA
	INTC_Disable(NUM_NFC);
	
	if ( NAND_Inform[Controller].uECCtest != 1)
	{
		for ( i = 0 ; i<64 ; i++)
		{
			*pSpareBuffer++ = aNand_Spare_Data[i];

		}
	}

	return eError;
 }



NAND_eERROR NAND_Write4096Page_8BitValid(u32 Controller, u32 uBlock, u32 uPage, u8 *pBuffer, u8 *pSpareBuffer)
{
 	u32 i, uBlockPage, uPageSize;
	u32 uMecc0, uMecc1, uMecc2, uMecc3, uStatus;
//	u32 uSecc, uTemp, uVal, uTempMECCSpare, uIndex2 ;
	NAND_eERROR eError;	
	u32 *pBufferW;
	u8 *pBufferW8;
	u32 *pSpareBufferW;
	u32 uTemp0;
	u32 uSpareSize, uIndex0,  uIndex1;
	
	pBufferW = (u32 *)pBuffer;
	pSpareBufferW = (u32 *)pSpareBuffer;

	#if 0 // Interupt Enable.
	g_Nand_RnBTransition = 0;
	#endif	

	uSpareSize = 128;	
	uBlockPage=(uBlock<<7) + uPage;
	uPageSize = 4096;

	*(unsigned volatile *)0x70200000=((*(unsigned volatile *)0x70200000)&~(0x3<<23))|(1<<30)|(1<<23); // Clock setting. and 8bit sellect.
	*(unsigned volatile *)0x70200004=(*(unsigned volatile *)0x70200004)|(0x1<<18)|(1<<10)|(1<<9); // Illegal and RnB Interrupt Enable.
	*(unsigned volatile *)0x70200028=(*(unsigned volatile *)0x70200028)|(1<<4); // RnB Pending Clear

	uTemp0 = Inp32(&(NAND(Controller)->rNFCONF));
	uTemp0 = uTemp0 | ((7<<12)|(7<<8)|(7<<4));
	Outp32(&(NAND(Controller)->rNFCONF), uTemp0);	

#if 0 // Interupt Enable.
	INTC_Unmask(NUM_NFC);
#endif	

	//////////////////////////////////////////////////////////////////////
	//                                               0~2048Byte Write                                           //
	//////////////////////////////////////////////////////////////////////  
	
	NF_MECC_Lock(Controller); // Main ECC Lock
	NF_nFCE_L(Controller); // CE is Low

	if(NAND_Inform[Controller].uAddrCycle == 4)
	{
		NF_CMD(Controller, 0x80);	// Write command
		NF_ADDR(Controller, 0); 	
	}
	else if(NAND_Inform[Controller].uAddrCycle == 5)
	{
		NF_CMD(Controller, 0x80);	// 1st Write command
		NF_ADDR(Controller, 0); 	
		NF_ADDR(Controller, 0); 
	}
 
	NF_ADDR(Controller, uBlockPage&0xff);		
	NF_ADDR(Controller, (uBlockPage>>8)&0xff);	// Block & Page num.
	NF_ADDR(Controller, (uBlockPage>>16)&0xff);	//

	for ( uIndex0 = 0 ; uIndex0<8; uIndex0++)
	{
		NF_MECC_UnLock(Controller);
		NF_RSTECC(Controller);    // Initialize ECC	

		for(i=0 ; i<512 ; i++) 
		{
			NF_WRDATA8(Controller, *pBuffer++);
		}

		while(!(NAND(Controller)->rNFSTAT&(1<<7))); // wait encoding done
		*(unsigned volatile *)0x70200028 |= (1<<7); // encoding done clear
		*(unsigned volatile *)0x70200004=(*(unsigned volatile *)0x70200004)|(1<<11); // MLS Stop

		NF_MECC_Lock(Controller);
		
		if ( NAND_Inform[Controller].uECCtest == 1)
		{
			for ( i=(13*uIndex0) ; i<(13+(13*uIndex0)) ; i++)
			{
				aNand_Spare_Data[i] = *pSpareBuffer++;
			}
		}
		else
		{
			uMecc0 = Inp32(&NAND(Controller)->rNFM8ECC0);
			uMecc1 = Inp32(&NAND(Controller)->rNFM8ECC1); 
			uMecc2 = Inp32(&NAND(Controller)->rNFM8ECC2);
			uMecc3 = Inp32(&NAND(Controller)->rNFM8ECC3);

			uIndex1 = (13*uIndex0);
			aNand_Spare_Data[uIndex1+0] = (u8)(uMecc0&0xFF);
			aNand_Spare_Data[uIndex1+1] = (u8)((uMecc0&0xFF00)>>8);
			aNand_Spare_Data[uIndex1+2] = (u8)((uMecc0&0xFF0000)>>16);
			aNand_Spare_Data[uIndex1+3] = (u8)((uMecc0&0xFF000000)>>24);
			aNand_Spare_Data[uIndex1+4] = (u8)(uMecc1&0xFF);
			aNand_Spare_Data[uIndex1+5] = (u8)((uMecc1&0xFF00)>>8);
			aNand_Spare_Data[uIndex1+6] = (u8)((uMecc1&0xFF0000)>>16);
			aNand_Spare_Data[uIndex1+7] = (u8)((uMecc1&0xFF000000)>>24);
			aNand_Spare_Data[uIndex1+8] = (u8)(uMecc2&0xFF);
			aNand_Spare_Data[uIndex1+9] = (u8)((uMecc2&0xFF00)>>8);
			aNand_Spare_Data[uIndex1+10] = (u8)((uMecc2&0xFF0000)>>16);
			aNand_Spare_Data[uIndex1+11] = (u8)((uMecc2&0xFF000000)>>24);
			aNand_Spare_Data[uIndex1+12] = (u8)(uMecc3&0xFF);
		}

	}

	
	//////////////////////////////////////////////////////////////////////
	//                                             Spare Area Write                                             //
	//////////////////////////////////////////////////////////////////////  

	for(i=0 ; i<128 ; i++)
		NF_WRDATA8(Controller, aNand_Spare_Data[i]);

	NF_CMD(Controller, 0x10);	// 2nd Write Command

	NF_DETECT_RnB(Controller);

	//Read Program Status
	NF_CMD(Controller, 0x70);

	while(!((uStatus = NF_RDDATA8(Controller)) & (1<<6)));
	// Write Operation Status Check.
	if(uStatus & 0x01)
		eError = eNAND_ProgramError;		// Error in Program
	else
		eError = eNAND_NoError;
	
	NF_nFCE_H(Controller); 

	//INTC_Mask(NUM_NFC); // FPGA
	INTC_Disable(NUM_NFC);

	if ( NAND_Inform[Controller].uECCtest != 1)
	{
		for ( i = 0 ; i<128 ; i++)
		{
			*pSpareBuffer++ = aNand_Spare_Data[i];

		}
	}

	return eError;
 }




NAND_eERROR NAND_WritePageSLC_8BitValid_24Byte(u32 Controller, u32 uBlock, u32 uPage, u8 *pBuffer, u8 *pSpareBuffer)
 {
 	u32 i, uBlockPage, uPageSize;
	u32 uMecc0, uMecc1, uMecc2, uMecc3, uStatus;
//	u32 uSecc, uTemp, uIndex,  uVal, uTempMECCSpare;
	NAND_eERROR eError;	
	u32 *pBufferW;
	u8 *pBufferW8;
	u32 *pSpareBufferW;
	u32 uTemp, uTemp0;
	u32 uSpareSize, uIndex, uVal, uTempMECCSpare;
	
	pBufferW = (u32 *)pBuffer;
	pSpareBufferW = (u32 *)pSpareBuffer;

#if 1 // Interupt Enable.
	g_Nand_RnBTransition = 0;
#endif	

	uSpareSize = 16;	
	uBlockPage=(uBlock<<5) + uPage;
	uPageSize = 512;

	*(unsigned volatile *)0x80200000=((*(unsigned volatile *)0x80200000)&~(0x3<<23))|(1<<30)|(1<<25)|(1<<23); // Clock setting. and 8bit sellect.
	*(unsigned volatile *)0x80200004=(*(unsigned volatile *)0x80200004)|(0x1<<18)|(1<<10)|(1<<9); // Illegal and RnB Interrupt Enable.
	*(unsigned volatile *)0x80200028=(*(unsigned volatile *)0x80200028)|(1<<4); // RnB Pending Clear

#if 1 // Timing max setting. 7, 7, 7 
	uTemp0 = Inp32(&(NAND(Controller)->rNFCONF));
	uTemp0 = uTemp0 | ((7<<12)|(7<<8)|(7<<4));
	Outp32(&(NAND(Controller)->rNFCONF), uTemp0);	
#endif

#if 1 // Interupt Enable.
	INTC_Disable(NUM_NFC);
#endif	
	
	NF_MECC_Lock(Controller); // Main ECC Lock
	NF_nFCE_L(Controller); // CE is Low

	NF_CMD(Controller, 0x80);	// Read command
	NF_ADDR(Controller, 0); 	// Address setting. 
	NF_ADDR(Controller, uBlockPage&0xff);		//
	NF_ADDR(Controller, (uBlockPage>>8)&0xff);	// Block & Page num.
	NF_ADDR(Controller, (uBlockPage>>16)&0xff);	//

	NF_MECC_UnLock(Controller);
	NF_RSTECC(Controller);    // Initialize ECC	

	for(i=0 ; i<24 ; i++) 
	{
		NF_WRDATA8(Controller, *pBuffer++);
	}

	while(!(NAND(Controller)->rNFSTAT&(1<<7))); // wait encoding done
	*(unsigned volatile *)0x80200028 |= (1<<7); // encoding done clear
	*(unsigned volatile *)0x80200004=(*(unsigned volatile *)0x80200004)|(1<<11); // MLS Stop

	NF_MECC_Lock(Controller);
	
	if ( NAND_Inform[Controller].uECCtest == 1)
	{

		for(i=24 ; i<512 ; i++) 
		{
			NF_WRDATA8(Controller, *pBuffer++);
		}

		for ( i=0; i<13; i++)
		{
			aNand_Spare_Data[i] = *pSpareBuffer++;
		}

		for (i=0 ; i <13 ; i++)
		{
			NF_WRDATA8(Controller, aNand_Spare_Data[i]);
		}

		NF_CLEAR_RnB(Controller);
		NF_CMD(Controller, 0x10);	// Read command
	#if 1 // Interupt Using. 
		while(!g_Nand_RnBTransition)
		{
			if(g_Nand_IllegalAccError == 1)
			{
				NAND_Reset(Controller);
				g_Nand_IllegalAccError = 0;
				return eNAND_ProgramError;
			}
		}
	#else
		NF_DETECT_RnB(Controller);
	#endif
	}
	else
	{
		uMecc0 = Inp32(&NAND(Controller)->rNFM8ECC0);
		uMecc1 = Inp32(&NAND(Controller)->rNFM8ECC1); 
		uMecc2 = Inp32(&NAND(Controller)->rNFM8ECC2);
		uMecc3 = Inp32(&NAND(Controller)->rNFM8ECC3);
		aNand_Spare_Data[0] = (u8)(uMecc0&0xFF);
		aNand_Spare_Data[1] = (u8)((uMecc0&0xFF00)>>8);
		aNand_Spare_Data[2] = (u8)((uMecc0&0xFF0000)>>16);
		aNand_Spare_Data[3] = (u8)((uMecc0&0xFF000000)>>24);
		aNand_Spare_Data[4] = (u8)(uMecc1&0xFF);
		aNand_Spare_Data[5] = (u8)((uMecc1&0xFF00)>>8);
		aNand_Spare_Data[6] = (u8)((uMecc1&0xFF0000)>>16);
		aNand_Spare_Data[7] = (u8)((uMecc1&0xFF000000)>>24);
		aNand_Spare_Data[8] = (u8)(uMecc2&0xFF);
		aNand_Spare_Data[9] = (u8)((uMecc2&0xFF00)>>8);
		aNand_Spare_Data[10] = (u8)((uMecc2&0xFF0000)>>16);
		aNand_Spare_Data[11] = (u8)((uMecc2&0xFF000000)>>24);
		aNand_Spare_Data[12] = (u8)(uMecc3&0xFF);
		aNand_Spare_Data[13] = 0xff;
		aNand_Spare_Data[14] = 0xff;
		aNand_Spare_Data[15] = 0xff;

		for(i=24 ; i<512 ; i++) 
		{
			NF_WRDATA8(Controller, *pBuffer++);
		}

		for (i=0 ; i <13 ; i++)
		{
			NF_WRDATA8(Controller, aNand_Spare_Data[i]);
		}

	#if 0 
		for(i=13; i<16 ; i++)
		{
			NF_WRDATA8(Controller, aNand_Spare_Data[i]);
		}
	#endif		

		NF_CLEAR_RnB(Controller);
		NF_CMD(Controller, 0x10);	// 
#if 1 // Interupt Using. 
		while(!g_Nand_RnBTransition)
		{
			if(g_Nand_IllegalAccError == 1)
			{
				NAND_Reset(Controller);
				g_Nand_IllegalAccError = 0;
				return eNAND_ProgramError;
			}
		}
#else
		NF_DETECT_RnB(Controller);
#endif
	}

	NF_CMD(Controller, 0x70);

	while(!((uStatus = NF_RDDATA8(Controller)) & (1<<6)));
	// 25. Write Operation Status Check.

	if(uStatus & 0x01)
		eError = eNAND_ProgramError;		// Error in Program
	else
		eError = eNAND_NoError;
	
	NF_nFCE_H(Controller); 

	if ( NAND_Inform[Controller].uECCtest != 1)
	{
		for ( i = 0 ; i<16 ; i++)
		{
			*pSpareBuffer++ = aNand_Spare_Data[i];

		}
	}

//	INTC_Mask(NUM_NFC); // FPGA
	INTC_Disable(NUM_NFC);
	
	return eError;
 }




 NAND_eERROR NAND_ReadPageSLC_8BitSimple(u32 Controller, u32 uBlock, u32 uPage, u8 *pBuffer, u8 *pSpareBuffer)
 {
 	u32 i, uBlockPage, uPageSize;
//	u32 uMecc0, uSecc, uMecc1, uMecc2, uMecc3;
	NAND_eERROR eError;	
	u32 *pBufferW;
//	u32 uTemp;
	u32 uSpareSize;
//	u32 uIndex, uVal, uTempMECCSpare;
//	u32 uCount=0, uCount1=0;
	
	pBufferW = (u32 *)pBuffer;

	// 01. aNAND_Spare_Data Init. add jungil 0625
	uSpareSize = 16;	


	uBlockPage=(uBlock<<5) + uPage;
	uPageSize = 512;

	for ( i=0; i<16 ; i++ )
	{
		aNand_Spare_Data[i] = 0x0;
	}

	*(unsigned volatile *)0x80200000=((*(unsigned volatile *)0x80200000)&~(0x3<<23))|(1<<30)|(1<<23)|(0x7<<12)|(0x7<<8)|(0x7<<4);

	*(unsigned volatile *)0x80200004 = 0x0;
//	*(unsigned volatile *)0x80200004=0xc7; // No Problem
	*(unsigned volatile *)0x80200004=(*(unsigned volatile *)0x80200004)|(0<<18)|(0<<11)|(1<<10)|(1<<9)|(1<<6)|(1<<0); // Init NFCONT

	*(unsigned volatile *)0x80200028=(*(unsigned volatile *)0x80200028)|(1<<5)|(1<<4); // Illegal access and RnB detectoin clear
	*(unsigned volatile *)0x80200028=(*(unsigned volatile *)0x80200028)|(1<<6)|(1<<7); // Encode / Decode done detection clear

	*(unsigned volatile *)0x80200004=(*(unsigned volatile *)0x80200004)|(1<<11); // MLC Stop 


	NF_MECC_Lock(Controller);
//	NF_RSTECC(Controller);    // Initialize ECC
//	NF_MECC_UnLock(Controller);

	NF_nFCE_L(Controller);
	NF_CLEAR_RnB(Controller);

	NF_CMD(Controller, 0x00);	// Read command
	NF_ADDR(Controller, 0); 	
	NF_ADDR(Controller, uBlockPage&0xff);		//
	NF_ADDR(Controller, (uBlockPage>>8)&0xff);	// Block & Page num.
	NF_ADDR(Controller, (uBlockPage>>16)&0xff);	//

	NF_DETECT_RnB(Controller);
	*(unsigned volatile *)0x80200028|=(1<<4); // RnB Detection Clear.

//	NF_MECC_Lock(Controller);
//	NF_RSTECC(Controller);    // Initialize ECC
	*(unsigned volatile *)0x80200004=(*(unsigned volatile *)0x80200004)|(1<<5); // Init Main ECC 
	NF_MECC_UnLock(Controller);



//	*(unsigned volatile *)0x80200004=(*(unsigned volatile *)0x80200004)|(1<<7); // Init Main ECC 

#if 0 
	for(i=0 ; i<uPageSize ; i++) 
	{
		*pBuffer++ = NF_RDDATA8(Controller);// Read one page
	}
#else
	for (i=0; i<uPageSize/4; i++)
	{
		*pBufferW++ = NF_RDDATA(Controller);	// Read one page
	}
#endif



#if 1 // Byte Read 
	for(i=0; i<13 ; i++)
	{
		aNand_Spare_Data[i] = NF_RDDATA8(Controller);
	}
#else // Word Read
	for(i=0; i<uSpareSize/4; i++)
	{
		aNand_Spare_Data_u32[i] = NF_RDDATA(Controller);
	}
#endif 

	*(unsigned volatile *)0x80200004|=(1<<7); // Main ECC Lock

//	NF_DETECT_RnB(Controller);
//	*(unsigned volatile *)0x80200028|=(1<<4); // RnB Detection Clear.	

	// Check decoding done 
	while(!(NAND(Controller)->rNFSTAT&(1<<6)));
	*(unsigned volatile *)0x80200028 |= (1<<6);

//	NF_MECC_Lock(Controller);

#if 1 // Byte Read
	for ( i=13 ; i<17 ; i++)
	{
		aNand_Spare_Data[i] = NF_RDDATA8(Controller);
	}
#endif

	NF_nFCE_H(Controller); 


	for (i=0 ; i <16 ; i++)
	{
		*pSpareBuffer++ = aNand_Spare_Data[i];
	}


	eError = NAND_CheckECCError_8Bit(Controller);

	return eError;


 }


 NAND_eERROR NAND_ReadPageSLC_8BitValid(u32 Controller, u32 uBlock, u32 uPage, u8 *pBuffer, u8 *pSpareBuffer)
 {
 	u32 i, uBlockPage, uPageSize;
//	u32 uMecc0, uSecc, uMecc1, uMecc2, uMecc3;
	NAND_eERROR eError;	
	u32 *pBufferW;
//	u32 uTemp;
	u32 uSpareSize;
//	u32 uIndex, uVal, uTempMECCSpare;
//	u32 uCount=0, uCount1=0;
	u32 uOption0;
	u32 uNFCONTreg, uNFCONFreg;

	for(i=0; i<13 ; i++)
	{
		aNand_Spare_Data[i] = 0x0;
	}
	
	pBufferW = (u32 *)pBuffer;

	uSpareSize = 16;	
	uBlockPage=(uBlock<<5) + uPage;
	uPageSize = 512;

	// Setting NFCONF Register.
	uNFCONFreg = Inp32(&(NAND(Controller)->rNFCONF));
	uNFCONFreg = uNFCONFreg & (~(0x1<<30)); // HCLK is more than 66Mhz.
	uNFCONFreg = ( uNFCONFreg & (~(0x3<<23)) ) | (0x1<<23) ; // 8bit ECC Select.
	Outp32(&(NAND(Controller)->rNFCONF), uNFCONFreg ); // NFCONF Register setting. 
	// Setting NFCONT Register.
	uNFCONTreg = Inp32(&(NAND(Controller)->rNFCONT)); // read NFCON
	uNFCONTreg = ( uNFCONTreg &~(0x1<<18) ) ; // 4bit ecc Decoding enable. 
//	uNFCONTreg &= ~((1<<13)|(1<<12));		// 4bit ECC encoding/decoding completion Interrupt disable
	uNFCONTreg = ( uNFCONTreg & ~(0x1<<12)) | (0x1<<12);	// 4bit ECC decoding completion Interrupt Enable.
	uNFCONTreg |= (1<<10) | (1<<9);		// Illegal Access & RnB Transition Interrupt enable
	uNFCONTreg |= (0x1<<6); // Spare ECC Lock 
	Outp32(&(NAND(Controller)->rNFCONT), uNFCONTreg); // write setting. NFCONT Register

	Outp32(&(NAND(Controller)->rNFSTAT), (1<<5)|(1<<4)); // Illegal Access & RnB Interrupt Pending bit clear

	//INTC_Unmask(NUM_NFC);

	// 3 Address Cycle Setting 
	#if 1 // address setting test.
	UART_Printf("----------------------------------------------------\n");
	UART_Printf(" 0: 3 Address Cycle , 1: 4 Address Cycle                                \n");
	uOption0 = UART_GetIntNum();
	UART_Printf("----------------------------------------------------\n");
	#else
	uOption0 = 1;
	#endif

	
	NF_MECC_Lock(Controller);

	NF_nFCE_L(Controller);
	NF_CLEAR_RnB(Controller);

	NF_CMD(Controller, 0x00);	// Read command
	NF_ADDR(Controller, 0); 	
	NF_ADDR(Controller, uBlockPage&0xff);		//
	NF_ADDR(Controller, (uBlockPage>>8)&0xff);	// Block & Page num.
	if (uOption0 == 1 ) NF_ADDR(Controller, (uBlockPage>>16)&0xff);	//

	#if 0
	while(!g_Nand_RnBTransition)
	{
		if(g_Nand_IllegalAccError == 1)
		{
			NAND_Reset(Controller);
			g_Nand_IllegalAccError = 0;
			return eNAND_ProgramError;
		}
	}
	#else
	NF_DETECT_RnB(Controller);
	NF_CLEAR_RnB(Controller);
	#endif

	//*(unsigned volatile *)0x80200004=((*(unsigned volatile *)0x80200004)&~(1<<7))|(1<<5); // Init Main ECC 
	NF_MECC_UnLock(Controller);
	NF_RSTECC(Controller);    // Initialize ECC

	#if 1
	for(i=0 ; i<uPageSize ; i++) 
	{
		*pBuffer++ = NF_RDDATA8(Controller);// Read one page
	}
	#else
	for (i=0; i<uPageSize/4; i++)
	{
		*pBufferW++ = NF_RDDATA(Controller);	// Read one page
	}
	#endif

	//*(unsigned volatile *)0x80200004|=(1<<7); // Main ECC Lock

	#if 1 // Byte Read 
	for(i=0; i<13 ; i++)
	{
		aNand_Spare_Data[i] = NF_RDDATA8(Controller);
	}
	#else // Word Read
	for(i=0; i<uSpareSize/4; i++)
	{
		aNand_Spare_Data_u32[i] = NF_RDDATA(Controller);
	}
	#endif 

	NF_MECC_Lock(Controller);

	// Check decoding done 
	while(!(NAND(Controller)->rNFSTAT&(1<<6)));
	Outp32(&(NAND(Controller)->rNFSTAT), (1<<6) ); // Clear Decodginf done
	
	#if 0 // Byte Read
	for ( i=13 ; i<17 ; i++)
	{
		aNand_Spare_Data[i] = NF_RDDATA8(Controller);
	}
	#endif

	NF_nFCE_H(Controller); 

	for (i=0 ; i <16 ; i++)
	{
		*pSpareBuffer++ = aNand_Spare_Data[i];
	}

	eError = NAND_CheckECCError_8Bit(Controller);

	INTC_Disable(NUM_NFC);

	return eError;



 }

int CorrectECC8Data(unsigned char *pEncodingBaseAddr)
{	
	unsigned int i,uErrorByte[9];//u32 uErrorByte1, uErrorByte2, uErrorByte3, uErrorByte4, uErrorByte5, uErrorByte6, uErrorByte7, uErrorByte8;	
	unsigned char uErrorBit[9];//u32 uErrorBit1, uErrorBit2, uErrorBit3, uErrorBit4, uErrorBit5, uErrorBit6, uErrorBit7, uErrorBit8;
	unsigned int uErrorType;
	
	// Searching Error Type //몇개냐
	uErrorType = ((NAND(0)->rNF8ECCERR0)>>25)&0xf;
	// Searching Error Byte //몇번째 바이트냐
	uErrorByte[1] = NAND(0)->rNF8ECCERR0&0x3ff;
	uErrorByte[2] = (NAND(0)->rNF8ECCERR0>>15)&0x3ff;
	
	uErrorByte[3] = (NAND(0)->rNF8ECCERR1)&0x3ff;
	uErrorByte[4] = (NAND(0)->rNF8ECCERR1>>11)&0x3ff;	
	uErrorByte[5] = (NAND(0)->rNF8ECCERR1>>22)&0x3ff;
	
	uErrorByte[6] = (NAND(0)->rNF8ECCERR2)&0x3ff;
	uErrorByte[7] = (NAND(0)->rNF8ECCERR2>>11)&0x3ff;
	uErrorByte[8] = (NAND(0)->rNF8ECCERR2>>22)&0x3ff;
	// Searching Error Bit //
	uErrorBit[1] = NAND(0)->rNFMLC8BITPT0&0xff;
	uErrorBit[2] = (NAND(0)->rNFMLC8BITPT0>>8)&0xff;
	uErrorBit[3] = (NAND(0)->rNFMLC8BITPT0>>16)&0xff;
	uErrorBit[4] = (NAND(0)->rNFMLC8BITPT0>>24)&0xff;
	
	uErrorBit[5] = NAND(0)->rNFMLC8BITPT1&0xff;
	uErrorBit[6] = (NAND(0)->rNFMLC8BITPT1>>8)&0xff;
	uErrorBit[7] = (NAND(0)->rNFMLC8BITPT1>>16)&0xff;
	uErrorBit[8] = (NAND(0)->rNFMLC8BITPT1>>24)&0xff;
	
	if(!uErrorType) 
		return 0;
	if(uErrorType == 0x9) return 1;
	
	for(i=1;i<=uErrorType ;i++)	
	{
		if(uErrorByte[i] < 512)	
			pEncodingBaseAddr[uErrorByte[i]]^=uErrorBit[i];
		else
		{
			;
		/*
			if(pNandInfo.uPageSize == 512)
			{
			}
			else if(pNandInfo.uPageSize == 2048)
			{
			}
			else if(pNandInfo.uPageSize == 4096)						
			{
				
			}
			*/
		}			
	}
	return 0;
	
}



 NAND_eERROR NAND_Read2048Page_8BitValid(u32 Controller, u32 uBlock, u32 uPage, u8 *pBuffer, u8 *pSpareBuffer)
 {
 	u32 i, uBlockPage, uPageSize;
	u32 u512PagesPerBlock;
//	u32 uMecc0, uSecc, uMecc1, uMecc2, uMecc3;
	NAND_eERROR eError;	
	u32 *pBufferW;
//	u32 uTemp;
	u32 uSpareSize;
//	u32 uIndex, uVal, uTempMECCSpare;
//	u32 uCount=0, uCount1=0;
	u32 uIndex0, uIndex1;
	unsigned int uResult[8]={0, };
	u32 uOption1;
	
	for(i=0; i<64 ; i++)
	{
		aNand_Spare_Data[i] = 0x0;
	}
	
	pBufferW = (u32 *)pBuffer;

	uSpareSize = 64;	
	uBlockPage=(uBlock<<6) + uPage;
	uPageSize = 2048;

	*(unsigned volatile *)0x70200000=((*(unsigned volatile *)0x70200000)&~(0x3<<23))|(1<<30)|(1<<23)|(0x7<<12)|(0x7<<8)|(0x7<<4);
	*(unsigned volatile *)0x70200004=( ((*(unsigned volatile *)0x70200004)&~(0x1<<18)) |(0<<18)|(0<<11)|(1<<10)|(1<<9)|(1<<6)|(1<<0)); // Init NFCONT
	*(unsigned volatile *)0x70200028=(*(unsigned volatile *)0x70200028)|(1<<5)|(1<<4); // Illegal access and RnB detectoin clear
	*(unsigned volatile *)0x70200028=(*(unsigned volatile *)0x70200028)|(1<<6)|(1<<7); // Encode / Decode done detection clear

#if 1
	////////////////////////////////////////////////////////////////////////
	//                                             0~511K Byte Read.                                             //
	////////////////////////////////////////////////////////////////////////	
	// 02. NAND Read Operation
	UART_Printf("Address Setting = 4: Address 4 /// 5: Address 5 ");
	uOption1 = UART_GetIntNum();
	NAND_Inform[Controller].uAddrCycle = uOption1;
	
	u512PagesPerBlock = (uPageSize>>9);
	NF_MECC_Lock(Controller); // Main ECC Lock
	NF_nFCE_L(Controller); // Chip Select Low
	NF_CLEAR_RnB(Controller); // RnB Clear

	for(i=0; i<u512PagesPerBlock; i++)
	{
		NF_MECC_Lock(Controller); // Main ECC Lock
		NF_CLEAR_RnB(Controller); // RnB Clear

		if(i==0)
		{
			NF_CMD(Controller,0x00);	// Read command
	
			NF_ADDR(Controller,0); 	
			NF_ADDR(Controller,0); 	
			NF_ADDR(Controller,uBlockPage&0xff);		//
			NF_ADDR(Controller,(uBlockPage>>8)&0xff);	// Block & Page num.
			if(NAND_Inform[Controller].uAddrCycle == 5)	NF_ADDR(Controller,(uBlockPage>>16)&0xff);	//
				
			NF_CMD(Controller,0x30);	// 2'nd command
	
			NF_DETECT_RnB(Controller); // RnB Detect.
			*(unsigned volatile *)0x70200028|=(1<<4); // RnB Detection Clear.
		}
		else
		{
			NF_CMD(Controller,0x05); // Random Address Access = 512K start
			NF_ADDR(Controller,(512*i)&0xFF);
			NF_ADDR(Controller,((512*i)>>8)&0xFF);
			NF_CMD(Controller,0xE0);
			
		}
		
		NF_MECC_UnLock(Controller); // Main ECC Unlock
		NF_RSTECC(Controller);    // Initialize ECC

		for(uIndex1=0;uIndex1<512;uIndex1++)  
			*pBuffer++=NF_RDDATA8(Controller);	// Read Main data

		// Spare Area Address Setting.
		NF_CMD(Controller,0x05);
		NF_ADDR(Controller,(uPageSize+13*i)&0xFF);
		NF_ADDR(Controller,((uPageSize+13*i)>>8)&0xFF);
		NF_CMD(Controller,0xE0);

		#if 1 
		for(uIndex0=0+13*i; uIndex0<13+13*i ; uIndex0++)
		{
			aNand_Spare_Data[uIndex0] = NF_RDDATA8(Controller); // Spare Data Read
		}
		#else
		for(i=0; i<13 ; i++)
			NF_RDDATA8(); // Spare Data Read
		#endif

		NF_MECC_Lock(Controller); // Main ECC Lock

		// Check decoding done 
		while(!(NAND(Controller)->rNFSTAT&(1<<6)));
		*(unsigned volatile *)0x70200028 |= (1<<6); // Detecting Done Clear

		// 03. NAND ECC Data Check 
		while( (NAND(Controller)->rNF8ECCERR0&(u32)((1<<31)) ) ); // Decoding Busy Check...
	//	if ( rNF8ECCERR0&(1<<29)) UART_Printf(" Free Page\n"); 	// Check Free Page
		uResult[i] = CorrectECC8Data((pBuffer-512));
	}
	NF_nFCE_H(Controller);  // Chip Select is High

	for (i=0 ; i <64 ; i++)
	{
		*pSpareBuffer++ = aNand_Spare_Data[i];
	}
	
	return (uResult[0]+uResult[1]+uResult[2]+uResult[3]+uResult[4]+uResult[5]+uResult[6]+uResult[7]);



#else

	////////////////////////////////////////////////////////////////////////
	//                                             0~511K Byte Read.                                             //
	////////////////////////////////////////////////////////////////////////	
	// 02. NAND Read Operation
	NF_MECC_Lock(Controller); // Main ECC Lock

	NF_nFCE_L(Controller); // Chip Select Low
	NF_CLEAR_RnB(Controller); // RnB Clear

	NF_CMD(Controller,0x00);	// Read command

	if(NAND_Inform[Controller].uAddrCycle == 4)
	{
		NF_CMD(Controller, 0x00);	// Read command
		NF_ADDR(Controller, 0); 	
	}
	else if(NAND_Inform[Controller].uAddrCycle == 5)
	{	
		NF_CMD(Controller, 0x00);	// 1st Read command
		NF_ADDR(Controller, 0); 	
		NF_ADDR(Controller, 0); 
	}

	NF_ADDR(Controller,uBlockPage&0xff);		//
	NF_ADDR(Controller,(uBlockPage>>8)&0xff);	// Block & Page num.
	NF_ADDR(Controller,(uBlockPage>>16)&0xff);	//

	NF_CMD(Controller,0x30);	// 2'nd command

	NF_DETECT_RnB(Controller); // RnB Detect.
	*(unsigned volatile *)0x70200028|=(1<<4); // RnB Detection Clear.

	NF_MECC_UnLock(Controller); // Main ECC Unlock
	NF_RSTECC(Controller);    // Initialize ECC

	for(i=0;i<512;i++)  
	{
		*pBuffer++=NF_RDDATA8(Controller);	// Read Main data
	}

	// Spare Area Address Setting.
	NF_CMD(Controller,0x05);
	NF_ADDR(Controller,(uPageSize)&0xFF);
	NF_ADDR(Controller,(uPageSize>>8)&0xFF);
	NF_CMD(Controller,0xE0);

	for(i=0; i<13; i++)
	{
		aNand_Spare_Data[i] = NF_RDDATA8(Controller); // Spare Data Read
	}

	NF_MECC_Lock(Controller); // Main ECC Lock

	// Check decoding done 
	while(!(NAND(Controller)->rNFSTAT&(1<<6)));
	*(unsigned volatile *)0x70200028 |= (1<<6); // Detecting Done Clear

	while( (NAND(Controller)->rNF8ECCERR0&(1<<31) ) ); // Decoding Busy Check...
	eError = NAND_CheckECCError_8Bit(Controller);
	if(eError != eNAND_NoError)
	{
		NF_nFCE_H(Controller);
		uTemp = NAND_ReadNFCONRegister(Controller);
		uTemp &= ~((1<<12)|(1<<13));		// ECC decoding Interrupt disable
		Outp32(&(NAND(Controller)->rNFCONT), uTemp);	
		NAND_Inform[Controller].uMLCECCPageReadSector = 0;		//0 ~ 511 Bytes	
		return eError;
	}

	////////////////////////////////////////////////////////////////////////////
	//                                             512 ~ 2048K Byte Read.                                             //
	////////////////////////////////////////////////////////////////////////////
	// 02. NAND Read Operation

	for ( uIndex0 = 1 ; uIndex0 < 4 ; uIndex0++)
	{
		NF_MECC_Lock(Controller); // Main ECC Lock
		NF_CLEAR_RnB(Controller); // RnB Clear

		NF_CMD(Controller,0x05); // Random Address Access = 512K start
		NF_ADDR(Controller,((512*uIndex0))&0xFF);
		NF_ADDR(Controller,((512*uIndex0)>>8)&0xFF);
		NF_CMD(Controller,0xE0);
		
		NF_MECC_UnLock(Controller); // Main ECC Unlock
		NF_RSTECC(Controller);    // Initialize ECC

		for(i=0;i<512;i++)  
		{
			*pBuffer++=NF_RDDATA8(Controller);	// Read Main data
		}

		// Spare Area Address Setting.
		NF_CMD(Controller,0x05);
		NF_ADDR(Controller,(uPageSize+(13*uIndex0))&0xFF);
		NF_ADDR(Controller,((uPageSize+(13*uIndex0))>>8)&0xFF);
		NF_CMD(Controller,0xE0);

		for(i=(0+(13*uIndex0)); i<(13+(13*uIndex0)) ; i++)
		{
			aNand_Spare_Data[i] = NF_RDDATA8(Controller); // Spare Data Read
		}

		NF_MECC_Lock(Controller); // Main ECC Lock

		while(!(NAND(Controller)->rNFSTAT&(1<<6)));
		*(unsigned volatile *)0x70200028 |= (1<<6); // Detecting Done Clear

		while( (NAND(Controller)->rNF8ECCERR0&(1<<31) ) ); // Decoding Busy Check...
		eError = NAND_CheckECCError_8Bit(Controller);
		if(eError != eNAND_NoError)
		{
			NF_nFCE_H(Controller);
			uTemp = NAND_ReadNFCONRegister(Controller);
			uTemp &= ~((1<<12)|(1<<13));		// ECC decoding Interrupt disable
			Outp32(&(NAND(Controller)->rNFCONT), uTemp);	
			NAND_Inform[Controller].uMLCECCPageReadSector = uIndex0;		//0 ~ 511 Bytes	
			return eError;
		}
	}


	NF_nFCE_H(Controller);
	uTemp = NAND_ReadNFCONRegister(Controller);
	uTemp &= ~((1<<12)|(1<<13));		// ECC decoding Interrupt disable
	Outp32(&(NAND(Controller)->rNFCONT), uTemp);	


	for (i=0 ; i <64 ; i++)
	{
		*pSpareBuffer++ = aNand_Spare_Data[i];
	}


	return eError;
#endif
 }

 NAND_eERROR NAND_Read4096Page_8BitValid(u32 Controller, u32 uBlock, u32 uPage, u8 *pBuffer, u8 *pSpareBuffer)
 {
 	u32 i, uBlockPage, uPageSize;
	u32 u512PagesPerBlock;
//	u32 uMecc0, uSecc, uMecc1, uMecc2, uMecc3;
	NAND_eERROR eError;	
	u32 *pBufferW;
//	u32 uTemp;
	u32 uSpareSize;
//	u32 uIndex, uVal, uTempMECCSpare;
//	u32 uCount=0, uCount1=0;
	u32 uIndex0, uIndex1;
	unsigned int uResult[8]={ 0,};
	
	for(i=0; i<128 ; i++)
	{
		aNand_Spare_Data[i] = 0x0;
	}
	
	pBufferW = (u32 *)pBuffer;

	uSpareSize = 128;	
	uBlockPage=(uBlock<<7) + uPage;
	uPageSize = 4096;

	*(unsigned volatile *)0x70200000=((*(unsigned volatile *)0x70200000)&~(0x3<<23))|(1<<30)|(1<<23)|(0x7<<12)|(0x7<<8)|(0x7<<4);
	*(unsigned volatile *)0x70200004=( ((*(unsigned volatile *)0x70200004)&~(0x1<<18)) |(0<<18)|(0<<11)|(1<<10)|(1<<9)|(1<<6)|(1<<0)); // Init NFCONT
	*(unsigned volatile *)0x70200028=(*(unsigned volatile *)0x70200028)|(1<<5)|(1<<4); // Illegal access and RnB detectoin clear
	*(unsigned volatile *)0x70200028=(*(unsigned volatile *)0x70200028)|(1<<6)|(1<<7); // Encode / Decode done detection clear

#if 1
	////////////////////////////////////////////////////////////////////////
	//                                             0~511K Byte Read.                                             //
	////////////////////////////////////////////////////////////////////////	
	// 02. NAND Read Operation
	
	
	u512PagesPerBlock = (uPageSize>>9);
	NF_MECC_Lock(Controller); // Main ECC Lock
	NF_nFCE_L(Controller); // Chip Select Low
	NF_CLEAR_RnB(Controller); // RnB Clear

	for(i=0; i<u512PagesPerBlock; i++)
	{
		NF_MECC_Lock(Controller); // Main ECC Lock
		NF_CLEAR_RnB(Controller); // RnB Clear

		if(i==0)
		{
			NF_CMD(Controller,0x00);	// Read command
	
			NF_ADDR(Controller,0); 	
			if(NAND_Inform[Controller].uAddrCycle == 5)	NF_ADDR(Controller,0); 	
			NF_ADDR(Controller,uBlockPage&0xff);		//
			NF_ADDR(Controller,(uBlockPage>>8)&0xff);	// Block & Page num.
			NF_ADDR(Controller,(uBlockPage>>16)&0xff);	//
				
			NF_CMD(Controller,0x30);	// 2'nd command
	
			NF_DETECT_RnB(Controller); // RnB Detect.
			*(unsigned volatile *)0x70200028|=(1<<4); // RnB Detection Clear.
		}
		else
		{
			NF_CMD(Controller,0x05); // Random Address Access = 512K start
			NF_ADDR(Controller,(512*i)&0xFF);
			NF_ADDR(Controller,((512*i)>>8)&0xFF);
			NF_CMD(Controller,0xE0);
			
		}
		
		NF_MECC_UnLock(Controller); // Main ECC Unlock
		NF_RSTECC(Controller);    // Initialize ECC

		for(uIndex1=0;uIndex1<512;uIndex1++)  
			*pBuffer++=NF_RDDATA8(Controller);	// Read Main data

		// Spare Area Address Setting.
		NF_CMD(Controller,0x05);
		NF_ADDR(Controller,(uPageSize+13*i)&0xFF);
		NF_ADDR(Controller,((uPageSize+13*i)>>8)&0xFF);
		NF_CMD(Controller,0xE0);

		#if 1 
		for(uIndex0=0+13*i; uIndex0<13+13*i ; uIndex0++)
		{
			aNand_Spare_Data[uIndex0] = NF_RDDATA8(Controller); // Spare Data Read
		}
		#else
		for(i=0; i<13 ; i++)
			NF_RDDATA8(); // Spare Data Read
		#endif

		NF_MECC_Lock(Controller); // Main ECC Lock

		// Check decoding done 
		while(!(NAND(Controller)->rNFSTAT&(1<<6)));
		*(unsigned volatile *)0x70200028 |= (1<<6); // Detecting Done Clear

		// 03. NAND ECC Data Check 
		while( (NAND(Controller)->rNF8ECCERR0&(u32)((1<<31) )) ); // Decoding Busy Check...
	//	if ( rNF8ECCERR0&(1<<29)) UART_Printf(" Free Page\n"); 	// Check Free Page
		uResult[i] = CorrectECC8Data((pBuffer-512));
	}
	NF_nFCE_H(Controller);  // Chip Select is High

	for (i=0 ; i <128 ; i++)
	{
		*pSpareBuffer++ = aNand_Spare_Data[i];
	}
	
	return (uResult[0]+uResult[1]+uResult[2]+uResult[3]+uResult[4]+uResult[5]+uResult[6]+uResult[7]);



#else

	////////////////////////////////////////////////////////////////////////
	//                                             0~511K Byte Read.                                             //
	////////////////////////////////////////////////////////////////////////	
	// 02. NAND Read Operation
	NF_MECC_Lock(Controller); // Main ECC Lock

	NF_nFCE_L(Controller); // Chip Select Low
	NF_CLEAR_RnB(Controller); // RnB Clear

	NF_CMD(Controller,0x00);	// Read command

	if(NAND_Inform[Controller].uAddrCycle == 4)
	{
		NF_CMD(Controller, 0x00);	// Read command
		NF_ADDR(Controller, 0); 	
	}
	else if(NAND_Inform[Controller].uAddrCycle == 5)
	{	
		NF_CMD(Controller, 0x00);	// 1st Read command
		NF_ADDR(Controller, 0); 	
		NF_ADDR(Controller, 0); 
	}

	NF_ADDR(Controller,uBlockPage&0xff);		//
	NF_ADDR(Controller,(uBlockPage>>8)&0xff);	// Block & Page num.
	NF_ADDR(Controller,(uBlockPage>>16)&0xff);	//

	NF_CMD(Controller,0x30);	// 2'nd command

	NF_DETECT_RnB(Controller); // RnB Detect.
	*(unsigned volatile *)0x70200028|=(1<<4); // RnB Detection Clear.

	NF_MECC_UnLock(Controller); // Main ECC Unlock
	NF_RSTECC(Controller);    // Initialize ECC

	for(i=0;i<512;i++)  
	{
		*pBuffer++=NF_RDDATA8(Controller);	// Read Main data
	}

	// Spare Area Address Setting.
	NF_CMD(Controller,0x05);
	NF_ADDR(Controller,(uPageSize)&0xFF);
	NF_ADDR(Controller,(uPageSize>>8)&0xFF);
	NF_CMD(Controller,0xE0);

	for(i=0; i<13; i++)
	{
		aNand_Spare_Data[i] = NF_RDDATA8(Controller); // Spare Data Read
	}

	NF_MECC_Lock(Controller); // Main ECC Lock

	// Check decoding done 
	while(!(NAND(Controller)->rNFSTAT&(1<<6)));
	*(unsigned volatile *)0x70200028 |= (1<<6); // Detecting Done Clear

	while( (NAND(Controller)->rNF8ECCERR0&(1<<31) ) ); // Decoding Busy Check...
	eError = NAND_CheckECCError_8Bit(Controller);
	if(eError != eNAND_NoError)
	{
		NF_nFCE_H(Controller);
		uTemp = NAND_ReadNFCONRegister(Controller);
		uTemp &= ~((1<<12)|(1<<13));		// ECC decoding Interrupt disable
		Outp32(&(NAND(Controller)->rNFCONT), uTemp);	
		NAND_Inform[Controller].uMLCECCPageReadSector = 0;		//0 ~ 511 Bytes	
		return eError;
	}

	////////////////////////////////////////////////////////////////////////////
	//                                             512 ~ 2048K Byte Read.                                             //
	////////////////////////////////////////////////////////////////////////////
	// 02. NAND Read Operation

	for ( uIndex0 = 1 ; uIndex0 < 4 ; uIndex0++)
	{
		NF_MECC_Lock(Controller); // Main ECC Lock
		NF_CLEAR_RnB(Controller); // RnB Clear

		NF_CMD(Controller,0x05); // Random Address Access = 512K start
		NF_ADDR(Controller,((512*uIndex0))&0xFF);
		NF_ADDR(Controller,((512*uIndex0)>>8)&0xFF);
		NF_CMD(Controller,0xE0);
		
		NF_MECC_UnLock(Controller); // Main ECC Unlock
		NF_RSTECC(Controller);    // Initialize ECC

		for(i=0;i<512;i++)  
		{
			*pBuffer++=NF_RDDATA8(Controller);	// Read Main data
		}

		// Spare Area Address Setting.
		NF_CMD(Controller,0x05);
		NF_ADDR(Controller,(uPageSize+(13*uIndex0))&0xFF);
		NF_ADDR(Controller,((uPageSize+(13*uIndex0))>>8)&0xFF);
		NF_CMD(Controller,0xE0);

		for(i=(0+(13*uIndex0)); i<(13+(13*uIndex0)) ; i++)
		{
			aNand_Spare_Data[i] = NF_RDDATA8(Controller); // Spare Data Read
		}

		NF_MECC_Lock(Controller); // Main ECC Lock

		while(!(NAND(Controller)->rNFSTAT&(1<<6)));
		*(unsigned volatile *)0x70200028 |= (1<<6); // Detecting Done Clear

		while( (NAND(Controller)->rNF8ECCERR0&(1<<31) ) ); // Decoding Busy Check...
		eError = NAND_CheckECCError_8Bit(Controller);
		if(eError != eNAND_NoError)
		{
			NF_nFCE_H(Controller);
			uTemp = NAND_ReadNFCONRegister(Controller);
			uTemp &= ~((1<<12)|(1<<13));		// ECC decoding Interrupt disable
			Outp32(&(NAND(Controller)->rNFCONT), uTemp);	
			NAND_Inform[Controller].uMLCECCPageReadSector = uIndex0;		//0 ~ 511 Bytes	
			return eError;
		}
	}


	NF_nFCE_H(Controller);
	uTemp = NAND_ReadNFCONRegister(Controller);
	uTemp &= ~((1<<12)|(1<<13));		// ECC decoding Interrupt disable
	Outp32(&(NAND(Controller)->rNFCONT), uTemp);	


	for (i=0 ; i <64 ; i++)
	{
		*pSpareBuffer++ = aNand_Spare_Data[i];
	}


	return eError;
#endif
 }




//////////
// Function Name : NAND_ReadPage_8Bit
// Function Description : This function read the 1 page data of external NAND device
// Input : 	Controller - Nand Controller Port Number 
//			uBlock - Read Block
//			uPage - Read Page
//			pBuffer - Read data buffer
// Output : 	Nand Error Type
NAND_eERROR NAND_ReadPage_8Bit(u32 Controller, u32 uBlock, u32 uPage, u8 *pBuffer, u8 *pSpareBuffer)
{
	NAND_eERROR eError;
	eError = NAND_ReadPageSLC_8BitValid(Controller, uBlock, uPage, pBuffer, pSpareBuffer);	
	return eError;
}

NAND_eERROR NAND_Read2048Page_8Bit(u32 Controller, u32 uBlock, u32 uPage, u8 *pBuffer, u8 *pSpareBuffer)
{
	NAND_eERROR eError;
	eError = NAND_Read2048Page_8BitValid(Controller, uBlock, uPage, pBuffer, pSpareBuffer);	
	return eError;
}

NAND_eERROR NAND_Read4096Page_8Bit(u32 Controller, u32 uBlock, u32 uPage, u8 *pBuffer, u8 *pSpareBuffer)
{
	NAND_eERROR eError;
	eError = NAND_Read4096Page_8BitValid(Controller, uBlock, uPage, pBuffer, pSpareBuffer);	
	return eError;
}



//////////
// Function Name : NAND_WritePage_Bit
// Function Description : This function write the 1 page data of external MLC Type NAND device
// Input : 	Controller - Nand Controller Port Number 
//			uBlock - Write Block
//			uPage - Write Page
//			pBuffer - Write data buffer
// Output : 	Nand Error Type
NAND_eERROR NAND_WritePage_8Bit(u32 Controller, u32 uBlock, u32 uPage, u8 *pBuffer, u8 *aSpareBuffer)
{
	NAND_eERROR eError;
	eError = NAND_WritePageSLC_8BitValid(Controller, uBlock, uPage, pBuffer, aSpareBuffer);	
	return eError;
}

NAND_eERROR NAND_WritePage_8Bit_24Byte(u32 Controller, u32 uBlock, u32 uPage, u8 *pBuffer, u8 *aSpareBuffer)
{
	NAND_eERROR eError;
	eError = NAND_WritePageSLC_8BitValid_24Byte(Controller, uBlock, uPage, pBuffer, aSpareBuffer);	
	return eError;
}

NAND_eERROR NAND_Write2048Page_8Bit(u32 Controller, u32 uBlock, u32 uPage, u8 *pBuffer, u8 *aSpareBuffer)
{
	NAND_eERROR eError;
	eError = NAND_Write2048Page_8BitValid(Controller, uBlock, uPage, pBuffer, aSpareBuffer);	
	return eError;
}

NAND_eERROR NAND_Write4096Page_8Bit(u32 Controller, u32 uBlock, u32 uPage, u8 *pBuffer, u8 *aSpareBuffer)
{
	NAND_eERROR eError;
	eError = NAND_Write4096Page_8BitValid(Controller, uBlock, uPage, pBuffer, aSpareBuffer);	
	return eError;
}




//////////
// Function Name : NAND_EraseSingleBlock_8Bit
// Function Description : This function erase 1 block data
// Input : 	Controller - Nand Controller Port Number 
//			uBlock - Erase Block
// Output : 	Nand Error Type
NAND_eERROR NAND_EraseSingleBlock_Basic(u32 Controller, u32 uBlock)
{
	u32 uBlockPage, uStatus;
	NAND_eERROR eError;

	if(NAND_Inform[Controller].uNandType == NAND_Normal8bit)
	{
		uBlockPage=(uBlock<<5);
	}
	else if(NAND_Inform[Controller].uNandType == NAND_Advanced8bit)
	{
		uBlockPage=(uBlock<<6);
	}
	else //if(NAND_Inform[Controller].uNandType == NAND_MLC8bit)
	{
		uBlockPage=(uBlock<<7);
	}

	NF_nFCE_L(Controller);
	NF_CLEAR_RnB(Controller);

	NF_CMD(Controller, 0x60);					// 1st Erase Command

	NF_ADDR(Controller, uBlockPage&0xff);	 		// The mark of bad block is in 0 page
	NF_ADDR(Controller, (uBlockPage>>8)&0xff);	 	// For block number A[24:17]
	NF_ADDR(Controller, (uBlockPage>>16)&0xff);  		// For block number A[25]

	NF_CMD(Controller, 0xd0);								// 2nd Erase command
	NF_DETECT_RnB(Controller);

	NF_CMD(Controller, 0x70);   // Read status command

	while(!((uStatus = NF_RDDATA8(Controller)) & (1<<6)));

	if(uStatus & 0x01)
		eError = eNAND_EraseError;		// Error in Program
	else
		eError = eNAND_NoError;
	   
	NF_nFCE_H(Controller);

	return eError;
}

NAND_eERROR NAND_WritePage_1Bit(u32 Controller, u32 uBlock, u32 uPage, u8 *pBuffer, u8 *aSpareBuffer)
{
	NAND_eERROR eError;

	if( (NAND_Inform[Controller].uNandType == NAND_Normal8bit) || (NAND_Inform[Controller].uNandType == NAND_Advanced8bit) )
		eError = NAND_WritePageSLC_1Bit(Controller, uBlock, uPage, pBuffer, aSpareBuffer);
	else //if(NAND_Inform[Controller].uNandType == NAND_MLC8bit)
		eError = NAND_WritePageMLC(Controller, uBlock, uPage, pBuffer, aSpareBuffer);

	return eError;
}

NAND_eERROR NAND_WritePageSLC_1Bit(u32 Controller, u32 uBlock, u32 uPage, u8 *pBuffer, u8 *aSpareBuffer)
{
	u32 i, uBlockPage, uPageSize, uSpareSize, uTemp;
	u32 uMecc, uSecc, uStatus;
	NAND_eERROR eError;	
	u8* pBufferSpare;
	u32 *pBufferW;
	u32 uNFCONFreg, uNFCONTreg;
	u32 uTemp0;
			
	pBufferW = (u32 *)pBuffer;			
	
	g_Nand_RnBTransition = 0;

	// Setting NFCONF Register.
	uNFCONFreg = Inp32(&(NAND(Controller)->rNFCONF));
	uNFCONFreg = uNFCONFreg & (~(0x1<<30)); // HCLK is more than 66Mhz.
	uNFCONFreg = uNFCONFreg & (~(0x3<<23)); // 1bit ECC Select.
//	uNFCONFreg = uNFCONFreg | ((7<<12)|(7<<8)|(7<<4)) ; // Timing 7,7,7 setting. add. jungil 
	Outp32(&(NAND(Controller)->rNFCONF), uNFCONFreg ); // NFCONF Register setting. 
	// Setting NFCONT Register.
	uNFCONTreg = Inp32(&(NAND(Controller)->rNFCONT)); // read NFCON
	uNFCONTreg &= ~((1<<13)|(1<<12));		// 4bit ECC encoding/decoding completion Interrupt disable
	uNFCONTreg |= (1<<10) | (1<<9);		// Illegal Access & RnB Transition Interrupt enable
	Outp32(&(NAND(Controller)->rNFCONT), uNFCONTreg); // write setting. NFCONT Register
	Outp32(&(NAND(Controller)->rNFSTAT), (1<<5)|(1<<4)); // Illegal Access & RnB Interrupt Pending bit clear

	INTC_Enable(NUM_NFC); // Interrupt Enable
	
	if(NAND_Inform[Controller].uNandType == NAND_Normal8bit)
	{
		uBlockPage=(uBlock<<5) + uPage;
		uPageSize = NAND_Inform[Controller].uPageSize;
		uSpareSize = NAND_Inform[Controller].uSpareSize;
	}
	else //if(NAND_Inform[Controller].uNandType == NAND_Advanced8bit)
	{
		uBlockPage=(uBlock<<6) + uPage;
		uPageSize = NAND_Inform[Controller].uPageSize;
		uSpareSize = NAND_Inform[Controller].uSpareSize;
	}


	NF_RSTECC(Controller);    // Initialize ECC
	NF_MECC_UnLock(Controller); // Main ECC Unlock

	NF_nFCE_L(Controller);    
	NF_CLEAR_RnB(Controller);

	NF_CMD(Controller, 0x80);	// Program command
	NF_ADDR(Controller, 0); 
		
	if(NAND_Inform[Controller].uNandType == NAND_Advanced8bit)
	//if(NAND_Inform[Controller].uAddrCycle == 5)
		NF_ADDR(Controller, 0); 
	
	NF_ADDR(Controller, uBlockPage&0xff);		//
	NF_ADDR(Controller, (uBlockPage>>8)&0xff);	// Block & Page num.
	NF_ADDR(Controller, (uBlockPage>>16)&0xff);	//

    	#if NAND_TRANSFER_MODE == POLLING_TRANSFER
	#if 0
		for(i=0 ; i<uPageSize ; i++) 
		{
			NF_WRDATA8(Controller, *pBuffer++);		// Write one page
		}
	#else
		for(i=0 ; i<uPageSize/4 ; i++) 
		{
			NF_WRDATA(Controller, *pBufferW++);	// Read one page
		}
	#endif		
	#elif NAND_TRANSFER_MODE == DMA_TRANSFER
		Nand_DmaDone = 0;
		DMACH_Setup(DMA_A, 0x0, (u32)pBufferW, 0, (u32)(&(NAND(Controller)->rNFDATA)), 1, WORD, uPageSize/4, DEMAND, MEM, DMA1_NAND_RX, BURST128, &g_oNandDmac1);
			
		DMACH_Start(&g_oNandDmac1);
		while(!Nand_DmaDone);	
	#endif

	NF_MECC_Lock(Controller); // Main ECC Lock
	
	g_Nand_IllegalAccError  = 0;

	if(NAND_Inform[Controller].uNandType == NAND_Normal8bit)
	{

		if ( NAND_Inform[Controller].uECCtest == 1)
		{
			aNand_Spare_Data[0] = aNand_Spare_Data_Temp[0];
			aNand_Spare_Data[1] = aNand_Spare_Data_Temp[1];
			aNand_Spare_Data[2] = aNand_Spare_Data_Temp[2];
			aNand_Spare_Data[3] = aNand_Spare_Data_Temp[3];
			aNand_Spare_Data[4] = 0xff;
		}
		else
		{
			// Main Area ECC
			uMecc = Inp32(&NAND(Controller)->rNFMECC0);
			aNand_Spare_Data[0] = (u8)(uMecc&0xFF);
			aNand_Spare_Data[1] = (u8)((uMecc&0xFF00)>>8);
			aNand_Spare_Data[2] = (u8)((uMecc&0xFF0000)>>16);
			aNand_Spare_Data[3] = (u8)((uMecc&0xFF000000)>>24);
			aNand_Spare_Data[4] = (u8)0xFF;			//Marking Valid Block
		}

		NF_SECC_UnLock(Controller); // Spare ECC Unlock

		if ( NAND_Inform[Controller].uSpareECC == 1) 
		{
			for(i=0;i<4;i++) 
			{
				NF_WRDATA8(Controller, *pBufferSpare++);		// Write spare array(Main ECC)			
	    		} 
		}
		else
		{
			for(i=0;i<4;i++) 
			{
				NF_WRDATA8(Controller, aNand_Spare_Data[i]);		// Write spare array(Main ECC)
	    		} 
		}

		NF_SECC_Lock(Controller);

		uSecc = Inp32(&NAND(Controller)->rNFSECC) & 0xFFFF; 		// Spare Area ECC
//if ( (NAND_Inform[Controller].uECCtest == 1) | (NAND_Inform[Controller].uSpareECC == 1) )
		if ( (NAND_Inform[Controller].uECCtest == 1) )		
		{
			aNand_Spare_Data[8] = aNand_Spare_Data_Temp[8];
			aNand_Spare_Data[9] = aNand_Spare_Data_Temp[9];
		}
		else			
		{
			aNand_Spare_Data[8] = (u8)(uSecc&0xFF);
			aNand_Spare_Data[9] = (u8)((uSecc&0xFF00)>>8);
		}

		for(i=4 ; i<uSpareSize; i++) 
		{
			NF_WRDATA8(Controller, aNand_Spare_Data[i]);  // Write spare array(Spare ECC and Mark)
		}  

		NF_CLEAR_RnB(Controller);
		NF_CMD(Controller, 0x10);	 // Write 2nd command	
	}
	else if(NAND_Inform[Controller].uNandType == NAND_Advanced8bit)
	{	
		uMecc = Inp32(&NAND(Controller)->rNFMECC0); 		// Main Area ECC

		aNand_Spare_Data[0] = (u8)0xFF;				//Marking Valid Block
		aNand_Spare_Data[1] = (u8)(uMecc&0xFF);
		aNand_Spare_Data[2] = (u8)((uMecc&0xFF00)>>8);
		aNand_Spare_Data[3] = (u8)((uMecc&0xFF0000)>>16);
		aNand_Spare_Data[4] = (u8)((uMecc&0xFF000000)>>24);

		NF_WRDATA8(Controller, aNand_Spare_Data[0]);	//Write Marking Valid Block
		
		NF_SECC_UnLock(Controller); // Spare ECC Unlock
		
		for(i=1;i<5;i++) 
		{
			NF_WRDATA8(Controller, aNand_Spare_Data[i]);		// Write spare array(Main ECC)
			//NF8_Spare_Data[i]=aNand_Spare_Data[i];
    		} 
		NF_SECC_Lock(Controller); // Spare ECC Lock

		uSecc = Inp32(&NAND(Controller)->rNFSECC) & 0xFFFF;  // Spare Area ECC

		aNand_Spare_Data[8] = (u8)(uSecc&0xFF);
		aNand_Spare_Data[9] = (u8)((uSecc&0xFF00)>>8);

		for(i=5 ; i<uSpareSize; i++) 
		{
			NF_WRDATA8(Controller, aNand_Spare_Data[i]);  // Write spare array(Spare ECC and Mark)
			//NF8_Spare_Data[i]=aNand_Spare_Data[i];
		}  

		NF_CLEAR_RnB(Controller);
		NF_CMD(Controller, 0x10);	 // Write 2nd command	
	}

	#if 1 // Interupt Mode
	while(!g_Nand_RnBTransition)
	{
		if(g_Nand_IllegalAccError == 1)
		{
			NAND_Reset(Controller);
			g_Nand_IllegalAccError = 0;
			return eNAND_ProgramError;
		}
	}
	#else // Polling Mode
	NF_DETECT_RnB(Controller);
	#endif

	NF_CMD(Controller, 0x70); 	//Read Program Status

	while(!((uStatus = NF_RDDATA8(Controller)) & (1<<6)));

	if(uStatus & 0x01)
		eError = eNAND_ProgramError;		// Error in Program
	else
		eError = eNAND_NoError;
	
	NF_nFCE_H(Controller);
	
	uTemp = Inp32(&(NAND(Controller)->rNFCONT));
	uTemp &= ~((1<<10) | (1<<9));		// Illegal Access & RnB Transition Interrupt disable
	Outp32(&(NAND(Controller)->rNFCONT), uTemp);

	if( (NAND_Inform[Controller].uECCtest == 0) & (NAND_Inform[Controller].uSpareECC == 0))
	{
		for(i=0 ; i<uSpareSize ; i++)
			aNand_Spare_Data_Temp[i] = aNand_Spare_Data[i];
	}

	#if 0 
	if(NAND_Inform[Controller].uSpareECC == 0)
	{
		for(i=0 ; i<uSpareSize ; i++)
			aNand_Spare_Data_Temp[i] = aNand_Spare_Data[i];
	}
	#endif	

	for(i=0 ; i<uSpareSize ; i++) // Spare ECC Value BACK Up
	{
		*aSpareBuffer++ = aNand_Spare_Data[i];
	}

	
	INTC_Disable(NUM_NFC);
	return eError;
}

NAND_eERROR NAND_ReadPage_1Bit(u32 Controller, u32 uBlock, u32 uPage, u8 *pBuffer, u8 *pSpareBuffer)
{
	NAND_eERROR eError;

	if( (NAND_Inform[Controller].uNandType == NAND_Normal8bit) || (NAND_Inform[Controller].uNandType == NAND_Advanced8bit) )
		eError = NAND_ReadPageSLC_1Bit(Controller, uBlock, uPage, pBuffer, pSpareBuffer);
	else //if(NAND_Inform[Controller].uNandType == NAND_MLC8bit)
		eError = NAND_ReadPageMLC(Controller, uBlock, uPage, pBuffer, pSpareBuffer);

	return eError;
}


NAND_eERROR NAND_ReadPageSLC_1Bit(u32 Controller, u32 uBlock, u32 uPage, u8 *pBuffer, u8 *pSpareBuffer)
{
	u32 i, uBlockPage, uPageSize;
	u32 uMecc, uSecc;
	NAND_eERROR eError;	
	u32 uTempMecc;
	u32 *pBufferW;
	u32 uNFCONFreg, uNFCONTreg;
	u32 uTemp0;
			
	pBufferW = (u32 *)pBuffer;			
	
	g_Nand_RnBTransition = 0;

	// Setting NFCONF Register.
	uNFCONFreg = Inp32(&(NAND(Controller)->rNFCONF));
	uNFCONFreg = uNFCONFreg & (~(0x1<<30)); // HCLK is more than 66Mhz.
	uNFCONFreg = uNFCONFreg & (~(0x3<<23)); // 1bit ECC Select.
//	uNFCONFreg = uNFCONFreg | ((7<<12)|(7<<8)|(7<<4)) ; // Timing 7,7,7 setting. add. jungil 
	Outp32(&(NAND(Controller)->rNFCONF), uNFCONFreg ); // NFCONF Register setting. 
	// Setting NFCONT Register.
	uNFCONTreg = Inp32(&(NAND(Controller)->rNFCONT)); // read NFCON
	uNFCONTreg &= ~((1<<13)|(1<<12));		// 4bit ECC encoding/decoding completion Interrupt disable
	uNFCONTreg |= (1<<10) | (1<<9);		// Illegal Access & RnB Transition Interrupt enable
	Outp32(&(NAND(Controller)->rNFCONT), uNFCONTreg); // write setting. NFCONT Register
	Outp32(&(NAND(Controller)->rNFSTAT), (1<<5)|(1<<4)); // Illegal Access & RnB Interrupt Pending bit clear

	INTC_Enable(NUM_NFC); // Interrupt Enable

	
	if(NAND_Inform[Controller].uNandType == NAND_Normal8bit)
	{
		uBlockPage=(uBlock<<5) + uPage;
		uPageSize = NAND_Inform[Controller].uPageSize;
	}
	else //if(NAND_Inform[Controller].uNandType == NAND_Advanced8bit)
	{
		uBlockPage=(uBlock<<6) + uPage;
		uPageSize = NAND_Inform[Controller].uPageSize;
	}

	NF_RSTECC(Controller);    // Initialize ECC
	NF_MECC_UnLock(Controller);
    
	NF_nFCE_L(Controller);    
	NF_CLEAR_RnB(Controller);

	//if(NAND_Inform[Controller].uNandType == NAND_Normal8bit)
	if(NAND_Inform[Controller].uAddrCycle == 4)
	{
		NF_CMD(Controller, 0x00);	// Read command
		NF_ADDR(Controller, 0); 	
	}
	else if(NAND_Inform[Controller].uAddrCycle == 5)
	{
		NF_CMD(Controller, 0x00);	// 1st Read command
		NF_ADDR(Controller, 0); 	
		NF_ADDR(Controller, 0); 
	}
	
	NF_ADDR(Controller, uBlockPage&0xff);		//
	NF_ADDR(Controller, (uBlockPage>>8)&0xff);	// Block & Page num.
	NF_ADDR(Controller, (uBlockPage>>16)&0xff);	//

	if(NAND_Inform[Controller].uNandType == NAND_Advanced8bit)
		NF_CMD(Controller, 0x30);	// 2'nd command
	
	NF_DETECT_RnB(Controller);

    	#if NAND_TRANSFER_MODE == POLLING_TRANSFER
	#if 0
		for(i=0 ; i<uPageSize ; i++) 
		{
			*pBuffer++ = NF_RDDATA8(Controller);	// Read one page
		}
	#else
		for(i=0 ; i<uPageSize/4 ; i++) 
		{
			*pBufferW++ = NF_RDDATA(Controller);	// Read one page
		}
	#endif
	#elif NAND_TRANSFER_MODE == DMA_TRANSFER
		Nand_DmaDone = 0;
		DMACH_Setup(DMA_A, 0x0, (u32)(&(NAND(Controller)->rNFDATA)), 1, (u32)pBufferW, 0, WORD, uPageSize/4, DEMAND, DMA1_NAND_TX, MEM, BURST128, &g_oNandDmac1);
			
		DMACH_Start(&g_oNandDmac1);
		while(!Nand_DmaDone);	
	#endif

	NF_MECC_Lock(Controller); // Main ECC Lock

	if(NAND_Inform[Controller].uNandType == NAND_Normal8bit)
	{
		NF_SECC_UnLock(Controller); // Spare ECC Unlock
		
		uMecc = NF_RDDATA(Controller); 		// Main Area ECC Check(1st ~ 4th byte : Main Area ECC Information)

		NF_SECC_Lock(Controller); // Spare ECC Lock 

		if((NAND_Inform[Controller].uECCtest == 0) & (NAND_Inform[Controller].uSpareECC == 0)) // Main ECC is Not, Spare ECC Is Test
		{
			Outp32(&NAND(Controller)->rNFMECCD0, ((uMecc&0xff00)<<8)|(uMecc&0xff) );
			Outp32(&NAND(Controller)->rNFMECCD1, ((uMecc&0xff000000)>>8)|((uMecc&0xff0000)>>16) );
		}
		else if ( (NAND_Inform[Controller].uECCtest == 1) | (NAND_Inform[Controller].uSpareECC == 1) ) // Main ECC Test. 
		{
			uTempMecc = ((u32)aNand_Spare_Data_Temp[3]<<24)|
						((u32)aNand_Spare_Data_Temp[2]<<16)|
						((u32)aNand_Spare_Data_Temp[1]<<8)|
						((u32)aNand_Spare_Data_Temp[0]);		
			Outp32(&NAND(Controller)->rNFMECCD0, ((uTempMecc&0xff00)<<8)|(uTempMecc&0xff) );
			Outp32(&NAND(Controller)->rNFMECCD1, ((uTempMecc&0xff000000)>>8)|((uTempMecc&0xff0000)>>16) );
		}
		
		aNand_Spare_Data[0] = (u8)(uMecc&0xFF);
		aNand_Spare_Data[1] = (u8)((uMecc&0xFF00)>>8);
		aNand_Spare_Data[2] = (u8)((uMecc&0xFF0000)>>16);
		aNand_Spare_Data[3] = (u8)((uMecc&0xFF000000)>>24);
		aNand_Spare_Data[4] = NF_RDDATA8(Controller);		 // read 5th byte(dummy)
	}
	else if(NAND_Inform[Controller].uNandType == NAND_Advanced8bit)
	{	
		aNand_Spare_Data[0] = NF_RDDATA8(Controller);		// Invalid Block Check data

		NF_SECC_UnLock(Controller); // Spare ECC Unlock
		
		uMecc = NF_RDDATA(Controller); 		// Main Area ECC Check

		if(NAND_Inform[Controller].uSpareECCtest == 0)
		{
			Outp32(&NAND(Controller)->rNFMECCD0, ((uMecc&0xff00)<<8)|(uMecc&0xff) );
			Outp32(&NAND(Controller)->rNFMECCD1, ((uMecc&0xff000000)>>8)|((uMecc&0xff0000)>>16) );
		}
		
		NF_SECC_Lock(Controller); // Spare ECC Lock 

		aNand_Spare_Data[1] = (u8)(uMecc&0xFF);
		aNand_Spare_Data[2] = (u8)((uMecc&0xFF00)>>8);
		aNand_Spare_Data[3] = (u8)((uMecc&0xFF0000)>>16);
		aNand_Spare_Data[4] = (u8)((uMecc&0xFF000000)>>24);
	}

	for(i=5; i<NAND_Inform[Controller].uSpareSize ; i++)
	{
		aNand_Spare_Data[i] = NF_RDDATA8(Controller);
	}

	// Spare Area ECC Check
	if ( (NAND_Inform[Controller].uECCtest == 1) | (NAND_Inform[Controller].uSpareECC == 1) )
	{
		aNand_Spare_Data[8] = aNand_Spare_Data_Temp[8];
		aNand_Spare_Data[9] = aNand_Spare_Data_Temp[9];
		uSecc = (aNand_Spare_Data[9]<<8) | (aNand_Spare_Data[8]);
	}
	else
	{
		//aNand_Spare_Data[8] = (u8)(uSecc&0xFF); // add 080314 jungil 
		//aNand_Spare_Data[9] = (u8)((uSecc&0xFF00)>>8);
		uSecc = (aNand_Spare_Data[9]<<8) | (aNand_Spare_Data[8]);		// add 080314 jungil 
	}

	Outp32(&NAND(Controller)->rNFSECCD, ((uSecc&0xff00)<<8)|(uSecc&0xff) );

	for(i=0; i<NAND_Inform[Controller].uSpareSize ; i++)
		*pSpareBuffer++ = aNand_Spare_Data[i];
		
	NF_nFCE_H(Controller); 

	eError = NAND_CheckECCError(Controller);

	return eError;
}

//////////
// Function Name : NAND_WritePage_4Bit
// Function Description : This function write the 1 page data of external MLC Type NAND device
// Input : 	Controller - Nand Controller Port Number 
//			uBlock - Write Block
//			uPage - Write Page
//			pBuffer - Write data buffer
// Output : 	Nand Error Type
NAND_eERROR NAND_WritePage_4Bit(u32 Controller, u32 uBlock, u32 uPage, u8 *pBuffer, u8 *aSpareBuffer)
{
	NAND_eERROR eError;

	if( (NAND_Inform[Controller].uNandType == NAND_Normal8bit) || (NAND_Inform[Controller].uNandType == NAND_Advanced8bit) )
		eError = NAND_WritePageSLC_4BitValid(Controller, uBlock, uPage, pBuffer, aSpareBuffer);
	else //if(NAND_Inform[Controller].uNandType == NAND_MLC8bit)
		eError = NAND_WritePageMLC(Controller, uBlock, uPage, pBuffer, aSpareBuffer);

	return eError;
}

NAND_eERROR NAND_WritePage_4Bit_Test(u32 Controller, u32 uBlock, u32 uPage, u8 *pBuffer, u8 *aSpareBuffer)
{
	NAND_eERROR eError;

	if( (NAND_Inform[Controller].uNandType == NAND_Normal8bit) || (NAND_Inform[Controller].uNandType == NAND_Advanced8bit) )
		eError = NAND_WritePageSLC_4BitValid_Test(Controller, uBlock, uPage, pBuffer, aSpareBuffer);
	else //if(NAND_Inform[Controller].uNandType == NAND_MLC8bit)
		eError = NAND_WritePageMLC(Controller, uBlock, uPage, pBuffer, aSpareBuffer);

	return eError;
}



NAND_eERROR NAND_WritePageSLC_4BitValid(u32 Controller, u32 uBlock, u32 uPage, u8 *pBuffer, u8 *pSpareBuffer)
 {
 	u32 i, uBlockPage, uPageSize;
	u32 uMecc0, uMecc1, uStatus;
	NAND_eERROR eError;	
	u32 *pBufferW;
	u8 *pBufferW8;
	u32 *pSpareBufferW;
	u32 uTemp0;
	u32 uSpareSize; 
	u32 uNFCONFreg, uNFCONTreg;
	u32 uOption0;
	
	pBufferW = (u32 *)pBuffer;
	pSpareBufferW = (u32 *)pSpareBuffer;

	uSpareSize = 16;	
	uBlockPage=(uBlock<<5) + uPage;
	uPageSize = 512;

	// Setting NFCONF Register.
	uNFCONFreg = Inp32(&(NAND(Controller)->rNFCONF));
//	uNFCONFreg = ( uNFCONFreg & (~(0x1<<30)) ) | (0x1<<30) ; // 0: HCLK is more than 66Mhz. 1: HCLK is less than 66Mhz
	uNFCONFreg = ( uNFCONFreg & (~(0x1<<30)) ) ; // 0: HCLK is more than 66Mhz. 1: HCLK is less than 66Mhz	
	uNFCONFreg = ( uNFCONFreg & (~(0x3<<23)) ) | (0x2<<23) ; // 4bit ECC Select.
	Outp32(&(NAND(Controller)->rNFCONF), uNFCONFreg ); // NFCONF Register setting. 
	// Setting NFCONT Register.
	uNFCONTreg = Inp32(&(NAND(Controller)->rNFCONT)); // read NFCON
	uNFCONTreg = ( uNFCONTreg &~(0x1<<18) ) |(0x1<<18) ; // 4bit ecc encoding enable. 
	uNFCONTreg &= ~((1<<13)|(1<<12));		// 4bit ECC encoding/decoding completion Interrupt disable
//	uNFCONTreg = ( uNFCONTreg & ~(0x1<<12)) | (0x1<<12);	// 4bit ECC decoding completion Interrupt Enable.
//	uNFCONTreg = ( uNFCONTreg & ~(0x1<<13)) | (0x1<<13);	// 4bit ECC Encoding completion Interrupt Enable.
	uNFCONTreg |= ( (1<<10) | (1<<9) );		// Illegal Access & RnB Transition Interrupt enable
	Outp32(&(NAND(Controller)->rNFCONT), uNFCONTreg); // write setting. NFCONT Register

	Outp32(&(NAND(Controller)->rNFSTAT), (1<<5)|(1<<4)); // Illegal Access & RnB Interrupt Pending bit clear

//	*(unsigned volatile *)0x70200028=(*(unsigned volatile *)0x70200028)|(1<<4)|(1<<7); // RnB Pending Clear, Encoding Done Clear


	// 3 Address Cycle Setting 
	#if 1 // adress setting test.
	UART_Printf("----------------------------------------------------\n");
	UART_Printf(" 0: 3 Address Cycle , 1: 4 Address Cycle                                \n");
	uOption0 = UART_GetIntNum();
	UART_Printf("----------------------------------------------------\n");
	#else
	uOption0 = 1;
	#endif


	NF_MECC_Lock(Controller); // Main ECC Lock

	NF_nFCE_L(Controller); // CE is Low

	NF_CMD(Controller, 0x80);	// Write command
	NF_ADDR(Controller, 0); 	// Address setting. 
	NF_ADDR(Controller, uBlockPage&0xff);		//
	NF_ADDR(Controller, (uBlockPage>>8)&0xff);	// Block & Page num.
	if ( uOption0 == 1) NF_ADDR(Controller, (uBlockPage>>16)&0xff);	//

	NF_MECC_UnLock(Controller);
	NF_RSTECC(Controller);    // Initialize ECC		

	for(i=0 ; i<uPageSize ; i++) 
	{
		NF_WRDATA8(Controller, *pBuffer++);
	}

	#if 0 //6410 Test
	while(!(NAND(Controller)->rNFSTAT&(1<<7))); // wait encoding done
	//	*(unsigned volatile *)0x80200004=(*(unsigned volatile *)0x80200004)|(1<<11); // MLS Stop
	*(unsigned volatile *)0x70200028 |= (1<<7); // encoding done clear
	*(unsigned volatile *)0x70200004=(*(unsigned volatile *)0x70200004)|(1<<11); // MLS Stop
	#endif

	NF_MECC_Lock(Controller);
	
	if ( NAND_Inform[Controller].uECCtest == 1)
	{
		for ( i=0; i<13; i++) 
		{
			aNand_Spare_Data[i] = *pSpareBuffer++;
		}

		for (i=0 ; i <13 ; i++) // Spare Area 14Byte Write ( 4Bit ECC : 7Byte, 8Bit ECC : 13Byte ) - Invalid Data Write
		{
			NF_WRDATA8(Controller, aNand_Spare_Data[i]);
		}

		NF_CLEAR_RnB(Controller);
		NF_CMD(Controller, 0x10);	// Read command
		NF_DETECT_RnB(Controller);
	}
	else
	{
		uMecc0 = Inp32(&NAND(Controller)->rNFMECC0);
		uMecc1 = Inp32(&NAND(Controller)->rNFMECC1); 
		aNand_Spare_Data[0] = (u8)(uMecc0&0xFF);
		aNand_Spare_Data[1] = (u8)((uMecc0&0xFF00)>>8);
		aNand_Spare_Data[2] = (u8)((uMecc0&0xFF0000)>>16);
		aNand_Spare_Data[3] = (u8)((uMecc0&0xFF000000)>>24);
		aNand_Spare_Data[4] = (u8)(uMecc1&0xFF);
		aNand_Spare_Data[5] = (u8)((uMecc1&0xFF00)>>8);
		aNand_Spare_Data[6] = (u8)((uMecc1&0xFF0000)>>16);
		aNand_Spare_Data[7] = 0xff;
		aNand_Spare_Data[8] = 0xff;
		aNand_Spare_Data[9] = 0xff;
		aNand_Spare_Data[10] = 0xff;
		aNand_Spare_Data[11] = 0xff;
		aNand_Spare_Data[12] = 0xff;
		aNand_Spare_Data[13] = 0xff;
		aNand_Spare_Data[14] = 0xff;
		aNand_Spare_Data[15] = 0xff;

		for (i=0 ; i <7 ; i++) // Spare Area 8Byte Write ( 4Bit ECC : 7Byte, 8Bit ECC : 13Byte ) - Valid Data Write
		{
			NF_WRDATA8(Controller, aNand_Spare_Data[i]);
		}

		NF_CLEAR_RnB(Controller);
		NF_CMD(Controller, 0x10);	// Read command
		NF_DETECT_RnB(Controller);
	}

	NF_CMD(Controller, 0x70);

	while(!((uStatus = NF_RDDATA8(Controller)) & (1<<6))); 	// Write Operation Status Check.

	if(uStatus & 0x01)
		eError = eNAND_ProgramError;		// Error in Program
	else
		eError = eNAND_NoError;
	
	NF_nFCE_H(Controller); 

	if ( NAND_Inform[Controller].uECCtest != 1)
	{
		for ( i = 0 ; i<16 ; i++)
		{
			*pSpareBuffer++ = aNand_Spare_Data[i];

		}
	}

	return eError;
 }

NAND_eERROR NAND_WritePageSLC_4BitValid_Test(u32 Controller, u32 uBlock, u32 uPage, u8 *pBuffer, u8 *pSpareBuffer)
 {
 	u32 i, uBlockPage, uPageSize;
	u32 uMecc0,  uMecc1;
//	u32 uSecc, uMecc2, uMecc3;
	NAND_eERROR eError;	
	u32 *pBufferW;
	u8 *pBufferW8;
	u32 *pSpareBufferW;
	u32 uTemp0;
	u32 uSpareSize; 
//	u32 uIndex, uVal , uTempMECCSpare;
	
	pBufferW = (u32 *)pBuffer;
	pSpareBufferW = (u32 *)pSpareBuffer;

	uSpareSize = 16;	
	uBlockPage=(uBlock<<5) + uPage;
	uPageSize = 512;

	*(unsigned volatile *)0x80200000=((*(unsigned volatile *)0x80200000)&~(0x3<<23))|(1<<30)|(0x2<<23); // Clock setting. and 4Bit ECC Select
	*(unsigned volatile *)0x80200004=( (*(unsigned volatile *)0x80200004)&~(1<<18) ) | (0<<11)|(1<<10)|(1<<9)|(1<<6)|(1<<0); // Init NFCONT
	*(unsigned volatile *)0x80200028=(*(unsigned volatile *)0x80200028)|(1<<4)|(1<<7); // RnB Pending Clear, Encoding Done Clear

#if 1 // Timing max setting. 7, 7, 7 
	uTemp0 = Inp32(&(NAND(Controller)->rNFCONF));
	uTemp0 = uTemp0 | ((7<<12)|(7<<8)|(7<<4));
	Outp32(&(NAND(Controller)->rNFCONF), uTemp0);	
#endif
	
	NF_MECC_Lock(Controller); // Main ECC Lock

//	NF_nFCE_L(Controller); // CE is Low

#if 0 
	NF_CMD(Controller, 0x80);	// Read command
	NF_ADDR(Controller, 0); 	// Address setting. 
	NF_ADDR(Controller, uBlockPage&0xff);		//
	NF_ADDR(Controller, (uBlockPage>>8)&0xff);	// Block & Page num.
	NF_ADDR(Controller, (uBlockPage>>16)&0xff);	//
#endif
	NF_MECC_UnLock(Controller);
	NF_RSTECC(Controller);    // Initialize ECC		

#if 0 
	NF_MECC_UnLock(Controller);
	NF_RSTECC(Controller);    // Initialize ECC	
//#else
	NF_RSTECC(Controller);    // Initialize ECC	
	NF_MECC_UnLock(Controller);
#endif

	for(i=0 ; i<uPageSize ; i++) 
	{
		NF_WRDATA8(Controller, *pBuffer++);
	}

	if ( NAND_Inform[Controller].uECCtest == 1)
	{
		for ( i=0; i<13; i++)
		{
			aNand_Spare_Data[i] = *pSpareBuffer++;
		}

		for (i=0 ; i <13 ; i++)
		{
			NF_WRDATA8(Controller, aNand_Spare_Data[i]);
		}

		while(!(NAND(Controller)->rNFSTAT&(1<<6))); // wait decoding done
		*(unsigned volatile *)0x80200028 |= (1<<6); // decoding done clear
		*(unsigned volatile *)0x80200004=(*(unsigned volatile *)0x80200004)|(1<<11); // MLS Stop

		NF_MECC_Lock(Controller);

		NF_CLEAR_RnB(Controller);
//		NF_CMD(Controller, 0x10);	// Read command
//		NF_DETECT_RnB(Controller);
	}
	else
	{
		uMecc0 = Inp32(&NAND(Controller)->rNFMECC0);
		uMecc1 = Inp32(&NAND(Controller)->rNFMECC1); 
		aNand_Spare_Data[0] = (u8)(uMecc0&0xFF);
		aNand_Spare_Data[1] = (u8)((uMecc0&0xFF00)>>8);
		aNand_Spare_Data[2] = (u8)((uMecc0&0xFF0000)>>16);
		aNand_Spare_Data[3] = (u8)((uMecc0&0xFF000000)>>24);
		aNand_Spare_Data[4] = (u8)(uMecc1&0xFF);
		aNand_Spare_Data[5] = (u8)((uMecc1&0xFF00)>>8);
		aNand_Spare_Data[6] = (u8)((uMecc1&0xFF0000)>>16);
		aNand_Spare_Data[7] = 0xff;
		aNand_Spare_Data[8] = 0xff;
		aNand_Spare_Data[9] = 0xff;
		aNand_Spare_Data[10] = 0xff;
		aNand_Spare_Data[11] = 0xff;
		aNand_Spare_Data[12] = 0xff;
		aNand_Spare_Data[13] = 0xff;
		aNand_Spare_Data[14] = 0xff;
		aNand_Spare_Data[15] = 0xff;

		for (i=0 ; i <7 ; i++)
		{
			NF_WRDATA8(Controller, aNand_Spare_Data[i]);
		}

	#if 0 
		for(i=13; i<16 ; i++)
		{
			NF_WRDATA8(Controller, aNand_Spare_Data[i]);
		}
	#endif		

		NF_CLEAR_RnB(Controller);
		NF_CMD(Controller, 0x10);	// Read command
		NF_DETECT_RnB(Controller);
	}

	eError = NAND_CheckECCError_4Bit(Controller);

	return eError;

 }




NAND_eERROR NAND_ReadPage_4Bit(u32 Controller, u32 uBlock, u32 uPage, u8 *pBuffer, u8 *pSpareBuffer)
{
	NAND_eERROR eError;
	eError = NAND_ReadPageSLC_4BitValid(Controller, uBlock, uPage, pBuffer, pSpareBuffer);	
	return eError;
}

NAND_eERROR NAND_ReadPageSLC_4BitValid(u32 Controller, u32 uBlock, u32 uPage, u8 *pBuffer, u8 *pSpareBuffer)
 {
 	u32 i, uBlockPage, uPageSize;
	NAND_eERROR eError;	
	u32 *pBufferW;
	u32 uSpareSize;
	u32 uNFCONTreg, uNFCONFreg;
	u32 uOption0;

	for(i=0; i<7 ; i++)
	{
		aNand_Spare_Data[i] = 0x0;
	}
	
	pBufferW = (u32 *)pBuffer;

	uSpareSize = 16;	
	uBlockPage=(uBlock<<5) + uPage;
	uPageSize = 512;

	// Setting NFCONF Register.
	uNFCONFreg = Inp32(&(NAND(Controller)->rNFCONF));
	uNFCONFreg = uNFCONFreg & (~(0x1<<30)); // HCLK is more than 66Mhz.
	uNFCONFreg = ( uNFCONFreg & (~(0x3<<23)) ) | (0x2<<23) ; // 4bit ECC Select.
	Outp32(&(NAND(Controller)->rNFCONF), uNFCONFreg ); // NFCONF Register setting. 
	// Setting NFCONT Register.
	uNFCONTreg = Inp32(&(NAND(Controller)->rNFCONT)); // read NFCON
	uNFCONTreg = ( uNFCONTreg &~(0x1<<18) ) ; // 4bit ecc Decoding enable. 
//	uNFCONTreg &= ~((1<<13)|(1<<12));		// 4bit ECC encoding/decoding completion Interrupt disable
	uNFCONTreg = ( uNFCONTreg & ~(0x1<<12)) | (0x1<<12);	// 4bit ECC decoding completion Interrupt Enable.
	uNFCONTreg |= (1<<10) | (1<<9);		// Illegal Access & RnB Transition Interrupt enable
	Outp32(&(NAND(Controller)->rNFCONT), uNFCONTreg); // write setting. NFCONT Register

	Outp32(&(NAND(Controller)->rNFSTAT), (1<<5)|(1<<4)); // Illegal Access & RnB Interrupt Pending bit clear

	// 3 Address Cycle Setting 
	#if 1 // adress setting test.
	UART_Printf("----------------------------------------------------\n");
	UART_Printf(" 0: 3 Address Cycle , 1: 4 Address Cycle                                \n");
	uOption0 = UART_GetIntNum();
	UART_Printf("----------------------------------------------------\n");
	#else
	uOption0 = 1;
	#endif

	NF_MECC_Lock(Controller);

	NF_nFCE_L(Controller);
	NF_CLEAR_RnB(Controller);

	NF_CMD(Controller, 0x00);	// Read command
	NF_ADDR(Controller, 0); 	
	NF_ADDR(Controller, uBlockPage&0xff);		//
	NF_ADDR(Controller, (uBlockPage>>8)&0xff);	// Block & Page num.
	if ( uOption0 == 1 ) NF_ADDR(Controller, (uBlockPage>>16)&0xff);	//

	NF_DETECT_RnB(Controller);
	NF_CLEAR_RnB(Controller);
	
	NF_MECC_UnLock(Controller);
	NF_RSTECC(Controller);    // Initialize ECC

	#if 0  // Read Data at Byte size
	for(i=0 ; i<uPageSize ; i++) 
	{
		*pBuffer++ = NF_RDDATA8(Controller);// Read one page
	}
	#else // Read Data at word size
	for (i=0; i<uPageSize/4; i++)
	{
		*pBufferW++ = NF_RDDATA(Controller);	// Read one page
	}
	#endif

	#if 1 // Byte Read 
	for(i=0; i<7 ; i++)	// Spare data read : ECC Data Read ( 4Bit ECC : 7Byte)
	{
		aNand_Spare_Data[i] = NF_RDDATA8(Controller);
	}
	#else // Word Read
	for(i=0; i<uSpareSize/4; i++)
	{
		aNand_Spare_Data_u32[i] = NF_RDDATA(Controller);
	}
	#endif 

	NF_MECC_Lock(Controller);

	while(!(NAND(Controller)->rNFSTAT&(1<<6))); 	// Check decoding done 
	Outp32(&(NAND(Controller)->rNFSTAT), (1<<6) ); // Clear Decodginf done

	NF_nFCE_H(Controller); 

	for (i=0 ; i <16 ; i++)
	{
		*pSpareBuffer++ = aNand_Spare_Data[i];
	}

	eError = NAND_CheckECCError_4Bit(Controller);

	return eError;

 }


