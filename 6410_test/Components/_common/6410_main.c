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
*	File Name : 6410_main.c
*  
*	File Description : This file implements the test main functon.
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
#include "sysc.h"
#include "gpio.h"
#include "intc.h"
#include "timer.h"
#include "uart.h"
#include "dmc.h"

extern void SYSC_Test(void);
extern void DMC_Test(void);
extern void SMC_Test(void);
extern void ONENAND_Test(void);
extern void NAND_Test(void);
extern void ATA_Test(void);
extern void PCCARD_Test(void);
extern void Test_GPIO(void);
extern void Test_DMA(void);
extern void SBLOCK_Test(void);
extern void LCD_Test(void);
extern void POST_Test(void);
extern void TVENC_Test(void);
extern void G2D_Test(void);
extern void ROTATOR_Test(void);
extern void CAMERA_Test(void);
extern void MFC_Test(void);
extern void JPEG_Test(void);
extern void MODEM_Test(void);
extern void HOSTIF_Test(void);
extern void OtgDev_Test(void);
extern void SDMMC_Test(void);
extern void HSI_Test(void);
extern void SPI_Test(void);
extern void IIC_Test(void);
extern void UART_Test(void);
extern void Timer_Test(void);
extern void RTC_Test(void);
extern void WDT_Test(void);
extern void AC97_Test(void);
extern void I2S_Test(void);
extern void PCM_Test(void);
extern void IRDA_Test(void);
extern void ADCTS_Test(void);
extern void KEYPAD_Test(void);
extern void FIMG3D_Test(void);
extern void GIB_Test(void);

const testFuncMenu menu[] =
{
#if 1
	TestSFR,				"SFR R/W Test",
	SYSC_Test,				"SYSC_Test   ",
	DMC_Test,				"DMC_Test    ",
	SMC_Test,				"SROMC_Test  ",	
	ONENAND_Test,			"ONENAND_Test",
	NAND_Test,				"NAND_Test   ",
	ATA_Test,				"ATA_Test    ",
	PCCARD_Test,			"PCCARD_Test ",
	Test_GPIO,				"GPIO_Test   ",
	Test_DMA,				"DMA_Test    ",
	SBLOCK_Test,			"SBLOCK_Test ",
	LCD_Test,				"LCD_Test    ",	
	POST_Test,				"POST_Test   ",	
	TVENC_Test,				"TVENC_Test  ",
	G2D_Test,				"G2D_Test    ",
	ROTATOR_Test,			"ROTATOR_Test",
	CAMERA_Test,			"CAMERA_Test ",
	MFC_Test,				"MFC_Test    ",
	JPEG_Test,				"JPEG_Test   ",
	MODEM_Test,				"MODEM_Test  ",
	HOSTIF_Test,			"HOSTIF_Test ",
	OtgDev_Test,			"OtgDev_Test ",
	SDMMC_Test,				"SDMMC_Test   ",
	HSI_Test,				"HSI_Test    ",
	SPI_Test,				"SPI_Test   ",
	IIC_Test,				"IIC_Test    ",
	UART_Test,				"UART_Test   ",
	Timer_Test,				"Timer_Test  ",
	RTC_Test,				"RTC_Test    ",
	AC97_Test,				"AC97_Test   ",
	I2S_Test,				"I2S_Test    ",	
	PCM_Test,				"PCM_Test    ",
	IRDA_Test,				"IRDA_Test   ",
	ADCTS_Test,				"ADCTS_Test  ",	
	KEYPAD_Test,			"KEYPAD_Test ",
	FIMG3D_Test,			"FIMG3D_Test		",
	//MDP i/f
#else
	NAND_Test,				"NAND_Test   ",
	ONENAND_Test,			"ONENAND_Test",
#endif	
	//	TestFunction,		"Desciption",	
	0, 0
};

//////////
// Function Name : main
// Function Desctiption : main function of the validation
// Input : NONE
// Output : NONE
// Version :
int main(void)
{	
	u32 i, uSel;
	u8 bClockChange=false;
	
	SYSTEM_InitException();
	SYSTEM_InitMmu();
	
	SYSC_ReadSystemID();
	SYSC_GetClkInform();
	
	CalibrateDelay();

	GPIO_Init();
	OpenConsole();
	INTC_Init();

#if 0 // async mode, 400:100:50MHz
	//SYSC_ChangeMode(eASYNC_MODE);
	SYSC_ChangeMode(eSYNC_MODE);
	SYSC_ChangeSYSCLK_1(eAPLL400M, eAPLL200M, 0, 0, 3);
	bClockChange=true;
#elif 0 // async mode, 532:133:66.5MHz
	//SYSC_ChangeMode(eASYNC_MODE);
	SYSC_ChangeMode(eSYNC_MODE);
	SYSC_ChangeSYSCLK_1(eAPLL532M, eAPLL266M, 0, 0, 3);	
	bClockChange=true;
#elif 0 // async mode, 667:133:66.5MHz
	SYSC_ChangeMode(eASYNC_MODE);
	SYSC_ChangeSYSCLK_1(eAPLL667M, eAPLL266M, 0, 0, 3);
	bClockChange=true;
#endif

	if(bClockChange==true)
	{
		SYSC_GetClkInform();		
		CalibrateDelay();
		OpenConsole();
	}

#if 0 	// For 533MHz-2  VDDARM= 1.2V,  VDDINT= 1.2V
	LTC3714_VoltageSet(3, 1200);	// ARM&INT 1.2V
	Delay(100);

	LTC3714_VoltageSet(2, 1200);	// INT 1.200V
	Delay(100);
#endif

#if 0 	// For 667MHz  VDDARM= 1.3V,  VDDINT= 1.300V
	LTC3714_VoltageSet(3, 1300);	// ARM&INT 1.3V
	Delay(100);

	LTC3714_VoltageSet(2, 1300);	// INT 1.300V
	Delay(100);
#endif

	SYSC_SetLockTime(eAPLL, 300);
	SYSC_SetLockTime(eMPLL, 300);
	SYSC_SetLockTime(eEPLL, 300);

	while(1)
	{
		UART_Printf("\n\n");
		UART_Printf("***************************************************\n");
		UART_Printf("*       S3C6410 - Test firmware v0.1              *\n");
		UART_Printf("***************************************************\n");
		UART_Printf("System ID : Revision [%d], Pass [%d]\n", g_System_Revision, g_System_Pass);
		UART_Printf("ARMCLK: %.2fMHz  HCLKx2: %.2fMHz  HCLK: %.2fMHz  PCLK: %.2fMHz\n",(float)g_ARMCLK/1.0e6, (float)g_HCLKx2/1.0e6, (float)g_HCLK/1.0e6, (float)g_PCLK/1.0e6);
	#if	(VIC_MODE == 1)
		UART_Printf("VIC mode / ");
	#else
		UART_Printf("non-VIC mode / ");
	#endif
		if(g_SYNCACK==eSYNC_MODE)
			UART_Printf("Sync Mode\n\n");
		else
			UART_Printf("Async Mode\n\n");
	
		for (i=0; (u32)(menu[i].desc)!=0; i++)
		{
			UART_Printf("%2d: %s  ", i, menu[i].desc);
			if(((i+1)%4)==0)
				Putc('\n');
		}

		UART_Printf("\n\nSelect the function to test : ");
		uSel =UART_GetIntNum();
		UART_Printf("\n");

		if (uSel<(sizeof(menu)/8-1))
			(menu[uSel].func) ();
	}
	
}
