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
} 
oNAND_REGS;

static void *NAND_pBase[NAND_CONNUM];
static u8 aNand_Spare_Data[NAND_SPARE_MAX] = {	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
												0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
												0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
												0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
u8 aNand_Spare_Data_Temp[NAND_SPARE_MAX] = {	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
												0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
												0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
												0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
static volatile u32 g_Nand_RnBTransition, g_Nand_4bitEccEncDone, g_Nand_4bitEccDecDone, g_Nand_IllegalAccError;
NAND_oInform NAND_Inform[NAND_CONNUM];
NAND_oEccError NAND_EccError;

#if (NAND_TRANSFER_MODE == DMA_TRANSFER)
	static volatile u32 Nand_DmaDone;
#endif
	
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

		uTemp = Inp32(&(NAND(Controller)->rNFCONF));
		if(g_HCLK  > 66000000)
			uTemp &= ~(1<<30);
		else
			uTemp |= (1<<30);
		Outp32(&(NAND(Controller)->rNFCONF), uTemp);
	}
	else
	{
		return FALSE;
	}		

	NAND_Reset(Controller);
	
#if NAND_TRANSFER_MODE == DMA_TRANSFER
		DMAC_InitCh(DMA1, DMA_ALL, &g_oNandDmac1);
		INTC_SetVectAddr(NUM_DMA1,  NAND_DmaISR);
		SYSC_SelectDMA(eSEL_PWM, 1);
		INTC_Enable(NUM_DMA1);
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
	//INT_Disable1(BIT_INT_NFC);
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
		UART_Printf("NAND Illegal Access Error.........!!\n");
		UART_Getc();
	}
	else if(uIntSource & NAND_RnB_TRANS)
	{
		Outp32(&NAND(0)->rNFSTAT, (1<<4));
		g_Nand_RnBTransition = 1;
	}

	// rb1004 : must be modified
	//INT_Enable1(BIT_INT_NFC);
	//Write_VECTADDR(0x0);	
	INTC_Enable(NUM_NFC);
	INTC_ClearVectAddr();
}


//////////
// Function Name : NAND_DmaISR
// Function Description : Nand DMA transfer IRQ routine
// Input : 	None
// Output : 	None
#if (NAND_TRANSFER_MODE == DMA_TRANSFER)
void __irq NAND_DmaISR(void)
{
  	DMACH_ClearIntPending(&g_oNandDmac1);

	//UART_Printf ("DMA ISR %d\n", Nand_DmaDone);

	Nand_DmaDone = 1;
	INTC_ClearVectAddr();
}
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

	pBufferW = (u32 *)pBuffer;
	
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

	NF_MECC_Lock(Controller);

	if(NAND_Inform[Controller].uNandType == NAND_Normal8bit)
	{
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
		
	}
	else if(NAND_Inform[Controller].uNandType == NAND_Advanced8bit)
	{	
		aNand_Spare_Data[0] = NF_RDDATA8(Controller);		// Invalid Block Check data

		NF_SECC_UnLock(Controller);
		
		// Main Area ECC Check
		uMecc = NF_RDDATA(Controller);

		if(NAND_Inform[Controller].uSpareECCtest == 0)
		{
			Outp32(&NAND(Controller)->rNFMECCD0, ((uMecc&0xff00)<<8)|(uMecc&0xff) );
			Outp32(&NAND(Controller)->rNFMECCD1, ((uMecc&0xff000000)>>8)|((uMecc&0xff0000)>>16) );
		}
		
		NF_SECC_Lock(Controller);

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

	// Spare Area ECC Check
	uSecc = (aNand_Spare_Data[9]<<8) | (aNand_Spare_Data[8]);
	
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
		if(NAND_Inform[Controller].uPerformanceCheck == 0)
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
		if(NAND_Inform[Controller].uPerformanceCheck == 0)
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
		if(NAND_Inform[Controller].uPerformanceCheck == 0)
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

	//for(i=22;i<29;i++) 
	for(i=22;i<36;i++) 
	{
		aNand_Spare_Data[i] = NF_RDDATA8(Controller);	
		if(NAND_Inform[Controller].uPerformanceCheck == 0)
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
			
	pBufferW = (u32 *)pBuffer;			
	g_Nand_RnBTransition = 0;

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
	}
	else if(NAND_Inform[Controller].uNandType == NAND_Advanced8bit)
	{	
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
	}

	while(!g_Nand_RnBTransition)
	{
		if(g_Nand_IllegalAccError == 1)
		{
			NAND_Reset(Controller);
			g_Nand_IllegalAccError = 0;
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

	//NF_MECC_Lock(Controller);

//EVT1 NFCONT[13] bit has no read path....Not check ECC encoding completion
//	while(!g_Nand_4bitEccEncDone);

	uMecc0 = Inp32(&(NAND(Controller)->rNFMECC0));
	uMecc1 = Inp32(&(NAND(Controller)->rNFMECC1)) & 0xFFFFFF;

	aNand_Spare_Data[1]=(u8)(uMecc0&0xff);
	aNand_Spare_Data[2]=(u8)((uMecc0>>8) & 0xff);
	aNand_Spare_Data[3]=(u8)((uMecc0>>16) & 0xff);
	aNand_Spare_Data[4]=(u8)((uMecc0>>24) & 0xff);
	aNand_Spare_Data[5]=(u8)(uMecc1&0xff);
	aNand_Spare_Data[6]=(u8)((uMecc1>>8) & 0xff);
	aNand_Spare_Data[7]=(u8)((uMecc1>>16) & 0xff);	

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

//EVT1 NFCONT[13] bit has no read path....Not check ECC encoding completion
//	while(!g_Nand_4bitEccEncDone);

	uMecc0 = Inp32(&(NAND(Controller)->rNFMECC0));
	uMecc1 = Inp32(&(NAND(Controller)->rNFMECC1)) & 0xFFFFFF;

	aNand_Spare_Data[8]=(u8)(uMecc0&0xff);
	aNand_Spare_Data[9]=(u8)((uMecc0>>8) & 0xff);
	aNand_Spare_Data[10]=(u8)((uMecc0>>16) & 0xff);
	aNand_Spare_Data[11]=(u8)((uMecc0>>24) & 0xff);
	aNand_Spare_Data[12]=(u8)(uMecc1&0xff);
	aNand_Spare_Data[13]=(u8)((uMecc1>>8) & 0xff);
	aNand_Spare_Data[14]=(u8)((uMecc1>>16) & 0xff);
	
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

//EVT1 NFCONT[13] bit has no read path....Not check ECC encoding completion
//	while(!g_Nand_4bitEccEncDone);

	uMecc0 = Inp32(&(NAND(Controller)->rNFMECC0));
	uMecc1 = Inp32(&(NAND(Controller)->rNFMECC1)) & 0xFFFFFF;

	aNand_Spare_Data[15]=(u8)(uMecc0&0xff);
	aNand_Spare_Data[16]=(u8)((uMecc0>>8) & 0xff);
	aNand_Spare_Data[17]=(u8)((uMecc0>>16) & 0xff);
	aNand_Spare_Data[18]=(u8)((uMecc0>>24) & 0xff);
	aNand_Spare_Data[19]=(u8)(uMecc1&0xff);
	aNand_Spare_Data[20]=(u8)((uMecc1>>8) & 0xff);
	aNand_Spare_Data[21]=(u8)((uMecc1>>16) & 0xff);
	
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

//EVT1 NFCONT[13] bit has no read path....Not check ECC encoding completion
//	while(!g_Nand_4bitEccEncDone);

	uMecc0 = Inp32(&(NAND(Controller)->rNFMECC0));
	uMecc1 = Inp32(&(NAND(Controller)->rNFMECC1)) & 0xFFFFFF;

	aNand_Spare_Data[22]=(u8)(uMecc0&0xff);
	aNand_Spare_Data[23]=(u8)((uMecc0>>8) & 0xff);
	aNand_Spare_Data[24]=(u8)((uMecc0>>16) & 0xff);
	aNand_Spare_Data[25]=(u8)((uMecc0>>24) & 0xff);
	aNand_Spare_Data[26]=(u8)(uMecc1&0xff);
	aNand_Spare_Data[27]=(u8)((uMecc1>>8) & 0xff);
	aNand_Spare_Data[28]=(u8)((uMecc1>>16) & 0xff);

	//////////////////////////
	// Spare Area Write
	//////////////////////////

#if (MLC_SPARE_ECC == 1)	
	{
		u32 uSecc0, uSecc1;
		
		g_Nand_4bitEccEncDone = 0;

		NF_RSTECC(Controller);
		NF_MECC_UnLock(Controller);
#if (NAND_REG_BUG == TRUE)
		uTemp = NAND_ReadNFCONRegister(Controller);
		uTemp |= (1<<12)|(1<<13);		// 4bit ECC decoding/encoding Interrupt enable...because error
		Outp32(&(NAND(Controller)->rNFCONT), uTemp);	
#endif	

		for(i=0 ; i<29 ; i++)
			NF_WRDATA8(Controller, aNand_Spare_Data[i]);

		NF_nFCE_H(Controller);
#if (NAND_REG_BUG == TRUE)
		uTemp = NAND_ReadNFCONRegister(Controller);
		uTemp |= (1<<12)|(1<<13);		// 4bit ECC decoding/encoding Interrupt enable...because error
		Outp32(&(NAND(Controller)->rNFCONT), uTemp);	
#endif	

		for(i=29 ; i<NAND_MLC_TRANS_SIZE ; i++)
			NF_WRDATA8(Controller, 0xFF);

//EVT1 NFCONT[13] bit has no read path....Not check ECC encoding completion
//	while(!g_Nand_4bitEccEncDone);

		uSecc0 = Inp32(&(NAND(Controller)->rNFMECC0));
		uSecc1 = Inp32(&(NAND(Controller)->rNFMECC1)) & 0xFFFFFF;

		aNand_Spare_Data[29]=(u8)(uSecc0&0xff);
		aNand_Spare_Data[30]=(u8)((uSecc0>>8) & 0xff);
		aNand_Spare_Data[31]=(u8)((uSecc0>>16) & 0xff);
		aNand_Spare_Data[32]=(u8)((uSecc0>>24) & 0xff);
		aNand_Spare_Data[33]=(u8)(uSecc1&0xff);
		aNand_Spare_Data[34]=(u8)((uSecc1>>8) & 0xff);
		aNand_Spare_Data[35]=(u8)((uSecc1>>16) & 0xff);		

		NF_nFCE_L(Controller);
#if (NAND_REG_BUG == TRUE)
		uTemp = NAND_ReadNFCONRegister(Controller);
		uTemp |= (1<<12)|(1<<13);		// 4bit ECC decoding/encoding Interrupt enable...because error
		Outp32(&(NAND(Controller)->rNFCONT), uTemp);	
#endif	

		for(i=29 ; i<uSpareSize ; i++)
			NF_WRDATA8(Controller, aNand_Spare_Data[i]);
	}
#else
	for(i=0 ; i<uSpareSize ; i++)
		NF_WRDATA8(Controller, aNand_Spare_Data[i]);
#endif

	NF_CMD(Controller, 0x10);	// 2nd Write Command

	while(!g_Nand_RnBTransition)
	{
		if(g_Nand_IllegalAccError == 1)
		{
			NAND_Reset(Controller);
			g_Nand_IllegalAccError = 0;
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
	INTC_Enable(NUM_NFC);

	//Invalid Block Check
	eError = NAND_CheckInvalidBlock(Controller, uBlock);
	if(eError != eNAND_NoError)
		return eError;

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
			NAND_Reset(Controller);
			g_Nand_IllegalAccError = 0;
			return eNAND_EraseError;
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
	//INT_Disable1(BIT_INT_NFC);
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

	uTemp = NAND_ReadNFCONRegister(Controller);
	uTemp |= (1<<10) | (1<<9);		// Illegal Access & RnB Transition Interrupt enable
	Outp32(&(NAND(Controller)->rNFCONT), uTemp);
	
	Outp32(&(NAND(Controller)->rNFSTAT), (1<<5)|(1<<4));		// Illegal Access & RnB Interrupt Pending bit clear
	INTC_Enable(NUM_NFC);	

	//Invalid Block Check
	for(i=uBlock ; i<(uBlock+uBlockNum-1) ; i++)
	{
		eError = NAND_CheckInvalidBlock(Controller, i);
		if(eError != eNAND_NoError)
			return eError;
	}

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

		while(!g_Nand_RnBTransition)
		{
			if(g_Nand_IllegalAccError == 1)
			{
				NAND_Reset(Controller);
				g_Nand_IllegalAccError = 0;
				NF_nFCE_H(Controller);
				uTemp = NAND_ReadNFCONRegister(Controller);
				uTemp &= ~((1<<10)|(1<<9));		// Illegal Access & RnB Transition Interrupt disable
				Outp32(&(NAND(Controller)->rNFCONT), uTemp);

				INTC_Disable(NUM_NFC);				
				return eNAND_EraseError;
			}
		}
		
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
	
		//if (NF_RDDATA8(Controller)&0x1) 						// Erase error check
		//{
		//	eError = eNAND_EraseError;
		//	return eError;
		//}
		//else 
		//	eError = eNAND_NoError;
		   
		NF_nFCE_H(Controller);
	}

	uTemp = NAND_ReadNFCONRegister(Controller);
	uTemp &= ~((1<<10)|(1<<9));		// Illegal Access & RnB Transition Interrupt disable
	Outp32(&(NAND(Controller)->rNFCONT), uTemp);

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
		Outp32(&(NAND(Controller)->rNFEBLK), (uEndBlock+1)<<5);
	}
	else if(NAND_Inform[Controller].uNandType == NAND_Advanced8bit)
	{
		Outp32(&(NAND(Controller)->rNFSBLK), uStartBlock<<6);
		Outp32(&(NAND(Controller)->rNFEBLK), (uEndBlock+1)<<6);
	}
	else if(NAND_Inform[Controller].uNandType == NAND_MLC8bit)
	{
		Outp32(&(NAND(Controller)->rNFSBLK), uStartBlock<<7);
		Outp32(&(NAND(Controller)->rNFEBLK), (uEndBlock+1)<<7);
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
		Outp32(&(NAND(Controller)->rNFEBLK), (uEndBlock+1)<<5);
	}
	else if(NAND_Inform[Controller].uNandType == NAND_Advanced8bit)
	{
		Outp32(&(NAND(Controller)->rNFSBLK), uStartBlock<<6);
		Outp32(&(NAND(Controller)->rNFEBLK), (uEndBlock+1)<<6);
	}
	else if(NAND_Inform[Controller].uNandType == NAND_MLC8bit)
	{
		Outp32(&(NAND(Controller)->rNFSBLK), uStartBlock<<7);
		Outp32(&(NAND(Controller)->rNFEBLK), (uEndBlock+1)<<7);
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
#if 0	
	if(uErrorType == NAND_ECCERR_1BIT)
	{
		pErrorBuffer[100] ^= 0x01;		// 1st bit toggle, then generate the 1bit ecc error		
	}
	else if(uErrorType == NAND_ECCERR_MULTI)
	{
		pErrorBuffer[100] ^= 0x01;		// 1st bit toggle, then generate the 1bit ecc error		
		pErrorBuffer[200] ^= 0x01;		// 1st bit toggle, then generate the 1bit ecc error			
	}
#else
	if(uErrorType == NAND_ECCERR_1BIT)
	{
		pErrorBuffer[NAND_EccError.uEccErrByte1] ^= (1<<NAND_EccError.uEccErrBit1);		
	}
	else if(uErrorType == NAND_ECCERR_MULTI)
	{
		pErrorBuffer[NAND_EccError.uEccErrByte1] ^= (1<<NAND_EccError.uEccErrBit1);	
		pErrorBuffer[NAND_EccError.uEccErrByte2] ^= (1<<NAND_EccError.uEccErrBit2);	
	}
#endif

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

	for(i=0 ; i<uPageSize/4 ; i++) 
	{
		NF_WRDATA(Controller, *pBuffer++);		// Write one page
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
	//INT_Disable1(BIT_INT_NFC);
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
	u32 uDataPos[5]={0, }, uSparePos[5]={0, };
	u32 uMLCECCErrBytePos[5]={0, };
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
	else //if(uErrorType == NAND_ECCERR_UNCORRECT)
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
			NAND_Reset(Controller);
			g_Nand_IllegalAccError = 0;
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
	//INT_Disable1(BIT_INT_NFC);
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
			NAND_Reset(Controller);
			g_Nand_IllegalAccError = 0;
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
			NAND_Reset(Controller);
			g_Nand_IllegalAccError = 0;
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



