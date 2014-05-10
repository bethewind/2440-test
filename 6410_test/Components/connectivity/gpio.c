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
*	File Name : gpio.c
*  
*	File Description : This file implements the API functons for GPIO.
*
*	Author : Heemyung.Noh
*	Dept. : AP Development Team
*	Created Date : 2006/12/26
*	Version : 0.1 
* 
*	History
*	- Created(Heemyung.Noh 2006/12/26)
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
#include "intc.h"
#include "gpio.h"

#define GPIO 		( ( volatile oGPIO_REGS * ) (GPIO_pBase) )


typedef struct tag_GPIO_REGS
{
	u32 rGPIOACON;			//0x7F008000
	u32 rGPIOADAT;
	u32 rGPIOAPUD;
	u32 rGPIOACONSLP;
	u32 rGPIOAPUDSLP;
	u32 reserved1[3];
	
	u32 rGPIOBCON;			//0x7F008020
	u32 rGPIOBDAT;
	u32 rGPIOBPUD;
	u32 rGPIOBCONSLP;
	u32 rGPIOBPUDSLP;	
	u32 reserved2[3];
		
	u32 rGPIOCCON;			//0x7F008040
	u32 rGPIOCDAT;
	u32 rGPIOCPUD;
	u32 rGPIOCCONSLP;
	u32 rGPIOCPUDSLP;	
	u32 reserved3[3];
		
	u32 rGPIODCON;			//0x7F008060
	u32 rGPIODDAT;
	u32 rGPIODPUD;
	u32 rGPIODCONSLP;
	u32 rGPIODPUDSLP;	
	u32 reserved4[3];
		
	u32 rGPIOECON;			//0x7F008080
	u32 rGPIOEDAT;
	u32 rGPIOEPUD;
	u32 rGPIOECONSLP;
	u32 rGPIOEPUDSLP;	
	u32 reserved5[3];
		
	u32 rGPIOFCON;			//0x7F0080A0
	u32 rGPIOFDAT;
	u32 rGPIOFPUD;
	u32 rGPIOFCONSLP;
	u32 rGPIOFPUDSLP;	
	u32 reserved6[3];
		
	u32 rGPIOGCON;			//0x7F0080C0
	u32 rGPIOGDAT;
	u32 rGPIOGPUD;
	u32 rGPIOGCONSLP;
	u32 rGPIOGPUDSLP;	
	u32 reserved7[3];
	
	u32 rGPIOHCON0;			//0x7F0080E0
	u32 rGPIOHCON1;
	u32 rGPIOHDAT;
	u32 rGPIOHPUD;
	u32 rGPIOHCONSLP;
	u32 rGPIOHPUDSLP;	
	u32 reserved8[2];

	u32 rGPIOICON;			//0x7F008100
	u32 rGPIOIDAT;
	u32 rGPIOIPUD;
	u32 rGPIOICONSLP;
	u32 rGPIOIPUDSLP;	
	u32 reserved9[3];

	u32 rGPIOJCON;			//0x7F008120
	u32 rGPIOJDAT;
	u32 rGPIOJPUD;
	u32 rGPIOJCONSLP;
	u32 rGPIOJPUDSLP;	
	u32 reserved10[3];
	
	u32 rGPIOOCON;			//0x7F008140
	u32 rGPIOODAT;
	u32 rGPIOOPUD;
	u32 rGPIOOCONSLP;
	u32 rGPIOOPUDSLP;	
	u32 reserved11[3];	

	u32 rGPIOPCON;			//0x7F008160
	u32 rGPIOPDAT;
	u32 rGPIOPPUD;
	u32 rGPIOPCONSLP;
	u32 rGPIOPPUDSLP;	
	u32 reserved12[3];

	u32 rGPIOQCON;			//0x7F008180
	u32 rGPIOQDAT;
	u32 rGPIOQPUD;
	u32 rGPIOQCONSLP;
	u32 rGPIOQPUDSLP;	
	u32 reserved13[3];	

	u32 rSPCON;				//0x7F0081A0
	u32 reserved14[3];
	u32 rMEM0CONSTOP;		//0x7F0081B0
	u32 rMEM1CONSTOP;		//0x7F0081B4
	u32 reserved15[2];
	u32 rMEM0CONSLP0;		//0x7F0081C0
	u32 rMEM0CONSLP1;		//0x7F0081C4
	u32 rMEM1CONSLP;		//0x7F0081C8
	u32 reserved;
	u32 rMEM0DRVCON;		//0x7F0081D0
	u32 rMEM1DRVCON;		//0x7F0081D4
	u32 reserved16[10];

	u32 rEINT12CON;			//0x7f008200
	u32 rEINT34CON;			//0x7f008204
	u32 rEINT56CON;			//0x7f008208
	u32 rEINT78CON;			//0x7f00820C
	u32 rEINT9CON;			//0x7f008210
	u32 reserved17[3];

	u32 rEINT12FLTCON;		//0x7f008220
	u32 rEINT34FLTCON;		//0x7f008224
	u32 rEINT56FLTCON;		//0x7f008228
	u32 rEINT78FLTCON;		//0x7f00822C
	u32 rEINT9FLTCON;		//0x7f008230
	u32 reserved18[3];

	u32 rEINT12MASK;		//0x7f008240
	u32 rEINT34MASK;		//0x7f008244
	u32 rEINT56MASK;		//0x7f008248
	u32 rEINT78MASK;		//0x7f00824C
	u32 rEINT9MASK;			//0x7f008250
	u32 reserved19[3];	

	u32 rEINT12PEND;		//0x7f008260
	u32 rEINT34PEND;		//0x7f008264
	u32 rEINT56PEND;		//0x7f008268
	u32 rEINT78PEND;		//0x7f00826C
	u32 rEINT9PEND;			//0x7f008270
	u32 reserved20[3];			

	u32 rPRIORITY;			//0x7f008280
	u32 rSERVICE;			//0x7f008284
	u32 rSERVICEPEND;		//0x7f008288
	u32 reserved21;

	u32 reserved22[348];
	
	u32 rGPIOKCON0;			//0x7f008800
	u32 rGPIOKCON1;			//0x7f008804
	u32 rGPIOKDAT;			//0x7f008808
	u32 rGPIOKPUD;			//0x7f00880c

	u32 rGPIOLCON0;			//0x7f008810
	u32 rGPIOLCON1;			//0x7f008814
	u32 rGPIOLDAT;			//0x7f008818
	u32 rGPIOLPUD;			//0x7f00881c

	u32 rGPIOMCON;			//0x7f008820
	u32 rGPIOMDAT;			//0x7f008824
	u32 rGPIOMPUD;			//0x7f008828	
	u32 reserved23;

	u32 rGPIONCON;			//0x7f008830
	u32 rGPIONDAT;			//0x7f008834
	u32 rGPIONPUD;			//0x7f008838	
	u32 reserved24;

	u32 reserved25[16];

	u32 rSPCONSLP;			//0x7f008880

	u32 reserved26[31];		

	u32 rEINT0CON0;			//0x7f008900
	u32 rEINT0CON1;			//0x7f008904
	u32 reserved27[2];

	u32 rEINT0FLTCON0;		//0x7f008910
	u32 rEINT0FLTCON1;		//0x7f008914
	u32 rEINT0FLTCON2;		//0x7f008918
	u32 rEINT0FLTCON3;		//0x7f00891c
	u32 rEINT0MASK;			//0x7f008920
	u32 rEINT0PEND;			//0x7f008924
	u32 reserved28[2];
	u32 rSLPEN;				//0x7f008930

} 
oGPIO_REGS;

static void *GPIO_pBase;

u32  g_IntCnt;



//////////
// Function Name : GPIO_Init
// Function Desctiption : This function initializes gpio sfr base address
// Input : NONE
// Output : NONE
// Version :
//
// Version : v0.1
void GPIO_Init(void)
{
	GPIO_pBase = (void *)GPIO_BASE;
}


//////////
// Function Name : GPIO_SetFunctionEach
// Function Desctiption : This function set each GPIO function
// Input : 	Id : GPIO port
//			eBitPos : GPIO bit
//			uFunction : Select the function
// Output : NONE
//
// Version : v0.1
void GPIO_SetFunctionEach(GPIO_eId Id, GPIO_eBitPos eBitPos, u32 uFunction)
{
	volatile u32 *pGPIOx_Reg0, *pGPIOx_Reg1;
	volatile u32 *pGPIO_Base_Addr;
	u32 uMuxBit, uConRegNum, uOffset;
	u32 uConValue;

	uMuxBit = (Id&0xF000)>>12;
	uConRegNum = (Id&0x0F00)>>8;
	uOffset = Id&0x00FF;

	pGPIO_Base_Addr = &(GPIO->rGPIOACON);
	
	if(uMuxBit == 4)
	{
		if(uConRegNum == 1)
		{
			pGPIOx_Reg0 = pGPIO_Base_Addr + (uOffset*0x10)/4;
			uConValue = *pGPIOx_Reg0;
			uConValue = (uConValue & ~(0xF<<(uMuxBit*eBitPos))) | (uFunction<<(uMuxBit*eBitPos));
			*pGPIOx_Reg0 = uConValue;
		}
		else if(uConRegNum == 2)
		{
			if(eBitPos < eGPIO_8)
			{
				pGPIOx_Reg0 = pGPIO_Base_Addr + (uOffset*0x10)/4;
				uConValue = *pGPIOx_Reg0;
				uConValue = (uConValue & ~(0xF<<(uMuxBit*eBitPos))) | (uFunction<<(uMuxBit*eBitPos));
				*pGPIOx_Reg0 = uConValue;
			}
			else 	// eBitPos >= eGPIO_8
			{
				pGPIOx_Reg1 = pGPIO_Base_Addr + (uOffset*0x10)/4 + 1;
				uConValue = *pGPIOx_Reg1;
				uConValue = (uConValue & ~(0xF<<(uMuxBit*(eBitPos-8)))) | (uFunction<<(uMuxBit*(eBitPos-8)));
				*pGPIOx_Reg1 = uConValue;
			}			
		}
	}
	else if(uMuxBit == 2)
	{
		pGPIOx_Reg0 = pGPIO_Base_Addr + (uOffset*0x10)/4;
		uConValue = *pGPIOx_Reg0;
		uConValue = (uConValue & ~(0x3<<(uMuxBit*eBitPos))) | (uFunction<<(uMuxBit*eBitPos));
		*pGPIOx_Reg0 = uConValue;
	}
}



//////////
// Function Name : GPIO_SetFunctionAll
// Function Desctiption : This function set all GPIO function selection
// Input : 	Id : GPIO port
//			uValue0 : Write value(control register 0)
//			uValue1 : Write value(control register 1 if exist)
// Output : NONE
//
// Version : v0.1
void GPIO_SetFunctionAll(GPIO_eId Id, u32 uValue0, u32 uValue1)
{
	volatile u32 *pGPIOx_Reg0, *pGPIOx_Reg1;
	volatile u32 *pGPIO_Base_Addr;
	u32 uConRegNum, uOffset;

	uConRegNum = (Id&0x0F00)>>8;
	uOffset = Id&0x00FF;

	pGPIO_Base_Addr = &(GPIO->rGPIOACON);
	
	if(uConRegNum == 1)
	{
		pGPIOx_Reg0 = pGPIO_Base_Addr + (uOffset*0x10)/4;
		*pGPIOx_Reg0 = uValue0;
	}
	else if(uConRegNum == 2)
	{
		pGPIOx_Reg0 = pGPIO_Base_Addr + (uOffset*0x10)/4;
		pGPIOx_Reg1 = pGPIO_Base_Addr + (uOffset*0x10)/4 + 1;
			
		*pGPIOx_Reg0 = uValue0;
		*pGPIOx_Reg1 = uValue1;
	}
}



//////////
// Function Name : GPIO_SetDataEach
// Function Desctiption : This function set each GPIO data bit
// Input : 	Id : GPIO port
//			eBitPos : GPIO bit
//			uValue : value
// Output : NONE
//
// Version : v0.1
void GPIO_SetDataEach(GPIO_eId Id, GPIO_eBitPos eBitPos, u32 uValue)
{
	volatile u32 *pGPIOx_DataReg;
	volatile u32 *pGPIO_Base_Addr;
	u32 uConRegNum, uOffset;
	u32 uDataValue;

	uConRegNum = (Id&0x0F00)>>8;
	uOffset = Id&0x00FF;

	pGPIO_Base_Addr = &(GPIO->rGPIOACON);
	
	pGPIOx_DataReg = pGPIO_Base_Addr + (uOffset*0x10)/4 + uConRegNum;
	uDataValue = *pGPIOx_DataReg;
	uDataValue = (uDataValue & ~(0x1<<eBitPos)) | (uValue<<eBitPos);
	*pGPIOx_DataReg = uDataValue;
}



//////////
// Function Name : GPIO_SetDataAll
// Function Desctiption : This function set all GPIO data bit
// Input : 	Id : GPIO port
//			uValue : value
// Output : NONE
//
// Version : v0.1
void GPIO_SetDataAll(GPIO_eId Id, u32 uValue)
{
	volatile u32 *pGPIOx_DataReg;
	volatile u32 *pGPIO_Base_Addr;
	u32 uConRegNum, uOffset;

	uConRegNum = (Id&0x0F00)>>8;
	uOffset = Id&0x00FF;

	pGPIO_Base_Addr = &(GPIO->rGPIOACON);
	
	pGPIOx_DataReg = pGPIO_Base_Addr + (uOffset*0x10)/4 + uConRegNum;
	*pGPIOx_DataReg = uValue;
}


//////////
// Function Name : GPIO_GetDataAll
// Function Desctiption : This function get all GPIO data bit
// Input : 	Id : GPIO port
//			uValue : value
// Output : 	Data register value
//
// Version : v0.1
u32 GPIO_GetDataAll(GPIO_eId Id)
{
	volatile u32 *pGPIOx_DataReg;
	volatile u32 *pGPIO_Base_Addr;
	u32 uConRegNum, uOffset;

	uConRegNum = (Id&0x0F00)>>8;
	uOffset = Id&0x00FF;

	pGPIO_Base_Addr = &(GPIO->rGPIOACON);
	
	pGPIOx_DataReg = pGPIO_Base_Addr + (uOffset*0x10)/4 + uConRegNum;
	return (*pGPIOx_DataReg);
}


//////////
// Function Name : GPIO_SetPullUpDownEach
// Function Desctiption : This function set each GPIO Pull-up/Down bits
// Input : 	Id : GPIO port
//			eBitPos : GPIO bit
//			uValue : value(2bit)
// Output : NONE
//
// Version : v0.1
void GPIO_SetPullUpDownEach(GPIO_eId Id, GPIO_eBitPos eBitPos, u32 uValue)
{
	volatile u32 *pGPIOx_PullUDReg;
	volatile u32 *pGPIO_Base_Addr;
	u32 uConRegNum, uDataRegNum, uOffset;
	u32 uPullValue;

	uConRegNum = (Id&0x0F00)>>8;
	uOffset = Id&0x00FF;
	uDataRegNum = 1;

	pGPIO_Base_Addr = &(GPIO->rGPIOACON);
	
	pGPIOx_PullUDReg = pGPIO_Base_Addr + (uOffset*0x10)/4 + (uConRegNum + uDataRegNum);
	uPullValue = *pGPIOx_PullUDReg;
	uPullValue = (uPullValue & ~(0x3<<(0x02*eBitPos))) | (uValue<<(0x02*eBitPos));
	*pGPIOx_PullUDReg = uPullValue;
}


//////////
// Function Name : GPIO_SetPullUpDownAll
// Function Desctiption : This function set all GPIO Pull-up/Down bits
// Input : 	Id : GPIO port
//			uValue : value(32bit)
// Output : NONE
//
// Version : v0.1
void GPIO_SetPullUpDownAll(GPIO_eId Id, u32 uValue)
{
	volatile u32 *pGPIOx_PullUDReg;
	volatile u32 *pGPIO_Base_Addr;
	u32 uConRegNum, uDataRegNum, uOffset;

	uConRegNum = (Id&0x0F00)>>8;
	uOffset = Id&0x00FF;
	uDataRegNum = 1;

	pGPIO_Base_Addr = &(GPIO->rGPIOACON);
	
	pGPIOx_PullUDReg = pGPIO_Base_Addr + (uOffset*0x10)/4 + (uConRegNum + uDataRegNum);
	*pGPIOx_PullUDReg = uValue;
}



//////////
// Function Name : GPIO_SetConRegSleepAll
// Function Desctiption : This function set all GPIO function when system enter to sleep mode
// Input : 	Id : GPIO port
//			uValue : value(32bit)
// Output : NONE
//
// Version : v0.1
void GPIO_SetConRegSleepAll(GPIO_eId Id, u32 uValue)
{
	volatile u32 *pGPIOx_ConSleepReg;
	volatile u32 *pGPIO_Base_Addr;
	u32 uConRegNum, uDataRegNum, uPullUDRegNum, uOffset;

	uConRegNum = (Id&0x0F00)>>8;
	uOffset = Id&0x00FF;
	uDataRegNum = 1;
	uPullUDRegNum = 1;

	pGPIO_Base_Addr = &(GPIO->rGPIOACON);
	
	pGPIOx_ConSleepReg = pGPIO_Base_Addr + (uOffset*0x10)/4 + (uConRegNum + uDataRegNum + uPullUDRegNum);
	*pGPIOx_ConSleepReg = uValue;
}


//////////
// Function Name : GPIO_SetPullUDSleepAll
// Function Desctiption : This function set all GPIO Pull-up/down when system enter to sleep mode
// Input : 	Id : GPIO port
//			uValue : value(32bit)
// Output : NONE
//
// Version : v0.1
void GPIO_SetPullUDSleepAll(GPIO_eId Id, u32 uValue)
{
	volatile u32 *pGPIOx_PullUDSleepReg;
	volatile u32 *pGPIO_Base_Addr;
	u32 uConRegNum, uDataRegNum, uPullUDRegNum, uConSleepRegNum, uOffset;

	uConRegNum = (Id&0x0F00)>>8;
	uOffset = Id&0x00FF;
	uDataRegNum = 1;
	uPullUDRegNum = 1;
	uConSleepRegNum = 1;

	pGPIO_Base_Addr = &(GPIO->rGPIOACON);
	
	pGPIOx_PullUDSleepReg = pGPIO_Base_Addr + (uOffset*0x10)/4 + (uConRegNum + uDataRegNum + uPullUDRegNum + uConSleepRegNum);
	*pGPIOx_PullUDSleepReg = uValue;
}

//////////
// Function Name : GPIO_SetLCDType
// Function Desctiption : This function set "LCD_SEL" field at SPCON register
// Input : 	eLCDType : eHOSTIF, eRGBIF, eYCbCr, eHidden
//			
// Output : NONE
//
// Version : v0.1
void GPIO_SetLCDType(LCD_eTYPE eLCDType)
{
	volatile u32 *pGPIO_SPCON_Addr;
	u32 uRegValue, uTemp;

	GPIO_pBase = (void *)GPIO_BASE;

	uTemp = eLCDType;
	
	pGPIO_SPCON_Addr = &(GPIO->rSPCON);
	uRegValue = *pGPIO_SPCON_Addr;
	uRegValue = (uRegValue & ~(0x3<<0)) | (uTemp<<0);

	*pGPIO_SPCON_Addr = uRegValue;
}



//////////
// Function Name : GPIO_SetEint0
// Function Desctiption : This function setup Eint[27:0]
// Input : 			uEINT_No: EINT No.
//					uINTType: Select EINT Type. 
//								Low, High, Falling, Rising, Both
//					uFltType :  Select Filter Type
//								DisFLT(Disable Filter), DLYFLT(Delay Filter), DIGFLT(Digital Filter)
//					uFltWidth : Digital Filter Width ( 1~0x3F)
// Output : NONE
//
// Version : v0.1
void GPIO_SetEint0(u32 uEINT_No , u32 uIntType, FLT_eTYPE eFltType,  u32 uFltWidth)
{

	volatile u32 *pEINTx_Reg, *pFLTx_Reg;
	volatile u32 *pGPIO_EINT_Addr;
	u32 uGpioPort, uFunc, uType ;	
	u32 uConValue;

	GPIO_pBase = (void *)GPIO_BASE;

	pGPIO_EINT_Addr = &(GPIO->rEINT0CON0);

	// EINT Port
	if( uEINT_No <= 15)
	{
		uGpioPort = uEINT_No;
		uFunc = 2;  					// EINT Function 
		GPIO_SetFunctionEach(eGPIO_N, (GPIO_eBitPos)uGpioPort, uFunc);		// ??
		GPIO_SetPullUpDownEach(eGPIO_N,(GPIO_eBitPos) uGpioPort, 0x0);		// disable Pull-up/dn
	}
	else if(uEINT_No >= 16 && uEINT_No <= 22)
	{
		uGpioPort = (uEINT_No - 8) ;
		uFunc = 3;
		GPIO_SetFunctionEach(eGPIO_L,(GPIO_eBitPos) uGpioPort, uFunc);   		  // ??
		GPIO_SetPullUpDownEach(eGPIO_L, (GPIO_eBitPos)uGpioPort, 0x0);

	}
	else if(uEINT_No >=23 && uEINT_No <=27)
	{
		uGpioPort = (uEINT_No - 23 );
		uFunc = 3;
		GPIO_SetFunctionEach(eGPIO_M, (GPIO_eBitPos)uGpioPort, uFunc);		// ??
		GPIO_SetPullUpDownEach(eGPIO_M, (GPIO_eBitPos)uGpioPort, 0x0);		// disable Pull-up/dn
	}
	else if (uEINT_No >= 27)
	{
		UART_Printf("Error Eint No. \n");
		Assert(0);
	}

	
	//  Interrupt Type 
	if( uEINT_No <= 15)
	{
		uType =(uEINT_No>>1);
		pEINTx_Reg = pGPIO_EINT_Addr ; 	
		uConValue = *pEINTx_Reg;
		uConValue = (uConValue & ~(0xF<<(uType*4))) | (uIntType<<(uType*4));
		*pEINTx_Reg = uConValue;
		
	}
	else if(uEINT_No >= 16 && uEINT_No <= 27)
	{
		uType = (uEINT_No>>5);
		pEINTx_Reg = pGPIO_EINT_Addr + 1;		// EINT0CON1
		uConValue= *pEINTx_Reg;
		uConValue = (uConValue & ~(0xF<<(uType*4))) | (uIntType<<(uType*4));
		*pEINTx_Reg = uConValue;
	}

	// Check Filter Width 
	if(uFltWidth >=0x40)
	{
		UART_Printf("Error Filter Width. \n");
		Assert(0);
	}

	// Fliter Type & Filter Width 
	if( uEINT_No <= 7)
	{
		uType =(uEINT_No>>1);
		pFLTx_Reg = pGPIO_EINT_Addr + 4 ; 	    // EINT0FLTCON0
		uConValue = *pFLTx_Reg;
		uConValue = (uConValue & ~(0xFF<<(uType*8))) |((uFltWidth<<(uType*8))|(eFltType<<(uType*8+6)));
		*pFLTx_Reg = uConValue;	
	}
	else if(uEINT_No >= 8  && uEINT_No <= 15)
	{
		uType =(uEINT_No>>1)-4;
		pFLTx_Reg = pGPIO_EINT_Addr + 5 ; 	    // EINT0FLTCON1
		uConValue = *pFLTx_Reg;
		uConValue = (uConValue & ~(0xFF<<(uType*8))) |((uFltWidth<<(uType*8))|(eFltType<<(uType*8+6)));
		*pFLTx_Reg = uConValue;	
	}
	else if(uEINT_No >= 16  && uEINT_No <= 23)
	{
		uType =(uEINT_No>>1)-8;
		pFLTx_Reg = pGPIO_EINT_Addr + 6 ; 	    // EINT0FLTCON2
		uConValue = *pFLTx_Reg;
		uConValue = (uConValue & ~(0xFF<<(uType*8))) |((uFltWidth<<(uType*8))|(eFltType<<(uType*8+6)));
		*pFLTx_Reg = uConValue;	
	}
	else if(uEINT_No >= 24  && uEINT_No <= 27)
	{
		uType =(uEINT_No>>1)-12;
		pFLTx_Reg = pGPIO_EINT_Addr + 7 ; 	    // EINT0FLTCON3
		uConValue = *pFLTx_Reg;
		uConValue = (uConValue & ~(0xFF<<(uType*8))) |((uFltWidth<<(uType*8))|(eFltType<<(uType*8+6)));
		*pFLTx_Reg = uConValue;	
	}

}



//////////
// Function Name : GPIO_EINT0ClrPend
// Function Desctiption : Clear Eint pending bit of the Eint[27:0]
// Input : 			uEINT_No: EINT No.
// Output : NONE
//
// Version : v0.1
void GPIO_EINT0ClrPend(u32 uEINT_No )
{

	volatile u32 *pEINT0PEND;
	u32 uConValue;

	pEINT0PEND = &(GPIO->rEINT0PEND);

	uConValue = (1<<uEINT_No);
	*pEINT0PEND = uConValue;

}

//////////
// Function Name : GPIO_EINT0DisMask
// Function Desctiption : Disable Mask bit of the Eint[27:0]
// Input : 			uEINT_No: EINT No.
// Output : NONE
//
// Version : v0.1
void GPIO_EINT0DisMask(u32 uEINT_No )
{

	volatile u32 *pMASK_Reg;
	u32 uConValue;

	pMASK_Reg = &(GPIO->rEINT0MASK);
	
	uConValue = *pMASK_Reg;
       uConValue = (uConValue & ~(0x1<<(uEINT_No)));
	*pMASK_Reg = uConValue;

}

//////////
// Function Name : GPIO_EINT0EnMask
// Function Desctiption : Enable Mask bit of the Eint[27:0]
// Input : 			uEINT_No: EINT No.
// Output : NONE
//
// Version : v0.1
void GPIO_EINT0EnMask(u32 uEINT_No )
{

	volatile u32 *pMASK_Reg;
	u32 uConValue;

	pMASK_Reg = &(GPIO->rEINT0MASK);
	
	uConValue = *pMASK_Reg;
       uConValue = (uConValue & ~(0x1<<(uEINT_No)))|(1<<uEINT_No);
	*pMASK_Reg = uConValue;

}






//////////
// Function Name : GPIO_SetEint1
// Function Desctiption : This function setup Eint Group 1(EINT1[14:0] -> GPA[7:0], GPB[6:0])
// Input : 			uEINT_No: EINT No.
//					uINTType: Select EINT Type. 
//								Low, High, Falling, Rising, Both
//					uFltType :  Select Filter Type
//								DisFLT(Disable Filter), EnFLT(Digital Filter)
//					uFltWidth : Filter Width
// Output : NONE
//
// Version : v0.1
void GPIO_SetEint1(u32 uEINT_No , u32 uIntType, FLT_eTYPE eFltType,  u32 uFltWidth)
{

	volatile u32 *pEINTx_Reg, *pFLTx_Reg;
	volatile u32 *pGPIO_EINT_Addr;
	u32 uGpioPort, uFunc, uType ;	
	u32 uConValue;

	GPIO_pBase = (void *)GPIO_BASE;

	pGPIO_EINT_Addr = &(GPIO->rEINT12CON);

	// EINT Port
	if( uEINT_No <= 7)
	{
		uGpioPort = uEINT_No;
		uFunc = 7;  					// EINT Function 
		GPIO_SetFunctionEach(eGPIO_A,(GPIO_eBitPos)uGpioPort, uFunc);		// ??
		GPIO_SetPullUpDownEach(eGPIO_A, (GPIO_eBitPos)uGpioPort, 0x0);		// disable Pull-up/dn
	}
	else if(uEINT_No >= 8 && uEINT_No <= 14)
	{
		uGpioPort = (uEINT_No - 8) ;
		uFunc = 7;
		GPIO_SetFunctionEach(eGPIO_B, (GPIO_eBitPos)uGpioPort, uFunc);   		  // ??
		GPIO_SetPullUpDownEach(eGPIO_B, (GPIO_eBitPos)uGpioPort, 0x0);

	}
	else if (uEINT_No >= 15)
	{
		UART_Printf("Error Eint No. \n");
		Assert(0);
	}

	
	//  Interrupt Type 
		uType =(uEINT_No>>2);
		pEINTx_Reg = pGPIO_EINT_Addr ;		//rEINT12CON 	
		uConValue = *pEINTx_Reg;
		uConValue = (uConValue & ~(0xF<<(uType*4))) | (uIntType<<(uType*4));
		*pEINTx_Reg = uConValue;

	// Fliter & Filter width	
	if( uFltWidth <= 0x3F)
	{
		uType =(uEINT_No>>3);				//  EINT1[7:0], EINT1[14:8]
		pFLTx_Reg = pGPIO_EINT_Addr + 8 ; 		// rEINT12FLTCON
		uConValue = *pFLTx_Reg;
		uConValue = (uConValue & ~(0xFF<<(uType*8))) |((uFltWidth<<(uType*8))|(eFltType<<(uType*8+7)));
		*pFLTx_Reg = uConValue;	
	}
	else
	{
		UART_Printf("Error Filter Width. \n");
		Assert(0);
	}
}

//////////
// Function Name : GPIO_EINT1ClrPend
// Function Desctiption : Clear Eint pending bit of the Eint Group 1(EINT1[14:0] )
// Input : 			uEINT_No: EINT No.
// Output : NONE
//
// Version : v0.1
void GPIO_EINT1ClrPend(u32 uEINT_No )
{

	volatile u32 *pEINT0PEND;
	u32 uConValue;

	pEINT0PEND = &(GPIO->rEINT12PEND);

	uConValue = (1<<uEINT_No);
	*pEINT0PEND = uConValue;

}

//////////
// Function Name : GPIO_EINT1DisMask
// Function Desctiption : Disable Mask bit of the Eint Group 1(EINT1[14:0] )
// Input : 			uEINT_No: EINT No.
// Output : NONE
//
// Version : v0.1
void GPIO_EINT1DisMask(u32 uEINT_No )
{

	volatile u32 *pMASK_Reg;
	u32 uConValue;

	pMASK_Reg = &(GPIO->rEINT12MASK);
	
	uConValue = *pMASK_Reg;
       uConValue = (uConValue & ~(0x1<<(uEINT_No)));
	*pMASK_Reg = uConValue;

}

//////////
// Function Name : GPIO_EINT1EnMask
// Function Desctiption : Enable Mask bit of the  Eint Group 1(EINT1[14:0] )
// Input : 			uEINT_No: EINT No.
// Output : NONE
//
// Version : v0.1
void GPIO_EINT1EnMask(u32 uEINT_No )
{

	volatile u32 *pMASK_Reg;
	u32 uConValue;

	pMASK_Reg = &(GPIO->rEINT12MASK);
	
	uConValue = *pMASK_Reg;
       uConValue = (uConValue & ~(0x1<<(uEINT_No)))|(1<<uEINT_No);
	*pMASK_Reg = uConValue;

}

//////////
// Function Name : GPIO_SetEint2
// Function Desctiption : This function setup Eint Group 2(EINT2[7:0] -> GPC[7:0])
// Input : 			uEINT_No: EINT No.
//					uINTType: Select EINT Type. 
//								Low, High, Falling, Rising, Both
//					uFltType :  Select Filter Type
//								DisFLT(Disable Filter), EnFLT(Digital Filter)
//					uFltWidth : Filter Width
// Output : NONE
//
// Version : v0.1
void GPIO_SetEint2(u32 uEINT_No , u32 uIntType, FLT_eTYPE eFltType,  u32 uFltWidth)
{

	volatile u32 *pEINTx_Reg, *pFLTx_Reg;
	volatile u32 *pGPIO_EINT_Addr;
	u32 uGpioPort, uFunc, uType ;	
	u32 uConValue;

	GPIO_pBase = (void *)GPIO_BASE;

	pGPIO_EINT_Addr = &(GPIO->rEINT12CON);

	// EINT Port
	if( uEINT_No <= 7)
	{
		uGpioPort = uEINT_No;
		uFunc = 7;  					// EINT Function 
		GPIO_SetFunctionEach(eGPIO_C, (GPIO_eBitPos)uGpioPort, uFunc);		// ??
		GPIO_SetPullUpDownEach(eGPIO_C, (GPIO_eBitPos)uGpioPort, 0x0);		// disable Pull-up/dn
	}
	else if (uEINT_No >= 8)
	{
		UART_Printf("Error Eint No. \n");
		Assert(0);
	}

	
	//  Interrupt Type 
		uType =(uEINT_No>>2);
		pEINTx_Reg = pGPIO_EINT_Addr ;		//rEINT12CON 	
		uConValue = *pEINTx_Reg;
		uConValue = (uConValue & ~(0xF<<(uType*4+16))) | (uIntType<<(uType*4+16));
		*pEINTx_Reg = uConValue;

	// Fliter & Filter width	
	if( uFltWidth <= 0x3F)
	{
		pFLTx_Reg = pGPIO_EINT_Addr + 8 ; 		// rEINT12FLTCON
		uConValue = *pFLTx_Reg;
		uConValue = (uConValue & ~(0xFF<<16)) |((uFltWidth<<16)|(eFltType<<23));
		*pFLTx_Reg = uConValue;	
	}
	else
	{
		UART_Printf("Error Filter Width. \n");
		Assert(0);
	}
}

//////////
// Function Name : GPIO_EINT2ClrPend
// Function Desctiption : Clear Eint pending bit of the Eint Group 2(EINT2[7:0] )
// Input : 			uEINT_No: EINT No.
// Output : NONE
//
// Version : v0.1
void GPIO_EINT2ClrPend(u32 uEINT_No )
{

	volatile u32 *pEINT0PEND;
	u32 uConValue;

	pEINT0PEND = &(GPIO->rEINT12PEND);

	uConValue = (1<<(uEINT_No+16));
	*pEINT0PEND = uConValue;

}

//////////
// Function Name : GPIO_EINT1DisMask
// Function Desctiption : Disable Mask bit of the Eint Group 2(EINT2[7:0] )
// Input : 			uEINT_No: EINT No.
// Output : NONE
//
// Version : v0.1
void GPIO_EINT2DisMask(u32 uEINT_No )
{

	volatile u32 *pMASK_Reg;
	u32 uConValue;

	pMASK_Reg = &(GPIO->rEINT12MASK);
	
	uConValue = *pMASK_Reg;
       uConValue = (uConValue & ~(0x1<<(uEINT_No+16)));
	*pMASK_Reg = uConValue;

}

//////////
// Function Name : GPIO_EINT1EnMask
// Function Desctiption : Enable Mask bit of the Eint Group 2(EINT2[7:0] )
// Input : 			uEINT_No: EINT No.
// Output : NONE
//
// Version : v0.1
void GPIO_EINT2EnMask(u32 uEINT_No )
{

	volatile u32 *pMASK_Reg;
	u32 uConValue;

	pMASK_Reg = &(GPIO->rEINT12MASK);
	
	uConValue = *pMASK_Reg;
       uConValue = (uConValue & ~(0x1<<(uEINT_No+16)))|(1<<uEINT_No+16);
	*pMASK_Reg = uConValue;

}

//////////
// Function Name : GPIO_SetEint3
// Function Desctiption : This function setup Eint Group 3(EINT3[9:0] -> GPD[4:0], GPE[4:0])
// Input : 			uEINT_No: EINT No.
//					uINTType: Select EINT Type. 
//								Low, High, Falling, Rising, Both
//					uFltType :  Select Filter Type
//								DisFLT(Disable Filter), EnFLT(Digital Filter)
//					uFltWidth : Filter Width
// Output : NONE
//
// Version : v0.1
void GPIO_SetEint3(u32 uEINT_No , u32 uIntType, FLT_eTYPE eFltType,  u32 uFltWidth)
{

	volatile u32 *pEINTx_Reg, *pFLTx_Reg;
	volatile u32 *pGPIO_EINT_Addr;
	u32 uGpioPort, uFunc, uType ;	
	u32 uConValue;

	GPIO_pBase = (void *)GPIO_BASE;

	pGPIO_EINT_Addr = &(GPIO->rEINT34CON);

	// EINT Port
	if( uEINT_No <= 4)
	{
		uGpioPort = uEINT_No;
		uFunc = 7;  					// EINT Function 
		GPIO_SetFunctionEach(eGPIO_D, (GPIO_eBitPos)uGpioPort, uFunc);		// ??
		GPIO_SetPullUpDownEach(eGPIO_D,(GPIO_eBitPos)uGpioPort, 0x0);		// disable Pull-up/dn
	}
	else if(uEINT_No >= 5 && uEINT_No <= 9)
	{
		uGpioPort = (uEINT_No - 5) ;
		uFunc = 7;
		GPIO_SetFunctionEach(eGPIO_E,(GPIO_eBitPos)uGpioPort, uFunc);   		  // ??
		GPIO_SetPullUpDownEach(eGPIO_E,(GPIO_eBitPos)uGpioPort, 0x0);

	}
	else if (uEINT_No >= 10)
	{
		UART_Printf("Error Eint No. \n");
		Assert(0);
	}

	
	//  Interrupt Type 
		uType =(uEINT_No>>2);
		pEINTx_Reg = pGPIO_EINT_Addr ;		  //rEINT34CON 	
		uConValue = *pEINTx_Reg;
		uConValue = (uConValue & ~(0xF<<(uType*4))) | (uIntType<<(uType*4));
		*pEINTx_Reg = uConValue;

	// Fliter & Filter width	
	if( uFltWidth <= 0x3F)
	{
		uType =(uEINT_No>>3);				//  EINT3[7:0], EINT3[9:8]
		pFLTx_Reg = pGPIO_EINT_Addr + 8 ; 		// rEINT34FLTCON
		uConValue = *pFLTx_Reg;
		uConValue = (uConValue & ~(0xFF<<(uType*8))) |((uFltWidth<<(uType*8))|(eFltType<<(uType*8+7)));
		*pFLTx_Reg = uConValue;	
	}
	else
	{
		UART_Printf("Error Filter Width. \n");
		Assert(0);
	}
}

//////////
// Function Name : GPIO_EINT3ClrPend
// Function Desctiption : Clear Eint pending bit of the Eint Group 3(EINT3[9:0] )
// Input : 			uEINT_No: EINT No.
// Output : NONE
//
// Version : v0.1
void GPIO_EINT3ClrPend(u32 uEINT_No )
{

	volatile u32 *pEINT0PEND;
	u32 uConValue;

	pEINT0PEND = &(GPIO->rEINT34PEND);

	uConValue = (1<<uEINT_No);
	*pEINT0PEND = uConValue;

}

//////////
// Function Name : GPIO_EINT3DisMask
// Function Desctiption : Disable Mask bit of the Eint Group 3(EINT3[9:0] )
// Input : 			uEINT_No: EINT No.
// Output : NONE
//
// Version : v0.1
void GPIO_EINT3DisMask(u32 uEINT_No )
{

	volatile u32 *pMASK_Reg;
	u32 uConValue;

	pMASK_Reg = &(GPIO->rEINT34MASK);
	
	uConValue = *pMASK_Reg;
       uConValue = (uConValue & ~(0x1<<(uEINT_No)));
	*pMASK_Reg = uConValue;

}

//////////
// Function Name : GPIO_EINT3EnMask
// Function Desctiption : Enable Mask bit of the  Eint Group 3(EINT3[9:0] )
// Input : 			uEINT_No: EINT No.
// Output : NONE
//
// Version : v0.1
void GPIO_EINT3EnMask(u32 uEINT_No )
{

	volatile u32 *pMASK_Reg;
	u32 uConValue;

	pMASK_Reg = &(GPIO->rEINT34MASK);
	
	uConValue = *pMASK_Reg;
       uConValue = (uConValue & ~(0x1<<(uEINT_No)))|(1<<uEINT_No);
	*pMASK_Reg = uConValue;

}

//////////
// Function Name : GPIO_SetEint4
// Function Desctiption : This function setup Eint Group 4(EINT4[13:0] -> GPF[13:0])
// Input : 			uEINT_No: EINT No.
//					uINTType: Select EINT Type. 
//								Low, High, Falling, Rising, Both
//					uFltType :  Select Filter Type
//								DisFLT(Disable Filter), EnFLT(Digital Filter)
//					uFltWidth : Filter Width
// Output : NONE
//
// Version : v0.1
void GPIO_SetEint4(u32 uEINT_No , u32 uIntType, FLT_eTYPE eFltType,  u32 uFltWidth)
{

	volatile u32 *pEINTx_Reg, *pFLTx_Reg;
	volatile u32 *pGPIO_EINT_Addr;
	u32 uGpioPort, uFunc, uType ;	
	u32 uConValue;

	GPIO_pBase = (void *)GPIO_BASE;

	pGPIO_EINT_Addr = &(GPIO->rEINT34CON);

	// EINT Port
	if( uEINT_No <= 13)
	{
		uGpioPort = uEINT_No;
		uFunc = 3;  					// EINT Function 
		GPIO_SetFunctionEach(eGPIO_F, (GPIO_eBitPos)uGpioPort, uFunc);		// ??
		GPIO_SetPullUpDownEach(eGPIO_F, (GPIO_eBitPos)uGpioPort, 0x0);		// disable Pull-up/dn
	}
	else if (uEINT_No >= 14)
	{
		UART_Printf("Error Eint No. \n");
		Assert(0);
	}

	
	//  Interrupt Type 
		uType =(uEINT_No>>2);
		pEINTx_Reg = pGPIO_EINT_Addr ;		  //rEINT34CON 	
		uConValue = *pEINTx_Reg;
		uConValue = (uConValue & ~(0xF<<(uType*4+16))) | (uIntType<<(uType*4+16));
		*pEINTx_Reg = uConValue;

	// Fliter & Filter width	
	if(   uFltWidth <= 0x3F)
	{
		uType =(uEINT_No>>3);				//  EINT4[7:0], EINT4[13:8]
		pFLTx_Reg = pGPIO_EINT_Addr + 8 ; 		// rEINT34FLTCON
		uConValue = *pFLTx_Reg;
		uConValue = (uConValue & ~(0xFF<<(uType*8+16))) |((uFltWidth<<(uType*8+16))|(eFltType<<(uType*8+23)));
		*pFLTx_Reg = uConValue;	
	}
	else
	{
		UART_Printf("Error Filter Width. \n");
		Assert(0);
	}
}

//////////
// Function Name : GPIO_EINT4ClrPend
// Function Desctiption : Clear Eint pending bit of the Eint Group 4(EINT4[13:0] )
// Input : 			uEINT_No: EINT No.
// Output : NONE
//
// Version : v0.1
void GPIO_EINT4ClrPend(u32 uEINT_No )
{

	volatile u32 *pEINT0PEND;
	u32 uConValue;

	pEINT0PEND = &(GPIO->rEINT34PEND);

	uConValue = (1<<(uEINT_No+16));
	*pEINT0PEND = uConValue;

}

//////////
// Function Name : GPIO_EINT4DisMask
// Function Desctiption : Disable Mask bit of the Eint Group 4(EINT4[13:0] )
// Input : 			uEINT_No: EINT No.
// Output : NONE
//
// Version : v0.1
void GPIO_EINT4DisMask(u32 uEINT_No )
{

	volatile u32 *pMASK_Reg;
	u32 uConValue;

	pMASK_Reg = &(GPIO->rEINT34MASK);
	
	uConValue = *pMASK_Reg;
       uConValue = (uConValue & ~(0x1<<(uEINT_No+16)));
	*pMASK_Reg = uConValue;

}

//////////
// Function Name : GPIO_EINT4EnMask
// Function Desctiption : Enable Mask bit of the Eint Group 4(EINT4[13:0] )
// Input : 			uEINT_No: EINT No.
// Output : NONE
//
// Version : v0.1
void GPIO_EINT4EnMask(u32 uEINT_No )
{

	volatile u32 *pMASK_Reg;
	u32 uConValue;

	pMASK_Reg = &(GPIO->rEINT34MASK);
	
	uConValue = *pMASK_Reg;
       uConValue = (uConValue & ~(0x1<<(uEINT_No+16)))|(1<<(uEINT_No+16));
	*pMASK_Reg = uConValue;

}

//////////
// Function Name : GPIO_SetEint5
// Function Desctiption : This function setup Eint Group 5(EINT5[6:0] -> GPG[6:0])
// Input : 			uEINT_No: EINT No.
//					uINTType: Select EINT Type. 
//								Low, High, Falling, Rising, Both
//					uFltType :  Select Filter Type
//								DisFLT(Disable Filter), EnFLT(Digital Filter)
//					uFltWidth : Filter Width
// Output : NONE
//
// Version : v0.1
void GPIO_SetEint5(u32 uEINT_No , u32 uIntType, FLT_eTYPE eFltType,  u32 uFltWidth)
{

	volatile u32 *pEINTx_Reg, *pFLTx_Reg;
	volatile u32 *pGPIO_EINT_Addr;
	u32 uGpioPort, uFunc, uType ;	
	u32 uConValue;

	GPIO_pBase = (void *)GPIO_BASE;

	pGPIO_EINT_Addr = &(GPIO->rEINT56CON);

	// EINT Port
	if( uEINT_No <= 6)
	{
		uGpioPort = uEINT_No;
		uFunc = 7;  					// EINT Function 
		GPIO_SetFunctionEach(eGPIO_G, (GPIO_eBitPos)uGpioPort, uFunc);		// ??
		GPIO_SetPullUpDownEach(eGPIO_G, (GPIO_eBitPos)uGpioPort, 0x0);		// disable Pull-up/dn
	}
	else if (uEINT_No >= 7)
	{
		UART_Printf("Error Eint No. \n");
		Assert(0);
	}

	
	//  Interrupt Type 
		uType =(uEINT_No>>2);
		pEINTx_Reg = pGPIO_EINT_Addr ;		  //rEINT56CON 	
		uConValue = *pEINTx_Reg;
		uConValue = (uConValue & ~(0xF<<(uType*4))) | (uIntType<<(uType*4));
		*pEINTx_Reg = uConValue;

	// Fliter & Filter width	
	if(uFltWidth <= 0x3F)
	{
		uType =(uEINT_No>>3);				
		pFLTx_Reg = pGPIO_EINT_Addr + 8 ; 		// rEINT56FLTCON
		uConValue = *pFLTx_Reg;
		uConValue = (uConValue & ~(0xFF<<(uType*8))) |((uFltWidth<<(uType*8))|(eFltType<<(uType*8+7)));
		*pFLTx_Reg = uConValue;	
	}
	else
	{
		UART_Printf("Error Filter Width. \n");
		Assert(0);
	}
}

//////////
// Function Name : GPIO_EINT5ClrPend
// Function Desctiption : Clear Eint pending bit of the Eint Group 5(EINT5[6:0] )
// Input : 			uEINT_No: EINT No.
// Output : NONE
//
// Version : v0.1
void GPIO_EINT5ClrPend(u32 uEINT_No )
{

	volatile u32 *pEINT0PEND;
	u32 uConValue;

	pEINT0PEND = &(GPIO->rEINT56PEND);

	uConValue = (1<<uEINT_No);
	*pEINT0PEND = uConValue;

}

//////////
// Function Name : GPIO_EINT5DisMask
// Function Desctiption : Disable Mask bit of the Eint Group 5(EINT5[6:0] )
// Input : 			uEINT_No: EINT No.
// Output : NONE
//
// Version : v0.1
void GPIO_EINT5DisMask(u32 uEINT_No )
{

	volatile u32 *pMASK_Reg;
	u32 uConValue;

	pMASK_Reg = &(GPIO->rEINT56MASK);
	
	uConValue = *pMASK_Reg;
       uConValue = (uConValue & ~(0x1<<(uEINT_No)));
	*pMASK_Reg = uConValue;

}

//////////
// Function Name : GPIO_EINT5EnMask
// Function Desctiption : Enable Mask bit of the  Eint Group 5(EINT5[6:0] )
// Input : 			uEINT_No: EINT No.
// Output : NONE
//
// Version : v0.1
void GPIO_EINT5EnMask(u32 uEINT_No )
{

	volatile u32 *pMASK_Reg;
	u32 uConValue;

	pMASK_Reg = &(GPIO->rEINT56MASK);
	
	uConValue = *pMASK_Reg;
       uConValue = (uConValue & ~(0x1<<(uEINT_No)))|(1<<uEINT_No);
	*pMASK_Reg = uConValue;

}

//////////
// Function Name : GPIO_SetEint6
// Function Desctiption : This function setup Eint Group 6(EINT6[9:0] -> GPH[9:0])
// Input : 			uEINT_No: EINT No.
//					uINTType: Select EINT Type. 
//								Low, High, Falling, Rising, Both
//					uFltType :  Select Filter Type
//								DisFLT(Disable Filter), EnFLT(Digital Filter)
//					uFltWidth : Filter Width
// Output : NONE
//
// Version : v0.1
void GPIO_SetEint6(u32 uEINT_No , u32 uIntType, FLT_eTYPE eFltType,  u32 uFltWidth)
{

	volatile u32 *pEINTx_Reg, *pFLTx_Reg;
	volatile u32 *pGPIO_EINT_Addr;
	u32 uGpioPort, uFunc, uType ;	
	u32 uConValue;

	GPIO_pBase = (void *)GPIO_BASE;

	pGPIO_EINT_Addr = &(GPIO->rEINT56CON);

	// EINT Port
	if( uEINT_No <= 9)
	{
		uGpioPort = uEINT_No;
		uFunc =	7;  					// EINT Function 
		GPIO_SetFunctionEach(eGPIO_H, (GPIO_eBitPos)uGpioPort, uFunc);		// ??
		GPIO_SetPullUpDownEach(eGPIO_H, (GPIO_eBitPos)uGpioPort, 0x0);		// disable Pull-up/dn
	}
	else if (uEINT_No >= 10)
	{
		UART_Printf("Error Eint No. \n");
		Assert(0);
	}

	
	//  Interrupt Type 
		uType =(uEINT_No>>2);
		pEINTx_Reg = pGPIO_EINT_Addr ;		  //rEINT56CON 	
		uConValue = *pEINTx_Reg;
		uConValue = (uConValue & ~(0xF<<(uType*4+16))) | (uIntType<<(uType*4+16));
		*pEINTx_Reg = uConValue;

	// Fliter & Filter width	
	if( uFltWidth <= 0x3F)
	{
		uType =(uEINT_No>>3);				
		pFLTx_Reg = pGPIO_EINT_Addr + 8 ; 		// rEINT56FLTCON
		uConValue = *pFLTx_Reg;
		uConValue = (uConValue & ~(0xFF<<(uType*8+16))) |((uFltWidth<<(uType*8+16))|(eFltType<<(uType*8+23)));
		*pFLTx_Reg = uConValue;	
	}
	else
	{
		UART_Printf("Error Filter Width. \n");
		Assert(0);
	}
}

//////////
// Function Name : GPIO_EINT6ClrPend
// Function Desctiption : Clear Eint pending bit of the Eint Group 6(EINT6[9:0] )
// Input : 			uEINT_No: EINT No.
// Output : NONE
//
// Version : v0.1
void GPIO_EINT6ClrPend(u32 uEINT_No )
{

	volatile u32 *pEINT0PEND;
	u32 uConValue;

	pEINT0PEND = &(GPIO->rEINT56PEND);

	uConValue = (1<<(uEINT_No+16));
	*pEINT0PEND = uConValue;

}

//////////
// Function Name : GPIO_EINT6DisMask
// Function Desctiption : Disable Mask bit of the Eint Group 6(EINT6[9:0] )
// Input : 			uEINT_No: EINT No.
// Output : NONE
//
// Version : v0.1
void GPIO_EINT6DisMask(u32 uEINT_No )
{

	volatile u32 *pMASK_Reg;
	u32 uConValue;

	pMASK_Reg = &(GPIO->rEINT56MASK);
	
	uConValue = *pMASK_Reg;
       uConValue = (uConValue & ~(0x1<<(uEINT_No+16)));
	*pMASK_Reg = uConValue;

}

//////////
// Function Name : GPIO_EINT6EnMask
// Function Desctiption : Enable Mask bit of the Eint Group 6(EINT6[9:0] )
// Input : 			uEINT_No: EINT No.
// Output : NONE
//
// Version : v0.1
void GPIO_EINT6EnMask(u32 uEINT_No )
{

	volatile u32 *pMASK_Reg;
	u32 uConValue;

	pMASK_Reg = &(GPIO->rEINT56MASK);
	
	uConValue = *pMASK_Reg;
       uConValue = (uConValue & ~(0x1<<(uEINT_No+16)))|(1<<(uEINT_No+16));
	*pMASK_Reg = uConValue;

}

//////////
// Function Name : GPIO_SetEint7
// Function Desctiption : This function setup Eint Group 7(EINT7[15:0] -> GPO[15:0])
// Input : 			uEINT_No: EINT No.
//					uINTType: Select EINT Type. 
//								Low, High, Falling, Rising, Both
//					uFltType :  Select Filter Type
//								DisFLT(Disable Filter), EnFLT(Digital Filter)
//					uFltWidth : Filter Width
// Output : NONE
//
// Version : v0.1
void GPIO_SetEint7(u32 uEINT_No , u32 uIntType, FLT_eTYPE eFltType,  u32 uFltWidth)
{

	volatile u32 *pEINTx_Reg, *pFLTx_Reg;
	volatile u32 *pGPIO_EINT_Addr;
	u32 uGpioPort, uFunc, uType ;	
	u32 uConValue;

	GPIO_pBase = (void *)GPIO_BASE;

	pGPIO_EINT_Addr = &(GPIO->rEINT78CON);

	// EINT Port
	if(uEINT_No <= 15)
	{
		uGpioPort = uEINT_No;
		uFunc = 3;  					// EINT Function 
		GPIO_SetFunctionEach(eGPIO_O, (GPIO_eBitPos)uGpioPort, uFunc);		// ??
		GPIO_SetPullUpDownEach(eGPIO_O, (GPIO_eBitPos)uGpioPort, 0x0);		// disable Pull-up/dn
	}
	else if (uEINT_No >= 16)
	{
		UART_Printf("Error Eint No. \n");
		Assert(0);
	}

	
	//  Interrupt Type 
		uType =(uEINT_No>>2);
		pEINTx_Reg = pGPIO_EINT_Addr ;		  //rEINT78CON 	
		uConValue = *pEINTx_Reg;
		uConValue = (uConValue & ~(0xF<<(uType*4))) | (uIntType<<(uType*4));
		*pEINTx_Reg = uConValue;

	// Fliter & Filter width	
	if( uFltWidth <= 0x3F)
	{
		uType =(uEINT_No>>3);				
		pFLTx_Reg = pGPIO_EINT_Addr + 8 ; 		// rEINT78FLTCON
		uConValue = *pFLTx_Reg;
		uConValue = (uConValue & ~(0xFF<<(uType*8))) |((uFltWidth<<(uType*8))|(eFltType<<(uType*8+7)));
		*pFLTx_Reg = uConValue;	
	}
	else
	{
		UART_Printf("Error Filter Width. \n");
		Assert(0);
	}
}

//////////
// Function Name : GPIO_EINT7ClrPend
// Function Desctiption : Clear Eint pending bit of the Eint Group 7(EINT7[15:0] )
// Input : 			uEINT_No: EINT No.
// Output : NONE
//
// Version : v0.1
void GPIO_EINT7ClrPend(u32 uEINT_No )
{

	volatile u32 *pEINT0PEND;
	u32 uConValue;

	pEINT0PEND = &(GPIO->rEINT78PEND);

	uConValue = (1<<uEINT_No);
	*pEINT0PEND = uConValue;

}

//////////
// Function Name : GPIO_EINT7DisMask
// Function Desctiption : Disable Mask bit of the Eint Group 7(EINT7[15:0] )
// Input : 			uEINT_No: EINT No.
// Output : NONE
//
// Version : v0.1
void GPIO_EINT7DisMask(u32 uEINT_No )
{

	volatile u32 *pMASK_Reg;
	u32 uConValue;

	pMASK_Reg = &(GPIO->rEINT78MASK);
	
	uConValue = *pMASK_Reg;
       uConValue = (uConValue & ~(0x1<<(uEINT_No)));
	*pMASK_Reg = uConValue;

}

//////////
// Function Name : GPIO_EINT7EnMask
// Function Desctiption : Enable Mask bit of the Eint Group 7(EINT7[15:0] )
// Input : 			uEINT_No: EINT No.
// Output : NONE
//
// Version : v0.1
void GPIO_EINT7EnMask(u32 uEINT_No )
{

	volatile u32 *pMASK_Reg;
	u32 uConValue;

	pMASK_Reg = &(GPIO->rEINT78MASK);
	
	uConValue = *pMASK_Reg;
       uConValue = (uConValue & ~(0x1<<(uEINT_No)))|(1<<uEINT_No);
	*pMASK_Reg = uConValue;

}

//////////
// Function Name : GPIO_SetEint8
// Function Desctiption : This function setup Eint Group 8(EINT8[14:0] -> GPP[14:0])
// Input : 			uEINT_No: EINT No.
//					uINTType: Select EINT Type. 
//								Low, High, Falling, Rising, Both
//					uFltType :  Select Filter Type
//								DisFLT(Disable Filter), EnFLT(Digital Filter)
//					uFltWidth : Filter Width
// Output : NONE
//
// Version : v0.1
void GPIO_SetEint8(u32 uEINT_No , u32 uIntType, FLT_eTYPE eFltType,  u32 uFltWidth)
{

	volatile u32 *pEINTx_Reg, *pFLTx_Reg;
	volatile u32 *pGPIO_EINT_Addr;
	u32 uGpioPort, uFunc, uType ;	
	u32 uConValue;

	GPIO_pBase = (void *)GPIO_BASE;

	pGPIO_EINT_Addr = &(GPIO->rEINT78CON);

	// EINT Port
	if( uEINT_No <= 14)
	{
		uGpioPort = uEINT_No;
		uFunc =	3;  					// EINT Function 
		GPIO_SetFunctionEach(eGPIO_P, (GPIO_eBitPos)uGpioPort, uFunc);		// ??
		GPIO_SetPullUpDownEach(eGPIO_P, (GPIO_eBitPos)uGpioPort, 0x0);		// disable Pull-up/dn
	}
	else if (uEINT_No >= 15)
	{
		UART_Printf("Error Eint No. \n");
		Assert(0);
	}

	
	//  Interrupt Type 
		uType =(uEINT_No>>2);
		pEINTx_Reg = pGPIO_EINT_Addr ;		  //rEINT78CON 	
		uConValue = *pEINTx_Reg;
		uConValue = (uConValue & ~(0xF<<(uType*4+16))) | (uIntType<<(uType*4+16));
		*pEINTx_Reg = uConValue;

	// Fliter & Filter width	
	if( uFltWidth <= 0x3F)
	{
		uType =(uEINT_No>>3);				
		pFLTx_Reg = pGPIO_EINT_Addr + 8 ; 		// rEINT78FLTCON
		uConValue = *pFLTx_Reg;
		uConValue = (uConValue & ~(0xFF<<(uType*8+16))) |((uFltWidth<<(uType*8+16))|(eFltType<<(uType*8+23)));
		*pFLTx_Reg = uConValue;	
	}
	else
	{
		UART_Printf("Error Filter Width. \n");
		Assert(0);
	}
}

//////////
// Function Name : GPIO_EINT8ClrPend
// Function Desctiption : Clear Eint pending bit of the Eint Group 8(EINT8[14:0] )
// Input : 			uEINT_No: EINT No.
// Output : NONE
//
// Version : v0.1
void GPIO_EINT8ClrPend(u32 uEINT_No )
{

	volatile u32 *pEINT0PEND;
	u32 uConValue;

	pEINT0PEND = &(GPIO->rEINT78PEND);

	uConValue = (1<<(uEINT_No+16));
	*pEINT0PEND = uConValue;

}

//////////
// Function Name : GPIO_EINT8DisMask
// Function Desctiption : Disable Mask bit of the Eint Group 8(EINT8[14:0] )
// Input : 			uEINT_No: EINT No.
// Output : NONE
//
// Version : v0.1
void GPIO_EINT8DisMask(u32 uEINT_No )
{

	volatile u32 *pMASK_Reg;
	u32 uConValue;

	pMASK_Reg = &(GPIO->rEINT78MASK);
	
	uConValue = *pMASK_Reg;
       uConValue = (uConValue & ~(0x1<<(uEINT_No+16)));
	*pMASK_Reg = uConValue;

}

//////////
// Function Name : GPIO_EINT8EnMask
// Function Desctiption : Enable Mask bit of the Eint Group 8(EINT8[14:0] )
// Input : 			uEINT_No: EINT No.
// Output : NONE
//
// Version : v0.1
void GPIO_EINT8EnMask(u32 uEINT_No )
{

	volatile u32 *pMASK_Reg;
	u32 uConValue;

	pMASK_Reg = &(GPIO->rEINT78MASK);
	
	uConValue = *pMASK_Reg;
       uConValue = (uConValue & ~(0x1<<(uEINT_No+16)))|(1<<(uEINT_No+16));
	*pMASK_Reg = uConValue;

}

//////////
// Function Name : GPIO_SetEint9
// Function Desctiption : This function setup Eint Group 9(EINT9[8:0] -> GPQ[8:0])
// Input : 			uEINT_No: EINT No.
//					uINTType: Select EINT Type. 
//								Low, High, Falling, Rising, Both
//					uFltType :  Select Filter Type
//								DisFLT(Disable Filter), EnFLT(Digital Filter)
//					uFltWidth : Filter Width
// Output : NONE
//
// Version : v0.1
void GPIO_SetEint9(u32 uEINT_No , u32 uIntType, FLT_eTYPE eFltType,  u32 uFltWidth)
{

	volatile u32 *pEINTx_Reg, *pFLTx_Reg;
	volatile u32 *pGPIO_EINT_Addr;
	u32 uGpioPort, uFunc, uType ;	
	u32 uConValue;

	GPIO_pBase = (void *)GPIO_BASE;

	pGPIO_EINT_Addr = &(GPIO->rEINT9CON);

	// EINT Port
	if( uEINT_No <= 8)
	{
		uGpioPort = uEINT_No;
		uFunc = 3;  					// EINT Function 
		GPIO_SetFunctionEach(eGPIO_Q, (GPIO_eBitPos)uGpioPort, uFunc);		// ??
		GPIO_SetPullUpDownEach(eGPIO_Q, (GPIO_eBitPos)uGpioPort, 0x0);		// disable Pull-up/dn
	}
	else if (uEINT_No >= 9)
	{
		UART_Printf("Error Eint No. \n");
		Assert(0);
	}

	
	//  Interrupt Type 
		uType =(uEINT_No>>2);
		pEINTx_Reg = pGPIO_EINT_Addr ;		  //rEINT78CON 	
		uConValue = *pEINTx_Reg;
		uConValue = (uConValue & ~(0xF<<(uType*4))) | (uIntType<<(uType*4));
		*pEINTx_Reg = uConValue;

	// Fliter & Filter width	
	if( uFltWidth <= 0x3F)
	{
		uType =(uEINT_No>>3);				
		pFLTx_Reg = pGPIO_EINT_Addr + 8 ; 		// rEINT78FLTCON
		uConValue = *pFLTx_Reg;
		uConValue = (uConValue & ~(0xFF<<(uType*8))) |((uFltWidth<<(uType*8))|(eFltType<<(uType*8+7)));
		*pFLTx_Reg = uConValue;	
	}
	else
	{
		UART_Printf("Error Filter Width. \n");
		Assert(0);
	}
}

//////////
// Function Name : GPIO_EINT9ClrPend
// Function Desctiption : Clear Eint pending bit of the Eint Group 9(EINT9[8:0] )
// Input : 			uEINT_No: EINT No.
// Output : NONE
//
// Version : v0.1
void GPIO_EINT9ClrPend(u32 uEINT_No )
{

	volatile u32 *pEINT0PEND;
	u32 uConValue;

	pEINT0PEND = &(GPIO->rEINT9PEND);

	uConValue = (1<<uEINT_No);
	*pEINT0PEND = uConValue;

}

//////////
// Function Name : GPIO_EINT9DisMask
// Function Desctiption : Disable Mask bit of the Eint Group 9(EINT9[8:0] )
// Input : 			uEINT_No: EINT No.
// Output : NONE
//
// Version : v0.1
void GPIO_EINT9DisMask(u32 uEINT_No )
{

	volatile u32 *pMASK_Reg;
	u32 uConValue;

	pMASK_Reg = &(GPIO->rEINT9MASK);
	
	uConValue = *pMASK_Reg;
       uConValue = (uConValue & ~(0x1<<(uEINT_No)));
	*pMASK_Reg = uConValue;

}

//////////
// Function Name : GPIO_EINT9EnMask
// Function Desctiption : Enable Mask bit of the Eint Group 9(EINT9[8:0] )
// Input : 			uEINT_No: EINT No.
// Output : NONE
//
// Version : v0.1
void GPIO_EINT9EnMask(u32 uEINT_No )
{

	volatile u32 *pMASK_Reg;
	u32 uConValue;

	pMASK_Reg = &(GPIO->rEINT9MASK);
	
	uConValue = *pMASK_Reg;
       uConValue = (uConValue & ~(0x1<<(uEINT_No)))|(1<<uEINT_No);
	*pMASK_Reg = uConValue;

}


//////////
// Function Name : Isr_Eintx
// Function Description : EINT ISR Routine
// Input : 	None
// Output :	None 
// Version : v0.1
void __irq Isr_Eint9(void)
{
          
	GPIO_EINT0ClrPend(9);
	g_IntCnt++;
	UART_Printf("EINT9 interrupt is occurred %d.\n", g_IntCnt);

	INTC_ClearVectAddr();
    
}

void __irq Isr_Eint10(void)
{
          
	GPIO_EINT0ClrPend(10);
	g_IntCnt++;
	UART_Printf("EINT10 interrupt is occurred %d.\n", g_IntCnt);

	INTC_ClearVectAddr();
    
}

void __irq Isr_Eint11(void)
{
          
	GPIO_EINT0ClrPend(11);
	g_IntCnt++;
	UART_Printf("EINT11 interrupt is occurred %d.\n", g_IntCnt);

	INTC_ClearVectAddr();
    
}

//////////
// Function Name : GPIO_SetMem0DrvStrength
// Function Desctiption : Set the drive strength of Mem Port0
// Input : 	uDrvStr : Written register Value
// Output : 	NONE
//
// Version : v0.1
// added by rb1004
void GPIO_SetMem0DrvStrength(u32 uDrvStr)
{
	Outp32(&GPIO->rMEM0DRVCON, uDrvStr);
}
