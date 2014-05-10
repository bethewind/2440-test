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
*	File Name : SROMC_test.c
*  
*	File Description : This file implements the functons for SROM controller test.
*
*	Author : Sunil Roe
*	Dept. : AP Development Team
*	Created Date : 2007/02/22
*	Version : 0.1 
* 
*	History
*	- Created(sunil.roe 2007/02/22)
*  	- Modify	(HeeMyung.Noh 2008/02/27)
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
#include "sromc.h"
#include "nand.h"
#include "onenand.h"
#include "cf.h"
#include "ata.h"
#include "timer.h"
#include "cs8900.h"

#define SRAM_DATA_SIZE	0x1000
#define NAND_DATA_SIZE	NAND_PAGE_512

//rb1004
#define SROMC_Tacs_MIN		(0)
#define SROMC_Tacs_MAX		(15)
#define SROMC_Tcos_MIN		(3)		//K4X4016T3F : 3[HCLK] at 133MHz
#define SROMC_Tcos_MAX		(15)
#define SROMC_Tacc_MIN		(6)		//K4X4016T3F : 6[HCLK] at 133Mhz
#define SROMC_Tacc_MAX		(32)
#define SROMC_Tcoh_MIN		(0)
#define SROMC_Tcoh_MAX		(15)
#define SROMC_Tcah_MIN		(0)
#define SROMC_Tcah_MAX		(15)

//#define SROMC_Tacp_MIN		(0)		// In case of Non Page Mode	
#define SROMC_Tacp_MIN		(1)		// In case of Page Mode Test

#define SROMC_Tacp_MAX		(15)

// Device
#define SROMC_6400			(0)
#define SROMC_6410			(1)
#define SROMC_DEVICE		SROMC_6410

// External SRAM Memory Access Size Setting
#if (SROMC_DEVICE == SROMC_6410)
//#define SROMC_SRAM_SIZE	(512*1024)	
#define SROMC_SRAM_SIZE	(6*1024)	
#else
#define SROMC_SRAM_SIZE	(64*1024)
#endif

// SROMC data pattern setting
#define SROMC_DATA_RANDOM	(0)
#define SROMC_DATA_ONEVALUE	(0x5A5A5A5A)
#define SROMC_DATA_PATTERN 	(SROMC_DATA_RANDOM)

// SROMC Data Transfer Type
#define SROMC_POLLING		(0)
#define SROMC_MEMCPY		(1)
#define SROMC_DMA			(2)
#define SROMC_TR_TYPE		SROMC_DMA

DMAC g_oSROMCDmac0;
u32 SROMC_DmaDone;

EBI_oInform	g_oaEBIInform[MEMIF_NUM];
u8 aBuffer[NAND_PAGE_MAX];
u8 aSpareBuffer[NAND_SPARE_MAX];
u32 aReadData[ONENAND_PAGESIZE/4] = {0, };
u32	g_uLLIBaseAddr = _DRAM_BaseAddress + 0x00100000;
volatile u32 Nand_DmaTestDone;
u32 g_uOneNANDSrcAddr = 0;


void __irq SROMC_DmaISR(void)
{
  	DMACH_ClearIntPending(&g_oSROMCDmac0);

	SROMC_DmaDone = 1;
	INTC_ClearVectAddr();
}

//rb1004.....for EBI Multi Access Test
extern DMAC g_oDmc0Dmac0;
void __irq SROMC_MADmaISR(void)
{
	u32 uIntStatus;
	
	//uIntStatus = DMAC_IntStatus(&g_oSROMCDmac0);
	uIntStatus = *(u32 *)0x75000000;

	if(uIntStatus & 0x01)		// SROMC Dma : DMA0 Ch0
	{
  		DMACH_ClearIntPendingrb1004(&g_oSROMCDmac0, 0x01);
		SROMC_DmaDone = 1;
	}

	INTC_ClearVectAddr();
}

u32 SROMC_WriteReadTest(SROMC_eBANK uBank, Bank_eTiming eTacs, Bank_eTiming eTcos,Bank_eTiming eTacc, 
							 					Bank_eTiming eTcoh, Bank_eTiming eTcah, Bank_eTiming eTacp, Page_eMode ePageMode )
{
	u32 i;
	u8 bError = FALSE;
	u32 *pWriteBaseData, *pReadBaseData, *pWriteData, *pReadData;
	u32 *pBankBaseAddress, *pAddress;

	pWriteBaseData = (u32 *)malloc(SROMC_SRAM_SIZE);
	if(pWriteBaseData == 0)
	{
		UART_Printf("Memory Allocation Error...\n");
		return 0;
	}
	pWriteData = pWriteBaseData;
	
	pReadBaseData = (u32 *)malloc(SROMC_SRAM_SIZE);	
	if(pReadBaseData == 0)
	{
		free(pWriteBaseData);
		UART_Printf("Memory Allocation Error...\n");
		return 0;
	}
	pReadData = pReadBaseData;

	pBankBaseAddress = (u32 *)((uBank * 0x08000000)+0x10000000);
	pAddress = pBankBaseAddress;

#if (SROMC_TR_TYPE == SROMC_DMA)
#if 1	//DMA0 controller
	DMAC_InitCh(DMA0, DMA_ALL, &g_oSROMCDmac0);
	INTC_SetVectAddr(NUM_DMA0,  SROMC_DmaISR);
	INTC_Enable(NUM_DMA0);
#elif 0	//DMA1 controller
	DMAC_InitCh(DMA1, DMA_ALL, &g_oSROMCDmac0);
	INTC_SetVectAddr(NUM_DMA1,  SROMC_DmaISR);
	INTC_Enable(NUM_DMA1);
#elif 0	//SDMA0 controller
	DMAC_InitCh(SDMA0, DMA_ALL, &g_oSROMCDmac0);
	INTC_SetVectAddr(NUM_SDMA0,  SROMC_DmaISR);
	INTC_Enable(NUM_SDMA0);
#else	//SDMA1 controller
	DMAC_InitCh(SDMA1, DMA_ALL, &g_oSROMCDmac0);
	INTC_SetVectAddr(NUM_SDMA1,  SROMC_DmaISR);
	INTC_Enable(NUM_SDMA1);	
#endif	
#endif	

	UART_Printf("Tacs(%d), Tcos(%d), Tacc(%d), Tcoh(%d), Tcah(%d), Tacp(%d)\n", eTacs, eTcos, eTacc, eTcoh, eTcah, eTacp);
	SROMC_SetTiming(uBank, eTacs, eTcos, eTacc, 	eTcoh, eTcah, eTacp, ePageMode);
	DelayfrTimer(milli, 1);
	
	pWriteData = pWriteBaseData;
	
	srand(eTacs + eTcos + eTacc + eTcoh + eTcah + eTacp);

#if 1	
	for(i=0; i<SROMC_SRAM_SIZE/4; i++)
	{
#if(SROMC_DATA_PATTERN == SROMC_DATA_RANDOM)
		*pWriteData++ = rand();
#else	// ONDT_DATA_PATTERN == ONDT_DATA_ONEVALUE
		*pWriteData++ = SROMC_DATA_ONEVALUE;
#endif
	}
#else	// rb1004... for 8-bit bus width test(external bus width : 16bit at SMDK)
	{	
		u8 cTemp;
		
		for(i=0; i<SROMC_SRAM_SIZE/2; i++)
		{
#if(SROMC_DATA_PATTERN == SROMC_DATA_RANDOM)
			cTemp = (u8)rand();
			*((u8 *)pWriteData)++ = cTemp;
			*((u8 *)pWriteData)++ = cTemp;
#else	// ONDT_DATA_PATTERN == ONDT_DATA_ONEVALUE
			*((u8 *)pWriteData)++ = (u8)(SROMC_DATA_ONEVALUE);
			*((u8 *)pWriteData)++ = (u8)(SROMC_DATA_ONEVALUE);
#endif
		}
	}
#endif

	/////////////////////////////////
	// Write from DRAM to SRAM memory
	/////////////////////////////////
	pWriteData = pWriteBaseData;
	pAddress = pBankBaseAddress;
	
#if (SROMC_TR_TYPE == SROMC_POLLING)
	for(i=0; i<SROMC_SRAM_SIZE/4; i++)
	{
		*pAddress++ =  *pWriteData++;
	}
#elif (SROMC_TR_TYPE == SROMC_MEMCPY)
	memcpy((void *)pAddress, (void *)pWriteData, SROMC_SRAM_SIZE);
#else
	SROMC_DmaDone = 0;
	DMACH_Setup(DMA_A, 0, (u32)pWriteData, 0, (u32)pAddress, 0, WORD, SROMC_SRAM_SIZE/4, DEMAND, MEM, MEM, BURST4, &g_oSROMCDmac0);
	DMACH_Start(&g_oSROMCDmac0);
	while(!SROMC_DmaDone);
#endif

	/////////////////////////////////
	// Read from SRAM memory to DRAM
	/////////////////////////////////
	pReadData = pReadBaseData;
	pAddress = pBankBaseAddress;
	
#if (SROMC_TR_TYPE == SROMC_POLLING)
	for(i=0; i<SROMC_SRAM_SIZE/4; i++)
	{
		*pReadData++ = *pAddress++;
	}
#elif (SROMC_TR_TYPE == SROMC_MEMCPY)
	memcpy((void *)pReadData, (void *)pAddress, SROMC_SRAM_SIZE);
#else
	SROMC_DmaDone = 0;
	DMACH_Setup(DMA_A, 0, (u32)pAddress, 0, (u32)pReadData, 0, WORD, SROMC_SRAM_SIZE/4, DEMAND, MEM, MEM, BURST4, &g_oSROMCDmac0);
	DMACH_Start(&g_oSROMCDmac0);
	while(!SROMC_DmaDone);
#endif			

	/////////////////////////////////
	// Compare Data
	/////////////////////////////////
	pReadData = pReadBaseData;
	pWriteData = pWriteBaseData;
	pAddress = pBankBaseAddress;
	bError = FALSE;
	
	for(i=0 ; i<SROMC_SRAM_SIZE/4 ; i++)
	{
		if(pReadData[i] != pWriteData[i])
		{
			UART_Printf("\t [Address: 0x%08x] [Write data : Read data] = [0x%08x  : 0x%08x ]  \n", pAddress, pWriteData[i], pReadData[i]);
			UART_Getc();
			bError = TRUE;
			break;
		}
		pAddress++;
	}
	if(bError == FALSE)
		UART_Printf("Test OK\n");	

	pWriteData = pWriteBaseData;
	for(i=0; i<SROMC_SRAM_SIZE/4; i++)
	{
		*pWriteData++ = 0;
	}

	free(pWriteBaseData);
	free(pReadBaseData);
	
	return 0;
}


void SROMC_Test(void)
{
	u32 uTacs, uTcos, uTacc, uTcoh, uTcah, uTacp;
	SROMC_eBANK uBank;
	Data_eWidth eDWidth;
	WAIT_eCTL eWAITCTL;
	Byte_eCTL eByteCTL;
	Page_eMode ePageMode;

	UART_Printf("[SROMC Test]\n");

	UART_Printf("Input the SROMC BANK Number[0~%d] : \n", eBankMAX);
	uBank = (SROMC_eBANK)UART_GetIntNum(); 
	if(uBank > 5)
		uBank = (SROMC_eBANK)0;
	UART_Printf("Input the Data Bus Width : 8-bit(0),   16-bit(1) \n");
	eDWidth = (Data_eWidth)UART_GetIntNum(); 	
	if(eDWidth > 1)
		eDWidth = (Data_eWidth)1;
	UART_Printf("Input the Wait Enable Control : Disable(0),  Enable(1)\n");
	eWAITCTL = (WAIT_eCTL)UART_GetIntNum(); 
	if(eWAITCTL > 1)
		eWAITCTL = (WAIT_eCTL)0;
	UART_Printf("Input the Byte Enable(nBE/nWBE) Setting : Not using(0),  Using(1)\n");
	eByteCTL = (Byte_eCTL)UART_GetIntNum(); 
	if(eByteCTL > 1)
		eByteCTL = (Byte_eCTL)0;
	UART_Printf("Input the Page Mode Setting : Normal(0),  4data(1)\n");
	ePageMode = (Page_eMode)UART_GetIntNum(); 
	if(ePageMode > 1)
		ePageMode = (Page_eMode)0;
	
	SROMC_SetBusWidth( uBank, eDWidth);
	SROMC_SetWaitControl(uBank, eWAITCTL);
	SROMC_SetByteEnable(uBank, eByteCTL);

	uTacs = SROMC_Tacs_MIN;
	uTcos = SROMC_Tcos_MIN;
	uTacc = SROMC_Tacc_MIN;
	uTcoh = SROMC_Tcoh_MIN;
	uTcah = SROMC_Tcah_MIN;
	
	// Tacp parameter test
	for(uTacp = SROMC_Tacp_MIN ; uTacp <= SROMC_Tacp_MAX ; uTacp++)
	{
		SROMC_WriteReadTest(uBank, (Bank_eTiming)uTacs, (Bank_eTiming)uTcos, (Bank_eTiming)uTacc, 
									(Bank_eTiming)uTcoh, (Bank_eTiming)uTcah, (Bank_eTiming)uTacp, ePageMode);
	}
	uTacp = SROMC_Tacp_MIN;

	// Tcah parameter test
	for(uTcah = SROMC_Tcah_MIN ; uTcah <= SROMC_Tcah_MAX ; uTcah++)
	{
		SROMC_WriteReadTest(uBank, (Bank_eTiming)uTacs, (Bank_eTiming)uTcos, (Bank_eTiming)uTacc, 
									(Bank_eTiming)uTcoh, (Bank_eTiming)uTcah, (Bank_eTiming)uTacp, ePageMode);
	}
	uTcah = SROMC_Tcah_MIN;

	// Tcoh parameter test
	for(uTcoh = SROMC_Tcoh_MIN ; uTcoh <= SROMC_Tcoh_MAX ; uTcoh++)
	{
		SROMC_WriteReadTest(uBank, (Bank_eTiming)uTacs, (Bank_eTiming)uTcos, (Bank_eTiming)uTacc, 
									(Bank_eTiming)uTcoh, (Bank_eTiming)uTcah, (Bank_eTiming)uTacp, ePageMode);
	}
	uTcoh = SROMC_Tcoh_MIN;

	// Tacc parameter test
	for(uTacc = SROMC_Tacc_MIN ; uTacc <= SROMC_Tacc_MAX ; uTacc++)
	{
		SROMC_WriteReadTest(uBank, (Bank_eTiming)uTacs, (Bank_eTiming)uTcos, (Bank_eTiming)uTacc, 
									(Bank_eTiming)uTcoh, (Bank_eTiming)uTcah, (Bank_eTiming)uTacp, ePageMode);
	}
	uTacc = SROMC_Tacc_MIN;

	// Tcos parameter test
	for(uTcos = SROMC_Tcos_MIN ; uTcos <= SROMC_Tcos_MAX ; uTcos++)
	{
		SROMC_WriteReadTest(uBank, (Bank_eTiming)uTacs, (Bank_eTiming)uTcos, (Bank_eTiming)uTacc, 
									(Bank_eTiming)uTcoh, (Bank_eTiming)uTcah, (Bank_eTiming)uTacp, ePageMode);
	}
	uTcos = SROMC_Tcos_MIN;

	// Tacs parameter test
	for(uTacs = SROMC_Tacs_MIN ; uTacs <= SROMC_Tacs_MAX ; uTacs++)
	{
		SROMC_WriteReadTest(uBank, (Bank_eTiming)uTacs, (Bank_eTiming)uTcos, (Bank_eTiming)uTacc, 
									(Bank_eTiming)uTcoh, (Bank_eTiming)uTcah, (Bank_eTiming)uTacp, ePageMode);
	}
	uTacs = SROMC_Tacs_MIN;	

	UART_Printf("\n\n");
}



void SROMC_AgingTest(void)
{
	u32 i, uNumClock, uTacs, uTcos, uTacc, uTcoh, uTcah, uTacp;
	SROMC_eBANK uBank;
	Data_eWidth eDWidth;
	WAIT_eCTL eWAITCTL;
	Byte_eCTL eByteCTL;
	Page_eMode ePageMode;
						   //{Sync Mode, APLL, MPLL, HCLKx2 divide, HCLK divide}
	u32 aCaseOfClock[][7] = { //Sync Mode
#if(SROMC_DEVICE == SROMC_6410)	
							{eSYNC_MODE, (APLL_eOUT)eAPLL532M, (APLL_eOUT)eAPLL266M, 2, 2},
							{eSYNC_MODE, (APLL_eOUT)eAPLL532M, (APLL_eOUT)eAPLL266M, 4, 2},
#endif	

#if 1
							{eASYNC_MODE, (APLL_eOUT)eAPLL532M, (APLL_eOUT)eAPLL266M, 1, 2},
							{eASYNC_MODE, (APLL_eOUT)eAPLL532M, (APLL_eOUT)eAPLL266M, 2, 2},
							{eASYNC_MODE, (APLL_eOUT)eAPLL667M, (APLL_eOUT)eAPLL266M, 1, 2},
							{eASYNC_MODE, (APLL_eOUT)eAPLL667M, (APLL_eOUT)eAPLL266M, 2, 2},
#endif
							};
						   
	UART_Printf("[SROMC Test]\n");

	UART_Printf("Input the SROMC BANK Number[0~%d] : \n", eBankMAX);
	uBank = (SROMC_eBANK)UART_GetIntNum(); 
	if(uBank > 5)
		uBank = (SROMC_eBANK)0;
	UART_Printf("Input the Data Bus Width : 8-bit(0),   16-bit(1) \n");
	eDWidth = (Data_eWidth)UART_GetIntNum(); 	
	if(eDWidth > 1)
		eDWidth = (Data_eWidth)1;
	UART_Printf("Input the Wait Enable Control : Disable(0),  Enable(1)\n");
	eWAITCTL = (WAIT_eCTL)UART_GetIntNum(); 
	if(eWAITCTL > 1)
		eWAITCTL = (WAIT_eCTL)0;
	UART_Printf("Input the Byte Enable(nBE/nWBE) Setting : Not using(0),  Using(1)\n");
	eByteCTL = (Byte_eCTL)UART_GetIntNum(); 
	if(eByteCTL > 1)
		eByteCTL = (Byte_eCTL)0;
	UART_Printf("Input the Page Mode Setting : Normal(0),  4data(1)\n");
	ePageMode = (Page_eMode)UART_GetIntNum(); 
	if(ePageMode > 1)
		ePageMode = (Page_eMode)0;
	
	SROMC_SetBusWidth( uBank, eDWidth);
	SROMC_SetWaitControl(uBank, eWAITCTL);
	SROMC_SetByteEnable(uBank, eByteCTL);

	uTacs = SROMC_Tacs_MIN;
	uTcos = SROMC_Tcos_MIN;
	uTacc = SROMC_Tacc_MIN;
	uTcoh = SROMC_Tcoh_MIN;
	uTcah = SROMC_Tcah_MIN;
	uTacp = SROMC_Tacp_MIN;

	uNumClock = sizeof(aCaseOfClock)/sizeof(aCaseOfClock[0]);

	while(!UART_GetKey())
	{
	for(i=0 ; i<uNumClock ; i++)
	{
		if(aCaseOfClock[i][0] == eSYNC_MODE)
		{
			SYSC_ChangeMode(eSYNC_MODE);
			SYSC_ChangeSYSCLK_1((APLL_eOUT)aCaseOfClock[i][1], (APLL_eOUT)aCaseOfClock[i][2], 0, 1, 3);
			SYSC_SetDIV0(0, 1, aCaseOfClock[i][4]-1, aCaseOfClock[i][3]-1,  3, 0, 1,0, 1, 0);

			SYSC_GetClkInform();
			OpenConsole();

			UART_Printf("\n\n");
			UART_Printf("====================================================================\n");
			UART_Printf("Sync Mode : Synchronous\n");
			UART_Printf("ARMCLK : HCLK : PCLK = %d : %d : %d MHz (MPLL : %d MHz)\n", 
								g_ARMCLK/1000000, g_HCLK/1000000, g_PCLK/1000000, g_MPLL/1000000 );
		}
		else		// Async Mode
		{
			SYSC_ChangeMode(eASYNC_MODE);
			SYSC_ChangeSYSCLK_1((APLL_eOUT)aCaseOfClock[i][1], (APLL_eOUT)aCaseOfClock[i][2], 0, 1, 3);
			SYSC_SetDIV0(0, 1, aCaseOfClock[i][4]-1, aCaseOfClock[i][3]-1,  3, 0, 1,0, 1, 0);

			SYSC_GetClkInform();
			OpenConsole();

			UART_Printf("\n\n");
			UART_Printf("====================================================================\n");
			UART_Printf("Sync Mode : Asynchronous\n");
			UART_Printf("ARMCLK : HCLK : PCLK = %d : %d : %d MHz (MPLL : %d MHz)\n", 
								g_ARMCLK/1000000, g_HCLK/1000000, g_PCLK/1000000, g_MPLL/1000000 );		
		}
		UART_Printf("\n");

		// Tacp parameter test
		for(uTacp = SROMC_Tacp_MIN ; uTacp <= SROMC_Tacp_MAX ; uTacp++)
		{
			SROMC_WriteReadTest(uBank, (Bank_eTiming)uTacs, (Bank_eTiming)uTcos, (Bank_eTiming)uTacc, 
										(Bank_eTiming)uTcoh, (Bank_eTiming)uTcah, (Bank_eTiming)uTacp, ePageMode);
		}
		uTacp = SROMC_Tacp_MIN;

		// Tcah parameter test
		for(uTcah = SROMC_Tcah_MIN ; uTcah <= SROMC_Tcah_MAX ; uTcah++)
		{
			SROMC_WriteReadTest(uBank, (Bank_eTiming)uTacs, (Bank_eTiming)uTcos, (Bank_eTiming)uTacc, 
										(Bank_eTiming)uTcoh, (Bank_eTiming)uTcah, (Bank_eTiming)uTacp, ePageMode);
		}
		uTcah = SROMC_Tcah_MIN;

		// Tcoh parameter test
		for(uTcoh = SROMC_Tcoh_MIN ; uTcoh <= SROMC_Tcoh_MAX ; uTcoh++)
		{
			SROMC_WriteReadTest(uBank, (Bank_eTiming)uTacs, (Bank_eTiming)uTcos, (Bank_eTiming)uTacc, 
										(Bank_eTiming)uTcoh, (Bank_eTiming)uTcah, (Bank_eTiming)uTacp, ePageMode);
		}
		uTcoh = SROMC_Tcoh_MIN;

		// Tacc parameter test
		for(uTacc = SROMC_Tacc_MIN ; uTacc <= SROMC_Tacc_MAX ; uTacc++)
		{
			SROMC_WriteReadTest(uBank, (Bank_eTiming)uTacs, (Bank_eTiming)uTcos, (Bank_eTiming)uTacc, 
										(Bank_eTiming)uTcoh, (Bank_eTiming)uTcah, (Bank_eTiming)uTacp, ePageMode);
		}
		uTacc = SROMC_Tacc_MIN;

		// Tcos parameter test
		for(uTcos = SROMC_Tcos_MIN ; uTcos <= SROMC_Tcos_MAX ; uTcos++)
		{
			SROMC_WriteReadTest(uBank, (Bank_eTiming)uTacs, (Bank_eTiming)uTcos, (Bank_eTiming)uTacc, 
										(Bank_eTiming)uTcoh, (Bank_eTiming)uTcah, (Bank_eTiming)uTacp, ePageMode);
		}
		uTcos = SROMC_Tcos_MIN;

		// Tacs parameter test
		for(uTacs = SROMC_Tacs_MIN ; uTacs <= SROMC_Tacs_MAX ; uTacs++)
		{
			SROMC_WriteReadTest(uBank, (Bank_eTiming)uTacs, (Bank_eTiming)uTcos, (Bank_eTiming)uTacc, 
										(Bank_eTiming)uTcoh, (Bank_eTiming)uTcah, (Bank_eTiming)uTacp, ePageMode);
		}
		uTacs = SROMC_Tacs_MIN;	
		
		UART_Printf("====================================================================\n");
	}
	}
	
	UART_Printf("\n\n");

	SYSC_ChangeMode(eSYNC_MODE);
	SYSC_ChangeSYSCLK_1(eAPLL532M, eAPLL266M, 0, 1, 3);
	SYSC_SetDIV0(0, 1, 1, 1,  3, 1, 1, 1, 1, 1);	
}


u32 SROMC_ByteWriteReadTest(SROMC_eBANK uBank, Bank_eTiming eTacs, Bank_eTiming eTcos,Bank_eTiming eTacc, 
							 					Bank_eTiming eTcoh, Bank_eTiming eTcah, Bank_eTiming eTacp, Page_eMode ePageMode )
{
	u32 i;
	u8 bError = FALSE;
	u8 *pWriteBaseData, *pReadBaseData, *pWriteData, *pReadData;
	u8 *pBankBaseAddress, *pAddress;

	pWriteBaseData = (u8 *)malloc(SROMC_SRAM_SIZE);
	if(pWriteBaseData == 0)
	{
		UART_Printf("Memory Allocation Error...\n");
		return 0;
	}
	pWriteData = pWriteBaseData;
	
	pReadBaseData = (u8 *)malloc(SROMC_SRAM_SIZE);	
	if(pReadBaseData == 0)
	{
		free(pWriteBaseData);
		UART_Printf("Memory Allocation Error...\n");
		return 0;
	}
	pReadData = pReadBaseData;

	pBankBaseAddress = (u8 *)((uBank * 0x08000000)+0x10000000);
	pAddress = pBankBaseAddress;

#if (SROMC_TR_TYPE == SROMC_DMA)
#if 1	//DMA0 controller
	DMAC_InitCh(DMA0, DMA_ALL, &g_oSROMCDmac0);
	INTC_SetVectAddr(NUM_DMA0,  SROMC_DmaISR);
	INTC_Enable(NUM_DMA0);
#elif 0	//DMA1 controller
	DMAC_InitCh(DMA1, DMA_ALL, &g_oSROMCDmac0);
	INTC_SetVectAddr(NUM_DMA1,  SROMC_DmaISR);
	INTC_Enable(NUM_DMA1);
#elif 0	//SDMA0 controller
	DMAC_InitCh(SDMA0, DMA_ALL, &g_oSROMCDmac0);
	INTC_SetVectAddr(NUM_SDMA0,  SROMC_DmaISR);
	INTC_Enable(NUM_SDMA0);
#else	//SDMA1 controller
	DMAC_InitCh(SDMA1, DMA_ALL, &g_oSROMCDmac0);
	INTC_SetVectAddr(NUM_SDMA1,  SROMC_DmaISR);
	INTC_Enable(NUM_SDMA1);	
#endif	
#endif	

	UART_Printf("Tacs(%d), Tcos(%d), Tacc(%d), Tcoh(%d), Tcah(%d), Tacp(%d)\n", eTacs, eTcos, eTacc, eTcoh, eTcah, eTacp);
	SROMC_SetTiming(uBank, eTacs, eTcos, eTacc, 	eTcoh, eTcah, eTacp, ePageMode);
	DelayfrTimer(milli, 1);
	
	pWriteData = pWriteBaseData;
	
	srand(eTacs + eTcos + eTacc + eTcoh + eTcah + eTacp);
#if 1	
	for(i=0; i<SROMC_SRAM_SIZE; i++)
	{
#if(SROMC_DATA_PATTERN == SROMC_DATA_RANDOM)
		*pWriteData++ = (u8)rand();
#else	// ONDT_DATA_PATTERN == ONDT_DATA_ONEVALUE
		*pWriteData++ = (u8)(SROMC_DATA_ONEVALUE);
#endif
	}
#else	// rb1004... for 8-bit bus width test(external bus width : 16bit at SMDK)
	{	
		u8 cTemp;
		
		for(i=0; i<SROMC_SRAM_SIZE/2; i++)
		{
#if(SROMC_DATA_PATTERN == SROMC_DATA_RANDOM)
			cTemp = (u8)rand();
			*pWriteData++ = cTemp;
			*pWriteData++ = cTemp;
#else	// ONDT_DATA_PATTERN == ONDT_DATA_ONEVALUE
			*pWriteData++ = (u8)(SROMC_DATA_ONEVALUE);
			*pWriteData++ = (u8)(SROMC_DATA_ONEVALUE);
#endif
		}
	}
#endif

	/////////////////////////////////
	// Write from DRAM to SRAM memory
	/////////////////////////////////
	pWriteData = pWriteBaseData;
	pAddress = pBankBaseAddress;
	
#if (SROMC_TR_TYPE == SROMC_POLLING)
	for(i=0; i<SROMC_SRAM_SIZE; i++)
	{
		*pAddress++ =  *pWriteData++;
	}
#elif (SROMC_TR_TYPE == SROMC_MEMCPY)
	UART_Printf("Memcpy() function is not byte access\n");
	free(pWriteBaseData);
	free(pReadBaseData);
	return 0;
#else
	SROMC_DmaDone = 0;
	DMACH_Setup(DMA_A, 0, (u32)pWriteData, 0, (u32)pAddress, 0, BYTE, SROMC_SRAM_SIZE, DEMAND, MEM, MEM, BURST4, &g_oSROMCDmac0);
	DMACH_Start(&g_oSROMCDmac0);
	while(!SROMC_DmaDone);
#endif

	/////////////////////////////////
	// Read from SRAM memory to DRAM
	/////////////////////////////////
	pReadData = pReadBaseData;
	pAddress = pBankBaseAddress;
	
#if (SROMC_TR_TYPE == SROMC_POLLING)
	for(i=0; i<SROMC_SRAM_SIZE; i++)
	{
		*pReadData++ = *pAddress++;
	}
#else
	SROMC_DmaDone = 0;
	DMACH_Setup(DMA_A, 0, (u32)pAddress, 0, (u32)pReadData, 0, BYTE, SROMC_SRAM_SIZE, DEMAND, MEM, MEM, BURST4, &g_oSROMCDmac0);
	DMACH_Start(&g_oSROMCDmac0);
	while(!SROMC_DmaDone);
#endif			

	/////////////////////////////////
	// Compare Data
	/////////////////////////////////
	pReadData = pReadBaseData;
	pWriteData = pWriteBaseData;
	pAddress = pBankBaseAddress;
	bError = FALSE;
	
	for(i=0 ; i<SROMC_SRAM_SIZE ; i++)
	{
		if(pReadData[i] != pWriteData[i])
		{
			UART_Printf("\t [Address: 0x%08x] [Write data : Read data] = [0x%02x  : 0x%02x ]  \n", pAddress, pWriteData[i], pReadData[i]);
			UART_Getc();
			bError = TRUE;
			break;
		}
		pAddress++;
	}
	if(bError == FALSE)
		UART_Printf("Test OK\n");	

	pWriteData = pWriteBaseData;
	for(i=0; i<SROMC_SRAM_SIZE; i++)
	{
		*pWriteData++ = 0;
	}

	free(pWriteBaseData);
	free(pReadBaseData);
	
	return 0;
}


void SROMC_ByteAccess(void)
{
	u32 uTacs, uTcos, uTacc, uTcoh, uTcah, uTacp;
	SROMC_eBANK uBank;
	Data_eWidth eDWidth;
	WAIT_eCTL eWAITCTL;
	Byte_eCTL eByteCTL;
	Page_eMode ePageMode;

	UART_Printf("[SROMC_ByteAccess]\n");

	UART_Printf("Input the SROMC BANK Number[0~%d] : \n", eBankMAX);
	uBank = (SROMC_eBANK)UART_GetIntNum(); 
	if(uBank > 5)
		uBank = (SROMC_eBANK)0;
	UART_Printf("Input the Data Bus Width : 8-bit(0),   16-bit(1) \n");
	eDWidth = (Data_eWidth)UART_GetIntNum(); 	
	if(eDWidth > 1)
		eDWidth = (Data_eWidth)1;
	UART_Printf("Input the Wait Enable Control : Disable(0),  Enable(1)\n");
	eWAITCTL = (WAIT_eCTL)UART_GetIntNum(); 
	if(eWAITCTL > 1)
		eWAITCTL = (WAIT_eCTL)0;
	UART_Printf("Input the Byte Enable(nBE/nWBE) Setting : Not using(0),  Using(1)\n");
	eByteCTL = (Byte_eCTL)UART_GetIntNum(); 
	if(eByteCTL > 1)
		eByteCTL = (Byte_eCTL)0;
	UART_Printf("Input the Page Mode Setting : Normal(0),  4data(1)\n");
	ePageMode = (Page_eMode)UART_GetIntNum(); 
	if(ePageMode > 1)
		ePageMode = (Page_eMode)0;
	
	SROMC_SetBusWidth( uBank, eDWidth);
	SROMC_SetWaitControl(uBank, eWAITCTL);
	SROMC_SetByteEnable(uBank, eByteCTL);

	uTacs = SROMC_Tacs_MIN;
	uTcos = SROMC_Tcos_MIN;
	uTacc = SROMC_Tacc_MIN;
	uTcoh = SROMC_Tcoh_MIN;
	uTcah = SROMC_Tcah_MIN;
	uTacp = SROMC_Tacp_MIN;

	// Tacp parameter test
	for(uTacp = SROMC_Tacp_MIN ; uTacp <= SROMC_Tacp_MAX ; uTacp++)
	{
		SROMC_ByteWriteReadTest(uBank, (Bank_eTiming)uTacs, (Bank_eTiming)uTcos, (Bank_eTiming)uTacc, 
									(Bank_eTiming)uTcoh, (Bank_eTiming)uTcah, (Bank_eTiming)uTacp, ePageMode);
	}
	uTacp = SROMC_Tacp_MIN;

	// Tcah parameter test
	for(uTcah = SROMC_Tcah_MIN ; uTcah <= SROMC_Tcah_MAX ; uTcah++)
	{
		SROMC_ByteWriteReadTest(uBank, (Bank_eTiming)uTacs, (Bank_eTiming)uTcos, (Bank_eTiming)uTacc, 
									(Bank_eTiming)uTcoh, (Bank_eTiming)uTcah, (Bank_eTiming)uTacp, ePageMode);
	}
	uTcah = SROMC_Tcah_MIN;

	// Tcoh parameter test
	for(uTcoh = SROMC_Tcoh_MIN ; uTcoh <= SROMC_Tcoh_MAX ; uTcoh++)
	{
		SROMC_ByteWriteReadTest(uBank, (Bank_eTiming)uTacs, (Bank_eTiming)uTcos, (Bank_eTiming)uTacc, 
									(Bank_eTiming)uTcoh, (Bank_eTiming)uTcah, (Bank_eTiming)uTacp, ePageMode);
	}
	uTcoh = SROMC_Tcoh_MIN;

	// Tacc parameter test
	for(uTacc = SROMC_Tacc_MIN ; uTacc <= SROMC_Tacc_MAX ; uTacc++)
	{
		SROMC_ByteWriteReadTest(uBank, (Bank_eTiming)uTacs, (Bank_eTiming)uTcos, (Bank_eTiming)uTacc, 
									(Bank_eTiming)uTcoh, (Bank_eTiming)uTcah, (Bank_eTiming)uTacp, ePageMode);
	}
	uTacc = SROMC_Tacc_MIN;

	// Tcos parameter test
	for(uTcos = SROMC_Tcos_MIN ; uTcos <= SROMC_Tcos_MAX ; uTcos++)
	{
		SROMC_ByteWriteReadTest(uBank, (Bank_eTiming)uTacs, (Bank_eTiming)uTcos, (Bank_eTiming)uTacc, 
									(Bank_eTiming)uTcoh, (Bank_eTiming)uTcah, (Bank_eTiming)uTacp, ePageMode);
	}
	uTcos = SROMC_Tcos_MIN;

	// Tacs parameter test
	for(uTacs = SROMC_Tacs_MIN ; uTacs <= SROMC_Tacs_MAX ; uTacs++)
	{
		SROMC_ByteWriteReadTest(uBank, (Bank_eTiming)uTacs, (Bank_eTiming)uTcos, (Bank_eTiming)uTacc, 
									(Bank_eTiming)uTcoh, (Bank_eTiming)uTcah, (Bank_eTiming)uTacp, ePageMode);
	}
	uTacs = SROMC_Tacs_MIN;	

	UART_Printf("\n\n");
}



u32 SROMC_HalfWordWriteReadTest(SROMC_eBANK uBank, Bank_eTiming eTacs, Bank_eTiming eTcos,Bank_eTiming eTacc, 
							 					Bank_eTiming eTcoh, Bank_eTiming eTcah, Bank_eTiming eTacp, Page_eMode ePageMode )
{
	u32 i;
	u8 bError = FALSE;
	u16 *pWriteBaseData, *pReadBaseData, *pWriteData, *pReadData;
	u16 *pBankBaseAddress, *pAddress;

	pWriteBaseData = (u16 *)malloc(SROMC_SRAM_SIZE);
	if(pWriteBaseData == 0)
	{
		UART_Printf("Memory Allocation Error...\n");
		return 0;
	}
	pWriteData = pWriteBaseData;
	
	pReadBaseData = (u16 *)malloc(SROMC_SRAM_SIZE);	
	if(pReadBaseData == 0)
	{
		free(pWriteBaseData);
		UART_Printf("Memory Allocation Error...\n");
		return 0;
	}
	pReadData = pReadBaseData;

	pBankBaseAddress = (u16 *)((uBank * 0x08000000)+0x10000000);
	pAddress = pBankBaseAddress;

#if (SROMC_TR_TYPE == SROMC_DMA)
#if 1	//DMA0 controller
	DMAC_InitCh(DMA0, DMA_ALL, &g_oSROMCDmac0);
	INTC_SetVectAddr(NUM_DMA0,  SROMC_DmaISR);
	INTC_Enable(NUM_DMA0);
#elif 0	//DMA1 controller
	DMAC_InitCh(DMA1, DMA_ALL, &g_oSROMCDmac0);
	INTC_SetVectAddr(NUM_DMA1,  SROMC_DmaISR);
	INTC_Enable(NUM_DMA1);
#elif 0	//SDMA0 controller
	DMAC_InitCh(SDMA0, DMA_ALL, &g_oSROMCDmac0);
	INTC_SetVectAddr(NUM_SDMA0,  SROMC_DmaISR);
	INTC_Enable(NUM_SDMA0);
#else	//SDMA1 controller
	DMAC_InitCh(SDMA1, DMA_ALL, &g_oSROMCDmac0);
	INTC_SetVectAddr(NUM_SDMA1,  SROMC_DmaISR);
	INTC_Enable(NUM_SDMA1);	
#endif	
#endif	

	UART_Printf("Tacs(%d), Tcos(%d), Tacc(%d), Tcoh(%d), Tcah(%d), Tacp(%d)\n", eTacs, eTcos, eTacc, eTcoh, eTcah, eTacp);
	SROMC_SetTiming(uBank, eTacs, eTcos, eTacc, 	eTcoh, eTcah, eTacp, ePageMode);
	DelayfrTimer(milli, 1);
	
	pWriteData = pWriteBaseData;
	
	srand(eTacs + eTcos + eTacc + eTcoh + eTcah + eTacp);
	for(i=0; i<SROMC_SRAM_SIZE/2; i++)
	{
#if(SROMC_DATA_PATTERN == SROMC_DATA_RANDOM)
		*pWriteData++ = (u16)rand();
#else	// ONDT_DATA_PATTERN == ONDT_DATA_ONEVALUE
		*pWriteData++ = SROMC_DATA_ONEVALUE;
#endif
	}

	/////////////////////////////////
	// Write from DRAM to SRAM memory
	/////////////////////////////////
	pWriteData = pWriteBaseData;
	pAddress = pBankBaseAddress;
	
#if (SROMC_TR_TYPE == SROMC_POLLING)
	for(i=0; i<SROMC_SRAM_SIZE/2; i++)
	{
		*pAddress++ =  *pWriteData++;
	}
#elif (SROMC_TR_TYPE == SROMC_MEMCPY)
	UART_Printf("Memcpy() function is not byte access\n");
	free(pWriteBaseData);
	free(pReadBaseData);
	return 0;
#else
	SROMC_DmaDone = 0;
	DMACH_Setup(DMA_A, 0, (u32)pWriteData, 0, (u32)pAddress, 0, HWORD, SROMC_SRAM_SIZE/2, DEMAND, MEM, MEM, BURST4, &g_oSROMCDmac0);
	DMACH_Start(&g_oSROMCDmac0);
	while(!SROMC_DmaDone);
#endif

	/////////////////////////////////
	// Read from SRAM memory to DRAM
	/////////////////////////////////
	pReadData = pReadBaseData;
	pAddress = pBankBaseAddress;
	
#if (SROMC_TR_TYPE == SROMC_POLLING)
	for(i=0; i<SROMC_SRAM_SIZE/2; i++)
	{
		*pReadData++ = *pAddress++;
	}
#else
	SROMC_DmaDone = 0;
	DMACH_Setup(DMA_A, 0, (u32)pAddress, 0, (u32)pReadData, 0, HWORD, SROMC_SRAM_SIZE/2, DEMAND, MEM, MEM, BURST4, &g_oSROMCDmac0);
	DMACH_Start(&g_oSROMCDmac0);
	while(!SROMC_DmaDone);
#endif			

	/////////////////////////////////
	// Compare Data
	/////////////////////////////////
	pReadData = pReadBaseData;
	pWriteData = pWriteBaseData;
	pAddress = pBankBaseAddress;
	bError = FALSE;
	
	for(i=0 ; i<SROMC_SRAM_SIZE/2 ; i++)
	{
		if(pReadData[i] != pWriteData[i])
		{
			UART_Printf("\t [Address: 0x%08x] [Write data : Read data] = [0x%04x  : 0x%04x ]  \n", pAddress, pWriteData[i], pReadData[i]);
			UART_Getc();
			bError = TRUE;
			break;
		}
		pAddress++;
	}
	if(bError == FALSE)
		UART_Printf("Test OK\n");	

	pWriteData = pWriteBaseData;
	for(i=0; i<SROMC_SRAM_SIZE/2; i++)
	{
		*pWriteData++ = 0;
	}

	free(pWriteBaseData);
	free(pReadBaseData);
	
	return 0;
}


void SROMC_HalfWordAccess(void)
{
	u32 uTacs, uTcos, uTacc, uTcoh, uTcah, uTacp;
	SROMC_eBANK uBank;
	Data_eWidth eDWidth;
	WAIT_eCTL eWAITCTL;
	Byte_eCTL eByteCTL;
	Page_eMode ePageMode;

	UART_Printf("[SROMC_HalfWordAccess]\n");

	UART_Printf("Input the SROMC BANK Number[0~%d] : \n", eBankMAX);
	uBank = (SROMC_eBANK)UART_GetIntNum(); 
	if(uBank > 5)
		uBank = (SROMC_eBANK)0;
	UART_Printf("Input the Data Bus Width : 8-bit(0),   16-bit(1) \n");
	eDWidth = (Data_eWidth)UART_GetIntNum(); 	
	if(eDWidth > 1)
		eDWidth = (Data_eWidth)1;
	UART_Printf("Input the Wait Enable Control : Disable(0),  Enable(1)\n");
	eWAITCTL = (WAIT_eCTL)UART_GetIntNum(); 
	if(eWAITCTL > 1)
		eWAITCTL = (WAIT_eCTL)0;
	UART_Printf("Input the Byte Enable(nBE/nWBE) Setting : Not using(0),  Using(1)\n");
	eByteCTL = (Byte_eCTL)UART_GetIntNum(); 
	if(eByteCTL > 1)
		eByteCTL = (Byte_eCTL)0;
	UART_Printf("Input the Page Mode Setting : Normal(0),  4data(1)\n");
	ePageMode = (Page_eMode)UART_GetIntNum(); 
	if(ePageMode > 1)
		ePageMode = (Page_eMode)0;
	
	SROMC_SetBusWidth( uBank, eDWidth);
	SROMC_SetWaitControl(uBank, eWAITCTL);
	SROMC_SetByteEnable(uBank, eByteCTL);

	uTacs = SROMC_Tacs_MIN;
	uTcos = SROMC_Tcos_MIN;
	uTacc = SROMC_Tacc_MIN;
	uTcoh = SROMC_Tcoh_MIN;
	uTcah = SROMC_Tcah_MIN;
	uTacp = SROMC_Tacp_MIN;

	// Tacp parameter test
	for(uTacp = SROMC_Tacp_MIN ; uTacp <= SROMC_Tacp_MAX ; uTacp++)
	{
		SROMC_HalfWordWriteReadTest(uBank, (Bank_eTiming)uTacs, (Bank_eTiming)uTcos, (Bank_eTiming)uTacc, 
									(Bank_eTiming)uTcoh, (Bank_eTiming)uTcah, (Bank_eTiming)uTacp, ePageMode);
	}
	uTacp = SROMC_Tacp_MIN;

	// Tcah parameter test
	for(uTcah = SROMC_Tcah_MIN ; uTcah <= SROMC_Tcah_MAX ; uTcah++)
	{
		SROMC_HalfWordWriteReadTest(uBank, (Bank_eTiming)uTacs, (Bank_eTiming)uTcos, (Bank_eTiming)uTacc, 
									(Bank_eTiming)uTcoh, (Bank_eTiming)uTcah, (Bank_eTiming)uTacp, ePageMode);
	}
	uTcah = SROMC_Tcah_MIN;

	// Tcoh parameter test
	for(uTcoh = SROMC_Tcoh_MIN ; uTcoh <= SROMC_Tcoh_MAX ; uTcoh++)
	{
		SROMC_HalfWordWriteReadTest(uBank, (Bank_eTiming)uTacs, (Bank_eTiming)uTcos, (Bank_eTiming)uTacc, 
									(Bank_eTiming)uTcoh, (Bank_eTiming)uTcah, (Bank_eTiming)uTacp, ePageMode);
	}
	uTcoh = SROMC_Tcoh_MIN;

	// Tacc parameter test
	for(uTacc = SROMC_Tacc_MIN ; uTacc <= SROMC_Tacc_MAX ; uTacc++)
	{
		SROMC_HalfWordWriteReadTest(uBank, (Bank_eTiming)uTacs, (Bank_eTiming)uTcos, (Bank_eTiming)uTacc, 
									(Bank_eTiming)uTcoh, (Bank_eTiming)uTcah, (Bank_eTiming)uTacp, ePageMode);
	}
	uTacc = SROMC_Tacc_MIN;

	// Tcos parameter test
	for(uTcos = SROMC_Tcos_MIN ; uTcos <= SROMC_Tcos_MAX ; uTcos++)
	{
		SROMC_HalfWordWriteReadTest(uBank, (Bank_eTiming)uTacs, (Bank_eTiming)uTcos, (Bank_eTiming)uTacc, 
									(Bank_eTiming)uTcoh, (Bank_eTiming)uTcah, (Bank_eTiming)uTacp, ePageMode);
	}
	uTcos = SROMC_Tcos_MIN;

	// Tacs parameter test
	for(uTacs = SROMC_Tacs_MIN ; uTacs <= SROMC_Tacs_MAX ; uTacs++)
	{
		SROMC_HalfWordWriteReadTest(uBank, (Bank_eTiming)uTacs, (Bank_eTiming)uTcos, (Bank_eTiming)uTacc, 
									(Bank_eTiming)uTcoh, (Bank_eTiming)uTcah, (Bank_eTiming)uTacp, ePageMode);
	}
	uTacs = SROMC_Tacs_MIN;	

	UART_Printf("\n\n");
}



extern void STOPT_Basic(void);
void SROMCT_System_PowerMode(void)
{
	u32 uPowerMode;
	
	UART_Printf("[SROMCT_System_PowerMode]\n");
	
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


//////////
// Function Name : SROMC_InitForMultiAccessTest
// Function Description : Simultaneous Access through the EBI
// Version : v0.1
// rb1004
u32 *pMAWriteBaseData, *pMAReadBaseData, *pMAWriteData, *pMAReadData;
u32 *pMABankBaseAddress, *pMAAddress;

void SROMC_InitForMultiWriteTest(SROMC_eBANK uBank, Data_eWidth eDWidth, WAIT_eCTL eWAITCTL, Byte_eCTL eByteCTL)
{
	u32 uTacs, uTcos, uTacc, uTcoh, uTcah, uTacp;
	u32 i;
	
	SROMC_Init();

	SROMC_SetBusWidth( uBank, eDWidth);
	SROMC_SetWaitControl(uBank, eWAITCTL);
	SROMC_SetByteEnable(uBank, eByteCTL);

	pMAWriteBaseData = (u32 *)malloc(SROMC_SRAM_SIZE);
	if(pMAWriteBaseData == 0)
	{
		UART_Printf("Memory Allocation Error...\n");
		return;
	}
	pMAWriteData = pMAWriteBaseData;
	
	pMAReadBaseData = (u32 *)malloc(SROMC_SRAM_SIZE);	
	if(pMAReadBaseData == 0)
	{
		free(pMAWriteBaseData);
		UART_Printf("Memory Allocation Error...\n");
		return;
	}
	pMAReadData = pMAReadBaseData;

	pMABankBaseAddress = (u32 *)((uBank * 0x08000000)+0x10000000);
	pMAAddress = pMABankBaseAddress;

	uTacs = SROMC_Tacs_MIN;
	uTcos = SROMC_Tcos_MIN;
	uTacc = SROMC_Tacc_MIN;
	uTcoh = SROMC_Tcoh_MIN;
	uTcah = SROMC_Tcah_MIN;

	SROMC_SetTiming(uBank, (Bank_eTiming)uTacs, (Bank_eTiming)uTcos, (Bank_eTiming)uTacc, 
								(Bank_eTiming)uTcoh, (Bank_eTiming)uTcah, (Bank_eTiming)uTacp, eNor_Mode);
	DelayfrTimer(milli, 1);
	
	pMAWriteData = pMAWriteBaseData;
	
	srand(uTacs + uTcos + uTacc + uTcoh + uTcah + uTacp);

	for(i=0; i<SROMC_SRAM_SIZE/4; i++)
	{
#if(SROMC_DATA_PATTERN == SROMC_DATA_RANDOM)
		*pMAWriteData++ = rand();
#else	// ONDT_DATA_PATTERN == ONDT_DATA_ONEVALUE
		*pMAWriteData++ = SROMC_DATA_ONEVALUE;
#endif
	}
}


void SROMC_MultiWriteTest(void)
{
	//DMA0 controller
	DMAC_InitCh(DMA0, DMA_ALL, &g_oSROMCDmac0);
	INTC_SetVectAddr(NUM_DMA0,  SROMC_MADmaISR);
	INTC_Enable(NUM_DMA0);
	
	/////////////////////////////////
	// Write from DRAM to SRAM memory
	/////////////////////////////////
	pMAWriteData = pMAWriteBaseData;
	pMAAddress = pMABankBaseAddress;
	
	SROMC_DmaDone = 0;
	DMACH_Setup(DMA_A, 0, (u32)pMAWriteData, 0, (u32)pMAAddress, 0, WORD, SROMC_SRAM_SIZE/4, DEMAND, MEM, MEM, BURST4, &g_oSROMCDmac0);
}


void SROMC_StartMultiWriteTest(void)
{
	DMACH_Start(&g_oSROMCDmac0);
}


void SROMC_CheckDoneMultiWriteTest(void)
{
	while(!SROMC_DmaDone);
}


void SROMC_MultiReadTest(void)
{
	//DMA0 controller
	DMAC_InitCh(DMA0, DMA_ALL, &g_oSROMCDmac0);
	INTC_SetVectAddr(NUM_DMA0,  SROMC_DmaISR);
	INTC_Enable(NUM_DMA0);

	/////////////////////////////////
	// Read from SRAM memory to DRAM
	/////////////////////////////////
	
	pMAReadData = pMAReadBaseData;
	pMAAddress = pMABankBaseAddress;

	SROMC_DmaDone = 0;
	DMACH_Setup(DMA_A, 0, (u32)pMAAddress, 0, (u32)pMAReadData, 0, WORD, SROMC_SRAM_SIZE/4, DEMAND, MEM, MEM, BURST4, &g_oSROMCDmac0);
}


void SROMC_StartMultiReadTest(void)
{
	DMACH_Start(&g_oSROMCDmac0);
}


void SROMC_CheckDoneMultiReadTest(void)
{
	while(!SROMC_DmaDone);
}


void SROMC_VerifyMultiAccessTest(void)
{
	u32 i;
	u8 bError = FALSE;
	
	/////////////////////////////////
	// Compare Data
	/////////////////////////////////
	pMAReadData = pMAReadBaseData;
	pMAWriteData = pMAWriteBaseData;
	pMAAddress = pMABankBaseAddress;
	bError = FALSE;
	
	for(i=0 ; i<SROMC_SRAM_SIZE/4 ; i++)
	{
		if(pMAReadData[i] != pMAWriteData[i])
		{
			UART_Printf("\t [Address: 0x%08x] [Write data : Read data] = [0x%08x  : 0x%08x ]  \n", pMAAddress, pMAWriteData[i], pMAReadData[i]);
			UART_Getc();
			bError = TRUE;
			break;
		}
		pMAAddress++;
	}
	if(bError == FALSE)
		UART_Printf("SROMC Test OK\n");	

	pMAWriteData = pMAWriteBaseData;
	pMAAddress = pMABankBaseAddress;
	
	for(i=0; i<SROMC_SRAM_SIZE/4; i++)
	{
		*pMAWriteData++ = 0;
		*pMAAddress++ = 0;
	}

	free(pMAWriteBaseData);
	free(pMAReadBaseData);

}


void SROMC_MultiAccessTestExample(void)
{
	SROMC_InitForMultiWriteTest(eBank1, e16bit, eDis_WAIT, eEn_CTL);
	SROMC_MultiWriteTest();
	// External IP DMA Write Start 
	SROMC_CheckDoneMultiWriteTest();
	// External IP DMA Write done check
	
	SROMC_MultiReadTest();
	// External IP DMA Read Start 
	SROMC_CheckDoneMultiReadTest();
	// External IP DMA Read done check

	SROMC_VerifyMultiAccessTest();
}


/*
void SROMC_Test(void)
{
	u32 uLoopCnt = 0;
	u32 uTempAddr = 0;

//	SYSC_CtrlEBIPrio(eEBI_FIX4);

	
#if 1
	EBI_SetDMAParams(eMEM_SROMC);

	//-------- SRAM setting --------------
	uTempAddr = g_oaEBIInform[eMEM_SROMC].uDstAddr;
	for(uLoopCnt=0 ; uLoopCnt<SRAM_DATA_SIZE ; uLoopCnt++)
	{
		*(volatile u32*)(uTempAddr) = 0;
		uTempAddr += 4;
	}

	UART_Printf("* Source       Buffer Address : 0x%x\n", g_oaEBIInform[eMEM_SROMC].uSrcAddr);
	UART_Printf("* Destination Buffer Address : 0x%x\n", g_oaEBIInform[eMEM_SROMC].uDstAddr);
	UART_Printf("If you want to exit, Press the 'x' key.\n");
        // Enable DMAs
	DMACH_Start(&g_oaEBIInform[eMEM_SROMC].oEBIDma);	// SROMC DMA Start
#endif
	while(1)
	{
		if(Getc()=='x')
		{
			break;
		}
	}

	EBI_CloseDMA(eMEM_SROMC);
}
*/

/*---------------------------------- ISR Routines ---------------------------------*/
//////////
// Function Name : ISR_EBIDMC0
// Function Description : This function implements the ISR of DMC0's DMA Interrupt.
// Input : 	NONE
// Output : 	NONE
// Version : v0.1
void __irq ISR_EBIDMC0(void)
{
	UART_Printf("ISR_EBIDMC0\n");
	
	INTC_Disable(g_oaEBIInform[eMEM_DMC0].ucIntNum);

	// Interrupt Clear
	DMACH_ClearIntPending(&g_oaEBIInform[eMEM_DMC0].oEBIDma);
	DMACH_ClearErrIntPending(&g_oaEBIInform[eMEM_DMC0].oEBIDma);

        // Channel Set-up
	DMACH_Setup(	g_oaEBIInform[eMEM_DMC0].eDMACh, 0x0, 
					g_oaEBIInform[eMEM_DMC0].uSrcAddr, g_oaEBIInform[eMEM_DMC0].bSrcFixed, 
					g_oaEBIInform[eMEM_DMC0].uDstAddr, g_oaEBIInform[eMEM_DMC0].bDstFixed, 
					g_oaEBIInform[eMEM_DMC0].eDataSz, g_oaEBIInform[eMEM_DMC0].uDataCnt, DEMAND, 
					g_oaEBIInform[eMEM_DMC0].eSrcReq, g_oaEBIInform[eMEM_DMC0].eDstReq, 
					g_oaEBIInform[eMEM_DMC0].eBurstMode, &g_oaEBIInform[eMEM_DMC0].oEBIDma);

	DMACH_Start(&g_oaEBIInform[eMEM_DMC0].oEBIDma);	// DMA Re-Start

	INTC_Enable(g_oaEBIInform[eMEM_DMC0].ucIntNum);

	INTC_ClearVectAddr();

}

//////////
// Function Name : ISR_EBISROMC
// Function Description : This function implements the ISR of DMC0's DMA Interrupt.
// Input : 	NONE
// Output : 	NONE
// Version : v0.1
void __irq ISR_EBISROMC(void)
{
//	UART_Printf("ISR_EBISROMC\n");
	
	INTC_Disable(g_oaEBIInform[eMEM_SROMC].ucIntNum);

	// Interrupt Clear
	DMACH_ClearIntPending(&g_oaEBIInform[eMEM_SROMC].oEBIDma);
	DMACH_ClearErrIntPending(&g_oaEBIInform[eMEM_SROMC].oEBIDma);

	if (CompareDMA(g_oaEBIInform[eMEM_SROMC].uSrcAddr, g_oaEBIInform[eMEM_SROMC].uDstAddr, (DATA_SIZE)WORD, SRAM_DATA_SIZE-1))
	{
//		UART_Printf(" >> Test Tx&Rx -> Ok << \n");
		UART_Printf("!");

	        // Channel Set-up
		DMACH_Setup(	g_oaEBIInform[eMEM_SROMC].eDMACh, 0x0, 
						g_oaEBIInform[eMEM_SROMC].uSrcAddr, g_oaEBIInform[eMEM_SROMC].bSrcFixed, 
						g_oaEBIInform[eMEM_SROMC].uDstAddr, g_oaEBIInform[eMEM_SROMC].bDstFixed, 
						g_oaEBIInform[eMEM_SROMC].eDataSz, g_oaEBIInform[eMEM_SROMC].uDataCnt, DEMAND, 
						g_oaEBIInform[eMEM_SROMC].eSrcReq, g_oaEBIInform[eMEM_SROMC].eDstReq, 
						g_oaEBIInform[eMEM_SROMC].eBurstMode, &g_oaEBIInform[eMEM_SROMC].oEBIDma);

		DMACH_Start(&g_oaEBIInform[eMEM_SROMC].oEBIDma);	// DMA Re-Start

		INTC_Enable(g_oaEBIInform[eMEM_SROMC].ucIntNum);

		INTC_ClearVectAddr();
	}
	else
	{
		UART_Printf(" >>*** Tx-data & Rx-data mismatch ***<< \n");
	}
}

//////////
// Function Name : ISR_EBINAND
// Function Description : This function implements the ISR of NAND's DMA Interrupt.
// Input : 	NONE
// Output : 	NONE
// Version : v0.1
void __irq ISR_EBINAND(void)
{
	UART_Printf("N ");
	
	INTC_Disable(g_oaEBIInform[eMEM_NAND].ucIntNum);

	// Interrupt Clear
	DMACH_ClearIntPending(&g_oaEBIInform[eMEM_NAND].oEBIDma);
	DMACH_ClearErrIntPending(&g_oaEBIInform[eMEM_NAND].oEBIDma);

        // Channel Set-up
	DMACH_Setup(	g_oaEBIInform[eMEM_NAND].eDMACh, 0x0, 
					g_oaEBIInform[eMEM_NAND].uSrcAddr, g_oaEBIInform[eMEM_NAND].bSrcFixed, 
					g_oaEBIInform[eMEM_NAND].uDstAddr, g_oaEBIInform[eMEM_NAND].bDstFixed, 
					g_oaEBIInform[eMEM_NAND].eDataSz, g_oaEBIInform[eMEM_NAND].uDataCnt, DEMAND, 
					g_oaEBIInform[eMEM_NAND].eSrcReq, g_oaEBIInform[eMEM_NAND].eDstReq, 
					g_oaEBIInform[eMEM_NAND].eBurstMode, &g_oaEBIInform[eMEM_NAND].oEBIDma);

	NAND_ReadPageSLCSetup(0, 50, 0);
	DMACH_Start(&g_oaEBIInform[eMEM_NAND].oEBIDma);	// DMA Re-Start
	INTC_Enable(g_oaEBIInform[eMEM_NAND].ucIntNum);

	INTC_ClearVectAddr();

//	DelayfrTimer(milli, 1);

}

//////////
// Function Name : ISR_EBINANDLLI
// Function Description : This function implements the ISR of NAND's DMA Interrupt.
// Input : 	NONE
// Output : 	NONE
// Version : v0.1
void __irq ISR_EBINANDLLI(void)
{
	INTC_Disable(g_oaEBIInform[eMEM_NAND].ucIntNum);
	UART_Printf(".");

//	NAND_WritePageSLCClose(0, aSpareBuffer);

	// Interrupt Clear
	DMACH_ClearIntPending(&g_oaEBIInform[eMEM_NAND].oEBIDma);
	DMACH_ClearErrIntPending(&g_oaEBIInform[eMEM_NAND].oEBIDma);

	Nand_DmaTestDone++;

	INTC_Enable(g_oaEBIInform[eMEM_NAND].ucIntNum);

	INTC_ClearVectAddr();

}

//////////
// Function Name : ISR_EBIOneNAND
// Function Description : This function implements the ISR of OneNAND's DMA Interrupt.
// Input : 	NONE
// Output : 	NONE
// Version : v0.1
void __irq ISR_EBIOneNAND(void)
{
	UART_Printf("ISR_EBIOneNAND\n");
	
	INTC_Disable(g_oaEBIInform[eMEM_OneNAND].ucIntNum);

	// Interrupt Clear
	DMACH_ClearIntPending(&g_oaEBIInform[eMEM_OneNAND].oEBIDma);
	DMACH_ClearErrIntPending(&g_oaEBIInform[eMEM_OneNAND].oEBIDma);

        // Channel Set-up
	DMACH_Setup(	g_oaEBIInform[eMEM_OneNAND].eDMACh, 0x0, 
					g_oaEBIInform[eMEM_OneNAND].uSrcAddr, g_oaEBIInform[eMEM_OneNAND].bSrcFixed, 
					g_oaEBIInform[eMEM_OneNAND].uDstAddr, g_oaEBIInform[eMEM_OneNAND].bDstFixed, 
					g_oaEBIInform[eMEM_OneNAND].eDataSz, g_oaEBIInform[eMEM_OneNAND].uDataCnt, DEMAND, 
					g_oaEBIInform[eMEM_OneNAND].eSrcReq, g_oaEBIInform[eMEM_OneNAND].eDstReq, 
					g_oaEBIInform[eMEM_OneNAND].eBurstMode, &g_oaEBIInform[eMEM_OneNAND].oEBIDma);

	NAND_ReadPageSLCSetup(0, 50, 0);
	DMACH_Start(&g_oaEBIInform[eMEM_OneNAND].oEBIDma);	// DMA Re-Start
	INTC_Enable(g_oaEBIInform[eMEM_OneNAND].ucIntNum);

	INTC_ClearVectAddr();

//	DelayfrTimer(milli, 1);

}

void EBI_SetDMAParams(Mem_eType eMemType)
{
	u32	uLoopCnt = 0;
	u32	uTotTxferBytes = 0;
	s32	iBankSel = 0;
	
	//------- Init DMA params
	switch(eMemType)
	{
		case eMEM_DMC0 :			
			UART_Printf("Selected DMAC 0 ..... \n");
			g_oaEBIInform[eMemType].ucIntNum		= NUM_DMA0;
			g_oaEBIInform[eMemType].eDMACon 		= DMA0;
			g_oaEBIInform[eMemType].eDMACh 		= DMA_A;
			g_oaEBIInform[eMemType].uSrcAddr 		= _DRAM_BaseAddress + 0x01000000;
			g_oaEBIInform[eMemType].uDstAddr 		= _DRAM_BaseAddress + 0x04000000;
			g_oaEBIInform[eMemType].bSrcFixed		= eINCREMENT;
			g_oaEBIInform[eMemType].bDstFixed		= eINCREMENT;
			g_oaEBIInform[eMemType].eDataSz		= WORD;		// Set DMA Data Width
			g_oaEBIInform[eMemType].uDataCnt		= 0x10000;		// Set DMA Transfer Count	
			g_oaEBIInform[eMemType].eSrcReq		= MEM;
			g_oaEBIInform[eMemType].eDstReq		= MEM;
			g_oaEBIInform[eMemType].eBurstMode		= SINGLE;	
			g_oaEBIInform[eMemType].pHandler		= ISR_EBIDMC0;

			uTotTxferBytes	= g_oaEBIInform[eMemType].uDataCnt * g_oaEBIInform[eMemType].eDataSz;

			// 0. Clear the rx buf.
			for (uLoopCnt = 0; uLoopCnt<uTotTxferBytes; uLoopCnt++)
				*(u8 *)(g_oaEBIInform[eMemType].uDstAddr+uLoopCnt) = 0;

			// 1. Set up the tx buf.
			for (uLoopCnt = 0; uLoopCnt<uTotTxferBytes; uLoopCnt++)
				*(u8 *)(g_oaEBIInform[eMemType].uSrcAddr+uLoopCnt) = (u8)(uLoopCnt+2)%0xff;	
			
			break;
		case eMEM_NAND :
			g_oaEBIInform[eMemType].ucIntNum		= NUM_DMA1;
			g_oaEBIInform[eMemType].eDMACon 		= DMA1;
			g_oaEBIInform[eMemType].eDMACh 		= DMA_A;
//			g_oaEBIInform[eMemType].uLLIBaseAddr	= 0x0;
			g_oaEBIInform[eMemType].uLLIBaseAddr	= g_uLLIBaseAddr+0x20;
			g_oaEBIInform[eMemType].uSrcAddr 		= 0x52000000;		
			g_oaEBIInform[eMemType].uDstAddr 		= (u32)(NFCON_BASE + 0x10);	// NFDATA register address
			g_oaEBIInform[eMemType].bSrcFixed		= eINCREMENT;
			g_oaEBIInform[eMemType].bDstFixed		= eFIX;
			g_oaEBIInform[eMemType].eDataSz		= WORD;					// Set DMA Data Width
//			g_oaEBIInform[eMemType].uDataCnt		= NAND_DATA_SIZE;		// Set DMA Transfer Count	
			g_oaEBIInform[eMemType].uDataCnt		= 64;		// Set DMA Transfer Count	
			g_oaEBIInform[eMemType].eSrcReq		= MEM;
			g_oaEBIInform[eMemType].eDstReq		= DMA1_NAND_RX;
			g_oaEBIInform[eMemType].eBurstMode		= SINGLE;	
//			g_oaEBIInform[eMemType].eBurstMode		= BURST128;	
			g_oaEBIInform[eMemType].pHandler		= ISR_EBINANDLLI;			
			break;

		case eMEM_SROMC :			
			UART_Printf("Selected SDMAC 0 ..... \n");
			UART_Printf("0)nCS0	1)nCS1	4)nCS4	5)nCS5\n");
			UART_Printf("\nSelect SROM Bank(\"0\" to exit) : ");
			iBankSel = UART_GetIntNum();
			if( (iBankSel < 0) || (iBankSel == 2) || (iBankSel == 3) || (iBankSel > 5)) 
			    return;		// return.

			SROMC_SetBank((u8)iBankSel, eEn_CTL, eEn_WAIT, e16bit, eNor_Mode, (Bank_eTiming)0, (Bank_eTiming)3, (Bank_eTiming)20, (Bank_eTiming)0, (Bank_eTiming)0, (Bank_eTiming)2);
			
			g_oaEBIInform[eMemType].ucIntNum		= NUM_SDMA0;
			g_oaEBIInform[eMemType].eDMACon 		= SDMA0;
			g_oaEBIInform[eMemType].eDMACh 		= DMA_A;
			g_oaEBIInform[eMemType].uSrcAddr 		= _SMC_BaseAddress + 0x08000000*iBankSel;
			g_oaEBIInform[eMemType].uDstAddr 		= _DRAM_BaseAddress + 0x04000000;
			g_oaEBIInform[eMemType].bSrcFixed		= eINCREMENT;
			g_oaEBIInform[eMemType].bDstFixed		= eINCREMENT;
			g_oaEBIInform[eMemType].eDataSz		= WORD;		// Set DMA Data Width
			g_oaEBIInform[eMemType].uDataCnt		= SRAM_DATA_SIZE;	// Set DMA Transfer Count	
			g_oaEBIInform[eMemType].eSrcReq		= MEM;
			g_oaEBIInform[eMemType].eDstReq		= MEM;
			g_oaEBIInform[eMemType].eBurstMode		= SINGLE;	
			g_oaEBIInform[eMemType].pHandler		= ISR_EBISROMC;

			uTotTxferBytes	= g_oaEBIInform[eMemType].uDataCnt * g_oaEBIInform[eMemType].eDataSz;

			// 0. Clear the rx buf.
			for (uLoopCnt = 0; uLoopCnt<uTotTxferBytes; uLoopCnt++)
				*(u8 *)(g_oaEBIInform[eMemType].uDstAddr+uLoopCnt) = 0;

			// 1. Set up the tx buf.
			for (uLoopCnt = 0; uLoopCnt<uTotTxferBytes; uLoopCnt++)
				*(u8 *)(g_oaEBIInform[eMemType].uSrcAddr+uLoopCnt) = (u8)(uLoopCnt+2)%0xff;	
			
			break;
		case eMEM_OneNAND :
			UART_Printf("Selected SDMAC 1 ..... \n");
			g_oaEBIInform[eMemType].ucIntNum		= NUM_SDMA1;
			g_oaEBIInform[eMemType].eDMACon 		= SDMA1;
			g_oaEBIInform[eMemType].eDMACh 		= DMA_A;
			g_oaEBIInform[eMemType].uSrcAddr 		= g_uOneNANDSrcAddr;		// NFDATA register address
			g_oaEBIInform[eMemType].uDstAddr 		= (u32)aReadData;
			g_oaEBIInform[eMemType].bSrcFixed		= eFIX;
			g_oaEBIInform[eMemType].bDstFixed		= eINCREMENT;
			g_oaEBIInform[eMemType].eDataSz		= WORD;					// Set DMA Data Width
			g_oaEBIInform[eMemType].uDataCnt		= ONENAND_PAGESIZE/4;		// Set DMA Transfer Count	
			g_oaEBIInform[eMemType].eSrcReq		= MEM;
			g_oaEBIInform[eMemType].eDstReq		= MEM;
			g_oaEBIInform[eMemType].eBurstMode		= BURST128;	
			g_oaEBIInform[eMemType].pHandler		= ISR_EBIOneNAND;

			uTotTxferBytes	= g_oaEBIInform[eMemType].uDataCnt * g_oaEBIInform[eMemType].eDataSz;

			// 0. Clear the rx buf.
//			for (uLoopCnt = 0; uLoopCnt<uTotTxferBytes; uLoopCnt++)
//				*(u8 *)(g_oaEBIInform[eMemType].uDstAddr+uLoopCnt) = 0;

			// 1. Set up the tx buf.
			for (uLoopCnt = 0; uLoopCnt<uTotTxferBytes; uLoopCnt++)
				*(u8 *)(g_oaEBIInform[eMemType].uSrcAddr+uLoopCnt) = (u8)(uLoopCnt+2)%0xff;	
			
			break;
	}
	
	DMAC_InitCh(g_oaEBIInform[eMemType].eDMACon, g_oaEBIInform[eMemType].eDMACh, &g_oaEBIInform[eMemType].oEBIDma);
	INTC_SetVectAddr(g_oaEBIInform[eMemType].ucIntNum,  g_oaEBIInform[eMemType].pHandler);
	INTC_Enable(g_oaEBIInform[eMemType].ucIntNum);

        // Channel, LLI_Address, SrcAddr, Src Type, DstAddr, Dst Type, Transfer Width, Transfer Size, OpMode(DEMAND), Src Req, Dst Req, Burst
        // Channel Set-up
	DMACH_Setup(	g_oaEBIInform[eMemType].eDMACh, g_oaEBIInform[eMemType].uLLIBaseAddr, 
					g_oaEBIInform[eMemType].uSrcAddr, g_oaEBIInform[eMemType].bSrcFixed, 
					g_oaEBIInform[eMemType].uDstAddr, g_oaEBIInform[eMemType].bDstFixed, 
					g_oaEBIInform[eMemType].eDataSz, g_oaEBIInform[eMemType].uDataCnt, DEMAND, 
					g_oaEBIInform[eMemType].eSrcReq, g_oaEBIInform[eMemType].eDstReq, 
					g_oaEBIInform[eMemType].eBurstMode, &g_oaEBIInform[eMemType].oEBIDma);
	
}

void EBI_CloseDMA(Mem_eType eMemType)
{
	INTC_Disable(g_oaEBIInform[eMemType].ucIntNum);
    	DMAC_Close(g_oaEBIInform[eMemType].eDMACon, g_oaEBIInform[eMemType].eDMACh, &g_oaEBIInform[eMemType].oEBIDma);	
}



#define SMC_NAND_DMA_TEST 1
#define SMC_SRAM_DMA_TEST 1
#define SMC_UDMA_TEST 0

//////////
// Function Name : SMC_TestEBIUsingNAND
// Function Description : Memory Subsystem Test(EBI Test)
//						1. DMC0				
//						2. SROMC using SRAM	: nCS1
//						3. NAND Flash			: nCS2
// Input : 	None
// Output :	None 
// Version : v0.1
extern void NANDT_Init(void);
void SMC_TestEBIUsingNAND(void)
{
	u32 uLoopCnt = 0;
	u32 uBlock = 0;
	u32 uPage = 0;
	u32 uTempAddr = 0;

	SYSC_CtrlEBIPrio(eEBI_FIX4);
#if (SMC_UDMA_TEST==1)
	//-------- CF Card Setting ----------------
	if (!ATA_Init(ATA_CON0, DIRECT_MODE))
	{
		UART_Printf("Fail to initialize ATA Mode...\n");
		return;
	}
	DelayfrTimer(milli, 100);
	ATA_IdentifyDevice(ATA_CON0);	
	DelayfrTimer(milli, 100);
	ATA_OpenMedia(ATA_CON0, eATA_MODE_UDMA);
#endif	
#if (SMC_NAND_DMA_TEST==1)
	//-------- Nand Flash setting --------------
	for(uLoopCnt=0 ; uLoopCnt<NAND_DATA_SIZE ; uLoopCnt++)
		*(unsigned char *)(0x52000000+uLoopCnt) = eATA_XFR_CMD_START;
	for(uLoopCnt=0 ; uLoopCnt<NAND_SPARE_16 ; uLoopCnt++)
		aSpareBuffer[uLoopCnt] = 0xFF;

	uBlock = 50;
	uPage = 0;

	NANDT_Init();

	NAND_WritePageSLCSetup(0, uBlock, uPage);
	Nand_DmaTestDone = 0;
	EBI_SetDMAParams(eMEM_NAND); 
	// Set LLI parameters for DMA loop operation
	SMC_SetLLI(g_oaEBIInform[eMEM_NAND].uSrcAddr, g_oaEBIInform[eMEM_NAND].uDstAddr, 
				(DATA_SIZE)g_oaEBIInform[eMEM_NAND].eDataSz , (BURST_MODE)g_oaEBIInform[eMEM_NAND].eBurstMode, g_oaEBIInform[eMEM_NAND].uDataCnt);
#endif 

#if (SMC_SRAM_DMA_TEST==1)
	//-------- SROMC(SRAM) setting --------------
	EBI_SetDMAParams(eMEM_SROMC);

	uTempAddr = g_oaEBIInform[eMEM_SROMC].uDstAddr;
	for(uLoopCnt=0 ; uLoopCnt<SRAM_DATA_SIZE ; uLoopCnt++)
		*(volatile u32*)(uTempAddr+4) = 0;
       
#endif

#if (SMC_SRAM_DMA_TEST==1)
	DMACH_Start(&g_oaEBIInform[eMEM_SROMC].oEBIDma);	// SROMC DMA Start
#endif	
#if (SMC_NAND_DMA_TEST==1)
	DMACH_Start(&g_oaEBIInform[eMEM_NAND].oEBIDma);	// NAND DMA Start
#endif
#if (SMC_UDMA_TEST==1)
	ATA_TestUDmaMode_Int_for_EBI();
#endif
#if (SMC_NAND_DMA_TEST==1)
	while(1)
	{
		if(Getc()=='x')
		{
			break;
		}
	}
	while(!Nand_DmaTestDone);	

	NAND_WritePageSLCClose(0, aSpareBuffer);
#endif
#if (SMC_UDMA_TEST==1)
	ATA_CloseMedia(ATA_CON0);
	ATA_SetEnable(ATA_CON0, DISABLE);
	CF_SetMUXReg(eCF_MUX_OUTPUT_DISABLE, eCF_MUX_CARDPWR_OFF, eCF_MUX_MODE_PCCARD);
#endif
//	EBI_CloseDMA(eMEM_DMC0);
#if (SMC_SRAM_DMA_TEST==1)
	EBI_CloseDMA(eMEM_SROMC);
#endif
#if (SMC_NAND_DMA_TEST==1)
	EBI_CloseDMA(eMEM_NAND);
#endif

}

//////////
// Function Name : SMC_TestATAResetBug
// Function Description : Memory Subsystem Test(EBI Test)
//						1. DMC0				
//						2. SROMC using SRAM	: nCS1
//						3. NAND Flash			: nCS2
// Input : 	None
// Output :	None 
// Version : v0.1
void SMC_TestATAResetBug(void)
{
	u32 uLoopCnt = 0;
	u32 uBlock = 0;
	u32 uPage = 0;
	u32 uTempAddr = 0;

	SYSC_CtrlEBIPrio(eEBI_FIX4);
#if (SMC_NAND_DMA_TEST==1)
	//-------- Nand Flash setting --------------
	for(uLoopCnt=0 ; uLoopCnt<NAND_DATA_SIZE ; uLoopCnt++)
		*(unsigned char *)(0x52000000+uLoopCnt) = eATA_XFR_CMD_START;
	for(uLoopCnt=0 ; uLoopCnt<NAND_SPARE_16 ; uLoopCnt++)
		aSpareBuffer[uLoopCnt] = 0xFF;

	uBlock = 50;
	uPage = 0;

	NANDT_Init();

	NAND_WritePageSLCSetup(0, uBlock, uPage);
	Nand_DmaTestDone = 0;
	EBI_SetDMAParams(eMEM_NAND); 
	// Set LLI parameters for DMA loop operation
	SMC_SetLLI(g_oaEBIInform[eMEM_NAND].uSrcAddr, g_oaEBIInform[eMEM_NAND].uDstAddr, 
				(DATA_SIZE)g_oaEBIInform[eMEM_NAND].eDataSz , (BURST_MODE)g_oaEBIInform[eMEM_NAND].eBurstMode, g_oaEBIInform[eMEM_NAND].uDataCnt);
#endif 

#if (SMC_SRAM_DMA_TEST==1)
	//-------- SROMC(SRAM) setting --------------
	EBI_SetDMAParams(eMEM_SROMC);

	uTempAddr = g_oaEBIInform[eMEM_SROMC].uDstAddr;
	for(uLoopCnt=0 ; uLoopCnt<SRAM_DATA_SIZE ; uLoopCnt++)
		*(volatile u32*)(uTempAddr+4) = 1;
       
#endif

#if (SMC_SRAM_DMA_TEST==1)
	DMACH_Start(&g_oaEBIInform[eMEM_SROMC].oEBIDma);	// SROMC DMA Start
#endif	
#if (SMC_NAND_DMA_TEST==1)
	DMACH_Start(&g_oaEBIInform[eMEM_NAND].oEBIDma);	// NAND DMA Start
#endif
#if (SMC_UDMA_TEST==1)
	while(1)
	{
		ATA_SetRst( 0, NORESET);		// ata device reset.	
		UART_Printf("*");
	}
#endif


}

void SMC_TestEBIUsingOneNAND(void)
{
	u32 uLoopCnt = 0;
	u32 uStartBlock = 0;
	u32 uStartPage = 0;

	SYSC_CtrlEBIPrio(eEBI_FIX4);

	//-------- CF Card Setting ----------------
	ATA_Init(ATA_CON0, DIRECT_MODE);
	ATA_OpenMedia(0, ATA_CON0, eATA_MODE_UDMA);
	
	Getc();	
#if 1
	//-------- OneNand setting --------------
	for(uLoopCnt=0; uLoopCnt<ONENAND_PAGESIZE/4; uLoopCnt++)
		aReadData[uLoopCnt] = 0;
	
	uStartBlock	=	0;
	uStartPage	=	1;

//	OneNANDT_Init();
//	g_uOneNANDSrcAddr = OneNandT_MemoryReadSetup(0,uStartBlock, uStartPage);

	UART_Printf("\n");
	UART_Printf("[%d block, %d page]", uStartBlock, uStartPage);
	for(uLoopCnt=0 ; uLoopCnt<ONENAND_PAGESIZE/4 ; uLoopCnt++)
	{
		if(!(uLoopCnt%4))
		{
			UART_Printf("\n");
			UART_Printf("0x%04x : ",uLoopCnt*4);
		}
		UART_Printf("0x%08x  ", aReadData[uLoopCnt]);
	}
	UART_Printf("\n");
#endif

//	EBI_SetDMAParams(eMEM_OneNAND);
//	DMACH_Start(&g_oaEBIInform[eMEM_OneNAND].oEBIDma);	// OneNAND DMA Start
#if 1
	//EBI_SetDMAParams(eMEM_DMC0);
	EBI_SetDMAParams(eMEM_SROMC);
	EBI_SetDMAParams(eMEM_OneNAND);

        // Enable DMAs
	//DMACH_Start(&g_oaEBIInform[eMEM_DMC0].oEBIDma);	// DMC0 	DMA Start
	DMACH_Start(&g_oaEBIInform[eMEM_SROMC].oEBIDma);	// SROMC DMA Start
	DMACH_Start(&g_oaEBIInform[eMEM_OneNAND].oEBIDma);	// OneNAND DMA Start
#endif
//	ATA_TestUDmaMode_Int_for_EBI();
	while(1)
	{
		if(Getc()=='x')
		{
			break;
		}
	}

	ATA_CloseMedia(ATA_CON0);
	ATA_SetEnable(ATA_CON0, DISABLE);
	CF_SetMUXReg(eCF_MUX_OUTPUT_DISABLE, eCF_MUX_CARDPWR_OFF, eCF_MUX_MODE_PCCARD);

//	NAND_ReadPageSLCClose(0, aSpareBuffer);
	//EBI_CloseDMA(eMEM_DMC0);
	EBI_CloseDMA(eMEM_SROMC);
	EBI_CloseDMA(eMEM_OneNAND);

}

void SMC_TestEBI(void)
{
	u32 uLoopCnt = 0;

	SYSC_CtrlEBIPrio(eEBI_FIX4);

	//-------- Nand Flash setting --------------
	for(uLoopCnt=0 ; uLoopCnt<NAND_DATA_SIZE ; uLoopCnt++)
		aBuffer[uLoopCnt] = eATA_XFR_CMD_ABORT;
	for(uLoopCnt=0 ; uLoopCnt<NAND_SPARE_16 ; uLoopCnt++)
		aSpareBuffer[uLoopCnt] = 0xFF;
#if 1
	//-------- CF Card Setting ----------------
	if (!ATA_Init(ATA_CON0, DIRECT_MODE))
	{
		UART_Printf("Fail to initialize ATA Mode...\n");
		return;
	}
	DelayfrTimer(milli, 100);
	ATA_IdentifyDevice(0, ATA_CON0);	
	DelayfrTimer(milli, 100);
	ATA_OpenMedia(0, ATA_CON0, eATA_MODE_UDMA);
#endif	

        // Enable DMAs
//	DMACH_Start(&g_oaEBIInform[eMEM_DMC0].oEBIDma);	// DMC0 	DMA Start
//	DMACH_Start(&g_oaEBIInform[eMEM_SROMC].oEBIDma);	// SROMC DMA Start
	DMACH_Start(&g_oaEBIInform[eMEM_NAND].oEBIDma);	// NAND DMA Start

	ATA_TestUDmaMode_Int_for_EBI();
	ATA_TestPDmaMode_Int_for_EBI(); 
	while(1)
	{
		if(Getc()=='x')
		{
			break;
		}
	}

	ATA_CloseMedia(ATA_CON0);
	ATA_SetEnable(ATA_CON0, DISABLE);
	CF_SetMUXReg(eCF_MUX_OUTPUT_DISABLE, eCF_MUX_CARDPWR_OFF, eCF_MUX_MODE_PCCARD);

//	NAND_ReadPageSLCClose(0, aSpareBuffer);
//	EBI_CloseDMA(eMEM_DMC0);
//	EBI_CloseDMA(eMEM_SROMC);
//	EBI_CloseDMA(eMEM_NAND);

}

//////////
// Function Name : SMC_SetLLI 
// Function Description : This function set the DMA LLI feature for NAND DMA operation
// Input : 	uSrcAddr - Source Address of Data
//			uDstAddr - Dest Address of Data
//			eTransferWidth - BYTE/HWORD/WORD
//			eBurst	- Burst mode
//			uDataCnts - data number which user inputs
// Output : 	None
// Version : v0.1
void SMC_SetLLI(u32 uSrcAddr, u32 uDstAddr, DATA_SIZE eTransferWidth, BURST_MODE eBurst, u32 uDataCnts)
{
	u32	*pLLI_Base = NULL;
	u32	uLoopCnt = 0; 
	u32	uOneTimeTxDataSize = 0;
	u32	uTotTxDataSize		= 0;
	u32	uMaxLoopCnt 		= 0;

	uOneTimeTxDataSize 	= eTransferWidth*DMAT_GetBurstSize(eBurst);
	uTotTxDataSize		= eTransferWidth*uDataCnts;
	uMaxLoopCnt 			= (uTotTxDataSize%uOneTimeTxDataSize == 0) ? uTotTxDataSize/uOneTimeTxDataSize : (uTotTxDataSize/uOneTimeTxDataSize + 1);

	pLLI_Base = (u32 *)g_uLLIBaseAddr;

	for(uLoopCnt=0 ; uLoopCnt<uMaxLoopCnt; uLoopCnt++)
	{
		*pLLI_Base++ = uSrcAddr + (uLoopCnt*uOneTimeTxDataSize);
		*pLLI_Base++ = uDstAddr;		// NAND Data Register
		*pLLI_Base++ = (g_uLLIBaseAddr + ((uLoopCnt+1)*0x20));
		*pLLI_Base++ = (eFIX<<27) | (eINCREMENT<<26) | (1<<25) | (0<<24) | ((eTransferWidth>>1)<<21)|((eTransferWidth>>1)<<18)|
						(eBurst<<15)|(eBurst<<12);
		*pLLI_Base++ = uOneTimeTxDataSize;
		pLLI_Base += 3;
	}

	*pLLI_Base++ = uSrcAddr + (uLoopCnt*uOneTimeTxDataSize);
	*pLLI_Base++ = uDstAddr;			// NAND Data Register
//	*pLLI_Base++ = 0;				// 	Finish LLI operation
	*pLLI_Base++ = g_uLLIBaseAddr;		// 	Loop DMA LLI operation
	*pLLI_Base++ = (1<<31) | (eFIX<<27) | (eINCREMENT<<26) | (1<<25) | (0<<24) | ((eTransferWidth>>1)<<21)|((eTransferWidth>>1)<<18)|(eBurst<<15)|(eBurst<<12);
	*pLLI_Base++ = uOneTimeTxDataSize;
}

void SMC_TestATABug(void)
{
	u32 uLoopCnt = 0;

	SYSC_CtrlEBIPrio(eEBI_FIX4);

#if (SMC_UDMA_TEST==1)
	//-------- CF Card Setting ----------------
	if (!ATA_Init(ATA_CON0, DIRECT_MODE))
	{
		UART_Printf("Fail to initialize ATA Mode...\n");
		return;
	}
	DelayfrTimer(milli, 100);
	ATA_IdentifyDevice(ATA_CON0);	
	DelayfrTimer(milli, 100);
	ATA_OpenMedia(ATA_CON0, eATA_MODE_UDMA);
#endif	

#if (SMC_NAND_DMA_TEST==1)
	//-------- Nand Flash setting --------------
	for(uLoopCnt=0 ; uLoopCnt<NAND_DATA_SIZE ; uLoopCnt++)
		*(unsigned char *)(0x52000000+uLoopCnt) = eATA_XFR_CMD_START;
	for(uLoopCnt=0 ; uLoopCnt<NAND_SPARE_16 ; uLoopCnt++)
		aSpareBuffer[uLoopCnt] = 0xFF;

//	NANDT_Init();
#endif	

#if (SMC_NAND_DMA_TEST==1)
//	NAND_WritePageSLCSetup(0, uBlock, uPage);
	Nand_DmaTestDone = 0;
	EBI_SetDMAParams(eMEM_NAND); 
	// Set LLI parameters for DMA loop operation
	SMC_SetLLI(g_oaEBIInform[eMEM_NAND].uSrcAddr, g_oaEBIInform[eMEM_NAND].uDstAddr, 
				(DATA_SIZE)g_oaEBIInform[eMEM_NAND].eDataSz , (BURST_MODE)g_oaEBIInform[eMEM_NAND].eBurstMode, g_oaEBIInform[eMEM_NAND].uDataCnt);

	DMACH_Start(&g_oaEBIInform[eMEM_NAND].oEBIDma);	// NAND DMA Start
#endif	

#if (SMC_UDMA_TEST==1)
	ATA_TestUDmaMode_Int_for_EBI();
#endif	

#if (SMC_NAND_DMA_TEST==1)
	while(1)
	{
		if(Getc()=='x')
		{
//			EBI_CloseDMA(eMEM_NAND);
			UART_Printf("Nand_DmaTestDone Count : %d\n", Nand_DmaTestDone);
			break;
		}
	}	
	while(!Nand_DmaTestDone);	
	
//	NAND_WritePageSLCClose(0, aSpareBuffer);
	EBI_CloseDMA(eMEM_NAND);
#endif	

#if (SMC_UDMA_TEST==1)
	ATA_CloseMedia(ATA_CON0);
	ATA_SetEnable(ATA_CON0, DISABLE);
	CF_SetMUXReg(eCF_MUX_OUTPUT_DISABLE, eCF_MUX_CARDPWR_OFF, eCF_MUX_MODE_PCCARD);
#endif

}
///////////////////////////////////////////////////////////////////////////////////
////////////////////                    SMC Main Test                  /////////////////////////// 
///////////////////////////////////////////////////////////////////////////////////
extern void Test_LAN9115(void);
const testFuncMenu smc_menu[] =
{
		SROMC_Test,					"SROMC Test",
		SROMC_ByteAccess,			"SROMC Byte Access",
		SROMC_HalfWordAccess,		"SROMC Half-Word Access",
		SROMC_AgingTest,			"SROMC Aging Test",
		SROMCT_System_PowerMode, "Power Mode",
		//SROMC_MultiAccessTestExample, "EBI Multi Test",
		
		CS8900_Test,				"CS8900 Test",
		Test_LAN9115, 				"LAN9115 Test",
		
//		SMC_TestEBIUsingNAND,		"Test Memory Subsystem(EBI) Using NAND",
//		SMC_TestEBIUsingOneNAND, 	"Test Memory Subsystem(EBI) Using OneNAND",
//		SMC_TestATABug,			"Test ATA Bug",
//		SMC_TestATAResetBug,		"Test ATA Reset Bug Test Using NAND",
		0,0
};

void SMC_Test(void)
{
	u32 i;
	s32 uSel;

	UART_Printf("[SROMC_Test]\n\n");
	
	SROMC_Init();
	SYSC_CtrlHCLKGate( eHCLK_DMC0	, 0);
	DelayfrTimer(milli, 10);
	SYSC_CtrlHCLKGate( eHCLK_DMC0	, 1);
	DelayfrTimer(milli, 10);
	
	while(1)
	{
		for (i=0; (u32)(smc_menu[i].desc)!=0; i++)
			UART_Printf("%2d: %s\n", i, smc_menu[i].desc);

		UART_Printf("\nSelect the function to test : ");
		uSel =UART_GetIntNum();
		UART_Printf("\n");
		if(uSel == -1) 
			break;
		if (uSel>=0 && uSel<(sizeof(smc_menu)/8-1))
			(smc_menu[uSel].func) ();
	}
}	


