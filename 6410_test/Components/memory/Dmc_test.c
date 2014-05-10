/**************************************************************************************
* 
*	Project Name : S3C6410 Validation
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
*	File Name : DMC_test.c
*  
*	File Description : This file implements the functons for DRAM controller test.
*
*	Author : Wonjoon Jang
*	Dept. : AP Development Team
*	Created Date : 2007/02/10
*	Version : 0.1 
* 
*	History
*	- Created(wonjoon.jang 2007/02/10)
*       - Revised for S3C6410(Sunil.Roe 2008/02/28)
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
#include "sysc.h"
#include "dma.h"
#include "intc.h"
#include "timer.h"
#include "rtc.h"
#include "system.h"

#include "dmc.h"

extern void SYSC_CLKInform(void);


static DMAC oDmact0;
static DMAC oDmact1;
extern volatile int g_DmaDone, g_DmaDone1;
volatile int g_RTC_ALM1;
volatile int g_DMCT_Err;

#define DMA_ADD (true)


void DMC_Test(void);


// Temp. function
void MemoryDump0( u32 *DumpAddress, u32 line)
{
	u32 i,j;
	
	for(i=0;i<line;i++) {
		UART_Printf("%08x : ",(u32)DumpAddress);
		for(j=0;j<8;j++) {
			UART_Printf("%08x ",*DumpAddress++);
		}
		putchar('\n');
	}
	putchar('\n');
}


void Mem_dump0 (void)
{

	int ch;
	
	
	 while(1)
    {
    	UART_Printf("dump address, '-1' to Exit \n");
    	//ch = Uart_getc();
    	ch = UART_GetIntNum();
    		if(ch==-1)
    	{
    		break;
    	}
    	else
    	{
    	    	
    	MemoryDump0( ((u32 *)(ch)), 40);
    	}

    }

}






//////////
// Function Name : DMCtoDMC_Test
// Function Description : DMC0 Init & Access test between DMC0 with DMC1 
// Input : 	None
// Output :	None 
// Version : v0.1
void	DMCtoDMC_Test(eFunction_Test eTest, oFunctionT_AutoVar oAutoVar)
//static void DMCtoDMC_Test(void)
{
	u32 i=0;
	u32 check_size, uDMC1_TestAddr, uDMC0_TestAddr;
	u32 uerror = 0;
	

    check_size			= 0x100000;
    uDMC1_TestAddr	= _DRAM_BaseAddress + 0x1000000;
    uDMC0_TestAddr   	= _DRAM0_BaseAddress+ 0x1000000;	
	
  ////////////////  DMC0 Initialize ////////////////////	
   DMC_Init(0, eALLEN,   eDIS_AutoPD , 0 );

   if(eTest == eTest_Manual)
	{
	 	UART_Printf("[DMCtoDMC Test]\n");
		g_DMCT_Err =0;
	}
   	
    ///////////////   DMC1 to DMC0 Copy Test /////////////////
     
   for(i=uDMC1_TestAddr;i<(uDMC1_TestAddr+check_size);i+=4)
    	*((unsigned int *)i)=i^0x55555555;
    	//*((unsigned int *)i)=0x55555555;
    
      

  Copy(uDMC1_TestAddr, uDMC0_TestAddr,check_size );
           

 	
    for(i=0;i<(0+check_size);i+=4)
    {
    	if(*((unsigned int *)(uDMC0_TestAddr+i))!=((uDMC1_TestAddr+i)^0x55555555))
    //	if(*((unsigned int *)i)!=(0x55555555))
    	{
    	    UART_Printf("Mem Error:0x%x=0x%x(%x)\n",(uDMC0_TestAddr+i),*((unsigned int *)(uDMC0_TestAddr+i)),(uDMC1_TestAddr+i)^0x55555555);
    	    uerror++;
    	}
    	if(uerror>20)
	    break;
    }
    
    if(uerror)
    	{
	    	UART_Printf("		DMC1 to DMC0 Copy test:FAILED\n"); 
	    	g_DMCT_Err =1;
    	}
    else 
    	UART_Printf("			DMC1 to DMC0 Copy test:O.K.\n");	

  ///////////////   DMC0 to DMC1 Copy Test /////////////////
  uerror =0;
  
  for(i=uDMC0_TestAddr;i<(uDMC0_TestAddr+check_size);i+=4)
    	*((unsigned int *)i)=i^0x55555555;
 
  Copy(uDMC0_TestAddr, uDMC1_TestAddr,check_size );	

    for(i=0;i<(0+check_size);i+=4)
    {
    	if(*((unsigned int *)(uDMC1_TestAddr+i))!=((uDMC0_TestAddr+i)^0x55555555))
    //	if(*((unsigned int *)i)!=(0x55555555))
    	{
    	    UART_Printf("Mem Error:0x%x=0x%x(%x)\n",(uDMC1_TestAddr+i),*((unsigned int *)(uDMC1_TestAddr+i)),(uDMC0_TestAddr+i)^0x55555555);
    	    uerror++;
    	}
    	if(uerror>20)
	    break;
    }
    if(uerror)
    	{
    	UART_Printf("			DMC0 to DMC1 Copy test:FAILED\n"); 
     	g_DMCT_Err =1;
    	}
    else 
    	UART_Printf("			DMC0 to DMC1 Copy test:O.K.\n");


}


//////////
// Function Name : DMCtoTCM_Test
// Function Description : Access test between DMC with TCM 
// Input : 	None
// Output :	None 
// Version : v0.1
void	DMCtoTCM_Test(eFunction_Test eTest, oFunctionT_AutoVar oAutoVar)
//static void DMCtoTCM_Test(void)
{
	u32 i=0;
	u32 check_size, uDMC1_TestAddr, uDMC0_TestAddr, uTCM_TestAddr;
	u32 uerror = 0;

    check_size			= 0x2000;
    uDMC1_TestAddr	= _DRAM_BaseAddress + 0x1000000;
    uDMC0_TestAddr   	= _DRAM0_BaseAddress+ 0x1000000;	
    uTCM_TestAddr 		= 0x80000000;	
	
   DMC_Init(0, eALLEN,   eDIS_AutoPD , 0 );
   if(eTest == eTest_Manual)
	{
	 	UART_Printf("[DMCtoTCM Test]\n");
		g_DMCT_Err =0;
	}
   	
   	
    ///////////////   DMC1 to TCM Copy Test /////////////////
     
   for(i=uDMC1_TestAddr;i<(uDMC1_TestAddr+check_size);i+=4)
    	*((unsigned int *)i)=i^0x55555555;
    	//*((unsigned int *)i)=0x55555555;
    
      

  Copy(uDMC1_TestAddr, uTCM_TestAddr ,check_size );
           

 	
    for(i=0;i<(0+check_size);i+=4)
    {
    	if(*((unsigned int *)(uTCM_TestAddr +i))!=((uDMC1_TestAddr+i)^0x55555555))
    //	if(*((unsigned int *)i)!=(0x55555555))
    	{
    	    UART_Printf("Mem Error:0x%x=0x%x(%x)\n",(uTCM_TestAddr +i),*((unsigned int *)(uTCM_TestAddr +i)),(uDMC1_TestAddr+i)^0x55555555);
    	    uerror++;
    	}
    	if(uerror>20)
	    break;
    }
    if(uerror)
    	{
    	UART_Printf("			DMC1 to TCM Copy test:FAILED\n"); 
    	g_DMCT_Err =1;
    	}
    else 
    	UART_Printf("			DMC1 to TCM Copy test:O.K.\n");	

  ///////////////   DMC0 to TCM Copy Test /////////////////
  uerror =0;
  
  for(i=uDMC0_TestAddr;i<(uDMC0_TestAddr+check_size);i+=4)
    	*((unsigned int *)i)=i^0x55555555;
 
  Copy(uDMC0_TestAddr, uTCM_TestAddr ,check_size );	

    for(i=0;i<(0+check_size);i+=4)
    {
    	if(*((unsigned int *)(uTCM_TestAddr +i))!=((uDMC0_TestAddr+i)^0x55555555))
    //	if(*((unsigned int *)i)!=(0x55555555))
    	{
    	    UART_Printf("Mem Error:0x%x=0x%x(%x)\n",(uTCM_TestAddr +i),*((unsigned int *)(uTCM_TestAddr +i)),(uDMC0_TestAddr+i)^0x55555555);
    	    uerror++;
    	}
    	if(uerror>20)
	    break;
    }
    if(uerror)
    	{
    	UART_Printf("			DMC0 to TCM Copy test:FAILED\n"); 
    	g_DMCT_Err =1;
    	}
    else 
    	UART_Printf("			DMC0 to TCM Copy test:O.K.\n");

}



void __irq Isr_RTC_Tick2(void)
{
	//-daedoo
	//RTC_ClearIntP(0);
	RTC_ClearPending(TimeTic);
	g_RTC_ALM1=1;
	

	INTC_ClearVectAddr();
}

void __irq Dma0Donet(void)
{
    	//u32 uTime;
    	
  	DMACH_ClearIntPending(&oDmact0);
 	//rDMAC0IntTCClear = 0xff;
 	//uTime = StopTimer(0);
	//UART_Printf ("uTime %d\n", uTime);
	g_DmaDone=1;
	//g_DmaDone++;
	INTC_ClearVectAddr();
     
}

void __irq Dma0Donet1(void)
{
    	//u32 uTime;
    	
  	DMACH_ClearIntPending(&oDmact1);
 	//rDMAC0IntTCClear = 0xff;
 	//uTime = StopTimer(2);
	//UART_Printf ("uTime %d\n", uTime);
	g_DmaDone1=1;
	//g_DmaDone1++;
	INTC_ClearVectAddr();
       
}




///////////////////////////////////////////////////////////////////////////
/////////
/////////	DRAM Performance Test				
/////////
///////////////////////////////////////////////////////////////////////////

//////////
// Function Name : DRAM_Copy_Test_P1
// Function Description : MemoryPort1 Perf. Test
// Input : 	None
// Output :	None 
// Version : v0.1
void DRAM_Copy_Test_P1(void)
{
	u32 uMode,uTime, uBurst,i;

	DMC_Init(1, eALLEN,   eDIS_AutoPD , 0 );
	
	UART_Printf("Select Operating Mode: 0: retain 1: Sync-533/133,   2: Async-533/133 \n");
	uMode = UART_GetIntNum();
	

	switch(uMode)
		{
			case 0:
				SYSC_CLKInform();
				break;
		
			case 1:		// Sync Mode
			SYSC_ChangeSYSCLK_1(eAPLL532M, eAPLL266M, 0, 1, 3);
			SYSC_ChangeMode(eSYNC_MODE);
			SYSC_ChangeSYSCLK_1(eAPLL532M, eAPLL266M, 0, 1, 3);
			
			break;

			case 2:		//ASync Mode
			//SYSC_ChangeMode(eASYNC_MODE);
			SYSC_ChangeSYSCLK_1(eAPLL532M, eAPLL266M, 0, 0, 3);
			break;
		}

	UART_Printf("===== Timer Check =====n");
	//UART_Getc();
	g_RTC_ALM1=0;
	INTC_SetVectAddr(NUM_RTC_TIC,Isr_RTC_Tick2);
	INTC_Enable(NUM_RTC_TIC);
	
	//-daedoo	
	/*
	RTC_SetCON(0,0,0,0,0,1);
	RTC_SetTime(InitYEAR,InitMONTH,InitDATE,InitDAY,InitHOUR,InitMIN,InitSEC);
	RTC_SetTickCNT(33);	
	RTC_SetCON(0,0,0,0,0,0);							// Tick Clock = 128Hz?
	RTC_ClearIntP(0);	
	*/

	RTC_TimeInit(InitYEAR,InitMONTH,InitDATE,InitDAY,InitHOUR,InitMIN,InitSEC);
	RTC_TickCnt(33);	
	RTC_TickClkSelect(CLK_32768Hz);
	RTC_ClearPending(TimeTic);
		
	StartTimer(0);
	//RTC_SetCON(1,0,0,0,0,1);
	RTC_TickTimeEnable(true);
	RTC_Enable(true);
		
	while(g_RTC_ALM1==0);
	uTime=StopTimer(0);
	 UART_Printf(" > Timer Check Time: %d us \n", uTime);
	//RTC_SetCON(0,0,0,0,0,0);
	RTC_Enable(false);
	INTC_Disable(NUM_RTC_TIC);

	UART_Printf("===== Memory Initialization =====\n");
	//UART_Getc();
	StartTimer(0);

	  for(i=0x51000000;i<(0x51000000+0x100000);i+=4)
    		*((unsigned int *)i)=i^0x55555555;
	  
	 uTime = StopTimer(0);
	UART_Printf(" > Memory Init.  Time: %d us \n", uTime);
	
	UART_Printf("***** 1. Memory Copy using ASM  *****\n");
	 StartTimer(0);
	MEMCOPY_TEST();
	uTime = StopTimer(0);
	UART_Printf(" > Word Copy Time: %d us \n", uTime);
   
	UART_Printf("***** 2. Memory Copy using memcpy()  *****\n");
	StartTimer(0);   
	memcpy((int *)0x52000000, (int *)0x51000000, 0x100000);
	 uTime = StopTimer(0);
	 UART_Printf(" > Word Copy Time: %d us \n", uTime);


	UART_Printf("***** 3. Memory Copy using FOR loop *****\n");
	//UART_Getc();
	// Word Copy : 1MB
	 StartTimer(0);   
	 Copy(0x51000000, 0x52000000, 0x40000);	// 1M 
	 uTime = StopTimer(0);
	 UART_Printf(" > Word Copy Time: %d us \n", uTime);
   
	// HWord Copy : 1MB
	 StartTimer(0);
	 Copy16(0x52000000, 0x53000000, 0x80000);	// 1M 
	 uTime = StopTimer(0);
	 UART_Printf(" > HWord Copy Time: %d us \n", uTime);

	// BYTE Copy : 1MB
	 StartTimer(0);
	 Copy8(0x53000000, 0x54000000, 0x100000);	// 1M 
	 uTime = StopTimer(0);
	UART_Printf(" > Byte Copy Time: %d us \n", uTime);

	UART_Printf("***** 4. Memory Copy Using DMA *****\n");
	UART_Printf(" > Select Burst Size : 0:SINGLE, 1:BURST4, 2:BURST8, 3:BURST16, 4:BURST32, 5:BURST64, 6:BURST128, 7:BURST256	: ");
	uBurst=UART_GetIntNum();

     	DMAC_InitCh(DMA0, DMA_ALL, &oDmact0);
	INTC_SetVectAddr(NUM_DMA0,  Dma0Donet);
	INTC_Enable(NUM_DMA0);

	// Word Copy : 1MB
	UART_Printf(" > Word Copy Time: ");	
	DMACH_Setup(DMA_A, 0x0, 0x51000000, 0, 0x52000000, 0, WORD, 0x40000, DEMAND, MEM, MEM, (BURST_MODE)uBurst, &oDmact0);
        g_DmaDone=0;
       StartTimer(0);
	DMACH_Start(&oDmact0);	 // Enable DMA
	while(g_DmaDone==0);								// Int.
	uTime = StopTimer(0);
	UART_Printf("%d us \n", uTime);

	// HWord Copy : 1MB
	UART_Printf(" > HWord Copy Time: ");
	DMACH_Setup(DMA_A, 0x0, 0x51000000, 0, 0x52000000, 0, HWORD, 0x80000, DEMAND, MEM, MEM, (BURST_MODE)uBurst, &oDmact0);
        g_DmaDone=0;
       StartTimer(0);
	DMACH_Start(&oDmact0);		 // Enable DMA
	while(g_DmaDone==0);		
 	uTime = StopTimer(0);
	UART_Printf("%d us \n", uTime);
 
	// BYTE Copy : 1MB
	UART_Printf(" > BYTE Copy Time: ");
	DMACH_Setup(DMA_A, 0x0, 0x51000000, 0, 0x52000000, 0, BYTE, 0x100000, DEMAND, MEM, MEM, (BURST_MODE)uBurst, &oDmact0);
        g_DmaDone=0;
       StartTimer(0);
	DMACH_Start(&oDmact0);		// Enable DMA
	while(g_DmaDone==0);		  
 	uTime = StopTimer(0);
	UART_Printf("%d us \n\n", uTime);
	
}

//////////
// Function Name : DRAM_Copy_Test_P0
// Function Description : MemoryPort1 Perf. Test
// Input : 	None
// Output :	None 
// Version : v0.1
void DRAM_Copy_Test_P0(void)
{
	u32 uMode,uTime, uBurst,i;

	DMC_Init(0, eALLEN,   eDIS_AutoPD , 0 );

	UART_Printf("Select Operating Mode: 0: retain 1: Sync-533/133,   2: Async-533/133 \n");
	uMode = UART_GetIntNum();
	

	switch(uMode)
		{
			case 0:
				SYSC_CLKInform();
				break;
		
			case 1:		// Sync Mode
			SYSC_ChangeSYSCLK_1(eAPLL532M, eAPLL266M, 0, 1, 3);
			SYSC_ChangeMode(eSYNC_MODE);
			SYSC_ChangeSYSCLK_1(eAPLL532M, eAPLL266M, 0, 1, 3);
			
			break;

			case 2:		//ASync Mode
			//SYSC_ChangeMode(eASYNC_MODE);
			SYSC_ChangeSYSCLK_1(eAPLL532M, eAPLL266M, 0, 0, 3);
			break;
		}

	UART_Printf("===== Timer Check =====\n");
	//UART_Getc();
	g_RTC_ALM1=0;
	INTC_SetVectAddr(NUM_RTC_TIC,Isr_RTC_Tick2);
	INTC_Enable(NUM_RTC_TIC);
	
	//-daedoo	
	/*
	RTC_SetCON(0,0,0,0,0,1);
	RTC_SetTime(InitYEAR,InitMONTH,InitDATE,InitDAY,InitHOUR,InitMIN,InitSEC);
	RTC_SetTickCNT(33);	
	RTC_SetCON(0,0,0,0,0,0);							// Tick Clock = 128Hz?
	RTC_ClearIntP(0);	
	*/

	RTC_TimeInit(InitYEAR,InitMONTH,InitDATE,InitDAY,InitHOUR,InitMIN,InitSEC);
	RTC_TickCnt(33);	
	RTC_TickClkSelect(CLK_32768Hz);
	RTC_ClearPending(TimeTic);
		
	StartTimer(0);

	RTC_TickTimeEnable(true);
	RTC_Enable(true);

	while(g_RTC_ALM1==0);
	uTime=StopTimer(0);
	 UART_Printf(" > Timer Check Time: %d us \n", uTime);
	//RTC_SetCON(0,0,0,0,0,0);
	RTC_Enable(false);
	INTC_Disable(NUM_RTC_TIC);

	UART_Printf("===== Memory Initialization =====\n");
	//UART_Getc();
	 StartTimer(0);
	  for(i=0x41000000;i<(0x41000000+0x100000);i+=4)
    		*((unsigned int *)i)=i^0x55555555;
	 uTime = StopTimer(0);
	 UART_Printf(" > Memory Init.  Time: %d us \n", uTime);
	 
	UART_Printf("***** 1. Memory Copy using ASM  *****\n");
	StartTimer(0);
	MEMCOPY_TEST0();
	uTime = StopTimer(0);
	UART_Printf(" > Word Copy Time: %d us \n", uTime);
	
	UART_Printf("***** 2. Memory Copy using memcpy()  *****\n");
	 StartTimer(0);
	memcpy((int *)0x42000000, (int *)0x41000000, 0x100000);
	 uTime = StopTimer(0);
	 UART_Printf(" > Word Copy Time: %d us \n", uTime);
   
	UART_Printf("***** 3. Memory Copy using FOR loop *****\n");
	//UART_Getc();	
	 StartTimer(0);   
	 Copy(0x41000000, 0x42000000, 0x40000);	// 1M 
	 uTime = StopTimer(0);
	 UART_Printf(" > Word Copy Time: %d us \n", uTime);

	 StartTimer(0);   
	 Copy16(0x41000000, 0x42200000, 0x80000);	// 1M 
	 uTime = StopTimer(0);
	 UART_Printf(" > HWord Copy Time: %d us \n", uTime);

	 StartTimer(0);   
	 Copy8(0x41000000, 0x42400000, 0x100000);	// 1M 
	 uTime = StopTimer(0);
	 UART_Printf(" > Byte Copy Time: %d us \n", uTime);

	 UART_Printf("***** 4. Memory Copy Using DMA *****\n");
	 UART_Printf(" > Select Burst Size : 0:SINGLE, 1:BURST4, 2:BURST8, 3:BURST16, 4:BURST32, 5:BURST64, 6:BURST128, 7:BURST256	: ");
	uBurst=UART_GetIntNum();

     	DMAC_InitCh(DMA0, DMA_ALL, &oDmact0);
	INTC_SetVectAddr(NUM_DMA0,  Dma0Donet);
	INTC_Enable(NUM_DMA0);

	// Word Copy : 1MB
	UART_Printf(" > Word Copy Time: ");
	DMACH_Setup(DMA_A, 0x0, 0x41000000, 0, 0x42000000, 0, WORD, 0x40000, DEMAND, MEM, MEM, (BURST_MODE)uBurst, &oDmact0);
        g_DmaDone=0;
       StartTimer(0);
	DMACH_Start(&oDmact0);
	while(g_DmaDone==0);								// Int.
	uTime = StopTimer(0);
	UART_Printf("%d us \n", uTime);

	 // HWord Copy : 1MB
	UART_Printf(" > HWord Copy Time: ");
	DMACH_Setup(DMA_A, 0x0, 0x41000000, 0, 0x42000000, 0, HWORD, 0x80000, DEMAND, MEM, MEM, (BURST_MODE)uBurst, &oDmact0);
        g_DmaDone=0;
       StartTimer(0);
	DMACH_Start(&oDmact0);
	while(g_DmaDone==0);		
	uTime = StopTimer(0);
	UART_Printf("%d us \n", uTime);
 
 	 // BYTE Copy : 1MB
	UART_Printf(" > BYTE Copy Time: ");
	DMACH_Setup(DMA_A, 0x0, 0x41000000, 0, 0x42000000, 0, BYTE, 0x100000, DEMAND, MEM, MEM, (BURST_MODE)uBurst, &oDmact0);
        g_DmaDone=0;
       StartTimer(0);
	DMACH_Start(&oDmact0);
	while(g_DmaDone==0);		  
	uTime = StopTimer(0);
	UART_Printf("%d us \n\n", uTime);
	
}


//////////
// Function Name : DRAM_Copy_Test_P0
// Function Description : MemoryPort1 Perf. Test
// Input : 	None
// Output :	None 
// Version : v0.1
void DRAM_Copy_ASM(void)
{
	u32 uMode,uTime;
	u32 i;
	

	UART_Printf("Select Operating Mode: 0: retain 1: Sync,   2: Async \n");
	uMode = UART_GetIntNum();
	

	switch(uMode)
		{
			case 0:
				SYSC_CLKInform();
				break;
		
			case 1:		// Sync Mode
			SYSC_ChangeSYSCLK_1(eAPLL532M, eAPLL266M, 0, 1, 3);
			SYSC_ChangeMode(eSYNC_MODE);
			SYSC_ChangeSYSCLK_1(eAPLL532M, eAPLL266M, 0, 1, 3);
			
			break;

			case 2:		//ASync Mode
			//SYSC_ChangeMode(eASYNC_MODE);
			SYSC_ChangeSYSCLK_1(eAPLL532M, eAPLL266M, 0, 0, 3);
			break;
		}
	

	DMC_Init(0, eALLEN,   eDIS_AutoPD , 0 );

  	for(i=0x51000000;i<(0x51000000+0x100000);i+=4)
    		*((unsigned int *)i)=i;
	
	 StartTimer(0);
	MEMCOPY_TEST();
	 uTime = StopTimer(0);
	 UART_Printf("Memory Port1 -Word Copy Time: %d us \n", uTime);

	 StartTimer(0);
	MEMCOPY_TEST0();
	 uTime = StopTimer(0);
	  UART_Printf("Memory Port0 Word Copy Time: %d us \n", uTime);
	
}


//////////
// Function Name : DRAM_Copy_MEMCPY
// Function Description : Performance Test - using MEMCPY
// Input : 	None
// Output :	None 
// Version : v0.1
void DRAM_Copy_MEMCPY(void)
{
	u32 uMode,uTime;
	u32 i;
	
	UART_Printf("Select Operating Mode: 0: retain 1: Sync,   2: Async \n");
	uMode = UART_GetIntNum();
	
	switch(uMode)
		{
			case 0:
				SYSC_CLKInform();
				break;
		
			case 1:		// Sync Mode
			SYSC_ChangeSYSCLK_1(eAPLL532M, eAPLL266M, 0, 1, 3);
			SYSC_ChangeMode(eSYNC_MODE);
			SYSC_ChangeSYSCLK_1(eAPLL532M, eAPLL266M, 0, 1, 3);
			
			break;

			case 2:		//ASync Mode
			//SYSC_ChangeMode(eASYNC_MODE);
			SYSC_ChangeSYSCLK_1(eAPLL532M, eAPLL266M, 0, 0, 3);
			break;
		}

	DMC_Init(0, eALLEN,   eDIS_AutoPD , 0 );

#if 1
  	for(i=0x51000000;i<(0x51000000+0x1000000);i+=4)
	  	*((unsigned int *)i)=i^0x55555555;

 	for(i=0x41000000;i<(0x41000000+0x100000);i+=4)
    		*((unsigned int *)i)=i^0x55555555;
#endif


	 StartTimer(0);
   

	// memcpy (dst, src, size);
	memcpy((int *)0x52000000, (int *)0x51000000, 0x100000);
	//memcpy((int *)0x52000000, (int *)0x51000000, 0x1000);

	 uTime = StopTimer(0);
	 UART_Printf("Port1 - Word Copy Time: %d us \n", uTime);

	 StartTimer(0);
       memcpy((int *)0x42000000, (int *)0x41000000, 0x100000);
	 uTime = StopTimer(0);
	  UART_Printf("Port0- Word Copy Time: %d us \n", uTime);	
	
}



#define NUM_ITERATIONS	10000
#define MEM_TEST_REGION_SZ (300*1024)
static u32 mem_test_region_src[ MEM_TEST_REGION_SZ / sizeof(u32) ];
static u32 mem_test_region_dst[ MEM_TEST_REGION_SZ / sizeof(u32) ];


void DRAM_Perf_Garmin(void)
{
	u32 uMode,uTime;
	u32 i;
	UART_Printf("Select Operating Mode: 0: retain 1: Sync,   2: Async , 3 Async 667/133\n");
	uMode = UART_GetIntNum();
	

	switch(uMode)
		{
			case 0:
				SYSC_CLKInform();
				break;

		
			case 1:		// Sync Mode
			SYSC_ChangeSYSCLK_1(eAPLL532M, eAPLL266M, 0, 1, 3);
			SYSC_ChangeMode(eSYNC_MODE);
			SYSC_ChangeSYSCLK_1(eAPLL532M, eAPLL266M, 0, 1, 3);
			
			break;

			case 2:		//ASync Mode
			//SYSC_ChangeMode(eASYNC_MODE);
			SYSC_ChangeSYSCLK_1(eAPLL532M, eAPLL266M, 0, 0, 3);
			//SYSC_ChangeSYSCLK_1(eAPLL667M, eAPLL266M, 0, 0, 3);
			break;

			case 3:		//ASync Mode
			//SYSC_ChangeMode(eASYNC_MODE);
			//SYSC_ChangeSYSCLK_1(eAPLL532M, eAPLL266M, 0, 0, 3);
			SYSC_ChangeSYSCLK_1(eAPLL667M, eAPLL266M, 0, 0, 3);
			break;
			
		}

	DMC_Init(0, eALLEN,   eDIS_AutoPD , 0 );


	 StartTimer(0);
   for( i = 0; i < NUM_ITERATIONS; i++ )
   //for(i=0; i<5000;i++)
    {
    memcpy( mem_test_region_dst, mem_test_region_src, MEM_TEST_REGION_SZ );
    }
	 uTime = StopTimer(0);
	 UART_Printf("Word Copy Time: %d us \n", uTime);
	
	
}



static u32 mem_test_region_src1[ MEM_TEST_REGION_SZ / sizeof(u32) ];
static u32 mem_test_region_dst1[ MEM_TEST_REGION_SZ / sizeof(u32) ];
static u32 mem_test_region_src2[ MEM_TEST_REGION_SZ / sizeof(u32) ];
static u32 mem_test_region_dst2[ MEM_TEST_REGION_SZ / sizeof(u32) ];

void MemoryRW_for_MultiMaster(void)
{
	u32 uMode,uTime;
	u32 i;
	u32 *psrcaddr0, *pdstaddr0;
	//u32 *psrcData, *pdstData;
	//u32 *psrcData1, *pdstData1, *psrcData2, *pdstData2;
	//u32 *psrcaddr1, *pdstaddr1, *psrcaddr2, *pdstaddr2;
	u32 psrcaddr1, pdstaddr1, psrcaddr2, pdstaddr2;

	UART_Printf("Select Operating Mode: 0: retain 1: Sync,   2: Async , 3 Async 667/133\n");
	uMode = UART_GetIntNum();
	

	switch(uMode)
		{
			case 0:
				SYSC_CLKInform();
				break;

		
			case 1:		// Sync Mode
			SYSC_ChangeSYSCLK_1(eAPLL532M, eAPLL266M, 0, 1, 3);
			SYSC_ChangeMode(eSYNC_MODE);
			SYSC_ChangeSYSCLK_1(eAPLL532M, eAPLL266M, 0, 1, 3);
			
			break;

			case 2:		//ASync Mode
			//SYSC_ChangeMode(eASYNC_MODE);
			SYSC_ChangeSYSCLK_1(eAPLL532M, eAPLL266M, 0, 0, 3);
			//SYSC_ChangeSYSCLK_1(eAPLL667M, eAPLL266M, 0, 0, 3);
			break;

			case 3:		//ASync Mode
			//SYSC_ChangeMode(eASYNC_MODE);
			//SYSC_ChangeSYSCLK_1(eAPLL532M, eAPLL266M, 0, 0, 3);
			SYSC_ChangeSYSCLK_1(eAPLL667M, eAPLL266M, 0, 0, 3);
			break;
			
		}

#if 1				// 20.8s (07.10) 667/133, RBC option(using CMM)   D-Cache Off 31.7s,  I/D Cache Off -> 38.1s	
	
	psrcaddr0 = (u32 *)mem_test_region_src;
	pdstaddr0 = (u32 *)mem_test_region_dst;
	psrcaddr1 =_DRAM_BaseAddress + 0x1000000;
	pdstaddr1 =_DRAM_BaseAddress + 0x2000000;
	psrcaddr2 =_DRAM_BaseAddress + 0x1200000;
	pdstaddr2 = _DRAM_BaseAddress + 0x2200000;
#endif


#if 0				//31.6s (07.10) 667/133, RBC option,   	
	SYSTEM_DisableDCache();
	SYSTEM_CleanInvalidateEntireDCache();
	psrcaddr0 = (u32 *)mem_test_region_src;
	pdstaddr0 = (u32 *)mem_test_region_dst;
	psrcaddr1 =_DRAM_BaseAddress + 0x1000000;
	pdstaddr1 =_DRAM_BaseAddress + 0x2000000;
	psrcaddr2 =_DRAM_BaseAddress + 0x1200000;
	pdstaddr2 = _DRAM_BaseAddress + 0x2200000;
#endif

#if 0		// 19.3s (07.10) 667/133, RBC option
	psrcData = (u32 *)malloc(MEM_TEST_REGION_SZ);	
	pdstData = (u32 *)malloc(MEM_TEST_REGION_SZ);
	psrcData1 = (u32 *)malloc(MEM_TEST_REGION_SZ);	
	pdstData1 = (u32 *)malloc(MEM_TEST_REGION_SZ);
	psrcData2 = (u32 *)malloc(MEM_TEST_REGION_SZ);	
	pdstData2 = (u32 *)malloc(MEM_TEST_REGION_SZ);
	
	psrcaddr0 = psrcData;
	pdstaddr0 = pdstData;
	psrcaddr1 = psrcData1;
	pdstaddr1 = pdstData1;
	psrcaddr2 = psrcData2;
	pdstaddr2 = pdstData2;
	//psrcaddr1 =_DRAM_BaseAddress + 0x1000000;
	//pdstaddr1 =_DRAM_BaseAddress + 0x2000000;
	//psrcaddr2 =_DRAM_BaseAddress + 0x1200000;
	//pdstaddr2 = _DRAM_BaseAddress + 0x2200000;
	
#endif

#if 0	
	SYSTEM_DisableDCache();
	SYSTEM_CleanInvalidateEntireDCache();

	psrcaddr0 = (u32 *)mem_test_region_src;
	pdstaddr0 = (u32 *)mem_test_region_dst;
	psrcaddr1 =(u32 *)mem_test_region_src1;
	pdstaddr1 =(u32 *)mem_test_region_dst1;
	psrcaddr2 =(u32 *)mem_test_region_src2;
	pdstaddr2 =(u32 *)mem_test_region_dst2;
#endif
		UART_Printf("\n\n Test Case: memcopy Only \n\n");


	// memcopy Only
		StartTimer(0);
		for( i = 0; i < NUM_ITERATIONS; i++ )
    		{

			memcpy( pdstaddr0, psrcaddr0, MEM_TEST_REGION_SZ );
  
		}
		uTime = StopTimer(0);
		UART_Printf("memcopy  Time(DataSize): (%d us/%dMB) \n", uTime, ((MEM_TEST_REGION_SZ/1024)*NUM_ITERATIONS)/1024);
	

	// Added 2 DMA Master Operaton

		UART_Printf("\n\n Test Case: Added 2 DMA Master Operation \n\n");
		// DMAC0  Setting
		DMAC_InitCh(DMA0, DMA_ALL, &oDmact0);
		INTC_SetVectAddr(NUM_DMA0,  Dma0Donet);
		INTC_Enable(NUM_DMA0);
		// SDMAC0  Setting
		DMAC_InitCh(SDMA0, DMA_ALL, &oDmact1);
		INTC_SetVectAddr(NUM_SDMA0,  Dma0Donet1);
		INTC_Enable(NUM_SDMA0);

		//Start Memory Copy with DMA operation
		 StartTimer(0);
  		 for( i = 0; i < NUM_ITERATIONS; i++ )
 		  {
	       	 g_DmaDone=0;
	       	 g_DmaDone1=0;

			// DMAC0 Channel Set-up , Transfer Count = WORD(4B) * (MEM_TEST_REGION_SZ/4)
			DMACH_Setup(DMA_A, 0x0, (u32)psrcaddr1, 0, (u32)pdstaddr1, 0, WORD, (MEM_TEST_REGION_SZ/4), DEMAND, MEM, MEM, BURST4, &oDmact0);
			// SDMAC0 Channel Set-up , Transfer Count = WORD(4B) * (MEM_TEST_REGION_SZ/4)
			DMACH_Setup(DMA_A, 0x0, (u32)psrcaddr2, 0, (u32)pdstaddr2, 0, WORD, (MEM_TEST_REGION_SZ/4), DEMAND, MEM, MEM, BURST4, &oDmact1);
			//DMAC0 Start
		      DMACH_Start(&oDmact0);
			// SDMAC0 Start
	        	DMACH_Start(&oDmact1);
 
 			memcpy( pdstaddr0, psrcaddr0, MEM_TEST_REGION_SZ );

		      while((g_DmaDone==0)||(g_DmaDone1==0));	
    
    }
	 uTime = StopTimer(0);
	UART_Printf("memcopy+DMA0+SDMA0  Time(DataSize): (%d us/%dMB) \n", uTime, 3*(((MEM_TEST_REGION_SZ/1024)*NUM_ITERATIONS)/1024));

	
	UART_Printf(" (srcaddr,dstaddr) (srcaddr,dstaddr) (srcaddr,dstaddr) : \n(0x%x, 0x%x)(0x%x, 0x%x)(0x%x, 0x%x)=\n", psrcaddr0, pdstaddr0, psrcaddr1,pdstaddr1,psrcaddr2,pdstaddr2);
	
}


void MemoryRW_for_MultiMaster1(void)
{
	u32 uMode,uTime;
	u32 i;
	u32 psrcaddr0,pdstaddr0;
	u32 psrcaddr1, pdstaddr1, psrcaddr2, pdstaddr2;

	UART_Printf("Select Operating Mode: 0: retain 1: Sync,   2: Async , 3 Async 667/133\n");
	uMode = UART_GetIntNum();
	

	switch(uMode)
		{
			case 0:
				SYSC_CLKInform();
				break;

		
			case 1:		// Sync Mode
			SYSC_ChangeSYSCLK_1(eAPLL532M, eAPLL266M, 0, 1, 3);
			SYSC_ChangeMode(eSYNC_MODE);
			SYSC_ChangeSYSCLK_1(eAPLL532M, eAPLL266M, 0, 1, 3);
			
			break;

			case 2:		//ASync Mode
			//SYSC_ChangeMode(eASYNC_MODE);
			SYSC_ChangeSYSCLK_1(eAPLL532M, eAPLL266M, 0, 0, 3);
			//SYSC_ChangeSYSCLK_1(eAPLL667M, eAPLL266M, 0, 0, 3);
			break;

			case 3:		//ASync Mode
			//SYSC_ChangeMode(eASYNC_MODE);
			//SYSC_ChangeSYSCLK_1(eAPLL532M, eAPLL266M, 0, 0, 3);
			SYSC_ChangeSYSCLK_1(eAPLL667M, eAPLL266M, 0, 0, 3);
			break;
			
		}

	psrcaddr0 = _DRAM_BaseAddress + 0x1400000;
	pdstaddr0 = _DRAM_BaseAddress + 0x2401000;

	psrcaddr1 =_DRAM_BaseAddress + 0x1000000;
	pdstaddr1 =_DRAM_BaseAddress + 0x2001000;
	psrcaddr2 =_DRAM_BaseAddress + 0x1200000;
	pdstaddr2 = _DRAM_BaseAddress + 0x2201000;


	UART_Printf(" (srcaddr,dstaddr) (srcaddr,dstaddr) (srcaddr,dstaddr) : \n(0x%x, 0x%x)(0x%x, 0x%x)(0x%x, 0x%x)=\n", psrcaddr0, pdstaddr0, pdstaddr1,pdstaddr1,pdstaddr2,pdstaddr2);
	
	// memcopy Only
		StartTimer(0);
		for( i = 0; i < NUM_ITERATIONS; i++ )
    		{

			memcpy((int *)pdstaddr0,(int *)psrcaddr0, MEM_TEST_REGION_SZ );
  
		}
		uTime = StopTimer(0);
		UART_Printf("memcopy  Time(DataSize): (%d us/%dMB) \n", uTime, ((MEM_TEST_REGION_SZ/1024)*NUM_ITERATIONS)/1024);
	

	// Added 2 DMA Master Operaton

		UART_Printf("\n\n Test Case: Added 2 DMA Master Operation \n\n");
		// DMAC0  Setting
		DMAC_InitCh(DMA0, DMA_ALL, &oDmact0);
		INTC_SetVectAddr(NUM_DMA0,  Dma0Donet);
		INTC_Enable(NUM_DMA0);
		// SDMAC0  Setting
		DMAC_InitCh(SDMA0, DMA_ALL, &oDmact1);
		INTC_SetVectAddr(NUM_SDMA0,  Dma0Donet1);
		INTC_Enable(NUM_SDMA0);

		//Start Memory Copy with DMA operation
		 StartTimer(0);
  		 for( i = 0; i < NUM_ITERATIONS; i++ )
 		  {
	       	 g_DmaDone=0;
	       	 g_DmaDone1=0;

			// DMAC0 Channel Set-up , Transfer Count = WORD(4B) * (MEM_TEST_REGION_SZ/4)
			DMACH_Setup(DMA_A, 0x0, (u32)psrcaddr1, 0, (u32)pdstaddr1, 0, WORD, (MEM_TEST_REGION_SZ/4), DEMAND, MEM, MEM, BURST4, &oDmact0);
			// SDMAC0 Channel Set-up , Transfer Count = WORD(4B) * (MEM_TEST_REGION_SZ/4)
			DMACH_Setup(DMA_A, 0x0, (u32)psrcaddr2, 0, (u32)pdstaddr2, 0, WORD, (MEM_TEST_REGION_SZ/4), DEMAND, MEM, MEM, BURST4, &oDmact1);
			//DMAC0 Start
		      DMACH_Start(&oDmact0);
			// SDMAC0 Start
	        	DMACH_Start(&oDmact1);
 
 			memcpy((int *)pdstaddr0,(int *)psrcaddr0, MEM_TEST_REGION_SZ );

		      while((g_DmaDone==0)||(g_DmaDone1==0));	
    
    }
	 uTime = StopTimer(0);
	UART_Printf("memcopy+DMA0+SDMA0  Time(DataSize): (%d us/%dMB) \n", uTime, 3*(((MEM_TEST_REGION_SZ/1024)*NUM_ITERATIONS)/1024));

	
	UART_Printf(" (srcaddr,dstaddr) (srcaddr,dstaddr) (srcaddr,dstaddr) : \n(0x%x, 0x%x)(0x%x, 0x%x)(0x%x, 0x%x)=\n", psrcaddr0, pdstaddr0, pdstaddr1,pdstaddr1,pdstaddr2,pdstaddr2);
	
}

#if 1
#define NUM_ITERATIONS_1	1000000
#define MEM_TEST_REGION_SZ_1 (3*1024)

static u32 mem_test_region_src_1[ MEM_TEST_REGION_SZ_1 / sizeof(u32) ];
static u32 mem_test_region_dst_1[ MEM_TEST_REGION_SZ_1 / sizeof(u32) ];
#endif

void MemoryRW_for_MultiMaster2(void)
{
	u32 uMode, uTSize,uTime;
	u32 i;
	u32 *psrcaddr0, *pdstaddr0;
	u32 *psrcaddr1, *pdstaddr1, *psrcaddr2, *pdstaddr2;
	u32 uMEM_TEST_REGION_SZ_1, uNUM_ITERATIONS_1;

	UART_Printf("Select Transfer Size per 1 Interation(KB)(1~300): \n");
	uTSize = UART_GetIntNum();


	uMEM_TEST_REGION_SZ_1 = (1024*uTSize);
	uNUM_ITERATIONS_1 = ((10000*300)/uTSize);


	//temp
	UART_Printf("Transfer Resion Size(KB):  %d  Iternation No.:  %d \n", (uMEM_TEST_REGION_SZ_1)/1024,(uNUM_ITERATIONS_1));


	UART_Printf("Select Operating Mode: 0: retain 1: Sync,   2: Async , 3 Async 667/133\n");
	uMode = UART_GetIntNum();
	

	switch(uMode)
		{
			case 0:
				SYSC_CLKInform();
				break;

		
			case 1:		// Sync Mode
			SYSC_ChangeSYSCLK_1(eAPLL532M, eAPLL266M, 0, 1, 3);
			SYSC_ChangeMode(eSYNC_MODE);
			SYSC_ChangeSYSCLK_1(eAPLL532M, eAPLL266M, 0, 1, 3);
			
			break;

			case 2:		//ASync Mode
			//SYSC_ChangeMode(eASYNC_MODE);
			SYSC_ChangeSYSCLK_1(eAPLL532M, eAPLL266M, 0, 0, 3);
			//SYSC_ChangeSYSCLK_1(eAPLL667M, eAPLL266M, 0, 0, 3);
			break;

			case 3:		//ASync Mode
			//SYSC_ChangeMode(eASYNC_MODE);
			//SYSC_ChangeSYSCLK_1(eAPLL532M, eAPLL266M, 0, 0, 3);
			SYSC_ChangeSYSCLK_1(eAPLL667M, eAPLL266M, 0, 0, 3);
			break;
			
		}

	//psrcData = (u32 *)malloc(MEM_TEST_REGION_SZ_1);	
	//pdstData = (u32 *)malloc(MEM_TEST_REGION_SZ_1);
#if 1
	psrcaddr0 = (u32 *)mem_test_region_src_1;
	pdstaddr0 = (u32 *)mem_test_region_dst_1;
#endif
	//psrcData = (u32 *)malloc(MEM_TEST_REGION_SZ_1);	
	//pdstData = (u32 *)malloc(MEM_TEST_REGION_SZ_1);
	//psrcaddr0 = psrcData;
	//pdstaddr0 = pdstData;

	psrcaddr1 =(u32 *)mem_test_region_src1;
	pdstaddr1 =(u32 *)mem_test_region_dst1;
	psrcaddr2 =(u32 *)mem_test_region_src2;
	pdstaddr2 =(u32 *)mem_test_region_dst2;

	// memcopy Only
		StartTimer(0);
		for( i = 0; i < uNUM_ITERATIONS_1; i++ )
    		{

			memcpy( pdstaddr0, psrcaddr0, uMEM_TEST_REGION_SZ_1 );
  
		}
		uTime = StopTimer(0);
		UART_Printf("memcopy  Time(DataSize): (%d us/%dMB) \n", uTime, ((uMEM_TEST_REGION_SZ_1/1024)*uNUM_ITERATIONS_1)/1024);
	

	// Added 2 DMA Master Operaton

		UART_Printf("\n\n Test Case: Added 2 DMA Master Operation \n\n");
		// DMAC0  Setting
		DMAC_InitCh(DMA0, DMA_ALL, &oDmact0);
		INTC_SetVectAddr(NUM_DMA0,  Dma0Donet);
		INTC_Enable(NUM_DMA0);
		// SDMAC0  Setting
		DMAC_InitCh(SDMA0, DMA_ALL, &oDmact1);
		INTC_SetVectAddr(NUM_SDMA0,  Dma0Donet1);
		INTC_Enable(NUM_SDMA0);

		//Start Memory Copy with DMA operation
		 StartTimer(0);
  		 for( i = 0; i < uNUM_ITERATIONS_1; i++ )
 		  {
	       	 g_DmaDone=0;
	       	 g_DmaDone1=0;

			// DMAC0 Channel Set-up , Transfer Count = WORD(4B) * (MEM_TEST_REGION_SZ/4)
			DMACH_Setup(DMA_A, 0x0, (u32)psrcaddr1, 0, (u32)pdstaddr1, 0, WORD, (uMEM_TEST_REGION_SZ_1/4), DEMAND, MEM, MEM, BURST4, &oDmact0);
			// SDMAC0 Channel Set-up , Transfer Count = WORD(4B) * (MEM_TEST_REGION_SZ/4)
			DMACH_Setup(DMA_A, 0x0, (u32)psrcaddr2, 0, (u32)pdstaddr2, 0, WORD, (uMEM_TEST_REGION_SZ_1/4), DEMAND, MEM, MEM, BURST4, &oDmact1);
			//DMAC0 Start
		      DMACH_Start(&oDmact0);
			// SDMAC0 Start
	        	DMACH_Start(&oDmact1);
 
 			memcpy( pdstaddr0, psrcaddr0, uMEM_TEST_REGION_SZ_1 );

		      while((g_DmaDone==0)||(g_DmaDone1==0));	
    
    }
	 uTime = StopTimer(0);
	UART_Printf("memcopy+DMA0+SDMA0  Time(DataSize): (%d us/%dMB) \n", uTime, 3*(((uMEM_TEST_REGION_SZ_1/1024)*uNUM_ITERATIONS_1)/1024));

	
	UART_Printf(" (srcaddr,dstaddr) (srcaddr,dstaddr) (srcaddr,dstaddr) : \n(0x%x, 0x%x)(0x%x, 0x%x)(0x%x, 0x%x)=\n", psrcaddr0, pdstaddr0, pdstaddr1,pdstaddr1,pdstaddr2,pdstaddr2);
	
}



void DRAM_Copy_TCM(void)
{
	u32 uMode,uTime;
	u32 i;
	UART_Printf("Select Operating Mode: 0: retain 1: Sync,   2: Async , 3 Async 667/133\n");
	uMode = UART_GetIntNum();
	

	switch(uMode)
		{
			case 0:
				SYSC_CLKInform();
				break;

		
			case 1:		// Sync Mode
			SYSC_ChangeSYSCLK_1(eAPLL532M, eAPLL266M, 0, 1, 3);
			SYSC_ChangeMode(eSYNC_MODE);
			SYSC_ChangeSYSCLK_1(eAPLL532M, eAPLL266M, 0, 1, 3);
			
			break;

			case 2:		//ASync Mode
			//SYSC_ChangeMode(eASYNC_MODE);
			SYSC_ChangeSYSCLK_1(eAPLL532M, eAPLL266M, 0, 0, 3);
			//SYSC_ChangeSYSCLK_1(eAPLL667M, eAPLL266M, 0, 0, 3);
			break;

			case 3:		//ASync Mode
			//SYSC_ChangeMode(eASYNC_MODE);
			//SYSC_ChangeSYSCLK_1(eAPLL532M, eAPLL266M, 0, 0, 3);
			SYSC_ChangeSYSCLK_1(eAPLL667M, eAPLL266M, 0, 0, 3);
			break;
			
		}

	DMC_Init(0, eALLEN,   eDIS_AutoPD , 0 );

#if 1
  	for(i=0x51000000;i<(0x51000000+0x1000000);i+=4)
	  	*((unsigned int *)i)=i;

  	for(i=0x50f00000;i<(0x50f00000+0x100000);i+=4)
	  	*((unsigned int *)i)=i;
  	
 	for(i=0x41000000;i<(0x41000000+0x100000);i+=4)
    		*((unsigned int *)i)=i;
#endif


//	while(!UART_GetKey())
//	{


	
	 StartTimer(0);
   
	for (i=0; i<100000; i++)
	{
	// memcpy (dst, src, size);
	memcpy((int *)0x52000000, (int *)0x51000000, 0x8000);
	}

	 uTime = StopTimer(0);
	 UART_Printf("memcopy to DMCtoDMC Time(DataSize): (%d us/%dMB) \n", uTime, 1*(((0x8000/1024)*100000)/1024));

	 StartTimer(0);
	for (i=0; i<100000; i++)
	{ 
       memcpy((int *)0x80000000, (int *)0x51000000, 0x8000);
	}
	 uTime = StopTimer(0);
	 UART_Printf("memcopy to DMCtoTCM Time(DataSize): (%d us/%dMB) \n", uTime, 1*(((0x8000/1024)*100000)/1024));


	 	
	 StartTimer(0);
   
	for (i=0; i<100000; i++)
	{
	// memcpy (dst, src, size);
	memcpy((int *)0x52000000, (int *)0x80000000, 0x8000);
	}

	 uTime = StopTimer(0);
	 UART_Printf("memcopy to TCMtoDMC Time(DataSize): (%d us/%dMB) \n", uTime, 1*(((0x8000/1024)*100000)/1024));


	 StartTimer(0);
   
	for (i=0; i<200000; i++)
	{
	// memcpy (dst, src, size);
	memcpy((int *)0x80004000, (int *)0x80000000, 0x4000);
	}

	 uTime = StopTimer(0);
	 UART_Printf("memcopy to TCMtoTCM Time(DataSize): (%d us/%dMB) \n", uTime, 1*(((0x4000/1024)*200000)/1024));


	 
	 StartTimer(0);
   
	for (i=0; i<800000; i++)
	{
	// memcpy (dst, src, size);
	memcpy((int *)0x50f04000, (int *)0x50f00000, 0x1000);
	}

	 uTime = StopTimer(0);
	 UART_Printf("memcopy to CBarea to CBarea Time(DataSize): (%d us/%dMB) \n", uTime, 1*(((0x1000/1024)*800000)/1024));
	 
//	}
	
	
}


#if 1		// Using 2 Port Case

static void Temp_DRAM_COPY(void)
{
	u32 i, check_size, uDMC1_TestAddr, uDMC0_TestAddr;
	u32 uerror = 0;

    check_size		= 0x3000000/4;
    uDMC1_TestAddr	= _DRAM_BaseAddress + 0x1000000;
    uDMC0_TestAddr   = _DRAM_BaseAddress+ 0x4000000;


while(uerror==0)		
{
    	
    for(i=uDMC1_TestAddr;i<(uDMC1_TestAddr+check_size*4);i+=4)
    	*((unsigned int *)i)=i^0x55555555;
    	//*((unsigned int *)i)=0x55555555;
        
   

	Copy(uDMC1_TestAddr, uDMC0_TestAddr,check_size );
           

 	
    for(i=0;i<(0+check_size*4);i+=4)
    {
    	if(*((unsigned int *)(uDMC0_TestAddr+i))!=((uDMC1_TestAddr+i)^0x55555555))
    //	if(*((unsigned int *)i)!=(0x55555555))
    	{
    	    UART_Printf("Mem Error:0x%x=0x%x(%x)\n",(uDMC0_TestAddr+i),*((unsigned int *)(uDMC0_TestAddr+i)),(uDMC1_TestAddr+i)^0x55555555);
    	    uerror++;
    	}
    	if(uerror>20)
	    break;
    }
    if(uerror)
    	UART_Printf("DMC0 Access test:FAILED\n"); 
    else 
    	UART_Printf("DMC0 Access test:O.K.\n");	
	

    for(i=uDMC1_TestAddr;i<(uDMC1_TestAddr+check_size*4);i+=4)
    	*((unsigned int *)i)=i;

    for(i=uDMC0_TestAddr;i<(uDMC0_TestAddr+check_size*4);i+=4)
    	*((unsigned int *)i)=i;

}

}
#endif

#if 0 		//for Type1 Test B/D
//////////
// Function Name : DMC0_Test
// Function Description : DMC0 Init & Access Test
// Input : 	None
// Output :	None 
// Version : v0.1
static void DMC1_TEST_Type1(void)
{
	u32 i, check_size, uDMC1_TestAddr, uDMC0_TestAddr;
	u32 uerror = 0;

    check_size		= 0x100000;
    uDMC1_TestAddr  = _DRAM_BaseAddress + 0x1000000;
    uDMC0_TestAddr   = _DRAM_BaseAddress+ 0x11000000;
		
	
	
     	
    for(i=uDMC1_TestAddr;i<(uDMC1_TestAddr+check_size);i+=4)
    	*((unsigned int *)i)=i^0x55555555;
    	//*((unsigned int *)i)=0x55555555;
    
    
   

	Copy(uDMC1_TestAddr, uDMC0_TestAddr,check_size );
           

 	
    for(i=0;i<(0+check_size);i+=4)
    {
    	if(*((unsigned int *)(uDMC0_TestAddr+i))!=((uDMC1_TestAddr+i)^0x55555555))
    //	if(*((unsigned int *)i)!=(0x55555555))
    	{
    	    UART_Printf("Mem Error:0x%x=0x%x(%x)\n",(uDMC0_TestAddr+i),*((unsigned int *)(uDMC0_TestAddr+i)),(uDMC1_TestAddr+i)^0x55555555);
    	    uerror++;
    	}
    	if(uerror>20)
	    break;
    }
    if(uerror)
    	UART_Printf("DMC0 Access test:FAILED\n"); 
    else 
    	UART_Printf("DMC0 Access test:O.K.\n");	
	


}

#endif

#if 0

static void Test_IndCKE(void)
{
	u32 i;



	  for(i=0x51000000;i<(0x51000000+0x100);i+=4)
    		*((unsigned int *)i)=i^0x55555555;


	while(1)
	{
		MemoryDump0( ((u32 *)(0x51000000)), 40);	

		
	}
	


}
#endif


///////////////////////////////////////////////////////////////////////////////////
////////////////////                    Full Function Test                  /////////////////////////// 
///////////////////////////////////////////////////////////////////////////////////

void DMC_FullFunction(void)
{

	oFunctionT_AutoVar oAutoTest;	
	
	UART_Printf("\n [ DMC Full Function Test ]\n");
	UART_Printf("============================================= \n");
	
	g_DMCT_Err =0;

	oAutoTest.Test_Parameter[0] = 0;
 	DMCtoDMC_Test(eTest_Auto, oAutoTest );

	///////////////  DMC to DMC Path  ///////////////////////
      
	if(g_DMCT_Err ==0)
	 	UART_Printf("		1. DMC to DMC Copy Test. :           PASS\n ");
	else
		UART_Printf("		1. DMC to DMC Copy Test. :          FAIL\n ");

	///////////////  DMC to TCM Path  ///////////////////////

	
	g_DMCT_Err =0;

	oAutoTest.Test_Parameter[0] = 0;
	DMCtoTCM_Test(eTest_Auto, oAutoTest );
	
	if(g_DMCT_Err ==0)
	 	UART_Printf("		2. DMC to TCM Copy Test. :           PASS\n ");
	else
		UART_Printf("		2. DMC to TCM Copy Test :          FAIL\n ");
	

}


u8 DMC_autotest(void)
{
	u32 uErrflag=0;
	oFunctionT_AutoVar oAutoTest;	
	
	UART_Printf("\n [ DMC Full Function Test ]\n");
	UART_Printf("============================================= \n");
	
	g_DMCT_Err =0;

	oAutoTest.Test_Parameter[0] = 0;
 	DMCtoDMC_Test(eTest_Auto, oAutoTest );

	///////////////  DMC to DMC Path  ///////////////////////
      
	if(g_DMCT_Err ==0)
	 	UART_Printf("		1. DMC to DMC Copy Test. :           PASS\n ");
	else
	{
		UART_Printf("		1. DMC to DMC Copy Test. :          FAIL\n ");
		uErrflag++;
	}
	///////////////  DMC to TCM Path  ///////////////////////

	
	g_DMCT_Err =0;

	oAutoTest.Test_Parameter[0] = 0;
	DMCtoTCM_Test(eTest_Auto, oAutoTest );
	
	if(g_DMCT_Err ==0)
	 	UART_Printf("		2. DMC to TCM Copy Test. :           PASS\n ");
	else
	{
		UART_Printf("		2. DMC to TCM Copy Test :          FAIL\n ");
		uErrflag++;
	}

	if(uErrflag==0) return true;
	else return false;
	

}




///////////////////////////////////////////////////////////////////////////////////
////////////////////                    DMC SUB Test                  /////////////////////////// 
///////////////////////////////////////////////////////////////////////////////////

const AutotestFuncMenu dmc_fmenu[] =
{
		
		DMCtoDMC_Test,		              "Test DMC to DMC Copy Test",
		DMCtoTCM_Test,		              "Test DMC to TCM Copy Test",
		//DMC1_TEST_Type1,			"Type1 B/D Test",
		//Test_IndCKE,				"Temp Test",
		0, 0
};


void DMC_Func_Test(void)
{
	u32 i;
	s32 uSel;
	oFunctionT_AutoVar oParameter;
	
	while(1)
	{
		for (i=0; (u32)(dmc_fmenu[i].desc)!=0; i++)
			UART_Printf("%2d: %s\n", i, dmc_fmenu[i].desc);

		UART_Printf("\nSelect the function to test : \n");
		uSel =UART_GetIntNum();
		UART_Printf("\n");
		if(uSel == -1) 
			break;

		if (uSel>=0 && uSel<(sizeof(dmc_fmenu)/8-1))
			(dmc_fmenu[uSel].func) (eTest_Manual, oParameter);
	}

}


const testFuncMenu dmc_pmenu[] =
{
		0,                      				"Exit",			
		Mem_dump0,				"Memory dump\n",
		
		DRAM_Copy_Test_P1,		"Perf_MEM1_Copy&DMA",
		DRAM_Copy_Test_P0,		"Perf_MEM0_Copy&DMA",
		DRAM_Copy_ASM,			"Perf_Test- Using ASM - 8 Burst",
		//DRAM_Copy_MEMCPY,		"Perf_Test- Using memcpy- 4 Burst",
		DRAM_Perf_Garmin,			"Perf_Test-Garmin",
		MemoryRW_for_MultiMaster,	"Mermory RW Test - for AXI(Data Cache)-heap",
		MemoryRW_for_MultiMaster1,	"Mermory RW Test - for AXI(NCNB)-Fixed Addr",
		MemoryRW_for_MultiMaster2,	"Mermory RW Test - for AXI(Data Cache)",
		DRAM_Copy_TCM,			"Perf. Test Between DMC with TCM\n",

		Temp_DRAM_COPY,			"DRAM Copy -Aging",
		0, 0
};


void DMC_Perf_Test(void)
{
	u32 i;
	s32 uSel;
	
	
	while(1)
	{
		for (i=0; (u32)(dmc_pmenu[i].desc)!=0; i++)
			UART_Printf("%2d: %s\n", i, dmc_pmenu[i].desc);

		UART_Printf("\nSelect the function to test : \n");
		uSel =UART_GetIntNum();
		UART_Printf("\n");
		if(uSel == -1) 
			break;

		if (uSel>=0 && uSel<(sizeof(dmc_pmenu)/8-1))
			(dmc_pmenu[uSel].func) ();
	}

}


///////////////////////////////////////////////////////////////////////////////////
////////////////////                    DMC Main Test                  /////////////////////////// 
///////////////////////////////////////////////////////////////////////////////////

const testFuncMenu dmc_menu[] =
{
		0,                      				"Exit\n",			
		Mem_dump0,				"Memory dump",

		DMC_Func_Test,				"DMC Function Test",
		DMC_Perf_Test,				"DMC Perf Test",

		DMC_FullFunction,			"DMC Full Function Test",
		
		0,0
};

void DMC_Test(void)
{
	u32 i;
	s32 uSel;

	UART_Printf("[DMC_Test]\n\n");
	

	while(1)
	{
		for (i=0; (u32)(dmc_menu[i].desc)!=0; i++)
			UART_Printf("%2d: %s\n", i, dmc_menu[i].desc);

		UART_Printf("\nSelect the function to test : ");
		uSel =UART_GetIntNum();
		UART_Printf("\n");
		if(uSel == -1) 
			break;
		if (uSel==0)
			break;
		if (uSel>=0 && uSel<(sizeof(dmc_menu)/8-1))
			(dmc_menu[uSel].func) ();
	}
}	




