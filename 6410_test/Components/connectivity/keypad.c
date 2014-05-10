/**************************************************************************************
* 
*	Project Name : S3C6410 Validation
*
*	Copyright 2008 by Samsung Electronics, Inc.
*	All rights reserved.
*
*	Project Description :
*		This software is only for validating functions of the S3C6410.
*		Anybody can use this software without our permission.
*  
*--------------------------------------------------------------------------------------
* 
*	File Name : keypad.c
*  
*	File Description : This file implements the API functons for KEYPAD
*
*	Author : Sung-Hyun, Na
*	Dept. : AP Development Team
*	Created Date : 2008/02/15
*	Version : 0.1 
* 
*	History
*	- Created(Sung-Hyun, Na 2008/02/15)
*
**************************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "keypad.h"

#include "option.h"
#include "library.h"
#include "sfr6410.h"
#include "system.h"
#include "sysc.h"
#include "gpio.h"
#include "intc.h"
#include "timer.h"

#define KEYSCANCODE		2

extern KEYPAD_INFOR	g_KEYInfor;

volatile u32 g_uKeyValue[2];				// To memorize key Position that last Key ISR found

void KEYPAD_Init(void)
{
	KEYPAD_INFOR* pInfor = KEYPAD_GetInfor();

	pInfor->m_ePort 			= KeyPort0;
	pInfor->m_eDFilterSource 	= DF_OSC;
	pInfor->m_uWakeup		= false;
	pInfor->m_eIntSource		= Key_Falling;
	pInfor->m_uFilterEn 		= true;
	pInfor->m_uColEn 		= 0xff;
	pInfor->m_uFilterVal 		= 0x3ff;	
	
	KEYPAD_SetKEYIFCON(pInfor);
	
	KEYIFOutp32(rKEYIFFC, pInfor->m_uFilterVal);		
}

KEYPAD_INFOR* KEYPAD_GetInfor(void)
{
	return &g_KEYInfor;
}



//////////
// Function Name : Isr_KEYPAD
// Function Description : This function is Interrupt Service Routine of KEYPAD
//									Following KEYPAD Scanning procedure
// Input : NONE
// Output : NONE
// Version : v0.1

void __irq Isr_KEYPAD(void)
{
	u8 aDat_row[16]	= {NULL};
	u8 uTemp1;
	u8 uTemp2		= 0xff;
	u8 uTemp3 		= 0xff;
	u8 uTemp4		= 0xff;
	u32 uX,uY;	
	
	u32 urKEYIFCON;
	u32 uIcnt;
	u32 urKEYIFSTSCLR, urKEYIFROW;
	u32 uCol, uRow;
	u32 uColScan, uRowScan;
	

	UART_Printf("\nKey pad Interrupt Service Routine\n");
	
	INTC_Disable(NUM_KEYPAD);

	urKEYIFSTSCLR = KEYIFSTSCLR_GetReg();	
	UART_Printf("Read Interrupt Pending Register, Value is 0x%x\n", urKEYIFSTSCLR);
	
	KEYPAD_STSCLR(0xff,0xff);	
	INTC_ClearVectAddr();

	if ( urKEYIFSTSCLR & 0xff )				// Falling Edge 
	{
		for ( uCol = 0; uCol < KEYIF_COLNUM ; uCol++)
		{
			uColScan = 0xff ^ (1 << uCol);
			KEYPAD_COL(uColScan, 0);
			Delay(50);					// change DelayfrTimer later 

			urKEYIFROW = KEYIFInp32(rKEYIFROW);
			if ( urKEYIFROW != 0xff)
			{
				KEYPAD_STSCLR(0xff,0xff);											// Unwanted Interrupt Source Makes. it must clear!
						
				for(uRow = 0;uRow < KEYIF_ROWNUM; uRow++)							//Scan X-Axis (COLUMN)
				{
					uRowScan = 0xff ^(1 << uRow);
					if ( urKEYIFROW == uRowScan )
					{
						g_uKeyValue[0] = uCol;
						g_uKeyValue[1] = uRow;
						UART_Printf("[%d, %d]\n", uRow, uCol);
						break;
						
					}				
				}				
			}
		}
	}

	else if ( urKEYIFSTSCLR & 0xff00 )		// Rising Edge
	{
		UART_Printf("\n[Row : %d, Col : %d]\n",g_uKeyValue[1], g_uKeyValue[0]);
	}
	
	/*
	#if (KEYSCANCODE == 0)
	// Single Key Scan

	// Disable Key Interrupt
	urKEYIFCON =  KEYIFInp32(rKEYIFCON);
	urKEYIFCON &= ~KEYPAD_EN_RisingINT & ~KEYPAD_EN_FallingINT;
	KEYIFOutp32(rKEYIFCON, urKEYIFCON);
	
	KEYPAD_COL(0,0);				//rKEYIFCOL=0x0;		//aa

	for (uCol = 0;uCol < 8;uCol++)		// Column Scan
	{
		
		KEYPAD_COL(uTemp1,0);
		DelayfrTimer(micro,1);

		uTemp2	= KEYIFInp32(rKEYIFROW);
		
		if (uTemp2 != 0xff)									//Scan Y-Axis (ROW)
		{
			uTemp2 = uTemp2 ^ 0xff;
			for(uX = 0;uX <8;uX++)							//Scan X-Axis (COLUMN)
			{
				if ( (uTemp2 >> uX) & 0x1 )
				UART_Printf("[%d,%d]\n", uX+1, uY+1);				//Display X, Y position
			
			}
	
		}
	}
	
	#elif (KEYSCANCODE == 1)	
	for (uX =0;uX <8; uX++)
	{

		if(  0x01 & ( uTemp1 >> uX) )
		{
			aDat_row[uX] = uX+1;
			uTemp2 = uTemp2 ^ (0x1 << (aDat_row[uX] -1));
			for(uY = 0; uY < 8; uY++)
			{
				uTemp3 = 0xff ^ (1 << uY);
				KEYPAD_COL(uTemp3,0);
				DelayfrTimer(micro,1000);
				//   	Delay(10);	//delay(100)=10msec, delay(10)=1msec 
				
//				if ( (Inp32(rKEYIFROW) == uTemp2) || (Inp32(rKEYIFROW) ==  (0xff ^ (0x1 << (aDat_row[uX] -1)))) || (Inp32(rKEYIFROW) == uTemp1))
					
				if ( (Inp32(rKEYIFROW) == uTemp2) ||  (Inp32(rKEYIFROW) == uTemp4) || (Inp32(rKEYIFROW) ==  (0xff ^ (0x1 << (aDat_row[uX] -1)))))
				{
					UART_Printf("[%d,%d]\n", aDat_row[uX], uY+1);
//					uTemp4 = uTemp4 ^ (1 << aDat_row[uX]-1);
				}

			}
		}

		else 
			aDat_row[uX] = 0;
		
	}

	#elif (KEYSCANCODE == 2)
	
	while(1)
	{


		KEYPAD_COL(0x7f,0);	
	//	rKEYIFCOL=0x1f;
		DelayfrTimer(micro,1000);
	//   	Delay(10);	//delay(100)=10msec, delay(10)=1msec 
		aDat_row[7]=((KEYPAD_ROW())&0xff);
		if(aDat_row[7]!=0xff) 
		{
			aDat_row[0]=0;
			aDat_row[1]=0;
			aDat_row[2]=0;
			aDat_row[3]=0;
			aDat_row[4]=0;
			aDat_row[5]=0;
			aDat_row[6]=0;		
			break;
		}


		KEYPAD_COL(0xbf,0);	
	//	rKEYIFCOL=0x1f;
		DelayfrTimer(micro,1000);
	//   	Delay(10);	//delay(100)=10msec, delay(10)=1msec 
		aDat_row[6]=((KEYPAD_ROW())&0xff);
		if(aDat_row[6]!=0xff) 
		{
			aDat_row[0]=0;
			aDat_row[1]=0;
			aDat_row[2]=0;
			aDat_row[3]=0;
			aDat_row[4]=0;
			aDat_row[5]=0;
			aDat_row[7]=0;		
			break;
		}



		KEYPAD_COL(0xdf,0);	
	//	rKEYIFCOL=0x1f;
		DelayfrTimer(micro,1000);
	//   	Delay(10);	//delay(100)=10msec, delay(10)=1msec 
		aDat_row[5]=((KEYPAD_ROW())&0xff);
		if(aDat_row[5]!=0xff) 
		{
			aDat_row[0]=0;
			aDat_row[1]=0;
			aDat_row[2]=0;
			aDat_row[3]=0;
			aDat_row[4]=0;
			aDat_row[6]=0;
			aDat_row[7]=0;		
			break;
		}

		KEYPAD_COL(0xef,0);
	//	rKEYIFCOL=0x2f;
		DelayfrTimer(micro,1000);
	//	Delay(10);	//delay(100)=10msec, delay(10)=1msec 
		aDat_row[4]=((KEYPAD_ROW())&0xff);
		if(aDat_row[4]!=0xff) 
		{
			aDat_row[0]=0;
			aDat_row[1]=0;
			aDat_row[2]=0;
			aDat_row[3]=0;
			aDat_row[5]=0;
			aDat_row[6]=0;
			aDat_row[7]=0;		
			break;
		}

		KEYPAD_COL(0xf7,0);
	//	rKEYIFCOL=0x37; 
		DelayfrTimer(micro,1000);
	//	Delay(10);	//delay(100)=10msec, delay(10)=1msec 
		aDat_row[3]=((KEYPAD_ROW())&0xff);
		if(aDat_row[3]!=0xff) 
		{
			aDat_row[0]=0;
			aDat_row[1]=0;
			aDat_row[2]=0;
			aDat_row[4]=0;
			aDat_row[5]=0;
			aDat_row[6]=0;
			aDat_row[7]=0;		
			break;
		}
		
		KEYPAD_COL(0xfb,0);
	//	rKEYIFCOL=0x3b;
		DelayfrTimer(micro,1000);
	//	Delay(10);	//delay(100)=10msec, delay(10)=1msec 
		aDat_row[2]=((KEYPAD_ROW())&0xff);
		if(aDat_row[2]!=0xff) 
		{
			aDat_row[0]=0;
			aDat_row[1]=0;
			aDat_row[3]=0;
			aDat_row[4]=0;
			aDat_row[5]=0;
			aDat_row[6]=0;
			aDat_row[7]=0;		
			break;
		}

		KEYPAD_COL(0xfd,0);
	//   	rKEYIFCOL=0x3d;
		DelayfrTimer(micro,1000);
	//	Delay(10);	//delay(100)=10msec, delay(10)=1msec 
		aDat_row[1]=((KEYPAD_ROW())&0xff);
		if(aDat_row[1]!=0xff) 
		{
			aDat_row[0]=0;
			aDat_row[2]=0;
			aDat_row[3]=0;
			aDat_row[4]=0;
			aDat_row[5]=0;
			aDat_row[6]=0;
			aDat_row[7]=0;
			break;
		}

		KEYPAD_COL(0xfe,0);
	//	rKEYIFCOL=0x3e;
		DelayfrTimer(micro,1000);
	//	Delay(10);  //delay(100)=10msec, delay(10)=1msec 
		aDat_row[0]=((KEYPAD_ROW())&0xff);
		if(aDat_row[0]!=0xff) 
		{
			aDat_row[1]=0;
			aDat_row[2]=0;
			aDat_row[3]=0;
			aDat_row[4]=0;
			aDat_row[5]=0;
			aDat_row[6]=0;
			aDat_row[7]=0;		
			break;
		}


	}


    //UART_Printf("row0:%x,row1:%x,row2:%x,row3:%x,row4:%x,row5:%x\n",
    //aDat_row[0],aDat_row[1],aDat_row[2],aDat_row[3],aDat_row[4],aDat_row[5]);

	for(uIcnt=0;uIcnt<9;uIcnt++)
	{
		if(aDat_row[uIcnt]==0xfe)
		{
			UART_Printf("[1,%d]\n",uIcnt+1);
		}
		else if(aDat_row[uIcnt]==0xfd)
		{
			UART_Printf("[2,%d]\n",uIcnt+1);
		}
		else if(aDat_row[uIcnt]==0xfb)
		{
			UART_Printf("[3,%d]\n",uIcnt+1);
		}
		else if(aDat_row[uIcnt]==0xf7)
		{
			UART_Printf("[4,%d]\n",uIcnt+1);
		}
		else if(aDat_row[uIcnt]==0xef)
		{
			UART_Printf("[5,%d]\n",uIcnt+1);
		}
		else if(aDat_row[uIcnt]==0xdf)
		{
			UART_Printf("[6,%d]\n",uIcnt+1);
		}
		else if(aDat_row[uIcnt]==0xbf)
		{
			UART_Printf("[7,%d]\n",uIcnt+1);
		}
		else if(aDat_row[uIcnt]==0x7f)
		{
			UART_Printf("[8,%d]\n",uIcnt+1);
		}		
	}		
	#else
	
	#endif
	*/
	
	
	KEYPAD_COL(0,0);
	Delay(100);
	KEYPAD_STSCLR(0xff,0xff);	
//	Outp32(rKEYIFCON, uTemp_KEY);
	INTC_Enable(NUM_KEYPAD);
	
 }

void __irq Isr_KEYPAD_WakeUp(void)
{
	u32 urKEYIFCON;
	u32 uIcnt;
	u32 urKEYIFSTSCLR, urKEYIFROW;
	u32 uCol, uRow;
	u32 uColScan, uRowScan;

	UART_Printf("\nKey Wake UP!!\n");
	UART_Printf("\nKey pad Interrupt Service Routine\n");	
	INTC_Disable(NUM_KEYPAD);
	
	UART_Printf("\n");
	UART_Printf("KEYIFCON Register Value is 0x%x\n", KEYIFInp32(rKEYIFCON));
	UART_Printf("KEYIFSTSCLR Register Value is 0x%x\n", KEYIFInp32(rKEYIFSTSCLR));
	UART_Printf("KEYIFCOL Register Value is 0x%x\n", KEYIFInp32(rKEYIFCOL));
	UART_Printf("KEYIFROW Register Value is 0x%x\n", KEYIFInp32(rKEYIFROW));
	UART_Printf("KEYIFFC Register Value is 0x%x\n", KEYIFInp32(rKEYIFFC));	
	UART_Printf("\n");
	
	urKEYIFSTSCLR = KEYIFSTSCLR_GetReg();	
	UART_Printf("Read Interrupt Pending Register, Value is 0x%x\n", urKEYIFSTSCLR);
	
	KEYPAD_STSCLR(0xff,0xff);	
	INTC_ClearVectAddr();
	
	KEYPAD_EnableWakeup(false);								// Disable Wake UP

	if ( urKEYIFSTSCLR & 0xff )										// Falling Edge Scan
	{
		for ( uCol = 0; uCol < KEYIF_COLNUM ; uCol++)
		{
			uColScan = 0xff ^ (1 << uCol);
			KEYPAD_COL(uColScan, 0);
			Delay(10);											// change DelayfrTimer later 

			urKEYIFROW = KEYIFInp32(rKEYIFROW);
			if ( urKEYIFROW != 0xff)
			{
				KEYPAD_STSCLR(0xff,0xff);						// Unwanted Interrupt Source Makes. it must clear!
						
				for(uRow = 0;uRow < KEYIF_ROWNUM; uRow++)		//Scan X-Axis (COLUMN)
				{
					uRowScan = 0xff ^(1 << uRow);
					if ( urKEYIFROW == uRowScan )
					{
						g_uKeyValue[0] = uCol;
						g_uKeyValue[1] = uRow;
						UART_Printf("[%d, %d]\n", uRow, uCol);
						break;
						
					}				
				}				
			}
		}
	}

	else if ( urKEYIFSTSCLR & 0xff00 )		// Rising Edge
	{
		UART_Printf("\n[Row : %d, Col : %d]\n",g_uKeyValue[1], g_uKeyValue[0]);
	}

	KEYPAD_COL(0,0);
	Delay(100);
	KEYPAD_STSCLR(0xff,0xff);	
//	Outp32(rKEYIFCON, uTemp_KEY);
	INTC_Enable(NUM_KEYPAD);

}

void KEYPAD_SetKEYIFCON(KEYPAD_INFOR* rhs_pInfor)
{
	u32 uKEYIFCON = 0;	

	uKEYIFCON = (rhs_pInfor->m_uWakeup << 4) | (rhs_pInfor->m_eDFilterSource << 3) | (rhs_pInfor->m_uFilterEn << 2) | (rhs_pInfor->m_eIntSource << 0);

	KEYIFOutp32(rKEYIFCON, uKEYIFCON);
	
}

void KEYPAD_SetPort(KEYPAD_INFOR* rhs_pInfor)
{
	u32 uSel = 0;	
	
	UART_Printf("\nSelect KEY Port");
	UART_Printf("\n0 : KEY PORT 0(HOST IF)[D]			1 : KEY PORT 1(MMC, EINT)\n");
	uSel = UART_GetIntNum(); 

	if ( uSel == 1)
	{
		rhs_pInfor->m_ePort = KeyPort1;
		KEYPAD_ClosePort(KeyPort0);
		Delay(1000);
		KEYPAD_InitPort(KeyPort1);
		Delay(1000);

	}

	else
	{
		rhs_pInfor->m_ePort = KeyPort0;
		KEYPAD_ClosePort(KeyPort1);
		Delay(1000);
		KEYPAD_InitPort(KeyPort0);
		Delay(1000);
	}
}

//////////
// Function Name : KEYPAD_Init
// Function Description : This function Selects KEYPAD Port between GPK, GPL and GPH, GPNand sets up them(HOST I/F)
// Input : ePort - Keypad port 
// Output : NONE
// Version : v0.1

void KEYPAD_InitPort(KEYPORT ePort)
{
	if (ePort == KeyPort0)		//KeyPort0
	{
		
		
		//Key Pad Row		Host I/F
		GPIO_SetFunctionEach(eGPIO_K, eGPIO_8, 3);
		GPIO_SetFunctionEach(eGPIO_K, eGPIO_9, 3);
		GPIO_SetFunctionEach(eGPIO_K, eGPIO_10, 3);
		GPIO_SetFunctionEach(eGPIO_K, eGPIO_11, 3);
		GPIO_SetFunctionEach(eGPIO_K, eGPIO_12, 3);
		GPIO_SetFunctionEach(eGPIO_K, eGPIO_13, 3);
		GPIO_SetFunctionEach(eGPIO_K, eGPIO_14, 3);
		GPIO_SetFunctionEach(eGPIO_K, eGPIO_15, 3);
		
		//Key Pad Column	Host I/F
		GPIO_SetFunctionEach(eGPIO_L, eGPIO_0, 3);
		GPIO_SetFunctionEach(eGPIO_L, eGPIO_1, 3);
		GPIO_SetFunctionEach(eGPIO_L, eGPIO_2, 3);
		GPIO_SetFunctionEach(eGPIO_L, eGPIO_3, 3);
		GPIO_SetFunctionEach(eGPIO_L, eGPIO_4, 3);
		GPIO_SetFunctionEach(eGPIO_L, eGPIO_5, 3);
		GPIO_SetFunctionEach(eGPIO_L, eGPIO_6, 3);
		GPIO_SetFunctionEach(eGPIO_L, eGPIO_7, 3);	
	
		//Pull Up/Down Disable
		GPIO_SetPullUpDownEach(eGPIO_K, eGPIO_8, 0);
		GPIO_SetPullUpDownEach(eGPIO_K, eGPIO_9, 0);
		GPIO_SetPullUpDownEach(eGPIO_K, eGPIO_10, 0);
		GPIO_SetPullUpDownEach(eGPIO_K, eGPIO_11, 0);
		GPIO_SetPullUpDownEach(eGPIO_K, eGPIO_12, 0);
		GPIO_SetPullUpDownEach(eGPIO_K, eGPIO_13, 0);
		GPIO_SetPullUpDownEach(eGPIO_K, eGPIO_14, 0);
		GPIO_SetPullUpDownEach(eGPIO_K, eGPIO_15, 0);
		GPIO_SetPullUpDownEach(eGPIO_L, eGPIO_0, 0);
		GPIO_SetPullUpDownEach(eGPIO_L, eGPIO_1, 0);
		GPIO_SetPullUpDownEach(eGPIO_L, eGPIO_2, 0);
		GPIO_SetPullUpDownEach(eGPIO_L, eGPIO_3, 0);
		GPIO_SetPullUpDownEach(eGPIO_L, eGPIO_4, 0);
		GPIO_SetPullUpDownEach(eGPIO_L, eGPIO_5, 0);
		GPIO_SetPullUpDownEach(eGPIO_L, eGPIO_6, 0);
		GPIO_SetPullUpDownEach(eGPIO_L, eGPIO_7, 0);

		
	}
	else 		//KeyPort1 : MMC/XEINT GPIO connected KEYPAD
	{

		//Key Pad Row		 	EINT 
		GPIO_SetFunctionEach(eGPIO_N,eGPIO_0,3);
		GPIO_SetFunctionEach(eGPIO_N,eGPIO_1,3);
		GPIO_SetFunctionEach(eGPIO_N,eGPIO_2,3);
		GPIO_SetFunctionEach(eGPIO_N,eGPIO_3,3);
		GPIO_SetFunctionEach(eGPIO_N,eGPIO_4,3);
		GPIO_SetFunctionEach(eGPIO_N,eGPIO_5,3);
		GPIO_SetFunctionEach(eGPIO_N,eGPIO_6,3);
		GPIO_SetFunctionEach(eGPIO_N,eGPIO_7,3);	
		
		
		//Key Pad Column		MMC
		GPIO_SetFunctionEach(eGPIO_H,eGPIO_0,4);
		GPIO_SetFunctionEach(eGPIO_H,eGPIO_1,4);
		GPIO_SetFunctionEach(eGPIO_H,eGPIO_2,4);
		GPIO_SetFunctionEach(eGPIO_H,eGPIO_3,4);
		GPIO_SetFunctionEach(eGPIO_H,eGPIO_4,4);
		GPIO_SetFunctionEach(eGPIO_H,eGPIO_5,4);
		GPIO_SetFunctionEach(eGPIO_H,eGPIO_6,4);
		GPIO_SetFunctionEach(eGPIO_H,eGPIO_7,4);
		
		//Pull Up/Down Disable
		GPIO_SetPullUpDownEach(eGPIO_H,eGPIO_0,0);
		GPIO_SetPullUpDownEach(eGPIO_H,eGPIO_1,0);
		GPIO_SetPullUpDownEach(eGPIO_H,eGPIO_2,0);
		GPIO_SetPullUpDownEach(eGPIO_H,eGPIO_3,0);
		GPIO_SetPullUpDownEach(eGPIO_H,eGPIO_4,0);
		GPIO_SetPullUpDownEach(eGPIO_H,eGPIO_5,0);
		GPIO_SetPullUpDownEach(eGPIO_H,eGPIO_6,0);
		GPIO_SetPullUpDownEach(eGPIO_H,eGPIO_7,0);
		GPIO_SetPullUpDownEach(eGPIO_N,eGPIO_0,0);
		GPIO_SetPullUpDownEach(eGPIO_N,eGPIO_1,0);
		GPIO_SetPullUpDownEach(eGPIO_N,eGPIO_2,0);
		GPIO_SetPullUpDownEach(eGPIO_N,eGPIO_3,0);
		GPIO_SetPullUpDownEach(eGPIO_N,eGPIO_4,0);
		GPIO_SetPullUpDownEach(eGPIO_N,eGPIO_5,0);
		GPIO_SetPullUpDownEach(eGPIO_N,eGPIO_6,0);
		GPIO_SetPullUpDownEach(eGPIO_N,eGPIO_7,0);
	}
	Delay(100);
//	rGPGPU=0xfff;

	UART_Printf("Key Pad Port%d Init!!!\n",ePort);
}

void KEYPAD_ClosePort(KEYPORT ePort)
{
	if (ePort == KeyPort0)		//KeyPort0
	{
		//Key Pad Row		Host I/F
		GPIO_SetFunctionEach(eGPIO_K, eGPIO_8, 0);
		GPIO_SetFunctionEach(eGPIO_K, eGPIO_9, 0);
		GPIO_SetFunctionEach(eGPIO_K, eGPIO_10, 0);
		GPIO_SetFunctionEach(eGPIO_K, eGPIO_11, 0);
		GPIO_SetFunctionEach(eGPIO_K, eGPIO_12, 0);
		GPIO_SetFunctionEach(eGPIO_K, eGPIO_13, 0);
		GPIO_SetFunctionEach(eGPIO_K, eGPIO_14, 0);
		GPIO_SetFunctionEach(eGPIO_K, eGPIO_15, 0);
		//Key Pad Column	Host I/F
		GPIO_SetFunctionEach(eGPIO_L, eGPIO_0, 0);
		GPIO_SetFunctionEach(eGPIO_L, eGPIO_1, 0);
		GPIO_SetFunctionEach(eGPIO_L, eGPIO_2, 0);
		GPIO_SetFunctionEach(eGPIO_L, eGPIO_3, 0);
		GPIO_SetFunctionEach(eGPIO_L, eGPIO_4, 0);
		GPIO_SetFunctionEach(eGPIO_L, eGPIO_5, 0);
		GPIO_SetFunctionEach(eGPIO_L, eGPIO_6, 0);
		GPIO_SetFunctionEach(eGPIO_L, eGPIO_7, 0);		
	}
	else 		//KeyPort1 : MMC/XEINT GPIO connected KEYPAD
	{
		//Key Pad Row		 	EINT 
		GPIO_SetFunctionEach(eGPIO_N, eGPIO_1, 0);
		GPIO_SetFunctionEach(eGPIO_N, eGPIO_2, 0);
		GPIO_SetFunctionEach(eGPIO_N, eGPIO_3, 0);
		GPIO_SetFunctionEach(eGPIO_N, eGPIO_4, 0);
		GPIO_SetFunctionEach(eGPIO_N, eGPIO_5, 0);
		GPIO_SetFunctionEach(eGPIO_N, eGPIO_6, 0);
		GPIO_SetFunctionEach(eGPIO_N, eGPIO_7, 0);	
		
		//Key Pad Column		MMC
		GPIO_SetFunctionEach(eGPIO_H, eGPIO_0, 0);
		GPIO_SetFunctionEach(eGPIO_H, eGPIO_1, 0);
		GPIO_SetFunctionEach(eGPIO_H, eGPIO_2, 0);
		GPIO_SetFunctionEach(eGPIO_H, eGPIO_3, 0);
		GPIO_SetFunctionEach(eGPIO_H, eGPIO_4, 0);
		GPIO_SetFunctionEach(eGPIO_H, eGPIO_5, 0);
		GPIO_SetFunctionEach(eGPIO_H, eGPIO_6, 0);
		GPIO_SetFunctionEach(eGPIO_H, eGPIO_7, 0);
		GPIO_SetFunctionEach(eGPIO_N, eGPIO_0, 0);
		
	}
	Delay(100);

}

//////////
// Function Name : KEYPAD_EnableWakeup
// Function Description : This function enables Stop/Idle mode wakeup
// Input : uEnWakeup [ 0: disabe	1: Low lvl wake up		2: Rising Edge wake up		4.Filling Edge wake up]
// Output : NONE
// Version : v0.1

void KEYPAD_Sel_Wakeup(KEYPAD_INFOR* rhs_pInfor)
{
	u32 uSel = 0;	
	
	UART_Printf("\nEnable Wake Up Source");
	UART_Printf("\n0 : Disable[D],    1 : Enable\n");
	uSel = UART_GetIntNum(); 

	KEYPAD_EnableWakeup(uSel);

}

void KEYPAD_EnableWakeup(u32 uSel)
{
	u32 uTemp = KEYIFInp32(rKEYIFCON);
	KEYPAD_INFOR* pInfor = KEYPAD_GetInfor();
	
	if (uSel == 1)
		uTemp |= KEYPAD_EN_WakeupINT;	
	else
		uTemp &= ~KEYPAD_EN_WakeupINT;
	
	pInfor->m_uWakeup = uSel;
	KEYIFOutp32(rKEYIFCON,uTemp);

}

//////////
// Function Name : KEYPAD_EnableDFFC
// Function Description : This function enables debouncing filter & filter clock
// Input : uEnDF [ 0: disable	1: enable]
// 		  uEnFC [ 0: disable	1: enable]
// Output : NONE
// Version : v0.1

void KEYPAD_SetFilterCNT(KEYPAD_INFOR* rhs_pInfor)
{
	u32 uSel 	= 0;

	UART_Printf("\nEnable the Filter Source Clock Counter?\n ");
	UART_Printf("\n0 : Disable(RTCCLK),    1 : Enable(OSCIN)[D]\n");
	uSel = UART_GetIntNum(); 	
	KEYPAD_EnableFilterCNT(uSel);

}

void KEYPAD_EnableFilterCNT(u32 uSel)
{
	u32 uTemp	= KEYIFInp32(rKEYIFCON);
	KEYPAD_INFOR* pInfor = KEYPAD_GetInfor();

	uTemp = KEYIFInp32(rKEYIFCON);
	if (uSel == 0)
	{
		uTemp &= ~KEYPAD_EN_FilterCounter;
		pInfor->m_eDFilterSource = DF_RTCCLK;
	}
	else
	{
		uTemp |= KEYPAD_EN_FilterCounter;
		pInfor->m_eDFilterSource = DF_OSC;
	}	
	KEYIFOutp32(rKEYIFCON,uTemp);
}



//////////
// Function Name : KEYPAD_EnableDFFC
// Function Description : This function enables debouncing filter & filter clock
// Input : uEnDF [ 0: disable	1: enable]
// 		  uEnFC [ 0: disable	1: enable]
// Output : NONE
// Version : v0.1

void KEYPAD_SetDBFilter(KEYPAD_INFOR* rhs_pInfor)
{
	
	u32 uSel 	= 0;	

	UART_Printf("\nEnable the Debounce Filter?\n ");
	UART_Printf("\n0 : Disable,    1 : Enable[D]\n");
	uSel = UART_GetIntNum(); 
	
	rhs_pInfor->m_uFilterEn= uSel;

	KEYPAD_EnableDBFilter(uSel);
	
}

void KEYPAD_EnableDBFilter(u32 uSel)
{
	u32 uTemp = KEYIFInp32(rKEYIFCON);
	if (uSel == 0)
		uTemp &= ~KEYPAD_EN_DebounceFilter;
	else
		uTemp |= KEYPAD_EN_DebounceFilter;

	KEYIFOutp32(rKEYIFCON, uTemp);
}
//////////
// Function Name : KEYPAD_SelectINT
// Function Description : This function selects Interrupt type
// Input : uSelect [1:Falling edge INT    2:Rising edge INT    3:Both edge INT]
// Output : NONE
// Version : v0.1

void KEYPAD_SetINT(KEYPAD_INFOR* rhs_pInfor)
{
	u32 uSel = 0;	
	
	UART_Printf("\nSelect the KeyPad Interrupt Type : ");
	UART_Printf("\n1 : Falling Edge[D],    2 : Rising Edge,    3 : Both Edge \n");
	uSel = UART_GetIntNum(); 

	KEYPAD_SelectINT((KEY_INTSOURCE) uSel, TRUE);
	
}

void KEYPAD_SelectINT(KEY_INTSOURCE eSel, u32 uEnable)
{
	u32 uTemp;	
	KEYPAD_INFOR* pInfor = KEYPAD_GetInfor();
	
	uTemp = KEYIFInp32(rKEYIFCON);	

	if ( uEnable == TRUE ) 
	{
		if(eSel == Key_Rising)				//Default Falling Edge Type
		{
			uTemp |= KEYPAD_EN_RisingINT;
			pInfor->m_eIntSource	= Key_Rising;
		}
		else if (eSel == Key_Both)
		{
			uTemp |= KEYPAD_EN_FallingINT | KEYPAD_EN_RisingINT;	
			pInfor->m_eIntSource	= Key_Both;
		}
		else
		{
			uTemp |= KEYPAD_EN_FallingINT;
			pInfor->m_eIntSource	= Key_Falling;
		}	
	}
	else
	{
		if(eSel == Key_Rising)				//Default Falling Edge Type
		{
			uTemp &= ~KEYPAD_EN_RisingINT;
			pInfor->m_eIntSource	= Key_Rising;
		}
		else if (eSel == Key_Both)
		{
			uTemp &= ~KEYPAD_EN_FallingINT & ~KEYPAD_EN_RisingINT;	
			pInfor->m_eIntSource	= Key_Both;
		}
		else
		{
			uTemp &= ~KEYPAD_EN_FallingINT;
			pInfor->m_eIntSource	= Key_Falling;
		}	
	}	
	KEYIFOutp32(rKEYIFCON, uTemp);
	
}

//////////
// Function Name : KEYPAD_SelectDFCNT
// Function Description : This function selects Interrupt type
// Input : uSelect [1:Falling edge INT    2:Rising edge INT    3:Both edge INT]
// Output : NONE
// Version : v0.1

void KEYPAD_SelectDFCNT(KEYPAD_INFOR* rhs_pInfor)
{
	u32 uTemp;
	s32 uSel = 0;
	
	uTemp = KEYIFInp32(rKEYIFFC) & ~(0x3ff);

	while(1)
	{
		
		UART_Printf("\nSelect Debouce Filter Counter Value(0 ~ 1023) : ");
		uSel = UART_GetIntNum(); 
		
		if(uSel >= 0 && uSel<=1023)				//Default Falling Edge Type
		{
			rhs_pInfor->m_uFilterVal = uSel;
			KEYIFOutp32(rKEYIFFC, uTemp | uSel);
			break;
		}
		else 		
			UART_Printf("\nError!!!  It is not range of Debouce Filter CNT.\n");
		
	}	
}

//////////
// Function Name : KEYPAD_STSCLR
// Function Description : This function checks Interrupt status and clear register
// Input : P_INT [Pressed interrupt is cleared when write data is '1']
// 		  R_INT [Pressed interrupt is cleared when write data is '1']
// Output : NONE
// Version : v0.1

void KEYPAD_STSCLR(u8 ucP_INT, u8 ucR_INT)
{
	KEYIFOutp32(rKEYIFSTSCLR, ucP_INT|ucR_INT<<8);
	
}

//////////
// Function Name : KEYPAD_STSCLR
// Function Description : This function checks Interrupt status and clear register
// Input : None 		  
// Output : Key Interrupt Status Register Value(0x7E00A004)
// Version : v0.1

u32 KEYIFSTSCLR_GetReg(void)
{
	return KEYIFInp32(rKEYIFSTSCLR);
}


//////////
// Function Name : KEYPAD_COL
// Function Description : This function checks COL data output and enable tri-state buffer
// Input : COL [ column data output]
// 		  COLEnable [0: tri-state buffer enable	1: disable]
// Output : NONE
// Version : v0.1

void KEYPAD_COL(u8 ucCOL,u8 ucCOLLEnable)
{
	KEYIFOutp32(rKEYIFCOL,ucCOL|ucCOLLEnable<<8);
	
}


//////////
// Function Name : KEYPAD_ROW
// Function Description : This function reads ROW data input
// Input : NONE
// Output : rKEYIFROW value(u32)
// Version : v0.1

u32 KEYPAD_ROW(void)
{
	return KEYIFInp32(rKEYIFROW);
}







/*

void __irq Key_SWISR(void)
{
	unsigned short dat_row[6];
	int i=0,j=0;
	uint32 restore_keycon;

	restore_keycon = rKEYIFCON;
	rKEYIFCON=0x0;	
	rKEYIFSTSCLR=0x3; //Interrupt pending clear
	INT_Disable0(BIT_KEYPAD);

	while(1)
	{
		//rKEYIFCOL=0x1f;
		Key_Column_Output(0x1f);
	    	Delay(10);	//delay(100)=10msec, delay(10)=1msec 
		dat_row[5]=(rKEYIFROW&0x3f);
		if(dat_row[5]!=0x3f) 
		{
			dat_row[0]=0;
			dat_row[1]=0;
			dat_row[2]=0;
			dat_row[3]=0;
			dat_row[4]=0;
			break;
		}

		//rKEYIFCOL=0x2f;
		Key_Column_Output(0x2f);
		Delay(10);	//delay(100)=10msec, delay(10)=1msec 
		dat_row[4]=(rKEYIFROW&0x3f);
		if(dat_row[4]!=0x3f) 
		{
			dat_row[0]=0;
			dat_row[1]=0;
			dat_row[2]=0;
			dat_row[3]=0;
			dat_row[5]=0;
			break;
		}

		//rKEYIFCOL=0x37; 
		Key_Column_Output(0x37);
		Delay(10);	//delay(100)=10msec, delay(10)=1msec 
		dat_row[3]=(rKEYIFROW&0x3f);
		if(dat_row[3]!=0x3f) 
		{
			dat_row[0]=0;
			dat_row[1]=0;
			dat_row[2]=0;
			dat_row[4]=0;
			dat_row[5]=0;
			break;
		}
		
		//rKEYIFCOL=0x3b;
		Key_Column_Output(0x3B);
		Delay(10);	//delay(100)=10msec, delay(10)=1msec 
		dat_row[2]=(rKEYIFROW&0x3f);
		if(dat_row[2]!=0x3f) 
		{
			dat_row[0]=0;
			dat_row[1]=0;
			dat_row[4]=0;
			dat_row[3]=0;
			dat_row[5]=0;
			break;
		}

		//rKEYIFCOL=0x3e;
		Key_Column_Output(0x3E);
		Delay(10);  //delay(100)=10msec, delay(10)=1msec 
		dat_row[0]=(rKEYIFROW&0x3f);
		if(dat_row[0]!=0x3f) 
		{
			dat_row[4]=0;
			dat_row[1]=0;
			dat_row[2]=0;
			dat_row[3]=0;
			dat_row[5]=0;
			break;
		}

	    	//rKEYIFCOL=0x3d;
	    	Key_Column_Output(0x3D);
		Delay(10);	//delay(100)=10msec, delay(10)=1msec 
		dat_row[1]=(rKEYIFROW&0x3f);
		if(dat_row[1]!=0x3f) 
		{
			dat_row[0]=0;
			dat_row[4]=0;
			dat_row[2]=0;
			dat_row[3]=0;
			dat_row[5]=0;
			break;
		}
	}


	for(i=0;i<7;i++)
	{
		if(dat_row[i]==0x3e)
		{
			UART_Printf("[1,%d]\n",i+1);
		}
		else if(dat_row[i]==0x3d)
		{
			UART_Printf("[2,%d]\n",i+1);
		}
		else if(dat_row[i]==0x3b)
		{
			UART_Printf("[3,%d]\n",i+1);
		}
		else if(dat_row[i]==0x37)
		{
			UART_Printf("[4,%d]\n",i+1);
		}
		else if(dat_row[i]==0x2f)
		{
			UART_Printf("[5,%d]\n",i+1);
		}
		else if(dat_row[i]==0x1f)
		{
			UART_Printf("[6,%d]\n",i+1);
		}
	}

	Key_Column_Output(0);
	rKEYIFCON=restore_keycon;

	INT_Enable0(BIT_KEYPAD);
	Write_VECTADDR(0x0);
 }

void Key_Column_Output(uint32 Col_data)
{
	rGPGDAT = (rGPGDAT & ~0xFC0) | (Col_data<<6);
}



void Key_pad_SWInit(void)
{
	rGPGCON=0x555aaa;		// Row : HW,  Column : SW
	rGPGPU=0xfff;

}
*/

