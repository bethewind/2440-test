

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
*	File Name : uart.c
*  
*	File Description : This file implements the API functon for UART.
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

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "option.h"
#include "library.h"
#include "sfr6410.h"
#include "system.h"
#include "uart.h"
#include "sysc.h"
#include "gpio.h"
#include "intc.h"
#include "dma.h"

#define UART_OFFSET		0x400

#define UART0_BASE		(UART_REGS *)(UART_BASE)
#define UART1_BASE		(UART_REGS *)(UART_BASE+UART_OFFSET)
#define UART2_BASE		(UART_REGS *)(UART_BASE+UART_OFFSET*2)
#define UART3_BASE		(UART_REGS *)(UART_BASE+UART_OFFSET*3)

#define UART_BUF 		(0x51000000)
#define UART_TX_BUF		(UART_BUF)
#define UART_RX_BUF		(UART_BUF+0x00200000)
#define FIFO_DEBUG_BUF	(UART_BUF+0x00400000)


#define TX_FIFO_RESET	(1<<2)
#define RX_FIFO_RESET	(1<<1)
#define TX_INT_TYPE		(1<<9) // 0:pulse 1:level
#define RX_INT_TYPE		(0<<8) // 0:pulse 1:level

#define RX_TIMEOUT_EN	(1<<7) // 0:disable 1:enable, disable for FIFO test
#define RX_ERR_INT_EN	(1<<6) // 0:disable 1:enable
#define RTS_ACTIVE		(1) // In normal mode, nRTS signal  0:low, 1:High

#define BIT_UART_MODEM	(1<<3)
#define BIT_UART_TXD	(1<<2)
#define BIT_UART_ERROR	(1<<1)
#define BIT_UART_RXD	(1)

#define TX_END_CHAR		NULL
#define RX_END_CHAR		'\r'

static	DMAC 	oUARTDma;


/////////
// Global variables
static UART_CON	g_AUartCon[5] = {{115200,0,0,0,0,0,3,0,0,1,1,1,1,1,1}
								, {115200,0,0,0,0,0,3,0,0,1,1,1,1,1,1}
								, {115200,0,0,0,0,0,3,0,0,1,1,1,1,1,1}
								, {115200,0,0,0,0,0,3,0,0,1,1,1,1,1,1}
								, {115200,0,0,0,0,0,3,0,0,1,1,1,1,1,1}}; 
						// control property per each channel. 4th data is defualt value for initialize

		     		
static volatile u8 g_AisTxDone[4] = {0, 0, 0, 0};
static volatile u8 g_AisRxDone[4] = {0, 0, 0, 0};
static u8 *g_pUartTxStr[4];
static u8 *g_pUartRxStr[4];

volatile u32 *g_pFifoDebug  = (u32 *)FIFO_DEBUG_BUF; //temporary for fifo count test
volatile u32 g_uFcnt = 0;
volatile u32 g_uOpClock=0;

static UART_REGS *g_pUartDebugRegs;

static u8 g_ucUartOpClk_DivRatio=0;
u32 g_uUartRxDmaTc=64;
RX_STRING_RESULT	g_sRxStringResult;


//////////
// Function Name : UART_InitDebugCh
// Function Description : This function initializes a certain uart ch. for debugging console
// Input : NONE 
// Output : NONE
// Version : 
void UART_InitDebugCh(u8 ch, u32 uBaudRate)
{
	SYSC_GetClkInform();
	UART_SetConfig(ch,0,1,1,4,1,2,1,uBaudRate,1,1,1,1);
	UART_Open(ch);
	return;
}


//////////
// Function Name : UART_Putc
// Function Description : This function write character data to uart debugging ch
// Input : NONE 
// Output : NONE
// Version : 
void UART_Putc(char data)
{
	u32 temp;

	if(data=='\n')
	{
		while(1)
		{
			temp = Inp32(&g_pUartDebugRegs->rUtrStat);
			temp&=0x2;
			if(temp)
				break;
		}
		Outp8(&g_pUartDebugRegs->rUtxh,'\r');
	}
	while(1)
	{
		temp = Inp32(&g_pUartDebugRegs->rUtrStat);
		temp&=0x02;
		if(temp)
			break;
	}
	Outp8(&g_pUartDebugRegs->rUtxh,data);
	
	return;
}


//////////
// Function Name : UART_TxEmpty
// Function Description : This function Hold Uart Tx until FIFO empty
// Input : NONE 
// Output : NONE
// Version : 
void UART_TxEmpty(void)
{
	u32 temp32;

	while(1)
	{
		temp32 = Inp32(&g_pUartDebugRegs->rUtrStat);		
		temp32&=0x04;
		if(temp32)
			break;
	}
}



//////////
// Function Name : UART_Getc
// Function Description : This function read character data from uart debugging ch
// Input : NONE 
// Output : temp8, character data received through uart
// Version : 
s8 UART_Getc( void)
{
	u32 temp32;
	char temp8;
	while(1)
	{
		temp32 = Inp32(&g_pUartDebugRegs->rUtrStat);		
		temp32&=0x01;
		if(temp32)
			break;
	}
	temp8 = Inp8(&g_pUartDebugRegs->rUrxh);
	return temp8;
}


//////////
// Function Name : UART_GetKey
// Function Description : This function read character data from uart debugging ch if there is received data
// Input : NONE 
// Output : temp8 or 0, character data received through uart or 0
// Version : 
s8 UART_GetKey( void)
{
	u32 temp32;
	char temp8;

	temp32 = Inp32(&g_pUartDebugRegs->rUtrStat);
	if(temp32 & 0x1)
	{
		temp8 = Inp8(&g_pUartDebugRegs->rUrxh);
		return temp8;
	}
	else
		return 0;
}



//////////
// Function Name : UART_GetIntNum
// Function Description : Input a number(hex, dec) from UART
// Input : NONE 
// Version : 
// added by rb1004

s32 UART_GetIntNum(void)
{
    u8 str[32];
	
    UART_GetString((s8 *)str);
    return Str2Int((s8 *)str);
}


//////////
// Function Name : UART_GetString
// Function Description : Input a string from UART
// Input :  
// Version : 
// added by rb1004
void UART_GetString(s8 *pStr)
{
    u8 *pStrOrg;
    u8 c;

    pStrOrg = (u8 *)pStr;
	
    while ((c= UART_Getc())!='\r')
    {
 		if (c=='\b') {
 		    if ((int)pStrOrg < (int)pStr) {
 				 UART_PutString("\b \b");
 				pStr--;
 		    }
 		}
 		else {
 		    *pStr++ = c;
 		     UART_Putc(c);
 		}
    }
	
    *pStr = '\0';
     UART_Putc('\n');
}


//////////
// Function Name : UART_PutString
// Function Description : Transmit a string through UART
// Input :  
// Version : 
// added by rb1004
void UART_PutString(const s8 *string)
{
    while(*string)
		UART_Putc(*string++);
}


//////////
// Function Name : Str2Int
// Function Description : convert string to digit.
// Input :  
// Version : 
// added by rb1004
static s32 Str2Int(s8 *string)
{
    s32 base     = 10;
    s32 minus    = 0;
    s32 result   = 0;
    s32 lastIndex;
    s32 i;

    if(string[0]=='-')
    {
        minus = 1;
        string++;
    }

    if(string[0]=='0' && (string[1]=='x' || string[1]=='X'))
    {
        base    = 16;
        string += 2;
    }

    lastIndex = strlen((const char *)string) - 1;

    if(lastIndex<0)
        return -1;

    if(string[lastIndex]=='h' || string[lastIndex]=='H' )
    {
        base = 16;
        string[lastIndex] = 0;
        lastIndex--;
    }

    if(base==10)
    {
        //result = atoi(string); // atoi conversion not working properly //trb 051011
		/* Alternate implementation for atoi */
        result = 0;
        for(i = 0; i<=lastIndex; i++){
			result = result * 10 + string[i] - 0x30;
       	}
        result = minus ? (-1*result):result;
    }
    else
    {
        for(i=0;i<=lastIndex;i++)
        {
            if(isalpha(string[i]))
            {
                if(isupper(string[i]))
                    result = (result<<4) + string[i] - 'A' + 10;
                else
                    result = (result<<4) + string[i] - 'a' + 10;
            }
            else
                result = (result<<4) + string[i] - '0';
        }
        result = minus ? (-1*result):result;
    }
    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////


//////////
// Function Name : UART_Config
// Function Description : This function set up UART by user's choice
// Input : NONE 
// Output : NONE
// Version : v0.1
u8 UART_Config(void)
{
	u8 cCh;
	s32 iNum = 0;	

	volatile UART_CON *pUartCon;	

	g_uOpClock = 0;	

	// Select Channel
	UART_Printf("Note : [D] mark means default value. If you press ENTER key, default value is selected.\n");
	UART_Printf("Select Channel(0~3) [D=1] : ");

	cCh = (u8)UART_GetIntNum();
	if ( cCh>3 )	cCh = 1; // default uart 1
		
	pUartCon = &g_AUartCon[cCh];
	
	//Set Other Options
	UART_Printf("\nSelect Other Options\n 0. Nothing[D]  1.Send Break Signal  2. Loop Back Mode  \n Choose : ");
	switch(UART_GetIntNum())
	{		
		default : 	
					pUartCon->cSendBreakSignal = 0x0;
					pUartCon->cLoopTest = 0x0;
					break;
					
		case 1 :		
					pUartCon->cSendBreakSignal = 1;	
					return cCh;

					
		case 2 :	 	
					pUartCon->cLoopTest = 1;		
					break;		
	}		
	
	
	//Set Parity mode
	UART_Printf("\nSelect Parity Mode\n 1. No parity[D] 2. Odd 3. Even 4. Forced as '1' 5. Forced as '0' \n Choose : ");
	switch(UART_GetIntNum())
	{		
		default : 	
			pUartCon->cParityBit = 0;		
			break;
		case 2 : 		
			pUartCon->cParityBit = 4;
			break;
		case 3 :		
			pUartCon->cParityBit = 5;
			break;
		case 4 :		
			pUartCon->cParityBit = 6;
			break;
		case 5 :		
			pUartCon->cParityBit = 7;
			break;
	}
	//Set the number of stop bit	
	UART_Printf("\n\nSelect Number of Stop Bit\n 1. One stop bit per frame[D] 2. Two stop bit per frame \n Choose : ");
	switch(UART_GetIntNum())
	{
		default : 	
			pUartCon->cStopBit = 0;
			break;
			
		case 2 : 		
			pUartCon->cStopBit = 1;
			break;		
	}
	//Set Word Length	
	UART_Printf("\n\nSelect Word Length\n 1. 5bits 2. 6bits 3. 7bits 4. 8bits[D] \n Choose : ");
	switch(UART_GetIntNum())
	{		
		case 1 :		
			pUartCon->cDataBit = 0;
			break;
		case 2 :		
			pUartCon->cDataBit = 1;
			break;
		case 3 :		
			pUartCon->cDataBit = 2;		
			break;	
		default :		
			pUartCon->cDataBit = 3;
			break;
	}
	
	
	// Set Operation clock
	UART_Printf("\n\nSelect Operating Clock\n 1. PCLK[D]	2. EXT_CLK0(pwm)	3. EXT_CLK1(EPLL/MPLL) \n Choose : ");
	switch (UART_GetIntNum())
	{
		case 2 :
			pUartCon->cOpClock = 1;
			// connect CLKOUT and UEXTCLK
			UART_Printf("\nInput PWM EXT_CLK by Pulse Generater\n");
			UART_Printf("How much CLK do you input through the pwmECLK?");
			UART_Printf("Mhz : ");
			g_uOpClock = UART_GetIntNum()*1000000;
			GPIO_SetFunctionEach(eGPIO_F,eGPIO_13,2);		
			break;

		case 3 :
			pUartCon->cOpClock = 3;
			UART_Printf("\nSelect Clock SRC\n 1.EPLL  2.MPLL \n Choose: ");
			
			switch(UART_GetIntNum())
			{
				case 1:
					SYSC_SetPLL(eEPLL,32,1,1,0);   //EPLL=192Mhz
					SYSC_ClkSrc(eEPLL_FOUT);
					SYSC_ClkSrc(eUART_MOUTEPLL);
					SYSC_SetDIV2(0,0,0,0,g_ucUartOpClk_DivRatio,0);	//5th argument : uart clock divider
					Delay(100);
					g_uOpClock = CalcEPLL(32,1,1,0)/(g_ucUartOpClk_DivRatio+1);
					UART_Printf("UART Op Clock = %.2fMhz\n",((float)g_uOpClock/1000000));
					// use EPLL output clock
					//CLK_SRC	UART_SEL[13]  0:EPLL
					//CLK_DIV2	UART_RATIO[19:16]
					break;
					
				case 2:
					SYSC_ClkSrc(eMPLL_FOUT);						
					SYSC_ClkSrc(eUART_DOUTMPLL);
					SYSC_SetDIV2(0,0,0,0,g_ucUartOpClk_DivRatio,0);	//5th argument : uart clock divider
					Delay(100);
					g_uOpClock = (u32)g_MPLL/(((SYSC_GetDIV0()>>4)&0x1)+1)/(g_ucUartOpClk_DivRatio+1);
					UART_Printf("UART Op Clock = %.2fMhz\n",((float)g_uOpClock/1000000));
					// use MPLL output clock
					//CLK_SRC	UART_SEL[13]	  1:MPLL
					//CLK_DIV2	UART_RATIO[19:16]
					break;
					
				default:
					SYSC_SetPLL(eEPLL,32,1,1,0);   //EPLL=192Mhz
					SYSC_ClkSrc(eEPLL_FOUT);
					SYSC_ClkSrc(eUART_MOUTEPLL);
					SYSC_SetDIV2(0,0,0,0,g_ucUartOpClk_DivRatio,0);	//5th argument : uart clock divider
					Delay(100);
					g_uOpClock = CalcEPLL(32,1,1,0)/(g_ucUartOpClk_DivRatio+1);
					UART_Printf("UART Op Clock = %.2fMhz\n",((float)g_uOpClock/1000000));
					// use EPLL output clock
					//CLK_SRC	UART_SEL[13]  0:EPLL
					//CLK_DIV2	UART_RATIO[19:16]
					break;								
			}
			break;

		default :
			pUartCon->cOpClock = 2; // PCLK		
			break;
	}


	// Select UART or IrDA 1.0
	UART_Printf("\n\nSelect External Interface Type\n 1. UART[D]   2. IrDA mode\n Choose : ");
	if (UART_GetIntNum() == 2)		
		pUartCon->cSelUartIrda = 1; // IrDA mode
	else					
		pUartCon->cSelUartIrda = 0; // URAT mode

	// Set Baudrate
	UART_Printf("\n\nType the baudrate and then change the same baudrate of host, too.\n");
	UART_Printf(" Baudrate (ex 9600, 115200[D], 921600) : ");
	pUartCon->uBaudrate = UART_GetIntNum();

	if ((s32)pUartCon->uBaudrate == -1) 
		pUartCon->uBaudrate = 115200;

	// Select UART operating mode
	UART_Printf("\n\nSelect Operating Mode\n 1. Interrupt[D]	2. DMA\n Choose : ");
	if (UART_GetIntNum() == 2)
	{
		pUartCon->cTxMode = 2; // DMA0 mode
		pUartCon->cRxMode = 3; // DMA1 mode
	}
	else
	{	
		pUartCon->cTxMode = 1; // Int mode
		pUartCon->cRxMode = 1; // Int mode
	}

	// Select UART FIFO mode
	UART_Printf("\n\nSelect FIFO Mode (Tx/Rx[byte])\n 1. no FIFO[D]  2. Empty/1  3. 16/8   4. 32/16  5. 48/32 \n Choose : ");
	iNum = UART_GetIntNum();
	if ( (iNum>1)&&(iNum<6) )
	{
		pUartCon->cEnableFifo = 1;
		pUartCon->cTxTrig = iNum -2;
		pUartCon->cRxTrig = iNum -2;
	}
	else 
	{
		pUartCon->cEnableFifo = 0;
	}

	// Select AFC mode enable/disable
	UART_Printf("\n\nSelect AFC Mode\n 1. Disable[D]	2. Enable\n Choose : ");
	if (UART_GetIntNum() == 2)
	{
		pUartCon->cAfc = 1; // AFC mode enable
		UART_Printf("Select nRTS trigger level(byte)\n 1. 63[D]   2. 56   3. 48   4. 40   5. 32   6. 24   7. 16   8. 8\n Choose : ");
		iNum = UART_GetIntNum();
		
		if ( (iNum>1)&&(iNum<9) )	
			pUartCon->cRtsTrig = iNum -1;

		else						
			pUartCon->cRtsTrig = 0; // default 63 byte
	}
	else
	{
		pUartCon->cAfc = 0; // AFC mode disable
	}
	
#if 1
	UART_Printf("\n -Brate = %d,\n -SelUartIrda = %d,\n -Looptest= %d,\n -Afc = %d,\n -EnFiFO = %d,\n",
		pUartCon->uBaudrate,
		pUartCon->cSelUartIrda,
		pUartCon->cLoopTest,
		pUartCon->cAfc,
		pUartCon->cEnableFifo);
	UART_Printf(" -OpClk = %d,\n -Databit = %d,\n -Paritybit = %d,\n -Stopbit = %d,\n -Txmode = %d,\n",
		pUartCon->cOpClock,
		pUartCon->cDataBit,
		pUartCon->cParityBit,
		pUartCon->cStopBit,
		pUartCon->cTxMode);
	UART_Printf(" -TxTrig = %d,\n -RxMode = %d,\n -RxTrig = %d,\n -RtsTrig = %d,\n -SendBsig = %d\n",
		pUartCon->cTxTrig,
		pUartCon->cRxMode,
		pUartCon->cRxTrig,
		pUartCon->cRtsTrig,
		pUartCon->cSendBreakSignal);
#endif

	return cCh;
}



//////////
// Function Name : UART_TxString
// Function Description : This function trasmits String through UART
// Input : ch [0~4 UART channel]
//		 str [character type string that you want to transmit, the last charater of string should be 'NULL']
// Output : NONE
// Version : v0.1

void UART_TxString(u8 ch, u8 *str)  // The last character of 'str' should be NULL
{
	volatile UART_REGS *pUartRegs;
	volatile UART_CON *pUartCon;
	u8 cTemp;
	u32 uTemp2;	

	pUartRegs = (volatile UART_REGS *)(UART_BASE+UART_OFFSET*ch);
	pUartCon = &g_AUartCon[ch];	

	g_AisTxDone[ch] = 0;
	g_pUartTxStr[ch] = str;

	g_pFifoDebug = (u32 *)FIFO_DEBUG_BUF;
	g_uFcnt = 0;
	
	cTemp = pUartCon->cTxMode  & 3;	
	
	if ( cTemp == 1 ) // interrupt mode
	{
		Outp32(&pUartRegs->rUintSp , (BIT_UART_MODEM|BIT_UART_TXD|BIT_UART_ERROR|BIT_UART_RXD));
		Outp32(&pUartRegs->rUintP , (BIT_UART_MODEM|BIT_UART_TXD|BIT_UART_ERROR|BIT_UART_RXD));

		uTemp2 = Inp32(&pUartRegs->rUintM);
		uTemp2 &= ~(BIT_UART_MODEM|BIT_UART_TXD|BIT_UART_ERROR|BIT_UART_RXD);
		Outp32(&pUartRegs->rUintM,uTemp2);
		
		INTC_Enable(NUM_UART0+ch);
	}
	else if ( cTemp==2 ) // dma mode
	{
		DMAC_InitCh(DMA0,DMA_A,&oUARTDma);
		
		switch(ch)
		{
			case 0:
				SYSC_SelectDMA(eSEL_UART0_0, 1);
		       	 // Channel, LLI_Address, SrcAddr, Src Type, DstAddr, Dst Type, Transfer Width, Transfer Size, OpMode(DEMAND), Src Req, Dst Req, Burst
				DMACH_Setup(DMA_A,0x0,(u32)str,0,(u32)(&pUartRegs->rUtxh),1,BYTE,strlen((const char*)str),DEMAND,MEM,DMA0_UART0_0,SINGLE,&oUARTDma);
				break;
				
			case 1:
				SYSC_SelectDMA(eSEL_UART1_0, 1);
				// Channel, LLI_Address, SrcAddr, Src Type, DstAddr, Dst Type, Transfer Width, Transfer Size, OpMode(DEMAND), Src Req, Dst Req, Burst
				DMACH_Setup(DMA_A,0x0,(u32)str,0,(u32)(&pUartRegs->rUtxh),1,BYTE,strlen((const char*)str),DEMAND,MEM,DMA0_UART1_0,SINGLE,&oUARTDma);
				break;
				
			case 2:
				SYSC_SelectDMA(eSEL_UART2_0, 1);
				// Channel, LLI_Address, SrcAddr, Src Type, DstAddr, Dst Type, Transfer Width, Transfer Size, OpMode(DEMAND), Src Req, Dst Req, Burst
				DMACH_Setup(DMA_A,0x0,(u32)str,0,(u32)(&pUartRegs->rUtxh),1,BYTE,strlen((const char*)str),DEMAND,MEM,DMA0_UART2_0,SINGLE,&oUARTDma);
				break;
				
			case 3:
				SYSC_SelectDMA(eSEL_UART3_0, 1);
				// Channel, LLI_Address, SrcAddr, Src Type, DstAddr, Dst Type, Transfer Width, Transfer Size, OpMode(DEMAND), Src Req, Dst Req, Burst
				DMACH_Setup(DMA_A,0x0,(u32)str,0,(u32)(&pUartRegs->rUtxh),1,BYTE,strlen((const char*)str),DEMAND,MEM,DMA0_UART3_0,SINGLE,&oUARTDma);
				break;
		}

		INTC_Enable(NUM_DMA0);	
		DMACH_ClearIntPending(&oUARTDma);
		DMACH_ClearErrIntPending(&oUARTDma);

		DMACH_Start(&oUARTDma);

	}	
	
	
	while(!g_AisTxDone[ch]);

	// for debugging fifo 
	if ( (Inp32(&pUartRegs->rUfCon) & 1) && (cTemp == 1) ) // fifo enable & interrupt mode
	{
		g_pFifoDebug = (u32 *)FIFO_DEBUG_BUF;
		while(*g_pFifoDebug) // g_pFifoDebug address, g_uFcnt, UfCon, Tx Fifo count
		{
			UART_Printf("[0x%x,", g_pFifoDebug);
			UART_Printf("%d,", *g_pFifoDebug++);
			UART_Printf("0x%x,", *g_pFifoDebug++);
			UART_Printf("%d]", *g_pFifoDebug++);
		}
	}
	
	INTC_Disable(NUM_UART0+ch);
	INTC_Disable(NUM_DMA0);		
}


//////////
// Function Name : UART_RxString
// Function Description : This function receives String through UART
// Input : ch [0~4 UART channel]
// Output : g_pUartRxStr[ch] [charater type received string]
// Version : v0.1
void UART_RxString(u8 ch)  // The last character of input string should be '\r'. simple test code
{
	volatile UART_REGS *pUartRegs;
	volatile UART_CON *pUartCon;	
	u8 cTemp;
	u32 uTemp2;
	

	pUartRegs = (volatile UART_REGS *)(UART_BASE+UART_OFFSET*ch);
	pUartCon = &g_AUartCon[ch];	

	g_AisRxDone[ch] = 0;
	g_pUartRxStr[ch] = ( u8 *)(UART_RX_BUF);
	
	g_pFifoDebug = (u32 *)FIFO_DEBUG_BUF;
	g_uFcnt = 0;

	while (Inp32(&pUartRegs->rUfStat )& 0x3f) 	//until rx fifo count 0 (fifo clear)
	{
		(u8)(Inp32(&pUartRegs->rUrxh));
	}

	Outp32(&pUartRegs->rUfCon, Inp32(&pUartRegs->rUfCon)|(1<<1));
		
	cTemp = pUartCon->cRxMode & 3;
	if ( cTemp == 1 ) // interrupt mode
	{
		Outp32(&pUartRegs->rUintSp , (BIT_UART_MODEM|BIT_UART_TXD|BIT_UART_ERROR|BIT_UART_RXD));
		Outp32(&pUartRegs->rUintP , (BIT_UART_MODEM|BIT_UART_TXD|BIT_UART_ERROR|BIT_UART_RXD));

		uTemp2 = Inp32(&pUartRegs->rUintM);
		uTemp2 &= ~(BIT_UART_ERROR|BIT_UART_RXD);
		Outp32(&pUartRegs->rUintM,uTemp2);

		INTC_Enable(NUM_UART0+ch);
	}
	
	else if ( cTemp == 3 ) // dma mode
	{

		uTemp2 = Inp32(&pUartRegs->rUintM);
		uTemp2 &= ~(BIT_UART_ERROR);
		Outp32(&pUartRegs->rUintM,uTemp2);

		INTC_Enable(NUM_UART0+ch);
		
		DMAC_InitCh(DMA0,DMA_B,&oUARTDma);

		switch(ch)
		{
			case 0:
				SYSC_SelectDMA(eSEL_UART0_1, 1);			
				// Channel, LLI_Address, SrcAddr, Src Type, DstAddr, Dst Type, Transfer Width, Transfer Size, OpMode(DEMAND), Src Req, Dst Req, Burst
				DMACH_Setup(DMA_B,0x0,(u32)(&pUartRegs->rUrxh),1,(u32)g_pUartRxStr[ch],0,BYTE,g_uUartRxDmaTc,DEMAND,DMA0_UART0_1,MEM,SINGLE,&oUARTDma);
				break;

			case 1:
				SYSC_SelectDMA(eSEL_UART1_1, 1);
				// Channel, LLI_Address, SrcAddr, Src Type, DstAddr, Dst Type, Transfer Width, Transfer Size, OpMode(DEMAND), Src Req, Dst Req, Burst
				DMACH_Setup(DMA_B,0x0,(u32)(&pUartRegs->rUrxh),1,(u32)g_pUartRxStr[ch],0,BYTE,g_uUartRxDmaTc,DEMAND,DMA0_UART1_1,MEM,SINGLE,&oUARTDma);
				break;
				
			case 2:
				SYSC_SelectDMA(eSEL_UART2_1, 1);
				// Channel, LLI_Address, SrcAddr, Src Type, DstAddr, Dst Type, Transfer Width, Transfer Size, OpMode(DEMAND), Src Req, Dst Req, Burst
				DMACH_Setup(DMA_B,0x0,(u32)(&pUartRegs->rUrxh),1,(u32)g_pUartRxStr[ch],0,BYTE,g_uUartRxDmaTc,DEMAND,DMA0_UART2_1,MEM,SINGLE,&oUARTDma);
				break;
				
			case 3:
				SYSC_SelectDMA(eSEL_UART3_1, 1);
				// Channel, LLI_Address, SrcAddr, Src Type, DstAddr, Dst Type, Transfer Width, Transfer Size, OpMode(DEMAND), Src Req, Dst Req, Burst
				DMACH_Setup(DMA_B,0x0,(u32)(&pUartRegs->rUrxh),1,(u32)g_pUartRxStr[ch],0,BYTE,g_uUartRxDmaTc,DEMAND,DMA0_UART3_1,MEM,SINGLE,&oUARTDma);
				break;
		}

		INTC_Enable(NUM_DMA0);
		
		DMACH_ClearIntPending(&oUARTDma);
		DMACH_ClearErrIntPending(&oUARTDma);

		DMACH_Start(&oUARTDma);
	}	

	while(1)
	{
		if(g_AisRxDone[ch])
		{
			g_sRxStringResult.result_ok = TRUE;
			break;
		}
		
		if (UART_GetKey())
		{
			g_sRxStringResult.result_ok=FALSE;
			break;
		}		
	}
		
	// for debugging fifo 
	if ( (Inp32(&pUartRegs->rUfCon) & 1) && (cTemp == 1) ) // fifo enable & interrupt mode
	{
		g_pFifoDebug = (u32 *)FIFO_DEBUG_BUF;
		while(*g_pFifoDebug) // g_pFifoDebug address, g_uFcnt, UfCon, Rx Fifo count
		{
			UART_Printf("[0x%x,", g_pFifoDebug);
			UART_Printf("%d,", *g_pFifoDebug++);
			UART_Printf("0x%x,", *g_pFifoDebug++);
			UART_Printf("%d]", *g_pFifoDebug++);
		}
	}

	INTC_Disable(NUM_UART0+ch);
	INTC_Disable(NUM_DMA0);

	g_pUartRxStr[ch] = ( u8 *)(UART_RX_BUF);
	
	g_sRxStringResult.str = g_pUartRxStr[ch];
}

//////////
// Function Name : UART_WrUtxh
// Function Description : This function writes value on Utxh register
// Input : Channe [0~3]               Character[which you wants to write on Utxh register]
// Output : NONE
// Version : v0.1

void UART_WrUtxh(u32 cCh,u8 cTx)
{
	volatile UART_REGS *pUartRegs;

	pUartRegs = (volatile UART_REGS *)(UART_BASE+UART_OFFSET*cCh);

	Outp8(&pUartRegs->rUtxh,cTx);
}


//////////
// Function Name : UART_RdUrxh
// Function Description : This function reads value on Urxh register
// Input : Channe [0~3]
// Output : Urxh value
// Version : v0.1

u8 UART_RdUrxh(u32 cCh)
{
	volatile UART_REGS *pUartRegs;

	pUartRegs = (volatile UART_REGS *)(UART_BASE+UART_OFFSET*cCh);

	return Inp8(&pUartRegs->rUrxh);
}

//////////
// Function Name : UART_RxEmpty
// Function Description : This function Hold Uart until Rx FIFO empty
// Input : Channe [0~3]
// Output : Urxh value
// Version : v0.1
void UART_RxEmpty(u32 cCh)
{
	volatile UART_REGS *pUartRegs;

	pUartRegs = (volatile UART_REGS *)(UART_BASE+UART_OFFSET*cCh);


	while (Inp32(&pUartRegs->rUfStat )& 0x3f) 	//until rx fifo count 0 (fifo clear)
	{
	(u8)(Inp32(&pUartRegs->rUrxh));
	}	


}



//////////
// Function Name : UART_RdUTRSTAT
// Function Description : This function reads out UTRSTAT register
// Input : Channe [0~3]
// Output : UTRSTAT value
// Version : v0.1

u32 UART_RdUTRSTAT(u32 cCh)
{
	volatile UART_REGS *pUartRegs;

	pUartRegs = (volatile UART_REGS *)(UART_BASE+UART_OFFSET*cCh);

	return Inp32(&pUartRegs->rUtrStat);
}


//////////
// Function Name : UART_RdUFSTAT
// Function Description : This function reads out UFSTAT register(be able to check RX FIFO cnt, full bit)
// Input : Channe [0~3]
// Output : UFSTAT value
// Version : v0.1
u32 UART_RdUFSTAT(u32 cCh)
{
	volatile UART_REGS *pUartRegs;

	pUartRegs = (volatile UART_REGS *)(UART_BASE+UART_OFFSET*cCh);

	return Inp32(&pUartRegs->rUfStat);
}

//////////
// Function Name : UART_RdUMSTAT
// Function Description : This function reads out UMSTAT register(be able to check CTS signal bit)
// Input : Channe [0~3]
// Output : UMSTAT value
// Version : v0.1
u32 UART_RdUMSTAT(u32 cCh)
{
	volatile UART_REGS *pUartRegs;

	pUartRegs = (volatile UART_REGS *)(UART_BASE+UART_OFFSET*cCh);

	return Inp32(&pUartRegs->rUmStat);
}

//////////
// Function Name : UART_WrRTS
// Function Description : This function choose RTS enable signal manually
// Input : RTS [RTS_inAct/RTS_Act]
// Output : NONE
// Version : v0.1
void UART_WrRTS(u32 cCh, RTS en)
{
	volatile UART_REGS *pUartRegs;

	pUartRegs = (volatile UART_REGS *)(UART_BASE+UART_OFFSET*cCh);

	Outp32(&pUartRegs->rUmCon,en);
}

//////////
// Function Name : UART_SendBreak
// Function Description : This function makes SendBreak signal on selected channel
// Input : Channe [0~3]
// Output : NONE
// Version : v0.1
void UART_SendBreak(u32 cCh)
{
	
	u32 uTemp;
	volatile UART_REGS *pUartRegs;

	pUartRegs = (volatile UART_REGS *)(UART_BASE+UART_OFFSET*cCh);


	uTemp = Inp32(&pUartRegs->rUCon);
	uTemp |= (1<<4);						// Send Break signal
	Outp32(&pUartRegs->rUCon,uTemp);
		
}

//////////
// Function Name : UART_EnModemINT
// Function Description : This function choose Modem Interrupt enable/disable
// Input :   cCh [ Uart channel ],    uEn [ 0 : disable     1 : enable]
// Output : NONE
// Version : v0.1
void UART_EnModemINT(u32 cCh,u32 uEn)
{
	u32 uTempReg;
	
	volatile UART_REGS *pUartRegs;

	pUartRegs = (volatile UART_REGS *)(UART_BASE+UART_OFFSET*cCh);

	uTempReg = Inp32(&pUartRegs->rUmCon);
	uTempReg &= ~(1<<3);
	uTempReg |=(uEn<<3);
	Outp32(&pUartRegs->rUmCon,uTempReg);
}


//////////
// Function Name : UART_Open
// Function Description : This function set up H/W(GPIO) and initialize SFR of UART for Debugging
// Input : ch [0~4 UART channel]
// Output : g_pUartRxStr[ch] [charater type received string]
// Version : v0.1

u8 UART_Open(u8 ch) // setting H/W & initializing regiter
{


	if(ch == 0)
		g_pUartDebugRegs = UART0_BASE;
	else if(ch == 1)
		g_pUartDebugRegs = UART1_BASE;
	else if(ch == 2)
		g_pUartDebugRegs = UART2_BASE;
	else if(ch == 3)
		g_pUartDebugRegs = UART3_BASE;	


	switch(ch)
	{
		case 0 :
			GPIO_SetFunctionEach(eGPIO_A, eGPIO_0, 2); //Uart0 RXD
			GPIO_SetFunctionEach(eGPIO_A, eGPIO_1, 2); //Uart0 TXD			
			GPIO_SetFunctionEach(eGPIO_A, eGPIO_2, 2); //Uart0 CTSn			
			GPIO_SetFunctionEach(eGPIO_A, eGPIO_3, 2); //Uart0 RTSn			
			
			if(UART_InitializeREG(UART0_BASE, &g_AUartCon[0])  == SENDBREAK)
				return SENDBREAK; // Initialize register set for Uart 0

			break;
		case 1 :
			GPIO_SetFunctionEach(eGPIO_A, eGPIO_4, 2); //Uart1 RXD
			GPIO_SetFunctionEach(eGPIO_A, eGPIO_5, 2); //Uart1 TXD			
			GPIO_SetFunctionEach(eGPIO_A, eGPIO_6, 2); //Uart1 CTSn			
			GPIO_SetFunctionEach(eGPIO_A, eGPIO_7, 2); //Uart1 RTSn		

			if(UART_InitializeREG(UART1_BASE, &g_AUartCon[1]) == SENDBREAK)
				return SENDBREAK;

			break;
		case 2 :
			GPIO_SetFunctionEach(eGPIO_B, eGPIO_0, 2); //Uart2 RXD
			GPIO_SetFunctionEach(eGPIO_B, eGPIO_1, 2); //Uart2 TXD			

			if(UART_InitializeREG(UART2_BASE, &g_AUartCon[2]) == SENDBREAK)
				return SENDBREAK;

			break;
		case 3 :
			GPIO_SetFunctionEach(eGPIO_B, eGPIO_2, 2); //Uart2 RXD
			GPIO_SetFunctionEach(eGPIO_B, eGPIO_3, 2); //Uart2 TXD			

			if(UART_InitializeREG(UART3_BASE, &g_AUartCon[3]) == SENDBREAK)
				return SENDBREAK;

			break;
		default :
			return FALSE;
	}
	return TRUE;
}

//////////
// Function Name : UART_Open2
// Function Description : This function set up H/W(GPIO) and initialize SFR of UART for Testing
// Input : ch [0~4 UART channel]
// Output : g_pUartRxStr[ch] [charater type received string]
// Version : v0.1

u8 UART_Open2(u8 ch) // setting H/W & initializing regiter
{

	INTC_SetVectAddr(NUM_DMA0,Isr_Dma);
	
	if(ch == 0)
	{
		INTC_SetVectAddr(NUM_UART0,Isr_Uart0);
	}
	else if(ch == 1)
	{
		INTC_SetVectAddr(NUM_UART1,Isr_Uart1);
	}
	else if(ch == 2)
	{
		INTC_SetVectAddr(NUM_UART2,Isr_Uart2);
	}
	else if(ch == 3)
	{
		INTC_SetVectAddr(NUM_UART3,Isr_Uart3);
	}
	else
		return 0;

	switch(ch)
	{
		case 0 :
			GPIO_SetFunctionEach(eGPIO_A, eGPIO_0, 2); //Uart0 RXD
			GPIO_SetFunctionEach(eGPIO_A, eGPIO_1, 2); //Uart0 TXD			
			GPIO_SetFunctionEach(eGPIO_A, eGPIO_2, 2); //Uart0 CTSn			
			GPIO_SetFunctionEach(eGPIO_A, eGPIO_3, 2); //Uart0 RTSn			
			
			if(UART_InitializeREG(UART0_BASE, &g_AUartCon[0])  == SENDBREAK)
				return SENDBREAK; // Initialize register set for Uart 0

			break;
		case 1 :
			GPIO_SetFunctionEach(eGPIO_A, eGPIO_4, 2); //Uart1 RXD
			GPIO_SetFunctionEach(eGPIO_A, eGPIO_5, 2); //Uart1 TXD			
			GPIO_SetFunctionEach(eGPIO_A, eGPIO_6, 2); //Uart1 CTSn			
			GPIO_SetFunctionEach(eGPIO_A, eGPIO_7, 2); //Uart1 RTSn		

			if(UART_InitializeREG(UART1_BASE, &g_AUartCon[1]) == SENDBREAK)
				return SENDBREAK;

			break;
		case 2 :
			GPIO_SetFunctionEach(eGPIO_B, eGPIO_0, 2); //Uart2 RXD
			GPIO_SetFunctionEach(eGPIO_B, eGPIO_1, 2); //Uart2 TXD			

			if(UART_InitializeREG(UART2_BASE, &g_AUartCon[2]) == SENDBREAK)
				return SENDBREAK;

			break;
		case 3 :
			GPIO_SetFunctionEach(eGPIO_B, eGPIO_2, 2); //Uart2 RXD
			GPIO_SetFunctionEach(eGPIO_B, eGPIO_3, 2); //Uart2 TXD			

			if(UART_InitializeREG(UART3_BASE, &g_AUartCon[3]) == SENDBREAK)
				return SENDBREAK;

			break;
		default :
			return FALSE;
	}
	return TRUE;
}


//////////
// Function Name : UART_OpenModem
// Function Description : This function enable Modem Interrupt on VIC
// Input : ch [0~4 UART channel]
// Output : NONE
// Version : v0.1
void UART_OpenModem(u8 ch) 
{
	volatile UART_REGS *pUartRegs;
	u32 uTemp2;
	

	pUartRegs = (volatile UART_REGS *)(UART_BASE+UART_OFFSET*ch);
	
	while (Inp32(&pUartRegs->rUfStat )& 0x3f) 	//until rx fifo count 0 (fifo clear)
	{
	(u8)(Inp32(&pUartRegs->rUrxh));
	}	

		
		Outp32(&pUartRegs->rUintSp , (BIT_UART_MODEM|BIT_UART_TXD|BIT_UART_ERROR|BIT_UART_RXD));
		Outp32(&pUartRegs->rUintP , (BIT_UART_MODEM|BIT_UART_TXD|BIT_UART_ERROR|BIT_UART_RXD));

		uTemp2 = Inp32(&pUartRegs->rUintM);
		uTemp2 &= ~(BIT_UART_MODEM);	
		Outp32(&pUartRegs->rUintM,uTemp2);

		INTC_Enable(NUM_UART0);
		INTC_Enable(NUM_UART1);
		INTC_Enable(NUM_UART2);
		INTC_Enable(NUM_UART3);


}



//////////
// Function Name : UART_Close
// Function Description : This function closed H/W(GPIO) and disable UART exception
// Input : ch [0~4 UART channel]
// Output : 1: success 2:fail
// Version : v0.1

u8 UART_Close(u8 ch)  // return to reset value
{
	volatile UART_REGS *pUartRegs;

	if (ch > 3) return FALSE;
	pUartRegs = (volatile UART_REGS *)(UART_BASE+UART_OFFSET*ch);

	while(1)
	{
		if(UART_RdUTRSTAT(ch)&0x4)	//wait until tx buffer and shift register become empty
		{
			Delay(1000);	//because of pad delay
			break;
		}
	}

	INTC_Disable(NUM_DMA0);
	
	pUartRegs->rUintM |= (BIT_UART_MODEM|BIT_UART_TXD|BIT_UART_ERROR|BIT_UART_RXD);
	
	switch(ch)
	{
		case 0 :
			INTC_Disable(NUM_UART0);
			GPIO_SetFunctionEach(eGPIO_A, eGPIO_0, 0); //Uart0 RXD
			GPIO_SetFunctionEach(eGPIO_A, eGPIO_1, 0); //Uart0 TXD			
			GPIO_SetFunctionEach(eGPIO_A, eGPIO_2, 0); //Uart0 CTSn			
			GPIO_SetFunctionEach(eGPIO_A, eGPIO_3, 0); //Uart0 RTSn		
	
			break;
		case 1 :
			INTC_Disable(NUM_UART1);
			GPIO_SetFunctionEach(eGPIO_A, eGPIO_4, 0); //Uart1 RXD
			GPIO_SetFunctionEach(eGPIO_A, eGPIO_5, 0); //Uart1 TXD			
			GPIO_SetFunctionEach(eGPIO_A, eGPIO_6, 0); //Uart1 CTSn			
			GPIO_SetFunctionEach(eGPIO_A, eGPIO_7, 0); //Uart1 RTSn		
			break;
		case 2 :
			INTC_Disable(NUM_UART2);
			GPIO_SetFunctionEach(eGPIO_B, eGPIO_0, 0); //Uart2 RXD
			GPIO_SetFunctionEach(eGPIO_B, eGPIO_1, 0); //Uart2 TXD	
			break;
		case 3 :
			INTC_Disable(NUM_UART3);
			GPIO_SetFunctionEach(eGPIO_B, eGPIO_2, 0); //Uart3 RXD
			GPIO_SetFunctionEach(eGPIO_B, eGPIO_3, 0); //Uart3 TXD		
			break;
		default :
			UART_Printf("Can't close UARTx!! Select 0,1, or 2!!");
			return FALSE;
	}
	return TRUE;
}


//////////
// Function Name : UART_InitializeREG
// Function Description : This function set up SFR by pre-defined value 
// Input : *pUartRegs [SFR value base address]
//		 *pUartCon [Pre-defined value's base address, UART_Config]
// Output : SENDBREAK [when sendbreak signal set up]
//		    0 [nomal mode (sendbreak signal disable)]
// Version : v0.1

u32 UART_InitializeREG( UART_REGS *pUartRegs, UART_CON *pUartCon) // Initialize register set with current control set
{

	
	UART_CalcBaudrate(pUartRegs, pUartCon);

	Outp32(&pUartRegs->rUfCon , (pUartCon->cTxTrig<<6)|(pUartCon->cRxTrig<<4)|TX_FIFO_RESET|RX_FIFO_RESET
						|(pUartCon->cEnableFifo));
	
	Outp32(&pUartRegs->rUlCon , (pUartCon->cSelUartIrda<<6)|(pUartCon->cParityBit<<3)|(pUartCon->cStopBit<<2)
						|(pUartCon->cDataBit));
	Outp32(&pUartRegs->rUCon , (pUartCon->cOpClock<<10)|TX_INT_TYPE|RX_INT_TYPE|RX_ERR_INT_EN|RX_TIMEOUT_EN
						|(pUartCon->cLoopTest<<5)| (pUartCon->cSendBreakSignal<<4) |(pUartCon->cTxMode<<2)|(pUartCon->cRxMode));		
	if(pUartCon->cSendBreakSignal)
		return SENDBREAK;
	Outp32(&pUartRegs->rUmCon , (pUartCon->cRtsTrig<<5)|(pUartCon->cAfc<<4)|RTS_ACTIVE);	
	Outp32(&pUartRegs->rUintM , 0xf); // mask
	Outp32(&pUartRegs->rUintSp , 0xf); // source pending clear
	Outp32(&pUartRegs->rUintP , 0xf); // pending clear

	return 0;
	
}


//////////
// Function Name : UART_CalcBaudrate
// Function Description : This function defines UbrDiv and UdivSlot value depends on OpClock src
// Input : *pUartRegs [SFR value base address]
//		 *pUartCon [Pre-defined value's base address, UART_Config]
// Output : NONE
// Version : v0.1

void UART_CalcBaudrate( UART_REGS *pUartRegs, UART_CON *pUartCon)
{
	const u32 aSlotTable[16] = {0x0000, 0x0080, 0x0808, 0x0888, 0x2222, 0x4924, 0x4a52, 0x54aa,
				     		0x5555, 0xd555, 0xd5d5, 0xddd5, 0xdddd, 0xdfdd, 0xdfdf, 0xffdf};
	float tempDiv;
	u32 nOpClock;
	u32 nSlot;
	
	switch(pUartCon->cOpClock)
	{
		case 1 : 			
			nOpClock = g_uOpClock;
			break;// Uextclk
		case 3 :
			nOpClock = g_uOpClock;			
			break;// Epll
		case 0 :
		case 2 : // Pclk
		default : 			
			nOpClock = g_PCLK;			
			break;
	}

	tempDiv = (nOpClock/(16.*pUartCon->uBaudrate)) - 1;
	nSlot = (u32)(((tempDiv - (int)tempDiv) * 16)+0.5);
	Outp32(&pUartRegs->rUbrDiv , (u32)(tempDiv));
	Outp32(&pUartRegs->rUdivSlot , aSlotTable[nSlot]);	
}


//////////
// Function Name : Isr_Uart0~3
// Function Description : These are Interrupt Service Routine of UART which are connected to Isr_UartSub
// Input : NONE
// Output : NONE
// Version : v0.1

void __irq Isr_Uart0(void)
{		
	
	Isr_UartSub(0);	
	INTC_ClearVectAddr();
}		

void __irq Isr_Uart1(void)
{	
	Isr_UartSub(1);
	INTC_ClearVectAddr();
}

void __irq Isr_Uart2(void)
{	
	Isr_UartSub(2);
	INTC_ClearVectAddr();
}

void __irq Isr_Uart3(void)
{	
	Isr_UartSub(3);
	INTC_ClearVectAddr();	
}	


//////////
// Function Name : Isr_UartSub
// Function Description : This is Uart SubISR which checks errors and operates Tx and Rx
// Input : NONE
// Output : NONE
// Version : v0.1

void Isr_UartSub(u8 ch)
{
	u32 ErrorStatus = 0;	
	volatile UART_REGS *pUartRegs;
	u32 uTemp;

	pUartRegs = ( UART_REGS *)(UART_BASE+UART_OFFSET*ch);
	
	// Check Errors 	
	if (Inp32(&pUartRegs->rUintP) & BIT_UART_ERROR)
	{
		UART_Printf("\nError occurred!!\n");
		uTemp = Inp32(&pUartRegs->rUintM);
		uTemp |= BIT_UART_ERROR;
		Outp32(&pUartRegs->rUintM , uTemp);
		
		Outp32(&pUartRegs->rUintSp , BIT_UART_ERROR);
		Outp32(&pUartRegs->rUintP , BIT_UART_ERROR);
		
		ErrorStatus = Inp32(&pUartRegs->rUerStat);

		switch(ErrorStatus)//to clear and check the status of register bits
		{
			case 1:
				UART_Printf("%d ch : Overrun error!\n",ch);
				break;
			case 2:
				UART_Printf("%d ch : Parity error!\n",ch);
				break;
			case 4:
				UART_Printf("%d ch : Frame error!\n",ch);
				break;
			case 6:
				UART_Printf("%d ch : Parity, Frame error!\n",ch);
				break;
			case 8:
				UART_Printf("%d ch : Breake detect\n",ch);
				break;
			case 0xa:
				UART_Printf("%d ch : Parity error & Break detect!\n",ch);
				break;
			case 0xc:
				UART_Printf("%d ch : Frame error & Break detect\n",ch);
				break;
			case 0xe:
				UART_Printf("%d ch : Parity, Frame error & Break detect!\n",ch);
				break;
			default :
				UART_Printf("Unknown error : 0x%x\n", ErrorStatus);
				break;
		}
		
		g_sRxStringResult.result_ok = FALSE;	

		//uTemp = Inp32(pUartRegs->rUintM );
		//uTemp &= ~(BIT_UART_ERROR);
		//Outp32(&pUartRegs->rUintM ,uTemp);		
	}

	// Tx ISR	
	if (Inp32(&pUartRegs->rUintP )& BIT_UART_TXD)
	{
		uTemp = Inp32(&pUartRegs->rUintM);
		uTemp |= BIT_UART_TXD;
		Outp32(&pUartRegs->rUintM , uTemp);
		Outp32(&pUartRegs->rUintSp , BIT_UART_TXD);
		Outp32(&pUartRegs->rUintP , BIT_UART_TXD);

		if (Inp32(&pUartRegs->rUfCon )& 1) // 1 : fifo enable 
		{
			// for debugging fifo 
			*g_pFifoDebug++ = ++g_uFcnt;
			*g_pFifoDebug++ = (Inp32(&pUartRegs->rUfCon)>>6)&3;
			*g_pFifoDebug++ = (Inp32(&pUartRegs->rUfStat)>>8)&0x3f;
			*g_pFifoDebug = 0;
			
			while (!(Inp32(&pUartRegs->rUfStat )& (1<<14)) && (*g_pUartTxStr[ch] != TX_END_CHAR)) 	//until tx fifo full or end of string
				Outp32(&pUartRegs->rUtxh , *g_pUartTxStr[ch]++);

			if(*g_pUartTxStr[ch] == TX_END_CHAR) 
			{
				*(g_pUartTxStr[ch]+1) = NULL;
				g_AisTxDone[ch] = 1;	
				return;
			}
			else 
			{
				uTemp = Inp32(&pUartRegs->rUintM);
				uTemp &= ~( BIT_UART_TXD);
				Outp32(&pUartRegs->rUintM , uTemp);
			}
		}
		else // 0 : fifo disable
		{
			if (*g_pUartTxStr[ch] != TX_END_CHAR)
			{
				Outp32(&pUartRegs->rUtxh , *g_pUartTxStr[ch]++);
				uTemp = Inp32(&pUartRegs->rUintM);
				uTemp &= ~( BIT_UART_TXD);
				Outp32(&pUartRegs->rUintM , uTemp);
			}
			else
			{
				*(g_pUartTxStr[ch]+1) = NULL;
				g_AisTxDone[ch] = 1;	
				return;
			}
		}
	}

	// Rx ISR	
	if (Inp32(&pUartRegs->rUintP) & BIT_UART_RXD)
	{
		uTemp = Inp32(&pUartRegs->rUintM);
		uTemp |= BIT_UART_RXD;
		Outp32(&pUartRegs->rUintM , uTemp);
		
		Outp32(&pUartRegs->rUintSp , BIT_UART_RXD);
		Outp32(&pUartRegs->rUintP , BIT_UART_RXD);

		if (pUartRegs->rUfCon & 1) // 1 : fifo enable 
		{
			// for debugging fifo 
			*g_pFifoDebug++ = ++g_uFcnt;
			*g_pFifoDebug++ = (Inp32(&pUartRegs->rUfCon)>>4)&3;
			*g_pFifoDebug++ = (Inp32(&pUartRegs->rUfStat))&0x3f;
			*g_pFifoDebug = 0;

			while (Inp32(&pUartRegs->rUfStat )& 0x3f) 	//until rx fifo count 0
			{
				*g_pUartRxStr[ch]++ = (u8)(Inp32(&pUartRegs->rUrxh));
			}
			
			if(*(g_pUartRxStr[ch]-1) == RX_END_CHAR) 
			{
				*g_pUartRxStr[ch] = NULL;
				g_AisRxDone[ch] = 1;
				return;
			}
			else 
			{
				uTemp = Inp32(&pUartRegs->rUintM);
				uTemp &= ~( BIT_UART_RXD);
				Outp32(&pUartRegs->rUintM , uTemp);
			}
		}
		else // 0 : fifo disable
		{
			*g_pUartRxStr[ch]++ = (u8)(Inp32(&pUartRegs->rUrxh));
			
			if(*(g_pUartRxStr[ch]-1) == RX_END_CHAR) 
			{
				*g_pUartRxStr[ch] = NULL;
				g_AisRxDone[ch] = 1;
				return;
			}
			else 
			{
				uTemp = Inp32(&pUartRegs->rUintM);
				uTemp &= ~( BIT_UART_RXD);
				Outp32(&pUartRegs->rUintM , uTemp);
			}
		}
	}

	// Modem UART ISR	
	if ((Inp32(&pUartRegs->rUintP ))& BIT_UART_MODEM)
	{

		uTemp = Inp32(&pUartRegs->rUintM);
		uTemp |= BIT_UART_MODEM;
		Outp32(&pUartRegs->rUintM , uTemp);
		Outp32(&pUartRegs->rUintSp , BIT_UART_MODEM);
		Outp32(&pUartRegs->rUintP , BIT_UART_MODEM);

		// Implementation for modem uart
		UART_Printf("Modem Int Occured!\n");

		uTemp = Inp32(&pUartRegs->rUintM);
		uTemp &= ~( BIT_UART_MODEM);
		Outp32(&pUartRegs->rUintM , uTemp);
	}

	// unknown UART interrupt 
	if (Inp32(&pUartRegs->rUintP )& ~(BIT_UART_MODEM|BIT_UART_TXD|BIT_UART_ERROR|BIT_UART_RXD))
		UART_Printf("UARTx sub INT - unknown sub interrupt!!\n");


}



//////////
// Function Name : Isr_Dma
// Function Description : This is Uart DMA request ISR which operates DMA request
// Input : NONE
// Output : NONE
// Version : v0.1

void __irq Isr_Dma(void) // only one for test
{

	u32 uStatus;
	u32 uConfig;	
	u8 ch;
	
	INTC_Disable(NUM_DMA0);

	uStatus = DMAC_IntStatus(&oUARTDma);
	
	if (uStatus&0x1)    // Tx
	{
		DMACH_Stop(&oUARTDma);
		DMACH_ClearIntPending(&oUARTDma);
		DMACH_ClearErrIntPending(&oUARTDma);

		uConfig = DMACH_Configuration(&oUARTDma);
		ch = ((uConfig & 0x3c0)>>6)>>1;	//find UART ch fr request src

		g_AisTxDone[ch] = 1;
	}	

	else if (uStatus&0x2)    // Rx
	{
		DMACH_Stop(&oUARTDma);

		DMACH_ClearIntPending(&oUARTDma);
		DMACH_ClearErrIntPending(&oUARTDma);

		uConfig = DMACH_Configuration(&oUARTDma);
		ch = (((uConfig & 0x1e)>>1)-1)>>1;	//find UART ch fr request src
		
		g_AisRxDone[ch] = 1;
		*(g_pUartRxStr[ch]+g_uUartRxDmaTc) = NULL; // added null after rx string

	}
	
	INTC_ClearVectAddr();			

}





//////////
// Function Name : UART_SetConfig
// Function Description : This function set up UART by pre-defined value (input)
// Input :
//		Channel(0~3)
//		Other Options 0. Nothing[D]  1.Send Break Signal  2. Loop Back Mode
//		Parity Mode 1. No parity[D] 2. Odd 3. Even 4. Forced as '1' 5. Forced as '0'
//		Number of Stop Bit 1. One stop bit per frame[D] 2. Two stop bit per frame
//		Word Length 1. 5bits 2. 6bits 3. 7bits 4. 8bits
//		Operating Clock 1. PCLK[D]	2. UEXTCLK	3. PLL
//            PLL selection  1.EPLL    2.MPLL
//		External Interface Type 1. UART[D]   2. IrDA mode
//		Baudrate (ex 9600, 115200[D], 921600)
//		Operating Mode 1. Interrupt[D]	2. DMA
//		FIFO Mode (Tx/Rx[byte]) 1. no FIFO[D]  2. Empty/1  3. 16/8   4. 32/16  5. 48/32
//		AFC Mode 1. Disable[D]	2. Enable
//		nRTS trigger level(byte) 1. 63[D]   2. 56   3. 48   4. 40   5. 32   6. 24   7. 16   8. 8
// Output : NONE
// Version : v0.1
// default UART channel setting is UART_SetConfig(0,0,1,1,4,1,2,1,115200,1,1,1,1)

u8 UART_SetConfig(u8 cCh, u32 uBreakorLoop,u32 uParity,u32 uNumStop,u32 uWordLength,u32 uOpCLK,u32 uSelPLL, 
				u32 uExtIFtype,u32 uBaudrate,u32 uSelOpmode,u32 uSelFIFO,u32 uSelAFC,u32 uRTSLvL)
{

	volatile UART_CON *pUartCon;

	pUartCon = &g_AUartCon[cCh];	

	
	//Set Other Options
//	UART_Printf("\nSelect Other Options\n 0. Nothing[D]  1.Send Break Signal  2. Loop Back Mode  \n Choose : ");
	switch(uBreakorLoop)
	{		
		default : 	
					pUartCon->cSendBreakSignal = 0x0;
					pUartCon->cLoopTest = 0x0;
					break;
					
		case 1 :		
					pUartCon->cSendBreakSignal = 1;	
					return cCh;
					
					
		case 2 :	 	
					pUartCon->cLoopTest = 1;		
					break;		
	}		
	
	
	//Set Parity mode
//	UART_Printf("\nSelect Parity Mode\n 1. No parity[D] 2. Odd 3. Even 4. Forced as '1' 5. Forced as '0' \n Choose : ");
	switch(uParity)
	{		
		default : 	
			pUartCon->cParityBit = 0;		
			break;
		case 2 : 		
			pUartCon->cParityBit = 4;
			break;
		case 3 :		
			pUartCon->cParityBit = 5;
			break;
		case 4 :		
			pUartCon->cParityBit = 6;
			break;
		case 5 :		
			pUartCon->cParityBit = 7;
			break;
	}
	//Set the number of stop bit	
//	UART_Printf("\n\nSelect Number of Stop Bit\n 1. One stop bit per frame[D] 2. Two stop bit per frame");
	switch(uNumStop)
	{
		default : 	
			pUartCon->cStopBit = 0;
			break;
			
		case 2 : 		
			pUartCon->cStopBit = 1;
			break;		
	}
	//Set Word Length	
//	UART_Printf("\n\nSelect Word Length\n 1. 5bits 2. 6bits 3. 7bits 4. 8bits[D] \n Choose : ");
	switch(uWordLength)
	{		
		case 1 :		
			pUartCon->cDataBit = 0;
			break;
		case 2 :		
			pUartCon->cDataBit = 1;
			break;
		case 3 :		
			pUartCon->cDataBit = 2;		
			break;	
		default :		
			pUartCon->cDataBit = 3;
			break;
	}
	
	
	// Set Operation clock
//	UART_Printf("\n\nSelect Operating Clock\n 1. PCLK[D]	2. UEXTCLK	3. EPLL \n Choose : ");
	switch (uOpCLK)
	{
		case 2 :
			pUartCon->cOpClock = 1;
			// connect CLKOUT and UEXTCLK
			UART_Printf("\nInput PWM EXT_CLK by Pulse Generater\n");
			UART_Printf("How much CLK do you input through the pwmECLK?");
			UART_Printf("Mhz : ");
			g_uOpClock = UART_GetIntNum()*1000000;
			GPIO_SetFunctionEach(eGPIO_F,eGPIO_13,2);		
			break;

		case 3 :
			pUartCon->cOpClock = 3;
			UART_Printf("\nSelect Clock SRC\n 1.EPLL  2.MPLL \n Choose: ");
			
			switch(UART_GetIntNum())
			{
				case 1:
					SYSC_SetPLL(eEPLL,32,1,1,0);   //EPLL=192Mhz
					SYSC_ClkSrc(eEPLL_FOUT);
					SYSC_ClkSrc(eUART_MOUTEPLL);
					SYSC_SetDIV2(0,0,0,0,g_ucUartOpClk_DivRatio,0);	//5th argument : uart clock divider
					Delay(100);
					g_uOpClock = CalcEPLL(32,1,1,0)/(g_ucUartOpClk_DivRatio+1);
					UART_Printf("UART Op Clock = %.2fMhz\n",((float)g_uOpClock/1000000));
					// use EPLL output clock
					//CLK_SRC	UART_SEL[13]  0:EPLL
					//CLK_DIV2	UART_RATIO[19:16]
					break;
					
				case 2:
					SYSC_ClkSrc(eMPLL_FOUT);						
					SYSC_ClkSrc(eUART_DOUTMPLL);
					SYSC_SetDIV2(0,0,0,0,g_ucUartOpClk_DivRatio,0);	//5th argument : uart clock divider
					Delay(100);
					g_uOpClock = (u32)g_MPLL/(((SYSC_GetDIV0()>>4)&0x1)+1)/(g_ucUartOpClk_DivRatio+1);
					UART_Printf("UART Op Clock = %.2fMhz\n",((float)g_uOpClock/1000000));
					// use MPLL output clock
					//CLK_SRC	UART_SEL[13]  1:MPLL
					//CLK_DIV2	UART_RATIO[19:16]
					break;
					
				default:
					SYSC_SetPLL(eEPLL,32,1,1,0);   //EPLL=192Mhz
					SYSC_ClkSrc(eEPLL_FOUT);
					SYSC_ClkSrc(eUART_MOUTEPLL);
					SYSC_SetDIV2(0,0,0,0,g_ucUartOpClk_DivRatio,0);	//5th argument : uart clock divider
					Delay(100);
					g_uOpClock = CalcEPLL(32,1,1,0)/(g_ucUartOpClk_DivRatio+1);
					UART_Printf("UART Op Clock = %.2fMhz\n",((float)g_uOpClock/1000000));
					// use EPLL output clock
					//CLK_SRC	UART_SEL[13]  0:EPLL
					//CLK_DIV2	UART_RATIO[19:16]
					break;								
			}
			break;

		default :
			pUartCon->cOpClock = 2; // PCLK		
			break;
	}

	// Select UART or IrDA 1.0
//	UART_Printf("\n\nSelect External Interface Type\n 1. UART[D]   2. IrDA mode\n Choose : ");
	if (uExtIFtype==2)		
		pUartCon->cSelUartIrda = 1; // IrDA mode
	else					
		pUartCon->cSelUartIrda = 0; // IrDA mode

	// Set Baudrate
//	UART_Printf("\n\nType the baudrate and then change the same baudrate of host, too.\n");
//	UART_Printf(" Baudrate (ex 9600, 115200[D], 921600) : ");
	pUartCon->uBaudrate = uBaudrate;

//	if ((s32)pUartCon->uBaudrate == -1) 
//		pUartCon->uBaudrate = 115200;

	// Select UART operating mode
//	UART_Printf("\n\nSelect Operating Mode\n 1. Interrupt[D]	2. DMA\n Choose : ");
	if (uSelOpmode==2)
	{
		pUartCon->cTxMode = 2; // DMA0 mode
		pUartCon->cRxMode = 3; // DMA1 mode
	}
	else
	{	
		pUartCon->cTxMode = 1; // Int mode
		pUartCon->cRxMode = 1; // Int mode
	}

	// Select UART FIFO mode
//	UART_Printf("\n\nSelect FIFO Mode (Tx/Rx[byte])\n 1. no FIFO[D]  2. Empty/1  3. 16/8   4. 32/16  5. 48/32 \n Choose : ");

	if ( (uSelFIFO>1)&&(uSelFIFO<6) )
	{
		pUartCon->cEnableFifo = 1;
		pUartCon->cTxTrig = uSelFIFO -2;
		pUartCon->cRxTrig = uSelFIFO -2;
	}
	else 
	{
		pUartCon->cEnableFifo = 0;
	}

	// Select AFC mode enable/disable
//	UART_Printf("\n\nSelect AFC Mode\n 1. Disable[D]	2. Enable\n Choose : ");
	if (uSelAFC == 2)
	{
		pUartCon->cAfc = 1; // AFC mode enable
//		UART_Printf("Select nRTS trigger level(byte)\n 1. 63[D]   2. 56   3. 48   4. 40   5. 32   6. 24   7. 16   8. 8\n Choose : ");
		
		
		if ( (uRTSLvL>1)&&(uRTSLvL<9) )	
			pUartCon->cRtsTrig = uRTSLvL -1;

		else						
			pUartCon->cRtsTrig = 0; // default 63 byte
	}
	else
	{
		pUartCon->cAfc = 0; // AFC mode disable
	}

	return cCh;
}


//////////
// Function Name : UART_SetConfig
// Function Description : This function set up UART by pre-defined value (input)
// Input :
//		Channel(0~3)
//		Other Options 0. Nothing[D]  1.Send Break Signal  2. Loop Back Mode
//		Parity Mode 1. No parity[D] 2. Odd 3. Even 4. Forced as '1' 5. Forced as '0'
//		Number of Stop Bit 1. One stop bit per frame[D] 2. Two stop bit per frame
//		Word Length 1. 5bits 2. 6bits 3. 7bits 4. 8bits
//		External Interface Type 1. UART[D]   2. IrDA mode
//		Baudrate (ex 9600, 115200[D], 921600)
//		Operating Mode 1. Interrupt[D]	2. DMA
//		FIFO Mode (Tx/Rx[byte]) 1. no FIFO[D]  2. Empty/1  3. 16/8   4. 32/16  5. 48/32
//		AFC Mode 1. Disable[D]	2. Enable
//		nRTS trigger level(byte) 1. 63[D]   2. 56   3. 48   4. 40   5. 32   6. 24   7. 16   8. 8
// Output : NONE
// Version : v0.1
// default UART channel setting is UART_SetConfig(0,0,1,1,4,1,2,1,115200,1,1,1,1)

u8 UART_SetConfig_2(u8 cCh, u32 uBreakorLoop,u32 uParity,u32 uNumStop,u32 uWordLength, 
				u32 uExtIFtype,u32 uBaudrate,u32 uSelOpmode,u32 uSelFIFO,u32 uSelAFC,u32 uRTSLvL)
{

	volatile UART_CON *pUartCon;

	pUartCon = &g_AUartCon[cCh];	

	
	//Set Other Options
//	UART_Printf("\nSelect Other Options\n 0. Nothing[D]  1.Send Break Signal  2. Loop Back Mode  \n Choose : ");
	switch(uBreakorLoop)
	{		
		default : 	
					pUartCon->cSendBreakSignal = 0x0;
					pUartCon->cLoopTest = 0x0;
					break;
					
		case 1 :		
					pUartCon->cSendBreakSignal = 1;	
					return cCh;
					
					
		case 2 :	 	
					pUartCon->cLoopTest = 1;		
					break;		
	}		
	
	
	//Set Parity mode
//	UART_Printf("\nSelect Parity Mode\n 1. No parity[D] 2. Odd 3. Even 4. Forced as '1' 5. Forced as '0' \n Choose : ");
	switch(uParity)
	{		
		default : 	
			pUartCon->cParityBit = 0;		
			break;
		case 2 : 		
			pUartCon->cParityBit = 4;
			break;
		case 3 :		
			pUartCon->cParityBit = 5;
			break;
		case 4 :		
			pUartCon->cParityBit = 6;
			break;
		case 5 :		
			pUartCon->cParityBit = 7;
			break;
	}
	//Set the number of stop bit	
//	UART_Printf("\n\nSelect Number of Stop Bit\n 1. One stop bit per frame[D] 2. Two stop bit per frame");
	switch(uNumStop)
	{
		default : 	
			pUartCon->cStopBit = 0;
			break;
			
		case 2 : 		
			pUartCon->cStopBit = 1;
			break;		
	}
	//Set Word Length	
//	UART_Printf("\n\nSelect Word Length\n 1. 5bits 2. 6bits 3. 7bits 4. 8bits[D] \n Choose : ");
	switch(uWordLength)
	{		
		case 1 :		
			pUartCon->cDataBit = 0;
			break;
		case 2 :		
			pUartCon->cDataBit = 1;
			break;
		case 3 :		
			pUartCon->cDataBit = 2;		
			break;	
		default :		
			pUartCon->cDataBit = 3;
			break;
	}

	pUartCon->cOpClock = 3;

	// Select UART or IrDA 1.0
//	UART_Printf("\n\nSelect External Interface Type\n 1. UART[D]   2. IrDA mode\n Choose : ");
	if (uExtIFtype==2)		
		pUartCon->cSelUartIrda = 1; // IrDA mode
	else					
		pUartCon->cSelUartIrda = 0; // IrDA mode

	// Set Baudrate
//	UART_Printf("\n\nType the baudrate and then change the same baudrate of host, too.\n");
//	UART_Printf(" Baudrate (ex 9600, 115200[D], 921600) : ");
	pUartCon->uBaudrate = uBaudrate;

//	if ((s32)pUartCon->uBaudrate == -1) 
//		pUartCon->uBaudrate = 115200;

	// Select UART operating mode
//	UART_Printf("\n\nSelect Operating Mode\n 1. Interrupt[D]	2. DMA\n Choose : ");
	if (uSelOpmode==2)
	{
		pUartCon->cTxMode = 2; // DMA0 mode
		pUartCon->cRxMode = 3; // DMA1 mode
	}
	else
	{	
		pUartCon->cTxMode = 1; // Int mode
		pUartCon->cRxMode = 1; // Int mode
	}

	// Select UART FIFO mode
//	UART_Printf("\n\nSelect FIFO Mode (Tx/Rx[byte])\n 1. no FIFO[D]  2. Empty/1  3. 16/8   4. 32/16  5. 48/32 \n Choose : ");

	if ( (uSelFIFO>1)&&(uSelFIFO<6) )
	{
		pUartCon->cEnableFifo = 1;
		pUartCon->cTxTrig = uSelFIFO -2;
		pUartCon->cRxTrig = uSelFIFO -2;
	}
	else 
	{
		pUartCon->cEnableFifo = 0;
	}

	// Select AFC mode enable/disable
//	UART_Printf("\n\nSelect AFC Mode\n 1. Disable[D]	2. Enable\n Choose : ");
	if (uSelAFC == 2)
	{
		pUartCon->cAfc = 1; // AFC mode enable
//		UART_Printf("Select nRTS trigger level(byte)\n 1. 63[D]   2. 56   3. 48   4. 40   5. 32   6. 24   7. 16   8. 8\n Choose : ");
		
		
		if ( (uRTSLvL>1)&&(uRTSLvL<9) )	
			pUartCon->cRtsTrig = uRTSLvL -1;

		else						
			pUartCon->cRtsTrig = 0; // default 63 byte
	}
	else
	{
		pUartCon->cAfc = 0; // AFC mode disable
	}

	return cCh;
}


//////////
// Function Name : CalcEPLL
// Function Description : This is Uart DMA request ISR which operates DMA request
// Input : NONE
// Output : NONE
// Version : v0.1

u32 CalcEPLL(u32 uMdiv,u32 uPdiv,u32 uSdiv,u32 uKdiv)
{
	float EPLL;

	EPLL=(((uMdiv+((float)uKdiv/(1<<16)))*FIN)/(uPdiv*(1<<uSdiv)));

	return (u32)EPLL;
}


//////////
//
void UART_SetEpll_forUART(u32 uMdiv,u32 uPdiv,u32 uSdiv,u32 uKdiv)
{
	SYSC_SetPLL(eEPLL,uMdiv,uPdiv,uSdiv,uKdiv);   //EPLL=192Mhz
	SYSC_ClkSrc(eEPLL_FOUT);
	SYSC_ClkSrc(eUART_MOUTEPLL);
	SYSC_SetDIV2(0,0,0,0,g_ucUartOpClk_DivRatio,0);	//5th argument : uart clock divider
	Delay(100);
	g_uOpClock = CalcEPLL(uMdiv,uPdiv,uSdiv,uKdiv)/(g_ucUartOpClk_DivRatio+1);
	UART_Printf("UART Op Clock = %.2fMhz\n",((float)g_uOpClock/1000000));
	
	return;
}

