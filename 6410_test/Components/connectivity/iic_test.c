/**************************************************************************************
* 
*	Project Name : S3C6410 Validation
*
*	Copyright 2006~2008 by Samsung Electronics, Inc.
*	All rights reserved.
*
*	Project Description :
*		This software is only for validating functions of the S3C6410.
*		Anybody can use this software without our permission.
*  
*--------------------------------------------------------------------------------------
* 
*	File Name : iic_test.c
*  
*	File Description : This file implements the API functons for IIC test.
*
*	Author : Woojin.Kim
*	Dept. : AP Development Team
*	Created Date : 2006/01/11
*	Version : 0.1 
* 
*	History
*	- Created(Woojin.Kim 2006/01/11)
*	- Changed(OnPil, Shin (SOP) 2008/03/01 ~ 2008/03/04)
*  
**************************************************************************************/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "iic.h"
#include "def.h"
#include "option.h"
#include "library.h"
#include "sfr6410.h"
#include "system.h"
#include "sysc.h"
#include "intc.h"
#include "sysc.h"

#define COMPAdd1	0x51200000
#define COMPAdd2	0x51300000

void SFR_DefaultIIC(void);
void SFR_TestIIC(void);

// Channel 0
void IIC0_BTest(void);		// Edited by SOP on 2008/03/03
void IIC0_MasterTxP(void);
void IIC0_SlaveRxP(void);
void IIC0_MasterRxP(void);
void IIC0_SlaveTxP(void);
void IIC0_SlaveTxInt(void);
void IIC0_SlaveRxInt(void);
void IIC0_CLKDIVTest(void);	// Added by SOP on 2008/04/03
void IIC0_CLKGateTest(void);	// Added by SOP on 2008/04/03
void IIC0_CLKDIV(u32 uHCLKx2_DIVN_R, u32 uPCLK_DIVN_R, u32 uFreq);	// Added by SOP on 2008/04/03

// Channel 1
void IIC1_BTest(void);		// Added by SOP on 2008/03/03
void IIC1_MasterTxP(void);
void IIC1_SlaveRxP(void);
void IIC1_MasterRxP(void);
void IIC1_SlaveTxP(void);
void IIC1_SlaveTxInt(void);
void IIC1_SlaveRxInt(void);
void IIC1_CLKDIVTest(void);	// Added by SOP on 2008/04/03
void IIC1_CLKGateTest(void);	// Added by SOP on 2008/04/04
void IIC1_CLKDIV(u32 uHCLKx2_DIVN_R, u32 uPCLK_DIVN_R, u32 uFreq);	// Added by SOP on 2008/04/03

u8 IIC_AutoTest(void);				// Added by SOP on 2008/04/22
u8 IIC1_AutoTest(void);			// Added by SOP on 2008/04/22

extern u8 g_uSlaveAddr;

REGINFO	sRegInfoIIC[] = 
{
	// IIC Channel 0
	{"rIICCON0	", I2C0_BASE+0x00, 8,	RW, DPDB, 0, 0x00},
	{"rIICSTAT0	", I2C0_BASE+0x04, 8,	RW, DPDB, 0, 0x00}, 
	{"rIICADD0	", I2C0_BASE+0x08, 8,	RW, DPDB, 0, 0x00}, 
	{"rIICCDS0	", I2C0_BASE+0x0c, 8,	RW, DPDB, 0, 0x00}, 
	{"rIICLC0	", I2C0_BASE+0x10, 3,	RW, DPDB, 0, 0x00}, 
	// IIC Channel 1
	{"rIICCON1	", I2C1_BASE+0x00, 8,	RW, DPDB, 0, 0x00},
	{"rIICSTAT1	", I2C1_BASE+0x04, 8,	RW, DPDB, 0, 0x00}, 
	{"rIICADD1	", I2C1_BASE+0x08, 8,	RW, DPDB, 0, 0x00}, 
	{"rIICCDS1	", I2C1_BASE+0x0c, 8,	RW, DPDB, 0, 0x00}, 
	{"rIICLC1	", I2C1_BASE+0x10, 3,	RW, DPDB, 0, 0x00}, 	
};


const testFuncMenu g_aIICTestFunc[] =
{	
	SFR_TestIIC,		"IIC SFR R/W test    ",
	//SFR_DefaultIIC,		"IIC SFR Default Value Check",

	// Channel 0
	IIC0_BTest,			"IIC0 MasterTx/Rx from EEPROM (KS24C080C) by Interrupt)",
	IIC0_MasterTxP,		"IIC0 Master Tx (B'd to B'd) by Polling",
	IIC0_SlaveRxP,		"IIC0 Slave  Rx (B'd to B'd) by Polling",
	IIC0_MasterRxP,		"IIC0 Master Rx (B'd to B'd) by Polling",
	IIC0_SlaveTxP,		"IIC0 Slave  Tx (B'd to B'd) by Polling",
	IIC0_SlaveTxInt,		"IIC0 Slave  Tx (B'd to B'd) by Interrupt",	
	IIC0_SlaveRxInt,		"IIC0 Slave  Rx (B'd to B'd) by Interrupt",
	IIC0_CLKDIVTest,	"IIC0 Clock Divider Test",
	IIC0_CLKGateTest,	"IIC0 Clock Gating (On/Off) Test",
	IIC_AutoTest,		"IIC0 Auto Test from EEPROM (KS24C080C) by Interrupt\n",

	// Channel 1
	IIC1_BTest,			"IIC1 MasterTx/Rx from EEPROM (S524AD0XD1) by Interrupt)",
	IIC1_MasterTxP,		"IIC1 Master Tx (B'd to B'd) by Polling",
	IIC1_SlaveRxP,		"IIC1 Slave  Rx (B'd to B'd) by Polling",
	IIC1_MasterRxP,		"IIC1 Master Rx (B'd to B'd) by Polling",
	IIC1_SlaveTxP,		"IIC1 Slave  Tx (B'd to B'd) by Polling",
	IIC1_SlaveTxInt,		"IIC1 Slave  Tx (B'd to B'd) by Interrupt",	
	IIC1_SlaveRxInt,		"IIC1 Slave  Rx (B'd to B'd) by Interrupt",
	IIC1_CLKDIVTest,	"IIC1 Clock Divider Test",
	IIC1_CLKGateTest,	"IIC1 Clock Gating (On/Off) Test",
	IIC1_AutoTest,		"IIC1 Auto Test from EEPROM (S524AD0XD1) by Interrupt",
	0,0
};


void IIC_Test(void)
{
	u32 uCountFunc=0;
	s32 iSel=0;
	
	while(1)
	{
		UART_Printf("\n\n================== IIC Function Test (2008/05/28) =====================\n\n");	
		
		for (uCountFunc=0; (u32)(g_aIICTestFunc[uCountFunc].desc)!=0; uCountFunc++)
			UART_Printf("%2d: %s\n", uCountFunc, g_aIICTestFunc[uCountFunc].desc);

		UART_Printf("\nSelect the function to test : ");
		iSel =UART_GetIntNum();
		UART_Printf("\n");
		if(iSel == -1) 
			break;

		if (iSel>=0 && iSel<(sizeof(g_aIICTestFunc)/8-1))
			(g_aIICTestFunc[iSel].func) ();
	}
}	


//////////
// Function Name : TestSFRIIC
// Function Description : This function test pattern for IIC SFR write/read
// Input : NONE
// Output : None
// Version : v0.0
void TestSFRIIC(void)		//Added by SOP on 2008/03/01
{
	volatile u32 *pAddr;	
	s32 i,j, count = sizeof(sRegInfoIIC)/sizeof(REGINFO), nPattern;
	u32 uRegStatus, uWritePatternWithMask, uReadWithMask;	
	
	u32 uPatternShitfOneBit[] = 
	{
		0x00000001,		0x00000002,		0x00000004,		0x00000008,		0x00000010,		0x00000020,		0x00000040,		0x00000080,
		0x00000100,		0x00000200,		0x00000400,		0x00000800,		0x00001000,		0x00002000,		0x00004000,		0x00008000,
		0x00010000,		0x00020000,		0x00040000,		0x00080000,		0x00100000,		0x00200000,		0x00400000,		0x00800000,
		0x01000000,		0x02000000,		0x04000000,		0x08000000,		0x10000000,		0x20000000,		0x40000000,		0x80000000,
	};
	
	u32 uDefaultBitMask[] =
	{
		0x00000001,		0x00000003,		0x00000007,		0x0000000F,		0x0000001F,		0x0000003F,		0x0000007F,		0x000000FF,
		0x000001FF,		0x000003FF,		0x000007FF,		0x00000FFF,		0x00001FFF,		0x00003FFF,		0x00007FFF,		0x0000FFFF,
		0x0001FFFF,		0x0003FFFF,		0x0007FFFF,		0x000FFFFF,		0x001FFFFF,		0x003FFFFF,		0x007FFFFF,		0x00FFFFFF,
		0x01FFFFFF,		0x03FFFFFF,		0x07FFFFFF,		0x0FFFFFFF,		0x1FFFFFFF,		0x3FFFFFFF,		0x7FFFFFFF,		0xFFFFFFFF,
	};

	
	for(i=0; i<count; i++)
	{		
		pAddr = (u32*)sRegInfoIIC[i].uAddr;		
		
		if(sRegInfoIIC[i].uFlag == DPDB)
		{		
			if(sRegInfoIIC[i].uRWType == RW)
			{
				for(j=0; j< sRegInfoIIC[i].uBitLen ; j++)
				{			
					uWritePatternWithMask = (uPatternShitfOneBit[j] &uDefaultBitMask[j]);
					*pAddr = uWritePatternWithMask;					
					
					uReadWithMask = (*pAddr & uDefaultBitMask[j]);	
					sRegInfoIIC[i].rValue = uReadWithMask;
					
					uRegStatus = (	uWritePatternWithMask !=uReadWithMask );					
					RPRINTF(sRegInfoIIC[i], uPatternShitfOneBit[j], uRegStatus, j, 1);
				}
			}
		}		
		else if(sRegInfoIIC[i].uFlag == DPPB)
		{		
			nPattern = (sizeof(uPatternShitfOneBit)/sizeof(uPatternShitfOneBit[0]));			
			if(sRegInfoIIC[i].uRWType == RW)
			{
				for(j=0; j<nPattern; j++)
				{
					if(uPatternShitfOneBit[j] & sRegInfoIIC[i].uPrivateBitMask)
					{
						uWritePatternWithMask = (uPatternShitfOneBit[j] & sRegInfoIIC[i].uPrivateBitMask);
						*pAddr = uWritePatternWithMask;
						
						uReadWithMask = (*pAddr & sRegInfoIIC[i].uPrivateBitMask);
						sRegInfoIIC[i].rValue = uReadWithMask;
						
						uRegStatus = (	uWritePatternWithMask != uReadWithMask );												
						RPRINTF(sRegInfoIIC[i], uPatternShitfOneBit[j], uRegStatus, j, 1);					
					}
				}
			}			
		}					
	}
	UART_Printf("\n\nIIC SFR Read/Write Test Complete!\n\n");
	return ;
}


//////////
// Function Name : SFR_TestIIC
// Function Description : This function tests SFR by write and read out from SFR
// Input : NONE
// Output : None
// Version : v0.0
void SFR_TestIIC(void)
{
	TestSFRIIC();
}

#if 0
//////////
// Function Name : SFR_DefaultIIC
// Function Description : This function read tests default value from SFR
// Input : NONE
// Output : None
// Version : v0.0
void SFR_DefaultIIC(void)
{
	s32 i, count = sizeof(sRegInfoIIC)/sizeof(REGINFO);

	for(i=0; i<count; i++)
	{		
		UART_Printf("\n%20s (0x%08X): 0x%02X", sRegInfoIIC[i].name, sRegInfoIIC[i].uAddr, sRegInfoIIC[i].rValue);
	}
	UART_Printf("\n\nIIC SFR Read Complete !!!\n\n");
	return ;
}
#endif

//////////
// Function Name : IIC0_Btest
// Function Description : This function tests IIC with 7-bit addresses by write and read out from EEPROM
// Input : NONE
// Output : Print out Data which readed out from EEPROM
// Version : v0.1
void IIC0_BTest( void)
{
	u32 uCnt, uCntErr=0, uLoop=0, uFixcnt=0;
	u8 cData;
	u8 *pCmp;
	u8 *pComp;
	u32 uMin=0x0;
	//u32 uMax=0x100;			
	u32 uMax=0x3e0;			// KS24C080C: 1KB (8Kbit) = 0x3e8	

	UART_Printf("[ IIC0 Test using Serial EEPROM (KS24C080C) ]\n");

	IIC_Open(400000);	
	
	while(!GetKey())
	{	
		UART_Printf("\nWrite (0x%x ~ 0x%x) and read back from EEPROM (KS24C080C)\n", uMin, (uMax-1));
		pCmp = (u8 *)COMPAdd1;
		pComp = (u8 *)COMPAdd2;	

		for(uCnt=uMin;uCnt<uMax;uCnt++) 
		{
			cData=0;
			*pComp++ = uCnt;		
			IIC_Write(0xa0, uCnt, uCnt);
			IIC_Read(0xa1,  uCnt, &cData);
			
			while((u8) uCnt != cData)				// If Bus Arbitration failed, So recovery Work Around by SOP on 2008/05/27
			{
				//UART_Printf("\n\n**** Retry Write and Read ***\n\n");		
				IIC_Write(0xa0, uCnt, uCnt);
				IIC_Read(0xa1,  uCnt, &cData);
				uFixcnt++;
			}
			
			UART_Printf("%02x ", cData);
			*pCmp++ = cData;
			if ((uCnt&0xf)==0xf) UART_Printf("\n");
		}

		pCmp = (u8 *)COMPAdd1;
		pComp = (u8 *)COMPAdd2;

		for (uCnt=uMin; uCnt<uMax; uCnt++)
		{
			if(*pCmp != *pComp)
			{
				uCntErr++;
				UART_Printf("Error %03d [Address= %6d]: Write= 0x%02x  ,  Read= 0x%02x\n", uCntErr,uCnt,*pComp,*pCmp);			
			}			
			
			pCmp++;
			pComp++;
		}
		uLoop++;
	}
	IIC_Close();
	
	if (uCntErr == 0)
	{
		UART_Printf("\nPass: %d Looping (%d Data Repaired)\n", uLoop, uFixcnt);	
	}
	else
	{
		UART_Printf("\n *** Fail: %d count !!!\n", uCntErr);
		Pause();
	}
}

//////////
// Function Name : IIC1_Btest
// Function Description : This function tests IIC with 7-bit addresses by write and read out from EEPROM
// Input : NONE
// Output : Print out Data which readed out from EEPROM
// Version : v0.1
void IIC1_BTest( void)		//Added by SOP on 2008/03/04	
{
	u32 uCnt, uCntErr=0, uLoop=0, uFixcnt=0;
	u8 cData;
	u8 *pCmp;
	u8 *pComp;
	u32 uMin=0x0;
	//u32 uMax=0x500;			
	u32 uMax=0x3e00;			// S524AD0XD1: 16KB (128Kbit) = 0x3e80	

	UART_Printf("[ IIC1 Test using Serial EEPROM (S524AD0XD1) ]\n");

	IIC1_Open(400000);	
	
	while(!GetKey())
	{	
		UART_Printf("\nWrite (0x%x ~ 0x%x) and read back from EEPROM (S524AD0XD1)\n", uMin, (uMax-1));
		pCmp = (u8 *)COMPAdd1;
		pComp = (u8 *)COMPAdd2;	

		for(uCnt=uMin;uCnt<uMax;uCnt++) 
		{
			cData=0;
			*pComp++ = uCnt;		
			IIC1_Write(0xae, uCnt, uCnt);
			IIC1_Read(0xaf,  uCnt, &cData);
			
			while((u8) uCnt != cData)				// If Bus Arbitration failed, So recovery Work Around by SOP on 2008/05/27
			{
				//UART_Printf("\n\n**** Retry Write and Read ***\n\n");		
				IIC1_Write(0xae, uCnt, uCnt);
				IIC1_Read(0xaf,  uCnt, &cData);
				uFixcnt++;
			}
			
			UART_Printf("%02x ", cData);
			*pCmp++ = cData;
			if ((uCnt&0xf)==0xf) UART_Printf("\n");
		}

		pCmp = (u8 *)COMPAdd1;
		pComp = (u8 *)COMPAdd2;

		for (uCnt=uMin; uCnt<uMax; uCnt++)
		{
			if(*pCmp != *pComp)
			{
				uCntErr++;
				UART_Printf("Error %03d [Address= %6d]: Write= 0x%02x  ,  Read= 0x%02x\n", uCntErr,uCnt,*pComp,*pCmp);			
			}			
			
			pCmp++;
			pComp++;
		}
		uLoop++;
	}
	IIC1_Close();
	
	if (uCntErr == 0)
	{
		UART_Printf("\nPass: %d Looping (%d Data Repaired)\n", uLoop, uFixcnt);	
	}
	else
	{
		UART_Printf("\n *** Fail: %d count !!!\n", uCntErr);
		Pause();
	}
}


//////////
// Function Name : IIC_AutoTest
// Function Description : This function tests IIC for AutoTest
// Input : NONE
// Output : True (Test success). False (Test fail)
// Version : v0.1
u8 IIC_AutoTest(void)				// Added by SOP on 2008/04/22
{
	u32 uCnt, uCntErr=0;
	u8 cData;
	u8 *pCmp;
	u8 *pComp;
	u32 uMin=0x0;
	u32 uMax=0x100;			// 1KB (8Kbit) = 0x3e8	

	UART_Printf("[ IIC0 Auto Test using Serial EEPROM (KS24C080C) ]\n");

	IIC_Open(400000);	
	
	UART_Printf("\nWrite (0x%x ~ 0x%x) and read back from EEPROM (KS24C080C)\n", uMin, (uMax-1));
	pCmp = (u8 *)COMPAdd1;
	pComp = (u8 *)COMPAdd2;	

	for(uCnt=uMin;uCnt<uMax;uCnt++) 
	{
		cData=0;
		*pComp++ = uCnt;		
		IIC_Write(0xa0, uCnt, uCnt);
		IIC_Read(0xa1,  uCnt, &cData);
		UART_Printf("%02x ", cData);
		*pCmp++ = cData;
		if ((uCnt&0xf)==0xf) UART_Printf("\n");
	}

	pCmp = (u8 *)COMPAdd1;
	pComp = (u8 *)COMPAdd2;

	for (uCnt=uMin; uCnt<uMax; uCnt++)
	{
		if(*pCmp != *pComp)
		{
			uCntErr++;
			UART_Printf("Error %03d [Address= %6d]: Write= 0x%02x  ,  Read= 0x%02x\n", uCntErr,uCnt,*pComp,*pCmp);			
		}			
		
		pCmp++;
		pComp++;
	}

	IIC_Close();
	
	if (uCntErr == 0)
	{
		UART_Printf("\nPassed.\n");	
		return true;	
	}
	else
	{
		UART_Printf("\n *** Fail: %d Data !!!\n", uCntErr);
		return false;		
	}
}


//////////
// Function Name : IIC1_AutoTest
// Function Description : This function tests IIC for AutoTest. (S524AD0XD1 128Kbit)
// Input : NONE
// Output : True (Test success). False (Test fail)
// Version : v0.1
u8 IIC1_AutoTest(void)
{
	u32 uCnt, uCntErr=0;
	u8 cData;
	u8 *pCmp;
	u8 *pComp;
	u32 uMin=0x0;
	u32 uMax=0x100;			// 16KB (128Kbit) = 0x3e80	

	UART_Printf("[ IIC1 Auto Test using Serial EEPROM (S524AD0XD1) ]\n");

	IIC1_Open(400000);	
	
	UART_Printf("\nWrite (0x%x ~ 0x%x) and read back from EEPROM (S524AD0XD1)\n", uMin, (uMax-1));
	pCmp = (u8 *)COMPAdd1;
	pComp = (u8 *)COMPAdd2;	

	for(uCnt=uMin;uCnt<uMax;uCnt++) 
	{
		cData=0;
		*pComp++ = uCnt;		
		IIC1_Write(0xae, uCnt, uCnt);
		IIC1_Read(0xaf,  uCnt, &cData);
		UART_Printf("%02x ", cData);
		*pCmp++ = cData;
		if ((uCnt&0xf)==0xf) UART_Printf("\n");
	}

	pCmp = (u8 *)COMPAdd1;
	pComp = (u8 *)COMPAdd2;

	for (uCnt=uMin; uCnt<uMax; uCnt++)
	{
		if(*pCmp != *pComp)
		{
			uCntErr++;
			UART_Printf("Error %03d [Address= %6d]: Write= 0x%02x  ,  Read= 0x%02x\n", uCntErr,uCnt,*pComp,*pCmp);			
		}			
		
		pCmp++;
		pComp++;
	}

	IIC1_Close();
	
	if (uCntErr == 0)
	{
		UART_Printf("\nPassed.\n");	
		return true;	
	}
	else
	{
		UART_Printf("\n *** Fail: %d Data !!!\n", uCntErr);
		return false;		
	}
}


//////////
// Function Name : IIC0_MasterTxP
// Function Description : This function tests polling mode of the MasterTx by B'd to B'd
// Input : NONE
// Output : Transmit data to SlaveRx Board
// Version : v0.1
// note) to test this function, SlaveRx should be ready
void IIC0_MasterTxP(void)
{
	u32 sCnt;
	u8 cData[100];
	static u8 cSlaveAddr = 0xd0;	

	UART_Printf("\n[ IIC0 Master Tx Test (Board to Board) ]\n");
	UART_Printf("Connect IIC related signals (IICSCL0, IICSDA0) between IIC0 Master TX Board and IIC0 Slave RX Board.\n");
	UART_Printf("Are you excution IIC0 Slave Rx (B'd to B'd) by Polling(or Interrupt) of IIC0 Slave Rx Board ?\n");	
	IIC_OpenPolling(400000);
	Pause();

	for(sCnt=0;sCnt<100;sCnt++)
		cData[sCnt]=(u8)sCnt;

	IIC_MasterWrP(cSlaveAddr, cData);
	UART_Printf("\n*** Completed transmit data.\n");	
	IIC_Close();
}


//////////
// Function Name : IIC1_MasterTxP
// Function Description : This function tests polling mode of the MasterTx by B'd to B'd
// Input : NONE
// Output : Transmit data to SlaveRx Board
// Version : v0.1
// note) to test this function, SlaveRx should be ready
void IIC1_MasterTxP(void)		//Added by SOP on 2008/03/04	
{
	u32 sCnt;
	u8 cData[100];
	static u8 cSlaveAddr = 0xd0;	

	UART_Printf("\n[ IIC1 Master Tx Test (Board to Board) ]\n");
	UART_Printf("Connect IIC related signals (IICSCL1, IICSDA1) between IIC1 Master TX Board and IIC1 Slave RX Board.\n");
	UART_Printf("Are you excution IIC1 Slave Rx (B'd to B'd) by Polling(or Interrupt) of IIC1 Slave Rx Board ?\n");	
	IIC1_OpenPolling(400000);
	Pause();

	for(sCnt=0;sCnt<100;sCnt++)
		cData[sCnt]=(u8)sCnt;

	IIC1_MasterWrP(cSlaveAddr, cData);
	UART_Printf("\n*** Completed transmit data.\n");	
	IIC1_Close();
}


//////////
// Function Name : IIC0_MasterRxP
// Function Description : This function tests polling mode of the MasterRx by B'd to B'd
// Input : NONE
// Output : Print out Data which readed out from SlaveTx Board
// Version : v0.1
// note ) to test this function , SlaveTx should be ready
void IIC0_MasterRxP(void)
{
	s32 cCnt;
	static u8 cData[256];
	static u8 cSlaveAddr = 0xc0;	

	UART_Printf("[ IIC0 Master Rx Test (Board to Board) ]\n");
	IIC_OpenPolling(400000);

	for(cCnt=0;cCnt<101;cCnt++)
		cData[cCnt]=0;

	UART_Printf("Connect IIC0 related signals (IICSCL0, IICSDA0) between IIC0 Master RX Board and IIC0 Slave TX Board.\n");
	UART_Printf("At first, SlaveTx should set up before MasterRx. Make sure that SlaveTx is set up.\n");	
	Pause();
	
	IIC_MasterRdP(cSlaveAddr,cData);

	UART_Printf("Received Address: 0x%02x\n\n",(u32)cSlaveAddr);
	for(cCnt=1;cCnt<101;cCnt++)
		UART_Printf("Received Data: %03d\n",(u32)cData[cCnt]);

	UART_Printf("\n*** Completed receive data.\n");	
	IIC_Close();
}


//////////
// Function Name : IIC1_MasterRxP
// Function Description : This function tests polling mode of the MasterRx by B'd to B'd
// Input : NONE
// Output : Print out Data which readed out from SlaveTx Board
// Version : v0.1
// note ) to test this function , SlaveTx should be ready
void IIC1_MasterRxP(void)		//Added by SOP on 2008/03/04	
{
	s32 cCnt;
	static u8 cData[256];
	static u8 cSlaveAddr = 0xc0;	

	UART_Printf("[ IIC1 Master Rx Test (Board to Board) ]\n");
	IIC1_OpenPolling(400000);

	for(cCnt=0;cCnt<101;cCnt++)
		cData[cCnt]=0;

	UART_Printf("Connect IIC1 related signals (IICSCL1, IICSDA1) between IIC1 Master RX Board and IIC1 Slave TX Board.\n");
	UART_Printf("At first, SlaveTx should set up before MasterRx. Make sure that SlaveTx is set up.\n");	
	Pause();
	
	IIC1_MasterRdP(cSlaveAddr,cData);

	UART_Printf("Received Address: 0x%02x\n\n",(u32)cSlaveAddr);
	for(cCnt=1;cCnt<101;cCnt++)
		UART_Printf("Received Data: %03d\n",(u32)cData[cCnt]);

	UART_Printf("\n*** Completed receive data.\n");	
	IIC1_Close();
}


//////////
// Function Name : IIC0_SlaveTxP
// Function Description : This function tests polling mode of the SlaveTx by B'd to B'd
// Input : NONE
// Output : Transmit data to MasterRx Board
// Version : v0.1
void IIC0_SlaveTxP(void)
{
	u32 sCnt;
	u8 cData[100];
	static u8 cSlaveAddr = 0xc0;

	UART_Printf("[ IIC0 Slave Tx Test (Board to Board) ]\n");
	UART_Printf("Connect IIC0 related signals (IICSCL0, IICSDA0) between IIC0 Master RX Board and IIC0 Slave TX Board.\n");
	IIC_OpenPolling(400000);

	for(sCnt=0;sCnt<100;sCnt++)
		cData[sCnt]=(u8)sCnt;

	IIC_SlaveWrP(&cSlaveAddr, cData);
	UART_Printf("\n*** Completed transmit data.\n");		
	IIC_Close();
}


//////////
// Function Name : IIC1_SlaveTxP
// Function Description : This function tests polling mode of the SlaveTx by B'd to B'd
// Input : NONE
// Output : Transmit data to MasterRx Board
// Version : v0.1
void IIC1_SlaveTxP(void)		//Added by SOP on 2008/03/04	
{
	u32 sCnt;
	u8 cData[100];
	static u8 cSlaveAddr = 0xc0;

	UART_Printf("[ IIC1 Slave Tx Test (Board to Board) ]\n");
	UART_Printf("Connect IIC1 related signals (IICSCL1, IICSDA1) between IIC1 Master RX Board and IIC1 Slave TX Board.\n");
	IIC1_OpenPolling(400000);

	for(sCnt=0;sCnt<100;sCnt++)
		cData[sCnt]=(u8)sCnt;

	IIC1_SlaveWrP(&cSlaveAddr, cData);
	UART_Printf("\n*** Completed transmit data.\n");		
	IIC1_Close();
}


//////////
// Function Name : IIC0_SlaveRxP
// Function Description : This function tests polling mode of the SlaveRx by B'd to B'd connection
// Input : NONE
// Output : Print out Data which readed out from MasterTx Board
// Version : v0.1
void IIC0_SlaveRxP(void)
{
	s32 cCnt;
	static u8 cData[256];
	static u8 cSlaveAddr = 0xd0;

	UART_Printf("\n[ IIC0 Slave Rx Test (Board to Board) ]\n");
	IIC_OpenPolling(400000);	

	for(cCnt=0;cCnt<101;cCnt++)
		cData[cCnt]=0;

	UART_Printf("Connect IIC0 related signals (IICSCL0, IICSDA0) between IIC0 Master TX Board and IIC0 Slave RX Board.\n");
	IIC_SlaveRdP(&cSlaveAddr,cData);
	cSlaveAddr = cData[0];			// Added by SOP on 2008/05/23
	
	UART_Printf("*** Received Address: 0x%02x\n\n",(u32)cSlaveAddr);
	
	for(cCnt=1;cCnt<101;cCnt++)
		UART_Printf("Received Data: %03d\n",(u32)cData[cCnt]);

	UART_Printf("\n*** Completed receive data\n");
	IIC_Close();
}


//////////
// Function Name : IIC1_SlaveRxP
// Function Description : This function tests polling mode of the SlaveRx by B'd to B'd connection
// Input : NONE
// Output : Print out Data which readed out from MasterTx Board
// Version : v0.1
void IIC1_SlaveRxP(void)		//Added by SOP on 2008/03/04	
{
	s32 cCnt;
	static u8 cData[256];
	static u8 cSlaveAddr = 0xd0;

	UART_Printf("\n[ IIC1 Slave Rx Test (Board to Board) ]\n");
	IIC1_OpenPolling(400000);	

	for(cCnt=0;cCnt<101;cCnt++)
		cData[cCnt]=0;

	UART_Printf("Connect IIC1 related signals (IICSCL1, IICSDA1) between IIC1 Master TX Board and IIC1 Slave RX Board.\n");
	IIC1_SlaveRdP(&cSlaveAddr,cData);
	cSlaveAddr = cData[0];			// Added by SOP on 2008/05/23
	
	UART_Printf("*** Received Address: 0x%02x\n\n",(u32)cSlaveAddr);
	
	for(cCnt=1;cCnt<101;cCnt++)
		UART_Printf("Received Data: %03d\n",(u32)cData[cCnt]);

	UART_Printf("\n*** Completed receive data\n");
	IIC1_Close();
}


//////////
// Function Name : IIC0_SlaveTxInt
// Function Description : This function tests Interrupt mode of the SlaveTx by B'd to B'd
// Input : NONE
// Output : Transmit data to MasterRx Board
// Version : v0.1
void IIC0_SlaveTxInt(void)
{
	u32 sCnt;
	u8 cData[100];
	static u8 cSlaveAddr= 0xc0;

	UART_Printf("\n[ IIC0 Slave Tx (Interrupt Mode) Test (Board to Board) ]\n");
	UART_Printf("Connect IIC0 related signals (IICSCL0, IICSDA0) between IIC0 Master RX(Polling) Board and IIC0 Slave TX Board.\n");
	IIC_Open(400000);					// Interrupt mode, Edited by SOP on 2008/03/03

	for(sCnt=0;sCnt<100;sCnt++)
		cData[sCnt]=(u8)sCnt;

	IIC_SlaveWrInt(&cSlaveAddr, cData);
	UART_Printf("\n*** Completed transmit data\n");	
	IIC_Close();
}


//////////
// Function Name : IIC1_SlaveTxInt
// Function Description : This function tests Interrupt mode of the SlaveTx by B'd to B'd
// Input : NONE
// Output : Transmit data to MasterRx Board
// Version : v0.1
void IIC1_SlaveTxInt(void)		//Added by SOP on 2008/03/04	
{
	u32 sCnt;
	u8 cData[100];
	static u8 cSlaveAddr= 0xc0;

	UART_Printf("\n[ IIC1 Slave Tx (Interrupt Mode) Test (Board to Board) ]\n");
	UART_Printf("Connect IIC1 related signals (IICSCL1, IICSDA1) between IIC1 Master RX(Polling) Board and IIC1 Slave TX Board.\n");
	IIC1_Open(400000);					// Interrupt mode, Edited by SOP on 2008/03/03

	for(sCnt=0;sCnt<100;sCnt++)
		cData[sCnt]=(u8)sCnt;

	IIC1_SlaveWrInt(&cSlaveAddr, cData);
	UART_Printf("\n*** Completed transmit data\n");	
	IIC1_Close();
}


//////////
// Function Name : IIC0_SlaveRxInt
// Function Description : This function tests Interrupt mode of the SlaveRx by B'd to B'd connection
// Input : NONE
// Output : Print out Data which readed out from MasterTxP Board
// Version : v0.1
void IIC0_SlaveRxInt(void)
{
	s32 cCnt;
	static u8 cData[256];
	static u8 cSlaveAddr = 0xd0;
	
    	UART_Printf("\n[ IIC0 Slave Rx Test (Board to Board) ]\n");
	IIC_Open(400000);					//Interrupt mode, Edited by SOP on 2008/03/03	
   
	for(cCnt=0;cCnt<101;cCnt++)
		cData[cCnt]=0;

    	UART_Printf("Connect IIC0 related signals (IICSCL0, IICSDA0) between IIC0 Master TX(Polling) Board and IIC0 Slave RX Board.\n");
	IIC_SlaveRdInt(&cSlaveAddr,cData);
	cSlaveAddr = cData[0];
	
	UART_Printf("Received Addrress: 0x%02x\n\n",(u32)cSlaveAddr);
	
	for(cCnt=1;cCnt<101;cCnt++)
		UART_Printf("Received Data: %03d\n",(u32)cData[cCnt]);

	UART_Printf("\n*** Completed receive data\n");
	IIC_Close();
}


//////////
// Function Name : IIC1_SlaveRxInt
// Function Description : This function tests Interrupt mode of the SlaveRx by B'd to B'd connection
// Input : NONE
// Output : Print out Data which readed out from MasterTxP Board
// Version : v0.1
void IIC1_SlaveRxInt(void)		//Added by SOP on 2008/03/04	
{
	s32 cCnt;
	static u8 cData[256];
	static u8 cSlaveAddr = 0xd0;
	
    	UART_Printf("\n[ IIC1 Slave Rx Test (Board to Board) ]\n");
	IIC1_Open(400000);					//Interrupt mode, Edited by SOP on 2008/03/03	
   
	for(cCnt=0;cCnt<101;cCnt++)
		cData[cCnt]=0;

    	UART_Printf("Connect IIC1 related signals (IICSCL1, IICSDA1) between IIC1 Master TX(Polling) Board and IIC1 Slave RX Board.\n");
	IIC1_SlaveRdInt(&cSlaveAddr,cData);
	cSlaveAddr = cData[0];
	
	UART_Printf("Received Addrress: 0x%02x\n\n",(u32)cSlaveAddr);
	
	for(cCnt=1;cCnt<101;cCnt++)
		UART_Printf("Received Data: %03d\n",(u32)cData[cCnt]);

	UART_Printf("\n*** Completed receive data\n");
	IIC1_Close();
}


//////////
// Function Name : IIC0_CLKDIVTest
// Function Description : This function tests is Clock divider control.
// Input : NONE
// Output : None
// Version : v0.1
void IIC0_CLKDIVTest(void)			// Added by SOP on 2008/03/24, Now Editing
{
	UART_Printf("[ IIC0 Clock Divider Control Test using Serial EEPROM ]\n");
	ClockInform();

	IIC0_CLKDIV(6,  5, 300000);	// If ARMCLK=532MHz, HCLKX2=76.00MHz, HCLK=38.00MHz, PCLK=12.67MHz
	Pause();
	IIC0_CLKDIV(6,  1, 400000);	// If ARMCLK=532MHz, HCLKX2=76.00MHz, HCLK=38.00MHz, PCLK=38.00MHz
	Pause();	
	IIC0_CLKDIV(1, 13, 200000);	// If ARMCLK=532MHz, HCLKX2=266.00MHz, HCLK=133.00MHz, PCLK=19.00MHz
	Pause();	
	IIC0_CLKDIV(7,  7, 200000);	// If ARMCLK=532MHz, HCLKX2=66.50MHz, HCLK=33.25MHz, PCLK=8.31MHz
	Pause();	
	IIC0_CLKDIV(7, 11, 100000);	// If ARMCLK=532MHz, HCLKX2=66.50MHz, HCLK=33.25MHz, PCLK=5.54MHz
	Pause();	
	IIC0_CLKDIV(1,  5, 400000);	// If ARMCLK=532MHz, HCLKX2=266.00MHz, HCLK=133.00MHz, PCLK=44.33MHz
	Pause();
	IIC0_CLKDIV(6, 11, 100000);	// If ARMCLK=532MHz, HCLKX2=76.00MHz, HCLK=38.00MHz, PCLK=6.33MHz	
	Pause();	
	
	//SYSC_SetDIV0(uAratio, uMratio, uHratio, uHx2ratio, uPratio, uONDratio, uSECUratio, uCAMratio, uJPEGratio, uMFCratio );
	SYSC_SetDIV0(0, 1, 1, 1,  3, 1, 1,0, 1, 0);
	//SYSC_GetClkInform(); 
	UART_InitDebugCh(0, 115200);
	Delay(100);
	ClockInform();
}

//////////
// Function Name : IIC1_CLKDIVTest
// Function Description : This function tests is Clock divider control.
// Input : NONE
// Output : None
// Version : v0.1
void IIC1_CLKDIVTest(void)			// Added by SOP on 2008/03/24, Now Editing
{
	UART_Printf("[ IIC1 Clock Divider Control Test using Serial EEPROM ]\n");
	ClockInform();

	IIC1_CLKDIV(6,  5, 300000);	// If ARMCLK=532MHz, HCLKX2=76.00MHz, HCLK=38.00MHz, PCLK=12.67MHz
	Pause();
	IIC1_CLKDIV(6,  1, 400000);	// If ARMCLK=532MHz, HCLKX2=76.00MHz, HCLK=38.00MHz, PCLK=38.00MHz
	Pause();	
	IIC1_CLKDIV(1, 13, 200000);	// If ARMCLK=532MHz, HCLKX2=266.00MHz, HCLK=133.00MHz, PCLK=19.00MHz
	Pause();	
	IIC1_CLKDIV(7,  7, 200000);	// If ARMCLK=532MHz, HCLKX2=66.50MHz, HCLK=33.25MHz, PCLK=8.31MHz
	Pause();	
	IIC1_CLKDIV(7, 11, 100000);	// If ARMCLK=532MHz, HCLKX2=66.50MHz, HCLK=33.25MHz, PCLK=5.54MHz
	Pause();	
	IIC1_CLKDIV(1,  5, 400000);	// If ARMCLK=532MHz, HCLKX2=266.00MHz, HCLK=133.00MHz, PCLK=44.33MHz
	Pause();
	IIC1_CLKDIV(6, 11, 100000);	// If ARMCLK=532MHz, HCLKX2=76.00MHz, HCLK=38.00MHz, PCLK=6.33MHz	
	Pause();	
	
	//SYSC_SetDIV0(uAratio, uMratio, uHratio, uHx2ratio, uPratio, uONDratio, uSECUratio, uCAMratio, uJPEGratio, uMFCratio );
	SYSC_SetDIV0(0, 1, 1, 1,  3, 1, 1,0, 1, 0);
	//SYSC_GetClkInform(); 
	UART_InitDebugCh(0, 115200);
	Delay(100);
	ClockInform();
}


//////////
// Function Name : IIC0_CLKGateTest
// Function Description : This function tests is Clock gating control.
// Input : NONE
// Output : None
// Version : v0.1
void IIC0_CLKGateTest(void)			// Added by SOP on 2008/03/24, Now Editing
{
	UART_Printf("[ IIC0 Clock Gating Control Test using Serial EEPROM ]\n");
	ClockInform();

	IIC0_CLKDIV(6,  5, 300000);	// If ARMCLK=532MHz, HCLKX2=76.00MHz, HCLK=38.00MHz, PCLK=12.67MHz
	UART_Printf("\nIIC0 OFF (PCLK_GATE[17])\n");
	SYSC_CtrlPCLKGate(ePCLK_I2C,0);
	UART_Printf("rHCLK_GATE: 0x%x, rPCLK_GATE: 0x%x, rSCLK_GATE: 0x%x \n", Inp32SYSC(0x30), Inp32SYSC(0x34), Inp32SYSC(0x38));
	Pause();

#if 0	// for Clock Gate Mask test
	IIC0_CLKDIV(1,  5, 400000);	// If ARMCLK=532MHz, HCLKX2=266.00MHz, HCLK=133.00MHz, PCLK=44.33MHz
	Pause();
#endif

	UART_Printf("\nIIC0 ON (PCLK_GATE[17])\n");
	SYSC_CtrlPCLKGate(ePCLK_I2C,1);
	UART_Printf("rHCLK_GATE: 0x%x, rPCLK_GATE: 0x%x, rSCLK_GATE: 0x%x \n", Inp32SYSC(0x30), Inp32SYSC(0x34), Inp32SYSC(0x38));

	IIC0_CLKDIV(6,  1, 400000);	// If ARMCLK=532MHz, HCLKX2=76.00MHz, HCLK=38.00MHz, PCLK=38.00MHz
	
	SYSC_SetDIV0(0, 1, 1, 1,  3, 1, 1,0, 1, 0);
	//SYSC_GetClkInform(); 
	UART_InitDebugCh(0, 115200);
	Delay(100);
	ClockInform();
}


//////////
// Function Name : IIC1_CLKGateTest
// Function Description : This function tests is Clock gating control.
// Input : NONE
// Output : None
// Version : v0.1
void IIC1_CLKGateTest(void)			// Added by SOP on 2008/03/24, Now Editing
{
	UART_Printf("[ IIC1 Clock Gating Control Test using Serial EEPROM ]\n");
	ClockInform();

	IIC1_CLKDIV(6,  5, 300000);	// If ARMCLK=532MHz, HCLKX2=76.00MHz, HCLK=38.00MHz, PCLK=12.67MHz
	UART_Printf("\nIIC1 OFF (PCLK_GATE[27])\n");
	SYSC_CtrlPCLKGate(ePCLK_IIC1,0);
	UART_Printf("rHCLK_GATE: 0x%x, rPCLK_GATE: 0x%x, rSCLK_GATE: 0x%x \n", Inp32SYSC(0x30), Inp32SYSC(0x34), Inp32SYSC(0x38));
	Pause();

#if 0	// for Clock Gate Mask test
	IIC1_CLKDIV(1,  5, 400000);	// If ARMCLK=532MHz, HCLKX2=266.00MHz, HCLK=133.00MHz, PCLK=44.33MHz
	Pause();
#endif

	UART_Printf("\nIIC1 ON (PCLK_GATE[27])\n");
	SYSC_CtrlPCLKGate(ePCLK_IIC1,1);
	UART_Printf("rHCLK_GATE: 0x%x, rPCLK_GATE: 0x%x, rSCLK_GATE: 0x%x \n", Inp32SYSC(0x30), Inp32SYSC(0x34), Inp32SYSC(0x38));

	IIC1_CLKDIV(6,  1, 400000);	// If ARMCLK=532MHz, HCLKX2=76.00MHz, HCLK=38.00MHz, PCLK=38.00MHz
	
	SYSC_SetDIV0(0, 1, 1, 1,  3, 1, 1,0, 1, 0);
	//SYSC_GetClkInform(); 
	UART_InitDebugCh(0, 115200);
	Delay(100);
	ClockInform();
}



//////////
// Function Name : IIC0_CLKDIV
// Function Description : This function is Clock Divider control.
// Input : HCLKX2, PCLK Divider Value
// Output : None
// Version : v0.1
void IIC0_CLKDIV(u32 uHCLKx2_DIVN_R, u32 uPCLK_DIVN_R, u32 uFreq)
{
	u32 uCnt, uMax=0x10;
	u8 cData;

	//SYSC_SetDIV0(uAratio, uMratio, uHratio, uHx2ratio, uPratio, uONDratio, uSECUratio, uCAMratio, uJPEGratio, uMFCratio );
	SYSC_SetDIV0(0, 1, 1, uHCLKx2_DIVN_R,  uPCLK_DIVN_R, 1, 1,0, 1, 0);
	//SYSC_GetClkInform(); 
	UART_InitDebugCh(0, 115200);
	Delay(100);

	UART_Printf("\nHCLKX2 Divide Value: %d ,  PCLK Divider Value: %d\n", uHCLKx2_DIVN_R, uPCLK_DIVN_R);
	ClockInform();

	IIC_Open(uFreq);		

	UART_Printf("\nWrite (0~255) and read back from EEPROM (KS24C080C)\n");

	for(uCnt=0;uCnt<uMax;uCnt++) 
	{
		cData=0;
		
		IIC_Write(0xa0,uCnt,uCnt);
		IIC_Read(0xa1,uCnt,&cData);		
		UART_Printf("%02x ",cData);
		if ((uCnt&0xf)==0xf) UART_Printf("\n");
	}

	IIC_Close();
}

//////////
// Function Name : IIC0_CLKDIV
// Function Description : This function is Clock Divider control.
// Input : HCLKX2, PCLK Divider Value
// Output : None
// Version : v0.1
void IIC1_CLKDIV(u32 uHCLKx2_DIVN_R, u32 uPCLK_DIVN_R, u32 uFreq)
{
	u32 uCnt, uMax=0x10;
	u8 cData;

	//SYSC_SetDIV0(uAratio, uMratio, uHratio, uHx2ratio, uPratio, uONDratio, uSECUratio, uCAMratio, uJPEGratio, uMFCratio );
	SYSC_SetDIV0(0, 1, 1, uHCLKx2_DIVN_R,  uPCLK_DIVN_R, 1, 1,0, 1, 0);
	//SYSC_GetClkInform(); 
	UART_InitDebugCh(0, 115200);
	Delay(100);

	UART_Printf("\nHCLKX2 Divide Value: %d ,  PCLK Divider Value: %d\n", uHCLKx2_DIVN_R, uPCLK_DIVN_R);
	ClockInform();

	IIC1_Open(uFreq);		

	UART_Printf("\nWrite (0~255) and read back from EEPROM (KS24C080C)\n");

	for(uCnt=0;uCnt<uMax;uCnt++) 
	{
		cData=0;
		
		IIC1_Write(0xa0,uCnt,uCnt);
		//UART_Printf("\n*** Writed\n");		// For debugging		
		IIC1_Read(0xa1,uCnt,&cData);		
		UART_Printf("%02x ",cData);
		if ((uCnt&0xf)==0xf) UART_Printf("\n");
	}

	IIC1_Close();
}
