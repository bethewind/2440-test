//===================================================================
// NAME		: irda.c	
// DESC		: ver 1.1 compatible test module
// History	: 2003.06.25 edited by junon jeon from SMDK5410 test code
//          : 2003.10.09 Edited to verify IrDA core.by Minsoo, Lim
//          : 2003.10.31 Edited by Junon (DMA rx)
//          : 2006.03.29 Edited by rb1004
//===================================================================
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
*	File Name : irDA_test.c
*  
*	File Description : This file implements the functons for IrDA test
*
*	Author : Woojin.Kim
*	Dept. : AP Development Team
*	Created Date : 2007/01/25
*	Version : 0.1 
* 
*	History"
*	- Edited (Woojin.Kim 2007/01/25)
*  
**************************************************************************************/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "option.h"
#include "library.h"
#include "sfr6410.h"
#include "system.h"
#include "IrDA.h"
#include "sysc.h"
#include "gpio.h"
#include "intc.h"
#include "dma.h"
#include "timer.h"


extern u8 * pIrDA_RXBUFFER;
extern u8 * pIrDA_CKBUFFER;

extern	u32 g_uIrDA_RxWrPnt;
extern	u32 g_uIrDA_RxRdPnt;
extern	u32 g_uIrDA_RxISRcount;


extern	u32 g_uIrDA_TxDone;
extern	s32 g_uIrDA_TxCnt;
extern	s32 g_uIrDA_TxNum;
extern	u32 g_uIrDA_LastDataRead ;
extern	u32 g_uIrDA_Txcount;
extern	u32 g_uIrDA_TxISRcount;

extern 	u8* pIrDA_TXBUFFER;

extern u32 g_aIrDA_TestBuf[20];
extern u32 g_aIrDA_TestInt[10];

extern u8* g_pbHead;
extern u8* g_pbTail;

extern u32 g_uIrDA_DONE;
extern u32 g_uIrDA_RxDone;

extern DMAC oIrDADma;

//volatile u32 g_uIrDA_DONE, g_uIrDA_TxDone, g_uIrDA_RxDone, g_uIrDA_RxCount;
//volatile u32 g_uIrDA_RxWrPnt, g_uIrDA_RxRdPnt;

#define rIrDA_RBR 0x7f007020

void IrDA_Fifo_Rx(void);
void IrDA_Fifo_Tx(void);
void Irda_Fifo_Int_Rx(u32 MODE, u32 PREAMBLE, u32 STARTFLAG, u32 RXFL, u32 RXTRIG);
void Irda_Fifo_Int_Tx(u32 MODE,u32 PREAMBLE,u32 STARTFLAG,u32 TXFL,u32 TXTRIG);
void Test_Irda_Fifo_Dma_Rx(u32 MODE, u32 PREAMBLE, u32 STARTFLAG, u32 RXFL, u32 RXTRIG);
void Test_Irda_Fifo_Dma_Tx(u32 MODE, u32 PREAMBLE, u32 STARTFLAG, u32 TXFL, u32 TXTRIG);
void CompareData(void);

const testFuncMenu g_aIRDATestFunc[] =
{
	IrDA_Fifo_Rx,		"IrDA FIFO Rx",
	IrDA_Fifo_Tx,		"IrDA FIFO Tx",
	0,0
};

void IRDA_Test(void)
{
	u32 uCountFunc=0;
	s32 iSel=0;

	
	while(1)
	{
	UART_Printf("\n\n================== IrDA Function Test =====================\n\n");
	
		for (uCountFunc=0; (u32)(g_aIRDATestFunc[uCountFunc].desc)!=0; uCountFunc++)
			UART_Printf("%2d: %s\n", uCountFunc, g_aIRDATestFunc[uCountFunc].desc);

		UART_Printf("\nSelect the function to test : ");
		iSel =UART_GetIntNum();
		UART_Printf("\n");
		if(iSel == -1) 
			break;

		if (iSel>=0 && iSel<(sizeof(g_aIRDATestFunc)/8-1))
			(g_aIRDATestFunc[iSel].func) ();
	}
}	



//////////
// Function Name : IrDA_Fifo_Rx
// Function Description : This function select FIR/MIR mode and Interrupt/DMA mode to test IrDA FIFO Rx
// Input : NONE
// Output : NONE
// Version : v0.1
void IrDA_Fifo_Rx(void)
{
	s32 iMode;
	s32 sSelect;
	s32 sClk;
	u8 cSelPort;

	UART_Printf("Which Port do you want to use?\n");
	UART_Printf("Port 2, 3 : ");
	cSelPort = (u8)UART_GetIntNum();
	if(cSelPort <2 || cSelPort >3)
		cSelPort = 2;
	
	UART_Printf("\nSelect Operation Clock Source     1. Ext_48Mhz   2.MPLL   3.EPLL : \n" );
	sClk = UART_GetIntNum();
	IrDA_ClkSrc((eIrDA_ClkSrc)sClk);
	
	IrDA_SetPort(cSelPort);
	
	UART_Printf("\nSelect transfering mode	1. FIR(D)    2. MIR full  3. MIR half : \n");
	iMode = UART_GetIntNum() - 1;
	if (iMode == -2) iMode = 0; 
	
	UART_Printf("IrDA Interrupt Rx Test\n");
	UART_Printf("Select control mode : 1. Interrupt(D)    2. DMA\n");
	sSelect = UART_GetIntNum();
	if(sSelect == -1)
		sSelect = 1;

	if(sSelect == 2)
		{
		IrDA_IntClear();		
		Test_Irda_Fifo_Dma_Rx(iMode, PREAMBLE_VALUE, STARTFLAG_VALUE, RXFL_DMAVALUE, RXTR_DMAVALUE);
		}
	else
		{
		INTC_SetVectAddr(NUM_IRDA,Isr_IrDA_Int_Rx);		
		INTC_Enable(NUM_IRDA);
		IrDA_IntClear();		
		Irda_Fifo_Int_Rx(iMode, PREAMBLE_VALUE, STARTFLAG_VALUE, RXFL_VALUE, RXTR_VALUE);
		}
	IrDA_Reset();
	IrDA_ReturnPort();
	CompareData();
}


//////////
// Function Name : IrDA_Fifo_Tx
// Function Description : This function select FIR/MIR mode and Interrupt/DMA mode to test IrDA FIFO Tx
// Input : NONE
// Output : NONE
// Version : v0.1
void IrDA_Fifo_Tx(void)
{
	s32 iMode;
	s32 sSelect;
	s32 sClk;
	u8 cSelPort;

	UART_Printf("Which Port do you want to use?\n");
	UART_Printf("Port 2, 3 : ");
	cSelPort = (u8)UART_GetIntNum();
	if(cSelPort <2 || cSelPort >3)
		cSelPort = 2;

	UART_Printf("\nSelect Operation Clock Source     1. Ext_48Mhz   2.MPLL   3.EPLL : \n" );
	sClk = UART_GetIntNum();
	IrDA_ClkSrc((eIrDA_ClkSrc)sClk);
	
	IrDA_SetPort(cSelPort);

	UART_Printf("\nSelect transfering mode	1. FIR(D)    2. MIR full  3. MIR half : \n");
	iMode = UART_GetIntNum() - 1;
	if (iMode == -2) 
		iMode = 0; 

	
	UART_Printf("IrDA Interrupt Tx Test\n");
	UART_Printf("Select control mode : 1. Interrupt(D)    2. DMA\n");
	sSelect = UART_GetIntNum();
	if(sSelect == -1)
		sSelect = 1;

	if(sSelect == 2)
		{
		IrDA_IntClear();	
		Test_Irda_Fifo_Dma_Tx(iMode, PREAMBLE_VALUE, STARTFLAG_VALUE, TXFL_DMAVALUE, TXTR_DMAVALUE);
		}
	else
		{
		// Register IrDA ISR
		INTC_SetVectAddr(NUM_IRDA,Isr_IrDA_Int_Tx);
		INTC_Enable(NUM_IRDA);
		IrDA_IntClear();	
		Irda_Fifo_Int_Tx(iMode, PREAMBLE_VALUE, STARTFLAG_VALUE, TXFL_VALUE, TXTR_VALUE);
		}
	IrDA_Reset();
	IrDA_ReturnPort();
}




//////////
// Function Name : IrDA_Fifo_Int_Rx
// Function Description : This function test IrDA RX interrupt mode with FIFO
// Input : NONE
// Output : NONE
// Version : v0.1

void Irda_Fifo_Int_Rx(u32 MODE, u32 PREAMBLE, u32 STARTFLAG, u32 RXFL, u32 RXTRIG)
{
	u32	uCnt;

	// Initialize IrDA
	IrDA_Init(MODE, PREAMBLE, STARTFLAG, RXFL, RXTRIG);

	pIrDA_RXBUFFER=(u8 *)MemoryRx;
	//for(uCnt=0;uCnt<=IrDA_RxBUFLEN;uCnt++)
	for(uCnt=0;uCnt<1024;uCnt++)
		*pIrDA_RXBUFFER++=0;
	pIrDA_RXBUFFER=(u8 *)MemoryRx;	
	// Initialize RX Data -----------------END

	for(uCnt=0 ; uCnt<20 ; uCnt++)
		g_aIrDA_TestBuf[uCnt] = 0;
	
	g_uIrDA_RxWrPnt = 0;
	g_uIrDA_RxRdPnt = 0;
	g_uIrDA_RxISRcount = 0;

	IrDA_EnTx(0);
	IrDA_EnRx(1);
	
	

	IrDA_SetIER(1,1,0,0,1,1,0,1);
	IrDA_EnInt(1);

	uCnt = 0;
	
	UART_Printf("\t");
	while(1)
	{
		if (g_uIrDA_RxWrPnt > g_uIrDA_RxRdPnt) 
		{
			UART_Printf("[D:%3d] ",*(pIrDA_RXBUFFER+g_uIrDA_RxRdPnt));
			g_uIrDA_RxRdPnt++;
			if(!((uCnt+1)%64)) UART_Printf("\n");
			if(!((uCnt+1)%8)) UART_Printf("\n\t");
			uCnt++;
		}
		else if(g_uIrDA_LastDataRead == 1)
		{
			UART_Printf("\nLast Data is read from FIFO.\n");

			g_uIrDA_LastDataRead = 0;
		}
		if (UART_GetKey() == '\r') break;
	}
	
	for(uCnt=0 ; uCnt<20 ; uCnt++)
		UART_Printf("\n g_aIrDA_TestBuf[%d] = %d", uCnt, g_aIrDA_TestBuf[uCnt]);
	for(uCnt=0 ; uCnt<10 ; uCnt++)
		UART_Printf("\n g_aIrDA_TestInt[%d] = 0x%02x", uCnt, g_aIrDA_TestInt[uCnt]);
}


//////////
// Function Name : IrDA_Fifo_Int_Tx
// Function Description : This function test IrDA Tx interrupt mode with FIFO
// Input : NONE
// Output : NONE
// Version : v0.1

void Irda_Fifo_Int_Tx(u32 MODE,u32 PREAMBLE,u32 STARTFLAG,u32 TXFL,u32 TXTRIG)
{
	u32	uCnt;


	UART_Printf("Data length to transmit (multiple of %d): ",IrDA_TxBUFLEN);
	g_uIrDA_TxCnt = 0;
	g_uIrDA_TxNum = UART_GetIntNum();
	if (g_uIrDA_TxNum == -1) g_uIrDA_TxNum = IrDA_TxBUFLEN; //data length

	// Initilize IrDA with the FRAME SIZE given by user.
	IrDA_Init(MODE, PREAMBLE, STARTFLAG, TXFL, TXTRIG);


		
	// Initialize Check Data ----------------START
	pIrDA_CKBUFFER=(u8 *)MemoryCk;
	for (uCnt=0; uCnt<g_uIrDA_TxNum; uCnt++) {
//		*pIrDA_CKBUFFER++ = 'U';
//		*pIrDA_CKBUFFER++ = 0x0;			// for Performance test(to check signal)
		*pIrDA_CKBUFFER++ = uCnt;
	}
	pIrDA_CKBUFFER=(u8 *)MemoryCk;
	// Initialize Check Data -----------------END

	g_uIrDA_TxDone = 0;

	IrDA_EnRx(0);
	IrDA_EnTx(1);
	
	
	IrDA_SetIER(1,1,1,1,1,1,1,1);
	
	IrDA_EnInt(1);


	while(!g_uIrDA_TxDone);

	while(!(IrDA_ReadLSR() & 0x80));			//Check Tx FIFO empty
	UART_Printf("Tx buffer is empty!!, Tx Done!!\n");

	UART_Printf("[Tx String :\n");
	pIrDA_CKBUFFER  = (u8 *)MemoryCk;	

	UART_Printf("\t");
	for(uCnt=0; uCnt<g_uIrDA_TxNum ; uCnt++)
	{
		UART_Printf("[D:%3d] ", *pIrDA_CKBUFFER++);
		if(!((uCnt+1)%64)) UART_Printf("\n");
		if(!((uCnt+1)%8)) UART_Printf("\n\t");
	}
	UART_Printf("\n");

	IrDA_SetIER(0,0,0,0,0,0,0,0);

	IrDA_EnInt(0);


}	

void Test_Irda_Fifo_Dma_Rx(u32 MODE,u32 PREAMBLE,u32 STARTFLAG,u32 RXFL,u32 RXTRIG)
{
	u32 i;
	
	
	// Initialize Check Data ----------------START
	pIrDA_RXBUFFER=(u8 *)MemoryRx;
	g_pbHead=(u8 *)MemoryCk;
	g_pbTail=(u8 *)MemoryCk;

	g_uIrDA_DONE = 0;	

	for(i=0 ; i<1024 ; i++)
		*g_pbHead++ = 0;
	g_pbHead=(u8 *)MemoryCk;

	INTC_SetVectAddr(NUM_DMA1,Isr_IrDA_Dma_RxDone);


	Init_Irda_Dma_Rx();
	IrDA_Init(MODE, PREAMBLE, STARTFLAG, RXFL, RXTRIG); // Initialize IrDA

	IrDA_EnDMA(1);					//enable Rx DMA
	IrDA_SetIER(1,1,1,1,1,1,1,1);
	IrDA_EnTx(0);
	IrDA_EnRx(1);					//enable Rx


	i=0;
	UART_Printf("\nRx String :\n\t");
	while(1)
	{
		if (g_uIrDA_DONE) 
		{
			while (g_pbTail != g_pbHead)
			{
				UART_Printf("[D:%3d] ", *g_pbTail);
				g_pbTail++;
				if(!((i+1)%64)) UART_Printf("\n");
				if(!((i+1)%8)) UART_Printf("\n\t");
				i++;
			}
		
			g_uIrDA_DONE=0;
			UART_Printf(".\n");	

			if(IrDA_ReadLSR() & 0x01)
			{
				pIrDA_RXBUFFER=(u8 *)MemoryRx;
				g_pbHead=(u8 *)MemoryCk;
				for(i=0 ; i<1024 ; i++)
					*g_pbHead++ = 0;
				g_pbTail = g_pbHead=(u8 *)MemoryCk;
				DMACH_AddrSetup(DMA_A, rIrDA_RBR, (u32)(pIrDA_RXBUFFER), &oIrDADma);
			}
		}
		if (UART_GetKey() == '\r') break;
	}

	DMACH_Stop(&oIrDADma);
	DMAC_Close(DMA1,DMA_A,&oIrDADma);

	IrDA_EnRx(0);
	
	INTC_Disable(NUM_DMA1);
 
}


void Test_Irda_Fifo_Dma_Tx(u32 MODE,u32 PREAMBLE,u32 STARTFLAG,u32 TXFL,u32 TXTRIG)
{
	u32 i;
	u32 uRegTmp;
	s32 IrDA_DAMTxNum = 0;
	
	UART_Printf("Data length to transmit (multiple of %d): ",DMAtoIrDA_TX_SIZE);
	IrDA_DAMTxNum = UART_GetIntNum(); //data length

	if (IrDA_DAMTxNum == -1) 
		IrDA_DAMTxNum = DMAtoIrDA_TX_SIZE; //data length

	g_uIrDA_Txcount = 0;
	g_uIrDA_TxISRcount = 0;
	g_uIrDA_DONE = 0;
	
	pIrDA_TXBUFFER=(u8 *)MemoryTx;
	for(i=0;i<IrDA_DAMTxNum;i++)
		*pIrDA_TXBUFFER++ = i;

	pIrDA_TXBUFFER=(u8*)MemoryTx;

	INTC_SetVectAddr(NUM_DMA1,Isr_IrDA_Dma_TxDone);


	
	IrDA_Init(MODE, PREAMBLE, STARTFLAG, TXFL, TXTRIG); // Initialize IrDA

	

	IrDA_EnDMA(0);			//Tx DMA

	IrDA_SetIER(1,1,1,1,1,1,1,1);
	IrDA_EnRx(0);
	IrDA_EnTx(1);			//enable Tx
	
	Init_Irda_Dma_Tx();				//as soon as startDMA, it transfer data even though IrDa is not ready
									//therefore DMA shoud be started after IrDA set up when Tx

	while(1)
	{
		if(g_uIrDA_DONE)
		{
			if(g_uIrDA_Txcount < IrDA_DAMTxNum)
			{
				g_uIrDA_DONE = 0;

				uRegTmp = DMACH_ReadSrcAddr(&oIrDADma);
				uRegTmp+=1;
				DMACH_Setup(DMA_B,0x0,uRegTmp,0,rIrDA_RBR,1,BYTE,DMAtoIrDA_TX_SIZE,DEMAND,MEM,DMA1_IrDA,BURST32,&oIrDADma);					
				DMACH_Start(&oIrDADma);
			}
			else
			{

				DMACH_Stop(&oIrDADma);				
				while(!(IrDA_ReadLSR() & 0x80));  //Check IrDA FIFO Empty
				IrDA_EnTx(0);
				DMAC_Close(DMA1,DMA_B,&oIrDADma);
				break;				
			}
		}
	}


	UART_Printf("\nTEST_IrDA_FIFO_DMA_TX is done");	
	UART_Printf("\nTx String :\n\t");

	for(i=0;i<IrDA_DAMTxNum;i++)
	{
		UART_Printf("[D:%3d] ", *pIrDA_TXBUFFER++);
		if(!((i+1)%64)) UART_Printf("\n");
		if(!((i+1)%8)) UART_Printf("\n\t");
	}

	INTC_Disable(NUM_DMA1);

}	
// IrDA DMA Test Code ============================================================== [END]

/*

// added by junon
void Test_IrDA_GPIO(void)
{
	int i;
	
	IrDA_SetPort();

	// GPIO port test	
	rGPHCON &= ~(0x3f<<18);	// GP28~31 output
	rGPHCON |= (0x15<<18);	// GP28~31 output
	rGPHPU &= ~(0x7<<9); 	//Uart port pull-up

	UART_Printf("Press Any key...GP16~18 will be triggered... if you want to exit, press Enter key");
	while(UART_Getc() != '\r') 
//		rGPDAT &=~(0xf<<28);
	{	
		i++;
		if ( (i%2) == 0 )	
			rGPHDAT |= (0x7<<9);
		else 
			rGPHDAT &=~(0x7<<9);
	}

	IrDA_ReturnPort();
}       


void Test_IrDA_Polling(void)
{
	// Initialize RX Buffer---------------START
	unsigned char c;
	pIrDA_RXBUFFER=(unsigned char *)MemoryRx;
	for(c=0;c<IrDA_BUFLEN;c++)
		*pIrDA_RXBUFFER++=0;
	*pIrDA_RXBUFFER=0;
	pIrDA_RXBUFFER=(unsigned char *)MemoryRx;	
	UART_Printf("IrDA Polling Rx!!!!\n");
	// Initialize RX Buffer-----------------END

	IrDA_SetPort();

	IrDA_Init(MODE_VALUE, PREAMBLE_VALUE, STARTFLAG_VALUE, RXFL_VALUE, RXTR_VALUE);
	
	IrDA_ReturnPort();
}
*/

void CompareData(void)
{
	u32 uCnt;
	u32 uBCnt;
	u32 uFcnt=0;


		// Clear Check Data ----------------START
		pIrDA_CKBUFFER=(u8 *)MemoryCk;
		for (uCnt=0; uCnt<0x100000; uCnt++)
			{
	//		*pIrDA_CKBUFFER++ = 'U';
			*pIrDA_CKBUFFER++ = 0x0;			// for Performance test(to check signal)
//			*pIrDA_CKBUFFER++ = uCnt;
			}
		pIrDA_CKBUFFER=(u8 *)MemoryCk;
		// Clear Check Data -----------------END

	

	UART_Printf("\nCount Number : ");
	uBCnt=UART_GetIntNum();
	UART_Printf("\n");

		// Initialize Check Data ----------------START
		pIrDA_CKBUFFER=(u8 *)MemoryCk;
		for (uCnt=0; uCnt<uBCnt; uCnt++)
			{
	//		*pIrDA_CKBUFFER++ = 'U';
	//		*pIrDA_CKBUFFER++ = 0x0;			// for Performance test(to check signal)
			*pIrDA_CKBUFFER++ = uCnt;
			}
		pIrDA_CKBUFFER=(u8 *)MemoryCk;
		// Initialize Check Data -----------------END
	

	
	pIrDA_RXBUFFER=(u8 *)MemoryRx;

		for (uCnt=0; uCnt<=uBCnt; uCnt++)
			{
			if(*pIrDA_CKBUFFER != *pIrDA_RXBUFFER)
				uFcnt++;

			*pIrDA_CKBUFFER++;
			*pIrDA_RXBUFFER++;			
			}

		UART_Printf("Rx data Check Done!!\n");
		UART_Printf("%d times have been failed\n",uFcnt);
		
		

}


