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
*	File Name : hsi_test.c
*  
*	File Description : This file implements mipi hsi test functions.
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
#include "intc.h"
#include "hsi.h"
#include "dma.h"
#include "sysc.h"
#include "timer.h"

#define MIPI_TX_MEM_BASE		(_DRAM_BaseAddress+0x02000000)
#define MIPI_RX_MEM_BASE		(_DRAM_BaseAddress+0x02800000)

volatile u32 *g_uHsiTxData=(volatile u32 *)MIPI_TX_MEM_BASE;
volatile u32 *g_uHsiRxData=(volatile u32 *)MIPI_RX_MEM_BASE;

volatile u8 bHsiTxDone = false;
volatile u8 bHsiRxDone = false;

u32 g_uHsiPreTxCount=0, g_uHsiTxCount = 0;
u32 g_uHsiRxCount = 0;
u32 g_uHsiDmaTxCnt, g_uHsiDmaRxCnt;

u8 bIsBreakTest = FALSE;
u8 bBreakResult = FALSE;
u8 bHsiLoopback = FALSE;

HSI_OP_MODE g_eHsiOpMode=HSI_INT_MODE;
HSI_FR_MODE g_eHsiFrMode=HSI_FRAME_MODE;
HSI_CHID_MODE g_eHsiChidMode=HSI_SINGLE_CHID_MODE;
HSI_CHID_BITS g_eHsiChidBits=HSI_CHID_3BIT;
HSI_CHID_VALUE g_eHsiChidValue=HSI_CHID_7;
HSI_DMA_RX_TRESH_VAL g_eHsiDmaThresVal=HSI_DREQ_4WORDS;
u8 g_ucHsiDmaRxOnLevel;

DMAC 	oHsiDmaTx, oHsiDmaRx;

//////////
// Function Name : PrepareTxData
// Function Description : This function prepares tx data before starting test
// Input : NONE
// Output : NONE
// Version :
void PrepareTxData(void)
{
	u32 i;

	for(i=1;i<(HSI_TEST_COUNT+1);i++)
	{
		//g_uHsiTxData[i]= (((i+0)%8)<<28)|(((i+1)%8)<<24)|(((i+2)%8)<<20)|(((i+3)%8)<<16)|
		//			(((i+4)%8)<<12)|(((i+5)%8)<<8)|(((i+6)%8)<<4)|(((i+7)%8)<<0);
		g_uHsiTxData[i-1] = ((u16)i<<16)|((u16)i<<0);
	}
	g_uHsiTxData[HSI_TEST_COUNT]=0;
}


//////////
// Function Name : PrepareRxData
// Function Description : This function initializes rx data before starting test
// Input : NONE
// Output : NONE
// Version :
void PrepareRxData(void)
{
	u32 i;

	for(i=0;i<HSI_TEST_COUNT;i++)
	{
		//g_uHsiRxData[i]= 0;
		g_uHsiRxData[i]= 0xffffffff;
	}
}

//////////
// Function Name : CompareTxRxData
// Function Description : This function compares received rx data with expected rx data
// Input : NONE
// Output : NONE
// Version :
u32 CompareTxRxData(void)
{
	u32 i, error_count=0;

	if(bIsBreakTest == TRUE)
	{
		return (!bBreakResult);
	}
	else
	{	
		if(g_uHsiRxCount!=HSI_TEST_COUNT)
		{
			UART_Printf("ERROR:rx data count(%d) is not same as tx count(%d)\n",g_uHsiRxCount,HSI_TEST_COUNT);
						
			for(i=0;i<((g_uHsiRxCount>HSI_TEST_COUNT)?g_uHsiRxCount:HSI_TEST_COUNT);i++)
			{
				if(g_uHsiRxData[i]!=g_uHsiTxData[i])
				{
					error_count++;
					UART_Printf(" %3dth => txData : 0x%08x, rxData : 0x%08x, ",(i+1),g_uHsiTxData[i],g_uHsiRxData[i]);
					if(error_count%2==0)
						UART_Printf("\n");
				}
				if(GetKey()=='x')
				{
					break;
				}
			}
			return (error_count==0?1:error_count);
		}
		else
		{
			for(i=0;i<HSI_TEST_COUNT;i++)
			{
				if(g_uHsiRxData[i]!=g_uHsiTxData[i])
				{
					error_count++;
					UART_Printf(" %3dth => txData : 0x%08x, rxData : 0x%08x, ",(i+1),g_uHsiTxData[i],g_uHsiRxData[i]);
					if(error_count%2==0)
						UART_Printf("\n");
				}
				if(GetKey()=='x')
				{
					break;
				}
			}
			return error_count;
		}
	}
}

//////////
// Function Name : TestLoopbackByInt
// Function Description : mipi hsi loopbak test using interrupt mode
// Input : NONE
// Output : NONE
// Version :
static void TestLoopbackByInt(void)
{	
	bHsiLoopback = true;
	g_eHsiOpMode=HSI_INT_MODE;
	bIsBreakTest = false;
	bBreakResult = false;
	
	g_uHsiDmaTxCnt = 0;
	g_uHsiPreTxCount = 0;
	g_uHsiTxCount	= 0;
	bHsiTxDone	= false;
		
	g_uHsiDmaRxCnt = 0;
	g_uHsiRxCount = 0;
	bHsiRxDone	= false;	

	UART_Printf("If you want to test a break frame, press 'b' key...\n");
	if(Getc()=='b')
	{
		bIsBreakTest = true;
		g_eHsiFrMode = HSI_FRAME_MODE;
	}
	else
	{
		bIsBreakTest = false;
		UART_Printf("Select TX Physical Mode...\n");
		UART_Printf(" 0 : Stream mode,   1 : Frame mode \n");
		g_eHsiFrMode = (HSI_FR_MODE)UART_GetIntNum();
		if(g_eHsiFrMode>HSI_FRAME_MODE)
		{
			UART_Printf("You entered wrong number!!!\n");
			return;
		}
	}
	
	if (bIsBreakTest == false)
	{
		//prepare data
		PrepareTxData();
		PrepareRxData();
	}

	//ISR mapping and interrupt enabling
	INTC_SetVectAddr(NUM_HSItx, HSI_Tx_ISR);
	INTC_Enable(NUM_HSItx);
	
	INTC_SetVectAddr(NUM_HSIrx, HSI_Rx_ISR);
	INTC_Enable(NUM_HSIrx);
	
	//register setting
	if(bIsBreakTest == true)
	{
		HSI_SetForTx(g_eHsiOpMode, HSI_FRAME_MODE, g_eHsiChidMode, g_eHsiChidBits, g_eHsiChidValue);
		HSI_SetForRx(g_eHsiOpMode, HSI_FRAME_MODE, g_eHsiChidMode, g_eHsiChidBits);
	}
	else
	{
		HSI_SetForTx(g_eHsiOpMode, g_eHsiFrMode, g_eHsiChidMode, g_eHsiChidBits, g_eHsiChidValue);
		HSI_SetForRx(g_eHsiOpMode, g_eHsiFrMode, g_eHsiChidMode, g_eHsiChidBits);
	}

	HSI_UnmaskTxInt(0x1f);	// unmask all interrupts
	HSI_UnmaskRxInt(0xfb);	// unmask all interrupts except rx-done

	if(bIsBreakTest == true)
	{
		HSI_SetBreakFrameTx();
	}

	if(bIsBreakTest == true)
	{		
		//wait until all data have received or test stops
		UART_Printf("If you want to stop, press 'x' key...\n");
		
		while(!bHsiRxDone)
		{
			if(GetKey()=='x')
			{
				break;
			}
		}
		
		HSI_ClearBreakFrameTx();
		
		//tx-interupt disable
		INTC_Disable(NUM_HSItx);
		
		//rx-interupt disable
		INTC_Disable(NUM_HSIrx);
		INTC_ClearVectAddr();
	}
	else
	{	
		//wait until all data have transmitted or test stops
		UART_Printf("If you want to stop, press 'x' key...\n");
		
		while(!bHsiTxDone)
		{
			if(GetKey()=='x')
			{
				break;
			}
		}
		HSI_UnmaskTxInt(0x00);		
		//tx-interupt disable
		INTC_Disable(NUM_HSItx);
		
		UART_Printf("TX COUNT : %d\n",g_uHsiTxCount);
		
		//wait until all data have received or test stops
		UART_Printf("If you want to stop, press 'x' key...\n");
		
		while(!bHsiRxDone)
		{
			if(GetKey()=='x')
			{
				break;
			}
		}
		HSI_UnmaskRxInt(0x00);	
		//rx-interupt disable
		INTC_Disable(NUM_HSIrx);
		INTC_ClearVectAddr();
		
		UART_Printf("RX COUNT : %d\n",g_uHsiRxCount);
	}

	//compare rxData with txData
	if(!CompareTxRxData())
	{
		UART_Printf("\n HSI TX/RX LOOPBACK TEST O.K.\n");
	}

	return;
}

//////////
// Function Name : TestLoopbackByRxDone
// Function Description : mipi hsi loopbak test using only rx-done interrupt
// Input : NONE
// Output : NONE
// Version :
static void TestLoopbackByRxDone(void)
{
	bHsiLoopback = true;
	g_eHsiOpMode=HSI_INT_MODE;
	bIsBreakTest = false;
	bBreakResult = false;
	
	g_uHsiDmaTxCnt = 0;
	g_uHsiPreTxCount = 0;
	g_uHsiTxCount	= 0;
	bHsiTxDone	= false;
		
	g_uHsiDmaRxCnt = 0;
	g_uHsiRxCount = 0;
	bHsiRxDone	= false;	

	UART_Printf("Select TX Physical Mode...\n");
	UART_Printf(" 0 : Stream mode,   1 : Frame mode \n");
	g_eHsiFrMode = (HSI_FR_MODE)UART_GetIntNum();	
	if(g_eHsiFrMode>HSI_FRAME_MODE)
	{
		UART_Printf("You entered wrong number!!!\n");
		return;
	}
	
	//prepare data
	PrepareTxData();
	PrepareRxData();

	//ISR mapping and interrupt enabling
	INTC_SetVectAddr(NUM_HSItx, HSI_Tx_ISR);
	INTC_Enable(NUM_HSItx);
	
	INTC_SetVectAddr(NUM_HSIrx, HSI_Rx_ISR);
	INTC_Enable(NUM_HSIrx);
	
	//register setting
	HSI_SetForTx(g_eHsiOpMode, g_eHsiFrMode, g_eHsiChidMode, g_eHsiChidBits, g_eHsiChidValue);
	HSI_SetForRx(g_eHsiOpMode, g_eHsiFrMode, g_eHsiChidMode, g_eHsiChidBits);
	
	HSI_UnmaskTxInt(0x1f);	// unmask all interrupts
	HSI_UnmaskRxInt(0xf4);	// unmask all interrupts except rx-fifo & full rx-fifo timeout

	//wait until all data have transmitted or test stops
	UART_Printf("If you want to stop, press 'x' key...\n");
	
	while(!bHsiTxDone)
	{
		if(GetKey()=='x')
		{
			break;
		}
	}
	HSI_UnmaskTxInt(0x00);		
	//tx-interupt disable
	INTC_Disable(NUM_HSItx);
	
	UART_Printf("TX COUNT : %d\n",g_uHsiTxCount);
	
	//wait until all data have received or test stops
	UART_Printf("If you want to stop, press 'x' key...\n");
	
	while(!bHsiRxDone)
	{
		if(GetKey()=='x')
		{
			break;
		}
	}
	HSI_UnmaskRxInt(0x00);	
	//rx-interupt disable
	INTC_Disable(NUM_HSIrx);
	INTC_ClearVectAddr();
	
	UART_Printf("RX COUNT : %d\n",g_uHsiRxCount);

	//compare rxData with txData
	if(!CompareTxRxData())
	{
		UART_Printf("\n HSI TX/RX LOOPBACK TEST O.K.\n");
	}
	return;
}

//////////
// Function Name : TestLoopbackByDma
// Function Description : mipi hsi loopbak test using dma mode
// Input : NONE
// Output : NONE
// Version :
static void TestLoopbackByDma(void)
{
	bHsiLoopback = true;
	bIsBreakTest = false;
	bBreakResult = false;
	g_eHsiOpMode = HSI_DMA_MODE;
	
	g_uHsiDmaTxCnt = 0;
	g_uHsiPreTxCount = 0;
	g_uHsiTxCount	= 0;
	bHsiTxDone	= false;
		
	g_uHsiDmaRxCnt = 0;
	g_uHsiRxCount = 0;
	bHsiRxDone	= false;	
		
	UART_Printf("Select TX Physical Mode...\n");
	UART_Printf(" 0 : Stream mode,   1 : Frame mode \n");
	g_eHsiFrMode = (HSI_FR_MODE)UART_GetIntNum();
	if(g_eHsiFrMode>HSI_FRAME_MODE)
	{
		UART_Printf("You entered wrong number!!!\n");
		return;
	}
	
	switch(g_eHsiDmaThresVal)
	{
		case HSI_DREQ_FULL:
			g_ucHsiDmaRxOnLevel = HSI_RXFIFO_DEPTH;
			break;
		case HSI_DREQ_16WORDS:
			g_ucHsiDmaRxOnLevel = 16;
			break;
		case HSI_DREQ_8WORDS:
			g_ucHsiDmaRxOnLevel = 8;
			break;
		case HSI_DREQ_4WORDS:
		default:
			g_ucHsiDmaRxOnLevel = 4;
			break;		 
	}
	
	//prepare data
	PrepareTxData();
	PrepareRxData();
	
	//ISR mapping and interrupt enabling
	INTC_SetVectAddr(NUM_HSItx, HSI_Tx_ISR);
	INTC_Enable(NUM_HSItx);
	
	INTC_SetVectAddr(NUM_HSIrx, HSI_Rx_ISR);
	INTC_Enable(NUM_HSIrx);
	
	//register setting
	HSI_SetForRx(g_eHsiOpMode, g_eHsiFrMode, g_eHsiChidMode, g_eHsiChidBits);
	HSI_SetForTx(g_eHsiOpMode, g_eHsiFrMode, g_eHsiChidMode, g_eHsiChidBits, g_eHsiChidValue);
	
	//////////
	// DMA0 Initailization
	SYSC_SelectDMA(eSEL_HSI_RX, 1);
	SYSC_SelectDMA(eSEL_HSI_TX, 1);
	DMAC_InitCh(DMA0 , DMA_ALL, &oHsiDmaRx);
	DMAC_InitCh(DMA0 , DMA_ALL, &oHsiDmaTx);

	INTC_SetVectAddr(NUM_DMA0 ,HSI_Dma_ISR);
	INTC_Enable(NUM_DMA0);
	
	// Interrupt Clear
	DMACH_ClearIntPending(&oHsiDmaRx);
	DMACH_ClearErrIntPending(&oHsiDmaRx);
	DMACH_ClearIntPending(&oHsiDmaTx);
	DMACH_ClearErrIntPending(&oHsiDmaTx);
	
	HSI_SetDmaForRx(HSI_TEST_COUNT);
	HSI_SetDmaForTx(HSI_TEST_COUNT);
	
	// Enable DMA
	DMACH_Start(&oHsiDmaRx);
	DMACH_Start(&oHsiDmaTx);
	//////////
	
	HSI_UnmaskRxInt(0xfb);	// unmask all interrupts except rx-done
	HSI_UnmaskTxInt(0x1e);	// unmask all interrupts except tx-fifo empty

	//wait until all data have transmitted or test stops
	UART_Printf("If you want to stop, press 'x' key...\n");
	
	while(!bHsiTxDone)
	{
		if(GetKey()=='x')
		{
			break;
		}
	}
	HSI_DisableTxDma();
	
	DMACH_Stop(&oHsiDmaTx);
	
	//tx-interupt disable
	INTC_Disable(NUM_HSItx);
	
	UART_Printf("TX COUNT : %d\n",g_uHsiTxCount);
	
	//wait until all data have received or test stops
	UART_Printf("If you want to stop, press 'x' key...\n");
	
	while(!bHsiRxDone)
	{
		if(GetKey()=='x')
		{
			break;
		}
	}
	HSI_DisableRxDma();	
	
	DMACH_Stop(&oHsiDmaRx);
	INTC_Disable(NUM_DMA0);	
	
	DMAC_Close(DMA0 , DMA_ALL, &oHsiDmaTx);
	DMAC_Close(DMA0 , DMA_ALL, &oHsiDmaRx);
	
	//rx-interupt disable
	INTC_Disable(NUM_HSIrx);
	INTC_ClearVectAddr();
	
	UART_Printf("RX COUNT : %d\n",g_uHsiRxCount);

	//compare rxData with txData
	if(!CompareTxRxData())
	{
		UART_Printf("\n HSI TX/RX LOOPBACK TEST O.K.\n");
	}

	return;
}

//////////
// Function Name : TestBd2BdTxByInt
// Function Description : mipi hsi b'd to b'd tx test using interrupt mode
// Input : NONE
// Output : NONE
// Version :
static void TestBd2BdTxByInt(void)
{
	bHsiLoopback = false;
	g_eHsiOpMode=HSI_INT_MODE;
	bIsBreakTest = false;
	bBreakResult = false;
	
	g_uHsiDmaTxCnt = 0;
	g_uHsiPreTxCount = 0;
	g_uHsiTxCount	= 0;
	bHsiTxDone	= false;
		
	g_uHsiDmaRxCnt = 0;
	g_uHsiRxCount = 0;
	bHsiRxDone	= false;

	UART_Printf("If you want to test a break frame, press 'b' key...\n");
	if(Getc()=='b')
	{
		bIsBreakTest = true;
		g_eHsiFrMode = HSI_FRAME_MODE;
	}
	else
	{
		bIsBreakTest = false;
		UART_Printf("Select TX Physical Mode...\n");
		UART_Printf(" 0 : Stream mode,   1 : Frame mode \n");
		g_eHsiFrMode = (HSI_FR_MODE)UART_GetIntNum();
		if(g_eHsiFrMode>HSI_FRAME_MODE)
		{
			UART_Printf("You entered wrong number!!!\n");
			return;
		}
	}
	
	if (bIsBreakTest == false)
	{
		//prepare data
		PrepareTxData();
	}

	//ISR mapping and interrupt enabling
	INTC_SetVectAddr(NUM_HSItx, HSI_Tx_ISR);
	INTC_Enable(NUM_HSItx);
	
	//register setting
	if(bIsBreakTest == true)
	{
		HSI_SetForTx(g_eHsiOpMode, HSI_FRAME_MODE, g_eHsiChidMode, g_eHsiChidBits, g_eHsiChidValue);
	}
	else
	{
		HSI_SetForTx(g_eHsiOpMode, g_eHsiFrMode, g_eHsiChidMode, g_eHsiChidBits, g_eHsiChidValue);
	}
	
	HSI_UnmaskTxInt(0x1f);	// unmask all interrupts

	if(bIsBreakTest == true)
	{
		HSI_SetBreakFrameTx();
	}

	if(bIsBreakTest == true)
	{		
		//wait until all data have received or test stops
		UART_Printf("If you want to stop, press 'x' key...\n");
		
		while(!bHsiTxDone)
		{
			if(GetKey()=='x')
			{
				break;
			}
		}
		
		HSI_ClearBreakFrameTx();
		
		//tx-interupt disable
		INTC_Disable(NUM_HSItx);
	}
	else
	{	
		//wait until all data have transmitted or test stops
		UART_Printf("If you want to stop, press 'x' key...\n");
		
		while(!bHsiTxDone)
		{
			if(GetKey()=='x')
			{
				break;
			}
		}
		//tx-interupt disable
		INTC_Disable(NUM_HSItx);
		
		UART_Printf("TX COUNT : %d\n",g_uHsiTxCount);
	}

	return;	
}

//////////
// Function Name : TestBd2BdTxByDma
// Function Description : mipi hsi b'd to b'd tx test using dma mode
// Input : NONE
// Output : NONE
// Version :
static void TestBd2BdTxByDma(void)
{
	bHsiLoopback = false;
	bIsBreakTest = false;
	bBreakResult = false;
	g_eHsiOpMode = HSI_DMA_MODE;
	
	g_uHsiDmaTxCnt = 0;
	g_uHsiPreTxCount = 0;
	g_uHsiTxCount	= 0;
	bHsiTxDone	= false;
		
	g_uHsiDmaRxCnt = 0;
	g_uHsiRxCount = 0;
	bHsiRxDone	= false;	
		
	UART_Printf("Select TX Physical Mode...\n");
	UART_Printf(" 0 : Stream mode,   1 : Frame mode \n");
	g_eHsiFrMode = (HSI_FR_MODE)UART_GetIntNum();
	if(g_eHsiFrMode>HSI_FRAME_MODE)
	{
		UART_Printf("You entered wrong number!!!\n");
		return;
	}
	
	//prepare data
	PrepareTxData();
	
	//ISR mapping and interrupt enabling
	INTC_SetVectAddr(NUM_HSItx, HSI_Tx_ISR);
	INTC_Enable(NUM_HSItx);
	
	//register setting
	HSI_SetForTx(g_eHsiOpMode, g_eHsiFrMode, g_eHsiChidMode, g_eHsiChidBits, g_eHsiChidValue);
	
	//////////
	// DMA0 Initailization
	SYSC_SelectDMA(eSEL_HSI_TX, 1);
	DMAC_InitCh(DMA0 , DMA_ALL, &oHsiDmaTx);

	INTC_SetVectAddr(NUM_DMA0 ,HSI_Dma_ISR);
	INTC_Enable(NUM_DMA0);
	
	// Interrupt Clear
	DMACH_ClearIntPending(&oHsiDmaTx);
	DMACH_ClearErrIntPending(&oHsiDmaTx);
	
	HSI_SetDmaForTx(HSI_TEST_COUNT);
	
	 // Enable DMA
	DMACH_Start(&oHsiDmaTx);
	//////////

	HSI_UnmaskTxInt(0x1e);	// unmask all interrupts except tx-fifo empty
	
	//wait until all data have transmitted or test stops
	UART_Printf("If you want to stop, press 'x' key...\n");
	
	while(!bHsiTxDone)
	{
		if(GetKey()=='x')
		{
			break;
		}
	}		
	HSI_DisableTxDma();
	
	DMACH_Stop(&oHsiDmaTx);
	INTC_Disable(NUM_DMA0);
	
	DMAC_Close(DMA0 , DMA_ALL, &oHsiDmaTx);
	
	//tx-interupt disable
	INTC_Disable(NUM_HSItx);
	
	UART_Printf("TX COUNT : %d\n",g_uHsiTxCount);

	return;
}

//////////
// Function Name : TestBd2BdRxByInt
// Function Description : mipi hsi b'd to b'd rx test using interrupt mode
// Input : NONE
// Output : NONE
// Version :
static void TestBd2BdRxByInt(void)
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

	UART_Printf("If you want to test a break frame, press 'b' key...\n");
	if(Getc()=='b')
	{
		bIsBreakTest = true;
		g_eHsiFrMode = HSI_FRAME_MODE;
	}
	else
	{
		bIsBreakTest = false;
		UART_Printf("Select TX Physical Mode...\n");
		UART_Printf(" 0 : Stream mode,   1 : Frame mode \n");
		g_eHsiFrMode = (HSI_FR_MODE)UART_GetIntNum();
		if(g_eHsiFrMode>HSI_FRAME_MODE)
		{
			UART_Printf("You entered wrong number!!!\n");
			return;
		}
	}
	
	if (bIsBreakTest == false)
	{
		//prepare data
		PrepareTxData();
		PrepareRxData();
	}

	//ISR mapping and interrupt enabling	
	INTC_SetVectAddr(NUM_HSIrx, HSI_Rx_ISR);
	INTC_Enable(NUM_HSIrx);
	
	//register setting
	if(bIsBreakTest == true)
	{
		HSI_SetForRx(g_eHsiOpMode, HSI_FRAME_MODE, g_eHsiChidMode, g_eHsiChidBits);
	}
	else
	{
		HSI_SetForRx(g_eHsiOpMode, g_eHsiFrMode, g_eHsiChidMode, g_eHsiChidBits);
	}

	HSI_UnmaskRxInt(0xfb);	// unmask all interrupts except rx-done

	if(bIsBreakTest == true)
	{		
		//wait until all data have received or test stops
		UART_Printf("If you want to stop, press 'x' key...\n");
		
		while(!bHsiRxDone)
		{
			if(GetKey()=='x')
			{
				break;
			}
		}
		
		//rx-interupt disable
		INTC_Disable(NUM_HSIrx);
		INTC_ClearVectAddr();
	}
	else
	{		
		//wait until all data have received or test stops
		UART_Printf("If you want to stop, press 'x' key...\n");
		
		while(!bHsiRxDone)
		{
			if(GetKey()=='x')
			{
				break;
			}
		}	
		//rx-interupt disable
		INTC_Disable(NUM_HSIrx);
		INTC_ClearVectAddr();
		
		UART_Printf("RX COUNT : %d\n",g_uHsiRxCount);
	}

	//compare rxData with txData
	if(!CompareTxRxData())
	{
		UART_Printf("\n HSI TX/RX TEST(b'd to b'd) O.K.\n");
	}

	return;
}

//////////
// Function Name : TestBd2BdRxByDma
// Function Description : mipi hsi b'd to b'd rx test using Dma mode
// Input : NONE
// Output : NONE
// Version :
static void TestBd2BdRxByDma(void)
{	
	bHsiLoopback = false;
	bIsBreakTest = false;
	bBreakResult = false;
	g_eHsiOpMode = HSI_DMA_MODE;
	
	g_uHsiDmaTxCnt = 0;
	g_uHsiPreTxCount = 0;
	g_uHsiTxCount	= 0;
	bHsiTxDone	= false;
		
	g_uHsiDmaRxCnt = 0;
	g_uHsiRxCount = 0;
	bHsiRxDone	= false;
		
	UART_Printf("Select TX Physical Mode...\n");
	UART_Printf(" 0 : Stream mode,   1 : Frame mode \n");
	g_eHsiFrMode = (HSI_FR_MODE)UART_GetIntNum();
	if(g_eHsiFrMode>HSI_FRAME_MODE)
	{
		UART_Printf("You entered wrong number!!!\n");
		return;
	}
	
	switch(g_eHsiDmaThresVal)
	{
		case HSI_DREQ_FULL:
			g_ucHsiDmaRxOnLevel = HSI_RXFIFO_DEPTH;
			break;
		case HSI_DREQ_16WORDS:
			g_ucHsiDmaRxOnLevel = 16;
			break;
		case HSI_DREQ_8WORDS:
			g_ucHsiDmaRxOnLevel = 8;
			break;
		case HSI_DREQ_4WORDS:
		default:
			g_ucHsiDmaRxOnLevel = 4;
			break;		 
	}
	
	//prepare data
	PrepareTxData();
	PrepareRxData();
	
	//ISR mapping and interrupt enabling
	INTC_SetVectAddr(NUM_HSIrx, HSI_Rx_ISR);
	INTC_Enable(NUM_HSIrx);

	//register setting
	HSI_SetForRx(g_eHsiOpMode, g_eHsiFrMode, g_eHsiChidMode, g_eHsiChidBits);
	
//////////
	// DMA0 Initailization
	SYSC_SelectDMA(eSEL_HSI_RX, 1);
	DMAC_InitCh(DMA0 , DMA_ALL, &oHsiDmaRx);

	INTC_SetVectAddr(NUM_DMA0 ,HSI_Dma_ISR);
	INTC_Enable(NUM_DMA0);
	
	// Interrupt Clear
	DMACH_ClearIntPending(&oHsiDmaRx);
	DMACH_ClearErrIntPending(&oHsiDmaRx);
	
	HSI_SetDmaForRx(HSI_TEST_COUNT);
	
	 // Enable DMA
	DMACH_Start(&oHsiDmaRx);
//////////

	HSI_UnmaskRxInt(0xfb);	// unmask all interrupts except rx-done

	//wait until all data have received or test stops
	UART_Printf("If you want to stop, press 'x' key...\n");
	
	while(!bHsiRxDone)
	{
		if(GetKey()=='x')
		{
			break;
		}
	}
	HSI_DisableRxDma();		
	
	DMACH_Stop(&oHsiDmaRx);
	INTC_Disable(NUM_DMA0);
	
	DMAC_Close(DMA0 , DMA_ALL, &oHsiDmaRx);
	
	//rx-interupt disable
	INTC_Disable(NUM_HSIrx);
	INTC_ClearVectAddr();
	
	UART_Printf("RX COUNT : %d\n",g_uHsiRxCount);

	//compare rxData with txData
	if(!CompareTxRxData())
	{
		UART_Printf("\n HSI TX/RX LOOPBACK TEST O.K.\n");
	}

	return;
}


//////////
// Function Name : TestBd2BdRxWakeupByInt
// Function Description : mipi hsi b'd to b'd rx test using interrupt mode
// Input : NONE
// Output : NONE
// Version :
static void TestBd2BdRxWakeupByInt(void)
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

	INTC_SetVectAddr(NUM_HSIrx, HSI_Rx_ISR);
	INTC_Enable(NUM_HSIrx);
	
	//register setting
	HSI_SetForRx(HSI_INT_MODE, HSI_FRAME_MODE, HSI_SINGLE_CHID_MODE,HSI_CHID_3BIT);

	HSI_UnmaskRxInt(0xfb);	// unmask all interrupts except rx-done

	UART_Printf("Now MIPI is ready to get interrupt as a wake up source, Try it! \n");

}


//////////
// Function Name : HSI_LoopbackByInt_autotest
// Function Description : mipi hsi loopbak auto-test using interrupt mode
// Input : NONE
// Output : NONE
// Version :
u8 HSI_LoopbackByInt_autotest(void)
{
	u8 bAutoTestResult = false;
	u8 bClockChange = false;
	u32 uClkDivider;
	
	SYSC_GetClkInform();
	
	if ((float)g_PCLK/1.0e6 > 50)
	{
		u8 ucPclkDiv;
			
		uClkDivider = SYSC_GetDIV0();
		ucPclkDiv = (uClkDivider>>12) & 0xf;

		SYSC_SetDIV0_all((uClkDivider & ~(0xf<<12)) | ((ucPclkDiv+2)<<12));
				
		SYSC_GetClkInform();		
		CalibrateDelay();
		UART_InitDebugCh(0, 115200);	
		
		bClockChange = true;		
	}
	
		
	HSI_SetPort();
	
	bHsiLoopback = true;
	g_eHsiOpMode=HSI_INT_MODE;
	bIsBreakTest = false;
		
	g_uHsiDmaTxCnt = 0;
	g_uHsiPreTxCount = 0;
	g_uHsiTxCount	= 0;
	bHsiTxDone	= false;
		
	g_uHsiDmaRxCnt = 0;
	g_uHsiRxCount = 0;
	bHsiRxDone	= false;
	
	g_eHsiFrMode = HSI_FRAME_MODE;
	
	//prepare data
	PrepareTxData();
	PrepareRxData();
	
	//ISR mapping and interrupt enabling
	INTC_SetVectAddr(NUM_HSItx, HSI_Tx_ISR);
	INTC_Enable(NUM_HSItx);
	
	INTC_SetVectAddr(NUM_HSIrx, HSI_Rx_ISR);
	INTC_Enable(NUM_HSIrx);
	
	//register setting
	HSI_SetForTx(g_eHsiOpMode, g_eHsiFrMode, g_eHsiChidMode, g_eHsiChidBits, g_eHsiChidValue);
	HSI_SetForRx(g_eHsiOpMode, g_eHsiFrMode, g_eHsiChidMode, g_eHsiChidBits);

	HSI_UnmaskTxInt(0x1f);	// unmask all interrupts
	HSI_UnmaskRxInt(0xfb);	// unmask all interrupts except rx-done

	//wait until all data have transmitted or test stops
	Disp("If you want to stop, press 'x' key...\n");
	
	while(!bHsiTxDone)
	{
		if(GetKey()=='x')
		{
			break;
		}
	}
	HSI_UnmaskTxInt(0x00);		
	//tx-interupt disable
	INTC_Disable(NUM_HSItx);
	
	Disp("TX COUNT : %d\n",g_uHsiTxCount);
	
	//wait until all data have received or test stops
	Disp("If you want to stop, press 'x' key...\n");
	
	while(!bHsiRxDone)
	{
		if(GetKey()=='x')
		{
			break;
		}
	}
	HSI_UnmaskRxInt(0x00);	
	//rx-interupt disable
	INTC_Disable(NUM_HSIrx);
	INTC_ClearVectAddr();
	
	Disp("RX COUNT : %d\n",g_uHsiRxCount);
	
	if (bClockChange == true)
	{
		SYSC_SetDIV0_all(uClkDivider);
		
		SYSC_GetClkInform();		
		CalibrateDelay();
		UART_InitDebugCh(0, 115200);
	}

	//compare rxData with txData
	bAutoTestResult = CompareTxRxData();
	if(!bAutoTestResult)
	{
		Disp("\n HSI TX/RX LOOPBACK TEST O.K.\n");
	}

	return (!bAutoTestResult);
}


//////////
// Function Name : HSI_LoopbackByDma_autotest
// Function Description : mipi hsi loopbak auto-test using DMA mode
// Input : NONE
// Output : NONE
// Version :
u8 HSI_LoopbackByDma_autotest(void)
{	
	u8 bAutoTestResult = false;
	u8 bClockChange = false;
	u32 uClkDivider;
	
	SYSC_GetClkInform();
	
	if ((float)g_PCLK/1.0e6 > 50)
	{
		u8 ucPclkDiv;
			
		uClkDivider = SYSC_GetDIV0();
		ucPclkDiv = (uClkDivider>>12) & 0xf;

		SYSC_SetDIV0_all((uClkDivider & ~(0xf<<12)) | ((ucPclkDiv+2)<<12));
				
		SYSC_GetClkInform();		
		CalibrateDelay();
		UART_InitDebugCh(0, 115200);	
		
		bClockChange = true;		
	}
			
	HSI_SetPort();
	
	bHsiLoopback = true;
	bIsBreakTest = false;
	g_eHsiOpMode = HSI_DMA_MODE;
	
	g_uHsiDmaTxCnt = 0;
	g_uHsiPreTxCount = 0;
	g_uHsiTxCount	= 0;
	bHsiTxDone	= false;
		
	g_uHsiDmaRxCnt = 0;
	g_uHsiRxCount = 0;
	bHsiRxDone	= false;	
	
	g_eHsiFrMode = HSI_FRAME_MODE;
	
	switch(g_eHsiDmaThresVal)
	{
		case HSI_DREQ_FULL:
			g_ucHsiDmaRxOnLevel = HSI_RXFIFO_DEPTH;
			break;
		case HSI_DREQ_16WORDS:
			g_ucHsiDmaRxOnLevel = 16;
			break;
		case HSI_DREQ_8WORDS:
			g_ucHsiDmaRxOnLevel = 8;
			break;
		case HSI_DREQ_4WORDS:
		default:
			g_ucHsiDmaRxOnLevel = 4;
			break;		 
	}
	
	//prepare data
	PrepareTxData();
	PrepareRxData();
	
	//ISR mapping and interrupt enabling
	INTC_SetVectAddr(NUM_HSItx, HSI_Tx_ISR);
	INTC_Enable(NUM_HSItx);
	
	INTC_SetVectAddr(NUM_HSIrx, HSI_Rx_ISR);
	INTC_Enable(NUM_HSIrx);
	
	//register setting
	HSI_SetForRx(g_eHsiOpMode, g_eHsiFrMode, g_eHsiChidMode, g_eHsiChidBits);
	HSI_SetForTx(g_eHsiOpMode, g_eHsiFrMode, g_eHsiChidMode, g_eHsiChidBits, g_eHsiChidValue);
	
	//////////
	// DMA0 Initailization
	SYSC_SelectDMA(eSEL_HSI_RX, 1);
	SYSC_SelectDMA(eSEL_HSI_TX, 1);
	DMAC_InitCh(DMA0 , DMA_ALL, &oHsiDmaRx);
	DMAC_InitCh(DMA0 , DMA_ALL, &oHsiDmaTx);

	INTC_SetVectAddr(NUM_DMA0 ,HSI_Dma_ISR);
	INTC_Enable(NUM_DMA0);
	
	// Interrupt Clear
	DMACH_ClearIntPending(&oHsiDmaRx);
	DMACH_ClearErrIntPending(&oHsiDmaRx);
	DMACH_ClearIntPending(&oHsiDmaTx);
	DMACH_ClearErrIntPending(&oHsiDmaTx);
	
	HSI_SetDmaForRx(HSI_TEST_COUNT);
	HSI_SetDmaForTx(HSI_TEST_COUNT);
	
	// Enable DMA
	DMACH_Start(&oHsiDmaRx);
	DMACH_Start(&oHsiDmaTx);
	//////////
	
	HSI_UnmaskRxInt(0xfb);	// unmask all interrupts except rx-done
	HSI_UnmaskTxInt(0x1e);	// unmask all interrupts except tx-fifo empty

	//wait until all data have transmitted or test stops
	Disp("If you want to stop, press 'x' key...\n");
	
	while(!bHsiTxDone)
	{
		if(GetKey()=='x')
		{
			break;
		}
	}
	HSI_DisableTxDma();
	
	DMACH_Stop(&oHsiDmaTx);
	
	//tx-interupt disable
	INTC_Disable(NUM_HSItx);
	
	Disp("TX COUNT : %d\n",g_uHsiTxCount);
	
	//wait until all data have received or test stops
	Disp("If you want to stop, press 'x' key...\n");
	
	while(!bHsiRxDone)
	{
		if(GetKey()=='x')
		{
			break;
		}
	}
	HSI_DisableRxDma();	
	
	DMACH_Stop(&oHsiDmaRx);
	INTC_Disable(NUM_DMA0);	
	
	DMAC_Close(DMA0 , DMA_ALL, &oHsiDmaTx);
	DMAC_Close(DMA0 , DMA_ALL, &oHsiDmaRx);
	
	//rx-interupt disable
	INTC_Disable(NUM_HSIrx);
	INTC_ClearVectAddr();
	
	Disp("RX COUNT : %d\n",g_uHsiRxCount);
	
	if (bClockChange == true)
	{
		SYSC_SetDIV0_all(uClkDivider);
		
		SYSC_GetClkInform();		
		CalibrateDelay();
		UART_InitDebugCh(0, 115200);
	}

	//compare rxData with txData
	bAutoTestResult = CompareTxRxData();
	if(!bAutoTestResult)
	{
		Disp("\n HSI TX/RX LOOPBACK TEST O.K.\n");
	}

	return (!bAutoTestResult);
}


//////////
// Function Name : Test_MipiHsi
// Function Description : mipi hsi main test function
// Input : NONE
// Output : NONE
// Version : 
void HSI_Test(void)
{
	int i, sel;

	const testFuncMenu menu[]=
	{		
		TestLoopbackByRxDone,				"Loopback Mode Test(Using only Rx-Done)\n",
		
		TestLoopbackByInt,					"Loopback Mode Test(Interrupt Mode)",
		TestBd2BdTxByInt,					"B'd to B'd Tx Test(Interrupt Mode)",
		TestBd2BdRxByInt,					"B'd to B'd Rx Test(Interrupt Mode)\n",
		
		TestLoopbackByDma,					"Loopback Mode Test(DMA Mode)",
		TestBd2BdTxByDma,					"B'd to B'd Tx Test(DMA Mode)",
		TestBd2BdRxByDma,					"B'd to B'd Rx Test(DMA Mode)",
		TestBd2BdRxWakeupByInt,			"Rx Wake up Source Test",

		HSI_LoopbackByInt_autotest,			"Loopback Auto Test(Interrupt Mode)",
		HSI_LoopbackByDma_autotest,		"Loopback Auto Test(DMA Mode)",
		
		0,0
	};
		
	HSI_SetPort();
	
	while(1)
	{		
		UART_Printf("\n");
		for (i=0; (int)(menu[i].desc)!=0; i++)
			UART_Printf("%2d: %s\n", i, menu[i].desc);

		UART_Printf("\nSelect the function to test : ");
		sel = UART_GetIntNum();
		UART_Printf("\n");

		if (sel == -1)
			break;
		else if (sel>=0 && sel<(sizeof(menu)/8-1))
			(menu[sel].func)();
	}	
}


