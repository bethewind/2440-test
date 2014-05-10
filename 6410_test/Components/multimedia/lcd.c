/********************************************************************************
* 
*	Project Name : S3C6410 Validation
*
*	Copyright 2006 by Samsung Electronics, Inc.
*	All rights reserved.
*
*	Project Description :
*		This software is only for verifying functions of the S3C6410.
*		Anybody can use this software without our permission.
*  
*--------------------------------------------------------------------------------
* 
*	File Name : lcd.c
*  
*	File Description :
*
*	Author	: Yoh-Han Lee
*	Dept. : AP Development Team
*	Created Date : 2006/12/27
*	Version : 0.2
* 
*	History
*	- Created (Y.H.Lee, 2006/12/27)
*   - Release Ver 0.1 (Y.H.Lee, 2007/03/29)
*   - Release Ver 0.2 (Y.H.Lee, 2007/06/07)
********************************************************************************/

#include "lcd.h"
#include "def.h"
#include "library.h"
#include "uart.h"
#include "gpio.h"
#include "sysc.h"
#include "Glib.h"
#include "Intc.h"
#include "spi.h"


#include <string.h>
#include <stdlib.h>

LCDC oLcdc; 
SPI_channel *ch1;

u32 uFrameCount;
// ITUIFCON0
#define EQUAL_HSYNC				(0<<24)
#define DELAYED_VSYNC			(1<<24)
#define CYCLE_DELAY_VSYNC(n)   (((n)&0xFF)<<16)
#define I656CLK_NORMAL			(0<<8)
#define I656CLK_INVERT			(1<<8)
#define I601HREF_NORMAL  		(0<<6)
#define I601HREF_INVERT  		(1<<6)
#define I601VSYNC_NORMAL  		(0<<5)
#define I601VSYNC_INVERT  		(1<<5)
#define I601HSYNC_NORMAL  		(0<<4)
#define I601HSYNC_INVERT  		(1<<4)
#define I601FIELD_NORMAL  		(0<<3)
#define I601FIELD_INVERT  		(1<<3)
#define I601CLK_NORMAL  		(0<<2)
#define I601CLK_INVERT  		(1<<2)
//////////////////////////////////////////////////////////////////////////////////////
// LCD API Functions
//////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
// Top Layer
///////////////////////////////////////////////////////////////////

#if 1 // LCD SPI Setting Test Jungil 
#define LCD_DELAY_1MS	18000
static void DelayLoop_1ms(int msec)
{
	volatile int j;
	for(j = 0; j < LCD_DELAY_1MS*msec; j++)  ;
}

typedef enum
{
	LTI_TYPE_INDEX,LTI_TYPE_INSTRUCTION, LTI_mPVA,
}LTI_TYPE;

void LTICommand(LTI_TYPE ltiType,u16 Data)
{
	if(ltiType==LTI_TYPE_INDEX)
	{
		SPIWrite(0x74, (u8)((Data>>8)&0xff), (u8)((Data)&0xff));
	}
	else if(ltiType==LTI_TYPE_INSTRUCTION)
	{
		SPIWrite(0x76, (u8)((Data>>8)&0xff), (u8)((Data)&0xff));
	}
	else if(ltiType==LTI_mPVA)
	{
		SPIWrite_2((u8)((Data>>8)&0xff), (u8)((Data)&0xff));
	}
}

const unsigned int LTV350QV_RGB_initialize[][3] =
{
	// Change as Module spec saids..
	///////////////////////////////////////////////////////////////////
	// Power On Reset State
	///////////////////////////////////////////////////////////////////

	// delay about 5ms

	{0x09, 0x0000, 10},

	// delay about 10ms

	///////////////////////////////////////////////////////////////////
	// Power Setting Function 1
	//////////////////////////////////////////////////////////////////

	{0x09, 0x4000, 0},
	{0x0a, 0x2000, 0},

	///////////////////////////////////////////////////////////////////
	// Power Setting Function 2
	//////////////////////////////////////////////////////////////////

	{0x09, 0x4055, 50},

	// delay about 50ms

	///////////////////////////////////////////////////////////////////
	// Instruction Setting
	///////////////////////////////////////////////////////////////////

	#if 0 
	{0x01, 0x409d, 0},		// Sync Mode
	{0x02, 0x0204, 0},		// Data Format RGB
	{0x03, 0x0100, 0},
	{0x04, 0x3000, 0},
	{0x05, 0x4003, 0},
	{0x06, 0x0009, 0},		//VBPD (7 < VBPD < 1024)
	{0x07, 0x000f, 0},		//HBPD (3 < HBPD < 512)
	{0x08, 0x0c00, 0},
	{0x10, 0x0103, 0},
	{0x11, 0x0301, 0},
	{0x12, 0x1f0f, 0},
	{0x13, 0x1f0f, 0},
	{0x14, 0x0707, 0},
	{0x15, 0x0307, 0},
	{0x16, 0x0707, 0},
	{0x17, 0x0000, 0},
	{0x18, 0x0004, 0},
	{0x19, 0x0000, 100},
	{0x0, 0x1, 0},
	#else
	{0x01, 0x409d, 0},		// Sync Mode
	{0x02, 0x0204, 0},		// Data Format RGB
	{0x03, 0x2100, 0}, 	// add.. 2100
	{0x04, 0x1000, 0},
	{0x05, 0x5003, 0},
	{0x06, 0x0009, 0},		//VBPD (7 < VBPD < 1024)
	{0x07, 0x000f, 0},		//HBPD (3 < HBPD < 512)
	{0x08, 0x0800, 0},
	{0x10, 0x0000, 0},
	{0x11, 0x0000, 0},
	{0x12, 0x000f, 0},
	{0x13, 0x1f00, 0},
	{0x14, 0x0000, 0},
	{0x15, 0x0000, 0},
	{0x16, 0x0000, 0},
	{0x17, 0x0000, 0},
	{0x18, 0x0000, 0},
	{0x19, 0x0000, 100},
	{0x0, 0x1, 0},
	#endif


	// delay about 2 frames
	// Delay about 50 ms
	//	Delay(500);

	// RGB I/F output start!!
	{0x0, 0x2, 0},
	//	LCD_Start();
	///////////////////////////////////////////////////////////////////
	// Display On Sequence
	///////////////////////////////////////////////////////////////////
	{0x09, 0x4a55, 0},
	{0x05, 0x5003, 0},

	{0, 0, 0}
};

const unsigned int LMS480QC0_RGB_initialize[][3] =
{

	{0x0063,0,0},	// default vlaue // set driver output 100% , hareware pin stanby
	{0x0155,0,0},	// default vlaue // interlaced display mode
	{0x0203,0,0},	// vertical display position setting 0x3(-13H) backporch period setting. default 0x10
	{0x0311,0,0},	// horizontal display position setting 0x11(-111NCLK) default 0x80
	{0x0400,0,0},	// default vlaue
	{0x0511,0,0},	// default vlaue
	{0x0600,0,0},	// signal polarity setting ODD/EVEN(high), CK(falling edge), HD(Low), VD(Low)
	{0x0700,0,0},	// default vlaue
	{0x0844,0,0},	// default vlaue
	{0x0902,0,0},	// BSEQ(0x2) ODD/Even Line 1,4,2,5,3,6 ?
	{0x0a2a,0,0},	// Bank Non Overlap time BNO[5:0]=0x2a(+10NCLK)
	{0x0b28,0,0},	// bank switch turn on time // default 0x40
	{0x0c2a,0,0},	// Gate non-overlap setting // default 0x40
	{0x0d3e,0,0},	// Gate source ouput delay setting // default 0x20
	{0x0e20,0,0},	// default vlaue
	{0x0f20,0,0},	// default vlaue
	{0x10f9,0,0},	// default 0xf0
	{0x11f4,0,0},	// default 0xf4
	{0x1210,0,0},	// default vlaue
	{0x1320,0,0},	// default vlaue
	{0x1420,0,0},	// default vlaue
	{0x1520,0,0},	// default vlaue
	{0x1680,0,0},	// default vlaue
	{0x1780,0,0},	// default vlaue
	{0x1880,0,0},	// default vlaue
	{0x2086,0,0},
	{0x2106,0,0},
	{0x220b,0,0},
	{0x2316,0,0},
	{0x2458,0,0},
	{0x2594,0,0},
	{0x26cf,0,0},
	{0x2755,0,0},
	{0x28bc,0,0},
	{0x2918,0,0},
	{0x2a69,0,0},
	{0x2bfe,0,0},
	{0x2c54,0,0},
	{0x2d95,0,0},
	{0x2efe,0,0},
	{0x5086,0,0},
	{0x5106,0,0},
	{0x520b,0,0},
	{0x5316,0,0},
	{0x5458,0,0},
	{0x5594,0,0},
	{0x56cf,0,0},
	{0x5755,0,0},
	{0x58bc,0,0},
	{0x5918,0,0},
	{0x5a69,0,0},
	{0x5bfe,0,0},
	{0x5c54,0,0},
	{0x5d95,0,0},
	{0x5efe,0,0},
	{0x2f21,0,0},
	{0,0,0}
};




#endif



void LCD_SimpleBasicDisp(void)
{
	u32 uHSz, uVSz;
	
	#if (LCD_MODULE_TYPE== LTV350QV_RGB)
		UART_Printf("Selected LCD Module Type: LTV350QV_RGB\n");
		uHSz=320;
		uVSz=240;
	
	#elif (LCD_MODULE_TYPE == LTS222QV_CPU)
		UART_Printf("Selected LCD Module Type: LTS222QV_CPU\n");
		uHSz=240;
		uVSz=320;

	#elif (LCD_MODULE_TYPE == LTP700WV_RGB)
		UART_Printf("Selected LCD Module Type: LTP700WV_RGB\n");
		uHSz=800;
		uVSz=480;	

	#elif (LCD_MODULE_TYPE == LTE480WV_RGB)
		UART_Printf("Selected LCD Module Type: LTE480WV_RGB\n");
		uHSz=800;
		uVSz=480;	
	
	#else
		Assert(0);
	#endif

	LCD_SetPort();
	
	LCD_InitLDI((CPUIF_LDI)LCD_CPUIF_CS);
	
	LCD_InitBase2((CLK_SRC)LCLK_DOUT_MPLL, PNR_Parallel_RGB, CPU_16BIT);
	LCD_InitWin(RGB16, uHSz, uVSz, 0, 0, uHSz, uVSz, 0, 0, LCD_FRAMEBUFFER_ST, WIN0, false);
	LCD_SetWinOnOff(1, WIN0);
	LCD_GetFrmSz(&uHSz, &uVSz, WIN0);
	GLIB_InitInstance(LCD_FRAMEBUFFER_ST, uHSz, uVSz, RGB16);
	GLIB_DrawPattern(uHSz, uVSz);

	LCD_Start();
	
	UART_Printf("\nIf you want to stop display, press any key\n");
	UART_Getc();
	
	LCD_SetAllWinOnOff(0);
	LCD_Stop();
	
}

//-----------------------------------------------------------
// Function Name :  LCD_SetPort
//
// Function Desctiption :
//
// Input :	
//
// Output :
//
// Version : 
//------------------------------------------------------------ 
void LCD_SetPort (void)
{

	///////////////////////////////////////////////////////////////////////////////
	(*(volatile unsigned *)0x7410800c)=0;	//Must be '0' for Normal-path instead of By-pass
	///////////////////////////////////////////////////////////////////////////////
	
	#if (LCD_MODULE_TYPE == LTV350QV_RGB)
		//Select LCD IF Pin Configure
		GPIO_SetLCDType(eRGBIF);
		//GPIO_SetLCDType(eHidden);	//To test Free-Run
		
		//Set LCD GPIO Port
		GPIO_SetFunctionAll(eGPIO_I, 0xaaaaaaaa, 2); //GPI[15..0]-> RGB VD[15..0]
		GPIO_SetFunctionAll(eGPIO_J, 0xaaaaaaaa, 2); //GPJ[7..0]-> RGB VD[23..16], GPJ[11..8]-> VCLK, VDEN, VSYNC, HSYNC

		//To test Free-Run
		//GPIO_SetFunctionEach(eGPIO_J, eGPIO_7, 0); //GPJ[7] -> VD[23]: Input
		//GPIO_SetPullUpDownEach(eGPIO_J, eGPIO_7, 0); //GPJ[7] -> Pull-down

		//Set PWM GPIO to control Back-light  Regulator  Shotdown Pin
		GPIO_SetFunctionEach(eGPIO_F, eGPIO_15, 1); //GPF[15] -> Output
		GPIO_SetDataEach(eGPIO_F, eGPIO_15 , 1); //GPF[15] -> High

		//Reset LDI
		GPIO_SetDataEach(eGPIO_N, eGPIO_5 , 1); //GPN[5] -> High
		GPIO_SetFunctionEach(eGPIO_N, eGPIO_5, 1); //GPN[5] -> Output
		Delay(1000);

		GPIO_SetDataEach(eGPIO_N, eGPIO_5 , 0); //GPN[5] -> Low 
		Delay(100);
		GPIO_SetDataEach(eGPIO_N, eGPIO_5 , 1); //GPN[5] -> High
		Delay(100);

		//Config SPI GPIO Port
#if 1 
		GPIO_SetFunctionEach(eGPIO_C, eGPIO_5, 1); //GPC[5]:SPI_SCLK -> Output
		GPIO_SetFunctionEach(eGPIO_C, eGPIO_6, 1); //GPC[6]:SPI_MOSI -> Output
		GPIO_SetFunctionEach(eGPIO_C, eGPIO_7, 1); //GPC[7]:SPI_CS -> Output
	
		GPIO_SetPullUpDownEach(eGPIO_C, eGPIO_5, 0); //GPC[5]:SPI_SCLK -> Pull-up/down Disable
		GPIO_SetPullUpDownEach(eGPIO_C, eGPIO_6, 0); //GPC[6]:SPI_MOSI -> Pull-up/down Disable
		GPIO_SetPullUpDownEach(eGPIO_C, eGPIO_7, 0); //GPC[7]:SPI_CS -> Pull-up/down Disable
		Delay(50); //Delay about 5ms
#else
		GPIO_SetFunctionEach(eGPIO_C, eGPIO_1, 1); //GPC[5]:SPI_SCLK -> Output
		GPIO_SetFunctionEach(eGPIO_C, eGPIO_2, 1); //GPC[6]:SPI_MOSI -> Output
		GPIO_SetFunctionEach(eGPIO_C, eGPIO_3, 1); //GPC[7]:SPI_CS -> Output
	
		GPIO_SetPullUpDownEach(eGPIO_C, eGPIO_1, 0); //GPC[5]:SPI_SCLK -> Pull-up/down Disable
		GPIO_SetPullUpDownEach(eGPIO_C, eGPIO_2, 0); //GPC[6]:SPI_MOSI -> Pull-up/down Disable
		GPIO_SetPullUpDownEach(eGPIO_C, eGPIO_3, 0); //GPC[7]:SPI_CS -> Pull-up/down Disable
		Delay(50); //Delay about 5ms
#endif

	#elif (LCD_MODULE_TYPE == LTS222QV_CPU)
		//Select LCD IF Pin Configure
		GPIO_SetLCDType(eHOSTIF);

		//Set LCD GPIO Port
		GPIO_SetFunctionAll(eGPIO_I, 0xaaaaaaaa, 2); //GPI[15..0]-> SYS VD[15..0]
		GPIO_SetFunctionAll(eGPIO_J, 0xaaaaaaaa, 2); 
		//GPJ[1..0]-> SYS VD[17..16], GPJ[11..6]-> SYS_VSYNC, SYS_OEn, SYS_CSn_main, SYS_CSn_sub, SYS_RS, SYS_WEn

		//Set PWM GPIO to control Back-light  Regulator En Pin
		GPIO_SetFunctionEach(eGPIO_F, eGPIO_15, 1); //GPF[15] -> Output
		GPIO_SetDataEach(eGPIO_F, eGPIO_15 , 1); //GPF[15] -> High

		//Reset LDI
		//GPIO_SetFunctionEach(eGPIO_N, eGPIO_5, 1); //GPN[5] -> Output
		//GPIO_SetDataEach(eGPIO_N, eGPIO_5 , 1); //GPN[5] -> High
		//Delay(1000);

		//Reset LDI
		GPIO_SetDataEach(eGPIO_N, eGPIO_5 , 1); //GPN[5] -> High
		GPIO_SetFunctionEach(eGPIO_N, eGPIO_5, 1); //GPN[5] -> Output
		Delay(1000);
		GPIO_SetDataEach(eGPIO_N, eGPIO_5 , 0); //GPN[5] -> Low
		Delay(100);
		GPIO_SetDataEach(eGPIO_N, eGPIO_5 , 1); //GPN[5] -> High
		Delay(100);

	#elif (LCD_MODULE_TYPE == LTP700WV_RGB ||LCD_MODULE_TYPE ==LTE480WV_RGB)
		//Select LCD IF Pin Configure
		GPIO_SetLCDType(eRGBIF);
		
		//Set LCD GPIO Port
		GPIO_SetFunctionAll(eGPIO_I, 0xaaaaaaaa, 2); //GPI[15..0]-> RGB VD[15..0]
		GPIO_SetFunctionAll(eGPIO_J, 0xaaaaaaaa, 2); //GPJ[7..0]-> RGB VD[23..16], GPJ[11..8]-> VCLK, VDEN, VSYNC, HSYNC

		//Set PWM GPIO to control Back-light  Regulator  Shotdown Pin
		GPIO_SetFunctionEach(eGPIO_F, eGPIO_15, 1); //GPF[15] -> Output
		GPIO_SetDataEach(eGPIO_F, eGPIO_15 , 1); //GPF[15] -> High

		//Reset LDI
		GPIO_SetDataEach(eGPIO_N, eGPIO_5 , 0); //GPN[5] -> Low 
		GPIO_SetFunctionEach(eGPIO_N, eGPIO_5, 1); //GPN[5] -> Output
		
		Delay(100);

	#elif (LCD_MODULE_TYPE == LTS222QV_SRGB)	
		//Select LCD IF Pin Configure
		GPIO_SetLCDType(eRGBIF);
		
		//Set LCD GPIO Port
		GPIO_SetFunctionAll(eGPIO_I, 0xaaaaaaaa, 2); //GPI[15..0]-> RGB VD[15..0]
		GPIO_SetFunctionAll(eGPIO_J, 0xaaaaaaaa, 2); //GPJ[7..0]-> RGB VD[23..16], GPJ[11..8]-> VCLK, VDEN, VSYNC, HSYNC

		//Set PWM GPIO to control Back-light  Regulator  Shotdown Pin
		GPIO_SetFunctionEach(eGPIO_F, eGPIO_15, 1); //GPF[15] -> Output
		GPIO_SetDataEach(eGPIO_F, eGPIO_15 , 1); //GPF[15] -> High

		//Reset LDI
		GPIO_SetDataEach(eGPIO_N, eGPIO_5 , 1); //GPN[5] -> High
		GPIO_SetFunctionEach(eGPIO_N, eGPIO_5, 1); //GPN[5] -> Output
		Delay(1000);

		GPIO_SetDataEach(eGPIO_N, eGPIO_5 , 0); //GPN[5] -> Low
		Delay(100);
		GPIO_SetDataEach(eGPIO_N, eGPIO_5 , 1); //GPN[5] -> High
		Delay(100);
#if 0  // SMDK REV0.2
		GPIO_SetFunctionEach(eGPIO_C, eGPIO_5, 1); //GPC[5]:SPI_SCLK -> Output
		GPIO_SetFunctionEach(eGPIO_C, eGPIO_6, 1); //GPC[6]:SPI_MOSI -> Output
		GPIO_SetFunctionEach(eGPIO_C, eGPIO_7, 1); //GPC[7]:SPI_CS -> Output
	
		GPIO_SetPullUpDownEach(eGPIO_C, eGPIO_5, 0); //GPC[5]:SPI_SCLK -> Pull-up/down Disable
		GPIO_SetPullUpDownEach(eGPIO_C, eGPIO_6, 0); //GPC[6]:SPI_MOSI -> Pull-up/down Disable
		GPIO_SetPullUpDownEach(eGPIO_C, eGPIO_7, 0); //GPC[7]:SPI_CS -> Pull-up/down Disable
		Delay(50); //Delay about 5ms
#else // SMRP REV0.0
		GPIO_SetFunctionEach(eGPIO_C, eGPIO_1, 1); //GPC[5]:SPI_SCLK -> Output
		GPIO_SetFunctionEach(eGPIO_C, eGPIO_2, 1); //GPC[6]:SPI_MOSI -> Output
		GPIO_SetFunctionEach(eGPIO_C, eGPIO_3, 1); //GPC[7]:SPI_CS -> Output
	
		GPIO_SetPullUpDownEach(eGPIO_C, eGPIO_1, 0); //GPC[5]:SPI_SCLK -> Pull-up/down Disable
		GPIO_SetPullUpDownEach(eGPIO_C, eGPIO_2, 0); //GPC[6]:SPI_MOSI -> Pull-up/down Disable
		GPIO_SetPullUpDownEach(eGPIO_C, eGPIO_3, 0); //GPC[7]:SPI_CS -> Pull-up/down Disable
		Delay(50); //Delay about 5ms
#endif


#elif (LCD_MODULE_TYPE == LTS222QV_PRGB)	
		//Select LCD IF Pin Configure
		GPIO_SetLCDType(eRGBIF);
		
		//Set LCD GPIO Port
		GPIO_SetFunctionAll(eGPIO_I, 0xaaaaaaaa, 2); //GPI[15..0]-> RGB VD[15..0]
		GPIO_SetFunctionAll(eGPIO_J, 0xaaaaaaaa, 2); //GPJ[7..0]-> RGB VD[23..16], GPJ[11..8]-> VCLK, VDEN, VSYNC, HSYNC

		//Set PWM GPIO to control Back-light  Regulator  Shotdown Pin
		GPIO_SetFunctionEach(eGPIO_F, eGPIO_15, 1); //GPF[15] -> Output
		GPIO_SetDataEach(eGPIO_F, eGPIO_15 , 1); //GPF[15] -> High

		//Reset LDI
		GPIO_SetDataEach(eGPIO_N, eGPIO_5 , 1); //GPN[5] -> High
		GPIO_SetFunctionEach(eGPIO_N, eGPIO_5, 1); //GPN[5] -> Output
		Delay(1000);

		GPIO_SetDataEach(eGPIO_N, eGPIO_5 , 0); //GPN[5] -> Low
		Delay(100);
		GPIO_SetDataEach(eGPIO_N, eGPIO_5 , 1); //GPN[5] -> High
		Delay(100);

#if 0  // SMDK REV0.2
		GPIO_SetFunctionEach(eGPIO_C, eGPIO_5, 1); //GPC[5]:SPI_SCLK -> Output
		GPIO_SetFunctionEach(eGPIO_C, eGPIO_6, 1); //GPC[6]:SPI_MOSI -> Output
		GPIO_SetFunctionEach(eGPIO_C, eGPIO_7, 1); //GPC[7]:SPI_CS -> Output
	
		GPIO_SetPullUpDownEach(eGPIO_C, eGPIO_5, 0); //GPC[5]:SPI_SCLK -> Pull-up/down Disable
		GPIO_SetPullUpDownEach(eGPIO_C, eGPIO_6, 0); //GPC[6]:SPI_MOSI -> Pull-up/down Disable
		GPIO_SetPullUpDownEach(eGPIO_C, eGPIO_7, 0); //GPC[7]:SPI_CS -> Pull-up/down Disable
		Delay(50); //Delay about 5ms
#else // SMRP REV0.0
		GPIO_SetFunctionEach(eGPIO_C, eGPIO_1, 1); //GPC[5]:SPI_SCLK -> Output
		GPIO_SetFunctionEach(eGPIO_C, eGPIO_2, 1); //GPC[6]:SPI_MOSI -> Output
		GPIO_SetFunctionEach(eGPIO_C, eGPIO_3, 1); //GPC[7]:SPI_CS -> Output
	
		GPIO_SetPullUpDownEach(eGPIO_C, eGPIO_1, 0); //GPC[5]:SPI_SCLK -> Pull-up/down Disable
		GPIO_SetPullUpDownEach(eGPIO_C, eGPIO_2, 0); //GPC[6]:SPI_MOSI -> Pull-up/down Disable
		GPIO_SetPullUpDownEach(eGPIO_C, eGPIO_3, 0); //GPC[7]:SPI_CS -> Pull-up/down Disable
		Delay(50); //Delay about 5ms
#endif

		

	#elif (LCD_MODULE_TYPE == LTV300GV_RGB)
		//Select LCD IF Pin Configure
		GPIO_SetLCDType(eRGBIF);
		
		//Set LCD GPIO Port
		GPIO_SetFunctionAll(eGPIO_I, 0xaaaaaaaa, 2); //GPI[15..0]-> RGB VD[15..0]
		GPIO_SetFunctionAll(eGPIO_J, 0xaaaaaaaa, 2); //GPJ[7..0]-> RGB VD[23..16], GPJ[11..8]-> VCLK, VDEN, VSYNC, HSYNC

		// GPIO Control sequence is so sensitive. Follow the below sequence.
		//Reset low    LDI
		GPIO_SetDataEach(eGPIO_N, eGPIO_5 , 0); //GPN[5] -> Low 
		GPIO_SetFunctionEach(eGPIO_N, eGPIO_5, 1); //GPN[5] -> Output		

		//Pannel On
		//GPIO_SetFunctionEach(eGPIO_F, eGPIO_13, 1); //GPF[15] -> Output
		//GPIO_SetDataEach(eGPIO_F, eGPIO_13 , 1); //GPF[15] -> High
		Delay(1000);
		
		//Reset High
		GPIO_SetDataEach(eGPIO_N, eGPIO_5 , 1); //GPN[5] -> High
		Delay(1000);

		//BackLight On
		GPIO_SetFunctionEach(eGPIO_F, eGPIO_15, 1); //GPF[15] -> Output
		GPIO_SetDataEach(eGPIO_F, eGPIO_15 , 1); //GPF[15] -> High	

	#elif (LCD_MODULE_TYPE == LMS480KC01_RGB) // add. jungil 
		//Select LCD IF Pin Configure
		GPIO_SetLCDType(eRGBIF);
		
		//Set LCD GPIO Port
		GPIO_SetFunctionAll(eGPIO_I, 0xaaaaaaaa, 2); //GPI[15..0]-> RGB VD[15..0]
		GPIO_SetFunctionAll(eGPIO_J, 0xaaaaaaaa, 2); //GPJ[7..0]-> RGB VD[23..16], GPJ[11..8]-> VCLK, VDEN, VSYNC, HSYNC

		//Set PWM GPIO to control Back-light  Regulator  Shotdown Pin
		GPIO_SetFunctionEach(eGPIO_F, eGPIO_15, 1); //GPF[15] -> Output
		GPIO_SetDataEach(eGPIO_F, eGPIO_15 , 1); //GPF[15] -> High

		//Reset LDI
		GPIO_SetFunctionEach(eGPIO_N, eGPIO_5, 1); //GPN[5] -> Output
		GPIO_SetDataEach(eGPIO_N, eGPIO_5 , 0); //GPN[5] -> Low 		
		GPIO_SetDataEach(eGPIO_N, eGPIO_5 , 1); //GPN[5] -> High
		Delay(100);

	#else
		Assert(0);
	#endif

}


void LCD_SetPort_ByPass(void)
{
	u32 i, j;
	
	(*(volatile unsigned *)0x7410800c)=0x8;	//Must be '1' for By-pass

	UART_Printf("\nSelect LCD By-Pass Mode.\n");
	UART_Printf("0: Chip Select Mode, 1: Address Decoding Mode\n");

	i = UART_GetIntNum();	

	if(i==0)
	{
		UART_Printf("\nSelect nCS Type.\n");
		UART_Printf("0: Main_nCS, 1: Sub_nCS\n");
		
		j = UART_GetIntNum();
	
		GPIO_SetFunctionAll(eGPIO_K, 0x22222222, 0x22222222);	//GPK[15:0] -> Host I/F Data[15:0]
		GPIO_SetFunctionAll(eGPIO_L, 0x0, 0x2200002);			//GPL[15:13] -> Host I/F Data[17:16]

		if (j==0)
		{
			UART_Printf("Chip Select Mode: Main\n");
			GPIO_SetFunctionAll(eGPIO_M, 0x022020, 0x0); 
		}
		else
		{
			UART_Printf("Chip Select Mode: Sub\n");
			GPIO_SetFunctionAll(eGPIO_M, 0x022200, 0x0); 
		}

		GPIO_SetConRegSleepAll(eGPIO_F, 0xffffffff);		
	}
	else
	{
		GPIO_SetFunctionAll(eGPIO_K, 0x22222222, 0x22222222);	//GPK[15:0] -> Host I/F Data[15:0]
		GPIO_SetFunctionAll(eGPIO_L, 0x2222222, 0x2222222);		//GPL[15:13] -> Host I/F Data[17:16]
		GPIO_SetFunctionAll(eGPIO_M, 0x022002, 0x0);

		GPIO_SetConRegSleepAll(eGPIO_F, 0xffffffff);
	}

	UART_Printf("\nEnter Any Key To Exit. \n");
	UART_Getc();
	
}


//-----------------------------------------------------------
// Function Name :  LCD_InitLDI
//
// Function Desctiption :
//
// Input :	
//
// Output :
//
// Version : 
//------------------------------------------------------------ 
void LCD_InitLDI(CPUIF_LDI eLdiMode)
{
	
	#if (LCD_MODULE_TYPE== LTV350QV_RGB)
		LCD_InitLTV350QV_RGB();
	
	#elif (LCD_MODULE_TYPE == LTS222QV_CPU)
		LCD_SetI80Timing(CS_SETUP_TIME,WR_SETUP_TIME, WR_ACT_TIME, WR_HOLD_TIME, LCD_CPUIF_CS);		
		//LCD_SetAutoCmdRate(PER_TWO_FRM, LCD_CPUIF_CS);
		LCD_InitLTS222QV_CPU(eLdiMode);
		LCD_InitStartPosOnLcd(eLdiMode);
		//LCD_SetAutoCmdRate(DISABLE_AUTO_FRM, LCD_CPUIF_CS);

	#elif (LCD_MODULE_TYPE== LTP700WV_RGB ||LCD_MODULE_TYPE ==LTE480WV_RGB)
		LCD_InitLTP700WV_RGB();

	#elif (LCD_MODULE_TYPE== LMS480KC01_RGB)
		LCD_InitLMS480KCO_RGB();
	
	#elif (LCD_MODULE_TYPE== LTS222QV_SRGB)
		LCD_InitLTS222QV_SRGB();
		LCD_InitStartPosOnLcd(LCD_CPUIF_CS);
	

	#elif (LCD_MODULE_TYPE== LTS222QV_PRGB)
		LCD_InitLTS222QV_PRGB();
		LCD_InitStartPosOnLcd(LCD_CPUIF_CS);


	#elif (LCD_MODULE_TYPE== LTV300GV_RGB)	
		//Need not be initialized
	#else
		Assert(0);
	
	#endif
	
}

void LCD_InitDISPC(CSPACE eBpp, u32 uFbAddr, LCD_WINDOW eWin, u8 bIsDoubleBuf)
{
	LCD_InitBase();
	LCD_InitWin(eBpp, oLcdc.m_uLcdHSz, oLcdc.m_uLcdVSz, 0, 0, oLcdc.m_uLcdHSz, oLcdc.m_uLcdVSz,
		0, 0, uFbAddr, eWin, bIsDoubleBuf);
}

void LCD_InitDISPC1(u32 uHSize, u32 uVSize, CSPACE eBpp, u32 uFbAddr, LCD_WINDOW eWin, u8 bIsDoubleBuf)
{
	LCD_InitBase1(uHSize, uVSize);
	LCD_InitWin(eBpp, oLcdc.m_uLcdHSz, oLcdc.m_uLcdVSz, 0, 0, oLcdc.m_uLcdHSz, oLcdc.m_uLcdVSz,
		0, 0, uFbAddr, eWin, bIsDoubleBuf);
}

//------------------------------------------------------------
// Function Name : LCDC_GetFrmSz
//
// Function Desctiption :
//  This function turns lcd on
//
// Input :
//
// Output : 	
//
// Version : 
//------------------------------------------------------------
void LCD_GetFrmSz(u32* uFrmHSz, u32* uFrmVSz, LCD_WINDOW eWin)
{
	*uFrmHSz = oLcdc.m_uFrmHSz[eWin];
	*uFrmVSz = oLcdc.m_uFrmVSz[eWin];
}

//------------------------------------------------------------
// Function Name : LCD_Start
//
// Function Desctiption :
//  This function turns lcd on
//
// Input :
//
// Output :	
//
// Version : 
//------------------------------------------------------------
void LCD_Start(void)
{
	u32 uLcdCon;

	uLcdCon=Inp32(LCD_BASE+rVIDCON0);
	uLcdCon |= 0x3<<0;
	LcdcOutp32(rVIDCON0, uLcdCon);
}


//-----------------------------------------------------------
// Function Name : LCD_Stop
//
// Function Desctiption :
//  This function turns lcd off
//
// Input :
//
// Output :	
//
// Version : 
//-----------------------------------------------------------
void LCD_Stop(void)
{
	u32 uLcdCon;
		
	uLcdCon=Inp32(LCD_BASE+rVIDCON0);

	uLcdCon &= ~(0x3<<0);
	LcdcOutp32(rVIDCON0, uLcdCon);	
	
} 

void LCD_PerFrameOff(void)
{
	u32 uLcdCon;
		
	uLcdCon=Inp32(LCD_BASE+rVIDCON0);

	uLcdCon &= ~(0x1<<0);
	LcdcOutp32(rVIDCON0, uLcdCon);	
}


///////////////////////////////////////////////////////////////////
// Middle Layer
///////////////////////////////////////////////////////////////////

//------------------------------------------------------------
// Function Name : LCD_InitBase
//
// Function Desctiption :
//  This function sets essential values according to LCD Type
//
// Input : g_eSysLcd, global variable for lcd type
//
// Output : oLcdc, global struct for lcd
//
// Version : 
//------------------------------------------------------------
void LCD_InitBase(void)
{
	u32 uLcdCon;
	u32 uILcdIntCon;
	u32 uClkVal, uClkDir;
	u32 uVidconReg;
	u32 i;
	LCD_WINDOW eWin;
	CLK_SRC eVClkSrc;

	
	oLcdc.m_bIsAutoBuf = false;
	oLcdc.m_uScanMode = 0; // progressive mode
	oLcdc.m_bIsLocalCalled[0] = false;
	oLcdc.m_bIsLocalCalled[1] = false;
	oLcdc.m_bIsLocalCalled[2] = false;

	#if (LCD_CLOCK_SOURCE == LCLK_HCLK)
		UART_Printf("LCD Clock Source: HCLK\n");
		oLcdc.m_uVideoClockSource = CLKSEL_F_HCLK;	
		eVClkSrc = SRC_HCLK;

	#elif (LCD_CLOCK_SOURCE == LCLK_MOUT_EPLL)
		UART_Printf("LCD Clock Source: EPLL OUT\n");
		oLcdc.m_uVideoClockSource = CLKSEL_F_SYSCON; 
		eVClkSrc = SRC_MOUT_EPLL;

	#elif (LCD_CLOCK_SOURCE == LCLK_DOUT_MPLL)
		UART_Printf("LCD Clock Source: MPLL OUT\n");
		oLcdc.m_uVideoClockSource = CLKSEL_F_SYSCON; 
		eVClkSrc = SRC_DOUT_MPLL;

	#elif (LCD_CLOCK_SOURCE == LCLK_FIN_EPLL)
		UART_Printf("LCD Clock Source: FIN\n");
		oLcdc.m_uVideoClockSource = CLKSEL_F_SYSCON; 
		eVClkSrc = SRC_FIN_EPLL;	

	#elif (LCD_CLOCK_SOURCE == LCLK_27M)
		UART_Printf("LCD Clock Source: 27M\n");
		oLcdc.m_uVideoClockSource = CLKSEL_F_EXTCLK;
		eVClkSrc = SRC_27M;
	#endif
	
	#if (LCD_MODULE_TYPE == LTV350QV_RGB)
	
		oLcdc.m_uLcdHSz = 320;
		oLcdc.m_uLcdVSz = 240;
		oLcdc.m_uVidOutFormat = VIDOUT_RGBIF;
		oLcdc.m_uDitherMode = RDITHPOS_8BIT|GDITHPOS_8BIT|BDITHPOS_8BIT;
		oLcdc.m_uDitherMode &= ~DITHERING_ENABLE;

		LcdcOutp32(rVIDCON1, IHSYNC_INVERT | IVSYNC_INVERT |IVDEN_INVERT); 	//Check 
		
		LcdcOutp32(rVIDTCON0, VBPDE(VBPD_LTV350QV) | VBPD(VBPD_LTV350QV) | VFPD(VFPD_LTV350QV) | VSPW(VSPW_LTV350QV));
		LcdcOutp32(rVIDTCON1, VFPDE(VFPD_LTV350QV) | HBPD(HBPD_LTV350QV) | HFPD(HFPD_LTV350QV) | HSPW(HSPW_LTV350QV));
		LcdcOutp32(rVIDTCON2, LINEVAL(oLcdc.m_uLcdVSz-1) | HOZVAL(oLcdc.m_uLcdHSz-1));
		LcdcOutp32(rDITHMODE, oLcdc.m_uDitherMode); // Fixed Dithering Matrix

	#elif (LCD_MODULE_TYPE == LTS222QV_CPU)
	
		oLcdc.m_uLcdHSz = 240;
		oLcdc.m_uLcdVSz = 320;
		oLcdc.m_uVidOutFormat = 
						(LCD_CPUIF_CS == MAIN) ? VIDOUT_I80IF0: VIDOUT_I80IF1;
			
		LcdcOutp32(rVIDTCON2, LINEVAL(oLcdc.m_uLcdVSz-1) | HOZVAL(oLcdc.m_uLcdHSz-1));

	#elif (LCD_MODULE_TYPE == LTP700WV_RGB ||LCD_MODULE_TYPE ==LTE480WV_RGB)
	
		oLcdc.m_uLcdHSz = 800;
		oLcdc.m_uLcdVSz = 480;
		oLcdc.m_uVidOutFormat = VIDOUT_RGBIF;
		oLcdc.m_uDitherMode = RDITHPOS_8BIT|GDITHPOS_8BIT|BDITHPOS_8BIT;
		oLcdc.m_uDitherMode &= ~DITHERING_ENABLE;

		LcdcOutp32(rVIDCON1, IHSYNC_INVERT | IVSYNC_INVERT |IVDEN_NORMAL);
		LcdcOutp32(rVIDTCON0, VBPDE(VBPD_LTP700WV) | VBPD(VBPD_LTP700WV) | VFPD(VFPD_LTP700WV) | VSPW(VSPW_LTP700WV));
		LcdcOutp32(rVIDTCON1, VFPDE(VFPD_LTP700WV) | HBPD(HBPD_LTP700WV) | HFPD(HFPD_LTP700WV) | HSPW(HSPW_LTP700WV));
		LcdcOutp32(rVIDTCON2, LINEVAL(oLcdc.m_uLcdVSz-1) | HOZVAL(oLcdc.m_uLcdHSz-1));
		LcdcOutp32(rDITHMODE, oLcdc.m_uDitherMode); // Fixed Dithering Matrix	

	#elif (LCD_MODULE_TYPE == LMS480KC01_RGB)

		oLcdc.m_uLcdHSz = 800;
		oLcdc.m_uLcdVSz = 480;
		oLcdc.m_uVidOutFormat = VIDOUT_RGBIF;
		oLcdc.m_uDitherMode = RDITHPOS_8BIT|GDITHPOS_8BIT|BDITHPOS_8BIT;
		oLcdc.m_uDitherMode &= ~DITHERING_ENABLE;

		LcdcOutp32(rVIDCON1, IHSYNC_NORMAL | IVSYNC_NORMAL  | IVDEN_NORMAL |IVCLK_FALL_EDGE); //Check 
		LcdcOutp32(rVIDTCON0, VBPDE(VBPD_LMS480CQ) | VBPD(VBPD_LMS480CQ) | VFPD(VFPD_LMS480CQ) | VSPW(VSPW_LMS480CQ));
		LcdcOutp32(rVIDTCON1, VFPDE(VFPD_LMS480CQ) | HBPD(HBPD_LMS480CQ) | HFPD(HFPD_LMS480CQ) | HSPW(HSPW_LMS480CQ));
		LcdcOutp32(rVIDTCON2, LINEVAL(oLcdc.m_uLcdVSz-1) | HOZVAL(oLcdc.m_uLcdHSz-1));
		LcdcOutp32(rDITHMODE, oLcdc.m_uDitherMode); 	//Fixed Dithering Matrix
		
	#elif (LCD_MODULE_TYPE == LTS222QV_SRGB)
	
		oLcdc.m_uLcdHSz = 240;
		oLcdc.m_uLcdVSz = 320;
		oLcdc.m_uVidOutFormat = VIDOUT_RGBIF;
		oLcdc.m_uDitherMode = RDITHPOS_5BIT|GDITHPOS_6BIT|BDITHPOS_5BIT;
		oLcdc.m_uDitherMode &= ~DITHERING_ENABLE;

		LcdcOutp32(rVIDCON1, IVCLK_FALL_EDGE | IHSYNC_INVERT | IVSYNC_INVERT | IVDEN_NORMAL);
		LcdcOutp32(rVIDTCON0, VBPDE(0) | VBPD(VBPD_LTS222QV) | VFPD(VFPD_LTS222QV) | VSPW(VSPW_LTS222QV));
		LcdcOutp32(rVIDTCON1, VFPDE(0) | HBPD(HBPD_LTS222QV) | HFPD(HFPD_LTS222QV) | HSPW(HSPW_LTS222QV));
		LcdcOutp32(rVIDTCON2, LINEVAL(oLcdc.m_uLcdVSz-1) | HOZVAL(oLcdc.m_uLcdHSz-1));
		LcdcOutp32(rDITHMODE, oLcdc.m_uDitherMode); // Fixed Dithering Matrix

#elif (LCD_MODULE_TYPE == LTS222QV_PRGB)
	
		oLcdc.m_uLcdHSz = 240;
		oLcdc.m_uLcdVSz = 320;
		oLcdc.m_uVidOutFormat = VIDOUT_RGBIF;
		oLcdc.m_uDitherMode = RDITHPOS_5BIT|GDITHPOS_6BIT|BDITHPOS_5BIT;
		oLcdc.m_uDitherMode &= ~DITHERING_ENABLE;

		LcdcOutp32(rVIDCON1, IVCLK_FALL_EDGE | IHSYNC_INVERT | IVSYNC_INVERT| IVDEN_NORMAL);
		//LcdcOutp32(rVIDCON1, IVCLK_RISE_EDGE | IHSYNC_INVERT | IVSYNC_INVERT| IVDEN_NORMAL);
		LcdcOutp32(rVIDTCON0, VBPDE(0) | VBPD(VBPD_LTS222QVP) | VFPD(VFPD_LTS222QVP) | VSPW(VSPW_LTS222QVP));
		LcdcOutp32(rVIDTCON1, VFPDE(0) | HBPD(HBPD_LTS222QVP) | HFPD(HFPD_LTS222QVP) | HSPW(HSPW_LTS222QVP));
		LcdcOutp32(rVIDTCON2, LINEVAL(oLcdc.m_uLcdVSz-1) | HOZVAL(oLcdc.m_uLcdHSz-1));
		LcdcOutp32(rDITHMODE, oLcdc.m_uDitherMode); // Fixed Dithering Matrix


	#elif (LCD_MODULE_TYPE == LTV300GV_RGB)
		
		oLcdc.m_uLcdHSz = 640;
		oLcdc.m_uLcdVSz = 480;
		oLcdc.m_uVidOutFormat = VIDOUT_RGBIF;
		oLcdc.m_uDitherMode = RDITHPOS_5BIT|GDITHPOS_6BIT|BDITHPOS_5BIT;
		oLcdc.m_uDitherMode &= ~DITHERING_ENABLE;
	
		LcdcOutp32(rVIDCON1, IHSYNC_INVERT | IVSYNC_INVERT |IVDEN_NORMAL);
		//LcdcOutp32(rVIDCON1, IHSYNC_NORMAL | IVSYNC_NORMAL |IVDEN_NORMAL);
		LcdcOutp32(rVIDTCON0, VBPDE(VBPD_LTV300GV) | VBPD(VBPD_LTV300GV) | VFPD(VFPD_LTV300GV) | VSPW(VSPW_LTV300GV));
		LcdcOutp32(rVIDTCON1, VFPDE(VFPD_LTV300GV) | HBPD(HBPD_LTV300GV) | HFPD(HFPD_LTV300GV) | HSPW(HSPW_LTV300GV));		
		LcdcOutp32(rVIDTCON2, LINEVAL(oLcdc.m_uLcdVSz-1) | HOZVAL(oLcdc.m_uLcdHSz-1));
		LcdcOutp32(rDITHMODE, oLcdc.m_uDitherMode); // Fixed Dithering Matrix	
		
	#else
		Assert(0); // Not yet implemented.
	#endif

	LCD_Stop();
	
	//Check up LCD to turn off
	
	while (1)
	{
		uLcdCon=Inp32(LCD_BASE+rVIDCON0);
		if( (uLcdCon&0x03) == 0 ) // checking whether disable the video output and the Display control signal or not.
		break;
	}

	LCD_GetClkValAndClkDir(oLcdc.m_uLcdHSz, oLcdc.m_uLcdVSz, &uClkVal, &uClkDir, eVClkSrc);

	uVidconReg =
		PROGRESSIVE | oLcdc.m_uVidOutFormat | SUB_16_MODE | MAIN_16_MODE | PNRMODE_RGB_P | CLKVALUP_ALWAYS |
		CLKVAL_F(uClkVal) | VCLKEN_DISABLE | CLKDIR_F(uClkDir) | oLcdc.m_uVideoClockSource|
		ENVID_DISABLE | ENVID_F_DISABLE;

	LcdcOutp32(rVIDCON0, uVidconReg);

	uILcdIntCon =Inp32(LCD_BASE+rVIDINTCON0);
	LcdcOutp32(rVIDINTCON0, uILcdIntCon | FRAMESEL0_BACK | FRAMESEL1_NONE | INTFRMEN_DISABLE |
				FIFOSEL_WIN0 | FIFOLEVEL_25 | INTFIFOEN_DISABLE | INTEN_DISABLE);

	LCD_SetAllWinOnOff(0); // Turn all windows off
	LCD_SetAllWinColorMapOnOff(0); // Turn all windows color map off
	LCD_SetAllWinColorKeyOnOff(0); // Turn all windows Color Key off

	for (i=1; i<5; i++)
	{
		eWin =
			(i == 1) ? WIN1 :
			(i == 2) ? WIN2 :
			(i == 3) ? WIN3 : WIN4;

		LCD_SetAlpha(0x0, 0x0, 0x0, 0x0, 0x0, 0x0, eWin);
	}

}

void LCD_InitBase1(u32 uLcdX, u32 uLcdY)
{
	u32 uLcdCon;
	u32 uILcdIntCon; 
	u32 uClkVal, uClkDir;
	u32 uVidconReg;
	u32 i;
	LCD_WINDOW eWin;
	CLK_SRC eVClkSrc;
	
	
	oLcdc.m_bIsAutoBuf = false;
	oLcdc.m_uScanMode = 0; // progressive mode
	oLcdc.m_bIsLocalCalled[0] = false;
	oLcdc.m_bIsLocalCalled[1] = false;
	oLcdc.m_bIsLocalCalled[2] = false;

	#if (LCD_CLOCK_SOURCE == LCLK_HCLK)
		oLcdc.m_uVideoClockSource = CLKSEL_F_HCLK;	
		eVClkSrc = SRC_HCLK;

	#elif (LCD_CLOCK_SOURCE == LCLK_MOUT_EPLL)
		oLcdc.m_uVideoClockSource = CLKSEL_F_SYSCON; 
		eVClkSrc = SRC_MOUT_EPLL;

	#elif (LCD_CLOCK_SOURCE == LCLK_DOUT_MPLL)
		oLcdc.m_uVideoClockSource = CLKSEL_F_SYSCON; 
		eVClkSrc = SRC_DOUT_MPLL;

	#elif (LCD_CLOCK_SOURCE == LCLK_FIN_EPLL)
		oLcdc.m_uVideoClockSource = CLKSEL_F_SYSCON; 
		eVClkSrc = SRC_FIN_EPLL;	

	#elif (LCD_CLOCK_SOURCE == LCLK_27M)
		oLcdc.m_uVideoClockSource = CLKSEL_F_EXTCLK;
		eVClkSrc = SRC_27M;
	#endif
	
	#if (LCD_MODULE_TYPE == LTV350QV_RGB)
	
		oLcdc.m_uLcdHSz = uLcdX;
		oLcdc.m_uLcdVSz = uLcdY;
		oLcdc.m_uVidOutFormat = VIDOUT_RGBIF;
		oLcdc.m_uDitherMode = RDITHPOS_8BIT|GDITHPOS_8BIT|BDITHPOS_8BIT;
		oLcdc.m_uDitherMode &= ~DITHERING_ENABLE;

		LcdcOutp32(rVIDCON1, IHSYNC_INVERT | IVSYNC_INVERT ); //Check 
		LcdcOutp32(rVIDTCON0, VBPDE(VBPD_LTV350QV) | VBPD(VBPD_LTV350QV) | VFPD(VFPD_LTV350QV) | VSPW(VSPW_LTV350QV));
		LcdcOutp32(rVIDTCON1, VFPDE(VFPD_LTV350QV) | HBPD(HBPD_LTV350QV) | HFPD(HFPD_LTV350QV) | HSPW(HSPW_LTV350QV));
		LcdcOutp32(rVIDTCON2, LINEVAL(oLcdc.m_uLcdVSz-1) | HOZVAL(oLcdc.m_uLcdHSz-1));
		LcdcOutp32(rDITHMODE, oLcdc.m_uDitherMode); // Fixed Dithering Matrix

	#elif (LCD_MODULE_TYPE == LTS222QV_CPU)
	
		oLcdc.m_uLcdHSz = uLcdX;
		oLcdc.m_uLcdVSz = uLcdY;
		oLcdc.m_uVidOutFormat = 
						(LCD_CPUIF_CS == MAIN) ? VIDOUT_I80IF0: VIDOUT_I80IF1;
			
		LcdcOutp32(rVIDTCON2, LINEVAL(oLcdc.m_uLcdVSz-1) | HOZVAL(oLcdc.m_uLcdHSz-1));

	#elif (LCD_MODULE_TYPE == LTP700WV_RGB ||LCD_MODULE_TYPE ==LTE480WV_RGB)
	
		oLcdc.m_uLcdHSz = uLcdX;
		oLcdc.m_uLcdVSz = uLcdY;
		oLcdc.m_uVidOutFormat = VIDOUT_RGBIF;
		oLcdc.m_uDitherMode = RDITHPOS_8BIT|GDITHPOS_8BIT|BDITHPOS_8BIT;
		oLcdc.m_uDitherMode &= ~DITHERING_ENABLE;

		LcdcOutp32(rVIDCON1, IHSYNC_INVERT | IVSYNC_INVERT |IVDEN_NORMAL);
		LcdcOutp32(rVIDTCON0, VBPDE(VBPD_LTP700WV) | VBPD(VBPD_LTP700WV) | VFPD(VFPD_LTP700WV) | VSPW(VSPW_LTP700WV));
		LcdcOutp32(rVIDTCON1, VFPDE(VFPD_LTP700WV) | HBPD(HBPD_LTP700WV) | HFPD(HFPD_LTP700WV) | HSPW(HSPW_LTP700WV));
		LcdcOutp32(rVIDTCON2, LINEVAL(oLcdc.m_uLcdVSz-1) | HOZVAL(oLcdc.m_uLcdHSz-1));
		LcdcOutp32(rDITHMODE, oLcdc.m_uDitherMode); // Fixed Dithering Matrix	

	#elif (LCD_MODULE_TYPE == LTS222QV_SRGB)
	
		oLcdc.m_uLcdHSz = 240;
		oLcdc.m_uLcdVSz = 320;
		oLcdc.m_uVidOutFormat = VIDOUT_RGBIF;
		oLcdc.m_uDitherMode = RDITHPOS_5BIT|GDITHPOS_6BIT|BDITHPOS_5BIT;
		oLcdc.m_uDitherMode &= ~DITHERING_ENABLE;

		LcdcOutp32(rVIDCON1, IVCLK_FALL_EDGE | IHSYNC_INVERT | IVSYNC_INVERT | IVDEN_NORMAL);
		LcdcOutp32(rVIDTCON0, VBPDE(0) | VBPD(VBPD_LTS222QV) | VFPD(VFPD_LTS222QV) | VSPW(VSPW_LTS222QV));
		LcdcOutp32(rVIDTCON1, VFPDE(0) | HBPD(HBPD_LTS222QV) | HFPD(HFPD_LTS222QV) | HSPW(HSPW_LTS222QV));
		LcdcOutp32(rVIDTCON2, LINEVAL(oLcdc.m_uLcdVSz-1) | HOZVAL(oLcdc.m_uLcdHSz-1));
		LcdcOutp32(rDITHMODE, oLcdc.m_uDitherMode); // Fixed Dithering Matrix

	#elif (LCD_MODULE_TYPE == LTV300GV_RGB)
		
		oLcdc.m_uLcdHSz = 640;
		oLcdc.m_uLcdVSz = 480;
		oLcdc.m_uVidOutFormat = VIDOUT_RGBIF;
		oLcdc.m_uDitherMode = RDITHPOS_5BIT|GDITHPOS_6BIT|BDITHPOS_5BIT;
		oLcdc.m_uDitherMode &= ~DITHERING_ENABLE;
	
		LcdcOutp32(rVIDCON1, IHSYNC_INVERT | IVSYNC_INVERT |IVDEN_NORMAL);
		//LcdcOutp32(rVIDCON1, IHSYNC_NORMAL | IVSYNC_NORMAL |IVDEN_NORMAL);
		LcdcOutp32(rVIDTCON0, VBPDE(VBPD_LTV300GV) | VBPD(VBPD_LTV300GV) | VFPD(VFPD_LTV300GV) | VSPW(VSPW_LTV300GV));
		LcdcOutp32(rVIDTCON1, VFPDE(VFPD_LTV300GV) | HBPD(HBPD_LTV300GV) | HFPD(HFPD_LTV300GV) | HSPW(HSPW_LTV300GV));		
		LcdcOutp32(rVIDTCON2, LINEVAL(oLcdc.m_uLcdVSz-1) | HOZVAL(oLcdc.m_uLcdHSz-1));
		LcdcOutp32(rDITHMODE, oLcdc.m_uDitherMode); // Fixed Dithering Matrix	

	#elif (LCD_MODULE_TYPE == LMS480KC01_RGB)

		oLcdc.m_uLcdHSz = 800;
		oLcdc.m_uLcdVSz = 480;
		oLcdc.m_uVidOutFormat = VIDOUT_RGBIF;
		oLcdc.m_uDitherMode = RDITHPOS_8BIT|GDITHPOS_8BIT|BDITHPOS_8BIT;
		oLcdc.m_uDitherMode &= ~DITHERING_ENABLE;

		LcdcOutp32(rVIDCON1, IHSYNC_NORMAL | IVSYNC_NORMAL  | IVDEN_NORMAL |IVCLK_FALL_EDGE); //Check 
		LcdcOutp32(rVIDTCON0, VBPDE(VBPD_LMS480CQ) | VBPD(VBPD_LMS480CQ) | VFPD(VFPD_LMS480CQ) | VSPW(VSPW_LMS480CQ));
		LcdcOutp32(rVIDTCON1, VFPDE(VFPD_LMS480CQ) | HBPD(HBPD_LMS480CQ) | HFPD(HFPD_LMS480CQ) | HSPW(HSPW_LMS480CQ));
		LcdcOutp32(rVIDTCON2, LINEVAL(oLcdc.m_uLcdVSz-1) | HOZVAL(oLcdc.m_uLcdHSz-1));
		LcdcOutp32(rDITHMODE, oLcdc.m_uDitherMode); 	//Fixed Dithering Matrix





	#else
		Assert(0); // Not yet implemented.
	#endif

	LCD_Stop();
	
	//Check up LCD to turn off
	
	while (1)
	{
		uLcdCon=Inp32(LCD_BASE+rVIDCON0);
		if( (uLcdCon&0x03) == 0 ) // checking whether disable the video output and the Display control signal or not.
		break;
	}

	LCD_GetClkValAndClkDir(oLcdc.m_uLcdHSz, oLcdc.m_uLcdVSz, &uClkVal, &uClkDir, eVClkSrc);

	uVidconReg =
		PROGRESSIVE | oLcdc.m_uVidOutFormat | SUB_16_MODE | MAIN_16_MODE | PNRMODE_RGB_P | CLKVALUP_ALWAYS |
		CLKVAL_F(uClkVal) | VCLKEN_DISABLE | CLKDIR_F(uClkDir) | oLcdc.m_uVideoClockSource |
		ENVID_DISABLE | ENVID_F_DISABLE;
	LcdcOutp32(rVIDCON0, uVidconReg);
	
	uILcdIntCon =Inp32(LCD_BASE+rVIDINTCON0);
	LcdcOutp32(rVIDINTCON0, uILcdIntCon | FRAMESEL0_BACK | FRAMESEL1_NONE | INTFRMEN_DISABLE |
				FIFOSEL_WIN0 | FIFOLEVEL_25 | INTFIFOEN_DISABLE | INTEN_DISABLE);

	LCD_SetAllWinOnOff(0); // Turn all windows off
	LCD_SetAllWinColorMapOnOff(0); // Turn all windows color map off
	LCD_SetAllWinColorKeyOnOff(0); // Turn all windows Color Key off

	for (i=1; i<5; i++)
	{
		eWin =
			(i == 1) ? WIN1 :
			(i == 2) ? WIN2 :
			(i == 3) ? WIN3 : WIN4;

		LCD_SetAlpha(0x0, 0x0, 0x0, 0x0, 0x0, 0x0, eWin);
	}

}

void LCD_InitBase2(CLK_SRC eVclkSrc, PNR_MODE ePnrMode, CPU_OUTPUT_DATAFORMAT eCpuOutDataFormat)
{
	u32 uLcdCon;
	u32 uILcdIntCon;
	u32 uClkVal, uClkDir;
	u32 uVidconReg;
	u32 i;
	CLK_SRC eVClkSrc;
	LCD_WINDOW eWin;
	
	oLcdc.m_bIsAutoBuf = false;
	oLcdc.m_uScanMode = 0; // progressive mode
	oLcdc.m_bIsLocalCalled[0] = false;
	oLcdc.m_bIsLocalCalled[1] = false;
	oLcdc.m_bIsLocalCalled[2] = false;

	#if (LCD_CLOCK_SOURCE == LCLK_HCLK)
		oLcdc.m_uVideoClockSource = CLKSEL_F_HCLK;	
		eVClkSrc = SRC_HCLK;

	#elif (LCD_CLOCK_SOURCE == LCLK_MOUT_EPLL)
		oLcdc.m_uVideoClockSource = CLKSEL_F_SYSCON; 
		eVClkSrc = SRC_MOUT_EPLL;

	#elif (LCD_CLOCK_SOURCE == LCLK_DOUT_MPLL)
		oLcdc.m_uVideoClockSource = CLKSEL_F_SYSCON; 
		eVClkSrc = SRC_DOUT_MPLL;

	#elif (LCD_CLOCK_SOURCE == LCLK_FIN_EPLL)
		oLcdc.m_uVideoClockSource = CLKSEL_F_SYSCON; 
		eVClkSrc = SRC_FIN_EPLL;	

	#elif (LCD_CLOCK_SOURCE == LCLK_27M)
		oLcdc.m_uVideoClockSource = CLKSEL_F_EXTCLK;
		eVClkSrc = SRC_27M;
	#endif

	if(ePnrMode == PNR_Parallel_RGB)		oLcdc.m_uPnrMode = PNRMODE_RGB_P;
	else if(ePnrMode == PNR_Parallel_BGR)	oLcdc.m_uPnrMode = PNRMODE_BGR_P;	
	else if(ePnrMode == PNR_Serial_RGB)		oLcdc.m_uPnrMode = PNRMODE_RGB_S;
	else if(ePnrMode == PNR_Serial_BGR)		oLcdc.m_uPnrMode = PNRMODE_BGR_S; 

	if(eCpuOutDataFormat == CPU_16BIT)		
	{
		oLcdc.m_uI80MainOutDataFormat = MAIN_16_MODE;
		oLcdc.m_uI80SubOutDataFormat = SUB_16_MODE;
	}
	else if(eCpuOutDataFormat == CPU_16_2BIT)	
	{
		oLcdc.m_uI80MainOutDataFormat = MAIN_16PLUS2_MODE ;
		oLcdc.m_uI80SubOutDataFormat = SUB_16PLUS2_MODE ;
	}		
	else if(eCpuOutDataFormat == CPU_9_9BIT)
	{
		oLcdc.m_uI80MainOutDataFormat = MAIN_9PLUS9_MODE ;
		oLcdc.m_uI80SubOutDataFormat = SUB_9PLUS9_MODE ;
	}	
	else if(eCpuOutDataFormat == CPU_16_8BIT)
	{
		oLcdc.m_uI80MainOutDataFormat = MAIN_16PLUS8_MODE;
		oLcdc.m_uI80SubOutDataFormat = SUB_16PLUS8_MODE;
	}
	else if(eCpuOutDataFormat == CPU_18BIT)
	{
		oLcdc.m_uI80MainOutDataFormat = MAIN_18_MODE;
		oLcdc.m_uI80SubOutDataFormat = SUB_18_MODE;
	}	
	else if(eCpuOutDataFormat == CPU_8_8BIT)
	{
		oLcdc.m_uI80MainOutDataFormat = MAIN_8PLUS8_MODE;
		oLcdc.m_uI80SubOutDataFormat = SUB_8PLUS8_MODE;
	}	


	///////////////////////////////////////////////////////////////////////////////////////////////////////
	#if (LCD_MODULE_TYPE == LTV350QV_RGB)
	
		oLcdc.m_uLcdHSz = 320;
		oLcdc.m_uLcdVSz = 240;
		oLcdc.m_uVidOutFormat = VIDOUT_RGBIF;
		//oLcdc.m_uVidOutFormat = VIDOUT_TV; //To Test Free-Run  
		oLcdc.m_uDitherMode = RDITHPOS_8BIT|GDITHPOS_8BIT|BDITHPOS_8BIT;
		oLcdc.m_uDitherMode &= ~DITHERING_ENABLE;

		LcdcOutp32(rVIDCON1, IHSYNC_INVERT | IVSYNC_INVERT  | IVDEN_INVERT); //Check 
		LcdcOutp32(rVIDTCON0, VBPDE(VBPD_LTV350QV) | VBPD(VBPD_LTV350QV) | VFPD(VFPD_LTV350QV) | VSPW(VSPW_LTV350QV));
		LcdcOutp32(rVIDTCON1, VFPDE(VFPD_LTV350QV) | HBPD(HBPD_LTV350QV) | HFPD(HFPD_LTV350QV) | HSPW(HSPW_LTV350QV));
		LcdcOutp32(rVIDTCON2, LINEVAL(oLcdc.m_uLcdVSz-1) | HOZVAL(oLcdc.m_uLcdHSz-1));
		LcdcOutp32(rDITHMODE, oLcdc.m_uDitherMode); // Fixed Dithering Matrix

	#elif (LCD_MODULE_TYPE == LTS222QV_CPU)
	
		oLcdc.m_uLcdHSz = 240;
		oLcdc.m_uLcdVSz = 320;
		oLcdc.m_uVidOutFormat = 
						(LCD_CPUIF_CS == MAIN) ? VIDOUT_I80IF0: VIDOUT_I80IF1;
			
		LcdcOutp32(rVIDTCON2, LINEVAL(oLcdc.m_uLcdVSz-1) | HOZVAL(oLcdc.m_uLcdHSz-1));

	#elif (LCD_MODULE_TYPE == LTP700WV_RGB ||LCD_MODULE_TYPE ==LTE480WV_RGB)

		oLcdc.m_uLcdHSz = 800;
		oLcdc.m_uLcdVSz = 480;
		oLcdc.m_uVidOutFormat = VIDOUT_RGBIF;
		oLcdc.m_uDitherMode = RDITHPOS_8BIT|GDITHPOS_8BIT|BDITHPOS_8BIT;
		oLcdc.m_uDitherMode &= ~DITHERING_ENABLE;

		LcdcOutp32(rVIDCON1, IHSYNC_INVERT | IVSYNC_INVERT  | IVDEN_NORMAL |IVCLK_RISE_EDGE); //Check 
		LcdcOutp32(rVIDTCON0, VBPDE(VBPD_LTP700WV) | VBPD(VBPD_LTP700WV) | VFPD(VFPD_LTP700WV) | VSPW(VSPW_LTP700WV));
		LcdcOutp32(rVIDTCON1, VFPDE(VFPD_LTP700WV) | HBPD(HBPD_LTP700WV) | HFPD(HFPD_LTP700WV) | HSPW(HSPW_LTP700WV));
		LcdcOutp32(rVIDTCON2, LINEVAL(oLcdc.m_uLcdVSz-1) | HOZVAL(oLcdc.m_uLcdHSz-1));
		LcdcOutp32(rDITHMODE, oLcdc.m_uDitherMode); 	//Fixed Dithering Matrix

	#elif (LCD_MODULE_TYPE == LMS480KC01_RGB)

		oLcdc.m_uLcdHSz = 800;
		oLcdc.m_uLcdVSz = 480;
		oLcdc.m_uVidOutFormat = VIDOUT_RGBIF;
		oLcdc.m_uDitherMode = RDITHPOS_8BIT|GDITHPOS_8BIT|BDITHPOS_8BIT;
		oLcdc.m_uDitherMode &= ~DITHERING_ENABLE;

		LcdcOutp32(rVIDCON1, IHSYNC_NORMAL | IVSYNC_NORMAL  | IVDEN_NORMAL |IVCLK_FALL_EDGE); //Check 
		LcdcOutp32(rVIDTCON0, VBPDE(VBPD_LMS480CQ) | VBPD(VBPD_LMS480CQ) | VFPD(VFPD_LMS480CQ) | VSPW(VSPW_LMS480CQ));
		LcdcOutp32(rVIDTCON1, VFPDE(VFPD_LMS480CQ) | HBPD(HBPD_LMS480CQ) | HFPD(HFPD_LMS480CQ) | HSPW(HSPW_LMS480CQ));
		LcdcOutp32(rVIDTCON2, LINEVAL(oLcdc.m_uLcdVSz-1) | HOZVAL(oLcdc.m_uLcdHSz-1));
		LcdcOutp32(rDITHMODE, oLcdc.m_uDitherMode); 	//Fixed Dithering Matrix


	#elif (LCD_MODULE_TYPE == LTS222QV_SRGB)
	
		oLcdc.m_uLcdHSz = 240;
		oLcdc.m_uLcdVSz = 320;
		oLcdc.m_uVidOutFormat = VIDOUT_RGBIF;
		oLcdc.m_uDitherMode = RDITHPOS_5BIT|GDITHPOS_6BIT|BDITHPOS_5BIT;
		oLcdc.m_uDitherMode &= ~DITHERING_ENABLE;

		LcdcOutp32(rVIDCON1, IVCLK_FALL_EDGE | IHSYNC_INVERT | IVSYNC_INVERT | IVDEN_NORMAL);
		LcdcOutp32(rVIDTCON0, VBPDE(0) | VBPD(VBPD_LTS222QV) | VFPD(VFPD_LTS222QV) | VSPW(VSPW_LTS222QV));
		LcdcOutp32(rVIDTCON1, VFPDE(0) | HBPD(HBPD_LTS222QV) | HFPD(HFPD_LTS222QV) | HSPW(HSPW_LTS222QV));
		LcdcOutp32(rVIDTCON2, LINEVAL(oLcdc.m_uLcdVSz-1) | HOZVAL(oLcdc.m_uLcdHSz-1));
		LcdcOutp32(rDITHMODE, oLcdc.m_uDitherMode); // Fixed Dithering Matrix	

	#elif (LCD_MODULE_TYPE == LTV300GV_RGB)
		
		oLcdc.m_uLcdHSz = 640;
		oLcdc.m_uLcdVSz = 480;
		oLcdc.m_uVidOutFormat = VIDOUT_RGBIF;
		oLcdc.m_uDitherMode = RDITHPOS_5BIT|GDITHPOS_6BIT|BDITHPOS_5BIT;
		oLcdc.m_uDitherMode &= ~DITHERING_ENABLE;
	
		LcdcOutp32(rVIDCON1, IHSYNC_INVERT | IVSYNC_INVERT |IVDEN_NORMAL);
		//LcdcOutp32(rVIDCON1, IHSYNC_NORMAL | IVSYNC_NORMAL |IVDEN_NORMAL);
		LcdcOutp32(rVIDTCON0, VBPDE(VBPD_LTV300GV) | VBPD(VBPD_LTV300GV) | VFPD(VFPD_LTV300GV) | VSPW(VSPW_LTV300GV));
		LcdcOutp32(rVIDTCON1, VFPDE(VFPD_LTV300GV) | HBPD(HBPD_LTV300GV) | HFPD(HFPD_LTV300GV) | HSPW(HSPW_LTV300GV));		
		LcdcOutp32(rVIDTCON2, LINEVAL(oLcdc.m_uLcdVSz-1) | HOZVAL(oLcdc.m_uLcdHSz-1));
		LcdcOutp32(rDITHMODE, oLcdc.m_uDitherMode); // Fixed Dithering Matrix	
		
	#else
		Assert(0); // Not yet implemented.
	#endif

	LCD_Stop();
	
	//Check up LCD to turn off
	
	while (1)
	{
		uLcdCon=Inp32(LCD_BASE+rVIDCON0);
		if( (uLcdCon&0x03) == 0 ) // checking whether disable the video output and the Display control signal or not.
		break;
	}

	LCD_GetClkValAndClkDir(oLcdc.m_uLcdHSz, oLcdc.m_uLcdVSz, &uClkVal, &uClkDir, eVClkSrc);

	uVidconReg =
		PROGRESSIVE | oLcdc.m_uVidOutFormat | oLcdc.m_uI80SubOutDataFormat | oLcdc.m_uI80MainOutDataFormat | oLcdc.m_uPnrMode | CLKVALUP_ALWAYS |
		CLKVAL_F(uClkVal) | VCLKEN_DISABLE | CLKDIR_F(uClkDir)| oLcdc.m_uVideoClockSource|
		ENVID_DISABLE | ENVID_F_DISABLE ;

	LcdcOutp32(rVIDCON0, uVidconReg);
	
	uILcdIntCon =Inp32(LCD_BASE+rVIDINTCON0);
	LcdcOutp32(rVIDINTCON0, uILcdIntCon | FRAMESEL0_BACK | FRAMESEL1_NONE | INTFRMEN_DISABLE |
				FIFOSEL_WIN0 | FIFOLEVEL_25 | INTFIFOEN_DISABLE | INTEN_DISABLE);

	LCD_SetAllWinOnOff(0); // Turn all windows off
	LCD_SetAllWinColorMapOnOff(0); // Turn all windows color map off
	LCD_SetAllWinColorKeyOnOff(0); // Turn all windows Color Key off

	for (i=1; i<5; i++)
	{
		eWin =
			(i == 1) ? WIN1 :
			(i == 2) ? WIN2 :
			(i == 3) ? WIN3 : WIN4;

		LCD_SetAlpha(0x0, 0x0, 0x0, 0x0, 0x0, 0x0, eWin);
	}

}

//------------------------------------------------------------
// Function Name : LCD_InitWin
//
// Function Desctiption :
//
// Input :	eBpp, background bpp
//			uFrameH, horizontal size for each window
//			uFrameV, vertical size for each window
//			uX_Frame, frame buffer start x offset
//			uY_Frame, frame buffer start y offset
//			uViewH, horizontal view size for each window
//			uViewV, vertical view size for each window
//			uX_Lcd, view start x offset
//			uY_Lcd, view start y offset
//			uFbAddr, frame buffer start address
//			eWin, window number
//			bIsDoubleBuf, using double buffer or not ?
//
// Output :
//
// Version : 
//------------------------------------------------------------
void LCD_InitWin(CSPACE eBpp, 
				  u32 uFrameH, 
				  u32 uFrameV, 
				  u32 uX_Frame, 
				  u32 uY_Frame,  
				  u32 uViewH, 
				  u32 uViewV,
				  u32 uX_Lcd, 
				  u32 uY_Lcd, 
				  u32 uFbAddr, 
				  LCD_WINDOW eWin,
				  u8 bIsDoubleBuf)
{
	u32 uOffset;
	//u32 uLineVal;
	u32 uBurstSize;

	if ( (eWin != WIN0 && eWin != WIN1) && bIsDoubleBuf == true )
		Assert(0); // In WIN2, WIN3 or WIN4, Double buffering can't be supported

	// Viewport size must be less than LCD size
	Assert(uViewH <= oLcdc.m_uLcdHSz);
	Assert(uViewV <= oLcdc.m_uLcdVSz);

	// Check (X,Y) coordinate is valid in LCD and Frame?
	Assert( ((uX_Frame + uViewH) <= uFrameH) && ((uY_Frame + uViewV) <= uFrameV) );
	Assert( (uX_Lcd + uViewH <= oLcdc.m_uLcdHSz) && (uY_Lcd + uViewV <= oLcdc.m_uLcdVSz) );

	// Double buffering is supported by only window 0 and window 1
	if ( ((eWin != WIN0) && (eWin != WIN1)) && (bIsDoubleBuf == true) )
		Assert(0);

	oLcdc.m_uFrmHSz[eWin] = uFrameH;
	oLcdc.m_uFrmVSz[eWin] = uFrameV;

	oLcdc.m_uViewHSz[eWin] = uViewH;
	oLcdc.m_uViewVSz[eWin] = uViewV;

	oLcdc.m_uLcdStX = uX_Lcd;
	oLcdc.m_uLcdStY = uY_Lcd;

	uOffset = oLcdc.m_uFrmHSz[eWin] - oLcdc.m_uViewHSz[eWin];

	oLcdc.m_uBytSwpMode = BYTSWP_DISABLE;		// BYTE swap disable
	oLcdc.m_uHawSwpMode = HAWSWP_DISABLE;		// Half-Word swap disable

	oLcdc.m_uMaxBurstLen = MAX_BURSTLEN_16WORD;		// DMA'burst max 16word burst
	oLcdc.m_uMinBurstLen = MIN_BURSTLEN_16WORD;		// DMA'burst min 16 word burst


	//Setting BPP, HW Swap, and Byte Swap
	if (eBpp == PAL1)
	{
		oLcdc.m_uBytes = 1;
		oLcdc.m_uBits = 4;
		Assert( !(uOffset%32) ); // Must keep to word-alignment
		oLcdc.m_uBppMode = BPPMODE_F_1BPP;

		oLcdc.m_uMaxBurstLen = MAX_BURSTLEN_4WORD;
		oLcdc.m_uMinBurstLen = MIN_BURSTLEN_16WORD;
	}
	else if (eBpp == PAL2)
	{
		oLcdc.m_uBytes = 1;
		oLcdc.m_uBits = 4;
		Assert( !(uOffset%16) ); // Must keep to word-alignment
		oLcdc.m_uBppMode = BPPMODE_F_2BPP;

		oLcdc.m_uMaxBurstLen = MAX_BURSTLEN_4WORD;
		oLcdc.m_uMinBurstLen = MIN_BURSTLEN_16WORD;
	}
	else if ( (eBpp == PAL4) && (eWin != WIN4) )
	{
		oLcdc.m_uBytes = 1;
		oLcdc.m_uBits = 2;

		Assert( !(uOffset%8) ); // Must keep to word-alignment
		oLcdc.m_uBppMode = BPPMODE_F_4BPP;

		oLcdc.m_uMaxBurstLen = MAX_BURSTLEN_4WORD;
		oLcdc.m_uMinBurstLen = MIN_BURSTLEN_16WORD;
	}
	else if ( (eBpp == PAL8) && ((eWin == WIN0) || (eWin == WIN1)) )
	{
		oLcdc.m_uBytes = 1;
		oLcdc.m_uBits = 1;
		Assert( !(uOffset%4) ); // Must keep to word-alignment
		oLcdc.m_uBppMode = BPPMODE_F_8BPP_PAL;
		oLcdc.m_uBytSwpMode = BYTSWP_ENABLE;
	}
	else if ( (eBpp == RGB8) && ((eWin == WIN1) || (eWin == WIN2)) )
	{
		oLcdc.m_uBytes = 1;
		oLcdc.m_uBits = 1;
		Assert( !(uOffset%4) ); // Must keep to word-alignment
		oLcdc.m_uBppMode = BPPMODE_F_8BPP_NOPAL;

		oLcdc.m_uBytSwpMode = BYTSWP_ENABLE;
	}
	else if (eBpp == RGB16)
	{
		oLcdc.m_uBytes = 2;
		oLcdc.m_uBits = 1;
		Assert( !(uOffset%2) ); // Must keep to word-alignment

		oLcdc.m_uBppMode = BPPMODE_F_16BPP_565;
		oLcdc.m_uHawSwpMode = HAWSWP_ENABLE;
	}
	else if (eBpp == ARGB16)
	{
		oLcdc.m_uBytes = 2;
		oLcdc.m_uBits = 1;
		Assert( !(uOffset%2) ); // Must keep to word-alignment

		oLcdc.m_uBppMode = BPPMODE_F_16BPP_A555;
		oLcdc.m_uHawSwpMode = HAWSWP_ENABLE;
	}
	else if (eBpp == RGB18)
	{
		oLcdc.m_uBytes = 4;
		oLcdc.m_uBits = 1;

		oLcdc.m_uBppMode = BPPMODE_F_18BPP_666;
	}
	else if (eBpp == RGB24)
	{
		oLcdc.m_uBytes = 4;
		oLcdc.m_uBits = 1;

		oLcdc.m_uBppMode = BPPMODE_F_24BPP_888;
	}
	else if (eBpp == ARGB24)
	{
		oLcdc.m_uBytes = 4;
		oLcdc.m_uBits = 1;

		oLcdc.m_uBppMode = BPPMODE_F_24BPP_A887;
	}
	else
	{
		Assert(0);
	}

	// Get offsetsize, pagewidth and lineVal
	if (oLcdc.m_uFrmHSz[eWin] >= oLcdc.m_uViewHSz[eWin])
	{
		oLcdc.m_uOffsetSz[eWin] = uOffset*oLcdc.m_uBytes/oLcdc.m_uBits;
		oLcdc.m_uPageWidth[eWin] = oLcdc.m_uViewHSz[eWin]*oLcdc.m_uBytes/oLcdc.m_uBits;

		// Offset size must be more than the burst size
		uBurstSize =
			(oLcdc.m_uMaxBurstLen == MAX_BURSTLEN_16WORD) ? 16*4 :
			(oLcdc.m_uMaxBurstLen == MAX_BURSTLEN_8WORD) ? 8*4 : 4*4;

		// Offset size must be more than the burst size
		Assert( oLcdc.m_uOffsetSz[eWin] == 0 || !(oLcdc.m_uOffsetSz[eWin]%4) );
		Assert(oLcdc.m_uPageWidth[eWin] > uBurstSize); // Page width must be more than burst size and be word-aligned

		//uLineVal = oLcdc.m_uViewVSz[eWin] - 1;
	} 
	else
	{
		Assert(0);
	}

	oLcdc.m_uImgStAddr[eWin] = uFbAddr;

	oLcdc.m_uFbStAddr[eWin] = uFbAddr + (oLcdc.m_uFrmHSz[eWin]*uY_Frame + uX_Frame)*oLcdc.m_uBytes/oLcdc.m_uBits;
	oLcdc.m_uFbEndAddr[eWin] = oLcdc.m_uFbStAddr[eWin] + (oLcdc.m_uOffsetSz[eWin]+oLcdc.m_uPageWidth[eWin])*(uFrameV);

	if ( (bIsDoubleBuf == true) && ((eWin == WIN0) || (eWin == WIN1)) )
	{
		oLcdc.m_uDoubleBufImgStAddr[eWin] = oLcdc.m_uImgStAddr[eWin] + oLcdc.m_uFrmHSz[eWin]*oLcdc.m_uFrmVSz[eWin]*oLcdc.m_uBytes/oLcdc.m_uBits;
		oLcdc.m_uDoubleBufStAddr[eWin] = oLcdc.m_uFbStAddr[eWin] + oLcdc.m_uFrmHSz[eWin]*oLcdc.m_uFrmVSz[eWin]*oLcdc.m_uBytes/oLcdc.m_uBits;
		oLcdc.m_uDoubleBufEndAddr[eWin] = oLcdc.m_uDoubleBufStAddr[eWin] + (oLcdc.m_uOffsetSz[eWin]+oLcdc.m_uPageWidth[eWin])*(uFrameV);
	}
	else if ( (bIsDoubleBuf == false) && ((eWin == WIN0) || (eWin == WIN1)) )
	{
		oLcdc.m_uDoubleBufStAddr[eWin] = oLcdc.m_uFbStAddr[eWin];
		oLcdc.m_uDoubleBufEndAddr[eWin] = oLcdc.m_uFbEndAddr[eWin];
	}

	LCD_InitWinRegs(eWin);

	// For back-ward compatibility LCDC V2.0
	if(eWin == WIN0)
		LCD_SetWinOnOff(1, WIN0); // first arguement == 1 -> turn on window
}


void LCD_InitDISPCForFifoIn(LCD_LOCAL_INPUT eInLocal, LCD_WINDOW eWin)
{
	LCD_InitBase2((CLK_SRC)LCLK_DOUT_MPLL, PNR_Parallel_RGB, CPU_16BIT);
	LCD_InitWinForFifoIn(eInLocal, eWin);
}


//-----------------------------------------------------------
// Function Name :  LCD_InitLTV350QV
//
// Function Desctiption :
//
// Input :	
//
// Output :
//
// Version : 
//------------------------------------------------------------
void LCD_InitLTV350QV_RGB(void)
{	
	u32 i=0;
#if 1 // SPI Function Test

	SPIInit();
	SPIStart();

	i=0;
	while(1)
	{
		LTICommand(LTI_TYPE_INDEX,LTV350QV_RGB_initialize[i][0]);
		LTICommand(LTI_TYPE_INSTRUCTION,LTV350QV_RGB_initialize[i][1]);
		
		if (LTV350QV_RGB_initialize[i][2]) DelayLoop_1ms(LTV350QV_RGB_initialize[i][2]);

		i++;
		if (LTV350QV_RGB_initialize[i][0] == 0 && LTV350QV_RGB_initialize[i][1] == 1) Delay(500);
		if (LTV350QV_RGB_initialize[i][0] == 0 && LTV350QV_RGB_initialize[i][1] == 2) LCD_Start();
		if (LTV350QV_RGB_initialize[i][0] == 0 && LTV350QV_RGB_initialize[i][1] == 0) break;
	}

	DelayLoop_1ms(1);
	
#else
	
	//Reset LCD Module
	//GPIO_SetDataEach(eGPIO_N, eGPIO_5 , 0); //GPN[5] -> Low
	//Delay(100);	//Delay about 10ms
	//GPIO_SetDataEach(eGPIO_N, eGPIO_5 , 1); //GPN[5] -> High	

	/*
	//Config SPI GPIO Port
	GPIO_SetFunctionEach(eGPIO_C, eGPIO_5, 1); //GPC[5]:SPI_SCLK -> Output
	GPIO_SetFunctionEach(eGPIO_C, eGPIO_6, 1); //GPC[6]:SPI_MOSI -> Output
	GPIO_SetFunctionEach(eGPIO_C, eGPIO_7, 1); //GPC[7]:SPI_CS -> Output
	
	GPIO_SetPullUpDownEach(eGPIO_C, eGPIO_5, 0); //GPC[5]:SPI_SCLK -> Pull-up/down Disable
	GPIO_SetPullUpDownEach(eGPIO_C, eGPIO_6, 0); //GPC[6]:SPI_MOSI -> Pull-up/down Disable
	GPIO_SetPullUpDownEach(eGPIO_C, eGPIO_7, 0); //GPC[7]:SPI_CS -> Pull-up/down Disable
	Delay(50); //Delay about 5ms
	*/

	//Set DEN, DCLK, and DSERI to High 
	LCD_DEN_Hi;
	LCD_DCLK_Hi;
	LCD_DSERI_Hi;	
	
	///////////////////////////////////////////////////////////////////
	// Init_Lcd_Function
	//////////////////////////////////////////////////////////////////


	LCD_PutCmdtoLTV350QV(0x01,0x001d);	
	LCD_PutCmdtoLTV350QV(0x02,0x0000);    
    LCD_PutCmdtoLTV350QV(0x03,0x0000);    
    LCD_PutCmdtoLTV350QV(0x04,0x0000);
    LCD_PutCmdtoLTV350QV(0x05,0x50a3);
    LCD_PutCmdtoLTV350QV(0x06,0x0000);
    LCD_PutCmdtoLTV350QV(0x07,0x0000);
    LCD_PutCmdtoLTV350QV(0x08,0x0000);
   	LCD_PutCmdtoLTV350QV(0x09,0x0000);
   	LCD_PutCmdtoLTV350QV(0x0a,0x0000);
   	LCD_PutCmdtoLTV350QV(0x10,0x0000);
   	LCD_PutCmdtoLTV350QV(0x11,0x0000);
   	LCD_PutCmdtoLTV350QV(0x12,0x0000);
   	LCD_PutCmdtoLTV350QV(0x13,0x0000);
   	LCD_PutCmdtoLTV350QV(0x14,0x0000);
   	LCD_PutCmdtoLTV350QV(0x15,0x0000);
   	LCD_PutCmdtoLTV350QV(0x16,0x0000);
   	LCD_PutCmdtoLTV350QV(0x17,0x0000);
   	LCD_PutCmdtoLTV350QV(0x18,0x0000);
   	LCD_PutCmdtoLTV350QV(0x19,0x0000);

	// Delay about 10ms
	Delay(100);
	
	///////////////////////////////////////////////////////////////////
	// Power Setting Function 1
	//////////////////////////////////////////////////////////////////
	LCD_PutCmdtoLTV350QV(0x09,0x4055);
	LCD_PutCmdtoLTV350QV(0x0a,0x0000);

	// Delay about 10ms
	Delay(100);
	
	/////////////////////////////////////////////////////////////////////
	// Power Setting 2
	/////////////////////////////////////////////////////////////////////

	LCD_PutCmdtoLTV350QV(0x0a,0x2000);

	// Delay about 50ms
	Delay(500);
	
	///////////////////////////////////////////////////////////////////
	// Instruction Setting
	///////////////////////////////////////////////////////////////////

	LCD_PutCmdtoLTV350QV(0x01,0x409d);
	LCD_PutCmdtoLTV350QV(0x02,0x0204);
	LCD_PutCmdtoLTV350QV(0x03,0x2100);
	LCD_PutCmdtoLTV350QV(0x04,0x1000);
	LCD_PutCmdtoLTV350QV(0x05,0x5003);
	LCD_PutCmdtoLTV350QV(0x06,0x0009);	//vbp
	LCD_PutCmdtoLTV350QV(0x07,0x000f);	//hbp
	LCD_PutCmdtoLTV350QV(0x08,0x0800);
	LCD_PutCmdtoLTV350QV(0x10,0x0000);
	LCD_PutCmdtoLTV350QV(0x11,0x0000);
	LCD_PutCmdtoLTV350QV(0x12,0x000f);
	LCD_PutCmdtoLTV350QV(0x13,0x1f00);
	LCD_PutCmdtoLTV350QV(0x14,0x0000);
	LCD_PutCmdtoLTV350QV(0x15,0x0000);
	LCD_PutCmdtoLTV350QV(0x16,0x0000);
	LCD_PutCmdtoLTV350QV(0x17,0x0000);
	LCD_PutCmdtoLTV350QV(0x18,0x0000);
	LCD_PutCmdtoLTV350QV(0x19,0x0000);

	// Delay about 50 ms
	Delay(500);


	LCD_Start();

	///////////////////////////////////////////////////////////////////
	// Display On Sequence
	///////////////////////////////////////////////////////////////////

	LCD_PutCmdtoLTV350QV(0x09,0x4a55);
	LCD_PutCmdtoLTV350QV(0x0a,0x2000);	
#endif	
}


void LCD_InitLTP700WV_RGB(void)
{
	u32 uHSz, uVSz;
			
	LCD_InitBase2((CLK_SRC)LCLK_DOUT_MPLL, PNR_Parallel_RGB, CPU_16BIT);
	LCD_InitWin(RGB24, oLcdc.m_uLcdHSz, oLcdc.m_uLcdVSz, 0, 0, oLcdc.m_uLcdHSz, oLcdc.m_uLcdVSz, 0, 0, LCD_FRAMEBUFFER_ST, WIN0, false);
	LCD_SetWinOnOff(1, WIN0);
	
	LCD_GetFrmSz(&uHSz, &uVSz, WIN0);
	GLIB_InitInstance(LCD_FRAMEBUFFER_ST, uHSz, uVSz, RGB24);
	GLIB_DrawPattern(uHSz, uVSz);

	INTC_SetVectAddr(NUM_LCD1, Isr_LCD_FrameCount);
	LCD_InitInt(LCD_FRAME_INT, LCD_SYSMAIN_INT, FIFO_25, FIFO_INT_WIN0, FINT_VSYNC);
	LCD_EnableInt();
	INTC_Enable(NUM_LCD1);

	LCD_Start();
	
	uFrameCount=0;
	
	while(1)
	{
		if(uFrameCount == 3)
		break;
	}
	
	GPIO_SetDataEach(eGPIO_N, eGPIO_5 , 1); //GPN[5] -> High	

	LCD_SetAllWinOnOff(0);
	LCD_Stop();

	INTC_Disable(NUM_LCD1);
}

void LCD_InitLMS480KCO_RGB(void)
{
	u32 uHSz, uVSz,i=0;

	//Reset LCD Module
//	GPIO_SetDataEach(eGPIO_N, eGPIO_5 , 0); //GPN[5] -> Low
//	Delay(200);	//Delay about 20ms
//	GPIO_SetDataEach(eGPIO_N, eGPIO_5 , 1); //GPN[5] -> High	
//	Delay(200);	//Delay about 10ms
	


	#if 0 
	//Set DEN, DCLK, and DSERI to High 
	LCD_DEN_Hi;
	LCD_DCLK_Hi;
	LCD_DSERI_Hi;	
	
	///////////////////////////////////////////////////////////////////
	// Init_Lcd_Function
	//////////////////////////////////////////////////////////////////

	LCD_PutCmdtoLMS480KCO(0x00,0x0063);	// default vlaue // set driver output 100% , hareware pin stanby
	LCD_PutCmdtoLMS480KCO(0x01,0x0055);	// default vlaue // interlaced display mode
	LCD_PutCmdtoLMS480KCO(0x02,0x0003);	// vertical display position setting 0x3(-13H) backporch period setting. default 0x10
	LCD_PutCmdtoLMS480KCO(0x03,0x0011);	// horizontal display position setting 0x11(-111NCLK) default 0x80
	LCD_PutCmdtoLMS480KCO(0x04,0x0000);	// default vlaue
	LCD_PutCmdtoLMS480KCO(0x05,0x0011);	// default vlaue
	LCD_PutCmdtoLMS480KCO(0x06,0x0000);	// signal polarity setting ODD/EVEN(high), CK(falling edge), HD(Low), VD(Low)
	LCD_PutCmdtoLMS480KCO(0x07,0x0000);	// default vlaue
	LCD_PutCmdtoLMS480KCO(0x08,0x0044);	// default vlaue
	LCD_PutCmdtoLMS480KCO(0x09,0x0002);	// BSEQ(0x2) ODD/Even Line 1,4,2,5,3,6 ?
	LCD_PutCmdtoLMS480KCO(0x0a,0x002a);	// Bank Non Overlap time BNO[5:0]=0x2a(+10NCLK)
	LCD_PutCmdtoLMS480KCO(0x0b,0x0028);	// bank switch turn on time // default 0x40
	LCD_PutCmdtoLMS480KCO(0x0c,0x002a);	// Gate non-overlap setting // default 0x40
	LCD_PutCmdtoLMS480KCO(0x0d,0x003e);	// Gate source ouput delay setting // default 0x20
	LCD_PutCmdtoLMS480KCO(0x0e,0x0020);	// default vlaue
	LCD_PutCmdtoLMS480KCO(0x0f,0x0020);	// default vlaue
	// tunning ?
	LCD_PutCmdtoLMS480KCO(0x10,0x00f9);	// default 0xf0
	LCD_PutCmdtoLMS480KCO(0x11,0x00f4);	// default 0xf4
	LCD_PutCmdtoLMS480KCO(0x12,0x0010);	// default vlaue
	LCD_PutCmdtoLMS480KCO(0x13,0x0020);	// default vlaue
	LCD_PutCmdtoLMS480KCO(0x14,0x0020);	// default vlaue
	LCD_PutCmdtoLMS480KCO(0x15,0x0020);	// default vlaue
	LCD_PutCmdtoLMS480KCO(0x16,0x0080);	// default vlaue
	LCD_PutCmdtoLMS480KCO(0x17,0x0080);	// default vlaue
	LCD_PutCmdtoLMS480KCO(0x18,0x0080);	// default vlaue
	LCD_PutCmdtoLMS480KCO(0x20,0x0086);
	LCD_PutCmdtoLMS480KCO(0x21,0x0006);
	LCD_PutCmdtoLMS480KCO(0x22,0x000b);
	LCD_PutCmdtoLMS480KCO(0x23,0x0016);
	LCD_PutCmdtoLMS480KCO(0x24,0x0058);
	LCD_PutCmdtoLMS480KCO(0x25,0x0094);
	LCD_PutCmdtoLMS480KCO(0x26,0x00cf);
	LCD_PutCmdtoLMS480KCO(0x27,0x0055);
	LCD_PutCmdtoLMS480KCO(0x28,0x00bc);
	LCD_PutCmdtoLMS480KCO(0x29,0x0018);
	LCD_PutCmdtoLMS480KCO(0x2a,0x0069);
	LCD_PutCmdtoLMS480KCO(0x2b,0x00fe);
	LCD_PutCmdtoLMS480KCO(0x2c,0x0054);
	LCD_PutCmdtoLMS480KCO(0x2d,0x0095);
	LCD_PutCmdtoLMS480KCO(0x2e,0x00fe);
	LCD_PutCmdtoLMS480KCO(0x50,0x0086);
	LCD_PutCmdtoLMS480KCO(0x51,0x0006);
	LCD_PutCmdtoLMS480KCO(0x52,0x000b);
	LCD_PutCmdtoLMS480KCO(0x53,0x0016);
	LCD_PutCmdtoLMS480KCO(0x54,0x0058);
	LCD_PutCmdtoLMS480KCO(0x55,0x0094);
	LCD_PutCmdtoLMS480KCO(0x56,0x00cf);
	LCD_PutCmdtoLMS480KCO(0x57,0x0055);
	LCD_PutCmdtoLMS480KCO(0x58,0x00bc);
	LCD_PutCmdtoLMS480KCO(0x59,0x0018);
	LCD_PutCmdtoLMS480KCO(0x5a,0x0069);
	LCD_PutCmdtoLMS480KCO(0x5b,0x00fe);
	LCD_PutCmdtoLMS480KCO(0x5c,0x0054);
	LCD_PutCmdtoLMS480KCO(0x5d,0x0095);
	LCD_PutCmdtoLMS480KCO(0x5e,0x00fe);
	LCD_PutCmdtoLMS480KCO(0x2f,0x0021);
#else

	SPIInit();
	SPIStart();

	i=0;
	while(1)
	{
		LTICommand(LTI_mPVA, LMS480QC0_RGB_initialize[i][0]);
//		LTICommand(LTI_mPVA, LMS480QC0_RGB_initialize[i][1]);
		
		if (LMS480QC0_RGB_initialize[i][2]) DelayLoop_1ms(LMS480QC0_RGB_initialize[i][2]);

		i++;

		if (LMS480QC0_RGB_initialize[i][0] == 0 && LMS480QC0_RGB_initialize[i][1] == 0) break;
	}

	DelayLoop_1ms(1);
	
#endif

	Delay(100);
	
	LCD_InitBase2((CLK_SRC)LCLK_DOUT_MPLL, PNR_Parallel_RGB, CPU_16BIT);
	LCD_InitWin(RGB24, oLcdc.m_uLcdHSz, oLcdc.m_uLcdVSz, 0, 0, oLcdc.m_uLcdHSz, oLcdc.m_uLcdVSz, 0, 0, LCD_FRAMEBUFFER_ST, WIN0, false);
	LCD_SetWinOnOff(1, WIN0);
	
	LCD_GetFrmSz(&uHSz, &uVSz, WIN0);
	GLIB_InitInstance(LCD_FRAMEBUFFER_ST, uHSz, uVSz, RGB24);
	GLIB_DrawPattern(uHSz, uVSz);

	INTC_SetVectAddr(NUM_LCD1, Isr_LCD_FrameCount);
	LCD_InitInt(LCD_FRAME_INT, LCD_SYSMAIN_INT, FIFO_25, FIFO_INT_WIN0, FINT_VSYNC);
	LCD_EnableInt();
	INTC_Enable(NUM_LCD1);

	LCD_Start();
	
	uFrameCount=0;
	
	while(1)
	{
		if(uFrameCount == 3)
		break;
	}
	
	GPIO_SetDataEach(eGPIO_N, eGPIO_5 , 1); //GPN[5] -> High	

	

}

void LCD_ExitLTP700WV_RGB(void)
{
	u32 uHSz, uVSz;
			
	LCD_InitBase2((CLK_SRC)LCLK_DOUT_MPLL, PNR_Parallel_RGB, CPU_16BIT);
	LCD_InitWin(RGB24, oLcdc.m_uLcdHSz, oLcdc.m_uLcdVSz, 0, 0, oLcdc.m_uLcdHSz, oLcdc.m_uLcdVSz, 0, 0, LCD_FRAMEBUFFER_ST, WIN0, false);
	LCD_SetWinOnOff(1, WIN0);
	
	LCD_GetFrmSz(&uHSz, &uVSz, WIN0);
	GLIB_InitInstance(LCD_FRAMEBUFFER_ST, uHSz, uVSz, RGB24);
	GLIB_DrawPattern(uHSz, uVSz);

	INTC_SetVectAddr(NUM_LCD1, Isr_LCD_FrameCount);
	LCD_InitInt(LCD_FRAME_INT, LCD_SYSMAIN_INT, FIFO_25, FIFO_INT_WIN0, FINT_VSYNC);
	LCD_EnableInt();
	INTC_Enable(NUM_LCD1);

	GPIO_SetDataEach(eGPIO_N, eGPIO_5 , 0); //GPN[5] -> Low

	LCD_Start();
	
	uFrameCount=0;
	
	while(1)
	{
		if(uFrameCount == 4)
		break;
	}

	LCD_SetAllWinOnOff(0);
	LCD_Stop();
}


//------------------------------------------------------------
// Function Name : LCD_SetI80Timing
//
// Function Desctiption : 
//  
//
// Input : 	
//
// Output :
//
// Version : 
//------------------------------------------------------------
void LCD_SetI80Timing(
	u8 ucCsSetupTime, u8 ucWrSetupTime, u8 ucWrActTime, u8 ucWrHoldTime,
	CPUIF_LDI eLdi)
{
	u32 uI80TimeReg, uI80TimeVal;

	LCD_SetClkSrc(SRC_HCLK);
	LCD_SetClkDirOrDiv(CLK_DIRECT);
	
	uI80TimeReg = (eLdi == MAIN) ? rI80IFCONA0 : rI80IFCONA1;

	uI80TimeVal=Inp32(LCD_BASE+uI80TimeReg);
	uI80TimeVal =
		LCD_CS_SETUP((u32)ucCsSetupTime) |
		LCD_WR_SETUP((u32)ucWrSetupTime) |
		LCD_WR_ACT((u32)ucWrActTime) |
		LCD_WR_HOLD((u32)ucWrHoldTime) |
		RSPOL_HIGH | SUCCEUP_TRIGGERED | SYSIFEN_ENABLE;	//Check the Bit1
	LcdcOutp32(uI80TimeReg, uI80TimeVal);
}


//------------------------------------------------------------
// Function Name : LCD_SetAutoCmdRate
//
// Function Desctiption :
//  
//
// Input :	
//
// Output :
//
// Version : 
//------------------------------------------------------------
void LCD_SetAutoCmdRate(CPU_AUTO_CMD_RATE eCmdRate, CPUIF_LDI eLdi)
{
	u32 uCmdRateVal;
	u32 uI80IfConReg, uI80IfConRegVal;

	uI80IfConReg = (eLdi == MAIN) ? rI80IFCONB0 : rI80IFCONB1;

	uCmdRateVal =
		(eCmdRate == DISABLE_AUTO_FRM) ? (0x0<<0) :
		(eCmdRate == PER_TWO_FRM) ? (0x1<<0) :
		(eCmdRate == PER_FOUR_FRM) ? (0x2<<0) :
		(eCmdRate == PER_SIX_FRM) ? (0x3<<0) :
		(eCmdRate == PER_EIGHT_FRM) ? (0x4<<0) :
		(eCmdRate == PER_TEN_FRM) ? (0x5<<0) :
		(eCmdRate == PER_TWELVE_FRM) ? (0x6<<0) :
		(eCmdRate == PER_FOURTEEN_FRM) ? (0x7<<0) :
		(eCmdRate == PER_SIXTEEN_FRM) ? (0x8<<0) :
		(eCmdRate == PER_EIGHTEEN_FRM) ? (0x9<<0) :
		(eCmdRate == PER_TWENTY_FRM) ? (0xa<<0) :
		(eCmdRate == PER_TWENTY_TWO_FRM) ? (0xb<<0) :
		(eCmdRate == PER_TWENTY_FOUR_FRM) ? (0xc<<0) :
		(eCmdRate == PER_TWENTY_SIX_FRM) ? (0xd<<0) :
		(eCmdRate == PER_TWENTY_EIGHT_FRM) ? (0xe<<0) : (0xf<<0);

	uI80IfConRegVal=Inp32(LCD_BASE+uI80IfConReg);
	uI80IfConRegVal &= ~(0xf<<0);
	uI80IfConRegVal |= uCmdRateVal;

	//uI80IfConRegVal |= 0x2<<5;
	
	LcdcOutp32(uI80IfConReg, uI80IfConRegVal);
}


//------------------------------------------------------------
// Function Name : LCD_InitLTS222QV_CPU
//
// Function Desctiption :
//  
//
// Input :	
//
// Output :
//
// Version : 
//------------------------------------------------------------
void LCD_InitLTS222QV_CPU(CPUIF_LDI eLdiMode)
{
	u32 uCmd[1];
	
	#if (LCD_MODULE_TYPE != LTS222QV_CPU)
		Assert(0);
	#endif
	
	oLcdc.m_bIsNormalCall = false;

	#if 1
	////////// Using Normal Mode ////////// 
	// power funtion 1
	uCmd[0] = 0x2201; LCD_PutCmdToLdi_NormalCmd(uCmd, 1, eLdiMode); Delay(1); // 50us, min. value. is 1

	uCmd[0] = 0x0301; LCD_PutCmdToLdi_NormalCmd(uCmd, 1, eLdiMode); Delay(42);	// 5ms

	//Initial funtion 1
	uCmd[0] = 0x00a0; LCD_PutCmdToLdi_NormalCmd(uCmd, 1, eLdiMode); Delay(1);
	uCmd[0] = 0x0110; LCD_PutCmdToLdi_NormalCmd(uCmd, 1, eLdiMode); Delay(1);
	uCmd[0] = 0x0200; LCD_PutCmdToLdi_NormalCmd(uCmd, 1, eLdiMode); Delay(1);
	uCmd[0] = 0x0510; LCD_PutCmdToLdi_NormalCmd(uCmd, 1, eLdiMode); Delay(1);
	uCmd[0] = 0x0d00; LCD_PutCmdToLdi_NormalCmd(uCmd, 1, eLdiMode); Delay(500);

	//Initial funtion 2
	uCmd[0] = 0x0e00; LCD_PutCmdToLdi_NormalCmd(uCmd, 1, eLdiMode); Delay(1);
	uCmd[0] = 0x0f01; LCD_PutCmdToLdi_NormalCmd(uCmd, 1, eLdiMode); Delay(1);
	uCmd[0] = 0x1000; LCD_PutCmdToLdi_NormalCmd(uCmd, 1, eLdiMode); Delay(1);
	uCmd[0] = 0x1100; LCD_PutCmdToLdi_NormalCmd(uCmd, 1, eLdiMode); Delay(1);
	uCmd[0] = 0x1200; LCD_PutCmdToLdi_NormalCmd(uCmd, 1, eLdiMode); Delay(1);
	uCmd[0] = 0x1300; LCD_PutCmdToLdi_NormalCmd(uCmd, 1, eLdiMode); Delay(1);
	uCmd[0] = 0x1400; LCD_PutCmdToLdi_NormalCmd(uCmd, 1, eLdiMode); Delay(1);
	uCmd[0] = 0x1500; LCD_PutCmdToLdi_NormalCmd(uCmd, 1, eLdiMode); Delay(1);
	uCmd[0] = 0x1600; LCD_PutCmdToLdi_NormalCmd(uCmd, 1, eLdiMode); Delay(1);
	uCmd[0] = 0x1700; LCD_PutCmdToLdi_NormalCmd(uCmd, 1, eLdiMode); Delay(1);
	uCmd[0] = 0x3401; LCD_PutCmdToLdi_NormalCmd(uCmd, 1, eLdiMode); Delay(1);
	uCmd[0] = 0x3500; LCD_PutCmdToLdi_NormalCmd(uCmd, 1, eLdiMode); Delay(500); 

	//Initial funtion 3
	uCmd[0] = 0x8d01; LCD_PutCmdToLdi_NormalCmd(uCmd, 1, eLdiMode); Delay(1);
	uCmd[0] = 0x8b28; LCD_PutCmdToLdi_NormalCmd(uCmd, 1, eLdiMode); Delay(1);
	uCmd[0] = 0x4b00; LCD_PutCmdToLdi_NormalCmd(uCmd, 1, eLdiMode); Delay(1);
	uCmd[0] = 0x4c00; LCD_PutCmdToLdi_NormalCmd(uCmd, 1, eLdiMode); Delay(1);
	uCmd[0] = 0x4d00; LCD_PutCmdToLdi_NormalCmd(uCmd, 1, eLdiMode); Delay(1);
	uCmd[0] = 0x4e00; LCD_PutCmdToLdi_NormalCmd(uCmd, 1, eLdiMode); Delay(1);
	uCmd[0] = 0x4f00; LCD_PutCmdToLdi_NormalCmd(uCmd, 1, eLdiMode); Delay(1);
	uCmd[0] = 0x5000; LCD_PutCmdToLdi_NormalCmd(uCmd, 1, eLdiMode); Delay(1);
	uCmd[0] = 0x8600; LCD_PutCmdToLdi_NormalCmd(uCmd, 1, eLdiMode); Delay(1);
	uCmd[0] = 0x8726; LCD_PutCmdToLdi_NormalCmd(uCmd, 1, eLdiMode); Delay(1);
	uCmd[0] = 0x8802; LCD_PutCmdToLdi_NormalCmd(uCmd, 1, eLdiMode); Delay(1);
	uCmd[0] = 0x8905; LCD_PutCmdToLdi_NormalCmd(uCmd, 1, eLdiMode); Delay(1);
	uCmd[0] = 0x3301; LCD_PutCmdToLdi_NormalCmd(uCmd, 1, eLdiMode); Delay(1);
	uCmd[0] = 0x3701; LCD_PutCmdToLdi_NormalCmd(uCmd, 1, eLdiMode); Delay(1); // modified by kwan. letter
	uCmd[0] = 0x7600; LCD_PutCmdToLdi_NormalCmd(uCmd, 1, eLdiMode); Delay(500);	 

	//Initial funtion 4
	uCmd[0] = 0x4200; LCD_PutCmdToLdi_NormalCmd(uCmd, 1, eLdiMode); Delay(1);
	uCmd[0] = 0x4300; LCD_PutCmdToLdi_NormalCmd(uCmd, 1, eLdiMode); Delay(1);
	uCmd[0] = 0x4400; LCD_PutCmdToLdi_NormalCmd(uCmd, 1, eLdiMode); Delay(1);
	uCmd[0] = 0x4500; LCD_PutCmdToLdi_NormalCmd(uCmd, 1, eLdiMode); Delay(1);
	uCmd[0] = 0x46ef; LCD_PutCmdToLdi_NormalCmd(uCmd, 1, eLdiMode); Delay(1);
	uCmd[0] = 0x4700; LCD_PutCmdToLdi_NormalCmd(uCmd, 1, eLdiMode); Delay(1);
	uCmd[0] = 0x4800; LCD_PutCmdToLdi_NormalCmd(uCmd, 1, eLdiMode); Delay(1);
	uCmd[0] = 0x4901; LCD_PutCmdToLdi_NormalCmd(uCmd, 1, eLdiMode); Delay(1);
	uCmd[0] = 0x4a3f; LCD_PutCmdToLdi_NormalCmd(uCmd, 1, eLdiMode); Delay(1);
	uCmd[0] = 0x3c00; LCD_PutCmdToLdi_NormalCmd(uCmd, 1, eLdiMode); Delay(1);
	uCmd[0] = 0x3d00; LCD_PutCmdToLdi_NormalCmd(uCmd, 1, eLdiMode); Delay(1);
	uCmd[0] = 0x3e01; LCD_PutCmdToLdi_NormalCmd(uCmd, 1, eLdiMode); Delay(1);
	uCmd[0] = 0x3f3f; LCD_PutCmdToLdi_NormalCmd(uCmd, 1, eLdiMode); Delay(1);
	uCmd[0] = 0x4003; LCD_PutCmdToLdi_NormalCmd(uCmd, 1, eLdiMode); Delay(1); // modified by kwan. letter
	uCmd[0] = 0x410a; LCD_PutCmdToLdi_NormalCmd(uCmd, 1, eLdiMode); Delay(500);

	//Initial funtion 5
	uCmd[0] = 0x8f00; LCD_PutCmdToLdi_NormalCmd(uCmd, 1, eLdiMode); Delay(1);
	uCmd[0] = 0x903f; LCD_PutCmdToLdi_NormalCmd(uCmd, 1, eLdiMode); Delay(1);
	uCmd[0] = 0x9133; LCD_PutCmdToLdi_NormalCmd(uCmd, 1, eLdiMode); Delay(1);
	uCmd[0] = 0x9277; LCD_PutCmdToLdi_NormalCmd(uCmd, 1, eLdiMode); Delay(1);
	uCmd[0] = 0x9377; LCD_PutCmdToLdi_NormalCmd(uCmd, 1, eLdiMode); Delay(1);
	uCmd[0] = 0x9417; LCD_PutCmdToLdi_NormalCmd(uCmd, 1, eLdiMode); Delay(1);
	uCmd[0] = 0x9500; LCD_PutCmdToLdi_NormalCmd(uCmd, 1, eLdiMode); Delay(1);
	uCmd[0] = 0x9600; LCD_PutCmdToLdi_NormalCmd(uCmd, 1, eLdiMode); Delay(1);
	uCmd[0] = 0x9733; LCD_PutCmdToLdi_NormalCmd(uCmd, 1, eLdiMode); Delay(1);
	uCmd[0] = 0x9877; LCD_PutCmdToLdi_NormalCmd(uCmd, 1, eLdiMode); Delay(1);
	uCmd[0] = 0x9977; LCD_PutCmdToLdi_NormalCmd(uCmd, 1, eLdiMode); Delay(1);
	uCmd[0] = 0x9a17; LCD_PutCmdToLdi_NormalCmd(uCmd, 1, eLdiMode); Delay(1);
	uCmd[0] = 0x9b00; LCD_PutCmdToLdi_NormalCmd(uCmd, 1, eLdiMode); Delay(1);
	uCmd[0] = 0x9c00; LCD_PutCmdToLdi_NormalCmd(uCmd, 1, eLdiMode); Delay(1);
	uCmd[0] = 0x9d80; LCD_PutCmdToLdi_NormalCmd(uCmd, 1, eLdiMode); Delay(500);
	uCmd[0] = 0x1d08; LCD_PutCmdToLdi_NormalCmd(uCmd, 1, eLdiMode); Delay(500);
	
	//power funtion 2
	uCmd[0] = 0x2300; LCD_PutCmdToLdi_NormalCmd(uCmd, 1, eLdiMode); Delay(5);
	uCmd[0] = 0x2494; LCD_PutCmdToLdi_NormalCmd(uCmd, 1, eLdiMode); Delay(5);
	uCmd[0] = 0x256f; LCD_PutCmdToLdi_NormalCmd(uCmd, 1, eLdiMode); Delay(500); 	// 40ms

	//power funtion 3
	uCmd[0] = 0x2810; LCD_PutCmdToLdi_NormalCmd(uCmd, 1, eLdiMode); Delay(1); // modified by kwan. letter
	uCmd[0] = 0x1a00; LCD_PutCmdToLdi_NormalCmd(uCmd, 1, eLdiMode); Delay(1);
	uCmd[0] = 0x2110; LCD_PutCmdToLdi_NormalCmd(uCmd, 1, eLdiMode); Delay(1);
	uCmd[0] = 0x1825; LCD_PutCmdToLdi_NormalCmd(uCmd, 1, eLdiMode); Delay(500);
	uCmd[0] = 0x1994; LCD_PutCmdToLdi_NormalCmd(uCmd, 1, eLdiMode); Delay(1); // modified by kwan. letter
	uCmd[0] = 0x18e5; LCD_PutCmdToLdi_NormalCmd(uCmd, 1, eLdiMode); Delay(500);	 //40ms
	uCmd[0] = 0x18f7; LCD_PutCmdToLdi_NormalCmd(uCmd, 1, eLdiMode); Delay(500);
	uCmd[0] = 0x1b09; LCD_PutCmdToLdi_NormalCmd(uCmd, 1, eLdiMode); Delay(500);	 //40ms, modified by kwan. letter
	uCmd[0] = 0x1f78; LCD_PutCmdToLdi_NormalCmd(uCmd, 1, eLdiMode); Delay(1);// modified by kwan. letter
	uCmd[0] = 0x2045; LCD_PutCmdToLdi_NormalCmd(uCmd, 1, eLdiMode); Delay(1);
	uCmd[0] = 0x1ec1; LCD_PutCmdToLdi_NormalCmd(uCmd, 1, eLdiMode); Delay(500);
	uCmd[0] = 0x2100; LCD_PutCmdToLdi_NormalCmd(uCmd, 1, eLdiMode); Delay(1);
	uCmd[0] = 0x3b01; LCD_PutCmdToLdi_NormalCmd(uCmd, 1, eLdiMode); Delay(500);	 //10ms
	uCmd[0] = 0x0020; LCD_PutCmdToLdi_NormalCmd(uCmd, 1, eLdiMode); Delay(500);	 //40ms

	#else
	////////// Using Manual Mode //////////  
	uCmd[0] = 0x2201; LCD_PutCmdToLdi_ManualCmd(uCmd, eLdiMode); Delay(1); // 50us, min. value. is 1
	uCmd[0] = 0x0301; LCD_PutCmdToLdi_ManualCmd(uCmd, eLdiMode); Delay(42);	// 5ms

	//Initial funtion 1
	uCmd[0] = 0x00a0; LCD_PutCmdToLdi_ManualCmd(uCmd, eLdiMode); Delay(1);
	uCmd[0] = 0x0110; LCD_PutCmdToLdi_ManualCmd(uCmd, eLdiMode); Delay(1);
	uCmd[0] = 0x0200; LCD_PutCmdToLdi_ManualCmd(uCmd, eLdiMode); Delay(1);
	uCmd[0] = 0x0510; LCD_PutCmdToLdi_ManualCmd(uCmd, eLdiMode); Delay(1);
	uCmd[0] = 0x0d00; LCD_PutCmdToLdi_ManualCmd(uCmd, eLdiMode); Delay(500); //original: 334

	//Initial funtion 2
	uCmd[0] = 0x0e00; LCD_PutCmdToLdi_ManualCmd(uCmd, eLdiMode); Delay(1);
	uCmd[0] = 0x0f01; LCD_PutCmdToLdi_ManualCmd(uCmd, eLdiMode); Delay(1);
	uCmd[0] = 0x1000; LCD_PutCmdToLdi_ManualCmd(uCmd, eLdiMode); Delay(1);
	uCmd[0] = 0x1100; LCD_PutCmdToLdi_ManualCmd(uCmd, eLdiMode); Delay(1);
	uCmd[0] = 0x1200; LCD_PutCmdToLdi_ManualCmd(uCmd, eLdiMode); Delay(1);
	uCmd[0] = 0x1300; LCD_PutCmdToLdi_ManualCmd(uCmd, eLdiMode); Delay(1);
	uCmd[0] = 0x1400; LCD_PutCmdToLdi_ManualCmd(uCmd, eLdiMode); Delay(1);
	uCmd[0] = 0x1500; LCD_PutCmdToLdi_ManualCmd(uCmd, eLdiMode); Delay(1);
	uCmd[0] = 0x1600; LCD_PutCmdToLdi_ManualCmd(uCmd, eLdiMode); Delay(1);
	uCmd[0] = 0x1700; LCD_PutCmdToLdi_ManualCmd(uCmd, eLdiMode); Delay(1);
	uCmd[0] = 0x3401; LCD_PutCmdToLdi_ManualCmd(uCmd, eLdiMode); Delay(1);
	uCmd[0] = 0x3500; LCD_PutCmdToLdi_ManualCmd(uCmd, eLdiMode); Delay(500); 

	//Initial funtion 3
	uCmd[0] = 0x8d01; LCD_PutCmdToLdi_ManualCmd(uCmd, eLdiMode); Delay(1);
	uCmd[0] = 0x8b28; LCD_PutCmdToLdi_ManualCmd(uCmd, eLdiMode); Delay(1);
	uCmd[0] = 0x4b00; LCD_PutCmdToLdi_ManualCmd(uCmd, eLdiMode); Delay(1);
	uCmd[0] = 0x4c00; LCD_PutCmdToLdi_ManualCmd(uCmd, eLdiMode); Delay(1);
	uCmd[0] = 0x4d00; LCD_PutCmdToLdi_ManualCmd(uCmd, eLdiMode); Delay(1);
	uCmd[0] = 0x4e00; LCD_PutCmdToLdi_ManualCmd(uCmd, eLdiMode); Delay(1);
	uCmd[0] = 0x4f00; LCD_PutCmdToLdi_ManualCmd(uCmd, eLdiMode); Delay(1);
	uCmd[0] = 0x5000; LCD_PutCmdToLdi_ManualCmd(uCmd, eLdiMode); Delay(1);
	uCmd[0] = 0x8600; LCD_PutCmdToLdi_ManualCmd(uCmd, eLdiMode); Delay(1);
	uCmd[0] = 0x8726; LCD_PutCmdToLdi_ManualCmd(uCmd, eLdiMode); Delay(1);
	uCmd[0] = 0x8802; LCD_PutCmdToLdi_ManualCmd(uCmd, eLdiMode); Delay(1);
	uCmd[0] = 0x8905; LCD_PutCmdToLdi_ManualCmd(uCmd, eLdiMode); Delay(1);
	uCmd[0] = 0x3301; LCD_PutCmdToLdi_ManualCmd(uCmd, eLdiMode); Delay(1);
	uCmd[0] = 0x3701; LCD_PutCmdToLdi_ManualCmd(uCmd, eLdiMode); Delay(1); // modified by kwan. letter
	uCmd[0] = 0x7600; LCD_PutCmdToLdi_ManualCmd(uCmd, eLdiMode); Delay(500);	

	//Initial funtion 4
	uCmd[0] = 0x4200; LCD_PutCmdToLdi_ManualCmd(uCmd, eLdiMode); Delay(1);
	uCmd[0] = 0x4300; LCD_PutCmdToLdi_ManualCmd(uCmd, eLdiMode); Delay(1);
	uCmd[0] = 0x4400; LCD_PutCmdToLdi_ManualCmd(uCmd, eLdiMode); Delay(1);
	uCmd[0] = 0x4500; LCD_PutCmdToLdi_ManualCmd(uCmd, eLdiMode); Delay(1);
	uCmd[0] = 0x46ef; LCD_PutCmdToLdi_ManualCmd(uCmd, eLdiMode); Delay(1);
	uCmd[0] = 0x4700; LCD_PutCmdToLdi_ManualCmd(uCmd, eLdiMode); Delay(1);
	uCmd[0] = 0x4800; LCD_PutCmdToLdi_ManualCmd(uCmd, eLdiMode); Delay(1);
	uCmd[0] = 0x4901; LCD_PutCmdToLdi_ManualCmd(uCmd, eLdiMode); Delay(1);
	uCmd[0] = 0x4a3f; LCD_PutCmdToLdi_ManualCmd(uCmd, eLdiMode); Delay(1);
	uCmd[0] = 0x3c00; LCD_PutCmdToLdi_ManualCmd(uCmd, eLdiMode); Delay(1);
	uCmd[0] = 0x3d00; LCD_PutCmdToLdi_ManualCmd(uCmd, eLdiMode); Delay(1);
	uCmd[0] = 0x3e01; LCD_PutCmdToLdi_ManualCmd(uCmd, eLdiMode); Delay(1);
	uCmd[0] = 0x3f3f; LCD_PutCmdToLdi_ManualCmd(uCmd, eLdiMode); Delay(1);
	uCmd[0] = 0x4003; LCD_PutCmdToLdi_ManualCmd(uCmd, eLdiMode); Delay(1); // modified by kwan. letter
	uCmd[0] = 0x410a; LCD_PutCmdToLdi_ManualCmd(uCmd, eLdiMode); Delay(500);

	//Initial funtion 5
	uCmd[0] = 0x8f00; LCD_PutCmdToLdi_ManualCmd(uCmd, eLdiMode); Delay(1);
	uCmd[0] = 0x903f; LCD_PutCmdToLdi_ManualCmd(uCmd, eLdiMode); Delay(1);
	uCmd[0] = 0x9133; LCD_PutCmdToLdi_ManualCmd(uCmd, eLdiMode); Delay(1);
	uCmd[0] = 0x9277; LCD_PutCmdToLdi_ManualCmd(uCmd, eLdiMode); Delay(1);
	uCmd[0] = 0x9377; LCD_PutCmdToLdi_ManualCmd(uCmd, eLdiMode); Delay(1);
	uCmd[0] = 0x9417; LCD_PutCmdToLdi_ManualCmd(uCmd, eLdiMode); Delay(1);
	uCmd[0] = 0x9500; LCD_PutCmdToLdi_ManualCmd(uCmd, eLdiMode); Delay(1);
	uCmd[0] = 0x9600; LCD_PutCmdToLdi_ManualCmd(uCmd, eLdiMode); Delay(1);
	uCmd[0] = 0x9733; LCD_PutCmdToLdi_ManualCmd(uCmd, eLdiMode); Delay(1);
	uCmd[0] = 0x9877; LCD_PutCmdToLdi_ManualCmd(uCmd, eLdiMode); Delay(1);
	uCmd[0] = 0x9977; LCD_PutCmdToLdi_ManualCmd(uCmd, eLdiMode); Delay(1);
	uCmd[0] = 0x9a17; LCD_PutCmdToLdi_ManualCmd(uCmd, eLdiMode); Delay(1);
	uCmd[0] = 0x9b00; LCD_PutCmdToLdi_ManualCmd(uCmd, eLdiMode); Delay(1);
	uCmd[0] = 0x9c00; LCD_PutCmdToLdi_ManualCmd(uCmd, eLdiMode); Delay(1);
	uCmd[0] = 0x9d80; LCD_PutCmdToLdi_ManualCmd(uCmd, eLdiMode); Delay(500);
	uCmd[0] = 0x1d08; LCD_PutCmdToLdi_ManualCmd(uCmd, eLdiMode); Delay(500);
	
	//power funtion 2
	uCmd[0] = 0x2300; LCD_PutCmdToLdi_ManualCmd(uCmd, eLdiMode); Delay(5);
	uCmd[0] = 0x2494; LCD_PutCmdToLdi_ManualCmd(uCmd, eLdiMode); Delay(5);
	uCmd[0] = 0x256f; LCD_PutCmdToLdi_ManualCmd(uCmd, eLdiMode); Delay(500); 	// 40ms

	//power funtion 3
	uCmd[0] = 0x2810; LCD_PutCmdToLdi_ManualCmd(uCmd, eLdiMode); Delay(1); // modified by kwan. letter
	uCmd[0] = 0x1a00; LCD_PutCmdToLdi_ManualCmd(uCmd, eLdiMode); Delay(1);
	uCmd[0] = 0x2110; LCD_PutCmdToLdi_ManualCmd(uCmd, eLdiMode); Delay(1);
	uCmd[0] = 0x1825; LCD_PutCmdToLdi_ManualCmd(uCmd, eLdiMode); Delay(500);
	uCmd[0] = 0x1994; LCD_PutCmdToLdi_ManualCmd(uCmd, eLdiMode); Delay(1); // modified by kwan. letter
	uCmd[0] = 0x18e5; LCD_PutCmdToLdi_ManualCmd(uCmd, eLdiMode); Delay(500);	 //40ms
	uCmd[0] = 0x18f7; LCD_PutCmdToLdi_ManualCmd(uCmd, eLdiMode); Delay(500);
	uCmd[0] = 0x1b09; LCD_PutCmdToLdi_ManualCmd(uCmd, eLdiMode); Delay(500);	 //40ms, modified by kwan. letter
	uCmd[0] = 0x1f78; LCD_PutCmdToLdi_ManualCmd(uCmd, eLdiMode); Delay(1);// modified by kwan. letter
	uCmd[0] = 0x2045; LCD_PutCmdToLdi_ManualCmd(uCmd, eLdiMode); Delay(1);
	uCmd[0] = 0x1ec1; LCD_PutCmdToLdi_ManualCmd(uCmd, eLdiMode); Delay(500);
	uCmd[0] = 0x2100; LCD_PutCmdToLdi_ManualCmd(uCmd, eLdiMode); Delay(1);
	uCmd[0] = 0x3b01; LCD_PutCmdToLdi_ManualCmd(uCmd, eLdiMode); Delay(500);	 //10ms
	uCmd[0] = 0x0020; LCD_PutCmdToLdi_ManualCmd(uCmd, eLdiMode); Delay(500);	 //40ms
	#endif
}


void LCD_InitLTS222QV_SRGB(void)
{
	//Select LCD IF Pin Configure
//	GPIO_SetLCDType(eHOSTIF);
	GPIO_SetLCDType(eRGBIF);	

	//Set LCD GPIO Port
	GPIO_SetFunctionAll(eGPIO_I, 0xaaaaaaaa, 2); //GPI[15..0]-> SYS VD[15..0]
	GPIO_SetFunctionAll(eGPIO_J, 0xaaaaaaaa, 2); 
	//GPJ[1..0]-> SYS VD[17..16], GPJ[11..6]-> SYS_VSYNC, SYS_OEn, SYS_CSn_main, SYS_CSn_sub, SYS_RS, SYS_WEn

	/*
	GPIO_SetDataEach(eGPIO_N, eGPIO_5 , 1); //GPN[5] -> High
	GPIO_SetFunctionEach(eGPIO_N, eGPIO_5, 1); //GPN[5] -> Output
	Delay(100);
	GPIO_SetDataEach(eGPIO_N, eGPIO_5 , 0); //GPN[5] -> Low
	Delay(1000); 
	GPIO_SetDataEach(eGPIO_N, eGPIO_5 , 1); //GPN[5] -> High
	Delay(100); */
	
	///////////////////////////////////////////////////////////////////
	// Power Setting Function 1
	//////////////////////////////////////////////////////////////////
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS, 0x22, 0x01);	 // PARTIAL 2 DISPLAY AREA RASTER-ROW NUMBER REGISTER 1
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS, 0x03, 0x01);	 // RESET REGISTER

	///////////////////////////////////////////////////////////////////
	// Initializing Function 1
	///////////////////////////////////////////////////////////////////
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x00,0xa0); Delay(5); // CONTROL REGISTER 1, delay about 300ns
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x01,0x10); Delay(5); // CONTROL REGISTER 2, delay about 300ns
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x02,0x00); Delay(5);	// RGB INTERFACE REGISTER
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x05,0x00); Delay(5);	// DATA ACCESS CONTROL REGISTER
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x0D,0x00); Delay(400); // delay about 40ms

	///////////////////////////////////////////////////////////////////
	// Initializing Function 2
	///////////////////////////////////////////////////////////////////
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x0E,0x00); Delay(5);  // delay about 300ns
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x0F,0x00); Delay(5);  // delay about 300ns
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x10,0x00); Delay(5);  // delay about 300ns
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x11,0x00); Delay(5);  // delay about 300ns
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x12,0x00); Delay(5);  // delay about 300ns 
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x13,0x00); Delay(5); // DISPLAY SIZE CONTROL REGISTER
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x14,0x00); Delay(5); // PARTIAL-OFF AREA COLOR REGISTER 1
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x15,0x00); Delay(5); // PARTIAL-OFF AREA COLOR REGISTER 2
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x16,0x00); Delay(5);  // PARTIAL 1 DISPLAY AREA STARTING REGISTER 1
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x17,0x00); Delay(5); // PARTIAL 1 DISPLAY AREA STARTING REGISTER 2
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x34,0x01); Delay(5); // POWER SUPPLY SYSTEM CONTROL REGISTER 14
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x35,0x00); Delay(400); // POWER SUPPLY SYSTEM CONTROL REGISTER 7

	////////////////////////////////////////////////////////////////////
	// Initializing Function 3
	////////////////////////////////////////////////////////////////////
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x8D,0x01); Delay(5);  // delay about 300ns
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x8B,0x28); Delay(5);  // delay about 300ns
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x4B,0x00); Delay(5);  // delay about 300ns
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x4C,0x00); Delay(5);  // delay about 300ns
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x4D,0x00); Delay(5);  // delay about 300ns
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x4E,0x00); Delay(5);  // delay about 300ns
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x4F,0x00); Delay(5);  // delay about 300ns
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x50,0x00); Delay(500);  //  ID CODE REGISTER 2, Check it out, delay about 50 ms
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x86,0x00); Delay(5);  // delay about 300ns
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x87,0x26); Delay(5);  // delay about 300ns
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x88,0x02); Delay(5);  // delay about 300ns
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x89,0x05); Delay(5);  // delay about 300ns
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x33,0x01); Delay(5);  //  POWER SUPPLY SYSTEM CONTROL REGISTER 13
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x37,0x06); Delay(500); //  POWER SUPPLY SYSTEM CONTROL REGISTER 12, Check it out
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x76,0x00); Delay(400); //  SCROLL AREA START REGISTER 2, delay about 30ms

	/////////////////////////////////////////////////////////////////////
	// Initializing Function 4
	/////////////////////////////////////////////////////////////////////
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x42,0x00); Delay(5); // delay about 300ns
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x43,0x00); Delay(5); // delay about 300ns
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x44,0x00); Delay(5); // delay about 300ns
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x45,0x00); Delay(5); //  CALIBRATION REGISTER
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x46,0xef); Delay(5);
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x47,0x00); Delay(5);
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x48,0x00); Delay(5);
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x49,0x01); Delay(500);  //  ID CODE REGISTER 1							check it out
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x4A,0x3f); Delay(5);  // delay about 300ns
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x3C,0x00); Delay(5);  // delay about 300ns
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x3D,0x00); Delay(5);  // delay about 300ns
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x3E,0x01); Delay(5);  // delay about 300ns
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x3F,0x3f); Delay(5);  // delay about 300ns
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x40,0x01); Delay(5);  // delay about 300ns, horizontal back porch, 050105 Boaz.Kim
	//	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x40,0x02); Delay(5); //	horizontal back porch	 //050105 Boaz.Kim
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x41,0x0a); Delay(5); //	vertical back porch
	//	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x41,0x08); Delay(5); //	vertical back porch

	#if 0 /// 6.15
		LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x8f, 0x05); Delay(400); // Gamma adjustment (upper amplitude-positive) (default: 0x05)
	#else
		LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x8F,0x3f); Delay(400); // this value is more comfortable to look
	#endif
	 
	/////////////////////////////////////////////////////////////////////
	// Initializing Function 5
	/////////////////////////////////////////////////////////////////////
	#if 0 
		// Gamma adjustment regs
		LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x90,0x05); Delay(5);  // delay about 300ns
		LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x91,0x44); Delay(5);  // delay about 300ns
		LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x92,0x44); Delay(5);  // delay about 300ns
		LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x93,0x44); Delay(5);  // delay about 300ns
		LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x94,0x33); Delay(5);  // delay about 300ns
		LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x95,0x05); Delay(5);  // delay about 300ns
		LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x96,0x05); Delay(5);  // delay about 300ns
		LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x97,0x44); Delay(5);  // delay about 300ns
		LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x98,0x44); Delay(5);  // delay about 300ns
		LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x99,0x44); Delay(5);  // delay about 300ns
		LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x9A,0x33); Delay(5);  // delay about 300ns
		LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x9B,0x33); Delay(5);  // delay about 300ns
		LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x9C,0x33); Delay(5);  // delay about 300ns
	#else  // this value is more comfortable to look
		LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x90,0x3f); Delay(5);  // delay about 300ns
		LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x91,0x33); Delay(5);  // delay about 300ns
		LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x92,0x77); Delay(5);  // delay about 300ns
		LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x93,0x77); Delay(5);  // delay about 300ns
		LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x94,0x17); Delay(5);  // delay about 300ns
		LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x95,0x3f); Delay(5);  // delay about 300ns
		LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x96,0x00); Delay(5);  // delay about 300ns
		LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x97,0x33); Delay(5);  // delay about 300ns
		LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x98,0x77); Delay(5);  // delay about 300ns
		LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x99,0x77); Delay(5);  // delay about 300ns
		LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x9A,0x17); Delay(5);  // delay about 300ns
		LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x9B,0x07); Delay(5);  // delay about 300ns
		LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x9C,0x07); Delay(5);  // delay about 300ns
	#endif
	
	#if (LCD_MODULE_TYPE == LTS222QV_SRGB)
		LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS, 0x9D, 0x81); Delay(400); //	6bit*3transfer RGB
	#else 
		Assert(0);
	#endif

	/////////////////////////////////////////////////////////////////////
	// Power Setting 2
	/////////////////////////////////////////////////////////////////////
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x1D,0x08); Delay(400); // delay about 50 us
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x23,0x00); Delay(500); //  PARTIAL 2 DISPLAY AREA RASTER-ROW NUMBER REGISTER 2
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x24,0x94); Delay(500); //  POWER SUPPLY SYSTEM CONTROL REGISTER 1
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x25,0x6f); Delay(400);  //  POWER SUPPLY SYSTEM CONTROL REGISTER 2

	/////////////////////////////////////////////////////////////////////
	// Power Setting 3
	/////////////////////////////////////////////////////////////////////
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x28,0x1e);  // 
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x1A,0x00);  // 
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x21,0x10);  //  PARTIAL 1 DISPLAY AREA RASTER-ROW NUMBER REGISTER 2 
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x18,0x25);  //  PARTIAL 2 DISPLAY AREA STARTING REGISTER 1

	// delay about 40ms
	Delay(400);					

	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x19,0x48);  //  PARTIAL 2 DISPLAY AREA STARTING REGISTER 2
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x18,0xe5);  //  PARTIAL 2 DISPLAY AREA STARTING REGISTER 1

	// delay about 10ms
	Delay(400);					

	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x18,0xF7);  //  PARTIAL 2 DISPLAY AREA STARTING REGISTER 1 

	// delay about 40ms
	Delay(400);		
	
	#if 0 /// 6.15 best look
		LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x1B,0x07);  // VS regulator ON at 4.5V

		// delay about 40ms
		Delay(400); 				

		LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x1F,0x5a);
		LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x20,0x54);
		LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x1E,0xc1);
	#elif 0 /// 6.15 
		LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x1B,0x01);  // 0x01 - VS regulator ON at 3.5V // 0x03 - ON at 4.0V
		// delay about 40ms
		Delay(400); 				
		LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x1F,0x5e); // Specify the VCOM1 amplitude between 0x33 and 0xb4.
		LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x20,0x5f); // Specify the VCOM1 center voltage between 0x07 and 0xbd.
		LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x1E,0xc1); // VCOM1: Normal operation, use the boosting circuit, use the extra CP1(twin CP1)
	#else
		LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x1B,0x07);  // org
		// delay about 80ms
		Delay(400); 				
		LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x1F,0x68); // Specify the VCOM1 amplitude between 0x33 and 0xb4.
		LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x20,0x45);  // Specify the VCOM1 center voltage between 0x07 and 0xbd.
		LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x1E,0xc1);  // VCOM1: Normal operation, use the boosting circuit, use the extra CP1(twin CP1) 
	#endif

	// delay about 10ms
	Delay(400);					

	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x21,0x00);  //  PARTIAL 1 DISPLAY AREA RASTER-ROW NUMBER REGISTER 2 
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x3B,0x01);  // 

	// delay about 20ms
	Delay(400);					

	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x00,0x20);  //  CONTROL REGISTER 1
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x02,0x01);  //  RGB INTERFACE REGISTER
	
	// delay about 10ms
	Delay(400);	
	
}

void LCD_InitLTS222QV_PRGB(void)
{
	UART_Printf("LCD Setting\n");
	//Select LCD IF Pin Configure
//PIO_SetLCDType(eHOSTIF);
	GPIO_SetLCDType(eRGBIF);	

	//Set LCD GPIO Port
	GPIO_SetFunctionAll(eGPIO_I, 0xaaaaaaaa, 2); //GPI[15..0]-> SYS VD[15..0]
	GPIO_SetFunctionAll(eGPIO_J, 0xaaaaaaaa, 2); 
	//GPJ[1..0]-> SYS VD[17..16], GPJ[11..6]-> SYS_VSYNC, SYS_OEn, SYS_CSn_main, SYS_CSn_sub, SYS_RS, SYS_WEn

	/*
	GPIO_SetDataEach(eGPIO_N, eGPIO_5 , 1); //GPN[5] -> High
	GPIO_SetFunctionEach(eGPIO_N, eGPIO_5, 1); //GPN[5] -> Output
	Delay(100);
	GPIO_SetDataEach(eGPIO_N, eGPIO_5 , 0); //GPN[5] -> Low
	Delay(1000); 
	GPIO_SetDataEach(eGPIO_N, eGPIO_5 , 1); //GPN[5] -> High
	Delay(100); */
	
	///////////////////////////////////////////////////////////////////
	// Power Setting Function 1
	//////////////////////////////////////////////////////////////////
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS, 0x22, 0x01);	 // PARTIAL 2 DISPLAY AREA RASTER-ROW NUMBER REGISTER 1
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS, 0x03, 0x01);	 // RESET REGISTER

	///////////////////////////////////////////////////////////////////
	// Initializing Function 1
	///////////////////////////////////////////////////////////////////
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x00,0xa0); Delay(5); // CONTROL REGISTER 1, delay about 300ns
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x01,0x10); Delay(5); // CONTROL REGISTER 2, delay about 300ns
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x02,0x00); Delay(5);	// RGB INTERFACE REGISTER
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x05,0x00); Delay(5);	// DATA ACCESS CONTROL REGISTER
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x0D,0x00); Delay(400); // delay about 40ms

	///////////////////////////////////////////////////////////////////
	// Initializing Function 2
	///////////////////////////////////////////////////////////////////
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x0E,0x00); Delay(5);  // delay about 300ns
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x0F,0x00); Delay(5);  // delay about 300ns
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x10,0x00); Delay(5);  // delay about 300ns
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x11,0x00); Delay(5);  // delay about 300ns
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x12,0x00); Delay(5);  // delay about 300ns 
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x13,0x00); Delay(5); // DISPLAY SIZE CONTROL REGISTER
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x14,0x00); Delay(5); // PARTIAL-OFF AREA COLOR REGISTER 1
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x15,0x00); Delay(5); // PARTIAL-OFF AREA COLOR REGISTER 2
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x16,0x00); Delay(5);  // PARTIAL 1 DISPLAY AREA STARTING REGISTER 1
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x17,0x00); Delay(5); // PARTIAL 1 DISPLAY AREA STARTING REGISTER 2
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x34,0x01); Delay(5); // POWER SUPPLY SYSTEM CONTROL REGISTER 14
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x35,0x00); Delay(400); // POWER SUPPLY SYSTEM CONTROL REGISTER 7

	////////////////////////////////////////////////////////////////////
	// Initializing Function 3
	////////////////////////////////////////////////////////////////////
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x8D,0x01); Delay(5);  // delay about 300ns
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x8B,0x28); Delay(5);  // delay about 300ns
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x4B,0x00); Delay(5);  // delay about 300ns
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x4C,0x00); Delay(5);  // delay about 300ns
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x4D,0x00); Delay(5);  // delay about 300ns
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x4E,0x00); Delay(5);  // delay about 300ns
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x4F,0x00); Delay(5);  // delay about 300ns
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x50,0x00); Delay(500);  //  ID CODE REGISTER 2, Check it out, delay about 50 ms
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x86,0x00); Delay(5);  // delay about 300ns
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x87,0x26); Delay(5);  // delay about 300ns
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x88,0x02); Delay(5);  // delay about 300ns
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x89,0x05); Delay(5);  // delay about 300ns
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x33,0x01); Delay(5);  //  POWER SUPPLY SYSTEM CONTROL REGISTER 13
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x37,0x06); Delay(500); //  POWER SUPPLY SYSTEM CONTROL REGISTER 12, Check it out
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x76,0x00); Delay(400); //  SCROLL AREA START REGISTER 2, delay about 30ms

	/////////////////////////////////////////////////////////////////////
	// Initializing Function 4
	/////////////////////////////////////////////////////////////////////
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x42,0x00); Delay(5); // delay about 300ns
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x43,0x00); Delay(5); // delay about 300ns
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x44,0x00); Delay(5); // delay about 300ns
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x45,0x00); Delay(5); //  CALIBRATION REGISTER
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x46,0xef); Delay(5);
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x47,0x00); Delay(5);
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x48,0x00); Delay(5);
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x49,0x01); Delay(500);  //  ID CODE REGISTER 1							check it out
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x4A,0x3f); Delay(5);  // delay about 300ns
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x3C,0x00); Delay(5);  // delay about 300ns
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x3D,0x00); Delay(5);  // delay about 300ns
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x3E,0x01); Delay(5);  // delay about 300ns
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x3F,0x3f); Delay(5);  // delay about 300ns
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x40,0x01); Delay(5);  // delay about 300ns, horizontal back porch, 050105 Boaz.Kim
	//	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x40,0x02); Delay(5); //	horizontal back porch	 //050105 Boaz.Kim
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x41,0x0a); Delay(5); //	vertical back porch
	//	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x41,0x08); Delay(5); //	vertical back porch

	#if 0 /// 6.15
		LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x8f, 0x05); Delay(400); // Gamma adjustment (upper amplitude-positive) (default: 0x05)
	#else
		LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x8F,0x3f); Delay(400); // this value is more comfortable to look
	#endif
	 
	/////////////////////////////////////////////////////////////////////
	// Initializing Function 5
	/////////////////////////////////////////////////////////////////////
	#if 0 
		// Gamma adjustment regs
		LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x90,0x05); Delay(5);  // delay about 300ns
		LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x91,0x44); Delay(5);  // delay about 300ns
		LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x92,0x44); Delay(5);  // delay about 300ns
		LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x93,0x44); Delay(5);  // delay about 300ns
		LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x94,0x33); Delay(5);  // delay about 300ns
		LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x95,0x05); Delay(5);  // delay about 300ns
		LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x96,0x05); Delay(5);  // delay about 300ns
		LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x97,0x44); Delay(5);  // delay about 300ns
		LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x98,0x44); Delay(5);  // delay about 300ns
		LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x99,0x44); Delay(5);  // delay about 300ns
		LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x9A,0x33); Delay(5);  // delay about 300ns
		LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x9B,0x33); Delay(5);  // delay about 300ns
		LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x9C,0x33); Delay(5);  // delay about 300ns
	#else  // this value is more comfortable to look
		LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x90,0x3f); Delay(5);  // delay about 300ns
		LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x91,0x33); Delay(5);  // delay about 300ns
		LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x92,0x77); Delay(5);  // delay about 300ns
		LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x93,0x77); Delay(5);  // delay about 300ns
		LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x94,0x17); Delay(5);  // delay about 300ns
		LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x95,0x3f); Delay(5);  // delay about 300ns
		LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x96,0x00); Delay(5);  // delay about 300ns
		LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x97,0x33); Delay(5);  // delay about 300ns
		LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x98,0x77); Delay(5);  // delay about 300ns
		LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x99,0x77); Delay(5);  // delay about 300ns
		LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x9A,0x17); Delay(5);  // delay about 300ns
		LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x9B,0x07); Delay(5);  // delay about 300ns
		LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x9C,0x07); Delay(5);  // delay about 300ns
	#endif
	
	#if (LCD_MODULE_TYPE == LTS222QV_PRGB)
		LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS, 0x9D, 0x80); Delay(400); //	6bit*3transfer RGB
	#endif

	/////////////////////////////////////////////////////////////////////
	// Power Setting 2
	/////////////////////////////////////////////////////////////////////
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x1D,0x08); Delay(400); // delay about 50 us
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x23,0x00); Delay(500); //  PARTIAL 2 DISPLAY AREA RASTER-ROW NUMBER REGISTER 2
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x24,0x94); Delay(500); //  POWER SUPPLY SYSTEM CONTROL REGISTER 1
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x25,0x6f); Delay(400);  //  POWER SUPPLY SYSTEM CONTROL REGISTER 2

	/////////////////////////////////////////////////////////////////////
	// Power Setting 3
	/////////////////////////////////////////////////////////////////////
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x28,0x1e);  // 
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x1A,0x00);  // 
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x21,0x10);  //  PARTIAL 1 DISPLAY AREA RASTER-ROW NUMBER REGISTER 2 
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x18,0x25);  //  PARTIAL 2 DISPLAY AREA STARTING REGISTER 1

	// delay about 40ms
	Delay(400);					

	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x19,0x48);  //  PARTIAL 2 DISPLAY AREA STARTING REGISTER 2
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x18,0xe5);  //  PARTIAL 2 DISPLAY AREA STARTING REGISTER 1

	// delay about 10ms
	Delay(400);					

	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x18,0xF7);  //  PARTIAL 2 DISPLAY AREA STARTING REGISTER 1 

	// delay about 40ms
	Delay(400);		
	
	#if 0 /// 6.15 best look
		LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x1B,0x07);  // VS regulator ON at 4.5V

		// delay about 40ms
		Delay(400); 				

		LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x1F,0x5a);
		LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x20,0x54);
		LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x1E,0xc1);
	#elif 0 /// 6.15 
		LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x1B,0x01);  // 0x01 - VS regulator ON at 3.5V // 0x03 - ON at 4.0V
		// delay about 40ms
		Delay(400); 				
		LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x1F,0x5e); // Specify the VCOM1 amplitude between 0x33 and 0xb4.
		LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x20,0x5f); // Specify the VCOM1 center voltage between 0x07 and 0xbd.
		LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x1E,0xc1); // VCOM1: Normal operation, use the boosting circuit, use the extra CP1(twin CP1)
	#else
		LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x1B,0x07);  // org
		// delay about 80ms
		Delay(400); 				
		LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x1F,0x68); // Specify the VCOM1 amplitude between 0x33 and 0xb4.
		LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x20,0x45);  // Specify the VCOM1 center voltage between 0x07 and 0xbd.
		LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x1E,0xc1);  // VCOM1: Normal operation, use the boosting circuit, use the extra CP1(twin CP1) 
	#endif

	// delay about 10ms
	Delay(400);					

	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x21,0x00);  //  PARTIAL 1 DISPLAY AREA RASTER-ROW NUMBER REGISTER 2 
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x3B,0x01);  // 

	// delay about 20ms
	Delay(400);					

	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x00,0x20);  //  CONTROL REGISTER 1
	LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x02,0x01);  //  RGB INTERFACE REGISTER
	
	// delay about 10ms
	Delay(400);	
	
}



//------------------------------------------------------------
// Function Name : LCD_InitStartPosOnLcd
//
// Function Desctiption :
//  
//
// Input :	
//
// Output :
//
// Version : 
//------------------------------------------------------------
void LCD_InitStartPosOnLcd(CPUIF_LDI eLdiMode)
{
	#if (LCD_MODULE_TYPE == LTS222QV_CPU)
	u32 uVal[3];
	#endif

	#if (LCD_MODULE_TYPE == LTV350QV_RGB)
		return;
	#elif (LCD_MODULE_TYPE == LTS222QV_CPU)
	
		uVal[0] = 0x4400;
		uVal[1] = 0x4200;
		uVal[2] = 0x4300;
		LCD_PutCmdToLdi_NormalCmd(uVal, 3, eLdiMode);	//Check if this code is valid
	
	#elif (LCD_MODULE_TYPE == LTS222QV_SRGB)
		// start addr setting
		LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x44, 0x00);		// y addr 2
		LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x42, 0x00);		// x addr
		LCD_WriteCmd((CPUIF_LDI)LCD_CPUIF_CS,  0x43, 0x00);		// y addr 1

		Delay(100);

		//Select LCD IF Pin Configure
		GPIO_SetLCDType(eRGBIF);
		
		//Set LCD GPIO Port
		GPIO_SetFunctionAll(eGPIO_I, 0xaaaaaaaa, 2); //GPI[15..0]-> RGB VD[15..0]
		GPIO_SetFunctionAll(eGPIO_J, 0xaaaaaaaa, 2); //GPJ[7..0]-> RGB VD[23..16], GPJ[11..8]-> VCLK, VDEN, VSYNC, HSYNC

	#elif (LCD_MODULE_TYPE == LTS222QV_PRGB)
		Delay(100);

		//Select LCD IF Pin Configure
		GPIO_SetLCDType(eRGBIF);
		
		//Set LCD GPIO Port
		GPIO_SetFunctionAll(eGPIO_I, 0xaaaaaaaa, 2); //GPI[15..0]-> RGB VD[15..0]
		GPIO_SetFunctionAll(eGPIO_J, 0xaaaaaaaa, 2); //GPJ[7..0]-> RGB VD[23..16], GPJ[11..8]-> VCLK, VDEN, VSYNC, HSYNC


	#else
		Assert(0);
	#endif
}


//------------------------------------------------------------
// Function Name : LCD_SetViewPosOnLcd
//
// Function Desctiption :
//  This function set the View-Port's position of LCD
//
// Input :	uX_Lcd, view x offset of lcd
//			uY_Lcd, view y offset of lcd
//			eWin, selected window number
//
// Output :
//
// Version : 
//------------------------------------------------------------
void LCD_SetViewPosOnLcd(u32 uX_Lcd, u32 uY_Lcd, LCD_WINDOW eWin) // For WIN0~4,
{
	u32 uDiv = oLcdc.m_uBytes/oLcdc.m_uBits;
	u32 uX_Lcd1, uX_Lcd2, uY_Lcd2;

	u32 uTempLeftReg, uTempRightReg;
	
	Assert(eWin != WIN0);

	uTempLeftReg =
		(eWin == WIN0) ? rVIDOSD0A :
		(eWin == WIN1) ? rVIDOSD1A :
		(eWin == WIN2) ? rVIDOSD2A :
		(eWin == WIN3) ? rVIDOSD3A : rVIDOSD4A;


	uTempRightReg =
		(eWin == WIN0) ? rVIDOSD0B :
		(eWin == WIN1) ? rVIDOSD1B :
		(eWin == WIN2) ? rVIDOSD2B :
		(eWin == WIN3) ? rVIDOSD3B : rVIDOSD4B;

	if ((uX_Lcd+oLcdc.m_uViewHSz[eWin] < oLcdc.m_uLcdHSz+1) && (uY_Lcd+oLcdc.m_uViewVSz[eWin] < oLcdc.m_uLcdVSz+1))
	{
		uX_Lcd1 = (uX_Lcd/uDiv)*uDiv;

		uX_Lcd2 = uX_Lcd1 + oLcdc.m_uViewHSz[eWin] - 1;
		uY_Lcd2 = uY_Lcd + oLcdc.m_uViewVSz[eWin] - 1;

		LcdcOutp32( uTempLeftReg, OSD_LTX_F(uX_Lcd1) | OSD_LTY_F(uY_Lcd) );
		LcdcOutp32( uTempRightReg,OSD_RBX_F(uX_Lcd2) | OSD_RBY_F(uY_Lcd2) );
	}
}

//------------------------------------------------------------
// Function Name : LCD_SetViewPosOnFrame
//
// Function Desctiption :
//  This function set the View-Port's position of Frame
//
// Input :	uX_Frame, view x offset of frame
//			uY_Frame, view y offset of frame
//			eWin, selected window number
//
// Output :
//
// Version : 
//------------------------------------------------------------
void LCD_SetViewPosOnFrame(u32 uX_Frame, u32 uY_Frame, LCD_WINDOW eWin) // For WIN0~4
{
	u32 uFbStAddrReg;

	uFbStAddrReg =
		(eWin == WIN0) ? rVIDW00ADD0B0 :
		(eWin == WIN1) ? rVIDW01ADD0B0 :
		(eWin == WIN2) ? rVIDW02ADD0 :
		(eWin == WIN3) ? rVIDW03ADD0 : rVIDW04ADD0;

	if ( (uX_Frame < oLcdc.m_uFrmHSz[eWin] - oLcdc.m_uLcdHSz + 1) && (uY_Frame < oLcdc.m_uFrmVSz[eWin] - oLcdc.m_uLcdVSz + 1) )
	{
		LcdcOutp32(uFbStAddrReg,
			oLcdc.m_uFbStAddr[eWin] + oLcdc.m_uFrmHSz[eWin]*uY_Frame*(oLcdc.m_uBytes/oLcdc.m_uBits) + uX_Frame*(oLcdc.m_uBytes/oLcdc.m_uBits));

		if (eWin == WIN0)
			LcdcOutp32(rVIDW00ADD0B1,
				oLcdc.m_uDoubleBufStAddr[0] + oLcdc.m_uFrmHSz[eWin]*uY_Frame*(oLcdc.m_uBytes/oLcdc.m_uBits) + uX_Frame*(oLcdc.m_uBytes/oLcdc.m_uBits));

	}
}


//------------------------------------------------------------
// Function Name : LCD_SetViewPosOnFrame
//
// Function Desctiption :
// Select Active Frame Buffer in Double Buffer
// if uWhichBuf == 0, Buffer 0. if uWhichBuf == 1, Buffer 1
//
// Input :	
//
// Output :
//
// Version : 
//------------------------------------------------------------
void LCD_SetActiveFrameBuf(u32 uWhichBuf, LCD_WINDOW eWin)
{
	u32 uWinConReg, uWinConVal;
	
	Assert( (eWin == WIN0) || (eWin == WIN1) ); // Double buffering is supported in WIN 0 or WIN 1
	Assert( (uWhichBuf == 0) || (uWhichBuf == 1) );

	uWinConReg = (eWin == WIN0) ? rWINCON0 : rWINCON1;

	uWinConVal=Inp32(LCD_BASE+uWinConReg);

	uWinConVal = (uWinConVal & ~(1<<20)) | uWhichBuf<<20;

	LcdcOutp32(uWinConReg, uWinConVal);
}


void LCD_SetColorMapOnOff(u32 uOnOff, LCD_WINDOW eWin)
{

	u32 uWinMapReg, uWinMapVal;

	uWinMapReg =
		(eWin == WIN0) ? rWIN0MAP :
		(eWin == WIN1) ? rWIN1MAP :
		(eWin == WIN2) ? rWIN2MAP :
		(eWin == WIN3) ? rWIN3MAP : rWIN4MAP;

	// Color Map is activated when next frame is run or ENVID_F is '0' and positive edge
	#if (LCD_MODULE_TYPE == LTS222QV_CPU)
		if (uOnOff == 1) LCD_Stop();
	#endif

	uWinMapVal=Inp32(LCD_BASE+uWinMapReg);

	if (uOnOff)
		LcdcOutp32(uWinMapReg, COLOR_MAP_ENABLE|uWinMapVal);
	else
		LcdcOutp32(uWinMapReg, ~COLOR_MAP_ENABLE&uWinMapVal);

	#if (LCD_MODULE_TYPE == LTS222QV_CPU)
		if (uOnOff == 1) LCD_Start();
	#endif
}

//------------------------------------------------------------
// Function Name : LCD_SetColorMapValue
//
// Function Desctiption :
//  When enable, Video DMA stops, and MapColor appears as back-ground image.
//
// Input :	
//
// Output :	
//
// Version : 
//------------------------------------------------------------
void LCD_SetColorMapValue(u32 uVal, LCD_WINDOW eWin)
{
	u32 uColVal = uVal&0x00ffffff;
	u32 uWinMapReg;

	uWinMapReg =
		(eWin == WIN0) ? rWIN0MAP :
		(eWin == WIN1) ? rWIN1MAP :
		(eWin == WIN2) ? rWIN2MAP :
		(eWin == WIN3) ? rWIN3MAP : rWIN4MAP;

	LcdcOutp32(uWinMapReg, COLOR_MAP_ENABLE|uColVal);
}


//------------------------------------------------------------
// Function Name : LCD_Trigger
//
// Function Desctiption :
//  This function trigs cpu i/f lcd with s/w trigger
//
// Input :	
//
// Output :	
//
// Version : 
//------------------------------------------------------------
void LCD_Trigger(void)
{
	u32 uTrigCtrl;

	uTrigCtrl=Inp32(LCD_BASE+rTRIGCON);

	if (oLcdc.m_bIsAutoBuf == true)
		LcdcOutp32(rTRIGCON, uTrigCtrl | SW_TRIG_MODE | SW_TRIGGER);
	else
	{
		#if ((LCD_MODULE_TYPE == LTS222QV_CPU) )
			LcdcOutp32(rTRIGCON, uTrigCtrl | SW_TRIG_MODE | SW_TRIGGER);
		#endif
		;
	}
}



///////////////////////////////////////////////////////////////////
// Bottom Layer
///////////////////////////////////////////////////////////////////

//------------------------------------------------------------
// Function Name : LCD_GetClkValAndClkDir
//
// Function Desctiption :
//  This function calculates clkval and determines clock source
//
// Input :	uLcdHSz, lcd horizontal size
//			uLcdVSz, lcd vertical size
//
// Output :	uClkVal, clock value for wanted vclk
//			uClkDir, clock source
//
// Version : 
//------------------------------------------------------------
void LCD_GetClkValAndClkDir(u32 uLcdHSz, u32 uLcdVSz, u32* uClkVal, u32* uClkDir, CLK_SRC eClkSrc)
{
	double dTmpVal, dVclkSrc;
	u32 uTotalTime;
	
	if(eClkSrc == SRC_HCLK)
	{
		
		LCD_SetClkSrc(SRC_HCLK);
		dVclkSrc = g_HCLK;		//should be changed in your system clock condition
	}
	else if(eClkSrc == SRC_MOUT_EPLL)
	{
		LCD_SetClkSrc(SRC_MOUT_EPLL);
		dVclkSrc = 97700000; 	//should be changed in your system clock condition	
	}
	else if(eClkSrc == SRC_DOUT_MPLL)
	{
		LCD_SetClkSrc(SRC_DOUT_MPLL);
		dVclkSrc = 100000000; 	//should be changed in your system clock condition
	}
	else if(eClkSrc == SRC_FIN_EPLL)
	{
		LCD_SetClkSrc(SRC_FIN_EPLL);
		dVclkSrc = 12000000; 	//should be changed in your system clock condition
	}
	else if(eClkSrc == SRC_27M)
		dVclkSrc = 27000000;		
		
	#if (LCD_MODULE_TYPE == LTP700WV_RGB ||LCD_MODULE_TYPE ==LTE480WV_RGB)
		uLcdHSz+= HBPD_LTP700WV + HFPD_LTP700WV + HSPW_LTP700WV +3;
		uLcdVSz+= VBPD_LTP700WV + VFPD_LTP700WV + VSPW_LTP700WV +3;

	#elif (LCD_MODULE_TYPE == LTS222QV_SRGB)
		uLcdHSz = uLcdHSz*3 + HBPD_LTS222QV + HFPD_LTS222QV + HSPW_LTS222QV +3;
		uLcdVSz+= VBPD_LTS222QV + VFPD_LTS222QV + VSPW_LTS222QV +3;

	#elif (LCD_MODULE_TYPE == LTV300GV_RGB)
		uLcdHSz += HBPD_LTV300GV + HFPD_LTV300GV + HSPW_LTV300GV +3;
		uLcdVSz+= VBPD_LTV300GV + VFPD_LTV300GV + VSPW_LTV300GV +3;

	#elif (LCD_MODULE_TYPE == LTV350QV_RGB)
		uLcdHSz += HBPD_LTV350QV + HFPD_LTV350QV + HSPW_LTV350QV +3;
		uLcdVSz+= VBPD_LTV350QV + VFPD_LTV350QV + VSPW_LTV350QV +3;	
	
	#endif
		
	uTotalTime =
	#if (LCD_MODULE_TYPE == LTS222QV_CPU)
				CS_SETUP_TIME + WR_SETUP_TIME + WR_ACT_TIME + WR_HOLD_TIME + 4;
	#else
				1;
	#endif

	if ( LCD_TU_Out_TEST == 1) dTmpVal = dVclkSrc/(double)(uTotalTime*uLcdHSz*uLcdVSz*30) - 1;
	else dTmpVal = dVclkSrc/(double)(uTotalTime*uLcdHSz*uLcdVSz*LCD_FRM_RATE) - 1;
	

	dTmpVal = (dTmpVal+0.5)*10;
	*uClkVal = (int)(dTmpVal/10.0);

	*uClkDir = (*uClkVal<1) ? 0 : 1;

 }


//------------------------------------------------------------
// Function Name : LCD_GetFbEndAddr
//
// Function Desctiption :
//
// Input :	
//
// Output :	
//
// Version : 
//------------------------------------------------------------
void LCD_GetFbEndAddr(u32* uFbAddr, LCD_WINDOW eWin) 
{ 
	*uFbAddr = oLcdc.m_uFbEndAddr[eWin]; 
}


//------------------------------------------------------------
// Function Name : LCD_GetDoubleBufAddr
//
// Function Desctiption :
//  This function set the View-Port's position of Frame
//
// Input :	uWhichAddr, which address (0=buffer0, 1=buffer1)
//			eWin, selected window number
//
// Output :	uFbAddr, double buffer start address
//
// Version : 
//------------------------------------------------------------
void LCD_GetDoubleBufAddr(u32* uFbAddr, u32 uWhichAddr, LCD_WINDOW eWin)
{
	Assert (eWin == WIN0 || eWin == WIN1);
	Assert(oLcdc.m_uFbStAddr[eWin] != oLcdc.m_uDoubleBufStAddr[eWin]);

	*uFbAddr = (!uWhichAddr) ? oLcdc.m_uImgStAddr[eWin] : oLcdc.m_uDoubleBufImgStAddr[eWin];
}


// if uMode == 0, Normal Mode. if uMode == 1, AutoBuffer Mode
void LCD_EnableAutoBuf(u32 uMode, LCD_WINDOW eWin)
{
	u32 uWinCtrlReg, uWinCtrlVal;
	
	Assert( (eWin == WIN0) || (eWin == WIN1));
	Assert( (uMode == 0) || (uMode == 1) );

	oLcdc.m_bIsAutoBuf = (uMode == 1) ? true : false;
	uWinCtrlReg = (eWin == WIN0) ? rWINCON0 : rWINCON1;

	uWinCtrlVal = Inp32(LCD_BASE+uWinCtrlReg);
	uWinCtrlVal = (uWinCtrlVal & ~(1<<19)) | uMode<<19;
	LcdcOutp32(uWinCtrlReg, uWinCtrlVal);
}

//------------------------------------------------------------
// Function Name : LCD_GetBufIdx
//
// Function Desctiption :
//  This function indicates which buffer is used in each window
//
// Input :	eWin, selected window number
//
// Output :	uBufNum, buffer number
//
// Version : 
//------------------------------------------------------------
void LCD_GetBufIdx(u32* uBufNum, LCD_WINDOW eWin)
{
	u32 uWinCtrlReg, uWinCtrlVal;
	u32 uTmpVal;
	
	Assert (eWin == WIN0 || eWin == WIN1);

	uWinCtrlReg = (eWin == WIN0) ? rWINCON0 : rWINCON1;

	uWinCtrlVal = Inp32(LCD_BASE+uWinCtrlReg);
	
	uTmpVal = (uWinCtrlVal&(0x1<<21));
	*uBufNum = (uTmpVal == (0x1<<21)) ? 1 : 0;
}


//------------------------------------------------------------
// Function Name : LCDC_InitWinRegs
//
// Function Desctiption :
//  This function fills sfr with value of global lcd struct
//
// Input :	eWin, window number
//
// Output :
//
// Version : 
//------------------------------------------------------------

void LCD_InitWinRegs(LCD_WINDOW eWin)
{
	u32 uWinConXReg;
	u32 uAlphaValReg;
	u32 uFrmBufStAddrReg;
	u32 uFrmBufEndAddrReg;
	u32 uOffsetPageWidthReg;
	u32 uOsdLeftPosReg;
	u32 uOsdRightPosReg;
	u32 uWinConXRegVal;
	u32 uLcdEndX, uLcdEndY;
	
	#if (LCD_MODULE_TYPE == LTS222QV_CPU)
		LcdcOutp32(rVIDTCON2, LINEVAL(oLcdc.m_uLcdVSz-1) | HOZVAL(oLcdc.m_uLcdHSz-1));			
	#endif


	uWinConXReg =
		(eWin == WIN0) ? rWINCON0 :
		(eWin == WIN1) ? rWINCON1 :
		(eWin == WIN2) ? rWINCON2 :
		(eWin == WIN3) ? rWINCON3 : rWINCON4;

	uAlphaValReg =
		(eWin == WIN1) ? rVIDOSD1C :
		(eWin == WIN2) ? rVIDOSD2C :
		(eWin == WIN3) ? rVIDOSD3C :
		(eWin == WIN4) ? rVIDOSD4C : rVIDOSD1C;

	uFrmBufStAddrReg =
		(eWin == WIN0) ? rVIDW00ADD0B0 :
		(eWin == WIN1) ? rVIDW01ADD0B0:
		(eWin == WIN2) ? rVIDW02ADD0 :
		(eWin == WIN3) ? rVIDW03ADD0 : rVIDW04ADD0;

	uFrmBufEndAddrReg =
		(eWin == WIN0) ? rVIDW00ADD1B0 :
		(eWin == WIN1) ? rVIDW01ADD1B0 :
		(eWin == WIN2) ? rVIDW02ADD1 :
		(eWin == WIN3) ? rVIDW03ADD1 : rVIDW04ADD1;

	uOffsetPageWidthReg =
		(eWin == WIN0) ? rVIDW00ADD2 :
		(eWin == WIN1) ? rVIDW01ADD2 :
		(eWin == WIN2) ? rVIDW02ADD2 :
		(eWin == WIN3) ? rVIDW03ADD2 : rVIDW04ADD2;

	uOsdLeftPosReg =
		(eWin == WIN0) ? rVIDOSD0A :
		(eWin == WIN1) ? rVIDOSD1A :
		(eWin == WIN2) ? rVIDOSD2A :
		(eWin == WIN3) ? rVIDOSD3A : rVIDOSD4A;

	uOsdRightPosReg =
		(eWin == WIN0) ? rVIDOSD0B :
		(eWin == WIN1) ? rVIDOSD1B :
		(eWin == WIN2) ? rVIDOSD2B :
		(eWin == WIN3) ? rVIDOSD3B : rVIDOSD4B;

	uWinConXRegVal = 0;
	
	if (eWin == WIN0)
	{
		if (!oLcdc.m_bIsLocalCalled[0]) // Input path is DMA
		{
			uWinConXRegVal =
				W0DMA | W0BUF0 | W0BUFAUTO_DISABLE | BITSWP_DISABLE |
				oLcdc.m_uBytSwpMode | oLcdc.m_uHawSwpMode|
				oLcdc.m_uMinBurstLen | oLcdc.m_uMaxBurstLen | oLcdc.m_uBppMode |
				ENWIN_F_DISABLE;
			
		}
		else // Input path is Local
		{
			Assert(oLcdc.m_eLocalIn[0] == IN_POST);

			if(oLcdc.m_uLocalInColorSpace[0] == LOCALIN_YCbCr)
			{
				uWinConXRegVal =
				W0LOCAL_POST | W0BUF0 | W0BUFAUTO_DISABLE | BITSWP_DISABLE |
				oLcdc.m_uBytSwpMode | oLcdc.m_uHawSwpMode| IN_LOCAL_YUV |
				oLcdc.m_uMinBurstLen | oLcdc.m_uMaxBurstLen | oLcdc.m_uBppMode |
				ENWIN_F_DISABLE;
			}
			else
			{
				uWinConXRegVal =
				W0LOCAL_POST | W0BUF0 | W0BUFAUTO_DISABLE | BITSWP_DISABLE |
				oLcdc.m_uBytSwpMode | oLcdc.m_uHawSwpMode| IN_LOCAL_RGB |
				oLcdc.m_uMinBurstLen | oLcdc.m_uMaxBurstLen | oLcdc.m_uBppMode |
				ENWIN_F_DISABLE;
			}
			
		}

		LcdcOutp32(rVIDW00ADD0B1, oLcdc.m_uDoubleBufStAddr[0]);
		LcdcOutp32(rVIDW00ADD1B1, oLcdc.m_uDoubleBufEndAddr[0]);
		LcdcOutp32(rVIDOSD0C, OSDSIZE(oLcdc.m_uViewHSz[0]*oLcdc.m_uViewVSz[0]));
	}
	else
	{
		if (eWin == WIN1)
		{
			if (!oLcdc.m_bIsLocalCalled[1]) // Input path is DMA
			{
				uWinConXRegVal =
					W0DMA | W0BUF0 | W0BUFAUTO_DISABLE | BITSWP_DISABLE |
					oLcdc.m_uBytSwpMode | oLcdc.m_uHawSwpMode|
					oLcdc.m_uMinBurstLen | oLcdc.m_uMaxBurstLen | BLD_PIX_PLANE | oLcdc.m_uBppMode |
					BLEND_ALPHA0_PLANE | ENWIN_F_DISABLE;
			}
			else // Input path is Local
			{
				Assert(oLcdc.m_eLocalIn[1] == IN_POST || oLcdc.m_eLocalIn[1] == IN_CIM);

				if(oLcdc.m_uLocalInColorSpace[1] == LOCALIN_YCbCr)
				{
					uWinConXRegVal =
					((oLcdc.m_eLocalIn[1] == IN_CIM) ? W1ENLOCAL_CIM : W1ENLOCAL_POST) |
					W1LOCAL| W0BUF0 | W0BUFAUTO_DISABLE | BITSWP_DISABLE |
					oLcdc.m_uBytSwpMode | oLcdc.m_uHawSwpMode| IN_LOCAL_YUV |
					oLcdc.m_uMinBurstLen | oLcdc.m_uMaxBurstLen | BLD_PIX_PLANE | oLcdc.m_uBppMode |
					BLEND_ALPHA0_PLANE | ENWIN_F_DISABLE;
				}
				else
				{
					uWinConXRegVal =
					((oLcdc.m_eLocalIn[1] == IN_CIM) ? W1ENLOCAL_CIM : W1ENLOCAL_POST) |
					W1LOCAL| W0BUF0 | W0BUFAUTO_DISABLE | BITSWP_DISABLE |
					oLcdc.m_uBytSwpMode | oLcdc.m_uHawSwpMode| IN_LOCAL_RGB |
					oLcdc.m_uMinBurstLen | oLcdc.m_uMaxBurstLen | BLD_PIX_PLANE | oLcdc.m_uBppMode |
					BLEND_ALPHA0_PLANE | ENWIN_F_DISABLE;
				}	
			}
			
			// set double buf. addr. and offset size
			LcdcOutp32(rVIDW01ADD0B1, oLcdc.m_uDoubleBufStAddr[1]);
			LcdcOutp32(rVIDW01ADD1B1, oLcdc.m_uDoubleBufEndAddr[1]);

			LcdcOutp32(rVIDOSD1D, OSDSIZE(oLcdc.m_uViewHSz[1]*oLcdc.m_uViewVSz[1]));			
		}
		else // eWin == WIN2 OR WIN3 OR WIN4
		{
			if (eWin == WIN2)
			{
				if (!oLcdc.m_bIsLocalCalled[2]) // Input path is DMA
				{
					uWinConXRegVal =
						W0DMA | BITSWP_DISABLE |
						oLcdc.m_uBytSwpMode | oLcdc.m_uHawSwpMode|
						oLcdc.m_uMinBurstLen | oLcdc.m_uMaxBurstLen | BLD_PIX_PLANE | oLcdc.m_uBppMode |
						BLEND_ALPHA0_PLANE | ENWIN_F_DISABLE;
				}
				else // Input path is Local
				{
					Assert(oLcdc.m_eLocalIn[2] == IN_POST || oLcdc.m_eLocalIn[2] == IN_CIM);

					if(oLcdc.m_uLocalInColorSpace[2] == LOCALIN_YCbCr)
					{
					
						uWinConXRegVal =
						((oLcdc.m_eLocalIn[2] == IN_CIM) ? W1ENLOCAL_CIM : W1ENLOCAL_POST) | 
						W1LOCAL | BITSWP_DISABLE |oLcdc.m_uBytSwpMode | oLcdc.m_uHawSwpMode| IN_LOCAL_YUV |
						oLcdc.m_uMinBurstLen | oLcdc.m_uMaxBurstLen | BLD_PIX_PLANE | oLcdc.m_uBppMode |
						BLEND_ALPHA0_PLANE | ENWIN_F_DISABLE;
					}
					else 
					{
						
						uWinConXRegVal =
						((oLcdc.m_eLocalIn[2] == IN_CIM) ? W1ENLOCAL_CIM : W1ENLOCAL_POST) | 
						W1LOCAL | BITSWP_DISABLE |oLcdc.m_uBytSwpMode | oLcdc.m_uHawSwpMode| IN_LOCAL_RGB |
						oLcdc.m_uMinBurstLen | oLcdc.m_uMaxBurstLen | BLD_PIX_PLANE | oLcdc.m_uBppMode |
						BLEND_ALPHA0_PLANE | ENWIN_F_DISABLE;
					}
					
				}

				LcdcOutp32(rVIDOSD2D, OSDSIZE(oLcdc.m_uViewHSz[2]*oLcdc.m_uViewVSz[2]));				
			}
			else // eWin == WIN3 || eWin == WIN4
			{
				uWinConXRegVal = 
					BITSWP_DISABLE | oLcdc.m_uBytSwpMode | oLcdc.m_uHawSwpMode |
					oLcdc.m_uMinBurstLen | oLcdc.m_uMaxBurstLen |
					BLD_PIX_PLANE | oLcdc.m_uBppMode | BLEND_ALPHA0_PLANE | ENWIN_F_DISABLE;
			}
		}

		LcdcOutp32(uAlphaValReg,
			ALPHA0_R(0xF) | ALPHA0_G(0xF) | ALPHA0_B(0xF) |
			ALPHA1_R(0xF) | ALPHA1_G(0xF)| ALPHA1_B(0xF));
	}

	LcdcOutp32(uWinConXReg, uWinConXRegVal);		
	LcdcOutp32(uFrmBufStAddrReg, oLcdc.m_uFbStAddr[eWin]);
	LcdcOutp32(uFrmBufEndAddrReg, oLcdc.m_uFbEndAddr[eWin]);
	LcdcOutp32(uOffsetPageWidthReg, (oLcdc.m_uOffsetSz[eWin]<<13) | oLcdc.m_uPageWidth[eWin]);

	uLcdEndX = oLcdc.m_uLcdStX + oLcdc.m_uViewHSz[eWin] - 1;
	uLcdEndY = oLcdc.m_uLcdStY + oLcdc.m_uViewVSz[eWin] - 1;

	if (oLcdc.m_uScanMode == 1) // Interlace mode
	{
		Assert(!(oLcdc.m_uLcdStX%2));	// Left top -> Y coordinate must be even number in interlace mode
		Assert(uLcdEndY%2); 			// Rigth bottom -> Y coordinate must be odd number in interlace mode

		// In interlace mode, Left Top Y(oLcdc.m_uLcdStY) and Right Bottom Y(uLcdEndY) must be divided by 2.
		// And, Right Bottom Y must be rounded down
		LcdcOutp32(uOsdLeftPosReg, OSD_LTX_F(oLcdc.m_uLcdStX) | (OSD_LTY_F(oLcdc.m_uLcdStY)>>1));
		LcdcOutp32(uOsdRightPosReg, OSD_RBX_F(uLcdEndX) | (OSD_RBY_F(uLcdEndY)>>1));
	}
	else // Progressive mode
	{
		LcdcOutp32(uOsdLeftPosReg, OSD_LTX_F(oLcdc.m_uLcdStX) | OSD_LTY_F(oLcdc.m_uLcdStY));
		LcdcOutp32(uOsdRightPosReg, OSD_RBX_F(uLcdEndX) | OSD_RBY_F(uLcdEndY));
	}
}


void LCD_InitWinForFifoIn(LCD_LOCAL_INPUT eLocalIn, LCD_WINDOW eWin)
{
	Assert(eLocalIn == IN_CIM || eLocalIn == IN_POST);

	oLcdc.m_bIsLocalCalled[eWin] = true;
	oLcdc.m_eLocalIn[eWin] = eLocalIn;
	LCD_InitWin(RGB24, oLcdc.m_uLcdHSz, oLcdc.m_uLcdVSz, 0, 0, oLcdc.m_uLcdHSz, oLcdc.m_uLcdVSz, 0, 0, 0, eWin, false);
}


void LCD_InitWinForFifoIn1(LCD_LOCAL_INPUT eLocalIn, LCD_WINDOW eWin, LOCAL_INPUT_COLORSPACE eLocalInSCS)
{
	Assert(eLocalIn == IN_CIM || eLocalIn == IN_POST);

	oLcdc.m_bIsLocalCalled[eWin] = true;
	oLcdc.m_eLocalIn[eWin] = eLocalIn;
	oLcdc.m_uLocalInColorSpace[eWin] = eLocalInSCS;
	
	LCD_InitWin(RGB24, oLcdc.m_uLcdHSz, oLcdc.m_uLcdVSz, 0, 0, oLcdc.m_uLcdHSz, oLcdc.m_uLcdVSz, 0, 0, 0, eWin, false);
}

void LCD_InitWinForFifoIn2(LCD_LOCAL_INPUT eLocalIn, LCD_WINDOW eWin, LOCAL_INPUT_COLORSPACE eLocalInSCS, u32 uLcdHsize, u32 uLcdVsize)
{
	Assert(eLocalIn == IN_CIM || eLocalIn == IN_POST);

	oLcdc.m_bIsLocalCalled[eWin] = true;
	oLcdc.m_eLocalIn[eWin] = eLocalIn;
	oLcdc.m_uLocalInColorSpace[eWin] = eLocalInSCS;
	
	LCD_InitWin(RGB24, uLcdHsize, uLcdVsize, 0, 0, uLcdHsize, uLcdVsize, 0, 0, 0, eWin, false);
}



//------------------------------------------------------------
// Function Name : LCD_SetWinOnOff
//
// Function Desctiption :
//  This function sets window on/off
//
// Input :	uOnOff, on/off value (on=1, off=0)
//			eWin, selected window number
//
// Output :
//
// Version : 
//------------------------------------------------------------
void LCD_SetWinOnOff(u32 uOnOff, LCD_WINDOW eWin)
{
	u32 uWinTemp;
	u32 uWinConReg;

	uWinConReg =
		(eWin == WIN0) ? rWINCON0 :
		(eWin == WIN1) ? rWINCON1 :
		(eWin == WIN2) ? rWINCON2 :
		(eWin == WIN3) ? rWINCON3 : rWINCON4;

	uWinTemp=Inp32(LCD_BASE+uWinConReg);

	if (uOnOff) // Turn OSD on
		uWinTemp |= ENWIN_F_ENABLE;
	else // Turn OSD off
		uWinTemp &= ~ENWIN_F_ENABLE;

	LcdcOutp32(uWinConReg, uWinTemp);
}

//------------------------------------------------------------
// Function Name : LCD_SetAllWinOnOff
//
// Function Desctiption :
//  This function sets all window on/off
//
// Input :	uOnOff, on/off value (on=1, off=0)
//
// Output :
//
// Version : 
//------------------------------------------------------------
void LCD_SetAllWinOnOff(u32 uOnOff)
{
	u32 uWinTemp[5];

	uWinTemp[0]=Inp32(LCD_BASE+rWINCON0);
	uWinTemp[1]=Inp32(LCD_BASE+rWINCON1);
	uWinTemp[2]=Inp32(LCD_BASE+rWINCON2);
	uWinTemp[3]=Inp32(LCD_BASE+rWINCON3);
	uWinTemp[4]=Inp32(LCD_BASE+rWINCON4);

	if (uOnOff)
	{
		LcdcOutp32(rWINCON0, uWinTemp[0]|ENWIN_F_ENABLE);
		LcdcOutp32(rWINCON1, uWinTemp[1]|ENWIN_F_ENABLE);
		LcdcOutp32(rWINCON2, uWinTemp[2]|ENWIN_F_ENABLE);
		LcdcOutp32(rWINCON3, uWinTemp[3]|ENWIN_F_ENABLE);
		LcdcOutp32(rWINCON4, uWinTemp[4]|ENWIN_F_ENABLE);
	}
	else
	{
		LcdcOutp32(rWINCON0, uWinTemp[0]&~ENWIN_F_ENABLE);
		LcdcOutp32(rWINCON1, uWinTemp[1]&~ENWIN_F_ENABLE);
		LcdcOutp32(rWINCON2, uWinTemp[2]&~ENWIN_F_ENABLE);
		LcdcOutp32(rWINCON3, uWinTemp[3]&~ENWIN_F_ENABLE);
		LcdcOutp32(rWINCON4, uWinTemp[4]&~ENWIN_F_ENABLE);
	}
}

//------------------------------------------------------------
// Function Name : LCD_SetAllWinColorMapOnOff
//
// Function Desctiption :
//
// Input :	
//
// Output :
//
// Version : 
//------------------------------------------------------------
void LCD_SetAllWinColorMapOnOff(u32 uOnOff)
{
	u32 uColorMapVal;
	u32 uColorMapReg;
	int i;

	for (i=0; i<5; i++)
	{
		uColorMapReg =
			(i == 0) ? rWIN0MAP :
			(i == 1) ? rWIN1MAP :
			(i == 2) ? rWIN2MAP :
			(i == 3) ? rWIN3MAP : rWIN4MAP;

		uColorMapVal=Inp32(LCD_BASE+uColorMapReg);

		if (uOnOff)
			LcdcOutp32(uColorMapReg, COLOR_MAP_ENABLE|uColorMapVal);
		else
			LcdcOutp32(uColorMapReg, ~COLOR_MAP_ENABLE&uColorMapVal);
	}

}

//------------------------------------------------------------
// Function Name : LCD_SetAllWinColorKeyOnOff
//
// Function Desctiption :
//
// Input :	
//
// Output :
//
// Version : 
//------------------------------------------------------------
 void LCD_SetAllWinColorKeyOnOff(u32 uOnOff)
{
	u32 uColorKeyReg, uColorKeyVal;
	int i;

	for (i=1; i<5; i++)
	{
		uColorKeyReg =
			(i == 1) ? rW1KEYCON0 :
			(i == 2) ? rW2KEYCON0 :
			(i == 3) ? rW3KEYCON0 : rW4KEYCON0;

		uColorKeyVal=Inp32(LCD_BASE+uColorKeyReg);

		if (uOnOff)
			LcdcOutp32(uColorKeyReg, uColorKeyVal | KEYEN_F_ENABLE);

		else
			LcdcOutp32(uColorKeyReg, uColorKeyVal & KEYBLEN_DISABLE & KEYEN_F_DISABLE);
	}
}

//------------------------------------------------------------
// Function Name : LCD_SetAlpha
//
// Function Desctiption :
//  This function sets alpha value for win 1 ~ 4
//
// Input :	ucA0R, alpha red value for AEN0
//			ucA0G, alpha green value for AEN0
//			ucA0B, alpha blue value for AEN0
//			ucA1R, alpha red value for AEN1
//			ucA1G, alpha green value for AEN1
//			ucA1B, alpha blue value for AEN1
//			eWin, selected window number
//
// Output :
//
// Version : 
//------------------------------------------------------------
void LCD_SetAlpha(u8 ucA0R, u8 ucA0G, u8 ucA0B, u8 ucA1R, u8 ucA1G, u8 ucA1B, LCD_WINDOW eWin)
{
	u8 ucA0RVal = ucA0R&0xf;
	u8 ucA0GVal = ucA0G&0xf;
	u8 ucA0BVal = ucA0B&0xf;
	u8 ucA1RVal = ucA1R&0xf;
	u8 ucA1GVal = ucA1G&0xf;
	u8 ucA1BVal = ucA1B&0xf;
	u32 uAlphaReg;

	// Can't support Alpha value to window 0
	Assert(eWin != WIN0);

	uAlphaReg =
		(eWin == WIN1) ? rVIDOSD1C :
		(eWin == WIN2) ? rVIDOSD2C :
		(eWin == WIN3) ? rVIDOSD3C : rVIDOSD4C;

	LcdcOutp32(uAlphaReg, ALPHA0_R(ucA0RVal) | ALPHA0_G(ucA0GVal) | ALPHA0_B(ucA0BVal) |
						ALPHA1_R(ucA1RVal) | ALPHA1_G(ucA1GVal) | ALPHA1_B(ucA1BVal));
}


void LCD_SetColorKeyOnOff(u32 uOnOff, LCD_WINDOW eWin)
{
	u32 uColorKeyReg, uColorKeyVal;
	
	Assert(eWin != WIN0);

	uColorKeyReg =
		(eWin == WIN1) ? rW1KEYCON0 :
		(eWin == WIN2) ? rW2KEYCON0 :
		(eWin == WIN3) ? rW3KEYCON0 : rW4KEYCON0;

	uColorKeyVal = Inp32(LCD_BASE+uColorKeyReg);

	if (uOnOff) // Turn Color Key On
		LcdcOutp32(uColorKeyReg, uColorKeyVal | KEYEN_F_ENABLE);
	else
		LcdcOutp32(uColorKeyReg, uColorKeyVal & KEYBLEN_DISABLE & KEYEN_F_DISABLE);
}

void LCD_SetColorKey(u32 uMaskKey, u32 uColorKey, COLOR_KEY_DIRECTION eDir, u8 bIsBlending, LCD_WINDOW eWin)
{
	u32 uColorKeyReg_0, uColorKeyReg_1;
	u32 uColorKeyVal_0;

	u32 uBlending = KEYBLEN_DISABLE;
	u32 uDir;

	Assert(eWin != WIN0);

	if (bIsBlending == true) // To use Key blending, BLD_PIX must be set 1 and ALPHA_SEL be set 0
	{
		u32 uWinConReg, uWinConVal;
		uBlending = KEYBLEN_ENABLE;
		
		uWinConReg =
			(eWin == WIN1) ? rWINCON1 :
			(eWin == WIN2) ? rWINCON2 :
			(eWin == WIN3) ? rWINCON3 : rWINCON4;

		uWinConVal = Inp32(LCD_BASE+uWinConReg);
		// Set BLD_PIX is set 1 and ALPHA_SEL is set 0
		LcdcOutp32(uWinConReg, uWinConVal | BLD_PIX_PIXEL & ALPHA_SEL_0);
	}

	uColorKeyReg_0 =
		(eWin == WIN1) ? rW1KEYCON0 :
		(eWin == WIN2) ? rW2KEYCON0 :
		(eWin == WIN3) ? rW3KEYCON0 : rW4KEYCON0;

	uColorKeyReg_1 =
		(eWin == WIN1) ? rW1KEYCON1 :
		(eWin == WIN2) ? rW2KEYCON1 :
		(eWin == WIN3) ? rW3KEYCON1 : rW4KEYCON1;

	uDir = (eDir == MATCH_FG_IMAGE) ? DIRCON_MATCH_FG_IMAGE : DIRCON_MATCH_BG_IMAGE;

	uColorKeyVal_0 = Inp32(LCD_BASE+uColorKeyReg_0);

	uColorKeyVal_0 &= ~(1<<26);
	uColorKeyVal_0 |= uBlending;

	uColorKeyVal_0 &= ~(1<<24);
	uColorKeyVal_0 |= uDir;

	uColorKeyVal_0 |= ( (uMaskKey&0xffffff)<<0 ) ;
	
	LcdcOutp32(uColorKeyReg_0, uColorKeyVal_0);
	LcdcOutp32(uColorKeyReg_1, uColorKey);
	
}


void LCD_SetBlendingType(BLENDING_APPLIED_UNIT eBldPix, BLENDING_SELECT_MODE eBldMode, LCD_WINDOW eWin)
{
	u32 uWinConReg, uWinConVal;
	u32 uBlendUnit, uBlendSel;

	Assert(eWin != WIN0); // Blending feature supports window 1 ~ 4

	uWinConReg =
		(eWin == WIN1) ? rWINCON1 :
		(eWin == WIN2) ? rWINCON2 :
		(eWin == WIN3) ? rWINCON3 : rWINCON4;

	uBlendUnit = (eBldPix == PER_PLANE) ? BLD_PIX_PLANE : BLD_PIX_PIXEL;

	uBlendSel =
		(eBldMode == ALPHA0_PLANE) ? BLEND_ALPHA0_PLANE :
		(eBldMode == ALPHA1_PLANE) ? BLEND_ALPHA1_PLANE :
		(eBldMode == DATA_PIXEL) ? BLEND_DATA_PIXEL :
		(eBldMode == AEN_PIXEL) ? BLEND_AEN_PIXEL : BLEND_COLORKEY_AREA;

	uWinConVal = Inp32(LCD_BASE+uWinConReg);
	LcdcOutp32(uWinConReg, uWinConVal & ~BLD_PIX_MASK|uBlendUnit & ~ BLEND_SEL_MASK | uBlendSel);
}



//-----------------------------------------------------------
// Function Name : LCD_SetPaletteCtrl
//
// Function Desctiption :
//
// Input :	
//
// Output :
//
// Version : 
//------------------------------------------------------------
void LCD_SetPaletteCtrl(CSPACE eBpp, LCD_WINDOW eWin)
{
	u32 uPalConVal, uPalConTmpVal, uPalConWrVal;
	
	Assert(eBpp == RGB16 || eBpp == RGB24);

	if ( (eWin == WIN2) || (eWin == WIN3) || (eWin == WIN4) )
		Assert(eBpp == RGB16);

	uPalConVal=Inp32(LCD_BASE+rWPALCON);
	uPalConTmpVal = uPalConVal;

	if ( (eWin == WIN0) || (eWin == WIN1) )
	{
		uPalConVal = (eBpp == RGB16) ? 6 : 1;
 		uPalConWrVal =
			(eWin == WIN0) ? ((uPalConTmpVal & ~(0x7)) | (uPalConVal<<0)) :
			((uPalConTmpVal & ~(0x38)) | (uPalConVal<<3)); // window 1
	}
	else // (eWin == WIN2) || (eWin == WIN3) || (eWin == WIN4)
	{
		uPalConWrVal = // In 0<<x, '0' means 16bit (R:5 G:6 B:5)
			(eWin == WIN2) ? ((uPalConTmpVal & ~(0x40)) | (0<<6)) :
			(eWin == WIN3) ? ((uPalConTmpVal & ~(0x80)) | (0<<7)) :
			((uPalConTmpVal & ~(0x100)) | (0<<8)); // window 4
	}

	LcdcOutp32(rWPALCON, uPalConWrVal);
}

//-----------------------------------------------------------
// Function Name : LCD_ChangePaletteClk
//
// Function Desctiption :
//
// Input :	
//
// Output :
//
// Version : 
//------------------------------------------------------------
void LCD_ChangePaletteClk(u32 uWhichClk)
{
	u32 uPalClk;

	Assert((uWhichClk == 1) || (uWhichClk == 0));

	uPalClk=Inp32(LCD_BASE+rWPALCON);

	if (uWhichClk) // HCLK, Enable writting data to palette
		uPalClk |= 1<<9;
	else // VCLK, Normal Mode
		uPalClk &= ~(1<<9);

	LcdcOutp32(rWPALCON, uPalClk);
}

u8 LCD_CheckPaletteRAM(LCD_WINDOW eWin)
{
	u32 uPaletteRAMAddr;
	u32 i, rData, uError = 0;

	uPaletteRAMAddr= 
		(eWin == WIN0) ? (LCD_BASE+W0PRAMSTART) : (LCD_BASE+W1PRAMSTART);

	for(i=0; i<256; i++)
		*(u32*)(uPaletteRAMAddr+i*4) = i;

	UART_Printf("Read Data from Palette RAM\n");

	for(i=0; i<256; i++)
	{
		rData = *(u32*)(uPaletteRAMAddr+i*4);
		//UART_Printf("0x%x\n", rData);
		
		if(rData != i)
			uError++;
	}

	if(uError == 0)
		return true;
	else
	 	return false;
}



//-----------------------------------------------------------
// Function Name : LCD_SetPaletteData
//
// Function Desctiption :
//
// Input :	
//
// Output :
//
// Version : 
//------------------------------------------------------------
void LCD_SetPaletteData(u32 uIdx, u32 uPalData, LCD_WINDOW eWin)
{
	u32 uRegAddr;
	u32 uRegVal;
	u32 uPos = 16*(uIdx%2);;

	uRegAddr =
		(eWin == WIN0) ? W0PRAMSTART+(uIdx)*4 :
		(eWin == WIN1) ? W1PRAMSTART+(uIdx)*4 :
		(eWin == WIN2) ? rW2PDATA01+(uIdx/2)*4 :
		(eWin == WIN3) ? rW3PDATA01+(uIdx/2)*4 :
		(uIdx == 0 || uIdx == 1) ? rW4PDATA01 : rW4PDATA23;

	if ( (eWin == WIN0) || (eWin == WIN1) )
	{
		Assert(uIdx < 256);

		LcdcOutp32(uRegAddr, uPalData);
	}
	else if ( (eWin == WIN2) || (eWin == WIN3) )
	{
		Assert(uIdx < 16);

		uRegVal=Inp32(LCD_BASE+uRegAddr);
		LcdcOutp32(uRegAddr,uRegVal&~(0xffff<<uPos)|(uPalData<<uPos));
	}
	else // eWin == WIN4
	{
		uRegVal=Inp32(LCD_BASE+uRegAddr);
		LcdcOutp32(uRegAddr,uRegVal&~(0xffff<<uPos)|(uPalData<<uPos));
	}
}

void LCD_SetByteSwap(SWAP_MODE eBitSwap, SWAP_MODE eByteSwap, SWAP_MODE eHawSwap, LCD_WINDOW eWin)
{
	u32 uWinConReg, uWinConVal;
	u32 uBitTemp, uByteTemp, uHalfTemp;

	uWinConReg =
		(eWin == WIN0) ? rWINCON0 :
		(eWin == WIN1) ? rWINCON1 :
		(eWin == WIN2) ? rWINCON2 :
		(eWin == WIN3) ? rWINCON3 : rWINCON4;

	uBitTemp = (eBitSwap == SWAP_BIT_ENABLE) ? 1 : 0;
	uByteTemp = (eByteSwap == SWAP_BYTE_ENABLE) ? 1 : 0;
	uHalfTemp = (eHawSwap == SWAP_HALFWORD_ENABLE) ? 1 : 0;

	uWinConVal = Inp32(LCD_BASE+uWinConReg);
	LcdcOutp32(uWinConReg, uWinConVal&~(7<<16)|(uBitTemp<<18)|(uByteTemp<<17)|(uHalfTemp<<16));
}

void LCD_SetBurstLength(BURST_LENGTH eBurstLength, LCD_WINDOW eWin)
{
	u32 uWinConReg, uWinConVal;
		
	uWinConReg =
		(eWin == WIN0) ? rWINCON0 :
		(eWin == WIN1) ? rWINCON1 :
		(eWin == WIN2) ? rWINCON2 :
		(eWin == WIN3) ? rWINCON3 : rWINCON4;
		
	uWinConVal = Inp32(LCD_BASE+uWinConReg);

	if (eBurstLength == BURST_16WORD) LcdcOutp32(uWinConReg, uWinConVal & ~(0x3<<9)|MAX_BURSTLEN_16WORD);
	else if (eBurstLength == BURST_8WORD) LcdcOutp32(uWinConReg, uWinConVal & ~(0x3<<9)|MAX_BURSTLEN_8WORD);
	else if (eBurstLength == BURST_4WORD) LcdcOutp32(uWinConReg, uWinConVal & ~(0x3<<9)|MAX_BURSTLEN_4WORD);
}

//------------------------------------------------------------
// Function Name : LCD_SetScanMode
//
// Function Desctiption :
//  This function sets scan mode 
//
// Input :	eScanMode, scan mode (0=progressive, 1=interlaced)
//
// Output :	
//
// Version : 
//------------------------------------------------------------
void LCD_SetScanMode(LCD_SCAN_MODE eScanMode)
{
	u32 uVidConRegVal;

	uVidConRegVal=Inp32(LCD_BASE+rVIDCON0);

	if(eScanMode == PROGRESSIVE_MODE)
	LcdcOutp32(rVIDCON0, uVidConRegVal &~(1<<29));
	else
	LcdcOutp32(rVIDCON0, uVidConRegVal |(1<<29));
}

//------------------------------------------------------------
// Function Name : LCD_SetOutputPath
//
// Function Desctiption :
//  This function sets output video mode 
//
// Input :	eOutLocal, output video type
//
// Output :	
//
// Version : 
//------------------------------------------------------------
void LCD_SetOutputPath(LCD_LOCAL_OUTPUT eOutLocal)
{
	u32 uVidConRegVal;
	u32 uPathVal;

	// if output mode is LCD mode, Scan mode always should be progressive mode
	if (eOutLocal != LCD_TV)
		LCD_SetScanMode(PROGRESSIVE_MODE);

	uPathVal = 
		(eOutLocal == LCD_RGB) ? VIDOUT_RGBIF :
		(eOutLocal == LCD_TV) ? VIDOUT_TV :			
		(eOutLocal == LCD_I80F) ? VIDOUT_I80IF0 :
		(eOutLocal == LCD_I80S) ? VIDOUT_I80IF1 :
		(eOutLocal == LCD_TVRGB) ? VIDOUT_TVNRGBIF :
		(eOutLocal == LCD_TVI80F) ? VIDOUT_TVNI80IF0 : VIDOUT_TVNI80IF1;

	uVidConRegVal=Inp32(LCD_BASE+rVIDCON0);
	uVidConRegVal &= ~(0x7<<26); // masking [28:26]
	uVidConRegVal |= uPathVal;
	LcdcOutp32(rVIDCON0, uVidConRegVal);
	LCD_SetClkVal(1);
}

//------------------------------------------------------------
// Function Name : LCD_SetClkSrc
//
// Function Desctiption :
//  This function sets video clock source 
//
//------------------------------------------------------------
void LCD_SetClkSrc(CLK_SRC eClkSrc)
{
	u32 uVidConVal;
	u32 uClkSrc, uEpllCon0;

	uClkSrc =
		(eClkSrc == SRC_HCLK) ? H_CLK_INPUT :
		(eClkSrc == SRC_27M) ? EXT_CLK_1_INPUT : EXT_CLK_0_INPUT;

	uVidConVal=Inp32(LCD_BASE+rVIDCON0);

	uVidConVal &= ~(0x3<<2);
	uVidConVal |= uClkSrc;

	LcdcOutp32(rVIDCON0, uVidConVal);

	uClkSrc = Inp32SYSC(0x1C);
	uEpllCon0 = Inp32SYSC(0x14);

	if(eClkSrc == SRC_MOUT_EPLL) 
	{
		Outp32SYSC(0x14, uEpllCon0 |(1u<<31));
		Delay(100);
		Outp32SYSC(0x1C, uClkSrc & ~(0x3<<26) |(1<<2));	
	}
	else if (eClkSrc == SRC_DOUT_MPLL) Outp32SYSC(0x1C, uClkSrc & ~(0x3<<26)|(1<<26));
	else if (eClkSrc == SRC_FIN_EPLL) Outp32SYSC(0x1C, uClkSrc & ~(0x3<<7)|(2<<26));

}

void LCD_SetClkSrc1(CLK_SRC eClkSrc)
{
	u32 uVidConVal;
	u32 uClkSrc;

	uClkSrc =
		(eClkSrc == HCLK_SRC) ? H_CLK_INPUT :
		(eClkSrc == ECLK0_SRC) ? EXT_CLK_0_INPUT : EXT_CLK_1_INPUT;

	uVidConVal=Inp32(LCD_BASE+rVIDCON0);

	uVidConVal &= ~(0x3<<2);
	uVidConVal |= uClkSrc;

	LcdcOutp32(rVIDCON0, uVidConVal);
}

void LCD_SelClkSrcFromSYSCON(CLK_SRC_FROMSYSCON eClkSrcFromSyscon)
{
	u32 uClkSrc;

	uClkSrc = Inp32SYSC(0x1C);
	 
	if (eClkSrcFromSyscon == MOUT_EPLL) Outp32SYSC(0x1C, uClkSrc & ~(0x3<<26));
	else if (eClkSrcFromSyscon == DOUT_MPLL) Outp32SYSC(0x1C, uClkSrc & ~(0x3<<26)|(1<<26));
	else if (eClkSrcFromSyscon == FIN_EPLL) Outp32SYSC(0x1C, uClkSrc & ~(0x3<<26)|(2<<26));
}

//------------------------------------------------------------
// Function Name : LCD_SetClkVal
//
// Function Desctiption :
//  This function sets vclk value 
//
// Input :	uClkVal, clkval (1=direct)
//
// Output :	
//
// Version : 
//------------------------------------------------------------
void LCD_SetClkVal(u8 uClkVal)
{
	u32 uVidConRegVal;
	
	uClkVal &= 0xff;

	uVidConRegVal=Inp32(LCD_BASE+rVIDCON0);

	if ( uClkVal > 1 )
	{
		uVidConRegVal &= ~(0x1<<4);
		uVidConRegVal &= ~(0xff<<6);
		LcdcOutp32(rVIDCON0, uVidConRegVal | (uClkVal<<6) | (1<<4));
	}
	else if ( uClkVal == 1 )
	{
		uVidConRegVal &= ~(0x1<<4);
		uVidConRegVal &= ~(0xff<<6);
		LcdcOutp32(rVIDCON0, uVidConRegVal | (uClkVal<<6) | (1<<4));
	}
	else 
	{
		uVidConRegVal &= ~(0x1<<4);
		LcdcOutp32(rVIDCON0, (1<<6) | (1<<4));
	}
	

	//uVidConRegVal &= ~(0xff<<6);
	//LcdcOutp32(VIDCON0, uVidConRegVal | (uClkVal<<6));
}

void LCD_SetClkDirOrDiv(CLK_DIR_DIV eClkDirDiv)
{
	u32 uVidConRegVal;
	
	uVidConRegVal=Inp32(LCD_BASE+rVIDCON0);

	if (eClkDirDiv == CLK_DIRECT)
	{
		uVidConRegVal &= ~(0x1<<4);
		LcdcOutp32(rVIDCON0, uVidConRegVal);
	}
	else if (eClkDirDiv == CLK_DIVIDED)
	{
		uVidConRegVal |= 0x1<<4;
		LcdcOutp32(rVIDCON0, uVidConRegVal);
	}
	
}

void LCD_SetCpuOutputFormat(CPU_OUTPUT_DATAFORMAT eCpuOutDataFormat)
{
	u32 uVidconReg, uI80MainOutDataFormat, uI80SubOutDataFormat;
		
	if(eCpuOutDataFormat == CPU_16BIT)		
	{
		uI80MainOutDataFormat = MAIN_16_MODE;
		uI80SubOutDataFormat = SUB_16_MODE;
	}
	else if(eCpuOutDataFormat == CPU_16_2BIT)	
	{
		uI80MainOutDataFormat = MAIN_16PLUS2_MODE ;
		uI80SubOutDataFormat = SUB_16PLUS2_MODE ;
	}		
	else if(eCpuOutDataFormat == CPU_9_9BIT)
	{
		uI80MainOutDataFormat = MAIN_9PLUS9_MODE ;
		uI80SubOutDataFormat = SUB_9PLUS9_MODE ;
	}	
	else if(eCpuOutDataFormat == CPU_16_8BIT)
	{
		uI80MainOutDataFormat = MAIN_16PLUS8_MODE;
		uI80SubOutDataFormat = SUB_16PLUS8_MODE;
	}
	else if(eCpuOutDataFormat == CPU_18BIT)
	{
		uI80MainOutDataFormat = MAIN_18_MODE;
		uI80SubOutDataFormat = SUB_18_MODE;
	}	
	else if(eCpuOutDataFormat == CPU_8_8BIT)
	{
		uI80MainOutDataFormat = MAIN_8PLUS8_MODE;
		uI80SubOutDataFormat = SUB_8PLUS8_MODE;
	}		

	uVidconReg = Inp32(LCD_BASE+rVIDCON0);
	
	uVidconReg &= ~(0x7<<23) & ~(0x7<<20);
	uVidconReg |= uI80SubOutDataFormat | uI80MainOutDataFormat ;

	LcdcOutp32(rVIDCON0, uVidconReg);
}


void LCD_EnableRgbPort(void)
{
	LcdcOutp32(rVIDCON2, 0x380);
}


void LCD_DisableRgbPort(void)
{
	LcdcOutp32(rVIDCON2, 0);
}


void LCD_DisplayPartialArea(
	u32 uSrcStX, u32 uSrcStY, u32 uDstStX, u32 uDstStY,
	u32 uPartialHSz, u32 uPartialVsz, u32 uSrcFbStAddr,
	LCD_WINDOW eWin)
{
	u32 uSrcEndX, uSrcEndY;
	//u32 uOffsetSz, uPageWidth;

	u32 uFrmBufStAddrReg, uFrmBufEndAddrReg;
	u32 uOffsetPageWidthReg;
	u32 uOsdLeftPosReg, uOsdRightPosReg;

	u32 uFbEndAddr;
#if (LCD_MODULE_TYPE == LTS222QV_CPU)	
	u32 uCmdVal[6];	
	u32 uStYQutotient, uStYRemainder;
	u32 uLdiEndX, uLdiEndY;
	u32 uEndYQutotient, uEndYRemainder;
#endif

	uSrcEndX = uSrcStX + uPartialHSz - 1;
	uSrcEndY = uSrcStY + uPartialVsz - 1;

	uFrmBufStAddrReg =
		(eWin == WIN0) ? rVIDW00ADD0B0 :
		(eWin == WIN1) ? rVIDW01ADD0B0:
		(eWin == WIN2) ? rVIDW02ADD0 :
		(eWin == WIN3) ? rVIDW03ADD0 : rVIDW04ADD0;

	uFrmBufEndAddrReg =
		(eWin == WIN0) ? rVIDW00ADD1B0 :
		(eWin == WIN1) ? rVIDW01ADD1B0 :
		(eWin == WIN2) ? rVIDW02ADD1 :
		(eWin == WIN3) ? rVIDW03ADD1 : rVIDW04ADD1;

	uOffsetPageWidthReg =
		(eWin == WIN0) ? rVIDW00ADD2 :
		(eWin == WIN1) ? rVIDW01ADD2 :
		(eWin == WIN2) ? rVIDW02ADD2 :
		(eWin == WIN3) ? rVIDW03ADD2 : rVIDW04ADD2;

	uOsdLeftPosReg =
		(eWin == WIN0) ? rVIDOSD0A :
		(eWin == WIN1) ? rVIDOSD1A :
		(eWin == WIN2) ? rVIDOSD2A :
		(eWin == WIN3) ? rVIDOSD3A : rVIDOSD4A;

	uOsdRightPosReg =
		(eWin == WIN0) ? rVIDOSD0B :
		(eWin == WIN1) ? rVIDOSD1B :
		(eWin == WIN2) ? rVIDOSD2B :
		(eWin == WIN3) ? rVIDOSD3B : rVIDOSD4B;

	uFbEndAddr = uSrcFbStAddr + uPartialHSz*uPartialVsz*oLcdc.m_uBytes/oLcdc.m_uBits;

	//uOffsetSz = (oLcdc.m_uFrmHSz[eWin] - uPartialHSz)*(oLcdc.m_uBytes/oLcdc.m_uBits);
	//uPageWidth = uPartialHSz*(oLcdc.m_uBytes/oLcdc.m_uBits);

	/////////////////////////////////////////////////////////
	// Set LCD Controller's SFR                            //
	/////////////////////////////////////////////////////////

	// Set display size
	LcdcOutp32(rVIDTCON2, LINEVAL(uPartialVsz-1) | HOZVAL(uPartialHSz-1));

	// Set start/ end addr. of frame buffer
	LcdcOutp32(uFrmBufStAddrReg, uSrcFbStAddr);
	LcdcOutp32(uFrmBufEndAddrReg, uFbEndAddr);

	// Set page width and offset size
	LcdcOutp32(uOffsetPageWidthReg, (oLcdc.m_uOffsetSz[eWin]<<13) | oLcdc.m_uPageWidth[eWin]);

	// Set window's left top and right down (X, Y) coordinate
	LcdcOutp32(uOsdLeftPosReg, OSD_LTX_F(uSrcStX) | OSD_LTY_F(uSrcStY));
	LcdcOutp32(uOsdRightPosReg, OSD_RBX_F(uSrcEndX) | OSD_RBY_F(uSrcEndY));


#if (LCD_MODULE_TYPE == LTS222QV_CPU)
	////////////////////////////////////////////////////////
	// LCD_LTS222Q_CPU Register Setting                   //
	////////////////////////////////////////////////////////

	// Set partial display mode
	// LDI Constraint
	Assert( (uDstStX == 0) && (uPartialHSz == 240) );

	uStYQutotient = uDstStY/0xff;
	uStYRemainder = uDstStY%0xff;

	uCmdVal[0] = 0x4200 + uDstStX;
	uCmdVal[1] = 0x4300 + uStYQutotient;
	uCmdVal[2] = 0x4400 + uStYRemainder;

	uLdiEndX = uDstStX + uPartialHSz - 1;
	uLdiEndY = uDstStY + uPartialVsz - 1;

	uEndYQutotient = uLdiEndY/0xff;
	uEndYRemainder = uLdiEndY%0xff;
	LCD_PutCmdToLdi_NormalCmd(uCmdVal, 3, LCD_CPUIF_CS); 
#else
	Assert(0);
#endif

}

void LCD_EnableDithering(u32 uEnable, DITHER_BIT eRedDitherBit, DITHER_BIT eGreenDitherBit, DITHER_BIT eBlueDitherBit)
{	
	Assert( (uEnable == 1) || (uEnable == 0) );

	if(eRedDitherBit == DITHER_5BIT)
		oLcdc.m_uDitherMode = RDITHPOS_5BIT;
	else if(eRedDitherBit == DITHER_6BIT)
		oLcdc.m_uDitherMode = RDITHPOS_6BIT;
	else if(eRedDitherBit == DITHER_8BIT)
		oLcdc.m_uDitherMode = RDITHPOS_8BIT;

	if(eGreenDitherBit == DITHER_5BIT)
		oLcdc.m_uDitherMode |= GDITHPOS_5BIT;
	else if(eGreenDitherBit == DITHER_6BIT)
		oLcdc.m_uDitherMode |= GDITHPOS_6BIT;
	else if(eGreenDitherBit == DITHER_8BIT)
		oLcdc.m_uDitherMode |= GDITHPOS_8BIT;

	if(eBlueDitherBit == DITHER_5BIT)
		oLcdc.m_uDitherMode |= BDITHPOS_5BIT;
	else if(eBlueDitherBit == DITHER_6BIT)
		oLcdc.m_uDitherMode |= BDITHPOS_6BIT;
	else if(eBlueDitherBit == DITHER_8BIT)
		oLcdc.m_uDitherMode |= BDITHPOS_8BIT;

	if(uEnable == 1) 
		oLcdc.m_uDitherMode |= DITHERING_ENABLE;
	else
		oLcdc.m_uDitherMode &= ~DITHERING_ENABLE;
	
	LcdcOutp32(rDITHMODE, oLcdc.m_uDitherMode); 	//Do not write non-zero in [31:7]
}

void LCD_PutCmdtoLMS480KCO(u16 addr, u16 data)
{
	u8 devIDCode=0x1D;
	int j;
	u8 delay=50;
			
	LCD_DEN_Hi; 		//	CS(EN) -> High	
	LCD_DCLK_Hi;		//	SCL(DCLK) -> High
	LCD_DSERI_Hi;		//	MOSI(DSERI) -> High

	delayLoop(delay);

	LCD_DEN_Lo; 		
	delayLoop(delay);
	
	for (j = 5; j >= 0; j--)
	{	
		LCD_DCLK_Lo;	

		if ((devIDCode >> j) & 0x0001)
		{
			LCD_DSERI_Hi;		
		}
		else
		{
			LCD_DSERI_Lo;
		}

		delayLoop(delay);

		LCD_DCLK_Hi;	
		delayLoop(delay);

	}

	// RS = "0" : index data
	LCD_DCLK_Lo;			
	LCD_DSERI_Lo;
	delayLoop(delay);
	LCD_DCLK_Hi;			
	delayLoop(delay);

	// Write
	LCD_DCLK_Lo;			
	LCD_DSERI_Lo;
	delayLoop(delay);
	LCD_DCLK_Hi;			
	delayLoop(delay);

	for (j = 15; j >= 0; j--)
	{
		LCD_DCLK_Lo;						

		if ((addr >> j) & 0x0001)	
		{
			LCD_DSERI_Hi;		
		}
		else
		{
			LCD_DSERI_Lo;
		}

		delayLoop(delay);

		LCD_DCLK_Hi;			
		delayLoop(delay);

	}
	LCD_DSERI_Hi;
	delayLoop(delay);
	
	LCD_DEN_Hi; 				
	delayLoop(delay*10);

	LCD_DEN_Lo; 		
	delayLoop(delay);
	
	for (j = 5; j >= 0; j--)
	{	
		LCD_DCLK_Lo;							

		if ((devIDCode >> j) & 0x0001)	
		{
			LCD_DSERI_Hi;		
		}
		else
		{
			LCD_DSERI_Lo;
		}

		delayLoop(delay);

		LCD_DCLK_Hi;			
		delayLoop(delay);
	}
	
	// RS = "1" instruction data
	LCD_DCLK_Lo;			
	LCD_DSERI_Hi;
	delayLoop(delay);
	LCD_DCLK_Hi;			
	delayLoop(delay);

	// Write
	LCD_DCLK_Lo;			
	LCD_DSERI_Lo;
	delayLoop(delay);
	LCD_DCLK_Hi;			
	delayLoop(delay);

	for (j = 15; j >= 0; j--)
	{
		LCD_DCLK_Lo;							

		if ((data >> j) & 0x0001)	
		{
			LCD_DSERI_Hi;		
		}
		else
		{
			LCD_DSERI_Lo;
		}

		delayLoop(delay);

		LCD_DCLK_Hi;			
		delayLoop(delay);
	}
	
	LCD_DEN_Hi; 				
	delayLoop(delay);

}
//-----------------------------------------------------------
// Function Name : LCD_PutCmdtoLTV350QV
//
// Function Desctiption :
//
// Input :	
//
// Output :
//
// Version : 
//------------------------------------------------------------
void LCD_PutCmdtoLTV350QV(u16 addr, u16 data)
{
	//GPIO Control for SPI with LTV350QV 
	
	u8 devIDCode=0x1D;
    int j;
	u8 delay=50;
			
	LCD_DEN_Hi; 		//	CS(EN) -> High	
	LCD_DCLK_Hi;		//	SCL(DCLK) -> High
	LCD_DSERI_Hi;		//	MOSI(DSERI) -> High

	delayLoop(delay);

	LCD_DEN_Lo; 		
	delayLoop(delay);
	
	for (j = 5; j >= 0; j--)
	{	
		LCD_DCLK_Lo;	

		if ((devIDCode >> j) & 0x0001)
		{
			LCD_DSERI_Hi;		
		}
		else
		{
			LCD_DSERI_Lo;
		}

		delayLoop(delay);

		LCD_DCLK_Hi;	
		delayLoop(delay);

	}
	
	// RS = "0" : index data
	LCD_DCLK_Lo;			
	LCD_DSERI_Lo;
	delayLoop(delay);
	LCD_DCLK_Hi;			
	delayLoop(delay);

	// Write
	LCD_DCLK_Lo;			
	LCD_DSERI_Lo;
	delayLoop(delay);
	LCD_DCLK_Hi;			
	delayLoop(delay);

	for (j = 15; j >= 0; j--)
	{
		LCD_DCLK_Lo;						

		if ((addr >> j) & 0x0001)	
		{
			LCD_DSERI_Hi;		
		}
		else
		{
			LCD_DSERI_Lo;
		}

		delayLoop(delay);

		LCD_DCLK_Hi;			
		delayLoop(delay);

	}
	LCD_DSERI_Hi;
	delayLoop(delay);
	
	LCD_DEN_Hi; 				
	delayLoop(delay*10);

	LCD_DEN_Lo; 		
	delayLoop(delay);
	
	for (j = 5; j >= 0; j--)
	{	
		LCD_DCLK_Lo;							

		if ((devIDCode >> j) & 0x0001)	
		{
			LCD_DSERI_Hi;		
		}
		else
		{
			LCD_DSERI_Lo;
		}

		delayLoop(delay);

		LCD_DCLK_Hi;			
		delayLoop(delay);
	}
	
	// RS = "1" instruction data
	LCD_DCLK_Lo;			
	LCD_DSERI_Hi;
	delayLoop(delay);
	LCD_DCLK_Hi;			
	delayLoop(delay);

	// Write
	LCD_DCLK_Lo;			
	LCD_DSERI_Lo;
	delayLoop(delay);
	LCD_DCLK_Hi;			
	delayLoop(delay);

	for (j = 15; j >= 0; j--)
	{
		LCD_DCLK_Lo;							

		if ((data >> j) & 0x0001)	
		{
			LCD_DSERI_Hi;		
		}
		else
		{
			LCD_DSERI_Lo;
		}

		delayLoop(delay);

		LCD_DCLK_Hi;			
		delayLoop(delay);
	}
	
	LCD_DEN_Hi; 				
	delayLoop(delay);

}

void LCD_SpiSendByte(u16 uData)
{
    int j;
	u8 delay=50;
			
	LCD_DEN_Hi; 		//	CS(EN) -> High	
	LCD_DCLK_Hi;		//	SCL(DCLK) -> High
	LCD_DSERI_Hi;		//	MOSI(DSERI) -> High

	delayLoop(delay);

	LCD_DEN_Lo; 		
	delayLoop(delay);
	
	for (j = 7; j >= 0; j--)
	{
		LCD_DCLK_Lo;							

		if ((uData >> j) & 0x0001)	
		{
			LCD_DSERI_Hi;		
		}
		else
		{
			LCD_DSERI_Lo;
		}

		delayLoop(delay);

		LCD_DCLK_Hi;			

		delayLoop(delay);
	}
	
	LCD_DEN_Hi; 				
	delayLoop(delay);

}


void delayLoop(u32 count) 
{ 
    u32 j; 
    for(j = 0; j < count; j++); 
}


//-----------------------------------------------------------
// Function Name : LCD_PutDataOrCmdToLdi
//
// Function Desctiption :
//
// Input :	
//
// Output :
//
// Version : 
//------------------------------------------------------------
void LCD_PutDataOrCmdToLdi(u32 uData[], u32 uDataNum,
	CPUIF_LDI eLdiMode, CPU_COMMAND_MODE eCmdMode, u8 IsCmd)
{
#if(LCD_MODULE_TYPE == LTS222QV_CPU)
	u32 uCmdCnt;
	u32 uCmdEnVal = 0;
	u32 uEnBit;
	u32 uRsVal=0;
	u32 uCmdBaseAddr;
	u32 uRsBitVal;
	int j;
#endif
	u32 uConRegAddr;
	u32 uConRegVal;

	Assert(0 < uDataNum <= 12);

	if (oLcdc.m_bIsNormalCall == false)
	{
		// In CPU I/F Normal, VIDOUT is determined and LCD is turned on
		// before RS, Enable and Normal Start bits are set
		LcdcOutp32(rVIDCON0, (eLdiMode == MAIN) ? VIDOUT_I80IF0 : VIDOUT_I80IF1);
		LCD_Start();

		oLcdc.m_bIsNormalCall = true;
	}

	if (eCmdMode == LCD_NORMAL_CMD || eCmdMode == LCD_NORMAL_N_AUTO_CMD)
	{
		uConRegAddr = (eLdiMode == MAIN) ? rI80IFCONB0 : rI80IFCONB1;

		// check the LCD_NORMAL_CMD CMD START bit to zero
		while(true)
		{
			uConRegVal=Inp32(LCD_BASE+uConRegAddr);
			if ( (uConRegVal&(1<<9)) == 0)
				break;
		}
	}

	#if(LCD_MODULE_TYPE == LTS222QV_CPU)
	
		uCmdBaseAddr = rLDI_CMD0;
		uCmdCnt = 0;
		uEnBit =
		(eCmdMode == LCD_NORMAL_CMD) ? 0x1 :
		(eCmdMode == LCD_AUTO_CMD) ? 0x2 : 0x3;

		for (j=0; j<uDataNum; j++)
		{
			// set enable bit
			uCmdEnVal |= uEnBit<<(2*uCmdCnt);
			uRsBitVal = (IsCmd == true) ? 0x0 : 0x1;

			// set RS bit to 0
			uRsVal |= (uRsBitVal<<uCmdCnt);

			// set data
			LcdcOutp32(uCmdBaseAddr, uData[j]);
			uCmdBaseAddr += 0x4;
			uCmdCnt++;
		}

		LcdcOutp32(rLDI_CMDCON0, uCmdEnVal);
		LcdcOutp32(rLDI_CMDCON1, uRsVal);


		if (eCmdMode == LCD_NORMAL_CMD || eCmdMode == LCD_NORMAL_N_AUTO_CMD)
		{
			// start NORMAL COMMAND START(NORMAL_CMD_ST)
			LcdcOutp32(uConRegAddr, uConRegVal|(1<<9)); 
		}

	#else
		Assert(0);
	#endif
}


void LCD_PutDataOrCmdToLdi_SelCmdMode(u32 uData[], u32 uDataNum, u32 uModeSel[],
	CPUIF_LDI eLdiMode, u8 IsCmd, u8 IsNormalCmdStart)
{
#if(LCD_MODULE_TYPE == LTS222QV_CPU)
	u32 uCmdCnt;
	u32 uCmdEnVal = 0;
	u32 uEnBit;
	u32 uRsVal=0;
	u32 uCmdBaseAddr;
	u32 uRsBitVal;
	u32 j;
#endif	
	u32 uConRegAddr;
	u32 uConRegVal;

	Assert(0 < uDataNum <= 12);

	if (oLcdc.m_bIsNormalCall == false)
	{
		// In CPU I/F Normal, VIDOUT is determined and LCD is turned on
		// before RS, Enable and Normal Start bits are set
		LcdcOutp32(rVIDCON0, (eLdiMode == MAIN) ? VIDOUT_I80IF0 : VIDOUT_I80IF1);
		LCD_Start();

		oLcdc.m_bIsNormalCall = true;
	}

	if (IsNormalCmdStart == true)
	{
		uConRegAddr = (eLdiMode == MAIN) ? rI80IFCONB0 : rI80IFCONB1;

		// check the LCD_NORMAL_CMD CMD START bit to zero
		while(true)
		{
			uConRegVal=Inp32(LCD_BASE+uConRegAddr);
			if ( (uConRegVal&(1<<9)) == 0)
				break;
		}
	}

	#if(LCD_MODULE_TYPE == LTS222QV_CPU)
	
		uCmdBaseAddr = rLDI_CMD0;
		uCmdCnt = 0;

		for (j=0; j<uDataNum; j++)
		{
		
			if(uModeSel[j] == LCD_DISABLE_CMD)				uEnBit = 0x0;
			else if(uModeSel[j] == LCD_NORMAL_CMD)			uEnBit = 0x1;
			else if(uModeSel[j] == LCD_AUTO_CMD)				uEnBit = 0x2;
			else if(uModeSel[j] == LCD_NORMAL_N_AUTO_CMD)	uEnBit = 0x3;
			
			// set enable bit
			uCmdEnVal |= uEnBit<<(2*uCmdCnt);
			
			uRsBitVal = (IsCmd == true) ? 0x0 : 0x1;

			// set RS bit to 0
			uRsVal |= (uRsBitVal<<uCmdCnt);

			// set data
			LcdcOutp32(uCmdBaseAddr, uData[j]);
			uCmdBaseAddr += 0x4;
			uCmdCnt++;
		}

		UART_Printf("rLDI_CMDCON0: 0x%x\n", uCmdEnVal);
		LcdcOutp32(rLDI_CMDCON0, uCmdEnVal);
		LcdcOutp32(rLDI_CMDCON1, uRsVal);


		if (IsNormalCmdStart == true)
		{
			// start NORMAL COMMAND START(NORMAL_CMD_ST)
			LcdcOutp32(uConRegAddr, uConRegVal|(1<<9)); 

		}

	#else
		Assert(0);
	#endif
}


void LCD_PutDataToLdi(u16 uData, CPUIF_LDI eLdiMode)
{
#if(LCD_MODULE_TYPE == LTS222QV_CPU)
	u32 uMode = 0;
#endif	

	#if(LCD_MODULE_TYPE == LTS222QV_CPU)
		uMode = (eLdiMode == MAIN)? 0x10: 0x8;
		
		LcdcOutp32(rSIFCCON0, uMode|0xe1);	// RS = High, OE = 0, WE = 0, enable = 1
		LcdcOutp32(rSIFCCON0, uMode|0xe3);	// RS = High, OE = 0, WE = 1, enable = 1
		LcdcOutp16(rSIFCCON1, uData);		// Put data
		LcdcOutp32(rSIFCCON0, uMode|0xe1);	// RS = High, OE = 0, WE = 0, enable = 1
		LcdcOutp32(rSIFCCON0, uMode|0x00);	// OE = 0, WE = 0, enable = 0
	#else
		Assert(0);
	#endif
}



void LCD_WriteCmd(CPUIF_LDI eLdiMode, u16 uAddr, u16 uData)
{
	u32 uMode = 0;	

	uMode = (eLdiMode == MAIN)? 0x10: 0x8; 
	
		LcdcOutp32(rSIFCCON0, uMode|0x01); 	//RS = Low, nOE = High, nWE = High, Manual Command Mode
		LcdcOutp32(rSIFCCON0, uMode|0x03);	//RS = Low, nOE = High, nWE = Low, Manual Command Mode

		LCD_SpiSendByte(uAddr);				
		
		LcdcOutp32(rSIFCCON0, uMode|0x01);	//RS = Low, nOE = High, nWE = High, Manual Command Mode
		LcdcOutp32(rSIFCCON0, uMode|0x00);	//CS =High

		delayLoop(100);
			
		LcdcOutp32(rSIFCCON0, uMode|0x01); 	//RS = Low, nOE = High, nWE = High, Manual Command Mode
		LcdcOutp32(rSIFCCON0, uMode|0x03);	//RS = Low, nOE = High, nWE = Low, Manual Command Mode

		LCD_SpiSendByte(uData);

		LcdcOutp32(rSIFCCON0, uMode|0x01);	//RS = Low, nOE = High, nWE = High, Manual Command Mode
		LcdcOutp32(rSIFCCON0, uMode|0x00);	//CS =High
}

//-----------------------------------------------------------
// Function Name : LCD_PutCmdToLdi_NormalCmd
//
// Function Desctiption :
//
// Input :	
//
// Output :
//
// Version : 
//------------------------------------------------------------
void LCD_PutCmdToLdi_NormalCmd(u32 uData[], u32 uDataNum, CPUIF_LDI eLdiMode)
{
	LCD_PutDataOrCmdToLdi(uData, uDataNum, eLdiMode, LCD_NORMAL_CMD, true);
}


void LCD_PutCmdToLdi_AutoCmd(u32 uData[], u32 uDataNum, CPUIF_LDI eLdiMode)
{
	LCD_PutDataOrCmdToLdi(uData, uDataNum, eLdiMode, LCD_AUTO_CMD, true);
}


void LCD_PutCmdToLdi_ManualCmd(u32 uData[], CPUIF_LDI eLdiMode)
{
#if(LCD_MODULE_TYPE == LTS222QV_CPU)
	u32 uMode;
	u32 Data;	

	Data=uData[0];
#endif

	#if(LCD_MODULE_TYPE ==LTS222QV_CPU)
		uMode = (eLdiMode == MAIN)? 0x10: 0x8; 
		LcdcOutp32(rSIFCCON0, uMode|0x01); 	//RS = Low, nOE = High, nWE = High, Manual Command Mode
		LcdcOutp32(rSIFCCON0, uMode|0x03);	//RS = Low, nOE = High, nWE = Low, Manual Command Mode
		LcdcOutp32(rSIFCCON1, Data);		 	// Put Cmd
		LcdcOutp32(rSIFCCON0, uMode|0x01);	//RS = Low, nOE = High, nWE = High, Manual Command Mode
		LcdcOutp32(rSIFCCON0, uMode|0x00);	//CS =High
	#else
		Assert(0);
	#endif
}

void LCD_GetDataFromLdi(u32* uData, CPUIF_LDI eLdiMode)
{
	#if(LCD_MODULE_TYPE == LTS222QV_CPU)
	u32 uMode = 0;
	
		uMode = (eLdiMode == MAIN)? 0x10: 0x8;	
	
		LcdcOutp32(rSIFCCON0, uMode|0xe1);	//RS = High, nOE = High, nWE = High, Manual Command Mode
		LcdcOutp32(rSIFCCON0, uMode|0xe5);	//RS = High, nOE = Low, nWE = High, Manual Command Mode 
		//Delay(1);
		*uData=Inp32(LCD_BASE+rSIFCCON2);	// Get Data
		LcdcOutp32(rSIFCCON0, uMode|0xe1);	//RS = High, nOE = High, nWE = High, Manual Command Mode
		LcdcOutp32(rSIFCCON0, uMode|0x00);	//CS =High
	#else
		Assert(0);
	#endif
}

void LCD_GetCmdFromLdi(u32* uCmd, CPUIF_LDI eLdiMode)
{
	#if(LCD_MODULE_TYPE == LTS222QV_CPU)
	u32 uMode;

		uMode = (eLdiMode == MAIN)? 0x10: 0x8;
		
		LcdcOutp32(rSIFCCON0, uMode|0x01);	//RS = Low, nOE = High, nWE = High, Manual Command Mode
		LcdcOutp32(rSIFCCON0, uMode|0x05); 	//RS = Low, nOE = Low, nWE = High, Manual Command Mode 
	
		*uCmd =Inp32(LCD_BASE+rSIFCCON2); // get command 
		
		LcdcOutp32(rSIFCCON0, uMode|0x01);	//RS = Low, nOE = High, nWE = High, Manual Command Mode
		LcdcOutp32(rSIFCCON0, uMode|0x00);	//CS =High
		
	#else
		Assert(0);
	#endif
}

u8 LCD_IsFrameDone(void)
{
	u32 uTrigConVal;

	uTrigConVal=Inp32(LCD_BASE+rTRIGCON);

	// LCDC_IsFrameDone Function is valid only when TRGMODE[0] is set to 1
	Assert( (uTrigConVal&(0x1<<0)) == (0x1<<0) );

	return ( ((uTrigConVal&(0x1<<2)) == (0x1<<2)) ? true : false );
}


void ConvertCSpaceToString(CSPACE eBpp, char* pcBpp)
{
	switch (eBpp)
	{
		case PAL1:
			strcpy(pcBpp, "1Bpp (Palette)");
			break;
		case PAL2:
			strcpy(pcBpp, "2Bpp (Palette)");
			break;
		case PAL4:
			strcpy(pcBpp, "4Bpp (Palette)");
			break;
		case PAL8:
			strcpy(pcBpp, "8Bpp (Palette)");
			break;
		case RGB8:
			strcpy(pcBpp, "RGB 8Bpp (R:3 G:3 B:2)");
			break;
		case RGB16:
			strcpy(pcBpp, "RGB 16Bpp (R:5 G:6 B:5)");
			break;
		case RGB24:
			strcpy(pcBpp, "RGB 24Bpp (R:8 G:8 B:8)");
			break;
		case ARGB8:
			strcpy(pcBpp, "ARGB 8Bpp (A:1 R:2 G:3 B:2)");
			break;
		case ARGB16:
			strcpy(pcBpp, "ARGB 16Bpp (A:1 R:5 G:5 B:5)");
			break;
		case ARGB24:
			strcpy(pcBpp, "ARGB 24Bpp (A:1 R:8 G:8 B:7)");
			break;
		default:
			Assert(0);
	}
}


void LCD_InitInt(LCD_INT eLcdInt, LCD_INT_SYS eLcdIntSys, FIFO_INT_LEVEL eFifoIntLevel, FIFO_INT_SEL eFifoIntSel, FRAME_INT_SEL eFIntSel )
{
	u32 uIntCtrl;

	uIntCtrl=Inp32(LCD_BASE+rVIDINTCON0);

	if (eLcdInt == LCD_FIFO_INT)
	{
		uIntCtrl &= (~(0x7F<<5)) & (~(0x7<<2)) & (~INTFIFOEN_ENABLE);
				
		if (eFifoIntLevel == FIFO_25)  			uIntCtrl = uIntCtrl |FIFOLEVEL_25; 
		else if (eFifoIntLevel == FIFO_50)		uIntCtrl = uIntCtrl |FIFOLEVEL_50; 
		else if (eFifoIntLevel == FIFO_75)		uIntCtrl = uIntCtrl |FIFOLEVEL_75; 
		else if (eFifoIntLevel ==  FIFO_EMPTY)	uIntCtrl = uIntCtrl |FIFOLEVEL_EMPTY; 
		else if (eFifoIntLevel ==  FIFO_FULL) 	uIntCtrl = uIntCtrl |FIFOLEVEL_FULL; 

		if(eFifoIntSel == FIFO_INT_WIN0)  		uIntCtrl = uIntCtrl |FIFOSEL_WIN0; 
		else if (eFifoIntSel == FIFO_INT_WIN1)	uIntCtrl = uIntCtrl |FIFOSEL_WIN1; 
		else if (eFifoIntSel == FIFO_INT_WIN2)	uIntCtrl = uIntCtrl |FIFOSEL_WIN2; 
		else if (eFifoIntSel == FIFO_INT_WIN3)	uIntCtrl = uIntCtrl |FIFOSEL_WIN3; 
		else if (eFifoIntSel == FIFO_INT_WIN4) 	uIntCtrl = uIntCtrl |FIFOSEL_WIN4; 

		uIntCtrl = uIntCtrl |INTFIFOEN_ENABLE;		
	}
	else if (eLcdInt == LCD_FRAME_INT)
	{
		uIntCtrl &= (~(0x3<<15)) & (~(0x3<<13)) & (~INTFRMEN_ENABLE);
				
		if (eFIntSel == FINT_BACKPORCH )  		 uIntCtrl = uIntCtrl |FRAMESEL0_BACK; 
		else if (eFIntSel == FINT_VSYNC)		 uIntCtrl = uIntCtrl |FRAMESEL0_VSYNC; 
		else if (eFIntSel == FINT_ACTIVE)		 uIntCtrl = uIntCtrl |FRAMESEL0_ACTIVE; 
		else if (eFIntSel == FINT_FRONTPORCH)	 uIntCtrl = uIntCtrl |FRAMESEL0_FRONT; 

		uIntCtrl |= INTFRMEN_ENABLE;	
	}
	else if (eLcdInt ==  LCD_SYSIF_INT)
	{
		uIntCtrl = uIntCtrl & (~(0x1<<19)) & (~(0x1<<18)) & (~SYSIFDONE_ENABLE);
				
		if (eLcdIntSys == LCD_SYSMAIN_INT)			
			uIntCtrl = uIntCtrl |SYSMAINCON_ENABLE; 
		else if (eLcdIntSys == LCD_SYSSUB_INT)  	
			uIntCtrl = uIntCtrl |SYSSUBCON_ENABLE; 				

		uIntCtrl = uIntCtrl |SYSIFDONE_ENABLE;	
	}

	UART_Printf("\n rVIDINTCON0: 0x%x  \n", uIntCtrl);	
	LcdcOutp32(rVIDINTCON0, uIntCtrl);

	return;
}

void LCD_EnableInt(void)
{
	u32 uIntCtrl;

	uIntCtrl=Inp32(LCD_BASE+rVIDINTCON0);
	
	LcdcOutp32(rVIDINTCON0, uIntCtrl | INTEN_ENABLE);

	return;
}

void LCD_DisableInt(void)
{
	u32 uIntCtrl;

	uIntCtrl=Inp32(LCD_BASE+rVIDINTCON0);
	
	LcdcOutp32(rVIDINTCON0, uIntCtrl & (~ INTEN_ENABLE));

	return;
}

void LCD_ClearInt(LCD_INT eLcdInt)
{
	u32 uTemp;

	uTemp = Inp32(LCD_BASE+rVIDINTCON1);
	
	if (eLcdInt == LCD_FIFO_INT)			LcdcOutp32(rVIDINTCON1, uTemp|INTFIFOPEND_CLEAR);
	else if (eLcdInt == LCD_FRAME_INT) 	LcdcOutp32(rVIDINTCON1, uTemp|INTFRMPEND_CLEAR);
	else if (eLcdInt ==  LCD_SYSIF_INT)	LcdcOutp32(rVIDINTCON1, uTemp|INTI80PEND_CLEAR);
}


 void __irq Isr_LCD_FrameCount(void)
 {
	volatile u32 temp;
	
	INTC_Disable(NUM_LCD1);
	LCD_DisableInt();

	uFrameCount++;


	LCD_ClearInt(LCD_FRAME_INT);
	INTC_ClearVectAddr();
	
	INTC_Enable(NUM_LCD1);
	LCD_EnableInt();
}




void LCD_InitBt601And656If(u32 uHsz, u32 uVsz)		// In order to transfer from FIMDv4.2 ITU port from FIMD ITU port 
{
	u32 uLcdCon;
	u32 uClkVal, uClkDir;
	u32 uVidconReg;
	LCD_WINDOW eWin;
	CLK_SRC eVClkSrc;
	eVClkSrc = SRC_MOUT_EPLL;

	oLcdc.m_uLcdHSz = uHsz;
	oLcdc.m_uLcdVSz = uVsz;
	oLcdc.m_uVidOutFormat = VIDOUT_TV;
	oLcdc.m_uDitherMode = RDITHPOS_5BIT|GDITHPOS_6BIT|BDITHPOS_5BIT;
	oLcdc.m_uDitherMode &= ~DITHERING_ENABLE;

#if 0
	LcdcOutp32(VIDCON1, IVCLK_FALL_EDGE | IHSYNC_INVERT | IVSYNC_INVERT | IVDEN_NORMAL);
#else
	LcdcOutp32(rVIDCON1, IVCLK_FALL_EDGE | IHSYNC_NORMAL | IVSYNC_NORMAL | IVDEN_NORMAL);
#endif	
	LcdcOutp32(rDITHMODE, oLcdc.m_uDitherMode); // Fixed Dithering Matrix

	LCD_Stop();

	// Check up LCD to turn off
	while (1)
	{
		uLcdCon = Inp32(LCD_BASE+rVIDCON0);
//		LcdcInp32(rVIDCON0, uLcdCon);
		if( (uLcdCon&0x03) == 0 ) // checking whether disable the video output and the Display control signal or not.
			break;
	}

	LCD_GetClkValAndClkDir(oLcdc.m_uLcdHSz, oLcdc.m_uLcdVSz, &uClkVal, &uClkDir,eVClkSrc);

	uVidconReg =
		PROGRESSIVE | oLcdc.m_uVidOutFormat | SUB_16_MODE | MAIN_16_MODE | PNRMODE_RGB_P | CLKVALUP_ALWAYS |
		CLKVAL_F(uClkVal) | VCLKFREE_NORMAL | CLKDIR_F(uClkDir) | CLKSEL_F_HCLK |
		ENVID_DISABLE | ENVID_F_DISABLE;
	LcdcOutp32(rVIDCON0, uVidconReg);
	LcdcOutp32(rVIDINTCON0, FRAMESEL0_BACK | FRAMESEL1_NONE | INTFRMEN_DISABLE |
				FIFOSEL_WIN0 | FIFOLEVEL_25 | INTFIFOEN_DISABLE | INTEN_DISABLE);
}


void LCD_SetBt601And656CtrlClk(u32 bIsVsyncDelayed, u32 bIs656clkInverted, u32 bIs601HrefInverted, u32 bIs601VsyncInverted,
								 u32 bIsHsyncInverted, u32 bIs601FieldInverted, u32 bIs601ClkInverted)
{	
	u32 uClkCon;

	oLcdc.m_bIsDelayVsync = bIsVsyncDelayed;

	uClkCon = Inp32(LCD_BASE+rITUIFCON0);
//	LcdcInp32(rITUIFCON0,uClkCon);	
	uClkCon=0;

	uClkCon |= (bIsVsyncDelayed<<24) | (bIs656clkInverted<<8)| (bIs601HrefInverted<<6) | 
		(bIs601VsyncInverted<<5) | (bIsHsyncInverted<<4) | (bIs601FieldInverted<<3) | (bIs601ClkInverted<<2);
		
	LcdcOutp32(rITUIFCON0,uClkCon);	
}

void LCD_SetBT601VsyncDly(u32 DelayCycle)
{
	u32 uVsyncTiming;

	Assert(DelayCycle>=64 && DelayCycle<=255);
	
//	LcdcInp32(rITUIFCON0,uVsyncTiming);	
	uVsyncTiming = Inp32(LCD_BASE+rITUIFCON0);

	if( oLcdc.m_bIsDelayVsync)
	{
		uVsyncTiming |=	CYCLE_DELAY_VSYNC(DelayCycle);
		LcdcOutp32(rITUIFCON0, uVsyncTiming);
	}
	
	else
		Assert(0);
}

void LCD_SetInterlace(void)
{
	u32 uLcdCon;

	uLcdCon = Inp32(LCD_BASE+rVIDCON0);
	
//	LcdcInp32(rVIDCON0, uLcdCon);
	uLcdCon |= (1<<29);
	LcdcOutp32(rVIDCON0, uLcdCon);
}

void LCD_SetITUInterface(void)
{
	u32 uTemp0, uTemp1;
	u32 uVIDCONReg, uITUIFCON0reg, uVIDCON2reg;
	u32 uSPCONreg;

	uVIDCONReg = Inp32(LCD_BASE+rVIDCON0);
	uITUIFCON0reg = Inp32(LCD_BASE+rITUIFCON0);
	uVIDCON2reg = Inp32(LCD_BASE+rVIDCON2);

	uVIDCONReg = (uVIDCONReg & ~(0x1<<29) ) | (0x1<<29); // Interlace setting
	uVIDCONReg = (uVIDCONReg & ~(0x3<<26) ) | (0x1<<26); // TV(ITU) Interface setting.
	uVIDCON2reg = (uVIDCON2reg &~(0x3<<22)) | (0x1<<23); // ITU601 setting
	uVIDCON2reg = (uVIDCON2reg &~(0x7<<12)) | (0x5<<12); // Software and YUV422 setting.
	
	UART_Printf("Select Interface ---- 1: Delay count mode ,  2: Field signal mode-----\n");
	uTemp0 = UART_GetIntNum();

	switch ( uTemp0 )
	{
		case 1: // Delay count mode
			UART_Printf("Input delay count num =\n");
			uTemp1 = UART_GetIntNum();
//			uVIDCONReg = (uVIDCONReg & ~(0x1<<29) ); // Progessive setting
			uITUIFCON0reg = ( uITUIFCON0reg & ~(0x1<<24) ) | (0x1<<24); // delayed vsync.
			uITUIFCON0reg = ( uITUIFCON0reg & ~(0xff<<16) ) | (uTemp1<<16); // delay count num setting.
			break;
		case 2: // Field signal mode
			uITUIFCON0reg = ( uITUIFCON0reg & ~(0x1<<24) ); // field signal mode
			uITUIFCON0reg = ( uITUIFCON0reg & ~(0xff<<16) ); // delay count num zero setting.
			break;
	}

	*(unsigned volatile *)0x7f0081a0 = ((*(unsigned volatile *)0x7f0081a0)&~(0x3<<0))|(2<<0); // GPIO Interface ITU setting.

	LcdcOutp32(rVIDCON0, uVIDCONReg); // VIDCON reg setting.
	LcdcOutp32(rITUIFCON0, uITUIFCON0reg); // ITU Interface reg setting.
	LcdcOutp32(rVIDCON2, uVIDCON2reg); // ITU Interface reg setting.	
	
}


void LCD_SetITUInterface_656(void)
{
	u32 uTemp0, uTemp1;
	u32 uVIDCONReg, uITUIFCON0reg, uVIDCON2reg;
	u32 uSPCONreg;

	uVIDCONReg = Inp32(LCD_BASE+rVIDCON0);
	uITUIFCON0reg = Inp32(LCD_BASE+rITUIFCON0);
	uVIDCON2reg = Inp32(LCD_BASE+rVIDCON2);

	uVIDCONReg = (uVIDCONReg & ~(0x1<<29) ) | (0x1<<29); // Interlace setting
	uVIDCONReg = (uVIDCONReg & ~(0x3<<26) ) | (0x1<<26); // TV(ITU) Interface setting.
	uVIDCON2reg = (uVIDCON2reg &~(0x3<<22)) | (0x1<<22); // ITU656 setting
	uVIDCON2reg = (uVIDCON2reg &~(0x7<<12)) | (0x5<<12); // Software and YUV422 setting. YUV 422 is valid at ITU 601, ITU656 format. YCBYCR Format Set
	
	uITUIFCON0reg = 0x0;
	uITUIFCON0reg = ( uITUIFCON0reg & ~(0x3<<8) ) | (0x0<<8); // V656_CLK active edge 0: normal, 1 : inverted
	uITUIFCON0reg = ( uITUIFCON0reg & ~(0x3<<8) ) | (0x0<<9); // polarity of F value 0: normal 1: inverted

	*(unsigned volatile *)0x7f0081a0 = ((*(unsigned volatile *)0x7f0081a0)&~(0x3<<0))|(2<<0); // GPIO Interface ITU setting.

	LcdcOutp32(rVIDCON0, uVIDCONReg); // VIDCON reg setting.
	LcdcOutp32(rITUIFCON0, uITUIFCON0reg); // ITU Interface reg setting.
	LcdcOutp32(rVIDCON2, uVIDCON2reg); // ITU Interface reg setting.	
	
}




