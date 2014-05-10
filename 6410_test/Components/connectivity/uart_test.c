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
*	File Name : uart_test.c
*  
*	File Description : This file implements the API functons for Timer UART test.
*
*	Author : Woojin,Kim
*	Dept. : AP Development Team
*	Created Date : 2007/01/16
*	Version : 0.1 
* 
*	History
*	- Created (Woojin,Kim 2007/01/16)
*	- Modified and Added some test functions(Haksoo, Kim 2008/03/28)
*  
**************************************************************************************/


#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <stdio.h>
#include "system.h"
#include "dma.h"
#include "uart.h"
#include "intc.h"
#include "def.h"
#include "library.h"
#include "timer.h"

void UART_Test(void);
void Uart_BasicTest(void);
void Uart_BasicTx_withPC(void);
void Uart_BasicRx_withPC(void);
void Uart_BasicTx_Bd2Bd(void);
void Uart_BasicRx_Bd2Bd(void);
void Uart_PollingTx(void);
void Uart_PollingRx(void);
void Uart_IrDATx(void);
void Uart_IrDARx(void);
void Uart_SendBreak(void);
void Uart_LoopBack(void);
void Uart_CheckRtsTriggerLevel(void);
void Uart_CheckOverrunError(void);
void Uart_SpeedPollingTx(void);
void Uart_SpeedPollingRx(void);
void Uart_GpsModuleTest(void);

void RxBufferClear(void);
void Uart_SpeedTx(void);
void Uart_SpeedRx(void);
void TxBufferInit(void);
void Uart_LoopBackPerformance(void);
void Uart_TxAgingTest(void);
void Uart_RTS(void);
void Uart_CTS(void);
void Uart_ModemInt(void);

#define UART_BUF 		(0x51000000)
#define UART_TX_BUF		(UART_BUF)
#define UART_RX_BUF		(UART_BUF+0x00200000)

extern u32 g_uUartRxDmaTc;
extern RX_STRING_RESULT	g_sRxStringResult;

const testFuncMenu g_aUARTTestFunc[] =
{	
	Uart_BasicTest,					"UART Tx/Rx test by user Input(with PC)",
	Uart_BasicTx_withPC,			"UART Tx test by user Input(with PC)",
	Uart_BasicRx_withPC,			"UART Rx test by user Input(with PC)",
	Uart_BasicTx_Bd2Bd,				"UART Tx test by user Input(B'd to B'd)",
	Uart_BasicRx_Bd2Bd,				"UART Rx test by user Input(B'd to B'd)",
	Uart_PollingTx,					"UART Polling Tx test(with PC)",
	Uart_PollingRx,					"UART Polling Rx test(with PC)",
	Uart_IrDATx,					"UART IrDA Tx test",
	Uart_IrDARx,					"UART IrDA Rx test",
	Uart_SendBreak,					"UART SendBreak signal test",
	Uart_LoopBack,					"UART LoopBack test",
	Uart_CheckRtsTriggerLevel,		"UART RTS Trigger Level test(with PC)",	
	Uart_CheckOverrunError,			"UART Overrun Error test(with PC)",
	Uart_SpeedPollingTx,			"UART Polling Tx for checking max speed",
	Uart_SpeedPollingRx,			"UART Polling Rx for checking max speed\n",
	
	Uart_GpsModuleTest,				"UART i/f check with SirfIII GPS Module(Integration B/D Only)",
/*	
	Uart_SpeedTx,					"UART Speed Tx test with INT FIFO",
	Uart_SpeedRx,					"UART Speed Rx test with DMA FIFO(1byte)",
	Uart_LoopBackPerformance,		"UART Speed test by LoopBack mode",
	Uart_TxAgingTest,				"UART Tx Aging Test",
	Uart_RTS,						"UART RTS(manual mode) test",
	Uart_CTS,						"UART CTS(manual mode) test",
	Uart_ModemInt,					"UART Modem Interrupt test",
*/	
	0,0
};


void UART_Test(void)
{
	u32 uCountFunc=0;
	s32 iSel=0;

	while(1)
	{
		// for preventing dummy data from being transmitted
		InitUartPort(0,TRUE);
		InitUartPort(1,TRUE);
		InitUartPort(2,FALSE);
		InitUartPort(3,FALSE);
	
		UART_Printf("\n\n================== UART Function Test =====================\n\n");
		for (uCountFunc=0; (u32)(g_aUARTTestFunc[uCountFunc].desc)!=0; uCountFunc++)
			UART_Printf("%2d: %s\n", uCountFunc, g_aUARTTestFunc[uCountFunc].desc);

		UART_Printf("\nSelect the function to test : ");
		iSel =UART_GetIntNum();
		UART_Printf("\n");
		if(iSel == -1) 
			break;

		if (iSel>=0 && iSel<(sizeof(g_aUARTTestFunc)/8-1))
			(g_aUARTTestFunc[iSel].func) ();
	}
	
	UART_Close(1);
	UART_Close(2);
	UART_Close(3);
}	


////////////
// File Name : Uart_BasicTest
// File Description : Check Uart Operation by user's Input
// Input : user select
// Output : Txstring & Rxstring through the selected channel
// Version : 0.1

void Uart_BasicTest(void)
{
	u8 ch;
	static u8 acTestString[]="\n\nABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890->Tx string good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890->Tx string good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890->Tx string good!!!\r";

	ch = UART_Config();	
	
	if (ch == 0)
	{
		UART_Printf("\nDebug Channel will be channel 1\n");
		
		UART_InitDebugCh(1,115200);
		Delay(1000);
	}
	
	if( UART_Open2(ch) == SENDBREAK ) 
	{
		UART_Printf("\n\nSend Break Signal has been transfered\n");
		return; 	
	}
	
	UART_Printf("\n[Uart channel %d Tx Test]\n",ch);	
	UART_TxString(ch, acTestString);
	UART_Printf("\nTx Done~ \n");

	RxBufferClear();
		
	// UART Rx test 		
	UART_Printf("\n[Uart channel %d Rx Test]\n",ch);
	UART_Printf("1)Interrupt mode : After typing characters and pressing ENTER key through uart%d.\n",ch);
	UART_Printf("2)DMA mode : After typing characters and pressing ENTER key until %d through uart%d.\n",g_uUartRxDmaTc,ch);	
	UART_Printf(" Then, you will see those characters.\n");
	UART_RxString(ch);
	
	if(g_sRxStringResult.result_ok==TRUE)
	{
		UART_Printf("\nRx : %s \n",g_sRxStringResult.str);
		UART_Printf("\nRx OK~\n");
	}
	else
	{		
		UART_Printf("\nRx FAIL~\n");
	}

	// Test End
	UART_Printf("\nComplete UART test!!! \n");
	UART_Close(ch);
	
	if (ch == 0)
	{
		UART_Printf("\nDebug Channel will be channel 0 again\n");
	
		UART_InitDebugCh(0,115200);
		Delay(1000);	
	}
}	

////////////
// File Name : Uart_BasicTx_withPC
// File Description : Check Uart Operation by user's Input
// Input : 
// Output : 
// Version : 0.1

void Uart_BasicTx_withPC(void)
{
	u8 ch;
	static u8 acTestString[]="\n\nABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890->Tx string good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890->Tx string good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890->Tx string good!!!\r";

	ch = UART_Config();	
	
	if (ch == 0)
	{
		UART_Printf("\nDebug Channel will be channel 1\n");
		
		UART_Printf("\nPress a key through uart 1 when uart0 is ready to tx.\n");
		
		UART_InitDebugCh(1,115200);
		Delay(1000);
		
		UART_Getc();
	}
	else
	{
		UART_Printf("\nPress a key through uart 0 when uart%d is ready to tx.\n",ch);
		
		UART_Getc();
	}
	
	if( UART_Open2(ch) == SENDBREAK ) 
	{
		UART_Printf("\n\nSend Break Signal has been transfered\n");
		return; 	
	}
	
	UART_Printf("\n[Uart channel %d Tx Test]\n",ch);
	UART_Printf(" You will see TestString through uart%d.\n",ch);	
	UART_TxString(ch, acTestString);
	UART_Printf("\nTx Done~ \n");
	
	UART_Close(ch);
	
	if (ch == 0)
	{
		UART_Printf("\nDebug Channel will be channel 0 again\n");
	
		UART_InitDebugCh(0,115200);
		Delay(1000);
	}
}

////////////
// File Name : Uart_BasicRx_withPC
// File Description : Check Uart Operation by user's Input
// Input : 
// Output : 
// Version : 0.1

void Uart_BasicRx_withPC(void)
{
	u8 ch;

	RxBufferClear();
	
	ch = UART_Config();	
	
	if (ch == 0)
	{
		UART_Printf("\nDebug Channel will be channel 1\n");
		
		UART_Printf("\nPress a key through uart 1 when uart0 is ready to rx.\n");
		
		UART_InitDebugCh(1,115200);
		Delay(1000);
		
		UART_Getc();
	}
	else
	{
		UART_Printf("\nPress a key through uart 0 when uart%d is ready to rx.\n",ch);
		
		UART_Getc();
	}
	
	if( UART_Open2(ch) == SENDBREAK ) 
	{
		UART_Printf("\n\nSend Break Signal has been transfered\n");
		return; 	
	}
		
	UART_Printf("\n[Uart channel %d Rx Test]\n",ch);
	UART_Printf("1)Interrupt mode : After typing characters and pressing ENTER key through uart%d.\n",ch);
	UART_Printf("2)DMA mode : After typing characters and pressing ENTER key until %d through uart%d.\n",g_uUartRxDmaTc,ch);	
	UART_Printf(" Then, you will see those characters.\n");
	UART_RxString(ch);
	
	if(g_sRxStringResult.result_ok==TRUE)
	{
		UART_Printf("\nRx : %s \n",g_sRxStringResult.str);
		UART_Printf("\nRx OK~\n");
	}
	else
	{		
		UART_Printf("\nRx FAIL~\n");
	}
	
	UART_Close(ch);
	
	if (ch == 0)
	{
		UART_Printf("\nDebug Channel will be channel 0 again\n");
	
		UART_InitDebugCh(0,115200);
		Delay(1000);
	}
}


////////////
// File Name : Uart_BasicTx_Bd2Bd
// File Description : Check Uart Operation by user's Input
// Input : 
// Output : 
// Version : 0.1

void Uart_BasicTx_Bd2Bd(void)
{
	u8 ch;
	static u8 acTestString[]="\n\nABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890->Tx string good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890->Tx string good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890->Tx string good!!!\r";

	ch = UART_Config();	
	
	if (ch == 0)
	{
		UART_Printf("\nDebug Channel will be channel 1\n");
		
		UART_Printf("\nPress a key through uart 1 when uart0 is ready to tx.\n");
		
		UART_InitDebugCh(1,115200);
		Delay(1000);
		
		UART_Getc();
	}
	else
	{
		UART_Printf("\nPress a key through uart 0 when uart%d is ready to tx.\n",ch);
		
		UART_Getc();
	}
	
	if( UART_Open2(ch) == SENDBREAK ) 
	{
		UART_Printf("\n\nSend Break Signal has been transfered\n");
		return; 	
	}
	
	UART_Printf("\n[Uart channel %d Tx Test]\n",ch);	
	UART_TxString(ch, acTestString);
	UART_Printf("\nTx Done~ \n");
	
	UART_Close(ch);
	
	if (ch == 0)
	{
		UART_Printf("\nDebug Channel will be channel 0 again\n");
		
		UART_Printf("\nPress a key through uart0 after you connect uart0 with PC.\n");
	
		UART_InitDebugCh(0,115200);
		Delay(1000);
		
		UART_Getc();	
	}
}

////////////
// File Name : Uart_BasicRx_Bd2Bd
// File Description : Check Uart Operation by user's Input
// Input : 
// Output : 
// Version : 0.1

void Uart_BasicRx_Bd2Bd(void)
{
	u8 ch;

	RxBufferClear();
	
	ch = UART_Config();	
	
	if (ch == 0)
	{
		UART_Printf("\nDebug Channel will be channel 1\n");
		
		UART_Printf("\nPress a key through uart 1 when uart0 is ready to rx.\n");
		
		UART_InitDebugCh(1,115200);
		Delay(1000);
		
		UART_Getc();
	}
	else
	{
		UART_Printf("\nPress a key through uart 0 when uart%d is ready to rx.\n",ch);
		
		UART_Getc();
	}
	
	if( UART_Open2(ch) == SENDBREAK ) 
	{
		UART_Printf("\n\nSend Break Signal has been transfered\n");
		return; 	
	}
		
	UART_Printf("\n[Uart channel %d Rx Test]\n",ch);
	UART_RxString(ch);
	
	if(g_sRxStringResult.result_ok==TRUE)
	{
		UART_Printf("\nRx : %s \n",g_sRxStringResult.str);
		UART_Printf("\nRx OK~\n");
	}
	else
	{		
		UART_Printf("\nRx FAIL~\n");
	}
	
	UART_Close(ch);
	
	if (ch == 0)
	{
		UART_Printf("\nDebug Channel will be channel 0 again\n");
		
		UART_Printf("\nPress a key through uart0 after you connect uart0 with PC.\n");
	
		UART_InitDebugCh(0,115200);
		Delay(1000);
		
		UART_Getc();
	}
}


////////////
// File Name : Uart_PollingTx
// File Description : Check Uart Polling Operation through the channel 1
// Input : NONE
// Output : Txstring
// Version : 0.1
void Uart_PollingTx(void)
{
	s8 sCh;	
	u8* pUartTxStr;
	u8 acTestPString[]="ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890->UART Tx Polling test is good!!!\r";

	UART_Printf("Which channel do you want to check(0~3)[D=1]?\n");
	UART_Printf("Choose : ");
	sCh = UART_GetIntNum();
	UART_Putc('\n');
	if (sCh == -1)
		sCh = 1;
	
	if (sCh == 0)
	{
		UART_Printf("\nDebug Channel will be channel 1\n");
		
		UART_Printf("\nPress a key through uart 1 when uart0 is ready to tx.\n");
		
		UART_InitDebugCh(1,115200);
		Delay(1000);
		
		UART_Getc();
	}
	else
	{
		UART_Printf("\nPress a key through uart 0 when uart%d is ready to tx.\n",sCh);
		
		UART_Getc();
	}
	
	UART_Putc('\n');
	
	UART_SetConfig(sCh,0,1,1,4,1,2,1,115200,1,1,1,1);	//UARTCLKSRC=PCLK
	UART_Open2(sCh);

	pUartTxStr = acTestPString;

	while(1)
	{
		if(UART_RdUTRSTAT(sCh)&0x4) 
		{
			UART_WrUtxh(sCh,*pUartTxStr);

			if(*pUartTxStr == '\r') 
				break;

			pUartTxStr++;
		}
	}
		
	UART_Printf("\nPress Enter when you complete to check Tx\n");
	UART_Getc();
	
	UART_Close(sCh);
	
	if (sCh == 0)
	{
		UART_Printf("\nDebug Channel will be channel 0 again\n");
		
		UART_Printf("\nPress a key through uart0 after you connect uart0 with PC.\n");
	
		UART_InitDebugCh(0,115200);
		Delay(1000);
		
		UART_Getc();	
	}

}


// File Name : Uart_PollingRx
// File Description : Check Uart Polling Operation through the channel 1 (AFC mode)
// Input : Typing any character throught the channel 1
// Output : Rxstring which has got throught the channel 1
// Version : 0.1

void Uart_PollingRx(void)
{
	s8 sCh;
	volatile u8 *pUartRxStr;

	UART_Printf("Which channel do you want to check(0~3)[D=1]?\n");
	UART_Printf("Choose : ");
	sCh = UART_GetIntNum();
	UART_Putc('\n');
	if (sCh == -1)
		sCh = 1;
	
	if (sCh == 0)
	{
		UART_Printf("\nDebug Channel will be channel 1\n");
		
		UART_Printf("\nPress a key through uart 1 when uart0 is ready to rx.\n");
		
		UART_InitDebugCh(1,115200);
		Delay(1000);
		
		UART_Getc();
	}
	else
	{
		UART_Printf("\nPress a key through uart 0 when uart%d is ready to rx.\n",sCh);
		
		UART_Getc();
	}	

	UART_Putc('\n');
	UART_Printf("Input any character through the channel %d\n",sCh);
	UART_Printf("Press Enter key will stop test & show Rxstring\n");
	
	UART_SetConfig(sCh,0,1,1,4,1,2,1,115200,1,1,1,1);	//UARTCLKSRC=PCLK
	UART_Open2(sCh);

	pUartRxStr=(u8 *)(UART_RX_BUF);
	RxBufferClear();
	
	while(1) 
	{
		if(UART_RdUTRSTAT(sCh)&0x1) 
		{

			*pUartRxStr = UART_RdUrxh(sCh);
			if(*pUartRxStr == '\r')
				break;
			
			pUartRxStr++;
		}
	}

	pUartRxStr =(u8 *)(UART_RX_BUF);

	UART_Printf("##Rx :  ");
	while(1) 
	{
		UART_Printf("%c",*pUartRxStr);
		if(*pUartRxStr == '\r')
			break;
		
		pUartRxStr++;
	}
	
	UART_Close(sCh);
	
	if (sCh == 0)
	{
		UART_Printf("\nDebug Channel will be channel 0 again\n");
		
		UART_Printf("\nPress a key through uart0 after you connect uart0 with PC.\n");
	
		UART_InitDebugCh(0,115200);
		Delay(1000);
		
		UART_Getc();
	}
}


// File Name : Uart_IrDATx
// File Description : Check Uart IrDA Operation(SIR) through the channel 2/3
// Input : NONE
// Output : Txstrings which has got throught the channel 2/3
// Version : 0.1
void Uart_IrDATx(void)
{
	s8 sCh;	
	static u8 acTestIString[]="\n\nABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890->UART IrDA Tx test is good!!!!\r";
		
	UART_Printf("Which channel do you want to check(0~3)[D=2]?\n");
	UART_Printf(" (note! uart0/1 : using wire,  uart2/3 : using IrDA module)\n");
	UART_Printf("Choose : ");
	sCh = UART_GetIntNum();
	UART_Putc('\n');
	if (sCh == -1)
		sCh = 2;
	
	if (sCh == 0)
	{
		UART_Printf("\nDebug Channel will be channel 1\n");
		
		UART_Printf("\nPress a key through uart 1 when uart0-IrDA is ready to tx.\n");
		
		UART_InitDebugCh(1,115200);
		Delay(1000);
		
		UART_Getc();
	}
	else
	{
		UART_Printf("\nPress a key through uart 0 when uart%d-IrDA is ready to tx.\n",sCh);
		
		UART_Getc();
	}
	
	UART_Putc('\n');
	
	sCh = UART_SetConfig(sCh,0,1,1,4,1,2,2,115200,1,1,1,1);
	UART_Open2(sCh);

	UART_Printf("\n[Uart channel %d Tx Test]\n",sCh);	
	UART_TxString(sCh, acTestIString);
	UART_Printf("\nTx Done~ \n");		

	UART_Close(sCh);
	
	if (sCh == 0)
	{
		UART_Printf("\nDebug Channel will be channel 0 again\n");
		
		UART_Printf("\nPress a key through uart0 after you connect uart0 with PC.\n");
	
		UART_InitDebugCh(0,115200);
		Delay(1000);
		
		UART_Getc();
	}
}

// File Name : Uart_IrDARx
// File Description : Check Uart IrDA Operation (SIR) through the channel 2/3
// Input : Type any character throught the channel 2
// Output : Rxstrings which has got throught the channel 2/3
// Version : 0.1
void Uart_IrDARx(void)
{
	s8 sCh;	
	
	UART_Printf("Which channel do you want to check(0~3)[D=2]?\n");
	UART_Printf(" (note! uart0/1 : using wire,  uart2/3 : using IrDA module)\n");
	UART_Printf("Choose : ");
	sCh = UART_GetIntNum();
	UART_Putc('\n');
	if (sCh == -1)
		sCh = 2;
	
	if (sCh == 0)
	{
		UART_Printf("\nDebug Channel will be channel 1\n");
		
		UART_Printf("\nPress a key through uart 1 when uart0-IrDA is ready to rx.\n");
		
		UART_InitDebugCh(1,115200);
		Delay(1000);
		
		UART_Getc();
	}
	else
	{
		UART_Printf("\nPress a key through uart 0 when uart%d-IrDA is ready to rx.\n",sCh);
		
		UART_Getc();
	}	

	UART_Putc('\n');

	UART_SetConfig(sCh,0,1,1,4,1,2,2,115200,1,1,1,1);
	UART_Open2(sCh);

	UART_RxString(sCh);
	if(g_sRxStringResult.result_ok==TRUE)
	{
		UART_Printf("\nRx : %s \n",g_sRxStringResult.str);
		UART_Printf("\nRx OK~\n");
	}
	else
	{		
		UART_Printf("\nRx FAIL~\n");
	}

	UART_Close(sCh);
	
	if (sCh == 0)
	{
		UART_Printf("\nDebug Channel will be channel 0 again\n");
		
		UART_Printf("\nPress a key through uart0 after you connect uart0 with PC.\n");
	
		UART_InitDebugCh(0,115200);
		Delay(1000);
		
		UART_Getc();	
	}		
}



// File Name : Uart_SendBreak
// File Description : Check Uart SendBreak signal 
// Input : NONE
// Output : NONE
// Version : 0.1
void Uart_SendBreak(void)
{
	s32 sCh;
	u8 cKey;	

	UART_Printf("Which channel do you want to check(0~3)[D=1]?\n");
	UART_Printf("Choose : ");
	sCh = UART_GetIntNum();
	UART_Putc('\n');
	if (sCh == -1)
		sCh = 1;
	else if (sCh == 0)
	{
		UART_Printf("\nDebug Channel will be channel 1\n");
		
		UART_InitDebugCh(1,115200);
		Delay(1000);	
	}	

	UART_SetConfig(sCh,0,1,1,4,1,1,1,115200,1,1,1,1);    //ch1, Interrupt, no FIFO
	UART_Open2(sCh);
	
	UART_Printf("Press any key then you can have a SendBreak signal through the channel %d, check it out!\n",sCh);	
	UART_Printf("If You enter 'x' , Test will be finished!\n");

	while(1)
	{

			cKey = UART_Getc();

			if ( cKey == 'x')
			{
				break;
			}

			UART_SendBreak(sCh);
		
	}

	UART_Close(sCh);
	
	if (sCh == 0)
	{
		UART_Printf("\nDebug Channel will be channel 0 again\n");
	
		UART_InitDebugCh(0,115200);
		Delay(1000);	
	}
	
}


// File Name : Uart_LoopBack
// File Description : Check Uart LoopBack mode 
// Input : NONE
// Output : NONE
// Version : 0.1
void Uart_LoopBack(void)
{
	u32 Txdone;
	u32 Rxdone;
	s32 sCh;
	volatile u8 *pUartRxStr;
	volatile u8* pTestLBString="ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx LoopBack test is good!!!\r";

	pUartRxStr=(u8 *)(UART_RX_BUF);
	RxBufferClear();	

	UART_Printf("Which channel do you want to check(0~3)[D=1]?\n");
	UART_Printf("Choose : ");
	sCh = UART_GetIntNum();
	UART_Putc('\n');
	if (sCh == -1)
		sCh = 1;
	else if (sCh == 0)
	{
		UART_Printf("\nDebug Channel will be channel 1\n");
		
		UART_InitDebugCh(1,115200);
		Delay(1000);
	}	

	UART_Printf("\n");
	
	UART_SetConfig(sCh,2,1,1,4,1,1,1,115200,1,1,1,1);    //Interrupt, no FIFO
	UART_Open2(sCh);
	Delay(1000);

	Txdone = 0;
	Rxdone = 0;
	
	while(!(Txdone&Rxdone))
	{
		if(!Txdone)
		{
			if(UART_RdUTRSTAT(sCh)&0x4) 
			{
				UART_WrUtxh(sCh,*pTestLBString);

				if(*pTestLBString == '\r') 
					Txdone = 1;

				pTestLBString++;
			}
		}

		if(!Rxdone)
		{
			if(UART_RdUTRSTAT(sCh)&0x1) 
			{
				*pUartRxStr = UART_RdUrxh(sCh);

				if(*pUartRxStr == '\r')
				{
					*(pUartRxStr+1) = NULL;
					Rxdone = 1;
						
				}					
					pUartRxStr++;
			}
		}

	}

	pUartRxStr =(u8 *)(UART_RX_BUF);
	
	UART_Printf("\n%s\n", pUartRxStr);
	
	UART_Close(sCh);
	
	if (sCh == 0)
	{
		UART_Printf("\nDebug Channel will be channel 0 again\n");
	
		UART_InitDebugCh(0,115200);
		Delay(1000);	
	}

}


// File Name : Uart_CheckRtsTriggerLevel
// File Description : Check if nRTS changes depending on trigger level setting(AFC mode)
// Input : 
// Output : 
// Version : 0.1

void Uart_CheckRtsTriggerLevel(void)
{
	s8 sCh;
	u8 uRtsTrigLevel;

	UART_Printf("Which channel do you want to check?\n");
	UART_Printf("[0 ~ 1] :");
	sCh = UART_GetIntNum();
	if ((sCh != 0)&&(sCh != 1))
		sCh = 1;
	else if (sCh == 0)
	{
		UART_Printf("\nDebug Channel will be channel 1\n");
		
		UART_InitDebugCh(1,115200);
		Delay(1000);
	}
	
	UART_Printf("Select nRTS trigger level(byte)\n");
	UART_Printf(" 1. 63[D]   2. 56   3. 48   4. 40   5. 32   6. 24   7. 16   8. 8\n Choose : ");
	uRtsTrigLevel = (u8)UART_GetIntNum();
	
	if ( (uRtsTrigLevel<1)||(uRtsTrigLevel>8) )	
		uRtsTrigLevel = 1; // default 63 byte
		
	UART_SetConfig(sCh,0,1,1,4,1,2,1,115200,1,2,2,uRtsTrigLevel);
	UART_Open2(sCh);
	
	UART_Printf("\nCheck if nRTS signal of uart%d is low\n", sCh);
	UART_Printf("After checking, press a key through a debugging channel\n");
	UART_Getc();
	
	UART_Printf("\nEnter a key through uart%d until trigger level...\n",sCh);
	UART_Printf("And then, check if nRTS signal of uart%d changes from low to high\n", sCh);
	UART_Printf("After checking, press a key through a debugging channel\n");
	UART_Getc();
	
	UART_RdUrxh(sCh);
	
	UART_Printf("\nCheck if nRTS signal of uart%d changes from high to low\n", sCh);
	UART_Printf("After checking, press a key through a debugging channel\n");
	UART_Getc();	
	
	while(UART_RdUFSTAT(sCh)&0x7f) 
		UART_RdUrxh(sCh);
		
	UART_Printf("\nTest End~");
	
	UART_Close(sCh);
	
	if (sCh == 0)
	{
		UART_Printf("\nDebug Channel will be channel 0 again\n");
	
		UART_InitDebugCh(0,115200);
		Delay(1000);	
	}
}


// File Name : Uart_CheckOverrunError
// File Description : 
// Input : Typing any character throught the channel 1
// Output : Rxstring which has got throught the channel 1
// Version : 0.1

void Uart_CheckOverrunError(void)
{
	s8 sCh;
	u8 uFifoMode;
	u8 *pucRxData=(u8 *)UART_RX_BUF;
	
	UART_Printf("Which channel do you want to check(0~3)[D=1]?\n");
	UART_Printf("Choose : ");
	sCh = UART_GetIntNum();
	UART_Putc('\n');
	if (sCh == -1)
		sCh = 1;
	else if (sCh == 0)
	{
		UART_Printf("\nDebug Channel will be channel 1\n");
		
		UART_InitDebugCh(1,115200);
		Delay(1000);
	}	
	
	UART_Printf("\n 1. FIFO Disable[D]   2. FIFO Enable\n Choose : ");
	uFifoMode=UART_GetIntNum();
	if((uFifoMode<1)||(uFifoMode>2))
		uFifoMode = 1;
	
	UART_SetConfig(sCh,0,1,1,4,1,2,1,115200,1,uFifoMode,1,1);	//UARTCLKSRC=PCLK
	UART_Open2(sCh);

	INTC_Enable(NUM_UART0+sCh);
	
	//unmask error-interrupt temporarily for test
	Outp32((0x7f005038+0x400*sCh),Inp8(0x7f005038+0x400*sCh)&~0x02);
	
	if(uFifoMode == 1)
	{
		UART_Printf("\nInput 2 characters through the channel %d!!!\n",sCh);
	}
	else
	{
		UART_Printf("\nInput 65 characters(RxFifo+1) through the channel %d!!!\n",sCh);
	}
	
	UART_Printf("\nAfter overrun error interrupt occurs, press a key through debugging channel\n");
	
	UART_Getc();
	
	while(UART_RdUFSTAT(sCh)&0x7f) 
		*pucRxData++=UART_RdUrxh(sCh);
	
	UART_Close(sCh);
	
	if (sCh == 0)
	{
		UART_Printf("\nDebug Channel will be channel 0 again\n");
	
		UART_InitDebugCh(0,115200);
		Delay(1000);	
	}
}


void Uart_SpeedPollingTx(void)
{
	s32 sCh;	
	s32 sBrate;
	//u8 ucCnt='0';	//for testing with PC
	u8 ucCnt=0;
	s32 uEpll_M, uEpll_P, uEpll_S, uEpll_K;
	s32 sTxTestCnt,sTotalTxCnt=0;

	////
	//select a test channel
	//
	UART_Printf("Which channel do you want to check(0~3)[D=1]?\n");
	UART_Printf("Choose : ");
	sCh = UART_GetIntNum();
	UART_Putc('\n');
	if (sCh == -1)
		sCh = 1;
	else if (sCh == 0)
	{
		UART_Printf("\nDebug Channel will be channel 1\n");
		
		UART_Printf("\nPress a key through uart 1 when uart1 is connected with PC.\n");
		
		UART_InitDebugCh(1,115200);
		Delay(1000);
		
		UART_Getc();
	}
	UART_Putc('\n');
	//
	////
	
	////
	//select a test baud-rate	
	//
	UART_Printf("Tx BaudRate (3Mbps[D]) : ");
	sBrate = UART_GetIntNum();
	if (sBrate == -1)
		sBrate = 3000000;
	UART_Putc('\n');
	//
	////	
	
	////
	//set EPLL in Syscon for UART	
	//
	UART_Printf("Now, Fepllout is 192Mhz, and Fuartclk is 192Mhz\n");
	UART_Printf("If you want to change, press 'y' key, otherwise, press 'Enter key\n");
	
	if(UART_Getc()=='y')
	{
		UART_Printf("M of EPLL (32[D]) : ");
		uEpll_M = UART_GetIntNum();
		if (uEpll_M == -1)
			uEpll_M = 32;
		UART_Putc('\n');
		
		UART_Printf("P of EPLL (1[D]) : ");
		uEpll_P = UART_GetIntNum();
		if (uEpll_P == -1)
			uEpll_P = 1;
		UART_Putc('\n');
		
		UART_Printf("S of EPLL (1[D]) : ");
		uEpll_S = UART_GetIntNum();
		if (uEpll_S == -1)
			uEpll_S = 1;
		UART_Putc('\n');
		
		UART_Printf("K of EPLL (0[D]) : ");
		uEpll_K = UART_GetIntNum();
		if (uEpll_K == -1)
			uEpll_K = 0;
		UART_Putc('\n');		
	}
	
	else
	{
		uEpll_M = 32;
		uEpll_P = 1;
		uEpll_S = 1;
		uEpll_K = 0;
	}
	
	UART_SetEpll_forUART(uEpll_M, uEpll_P, uEpll_S, uEpll_K);
	//
	////
	
	
	////
	// set tx test length
	//
	UART_Printf("\nEnter Tx Test Length (1:1M[D]  2:10M  3:100M  4:1G)\n Choose : ");
	sTxTestCnt = UART_GetIntNum();
	switch(sTxTestCnt)
	{
		case 2:
			sTxTestCnt = 0x00a00000;
			break;
		case 3:
			sTxTestCnt = 0x06400000;
			break;
		case 4:
			sTxTestCnt = 0x40000000;
			break;
		default:
			sTxTestCnt = 0x00100000;
			break;
	}
	UART_Putc('\n');
	//
	////
	
	
	////
	// open uart
	//
	sCh = UART_SetConfig_2(sCh,0,1,1,4,1,sBrate,1,2,1,1);	//fifo mode
	UART_Open2(sCh);
	//
	////
	
	////
	// change ubrdiv and udivslot reg. by force
	{
		const u32 aSlotTable[16] = {0x0000, 0x0080, 0x0808, 0x0888, 0x2222, 0x4924, 0x4a52, 0x54aa,
				     		0x5555, 0xd555, 0xd5d5, 0xddd5, 0xdddd, 0xdfdd, 0xdfdf, 0xffdf};
		s32 sUbrDiv, sNumOfUdivSlot;
		extern volatile u32 g_uOpClock;
		u8 ucCnt;
		u16 usUbrdiv, usUdivSlot;
		
		UART_Printf("If you want to change ubrdiv & udivslot by force, press 'y' key, otherwise, press 'Enter key\n");
		if(UART_Getc()=='y')
		{
			UART_Printf(" UBRDIV%d : ",sCh);
			sUbrDiv = UART_GetIntNum();
			if (sUbrDiv != -1)
				Outp16((0x7f005028+sCh*0x400), (u16)sUbrDiv);
			
			UART_Printf(" # of 1's in UDIVSLOT%d : ",sCh);
			sNumOfUdivSlot = UART_GetIntNum();
			if (sNumOfUdivSlot != -1)
				Outp16((0x7f00502C+sCh*0x400), (u16)aSlotTable[sNumOfUdivSlot]);
			UART_Putc('\n');
		}
	
		usUbrdiv = Inp16(0x7f005028+sCh*0x400);
		usUdivSlot = Inp16(0x7f00502C+sCh*0x400);
		
		sNumOfUdivSlot = 0;
		
		for(ucCnt=0;ucCnt<16;ucCnt++)
		{
			if((usUdivSlot>>ucCnt)&0x1)
				sNumOfUdivSlot++;
		}
		
		UART_Printf("\n Ideal BaudRate=%d, Real BaudRate=%.1f\n",
			sBrate,(float)g_uOpClock/16/(usUbrdiv+1+(float)sNumOfUdivSlot/16));
		UART_Printf(" (Fuartclkin=%.4fMhz, UBRDIVn=%d, # of 1's in UDIVSLOTn=%d)\n\n",
			(float)g_uOpClock/1000000, usUbrdiv, sNumOfUdivSlot);
	}
	//
	////


	////
	// tx until tx-count reaches to test-count
	//
	UART_Printf("\nPress a key through debugging channel when uart%d is ready to tx.\n",sCh);	
	UART_Getc();
	UART_Printf("Tx has started....\n");
	while(!UART_GetKey())
	{
		if(!(UART_RdUFSTAT(sCh)&0x4000)) 		//if TX Fifo is not Full
		{
			UART_WrUtxh(sCh,ucCnt);
			
			ucCnt++;
			sTotalTxCnt++;
			
			if(sTotalTxCnt == sTxTestCnt)
				break;
			
			//for test with PC
			/*
			if(ucCnt == ':')	//ascii code order => 0123456789:
				ucCnt='0';	
			*/
		}
	}
	//
	////

	UART_Close(sCh);
	
	UART_Printf("\n\n0x%08x bytes are transmitted.\n", sTotalTxCnt);
	
	if (sCh == 0)
	{
		UART_Printf("\nDebug Channel will be channel 0 again\n");
		
		UART_Printf("\nPress a key through uart0 after you connect uart0 with PC.\n");
	
		UART_InitDebugCh(0,115200);
		Delay(1000);
		
		UART_Getc();
	}
}


void Uart_SpeedPollingRx(void)
{
	s32 sCh;	
	s32 sBrate;
	//u8 ucCnt='0';	//for testing with PC
	u8 ucCnt=0;
	u8 ucRdData;
	s32 uEpll_M, uEpll_P, uEpll_S, uEpll_K;
	s32 sRxTestCnt,sTotalRxCnt=0;

	////
	//select a test channel
	//
	UART_Printf("Which channel do you want to check(0~3)[D=1]?\n");
	UART_Printf("Choose : ");
	sCh = UART_GetIntNum();
	UART_Putc('\n');
	if (sCh == -1)
		sCh = 1;
	else if (sCh == 0)
	{
		UART_Printf("\nDebug Channel will be channel 1\n");		
		
		UART_Printf("\nPress a key through uart 1 when uart1 is connected with PC.\n");
		
		UART_InitDebugCh(1,115200);
		Delay(1000);
		
		UART_Getc();
	}
	UART_Putc('\n');
	//
	////
	
	////
	//select a test baud-rate	
	//
	UART_Printf("Rx BaudRate (3Mbps[D]) : ");
	sBrate = UART_GetIntNum();
	if (sBrate == -1)
		sBrate = 3000000;
	UART_Putc('\n');
	//
	////	
	
	////
	//set EPLL in Syscon for UART	
	//
	UART_Printf("Now, Fepllout is 192Mhz, and Fuartclk is 192Mhz\n");
	UART_Printf("If you want to change, press 'y' key, otherwise, press 'Enter key\n");
	
	if(UART_Getc()=='y')
	{
		UART_Printf("M of EPLL (32[D]) : ");
		uEpll_M = UART_GetIntNum();
		if (uEpll_M == -1)
			uEpll_M = 32;
		UART_Putc('\n');
		
		UART_Printf("P of EPLL (1[D]) : ");
		uEpll_P = UART_GetIntNum();
		if (uEpll_P == -1)
			uEpll_P = 1;
		UART_Putc('\n');
		
		UART_Printf("S of EPLL (1[D]) : ");
		uEpll_S = UART_GetIntNum();
		if (uEpll_S == -1)
			uEpll_S = 1;
		UART_Putc('\n');
		
		UART_Printf("K of EPLL (0[D]) : ");
		uEpll_K = UART_GetIntNum();
		if (uEpll_K == -1)
			uEpll_K = 0;
		UART_Putc('\n');		
	}
	
	else
	{
		uEpll_M = 32;
		uEpll_P = 1;
		uEpll_S = 1;
		uEpll_K = 0;
	}
	
	UART_SetEpll_forUART(uEpll_M, uEpll_P, uEpll_S, uEpll_K);
	//
	////
	
	////
	// set rx test length
	//
	UART_Printf("\nEnter Rx Test Length (1:1M[D]  2:10M  3:100M  4:1G)\n Choose : ");
	sRxTestCnt = UART_GetIntNum();
	switch(sRxTestCnt)
	{
		case 2:
			sRxTestCnt = 0x00a00000;
			break;
		case 3:
			sRxTestCnt = 0x06400000;
			break;
		case 4:
			sRxTestCnt = 0x40000000;
			break;
		default:
			sRxTestCnt = 0x00100000;
			break;
	}
	UART_Putc('\n');
	//
	////
	
	////
	// open uart
	//
	sCh = UART_SetConfig_2(sCh,0,1,1,4,1,sBrate,1,2,1,1);	//fifo mode
	UART_Open2(sCh);
	//
	////
	
	////
	// change ubrdiv and udivslot reg. by force
	{
		const u32 aSlotTable[16] = {0x0000, 0x0080, 0x0808, 0x0888, 0x2222, 0x4924, 0x4a52, 0x54aa,
				     		0x5555, 0xd555, 0xd5d5, 0xddd5, 0xdddd, 0xdfdd, 0xdfdf, 0xffdf};
		s32 sUbrDiv, sNumOfUdivSlot;
		extern volatile u32 g_uOpClock;
		u8 ucCnt;
		u16 usUbrdiv, usUdivSlot;
		
		UART_Printf("If you want to change ubrdiv & udivslot by force, press 'y' key, otherwise, press 'Enter key\n");
		if(UART_Getc()=='y')
		{
			UART_Printf(" UBRDIV%d : ",sCh);
			sUbrDiv = UART_GetIntNum();
			if (sUbrDiv != -1)
				Outp16((0x7f005028+sCh*0x400), (u16)sUbrDiv);
			
			UART_Printf(" # of 1's in UDIVSLOT%d : ",sCh);
			sNumOfUdivSlot = UART_GetIntNum();
			if (sNumOfUdivSlot != -1)
				Outp16((0x7f00502C+sCh*0x400), (u16)aSlotTable[sNumOfUdivSlot]);
			UART_Putc('\n');
		}
	
		usUbrdiv = Inp16(0x7f005028+sCh*0x400);
		usUdivSlot = Inp16(0x7f00502C+sCh*0x400);
		
		sNumOfUdivSlot = 0;
		
		for(ucCnt=0;ucCnt<16;ucCnt++)
		{
			if((usUdivSlot>>ucCnt)&0x1)
				sNumOfUdivSlot++;
		}
		
		UART_Printf("\n Ideal BaudRate=%d, Real BaudRate=%.1f\n",
			sBrate,(float)g_uOpClock/16/(usUbrdiv+1+(float)sNumOfUdivSlot/16));
		UART_Printf(" (Fuartclkin=%.4fMhz, UBRDIVn=%d, # of 1's in UDIVSLOTn=%d)\n\n",
			(float)g_uOpClock/1000000, usUbrdiv, sNumOfUdivSlot);
	}
	//
	////


	////
	// rx until rx-count reaches to test-count
	UART_Printf("\nPress a key through debugging channel when uart%d is ready to rx.\n",sCh);	
	UART_Getc();
	UART_Printf("Rx has started....\n");
	while(!UART_GetKey()) 
	{
		if(UART_RdUTRSTAT(sCh)&0x1)	//receive buffer has data
		{
			ucRdData = UART_RdUrxh(sCh);
			sTotalRxCnt++;
			
			if(ucCnt != ucRdData)	
			{
				UART_Printf("\nTest is failed!!!\n");
				UART_Printf("(Expected data is 0x%02x, Received data is 0x%02x)\n",ucCnt, ucRdData);
				break;
			}
			
			if((sTotalRxCnt%0x80000)==0)	//every 512k
				UART_Putc('.');
			
			if(sTotalRxCnt == sRxTestCnt)
			{
				UART_Printf("\nTest is OK!!!\n");
				break;
			}
			
			ucCnt++;
			
			//for test with PC
			/*
			if(ucCnt == ':')	//ascii code order => 0123456789:
				ucCnt='0';
			*/
		}
	}
	//
	////

	UART_Close(sCh);
	
	UART_Printf("\n\n0x%08x bytes are received.\n", sTotalRxCnt);
	
	if (sCh == 0)
	{
		UART_Printf("\nDebug Channel will be channel 0 again\n");
		
		UART_Printf("\nPress a key through uart0 after you connect uart0 with PC.\n");
	
		UART_InitDebugCh(0,115200);
		Delay(1000);
		
		UART_Getc();
	}
}

////////////
// File Name : Uart_GpsModuleTest
// File Description : Check Uart i/f with SirfIII GPS Module
// Input : 
// Output : 
// Version : 0.1

void Uart_GpsModuleTest(void)
{
	s8 sCh=2;
	u8 i=0;	
	
	UART_SetConfig(sCh,0,1,1,4,1,2,1,9600,1,1,1,1);	//UARTCLKSRC=PCLK
	UART_Open2(sCh);
	
	Delay(1000);
	
	UART_Printf("\nReceive Data from SiRF3 Module \n");

	while(i<12)
	{
		i++;
		UART_RxString(sCh);
		if(g_sRxStringResult.result_ok==TRUE)
		{
			UART_Printf("\nRx : %s \n",g_sRxStringResult.str);
		}
		else
		{			
			UART_Printf("\nRx FAIL~\n");
		}		
	}
	
	printf("\nRx Done~ \n");
	printf("\nComplete SiRF3 test!!! \n");
	UART_Close(sCh);
	
}

// File Name : RxBufferClear
// File Description : Make Buffer as 0 for Polling test
// Input : NONE
// Output : NONE
// Version : 0.1

void RxBufferClear(void)
{
	u32 uCnt;
	u32 *ptrBuf;

	ptrBuf=(u32 *)(UART_RX_BUF);

	for(uCnt=0;uCnt<20000;uCnt+=1)
		*(ptrBuf+uCnt) = 0xffffffff;
	
}

// File Name : Uart_SpeedTx
// File Description : Check Uart Speed Operation through the channel 1 (AFC mode)
// Input : NONE
// Output : Txstring on channel 1
// Version : 0.1

void Uart_SpeedTx(void)
{
	s32 sCh;	
	s32 sBrate=0;
	static u8 acTestPString[]="ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Speed test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is end\
ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Speed test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is end\
ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Speed test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is end\
ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Speed test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is end\
ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Speed test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is end\
ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Speed test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is end\
ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Speed test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is end\
ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Speed test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is end\
ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Speed test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is end\
ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Speed test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is end\
ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Speed test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is end\
ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Speed test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is end\
ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Speed test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is end\
ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Speed test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is end\
ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Speed test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is end\
ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Speed test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is end\
ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Speed test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is end\
ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Speed test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling  end\r";
//	static u8 acTestPString[]="!!\r";
//	static u8 acTestPString[]="ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678901234567890->UART Tx Polling test is good!!!\r";
//	static u8 acTestPString[]="UUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU!!!!\r ";

	

	UART_Printf("Which channel do you want to check(0~3)[D=1]?\n");
	UART_Printf("Choose : ");
	sCh = UART_GetIntNum();
	UART_Putc('\n');
	if (sCh == -1)
		sCh = 1;
	else if (sCh == 0)
	{
		UART_Printf("\nDebug Channel will be channel 1\n");
		
		UART_InitDebugCh(1,115200);
		Delay(1000);
	}	

	UART_Printf("\n");

	UART_Printf("How much BaudRate do you want to use as a Tx?\n");
	UART_Printf("BaudRate (ex 9600, 115200[D], 921600) : ");
	sBrate = UART_GetIntNum();
	if (sBrate == -1)
		sBrate = 115200;

	UART_SetConfig(sCh,0,1,1,4,3,1,1,sBrate,1,4,1,1);	
//	ch = UART_SetConfig(1,0,1,1,4,3,1,1,sBrate,1,4,2,7);
	UART_Open2(sCh);

	UART_Printf("\n Get ready to Rx and tPress Enter (bcuz of GPIO setting).  It will start to Tx !!!\n"); 
	UART_Getc();

 	UART_TxString(sCh,acTestPString);

	UART_Printf("\nPress Enter when you complete to check Rx\n");
	UART_Getc();

	UART_Close(sCh);
	
	if (sCh == 0)
	{
		UART_Printf("\nDebug Channel will be channel 0 again\n");
	
		UART_InitDebugCh(0,115200);
		Delay(1000);	
	}
}


// File Name : Uart_SpeedRx
// File Description : Check Uart Speed Operation through the channel 1 (AFC mode)
// Input : Type any character throught the channel 1
// Output : Rxstrings which has got throught the channel 1
// Version : 0.1

void Uart_SpeedRx(void)
{
	s32 sCh;	
	s32 sBrate=0;
	
	UART_Printf("Which channel do you want to check(0~3)[D=1]?\n");
	UART_Printf("Choose : ");
	sCh = UART_GetIntNum();
	UART_Putc('\n');
	if (sCh == -1)
		sCh = 1;
	else if (sCh == 0)
	{
		UART_Printf("\nDebug Channel will be channel 1\n");
		
		UART_InitDebugCh(1,115200);
		Delay(1000);
	}	

	UART_Printf("\n");

	UART_Printf("How much BaudRate do you want to use as a Rx?\n");
	UART_Printf("BaudRate (ex 9600, 115200[D], 921600) : ");
	sBrate = UART_GetIntNum();
	if (sBrate == -1)
		sBrate = 115200;
	UART_Printf("\n");

	
	UART_SetConfig(sCh,0,1,1,4,3,1,1,sBrate,2,2,1,1);  //DMA mode
//	sCh = UART_SetConfig(sCh,0,1,1,4,3,1,1,sBrate,1,1,1,1);  //interrupt mode
//	ch = UART_SetConfig(1,0,1,1,4,3,1,1,sBrate,1,4,2,7);
	UART_Open2(sCh);

	RxBufferClear();


	UART_Printf("\n[Uart channel %d Rx Test]\n",sCh);
	UART_Printf("Input any character through the channel %d\n",sCh);
	UART_Printf("Press Enter key will stop test & show Rxstring\n");
	UART_RxString(sCh);
	if(g_sRxStringResult.result_ok==TRUE)
	{
		UART_Printf("\nRx : %s \n",g_sRxStringResult.str);
		UART_Printf("\nRx OK~\n");
	}
	else
	{		
		UART_Printf("\nRx FAIL~\n");
	}

	// Test End
	UART_Printf("\nComplete UART test!!! \n");
	UART_Close(sCh);
	
	if (sCh == 0)
	{
		UART_Printf("\nDebug Channel will be channel 0 again\n");
	
		UART_InitDebugCh(0,115200);
		Delay(1000);	
	}
			
}

void TxBufferInit(void)
{
	u32 uCnt;
 	u32 *pRec_buf = (u32 *)UART_TX_BUF; 
	
	for(uCnt=0;uCnt<10000;uCnt++)
	{
		*(pRec_buf+uCnt)=0x55555555;
	}
	*(pRec_buf+uCnt) = 0x0d;
	*(pRec_buf) = 0x01020304;
}


// File Name : Uart_LoopBack
// File Description : Check Uart LoopBack mode 
// Input : NONE
// Output : NONE
// Version : 0.1
void Uart_LoopBackPerformance(void)
{
	s32 sCh;
	s32 sBrate;
	u8 ucTxCnt = 0;
	u8 ucCnt = 0;
	u32 ucCnt2 = 0;
	
	RxBufferClear();

	UART_Printf("Which channel do you want to check(0~3)[D=1]?\n");
	UART_Printf("Choose : ");
	sCh = UART_GetIntNum();
	UART_Putc('\n');
	if (sCh == -1)
		sCh = 1;
	else if (sCh == 0)
	{
		UART_Printf("\nDebug Channel will be channel 1\n");
		
		UART_InitDebugCh(1,115200);
		Delay(1000);
	}	

	UART_Printf("\n");


	UART_Printf("How much BaudRate do you want to use as a Tx?\n");
	UART_Printf("BaudRate (ex 9600, 115200[D], 921600) : ");
	sBrate = UART_GetIntNum();
	if (sBrate == -1)
		sBrate = 115200;
	UART_Printf("\n");


		
	UART_SetConfig(sCh,2,1,1,4,3,1,1,sBrate,1,3,1,1);    
	UART_Open2(sCh);
	Delay(1000);

	UART_Printf("Press any key when you ready to Start/n");
	UART_Getc();



/*	
	Txdone = 1;
	Rxdone = 1;
	while(Txdone|Rxdone)
		{
		if(Txdone)
			{
				if(UART_RdUTRSTAT(sCh)&0x4) 
				{
				UART_WrUtxh(sCh,*pTestLBString);

					if(*pTestLBString == '\r') 
					Txdone = 0;

				pTestLBString++;
				}
			}

		if(Rxdone)
			{

				if(UART_RdUTRSTAT(sCh)&0x1) 
				{

				*pUartRxStr = UART_RdUrxh(sCh);


					if(*pUartRxStr == '\r')
						Rxdone = 0;

					
					pUartRxStr++;
				}
			}

		}


	pUartRxStr =(u8 *)(UART_RX_BUF);
	
	while(1) 
		{
			UART_Printf("%c",*pUartRxStr);
			if(*pUartRxStr == '\r')
				break;
			
			pUartRxStr++;
		}

*/
	UART_RxEmpty(sCh);


	while(!UART_GetKey())
		{
		
//				if(!(UART_RdUFSTAT(sCh)&0x4000)) 		//if TX Fifo is not Full
//				if(UART_RdUTRSTAT(sCh)&0x4) 				//if TX Fifo empty

			while(!(UART_RdUFSTAT(sCh)&0x4000))
				{
					UART_WrUtxh(sCh,ucTxCnt);
					//UART_Printf("%d\b\b",ucCnt);
					//Delay(100);
					ucTxCnt++;
					if(ucTxCnt == 0xf)
						ucTxCnt=0;
				}

//			if((UART_RdUFSTAT(sCh)&0x40)) 		//if Rx Fifo is Full
			if(UART_RdUTRSTAT(sCh)&0x1) 				//receive buffer has data
			{

				if(ucCnt == UART_RdUrxh(sCh))	
					{
					//UART_Printf("%d\b\b",ucCnt);
					ucCnt++;
						if(ucCnt == 0xf)
							{
							ucCnt=0;
							ucCnt2++;
							}
					}
				
				else
					{
					UART_Printf("Test is failed!!!\n");
					UART_Printf("Cnt = %d\n",ucCnt);
					UART_Printf("Cnt2 = %d",ucCnt2*15);
					break;
					}

			}
		}



	
	UART_Close(sCh);
	
	if (sCh == 0)
	{
		UART_Printf("\nDebug Channel will be channel 0 again\n");
	
		UART_InitDebugCh(0,115200);
		Delay(1000);	
	}
}



// File Name : Uart_TxAgingTest
// File Description : for Uart Pending bit problem validation
// Input : NONE
// Output : NONE
// Version : 0.1
void Uart_TxAgingTest(void)
{
	u8 ch;
	static u8 acTestString[]="ABCDEFGHIJKLMNOPQRSTUVWXYZ123456789abcdefghijklmnopqrstuvwxyz0123456789 ABCDEFGHIJKLMNOPQRSTUVWXYZ123456789abcdefghijklmnopqrstuvwxyz0123456789 ABCDEFGHIJKLMNOPQRSTUVWXYZ123456789abcdefghijklmnopqrstuvwxyz0123456789 ABCDEFGHIJKLMNOPQRSTUVWXYZ123456789abcdefghijklmnopqrstuvwxyz0123456789 ABCDEFGHIJKLMNOPQRSTUVWXYZ123456789abcdefghijklmnopqrstuvwxyz0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ123456789abcdefghijklmnopqrstuvwxyz0123456789 ABCDEFGHIJKLMNOPQRSTUVWXYZ123456789abcdefghijklmnopqrstuvwxyz0123456789 ABCDEFGHIJKLMNOPQRSTUVWXYZ123456789abcdefghijklmnopqrstuvwxyz0123456789 ABCDEFGHIJKLMNOPQRSTUVWXYZ123456789abcdefghijklmnopqrstuvwxyz0123456789 ABCDEFGHIJKLMNOPQRSTUVWXYZ123456789abcdefghijklmnopqrstuvwxyz0123456789\r";
//	static u8 acTestString[]="UUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU!!!!\r ";
//	static u8 acTestString[]="ABCDEFGHIJKLMNOPQRSTUVWXYZ123456789abcdefghijklmnopqrstuvwxyz0123456789 ABCDEFGHIJKLMNOPQRSTUVWXYZ123456789abcdefghijklmnopqrstuvwxyz0123456789 ABCDEFGHIJKLMNOPQRSTUVWXYZ123456789abcdefghijklmnopqrstuvwxyz0123456789 ABCDEFGHIJKLMNOPQRSTUVWXYZ123456789abcdefghijklmnopqrstuvwxyz0123456789 ABCDEFGHIJKLMNOPQRSTUVWXYZ123456789abcdefghijklmnopqrstuvwxyz0123456789 ";

	ch = UART_Config();	
	if( UART_Open2(ch) == SENDBREAK ) 
	{
		UART_Printf("\n\nSend Break Signal has been transfered\n");
		return; 	
	} 	

	UART_EnModemINT(ch,1);
	Delay(1000);	
	// UART Tx test with interrupt	
	UART_Printf("\n[Uart channel %d Tx Test]\n",ch);	
	while(!UART_GetKey())
		{
	UART_TxString(ch, acTestString);
		}
	UART_Printf("\nTx Done~ \n");	
}


// File Name : Uart_RTS
// File Description : Check Uart RTS/CTS manually (non AFC)
// Input : NONE
// Output : NONE
// Version : 0.1
void Uart_RTS(void)
{
	volatile u8 *pUartRxStr;
	s8 sCh;

	UART_Printf("Which channel do you want to check?\n");
	UART_Printf("[0 or 1] :");
	sCh = UART_GetIntNum();
	
	if (sCh == 0)
	{
		UART_Printf("\nDebug Channel will be channel 1\n");
		UART_Printf("\n");
		UART_Printf("change debug port cable to channel 1 and Press any key to ready RTS\n");

		UART_InitDebugCh(1,115200);
		Delay(1000);
	}	
	else
	{
		UART_Printf("Press any key to start RTS\n");
		sCh = 1;
	}
	UART_Printf("\n");
	

	
	UART_SetConfig(sCh,0,1,1,4,1,2,1,115200,1,5,1,1);
	UART_Open2(sCh);
	Delay(1000);
	pUartRxStr=(u8 *)(UART_RX_BUF);
	RxBufferClear();

	UART_Getc();	
	
	UART_WrRTS(sCh,RTS_inAct);
	UART_Printf("Start up Tx(CTS) on the other B'd and then press any key, it will start Rx(RTS)\n");
	UART_Getc();

	
	while(1) 
		{

			if(UART_RdUFSTAT(sCh)&0x3f)
				{
				UART_WrRTS(sCh,RTS_inAct);					//disable RTS
					if(UART_RdUTRSTAT(sCh)&0x1) 
						{
						
						*pUartRxStr = UART_RdUrxh(sCh);
	
							if(*pUartRxStr == '\r')
								break;

							
							pUartRxStr++;
						}
					}
			UART_WrRTS(sCh,RTS_Act);						//enbale RTS

		}

	pUartRxStr =(u8 *)(UART_RX_BUF);

	UART_Printf("##Rx :  ");
	while(1) 
		{
			UART_Printf("%c",*pUartRxStr);
			if(*pUartRxStr == '\r')
				break;
			
			pUartRxStr++;
		}
	if(sCh == 0)
		{
		UART_Printf("\n\nPress any key when you ready to change debug port as 0\n");
		UART_Printf("Change debug port cable to channel 0!\n");
		}
	else
		UART_Printf("Press any key when you end test\n");
	UART_Getc();

	UART_Close(sCh);
	
	if (sCh == 0)
	{
		UART_Printf("\nDebug Channel will be channel 0 again\n");
	
		UART_InitDebugCh(0,115200);
		Delay(1000);	
	}

}


// File Name : Uart_CTS
// File Description : Check Uart RTS/CTS manually (non AFC)
// Input : NONE
// Output : NONE
// Version : 0.1
void Uart_CTS(void)
{
	u8* pUartTxStr;
//	u8 acTestPString[]="\r";
	u8 acTestPString[]="ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890->UART Tx CTS test is good!!!!ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890->UART Tx CTS test is good!!!!\r";
//	static u8* acTestPString="ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890->UART Tx Polling test is good!!!!\r";
	s8 sCh;

	UART_Printf("Which channel do you want to check?\n");
	UART_Printf("[0 or 1] :");
	sCh = UART_GetIntNum();
	
	if (sCh == 0)
	{
		UART_Printf("\nDebug Channel will be channel 1\n");

		UART_Printf("\n");
		UART_Printf("Change Debug port cable to channel 1 and Press any key to start CTS\n");

		UART_InitDebugCh(1,115200);
		Delay(1000);
	}	
	else
	{
		UART_Printf("Press any key to start CTS\n");
		sCh = 1;
	}


		
	UART_SetConfig(sCh,0,1,1,4,1,2,1,115200,1,5,1,1);
	UART_Open2(sCh);
	Delay(1000);
	UART_Getc();
	UART_Printf("### CTS started ###\n");
	

	pUartTxStr = acTestPString;

	while(1) {

				if(UART_RdUMSTAT(sCh)&0x1)			// check CTS signal
					{
						if(UART_RdUTRSTAT(sCh)&0x4) 
						{
							UART_WrUtxh(sCh,*pUartTxStr);

							if(*pUartTxStr == '\r') 
								break;

							pUartTxStr++;
						}
					}
		}


	if(sCh == 0)
	UART_Printf("\n\nPress Enter when you complete to check Rx and then change Debug port cable to channel 0\n");
	else
	UART_Printf("\n\nPress Enter when you complete to check Rx\n");
	
	UART_Getc();

	UART_Close(sCh);
	
	if (sCh == 0)
	{
		UART_Printf("\nDebug Channel will be channel 0 again\n");
	
		UART_InitDebugCh(0,115200);
		Delay(1000);	
	}
}



// File Name : Uart_ModemInt
// File Description : Check Uart Modem Interrupt by RTS/CTS check 
// Input : NONE
// Output : NONE
// Version : 0.1
void Uart_ModemInt(void)
{
	u8* pUartTxStr;
//	u8 acTestPString[]="\r";
	u8 acTestPString[]="0123456789\r";
//	static u8* acTestPString="ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890->UART Tx Polling test is good!!!!\r";
	s8 sCh;

	UART_Printf("\n\n     ########################################################\n");
	UART_Printf("     This is the Modem Interrupt check test which is using manual mode of CTS function   \n");
	UART_Printf("     This check should be operate with UART_RTS test on the other Board!\n");
	UART_Printf("     At first, Set up UART_RTS test on the other connected Board n Press Any Key to start\n");
	UART_Printf("     ########################################################\n");	
	UART_Getc();



	UART_Printf("Which channel do you want to check?\n");
	UART_Printf("[0 or 1] :");
	sCh = UART_GetIntNum();
	
	if (sCh == 0)
	{
		UART_Printf("\nDebug Channel will be channel 1\n");

		UART_Printf("\n");
		UART_Printf("Change Debug port cable to channel 1 and Press any key to start CTS\n");

		UART_InitDebugCh(1,115200);
		Delay(1000);
	}	
	else
	{
		UART_Printf("Press any key to start CTS\n");
		sCh = 1;
	}

	UART_Getc();
	
	UART_Printf("\n");

	UART_Printf("\n### CTS Started ###\n");
		
	UART_SetConfig(sCh,0,1,1,4,1,2,1,115200,1,5,1,1);
	UART_Open2(sCh);
	UART_EnModemINT(sCh,1);	
	Delay(1000);	
	UART_OpenModem(sCh) ;

	

	pUartTxStr = acTestPString;

	while(1) {

				if(UART_RdUMSTAT(sCh)&0x1)			// check CTS signal
					{
						if(UART_RdUTRSTAT(sCh)&0x4) 
						{
							UART_WrUtxh(sCh,*pUartTxStr);

							if(*pUartTxStr == '\r') 
								break;

							pUartTxStr++;
						}
					}

		}




	if(sCh == 0)
	UART_Printf("\n\nPress Enter when you complete to check Rx and then change Debug port cable to channel 0\n");
	else
	UART_Printf("\n\nPress Enter when you complete to check Rx\n");
	
	UART_Getc();

	UART_Close(sCh);
	
	if (sCh == 0)
	{
		UART_Printf("\nDebug Channel will be channel 0 again\n");
	
		UART_InitDebugCh(0,115200);
		Delay(1000);	
	}	

	UART_Getc();
}


