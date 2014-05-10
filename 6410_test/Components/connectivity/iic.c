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
*	File Name : iic.c
*  
*	File Description : This file implements the API functons for IIC
*
*	Author : Woojin.Kim
*	Dept. : AP Development Team
*	Created Date : 2007/01/11
*	Version : 0.1 
* 
*	History
*	- Created(Woojin.Kim 2007/01/11)
*	- Edited (OnPil,Shin (SOP) 2008/03/01 ~ 2008/03/04)
*  
**************************************************************************************/

#include "iic.h"
#include "option.h"
#include "library.h"
#include "sfr6410.h"
#include "system.h"
#include "gpio.h"
#include "intc.h"
#include "def.h"
#include "sysc.h"

//#define IICBUFSIZE	0x20

//#define VIC0_BASE			0x71200000
//#define VIC1_BASE			0x71300000

#define rVIC0IRQSTATUS		(VIC0_BASE+0x000)
#define rVIC1IRQSTATUS		(VIC1_BASE+0x000)

#define rVIC0INTENABLE		(VIC0_BASE+0x010)
#define rVIC1INTENABLE		(VIC1_BASE+0x010)

static volatile u8 *g_PcIIC_BUFFER;
static volatile u32 g_uIIC_PT;
static volatile u32 g_uIIC_DATALEN;		// Edited by SOP on 2008/03/04
static volatile u8 g_cIIC_STAT0;
static volatile u8 g_cIIC_SlaveRxDone;
static volatile u8 g_cIIC_SlaveTxDone;


//////////
// Function Name : Isr_IIC
// Function Description : This function is Interrupt Service Routine of IIC
//					  when interrupt occurs, check IICSTAT, find mode and operate it
// Input : NONE
// Output : NONE
// Version : v0.1
void __irq Isr_IIC( void)
{
	u32 uTemp0 = 0;
	u8 cCnt;
	
	g_cIIC_STAT0 = Inp32(rIICSTAT0);

	switch( (g_cIIC_STAT0>>6)&0x3) 
	{
		case SlaveRX	:	
			//UART_Printf("SlaveRx (Interrupt Mode) g_uIIC_PT Count: %d\n",g_uIIC_PT);			

			if(g_uIIC_PT<101)		// Edited by SOP on 2008/05/23 				
			{
				//UART_Printf("SlaveRx (Interrupt Mode) g_uIIC_PT Count: %d\n",g_uIIC_PT);				
				g_PcIIC_BUFFER[g_uIIC_PT++]=Inp8(rIICDS0);
				Delay(100);
				
				uTemp0 = Inp32(rIICCON0);
				uTemp0 &= ~(1<<4);			//	Clear pending bit to resume
				uTemp0 &= ~(1<<7);			// Disable Ack generation, Added by SOP on 04/11/2008				
				Outp32(rIICCON0,uTemp0);
				
				if(g_uIIC_PT>100) 
				{
					Outp8(rIICSTAT0,0x0);			// Stop Int
					
					uTemp0 = Inp32(rIICCON0);
					uTemp0 &= ~(1<<4);			//	Clear pending bit to resume
					Outp32(rIICCON0,uTemp0);

					g_cIIC_SlaveRxDone = 1;

					Delay(1);					//	wait until Stop Condition is in effect
					
					//UART_Printf("*** IIC0 ISR SlaveRX\n");		// For Debugging	
					break;
				}				
				break;
			}
		case SlaveTX	:		
			if(g_uIIC_PT>100)
			{
				Outp32(rIICSTAT0,0xd0);		//	Stop Master Tx condition, ACK flag clear

				uTemp0 = Inp32(rIICCON0);
				uTemp0 &= ~(1<<4);			//	Clear pending bit to resume
				Outp32(rIICCON0,uTemp0);

				g_cIIC_SlaveTxDone = 1;

				Delay(1);					//	wait until Stop Condition is in effect
				break;
			}

			Outp8(rIICDS0,g_PcIIC_BUFFER[g_uIIC_PT++]);
			for(cCnt=0;cCnt<10;cCnt++);	//	for setup time (rising edge of IICSCL)
			Delay(100);
			
			uTemp0 = Inp32(rIICCON0);
			uTemp0 &= ~(1<<4);			//	Clear pending bit to resume
			Outp32(rIICCON0,uTemp0);
			
			//UART_Printf("*** IIC0 ISR SlaveTX\n");		// For Debugging				
			break;

		case MasterRX:
			//UART_Printf("*** Entering IIC0 ISR MasterRX\n");		// For Debugging
			
			if (g_uIIC_PT>0)
				g_PcIIC_BUFFER[g_uIIC_PT-1] = Inp32(rIICDS0);

			g_uIIC_PT++;

			if (g_uIIC_PT==g_uIIC_DATALEN)
			{
				uTemp0 = Inp32(rIICCON0);
				uTemp0 |= (1<<7);			// Enable Ack generation				
				Outp32(rIICCON0,uTemp0);	//
			}
			else if (g_uIIC_PT > g_uIIC_DATALEN)
			{
				// Added by SOP on 04/11/2008 -----
				uTemp0 = Inp32(rIICCON0);
				uTemp0 &= ~(1<<7);			// Disable Ack generation
				Outp32(rIICCON0,uTemp0);	// 
				//----------------------------------				

				Outp32(rIICSTAT0,0x90);		//	Stop Master Rx condition
			}

			uTemp0 = Inp32(rIICCON0);
			uTemp0 &= ~(1<<4);			//	Clear pending bit to resume
			Outp32(rIICCON0,uTemp0);
			
			Delay(1);		// wait until the stop condition takes effect.
			//UART_Printf("*** Ended IIC0 ISR MasterRX\n");		// For Debugging			
			break;

			//Outp32(rIICSTAT0,0xB0);				//	Master Rx Start.

		case MasterTX:
			//UART_Printf("*** Entering IIC0 ISR MasterTX (rIICCON0 =>0xb8): 0x%02x\n", Inp32(rIICCON0));	// For Debugging
			//UART_Printf("*** IIC0 ISR MasterTX (rVIC1IRQSTATUS): 0x%08x\n", Inp32(rVIC1IRQSTATUS));	// For Debugging
			
			//uTmp2 = Inp32(rVIC1IRQSTATUS);
			//uTmp2 &= (1<<18);						
			//UART_Printf("*** IIC0 ISR MasterTX (rVIC1IRQSTATUS =>1): %d\n", (uTmp2>>18));			
			
			if (g_uIIC_PT<g_uIIC_DATALEN)
			{
				Outp32(rIICDS0,g_PcIIC_BUFFER[g_uIIC_PT]);
			}				
			else	
			{
				// Added by SOP on 04/11/2008 -----
				uTemp0 = Inp32(rIICCON0);
				uTemp0 &= ~(1<<7);			// Disable Ack generation
				Outp32(rIICCON0,uTemp0);	// 
				//----------------------------------

				Outp32(rIICSTAT0,0xd0);	// Stop Master Tx condition, ACK flag clear
				g_uIIC_PT++;			// Added For Test by SOP on 2008/05/27				
			}

			g_uIIC_PT++;
			uTemp0 = Inp32(rIICCON0);
			uTemp0 &= ~(1<<4);			//	Clear pending bit to resume
			Outp32(rIICCON0,uTemp0);
			
			Delay(1);	// wait until Stop Condition is in effect, Need to here Delay(1)	
			//UART_Printf("*** Ended IIC ISR MasterTX (rIICCON0 =>0x28 or a8): 0x%02x\n", uTemp0);		// For Debugging			

			//Outp32(rIICSTAT0,0xf0);				// Master Tx Start.				
			break;
	}

	g_cIIC_STAT0	&=0xf;
	INTC_ClearVectAddr();
}

//////////
// Function Name : Isr_IIC1
// Function Description : This function is Interrupt Service Routine of IIC
//					  when interrupt occurs, check IICSTAT, find mode and operate it
// Input : NONE
// Output : NONE
// Version : v0.1
void __irq Isr_IIC1( void)
{
	u32 uTemp0 = 0;
	u32 uTmp0;
	u8 cCnt;
	
	g_cIIC_STAT0 = Inp32(rIICSTAT1);

	switch( (g_cIIC_STAT0>>6)&0x3) 
	{
		case SlaveRX	:	
			//UART_Printf("SlaveRx (Interrupt Mode) g_uIIC_PT Count: %d\n",g_uIIC_PT);			

			if(g_uIIC_PT<101)	// Edited by SOP on 2008/05/23  				
			{
				//UART_Printf("SlaveRx (Interrupt Mode) g_uIIC_PT Count: %d\n",g_uIIC_PT);				
				g_PcIIC_BUFFER[g_uIIC_PT++]=Inp8(rIICDS1);
				Delay(100);
				
				uTemp0 = Inp32(rIICCON1);
				uTemp0 &= ~(1<<4);			//	Clear pending bit to resume
				uTemp0 &= ~(1<<7);			// Disable Ack generation, Added by SOP on 04/11/2008				
				Outp32(rIICCON1,uTemp0);
				
				if(g_uIIC_PT>100)
				{
					Outp8(rIICSTAT1,0x0);			// Stop Int
					
					uTemp0 = Inp32(rIICCON1);
					uTemp0 &= ~(1<<4);			//	Clear pending bit to resume
					Outp32(rIICCON1,uTemp0);

					g_cIIC_SlaveRxDone = 1;

					Delay(1);					//	wait until Stop Condition is in effect
					
					//UART_Printf("*** IIC1 ISR SlaveRX\n");		// For Debugging	
					break;
				}					
				break;
			}

		case SlaveTX	:		
			if(g_uIIC_PT>100)
			{
				Outp32(rIICSTAT1,0xd0);		//	Stop Master Tx condition, ACK flag clear

				uTemp0 = Inp32(rIICCON1);
				uTemp0 &= ~(1<<4);			//	Clear pending bit to resume
				Outp32(rIICCON1,uTemp0);

				g_cIIC_SlaveTxDone = 1;

				Delay(1);					//	wait until Stop Condition is in effect
				break;
			}

			Outp8(rIICDS1,g_PcIIC_BUFFER[g_uIIC_PT++]);
			for(cCnt=0;cCnt<10;cCnt++);	//	for setup time (rising edge of IICSCL)
			Delay(100);
			
			uTemp0 = Inp32(rIICCON1);
			uTemp0 &= ~(1<<4);			//	Clear pending bit to resume
			Outp32(rIICCON1,uTemp0);
			
			//UART_Printf("*** IIC1 ISR SlaveTX\n");		// For Debugging				
			break;

		case MasterRX:
			//UART_Printf("*** Entering IIC1 ISR MasterRX\n");		// For Debugging
		
			if (g_uIIC_PT>0)
			{
				g_PcIIC_BUFFER[g_uIIC_PT-1] = Inp32(rIICDS1);
				//UART_Printf("R:%02x ", g_PcIIC_BUFFER[g_uIIC_PT-1]);				
			}
			
			g_uIIC_PT++;

			if (g_uIIC_PT==g_uIIC_DATALEN)
			{
				uTemp0 = Inp32(rIICCON1);
				uTemp0 |= (1<<7);			// Enable Ack generation				
				Outp32(rIICCON1,uTemp0);	//		
			}
			else if (g_uIIC_PT > g_uIIC_DATALEN)
			{
				// Added by SOP on 04/11/2008 -----
				uTemp0 = Inp32(rIICCON1);
				uTemp0 &= ~(1<<7);			// Disable Ack generation
				Outp32(rIICCON1,uTemp0);	// 
				//----------------------------------				

				Outp32(rIICSTAT1,0x90);		//	Stop Master Rx condition
				g_uIIC_DATALEN = 0;		// For test by SOP 2008/05/27					
			}

			uTemp0 = Inp32(rIICCON1);
			uTemp0 &= ~(1<<4);			//	Clear pending bit to resume
			Outp32(rIICCON1,uTemp0);
			
			Delay(1);		// wait until the stop condition takes effect.
			//UART_Printf("*** Ended IIC1 ISR MasterRX\n");		// For Debugging			
			break;

			//Outp32(rIICSTAT1,0xB0);				//	Master Rx Start.

		case MasterTX:
			//UART_Printf("*** Entering IIC1 ISR MasterTX (rIICCON1 =>0xb8): 0x%02x\n", Inp32(rIICCON1));	// For Debugging
			//UART_Printf("*** IIC1 ISR MasterTX (rVIC0IRQSTATUS): 0x%08x\n", Inp32(rVIC0IRQSTATUS));	// For Debugging
			
			//uTmp2 = Inp32(rVIC0IRQSTATUS);
			//uTmp2 &= (1<<18);						
			//UART_Printf("*** IIC1 ISR MasterTX (rVIC0IRQSTATUS =>1): %d\n", (uTmp2>>18));	

			if (g_uIIC_PT<g_uIIC_DATALEN)
			{
				Outp32(rIICDS1,g_PcIIC_BUFFER[g_uIIC_PT]);
				//UART_Printf("W:%02x ", g_PcIIC_BUFFER[g_uIIC_PT]);				
			}				
			else	
			{
				// Added by SOP on 04/11/2008 -----
				uTemp0 = Inp32(rIICCON1);
				uTemp0 &= ~(1<<7);			// Disable Ack generation
				Outp32(rIICCON1,uTemp0);	// 
				//----------------------------------

				Outp32(rIICSTAT1,0xd0);	//	Stop Master Tx condition, ACK flag clear
				g_uIIC_PT++;			// Added For Test by SOP on 2008/05/27					
			}

			g_uIIC_PT++;
			uTemp0 = Inp32(rIICCON1);
			uTemp0 &= ~(1<<4);			//	Clear pending bit to resume
			Outp32(rIICCON1,uTemp0);
			
			Delay(1);	// wait until Stop Condition is in effect, Need to here Delay(1)	
			//UART_Printf("*** Ended IIC1 ISR MasterTX (rIICCON1 =>0x28 or a8): 0x%02x\n", uTemp0);		// For Debugging			

			//Outp32(rIICSTAT1,0xf0);				// Master Tx Start.				
			break;
	}

	g_cIIC_STAT0	&=0xf;
	INTC_ClearVectAddr();
}



//////////
// Function Name : IIC_Open
// Function Description : This function Set up VIC & IICCON with user's input frequency which determines uClkValue
// Input : ufreq	ufreq(Hz) = PCLK/16/uClkValue
// Output : NONE
// Version : v0.1
void IIC_Open( u32 ufreq)		//	Hz order. freq(Hz) = PCLK/16/clk_divider
{
	u32	uSelClkSrc;
	u32	uClkValue;

	UART_Printf("\n*** IIC0 Setting Frequency: %d Hz\n",ufreq);

	INTC_SetVectAddr(NUM_IIC,Isr_IIC);
	INTC_Enable(NUM_IIC);

	GPIO_SetFunctionEach(eGPIO_B,eGPIO_5,2);
	GPIO_SetFunctionEach(eGPIO_B,eGPIO_6,2);	
	GPIO_SetPullUpDownEach(eGPIO_B,eGPIO_5,2);
	GPIO_SetPullUpDownEach(eGPIO_B,eGPIO_6,2);

	if ((((g_PCLK>>4)/ufreq)-1)>0xf) 
	{
		uSelClkSrc	=	1;
		uClkValue	=	((g_PCLK>>9)/ufreq) -1;		//	PCLK/512/freq
		UART_Printf("*** PCLK= %.2fMHz,  SelClkSrc= %d (IICCLK=PCLK/512)   ,ClkDivideValue= %d\n",(float)g_PCLK/1.0e6,uSelClkSrc,uClkValue);		
		UART_Printf("*** IICCLK= %d Hz,  IICSCL (Tx clock)=  %d Hz\n",(g_PCLK/512),((g_PCLK/512)/(uClkValue+1)));			
	} 
	else 
	{
		uSelClkSrc	=	0;
		uClkValue	=	((g_PCLK>>4)/ufreq) -1;		//	PCLK/16/freq
		UART_Printf("*** PCLK= %.2fMHz,  SelClkSrc= %d (IICCLK=PCLK/16)   ,ClkDivideValue= %d\n",(float)g_PCLK/1.0e6,uSelClkSrc,uClkValue);
		UART_Printf("*** IICCLK= %d Hz,  IICSCL (Tx clock)=  %d Hz\n",(g_PCLK/16),((g_PCLK/16)/(uClkValue+1)));		
	}

	//Prescaler IICCLK=PCLK/16, Enable interrupt, Transmit clock value Tx clock=IICCLK/16
	Outp32(rIICCON0,(uSelClkSrc<<6) | (1<<5) | (uClkValue&0xf));
	Outp32(rIICADD0,0xc0);		//	Slave address = [7:1]

	Outp32(rIICSTAT0,0x10);		//	IIC bus data output enable(Rx/Tx)
	Outp32(rIICLC0,0x4);			//	SDA Filter Enable,delayed 0clks
	//Outp32(rIICLC0,0x5);		//	SDA Filter enable,delayed 5clks
	//Outp32(rIICLC0,0x6);		//	SDA Filter enable,delayed 10clks
	//Outp32(rIICLC0,0x7);		//	SDA Filter enable,delayed 15clks	
}


//////////
// Function Name : IIC1_Open
// Function Description : This function Set up VIC & IICCON with user's input frequency which determines uClkValue
// Input : ufreq	ufreq(Hz) = PCLK/16/uClkValue
// Output : NONE
// Version : v0.1
void IIC1_Open( u32 ufreq)		//	Hz order. freq(Hz) = PCLK/16/clk_divider
{
	u32	uSelClkSrc;
	u32	uClkValue;

	UART_Printf("\n*** IIC1 Setting Frequency: %d Hz\n",ufreq);
	
	INTC_SetVectAddr(NUM_I2C1,Isr_IIC1);
	INTC_Enable(NUM_I2C1);

	GPIO_SetFunctionEach(eGPIO_B,eGPIO_2,6);
	GPIO_SetFunctionEach(eGPIO_B,eGPIO_3,6);	
	GPIO_SetPullUpDownEach(eGPIO_B,eGPIO_2,6);
	GPIO_SetPullUpDownEach(eGPIO_B,eGPIO_3,6);

	//Pause();		// For debugging
	
	if ((((g_PCLK>>4)/ufreq)-1)>0xf) 
	{
		uSelClkSrc	=	1;
		uClkValue	=	((g_PCLK>>9)/ufreq) -1;		//	PCLK/512/freq
		//uClkValue	=	((g_PCLK/512)/ufreq) -1;		//	PCLK/512/freq		
		UART_Printf("*** PCLK= %.2fMHz,  SelClkSrc= %d (IICCLK=PCLK/512)   ,ClkDivideValue= %d\n",(float)g_PCLK/1.0e6,uSelClkSrc,uClkValue);		
		UART_Printf("*** IICCLK= %d Hz,  IICSCL (Tx clock)=  %d Hz\n",(g_PCLK/512),((g_PCLK/512)/(uClkValue+1)));			
	} 
	else 
	{
		uSelClkSrc	=	0;
		uClkValue	=	((g_PCLK>>4)/ufreq) -1;		//	PCLK/16/freq
		//uClkValue	=	((g_PCLK/16)/ufreq) -1;		//	PCLK/16/freq		
		UART_Printf("*** PCLK= %.2fMHz,  SelClkSrc= %d (IICCLK=PCLK/16)   ,ClkDivideValue= %d\n",(float)g_PCLK/1.0e6,uSelClkSrc,uClkValue);		
		UART_Printf("*** IICCLK= %d Hz,  IICSCL (Tx clock)=  %d Hz\n",(g_PCLK/16),((g_PCLK/16)/(uClkValue+1)));		
	}

	Outp32(rIICCON1,(uSelClkSrc<<6) | (1<<5) | (uClkValue&0xf));
	Outp32(rIICADD1,0xc0);		//	Slave address = [7:1]
	Outp32(rIICSTAT1,0x10);		//	IIC bus data output enable(Rx/Tx)
	Outp32(rIICLC1,0x4);			//	SDA Filter Enable,delayed 0clks
	//Outp32(rIICLC1,0x5);		//	SDA Filter enable,delayed 5clks
	//Outp32(rIICLC1,0x6);		//	SDA Filter enable,delayed 10clks
	//Outp32(rIICLC1,0x7);		//	SDA Filter enable,delayed 15clks		
}


//////////
// Function Name : IIC_OpenPolling
// Function Description : This function Set up IICCON with user's input frequency which determines uClkValue
//                                  (without ISR set up,for polling mode)
// Input : ufreq	ufreq(Hz) = PCLK/16/uClkValue
// Output : NONE
// Version : v0.1
void IIC_OpenPolling( u32 ufreq)		//	Hz order. freq(Hz) = PCLK/16/clk_divider
{
	u32	uSelClkSrc;
	u32	uClkValue;

	UART_Printf("\n*** IIC0 Setting Frequency: %d Hz\n",ufreq);

	GPIO_SetFunctionEach(eGPIO_B,eGPIO_5,2);
	GPIO_SetFunctionEach(eGPIO_B,eGPIO_6,2);	
	GPIO_SetPullUpDownEach(eGPIO_B,eGPIO_5,2);
	GPIO_SetPullUpDownEach(eGPIO_B,eGPIO_6,2);

	if ((((g_PCLK>>4)/ufreq)-1)>0xf) 
	{
		uSelClkSrc	=	1;
		uClkValue	=	((g_PCLK>>9)/ufreq) -1;		//	PCLK/512/freq
		UART_Printf("*** PCLK= %.2fMHz,  SelClkSrc= %d (IICCLK=PCLK/512)   ,ClkDivideValue= %d\n",(float)g_PCLK/1.0e6,uSelClkSrc,uClkValue);				
	} 
	else 
	{
		uSelClkSrc	=	0;
		uClkValue	=	((g_PCLK>>4)/ufreq) -1;		//	PCLK/16/freq
		UART_Printf("*** PCLK= %.2fMHz,  SelClkSrc= %d (IICCLK=PCLK/16)   ,ClkDivideValue= %d\n",(float)g_PCLK/1.0e6,uSelClkSrc,uClkValue);
	}

	UART_Printf("*** IICCLK= %d Hz,  IICSCL (Tx clock)=  %d Hz\n",(g_PCLK/16),((g_PCLK/16)/(uClkValue+1)));
		//Prescaler IICCLK=PCLK/16, Enable interrupt, Transmit clock value Tx clock=IICCLK/16
	Outp32(rIICCON0,(uSelClkSrc<<6) | (1<<5) | (uClkValue&0xf));
	Outp32(rIICADD0,0xc0);		//	Slave address = [7:1]
	Outp32(rIICSTAT0,0x10);		//	IIC bus data output enable(Rx/Tx)
	Outp32(rIICLC0,0x7);			//	SDA Filter Enable, delay 15 clocks
}

//////////
// Function Name : IIC1_OpenPolling
// Function Description : This function Set up IICCON with user's input frequency which determines uClkValue
//                                  (without ISR set up,for polling mode)
// Input : ufreq	ufreq(Hz) = PCLK/16/uClkValue
// Output : NONE
// Version : v0.1
void IIC1_OpenPolling( u32 ufreq)		//	Hz order. freq(Hz) = PCLK/16/clk_divider
{
	u32	uSelClkSrc;
	u32	uClkValue;

	UART_Printf("\n*** IIC1 Setting Frequency: %d Hz\n",ufreq);

	GPIO_SetFunctionEach(eGPIO_B,eGPIO_2,6);
	GPIO_SetFunctionEach(eGPIO_B,eGPIO_3,6);	
	GPIO_SetPullUpDownEach(eGPIO_B,eGPIO_2,6);
	GPIO_SetPullUpDownEach(eGPIO_B,eGPIO_3,6);

	if ((((g_PCLK>>4)/ufreq)-1)>0xf) 
	{
		uSelClkSrc	=	1;
		uClkValue	=	((g_PCLK>>9)/ufreq) -1;		//	PCLK/512/freq
		UART_Printf("*** PCLK= %.2fMHz,  SelClkSrc= %d (IICCLK=PCLK/512)   ,ClkDivideValue= %d\n",(float)g_PCLK/1.0e6,uSelClkSrc,uClkValue);		
	} 
	else 
	{
		uSelClkSrc	=	0;
		uClkValue	=	((g_PCLK>>4)/ufreq) -1;		//	PCLK/16/freq
		UART_Printf("*** PCLK= %.2fMHz,  SelClkSrc= %d (IICCLK=PCLK/16)   ,ClkDivideValue= %d\n",(float)g_PCLK/1.0e6,uSelClkSrc,uClkValue);		
	}
	
	UART_Printf("*** IICCLK= %d Hz,  IICSCL (Tx clock)=  %d Hz\n",(g_PCLK/16),((g_PCLK/16)/(uClkValue+1)));
		//Prescaler IICCLK=PCLK/16, Enable interrupt, Transmit clock value Tx clock=IICCLK/16
	Outp32(rIICCON1,(uSelClkSrc<<6) | (1<<5) | (uClkValue&0xf));
	Outp32(rIICADD1,0xc0);		//	Slave address = [7:1]
	Outp32(rIICSTAT1,0x10);		//	IIC bus data output enable(Rx/Tx)
	Outp32(rIICLC1,0x7);			//	SDA Filter disable
}


//////////
// Function Name : IIC_Close
// Function Description : This function disable IIC
// Input : NONE
// Output : NONE
// Version : v0.1
void IIC_Close(void)
{
	INTC_Disable(NUM_IIC);
	Outp32(rIICSTAT0,0x0);                		//	IIC bus data output disable(Rx/Tx)
	GPIO_SetFunctionEach(eGPIO_B,eGPIO_5,0);
	GPIO_SetFunctionEach(eGPIO_B,eGPIO_6,0);	
	GPIO_SetPullUpDownEach(eGPIO_B,eGPIO_5,0);
	GPIO_SetPullUpDownEach(eGPIO_B,eGPIO_6,0);
}

//////////
// Function Name : IIC1_Close
// Function Description : This function disable IIC
// Input : NONE
// Output : NONE
// Version : v0.1
void IIC1_Close(void)
{
	INTC_Disable(NUM_I2C1);
	Outp32(rIICSTAT1,0x0);                		//	IIC bus data output disable(Rx/Tx)

	GPIO_SetFunctionEach(eGPIO_B,eGPIO_2,0);
	GPIO_SetFunctionEach(eGPIO_B,eGPIO_3,0);	
	GPIO_SetPullUpDownEach(eGPIO_B,eGPIO_2,0);
	GPIO_SetPullUpDownEach(eGPIO_B,eGPIO_3,0);	
}



//////////
// Function Name : IIC_SetWrite
// Function Description : This function sets up write mode with 7-bit addresses
// Input :  cSlaveAddr [8bit SlaveDeviceAddress], 
//		  pData[Data which you want to write], 
//		  uDataLen [Data Length]
// Output : NONE
// Version : v0.1
void IIC_SetWrite( u8 cSlaveAddr,  u8 *pData, u32 uDataLen)
{
	u32 uTmp1;

	uTmp1 = Inp32(rIICSTAT0);

	while((uTmp1&(1<<5)) || (uTmp1&(1<<3)))	//	Wait until IIC bus is free or Arbitration successful.
	{
		uTmp1 = Inp32(rIICSTAT0);
	}

	g_PcIIC_BUFFER	=	pData;
	g_uIIC_PT		=	0;
	g_uIIC_DATALEN	=	uDataLen;

	uTmp1 = Inp32(rIICCON0);
	uTmp1 |= (1<<7);						// ACK Enable
	Outp32(rIICCON0,uTmp1);				// Ack generation Enable
	Outp32(rIICDS0,cSlaveAddr);
	Outp32(rIICSTAT0,0xf0);				// Master Tx Start.

	IIC_Wait();		// Added by SOP on 2008/04/10
}


//////////
// Function Name : IIC1_SetWrite
// Function Description : This function sets up write mode with 7-bit addresses
// Input :  cSlaveAddr [8bit SlaveDeviceAddress], 
//		  pData[Data which you want to write], 
//		  uDataLen [Data Length]
// Output : NONE
// Version : v0.1
void IIC1_SetWrite( u8 cSlaveAddr,  u8 *pData, u32 uDataLen)
{
	u32 uTmp1;

	uTmp1 = Inp32(rIICSTAT1);

	while((uTmp1&(1<<5)) || (uTmp1&(1<<3)))	//	Wait until IIC bus is free or Arbitration successful.
	{
		uTmp1 = Inp32(rIICSTAT1);
	}

	g_PcIIC_BUFFER	=	pData;
	g_uIIC_PT		=	0;
	g_uIIC_DATALEN	=	uDataLen;

	uTmp1 = Inp32(rIICCON1);
	uTmp1 |= (1<<7);						// ACK Enable
	Outp32(rIICCON1,uTmp1);				// Ack generation Enable
	Outp32(rIICDS1,cSlaveAddr);
	Outp32(rIICSTAT1,0xf0);				// Master Tx Start.

	IIC_Wait();		// Added by SOP on 2008/04/10
}


//////////
// Function Name : IIC_SetRead
// Function Description : This function sets up Read mode with 7-bit addresses
// Input : cSlaveAddr [8bit SlaveDeviceAddress], 
//		  pData[Data which you want to read], 
//		  uDataLen [Data Length]
// Output : NONE
// Version : v0.1
void IIC_SetRead(  u8 cSlaveAddr,  u8 *pData, u32 uDataLen)
{
	u32 uTmp0;
	u32 uTmp3;
	
	uTmp0= Inp32(rIICSTAT0);
	
	while((uTmp0&(1<<5)) || (uTmp0&(1<<3)))	//	Wait until IIC bus is free.
	{
		uTmp0 = Inp32(rIICSTAT0);	
	}	

	g_PcIIC_BUFFER	=	pData;
	g_uIIC_PT		=	0;
	g_uIIC_DATALEN	=	uDataLen;

	uTmp3 = Inp32(rIICCON0);
	uTmp3 |= (1<<7);
	Outp32(rIICCON0,uTmp3);				//	Ack generation Enable
	Outp32(rIICDS0,cSlaveAddr);

	Outp32(rIICSTAT0,0xB0);				//	Master Rx Start.

	IIC_Wait();							// Added by SOP on 2008/04/10
}


//////////
// Function Name : IIC1_SetRead
// Function Description : This function sets up Read mode with 7-bit addresses
// Input : cSlaveAddr [8bit SlaveDeviceAddress], 
//		  pData[Data which you want to read], 
//		  uDataLen [Data Length]
// Output : NONE
// Version : v0.1
void IIC1_SetRead(  u8 cSlaveAddr,  u8 *pData, u32 uDataLen)
{
	u32 uTmp0;
	u32 uTmp3;

	uTmp0= Inp32(rIICSTAT1);
	
	while((uTmp0&(1<<5)) || (uTmp0&(1<<3)))	//	Wait until IIC bus is free.
	{
		uTmp0 = Inp32(rIICSTAT1);	
	}	

	g_PcIIC_BUFFER	=	pData;
	g_uIIC_PT		=	0;
	g_uIIC_DATALEN	=	uDataLen;

	uTmp3 = Inp32(rIICCON1);
	uTmp3 |= (1<<7);
	Outp32(rIICCON1,uTmp3);				//	Ack generation Enable
	Outp32(rIICDS1,cSlaveAddr);

	Outp32(rIICSTAT1,0xB0);				//	Master Rx Start.

	IIC_Wait();							// Added by SOP on 2008/04/10
}


//////////
// Function Name : IIC_Wait
// Function Description : This function waits until the command takes effect
//											But not for IIC bus free
// Input : NONE 
// Output : NONE
// Version : v0.1
void IIC_Wait( void)						//	Waiting for the command takes effect.
{										//	But not for IIC bus free.
	while(g_uIIC_PT<=g_uIIC_DATALEN);
}

//////////
// Function Name : IIC_Status
// Function Description : This function returns IIC Status Register value at last interrupt occur
// Input : NONE
// Output : NONE
// Version : v0.1
u8 IIC_Status( void)						//	Return IIC Status Register value at last interrupt occur.
{
	return	g_cIIC_STAT0;
}

//////////
// Function Name : IIC_Write
// Function Description : This function STARTs up write mode with 7-bit addresses
// Input : cSlaveAddr [8bit SlaveDeviceAddress], 
//		 cAddr[8bit Address where you want to write], 
//		 pData[Data which you want to write]
// Output : NONE
// Version : v0.1
void IIC_Write(u8 cSlaveAddr, u8 cAddr, u8 cData)
{
	u8 cD[2];
	u32 uTmp0;

	cD[0]=cAddr;
	cD[1]=cData;

	do					//	Polling for an ACK signal from Serial EEPROM.
	{					
		Delay(30);		// Need to time for EEPROM
		IIC_SetWrite(cSlaveAddr, NULL, 0);
	} while(IIC_Status()&0x1);

	IIC_SetWrite(cSlaveAddr, cD, 2);	
}


//////////
// Function Name : IIC_Write16
// Function Description : This function STARTs up write mode with 7-bit addresses
// Input : cSlaveAddr [8bit SlaveDeviceAddress], 
//		 cAddr[8bit Address where you want to write], 
//		 pData[16bit Data which you want to write]
// Output : NONE
// Version : v0.1
void IIC_Write16(u8 cSlaveAddr, u8 cAddr, u16 cData)
{
	u8 cD[3];

	cD[0]=cAddr;
	cD[1]=(cData & 0xff00) >> 8;			//MSB First Data
	cD[2]=cData & 0xff;					//LSB Last Data
	
	do
	{									//	Polling for an ACK signal from SerialEEPROM.
		IIC_SetWrite(cSlaveAddr, NULL, 0);
		IIC_Wait();
	}while(IIC_Status()&0x1);
	
	IIC_SetWrite(cSlaveAddr, cD, 3);
}

//////////
// Function Name : IIC1_Write
// Function Description : This function STARTs up write mode with 7-bit addresses
// Input : cSlaveAddr [8bit SlaveDeviceAddress], 
//		 cAddr[8bit Address where you want to write], 
//		 pData[Data which you want to write]
// Output : NONE
// Version : v0.1
void IIC1_Write(u8 cSlaveAddr, u16 cAddr, u8 cData)
{
	u8 cD[3];
	u32 uTmp0;

	cD[0]=(cAddr & 0xff00) >> 8;			//MSB First Address
	cD[1]=cAddr & 0xff;					//LSB Last Address
	cD[2]=cData;

	do					//	Polling for an ACK signal from Serial EEPROM.
	{					
		Delay(30);		// Need to time for EEPROM
		IIC1_SetWrite(cSlaveAddr, NULL, 0);
	} while(IIC_Status()&0x1);

	IIC1_SetWrite(cSlaveAddr, cD, 3);	
}


//////////
// Function Name : IIC_Read
// Function Description : This function STARTs up read mode with 7-bit addresses
// Input : cSlaveAddr [8bit SlaveDeviceAddress], 
//		 cAddr [8bit Address where you want to read], 
//		 cData [pointer of Data which you want to read]
// Output : NONE
// Version : v0.1
void IIC_Read(u8 cSlaveAddr,u8 cAddr,u8 *cData)
{
	do		//	Polling for an ACK signal from SerialEEPROM.
	{					
		IIC_SetWrite(cSlaveAddr, NULL, 0);
	} while(IIC_Status()&0x1);

	IIC_SetWrite( cSlaveAddr, &cAddr, 1);			// following EEPROM random address access procedure	
	IIC_SetRead( cSlaveAddr, cData, 1);
}


//////////
// Function Name : IIC_Read
// Function Description : This function STARTs up read mode with 7-bit addresses
// Input : cSlaveAddr [8bit SlaveDeviceAddress], 
//		 cAddr [8bit Address where you want to read], 
//		 cData [pointer of Data which you want to read]
// Output : NONE
// Version : v0.1
void IIC1_Read(u8 cSlaveAddr, u16 cAddr, u8 *cData)
{
	u8 cD[2];
	u32 uTmp0;

	cD[0]=(cAddr & 0xff00) >> 8;			//MSB First Address
	cD[1]=cAddr & 0xff;					//LSB Last Address

	do		//	Polling for an ACK signal from SerialEEPROM.
	{		
		IIC1_SetWrite(cSlaveAddr, NULL, 0);
	} while(IIC_Status()&0x1);

	IIC1_SetWrite( cSlaveAddr, cD, 2);			// following EEPROM random address access procedure	
	IIC1_SetRead( cSlaveAddr, cData, 1);	
}


//////////
// Function Name : IIC_MasterWrP
// Function Description : This function do MasterTx mode by polling operation
// Input : cSlaveAddr [8bit SlaveDeviceAddress], 
//		 pData [pointer of Data which you want to Tx]
// Output : NONE
// Version : v0.1
void IIC_MasterWrP(u8 cSlaveAddr,u8 * pData)
{
	u32 uTmp0;
	u32 uTmp1;
	u8 cCnt;
	s32 sDcnt = 100;
	u32 uPT = 0;
	
	uTmp0 = Inp32(rIICSTAT0);
	while(uTmp0&(1<<5))					//	Wait until IIC bus is free.
		uTmp0 = Inp32(rIICSTAT0);			

	uTmp1 = Inp32(rIICCON0);
	uTmp1 |= (1<<7);
	Outp32(rIICCON0,uTmp1);				//	Ack generation Enable
	Outp32(rIICDS0,cSlaveAddr);
	Outp32(rIICSTAT0,0xf0);				//	Master Tx Start.

	while(!(sDcnt == -1))
	{
		if(Inp8(rIICCON0)&0x10)			//until Int Pending
		{
			if((sDcnt--) == 0)
			{
				Outp32(rIICSTAT0,0xd0);		//	Stop Master Tx condition, ACK flag clear
				
				uTmp0 = Inp32(rIICCON0);
				uTmp0 &= ~(1<<4);			//	Clear pending bit to resume
				Outp32(rIICCON0,uTmp0);

				Delay(1);					//	wait until Stop Condition is in effect
				break;
			}

			Outp8(rIICDS0,pData[uPT++]);
			for(cCnt=0;cCnt<10;cCnt++);		//	for setup time (rising edge of IICSCL)
			
			uTmp0 = Inp32(rIICCON0);
			uTmp0 &= ~(1<<4);				//	Clear pending bit to resume
			Outp32(rIICCON0,uTmp0);				
		}
	}
}


//////////
// Function Name : IIC1_MasterWrP
// Function Description : This function do MasterTx mode by polling operation
// Input : cSlaveAddr [8bit SlaveDeviceAddress], 
//		 pData [pointer of Data which you want to Tx]
// Output : NONE
// Version : v0.1
void IIC1_MasterWrP(u8 cSlaveAddr,u8 * pData)
{
	u32 uTmp0;
	u32 uTmp1;
	u8 cCnt;
	s32 sDcnt = 100;
	u32 uPT = 0;
	
	uTmp0 = Inp32(rIICSTAT1);
	while(uTmp0&(1<<5))					//	Wait until IIC bus is free.
		uTmp0 = Inp32(rIICSTAT1);			

	uTmp1 = Inp32(rIICCON1);
	uTmp1 |= (1<<7);
	Outp32(rIICCON1,uTmp1);				//	Ack generation Enable
	Outp32(rIICDS1,cSlaveAddr);
	Outp32(rIICSTAT1,0xf0);				//	Master Tx Start.

	while(!(sDcnt == -1))
	{
		if(Inp8(rIICCON1)&0x10)			//until Int Pending
		{
			if((sDcnt--) == 0)
			{
				Outp32(rIICSTAT1,0xd0);		//	Stop Master Tx condition, ACK flag clear
				
				uTmp0 = Inp32(rIICCON1);
				uTmp0 &= ~(1<<4);			//	Clear pending bit to resume
				Outp32(rIICCON1,uTmp0);

				Delay(1);					//	wait until Stop Condition is in effect
				break;
			}

			Outp8(rIICDS1,pData[uPT++]);
			for(cCnt=0;cCnt<10;cCnt++);		//	for setup time (rising edge of IICSCL)
			
			uTmp0 = Inp32(rIICCON1);
			uTmp0 &= ~(1<<4);				//	Clear pending bit to resume
			Outp32(rIICCON1,uTmp0);				
		}
	}
}


//////////
// Function Name : IIC_MasterRdP
// Function Description : This function do MasterRd mode by polling operation
// Input : cSlaveAddr [8bit SlaveDeviceAddress], 
//		 pData [pointer of Data which you want to Rx]
// Output : NONE
// Version : v0.1
void IIC_MasterRdP(u8 cSlaveAddr,u8 * pData)
{
	u32 uTmp0;
	u32 uTmp1;
	u8 cCnt;
	
	uTmp0 = Inp32(rIICSTAT0);
	while(uTmp0&(1<<5))					//	Wait until IIC bus is free.
		uTmp0 = Inp32(rIICSTAT0);			

	uTmp1 = Inp32(rIICCON0);
	uTmp1 |= (1<<7);
	Outp32(rIICCON0,uTmp1);				//	Ack generation Enable

	Outp32(rIICDS0,cSlaveAddr);

	Outp32(rIICSTAT0,0xB0);				//	Master Rx Start.

	while((Inp8(rIICSTAT0)&0x1));

	cCnt=0;
	
	while(cCnt<101) 
	{
		if(Inp8(rIICCON0)&0x10) 
		{
			pData[cCnt]=Inp8(rIICDS0);
			cCnt++;
			
			uTmp0 = Inp32(rIICCON0);
			uTmp0 &= ~(1<<4);			//	Clear pending bit to resume
			Outp32(rIICCON0,uTmp0);				
		}
	}
	Outp8(rIICSTAT0,0x90);				// stop bit gen.
}


//////////
// Function Name : IIC1_MasterRdP
// Function Description : This function do MasterRd mode by polling operation
// Input : cSlaveAddr [8bit SlaveDeviceAddress], 
//		 pData [pointer of Data which you want to Rx]
// Output : NONE
// Version : v0.1
void IIC1_MasterRdP(u8 cSlaveAddr,u8 * pData)
{
	u32 uTmp0;
	u32 uTmp1;
	u8 cCnt;
	
	uTmp0 = Inp32(rIICSTAT1);
	while(uTmp0&(1<<5))					//	Wait until IIC bus is free.
		uTmp0 = Inp32(rIICSTAT1);			

	uTmp1 = Inp32(rIICCON1);
	uTmp1 |= (1<<7);
	Outp32(rIICCON1,uTmp1);				//	Ack generation Enable

	Outp32(rIICDS1,cSlaveAddr);

	Outp32(rIICSTAT1,0xB0);				//	Master Rx Start.

	while((Inp8(rIICSTAT1)&0x1));

	cCnt=0;
	
	while(cCnt<101) 
	{
		if(Inp8(rIICCON1)&0x10) 
		{
			pData[cCnt]=Inp8(rIICDS1);
			cCnt++;
			
			uTmp0 = Inp32(rIICCON1);
			uTmp0 &= ~(1<<4);			//	Clear pending bit to resume
			Outp32(rIICCON1,uTmp0);				
		}
	}
	Outp8(rIICSTAT1,0x90);				// stop bit gen.
}


//////////
// Function Name : IIC_SlaveRdP
// Function Description : This function do SlaveRx mode by polling operation
// Input : pSlaveAddr [pointer of 8bit SlaveDeviceAddress], 
//		 pData [pointer of Data which you want to Rx]
// Output : NONE
// Version : v0.1
void IIC_SlaveRdP(u8 *pSlaveAddr,u8 *pData)
{
	u32 uTmp0;
	u32 uTmp1;
	u8 cCnt;
	
	uTmp0 = Inp32(rIICSTAT0);
	while(uTmp0&(1<<5))					//	Wait until IIC bus is free.
		uTmp0 = Inp32(rIICSTAT0);			

	uTmp1 = Inp32(rIICCON0);
	uTmp1 |= (1<<7);
	Outp32(rIICCON0,uTmp1);				//	Ack generation Enable

	uTmp0 = Inp32(rIICSTAT0);
	uTmp0 &= ~(1<<4);
	Outp32(rIICSTAT0,uTmp0);				// Disable Rx/Tx for setting SlaveAddr
	Outp8(rIICADD0,*pSlaveAddr);
	Outp32(rIICSTAT0,0x10);				//	Slave Rx Start.
	UART_Printf("Wait for Slave Addr\n");
	
	cCnt=0;
	
	while(cCnt<101) 
	{
		if(Inp8(rIICCON0)&0x10) 
		{
			pData[cCnt]=Inp8(rIICDS0);
			cCnt++;
			
			uTmp0 = Inp32(rIICCON0);
			uTmp0 &= ~(1<<4);			//	Clear pending bit to resume
			Outp32(rIICCON0,uTmp0);		
		}
	}
	*pSlaveAddr = pData[0];
	Outp8(rIICSTAT0,0x0);
}


//////////
// Function Name : IIC1_SlaveRdP
// Function Description : This function do SlaveRx mode by polling operation
// Input : pSlaveAddr [pointer of 8bit SlaveDeviceAddress], 
//		 pData [pointer of Data which you want to Rx]
// Output : NONE
// Version : v0.1
void IIC1_SlaveRdP(u8 *pSlaveAddr,u8 *pData)
{
	u32 uTmp0;
	u32 uTmp1;
	u8 cCnt;
	
	uTmp0 = Inp32(rIICSTAT1);
	while(uTmp0&(1<<5))					//	Wait until IIC bus is free.
		uTmp0 = Inp32(rIICSTAT1);			

	uTmp1 = Inp32(rIICCON1);
	uTmp1 |= (1<<7);
	Outp32(rIICCON1,uTmp1);				//	Ack generation Enable

	uTmp0 = Inp32(rIICSTAT1);
	uTmp0 &= ~(1<<4);
	Outp32(rIICSTAT1,uTmp0);				// Disable Rx/Tx for setting SlaveAddr
	Outp8(rIICADD1,*pSlaveAddr);
	Outp32(rIICSTAT1,0x10);				//	Slave Rx Start.
	UART_Printf("Wait for Slave Addr\n");
	
	cCnt=0;

	while(cCnt<101) 
	{
		if(Inp8(rIICCON1)&0x10) 
		{
			pData[cCnt]=Inp8(rIICDS1);
			cCnt++;
			
			uTmp0 = Inp32(rIICCON1);
			uTmp0 &= ~(1<<4);			//	Clear pending bit to resume
			Outp32(rIICCON1,uTmp0);		
		}
	}
	*pSlaveAddr = pData[0];
	Outp8(rIICSTAT1,0x0);
}


//////////
// Function Name : IIC_SlaveWrP
// Function Description : This function do SlaveTx mode by polling operation
// Input : pSlaveAddr [pointer of 8bit SlaveDeviceAddress], 
//		 pData [pointer of Data which you want to Tx]
// Output : NONE
// Version : v0.1
void IIC_SlaveWrP(u8 *pSlaveAddr,u8 *pData)
{
	u32 uTmp0;
	u32 uTmp1;
	u8 cCnt;
	s32 sDcnt = 100;
	u32 uPT = 0;

	uTmp0 = Inp32(rIICSTAT0);
	while(uTmp0&(1<<5))					//	Wait until IIC bus is free.
		uTmp0 = Inp32(rIICSTAT0);			

	uTmp1 = Inp32(rIICCON0);
	uTmp1 |= (1<<7);
	Outp32(rIICCON0,uTmp1);				//	Ack generation Enable

	uTmp0 = Inp32(rIICSTAT0);
	uTmp0 &=~(1<<4);
	Outp32(rIICSTAT0,uTmp0);				// Disable Rx/Tx for setting SlaveAddr

	Outp8(rIICADD0,*pSlaveAddr);

	Outp32(rIICSTAT0,0x50);				//	Slave Tx Start.

	while(!(sDcnt == -1))
	{
		if(Inp8(rIICCON0)&0x10)			//until Int Pending
		{
			if((sDcnt--) == 0)
			{
				Outp32(rIICSTAT0,0xd0);		//	Stop Master Tx condition, ACK flag clear
				
				uTmp0 = Inp32(rIICCON0);
				uTmp0 &= ~(1<<4);			//	Clear pending bit to resume
				Outp32(rIICCON0,uTmp0);

				Delay(1);					//	wait until Stop Condition is in effect
				break;
			}

			Outp8(rIICDS0,pData[uPT++]);
			for(cCnt=0;cCnt<10;cCnt++);		//	for setup time (rising edge of IICSCL)
			
			uTmp0 = Inp32(rIICCON0);
			uTmp0 &= ~(1<<4);				//	Clear pending bit to resume
			Outp32(rIICCON0,uTmp0);				
		}
	}
}

//////////
// Function Name : IIC1_SlaveWrP
// Function Description : This function do SlaveTx mode by polling operation
// Input : pSlaveAddr [pointer of 8bit SlaveDeviceAddress], 
//		 pData [pointer of Data which you want to Tx]
// Output : NONE
// Version : v0.1
void IIC1_SlaveWrP(u8 *pSlaveAddr,u8 *pData)
{
	u32 uTmp0;
	u32 uTmp1;
	u8 cCnt;
	s32 sDcnt = 100;
	u32 uPT = 0;

	uTmp0 = Inp32(rIICSTAT1);
	while(uTmp0&(1<<5))					//	Wait until IIC bus is free.
		uTmp0 = Inp32(rIICSTAT1);			

	uTmp1 = Inp32(rIICCON1);
	uTmp1 |= (1<<7);
	Outp32(rIICCON1,uTmp1);				//	Ack generation Enable

	uTmp0 = Inp32(rIICSTAT1);
	uTmp0 &=~(1<<4);
	Outp32(rIICSTAT1,uTmp0);				// Disable Rx/Tx for setting SlaveAddr

	Outp8(rIICADD1,*pSlaveAddr);

	Outp32(rIICSTAT1,0x50);				//	Slave Tx Start.


	while(!(sDcnt == -1))
	{
		if(Inp8(rIICCON1)&0x10)			//until Int Pending
		{
			if((sDcnt--) == 0)
			{
				Outp32(rIICSTAT1,0xd0);		//	Stop Master Tx condition, ACK flag clear
				
				uTmp0 = Inp32(rIICCON1);
				uTmp0 &= ~(1<<4);			//	Clear pending bit to resume
				Outp32(rIICCON1,uTmp0);

				Delay(1);					//	wait until Stop Condition is in effect
				break;
			}

			Outp8(rIICDS1,pData[uPT++]);
			for(cCnt=0;cCnt<10;cCnt++);		//	for setup time (rising edge of IICSCL)
			
			uTmp0 = Inp32(rIICCON1);
			uTmp0 &= ~(1<<4);				//	Clear pending bit to resume
			Outp32(rIICCON1,uTmp0);				
		}
	}
}


//////////
// Function Name : IIC_SlaveRdInt
// Function Description : This function do SlaveRx mode by Interrupt operation
// Input : pSlaveAddr [pointer of 8bit SlaveDeviceAddress], 
//		 pData [pointer of Data which you want to Rx]
// Output : NONE
// Version : v0.1
void IIC_SlaveRdInt(u8 *pSlaveAddr,u8 *pData)
{
	u32 uTmp0;
	u32 uTmp1;
	
	g_PcIIC_BUFFER	=	pData;
	g_uIIC_PT		=	0;
	g_cIIC_SlaveRxDone = 0;

	uTmp0 = Inp32(rIICSTAT0);
	while(uTmp0&(1<<5))					//	Wait until IIC bus is free.
		uTmp0 = Inp32(rIICSTAT0);			

	uTmp1 = Inp32(rIICCON0);
	uTmp1 |= (1<<7);
	Outp32(rIICCON0,uTmp1);				//	Ack generation Enable

	uTmp0 = Inp32(rIICSTAT0);
	uTmp0 &= ~(1<<4);
	Outp32(rIICSTAT0,uTmp0);				// Disable Rx/Tx for setting SlaveAddr
	Outp8(rIICADD0,*pSlaveAddr);
	Outp32(rIICSTAT0,0x10);				//	Slave Rx Start.
	while(!(g_cIIC_SlaveRxDone));
}


//////////
// Function Name : IIC1_SlaveRdInt
// Function Description : This function do SlaveRx mode by Interrupt operation
// Input : pSlaveAddr [pointer of 8bit SlaveDeviceAddress], 
//		 pData [pointer of Data which you want to Rx]
// Output : NONE
// Version : v0.1
void IIC1_SlaveRdInt(u8 *pSlaveAddr,u8 *pData)
{
	u32 uTmp0;
	u32 uTmp1;
	
	g_PcIIC_BUFFER	=	pData;
	g_uIIC_PT		=	0;
	g_cIIC_SlaveRxDone = 0;

	uTmp0 = Inp32(rIICSTAT1);
	while(uTmp0&(1<<5))					//	Wait until IIC bus is free.
		uTmp0 = Inp32(rIICSTAT1);			

	uTmp1 = Inp32(rIICCON1);
	uTmp1 |= (1<<7);
	Outp32(rIICCON1,uTmp1);				//	Ack generation Enable

	uTmp0 = Inp32(rIICSTAT1);
	uTmp0 &= ~(1<<4);
	Outp32(rIICSTAT1,uTmp0);				// Disable Rx/Tx for setting SlaveAddr
	Outp8(rIICADD1,*pSlaveAddr);
	Outp32(rIICSTAT1,0x10);				//	Slave Rx Start.
	while(!(g_cIIC_SlaveRxDone));
}


//////////
// Function Name : IIC_SlaveWrInt
// Function Description : This function do SlaveTx mode by Interrupt operation
// Input : pSlaveAddr [pointer of 8bit SlaveDeviceAddress], 
//		 pData [pointer of Data which you want to Tx]
// Output : NONE
// Version : v0.1
void IIC_SlaveWrInt(u8 *pSlaveAddr,u8 *pData)
{
	u32 uTmp0;
	u32 uTmp1;
	
	g_PcIIC_BUFFER	=	pData;
	g_uIIC_PT		=	0;
	g_cIIC_SlaveTxDone = 0;

	uTmp0 = Inp32(rIICSTAT0);
	while(uTmp0&(1<<5))					//	Wait until IIC bus is free.
		uTmp0 = Inp32(rIICSTAT0);			

	uTmp1 = Inp32(rIICCON0);
	uTmp1 |= (1<<7);
	Outp32(rIICCON0,uTmp1);				//	Ack generation Enable

	uTmp0 = Inp32(rIICSTAT0);
	uTmp0 &=~(1<<4);
	Outp32(rIICSTAT0,uTmp0);				// Disable Rx/Tx for setting SlaveAddr
	Outp8(rIICADD0,*pSlaveAddr);
	Outp32(rIICSTAT0,0x50);				//	Slave Tx Start.
	while(!(g_cIIC_SlaveTxDone));
}


//////////
// Function Name : IIC1_SlaveWrInt
// Function Description : This function do SlaveTx mode by Interrupt operation
// Input : pSlaveAddr [pointer of 8bit SlaveDeviceAddress], 
//		 pData [pointer of Data which you want to Tx]
// Output : NONE
// Version : v0.1
void IIC1_SlaveWrInt(u8 *pSlaveAddr,u8 *pData)
{
	u32 uTmp0;
	u32 uTmp1;
	
	g_PcIIC_BUFFER	=	pData;
	g_uIIC_PT		=	0;
	g_cIIC_SlaveTxDone = 0;

	uTmp0 = Inp32(rIICSTAT1);
	while(uTmp0&(1<<5))					//	Wait until IIC bus is free.
		uTmp0 = Inp32(rIICSTAT1);			

	uTmp1 = Inp32(rIICCON1);
	uTmp1 |= (1<<7);
	Outp32(rIICCON1,uTmp1);				//	Ack generation Enable

	uTmp0 = Inp32(rIICSTAT1);
	uTmp0 &=~(1<<4);
	Outp32(rIICSTAT1,uTmp0);				// Disable Rx/Tx for setting SlaveAddr
	Outp8(rIICADD1,*pSlaveAddr);
	Outp32(rIICSTAT1,0x50);				//	Slave Tx Start.
	while(!(g_cIIC_SlaveTxDone));
}


void ClockInform(void)			// Added by SOP on 2008/03/27
{
	SYSC_GetClkInform();
	UART_Printf("------------------------------------------------------------------------------\n");	
	UART_Printf("ARMCLK: %.2fMHz  HCLKx2: %.2fMHz  HCLK: %.2fMHz  PCLK: %.2fMHz\n",(float)g_ARMCLK/1.0e6, (float)g_HCLKx2/1.0e6, (float)g_HCLK/1.0e6, (float)g_PCLK/1.0e6);	
	UART_Printf("------------------------------------------------------------------------------\n");
}

