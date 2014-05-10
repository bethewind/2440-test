/**************************************************************************************
* 
*	Project Name : S3C6410 Validation
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
*	File Name : DMA_test.c
*  
*	File Description : This file implements the functons for DMA controller test.
*
*	Author : Wonjoon Jang
*	Dept. : AP Development Team
*	Created Date : 2007/01/02
*	Version : 0.1 
* 
*	History
*	- Created(wonjoon.jang 2007/01/02)
*	- Modified(sunil.roe 2007)
*	- Modified for 6410(woojin.kim 2008/02/27)
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
#include "gpio.h"

#include "timer.h"

static void DMAT_MemtoMem(void);
static void DMAT_MemtoMemLLI(void);
void Test_DMA(void);

volatile int g_DmaDone;
volatile int g_DmaDone0, g_DmaDone1, g_DmaDone2, g_DmaDone3;
volatile int g_TSize = 0x100000;

// SMDK6400 _DRAM_BaseAddress + 0x800_0000;
u32	uLLIBaseAddr = _DRAM_BaseAddress + 0x3000000;

u32 uTxBuffAddr = _DRAM_BaseAddress + 0x1000000;
u32 uRxBuffAddr = _DRAM_BaseAddress + 0x2000000;

#define uRxBuffAddr0 _DRAM_BaseAddress + 0x2000000
#define uRxBuffAddr1 uRxBuffAddr0 + 0x00800000
#define uRxBuffAddr2 uRxBuffAddr0 + 0x01000000
#define uRxBuffAddr3 uRxBuffAddr0 + 0x01800000

#define DMAC0TxSize		0x400
#define DMAC1TxSize		0xf0000
#define SDMAC0TxSize	0xf0000
#define SDMAC1TxSize	0xf0000

u32 uDataCnts=50000;

static DMAC oDmac0, oDmac1, oDmac2, oDmac3;


// Temp. function
void MemoryDump( u32 *DumpAddress, u32 line)
{
	u32 i,j;
	
	for(i=0;i<line;i++) {
		UART_Printf("%08x : ",(u32)DumpAddress);
		for(j=0;j<8;j++) {
			UART_Printf("%08x ",*DumpAddress++);
		}
		putchar('\n');
	}
	putchar('\n');
}


void Mem_dump (void)
{

	int ch;
	
	
	 while(1)
    {
    	UART_Printf("dump address, '-1' to Exit \n");
    	//ch = Uart_getc();
    	ch = UART_GetIntNum();
    		if(ch==-1)
    	{
    		break;
    	}
    	else
    	{
    	    	
    	MemoryDump( ((u32 *)(ch)), 40);
    	}

    }

}

u8 CompareDMA(u32 a0, u32 a1, DATA_SIZE eDataSize, u32 uDataCnt)
{
	u32* pD0 = (u32 *)a0;
	u32* pD1 = (u32 *)a1;
	
	u32	uSrcData = 0;
	u32	uDstData = 0;
	u8 ret = true;
	u32	uLoopCnt = 0;
	u32	uTotDataCnt = uDataCnt * eDataSize;		// Bytes
//	u32	uTotLoopCnt = uTotDataCnt/4 + 1;
	u32	uTotLoopCnt = uTotDataCnt/4;

	DelayfrTimer(micro, 100);
	for (uLoopCnt=0; uLoopCnt< uTotLoopCnt; uLoopCnt++)		// 4Byte 단위 비교 
	{
		uSrcData = *pD0;
		uDstData = *pD1;
		if (uSrcData != uDstData) 
		{
			ret = false;
			UART_Printf(" %08x=%08x <-> %08x=%08x\n", pD0, uSrcData, pD1, uDstData);
			Getc();
		}
		pD0++;
		pD1++;
	}

	if (ret == false)
		UART_Printf("\n");

	return ret;
}


u8 Compare32(u32 a0, u32 a1, u32 words)
{
	u32* pD0 = (u32 *)a0;
	u32* pD1 = (u32 *)a1;
	u8 ret = true;
	u32  ecnt = 0;
	u32 i;

	for (i=0; i<words; i++)
	{
		if (*pD0 != *pD1) 
		{
			ret = false;
			UART_Printf(" %08x=%08x <-> %08x=%08x\n", pD0, *pD0, pD1, *pD1);
			ecnt++;
		}
		pD0++;
		pD1++;
	}

	if (ret == false)
		UART_Printf("\n");

	return ret;
}

//////////
// Function Name : DmaxDone
// Function Description : DMA ISR Routine
// Input : 	None
// Output :	None 
// Version : v0.1
void __irq Dma0Done(void)
{
    
  	DMACH_ClearIntPending(&oDmac0);
 	//rDMAC0IntTCClear = 0xff;
	UART_Printf ("DMA ISR %d\n", g_DmaDone);
	g_DmaDone=1;
	g_DmaDone++;
	INTC_ClearVectAddr();
       //Write_VECTADDR(0x0);
}

void __irq Dma1Done(void)
{
//	UART_Printf("Current TC1 : %x\n",rDMAC1C0Control);

	DMACH_ClearIntPending(&oDmac1);
	//rDMAC1IntTCClear = 0xff;
	UART_Printf ("DMA ISR %d\n", g_DmaDone);
	g_DmaDone=1;
	g_DmaDone++;
	INTC_ClearVectAddr();
}

void __irq Sdma0Done(void)
{
	DMACH_ClearIntPending(&oDmac2);
	//rDMAC1IntTCClear = 0xff;
	UART_Printf ("DMA ISR %d\n", g_DmaDone);
	g_DmaDone=1;
	g_DmaDone++;
	INTC_ClearVectAddr();
}

void __irq Sdma1Done(void)
{
	DMACH_ClearIntPending(&oDmac3);
	//rDMAC1IntTCClear = 0xff;
	UART_Printf ("DMA ISR %d\n", g_DmaDone);
	g_DmaDone=1;
	g_DmaDone++;
	INTC_ClearVectAddr();
}

//////////
// Function Name : DmaxDoneW
// Function Description : DMA ISR Routine
// Input : 	None
// Output :	None 
// Version : v0.1
void __irq Dma0DoneW(void)
{
   	DMACH_ClearIntPending(&oDmac0);	
//	UART_Printf("DMA0 ISR %d\n", g_DmaDone0);

	if ( g_DmaDone0 < (g_TSize/DMAC0TxSize) )
	{
		DMACH_SetTransferSize(DMAC0TxSize,&oDmac0);					// set TransferSize
		DMACH_Start(&oDmac0);			
	}
	else
	{
		INTC_Disable(NUM_DMA0);
	}
	g_DmaDone0++;
	INTC_ClearVectAddr();
}

void __irq Dma1DoneW(void)
{
   	DMACH_ClearIntPending(&oDmac1);	
//	UART_Printf("DMA1 ISR %d\n", g_DmaDone1);

	if ( g_DmaDone1 < (g_TSize/DMAC1TxSize) )
	{
		DMACH_SetTransferSize(DMAC1TxSize,&oDmac1);					// set TransferSize
		DMACH_Start(&oDmac1);			
	}
	else
	{
		INTC_Disable(NUM_DMA1);
	}
	g_DmaDone1++;
	INTC_ClearVectAddr();
}

void __irq Sdma0DoneW(void)
{
   	DMACH_ClearIntPending(&oDmac2);	
//	Disp ("SDMA0 ISR %d\n", g_DmaDone2);

	if ( g_DmaDone2 < (g_TSize/SDMAC0TxSize) )
	{
		DMACH_SetTransferSize(SDMAC0TxSize,&oDmac2);					// set TransferSize
		DMACH_Start(&oDmac2);			
	}
	else
	{
		INTC_Disable(NUM_SDMA0);
	}
	g_DmaDone2++;
	INTC_ClearVectAddr();
}

void __irq Sdma1DoneW(void)
{
   	DMACH_ClearIntPending(&oDmac3);	
//	Disp ("SDMA1 ISR %d\n", g_DmaDone3);

	if ( g_DmaDone3 < (g_TSize/SDMAC1TxSize) )
	{
		DMACH_SetTransferSize(SDMAC1TxSize,&oDmac3);					// set TransferSize
		DMACH_Start(&oDmac3);			
	}
	else
	{
		INTC_Disable(NUM_SDMA1);
	}
	g_DmaDone3++;
	INTC_ClearVectAddr();

}

//////////
// Function Name : DMAT_MemtoMem
// Function Description : Memory to Memory Transfer Test
// Input : 	None
// Output :	None 
// Version : v0.1
static void DMAT_MemtoMem(void)
{
	u32 i, csel, usel;
	u32 uTsize, uBurst, uCh;

	UART_Printf("\nSelect DMA Controller : 0:DMA0, 1:DMA1, 2:SDMA0, 3:SDMA1	: ");
	csel=UART_GetIntNum();
	UART_Printf("\n");

	switch(csel)
	{
		case 0: 
				UART_Printf("Selected DMAC 0 ..... \n");
			     	DMAC_InitCh(DMA0, DMA_ALL, &oDmac0);
				INTC_SetVectAddr(NUM_DMA0,  Dma0Done);
				INTC_Enable(NUM_DMA0);
			     	break;
               
		case 1: 
				UART_Printf("Selected DMAC 1 ..... \n");
			     	DMAC_InitCh(DMA1, DMA_ALL, &oDmac1);
				INTC_SetVectAddr(NUM_DMA1,  Dma1Done);
				INTC_Enable(NUM_DMA1);
			     	break;
		case 2: 
				UART_Printf("Selected SDMAC 0 ..... \n");
			     	DMAC_InitCh(SDMA0, DMA_ALL, &oDmac2);
				INTC_SetVectAddr(NUM_SDMA0, Sdma0Done );
				INTC_Enable(NUM_SDMA0);
			     	break;
		case 3: 
				UART_Printf("Selected SDMAC 1 ..... \n");
			     	DMAC_InitCh(SDMA1, DMA_ALL, &oDmac3);
				INTC_SetVectAddr(NUM_SDMA1, Sdma1Done);
				INTC_Enable(NUM_SDMA1);
			     	break;
			     	
		default : Assert(0);
	}
       

       UART_Printf("\nSelect Channel : 0:CH0, 1:CH1, 2:CH2, 3:CH3, 4:CH4, 5:CH5, 6:CH6, 7:CH7	: ");
	usel=UART_GetIntNum();
	UART_Printf("\n");

	switch(usel)
	{
		case 0: 
				uCh= DMA_A;
			     	break;             
		case 1: 
				uCh= DMA_B;
			     	break;
		case 2: 
				uCh= DMA_C;
			     	break;
		case 3: 
				uCh = DMA_D;
			     	break;	     	
		case 4: 
				uCh = DMA_E;
			     	break;
		case 5: 
				uCh = DMA_F;
			     	break;
		case 6: 
				uCh = DMA_G;
			     	break;
		case 7: 
				uCh = DMA_H;
			     	break;				
			     	
		default : Assert(0);
	}



	UART_Printf("\nSelect Transfer Width : 0:BYTE, 1:HWORD, 2:WORD	: ");
	usel=UART_GetIntNum();
	UART_Printf("\n");

       switch(usel)
	{
		case 0: 
				uTsize = BYTE;
			     	break;
               
		case 1: 
				uTsize = HWORD;
			     	break;
		case 2: 
				uTsize = WORD;
			     	break;
					     	
		default : uTsize = WORD;
			     	
//			Assert(0);
	}

       UART_Printf("\nSelect Burst Size : 0:SINGLE, 1:BURST4, 2:BURST8, 3:BURST16, 4:BURST32, 5:BURST64, 6:BURST128, 7:BURST256	: ");
	usel=UART_GetIntNum();
	UART_Printf("\n");

	switch(usel)
	{
		case 0: 
				uBurst = SINGLE;
			     	break;             
		case 1: 
				uBurst = BURST4;
			     	break;
		case 2: 
				uBurst = BURST8;
			     	break;
		case 3: 
				uBurst = BURST16;
			     	break;	     	
		case 4: 
				uBurst = BURST32;
			     	break;
		case 5: 
				uBurst = BURST64;
			     	break;
		case 6: 
				uBurst = BURST128;
			     	break;
		case 7: 
				uBurst = BURST256;
			     	break;				
			     	
		default :	uBurst = SINGLE;
			     	
			//Assert(0);
	}
  
       UART_Printf("\nSelect Transfer Size [1~0x200_0000] :   ");
	uDataCnts=UART_GetIntNum();
	UART_Printf("\n");

	// 0. Clear the rx/tx buf.
	for (i = 0; i<(uDataCnts*uTsize+16); i++)
	{
		*(u8 *)(uRxBuffAddr+i) = 0;
		*(u8 *)(uTxBuffAddr+i) = 0;
	}
	
	// 1. Set up the tx buf.
	for (i = 0; i<uDataCnts*uTsize; i++)
		*(u8 *)(uTxBuffAddr+i) = (u8)(i+2)%0xff;

	g_DmaDone=0;

	switch(csel)
	{
		case 0 :
		        // Channel, LLI_Address, SrcAddr, Src Type, DstAddr, Dst Type, Transfer Width, Transfer Size, OpMode(DEMAND), Src Req, Dst Req, Burst
			DMACH_Setup((DMA_CH)uCh, 0x0, uTxBuffAddr, 0, uRxBuffAddr, 0, (DATA_SIZE)uTsize, uDataCnts, DEMAND, MEM, MEM, (BURST_MODE)uBurst, &oDmac0);
	       	DMACH_Start(&oDmac0);
			break;
		case 1 :
		        // Channel, LLI_Address, SrcAddr, Src Type, DstAddr, Dst Type, Transfer Width, Transfer Size, OpMode(DEMAND), Src Req, Dst Req, Burst
			DMACH_Setup((DMA_CH)uCh, 0x0, uTxBuffAddr, 0, uRxBuffAddr, 0, (DATA_SIZE)uTsize, uDataCnts, DEMAND, MEM, MEM, (BURST_MODE)uBurst, &oDmac1);
	       	DMACH_Start(&oDmac1);
			break;
		case 2 :
		        // Channel, LLI_Address, SrcAddr, Src Type, DstAddr, Dst Type, Transfer Width, Transfer Size, OpMode(DEMAND), Src Req, Dst Req, Burst
			DMACH_Setup((DMA_CH)uCh, 0x0, uTxBuffAddr, 0, uRxBuffAddr, 0, (DATA_SIZE)uTsize, uDataCnts, DEMAND, MEM, MEM, (BURST_MODE)uBurst, &oDmac2);
	       	DMACH_Start(&oDmac2);
			break;
		case 3 :
		        // Channel, LLI_Address, SrcAddr, Src Type, DstAddr, Dst Type, Transfer Width, Transfer Size, OpMode(DEMAND), Src Req, Dst Req, Burst
			DMACH_Setup((DMA_CH)uCh, 0x0, uTxBuffAddr, 0, uRxBuffAddr, 0, (DATA_SIZE)uTsize, uDataCnts, DEMAND, MEM, MEM, (BURST_MODE)uBurst, &oDmac3);
	       	DMACH_Start(&oDmac3);
			break;
		default :
			Assert(0);
	}

	while(g_DmaDone==0);								// Int.
	//while (!DMAC_IsTransferDone(&oDmac0));			// Polling

	if (CompareDMA(uTxBuffAddr, uRxBuffAddr, (DATA_SIZE)uTsize, uDataCnts))
		UART_Printf(" >> Test Tx&Rx -> Ok << \n");
	else
	{
		UART_Printf(" >>*** Tx-data & Rx-data mismatch ***<< \n");
	}


	switch(csel)
	{
		case 0: 
				INTC_Disable(NUM_DMA0);
			    	DMAC_Close(DMA0, DMA_ALL, &oDmac0);
			     	break;
               
		case 1: 
				INTC_Disable(NUM_DMA1);
				DMAC_Close(DMA1, DMA_ALL, &oDmac1);
			     	break;
		case 2: 
				INTC_Disable(NUM_SDMA0);
				DMAC_Close(SDMA0, DMA_ALL, &oDmac2);
			     	break;
		case 3: 
				INTC_Disable(NUM_SDMA1);
				DMAC_Close(SDMA1, DMA_ALL, &oDmac3);
			     	break;
			     	
		default : Assert(0);
	}
	

}

u32 DMAT_GetBurstSize(BURST_MODE eBurst)
{
	u32	uBurstSize = 0;

	switch(eBurst)
	{
		case SINGLE :
			uBurstSize = 1;
			break;
		case BURST4 :
			uBurstSize = 4;
			break;
		case BURST8 :
			uBurstSize = 8;
			break;
		case BURST16 :
			uBurstSize = 16;
			break;
		case BURST32 :
			uBurstSize = 32;
			break;
		case BURST64 :
			uBurstSize = 64;
			break;
		case BURST128 :
			uBurstSize = 128;
			break;
		case BURST256 :
			uBurstSize = 256;
			break;
	}
	return uBurstSize;
}
//////////
// Function Name : DMAT_SetLLI
// Function Description : This function set the DMA LLI feature
// Input : 	uSrcAddr - Source Address of Data
//			uDstAddr - Dest Address of Data
//			eTransferWidth - BYTE/HWORD/WORD
//			eBurst	- Burst mode
//			uDataCnts - data number which user inputs
// Output : 	None
// Version : v0.1
void DMAT_SetLLI(u32 uSrcAddr, u32 uDstAddr, DATA_SIZE eTransferWidth, BURST_MODE eBurst, u32 uDataCnts)
{
	u32	*pLLI_Base = NULL;
	u32	uLoopCnt = 0;
	u32	uOneTimeTxDataSize = 0;
	u32	uTotTxDataSize		= 0;
	u32	uMaxLoopCnt 		= 0;

	uOneTimeTxDataSize 	= eTransferWidth*DMAT_GetBurstSize(eBurst);
	uTotTxDataSize		= eTransferWidth*uDataCnts;
	uMaxLoopCnt 			= (uTotTxDataSize%uOneTimeTxDataSize == 0) ? uTotTxDataSize/uOneTimeTxDataSize : (uTotTxDataSize/uOneTimeTxDataSize + 1);

	pLLI_Base = (u32 *)uLLIBaseAddr;

	for(uLoopCnt=0 ; uLoopCnt<uMaxLoopCnt; uLoopCnt++)
	{
		*pLLI_Base++ = uSrcAddr + (uLoopCnt*uOneTimeTxDataSize);
		*pLLI_Base++ = uDstAddr + (uLoopCnt*uOneTimeTxDataSize);
		*pLLI_Base++ = (uLLIBaseAddr + ((uLoopCnt+1)*0x20));
		*pLLI_Base++ = (1<<27) | (1<<26) | (0<<25) | (0<<24) | ((eTransferWidth>>1)<<21)|((eTransferWidth>>1)<<18)|(eBurst<<15)|(eBurst<<12);
//		*pLLI_Base++ = uDataCnts;
		*pLLI_Base++ = uOneTimeTxDataSize;
		pLLI_Base += 3;
	}

	*pLLI_Base++ = uSrcAddr + (uLoopCnt*uOneTimeTxDataSize);
	*pLLI_Base++ = uDstAddr + (uLoopCnt*uOneTimeTxDataSize);
	*pLLI_Base++ = 0;				// 	Finish LLI operation
//	*pLLI_Base++ = uLLIBaseAddr;		// 	Loop DMA LLI operation
	*pLLI_Base++ = (1<<31) | (1<<27) | (1<<26) | (0<<25) | (0<<24) | ((eTransferWidth>>1)<<21)|((eTransferWidth>>1)<<18)|(eBurst<<15)|(eBurst<<12);
//	*pLLI_Base++ = uDataCnts;
	*pLLI_Base++ = uOneTimeTxDataSize;
}

//////////
// Function Name : DMAT_MemtoMemLLI
// Function Description : Memory to Memory Transfer Test
// Input : 	None
// Output :	None 
// Version : v0.1
static void DMAT_MemtoMemLLI(void)
{
	u32 i, csel, usel;
	u32 uTsize, uBurst, uCh;

	g_DmaDone=0;
	
	UART_Printf("\nSelect DMA Controller : 0:DMA0, 1:DMA1, 2:SDMA0, 3:SDMA1	: ");
	csel=UART_GetIntNum();
	UART_Printf("\n");

	switch(csel)
	{
		case 0: 
				UART_Printf("Selected DMAC 0 ..... \n");
			     	DMAC_InitCh(DMA0, DMA_ALL, &oDmac0);
				INTC_SetVectAddr(NUM_DMA0,  Dma0Done);
				INTC_Enable(NUM_DMA0);
			     	break;
               
		case 1: 
				UART_Printf("Selected DMAC 1 ..... \n");
			     	DMAC_InitCh(DMA1, DMA_ALL, &oDmac1);
				INTC_SetVectAddr(NUM_DMA1,  Dma1Done);
				INTC_Enable(NUM_DMA1);
			     	break;
		case 2: 
				UART_Printf("Selected SDMAC 0 ..... \n");
			     	DMAC_InitCh(SDMA0, DMA_ALL, &oDmac2);
				INTC_SetVectAddr(NUM_SDMA0, Sdma0Done );
				INTC_Enable(NUM_SDMA0);
			     	break;
		case 3: 
				UART_Printf("Selected SDMAC 1 ..... \n");
			     	DMAC_InitCh(SDMA1, DMA_ALL, &oDmac3);
				INTC_SetVectAddr(NUM_SDMA1, Sdma1Done);
				INTC_Enable(NUM_SDMA1);
			     	break;
			     	
		default : Assert(0);
	}
       

       UART_Printf("\nSelect Channel : 0:CH0, 1:CH1, 2:CH2, 3:CH3, 4:CH4, 5:CH5, 6:CH6, 7:CH7	: ");
	usel=UART_GetIntNum();
	UART_Printf("\n");

	switch(usel)
	{
		case 0: 
				uCh= DMA_A;
			     	break;             
		case 1: 
				uCh= DMA_B;
			     	break;
		case 2: 
				uCh= DMA_C;
			     	break;
		case 3: 
				uCh = DMA_D;
			     	break;	     	
		case 4: 
				uCh = DMA_E;
			     	break;
		case 5: 
				uCh = DMA_F;
			     	break;
		case 6: 
				uCh = DMA_G;
			     	break;
		case 7: 
				uCh = DMA_H;
			     	break;				
			     	
		default : Assert(0);
	}



	UART_Printf("\nSelect Transfer Width : 0:BYTE, 1:HWORD, 2:WORD	: ");
	usel=UART_GetIntNum();
	UART_Printf("\n");

       switch(usel)
	{
		case 0: 
				uTsize = BYTE;
			     	break;
               
		case 1: 
				uTsize = HWORD;
			     	break;
		case 2: 
				uTsize = WORD;
			     	break;
					     	
		default : Assert(0);
	}

       UART_Printf("\nSelect Burst Size : 0:SINGLE, 1:BURST4, 2:BURST8, 3:BURST16, 4:BURST32, 5:BURST64, 6:BURST128, 7:BURST256	: ");
	usel=UART_GetIntNum();
	UART_Printf("\n");

	switch(usel)
	{
		case 0: 
				uBurst = SINGLE;
			     	break;             
		case 1: 
				uBurst = BURST4;
			     	break;
		case 2: 
				uBurst = BURST8;
			     	break;
		case 3: 
				uBurst = BURST16;
			     	break;	     	
		case 4: 
				uBurst = BURST32;
			     	break;
		case 5: 
				uBurst = BURST64;
			     	break;
		case 6: 
				uBurst = BURST128;
			     	break;
		case 7: 
				uBurst = BURST256;
			     	break;				
			     	
		default : Assert(0);
	}
  
       UART_Printf("\nSelect Transfer Size [1~0x200_0000] :   ");
	uDataCnts=UART_GetIntNum();
	UART_Printf("\n");

	// 0. Clear the rx/tx buf.
	for (i = 0; i<(uDataCnts*uTsize+16); i++)
	{
		*(u8 *)(uRxBuffAddr+i) = 0;
		*(u8 *)(uTxBuffAddr+i) = 0;
	}
	
	// 1. Set up the tx buf.
	for (i = 0; i<uDataCnts*uTsize; i++)
		*(u8 *)(uTxBuffAddr+i) = (u8)(i+2)%0xff;

	DMAT_SetLLI(uTxBuffAddr, uRxBuffAddr, (DATA_SIZE)uTsize, (BURST_MODE)uBurst, uDataCnts);
	UART_Printf("LLI Table Composition complete... now, DMA Start!!\n");
	switch(csel)
	{
		case 0 :
		        // Channel, LLI_Address, SrcAddr, Src Type, DstAddr, Dst Type, Transfer Width, Transfer Size, OpMode(DEMAND), Src Req, Dst Req, Burst
			DMACH_Setup((DMA_CH)uCh, uLLIBaseAddr+0x20, uTxBuffAddr, 0, uRxBuffAddr, 0, (DATA_SIZE)uTsize, uDataCnts, DEMAND, MEM, MEM, (BURST_MODE)uBurst, &oDmac0);
			DMACH_Start(&oDmac0);
			break;
		case 1 :
		        // Channel, LLI_Address, SrcAddr, Src Type, DstAddr, Dst Type, Transfer Width, Transfer Size, OpMode(DEMAND), Src Req, Dst Req, Burst
			DMACH_Setup((DMA_CH)uCh, uLLIBaseAddr+0x20, uTxBuffAddr, 0, uRxBuffAddr, 0, (DATA_SIZE)uTsize, uDataCnts, DEMAND, MEM, MEM, (BURST_MODE)uBurst, &oDmac1);
			DMACH_Start(&oDmac1);
			break;
		case 2 :
		        // Channel, LLI_Address, SrcAddr, Src Type, DstAddr, Dst Type, Transfer Width, Transfer Size, OpMode(DEMAND), Src Req, Dst Req, Burst
			DMACH_Setup((DMA_CH)uCh, uLLIBaseAddr+0x20, uTxBuffAddr, 0, uRxBuffAddr, 0, (DATA_SIZE)uTsize, uDataCnts, DEMAND, MEM, MEM, (BURST_MODE)uBurst, &oDmac2);
			DMACH_Start(&oDmac2);
			break;
		case 3 :
		        // Channel, LLI_Address, SrcAddr, Src Type, DstAddr, Dst Type, Transfer Width, Transfer Size, OpMode(DEMAND), Src Req, Dst Req, Burst
			DMACH_Setup((DMA_CH)uCh, uLLIBaseAddr+0x20, uTxBuffAddr, 0, uRxBuffAddr, 0, (DATA_SIZE)uTsize, uDataCnts, DEMAND, MEM, MEM, (BURST_MODE)uBurst, &oDmac3);
			DMACH_Start(&oDmac3);
			break;
		default :
			Assert(0);
	}

	while(g_DmaDone==0);								// Int.
	//while (!DMAC_IsTransferDone(&oDmac0));			// Polling

	if (CompareDMA(uTxBuffAddr, uRxBuffAddr, (DATA_SIZE)uTsize, uDataCnts))
		UART_Printf(" >> Test Tx&Rx -> Ok << \n");
	else
	{
		UART_Printf(" >>*** Tx-data & Rx-data mismatch ***<< \n");
	}


	switch(csel)
	{
		case 0: 
				INTC_Disable(NUM_DMA0);
			    	DMAC_Close(DMA0, DMA_ALL, &oDmac0);
			     	break;
               
		case 1: 
				INTC_Disable(NUM_DMA1);
				DMAC_Close(DMA1, DMA_ALL, &oDmac1);
			     	break;
		case 2: 
				INTC_Disable(NUM_SDMA0);
				DMAC_Close(SDMA0, DMA_ALL, &oDmac2);
			     	break;
		case 3: 
				INTC_Disable(NUM_SDMA1);
				DMAC_Close(SDMA1, DMA_ALL, &oDmac3);
			     	break;
			     	
		default : Assert(0);
	}
}

//////////
// Function Name : DMAT_SoftReq
// Function Description : DMA Software Request Test
// Input : 	None
// Output :	None 
// Version : v0.1
static void DMAT_SoftReq(void)
{
	u32 i, csel;

	g_DmaDone=0;
	
	UART_Printf("\nSelect DMA Controller : 0:DMA0, 1:DMA1, 2:SDMA0, 3:SDMA1	: ");
	csel=UART_GetIntNum();
	UART_Printf("\n");

	switch(csel)
	{
		case 0: 
				UART_Printf("Selected DMAC 0 ..... \n");
				SYSC_SelectDMA(eSEL_I2S0_TX, 1);
				
			     	DMAC_InitCh(DMA0, DMA_ALL, &oDmac0);
				INTC_SetVectAddr(NUM_DMA0,  Dma0Done);
				INTC_Enable(NUM_DMA0);
			     	break;
               
		case 1: 
				UART_Printf("Selected DMAC 1 ..... \n");
				SYSC_SelectDMA(eSEL_PWM, 1);
				
			     	DMAC_InitCh(DMA1, DMA_ALL, &oDmac1);
				INTC_SetVectAddr(NUM_DMA1,  Dma1Done);
				INTC_Enable(NUM_DMA1);

				DMACH_ClearErrIntPending(&oDmac1);
			  	DMACH_ClearIntPending(&oDmac1);
				
			     	break;
		case 2: 
				UART_Printf("Selected SDMAC 0 ..... \n");
				SYSC_SelectDMA(eSEL_I2S0_TX, 0);
				
			     	DMAC_InitCh(SDMA0, DMA_ALL, &oDmac2);
				INTC_SetVectAddr(NUM_SDMA0, Sdma0Done );
				INTC_Enable(NUM_SDMA0);
			     	break;
		case 3: 
				UART_Printf("Selected SDMAC 1 ..... \n");
				SYSC_SelectDMA(eSEL_PWM, 0);
				
			     	DMAC_InitCh(SDMA1, DMA_ALL, &oDmac3);
				INTC_SetVectAddr(NUM_SDMA1, Sdma1Done);
				INTC_Enable(NUM_SDMA1);
			     	break;
			     	
		default : Assert(0);
	}
       
#if 0
       UART_Printf("\nSelect Transfer Size [1~0x200_0000] :   ");
	uDataCnts=UART_GetIntNum();
	UART_Printf("\n");
#else
	uDataCnts=100;
#endif

	// 0. Clear the rx/tx buf.
	for (i = 0; i<(uDataCnts*WORD+16); i++)
	{
		*(u8 *)(uRxBuffAddr+i) = 0;
		*(u8 *)(uTxBuffAddr+i) = 0;
	}
	
	// 1. Set up the tx buf.
	for (i = 0; i<uDataCnts*WORD; i++)
		*(u8 *)(uTxBuffAddr+i) = (u8)(i+2)%0xff;

	uRxBuffAddr = 0x7f006010;
//	UART_Printf("uRxBuffAddr:0x%x\n", uRxBuffAddr);

	switch(csel)
	{
		case 0: 	 // Channel Set-up
				DMACH_Setup((DMA_CH)DMA_A, 0x0, uTxBuffAddr, 0, 0x7F002010, 1, (DATA_SIZE)WORD, 1, DEMAND, MEM, DMA0_I2S0_TX, (BURST_MODE)SINGLE, &oDmac0);	
			        // Enable DMA
				DMACH_Start(&oDmac0);

				UART_Printf("DMA doesn't send request not yet. Now sending...\n");
				Getc();
				DMACH_SoftBurstReq(&oDmac0, DMA0_I2S0_TX);	// SoftWare Burst Request to DMA0_I2S0_TX source
				UART_Printf("DMA request was sent.\n");

			     	break;               
		case 1: 
				DMACH_Setup((DMA_CH)DMA_A, 0x0, uTxBuffAddr, 0, 0x7f006010, 1, (DATA_SIZE)WORD, 1, DEMAND, MEM, DMA1_PWM, (BURST_MODE)SINGLE, &oDmac1);
			        // Enable DMA
				DMACH_Start(&oDmac1);

				UART_Printf("DMA doesn't send request not yet. Now sending...\n");
				Getc();
				DMACH_SoftBurstReq(&oDmac1, DMA1_PWM);	// SoftWare Burst Request to DMA1_PWM source
				UART_Printf("DMA request was sent.\n");

			     	break;
		case 2: 
				DMACH_Setup((DMA_CH)DMA_A, 0x0, uTxBuffAddr, 0, 0x7F002010, 1, (DATA_SIZE)WORD, 1, DEMAND, MEM, DMA0_I2S0_TX, (BURST_MODE)SINGLE, &oDmac2);	
			        // Enable DMA
				DMACH_Start(&oDmac2);

				UART_Printf("DMA doesn't send request not yet. Now sending...\n");
				Getc();
				DMACH_SoftBurstReq(&oDmac2, DMA0_I2S0_TX);	// SoftWare Burst Request to DMA0_I2S0_TX source
				UART_Printf("DMA request was sent.\n");

			     	break;
		case 3: 
				DMACH_Setup((DMA_CH)DMA_A, 0x0, uTxBuffAddr, 0, 0x7f006010, 1, (DATA_SIZE)WORD, 1, DEMAND, MEM, DMA1_PWM, (BURST_MODE)SINGLE, &oDmac3);
			        // Enable DMA
				DMACH_Start(&oDmac3);

				UART_Printf("DMA doesn't send request not yet. Now sending...\n");
				Getc();
				DMACH_SoftBurstReq(&oDmac3, DMA1_PWM);	// SoftWare Burst Request to DMA1_PWM source
				UART_Printf("DMA request was sent.\n");
			     	break;
			     	
		default : Assert(0);
	}	
        
	while(g_DmaDone==0);								// Int.
#if 0
	if (CompareDMA(uTxBuffAddr, uRxBuffAddr, (DATA_SIZE)WORD, uDataCnts))
		UART_Printf(" >> Test Tx&Rx -> Ok << \n");
	else
	{
		UART_Printf(" >>*** Tx-data & Rx-data mismatch ***<< \n");
	}
#else
	UART_Printf("SoftDMA request was received.\n");
#endif

	switch(csel)
	{
		case 0: 
				INTC_Disable(NUM_DMA0);
			    	DMAC_Close(DMA0, DMA_ALL, &oDmac0);
			     	break;
               
		case 1: 
				INTC_Disable(NUM_DMA1);
				DMAC_Close(DMA1, DMA_ALL, &oDmac1);
			     	break;
		case 2: 
				INTC_Disable(NUM_SDMA0);
				DMAC_Close(SDMA0, DMA_ALL, &oDmac2);
			     	break;
		case 3: 
				INTC_Disable(NUM_SDMA1);
				DMAC_Close(SDMA1, DMA_ALL, &oDmac3);
			     	break;
			     	
		default : Assert(0);
	}	
}

//////////
// Function Name : DMAT_WorstCase
// Function Description : DMA Worst Case Test
// Input : 	None
// Output :	None 
// Version : v0.1
static void DMAT_WorstCase(void)
{
	u32	uLoopCnt = 0;
	volatile u32  uMemSum =0, uMemSum0=0, uMemSum1=0, uMemSum2=0, uMemSum3=0;
	u32 uDstAddrOffset = 0;

	INTC_Disable(NUM_DMA0);
	INTC_Disable(NUM_DMA1);
	INTC_Disable(NUM_SDMA0);
	INTC_Disable(NUM_SDMA1);
	
	DMAC_InitCh(DMA0, DMA_ALL, &oDmac0);
	INTC_SetVectAddr(NUM_DMA0,  Dma0DoneW);

	DMAC_InitCh(DMA1, DMA_ALL, &oDmac1);
	INTC_SetVectAddr(NUM_DMA1,  Dma1DoneW);

	DMAC_InitCh(SDMA0, DMA_ALL, &oDmac2);
	INTC_SetVectAddr(NUM_SDMA0, Sdma0DoneW );

	DMAC_InitCh(SDMA1, DMA_ALL, &oDmac3);
	INTC_SetVectAddr(NUM_SDMA1, Sdma1DoneW );

	g_DmaDone0 = 0;
	g_DmaDone1 = 0;
	g_DmaDone2 = 0;
	g_DmaDone3 = 0;

    // Memory Filling
   	for(uLoopCnt=uTxBuffAddr;uLoopCnt<uTxBuffAddr+g_TSize;uLoopCnt+=4)
	{
		*((unsigned int *)uLoopCnt)=uLoopCnt;
	      uMemSum += uLoopCnt;
	}	
	//-------- DMAC0 -----------		
        // Channel, LLI_Address, SrcAddr, Src Type, DstAddr, Dst Type, Transfer Width, Transfer Size, OpMode(DEMAND), Src Req, Dst Req, Burst
	DMACH_Setup((DMA_CH)DMA_A, 0x0, uTxBuffAddr, 0, uRxBuffAddr0, 0, (DATA_SIZE)WORD, DMAC0TxSize, DEMAND, MEM, MEM, (BURST_MODE)BURST8, &oDmac0);        
	DMACH_Start(&oDmac0);	// Enable DMA
	INTC_Enable(NUM_DMA0);

	DelayfrTimer(milli, 1000);
	
	//-------- DMAC1 -----------		
        // Channel, LLI_Address, SrcAddr, Src Type, DstAddr, Dst Type, Transfer Width, Transfer Size, OpMode(DEMAND), Src Req, Dst Req, Burst
	DMACH_Setup((DMA_CH)DMA_A, 0x0, uTxBuffAddr, 0, uRxBuffAddr1, 0, (DATA_SIZE)WORD, DMAC1TxSize, DEMAND, MEM, MEM, (BURST_MODE)BURST8, &oDmac1);        
	DMACH_Start(&oDmac1);	// Enable DMA
	INTC_Enable(NUM_DMA1);	
	
	//-------- DMAC2 -----------		
        // Channel, LLI_Address, SrcAddr, Src Type, DstAddr, Dst Type, Transfer Width, Transfer Size, OpMode(DEMAND), Src Req, Dst Req, Burst
	DMACH_Setup((DMA_CH)DMA_A, 0x0, uTxBuffAddr, 0, uRxBuffAddr2, 0, (DATA_SIZE)WORD, SDMAC0TxSize, DEMAND, MEM, MEM, (BURST_MODE)BURST8, &oDmac2);        
	DMACH_Start(&oDmac2);	// Enable DMA
	INTC_Enable(NUM_SDMA0);
	
	//-------- DMAC3 -----------		
        // Channel, LLI_Address, SrcAddr, Src Type, DstAddr, Dst Type, Transfer Width, Transfer Size, OpMode(DEMAND), Src Req, Dst Req, Burst
	DMACH_Setup((DMA_CH)DMA_A, 0x0, uTxBuffAddr, 0, uRxBuffAddr3, 0, (DATA_SIZE)WORD, SDMAC1TxSize, DEMAND, MEM, MEM, (BURST_MODE)BURST8, &oDmac3);
	DMACH_Start(&oDmac3);	// Enable DMA
	INTC_Enable(NUM_SDMA1);

	UART_Printf("Waiting Transfer Count Interrupt\n");
//	while(g_DmaDone0<(g_TSize/DMAC0TxSize)||g_DmaDone1<(g_TSize/DMAC1TxSize)|g_DmaDone2<(g_TSize/SDMAC0TxSize)|g_DmaDone3<(g_TSize/SDMAC1TxSize));
	while(g_DmaDone1<(g_TSize/DMAC1TxSize)|g_DmaDone2<(g_TSize/SDMAC0TxSize)|g_DmaDone3<(g_TSize/SDMAC1TxSize));
	
	UART_Printf("DMAC0 Test ==========================\n");
	uDstAddrOffset = 0;
	for(uLoopCnt=uRxBuffAddr0;uLoopCnt<(uRxBuffAddr0+g_TSize);uLoopCnt+=4) //Compare source data with destination data
	{
		if(*((unsigned int *)uLoopCnt) != (uTxBuffAddr+uDstAddrOffset))
		{
			UART_Printf("Error!!! Src value=%x, Addr %x=%x\n",uTxBuffAddr+uDstAddrOffset,uLoopCnt,*((unsigned int *)uLoopCnt));
		}
		uMemSum0+=*((unsigned int *)uLoopCnt);

		uDstAddrOffset += 4;
	}
	
	UART_Printf("DMAC1 Test ==========================\n");
	uDstAddrOffset = 0;
	for(uLoopCnt=uRxBuffAddr1;uLoopCnt<(uRxBuffAddr1+g_TSize);uLoopCnt+=4) //Compare source data with destination data
	{
		if(*((unsigned int *)uLoopCnt) != (uTxBuffAddr+uDstAddrOffset))
		{
			UART_Printf("Error!!! Src value=%x, Addr %x=%x\n",uTxBuffAddr+uDstAddrOffset,uLoopCnt,*((unsigned int *)uLoopCnt));
		}
		uMemSum1 += *((unsigned int *)uLoopCnt);

		uDstAddrOffset += 4;
	}

	UART_Printf("SDMAC0 Test ==========================\n");
	uDstAddrOffset = 0;
	for(uLoopCnt=uRxBuffAddr2;uLoopCnt<(uRxBuffAddr2+g_TSize);uLoopCnt+=4) //Compare source data with destination data
	{
		if(*((unsigned int *)uLoopCnt) != (uTxBuffAddr+uDstAddrOffset))
		{
			UART_Printf("Error!!! Src value=%x, Addr %x=%x\n",uTxBuffAddr+uDstAddrOffset,uLoopCnt,*((unsigned int *)uLoopCnt));
		}
		uMemSum2+=*((unsigned int *)uLoopCnt);

		uDstAddrOffset += 4;
	}

	UART_Printf("SDMAC1 Test ==========================\n");
	uDstAddrOffset = 0;
	for(uLoopCnt=uRxBuffAddr3;uLoopCnt<(uRxBuffAddr3+g_TSize);uLoopCnt+=4) //Compare source data with destination data
	{
		if(*((unsigned int *)uLoopCnt) != (uTxBuffAddr+uDstAddrOffset))
		{
			UART_Printf("Error!!! Src value=%x, Addr %x=%x\n",uTxBuffAddr+uDstAddrOffset,uLoopCnt,*((unsigned int *)uLoopCnt));
		}
		uMemSum3+=*((unsigned int *)uLoopCnt);

		uDstAddrOffset += 4;
	}

	UART_Printf("memSum=%x,memSum0=%x\n",uMemSum,uMemSum0);
	UART_Printf("memSum=%x,memSum1=%x\n",uMemSum,uMemSum1);
	UART_Printf("memSum=%x,memSum2=%x\n",uMemSum,uMemSum2);
	UART_Printf("memSum=%x,memSum3=%x\n",uMemSum,uMemSum3);
	
	if(uMemSum==uMemSum0)
		UART_Printf("DMAC0 test result......................O.K.\n");
	else
		UART_Printf("DMAC0 test result......................ERROR.\n");
	
	if(uMemSum==uMemSum1)
		UART_Printf("DMAC1 test result......................O.K.\n");
	else
		UART_Printf("DMAC1 test result......................ERROR.\n");
	

	if(uMemSum==uMemSum2)
		UART_Printf("SDMAC0 test result......................O.K.\n");
	else
		UART_Printf("SDMAC0 test result......................ERROR.\n");
	
    
	if(uMemSum==uMemSum3)
		UART_Printf("SDMAC1 test result......................O.K.\n");
	else
		UART_Printf("SDMAC1 test result......................ERROR.\n");
	
}

//////////
// Function Name : DMAT_External
// Function Description : External DMA Test
// Input : 	None
// Output :	None 
// Version : v0.1
//---------------------------------------------
// This function implements External DMA Request/Ack test using GPIO toggle.
// When S3C6410 gets external DMA request through XexdREQ pin, she puts DMA ack signal to XexdACK pin.
// Whenever High-to-Low transition of XexdREQ occures, the data transfer defined by DMACH_Setup function would be executed.
// If uDataCnts becomes 0, Dma1Done interrupt will be generated.
//
// GPF[13] means external input which activates DMA Request.
// This pin is located in the left side of SMDK6410 Base Board.
// So, this pin should be connected to XexdREQ pin on SMDK6410 Base Board in order to test Ext. DMA Request.
// Refer to SMDK6400 Board Schematic.
static void DMAT_External(void)
{
	u32 i;
	u8 	ucExitKey=0;
	
	g_DmaDone=0;
	GPIO_SetFunctionEach(eGPIO_F,eGPIO_13,1);
	GPIO_SetDataEach(eGPIO_F, eGPIO_13 , 1); 		//GPF[13] -> High

//	GPIO_SetFunctionEach(eGPIO_B, eGPIO_0, 3);	// set Input/Output/XuRXD[2]/XexdREQ/XirRXD/ADDR_CF[0]/GPB[0] as XexdREQ
//	GPIO_SetFunctionEach(eGPIO_B, eGPIO_1, 3);	// set Input/Output/XuTXD[2]/XexdACK/XirTXD/ADDR_CF[1]/GPB[1] as XexdACK
	GPIO_SetFunctionEach(eGPIO_B, eGPIO_2, 4);		// set Input/Output/XuRXD[3]/IrDARXD/XexdREQ/ADDR_CF[2]/GPB[2] as XexdREQ
	GPIO_SetFunctionEach(eGPIO_B, eGPIO_3, 4);		// set Input/Output/XuTXD[3]/IrDATXD/XexdACK/ADDR_CF[3]/GPB[3] as XexdACK
	
	SYSC_SelectDMA(eSEL_EXTERNAL, 1);
	
	DMAC_InitCh(DMA1, DMA_ALL, &oDmac1);
	INTC_SetVectAddr(NUM_DMA1,  Dma1Done);
	INTC_Enable(NUM_DMA1);

	DMACH_ClearErrIntPending(&oDmac1);
  	DMACH_ClearIntPending(&oDmac1);
	
#if 0
       UART_Printf("\nSelect Transfer Size [1~0x200_0000] :   ");
	uDataCnts=UART_GetIntNum();
	UART_Printf("\n");
#else
	uDataCnts=12;
#endif

	// 0. Clear the rx/tx buf.
	for (i = 0; i<(uDataCnts*WORD+16); i++)
	{
		*(u8 *)(uRxBuffAddr+i) = 0;
		*(u8 *)(uTxBuffAddr+i) = 0;
	}
	
	// 1. Set up the tx buf.
	for (i = 0; i<uDataCnts*WORD; i++)
		*(u8 *)(uTxBuffAddr+i) = (u8)(i+2)%0xff;

	DMACH_Setup((DMA_CH)DMA_A, 0x0, uTxBuffAddr, 0, uRxBuffAddr, 0, (DATA_SIZE)WORD, uDataCnts, DEMAND, DMA1_EXTERNAL, MEM, (BURST_MODE)BURST4, &oDmac1);
        // Enable DMA
	DMACH_Start(&oDmac1);


	UART_Printf("Cuation! To execute this function, GPF[13] should be connected to XuRXD[2]/XexdREQ pin.\n");
	UART_Printf("Please refer to SMDK6400 Base Board schematic in order to connect these pins.\n");
	UART_Getc();
	UART_Printf("\nHigh->Low transition of XexdReq signal will trigger DMA data transfer.\n");
	UART_Printf("All data transfer will be done if this kind of transfer occures 3 times.\n");
	UART_Printf("Waiting the response of External DMA Request.....\n");
	UART_Printf("'e':Low->High\n");
	UART_Printf("'r':High->Low\n");
	UART_Printf(">Current XexdReq State : High\n");

	while(g_DmaDone==0)								// Int.
	{
		ucExitKey = UART_Getc();
		
	    	if(ucExitKey == 'r')
		{
			UART_Printf(">Ext. DMA Request Low\n");
			GPIO_SetDataEach(eGPIO_F, eGPIO_13 , 0); 		//GPF[13] -> Low
	    	}
	    	else if(ucExitKey == 'e')
		{
			UART_Printf(">Ext. DMA Request High\n");
			GPIO_SetDataEach(eGPIO_F, eGPIO_13 , 1); 		//GPF[13] -> High
	    	}
		else if(ucExitKey == 'x')
		{
			break;
		}
	}
	UART_Printf("OK!! \n");

	if (CompareDMA(uTxBuffAddr, uRxBuffAddr, (DATA_SIZE)WORD, uDataCnts))
		UART_Printf(" >> Test Tx&Rx -> Ok << \n");
	else
	{
		UART_Printf(" >>*** Tx-data & Rx-data mismatch ***<< \n");
	}

	INTC_Disable(NUM_DMA1);
	DMAC_Close(DMA1, DMA_ALL, &oDmac1);
}

void TestGPIO(void)
{
	s32 iGPIOSel = 0;
	s32 iGPIOPortSel = 0;
	GPIO_eId eGPIOId = eGPIO_D;
	
	UART_Printf("\nSelect GPIO : 0:GPD, 1:GPE");
	iGPIOSel=UART_GetIntNum();
	UART_Printf("\n");

	UART_Printf("\nSelect GPIO Port: 0:Port0, 1:Port1, 2:Port2, 3:Port3, 4:Port4");
	iGPIOPortSel=UART_GetIntNum();
	UART_Printf("\n");

	eGPIOId = (GPIO_eId)((u32)eGPIO_D + iGPIOSel*2);
	
	GPIO_SetFunctionEach(eGPIOId,(GPIO_eBitPos)iGPIOPortSel,1);	// GPD[0] -> output
	GPIO_SetDataEach(eGPIOId, (GPIO_eBitPos)iGPIOPortSel, 0); 		//GPD[0] -> Low
	GPIO_SetDataEach(eGPIOId, (GPIO_eBitPos)iGPIOPortSel , 1); 		//GPD[0] -> High
	GPIO_SetDataEach(eGPIOId, (GPIO_eBitPos)iGPIOPortSel , 0); 		//GPD[0] -> Low
}

///////////////////////////////////////////////////////////////////////////////////
////////////////////                 DMA Full Test                     /////////////////////////// 
///////////////////////////////////////////////////////////////////////////////////

#define MAX_DMA_CNT	0x7D000*5	//	(20MB)
#define MAX_AUTODMA_CNT	0x10000		//	(64KB)
//#define MAX_DMA_CNT	0x200

//////////
// Function Name : DMAT_FullFunction
// Function Description : DMA full function test
// Input : 	None
// Version : v0.1

void DMAT_FullFunction(void)
{	
	u32 i, j, k,uNumClock, uNumOndMode;
	u32	*puTxDataBuffer, *puRxDataBuffer;

	u32			uIntSrcNo	= NUM_DMA0;
	DMA_UNIT	eDmaUnit 	= DMA0;
	DMA_CH		eDmaCh		= DMA_A;
	BURST_MODE	eBurstMode	= BURST4;
	u32			uDataCnts	= 0;	
	
						   //{Sync Mode, APLL, MPLL, HCLKx2 divide, HCLK divide, Flash CLK divide, SMCLK divide}
	u32 aCaseOfClock[][7] = { //Sync Mode
#if 1
							{eSYNC_MODE, eAPLL400M, eAPLL200M, 2, 2, 2, 2}, 
							{eSYNC_MODE, eAPLL400M, eAPLL200M, 2, 2, 3, 2},
							{eSYNC_MODE, eAPLL400M, eAPLL200M, 2, 2, 4, 2},
						
							{eSYNC_MODE, eAPLL400M, eAPLL200M, 4, 2, 2, 2},
							{eSYNC_MODE, eAPLL400M, eAPLL200M, 4, 2, 3, 2},
							{eSYNC_MODE, eAPLL400M, eAPLL200M, 4, 2, 4, 2},
						
							{eSYNC_MODE, eAPLL532M, eAPLL266M, 2, 2, 2, 2},
							{eSYNC_MODE, eAPLL532M, eAPLL266M, 2, 2, 3, 2},
							{eSYNC_MODE, eAPLL532M, eAPLL266M, 2, 2, 4, 2},
							
							{eSYNC_MODE, eAPLL532M, eAPLL266M, 4, 2, 2, 2},
							{eSYNC_MODE, eAPLL532M, eAPLL266M, 4, 2, 3, 2},
							{eSYNC_MODE, eAPLL532M, eAPLL266M, 4, 2, 4, 2},
#endif
						   	//Async Mode
							{eASYNC_MODE, eAPLL400M, eAPLL400M, 2, 2, 2, 2},
							{eASYNC_MODE, eAPLL400M, eAPLL400M, 2, 2, 3, 2},
							{eASYNC_MODE, eAPLL400M, eAPLL400M, 2, 2, 4, 2},
							
							{eASYNC_MODE, eAPLL400M, eAPLL400M, 4, 2, 2, 2},
							{eASYNC_MODE, eAPLL400M, eAPLL400M, 4, 2, 3, 2},
							{eASYNC_MODE, eAPLL400M, eAPLL400M, 4, 2, 4, 2},
							
							{eASYNC_MODE, eAPLL532M, eAPLL400M, 2, 2, 2, 2},
							{eASYNC_MODE, eAPLL532M, eAPLL400M, 2, 2, 3, 2},
							{eASYNC_MODE, eAPLL532M, eAPLL400M, 2, 2, 4, 2},
							
							{eASYNC_MODE, eAPLL532M, eAPLL400M, 4, 2, 2, 2},
							{eASYNC_MODE, eAPLL532M, eAPLL400M, 4, 2, 3, 2},
							{eASYNC_MODE, eAPLL532M, eAPLL400M, 4, 2, 4, 2},
							
							{eASYNC_MODE, eAPLL532M, eAPLL532M, 2, 2, 2, 2},
							{eASYNC_MODE, eAPLL532M, eAPLL532M, 2, 2, 3, 2},
							{eASYNC_MODE, eAPLL532M, eAPLL532M, 2, 2, 4, 2},

							{eASYNC_MODE, eAPLL532M, eAPLL532M, 4, 2, 2, 2},
							{eASYNC_MODE, eAPLL532M, eAPLL532M, 4, 2, 3, 2},
							{eASYNC_MODE, eAPLL532M, eAPLL532M, 4, 2, 4, 2} };
#if 0							
							//	{Controller, 	Interrupt Src,	Channel, 		Burst, 		Data Cnt}
	u32 aCaseOfDMAMode[][5] = {	{DMA0, 		NUM_DMA0, 	DMA_A, 		BURST4,		MAX_DMA_CNT},		// 	0x200(512) * WORD = 2K
								{DMA0, 		NUM_DMA0, 	DMA_B,		BURST8,		MAX_DMA_CNT},		//	0x200(512) * WORD = 2K
								{DMA0, 		NUM_DMA0, 	DMA_C,		BURST16,	MAX_DMA_CNT},		//	0x200(5120) * WORD = 20K
								{DMA0, 		NUM_DMA0, 	DMA_D,		BURST32,	MAX_DMA_CNT},		//	0x200(5120) * WORD = 20K
								{DMA0, 		NUM_DMA0, 	DMA_E,		BURST64,	MAX_DMA_CNT},		//	0xC800(51200) * WORD = 200K
								{DMA0, 		NUM_DMA0, 	DMA_F,		BURST128,	MAX_DMA_CNT},		//	0xC800(51200) * WORD = 200K
								{DMA0, 		NUM_DMA0, 	DMA_G,		BURST256,	MAX_DMA_CNT},	//	0x7D000(512000) * WORD = 2M
								{DMA0, 		NUM_DMA0, 	DMA_H,		BURST256,	MAX_DMA_CNT},	//	0x7D000(512000) * WORD = 2M

								{DMA1, 		NUM_DMA1, 	DMA_A, 		BURST4,		MAX_DMA_CNT},		// 	0x200(512) * WORD = 2K
								{DMA1, 		NUM_DMA1, 	DMA_B,		BURST8,		MAX_DMA_CNT},		//	0x200(512) * WORD = 2K
								{DMA1, 		NUM_DMA1, 	DMA_C,		BURST16,	MAX_DMA_CNT},		//	0x200(5120) * WORD = 20K
								{DMA1, 		NUM_DMA1, 	DMA_D,		BURST32,	MAX_DMA_CNT},		//	0x200(5120) * WORD = 20K
								{DMA1, 		NUM_DMA1, 	DMA_E,		BURST64,	MAX_DMA_CNT},		//	0xC800(51200) * WORD = 200K
								{DMA1, 		NUM_DMA1, 	DMA_F,		BURST128,	MAX_DMA_CNT},		//	0xC800(51200) * WORD = 200K
								{DMA1, 		NUM_DMA1, 	DMA_G,		BURST256,	MAX_DMA_CNT},	//	0x7D000(512000) * WORD = 2M
								{DMA1, 		NUM_DMA1, 	DMA_H,		BURST256,	MAX_DMA_CNT},	//	0x7D000(512000) * WORD = 2M
								 };

							//	{Controller, 	Interrupt Src,	Channel, 		Burst, 		Data Cnt}
	u32 aCaseOfDMAMode[][5] = {	{SDMA0, 		NUM_SDMA0, 	DMA_A, 		BURST4,		MAX_DMA_CNT},		// 	0x200(512) * WORD = 2K
								{SDMA0, 		NUM_SDMA0, 	DMA_B,		BURST8,		MAX_DMA_CNT},		//	0x200(512) * WORD = 2K
								{SDMA0, 		NUM_SDMA0, 	DMA_C,		BURST16,	MAX_DMA_CNT},		//	0x200(5120) * WORD = 20K
								{SDMA0, 		NUM_SDMA0, 	DMA_D,		BURST32,	MAX_DMA_CNT},		//	0x200(5120) * WORD = 20K
								{SDMA0, 		NUM_SDMA0, 	DMA_E,		BURST64,	MAX_DMA_CNT},		//	0xC800(51200) * WORD = 200K
								{SDMA0, 		NUM_SDMA0, 	DMA_F,		BURST128,	MAX_DMA_CNT},		//	0xC800(51200) * WORD = 200K
								{SDMA0, 		NUM_SDMA0, 	DMA_G,		BURST256,	MAX_DMA_CNT},	//	0x7D000(512000) * WORD = 2M
								{SDMA0, 		NUM_SDMA0, 	DMA_H,		BURST256,	MAX_DMA_CNT},	//	0x7D000(512000) * WORD = 2M

								{SDMA1, 		NUM_SDMA1, 	DMA_A, 		BURST4,		MAX_DMA_CNT},		// 	0x200(512) * WORD = 2K
								{SDMA1, 		NUM_SDMA1, 	DMA_B,		BURST8,		MAX_DMA_CNT},		//	0x200(512) * WORD = 2K
								{SDMA1, 		NUM_SDMA1, 	DMA_C,		BURST16,	MAX_DMA_CNT},		//	0x200(5120) * WORD = 20K
								{SDMA1, 		NUM_SDMA1, 	DMA_D,		BURST32,	MAX_DMA_CNT},		//	0x200(5120) * WORD = 20K
								{SDMA1, 		NUM_SDMA1, 	DMA_E,		BURST64,	MAX_DMA_CNT},		//	0xC800(51200) * WORD = 200K
								{SDMA1, 		NUM_SDMA1, 	DMA_F,		BURST128,	MAX_DMA_CNT},		//	0xC800(51200) * WORD = 200K
								{SDMA1, 		NUM_SDMA1, 	DMA_G,		BURST256,	MAX_DMA_CNT},	//	0x7D000(512000) * WORD = 2M
								{SDMA1, 		NUM_SDMA1, 	DMA_H,		BURST256,	MAX_DMA_CNT},	//	0x7D000(512000) * WORD = 2M
								 };


#else
							//	{Controller, 	Interrupt Src,	Channel, 		Burst, 		Data Cnt}
	u32 aCaseOfDMAMode[][5] = {	{DMA0, 		NUM_DMA0, 	DMA_A, 		BURST4,		MAX_DMA_CNT},		// 	0x200(512) * WORD = 2K
								{DMA0, 		NUM_DMA0, 	DMA_B,		BURST8,		MAX_DMA_CNT},		//	0x200(512) * WORD = 2K
								{DMA0, 		NUM_DMA0, 	DMA_C,		BURST16,	MAX_DMA_CNT},		//	0x200(5120) * WORD = 20K
								{DMA0, 		NUM_DMA0, 	DMA_D,		BURST32,	MAX_DMA_CNT},		//	0x200(5120) * WORD = 20K
								{DMA0, 		NUM_DMA0, 	DMA_E,		BURST64,	MAX_DMA_CNT},		//	0xC800(51200) * WORD = 200K
								{DMA0, 		NUM_DMA0, 	DMA_F,		BURST128,	MAX_DMA_CNT},		//	0xC800(51200) * WORD = 200K
								{DMA0, 		NUM_DMA0, 	DMA_G,		BURST256,	MAX_DMA_CNT},	//	0x7D000(512000) * WORD = 2M
								{DMA0, 		NUM_DMA0, 	DMA_H,		BURST256,	MAX_DMA_CNT},	//	0x7D000(512000) * WORD = 2M

								{DMA1, 		NUM_DMA1, 	DMA_A, 		BURST4,		MAX_DMA_CNT},		// 	0x200(512) * WORD = 2K
								{DMA1, 		NUM_DMA1, 	DMA_B,		BURST8,		MAX_DMA_CNT},		//	0x200(512) * WORD = 2K
								{DMA1, 		NUM_DMA1, 	DMA_C,		BURST16,	MAX_DMA_CNT},		//	0x200(5120) * WORD = 20K
								{DMA1, 		NUM_DMA1, 	DMA_D,		BURST32,	MAX_DMA_CNT},		//	0x200(5120) * WORD = 20K
								{DMA1, 		NUM_DMA1, 	DMA_E,		BURST64,	MAX_DMA_CNT},		//	0xC800(51200) * WORD = 200K
								{DMA1, 		NUM_DMA1, 	DMA_F,		BURST128,	MAX_DMA_CNT},		//	0xC800(51200) * WORD = 200K
								{DMA1, 		NUM_DMA1, 	DMA_G,		BURST256,	MAX_DMA_CNT},	//	0x7D000(512000) * WORD = 2M
								{DMA1, 		NUM_DMA1, 	DMA_H,		BURST256,	MAX_DMA_CNT},	//	0x7D000(512000) * WORD = 2M
								
								{SDMA0, 		NUM_SDMA0, 	DMA_A, 		BURST4,		MAX_DMA_CNT},		// 	0x200(512) * WORD = 2K
								{SDMA0, 		NUM_SDMA0, 	DMA_B,		BURST8,		MAX_DMA_CNT},		//	0x200(512) * WORD = 2K
								{SDMA0, 		NUM_SDMA0, 	DMA_C,		BURST16,	MAX_DMA_CNT},		//	0x200(5120) * WORD = 20K
								{SDMA0, 		NUM_SDMA0, 	DMA_D,		BURST32,	MAX_DMA_CNT},		//	0x200(5120) * WORD = 20K
								{SDMA0, 		NUM_SDMA0, 	DMA_E,		BURST64,	MAX_DMA_CNT},		//	0xC800(51200) * WORD = 200K
								{SDMA0, 		NUM_SDMA0, 	DMA_F,		BURST128,	MAX_DMA_CNT},		//	0xC800(51200) * WORD = 200K
								{SDMA0, 		NUM_SDMA0, 	DMA_G,		BURST256,	MAX_DMA_CNT},	//	0x7D000(512000) * WORD = 2M
								{SDMA0, 		NUM_SDMA0, 	DMA_H,		BURST256,	MAX_DMA_CNT},	//	0x7D000(512000) * WORD = 2M

								{SDMA1, 		NUM_SDMA1, 	DMA_A, 		BURST4,		MAX_DMA_CNT},		// 	0x200(512) * WORD = 2K
								{SDMA1, 		NUM_SDMA1, 	DMA_B,		BURST8,		MAX_DMA_CNT},		//	0x200(512) * WORD = 2K
								{SDMA1, 		NUM_SDMA1, 	DMA_C,		BURST16,	MAX_DMA_CNT},		//	0x200(5120) * WORD = 20K
								{SDMA1, 		NUM_SDMA1, 	DMA_D,		BURST32,	MAX_DMA_CNT},		//	0x200(5120) * WORD = 20K
								{SDMA1, 		NUM_SDMA1, 	DMA_E,		BURST64,	MAX_DMA_CNT},		//	0xC800(51200) * WORD = 200K
								{SDMA1, 		NUM_SDMA1, 	DMA_F,		BURST128,	MAX_DMA_CNT},		//	0xC800(51200) * WORD = 200K
								{SDMA1, 		NUM_SDMA1, 	DMA_G,		BURST256,	MAX_DMA_CNT},	//	0x7D000(512000) * WORD = 2M
								{SDMA1, 		NUM_SDMA1, 	DMA_H,		BURST256,	MAX_DMA_CNT},	//	0x7D000(512000) * WORD = 2M
								 };

#endif

	uNumClock = sizeof(aCaseOfClock)/sizeof(aCaseOfClock[0]);
	uNumOndMode = sizeof(aCaseOfDMAMode)/sizeof(aCaseOfDMAMode[0]);

	puTxDataBuffer = (u32 *)malloc(MAX_DMA_CNT*WORD);
	puRxDataBuffer = (u32 *)malloc(MAX_DMA_CNT*WORD);


	// 1. Set up the tx buf.
	for (k = 0; k<MAX_DMA_CNT*WORD; k++)
		*((u8 *)puTxDataBuffer+k) = (u8)(k+2)%0xff;

	for(i=0 ; i<uNumClock ; i++)
	{
		if(aCaseOfClock[i][0] == eSYNC_MODE)
		{
			SYSC_ChangeMode(eSYNC_MODE);
			SYSC_ChangeSYSCLK_1((APLL_eOUT)aCaseOfClock[i][1], (APLL_eOUT)aCaseOfClock[i][2], 0, 1, 3);
			SYSC_SetDIV0(0, 1, aCaseOfClock[i][4]-1, aCaseOfClock[i][3]-1,  3, aCaseOfClock[i][5]-1, 1,0, 1, 0);

			SYSC_GetClkInform();
			OpenConsole();

			UART_Printf("\n\n");
			UART_Printf("====================================================================\n");
			UART_Printf("Sync Mode : Synchronous\n");
			UART_Printf("ARMCLK : HCLK : FlashCLK : SMCLK = %d : %d : %d : %d MHz\n", 
								g_ARMCLK/1000000, g_HCLK/1000000, 
								g_ARMCLK/(1000000*aCaseOfClock[i][3]*aCaseOfClock[i][5]),
								g_ARMCLK/(1000000*aCaseOfClock[i][3]*aCaseOfClock[i][5]*aCaseOfClock[i][6]) );
		}
		else		// Async Mode
		{
			SYSC_ChangeMode(eASYNC_MODE);
			SYSC_ChangeSYSCLK_1((APLL_eOUT)aCaseOfClock[i][1], (APLL_eOUT)aCaseOfClock[i][2], 0, 1, 3);
			SYSC_SetDIV0(0, 1, aCaseOfClock[i][4]-1, aCaseOfClock[i][3]-1,  3, aCaseOfClock[i][5]-1, 1,0, 1, 0);

			SYSC_GetClkInform();
			OpenConsole();

			UART_Printf("\n\n");
			UART_Printf("====================================================================\n");
			UART_Printf("Sync Mode : Asynchronous\n");
			UART_Printf("ARMCLK : HCLK : FlashCLK : SMCLK = %d : %d : %d : %d MHz (MPLL : %d MHz)\n", 
								g_ARMCLK/1000000, g_HCLK/1000000, 
								g_MPLL/(1000000*aCaseOfClock[i][3]*aCaseOfClock[i][5]),
								g_MPLL/(1000000*aCaseOfClock[i][3]*aCaseOfClock[i][5]*aCaseOfClock[i][6]), g_MPLL );			
		}
		UART_Printf("\n");

		for(j=0 ; j<uNumOndMode ; j++)
		{
			uIntSrcNo	= aCaseOfDMAMode[j][1];
			eDmaUnit	= (DMA_UNIT)aCaseOfDMAMode[j][0];
			eDmaCh		= (DMA_CH)aCaseOfDMAMode[j][2];
			eBurstMode	= (BURST_MODE)aCaseOfDMAMode[j][3];
			uDataCnts 	= aCaseOfDMAMode[j][4];
			g_DmaDone = 0;
			
			// 0. Clear the rx/tx buf.
			for (k = 0; k<(uDataCnts*WORD); k++)
			{
				*((u8 *)puRxDataBuffer+k) = 0;
			}
			
			UART_Printf("------------------------------ %d --------------------------------------\n", j);

		     	DMAC_InitCh(eDmaUnit, eDmaCh, &oDmac0);
			INTC_SetVectAddr(uIntSrcNo,  Dma0Done);
			INTC_Enable(uIntSrcNo);

			DMACH_Setup((DMA_CH)eDmaCh, 0x0, (u32)puTxDataBuffer, 0, (u32)puRxDataBuffer, 0, (DATA_SIZE)WORD, uDataCnts, DEMAND, MEM, MEM, (BURST_MODE)eBurstMode, &oDmac0);
		        // Enable DMA
			DMACH_Start(&oDmac0);

			while(g_DmaDone==0);								// Int.

			if (CompareDMA((u32)puTxDataBuffer, (u32)puRxDataBuffer, (DATA_SIZE)WORD, uDataCnts))
			{
				UART_Printf(" >> Test Tx&Rx -> Ok << \n");
			}
			else
			{
				UART_Printf(" >>*** Tx-data & Rx-data mismatch ***<< \n");
			}

			INTC_Disable(uIntSrcNo);
		    	DMAC_Close(eDmaUnit, eDmaCh, &oDmac0);

		}
		
		UART_Printf("====================================================================\n");
	}
		
}




//////////
// Function Name : DMAT_Autotest
// Function Description : DMA Autotest based on full function test
// Input : 	None
// Version : v0.1

u8 DMAT_Autotest(void)
{	
	u32 i, j, k,uNumClock, uNumOndMode;
	u32	*puTxDataBuffer, *puRxDataBuffer;

	u32			uIntSrcNo	= NUM_DMA0;
	DMA_UNIT	eDmaUnit 	= DMA0;
	DMA_CH		eDmaCh		= DMA_A;
	BURST_MODE	eBurstMode	= BURST4;
	u32			uDataCnts	= 0;	
	u8			bFlagOrder = TRUE;
	
						   //{Sync Mode, APLL, MPLL, HCLKx2 divide, HCLK divide, Flash CLK divide, SMCLK divide}
	u32 aCaseOfClock[][7] = { //Sync Mode
							{eSYNC_MODE, eAPLL532M, eAPLL266M, 4, 2, 2, 2},
							};
							//	{Controller, 	Interrupt Src,	Channel, 		Burst, 		Data Cnt}
	u32 aCaseOfDMAMode[][5] = {	{DMA0, 		NUM_DMA0, 	DMA_A, 		BURST4,		MAX_AUTODMA_CNT},		// 	0x200(512) * WORD = 2K
								{DMA0, 		NUM_DMA0, 	DMA_B,		BURST8,		MAX_AUTODMA_CNT},		//	0x200(512) * WORD = 2K
								{DMA0, 		NUM_DMA0, 	DMA_C,		BURST16,	MAX_AUTODMA_CNT},		//	0x200(5120) * WORD = 20K
								{DMA0, 		NUM_DMA0, 	DMA_D,		BURST32,	MAX_AUTODMA_CNT},		//	0x200(5120) * WORD = 20K
								{DMA0, 		NUM_DMA0, 	DMA_E,		BURST64,	MAX_AUTODMA_CNT},		//	0xC800(51200) * WORD = 200K
								{DMA0, 		NUM_DMA0, 	DMA_F,		BURST128,	MAX_AUTODMA_CNT},		//	0xC800(51200) * WORD = 200K
								{DMA0, 		NUM_DMA0, 	DMA_G,		BURST256,	MAX_AUTODMA_CNT},	//	0x7D000(512000) * WORD = 2M
								{DMA0, 		NUM_DMA0, 	DMA_H,		BURST256,	MAX_AUTODMA_CNT},	//	0x7D000(512000) * WORD = 2M

								{DMA1, 		NUM_DMA1, 	DMA_A, 		BURST4,		MAX_AUTODMA_CNT},		// 	0x200(512) * WORD = 2K
								{DMA1, 		NUM_DMA1, 	DMA_B,		BURST8,		MAX_AUTODMA_CNT},		//	0x200(512) * WORD = 2K
								{DMA1, 		NUM_DMA1, 	DMA_C,		BURST16,	MAX_AUTODMA_CNT},		//	0x200(5120) * WORD = 20K
								{DMA1, 		NUM_DMA1, 	DMA_D,		BURST32,	MAX_AUTODMA_CNT},		//	0x200(5120) * WORD = 20K
								{DMA1, 		NUM_DMA1, 	DMA_E,		BURST64,	MAX_AUTODMA_CNT},		//	0xC800(51200) * WORD = 200K
								{DMA1, 		NUM_DMA1, 	DMA_F,		BURST128,	MAX_AUTODMA_CNT},		//	0xC800(51200) * WORD = 200K
								{DMA1, 		NUM_DMA1, 	DMA_G,		BURST256,	MAX_AUTODMA_CNT},	//	0x7D000(512000) * WORD = 2M
								{DMA1, 		NUM_DMA1, 	DMA_H,		BURST256,	MAX_AUTODMA_CNT},	//	0x7D000(512000) * WORD = 2M
								
								{SDMA0, 		NUM_SDMA0, 	DMA_A, 		BURST4,		MAX_AUTODMA_CNT},		// 	0x200(512) * WORD = 2K
								{SDMA0, 		NUM_SDMA0, 	DMA_B,		BURST8,		MAX_AUTODMA_CNT},		//	0x200(512) * WORD = 2K
								{SDMA0, 		NUM_SDMA0, 	DMA_C,		BURST16,	MAX_AUTODMA_CNT},		//	0x200(5120) * WORD = 20K
								{SDMA0, 		NUM_SDMA0, 	DMA_D,		BURST32,	MAX_AUTODMA_CNT},		//	0x200(5120) * WORD = 20K
								{SDMA0, 		NUM_SDMA0, 	DMA_E,		BURST64,	MAX_AUTODMA_CNT},		//	0xC800(51200) * WORD = 200K
								{SDMA0, 		NUM_SDMA0, 	DMA_F,		BURST128,	MAX_AUTODMA_CNT},		//	0xC800(51200) * WORD = 200K
								{SDMA0, 		NUM_SDMA0, 	DMA_G,		BURST256,	MAX_AUTODMA_CNT},	//	0x7D000(512000) * WORD = 2M
								{SDMA0, 		NUM_SDMA0, 	DMA_H,		BURST256,	MAX_AUTODMA_CNT},	//	0x7D000(512000) * WORD = 2M

								{SDMA1, 		NUM_SDMA1, 	DMA_A, 		BURST4,		MAX_AUTODMA_CNT},		// 	0x200(512) * WORD = 2K
								{SDMA1, 		NUM_SDMA1, 	DMA_B,		BURST8,		MAX_AUTODMA_CNT},		//	0x200(512) * WORD = 2K
								{SDMA1, 		NUM_SDMA1, 	DMA_C,		BURST16,	MAX_AUTODMA_CNT},		//	0x200(5120) * WORD = 20K
								{SDMA1, 		NUM_SDMA1, 	DMA_D,		BURST32,	MAX_AUTODMA_CNT},		//	0x200(5120) * WORD = 20K
								{SDMA1, 		NUM_SDMA1, 	DMA_E,		BURST64,	MAX_AUTODMA_CNT},		//	0xC800(51200) * WORD = 200K
								{SDMA1, 		NUM_SDMA1, 	DMA_F,		BURST128,	MAX_AUTODMA_CNT},		//	0xC800(51200) * WORD = 200K
								{SDMA1, 		NUM_SDMA1, 	DMA_G,		BURST256,	MAX_AUTODMA_CNT},	//	0x7D000(512000) * WORD = 2M
								{SDMA1, 		NUM_SDMA1, 	DMA_H,		BURST256,	MAX_AUTODMA_CNT},	//	0x7D000(512000) * WORD = 2M
								 };



	uNumClock = sizeof(aCaseOfClock)/sizeof(aCaseOfClock[0]);
	uNumOndMode = sizeof(aCaseOfDMAMode)/sizeof(aCaseOfDMAMode[0]);

	puTxDataBuffer = (u32 *)malloc(MAX_AUTODMA_CNT*WORD);
	puRxDataBuffer = (u32 *)malloc(MAX_AUTODMA_CNT*WORD);


	// 1. Set up the tx buf.
	for (k = 0; k<MAX_AUTODMA_CNT*WORD; k++)
		*((u8 *)puTxDataBuffer+k) = (u8)(k+2)%0xff;

		UART_Printf("\n");

		for(j=0 ; j<uNumOndMode ; j++)
		{
			uIntSrcNo	= aCaseOfDMAMode[j][1];
			eDmaUnit	= (DMA_UNIT)aCaseOfDMAMode[j][0];
			eDmaCh		= (DMA_CH)aCaseOfDMAMode[j][2];
			eBurstMode	= (BURST_MODE)aCaseOfDMAMode[j][3];
			uDataCnts 	= aCaseOfDMAMode[j][4];
			g_DmaDone = 0;
			
			// 0. Clear the rx/tx buf.
			for (k = 0; k<(uDataCnts*WORD); k++)
			{
				*((u8 *)puRxDataBuffer+k) = 0;
			}
			
			UART_Printf("------------------------------ %d --------------------------------------\n", j);

		     	DMAC_InitCh(eDmaUnit, eDmaCh, &oDmac0);
			INTC_SetVectAddr(uIntSrcNo,  Dma0Done);
			INTC_Enable(uIntSrcNo);

			DMACH_Setup((DMA_CH)eDmaCh, 0x0, (u32)puTxDataBuffer, 0, (u32)puRxDataBuffer, 0, (DATA_SIZE)WORD, uDataCnts, DEMAND, MEM, MEM, (BURST_MODE)eBurstMode, &oDmac0);
		        // Enable DMA
			DMACH_Start(&oDmac0);

			while(g_DmaDone==0);								// Int.

			if (CompareDMA((u32)puTxDataBuffer, (u32)puRxDataBuffer, (DATA_SIZE)WORD, uDataCnts))
			{
				UART_Printf(" >> Test Tx&Rx -> Ok << \n");
				
			}
			else
			{
				UART_Printf(" >>*** Tx-data & Rx-data mismatch ***<< \n");
				bFlagOrder = FALSE;
			}

			INTC_Disable(uIntSrcNo);
		    	DMAC_Close(eDmaUnit, eDmaCh, &oDmac0);

		}
		
		UART_Printf("====================================================================\n");


	free(puTxDataBuffer);
	free(puRxDataBuffer);
	if(bFlagOrder==FALSE)
		return false;
	else
		return true;
		
}



///////////////////////////////////////////////////////////////////////////////////
////////////////////                    DMA Main Test                  /////////////////////////// 
///////////////////////////////////////////////////////////////////////////////////

const testFuncMenu dma_menu[] =
{
		0,                      	"Exit",
		Mem_dump,					"Memory dump			",
		DMAT_MemtoMem,				"Test DMA From Mem To Mem",
		DMAT_MemtoMemLLI,			"Test DMA LLI From Mem To Mem",
		DMAT_SoftReq,				"Test DMA Software Request",
		DMAT_WorstCase,				"Test 4 DMAs concurrently",
		DMAT_External,				"Test External DMA through Modem I/F",
//		DMAT_ExternalDMAReq,		"Test External DMA Request",
		DMAT_FullFunction,			"Test DMA Full Function",
		DMAT_Autotest,				"Autotest",
//		TestGPIO,					"Test GPIO ",
		0,0
};

void Test_DMA(void)
{
	u32 i;
	s32 uSel;

	UART_Printf("[DMA_Test]\n\n");
	
	while(1)
	{
		for (i=0; (u32)(dma_menu[i].desc)!=0; i++)
			UART_Printf("%2d: %s\n", i, dma_menu[i].desc);

		UART_Printf("\nSelect the function to test : ");
		uSel =UART_GetIntNum();
		UART_Printf("\n");
		if(uSel == -1) 
			break;
		if (uSel==0)
			break;
		if (uSel>=0 && uSel<(sizeof(dma_menu)/8-1))
			(dma_menu[uSel].func) ();
	}
}	




