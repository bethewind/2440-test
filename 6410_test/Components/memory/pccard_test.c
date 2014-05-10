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
*	File Name : pccard_test.c
*  
*	File Description : This file implements PCCard mode test functions of CF Controller.
*
*	Author : Sunil,Roe
*	Dept. : AP Development Team
*	Created Date : 2007/1/17
*	Version : 0.1 
* 
*	History
*	- Created(Sunil,Roe 2007/1/17)
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
#include "gpio.h"
#include "intc.h"
#include "dma.h"

#include "pccard.h"
#include "cf.h"

// global variables
u8 g_ucPCCardConNum;
volatile PCCARD_oInform	g_oaPCCARDInform[PCCARD_CONNUM];

// function declaration
void PCCARD_PrintCIS(void);
void PCCARD_OpenMedia(u8, ePCCARD_MODE);
void PCCARD_CloseMedia(void);
void PCCARD_TestClear(void);
void PCCARD_TestRead(void);
u32 PCCARD_SelectMode(void);
void PCCARD_TestWriteRead(void);
void PCCARD_TestIntWriteRead(void);

const testFuncMenu g_aPCCARDTestFunc[]=
{
	PCCARD_PrintCIS,			"PC Card CIS(Card Information Structure)",
	PCCARD_TestWriteRead,		"PC Card Common memory Write/Read test", 
	PCCARD_TestRead,			"PC Card Common memory Read test",
	PCCARD_TestClear,			"PC Card memory clear data",
	PCCARD_TestIntWriteRead,		"PC Card Common memory Interrupt mode Write/Read test", 
	0,0 
};

//////////
// Function Name : PCCARD_Test
// Function Description : This function prints out PCCARD test functions through UART.
// Input : 	NONE
// Output : 	NONE
// Version : v0.1
void PCCARD_Test(void)
{
	u32 uCountFunc = 0;
	s32	iSel = 0;
	

	UART_Printf("\n\n================== PCCARD Function Test =====================\n\n");
	g_ucPCCardConNum = PCCARD_CON0; 
	
	if (!PCCARD_Init(g_ucPCCardConNum))
	{
		UART_Printf("Fail to initialize PCCARD Mode...\n");
		return;
	}

	while(1)
	{
		for (uCountFunc=0; (u32)(g_aPCCARDTestFunc[uCountFunc].desc)!=0; uCountFunc++)
			UART_Printf("%2d: %s\n", uCountFunc, g_aPCCARDTestFunc[uCountFunc].desc);

		UART_Printf("\nSelect the function to test : ");
		iSel =UART_GetIntNum();
		UART_Printf("\n");
		if(iSel == -1) 
			break;

		if (iSel>=0 && iSel<(sizeof(g_aPCCARDTestFunc)/8-1))
			(g_aPCCARDTestFunc[iSel].func) ();
	}

	PCCARD_ReturnPort(g_ucPCCardConNum);
}

/*---------------------------------- Test Functions -------------------------------*/
//////////
// Function Name : PCCARD_PrintCIS
// Function Description : 
//   This function prints out the information of CF Card.
// Input : NONE
// Output : NONE
// Version : v0.1 
void PCCARD_PrintCIS(void)
{
	u16	ucLoopCnt 	= 0;
	u8	ucSubLoopCnt 	= 0;
	u8	aucStr[16] 	= {0};
	u8	ucChar		= 0;
	u32	uCisEnd		= 0;
	
//	PCCARD_OpenMedia(g_ucPCCardConNum,ePCCARD_COMMON_MEMORY);

	UART_Printf("[Card Information Structure.]\n");

	// just test for attribute region
	for (ucLoopCnt=0;ucLoopCnt<6;ucLoopCnt++) //0x01, 04, DF, 4A, 01, FF
	{
   		ucChar = PCCARD_GetAttribData( g_ucPCCardConNum, ucLoopCnt);
   		UART_Printf(" at_addr : 0x%x, at_data : 0x%x\n", ucLoopCnt*2, ucChar);
	}
	while(1)
	{
		ucChar = PCCARD_GetAttribData( g_ucPCCardConNum, (uCisEnd));		// id
//		UART_Printf("addr:0x%x, ucChar:0x%x\n", uCisEnd*2, ucChar);
		if (ucChar == 0xff)	//0xff= termination tuple	
			break;
		uCisEnd++;
		ucChar = PCCARD_GetAttribData( g_ucPCCardConNum, (uCisEnd));		// next tuple pointer
		uCisEnd += ucChar+1;
	}
	UART_Printf("cisEnd=0~%x\n",uCisEnd);

	for (ucLoopCnt=0; ucLoopCnt<=uCisEnd; ucLoopCnt+=1)
	{
		ucChar = PCCARD_GetAttribData( g_ucPCCardConNum, ucLoopCnt);
		aucStr[(ucLoopCnt%0x10)]=ucChar;
		UART_Printf("%2x,",ucChar);
		if ((ucLoopCnt%0x10)>=0xf)
		{
			UART_Printf("//");
			for (ucSubLoopCnt=0; ucSubLoopCnt<0x10; ucSubLoopCnt++)
			{
				if (aucStr[ucSubLoopCnt]>=' ' && aucStr[ucSubLoopCnt]<=127)
					UART_Printf("%c",aucStr[ucSubLoopCnt]);
				else 
					UART_Printf(".");
			}
			UART_Printf("\n");
		}
	}
	UART_Printf("\n");	

//	PCCARD_CloseMedia();	
}

//////////
// Function Name : PCCARD_OpenMedia
// Function Description : 
//   This function initializes PCCard media.
// Input : ucCon - PCCARD Controller Number 
//			ePccardMode - MEM/PrimaryIO/SecondaryIO/ContiguousIO
// Output : NONE
// Version : v0.1 
void PCCARD_OpenMedia(u8 ucCon, ePCCARD_MODE ePccardMode)
{
	PCCARD_SetConfig(ucCon, ePCCARD_WIDTH_16BIT);	// attr mem 16bit, com mem 16bit, IO 16bit
	PCCARD_SetINTMask(ucCon, MASK);				// mask ERR, IREQ, CD
	PCCARD_SetCardAccessTiming(ucCon, 3, 0x19, 9);	// Hold, Command, Setup Timing to access attr/IO/comm area

	PCCARD_ResetCard(ucCon, RESET);					// CF Card Reset

	PCCARD_ChangePCCARDMode(ucCon, ePccardMode);
}

//////////
// Function Name : PCCARD_CloseMedia
// Function Description : 
//   This function closes PCCard media.
// Input : NONE
// Output : NONE
// Version : v0.1 
void PCCARD_CloseMedia(void)
{
	
}

//////////
// Function Name : PCCARD_CloseMedia
// Function Description : 
//   This function closes PCCard media.
// Input : NONE
// Output : NONE
// Version : v0.1 
void __irq Isr_PCCARD(void) 
{
	u32 uReg;
	u32 uReg2;

	UART_Printf("Interrupt Occured\n");
	uReg = PCCARD_GetRegValue(g_ucPCCardConNum,ePCCARD_INTMSK_SRC);
	UART_Printf("PCCARD_SRC = 0x%x \n",uReg);


	uReg &= 0xf;
	
	switch(uReg)
		{
		case 0x1 : 	
			uReg2 = PCCARD_GetRegValue(g_ucPCCardConNum,ePCCARD_CNFG_STATUS);
			uReg2 &= 0x1;
			if (uReg2 == 1)
				UART_Printf("Card not detected!! \n");
			else
				UART_Printf("Card detected!! \n");

			break;

		case 0x2 :
			UART_Printf("CF card Interrupt requested!! \n");
			break;

		case 0x4 :
			UART_Printf("Error! Host access no card in slot!! \n");			
			break;

		case 0x0 :
			UART_Printf("No error!! \n");			
			break;
							
		default:
			UART_Printf("Unknown Error occured!! \n");			
		}

	PCCARD_SetRegValue(g_ucPCCardConNum,ePCCARD_INTMSK_SRC, uReg);
		
	INTC_ClearVectAddr();
	
}


//////////
// Function Name : PCCARD_TestWriteRead
// Function Description : 
//   This function tests memory Write & Read of CF blocks.
// Input : NONE
// Output : NONE
// Version : v0.1 
void PCCARD_TestWriteRead(void)
{
	u32	uMode		= 0;
	u32 uDeviceLba	= 0;
	u32 uBlockCount	= 0;
	u32	uLoopCnt 	= 0;


	

	uMode = PCCARD_SelectMode();	
	PCCARD_OpenMedia(g_ucPCCardConNum,(ePCCARD_MODE)uMode);


	UART_Printf("\nInput device sector address\n");
	uDeviceLba = (u32)UART_GetIntNum();

	UART_Printf("Input sector count\n");
	uBlockCount = (u32)UART_GetIntNum();

	UART_Printf("Clear buffer!!\n");
	for (uLoopCnt=0; uLoopCnt<uBlockCount*512; uLoopCnt++){
		CF_SetMem(eBYTE, (u32)((u8*)g_oaPCCARDInform[g_ucPCCardConNum].puPCCARDReadBuf + uLoopCnt), 0);
		CF_SetMem(eBYTE, (u32)((u8*)g_oaPCCARDInform[g_ucPCCardConNum].puPCCARDWriteBuf + uLoopCnt), (uLoopCnt+3)%256);
	}

	UART_Printf("Press Enter \n");	
	UART_Getc();

	PCCARD_WriteBlocks(g_ucPCCardConNum, uDeviceLba, uBlockCount, (u32)(g_oaPCCARDInform[g_ucPCCardConNum].puPCCARDWriteBuf));
	PCCARD_ReadBlocks(g_ucPCCardConNum, uDeviceLba, uBlockCount, (u32)(g_oaPCCARDInform[g_ucPCCardConNum].puPCCARDReadBuf));

	if ( Compare((u32)(g_oaPCCARDInform[g_ucPCCardConNum].puPCCARDWriteBuf), (u32)(g_oaPCCARDInform[g_ucPCCardConNum].puPCCARDReadBuf), uBlockCount*128) == FALSE )
	{
		UART_Printf("Error detected.. W:0x%x, R:0x%x\n", *(u32 *)((u32)(g_oaPCCARDInform[g_ucPCCardConNum].puPCCARDWriteBuf)+512), *(u32 *)((u32)(g_oaPCCARDInform[g_ucPCCardConNum].puPCCARDReadBuf)+512));
		Dump32((u32)(g_oaPCCARDInform[g_ucPCCardConNum].puPCCARDReadBuf), uBlockCount*128);
	}
	else
	{
		UART_Printf("Write/Read in PC Card mode is OK\n");
	}

	PCCARD_CloseMedia();	
}

//////////
// Function Name : PCCARD_TestRead
// Function Description : 
//   This function tests memory clear of CF blocks.
// Input : NONE
// Output : NONE
// Version : v0.1 
void PCCARD_TestRead(void)
{
	u32	uMode		= 0;
	u32 uDeviceLba	= 0;
	u32 uBlockCount	= 0;
	u32	uLoopCnt 	= 0;

	uMode = PCCARD_SelectMode();	
	PCCARD_OpenMedia(g_ucPCCardConNum,(ePCCARD_MODE)uMode);

	UART_Printf("\nInput device sector address\n");
	uDeviceLba = (u32)UART_GetIntNum();

	UART_Printf("Input sector count\n");
	uBlockCount = (u32)UART_GetIntNum();

	UART_Printf("Clear buffer!!\n");
	for (uLoopCnt=0; uLoopCnt<uBlockCount*512/4; uLoopCnt++){
		CF_SetMem(eWORD, (u32)(g_oaPCCARDInform[g_ucPCCardConNum].puPCCARDReadBuf + uLoopCnt), 0);
	}

	UART_Printf("Press Enter \n");	
	UART_Getc();

	PCCARD_ReadBlocks(g_ucPCCardConNum, uDeviceLba, uBlockCount, (u32)(g_oaPCCARDInform[g_ucPCCardConNum].puPCCARDReadBuf));

	UART_Printf("Dump.....!!\n");
	Dump32((u32)(g_oaPCCARDInform[g_ucPCCardConNum].puPCCARDReadBuf), uBlockCount*128);

	PCCARD_CloseMedia();	
}


//////////
// Function Name : PCCARD_TestClear
// Function Description : 
//   This function tests memory clear of CF blocks.
// Input : NONE
// Output : NONE
// Version : v0.1 
void PCCARD_TestClear(void)
{
	u32	uMode		= 0;
	u32 uDeviceLba	= 0;
	u32 uBlockCount	= 0;
	u32	uLoopCnt 	= 0;

	uMode = PCCARD_SelectMode();	
	PCCARD_OpenMedia(g_ucPCCardConNum,(ePCCARD_MODE)uMode);

	UART_Printf("\nInput device sector address\n");
	uDeviceLba = (u32)UART_GetIntNum();

	UART_Printf("Input sector count\n");
	uBlockCount = (u32)UART_GetIntNum();

	UART_Printf("Clear buffer!!\n");
	for (uLoopCnt=0; uLoopCnt<uBlockCount*512/4; uLoopCnt++){
		CF_SetMem(eWORD, (u32)(g_oaPCCARDInform[g_ucPCCardConNum].puPCCARDWriteBuf + uLoopCnt), 0);
	}

	UART_Printf("Press Enter \n");	
	UART_Getc();

	PCCARD_WriteBlocks(g_ucPCCardConNum, uDeviceLba, uBlockCount, (u32)(g_oaPCCARDInform[g_ucPCCardConNum].puPCCARDWriteBuf));

	UART_Printf("Clear sectors as zero in PC Card mode.\n");

	PCCARD_CloseMedia();	
}

//////////
// Function Name : PCCARD_TestIntWriteRead
// Function Description : 
//   This function tests memory Write & Read of CF blocks with Interrupt mode
// Input : NONE
// Output : NONE
// Version : v0.1 
void PCCARD_TestIntWriteRead(void)
{
	u32	uMode		= 0;
	u32 uDeviceLba	= 0;
	u32 uBlockCount	= 0;
	u32	uLoopCnt 	= 0;

	uMode = PCCARD_SelectMode();	
	PCCARD_OpenMedia(g_ucPCCardConNum,(ePCCARD_MODE)uMode);


	PCCARD_SetINTMask(g_ucPCCardConNum, UNMASK);				// unmask ERR, IREQ, CD

	INTC_SetVectAddr( NUM_CFC, Isr_PCCARD);
	INTC_Enable( NUM_CFC);


	UART_Printf("\nInput device sector address\n");
	uDeviceLba = (u32)UART_GetIntNum();

	UART_Printf("Input sector count\n");
	uBlockCount = (u32)UART_GetIntNum();

	UART_Printf("Clear buffer!!\n");
	for (uLoopCnt=0; uLoopCnt<uBlockCount*512; uLoopCnt++){
		CF_SetMem(eBYTE, (u32)((u8*)g_oaPCCARDInform[g_ucPCCardConNum].puPCCARDReadBuf + uLoopCnt), 0);
		CF_SetMem(eBYTE, (u32)((u8*)g_oaPCCARDInform[g_ucPCCardConNum].puPCCARDWriteBuf + uLoopCnt), (uLoopCnt+3)%256);
	}

	UART_Printf("Press Enter \n");	
	UART_Getc();

	PCCARD_WriteBlocks(g_ucPCCardConNum, uDeviceLba, uBlockCount, (u32)(g_oaPCCARDInform[g_ucPCCardConNum].puPCCARDWriteBuf));
	PCCARD_ReadBlocks(g_ucPCCardConNum, uDeviceLba, uBlockCount, (u32)(g_oaPCCARDInform[g_ucPCCardConNum].puPCCARDReadBuf));

	if ( Compare((u32)(g_oaPCCARDInform[g_ucPCCardConNum].puPCCARDWriteBuf), (u32)(g_oaPCCARDInform[g_ucPCCardConNum].puPCCARDReadBuf), uBlockCount*128) == FALSE )
	{
		UART_Printf("Error detected.. W:0x%x, R:0x%x\n", *(u32 *)((u32)(g_oaPCCARDInform[g_ucPCCardConNum].puPCCARDWriteBuf)+512), *(u32 *)((u32)(g_oaPCCARDInform[g_ucPCCardConNum].puPCCARDReadBuf)+512));
		Dump32((u32)(g_oaPCCARDInform[g_ucPCCardConNum].puPCCARDReadBuf), uBlockCount*128);
	}
	else
	{
		UART_Printf("Write/Read in PC Card mode is OK\n");
	}

	PCCARD_CloseMedia();	

	INTC_Disable( NUM_CFC);
}



u32 PCCARD_SelectMode(void)
{
	UART_Printf("\nWhich mode do you want test? \n");
	UART_Printf(" 0:Memory mode[D], 1:IO mode1(Contiguous), 2 :IO mode2(Primary), 3 :IO mode3(Secondary)\n");
	return (u32)UART_GetIntNum();
}

