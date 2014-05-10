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
*	File Name : irDA.c
*  
*	File Description : This file implements the API functons for IrDA
*
*	Author : Woojin.Kim
*	Dept. : AP Development Team
*	Created Date : 2007/01/25
*	Version : 0.1 
* 
*	History"
*	- Created(Woojin.Kim 2007/01/25)
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

volatile u32 g_uIrDA_DONE;

volatile u32 g_uIrDA_TxDone;
volatile u32 g_uIrDA_Txcount = 0;
volatile u32 g_uIrDA_TxISRcount = 0;

volatile u32 g_uIrDA_RxWrPnt;
volatile u32 g_uIrDA_RxRdPnt;
volatile u32 g_uIrDA_RxISRcount = 0;

volatile u8 * pIrDA_RXBUFFER;
volatile u8 * pIrDA_TXBUFFER;
volatile u8 * pIrDA_CKBUFFER;

volatile s32 g_uIrDA_TxNum;
volatile s32 g_uIrDA_TxCnt;

u8 *g_pbHead;
u8 *g_pbTail;

u32 g_uIrDA_LastDataRead = 0;

volatile u32 g_aIrDA_TestBuf[20];
volatile u32 g_uIrDA_TestCnt = 0;
volatile u32 g_aIrDA_TestInt[10];
volatile u32 g_uIrDA_IntCnt = 0;

DMAC oIrDADma;

//////////
// Function Name : IrDA_Reset
// Function Description : This function Initialize IrDA register by writing 0 on it
// Input : NONE
// Output : NONE
// Version : v0.1
void IrDA_Reset(void)
{
	Outp32(rIrDA_CNT , 0 );
	Outp32(rIrDA_MDR , 0 );
	Outp32(rIrDA_CNF , 0 );
	Outp32(rIrDA_IER , 0 );
	Outp32(rIrDA_IIR , 0 );
	Outp32(rIrDA_LSR , 0x83);
	Outp32(rIrDA_FCR , 0 );
	Outp32(rIrDA_PLR , 0 );
	Outp32(rIrDA_TXFLL , 0 );
	Outp32(rIrDA_TXFLH , 0 );
	Outp32(rIrDA_RXFLL , 0 );
	Outp32(rIrDA_RXFLH , 0 );
}


//////////
// Function Name : IrDA_SetPort
// Function Description : This function set up GPIO to use IrDA
// Input : NONE
// Output : NONE
// Version : v0.1
void IrDA_SetPort(u8 SelPort)
{

	if(SelPort == 3) 									// uart3 客 muxing 等 IrDa
		{
		GPIO_SetFunctionEach(eGPIO_B, eGPIO_2,3);
		GPIO_SetFunctionEach(eGPIO_B, eGPIO_3,3);
		GPIO_SetFunctionEach(eGPIO_B, eGPIO_4,2);
		GPIO_SetPullUpDownEach(eGPIO_B, eGPIO_2,0);
		GPIO_SetPullUpDownEach(eGPIO_B, eGPIO_3,0);
		GPIO_SetPullUpDownEach(eGPIO_B, eGPIO_4,0);		
		    UART_Printf("IrDA Port Initialize!!!\n");
		}
	else												// uart2 客 muxing 等 IrDA
		{
		GPIO_SetFunctionEach(eGPIO_B, eGPIO_0,4);
		GPIO_SetFunctionEach(eGPIO_B, eGPIO_1,4);
		GPIO_SetFunctionEach(eGPIO_B, eGPIO_4,2);
		GPIO_SetPullUpDownEach(eGPIO_B, eGPIO_0,0);
		GPIO_SetPullUpDownEach(eGPIO_B, eGPIO_1,0);
		GPIO_SetPullUpDownEach(eGPIO_B, eGPIO_4,0);
		    UART_Printf("IrDA Port Initialize!!!\n");
		}

}


//////////
// Function Name : IrDA_SetPort
// Function Description : This function re-set up GPIO default
// Input : NONE
// Output : NONE
// Version : v0.1
void IrDA_ReturnPort(void)
{
#if 1
	GPIO_SetFunctionEach(eGPIO_B, eGPIO_0,0);
	GPIO_SetFunctionEach(eGPIO_B, eGPIO_1,0);
	GPIO_SetFunctionEach(eGPIO_B, eGPIO_4,0);
#else									
	GPIO_SetFunctionEach(eGPIO_B, eGPIO_2,0);
	GPIO_SetFunctionEach(eGPIO_B, eGPIO_3,0);
	GPIO_SetFunctionEach(eGPIO_B, eGPIO_4,0);
#endif
	
}


//////////
// Function Name : IrDA_ClkSrc
// Function Description : This function select IrDA clock source
// Input :  Ext48Mhz, MPLL, EPLL
// Output : NONE
// Version : v0.1
void IrDA_ClkSrc(eIrDA_ClkSrc ClkSrc)
{
	SYSC_SetDIV2(0,0,0,0,0,0);
	switch(ClkSrc)
		{
			case Ext48Mhz : 						// you need to turn on USB power to use this clk
				
					SYSC_ClkSrc(eIRDA_48M);
					break;
					
			case MPLL:   

					SYSC_SetPLL(eMPLL,384,3,4,0);	//48Mhz
					UART_Getc();					//to compensate PLL lock time
					SYSC_ClkSrc(eMPLL_FOUT);
					SYSC_ClkSrc(eIRDA_DOUTMPLL);
					
					SYSC_GetClkInform();
					UART_Printf("MPLL = %dMhz\n",g_MPLL/1000000/2);
					break;
						
			case EPLL:
					SYSC_SetPLL(eEPLL,56,7,1,0);	//48Mhz
					SYSC_ClkSrc(eIRDA_MOUTEPLL);
					SYSC_ClkSrc(eEPLL_FOUT);

					break;
			default :								//default : use EPLL
					SYSC_SetPLL(eEPLL,56,7,1,0);
					SYSC_ClkSrc(eIRDA_MOUTEPLL);
					SYSC_ClkSrc(eEPLL_FOUT);

					break;

		}


}


//////////
// Function Name : IrDA_Init
// Function Description : This function Initialize IrDA by input
// Input :uMODE [ 010 : MIR mode	100 : FIR mode]
// 		 uPREAMBLE [ 00: 16,   01: 4,   10: 8,  11: 32 preamble data length]
// 		 uSTARTFLAG [ 2 ~ 15 start flag]
//		 uRXTXFL [ 0 ~ 65535 Frame length]
//		 uRXTXTRIG [ 00 - reserved
//					01 - TX(12/48),RX(4/16), 
//					10 - TX(8/32),RX(8/32),  
//					11 - TX(2/8),RX(14/56) [for 16/64byte]]  FIFO TRIG level set
// Output : NONE
// Version : v0.1
void IrDA_Init(u32 uMODE, u32 uPREAMBLE, u32 uSTARTFLAG, u32 uRXTXFL, u32 uRXTXTRIG)

{
	int selFIR = 0;
	int selMIR = 0;
	
	if (uMODE != 0)
	{
		selFIR = 1;
		if (uMODE == 1)
		{
			selMIR = 0;
			UART_Printf(" [MIR full mode]\n");	
		}
		if (uMODE == 2)
		{
			selMIR = 1;
			UART_Printf(" [MIR half mode]\n");	
		}
	}		
	else 
	{
		//selFIR = 4;
		selFIR = 0;
		UART_Printf(" [FIR mode]\n");
	}

	// IrDA Control Register
	// Tx disable/Rx disable/No Frame abort/SDBW High
	Outp32(rIrDA_CNT , (IrDA_LOOP_MODE<<5)|(selMIR<<4)|(IrDA_SEND_SIP<<3)|(1));

	// Mode & Transceiver Set Register
	Outp32(rIrDA_MDR , (1<<4) | (1<<3) | selFIR);	// Sip end of  frame / HP / Mode 
	//rIrDA_MDR = (0<<4) | (1<<3) | selFIR;	// Sip every frame / HP / Mode 

	UART_Printf("rMDR = 0x%x, rCNT = 0x%x\n",Inp32(rIrDA_MDR), Inp32(rIrDA_CNT));

	// Interrupt & DMA Control Register
	Outp32(rIrDA_CNF , 0x0);		// Disable Interrupt & DMA

	// Interrupt Enable Register
	Outp32(rIrDA_IER , 0x0);		// Disable All interrupts

	// FIFO Control Register
	// Tx FIFO reset[2] / RX FIFO reset[1]
	Outp32(rIrDA_FCR , (uRXTXTRIG<<6)|(IrDA_FIFOSIZE<<5)|(1<<2)|(1<<1)|(IrDA_FIFOENB));

	// Set Start Field Register or Preamble length
	Outp32(rIrDA_PLR , ((uPREAMBLE << 6) | (uRXTXTRIG<<4) | uSTARTFLAG));
	//rIrDA_PLR= ((uPREAMBLE << 6) | (3<<4) | uSTARTFLAG);

	// Receive Frame Length Register
	//rIrDA_RXFLL = uRXTXFL & 0xff;
	//rIrDA_RXFLH= (uRXTXFL>>8) & 0xff;
	Outp32(rIrDA_RXFLL , 0xff);
	Outp32(rIrDA_RXFLH , 0xff);
	UART_Printf(" [RXFL-L] %d, [RXFL-H] %d\n", (u8)Inp32(rIrDA_RXFLL), (u8)Inp32(rIrDA_RXFLH));

	// Transmit Frame Length Register
	Outp32(rIrDA_TXFLL , (uRXTXFL & 0xff));
	Outp32(rIrDA_TXFLH , ((uRXTXFL>>8) & 0xff));
	UART_Printf(" [TXFL-L] %d, [TXFL-H] %d\n", (u8)Inp32(rIrDA_TXFLL), (u8)Inp32(rIrDA_TXFLH));

	while(!(Inp32(rIrDA_FCR) & 0x18)) 
		if(UART_GetKey()) break;

	UART_Printf("Tx and Rx FIFO clear is over...\n");
//	return 1;
}




//////////
// Function Name : IrDA_IntClear
// Function Description : This function clears interrupt
// Input : NONE
// Output : NONE
// Version : v0.1

void IrDA_IntClear(void)
{
	Outp32(rIrDA_INTCLR,0xffffffff);
}


//////////
// Function Name : IrDA_EnRx
// Function Description : This function enables/disable Rx
// Input : uEn [0: Disable	1: Enable]
// Output : NONE
// Version : v0.1

void IrDA_EnRx(u32 uEn)
{
	u32 uTemp;
	uTemp = Inp32(rIrDA_CNT);
	uTemp |= (uEn<<6);
	Outp32(rIrDA_CNT,uTemp);
}

//////////
// Function Name : IrDA_EnTx
// Function Description : This function enables/disable Tx
// Input : uEn [0: Disable	1: Enable]
// Output : NONE
// Version : v0.1

void IrDA_EnTx(u32 uEn)
{
	u32 uTemp;
	uTemp = Inp32(rIrDA_CNT);
	uTemp |= (uEn<<7);
	Outp32(rIrDA_CNT,uTemp);
}

//////////
// Function Name : IrDA_SetIER
// Function Description : This function enables Interrupt output signal
// Input : 	LB,ErrInd,TxUnder,LBdetect,RxOver,LBread,TxFbelow,RxFover [0:Disable	 1:Enable]
// Output : NONE
// Version : v0.1

void IrDA_SetIER(u32 LB,u32 ErrInd,u32 TxUnder,u32 LBdetect,u32 RxOver,u32 LBread,u32 TxFbelow,u32 RxFover)
{

	Outp32(rIrDA_IER,(LB<<7)|(ErrInd<<6)|(TxUnder<<5)
		|(LBdetect<<4)|(RxOver<<3)|(LBread<<2)|(TxFbelow<<1)|(RxFover<<0));
}


//////////
// Function Name : IrDA_ReadLSR
// Function Description : This function enables Interrupt output signal
// Input : NONE
// Output : NONE
// Version : v0.1

u32 IrDA_ReadLSR(void)
{
	return Inp32(rIrDA_LSR);
}


//////////
// Function Name : IrDA_EnInt
// Function Description : This function enables Interrupt output signal
// Input : uEn [0: Disable	1: Enable]
// Output : NONE
// Version : v0.1

void IrDA_EnInt(u32 uEn)
{
	Outp32(rIrDA_CNF,uEn);
}


//////////
// Function Name : IrDA_EnDMA
// Function Description : This function enables DMA with selected TX/RX mode
// Input : uTxRx [ 0: Tx DMA	1: Rx DMA ]
// Output : NONE
// Version : v0.1

void IrDA_EnDMA(u32 uTxRx)
{
	Outp32(rIrDA_CNF,(1<<3)|(uTxRx<<2)|0);
}


//////////
// Function Name : Isr_IrDA_Int_Rx
// Function Description : This function is Interrupt Service Routine of IrDA Rx interrupt mode
// Input : NONE
// Output : NONE
// Version : v0.1
void __irq Isr_IrDA_Int_Rx(void)
{
	u8 cStatus=0;
	eIrDA_Error Error = No_Error;
	u32 uTmp;
	
	//rb1004........ Caution : Don't use UART_Printf() function in IrDA ISR, othewise IrDA may be not operated because of delay.
	INTC_Disable(NUM_IRDA);
	IrDA_IntClear();
//	Outp32(rIrDA_INTCLR , 0xFFFFFFFF);	

	g_uIrDA_RxISRcount++;

	cStatus = (u8)Inp32(rIrDA_IIR); // Sub-sub pending clear
	//UART_Printf("\nrIrDA_FCR : 0x%08x\n",rIrDA_FCR);


 	g_aIrDA_TestInt[g_uIrDA_IntCnt++] = cStatus;
	g_aIrDA_TestBuf[g_uIrDA_TestCnt++] = 1;

	if(cStatus & (1<<6)) // RX error indication
	{ 
		UART_Printf("\nrIrDA_IIR : 0x%08x\n",cStatus);
		Error = __Isr_IrDA_Sub_RxErr();
		g_aIrDA_TestBuf[g_uIrDA_TestCnt++] = 2;
	}

	if(cStatus & (1<<3))  // RX Overrun
	{
		UART_Printf("IrDA Rx Overrun Error Occurs!!\n");
		UART_Printf("Rx FIFO Reset.... : 0x%x\n", Error);
		uTmp = Inp32(rIrDA_FCR);
		uTmp |= 0x02;
		Outp32(rIrDA_FCR,uTmp);
		while( !(Inp32(rIrDA_FCR) & 0x08) );
		Error = RxOverRun_Error;
		g_aIrDA_TestBuf[g_uIrDA_TestCnt++] = 3;
	}

	if(Error == No_Error)
	{
		//for(i=0 ; i<500 ; i++);
		if(cStatus & 0x01) 
		{
			while(Inp32(rIrDA_RXNO) > 0) 
			{
				*(pIrDA_RXBUFFER+g_uIrDA_RxWrPnt) = (u8)Inp32(rIrDA_RBR);
				g_uIrDA_RxWrPnt++;
//				UART_Printf("RxNO : 0x%x\n",Inp32(rIrDA_RXNO));
				//for(i=0 ; i<50 ; i++);
			}
			g_aIrDA_TestBuf[g_uIrDA_TestCnt++] = 4;
		} 
		
	}
	else
	{	
		//Reg_Dump();
		g_aIrDA_TestBuf[g_uIrDA_TestCnt++] = 5;
		if(Error & CRC_Error)
			UART_Printf("CRC error...!!!\n");
		if(Error & PHY_Error)
			UART_Printf("Phy error...!!!\n");
		if(Error & Frame_Error)
			UART_Printf("Frame Length error...!!!\n");
		
		//UART_Printf("Rx FIFO Reset.... : 0x%x\n", Error);
		//rIrDA_FCR |= 0x02;
		//while( !(rIrDA_FCR & 0x08) );
	}

	if(cStatus & 0x04) //Last data read from FIFO
	{
		g_uIrDA_LastDataRead = 1;
		g_aIrDA_TestBuf[g_uIrDA_TestCnt++] = 6;
	}
		

	INTC_Enable(NUM_IRDA);
	
	INTC_ClearVectAddr();
}


//////////
// Function Name : __Isr_IrDA_Sub_RxErr
// Function Description : This function is sub Interrupt Service Routine of IrDA Int Rx which presents Error status
// Input : NONE
// Output : eIrDA_Error
// Version : v0.1
eIrDA_Error __Isr_IrDA_Sub_RxErr()
{
	u32 uIrDA_ErrorStatus;
	// Modified by rb1004
	uIrDA_ErrorStatus = IrDA_ReadLSR();
	UART_Printf("\nError.......[g_uIrDA_ErrorStatus : 0x%08x]\n", uIrDA_ErrorStatus);
	uIrDA_ErrorStatus &= 0x1c;
	
	switch(uIrDA_ErrorStatus>>2)	//to clear and check the status of register bits
	{
		case 1:

			return CRC_Error;

		case 2:

			return PHY_Error;

		case 3:

			return (eIrDA_Error)(CRC_Error | PHY_Error);

		case 4:

			return Frame_Error;

		case 5:

			return (eIrDA_Error)(CRC_Error | Frame_Error);

		case 6:

			return (eIrDA_Error)(PHY_Error | Frame_Error);

		case 7:

			return (eIrDA_Error)(CRC_Error | PHY_Error | Frame_Error);

		default :
			return No_Error;
//			break;
	}
}

//==============================================================================


//////////
// Function Name : Isr_IrDA_Int_Tx
// Function Description : This function is Interrupt Service Routine of IrDA Tx interrupt mode
// Input : NONE
// Output : NONE
// Version : v0.1
void __irq Isr_IrDA_Int_Tx()
{
	u8 cStatus=0;
	u32 uTemp;
	u32 uCnt;
	
	INTC_Disable(NUM_IRDA);
	IrDA_IntClear();

	cStatus = (u8)Inp32(rIrDA_IIR);			// Read IIR(Interrupt Identification Register) of IrDA

	if(cStatus & 0x2) // TX Trigger
	{
//		for (uCnt=0;uCnt<5;uCnt++)			//for 65535
		for (uCnt=0;uCnt<IrDA_TxBUFLEN;uCnt++)
		{
			//rIrDA_RBR = g_cData;
			//*pIrDA_CKBUFFER++ = g_cData++;
			Outp32(rIrDA_RBR , *pIrDA_CKBUFFER++);
//			UART_Printf("FIFO TxNO : 0x%x\n",Inp32(rIrDA_TXNO));
		}

		g_uIrDA_TxCnt += IrDA_TxBUFLEN;
//		g_uIrDA_TxCnt += 5;					//for 65536
		//g_uIrDA_TxCnt +=g_uIrDA_TxNum;
		if (g_uIrDA_TxNum <= g_uIrDA_TxCnt) 
		{
			uTemp = Inp32(rIrDA_IER);
			uTemp &= ~(1<<1);
			Outp32(rIrDA_IER,uTemp);
//			rIrDA_IER &= ~(1<<1);
			g_uIrDA_TxDone = 1;
		}
		
	}
//	if(cStatus & 0x20) // TX Trigger
//	{
//		UART_Printf("Tx Underrun error!! \n");
//		pIrDA_CKBUFFER  = (unsigned char *)MemoryCk; 
//		pIrDA_CKBUFFER1 = (unsigned char *)MemoryCk1;	
//		for(i=0; i<g_uIrDA_TxCnt ; i++)
//			UART_Printf("[D:%3d, N:%3d] ", *pIrDA_CKBUFFER++, *pIrDA_CKBUFFER1++);
//	}


	INTC_Enable(NUM_IRDA);
	INTC_ClearVectAddr();

}


// IrDA Interrupt Test Code end============================================================[END]



// IrDA DMA Test Code Start=============================================================[START]
void __irq Isr_IrDA_Dma_RxDone(void)
{
	u32 i;
	u32 uRegTmp;
	
	eIrDA_Error Error = No_Error;
	u8 status = No_Error;

	INTC_Disable(NUM_DMA1);

	DMACH_Stop(&oIrDADma);
	DMACH_ClearIntPending(&oIrDADma);
	DMACH_ClearErrIntPending(&oIrDADma);



	status = (u8)Inp32(rIrDA_IIR); // Sub-sub pending clear

	if(status & (1<<3))  // RX Overrun
	{
		UART_Printf("IrDA Rx Overrun Error Occurs!!\n");
		UART_Printf("Rx FIFO Reset.... : 0x%x\n", Error);

		uRegTmp = Inp32(rIrDA_FCR);
		uRegTmp |= 0x02;
		Outp32(rIrDA_FCR,uRegTmp);

		while( !(Inp32(rIrDA_FCR) & 0x08) );
		Error = RxOverRun_Error;
	}

	if(status & (1<<6)) // RX error indication
	{ 
		Error = __Isr_IrDA_Sub_RxErr();
	}

	if(Error == No_Error)
	{
		//g_uIrDA_DataRcvCnt += IrDAtoDMA_RX_SIZE;
		for(i=0 ; i<IrDAtoDMA_RX_SIZE ; i++)
			*g_pbHead++ = *pIrDA_RXBUFFER++;
	}
	else
	{
//		UART_Printf("\nError.......[g_uIrDA_ErrorStatus : 0x%08x]\n", g_uIrDA_ErrorStatus);
		if(Error & CRC_Error)
			UART_Printf("CRC error...!!!\n");
		if(Error & PHY_Error)
			UART_Printf("Phy error...!!!\n");
		if(Error & Frame_Error)
			UART_Printf("Frame Length error...!!!\n");
		
		//UART_Printf("Rx FIFO Reset.... : 0x%x\n", Error);
		//rIrDA_FCR |= 0x02;
		//while( !(rIrDA_FCR & 0x08) );
	}

	//if(Irda_Status & 0x80)
	g_uIrDA_DONE = 1;
	//g_uIrDA_DataRcvCnt += IrDAtoDMA_RX_SIZE;

	uRegTmp = DMACH_ReadDstAddr(&oIrDADma);
	uRegTmp+=1;
	DMACH_Setup(DMA_A,0x0,rIrDA_RBR,1,uRegTmp,0,BYTE,IrDAtoDMA_RX_SIZE,DEMAND,DMA1_IrDA,MEM,BURST16,&oIrDADma);

	INTC_Enable(NUM_DMA1);
	
	DMACH_Start(&oIrDADma);

	INTC_ClearVectAddr();
}



void Init_Irda_Dma_Rx(void)
{

	//--- DMAC 1 Initialize& Channel 1 
	SYSC_SelectDMA(eSEL_IRDA, 1);
	DMAC_InitCh(DMA1, DMA_A,&oIrDADma);


	
	DMACH_ClearIntPending(&oIrDADma);
	DMACH_ClearErrIntPending(&oIrDADma);


	UART_Printf("Source Address = 0x%x\n", rIrDA_RBR);
	UART_Printf("Dest. Address = 0x%x\n", pIrDA_RXBUFFER);

	// Channel Control Register

	DMACH_Setup(DMA_A,0x0,(u32)(rIrDA_RBR),1,(u32)pIrDA_RXBUFFER,0,BYTE,IrDAtoDMA_RX_SIZE,DEMAND,DMA1_IrDA,MEM,BURST16,&oIrDADma);

	INTC_Enable(NUM_DMA1);

	DMACH_Start(&oIrDADma);
	
}

void __irq Isr_IrDA_Dma_TxDone(void)
{
	u32 uRegTmp;


	eIrDA_Error Error = No_Error;
	u8 status = No_Error;
	
	INTC_Disable(NUM_DMA1);


	DMACH_ClearIntPending(&oIrDADma);
	DMACH_ClearErrIntPending(&oIrDADma);
	

	g_uIrDA_TxISRcount++;


	status = (u8)Inp32(rIrDA_IIR); // Sub-sub pending clear

	g_uIrDA_DONE = 1;
	
	if(status & (1<<5))  // RX Overrun
	{
		UART_Printf("IrDA Tx Underrun Error Occurs!!\n");
		UART_Printf("Tx FIFO Reset.... : 0x%x\n", Error);
		uRegTmp = Inp32(rIrDA_FCR);
		uRegTmp |= 0x4;
		Outp32(rIrDA_FCR,uRegTmp);

		while( !(Inp32(rIrDA_FCR) & 0x10) );
		Error = TxUnderRun_Error;
	}

//	if(status & (1<<6)) // RX error indication
//	{ 
//		Error = __Isr_IrDA_Sub_RxErr();
//	}

	if(Error == No_Error)
	{
		//for(i=0 ; i<IrDA_TxBUFLEN ; i++)
		//	*g_pbHead++ = *pIrDA_TXBUFFER++;
		g_uIrDA_Txcount += DMAtoIrDA_TX_SIZE;
	}
	/*
	else
	{
		UART_Printf("\nError.......[g_uIrDA_ErrorStatus : 0x%08x]\n", g_uIrDA_ErrorStatus);
		if(Error & CRC_Error)
			UART_Printf("CRC error...!!!\n");
		if(Error & PHY_Error)
			UART_Printf("Phy error...!!!\n");
		if(Error & Frame_Error)
			UART_Printf("Frame Length error...!!!\n");
		//UART_Printf("Rx FIFO Reset.... : 0x%x\n", Error);
		//rIrDA_FCR |= 0x02;
		//while( !(rIrDA_FCR & 0x08) );
	}
	*/
	//if(Irda_Status & 0x80)

/*
	if( g_uIrDA_Txcount < g_uIrDA_TxNum)
	{
		rDMAC2C2SrcAddr = rDMAC2C2SrcAddr+1;
		//rDMAC2C2Control |= (IrDA_TxBUFLEN);
		rDMAC2C2Control |= DMAtoIrDA_TX_SIZE;
		rDMAC2C2Config |= (1<<0);
		UART_Printf("g_uIrDA_Txcount < g_uIrDA_TxNum.............\n");
	}
	else
	{
		rDMAC2Configuration  = 0x0 ;
		UART_Printf("g_uIrDA_Txcount > g_uIrDA_TxNum..........End\n");
	}
*/
	INTC_Enable(NUM_DMA1);
	INTC_ClearVectAddr();

}

void Init_Irda_Dma_Tx()
{
		
	INTC_Disable(NUM_DMA1);
	
	//--- DMAC 1 Initialize& Channel 2 

	SYSC_SelectDMA(eSEL_IRDA, 1);
	DMAC_InitCh(DMA1,DMA_B,&oIrDADma);
	

	DMACH_ClearIntPending(&oIrDADma);
	DMACH_ClearErrIntPending(&oIrDADma);


	UART_Printf("Source Address = 0x%x\n", pIrDA_TXBUFFER);
	UART_Printf("Dest. Address = 0x%x\n", rIrDA_RBR);

	DMACH_Setup(DMA_B,0x0,(u32)pIrDA_TXBUFFER,0,rIrDA_RBR,1,BYTE,DMAtoIrDA_TX_SIZE,DEMAND,MEM,DMA1_IrDA,BURST32,&oIrDADma);	

	INTC_Enable(NUM_DMA1);


	DMACH_Start(&oIrDADma);
	
}






