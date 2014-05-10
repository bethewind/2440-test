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
*	File Name : system.c
*  
*	File Description : This file implements exceptions and MMU/Cache control.
*
*	Author : Haksoo,Kim
*	Dept. : AP Development Team
*	Created Date : 2006/11/08
*	Version : 0.1 
* 
*	History
*	- Created(Haksoo,Kim 2006/11/08)
*  
**************************************************************************************/

#include <stdio.h>

#include "option.h"
#include "library.h"
#include "sfr6410.h"
#include "system.h"
#include "intc.h"

////
// Exception
//
#define pExceptionRESET			(*(u32 *)(_Exception_Vector + 0x0))
#define pExceptionUNDEF			(*(u32 *)(_Exception_Vector + 0x4))
#define pExceptionSWI			(*(u32 *)(_Exception_Vector + 0x8))
#define pExceptionPABORT		(*(u32 *)(_Exception_Vector + 0xc))
#define pExceptionDABORT		(*(u32 *)(_Exception_Vector + 0x10))
#define pExceptionRESERVED		(*(u32 *)(_Exception_Vector + 0x14))
#define pExceptionIRQ			(*(u32 *)(_Exception_Vector + 0x18))
#define pExceptionFIQ			(*(u32 *)(_Exception_Vector + 0x1c))


////
// MMU/Cache
//
#define DESC_SEC		(2<<0)

#define CB				(3<<2)				//cache_on, write_back
#define CNB				(2<<2)				//cache_on, write_through 
#define NCB				(1<<2)				//cache_off,WR_BUF on
#define NCNB			(0<<2)				//cache_off,WR_BUF off
#define AP_RW			(3<<10)				//supervisor=RW, user=RW
#define AP_RO			(2<<10)				//supervisor=RW, user=RO
#define AP_NO			(1<<10)				//supervisor=RW, user=No access

#define DOMAIN_FAULT	(0x0)
#define DOMAIN_CHK		(0x1) 
#define DOMAIN_NOTCHK	(0x3) 
#define DOMAIN0			(0x0<<5)
#define DOMAIN1			(0x1<<5)

#define DOMAIN0_ATTR	(DOMAIN_CHK<<0) 
#define DOMAIN1_ATTR	(DOMAIN_FAULT<<2) 

#define RW_CB			(AP_RW|DOMAIN0|CB|DESC_SEC)
#define RW_CNB			(AP_RW|DOMAIN0|CNB|DESC_SEC)
#define RW_NCB			(AP_RW|DOMAIN0|NCB|DESC_SEC)
#define RW_NCNB			(AP_RW|DOMAIN0|NCNB|DESC_SEC)
#define RW_FAULT		(AP_RW|DOMAIN1|NCNB|DESC_SEC)
#define RW_NSNCNB		((0<<19)|AP_RW|DOMAIN0|NCNB|DESC_SEC)

//////////
// Function Name : ExceptionUNDEF
// Function Description : 
// Input : NONE
// Output : NONE
// Version : 
void ExceptionUNDEF(void)
{
	UART_Printf("Undefined instruction exception.\n");
	while(1);
}

//////////
// Function Name : ExceptionSWI
// Function Description : 
// Input : NONE
// Output : NONE
// Version : 
void ExceptionSWI(void)
{
	UART_Printf("SWI exception.\n");
	while(1);
}

//////////
// Function Name : ExceptionPABORT
// Function Description : 
// Input : NONE
// Output : NONE
// Version : 
void ExceptionPABORT(void)
{
	UART_Printf("IFSR=0x%x\n",SYSTEM_ReadIFSR());
	UART_Printf("FAR=0x%x\n",SYSTEM_ReadFAR());
	UART_Printf("Pabort exception.\n");
	while(1);
}

//////////
// Function Name : ExceptionDABORT
// Function Description : 
// Input : NONE
// Output : NONE
// Version : 
void ExceptionDABORT(void)
{
	UART_Printf("DFSR=0x%x\n",SYSTEM_ReadDFSR());
	UART_Printf("FAR=0x%x\n",SYSTEM_ReadFAR());
	UART_Printf("Dabort exception.\n");
	while(1);
}

void (*IntHandlerTable[INT_LIMIT])(void);
//////////
// Function Name : ExceptionIRQ
// Function Description : 
// Input : NONE
// Output : NONE
// Version : 
void ExceptionIRQ(void)
{
	u32 intNum;

	intNum=INTC_ReadIntSrc();
	IntHandlerTable[intNum]();

	return;
}

//////////
// Function Name : ExceptionFIQ
// Function Description : 
// Input : NONE
// Output : NONE
// Version : 
void ExceptionFIQ(void)
{
	UART_Printf("FIQ exception.\n");
	while(1);
}

//////////
// Function Name : SYSTEM_InitException
// Function Description : 
// Input : NONE
// Output : NONE
// Version
#if	(VIC_MODE == 0)
extern void IsrIRQ(void);
#endif
void SYSTEM_InitException( void)
{
	pExceptionUNDEF		=	(u32)ExceptionUNDEF;
	pExceptionSWI  		=	(u32)ExceptionSWI;
	pExceptionPABORT	=	(u32)ExceptionPABORT;
	pExceptionDABORT	=	(u32)ExceptionDABORT;
#if (VIC_MODE == 0)	
	pExceptionIRQ		=	(u32)IsrIRQ;
#else
	pExceptionIRQ		=	(u32)ExceptionIRQ;
#endif		
	pExceptionFIQ		=	(u32)ExceptionFIQ;

	INTC_Init();

#if (VIC_MODE == 0)
	SYSTEM_DisableVIC();
#else
	SYSTEM_EnableVIC();
#endif
	SYSTEM_EnableIRQ();
	SYSTEM_EnableFIQ();
}

//////////
// Function Name : SYSTEM_SetMmuMtt
// Function Description : 
// Input : NONE
// Output : NONE
// Version : 
void SYSTEM_SetMmuMtt(u32 vaddrStart,u32 vaddrEnd,u32 paddrStart,u32 attr)
{
    u32 *pTT;
    int i,nSec;

    pTT		=	(u32 *)_MMUTT_BaseAddress+(vaddrStart>>20);
    nSec	=	(vaddrEnd>>20)-(vaddrStart>>20);
    
    for(i=0;i<=nSec;i++)
    	*pTT++	=	attr | (((paddrStart>>20)+i)<<20);
}

//////////
// Function Name : SYSTEM_InitMmu
// Function Description : 
// Input : NONE
// Output : NONE
// Version : 
void SYSTEM_InitMmu(void)
{

	//========================== IMPORTANT NOTE =========================
	//The current stack and code area can't be re-mapped in this routine.
	//If you want memory map mapped freely, your own sophiscated MMU
	//initialization code is needed.
	//===================================================================

	SYSTEM_DisableICache();
	SYSTEM_DisableDCache();

	SYSTEM_InvalidateEntireICache();
	SYSTEM_CleanInvalidateEntireDCache();

	SYSTEM_DisableMMU();
	SYSTEM_InvalidateTLB();

	//	SROM
	if( (SYSC_GetMEMCFGSTAT()&(0x3<<5)) != (2<<5) )
	{
		SYSTEM_SetMmuMtt(0x00000000,0x07f00000,0x00000000,RW_NCNB);	//mirroed region
		SYSTEM_SetMmuMtt(0x08000000,0x0bf00000,0x08000000,RW_NCNB);	//internal rom
		SYSTEM_SetMmuMtt(0x0c000000,0x0ff00000,0x0c000000,RW_NCNB);	//stepping stone
	}
	else
	{
		//Only for OneNand Booting Mode..Map00 error
		SYSTEM_SetMmuMtt(0x0c000000,0x0ff00000,0x00000000,RW_NCNB);	//mirroed region
		SYSTEM_SetMmuMtt(0x04000000,0x07f00000,0x04000000,RW_NCNB);
		SYSTEM_SetMmuMtt(0x08000000,0x0bf00000,0x08000000,RW_NCNB);	//internal rom
		SYSTEM_SetMmuMtt(0x00000000,0x03f00000,0x0c000000,RW_NCNB);	//stepping stone
	}
	SYSTEM_SetMmuMtt(0x10000000,0x17f00000,0x10000000,RW_NCNB);	//nCS0
	SYSTEM_SetMmuMtt(0x18000000,0x1ff00000,0x18000000,RW_NCNB);	//nCS1
	SYSTEM_SetMmuMtt(0x20000000,0x27f00000,0x20000000,RW_NCNB);	//nCS2
	SYSTEM_SetMmuMtt(0x28000000,0x2ff00000,0x28000000,RW_NCNB);	//nCS3
	SYSTEM_SetMmuMtt(0x30000000,0x37f00000,0x30000000,RW_NCNB);	//nCS4
	SYSTEM_SetMmuMtt(0x38000000,0x3ff00000,0x38000000,RW_NCNB);	//nCS5

	//	SDRAM (MEM Port0)

	SYSTEM_SetMmuMtt(_DRAM0_BaseAddress+0x00000000,_DRAM0_BaseAddress+0x03ffffff,_DRAM0_BaseAddress+0x00000000,RW_NCNB);
	
	//	SDRAM (MEM Port1)
	SYSTEM_SetMmuMtt(_DRAM_BaseAddress+0x00000000,_DRAM_BaseAddress+0x00f00000,_DRAM_BaseAddress+0x00000000,RW_CB);
	SYSTEM_SetMmuMtt(_DRAM_BaseAddress+0x01000000,_DRAM_BaseAddress+0x07e00000,_DRAM_BaseAddress+0x01000000,RW_NCNB);
	SYSTEM_SetMmuMtt(_DRAM_BaseAddress+0x07f00000,_DRAM_BaseAddress+0x07f00000,_DRAM_BaseAddress+0x07f00000,RW_CB);

	//	SFR & Etc
	SYSTEM_SetMmuMtt(0x70000000,0x7ff00000,0x70000000,RW_NCNB);  //SFR

	//TCM
	SYSTEM_SetMmuMtt(0x80000000,0x80f00000,0x80000000,RW_NCNB);  
	
	SYSTEM_SetTTBase(_MMUTT_BaseAddress);

	//DOMAIN1: no_access, DOMAIN0,2~15=client(AP is checked)
	SYSTEM_SetDomain(0x55555550|DOMAIN1_ATTR|DOMAIN0_ATTR); 

	SYSTEM_SetFCSEPID(0x0);
	SYSTEM_EnableAlignFault();

	SYSTEM_EnableMMU();
	SYSTEM_EnableICache();
	SYSTEM_EnableDCache(); //DCache should be turned on after MMU is turned on.
	SYSTEM_EnableBP();

}
