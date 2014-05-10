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
*	File Name : onenand_test.c
*  
*	File Description : This file implements the functons for OneNand controller test.
*
*	Author : Heemyung.noh
*	Dept. : AP Development Team
*	Created Date : 2006/11/10
*	Version : 0.1 
* 
*	History
*	- Created(Heemyung.noh 2006/11/10)
*  
**************************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "def.h"
#include "option.h"
#include "library.h"
#include "sfr6410.h"
#include "system.h"
#include "sysc.h"
#include "intc.h"
#include "onenand.h"
#include "dma.h"
#include "timer.h"

#define ONENAND0	0
#define ONENAND1	1

#define ONENANDT_FULLBLOCKERASE		(FALSE)

#define ONDT_ISR_DEBUG			(FALSE)

#define ONDT_ASYNC				(0)
#define ONDT_SYNC				(1)

#define ONDT_ISR_CHECK			(FALSE)

#define ONDT_DATA_RANDOM		(0x10)
#define ONDT_DATA_BLOCK_PAGE	(0x20)
#define ONDT_DATA_ONEVALUE	(0xFFFF0000)
#define ONDT_DATA_PATTERN		ONDT_DATA_RANDOM

#define ONDT_NUMOFBAD_BUFFER	(100)

u32 g_OneNandContNum;

extern volatile ONENAND_oInform	OneNand_Inform[2];
extern OneNandT_oInterruptFlag 	OneNandT_oIntFlag; 
void *OneNandT_ISRCallBack = 0;
static u8 OneNand_ISR_Debug = TRUE;
extern DMAC g_oONDDmac0;
u32 aOneNandBadBlock[ONDT_NUMOFBAD_BUFFER];

//for test
#if (ONDT_ISR_CHECK)
u8 OneNandT_IntCheck[1000] = {0,};
u32 uIntCount=0;
#endif
//extern u32 OneNand_DmaDone;
//extern DMAC g_oONDDmac0;

//////////
// Function Name : OneNandT_DevInformDisplay
// Function Description : Display the information of OneNand Memory device
// Input : 	uController - OneNand Controller Number
// Version : v0.1
void OneNandT_DevInformDisplay(u32 uController)
{
	u32	uManufID, uDeviceID, uDataBufSize, uBootBufSize, uAmountOfBuf, uTechnology;
	float fFlashClcok;
	
	uManufID = OneNand_Inform[uController].uManufID;
	uDeviceID = OneNand_Inform[uController].uDeviceID;
	uDataBufSize = OneNand_Inform[uController].uDataBufSize;
	uBootBufSize = OneNand_Inform[uController].uBootBufSize;
	uAmountOfBuf = OneNand_Inform[uController].uAmountOfBuf;
	uTechnology = OneNand_Inform[uController].uTechnology;
	fFlashClcok = (float)OneNand_Inform[uController].uFlashClock/(float)1000000;

	UART_Printf("OneNand Core Clock : %.04fMHz\n", fFlashClcok);
	UART_Printf("OneNand Interface Clock : %.04fMHz\n", fFlashClcok/2);
	UART_Printf("\n");
	UART_Printf("Manufacturer ID : 0x%04x\n", uManufID);
	UART_Printf("Device ID Reg.: 0x%04x\n", uDeviceID);
	switch((uDeviceID & 0xF0)>>4)
	{
		case 0	:	UART_Printf("Density of OneNand : 128Mb\n");
					break;
		case 1	: 	UART_Printf("Density of OneNand : 256Mb\n");
					break;
		case 2	: 	UART_Printf("Density of OneNand : 512Mb\n");
					break;
		case 3	: 	UART_Printf("Density of OneNand : 1Gb\n");
					break;
		case 4	: 	UART_Printf("Density of OneNand : 2Gb\n");
					break;
		case 5	: 	UART_Printf("Density of OneNand : 4Gb\n");
					break;
		default	:	UART_Printf("Reserved\n");
					break;
	}
	switch((uDeviceID & 0x8)>>3)
	{
		case 0	:	UART_Printf("DDP : Single Data Path\n");
					break;
		case 1	: 	UART_Printf("DDP : Dual Data Path\n");
					break;
		default	:	UART_Printf("Reserved\n");
					break;
	}	
	switch((uDeviceID & 0x4)>>2)
	{
		case 0	:	UART_Printf("Type : Muxed OneNand\n");
					break;
		case 1	: 	UART_Printf("Type : Demuxed OneNand\n");
					break;
		default	:	UART_Printf("Reserved\n");
					break;
	}
	
	UART_Printf("Data Buffer Size : 0x%x\n", uDataBufSize);
	UART_Printf("Boot Buffer Size : 0x%x\n", uBootBufSize);
	UART_Printf("Number of Data Buffer Register : %d\n", uAmountOfBuf>>8);
	UART_Printf("Number of Boot Buffer Register : %d\n", uAmountOfBuf&0xFF);
	UART_Printf("Technology : 0x%04x\n", uTechnology);
	UART_Printf("\n\n");
}



//////////
// Function Name : OneNandT_DisplayInterfaceCondition
// Function Description : Display the information of OneNand Interface Condition
// Input : 	uController - OneNand Controller Number
// Version : v0.1
void OneNandT_DisplayInterfaceCondition(u32 uController)
{
	u32	uMemConfig, uReadMode, uBurstLatency, uBurstLength, uWriteMode, uBurstLenDevice;
	float fFlashClcok;
	
	uMemConfig = ONENAND_GetMemCfg(uController);
	uReadMode = (uMemConfig & (1<<15))>>15;
	uBurstLatency = (uMemConfig & (7<<12))>>12;
	uBurstLength = (uMemConfig & (7<<9))>>9;
	uWriteMode = (uMemConfig & (1<<1))>>1;
	uBurstLenDevice = ONENAND_GetBurstLength(uController);
	
	fFlashClcok = (float)OneNand_Inform[uController].uFlashClock/(float)1000000;
	
	UART_Printf("OneNand Core Clock : %.04fMHz\n", fFlashClcok);
	UART_Printf("OneNand Interface Clock : %.04fMHz\n", fFlashClcok/2);
	UART_Printf("\n");
	switch(uReadMode)
	{
		case 0	:	UART_Printf("Read Mode     : Asynchronous\n");
					break;
		case 1	: 	UART_Printf("Read Mode     : Synchronous\n");
					break;
		default	:	UART_Printf("Reserved\n");
					break;
	}
	switch(uWriteMode)
	{
		case 0	:	UART_Printf("Write Mode    : Asynchronous\n");
					break;
		case 1	: 	UART_Printf("Write Mode    : Synchronous\n");
					break;
		default	:	UART_Printf("Reserved\n");
					break;
	}
	
	UART_Printf("Burst Latency : %d\n", uBurstLatency);
	
	switch(uBurstLength)
	{
		case 0	:	UART_Printf("Burst Length  : Continuous\n");
					break;
		case 1	: 	UART_Printf("Burst Length  : 4 halfwords\n");
					break;
		case 2	: 	UART_Printf("Burst Length  : 8 halfwords\n");
					break;
		case 3	: 	UART_Printf("Burst Length  : 16 halfwords\n");
					break;
		case 4	: 	UART_Printf("Burst Length  : 32 halfwords\n");
					break;
		case 5	: 	UART_Printf("Burst Length  : 1K halfwords\n");
					break;
		default	:	UART_Printf("Reserved\n");
					break;
	}

	UART_Printf("Burst Length(Device) : %d\n", uBurstLenDevice);
}


///////////////////////////////////////////////////////////////////////////////////
///////////////////                     Interrupt ISR Routine                ///////////////////////// 
//////////////////////////////////////////////////////////////////////////////////

//////////
// Function Name : OneNandT_ISR_LdFailEccErr
// Function Description : OneNand IRQ routine(Load Operation Fail or ECC Error)
// Input : 	uController - OneNand Controller Number
// Version : v0.1
void OneNandT_ISR_LdFailEccErr(u32 uController)
{
	u32 uEccErr;
	
	ONENAND_AckInterrupt(uController, (OneNAND_eINTERROR)eOND_LDFAILECCERR);
//#if (ONDT_ISR_DEBUG)
	if(OneNand_ISR_Debug == TRUE)
	{
		UART_Printf("[ONENANDT_ISR_LdFailEccErr(%d)]\n",uController);
		UART_Printf("[rECC_ERR_STAT%d : 0x%08x]\n",uController, ONENAND_GetEccErrStatus(uController));
		ONENAND_DirectRead(uController, OND_ECCSTATUS, &uEccErr);
		UART_Printf("[ECC_ERR_STAT(Device) : 0x%08x]\n",uEccErr);
		UART_Getc();
	}
//#endif
	OneNandT_oIntFlag.LdFailEccErrInt = 1;
}

//////////
// Function Name : OneNandT_ISR_IntTo
// Function Description : OneNand IRQ routine(Interrupt Time-out)
// Input : 	uController - OneNand Controller Number
// Version : v0.1
void OneNandT_ISR_IntTo(u32 uController)
{
	ONENAND_AckInterrupt(uController, (OneNAND_eINTERROR)eOND_INTTO);
	
//#if (ONDT_ISR_DEBUG)
	if(OneNand_ISR_Debug == TRUE)
		UART_Printf("[ONENAND_ISR_IntTo(%d)]\n",uController);
//#endif
//	ONENAND_Reset(g_OneNandContNum, ONENAND_CORERESET);
	*(u32 *)0x7e00fa0c = 0x12;
	
	OneNandT_oIntFlag.IntToInt = 1;

}

//////////
// Function Name : OneNandT_ISR_PgmFail
// Function Description : OneNand IRQ routine(Program Operation Fail)
// Input : 	uController - OneNand Controller Number
// Version : v0.1
void OneNandT_ISR_PgmFail(u32 uController)
{
	ONENAND_AckInterrupt(uController, (OneNAND_eINTERROR)eOND_PGMFAIL);
//#if (ONDT_ISR_DEBUG)
	if(OneNand_ISR_Debug == TRUE)
		UART_Printf("[ONENAND_ISR_PgmFail(%d)]\n",uController);
//#endif
	OneNandT_oIntFlag.PgmFailInt = 1;
}

//////////
// Function Name : OneNandT_ISR_ErsFail
// Function Description : OneNand IRQ routine(Erase Operation Fail)
// Input : 	uController - OneNand Controller Number
// Version : v0.1
void OneNandT_ISR_ErsFail(u32 uController)
{
	ONENAND_AckInterrupt(uController, (OneNAND_eINTERROR)eOND_ERSFAIL);
//#if (ONDT_ISR_DEBUG)
	if(OneNand_ISR_Debug == TRUE)
		UART_Printf("[ONENAND_ISR_ErsFail(%d)]\n",uController);
//#endif
	OneNandT_oIntFlag.ErsFailInt = 1;
}


//////////
// Function Name : OneNandT_ISR_LoadCmp
// Function Description : OneNand IRQ routine(Load Operation has been completed)
// Input : 	uController - OneNand Controller Number
// Version : v0.1
void OneNandT_ISR_LoadCmp(u32 uController)
{
	ONENAND_AckInterrupt(uController, (OneNAND_eINTERROR)eOND_LOADCMP);
#if (ONDT_ISR_DEBUG)
	if(OneNand_ISR_Debug == TRUE)
		UART_Printf("[ONENAND_ISR_LoadCmp(%d)]\n",uController);
#endif
	OneNandT_oIntFlag.LoadCmpInt = 1;

}

//////////
// Function Name : OneNandT_ISR_PgmCmp
// Function Description : OneNand IRQ routine(Program Operation has been completed)
// Input : 	uController - OneNand Controller Number
// Version : v0.1
void OneNandT_ISR_PgmCmp(u32 uController)
{
	ONENAND_AckInterrupt(uController, (OneNAND_eINTERROR)eOND_PGMCMP);
#if (ONDT_ISR_DEBUG)
	if(OneNand_ISR_Debug == TRUE)
		UART_Printf("[ONENAND_ISR_PgmCmp(%d)]\n",uController);
#endif
	OneNandT_oIntFlag.PgmCmpInt = 1;
}

//////////
// Function Name : OneNandT_ISR_ErsCmp
// Function Description : OneNand IRQ routine(Erase Operation has been completed)
// Input : 	uController - OneNand Controller Number
// Version : v0.1
void OneNandT_ISR_ErsCmp(u32 uController)
{
	ONENAND_AckInterrupt(uController, (OneNAND_eINTERROR)eOND_ERSCMP);
#if (ONDT_ISR_DEBUG)
	if(OneNand_ISR_Debug == TRUE)
		UART_Printf("[ONENAND_ISR_ErsCmp(%d)]\n",uController);
#endif
	OneNandT_oIntFlag.ErsCmpInt = 1;

}

//////////
// Function Name : OneNandT_ISR_BlkRwCmp
// Function Description : OneNand IRQ routine(Teh Block read or write transfer has been completed)
// Input : 	uController - OneNand Controller Number
// Version : v0.1
void OneNandT_ISR_BlkRwCmp(u32 uController)
{
	ONENAND_AckInterrupt(uController, (OneNAND_eINTERROR)eOND_BLKRWCMP);
#if (ONDT_ISR_DEBUG)
	if(OneNand_ISR_Debug == TRUE)
		UART_Printf("[ONENAND_ISR_BlkRwCmp(%d)]\n",uController);
#endif
	OneNandT_oIntFlag.BlkRwCmpInt = 1;
}

//////////
// Function Name : OneNandT_ISR_LockedBlk
// Function Description : OneNand IRQ routine(The address to program or erase is in a protected block)
// Input : 	uController - OneNand Controller Number
// Version : v0.1
void OneNandT_ISR_LockedBlk(u32 uController)
{
	ONENAND_AckInterrupt(uController, (OneNAND_eINTERROR)eOND_LOCKEDBLK);
#if (ONDT_ISR_DEBUG)
	if(OneNand_ISR_Debug == TRUE)
		UART_Printf("[ONENAND_ISR_LockedBlk(%d)]\n",uController);
#endif
	OneNandT_oIntFlag.LockedBlkInt = 1;

}

//////////
// Function Name : OneNandT_ISR_UnsupCmd
// Function Description : OneNand IRQ routine(Unsupported command)
// Input : 	uController - OneNand Controller Number
// Version : v0.1
void OneNandT_ISR_UnsupCmd(u32 uController)
{
	ONENAND_AckInterrupt(uController, (OneNAND_eINTERROR)eOND_UNSUPCMD);
//#if (ONDT_ISR_DEBUG)
	if(OneNand_ISR_Debug == TRUE)
		UART_Printf("[ONENAND_ISR_UnsupCmd(%d)]\n",uController);
//#endif
	OneNandT_oIntFlag.UnsupCmdInt = 1;

}

//////////
// Function Name : OneNandT_ISR_IntAct
// Function Description : OneNand IRQ routine(Device's INT pin is actively transitioning)
// Input : 	uController - OneNand Controller Number
// Version : v0.1
void OneNandT_ISR_IntAct(u32 uController)
{
	ONENAND_AckInterrupt(uController, (OneNAND_eINTERROR)eOND_INTACT);
//#if (ONDT_ISR_DEBUG)
//	UART_Printf("[ONENAND_ISR_IntAct(%d)]\n",uController);
//#endif
	OneNandT_oIntFlag.IntActInt = 1;
	OneNandT_oIntFlag.IntActInt_Count ++;
}

//////////
// Function Name : OneNandT_ISR_RdyAct
// Function Description : OneNand IRQ routine(Device's RDY pin is actively transitioning)
// Input : 	uController - OneNand Controller Number
// Version : v0.1
void OneNandT_ISR_RdyAct(u32 uController)
{
	ONENAND_AckInterrupt(uController, (OneNAND_eINTERROR)eOND_RDYACT);
//#if (ONDT_ISR_DEBUG)	
//	UART_Printf("[ONENAND_ISR_RdyAct(%d)]\n",uController);
//#endif
	OneNandT_oIntFlag.RdyActInt = 1;
}

//////////
// Function Name : OneNandT_ISR_RstCmp
// Function Description : OneNand IRQ routine(Reset and Initialization has been completed)
// Input : 	uController - OneNand Controller Number
// Version : v0.1
void OneNandT_ISR_RstCmp(u32 uController)
{
	ONENAND_AckInterrupt(uController, (OneNAND_eINTERROR)eOND_RSTCMP);
//#if (ONDT_ISR_DEBUG)
//	UART_Printf("[ONENAND_ISR_RstCmp(%d)]\n",uController);
//#endif
	OneNandT_oIntFlag.RstCmpInt = 1;
}


//////////
// Function Name : OneNandT_ISR_CacheOpErr
// Function Description : OneNand IRQ routine(An error occurred during a cache read or write setup or operation)
// Input : 	uController - OneNand Controller Number
// Version : v0.1
void OneNandT_ISR_CacheOpErr(u32 uController)
{
	ONENAND_AckInterrupt(uController, (OneNAND_eINTERROR)eOND_CACHEOPERR);
#if (ONDT_ISR_DEBUG)
	if(OneNand_ISR_Debug == TRUE)
		UART_Printf("[ONENAND_ISR_CacheOpErr(%d)]\n",uController);
#endif
	OneNandT_oIntFlag.CacheOpErrInt = 1;

}


//////////
// Function Name : OneNandT_ISR_Handler
// Function Description : OneNand IRQ Main Routine
// Input : 	uController - OneNand Controller Number
// Version : v0.1
void OneNandT_ISR_Handler(u32 uController)
{
	u32 uInterrupt;
	
	uInterrupt = ONENAND_GetInterruptStatus(uController);
	uInterrupt &= ONENAND_GetInterruptMask(uController);		//Enabled Interrupt Check

	if(uInterrupt & (eOND_LDFAILECCERR))
	{
		OneNandT_ISR_LdFailEccErr(uController);	
#if (ONDT_ISR_CHECK)
		OneNandT_IntCheck[uIntCount]=1;
		uIntCount++;
		if(uIntCount == 1000)
		{
			uIntCount = 0;
			for(i=0 ; i<1000 ; i++)
				OneNandT_IntCheck[i]=0;
		}
#endif
	}
	if(uInterrupt & (eOND_INTTO))
	{
		OneNandT_ISR_IntTo(uController);
#if (ONDT_ISR_CHECK)
		OneNandT_IntCheck[uIntCount]=2;
		uIntCount++;
		if(uIntCount == 1000)
		{
			uIntCount = 0;
			for(i=0 ; i<1000 ; i++)
				OneNandT_IntCheck[i]=0;
		}
#endif
	}
	if(uInterrupt & (eOND_PGMFAIL))
	{
		OneNandT_ISR_PgmFail(uController);	
#if (ONDT_ISR_CHECK)
		OneNandT_IntCheck[uIntCount]=3;
		uIntCount++;
		if(uIntCount == 1000)
		{
			uIntCount = 0;
			for(i=0 ; i<1000 ; i++)
				OneNandT_IntCheck[i]=0;
		}
#endif
	}
	if(uInterrupt & (eOND_ERSFAIL))
	{
		OneNandT_ISR_ErsFail(uController);	
#if (ONDT_ISR_CHECK)
		OneNandT_IntCheck[uIntCount]=4;
		uIntCount++;
		if(uIntCount == 1000)
		{
			uIntCount = 0;
			for(i=0 ; i<1000 ; i++)
				OneNandT_IntCheck[i]=0;
		}
#endif
	}
	if(uInterrupt & (eOND_LOADCMP))
	{
		OneNandT_ISR_LoadCmp(uController);	
#if (ONDT_ISR_CHECK)
		OneNandT_IntCheck[uIntCount]=5;
		uIntCount++;
		if(uIntCount == 1000)
		{
			uIntCount = 0;
			for(i=0 ; i<1000 ; i++)
				OneNandT_IntCheck[i]=0;
		}
#endif
	}
	if(uInterrupt & (eOND_PGMCMP))
	{
		OneNandT_ISR_PgmCmp(uController);	
#if (ONDT_ISR_CHECK)
		OneNandT_IntCheck[uIntCount]=6;
		uIntCount++;
		if(uIntCount == 1000)
		{
			uIntCount = 0;
			for(i=0 ; i<1000 ; i++)
				OneNandT_IntCheck[i]=0;
		}
#endif
	}
	if(uInterrupt & (eOND_ERSCMP))
	{
		OneNandT_ISR_ErsCmp(uController);	
#if (ONDT_ISR_CHECK)
		OneNandT_IntCheck[uIntCount]=7;
		uIntCount++;
		if(uIntCount == 1000)
		{
			uIntCount = 0;
			for(i=0 ; i<1000 ; i++)
				OneNandT_IntCheck[i]=0;
		}
#endif
	}
	if(uInterrupt & (eOND_BLKRWCMP))
	{
		OneNandT_ISR_BlkRwCmp(uController);
#if (ONDT_ISR_CHECK)
		OneNandT_IntCheck[uIntCount]=8;
		uIntCount++;
		if(uIntCount == 1000)
		{
			uIntCount = 0;
			for(i=0 ; i<1000 ; i++)
				OneNandT_IntCheck[i]=0;
		}
#endif
	}
	if(uInterrupt & (eOND_LOCKEDBLK))
	{
		OneNandT_ISR_LockedBlk(uController);
#if (ONDT_ISR_CHECK)
		OneNandT_IntCheck[uIntCount]=9;
		uIntCount++;
		if(uIntCount == 1000)
		{
			uIntCount = 0;
			for(i=0 ; i<1000 ; i++)
				OneNandT_IntCheck[i]=0;
		}
#endif
	}
	if(uInterrupt & (eOND_UNSUPCMD))
	{
		OneNandT_ISR_UnsupCmd(uController);	
#if (ONDT_ISR_CHECK)
		OneNandT_IntCheck[uIntCount]=10;
		uIntCount++;
		if(uIntCount == 1000)
		{
			uIntCount = 0;
			for(i=0 ; i<1000 ; i++)
				OneNandT_IntCheck[i]=0;
		}
#endif
	}
	if(uInterrupt & (eOND_INTACT))
	{
		OneNandT_ISR_IntAct(uController);	
#if (ONDT_ISR_CHECK)
		OneNandT_IntCheck[uIntCount]=11;
		uIntCount++;
		if(uIntCount == 1000)
		{
			uIntCount = 0;
			for(i=0 ; i<1000 ; i++)
				OneNandT_IntCheck[i]=0;
		}
#endif
	}
	if(uInterrupt & (eOND_RDYACT))
	{
		OneNandT_ISR_RdyAct(uController);
#if (ONDT_ISR_CHECK)
		OneNandT_IntCheck[uIntCount]=12;
		uIntCount++;
		if(uIntCount == 1000)
		{
			uIntCount = 0;
			for(i=0 ; i<1000 ; i++)
				OneNandT_IntCheck[i]=0;
		}
#endif
	}
	if(uInterrupt & (eOND_RSTCMP))
	{
		OneNandT_ISR_RstCmp(uController);	
#if (ONDT_ISR_CHECK)
		OneNandT_IntCheck[uIntCount]=13;
		uIntCount++;
		if(uIntCount == 1000)
		{
			uIntCount = 0;
			for(i=0 ; i<1000 ; i++)
				OneNandT_IntCheck[i]=0;
		}
#endif
	}
	if(uInterrupt & (eOND_CACHEOPERR))
	{
		OneNandT_ISR_CacheOpErr(uController);		
#if (ONDT_ISR_CHECK)
		OneNandT_IntCheck[uIntCount]=14;
		uIntCount++;
		if(uIntCount == 1000)
		{
			uIntCount = 0;
			for(i=0 ; i<1000 ; i++)
				OneNandT_IntCheck[i]=0;
		}
#endif
	}
	//UART_Printf("IS:0x%04x\n",uInterrupt);	
}



//////////
// Function Name : OneNandT_SetISRHandler
// Function Description : Register Interrupt Handler 
// Input : 	None
// Version : v0.1
void OneNandT_SetISRHandler(u32 Controller)
{
	if(Controller == ONENAND0)
	{
		INTC_Enable(NUM_ONENAND0);
		INTC_SetVectAddr(NUM_ONENAND0, ONENAND_ISR);	
	}
	else if(Controller == ONENAND1)
	{
		INTC_Enable(NUM_ONENAND1);
		INTC_SetVectAddr(NUM_ONENAND1, ONENAND_ISR);
	}
	
	OneNandT_ISRCallBack = (void *)OneNandT_ISR_Handler;
	
	ONENAND_SetISRHandler(OneNandT_ISRCallBack);
}
	

///////////////////////////////////////////////////////////////////////////////////
////////////////////////                     Lock Test                ////////////////////////////// 
///////////////////////////////////////////////////////////////////////////////////


void OneNandT_ReadLockStatus(u32 uStartBlock, u32 uEndBlock, u8 *pLockStatus)
{
	u32 i, uData;
	
	for(i=uStartBlock ; i<=uEndBlock ; i++)
	{
		ONENAND_DirectWrite(g_OneNandContNum, OND_STARTADDR1, i);
		DelayfrTimer(milli, 10);
		ONENAND_DirectRead(g_OneNandContNum, OND_WPROT_STATUS, &uData);

		if ((uData&0xFFFF) == 0x1) 
		{
			*(pLockStatus+i) = ONENAND_LOCKTIGHT;
		}
		else if ((uData&0xFFFF) == 0x2) 
		{
			*(pLockStatus+i) = ONENAND_LOCK;
		}		
		else
		{
			*(pLockStatus+i) = ONENAND_UNLOCK;
		}
	}


}


//////////
// Function Name : OneNandT_UnlockBlock
// Function Description : Memory Block Unlock Test
// Input : 	eTest - Manual test or Auto test selection
//			oAutoVar - Parameters for Auto-Test
// Version : v0.1
void OneNandT_UnlockBlock(eFunction_Test eTest, oFunctionT_AutoVar oAutoVar)
{
	u8 uError = FALSE;
	u32 i, uStartBlkAddr, uEndBlkAddr, uData;
	OneNAND_eINTERROR eErrorType;

	if(eTest == eTest_Manual)
		UART_Printf("[OneNandT_UnlockBlock]\n");

	if(eTest == eTest_Manual)
	{	
		UART_Printf("Input the Start block to unlock[0~%d] : \n", OneNand_Inform[g_OneNandContNum].uNumOfBlock-1);
		uStartBlkAddr = UART_GetIntNum(); 
		UART_Printf("Input the End block to unlock[0~%d] : \n", OneNand_Inform[g_OneNandContNum].uNumOfBlock-1);
		uEndBlkAddr = UART_GetIntNum(); 
	}
	else
	{
		uStartBlkAddr = oAutoVar.Test_Parameter[0]; 
		uEndBlkAddr = oAutoVar.Test_Parameter[1]; 
	}
	
	ONENAND_UnlockBlock(g_OneNandContNum, uStartBlkAddr, uEndBlkAddr);

	if(eTest == eTest_Manual)
		UART_Printf("Lock Status Read\n");
	
	for(i=uStartBlkAddr ; i<=uEndBlkAddr ; i++)
	{
		uError = FALSE;
		ONENAND_DirectWrite(g_OneNandContNum, OND_STARTADDR1, i);
		ONENAND_DirectRead(g_OneNandContNum, OND_WPROT_STATUS, &uData);

		if ((uData&0xFFFF) != 0x4) 
		{
			UART_Printf(" Failed unlock block, locked status (0x%x) : %d Block\n", uData, i);
			uError = TRUE;
			UART_Getc();
			//break;
		}
	}
	if((eTest == eTest_Manual)&&(uError==FALSE))
		UART_Printf("Lock Status Read....OK\n");
	
	if(eTest == eTest_Manual)
		UART_Printf("Erase the unlocked block\n");

	OneNand_ISR_Debug = FALSE;
	for(i=uStartBlkAddr ; i<=uEndBlkAddr ; i++)
	{
		eErrorType = ONENAND_EraseBlock(g_OneNandContNum, i, i);

		if(eErrorType != eOND_NOERROR)
		{
			UART_Printf("Fail......Erased the Unlock Block : %d Block\n", i);
			uError = TRUE;
			UART_Getc();
		}
	}
	if((eTest == eTest_Manual)&&(uError==FALSE))
		UART_Printf("Erase the unlocked block...OK\n");

	if(eTest == eTest_Manual)
	{	
		if (uError==TRUE) 
		{
			UART_Printf("\nOneNAND unlock block FAILED!!!\n");
		}	
		else 
		{
			UART_Printf("\nOneNAND unlock block SUCCESS\n");
		}
	}
}


//////////
// Function Name : OneNandT_UnlockAllBlock
// Function Description : Memory All Block Unlock Test
// Input : 	eTest - Manual test or Auto test selection
//			oAutoVar - Parameters for Auto-Test
// Version : v0.1
void OneNandT_UnlockAllBlock(eFunction_Test eTest, oFunctionT_AutoVar oAutoVar)
{
	OneNAND_eINTERROR uError;
	u8 bError = FALSE;
	u32 i, uData;

	UART_Printf("[OneNandT_UnlockAllBlock]\n");
	
	uError = ONENAND_UnlockAllBlock(g_OneNandContNum);
	if (uError==eOND_UNSUPCMD) 
	{
		UART_Printf("\nError : OneNAND Unsupported Command(All Block Erase[0x0E command])\n");

		// OneNand unlock using 0x08/0x09 command
		UART_Printf("OneNAND unlock block will be retry using 0x08/0x09 command\n");
		//ONENAND_UnlockBlock(g_OneNandContNum, 0, OneNand_Inform[g_OneNandContNum].uNumOfBlock-1);
		if(OneNand_Inform[g_OneNandContNum].uDbsDfs == 0)
			ONENAND_UnlockBlock(g_OneNandContNum, 0, OneNand_Inform[g_OneNandContNum].uNumOfBlock-1);
		else
		{
			ONENAND_UnlockBlock(g_OneNandContNum, 0, (OneNand_Inform[g_OneNandContNum].uNumOfBlock/2)-1);
			ONENAND_UnlockBlock(g_OneNandContNum, OneNand_Inform[g_OneNandContNum].uNumOfBlock/2, OneNand_Inform[g_OneNandContNum].uNumOfBlock-1);
		}
		
		for(i=0 ; i<=OneNand_Inform[g_OneNandContNum].uNumOfBlock ; i++)
		{
			bError = FALSE;
			ONENAND_DirectWrite(g_OneNandContNum, OND_STARTADDR1, i);
			ONENAND_DirectRead(g_OneNandContNum, OND_WPROT_STATUS, &uData);

			if ((uData&0xFFFF) != 0x4) 
			{
				UART_Printf(" Failed unlock block, locked status (0x%x) : %d Block\n", uData, i);
				bError = TRUE;
				UART_Getc();
				break;
			}
		}
	
		if (bError==TRUE) 
		{
			UART_Printf("\nOneNAND unlock block FAILED!!!\n");
		}	
		else 
		{
			UART_Printf("\nOneNAND unlock block SUCCESS\n");
		}	
	}	
	else 
	{
		UART_Printf("\nOneNAND unlock all block SUCCESS\n");
	}	
}


//////////
// Function Name : OneNandT_LockBlock
// Function Description : Memory Block Lock Test
// Input : 	eTest - Manual test or Auto test selection
//			oAutoVar - Parameters for Auto-Test
// Version : v0.1
void OneNandT_LockBlock(eFunction_Test eTest, oFunctionT_AutoVar oAutoVar)
{
	u8 uError = FALSE;
	u32 i, uStartBlkAddr, uEndBlkAddr, uData;
	OneNAND_eINTERROR eErrorType;

	if(eTest == eTest_Manual)
		UART_Printf("[OneNandT_LockBlock]\n");

	if(eTest == eTest_Manual)
	{
		UART_Printf("Input the Start block to lock[0~%d] : \n", OneNand_Inform[g_OneNandContNum].uNumOfBlock-1);
		uStartBlkAddr = UART_GetIntNum(); 
		UART_Printf("Input the End block to  lock[0~%d] : \n", OneNand_Inform[g_OneNandContNum].uNumOfBlock-1);
		uEndBlkAddr = UART_GetIntNum(); 
	}
	else
	{
		uStartBlkAddr = oAutoVar.Test_Parameter[0]; 
		uEndBlkAddr = oAutoVar.Test_Parameter[1]; 
	}
	
	ONENAND_LockBlock(g_OneNandContNum, uStartBlkAddr, uEndBlkAddr);

	if(eTest == eTest_Manual)
		UART_Printf("Lock Status Read\n");
	
	for(i=uStartBlkAddr ; i<=uEndBlkAddr ; i++)
	{
		uError = FALSE;
		ONENAND_DirectWrite(g_OneNandContNum, OND_STARTADDR1, i);
		ONENAND_DirectRead(g_OneNandContNum, OND_WPROT_STATUS, &uData);

		if ((uData&0xFFFF) != ONENAND_LOCK) 
		{
			UART_Printf(" Failed lock block, locked status (0x%x) : %d Block\n", uData, i);
			uError = TRUE;
			UART_Getc();
			//break;
		}
	}
	if((eTest == eTest_Manual)&&(uError==FALSE))
		UART_Printf("Lock Status Read....OK\n");

	if(eTest == eTest_Manual)
		UART_Printf("Erase the locked block\n");

	OneNand_ISR_Debug = FALSE;
	for(i=uStartBlkAddr ; i<=uEndBlkAddr ; i++)
	{
		eErrorType = ONENAND_EraseBlock(g_OneNandContNum, i, i);

		if(!(eErrorType&eOND_LOCKEDBLK))
		{
			UART_Printf("Fail......Erased the Lock Block : %d Block\n", i);
			uError = TRUE;
			UART_Getc();
		}
	}
	OneNand_ISR_Debug = TRUE;
	
	if((eTest == eTest_Manual)&&(uError==FALSE))
		UART_Printf("Erase the locked block...OK\n");
	
	if(eTest == eTest_Manual)
	{
		if (uError==TRUE) 
		{
			UART_Printf("\nOneNAND lock block FAILED!!!\n");
		}
		else 
		{
			UART_Printf("\nOneNAND lock block SUCCESS\n");
		}
		UART_Printf("\n");
	}
}


//////////
// Function Name : OneNandT_LockTight
// Function Description : Memory Block Lock-Tight Test
// Input : 	eTest - Manual test or Auto test selection
//			oAutoVar - Parameters for Auto-Test
// Version : v0.1
void OneNandT_LockTight(eFunction_Test eTest, oFunctionT_AutoVar oAutoVar)
{
	u8 bError = FALSE;
	u32 i, uStartBlkAddr, uEndBlkAddr, uData;
	u8 *pLockStatus;
	OneNAND_eINTERROR eErrorType;

	if(eTest == eTest_Manual)
		UART_Printf("[OneNandT_LockTight]\n");

	if(eTest == eTest_Manual)
	{
		UART_Printf("Input the Start block to lock-tight[0~%d] : \n", OneNand_Inform[g_OneNandContNum].uNumOfBlock-1);
		uStartBlkAddr = UART_GetIntNum(); 
		UART_Printf("Input the End block to  lock-tight[0~%d] : \n", OneNand_Inform[g_OneNandContNum].uNumOfBlock-1);
		uEndBlkAddr = UART_GetIntNum(); 
	}
	else
	{
		uStartBlkAddr = oAutoVar.Test_Parameter[0]; 
		uEndBlkAddr = oAutoVar.Test_Parameter[1]; 
	}
	
#if 0	
	bError = ONENAND_LockBlock(g_OneNandContNum, uStartBlkAddr, uEndBlkAddr);
	if (bError==FALSE) 
		UART_Printf("\nOneNAND lock block FAILED!!!\n");
	else 
		UART_Printf("\nOneNAND lock block SUCCESS\n");
#endif

	pLockStatus = (u8 *)malloc(OneNand_Inform[g_OneNandContNum].uNumOfBlock);

	if(pLockStatus == 0)
	{
		UART_Printf("Memory Allocation Error...\n");
		return;
	}
	
	OneNandT_ReadLockStatus(uStartBlkAddr, uEndBlkAddr, pLockStatus);
	
	ONENAND_LockTightBlock(g_OneNandContNum, uStartBlkAddr, uEndBlkAddr);

	if(eTest == eTest_Manual)
		UART_Printf("Lock-tight Status Read\n");
	
	for(i=uStartBlkAddr ; i<=uEndBlkAddr ; i++)
	{
		ONENAND_DirectWrite(g_OneNandContNum, OND_STARTADDR1, i);
		ONENAND_DirectRead(g_OneNandContNum, OND_WPROT_STATUS, &uData);

		if(*(pLockStatus+i) != ONENAND_UNLOCK)
		{
			if ((uData&0xFFFF) != ONENAND_LOCKTIGHT) 
			{
				UART_Printf(" Failed lock-tight block, locked status (0x%x) : %d Block\n", uData, i);
				bError = TRUE;
				UART_Getc();
				//break;
			}
		}
		else
		{
			if ((uData&0xFFFF) != ONENAND_UNLOCK) 
			{
				UART_Printf(" Fail : lock-tight the Unlock block, locked status (0x%x) : %d Block\n", uData, i);
				bError = TRUE;
				UART_Getc();
				//break;
			}
		}
	}
	if((eTest == eTest_Manual)&&(bError==FALSE))
		UART_Printf("Lock-tight Status Read....OK\n");

	if(bError == TRUE)
	{
		free(pLockStatus);
		return;
	}
	else
		OneNandT_ReadLockStatus(uStartBlkAddr, uEndBlkAddr, pLockStatus);
	
	if(eTest == eTest_Manual)
		UART_Printf("Erase the lock-tighted block\n");
	
	OneNand_ISR_Debug = FALSE;
	for(i=uStartBlkAddr ; i<=uEndBlkAddr ; i++)
	{
		eErrorType = ONENAND_EraseBlock(g_OneNandContNum, i, i);

		if(*(pLockStatus+i) != ONENAND_UNLOCK)
		{
			if(!(eErrorType&eOND_LOCKEDBLK))
			{
				UART_Printf("Fail......Erased the Lock-tighted Block : %d Block\n", i);
				bError = TRUE;
				UART_Getc();
			}
		}
		else		//if(*(pLockStatus+i-uStartBlkAddr) == ONENAND_UNLOCK)
		{
			if(eErrorType != eOND_NOERROR)
			{
				UART_Printf("Fail......Erased the Unlocked Block : %d Block\n", i);
				bError = TRUE;
				UART_Getc();
			}
		}
	}
	OneNand_ISR_Debug = TRUE;
	
	if((eTest == eTest_Manual)&&(bError==FALSE))
		UART_Printf("Erase the lock-tighted block...OK\n");

	if(bError == TRUE)
	{
		free(pLockStatus);
		return;
	}
	
	if(eTest == eTest_Manual)
		UART_Printf("Unlock the lock-tighted block\n");
	
	ONENAND_UnlockBlock(g_OneNandContNum, uStartBlkAddr, uEndBlkAddr);

	for(i=uStartBlkAddr ; i<=uEndBlkAddr ; i++)
	{
		ONENAND_DirectWrite(g_OneNandContNum, OND_STARTADDR1, i);
		ONENAND_DirectRead(g_OneNandContNum, OND_WPROT_STATUS, &uData);

		if(*(pLockStatus+i) == ONENAND_LOCKTIGHT)
		{
			if ((uData&0xFFFF) != ONENAND_LOCKTIGHT) 
			{
				UART_Printf(" Fail...unlocking the lock-tighted block, locked status (0x%x) : %d Block\n", uData, i);
				bError = TRUE;
				UART_Getc();
				//break;
			}
		}
		else
		{
			if ((uData&0xFFFF) != ONENAND_UNLOCK) 
			{
				UART_Printf(" Fail...Unlocking the lock or unlock block, locked status (0x%x) : %d Block\n", uData, i);
				bError = TRUE;
				UART_Getc();
				//break;
			}
		}
	}
	
	if((eTest == eTest_Manual)&&(bError==FALSE))
		UART_Printf("Unlock the lock-tighted block....OK\n");		

	if(bError == TRUE)
	{
		free(pLockStatus);
		return;
	}
	
	if(eTest == eTest_Manual)
	{
		if (bError==TRUE) 
		{
			UART_Printf("\nOneNAND lock block FAILED!!!\n");
		}
		else 
		{
			UART_Printf("\nOneNAND lock block SUCCESS\n");	
		}
	}

	free(pLockStatus);
}


//////////
// Function Name : OneNandT_LockUnlock_Aging
// Function Description : Memory Block Lock & Unlock Aging Test
// Input : 	eTest - Manual test or Auto test selection
//			oAutoVar - Parameters for Auto-Test
// Version : v0.1
void OneNandT_LockUnlock_Aging(eFunction_Test eTest, oFunctionT_AutoVar oAutoVar)
{
	u32 i, uRepeatNum;
	
	UART_Printf("[OneNandT_LockUnlock_Aging]\n");

	UART_Printf("Input the Lock&Unlock repeat numner : \n");
	uRepeatNum = UART_GetIntNum(); 
	
	if(OneNand_Inform[g_OneNandContNum].uDbsDfs == 0)
	{
		for(i=0 ; i<uRepeatNum ; i++)
		{
			UART_Printf("Unlock All [%d]\n",i);
			ONENAND_UnlockBlock(g_OneNandContNum, 0, OneNand_Inform[g_OneNandContNum].uNumOfBlock-1);
			DelayfrTimer(milli, 100);
			UART_Printf("Lock All [%d]\n",i);
			ONENAND_LockBlock(g_OneNandContNum, 0, OneNand_Inform[g_OneNandContNum].uNumOfBlock-1);
			DelayfrTimer(milli, 100);
		}
	}
	else
	{
		for(i=0 ; i<uRepeatNum ; i++)
		{	
			UART_Printf("Unlock All [%d]\n",i);
			ONENAND_UnlockBlock(g_OneNandContNum, 0, (OneNand_Inform[g_OneNandContNum].uNumOfBlock/2)-1);
			ONENAND_UnlockBlock(g_OneNandContNum, OneNand_Inform[g_OneNandContNum].uNumOfBlock/2, OneNand_Inform[g_OneNandContNum].uNumOfBlock-1);
			DelayfrTimer(milli, 100);
			UART_Printf("Lock All [%d]\n",i);
			ONENAND_LockBlock(g_OneNandContNum, 0, (OneNand_Inform[g_OneNandContNum].uNumOfBlock/2)-1);
			ONENAND_LockBlock(g_OneNandContNum, OneNand_Inform[g_OneNandContNum].uNumOfBlock/2, OneNand_Inform[g_OneNandContNum].uNumOfBlock-1);
			DelayfrTimer(milli, 100);		
		}
	}


}


const AutotestFuncMenu onenand_lock[] =
{
	OneNandT_UnlockBlock,             				"Unlock Block",
	OneNandT_UnlockAllBlock,					"Unlock All Block",
	OneNandT_LockBlock,						"Lock Block",
	OneNandT_LockTight,						"Lock-tight Block",
	OneNandT_LockUnlock_Aging,					"Lock, Unlock Aging",
	0, 0
};

void OneNandT_LockTest(void)
{
	u32 i;
	s32 uSel;
	oFunctionT_AutoVar oParameter;
	
	while(1)
	{
		for (i=0; (u32)(onenand_lock[i].desc)!=0; i++)
			UART_Printf("%2d: %s\n", i, onenand_lock[i].desc);

		UART_Printf("\nSelect the function to test : \n");
		uSel =UART_GetIntNum();
		UART_Printf("\n");
		if(uSel == -1) 
			break;

		if (uSel>=0 && uSel<(sizeof(onenand_lock)/8-1))
			(onenand_lock[uSel].func) (eTest_Manual, oParameter);
	}
}




///////////////////////////////////////////////////////////////////////////////////
////////////////////////                     Erase Test              ////////////////////////////// 
///////////////////////////////////////////////////////////////////////////////////

//////////
// Function Name : OneNandT_EraseBlock
// Function Description : Memory Block Erase Test
// Input : 	eTest - Manual test or Auto test selection
//			oAutoVar - Parameters for Auto-Test
// Version : v0.1
void OneNandT_EraseBlock(eFunction_Test eTest, oFunctionT_AutoVar oAutoVar)
{
	OneNAND_eINTERROR uError;
	u32 i, uStartBlkAddr, uEndBlkAddr;
	//u8 bVerify;

	UART_Printf("[OneNandT_EraseBlock]\n");
	if(eTest == eTest_Manual)
	{
		UART_Printf("Input the Start block to erase[0~%d] : \n", OneNand_Inform[g_OneNandContNum].uNumOfBlock-1);
		uStartBlkAddr = UART_GetIntNum(); 
		UART_Printf("Input the End block to  erase[0~%d] : \n", OneNand_Inform[g_OneNandContNum].uNumOfBlock-1);
		uEndBlkAddr = UART_GetIntNum(); 
	}
	else
	{
		uStartBlkAddr = oAutoVar.Test_Parameter[0];
		uEndBlkAddr = oAutoVar.Test_Parameter[1];
	}
	
#if	(ONENANDT_FULLBLOCKERASE == TRUE)	//Full Block Erase
	{
		u32 j;
		
		for(j=0 ; j<OneNand_Inform[g_OneNandContNum].uNumOfBlock ; j++)
		{
			uStartBlkAddr = uEndBlkAddr = j;
#endif

	for(i=0 ; i<200 ; i++)
	{
	uError = ONENAND_EraseBlock(g_OneNandContNum, uStartBlkAddr, uEndBlkAddr);
	
	if (uError&eOND_ERSFAIL) 
	{
		UART_Printf("\nOneNAND erase block FAILED!!!\n");	
		if(uError&eOND_LOCKEDBLK)
		{
			UART_Printf(" =>The address to erase is in a protected block\n");	
			UART_Getc();
			return;
		}
	}
	else if (uError==eOND_NOERROR) 
	{
		if(uStartBlkAddr == uEndBlkAddr)
			UART_Printf("\nOneNAND erase block SUCCESS : %d Block\n", uStartBlkAddr);
		else
			UART_Printf("\nOneNAND erase block SUCCESS : %d Block ~ %d Block\n", uStartBlkAddr, uEndBlkAddr);
	}
	UART_Printf("\nCount : %d\n", i);
	}
	
#if 0
	bVerify = TRUE;
	for(i=uStartBlkAddr ; i<=uEndBlkAddr ; i++)
	{
		uError = ONENAND_EraseVerify(g_OneNandContNum, i);

		if (uError&eOND_ERSFAIL) 
		{
			UART_Printf("\nOneNAND erase block verify FAILED[Block : %d]\n", i);	
			bVerify = FALSE;
			//break;
		}
	}

	if(bVerify)
		UART_Printf("OneNAND erase block Verify OK\n");
	else
	{
		UART_Printf("OneNAND erase block Verify FAIL\n");
		UART_Getc();
	}
#endif

#if	(ONENANDT_FULLBLOCKERASE == TRUE)
		}
	}
#endif

	UART_Printf("\n");
}


//////////
// Function Name : OneNandT_EraseVerify
// Function Description : Memory Block Erase Verify Test
// Input : 	eTest - Manual test or Auto test selection
//			oAutoVar - Parameters for Auto-Test
// Version : v0.1
void OneNandT_EraseVerify(eFunction_Test eTest, oFunctionT_AutoVar oAutoVar)
{
	OneNAND_eINTERROR uError;
	u32 uStartBlkAddr, uEndBlkAddr;	//, i;

	if(eTest == eTest_Manual)
	{
		UART_Printf("[OneNandT_EraseVerify]\n");
	
		UART_Printf("Input the Start block to erase[0~%d] : \n", OneNand_Inform[g_OneNandContNum].uNumOfBlock-1);
		uStartBlkAddr = UART_GetIntNum(); 
		UART_Printf("Input the End block to  erase[0~%d] : \n", OneNand_Inform[g_OneNandContNum].uNumOfBlock-1);
		uEndBlkAddr = UART_GetIntNum(); 
	}
	else
	{
		uStartBlkAddr = oAutoVar.Test_Parameter[0];
		uEndBlkAddr = oAutoVar.Test_Parameter[1];
	}
	
	uError = ONENAND_EraseBlock(g_OneNandContNum, uStartBlkAddr, uEndBlkAddr);
	
	if (uError&eOND_ERSFAIL) 
	{
		UART_Printf("OneNAND erase block FAILED!!!\n");	
		UART_Getc();
	}
	else if (uError==eOND_NOERROR) 
	{
		UART_Printf("OneNAND erase block SUCCESS : %d ~ %d Block\n", uStartBlkAddr, uEndBlkAddr);
	}

#if 0
	for(i=uStartBlkAddr ; i<=uEndBlkAddr ; i++)
	{
		uError = ONENAND_EraseVerify(g_OneNandContNum, i);

		if (uError&eOND_ERSFAIL) 
		{
			UART_Printf("\nOneNAND erase block verify FAILED[Block : %d]\n", i);	
			break;
		}
	}


	if (uError==eOND_NOERROR) 
	{
		UART_Printf("\nOneNAND erase block verify SUCCESS\n");
	}	
#endif	
}

//////////
// Function Name : OneNandT_CheckBadBlock
// Function Description : Check the bad block
// Input : 	eTest - Manual test or Auto test selection
//			oAutoVar - Parameters for Auto-Test
// Version : v0.1
void OneNandT_CheckBadBlock(eFunction_Test eTest, oFunctionT_AutoVar oAutoVar)
{
	u32 i, j;
	OneNAND_eINTERROR uError;

	for(i=0 ; i<ONDT_NUMOFBAD_BUFFER; i++)
		aOneNandBadBlock[i] = OneNand_Inform[g_OneNandContNum].uNumOfBlock;
	
	j = 0;
	// Except 0 Block
	for(i=1 ; i<OneNand_Inform[g_OneNandContNum].uNumOfBlock ; i++)
	{
		uError = ONENAND_EraseBlock(g_OneNandContNum, i, i);
		
		if (uError&eOND_ERSFAIL) 
		{
			aOneNandBadBlock[j++] = i;
		}
	}

	if(eTest == eTest_Manual)
	{
		UART_Printf("OneNAND Bad Block : \n");	

		j = 0;
		while(1)
		{
			if(aOneNandBadBlock[j] != OneNand_Inform[g_OneNandContNum].uNumOfBlock)
			{
				UART_Printf("[%d], ", aOneNandBadBlock[j]);	
				j++;
			}
			else
				break;
			
			if(j >= ONDT_NUMOFBAD_BUFFER)
				break;			
		}
		UART_Printf("\n\n");
	}
}


const AutotestFuncMenu onenand_erase[] =
{
//	OneNandT_EraseBlock,             				"Erase Block",
	OneNandT_EraseVerify,						"Erase Verify",
	OneNandT_CheckBadBlock,					"Check Bad Block",
	0, 0
};

void OneNandT_EraseTest(void)
{
	u32 i;
	s32 uSel;
	oFunctionT_AutoVar oParameter;
	
	while(1)
	{
		for (i=0; (u32)(onenand_erase[i].desc)!=0; i++)
			UART_Printf("%2d: %s\n", i, onenand_erase[i].desc);

		UART_Printf("\nSelect the function to test : ");
		uSel =UART_GetIntNum();
		UART_Printf("\n");
		if(uSel == -1) 
			break;

		if (uSel>=0 && uSel<(sizeof(onenand_erase)/8-1))
			(onenand_erase[uSel].func) (eTest_Manual, oParameter);
	}
}




///////////////////////////////////////////////////////////////////////////////////
/////////////////////                     Parameter Setting              /////////////////////////// 
///////////////////////////////////////////////////////////////////////////////////

//////////
// Function Name : OneNandT_SetLatency
// Function Description : This function sets the Burst read latency in cycles
// Input : 	None
// Version : 	v0.1
void OneNandT_SetLatency(void)
{
	u32 uLatency;

	UART_Printf("[OneNandT_SetLatency]\n");
	
	UART_Printf("Input the Latency[3~7] : \n");
	uLatency = UART_GetIntNum(); 

	if(uLatency < 3)
	{
		uLatency = 3;
	}
	else if(uLatency > 10)
	{
		uLatency = 10;
	}

	if(uLatency > 7)
	{
		uLatency -= 8;
	}

	ONENAND_SetBurstLatency(g_OneNandContNum, (OneNAND_eLATENCY)uLatency);

}


//////////
// Function Name : OneNandT_SetBurstLength
// Function Description : This function sets the Burst length
// Input : 	None
// Version : 	v0.1
void OneNandT_SetBurstLength(void)
{
	u32 uBurst;

	UART_Printf("[OneNandT_SetBurstLength]\n");
	UART_Printf("Input the Burst : [0:Async, 1:Continuous, 2:Burst4, 3:Burst8, 4:Burst16, 5:Burst32 \n");
	uBurst = UART_GetIntNum(); 

	if(uBurst > 5)
		uBurst = 5;

	ONENAND_SetSyncMode(g_OneNandContNum, (OneNAND_eMODE)uBurst);
}


//////////
// Function Name : OneNandT_SetECCErrorCorrection
// Function Description : This function enables or disables the ECC Error Correction Operation
// Input : 	None
// Version : 	v0.1
void OneNandT_SetECCErrorCorrection(void)
{
	u32 uEccErrCorrect;
	
	UART_Printf("[OneNandT_SetECCErrorCorrection]\n");
	UART_Printf("ECC Error Correction[0:With Correction, 1:Without Correction]\n");
	uEccErrCorrect = UART_GetIntNum(); 

	if(uEccErrCorrect == 0)
		ONENAND_EnableECCCorrection(g_OneNandContNum, ONENAND_WITH_CORRECT);
	else if(uEccErrCorrect == 1)
		ONENAND_EnableECCCorrection(g_OneNandContNum, ONENAND_WITHOUT_CORRECT);
	else
		UART_Printf("Selection fail..retry\n");
}


//////////
// Function Name : OneNandT_SetOperationClock
// Function Description : This function sets the Operating Clock(Flash Clock divided by HCLKx2)
// Input : 	None
// Version : 	v0.1
void OneNandT_SetOperationClock(void)
{
	u32 uSelect;
	float fFlashClcok;
	
	UART_Printf("[OneNandT_SetOperationClock]\n");
	UART_Printf("0:Divide by 1,  1:Divide by 2,  2:Divide by 3, 3:Divide by 4\n");
	UART_Printf("Select : ");
	uSelect = UART_GetIntNum(); 
	
	ONENAND_SetFlashClock(g_OneNandContNum, (OneNAND_eFlashClock)uSelect);
	ONENAND_SetAccClock(g_OneNandContNum);
	
	fFlashClcok = (float)OneNand_Inform[g_OneNandContNum].uFlashClock/(float)1000000;
	
	UART_Printf("OneNand Core Clock : %.04fMHz\n", fFlashClcok);
	UART_Printf("OneNand Interface Clock : %.04fMHz\n", fFlashClcok/2);
	UART_Printf("\n");
	
}


extern volatile u32 OneNand_DmaDone;
extern DMAC g_oONDDmac0;
void OneNandT_SetMemCfgReg(void)
{
	u32 i, uConfig, uCount=0;
	u32 uData1, uData2;
	u32 uSrcAddr, uDstAddr;

	uSrcAddr = 0x10000000;
	uDstAddr = 0x10008000;

	for(i=0 ; i<0x4000 ; i++)
		*(u16 *)(uSrcAddr+2*i) = i;
	for(i=0 ; i<0x4000 ; i++)
		*(u16 *)(uDstAddr+2*i) = 0;
	
	
	*(u32 *)0x70000000 = 0x09;

	DMAC_InitCh(DMA0, DMA_ALL, &g_oONDDmac0);
	INTC_SetVectAddr(NUM_DMA0,  ONENAND_DmaISR);
	INTC_Enable(NUM_DMA0);
	
	while(!UART_GetKey())
	{
		OneNand_DmaDone = 0;
		DMACH_Setup(DMA_A, 0, (u32)uSrcAddr, 0, (u32)uDstAddr, 0, WORD, 0x2000, DEMAND, MEM, MEM, BURST128, &g_oONDDmac0);
		DMACH_Start(&g_oONDDmac0);
		
		uData1 = 0x40E0;
		uData2 = 0xC6E2;
		ONENAND_SetMemCfgTest(g_OneNandContNum, uData1, uData2);
		ONENAND_DirectRead(g_OneNandContNum, OND_SYSCONFIG, &uConfig);
		if((uConfig&0xFFFF) != uData2)
		{
			UART_Printf("1.OND_SYSCONFIG : 0x%04x(Written Data : 0x%04x)\n", uConfig&0xFFFF, uData2);
			UART_Getc();
		}
		while(!OneNand_DmaDone);

		for(i=0 ; i<0x4000 ; i++)
		{
			if(*(u16 *)(uSrcAddr+2*i) != *(u16 *)(uDstAddr+2*i))
			{
				UART_Printf("Data Compare Error : 0x%04x(Source Data : 0x%04x)\n", *(u16 *)(uDstAddr+2*i), *(u16 *)(uSrcAddr+2*i));
				UART_Getc();
			}
		}
		for(i=0 ; i<0x4000 ; i++)
			*(u16 *)(uDstAddr+2*i) = 0;

		
		OneNand_DmaDone = 0;
		DMACH_Setup(DMA_A, 0, (u32)uSrcAddr, 0, (u32)uDstAddr, 0, WORD, 0x2000, DEMAND, MEM, MEM, BURST128, &g_oONDDmac0);
		DMACH_Start(&g_oONDDmac0);
		
		uData1 = 0xC6E0;
		uData2 = 0x46E0;
		ONENAND_SetMemCfgTest(g_OneNandContNum, uData1, uData2);
		ONENAND_DirectRead(g_OneNandContNum, OND_SYSCONFIG, &uConfig);
		if((uConfig&0xFFFF) != uData2)
		{
			UART_Printf("2.OND_SYSCONFIG : 0x%04x(Written Data : 0x%04x)\n", uConfig&0xFFFF, uData2);
			UART_Getc();
		}	
		while(!OneNand_DmaDone);
		for(i=0 ; i<0x4000 ; i++)
		{
			if(*(u16 *)(uSrcAddr+2*i) != *(u16 *)(uDstAddr+2*i))
			{
				UART_Printf("Data Compare Error : 0x%04x(Source Data : 0x%04x)\n", *(u16 *)(uDstAddr+2*i), *(u16 *)(uSrcAddr+2*i));
				UART_Getc();
			}
		}
		for(i=0 ; i<0x4000 ; i++)
			*(u16 *)(uDstAddr+2*i) = 0;

		OneNand_DmaDone = 0;
		DMACH_Setup(DMA_A, 0, (u32)uSrcAddr, 0, (u32)uDstAddr, 0, WORD, 0x2000, DEMAND, MEM, MEM, BURST128, &g_oONDDmac0);
		DMACH_Start(&g_oONDDmac0);
		
		uData1 = 0x40E0;
		uData2 = 0xC6E2;
		ONENAND_SetMemCfgTest(g_OneNandContNum, uData1, uData2);
		ONENAND_DirectRead(g_OneNandContNum, OND_SYSCONFIG, &uConfig);
		if((uConfig&0xFFFF) != uData2)
		{
			UART_Printf("3.OND_SYSCONFIG : 0x%04x(Written Data : 0x%04x)\n", uConfig&0xFFFF, uData2);
			UART_Getc();
		}
		while(!OneNand_DmaDone);
		for(i=0 ; i<0x4000 ; i++)
		{
			if(*(u16 *)(uSrcAddr+2*i) != *(u16 *)(uDstAddr+2*i))
			{
				UART_Printf("Data Compare Error : 0x%04x(Source Data : 0x%04x)\n", *(u16 *)(uDstAddr+2*i), *(u16 *)(uSrcAddr+2*i));
				UART_Getc();
			}
		}
		for(i=0 ; i<0x4000 ; i++)
			*(u16 *)(uDstAddr+2*i) = 0;

		OneNand_DmaDone = 0;
		DMACH_Setup(DMA_A, 0, (u32)uSrcAddr, 0, (u32)uDstAddr, 0, WORD, 0x2000, DEMAND, MEM, MEM, BURST128, &g_oONDDmac0);
		DMACH_Start(&g_oONDDmac0);
	
		uData1 = 0x40E0;
		uData2 = 0xC6E0;
		ONENAND_SetMemCfgTest(g_OneNandContNum, uData1, uData2);
		ONENAND_DirectRead(g_OneNandContNum, OND_SYSCONFIG, &uConfig);
		if((uConfig&0xFFFF) != uData2)
		{
			UART_Printf("4.OND_SYSCONFIG : 0x%04x(Written Data : 0x%04x)\n", uConfig&0xFFFF, uData2);
			UART_Getc();
		}		
		while(!OneNand_DmaDone);
		for(i=0 ; i<0x4000 ; i++)
		{
			if(*(u16 *)(uSrcAddr+2*i) != *(u16 *)(uDstAddr+2*i))
			{
				UART_Printf("Data Compare Error : 0x%04x(Source Data : 0x%04x)\n", *(u16 *)(uDstAddr+2*i), *(u16 *)(uSrcAddr+2*i));
				UART_Getc();
			}
		}
		for(i=0 ; i<0x4000 ; i++)
			*(u16 *)(uDstAddr+2*i) = 0;
	
		uCount++;
		if(!(uCount % 10))
			UART_Printf("Count : %d\n", uCount);

	}
}



const testFuncMenu onenand_param[] =
{
	OneNandT_SetLatency,             					"Latency Setting",
	OneNandT_SetBurstLength,						"Burst Length",
	OneNandT_SetECCErrorCorrection,				"ECC Error Correction",
	OneNandT_SetOperationClock,					"Flash Clock",
	OneNandT_SetMemCfgReg,							"MemCfg Register Write",
	
	0, 0
};

void OneNandT_SetParameter(void)
{
	u32 i;
	s32 uSel;
	
	while(1)
	{
		for (i=0; (u32)(onenand_param[i].desc)!=0; i++)
			UART_Printf("%2d: %s\n", i, onenand_param[i].desc);

		UART_Printf("\nSelect the function to test : ");
		uSel =UART_GetIntNum();
		UART_Printf("\n");
		if(uSel == -1) 
			break;

		if (uSel>=0 && uSel<(sizeof(onenand_param)/8-1))
			(onenand_param[uSel].func) ();
	}
}




///////////////////////////////////////////////////////////////////////////////////
////////////////////                     Copy-Back   Test                /////////////////////////// 
///////////////////////////////////////////////////////////////////////////////////

//////////
// Function Name : OneNandT_CopyBackTest
// Function Description : Memory Copy Back Test
// Input : 	None
// Version : v0.1
void OneNandT_CopyBackTest(void)
{
	u8 uError;
	u32 i, uSourceBlkAddr, uVerifyBlock, uPageSize, uReadPageCnt; 
	u8 uSourcePageAddr, uVerifyPage;
	u32 uDestinationBlkAddr;
	u8 uDestinationPageAddr;
	u32 *aWriteData, *aReadData;

	aWriteData = (u32 *)malloc(ONENAND_PAGESIZE);
	aReadData = (u32 *)malloc(ONENAND_PAGESIZE);

	UART_Printf("[OneNandT_CopyBackTest]\n");
	UART_Printf("Input the Source block number to Copy-back[0~%d] : \n", OneNand_Inform[g_OneNandContNum].uNumOfBlock-1);
	uSourceBlkAddr = UART_GetIntNum(); 
	UART_Printf("Input the Source block page to Copy-back[0~%d] : \n", OneNand_Inform[g_OneNandContNum].uNumOfPage-1);
	uSourcePageAddr = (u8)UART_GetIntNum();
	UART_Printf("Input the page size to Copy-back[0~0xFF] : \n");
	uPageSize = UART_GetIntNum();

	UART_Printf("Input the Destination block number to Copy-back[0~%d] : \n", OneNand_Inform[g_OneNandContNum].uNumOfBlock-1);
	uDestinationBlkAddr = UART_GetIntNum(); 
	UART_Printf("Input the Destination block page to Copy-back[0~%d] : \n", OneNand_Inform[g_OneNandContNum].uNumOfPage-1);
	uDestinationPageAddr = (u8)UART_GetIntNum();	

	UART_Printf("1. Copy-Back\n");
	uError = FALSE;
	ONENAND_CopyBack(g_OneNandContNum, uSourceBlkAddr, uSourcePageAddr, uDestinationBlkAddr, uDestinationPageAddr, uPageSize);

	//rb1004 : must be added...Compare data
	UART_Printf("2. Read & Verify\n");
	uReadPageCnt = 0;
	while(uReadPageCnt < uPageSize)
	{
		//Source data read
		uVerifyBlock = uSourceBlkAddr + ((u32)uSourcePageAddr + uReadPageCnt)/OneNand_Inform[g_OneNandContNum].uNumOfPage;
		uVerifyPage = (uSourcePageAddr + (u8)uReadPageCnt)%(u8)OneNand_Inform[g_OneNandContNum].uNumOfPage;
		ONENAND_ReadPage(g_OneNandContNum, uVerifyBlock, uVerifyPage, (u32*)aWriteData);
	
		//Destination data read
		uVerifyBlock = uDestinationBlkAddr + ((u32)uDestinationPageAddr + uReadPageCnt)/OneNand_Inform[g_OneNandContNum].uNumOfPage;
		uVerifyPage = (uDestinationPageAddr + (u8)uReadPageCnt)%(u8)OneNand_Inform[g_OneNandContNum].uNumOfPage;
		ONENAND_ReadPage(g_OneNandContNum, uVerifyBlock, uVerifyPage, (u32*)aReadData);
		
		for(i=0 ; i<ONENAND_PAGESIZE/4 ; i++)
		{
			if(aReadData[i] != aWriteData[i])
			{
				UART_Printf("Write&Read Verify Error [%d Block : %d Page(Destination Address)] :  \n", uVerifyBlock, uVerifyPage);
				UART_Printf("\t [Write data : Read data] = [0x%08x  : 0x%08x ]  \n", aWriteData[i], aReadData[i]);
				uError = TRUE;
				break;
			}		
		}
	
		uReadPageCnt++;

		for(i=0; i<ONENAND_PAGESIZE/4; i++)
		{
			aReadData[i] = 0;
			aWriteData[i] = 0;
		}			
	}
	
	if (uError==TRUE) 
	{
		UART_Printf("\nOneNAND Copy-back FAILED!!!\n");	
		UART_Printf("\n");
	}
	else if (uError==FALSE) 
	{
		UART_Printf("\n");
		UART_Printf("Write&Read Verify OK\n");
		UART_Printf("OneNAND Copy-back SUCCESS\n");
		UART_Printf("\n");
	}
	free(aWriteData);
	free(aReadData);	
}



///////////////////////////////////////////////////////////////////////////////////
////////////////////                  OneNand Program Test              ///////////////////////// 
///////////////////////////////////////////////////////////////////////////////////

//////////
// Function Name : OneNandT_MemoryWriteRead
// Function Description : Memory Write & Read(Verify) Test
// Input : 	eTest - Manual test or Auto test selection
//			oAutoVar - Parameters for Auto-Test
// Version : v0.1
void OneNandT_MemoryWriteRead(eFunction_Test eTest, oFunctionT_AutoVar oAutoVar)
{
	u8 uError;
	u32 uStartBlkAddr, uPageSize; 
	u32 uBlkAddr, uWritePageCnt, uReadPageCnt, i;	
	u8 uStartPageAddr, uPageAddr;
	//u32 aWriteData[ONENAND_PAGESIZE/4] = {0, };
	//u32 aReadData[ONENAND_PAGESIZE/4] = {0, };
	u32 *aWriteData, *aReadData;

	uError = FALSE;

	aWriteData = (u32 *)malloc(ONENAND_PAGESIZE);
	aReadData = (u32 *)malloc(ONENAND_PAGESIZE);

	if((aWriteData == 0) || (aReadData == 0))
	{
		UART_Printf("Memory Allocation Error...\n");
		return;
	}
	
	if(eTest == eTest_Manual)
	{
		UART_Printf("[OneNandT_MemoryWriteRead]\n");

		UART_Printf("Input the Start block number to write[0~%d] : \n", OneNand_Inform[g_OneNandContNum].uNumOfBlock-1);
		uStartBlkAddr = UART_GetIntNum(); 
		UART_Printf("Input the Start page number to write[0~%d] : \n", OneNand_Inform[g_OneNandContNum].uNumOfPage-1);
		uStartPageAddr = (u8)UART_GetIntNum();
		UART_Printf("Input the page size to write : \n");
		uPageSize = UART_GetIntNum();
	}
	else
	{
		uStartBlkAddr = oAutoVar.Test_Parameter[0];
		uStartPageAddr = oAutoVar.Test_Parameter[1];
		uPageSize = oAutoVar.Test_Parameter[2];	
	}
	
	//Data Write
	if(eTest == eTest_Manual)
		UART_Printf("1. Memory Write\n");
	uWritePageCnt = 0;
	uBlkAddr = uStartBlkAddr;
	uPageAddr = uStartPageAddr;
	while(uWritePageCnt < uPageSize)
	{
#if 1
	#if(ONDT_DATA_PATTERN == ONDT_DATA_RANDOM)
		srand((uBlkAddr<<20)|(uPageAddr<<12));
	#endif
	
		for(i=0; i<ONENAND_PAGESIZE/4; i++)
		{
	#if(ONDT_DATA_PATTERN == ONDT_DATA_RANDOM)
			aWriteData[i] = rand();
	#elif (ONDT_DATA_PATTERN == ONDT_DATA_BLOCK_PAGE)
			aWriteData[i] = i +  (uPageAddr<<12) + (uBlkAddr<<20);
	#else	// ONDT_DATA_PATTERN == ONDT_DATA_ONEVALUE
			aWriteData[i] = ONDT_DATA_ONEVALUE;
	#endif
		}	
#endif

		ONENAND_WritePage(g_OneNandContNum, uBlkAddr, uPageAddr, (u32*)aWriteData);
		uPageAddr++;
		if(uPageAddr == OneNand_Inform[g_OneNandContNum].uNumOfPage)
		{
			uPageAddr = 0;
			uBlkAddr++;
		}
		uWritePageCnt++;
	}

#if 1
	// Data Read
	if(eTest == eTest_Manual)
		UART_Printf("2. Memory Read & Verify\n");
	uReadPageCnt = 0;
	uBlkAddr = uStartBlkAddr;
	uPageAddr = uStartPageAddr;	
	while(uReadPageCnt < uPageSize)
	{
		ONENAND_ReadPage(g_OneNandContNum, uBlkAddr, uPageAddr, (u32*)aReadData);
#if 1
	#if(ONDT_DATA_PATTERN == ONDT_DATA_RANDOM)
		srand((uBlkAddr<<20)|(uPageAddr<<12));
	#endif
	
		for(i=0; i<ONENAND_PAGESIZE/4; i++)
		{
	#if(ONDT_DATA_PATTERN == ONDT_DATA_RANDOM)
			aWriteData[i] = rand();
	#elif (ONDT_DATA_PATTERN == ONDT_DATA_BLOCK_PAGE)
			aWriteData[i] = i +  (uPageAddr<<12) + (uBlkAddr<<20);
	#else	// ONDT_DATA_PATTERN == ONDT_DATA_ONEVALUE
			aWriteData[i] = ONDT_DATA_ONEVALUE;
	#endif
		}
#endif

		for(i=0 ; i<ONENAND_PAGESIZE/4 ; i++)
		{
			if(aReadData[i] != aWriteData[i])
			{
				UART_Printf("Write&Read Verify Error [%d Block : %d Page] :  \n", uBlkAddr, uPageAddr);
				UART_Printf("\t [Write data : Read data] = [0x%08x  : 0x%08x ]  \n", aWriteData[i], aReadData[i]);
				UART_Getc();
				uError = TRUE;
				break;
			}
		}

		uPageAddr++;
		if(uPageAddr == OneNand_Inform[g_OneNandContNum].uNumOfPage)
		{
			uPageAddr = 0;
			uBlkAddr++;
		}
		uReadPageCnt++;

		for(i=0; i<ONENAND_PAGESIZE/4; i++)
			aReadData[i] = 0; 
		
		if(eTest == eTest_Auto)
		{
			UART_Printf(".");
			if(!(uReadPageCnt%10))
				UART_Printf("\b\b\b\b\b\b\b\b\b\b");
		}
			
	}
#endif

	if(eTest == eTest_Manual)
	{
		if(uError == FALSE)
			UART_Printf("Write&Read Verify OK\n");
		UART_Printf("\n");
	}

	free(aWriteData);
	free(aReadData);
}

void OneNandT_MemoryWriteRead_Test(eFunction_Test eTest, oFunctionT_AutoVar oAutoVar)
{
	u8 uError;
	u32 uStartBlkAddr, uPageSize; 
	u32 uBlkAddr, uWritePageCnt, i;	
	u8 uStartPageAddr, uPageAddr;
	u32 *aWriteData, *aReadData;
	u32 uWriteValue;
	
	uError = FALSE;

	aWriteData = (u32 *)malloc(ONENAND_PAGESIZE);
	aReadData = (u32 *)malloc(ONENAND_PAGESIZE);

	if((aWriteData == 0) || (aReadData == 0))
	{
		UART_Printf("Memory Allocation Error...\n");
		return;
	}
	
	if(eTest == eTest_Manual)
	{
		UART_Printf("[OneNandT_MemoryWriteRead]\n");

		UART_Printf("Input the Start block number to write[0~%d] : \n", OneNand_Inform[g_OneNandContNum].uNumOfBlock-1);
		uStartBlkAddr = UART_GetIntNum(); 
		UART_Printf("Input the Start page number to write[0~%d] : \n", OneNand_Inform[g_OneNandContNum].uNumOfPage-1);
		uStartPageAddr = (u8)UART_GetIntNum();
		UART_Printf("Input the page size to write : \n");
		uPageSize = UART_GetIntNum();
		UART_Printf("Input the value to write : \n");
		uWriteValue = UART_GetIntNum();
	}
	else
	{
		uStartBlkAddr = oAutoVar.Test_Parameter[0];
		uStartPageAddr = oAutoVar.Test_Parameter[1];
		uPageSize = oAutoVar.Test_Parameter[2];
		uWriteValue = oAutoVar.Test_Parameter[3];
	}
	
	//Data Write
	if(eTest == eTest_Manual)
		UART_Printf("1. Memory Write\n");
	
	uWritePageCnt = 0;
	uBlkAddr = uStartBlkAddr;
	uPageAddr = uStartPageAddr;
	while(uWritePageCnt < uPageSize)
	{
#if 1
	#if(ONDT_DATA_PATTERN == ONDT_DATA_RANDOM)
		srand((uBlkAddr<<20)|(uPageAddr<<12));
	#endif
	
		for(i=0; i<ONENAND_PAGESIZE/4; i++)
		{
	#if(ONDT_DATA_PATTERN == ONDT_DATA_RANDOM)
			aWriteData[i] = rand();
	#elif (ONDT_DATA_PATTERN == ONDT_DATA_BLOCK_PAGE)
			aWriteData[i] = i +  (uPageAddr<<12) + (uBlkAddr<<20);
	#else	// ONDT_DATA_PATTERN == ONDT_DATA_ONEVALUE
			aWriteData[i] = ONDT_DATA_ONEVALUE;
	#endif
		}
#endif

		ONENAND_WritePage(g_OneNandContNum, uBlkAddr, uPageAddr, (u32*)aWriteData);

///////////////////////////////////////
///////////////////////////////////////
#if 0
		ONENAND_ReadPage(g_OneNandContNum, uBlkAddr, uPageAddr, (u32*)aReadData);
#if 0
		for(i=0; i<ONENAND_PAGESIZE/4; i++)
			aWriteData[i] = i +  (uPageAddr<<12) + (uBlkAddr<<20);
			//aWriteData[i] = uWriteValue;
#endif

		for(i=0 ; i<ONENAND_PAGESIZE/4 ; i++)
		{
			if(aReadData[i] != aWriteData[i])
			{
				UART_Printf("Write&Read Verify Error [%d Block : %d Page] :  \n", uBlkAddr, uPageAddr);
				UART_Printf("\t [Write data : Read data] = [0x%08x  : 0x%08x ]  \n", aWriteData[i], aReadData[i]);
				UART_Getc();
				uError = TRUE;
				break;
			}
		}
#elif 1
		{
			u32 *OneNand_DataRAM;
			u32 uData;
			
			if(g_OneNandContNum == ONENAND0)
				OneNand_DataRAM = (u32 *)(0x20000000 + ONENAND_DATARAM0_BASE);
			else
				OneNand_DataRAM = (u32 *)(0x28000000 + ONENAND_DATARAM0_BASE);
			
			//for(i=0; i<ONENAND_PAGESIZE/4; i++)
			//{
				//aWriteData[i] = i +  (uPageAddr<<12) + (uBlkAddr<<20);
				//aWriteData[i] = 0x1f001f00|(uPageAddr<<16)|uPageAddr;
				//aWriteData[i] = uWriteValue;
			//	srand(i);
			//	aWriteData[i] = rand();
			//}
			
			for(i=0 ; i<ONENAND_PAGESIZE/4 ; i++)
			{
				uData = *(OneNand_DataRAM+i);
				if(uData  != aWriteData[i])
				{
					UART_Printf("Write&Read Verify Error [%d Block : %d Page : %dth word] :  \n", uBlkAddr, uPageAddr, i);
					UART_Printf("\t [Write data : Read data] = [0x%08x  : 0x%08x ]\n", aWriteData[i], uData);
					UART_Getc();
					uError = TRUE;
					break;
				}
			}
		}

#endif		
//////////////////////////////////////////
//////////////////////////////////////////		
				
		uPageAddr++;
		if(uPageAddr == OneNand_Inform[g_OneNandContNum].uNumOfPage)
		{
			uPageAddr = 0;
			uBlkAddr++;
		}
		uWritePageCnt++;
	}

#if 0
	// Data Read
	if(eTest == eTest_Manual)
		UART_Printf("2. Memory Read & Verify\n");
	uReadPageCnt = 0;
	uBlkAddr = uStartBlkAddr;
	uPageAddr = uStartPageAddr;	
	while(uReadPageCnt < uPageSize)
	{
		ONENAND_ReadPage(g_OneNandContNum, uBlkAddr, uPageAddr, (u32*)aReadData);
#if 1
	#if(ONDT_DATA_PATTERN == ONDT_DATA_RANDOM)
		srand((uBlkAddr<<20)|(uPageAddr<<12));
	#endif
	
		for(i=0; i<ONENAND_PAGESIZE/4; i++)
		{
	#if(ONDT_DATA_PATTERN == ONDT_DATA_RANDOM)
			aWriteData[i] = rand();
	#elif (ONDT_DATA_PATTERN == ONDT_DATA_BLOCK_PAGE)
			aWriteData[i] = i +  (uPageAddr<<12) + (uBlkAddr<<20);
	#else	// ONDT_DATA_PATTERN == ONDT_DATA_ONEVALUE
			aWriteData[i] = ONDT_DATA_ONEVALUE;
	#endif
		}
#endif

		for(i=0 ; i<ONENAND_PAGESIZE/4 ; i++)
		{
			if(aReadData[i] != aWriteData[i])
			{
				UART_Printf("Write&Read Verify Error [%d Block : %d Page] :  \n", uBlkAddr, uPageAddr);
				UART_Printf("\t [Write data : Read data] = [0x%08x  : 0x%08x ]  \n", aWriteData[i], aReadData[i]);
				UART_Getc();
				uError = TRUE;
				break;
			}
		}

		uPageAddr++;
		if(uPageAddr == OneNand_Inform[g_OneNandContNum].uNumOfPage)
		{
			uPageAddr = 0;
			uBlkAddr++;
		}
		uReadPageCnt++;

		for(i=0; i<ONENAND_PAGESIZE/4; i++)
			aReadData[i] = 0; 
		
		if(eTest == eTest_Auto)
		{
			UART_Printf(".");
			if(!(uReadPageCnt%10))
				UART_Printf("\b\b\b\b\b\b\b\b\b\b");
		}
			
	}
#endif

	if(eTest == eTest_Manual)
	{
		if(uError == FALSE)
			UART_Printf("Write&Read Verify OK\n");
		UART_Printf("\n");
	}

	free(aWriteData);
	free(aReadData);
}



//////////
// Function Name : OneNandT_MemoryRead
// Function Description : Memory Read & Display Test
// Input : 	eTest - Manual test or Auto test selection
//			oAutoVar - Parameters for Auto-Test
// Version : v0.1
void OneNandT_MemoryRead(eFunction_Test eTest, oFunctionT_AutoVar oAutoVar)
{
	u32 uStartBlkAddr, uPageSize; 
	u32 uBlkAddr, uReadPageCnt, i;	
	u8 uStartPageAddr, uPageAddr;
	//u32 aReadData[ONENAND_PAGESIZE/4] = {0, };
	u32 *aReadData;

	//aReadData = (u32 *)0x0c000000;
	if(eTest == eTest_Manual)
	{	
		UART_Printf("[OneNandT_MemoryRead]\n");
		
		UART_Printf("Input the Start block number to read[0~%d] : \n", OneNand_Inform[g_OneNandContNum].uNumOfBlock-1);
		uStartBlkAddr = UART_GetIntNum(); 
		UART_Printf("Input the Start page number to read[0~%d] : \n", OneNand_Inform[g_OneNandContNum].uNumOfPage-1);
		uStartPageAddr = (u8)UART_GetIntNum();
		UART_Printf("Input the page size to read : \n");
		uPageSize = UART_GetIntNum();
	}
	else
	{
		uStartBlkAddr = oAutoVar.Test_Parameter[0];
		uStartPageAddr= oAutoVar.Test_Parameter[1];
		uPageSize = oAutoVar.Test_Parameter[2];
	}
	
	aReadData = (u32 *)malloc(ONENAND_PAGESIZE);

	if(aReadData == 0)
	{
		UART_Printf("Memory Allocation Error...\n");
		return;
	}
	
	// Data Read
	uReadPageCnt = 0;
	uBlkAddr = uStartBlkAddr;
	uPageAddr = uStartPageAddr;	
	while(uReadPageCnt < uPageSize)
	{
		ONENAND_ReadPage(g_OneNandContNum, uBlkAddr, uPageAddr, (u32*)aReadData);

#if 1
		if(eTest == eTest_Manual)
		{	
			UART_Printf("\n");
			UART_Printf("[%d block, %d page]", uBlkAddr, uPageAddr);
			for(i=0 ; i<ONENAND_PAGESIZE/4 ; i++)
			{
				if(!(i%4))
				{
					UART_Printf("\n");
					UART_Printf("0x%04x : ",i*4);
				}
				UART_Printf("0x%08x  ", aReadData[i]);
			}
			UART_Printf("\n");
		}
#endif		
		uPageAddr++;
		if(uPageAddr == OneNand_Inform[g_OneNandContNum].uNumOfPage)
		{
			uPageAddr = 0;
			uBlkAddr++;
		}
		uReadPageCnt++;

		for(i=0; i<ONENAND_PAGESIZE/4; i++)
			aReadData[i] = 0;
	}

	free(aReadData);	
}


//////////
// Function Name : OneNandT_ReadModifyWrite
// Function Description : Memory Read/Modify/Write Test
// Input : 	eTest - Manual test or Auto test selection
//			oAutoVar - Parameters for Auto-Test
// Version : v0.1
void OneNandT_ReadModifyWrite(eFunction_Test eTest, oFunctionT_AutoVar oAutoVar)
{
	u32 uBlkAddr;
	u32 uModifysize, i;
	//u32 aWriteData[ONENAND_PAGESIZE/4] = {0, };
	//u32 aReadData[ONENAND_PAGESIZE/4] = {0, };
	u32 *aWriteData, *aReadData;
	u8 uPageAddr;
	u8 uError;

	uError = FALSE;

	aWriteData = (u32 *)malloc(ONENAND_PAGESIZE);
	aReadData = (u32 *)malloc(ONENAND_PAGESIZE);

	if((aWriteData == 0) || (aReadData == 0))
	{
		UART_Printf("Memory Allocation Error...\n");
		return;
	}
	
	if(eTest == eTest_Manual)
	{
		UART_Printf("[OneNandT_ReadModifyWrite]\n");
		
		UART_Printf("Input the block number to modify[0~%d] : \n", OneNand_Inform[g_OneNandContNum].uNumOfBlock-1);
		uBlkAddr = UART_GetIntNum(); 
		UART_Printf("Input the page number to modify[0~%d] : \n", OneNand_Inform[g_OneNandContNum].uNumOfPage-1);
		uPageAddr = (u8)UART_GetIntNum();

	//	ONENAND_EraseBlock(g_OneNandContNum, uBlkAddr, uBlkAddr);

		ONENAND_EnableInterrupt(g_OneNandContNum, eOND_INTACT);
	}
	else
	{
		uBlkAddr = oAutoVar.Test_Parameter[0];
		uPageAddr = oAutoVar.Test_Parameter[1];
	}
	
	//Load the Page to map 00 XIP buffer
	if(eTest == eTest_Manual)
		UART_Printf("1. Load to XIP Buffer\n");
	ONENAND_LoadToXIPBuffer(g_OneNandContNum, uBlkAddr, uPageAddr);

//rb1004...for dma test	
#if 0
	{
		u32 uSrcAddr;
		
		OneNand_DmaDone = 0;
		uSrcAddr = 0x20000000;
		DMACH_Setup(DMA_A, 0x0, uSrcAddr, 0, (u32)aReadData, 0, WORD, ONENAND_PAGESIZE/4, DEMAND, MEM, MEM, BURST16, &g_oONDDmac0);
		DMACH_Start(&g_oONDDmac0);

		while(!OneNand_DmaDone);	
	}
#endif

	//Modify data
	if(eTest == eTest_Manual)
		UART_Printf("2. Modify the XIP Buffer\n");
	uModifysize = 0x400/4;		// 1K Modify(0x0000 ~ 0x0400)
	for(i=0 ; i<uModifysize ; i++)
		ONENAND_WriteIntoDataram(g_OneNandContNum, 4*i, i+1);		

	for(i=0 ; i<ONENAND_PAGESIZE/4 ; i++)
		ONENAND_ReadOutDataram(g_OneNandContNum, 4*i, &aWriteData[i]);		//Read/Modify/Write command -> 0x00 Address:DataRam0
	
	//Wrtie the data in the map 00 XIP buffer
	if(eTest == eTest_Manual)
		UART_Printf("3. Write from XIP Buffer to Flash Memory\n");
	ONENAND_WriteFromXIPBuffer(g_OneNandContNum, uBlkAddr, uPageAddr);

	//Data Verify
#if 0
	ONENAND_WriteCmd(g_OneNandContNum, uBlkAddr, uPageAddr, 0x10);
	for(i=0 ; i<ONENAND_PAGESIZE/4 ; i++)
		ONENAND_ReadOutDataram(g_OneNandContNum, 4*i, &aReadData[i]);
	ONENAND_WriteCmd(g_OneNandContNum, uBlkAddr, uPageAddr, 0x11);
#else	
	if(eTest == eTest_Manual)
		UART_Printf("4. Verify the Flash Memory\n");
	ONENAND_ReadPage(g_OneNandContNum, uBlkAddr, uPageAddr, (u32*)aReadData);
#endif

	for(i=0 ; i<ONENAND_PAGESIZE/4 ; i++)
	{
		if(aReadData[i] != aWriteData[i])
		{
			UART_Printf("Write&Read Verify Error [%d Block : %d Page] :  \n", uBlkAddr, uPageAddr);
			UART_Printf("\t [Write data : Read data] = [0x%08x  : 0x%08x ] :  \n", aWriteData[i], aReadData[i]);
			uError = TRUE;
			break;
		}
	}

	if(uError == FALSE)
	{
		if(eTest == eTest_Manual)
			UART_Printf("Read/Modify/Write Operation OK \n");	
	}

	if(eTest == eTest_Manual)
		UART_Printf("\n");	

	free(aWriteData);
	free(aReadData);
}


//////////
// Function Name : OneNandT_SinglePipelineRead
// Function Description : Single pipelined reads followed by data read out
// Input : 	eTest - Manual test or Auto test selection
//			oAutoVar - Parameters for Auto-Test
// Version : v0.1
void OneNandT_SinglePipelineRead(eFunction_Test eTest, oFunctionT_AutoVar oAutoVar)
{
	u32 i, j, uStartBlkAddr, uReadPageCnt, uBlkAddr;
	u8 uStartPageAddr, uPageAddr, uPageSize; 
	//u32 aReadData[ONENAND_PAGESIZE/4] = {0, };
	u32 *pReadData, *pReadDataBase;

	if(eTest == eTest_Manual)
	{
		UART_Printf("[OneNandT_SinglePipelineRead]\n");

		UART_Printf("Input the Start block number to read[0~%d] : \n", OneNand_Inform[g_OneNandContNum].uNumOfBlock-1);
		uStartBlkAddr = UART_GetIntNum(); 
		UART_Printf("Input the Start page number to read[0~%d] : \n", OneNand_Inform[g_OneNandContNum].uNumOfPage-1);
		uStartPageAddr = (u8)UART_GetIntNum();
		UART_Printf("Input the page size to read : \n");
		uPageSize = (u8)UART_GetIntNum();
	}
	else
	{
		uStartBlkAddr = oAutoVar.Test_Parameter[0];
		uStartPageAddr = oAutoVar.Test_Parameter[1];
		uPageSize = oAutoVar.Test_Parameter[2];
	}
	
	pReadDataBase = (u32 *)malloc(ONENAND_PAGESIZE*uPageSize);
	if(pReadDataBase == 0)
	{
		UART_Printf("Memory allocation Error...\n");
		return;
	}
	else
	{
		pReadData = pReadDataBase;

		for(i=0 ; i<(ONENAND_PAGESIZE*uPageSize)/4 ; i++)
			*pReadData++ = 0;
	}

	pReadData = pReadDataBase;
#if 0
	m=0;
	while(!UART_GetKey())
	{
	m++;

	pReadData = pReadDataBase;

	for(i=0 ; i<(ONENAND_PAGESIZE*uPageSize)/4 ; i++)
			*pReadData++ = 0;

	pReadData = pReadDataBase;
	
//	if(m%2)
//		ONENAND_EnableECCCorrection(g_OneNandContNum, ONENAND_WITH_CORRECT);
//	else
//		ONENAND_EnableECCCorrection(g_OneNandContNum, ONENAND_WITHOUT_CORRECT);

#endif

#if (ONENAND_ACCESS_MODE == ONENAND_COMMAND)	
	ONENAND_PipelineReadAhead(g_OneNandContNum, uStartBlkAddr, uStartPageAddr, uPageSize);

	uReadPageCnt = 0;
	uBlkAddr = uStartBlkAddr;
	uPageAddr = uStartPageAddr;	
	OneNandT_oIntFlag.IntActInt_Count = 0;
	
	while(uReadPageCnt < uPageSize)
	{
		ONENAND_ReadPageForCacheRead(g_OneNandContNum, uBlkAddr, uPageAddr, (u32*)pReadData, uPageSize);
		pReadData += ONENAND_PAGESIZE/4;

		//Page Address증가 확인 필요???
		uPageAddr++;
		if(uPageAddr == OneNand_Inform[g_OneNandContNum].uNumOfPage)
		{
			uPageAddr = 0;
			uBlkAddr++;
		}
		uReadPageCnt++;

		//for(i=0; i<ONENAND_PAGESIZE/4; i++)
		//	aReadData[i] = 0;
	}	
#else
	ONENAND_PipelineReadAheadDirectAcc(g_OneNandContNum, uStartBlkAddr, uStartPageAddr, uPageSize, (u32*)pReadData);
#endif	

#if 0
	pReadData = pReadDataBase;
	for(j=0 ; j<uPageSize ; j++)
	{
		uPageAddr = (uStartPageAddr+i)%64;
		uBlkAddr = uStartBlkAddr + (uStartPageAddr+i)/64;
		UART_Printf("\n");
		UART_Printf("[%d block, %d page]", uBlkAddr, uPageAddr);
		for(i=0 ; i<ONENAND_PAGESIZE/4 ; i++)
		{
			if(!(i%4))
			{
				UART_Printf("\n");
				UART_Printf("0x%04x : ",i*4);
			}
			UART_Printf("0x%08x  ", *pReadData++);
		}
		UART_Printf("\n");
	}
#else
	{
		u32 aWriteData[ONENAND_PAGESIZE/4] = {0, };
		u32 uError = FALSE;
		u32 k;

		uBlkAddr = uStartBlkAddr;
		uPageAddr = uStartPageAddr;
		pReadData = pReadDataBase;
	
		for(i=0 ; i<uPageSize ; i++)
		{
	#if(ONDT_DATA_PATTERN == ONDT_DATA_RANDOM)
			srand((uBlkAddr<<20)|(uPageAddr<<12));
	#endif
		
			for(k=0; k<ONENAND_PAGESIZE/4; k++)
			{
	#if(ONDT_DATA_PATTERN == ONDT_DATA_RANDOM)
				aWriteData[k] = rand();
	#elif (ONDT_DATA_PATTERN == ONDT_DATA_BLOCK_PAGE)
				aWriteData[k] = k +  (uPageAddr<<12) + (uBlkAddr<<20);
	#else	// ONDT_DATA_PATTERN == ONDT_DATA_ONEVALUE
				aWriteData[k] = ONDT_DATA_ONEVALUE;
	#endif
			}
				
			for(j=0 ; j<ONENAND_PAGESIZE/4 ; j++)
			{
				if(aWriteData[j] != *(pReadData+j))
				{
					UART_Printf("[Write data : Read data] = [0x%08x :  0x%08x] \n", aWriteData[j], *(pReadData+j));
					UART_Getc();
					uError = TRUE;
					break;
				}
			}
			uPageAddr++;
			if(uPageAddr == OneNand_Inform[g_OneNandContNum].uNumOfPage)
			{
				uPageAddr = 0;
				uBlkAddr++;
			}
			pReadData += ONENAND_PAGESIZE/4;
		}

		if(uError == FALSE)
		{
			if(eTest == eTest_Manual)
				UART_Printf("Cache Read Verify OK\n");
		}
	}
	if(eTest == eTest_Manual)
		UART_Printf("\n");
#endif
//	}
	
	free(pReadDataBase);
}


extern volatile u32 MemToMem_DmaDone;
extern volatile u32 MemToMem_DmaDone1;

void __irq Isr_TIMER0_OneNand(void)
{
	u32 uTmp;
	
	uTmp = Inp32(rTINT_CSTAT);
	uTmp = (uTmp & 0x1f)| (1<<5);
	Outp32(rTINT_CSTAT,uTmp);       //  Timer0 Interrupt Clear register

#if 0    
    	if(MemToMem_DmaDone== 1)
    	{
		MemToMem_DmaDone = 0;
		DMACH_Setup((DMA_CH)DMA_C, 0x0, (u32)0x52000000, 0, (u32)0x53000000, 0, (DATA_SIZE)WORD, 0x1000000/4, DEMAND, MEM, MEM, (BURST_MODE)BURST32, &g_oONDDmac0);
		DMACH_Start(&g_oONDDmac0);
    	}
#else		
	UART_Printf("T");
  	DelayfrTimer(milli, 11);
#endif

   	 INTC_ClearVectAddr();
}

void __irq Isr_TIMER1_OneNand(void)
{
	u32 uTmp;
	
	uTmp = Inp32(rTINT_CSTAT);
	uTmp = (uTmp & 0x1f)| (1<<6);
	Outp32(rTINT_CSTAT,uTmp);       //  Timer0 Interrupt Clear register
	
     	if(MemToMem_DmaDone1== 1)
    	{
	    	//UART_Printf("Q");
		MemToMem_DmaDone1 = 0;   
		DMACH_Setup((DMA_CH)DMA_B, 0x0, (u32)0x50000000, 0, (u32)0x51000000, 0, (DATA_SIZE)WORD, 0x1000000/4, DEMAND, MEM, MEM, (BURST_MODE)BURST4, &g_oONDDmac0);
		DMACH_Start(&g_oONDDmac0);
     	}
		
   	 INTC_ClearVectAddr();
}



//////////
// Function Name : OneNandT_SinglePipelineReadWithSpare
// Function Description : Single pipelined reads followed by data read out
// Input : 	eTest - Manual test or Auto test selection
//			oAutoVar - Parameters for Auto-Test
// Version : v0.1

void OneNandT_SinglePipelineReadWithSpare(eFunction_Test eTest, oFunctionT_AutoVar oAutoVar)
{
	u32 i, j, m, uStartBlkAddr, uReadPageCnt, uBlkAddr;
	u8 uStartPageAddr, uPageAddr, uPageSize; 
	u32 *pReadData, *pReadDataBase;
	u32 aSpareReadData[ONENAND_SPARESIZE/4] = {0, };
	//u32 uCacheReadSpare_Fail=0;
//	u32 uPageSizeTemp;

	if(eTest == eTest_Manual)
	{
		UART_Printf("[OneNandT_SinglePipelineReadWithSpare]\n");

		UART_Printf("Input the Start block number to read[0~%d] : \n", OneNand_Inform[g_OneNandContNum].uNumOfBlock-1);
		uStartBlkAddr = UART_GetIntNum(); 
		UART_Printf("Input the Start page number to read[0~%d] : \n", OneNand_Inform[g_OneNandContNum].uNumOfPage-1);
		uStartPageAddr = (u8)UART_GetIntNum();
		//UART_Printf("Input the page size to read : \n");
		//uPageSize = (u8)UART_GetIntNum();
	}
	else
	{
		uStartBlkAddr = oAutoVar.Test_Parameter[0];
		uStartPageAddr = oAutoVar.Test_Parameter[1];
		uPageSize = oAutoVar.Test_Parameter[2];
	}

	//pReadDataBase = (u32 *)malloc(ONENAND_PAGESIZE*uPageSize);
	pReadDataBase = (u32 *)malloc(ONENAND_PAGESIZE*64);
	if(pReadDataBase == 0)
	{
		UART_Printf("Memory allocation Error...\n");
		return;
	}
	else
	{
		pReadData = pReadDataBase;

		//for(i=0 ; i<(ONENAND_PAGESIZE*uPageSize)/4 ; i++)
		for(i=0 ; i<(ONENAND_PAGESIZE*64)/4 ; i++)
			*pReadData++ = 0;
	}

	ONENAND_EnableSpareTransfer(g_OneNandContNum, TRUE);

	m=0;
//	while(!UART_GetKey())
	{
		m++;
		srand(m);
		uPageSize = (u8)(rand()%64);
		if(uPageSize < 2)
			uPageSize = 2;
		
		pReadData = pReadDataBase;
//		uPageSizeTemp = uPageSize;
		
		for(i=0 ; i<(ONENAND_PAGESIZE*uPageSize)/4 ; i++)
				*pReadData++ = 0;

		pReadData = pReadDataBase;
		
		if(eTest == eTest_Manual)
		{
			UART_Printf("Cache Read(with Spare Area) : (%d page)\n", uPageSize);
		}
		
		ONENAND_PipelineReadAhead(g_OneNandContNum, uStartBlkAddr, uStartPageAddr, uPageSize);

		uReadPageCnt = 0;
		uBlkAddr = uStartBlkAddr;
		uPageAddr = uStartPageAddr;	
		OneNandT_oIntFlag.IntActInt_Count = 0;

		while(uReadPageCnt < uPageSize)
		{
			ONENAND_ReadPageForCacheReadWithSpare(g_OneNandContNum, uBlkAddr, uPageAddr, (u32*)pReadData, aSpareReadData, uPageSize);

			pReadData += ONENAND_PAGESIZE/4;

			uPageAddr++;
			if(uPageAddr == OneNand_Inform[g_OneNandContNum].uNumOfPage)
			{
				uPageAddr = 0;
				uBlkAddr++;
			}
			uReadPageCnt++;
#if 0
			for(i=0 ; i<ONENAND_SPARESIZE/4 ; i++)
			{
				if(!(i%4))
				{
					UART_Printf("\n");
					UART_Printf("0x%04x : ",i*4);
				}
				UART_Printf("0x%08x  ", aSpareReadData[i]);
			}
			UART_Printf("\n");
#endif		
			for(i=0; i<ONENAND_SPARESIZE/4; i++)
				aSpareReadData[i] = 0;
		}	
#if 1
		{
			u32 aWriteData[ONENAND_PAGESIZE/4] = {0, };
			u32 uError = FALSE;
			u32 k;

			uBlkAddr = uStartBlkAddr;
			uPageAddr = uStartPageAddr;
			pReadData = pReadDataBase;
		
			for(i=0 ; i<uPageSize ; i++)
			{
		#if(ONDT_DATA_PATTERN == ONDT_DATA_RANDOM)
				srand((uBlkAddr<<20)|(uPageAddr<<12));
		#endif
			
				for(k=0; k<ONENAND_PAGESIZE/4; k++)
				{
		#if(ONDT_DATA_PATTERN == ONDT_DATA_RANDOM)
					aWriteData[k] = rand();
		#elif (ONDT_DATA_PATTERN == ONDT_DATA_BLOCK_PAGE)
					aWriteData[k] = k +  (uPageAddr<<12) + (uBlkAddr<<20);
		#else	// ONDT_DATA_PATTERN == ONDT_DATA_ONEVALUE
					aWriteData[k] = ONDT_DATA_ONEVALUE;
		#endif
				}
					
				for(j=0 ; j<ONENAND_PAGESIZE/4 ; j++)
				{
					if(aWriteData[j] != *(pReadData+j))
					{
						UART_Printf("[Write data : Read data] = [0x%08x :  0x%08x] \n", aWriteData[j], *(pReadData+j));
						UART_Getc();
						uError = TRUE;
						break;
					}
				}
				uPageAddr++;
				if(uPageAddr == OneNand_Inform[g_OneNandContNum].uNumOfPage)
				{
					uPageAddr = 0;
					uBlkAddr++;
				}
				pReadData += ONENAND_PAGESIZE/4;
			}

			if(uError == FALSE)
			{
				if(eTest == eTest_Manual)
					UART_Printf("Cache Read(with Spare Area) Verify OK : %d (%d page)\n", m, uPageSize);
			}
		if(eTest == eTest_Manual)
			UART_Printf("\n");
		}	
#endif
	}
	
	ONENAND_EnableSpareTransfer(g_OneNandContNum, FALSE);
	
	free(pReadDataBase);
}


//////////
// Function Name : OneNandT_MultiPipelineRead
// Function Description : Sequencial pipelined reads followed by all data read at one time
// Input : 	eTest - Manual test or Auto test selection
//			oAutoVar - Parameters for Auto-Test
// Version : v0.1
void OneNandT_MultiPipelineRead(eFunction_Test eTest, oFunctionT_AutoVar oAutoVar)
{
	u32 i, j, uStartBlkAddr1, uStartBlkAddr2, uStartBlkAddr3, uReadPageCnt, uBlkAddr;
	u8 uStartPageAddr1, uStartPageAddr2, uStartPageAddr3;
	u8 uPageSize1, uPageSize2, uPageSize3, uPageSizeSum;
	u32 *pReadData1, *pReadDataBase1, *pReadData2, *pReadDataBase2, *pReadData3, *pReadDataBase3;
	u8 uPageAddr; 

	if(eTest == eTest_Manual)
	{
		UART_Printf("[OneNandT_MultiPipelineRead]\n");

		UART_Printf("Input the first block number to read[0~%d] : \n", OneNand_Inform[g_OneNandContNum].uNumOfBlock-1);
		uStartBlkAddr1 = UART_GetIntNum(); 
		UART_Printf("Input the first page number to read[0~%d] : \n", OneNand_Inform[g_OneNandContNum].uNumOfPage-1);
		uStartPageAddr1 = (u8)UART_GetIntNum();
		UART_Printf("Input the page size to read : \n");
		uPageSize1 = (u8)UART_GetIntNum();

		UART_Printf("Input the second block number to read[0~%d] : \n", OneNand_Inform[g_OneNandContNum].uNumOfBlock-1);
		uStartBlkAddr2 = UART_GetIntNum(); 
		UART_Printf("Input the second page number to read[0~%d] : \n", OneNand_Inform[g_OneNandContNum].uNumOfPage-1);
		uStartPageAddr2 = (u8)UART_GetIntNum();
		UART_Printf("Input the page size to read : \n");
		uPageSize2 = (u8)UART_GetIntNum();

		UART_Printf("Input the third block number to read[0~%d] : \n", OneNand_Inform[g_OneNandContNum].uNumOfBlock-1);
		uStartBlkAddr3 = UART_GetIntNum(); 
		UART_Printf("Input the third page number to read[0~%d] : \n", OneNand_Inform[g_OneNandContNum].uNumOfPage-1);
		uStartPageAddr3 = (u8)UART_GetIntNum();
		UART_Printf("Input the page size to read : \n");
		uPageSize3 = (u8)UART_GetIntNum();
	}
	else
	{
		uStartBlkAddr1 = oAutoVar.Test_Parameter[0];
		uStartPageAddr1 = oAutoVar.Test_Parameter[1];
		uPageSize1 = oAutoVar.Test_Parameter[2];
		uStartBlkAddr2 = oAutoVar.Test_Parameter[3];
		uStartPageAddr2 = oAutoVar.Test_Parameter[4];
		uPageSize2 = oAutoVar.Test_Parameter[5];
		uStartBlkAddr3 = oAutoVar.Test_Parameter[6];
		uStartPageAddr3 = oAutoVar.Test_Parameter[7];
		uPageSize3 = oAutoVar.Test_Parameter[8];
	}
	
	pReadDataBase1 = (u32 *)malloc(ONENAND_PAGESIZE*uPageSize1);
	if(pReadDataBase1 == 0)
	{
		UART_Printf("Memory allocation Error...\n");
		return;
	}
	else
	{
		pReadData1 = pReadDataBase1;

		for(i=0 ; i<(ONENAND_PAGESIZE*uPageSize1)/4 ; i++)
			*pReadData1++ = 0;
	}
	pReadData1 = pReadDataBase1;

	pReadDataBase2 = (u32 *)malloc(ONENAND_PAGESIZE*uPageSize2);
	if(pReadDataBase2 == 0)
	{
		UART_Printf("Memory allocation Error...\n");
		free(pReadDataBase1);
		return;
	}
	else
	{
		pReadData2 = pReadDataBase2;

		for(i=0 ; i<(ONENAND_PAGESIZE*uPageSize2)/4 ; i++)
			*pReadData2++ = 0;
	}
	pReadData2 = pReadDataBase2;

	pReadDataBase3 = (u32 *)malloc(ONENAND_PAGESIZE*uPageSize3);
	if(pReadDataBase3 == 0)
	{
		UART_Printf("Memory allocation Error...\n");
		free(pReadDataBase1);
		free(pReadDataBase2);
		return;
	}
	else
	{
		pReadData3 = pReadDataBase3;

		for(i=0 ; i<(ONENAND_PAGESIZE*uPageSize3)/4 ; i++)
			*pReadData3++ = 0;
	}
	pReadData3 = pReadDataBase3;
	
	ONENAND_PipelineReadAhead(g_OneNandContNum, uStartBlkAddr1, uStartPageAddr1, uPageSize1);
	ONENAND_PipelineReadAhead(g_OneNandContNum, uStartBlkAddr2, uStartPageAddr2, uPageSize2);
	ONENAND_PipelineReadAhead(g_OneNandContNum, uStartBlkAddr3, uStartPageAddr3, uPageSize3);

	uReadPageCnt = 0;
	uBlkAddr = uStartBlkAddr1;
	uPageAddr = uStartPageAddr1;	
	OneNandT_oIntFlag.IntActInt_Count = 0;
	uPageSizeSum = uPageSize1 + uPageSize2 + uPageSize3;
	
	while(uReadPageCnt < uPageSize1)
	{
		ONENAND_ReadPageForCacheRead(g_OneNandContNum, uBlkAddr, uPageAddr, (u32*)pReadData1, uPageSizeSum);
		pReadData1 += ONENAND_PAGESIZE/4;

		//Page Address증가 확인 필요???
		uPageAddr++;
		if(uPageAddr == OneNand_Inform[g_OneNandContNum].uNumOfPage)
		{
			uPageAddr = 0;
			uBlkAddr++;
		}
		uReadPageCnt++;

		//for(i=0; i<ONENAND_PAGESIZE/4; i++)
		//	aReadData[i] = 0;
	}	

	uReadPageCnt = 0;
	uBlkAddr = uStartBlkAddr2;
	uPageAddr = uStartPageAddr2;	
	while(uReadPageCnt < uPageSize2)
	{
		ONENAND_ReadPageForCacheRead(g_OneNandContNum, uBlkAddr, uPageAddr, (u32*)pReadData2, uPageSizeSum);
		pReadData2 += ONENAND_PAGESIZE/4;

		uPageAddr++;
		if(uPageAddr == OneNand_Inform[g_OneNandContNum].uNumOfPage)
		{
			uPageAddr = 0;
			uBlkAddr++;
		}
		uReadPageCnt++;
	}

	uReadPageCnt = 0;
	uBlkAddr = uStartBlkAddr3;
	uPageAddr = uStartPageAddr3;	
	while(uReadPageCnt < uPageSize3)
	{
		ONENAND_ReadPageForCacheRead(g_OneNandContNum, uBlkAddr, uPageAddr, (u32*)pReadData3, uPageSizeSum);
		pReadData3 += ONENAND_PAGESIZE/4;

		uPageAddr++;
		if(uPageAddr == OneNand_Inform[g_OneNandContNum].uNumOfPage)
		{
			uPageAddr = 0;
			uBlkAddr++;
		}		
		uReadPageCnt++;
	}	
	
#if 0
	pReadData1 = pReadDataBase1;
	for(i=0 ; i<uPageSize1 ; i++)
	{
		uPageAddr = (uStartPageAddr1+i)%64;
		uBlkAddr = uStartBlkAddr1 + (uStartPageAddr1+i)/64;
		UART_Printf("\n");
		UART_Printf("[%d block, %d page]", uBlkAddr, uPageAddr);
		for(j=0 ; j<ONENAND_PAGESIZE/4 ; j++)
		{
			if(!(j%4))
			{
				UART_Printf("\n");
				UART_Printf("0x%04x : ",i*4);
			}
			UART_Printf("0x%08x  ", *pReadData1++);
		}
		UART_Printf("\n");
	}

	pReadData2 = pReadDataBase2;
	for(i=0 ; i<uPageSize2 ; i++)
	{
		uPageAddr = (uStartPageAddr2+i)%64;
		uBlkAddr = uStartBlkAddr2 + (uStartPageAddr2+i)/64;
		UART_Printf("\n");
		UART_Printf("[%d block, %d page]", uBlkAddr, uPageAddr);
		for(j=0 ; j<ONENAND_PAGESIZE/4 ; j++)
		{
			if(!(j%4))
			{
				UART_Printf("\n");
				UART_Printf("0x%04x : ",i*4);
			}
			UART_Printf("0x%08x  ", *pReadData2++);
		}
		UART_Printf("\n");
	}

	pReadData3 = pReadDataBase3;
	for(i=0 ; i<uPageSize3 ; i++)
	{
		uPageAddr = (uStartPageAddr3+i)%64;
		uBlkAddr = uStartBlkAddr3 + (uStartPageAddr3+i)/64;
		UART_Printf("\n");
		UART_Printf("[%d block, %d page]", uBlkAddr, uPageAddr);
		for(j=0 ; j<ONENAND_PAGESIZE/4 ; j++)
		{
			if(!(j%4))
			{
				UART_Printf("\n");
				UART_Printf("0x%04x : ",i*4);
			}
			UART_Printf("0x%08x  ", *pReadData3++);
		}
		UART_Printf("\n");
	}	
#else
	{
		u32 k, uError = FALSE;
		u32 aWriteData[ONENAND_PAGESIZE/4] = {0, };
		
		pReadData1 = pReadDataBase1;
		for(i=0 ; i<uPageSize1 ; i++)
		{
			uPageAddr = (uStartPageAddr1+i)%64;
			uBlkAddr = uStartBlkAddr1 + (uStartPageAddr1+i)/64;

	#if(ONDT_DATA_PATTERN == ONDT_DATA_RANDOM)
			srand((uBlkAddr<<20)|(uPageAddr<<12));
	#endif
		
			for(k=0; k<ONENAND_PAGESIZE/4; k++)
			{
	#if(ONDT_DATA_PATTERN == ONDT_DATA_RANDOM)
				aWriteData[k] = rand();
	#elif (ONDT_DATA_PATTERN == ONDT_DATA_BLOCK_PAGE)
				aWriteData[k] = k +  (uPageAddr<<12) + (uBlkAddr<<20);
	#else	// ONDT_DATA_PATTERN == ONDT_DATA_ONEVALUE
				aWriteData[k] = ONDT_DATA_ONEVALUE;
	#endif
			}
		
			for(j=0 ; j<ONENAND_PAGESIZE/4 ; j++)
				if(aWriteData[j] != *pReadData1++)
				{
					UART_Printf("[Write data : Read data] = [0x%08x :  0x%08x] \n", aWriteData[j], *(pReadData1-1));
					UART_Getc();
					uError = TRUE;
					break;
				}
			uPageAddr++;
		}
		if(uError == FALSE)
		{
			if(eTest == eTest_Manual)
				UART_Printf("1st Cache Read Verify OK\n");
		}
		uError = FALSE;

		pReadData2 = pReadDataBase2;
		for(i=0 ; i<uPageSize2 ; i++)
		{
			uPageAddr = (uStartPageAddr2+i)%64;
			uBlkAddr = uStartBlkAddr2 + (uStartPageAddr2+i)/64;

	#if(ONDT_DATA_PATTERN == ONDT_DATA_RANDOM)
			srand((uBlkAddr<<20)|(uPageAddr<<12));
	#endif
		
			for(k=0; k<ONENAND_PAGESIZE/4; k++)
			{
	#if(ONDT_DATA_PATTERN == ONDT_DATA_RANDOM)
				aWriteData[k] = rand();
	#elif (ONDT_DATA_PATTERN == ONDT_DATA_BLOCK_PAGE)
				aWriteData[k] = k +  (uPageAddr<<12) + (uBlkAddr<<20);
	#else	// ONDT_DATA_PATTERN == ONDT_DATA_ONEVALUE
				aWriteData[k] = ONDT_DATA_ONEVALUE;
	#endif
			}
			
			for(j=0 ; j<ONENAND_PAGESIZE/4 ; j++)
				if(aWriteData[j] != *pReadData2++)
				{
					UART_Printf("[Write data : Read data] = [0x%08x :  0x%08x] \n", aWriteData[j], *(pReadData2-1));
					UART_Getc();
					uError = TRUE;
					break;
				}
			uPageAddr++;
		}
		if(uError == FALSE)
		{
			if(eTest == eTest_Manual)
				UART_Printf("2nd Cache Read Verify OK\n");
		}
		uError = FALSE;

		pReadData3 = pReadDataBase3;
		for(i=0 ; i<uPageSize3 ; i++)
		{
			uPageAddr = (uStartPageAddr3+i)%64;
			uBlkAddr = uStartBlkAddr3 + (uStartPageAddr3+i)/64;

	#if(ONDT_DATA_PATTERN == ONDT_DATA_RANDOM)
			srand((uBlkAddr<<20)|(uPageAddr<<12));
	#endif
		
			for(k=0; k<ONENAND_PAGESIZE/4; k++)
			{
	#if(ONDT_DATA_PATTERN == ONDT_DATA_RANDOM)
				aWriteData[k] = rand();
	#elif (ONDT_DATA_PATTERN == ONDT_DATA_BLOCK_PAGE)
				aWriteData[k] = k +  (uPageAddr<<12) + (uBlkAddr<<20);
	#else	// ONDT_DATA_PATTERN == ONDT_DATA_ONEVALUE
				aWriteData[k] = ONDT_DATA_ONEVALUE;
	#endif
			}
			
			for(j=0 ; j<ONENAND_PAGESIZE/4 ; j++)
				if(aWriteData[j] != *pReadData3++)
				{
					UART_Printf("[Write data : Read data] = [0x%08x :  0x%08x] \n", aWriteData[j], *(pReadData3-1));
					UART_Getc();
					uError = TRUE;
					break;
				}
			uPageAddr++;
		}	
		if(uError == FALSE)
		{
			if(eTest == eTest_Manual)
				UART_Printf("3rd Cache Read Verify OK\n");
		}	
	}
	if(eTest == eTest_Manual)
		UART_Printf("\n");
#endif

	free(pReadDataBase1);
	free(pReadDataBase2);
	free(pReadDataBase3);
}



//////////
// Function Name : OneNandT_SpareAccess
// Function Description : Read the Memory Main&Spare Area
// Input : 	eTest - Manual test or Auto test selection
//			oAutoVar - Parameters for Auto-Test
// Version : v0.1
void OneNandT_SpareAccess(eFunction_Test eTest, oFunctionT_AutoVar oAutoVar)
{
	u32 uStartBlkAddr, uPageSize, uWritePageCnt; 
	u32 uBlkAddr, uReadPageCnt, i;	
	u8 uStartPageAddr, uPageAddr;
	u8 uError;
	//u32 aWriteData[ONENAND_PAGESIZE/4] = {0, };
	//u32 aSpareWriteData[ONENAND_SPARESIZE/4];
	//u32 aReadData[ONENAND_PAGESIZE/4] = {0, };
	//u32 aSpareReadData[ONENAND_SPARESIZE/4] = {0, };
	u32 *aWriteData, *aSpareWriteData;
	u32 *aReadData, *aSpareReadData;

	UART_Printf("[OneNandT_SpareAccess]\n");
	
	UART_Printf("Input the Start block number to access[0~%d] : \n", OneNand_Inform[g_OneNandContNum].uNumOfBlock-1);
	uStartBlkAddr = UART_GetIntNum(); 
	UART_Printf("Input the Start page number to access[0~%d] : \n", OneNand_Inform[g_OneNandContNum].uNumOfPage-1);
	uStartPageAddr = (u8)UART_GetIntNum();
	UART_Printf("Input the page size to access : \n");
	uPageSize = UART_GetIntNum();

	aWriteData = (u32 *)malloc(ONENAND_PAGESIZE);
	if(aWriteData == 0)
	{
		UART_Printf("Memory allocation Error...\n");
		return;
	}

	aSpareWriteData = (u32 *)malloc(ONENAND_SPARESIZE);
	if(aSpareWriteData == 0)
	{
		free(aWriteData);
		UART_Printf("Memory allocation Error...\n");
		return;
	}

	aReadData = (u32 *)malloc(ONENAND_PAGESIZE);
	if(aReadData == 0)
	{
		free(aWriteData);
		free(aSpareWriteData);
		UART_Printf("Memory allocation Error...\n");
		return;
	}

	aSpareReadData = (u32 *)malloc(ONENAND_SPARESIZE);
	if(aSpareReadData == 0)
	{
		free(aWriteData);
		free(aSpareWriteData);
		free(aReadData);
		UART_Printf("Memory allocation Error...\n");
		return;
	}	
	
#if (ONENAND_ACCESS_MODE == ONENAND_COMMAND)
	ONENAND_EnableSpareTransfer(g_OneNandContNum, TRUE);
#endif

	uWritePageCnt = 0;
	uBlkAddr = uStartBlkAddr;
	uPageAddr = uStartPageAddr;

	UART_Printf("1. Writing the data\n");
	while(uWritePageCnt < uPageSize)
	{
#if 1
	#if(ONDT_DATA_PATTERN == ONDT_DATA_RANDOM)
		srand((uBlkAddr<<20)|(uPageAddr<<12));
	#endif
	
		for(i=0; i<ONENAND_PAGESIZE/4; i++)
		{
	#if(ONDT_DATA_PATTERN == ONDT_DATA_RANDOM)
			aWriteData[i] = rand();
	#elif (ONDT_DATA_PATTERN == ONDT_DATA_BLOCK_PAGE)
			aWriteData[i] = i +  (uPageAddr<<12) + (uBlkAddr<<20);
	#else	// ONDT_DATA_PATTERN == ONDT_DATA_ONEVALUE
			aWriteData[i] = ONDT_DATA_ONEVALUE;
	#endif
		}	

		for(i=0; i<ONENAND_SPARESIZE/4; i++)
		{
			aSpareWriteData[i] = 0xFFFFFFFF;
		}		
#endif

		ONENAND_WritePageWithSpare(g_OneNandContNum, uBlkAddr, uPageAddr, (u32*)aWriteData, (u32 *)aSpareWriteData);
		uPageAddr++;
		if(uPageAddr == OneNand_Inform[g_OneNandContNum].uNumOfPage)
		{
			uPageAddr = 0;
			uBlkAddr++;
		}
		uWritePageCnt++;
	}

#if 0
#if 1
	// Data Read
	if(eTest == eTest_Manual)
		UART_Printf("2. Memory Read & Verify\n");
	uReadPageCnt = 0;
	uBlkAddr = uStartBlkAddr;
	uPageAddr = uStartPageAddr;	
	while(uReadPageCnt < uPageSize)
	{
		ONENAND_ReadPageWithSpare(g_OneNandContNum, uBlkAddr, uPageAddr, (u32*)aReadData, (u32 *)aSpareReadData);
#if 1
	#if(ONDT_DATA_PATTERN == ONDT_DATA_RANDOM)
		srand((uBlkAddr<<20)|(uPageAddr<<12));
	#endif
	
		for(i=0; i<ONENAND_PAGESIZE/4; i++)
		{
	#if(ONDT_DATA_PATTERN == ONDT_DATA_RANDOM)
			aWriteData[i] = rand();
	#elif (ONDT_DATA_PATTERN == ONDT_DATA_BLOCK_PAGE)
			aWriteData[i] = i +  (uPageAddr<<12) + (uBlkAddr<<20);
	#else	// ONDT_DATA_PATTERN == ONDT_DATA_ONEVALUE
			aWriteData[i] = ONDT_DATA_ONEVALUE;
	#endif
		}
#endif

		for(i=0 ; i<ONENAND_PAGESIZE/4 ; i++)
		{
			if(aReadData[i] != aWriteData[i])
			{
				UART_Printf("Write&Read Verify Error [%d Block : %d Page] :  \n", uBlkAddr, uPageAddr);
				UART_Printf("\t [Write data : Read data] = [0x%08x  : 0x%08x ]  \n", aWriteData[i], aReadData[i]);
				UART_Getc();
				uError = TRUE;
				break;
			}
		}

		uPageAddr++;
		if(uPageAddr == OneNand_Inform[g_OneNandContNum].uNumOfPage)
		{
			uPageAddr = 0;
			uBlkAddr++;
		}
		uReadPageCnt++;

		for(i=0; i<ONENAND_PAGESIZE/4; i++)
			aReadData[i] = 0; 
		
		if(eTest == eTest_Auto)
		{
			UART_Printf(".");
			if(!(uReadPageCnt%10))
				UART_Printf("\b\b\b\b\b\b\b\b\b\b");
		}
			
	}
#endif

#else	
	// Data Read
	uReadPageCnt = 0;
	uBlkAddr = uStartBlkAddr;
	uPageAddr = uStartPageAddr;	

	UART_Printf("2. Reading the data\n");
	while(uReadPageCnt < uPageSize)
	{
		ONENAND_ReadPageWithSpare(g_OneNandContNum, uBlkAddr, uPageAddr, (u32*)aReadData, (u32 *)aSpareReadData);
		
		uError = FALSE;
#if 1		// Data Comparison
	#if(ONDT_DATA_PATTERN == ONDT_DATA_RANDOM)
		srand((uBlkAddr<<20)|(uPageAddr<<12));
	#endif
	
		for(i=0; i<ONENAND_PAGESIZE/4; i++)
		{
	#if(ONDT_DATA_PATTERN == ONDT_DATA_RANDOM)
			aWriteData[i] = rand();
	#elif (ONDT_DATA_PATTERN == ONDT_DATA_BLOCK_PAGE)
			aWriteData[i] = i +  (uPageAddr<<12) + (uBlkAddr<<20);
	#else	// ONDT_DATA_PATTERN == ONDT_DATA_ONEVALUE
			aWriteData[i] = ONDT_DATA_ONEVALUE;
	#endif
		}

		for(i=0 ; i<ONENAND_PAGESIZE/4 ; i++)
		{
			if(aReadData[i] != aWriteData[i])
			{
				UART_Printf("Write&Read Verify Error [%d Block : %d Page] :  \n", uBlkAddr, uPageAddr);
				UART_Printf("\t [Write data : Read data] = [0x%08x  : 0x%08x ]  \n", aWriteData[i], aReadData[i]);
				UART_Getc();
				uError = TRUE;
				break;
			}
		}

		if(uError == FALSE)
			UART_Printf("[%d block, %d page] : Verify OK\n", uBlkAddr, uPageAddr);
#endif		
		
		for(i=0 ; i<ONENAND_PAGESIZE/4 ; i++)
		{
			if(!(i%4))
			{
				UART_Printf("\n");
				UART_Printf("0x%04x : ",i*4);
			}
			UART_Printf("0x%08x  ", aReadData[i]);
		}
		UART_Printf("\n\n");
		UART_Printf("[Spare Area]");
		for(i=0 ; i<ONENAND_SPARESIZE/4 ; i++)
		{
			if(!(i%4))
			{
				UART_Printf("\n");
				UART_Printf("0x%04x : ",i*4);
			}
			UART_Printf("0x%08x  ", aSpareReadData[i]);
		}		
		UART_Printf("\n\n\n");
		
		uPageAddr++;
		if(uPageAddr == OneNand_Inform[g_OneNandContNum].uNumOfPage)
		{
			uPageAddr = 0;
			uBlkAddr++;
		}
		uReadPageCnt++;

		for(i=0; i<ONENAND_PAGESIZE/4; i++)
			aReadData[i] = 0;

		//ONENAND_ConfigMainAccess(g_OneNandContNum, uBlkAddr, uPageAddr);		
	}
#endif

#if (ONENAND_ACCESS_MODE == ONENAND_COMMAND)
	ONENAND_EnableSpareTransfer(g_OneNandContNum, FALSE);
#endif

	free(aWriteData);
	free(aSpareWriteData);
	free(aReadData);
	free(aSpareReadData);
}



//////////
// Function Name : OneNandT_OTPWrite
// Function Description : Write the OTP Area
// Input : 	eTest - Manual test or Auto test selection
//			oAutoVar - Parameters for Auto-Test
// Version : v0.1
void OneNandT_OTPWrite(eFunction_Test eTest, oFunctionT_AutoVar oAutoVar)
{
	u32 uPageSize; 
	u32 uBlkAddr, uReadPageCnt, i;	
	u8 uStartPageAddr, uPageAddr;
	//u32 aWriteData[ONENAND_PAGESIZE/4] = {0, };
	u32 *aWriteData;
	
	UART_Printf("[OneNandT_OTPWrite]\n");
	
	UART_Printf("Input the Start page number to write[0~10] : \n");
	uStartPageAddr = (u8)UART_GetIntNum();
	UART_Printf("Input the page size to write : \n");
	uPageSize = UART_GetIntNum();

	aWriteData = (u32 *)malloc(ONENAND_PAGESIZE);
	if(aWriteData == 0)
	{
		UART_Printf("Memory allocation Error...\n");
		return;
	}
	
	srand(0x3);
	for(i=0; i<ONENAND_PAGESIZE/4; i++)
		aWriteData[i] = rand();

	//Data Write
	uReadPageCnt = 0;
	uBlkAddr = 0;
	uPageAddr = uStartPageAddr;	

	UART_Printf("1. OTP Area Write\n");
	ONENAND_ConfigOTPAccess(g_OneNandContNum, uBlkAddr, uPageAddr);
		
	while(uReadPageCnt < uPageSize)
	{
		ONENAND_WritePage(g_OneNandContNum, uBlkAddr, uPageAddr, (u32*)aWriteData);

		UART_Printf("\n");
		UART_Printf("[%d block, %d page]", uBlkAddr, uPageAddr);
		for(i=0 ; i<ONENAND_PAGESIZE/4 ; i++)
		{
			if(!(i%4))
			{
				UART_Printf("\n");
				UART_Printf("0x%04x : ",i*4);
			}
			UART_Printf("0x%08x  ", aWriteData[i]);
		}
		UART_Printf("\n\n");

		uPageAddr++;
		uReadPageCnt++;
	}

	ONENAND_Reset(g_OneNandContNum, ONENAND_CORERESET);

	free(aWriteData);
}


//////////
// Function Name : OneNandT_OTPRead
// Function Description : Read the OTP Area
// Input : 	eTest - Manual test or Auto test selection
//			oAutoVar - Parameters for Auto-Test
// Version : v0.1
void OneNandT_OTPRead(eFunction_Test eTest, oFunctionT_AutoVar oAutoVar)
{
	u32 uPageSize; 
	u32 uBlkAddr, uReadPageCnt, i;	
	u8 uStartPageAddr, uPageAddr;
	//u32 aReadData[ONENAND_PAGESIZE/4] = {0, };
	u32 *aReadData;
	
	UART_Printf("[OneNandT_OTPRead]\n");
	
	UART_Printf("Input the Start page number to read[0~10] : \n");
	uStartPageAddr = (u8)UART_GetIntNum();
	UART_Printf("Input the page size to read : \n");
	uPageSize = UART_GetIntNum();

	aReadData = (u32 *)malloc(ONENAND_PAGESIZE);
	if(aReadData == 0)
	{
		UART_Printf("Memory allocation Error...\n");
		return;
	}
	
	// Data Read
	uReadPageCnt = 0;
	uBlkAddr = 0;
	uPageAddr = uStartPageAddr;	

	UART_Printf("1. OTP Area Read\n");
	UART_Printf("\n");
	ONENAND_ConfigOTPAccess(g_OneNandContNum, uBlkAddr, uPageAddr);
		
	while(uReadPageCnt < uPageSize)
	{
		ONENAND_ReadPage(g_OneNandContNum, uBlkAddr, uPageAddr, (u32*)aReadData);

		UART_Printf("\n");
		UART_Printf("[%d block, %d page]", uBlkAddr, uPageAddr);
		for(i=0 ; i<ONENAND_PAGESIZE/4 ; i++)
		{
			if(!(i%4))
			{
				UART_Printf("\n");
				UART_Printf("0x%04x : ",i*4);
			}
			UART_Printf("0x%08x  ", aReadData[i]);
		}
		UART_Printf("\n\n");

		uPageAddr++;
		uReadPageCnt++;

		for(i=0; i<ONENAND_PAGESIZE/4; i++)
			aReadData[i] = 0;
	}

	ONENAND_Reset(g_OneNandContNum, ONENAND_CORERESET);

	free(aReadData);
}



const AutotestFuncMenu onenand_program[] =
{
//	OneNandT_MemoryWriteRead_Test,					"Device W/R Test",

	OneNandT_MemoryWriteRead,					"Device W/R(MAP01)",
	OneNandT_MemoryRead,							"Device Read",	
#if (ONENAND_ACCESS_MODE == ONENAND_COMMAND)	
	OneNandT_ReadModifyWrite,						"Read/Modify/Write(MAP10, MAP00)",
#endif	
	OneNandT_SinglePipelineRead,					"Single Pipeline Read-Ahead",	
	OneNandT_SinglePipelineReadWithSpare,			"Single Pipeline Read-Ahead with Spare",	
#if (ONENAND_ACCESS_MODE == ONENAND_COMMAND)	
	OneNandT_MultiPipelineRead,						"Multi Pipeline Read-Ahead",
#endif
	OneNandT_OTPWrite,							"OTP Area Write",
	OneNandT_OTPRead,							"OTP Area Read",
	OneNandT_SpareAccess, 						"Spare Area Access",
	0, 0
};

void OneNandT_ProgramMemory(void)
{
	u32 i;
	s32 uSel;
	oFunctionT_AutoVar oAutoVar;
	
	while(1)
	{
		for (i=0; (u32)(onenand_program[i].desc)!=0; i++)
			UART_Printf("%2d: %s\n", i, onenand_program[i].desc);

		UART_Printf("\nSelect the function to test : ");
		uSel =UART_GetIntNum();
		UART_Printf("\n");
		if(uSel == -1) 
			break;

		if (uSel>=0 && uSel<(sizeof(onenand_program)/8-1))
			(onenand_program[uSel].func) (eTest_Manual, oAutoVar);
	}
}



///////////////////////////////////////////////////////////////////////////////////
/////////////////                     OneNand Direct Access                 ///////////////////////// 
///////////////////////////////////////////////////////////////////////////////////

#if (ONENAND_INTMODE == ONENAND_INTERRUPT)
//////////
// Function Name : OneNandT_DirectAccess
// Function Description : OneNand direct access test using Map11 command
// Input : 	None
// Version : v0.1
void OneNandT_DirectAccess(void)
{
	u32 i, uBlock, uPage;
	u8 bError;
	u32 aWriteData[ONENAND_PAGESIZE/2] = {0, };
	u32 aReadData[ONENAND_PAGESIZE/2] = {0, };
	u32 uDRead;
		
	UART_Printf("[OneNandT_DirectAccess]\n\n");

	UART_Printf("\n");
	UART_Printf("1. OneNAND register read\n");
	ONENAND_DirectRead(g_OneNandContNum, OND_MANUFACT_ID, &uDRead);
	UART_Printf(" OneNAND Manufacturer ID register (0x%x) = 0x%04x\n", OND_MANUFACT_ID/4, uDRead&0xFFFF);
	
	ONENAND_DirectRead(g_OneNandContNum, OND_DEVICE_ID, &uDRead);
	UART_Printf(" OneNAND Device ID register (0x%x) = 0x%04x\n", OND_DEVICE_ID/4, uDRead&0xFFFF);
	
	ONENAND_DirectRead(g_OneNandContNum, OND_VER_ID, &uDRead);
	UART_Printf(" OneNAND Version ID register (0x%x) = 0x%04x\n", OND_VER_ID/4, uDRead&0xFFFF);
		
	ONENAND_DirectRead(g_OneNandContNum, OND_DATA_BUFSIZE, &uDRead);
	UART_Printf(" OneNAND Data Buffer size register (0x%x) = 0x%04x\n", OND_DATA_BUFSIZE/4, uDRead&0xFFFF);
	
	ONENAND_DirectRead(g_OneNandContNum, OND_BOOT_BUFSIZE, &uDRead);
	UART_Printf(" OneNAND Boot Buffer size register (0x%x) = 0x%04x\n", OND_BOOT_BUFSIZE/4, uDRead&0xFFFF);
	
	ONENAND_DirectRead(g_OneNandContNum, OND_AMOUNT_OF_BUFSIZE, &uDRead);
	UART_Printf(" OneNAND Amount of buffers register (0x%x) = 0x%04x\n", OND_AMOUNT_OF_BUFSIZE/4, uDRead&0xFFFF);
	
	ONENAND_DirectRead(g_OneNandContNum, OND_TECHNOLOGY, &uDRead);
	UART_Printf(" OneNAND Technology register (0x%x) = 0x%04x\n", OND_TECHNOLOGY/4, uDRead&0xFFFF);

	ONENAND_DirectRead(g_OneNandContNum, OND_STARTADDR1, &uDRead);
	UART_Printf(" OneNAND STARTADDR1 register (0x%x) = 0x%04x\n", OND_STARTADDR1/4, uDRead&0xFFFF);

	ONENAND_DirectRead(g_OneNandContNum, OND_STARTADDR2, &uDRead);
	UART_Printf(" OneNAND STARTADDR2 register (0x%x) = 0x%04x\n", OND_STARTADDR2/4, uDRead&0xFFFF);
	
	ONENAND_DirectRead(g_OneNandContNum, OND_STARTADDR3, &uDRead);
	UART_Printf(" OneNAND STARTADDR3 register (0x%x) = 0x%04x\n", OND_STARTADDR3/4, uDRead&0xFFFF);

	ONENAND_DirectRead(g_OneNandContNum, OND_STARTADDR4, &uDRead);
	UART_Printf(" OneNAND STARTADDR4 register (0x%x) = 0x%04x\n", OND_STARTADDR4/4, uDRead&0xFFFF);

	ONENAND_DirectRead(g_OneNandContNum, OND_STARTADDR8, &uDRead);
	UART_Printf(" OneNAND STARTADDR3 register (0x%x) = 0x%04x\n", OND_STARTADDR8/4, uDRead&0xFFFF);

	ONENAND_DirectRead(g_OneNandContNum, OND_STARTBUF, &uDRead);
	UART_Printf(" OneNAND START_Buffer register (0x%x) = 0x%04x\n", OND_STARTBUF/4, uDRead&0xFFFF);

	ONENAND_DirectRead(g_OneNandContNum, OND_SYSCONFIG, &uDRead);
	UART_Printf(" OneNAND System_configuration1 register (0x%x) = 0x%04x\n", OND_SYSCONFIG/4, uDRead&0xFFFF);

	ONENAND_DirectRead(g_OneNandContNum, OND_STATUS, &uDRead);
	UART_Printf(" OneNAND Controller_Status register (0x%x) = 0x%04x\n", OND_STATUS/4, uDRead&0xFFFF);

	ONENAND_DirectRead(g_OneNandContNum, OND_INTSTATUS, &uDRead);
	UART_Printf(" OneNAND Interrupt_Status register (0x%x) = 0x%04x\n", OND_INTSTATUS/4, uDRead&0xFFFF);
	
	ONENAND_DirectRead(g_OneNandContNum, OND_STARTBLOCKADDR, &uDRead);
	UART_Printf(" OneNAND Start_Block_Address register (0x%x) = 0x%04x\n", OND_STARTBLOCKADDR/4, uDRead&0xFFFF);

	ONENAND_DirectRead(g_OneNandContNum, OND_WPROT_STATUS, &uDRead);
	UART_Printf(" OneNAND Write_Protect_Status register (0x%x) = 0x%04x\n", OND_WPROT_STATUS/4, uDRead&0xFFFF);

	UART_Printf("2. OneNand 100~109 Block Erase\n");
	for(uBlock=100 ; uBlock<110 ; uBlock++) 
	{
		bError = ONENAND_EraseBlockDirectAccess(g_OneNandContNum, uBlock);
		if(bError)
		{
			UART_Printf("Erase Block Error [Block : %d]\n", uBlock);
			return;
		}
	}
	UART_Printf(" =>OneNand 100~109 Block Erase is completed\n");

	UART_Printf("3. OneNand Page Write(100~109 Block)\n");
	srand(0x4);
	for(i=0; i<ONENAND_PAGESIZE/2; i++)
		aWriteData[i] = rand();
		//aWriteData[i] = i;
	
	for(i=0 ; i<ONENAND_PAGESIZE/2 ; i++)
	{
		ONENAND_DirectWrite(g_OneNandContNum, ONENAND_DATARAM_DIRECT+(4*i), aWriteData[i]);
	}
	
	for(uBlock=100 ; uBlock<110 ; uBlock++)
	{
		for(uPage=0 ; uPage<OneNand_Inform[g_OneNandContNum].uNumOfPage ; uPage++)
		{
			bError = ONENAND_WritePageDirectAccess(g_OneNandContNum, uBlock, uPage);
			if(bError)
			{
				UART_Printf("Page Write Error [Block : %d, Page : %d]\n", uBlock, uPage);
				return;
			}
		}
	}
	UART_Printf(" =>OneNand Page Write(100~109 Block) is completed\n");

	UART_Printf("4. OneNand Page Read(100~109 Block)\n");
	
	for(uBlock=100 ; uBlock<110 ; uBlock++)
	{
		for(uPage=0 ; uPage<OneNand_Inform[g_OneNandContNum].uNumOfPage ; uPage++)
		{
			bError = ONENAND_ReadPageDirectAccess(g_OneNandContNum, uBlock, uPage);
			if(bError)
			{
				UART_Printf("Page Write Error [Block : %d, Page : %d]\n", uBlock, uPage);
				UART_Getc();
				return;
			}
			
			for(i=0; i<ONENAND_PAGESIZE/2; i++)
				aReadData[i] = 0;
				
			for(i=0 ; i<ONENAND_PAGESIZE/2 ; i++)
			{	
				ONENAND_DirectRead(g_OneNandContNum, ONENAND_DATARAM_DIRECT+(4*i), aReadData+i);
				if((aReadData[i]&0xFFFF) != (aWriteData[i]&0xFFFF))
				{
					UART_Printf("Page Read Error [Block : %d, Page : %d]\n", uBlock, uPage);
					UART_Printf(" [Write Data : Read Data] = [0x%04x : 0x%04x]\n", aWriteData[i]&0xFFFF, aReadData[i]&0xFFFF);
					UART_Getc();
					return;
				}
			}		
		}
	}

	UART_Printf(" =>OneNand Page read(100~109 Block) is completed\n");
	UART_Printf("\n");
	
}
#endif 		//  (ONENAND_INTMODE == ONENAND_INTERRUPT)

///////////////////////////////////////////////////////////////////////////////////
/////////////////                     OneNand Binary Write                  ///////////////////////// 
///////////////////////////////////////////////////////////////////////////////////

//////////
// Function Name : OneNandT_ProgramBinary
// Function Description : Write the binary from DRAM to OneNand (ex. boot code, image etc)
// Input : 	None
// Version : v0.1
void OneNandT_ProgramBinary(void)
{
	u32 uBlkAddr, uSize;
	u32 *aReadData;
	u32 uSourceAddr, uPageToWrite, uWriteBlock, i, j;
	u32 *pSourceData;
	u8 uPageAddr;
	
	UART_Printf("[OneNandT_ProgramBinary]\n\n");

	uSourceAddr = _DRAM_BaseAddress+0x01000000;
	UART_Printf("Caution : You must put BINARY file into 0x%08x before programming\n",uSourceAddr);

	UART_Printf("Enter block # to program : ");
	uBlkAddr = UART_GetIntNum();
	if((s32)uBlkAddr == -1)
		uBlkAddr = 0;

	UART_Printf("Enter size to program : ");
	uSize = UART_GetIntNum();
	if((s32)uSize == -1)
		uSize = 0x40000;

	aReadData= (u32 *)malloc(ONENAND_PAGESIZE);
	if(aReadData == 0)
	{
		UART_Printf("malloc() error..!!");
		return;
	}
	
	uPageToWrite = uSize/ONENAND_PAGESIZE + 1;
	
	//while(!UART_GetKey())
	{
	pSourceData = (u32 *)uSourceAddr;

	ONENAND_EraseBlock(g_OneNandContNum, uBlkAddr, uBlkAddr+uPageToWrite/OneNand_Inform[g_OneNandContNum].uNumOfPage+1);

	//Binary Write
	for(i=0 ; i<uPageToWrite ; i++)
	{
		uPageAddr = i%64;
		uWriteBlock = uBlkAddr + i/64;
		ONENAND_WritePage(g_OneNandContNum, uWriteBlock, uPageAddr, pSourceData);
		pSourceData += ONENAND_PAGESIZE/4;
	}
	//}
	
	//Verify
	pSourceData = (u32 *)uSourceAddr;
	for(i=0 ; i<uPageToWrite ; i++)
	{
		uPageAddr = i%64;
		uWriteBlock = uBlkAddr + i/64;
		ONENAND_ReadPage(g_OneNandContNum, uWriteBlock, uPageAddr, aReadData);		

		for(j=0 ; j<ONENAND_PAGESIZE/4 ; j++)
		{
			if(*pSourceData != aReadData[j])
			{
				UART_Printf("OneNand Binary Write&Verify Error[%d Block, %d Page]\n", uWriteBlock, uPageAddr);
				UART_Printf("\t[Source Data : Read Data] = [0x%08x : 0x%08x]\n\n", *pSourceData, aReadData[j]);
				UART_Getc();
				return;
			}
			pSourceData++;
		}
	}

	UART_Printf("OneNand Binary Write&Verify Complete\n\n");
	}
}


///////////////////////////////////////////////////////////////////////////////////
////////////////////                     OneNand Reset Test              /////////////////////////// 
///////////////////////////////////////////////////////////////////////////////////

//////////
// Function Name : OneNandT_Reset
// Function Description : OneNand Reset test
// Input : 	None
// Version : v0.1
void OneNandT_Reset(void)
{
	u32 uResetMode;
	
	UART_Printf("[OneNandT_Reset]\n\n");

	UART_Printf("Select the Reset Mode : \n");
	UART_Printf("1. Warm Reset(D),  2. Core Reset,  3. Hot Reset \n");
	uResetMode = UART_GetIntNum();

	switch(uResetMode)
	{
		case 1	: 	ONENAND_Reset(g_OneNandContNum, ONENAND_WARMRESET);
					UART_Printf("Warm Reset is completed\n");
					break;
		case 2	: 	ONENAND_Reset(g_OneNandContNum, ONENAND_CORERESET);
					UART_Printf("Core Reset is completed\n");
					break;					
		case 3	: 	ONENAND_Reset(g_OneNandContNum, ONENAND_HOTRESET);
					UART_Printf("Hot Reset is completed\n");
					break;
		default	:	ONENAND_Reset(g_OneNandContNum, ONENAND_WARMRESET);
					UART_Printf("Warm Reset is completed\n");
					break;
	}
	
	ONENAND_UnlockBlock(g_OneNandContNum, 0, OneNand_Inform[g_OneNandContNum].uNumOfBlock-1);
	
	UART_Printf("\n");
}



void OneNandT_ReadIntCount(void)
{
	UART_Printf("OneNand Int Count : %d\n", OneNandT_oIntFlag.IntActInt_Count);
	OneNandT_oIntFlag.IntActInt_Count = 0;
}

extern OneNandDirectBootCopy(void);
void OneNandT_BootCopy(void)
{
	UART_Printf("[OneNandT_BootCopy]\n");
	
	OneNandDirectBootCopy();
}


u32 OneNandT_GetTestStartBlock(void)
{
	u32 i, uBadBlockNum, uStartBlock0;	//, uStartBlock1;
	oFunctionT_AutoVar oAutoTest;
		
	OneNandT_CheckBadBlock(eTest_Auto, oAutoTest);

	i=0;
	while(1)
	{
		if(aOneNandBadBlock[i] == 0)
		{
			uBadBlockNum = i;
			break;
		}
		i++;

		if(i >= ONDT_NUMOFBAD_BUFFER)
				break;
	}

	uStartBlock0 = OneNand_Inform[g_OneNandContNum].uNumOfBlock;
	
	if(aOneNandBadBlock[0] == 0)
		uStartBlock0 = 200;
	
	for(i=0 ; i<uBadBlockNum ; i++)
	{
		if( (aOneNandBadBlock[i+1]-aOneNandBadBlock[i]) > 150 )
		{
			uStartBlock0 = aOneNandBadBlock[i] + 1;
			break;
		}
	}

	if( uStartBlock0 == OneNand_Inform[g_OneNandContNum].uNumOfBlock)
	{
		UART_Printf("Bad Block Fail\n");
		return 0;
	}

	return uStartBlock0;
}


extern void STOPT_Basic(void);
void System_PowerMode(void)
{
	u32 uPowerMode;
	
	UART_Printf("[System_PowerMode]\n");
	
	UART_Printf("0. Idle Mode,  1. Stop Mode,  2. Deep Stop Mode,   3. Sleep Mode \n");
	uPowerMode = UART_GetIntNum();
	if(uPowerMode > 3)
		uPowerMode = 0;

	switch (uPowerMode)
	{
		case 0	: 	SYSC_IDLE();
					break;
		case 1	: 	//SYSC_STOP();
					STOPT_Basic();
					break;
		case 2	: 	SYSC_DEEPSTOP();
					break;
		case 3 	:	SYSC_SLEEP();
					break;
		default	:	SYSC_IDLE();
					break;
	}

}


///////////////////////////////////////////////////////////////////////////////////
////////////////////                     OneNand Main Test              /////////////////////////// 
///////////////////////////////////////////////////////////////////////////////////

const testFuncMenu onenand_menu[] = 
{
	OneNandT_LockTest,							"Lock Block",
	OneNandT_EraseTest,						"Erase Block",
	OneNandT_CopyBackTest,					"Copy Back",
	OneNandT_ProgramMemory,					"Program Page",
#if (ONENAND_INTMODE == ONENAND_INTERRUPT)	
	OneNandT_DirectAccess,						"Direct Access",
#endif	
	OneNandT_ProgramBinary,					"Binary Program",
	OneNandT_SetParameter,					"Parameter Setting",
	OneNandT_Reset,							"Reset",	
	System_PowerMode,							"System Power Mode",
	0, 0
};

void ONENAND_Test(void)
{
	u32 i;
	s32 uSel;
	
	UART_Printf("[ONENAND_Test]\n\n");
	UART_Printf("Input the OneNand Controller Number (OneNand0[0], OneNand1[1]) :");
	g_OneNandContNum = UART_GetIntNum();

	if(g_OneNandContNum > 1)
		g_OneNandContNum = ONENAND0; 

	OneNandT_SetISRHandler(g_OneNandContNum);
	if(ONENAND_Init(g_OneNandContNum))
		OneNandT_DevInformDisplay(g_OneNandContNum);

	//OneNandT_SetISRHandler(g_OneNandContNum);
	//ONENAND_EnableAllInterrupt(g_OneNandContNum);
	//ONENAND_DisableInterrupt(g_OneNandContNum,  eOND_RDYACT);
	
	//added by rb1004....2007.04.23
	if(OneNand_Inform[g_OneNandContNum].uDbsDfs == 0)
		ONENAND_UnlockBlock(g_OneNandContNum, 0, OneNand_Inform[g_OneNandContNum].uNumOfBlock-1);
	else
	{
		ONENAND_UnlockBlock(g_OneNandContNum, 0, (OneNand_Inform[g_OneNandContNum].uNumOfBlock/2)-1);
		ONENAND_UnlockBlock(g_OneNandContNum, OneNand_Inform[g_OneNandContNum].uNumOfBlock/2, OneNand_Inform[g_OneNandContNum].uNumOfBlock-1);
		//ONENAND_UnlockBlock(g_OneNandContNum, 0, (OneNand_Inform[g_OneNandContNum].uNumOfBlock/2)-1);
	}
	
	while(1)
	{
		for (i=0; (u32)(onenand_menu[i].desc)!=0; i++)
			UART_Printf("%2d: %s\n", i, onenand_menu[i].desc);

		UART_Printf("\nSelect the function to test : ");
		uSel =UART_GetIntNum();
		UART_Printf("\n");
		if(uSel == -1) 
			break;

		if (uSel>=0 && uSel<(sizeof(onenand_menu)/8-1))
			(onenand_menu[uSel].func) ();
	}
}	


