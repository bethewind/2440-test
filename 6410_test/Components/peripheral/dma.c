/**************************************************************************************
* 
*	Project Name : S3C6400 Validation
*
*	Copyright 2006 by Samsung Electronics, Inc.
*	All rights reserved.
*
*	Project Description :
*		This software is only for validating functions of the S3C6400.
*		Anybody can use this software without our permission.
*  
*--------------------------------------------------------------------------------------
* 
*	File Name : Dma.c
*  
*	File Description : This file implements the API functons for OneNand controller.
*
*	Author : Wonjoon Jang
*	Dept. : AP Development Team
*	Created Date : 2006/12/29
*	Version : 0.1 
* 
*	History
*	- Created(wonjoon.jang 2006/12/29)
*  
**************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "def.h"
#include "option.h"
#include "library.h"
#include "sfr6410.h"
#include "system.h"
#include "sysc.h"
#include "dma.h"
#include "intc.h"

enum DMA_REG
{
	DMA_INT 			=  0x000, 	//[7:0] RO Interrupt status
	DMA_INTTC			=  0x004, 	//[7:0] RO Interrupt terminal count status
	DMA_INTTCC			=  0x008, 	//[7:0] WO Interrupt terminal count clear 
	DMA_INTERR			=  0x00C, 	//[7:0] RO Interrupt error status
	DMA_INTERRC		=  0x010, 	//[7:0] WO Interrupt error clear
	DMA_RAWINTTC		=  0x014, 	//[7:0] RO Raw interrupt terminal count state
	DMA_RAWINTERR	=  0x018,	//[7:0] RO Raw Interrupt error status
	DMA_ENBLDCH 		=  0x01C,	//[7:0] RO enabled channels
	DMA_SOFTBREQ		=  0x020,	//[15:0] RW Software Burst Request 16peripheral
	DMA_SOFTSREQ		=  0x024, 	//[15:0] RW Software Single Request 16peripheral
	DMA_SOFTLBREQ		=  0x028, 	//[15:0] RW Software Last Burst Request 16peripheral 
	DMA_SOFTLSREQ		=  0x02C,	//[15:0] RW Software Last Single Request 16peripheral  
	DMA_CONFIG 		=  0x030, 
									// [2] M2 AHB Master 2 endianness configuration 0=little,1=big
									// [1] M1 AHB Master 2 endianness configuration 0=little,1=big
									// [0] DMAC enable 0=disabled,1=enabled
	DMA_SYNC			=  0x034,
	DMA_CH0			=  0x100,
	DMA_CH1			=  0x120,
	DMA_CH2			=  0x140,
	DMA_CH3			=  0x160,
	DMA_CH4			=  0x180,
	DMA_CH5			=  0x1A0,
	DMA_CH6			=  0x1C0,
	DMA_CH7			=  0x1E0
};


enum DMA_CH_REG 
{
	DMA_CH_SRCADDR	= 0x00,
	DMA_CH_DSTADDR 	= 0x04,
	DMA_CH_LLI			= 0x08,
								//[31:2] Linked list item Address bits[1:0] are 0
								//[1] Undefined
								//[0] AHB master select next LLI 0=M1 1=M2
	DMA_CH_CONTROL0	= 0x0C,
	DMA_CH_CONTROL1	= 0x10,
	DMA_CH_CONFIG		= 0x14,
								//[18]   RW Halt 0=enable DMA requests 1=ignore extra source DMA
								//[17]   RO  0=FIFO empty 1=FIFO has data
								//[16]   RW Lock
								//[15]   RW Terminal count interrupt mask
								//[14]   RW Interrupt error mask
								//[13:11]RW FlowCntrl
								//[9:6]  RW DestPeripheral
								//[4:1]  RW SrcPeripheral
								//[0]    RW Channel enable
	DMA_CH_CONFIGEXP	= 0x18	
								//[2:1]	RW 00=Modem TX0 Request, 01 TX1 Request, 10 RX0 Request, 11 RX1 Request
								//[0]	RW Enable for Peri. DMA Request  0=Disable  1=Enable
};

// DMA Controller Register
#define DmaOutp32(offset, value) Outp32(sCh->m_uBaseAddr+offset, value)
#define DmaInp32(offset) Inp32(sCh->m_uBaseAddr+offset)
// DMA Channel Register
#define DmaChOutp32(offset, value) Outp32(sCh->m_uBaseAddr+sCh->m_uChAddr+offset, value)
#define DmaChInp32(offset) Inp32(sCh->m_uBaseAddr+sCh->m_uChAddr+offset)


//////////
// Function Name : DMAC_InitCh
// Function Description : This function initializes a certain DMA Controller
// Input : 	eUnit - DMA Controller Number,
//			eCh  - Used DMA Channel, DMA_A ~ DMA_H
// Output : 	void
//			
// Version : v0.1

void DMAC_InitCh(DMA_UNIT eUnit, DMA_CH eCh, DMAC *sCh)
{
	u32 x;

	if ( eUnit == DMA0 )
		sCh->m_uBaseAddr = DMA0_BASE;
	else if ( eUnit == DMA1 )
		sCh->m_uBaseAddr = DMA1_BASE;
	else if ( eUnit == SDMA0 )
		sCh->m_uBaseAddr = SDMA0_BASE;
	else if ( eUnit == SDMA1 )
		sCh->m_uBaseAddr = SDMA1_BASE;
	else
		Assert(0);

	DmaOutp32(DMA_CONFIG, 0x1);	// DMAC enabled

	x = eCh;

	DmaOutp32(DMA_INTTCC, x);		//clear pending
	DmaOutp32(DMA_INTERRC, x); 		//clear pending
}

//////////
// Function Name : DMAC_Close
// Function Description : This function close a certain DMA Controller
// Input : 	eUnit - DMA Controller Number,
//			eCh  - Used DMA Channel, DMA_A ~ DMA_H
// Output : 	void
//			
// Version : v0.1

void DMAC_Close(DMA_UNIT eUnit, DMA_CH eCh, DMAC *sCh)
{
	u32 x;

	if ( eUnit == DMA0 )
		sCh->m_uBaseAddr = DMA0_BASE;
	else if ( eUnit == DMA1 )
		sCh->m_uBaseAddr = DMA1_BASE;
	else if ( eUnit == SDMA0 )
		sCh->m_uBaseAddr = SDMA0_BASE;
	else if ( eUnit == SDMA1 )
		sCh->m_uBaseAddr = SDMA1_BASE;
	else
		Assert(0);

	DmaOutp32(DMA_CONFIG, 0x0);	// DMAC enabled

	x = eCh;

	DmaOutp32(DMA_INTTCC, x);		//clear pending
	DmaOutp32(DMA_INTERRC, x); 		//clear pending
}

//////////
// Function Name : DMAC_IsTransferDone
// Function Description : This function check the transferring datas are done.  
// Input : 	*sCh  : DMAC, Enabled Channel address
// Output : 	true   :  Transfer is done
//			false  :  Tranfser is not done
// Version : v0.1
u8 DMACH_IsTransferDone(DMAC *sCh)
{
	u32 x;
	u8 res;

	x = DmaChInp32(DMA_CH_CONFIG);

	if ( (x&0x1) == 0 )
		res = true;
	else
		res = false;

	return res;
}

u32 DMAC_IntStatus(DMAC *sCh)
{
	return DmaInp32(DMA_INT);
}


u32 DMACH_Configuration(DMAC *sCh)
{

	return DmaChInp32(DMA_CH_CONFIG);	
}


//////////
// Function Name : DMAC_Stop
// Function Description :  Stop DMA Operation.
// Input : 	*sCh  : DMAC , Enabled Channel address
// Output : 	void
//			
// Version : v0.1
void DMACH_Stop(DMAC *sCh)
{
	u32 uReg;
	
	uReg = DmaChInp32(DMA_CH_CONFIG);
	
	uReg &= ~(1<<0);
	DmaChOutp32(DMA_CH_CONFIG, uReg);
}

//////////
// Function Name : DMACH_GetChannelNumber
// Function Description : This function get dma channel number.
// Input : 	*sCh  : DMAC , Enabled Channel address
// Output : 	Channel status.
//			
// Version : v0.1
DMA_CH DMACH_GetChannelNumber(DMAC *sCh)
{
	u32 uRead;

	uRead = DmaInp32(DMA_INT);

	return (DMA_CH)uRead;
}

//////////
// Function Name : DMAC_ClearIntPending
// Function Description : This function clear the dma TC interrupt pending register.  
// Input : 	*sCh  : DMAC , Enabled Channel address
// Output : 	void
//			
// Version : v0.1
void DMACH_ClearIntPending(DMAC *sCh)
{
	u32 uRead;

       uRead = 	DmaInp32(DMA_INTTC);       
       
	DmaOutp32(DMA_INTTCC, uRead);	
	
}

void DMACH_ClearIntPendingrb1004(DMAC *sCh, u32 uPending)
{
	DmaOutp32(DMA_INTTCC, uPending);	
}

//////////
// Function Name : DMAC_ClearErrIntPending
// Function Description : This function clear the dma Err interrupt pending register.  
// Input : 	*sCh  : DMAC , Enabled Channel address
// Output : 	void
//			
// Version : v0.1
void DMACH_ClearErrIntPending(DMAC *sCh)
{
	u32 uRead;

       uRead = 	DmaInp32(DMA_INTERR);     
       
	DmaOutp32(DMA_INTERRC, uRead);
}

//////////
// Function Name : DMAC_Setup
// Function Description : Set Channel information 
// Input : 	eCh  		: Selected channel.
//			uLLIAddr        : Linked List Item Address
//						: not using LLI => Write 0 (Default)
//			uSrcAddr		: Source Address
//			bSrcFixed	: "1" Source Fixed. "0" Source Increment. 
//			uDstAddr        : Destination Address
//			bDstFixed       : "1" Destination Fixed.  "0" Destination Increment.
//			eDataSz         : Transfer Width => BYTE/HWORD(Half-Word/WORD
//			uDataCnt		: Transfer Size [24:0] => 1 ~ 0x200_0000
//			eOpMode        : not using in pl080, DEMAND
//			eSrcReq&eDstReq : Peripheral Request Source,   Memory => MEM
//							[DMAC0, SDMAC0]
//    						DMA0_UART0_0, DMA0_UART0_1, DMA0_UART1_0, DMA0_UART1_1,
//							DMA0_UART2_0, DMA0_UART2_1, DMA0_UART3_0, DMA0_UART3_1,
//							DMA0_PCM0_TX,DMA0_PCM0_RX, DMA0_I2S0_TX, DMA0_I2S0_RX,
//							DMA0_SPI0_TX, DMA0_SPI0_RX,  DMA0_HSI_I2SV40_TX,   DMA0_HSI_I2SV40_RX,
//							DMA0_MODEM_TX, DMA0_MODEM_RX
//							[DMAC1, SDMAC1]
//							DMA1_PCM1_TX, DMA1_PCM1_RX, DMA1_I2S1_TX, DMA1_I2S1_RX,
//							DMA1_SPI1_TX,  DMA1_SPI1_RX,  DMA1_AC_PCMout, DMA1_AC_PCMin,
//							DMA1_AC_MICin, DMA1_PWM, DMA1_IrDA, DMA1_EXTERNAL, DMA1_GIB
//							DMA1_MODEM_TX, DMA1_MODEM_RX
//							SDMA1_SECU_RX, SDMA1_SEXU_TX     => Only SDMAC1
//			eBurstMode	: Burst Size,  
//							SINGLE, BURST4, BURST8, BURST16, BURST32, BURST64, BURST128, BURST256
//
// Output : 	void
//			
// Version : v0.1

void DMACH_Setup(
	DMA_CH eCh, u32 uLLIAddr, u32 uSrcAddr, u8 bSrcFixed, u32 uDstAddr, u8 bDstFixed, DATA_SIZE eDataSz, u32 uDataCnt,
	DMA_HS_MODE eOpMode, DREQ_SRC eSrcReq, DREQ_SRC eDstReq, BURST_MODE eBurstMode, DMAC *sCh)
{
	u32 uReg=0;
	u32 uBurstSz=0;
	u32 uTrWidth=0;
	u32 uTCEnable=0;
	DMA_AHB eSrcAhb=0, eDstAhb=0;


	switch(eCh)
	{
		case DMA_A: sCh->m_uChAddr = DMA_CH0;  break;
		case DMA_B: sCh->m_uChAddr = DMA_CH1;  break;
		case DMA_C: sCh->m_uChAddr = DMA_CH2;  break;
		case DMA_D: sCh->m_uChAddr = DMA_CH3;  break;
		case DMA_E: sCh->m_uChAddr = DMA_CH4;  break;
		case DMA_F: sCh->m_uChAddr = DMA_CH5;  break;
		case DMA_G: sCh->m_uChAddr = DMA_CH6;  break;
		case DMA_H: sCh->m_uChAddr = DMA_CH7;  break;
		default : Assert(0);
	}

	Assert(uDataCnt < 0x2000000);
	

//-------Modem I/F Mem to Mem setting Register----------//
	switch(eSrcReq)
	{
		case DMA0_MODEM_RX: uReg = 0x5; break;
		case DMA0_MODEM_TX: uReg = 0x1; break;
		case DMA1_MODEM_RX: uReg = 0x7; break;
		case DMA1_MODEM_TX: uReg = 0x3; break;
		default : uReg = 0x0;	break;
	}	

	if(uReg == 0x0)
	{
		switch(eDstReq)
		{
			case DMA0_MODEM_RX: uReg = 0x5; break;
			case DMA0_MODEM_TX: uReg = 0x1; break;
			case DMA1_MODEM_RX: uReg = 0x7; break;
			case DMA1_MODEM_TX: uReg = 0x3; break;
			default : uReg = 0x0;	break;
		}	
	}
	
	DmaChOutp32(DMA_CH_CONFIGEXP, uReg);	
//--------------------------------------------------//

//-------For Mem to Mem Transfer setting by Modem I/F --------//	
	if((eSrcReq == DMA0_MODEM_RX) || (eSrcReq == DMA0_MODEM_TX)|| (eSrcReq == DMA1_MODEM_RX)||(eSrcReq == DMA1_MODEM_TX))
		eSrcReq = DMA_MODEM;
	if((eDstReq == DMA0_MODEM_RX) || (eDstReq == DMA0_MODEM_TX)|| (eDstReq == DMA1_MODEM_RX)||(eDstReq == DMA1_MODEM_TX))
		eDstReq = DMA_MODEM;
//-------------------------------------------------------//

	
	

#if 0
	if((eSrcReq == MEM) || (eSrcReq == DMA0_OND_TX))
		eSrcAhb = AHB_MASTER1;
	else if(eSrcReq == DMA1_NAND_TX)
		eSrcAhb = AHB_MASTER2;
	else
		eSrcAhb = AHB_MASTER2;

	if((eDstReq == MEM) || (eDstReq == DMA0_OND_RX))
		eDstAhb = AHB_MASTER1;
	else if(eDstReq == DMA1_NAND_RX)
		eDstAhb = AHB_MASTER2;	
	else
		eDstAhb = AHB_MASTER2;	
#else
/*									// 6400 code
	if((eSrcReq == MEM) || (eSrcReq == DMA0_OND_TX)|| (eSrcReq == DMA1_EXTERNAL))
		eSrcAhb = AHB_MASTER1;
	else if(eSrcReq == DMA1_NAND_TX)
		eSrcAhb = AHB_MASTER2;
	else
		eSrcAhb = AHB_MASTER2;

	if((eDstReq == MEM) || (eDstReq == DMA0_OND_RX)|| (eDstReq == DMA1_EXTERNAL))
		eDstAhb = AHB_MASTER1;
	else if(eDstReq == DMA1_NAND_RX)
		eDstAhb = AHB_MASTER2;	
	else
		eDstAhb = AHB_MASTER2;	
*/								
	if((eSrcReq == MEM) || (eSrcReq == DMA0_OND_TX)|| (eSrcReq == DMA1_EXTERNAL)||(eSrcReq == DMA_MODEM))
		eSrcAhb = AHB_MASTER1;
	else if(eSrcReq == DMA1_NAND_TX)
		eSrcAhb = AHB_MASTER2;
	else
		eSrcAhb = AHB_MASTER2;

	if((eDstReq == MEM) || (eDstReq == DMA0_OND_RX)|| (eDstReq == DMA1_EXTERNAL)||(eDstReq == DMA_MODEM))
		eDstAhb = AHB_MASTER1;
	else if(eDstReq == DMA1_NAND_RX)
		eDstAhb = AHB_MASTER2;	
	else
		eDstAhb = AHB_MASTER2;	

#endif

	DmaChOutp32(DMA_CH_SRCADDR,	uSrcAddr);
	DmaChOutp32(DMA_CH_DSTADDR, uDstAddr);

	switch(eDataSz)
	{
		case WORD   : uTrWidth = ((2<<21)|(2<<18));break;
		case HWORD  : uTrWidth = ((1<<21)|(1<<18));break;
		case BYTE   : uTrWidth = ((0<<21)|(0<<18));break;
		default : Assert(0);
	}

	switch(eBurstMode)
	{
		case SINGLE    : uBurstSz = ((0<<15)|(0<<12));break;
		case BURST4    : uBurstSz = ((1<<15)|(1<<12));break;
		case BURST8    : uBurstSz = ((2<<15)|(2<<12));break;
		case BURST16   : uBurstSz = ((3<<15)|(3<<12));break;
		case BURST32   : uBurstSz = ((4<<15)|(4<<12));break; 
		case BURST64   : uBurstSz = ((5<<15)|(5<<12));break;
		case BURST128  : uBurstSz = ((6<<15)|(6<<12));break;
		case BURST256  : uBurstSz = ((7<<15)|(7<<12));break;
		default : Assert(0);
	}

	//Channel LLI Register Setting => not using => 0x0
	uReg =
		(uLLIAddr&~(0x3))|
		(AHB_MASTER1);		
	DmaChOutp32(DMA_CH_LLI, uReg);

	if(uLLIAddr == 0)
		uTCEnable = 1;
	else
		uTCEnable = 0;
	
      // Channel Control Register0 Setting 
      // [31]: Terminal Count interrupt 
	uReg =
		(uTCEnable<<31) |
		(bDstFixed? 0 : 1)<<27 |
		(bSrcFixed? 0 : 1)<<26 |
		(eDstAhb? 1 : 0)<<25 |
		(eSrcAhb? 1 : 0)<<24 |
		(uTrWidth) |
		(uBurstSz);
      
	DmaChOutp32(DMA_CH_CONTROL0, uReg);

      // Channel Control Register1 Setting => Transfer Size
	DmaChOutp32(DMA_CH_CONTROL1, uDataCnt);

	// Channel Configuration Register1 Setting 
#if 0
	uReg = 
		(0<<18) | // enable DMA requests
		(0<<16) | // disables locked transfers
		(1<<15) | // Teminal count interrupt enable
		(0<<14) | // Interrupt error mask
		((((eSrcReq == MEM)||(eSrcReq == DMA1_NAND_TX)||(eSrcReq == DMA0_OND_TX))? 0 : 1)<<12) |
		(((eDstReq == DMA0_OND_RX)? 1 : 0)<<10) |
		((((eDstReq == MEM)||(eDstReq == DMA1_NAND_RX)||(eDstReq == DMA0_OND_RX))? 0 : 1)<<11) |
		(((eDstReq == MEM)? 0:eDstReq)<<6) | // DstPerpheral
		(((eSrcReq == DMA0_OND_TX)? 1 : 0)<<5) |
		(((eSrcReq == MEM)? 0:eSrcReq)<<1) ; // SrcPeripheral
#else
		
	uReg = 
		(0<<18) | // enable DMA requests
		(0<<16) | // disables locked transfers
		(1<<15) | // Teminal count interrupt enable
		(0<<14) | // Interrupt error mask
		((((eSrcReq == MEM)||(eSrcReq == DMA1_NAND_TX)||(eSrcReq == DMA0_OND_TX)||(eSrcReq == DMA_MODEM))? 0 : 1)<<12) |	//FlowCntl
		((((eDstReq == MEM)||(eDstReq == DMA1_NAND_RX)||(eDstReq == DMA0_OND_RX)||(eSrcReq == DMA_MODEM))? 0 : 1)<<11) |
		(((eDstReq == DMA0_OND_RX)? 1 : 0)<<10) |									// OneNandModeDst
		((((eDstReq == MEM)||(eSrcReq == DMA_MODEM))? 0:(eDstReq&0xf))<<6) | // DstPerpheral
		(((eSrcReq == DMA0_OND_TX)? 1 : 0)<<5) |									// OneNandModeSrc
		((((eSrcReq == MEM)||(eSrcReq == DMA_MODEM))? 0:(eSrcReq&0xf))<<1) ; // SrcPeripheral


/*
	uReg = 
		(0<<18) | // enable DMA requests
		(0<<16) | // disables locked transfers
		(1<<15) | // Teminal count interrupt enable
		(0<<14) | // Interrupt error mask
		((((eSrcReq == MEM)||(eSrcReq == DMA1_NAND_TX)||(eSrcReq == DMA0_OND_TX)||(eSrcReq == DMA1_EXTERNAL)||(eSrcReq == DMA_MODEM))? 0 : 1)<<12) |	//FlowCntl
		((((eDstReq == MEM)||(eDstReq == DMA1_NAND_RX)||(eDstReq == DMA0_OND_RX)||(eDstReq == DMA1_EXTERNAL)||(eSrcReq == DMA_MODEM))? 0 : 1)<<11) |
		(((eDstReq == DMA0_OND_RX)? 1 : 0)<<10) |									// OneNandModeDst
		((((eDstReq == MEM)||(eDstReq == DMA1_EXTERNAL)||(eSrcReq == DMA_MODEM))? 0:(eDstReq&0xf))<<6) | // DstPerpheral
		(((eSrcReq == DMA0_OND_TX)? 1 : 0)<<5) |									// OneNandModeSrc
		((((eSrcReq == MEM)||(eSrcReq == DMA1_EXTERNAL)||(eSrcReq == DMA_MODEM))? 0:(eSrcReq&0xf))<<1) ; // SrcPeripheral
*/
#endif

//		(eDstReq <<6) | // DstPerpheral
//		(eSrcReq <<1) ; // SrcPeripheral

	
	DmaChOutp32(DMA_CH_CONFIG, uReg);	





}

//////////
// Function Name : DMAC_Start
// Function Description :  Enable DMA Channel
// Input : 	*sCh  : DMAC , Enabled Channel address
// Output : 	void
//			
// Version : v0.1
void DMACH_Start(DMAC *sCh)
{
	u32 uReg;
	
	uReg = DmaChInp32(DMA_CH_CONFIG);
	
	uReg |= (1<<0);                                         // Channel Enable
	DmaChOutp32(DMA_CH_CONFIG, uReg);
}

//////////
// Function Name : DMACH_AddrSetup
// Function Description : Set Source address and destination address.
// Input : 	eCh  		: Selected channel.
//			uSrcAddr		: Source Address
//			uDstAddr        : Destination Address
//
// Output : 	void
//			
// Version : v0.1

void DMACH_AddrSetup(DMA_CH eCh,  u32 uSrcAddr, u32 uDstAddr, DMAC *sCh)
{
	

	switch(eCh)
	{
		case DMA_A: sCh->m_uChAddr = DMA_CH0;  break;
		case DMA_B: sCh->m_uChAddr = DMA_CH1;  break;
		case DMA_C: sCh->m_uChAddr = DMA_CH2;  break;
		case DMA_D: sCh->m_uChAddr = DMA_CH3;  break;
		case DMA_E: sCh->m_uChAddr = DMA_CH4;  break;
		case DMA_F: sCh->m_uChAddr = DMA_CH5;  break;
		case DMA_G: sCh->m_uChAddr = DMA_CH6;  break;
		case DMA_H: sCh->m_uChAddr = DMA_CH7;  break;
		default : Assert(0);
	}

	DmaChOutp32(DMA_CH_SRCADDR,	uSrcAddr);
	DmaChOutp32(DMA_CH_DSTADDR, uDstAddr);


}


//////////
// Function Name : DMACH_ReadSrcAddr
// Function Description : Read Source Address
// Input :  Structure definition of DMAC
// Output : 	Source Address
// Version : v0.1
// added by Woojin
u32 DMACH_ReadSrcAddr(DMAC *sCh)
{
	return DmaChInp32(DMA_CH_SRCADDR);
}


//////////
// Function Name : DMACH_ReadDstAddr
// Function Description : Read Destination Address
// Input :  Structure definition of DMAC
// Output : 	Destination Address
// Version : v0.1
// added by Woojin
u32 DMACH_ReadDstAddr(DMAC *sCh)
{
	return DmaChInp32(DMA_CH_DSTADDR);
}

//added by rb1004
void DMACH_WriteSrcAddr(DMAC *sCh, u32 uSrcAddr)
{
	DmaChOutp32(DMA_CH_SRCADDR, uSrcAddr);
}

//added by rb1004
void DMACH_WriteDstAddr(DMAC *sCh, u32 uDstAddr)
{
	DmaChOutp32(DMA_CH_DSTADDR, uDstAddr);
}


//added by rb1004
void DMACH_WriteTransferSize(DMAC *sCh, u32 uSize)
{
	DmaChOutp32(DMA_CH_CONTROL1, uSize);
}


//added by rb1004
void DMACH_SoftBurstReq(DMAC *sCh, DREQ_SRC eSrcReq)
{
	DmaOutp32(DMA_SOFTBREQ,	(1<<(eSrcReq&0xf)));
//	while(!(DmaInp32(DMA_SOFTBREQ) & (1<<eSrcReq)));
}

//added by rb1004
void DMACH_SoftBurstLastReq(DMAC *sCh, DREQ_SRC eSrcReq)
{
	DmaOutp32(DMA_SOFTLBREQ,	(1<<(eSrcReq&0xf)));
	//while(!(DmaInp32(DMA_SOFTLBREQ) & (1<<eSrcReq)));
}
//////////
// Function Name : DMACH_SetTransferSize
// Function Description : This function sets TransferSize
// Input :  Transfer Size
// Output : 	NONE
// Version : v0.1
// added by Woojin
void DMACH_SetTransferSize(u32 uSize, DMAC *sCh)
{
	DmaChOutp32(DMA_CH_CONTROL1, uSize);
	
}



