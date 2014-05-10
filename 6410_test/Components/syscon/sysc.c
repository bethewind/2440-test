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
*	File Name : sysc.c
*  
*	File Description : This file implements the API functons for system controller.
*
*	Author : Haksoo,Kim
*	Dept. : AP Development Team
*	Created Date : 2006/11/08
*	Version : 0.1 
* 
*	History
*	- Created(Haksoo,Kim 2006/11/08)
*	- Added sfr (Wonjoon.Jang 2007/01/08)
*  
**************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "option.h"
#include "library.h"
#include "sfr6410.h"
#include "system.h"

#include "gpio.h"
#include "sysc.h"
#include "rtc.h"
#include "adcts.h"
#include "intc.h"
#include "sdhc.h"

#define dprintf	UART_Printf
//#define dprintf


//
#define rAPLL_LOCK			(SYSCON_BASE+0x000)
#define rMPLL_LOCK			(SYSCON_BASE+0x004)
#define rEPLL_LOCK			(SYSCON_BASE+0x008)
#define rAPLL_CON			(SYSCON_BASE+0x00c)
#define rMPLL_CON			(SYSCON_BASE+0x010)
#define rEPLL_CON0			(SYSCON_BASE+0x014)
#define rEPLL_CON1			(SYSCON_BASE+0x018)
#define rCLK_SRC				(SYSCON_BASE+0x01c)
#define rCLK_SRC2			(SYSCON_BASE+0x10c)
#define rCLK_DIV0			(SYSCON_BASE+0x020)
#define rCLK_DIV1			(SYSCON_BASE+0x024)
#define rCLK_DIV2			(SYSCON_BASE+0x028)
#define rCLK_OUT				(SYSCON_BASE+0x02c)
#define rHCLK_GATE			(SYSCON_BASE+0x030)
#define rPCLK_GATE			(SYSCON_BASE+0x034)
#define rSCLK_GATE			(SYSCON_BASE+0x038)
#define rMEM0_CLK_GATE		(SYSCON_BASE+0x03c)

//
#define rAHB_CON0			(SYSCON_BASE+0x100)
#define rAHB_CON1			(SYSCON_BASE+0x104)
#define rAHB_CON2			(SYSCON_BASE+0x108)
#define rSDMA_SEL			(SYSCON_BASE+0x110)
#define rSW_RST				(SYSCON_BASE+0x114)
#define rSYS_ID				(SYSCON_BASE+0x118)
#define rMEM_SYS_CFG		(SYSCON_BASE+0x120)
#define rQOS_OVERRIDE0		(SYSCON_BASE+0x124)
#define rQOS_OVERRIDE1		(SYSCON_BASE+0x128)
#define rMEM_CFG_STAT		(SYSCON_BASE+0x12c)
//
#define rPWR_CFG			(SYSCON_BASE+0x804)
#define rEINT_MASK			(SYSCON_BASE+0x808)
#define rNORMAL_CFG			(SYSCON_BASE+0x810)
#define rSTOP_CFG			(SYSCON_BASE+0x814)
#define rSLEEP_CFG			(SYSCON_BASE+0x818)
#define rSTOP_MEM_CFG		(SYSCON_BASE+0x81C)
#define rOSC_FREQ			(SYSCON_BASE+0x820)
#define rOSC_STABLE			(SYSCON_BASE+0x824)
#define rPWR_STABLE			(SYSCON_BASE+0x828)
#define rFPC_STABLE			(SYSCON_BASE+0x82c)
#define rMTC_STABLE			(SYSCON_BASE+0x830)
#define rBUS_CACHEABLE_CON	(SYSCON_BASE+0x838)

// 
#define rOTHERS				(SYSCON_BASE+0x900)
#define rRST_STAT			(SYSCON_BASE+0x904)
#define rWAKEUP_STAT		(SYSCON_BASE+0x908)
#define rBLK_PWR_STAT		(SYSCON_BASE+0x90c)
#define rINFORM0			(SYSCON_BASE+0xA00)
#define rINFORM1			(SYSCON_BASE+0xA04)
#define rINFORM2			(SYSCON_BASE+0xA08)
#define rINFORM3			(SYSCON_BASE+0xA0c)
#define rINFORM4			(SYSCON_BASE+0xA10)
#define rINFORM5			(SYSCON_BASE+0xA14)
#define rINFORM6			(SYSCON_BASE+0xA18)
#define rINFORM7			(SYSCON_BASE+0xA1c)


u8	g_System_Revision, g_System_Pass, g_SYNCACK;
u32	g_APLL, g_MPLL, g_ARMCLK, g_HCLKx2, g_HCLK, g_PCLK;




//////////
// Function Name : SYSC_ReadSystemID
// Function Description : This function reads system ID register
// Input : NONE 
// Output : NONE
// Version : 
void SYSC_ReadSystemID(void)
{
	u32 temp;

	temp = Inp32(rSYS_ID);

	g_System_Revision	=	(temp>>4)&0xf;
	g_System_Pass 	=	(temp)&0xf;

	return;
}

//////////
// Function Name : SYSC_GetClkInform
// Function Description : This function gets common clock information
// Input : NONE 
// Output : NONE
// Version : 
void SYSC_GetClkInform( void)
{
	u8 muxApll, muxMpll, muxSync;
	u8 divApll, divHclkx2, divHclk, divPclk;
	u16 pllM, pllP, pllS;
	u32 temp;
	
	////
	// clock division ratio	
	temp = Inp32(rCLK_DIV0);
	divApll = temp & 0xf;
	divHclkx2 = (temp>>9) & 0x7;
	divHclk = (temp>>8) & 0x1;
	divPclk = (temp>>12) & 0xf;

	////
	// Operating Mode
	temp = Inp32(rOTHERS);
	temp = (temp>>8)&0xf;
	if(temp)
	{
		g_SYNCACK = 1;
	}
	else
	{
		g_SYNCACK = 0;
	}
	
	////
	// ARMCLK
	muxApll = Inp32(rCLK_SRC) & 0x1;
	if(muxApll)	//FOUT
	{			
		temp = Inp32(rAPLL_CON);
		pllM = (temp>>16)&0x3ff;
		pllP = (temp>>8)&0x3f;
		pllS = (temp&0x7);

		g_APLL = ((FIN>>pllS)/pllP)*pllM;
	}
	else	//FIN
	{
		g_APLL = FIN;
	}
	
	g_ARMCLK = g_APLL/(divApll+1);
	
	////
	// HCLK
	muxSync = (Inp32(rOTHERS)>>7) & 0x1;
	if(muxSync)	//synchronous mode
	{
		g_HCLKx2 = g_APLL/(divHclkx2+1);
		
		temp = Inp32(rMPLL_CON);
		pllM = (temp>>16)&0x3ff;
		pllP = (temp>>8)&0x3f;
		pllS = (temp&0x7);

		g_MPLL = ((FIN>>pllS)/pllP)*pllM;
	}
	else
	{
		muxMpll = (Inp32(rCLK_SRC)>>1) & 0x1;
		if(muxMpll)	//FOUT
		{						
			temp = Inp32(rMPLL_CON);
			pllM = (temp>>16)&0x3ff;
			pllP = (temp>>8)&0x3f;
			pllS = (temp&0x7);

			g_MPLL = ((FIN>>pllS)/pllP)*pllM;
		}
		else	//FIN
		{
			g_MPLL = FIN;
		}
		g_HCLKx2 = g_MPLL/(divHclkx2+1);		
	}
	
	g_HCLK = g_HCLKx2/(divHclk+1);
	
	////
	// PCLK
	g_PCLK = g_HCLKx2/(divPclk+1);

	return;
	
}


//////////
// Function Name : SYSC_SetPLL
// Function Description : This function control PLL Output Frequency (APLL, MPLL:PLL9025X, EPLL:PLL9024X)
//				  Fout = (mdiv * Fin) / (pdiv x 2^sdiv),   Fout = ((mdiv+k/2^16) * Fin) / (pdiv x 2^sdiv)
// Input : 				ePLL  : APLL, MPLL, EPLL	
//						uMdiv : Mdiv Value ( 56 ~ 1023), ( 13 ~ 255)
//						uPdiv :  Pdiv Value ( 1~63)
//						uSdiv : Sdiv Value ( 0~5 )
//						uKdiv : PLL9025X (Not Used, 0), PLL9024X(0~65535)
// Output : NONE
// Version : 0.1 by boaz
void SYSC_SetPLL(PLL_eTYPE  ePLL, u32 uMdiv, u32 uPdiv, u32 uSdiv, u32 uKdiv)
{
	u32 temp, uRegValue;


	switch(ePLL)
	{
		case eAPLL: 
			// Check the Divider Value.
			if( uMdiv < 64 || uMdiv > 1023)
			{
				UART_Printf(" Wrong  Mdiv Value, Correct Value Range = (56 ~ 1023) (%d)\n", uMdiv);
			}
			if( uPdiv < 1 || uPdiv > 63)
			{
				UART_Printf(" Wrong  Pdiv Value, Correct Value Range = (1 ~ 63) (%d)\n", uPdiv);
			}
			if( uSdiv > 5)
			{
				UART_Printf(" Wrong  Sdiv Value, Correct Value Range = (0 ~ 5) (%d)\n", uSdiv);
			}
      			// Check the Fvco Range

      			temp = ((FIN/uPdiv)*uMdiv)/1000000;
			if( temp <800 || temp > 1600)
			{

				UART_Printf(" Please select the proper M,P,S divider value\n");
				UART_Printf(" Fvco Range = (800MHz ~ 1600MHz),  Current Value is (%d)MHz\n", temp);
			}

			uRegValue = (u32)(((u32)(0x1<<31))|(uMdiv<<16)|(uPdiv<<8)|(uSdiv<<0));
			Outp32(rAPLL_CON, uRegValue);
			break;

		case eMPLL: 
			// Check the Divider Value.
			if( uMdiv < 64 || uMdiv > 1023)
			{
				UART_Printf(" Wrong  Mdiv Value, Correct Value Range = (56 ~ 1023) (%d)\n", uMdiv);
			}
			if( uPdiv < 1 || uPdiv > 63)
			{
				UART_Printf(" Wrong  Pdiv Value, Correct Value Range = (1 ~ 63) (%d)\n", uPdiv);
			}
			if( uSdiv > 5)
			{
				UART_Printf(" Wrong  Sdiv Value, Correct Value Range = (0 ~ 5) (%d)\n", uSdiv);
			}
      			// Check the Fvco Range

      			temp = ((FIN/uPdiv)*uMdiv)/1000000;
			if( temp <800 || temp > 1600)
			{
				UART_Printf(" Please select the proper M,P,S divider value\n");
				UART_Printf(" Fvco Range = (800MHz ~ 1600MHz),  Current Value is (%d)MHz\n", temp);
			}

			uRegValue = (u32)(((u32)(0x1<<31))|(uMdiv<<16)|(uPdiv<<8)|(uSdiv<<0));
			Outp32(rMPLL_CON, uRegValue);	
			break;

		case eEPLL: 
			// Check the Divider Value.
			if( uMdiv < 16 || uMdiv > 255)
			{
				UART_Printf(" Wrong  Mdiv Value, Correct Value Range = (56 ~ 1023) (%d)\n", uMdiv);
			}
			if( uPdiv < 1 || uPdiv > 63)
			{
				UART_Printf(" Wrong  Pdiv Value, Correct Value Range = (1 ~ 63) (%d)\n", uPdiv);
			}
			if( uSdiv > 5)
			{
				UART_Printf(" Wrong  Sdiv Value, Correct Value Range = (0 ~ 5) (%d)\n", uSdiv);
			}
			if( uKdiv >65535)
			{
				UART_Printf(" Wrong  Kdiv Value, Correct Value Range = (0 ~ 65535) (%d)\n", uKdiv);
			}
		      // Check the Fvco Range

		       temp = (FIN/uPdiv)*(uMdiv+(uKdiv/(2^16)))/1000000;
			if( temp <300 || temp > 600)
			{
				UART_Printf(" Please select the proper M,P,S divider value\n");
				UART_Printf(" Fvco Range = (300MHz ~ 600MHz),  Current Value is (%d)MHz\n", temp);
			}

			Outp32(rEPLL_CON1, uKdiv);
			uRegValue = (u32)(((u32)(0x1<<31))|(uMdiv<<16)|(uPdiv<<8)|(uSdiv<<0));
			Outp32(rEPLL_CON0, uRegValue);
			break;
		}

	// Get Information
	SYSC_GetClkInform();
}

//////////
// Function Name : SYSC_StopPLL
// Function Description : This function stop PLL.
//				  Fout = (mdiv * Fin) / (pdiv x 2^sdiv),   Fout = ((mdiv+k/2^16) * Fin) / (pdiv x 2^sdiv)
// Input : 				ePLL  : eAPLL,eMPLL, eEPLL
//
//						
// Output : NONE
// Version : 
void SYSC_StopPLL(PLL_eTYPE  ePLL)
{
	u32  uRegValue;


	switch(ePLL)
	{
		case eAPLL: 

			uRegValue = Inp32(rAPLL_CON);
			uRegValue = (uRegValue & ~(0x1<<31)); 
			Outp32(rAPLL_CON, uRegValue);
			break;

		case eMPLL: 
			uRegValue = Inp32(rMPLL_CON);
			uRegValue = (uRegValue & ~(0x1<<31)); 
			Outp32(rMPLL_CON, uRegValue);	
			break;

		case eEPLL: 
			uRegValue = Inp32(rEPLL_CON0);
			uRegValue = (uRegValue & ~(0x1<<31)); 
			Outp32(rEPLL_CON0, uRegValue);
			break;
		}

	
}

/////////
// Function Name : SYSC_RdLockDetect
// Function Description : This function reads Lock Detect bit of the A/MPLL (Hidden)
// Input : 		ePLL  : eAPLL, eMPLL
// Output :            TRUE/FALSE
// Version : 

u8  SYSC_RdLockDetect(PLL_eTYPE  ePLL)
{
	u32 uRegValue, uLockDetect;

	switch(ePLL)
	{
		case eAPLL: 

			uRegValue = Inp32(rAPLL_CON);
			uLockDetect = (uRegValue >>30)&(0x1); 
			break;

		case eMPLL: 
			uRegValue = Inp32(rMPLL_CON);
			uLockDetect = (uRegValue >>30)&(0x1); 
			break;

	}

	if(uLockDetect == 1)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}

	
}

//////////
// Function Name : SYSC_ChangeSYSCLK
// Function Description : This function control System Clock  
//				  Fout = (mdiv * Fin) / (pdiv x 2^sdiv),   Fout = ((mdiv+k/2^16) * Fin) / (pdiv x 2^sdiv)
// Input : 				eAPLLMPS  : APLL M,P,S Value
//						eMPLLMPS  : MPLL M,P,S Value  ( using at Async Mode )
//						uARM_Ratio : ARM Divider
//						uHCLKx2_Ratio : HCLKx2 Divider
//						uPCLK_Ratio : PCLK Divider
//						Other values are retained
// Output : NONE
// Version : 
void SYSC_ChangeSYSCLK(APLL_eOUT  eAPLLMPS, APLL_eOUT eMPLLMPS, u32 uARM_Ratio, u32 uHCLKx2_Ratio, u32 uPCLK_Ratio)
{
	u32 uRegValue, uRegValue_A;
	u32 uRegValue_M;//, uRegValue_E;
	u32 uMVAL, uPVAL, uSVAL;
	u32 uMVAL_M, uPVAL_M, uSVAL_M;
	u32 uTemp;
	
	uMVAL = (eAPLLMPS&0x3FF0000)>>16;
	uPVAL = (eAPLLMPS&0xFF00)>>8;
	uSVAL = eAPLLMPS&0x00FF;
	uRegValue_A = (u32)(((u32)(0x1<<31))|(uMVAL<<16)|(uPVAL<<8)|(uSVAL<<0));

	uMVAL_M = (eMPLLMPS&0x3FF0000)>>16;
	uPVAL_M = (eMPLLMPS&0xFF00)>>8;
	uSVAL_M = eMPLLMPS&0x00FF;


	uTemp = (Inp32(rOTHERS)>>8)&0xF;


	if(uTemp==0xF )		// Sync Mode
	{
	
		uRegValue_M = Inp32(rMPLL_CON);

	}
	else			// ASync Mode
	{
		uRegValue_M = (u32)(((u32)(0x1<<31))|(uMVAL_M<<16)|(uPVAL_M<<8)|(uSVAL_M<<0));
		//uRegValue_E = Inp32(rEPLL_CON0);
	}
	
	
	// Divider Change
	uRegValue = Inp32(rCLK_DIV0);
	uRegValue = (uRegValue & ~(0xFE07)) | ((uPCLK_Ratio<<12)|(uHCLKx2_Ratio<<9)|(uARM_Ratio<<0));
	Outp32(rCLK_DIV0, uRegValue);

#if 0	
	// EVT0 W.A 
	Outp32(rMPLL_CON, uRegValue_M);

	if(uTemp == 0)
	{
		Outp32(rEPLL_CON0, uRegValue_E);
	}
	// APLL Set
#endif

	Outp32(rAPLL_CON, uRegValue_A);
	Delay(10);
	Outp32(rMPLL_CON, uRegValue_M);
	Delay(10);

	
	SYSC_GetClkInform();
	UART_InitDebugCh(0, 115200);
	Delay(10);

	UART_Printf("============================ \n");
	UART_Printf("Current Clock Information \n");
	UART_Printf("ARMCLK: %.2fMHz  HCLKx2: %.2fMHz  HCLK: %.2fMHz  PCLK: %.2fMHz\n",(float)g_ARMCLK/1.0e6, (float)g_HCLKx2/1.0e6, (float)g_HCLK/1.0e6, (float)g_PCLK/1.0e6);
	UART_Printf("============================ \n");
	

}



//////////
// Function Name : SYSC_ChangeSYSCLK_1
// Function Description : This function control System Clock  
//				  Fout = (mdiv * Fin) / (pdiv x 2^sdiv),   Fout = ((mdiv+k/2^16) * Fin) / (pdiv x 2^sdiv)
// Input : 				eAPLLMPS  : APLL M,P,S Value
//						eMPLLMPS  : MPLL M,P,S Value  ( using at Async Mode )
//						uARM_Ratio : ARM Divider
//						uHCLKx2_Ratio : HCLKx2 Divider
//						uPCLK_Ratio : PCLK Divider
//						Other values are retained
// Output : NONE
// Version : 
void SYSC_ChangeSYSCLK_1(APLL_eOUT  eAPLLMPS, APLL_eOUT eMPLLMPS, u32 uARM_Ratio, u32 uHCLKx2_Ratio, u32 uPCLK_Ratio)
{
	u32 uRegValue_A;
	u32 uRegValue_M;//, uRegValue_E;
	u32 uMVAL, uPVAL, uSVAL;
	u32 uMVAL_M, uPVAL_M, uSVAL_M;
	u32 uSYNCMODE;
	u32 uRegValue_DIV0, uRegValue_SRC, uTemp, uTemp1;
	
	uMVAL = (eAPLLMPS&0x3FF0000)>>16;
	uPVAL = (eAPLLMPS&0xFF00)>>8;
	uSVAL = eAPLLMPS&0x00FF;
	uRegValue_A = (u32)(((u32)(0x1<<31))|(uMVAL<<16)|(uPVAL<<8)|(uSVAL<<0));

	uMVAL_M = (eMPLLMPS&0x3FF0000)>>16;
	uPVAL_M = (eMPLLMPS&0xFF00)>>8;
	uSVAL_M = eMPLLMPS&0x00FF;


	uSYNCMODE = (Inp32(rOTHERS)>>8)&0xF;


	if(uSYNCMODE==0xF )		// Sync Mode
	{
	
		uRegValue_M = Inp32(rMPLL_CON);

	}
	else			// ASync Mode
	{
		uRegValue_M = (u32)(((u32)(0x1<<31))|(uMVAL_M<<16)|(uPVAL_M<<8)|(uSVAL_M<<0));
		//uRegValue_E = Inp32(rEPLL_CON0);
	}


	// Clock Divider Change 1:2:2
	uRegValue_DIV0 = Inp32(rCLK_DIV0);
	uTemp = (uRegValue_DIV0&~(0xFFFF))|(1<<12)|((0<<9)|(1<<8)|(1<<4)|(0<<0));		// ARM:HCLKx2:HCLK:PCLK = 1:1:2:2

	// Set-up Divider Value
	uRegValue_DIV0 = (uRegValue_DIV0 & ~(0xFE07)) | ((uPCLK_Ratio<<12)|(uHCLKx2_Ratio<<9)|(uARM_Ratio<<0));

	uRegValue_SRC = Inp32(rCLK_SRC);
	uTemp1 = (uRegValue_SRC&~(0x7))|0x0;

	Outp32(rCLK_SRC, uTemp1);	// Clock Source Change
	Outp32(rCLK_DIV0, uTemp);	// Change Clock Divider
	
	
	// PLL Change
	if(uSYNCMODE == 0)
	{
		Outp32(rMPLL_CON, uRegValue_M);
	}

	Outp32(rAPLL_CON, uRegValue_A);

	while(((Inp32(rAPLL_CON)>>30)&0x1)==0);


	// Change Divider Value
	Outp32(rCLK_DIV0, uRegValue_DIV0);
	// Change Clock SRC
	Outp32(rCLK_SRC, uRegValue_SRC);

	
	SYSC_GetClkInform();
	UART_InitDebugCh(0, 115200);
	Delay(10);

//	UART_Printf("============================ \n");
//	UART_Printf("Current Clock Information \n");
//	UART_Printf("ARMCLK: %.2fMHz  HCLKx2: %.2fMHz  HCLK: %.2fMHz  PCLK: %.2fMHz\n",(float)g_ARMCLK/1.0e6, (float)g_HCLKx2/1.0e6, (float)g_HCLK/1.0e6, (float)g_PCLK/1.0e6);
//	UART_Printf("============================ \n");
	

}



#define uTIME_OF_XTAL	(1.0e6/(float)FIN)	

//////////
// Function Name : SYSC_SetLockTime
// Function Description : This function control APLL Lock Time (PLL9025X), Max 100us
//				  Fout = (mdiv * Fin) / (pdiv x 2^sdiv)
// Input : 				uLockTime : Max 100us, us order
//						ePLL		  : PLL Type
// Output : NONE
// Version : 
void SYSC_SetLockTime( PLL_eTYPE  ePLL, u32 uLockTime)
{
	u32 uLockCount;
	
	uLockCount = (unsigned int)((float)uLockTime/((float)uTIME_OF_XTAL));
//	UART_Printf("uLockCount : 0x%x\n", uLockCount);

	switch(ePLL)
	{
		case eAPLL: 
			Outp32(rAPLL_LOCK, uLockCount);  break;
		case eMPLL: 
			Outp32(rMPLL_LOCK, uLockCount); break;
		case eEPLL: 
			Outp32(rEPLL_LOCK, uLockCount); break;
					
	}

}

//boaz.kim
//////////
// Function Name : SYSC_ClkSrc
// Function Description : This function select Clock Source
//				 
// Input : 		  CLKSRC_eId
// Output : NONE
// Version : 
void SYSC_ClkSrc( CLKSRC_eId  eClkSrc)
{
	u32 uReg, uCtrl, uOffset, uFuct;
	u32 uRegValue;

	uReg = (eClkSrc&0x10000)>>16;
	uCtrl = (eClkSrc&0xF000)>>12;
	uOffset = (eClkSrc&0x0FF0)>>4;
	uFuct = eClkSrc&0x000F;

	if(uReg==0)
	{
		uRegValue = Inp32(rCLK_SRC);
		uRegValue = (uRegValue & ~(uCtrl<<(uOffset))) | (uFuct<<(uOffset));
		Outp32(rCLK_SRC, uRegValue);		
	}
	else if(uReg==1)
	{
		uRegValue = Inp32(rCLK_SRC2);
		uRegValue = (uRegValue & ~(uCtrl<<(uOffset))) | (uFuct<<(uOffset));
		Outp32(rCLK_SRC2, uRegValue);		
	}
	else Assert(0);
		

}

//////////
// Function Name : SYSC_SetDIV
// Function Description : This function set Clock divider0 register ratio
//				 
// Input : CLKDIV_eId, division value
//
// Output : NONE
// Version : 
void SYSC_SetDIV( CLKDIV_eId eClkDiv, u32 uDivVal )
{
	u32 uReg, uCtrl, uOffset;
	u32 uRegValue;

	uReg = (eClkDiv&0xF000)>>12;
	uCtrl = (eClkDiv&0xF00)>>8;
	uOffset = (eClkDiv&0xFF);


	if(uReg==0)
	{
		uRegValue = Inp32(rCLK_DIV0);
		uRegValue = (uRegValue & ~(uCtrl<<(uOffset))) | (uDivVal<<(uOffset));
		Outp32(rCLK_DIV0, uRegValue);		
	}
	else if(uReg==1)
	{
		uRegValue = Inp32(rCLK_DIV1);
		uRegValue = (uRegValue & ~(uCtrl<<(uOffset))) | (uDivVal<<(uOffset));
		Outp32(rCLK_DIV1, uRegValue);				
	}
	else if(uReg==2)
	{
		uRegValue = Inp32(rCLK_DIV2);
		uRegValue = (uRegValue & ~(uCtrl<<(uOffset))) | (uDivVal<<(uOffset));
		Outp32(rCLK_DIV2, uRegValue);				
	}
	else Assert(0);
	

}


//////////
// Function Name : SYSC_SetDIV0
// Function Description : This function set Clock divider0 register ratio
//				 
// Input : 		  	uAratio 	: DIV_APLL ( 0 ~ 15) 
//					uMratio 	: DIV_MPLL ( 0~1)
//					uHratio 	: DIV_HCLK (0~1),  HCLKx2
//					uHx2ratio: DIV_HCLKx2(0~7), (APLL or MPLL)
//					uPratio	: DIV_PCLK (0~15),  HCLKx2
//					uONDratio: DIV_ONDCLK(0~3), HCLKx2
//					uSECUratio:DIV_Security(0~3), HCLKx2
//					uCAMratio: DIV_CAM(0~15), HCLKx2
//					uJPEGratio: DIV_JPEG(0~15), HCLKx2
//					uMFCratio: DIV_MFC(0~15), HCLKx2
// Output : NONE
// Version : 
void SYSC_SetDIV0( u32 uAratio, u32 uMratio, u32 uHratio, u32 uHx2ratio, u32 uPratio, 
	u32 uONDratio, u32 uSECUratio, u32 uCAMratio, u32 uJPEGratio, u32 uMFCratio )
{
	u32 uRegValue;

	uRegValue =
		( (uMFCratio<<28)|		// HCLKx2/(uMFCratio + 1)
		(uJPEGratio<<24) |		// HCLKx2/(uJPEGratio + 1)
		(uCAMratio<<20) |		// HCLKx2/(uCAMratio + 1)
		(uSECUratio<<18)|		// HCLKx2/(uSECUratio + 1)
		(uONDratio<<16) |		// HCLKx2/(uONDratio +1 )
		(uPratio<<12) |			// HCLKx2/(uPratio+1)
		(uHx2ratio<<9) |
		(uHratio<<8)|			// HCLKx2(uHratio+1)
		(uMratio<<4)|
		(uAratio<<0)) ;


	Outp32(rCLK_DIV0, uRegValue);

}

//////////
// Function Name : SYSC_SetDIV0_all
// Function Description : This function set Clock divider0 register ratio
// Input : 	None
// Version : 
// add by rb1004
void SYSC_SetDIV0_all(u32 uSetRatio)
{
	Outp32(rCLK_DIV0, uSetRatio);
}


//////////
// Function Name : SYSC_GetDIV0
// Function Description : This function get Clock divider0 register ratio
// Input : 	None
// Version : 
// add by rb1004
u32 SYSC_GetDIV0( void)
{
	return  Inp32(rCLK_DIV0);
}





//////////
// Function Name : SYSC_SetDIV1
// Function Description : This function set Clock divider1 register ratio
//				 
// Input : 		  	uMMC0ratio 	: (0~15)
//					uMMC1ratio	: (0~15)
//					uMMC2ratio	: (0~15)
//					uLCDratio	: (0~15)
//					uSCALERratio : (0~15)
//					uHOSTratio	: (0~15)
// Output : NONE
// Version : 
void SYSC_SetDIV1( u32 uMMC0ratio, u32 uMMC1ratio, u32 uMMC2ratio, u32 uLCDratio, u32 uSCALERratio, u32 uHOSTratio) 
{
	u32 uRegValue;

	uRegValue =
		((uHOSTratio<<20) |		
		(uSCALERratio<<16) |			
		(uLCDratio<<12) |
		(uMMC2ratio<<8)|			
		(uMMC1ratio<<4)|
		(uMMC0ratio<<0)) ;

	Outp32(rCLK_DIV1, uRegValue);

}

//////////
// Function Name : SYSC_SetDIV2
// Function Description : This function set Clock divider2 register ratio
//				 
// Input : 		  	uSPI0ratio 	: (0~15)
//					uSPI1ratio	: (0~15)
//					uAUDIO0ratio	: (0~15)
//					uAUDIO1ratio	: (0~15)
//					uUARTratio : (0~15)
//					uIRDAratio	: (0~15)
// Output : NONE
// Version : 
void SYSC_SetDIV2( u32 uSPI0ratio	, u32 uSPI1ratio, u32 uAUDIO0ratio, u32 uAUDIO1ratio, u32 uUARTratio, u32 uIRDAratio) 
{
	u32 uRegValue;

	uRegValue =
		((uIRDAratio<<20) |		
		(uUARTratio<<16) |			
		(uAUDIO1ratio<<12) |
		(uAUDIO0ratio<<8)|			
		(uSPI1ratio<<4)|
		(uSPI0ratio<<0)) ;

	Outp32(rCLK_DIV2, uRegValue);

}


//////////
// Function Name : SYSC_CtrlCLKGate
// Function Description : This function control HCLK_GATE Register
//				 
// Input : 		  	eGATE
//					uCtrl      :  Enable_CLK : (1), 
//							  Disable_CLK : (0)
// Output : NONE
// Version : 
void SYSC_CtrlCLKGate( CLOCK_eGATE eGate,  u32 uCtrl)
{
	u32 uRegValue, uTemp;

	uTemp = eGate;

	
	uRegValue =Inp32(rHCLK_GATE);
	uRegValue = (uRegValue & ~(0x1<<(uTemp))) | (uCtrl<<(uTemp));
	Outp32(rHCLK_GATE, uRegValue);

}


//////////
// Function Name : SYSC_CtrlHCLKGate
// Function Description : This function control HCLK_GATE Register
//				 
// Input : 		  	H_eGATE
//					uCtrl      :  Enable_CLK : (1), 
//							  Disable_CLK : (0)
// Output : NONE
// Version : 
void SYSC_CtrlHCLKGate( H_eGATE eHCLKGATE	, u32 uCtrl)
{
	u32 uRegValue, uTemp;

	uTemp = eHCLKGATE;

	
	uRegValue =Inp32(rHCLK_GATE);
	uRegValue = (uRegValue & ~(0x1<<(uTemp))) | (uCtrl<<(uTemp));
	Outp32(rHCLK_GATE, uRegValue);
}

//////////
// Function Name : SYSC_CtrlPCLKGate
// Function Description : This function control PCLK_GATE Register
//				 
// Input : 		  	P_eGATE
//					uCtrl      :  Enable_CLK : (1), 
//							  Disable_CLK : (0)
// Output : NONE
// Version : 
void SYSC_CtrlPCLKGate( P_eGATE ePCLKGATE	, u32 uCtrl)
{
	u32 uRegValue, uTemp;

	uTemp = ePCLKGATE;

	
	uRegValue =Inp32(rPCLK_GATE);
	uRegValue = (uRegValue & ~(0x1<<(uTemp))) | (uCtrl<<(uTemp));
	Outp32(rPCLK_GATE, uRegValue);

}

//////////
// Function Name : SYSC_CtrlSCLKGate
// Function Description : This function control SCLK_GATE Register
//				 
// Input : 		  	S_eGATE
//					uCtrl      :  Enable_CLK : (1), 
//							  Disable_CLK : (0)
// Output : NONE
// Version : 
void SYSC_CtrlSCLKGate( S_eGATE eSCLKGATE	, u32 uCtrl)
{
	u32 uRegValue, uTemp;

	uTemp = eSCLKGATE;

	
	uRegValue =Inp32(rSCLK_GATE);
	uRegValue = (uRegValue & ~(0x1<<(uTemp))) | (uCtrl<<(uTemp));
	Outp32(rSCLK_GATE, uRegValue);

}

//////////
// Function Name : SYSC_SetCLKGateAll
// Function Description : This function control SCLK_GATE Register
//				 
// Input : NONE
// Output : NONE
// Version : 

void SYSC_SetCLKGateAll(u32 uOnOff)
{
	if(uOnOff==1) {
		Outp32(rPCLK_GATE, 0xffffffff);
		Outp32(rSCLK_GATE, 0xffffffff);
		Outp32(rHCLK_GATE, 0xffffffff);
	}	
	else if(uOnOff==0) {
		Outp32(rPCLK_GATE, 0x0);
		Outp32(rSCLK_GATE, 0x0);
		Outp32(rHCLK_GATE, 0x0);
	}	
		
}



//////////
// Function Name : SYSC_CtrlCLKOUT
// Function Description : This function select CLK output 
//				 
// Input : 		  	CLKOUT_eTYPE :  FOUT_APLL, FOUT_EPLL, HCLK, CLK27M, CLK48M, RTC, Tick, DOUT
//					uDivVAL     :  (0~15)
//							  
// Output : NONE
// Version : 
void SYSC_CtrlCLKOUT( CLKOUT_eTYPE eCLKOUT_TYPE, u32 uDivVAL)
{
	u32 uRegValue;
    u32 uFunct;

        
    GPIO_SetFunctionEach(eGPIO_F, eGPIO_14, 3);	// Select GPIO_SELECT
    GPIO_SetPullUpDownEach(eGPIO_F, eGPIO_14, 0);
       	
    uFunct = eCLKOUT_TYPE;
	uRegValue =Inp32(rCLK_OUT);
	uRegValue = (uRegValue & ~(0xFF<<12)) | ((uFunct<<12)|(uDivVAL<<16));
	Outp32(rCLK_OUT, uRegValue);

}


//////////
// Function Name : SYSC_CtrlDCLK
// Function Description : This function control  DCLK output 
//				 
// Input : 		  	uDCLKCMP :(0~15) DCLK compare value, clock toggle value, 
//					uDCLKDIV : (0~15) DCLK divid value,  F_DCLK= Src_CLK/(uDCLKDIV+1)
//					uDCLKSEL :  "0" PCLK,  "1" 48MHz
//					uDCLKEN   : Enable_CLK : (1), 
//							    Disable_CLK : (0)
// Output : NONE
// Version : 
void SYSC_CtrlDCLK( u32 uDCLKCMP, u32 uDCLKDIV, u32 uDCLKSEL, u32 uDCLKEN)
{
	u32 uRegValue;
		
	uRegValue =Inp32(rCLK_OUT);
	uRegValue = (uRegValue & ~(0xFFF<<0)) | ((uDCLKCMP<<8)|(uDCLKDIV<<4)|(uDCLKSEL<<1)|(uDCLKEN<<0));
	Outp32(rCLK_OUT, uRegValue);

}

	// Value & 0xF00 : The BUS Control Register ID, OffSet
	// Value & 0x0F0 : The BUS Priority Type ID
	// Value & 0x00F : The Fixed Priority Order


//////////
// Function Name : SYSC_CtrlBUSPrio
// Function Description : This function control AHB Bus Priority
//				 
// Input : 		  BUSCTRL_eId
// Output : NONE
// Version : 
void SYSC_CtrlBUSPrio( BUSCTRL_eId  eBusId)
{
	u32 uOffset, uPType, uForder, uOffset1;
	u32 uRegValue;

	uOffset = (eBusId&0xF00)>>8;
	uPType = (eBusId&0x0F0)>>4;
	uForder = eBusId&0x00F;

	if(uOffset <= 3)
	{
		uOffset1 = uOffset;
		uRegValue = Inp32(rAHB_CON0);
		uRegValue = (uRegValue & ~(0xFF<<(uOffset1*7))) | ((uPType<<(uOffset1*7 + 4))|(uForder<<(uOffset1*7)));
		Outp32(rAHB_CON0, uRegValue);
	}
	else if(uOffset >= 4 && uOffset <= 7)
	{
		uOffset1 = uOffset - 4;
		uRegValue = Inp32(rAHB_CON1);
		uRegValue = (uRegValue & ~(0xFF<<(uOffset1*7))) | ((uPType<<(uOffset1*7 + 4))|(uForder<<(uOffset1*7)));
		Outp32(rAHB_CON1, uRegValue);
	}
	else if(uOffset >= 8 && uOffset <= 9)
	{
		uOffset1 = uOffset - 8;
		uRegValue = Inp32(rAHB_CON2);
		uRegValue = (uRegValue & ~(0xFF<<(uOffset1*7))) | ((uPType<<(uOffset1*7 + 4))|(uForder<<(uOffset1*7)));
		Outp32(rAHB_CON2, uRegValue);
	}

}



//////////
// Function Name : SYSC_CtrlBUSLock
// Function Description : This function control HLOCK for each bus
//				 
// Input : 		  
// Output : NONE
// Version : 


//  TBD



//////////
// Function Name : SYSC_SeletDMA
// Function Description : This function select between SDMA and General DMA
//				 
// Input : 		  	SEL_eSDMA : 
//					uSEL     :  #define		SEL_SDMA		(0)
//							 #define		SEL_GDMA		(1)
//							  
// Output : NONE
// Version : 
void SYSC_SelectDMA( DMASELECT_eID eSEL_DMA, u32 uSEL)
{
	u32 uRegValue;
	u32 uOffset;

	uOffset = eSEL_DMA;	
	uRegValue =Inp32(rSDMA_SEL);
	
	uRegValue = (uRegValue & ~(0x1<<uOffset)) | (uSEL<<uOffset);
	Outp32(rSDMA_SEL, uRegValue);

}



//////////
// Function Name : SYSC_CtrlEBIPrio
// Function Description : This function control EBI Priority
//				 
// Input : 		  EBIPrio_eID
// Output : NONE
// Version : 
void SYSC_CtrlEBIPrio( EBIPrio_eID  eEBIId)
{
	u32 uPType, uForder;
	u32 uRegValue;

	uPType = (eEBIId&0xF0)>>4;
	uForder = eEBIId&0x0F;

	uRegValue = Inp32(rMEM_SYS_CFG);
	uRegValue = (uRegValue & ~(0xF<<8)) | ((uPType<<11)|(uForder<<8));
	Outp32(rMEM_SYS_CFG, uRegValue);

}

//////////
// Function Name : SYSC_CtrlCSMEM0
// Function Description : This function control static memory chip selection muxing of MEM0
//				 
// Input : 		  			eM0CSn2 : eCS_SROMC, eCS_ONDC, eCS_NFC
//							eM0CSn3 : eCS_SROMC, eCS_ONDC, eCS_NFC
//							eM0CSn4 : eCS_SROMC, eCS_CFC
//							eM0CSn5 : eCS_SROMC, eCS_CFC
// Output : NONE
// Version : 
void SYSC_CtrlCSMEM0( SelCS_eMEM0  eM0CSn2, SelCS_eMEM0 eM0CSn3, SelCS_eMEM0 eM0CSn4, SelCS_eMEM0 eM0CSn5 )
{
	u32 uRegValue;

	uRegValue = Inp32(rMEM_SYS_CFG);
	uRegValue = (uRegValue & ~(0x3F<<0)) | ((eM0CSn5<<5)|(eM0CSn4<<4)|(eM0CSn3<<2)|(eM0CSn2<<0));
	Outp32(rMEM_SYS_CFG, uRegValue);

}


//////////
// Function Name : SYSC_SetQos
// Function Description : This function set QOS_OV_ID
//				 
// Input : 		  
// Output : NONE
// Version : 


//  TBD


//////////
// Function Name : SYSC_RdMEMCFG
// Function Description : This function read MEM_CFC_STAT Register
//				 
// Input :   NONE	  			
// Output : NONE
// Version : 
void SYSC_RdMEMCFGSTAT( void )
{
	u32 uRegValue;
	u32 uTemp;

	uRegValue = Inp32(rMEM_CFG_STAT);

	// EBI Priority Scheme
	uTemp = ( uRegValue >>15 ) & 0x1;
	UART_Printf(" Current EBI Priority Scheme 	(0: Fixed, 1: Circular) :    %d \n",  uTemp);
	uTemp = (uRegValue >> 12) & 0x7 ;
	UART_Printf(" Current EBI Fixed Priority setting					 :    %d \n",  uTemp);

	// CF I/F
	uTemp = ( uRegValue >>10 ) & 0x1;
	UART_Printf(" Current CF I/F Setting  (0: EBI, 1: Independet)	 :    %d \n",  uTemp);

	// NAND Type Setting
	uTemp = ( uRegValue >>9 ) & 0x1;
	UART_Printf(" Current NAND Type (0:OneNAND, 1: NAND)		 :    %d \n",  uTemp);
	uTemp = ( uRegValue >>3 ) & 0x1;
	UART_Printf(" Current NAND Init Setting  (0: Normal NAND, 1: Advanced NAND)	 :    %d \n",  uTemp);
	uTemp = ( uRegValue >>2 ) & 0x1;
	UART_Printf(" Show address cycle init. setting of NAND  	 :    %d \n",  uTemp);
	uTemp = ( uRegValue >>0 ) & 0x1;
	UART_Printf(" Show NAND Page Size	 :    %d \n",  uTemp);

	uTemp = ( uRegValue >>8 ) & 0x1;
	UART_Printf(" Current CS0 Bus Width  (0: 8-bit, 1: 16-bit)		 :    %d \n",  uTemp);
	uTemp = ( uRegValue >>1 ) & 0x1;
	UART_Printf(" Show CS0 Bus width (init. Setting)  (0:8bit, 1: 16bit)	 :    %d \n",  uTemp);

	uTemp = ( uRegValue >>7 ) & 0x1;
	UART_Printf(" NAND Booting   (0: not Used, 1: used)	 		:    %d \n",  uTemp);

	uTemp = ( uRegValue >>5 ) & 0x3;
	UART_Printf(" Current Booting Type  (0: NFCON, 1: SROMC, 2: ONDC,  3: Internal ROM)	 :    %d \n",  uTemp);

	uTemp = ( uRegValue >>4 ) & 0x1;
	UART_Printf(" Current ADDR Expand  (0: Used MEM1 Data, 1:Used MEM0 Addr)	 :    %d \n",  uTemp);


}


//////////
// Function Name : SYSC_RdMEMCFG
// Function Description : This function gets MEM_CFC_STAT Register
//				 
// Input :   NONE	  			
// Output : NONE
// rb1004
u32 SYSC_GetMEMCFGSTAT( void )
{
	return Inp32(rMEM_CFG_STAT);
}


//////////
// Function Name : SYSC_SetCFGWFI
// Function Description : This function configure ARM1176 STANBYWFI ( CFG_STANDBY (in PWR_CFG Register))
//				  ( CFG_STANDBYWFI & OSC Control in Power Mode )
// Input : 		  	eWFIMode : eIGNORE, eIDLE, eSTOP, eSLEEP
//					uOSCCLK     :  #define		Disable_CLK		(0)
//								 #define		Enable_CLK		(1)
//							  
// Output : NONE
// Version : 
void SYSC_SetCFGWFI( CFG_eWFI eWFIMode, u32 uOSCCLK)
{
	u32 uRegValue, uRegValue1;

	uRegValue =Inp32(rPWR_CFG);

	switch(eWFIMode)
	{
		case eIGNORE: 
			uRegValue = (uRegValue & ~(0x61<<0)) | ((eWFIMode<<5)|(1<<0));		// CFG_STANDBY= ignore,  OSC_27 = Enable
			Outp32(rPWR_CFG, uRegValue);  break;
		case eIDLE: 
			uRegValue = (uRegValue & ~(0x61<<0)) | ((eWFIMode<<5)|(uOSCCLK<<0));		
			Outp32(rPWR_CFG, uRegValue);  break;
		case eSTOP: 
			uRegValue = (uRegValue & ~(0x61<<0)) | ((eWFIMode<<5)|(uOSCCLK<<0));
			uRegValue1=Inp32(rSTOP_CFG);
			uRegValue1 = (uRegValue1 & ~(0x3<<0)) | ((uOSCCLK<<1)|(uOSCCLK<<0));
			//uRegValue1 = (uRegValue1 & ~(0x3<<0)) | ((uOSCCLK<<1)|(1<<0));
			Outp32(rPWR_CFG, uRegValue); 
			Outp32(rSTOP_CFG, uRegValue1); 
			break;
		case eSLEEP: 
			uRegValue = (uRegValue & ~(0x61<<0)) | ((eWFIMode<<5)|(uOSCCLK<<0));
			uRegValue1=Inp32(rSLEEP_CFG);
			uRegValue1 = (uRegValue1 & ~(0x61<<0)) | ((uOSCCLK<<0));
			Outp32(rPWR_CFG, uRegValue); 
			Outp32(rSLEEP_CFG, uRegValue1); 
			break;
								
	}

}

//////////
// Function Name : SYSC_SetBATF
// Function Description : This function configure BATFLT ( (in PWR_CFG Register))
//				  
// Input : 		  	eBATFLT : eFLT_IGNORE, eFLT_INT, eFLT_SLEEP
//					uINTSRC     :  (0) Wake-up Source in ESLEEP :  nWRESET
//								(1) Wake-up Source in ESLEEP :  SLEEP Wake-up Source
//		
//							  
// Output : NONE
// Version : 
void SYSC_SetBATF( BATFLT_eMODE eBATFLT, u32 uINTSRC)
{
	u32 uRegValue;

	uRegValue =Inp32(rPWR_CFG);

	switch(eBATFLT)
	{
		case eFLT_IGNORE: 
			uRegValue = (uRegValue & ~(0x1F<<0)) | ((eBATFLT<<3)|(1<<0));		
			Outp32(rPWR_CFG, uRegValue);  break;
		case eFLT_INT: 
			uRegValue = (uRegValue & ~(0x1F<<0)) | ((eBATFLT<<3)|(1<<0));		
			Outp32(rPWR_CFG, uRegValue);  break;
		case eFLT_SLEEP: 
			uRegValue = (uRegValue & ~(0x1F<<0)) | ((eBATFLT<<3)|(uINTSRC<<2)|(1<<0));
			Outp32(rPWR_CFG, uRegValue); 
			break;					
	}

}

//////////
// Function Name : SYSC_SetNORCFG
// Function Description : This function configure Power Control of the internal block in the Stop Mode 
//				  
// Input : 		  	uIROM :  Power Control of the IROM Sub Block (0: OFF, 1:ON)
//					uDM_ETM :  Power Control of the DOMAIN_ETM Sub Block (0: OFF, 1:ON)
//					uDM_S :  Power Control of the DOMAIN_S Sub Block (0: OFF, 1:ON)
//					uDM_F :  Power Control of the DOMAIN_F Sub Block (0: OFF, 1:ON)
//					uDM_P :  Power Control of the DOMAIN_P Sub Block (0: OFF, 1:ON)
//					uDM_I :  Power Control of the DOMAIN_I Sub Block (0: OFF, 1:ON)
//					uDM_SS :  Power Control of the DOMAIN_SS Sub Block (0: ON, 1:OFF)
//					uDM_G :  Power Control of the DOMAIN_G Sub Block (0: OFF, 1:ON)
//					uDM_V :  Power Control of the DOMAIN_V Sub Block (0: OFF, 1:ON)
// Output : NONE
// Version : 
void SYSC_SetNORCFG( u32 uIROM, u32 uDM_ETM, u32 uDM_S, u32 uDM_F, u32 uDM_P, u32 uDM_I, u32 uDM_SS, u32 uDM_G, u32 uDM_V)
{
	u32 uRegValue;

	uRegValue =Inp32(rNORMAL_CFG);
	uRegValue = (uRegValue & ~(1<<30|0xFF<<9)) | ((uIROM<<30)|(uDM_ETM<<16)|(uDM_S<<15)|(uDM_F<<14)|(uDM_P<<13)
		             |(uDM_I<<12)|(uDM_SS<<11)|(uDM_G<<10)|(uDM_V<<9));
	Outp32(rNORMAL_CFG, uRegValue); 
}


//////////
// Function Name : SYSC_SetSTOPCFG
// Function Description : This function configure Power Control of the internal block in the Stop Mode 
//				  
// Input : 		  	uMEM_ARM :  Power Control of the Memory Block in ARM Module (0: OFF, 1:ON)
//					uMEM_TOP :  Power Control of the Memory Block in Top Module (0: OFF, 1:ON)
//					uLogic_ARM :  Power Control of the Logic Block in ARM Module (0: OFF, 1:ON)
//					uLogic_ARM :  Power Control of the Logic Block in Top Module (0: OFF, 1:ON)		  
// Output : NONE
// Version : 
void SYSC_SetSTOPCFG( u32 uMEM_ARM, u32 uMEM_Top, u32 uLogic_ARM, u32 uLogic_Top)
{
	u32 uRegValue;

	uRegValue =Inp32(rSTOP_CFG);
#if 1 //original
	uRegValue = (uRegValue & ~(1<<29|1<<20|1<<17|1<<8|1)) | ((uMEM_ARM<<29)|(uMEM_Top<<20)|(uLogic_ARM<<17)|(uLogic_Top<<8)|(1));
#elif 0 // sub-block on
	uRegValue = (uRegValue & ~(0x3FFFFF<<8)) | ((uMEM_ARM<<29)|(uMEM_Top<<20)|(uLogic_ARM<<17)|(uLogic_Top<<8)|(0xfd<<9));
#else // all on
	uRegValue=0xffffffff;
#endif

	Outp32(rSTOP_CFG, uRegValue); 
}


//////////
// Function Name : SYSC_SetSTOPMEMCFG
// Function Description : This function configure Power Control of the internal memory block in the Stop Mode 
//				  
// Input : 		  	uMODEM : Power Control of the Memory Block in MODEM I/F (0: OFF, 1:RETENTION)
//					uHOST : Power Control of the Memory Block in HOST I/F (0: OFF, 1:RETENTION)
//					uOTG : Power Control of the Memory Block in OTG (0: OFF, 1:RETENTION)
//					uHSMMC : Power Control of the Memory Block in HSMMC (0: OFF, 1:RETENTION)
//					uIROM: Power Control of the Memory Block in IROM (0: OFF, 1:RETENTION)
//					uIRDA : Power Control of the Memory Block in IRDA (0: OFF, 1:RETENTION)
//					uNFCON : Power Control of the Memory Block in NFCON(Stepping Stone) (0: OFF, 1:RETENTION)
// Output : NONE
// Version : 
void SYSC_SetSTOPMEMCFG( u32 uMODEM, u32 uHOST, u32 uOTG, u32 uHSMMC, u32 uIROM, u32 uIRDA, u32 uNFCON)
{
	u32 uRegValue;

	uRegValue =Inp32(rSTOP_MEM_CFG);
	uRegValue = (uRegValue & ~(0x7F)) | ((uMODEM<<6)|(uHOST<<5)|(uOTG<<4)|(uHSMMC<<3)|(uIROM<<2)|(uIRDA<<1)|(uNFCON<<0));
	Outp32(rSTOP_MEM_CFG, uRegValue); 
}



//////////
// Function Name : SYSC_SetWakeMASK
// Function Description : This function control Wake-up source Mask  
//				  
// Input : 		  	WKUPMSK_eID : Wake-up Source ID
//					uMaskCtrl     :  (0) Mask disable , Wake-up source Enable
//								 (1) Mask Enable,  Wake-up source disable
//		
//							  
// Output : NONE
// Version : 
void SYSC_SetWakeMASK( WKUPMSK_eID eWKUPSRC, u32 uMaskCtrl)
{
	u32 uRegValue;
	u32 uRegOffset, uMaskID;

	uRegOffset = (eWKUPSRC&0xF00)>>8;
	uMaskID = eWKUPSRC&0x0FF;

	if(uRegOffset == 0)
	{
		uRegValue =Inp32(rPWR_CFG);
		uRegValue = (uRegValue & ~(0x1<<(uMaskID+7))) | (uMaskCtrl<<(uMaskID+7));	
		Outp32(rPWR_CFG, uRegValue);
	}
	else if(uRegOffset == 1)
	{
		uRegValue = Inp32(rEINT_MASK);
		uRegValue = (uRegValue & ~(0x1<<(uMaskID))) | (uMaskCtrl<<(uMaskID));
		Outp32(rEINT_MASK, uRegValue);
	}
	
}


//////////
// Function Name : SYSC_CtrlNORCFG
// Function Description : This function control Block Power off of the internal logic during normal operation 
//				  
// Input : 		  	eCFGId : Internal block  Domain Id
//					uPwrCtrl     :   (0) LP mode,  Block Power Off
//								 (1) Active mode, Block Power ON
//		
//							  
// Output : NONE
// Version : 
void SYSC_CtrlNORCFG( NORGFC_eID eCFGId, u32 uPwrCtrl)
{
	u32 uRegValue, uTemp;

	uRegValue =Inp32(rNORMAL_CFG);
	uTemp = eCFGId;

       uRegValue = (uRegValue & ~(1<<uTemp)) | (uPwrCtrl <<uTemp);
	Outp32(rNORMAL_CFG, uRegValue);
}


//////////
// Function Name : SYSC_CtrlSTOPCFG
// Function Description : This function control Block Power off of the internal logic during normal operation 
//				  
// Input : 		  	eCFGId : Internal block  Domain Id
//					uPwrCtrl     :   (0) LP mode,  Block Power Off
//								 (1) Active mode, Block Power ON
//		
//							  
// Output : NONE
// Version : 
void SYSC_CtrlSTOPCFG( STOPCFG_eID eCFGId, u32 uPwrCtrlStop)
{
	u32 uRegValue, uTemp;

	uRegValue =Inp32(rSTOP_CFG);
	uTemp = eCFGId;

       uRegValue = (uRegValue & ~(1<<uTemp)) | (uPwrCtrlStop <<uTemp);
	Outp32(rSTOP_CFG, uRegValue);
}

//////////
// Function Name : SYSC_CtrlSTOPMEMCFG
// Function Description : This function control Block Power off of the internal logic during normal operation 
//				  
// Input :			eStopMemCfgId : Top Memory Id
//					uPwrCtrlStopMem :	 (0) LP mode,  Block Power Off
//								 (1) Active mode, Block Power ON
//		
//							  
// Output : NONE
// Version : 
void SYSC_CtrlSTOPMEMCFG(STOPMEMCFG_eID eStopMemCfgId, u32 uPwrCtrlStopMem )
{
	u32 uRegValue, uTemp;

	uRegValue =Inp32(rSTOP_MEM_CFG);
	uTemp = eStopMemCfgId;

       uRegValue = (uRegValue & ~(1<<uTemp)) | (uPwrCtrlStopMem <<uTemp);
	Outp32(rSTOP_MEM_CFG, uRegValue);			
}




//////////
// Function Name : SYSC_ChangeMode
// Function Description : This function change Operating Mode
//				  
// Input : 		  	
//					eOPMode     :   eASYNC_MODE(0) Asynchronous mode
//								  eSYNC_MODE (1) Synchronous mode
//		
//							  
// Output : NONE
// Version : 
void SYSC_ChangeMode(SYSC_eOPMode eOPMode)
{
	u32 uRegValue,  uCurrMode, uDIV0, uDIV0_org, uSRC, uSRC_org;
	u32 uHCLKx2_Ratio;

	uRegValue =Inp32(rOTHERS);
	uCurrMode = (uRegValue >> 8 ) & 0xF ;
	uDIV0_org = Inp32(rCLK_DIV0);
	uSRC_org = Inp32(rCLK_SRC);
	uHCLKx2_Ratio=(uDIV0_org>>9)&0x7;

	
	if((eOPMode == 0)&&(uCurrMode == 0xF))				// Current = SyncMode, Next = AsyncMode	
	{
//		UART_Printf("Mode is changed to Asyn. from Sync.\n");
		// For FirmWare Test => APLL Out = MPLL*2
		if(uHCLKx2_Ratio == 0)
		{
			uDIV0_org = uDIV0_org;
		}
		else 
		{
			uDIV0_org = (uDIV0_org & ~(0x7<<9)) | (0 <<9);
		}
		
		uSRC = (uSRC_org & ~(0x3<<0)) | (0 <<0);				// A&MPLL Change to Clock Source
		uDIV0 = (uDIV0_org & ~(0xFFFF<<0)) | (0x1100 <<0);	// Clock Divider 1:1 ??

		Outp32(rCLK_SRC, uSRC);
		Outp32(rCLK_DIV0, uDIV0);
		
		uRegValue = (uRegValue & ~(0x3<<6)) | (1<<6);		// SYNCMODE(SYNCREQ) = ASYNC, VICEN= Async, MUXSEL=Sync
		Outp32(rOTHERS, uRegValue);

		while(((Inp32(rOTHERS)>>8)&0xF));				// Wait SYNCMODEACK = 0x0

		uRegValue = Inp32(rOTHERS);
		uRegValue = (uRegValue&~(0x1<<6)) | (0<<6);		// Change SYNCMUX to AsyncMode
		Outp32(rOTHERS, uRegValue);

		Outp32(rCLK_DIV0, uDIV0_org);
		Outp32(rCLK_SRC, uSRC_org);

		
	}
	else if ((eOPMode == 1)&&(uCurrMode == 0x0))			// Current = AsyncMode, Next = SyncMode	
	{
//		UART_Printf("Mode is changed to Syn. from Aync.\n");
		uSRC = (uSRC_org & ~(0x3<<0)) | (0 <<0);				// A&MPLL Change to Clock Source
		uDIV0 = (uDIV0_org & ~(0xFFFF<<0)) | (0x1100 <<0);	      // Clock Divider 1:1 ??

		if(uHCLKx2_Ratio == 0)
		{
			uDIV0_org = (uDIV0_org & ~(0x7<<9)) | (1 <<9);
		}
		else 
		{
			uDIV0_org = uDIV0_org;
			
		}

		Outp32(rCLK_SRC, uSRC);
		Outp32(rCLK_DIV0, uDIV0);
	
		uRegValue = (uRegValue & ~(0x1<<6)) | (1<<6);		// Change SYNCMUX to SyncMode
		Outp32(rOTHERS, uRegValue);

		__asm {

			nop;
			nop;
			nop;
			nop;
			nop;
			
			}											// Wait for a while (TBD)

		uRegValue = (uRegValue & ~(0x1<<7)) | (1<<7);		// SYNCMODE = SYNC, VICEN=SYNC, MUXSEL=SYNC
		Outp32(rOTHERS, uRegValue);
		
		while(!(((Inp32(rOTHERS)>>8)&0xf) == 0xF));			// Wait SYNCMODEACK = 0xF

		
		Outp32(rCLK_DIV0, uDIV0_org);
	
		Outp32(rCLK_SRC, uSRC_org);
	
	}

	Delay(10);
		
}

//////////
// Function Name : SYSC_CheckOpMode
// Function Description : This function return the current operation mode
//				  
// Input : 		  	
//							  
// Output : NONE
// Version : 
u32 SYSC_CheckOpMode(void)
{
	u32 uRegValue;

	uRegValue =Inp32(rOTHERS);
       uRegValue &= (1<<7);

	return (uRegValue>>7);		
}


//////////
// Function Name : SYSC_ClrBATFINT
// Function Description : This function clear int pending bit about nBATF Int.
//				  
// Input : 		  	
//							  
// Output : NONE
// Version : 
void SYSC_ClrBATFINT(void)
{
	u32 uRegValue;

	uRegValue =Inp32(rOTHERS);
       uRegValue |= (1<<12);
       Outp32(rOTHERS, uRegValue);
		
}






//////////
// Function Name : SYSC_RdRSTSTAT
// Function Description : This function read Rest Status Register
//				  
// Input : NONE		  	
//							  
// Output : Status Value
// Version : 
u32 SYSC_RdRSTSTAT(u32 uPRINT)
{
	u32 uRegValue;
	u32 uRstId;

	uRegValue =Inp32(rRST_STAT);

	if(uPRINT == 1)
	UART_Printf("Reset Status Register:	0x%x\n",  uRegValue);

	if( uRegValue & (1<<0))
	{
		if(uPRINT == 1)
		UART_Printf("External reset by XnRESET  \n\n");
		uRstId = 0;
	}
	else if (uRegValue & (1<<1))
	{
		if(uPRINT == 1)
		UART_Printf("Warm reset by XnWRESET \n\n");
		uRstId = 1;
		
	}
	else if (uRegValue & (1<<2))
	{
		if(uPRINT == 1)
		UART_Printf("Watch dog timer reset by WDTRST\n\n");
		uRstId = 2;
		
	}
	else if (uRegValue & (1<<3))
	{
		if(uPRINT == 1)
		UART_Printf("Reset by SLEEP Mode Wake-up \n\n");
		uRstId = 3;
		
	}
	else if (uRegValue & (1<<4))
	{
		if(uPRINT == 1)
		UART_Printf("Reset by ESLEEP mode Wake-up \n\n");
		uRstId = 4;
	}
	else if (uRegValue & (1<<5))
	{
		if(uPRINT == 1)
		UART_Printf("Software reset by SWRESET\n\n");
		uRstId = 5;		
	}	

	return uRstId;
}

//////////
// Function Name : SYSC_RdBLKPWR
// Function Description : This function read Block Power Status Register
//				  
// Input : 		  	
//							  
// Output : NONE
// Version : 
void SYSC_RdBLKPWR(void)
{
	u32 uRegValue;

	uRegValue =Inp32(rBLK_PWR_STAT);
	UART_Printf("Block Power Status Register	0x%x\n",  uRegValue);
#if 0
	UART_Printf("Block Top Power	%d\n",  uRegValue&(0x1<<0));
	UART_Printf("Block V    Power	%d\n",  (uRegValue&(0x1<<1))>>1);
	UART_Printf("Block I	    Power	%d\n",  (uRegValue&(0x1<<2))>>2);	
	UART_Printf("Block P    Power	%d\n",  (uRegValue&(0x1<<3))>>3);	
	UART_Printf("Block F	    Power	%d\n",  (uRegValue&(0x1<<4))>>4);	
	UART_Printf("Block S    Power	%d\n",  (uRegValue&(0x1<<5))>>5);	
	UART_Printf("Block ETM    Power	%d\n",  (uRegValue&(0x1<<6))>>6);	
#endif		
}

//////////
// Function Name : SYSC_WaitingForBLKPWR
// Function Description : This function read Block Power Status Register
//				  
// Input : eCFGId : Internal block power status Id	
//		 uOnOff  : On or Off
//							  
// Output : NONE
// Version : 
void SYSC_WaitingForBLKPWR(BLKPWRSTA_eID eCFGId, u32 uOnOff)
{
	u32 uRegValue;

	while(!(Inp32(rBLK_PWR_STAT)&(1<<eCFGId)==uOnOff));
	
}

//////////
// Function Name : SYSC_WaitingForBLKPWR
// Function Description : This function read Block Power Status Register
//				  
// Input : eCFGId : Internal block power status Id	
//		 uOnOff  : On or Off
//							  
// Output : NONE
// Version : 
void SYSC_WaitingForBLKPWRAll(u32 uOnOff)
{
	u32 uRegValue;

	if(uOnOff==1)
		while(!(Inp32(rBLK_PWR_STAT)&(0xff)==0xff));
	else if(uOnOff==0)
		while(!(Inp32(rBLK_PWR_STAT)&(0xff)==0x800));
	else Assert(0);

	
}



//////////
// Function Name : SYSC_ClrWKUPSTAT
// Function Description : This function clear Wake-up Status Register
//				  
// Input : 		  	
//							  
// Output : NONE
// Version : 
void SYSC_ClrWKUPSTAT(void)
{
	u32 uRegValue;

	uRegValue =Inp32(rWAKEUP_STAT);

//	UART_Printf("Wake-up Status Register:0x%x\n",  uRegValue);

	switch(uRegValue&0xfff) {
		case 0x1:
			UART_Printf("___ Wake-up by EINT[0x%x] \n", uRegValue);
			uRegValue |= (1<<0);
			Outp32(rWAKEUP_STAT,uRegValue);
			break;
		case 0x2:
			UART_Printf("___ Wake-up by RTC ALARM[0x%x] \n", uRegValue);
			uRegValue |= (1<<1);
			Outp32(rWAKEUP_STAT,uRegValue);
			break;
		case 0x4:
			UART_Printf("___ Wake-up by RTC Tick[0x%x] \n", uRegValue);
			uRegValue |= (1<<2);
			Outp32(rWAKEUP_STAT,uRegValue);
			break;
		case 0x8:
			UART_Printf("___ Wake-up by Touch Screen [0x%x] \n", uRegValue);
			uRegValue |= (1<<3);
			Outp32(rWAKEUP_STAT,uRegValue);
			break;
		case 0x10:
			UART_Printf("___ Wake-up by KeyPad [0x%x] \n", uRegValue);
			uRegValue |= (1<<4);
			Outp32(rWAKEUP_STAT,uRegValue);
			break;
		case 0x20:
			UART_Printf("___ Wake-up by MSM Modem[0x%x] \n", uRegValue);
			uRegValue |= (1<<5);
			Outp32(rWAKEUP_STAT,uRegValue);
			break;
		case 0x40:
			UART_Printf("___ Wake-up by BATFLT[0x%x] \n", uRegValue);
			uRegValue |= (1<<6);
			Outp32(rWAKEUP_STAT,uRegValue);
			break;
		case 0x80:
			UART_Printf("___ Wake-up by Warm Reset[0x%x] \n", uRegValue);
			uRegValue |= (1<<7);
			Outp32(rWAKEUP_STAT,uRegValue);
			break;
		case 0x100:
			UART_Printf("___ Wake-up by HSI[0x%x] \n", uRegValue);
			uRegValue |= (1<<8);
			Outp32(rWAKEUP_STAT,uRegValue);
			break;
		case 0x200:
			UART_Printf("___ Wake-up by MMC0[0x%x] \n", uRegValue);
			uRegValue |= (1<<9);
			Outp32(rWAKEUP_STAT,uRegValue);
			break;
		case 0x400:
			UART_Printf("___ Wake-up by MMC1[0x%x] \n", uRegValue);
			uRegValue |= (1<<10);
			Outp32(rWAKEUP_STAT,uRegValue);
			break;
		case 0x800:
			UART_Printf("___ Wake-up by MMC2[0x%x] \n", uRegValue);
			uRegValue |= (1<<11);
			Outp32(rWAKEUP_STAT,uRegValue);
			break;
		default:
			UART_Printf("___ Wake-up by Wrong[0x%x] \n", uRegValue);
			uRegValue |= (1<<11);
			Outp32(rWAKEUP_STAT,uRegValue);
			break;
	}	
}


//////////
// Function Name : SYSC_InitData_SDRAM
// Function Description : This function initializes data in the DRAM
//					 
// Input : 	check_start - start address, check_size- data size
// Output : 	
//			
// Version : v0.1
void SYSC_InitData_SDRAM(int check_start, int check_size)
{
    int i;
#if DBG_PRINT
    UART_Printf("[SDRAM Initialization]\n");
    UART_Printf("Fill SDRAM for self-refresh test.\n");
#endif
    for(i=check_start;i<(check_start+check_size);i+=4)
    	*((unsigned int *)i)=i^0x55555555;
    	//*((unsigned int *)i)=0x55555555;
#if DBG_PRINT    
    UART_Printf("Filling SDRAM is completed.\n");
#endif
    
}

//////////
// Function Name : SYSC_CheckData_SDRAM
// Function Description : This function check data in the DRAM 
//					  
// Input : 	check_start - start address, check_size- data size
// Output : 	
//			
// Version : v0.1
void SYSC_CheckData_SDRAM(int check_start, int check_size)
{
    int i;
    int error=0;
	
    dprintf("Check SDRAM for self-refresh test\n");
    for(i=check_start;i<(check_start+check_size);i+=4)
    {
    	if(*((unsigned int *)i)!=(i^0x55555555))
    	{
    	    dprintf("Mem Error:%x=%x(%x)\n",i,*((unsigned int *)i),i^0x55555555);
    	    error++;
    	}
    	if(error>20)
	    break;
    }
    if(error)
    	{
      	UART_Printf("SDRAM self-refresh test:FAILED\n"); 
	while(1);	
      	}
    else
    	{
    	UART_Printf("SDRAM self-refresh test:O.K.\n");	
    	}

}

#if 0
//////////

// Function Name : Isr_SYSC_KEY

// Function Description : KeyWake UP Isr 

// Input :  None

// Output : None 

// Version : v0.1
 

void __irq Isr_SYSC_KEY(void)

{

    u32 uCol, uRow;

    u32 uColScan, uRowScan;

    u32 urKEYIFSTSCLR, urKEYIFROW;

    

    INTC_Disable(NUM_KEYPAD);

 

//    UART_Printf("\n");
//    UART_Printf("KEYIFCON Register Value is 0x%x\n", KEYIFInp32(rKEYIFCON));
    UART_Printf("KEYIFSTSCLR Register Value is 0x%x\n", KEYIFInp32(rKEYIFSTSCLR));
//    UART_Printf("KEYIFCOL Register Value is 0x%x\n", KEYIFInp32(rKEYIFCOL));
    UART_Printf("KEYIFROW Register Value is 0x%x\n", KEYIFInp32(rKEYIFROW));
//  UART_Printf("KEYIFFC Register Value is 0x%x\n", KEYIFInp32(rKEYIFFC));  

//    UART_Printf("\n");
    

    KEYPAD_STSCLR(0xff,0xff);   
    INTC_ClearVectAddr(); 

    if ( urKEYIFSTSCLR & 0xff )                                     // Falling Edge Scan
    {
        for ( uCol = 0; uCol < KEYIF_COLNUM ; uCol++)
        {
            uColScan = 0xff ^ (1 << uCol);
            KEYPAD_COL(uColScan, 0);
            Delay(10);                                         // change DelayfrTimer later 
            urKEYIFROW = KEYIFInp32(rKEYIFROW);
            if ( urKEYIFROW != 0xff)  {
                KEYPAD_STSCLR(0xff,0xff);                       // Unwanted Interrupt Source Makes. it must clear!
                for(uRow = 0;uRow < KEYIF_ROWNUM; uRow++)  {     //Scan X-Axis (COLUMN)
                    uRowScan = 0xff ^(1 << uRow);
                    if ( urKEYIFROW == uRowScan ) {
                        UART_Printf("[%d, %d]\n", uRow, uCol);
                        break;
                    }
                }               

            }

        }

    }
	else if ( urKEYIFSTSCLR & 0xff00 )  {    // Rising Edge
		UART_Printf("\KEY Wake UP!\n");
	}

 

    KEYPAD_COL(0,0);

    Delay(100);

    KEYPAD_STSCLR(0xff,0xff);   

//  Outp32(rKEYIFCON, uTemp_KEY);

    INTC_Enable(NUM_KEYPAD);

 

}



void SYSC_SetKEYIFWakeup(void)

{

    u32 uTemp;

 

    KEYPAD_InitPort(KeyPort0);

    KEYPAD_ClosePort(KeyPort1);

    

    KEYPAD_Init();  

    KEYPAD_EnableWakeup(true);

    

    KEYPAD_COL(0,0);            // Column Low

 

    INTC_ClearVectAddr();

    KEYPAD_STSCLR(0xff,0xff);   // Key Interrupt Clear  

    INTC_SetVectAddr(NUM_KEYPAD,Isr_SYSC_KEY);

    INTC_Enable(NUM_KEYPAD);

 

}
#endif

//////////
// Function Name : LTC3714_VoltageSet 
// Function Description : CLKGate_Test in the Normal Mode 
// Input : 					uPwr : 1:  ARM Voltage Control,  2: Internal Voltage Control, 3: Both Voltage Control
//							uVoltage :  1mV 
// Output :	None 
// Version : v0.1
void LTC3714_VoltageSet(u32 uPwr, u32 uVoltage)  
{

     int uvtg, uRegValue;
	//////////////////////////////////////////////
	// GPN15  GPN14 GNP13 GPN12 GPN11
	//  VID4    VID3   VID2    VID1   VID0		// Voltage
	
	//	0	 0	  0 	0	 0		// 1.75V
	//	0	 0	  0 	0	 1		// 1.70V
	//	0	 0	  0 	1	 0		// 1.65V
	//	0	 0	  0 	1	 1		// 1.60V
	//	0	 0	  1 	0	 0		// 1.55V
	//	0	 0	  1 	0	 1		// 1.50V
	//	0	 0	  1 	1	 0		// 1.45V
	//	0	 0	  1 	1	 1		// 1.40V
	//	0	 1	  0 	0	 0		// 1.35V
	//	0	 1	  0 	0	 1		// 1.30V
	//	0	 1	  0 	1	 0		// 1.25V
	//	0	 1	  0 	1	 1		// 1.20V
	//	0	 1	  1 	0	 0		// 1.15V
	//	0	 1	  1 	0	 1		// 1.10V
	//	0	 1	  1 	1	 0		// 1.05V
	//	0	 1	  1 	1	 1		// 1.00V
	//   1	 0      0   0       0		// 0.975V
	//	1	 0	  0 	0	 1		// 0.950V
	//    1      0      0   1       0           // 0.925V
	//	1	 0	  0 	1	 1		// 0.900V
	//    1      0      1   0       0           // 0.875V
	//	1	 0	  1 	0	 1		// 0.850V
	//    1      0      1   1       0          //  0.825V
	//	1	 0	  1 	1	 1		// 0.800V
	//    1      1      0   0       0          //  0.775V
	//    1      1      0   0       1          //  0.750V
	//    1      1      0   1       0          //  0.725V
	//    1      1      0    1      1          //  0.700V
	//    1      1      1    0      0          //  0.675V
	//    1      1      1    0      1          //  0.650V
	//    1      1      1    1      0          //  0.625V
	//    1      1      1    1      1          //  0.600V
	
   	uvtg=uVoltage;

	//GPIO Setting - For LTC3714 VID
	GPIO_SetFunctionEach(eGPIO_N, eGPIO_11, 1);		//XEINT11(VID0) as Output
	GPIO_SetFunctionEach(eGPIO_N, eGPIO_12, 1);		//XEINT12(VID1) as Output
	GPIO_SetFunctionEach(eGPIO_N, eGPIO_13, 1);		//XEINT13(VID2) as Output
	GPIO_SetFunctionEach(eGPIO_N, eGPIO_14, 1);		//XEINT14(VID3) as Output
	GPIO_SetFunctionEach(eGPIO_N, eGPIO_15, 1);		//XEINT11(VID4) as Output

	GPIO_SetPullUpDownEach(eGPIO_N, eGPIO_11, 0);	// Pull-up/dn disable
	GPIO_SetPullUpDownEach(eGPIO_N, eGPIO_12, 0);
	GPIO_SetPullUpDownEach(eGPIO_N, eGPIO_13, 0);
	GPIO_SetPullUpDownEach(eGPIO_N, eGPIO_14, 0);
	GPIO_SetPullUpDownEach(eGPIO_N, eGPIO_15, 0);


	// Latch Control Signal 
	// CORE_REG_OE: XhiA9(GPL9),  ARM_REG_LE: XhiA8(GPL8), INT_REG_LE: XhiA10(GPL10)
	GPIO_SetFunctionEach(eGPIO_L, eGPIO_8, 1);		//XhiA8(ARM_REG_LE) as Output
	GPIO_SetFunctionEach(eGPIO_L, eGPIO_9, 1);		//XhiA9(CORE_REG_OE) as Output
	GPIO_SetFunctionEach(eGPIO_L, eGPIO_10,1);		//XhiA10(INT_REG_LE)as Output

	GPIO_SetPullUpDownEach(eGPIO_L, eGPIO_8, 0);	// Pull-up/dn disable
	GPIO_SetPullUpDownEach(eGPIO_L, eGPIO_9, 0);
	GPIO_SetPullUpDownEach(eGPIO_L, eGPIO_10, 0);
		
	
       uRegValue = Inp32(0x7F008834);					// GPNDAT Register

   

    switch (uvtg)
	{
	case 1750:
	      uRegValue=(uRegValue&~(0x1f<<11))|((0<<15)|(0<<14)|(0<<13)|(0<<12)|(0<<11));	//D4~0
	      break;

	case 1700:
	     uRegValue=(uRegValue&~(0x1f<<11))|((0<<15)|(0<<14)|(0<<13)|(0<<12)|(1<<11));	//D4~0
	      break;

	case 1650:
		 uRegValue=(uRegValue&~(0x1f<<11))|((0<<15)|(0<<14)|(0<<13)|(1<<12)|(0<<11));	//D4~0
		 break;
       
	case 1600:
		uRegValue=(uRegValue&~(0x1f<<11))|((0<<15)|(0<<14)|(0<<13)|(1<<12)|(1<<11));	//D4~0
		break;

	case 1550:
		 uRegValue=(uRegValue&~(0x1f<<11))|((0<<15)|(0<<14)|(1<<13)|(0<<12)|(0<<11));	//D4~0
		 break;

	case 1500:
		uRegValue=(uRegValue&~(0x1f<<11))|((0<<15)|(0<<14)|(1<<13)|(0<<12)|(1<<11));	//D4~0
		 break;

	case 1450:
		uRegValue=(uRegValue&~(0x1f<<11))|((0<<15)|(0<<14)|(1<<13)|(1<<12)|(0<<11));	//D4~0
		 break;

	case 1400:
		 uRegValue=(uRegValue&~(0x1f<<11))|((0<<15)|(0<<14)|(1<<13)|(1<<12)|(1<<11));	//D4~0
		 break;

	case 1350:
	      uRegValue=(uRegValue&~(0x1f<<11))|((0<<15)|(1<<14)|(0<<13)|(0<<12)|(0<<11));	//D4~0
	      break;

	case 1300:
		 uRegValue=(uRegValue&~(0x1f<<11))|((0<<15)|(1<<14)|(0<<13)|(0<<12)|(1<<11));	//D4~0
		 break;
       
	case 1250:
		uRegValue=(uRegValue&~(0x1f<<11))|((0<<15)|(1<<14)|(0<<13)|(1<<12)|(0<<11));	//D4~0
		break;

	case 1200:
		 uRegValue=(uRegValue&~(0x1f<<11))|((0<<15)|(1<<14)|(0<<13)|(1<<12)|(1<<11));	//D4~0
		 break;

	case 1150:
		uRegValue=(uRegValue&~(0x1f<<11))|((0<<15)|(1<<14)|(1<<13)|(0<<12)|(0<<11));	//D4~0
		 break;

	case 1100:
		 uRegValue=(uRegValue&~(0x1f<<11))|((0<<15)|(1<<14)|(1<<13)|(0<<12)|(1<<11));	//D4~0
		 break;

	case 1050:
		 uRegValue=(uRegValue&~(0x1f<<11))|((0<<15)|(1<<14)|(1<<13)|(1<<12)|(0<<11));	//D4~0
		 break;

	case 1000:
		 uRegValue=(uRegValue&~(0x1f<<11))|((0<<15)|(1<<14)|(1<<13)|(1<<12)|(1<<11));	//D4~0
		break;

	case 975:
		uRegValue=(uRegValue&~(0x1f<<11))|((1<<15)|(0<<14)|(0<<13)|(0<<12)|(0<<11));	//D4~0
		 break;

	case 950:
		uRegValue=(uRegValue&~(0x1f<<11))|((1<<15)|(0<<14)|(0<<13)|(0<<12)|(1<<11));	//D4~0
		 break;
		 
	case 925:
		uRegValue=(uRegValue&~(0x1f<<11))|((1<<15)|(0<<14)|(0<<13)|(1<<12)|(0<<11));	//D4~0
		 break;
		 
	case 900:
		uRegValue=(uRegValue&~(0x1f<<11))|((1<<15)|(0<<14)|(0<<13)|(1<<12)|(1<<11));	//D4~0
		 break;		 

	case 875:
		uRegValue=(uRegValue&~(0x1f<<11))|((1<<15)|(0<<14)|(1<<13)|(0<<12)|(0<<11));	//D4~0
		 break;

	case 850:
		uRegValue=(uRegValue&~(0x1f<<11))|((1<<15)|(0<<14)|(1<<13)|(0<<12)|(1<<11));	//D4~0
		 break;

	case 825:
		uRegValue=(uRegValue&~(0x1f<<11))|((1<<15)|(0<<14)|(1<<13)|(1<<12)|(0<<11));	//D4~0
		 break;

	case 800:
		uRegValue=(uRegValue&~(0x1f<<11))|((1<<15)|(0<<14)|(1<<13)|(1<<12)|(1<<11));	//D4~0
		 break;

	case 775:
		uRegValue=(uRegValue&~(0x1f<<11))|((1<<15)|(1<<14)|(0<<13)|(0<<12)|(0<<11));	//D4~0
		 break;

	case 750:
		uRegValue=(uRegValue&~(0x1f<<11))|((1<<15)|(1<<14)|(0<<13)|(0<<12)|(1<<11));	//D4~0
		 break;

	case 725:
		uRegValue=(uRegValue&~(0x1f<<11))|((1<<15)|(1<<14)|(0<<13)|(1<<12)|(0<<11));	//D4~0
		 break;
		 
	case 700:
		uRegValue=(uRegValue&~(0x1f<<11))|((1<<15)|(1<<14)|(0<<13)|(1<<12)|(1<<11));	//D4~0
		 break;

	case 675:
		uRegValue=(uRegValue&~(0x1f<<11))|((1<<15)|(1<<14)|(1<<13)|(0<<12)|(0<<11));	//D4~0
		 break;		 

	case 650:
		uRegValue=(uRegValue&~(0x1f<<11))|((1<<15)|(1<<14)|(1<<13)|(0<<12)|(1<<11));	//D4~0
		 break;

	case 625:
		uRegValue=(uRegValue&~(0x1f<<11))|((1<<15)|(1<<14)|(1<<13)|(1<<12)|(0<<11));	//D4~0
		 break;

	case 600:
		uRegValue=(uRegValue&~(0x1f<<11))|((1<<15)|(1<<14)|(1<<13)|(1<<12)|(1<<11));	//D4~0
		 break;
 
    default:	// 1.00V
		uRegValue=(uRegValue&~(0x1f<<11))|((0<<15)|(1<<14)|(1<<13)|(1<<12)|(1<<11));	//D4~0
		 break;	 		
	}
	
	UART_Printf("uRegValue(VID_Data) 0x%x\n", uRegValue);

	// Control Data 
	Outp32(0x7F008834,uRegValue);

	if(uPwr == 1 )		//ARM Voltage Control => ARM_REG_LE => Output H => Data Changed
	{
		GPIO_SetDataEach(eGPIO_L, eGPIO_8, 1);
		UART_Printf("ARM REG LE\n");
	}
	else if(uPwr == 2 )	// INT Voltage Control
	{
		GPIO_SetDataEach(eGPIO_L, eGPIO_10, 1);
		UART_Printf("INT REG LE\n");
	}
	else if (uPwr== 3) 	// Both Voltage Control
	{
		GPIO_SetDataEach(eGPIO_L, eGPIO_8, 1);
		GPIO_SetDataEach(eGPIO_L, eGPIO_10, 1);
			
		//UART_Printf("ARM&INT REG LE\n");
		//while(!UART_GetKey());
		
	}

	// Output Enable
	GPIO_SetDataEach(eGPIO_L, eGPIO_9, 1);

    UART_Printf("Latch Out Enable\n");
    //while(!UART_GetKey());
	Delay(100);

	GPIO_SetDataEach(eGPIO_L, eGPIO_8, 0);		// Latch Disable
	GPIO_SetDataEach(eGPIO_L, eGPIO_10,0);		// Latch Disable
	

}


void __irq Isr_SYSC_RTC_Tick_Wu(void)
{
	
	//GPIO_SetFunctionEach(eGPIO_F, eGPIO_14,3); 	//GPF14[] = 3 : CLKOUT[0]
	//RTC_ClearIntP(0); //-daedoo
	RTC_ClearPending(TimeTic);
	
	UART_Printf("[WU] RTC Tick interrupt is occurred .\n");

	INTC_ClearVectAddr();

}


//////////
// Function Name : SetAlarmTick
// Function Description : Setting RTC Alarm Int. - after 10sec.
// Input : 	None
// Output :	None 
// Version : v0.1
void SYSC_SetAlarmTick (void)
{

	INTC_SetVectAddr(NUM_RTC_TIC, Isr_SYSC_RTC_Tick_Wu);
	
	// RTC Init. //-daedoo
	//RTC_SetCON(0,0,0,0,0,1);
	RTC_TimeInit(InitYEAR,InitMONTH,InitDATE,InitDAY,InitHOUR,InitMIN,InitSEC);
#if 0   // 5sec, Tick Clock =128Hz	
	//RTC_SetTickCNT(640);	
	//RTC_SetCON(0,8,0,0,0,0);							// Tick Clock = 128Hz?
       						// 1/128*1280 = 10Sec?
#endif 
	//RTC_SetTickCNT(65500);
	//RTC_SetCON(0,0,0,0,0,0);	       
	RTC_TickCnt(5000);
	RTC_Enable(false);

	

	//RTC_ClearIntP(0);								// Clear Int. Pending
	RTC_ClearPending(TimeTic);
#if 0	
	//RTC_SetCON(1,8,0,0,0,1);
#endif
	//RTC_SetCON(1,0,0,0,0,1);
	RTC_TickTimeEnable(true);
	RTC_Enable(true);

	UART_Printf("	RTC Tick is set for wake-up source\n");

	INTC_Enable(NUM_RTC_TIC);

	
}


//////////
// Function Name : Isr_SYSC_AlmWu
// Function Description : Stop Mode Test - Baisc Test 
// Input : 	None
// Output :	None 
// Version : v0.1
void __irq Isr_SYSC_AlmWu(void)
{
	//-daedoo
	//RTC_ClearIntP(1);
	RTC_ClearPending(ALARM);
	//uCntAlarm=1;
//#if DBG_PRINT
	//UART_Printf(" [Wu]RTC alarm test OK\n");
//#endif

	dprintf("r*\n");
	INTC_ClearVectAddr();	
	INTC_Disable(NUM_RTC_ALARM);
}

//////////
// Function Name : SetAlarmWakeup
// Function Description : Setting RTC Alarm Int. - after 10sec.
// Input : 	None
// Output :	None 
// Version : v0.1
void SYSC_SetAlarmWakeup (void)
{

	//-daedoo
	//RTC_ClearIntP(1);	
	RTC_ClearPending(ALARM);
	INTC_SetVectAddr(NUM_RTC_ALARM,Isr_SYSC_AlmWu);

	/*
	RTC_SetCON(0,0,0,0,0,1);
	RTC_SetTime(InitYEAR,InitMONTH,InitDATE,InitDAY,InitHOUR,InitMIN,InitSEC);
	RTC_SetAlmTime(AlmYear,AlmMon,AlmDate,AlmHour,AlmMin,AlmSec+5);
	RTC_SetAlmEn(1,0,0,0,0,0,1);
	RTC_SetCON(0,0,0,0,0,0);
	*/

	RTC_TimeInit(InitYEAR,InitMONTH,InitDATE,InitDAY,InitHOUR,InitMIN,InitSEC);
	RTC_AlarmTimeInit(AlmYear,AlmMon,AlmDate,AlmHour,AlmMin,AlmSec+5);
	RTC_AlarmEnable(true, true, true, true, true, true, true);
	
	SYSC_SetWakeMASK(eMASK_ALARM	, 0);

	//dprintf("	RTC Alarm is set for wake-up source\n");

	INTC_Enable(NUM_RTC_ALARM);
	
}
//////////
// Function Name : Isr_SYSC_Eint9
// Function Description : EINT9 ISR Routine
// Input : 	None
// Output :	None 
// Version : v0.1
void __irq Isr_SYSC_WUEint9(void)
{
          
	GPIO_EINT0ClrPend(9);
	
	//UART_Printf("[WU] EINT9 interrupt is occurred .\n");
	dprintf("e*\n");
      // UART_Printf("0x7f008924= 0x%x\n", Inp32(0x7F008924));
	INTC_ClearVectAddr();  
	INTC_Disable(NUM_EINT1);
	  
    
}

//////////
// Function Name : Isr_SYSC_Eint10
// Function Description : EINT10 ISR Routine
// Input : 	None
// Output :	None 
// Version : v0.1
void __irq Isr_SYSC_WUEint10(void)
{
          
	GPIO_EINT0ClrPend(10);
//	UART_Printf("[WU]EINT10 interrupt is occurred .\n");

	INTC_ClearVectAddr();
    
}

//////////
// Function Name : Isr_SYSC_Eint11
// Function Description : EINT11 ISR Routine
// Input : 	None
// Output :	None 
// Version : v0.1
void __irq Isr_SYSC_WUEint11(void)
{
          
	GPIO_EINT0ClrPend(11);
//	UART_Printf("[WU]EINT11 interrupt is occurred .\n");

	INTC_ClearVectAddr();    
}


#if 0
//////////
// Function Name : SYSC_SetEintWakeup
// Function Description : Setting EINT9,10,11 Int. - Falling Edge
// Input : 	None
// Output :	None 
// Version : v0.1
void SYSC_SetEintWakeup (void)
{
	GPIO_Init();
	
	INTC_SetVectAddr(NUM_EINT1, Isr_SYSC_WUEint9);
//	INTC_SetVectAddr(NUM_EINT1, Isr_SYSC_WUEint10);
//	INTC_SetVectAddr(NUM_EINT1, Isr_SYSC_WUEint11);	

	GPIO_SetPullUpDownEach(eGPIO_N, eGPIO_9, 0x0);  // Pull Down Disable
	GPIO_SetPullUpDownEach(eGPIO_N, eGPIO_11, 0x0);  // Pull Down Disable
	GPIO_SetPullUpDownEach(eGPIO_N, eGPIO_10, 0x0); // Pull Down Disable

	Delay(100);
	//GPIO_SetEint0(9, Low_Level, eDLYFLT, 0);
	GPIO_SetEint0(9, Falling_Edge, eDLYFLT, 0);  			// EVT1 OK
//	GPIO_SetEint0(10, Falling_Edge, eDLYFLT, 0);  			// EVT1 OK
//	GPIO_SetEint0(11, Falling_Edge, eDLYFLT, 0);  			// EVT1 OK	

	GPIO_EINT0ClrPend(9);
	GPIO_EINT0ClrPend(10);
	GPIO_EINT0ClrPend(11);

	GPIO_EINT0DisMask(9);

//	dprintf("	EINT9  is set for wake-up source\n");

	INTC_Enable(NUM_EINT1);
	
}
#else
//////////
// Function Name : SYSC_SetEintWakeup
// Function Description : Setting EINT9,10,11 Int. - Falling Edge
// Input : 	None
// Output :	None 
// Version : v0.1
void SYSC_SetEintWakeup (void)
{
	GPIO_Init();
	
	INTC_SetVectAddr(NUM_EINT1, Isr_SYSC_WUEint9);
//	INTC_SetVectAddr(NUM_EINT1, Isr_SYSC_WUEint10);
//	INTC_SetVectAddr(NUM_EINT1, Isr_SYSC_WUEint11);	

	GPIO_SetPullUpDownEach(eGPIO_N, eGPIO_9, 0x0);  // Pull Down Disable
	GPIO_SetPullUpDownEach(eGPIO_N, eGPIO_11, 0x0);  // Pull Down Disable
	GPIO_SetPullUpDownEach(eGPIO_N, eGPIO_10, 0x0); // Pull Down Disable

	Delay(100);
	//GPIO_SetEint0(9, Low_Level, eDLYFLT, 0);
	GPIO_SetEint0(9, Falling_Edge, eDLYFLT, 0);  			// EVT1 OK
//	GPIO_SetEint0(10, Falling_Edge, eDLYFLT, 0);  			// EVT1 OK
//	GPIO_SetEint0(11, Falling_Edge, eDLYFLT, 0);  			// EVT1 OK	

	GPIO_EINT0ClrPend(9);
	GPIO_EINT0ClrPend(10);
	GPIO_EINT0ClrPend(11);

	GPIO_EINT0DisMask(9);

//	dprintf("	EINT9  is set for wake-up source\n");

	INTC_Enable(NUM_EINT1);

//	SYSC_CtrlHCLKGate(eHCLK_INTC, 0);
}

#endif




//////////
// Function Name : Isr_SYSC_ADCTS_Wu
// Function Description : ADCTS ISR Routine
// Input : 	None
// Output :	None 
// Version : v0.1
void __irq	Isr_SYSC_ADCTS_Wu(void)
{
//	UART_Printf("[WU] Touch interrupt is occurred .\n");

	ADCTS_SetADCClearWKInt();
	INTC_ClearVectAddr();
}


//////////
// Function Name : SYSC_SetADCTSWakeup
// Function Description : 
// Input : 	None
// Output :	None 
// Version : v0.1
void SYSC_SetADCTSWakeup (void)
{
	u32 uTemp;
	ADCTS_Init();
	ADC_InitADCCON();

	Outp32(0x7E00B000, (Inp32(0x7E00B000)|(1<<2)));
	ADCTS_SetDelay(20);

	uTemp = Inp32(0x7E00B008)|(1<<16);    // RTC Clock
	Outp32(0x7E00B008, uTemp);
//	UART_Printf("	Touch Screen  is set for wake-up source\n");

	Outp32(0x7E00B004, 0x90);
	
	ADCTS_SetMeasureMode(eADCTS_MEASURE_FOR_STYLUS);
	Outp32(0x7E00B004, 0xd3);
	
//	dprintf("	rADCCON: 0x%x\n", Inp32(0x7E00B000));
//	dprintf("	rADCTS: 0x%x\n", Inp32(0x7E00B004));
//	dprintf("	rADCDLY: 0x%x\n", Inp32(0x7E00B008));


	INTC_SetVectAddr(NUM_PENDNUP, Isr_SYSC_ADCTS_Wu);
	INTC_Enable(NUM_PENDNUP);

}


//////////
// Function Name : Isr_SYSC_WUBATF
// Function Description : Battery Fault Isr 
// Input : 	None
// Output :	None 
// Version : v0.1
void __irq Isr_SYSC_WUBATF(void)
{
	SYSC_ClrBATFINT();
	UART_Printf("[WU]Battery Fault is occurred!\n");
	INTC_ClearVectAddr();
	INTC_Disable(NUM_BATF);	
}

//////////
// Function Name : SetBatteryFaultEvent
// Function Description : Setting Battery Falult Event
// Input : 	None
// Output :	None 
// Version : v0.1
void SYSC_SetBatteryFaultEvent(BATFLT_eMODE eBATFLT) // It should be modified..... boaz.kim
{
	if(eBATFLT==eFLT_INT)
	{
		//Outp32SYSC(0x804,0x80); // BATFn_MASK_??
		
		INTC_SetVectAddr(NUM_BATF, Isr_SYSC_WUBATF);
		SYSC_SetBATF(eFLT_INT, 0);
		INTC_Enable(NUM_BATF);
	}
	if(eBATFLT==eFLT_SLEEP)
	{
		UART_Printf("Inform register is set!\n");
		Outp32Inform(0,0xABCD6400);	
		SYSC_SetBATF(eFLT_SLEEP, 1);	
	}
}

//////////
// Function Name : SYSC_WDT_Reset
// Function Description : WatchDog Reset Setting
// Input : 	None
// Output :	None 
// Version : v0.1
void SYSC_WDT_Reset(void)
{

	Outp32(0x7e004000,(0<<8|1<<5|0<<3|0<<2|1));
	Outp32(0x7e004004, 0xffff);
	Outp32(0x7e004004, 0xffff);
}



//////////
// Function Name : SYSC_IDLE
// Function Description : IDLE Mode Test - Baisc Test 
// Input : 	None
// Output :	None 
// Version : v0.1
void SYSC_IDLE (void)
{
	u32 uRegValue;
	u32 ucnt;

	
	UART_Printf("[IDLE Mode Baisc Test]\n");
	UART_Printf("After 10 seconds, S3C6410 will wake up by RTC alarm interrupt.\n");
	UART_Printf("S3C6410 will also wake up by EINT9, ADCTS, or BATT_FLT.\n");


	ucnt=Inp32(rINFORM0);	
	ucnt++;
	if(ucnt==0xffffffff){
		ucnt=0;
		Outp32(rINFORM1, (Inp32(rINFORM1))++);
	}
	Outp32(rINFORM0, ucnt);
	UART_Printf("[%d] %d-th IDLE\n",  Inp32(rINFORM1), ucnt);
	
	////////////////////////////////
	// Wake-up Source Setting
	////////////////////////////////
	UART_Printf("Wake-up sourceis set! \n");

	SYSC_SetAlarmWakeup();
	SYSC_SetEintWakeup();
	Outp32SYSC(0x804, 0x1<<7);			// nBATF Wake-up Mask disable
	SYSC_SetBatteryFaultEvent(eFLT_INT);
	SYSC_SetADCTSWakeup();
	SYSC_SetAlarmTick();


	////////////////////////////////
	// Enter Idle Mode
	// 		1. Setting "CFG_STANDBYWFI" to "2'b01"  ( PWR_CFG(0x7E00_F804)
	//          2.  Enter to the STANDBYWFI Command
	////////////////////////////////
	
	UART_Printf("CPU will goes to IDLE Mode \n");
	UART_TxEmpty();

	SYSC_SetCFGWFI(eIDLE,0);		// IDLE,  Clock Disable
	MMU_WaitForInterrupt();


	SYSC_ClrWKUPSTAT();
	uRegValue=Inp32(rWAKEUP_STAT);
	UART_Printf("Return to Normal Mode \n\n");
	UART_Printf("rWAKEUP_STAT=0x%x\n", uRegValue);
	Outp32(rWAKEUP_STAT, uRegValue);
	UART_Printf("rWAKEUP_STAT=0x%x\n Wake-up Status Clear\n", uRegValue);

	// Int. Disable
	INTC_Disable(NUM_EINT1);
	INTC_Disable(NUM_RTC_ALARM);
	INTC_Disable(NUM_BATF);
	INTC_Disable(NUM_PENDNUP);
	INTC_Disable(NUM_RTC_TIC);

}

//////////
// Function Name : SYSC_STOP
// Function Description : Stop Mode Test - Baisc Test 
// Input : 	None
// Output :	None 
// Version : v0.1
void SYSC_STOP (void)
{
	u32 uRegValue;

	u32 uPclkGate, uHclkGate, uSclkGate;
	u32 uNormalCfg, uStopCfg, uStopMemCfg;

	
	UART_Printf("\n[STOP Mode Basic Test]\n");
	UART_Printf("After 10 seconds, S3C6410 will wake up by RTC alarm interrupt.\n");
	UART_Printf("S3C6410 will also wake up by EINT9, ADCTS, or BATT_FLT.\n");


	////////////////////////////////
	//  Save I/O Ports 
	////////////////////////////////
	// Save the port configurations
	//UART_Printf("I/O Port is stored!\n");
	//for(i=0;i<36;i++) {
	//	portStatus[i]=*( (volatile U32 *)0x56000000 + i);	// 0x5600_0000:GPACON addr.
	//	}
	//UART_Printf("I/O Port is set for Stop Mode!\n");
	//ConfigStopGPIO();
	//ConfigMiscIO_stop();

	dprintf("Test pattern for SDRAM Self-Refresh is filled!\n");
	SYSC_InitData_SDRAM(_DRAM_BaseAddress+0x1000000, 0x10000);

	UART_Printf("Wake-up source is set!\n");
	////////////////////////////////
	// Wake-up Source Setting
	//      Wake-up Source Interrupts are must enabled at Stop Mode
	////////////////////////////////
	SYSC_SetAlarmWakeup();
	SYSC_SetEintWakeup();
	SYSC_SetADCTSWakeup();
	Outp32SYSC(0x804, 0x1<<7);			// nBATF Wake-up Mask disable
	SYSC_SetBatteryFaultEvent(eFLT_INT);	


	SYSC_OSCSTAB(1);			
	   
	SYSC_SetSTOPCFG(1, 1, 1, 1);		// Set ARM_LOGIC/MEM, TOP_LOGIC/MEM to "1"
	SYSC_SetSTOPMEMCFG(1, 1, 1, 1, 1, 1, 1);	

	UART_Printf("CPU will go to Stop Mode!\n");
	UART_TxEmpty();

	SYSC_SetCFGWFI(eSTOP, 0);
	MMU_WaitForInterrupt();
	

	UART_Printf("nCPU will go to Normal Mode!\n");
	SYSC_ClrWKUPSTAT();	
	SYSC_CheckData_SDRAM(_DRAM_BaseAddress+0x1000000, 0x10000);
	
	INTC_Disable(NUM_EINT1);
	INTC_Disable(NUM_RTC_ALARM);
	INTC_Disable(NUM_BATF);
	INTC_Disable(NUM_PENDNUP);

}


//////////
// Function Name : SYSC_STOP
// Function Description : Stop Mode Test - Baisc Test 
// Input : 	None
// Output :	None 
// Version : v0.1
void SYSC_STOP_Debugging(void)
{
	u32 uRegValue;

	u32 uPclkGate, uHclkGate, uSclkGate;
	u32 uNormalCfg, uStopCfg, uStopMemCfg;

	u32 uSel;
	u32 ucnt;
	u32 uOsc;

	
//	UART_Printf("\n[STOP Mode Basic Test]\n");
//	UART_Printf("After 10 seconds, S3C6410 will wake up by RTC alarm interrupt.\n");
//	UART_Printf("S3C6410 will also wake up by EINT9, ADCTS, or BATT_FLT.\n");


	////////////////////////////////
	//  Save I/O Ports 
	////////////////////////////////
	// Save the port configurations
	//UART_Printf("I/O Port is stored!\n");
	//for(i=0;i<36;i++) {
	//	portStatus[i]=*( (volatile U32 *)0x56000000 + i);	// 0x5600_0000:GPACON addr.
	//	}
	//UART_Printf("I/O Port is set for Stop Mode!\n");
	//ConfigStopGPIO();
	//ConfigMiscIO_stop();

	UART_Printf("%d-th Test!\n", Inp32(rINFORM3));

	if( Inp32(rINFORM3)>=1 )
	{
		UART_Printf("Wake-up form Stop Mode \n");
		SYSC_CheckData_SDRAM(_DRAM_BaseAddress+0x1000000, 0x10000);

		//Check & Clear Wake-up Source 
		SYSC_ClrWKUPSTAT();
		//Read RawInterrupt Status
//		UART_Printf("VIC0RawStatus: 0x%x \n", Inp32(0x71200008));
//		UART_Printf("VIC1RawStatus: 0x%x \n", Inp32(0x71300008));

		
//		UART_Printf("SLEEP mode test is done\n");
		//g_OnTest = 1;
	}
				// Entering into SLEEP Mode
	
		//g_OnTest=0;
	ucnt=Inp32(rINFORM3);
	ucnt++;
	if(ucnt==0xffffffff) ucnt=0;
	Outp32(rINFORM3, ucnt);

			
		UART_Printf("Test pattern for SDRAM Self-Refresh is filled!\n");
		SYSC_InitData_SDRAM(_DRAM_BaseAddress+0x1000000, 0x10000);

//	UART_Printf("Wake-up source is set!\n");
	////////////////////////////////
	// Wake-up Source Setting
	//      Wake-up Source Interrupts are must enabled at Stop Mode
	////////////////////////////////
	SYSC_SetAlarmWakeup();
	SYSC_SetEintWakeup();
//	Outp32SYSC(0x804, 0x1<<7);			// nBATF Wake-up Mask disable
//	SYSC_SetBatteryFaultEvent(eFLT_INT);
	SYSC_SetADCTSWakeup();

       /////////////////////////////////
       // 1) Check&Enable HCLK_GATE[22,21,0]
       // 2) Check BLK_PWR_STAT 
       // 3) Check&Clear WAKEUP_STAT
       //////////////////////////////


#ifdef S3C6410	

	uPclkGate=Inp32(rPCLK_GATE);
	uSclkGate=Inp32(rSCLK_GATE);	
	uHclkGate=Inp32(rHCLK_GATE);	
	uNormalCfg=Inp32(rNORMAL_CFG);
	uStopCfg=Inp32(rSTOP_CFG);
	uStopMemCfg=Inp32(rSTOP_MEM_CFG);	
	
	UART_Printf("\nCPU will goes to Stop Mode!\n\n");
	UART_TxEmpty();

	//All Clocks and powers are enabled.
	SYSC_SetCLKGateAll(1);

#if 0

	UART_Printf("0:APLL, 1:EPLL, 2:HCLK, 3:CLK48, 4:CLK27M, 5:RTC, 6:TICK, 7:DOUT, 8:FIN, 9:FrefAPLL, 10:FfbAPLL,11:FrefMPLL, 12:FfbMPLL,13:FrefEPLL, 14:FfbEPLL: ");
	uSel =UART_GetIntNum();

	Outp32(rCLK_OUT, uSel<<12); // FOUTapll
	//	Outp32(rCLK_OUT, 0x1<<12); // FOUTapll
	//	Outp32(rCLK_OUT, 0x2<<12); // FOUTapll
	//	Outp32(rCLK_OUT, 0x0<<12); // FOUTapll
	//	Outp32(rCLK_OUT, 0x0<<12); // FOUTapll
	//	Outp32(rCLK_OUT, 0x0<<12); // FOUTapll
	//	Outp32(rCLK_OUT, 0x0<<12); // FOUTapll
	//	Outp32(rCLK_OUT, 0x0<<12); // FOUTapll
	//	Outp32(rCLK_OUT, 0x0<<12); // FOUTapll


	GPIO_SetPullUpDownEach(eGPIO_F, eGPIO_14, 0x0);  // Pull Down Disable
	GPIO_SetFunctionEach(eGPIO_F, eGPIO_14, 0x3);

	UART_Getc();


#endif

#if 0	
	Outp32(rHCLK_GATE, (Inp32(rHCLK_GATE)&~(1<<31|1<<8|1)));
//	Outp32(rHCLK_GATE, (Inp32(rHCLK_GATE)&~(1<<31|1<<8)));
	Outp32(rMEM0_CLK_GATE, 0x3e);
	UART_Printf("HCLKGATE:0x%x, MEM0CLKGATE:0x%x\n", Inp32(rHCLK_GATE), Inp32(rMEM0_CLK_GATE));
#endif

#if 0 // stop vector
	Outp32(rPWR_CFG, 0x240);	
	Outp32(rSTOP_CFG, 0x0);
#endif

//	UART_Printf("0: OSC Diable, 1:	OSC Enable\n");
//	uOsc=UART_GetIntNum();


#if 1 // ∫Ò¡§ªÛ µø¿€«œ¡ˆ∏∏ wake-up¿∫ µ .
	SYSC_SetNORCFG(1,1,1,1,1,1,0,1,1);
//	SYSC_SetNORCFG(0,1,0,0,0,0,1,0,0);

//	SYSC_WaitingForBLKPWRAll(1); //waiting for stabilization of Sub-Block Power
	//Outp32(rNORMAL_CFG, (0xffffff<<12|0x0<<11|0x3<<9|0x100));
	SYSC_SetSTOPCFG(1, 1, 1, 1);
	
//	SYSC_SetSTOPCFG(0, 0, 0, 0);
	//Outp32(rSTOP_CFG, (0x1<<29|0x1<<20|0x1<<17|0x8<<9|0x1<<8|0x0));
//	Outp32(rBUS_CACHEABLE_CON, (0x3<<16));
	SYSC_SetSTOPMEMCFG(1, 1, 1, 1, 1, 1, 1);	
//	SYSC_SetSTOPMEMCFG(0, 0, 0, 0, 0, 0, 0);	
	//Outp32(rSTOP_MEM_CFG, 0x7);


//	SYSC_SetCFGWFI(eSTOP, uOsc);
	SYSC_SetCFGWFI(eSTOP, 0);

#endif
//	UART_Printf("PWRCFG:0x%x, EINTMASK:0x%x, NORMALCFG:0x%x, STOPCFG:0x%x\n", Inp32SYSC(0x804),Inp32SYSC(0x808),Inp32SYSC(0x810),Inp32SYSC(0x814));
//	UART_Printf(" SLEEPCFG:0x%x, STOPMEMCFG:0x%x, OTHERS:0x%x, BLKPWRSTAT:0x%x\n",Inp32SYSC(0x818),Inp32SYSC(0x81c),Inp32SYSC(0x900),Inp32SYSC(0x90c)); 
	UART_TxEmpty();

#endif
#if 1
	MMU_WaitForInterrupt();
#else
	Outp32(0x7e00f800, 0x2);
#endif
	
	////////////////////////////////
	//  Restore  I/O Ports 
	////////////////////////////////
	
/*
	//Restore the port configurations
	UART_Printf("\nI/O Port is restored!\n");
	for(i=0;i<36;i++){
		*( (volatile U32 *)0x56000000 + i)=portStatus[i];
	}
*/
		
#ifdef 0
	Outp32(rSTOP_MEM_CFG, uStopMemCfg);
	Outp32(rSTOP_CFG, uStopCfg);
	Outp32(rNORMAL_CFG, uNormalCfg);
	Outp32(rHCLK_GATE, uHclkGate);
	Outp32(rSCLK_GATE, uSclkGate);
	Outp32(rPCLK_GATE, uPclkGate);
#endif

	uRegValue=Inp32(rWAKEUP_STAT);
	DisplayLED(0x3);
	UART_Printf("Return to Normal Mode.\n");
	UART_Printf("rWAKEUP_STAT=0x%x\n", uRegValue);
	Outp32(rWAKEUP_STAT,uRegValue);
	UART_Printf("rWAKEUP_STAT=0x%x\n Wake-up Status Clear\n", uRegValue);
	
	SYSC_RdBLKPWR();

	// Int. Disable
	INTC_Disable(NUM_EINT1);
	INTC_Disable(NUM_RTC_ALARM);
	INTC_Disable(NUM_BATF);
	INTC_Disable(NUM_PENDNUP);

}




//////////
// Function Name : SYSC_STOP
// Function Description : Stop Mode Test - Baisc Test 
// Input : 	None
// Output :	None 
// Version : v0.1
void SYSC_STOP_Config(void)
{
	u32 uRegValue;

	u32 uPclkGate, uHclkGate, uSclkGate;
	u32 uNormalCfg, uStopCfg, uStopMemCfg;

	u32 uSel;
	u32 ucnt;
	u32 uOsc;


	
	UART_Printf("\n[STOP Mode Basic Test]\n");
	dprintf("After 10 seconds, S3C6410 will wake up by RTC alarm interrupt.\n");
	dprintf("S3C6410 will also wake up by EINT9, ADCTS, or BATT_FLT.\n");


	////////////////////////////////
	//  Save I/O Ports 
	////////////////////////////////
	// Save the port configurations
	//UART_Printf("I/O Port is stored!\n");
	//for(i=0;i<36;i++) {
	//	portStatus[i]=*( (volatile U32 *)0x56000000 + i);	// 0x5600_0000:GPACON addr.
	//	}
	//UART_Printf("I/O Port is set for Stop Mode!\n");
	//ConfigStopGPIO();
	//ConfigMiscIO_stop();

	UART_Printf("[%d]%d-th Test!\n", Inp32(rINFORM1), Inp32(rINFORM2));

	if( Inp32(rINFORM2)==0 )
	{
		dprintf("Test pattern for SDRAM Self-Refresh is filled!\n");
		SYSC_InitData_SDRAM(_DRAM_BaseAddress+0x1000000, 0x10000);
	}
//	else if((Inp32(rINFORM1)!=0) && !(Inp32(rSTOP_CFG)&1<<17))  {
	else if((Inp32(rINFORM2)!=0) ) {
		dprintf("Wake-up form Stop Mode \n");
		SYSC_CheckData_SDRAM(_DRAM_BaseAddress+0x1000000, 0x10000);
		if(!(Inp32(rSTOP_CFG)&1<<17)) {
			SYSC_ClrWKUPSTAT();
			Outp32(rINFORM3, 0x0);
		}
	}	

	ucnt=Inp32(rINFORM2);
	ucnt++;
	if(ucnt==0xffffffff) {
		ucnt=0;
		Outp32(rINFORM1, (Inp32(rINFORM1))++);
	}	
	Outp32(rINFORM2, ucnt);

			

	////////////////////////////////
	// Wake-up Source Setting
	//      Wake-up Source Interrupts are must enabled at Stop Mode
	////////////////////////////////
//	SYSC_SetAlarmWakeup();
	SYSC_SetEintWakeup();
//	Outp32SYSC(0x804, 0x1<<7);			// nBATF Wake-up Mask disable
//	SYSC_SetBatteryFaultEvent(eFLT_INT);
//	SYSC_SetADCTSWakeup();

       /////////////////////////////////
       // 1) Check&Enable HCLK_GATE[22,21,0]
       // 2) Check BLK_PWR_STAT 
       // 3) Check&Clear WAKEUP_STAT
       //////////////////////////////

	uPclkGate=Inp32(rPCLK_GATE);
	uSclkGate=Inp32(rSCLK_GATE);	
	uHclkGate=Inp32(rHCLK_GATE);	
	uNormalCfg=Inp32(rNORMAL_CFG);
	uStopCfg=Inp32(rSTOP_CFG);
	uStopMemCfg=Inp32(rSTOP_MEM_CFG);	
	
	//All Clocks and powers are enabled.
	SYSC_SetCLKGateAll(1);

#if 0

	UART_Printf("0:APLL, 1:EPLL, 2:HCLK, 3:CLK48, 4:CLK27M, 5:RTC, 6:TICK, 7:DOUT, 8:FIN, 9:FrefAPLL, 10:FfbAPLL,11:FrefMPLL, 12:FfbMPLL,13:FrefEPLL, 14:FfbEPLL: ");
	uSel =UART_GetIntNum();

	Outp32(rCLK_OUT, uSel<<12); // FOUTapll
	//	Outp32(rCLK_OUT, 0x1<<12); // FOUTapll
	//	Outp32(rCLK_OUT, 0x2<<12); // FOUTapll
	//	Outp32(rCLK_OUT, 0x0<<12); // FOUTapll
	//	Outp32(rCLK_OUT, 0x0<<12); // FOUTapll
	//	Outp32(rCLK_OUT, 0x0<<12); // FOUTapll
	//	Outp32(rCLK_OUT, 0x0<<12); // FOUTapll
	//	Outp32(rCLK_OUT, 0x0<<12); // FOUTapll
	//	Outp32(rCLK_OUT, 0x0<<12); // FOUTapll


	GPIO_SetPullUpDownEach(eGPIO_F, eGPIO_14, 0x0);  // Pull Down Disable
	GPIO_SetFunctionEach(eGPIO_F, eGPIO_14, 0x3);

	UART_Printf("0: OSC Diable, 1:	OSC Enable\n");
	uOsc=UART_GetIntNum();

#endif

	SYSC_OSCSTAB(0x1);			
	SYSC_PWRSTAB(0x1);			


#if 0 //ok
	SYSC_SetNORCFG(1,1,1,1,1,1,0,1,1);
	SYSC_SetSTOPCFG(1, 1, 1, 1);
	SYSC_SetSTOPMEMCFG(1, 1, 1, 1, 1, 1, 1);	
#elif 0 //ok
	SYSC_SetNORCFG(0,0,0,0,0,0,1,0,0); 
	SYSC_SetSTOPCFG(1, 1, 1, 1);
	SYSC_SetSTOPMEMCFG(1, 1, 1, 1, 1, 1, 1);	
#elif 0 // ok all off nandbooting case 
	SYSC_SetNORCFG(0,0,0,0,0,0,1,0,0); 
	SYSC_SetSTOPCFG(0, 1, 0, 0);
	SYSC_SetSTOPMEMCFG(0, 0, 0, 0, 0, 0, 1);	
#elif 0 // all off norbooting case 
	SYSC_SetNORCFG(0,0,0,0,0,0,1,0,0); 
	SYSC_SetSTOPCFG(0, 0, 0, 0);
	SYSC_SetSTOPMEMCFG(0, 0, 0, 0, 0, 0, 0);
#elif 0
	SYSC_SetNORCFG(0,1,0,0,0,0,1,0,0); 
	SYSC_SetSTOPCFG(0, 1, 0, 0);
	SYSC_SetSTOPMEMCFG(0, 0, 0, 0, 0, 0, 1);
#elif 0
	SYSC_SetNORCFG(0,1,0,0,0,0,1,0,0); 
	SYSC_SetSTOPCFG(1, 0, 1, 0);
	SYSC_SetSTOPMEMCFG(0, 0, 0, 0, 0, 0, 0);
//	SYSC_SetSTOPMEMCFG(1, 1, 1, 1, 1, 1, 1);
#elif 0 //okay
	SYSC_SetNORCFG(0,1,0,0,0,0,1,0,0); 
	SYSC_SetSTOPCFG(0, 1, 1, 0);
	SYSC_SetSTOPMEMCFG(0, 0, 0, 0, 0, 0, 1);
#endif



/* NOR Dabort, NAND OK
	SYSC_SetNORCFG(0,0,0,0,0,0,1,0,0); 
	SYSC_SetSTOPCFG(0, 1, 0, 0);
	SYSC_SetSTOPMEMCFG(0, 0, 0, 0, 0, 0, 1);	
*/

/* NOR Dabort
	SYSC_SetNORCFG(0,0,0,0,0,0,1,0,0); 
	SYSC_SetSTOPCFG(0, 1, 0, 0);
	SYSC_SetSTOPMEMCFG(0, 1, 1, 0, 0, 0, 1);	
*/

/*
// Nor/NAND good
	SYSC_SetNORCFG(0,0,0,0,0,0,1,0,0); 
	SYSC_SetSTOPCFG(0, 1, 0, 0);
	SYSC_SetSTOPMEMCFG(0, 0, 0, 0, 0, 0, 1);	
//	Outp32(rBUS_CACHEABLE_CON, 1<<17);
*/

//	SYSC_SetSTOPCFG(1, 0, 1, 0); // MEM_ARM=on, MEM_TOP=off, LOGIC_ARM=on, LOGIC_TOP=off

#if 0 // Aging Case Stop combination(ARM Alive)

	SYSC_SetSTOPCFG(1,(ucnt&(0x1<<8))>>8, 1, (ucnt&(0x1<<7))>>7);
	SYSC_SetSTOPMEMCFG((ucnt&(0x1<<6))>>6,(ucnt&(0x1<<5))>>5,(ucnt&(0x1<<4))>>4,(ucnt&(0x1<<3))>>3,(ucnt&(0x1<<2))>>2,(ucnt&(0x1<<1))>>1,(ucnt&(0x1<<0))>>0);

#endif	
#if 0 // OKAY
//	SYSC_SetSTOPCFG(1, 0, 1, 0);
//	SYSC_SetSTOPMEMCFG(0, 0, 0, 0, 0, 0, 0);

//	SYSC_SetSTOPCFG(1, 0, 1, 1);
//	SYSC_SetSTOPMEMCFG(0, 0, 0, 0, 0, 0, 0);

//	SYSC_SetSTOPCFG(1, 1, 1, 0);
//	SYSC_SetSTOPMEMCFG(0, 0, 0, 0, 0, 0, 0);

//	SYSC_SetSTOPCFG(1, 1, 1, 1);
//	SYSC_SetSTOPMEMCFG(0, 0, 0, 0, 0, 0, 0);
#endif

#if 0
///* ∏ÿ√„... UART Printf«œ¥Ÿ∞°....
//	Outp32(rNORMAL_CFG, 0xbffe0100);
//	Outp32(rNORMAL_CFG, 0xffffff00);
	Outp32(rSTOP_CFG, 0x20000);
	Outp32(rSTOP_MEM_CFG, 0x0);
///

#endif


#if 0 ///// AGING
	SYSC_SetNORCFG((ucnt&(0x1<<3))>>3,(ucnt&(0x1<<4))>>4,(ucnt&(0x1<<5))>>5,(ucnt&(0x1<<6))>>6,(ucnt&(0x1<<7))>>7,(ucnt&(0x1<<8))>>8,(ucnt&(0x1<<9))>>9,(ucnt&(0x1<<10))>>10,(ucnt&(0x1<<11))>>11); 
//	SYSC_SetSTOPCFG(0,0,0,0);
	SYSC_SetSTOPCFG((ucnt&(0x1<<1))>>1,  (ucnt&(0x1<<0))>>0, (ucnt&(0x1<<1))>>1, (ucnt&(0x1<<2))>>2);
	SYSC_SetSTOPMEMCFG((ucnt&(0x1<<12))>>12,(ucnt&(0x1<<13))>>13,(ucnt&(0x1<<14))>>14,(ucnt&(0x1<<15))>>15,(ucnt&(0x1<<16))>>16,(ucnt&(0x1<<17))>>17,(ucnt&(0x1<<18))>>18);


	if(!(Inp32(rSTOP_CFG)&1<<17))
		Outp32(rINFORM3, 0xdeaddead);

#else
	SYSC_SetSTOPCFG(0,1,0,1);
	SYSC_SetSTOPMEMCFG(1,1,1,1,1,1,1);
	Outp32(rINFORM3, 0xdeaddead);

#endif

	UART_Printf("PWRCFG:0x%x, EINTMASK:0x%x, NORMALCFG:0x%x, STOPCFG:0x%x\n", Inp32SYSC(0x804),Inp32SYSC(0x808),Inp32SYSC(0x810),Inp32SYSC(0x814));
	UART_Printf(" SLEEPCFG:0x%x, STOPMEMCFG:0x%x, OTHERS:0x%x, BLKPWRSTAT:0x%x\n",Inp32SYSC(0x818),Inp32SYSC(0x81c),Inp32SYSC(0x900),Inp32SYSC(0x90c)); 


//	dprintf("PWRCFG:0x%x, EINTMASK:0x%x, NORMALCFG:0x%x, STOPCFG:0x%x\n", Inp32SYSC(0x804),Inp32SYSC(0x808),Inp32SYSC(0x810),Inp32SYSC(0x814));
//	dprintf(" SLEEPCFG:0x%x, STOPMEMCFG:0x%x, OTHERS:0x%x, BLKPWRSTAT:0x%x\n",Inp32SYSC(0x818),Inp32SYSC(0x81c),Inp32SYSC(0x900),Inp32SYSC(0x90c)); 
	UART_Printf("S3C6410 will go to Stop Mode!\n");	
	UART_TxEmpty();

	SYSC_SetCFGWFI(eSTOP, 0);

#if 0
	if(!(Inp32(rSTOP_CFG)&1<<20)) {		
		UART_Printf("cache\n");
		MMU_WaitForInterruptWithCache();
	}	
	else
#endif		
		MMU_WaitForInterrupt();
	
	
	////////////////////////////////
	//  Restore  I/O Ports 
	////////////////////////////////
	
/*
	//Restore the port configurations
	UART_Printf("\nI/O Port is restored!\n");
	for(i=0;i<36;i++){
		*( (volatile U32 *)0x56000000 + i)=portStatus[i];
	}
*/
		
#ifdef 0
	Outp32(rSTOP_MEM_CFG, uStopMemCfg);
	Outp32(rSTOP_CFG, uStopCfg);
	Outp32(rNORMAL_CFG, uNormalCfg);
	Outp32(rHCLK_GATE, uHclkGate);
	Outp32(rSCLK_GATE, uSclkGate);
	Outp32(rPCLK_GATE, uPclkGate);
#endif


	//Check & Clear Wake-up Source 


	SYSC_SetNORCFG(1,1,1,1,1,1,0,1,1);
	SYSC_WaitingForBLKPWRAll(1);
//	OpenConsole();
	SYSC_RdBLKPWR();

	dprintf("Wake-up form Stop Mode \n");
	SYSC_CheckData_SDRAM(_DRAM_BaseAddress+0x1000000, 0x10000);
	SYSC_ClrWKUPSTAT();


//	DisplayLED(0x3);



	// Int. Disable
	INTC_Disable(NUM_EINT1);
	INTC_Disable(NUM_RTC_ALARM);
	INTC_Disable(NUM_BATF);
	INTC_Disable(NUM_PENDNUP);

}



//////////
// Function Name : SYSC_SLEEP0
// Function Description : Sleep Mode Test - Baisc Test 
// Input : 	None
// Output :	None 
// Version : v0.1
void SYSC_SLEEP0 (void)
{
	u32 uRstId;	
	u32 ucnt;

	UART_Printf("[%d]%d-th Test!\n", Inp32(rINFORM1), Inp32(rINFORM2));

	uRstId = SYSC_RdRSTSTAT(1);

	if( Inp32(rINFORM2)==0 )
	{
		dprintf("Test pattern for SDRAM Self-Refresh is filled!\n");
		SYSC_InitData_SDRAM(_DRAM_BaseAddress+0x1000000, 0x10000);
	}
//	else if((Inp32(rINFORM1)!=0) && !(Inp32(rSTOP_CFG)&1<<17))  {
	else if((Inp32(rINFORM2)!=0) ) {
		dprintf("Wake-up form Sleep Mode \n");
		SYSC_CheckData_SDRAM(_DRAM_BaseAddress+0x1000000, 0x10000);
		SYSC_ClrWKUPSTAT();
		Outp32(rINFORM3, 0x0);
	}	

	ucnt=Inp32(rINFORM2);
	ucnt++;
	if(ucnt==0xffffffff) {
		ucnt=0;
		Outp32(rINFORM1, (Inp32(rINFORM1))++);
	}	
	Outp32(rINFORM2, ucnt);	


	// ADC & RTC OFF		
	ADC_SelectStandbyMode(1);		//ADC Stand-by
	//-daedoo
	//RTC_SetCON(0,0,0,0,0,0);			//RTC SFR R/W Disable.
	RTC_Enable(false);
	////////////////////////////////
	//  Save I/O Ports 
	////////////////////////////////
	// Save the port configurations
	//UART_Printf("I/O Port is stored!\n");
	//for(i=0;i<36;i++) {
	//	portStatus[i]=*( (volatile U32 *)0x56000000 + i);	// 0x5600_0000:GPACON addr.
	//	}
	//UART_Printf("I/O Port is set for Stop Mode!\n");
	//ConfigSleepGPIO();
		
	UART_Printf("Wake-up source is set!\n");
	////////////////////////////////
	// Wake-up Source Setting
	//      Wake-up Source Interrupts are must enabled at Stop Mode
	////////////////////////////////
	SYSC_SetAlarmWakeup();
	SYSC_SetEintWakeup();
	Outp32(rPWR_CFG, 0x1<<7);			// nBATF Wake-up Mask disable
	SYSC_SetBatteryFaultEvent(eFLT_INT);
	UART_Printf("System stablilization counter register!\n");
	////////////////////////////////
	// OSCSTABLE = Clock Control
	// PWRSTABLE = Reset Control
	////////////////////////////////	
	SYSC_OSCSTAB(0x1);			
	SYSC_PWRSTAB(0x1);			


//	SYSC_SetNORCFG((ucnt&(0x1<<0))>>0,(ucnt&(0x1<<1))>>1,(ucnt&(0x1<<2))>>2,(ucnt&(0x1<<3))>>3,(ucnt&(0x1<<4))>>4,(ucnt&(0x1<<5))>>5,(ucnt&(0x1<<6))>>6,(ucnt&(0x1<<7))>>7,(ucnt&(0x1<<8))>>8); 


	////////////////////////////////
	// Enter the Stop Mode
	// 		1. Setting "CFG_STANDBYWFI" to "2'b11"  ( PWR_CFG(0x7E00_F804)
	//          2. Setting "SLEEP_CFG"(0x7E00_F818) ->  OSC_EN ="0"
	//          2.  Enter to the STANDBYWFI Command
	////////////////////////////////

	UART_Printf("PWRCFG:0x%x, EINTMASK:0x%x, NORMALCFG:0x%x, STOPCFG:0x%x\n", Inp32SYSC(0x804),Inp32SYSC(0x808),Inp32SYSC(0x810),Inp32SYSC(0x814));
	UART_Printf(" SLEEPCFG:0x%x, STOPMEMCFG:0x%x, OTHERS:0x%x, BLKPWRSTAT:0x%x\n",Inp32SYSC(0x818),Inp32SYSC(0x81c),Inp32SYSC(0x900),Inp32SYSC(0x90c)); 
	UART_Printf("\nCPU will go to SLEEP Mode!\n\n");
	UART_TxEmpty();				// 
	Outp32(rINFORM3, 0xDEADDEAD);
	SYSC_SetCFGWFI(eSLEEP, 0);		
	MMU_WaitForInterrupt();
	
	UART_Printf("CPU doesn't go to Sleep Mode\n");


}






//////////
// Function Name : SYSC_DEEPSTOP
// Function Description : Deep Stop Mode Test - Baisc Test 
// Input : 	None
// Output :	None 
// Version : v0.1
void SYSC_DEEPSTOP(void)
{
	u32 uRegValue;

	u32 uPclkGate, uHclkGate, uSclkGate;
	u32 uNormalCfg, uStopCfg, uStopMemCfg;

	u32 uSel;
	u32 ucnt;
	u32 uOsc;


	
	UART_Printf("\n[STOP Mode Basic Test]\n");
	dprintf("After 10 seconds, S3C6410 will wake up by RTC alarm interrupt.\n");
	dprintf("S3C6410 will also wake up by EINT9, ADCTS, or BATT_FLT.\n");


	////////////////////////////////
	//  Save I/O Ports 
	////////////////////////////////
	// Save the port configurations
	//UART_Printf("I/O Port is stored!\n");
	//for(i=0;i<36;i++) {
	//	portStatus[i]=*( (volatile U32 *)0x56000000 + i);	// 0x5600_0000:GPACON addr.
	//	}
	//UART_Printf("I/O Port is set for Stop Mode!\n");
	//ConfigStopGPIO();
	//ConfigMiscIO_stop();

	UART_Printf("[%d]%d-th Test!\n", Inp32(rINFORM1), Inp32(rINFORM2));

	if( Inp32(rINFORM2)==0 )
	{
		dprintf("Test pattern for SDRAM Self-Refresh is filled!\n");
		SYSC_InitData_SDRAM(_DRAM_BaseAddress+0x1000000, 0x10000);
	}
//	else if((Inp32(rINFORM1)!=0) && !(Inp32(rSTOP_CFG)&1<<17))  {
	else if((Inp32(rINFORM2)!=0) ) {
		dprintf("Wake-up form Stop Mode \n");
		SYSC_CheckData_SDRAM(_DRAM_BaseAddress+0x1000000, 0x10000);
		if(!(Inp32(rSTOP_CFG)&1<<17)) {
			SYSC_ClrWKUPSTAT();
			Outp32(rINFORM3, 0x0);
		}
	}	

	ucnt=Inp32(rINFORM2);
	ucnt++;
	if(ucnt==0xffffffff) {
		ucnt=0;
		Outp32(rINFORM1, (Inp32(rINFORM1))++);
	}	
	Outp32(rINFORM2, ucnt);

			

	////////////////////////////////
	// Wake-up Source Setting
	//      Wake-up Source Interrupts are must enabled at Stop Mode
	////////////////////////////////
	SYSC_SetAlarmWakeup();
	SYSC_SetEintWakeup();
//	Outp32SYSC(0x804, 0x1<<7);			// nBATF Wake-up Mask disable
//	SYSC_SetBatteryFaultEvent(eFLT_INT);
//	SYSC_SetADCTSWakeup();

       /////////////////////////////////
       // 1) Check&Enable HCLK_GATE[22,21,0]
       // 2) Check BLK_PWR_STAT 
       // 3) Check&Clear WAKEUP_STAT
       //////////////////////////////

	uPclkGate=Inp32(rPCLK_GATE);
	uSclkGate=Inp32(rSCLK_GATE);	
	uHclkGate=Inp32(rHCLK_GATE);	
	uNormalCfg=Inp32(rNORMAL_CFG);
	uStopCfg=Inp32(rSTOP_CFG);
	uStopMemCfg=Inp32(rSTOP_MEM_CFG);	
	
	//All Clocks and powers are enabled.
	SYSC_SetCLKGateAll(1);


	SYSC_OSCSTAB(0x1);			
	SYSC_PWRSTAB(0x1);			

#if 0
// NOR Booting
	SYSC_SetNORCFG(0,1,0,0,0,0,1,0,0); 
	SYSC_SetSTOPCFG(0,1,0,1);
	SYSC_SetSTOPMEMCFG(1,1,1,1,1,1,1);
//	Outp32(rINFORM3, 0xdeaddead);
#elif 0
	SYSC_SetNORCFG((ucnt&(0x1<<3))>>3,(ucnt&(0x1<<4))>>4,(ucnt&(0x1<<5))>>5,(ucnt&(0x1<<6))>>6,(ucnt&(0x1<<7))>>7,(ucnt&(0x1<<8))>>8,(ucnt&(0x1<<9))>>9,(ucnt&(0x1<<10))>>10,(ucnt&(0x1<<11))>>11); 
	SYSC_SetSTOPCFG((ucnt&(0x1<<1))>>1,  (ucnt&(0x1<<0))>>0, (ucnt&(0x1<<1))>>1, (ucnt&(0x1<<2))>>2);
	SYSC_SetSTOPMEMCFG((ucnt&(0x1<<12))>>12,(ucnt&(0x1<<13))>>13,(ucnt&(0x1<<14))>>14,(ucnt&(0x1<<15))>>15,(ucnt&(0x1<<16))>>16,(ucnt&(0x1<<17))>>17,(ucnt&(0x1<<18))>>18);
	if(!(Inp32(rSTOP_CFG)&1<<17))
			Outp32(rINFORM3, 0xdeaddead);
#elif 0	
	SYSC_SetNORCFG((ucnt&(0x1<<4))>>4,(ucnt&(0x1<<5))>>5,(ucnt&(0x1<<6))>>6,(ucnt&(0x1<<7))>>7,(ucnt&(0x1<<8))>>8,(ucnt&(0x1<<9))>>9,(ucnt&(0x1<<10))>>10,(ucnt&(0x1<<11))>>11,(ucnt&(0x1<<12))>>12); 
	SYSC_SetSTOPCFG((ucnt&(0x1<<0))>>0,  (ucnt&(0x1<<1))>>1, (ucnt&(0x1<<2))>>2, (ucnt&(0x1<<3))>>3);
	SYSC_SetSTOPMEMCFG((ucnt&(0x1<<13))>>13,(ucnt&(0x1<<14))>>14,(ucnt&(0x1<<15))>>15,(ucnt&(0x1<<16))>>16,(ucnt&(0x1<<17))>>17,(ucnt&(0x1<<18))>>18,(ucnt&(0x1<<19))>>19);
	if(!(Inp32(rSTOP_CFG)&1<<17))
			Outp32(rINFORM3, 0xdeaddead);
#else
	SYSC_SetNORCFG(0,0,0,0,0,0,0,0,0); 
//	SYSC_SetSTOPCFG(0,0,1,0);
//	SYSC_SetSTOPCFG(0,0,1,1);
//	SYSC_SetSTOPCFG(0,1,1,0);
	SYSC_SetSTOPCFG(0,1,1,1);
	SYSC_SetSTOPMEMCFG(1,1,1,1,1,1,1);
	if(!(Inp32(rSTOP_CFG)&1<<17))
			Outp32(rINFORM3, 0xdeaddead);	
#endif




	UART_Printf("PWRCFG:0x%x, EINTMASK:0x%x, NORMALCFG:0x%x, STOPCFG:0x%x\n", Inp32SYSC(0x804),Inp32SYSC(0x808),Inp32SYSC(0x810),Inp32SYSC(0x814));
	UART_Printf(" SLEEPCFG:0x%x, STOPMEMCFG:0x%x, OTHERS:0x%x, BLKPWRSTAT:0x%x\n",Inp32SYSC(0x818),Inp32SYSC(0x81c),Inp32SYSC(0x900),Inp32SYSC(0x90c)); 


//	dprintf("PWRCFG:0x%x, EINTMASK:0x%x, NORMALCFG:0x%x, STOPCFG:0x%x\n", Inp32SYSC(0x804),Inp32SYSC(0x808),Inp32SYSC(0x810),Inp32SYSC(0x814));
//	dprintf(" SLEEPCFG:0x%x, STOPMEMCFG:0x%x, OTHERS:0x%x, BLKPWRSTAT:0x%x\n",Inp32SYSC(0x818),Inp32SYSC(0x81c),Inp32SYSC(0x900),Inp32SYSC(0x90c)); 
	UART_Printf("S3C6410 will go to Stop Mode!\n");	
	UART_TxEmpty();

	SYSC_SetCFGWFI(eSTOP, 0);

#if 0
	if(!(Inp32(rSTOP_CFG)&1<<20)) {		
		UART_Printf("cache\n");
		MMU_WaitForInterruptWithCache();
	}	
	else
#endif		
		MMU_WaitForInterrupt();
	
	
	////////////////////////////////
	//  Restore  I/O Ports 
	////////////////////////////////
	
/*
	//Restore the port configurations
	UART_Printf("\nI/O Port is restored!\n");
	for(i=0;i<36;i++){
		*( (volatile U32 *)0x56000000 + i)=portStatus[i];
	}
*/
		
#ifdef 0
	Outp32(rSTOP_MEM_CFG, uStopMemCfg);
	Outp32(rSTOP_CFG, uStopCfg);
	Outp32(rNORMAL_CFG, uNormalCfg);
	Outp32(rHCLK_GATE, uHclkGate);
	Outp32(rSCLK_GATE, uSclkGate);
	Outp32(rPCLK_GATE, uPclkGate);
#endif


	//Check & Clear Wake-up Source 


	SYSC_SetNORCFG(1,1,1,1,1,1,0,1,1);
	SYSC_WaitingForBLKPWRAll(1);
//	OpenConsole();
	SYSC_RdBLKPWR();

	dprintf("Wake-up form Stop Mode \n");
	SYSC_CheckData_SDRAM(_DRAM_BaseAddress+0x1000000, 0x10000);
	SYSC_ClrWKUPSTAT();


//	DisplayLED(0x3);



	// Int. Disable
	INTC_Disable(NUM_EINT1);
	INTC_Disable(NUM_RTC_ALARM);
	INTC_Disable(NUM_BATF);
	INTC_Disable(NUM_PENDNUP);

}



//////////
// Function Name : SYSC_DEEPSTOP
// Function Description : Deep Stop Mode Test - Baisc Test 
// Input : 	None
// Output :	None 
// Version : v0.1

void SYSC_DEEPSTOP_ (void)
{
	//u32 uRegValue;
	u32	uDSTOP;
	u32 uRstId;
	u32 ucnt;

	uDSTOP= DeepStopStatus();
	SYSC_RdRSTSTAT(1);
#ifdef S3C6410
	if( ( uRstId == 3 ) && (Inp32(rINFORM3)==0xDEADDEAD ))
#else
	if((Inp32(rINFORM3)==0xDEADDEAD ))
#endif		
	{
		
		SYSC_CheckData_SDRAM(_DRAM_BaseAddress+0x1000000, 0x10000);

		SYSC_ClrWKUPSTAT();

		//Read RawInterrupt Status
		UART_Printf("VIC0RawStatus: 0x%x \n", Inp32(0x71200008));
		UART_Printf("VIC1RawStatus: 0x%x \n", Inp32(0x71300008));

		UART_Printf("Deep Stop mode test is done\n");
		Outp32(rINFORM3, 0x0);
	}

else			// Entering into Deep Stop Mode
	{
	
	UART_Printf("\n[Deep STOP Mode Basic Test]\n");
	UART_Printf("After 10 seconds, S3C6410 will wake up by RTC alarm interrupt.\n");
	UART_Printf("S3C6410 will also wake up by EINT9, ADCTS, or BATT_FLT.\n");


	UART_Printf("Test pattern for SDRAM Self-Refresh is filled!\n");
	SYSC_InitData_SDRAM(_DRAM_BaseAddress+0x1000000, 0x10000);
	Outp32Inform(5, 0x0);
	////////////////////////////////
	//  Save I/O Ports 
	////////////////////////////////
	// Save the port configurations
	//UART_Printf("I/O Port is stored!\n");
	//for(i=0;i<36;i++) {
	//	portStatus[i]=*( (volatile U32 *)0x56000000 + i);	// 0x5600_0000:GPACON addr.
	//	}
	//UART_Printf("I/O Port is set for Stop Mode!\n");
	//ConfigStopGPIO();
	//ConfigMiscIO_stop();


	UART_Printf("Wake-up source is set!\n");
	////////////////////////////////
	// Wake-up Source Setting
	//      Wake-up Source Interrupts are must enabled at Stop Mode
	////////////////////////////////
//	SYSC_SetAlarmWakeup();
	SYSC_SetEintWakeup();
//	Outp32SYSC(0x804, 0x1<<7);			// nBATF Wake-up Mask disable
//	SYSC_SetBatteryFaultEvent(eFLT_INT);
//	SYSC_SetADCTSWakeup();	

       /////////////////////////////////
       // 1) Check&Enable HCLK_GATE[22,21,0]
       // 2) Check BLK_PWR_STAT 
       // 3) Check&Clear WAKEUP_STAT
       //////////////////////////////
//	CheckHCLK_ForPM();
//       CheckBLKPWR_ForPM();
//	CheckWAKESTAT_ForPM(1);

	Outp32(rINFORM3, 0xDEADDEAD);
//	Outp32(rINFORM2, (u32)SYSC_DEEPSTOP);	
	UART_Printf("\nCPU will goes to Deep Stop Mode!\n\n");
	UART_TxEmpty();				// 

	////////////////////////////////
	// Enter the Stop Mode
	// 		1. Setting "CFG_STANDBYWFI" to "2'b10"  ( PWR_CFG(0x7E00_F804)
	//          2. Setting "STOP_CFG"(0x7E00_F814) -> ARM_LOGIC/MEM, TOP_LOGIC/MEM OFF, OSC_EN ="0"
	//          2.  Enter to the STANDBYWFI Command
	////////////////////////////////

	SYSC_OSCSTAB(1);			
	SYSC_SetNORCFG(0,0,0,0,0,0,1,0,0); // All Sub-block powers are off.
	SYSC_SetSTOPCFG(0, 0, 0, 0);		// Set ARM_LOGIC/MEM, TOP_LOGIC/MEM to "1"
	SYSC_SetSTOPMEMCFG(0, 0, 0, 0, 0, 0, 0); // Set ARM_LOGIC/MEM, TOP_LOGIC/MEM to "1"	
	

	SYSC_SetCFGWFI(eSTOP, 0);		// 
	MMU_WaitForInterrupt();

	UART_Printf("CPU doesn't go to Deep Stop Mode\n");
/*
	//Restore the port configurations
	UART_Printf("\nI/O Port is restored!\n");
	for(i=0;i<36;i++){
		*( (volatile U32 *)0x56000000 + i)=portStatus[i];
	}
*/
	// Int. Disable
	INTC_Disable(NUM_EINT1);
	INTC_Disable(NUM_RTC_ALARM);
	INTC_Disable(NUM_BATF);
	INTC_Disable(NUM_PENDNUP);

	}
	// Int. Disable
	INTC_Disable(NUM_EINT1);
	INTC_Disable(NUM_RTC_ALARM);
	INTC_Disable(NUM_BATF);
	INTC_Disable(NUM_PENDNUP);


}






//////////
// Function Name : SYSC_DEEPSTOP_Config
// Function Description : Deep Stop Mode Test - Baisc Test 
// Input : 	None
// Output :	None 
// Version : v0.1
void SYSC_DEEPSTOP_Config(void)
{
	u32 uRegValue;

	u32 uPclkGate, uHclkGate, uSclkGate;
	u32 uNormalCfg, uStopCfg, uStopMemCfg;

	u32 uSel;
	u32 ucnt;
	u32 uOsc;


	
	UART_Printf("\n[STOP Mode Basic Test]\n");
	dprintf("After 10 seconds, S3C6410 will wake up by RTC alarm interrupt.\n");
	dprintf("S3C6410 will also wake up by EINT9, ADCTS, or BATT_FLT.\n");


	////////////////////////////////
	//  Save I/O Ports 
	////////////////////////////////
	// Save the port configurations
	//UART_Printf("I/O Port is stored!\n");
	//for(i=0;i<36;i++) {
	//	portStatus[i]=*( (volatile U32 *)0x56000000 + i);	// 0x5600_0000:GPACON addr.
	//	}
	//UART_Printf("I/O Port is set for Stop Mode!\n");
	//ConfigStopGPIO();
	//ConfigMiscIO_stop();

	UART_Printf("[%d]%d-th Test!\n", Inp32(rINFORM1), Inp32(rINFORM2));

	if( Inp32(rINFORM2)==0 )
	{
		dprintf("Test pattern for SDRAM Self-Refresh is filled!\n");
		SYSC_InitData_SDRAM(_DRAM_BaseAddress+0x1000000, 0x10000);
	}
//	else if((Inp32(rINFORM1)!=0) && !(Inp32(rSTOP_CFG)&1<<17))  {
	else if((Inp32(rINFORM2)!=0) ) {
		dprintf("Wake-up form Stop Mode \n");
		SYSC_CheckData_SDRAM(_DRAM_BaseAddress+0x1000000, 0x10000);
		if(!(Inp32(rSTOP_CFG)&1<<17)) {
			SYSC_ClrWKUPSTAT();
			Outp32(rINFORM3, 0x0);
		}
	}	

	ucnt=Inp32(rINFORM2);
	ucnt++;
	if(ucnt==0xffffffff) {
		ucnt=0;
		Outp32(rINFORM1, (Inp32(rINFORM1))++);
	}	
	Outp32(rINFORM2, ucnt);

			

	////////////////////////////////
	// Wake-up Source Setting
	//      Wake-up Source Interrupts are must enabled at Stop Mode
	////////////////////////////////
	SYSC_SetAlarmWakeup();
	SYSC_SetEintWakeup();
	Outp32SYSC(0x804, 0x1<<7);			// nBATF Wake-up Mask disable
	SYSC_SetBatteryFaultEvent(eFLT_INT);
	SYSC_SetADCTSWakeup();

       /////////////////////////////////
       // 1) Check&Enable HCLK_GATE[22,21,0]
       // 2) Check BLK_PWR_STAT 
       // 3) Check&Clear WAKEUP_STAT
       //////////////////////////////

	uPclkGate=Inp32(rPCLK_GATE);
	uSclkGate=Inp32(rSCLK_GATE);	
	uHclkGate=Inp32(rHCLK_GATE);	
	uNormalCfg=Inp32(rNORMAL_CFG);
	uStopCfg=Inp32(rSTOP_CFG);
	uStopMemCfg=Inp32(rSTOP_MEM_CFG);	
	
	//All Clocks and powers are enabled.
	SYSC_SetCLKGateAll(1);


	SYSC_OSCSTAB(0x1);			
	SYSC_PWRSTAB(0x1);			

#if 0
// NOR Booting
	SYSC_SetNORCFG(0,1,0,0,0,0,1,0,0); 
	SYSC_SetSTOPCFG(0,1,0,1);
	SYSC_SetSTOPMEMCFG(1,1,1,1,1,1,1);
	Outp32(rINFORM3, 0xdeaddead);
#elif 0 //nor booting ok
	SYSC_SetNORCFG((ucnt&(0x1<<3))>>3,(ucnt&(0x1<<4))>>4,(ucnt&(0x1<<5))>>5,(ucnt&(0x1<<6))>>6,(ucnt&(0x1<<7))>>7,(ucnt&(0x1<<8))>>8,(ucnt&(0x1<<9))>>9,(ucnt&(0x1<<10))>>10,(ucnt&(0x1<<11))>>11); 
	SYSC_SetSTOPCFG((ucnt&(0x1<<1))>>1,  (ucnt&(0x1<<0))>>0, (ucnt&(0x1<<1))>>1, (ucnt&(0x1<<2))>>2);
	SYSC_SetSTOPMEMCFG((ucnt&(0x1<<12))>>12,(ucnt&(0x1<<13))>>13,(ucnt&(0x1<<14))>>14,(ucnt&(0x1<<15))>>15,(ucnt&(0x1<<16))>>16,(ucnt&(0x1<<17))>>17,(ucnt&(0x1<<18))>>18);
	if(!(Inp32(rSTOP_CFG)&1<<17))
			Outp32(rINFORM3, 0xdeaddead);
#elif 0 //nand booting ok	
	SYSC_SetNORCFG((ucnt&(0x1<<8))>>8,(ucnt&(0x1<<9))>>9,(ucnt&(0x1<<10))>>10,(ucnt&(0x1<<11))>>11,(ucnt&(0x1<<12))>>12,(ucnt&(0x1<<13))>>13,(ucnt&(0x1<<14))>>14,(ucnt&(0x1<<15))>>15,(ucnt&(0x1<<16))>>16); 
	SYSC_SetSTOPCFG((ucnt&(0x1<<0))>>0,  1, (ucnt&(0x1<<0))>>0, (ucnt&(0x1<<1))>>1);
	SYSC_SetSTOPMEMCFG((ucnt&(0x1<<2))>>2,(ucnt&(0x1<<3))>>3,(ucnt&(0x1<<4))>>4,(ucnt&(0x1<<5))>>5,(ucnt&(0x1<<6))>>6,(ucnt&(0x1<<7))>>7, 1);
	if(!(Inp32(rSTOP_CFG)&1<<17))
			Outp32(rINFORM3, 0xdeaddead);
#elif 1 //onenand booting ok	
	SYSC_SetNORCFG((ucnt&(0x1<<8))>>8,(ucnt&(0x1<<9))>>9,(ucnt&(0x1<<10))>>10,(ucnt&(0x1<<11))>>11,(ucnt&(0x1<<12))>>12,(ucnt&(0x1<<13))>>13,(ucnt&(0x1<<14))>>14,(ucnt&(0x1<<15))>>15,(ucnt&(0x1<<16))>>16); 
	SYSC_SetSTOPCFG((ucnt&(0x1<<0))>>0,  1, (ucnt&(0x1<<0))>>0, (ucnt&(0x1<<1))>>1);
	SYSC_SetSTOPMEMCFG((ucnt&(0x1<<2))>>2,(ucnt&(0x1<<3))>>3,(ucnt&(0x1<<4))>>4,(ucnt&(0x1<<5))>>5,(ucnt&(0x1<<6))>>6,(ucnt&(0x1<<7))>>7, 1);
	if(!(Inp32(rSTOP_CFG)&1<<17))
			Outp32(rINFORM3, 0xdeaddead);	

#else	
	SYSC_SetNORCFG((ucnt&(0x1<<4))>>4,(ucnt&(0x1<<5))>>5,(ucnt&(0x1<<6))>>6,(ucnt&(0x1<<7))>>7,(ucnt&(0x1<<8))>>8,(ucnt&(0x1<<9))>>9,(ucnt&(0x1<<10))>>10,(ucnt&(0x1<<11))>>11,(ucnt&(0x1<<12))>>12); 
	SYSC_SetSTOPCFG((ucnt&(0x1<<0))>>0,  (ucnt&(0x1<<1))>>1, (ucnt&(0x1<<2))>>2, (ucnt&(0x1<<3))>>3);
	SYSC_SetSTOPMEMCFG((ucnt&(0x1<<13))>>13,(ucnt&(0x1<<14))>>14,(ucnt&(0x1<<15))>>15,(ucnt&(0x1<<16))>>16,(ucnt&(0x1<<17))>>17,(ucnt&(0x1<<18))>>18,(ucnt&(0x1<<19))>>19);
	if(!(Inp32(rSTOP_CFG)&1<<17))
			Outp32(rINFORM3, 0xdeaddead);

#endif




	UART_Printf("PWRCFG:0x%x, EINTMASK:0x%x, NORMALCFG:0x%x, STOPCFG:0x%x\n", Inp32SYSC(0x804),Inp32SYSC(0x808),Inp32SYSC(0x810),Inp32SYSC(0x814));
	UART_Printf(" SLEEPCFG:0x%x, STOPMEMCFG:0x%x, OTHERS:0x%x, BLKPWRSTAT:0x%x\n",Inp32SYSC(0x818),Inp32SYSC(0x81c),Inp32SYSC(0x900),Inp32SYSC(0x90c)); 


//	dprintf("PWRCFG:0x%x, EINTMASK:0x%x, NORMALCFG:0x%x, STOPCFG:0x%x\n", Inp32SYSC(0x804),Inp32SYSC(0x808),Inp32SYSC(0x810),Inp32SYSC(0x814));
//	dprintf(" SLEEPCFG:0x%x, STOPMEMCFG:0x%x, OTHERS:0x%x, BLKPWRSTAT:0x%x\n",Inp32SYSC(0x818),Inp32SYSC(0x81c),Inp32SYSC(0x900),Inp32SYSC(0x90c)); 
	UART_Printf("S3C6410 will go to Stop Mode!\n");	
	UART_TxEmpty();

	SYSC_SetCFGWFI(eSTOP, 0);

#if 0
	if(!(Inp32(rSTOP_CFG)&1<<20)) {		
		UART_Printf("cache\n");
		MMU_WaitForInterruptWithCache();
	}	
	else
#endif		
		MMU_WaitForInterrupt();
	
	
	////////////////////////////////
	//  Restore  I/O Ports 
	////////////////////////////////
	
/*
	//Restore the port configurations
	UART_Printf("\nI/O Port is restored!\n");
	for(i=0;i<36;i++){
		*( (volatile U32 *)0x56000000 + i)=portStatus[i];
	}
*/
		
#ifdef 0
	Outp32(rSTOP_MEM_CFG, uStopMemCfg);
	Outp32(rSTOP_CFG, uStopCfg);
	Outp32(rNORMAL_CFG, uNormalCfg);
	Outp32(rHCLK_GATE, uHclkGate);
	Outp32(rSCLK_GATE, uSclkGate);
	Outp32(rPCLK_GATE, uPclkGate);
#endif


	//Check & Clear Wake-up Source 


	SYSC_SetNORCFG(1,1,1,1,1,1,0,1,1);
	SYSC_WaitingForBLKPWRAll(1);
//	OpenConsole();
	SYSC_RdBLKPWR();

	dprintf("Wake-up form Stop Mode \n");
	SYSC_CheckData_SDRAM(_DRAM_BaseAddress+0x1000000, 0x10000);
	SYSC_ClrWKUPSTAT();


//	DisplayLED(0x3);



	// Int. Disable
	INTC_Disable(NUM_EINT1);
	INTC_Disable(NUM_RTC_ALARM);
	INTC_Disable(NUM_BATF);
	INTC_Disable(NUM_PENDNUP);

}


void SYSC_STOP_Aging(void)
{
//	while(! UART_GetKey()) 	SYSC_STOP_Debugging();
	while(! UART_GetKey())	SYSC_STOP_Config();

}

void SYSC_IDLE_AGING(void)
{
	//	while(! UART_GetKey())	SYSC_STOP_Debugging();
		while(! UART_GetKey())	SYSC_IDLE();
}

void SYSC_DEEPSTOP_Aging(void)
{
	while(! UART_GetKey()) SYSC_DEEPSTOP_Config();	
}






//////////
// Function Name : SYSC_SLEEP
// Function Description : Sleep Mode Test - Baisc Test 
// Input : 	None
// Output :	None 
// Version : v0.1
void SYSC_SLEEP_AGING (void)
{
	u32 uRstId;	
	u32 uOsc;
	u32 uCnt;
	

	UART_Printf("[%d]%d-th Test!\n", Inp32(rINFORM1), Inp32(rINFORM2));


	if( Inp32(rINFORM1)==0 )
	{
		dprintf("Test pattern for SDRAM Self-Refresh is filled!\n");
		SYSC_InitData_SDRAM(_DRAM_BaseAddress+0x1000000, 0x10000);
	}

	uCnt=Inp32(rINFORM2);
	uCnt++;
	if(uCnt==0xffffffff) {
		uCnt=0;
		Outp32(rINFORM1, (Inp32(rINFORM1))++);
	}	
	Outp32(rINFORM2, uCnt);	

	uRstId = SYSC_RdRSTSTAT(1);	
	if( ( uRstId == 3 ) && ((Inp32(rINFORM3)==0xDEADDEAD )) )
	{
		UART_Printf("Wake-up form SLEEP Mode \n");
		SYSC_CheckData_SDRAM(_DRAM_BaseAddress+0x1000000, 0x10000);

		//Check & Clear Wake-up Source 
		SYSC_ClrWKUPSTAT();
		//Read RawInterrupt Status
		UART_Printf("VIC0RawStatus: 0x%x \n", Inp32(0x71200008));
		UART_Printf("VIC1RawStatus: 0x%x \n", Inp32(0x71300008));
	}
	else			// Entering into SLEEP Mode
	{		
		UART_Printf("[SLEEP Mode Test]\n");
		UART_Printf("S3C6410 will wake-up by EINT9~11 or Other wake-up sources\n");

		UART_Printf("Test pattern for SDRAM Self-Refresh is filled!\n");
		SYSC_InitData_SDRAM(_DRAM_BaseAddress+0x1000000, 0x10000);

		// ADC & RTC OFF		
		ADC_SelectStandbyMode(1);		//ADC Stand-by
		//-daedoo
		//RTC_SetCON(0,0,0,0,0,0);			//RTC SFR R/W Disable.
		RTC_Enable(false);

		////////////////////////////////
		//  Save I/O Ports 
		////////////////////////////////
		// Save the port configurations
		//UART_Printf("I/O Port is stored!\n");
		//for(i=0;i<36;i++) {
		//	portStatus[i]=*( (volatile U32 *)0x56000000 + i);	// 0x5600_0000:GPACON addr.
		//	}
		//UART_Printf("I/O Port is set for Stop Mode!\n");
		//ConfigSleepGPIO();

		
		UART_Printf("Wake-up source is set!\n");
		////////////////////////////////
		// Wake-up Source Setting
		//      Wake-up Source Interrupts are must enabled at Stop Mode
		////////////////////////////////
   		SYSC_SetAlarmWakeup();
		SYSC_SetEintWakeup();
		Outp32SYSC(0x804, 0x1<<7);			// nBATF Wake-up Mask disable
		SetBatteryFaultEvent(eFLT_INT);
	

		UART_Printf("System stablilization counter register!\n");
		////////////////////////////////
		// OSCSTABLE = Clock Control
		// PWRSTABLE = Reset Control
		////////////////////////////////	
		SYSC_OSCSTAB(0x1);			
		SYSC_PWRSTAB(0x1);			

		UART_Printf("PWRCFG:0x%x, EINTMASK:0x%x, NORMALCFG:0x%x, STOPCFG:0x%x\n", Inp32SYSC(0x804),Inp32SYSC(0x808),Inp32SYSC(0x810),Inp32SYSC(0x814));
		UART_Printf(" SLEEPCFG:0x%x, STOPMEMCFG:0x%x, OTHERS:0x%x, BLKPWRSTAT:0x%x\n",Inp32SYSC(0x818),Inp32SYSC(0x81c),Inp32SYSC(0x900),Inp32SYSC(0x90c)); 
		UART_Printf("\nCPU will go to SLEEP Mode!\n\n");
		UART_TxEmpty(); 			// 


		Outp32(rINFORM3, 0xDEADDEAD);

		SYSC_SetCFGWFI(eSLEEP, 0);	
		MMU_WaitForInterrupt();
		

		UART_Printf("CPU doesn't go to Sleep Mode\n");

	}

}



//////////
// Function Name : SYSC_SLEEP
// Function Description : Sleep Mode Test - Baisc Test 
// Input : 	None
// Output :	None 
// Version : v0.1
void SYSC_SLEEP (void)
{
	u32 uRstId;	

	u32 uOsc;
	
	uRstId = SYSC_RdRSTSTAT(1);
	
	if( ( uRstId == 3 ) && ((Inp32(rINFORM3)==0xDEADDEAD )) )
	{
		UART_Printf("Wake-up form SLEEP Mode \n");
		SYSC_CheckData_SDRAM(_DRAM_BaseAddress+0x1000000, 0x10000);

		//Check & Clear Wake-up Source 
		SYSC_ClrWKUPSTAT();
		//Read RawInterrupt Status
		UART_Printf("VIC0RawStatus: 0x%x \n", Inp32(0x71200008));
		UART_Printf("VIC1RawStatus: 0x%x \n", Inp32(0x71300008));

		
		UART_Printf("SLEEP mode test is done\n");
	}
	else			// Entering into SLEEP Mode
	{

		
		UART_Printf("[SLEEP Mode Test]\n");
		UART_Printf("S3C6410 will wake-up by EINT9~11 or Other wake-up sources\n");

		UART_Printf("Test pattern for SDRAM Self-Refresh is filled!\n");
		SYSC_InitData_SDRAM(_DRAM_BaseAddress+0x1000000, 0x10000);

		// ADC & RTC OFF		
		ADC_SelectStandbyMode(1);		//ADC Stand-by
		//-daedoo
		//RTC_SetCON(0,0,0,0,0,0);			//RTC SFR R/W Disable.
		RTC_Enable(false);

		////////////////////////////////
		//  Save I/O Ports 
		////////////////////////////////
		// Save the port configurations
		//UART_Printf("I/O Port is stored!\n");
		//for(i=0;i<36;i++) {
		//	portStatus[i]=*( (volatile U32 *)0x56000000 + i);	// 0x5600_0000:GPACON addr.
		//	}
		//UART_Printf("I/O Port is set for Stop Mode!\n");
		//ConfigSleepGPIO();

		
		UART_Printf("Wake-up source is set!\n");
		////////////////////////////////
		// Wake-up Source Setting
		//      Wake-up Source Interrupts are must enabled at Stop Mode
		////////////////////////////////
   		SYSC_SetAlarmWakeup();
		SYSC_SetEintWakeup();
		Outp32SYSC(0x804, 0x1<<7);			// nBATF Wake-up Mask disable
		SetBatteryFaultEvent(eFLT_INT);
	

		UART_Printf("System stablilization counter register!\n");
		////////////////////////////////
		// OSCSTABLE = Clock Control
		// PWRSTABLE = Reset Control
		////////////////////////////////	
		SYSC_OSCSTAB(0x1);			
		SYSC_PWRSTAB(0x1);			


		UART_Printf("PWRCFG:0x%x, EINTMASK:0x%x, NORMALCFG:0x%x, STOPCFG:0x%x\n", Inp32SYSC(0x804),Inp32SYSC(0x808),Inp32SYSC(0x810),Inp32SYSC(0x814));
		UART_Printf(" SLEEPCFG:0x%x, STOPMEMCFG:0x%x, OTHERS:0x%x, BLKPWRSTAT:0x%x\n",Inp32SYSC(0x818),Inp32SYSC(0x81c),Inp32SYSC(0x900),Inp32SYSC(0x90c)); 
		UART_Printf("\nCPU will go to SLEEP Mode!\n\n");
		UART_TxEmpty(); 			// 

		////////////////////////////////
		// Enter the Stop Mode
		// 		1. Setting "CFG_STANDBYWFI" to "2'b11"  ( PWR_CFG(0x7E00_F804)
		//          2. Setting "SLEEP_CFG"(0x7E00_F818) ->  OSC_EN ="0"
		//          2.  Enter to the STANDBYWFI Command
		////////////////////////////////

		Outp32(rINFORM3, 0xDEADDEAD);

		SYSC_SetCFGWFI(eSLEEP, 0);	
		MMU_WaitForInterrupt();
		

		UART_Printf("CPU doesn't go to Sleep Mode\n");

	}

}




//////////
// Function Name : SYSC_IDLE
// Function Description : IDLE Mode Test - Baisc Test 
// Input : 	None
// Output :	None 
// Version : v0.1
void SYSC_IDLE_Current (void)
{
	u32 uRegValue;
	u32 ucnt;

	
	UART_Printf("[IDLE Mode Baisc Test]\n");
	UART_Printf("After 10 seconds, S3C6410 will wake up by RTC alarm interrupt.\n");
	UART_Printf("S3C6410 will also wake up by EINT9, ADCTS, or BATT_FLT.\n");


	ucnt=Inp32(rINFORM0);	
	ucnt++;
	if(ucnt==0xffffffff){
		ucnt=0;
		Outp32(rINFORM1, (Inp32(rINFORM1))++);
	}
	Outp32(rINFORM0, ucnt);
	UART_Printf("[%d] %d-th IDLE\n",  Inp32(rINFORM1), ucnt);
	
	////////////////////////////////
	// Wake-up Source Setting
	////////////////////////////////
	UART_Printf("Wake-up sourceis set! \n");

	SYSC_SetAlarmWakeup();
	SYSC_SetEintWakeup();
//	Outp32SYSC(0x804, 0x1<<7);			// nBATF Wake-up Mask disable
//	SYSC_SetBatteryFaultEvent(eFLT_INT);
//	SYSC_SetADCTSWakeup();
//	SYSC_SetAlarmTick();


	////////////////////////////////
	// Enter Idle Mode
	// 		1. Setting "CFG_STANDBYWFI" to "2'b01"  ( PWR_CFG(0x7E00_F804)
	//          2.  Enter to the STANDBYWFI Command
	////////////////////////////////
	
	UART_Printf("CPU will goes to IDLE Mode \n");
	UART_TxEmpty();

	SYSC_SetCFGWFI(eIDLE,0);		// IDLE,  Clock Disable
	MMU_WaitForInterrupt();


	SYSC_ClrWKUPSTAT();
	uRegValue=Inp32(rWAKEUP_STAT);
	UART_Printf("Return to Normal Mode \n\n");
	UART_Printf("rWAKEUP_STAT=0x%x\n", uRegValue);
	Outp32(rWAKEUP_STAT, uRegValue);
	UART_Printf("rWAKEUP_STAT=0x%x\n Wake-up Status Clear\n", uRegValue);

	// Int. Disable
	INTC_Disable(NUM_EINT1);
	INTC_Disable(NUM_RTC_ALARM);
	INTC_Disable(NUM_BATF);
	INTC_Disable(NUM_PENDNUP);
	INTC_Disable(NUM_RTC_TIC);

}


//////////
// Function Name : SYSC_STOP
// Function Description : Stop Mode Test - Baisc Test 
// Input : 	None
// Output :	None 
// Version : v0.1
void SYSC_STOP_Current (void)
{
	u32 uRegValue;

	u32 uPclkGate, uHclkGate, uSclkGate;
	u32 uNormalCfg, uStopCfg, uStopMemCfg;

	
	UART_Printf("\n[STOP Mode Basic Test]\n");
	UART_Printf("After 10 seconds, S3C6410 will wake up by RTC alarm interrupt.\n");
	UART_Printf("S3C6410 will also wake up by EINT9, ADCTS, or BATT_FLT.\n");


	////////////////////////////////
	//  Save I/O Ports 
	////////////////////////////////
	// Save the port configurations
	//UART_Printf("I/O Port is stored!\n");
	//for(i=0;i<36;i++) {
	//	portStatus[i]=*( (volatile U32 *)0x56000000 + i);	// 0x5600_0000:GPACON addr.
	//	}
	//UART_Printf("I/O Port is set for Stop Mode!\n");
	//ConfigStopGPIO();
	//ConfigMiscIO_stop();

	dprintf("Test pattern for SDRAM Self-Refresh is filled!\n");
	SYSC_InitData_SDRAM(_DRAM_BaseAddress+0x1000000, 0x10000);

	UART_Printf("Wake-up source is set!\n");
	////////////////////////////////
	// Wake-up Source Setting
	//      Wake-up Source Interrupts are must enabled at Stop Mode
	////////////////////////////////
	SYSC_SetAlarmWakeup();
	SYSC_SetEintWakeup();
//	SYSC_SetADCTSWakeup();
//	Outp32SYSC(0x804, 0x1<<7);			// nBATF Wake-up Mask disable
//	SYSC_SetBatteryFaultEvent(eFLT_INT);	


	SYSC_OSCSTAB(1);			
	   
	SYSC_SetSTOPCFG(1, 1, 1, 1);		// Set ARM_LOGIC/MEM, TOP_LOGIC/MEM to "1"
	SYSC_SetSTOPMEMCFG(1, 1, 1, 1, 1, 1, 1);	

	UART_Printf("CPU will go to Stop Mode!\n");
	UART_TxEmpty();

	SYSC_SetCFGWFI(eSTOP, 0);
	MMU_WaitForInterrupt();
	

	UART_Printf("nCPU will go to Normal Mode!\n");
	SYSC_ClrWKUPSTAT();	
	SYSC_CheckData_SDRAM(_DRAM_BaseAddress+0x1000000, 0x10000);
	
	INTC_Disable(NUM_EINT1);
	INTC_Disable(NUM_RTC_ALARM);
	INTC_Disable(NUM_BATF);
	INTC_Disable(NUM_PENDNUP);

}


//////////
// Function Name : SYSC_STOP
// Function Description : Stop Mode Test - Baisc Test 
// Input : 	None
// Output :	None 
// Version : v0.1
void SYSC_STOP_Current2 (void)
{
	u32 uRegValue;

	u32 uPclkGate, uHclkGate, uSclkGate;
	u32 uNormalCfg, uStopCfg, uStopMemCfg;

	
	UART_Printf("\n[STOP Mode Basic Test]\n");
	UART_Printf("After 10 seconds, S3C6410 will wake up by RTC alarm interrupt.\n");
	UART_Printf("S3C6410 will also wake up by EINT9, ADCTS, or BATT_FLT.\n");


	////////////////////////////////
	//  Save I/O Ports 
	////////////////////////////////
	// Save the port configurations
	//UART_Printf("I/O Port is stored!\n");
	//for(i=0;i<36;i++) {
	//	portStatus[i]=*( (volatile U32 *)0x56000000 + i);	// 0x5600_0000:GPACON addr.
	//	}
	//UART_Printf("I/O Port is set for Stop Mode!\n");
	//ConfigStopGPIO();
	//ConfigMiscIO_stop();

	dprintf("Test pattern for SDRAM Self-Refresh is filled!\n");
	SYSC_InitData_SDRAM(_DRAM_BaseAddress+0x1000000, 0x10000);

	UART_Printf("Wake-up source is set!\n");
	////////////////////////////////
	// Wake-up Source Setting
	//      Wake-up Source Interrupts are must enabled at Stop Mode
	////////////////////////////////
	SYSC_SetAlarmWakeup();
	SYSC_SetEintWakeup();
//	SYSC_SetADCTSWakeup();
//	Outp32SYSC(0x804, 0x1<<7);			// nBATF Wake-up Mask disable
//	SYSC_SetBatteryFaultEvent(eFLT_INT);	


	SYSC_OSCSTAB(1);			
//	SYSC_SetSTOPCFG(1, 1, 1, 1);		// Set ARM_LOGIC/MEM, TOP_LOGIC/MEM to "1"	
//	SYSC_SetSTOPMEMCFG(1, 1, 1, 1, 1, 1, 1);	

	Outp32(rNORMAL_CFG, 0xfffff700);
	Outp32(rSTOP_CFG, 0xfffff700);
	Outp32(rSTOP_MEM_CFG, 0x7f);

	UART_Printf("CPU will go to Stop Mode!\n");
	UART_TxEmpty();

	SYSC_SetCFGWFI(eSTOP, 0);
	MMU_WaitForInterrupt();
	

	UART_Printf("nCPU will go to Normal Mode!\n");
	SYSC_ClrWKUPSTAT();	
	SYSC_CheckData_SDRAM(_DRAM_BaseAddress+0x1000000, 0x10000);
	
	INTC_Disable(NUM_EINT1);
	INTC_Disable(NUM_RTC_ALARM);
	INTC_Disable(NUM_BATF);
	INTC_Disable(NUM_PENDNUP);

}


//////////
// Function Name : SYSC_SLEEP0
// Function Description : Sleep Mode Test - Baisc Test 
// Input : 	None
// Output :	None 
// Version : v0.1
void SYSC_SLEEP_Current(void)
{
	u32 uRstId;	
	u32 ucnt;

	UART_Printf("[%d]%d-th Test!\n", Inp32(rINFORM1), Inp32(rINFORM2));

	uRstId = SYSC_RdRSTSTAT(1);

	if( Inp32(rINFORM2)==0 )
	{
		dprintf("Test pattern for SDRAM Self-Refresh is filled!\n");
		SYSC_InitData_SDRAM(_DRAM_BaseAddress+0x1000000, 0x10000);
	}
//	else if((Inp32(rINFORM1)!=0) && !(Inp32(rSTOP_CFG)&1<<17))  {
	else if((Inp32(rINFORM2)!=0) ) {
		dprintf("Wake-up form Sleep Mode \n");
		SYSC_CheckData_SDRAM(_DRAM_BaseAddress+0x1000000, 0x10000);
		SYSC_ClrWKUPSTAT();
		Outp32(rINFORM3, 0x0);
	}	

	ucnt=Inp32(rINFORM2);
	ucnt++;
	if(ucnt==0xffffffff) {
		ucnt=0;
		Outp32(rINFORM1, (Inp32(rINFORM1))++);
	}	
	Outp32(rINFORM2, ucnt);	


	// ADC & RTC OFF		
	ADC_SelectStandbyMode(1);		//ADC Stand-by
	//-daedoo
	//RTC_SetCON(0,0,0,0,0,0);			//RTC SFR R/W Disable.
	RTC_Enable(false);
	
	////////////////////////////////
	//  Save I/O Ports 
	////////////////////////////////
	// Save the port configurations
	//UART_Printf("I/O Port is stored!\n");
	//for(i=0;i<36;i++) {
	//	portStatus[i]=*( (volatile U32 *)0x56000000 + i);	// 0x5600_0000:GPACON addr.
	//	}
	//UART_Printf("I/O Port is set for Stop Mode!\n");
	//ConfigSleepGPIO();
		
	UART_Printf("Wake-up source is set!\n");
	////////////////////////////////
	// Wake-up Source Setting
	//      Wake-up Source Interrupts are must enabled at Stop Mode
	////////////////////////////////
	SYSC_SetAlarmWakeup();
	SYSC_SetEintWakeup();
//	Outp32(rPWR_CFG, 0x1<<7);			// nBATF Wake-up Mask disable
//	SYSC_SetBatteryFaultEvent(eFLT_INT);
//	UART_Printf("System stablilization counter register!\n");
	////////////////////////////////
	// OSCSTABLE = Clock Control
	// PWRSTABLE = Reset Control
	////////////////////////////////	
	SYSC_OSCSTAB(0x1);			
	SYSC_PWRSTAB(0x1);			


//	SYSC_SetNORCFG((ucnt&(0x1<<0))>>0,(ucnt&(0x1<<1))>>1,(ucnt&(0x1<<2))>>2,(ucnt&(0x1<<3))>>3,(ucnt&(0x1<<4))>>4,(ucnt&(0x1<<5))>>5,(ucnt&(0x1<<6))>>6,(ucnt&(0x1<<7))>>7,(ucnt&(0x1<<8))>>8); 


	////////////////////////////////
	// Enter the Stop Mode
	// 		1. Setting "CFG_STANDBYWFI" to "2'b11"  ( PWR_CFG(0x7E00_F804)
	//          2. Setting "SLEEP_CFG"(0x7E00_F818) ->  OSC_EN ="0"
	//          2.  Enter to the STANDBYWFI Command
	////////////////////////////////

	UART_Printf("PWRCFG:0x%x, EINTMASK:0x%x, NORMALCFG:0x%x, STOPCFG:0x%x\n", Inp32SYSC(0x804),Inp32SYSC(0x808),Inp32SYSC(0x810),Inp32SYSC(0x814));
	UART_Printf(" SLEEPCFG:0x%x, STOPMEMCFG:0x%x, OTHERS:0x%x, BLKPWRSTAT:0x%x\n",Inp32SYSC(0x818),Inp32SYSC(0x81c),Inp32SYSC(0x900),Inp32SYSC(0x90c)); 
	UART_Printf("\nCPU will go to SLEEP Mode!\n\n");
	UART_TxEmpty();				// 
	Outp32(rINFORM3, 0xDEADDEAD);
	SYSC_SetCFGWFI(eSLEEP, 0);		
	MMU_WaitForInterrupt();
	
	UART_Printf("CPU doesn't go to Sleep Mode\n");


}


//////////
// Function Name : SYSC_DEEPSTOP
// Function Description : Deep Stop Mode Test - Baisc Test 
// Input : 	None
// Output :	None 
// Version : v0.1
void SYSC_DEEPSTOP_Current(void)
{
	u32 uRegValue;

	u32 uPclkGate, uHclkGate, uSclkGate;
	u32 uNormalCfg, uStopCfg, uStopMemCfg;

	u32 uSel;
	u32 ucnt;
	u32 uOsc;
	u32 i;


	
	UART_Printf("\n[STOP Mode Basic Test]\n");
	dprintf("After 10 seconds, S3C6410 will wake up by RTC alarm interrupt.\n");
	dprintf("S3C6410 will also wake up by EINT9, ADCTS, or BATT_FLT.\n");


	////////////////////////////////
	//  Save I/O Ports 
	////////////////////////////////
	// Save the port configurations
	//UART_Printf("I/O Port is stored!\n");
	//for(i=0;i<36;i++) {
	//	portStatus[i]=*( (volatile U32 *)0x56000000 + i);	// 0x5600_0000:GPACON addr.
	//	}
	//UART_Printf("I/O Port is set for Stop Mode!\n");
	//ConfigStopGPIO();
	//ConfigMiscIO_stop();

	UART_Printf("[%d]%d-th Test!\n", Inp32(rINFORM1), Inp32(rINFORM2));

	if( Inp32(rINFORM2)==0 )
	{
		dprintf("Test pattern for SDRAM Self-Refresh is filled!\n");
		SYSC_InitData_SDRAM(_DRAM_BaseAddress+0x1000000, 0x10000);
	}
//	else if((Inp32(rINFORM1)!=0) && !(Inp32(rSTOP_CFG)&1<<17))  {
	else if((Inp32(rINFORM2)!=0) ) {
		dprintf("Wake-up form Stop Mode \n");
		SYSC_CheckData_SDRAM(_DRAM_BaseAddress+0x1000000, 0x10000);
		if(!(Inp32(rSTOP_CFG)&1<<17)) {
			SYSC_ClrWKUPSTAT();
			Outp32(rINFORM3, 0x0);
		}
	}	

	ucnt=Inp32(rINFORM2);
	ucnt++;
	if(ucnt==0xffffffff) {
		ucnt=0;
		Outp32(rINFORM1, (Inp32(rINFORM1))++);
	}	
	Outp32(rINFORM2, ucnt);

			

	////////////////////////////////
	// Wake-up Source Setting
	//      Wake-up Source Interrupts are must enabled at Stop Mode
	////////////////////////////////
	SYSC_SetAlarmWakeup();
	SYSC_SetEintWakeup();
//	Outp32SYSC(0x804, 0x1<<7);			// nBATF Wake-up Mask disable
//	SYSC_SetBatteryFaultEvent(eFLT_INT);
//	SYSC_SetADCTSWakeup();

       /////////////////////////////////
       // 1) Check&Enable HCLK_GATE[22,21,0]
       // 2) Check BLK_PWR_STAT 
       // 3) Check&Clear WAKEUP_STAT
       //////////////////////////////

	uPclkGate=Inp32(rPCLK_GATE);
	uSclkGate=Inp32(rSCLK_GATE);	
	uHclkGate=Inp32(rHCLK_GATE);	
	uNormalCfg=Inp32(rNORMAL_CFG);
	uStopCfg=Inp32(rSTOP_CFG);
	uStopMemCfg=Inp32(rSTOP_MEM_CFG);	
	
	//All Clocks and powers are enabled.
	SYSC_SetCLKGateAll(1);


	SYSC_OSCSTAB(0x1);			
	SYSC_PWRSTAB(0x1);			

#if 0
// NOR Booting
	SYSC_SetNORCFG(0,1,0,0,0,0,1,0,0); 
	SYSC_SetSTOPCFG(0,1,0,1);
	SYSC_SetSTOPMEMCFG(1,1,1,1,1,1,1);
//	Outp32(rINFORM3, 0xdeaddead);
#elif 0
	SYSC_SetNORCFG((ucnt&(0x1<<3))>>3,(ucnt&(0x1<<4))>>4,(ucnt&(0x1<<5))>>5,(ucnt&(0x1<<6))>>6,(ucnt&(0x1<<7))>>7,(ucnt&(0x1<<8))>>8,(ucnt&(0x1<<9))>>9,(ucnt&(0x1<<10))>>10,(ucnt&(0x1<<11))>>11); 
	SYSC_SetSTOPCFG((ucnt&(0x1<<1))>>1,  (ucnt&(0x1<<0))>>0, (ucnt&(0x1<<1))>>1, (ucnt&(0x1<<2))>>2);
	SYSC_SetSTOPMEMCFG((ucnt&(0x1<<12))>>12,(ucnt&(0x1<<13))>>13,(ucnt&(0x1<<14))>>14,(ucnt&(0x1<<15))>>15,(ucnt&(0x1<<16))>>16,(ucnt&(0x1<<17))>>17,(ucnt&(0x1<<18))>>18);
	if(!(Inp32(rSTOP_CFG)&1<<17))
			Outp32(rINFORM3, 0xdeaddead);
#elif 0	
	SYSC_SetNORCFG((ucnt&(0x1<<4))>>4,(ucnt&(0x1<<5))>>5,(ucnt&(0x1<<6))>>6,(ucnt&(0x1<<7))>>7,(ucnt&(0x1<<8))>>8,(ucnt&(0x1<<9))>>9,(ucnt&(0x1<<10))>>10,(ucnt&(0x1<<11))>>11,(ucnt&(0x1<<12))>>12); 
	SYSC_SetSTOPCFG((ucnt&(0x1<<0))>>0,  (ucnt&(0x1<<1))>>1, (ucnt&(0x1<<2))>>2, (ucnt&(0x1<<3))>>3);
	SYSC_SetSTOPMEMCFG((ucnt&(0x1<<13))>>13,(ucnt&(0x1<<14))>>14,(ucnt&(0x1<<15))>>15,(ucnt&(0x1<<16))>>16,(ucnt&(0x1<<17))>>17,(ucnt&(0x1<<18))>>18,(ucnt&(0x1<<19))>>19);
	if(!(Inp32(rSTOP_CFG)&1<<17))
			Outp32(rINFORM3, 0xdeaddead);
#else
	SYSC_SetNORCFG(0,0,0,0,0,0,0,0,0); 
	SYSC_SetSTOPCFG(0,0,0,0);
	SYSC_SetSTOPMEMCFG(0,0,0,0,0,0,0);

//	SYSC_SetSTOPCFG(0,1,0,0);
//	SYSC_SetSTOPMEMCFG(0,0,0,0,0,0,1);

	
	if(!(Inp32(rSTOP_CFG)&1<<17))
			Outp32(rINFORM3, 0xdeaddead);	
#endif

	for(i=0x0c000000; i<0x0c000000+0x2000; i+=4)
		*(unsigned int *)i=i;



	UART_Printf("PWRCFG:0x%x, EINTMASK:0x%x, NORMALCFG:0x%x, STOPCFG:0x%x\n", Inp32SYSC(0x804),Inp32SYSC(0x808),Inp32SYSC(0x810),Inp32SYSC(0x814));
	UART_Printf(" SLEEPCFG:0x%x, STOPMEMCFG:0x%x, OTHERS:0x%x, BLKPWRSTAT:0x%x\n",Inp32SYSC(0x818),Inp32SYSC(0x81c),Inp32SYSC(0x900),Inp32SYSC(0x90c)); 


//	dprintf("PWRCFG:0x%x, EINTMASK:0x%x, NORMALCFG:0x%x, STOPCFG:0x%x\n", Inp32SYSC(0x804),Inp32SYSC(0x808),Inp32SYSC(0x810),Inp32SYSC(0x814));
//	dprintf(" SLEEPCFG:0x%x, STOPMEMCFG:0x%x, OTHERS:0x%x, BLKPWRSTAT:0x%x\n",Inp32SYSC(0x818),Inp32SYSC(0x81c),Inp32SYSC(0x900),Inp32SYSC(0x90c)); 
	UART_Printf("S3C6410 will go to Stop Mode!\n");	
	UART_TxEmpty();

	SYSC_SetCFGWFI(eSTOP, 0);

#if 0
	if(!(Inp32(rSTOP_CFG)&1<<20)) {		
		UART_Printf("cache\n");
		MMU_WaitForInterruptWithCache();
	}	
	else
#endif		
		MMU_WaitForInterrupt();
	
	
	////////////////////////////////
	//  Restore  I/O Ports 
	////////////////////////////////
	
/*
	//Restore the port configurations
	UART_Printf("\nI/O Port is restored!\n");
	for(i=0;i<36;i++){
		*( (volatile U32 *)0x56000000 + i)=portStatus[i];
	}
*/
		
#ifdef 0
	Outp32(rSTOP_MEM_CFG, uStopMemCfg);
	Outp32(rSTOP_CFG, uStopCfg);
	Outp32(rNORMAL_CFG, uNormalCfg);
	Outp32(rHCLK_GATE, uHclkGate);
	Outp32(rSCLK_GATE, uSclkGate);
	Outp32(rPCLK_GATE, uPclkGate);
#endif


	//Check & Clear Wake-up Source 


	SYSC_SetNORCFG(1,1,1,1,1,1,0,1,1);
	SYSC_WaitingForBLKPWRAll(1);
//	OpenConsole();
	SYSC_RdBLKPWR();

	dprintf("Wake-up form Stop Mode \n");
	SYSC_CheckData_SDRAM(_DRAM_BaseAddress+0x1000000, 0x10000);
	SYSC_ClrWKUPSTAT();


//	DisplayLED(0x3);



	// Int. Disable
	INTC_Disable(NUM_EINT1);
	INTC_Disable(NUM_RTC_ALARM);
	INTC_Disable(NUM_BATF);
	INTC_Disable(NUM_PENDNUP);

}



void SYSC_SDHC_Clock_On(u32 uCh)
{
	SDHC sCh;
	int uDivisor = 0;
	SDHC_OpenMediaWithMode( 1, SDHC_POLLING_MODE, SDHC_EPLL, 0, uCh, &sCh);
	Outp32( sCh.m_uBaseAddr+SDHC_CONTROL2, (0x0<<15)|(0x0<<14)|(0x1<<8)|(SDHC_EPLL<<4) );
	Outp16( sCh.m_uBaseAddr+SDHC_CLK_CTRL, (Inp16(sCh.m_uBaseAddr+SDHC_CLK_CTRL)&(~(0xff<<8)))|(uDivisor<<8)|(1<<0) );
	SDHC_SetSdClockOnOff(1, &sCh); // If the sd clock is to be changed, you need to stop sd-clock.
}


