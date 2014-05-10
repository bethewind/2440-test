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
*	File Name : ata_test.c
*  
*	File Description : This file implements PIO/UDMA mode test functions of CF Controller.
*
*	Author : Sunil,Roe
*	Dept. : AP Development Team
*	Created Date : 2007/1/26
*	Version : 0.1 
* 
*	History
*	- Created(Sunil,Roe 2007/1/26)
*   
**************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "system.h"
#include "library.h"
#include "intc.h"
#include "timer.h"
#include "ata.h"
#include "cf.h"
#include "gpio.h"
#include "nand.h"
#include "dma.h"
#include "sromc.h"

#define NAND_DATA_SIZE	NAND_PAGE_512

// global variables
u8 g_ucOpMode;
u8 g_ucATAConNum;
volatile ATA_oInform	g_oaATAInform[ATA_CONNUM];
volatile ATA_oInform	g_oaATAInformMaster[ATA_CONNUM];
volatile ATA_oInform	g_oaATAInformSlave[ATA_CONNUM];

extern EBI_oInform	g_oaEBIInform[];
extern u8 aBuffer[];
extern u8 aSpareBuffer[];

void Set_Ata_Cmd_START(void);
void Set_Ata_Cmd_STOP(void);
void Set_Ata_Cmd_ABORT(void);
void Set_Ata_Cmd_CONTINUE(void);

// function declaration
void ATA_Test(void);
void ATA_TestChangeModeToAta(void);
void ATA_TestReset(void);
void ATA_PrintDeviceInfo();
void ATA_TestBasicWriteRead(void);
void ATA_TestPioCpuMode(void);
void ATA_TestPioCpuModeRead(void);
void ATA_ClearPioCpuMode(void);
void ATA_TestPioDmaMode(void);
void	ATA_TestPioDmaModeWrite(void);
void	ATA_TestPioDmaModeRead(void);
void ATA_TestUdmaMode(void);
void	ATA_TestUdmaModeRead(void);
void ATA_TestPioDmaMode_Int(void);
void	ATA_TestUDmaMode_Int(void);
void ATA_AgingTestPioDmaMode(void);
void ATA_AgingTestUdmaMode(void);
void ATA_AgingTestUdmaModeMasterSlave(void);
void	ATA_TestUDmaMode_Int_for_AtaBug(void);
u8 ATA_OpenMedia(u8 ucCon, eATA_MODE_6400 eAtaMode, eATA_SLOT_SEL eSlotsel);
u8 ATA_CloseMedia(u8 ucCon);
u8 ATA_PioCpu_autotest(void);
u8 ATA_PioDma_autotest(void);
u8 ATA_UDmaMode_autotest(void);

const testFuncMenu g_aATATestFunc[]=
{


		ATA_TestChangeModeToAta,			"Change mode to ATA",	
		ATA_TestReset,						"Reset ATA device\n",

		ATA_PrintDeviceInfo,					"Print ATA Device Information",
		ATA_TestBasicWriteRead,				"Basic Write/Read test\n",
		
//		ATA_ClearPioCpuMode,				"PIO_CPU mode Clear test",
		ATA_TestPioCpuMode,				"PIO_CPU mode Write/Read test",
		ATA_TestPioCpuModeRead,			"PIO_CPU mode Read test",		
		ATA_TestPioDmaMode,				"PIO_DMA mode Write/Read test [Polling mode]",
//		ATA_TestPioDmaModeWrite,			"PIO_DMA mode Write test [Polling mode]",
		ATA_TestPioDmaModeRead,			"PIO_DMA mode Read test [Polling mode]",
		ATA_TestUdmaMode,					"UDMA mode Write/Read test [Polling mode]",
		ATA_TestUdmaModeRead,			"UDMA mode Read test [Polling mode]\n",

		ATA_TestPioDmaMode_Int,			"PIO_DMA Write/Read test [Interrupt mode]",
		ATA_TestUDmaMode_Int,				"UDMA Write/Read test [Interrupt mode]\n",

		ATA_AgingTestPioDmaMode,			"PIO_DMA mode Write/Read Aging test [Polling mode]",
		ATA_AgingTestUdmaMode,				"UDMA mode Write/Read Aging test [Polling mode]",		
		ATA_TestUDmaMode_Int_for_AtaBug,		"UDMA Write/Read test for ATA Bug[Interrupt mode]",	

		ATA_AgingTestUdmaModeMasterSlave,	"Master and Slave Slot UDMA mode Write/Read Aging test [Polling mode]",
		ATA_PioCpu_autotest,				"PIOCPU Autotest",
		ATA_PioDma_autotest,				"PIODMA Autotest",
		ATA_UDmaMode_autotest,			"UDMA Autotest",
		0,0
};


//////////
// Function Name : ATA_Test
// Function Description : This function prints out ATA test functions through UART.
// Input : 	NONE
// Output : 	NONE
// Version : v0.1
void ATA_Test(void)
{
	u32 uCountFunc = 0;
	s32	iSel = 0;

//	GPIO_SetFunctionEach(eGPIO_P, eGPIO_11, 1);	// set REGata/GPP11 as output
//	GPIO_SetDataEach(eGPIO_P, eGPIO_11 , 0); 		//GPP[11] -> Low

	GPIO_SetPullUpDownEach(eGPIO_P, eGPIO_14, 2);		//

	UART_Printf("\n\n================== ATA Function Test =====================\n\n");
	UART_Printf("\nSelect the operation mode : 0)Indirect(default)	1)Direct");
	iSel =UART_GetIntNum();
	UART_Printf("\n");
	if(iSel == -1) 
		g_ucOpMode = INDIRECT_MODE;
	else
		g_ucOpMode = (u8)iSel;

	g_ucATAConNum = ATA_CON0; 
	
	if (!ATA_Init(g_ucATAConNum, g_ucOpMode))
	{
		UART_Printf("Fail to initialize ATA Mode...\n");
		return;
	}

//	GPIO_SetFunctionEach(eGPIO_P, eGPIO_11, 2);	// set REGata/GPP11 as REGata

	while(1)
	{
		for (uCountFunc=0; (u32)(g_aATATestFunc[uCountFunc].desc)!=0; uCountFunc++)
			UART_Printf("%2d: %s\n", uCountFunc, g_aATATestFunc[uCountFunc].desc);

		UART_Printf("\nSelect the function to test : ");
		iSel =UART_GetIntNum();
		UART_Printf("\n");
		if(iSel == -1) 
			break;

		if (iSel>=0 && iSel<(sizeof(g_aATATestFunc)/8-1))
			(g_aATATestFunc[iSel].func) ();
	}

//	ATA_ReturnPort(g_ucATAConNum);
	ATA_SetEnable(g_ucATAConNum, DISABLE);
	CF_SetMUXReg(eCF_MUX_OUTPUT_DISABLE, eCF_MUX_CARDPWR_OFF, eCF_MUX_MODE_PCCARD);

}

/*---------------------------------- Test Functions -------------------------------*/
//////////
// Function Name : ATA_TestChangeModeToAta
// Function Description : 
//   This function prints out the information of CF Card.
// Input : NONE
// Output : NONE
// Version : v0.1 
void ATA_TestChangeModeToAta(void)
{
	ATA_ChangeATAMode(g_ucATAConNum);
}

//////////
// Function Name : ATA_TestReset
// Function Description : 
//   This function tests reset function of ATA Controller.
// Input : NONE
// Output : NONE
// Version : v0.1 
void ATA_TestReset(void)
{
	ATA_TestResetAll(g_ucATAConNum);
	if (!ATA_Init(g_ucATAConNum, g_ucOpMode))
	{
		UART_Printf("Fail to initialize ATA Mode...\n");
		return;
	}	
}


void ATA_PrintDeviceInfo()
{
	eATA_SLOT_SEL eSlotsel;
	
	UART_Printf("\n Which slot do you want to check ?  0 = Master slot,  1 = Slave Slot ");
	eSlotsel = UART_GetIntNum();
	UART_Printf("\n");
	while (eSlotsel > 1)
		{
		UART_Printf("\n caution : You chose wrong number!choose again !");		
		UART_Printf("\n Which slot do you want to check ?  0 = Master slot,  1 = Slave Slot ");
		eSlotsel = UART_GetIntNum();
		UART_Printf("\n");
		}
		
	
	ATA_IdentifyDevice(g_ucATAConNum,(eATA_SLOT_SEL)eSlotsel);
}

//////////
// Function Name : ATA_TestBasicWriteRead
// Function Description : 
//   This function tests basic read/write function of ATA.
// Input : NONE
// Output : NONE
// Version : v0.1 
void ATA_TestBasicWriteRead(void)
{
	u32		uDeviceLBA	= 0;
	u32		uSector		= 0;
	u32		uLoopCnt 	= 0;

	ATA_OpenMedia(g_ucATAConNum, eATA_MODE_NONE,eATA_MASTER);

	UART_Printf("\nInput device sector address[max: 0x%x]\n",g_oaATAInform[g_ucATAConNum].uMaxSectors);
	uDeviceLBA = (u32)UART_GetIntNum();

	UART_Printf("Input sector count[max: 0x%x]\n",g_oaATAInform[g_ucATAConNum].uMaxSectors - uDeviceLBA);
	uSector = (u32)UART_GetIntNum();

	for (uLoopCnt=0; uLoopCnt < uSector*512; uLoopCnt++)
	{
		CF_SetMem(eBYTE, (u32)((u8*)g_oaATAInform[g_ucATAConNum].puATAReadBuf + uLoopCnt), 0);			//clear read buffer as 0
		CF_SetMem(eBYTE, (u32)((u8*)g_oaATAInform[g_ucATAConNum].puATAWriteBuf + uLoopCnt), (uLoopCnt+0)%256);	// set values on write buffer (0~0xff)
	}

	UART_Printf("Press Enter \n");	
	UART_Getc();

	ATA_WriteBlocks(g_ucATAConNum, uDeviceLBA, uSector, (u32)(g_oaATAInform[g_ucATAConNum].puATAWriteBuf),eATA_MASTER);
	ATA_ReadBlocks(g_ucATAConNum, uDeviceLBA, uSector, (u32)(g_oaATAInform[g_ucATAConNum].puATAReadBuf),eATA_MASTER);

	if (Compare((u32)(g_oaATAInform[g_ucATAConNum].puATAWriteBuf), (u32)(g_oaATAInform[g_ucATAConNum].puATAReadBuf), uSector*128) == FALSE)
	{
		UART_Printf("Error detected\n");
		Dump32((u32)(g_oaATAInform[g_ucATAConNum].puATAReadBuf), uSector*128);
	}
	else
	{
		UART_Printf("Write/Read operation is OK\n");
	}
	ATA_CloseMedia(g_ucATAConNum);
}

//////////
// Function Name : ATA_TestPioCpuMode
// Function Description : 
//   This function tests  function of ATA.
// Input : NONE
// Output : NONE
// Version : v0.1 
void ATA_TestPioCpuMode(void)
{
	u32		uDeviceLBA	= 0;
	u32		uSector		= 0;
	u32		uLoopCnt 	= 0;
	u32		uSlotSel		= 0;

	UART_Printf("Which Slot do you want to check? \n");
	UART_Printf("[0] = Slot 0 ,    [1] = Slot 1 \n");
	uSlotSel = UART_GetIntNum();
	UART_Printf("\n");

	if(uSlotSel == 1)
	ATA_OpenMedia(g_ucATAConNum, eATA_MODE_PIOCPU, eATA_SLAVE);
	else
	ATA_OpenMedia(g_ucATAConNum, eATA_MODE_PIOCPU, eATA_MASTER);


	UART_Printf("\nInput device sector address[max: 0x%x]\n",g_oaATAInform[g_ucATAConNum].uMaxSectors);
	uDeviceLBA = (u32)UART_GetIntNum();

	UART_Printf("Input sector count[max: 0x%x]\n",g_oaATAInform[g_ucATAConNum].uMaxSectors - uDeviceLBA);
	uSector = (u32)UART_GetIntNum();

	for (uLoopCnt=0; uLoopCnt < uSector*512; uLoopCnt++)
	{
		CF_SetMem(eBYTE, (u32)((u8*)g_oaATAInform[g_ucATAConNum].puATAReadBuf + uLoopCnt), 0);
		CF_SetMem(eBYTE, (u32)((u8*)g_oaATAInform[g_ucATAConNum].puATAWriteBuf + uLoopCnt), (uLoopCnt+1)%256);
	}

	UART_Printf("Press Enter \n");	
	UART_Getc();

	if(uSlotSel == 1)
	{
		ATA_WriteBlocks(g_ucATAConNum, uDeviceLBA, uSector, (u32)(g_oaATAInform[g_ucATAConNum].puATAWriteBuf), eATA_SLAVE);
		ATA_ReadBlocks(g_ucATAConNum, uDeviceLBA, uSector, (u32)(g_oaATAInform[g_ucATAConNum].puATAReadBuf), eATA_SLAVE);
	}
	else
	{
		ATA_WriteBlocks(g_ucATAConNum, uDeviceLBA, uSector, (u32)(g_oaATAInform[g_ucATAConNum].puATAWriteBuf), eATA_MASTER);
		ATA_ReadBlocks(g_ucATAConNum, uDeviceLBA, uSector, (u32)(g_oaATAInform[g_ucATAConNum].puATAReadBuf), eATA_MASTER);		
	}
	
	if (Compare((u32)(g_oaATAInform[g_ucATAConNum].puATAWriteBuf), (u32)(g_oaATAInform[g_ucATAConNum].puATAReadBuf), uSector*128) == FALSE)
	{
		UART_Printf("Error detected\n");
		Dump32((u32)(g_oaATAInform[g_ucATAConNum].puATAReadBuf), uSector*128);
	}
	else
	{
		UART_Printf("Write/Read operation is OK\n");
	}
	ATA_CloseMedia(g_ucATAConNum);
}

//////////
// Function Name : ATA_ClearPioCpuMode
// Function Description : 
//   This function tests  function of ATA.
// Input : NONE
// Output : NONE
// Version : v0.1 
void ATA_ClearPioCpuMode(void)
{
	u32		uDeviceLBA	= 0;
	u32		uSector		= 0;
	u32		uLoopCnt 	= 0;
	u32		uSlotSel		= 0;

	UART_Printf("Which Slot do you want to check? \n");
	UART_Printf("[0] = Slot 0 ,    [1] = Slot 1 \n");
	uSlotSel = UART_GetIntNum();
	UART_Printf("\n");

	if(uSlotSel == 1)
	ATA_OpenMedia(g_ucATAConNum, eATA_MODE_PIOCPU, eATA_SLAVE);
	else
	ATA_OpenMedia(g_ucATAConNum, eATA_MODE_PIOCPU, eATA_MASTER);

	UART_Printf("\nInput device sector address[max: 0x%x]\n",g_oaATAInform[g_ucATAConNum].uMaxSectors);
	uDeviceLBA = (u32)UART_GetIntNum();

	UART_Printf("Input sector count[max: 0x%x]\n",g_oaATAInform[g_ucATAConNum].uMaxSectors - uDeviceLBA);
	uSector = (u32)UART_GetIntNum();

	for (uLoopCnt=0; uLoopCnt < uSector*512; uLoopCnt++)
	{
		CF_SetMem(eBYTE, (u32)((u8*)g_oaATAInform[g_ucATAConNum].puATAReadBuf + uLoopCnt), 0);
		CF_SetMem(eBYTE, (u32)((u8*)g_oaATAInform[g_ucATAConNum].puATAWriteBuf + uLoopCnt), 0);
	}

	UART_Printf("Press Enter \n");	
	UART_Getc();
	if(uSlotSel == 1)
	ATA_WriteBlocks(g_ucATAConNum, uDeviceLBA, uSector, (u32)(g_oaATAInform[g_ucATAConNum].puATAWriteBuf), eATA_SLAVE);
	else
	ATA_WriteBlocks(g_ucATAConNum, uDeviceLBA, uSector, (u32)(g_oaATAInform[g_ucATAConNum].puATAWriteBuf), eATA_SLAVE);

	ATA_CloseMedia(g_ucATAConNum);

	UART_Printf("CF Mem Clear has been completed.\n");
}

//////////
// Function Name : ATA_TestPioCpuModeRead
// Function Description : 
//   This function tests  function of ATA.
// Input : NONE
// Output : NONE
// Version : v0.1 
void ATA_TestPioCpuModeRead(void)
{
	u32		uDeviceLBA	= 0;
	u32		uSector		= 0;
	u32		uLoopCnt 	= 0;
	u32		uSlotSel		= 0;

	UART_Printf("Which Slot do you want to check? \n");
	UART_Printf("[0] = Slot 0 ,    [1] = Slot 1 \n");
	uSlotSel = UART_GetIntNum();
	UART_Printf("\n");

	if(uSlotSel == 1)
	ATA_OpenMedia(g_ucATAConNum, eATA_MODE_PIOCPU, eATA_SLAVE);
	else
	ATA_OpenMedia(g_ucATAConNum, eATA_MODE_PIOCPU, eATA_MASTER);


	UART_Printf("\nInput device sector address[max: 0x%x]\n",g_oaATAInform[g_ucATAConNum].uMaxSectors);
	uDeviceLBA = (u32)UART_GetIntNum();

	UART_Printf("Input sector count[max: 0x%x]\n",g_oaATAInform[g_ucATAConNum].uMaxSectors - uDeviceLBA);
	uSector = (u32)UART_GetIntNum();

	for (uLoopCnt=0; uLoopCnt < uSector*512; uLoopCnt++)
	{
		CF_SetMem(eBYTE, (u32)((u8*)g_oaATAInform[g_ucATAConNum].puATAReadBuf + uLoopCnt), 0);
	}

	UART_Printf("Press Enter \n");	
	UART_Getc();

	if(uSlotSel == 1)
	ATA_ReadBlocks(g_ucATAConNum, uDeviceLBA, uSector, (u32)(g_oaATAInform[g_ucATAConNum].puATAReadBuf), eATA_SLAVE);
	else
	ATA_ReadBlocks(g_ucATAConNum, uDeviceLBA, uSector, (u32)(g_oaATAInform[g_ucATAConNum].puATAReadBuf), eATA_MASTER);

	Dump32((u32)(g_oaATAInform[g_ucATAConNum].puATAReadBuf), uSector*128);

	ATA_CloseMedia(g_ucATAConNum);
}

//////////
// Function Name : ATA_TestPioDmaMode
// Function Description : 
//   This function tests  function of ATA.
// Input : NONE
// Output : NONE
// Version : v0.1 
void ATA_TestPioDmaMode(void)
{
	u32		uDeviceLBA	= 0;
	u32		uSector		= 0;
	u32		uLoopCnt 	= 0;
	u32		uSlotSel		= 0;

	UART_Printf("Which Slot do you want to check? \n");
	UART_Printf("[0] = Slot 0 ,    [1] = Slot 1 \n");
	uSlotSel = UART_GetIntNum();
	UART_Printf("\n");

	if(uSlotSel == 1)
	ATA_OpenMedia(g_ucATAConNum, eATA_MODE_PIODMA, eATA_SLAVE);
	else
	ATA_OpenMedia(g_ucATAConNum, eATA_MODE_PIODMA, eATA_MASTER);

	UART_Printf("\nInput device sector address[max: 0x%x]\n",g_oaATAInform[g_ucATAConNum].uMaxSectors);
	uDeviceLBA = (u32)UART_GetIntNum();

	UART_Printf("Input sector count[max: 0x%x]\n",g_oaATAInform[g_ucATAConNum].uMaxSectors - uDeviceLBA);
	uSector = (u32)UART_GetIntNum();

	for (uLoopCnt=0; uLoopCnt < uSector*512; uLoopCnt++)
	{
		CF_SetMem(eBYTE, (u32)((u8*)g_oaATAInform[g_ucATAConNum].puATAReadBuf + uLoopCnt), 0);
		CF_SetMem(eBYTE, (u32)((u8*)g_oaATAInform[g_ucATAConNum].puATAWriteBuf + uLoopCnt), (uLoopCnt+2)%256);
	}

	UART_Printf("Press Enter \n");	
	UART_Getc();

	if(uSlotSel == 1)
	{
	ATA_WriteBlocks(g_ucATAConNum, uDeviceLBA, uSector, (u32)(g_oaATAInform[g_ucATAConNum].puATAWriteBuf), eATA_SLAVE);
	ATA_ReadBlocks(g_ucATAConNum, uDeviceLBA, uSector, (u32)(g_oaATAInform[g_ucATAConNum].puATAReadBuf), eATA_SLAVE);
	}
	else
	{
	ATA_WriteBlocks(g_ucATAConNum, uDeviceLBA, uSector, (u32)(g_oaATAInform[g_ucATAConNum].puATAWriteBuf), eATA_MASTER);
	ATA_ReadBlocks(g_ucATAConNum, uDeviceLBA, uSector, (u32)(g_oaATAInform[g_ucATAConNum].puATAReadBuf), eATA_MASTER);
	}
	
	if (Compare((u32)(g_oaATAInform[g_ucATAConNum].puATAWriteBuf), (u32)(g_oaATAInform[g_ucATAConNum].puATAReadBuf), uSector*128) == FALSE)
	{
		UART_Printf("Error detected\n");
		Dump32((u32)(g_oaATAInform[g_ucATAConNum].puATAReadBuf), uSector*128);
	}
	else
	{
		UART_Printf("Write/Read operation is OK\n");
	}
	ATA_CloseMedia(g_ucATAConNum);
}

//////////
// Function Name : ATA_AgingTestPioDmaMode
// Function Description : 
//   This function tests  PioDMA aging.
// Input : NONE
// Output : NONE
// Version : v0.1 
void ATA_AgingTestPioDmaMode(void)
{
	u32		uDeviceLBA	= 0;
	u32		uSector		= 0;
	u32		uLoopCnt 	= 0;
	u32		uSlotSel		= 0;

	UART_Printf("Which Slot do you want to check? \n");
	UART_Printf("[0] = Slot 0 ,    [1] = Slot 1 \n");
	uSlotSel = UART_GetIntNum();
	UART_Printf("\n");

	if(uSlotSel == 1)
	ATA_OpenMedia(g_ucATAConNum, eATA_MODE_PIODMA, eATA_SLAVE);
	else
	ATA_OpenMedia(g_ucATAConNum, eATA_MODE_PIODMA, eATA_MASTER);

	while(1)
	{
		ATA_InitBuffer(g_ucATAConNum);
		uDeviceLBA = 0;

		uSector = 100;

		for (uLoopCnt=0; uLoopCnt < uSector*512; uLoopCnt++)
		{
			CF_SetMem(eBYTE, (u32)((u8*)g_oaATAInform[g_ucATAConNum].puATAReadBuf + uLoopCnt), 0);
			CF_SetMem(eBYTE, (u32)((u8*)g_oaATAInform[g_ucATAConNum].puATAWriteBuf + uLoopCnt), (uLoopCnt+2)%256);
		}

		if(uSlotSel == 1)
		{
		ATA_WriteBlocks(g_ucATAConNum, uDeviceLBA, uSector, (u32)(g_oaATAInform[g_ucATAConNum].puATAWriteBuf), eATA_SLAVE);
		ATA_ReadBlocks(g_ucATAConNum, uDeviceLBA, uSector, (u32)(g_oaATAInform[g_ucATAConNum].puATAReadBuf), eATA_SLAVE);
		}
		else
		{
		ATA_WriteBlocks(g_ucATAConNum, uDeviceLBA, uSector, (u32)(g_oaATAInform[g_ucATAConNum].puATAWriteBuf), eATA_MASTER);
		ATA_ReadBlocks(g_ucATAConNum, uDeviceLBA, uSector, (u32)(g_oaATAInform[g_ucATAConNum].puATAReadBuf), eATA_MASTER);
		}

		if (Compare((u32)(g_oaATAInform[g_ucATAConNum].puATAWriteBuf), (u32)(g_oaATAInform[g_ucATAConNum].puATAReadBuf), uSector*128) == FALSE)
		{
			UART_Printf("Error detected\n\n");
			Dump32((u32)(g_oaATAInform[g_ucATAConNum].puATAReadBuf), uSector*128);
			break;
		}
		else
		{
			UART_Printf("Write/Read operation is OK\n\n");
		}
	}
	ATA_CloseMedia(g_ucATAConNum);
}

//////////
// Function Name : ATA_TestPioDmaModeWrite
// Function Description : 
//   This function tests  function of ATA.
// Input : NONE
// Output : NONE
// Version : v0.1 
void ATA_TestPioDmaModeWrite(void)
{
	u32		uDeviceLBA	= 0;
	u32		uSector		= 0;
	u32		uLoopCnt 	= 0;
	u32		uSlotSel		= 0;

	UART_Printf("Which Slot do you want to check? \n");
	UART_Printf("[0] = Slot 0 ,    [1] = Slot 1 \n");
	uSlotSel = UART_GetIntNum();
	UART_Printf("\n");

	if(uSlotSel == 1)
	ATA_OpenMedia(g_ucATAConNum, eATA_MODE_PIODMA, eATA_SLAVE);
	else
	ATA_OpenMedia(g_ucATAConNum, eATA_MODE_PIODMA, eATA_MASTER);
	
	UART_Printf("\nInput device sector address[max: 0x%x]\n",g_oaATAInform[g_ucATAConNum].uMaxSectors);
	uDeviceLBA = (u32)UART_GetIntNum();

	UART_Printf("Input sector count[max: 0x%x]\n",g_oaATAInform[g_ucATAConNum].uMaxSectors - uDeviceLBA);
	uSector = (u32)UART_GetIntNum();

	for (uLoopCnt=0; uLoopCnt < uSector*512; uLoopCnt++)
	{
		CF_SetMem(eBYTE, (u32)((u8*)g_oaATAInform[g_ucATAConNum].puATAReadBuf + uLoopCnt), 0);
		CF_SetMem(eBYTE, (u32)((u8*)g_oaATAInform[g_ucATAConNum].puATAWriteBuf + uLoopCnt), (uLoopCnt+2)%256);
	}

	UART_Printf("Press Enter \n");	
	UART_Getc();

	if(uSlotSel == 1)
	ATA_WriteBlocks(g_ucATAConNum, uDeviceLBA, uSector, (u32)(g_oaATAInform[g_ucATAConNum].puATAWriteBuf), eATA_SLAVE);
	else
	ATA_WriteBlocks(g_ucATAConNum, uDeviceLBA, uSector, (u32)(g_oaATAInform[g_ucATAConNum].puATAWriteBuf), eATA_MASTER);
	
	ATA_CloseMedia(g_ucATAConNum);
	UART_Printf("To Write to CF Memory has been successful.\n");
}

//////////
// Function Name : ATA_TestPioDmaModeRead
// Function Description : 
//   This function tests  function of ATA.
// Input : NONE
// Output : NONE
// Version : v0.1 
void	ATA_TestPioDmaModeRead(void)
{
	u32		uDeviceLBA	= 0;
	u32		uSector		= 0;
	u32		uLoopCnt 	= 0;
	u32		uSlotSel		= 0;

	UART_Printf("Which Slot do you want to check? \n");
	UART_Printf("[0] = Slot 0 ,    [1] = Slot 1 \n");
	uSlotSel = UART_GetIntNum();
	UART_Printf("\n");

	if(uSlotSel == 1)
	ATA_OpenMedia(g_ucATAConNum, eATA_MODE_PIODMA, eATA_SLAVE);
	else
	ATA_OpenMedia(g_ucATAConNum, eATA_MODE_PIODMA, eATA_MASTER);

	UART_Printf("\nInput device sector address[max: 0x%x]\n",g_oaATAInform[g_ucATAConNum].uMaxSectors);
	uDeviceLBA = (u32)UART_GetIntNum();

	UART_Printf("Input sector count[max: 0x%x]\n",g_oaATAInform[g_ucATAConNum].uMaxSectors - uDeviceLBA);
	uSector = (u32)UART_GetIntNum();

	for (uLoopCnt=0; uLoopCnt < uSector*512; uLoopCnt++)
	{
		CF_SetMem(eBYTE, (u32)((u8*)g_oaATAInform[g_ucATAConNum].puATAReadBuf + uLoopCnt), 0);
	}

	UART_Printf("Press Enter \n");	
	UART_Getc();

	if(uSlotSel == 1)
	ATA_ReadBlocks(g_ucATAConNum, uDeviceLBA, uSector, (u32)(g_oaATAInform[g_ucATAConNum].puATAReadBuf), eATA_SLAVE);
	else
	ATA_ReadBlocks(g_ucATAConNum, uDeviceLBA, uSector, (u32)(g_oaATAInform[g_ucATAConNum].puATAReadBuf), eATA_MASTER);

	Dump32((u32)(g_oaATAInform[g_ucATAConNum].puATAReadBuf), uSector*128);

	ATA_CloseMedia(g_ucATAConNum);
}

//////////
// Function Name : ATA_TestUdmaMode
// Function Description : 
//   This function tests  function of ATA.
// Input : NONE
// Output : NONE
// Version : v0.1 
void ATA_TestUdmaMode(void)
{
	u32		uDeviceLBA	= 0;
	u32		uSector		= 0;
	u32		uLoopCnt 	= 0;
	u32		uSlotSel		= 0;

	UART_Printf("Which Slot do you want to check? \n");
	UART_Printf("[0] = Slot 0 ,    [1] = Slot 1 \n");
	uSlotSel = UART_GetIntNum();
	UART_Printf("\n");

	if(uSlotSel == 1)
	ATA_OpenMedia(g_ucATAConNum, eATA_MODE_UDMA, eATA_SLAVE);
	else
	ATA_OpenMedia(g_ucATAConNum, eATA_MODE_UDMA, eATA_MASTER);
	
	UART_Printf("\nInput device sector address[max: 0x%x]\n",g_oaATAInform[g_ucATAConNum].uMaxSectors);
	uDeviceLBA = (u32)UART_GetIntNum();

	UART_Printf("Input sector count[max: 0x%x]\n",g_oaATAInform[g_ucATAConNum].uMaxSectors - uDeviceLBA);
	uSector = (u32)UART_GetIntNum();

	for (uLoopCnt=0; uLoopCnt < uSector*512; uLoopCnt++)
	{
		CF_SetMem(eBYTE, (u32)((u8*)g_oaATAInform[g_ucATAConNum].puATAReadBuf + uLoopCnt), 0);
		CF_SetMem(eBYTE, (u32)((u8*)g_oaATAInform[g_ucATAConNum].puATAWriteBuf + uLoopCnt), (uLoopCnt+3)%256);
	}

	UART_Printf("Press Enter \n");	
	UART_Getc();
	
	if(uSlotSel == 1)
	{
		ATA_WriteBlocks(g_ucATAConNum, uDeviceLBA, uSector, (u32)(g_oaATAInform[g_ucATAConNum].puATAWriteBuf), eATA_SLAVE);
		ATA_ReadBlocks(g_ucATAConNum, uDeviceLBA, uSector, (u32)(g_oaATAInform[g_ucATAConNum].puATAReadBuf), eATA_SLAVE);
	}
	else
	{
		ATA_WriteBlocks(g_ucATAConNum, uDeviceLBA, uSector, (u32)(g_oaATAInform[g_ucATAConNum].puATAWriteBuf), eATA_MASTER);
		ATA_ReadBlocks(g_ucATAConNum, uDeviceLBA, uSector, (u32)(g_oaATAInform[g_ucATAConNum].puATAReadBuf), eATA_MASTER);		
	}
	
	if (Compare((u32)(g_oaATAInform[g_ucATAConNum].puATAWriteBuf), (u32)(g_oaATAInform[g_ucATAConNum].puATAReadBuf), uSector*128) == FALSE)
	{
		UART_Printf("Error detected\n");
		Dump32((u32)(g_oaATAInform[g_ucATAConNum].puATAReadBuf), uSector*128);
	}
	else
	{
		UART_Printf("Write/Read operation in UDMA mode is OK\n");
	}
	ATA_CloseMedia(g_ucATAConNum);
}

//////////
// Function Name : ATA_AgingTestUdmaMode
// Function Description : 
//   This function tests  function of ATA.
// Input : NONE
// Output : NONE
// Version : v0.1 
void ATA_AgingTestUdmaMode(void)
{
	u32		uDeviceLBA	= 0;		// start sector
	u32		uSector		= 0;		// sector count
	u32		uLoopCnt 	= 0;
	u32		uMaxSectors	= 0;
	u32		uRemainSectors = 0;
	u32		uSlotSel		= 0;

	UART_Printf("Which Slot do you want to check? \n");
	UART_Printf("[0] = Slot 0 ,    [1] = Slot 1 \n");
	uSlotSel = UART_GetIntNum();
	UART_Printf("\n");

	if(uSlotSel == 1)
	ATA_OpenMedia(g_ucATAConNum, eATA_MODE_UDMA, eATA_SLAVE);
	else
	ATA_OpenMedia(g_ucATAConNum, eATA_MODE_UDMA, eATA_MASTER);

	uMaxSectors 		= g_oaATAInform[g_ucATAConNum].uMaxSectors;

	UART_Printf(" ::::: UDMA Aging Test Mode :::::\n");
	UART_Printf(" > MaxSectors : %d sectors\n", uMaxSectors);
	uRemainSectors	= uMaxSectors;
	uSector			= 4096;	// 4096sectors(4096*512=2,097,152Bytes) per one time
	while(uRemainSectors>0)
	{
		if ( uRemainSectors < uSector )
			uSector = uRemainSectors;
		
		for (uLoopCnt=0; uLoopCnt < uSector*512; uLoopCnt++)
		{
			CF_SetMem(eBYTE, (u32)((u8*)g_oaATAInform[g_ucATAConNum].puATAReadBuf + uLoopCnt), 0);
			CF_SetMem(eBYTE, (u32)((u8*)g_oaATAInform[g_ucATAConNum].puATAWriteBuf + uLoopCnt), (uLoopCnt+3)%256);
		}

		if(uSlotSel == 1)
		{
			ATA_WriteBlocks(g_ucATAConNum, uDeviceLBA, uSector, (u32)(g_oaATAInform[g_ucATAConNum].puATAWriteBuf), eATA_SLAVE);
			ATA_ReadBlocks(g_ucATAConNum, uDeviceLBA, uSector, (u32)(g_oaATAInform[g_ucATAConNum].puATAReadBuf), eATA_SLAVE);
		}
		else
		{
			ATA_WriteBlocks(g_ucATAConNum, uDeviceLBA, uSector, (u32)(g_oaATAInform[g_ucATAConNum].puATAWriteBuf), eATA_MASTER);
			ATA_ReadBlocks(g_ucATAConNum, uDeviceLBA, uSector, (u32)(g_oaATAInform[g_ucATAConNum].puATAReadBuf), eATA_MASTER);		
		}
	
		if (Compare((u32)(g_oaATAInform[g_ucATAConNum].puATAWriteBuf), (u32)(g_oaATAInform[g_ucATAConNum].puATAReadBuf), uSector*128) == FALSE)
		{
			UART_Printf("Error detected\n");
			Dump32((u32)(g_oaATAInform[g_ucATAConNum].puATAReadBuf), uSector*128);
			break;
		}
		else
		{
			UART_Printf("Write/Read operation at (%d ~ %d) sectors in UDMA mode is OK\n", uDeviceLBA, (uDeviceLBA+uSector-1));
		}

		uDeviceLBA += uSector;
		uRemainSectors -= uSector;
		UART_Printf("-----------------------------------------------------\n");	
	}

	ATA_CloseMedia(g_ucATAConNum);
}



//////////
// Function Name : ATA_AgingTestUdmaMode
// Function Description : 
//   This function tests  function of ATA.
// Input : NONE
// Output : NONE
// Version : v0.1 
void ATA_AgingTestUdmaModeMasterSlave(void)
{
	u32		uDeviceLBAMaster	= 0;		// start sector
	u32		uDeviceLBASlave	= 0;		// start sector	
	u32		uSectorMaster		= 0;		// sector count
	u32		uSectorSlave		= 0;		// sector count	
	u32		uLoopCnt 	= 0;
	u32		uMaxSectorsMaster	= 0;
	u32		uMaxSectorsSlave	= 0;	
	u32		uRemainSectorsMaster = 0;
	u32		uRemainSectorsSlave = 0;	

	ATA_OpenMedia(g_ucATAConNum, eATA_MODE_NONE, eATA_MASTER);
	uMaxSectorsMaster 	= g_oaATAInform[g_ucATAConNum].uMaxSectors;

	ATA_OpenMedia(g_ucATAConNum, eATA_MODE_NONE, eATA_SLAVE);
	uMaxSectorsSlave 		= g_oaATAInform[g_ucATAConNum].uMaxSectors;


	UART_Printf(" ::::: UDMA Aging Test Mode :::::\n");
	UART_Printf(" > MaxSectorsMaster : %d sectors\n", uMaxSectorsMaster);
	UART_Printf(" > MaxSectorsSlave : %d sectors\n", uMaxSectorsSlave);	
	
	uRemainSectorsMaster	= uMaxSectorsMaster;
	uRemainSectorsSlave	= uMaxSectorsSlave;
	uSectorMaster			= 4096;	// 4096sectors(4096*512=2,097,152Bytes) per one time
	uSectorSlave			= 4096;	// 4096sectors(4096*512=2,097,152Bytes) per one time

	while(1)
	{

		ATA_OpenMedia(g_ucATAConNum, eATA_MODE_PIODMA, eATA_MASTER);
//		ATA_OpenMedia(g_ucATAConNum, eATA_MODE_NONE, eATA_MASTER);
		uMaxSectorsMaster 	= g_oaATAInform[g_ucATAConNum].uMaxSectors;

		if ( uRemainSectorsMaster < uSectorMaster )
			uSectorMaster = uRemainSectorsMaster;
		
		for (uLoopCnt=0; uLoopCnt < uSectorMaster*512; uLoopCnt++)
		{
			CF_SetMem(eBYTE, (u32)((u8*)g_oaATAInform[g_ucATAConNum].puATAReadBuf + uLoopCnt), 0);
			CF_SetMem(eBYTE, (u32)((u8*)g_oaATAInform[g_ucATAConNum].puATAWriteBuf + uLoopCnt), (uLoopCnt+3)%256);
		}

		ATA_WriteBlocks(g_ucATAConNum, uDeviceLBAMaster, uSectorMaster, (u32)(g_oaATAInform[g_ucATAConNum].puATAWriteBuf), eATA_MASTER);
		ATA_ReadBlocks(g_ucATAConNum, uDeviceLBAMaster, uSectorMaster, (u32)(g_oaATAInform[g_ucATAConNum].puATAReadBuf), eATA_MASTER);

		if (Compare((u32)(g_oaATAInform[g_ucATAConNum].puATAWriteBuf), (u32)(g_oaATAInform[g_ucATAConNum].puATAReadBuf), uSectorMaster*128) == FALSE)
		{
			UART_Printf("Error detected\n");
			Dump32((u32)(g_oaATAInform[g_ucATAConNum].puATAReadBuf), uSectorMaster*128);
			break;
		}
		else
		{
			UART_Printf("Write/Read operation at (%d ~ %d) sectors in UDMA mode is OK\n", uDeviceLBAMaster, (uDeviceLBAMaster+uSectorMaster-1));
		}

		uDeviceLBAMaster += uSectorMaster;
		uRemainSectorsMaster -= uSectorMaster;

		if (uRemainSectorsMaster == 0)
		{
			uRemainSectorsMaster	= uMaxSectorsMaster;
			uDeviceLBAMaster	= 0;
		}
		UART_Printf("-----------------------------------------------------\n");	

		ATA_OpenMedia(g_ucATAConNum, eATA_MODE_PIODMA, eATA_SLAVE);
		uMaxSectorsSlave 		= g_oaATAInform[g_ucATAConNum].uMaxSectors;


		if ( uRemainSectorsSlave <= uSectorSlave )
			uSectorSlave = uRemainSectorsSlave;
		
		for (uLoopCnt=0; uLoopCnt < uSectorSlave*512; uLoopCnt++)
		{
			CF_SetMem(eBYTE, (u32)((u8*)g_oaATAInform[g_ucATAConNum].puATAReadBuf + uLoopCnt), 0);
			CF_SetMem(eBYTE, (u32)((u8*)g_oaATAInform[g_ucATAConNum].puATAWriteBuf + uLoopCnt), (uLoopCnt+3)%256);
		}

		ATA_WriteBlocks(g_ucATAConNum, uDeviceLBASlave, uSectorSlave, (u32)(g_oaATAInform[g_ucATAConNum].puATAWriteBuf), eATA_SLAVE);
		ATA_ReadBlocks(g_ucATAConNum, uDeviceLBASlave, uSectorSlave, (u32)(g_oaATAInform[g_ucATAConNum].puATAReadBuf), eATA_SLAVE);

		if (Compare((u32)(g_oaATAInform[g_ucATAConNum].puATAWriteBuf), (u32)(g_oaATAInform[g_ucATAConNum].puATAReadBuf), uSectorSlave*128) == FALSE)
		{
			UART_Printf("Error detected\n");
			Dump32((u32)(g_oaATAInform[g_ucATAConNum].puATAReadBuf), uSectorSlave*128);
			break;
		}
		else
		{
			UART_Printf("Write/Read operation at (%d ~ %d) sectors in UDMA mode is OK\n", uDeviceLBASlave, (uDeviceLBASlave+uSectorSlave-1));
		}

		uDeviceLBASlave += uSectorSlave;
		uRemainSectorsSlave -= uSectorSlave;

		if (uRemainSectorsSlave == 0)
		{
			uRemainSectorsSlave	= uMaxSectorsSlave;
			uDeviceLBASlave = 0 ;
		}
		UART_Printf("-----------------------------------------------------\n");	



	}

	ATA_CloseMedia(g_ucATAConNum);
}

//////////
// Function Name : ATA_TestUdmaModeRead
// Function Description : 
//   This function tests  function of ATA.
// Input : NONE
// Output : NONE
// Version : v0.1 
void	ATA_TestUdmaModeRead(void)
{
	u32		uDeviceLBA	= 0;
	u32		uSector		= 0;
	u32		uLoopCnt 	= 0;
	u32		uSlotSel		= 0;

	UART_Printf("Which Slot do you want to check? \n");
	UART_Printf("[0] = Slot 0 ,    [1] = Slot 1 \n");
	uSlotSel = UART_GetIntNum();
	UART_Printf("\n");

	if(uSlotSel == 1)
	ATA_OpenMedia(g_ucATAConNum, eATA_MODE_UDMA, eATA_SLAVE);
	else
	ATA_OpenMedia(g_ucATAConNum, eATA_MODE_UDMA, eATA_MASTER);
	
	UART_Printf("\nInput device sector address[max: 0x%x]\n",g_oaATAInform[g_ucATAConNum].uMaxSectors);
	uDeviceLBA = (u32)UART_GetIntNum();

	UART_Printf("Input sector count[max: 0x%x]\n",g_oaATAInform[g_ucATAConNum].uMaxSectors - uDeviceLBA);
	uSector = (u32)UART_GetIntNum();

	for (uLoopCnt=0; uLoopCnt < uSector*512; uLoopCnt++)
	{
		CF_SetMem(eBYTE, (u32)((u8*)g_oaATAInform[g_ucATAConNum].puATAReadBuf + uLoopCnt), 0);
	}

	UART_Printf("Press Enter \n");	
	UART_Getc();

	if(uSlotSel == 1)
	ATA_ReadBlocks(g_ucATAConNum, uDeviceLBA, uSector, (u32)(g_oaATAInform[g_ucATAConNum].puATAReadBuf), eATA_SLAVE);
	else
	ATA_ReadBlocks(g_ucATAConNum, uDeviceLBA, uSector, (u32)(g_oaATAInform[g_ucATAConNum].puATAReadBuf), eATA_MASTER);

	Dump32((u32)(g_oaATAInform[g_ucATAConNum].puATAReadBuf), uSector*128);

	ATA_CloseMedia(g_ucATAConNum);
}

void __irq Isr_AtaDma(void) 
{
	u32 uNthBit;
	u8 bStatus;

	if (!ATA_FindInterruptRequest(g_ucATAConNum,&uNthBit)) {
		INTC_ClearVectAddr();
	}

	UART_Printf("ISR Ata : %d\n", uNthBit);

#if 0	// These lines are just for ATA Bug Test
	ATA_SetTransferCommand(g_ucATAConNum, eATA_XFR_CMD_STOP);
	ATA_SetTransferCommand(g_ucATAConNum, eATA_XFR_CMD_CONTINUE);
#endif	

	ATA_ClearPending(g_ucATAConNum, uNthBit);

	if (uNthBit == 0 || uNthBit == 1)
	{
		ATA_IsDmaDone(g_ucATAConNum);
		g_oaATAInform[g_ucATAConNum].bIsXferDone = TRUE;
	}
	else if (uNthBit == 2)
	{
		g_oaATAInform[g_ucATAConNum].bIsDone = TRUE;
	}


	INTC_ClearVectAddr();

	if (uNthBit == 3)
	{
		bStatus = ATA_IsTBufFullContinue(g_ucATAConNum);
		if (bStatus) 
			ATA_SetTransferCommand(g_ucATAConNum, eATA_XFR_CMD_CONTINUE);
	}
	else if (uNthBit == 4)
	{
		bStatus = ATA_IsSBufEmptyContinue(g_ucATAConNum);
		if (bStatus) 
			ATA_SetTransferCommand(g_ucATAConNum, eATA_XFR_CMD_CONTINUE);
	}

}
//////////
// Function Name : ATA_TestPioDmaMode_Int
// Function Description : 
//   This function tests  function of ATA.
// Input : NONE
// Output : NONE
// Version : v0.1 
void ATA_TestPioDmaMode_Int(void)
{
	u32		uDeviceLBA	= 0;
	u32		uSector		= 0;
	u32		uLoopCnt 	= 0;
	u32		uSlotSel 	= 0;
	
	UART_Printf("Which Slot do you want to check? \n");
	UART_Printf("[0] = Slot 0 ,    [1] = Slot 1 \n");
	uSlotSel = UART_GetIntNum();
	UART_Printf("\n");

	if(uSlotSel == 1)
	ATA_OpenMedia(g_ucATAConNum, eATA_MODE_PIODMA, eATA_SLAVE);
	else
	ATA_OpenMedia(g_ucATAConNum, eATA_MODE_PIODMA, eATA_MASTER);

	UART_Printf("\nInput device sector address[max: 0x%x]\n",g_oaATAInform[g_ucATAConNum].uMaxSectors);
	uDeviceLBA = (u32)UART_GetIntNum();

	UART_Printf("Input sector count[max: 0x%x]\n",g_oaATAInform[g_ucATAConNum].uMaxSectors - uDeviceLBA);
	uSector = (u32)UART_GetIntNum();

	for (uLoopCnt=0; uLoopCnt < uSector*512; uLoopCnt++)
	{
		CF_SetMem(eBYTE, (u32)((u8*)g_oaATAInform[g_ucATAConNum].puATAReadBuf + uLoopCnt), 0);
		CF_SetMem(eBYTE, (u32)((u8*)g_oaATAInform[g_ucATAConNum].puATAWriteBuf + uLoopCnt), (uLoopCnt+4)%256);
	}

	UART_Printf("Press Enter \n");	
	UART_Getc();

	g_oaATAInform[g_ucATAConNum].bIsXferDone = FALSE;

	ATA_ClearAllInterrupt(g_ucATAConNum);

	INTC_SetVectAddr( NUM_CFC, Isr_AtaDma);
	INTC_Enable( NUM_CFC);
	
	for (uLoopCnt=0; uLoopCnt<uSector; uLoopCnt++)
	{
//		UART_Printf("(W)uDeviceLBA:%d, uLoopCnt:%d, uSector:%d\n", uDeviceLBA, uLoopCnt, uSector);

		if(uSlotSel == 1)
		ATA_StartWritingSectors(g_ucATAConNum, uDeviceLBA+uLoopCnt, 1, (u32)(g_oaATAInform[g_ucATAConNum].puATAWriteBuf+uLoopCnt*512), eATA_SLAVE);
		else
		ATA_StartWritingSectors(g_ucATAConNum, uDeviceLBA+uLoopCnt, 1, (u32)(g_oaATAInform[g_ucATAConNum].puATAWriteBuf+uLoopCnt*512), eATA_MASTER);

		while(g_oaATAInform[g_ucATAConNum].bIsXferDone != TRUE) ;
		g_oaATAInform[g_ucATAConNum].bIsXferDone = FALSE;	
	}

	for (uLoopCnt=0; uLoopCnt<uSector; uLoopCnt++)
	{
//		UART_Printf("(R)uDeviceLBA:%d, uLoopCnt:%d, uSector:%d\n", uDeviceLBA, uLoopCnt, uSector);

		if(uSlotSel == 1)
		ATA_StartReadingSectors(g_ucATAConNum, uDeviceLBA+uLoopCnt, 1, (u32)(g_oaATAInform[g_ucATAConNum].puATAReadBuf+uLoopCnt*512), eATA_SLAVE);
		else
		ATA_StartReadingSectors(g_ucATAConNum, uDeviceLBA+uLoopCnt, 1, (u32)(g_oaATAInform[g_ucATAConNum].puATAReadBuf+uLoopCnt*512), eATA_MASTER);

		while(g_oaATAInform[g_ucATAConNum].bIsXferDone != TRUE) ;
		g_oaATAInform[g_ucATAConNum].bIsXferDone = FALSE;	
	}

	if (Compare((u32)(g_oaATAInform[g_ucATAConNum].puATAWriteBuf), (u32)(g_oaATAInform[g_ucATAConNum].puATAReadBuf), uSector*128) == FALSE)
	{
		UART_Printf("Error detected\n");
		Dump32((u32)(g_oaATAInform[g_ucATAConNum].puATAReadBuf), uSector*128);
	}
	else
	{
		UART_Printf("Write/Read operation is OK\n");
	}

	INTC_Disable( NUM_CFC);
	
	ATA_CloseMedia(g_ucATAConNum);
}

//////////
// Function Name : ATA_TestUDmaMode_Int
// Function Description : 
//   This function tests  function of ATA.
// Input : NONE
// Output : NONE
// Version : v0.1 
void	ATA_TestUDmaMode_Int(void)
{
	u32		uDeviceLBA	= 0;
	u32		uSector		= 0;
	u32		uLoopCnt 	= 0;

	u32		uCurrentCount;
	u32		uRemainderCount;
	u32		uCurrentLba;
	u32		uCurrentAddr;
	u32		uRound;
	u8		ucCon = g_ucATAConNum;
	u32		uSlotSel		= 0;

	UART_Printf("Which Slot do you want to check? \n");
	UART_Printf("[0] = Slot 0 ,    [1] = Slot 1 \n");
	uSlotSel = UART_GetIntNum();
	UART_Printf("\n");

	if(uSlotSel == 1)
	ATA_OpenMedia(g_ucATAConNum, eATA_MODE_UDMA, eATA_SLAVE);
	else
	ATA_OpenMedia(g_ucATAConNum, eATA_MODE_UDMA, eATA_MASTER);
	
	UART_Printf("\nInput device sector address[max: 0x%x]\n",g_oaATAInform[ucCon].uMaxSectors);
	uDeviceLBA = (u32)UART_GetIntNum();

	UART_Printf("Input sector count[max: 0x%x]\n",g_oaATAInform[ucCon].uMaxSectors - uDeviceLBA);
	uSector = (u32)UART_GetIntNum();

	for (uLoopCnt=0; uLoopCnt < uSector*512; uLoopCnt++)
	{
		CF_SetMem(eBYTE, (u32)((u8*)g_oaATAInform[ucCon].puATAReadBuf + uLoopCnt), 0);
		CF_SetMem(eBYTE, (u32)((u8*)g_oaATAInform[ucCon].puATAWriteBuf + uLoopCnt), (uLoopCnt+5)%256);
	}

	UART_Printf("Press Enter \n");	
	UART_Getc();

	g_oaATAInform[ucCon].bIsXferDone = FALSE;

	ATA_ClearAllInterrupt(ucCon);

	INTC_SetVectAddr( NUM_CFC, Isr_AtaDma);
	INTC_Enable( NUM_CFC);

	// Max transfer block count per command is 256.
	uRemainderCount = uSector;
	uRound = 0;	
	
	while(uRemainderCount != 0) {
		if(uRemainderCount>256) {
			uCurrentCount = 256; //0 means 256
			uRemainderCount -= 256;
		} else {
			uCurrentCount = uRemainderCount;
			uRemainderCount = 0;
		}
		uCurrentLba = uDeviceLBA + uRound*256;
		uCurrentAddr = (u32)(g_oaATAInform[ucCon].puATAWriteBuf + uRound*256*512);

		if(uSlotSel == 1)
		ATA_StartWritingSectors(ucCon, uCurrentLba, uCurrentCount, uCurrentAddr,eATA_SLAVE);
		else
		ATA_StartWritingSectors(ucCon, uCurrentLba, uCurrentCount, uCurrentAddr,eATA_MASTER);

		while(g_oaATAInform[ucCon].bIsXferDone != TRUE);
		g_oaATAInform[ucCon].bIsXferDone = FALSE;

		uRound++;
	}

	UART_Printf("Write Done\n");

	uRemainderCount = uSector;
	uRound = 0;	
	
	while(uRemainderCount != 0) {
		if(uRemainderCount>256) {
			uCurrentCount = 256; //0 means 256
			uRemainderCount -= 256;
		} else {
			uCurrentCount = uRemainderCount;
			uRemainderCount = 0;
		}
		uCurrentLba = uDeviceLBA + uRound*256;
		uCurrentAddr = (u32)(g_oaATAInform[ucCon].puATAReadBuf + uRound*256*512);

		if(uSlotSel == 1)
		ATA_StartReadingSectors(ucCon, uCurrentLba, uCurrentCount, uCurrentAddr,eATA_SLAVE);
		else
		ATA_StartReadingSectors(ucCon, uCurrentLba, uCurrentCount, uCurrentAddr,eATA_MASTER);

		while(g_oaATAInform[ucCon].bIsXferDone != TRUE) ;
		g_oaATAInform[ucCon].bIsXferDone = FALSE;	

		uRound++;
	}
	
	UART_Printf("Read Done\n");	

	if (Compare((u32)(g_oaATAInform[ucCon].puATAWriteBuf), (u32)(g_oaATAInform[ucCon].puATAReadBuf), uSector*128) == FALSE)
	{
		UART_Printf("Error detected\n");
		Dump32((u32)(g_oaATAInform[ucCon].puATAReadBuf), uSector*128);
	}
	else
	{
		UART_Printf("Write/Read operation in UDMA mode is OK\n");
	}

	INTC_Disable( NUM_CFC);
	
	ATA_CloseMedia(ucCon);
}

//////////
// Function Name : ATA_OpenMedia
// Function Description : 
//   This function opens ATA media.
// Input : ucCon - ATA Controller Number 
//		eATAMode - PIO/PIOCPU/PDMA/UDMA
// Output : NONE
// Version : v0.1 
u8 ATA_OpenMedia(u8 ucCon, eATA_MODE_6400 eAtaMode, eATA_SLOT_SEL eSlotSel)
{
	eATA_MODE_6400	eMode = eATA_MODE_PIO;

	UART_Printf("ATA Mode : %s\n", ATA_GetModeName(eAtaMode));
	
	ATA_IdentifyDevice(ucCon,eSlotSel);
	
	
	if ( eAtaMode == eATA_MODE_NONE )
	{
		if ( g_oaATAInform[ucCon].eMaxUdmaMode == 0 )
		{
			eMode = eATA_MODE_PIO;
		}
		else
		{
			eMode = eATA_MODE_UDMA;
		}
	} 
	else 
	{
		eMode = eAtaMode ;
	}		

	g_oaATAInform[ucCon].eAtaMode = eMode;

	switch(eMode)
	{
		case eATA_MODE_PIOCPU :
		case eATA_MODE_PIODMA :
			ATA_SetPIOMode(ucCon, g_oaATAInform[ucCon].eMaxPioMode,eSlotSel);
			break;
		case eATA_MODE_UDMA :
			ATA_SetPIOMode(ucCon, eATA_PIO0, eSlotSel);							//?
			ATA_SetUdmaMode(ucCon, g_oaATAInform[ucCon].eMaxUdmaMode,eSlotSel);
			break;
		default :
			UART_Printf ("Invalid ATA Mode!!\n");
			return false;
	}
	return true;


	
}


//////////
// Function Name : ATA_CloseMedia
// Function Description : 
//   This function opens ATA media.
// Input : ucCon - ATA Controller Number 
//		eATAMode - PIO/PIOCPU/PDMA/UDMA
// Output : NONE
// Version : v0.1 
u8 ATA_CloseMedia(u8 ucCon)
{
	return true;
}

void ATA_TestPDmaMode_Int_for_EBI(void)
//void ATA_TestPioDmaMode(void)
{
	u32		uDeviceLBA	= 0;
	u32		uSector		= 0;
	u32		uLoopCnt 	= 0;

	uDeviceLBA = 0;
	uSector = 100;

	for (uLoopCnt=0; uLoopCnt < uSector*512; uLoopCnt++)
	{
		CF_SetMem(eBYTE, (u32)((u8*)g_oaATAInform[g_ucATAConNum].puATAReadBuf + uLoopCnt), 0);
		CF_SetMem(eBYTE, (u32)((u8*)g_oaATAInform[g_ucATAConNum].puATAWriteBuf + uLoopCnt), (uLoopCnt+2)%256);
	}

	UART_Printf("Press Enter \n");	
	UART_Getc();

	ATA_WriteBlocks(g_ucATAConNum, uDeviceLBA, uSector, (u32)(g_oaATAInform[g_ucATAConNum].puATAWriteBuf), eATA_MASTER);
	ATA_ReadBlocks(g_ucATAConNum, uDeviceLBA, uSector, (u32)(g_oaATAInform[g_ucATAConNum].puATAReadBuf), eATA_MASTER);

	if (Compare((u32)(g_oaATAInform[g_ucATAConNum].puATAWriteBuf), (u32)(g_oaATAInform[g_ucATAConNum].puATAReadBuf), uSector*128) == FALSE)
	{
		UART_Printf("Error detected\n");
		Dump32((u32)(g_oaATAInform[g_ucATAConNum].puATAReadBuf), uSector*128);
	}
	else
	{
		UART_Printf("Write/Read operation is OK\n");
	}

}


void	ATA_TestUDmaMode_Int_for_EBI(void)
{
	u32		uDeviceLBA	= 0;
	u32		uSector		= 0;
	u32		uLoopCnt 	= 0;

	u32		uCurrentCount;
	u32		uRemainderCount;
	u32		uCurrentLba;
	u32		uCurrentAddr;
	u32		uRound;
	u8		ucCon = g_ucATAConNum;

	uDeviceLBA = 0;
	uSector = 	100;

	for (uLoopCnt=0; uLoopCnt < uSector*512; uLoopCnt++)
	{
		CF_SetMem(eBYTE, (u32)((u8*)g_oaATAInform[ucCon].puATAReadBuf + uLoopCnt), 0);
		CF_SetMem(eBYTE, (u32)((u8*)g_oaATAInform[ucCon].puATAWriteBuf + uLoopCnt), (uLoopCnt+5)%256);
	}

	while(1)
	{
		g_oaATAInform[ucCon].bIsXferDone = FALSE;

		ATA_ClearAllInterrupt(ucCon);

		INTC_SetVectAddr( NUM_CFC, Isr_AtaDma);
		INTC_Enable( NUM_CFC);

		// Max transfer block count per command is 256.
		uRemainderCount = uSector;
		uRound = 0;	
		
		while(uRemainderCount != 0) {
			if(uRemainderCount>256) {
				uCurrentCount = 256; //0 means 256
				uRemainderCount -= 256;
			} else {
				uCurrentCount = uRemainderCount;
				uRemainderCount = 0;
			}
			uCurrentLba = uDeviceLBA + uRound*256;
			uCurrentAddr = (u32)(g_oaATAInform[ucCon].puATAWriteBuf + uRound*256*512);
#if 0
			ATA_StartWritingBlocks(ucCon, uCurrentLba, uCurrentCount, uCurrentAddr);
#else
			ATA_StartWritingSectors(ucCon, uCurrentLba, uCurrentCount, uCurrentAddr, eATA_MASTER);
#endif
			while(g_oaATAInform[ucCon].bIsXferDone != TRUE);
			g_oaATAInform[ucCon].bIsXferDone = FALSE;

			uRound++;
		}
	}
}

#define NAND_DMA_TEST 1
#define UDMA_TEST 1
#define PDMA_TEST 1

//////////
// Function Name : ATA_TestUDmaMode_Int_for_AtaBug
// Function Description : 
//   This function tests  function of ATA.
// Input : NONE
// Output : NONE
// Version : v0.1 
extern void NANDT_Init(void);
void	ATA_TestUDmaMode_Int_for_AtaBug(void)
{
	u32		uDeviceLBA	= 0;
	u32		uLoopCnt 	= 0;
	u32		uTempLoopCnt 	= 0;

	u8		ucCon = g_ucATAConNum;

	u32 uBlock = 0;
	u32 uPage = 0;
	

#if (NAND_DMA_TEST==1)
	//-------- Nand Flash setting --------------
	for(uLoopCnt=0 ; uLoopCnt<NAND_DATA_SIZE ; uLoopCnt++)
		*(unsigned char *)(0x52000000+uLoopCnt) = eATA_XFR_CMD_CONTINUE;
	for(uLoopCnt=0 ; uLoopCnt<NAND_SPARE_16 ; uLoopCnt++)
		aSpareBuffer[uLoopCnt] = 0xFF;

	uBlock = 50;
	uPage = 0;

	NANDT_Init();
#endif
#if (UDMA_TEST==1)
	ATA_OpenMedia(ucCon, eATA_MODE_UDMA, eATA_MASTER);

	UART_Printf("\nInput device sector address[max: 0x%x]\n",g_oaATAInform[ucCon].uMaxSectors);
	uDeviceLBA = (u32)UART_GetIntNum();

	for (uLoopCnt=0; uLoopCnt < 512; uLoopCnt++)
	{
		CF_SetMem(eBYTE, (u32)((u8*)g_oaATAInform[ucCon].puATAReadBuf + uLoopCnt), 0);
		CF_SetMem(eBYTE, (u32)((u8*)g_oaATAInform[ucCon].puATAWriteBuf + uLoopCnt), (uLoopCnt+5)%256);
	}

	g_oaATAInform[ucCon].bIsXferDone = FALSE;

	ATA_ClearAllInterrupt(ucCon);

	INTC_SetVectAddr( NUM_CFC, Isr_AtaDma);
	INTC_Enable( NUM_CFC);

#if 0
			ATA_StartWritingBlocks(ucCon, uCurrentLba, uCurrentCount, uCurrentAddr);
#else
			ATA_StartWritingSectors(ucCon, uDeviceLBA, 1, (u32)g_oaATAInform[ucCon].puATAWriteBuf,eATA_MASTER);
#endif

	while(g_oaATAInform[ucCon].bIsXferDone != TRUE);
	g_oaATAInform[ucCon].bIsXferDone = FALSE;
#endif
#if (NAND_DMA_TEST==1)
	NAND_WritePageSLCSetup(0, uBlock, uPage);
	EBI_SetDMAParams(eMEM_NAND);
	// Set LLI parameters for DMA loop operation
	SMC_SetLLI(g_oaEBIInform[eMEM_NAND].uSrcAddr, g_oaEBIInform[eMEM_NAND].uDstAddr, 
				(DATA_SIZE)g_oaEBIInform[eMEM_NAND].eDataSz , (BURST_MODE)g_oaEBIInform[eMEM_NAND].eBurstMode, g_oaEBIInform[eMEM_NAND].uDataCnt);

	DMACH_Start(&g_oaEBIInform[eMEM_NAND].oEBIDma);	// NAND DMA Start

#endif	
#if (UDMA_TEST==1)
	uLoopCnt = 0;
	while(1)
	{
		uLoopCnt++;
		if (uLoopCnt == 0xffffffff) uLoopCnt = 0;
		for(uTempLoopCnt = 0; uTempLoopCnt < uLoopCnt; uTempLoopCnt++) ;

#if 0
		ATA_StartReadingBlocks(ucCon, uDeviceLBA, 1, (u32)g_oaATAInform[ucCon].puATAReadBuf);
#else
		ATA_StartReadingSectors(ucCon, uDeviceLBA, 1, (u32)g_oaATAInform[ucCon].puATAReadBuf, eATA_MASTER);
#endif
		
		while(g_oaATAInform[ucCon].bIsXferDone != TRUE);
		g_oaATAInform[ucCon].bIsXferDone = FALSE;

		if (Compare((u32)(g_oaATAInform[ucCon].puATAWriteBuf), (u32)(g_oaATAInform[ucCon].puATAReadBuf), 1) == FALSE)
		{
			UART_Printf("Error detected\n");
			Dump32((u32)(g_oaATAInform[ucCon].puATAReadBuf), 128);
			break;
		}
		else
		{
			UART_Printf("O");
		}		
	}
	NAND_WritePageSLCClose(0, aSpareBuffer);
	EBI_CloseDMA(eMEM_NAND);

	INTC_Disable( NUM_CFC);
	
	ATA_CloseMedia(ucCon);
#endif	
}



//----------------------------- Added for AutoTest -----------------------------//

//////////
// Function Name : ATA_PioCpu_autotest
// Function Description : 
//   This function tests  function of ATA.
// Input : NONE
// Output : u8
// Version : v0.1 
u8 ATA_PioCpu_autotest(void)
{
	u32		uDeviceLBA	= 0;
	u32		uSector		= 0;
	u32		uLoopCnt 	= 0;
/*
	AUTO_preparing(-1, "cpu b'd top right J701 <- WITHOUT  mipi connector(FPC701A) \n\
cpu b'd top left JA20 -> Jumper  to _[ ]  011, to set 3.3V\n\
base b'd CFG4  SW1,2,3,4 to 0,1,0,0");
*/
	GPIO_SetPullUpDownEach(eGPIO_P, eGPIO_14, 2);
	if (!ATA_Init(ATA_CON0, 1))
	{
		UART_Printf("Fail to initialize ATA Mode...\n");
		return false;
	}

	ATA_OpenMedia(g_ucATAConNum, eATA_MODE_PIOCPU,eATA_MASTER);

	uDeviceLBA = 1000;
	uSector = 5000;
	UART_Printf("\nStart sector address = %d, Copying sector size = %d sectors \n", uDeviceLBA, uSector);

	for (uLoopCnt=0; uLoopCnt < uSector*512; uLoopCnt++)
	{
		CF_SetMem(eBYTE, (u32)((u8*)g_oaATAInform[g_ucATAConNum].puATAReadBuf + uLoopCnt), 0);
		CF_SetMem(eBYTE, (u32)((u8*)g_oaATAInform[g_ucATAConNum].puATAWriteBuf + uLoopCnt), (uLoopCnt+1)%256);
	}

	ATA_WriteBlocks(g_ucATAConNum, uDeviceLBA, uSector, (u32)(g_oaATAInform[g_ucATAConNum].puATAWriteBuf),eATA_MASTER);
	ATA_ReadBlocks(g_ucATAConNum, uDeviceLBA, uSector, (u32)(g_oaATAInform[g_ucATAConNum].puATAReadBuf),eATA_MASTER);

	ATA_CloseMedia(g_ucATAConNum);

	if (Compare((u32)(g_oaATAInform[g_ucATAConNum].puATAWriteBuf), (u32)(g_oaATAInform[g_ucATAConNum].puATAReadBuf), uSector*128) == FALSE)
	{
		UART_Printf("Error detected\n");
		Dump32((u32)(g_oaATAInform[g_ucATAConNum].puATAReadBuf), 32);
		return false;
	}
	else
	{
		UART_Printf("Write/Read operation is OK\n");
		return true;
	}
}


//////////
// Function Name : ATA_PioDma_autotest
// Function Description : 
//   This function tests  function of ATA.
// Input : NONE
// Output : u8
// Version : v0.1 
u8 ATA_PioDma_autotest(void)
{
	u32		uDeviceLBA	= 0;
	u32		uSector		= 0;
	u32		uLoopCnt 	= 0;

	GPIO_SetPullUpDownEach(eGPIO_P, eGPIO_14, 2);
	if (!ATA_Init(ATA_CON0, 1))
	{
		UART_Printf("Fail to initialize ATA Mode...\n");
		return false;
	}

	ATA_OpenMedia(g_ucATAConNum, eATA_MODE_PIODMA,eATA_MASTER);

	uDeviceLBA = 1000;
	uSector = 10000;
	UART_Printf("\nStart sector address = %d, Copying sector size = %d sectors \n", uDeviceLBA, uSector);

	for (uLoopCnt=0; uLoopCnt < uSector*512; uLoopCnt++)
	{
		CF_SetMem(eBYTE, (u32)((u8*)g_oaATAInform[g_ucATAConNum].puATAReadBuf + uLoopCnt), 0);
		CF_SetMem(eBYTE, (u32)((u8*)g_oaATAInform[g_ucATAConNum].puATAWriteBuf + uLoopCnt), (uLoopCnt+2)%256);
	}

	ATA_WriteBlocks(g_ucATAConNum, uDeviceLBA, uSector, (u32)(g_oaATAInform[g_ucATAConNum].puATAWriteBuf),eATA_MASTER);
	ATA_ReadBlocks(g_ucATAConNum, uDeviceLBA, uSector, (u32)(g_oaATAInform[g_ucATAConNum].puATAReadBuf),eATA_MASTER);

	ATA_CloseMedia(g_ucATAConNum);

	if (Compare((u32)(g_oaATAInform[g_ucATAConNum].puATAWriteBuf), (u32)(g_oaATAInform[g_ucATAConNum].puATAReadBuf), uSector*128) == FALSE)
	{
		UART_Printf("Error detected\n");
		Dump32((u32)(g_oaATAInform[g_ucATAConNum].puATAReadBuf), 32);
		return false;
	}
	else
	{
		UART_Printf("Write/Read operation is OK\n");
		return true;
	}
}


//////////
// Function Name : ATA_UDmaMode_autotest
// Function Description : 
//   This function tests  function of ATA.
// Input : NONE
// Output : u8
// Version : v0.1 
u8 ATA_UDmaMode_autotest(void)
{
	u32		uDeviceLBA	= 0;
	u32		uSector		= 0;
	u32		uLoopCnt 	= 0;

	GPIO_SetPullUpDownEach(eGPIO_P, eGPIO_14, 2);
	if (!ATA_Init(ATA_CON0, 1))
	{
		UART_Printf("Fail to initialize ATA Mode...\n");
		return false;
	}

	ATA_OpenMedia(g_ucATAConNum, eATA_MODE_UDMA,eATA_MASTER);

	uDeviceLBA = 1000;
	uSector = 20000;
	UART_Printf("\nStart sector address = %d, Copying sector size = %d sectors \n", uDeviceLBA, uSector);

	for (uLoopCnt=0; uLoopCnt < uSector*512; uLoopCnt++)
	{
		CF_SetMem(eBYTE, (u32)((u8*)g_oaATAInform[g_ucATAConNum].puATAReadBuf + uLoopCnt), 0);
		CF_SetMem(eBYTE, (u32)((u8*)g_oaATAInform[g_ucATAConNum].puATAWriteBuf + uLoopCnt), (uLoopCnt+3)%256);
	}

	ATA_WriteBlocks(g_ucATAConNum, uDeviceLBA, uSector, (u32)(g_oaATAInform[g_ucATAConNum].puATAWriteBuf),eATA_MASTER);
	ATA_ReadBlocks(g_ucATAConNum, uDeviceLBA, uSector, (u32)(g_oaATAInform[g_ucATAConNum].puATAReadBuf),eATA_MASTER);

	ATA_CloseMedia(g_ucATAConNum);

	if (Compare((u32)(g_oaATAInform[g_ucATAConNum].puATAWriteBuf), (u32)(g_oaATAInform[g_ucATAConNum].puATAReadBuf), uSector*128) == FALSE)
	{
		UART_Printf("Error detected\n");
		Dump32((u32)(g_oaATAInform[g_ucATAConNum].puATAReadBuf), 32);
/*
		AUTO_preparing(-1, "for next test, restore as following\n\
cpu b'd top left JA20 -> JUMPER  to [ ]_  110, to set 1.8V \n\
cpu b'd top right J701 <- INSERT MIPI connector(FPC701A)");		
*/
		return false;
	}
	else
	{
		UART_Printf("Write/Read operation in UDMA mode is OK\n");
/*
		AUTO_preparing(-1, "for next test, restore as following\n\
cpu b'd top left JA20 -> JUMPER  to [ ]_  110, to set 1.8V \n\
cpu b'd top right J701 <- INSERT MIPI connector(FPC701A)");				
*/
		return true;
	}
}


