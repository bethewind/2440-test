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
*	File Name : otg_dev_test.c
*  
*	File Description : This file implements the USB OTG test functions.
*
*	Author : Haksoo,Kim
*	Dept. : AP Development Team
*	Created Date : 2007/07/11
*	Version : 0.1 
* 
*	History
*	- Created(Haksoo,Kim 2007/07/11)
*  
**************************************************************************************/


#include <stdio.h>

#include "option.h"
#include "library.h"
#include "sfr6410.h"
#include "system.h"
#include "sysc.h"
#include "intc.h"
#include "otg_dev.h"
#include "gpio.h"
#include "timer.h"
#include "nand.h"

USB_OPMODE eOpMode = USB_DMA;
USB_SPEED eSpeed = USB_HIGH;
u8 download_run=false;
u32 tempDownloadAddress;
u8 g_bSuspendResume=false;

//////////
// Function Name : Isr_UsbOtg
// Function Description : USB OTG ISR
// Input : NONE
// Output : NONE
// Version : 
void __irq Isr_UsbOtg(void)
{
	INTC_Disable(NUM_OTG);
	
	OTGDEV_HandleEvent();
	
	INTC_Enable(NUM_OTG);
	
	INTC_ClearVectAddr();
}

//////////
// Function Name : Download_Only
// Function Description : This function downloads a image file thru DNW.
//						You can configure the address to downlad in DNW "Configuration-Options" menu.
// Input : NONE
// Output : NONE
// Version : 
static void Download_Only(void)
{
	u8 *pucDownloadAddr;
	
    UART_Printf("Enter the download address(0x...):");
    pucDownloadAddr=(u8 *)UART_GetIntNum();
    if(pucDownloadAddr==(u8 *)0xffffffff)
    {
    	pucDownloadAddr=(u8 *)DefaultDownloadAddress;
    }
    
    UART_Printf("The temporary download address is 0x%x.\n\n",pucDownloadAddr);
    
    DownloadImageThruUsbOtg(pucDownloadAddr);
	
}

//////////
// Function Name : Upload_Only
// Function Description : This function uploads a image file thru DNW
//						You can configure the address and the size to upload in DNW "Configuration-Options" menu.
// Input : NONE
// Output : NONE
// Version : 
void Upload_Only()
{
	UploadImageThruUsbOtg();
}

//////////
// Function Name : Select_OpMode
// Function Description : This function selects an operation mode of USB OTG of CPU or DMA mode.
// Input : NONE
// Output : NONE
// Version : 
void Select_OpMode(void)
{
	int iSel;
	
	UART_Printf(" Current Op Mode : ");
	if(eOpMode == USB_CPU)
	{
		UART_Printf("CPU mode\n");
	}
	else if(eOpMode== USB_DMA)
	{
		UART_Printf("DMA mode\n");
	}

	UART_Printf(" Enter the op. mode (0: CPU_MODE, 1: DMA_MODE) : ");
	iSel = UART_GetIntNum();

	if (iSel != -1) 
	{
		if (iSel == 0)
			eOpMode = USB_CPU;
		else if (iSel == 1)
			eOpMode = USB_DMA;
		else
			UART_Printf("Invalid selection\n");
	}
}

//////////
// Function Name : OtgDev_Test
// Function Desctiption : test function of the OTG DEV
// Input : NONE
// Output : NONE
// Version :
void OtgDev_Test(void)
{
	s32 i, sel;
			
	const testFuncMenu menu[]=
	{
		Download_Only,			"Donwload Only",
		Upload_Only,			"Upload Only",
		Select_OpMode,			"Select Op Mode",
		0,                      0
	};
	
	while(1)
	{
		UART_Printf("\n");
		for (i=0; (int)(menu[i].desc)!=0; i++)
			UART_Printf("%2d: %s\n", i, menu[i].desc);

		UART_Printf("\nSelect the function to test : ");
		sel = UART_GetIntNum();
		UART_Printf("\n");

		if (sel == -1)
			break;
		else if (sel>=0 && sel<(sizeof(menu)/8-1))
			(menu[sel].func)();
	}
}

