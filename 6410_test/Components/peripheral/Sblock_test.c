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
*	File Name : sblock_test.c
*  
*	File Description : This file implements the functons for security sub-system test.
*
*	Author : Wonjoon Jang
*	Dept. : AP Development Team
*	Created Date : 2007/01/17
*	Version : 0.1 
* 
*	History
*	- Created(wonjoon.jang 2007/01/17)
*  
**************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#include "def.h"
#include "option.h"
#include "library.h"
#include "sfr6410.h"
#include "system.h"
#include "intc.h"
#include "gpio.h"
#include "sysc.h"
#include "timer.h"
#include "sblock.h"
#include "sblock_data.h"



void SBLOCK_Test(void);

// must be changed
#define INPUT_BUF			(0x52000000)
#define INPUT_BUF1			(0x52100000)
#define OUTPUT_BUF			(0x52400000)
#define OUTPUT_BUF1		(0x52500000)
#define GOLDEN_BUF			(0x52800000)
#define GOLDEN_BUF1		(0x52810000)
#define MID_BUF				(0x52C00000)

#define Sblock_DEBUG		(false)

//static SBLK oSblk;
//static INTC oIntc;

static OPER_MODE eAesOperMode;
static OPER_MODE eDesOperMode;
static OPER_MODE eHashOperMode;
static u32 g_SDma1Done;

volatile int  g_AES_Key;
volatile int g_sblock_err;

void Init_BUF(u32 da, u32 words)
{
	u32 i;	
	for (i=0; i<words; i++)
		*(u32 *)(da+i*4) = 0;
}


//////////
// Function Name : Isr_SDMA0
// Function Description : Isr routine
// Input : 	None
// Output :	None 
// Version : v0.1
void __irq Isr_SDMA1(void)
{
	u32 uRead;
	//for Test
	INTC_Disable(NUM_SDMA1);
	uRead = Inp32(0x7DC00000);	//SDMA1 INT STATUS
	
	uRead = (uRead>>6)&0x3;
	if(uRead== 1)
	{
		SBLK_ClearIntPending(0);
		//UART_Printf("		SDMA1 Rx ISR Occurred\n");
	}
	else if(uRead== 2)
	{
		SBLK_ClearIntPending(1);
		g_SDma1Done = 1;
		//UART_Printf("		SDMA1 Tx ISR Occurred\n");
	}
	else
	{
		//UART_Printf("		SDMA1 Rx/Tx ISR Occurred\n");
		SBLK_ClearIntPending(1);
		SBLK_ClearIntPending(0);
		g_SDma1Done = 1;
	}
/*
	SBLK_ClearIntPending(0);
	//Delay(1);
	//UART_Printf("		SDMA1 Int 0x%x \n", uRead);
	//UART_Printf("		SDMA1 ISR Occurred\n");
	g_SDma1Done = 1;
*/
	INTC_ClearVectAddr();
	INTC_Enable(NUM_SDMA1);
}

//////////
// Function Name : DispOperMode
// Function Description : Display operating mode
// Input : 	None
// Output :	None 
// Version : v0.1
void DispOperMode(OPER_MODE eOperMode)
{
	if (eOperMode == ECB) UART_Printf("ECB");
	else if (eOperMode == CBC) UART_Printf("CBC");
	else if (eOperMode == CTR) UART_Printf("CTR");
	else if (eOperMode == HMAC) UART_Printf("HMAC");
	else if (eOperMode == SHA1) UART_Printf("SHA1");
	else if (eOperMode == PRNG) UART_Printf("PRNG");
}

void SetAesOperMode(void)
{
	int sel;

	UART_Printf("Select Operation Mode\n");
	UART_Printf("1)ECB  2)CBC 3) CTR : \n");
	sel=UART_GetIntNum();
	if (sel == 1) eAesOperMode = ECB;
	else if (sel == 2) eAesOperMode = CBC;
	else if (sel == 3) eAesOperMode = CTR;

	UART_Printf("Your selection is ");
	DispOperMode(eAesOperMode);
	//UART_Printf("\n");
}
void SetDesOperMode(void)
{
	int sel;

	UART_Printf("Select Operation Mode\n");
	UART_Printf("1)ECB  2)CBC : \n");
	sel=UART_GetIntNum();
	if (sel == 1) eDesOperMode = ECB;
	else if (sel == 2) eDesOperMode = CBC;

	UART_Printf("Your selection is ");
	DispOperMode(eDesOperMode);
	//UART_Printf("\n");
}

void SetHashOperMode(void)
{
	int sel;

	UART_Printf("Select Operation Mode\n");
	UART_Printf("1)HMAC  2)SHA1  3)PRNG  : \n");
	sel=UART_GetIntNum();
	if (sel == 1) eHashOperMode = HMAC;
	else if (sel == 2) eHashOperMode = SHA1;
	else if (sel == 3) eHashOperMode = PRNG;

	UART_Printf("Your selection is ");
	DispOperMode(eHashOperMode);
	//UART_Printf("\n");
}

//////////
// Function Name : Test_AES_CPU
// Function Description : AES CPU mode Test
// Input : 	None
// Output :	None 
// Version : v0.1

void Test_AES_CPU(eFunction_Test eTest, oFunctionT_AutoVar oAutoVar)
//void Test_AES_CPU(void)
{
	u32 uRet, i, usel;
	//u8 uError;
	//g_sblock_err=0;
	
	if(eTest == eTest_Manual)
	{
		UART_Printf("[SBLOCK - AES CPU Mode Test]\n");

		UART_Printf("Select AES Operating Mode  : 1)ECB   2)CBC   3)CTR \n");
		usel=UART_GetIntNum();
		if (usel == 1) eAesOperMode = ECB;
		else if (usel == 2) eAesOperMode = CBC;
		else if (usel == 3) eAesOperMode = CTR;

		UART_Printf("Your selection is ");
		DispOperMode(eAesOperMode);
		UART_Printf("\n");
	}
	else
	{
		eAesOperMode = (OPER_MODE)oAutoVar.Test_Parameter[0];
	}
	
	//////////////////////////
	////// Encryption
	SBLK_Init(AES, eAesOperMode, ENC);
	
	for (i=0; i<4; i++)
	{
		SBLK_PutDataToInReg((u32)uAesPlainText+i*4*4, 4, LASTBYTE_1ST);
		SBLK_StartByManual();

		while(!SBLK_IsOutputReady());

		SBLK_GetDataFromOutReg(OUTPUT_BUF+i*4*4, 4);
	}
	Copy((u32)uAesCipherText[eAesOperMode], GOLDEN_BUF, 16);
	uRet = Compare(OUTPUT_BUF, GOLDEN_BUF, 16);

	if(eTest == eTest_Manual)
	{
		if (uRet)
		{
		UART_Printf("AES_");
		DispOperMode(eAesOperMode);
		UART_Printf("_CPU_ENC Success..\n");
		}
		else
		{
		UART_Printf("AES");
		DispOperMode(eAesOperMode);
		UART_Printf("_CPU_ENC Failed..\n");
		}
	}
	else
	{
		if(uRet == false)
			{
				UART_Printf("AES");
				DispOperMode(eAesOperMode);
				UART_Printf("_CPU_ENC Failed..\n");
				g_sblock_err =1;
				UART_Getc();
			}
		else if (Sblock_DEBUG)
			{
				UART_Printf("AES_");
				DispOperMode(eAesOperMode);
				UART_Printf("_CPU_ENC Success..\n");
			}
	}
	
	/////////////////////////
	////// Decryption
	if (eAesOperMode == CTR)
		SBLK_Init(AES, eAesOperMode, ENC);
	else
		SBLK_Init(AES, eAesOperMode, DEC);

	for (i=0; i<4; i++)
	{
		SBLK_PutDataToInReg(OUTPUT_BUF+i*4*4, 4, LASTBYTE_1ST);
		SBLK_StartByManual();

		while(!SBLK_IsOutputReady());

		SBLK_GetDataFromOutReg(INPUT_BUF+i*4*4, 4);
	}
	Copy((u32)uAesPlainText, GOLDEN_BUF, 16);
	uRet = Compare(INPUT_BUF, GOLDEN_BUF, 16);
	if(eTest == eTest_Manual)
	{
		if (uRet)
		{
		UART_Printf("AES_");
		DispOperMode(eAesOperMode);
		UART_Printf("_CPU_DEC Success..\n\n");
		}
		else
		{
		UART_Printf("AES");
		DispOperMode(eAesOperMode);
		UART_Printf("_CPU_DEC Failed..\n\n");
		}
	}
	else
	{
		if(uRet == false)
			{
				UART_Printf("AES");
				DispOperMode(eAesOperMode);
				UART_Printf("_CPU_DEC Failed..\n");
				g_sblock_err =1;
				UART_Getc();
			}
		else if (Sblock_DEBUG)
			{
				UART_Printf("AES_");
				DispOperMode(eAesOperMode);
				UART_Printf("_CPU_DEC Success..\n");
			}
	}

	
}

//////////
// Function Name : Test_AES_FIFO
// Function Description : AES FIFO mode Test
// Input : 	None
// Output :	None 
// Version : v0.1
void Test_AES_FIFO(eFunction_Test eTest, oFunctionT_AutoVar oAutoVar)
{
	u32 uRet, usel;

	//g_sblock_err=0;

	if(eTest == eTest_Manual)
	{
		UART_Printf("[SBLOCK - AES FIFO Mode Test]\n");

		UART_Printf("Select AES Operating Mode  : 1)ECB   2)CBC   3)CTR \n");
		usel=UART_GetIntNum();
		if (usel == 1) eAesOperMode = ECB;
		else if (usel == 2) eAesOperMode = CBC;
		else if (usel == 3) eAesOperMode = CTR;

		UART_Printf("Your selection is ");
		DispOperMode(eAesOperMode);
		UART_Printf("\n");
	}
	else
	{
		eAesOperMode = (OPER_MODE)oAutoVar.Test_Parameter[0];
	}
	
	//////////////////////////
	////// Encryption
	SBLK_Init(AES, eAesOperMode, ENC);
	SBLK_SetFifo(16, 4, 16, 4, LASTBYTE_1ST);
	SBLK_StartFifo();
	SBLK_PutDataToRxFifo((u32)uAesPlainText, 16);
	
	while(!SBLK_IsFifoTransDone());

	SBLK_GetDataFromTxFifo(OUTPUT_BUF, 16);
	Copy((u32)uAesCipherText[eAesOperMode], GOLDEN_BUF, 16);
	uRet = Compare(OUTPUT_BUF, GOLDEN_BUF, 16);

	if(eTest == eTest_Manual)
	{
		if (uRet)
		{
		UART_Printf("AES_");
		DispOperMode(eAesOperMode);
		UART_Printf("_FIFO_ENC Success..\n");
		}
		else
		{
		UART_Printf("AES");
		DispOperMode(eAesOperMode);
		UART_Printf("_FIFO_ENC Failed..\n");
		}
	}
	else
	{
		if(uRet == false)
			{
				UART_Printf("AES");
				DispOperMode(eAesOperMode);
				UART_Printf("_FIFO_ENC Failed..\n");
				g_sblock_err =1;
				UART_Getc();
			}
		else if (Sblock_DEBUG)
			{
			UART_Printf("AES_");
			DispOperMode(eAesOperMode);
			UART_Printf("_FIFO_ENC Success..\n");
			}
	}

	/////////////////////////
	////// Decryption
	if (eAesOperMode == CTR)
		SBLK_Init(AES, eAesOperMode, ENC);
	else
		SBLK_Init(AES, eAesOperMode, DEC);

	SBLK_SetFifo(16, 4, 16, 4, LASTBYTE_1ST);
	SBLK_StartFifo();
	SBLK_PutDataToRxFifo(OUTPUT_BUF, 16);

	while(!SBLK_IsFifoTransDone());

	SBLK_GetDataFromTxFifo(INPUT_BUF, 16);
	Copy((u32)uAesPlainText, GOLDEN_BUF, 16);
	uRet = Compare(INPUT_BUF, GOLDEN_BUF, 16);

	if(eTest == eTest_Manual)
	{
		if (uRet)
		{
		UART_Printf("AES_");
		DispOperMode(eAesOperMode);
		UART_Printf("_FIFO_DEC Success..\n");
		}
		else
		{
		UART_Printf("AES");
		DispOperMode(eAesOperMode);
		UART_Printf("_FIFO_DEC Failed..\n");
		}
	}
	else
	{
		if(uRet == false)
			{
				UART_Printf("AES");
				DispOperMode(eAesOperMode);
				UART_Printf("_FIFO_DEC Failed..\n");
				g_sblock_err =1;
				UART_Getc();
			}
		else if (Sblock_DEBUG)
			{
			UART_Printf("AES_");
			DispOperMode(eAesOperMode);
			UART_Printf("_FIFO_DEC Success..\n");
			}
	}

}
//////////
// Function Name : Test_AES_DMA
// Function Description : AES DMA mode Test
// Input : 	None
// Output :	None 
// Version : v0.1
void Test_AES_DMA(eFunction_Test eTest, oFunctionT_AutoVar oAutoVar)
{
	u32 uRet, usel;

	//g_sblock_err=0;

	if(eTest == eTest_Manual)
	{
		UART_Printf("[SBLOCK - AES DMA_FIFO Mode Test]\n");

		UART_Printf("Select AES Operating Mode  : 1)ECB   2)CBC   3)CTR \n");
		usel=UART_GetIntNum();
		if (usel == 1) eAesOperMode = ECB;
		else if (usel == 2) eAesOperMode = CBC;
		else if (usel == 3) eAesOperMode = CTR;

		UART_Printf("Your selection is ");
		DispOperMode(eAesOperMode);
		UART_Printf("\n");
	}
	else
	{
		eAesOperMode = (OPER_MODE)oAutoVar.Test_Parameter[0];
	}


	INTC_SetVectAddr(NUM_SDMA1, Isr_SDMA1); //Rx Operation 
	INTC_Enable(NUM_SDMA1);
	
	//////////////////////////
	////// Encryption
	SBLK_Init(AES, eAesOperMode, ENC);
	SBLK_SetFifo(16, 4, 16, 4, LASTBYTE_1ST);
	SBLK_SetSDMA(INPUT_BUF, 16, OUTPUT_BUF, 16);
	Copy((u32)uAesPlainText, INPUT_BUF, 16);

	g_SDma1Done = 0;

	SBLK_StartFifo();
	SBLK_StartSDMA();

	while(!g_SDma1Done);

	Copy((u32)uAesCipherText[eAesOperMode], GOLDEN_BUF, 16);
	uRet = Compare(OUTPUT_BUF, GOLDEN_BUF, 16);

	// add BUF init
	Init_BUF(INPUT_BUF, 16);

	if(eTest == eTest_Manual)
	{
		if (uRet)
		{
		UART_Printf("AES_");
		DispOperMode(eAesOperMode);
		UART_Printf("_DMA_FIFO_ENC Success..\n");
		}
		else
		{
		UART_Printf("AES_");
		DispOperMode(eAesOperMode);
		UART_Printf("_DMA_FIFO_ENC Failed..\n");
		}
	}
	else
	{
		if(uRet == false)
			{
				UART_Printf("AES");
				DispOperMode(eAesOperMode);
				UART_Printf("_DMA_FIFO_ENC Failed..\n");
				g_sblock_err =1;
				UART_Getc();
			}
		else if (Sblock_DEBUG)
			{
			UART_Printf("AES_");
			DispOperMode(eAesOperMode);
			UART_Printf("_DMA_FIFO_ENC Success..\n");
			}
	}

	/////////////////////////
	////// Decryption

	if (eAesOperMode == CTR)
		SBLK_Init(AES, eAesOperMode, ENC);
	else
		SBLK_Init(AES, eAesOperMode, DEC);
	SBLK_SetFifo(16, 4, 16, 4, LASTBYTE_1ST);
	SBLK_SetSDMA(OUTPUT_BUF, 16, INPUT_BUF, 16);

	g_SDma1Done = 0;

	SBLK_StartFifo();
	SBLK_StartSDMA();

	while(!g_SDma1Done);

	Copy((u32)uAesPlainText, GOLDEN_BUF, 16);
	uRet = Compare(INPUT_BUF, GOLDEN_BUF, 16);

	if(eTest == eTest_Manual)
	{
		if (uRet)
		{
		UART_Printf("AES_");
		DispOperMode(eAesOperMode);
		UART_Printf("_DMA_FIFO_DEC Success..\n");
		}
		else
		{
		UART_Printf("AES");
		DispOperMode(eAesOperMode);
		UART_Printf("_DMA_FIFO_DEC Failed..\n");
		}
	}
	else
	{
		if(uRet == false)
			{
				UART_Printf("AES");
				DispOperMode(eAesOperMode);
				UART_Printf("_DMA_FIFO_DEC Failed..\n");
				g_sblock_err =1;
				UART_Getc();
			}
		else if (Sblock_DEBUG)
			{
			UART_Printf("AES_");
			DispOperMode(eAesOperMode);
			UART_Printf("_DMA_FIFO_DEC Success..\n");
			}
	}

	Init_BUF(INPUT_BUF, 16);
	Init_BUF(GOLDEN_BUF, 16);
	Init_BUF(OUTPUT_BUF, 16);
	INTC_Disable(NUM_SDMA1);
	
}

//////////
// Function Name : Test_DES_CPU
// Function Description : DES CPU mode Test
// Input : 	None
// Output :	None 
// Version : v0.1
void Test_DES_CPU(eFunction_Test eTest, oFunctionT_AutoVar oAutoVar)
{
	u32 uRet, i, usel;

	//g_sblock_err=0;

	if(eTest == eTest_Manual)
	{
		UART_Printf("[SBLOCK - DES CPU Mode Test]\n");

		UART_Printf("Select DES Operating Mode  : 1)ECB   2)CBC    \n");
		usel=UART_GetIntNum();
		if (usel == 1) eDesOperMode = ECB;
		else if (usel == 2) eDesOperMode = CBC;

		UART_Printf("Your selection is ");
		DispOperMode(eDesOperMode);
		UART_Printf("\n");
	}
	else
	{
		eDesOperMode = (OPER_MODE)oAutoVar.Test_Parameter[0];
	}

	//////////////////////////////////
	// Encryption
	SBLK_Init(DES, eDesOperMode, ENC);

	for (i=0; i<8; i++)
	{
		SBLK_PutDataToInReg((u32)uDesPlainText+i*4*2, 2, LASTBYTE_1ST);
		SBLK_StartByManual();

		while(!SBLK_IsOutputReady());

		SBLK_GetDataFromOutReg(OUTPUT_BUF+i*4*2, 2);
	}
	Copy((u32)uDesCipherText[eDesOperMode], GOLDEN_BUF, 16);
	uRet = Compare(OUTPUT_BUF, GOLDEN_BUF, 16);


	if(eTest == eTest_Manual)
	{
		if (uRet)
		{
		UART_Printf("DES_");
		DispOperMode(eDesOperMode);
		UART_Printf("_CPU_ENC Success..\n");
		}
		else
		{
		UART_Printf("DES");
		DispOperMode(eDesOperMode);
		UART_Printf("_CPU_ENC Failed..\n");
		}
	}
	else
	{
		if(uRet == false)
			{
				UART_Printf("DES");
				DispOperMode(eDesOperMode);
				UART_Printf("_CPU_ENC Failed..\n");
				g_sblock_err =1;
				UART_Getc();
			}
	}

	/////////////////////////////////
	// Decryption
	SBLK_Init(DES, eDesOperMode, DEC);

	for (i=0; i<8; i++)
	{
		SBLK_PutDataToInReg(OUTPUT_BUF+i*4*2, 2, LASTBYTE_1ST);
		SBLK_StartByManual();

		while(!SBLK_IsOutputReady());

		SBLK_GetDataFromOutReg(INPUT_BUF+i*4*2, 2);
	}
	Copy((u32)uDesPlainText, GOLDEN_BUF, 16);
	uRet = Compare(INPUT_BUF, GOLDEN_BUF, 16);


	if(eTest == eTest_Manual)
	{
		if (uRet)
		{
		UART_Printf("DES_");
		DispOperMode(eDesOperMode);
		UART_Printf("_CPU_DEC Success..\n");
		}
		else
		{
		UART_Printf("DES");
		DispOperMode(eDesOperMode);
			UART_Printf("_CPU_DEC Failed..\n");
		}
	}
	else
	{
		if(uRet == false)
			{
				UART_Printf("DES");
				DispOperMode(eDesOperMode);
				UART_Printf("_CPU_DEC Failed..\n");
				g_sblock_err =1;
				UART_Getc();
			}
	}

}


void SBLK_PutDataToRxFifo_Temp(u32 uSrcAddr, u32 uSize)
{	
	u32 i, uRemainedSize, uInputSize;
	u32 *pSrcAddr;

	pSrcAddr = (u32 *)uSrcAddr;
	uRemainedSize = uSize;


	SBLK_GetAvailableRxFifoSize(&uInputSize);
//	UART_Printf("uInputSize = 0x%x\n", uInputSize);
	uInputSize = uRemainedSize;
//	UART_Printf("uInputSize = 0x%x\n", uInputSize);
//	Delay(100000);
	
	for(i=0; i<uInputSize; i++)
	{
		//Outp32(rFIFO_RX_WR_BUF, *pSrcAddr++);
		Outp32(0x7D400040, *pSrcAddr++);
	}


/*	
	while(uRemainedSize > 0)
	{
		SBLK_GetAvailableRxFifoSize(&uInputSize);
		UART_Printf("uInputSize = 0x%x\n", uInputSize);
		
		if (uInputSize >= uRemainedSize)
		{
			uInputSize = uRemainedSize;
			uRemainedSize = 0;
		}
		else
		{
			uRemainedSize -= uInputSize;
		}

		for(i=0; i<uInputSize; i++)
			{
			//Outp32(rFIFO_RX_WR_BUF, *pSrcAddr++);
			Outp32(0x7D400040, *pSrcAddr++);
			}
	}
*/	
}


//////////
// Function Name : Test_DES_FIFO
// Function Description : DES FIFO mode Test
// Input : 	None
// Output :	None 
// Version : v0.1
//////////
// Function Name : Test_DES_FIFO
// Function Description : DES FIFO mode Test
// Input : 	None
// Output :	None 
// Version : v0.1
void Test_DES_FIFO(eFunction_Test eTest, oFunctionT_AutoVar oAutoVar)
{
	u32 uRet, usel;

	//g_sblock_err=0;

	if(eTest == eTest_Manual)
	{
		UART_Printf("[SBLOCK - DES FIFO Mode Test]\n");

		UART_Printf("Select DES Operating Mode  : 1)ECB   2)CBC    \n");
		usel=UART_GetIntNum();
		if (usel == 1) eDesOperMode = ECB;
		else if (usel == 2) eDesOperMode = CBC;

		UART_Printf("Your selection is ");
		DispOperMode(eDesOperMode);
		UART_Printf("\n");
	}
	else
	{
		eDesOperMode = (OPER_MODE)oAutoVar.Test_Parameter[0];
	}

	//////////////////////////////////
	// Encryption

	SBLK_Init(DES, eDesOperMode, ENC);
	SBLK_SetFifo(16, 2, 16, 2, LASTBYTE_1ST);
	SBLK_StartFifo();
	SBLK_PutDataToRxFifo((u32)uDesPlainText, 16);

	while(!SBLK_IsFifoTransDone());

	SBLK_GetDataFromTxFifo(OUTPUT_BUF, 16);
	Copy((u32)uDesCipherText[eDesOperMode], GOLDEN_BUF, 16);
	uRet = Compare(OUTPUT_BUF, GOLDEN_BUF, 16);

	if(eTest == eTest_Manual)
	{
		if (uRet)
		{
		UART_Printf("DES_");
		DispOperMode(eDesOperMode);
		UART_Printf("_FIFO_ENC Success..\n");
		}
		else
		{
		UART_Printf("DES");
		DispOperMode(eDesOperMode);
		UART_Printf("_FIFO_ENC Failed..\n");
		}
	}
	else
	{
		if(uRet == false)
			{
				UART_Printf("DES");
				DispOperMode(eDesOperMode);
				UART_Printf("_FIFO_ENC Failed..\n");
				g_sblock_err =1;
				UART_Getc();
			}
	}
	////////////////////////////////
	// Decryption
	SBLK_Init(DES, eDesOperMode, DEC);

	SBLK_SetFifo(16, 2, 16, 2, LASTBYTE_1ST);
	SBLK_StartFifo();
	SBLK_PutDataToRxFifo(OUTPUT_BUF, 16);

	while(!SBLK_IsFifoTransDone());

	SBLK_GetDataFromTxFifo(INPUT_BUF, 16);
	Copy((u32)uDesPlainText, GOLDEN_BUF, 16);
	uRet = Compare(INPUT_BUF, GOLDEN_BUF, 16);

	if(eTest == eTest_Manual)
	{
		if (uRet)
		{
		UART_Printf("DES_");
		DispOperMode(eDesOperMode);
		UART_Printf("_FIFO_DEC Success..\n");
		}
		else
		{
		UART_Printf("DES");
		DispOperMode(eDesOperMode);
		UART_Printf("_FIFO_DEC Failed..\n");
		}
	}
	else
	{
		if(uRet == false)
			{
				UART_Printf("DES");
				DispOperMode(eDesOperMode);
				UART_Printf("_FIFO_DEC Failed..\n");
				g_sblock_err =1;
				UART_Getc();
			}
	}

}

//////////
// Function Name : Test_DES_DMA
// Function Description : DES DMA mode Test
// Input : 	None
// Output :	None 
// Version : v0.1
void Test_DES_DMA(eFunction_Test eTest, oFunctionT_AutoVar oAutoVar)
{
	u32 uRet, usel;


	//g_sblock_err=0;

	if(eTest == eTest_Manual)
	{
		UART_Printf("[SBLOCK - DES FIFO Mode Test]\n");

		UART_Printf("Select DES Operating Mode  : 1)ECB   2)CBC    \n");
		usel=UART_GetIntNum();
		if (usel == 1) eDesOperMode = ECB;
		else if (usel == 2) eDesOperMode = CBC;

		UART_Printf("Your selection is ");
		DispOperMode(eDesOperMode);
		UART_Printf("\n");
	}
	else
	{
		eDesOperMode = (OPER_MODE)oAutoVar.Test_Parameter[0];
	}

	INTC_SetVectAddr(NUM_SDMA1, Isr_SDMA1); //Rx Operation 
	INTC_Enable(NUM_SDMA1);
	
	//////////////////////////////
	// Encryption
	SBLK_Init(DES, eDesOperMode, ENC);
	SBLK_SetFifo(16, 2, 16, 2, LASTBYTE_1ST);
	SBLK_SetSDMA(INPUT_BUF, 16, OUTPUT_BUF, 16);
	Copy((u32)uDesPlainText, INPUT_BUF, 16);

	g_SDma1Done = 0;

	SBLK_StartFifo();
	SBLK_StartSDMA();

	while(!g_SDma1Done);

	Copy((u32)uDesCipherText[eDesOperMode], GOLDEN_BUF, 16);
	uRet = Compare(OUTPUT_BUF, GOLDEN_BUF, 16);

	if(eTest == eTest_Manual)
	{
		if (uRet)
		{
		UART_Printf("DES_");
		DispOperMode(eDesOperMode);
		UART_Printf("_DMA_ENC Success..\n");
		}
		else
		{
		UART_Printf("DES");
		DispOperMode(eDesOperMode);
		UART_Printf("_DMA_ENC Failed..\n");
		}
	}
	else
	{
		if(uRet == false)
			{
				UART_Printf("DES");
				DispOperMode(eDesOperMode);
				UART_Printf("_DMA_ENC Failed..\n");
				g_sblock_err =1;
				UART_Getc();
			}
	}
	
	////////////////////////////////
	// Decryption
	SBLK_Init(DES, eDesOperMode, DEC);
	SBLK_SetFifo(16, 2, 16, 2, LASTBYTE_1ST);
	SBLK_SetSDMA(OUTPUT_BUF, 16, INPUT_BUF, 16);

	g_SDma1Done = 0;

	SBLK_StartFifo();
	SBLK_StartSDMA();

	while(!g_SDma1Done);

	Copy((u32)uDesPlainText, GOLDEN_BUF, 16);
	uRet = Compare(INPUT_BUF, GOLDEN_BUF, 16);

	if(eTest == eTest_Manual)
	{
		if (uRet)
		{
		UART_Printf("DES_");
		DispOperMode(eDesOperMode);
		UART_Printf("_DMA_DEC Success..\n");
		}
		else
		{
		UART_Printf("DES");
		DispOperMode(eDesOperMode);
		UART_Printf("_DMA_DEC Failed..\n");
		}
	}
	else
	{
		if(uRet == false)
			{
				UART_Printf("DES");
				DispOperMode(eDesOperMode);
				UART_Printf("_DMA_DEC Failed..\n");
				g_sblock_err =1;
				UART_Getc();
			}
	}
	INTC_Disable(NUM_SDMA1);
}

//////////
// Function Name : Test_HASH_CPU
// Function Description : HASH CPU mode Test
// Input : 	None
// Output :	None 
// Version : v0.1
void Test_HASH_CPU(eFunction_Test eTest, oFunctionT_AutoVar oAutoVar)
{
	u32 uRet, usel;

	//g_sblock_err=0;
	
	if(eTest == eTest_Manual)
	{
		UART_Printf("[SBLOCK - HASH CPU Mode Test]\n");

		UART_Printf("Select HASH Operating Mode  : 1)HMAC  2)SHA1  3)PRNG \n");
		usel=UART_GetIntNum();
		if (usel == 1) eHashOperMode = HMAC;
		else if (usel == 2) eHashOperMode = SHA1;
		else if (usel == 3) eHashOperMode = PRNG;

		UART_Printf("Your selection is ");
		DispOperMode(eHashOperMode);
		UART_Printf("\n");
	}
	else
	{
		eHashOperMode = (OPER_MODE)oAutoVar.Test_Parameter[0];
	}

	SBLK_Init(HASH, eHashOperMode, ENC);
	if (eHashOperMode == HMAC)
		SBLK_PutDataToInReg((u32)uHmacPlainText19, 19, LASTBYTE_1ST);
	else if (eHashOperMode == SHA1)
		SBLK_PutDataToInReg((u32)uSha1PlainText16, 16, LASTBYTE_4TH);

	while(!SBLK_IsOutputReady());

	if (eHashOperMode == PRNG)
	{
		SBLK_GetDataFromOutReg(OUTPUT_BUF, 10);
 		if(eTest == eTest_Manual)
 			{
			Dump32(OUTPUT_BUF, 10);
 			}
		return;
	}
	SBLK_GetDataFromOutReg(OUTPUT_BUF, 5);
	if (eHashOperMode == HMAC)
		Copy((u32)uHmacCiperText19, GOLDEN_BUF, 5);
	else if (eHashOperMode == SHA1)
		Copy((u32)uSha1CipherText16, GOLDEN_BUF, 5);
	uRet = Compare(OUTPUT_BUF, GOLDEN_BUF, 5);

	if(eTest == eTest_Manual)
	{
		if (uRet)
		{
		UART_Printf("HASH_");
		DispOperMode(eHashOperMode);
		if (eHashOperMode == HMAC)
			UART_Printf("_CPU 19 Success..\n");
		else if (eHashOperMode == SHA1)
			UART_Printf("_CPU 16 Success..\n");
		}
		else
		{
		UART_Printf("HASH_");
		DispOperMode(eHashOperMode);
		if (eHashOperMode == HMAC)
			UART_Printf("_CPU 19 Failed..\n");
		else if (eHashOperMode == SHA1)
			UART_Printf("_CPU 16 Failed..\n");
		}
	}
	else
	{
		if(uRet == false)
			{
			UART_Printf("HASH_");
			DispOperMode(eHashOperMode);
			if (eHashOperMode == HMAC)
				UART_Printf("_CPU 19 Failed..\n");
			else if (eHashOperMode == SHA1)
				UART_Printf("_CPU 16 Failed..\n");

				g_sblock_err =1;
				UART_Getc();
			}
	}

	SBLK_Init(HASH, eHashOperMode, ENC);
	if (eHashOperMode == HMAC)
		SBLK_PutDataToInReg((u32)uHmacPlainText47, 47, LASTBYTE_4TH);
	else if (eHashOperMode == SHA1)
		SBLK_PutDataToInReg((u32)uSha1PlainText47, 47, LASTBYTE_4TH);

	while(!SBLK_IsOutputReady());

	SBLK_GetDataFromOutReg(OUTPUT_BUF, 5);
	if (eHashOperMode == HMAC)
		Copy((u32)uHmacCiperText47, GOLDEN_BUF, 5);
	else if (eHashOperMode == SHA1)
		Copy((u32)uSha1CipherText47, GOLDEN_BUF, 5);
	uRet = Compare(OUTPUT_BUF, GOLDEN_BUF, 5);

	if(eTest == eTest_Manual)
	{
		if (uRet)
		{
		UART_Printf("HASH_");
		DispOperMode(eHashOperMode);
		UART_Printf("_CPU 47 Success..\n");
		}
		else
		{
		UART_Printf("HASH_");
		DispOperMode(eHashOperMode);
		UART_Printf("_CPU 47 Failed..\n");
		}
	}
	else
	{
		if(uRet == false)
			{
			UART_Printf("HASH_");
			DispOperMode(eHashOperMode);
			UART_Printf("_CPU 47 Failed..\n");
			g_sblock_err =1;
			UART_Getc();
			}
	}

}

//////////
// Function Name : Test_HASH_FIFO
// Function Description : HASH FIFO mode Test
// Input : 	None
// Output :	None 
// Version : v0.1
void Test_HASH_FIFO(eFunction_Test eTest, oFunctionT_AutoVar oAutoVar)
{
	u32 uRet, usel;

	//g_sblock_err=0;
	
	if(eTest == eTest_Manual)
	{
		UART_Printf("[SBLOCK - HASH FIFO Mode Test]\n");

		UART_Printf("Select HASH Operating Mode  : 1)HMAC  2)SHA1  3)PRNG \n");
		usel=UART_GetIntNum();
		if (usel == 1) eHashOperMode = HMAC;
		else if (usel == 2) eHashOperMode = SHA1;
		else if (usel == 3) eHashOperMode = PRNG;

		UART_Printf("Your selection is ");
		DispOperMode(eHashOperMode);
		UART_Printf("\n");
	}
	else
	{
		eHashOperMode = (OPER_MODE)oAutoVar.Test_Parameter[0];
	}

	SBLK_Init(HASH, eHashOperMode, ENC);
	if (eHashOperMode == HMAC)
	{
		SBLK_SetFifo(19, 16, 5, 16, LASTBYTE_1ST);
		SBLK_StartFifo();
		SBLK_PutDataToRxFifo((u32)uHmacPlainText19, 19);
	}
	else if (eHashOperMode == SHA1)
	{
		SBLK_SetFifo(16, 16, 5, 16, LASTBYTE_4TH);
		SBLK_StartFifo();
		SBLK_PutDataToRxFifo((u32)uSha1PlainText16, 16);
	}
	else if (eHashOperMode == PRNG)
	{
		SBLK_SetFifo(10, 10, 10, 10, LASTBYTE_1ST);
		SBLK_StartFifo();
	}

	while(!SBLK_IsFifoTransDone());

	if (eHashOperMode == PRNG)
	{
		SBLK_GetDataFromTxFifo(OUTPUT_BUF, 10);
		SBLK_GetDataFromOutReg(GOLDEN_BUF, 10);
		uRet = Compare(OUTPUT_BUF, GOLDEN_BUF, 10);
		if(eTest == eTest_Manual)
		{
			if (uRet)
			{
			UART_Printf("HASH_PRNG_FIFO Success..\n");
			}
			else
			{
			UART_Printf("HASH_PRNG_FIFO Failed..\n");
			}
		}
		else
		{
			if(uRet == false)
				{
				UART_Printf("HASH_PRNG_FIFO Failed..\n");
				g_sblock_err =1;
				UART_Getc();
				}
		}
		return;
	}

	SBLK_GetDataFromTxFifo(OUTPUT_BUF, 5);
	if (eHashOperMode == HMAC)
		Copy((u32)uHmacCiperText19, GOLDEN_BUF, 5);
	else if (eHashOperMode == SHA1)
		Copy((u32)uSha1CipherText16, GOLDEN_BUF, 5);
	uRet = Compare(OUTPUT_BUF, GOLDEN_BUF, 5);
	if(eTest == eTest_Manual)
	{
		if (uRet)
		{
			UART_Printf("HASH_");
			DispOperMode(eHashOperMode);
			if (eHashOperMode == HMAC)
				UART_Printf("_FIFO 19 Success..\n");
			else if (eHashOperMode == SHA1)
				UART_Printf("_FIFO 16 Success..\n");
		}
		else
		{
			UART_Printf("HASH_");
			DispOperMode(eHashOperMode);
			if (eHashOperMode == HMAC)
				UART_Printf("_FIFO 19 Failed..\n");
			else if (eHashOperMode == SHA1)
				UART_Printf("_FIFO 16 Failed..\n");
		}
	}
	else
	{
		if(uRet == false)
			{
			UART_Printf("HASH_");
			DispOperMode(eHashOperMode);
			if (eHashOperMode == HMAC)
				UART_Printf("_FIFO 19 Failed..\n");
			else if (eHashOperMode == SHA1)
				UART_Printf("_FIFO 16 Failed..\n");

			g_sblock_err =1;
			UART_Getc();
			}
	}

	
	SBLK_Init(HASH, eHashOperMode, ENC);
	if (eHashOperMode == HMAC)
	{
		SBLK_SetFifo(47, 16, 5, 16, LASTBYTE_4TH);
		SBLK_StartFifo();
		SBLK_PutDataToRxFifo((u32)uHmacPlainText47, 47);
	}
	else if (eHashOperMode == SHA1)
	{
		SBLK_SetFifo(47, 16, 5, 16, LASTBYTE_4TH);
		SBLK_StartFifo();
		SBLK_PutDataToRxFifo((u32)uSha1PlainText47, 47);
	}

	while(!SBLK_IsFifoTransDone());

	SBLK_GetDataFromTxFifo(OUTPUT_BUF, 5);
	if (eHashOperMode == HMAC)
		Copy((u32)uHmacCiperText47, GOLDEN_BUF, 5);
	else if (eHashOperMode == SHA1)
		Copy((u32)uSha1CipherText47, GOLDEN_BUF, 5);
	uRet = Compare(OUTPUT_BUF, GOLDEN_BUF, 5);

	if(eTest == eTest_Manual)
	{
		if (uRet)
		{
		UART_Printf("HASH_");
		DispOperMode(eHashOperMode);
		UART_Printf("_FIFO 47 Success..\n");
		}
		else
		{
		UART_Printf("HASH_");
		DispOperMode(eHashOperMode);
		UART_Printf("_FIFO 47 Failed..\n");
		}
	}
	else
	{
		if(uRet == false)
			{
			UART_Printf("HASH_");
			DispOperMode(eHashOperMode);
			UART_Printf("_FIFO 47 Failed..\n");
			g_sblock_err =1;
			UART_Getc();
			}
	}
 }

//////////
// Function Name : Test_HASH_DMA
// Function Description : HASH DMA mode Test
// Input : 	None
// Output :	None 
// Version : v0.1
void Test_HASH_DMA(eFunction_Test eTest, oFunctionT_AutoVar oAutoVar)
{
	u32 uRet, usel;
	
	//g_sblock_err=0;
	
	if(eTest == eTest_Manual)
	{
		UART_Printf("[SBLOCK - HASH DMA Mode Test]\n");

		UART_Printf("Select HASH Operating Mode  : 1)HMAC  2)SHA1  3)PRNG \n");
		usel=UART_GetIntNum();
		if (usel == 1) eHashOperMode = HMAC;
		else if (usel == 2) eHashOperMode = SHA1;
		else if (usel == 3) eHashOperMode = PRNG;

		UART_Printf("Your selection is ");
		DispOperMode(eHashOperMode);
		UART_Printf("\n");
	}
	else
	{
		eHashOperMode = (OPER_MODE)oAutoVar.Test_Parameter[0];
	}	


	INTC_SetVectAddr(NUM_SDMA1, Isr_SDMA1); //Rx Operation 
	INTC_Enable(NUM_SDMA1);

	
	if (eHashOperMode == PRNG)
	{
		if(eTest == eTest_Manual)
			UART_Printf("This mode isn't supported..\n");
		return;
	}

	SBLK_Init(HASH, eHashOperMode, ENC);
	if (eHashOperMode == HMAC)
	{
		SBLK_SetFifo(19, 16, 5, 16, LASTBYTE_1ST);
		SBLK_SetSDMA(INPUT_BUF, 19, OUTPUT_BUF, 5);
		Copy((u32)uHmacPlainText19, INPUT_BUF, 19);
	}
	else
	{
		SBLK_SetFifo(16, 16, 5, 16, LASTBYTE_4TH);
		SBLK_SetSDMA(INPUT_BUF, 16, OUTPUT_BUF, 5);
		Copy((u32)uSha1PlainText16, INPUT_BUF, 16);
	}


	if (eHashOperMode == HMAC)
		Copy((u32)uHmacCiperText19, GOLDEN_BUF, 5);
	else if (eHashOperMode == SHA1)
		Copy((u32)uSha1CipherText16, GOLDEN_BUF, 5);

	g_SDma1Done= 0;

	SBLK_StartFifo();
	SBLK_StartSDMA();

	while(!g_SDma1Done);
	//Delay(1);
	
	uRet = Compare(OUTPUT_BUF, GOLDEN_BUF, 5);

	if(eTest == eTest_Manual)
	{
		if (uRet)
		{
			UART_Printf("HASH_");
			DispOperMode(eHashOperMode);
			if (eHashOperMode == HMAC)
				UART_Printf("_DMA 19 Success..\n");
			else if (eHashOperMode == SHA1)
				UART_Printf("_DMA 16 Success..\n");
		}
		else
		{
			UART_Printf("HASH_");
			DispOperMode(eHashOperMode);
			if (eHashOperMode == HMAC)
				UART_Printf("_DMA 19 Failed..\n");
			else if (eHashOperMode == SHA1)
				UART_Printf("_DMA 16 Failed..\n");
		}
	}
	else
	{
		if(uRet == false)
			{
			UART_Printf("HASH_");
			DispOperMode(eHashOperMode);
			if (eHashOperMode == HMAC)
				UART_Printf("_DMA 19 Failed..\n");
			else if (eHashOperMode == SHA1)
				UART_Printf("_DMA 16 Failed..\n");

			g_sblock_err =1;
			UART_Getc();
			}
	}

	Init_BUF(OUTPUT_BUF, 5);
	Init_BUF(GOLDEN_BUF, 5);

	SBLK_Init(HASH, eHashOperMode, ENC);
	if (eHashOperMode == HMAC)
	{
		SBLK_SetFifo(47, 16, 5, 16, LASTBYTE_4TH);
		SBLK_SetSDMA(INPUT_BUF, 47, OUTPUT_BUF, 5);
		Copy((u32)uHmacPlainText47, INPUT_BUF, 47);
	}
	else
	{
		SBLK_SetFifo(47, 16, 5, 16, LASTBYTE_4TH);
		SBLK_SetSDMA(INPUT_BUF, 47, OUTPUT_BUF, 5);
		Copy((u32)uSha1PlainText47, INPUT_BUF, 47);
	}


	if (eHashOperMode == HMAC)
		Copy((u32)uHmacCiperText47, GOLDEN_BUF, 5);
	else if (eHashOperMode == SHA1)
		Copy((u32)uSha1CipherText47, GOLDEN_BUF, 5);
	
	g_SDma1Done = 0;

	SBLK_StartFifo();
	SBLK_StartSDMA();

	while(!g_SDma1Done);

	//Delay(1);


	uRet = Compare(OUTPUT_BUF, GOLDEN_BUF, 5);
	if(eTest == eTest_Manual)
	{
		if (uRet)
		{
		UART_Printf("HASH_");
		DispOperMode(eHashOperMode);
		UART_Printf("_DMA 47 Success..\n");
		}
		else
		{
		UART_Printf("HASH_");
		DispOperMode(eHashOperMode);
		UART_Printf("_DMA 47 Failed..\n");
		}
	}
	else
	{
		if(uRet == false)
			{
			UART_Printf("HASH_");
			DispOperMode(eHashOperMode);
			UART_Printf("_DMA 47 Failed..\n");
			g_sblock_err =1;
			UART_Getc();
			}
	}

	Init_BUF(OUTPUT_BUF, 5);
	Init_BUF(GOLDEN_BUF, 5);
INTC_Disable(NUM_SDMA1);

}



void Sblock_ClockChange(void)
{
	u32 uRatio, uRegValue, uMode;
	
	UART_Printf("Select Secure Ratio:  1, or 3\n");
	uRatio = UART_GetIntNum();

	uRegValue= Inp32SYSC(0x20);
	uRegValue = uRegValue&~(3<<18)|(uRatio<<18);
	Outp32SYSC(0x20, uRegValue);

	UART_Printf("rCLK_DIV0: 0x%x\n", Inp32SYSC(0x20));


	UART_Printf("Select Operating Mode: 0: Sync,   1: Async \n");
	uMode = UART_GetIntNum();

	switch(uMode)
		{
			case 0:		// Sync Mode
			SYSC_ChangeMode(eSYNC_MODE);
			SYSC_ChangeSYSCLK_1(eAPLL532M, eAPLL266M, 0, 1, 3);
			break;

			case 1:		//ASync Mode
			SYSC_ChangeMode(eASYNC_MODE);
			SYSC_ChangeSYSCLK_1(eAPLL532M, eAPLL266M, 0, 0, 3);
			break;
		}

			SYSC_GetClkInform();
			OpenConsole();
			UART_InitDebugCh(0, 115200);
			Delay(10);
			
			UART_Printf("\n\n");
			
			UART_Printf("ARMCLK: %.2fMHz  HCLKx2: %.2fMHz  HCLK: %.2fMHz  PCLK: %.2fMHz  Secure_CLK: %.2fMHz\n",(float)g_ARMCLK/1.0e6, (float)g_HCLKx2/1.0e6, (float)g_HCLK/1.0e6, (float)g_PCLK/1.0e6, (float)g_HCLKx2/(1.0e6 * (uRatio+1)));
			UART_Printf("SYNC Mode : %d\n", g_SYNCACK);
			UART_Printf("\n");
	

	
}


//////////
// Function Name : Test_HASH_CPU
// Function Description : HASH CPU mode Test
// Input : 	None
// Output :	None 
// Version : v0.1
/*
void Test_HASH_CPU_Added(void)
{
	u32 uRet;

	SBLK_Init(HASH, SHA1, ENC);

	//SHA1 - #0
	SBLK_PutDataToInReg((u32)uSha1PlainText1, 1, LASTBYTE_1ST);

	while(!SBLK_IsOutputReady());

	SBLK_GetDataFromOutReg(OUTPUT_BUF, 5);

	Copy((u32)uSha1CipherText1, GOLDEN_BUF, 5);
	uRet = Compare(OUTPUT_BUF, GOLDEN_BUF, 5);
	if (uRet)
	{
		UART_Printf("HASH_SHA1_CPU 1, LAST_1ST Success \n");
		
	}
	else
	{
		UART_Printf("HASH_SHA1_CPU 1, LAST_1ST Failed \n");
	}


	UART_Printf(" \n");
	
}
*/
/*
void Test_HASH_CPU_Added(void)
{
	u32 uRet;

	//SHA1 - #0
	SBLK_Init(HASH, SHA1, ENC);
	
	SBLK_PutDataToInReg((u32)uSha1PlainText1, 1, LASTBYTE_1ST);

	while(!SBLK_IsOutputReady());

	SBLK_GetDataFromOutReg(OUTPUT_BUF, 5);

	//SHA1- #1

	SBLK_Init(HASH, SHA1, ENC);

	SBLK_PutDataToInReg((u32)uSha1PlainText1, 1, LASTBYTE_2ND);

	while(!SBLK_IsOutputReady());

	SBLK_GetDataFromOutReg(OUTPUT_BUF+5*4, 5);

	//SHA1- #3

	SBLK_Init(HASH, SHA1, ENC);

	SBLK_PutDataToInReg((u32)uSha1PlainText1, 1, LASTBYTE_3RD);

	while(!SBLK_IsOutputReady());

	SBLK_GetDataFromOutReg(OUTPUT_BUF+10*4, 5);


	//SHA1- #4

	SBLK_Init(HASH, SHA1, ENC);

	SBLK_PutDataToInReg((u32)uSha1PlainText1, 1, LASTBYTE_4TH);

	while(!SBLK_IsOutputReady());

	SBLK_GetDataFromOutReg(OUTPUT_BUF+15*4, 5);
	

	Copy((u32)uSha1CipherText1, GOLDEN_BUF, 5*4);
	uRet = Compare(OUTPUT_BUF, GOLDEN_BUF, 5*4);
	if (uRet)
	{
		UART_Printf("HASH_SHA1_CPU 1,  Success \n");
		
	}
	else
	{
		UART_Printf("HASH_SHA1_CPU 1,  Failed \n");
	}


	UART_Printf(" \n");
	
}
*/
/*
void Test_HASH_CPU_Added(void)
{
	u32 uRet, uLASTBYTE,uRep;


	for(uRep=0;uRep<4;uRep++)
	{
		SBLK_Init(HASH, SHA1, ENC);
		switch(uRep)
		{
			case 0: 
				uLASTBYTE= LASTBYTE_1ST;
			     	break;           
			case 1: 
				uLASTBYTE= LASTBYTE_2ND;
			     	break;     
			case 2: 
				uLASTBYTE= LASTBYTE_3RD;
			     	break;        
			case 3: 
				uLASTBYTE= LASTBYTE_4TH;
			     	break;   	     	
		}
	
		SBLK_PutDataToInReg((u32)uSha1PlainText16, 1, (LVALID_BYTE)uLASTBYTE);
		while(!SBLK_IsOutputReady());
		SBLK_GetDataFromOutReg(OUTPUT_BUF + (uRep*5)*4, 5);
		}


	Copy((u32)uSha1CipherText1, GOLDEN_BUF, 5*4);
	uRet = Compare(OUTPUT_BUF, GOLDEN_BUF, 5*4);
	if (uRet)
	{
		UART_Printf("HASH_SHA1_CPU 1,  Success \n");
		
	}
	else
	{
		UART_Printf("HASH_SHA1_CPU 1,  Failed \n");
	}


	UART_Printf(" \n");
	
}
*/

void Test_HASH_CPU_Added(eFunction_Test eTest, oFunctionT_AutoVar oAutoVar)
{
	u32 uRet, uLASTBYTE,uRep, i;

   for(i=1;i<16;i++)
   {
	for(uRep=0;uRep<4;uRep++)
	{
		SBLK_Init(HASH, SHA1, ENC);
		switch(uRep)
		{
			case 0: 
				uLASTBYTE= LASTBYTE_1ST;
			     	break;           
			case 1: 
				uLASTBYTE= LASTBYTE_2ND;
			     	break;     
			case 2: 
				uLASTBYTE= LASTBYTE_3RD;
			     	break;        
			case 3: 
				uLASTBYTE= LASTBYTE_4TH;
			     	break;   	     	
		}
	
		SBLK_PutDataToInReg((u32)uSha1PlainText16, i, (LVALID_BYTE)uLASTBYTE);
		while(!SBLK_IsOutputReady());
		SBLK_GetDataFromOutReg(OUTPUT_BUF + (uRep*5)*4, 5);
		}

				
		switch(i)
		{
			case 1: 
				Copy((u32)uSha1CipherText1, GOLDEN_BUF, 5*4);
			     	break;           
			case 2: 
				Copy((u32)uSha1CipherText2, GOLDEN_BUF, 5*4);
			     	break;     
			case 3: 
				Copy((u32)uSha1CipherText3, GOLDEN_BUF, 5*4);
			     	break;        
			case 4: 
				Copy((u32)uSha1CipherText4, GOLDEN_BUF, 5*4);
			     	break;   	     	
			case 5: 
				Copy((u32)uSha1CipherText5, GOLDEN_BUF, 5*4);
			     	break;   	     	
			case 6: 
				Copy((u32)uSha1CipherText6, GOLDEN_BUF, 5*4);
			     	break;   	      	
			case 7: 
				Copy((u32)uSha1CipherText7, GOLDEN_BUF, 5*4);
			     	break;   	       	
			case 8: 
				Copy((u32)uSha1CipherText8, GOLDEN_BUF, 5*4);
			     	break;   	   
			case 9: 
				Copy((u32)uSha1CipherText9, GOLDEN_BUF, 5*4);
			     	break;   	   	     	
			case 10: 
				Copy((u32)uSha1CipherText10, GOLDEN_BUF, 5*4);
			     	break;
			case 11: 
				Copy((u32)uSha1CipherText11, GOLDEN_BUF, 5*4);
			     	break;
 			case 12: 
				Copy((u32)uSha1CipherText12, GOLDEN_BUF, 5*4);
			     	break;    	
			case 13: 
				Copy((u32)uSha1CipherText13, GOLDEN_BUF, 5*4);
			     	break;     	
			case 14: 
				Copy((u32)uSha1CipherText14, GOLDEN_BUF, 5*4);
			     	break;
			case 15: 
				Copy((u32)uSha1CipherText15, GOLDEN_BUF, 5*4);
				break;
			     	
			     	
		}



	
	uRet = Compare(OUTPUT_BUF, GOLDEN_BUF, 5*4);

	if(eTest == eTest_Manual)
	{
		if (uRet)
		{
		UART_Printf("HASH_SHA1_CPU %d,  Success \n", i);
		}
		else
		{
		UART_Printf("HASH_SHA1_CPU %d,  Failed \n", i );
		}
	}
	else
	{
		if(uRet == false)
			{
				UART_Printf("HASH_SHA1_CPU %d,  Failed \n", i );
				g_sblock_err =1;
				UART_Getc();
			}
	}

  }

}


void Test_SHA1_INTERMEDIATE(eFunction_Test eTest, oFunctionT_AutoVar oAutoVar)
{
	u32 uRet, i;

	SBLK_Init(HASH, SHA1, ENC);
	//Outp32(0x7D300000, 0x9);

/*
	// Intermediate FIFO Setting
	Outp32(0x7D400000, 0x04);		// Rx Fifo Host Write Enable/Start
	Outp32(0x7D800000, 0x04);		// Tx Fifo Host Write Enable/Start

	Outp32(0x7D400004, 0x79);		// Rx Fifo Message Length
	Outp32(0x7D400008, 0x30010);	// Full 32bit, block Size 0x10
	Outp32(0x7D40000C, 0x7D300004);	// Rx FIFO Destination Address, HASH Rx DATA

	Outp32(0x7D800004, 0x5);		// Tx FIFO Message Length
	Outp32(0x7D800008, 0x10);
	//Outp32(0x7D80000C, 0x7D300034);	// Tx FIFO Source Address => Rx HASHOUT ??
	Outp32(0x7D80000C, 0x7D700034);	// Tx FIFO Source Address => Rx HASHOUT ??
	
	Outp32(0x7D400000, 0xB3);		// Rx FIFO Start 
	Outp32(0x7D800000, 0xB3);		// Tx FIFO Start	
*/

	SBLK_SetFifo(0x79, 0x30010, 5, 0x10, LASTBYTE_4TH);
	SBLK_StartFifo();

	//for(i=0;i<7;i++){
	for(i=0;i<6;i++){
	//Wait Write Enable...		
	SBLK_PutDataToRxFifo((u32)uSha1PlainText16, 16);
	}
	

	while(((Inp32(0x7D400000)>>8)&0xFF)>1);	// RX_FIFO Empty Polling
	while(((Inp32(0x7D300030)>>3)&0x3)==3);	// Ready to receivce

	//Delay(10);
	//SBLK_GetDataFromTxFifo(OUTPUT_BUF, 5);
	
	Copy(0x7D30005c, MID_BUF, 5);


	// HMAC Operating.
	SBLK_Init(HASH, HMAC, ENC);
	SBLK_SetFifo(47, 16, 5, 16, LASTBYTE_4TH);
	SBLK_StartFifo();
	SBLK_PutDataToRxFifo((u32)uHmacPlainText47, 47);
	while(!SBLK_IsFifoTransDone());
	SBLK_GetDataFromTxFifo(OUTPUT_BUF, 5);
	Copy((u32)uHmacCiperText47, GOLDEN_BUF, 5);
	uRet = Compare(OUTPUT_BUF, GOLDEN_BUF, 5);

	if(eTest == eTest_Manual)
	{
		if (uRet)
		{
		UART_Printf("HASH_HMAC ,  Success \n");
		
		}
		else
		{
		UART_Printf("HASH_HMAC ,  Failed \n" );
		}
	}
	// Start 2nd SHA1 Operation
	Outp32(0x7D300000, 0x0);
	Outp32(0x7D400000, 0x4);
	Outp32(0x7D800000, 0x4);

		
	//Copy(MID_BUF, 0x7D300070, 5);		//Load Back-up Intermediate Value
	//Copy(MID_BUF, 0x7D700070, 5);

/*
	Copy(MID_BUF+0x10, 0x7D300070,1);
	Copy(MID_BUF+0xc, 0x7D300074,1);
	Copy(MID_BUF+8, 0x7D300078,1);
	Copy(MID_BUF+4, 0x7D30007c,1);
	Copy(MID_BUF, 0x7D300080,1);
*/
	

	Outp32(0x7D300070, 0xcd381861);		// Golden Data => MID Value
	Outp32(0x7D300074, 0x1a921f94);
	Outp32(0x7D300078, 0x6e76da53);
	Outp32(0x7D30007c, 0x5c52c230);
	Outp32(0x7D300080, 0x64e696cd);

/*
	Outp32(0x7D300070, 0x64e696cd);
	Outp32(0x7D300074, 0x5c52c230);
	Outp32(0x7D300078, 0x6e76da53);
	Outp32(0x7D30007c,0x1a921f94);
	Outp32(0x7D300080, 0xcd381861);
*/


/*
	Outp32(0x7D300070, 0x611838cd);
	Outp32(0x7D300074, 0x941f921a);
	Outp32(0x7D300078, 0x53da766e);
	Outp32(0x7D30007c, 0x30c2525c);
	Outp32(0x7D300080, 0xcd96e664);
*/

/*
	Outp32(0x7D300080, 0x611838cd);
	Outp32(0x7D30007c, 0x941f921a);
	Outp32(0x7D300078, 0x53da766e);
	Outp32(0x7D300074, 0x30c2525c);
	Outp32(0x7D300070, 0xcd96e664);
*/

	
	//UART_Printf("HASH_CTRL Reg: 0x%x\n", Inp32(0x7D300000));
	  // FIFO Reset
	//Outp32(0x7D300000, 0x109);		// Usign Back-up IV, not SHA1 Constants	
	//Copy(MID_BUF, 0x7D700070, 5);		//Load Back-up Intermediate Value
	Outp32(0x7D300084, 0x0);
	Outp32(0x7D300088,0xC0000);

	// SHA1 Pre Length Load,  96Word => C00

	Outp32(0x7D300000, 0x109);		// Usign Back-up IV, not SHA1 Constants
	//Outp32(0x7D300000, 0x1c9);		// Usign Back-up IV, not SHA1 Constants
	//Outp32(0x7D300000, 0x109);		// Usign Back-up IV, not SHA1 Constants
	//SBLK_Init_T(HASH, SHA1, ENC);
	Outp32(0x7D400000, 0x4);
	Outp32(0x7D800000, 0x4);  // FIFO Reset
	SBLK_SetFifo(0x19, 0x30010, 5, 0x10, LASTBYTE_4TH);
	//SBLK_StartFifo();
	Outp32(0x7D400000, 0xB3);		// Rx FIFO Start 
	Outp32(0x7D800000, 0xB3);		// Tx FIFO Start	



	//UART_Printf("HASH_CTRL Reg: 0x%x\n", Inp32(0x7D300000));
/*
	// Intermediate FIFO Setting
	Outp32(0x7D400000, 0x04);		// Rx Fifo Host Write Enable/Start
	Outp32(0x7D800000, 0x04);		// Tx Fifo Host Write Enable/Start

	Outp32(0x7D400004, 0x19);		// Rx Fifo Message Length
	Outp32(0x7D400008, 0x30010);	// Full 32bit, block Size 0x10
	Outp32(0x7D40000C, 0x7D300004);	// Rx FIFO Destination Address, HASH Rx DATA

	Outp32(0x7D800004, 0x5);		// Tx FIFO Message Length
	Outp32(0x7D800008, 0x10);
	//Outp32(0x7D80000C, 0x7D300034);	// Tx FIFO Source Address => Rx HASHOUT ??
	Outp32(0x7D80000C, 0x7D700034);	// Tx FIFO Source Address => Rx HASHOUT ??
	
	Outp32(0x7D400000, 0xB3);		// Rx FIFO Start 
	Outp32(0x7D800000, 0xB3);		// Tx FIFO Start	
*/


	
//	while(((Inp32(0x7D400000)>>16)&0xFFFF)<0x10);	//
	SBLK_PutDataToRxFifo((u32)uSha1PlainText16, 16);
//	while(((Inp32(0x7D400000)>>16)&0xFFFF)<0x10);
	SBLK_PutDataToRxFifo((u32)uSha1PlainText16, 9);
       //Outp32(0x7D300000, 0x0C9);
//	Outp32(0x7D400040, 0x64636261);
	//SBLK_PutDataToInReg1((u32)uSha1PlainText47, 25, LASTBYTE_4TH);

	
	//while(!SBLK_IsOutputReady());
	while(!SBLK_IsFifoTransDone());
	//SBLK_GetDataFromOutReg(OUTPUT_BUF, 5);

	SBLK_GetDataFromTxFifo(OUTPUT_BUF, 5);

	Copy((u32)uSha1CipherText_Inter, GOLDEN_BUF, 5);
	uRet = Compare(OUTPUT_BUF, GOLDEN_BUF, 5);
	if(eTest == eTest_Manual)
	{
		if (uRet)
		{
		UART_Printf("HASH_SHA1_CPU 1, LAST_1ST Success \n");
		}
		else
		{
		UART_Printf("HASH_SHA1_CPU 1, LAST_1ST Failed \n");
		}
	}
	else
	{
		if(uRet == false)
			{
				UART_Printf("HASH_SHA1_CPU 1, LAST_1ST Failed \n");
				g_sblock_err =1;
				UART_Getc();
			}
	}

	
	
	
}

#if 0
void Test_SHA1_INTERMEDIATE_T(void)
{
	u32 uRet, i, uTemp, j,k, uShift;

	
while(j<<0x1000)
{
	for(k=0;k<4;k++)
	{
	SBLK_Init(HASH, SHA1, ENC);
	//Outp32(0x7D300000, 0x9);

/*
	// Intermediate FIFO Setting
	Outp32(0x7D400000, 0x04);		// Rx Fifo Host Write Enable/Start
	Outp32(0x7D800000, 0x04);		// Tx Fifo Host Write Enable/Start

	Outp32(0x7D400004, 0x79);		// Rx Fifo Message Length
	Outp32(0x7D400008, 0x30010);	// Full 32bit, block Size 0x10
	Outp32(0x7D40000C, 0x7D300004);	// Rx FIFO Destination Address, HASH Rx DATA

	Outp32(0x7D800004, 0x5);		// Tx FIFO Message Length
	Outp32(0x7D800008, 0x10);
	//Outp32(0x7D80000C, 0x7D300034);	// Tx FIFO Source Address => Rx HASHOUT ??
	Outp32(0x7D80000C, 0x7D700034);	// Tx FIFO Source Address => Rx HASHOUT ??
	
	Outp32(0x7D400000, 0xB3);		// Rx FIFO Start 
	Outp32(0x7D800000, 0xB3);		// Tx FIFO Start	
*/

	SBLK_SetFifo(0x79, 0x30010, 5, 0x10, LASTBYTE_4TH);
	SBLK_StartFifo();

	//for(i=0;i<7;i++){
	for(i=0;i<6;i++){
	//Wait Write Enable...		
	SBLK_PutDataToRxFifo((u32)uSha1PlainText16, 16);
	}
	

	while(((Inp32(0x7D400000)>>8)&0xFF)>1);	// RX_FIFO Empty Polling
	while(((Inp32(0x7D300030)>>3)&0x3)==3);	// Ready to receivce

	Delay(10);
	//SBLK_GetDataFromTxFifo(OUTPUT_BUF, 5);
	
	Copy(0x7D30005c, MID_BUF, 5);


	// HMAC Operating.
	SBLK_Init(HASH, HMAC, ENC);
	SBLK_SetFifo(47, 16, 5, 16, LASTBYTE_4TH);
	SBLK_StartFifo();
	SBLK_PutDataToRxFifo((u32)uHmacPlainText47, 47);
	while(!SBLK_IsFifoTransDone());
	SBLK_GetDataFromTxFifo(OUTPUT_BUF, 5);
	Copy((u32)uHmacCiperText47, GOLDEN_BUF, 5);
	uRet = Compare(OUTPUT_BUF, GOLDEN_BUF, 5);
	if (uRet)
	{
		//UART_Printf("HASH_HMAC ,  Success \n");
		
	}
	else
	{
		UART_Printf("HASH_HMAC ,  Failed \n" );
	}

	// Start 2nd SHA1 Operation

	
	//Copy(MID_BUF, 0x7D300070, 5);		//Load Back-up Intermediate Value
	//Copy(MID_BUF, 0x7D700070, 5);

/*
	Copy(MID_BUF+0x10, 0x7D300070,1);
	Copy(MID_BUF+0xc, 0x7D300074,1);
	Copy(MID_BUF+8, 0x7D300078,1);
	Copy(MID_BUF+4, 0x7D30007c,1);
	Copy(MID_BUF, 0x7D300080,1);
*/
	

	Outp32(0x7D300070, 0xcd381861);		// Golden Data => MID Value
	Outp32(0x7D300074, 0x1a921f94);
	Outp32(0x7D300078, 0x6e76da53);
	Outp32(0x7D30007c, 0x5c52c230);
	Outp32(0x7D300080, 0x64e696cd);

	
/*
	Outp32(0x7D300070, 0x64e696cd);	//=> Fail... 0x364a80e1
	Outp32(0x7D300074, 0x5c52c230);
	Outp32(0x7D300078, 0x6e76da53);
	Outp32(0x7D30007c, 0x1a921f94);
	Outp32(0x7D300080, 0xcd381861);
*/
/*
	Outp32(0x7D300070, 0xcd96e664);	//=> Fail... 0x364a80e1
	Outp32(0x7D300074, 0x30c2525c);
	Outp32(0x7D300078, 0x53da766e);
	Outp32(0x7D30007c, 0x941f921a);
	Outp32(0x7D300080, 0x611838cd);
*/	
/*
	Outp32(0x7D300080, 0x611838cd);
	Outp32(0x7D30007c, 0x941f921a);
	Outp32(0x7D300078, 0x53da766e);
	Outp32(0x7D300074, 0x30c2525c);
	Outp32(0x7D300070, 0xcd96e664);
*/

	//UART_Printf("PRE_MSG_LENGTH 02\n");
	//uTemp = UART_GetIntNum();

	
	//Copy(MID_BUF, 0x7D700070, 5);		//Load Back-up Intermediate Value
	Outp32(0x7D300084, 0x0);
	Outp32(0x7D300088,j<<uShift);
	//Outp32(0x7D300088,0xC0000);
	//Outp32(0x7D300088,0x60000000);
	//Outp32(0x7D700088,0xC00);		// SHA1 Pre Length Load,  96Word => C00

	//Outp32(0x7D300000, 0x109);		// Usign Back-up IV, not SHA1 Constants
	//Outp32(0x7D300000, 0x1c9);		// Usign Back-up IV, not SHA1 Constants
	//Outp32(0x7D300000, 0x109);		// Usign Back-up IV, not SHA1 Constants
	//SBLK_Init_T(HASH, SHA1, ENC);

	//SBLK_Init(HASH, SHA1, ENC);
	
	Outp32(0x7D300000, 0x109);
	//SBLK_PutDataToInReg((u32)uSha1PlainText47, 25, LASTBYTE_4TH);
	//SBLK_PutDataToInReg1((u32)uSha1PlainText47, 25, LASTBYTE_4TH);

	Outp32(0x7D300004, 0x64636261);

	//Outp32(0x7D300000, 0x1);
	for(i=0;i<23;i++){
			Outp32(0x7D300004, 0x64636261);
					}
	Outp32(0x7D300000, 0xD1);
	Outp32(0x7D300004, 0x64636261);

	
	while(!SBLK_IsOutputReady());
	//while(!SBLK_IsFifoTransDone());
	SBLK_GetDataFromOutReg(OUTPUT_BUF, 5);
	//SBLK_GetDataFromTxFifo(OUTPUT_BUF, 5);

	Copy((u32)uSha1CipherText_Inter, GOLDEN_BUF, 5);
	uRet = Compare(OUTPUT_BUF, GOLDEN_BUF, 5);
	if (uRet)
	{
		UART_Printf("HASH_SHA1_CPU 1, LAST_1ST Success \n");
		UART_Printf("PRE_LENGTH : 0x%x\n",j);
		break;
		
	}
	else
	{
		UART_Printf("HASH_SHA1_CPU 1 0x%x, LAST_1ST Failed \n", j);
	}

	uShift = uShift +8;
	}
	j++;

}	

	UART_Printf(" \n");
	
}
#endif

void Test_CBC_IV_FIFO(eFunction_Test eTest, oFunctionT_AutoVar oAutoVar)
{
	u32 uRet;

	//g_sblock_err =0;

	if(eTest == eTest_Manual)
	{
		UART_Printf("[SBLOCK - AES(CBC) IV FIFO Test]\n");

		UART_Printf(" Choose AES Key Select :  0 : 128bit,  1:192bit,  2:256bits \n");
		g_AES_Key = UART_GetIntNum();
		
	}
	else
	{
		g_AES_Key = oAutoVar.Test_Parameter[0];
	}

	 switch(g_AES_Key)
	 	{
		case 0:
	 		Copy((u32)uAesCipherText128, GOLDEN_BUF, 16);
	 		Copy((u32)uAesPlainText, GOLDEN_BUF1, 16);
		   
			 break;
		case 1:
			Copy((u32)uAesCipherText192, GOLDEN_BUF, 16);
			Copy((u32)uAesPlainText, GOLDEN_BUF1, 16);
			 break;
	
		case 2:
			Copy((u32)uAesCipherText256, GOLDEN_BUF, 16);
			Copy((u32)uAesPlainText, GOLDEN_BUF1, 16);
			 break;
	 	}

	// Encryption
	//SBLK_Init(AES, CBC, ENC);
	SBLK_Init_Test2(AES, CBC, ENC);
	SBLK_SetFifo(8, 4, 8, 4, LASTBYTE_1ST);
	SBLK_StartFifo();
	SBLK_PutDataToRxFifo((u32)uAesPlainText, 8);
	
	while(!SBLK_IsFifoTransDone());

	// Intermediate IV Back-up
	Copy(0x7D1000A0, MID_BUF, 4);
	SBLK_GetDataFromTxFifo(OUTPUT_BUF, 8);
	
	//Copy((u32)uAesCipherText[CBC], GOLDEN_BUF, 16);
	uRet = Compare(OUTPUT_BUF, GOLDEN_BUF, 8);

	if(eTest == eTest_Manual)
	{
		if (uRet)
		{
		UART_Printf("AES_");
		DispOperMode(eAesOperMode);
		UART_Printf("_IV_1st_ENC Success..\n");
		}
		else
		{
		UART_Printf("AES");
		DispOperMode(eAesOperMode);
		UART_Printf("_IV_1st_ENC Failed..\n");
		}
	}
	else
	{
		if(uRet == false)
			{
				UART_Printf("_IV_1st_ENC Failed..\n");
				g_sblock_err =1;
				UART_Getc();
			}
	}
	
	////////////////////////////
	// Decryption
	//SBLK_Init(AES, CBC, DEC);
	SBLK_Init_Test2(AES, CBC, DEC);
	SBLK_SetFifo(8, 4, 8, 4, LASTBYTE_1ST);
	SBLK_StartFifo();
	SBLK_PutDataToRxFifo(GOLDEN_BUF, 8);

	while(!SBLK_IsFifoTransDone());

	// Intermediate IV Back-up
	Copy(0x7D1000A0, MID_BUF+0x20, 4);

	SBLK_GetDataFromTxFifo(INPUT_BUF, 8);
	//Copy((u32)uAesPlainText, GOLDEN_BUF, 16);
	uRet = Compare(INPUT_BUF, GOLDEN_BUF1, 8);

	if(eTest == eTest_Manual)
	{
		if (uRet)
		{
		UART_Printf("AES_");
		DispOperMode(eAesOperMode);
		UART_Printf("_IV_1st_DEC Success..\n");
		}
		else
		{
		UART_Printf("AES");
		DispOperMode(eAesOperMode);
		UART_Printf("_IV_1st_DEC Failed..\n");
		}
	}
	else
	{
		if(uRet == false)
			{
				UART_Printf("_IV_1st_DEC Failed..\n");
				g_sblock_err =1;
				UART_Getc();
			}
	}

	// 2nd Encryption 
	SBLK_Init_Test2(AES, CBC, ENC);
	Copy(MID_BUF, 0x7D1000A0, 4);	// Intermediate IV
		
	SBLK_SetFifo(8, 4, 8, 4, LASTBYTE_1ST);
	SBLK_StartFifo();
	SBLK_PutDataToRxFifo((u32)uAesPlainText+0x20, 8);
	
	while(!SBLK_IsFifoTransDone());

	// Intermediate IV Back-up
	
	SBLK_GetDataFromTxFifo(OUTPUT_BUF+0x20, 8);
	
	//Copy((u32)uAesCipherText[CBC], GOLDEN_BUF, 16);
	uRet = Compare(OUTPUT_BUF+0x20, GOLDEN_BUF+0x20, 8);

	if(eTest == eTest_Manual)
	{
		if (uRet)
		{
		UART_Printf("AES_");
		DispOperMode(eAesOperMode);
		UART_Printf("_IV_2nd_ENC Success..\n");
		}
		else
		{
		UART_Printf("AES");
		DispOperMode(eAesOperMode);
		UART_Printf("_IV_2nd_ENC Failed..\n");
		}
	}
	else
	{
		if(uRet == false)
			{
				UART_Printf("_IV_2nd_ENC Failed..\n");
				g_sblock_err =1;
				UART_Getc();
			}
	}


	// 2nd Decryption
	// Decryption

	SBLK_Init_Test2(AES, CBC, DEC);
	Copy(MID_BUF+0x20, 0x7D1000A0, 4);	// Intermediate IV
	SBLK_SetFifo(8, 4, 8, 4, LASTBYTE_1ST);
	SBLK_StartFifo();
	SBLK_PutDataToRxFifo(GOLDEN_BUF+0x20, 8);

	while(!SBLK_IsFifoTransDone());

	SBLK_GetDataFromTxFifo(INPUT_BUF+0x20, 8);
	//Copy((u32)uAesPlainText, GOLDEN_BUF, 16);
	uRet = Compare(INPUT_BUF+0x20, GOLDEN_BUF1+0x20, 8);

	if(eTest == eTest_Manual)
	{
		if (uRet)
		{
		UART_Printf("AES_");
		DispOperMode(eAesOperMode);
		UART_Printf("_IV_2nd_DEC Success..\n");
		}
		else
		{
		UART_Printf("AES");
		DispOperMode(eAesOperMode);
		UART_Printf("_IV_2nd_DEC Failed..\n");
		}
	}
	else
	{
		if(uRet == false)
			{
				UART_Printf("_IV_2nd_DEC Failed..\n");
				g_sblock_err =1;
				UART_Getc();
			}
	}
	
}

void Test_CTR_Counter(eFunction_Test eTest, oFunctionT_AutoVar oAutoVar)
{
	u32 uRet;

	//g_sblock_err =0;

	if(eTest == eTest_Manual)
	{
		UART_Printf("[SBLOCK - AES(CTR) Counter Edge Test]\n");

		UART_Printf(" Choose AES Key Select :  0 : 128bit,  1:192bit,  2:256bits \n");
		g_AES_Key = UART_GetIntNum();
		
	}
	else
	{
		g_AES_Key = oAutoVar.Test_Parameter[0];
	}

	 switch(g_AES_Key)
	 	{
		case 0:
	 		Copy((u32)uAesCTRCipherText, GOLDEN_BUF, 16);
	 		Copy((u32)uAesPlainText, GOLDEN_BUF1, 16);
		   
			 break;
		case 1:
			Copy((u32)uAesCTRCipherText+0x40, GOLDEN_BUF, 16);
			Copy((u32)uAesPlainText, GOLDEN_BUF1, 16);
			 break;
	
		case 2:
			Copy((u32)uAesCTRCipherText+0xc0, GOLDEN_BUF, 16);
			Copy((u32)uAesPlainText, GOLDEN_BUF1, 16);
			 break;
	 	}

	// Encryption
	//SBLK_Init(AES, CBC, ENC);
	SBLK_Init_Test3(AES, CTR, ENC);
	SBLK_SetFifo(16, 4, 16, 4, LASTBYTE_1ST);
	SBLK_StartFifo();
	SBLK_PutDataToRxFifo((u32)uAesPlainText, 16);
	
	while(!SBLK_IsFifoTransDone());

	// Intermediate IV Back-up
	
	SBLK_GetDataFromTxFifo(OUTPUT_BUF, 16);
	
	//Copy((u32)uAesCipherText[CBC], GOLDEN_BUF, 16);
	uRet = Compare(OUTPUT_BUF, GOLDEN_BUF, 16);
	
	if(eTest == eTest_Manual)
	{
		if (uRet)
		{
		UART_Printf("AES_");
		UART_Printf("_CTR_Counter1_ENC Success..\n");
		}
		else
		{
		UART_Printf("AES");
		UART_Printf("_CTR_Counter1_ENC Failed..\n");
		}
	}
	else
	{
		if(uRet == false)
			{
				UART_Printf("_CTR_Counter1_ENC Failed..\n");
				g_sblock_err =1;
				UART_Getc();
			}
	}

	// Decryption

	//SBLK_Init(AES, CBC, DEC);
	SBLK_Init_Test3(AES, CTR, ENC);
	SBLK_SetFifo(16, 4,16, 4, LASTBYTE_1ST);
	SBLK_StartFifo();
	SBLK_PutDataToRxFifo(GOLDEN_BUF, 16);
	//SBLK_PutDataToRxFifo(OUTPUT_BUF, 16);

	while(!SBLK_IsFifoTransDone());

	// Intermediate IV Back-up
	SBLK_GetDataFromTxFifo(INPUT_BUF, 16);
	//Copy((u32)uAesPlainText, GOLDEN_BUF, 16);
	uRet = Compare(INPUT_BUF, GOLDEN_BUF1, 16);

	if(eTest == eTest_Manual)
	{
		if (uRet)
		{
		UART_Printf("AES_");
		UART_Printf("_CTR_Counter_DEC Success..\n");
		}
		else
		{
		UART_Printf("AES");
		UART_Printf("_CTR_Counter_DEC Failed..\n");
		}
	}
	else
	{
		if(uRet == false)
			{
				UART_Printf("_CTR_Counter_DEC Failed..\n");
				g_sblock_err =1;
				UART_Getc();
			}
	}
	
}

void Test_CTR_ICTR_FIFO(eFunction_Test eTest, oFunctionT_AutoVar oAutoVar)
{
	
	u32 uRet;

	//g_sblock_err =0;

	if(eTest == eTest_Manual)
	{
		UART_Printf("[SBLOCK - AES(CTR) INCR FIFO Test]\n");

		UART_Printf(" Choose AES Key Select :  0 : 128bit,  1:192bit,  2:256bits \n");
		g_AES_Key = UART_GetIntNum();
		
	}
	else
	{
		g_AES_Key = oAutoVar.Test_Parameter[0];
	}

	 switch(g_AES_Key)
	 	{
		case 0:
	 		Copy((u32)uAesCTRCipherText_, GOLDEN_BUF, 16);
	 		Copy((u32)uAesPlainText, GOLDEN_BUF1, 16);
		   
			 break;
		case 1:
			Copy((u32)uAesCTRCipherText_+0x40, GOLDEN_BUF, 16);
			Copy((u32)uAesPlainText, GOLDEN_BUF1, 16);
			 break;
	
		case 2:
			Copy((u32)uAesCTRCipherText_+0x80, GOLDEN_BUF, 16);
			Copy((u32)uAesPlainText, GOLDEN_BUF1, 16);
			 break;
	 	}

	// Encryption
	//SBLK_Init(AES, CBC, ENC);
	SBLK_Init_Test2(AES, CTR, ENC);
	SBLK_SetFifo(8, 4, 8, 4, LASTBYTE_1ST);
	SBLK_StartFifo();
	SBLK_PutDataToRxFifo((u32)uAesPlainText, 8);
	
	while(!SBLK_IsFifoTransDone());

	// Intermediate CTR Back-up
	Copy(0x7D1000B0, MID_BUF, 4);
	SBLK_GetDataFromTxFifo(OUTPUT_BUF, 8);
	
	//Copy((u32)uAesCipherText[CBC], GOLDEN_BUF, 16);
	uRet = Compare(OUTPUT_BUF, GOLDEN_BUF, 8);
	if(eTest == eTest_Manual)
	{
		if (uRet)
		{
		UART_Printf("AES_");
		UART_Printf("_IV_1st_ENC Success..\n");
		}
		else
		{
		UART_Printf("AES");
		UART_Printf("_IV_1st_ENC Failed..\n");
		}
	}
	else
	{
		if(uRet == false)
			{
				UART_Printf("AES_CTR");
				UART_Printf("_IV_1st_ENC Failed..\n");
				g_sblock_err =1;
				UART_Getc();
			}
	}
	
	// Decryption

	//SBLK_Init(AES, CBC, DEC);
	SBLK_Init_Test2(AES, CTR, ENC);
	SBLK_SetFifo(8, 4, 8, 4, LASTBYTE_1ST);
	SBLK_StartFifo();
	SBLK_PutDataToRxFifo(GOLDEN_BUF, 8);

	while(!SBLK_IsFifoTransDone());

	// Intermediate CTR Back-up
	Copy(0x7D1000B0, MID_BUF+0x20, 4);

	SBLK_GetDataFromTxFifo(INPUT_BUF, 8);
	//Copy((u32)uAesPlainText, GOLDEN_BUF, 16);
	uRet = Compare(INPUT_BUF, GOLDEN_BUF1, 8);
	if(eTest == eTest_Manual)
	{
		if (uRet)
		{
		UART_Printf("AES_");
		UART_Printf("_IV_1st_DEC Success..\n");
		}
		else
		{
		UART_Printf("AES");
		UART_Printf("_IV_1st_DEC Failed..\n");
		}
	}
	else
	{
		if(uRet == false)
			{
				UART_Printf("AES_CTR");
				UART_Printf("_IV_1st_DEC Failed..\n");
				g_sblock_err =1;
				UART_Getc();
			}
	}


	// 2nd Encryption 
	SBLK_Init_Test2(AES, CTR, ENC);
	Copy(MID_BUF, 0x7D1000B0, 4);	// Intermediate IV
		
	SBLK_SetFifo(8, 4, 8, 4, LASTBYTE_1ST);
	SBLK_StartFifo();
	SBLK_PutDataToRxFifo((u32)uAesPlainText+0x20, 8);
	
	while(!SBLK_IsFifoTransDone());

	// Intermediate IV Back-up
	
	SBLK_GetDataFromTxFifo(OUTPUT_BUF+0x20, 8);
	
	//Copy((u32)uAesCipherText[CBC], GOLDEN_BUF, 16);
	uRet = Compare(OUTPUT_BUF+0x20, GOLDEN_BUF+0x20, 8);
	if(eTest == eTest_Manual)
	{
		if (uRet)
		{
		UART_Printf("AES_");
		UART_Printf("_IV_2nd_ENC Success..\n");
		}
		else
		{
		UART_Printf("AES");
		UART_Printf("_IV_2nd_ENC Failed..\n");
		}
	}
	else
	{
		if(uRet == false)
			{
				UART_Printf("AES_CTR");
				UART_Printf("_IV_2nd_ENC Failed..\n");
				g_sblock_err =1;
				UART_Getc();
			}
	}

	// 2nd Decryption
	// Decryption

	SBLK_Init_Test2(AES, CTR, ENC);
	Copy(MID_BUF+0x20, 0x7D1000B0, 4);	// Intermediate IV
	SBLK_SetFifo(8, 4, 8, 4, LASTBYTE_1ST);
	SBLK_StartFifo();
	SBLK_PutDataToRxFifo(GOLDEN_BUF+0x20, 8);

	while(!SBLK_IsFifoTransDone());

	SBLK_GetDataFromTxFifo(INPUT_BUF+0x20, 8);
	//Copy((u32)uAesPlainText, GOLDEN_BUF, 16);
	uRet = Compare(INPUT_BUF+0x20, GOLDEN_BUF1+0x20, 8);
	if(eTest == eTest_Manual)
	{
		if (uRet)
		{
		UART_Printf("AES_");
		UART_Printf("_IV_2nd_DEC Success..\n");
		}
		else
		{
		UART_Printf("AES");
		UART_Printf("_IV_2nd_DEC Failed..\n");
		}
	}
	else
	{
		if(uRet == false)
			{
				UART_Printf("AES_CTR");
				UART_Printf("_IV_2nd_DEC Failed..\n");
				g_sblock_err =1;
				UART_Getc();
			}
	}

	
}

void Test_AES_CONT_DEC(eFunction_Test eTest, oFunctionT_AutoVar oAutoVar)
{
	u32 uRet;

	//g_sblock_err=0;

	if(eTest == eTest_Manual)
	{
		UART_Printf("[SBLOCK - AES Continuos Decryption Test]\n");

	}
	else
	{
		
	}
	Copy((u32)uAesCipherText, GOLDEN_BUF, 16);
	Copy((u32)uAesPlainText, GOLDEN_BUF1, 16);
	
       ///////////////////////
	//ECB 128bit Decryption	
	SBLK_Init(AES, ECB, DEC);
	SBLK_SetFifo(0xc, 4, 0xc, 4, LASTBYTE_1ST);
	SBLK_StartFifo();
	SBLK_PutDataToRxFifo(GOLDEN_BUF, 4);

	while(((Inp32(0x7D800000)>>16)&0xFFFF)==0x4);	// Tx FIFO 1Word Read Done From AES

	Copy(0x7D1000A0, MID_BUF, 4);						// Intermedate IV Backup  
	SBLK_GetDataFromTxFifo(INPUT_BUF, 4);
	Outp32(0x7D400000, 0x4);
	Outp32(0x7D800000, 0x4);  // FIFO Reset
	
	uRet = Compare(INPUT_BUF, GOLDEN_BUF1, 4);
	
	
	if(eTest == eTest_Manual)
	{
		if (uRet)
		{
		UART_Printf("ECB 128bit DEC Success..\n");
		}
		else
		{
		
		UART_Printf("ECB 128bit DEC Failed..\n");
		}
	}
	else
	{
		if(uRet == false)
			{
				UART_Printf("ECB 128bit DEC Failed..\n");
				g_sblock_err =1;
				UART_Getc();
			}
	}
	////////////////////////////////
	//ECB 128bit Continuous Decryption
	Outp32(0x7D100000, 0x118); 		// 128Bit ECB Continuous Decryption.
	Copy(MID_BUF, 0x7D1000A0, 4);		// Load IV 
	SBLK_SetFifo(0xc, 4, 0xc, 4, LASTBYTE_1ST);
	SBLK_StartFifo();
	SBLK_PutDataToRxFifo(GOLDEN_BUF+0x10, 12);
	while(((Inp32(0x7D800000)>>25)&0xFF)==0x1);	// Transfer Done Polling
	
	SBLK_GetDataFromTxFifo(INPUT_BUF+0x10, 12);
	uRet = Compare(INPUT_BUF+0x10, GOLDEN_BUF1+0x10, 12);

	if(eTest == eTest_Manual)
	{
		if (uRet)
		{
		UART_Printf("ECB 128bit Cont. DEC Success..\n");
		}
		else
		{
		
		UART_Printf("ECB 128bit Cont. DEC Failed..\n");
		}
	}
	else
	{
		if(uRet == false)
			{
				UART_Printf("ECB 128bit Cont. DEC Failed..\n");
				g_sblock_err =1;
				UART_Getc();
			}
	}
	
	

	//////////////////////////////////////////////

	Copy((u32)uAesCipherText+0x40, GOLDEN_BUF, 16);

	///////////////////////////////////////
	//CBC 128bit Decryption	
	SBLK_Init(AES, CBC, DEC);
	SBLK_SetFifo(0xc, 4, 0xc, 4, LASTBYTE_1ST);
	SBLK_StartFifo();
	SBLK_PutDataToRxFifo(GOLDEN_BUF, 4);

	while(((Inp32(0x7D800000)>>16)&0xFFFF)==0x4);	// Tx FIFO 1Word Read Done From AES
	
	Copy(0x7D1000A0, MID_BUF, 4);						// Intermedate IV Backup
	SBLK_GetDataFromTxFifo(INPUT_BUF, 4);

	Outp32(0x7D400000, 0x4);
	Outp32(0x7D800000, 0x4);  // FIFO Reset
	
	uRet = Compare(INPUT_BUF, GOLDEN_BUF1, 4);

	if(eTest == eTest_Manual)
	{
		if (uRet)
		{
		UART_Printf("CBC 128bit DEC Success..\n");
		}
		else
		{
		
		UART_Printf("CBC 128bit DEC Failed..\n");
		}
	}
	else
	{
		if(uRet == false)
			{
				UART_Printf("CBC 128bit DEC Failed..\n");
				g_sblock_err =1;
				UART_Getc();
			}
	}

	
	///////////////////////////////////////
	//CBC 128bit Continuous Decryption
	Outp32(0x7D100000, 0x128); 		// 128Bit CBC Continuous Decryption.
	Copy(MID_BUF, 0x7D1000A0, 4);		// Load IV 
	
	SBLK_SetFifo(0xc, 4, 0xc, 4, LASTBYTE_1ST);
	SBLK_StartFifo();
	SBLK_PutDataToRxFifo(GOLDEN_BUF+0x10, 12);
	while(((Inp32(0x7D800000)>>25)&0xFF)==0x1);	// Transfer Done Polling
	
	SBLK_GetDataFromTxFifo(INPUT_BUF+0x10, 12);


	uRet = Compare(INPUT_BUF+0x14, GOLDEN_BUF1+0x14, 11);

	if(eTest == eTest_Manual)
	{
		if (uRet)
		{
		UART_Printf("CBC 128bit Cont. DEC Success..\n");
		}
		else
		{
		
		UART_Printf("CBC 128bit Cont. DEC Failed..\n");
		}
	}
	else
	{
		if(uRet == false)
			{
				UART_Printf("CBC 128bit Cont. DEC Failed..\n");
				g_sblock_err =1;
				UART_Getc();
			}
	}
	

	//////////////////////////////////////////////

	Copy((u32)uAesCipherText+0x80, GOLDEN_BUF, 16);


	//CBC 128bit Decryption	
	SBLK_Init(AES, CTR, ENC);
	SBLK_SetFifo(0xc, 4, 0xc, 4, LASTBYTE_1ST);
	SBLK_StartFifo();
	SBLK_PutDataToRxFifo(GOLDEN_BUF, 4);

	while(((Inp32(0x7D800000)>>16)&0xFFFF)==0x4);	// Tx FIFO 1Word Read Done From AES
	
	Copy(0x7D1000B0, MID_BUF, 4);						// Intermedate CTR Backup
	SBLK_GetDataFromTxFifo(INPUT_BUF, 4);

	Outp32(0x7D400000, 0x4);
	Outp32(0x7D800000, 0x4);  // FIFO Reset
	
	uRet = Compare(INPUT_BUF, GOLDEN_BUF1, 4);

	if(eTest == eTest_Manual)
	{
		if (uRet)
		{
		UART_Printf("CTR 128bit DEC Success..\n");
		}
		else
		{
		
		UART_Printf("CTR 128bit DEC Failed..\n");
		}
	}
	else
	{
		if(uRet == false)
			{
				UART_Printf("CTR 128bit DEC Failed..\n");
				g_sblock_err =1;
				UART_Getc();
			}
	}
	
	
	//ECB 128bit Continuous Decryption
	Outp32(0x7D100000, 0x130); 		// 128Bit CBC Continuous Decryption.
	Copy(MID_BUF, 0x7D1000B0, 4);		// Load CTR
	SBLK_SetFifo(0xc, 4, 0xc, 4, LASTBYTE_1ST);
	SBLK_StartFifo();
	SBLK_PutDataToRxFifo(GOLDEN_BUF+0x10, 12);
	while(((Inp32(0x7D800000)>>25)&0xFF)==0x1);	// Transfer Done Polling
	
	SBLK_GetDataFromTxFifo(INPUT_BUF+0x10, 12);


	uRet = Compare(INPUT_BUF+0x10, GOLDEN_BUF1+0x10, 12);


	if(eTest == eTest_Manual)
	{
		if (uRet)
		{
		UART_Printf("CTR 128bit Cont. DEC Success..\n");
		}
		else
		{
		
		UART_Printf("CTR 128bit Cont. DEC Failed..\n");
		}
	}
	else
	{
		if(uRet == false)
			{
				UART_Printf("CTR 128bit Cont. DEC Failed..\n");
				g_sblock_err =1;
				UART_Getc();
			}
	}

	
}

#if 0
void Test_AES_Temp(void)
{
	u32 uRet, i;

	// Encryption
	//SBLK_Init(AES, CBC, ENC);
	SBLK_Init_Test(AES, eAesOperMode, ENC);

	for (i=0; i<4; i++)
	{
		SBLK_PutDataToInReg((u32)uAesPlainText+i*4*4, 4, LASTBYTE_1ST);	// LASTBYTE => not affect to AES operating
		SBLK_StartByManual();

		while(!SBLK_IsOutputReady());

		SBLK_GetDataFromOutReg(OUTPUT_BUF+i*4*4, 4);
	}


	// Decryption
	if (eAesOperMode == CTR)
		SBLK_Init_Test(AES, eAesOperMode, ENC);
	else
		SBLK_Init_Test(AES, eAesOperMode, DEC);

	for (i=0; i<4; i++)
	{
		SBLK_PutDataToInReg(OUTPUT_BUF+i*4*4, 4, LASTBYTE_1ST);
		SBLK_StartByManual();

		while(!SBLK_IsOutputReady());

		SBLK_GetDataFromOutReg(GOLDEN_BUF+i*4*4, 4);
	}

	Copy((u32)uAesPlainText, INPUT_BUF, 16);		// Original Data
	uRet = Compare(INPUT_BUF, GOLDEN_BUF, 16);					// Original ->ENC->DEC
	if (uRet)
	{
		UART_Printf("AES_");
		DispOperMode(eAesOperMode);
		UART_Printf("_CPU Success..\n");
	}
	else
	{
		UART_Printf("AES");
		DispOperMode(eAesOperMode);
		UART_Printf("_CPU Failed..\n");
	}
}


void Test_DES_Temp(void)
{
	u32 uRet, i;

	// Encryption
	SBLK_Init_Test(DES, eDesOperMode, ENC);

	for (i=0; i<8; i++)
	{
		SBLK_PutDataToInReg((u32)uDesPlainText+i*4*2, 2, LASTBYTE_1ST);
		SBLK_StartByManual();

		while(!SBLK_IsOutputReady());

		SBLK_GetDataFromOutReg(OUTPUT_BUF+i*4*2, 2);
	}


	// Decryption
	SBLK_Init_Test(DES, eDesOperMode, DEC);

	for (i=0; i<8; i++)
	{
		SBLK_PutDataToInReg(OUTPUT_BUF+i*4*2, 2, LASTBYTE_1ST);
		SBLK_StartByManual();

		while(!SBLK_IsOutputReady());

		SBLK_GetDataFromOutReg(GOLDEN_BUF+i*4*2, 2);
	}
	Copy((u32)uDesPlainText, INPUT_BUF, 16);
	uRet = Compare(INPUT_BUF, GOLDEN_BUF, 16);
	if (uRet)
	{
		UART_Printf("DES_");
		DispOperMode(eDesOperMode);
		UART_Printf("_CPU_(TDES)Success..\n");
	}
	else
	{
		UART_Printf("DES");
		DispOperMode(eDesOperMode);
		UART_Printf("_CPU_(TDES) Failed..\n");
	}
}


void Temp(void)
{
	u32 uRet,i;

	SBLK_Init(HASH, SHA1, ENC);

	//SHA1 - #0
	/*
	for(i=0; i<7; i++){
	SBLK_PutDataToInReg((u32)uSha1PlainText16, 16, LASTBYTE_4TH);
	}
	SBLK_PutDataToInReg((u32)uSha1PlainText16, 9, LASTBYTE_4TH); 		// bfb94b76, 5f566e1b
	*/
	SBLK_PutDataToInReg((u32)uSha1PlainText121, 25, LASTBYTE_4TH);
	
	while(!SBLK_IsOutputReady());

	SBLK_GetDataFromOutReg(OUTPUT_BUF, 5);

	Copy((u32)uSha1CipherText1, GOLDEN_BUF, 5);
	uRet = Compare(OUTPUT_BUF, GOLDEN_BUF, 5);
	if (uRet)
	{
		UART_Printf("HASH_SHA1_CPU 121 \n");
		
	}
	else
	{
		UART_Printf("HASH_SHA1_CPU 121  \n");
	}


	UART_Printf(" \n");
	
}


void Temp3(void)
{
	u32 uRet, i ;

	SBLK_Init(HASH, SHA1, ENC);
	
	//SBLK_SetFifo(0x79, 0x30010, 5, 0x10, LASTBYTE_4TH);
	SBLK_SetFifo(0x79, 0x30010, 5, 0x10, LASTBYTE_4TH);
	SBLK_StartFifo();
	
	for(i=0;i<6;i++){
	//Wait Write Enable...		
	SBLK_PutDataToRxFifo((u32)uSha1PlainText16, 16);
	}
	SBLK_PutDataToRxFifo((u32)uSha1PlainText16, 16);
	SBLK_PutDataToRxFifo((u32)uSha1PlainText16, 9);
	while(!SBLK_IsFifoTransDone());

	
	SBLK_GetDataFromTxFifo(OUTPUT_BUF, 5);
	
	Copy((u32)uSha1CipherText16, GOLDEN_BUF, 5);
	uRet = Compare(OUTPUT_BUF, GOLDEN_BUF, 5);
	if (uRet)
	{
		UART_Printf("HASH_");
		DispOperMode(eHashOperMode);
		if (eHashOperMode == HMAC)
			UART_Printf("_FIFO 19 Success..\n");
		else if (eHashOperMode == SHA1)
			UART_Printf("_FIFO 16 Success..\n");
	}
	else
	{
		UART_Printf("HASH_");
		DispOperMode(eHashOperMode);
		if (eHashOperMode == HMAC)
			UART_Printf("_FIFO 19 Failed..\n");
		else if (eHashOperMode == SHA1)
			UART_Printf("_FIFO 16 Failed..\n");
	}

	
}

#endif

#if 0
void SBlock_AutoT(void)
{
	u32 usel;
	
	UART_Printf("\n[SBLOCK Test]\n");
	
	///////////////////////////////////////
	/////////  AES Test   ///////////////////
	for(usel=1;usel<4;usel++)
	{
		if (usel == 1) eAesOperMode = ECB;
		else if (usel == 2) eAesOperMode = CBC;
		else if (usel == 3) eAesOperMode = CTR;	
	Test_AES_CPU();
	Test_AES_FIFO();
	Test_AES_DMA();

	}

	Test_CBC_IV_FIFO();
	Test_CTR_Counter();
	Test_CTR_ICTR_FIFO();
	Test_AES_CONT_DEC();

	///////////////////////////////////////
	/////////  DES Test   ///////////////////
	for(usel=1;usel<3;usel++)
	{
		if (usel == 1) eDesOperMode = ECB;
		else if (usel == 2) eDesOperMode = CBC;
	Test_DES_CPU();
	Test_DES_FIFO();
	Test_DES_DMA();
	}

	///////////////////////////////////////
	/////////  HASH Test   ///////////////////
	for(usel=1;usel<3;usel++)
	{
		if (usel == 1) eHashOperMode = HMAC;
		else if (usel == 2) eHashOperMode = SHA1;
	Test_HASH_CPU();
	Test_HASH_FIFO();
	Test_HASH_DMA();

	}

	Test_HASH_CPU_Added();
	Test_SHA1_INTERMEDIATE();

	eHashOperMode=PRNG;
	Test_HASH_FIFO();


	UART_Printf("End of Test\n");

	
}
#endif

const AutotestFuncMenu sblock_aes[] =
{
	Test_AES_CPU,             				"CPU Mode Test",
	Test_AES_FIFO,						"FIFO Mode Test",
	Test_AES_DMA,						"DMA Mode Test",
	Test_AES_CONT_DEC, 				"Continuous Decryption Test",
	Test_CBC_IV_FIFO,					"Test AES(CBC) IV FIFO Test",
	Test_CTR_Counter,					"Test AES(CTR) Counter Edge Test",
	Test_CTR_ICTR_FIFO, 				"Test AES(CTR) INCR FIFO Test",
	0, 0
};


void SblockT_AES(void)
{
	u32 i;
	s32 uSel;
	oFunctionT_AutoVar oParameter;
	
	while(1)
	{
		for (i=0; (u32)(sblock_aes[i].desc)!=0; i++)
			UART_Printf("%2d: %s\n", i, sblock_aes[i].desc);

		UART_Printf("\nSelect the function to test : \n");
		uSel =UART_GetIntNum();
		UART_Printf("\n");
		if(uSel == -1) 
			break;

		if (uSel>=0 && uSel<(sizeof(sblock_aes)/8-1))
			(sblock_aes[uSel].func) (eTest_Manual, oParameter);
	}

}


const AutotestFuncMenu sblock_des[] =
{
	Test_DES_CPU,             				"CPU Mode Test",
	Test_DES_FIFO,						"FIFO Mode Test",
	Test_DES_DMA,						"DMA Mode Test",
	
	0, 0
};


void SblockT_DES(void)
{
	u32 i;
	s32 uSel;
	oFunctionT_AutoVar oParameter;
	
	while(1)
	{
		for (i=0; (u32)(sblock_des[i].desc)!=0; i++)
			UART_Printf("%2d: %s\n", i, sblock_des[i].desc);

		UART_Printf("\nSelect the function to test : \n");
		uSel =UART_GetIntNum();
		UART_Printf("\n");
		if(uSel == -1) 
			break;

		if (uSel>=0 && uSel<(sizeof(sblock_des)/8-1))
			(sblock_des[uSel].func) (eTest_Manual, oParameter);
	}

}

const AutotestFuncMenu sblock_hash[] =
{
		Test_HASH_CPU,			"Test_HASH_CPU",
		Test_HASH_FIFO,		"Test_HASH_FIFO",
		Test_HASH_DMA,			"Test_HASH_DMA",

		Test_HASH_CPU_Added,	"Test_HASH_CPU_Added",
		Test_SHA1_INTERMEDIATE,	"Test_HASH(SHA1) Intermediate FIFO",
	
	0, 0
};


void SblockT_HASH(void)
{
	u32 i;
	s32 uSel;
	oFunctionT_AutoVar oParameter;
	
	while(1)
	{
		for (i=0; (u32)(sblock_hash[i].desc)!=0; i++)
			UART_Printf("%2d: %s\n", i, sblock_hash[i].desc);

		UART_Printf("\nSelect the function to test : \n");
		uSel =UART_GetIntNum();
		UART_Printf("\n");
		if(uSel == -1) 
			break;

		if (uSel>=0 && uSel<(sizeof(sblock_hash)/8-1))
			(sblock_hash[uSel].func) (eTest_Manual, oParameter);
	}

}

///////  Error detecton must be changed.......
void SblockT_FullFunction(void)
{
	u32 i, uNumMode, uRegValue;
	oFunctionT_AutoVar oAutoTest;	
	u32 aCaseOfAES[] = { ECB, CBC, CTR};  
	u32 aCaseOfDES[] = { ECB, CBC}; 
	u32 aCaseOfHASH[] = {  HMAC, SHA1, PRNG}; 
	

	uRegValue= Inp32SYSC(0x20);
	uRegValue = uRegValue&~(3<<18)|(3<<18);	// SECURE_RATIO = 3
	Outp32SYSC(0x20, uRegValue);

	
	UART_Printf("\n [ Sblock Full Function Test ]\n");
	UART_Printf("============================================= \n");
	
	//////////////////////////////////////////
	//////   AES Test
	g_sblock_err = 0;
	uNumMode = sizeof(aCaseOfAES)/sizeof(aCaseOfAES[0]);
	for(i=0; i<uNumMode;i++)
	{
		oAutoTest.Test_Parameter[0] = aCaseOfAES[i];
	 	Test_AES_CPU(eTest_Auto, oAutoTest );
	 	Test_AES_FIFO(eTest_Auto, oAutoTest );
	 	Test_AES_DMA(eTest_Auto, oAutoTest );
	}
	
	for(i=0; i<3;i++)
	{
		oAutoTest.Test_Parameter[0] = i;
		Test_CBC_IV_FIFO(eTest_Auto, oAutoTest );
		Test_CTR_Counter(eTest_Auto, oAutoTest );
		Test_CTR_ICTR_FIFO(eTest_Auto, oAutoTest );

	}
		

	
	if(g_sblock_err == 0)
	{
		UART_Printf("	AES Operation :			PASS \n");
	}
	else
	{
		UART_Printf("	AES Operation :			FAIL\n");
	}


	//////////////////////////////////////////
	//////   DES Test
	g_sblock_err = 0;
	uNumMode = sizeof(aCaseOfDES)/sizeof(aCaseOfDES[0]);
	for(i=0; i<uNumMode;i++)
	{
		oAutoTest.Test_Parameter[0] = aCaseOfDES[i];
	 	Test_DES_CPU(eTest_Auto, oAutoTest );
	 	Test_DES_FIFO(eTest_Auto, oAutoTest );
	 	Test_DES_DMA(eTest_Auto, oAutoTest );
	}

	if(g_sblock_err == 0)
	{
		UART_Printf("	DES Operation :			PASS \n");
	}
	else
	{
		UART_Printf("	DES Operation :			FAIL\n");
	}

	//////////////////////////////////////////
	//////   HASH Test
	g_sblock_err = 0;
	uNumMode = sizeof(aCaseOfHASH)/sizeof(aCaseOfHASH[0]);
	for(i=0; i<uNumMode;i++)
	{
		oAutoTest.Test_Parameter[0] = aCaseOfHASH[i];
	 	Test_HASH_CPU(eTest_Auto, oAutoTest );
	 	Test_HASH_FIFO(eTest_Auto, oAutoTest );
	 	Test_HASH_DMA(eTest_Auto, oAutoTest );
	}
		Test_HASH_CPU_Added(eTest_Auto, oAutoTest );
		Test_SHA1_INTERMEDIATE(eTest_Auto, oAutoTest );
	
	if(g_sblock_err == 0)
	{
		UART_Printf("	HASH Operation :			PASS \n");
	}
	else
	{
		UART_Printf("	HASH Operation :			FAIL\n");
	}


	// INTC Clear
	uRegValue = Inp32(0x7D000000);
	Outp32(0x7D000000, uRegValue&0x0);

}


///////  Error detecton must be changed.......
u8  Sblock_AES_autotest(void)
{
	u32 i, uNumMode, uRegValue;
	oFunctionT_AutoVar oAutoTest;	
	u32 aCaseOfAES[] = { ECB, CBC, CTR};  
	//u32 aCaseOfDES[] = { ECB, CBC}; 
	//u32 aCaseOfHASH[] = {  HMAC, SHA1, PRNG}; 

	//int f = 0;

	uRegValue= Inp32SYSC(0x20);
	uRegValue = uRegValue&~(3<<18)|(3<<18);	// SECURE_RATIO = 3
	//Outp32SYSC(0x20, uRegValue);

	
	UART_Printf("\n [ Sblock AES Function Test ]\n");
	UART_Printf("============================================= \n");
	
	//////////////////////////////////////////
	//////   AES Test
	g_sblock_err = 0;
	uNumMode = sizeof(aCaseOfAES)/sizeof(aCaseOfAES[0]);
	for(i=0; i<uNumMode;i++)
	{
		oAutoTest.Test_Parameter[0] = aCaseOfAES[i];
	 	Test_AES_CPU(eTest_Auto, oAutoTest );
	 	Test_AES_FIFO(eTest_Auto, oAutoTest );
	 	Test_AES_DMA(eTest_Auto, oAutoTest );
	}
	
	for(i=0; i<3;i++)
	{
		oAutoTest.Test_Parameter[0] = i;
		Test_CBC_IV_FIFO(eTest_Auto, oAutoTest );
		Test_CTR_Counter(eTest_Auto, oAutoTest );
		Test_CTR_ICTR_FIFO(eTest_Auto, oAutoTest );

	}
		

	// INTC Clear
	uRegValue = Inp32(0x7D000000);
	Outp32(0x7D000000, uRegValue&0x0);

	
	if(g_sblock_err == 0)
	{
		UART_Printf("	AES Operation :			PASS \n");
		return true;
	}
	else
	{
		UART_Printf("	AES Operation :			FAIL\n");
		return false;
	}


	
}


u8 Sblock_DES_autotest(void)
{
	u32 i, uNumMode, uRegValue;
	oFunctionT_AutoVar oAutoTest;	
	//u32 aCaseOfAES[] = { ECB, CBC, CTR};  
	u32 aCaseOfDES[] = { ECB, CBC}; 
	//u32 aCaseOfHASH[] = {  HMAC, SHA1, PRNG}; 
	

	uRegValue= Inp32SYSC(0x20);
	uRegValue = uRegValue&~(3<<18)|(3<<18);	// SECURE_RATIO = 3
	//Outp32SYSC(0x20, uRegValue);

	
	UART_Printf("\n [ Sblock DES Function Test ]\n");
	UART_Printf("============================================= \n");
	
	
	//////////////////////////////////////////
	//////   DES Test
	g_sblock_err = 0;
	uNumMode = sizeof(aCaseOfDES)/sizeof(aCaseOfDES[0]);
	for(i=0; i<uNumMode;i++)
	{
		oAutoTest.Test_Parameter[0] = aCaseOfDES[i];
	 	Test_DES_CPU(eTest_Auto, oAutoTest );
	 	Test_DES_FIFO(eTest_Auto, oAutoTest );
	 	Test_DES_DMA(eTest_Auto, oAutoTest );
	}

	uRegValue = Inp32(0x7D000000);
	Outp32(0x7D000000, uRegValue&0x0);


	if(g_sblock_err == 0)
	{
		UART_Printf("	DES Operation :			PASS \n");
		return true;
	}
	else
	{
		UART_Printf("	DES Operation :			FAIL\n");
		return false;
	}

	
}

///////  Error detecton must be changed.......
u8 Sblock_HASH_autotest(void)
{
	u32 i, uNumMode, uRegValue;
	oFunctionT_AutoVar oAutoTest;	
	//u32 aCaseOfAES[] = { ECB, CBC, CTR};  
	//u32 aCaseOfDES[] = { ECB, CBC}; 
	u32 aCaseOfHASH[] = {  HMAC, SHA1, PRNG}; 
	

	uRegValue= Inp32SYSC(0x20);
	uRegValue = uRegValue&~(3<<18)|(3<<18);	// SECURE_RATIO = 3
	//Outp32SYSC(0x20, uRegValue);

	
	UART_Printf("\n [ Sblock HASH Function Test ]\n");
	UART_Printf("============================================= \n");
	
	
	//////////////////////////////////////////
	//////   HASH Test
	g_sblock_err = 0;
	uNumMode = sizeof(aCaseOfHASH)/sizeof(aCaseOfHASH[0]);
	for(i=0; i<uNumMode;i++)
	{
		oAutoTest.Test_Parameter[0] = aCaseOfHASH[i];
	 	Test_HASH_CPU(eTest_Auto, oAutoTest );
	 	Test_HASH_FIFO(eTest_Auto, oAutoTest );
	 	Test_HASH_DMA(eTest_Auto, oAutoTest );
	}
		Test_HASH_CPU_Added(eTest_Auto, oAutoTest );
		Test_SHA1_INTERMEDIATE(eTest_Auto, oAutoTest );

	uRegValue = Inp32(0x7D000000);
	Outp32(0x7D000000, uRegValue&0x0);	
	INTC_Disable(NUM_SDMA1);
	
	if(g_sblock_err == 0)
	{
		UART_Printf("	HASH Operation :			PASS \n");
		return true;
	}
	else
	{
		UART_Printf("	HASH Operation :			FAIL\n");
		return false;
	}


}




///////  Error detecton must be changed.......
void SblockT_AgingFunction(void)
{
	u32 i, uNumMode, uRegValue;
	oFunctionT_AutoVar oAutoTest;	
	u32 aCaseOfAES[] = { ECB, CBC, CTR};  
	u32 aCaseOfDES[] = { ECB, CBC}; 
	u32 aCaseOfHASH[] = {  HMAC, SHA1, PRNG}; 
	u32 utest, uClock;
	u32 uAES_err, uDES_err, uHASH_err;
	
	uRegValue= Inp32SYSC(0x20);
	uRegValue = uRegValue&~(3<<18)|(3<<18);	// SECURE_RATIO = 3
	Outp32SYSC(0x20, uRegValue);


	
	UART_Printf("\n [ Sblock Full Function Test ]\n");
	UART_Printf("============================================= \n");

for(uClock=0;uClock<2;uClock++)
{
	if(uClock==1) 
		{
			uRegValue= Inp32SYSC(0x20);
			uRegValue = uRegValue&~(3<<18)|(1<<18);	// SECURE_RATIO =1
			//Outp32SYSC(0x20, uRegValue);
			UART_Printf("[ SECUR_CLK=HCLK ]\n");
		}
	if(uClock==0)
		{
			uRegValue= Inp32SYSC(0x20);
			uRegValue = uRegValue&~(3<<18)|(3<<18);	// SECURE_RATIO = 3
			//Outp32SYSC(0x20, uRegValue);
			UART_Printf("[ SECUR_CLK=HCLK/2 ]\n");
		}
	
	utest=0;
	uAES_err=0;
	uDES_err=0;
	uHASH_err=0;
	
	while(utest<200000)
	{
	 if(UART_GetKey()!=0x0) break;
	 if(utest%10000 == 0)  UART_Printf("test no(AES_err)(DES_err)(HASH_err) : %d(%d)(%d)(%d) \n", utest,uAES_err, uDES_err,uHASH_err);
	//////////////////////////////////////////
	//////   AES Test
	
	g_sblock_err = 0;
	uNumMode = sizeof(aCaseOfAES)/sizeof(aCaseOfAES[0]);
	for(i=0; i<uNumMode;i++)
	{
		oAutoTest.Test_Parameter[0] = aCaseOfAES[i];
	 	Test_AES_CPU(eTest_Auto, oAutoTest );
	 	Test_AES_FIFO(eTest_Auto, oAutoTest );
	 	Test_AES_DMA(eTest_Auto, oAutoTest );
	}
	
	for(i=0; i<3;i++)
	{
		oAutoTest.Test_Parameter[0] = i;
		Test_CBC_IV_FIFO(eTest_Auto, oAutoTest );
		Test_CTR_Counter(eTest_Auto, oAutoTest );
		Test_CTR_ICTR_FIFO(eTest_Auto, oAutoTest );

	}
		

	
	if(g_sblock_err == 0)
	{
		//UART_Printf("	AES Operation :			PASS \n");
		
	}
	else
	{
		//UART_Printf("	AES Operation :			FAIL\n");
		uAES_err++;
	}


	//////////////////////////////////////////
	//////   DES Test
	g_sblock_err = 0;
	uNumMode = sizeof(aCaseOfDES)/sizeof(aCaseOfDES[0]);
	for(i=0; i<uNumMode;i++)
	{
		oAutoTest.Test_Parameter[0] = aCaseOfDES[i];
	 	Test_DES_CPU(eTest_Auto, oAutoTest );
	 	Test_DES_FIFO(eTest_Auto, oAutoTest );
	 	Test_DES_DMA(eTest_Auto, oAutoTest );
	}

	if(g_sblock_err == 0)
	{
		//UART_Printf("	DES Operation :			PASS \n");
	}
	else
	{
		//UART_Printf("	DES Operation :			FAIL\n");
		uDES_err++;
	}

	//////////////////////////////////////////
	//////   HASH Test
	g_sblock_err = 0;
	uNumMode = sizeof(aCaseOfHASH)/sizeof(aCaseOfHASH[0]);
	for(i=0; i<uNumMode;i++)
	{
		oAutoTest.Test_Parameter[0] = aCaseOfHASH[i];
	 	Test_HASH_CPU(eTest_Auto, oAutoTest );
	 	Test_HASH_FIFO(eTest_Auto, oAutoTest );
	 	Test_HASH_DMA(eTest_Auto, oAutoTest );
	}
		Test_HASH_CPU_Added(eTest_Auto, oAutoTest );
		Test_SHA1_INTERMEDIATE(eTest_Auto, oAutoTest );
	
	if(g_sblock_err == 0)
	{
		//UART_Printf("	HASH Operation :			PASS \n");
	}
	else
	{
		//UART_Printf("	HASH Operation :			FAIL\n");
		uHASH_err++;
	}


	// INTC Clear
	uRegValue = Inp32(0x7D000000);
	Outp32(0x7D000000, uRegValue&0x0);
	utest++;
	}

	}	
}




void SblockT_AgingFunction_New(void)
{
	u32 i,j, uNumMode, uRegValue;
	oFunctionT_AutoVar oAutoTest;	
	u32 aCaseOfAES[] = { ECB, CBC, CTR};  
	u32 aCaseOfDES[] = { ECB, CBC}; 
	u32 aCaseOfHASH[] = {  HMAC, SHA1}; 
	u32 utest;
	u32 uAES_err, uDES_err, uHASH_err;
	u32 uNumClock;
	//u32 uTestSel, uTime;
	u32 aCaseOfClock[][7] = { //Sync Mode
							// OP Mode,  APLL, MPLL, ARM_Ratio,  HCLKx2_Ratio, PCLK_Ratio, Secure Clock Ratio
							
							{eASYNC_MODE, (APLL_eOUT)eAPLL532M, (APLL_eOUT)eAPLL266M, 1, 1, 4, 4},
							{eASYNC_MODE, (APLL_eOUT)eAPLL532M, (APLL_eOUT)eAPLL266M, 1, 1, 4, 2},							
							{eASYNC_MODE, (APLL_eOUT)eAPLL532M, (APLL_eOUT)eAPLL266M, 1, 1, 6, 4},
							{eASYNC_MODE, (APLL_eOUT)eAPLL532M, (APLL_eOUT)eAPLL266M, 1, 1, 6, 2},
							{eASYNC_MODE, (APLL_eOUT)eAPLL532M, (APLL_eOUT)eAPLL266M, 1, 1, 8, 4},
							{eASYNC_MODE, (APLL_eOUT)eAPLL532M, (APLL_eOUT)eAPLL266M, 1, 1, 8, 2},

							{eASYNC_MODE, (APLL_eOUT)eAPLL532M, (APLL_eOUT)eAPLL266M, 2, 2, 2, 4},	//266/66/66/33(SecureCLK)
							{eASYNC_MODE, (APLL_eOUT)eAPLL532M, (APLL_eOUT)eAPLL266M, 2, 2, 2, 2},							
							{eASYNC_MODE, (APLL_eOUT)eAPLL532M, (APLL_eOUT)eAPLL266M, 2, 2, 4, 4},
							{eASYNC_MODE, (APLL_eOUT)eAPLL532M, (APLL_eOUT)eAPLL266M, 2, 2, 4, 2},
							{eASYNC_MODE, (APLL_eOUT)eAPLL532M, (APLL_eOUT)eAPLL266M, 2, 2, 6, 4},
							{eASYNC_MODE, (APLL_eOUT)eAPLL532M, (APLL_eOUT)eAPLL266M, 2, 2, 6, 2},
							
							{eASYNC_MODE, (APLL_eOUT)eAPLL400M, (APLL_eOUT)eAPLL266M, 1, 1, 4, 4},
							{eASYNC_MODE, (APLL_eOUT)eAPLL400M, (APLL_eOUT)eAPLL266M, 1, 1, 4, 2},							
							{eASYNC_MODE, (APLL_eOUT)eAPLL400M, (APLL_eOUT)eAPLL266M, 1, 1, 6, 4},
							{eASYNC_MODE, (APLL_eOUT)eAPLL400M, (APLL_eOUT)eAPLL266M, 1, 1, 6, 2},
							{eASYNC_MODE, (APLL_eOUT)eAPLL400M, (APLL_eOUT)eAPLL266M, 1, 1, 8, 4},
							{eASYNC_MODE, (APLL_eOUT)eAPLL400M, (APLL_eOUT)eAPLL266M, 1, 1, 8, 2},

							{eASYNC_MODE, (APLL_eOUT)eAPLL532M, (APLL_eOUT)eAPLL200M, 1, 1, 4, 4},
							{eASYNC_MODE, (APLL_eOUT)eAPLL532M, (APLL_eOUT)eAPLL200M, 1, 1, 4, 2},							
							{eASYNC_MODE, (APLL_eOUT)eAPLL532M, (APLL_eOUT)eAPLL200M, 1, 1, 6, 4},
							{eASYNC_MODE, (APLL_eOUT)eAPLL532M, (APLL_eOUT)eAPLL200M, 1, 1, 6, 2},
							{eASYNC_MODE, (APLL_eOUT)eAPLL532M, (APLL_eOUT)eAPLL200M, 1, 1, 8, 4},
							{eASYNC_MODE, (APLL_eOUT)eAPLL532M, (APLL_eOUT)eAPLL200M, 1, 1, 8, 2},


							{eASYNC_MODE, (APLL_eOUT)eAPLL400M, (APLL_eOUT)eAPLL200M, 1, 1, 4, 4},
							{eASYNC_MODE, (APLL_eOUT)eAPLL400M, (APLL_eOUT)eAPLL200M, 1, 1, 4, 2},							
							{eASYNC_MODE, (APLL_eOUT)eAPLL400M, (APLL_eOUT)eAPLL200M, 1, 1, 6, 4},
							{eASYNC_MODE, (APLL_eOUT)eAPLL400M, (APLL_eOUT)eAPLL200M, 1, 1, 6, 2},
							{eASYNC_MODE, (APLL_eOUT)eAPLL400M, (APLL_eOUT)eAPLL200M, 1, 1, 8, 4},
							{eASYNC_MODE, (APLL_eOUT)eAPLL400M, (APLL_eOUT)eAPLL200M, 1, 1, 8, 2},

							{eASYNC_MODE, (APLL_eOUT)eAPLL667M, (APLL_eOUT)eAPLL266M, 1, 1, 4, 4},
							{eASYNC_MODE, (APLL_eOUT)eAPLL667M, (APLL_eOUT)eAPLL266M, 1, 1, 4, 2},							
							{eASYNC_MODE, (APLL_eOUT)eAPLL667M, (APLL_eOUT)eAPLL266M, 1, 1, 6, 4},
							{eASYNC_MODE, (APLL_eOUT)eAPLL667M, (APLL_eOUT)eAPLL266M, 1, 1, 6, 2},
							{eASYNC_MODE, (APLL_eOUT)eAPLL667M, (APLL_eOUT)eAPLL266M, 1, 1, 8, 4},
							{eASYNC_MODE, (APLL_eOUT)eAPLL667M, (APLL_eOUT)eAPLL266M, 1, 1, 8, 2},

							{eASYNC_MODE, (APLL_eOUT)eAPLL532M, (APLL_eOUT)eAPLL266M, 1, 1, 4, 4},

							// Sync Mode Test
							{eSYNC_MODE, (APLL_eOUT)eAPLL532M, (APLL_eOUT)eAPLL266M, 1, 2, 4, 4},
							{eSYNC_MODE, (APLL_eOUT)eAPLL532M, (APLL_eOUT)eAPLL266M, 1, 2, 4, 2},							
							{eSYNC_MODE, (APLL_eOUT)eAPLL532M, (APLL_eOUT)eAPLL266M, 1, 2, 6, 4},
							{eSYNC_MODE, (APLL_eOUT)eAPLL532M, (APLL_eOUT)eAPLL266M, 1, 2, 6, 2},
							{eSYNC_MODE, (APLL_eOUT)eAPLL532M, (APLL_eOUT)eAPLL266M, 1, 2, 8, 4},
							{eSYNC_MODE, (APLL_eOUT)eAPLL532M, (APLL_eOUT)eAPLL266M, 1, 2, 8, 2},

							{eSYNC_MODE, (APLL_eOUT)eAPLL532M, (APLL_eOUT)eAPLL266M, 2, 2, 2, 4},	
							{eSYNC_MODE, (APLL_eOUT)eAPLL532M, (APLL_eOUT)eAPLL266M, 2, 2, 2, 2},							
							{eSYNC_MODE, (APLL_eOUT)eAPLL532M, (APLL_eOUT)eAPLL266M, 2, 2, 4, 4},
							{eSYNC_MODE, (APLL_eOUT)eAPLL532M, (APLL_eOUT)eAPLL266M, 2, 2, 4, 2},
							{eSYNC_MODE, (APLL_eOUT)eAPLL532M, (APLL_eOUT)eAPLL266M, 2, 2, 6, 4},
							{eSYNC_MODE, (APLL_eOUT)eAPLL532M, (APLL_eOUT)eAPLL266M, 2, 2, 6, 2},

							{eSYNC_MODE, (APLL_eOUT)eAPLL400M, (APLL_eOUT)eAPLL266M, 1, 2, 4, 4},
							{eSYNC_MODE, (APLL_eOUT)eAPLL400M, (APLL_eOUT)eAPLL266M, 1, 2, 4, 2},							
							{eSYNC_MODE, (APLL_eOUT)eAPLL400M, (APLL_eOUT)eAPLL266M, 1, 2, 6, 4},
							{eSYNC_MODE, (APLL_eOUT)eAPLL400M, (APLL_eOUT)eAPLL266M, 1, 2, 6, 2},
							{eSYNC_MODE, (APLL_eOUT)eAPLL400M, (APLL_eOUT)eAPLL266M, 1, 2, 8, 4},
							{eSYNC_MODE, (APLL_eOUT)eAPLL400M, (APLL_eOUT)eAPLL266M, 1, 2, 8, 2},

							{eSYNC_MODE, (APLL_eOUT)eAPLL532M, (APLL_eOUT)eAPLL200M, 1, 2, 4, 4},
							{eSYNC_MODE, (APLL_eOUT)eAPLL532M, (APLL_eOUT)eAPLL200M, 1, 2, 4, 2},							
							{eSYNC_MODE, (APLL_eOUT)eAPLL532M, (APLL_eOUT)eAPLL200M, 1, 2, 6, 4},
							{eSYNC_MODE, (APLL_eOUT)eAPLL532M, (APLL_eOUT)eAPLL200M, 1, 2, 6, 2},
							{eSYNC_MODE, (APLL_eOUT)eAPLL532M, (APLL_eOUT)eAPLL200M, 1, 2, 8, 4},
							{eSYNC_MODE, (APLL_eOUT)eAPLL532M, (APLL_eOUT)eAPLL200M, 1, 2, 8, 2},

							//Additional  SCLK Divider
							{eASYNC_MODE, (APLL_eOUT)eAPLL532M, (APLL_eOUT)eAPLL266M, 1, 1, 4, 1},
							{eASYNC_MODE, (APLL_eOUT)eAPLL532M, (APLL_eOUT)eAPLL266M, 1, 1, 4, 2},							
							{eASYNC_MODE, (APLL_eOUT)eAPLL532M, (APLL_eOUT)eAPLL266M, 1, 1, 4, 3},
							{eASYNC_MODE, (APLL_eOUT)eAPLL532M, (APLL_eOUT)eAPLL266M, 1, 1, 4, 4},

							{eASYNC_MODE, (APLL_eOUT)eAPLL532M, (APLL_eOUT)eAPLL266M, 1, 2, 4, 1},
							{eASYNC_MODE, (APLL_eOUT)eAPLL532M, (APLL_eOUT)eAPLL266M, 1, 2, 4, 2},							
							{eASYNC_MODE, (APLL_eOUT)eAPLL532M, (APLL_eOUT)eAPLL266M, 1, 2, 4, 3},
							{eASYNC_MODE, (APLL_eOUT)eAPLL532M, (APLL_eOUT)eAPLL266M, 1, 2, 4, 4},

							{eSYNC_MODE, (APLL_eOUT)eAPLL532M, (APLL_eOUT)eAPLL266M, 1, 2, 4, 1},
							{eSYNC_MODE, (APLL_eOUT)eAPLL532M, (APLL_eOUT)eAPLL266M, 1, 2, 4, 2},							
							{eSYNC_MODE, (APLL_eOUT)eAPLL532M, (APLL_eOUT)eAPLL266M, 1, 2, 4, 3},
							{eSYNC_MODE, (APLL_eOUT)eAPLL532M, (APLL_eOUT)eAPLL266M, 1, 2, 4, 4},

							{eSYNC_MODE, (APLL_eOUT)eAPLL532M, (APLL_eOUT)eAPLL266M, 1, 3, 4, 1},
							{eSYNC_MODE, (APLL_eOUT)eAPLL532M, (APLL_eOUT)eAPLL266M, 1, 3, 4, 2},							
							{eSYNC_MODE, (APLL_eOUT)eAPLL532M, (APLL_eOUT)eAPLL266M, 1, 3, 4, 3},
							{eSYNC_MODE, (APLL_eOUT)eAPLL532M, (APLL_eOUT)eAPLL266M, 1, 3, 4, 4},

							{eSYNC_MODE, (APLL_eOUT)eAPLL532M, (APLL_eOUT)eAPLL266M, 1, 2, 4, 1}
							
							};


	uNumClock = sizeof(aCaseOfClock)/sizeof(aCaseOfClock[0]);

	//UART_Printf("Add Delay : \n");
	//uTestSel=UART_GetIntNum();
	

for(i=0 ; i<uNumClock ; i++)
{

		if(aCaseOfClock[i][0] == eSYNC_MODE)
		{
			SYSC_ChangeMode(eSYNC_MODE);
			SYSC_ChangeSYSCLK_1((APLL_eOUT)aCaseOfClock[i][1], (APLL_eOUT)aCaseOfClock[i][2], 0, 1, 7);

			//void SYSC_SetDIV0( u32 uAratio, u32 uMratio, u32 uHratio, u32 uHx2ratio, u32 uPratio, u32 uONDratio, u32 uSECUratio, u32 uCAMratio, u32 uJPEGratio, u32 uMFCratio )
			SYSC_SetDIV0(aCaseOfClock[i][3]-1, 1, 1, aCaseOfClock[i][4]-1,  aCaseOfClock[i][5]-1, 1, aCaseOfClock[i][6]-1,0, 1, 0);

			SYSC_GetClkInform();
			OpenConsole();
			UART_InitDebugCh(0, 115200);
			Delay(10);
			
			UART_Printf("\n\n");
			UART_Printf("Sync Mode : Synchronous\n");
			UART_Printf("ARMCLK: %.2fMHz  HCLKx2: %.2fMHz  HCLK: %.2fMHz  PCLK: %.2fMHz  Secure_CLK: %.2fMHz\n",(float)g_ARMCLK/1.0e6, (float)g_HCLKx2/1.0e6, (float)g_HCLK/1.0e6, (float)g_PCLK/1.0e6, (float)g_HCLKx2/(1.0e6 * aCaseOfClock[i][6]));
			UART_Printf("SYNC Mode : %d\n", g_SYNCACK);
			UART_Printf("CLK_DIV0: 0x%x\n", Inp32SYSC(0x20));
			UART_Printf("\n");
		}
		else		// Async Mode
		{
			SYSC_ChangeMode(eASYNC_MODE);
			SYSC_ChangeSYSCLK_1((APLL_eOUT)aCaseOfClock[i][1], (APLL_eOUT)aCaseOfClock[i][2], 0, 1, 7);
			SYSC_SetDIV0(aCaseOfClock[i][3]-1, 1, 1, aCaseOfClock[i][4]-1,  aCaseOfClock[i][5]-1, 1, aCaseOfClock[i][6]-1,0, 1, 0);

			SYSC_GetClkInform();
			OpenConsole();
			UART_InitDebugCh(0, 115200);
			Delay(10);
			
			UART_Printf("\n\n");
			UART_Printf("Sync Mode : ASynchronous\n");
			UART_Printf("ARMCLK: %.2fMHz  HCLKx2: %.2fMHz  HCLK: %.2fMHz  PCLK: %.2fMHz  Secure_CLK: %.2fMHz\n",(float)g_ARMCLK/1.0e6, (float)g_HCLKx2/1.0e6, (float)g_HCLK/1.0e6, (float)g_PCLK/1.0e6, (float)g_HCLKx2/(1.0e6*aCaseOfClock[i][6]));
			UART_Printf("SYNC Mode : %d\n", g_SYNCACK);
			UART_Printf("CLK_DIV0: 0x%x\n", Inp32SYSC(0x20));
			UART_Printf("\n");

		}

	
//	uRegValue= Inp32SYSC(0x20);
//	uRegValue = uRegValue&~(3<<18)|(3<<18);	// SECURE_RATIO = 3
//	Outp32SYSC(0x20, uRegValue);


	
	UART_Printf("\n [ Sblock Full Function Test ]\n");
	UART_Printf("============================================= \n");


	
	utest=0;
	uAES_err=0;
	uDES_err=0;
	uHASH_err=0;

	SYSC_CtrlHCLKGate(eHCLK_SECUR, 0);	// Clock Gating
	//SYSC_SetNORCFG( u32 uIROM, u32 uDM_ETM, u32 uDM_S, u32 uDM_F, u32 uDM_P, u32 uDM_I, u32 uDM_SS, u32 uDM_G, u32 uDM_V)
	SYSC_SetNORCFG( 1, 1, 0, 1, 1, 1, 1, 1, 1);
   	
	while(utest<10000)
	{
	 if(UART_GetKey()!=0x0) break;
	 if(utest%100 == 0)  UART_Printf("test no(AES_err)(DES_err)(HASH_err) : %d(%d)(%d)(%d) \n", utest,uAES_err, uDES_err,uHASH_err);
	//////////////////////////////////////////
	//////   AES Test

	Delay(10);

	SYSC_CtrlHCLKGate(eHCLK_SECUR, 1);	// Clock Gating
	//StartTimer(0);
	SYSC_SetNORCFG( 1, 1, 1, 1, 1, 1, 1, 1, 1);
	
	while(!((Inp32(0x7E00F90C)>>5)&0x1 == 1));	// Sblock Power Enable
	//SYSC_CtrlHCLKGate(eHCLK_SECUR, 1);
	//Delay(10); 								// Check Point!!! 08.03.04  Delay10 OK
	//Delay(uTestSel);								// Aging Fail
	//uTime=StopTimer(0);
	//if(utest==0)UART_Printf("ON to Start Time %d \n", uTime);	
	
	g_sblock_err = 0;
	uNumMode = sizeof(aCaseOfAES)/sizeof(aCaseOfAES[0]);
	for(j=0; j<uNumMode;j++)
	{
		oAutoTest.Test_Parameter[0] = aCaseOfAES[j];
	 	Test_AES_CPU(eTest_Auto, oAutoTest );
	 	Test_AES_FIFO(eTest_Auto, oAutoTest );
	 	Test_AES_DMA(eTest_Auto, oAutoTest );
	}
	
	for(j=0; j<3;j++)
	{
		oAutoTest.Test_Parameter[0] = j;
		Test_CBC_IV_FIFO(eTest_Auto, oAutoTest );
		Test_CTR_Counter(eTest_Auto, oAutoTest );
		Test_CTR_ICTR_FIFO(eTest_Auto, oAutoTest );

	}
		

	
	if(g_sblock_err == 0)
	{
		//UART_Printf("	AES Operation :			PASS \n");
		
	}
	else
	{
		//UART_Printf("	AES Operation :			FAIL\n");
		uAES_err++;
	}


	//////////////////////////////////////////
	//////   DES Test
	g_sblock_err = 0;
	uNumMode = sizeof(aCaseOfDES)/sizeof(aCaseOfDES[0]);
	for(j=0; j<uNumMode;j++)
	{
		oAutoTest.Test_Parameter[0] = aCaseOfDES[j];
	 	Test_DES_CPU(eTest_Auto, oAutoTest );
	 	Test_DES_FIFO(eTest_Auto, oAutoTest );
	 	Test_DES_DMA(eTest_Auto, oAutoTest );
	}

	if(g_sblock_err == 0)
	{
		//UART_Printf("	DES Operation :			PASS \n");
	}
	else
	{
		//UART_Printf("	DES Operation :			FAIL\n");
		uDES_err++;
	}

	//////////////////////////////////////////
	//////   HASH Test
	g_sblock_err = 0;
	uNumMode = sizeof(aCaseOfHASH)/sizeof(aCaseOfHASH[0]);
	for(j=0; j<uNumMode;j++)
	{
		oAutoTest.Test_Parameter[0] = aCaseOfHASH[j];
	 	Test_HASH_CPU(eTest_Auto, oAutoTest );
	 	Test_HASH_FIFO(eTest_Auto, oAutoTest );
	 	Test_HASH_DMA(eTest_Auto, oAutoTest );
	}
		Test_HASH_CPU_Added(eTest_Auto, oAutoTest );
		Test_SHA1_INTERMEDIATE(eTest_Auto, oAutoTest );
	
	if(g_sblock_err == 0)
	{
		//UART_Printf("	HASH Operation :			PASS \n");
	}
	else
	{
		//UART_Printf("	HASH Operation :			FAIL\n");
		uHASH_err++;
	}


	// INTC Clear
	uRegValue = Inp32(0x7D000000);
	Outp32(0x7D000000, uRegValue&0x0);
	utest++;

	SYSC_CtrlHCLKGate(eHCLK_SECUR, 0);	// Clock Gating
	//SYSC_SetNORCFG( u32 uIROM, u32 uDM_ETM, u32 uDM_S, u32 uDM_F, u32 uDM_P, u32 uDM_I, u32 uDM_SS, u32 uDM_G, u32 uDM_V)
	SYSC_SetNORCFG( 1, 1, 0, 1, 1, 1, 1, 1, 1);
	
	}

	
}
}


///// 
///////////////////////////////////////////////////////////////////////////////
// Temp....ing
void __irq Isr_SDMA2(void)
{
	//u32 uRead;
	//for Test
	//uRead = Inp32(0x7DC00000);	//SDMA1 INT STATUS
/*
	uRead = (uRead>>6)&0x3;
	if(uRead== 1)
	{
		printf("		SDMA1 Rx ISR Occurred\n");
	}
	else if(uRead== 2)
	{
		printf("		SDMA1 Tx ISR Occurred\n");
	}
	else
	{
		printf("		SDMA1 Rx/Tx ISR Occurred\n");
	}
*/
	SBLK_ClearIntPending(0);
	//Delay(1);
	//printf("		SDMA1 Int 0x%x \n", uRead);
	//printf("		SDMA1 ISR Occurred\n");
	//g_SDma1Done = 1;
	g_SDma1Done ++;

#if 0
	SBLK_SetFifo(16, 4, 16, 4, LASTBYTE_1ST);
	SBLK_SetSDMA((INPUT_BUF+g_SDma1Done*16*4), 16, (OUTPUT_BUF+g_SDma1Done*16*4), 16);
	SBLK_StartFifo();
	SBLK_StartSDMA();
#endif
	

	INTC_ClearVectAddr();
}

void __irq Isr_SDMA_b(void)
{
	//u32 uRead;
	//for Test
	//uRead = Inp32(0x7DC00000);	//SDMA1 INT STATUS
/*
	uRead = (uRead>>6)&0x3;
	if(uRead== 1)
	{
		printf("		SDMA1 Rx ISR Occurred\n");
	}
	else if(uRead== 2)
	{
		printf("		SDMA1 Tx ISR Occurred\n");
	}
	else
	{
		printf("		SDMA1 Rx/Tx ISR Occurred\n");
	}
*/
	SBLK_ClearIntPending(0);
	//Delay(1);
	//printf("		SDMA1 Int 0x%x \n", uRead);
	//printf("		SDMA1 ISR Occurred\n");
	//g_SDma1Done = 1;
	g_SDma1Done ++;

#if 1
	SBLK_SetFifo(16, 4, 16, 4, LASTBYTE_1ST);
	SBLK_SetSDMA((INPUT_BUF+g_SDma1Done*16*4), 16, (OUTPUT_BUF+g_SDma1Done*16*4), 16);
	SBLK_StartFifo();
	SBLK_StartSDMA();
#endif
	

	INTC_ClearVectAddr();
}


void __irq Isr_SDMA_a(void)
{
	//u32 uRead;
	//for Test
	//uRead = Inp32(0x7DC00000);	//SDMA1 INT STATUS
/*
	uRead = (uRead>>6)&0x3;
	if(uRead== 1)
	{
		printf("		SDMA1 Rx ISR Occurred\n");
	}
	else if(uRead== 2)
	{
		printf("		SDMA1 Tx ISR Occurred\n");
	}
	else
	{
		printf("		SDMA1 Rx/Tx ISR Occurred\n");
	}
*/
	SBLK_ClearIntPending(0);
	//Delay(1);
	//printf("		SDMA1 Int 0x%x \n", uRead);
	//printf("		SDMA1 ISR Occurred\n");
	//g_SDma1Done = 1;
	g_SDma1Done ++;

#if 1
	SBLK_SetFifo(16, 2, 16, 2, LASTBYTE_1ST);
	SBLK_SetSDMA((INPUT_BUF+g_SDma1Done*16*4), 16, (OUTPUT_BUF+g_SDma1Done*16*4), 16);
	SBLK_StartFifo();
	SBLK_StartSDMA();
#endif
	

	INTC_ClearVectAddr();
}

//////////
// Function Name : Test_AES_DMA
// Function Description : AES DMA mode Test
// Input : 	None
// Output :	None 
// Version : v0.1
void Test_SBLK_Perform1(void)
{
	u32  usel;
	u32 i;
	u32 uTestTime;
	u32 uRatio, uRegValue;

	// SBLK Divider
	UART_Printf("Select Secure Ratio:  1, or 3\n");
	uRatio = UART_GetIntNum();

	uRegValue= Inp32SYSC(0x20);
	uRegValue = uRegValue&~(3<<18)|(uRatio<<18);
	Outp32SYSC(0x20, uRegValue);

	UART_Printf("rCLK_DIV0: 0x%x\n", Inp32SYSC(0x20));

#if 0
	UART_Printf("Select Operating Mode: 0: Sync,   1: Async , 2 Async 667/133\n");
	uMode = UART_GetIntNum();

	switch(uMode)
		{
			case 0:		// Sync Mode
			SYSC_ChangeSYSCLK_1(eAPLL532M, eAPLL266M, 0, 1, 3);
			SYSC_ChangeMode(eSYNC_MODE);
			SYSC_ChangeSYSCLK_1(eAPLL532M, eAPLL266M, 0, 1, 3);
			
			break;

			case 1:		//ASync Mode
			//SYSC_ChangeMode(eASYNC_MODE);
			SYSC_ChangeSYSCLK_1(eAPLL532M, eAPLL266M, 0, 0, 3);
			//SYSC_ChangeSYSCLK_1(eAPLL667M, eAPLL266M, 0, 0, 3);
			break;

			case 2:		//ASync Mode
			//SYSC_ChangeMode(eASYNC_MODE);
			//SYSC_ChangeSYSCLK_1(eAPLL532M, eAPLL266M, 0, 0, 3);
			SYSC_ChangeSYSCLK_1(eAPLL667M, eAPLL266M, 0, 0, 3);
			break;
			
		}
#endif
	
		UART_Printf("[SBLOCK - AES DMA_FIFO Mode Test]\n");

		UART_Printf("Select AES Operating Mode  : 1)ECB   2)CBC   3)CTR \n");
		usel=UART_GetIntNum();
		if (usel == 1) eAesOperMode = ECB;
		else if (usel == 2) eAesOperMode = CBC;
		else if (usel == 3) eAesOperMode = CTR;

		UART_Printf("Your selection is ");
		DispOperMode(eAesOperMode);
		UART_Printf("\n");

	
	INTC_SetVectAddr(NUM_SDMA1, Isr_SDMA_a); //Rx Operation 
	INTC_Enable(NUM_SDMA1);

	//Plain Text  1MB
	for (i =0 ; i<16384; i++)	   // 16384
	{
		Copy((u32)uAesPlainText, (INPUT_BUF+i*4*16), 16);
	}

	//Golden Data 1MB
	for (i =0 ; i<16384; i++)
	{
		Copy((u32)uAesCipherText[eAesOperMode], (GOLDEN_BUF+i*4*16), 16);
	}

	
	//////////////////////////
	////// Encryption
	g_SDma1Done = 0;
	StartTimer(0);
	
	SBLK_Init(AES, eAesOperMode, ENC);


	//for (i =0 ; i<16384; i++)
	//{
	SBLK_SetFifo(16, 4, 16, 4, LASTBYTE_1ST);
	SBLK_SetSDMA(INPUT_BUF, 16, OUTPUT_BUF, 16);

	SBLK_StartFifo();
	SBLK_StartSDMA();
	//}

	
	while(g_SDma1Done<16384);
	uTestTime=StopTimer(0);
	INTC_Disable(NUM_SDMA1);
	SBLK_ClearIntPending(0);
	UART_Printf("uTestTime = %d\n", uTestTime);

	//UART_Getc();
	#if 0
	//uRet = Compare(OUTPUT_BUF, GOLDEN_BUF, 262144);
	//uRet = Compare(OUTPUT_BUF, GOLDEN_BUF, 16);
		if (uRet)
		{
		UART_Printf("AES_");
		DispOperMode(eAesOperMode);
		UART_Printf("_DMA_FIFO_ENC Success..\n");
		}
		else
		{
		UART_Printf("AES_");
		DispOperMode(eAesOperMode);
		UART_Printf("_DMA_FIFO_ENC Failed..\n");
		}
	#endif

	
	
}

//////////
// Function Name : Test_AES_DMA
// Function Description : AES DMA mode Test
// Input : 	None
// Output :	None 
// Version : v0.1
void Test_SBLK_Perform2(void)
{
	u32  usel;
	u32 i;
	u32 uTestTime;
	u32 uRatio, uRegValue;

	// SBLK Divider
	UART_Printf("Select Secure Ratio:  1, or 3\n");
	uRatio = UART_GetIntNum();

	uRegValue= Inp32SYSC(0x20);
	uRegValue = uRegValue&~(3<<18)|(uRatio<<18);
	Outp32SYSC(0x20, uRegValue);

	UART_Printf("rCLK_DIV0: 0x%x\n", Inp32SYSC(0x20));

	#if 0
	UART_Printf("Select Operating Mode: 0: Sync,   1: Async , 2 Async 667/133\n");
	uMode = UART_GetIntNum();

	switch(uMode)
		{
			case 0:		// Sync Mode
			SYSC_ChangeSYSCLK_1(eAPLL532M, eAPLL266M, 0, 1, 3);
			SYSC_ChangeMode(eSYNC_MODE);
			SYSC_ChangeSYSCLK_1(eAPLL532M, eAPLL266M, 0, 1, 3);
			
			break;

			case 1:		//ASync Mode
			//SYSC_ChangeMode(eASYNC_MODE);
			SYSC_ChangeSYSCLK_1(eAPLL532M, eAPLL266M, 0, 0, 3);
			//SYSC_ChangeSYSCLK_1(eAPLL667M, eAPLL266M, 0, 0, 3);
			break;

			case 2:		//ASync Mode
			//SYSC_ChangeMode(eASYNC_MODE);
			//SYSC_ChangeSYSCLK_1(eAPLL532M, eAPLL266M, 0, 0, 3);
			SYSC_ChangeSYSCLK_1(eAPLL667M, eAPLL266M, 0, 0, 3);
			break;
			
		}
	 #endif
	
		UART_Printf("[SBLOCK - DES_DMA Mode Test]\n");

		UART_Printf("Select DES Operating Mode  : 1)ECB   2)CBC    \n");
		usel=UART_GetIntNum();
		if (usel == 1) eDesOperMode = ECB;
		else if (usel == 2) eDesOperMode = CBC;

		UART_Printf("Your selection is ");
		DispOperMode(eDesOperMode);
		UART_Printf("\n");

	
	INTC_SetVectAddr(NUM_SDMA1, Isr_SDMA_b); //Rx Operation 
	INTC_Enable(NUM_SDMA1);

	//Plain Text  1MB
	for (i =0 ; i<16384; i++)	   // 16384
	{

		Copy((u32)uDesPlainText, (INPUT_BUF+i*4*16), 16);

	}

	//Golden Data 1MB
	for (i =0 ; i<16384; i++)
	{
			Copy((u32)uDesCipherText[eDesOperMode], (GOLDEN_BUF+i*4*16), 16);
	}

	
	//////////////////////////
	////// Encryption
	g_SDma1Done = 0;
	StartTimer(0);
	
	SBLK_Init(DES, eAesOperMode, ENC);


	//for (i =0 ; i<16384; i++)
	//{
	SBLK_SetFifo(16, 2, 16, 2, LASTBYTE_1ST);
	SBLK_SetSDMA(INPUT_BUF, 16, OUTPUT_BUF, 16);

	SBLK_StartFifo();
	SBLK_StartSDMA();
	//}

	
	while(g_SDma1Done<16384);
	uTestTime=StopTimer(0);

	UART_Printf("uTestTime = %d\n", uTestTime);
	INTC_Disable(NUM_SDMA1);
	SBLK_ClearIntPending(0);

	//UART_Getc();
	#if 0
	//uRet = Compare(OUTPUT_BUF, GOLDEN_BUF, 262144);
	//uRet = Compare(OUTPUT_BUF, GOLDEN_BUF, 16);
		if (uRet)
		{
		UART_Printf("AES_");
		DispOperMode(eAesOperMode);
		UART_Printf("_DMA_FIFO_ENC Success..\n");
		}
		else
		{
		UART_Printf("AES_");
		DispOperMode(eAesOperMode);
		UART_Printf("_DMA_FIFO_ENC Failed..\n");
		}
	#endif

	
	
}



// Temp....ing
//////////
// Function Name : Test_AES_DMA
// Function Description : AES DMA mode Test
// Input : 	None
// Output :	None 
// Version : v0.1
void Test_SBLK_Perform3(void)
{
	//u32 uRet, usel;
	u32 i;
	u32 uTestTime;
	u32 uRatio, uRegValue;

	// SBLK Divider
	UART_Printf("Select Secure Ratio:  1, or 3\n");
	uRatio = UART_GetIntNum();

	uRegValue= Inp32SYSC(0x20);
	uRegValue = uRegValue&~(3<<18)|(uRatio<<18);
	Outp32SYSC(0x20, uRegValue);

	UART_Printf("rCLK_DIV0: 0x%x\n", Inp32SYSC(0x20));

	#if 0
	UART_Printf("Select Operating Mode: 0: Sync,   1: Async , 2 Async 667/133\n");
	uMode = UART_GetIntNum();

	switch(uMode)
		{
			case 0:		// Sync Mode
			SYSC_ChangeSYSCLK_1(eAPLL532M, eAPLL266M, 0, 1, 3);
			SYSC_ChangeMode(eSYNC_MODE);
			SYSC_ChangeSYSCLK_1(eAPLL532M, eAPLL266M, 0, 1, 3);
			
			break;

			case 1:		//ASync Mode
			//SYSC_ChangeMode(eASYNC_MODE);
			SYSC_ChangeSYSCLK_1(eAPLL532M, eAPLL266M, 0, 0, 3);
			//SYSC_ChangeSYSCLK_1(eAPLL667M, eAPLL266M, 0, 0, 3);
			break;

			case 2:		//ASync Mode
			//SYSC_ChangeMode(eASYNC_MODE);
			//SYSC_ChangeSYSCLK_1(eAPLL532M, eAPLL266M, 0, 0, 3);
			SYSC_ChangeSYSCLK_1(eAPLL667M, eAPLL266M, 0, 0, 3);
			break;
			
		}

	#endif
	
	 eHashOperMode = SHA1;
	//eHashOperMode = HMAC;

	INTC_SetVectAddr(NUM_SDMA1, Isr_SDMA2); //Rx Operation 
	INTC_Enable(NUM_SDMA1);

	//Plain Text  1MB
	for (i =0 ; i<16384; i++)
	{
		Copy((u32)uSha1PlainText16, (INPUT_BUF+i*4*16), 16);
	}
	g_SDma1Done= 0;
	
	StartTimer(0);
	SBLK_Init(HASH, eHashOperMode, ENC);
	
	SBLK_SetFifo(262144, 16, 5, 16, LASTBYTE_4TH);
	SBLK_SetSDMA(INPUT_BUF, 262144, OUTPUT_BUF, 5);
	
	SBLK_StartFifo();
	SBLK_StartSDMA();

	while(!g_SDma1Done);
	
	uTestTime=StopTimer(0);
	INTC_Disable(NUM_SDMA1);
	SBLK_ClearIntPending(0);
	UART_Printf("SHA-1 uTestTime = %d \n", uTestTime);
	Copy((u32)uSha1CipherText_1M, GOLDEN_BUF, 5);
	//uRet = Compare(OUTPUT_BUF, GOLDEN_BUF, 5);

	// HMAC Operation
	eHashOperMode = HMAC;

	g_SDma1Done =0;
	INTC_Enable(NUM_SDMA1);

	StartTimer(0);
	SBLK_Init(HASH, eHashOperMode, ENC);
	SBLK_SetFifo(262144, 16, 5, 16, LASTBYTE_4TH);
	SBLK_SetSDMA(INPUT_BUF, 262144, OUTPUT_BUF, 5);
	
	SBLK_StartFifo();
	SBLK_StartSDMA();

	while(!g_SDma1Done);
	
	uTestTime=StopTimer(0);
	INTC_Disable(NUM_SDMA1);
	SBLK_ClearIntPending(0);
	UART_Printf("HMAC  uTestTime = %d \n", uTestTime);
	Copy((u32)uhmacCipherText_1M, GOLDEN_BUF, 5);
	//uRet = Compare(OUTPUT_BUF, GOLDEN_BUF, 5);
	
}




///////////////////////////////////////////////////////////////////////////////////
////////////////////                   Secure Block Main Test           /////////////////////////// 
///////////////////////////////////////////////////////////////////////////////////

const testFuncMenu secu_menu[] =
{
		SblockT_AES,			"AES Mode Test",
		SblockT_DES,			"DES Mode Test",
		SblockT_HASH,			"HASH Mode Test",
	
#if 0
		Test_AES_Temp,			"Test AES - Additional test_Temp",
		Test_DES_Temp,			"Test DES - Additional test_Temp",
		Temp,					"Temp Test...",
		Temp3,					"Temp Test..FIFO",

		SetAesOperMode,		"Set AES Operation Mode",
		SetDesOperMode,		"Set DES Operation Mode",
		SetHashOperMode,		"Set HASH Operation Mode",
#endif		

		SblockT_FullFunction,			"Sblock Full Test",
		Sblock_AES_autotest,			"	AES_Full_Function",
		Sblock_DES_autotest,			"	DES_Full_Function",
		Sblock_HASH_autotest,			"	HASH_Full_Function",
		SblockT_AgingFunction,			"Aging Full Test",
		SblockT_AgingFunction_New,			"Aging Full Test_New",
		Sblock_ClockChange,		"Clock Change",

		Test_SBLK_Perform1,       "AES-Perform",
		Test_SBLK_Perform2,       "DES-Perform",
		Test_SBLK_Perform3,       "HASH-Perform",
		
	0, 0
};

void SBLOCK_Test(void)
{
	u32 i;
	s32 uSel;

	UART_Printf("[Security Sub System Test]\n\n");

	eAesOperMode = ECB;
	eDesOperMode = ECB;
	eHashOperMode = HMAC;

	//Add Stop Mode Enter/Exit
	//SYSC_STOP();

	
	while(1)
	{
		for (i=0; (u32)(secu_menu[i].desc)!=0; i++)
			UART_Printf("%2d: %s\n", i, secu_menu[i].desc);

		UART_Printf("\nSelect the function to test : ");
		uSel =UART_GetIntNum();
		UART_Printf("\n");
		if(uSel == -1) 
			break;

		if (uSel>=0 && uSel<(sizeof(secu_menu)/8-1))
			(secu_menu[uSel].func) ();
	}
}	


