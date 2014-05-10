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
*	File Name : uart.h
*  
*	File Description : This file declares prototypes of UART API funcions.
*
*	Author : Woojin,Kim
*	Dept. : AP Development Team
*	Created Date : 2007/01/16
*	Version : 0.1 
* 
*	History
*	- Creat debug function (InitDebug,Putc,Getc,Getkey)(Haksoo,Kim 2006/11/08) 
*	- Creat Full function (Woojin,Kim 2007/01/16)
*  
**************************************************************************************/

#ifndef __UART_H__
#define __UART_H__


#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

#include "def.h"

#define SENDBREAK			(0x99)

typedef struct tag_UART_REGS
{
	u32 rUlCon;
	u32 rUCon;
	u32 rUfCon;
	u32 rUmCon;
	u32 rUtrStat;
	u32 rUerStat;
	u32 rUfStat;
	u32 rUmStat;
	u32 rUtxh;
	u32 rUrxh;
	u32 rUbrDiv;
	u32 rUdivSlot;
	u32 rUintP;
	u32 rUintSp;
	u32 rUintM;
} UART_REGS;


typedef struct tag_UART_CON
{
	u32 uBaudrate;
	u8 cSelUartIrda; // Uart mode
	u8 cLoopTest;  // UCON[5]  (0 : normal, 1 : LoopBack)
	u8 cAfc;
	u8 cEnableFifo;
	u8 cOpClock; 	// 0,2:PCLK, 1:UEXTCLK, 2:EPLL
	u8 cDataBit; 	// 0:5bit, 1:6bit, 2:7bit, 3:8bit
	u8 cParityBit; 	// 0:no parity, 1:odd, 2:even, 3:forced 1, 4:forced 0
	u8 cStopBit; 	// 0:one stopbit, 1:two stopbit
	u8 cTxMode; 	// 0:disable, 1:interrupt or polling, 2:DMA0, 3:DMA1
	u8 cTxTrig; 	// 0:empty, 1:16, 2:32, 3:48 (byte)
	u8 cRxMode; 	// 0:disable, 1:interrupt or polling, 2:DMA0, 3:DMA1
	u8 cRxTrig; 	// 0:1, 1:8, 2:16, 3:32 (byte)
	u8 cRtsTrig; 	// 0:63, 1:56, 2:48, ... , 7:8 (byte)
	u8 cSendBreakSignal;  // ULCON[4]  (0 : normal, 1 : Send Break Signal)
} UART_CON;

typedef enum RTS
{
	RTS_inAct = 0,
	RTS_Act = 1
}RTS;

typedef struct
{
	u8	result_ok;
	u8	*str;
}RX_STRING_RESULT;

// add jungil 080302
#define NormalMode	1
#define IrDaMode	2
#define NoParity	0
#define OddParity	2
#define EvenParity	3
#define ParityCheck1	4
#define ParityCheck0	5
#define OneStopBit	1
#define TwoStopBit	2
#define DataLength5bit	0
#define DataLength6bit	1
#define DataLength7bit	2
#define DataLength8bit	3
#define SendBreakSiganl	0x1
#define LoopTest	0x2
#define UARTPCLK		0x1
#define UARTEXTCLK	0x2
#define UARTEPLL		0x3
#define UARTPolORIRQMode 0x1
#define UARTDMAMode	0x2





void UART_InitDebugCh(u8 ch, u32 uBaudRate);
void UART_Putc(char data);
void UART_TxEmpty(void);
s8 UART_Getc(void);
s8 UART_GetKey(void);
u8 UART_Config(void);
void UART_TxString(u8 ch, u8 * str);
void UART_RxString(u8 ch);
void UART_OpenModem(u8 ch) ;
void UART_WrUtxh(u32 cCh,u8 cTx);
u8 UART_RdUrxh(u32 cCh);
void UART_RxEmpty(u32 cCh);
u32 UART_RdUTRSTAT(u32 cCh);
u32 UART_RdUFSTAT(u32 cCh);
u32 UART_RdUMSTAT(u32 cCh);
void UART_WrRTS(u32 cCh, RTS en);
void UART_SendBreak(u32 cCh);
void UART_EnModemINT(u32 cCh,u32 uEn);
u8 UART_Open(u8 ch);
u8 UART_Open2(u8 ch);
u8 UART_Close(u8 ch);
u32 UART_InitializeREG( UART_REGS *pUartRegs, UART_CON *pUartCon);
void UART_CalcBaudrate( UART_REGS *pUartRegs, UART_CON *pUartCon);
void __irq Isr_Uart0(void);
void __irq Isr_Uart1(void);
void __irq Isr_Uart2(void);
void __irq Isr_Uart3(void);
void Isr_UartSub(u8 ch);
void __irq Isr_Dma(void);
u8 UART_SetConfig(u8 cCh, u32 uBreakorLoop, u32 uParity, u32 uNumStop, u32 uWordLength, u32 uOpCLK, 
					u32 uSelPLL, u32 uExtIFtype, u32 uBaudrate, u32 uSelOpmode, u32 uSelFIFO, u32 uSelAFC, u32 uRTSLvL);
u8 UART_SetConfig_2(u8 cCh, u32 uBreakorLoop,u32 uParity,u32 uNumStop,u32 uWordLength, 
				u32 uExtIFtype,u32 uBaudrate,u32 uSelOpmode,u32 uSelFIFO,u32 uSelAFC,u32 uRTSLvL);



s32 UART_GetIntNum(void);
static s32 Str2Int(s8 *string);
void UART_PutString(const s8 *string);
void UART_GetString(s8 *pStr);
u32 CalcEPLL(u32 uMdiv,u32 uPdiv,u32 uSdiv,u32 uKdiv);
void UART_SetEpll_forUART(u32 uMdiv,u32 uPdiv,u32 uSdiv,u32 uKdiv);

#ifdef __cplusplus
}
#endif

#endif //__UART_H__

