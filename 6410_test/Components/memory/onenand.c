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
*	File Name : onenand.c
*  
*	File Description : This file implements the API functons for OneNand controller.
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

#include "def.h"
#include "option.h"
#include "library.h"
#include "sfr6410.h"
#include "system.h"
#include "sysc.h"
#include "intc.h"
#include "onenand.h"
#include "gpio.h"
#include "dma.h"
#include "timer.h"

#define ONENAND0_MEM_BASE		(0x20000000)
#define ONENAND1_MEM_BASE		(0x28000000)

#define ONENAND1_OFFSET 	(0x80000)
#define ONENAND(__n) 		( ( volatile oONENAND_REGS * ) ( ONENAND_pBase[__n] ) )

#if(OND_DEVICE == OND_6410)
#define BUFFER				(0<<24)
#define ARRAY_RW            		(1<<24)
#define COMMANDS            	(2<<24)
#define DIRECT_ACCESS       	(3<<24)
#else
#define BUFFER				(0<<22)
#define ARRAY_RW            		(1<<22)
#define COMMANDS            	(2<<22)
#define DIRECT_ACCESS       	(3<<22)
#endif

//For OneNand Direct Access
//for 1Gb Single(1024Block) or 2Gb Dual Die(1024*2Block)
#define	DBS(b)		((b<<5)&0x8000)		// BufferRAM of DDP (Device BufferRAM Select)
#define	DFS(b)		((b<<5)&0x8000)		// flash core of DDP (Device Flash Core Select)
#define	FBA(b)		(b&0x03ff)			// NAND Flash Block Address	

//for 2Gb Single(2048Block) or 4Gb Dual Die(2048*2Block)
#define 	DBS2(b)		((b<<4)&0x8000)	
#define	DFS2(b)		((b<<4)&0x8000)		// flash core of DDP (Device Flash Core Select)
#define	FBA2(b)		(b&0x07ff)	

#define	FPA(p)		((p<<2)&0xfc)		// NAND Flash Page Address
#define	FSA(s)		(s&0x3)				// NAND Flash Sector Address
#define	BSA(r)		((r<<8)&0x0f00)		// BufferRAM Sector Address
#define	BSC(n)		(n&0x3)				// BufferRAM Sector Count
#define	SBA(b)		(b&0x03ff)			// Start Block Address

#define OND_DATARAM0				(0x200*4)

//Command Register - CMD 
#define OND_LD_MS_TO_BUF	0x0000
#define OND_LD_SS_TO_BUF	0x0013
#define OND_PG_MS_FR_BUF	0x0080
#define OND_PG_SS_FR_BUF	0x001a
#define OND_CP_BACK_PRG		0x001b
#define OND_UNLOCK			0x0023
#define OND_ALLUNLOCK			0x0027
#define OND_LOCK			0x002a
#define OND_LOCK_TIGHT		0x002c
#define OND_ERASE_VERIFY	0x0071
#define OND_BLK_ERASE		0x0094
#define OND_MULTY_ERASE		0x0095
#define OND_ERASE_SUS		0x00b0
#define OND_ERASE_RESUME	0x0030
#define OND_RST_CORE		0x00f0
#define OND_RST_OND			0x00f3
#define OND_OTP_ACS			0x0065
#define OND_CACHE_READ		0x000e
#define OND_FINISH_CACHE_READ		0x000c
	
typedef struct tag_ONENAND_REGS
{
	u32 rMemCfg;
	u32 reserved1[3];
	u32 rBurstLength;
	u32 reserved2[3];
	u32 rMemReset;
	u32 reserved3[3];
	u32 rIntErrStat;
	u32 reserved4[3];
	u32 rIntErrMask;
	u32 reserved5[3];
	u32 rIntErrAck;
	u32 reserved6[3];
	u32 rEccErrStat;
	u32 reserved7[3];
	u32 rManufactID;
	u32 reserved8[3];
	u32 rDeviceID;
	u32 reserved9[3];
	u32 rDataBufSize;
	u32 reserved10[3];
	u32 rBootBufSize;
	u32 reserved11[3];
	u32 rBufAmount;
	u32 reserved12[3];
	u32 rTech;
	u32 reserved13[3];
	u32 rFbaWidth;
	u32 reserved14[3];
	u32 rFpaWidth;
	u32 reserved15[3];
	u32 rFsaWidth;
	u32 reserved16[3];
	u32 rRevision;
	u32 reserved17[3];
	u32 rDataRam0;
	u32 reserved18[3];
	u32 rDataRam1;
	u32 reserved19[3];
	u32 rSyncMode;
	u32 reserved20[3];
	u32 rTransSpare;
	u32 reserved21[3];
	u32 rLockBit;
	u32 reserved22[3];
	u32 rDbsDfsWidth;
	u32 reserved23[3];
	u32 rPageCnt;
	u32 reserved24[3];
	u32 rErrPageAddr;
	u32 reserved25[3];
	u32 rBurstRdLat;
	u32 reserved26[3];
	u32 rIntPinEnable;
	u32 reserved27[3];
	u32 rIntMonCyc;
	u32 reserved28[3];
	u32 rAccClock;
	u32 reserved29[3];
	u32 rSlowRdPath;
	u32 reserved30[3];
	u32 rErrBlkAddr;
	u32 reserved31[3];	
	u32 rFlashVerID;			//Base + 0x1F0
#if(OND_DEVICE == OND_6410)	
	u32 reserved32[67];
	u32 rFlashAuxCntrl;		//Base + 0x300
	u32 reserved33[3];
	u32 rFlashAfifoCnt;		//Base + 0x310
#endif	
} 
oONENAND_REGS;

static volatile u32	OneNAND_BUFFER_BASE[ONENAND_CONNUM];
static volatile u32	OneNAND_ARRAY_BASE[ONENAND_CONNUM];
static volatile u32	OneNAND_CMD_BASE[ONENAND_CONNUM];
static volatile u32	OneNAND_DIRECT_BASE[ONENAND_CONNUM];

static void *ONENAND_pBase[ONENAND_CONNUM];
volatile ONENAND_oInform	OneNand_Inform[ONENAND_CONNUM];

static u32 FbaShift[ONENAND_CONNUM], FpaShift[ONENAND_CONNUM];//, FsaShift[ONENAND_CONNUM];//, DfsDbs[ONENAND_CONNUM];
static u32 FbaMask[ONENAND_CONNUM], FpaMask[ONENAND_CONNUM];//, FsaMask[ONENAND_CONNUM];//, DfsMask[ONENAND_CONNUM];
static OneNand_sCallBack OneNand_ISRCallBack;
OneNandT_oInterruptFlag 	OneNandT_oIntFlag; 
volatile u32 OneNand_DmaDone;
volatile u32 MemToMem_DmaDone=1;
volatile u32 MemToMem_DmaDone1=1;

DMAC g_oONDDmac0;

DMA_UNIT OND_Dma_Controller = DMA1;
DMA_CH OND_Dma_Channel = DMA_A;

//u32 aOneNandBadBlock[ONDT_NUMOFBAD_BUFFER];

#if (OND_TRANS_MODE == OND_LDM_INST)
extern void OneNand4burstPageRead(u32 uBaseAddr, u32 uSrcAddr, u32 uDstAddr);
extern void OneNand4burstSpareRead(u32 uBaseAddr, u32 uSrcAddr, u32 uDstAddr);

extern void OneNand4burstPageWrite(u32 uBaseAddr, u32 uSrcAddr, u32 uDstAddr);
extern void OneNand4burstSpareWrite(u32 uBaseAddr, u32 uSrcAddr, u32 uDstAddr);

extern void OneNand8burstPageRead(u32 uBaseAddr, u32 uSrcAddr, u32 uDstAddr);
extern void OneNand8burstSpareRead(u32 uBaseAddr, u32 uSrcAddr, u32 uDstAddr);

extern void OneNand8burstPageWrite(u32 uBaseAddr, u32 uSrcAddr, u32 uDstAddr);
extern void OneNand8burstSpareWrite(u32 uBaseAddr, u32 uSrcAddr, u32 uDstAddr);

extern void OneNand8burstPageReadDir(u32 uBaseAddr, u32 uSrcAddr, u32 uDstAddr);
extern void OneNand8burstPageWriteDir(u32 uBaseAddr, u32 uSrcAddr, u32 uDstAddr);

extern void OneNand4burstSpareReadTemp(u32 uBaseAddr, u32 uSrcAddr, u32 uDstAddr);
extern void OneNand4burstPageRead_test(u32 uBaseAddr, u32 uSrcAddr, u32 uDstAddr);
#endif


//////////
// Function Name : ONENAND_Init
// Function Description : This function initializes a certain OneNand Controller
// Input : 	Controller - OneNand Controller Port Number 
// Output : 	TRUE - Memory Device is reset
//			FALSE - Memory Device is not reset because of ERROR
// Version : v0.1
	
extern void __irq ISR_DMAPlayDone(void);

u8	ONENAND_Init(u32 Controller)
{
	u32 uBaseAddress;

	if(Controller == 0)
	{
		uBaseAddress = ONENAND0_BASE;
	}
	else if(Controller == 1)
	{
		uBaseAddress = ONENAND1_BASE;
	}
	else
	{
		return FALSE;
	}
	
	ONENAND_pBase[Controller] = (void *)uBaseAddress;

#if(OND_DEVICE == OND_6410)
	ONENAND_SetWatchDog_Reset(Controller, OND_WD_DISABLE);
#endif
	ONENAND_SetAccClock(Controller);

	ONENAND_GetDevInformation(Controller);
	ONENAND_SetMemSpace(Controller);

	//Pending all clear
	Outp32(&ONENAND(Controller)->rIntErrAck, (u32)(0x3FFF));
	
#if (ONENAND_INTMODE == ONENAND_INTERRUPT)
#if (ONENAND_ACCESS_MODE == ONENAND_COMMAND)
	ONENAND_EnableAllInterrupt(Controller);
	ONENAND_DisableInterrupt(Controller,  eOND_RDYACT);
#else
	ONENAND_EnableAllInterrupt(Controller);
	ONENAND_DisableInterrupt(Controller,  eOND_RDYACT);
	ONENAND_DisableInterrupt(Controller,  eOND_ERSCMP);	
	ONENAND_DisableInterrupt(Controller,  eOND_BLKRWCMP);	
	ONENAND_DisableInterrupt(Controller,  eOND_PGMCMP);	
#endif
#else
	ONENAND_EnableInterrupt(Controller,  eOND_INTTO);
#endif

	ONENAND_EnableECCCorrection(Controller, ONENAND_WITH_CORRECT);
	ONENAND_EnableIOBE(Controller, 0);
	ONENAND_SetBurstLatency(Controller, eOND_LATENCY4);
	ONENAND_SetSyncMode(Controller, eOND_SYNC_BURST16);

	//ONENAND_EnableAllInterrupt(Controller);
	//ONENAND_DisableInterrupt(Controller,  eOND_RDYACT);

#if (OND_TRANS_MODE == OND_DMA)
	{
		u32 uNum_Dma;
		
		if(OND_Dma_Controller == DMA0)
			uNum_Dma = NUM_DMA0;
		else if(OND_Dma_Controller == DMA1)
			uNum_Dma = NUM_DMA1;
		else if(OND_Dma_Controller == SDMA0)
			uNum_Dma = NUM_SDMA0;
		else if(OND_Dma_Controller == SDMA1)
			uNum_Dma = NUM_SDMA1;
		else
			uNum_Dma = NUM_DMA0;
		
		DMAC_InitCh(OND_Dma_Controller, DMA_ALL, &g_oONDDmac0);
		INTC_SetVectAddr(uNum_Dma,  ONENAND_DmaISR);
		INTC_Enable(uNum_Dma);
	}
#endif

	return TRUE;
}


//////////
// Function Name : ONENAND_SetLLI
// Function Description : This function set the DMA LLI feature
// Input : 	None
// Output : 	None
// Version : v0.1
void ONENAND_SetLLI(u32 uSrcAddr, u32 uDstAddr)
{
	u32 *pLLI_Base;
	u32 i;

	pLLI_Base = (u32 *)ONENAND_LLI_BASE;

	for(i=0 ; i<127; i++)
	{
		*pLLI_Base++ = uSrcAddr;
		*pLLI_Base++ = uDstAddr + (i*0x10);
		*pLLI_Base++ = (ONENAND_LLI_BASE + ((i+1)*0x20));
		*pLLI_Base++ = 0x0C489000;
		*pLLI_Base++ = 0x4;
		pLLI_Base += 3;
	}

	*pLLI_Base++ = uSrcAddr;
	*pLLI_Base++ = uDstAddr + (i*0x10);
	*pLLI_Base++ = 0;
	*pLLI_Base++ = 0x8C489000;
	*pLLI_Base++ = 0x4;
}



void __irq ONENAND_DmaISR(void)
{
  	DMACH_ClearIntPending(&g_oONDDmac0);

//	UART_Printf ("DMA ISR %d\n", OneNand_DmaDone);
	OneNand_DmaDone = 1;
	INTC_ClearVectAddr();
}


//////////
// Function Name : ONENAND_SetWatchDog_Reset
// Function Description : Set the OneNand Watchdog reset to enable or disable
// Input : 	Controller - OneNand Controller Port Number 
//			uWatchDog - 	OND_WD_DISABLE		(0x01)
//						OND_WD_ENABLE		(0x00)
#if(OND_DEVICE == OND_6410)	
void ONENAND_SetWatchDog_Reset(u32 Controller, u32 uWatchDog)
{
	Outp32(&ONENAND(Controller)->rFlashAuxCntrl, uWatchDog);
}
#endif

//////////
// Function Name : ONENAND_Reset
// Function Description : Reset the OneNand Device 
// Input : 	Controller - OneNand Controller Port Number 
//			Reset - 	ONENAND_WARMRESET		(0x01)
//					ONENAND_CORERESET		(0x02)
//					ONENAND_HOTRESET		(0x03)
// Version : v0.1
void ONENAND_Reset(u32 Controller, u32 uReset)
{
	u32 uMemCfg_Value, uConfig;
	
	OneNandT_oIntFlag.IntActInt = 0;
	Outp32(&ONENAND(Controller)->rMemReset, uReset);
#if (ONENAND_INTMODE == ONENAND_INTERRUPT)	
	while(!OneNandT_oIntFlag.IntActInt);
#else
	while( !(Inp32(&ONENAND(Controller)->rIntErrStat) & eOND_INTACT) );
	Outp32(&ONENAND(Controller)->rIntErrAck, eOND_INTACT);
#endif
	
	//Wait Reset Complete
	while(Inp32(&ONENAND(Controller)->rMemReset));

	//2007.07.05....Warm/Hot Reset Operation reset the registers of device
	if(uReset != ONENAND_CORERESET)
	{
		uMemCfg_Value = Inp32(&ONENAND(Controller)->rMemCfg);
		Outp32(&ONENAND(Controller)->rMemCfg, 0x40e0);
		ONENAND_DirectRead(Controller, OND_SYSCONFIG, &uConfig);
		
		Outp32(&ONENAND(Controller)->rMemCfg, uMemCfg_Value);
		ONENAND_DirectRead(Controller, OND_SYSCONFIG, &uConfig);
	}

	//for test
	if(uReset == ONENAND_WARMRESET)
	{
		if(OneNand_Inform[Controller].uDbsDfs == 0)
			ONENAND_UnlockBlock(Controller, 0, OneNand_Inform[Controller].uNumOfBlock-1);
		else
		{
			ONENAND_UnlockBlock(Controller, 0, (OneNand_Inform[Controller].uNumOfBlock/2)-1);
			ONENAND_UnlockBlock(Controller, OneNand_Inform[Controller].uNumOfBlock/2, OneNand_Inform[Controller].uNumOfBlock-1);
		}	
	}
}

//////////
// Function Name : ONENAND_SetISRHandler
// Function Description : Register Interrupt Handler 
// Input : 	Ond_Callback - Array with the Interrupt Handler function address
// Version : v0.1
void ONENAND_SetISRHandler(void *Ond_Callback)
{
	OneNand_ISRCallBack.ONENAND_ISR_Func = (ONENAND_ISR_Routine)Ond_Callback;
}

//////////
// Function Name : ONENAND_SetBurstLatency
// Function Description : Sets the burst read latency in cycles 
// Input : 	Controller - OneNand Controller Port Number 
//			eLatency - (OneNAND_eLATENCY)latency cycle
// Version : v0.1
void ONENAND_SetBurstLatency(u32 Controller, OneNAND_eLATENCY eLatency)
{
	u32 uBurstReadLatency, uConfig;

	uBurstReadLatency = Inp32(&ONENAND(Controller)->rMemCfg);
	uBurstReadLatency &= ~(0x7<<12);

	uBurstReadLatency |= (eLatency<<12);
	
	Outp32(&ONENAND(Controller)->rMemCfg, uBurstReadLatency);

	//2007.07.23... rb1004 : MemCfg write check
	ONENAND_DirectRead(Controller, OND_SYSCONFIG, &uConfig);

}


//////////
// Function Name : ONENAND_SetFlashClock
// Function Description : Sets the OneNand Internal Flash Clock
// Input : 	Controller - OneNand Controller Port Number 
// Version : v0.1
void ONENAND_SetFlashClock(u32 Controller, OneNAND_eFlashClock eFlashClock)
{
	u32 uClkDivider;
	
	uClkDivider = SYSC_GetDIV0();
	uClkDivider = (uClkDivider & ~(3<<16)) | (eFlashClock<<16);

	SYSC_SetDIV0_all(uClkDivider);
}


//for test
void ONENAND_SetMemCfgTest(u32 Controller, u32 uData1, u32 uData2)
{
	//u32 uConfig;
	
	Outp32(&ONENAND(Controller)->rMemCfg, uData1);
//	ONENAND_DirectRead(Controller, OND_SYSCONFIG, &uConfig);
//	if((uConfig&0xFFFF) != uData1)
//		UART_Getc();
	
	Outp32(&ONENAND(Controller)->rMemCfg, uData2);	
//	ONENAND_DirectRead(Controller, OND_SYSCONFIG, &uConfig);
//	if((uConfig&0xFFFF) != uData2)
//		UART_Getc();	
}



//////////
// Function Name : ONENAND_SetAccClock
// Function Description : Sets the number of cycles required to cover the access time of the Flash memory device
//					 ACCESS_CLK = 35ns/Clock period + 1
// Input : 	Controller - OneNand Controller Port Number 
// Version : v0.1
void ONENAND_SetAccClock(u32 Controller)
{
	u32 uClkDivider, uAccClock;

	uClkDivider = SYSC_GetDIV0();
	uClkDivider = (uClkDivider & (3<<16))>>16;
	
	OneNand_Inform[Controller].uFlashClock = g_HCLK;

	uAccClock = (35*(OneNand_Inform[Controller].uFlashClock/2/1000))/1000000 + 1;
	//uAccClock = 1;
	Outp32(&ONENAND(Controller)->rAccClock, uAccClock&0x7);
}



//////////
// Function Name : ONENAND_SyncMode
// Function Description : Set the OneNand Transfer Mode include burst length 
// Input : 	Controller - OneNand Controller Port Number 
//			uMode - 	Read&Write Transfer Mode
// Version : v0.1
void ONENAND_SetSyncMode(u32 Controller, OneNAND_eMODE eMode)
{
	u32 uSyncMode;
	u32 uConfig;

	uSyncMode = Inp32(&ONENAND(Controller)->rMemCfg);
	uSyncMode &= ~((1<<15)|(0x7<<9)|(1<<1));
	
	switch (eMode)
	{
		case eOND_SYNC_CONT:
			uSyncMode|= (0x1<<15)|(0x0<<9)|(0x1<<1);
			Outp32(&ONENAND(Controller)->rBurstLength, 16);
			//Outp32(&ONENAND(Controller)->rBurstLength, 0);
			Outp32(&ONENAND(Controller)->rMemCfg, uSyncMode);		//sync, burst:incr
			break;

		case eOND_SYNC_BURST4:
			uSyncMode|= (0x1<<15)|(0x1<<9)|(0x1<<1);
			Outp32(&ONENAND(Controller)->rBurstLength, 4);
			Outp32(&ONENAND(Controller)->rMemCfg, uSyncMode);		//sync, burst:4
			break;

		case eOND_SYNC_BURST8:
			uSyncMode|= (0x1<<15)|(0x2<<9)|(0x1<<1);
			Outp32(&ONENAND(Controller)->rBurstLength, 8);
			Outp32(&ONENAND(Controller)->rMemCfg, uSyncMode);		//sync, burst:8
			break;

		case eOND_SYNC_BURST16:
			uSyncMode|= (0x1<<15)|(0x3<<9)|(0x1<<1); //|(1<<4);
			//uSyncMode|= (0x1<<15)|(0x3<<9)|(0x0<<1); //|(1<<4);
			Outp32(&ONENAND(Controller)->rBurstLength, 16);
			Outp32(&ONENAND(Controller)->rMemCfg, uSyncMode);		//sync, burst:16
			break;

		case eOND_SYNC_BURST32:
			uSyncMode|= (0x1<<15)|(0x4<<9)|(0x1<<1);
			Outp32(&ONENAND(Controller)->rBurstLength, 16);
			Outp32(&ONENAND(Controller)->rMemCfg, uSyncMode);		//sync, burst:16
			break;

		case eOND_ASYNC:
		default:
			uSyncMode|= (0x0<<15)|(0x0<<9)|(0x0<<1);
			Outp32(&ONENAND(Controller)->rBurstLength, 0);
			Outp32(&ONENAND(Controller)->rMemCfg, uSyncMode);		//rm async, burst:4
			break;
	}

	//DelayfrTimer(milli, 100);
	ONENAND_DirectRead(Controller, OND_SYSCONFIG, &uConfig);
	//UART_Printf("OND_SYSCONFIG : 0x%04x\n", uConfig);
}


//////////
// Function Name : ONENAND_SyncMode
// Function Description : Set the OneNand Transfer Mode include burst length 
// Input : 	Controller - OneNand Controller Port Number 
//			uMode - 	Read&Write Transfer Mode
// Version : v0.1
void ONENAND_SetSyncMode_1(u32 Controller, u32 uReadMode, u32 uWriteMode, u32 uBurst)
{
	u32 uSyncMode;

	uSyncMode = Inp32(&ONENAND(Controller)->rMemCfg);
	uSyncMode &= ~((1<<15)|(0x7<<9)|(1<<1));
	uSyncMode |= ((uReadMode<<15)|(uBurst<<9)|(uWriteMode<<1));

	if(uBurst == BURST4)
		Outp32(&ONENAND(Controller)->rBurstLength, 4);
	else if(uBurst == BURST8)
		Outp32(&ONENAND(Controller)->rBurstLength, 8);
	else		
		Outp32(&ONENAND(Controller)->rBurstLength, 16);

	Outp32(&ONENAND(Controller)->rMemCfg, uSyncMode);
	
	////////////
	// A-die Bug at 50MHz(FlashCLK)
//	do
//	{
//		Outp32(&ONENAND(Controller)->rMemCfg, uSyncMode);
//		ONENAND_DirectRead(Controller, OND_SYSCONFIG, &uConfig);
//	}while((uConfig&0xFFFF) != uSyncMode);
}



//////////
// Function Name : ONENAND_EnableIOBE
// Function Description : Set the I/O Buffer enable for INT and RDY signals
// Input : 	Controller - OneNand Controller Port Number 
//			uEnable - IOBE Enable/Disable
// Version : v0.1
void ONENAND_EnableIOBE(u32 Controller, u32 uEnable)
{
	u32 uEnableIOBE, uConfig;

	uEnableIOBE = Inp32(&ONENAND(Controller)->rMemCfg);

#if 0	
	uEnableIOBE &= ~(0x7<<5);
	uEnableIOBE |= (1<<7)|(1<<6)|(uEnable<<5);
	
	Outp32(&ONENAND(Controller)->rMemCfg, uEnableIOBE);
#else
	//rb1004 OneNand Sync write ......070425...EVT0
	if(!(uEnableIOBE & (1<<5)))
	{
		uEnableIOBE &= ~(0x7<<5);
		uEnableIOBE |= (1<<7)|(1<<6)|(uEnable<<5);
		
		Outp32(&ONENAND(Controller)->rMemCfg, uEnableIOBE);
		
		//2007.07.23... rb1004 : MemCfg write check
		ONENAND_DirectRead(Controller, OND_SYSCONFIG, &uConfig);
	}
#endif	
}


//////////
// Function Name : ONENAND_EnableECCCorrection
// Function Description : Set the ECC Error Correction Operation
// Input : 	Controller - OneNand Controller Port Number 
//			uEnable -ONENAND_WITH_CORRECT :  ECC Error Correction enable
//					ONENAND_WITHOUT_CORRECT : ECC Error Correction disable
// Version : v0.1
void ONENAND_EnableECCCorrection(u32 Controller, u32 uEnable)
{
	u32 uEnableECC;
	u32 uConfig;

	uEnableECC = Inp32(&ONENAND(Controller)->rMemCfg);

#if 0
	if(uEnable == ONENAND_WITH_CORRECT)
		uEnableECC &= ~(0x1<<8);
	else if(uEnable == ONENAND_WITHOUT_CORRECT)
		uEnableECC |= (0x1<<8);
	
	Outp32(&ONENAND(Controller)->rMemCfg, uEnableECC);
#else
	//rb1004 OneNand Sync write ......070425...EVT0
	if(uEnable == ONENAND_WITH_CORRECT)
	{
		if((uEnableECC&(1<<8)))
		{
			uEnableECC &= ~(0x1<<8);
			Outp32(&ONENAND(Controller)->rMemCfg, uEnableECC);	
			
			//2007.07.23... rb1004 : MemCfg write check
			ONENAND_DirectRead(Controller, OND_SYSCONFIG, &uConfig);
			//UART_Printf("uConfig : 0x%08x\n", uConfig);
		}
	}
	else if(uEnable == ONENAND_WITHOUT_CORRECT)
	{
		if(!(uEnableECC&(1<<8)))
		{
			uEnableECC |= (0x1<<8);
			Outp32(&ONENAND(Controller)->rMemCfg, uEnableECC);	
			
			//2007.07.23... rb1004 : MemCfg write check
			ONENAND_DirectRead(Controller, OND_SYSCONFIG, &uConfig);
			//UART_Printf("uConfig : 0x%08x\n", uConfig);			
		}
	}

#endif
}

//////////
// Function Name : ONENAND_EnableSpareTransfer
// Function Description : If enabled, The main data area for the page will be transferred first and then the spare area
// Input : 	Controller - OneNand Controller Port Number 
//			bEnable - IOBE Enable/Disable
// Version : v0.1
void ONENAND_EnableSpareTransfer(u32 Controller, u8 bEnable)
{
	Outp32(&ONENAND(Controller)->rTransSpare, (u32)bEnable);
}


//////////
// Function Name : ONENAND_GetDevInformation
// Function Description : Get the OneNand Device Infomation
// Input : 	Controller - OneNand Controller Port Number 
// Version : v0.1
void ONENAND_GetDevInformation(u32 Controller)
{
	OneNand_Inform[Controller].uManufID = Inp32(&ONENAND(Controller)->rManufactID);
	OneNand_Inform[Controller].uDeviceID = Inp32(&ONENAND(Controller)->rDeviceID);
	OneNand_Inform[Controller].uDataBufSize = Inp32(&ONENAND(Controller)->rDataBufSize);
	OneNand_Inform[Controller].uBootBufSize = Inp32(&ONENAND(Controller)->rBootBufSize);
	OneNand_Inform[Controller].uAmountOfBuf = Inp32(&ONENAND(Controller)->rBufAmount);
	OneNand_Inform[Controller].uTechnology = Inp32(&ONENAND(Controller)->rTech);
	OneNand_Inform[Controller].uFlashVerID = Inp32(&ONENAND(Controller)->rFlashVerID);
}

//////////
// Function Name : ONENAND_GetMemCfg
// Function Description : Get the OneNand MEM_CFG register value
// Input : 	Controller - OneNand Controller Port Number 
// Version : v0.1
u32 ONENAND_GetMemCfg(u32 Controller)
{
	return (Inp32(&ONENAND(Controller)->rMemCfg));
}


//////////
// Function Name : ONENAND_GetBurstLength
// Function Description : Get the OneNand BURST_LEN register value
// Input : 	Controller - OneNand Controller Port Number 
// Version : v0.1
u32 ONENAND_GetBurstLength(u32 Controller)
{
	return (Inp32(&ONENAND(Controller)->rBurstLength));
}



//////////
// Function Name : ONENAND_GetInterruptStatus
// Function Description : Get the OneNand Interrupt Error Status Register Infomation
// Input : 	Controller - OneNand Controller Port Number 
// Version : v0.1
u32 ONENAND_GetInterruptStatus(u32 uController)
{
	u32 uIntStatus;
	
	uIntStatus = Inp32(&ONENAND(uController)->rIntErrStat) & 0x3FFF;

	return uIntStatus;
}

//////////
// Function Name : ONENAND_GetInterruptMask
// Function Description : Get the OneNand Interrupt Error Mask Register Infomation
// Input : 	Controller - OneNand Controller Port Number 
// Version : v0.1
u32 ONENAND_GetInterruptMask(u32 uController)
{
	u32 uIntMask;
	
	uIntMask = Inp32(&ONENAND(uController)->rIntErrMask) & 0x3FFF;

	return uIntMask;
}

//////////
// Function Name : ONENAND_GetEccErrStatus
// Function Description : Get the OneNand ECC Error Status
// Input : 	Controller - OneNand Controller Port Number 
// Version : v0.1
u32 ONENAND_GetEccErrStatus(u32 uController)
{
	u32 uEccErr;
	
	uEccErr = Inp32(&ONENAND(uController)->rEccErrStat) & 0xFFFF;

	return uEccErr;
}

//////////
// Function Name : ONENAND_SetMemSpace
// Function Description : Set the OneNand Device Infomation to Controller Register & Set variable about the MEM_ADDR field Information
// Input : 	Controller - OneNand Controller Port Number 
// Version : v0.1
void ONENAND_SetMemSpace(u32 Controller)
{
	u32 uDeviceDensity, uDeviceDDP;

	uDeviceDensity = (Inp32(&ONENAND(Controller)->rDeviceID) & 0xF0)>>4;
	uDeviceDDP = (Inp32(&ONENAND(Controller)->rDeviceID) & 0x8)>>3;

	switch(uDeviceDensity)
	{
		case 0 :	Outp32(&ONENAND(Controller)->rFbaWidth, 8);
				Outp32(&ONENAND(Controller)->rFpaWidth, 6);
				Outp32(&ONENAND(Controller)->rFsaWidth, 1);
				Outp32(&ONENAND(Controller)->rDbsDfsWidth, 0);
#if(OND_DEVICE == OND_6410)				
				FbaShift[Controller] = 12;
				FpaShift[Controller] = 6;
#else
				FbaShift[Controller] = 9;
				FpaShift[Controller] = 3;
#endif
				FbaMask[Controller] = 0xFF;
				FpaMask[Controller] = 0x3F;
				OneNand_Inform[Controller].uNumOfBlock = 256;
				OneNand_Inform[Controller].uCacheReadSupport = 0;
				break;
		case 1 :	Outp32(&ONENAND(Controller)->rFbaWidth, 9);
				Outp32(&ONENAND(Controller)->rFpaWidth, 6);
				Outp32(&ONENAND(Controller)->rFsaWidth, 1);
				Outp32(&ONENAND(Controller)->rDbsDfsWidth, 0);
#if(OND_DEVICE == OND_6410)				
				FbaShift[Controller] = 12;
				FpaShift[Controller] = 6;
#else
				FbaShift[Controller] = 9;
				FpaShift[Controller] = 3;
#endif
				FbaMask[Controller] = 0x1FF;
				FpaMask[Controller] = 0x3F;
				OneNand_Inform[Controller].uNumOfBlock = 512;
				OneNand_Inform[Controller].uCacheReadSupport = 0;
				break;
		case 2 :	Outp32(&ONENAND(Controller)->rFbaWidth, 9);
				Outp32(&ONENAND(Controller)->rFpaWidth, 6);
				Outp32(&ONENAND(Controller)->rFsaWidth, 2);
				Outp32(&ONENAND(Controller)->rDbsDfsWidth, 0);
#if(OND_DEVICE == OND_6410)				
				FbaShift[Controller] = 12;
				FpaShift[Controller] = 6;
#else				
				FbaShift[Controller] = 10;
				FpaShift[Controller] = 4;
#endif				
				FbaMask[Controller] = 0x1FF;
				FpaMask[Controller] = 0x3F;
				OneNand_Inform[Controller].uNumOfBlock = 512;
				OneNand_Inform[Controller].uCacheReadSupport = 0;
				break;
		case 3 :	Outp32(&ONENAND(Controller)->rFpaWidth, 6);
				Outp32(&ONENAND(Controller)->rFsaWidth, 2);
				if(uDeviceDDP)
				{
					Outp32(&ONENAND(Controller)->rFbaWidth, 9);
					Outp32(&ONENAND(Controller)->rDbsDfsWidth, 1);
					//added by rb1004....2007.04.23
					OneNand_Inform[Controller].uDbsDfs = 1;
					OneNand_Inform[Controller].uCacheReadSupport = 0;
				}
				else
				{
					Outp32(&ONENAND(Controller)->rFbaWidth, 10);
					Outp32(&ONENAND(Controller)->rDbsDfsWidth, 0);
					//added by rb1004....2007.04.23
					OneNand_Inform[Controller].uDbsDfs = 0;
					OneNand_Inform[Controller].uCacheReadSupport = 0;
				}
#if(OND_DEVICE == OND_6410)				
				FbaShift[Controller] = 12;
				FpaShift[Controller] = 6;
#else				
				FbaShift[Controller] = 10;
				FpaShift[Controller] = 4;
#endif				
				FbaMask[Controller] = 0x3FF;
				FpaMask[Controller] = 0x3F;
				OneNand_Inform[Controller].uNumOfBlock = 1024;
				break;
		case 4 :	Outp32(&ONENAND(Controller)->rFpaWidth, 6);
				Outp32(&ONENAND(Controller)->rFsaWidth, 2);
				if(uDeviceDDP)
				{
					Outp32(&ONENAND(Controller)->rFbaWidth, 10);
					Outp32(&ONENAND(Controller)->rDbsDfsWidth, 1);
					//added by rb1004....2007.04.23
					OneNand_Inform[Controller].uDbsDfs = 1;
					OneNand_Inform[Controller].uCacheReadSupport = 0;
				}
				else
				{
					Outp32(&ONENAND(Controller)->rFbaWidth, 11);
					Outp32(&ONENAND(Controller)->rDbsDfsWidth, 0);
					//added by rb1004....2007.04.23
					OneNand_Inform[Controller].uDbsDfs = 0;
					OneNand_Inform[Controller].uCacheReadSupport = 1;
				}
#if(OND_DEVICE == OND_6410)				
				FbaShift[Controller] = 12;
				FpaShift[Controller] = 6;
#else
				FbaShift[Controller] = 10;
				FpaShift[Controller] = 4;
#endif				
				FbaMask[Controller] = 0x7FF;
				FpaMask[Controller] = 0x3F;
				OneNand_Inform[Controller].uNumOfBlock = 2048;
				break;	
		case 5 :	Outp32(&ONENAND(Controller)->rFpaWidth, 6);
				Outp32(&ONENAND(Controller)->rFsaWidth, 2);
				if(uDeviceDDP)
				{
					Outp32(&ONENAND(Controller)->rFbaWidth, 11);
					Outp32(&ONENAND(Controller)->rDbsDfsWidth, 1);
					//added by rb1004....2007.04.23
					OneNand_Inform[Controller].uDbsDfs = 1;
					OneNand_Inform[Controller].uCacheReadSupport = 1;
				}
				else
				{
					Outp32(&ONENAND(Controller)->rFbaWidth, 12);
					Outp32(&ONENAND(Controller)->rDbsDfsWidth, 0);
					//added by rb1004....2007.04.23
					OneNand_Inform[Controller].uDbsDfs = 0;
					OneNand_Inform[Controller].uCacheReadSupport = 1;
				}
#if(OND_DEVICE == OND_6410)				
				FbaShift[Controller] = 12;
				FpaShift[Controller] = 6;
#else				
				FbaShift[Controller] = 10;
				FpaShift[Controller] = 4;
#endif				
				FbaMask[Controller] = 0xFFF;
				FpaMask[Controller] = 0x3F;
				OneNand_Inform[Controller].uNumOfBlock = 4096;
				break;
		default :	break;
	}

	//added by rb1004....2007.06.13
	OneNand_Inform[Controller].uTotalBlockWidth = 	
						(Inp32(&ONENAND(Controller)->rFbaWidth)&0x1F) + (Inp32(&ONENAND(Controller)->rDbsDfsWidth)&0x3);
	
	if(Controller == 0)
	{
		OneNAND_BUFFER_BASE[Controller] = (ONENAND0_MEM_BASE + BUFFER);
		OneNAND_ARRAY_BASE[Controller] = (ONENAND0_MEM_BASE + ARRAY_RW);
		OneNAND_CMD_BASE[Controller] = (ONENAND0_MEM_BASE + COMMANDS);
		OneNAND_DIRECT_BASE[Controller] = (ONENAND0_MEM_BASE + DIRECT_ACCESS);
	}
	else if(Controller == 1)
	{
		OneNAND_BUFFER_BASE[Controller] = (ONENAND1_MEM_BASE + BUFFER);
		OneNAND_ARRAY_BASE[Controller] = (ONENAND1_MEM_BASE + ARRAY_RW);
		OneNAND_CMD_BASE[Controller] = (ONENAND1_MEM_BASE + COMMANDS);
		OneNAND_DIRECT_BASE[Controller] = (ONENAND1_MEM_BASE + DIRECT_ACCESS);
	}

	OneNand_Inform[Controller].uNumOfPage = Pow(2, Inp32(&ONENAND(Controller)->rFpaWidth));
}

//////////
// Function Name : ONENAND_UnlockBlock
// Function Description : Unlock Memory Block
// Input : 	Controller - OneNand Controller Port Number 
//			uStartBlkAddr - Start block for the unlock
//			uEndBlkAddr - End block for the unlock
// Version : v0.1
u8 ONENAND_UnlockBlock(u32 uController, u32 uStartBlkAddr, u32 uEndBlkAddr)
{
	//u32 uData;

#if (ONENAND_ACCESS_MODE == ONENAND_COMMAND)
	OneNandT_oIntFlag.IntActInt = 0;
	
	if(uStartBlkAddr == uEndBlkAddr)
	{
		ONENAND_WriteCmd(uController, uEndBlkAddr, 0, 0x09);
	}
	else
	{
		ONENAND_WriteCmd(uController, uStartBlkAddr, 0, 0x08);
		ONENAND_WriteCmd(uController, uEndBlkAddr, 0, 0x09);
	}	

#if (ONENAND_INTMODE == ONENAND_INTERRUPT)
	while(!OneNandT_oIntFlag.IntActInt);
#else
	while( !(Inp32(&ONENAND(uController)->rIntErrStat) & eOND_INTACT) );
	Outp32(&ONENAND(uController)->rIntErrAck, eOND_INTACT);
#endif

#if 0	
	ONENAND_DirectRead(Controller, OND_WPROT_STATUS, &uData);

	if ((uData&0xFFFF) != 0x4) 
	{
		//UART_Printf(" Failed unlock block, locked status (0x%x)\n", uData);
		return TRUE;
	}
#endif

	return FALSE;

#else	// ONENAND_ACCESS_MODE == ONENAND_DIRECT

#if (ONENAND_INTMODE == ONENAND_INTERRUPT)		// Polling Mode is Not implemented
	u32 i;

	for(i=uStartBlkAddr ; i<=uEndBlkAddr ; i++)
	{
		OneNandT_oIntFlag.IntActInt = 0;

		if((OneNand_Inform[uController].uTotalBlockWidth == 10) || (OneNand_Inform[uController].uTotalBlockWidth == 11))		// 1Gb Single(1024Block), 2Gb DDP(1024*2Block)
		{
			ONENAND_DirectWrite(uController, OND_STARTADDR1, (DFS(i) | FBA(i)));
			ONENAND_DirectWrite(uController, OND_STARTADDR2, DBS(i));
		}
		else if(OneNand_Inform[uController].uTotalBlockWidth == 12)		// 2Gb Single(2048Block), 4Gb DDP(2048*2Block)
		{
			ONENAND_DirectWrite(uController, OND_STARTADDR1, (DFS2(i) | FBA2(i)));
			ONENAND_DirectWrite(uController, OND_STARTADDR2, DBS2(i));
		}
		else
		{
			// Not implemented, 128Mb, 256Mb, 512Mb
			Assert(0);
		}

		ONENAND_DirectWrite(uController, OND_STARTBLOCKADDR, i);
		ONENAND_DirectWrite(uController, OND_INTSTATUS, 0);
		ONENAND_DirectWrite(uController, OND_COMMAND, OND_UNLOCK);
	
		while(!OneNandT_oIntFlag.IntActInt);
	}
	return FALSE;
#endif
#endif
}


//////////
// Function Name : ONENAND_UnlockAllBlock
// Function Description : Unlock Memory All Block
// Input : 	Controller - OneNand Controller Port Number 
// Version : v0.1
OneNAND_eINTERROR ONENAND_UnlockAllBlock(u32 uController)
{
	//u32 uError;

	OneNandT_oIntFlag.IntActInt = 0;
	OneNandT_oIntFlag.UnsupCmdInt = 0;

#if (ONENAND_ACCESS_MODE == ONENAND_COMMAND)	
	ONENAND_WriteCmd(uController, 0, 0, 0x0E);

#if (ONENAND_INTMODE == ONENAND_INTERRUPT)
	while(!OneNandT_oIntFlag.IntActInt);
#else
	while( !(Inp32(&ONENAND(uController)->rIntErrStat) & eOND_INTACT) );
	Outp32(&ONENAND(uController)->rIntErrAck, eOND_INTACT);
#endif


#else	// ONENAND_ACCESS_MODE == ONENAND_DIRECT

#if (ONENAND_INTMODE == ONENAND_INTERRUPT)		// Polling Mode is Not implemented
	if((OneNand_Inform[uController].uTotalBlockWidth == 10) || (OneNand_Inform[uController].uTotalBlockWidth == 11))		// 1Gb Single(1024Block), 2Gb DDP(1024*2Block)
	{
		ONENAND_DirectWrite(uController, OND_STARTADDR1, 0);
		ONENAND_DirectWrite(uController, OND_STARTADDR2, DBS(0));
	}
	else if(OneNand_Inform[uController].uTotalBlockWidth == 12)		// 2Gb Single(2048Block), 4Gb DDP(2048*2Block)
	{
		ONENAND_DirectWrite(uController, OND_STARTADDR1, 0);
		ONENAND_DirectWrite(uController, OND_STARTADDR2, DBS2(0));
	}
	else
	{
		// Not implemented, 128Mb, 256Mb, 512Mb
		Assert(0);
	}

	ONENAND_DirectWrite(uController, OND_STARTBLOCKADDR, 0);
	ONENAND_DirectWrite(uController, OND_INTSTATUS, 0);
	ONENAND_DirectWrite(uController, OND_COMMAND, OND_ALLUNLOCK);

	while(!OneNandT_oIntFlag.IntActInt);

	if(OneNand_Inform[uController].uDbsDfs == 1)
	{
		OneNandT_oIntFlag.IntActInt = 0;
		
		if((OneNand_Inform[uController].uTotalBlockWidth == 10) || (OneNand_Inform[uController].uTotalBlockWidth == 11))		// 1Gb Single(1024Block), 2Gb DDP(1024*2Block)
		{
			ONENAND_DirectWrite(uController, OND_STARTADDR1, 0x8000);
			ONENAND_DirectWrite(uController, OND_STARTADDR2, 0x8000);
		}
		else if(OneNand_Inform[uController].uTotalBlockWidth == 12)		// 2Gb Single(2048Block), 4Gb DDP(2048*2Block)
		{
			ONENAND_DirectWrite(uController, OND_STARTADDR1, 0x8000);
			ONENAND_DirectWrite(uController, OND_STARTADDR2, 0x8000);
		}
		else
		{
			// Not implemented, 128Mb, 256Mb, 512Mb
			Assert(0);
		}

		ONENAND_DirectWrite(uController, OND_STARTBLOCKADDR, 0);
		ONENAND_DirectWrite(uController, OND_INTSTATUS, 0);
		ONENAND_DirectWrite(uController, OND_COMMAND, OND_ALLUNLOCK);

		while(!OneNandT_oIntFlag.IntActInt);
	}
#endif		// ONENAND_INTMODE == ONENAND_INTERRUPT
#endif		// ONENAND_ACCESS_MODE == ONENAND_DIRECT

#if (ONENAND_INTMODE == ONENAND_INTERRUPT)	
	if(OneNandT_oIntFlag.UnsupCmdInt == 1)
		return eOND_UNSUPCMD;
	return eOND_NOERROR;
#else
	if(Inp32(&ONENAND(uController)->rIntErrStat) & eOND_UNSUPCMD)
		return eOND_UNSUPCMD;
	return eOND_NOERROR;
#endif
}

//////////
// Function Name : ONENAND_LockBlock
// Function Description : Lock Memory Block
// Input : 	Controller - OneNand Controller Port Number 
//			uStartBlkAddr - Start block for the lock
//			uEndBlkAddr - End block for the lock
// Version : v0.1
u8 ONENAND_LockBlock(u32 uController, u32 uStartBlkAddr, u32 uEndBlkAddr)
{

#if (ONENAND_ACCESS_MODE == ONENAND_COMMAND)
	OneNandT_oIntFlag.IntActInt = 0;
		
	if(uStartBlkAddr == uEndBlkAddr)
	{
		ONENAND_WriteCmd(uController, uEndBlkAddr, 0, 0x0B);
	}
	else
	{
		ONENAND_WriteCmd(uController, uStartBlkAddr, 0, 0x0A);
		ONENAND_WriteCmd(uController, uEndBlkAddr, 0, 0x0B);
	}	

#if (ONENAND_INTMODE == ONENAND_INTERRUPT)
	while(!OneNandT_oIntFlag.IntActInt);
#else
	while( !(Inp32(&ONENAND(uController)->rIntErrStat) & eOND_INTACT) );
	Outp32(&ONENAND(uController)->rIntErrAck, eOND_INTACT);
#endif

#else	// ONENAND_ACCESS_MODE == ONENAND_DIRECT

#if (ONENAND_INTMODE == ONENAND_INTERRUPT)		// Polling Mode is Not implemented
	u32 i;

	for(i=uStartBlkAddr ; i<=uEndBlkAddr ; i++)
	{
		OneNandT_oIntFlag.IntActInt = 0;

		if((OneNand_Inform[uController].uTotalBlockWidth == 10) || (OneNand_Inform[uController].uTotalBlockWidth == 11))		// 1Gb Single(1024Block), 2Gb DDP(1024*2Block)
		{
			ONENAND_DirectWrite(uController, OND_STARTADDR1, (DFS(i) | FBA(i)));
			ONENAND_DirectWrite(uController, OND_STARTADDR2, DBS(i));
		}
		else if(OneNand_Inform[uController].uTotalBlockWidth == 12)		// 2Gb Single(2048Block), 4Gb DDP(2048*2Block)
		{
			ONENAND_DirectWrite(uController, OND_STARTADDR1, (DFS2(i) | FBA2(i)));
			ONENAND_DirectWrite(uController, OND_STARTADDR2, DBS2(i));
		}
		else
		{
			// Not implemented, 128Mb, 256Mb, 512Mb
			Assert(0);
		}

		ONENAND_DirectWrite(uController, OND_STARTBLOCKADDR, i);
		ONENAND_DirectWrite(uController, OND_INTSTATUS, 0);
		ONENAND_DirectWrite(uController, OND_COMMAND, OND_LOCK);
	
		while(!OneNandT_oIntFlag.IntActInt);
	}
#endif		// ONENAND_INTMODE == ONENAND_INTERRUPT
#endif		// ONENAND_ACCESS_MODE == ONENAND_DIRECT

	return FALSE;
}

//////////
// Function Name : ONENAND_LockTightBlock
// Function Description : Lock-tight Memory Block
// Input : 	Controller - OneNand Controller Port Number 
//			uStartBlkAddr - Start block for the lock-tight
//			uEndBlkAddr - End block for the lock-tight
// Version : v0.1
u8 ONENAND_LockTightBlock(u32 uController, u32 uStartBlkAddr, u32 uEndBlkAddr)
{

#if (ONENAND_ACCESS_MODE == ONENAND_COMMAND)
	OneNandT_oIntFlag.IntActInt = 0;
	
	if(uStartBlkAddr == uEndBlkAddr)
	{
		ONENAND_WriteCmd(uController, uEndBlkAddr, 0, 0x0D);
	}
	else
	{
		ONENAND_WriteCmd(uController, uStartBlkAddr, 0, 0x0C);
		ONENAND_WriteCmd(uController, uEndBlkAddr, 0, 0x0D);
	}	

#if (ONENAND_INTMODE == ONENAND_INTERRUPT)
	while(!OneNandT_oIntFlag.IntActInt);
#else
	while( !(Inp32(&ONENAND(uController)->rIntErrStat) & eOND_INTACT) );
	Outp32(&ONENAND(uController)->rIntErrAck, eOND_INTACT);
#endif

#else	// ONENAND_ACCESS_MODE == ONENAND_DIRECT

#if (ONENAND_INTMODE == ONENAND_INTERRUPT)		// Polling Mode is Not implemented
	u32 i;

	for(i=uStartBlkAddr ; i<=uEndBlkAddr ; i++)
	{
		OneNandT_oIntFlag.IntActInt = 0;

		if((OneNand_Inform[uController].uTotalBlockWidth == 10) || (OneNand_Inform[uController].uTotalBlockWidth == 11))		// 1Gb Single(1024Block), 2Gb DDP(1024*2Block)
		{
			ONENAND_DirectWrite(uController, OND_STARTADDR1, (DFS(i) | FBA(i)));
			ONENAND_DirectWrite(uController, OND_STARTADDR2, DBS(i));
		}
		else if(OneNand_Inform[uController].uTotalBlockWidth == 12)		// 2Gb Single(2048Block), 4Gb DDP(2048*2Block)
		{
			ONENAND_DirectWrite(uController, OND_STARTADDR1, (DFS2(i) | FBA2(i)));
			ONENAND_DirectWrite(uController, OND_STARTADDR2, DBS2(i));
		}
		else
		{
			// Not implemented, 128Mb, 256Mb, 512Mb
			Assert(0);
		}

		ONENAND_DirectWrite(uController, OND_STARTBLOCKADDR, i);
		ONENAND_DirectWrite(uController, OND_INTSTATUS, 0);
		ONENAND_DirectWrite(uController, OND_COMMAND, OND_LOCK_TIGHT);
	
		while(!OneNandT_oIntFlag.IntActInt);
	}
#endif		// ONENAND_INTMODE == ONENAND_INTERRUPT
#endif		// ONENAND_ACCESS_MODE == ONENAND_DIRECT

	return FALSE;
}


//////////
// Function Name : ONENAND_EraseBlock
// Function Description : Erase Memory Block
// Input : 	Controller - OneNand Controller Port Number 
//			uStartBlkAddr - Start block for the erase
//			uEndBlkAddr - End block for the erase
// Version : v0.1
OneNAND_eINTERROR ONENAND_EraseBlock(u32 Controller, u32 uStartBlkAddr, u32 uEndBlkAddr)
{
	u32 i, j, uBlockSize, uQuotient, uRemainder;
	u32 uBlock;

	if(uEndBlkAddr < uStartBlkAddr)
		return eOND_NOERROR;

#if (ONENAND_ACCESS_MODE == ONENAND_COMMAND)

#if (ONENAND_INTMODE == ONENAND_INTERRUPT)
	OneNandT_oIntFlag.IntActInt = 0;
	OneNandT_oIntFlag.ErsFailInt = 0;
	OneNandT_oIntFlag.LockedBlkInt = 0;
	OneNandT_oIntFlag.ErsCmpInt = 0;
#endif	

	if(uStartBlkAddr == uEndBlkAddr)
	{
		ONENAND_WriteCmd(Controller, uEndBlkAddr, 0, 0x03);
#if (ONENAND_INTMODE == ONENAND_INTERRUPT)
		while(!OneNandT_oIntFlag.IntActInt);

		OneNandT_oIntFlag.IntActInt = 0;
		ONENAND_WriteCmd(Controller, uEndBlkAddr, 0, 0x15);
		while(!OneNandT_oIntFlag.IntActInt);

		if(OneNandT_oIntFlag.ErsFailInt == 1)
		{
			OneNandT_oIntFlag.ErsFailInt = 0;
			if(OneNandT_oIntFlag.LockedBlkInt == 1)
				return (OneNAND_eINTERROR)(eOND_ERSFAIL | eOND_LOCKEDBLK);
			return eOND_ERSFAIL;
		}
#else
		while( !(Inp32(&ONENAND(Controller)->rIntErrStat) & eOND_INTACT) );
		Outp32(&ONENAND(Controller)->rIntErrAck, eOND_INTACT);
		
		ONENAND_WriteCmd(Controller, uEndBlkAddr, 0, 0x15);
		
		while( !(Inp32(&ONENAND(Controller)->rIntErrStat) & eOND_INTACT) );
		Outp32(&ONENAND(Controller)->rIntErrAck, eOND_INTACT);	

		DelayfrTimer(micro, 1);
		
		if(Inp32(&ONENAND(Controller)->rIntErrStat) & eOND_ERSFAIL)
		{
			Outp32(&ONENAND(Controller)->rIntErrAck, eOND_ERSFAIL);
			
			if(Inp32(&ONENAND(Controller)->rIntErrStat) & eOND_LOCKEDBLK)
			{
				Outp32(&ONENAND(Controller)->rIntErrAck, eOND_LOCKEDBLK);
				return (OneNAND_eINTERROR)(eOND_ERSFAIL | eOND_LOCKEDBLK);
			}
			return eOND_ERSFAIL;
		}		
#endif

	}
	else
	{
		uBlockSize = uEndBlkAddr - uStartBlkAddr + 1;
		uQuotient = uBlockSize/OND_MULTIERASE_SIZE;
		uRemainder = uBlockSize%OND_MULTIERASE_SIZE;
		uBlock = uStartBlkAddr;
	
		for(i=0 ; i<uQuotient ; i++)
		{
			for(j=uBlock ; j<(uBlock+OND_MULTIERASE_SIZE-1) ; j++)
			{
#if (ONENAND_INTMODE == ONENAND_INTERRUPT)			
				OneNandT_oIntFlag.IntActInt = 0;
				OneNandT_oIntFlag.ErsCmpInt= 0;

				ONENAND_WriteCmd(Controller, j, 0, 0x01);
				
				while(!OneNandT_oIntFlag.IntActInt);
				while(!OneNandT_oIntFlag.ErsCmpInt);
				
				OneNandT_oIntFlag.IntActInt = 0;
				OneNandT_oIntFlag.ErsCmpInt= 0;
#else	
				ONENAND_WriteCmd(Controller, j, 0, 0x01);

				while( !(Inp32(&ONENAND(Controller)->rIntErrStat) & eOND_INTACT) );
				Outp32(&ONENAND(Controller)->rIntErrAck, eOND_INTACT);
				while( !(Inp32(&ONENAND(Controller)->rIntErrStat) & eOND_ERSCMP) );
				Outp32(&ONENAND(Controller)->rIntErrAck, eOND_ERSCMP);
#endif				
			}
			ONENAND_WriteCmd(Controller, uBlock+OND_MULTIERASE_SIZE-1, 0, 0x03);

#if (ONENAND_INTMODE == ONENAND_INTERRUPT)
			while(!OneNandT_oIntFlag.IntActInt);
			while(!OneNandT_oIntFlag.ErsCmpInt);
#else
			while( !(Inp32(&ONENAND(Controller)->rIntErrStat) & eOND_INTACT) );
			Outp32(&ONENAND(Controller)->rIntErrAck, eOND_INTACT);
			while( !(Inp32(&ONENAND(Controller)->rIntErrStat) & eOND_ERSCMP) );
			Outp32(&ONENAND(Controller)->rIntErrAck, eOND_ERSCMP);
#endif

			// Verify Erase block
			for(j=uBlock ; j<=(uBlock+OND_MULTIERASE_SIZE-1) ; j++)
			{
#if (ONENAND_INTMODE == ONENAND_INTERRUPT)			
				OneNandT_oIntFlag.IntActInt = 0;

				ONENAND_WriteCmd(Controller, j, 0, 0x15);
				while(!OneNandT_oIntFlag.IntActInt);
				
				if(OneNandT_oIntFlag.ErsFailInt == 1)
				{
					OneNandT_oIntFlag.ErsFailInt = 0;
					if(OneNandT_oIntFlag.LockedBlkInt == 1)
						return (OneNAND_eINTERROR)(eOND_ERSFAIL | eOND_LOCKEDBLK);
					return eOND_ERSFAIL;
				}			
#else
				ONENAND_WriteCmd(Controller, j, 0, 0x15);

				while( !(Inp32(&ONENAND(Controller)->rIntErrStat) & eOND_INTACT) );
				Outp32(&ONENAND(Controller)->rIntErrAck, eOND_INTACT);

				if(Inp32(&ONENAND(Controller)->rIntErrStat) & eOND_ERSFAIL)
				{
					Outp32(&ONENAND(Controller)->rIntErrAck, eOND_ERSFAIL);
					
					if(Inp32(&ONENAND(Controller)->rIntErrStat) & eOND_LOCKEDBLK)
					{
						Outp32(&ONENAND(Controller)->rIntErrAck, eOND_LOCKEDBLK);
						return (OneNAND_eINTERROR)(eOND_ERSFAIL | eOND_LOCKEDBLK);
					}
					return eOND_ERSFAIL;
				}		
#endif
			}
			uBlock +=  OND_MULTIERASE_SIZE;
		}

		if(uRemainder > 0)
		{
			for(i=uBlock ; i<(uBlock+uRemainder-1) ; i++)
			{
#if (ONENAND_INTMODE == ONENAND_INTERRUPT)			
				OneNandT_oIntFlag.IntActInt = 0;
				OneNandT_oIntFlag.ErsCmpInt= 0;
				ONENAND_WriteCmd(Controller, i, 0, 0x01);
				while(!OneNandT_oIntFlag.IntActInt);
				while(!OneNandT_oIntFlag.ErsCmpInt);
#else	
				ONENAND_WriteCmd(Controller, i, 0, 0x01);
				while( !(Inp32(&ONENAND(Controller)->rIntErrStat) & eOND_INTACT) );
				Outp32(&ONENAND(Controller)->rIntErrAck, eOND_INTACT);
				while( !(Inp32(&ONENAND(Controller)->rIntErrStat) & eOND_ERSCMP) );
				Outp32(&ONENAND(Controller)->rIntErrAck, eOND_ERSCMP);
#endif	
			}
			ONENAND_WriteCmd(Controller, uBlock+uRemainder-1, 0, 0x03);

#if (ONENAND_INTMODE == ONENAND_INTERRUPT)
			while(!OneNandT_oIntFlag.IntActInt);
			while(!OneNandT_oIntFlag.ErsCmpInt);
#else
			while( !(Inp32(&ONENAND(Controller)->rIntErrStat) & eOND_INTACT) );
			Outp32(&ONENAND(Controller)->rIntErrAck, eOND_INTACT);
			while( !(Inp32(&ONENAND(Controller)->rIntErrStat) & eOND_ERSCMP) );
			Outp32(&ONENAND(Controller)->rIntErrAck, eOND_ERSCMP);			
#endif

			// Verify Erase block
			for(i=uBlock ; i<=(uBlock+uRemainder-1) ; i++)
			{
#if (ONENAND_INTMODE == ONENAND_INTERRUPT)			
				OneNandT_oIntFlag.IntActInt = 0;
				ONENAND_WriteCmd(Controller, i, 0, 0x15);
				while(!OneNandT_oIntFlag.IntActInt);
				
				if(OneNandT_oIntFlag.ErsFailInt == 1)
				{
					OneNandT_oIntFlag.ErsFailInt = 0;
					if(OneNandT_oIntFlag.LockedBlkInt == 1)
						return (OneNAND_eINTERROR)(eOND_ERSFAIL | eOND_LOCKEDBLK);
					return eOND_ERSFAIL;
				}			
#else
				ONENAND_WriteCmd(Controller, i, 0, 0x15);
				while( !(Inp32(&ONENAND(Controller)->rIntErrStat) & eOND_INTACT) );
				Outp32(&ONENAND(Controller)->rIntErrAck, eOND_INTACT);

				if(Inp32(&ONENAND(Controller)->rIntErrStat) & eOND_ERSFAIL)
				{
					Outp32(&ONENAND(Controller)->rIntErrAck, eOND_ERSFAIL);
					
					if(Inp32(&ONENAND(Controller)->rIntErrStat) & eOND_LOCKEDBLK)
					{
						Outp32(&ONENAND(Controller)->rIntErrAck, eOND_LOCKEDBLK);
						return (OneNAND_eINTERROR)(eOND_ERSFAIL | eOND_LOCKEDBLK);
					}
					return eOND_ERSFAIL;
				}				
#endif
				
			
			}
		}
	}
#if (ONENAND_INTMODE == ONENAND_INTERRUPT)		
	if(OneNandT_oIntFlag.ErsFailInt == 1)
		return eOND_ERSFAIL;
	else
		return eOND_NOERROR;
#else
	if(Inp32(&ONENAND(Controller)->rIntErrStat) & eOND_ERSFAIL)
		return eOND_ERSFAIL;
	else
		return eOND_NOERROR;
#endif
	
#else	// (ONENAND_ACCESS_MODE == ONENAND_DIRECT)

#if (ONENAND_INTMODE == ONENAND_INTERRUPT)	  		// Polling Mode is Not implemented 
	if(uStartBlkAddr == uEndBlkAddr)
	{
		ONENAND_EraseBlockDirectAccess(Controller, uEndBlkAddr);

		if(OneNandT_oIntFlag.ErsFailInt == 1)
		{
			OneNandT_oIntFlag.ErsFailInt = 0;
			if(OneNandT_oIntFlag.LockedBlkInt == 1)
				return (OneNAND_eINTERROR)(eOND_ERSFAIL | eOND_LOCKEDBLK);
			return eOND_ERSFAIL;
		}
	}
	else
	{
		uBlockSize = uEndBlkAddr - uStartBlkAddr + 1;
		uQuotient = uBlockSize/OND_MULTIERASE_SIZE;
		uRemainder = uBlockSize%OND_MULTIERASE_SIZE;
		uBlock = uStartBlkAddr;
	
		for(i=0 ; i<uQuotient ; i++)
		{

			ONENAND_EraseMultiBlockDirectAccess(Controller, uBlock, uBlock+OND_MULTIERASE_SIZE-1);
			uBlock +=  OND_MULTIERASE_SIZE;
		}

		if(uRemainder > 0)
		{
			if(uRemainder == 1)
				ONENAND_EraseBlockDirectAccess(Controller, uBlock);
			else
				ONENAND_EraseMultiBlockDirectAccess(Controller, uBlock, uBlock+uRemainder-1);
		}
		
		if(OneNandT_oIntFlag.ErsFailInt == 1)
		{
			OneNandT_oIntFlag.ErsFailInt = 0;
			if(OneNandT_oIntFlag.LockedBlkInt == 1)
				return (OneNAND_eINTERROR)(eOND_ERSFAIL | eOND_LOCKEDBLK);
			return eOND_ERSFAIL;
		}
	}

	return eOND_NOERROR;
#endif		// ONENAND_INTMODE == ONENAND_INTERRUPT
#endif		// ONENAND_ACCESS_MODE == ONENAND_DIRECT

}


//////////
// Function Name : ONENAND_EraseVerify
// Function Description : Verify Memory Block Erase
// Input : 	Controller - OneNand Controller Port Number 
//			uBlkAddr - Block Number
// Version : v0.1
OneNAND_eINTERROR ONENAND_EraseVerify(u32 Controller, u32 uBlkAddr)
{
	u8 uError;

#if (ONENAND_INTMODE == ONENAND_INTERRUPT)
	OneNandT_oIntFlag.IntActInt = 0;
#endif
	
	ONENAND_WriteCmd(Controller, uBlkAddr, 0, 0x15);

#if (ONENAND_INTMODE == ONENAND_INTERRUPT)
	while(!OneNandT_oIntFlag.IntActInt);
#else
	while( !(Inp32(&ONENAND(Controller)->rIntErrStat) & eOND_INTACT) );
	Outp32(&ONENAND(Controller)->rIntErrAck, eOND_INTACT);
#endif
	
	uError = Inp32(&ONENAND(Controller)->rIntErrStat);
	if (uError & (1<<3)) 
	{
		//Erase fail
		Outp32(&ONENAND(Controller)->rIntErrAck, (1<<3));
		return eOND_ERSFAIL;
	}
	return eOND_NOERROR;
}

//////////
// Function Name : ONENAND_CopyBack
// Function Description : Copy-back operation
// Input : 	Controller - OneNand Controller Port Number 
//			uSourceBlkAddr - Source Block number
//			uSourcePageAddr - Source Page number
//			uDestinationBlkAddr - Destination Block number
//			uDestinationPageAddr - Destination Page number
//			uPageSize - Page size to be copied
// Version : v0.1
u8 ONENAND_CopyBack(u32 Controller, u32 uSourceBlkAddr, u8 uSourcePageAddr, u32 uDestinationBlkAddr, u8 uDestinationPageAddr, u32 uPageSize)
{
#if (ONENAND_INTMODE == ONENAND_INTERRUPT)
	OneNandT_oIntFlag.IntActInt = 0;
	OneNandT_oIntFlag.BlkRwCmpInt = 0;
	OneNandT_oIntFlag.PgmCmpInt = 0;
#endif
	
	ONENAND_WriteCmd(Controller, uSourceBlkAddr, uSourcePageAddr, 0x1000);
	ONENAND_WriteCmd(Controller, uDestinationBlkAddr, uDestinationPageAddr, (0x2000|(uPageSize&0xFF)));
	
#if (ONENAND_INTMODE == ONENAND_INTERRUPT)
	while(!OneNandT_oIntFlag.IntActInt);
	while(!OneNandT_oIntFlag.BlkRwCmpInt);
	while(!OneNandT_oIntFlag.PgmCmpInt);
#else
	while( !(Inp32(&ONENAND(Controller)->rIntErrStat) & eOND_INTACT) );
	Outp32(&ONENAND(Controller)->rIntErrAck, eOND_INTACT);
	while( !(Inp32(&ONENAND(Controller)->rIntErrStat) & eOND_BLKRWCMP) );
	Outp32(&ONENAND(Controller)->rIntErrAck, eOND_BLKRWCMP);
	while( !(Inp32(&ONENAND(Controller)->rIntErrStat) & eOND_PGMCMP) );
	Outp32(&ONENAND(Controller)->rIntErrAck, eOND_PGMCMP);	
#endif
		
	return TRUE;
}

//////////
// Function Name : ONENAND_WritePage
// Function Description : Write 1 Page
// Input : 	Controller - OneNand Controller Port Number 
//			uBlkAddr - Block Number to write
//			uPageAddr - Page number to write
//			aData - Start Address of the data array
// Version : v0.1

u8 ONENAND_WritePage(u32 Controller, u32 uBlkAddr, u8 uPageAddr, u32* aData)
{
#if (ONENAND_ACCESS_MODE == ONENAND_COMMAND)

#if (ONENAND_INTMODE == ONENAND_INTERRUPT)
	OneNandT_oIntFlag.IntActInt = 0;
	OneNandT_oIntFlag.BlkRwCmpInt = 0;
	OneNandT_oIntFlag.PgmCmpInt = 0;
#endif	

#if (OND_TRANS_MODE == OND_POLLING)
	{
		u32 i;
		
		for(i=0; i<ONENAND_PAGESIZE/4; i++)
		{
			ONENAND_WriteIntoFlash(Controller, uBlkAddr, uPageAddr, *aData);
			aData++;
		}
	}

#elif (OND_TRANS_MODE == OND_DMA)
	{
		u32 uDstAddr;

		OneNand_DmaDone = 0;
		uDstAddr = OneNAND_ARRAY_BASE[Controller]  + ((FbaMask[Controller] & uBlkAddr)<<FbaShift[Controller]) +((FpaMask[Controller] & uPageAddr)<<FpaShift[Controller]);

		DMACH_Setup(OND_Dma_Channel, 0, (u32)aData, 0, (u32)uDstAddr, 0, WORD, ONENAND_PAGESIZE/4, DEMAND, MEM, DMA0_OND_RX, BURST4, &g_oONDDmac0);

		DMACH_Start(&g_oONDDmac0);

		while(!OneNand_DmaDone);	
	}
#else
	{
		u32 uDstAddr, uContBaseAddr;

		if(Controller == 0)
			uContBaseAddr = ONENAND0_BASE;
		else
			uContBaseAddr = ONENAND1_BASE;
		
		uDstAddr = OneNAND_ARRAY_BASE[Controller]  + ((FbaMask[Controller] & uBlkAddr)<<FbaShift[Controller]) +((FpaMask[Controller] & uPageAddr)<<FpaShift[Controller]);

		#if (OND_LDM_BURST_SIZE	== OND_LDM_8BURST)
			OneNand8burstPageWrite(uContBaseAddr, (u32)aData, uDstAddr);
		#else
			OneNand4burstPageWrite(uContBaseAddr, (u32)aData, uDstAddr);
		#endif

	}	
#endif

#if (ONENAND_INTMODE == ONENAND_INTERRUPT)
	while(!OneNandT_oIntFlag.IntActInt);
	while(!OneNandT_oIntFlag.BlkRwCmpInt);
	while(!OneNandT_oIntFlag.PgmCmpInt);
#else
	while( !(Inp32(&ONENAND(Controller)->rIntErrStat) & eOND_INTACT) );
	Outp32(&ONENAND(Controller)->rIntErrAck, eOND_INTACT);
	while( !(Inp32(&ONENAND(Controller)->rIntErrStat) & eOND_BLKRWCMP) );
	Outp32(&ONENAND(Controller)->rIntErrAck, eOND_BLKRWCMP);
	while( !(Inp32(&ONENAND(Controller)->rIntErrStat) & eOND_PGMCMP) );
	Outp32(&ONENAND(Controller)->rIntErrAck, eOND_PGMCMP);	
#endif
	
	return TRUE;

#else	// (ONENAND_ACCESS_MODE == ONENAND_DIRECT)

#if (ONENAND_INTMODE == ONENAND_INTERRUPT)		// Polling Mode is NOT implemented

#if (OND_TRANS_MODE == OND_DMA)
	ONENAND_DirectWrite_DMA(Controller, ONENAND_DATARAM0_BASE, (u32 *)aData);
#elif(OND_TRANS_MODE == OND_LDM_INST)
	{
		u32 uDstAddr, uContBaseAddr;

		if(Controller == 0)
			uContBaseAddr = ONENAND0_BASE;
		else
			uContBaseAddr = ONENAND1_BASE;
		
		uDstAddr = OneNAND_BUFFER_BASE[Controller]  + ONENAND_DATARAM0_BASE;
		OneNand8burstPageWrite(uContBaseAddr, (u32)aData, uDstAddr);
	}
#else
	{
		u32 i;
	
		for(i=0; i<ONENAND_PAGESIZE/4; i++)
		{
			ONENAND_WriteIntoDataram(Controller, ONENAND_DATARAM0_BASE+(4*i),*aData);
			aData++;
		}
	}
#endif
	ONENAND_WritePageDirectAccess(Controller, uBlkAddr, uPageAddr);
		
	return TRUE;
#endif		// ONENAND_INTMODE == ONENAND_INTERRUPT	
#endif		// (ONENAND_ACCESS_MODE == ONENAND_DIRECT)
}



//////////
// Function Name : ONENAND_WritePageWithSpare
// Function Description : Write 1 Page wit spare area
// Input : 	Controller - OneNand Controller Port Number 
//			uBlkAddr - Block Number to write
//			uPageAddr - Page number to write
//			aData - Start Address of the data array
// Version : v0.1

u8 ONENAND_WritePageWithSpare(u32 Controller, u32 uBlkAddr, u8 uPageAddr, u32* aData, u32* pSpare)
{
#if (ONENAND_ACCESS_MODE == ONENAND_COMMAND)

#if (ONENAND_INTMODE == ONENAND_INTERRUPT)
	OneNandT_oIntFlag.IntActInt = 0;
	OneNandT_oIntFlag.BlkRwCmpInt = 0;
	OneNandT_oIntFlag.PgmCmpInt = 0;
#endif	

#if (OND_TRANS_MODE == OND_POLLING)
	{
		u32 i;
		
		for(i=0; i<ONENAND_PAGESIZE/4; i++)
		{
			ONENAND_WriteIntoFlash(Controller, uBlkAddr, uPageAddr, *aData);
			aData++;
		}

		for(i=0; i<ONENAND_SPARESIZE/4; i++)
		{
			ONENAND_WriteIntoFlash(Controller, uBlkAddr, uPageAddr, *pSpare);
			pSpare++;
		}
	}

#elif (OND_TRANS_MODE == OND_DMA)
	{
		u32 uDstAddr;

		OneNand_DmaDone = 0;
		uDstAddr = OneNAND_ARRAY_BASE[Controller]  + ((FbaMask[Controller] & uBlkAddr)<<FbaShift[Controller]) +((FpaMask[Controller] & uPageAddr)<<FpaShift[Controller]);

		DMACH_Setup(OND_Dma_Channel, 0, (u32)aData, 0, (u32)uDstAddr, 0, WORD, ONENAND_PAGESIZE/4, DEMAND, MEM, DMA0_OND_RX, BURST4, &g_oONDDmac0);
	
		DMACH_Start(&g_oONDDmac0);
		while(!OneNand_DmaDone);	

		OneNand_DmaDone = 0;
		DMACH_Setup(OND_Dma_Channel, 0, (u32)pSpare, 0, (u32)uDstAddr, 0, WORD, ONENAND_SPARESIZE/4, DEMAND, MEM, DMA0_OND_RX, BURST4, &g_oONDDmac0);
	
		DMACH_Start(&g_oONDDmac0);
		while(!OneNand_DmaDone);	
		
	}
#else
	{
		u32 uDstAddr, uContBaseAddr;

		if(Controller == 0)
			uContBaseAddr = ONENAND0_BASE;
		else
			uContBaseAddr = ONENAND1_BASE;
		
		uDstAddr = OneNAND_ARRAY_BASE[Controller]  + ((FbaMask[Controller] & uBlkAddr)<<FbaShift[Controller]) +((FpaMask[Controller] & uPageAddr)<<FpaShift[Controller]);
#if (OND_LDM_BURST_SIZE	== OND_LDM_8BURST)
		OneNand8burstPageWrite(uContBaseAddr, (u32)aData, uDstAddr);
//		DelayfrTimer(milli, 85);
		OneNand8burstSpareWrite(uContBaseAddr, (u32)pSpare, uDstAddr);
#else
		OneNand4burstPageWrite(uContBaseAddr, (u32)aData, uDstAddr);
//		DelayfrTimer(milli, 85);
		OneNand4burstSpareWrite(uContBaseAddr, (u32)pSpare, uDstAddr);		
#endif
	}	
#endif

#if (ONENAND_INTMODE == ONENAND_INTERRUPT)
	while(!OneNandT_oIntFlag.IntActInt);
	while(!OneNandT_oIntFlag.BlkRwCmpInt);
	while(!OneNandT_oIntFlag.PgmCmpInt);	
#else
	while( !(Inp32(&ONENAND(Controller)->rIntErrStat) & eOND_INTACT) );
	Outp32(&ONENAND(Controller)->rIntErrAck, eOND_INTACT);
	while( !(Inp32(&ONENAND(Controller)->rIntErrStat) & eOND_BLKRWCMP) );
	Outp32(&ONENAND(Controller)->rIntErrAck, eOND_BLKRWCMP);
	while( !(Inp32(&ONENAND(Controller)->rIntErrStat) & eOND_PGMCMP) );
	Outp32(&ONENAND(Controller)->rIntErrAck, eOND_PGMCMP);	
#endif

	return TRUE;

#else	// (ONENAND_ACCESS_MODE == ONENAND_DIRECT)

#if (ONENAND_INTMODE == ONENAND_INTERRUPT)		// Polling Mode is NOT implemented

#if (OND_TRANS_MODE == OND_DMA)
	ONENAND_DirectWrite_DMA(Controller, ONENAND_DATARAM0_BASE, (u32 *)aData);
#elif(OND_TRANS_MODE == OND_LDM_INST)
	{
		u32 uDstAddr, uContBaseAddr;

		if(Controller == 0)
			uContBaseAddr = ONENAND0_BASE;
		else
			uContBaseAddr = ONENAND1_BASE;
		
		uDstAddr = OneNAND_BUFFER_BASE[Controller]  + ONENAND_DATARAM0_BASE;
		OneNand8burstPageWriteDir(uContBaseAddr, (u32)aData, uDstAddr);
	}
#else
	{
		u32 i;
	
		for(i=0; i<ONENAND_PAGESIZE/4; i++)
		{
			ONENAND_WriteIntoDataram(Controller, ONENAND_DATARAM0_BASE+(4*i),*aData);
			aData++;
		}
	}
#endif	
	ONENAND_WritePageDirectAccess(Controller, uBlkAddr, uPageAddr);
		
	return TRUE;
#endif		// ONENAND_INTMODE == ONENAND_INTERRUPT	
#endif		// (ONENAND_ACCESS_MODE == ONENAND_DIRECT)
}



//////////
// Function Name : ONENAND_ReadPage
// Function Description : Read 1 Page
// Input : 	Controller - OneNand Controller Port Number 
//			uBlkAddr - Block Number to read
//			uPageAddr - Page number to read
//			aData - Start Address of the data array
// Version : v0.1
extern void TCM_DMA_ReadTransfer(u32 uSrcAddr, u32 uDstAddr);
u8 ONENAND_ReadPage(u32 Controller, u32 uBlkAddr, u8 uPageAddr, u32* aData)
{

#if (ONENAND_ACCESS_MODE == ONENAND_COMMAND)

#if (ONENAND_INTMODE == ONENAND_INTERRUPT)
	OneNandT_oIntFlag.IntActInt = 0;
	OneNandT_oIntFlag.BlkRwCmpInt = 0;
	OneNandT_oIntFlag.LoadCmpInt = 0;
#endif

#if (OND_TRANS_MODE == OND_POLLING)
	{
		u32 i;
	
		for(i=0; i<ONENAND_PAGESIZE/4; i++)
		{
			ONENAND_ReadOutFlash(Controller, uBlkAddr, uPageAddr, aData);
			aData++;
		}
	}
#elif (OND_TRANS_MODE == OND_DMA)
	{
#if 0		//TCM DMA
		u32 uSrcAddr;

		uSrcAddr = OneNAND_ARRAY_BASE[Controller]  + ((FbaMask[Controller] & uBlkAddr)<<FbaShift[Controller]) +((FpaMask[Controller] & uPageAddr)<<FpaShift[Controller]);

		TCM_DMA_ReadTransfer(uSrcAddr, (u32)aData);

#else	
		u32 uSrcAddr;

		OneNand_DmaDone = 0;
		uSrcAddr = OneNAND_ARRAY_BASE[Controller]  + ((FbaMask[Controller] & uBlkAddr)<<FbaShift[Controller]) +((FpaMask[Controller] & uPageAddr)<<FpaShift[Controller]);

		DMACH_Setup(OND_Dma_Channel, 0, (u32)uSrcAddr, 0, (u32)aData, 0, WORD, ONENAND_PAGESIZE/4, DEMAND, DMA0_OND_TX, MEM, BURST4, &g_oONDDmac0);

		DMACH_Start(&g_oONDDmac0);

		while(!OneNand_DmaDone);	
#endif		
	}
#else	// using the "LDM" instruction
	{
		u32 uSrcAddr, uContBaseAddr;

		if(Controller == 0)
			uContBaseAddr = ONENAND0_BASE;
		else
			uContBaseAddr = ONENAND1_BASE;
		
		uSrcAddr = OneNAND_ARRAY_BASE[Controller]  + ((FbaMask[Controller] & uBlkAddr)<<FbaShift[Controller]) +((FpaMask[Controller] & uPageAddr)<<FpaShift[Controller]);

		#if (OND_LDM_BURST_SIZE	== OND_LDM_8BURST)
			OneNand8burstPageRead(uContBaseAddr, uSrcAddr, (u32)aData);
		#else
			OneNand4burstPageRead(uContBaseAddr, uSrcAddr, (u32)aData);
		#endif
	}
#endif

#if (ONENAND_INTMODE == ONENAND_INTERRUPT)
	while(!OneNandT_oIntFlag.IntActInt);
	while(!OneNandT_oIntFlag.BlkRwCmpInt);
	while(!OneNandT_oIntFlag.LoadCmpInt);	

	OneNandT_oIntFlag.IntActInt = 0;
	OneNandT_oIntFlag.BlkRwCmpInt = 0;
	OneNandT_oIntFlag.LoadCmpInt = 0;
#else
	while( !(Inp32(&ONENAND(Controller)->rIntErrStat) & eOND_INTACT) );
	Outp32(&ONENAND(Controller)->rIntErrAck, eOND_INTACT);
	while( !(Inp32(&ONENAND(Controller)->rIntErrStat) & eOND_BLKRWCMP) );
	Outp32(&ONENAND(Controller)->rIntErrAck, eOND_BLKRWCMP);
	while( !(Inp32(&ONENAND(Controller)->rIntErrStat) & eOND_LOADCMP) );
	Outp32(&ONENAND(Controller)->rIntErrAck, eOND_LOADCMP);	
#endif
	
	return TRUE;

	
#else	// (ONENAND_ACCESS_MODE == ONENAND_DIRECT)

#if (ONENAND_INTMODE == ONENAND_INTERRUPT)		// Polling Mode is NOT implemented
	ONENAND_ReadPageDirectAccess(Controller, uBlkAddr, uPageAddr);
#if (OND_TRANS_MODE == OND_DMA)
	ONENAND_DirectRead_DMA(Controller, ONENAND_DATARAM0_BASE, (u32 *)aData);
#elif (OND_TRANS_MODE == OND_LDM_INST)
	{
		u32 uSrcAddr, uContBaseAddr;

		if(Controller == 0)
			uContBaseAddr = ONENAND0_BASE;
		else
			uContBaseAddr = ONENAND1_BASE;
		
		uSrcAddr = OneNAND_BUFFER_BASE[Controller]  + ONENAND_DATARAM0_BASE;
		OneNand8burstPageReadDir(uContBaseAddr, uSrcAddr, (u32)aData);
	}
#else
	{
		u32 i;
	
		for(i=0; i<ONENAND_PAGESIZE/4; i++)
		{
			ONENAND_ReadOutDataram(Controller, ONENAND_DATARAM0_BASE+(4*i),aData);
			aData++;
		}
	}
#endif
		
	return TRUE;
#endif		// ONENAND_INTMODE == ONENAND_INTERRUPT
#endif
}



u8 ONENAND_ReadPageForCacheRead(u32 Controller, u32 uBlkAddr, u8 uPageAddr, u32* aData, u32 uPageSize)
{

#if (ONENAND_ACCESS_MODE == ONENAND_COMMAND)

#if (ONENAND_INTMODE == ONENAND_INTERRUPT)
	OneNandT_oIntFlag.IntActInt = 0;
	OneNandT_oIntFlag.BlkRwCmpInt = 0;
	OneNandT_oIntFlag.LoadCmpInt = 0;
#endif
	
#if (OND_TRANS_MODE == OND_POLLING)
	{
		u32 i;
	
		for(i=0; i<ONENAND_PAGESIZE/4; i++)
		{
			ONENAND_ReadOutFlash(Controller, uBlkAddr, uPageAddr, aData);
			aData++;
		}
	}
#elif (OND_TRANS_MODE == OND_DMA)
	{
		u32 uSrcAddr;

		OneNand_DmaDone = 0;
		uSrcAddr = OneNAND_ARRAY_BASE[Controller]  + ((FbaMask[Controller] & uBlkAddr)<<FbaShift[Controller]) +((FpaMask[Controller] & uPageAddr)<<FpaShift[Controller]);

		DMACH_Setup(OND_Dma_Channel, 0, (u32)uSrcAddr, 0, (u32)aData, 0, WORD, ONENAND_PAGESIZE/4, DEMAND, DMA0_OND_TX, MEM, BURST4, &g_oONDDmac0);
	
		DMACH_Start(&g_oONDDmac0);
		while(!OneNand_DmaDone);	
	}
#else
	{
		u32 uSrcAddr, uContBaseAddr;

		if(Controller == 0)
			uContBaseAddr = ONENAND0_BASE;
		else
			uContBaseAddr = ONENAND1_BASE;
		
		uSrcAddr = OneNAND_ARRAY_BASE[Controller]  + ((FbaMask[Controller] & uBlkAddr)<<FbaShift[Controller]) +((FpaMask[Controller] & uPageAddr)<<FpaShift[Controller]);
		#if (OND_LDM_BURST_SIZE	== OND_LDM_8BURST)
			OneNand8burstPageRead(uContBaseAddr, uSrcAddr, (u32)aData);
		#else
			OneNand4burstPageRead(uContBaseAddr, uSrcAddr, (u32)aData);
		#endif
//		DelayfrTimer(milli, 15);
//		OneNand4burstPageRead_test(uContBaseAddr, uSrcAddr, (u32)(aData+4*0x70));		
	}
#endif

#if 0
	// rb1004...When HCLK is low(~50MHz), Interrupt loss is occured.
	if(OneNandT_oIntFlag.IntActInt_Count  != uPageSize)
	{
		while(!OneNandT_oIntFlag.IntActInt);
		while(!OneNandT_oIntFlag.BlkRwCmpInt);
		while(!OneNandT_oIntFlag.LoadCmpInt);	
	}
#endif

#if (ONENAND_INTMODE == ONENAND_POLLING)
	Outp32(&ONENAND(Controller)->rIntErrAck, eOND_INTACT);
	Outp32(&ONENAND(Controller)->rIntErrAck, eOND_BLKRWCMP);
	Outp32(&ONENAND(Controller)->rIntErrAck, eOND_LOADCMP);	
#endif

	return TRUE;

#else	// (ONENAND_ACCESS_MODE == ONENAND_DIRECT)

#if (OND_TRANS_MODE == OND_DMA)
	ONENAND_DirectRead_DMA(Controller, ONENAND_DATARAM0_BASE, (u32 *)aData);
#elif (OND_TRANS_MODE == OND_LDM_INST)
	{
		u32 uSrcAddr, uContBaseAddr;

		if(Controller == 0)
			uContBaseAddr = ONENAND0_BASE;
		else
			uContBaseAddr = ONENAND1_BASE;
		
		uSrcAddr = OneNAND_BUFFER_BASE[Controller]  + ONENAND_DATARAM0_BASE;
		OneNand8burstPageReadDir(uContBaseAddr, uSrcAddr, (u32)aData);
	}
#else
	{
		u32 i;
	
		for(i=0; i<ONENAND_PAGESIZE/4; i++)
		{
			ONENAND_ReadOutDataram(Controller, ONENAND_DATARAM0_BASE+(4*i),aData);
			aData++;
		}
	}
#endif
		
	return TRUE;
#endif
}



u8 ONENAND_ReadPageForCacheReadWithSpare(u32 Controller, u32 uBlkAddr, u8 uPageAddr, u32* aData, u32 *aSpare, u32 uPageSize)
{
	u32 j;

#if (ONENAND_INTMODE == ONENAND_INTERRUPT)	
	OneNandT_oIntFlag.IntActInt = 0;
	OneNandT_oIntFlag.BlkRwCmpInt = 0;
	OneNandT_oIntFlag.LoadCmpInt = 0;
#endif

#if (OND_TRANS_MODE == OND_POLLING)
	{
		u32 i;

//		UART_Printf("1");
		for(i=0; i<ONENAND_PAGESIZE/4; i++)
		{
			ONENAND_ReadOutFlash(Controller, uBlkAddr, uPageAddr, aData);
			aData++;
		}
	}

//	UART_Printf("2");
	DelayfrTimer(milli, 1);
		
	for(j=0; j<ONENAND_SPARESIZE/4; j++)
	{
		ONENAND_ReadOutFlash(Controller, uBlkAddr, uPageAddr, aSpare);
		aSpare++;
	}
//	UART_Printf("3");
#elif (OND_TRANS_MODE == OND_DMA)
	{
		u32 uSrcAddr;

		OneNand_DmaDone = 0;
		uSrcAddr = OneNAND_ARRAY_BASE[Controller]  + ((FbaMask[Controller] & uBlkAddr)<<FbaShift[Controller]) +((FpaMask[Controller] & uPageAddr)<<FpaShift[Controller]);

		//DMACH_Setup(OND_Dma_Channel, 0, (u32)uSrcAddr, 0, (u32)aData, 0, WORD, ONENAND_PAGESIZE/4, DEMAND, DMA0_OND_TX, MEM, BURST4, &g_oONDDmac0);
		DMACH_Setup(OND_Dma_Channel, 0, (u32)uSrcAddr, 0, (u32)aData, 0, WORD, ONENAND_PAGESIZE/4 + ONENAND_SPARESIZE/4, DEMAND, DMA0_OND_TX, MEM, BURST4, &g_oONDDmac0);

		DMACH_Start(&g_oONDDmac0);

		while(!OneNand_DmaDone);	
#if 0		
		DelayfrTimer(milli, 1);
		
		//Spare Area Read
		OneNand_DmaDone = 0;
		DMACH_Setup(OND_Dma_Channel, 0, (u32)uSrcAddr, 0, (u32)aSpare, 0, WORD, ONENAND_SPARESIZE/4, DEMAND, DMA0_OND_TX, MEM, BURST4, &g_oONDDmac0);
	
		DMACH_Start(&g_oONDDmac0);
		while(!OneNand_DmaDone);	
		UART_Printf("3");
#endif

	}
#else		//LDM
	{
		u32 i, uConfig, uSrcAddr, uContBaseAddr;

		if(Controller == 0)
			uContBaseAddr = ONENAND0_BASE;
		else
			uContBaseAddr = ONENAND1_BASE;
		
		uSrcAddr = OneNAND_ARRAY_BASE[Controller]  + ((FbaMask[Controller] & uBlkAddr)<<FbaShift[Controller]) +((FpaMask[Controller] & uPageAddr)<<FpaShift[Controller]);

//		UART_Printf("1");
		#if (OND_LDM_BURST_SIZE	== OND_LDM_8BURST)
			OneNand8burstPageRead(uContBaseAddr, uSrcAddr, (u32)aData);
		#else
			OneNand4burstPageRead(uContBaseAddr, uSrcAddr, (u32)aData);
		#endif
//		UART_Printf("2");
//		DelayfrTimer(milli, 20);
//		OneNand4burstPageRead_test(uContBaseAddr, uSrcAddr, (u32)aData+4*0x70);
//		UART_Printf("3");
//		DelayfrTimer(milli, 1);

#if 0	// for OneNand WatchDog Error Test. System hang(at 6400)
		DelayfrTimer(milli, 100);
#endif	
		#if (OND_LDM_BURST_SIZE	== OND_LDM_8BURST)
			OneNand8burstSpareRead(uContBaseAddr, uSrcAddr, (u32)aSpare);
		#else
			OneNand4burstSpareRead(uContBaseAddr, uSrcAddr, (u32)aSpare);
		#endif
//		UART_Printf("4");
	}
#endif
	
#if 0
	// rb1004...When HCLK is low(~50MHz), Interrupt loss is occured.
	if(OneNandT_oIntFlag.IntActInt_Count  != uPageSize)
	{
		while(!OneNandT_oIntFlag.IntActInt);
		while(!OneNandT_oIntFlag.BlkRwCmpInt);
		while(!OneNandT_oIntFlag.LoadCmpInt);	
	}
#endif

#if (ONENAND_INTMODE == ONENAND_POLLING)
	Outp32(&ONENAND(Controller)->rIntErrAck, eOND_INTACT);
	Outp32(&ONENAND(Controller)->rIntErrAck, eOND_BLKRWCMP);
	Outp32(&ONENAND(Controller)->rIntErrAck, eOND_LOADCMP);	
#endif

	return TRUE;

}



//////////
// Function Name : ONENAND_ReadPageWithSpare
// Function Description : Read 1 Page with Spare area data
// Input : 	Controller - OneNand Controller Port Number 
//			uBlkAddr - Block Number to read
//			uPageAddr - Page number to read
//			aData - Start Address of the data array
// Version : v0.1
u8 ONENAND_ReadPageWithSpare(u32 Controller, u32 uBlkAddr, u8 uPageAddr, u32* aData, u32 *aSpare)
{
	u32 j;
	
#if (ONENAND_ACCESS_MODE == ONENAND_COMMAND)

#if (ONENAND_INTMODE == ONENAND_INTERRUPT)
	OneNandT_oIntFlag.IntActInt = 0;
	OneNandT_oIntFlag.BlkRwCmpInt = 0;
	OneNandT_oIntFlag.LoadCmpInt = 0;
#endif	

#if (OND_TRANS_MODE == OND_POLLING)
	{
		u32 i;
	
		for(i=0; i<ONENAND_PAGESIZE/4; i++)
		{
			ONENAND_ReadOutFlash(Controller, uBlkAddr, uPageAddr, aData);
			aData++;
		}
	}

	for(j=0; j<ONENAND_SPARESIZE/4; j++)
	{
		ONENAND_ReadOutFlash(Controller, uBlkAddr, uPageAddr, aSpare);
		aSpare++;
	}
#elif (OND_TRANS_MODE == OND_DMA)
	{
#if 0		//TCM DMA
		u32 uSrcAddr;

		uSrcAddr = OneNAND_ARRAY_BASE[Controller]  + ((FbaMask[Controller] & uBlkAddr)<<FbaShift[Controller]) +((FpaMask[Controller] & uPageAddr)<<FpaShift[Controller]);

		TCM_DMA_ReadTransfer(uSrcAddr, (u32)aData);

#else	
		u32 uSrcAddr;

		OneNand_DmaDone = 0;
		uSrcAddr = OneNAND_ARRAY_BASE[Controller]  + ((FbaMask[Controller] & uBlkAddr)<<FbaShift[Controller]) +((FpaMask[Controller] & uPageAddr)<<FpaShift[Controller]);
		DMACH_Setup(OND_Dma_Channel, 0, (u32)uSrcAddr, 0, (u32)aData, 0, WORD, ONENAND_PAGESIZE/4, DEMAND, DMA0_OND_TX, MEM, BURST4, &g_oONDDmac0);
		DMACH_Start(&g_oONDDmac0);
		while(!OneNand_DmaDone);	

		OneNand_DmaDone = 0;
		DMACH_Setup(OND_Dma_Channel, 0, (u32)uSrcAddr, 0, (u32)aSpare, 0, WORD, ONENAND_SPARESIZE/4, DEMAND, DMA0_OND_TX, MEM, BURST4, &g_oONDDmac0);
		DMACH_Start(&g_oONDDmac0);
		while(!OneNand_DmaDone);			
#endif		
	}
#else	// using the "LDM" instruction
	{
		u32 uSrcAddr, uContBaseAddr;

		if(Controller == 0)
			uContBaseAddr = ONENAND0_BASE;
		else
			uContBaseAddr = ONENAND1_BASE;
		
		uSrcAddr = OneNAND_ARRAY_BASE[Controller]  + ((FbaMask[Controller] & uBlkAddr)<<FbaShift[Controller]) +((FpaMask[Controller] & uPageAddr)<<FpaShift[Controller]);
		#if (OND_LDM_BURST_SIZE	== OND_LDM_8BURST)
			OneNand8burstPageRead(uContBaseAddr, uSrcAddr, (u32)aData);
		#else
			OneNand4burstPageRead(uContBaseAddr, uSrcAddr, (u32)aData);
		#endif

//		DelayfrTimer(milli, 85);
		#if (OND_LDM_BURST_SIZE	== OND_LDM_8BURST)
			OneNand8burstSpareRead(uContBaseAddr, uSrcAddr, (u32)aSpare);
		#else
			OneNand4burstSpareRead(uContBaseAddr, uSrcAddr, (u32)aSpare);
		#endif
	}
	
#endif

#if (ONENAND_INTMODE == ONENAND_INTERRUPT)
	while(!OneNandT_oIntFlag.IntActInt);
	while(!OneNandT_oIntFlag.BlkRwCmpInt);
	while(!OneNandT_oIntFlag.LoadCmpInt);	

	OneNandT_oIntFlag.IntActInt = 0;
	OneNandT_oIntFlag.BlkRwCmpInt = 0;
	OneNandT_oIntFlag.LoadCmpInt = 0;
#else
	while( !(Inp32(&ONENAND(Controller)->rIntErrStat) & eOND_INTACT) );
	Outp32(&ONENAND(Controller)->rIntErrAck, eOND_INTACT);
	while( !(Inp32(&ONENAND(Controller)->rIntErrStat) & eOND_BLKRWCMP) );
	Outp32(&ONENAND(Controller)->rIntErrAck, eOND_BLKRWCMP);
	while( !(Inp32(&ONENAND(Controller)->rIntErrStat) & eOND_LOADCMP) );
	Outp32(&ONENAND(Controller)->rIntErrAck, eOND_LOADCMP);	
#endif
	
#else	// (ONENAND_ACCESS_MODE == ONENAND_DIRECT)

#if (ONENAND_INTMODE == ONENAND_INTERRUPT)		// Polling Mode is NOT implemented
	ONENAND_ReadPageDirectAccess(Controller, uBlkAddr, uPageAddr);
#if (OND_TRANS_MODE == OND_DMA)
	ONENAND_DirectRead_DMA(Controller, ONENAND_DATARAM0_BASE, (u32 *)aData);
#elif (OND_TRANS_MODE == OND_LDM_INST)
	{
		u32 uSrcAddr, uContBaseAddr;

		if(Controller == 0)
			uContBaseAddr = ONENAND0_BASE;
		else
			uContBaseAddr = ONENAND1_BASE;
		
		uSrcAddr = OneNAND_BUFFER_BASE[Controller]  + ONENAND_DATARAM0_BASE;
		OneNand8burstPageReadDir(uContBaseAddr, uSrcAddr, (u32)aData);
	}
#else
	{
		u32 i;
	
		for(i=0; i<ONENAND_PAGESIZE/4; i++)
		{
			ONENAND_ReadOutDataram(Controller, ONENAND_DATARAM0_BASE+(4*i),aData);
			aData++;
		}
	}
#endif

	for(j=0; j<ONENAND_SPARESIZE/4; j++)
	{
		ONENAND_ReadOutDataram(Controller, ONENAND_SPARERAM00_BASE+(4*j), aSpare);
		aSpare++;
	}
#endif
#endif
	
	return TRUE;
}


//////////
// Function Name : ONENAND_LoadToXIPBuffer
// Function Description : Load the page specified by the FBA,FPA,FSA to the map00 XIP buffer
// Input : 	Controller - OneNand Controller Port Number 
//			uBlkAddr - Block Number to load
//			uPageAddr - Page number to load
// Version : v0.1
void ONENAND_LoadToXIPBuffer(u32 Controller, u32 uBlkAddr, u8 uPageAddr)
{
	OneNandT_oIntFlag.IntActInt = 0;
	OneNandT_oIntFlag.BlkRwCmpInt = 0;
	OneNandT_oIntFlag.LoadCmpInt = 0;
	
	ONENAND_WriteCmd(Controller, uBlkAddr, uPageAddr, 0x10);

#if (ONENAND_INTMODE == ONENAND_INTERRUPT)
	while(!OneNandT_oIntFlag.IntActInt);
	while(!OneNandT_oIntFlag.BlkRwCmpInt);
	while(!OneNandT_oIntFlag.LoadCmpInt);	
#else
	while( !(Inp32(&ONENAND(Controller)->rIntErrStat) & eOND_INTACT) );
	Outp32(&ONENAND(Controller)->rIntErrAck, eOND_INTACT);
	while( !(Inp32(&ONENAND(Controller)->rIntErrStat) & eOND_BLKRWCMP) );
	Outp32(&ONENAND(Controller)->rIntErrAck, eOND_BLKRWCMP);
	while( !(Inp32(&ONENAND(Controller)->rIntErrStat) & eOND_LOADCMP) );
	Outp32(&ONENAND(Controller)->rIntErrAck, eOND_LOADCMP);	
#endif
}



//////////
// Function Name : ONENAND_WriteFromXIPBuffer
// Function Description : Write the data int the map00 XIP buffer to the page specified by the FBA,FPA, and FSA
// Input : 	Controller - OneNand Controller Port Number 
//			uBlkAddr - Block Number to write
//			uPageAddr - Page number to write
// Version : v0.1
void ONENAND_WriteFromXIPBuffer(u32 Controller, u32 uBlkAddr, u8 uPageAddr)
{
	OneNandT_oIntFlag.IntActInt = 0;
	OneNandT_oIntFlag.BlkRwCmpInt = 0;
	OneNandT_oIntFlag.PgmCmpInt = 0;
	
	ONENAND_WriteCmd(Controller, uBlkAddr, uPageAddr, 0x11);

#if (ONENAND_INTMODE == ONENAND_INTERRUPT)
	while(!OneNandT_oIntFlag.IntActInt);
	while(!OneNandT_oIntFlag.BlkRwCmpInt);
	while(!OneNandT_oIntFlag.PgmCmpInt);
#else
	while( !(Inp32(&ONENAND(Controller)->rIntErrStat) & eOND_INTACT) );
	Outp32(&ONENAND(Controller)->rIntErrAck, eOND_INTACT);
	while( !(Inp32(&ONENAND(Controller)->rIntErrStat) & eOND_BLKRWCMP) );
	Outp32(&ONENAND(Controller)->rIntErrAck, eOND_BLKRWCMP);
	while( !(Inp32(&ONENAND(Controller)->rIntErrStat) & eOND_PGMCMP) );
	Outp32(&ONENAND(Controller)->rIntErrAck, eOND_PGMCMP);	
#endif
}



//////////
// Function Name : ONENAND_PipelineReadAhead
// Function Description : Initiate a copy of PP pages from the memory to the memory controller
// Input : 	Controller - OneNand Controller Port Number 
//			uBlkAddr - Block Number to write
//			uPageAddr - Page number to write
// Version : v0.1
void ONENAND_PipelineReadAhead(u32 uController, u32 uBlkAddr, u8 uPageAddr, u8 uPageSize)
{
	ONENAND_WriteCmd(uController, uBlkAddr, uPageAddr, (0x4000|(u16)uPageSize));

}



//////////
// Function Name : ONENAND_ConfigOTPAccess
// Function Description : Configure the controller to access the OTP area of the flash
// Input : 	Controller - OneNand Controller Port Number 
//			uBlkAddr - Block Number to load
//			uPageAddr - Page number to load
// Version : v0.1
void ONENAND_ConfigOTPAccess(u32 Controller, u32 uBlkAddr, u8 uPageAddr)
{
	OneNandT_oIntFlag.IntActInt = 0;
	
	ONENAND_WriteCmd(Controller, uBlkAddr, uPageAddr, 0x12);

#if (ONENAND_INTMODE == ONENAND_INTERRUPT)
	while(!OneNandT_oIntFlag.IntActInt);
#else
	while( !(Inp32(&ONENAND(Controller)->rIntErrStat) & eOND_INTACT) );
	Outp32(&ONENAND(Controller)->rIntErrAck, eOND_INTACT);
#endif
}


//////////
// Function Name : ONENAND_ConfigSpareAccess
// Function Description : Configure the controller to access the spare area of the flash
// Input : 	Controller - OneNand Controller Port Number 
//			uBlkAddr - Block Number to load
//			uPageAddr - Page number to load
// Version : v0.1
void ONENAND_ConfigSpareAccess(u32 Controller, u32 uBlkAddr, u8 uPageAddr)
{
	OneNandT_oIntFlag.IntActInt = 0;
	
	ONENAND_WriteCmd(Controller, uBlkAddr, uPageAddr, 0x13);

#if (ONENAND_INTMODE == ONENAND_INTERRUPT)
	while(!OneNandT_oIntFlag.IntActInt);
#else
	while( !(Inp32(&ONENAND(Controller)->rIntErrStat) & eOND_INTACT) );
	Outp32(&ONENAND(Controller)->rIntErrAck, eOND_INTACT);
#endif
}


//////////
// Function Name : ONENAND_ConfigMainAccess
// Function Description : Configure the controller to access the main area of the flash
// Input : 	Controller - OneNand Controller Port Number 
//			uBlkAddr - Block Number to load
//			uPageAddr - Page number to load
// Version : v0.1
void ONENAND_ConfigMainAccess(u32 Controller, u32 uBlkAddr, u8 uPageAddr)
{
	OneNandT_oIntFlag.IntActInt = 0;
	ONENAND_WriteCmd(Controller, uBlkAddr, uPageAddr, 0x14);

#if 0
#if (ONENAND_INTMODE == ONENAND_INTERRUPT)
	while(!OneNandT_oIntFlag.IntActInt);
#else
	while( !(Inp32(&ONENAND(Controller)->rIntErrStat) & eOND_INTACT) );
	Outp32(&ONENAND(Controller)->rIntErrAck, eOND_INTACT);
#endif
#endif
}



//////////
// Function Name : ONENAND_EnableIntPin
// Function Description : Interrupt Pin Enable&Disable
// Input : 	Controller - OneNand Controller Port Number 
//			Enable - 1:Enable, 0: Disable
// Version : v0.1
void ONENAND_EnableIntPin(u32 Controller, u32 Enable)
{
	Outp32(&ONENAND(Controller)->rIntPinEnable, Enable&0x01);
}

//////////
// Function Name : ONENAND_WriteIntoDataram
// Function Description : Write into DataRam(Map00)
// Input : 	Controller - OneNand Controller Port Number 
//			uAddr - XIP buffer Address
//			uData - Data to write
// Version : v0.1
void ONENAND_WriteIntoDataram(u32 Controller, u32 uAddr, u32 uData)
{
	Outp32(OneNAND_BUFFER_BASE[Controller] + ((0x1fffe & uAddr)<<0), uData);
}

//////////
// Function Name : ONENAND_ReadOutDataram
// Function Description : Read from DataRam(Map00)
// Input : 	Controller - OneNand Controller Port Number 
//			uAddr - XIP buffer Address
//			uData - Data to read
// Version : v0.1
void ONENAND_ReadOutDataram(u32 Controller, u32 uAddr, u32 *uData)
{
	*uData = Inp32(OneNAND_BUFFER_BASE[Controller]  + ((0x1fffe & uAddr)<<0));
}

//////////
// Function Name : ONENAND_WriteIntoFlash
// Function Description : Normal high-speed write accesses to the memory array(Map01)
// Input : 	Controller - OneNand Controller Port Number 
//			uBlockAddr - Block Address to write
//			uPageAddr - Page Address to write
//			uData - Data to write
// Version : v0.1
void ONENAND_WriteIntoFlash(u32 Controller, u32 uBlockAddr, u8 uPageAddr, u32 uData)
{
	Outp32(OneNAND_ARRAY_BASE[Controller]  + ((FbaMask[Controller] & uBlockAddr)<<FbaShift[Controller]) +((FpaMask[Controller] & uPageAddr)<<FpaShift[Controller]), uData);
}

//////////
// Function Name : ONENAND_ReadOutFlash
// Function Description : Normal high-speed read accesses to the memory array(Map01)
// Input : 	Controller - OneNand Controller Port Number 
//			uBlockAddr - Block Address to read
//			uPageAddr - Page Address to read
//			uData - Data to read
// Version : v0.1
void ONENAND_ReadOutFlash(u32 Controller, u32 uBlockAddr, u8 uPageAddr, u32 *uData)
{
	*uData = Inp32(OneNAND_ARRAY_BASE[Controller]  + ((FbaMask[Controller] & uBlockAddr)<<FbaShift[Controller]) +((FpaMask[Controller] & uPageAddr)<<FpaShift[Controller]));
}

//////////
// Function Name : ONENAND_WriteCmd
// Function Description : Control the special functions of the memory device(Map10)
// Input : 	Controller - OneNand Controller Port Number 
//			uBlockAddr - Block Address to access
//			uPageAddr - Page Address to access
//			uData - Data to access
// Version : v0.1
void ONENAND_WriteCmd(u32 Controller, u32 uBlockAddr, u8 uPageAddr, u32 uData)
{
	Outp32(OneNAND_CMD_BASE[Controller]  + ((FbaMask[Controller] & uBlockAddr)<<FbaShift[Controller]) +((FpaMask[Controller] & uPageAddr)<<FpaShift[Controller]), uData);
}

//////////
// Function Name : ONENAND_ReadCmd
// Function Description : Control the special functions of the memory device(Map10)
// Input : 	Controller - OneNand Controller Port Number 
//			uBlockAddr - Block Address to access
//			uPageAddr - Page Address to access
//			uData - Data to access
// Version : v0.1
void ONENAND_ReadCmd(u32 Controller, u32 BlockAddr, u8 uPageAddr, u32 *uData)
{
	*uData = Inp32(OneNAND_CMD_BASE[Controller]  + ((FbaMask[Controller] & BlockAddr)<<FbaShift[Controller])+((FpaMask[Controller] & uPageAddr)<<FpaShift[Controller]));
}

//////////
// Function Name : ONENAND_DirectWrite
// Function Description : Direct Memory Access(Map11)
// Input : 	Controller - OneNand Controller Port Number 
//			uDeviceAddr - Memory Device Address
//			uData - Data to write
// Version : v0.1
void ONENAND_DirectWrite(u32 Controller, u32 uDeviceAddr, u32 uData)
{
	Outp32(OneNAND_DIRECT_BASE[Controller]  + uDeviceAddr, uData);
}

//////////
// Function Name : ONENAND_DirectRead
// Function Description : Direct Memory Access(Map11)
// Input : 	Controller - OneNand Controller Port Number 
//			uDeviceAddr - Memory Device Address
//			uData - Data to read
// Version : v0.1
void ONENAND_DirectRead(u32 Controller, u32 uDeviceAddr, u32 *uData)
{
	*uData = Inp32(OneNAND_DIRECT_BASE[Controller]  + uDeviceAddr);
}


#if (OND_TRANS_MODE == OND_DMA)
//////////
// Function Name : ONENAND_DirectRead_DMA
// Function Description : Direct Memory Access(Map11) using DMA
// Input : 	Controller - OneNand Controller Port Number 
//			uDeviceAddr - Memory Device Address
//			uData - Data to read
// Version : v0.1
void ONENAND_DirectRead_DMA(u32 Controller, u32 uDeviceAddr, u32 *uData)
{
	u32 uSrcAddr;

	OneNand_DmaDone = 0;
	uSrcAddr = OneNAND_BUFFER_BASE[Controller]  + ((0x1fffe & uDeviceAddr)<<0);

	DMACH_Setup(DMA_A, 0x0, (u32)uSrcAddr, 0, (u32)uData, 0, WORD, ONENAND_PAGESIZE/4, DEMAND, MEM, MEM, BURST128, &g_oONDDmac0);
				
	DMACH_Start(&g_oONDDmac0);
	while(!OneNand_DmaDone);
}


//////////
// Function Name : ONENAND_DirectWrite_DMA
// Function Description : Direct Memory Access(Map11) using DMA
// Input : 	Controller - OneNand Controller Port Number 
//			uDeviceAddr - Memory Device Address
//			uData - Data to write
// Version : v0.1
void ONENAND_DirectWrite_DMA(u32 Controller, u32 uDeviceAddr, u32 *uData)
{
	u32 uDstAddr;

	OneNand_DmaDone = 0;
	uDstAddr = OneNAND_BUFFER_BASE[Controller]  + ((0x1fffe & uDeviceAddr)<<0);

	DMACH_Setup(DMA_A, 0x0, (u32)uData, 0, (u32)uDstAddr, 0, WORD, ONENAND_PAGESIZE/4, DEMAND, MEM, MEM, BURST128, &g_oONDDmac0);
				
	DMACH_Start(&g_oONDDmac0);
	while(!OneNand_DmaDone);
}
#endif

//////////
// Function Name : ONENAND_EnableInterrupt
// Function Description : Interrupt Status Enable
// Input : 	Controller - OneNand Controller Port Number 
//			eInterrupt - Interrupt Source
// Version : v0.1
void ONENAND_EnableInterrupt(u32 uController, OneNAND_eINTERROR eInterrupt)
{
	u32 uEnableBit;

	uEnableBit = Inp32(&ONENAND(uController)->rIntErrMask);

	Outp32(&ONENAND(uController)->rIntErrMask, (u32)(uEnableBit | eInterrupt));

	ONENAND_EnableIntPin(uController, 1);
#if 0
	if(eInterrupt == eOND_INTACT)
	{
		ONENAND_EnableIOBE(uController, 1);
		Outp32(&ONENAND(uController)->rIntPinEnable, 1);
	}
#endif
}

//////////
// Function Name : ONENAND_EnableAllInterrupt
// Function Description : Interrupt Status All Enable
// Input : 	Controller - OneNand Controller Port Number 
// Version : v0.1
void ONENAND_EnableAllInterrupt(u32 uController)
{
	ONENAND_EnableIOBE(uController, 1);
	Outp32(&ONENAND(uController)->rIntErrMask, (u32)(0x3FFF));
	ONENAND_EnableIntPin(uController, 1);
}

//////////
// Function Name : ONENAND_DisableInterrupt
// Function Description : Interrupt Status Disable
// Input : 	Controller - OneNand Controller Port Number 
//			eInterrupt - Interrupt Source
// Version : v0.1
void ONENAND_DisableInterrupt(u32 uController, OneNAND_eINTERROR eInterrupt)
{
	u32 uDisableBit;

	uDisableBit = Inp32(&ONENAND(uController)->rIntErrMask);
	
	Outp32(&ONENAND(uController)->rIntErrMask, (u32)(uDisableBit & (~eInterrupt)));

//	rb1004..... 2007.09.28
//	if(eInterrupt == eOND_INTACT)
//		ONENAND_EnableIntPin(uController, 0);
}

//////////
// Function Name : ONENAND_DisableAllInterrupt
// Function Description : Interrupt Status All Disable
// Input : 	Controller - OneNand Controller Port Number 
// Version : v0.1
void ONENAND_DisableAllInterrupt(u32 uController)
{
	Outp32(&ONENAND(uController)->rIntErrMask, (u32)(0));

//	rb1004..... 2007.09.28
//	Outp32(&ONENAND(uController)->rIntPinEnable, 0);
}

//////////
// Function Name : ONENAND_AckInterrupt
// Function Description : Interrupt Status Clear(Ack Interrupt)
// Input : 	Controller - OneNand Controller Port Number 
//			eInterrupt - Interrupt Source
// Version : v0.1
void ONENAND_AckInterrupt(u32 uController, OneNAND_eINTERROR eInterrupt)
{
	Outp32(&ONENAND(uController)->rIntErrAck, (u32)eInterrupt);

	//while((Inp32(&ONENAND(uController)->rIntErrStat) & eInterrupt));	// Wait Interrupt Status Clear
}

//////////
// Function Name : ONENAND_ISR
// Function Description : OneNand IRQ routine
// Input : 	None
// Version : v0.1
void __irq ONENAND_ISR(void)
{
	u32 uController;
	u32 uTemp;

	uTemp = INTC_GetVicIrqStatus(1);
	uTemp = (uTemp&(3<<11))>>11; 
	if(uTemp & 1)
	{
		INTC_Disable(NUM_ONENAND0);
		uController = 0;
	}
	else if(uTemp & 2)
	{
		INTC_Disable(NUM_ONENAND1);
		uController = 1;
	}
	else
	{
		return;
	}

	if(OneNand_ISRCallBack.ONENAND_ISR_Func)
		OneNand_ISRCallBack.ONENAND_ISR_Func(uController);	

	if(uController == 0)
	{
		INTC_Enable(NUM_ONENAND0);
	}
	else if(uController == 1)
	{
		INTC_Enable(NUM_ONENAND1);
	}	
	
	INTC_ClearVectAddr();
}


#if (ONENAND_INTMODE == ONENAND_INTERRUPT)
//////////
// Function Name : ONENAND_EraseBlockDirectAccess
// Function Description : Block Erase using Direct Access
// Input : 	uController - OneNand Controller Port Number 
//			uBlock - Block to erase
//Output : 	TRUE - Erase Error
//			FALSE - Erase Success
// Version : v0.1
u8 ONENAND_EraseBlockDirectAccess(u32 uController, u32 uBlock)
{
	u32 uStatus=0;

	OneNandT_oIntFlag.IntActInt = 0;
	OneNandT_oIntFlag.ErsFailInt = 0;
	OneNandT_oIntFlag.ErsCmpInt = 0;
	
	if((OneNand_Inform[uController].uTotalBlockWidth == 10) || (OneNand_Inform[uController].uTotalBlockWidth == 11))		// 1Gb Single(1024Block), 2Gb DDP(1024*2Block)
	{
		ONENAND_DirectWrite(uController, OND_STARTADDR1, (DFS(uBlock) | FBA(uBlock)));
		ONENAND_DirectWrite(uController, OND_STARTADDR2, DBS(uBlock));
	}
	else if(OneNand_Inform[uController].uTotalBlockWidth == 12)		// 2Gb Single(2048Block), 4Gb DDP(2048*2Block)
	{
		ONENAND_DirectWrite(uController, OND_STARTADDR1, (DFS2(uBlock) | FBA2(uBlock)));
		ONENAND_DirectWrite(uController, OND_STARTADDR2, DBS2(uBlock));
	}
	else
	{
		// Not implemented, 128Mb, 256Mb, 512Mb
		Assert(0);
	}
	
	ONENAND_DirectWrite(uController, OND_INTSTATUS, 0);
	ONENAND_DirectWrite(uController, OND_COMMAND, OND_BLK_ERASE);

	while(!OneNandT_oIntFlag.IntActInt);

	//Map11 problem
	//while(!OneNandT_oIntFlag.ErsCmpInt);		//In case of DDP, Interrupt is not occured because of the wrong DBS bit

	if((OneNand_Inform[uController].uTotalBlockWidth == 10) || (OneNand_Inform[uController].uTotalBlockWidth == 11))		// 1Gb Single(1024Block), 2Gb DDP(1024*2Block)
		ONENAND_DirectWrite(uController, OND_STARTADDR2, DBS(uBlock));
	else if(OneNand_Inform[uController].uTotalBlockWidth == 12)		// 2Gb Single(2048Block), 4Gb DDP(2048*2Block)
		ONENAND_DirectWrite(uController, OND_STARTADDR2, DBS2(uBlock));

	while(!((uStatus&0xFFFF) & OND_ERASEINT))
		ONENAND_DirectRead(uController, OND_INTSTATUS, &uStatus);

	uStatus = 0;
	ONENAND_DirectRead(uController, OND_STATUS, &uStatus);
	if((uStatus&0xFFFF) & (1<<10))
		return TRUE;
	return FALSE;
}


//////////
// Function Name : ONENAND_EraseMultiBlockDirectAccess
// Function Description : Multi-Block Erase using Direct Access
// Input : 	uController - OneNand Controller Port Number 
//			uStartBlock - Start Block to erase
//			uEndBlock - End Block to erase
//Output : 	TRUE - Erase Error
//			FALSE - Erase Success
// Version : v0.1
u8 ONENAND_EraseMultiBlockDirectAccess(u32 uController, u32 uStartBlock, u32 uEndBlock)
{
	u32 i, uStatus;

	Assert((uEndBlock - uStartBlock)<=64);

/////////////////////////////////////////////
	OneNandT_oIntFlag.IntActInt = 0;
	OneNandT_oIntFlag.ErsFailInt = 0;
	OneNandT_oIntFlag.ErsCmpInt = 0;
////////////////////////////////////////////////

	for(i=uStartBlock ; i<uEndBlock ; i++)
	{
		OneNandT_oIntFlag.IntActInt = 0;

		if((OneNand_Inform[uController].uTotalBlockWidth == 10) || (OneNand_Inform[uController].uTotalBlockWidth == 11))		// 1Gb Single(1024Block), 2Gb DDP(1024*2Block)
		{
			ONENAND_DirectWrite(uController, OND_STARTADDR1, (DFS(i) | FBA(i)));
			ONENAND_DirectWrite(uController, OND_STARTADDR2, DBS(i));
		}
		else if(OneNand_Inform[uController].uTotalBlockWidth == 12)		// 2Gb Single(2048Block), 4Gb DDP(2048*2Block)
		{
			ONENAND_DirectWrite(uController, OND_STARTADDR1, (DFS2(i) | FBA2(i)));
			ONENAND_DirectWrite(uController, OND_STARTADDR2, DBS2(i));
		}
		else
		{
			// Not implemented, 128Mb, 256Mb, 512Mb
			Assert(0);
		}

		ONENAND_DirectWrite(uController, OND_INTSTATUS, 0);
		ONENAND_DirectWrite(uController, OND_COMMAND, OND_MULTY_ERASE);

		while(!OneNandT_oIntFlag.IntActInt);
	}

	if((OneNand_Inform[uController].uTotalBlockWidth == 10) || (OneNand_Inform[uController].uTotalBlockWidth == 11))		// 1Gb Single(1024Block), 2Gb DDP(1024*2Block)
	{
		ONENAND_DirectWrite(uController, OND_STARTADDR1, (DFS(uEndBlock) | FBA(uEndBlock)));
	}
	else if(OneNand_Inform[uController].uTotalBlockWidth == 12)		// 2Gb Single(2048Block), 4Gb DDP(2048*2Block)
	{
		ONENAND_DirectWrite(uController, OND_STARTADDR1, (DFS2(uEndBlock) | FBA2(uEndBlock)));
	}
	else
	{
		// Not implemented, 128Mb, 256Mb, 512Mb
		Assert(0);
	}

	ONENAND_DirectWrite(uController, OND_INTSTATUS, 0);
	ONENAND_DirectWrite(uController, OND_COMMAND, OND_BLK_ERASE);

	while(!OneNandT_oIntFlag.IntActInt);

	//Erase Verify
	for(i=uStartBlock ; i<=uEndBlock ; i++)
	{
		OneNandT_oIntFlag.IntActInt = 0;

		if((OneNand_Inform[uController].uTotalBlockWidth == 10) || (OneNand_Inform[uController].uTotalBlockWidth == 11))		// 1Gb Single(1024Block), 2Gb DDP(1024*2Block)
		{
			ONENAND_DirectWrite(uController, OND_STARTADDR1, (DFS(i) | FBA(i)));
		}
		else if(OneNand_Inform[uController].uTotalBlockWidth == 12)		// 2Gb Single(2048Block), 4Gb DDP(2048*2Block)
		{
			ONENAND_DirectWrite(uController, OND_STARTADDR1, (DFS2(i) | FBA2(i)));
		}
		else
		{
			// Not implemented, 128Mb, 256Mb, 512Mb
			Assert(0);
		}

		ONENAND_DirectWrite(uController, OND_INTSTATUS, 0);
		ONENAND_DirectWrite(uController, OND_COMMAND, OND_ERASE_VERIFY);

		while(!OneNandT_oIntFlag.IntActInt);

		uStatus = 0;
		ONENAND_DirectRead(uController, OND_STATUS, &uStatus);
		if((uStatus&0xFFFF) & (1<<10))
		{
			UART_Printf("Erase Verify(Direct Access) Error...!!\n");
			UART_Getc();
			return TRUE;
		}
	}

	return FALSE;
}


//////////
// Function Name : ONENAND_WritePageDirectAccess
// Function Description : Page Write using Direct Access
// Input : 	uController - OneNand Controller Port Number 
//			uBlock - Block to write
//			uPage - Page to write
//Output : 	TRUE - Erase Error
//			FALSE - Erase Success
// Version : v0.1
u8 ONENAND_WritePageDirectAccess(u32 uController, u32 uBlock, u32 uPage)
{
	u32 uStatus=0;

	OneNandT_oIntFlag.IntActInt = 0;
	OneNandT_oIntFlag.BlkRwCmpInt = 0;
	OneNandT_oIntFlag.PgmCmpInt = 0;

	if((OneNand_Inform[uController].uTotalBlockWidth == 10) || (OneNand_Inform[uController].uTotalBlockWidth == 11))		// 1Gb Single(1024Block), 2Gb DDP(1024*2Block)
	{
		ONENAND_DirectWrite(uController, OND_STARTADDR1, (DFS(uBlock) | FBA(uBlock)));
		ONENAND_DirectWrite(uController, OND_STARTADDR2, DBS(uBlock));
	}
	else if(OneNand_Inform[uController].uTotalBlockWidth == 12)		// 2Gb Single(2048Block), 4Gb DDP(2048*2Block)
	{
		ONENAND_DirectWrite(uController, OND_STARTADDR1, (DFS2(uBlock) | FBA2(uBlock)));
		ONENAND_DirectWrite(uController, OND_STARTADDR2, DBS2(uBlock));
	}
	else
	{
		// Not implemented, 128Mb, 256Mb, 512Mb
		Assert(0);
	}

	ONENAND_DirectWrite(uController, OND_STARTADDR8, FPA(uPage) | FSA(0));
	ONENAND_DirectWrite(uController, OND_STARTBUF, BSA(0x8) | BSC(0));
	
	ONENAND_DirectWrite(uController, OND_INTSTATUS, 0);
	ONENAND_DirectWrite(uController, OND_COMMAND, OND_PG_MS_FR_BUF);
#if 0
	uStatus = 0;
	while(!((uStatus&0xFFFF) & (1<<15)))
	{
		ONENAND_DirectRead(uController, OND_INTSTATUS, &uStatus);
	}

	uStatus = 0;
	ONENAND_DirectRead(uController, OND_STATUS, &uStatus);
	if((uStatus&0xFFFF) & (1<<12))
		return TRUE;
	return FALSE;
#endif	
	while(!OneNandT_oIntFlag.IntActInt);

	//In case of DDP, Interrupt is not occured because of the wrong DBS bit
	//while(!OneNandT_oIntFlag.BlkRwCmpInt);
	//while(!OneNandT_oIntFlag.PgmCmpInt);
	if((OneNand_Inform[uController].uTotalBlockWidth == 10) || (OneNand_Inform[uController].uTotalBlockWidth == 11))		// 1Gb Single(1024Block), 2Gb DDP(1024*2Block)
		ONENAND_DirectWrite(uController, OND_STARTADDR2, DBS(uBlock));
	else if(OneNand_Inform[uController].uTotalBlockWidth == 12)		// 2Gb Single(2048Block), 4Gb DDP(2048*2Block)
		ONENAND_DirectWrite(uController, OND_STARTADDR2, DBS2(uBlock));
	
	while(!((uStatus&0xFFFF) & OND_WRITEINT))
		ONENAND_DirectRead(uController, OND_INTSTATUS, &uStatus);

	return FALSE;	
}


//////////
// Function Name : ONENAND_ReadPageDirectAccess
// Function Description : Page Read using Direct Access
// Input : 	uController - OneNand Controller Port Number 
//			uBlock - Block to read
//			uPage - Page to read
//Output : 	TRUE - Erase Error
//			FALSE - Erase Success
// Version : v0.1
u8 ONENAND_ReadPageDirectAccess(u32 uController, u32 uBlock, u32 uPage)
{
	u32 uStatus=0;

	OneNandT_oIntFlag.IntActInt = 0;
	OneNandT_oIntFlag.BlkRwCmpInt = 0;
	OneNandT_oIntFlag.LoadCmpInt = 0;

	if((OneNand_Inform[uController].uTotalBlockWidth == 10) || (OneNand_Inform[uController].uTotalBlockWidth == 11))		// 1Gb Single(1024Block), 2Gb DDP(1024*2Block)
	{
		ONENAND_DirectWrite(uController, OND_STARTADDR1, (DFS(uBlock) | FBA(uBlock)));
		ONENAND_DirectWrite(uController, OND_STARTADDR2, DBS(uBlock));
	}
	else if(OneNand_Inform[uController].uTotalBlockWidth == 12)		// 2Gb Single(2048Block), 4Gb DDP(2048*2Block)
	{
		ONENAND_DirectWrite(uController, OND_STARTADDR1, (DFS2(uBlock) | FBA2(uBlock)));
		ONENAND_DirectWrite(uController, OND_STARTADDR2, DBS2(uBlock));
	}
	else
	{
		// Not implemented, 128Mb, 256Mb, 512Mb
		Assert(0);
	}
	
	//ONENAND_DirectWrite(uController, OND_STARTADDR1, DFS(uBlock) | FBA(uBlock));
	ONENAND_DirectWrite(uController, OND_STARTADDR8, FPA(uPage) | FSA(0));
	ONENAND_DirectWrite(uController, OND_STARTBUF, BSA(0x8) | BSC(0));
	//ONENAND_DirectWrite(uController, OND_STARTADDR2, DBS(uBlock));
	
	ONENAND_DirectWrite(uController, OND_INTSTATUS, 0);
	ONENAND_DirectWrite(uController, OND_COMMAND, OND_LD_MS_TO_BUF);

#if 0
	uStatus = 0;
	while(!((uStatus&0xFFFF) & (1<<15)))
	{
		ONENAND_DirectRead(uController, OND_INTSTATUS, &uStatus);
	}

	uStatus = 0;
	ONENAND_DirectRead(uController, OND_STATUS, &uStatus);
	if((uStatus&0xFFFF) & (1<<13))
		return TRUE;
	return FALSE;
#endif
	while(!OneNandT_oIntFlag.IntActInt);

	//In case of DDP, Interrupt is not occured because of the wrong DBS bit	
	//while(!OneNandT_oIntFlag.BlkRwCmpInt);
	//while(!OneNandT_oIntFlag.LoadCmpInt);	

	if((OneNand_Inform[uController].uTotalBlockWidth == 10) || (OneNand_Inform[uController].uTotalBlockWidth == 11))		// 1Gb Single(1024Block), 2Gb DDP(1024*2Block)
		ONENAND_DirectWrite(uController, OND_STARTADDR2, DBS(uBlock));
	else if(OneNand_Inform[uController].uTotalBlockWidth == 12)		// 2Gb Single(2048Block), 4Gb DDP(2048*2Block)
		ONENAND_DirectWrite(uController, OND_STARTADDR2, DBS2(uBlock));
	
	while(!((uStatus&0xFFFF) & OND_READINT))
		ONENAND_DirectRead(uController, OND_INTSTATUS, &uStatus);
	
	return FALSE;
}


//////////
// Function Name : ONENAND_PipelineReadAheadDirectAcc
// Function Description : Pages Read using Cache read(Direct Access)
// Input : 	Controller - OneNand Controller Port Number 
//			uBlkAddr - Start Block Number to read
//			uPageAddr - Start Page number to read
//			uPageSize - Page number to read
//			pData - Destination Address
// Version : v0.1
void ONENAND_PipelineReadAheadDirectAcc(u32 uController, u32 uBlkAddr, u8 uPageAddr, u8 uPageSize, u32 *pData)
{
	u32 i, uStatus=0, uReadPageCnt=0;
	u32 uDataRamAddr, uBlock, uPage;
	u32 *pReadBuffer;

	Assert(uPageSize >= 4);
	
	uBlock = uBlkAddr;
	uPage = uPageAddr;
	pReadBuffer = pData;
	
	OneNandT_oIntFlag.IntActInt = 0;

	// First Page
	ONENAND_DirectWrite(uController, OND_STARTADDR3, uBlock);
	ONENAND_DirectWrite(uController, OND_STARTADDR4, uPage<<2);
	ONENAND_DirectWrite(uController, OND_STARTBUF, 0x0800);

	// Second Page	
	uPage++;
	uBlock = uBlkAddr+(uPage/OneNand_Inform[uController].uNumOfPage);
	uPage = uPage%OneNand_Inform[uController].uNumOfPage;
	
	ONENAND_DirectWrite(uController, OND_STARTADDR1, uBlock);
	if((OneNand_Inform[uController].uTotalBlockWidth == 10) || (OneNand_Inform[uController].uTotalBlockWidth == 11))		// 1Gb Single(1024Block), 2Gb DDP(1024*2Block)
		ONENAND_DirectWrite(uController, OND_STARTADDR2, DBS(uBlock));
	else if(OneNand_Inform[uController].uTotalBlockWidth == 12)		// 2Gb Single(2048Block), 4Gb DDP(2048*2Block)
		ONENAND_DirectWrite(uController, OND_STARTADDR2, DBS2(uBlock));
	ONENAND_DirectWrite(uController, OND_STARTADDR8, uPage<<2);
	
	ONENAND_DirectWrite(uController, OND_INTSTATUS, 0);
	ONENAND_DirectWrite(uController, OND_COMMAND, OND_CACHE_READ);

	while( !(((uStatus&0xffff) & (1<<15)) && ((uStatus&0xffff) & (1<<13))))
		ONENAND_DirectRead(uController, OND_STATUS, &uStatus);

	// Third Page
	uPage++;
	uBlock = uBlkAddr+(uPage/OneNand_Inform[uController].uNumOfPage);
	uPage = uPage%OneNand_Inform[uController].uNumOfPage;
	
	ONENAND_DirectWrite(uController, OND_STARTADDR1, uBlock);
	ONENAND_DirectWrite(uController, OND_STARTADDR8, uPage<<2);

	while(!OneNandT_oIntFlag.IntActInt);
	OneNandT_oIntFlag.IntActInt = 0;

	ONENAND_DirectRead(uController, OND_STATUS, &uStatus);
	if((uStatus&0xFFFF) & (1<<10))
	{
		UART_Printf("Cache Read Error...!!\n");
		UART_Getc();
		return;
	}

	ONENAND_DirectWrite(uController, OND_INTSTATUS, 0);

	for(i=3 ; i<uPageSize ; i++)
	{
		ONENAND_DirectWrite(uController, OND_COMMAND, OND_CACHE_READ);
////////////////////////////////////
		DelayfrTimer(micro, 15);
		
		if(uReadPageCnt%2)
			uDataRamAddr = ONENAND_DATARAM1_BASE;		// odd 
		else
			uDataRamAddr = ONENAND_DATARAM0_BASE;		// even
		
#if (OND_TRANS_MODE == OND_DMA)
		ONENAND_DirectRead_DMA(uController, uDataRamAddr, (u32 *)pReadBuffer);
		pReadBuffer += ONENAND_PAGESIZE/4;
#elif (OND_TRANS_MODE == OND_LDM_INST)
	{
		u32 uSrcAddr, uContBaseAddr;

		if(uController == 0)
			uContBaseAddr = ONENAND0_BASE;
		else
			uContBaseAddr = ONENAND1_BASE;
		
		uSrcAddr = OneNAND_BUFFER_BASE[uController]  + uDataRamAddr;
		OneNand8burstPageReadDir(uContBaseAddr, uSrcAddr, (u32)pReadBuffer);
		pReadBuffer += ONENAND_PAGESIZE/4;
	}		
#else
		{
			u32 k;
		
			for(k=0; k<ONENAND_PAGESIZE/4; k++)
			{
				ONENAND_ReadOutDataram(uController, uDataRamAddr+(4*k),pReadBuffer);
				pReadBuffer++;
			}
		}
#endif
		uReadPageCnt++;

		uPage++;
		uBlock = uBlkAddr+(uPage/OneNand_Inform[uController].uNumOfPage);
		uPage = uPage%OneNand_Inform[uController].uNumOfPage;
	
		ONENAND_DirectWrite(uController, OND_STARTADDR1, uBlock);
		ONENAND_DirectWrite(uController, OND_STARTADDR8, uPage<<2);	

		while(!OneNandT_oIntFlag.IntActInt);
		OneNandT_oIntFlag.IntActInt = 0;

		ONENAND_DirectRead(uController, OND_STATUS, &uStatus);
		if((uStatus&0xFFFF) & (1<<10))
		{
			UART_Printf("Cache Read Error...!!\n");
			UART_Getc();
			return;
		}
		ONENAND_DirectWrite(uController, OND_INTSTATUS, 0);
	}		

	ONENAND_DirectWrite(uController, OND_COMMAND, OND_CACHE_READ);
///////////////////////////////////////
	DelayfrTimer(micro, 15);
	
	if(uReadPageCnt%2)
		uDataRamAddr = ONENAND_DATARAM1_BASE;		
	else
		uDataRamAddr = ONENAND_DATARAM0_BASE;		
			
#if (OND_TRANS_MODE == OND_DMA)
	ONENAND_DirectRead_DMA(uController, uDataRamAddr, (u32 *)pReadBuffer);
	pReadBuffer += ONENAND_PAGESIZE/4;
#elif (OND_TRANS_MODE == OND_LDM_INST)
	{
		u32 uSrcAddr, uContBaseAddr;

		if(uController == 0)
			uContBaseAddr = ONENAND0_BASE;
		else
			uContBaseAddr = ONENAND1_BASE;
		
		uSrcAddr = OneNAND_BUFFER_BASE[uController]  + uDataRamAddr;
		OneNand8burstPageReadDir(uContBaseAddr, uSrcAddr, (u32)pReadBuffer);
		pReadBuffer += ONENAND_PAGESIZE/4;
	}
#else
	{
		u32 k;
	
		for(k=0; k<ONENAND_PAGESIZE/4; k++)
		{
			ONENAND_ReadOutDataram(uController, uDataRamAddr+(4*k),pReadBuffer);
			pReadBuffer++;
		}
	}
#endif	
	uReadPageCnt++;

	while(!OneNandT_oIntFlag.IntActInt);
	OneNandT_oIntFlag.IntActInt = 0;

	ONENAND_DirectRead(uController, OND_STATUS, &uStatus);
	if((uStatus&0xFFFF) & (1<<10))
	{
		UART_Printf("Cache Read Error...!!\n");
		UART_Getc();
		return;
	}
	ONENAND_DirectWrite(uController, OND_INTSTATUS, 0);

	ONENAND_DirectWrite(uController, OND_COMMAND, OND_FINISH_CACHE_READ);
///////////////////////////////////////
	DelayfrTimer(micro, 15);

	if(uReadPageCnt%2)
		uDataRamAddr = ONENAND_DATARAM1_BASE;		
	else
		uDataRamAddr = ONENAND_DATARAM0_BASE;		
			
#if (OND_TRANS_MODE == OND_DMA)
	ONENAND_DirectRead_DMA(uController, uDataRamAddr, (u32 *)pReadBuffer);
	pReadBuffer += ONENAND_PAGESIZE/4;
#elif (OND_TRANS_MODE == OND_LDM_INST)
	{
		u32 uSrcAddr, uContBaseAddr;

		if(uController == 0)
			uContBaseAddr = ONENAND0_BASE;
		else
			uContBaseAddr = ONENAND1_BASE;
		
		uSrcAddr = OneNAND_BUFFER_BASE[uController]  + uDataRamAddr;
		OneNand8burstPageReadDir(uContBaseAddr, uSrcAddr, (u32)pReadBuffer);
		pReadBuffer += ONENAND_PAGESIZE/4;
	}	
#else
	{
		u32 k;
	
		for(k=0; k<ONENAND_PAGESIZE/4; k++)
		{
			ONENAND_ReadOutDataram(uController, uDataRamAddr+(4*k),pReadBuffer);
			pReadBuffer++;
		}
	}
#endif	
	uReadPageCnt++;

	while(!OneNandT_oIntFlag.IntActInt);
	OneNandT_oIntFlag.IntActInt = 0;

	ONENAND_DirectRead(uController, OND_STATUS, &uStatus);
	if((uStatus&0xFFFF) & (1<<10))
	{
		UART_Printf("Cache Read Error...!!\n");
		UART_Getc();
		return;
	}

	if(uReadPageCnt%2)
		uDataRamAddr = ONENAND_DATARAM1_BASE;		
	else
		uDataRamAddr = ONENAND_DATARAM0_BASE;		
			
#if (OND_TRANS_MODE == OND_DMA)
	ONENAND_DirectRead_DMA(uController, uDataRamAddr, (u32 *)pReadBuffer);
	pReadBuffer += ONENAND_PAGESIZE/4;
#elif (OND_TRANS_MODE == OND_LDM_INST)
	{
		u32 uSrcAddr, uContBaseAddr;

		if(uController == 0)
			uContBaseAddr = ONENAND0_BASE;
		else
			uContBaseAddr = ONENAND1_BASE;
		
		uSrcAddr = OneNAND_BUFFER_BASE[uController]  + uDataRamAddr;
		OneNand8burstPageReadDir(uContBaseAddr, uSrcAddr, (u32)pReadBuffer);
		pReadBuffer += ONENAND_PAGESIZE/4;
	}	
#else
	{
		u32 k;
	
		for(k=0; k<ONENAND_PAGESIZE/4; k++)
		{
			ONENAND_ReadOutDataram(uController, uDataRamAddr+(4*k),pReadBuffer);
			pReadBuffer++;
		}
	}
#endif	
	uReadPageCnt++;

	if(uReadPageCnt != uPageSize)
	{
		UART_Printf("Cache Read Page Count Error...!!\n");
		UART_Getc();
		return;
	}
}

#endif

