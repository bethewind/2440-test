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
*	File Name : hsi.c
*  
*	File Description : This file implements the API functions for MIPI HSI.
*
*	Author : Haksoo,Kim
*	Dept. : AP Development Team
*	Created Date : 2007/03/03
*	Version : 0.1 
* 
*	History
*	- Created(Haksoo,Kim 2007/03/03)
*  
**************************************************************************************/

#include "option.h"
#include "library.h"
#include "sfr6410.h"
#include "system.h"
#include "hsi.h"
#include "gpio.h"
#include "intc.h"
#include "dma.h"

//#define DBG_HSI
#ifdef DBG_HSI
	#define DbgHsi	Disp
#else
	#define DbgHsi(...)
#endif
//#define DBG_HSI_0
#ifdef DBG_HSI_0
	#define DbgHsi0	Disp
#else
	#define DbgHsi0(...)
#endif

//==========================
// MIPI HSI REGISTERS
//==========================
enum HSI_REGS
{
	//===================================
	// Tx Controller Registers
	rTX_STATUS_REG		= 0x7E006000,
	rTX_CONFIG_REG		= 0x7E006004,
	rTX_INTSRC_REG		= 0x7E00600C,
	rTX_INTMSK_REG		= 0x7E006010,
	rTX_SWRST_REG		= 0x7E006014,
	rTX_CHID_REG		= 0x7E006018,
	rTX_DATA_REG		= 0x7E00601C,
	
	//===================================
	// Rx Controller Registers
	rRX_STATUS_REG		= 0x7E007000,
	rRX_CONFIG0_REG		= 0x7E007004,
	rRX_CONFIG1_REG		= 0x7E007008,
	rRX_INTSRC_REG		= 0x7E00700C,
	rRX_INTMSK_REG		= 0x7E007010,
	rRX_SWRST_REG		= 0x7E007014,
	rRX_CHID_REG		= 0x7E007018,
	rRX_DATA_REG		= 0x7E00701C	
 };
 
#if 1	//070615
#define	TX_HOLD_TO_VAL		0xa
#define	TX_IDLE_TO_VAL		0x28
#define	TX_REQ_TO_VAL		0xff

#define RX_STATE_TO_VAL		0x30	//proper for normal cases
//#define RX_STATE_TO_VAL		0xff	//proper for using only rx-done intr.
#define RX_ACK_TO_VAL		0x80
#define RX_FIFO_TO_VAL1		0x100
#define RX_FIFO_TO_VAL2		0x10000

#else	//temp(maximum values)
#define	TX_HOLD_TO_VAL		0xff
#define	TX_IDLE_TO_VAL		0xff
#define	TX_REQ_TO_VAL		0xff

#define RX_STATE_TO_VAL		0x3ff
#define RX_ACK_TO_VAL		0xff
#define RX_FIFO_TO_VAL1		0xffffff
#define RX_FIFO_TO_VAL2		0xffffff
#endif

extern volatile u32 *g_uHsiTxData;
extern volatile u32 *g_uHsiRxData;

extern volatile u8 bHsiTxDone;
extern volatile u8 bHsiRxDone;

extern u32 g_uHsiPreTxCount;
extern u32 g_uHsiTxCount;
extern u32 g_uHsiRxCount;
extern u32 g_uHsiDmaTxCnt, g_uHsiDmaRxCnt;
extern HSI_DMA_RX_TRESH_VAL g_eHsiDmaThresVal;
extern u8 g_ucHsiDmaRxOnLevel;
extern HSI_OP_MODE g_eHsiOpMode;
extern HSI_FR_MODE g_eHsiFrMode;

extern DMAC 	oHsiDmaTx, oHsiDmaRx;

extern u8 bIsBreakTest;
extern u8 bBreakResult;
extern u8 bHsiLoopback;

//////////
// Function Name : HSI_SetPort
// Function Description : This function sets GPIO for HSI
// Input : NONE
// Output : NONE
// Version : 
void HSI_SetPort(void)
{
	GPIO_SetFunctionEach(eGPIO_K, eGPIO_0, 3);
	GPIO_SetFunctionEach(eGPIO_K, eGPIO_1, 3);
	GPIO_SetFunctionEach(eGPIO_K, eGPIO_2, 3);
	GPIO_SetFunctionEach(eGPIO_K, eGPIO_3, 3);
	GPIO_SetFunctionEach(eGPIO_K, eGPIO_4, 3);
	GPIO_SetFunctionEach(eGPIO_K, eGPIO_5, 3);
	GPIO_SetFunctionEach(eGPIO_K, eGPIO_6, 3);
	GPIO_SetFunctionEach(eGPIO_K, eGPIO_7, 3);

	// s/w work-around for GPIO bug
	InitUartPort(0, FALSE);

	return;
}

//////////
// Function Name : HSI_DisableTxDma
// Function Description : This function disables Tx DMA request
// Input : NONE
// Output : NONE
// Version : 
void HSI_DisableTxDma(void)
{
	u32 uTemp;
	
	uTemp = Inp32(rTX_INTMSK_REG);
	uTemp |= 1u<<31;
	Outp32(rTX_INTMSK_REG, uTemp);	

	return;
}

//////////
// Function Name : HSI_DisableRxDma
// Function Description : This function disables Tx DMA request
// Input : NONE
// Output : NONE
// Version : 
void HSI_DisableRxDma(void)
{
	u32 uTemp;
	
	uTemp = Inp32(rRX_INTMSK_REG);
	uTemp |= 1u<<31;
	Outp32(rRX_INTMSK_REG, uTemp);	

	return;
}

//////////
// Function Name : HSI_SetForTx
// Function Description : This function sets hsi sfr in order to tx
// Input : eFrMode, hsi operation mode(stream or frame)
//			eChidMode, ch id mode(burst or single)
//			eChidBits, ch id bit count(0~3)
//			eChidValue, ch id(0~7)
// Output : NONE
// Version : 
void HSI_SetForTx(HSI_OP_MODE eOpMode, HSI_FR_MODE eFrMode, HSI_CHID_MODE eChidMode, HSI_CHID_BITS eChidBits, HSI_CHID_VALUE eChidValue)
{
	u32 uTemp32;

	switch (eChidBits)
	{
		case HSI_CHID_0BIT:
			Assert(eChidValue==0);
			break;
		case HSI_CHID_1BIT:
			Assert(eChidValue<=1);
			break;
		case HSI_CHID_2BIT:
			Assert(eChidValue<=3);
			break;
		case HSI_CHID_3BIT:
			Assert(eChidValue<=7);
			break;
		default :
			Assert(eChidValue<=7);
			break;		
	}

	Outp32(rTX_SWRST_REG, 1);
	Outp32(rTX_SWRST_REG, 0);

	Outp32(rTX_CONFIG_REG, (1<<4));	//clear generated error

	Outp32(rTX_CONFIG_REG, (eChidBits<<2)|(eChidMode<<1)|(eFrMode<<0));

	Outp32(rTX_CHID_REG, eChidValue);

	uTemp32 = Inp32(rTX_CONFIG_REG);
	uTemp32 = uTemp32 & ~0xffffffe0 | ((TX_HOLD_TO_VAL<<24) | (TX_IDLE_TO_VAL<<16) | (TX_REQ_TO_VAL<<8) |
			(HSI_TIMER_ON<<7) | (HSI_TIMER_ON<<6) | (HSI_TIMER_ON<<5));
	Outp32(rTX_CONFIG_REG, uTemp32);

	Outp32(rTX_INTSRC_REG, 0x1f);	// clear interrupt pending

	if(eOpMode == HSI_INT_MODE)
	{
		Outp32(rTX_INTMSK_REG, 0x8000001f);	// mask tx interrupt
	}
	else if(eOpMode == HSI_DMA_MODE)
	{
		Outp32(rTX_INTMSK_REG, 0x0000001f);	// mask tx interrupt & enable dma_req
	}
	else
	{
		Assert(0);
	}

	return;
}


//////////
// Function Name : HSI_SetForRx
// Function Description : This function sets hsi sfr in order to rx
// Input : eFrMode, hsi operation mode(stream or frame)
//			eChidMode, ch id mode(burst or single)
//			eChidBits, ch id bit count(0~3)
// Output : NONE
// Version : 
void HSI_SetForRx(HSI_OP_MODE eOpMode, HSI_FR_MODE eFrMode, HSI_CHID_MODE eChidMode, HSI_CHID_BITS eChidBits)
{
	u32 uTemp32;

	Outp32(rRX_SWRST_REG, 1);
	Outp32(rRX_SWRST_REG, 0);

	Outp32(rRX_CONFIG0_REG, (1<<5)|(1<<4));	//clear break state & generated error

	Outp32(rRX_CONFIG0_REG, (eChidBits<<2)|(eChidMode<<1)|(eFrMode<<0));

	uTemp32 = Inp32(rRX_CONFIG0_REG);
	uTemp32 = uTemp32 & ~0x3fffff40 | ((g_eHsiDmaThresVal<<28)|(RX_STATE_TO_VAL<<16) | (RX_ACK_TO_VAL<<8) | (HSI_TIMER_ON<<6));	
	Outp32(rRX_CONFIG0_REG, uTemp32);

	if(eOpMode == HSI_INT_MODE)
		Outp32(rRX_CONFIG1_REG, (HSI_TIMER_ON<<27)|(RX_FIFO_TO_VAL1<<0));
	if(eOpMode == HSI_DMA_MODE)
		Outp32(rRX_CONFIG1_REG, (HSI_TIMER_ON<<27)|(RX_FIFO_TO_VAL2<<0));
	
	Outp32(rRX_INTSRC_REG, 0xff);	// clear interrupt pending

	if(eOpMode == HSI_INT_MODE)
	{
		Outp32(rRX_INTMSK_REG, 0x800000ff);	// mask rx interrupt
	}
	else if(eOpMode == HSI_DMA_MODE)
	{
		Outp32(rRX_INTMSK_REG, 0x000000ff);	// mask rx interrupt & enable dma_req
	}
	else
	{
		Assert(0);
	}

	

	return;
}

//////////
// Function Name : HSI_Tx_ISR
// Function Description : hsi tx isr
// Input : NONE
// Output : NONE
// Version : 
void __irq HSI_Tx_ISR(void)
{
	u32	uIntSrc, uTemp32;

	uIntSrc = Inp32(rTX_INTSRC_REG) & ~(Inp32(rTX_INTMSK_REG));		// w/o because of interrupt masking trouble

	if (uIntSrc & TXHOLD_TIMEOUT) {
		DbgHsi0("TxHOLD timeout.\n");
	}

	if (uIntSrc & TXIDLE_TIMEOUT) {
		DbgHsi0("TxIDLE timeout.\n");
	}
	
	if (uIntSrc & TXREQ_TIMEOUT) {
		DbgHsi0("TxREQ timeout.\n");
	}

	if (uIntSrc & TX_BREAKFRAME_END) {
		DbgHsi("Tx Break-Frame end.\n");
		if (bIsBreakTest == TRUE)
		{
			bHsiTxDone	=	1;	
			Outp32(rTX_INTSRC_REG, 0x1f);
			Outp32(rTX_INTMSK_REG, 0x1f);
		}
	}

	if (uIntSrc & TXFIFO_EMPTY) {
		DbgHsi("Tx FIFO Empty\n");	
		
		g_uHsiTxCount = g_uHsiPreTxCount;
		
		if(!(g_uHsiTxCount<HSI_TEST_COUNT))
		{
			bHsiTxDone	= 1;
			Outp32(rTX_INTSRC_REG, 0x1f);
			Outp32(rTX_INTMSK_REG, 0x1f);
		}
		else
		{		
			while(!(Inp32(rTX_STATUS_REG)&(1<<17)))
			{
				if (g_uHsiPreTxCount < HSI_TEST_COUNT)
				{
					Outp32(rTX_DATA_REG, g_uHsiTxData[g_uHsiPreTxCount++]);
				}
				else {
					break;
				}
			}
		}
	}

	uTemp32 = Inp32(rTX_CONFIG_REG);
	uTemp32 |= (1<<4);
	Outp32(rTX_CONFIG_REG, uTemp32);
	
	uTemp32 = Inp32(rTX_CONFIG_REG);
	uTemp32 &= ~(1<<4);
	Outp32(rTX_CONFIG_REG, uTemp32);

	Outp32(rTX_INTSRC_REG, uIntSrc);
	
	INTC_ClearVectAddr();

	return;
}

//////////
// Function Name : HSI_Rx_ISR
// Function Description : hsi rx isr
// Input : NONE
// Output : NONE
// Version : 
void __irq HSI_Rx_ISR(void)
{
	u32	uIntSrc, uTemp32;

	uIntSrc = Inp32(rRX_INTSRC_REG) & ~(Inp32(rRX_INTMSK_REG));		// w/o because of interrupt masking trouble

	if (uIntSrc & RX_BREAK_DONE)
	{
		DbgHsi("Rx Break-Frame done.\n");

		uTemp32 = Inp32(rRX_CONFIG0_REG);
		uTemp32 |= (1<<5);
		Outp32(rRX_CONFIG0_REG, uTemp32);
		
		uTemp32 = Inp32(rRX_CONFIG0_REG);
		uTemp32 &= ~(1<<5);
		Outp32(rRX_CONFIG0_REG, uTemp32);
		
		if(bIsBreakTest == TRUE)
		{
			bBreakResult = TRUE;
			bHsiRxDone = 1;
			Outp32(rRX_INTSRC_REG, 0xff);
			Outp32(rRX_INTMSK_REG, 0xff);
		}			
	}
	
	if (uIntSrc & RX_ADDED_CLOCK) {
		DbgHsi0("Rx Added Clock.\n");
	}

	if (uIntSrc & RX_MISSED_CLOCK) {
		DbgHsi0("Rx Missed Clock.\n");
	}
	
	if (uIntSrc & RXACK_TIMEOUT) {
		DbgHsi0("Rx ACK timeout.\n");
	}

	if (uIntSrc & RX_BREAKFRAME_ERR) {
		DbgHsi("Rx Break-Frame Error.\n");
	}

	if (uIntSrc & RX_DONE) {
		g_uHsiRxData[g_uHsiRxCount++] = Inp32(rRX_DATA_REG);
		DbgHsi0("Rx Done.\n");
		if(!(g_uHsiRxCount< HSI_TEST_COUNT))
		{
			bHsiRxDone = 1;	
			Outp32(rRX_INTSRC_REG, 0xff);
			Outp32(rRX_INTMSK_REG, 0xff);
		}
	}
	
	if ((uIntSrc & RXFIFO_TIMEOUT)||(uIntSrc & RXFIFO_FULL)) 
	{
		if (uIntSrc & RXFIFO_TIMEOUT)
		{
			DbgHsi("Rx FIFO timeout.\n");
		}
		else 
		{
			DbgHsi("Rx FIFO Full\n");
		}
		
		// Check empty and if not empty, read data
		while(!(Inp32(rRX_STATUS_REG) & (1<<16)))
		{
			g_uHsiRxData[g_uHsiRxCount++] = Inp32(rRX_DATA_REG);
		}
		if(!(g_uHsiRxCount< HSI_TEST_COUNT))
		{
			bHsiRxDone = 1;	
			Outp32(rRX_INTSRC_REG, 0xff);
			Outp32(rRX_INTMSK_REG, 0xff);
		}
		Outp32(rRX_INTSRC_REG, (RXFIFO_TIMEOUT | RXFIFO_FULL));
	}

	uTemp32 = Inp32(rRX_CONFIG0_REG);
	uTemp32 |= (1<<4);
	Outp32(rRX_CONFIG0_REG, uTemp32);
	
	uTemp32 = Inp32(rRX_CONFIG0_REG);
	uTemp32 &= ~(1<<4);
	Outp32(rRX_CONFIG0_REG, uTemp32);

	Outp32(rRX_INTSRC_REG, uIntSrc);
	
	INTC_ClearVectAddr();

	return;
}

//////////
// Function Name : HSI_SetDmaForTx
// Function Description : this function sets DMA for tx
// Input : NONE
// Output : NONE
// Version : 
void HSI_SetDmaForTx(u32 uCount)
{	
	if (uCount>0)
		g_uHsiDmaTxCnt = uCount;
	else
		Assert(0);
	
	// Channel, LLI_Address, SrcAddr, Src Type, DstAddr, Dst Type, Transfer Width, Transfer Size, OpMode(DEMAND), Src Req, Dst Req, Burst
	// Channel Set-up			[source increment, dest fixed]
	DMACH_Setup(DMA_A, 0x0, (u32)&g_uHsiTxData[g_uHsiTxCount] , 0, (u32)rTX_DATA_REG, 1, WORD, g_uHsiDmaTxCnt, DEMAND, MEM, DMA0_HSI_I2SV40_TX, SINGLE, &oHsiDmaTx);

	return;
}

//////////
// Function Name : HSI_SetDmaForRx
// Function Description : this function sets DMA for rx
// Input : NONE
// Output : NONE
// Version : 
void HSI_SetDmaForRx(u32 uCount)
{
	if(uCount<=g_ucHsiDmaRxOnLevel)
	{
		return;
	}	
	else
	{
		g_uHsiDmaRxCnt = uCount-g_ucHsiDmaRxOnLevel-1;
	}
	
	// Channel, LLI_Address, SrcAddr, Src Type, DstAddr, Dst Type, Transfer Width, Transfer Size, OpMode(DEMAND), Src Req, Dst Req, Burst
	// Channel Set-up			[source increment, dest fixed]
	DMACH_Setup(DMA_B, 0x0, (u32)rRX_DATA_REG , 1, (u32)&g_uHsiRxData[g_uHsiRxCount], 0, WORD, g_uHsiDmaRxCnt, DEMAND, DMA0_HSI_I2SV40_RX, MEM, SINGLE, &oHsiDmaRx);

	return;
}

//////////
// Function Name : HSI_Dma_ISR
// Function Description : hsi dma isr
// Input : NONE
// Output : NONE
// Version : 
void __irq HSI_Dma_ISR(void)
{
	u32 uStatus;
	
	INTC_Disable(NUM_DMA0);

	uStatus = DMAC_IntStatus(&oHsiDmaTx);
	
	if (uStatus&DMA_A)    // Tx
	{	
		DbgHsi("hsi tx dma\n");
		
		DMACH_Stop(&oHsiDmaTx);
		
		g_uHsiTxCount += g_uHsiDmaTxCnt;
		
		if (g_uHsiTxCount==HSI_TEST_COUNT)
		{
			bHsiTxDone = true; 
		}		
	}

	uStatus = DMAC_IntStatus(&oHsiDmaRx);
	
	if (uStatus&DMA_B)
	{
		DbgHsi("hsi rx dma\n");
		
		DMACH_Stop(&oHsiDmaRx);
		
		g_uHsiRxCount += g_uHsiDmaRxCnt;	
	}
	
	// Interrupt Clear
	DMACH_ClearIntPending(&oHsiDmaTx);
	DMACH_ClearErrIntPending(&oHsiDmaTx);
	DMACH_ClearIntPending(&oHsiDmaRx);
	DMACH_ClearErrIntPending(&oHsiDmaRx);
	
	INTC_Enable(NUM_DMA0);

	INTC_ClearVectAddr();
	
	return;
}

//////////
// Function Name : HSI_SetBreakFrameTx
// Function Description : this function sets tx_break_frame
// Input : NONE
// Output : NONE
// Version : 
void HSI_SetBreakFrameTx(void)
{
	u32 uTemp32;

	uTemp32 = Inp32(rTX_CHID_REG);
	uTemp32 |= (1u<<31)|(1<<30);	//break frame transfer
	Outp32(rTX_CHID_REG, uTemp32);

	uTemp32 = Inp32(rTX_INTMSK_REG);
	uTemp32 &= ~(TX_BREAKFRAME_END);	// unmask tx breakframe_end interrupt
	Outp32(rTX_INTMSK_REG, uTemp32);

	return;
}

//////////
// Function Name : HSI_ClearBreakFrameTx
// Function Description : this function clears tx_break_frame
// Input : NONE
// Output : NONE
// Version : 
void HSI_ClearBreakFrameTx(void)
{
	u32 uTemp32;

	uTemp32 = Inp32(rTX_CHID_REG);
	uTemp32 |= (1<<29);	// clear break frame
	Outp32(rTX_CHID_REG, uTemp32);

	return;
}

//////////
// Function Name : HSI_UnmaskTxInt
// Function Description : this function unmasks specific tx interrupts
// Input : NONE
// Output : NONE
// Version : 
void HSI_UnmaskTxInt(u32 uInt)
{
	u32 uTemp32;

	uTemp32 = Inp32(rTX_INTMSK_REG);
	uTemp32 = (uTemp32 | 0x1f) & ~uInt;
	Outp32(rTX_INTMSK_REG, uTemp32);

	return;
}

//////////
// Function Name : HSI_UnmaskRxInt
// Function Description : this function unmasks specific rx interrupts
// Input : NONE
// Output : NONE
// Version : 
void HSI_UnmaskRxInt(u32 uInt)
{
	u32 uTemp32;

	uTemp32 = Inp32(rRX_INTMSK_REG);
	uTemp32 = (uTemp32 | 0xff) & ~uInt;
	Outp32(rRX_INTMSK_REG, uTemp32);

	return;
}

//////////
// Function Name : TestBd2BdRxWakeupByInt
// Function Description : mipi hsi b'd to b'd rx test using interrupt mode
// Input : NONE
// Output : NONE
// Version :
void HSI_WakeupByInt(void)
{
	bHsiLoopback = false;
	bIsBreakTest = false;
	bBreakResult = false;

	g_eHsiOpMode = HSI_INT_MODE;
	
	g_uHsiDmaTxCnt = 0;
	g_uHsiPreTxCount = 0;
	g_uHsiTxCount	= 0;
	bHsiTxDone	= false;
		
	g_uHsiDmaRxCnt = 0;
	g_uHsiRxCount = 0;
	bHsiRxDone	= false;


	bIsBreakTest = false;
	g_eHsiFrMode = HSI_FRAME_MODE;
	g_eHsiDmaThresVal = HSI_DREQ_4WORDS;

	INTC_SetVectAddr(NUM_HSIrx, HSI_Rx_ISR);
	INTC_Enable(NUM_HSIrx);
	
	//register setting
	HSI_SetForRx(HSI_INT_MODE, HSI_FRAME_MODE, HSI_SINGLE_CHID_MODE,HSI_CHID_3BIT);

	HSI_UnmaskRxInt(0xfb);	// unmask all interrupts except rx-done

	UART_Printf("### Now MIPI is ready to operate as a wake up source ### \n");

}


