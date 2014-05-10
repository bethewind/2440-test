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
*	File Name : sysc.h
*  
*	File Description : This file declares prototypes of system controller API funcions.
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

#ifndef __SYSC_H__
#define __SYSC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

#include "def.h"

extern u8		g_System_Revision, g_System_Pass, g_SYNCACK;
extern u32	g_MPLL, g_ARMCLK, g_HCLK, g_PCLK, g_MCLK;

#define S3C6410


#define DBGMSG
#undef  DBGMSG
	
#define 		Enable_CLK		(1)
#define 		Disable_CLK		(0)
// for rSDMA_SEL 
#define		SEL_SDMA		(0)
#define		SEL_GDMA		(1)

typedef enum PLL_TYPE
{
	eAPLL	=  0x0,
	eMPLL	=  0x1, 		
	eEPLL	=  0x2
}PLL_eTYPE;

typedef enum CLKSRC_Id
{
	// Vale  & 0xF0000 : '0': CLKSRC, '1': CLKSRC2
	// Value & 0x0F000 : The control bit number of the Clock Source register
	// Value & 0x00FF0 : The Offset bit number of the Clock Source register (0(0x)~31(0x1F))
	// Value & 0x0000F : The control bit value of the Clock Source register

	eTV_27M				=	0x011F0,	// Register : CLKSRC Control : 1bit,  Control bit  : [31],  Value : 0, (27MHz)
	eTV_FINEPLL			=	0x011F1,	// Register : CLKSRC Control : 1bit,  Control bit  : [31],  Value : 1, (FIN_EPLL)
	eDAC_27M			=	0x011E0,	// Register : CLKSRC Control : 1bit,  Control bit  : [30],  Value : 0, (27MHz)
	eDAC_FINEPLL		=	0x011E1,	// Register : CLKSRC Control : 1bit,  Control bit  : [30],  Value : 1, (FIN_EPLL)
	eSCALER_MOUTEPLL	=	0x031C0,	// Register : CLKSRC Control : 2bit,  Control bit  : [28],  Value : 0, (FOUT_EPLL)
	eSCALER_DOUTMPLL	=	0x031C1,	// Register : CLKSRC Control : 2bit,  Control bit  : [28],  Value : 1, (DOUT_MPLL)
	eSCALER_FINEPLL		=	0x031C2,	// Register : CLKSRC Control : 2bit,  Control bit  : [28],  Value : 2, (FIN_EPLL)	
	eLCD_MOUTEPLL		=	0x031A0,	// Register : CLKSRC Control : 2bit,  Control bit  : [26],  Value : 0, (FOUT_EPLL)
	eLCD_DOUTMPLL		=	0x031A1,	// Register : CLKSRC Control : 2bit,  Control bit  : [26],  Value : 1, (DOUT_MPLL)
	eLCD_FINEPLL		=	0x031A2,	// Register : CLKSRC Control : 2bit,  Control bit  : [26],  Value : 2, (FIN_EPLL)
	eIRDA_MOUTEPLL		=	0x03180,	// Register : CLKSRC Control : 2bit,  Control bit  : [24],  Value : 0, (FOUT_EPLL)
	eIRDA_DOUTMPLL		=	0x03181,	// Register : CLKSRC Control : 2bit,  Control bit  : [24],  Value : 1, (DOUT_MPLL)
	eIRDA_FINEPLL		=	0x03182,	// Register : CLKSRC Control : 2bit,  Control bit  : [24],  Value : 2, (FIN_EPLL)
	eIRDA_48M			=	0x03183,	// Register : CLKSRC Control : 2bit,  Control bit  : [24],  Value : 3, (48MHz)
	eMMC2_MOUTEPLL		=	0x03160,	// Register : CLKSRC Control : 2bit,  Control bit  : [22],  Value : 0, (FOUT_EPLL)	
	eMMC2_DOUTMPLL		=	0x03161,	// Register : CLKSRC Control : 2bit,  Control bit  : [22],  Value : 1, (DOUT_MPLL)
	eMMC2_FINEPLL		=	0x03162,	// Register : CLKSRC Control : 2bit,  Control bit  : [22],  Value : 2, (FIN_EPLL)
	eMMC2_27M			=	0x03163,	// Register : CLKSRC Control : 2bit,  Control bit  : [22],  Value : 3, (27MHz)
	eMMC1_MOUTEPLL		=	0x03140,	// Register : CLKSRC Control : 2bit,  Control bit  : [20],  Value : 0, (FOUT_EPLL)	
	eMMC1_DOUTMPLL		=	0x03141,	// Register : CLKSRC Control : 2bit,  Control bit  : [20],  Value : 1, (DOUT_MPLL)
	eMMC1_FINEPLL		=	0x03142,	// Register : CLKSRC Control : 2bit,  Control bit  : [20],  Value : 2, (FIN_EPLL)
	eMMC1_27M			=	0x03143,	// Register : CLKSRC Control : 2bit,  Control bit  : [20],  Value : 3, (27MHz)
	eMMC0_MOUTEPLL		=	0x03120,	// Register : CLKSRC Control : 2bit,  Control bit  : [18],  Value : 0, (FOUT_EPLL)	
	eMMC0_DOUTMPLL		=	0x03121,	// Register : CLKSRC Control : 2bit,  Control bit  : [18],  Value : 1, (DOUT_MPLL)
	eMMC0_FINEPLL		=	0x03122,	// Register : CLKSRC Control : 2bit,  Control bit  : [18],  Value : 2, (FIN_EPLL)
	eMMC0_27M			=	0x03123,	// Register : CLKSRC Control : 2bit,  Control bit  : [18],  Value : 3, (27MHz)
	eSPI1_MOUTEPLL		=	0x03100,	// Register : CLKSRC Control : 2bit,  Control bit  : [16],  Value : 0, (FOUT_EPLL)	
	eSPI1_DOUTMPLL		=	0x03101,	// Register : CLKSRC Control : 2bit,  Control bit  : [16],  Value : 1, (DOUT_MPLL)
	eSPI1_FINEPLL		=	0x03102,	// Register : CLKSRC Control : 2bit,  Control bit  : [16],  Value : 2, (FIN_EPLL)
	eSPI1_27M			=	0x03103,	// Register : CLKSRC Control : 2bit,  Control bit  : [16],  Value : 3, (27MHz)
	eSPI0_MOUTEPLL		=	0x030E0,	// Register : CLKSRC Control : 2bit,  Control bit  : [14],  Value : 0, (FOUT_EPLL)	
	eSPI0_DOUTMPLL		=	0x030E1,	// Register : CLKSRC Control : 2bit,  Control bit  : [14],  Value : 1, (DOUT_MPLL)
	eSPI0_FINEPLL		=	0x030E2,	// Register : CLKSRC Control : 2bit,  Control bit  : [14],  Value : 2, (FIN_EPLL)
	eSPI0_27M			=	0x030E3,	// Register : CLKSRC Control : 2bit,  Control bit  : [14],  Value : 3, (27MHz)
	eUART_MOUTEPLL		=	0x010D0,	// Register : CLKSRC Control : 1bit,  Control bit  : [13],  Value : 0, (FOUT_EPLL)
	eUART_DOUTMPLL		=	0x010D1,	// Register : CLKSRC Control : 1bit,  Control bit  : [13],  Value : 1, (DOUT_MPLL)
	eAUDIO1_MOUTEPLL	=	0x070A0,	// Register : CLKSRC Control : 3bit,  Control bit  : [10],  Value : 0, (FOUT_EPLL)
	eAUDIO1_DOUTMPLL	=	0x070A1,	// Register : CLKSRC Control : 3bit,  Control bit  : [10],  Value : 1, (DOUT_MPLL)	
	eAUDIO1_FINEPLL		=	0x070A2,	// Register : CLKSRC Control : 3bit,  Control bit  : [10],  Value : 2, (FIN_EPLL)
	eAUDIO1_I2SCDCLK1	=	0x070A3,	// Register : CLKSRC Control : 3bit,  Control bit  : [10],  Value : 3, (IISCDCLK1)
	eAUDIO1_PCMCDCLK	=	0x070A4,	// Register : CLKSRC Control : 3bit,  Control bit  : [10],  Value : 4, (PCMCDCLK)
	eAUDIO0_MOUTEPLL	=	0x07070,	// Register : CLKSRC Control : 3bit,  Control bit  : [7],  Value : 0, (FOUT_EPLL)
	eAUDIO0_DOUTMPLL	=	0x07071,	// Register : CLKSRC Control : 3bit,  Control bit  : [7],  Value : 1, (DOUT_MPLL)	
	eAUDIO0_FINEPLL		=	0x07072,	// Register : CLKSRC Control : 3bit,  Control bit  : [7],  Value : 2, (FIN_EPLL)
	eAUDIO0_I2SCDCLK0	=	0x07073,	// Register : CLKSRC Control : 3bit,  Control bit  : [7],  Value : 3, (IISCDCLK0)
	eAUDIO0_PCMCDCLK	=	0x07074,	// Register : CLKSRC Control : 3bit,  Control bit  : [7],  Value : 4, (PCMCDCLK)
	eUHOST_48M			=	0x03050,	// Register : CLKSRC Control : 2bit,  Control bit  : [5],  Value : 0, (48MHz)
	eUHOST_MOUTEPLL		=	0x03051,	// Register : CLKSRC Control : 2bit,  Control bit  : [5],  Value : 1, (FOUT EPLL)
	eUHOST_DOUTMPLL		=	0x03052,	// Register : CLKSRC Control : 2bit,  Control bit  : [5],  Value : 2, (DOUT MPLL)
	eUHOST_FINEPLL		=	0x03053,	// Register : CLKSRC Control : 2bit,  Control bit  : [5],  Value : 3, (FIN EPLL)
	eMFC_HCLKx2			=	0x01040,	// Register : CLKSRC Control : 1bit,  Control bit  : [4],  Value : 0, (HCLKx2)
	eMFC_MOUTEPLL		=	0x01041,	// Register : CLKSRC Control : 1bit,  Control bit  : [4],  Value : 1, (MOUT_EPLL)
	eEPLL_FIN			= 	0x01020,	// Register : CLKSRC Control : 1bit,  Control bit  : [2],  Value : 0, (FIN)
	eEPLL_FOUT			=	0x01021,	// Register : CLKSRC Control : 1bit,  Control bit  : [2],  Value : 1, (FOUT)
	eMPLL_FIN			= 	0x01010,	// Register : CLKSRC Control : 1bit,  Control bit  : [1],  Value : 0, (FIN)
	eMPLL_FOUT			=	0x01011,	// Register : CLKSRC Control : 1bit,  Control bit  : [1],  Value : 1, (FOUT)
	eAPLL_FIN			= 	0x01000,	// Register : CLKSRC Control : 1bit,  Control bit  : [0],  Value : 0, (FIN)
	eAPLL_FOUT			=	0x01001,	// Register : CLKSRC Control : 1bit,  Control bit  : [0],  Value : 1, (FOUT)
	eAUDIO2_MOUTEPLL	=	0x17000,	// Register : CLKSRC2 Control : 3bit,  Control bit  : [0],  Value : 0, (MOUT EPLL)
	eAUDIO2_DOUTMPLL	=	0x17001,	// Register : CLKSRC2 Control : 3bit,  Control bit  : [0],  Value : 1, (DOUT MPLL)
	eAUDIO2_FINEPLL		=	0x17002,	// Register : CLKSRC2 Control : 3bit,  Control bit  : [0],  Value : 2, (FIN EPLL)
	eAUDIO2_I2SCDCLK1	=	0x17003,	// Register : CLKSRC2 Control : 3bit,  Control bit  : [0],  Value : 3, (I2SCDCLK1)
	eAUDIO2_PCMCDCLK	=	0x17004,	// Register : CLKSRC2 Control : 3bit,  Control bit  : [0],  Value : 4, (PCMCDCLK)				
}CLKSRC_eId;

typedef enum CLKDIV_Id
{
	// Vale  & 0xF00 : '0': CLKDIV0, '1': CLKDIV1, '2': CLKDIV2
	// Value & 0x0F00 : The control bit number of the Clock Source register
	// Value & 0x00FF : The Offset bit number of the Clock Source register (0(0x)~31(0x1F))
	eMFC_RATIO			= 	0x0F1C,	// Register:CLKDIV0, Control:4bit, Control bit:[28]
	eJPEG_RATIO			= 	0x0F18,	// Register:CLKDIV0, Control:4bit, Control bit:[24]
	eCAM_RATIO			= 	0x0F14,	// Register:CLKDIV0, Control:4bit, Control bit:[20]
	eSECUR_RATIO		= 	0x0312,	// Register:CLKDIV0, Control:2bit, Control bit:[18]
	ePCLK_RATIO			= 	0x0F0C,	// Register:CLKDIV0, Control:4bit, Control bit:[12]
	eHCLKX2_RATIO		= 	0x0709,	// Register:CLKDIV0, Control:3bit, Control bit:[9]
	eHCLK_RATIO			= 	0x0108,	// Register:CLKDIV0, Control:1bit, Control bit:[8]
	eMPLL_RATIO			= 	0x0104,	// Register:CLKDIV0, Control:1bit, Control bit:[4]
	eARM_RATIO			= 	0x0F00,	// Register:CLKDIV0, Control:4bit, Control bit:[0]
	eFIMC_RATIO			= 	0x1F18,	// Register:CLKDIV1, Control:4bit, Control bit:[24]
	eUHOST_RATIO		= 	0x1F14,	// Register:CLKDIV1, Control:4bit, Control bit:[20]
	eSCALER_RATIO		= 	0x1F10,	// Register:CLKDIV1, Control:4bit, Control bit:[16]
	eLCD_RATIO			= 	0x1F0C,	// Register:CLKDIV1, Control:4bit, Control bit:[12]
	eMMC2_RATIO			= 	0x1F08,	// Register:CLKDIV1, Control:4bit, Control bit:[8]
	eMMC1_RATIO			= 	0x1F04,	// Register:CLKDIV1, Control:4bit, Control bit:[4]
	eMMC0_RATIO			= 	0x1F00,	// Register:CLKDIV1, Control:4bit, Control bit:[0]
	eAUDIO2_RATIO		= 	0x2F18,	// Register:CLKDIV2, Control:4bit, Control bit:[24]
	eIRDA_RATIO			= 	0x2F14,	// Register:CLKDIV2, Control:4bit, Control bit:[20]
	eUART_RATIO			= 	0x2F10,	// Register:CLKDIV2, Control:4bit, Control bit:[16]
	eAUDIO1_RATIO		= 	0x2F0C,	// Register:CLKDIV2, Control:4bit, Control bit:[12]
	eAUDIO0_RATIO		= 	0x2F08,	// Register:CLKDIV2, Control:4bit, Control bit:[8]
	eSPI1_RATIO			= 	0x2F04,	// Register:CLKDIV2, Control:4bit, Control bit:[4]						
	eSPI0_RATIO			= 	0x2F00,	// Register:CLKDIV2, Control:4bit, Control bit:[0]							
}CLKDIV_eId;

//not implemented

typedef enum CLOCK_GATE
{

	eDMC0 = 0x1,
/*	eDMC1 = 0x2,
	eSROM = 0x3,
	eONENAND0 = 0x1,	
	eONENAND1 = 0x1,
	eNFCON = 0x1,
	eCFCON = 0x1,
	eGPIO = 0x1,
	eDMA0 = 0x1,
	eDMA1 = 0x1,
	eSDMA0 = 0x1,	
	eSDMA1 = 0x1,
	eVIC = 0x1,
	eSECUR = 0x1,
	eLCD = 0x1,
	eDMC0 = 0x1,
	eDMC0 = 0x1,
	eDMC0 = 0x1,	
	eDMC0 = 0x1,
	eDMC0 = 0x1,
	eDMC0 = 0x1,
	eDMC0 = 0x1,
	eDMC0 = 0x1,
	eDMC0 = 0x1,
	eDMC0 = 0x1,	
	eDMC0 = 0x1,
	eDMC0 = 0x1,
	eDMC0 = 0x1,
*/ 	
}CLOCK_eGATE;


typedef enum H_GATE
{
	eHCLK_MFC		=	0,
	eHCLK_INTC		=	1, 		
	eHCLK_TZIC		=	2,
	eHCLK_LCD		=	3,
	eHCLK_ROT		=	4,
	eHCLK_POST		=	5,
	eHCLK_TV		=	7,
	eHCLK_2D		=	8,
	eHCLK_SCALER 	=	9,
	eHCLK_CAM		=	10,
	eHCLK_JPEG		=	11,
	eHCLK_DMA0	=	12,
	eHCLK_DMA1	=	13,
	eHCLK_HOSTIF 	=	14,
	eHCLK_Modem 	=	15,
	eHCLK_MDP		=	16,
	eHCLK_MMC0	=	17,
	eHCLK_MMC1	=	18,
	eHCLK_MMC2	=	19,
	eHCLK_OTG		=	20,
	eHCLK_DMC0		=	21,
	eHCLK_DMC1		=	22,
	eHCLK_DDR0		=	23,
	eHCLK_DDR1		=	24,
	eHCLK_IROM		=	25,
	eHCLK_UHOST 	=	26,		//EVT0
	eHCLK_SDMA0	=	26,
	eHCLK_SDMA1 	=	27,
	eHCLK_SECUR  	=	28,
	//eHCLK_UHOST	=	29,		//EVT1
	eHCLK_BUS		=	30
}H_eGATE;

typedef enum P_GATE
{
	ePCLK_MFC		=	0,
	ePCLK_UART0	=	1, 		
	ePCLK_UART1	=	2,
	ePCLK_UART2	=	3,
	ePCLK_UART3	=	4,
	ePCLK_WDT		=	5,
	ePCLK_RTC		=	6,
	ePCLK_PWM		=	7,
	ePCLK_PCM0		=	8,
	ePCLK_PCM1		=	9,
	ePCLK_IRDA		=	10,
	ePCLK_KEYPAD 	=	11,
	ePCLK_TSADC	=	12,
	ePCLK_TZPC		=	13,
	ePCLK_AC97   	=	14,
	ePCLK_I2S0 		=	15,
	ePCLK_I2S1		=	16,
	ePCLK_I2C		=	17,
	ePCLK_GPIO		=	18,
	ePCLK_HSITX	=	19,
	ePCLK_HSIRX	=	20,
	ePCLK_SPI0		=	21,
	ePCLK_SPI1		=	22,
	ePCLK_CHIPID	=	23,
	ePCLK_SKEY		=	24,
	ePCLK_IIC1		= 	27
}P_eGATE;

typedef enum S_GATE
{
	eSCLK_JPEG		=	1, 		
	eSCLK_CAM		=	2,
	eSCLK_MFC		=	3,
	eSCLK_OND		=	4,
	eSCLK_UART		=	5,
	eSCLK_IRDA		=	6,
	eSCLK_SECUR	=	7,
	eSCLK_AUDIO0	=	8,
	eSCLK_AUDIO1	=	9,
	eSCLK_POST0	=	10,
	eSCLK_POST1	=	11,
	eSCLK_POST0_27=	12,
	eSCLK_POST1_27=	13,
	eSCLK_LCD		=	14,
	eSCLK_LCD_27	=	15,
	eSCLK_SCALER	=	16,
	eSCLK_SCALER27	=	17,
	eSCLK_TV27		=	18,
	eSCLK_DAC27	=	19,
	eSCLK_SPI0		=	20,
	eSCLK_SPI1		=	21,
	eSCLK_SPI0_48	=	22,
	eSCLK_SPI1_48	=	23,
	eSCLK_MMC0		=	24,
	eSCLK_MMC1		=	25,
	eSCLK_MMC2		=	26,
	eSCLK_MMC0_48	=	27,
	eSCLK_MMC1_48 =	28,
	eSCLK_MMC2_48 =	29,
	eSCLK_UHOST	=	30
}S_eGATE;

// 
typedef enum CLKOUT_TYPE
{
	eCLKOUT_APLLOUT	=  0x0,		// FOUT_APLL/2
	eCLKOUT_EPLLOUT	=  0x1, 		
	eCLKOUT_HCLK		=  0x2,
	eCLKOUT_48M		=  0x3,
	eCLKOUT_27M		=  0x4,
	eCLKOUT_RTC		=  0x5,
	eCLKOUT_TICK		=  0x6,
	eCLKOUT_DOUT		=  0x7,
	eCLKOUT_FIN		=  0x8, 		
	eCLKOUT_FREFAPLL	=  0x9,
	eCLKOUT_FFBAPLL2	=  0xa,
	eCLKOUT_FREFMPLL	=  0xb,
	eCLKOUT_FFBMPLL2	=  0xc,
	eCLKOUT_FREFEPLL	=  0xd,
	eCLKOUT_FREFEPLL2	=  0xe,	
}CLKOUT_eTYPE;


typedef enum BUSCTRL_Id
{
	// Value & 0xF00 : The BUS Control Register ID, OffSet
	// Value & 0x0F0 : The BUS Priority Type ID
	// Value & 0x00F : The Fixed Priority Order

	eAHBF_FIX0			=	0x000,		// rAHB_CON0(0),  Fixed Order,  Fixed Order type 0
	eAHBF_FIX1			=	0x001,		// rAHB_CON0(0),  Fixed Order,  Fixed Order type 1
	eAHBF_FIX2			=	0x002,		// rAHB_CON0(0),  Fixed Order,  Fixed Order type 2
	eAHBF_FIX3			=	0x003,		// rAHB_CON0(0),  Fixed Order,  Fixed Order type 3
	eAHBF_FIX4			=	0x004,		// rAHB_CON0(0),  Fixed Order,  Fixed Order type 4
	eAHBF_FIX5			=	0x005,		// rAHB_CON0(0),  Fixed Order,  Fixed Order type 5
	eAHBF_FIX6			=	0x006,		// rAHB_CON0(0),  Fixed Order,  Fixed Order type 6
	eAHBF_LGL			=	0x010,		// rAHB_CON0(0),  Last grant order.
	eAHBF_ROT			=	0x020,		// rAHB_CON0(0),  Rotation
	eAHBX_FIX0			=	0x100,		// rAHB_CON0(1),  Fixed Order,  Fixed Order type 0
	eAHBX_FIX1			=	0x101,		// rAHB_CON0(1),  Fixed Order,  Fixed Order type 1
	eAHBX_FIX2			=	0x102,		// rAHB_CON0(1),  Fixed Order,  Fixed Order type 2
	eAHBX_FIX3			=	0x103,		// rAHB_CON0(1),  Fixed Order,  Fixed Order type 3
	eAHBX_FIX4			=	0x104,		// rAHB_CON0(1),  Fixed Order,  Fixed Order type 4
	eAHBX_FIX5			=	0x105,		// rAHB_CON0(1),  Fixed Order,  Fixed Order type 5
	eAHBX_FIX6			=	0x106,		// rAHB_CON0(1),  Fixed Order,  Fixed Order type 6
	eAHBX_LGL			=	0x110,		// rAHB_CON0(1),  Last grant order.
	eAHBX_ROT			=	0x120,		// rAHB_CON0(1),  Rotation
	eAHBP_FIX0			=	0x200,		// rAHB_CON0(2),  Fixed Order,  Fixed Order type 0
	eAHBP_FIX1			=	0x201,		// rAHB_CON0(2),  Fixed Order,  Fixed Order type 1
	eAHBP_FIX2			=	0x202,		// rAHB_CON0(2),  Fixed Order,  Fixed Order type 2
	eAHBP_FIX3			=	0x203,		// rAHB_CON0(2),  Fixed Order,  Fixed Order type 3
	eAHBP_FIX4			=	0x204,		// rAHB_CON0(2),  Fixed Order,  Fixed Order type 4
	eAHBP_FIX5			=	0x205,		// rAHB_CON0(2),  Fixed Order,  Fixed Order type 5
	eAHBP_FIX6			=	0x206,		// rAHB_CON0(2),  Fixed Order,  Fixed Order type 6
	eAHBP_LGL			=	0x210,		// rAHB_CON0(2),  Last grant order.
	eAHBP_ROT			=	0x220,		// rAHB_CON0(2),  Rotation
	eAHBI_FIX0			=	0x300,		// rAHB_CON0(3),  Fixed Order,  Fixed Order type 0
	eAHBI_FIX1			=	0x301,		// rAHB_CON0(3),  Fixed Order,  Fixed Order type 1
	eAHBI_FIX2			=	0x302,		// rAHB_CON0(3),  Fixed Order,  Fixed Order type 2
	eAHBI_FIX3			=	0x303,		// rAHB_CON0(3),  Fixed Order,  Fixed Order type 3
	eAHBI_FIX4			=	0x304,		// rAHB_CON0(3),  Fixed Order,  Fixed Order type 4
	eAHBI_FIX5			=	0x305,		// rAHB_CON0(3),  Fixed Order,  Fixed Order type 5
	eAHBI_FIX6			=	0x306,		// rAHB_CON0(3),  Fixed Order,  Fixed Order type 6
	eAHBI_LGL			=	0x310,		// rAHB_CON0(3),  Last grant order.
	eAHBI_ROT			=	0x320,		// rAHB_CON0(3),  Rotation
	eAHBT0_FIX0		=	0x400,		// rAHB_CON1(0),  Fixed Order,  Fixed Order type 0
	eAHBT0_FIX1		=	0x401,		// rAHB_CON1(0),  Fixed Order,  Fixed Order type 1
	eAHBT0_FIX2		=	0x402,		// rAHB_CON1(0),  Fixed Order,  Fixed Order type 2
	eAHBT0_FIX3		=	0x403,		// rAHB_CON1(0),  Fixed Order,  Fixed Order type 3
	eAHBT0_FIX4		=	0x404,		// rAHB_CON1(0),  Fixed Order,  Fixed Order type 4
	eAHBT0_FIX5		=	0x405,		// rAHB_CON1(0),  Fixed Order,  Fixed Order type 5
	eAHBT0_FIX6		=	0x406,		// rAHB_CON1(0),  Fixed Order,  Fixed Order type 6
	eAHBT0_LGL			=	0x410,		// rAHB_CON1(0),  Last grant order.
	eAHBT0_ROT			=	0x420,		// rAHB_CON1(0),  Rotation
	eAHBT1_FIX0		=	0x500,		// rAHB_CON1(1),  Fixed Order,  Fixed Order type 0
	eAHBT1_FIX1		=	0x501,		// rAHB_CON1(1),  Fixed Order,  Fixed Order type 1
	eAHBT1_FIX2		=	0x502,		// rAHB_CON1(1),  Fixed Order,  Fixed Order type 2
	eAHBT1_FIX3		=	0x503,		// rAHB_CON1(1),  Fixed Order,  Fixed Order type 3
	eAHBT1_FIX4		=	0x504,		// rAHB_CON1(1),  Fixed Order,  Fixed Order type 4
	eAHBT1_FIX5		=	0x505,		// rAHB_CON1(1),  Fixed Order,  Fixed Order type 5
	eAHBT1_FIX6		=	0x506,		// rAHB_CON1(1),  Fixed Order,  Fixed Order type 6
	eAHBT1_LGL			=	0x510,		// rAHB_CON1(1),  Last grant order.
	eAHBT1_ROT			=	0x520,		// rAHB_CON1(1),  Rotation
	eAHBM0_FIX0		=	0x600,		// rAHB_CON1(2),  Fixed Order,  Fixed Order type 0
	eAHBM0_FIX1		=	0x601,		// rAHB_CON1(2),  Fixed Order,  Fixed Order type 1
	eAHBM0_FIX2		=	0x602,		// rAHB_CON1(2),  Fixed Order,  Fixed Order type 2
	eAHBM0_FIX3		=	0x603,		// rAHB_CON1(2),  Fixed Order,  Fixed Order type 3
	eAHBM0_FIX4		=	0x604,		// rAHB_CON1(2),  Fixed Order,  Fixed Order type 4
	eAHBM0_FIX5		=	0x605,		// rAHB_CON1(2),  Fixed Order,  Fixed Order type 5
	eAHBM0_FIX6		=	0x606,		// rAHB_CON1(2),  Fixed Order,  Fixed Order type 6
	eAHBM0_LGL			=	0x610,		// rAHB_CON1(2),  Last grant order.
	eAHBM0_ROT		=	0x620,		// rAHB_CON1(2),  Rotation
	eAHBM1_FIX0		=	0x700,		// rAHB_CON1(3),  Fixed Order,  Fixed Order type 0
	eAHBM1_FIX1		=	0x701,		// rAHB_CON1(3),  Fixed Order,  Fixed Order type 1
	eAHBM1_FIX2		=	0x702,		// rAHB_CON1(3),  Fixed Order,  Fixed Order type 2
	eAHBM1_FIX3		=	0x703,		// rAHB_CON1(3),  Fixed Order,  Fixed Order type 3
	eAHBM1_FIX4		=	0x704,		// rAHB_CON1(3),  Fixed Order,  Fixed Order type 4
	eAHBM1_FIX5		=	0x705,		// rAHB_CON1(3),  Fixed Order,  Fixed Order type 5
	eAHBM1_FIX6		=	0x706,		// rAHB_CON1(3),  Fixed Order,  Fixed Order type 6
	eAHBM1_LGL			=	0x710,		// rAHB_CON1(3),  Last grant order.
	eAHBM1_ROT		=	0x720,		// rAHB_CON1(3),  Rotation
	eAHBS0_FIX0		=	0x800,		// rAHB_CON2(0),  Fixed Order,  Fixed Order type 0
	eAHBS0_FIX1		=	0x801,		// rAHB_CON2(0),  Fixed Order,  Fixed Order type 1
	eAHBS0_FIX2		=	0x802,		// rAHB_CON2(0),  Fixed Order,  Fixed Order type 2
	eAHBS0_FIX3		=	0x803,		// rAHB_CON2(0),  Fixed Order,  Fixed Order type 3
	eAHBS0_FIX4		=	0x804,		// rAHB_CON2(0),  Fixed Order,  Fixed Order type 4
	eAHBS0_FIX5		=	0x805,		// rAHB_CON2(0),  Fixed Order,  Fixed Order type 5
	eAHBS0_FIX6		=	0x806,		// rAHB_CON2(0),  Fixed Order,  Fixed Order type 6
	eAHBS0_LGL			=	0x810,		// rAHB_CON2(0),  Last grant order.
	eAHBS0_ROT			=	0x820,		// rAHB_CON2(0),  Rotation
	eAHBS1_FIX0		=	0x900,		// rAHB_CON2(1),  Fixed Order,  Fixed Order type 0
	eAHBS1_FIX1		=	0x901,		// rAHB_CON2(1),  Fixed Order,  Fixed Order type 1
	eAHBS1_FIX2		=	0x902,		// rAHB_CON2(1),  Fixed Order,  Fixed Order type 2
	eAHBS1_FIX3		=	0x903,		// rAHB_CON2(1),  Fixed Order,  Fixed Order type 3
	eAHBS1_FIX4		=	0x904,		// rAHB_CON2(1),  Fixed Order,  Fixed Order type 4
	eAHBS1_FIX5		=	0x905,		// rAHB_CON2(1),  Fixed Order,  Fixed Order type 5
	eAHBS1_FIX6		=	0x906,		// rAHB_CON2(1),  Fixed Order,  Fixed Order type 6
	eAHBS1_LGL			=	0x910,		// rAHB_CON2(1),  Last grant order.
	eAHBS1_ROT			=	0x920		// rAHB_CON2(1),  Rotation
	
}BUSCTRL_eId;

typedef enum DMASELECT_ID
{
	eSEL_UART0_0		=	0,
	eSEL_UART0_1		=	1, 		
	eSEL_UART1_0		=	2,
	eSEL_UART1_1		=	3,
	eSEL_UART2_0		=	4,
	eSEL_UART2_1		=	5,
	eSEL_UART3_0		=	6,
	eSEL_UART3_1		=	7,
	eSEL_PCM0_TX		=	8,
	eSEL_PCM0_RX		=	9,
	eSEL_I2S0_TX		=	10,
	eSEL_I2S0_RX		=	11,
	eSEL_SPI0_TX		=	12,
	eSEL_SPI0_RX		=	13,
	eSEL_HSI_TX		=	14,
	eSEL_HSI_RX		=	15,
	eSEL_PCM1_TX		=	16,
	eSEL_PCM1_RX		=	17,
	eSEL_I2S1_TX		=	18,
	eSEL_I2S1_RX		=	19,
	eSEL_SPI1_TX		=	20,
	eSEL_SPI1_RX		=	21,
	eSEL_AC_PCMOUT	=	22,
	eSEL_AC_PCMIN		=	23,
	eSEL_AC_MICIN		=	24,
	eSEL_PWM			=	25,
	eSEL_IRDA			=	26,
	eSEL_EXTERNAL		=	27,
	eSEL_SECU_RX		=	30,
	eSEL_SECU_TX		=	31,
	eSEL_MODEM_TX0	=	32,	// 0x20
	eSEL_MODEM_TX1	=	33,	// 0x21
	eSEL_MODEM_RX0	=	34,	// 0x22
	eSEL_MODEM_RX1	=	35	// 0x23
}DMASELECT_eID;

typedef enum EBIPrio_ID
{
	// Value & 0xF0 : Priority Type, 0: Fixed,  1: Circular
	// Value & 0x0F : The Fixed Priority Order

	eEBI_CIRCULAR		=	0x10,		// Circular Priority
	eEBI_FIX0			=	0x00,		// Fixed Type0 : DMC0-SROMC-OND0-OND1-NFC-CFC
	eEBI_FIX1			=	0x01,		// Fixed Type1 : DMC0-OND0-OND1-SROMC-NFC-CFC
	eEBI_FIX2			=	0x02,		// Fixed Type2 : DMC0-OND1-NFC-SROMC-OND0-CFC
	eEBI_FIX3			=	0x03,		// Fixed Type3 : DMC0-NFC-SROMC-OND0-OND1-CFC
	eEBI_FIX4			=	0x04,		// Fixed Type4 : DMC0-CFC-SROMC-OND0-OND1-NFC
	eEBI_FIX5			=	0x05		// Fixed Type5 : SROMC-DMC0-OND0-OND1-NFC-CFC
	
}EBIPrio_eID;

typedef enum SelCS_MEM0
{
	eCS_SROMC			=	0,
	eCS_CFC			=	1,
	eCS_ONDC			=	2,
	eCS_NFC			=	3
}SelCS_eMEM0;

typedef enum CFG_WFI
{
	eIGNORE			= 	0,
	eIDLE				=	1,
	eSTOP				=	2,
	eSLEEP				=	3
}CFG_eWFI;

typedef enum BATFLT_MODE
{
	eFLT_IGNORE		= 	0,
	eFLT_INT			=	1,
	eFLT_SLEEP			=	3		// Manual Changed
}BATFLT_eMODE;

typedef enum WKUPMSK_ID
{
	// Value & 0xF00 : Register Offset : 0 (rPWR_CFG), 1(rEINT_MASK)
	// Value & 0x0FF : Bit Offset

	eMASK_BATF	=	0x000,
	eMASK_KEY		=	0x001,
	eMASK_MSM		=	0x002,
	eMASK_ALARM	=	0x003,
	eMASK_TICK		=	0x004,
	eMASK_TS		=	0x005,
	eMASK_HSI		=	0x006,
	eMASK_MMC0	=	0x007,
	eMASK_MMC1	=	0x008,
	eMASK_MMC2	=	0x009,
	
	eMASK_EINT0	=	0x100,
	eMASK_EINT1	=	0x101,
	eMASK_EINT2	=	0x102,
	eMASK_EINT3	=	0x103,
	eMASK_EINT4	=	0x104,
	eMASK_EINT5	=	0x105,
	eMASK_EINT6	=	0x106,
	eMASK_EINT7	=	0x107,
	eMASK_EINT8	=	0x108,
	eMASK_EINT9	=	0x109,
	eMASK_EINT10	=	0x10A,
	eMASK_EINT11	=	0x10B,
	eMASK_EINT12	=	0x10C,
	eMASK_EINT13	=	0x10D,
	eMASK_EINT14	=	0x10E,
	eMASK_EINT15	=	0x10F,
	eMASK_EINT16	=	0x110,
	eMASK_EINT17	=	0x111,
	eMASK_EINT18	=	0x112,
	eMASK_EINT19	=	0x113,
	eMASK_EINT20	=	0x114,
	eMASK_EINT21	=	0x115,
	eMASK_EINT26	=	0x116,
	eMASK_EINT27	=	0x117
	
}WKUPMSK_eID;

typedef enum NORGFC_ID
{
	eNOR_DOMAINV		= 	9,
	eNOR_DOMAING		= 	10,		
	eNOR_DOMAINSS	=	11,		
	eNOR_DOMAINI		=	12,
	eNOR_DOMAINP		=	13,
	eNOR_DOMAINF		=	14,
	eNOR_DOMAINS		=	15,
	eNOR_DOMAINETM	=	16,
	eNOR_IROM			=	30
	
}NORGFC_eID;

typedef enum BLKPWRSTA_ID
{
	eBLKPWRSTA_DOMAINTOP	=	0,	
	eBLKPWRSTA_DOMAINV		=	1,
	eBLKPWRSTA_DOMAINI		=	2,
	eBLKPWRSTA_DOMAINP		=	3,
	eBLKPWRSTA_DOMAINF		=	4,
	eBLKPWRSTA_DOMAINS		=	5,
	eBLKPWRSTA_DOMAINETM	=	6,
	eBLKPWRSTA_DOMAING		=	7,
	eBLKPWRSTA_DOMAINSS	 	=	8,	
}BLKPWRSTA_eID;


typedef enum STOPCFG_ID
{
	eSTOP_OSCEN		=	0,
	eSTOP_TOPLOGIC	=	8,
	eSTOP_DOMAINV		= 	9,
	eSTOP_DOMAING		= 	10,		
	eSTOP_DOMAINSS	=	11,		
	eSTOP_DOMAINI		=	12,
	eSTOP_DOMAINP		=	13,
	eSTOP_DOMAINF		=	14,
	eSTOP_DOMAINS		=	15,
	eSTOP_DOMAINETM	=	16,
	eSTOP_ARMLOGIC	=	17,
	eSTOP_TOPMEMORY	=	20,
	eSTOP_ARMMEMORY	=	29,
	eSTOP_IROM			=	30
	
}STOPCFG_eID;

typedef enum STOPMEMGFC_ID
{
	eSTOPMEM_NFCON	=	0,
	eSTOPMEM_IRDA		=	1,
	eSTOPMEM_IROM		=	2,
	eSTOPMEM_HSMMC	=	3,
	eSTOPMEM_OTG		=	4,
	eSTOPMEM_HOSTIF	=	5,
	eSTOPMEM_MODEMIF	=	6,
}STOPMEMCFG_eID;


typedef enum SYSC_OPMode
{
	eASYNC_MODE		=	0,
	eSYNC_MODE		= 	1
}SYSC_eOPMode;

typedef enum APLL_OUT
{
	eAPLL900M	=	0x0e10300,
	eAPLL800M	=	0x1900301,
	eAPLL667M	=	0x29b0601,		// need to check
	eAPLL666M	=	0x14d0301,
	
//	eAPLL600M	=	0x2580301,
	eAPLL560M	=	0x1180301,
	eAPLL550M	=	0x1130301,
	eAPLL540M	=	0x10e0301,
	eAPLL532M	=	0x10a0301,

//	eAPLL520M	=	0x2080601,
	eAPLL510M	=	0x0FF0301,
	eAPLL500M	=  	0x0F40301,
	eAPLL490M	=	0x0f50301,
	eAPLL480M	= 	0x0f00301,
	eAPLL470M	=	0x0eb0301,
	eAPLL460M	=	0x0e60301,
	eAPLL450M	=	0x0e10301,	
	eAPLL440M	=	0x0dc0301,	
	eAPLL430M	=	0x0d70301,
	eAPLL420M	=	0x0d20301,			// 420, 3, 2
	eAPLL410M	=	0x0cd0301,			// 410, 3, 2
	eAPLL400M	=	0x1900302,			// 400, 3, 2
	
	eAPLL380M	=	0x17C0302,
	eAPLL340M	=	0x1540302,
	eAPLL300M	=	0x12C0302,

	eAPLL266M	=	0x10a0302,
	eAPLL200M	= 	0x1900303,
	
	eAPLL133M	=	0x10a0301,
	eAPLL100M	= 	0x1900304,
//	eAPLL66_5M	= 	0x2140604,
	eAPLL50M	= 	0x1900305,
	
}APLL_eOUT;

/******************************************/
/*          Function Define               */
/******************************************/

#define Outp32Inform(No_Reg, data)	(*(volatile u32 *)(0x7E00FA00+ 4*No_Reg) = (data))
#define Inp32Inform(No_Reg)			(*(volatile u32 *)(0x7E00FA00+ 4*No_Reg))
#define Outp32SYSC(Offset, data)		(*(volatile u32 *)(0x7E00F000+ Offset) = (data))
#define Inp32SYSC(Offset)				(*(volatile u32 *)(0x7E00F000+Offset))


#define SYSC_SWRST()         			{Outp32SYSC(0x114, 0x6410);}
#define SYSC_EBICF()					{Outp32SYSC(0x120, Inp32SYSC(0x120)&~(1<<14));}
#define SYSC_IndepCF()				{Outp32SYSC(0x120, Inp32SYSC(0x120)|(1<<14));}
#define SYSC_8bitBUS()				{Outp32SYSC(0x120, Inp32SYSC(0x120)&~(1<<12));}
#define SYSC_16bitBUS()				{Outp32SYSC(0x120, Inp32SYSC(0x120)|(1<<12));}	
#define SYSC_ExpandMEM1()			{Outp32SYSC(0x120, Inp32SYSC(0x120)&~(1<<7));}
#define SYSC_ExpandMEM0()			{Outp32SYSC(0x120, Inp32SYSC(0x120)|(1<<7));}	
#define SYSC_DisOSC27()				{Outp32SYSC(0x804, Inp32SYSC(0x804)&~(1<<0));}
#define SYSC_EnOSC27()				{Outp32SYSC(0x804, Inp32SYSC(0x804)|(1<<0));}	
#define SYSC_DisOSCSLEEP()			{Outp32SYSC(0x818, Inp32SYSC(0x818)&~(1<<0));}
#define SYSC_EnOSCSLEEP()			{Outp32SYSC(0x818, Inp32SYSC(0x818)|(1<<0));}	
#define SYSC_BLKPwrONAll()			{Outp32SYSC(0x810, 0xFFFFFFFF);}
#define SYSC_BLKPwrOffAll()			{Outp32SYSC(0x810, 0xBFFE0D00);}
#define SYSC_OSCFREQ(data)			{Outp32SYSC(0x820,data);}
#define  SYSC_OSCSTAB(data)			{Outp32SYSC(0x824,data);}
#define SYSC_PWRSTAB(data)			{Outp32SYSC(0x828,data);}
#define SYSC_FPCSTAB(data)			{Outp32SYSC(0x82c,data);}
#define SYSC_MTCSTAB(data)			{Outp32SYSC(0x830,data);}

#define SYSC_OTGSIGDisMSK()			{Outp32SYSC(0x900, Inp32SYSC(0x900)&~(1<<16));}
#define SYSC_OTGSIGEnMSK()			{Outp32SYSC(0x900, Inp32SYSC(0x900)|(1<<16));}	
#define SYSC_OTGDisPwr()			{Outp32SYSC(0x900, Inp32SYSC(0x900)&~(1<<14));}
#define SYSC_OTGEnPwr()				{Outp32SYSC(0x900, Inp32SYSC(0x900)|(1<<14));}
#define SYSC_ClrDBGACK()				{Outp32SYSC(0x900, Inp32SYSC(0x900)|(1<<13));}
#define SYSC_DisDBG()				{Outp32SYSC(0x900, Inp32SYSC(0x900)&~(1<<3));}
#define SYSC_EnDBG()				{Outp32SYSC(0x900, Inp32SYSC(0x900)|(1<<3));}
#define SYSC_DisNonInvDBG()			{Outp32SYSC(0x900, Inp32SYSC(0x900)&~(1<<2));}
#define SYSC_EnNonInvDBG()			{Outp32SYSC(0x900, Inp32SYSC(0x900)|(1<<2));}
#define SYSC_DisInvDBG()				{Outp32SYSC(0x900, Inp32SYSC(0x900)&~(1<<1));}
#define SYSC_EnInvDBG()				{Outp32SYSC(0x900, Inp32SYSC(0x900)|(1<<1));}
#define SYSC_EnCP15()				{Outp32SYSC(0x900, Inp32SYSC(0x900)&~(1<<0));}
#define SYSC_DisCP15()				{Outp32SYSC(0x900, Inp32SYSC(0x900)|(1<<0));}



void SYSC_ReadSystemID( void);
void SYSC_GetClkInform( void);
void SYSC_SetPLL(PLL_eTYPE  ePLL, u32 uMdiv, u32 uPdiv, u32 uSdiv, u32 uKdiv);
void SYSC_StopPLL(PLL_eTYPE  ePLL);
void SYSC_ChangeSYSCLK(APLL_eOUT  eAPLLMPS, APLL_eOUT eMPLLMPS, u32 uARM_Ratio, u32 uHCLKx2_Ratio, u32 uPCLK_Ratio);
void SYSC_ChangeSYSCLK_1(APLL_eOUT  eAPLLMPS, APLL_eOUT eMPLLMPS, u32 uARM_Ratio, u32 uHCLKx2_Ratio,u32 uPCLK_Ratio);
u8  SYSC_RdLockDetect(PLL_eTYPE  ePLL);
void SYSC_SetLockTime( PLL_eTYPE  ePLL, u32 uLockTime);
void SYSC_ClkSrc( CLKSRC_eId  eClkSrc);
void SYSC_SetDIV0( u32 uAratio, u32 uMratio, u32 uHratio, u32 uHx2ratio, u32 uPratio, 
	u32 uONDratio, u32 uSECUratio, u32 uCAMratio, u32 uJPEGratio, u32 uMFCratio );
	
void SYSC_SetDIV( CLKDIV_eId eClkDiv, u32 uDivVal );	
void SYSC_SetDIV1( u32 uMMC0ratio, u32 uMMC1ratio, u32 uMMC2ratio, u32 uLCDratio, u32 uSCALERratio, u32 uHOSTratio) ;
void SYSC_SetDIV0_all(u32 uSetRatio);
u32 SYSC_GetDIV0( void);
void SYSC_SetDIV2( u32 uSPI0ratio	, u32 uSPI1ratio, u32 uAUDIO0ratio, u32 uAUDIO1ratio, u32 uUARTratio, u32 uIRDAratio);
void SYSC_CtrlHCLKGate( H_eGATE eHCLKGATE	, u32 uCtrl);
void SYSC_CtrlPCLKGate( P_eGATE ePCLKGATE	, u32 uCtrl);
void SYSC_CtrlSCLKGate( S_eGATE eSCLKGATE	, u32 uCtrl);
void SYSC_SetCLKGateAll(u32 uOnOff);

void SYSC_CtrlCLKOUT( CLKOUT_eTYPE eCLKOUT_TYPE, u32 uDivVAL);
void SYSC_CtrlDCLK( u32 uDCLKCMP, u32 uDCLKDIV, u32 uDCLKSEL, u32 uDCLKEN);
void SYSC_CtrlBUSPrio( BUSCTRL_eId  eBusId);
void SYSC_SelectDMA( DMASELECT_eID eSEL_DMA, u32 uSEL);
void SYSC_CtrlEBIPrio( EBIPrio_eID  eEBIId);
void SYSC_CtrlCSMEM0( SelCS_eMEM0  eM0CSn2, SelCS_eMEM0 eM0CSn3, SelCS_eMEM0 eM0CSn4, SelCS_eMEM0 eM0CSn5 );
void SYSC_RdMEMCFGSTAT( void );
u32 SYSC_GetMEMCFGSTAT( void );
void SYSC_SetCFGWFI( CFG_eWFI eWFIMode, u32 uOSCCLK);
void SYSC_SetBATF( BATFLT_eMODE eBATFLT, u32 uINTSRC);
void SYSC_SetNORCFG( u32 uIROM, u32 uDM_ETM, u32 uDM_S, u32 uDM_F, u32 uDM_P, u32 uDM_I, u32 uDM_SS, u32 uDM_G, u32 uDM_V);
void SYSC_SetSTOPCFG( u32 uMEM_ARM, u32 uMEM_Top, u32 uLogic_ARM, u32 uLogic_Top);
void SYSC_SetSTOPMEMCFG( u32 uMODEM, u32 uHOST, u32 uOTG, u32 uHSMMC, u32 uIROM, u32 uIRDA, u32 uNFCON);
void SYSC_SetWakeMASK( WKUPMSK_eID eWKUPSRC, u32 uMaskCtrl);
void SYSC_CtrlNORCFG( NORGFC_eID eCFGId, u32 uPwrCtrl);
void SYSC_CtrlSTOPCFG( STOPCFG_eID eCFGId, u32 uPwrCtrlStop);
void SYSC_CtrlSTOPMEMCFG(STOPMEMCFG_eID eStopMemCfgId, u32 uPwrCtrlStopMem );
void SYSC_ChangeMode(SYSC_eOPMode eOPMode);
u32 SYSC_CheckOpMode(void);
void SYSC_ClrBATFINT(void);
u32 SYSC_RdRSTSTAT(u32 uPRINT);
void SYSC_RdBLKPWR(void);
void SYSC_WaitingForBLKPWR(BLKPWRSTA_eID eCFGId, u32 uOnOff);
void SYSC_WaitingForBLKPWRAll(u32 uOnOff);
void SYSC_ClrWKUPSTAT(void);

void SYSC_InitData_SDRAM(int check_start, int check_size);
void SYSC_CheckData_SDRAM(int check_start, int check_size);


void LTC3714_VoltageSet(u32 uPwr, u32 uVoltage)  ;

void __irq Isr_SYSC_RTC_Tick_Wu(void);
void SYSC_SetAlarmTick (void);


void __irq Isr_SYSC_AlmWu(void);
void SYSC_SetAlarmWakeup (void);
void __irq Isr_SYSC_WUEint9(void);
void __irq Isr_SYSC_WUEint10(void);
void __irq Isr_SYSC_WUEint11(void);
void SYSC_SetEintWakeup (void);
void __irq	Isr_SYSC_ADCTS_Wu(void);
void SYSC_SetADCTSWakeup (void);
void __irq Isr_SYSC_WUBATF(void);
void SYSC_SetBatteryFaultEvent(BATFLT_eMODE eBATFLT);

void SYSC_WDT_Reset(void);


void SYSC_IDLE (void);
void SYSC_STOP (void);
void SYSC_SLEEP (void);
void SYSC_DEEPSTOP (void);
void SYSC_STOP_Debugging (void);
void SYSC_IDLE_AGING(void);

void SYSC_STOP_Aging(void);
void SYSC_SLEEP0 (void);
void SYSC_STOP_Config(void);
void SYSC_DEEPSTOP_Config(void);
void SYSC_DEEPSTOP_Aging(void);

void SYSC_IDLE_Current(void);
void SYSC_STOP_Current(void);
void SYSC_STOP_Current2(void);
void SYSC_SLEEP_Current(void);
void SYSC_DEEPSTOP_Current(void);
void SYSC_SDHC_Clock_On(u32 uCh);








#ifdef __cplusplus
}
#endif

#endif //__SYSC_H__

