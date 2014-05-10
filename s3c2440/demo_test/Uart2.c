//====================================================================
// File Name : uart2.c
// Function  : S3C2440 UART-2 test code
// Program   : Shin, On Pil (SOP)
// Date      : May 30, 2002
// Version   : 0.0
// History
// -2410
//   0.0 : Programming start (February 20,2002) -> SOP
//   1.0 (20020530) : First release for customer
//   1.1 (20020801) : Strata NOR Flash Added and etc. -> SOP
// -2440
//   R0.0 (20030421): Modified for 2440. -> Junon
//====================================================================

#include <string.h>
#include <stdlib.h>
#include "2440addr.h"
#include "2440lib.h"
#include "def.h"
#include "uart0.h"
#include "uart2.h"

#define __1 (unsigned int)1

void __irq Uart2_TxInt(void);
void __irq Uart2_RxIntOrErr(void);
void __irq Uart2_TxDmaDone(void);
void __irq Uart2_RxDmaDone(void);
void __irq Uart2_RxDmaOrErr(void);
void __irq Uart2_TxFifo(void);
void __irq Uart2_RxFifoOrErr(void);

void __sub_Uart2_RxInt(void);
void __sub_Uart2_RxFifo(void);
void __sub_Uart2_RxErrInt(void);

volatile static char *uart2TxStr;
volatile static char *uart2RxStr;

//---------------------------------------UART2 test function-------------------------------------
void __irq Uart2_TxInt(void)
{
    rINTSUBMSK|=(BIT_SUB_RXD2|BIT_SUB_TXD2|BIT_SUB_ERR2);	// Just for the safety
    if(*uart2TxStr != '\0')
    {
    	WrUTXH2(*uart2TxStr++);
        ClearPending(BIT_UART2);	// Clear master pending
        rSUBSRCPND=(BIT_SUB_TXD2);
        rINTSUBMSK&=~(BIT_SUB_TXD2);	// Unmask sub int
    }
    else
    {
    	isTxInt=0;
        ClearPending(BIT_UART2);	// Clear master pending
        rSUBSRCPND=(BIT_SUB_TXD2);
        rINTMSK|=(BIT_UART2);		// Unmask sub int
    }
}

void __irq Uart2_RxIntOrErr(void)
{
    rINTSUBMSK|=(BIT_SUB_RXD2|BIT_SUB_TXD2|BIT_SUB_ERR2);
    if(rSUBSRCPND&BIT_SUB_RXD2) __sub_Uart2_RxInt();
    else __sub_Uart2_RxErrInt();

    ClearPending(BIT_UART2);
    rSUBSRCPND=(BIT_SUB_RXD2|BIT_SUB_ERR2);	// Clear Sub int pending    
    rINTSUBMSK&=~(BIT_SUB_RXD2|BIT_SUB_ERR2);    
}

void __irq Uart2_TxDmaDone(void)
{
    rDMASKTRIG3=0x0;	// Stop Dma3
    isDone=0;
    rINTMSK |= BIT_DMA3;
    ClearPending(BIT_DMA3);
}

void __irq Uart2_RxDmaDone(void)
{
    rDMASKTRIG3=0x0;	//DMA3 Channel Off
    isDone=0;
    *(uart2RxStr+5)='\0';
    rINTMSK|=(BIT_DMA3);
    ClearPending(BIT_DMA3);
}

void __irq Uart2_RxDmaOrErr(void)
{
    rINTSUBMSK|=(BIT_SUB_RXD2|BIT_SUB_TXD2|BIT_SUB_ERR2);
    if(rSUBSRCPND&BIT_SUB_RXD2) Uart_Printf("Error : UART2 Rx Interrupt is occured!!!\n");
    else __sub_Uart2_RxErrInt();

    ClearPending(BIT_UART2);
    rSUBSRCPND=(BIT_SUB_ERR2);	// Clear Sub int pending    
    rINTSUBMSK&=~(BIT_SUB_ERR2);    
}

void __irq Uart2_TxFifo(void)
{
    rINTSUBMSK|=(BIT_SUB_RXD2|BIT_SUB_TXD2|BIT_SUB_ERR2);	// Just for the safety
    while (!(rUFSTAT2 & 0x4000) && (*uart2TxStr != '\0')) 	//until tx fifo full or end of string
    	WrUTXH2(*uart2TxStr++);	

    if(*uart2TxStr == '\0') 
    {
    	rINTMSK|=BIT_UART2;
        rSUBSRCPND=BIT_SUB_TXD2;	// Clear Sub int pending
        ClearPending(BIT_UART2);	// Clear master pending
    }
    else 
    {
        ClearPending(BIT_UART2);	// Clear master pending
        rSUBSRCPND=BIT_SUB_TXD2;	// Clear Sub int pending
        rINTSUBMSK&=~(BIT_SUB_TXD2);	// Unmask sub int
    }
}

void __irq Uart2_RxFifoOrErr(void)
{
    rINTSUBMSK|=(BIT_SUB_RXD2|BIT_SUB_TXD2|BIT_SUB_ERR2);
    if(rSUBSRCPND&BIT_SUB_RXD2) __sub_Uart2_RxFifo();
    else __sub_Uart2_RxErrInt();
    ClearPending(BIT_UART2);
    rSUBSRCPND=(BIT_SUB_RXD2|BIT_SUB_ERR2);
    rINTSUBMSK&=~(BIT_SUB_RXD2|BIT_SUB_ERR2);
}

void __sub_Uart2_RxInt(void)
{
    if(RdURXH2()!='\r') 
    {
    	Uart_Printf("%c",RdURXH2());
       *uart2RxStr++ =(char) RdURXH2();
    }
    else
    {
    	isRxInt=0;
    	*uart2RxStr='\0';
    	Uart_Printf("\n");
    }
}

void __sub_Uart2_RxFifo(void)
{
    while(rUFSTAT2&0x7f)	//During the Rx FIFO is not empty
    {
	rx_point++;
	if(rx_point<5)
		 rx_filesize |= (RdURXH2()<<(8*(rx_point-1))); // First 4-bytes mean file size
	else if(rx_point>(rx_filesize-2))	
	{
		rx_dncs |= (RdURXH2()<<(8*(1-(rx_filesize-rx_point)))); //Last 2-bytes mean checksum.
		if(rx_point==rx_filesize) rx_isdone=0;
	}
	else
		rx_checksum+=RdURXH2();
    }
}

  
void __sub_Uart2_RxErrInt(void)
{
    switch(rUERSTAT2)//to clear and check the status of register bits
    {
	case 1:
	    Uart_Printf("Overrun error\n");
	    break;
	case 2:
	    Uart_Printf("Parity error\n");
	    break;
	case 4:
	    Uart_Printf("Frame error\n");
	    break;
	case 8:
	    Uart_Printf("Breake detect\n");
	    break;
	default :
	    break;
    }
   	isRxInt=0;
}


void Test_Uart2_Int(void)
{
	U8 ch;
	
    Uart_Port_Set(); 
    // Additional configuration for UART2 port
    //rGPHCON&=0x3fafff;      // TXD2,RXD2
    Uart_Select(1);    

	/******** For operating clock test *********/
	Uart_Printf("[Uart channel 0 Interrupt test]\n");
	Uart_Printf("Select operating clock 1. PCLK(D)    2. UCLK     3. FCLK/n  \nChoose : ");
	ch = Uart_Getch();
	switch (ch)
	{
	case '2' :
		Uart_Uextclk_En(2, 115200, 14745600);
		break;
	case '3' :
		Uart_Fclkn_En(2, 115200);
		break;
	default :
		Uart_Pclk_En(2, 115200);
	}
	
	/******** Select UART or IrDA *********/
	Uart_Printf("Select 1. UART or 2. IrDA(D) mode\nChoose : ");
	if (Uart_Getch() == '1')
		rULCON2 &= ~(1<<6); // UART mode
	else
		rULCON2 |= (1<<6); // IrDA mode


    /*********** UART2 Tx test with interrupt ***********/  
    isTxInt=1;
    uart2TxStr="ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890->UART2 Tx interrupt test is good!!!!\r\n";
    Uart_Printf("[Uart channel 2 Tx Interrupt Test]\n");

    //-------------------------------------------------------------------------------------------->
    Uart_Printf("Jumper Setting  <======> J17:2-3, J19:2-3, J16: 1-2, J18:1-2");
    Uart_Printf("\nConnect PC[COM1 or COM2] and UART2 of SMDK2440 with a serial cable!!! \n");
    Uart_Printf("Then, press any key........\n");
    Uart_Select(2);	// Change the uart port    
    Uart_Getch();
    //----------------------------------------->

    pISR_UART2=(unsigned)Uart2_TxInt;

    rULCON2=(0<<6)|(0<<3)|(0<<2)|(3);	// Normal,No parity,One stop bit, 8bit
    rUCON2 |= (TX_INTTYPE<<9)|(RX_INTTYPE<<8)|(0<<7)|(0<<6)|(0<<5)|(0<<4)|(1<<2)|(1);
    //Clock,Tx:Def,Rx:Def,Rx timeout:x,Rx error int:x,Loop-back:x,Send break:x,Tx:int,Rx:int
    Uart_TxEmpty(1); //wait until tx buffer is empty.
    rINTMSK=~(BIT_UART2);
    rINTSUBMSK=~(BIT_SUB_TXD2);

    while(isTxInt);

    /*********** UART2 Rx test with interrupt ***********/
    isRxInt=1;
    uart2RxStr=(volatile char *)UARTBUFFER;
    Uart_Printf("\n[Uart channel 2 Rx Interrupt Test]:\n");
    Uart_Printf("After typing ENTER key, you will see the characters which was typed by you.");
    Uart_Printf("\nTo quit, press ENTER key.!!!\n");
    
    pISR_UART2 =(unsigned)Uart2_RxIntOrErr;

    rULCON2=(0<<6)|(0<<3)|(0<<2)|(3);	// Normal,No parity,One stop bit, 8bit
    rUCON2 |= (TX_INTTYPE<<9)|(RX_INTTYPE<<8)|(0<<7)|(1<<6)|(0<<5)|(0<<4)|(1<<2)|(1);
    //Clock,Tx:pulse,Rx:pulse,Rx timeout:x,Rx error int:o,Loop-back:x,Send break:x,Tx:int,Rx:int

    // Clear Int Pending and Unmask
    ClearPending(BIT_UART2);
    rINTMSK=~(BIT_UART2);
    rSUBSRCPND=(BIT_SUB_RXD2|BIT_SUB_ERR2);
    rINTSUBMSK=~(BIT_SUB_RXD2|BIT_SUB_ERR2);
    
    while(isRxInt);

    rINTSUBMSK|=(BIT_SUB_RXD2|BIT_SUB_TXD2|BIT_SUB_ERR2);
    rINTMSK|=(BIT_UART2);

    Uart_Printf("%s\n",(char *)UARTBUFFER);


    //----------------------------------------------------------------------------------------->    
    Uart_Printf("Jumper Setting  <======>  J16: 2-3, J18:2-3");
    Uart_Printf("\nConnect PC[COM1 or COM2] and UART1 of SMDK2440 with a serial cable!!! \n");
    Uart_Printf("Then, press any key........\n");
    Uart_Select(1);
    Uart_Getch();
    //--------------------------->


    Uart_Port_Return();
}

void Test_Uart2_Dma(void)
{
    Uart_Port_Set(); 
    // Additional configuration for UART2 port
    //rGPHCON&=0x3fafff;      // TXD2,RXD2
    
    Uart_Select(1);    
    /*********** UART2 Tx test with DMA3 ***********/ 
    isDone=1;
    uart2TxStr="ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890->UART2 Tx Test by DMA3 is good!!!!\r\n";
    Uart_Printf("\n[Uart channel 2 DMA3 Tx Test]\n");
    Uart_TxEmpty(1);
    
    //-------------------------------------------------------------------------------------------->
    Uart_Printf("Jumper Setting  <======>  J17:2-3, J19:2-3, J16: 1-2, J18:1-2");
    Uart_Printf("\nConnect PC[COM1 or COM2] and UART2 of SMDK2440 with a serial cable!!! \n");
    Uart_Printf("Then, press any key........\n");
    Uart_Select(2);	// Change the uart port    
    Uart_Getch();
    Uart_Printf("Port is changed.......\n");
    //----------------------------------------->
  
    
    pISR_DMA3  =(unsigned)Uart2_TxDmaDone;

    rULCON2=(0<<6)|(0<<3)|(0<<2)|(3);	// Normal,No parity,One stop bit, 8bit
    rUCON2 = (TX_INTTYPE<<9)|(RX_INTTYPE<<8)|(0<<7)|(0<<6)|(0<<5)|(0<<4)|(2<<2)|(1);
    //Clock,Tx:Def,Rx:Def,Rx timeout:x,Rx error int:x,Loop-back:x,Send break:x,Tx:dma3,Rx:int

    /***DMA3 init***/
    rDISRC3=(U32)uart2TxStr;	// Start address
    rDISRCC3=(0<<1)|(0);		// AHB,Increment
    rDIDST3=(U32)UTXH2;			// Memory buffer Address
    rDIDSTC3=(1<<1)|(1);		// APB,Fixed
    rDCON3=((unsigned int)1<<31)|(0<<30)|(1<<29)|(0<<28)|(0<<27)|(0<<24)|(1<<23)|(1<<22)|(0<<20)|strlen((char*)uart2TxStr);
    //handshake, sync PCLK, TC int, single tx, single service, Uart2, H/W request,auto-reload off, Byte size Tx, Tx count value
    rINTMSK=~(BIT_DMA3);
    rDMASKTRIG3=(0<<2)|(1<<1)|(0);    //no-stop, DMA3 channel on, no-SW trigger 

    while(isDone);

    /*********** UART2 Rx test with DMA3 ***********/ 
    isDone=1;
    uart2RxStr=(char *)UARTBUFFER;
    Uart_Printf("\n[Uart channel 2 DMA3 Rx Test]\n");
    Uart_Printf("Type any five keys!!!\n");    
    Uart_Printf("Then you will see what you typed.\n");

    pISR_DMA3=(unsigned)Uart2_RxDmaDone;
    pISR_UART2=(unsigned)Uart2_RxDmaOrErr;
    
    rULCON2=(0<<6)|(0<<3)|(0<<2)|(3);	// Normal,No parity,One stop bit, 8bit
    rUCON2 &= 0x400;	// For the PCLK <-> UCLK fuction  
    rUCON2 |= (TX_INTTYPE<<9)|(RX_INTTYPE<<8)|(0<<7)|(1<<6)|(0<<5)|(0<<4)|(1<<2)|(2);
    //Clock,Tx:Def,Rx:Def,Rx timeout:x,Rx error int:o,Loop-back:x,Send break:x,Tx:int,Rx:dma3

    /***DMA3 init***/
    rDISRC3=(U32)URXH2;			// Start address
    rDISRCC3=(1<<1)|(1);		// APB,Fixed
    rDIDST3=(U32)uart2RxStr;	// Memory buffer Address
    rDIDSTC3= (0<<1)|(0);		// AHB,Increment
    rDCON3=((unsigned int)1<<31)|(0<<30)|(1<<29)|(0<<28)|(0<<27)|(0<<24)|(1<<23)|(1<<22)|(0<<20)|(5);
    //handshake, sync PCLK, TC int, single tx, single service, Uart2, H/W request,auto-reload off, Byte size Tx, Tx count value

    // Clear Int Pending and Unmask
    ClearPending(BIT_UART2);
    rINTMSK=~(BIT_DMA3|BIT_UART2);
    rSUBSRCPND=(BIT_SUB_RXD2|BIT_SUB_TXD2|BIT_SUB_ERR2);
    rINTSUBMSK=~(BIT_SUB_ERR2);
    rDMASKTRIG3=(0<<2)|(1<<1)|(0);    //no-stop, DMA3 channel on, no-SW trigger 

    while(isDone);

    Uart_Printf("%s\n",uart2RxStr);
    

    //----------------------------------------------------------------------------------------->    
    Uart_Printf("Jumper Setting  <======>  J16: 2-3, J18:2-3");
    Uart_Printf("\nConnect PC[COM1 or COM2] and UART1 of SMDK2440 with a serial cable!!! \n");
    Uart_Printf("Then, press any key........\n");
    Uart_Select(1);
    Uart_Getch();
    //--------------------------->


    Uart_Port_Return();
}

void Test_Uart2_Fifo(void)
{
    Uart_Port_Set(); 
    // Additional configuration for UART2 port
    //rGPHCON&=0x3fafff;      // TXD2,RXD2

    Uart_Select(1);
    /******* UART2 Tx FIFO test with interrupt ********/     
    Uart_Printf("[Uart channel 2 Tx FIFO Interrupt Test]\n");
    Uart_TxEmpty(1);	//wait until tx buffer is empty.
    
    //-------------------------------------------------------------------------------------------->
    Uart_Printf(" Jumper Setting  <======> J17:2-3, J19:2-3, J16: 1-2, J18:1-2");
    Uart_Printf("\nConnect PC[COM1 or COM2] and UART2 of SMDK2440 with a serial cable!!! \n");
    Uart_Printf("Then, press any key........\n");
    Uart_Select(2);	// Change the uart port    
    Uart_Getch();
    //----------------------------------------->
    
    
    /* <Tx Trigger Level:8Byte> */    
    uart2TxStr="ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890->UART2 Tx FIFO interrupt(8byte) test is good!!!!\r\n";
    pISR_UART2=(U32)Uart2_TxFifo;
    rULCON2=(0<<6)|(0<<3)|(0<<2)|(3);	// Normal,No parity,One stop bit, 8bit
    rUCON2 |= (TX_INTTYPE<<9)|(RX_INTTYPE<<8)|(0<<7)|(0<<6)|(0<<5)|(0<<4)|(1<<2)|(1);
    //Clock,Tx:Def,Rx:Def,Rx timeout:x,Rx error int:x,Loop-back:x,Send break:x,Tx:int,Rx:int
    rUFCON2=(2<<6)|(1<<4)|(1<<2)|(1<<1)|(1);
    //Tx and Rx FIFO Trigger Level:8byte,Tx and Rx FIFO Reset,FIFO on
    rINTMSK=~(BIT_UART2);
    rINTSUBMSK=~(BIT_SUB_TXD2);
    Delay(500);
    rUFCON2=(2<<6)|(1<<4)|(1<<2)|(1<<1)|(0);
    //Tx and Rx FIFO Trigger Level:8byte,Tx and Rx FIFO Reset,FIFO off

    /* <Tx Trigger Level:12Byte> */    
    uart2TxStr="ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890->UART2 Tx FIFO interrupt(12byte) test is good!!!!\r\n";
    pISR_UART2=(unsigned)Uart2_TxFifo;
    rULCON2=(0<<6)|(0<<3)|(0<<2)|(3);	// Normal,No parity,One stop bit, 8bit
    rUCON2 |= (TX_INTTYPE<<9)|(RX_INTTYPE<<8)|(0<<7)|(0<<6)|(0<<5)|(0<<4)|(1<<2)|(1);
    //Clock,Tx:Def,Rx:Def,Rx timeout:x,Rx error int:x,Loop-back:x,Send break:x,Tx:int,Rx:int
    rUFCON2=(3<<6)|(2<<4)|(1<<2)|(1<<1)|(1);
    //Tx and Rx FIFO Trigger Level:12byte,Tx and Rx FIFO Reset,FIFO on
    rINTMSK=~(BIT_UART2);
    rINTSUBMSK=~(BIT_SUB_TXD2);
    Delay(500);
    rUFCON2=(3<<6)|(2<<4)|(1<<2)|(1<<1)|(0);
    //Tx and Rx FIFO Trigger Level:12byte,Tx and Rx FIFO Reset,FIFO off

    
    /******* UART2 Rx FIFO test with interrupt ********/     
    rx_dncs=0;
    rx_point=0;
    rx_isdone=1;
    rx_filesize=0;
    rx_checksum=0;
    Uart_Printf("[Uart channel 2 Rx FIFO Interrupt Test]\n");

    pISR_UART2=(unsigned)Uart2_RxFifoOrErr;

    rULCON2=(0<<6)|(0<<3)|(0<<2)|(3);	// Normal,No parity,One stop bit, 8bit
    rUCON2 |= (TX_INTTYPE<<9)|(RX_INTTYPE<<8)|(1<<7)|(1<<6)|(0<<5)|(0<<4)|(1<<2)|(1);
    //Clock,Tx:Def,Rx:Def,Rx timeout:o,Rx error int:o,Loop-back:x,Send break:x,Tx:int,Rx:int
    rUFCON2=(1<<6)|(0<<4)|(1<<2)|(1<<1)|(1);
    //Tx and Rx FIFO Trigger Level:4byte,Tx and Rx FIFO Reset,FIFO on

    // Clear Int Pending and Unmask 
    ClearPending(BIT_UART2);
    rINTMSK=~(BIT_UART2);
    rSUBSRCPND=(BIT_SUB_RXD2|BIT_SUB_TXD2|BIT_SUB_ERR2);
    rINTSUBMSK=~(BIT_SUB_RXD2|BIT_SUB_ERR2);

    Uart_Printf("Download the target file[*.bin] by Uart2\n");
	
    while(rx_isdone);

    rINTMSK |= (BIT_UART2);	
    rINTSUBMSK|=(BIT_SUB_RXD2|BIT_SUB_TXD2|BIT_SUB_ERR2);

    rUFCON2=(3<<6)|(2<<4)|(1<<2)|(1<<1)|(0);
    //Tx and Rx FIFO Trigger Level:12byte,Tx and Rx FIFO Reset,FIFO off
			
    if(rx_dncs==(rx_checksum&0xffff)) 
	Uart_Printf("\nDownload test OK!!!\n");
    else 
	Uart_Printf("\nError!!!\n");


    //----------------------------------------------------------------------------------------->    
    Uart_Printf("Jumper Setting  <======>  J16: 2-3, J18:2-3");
    Uart_Printf("\nConnect PC[COM1 or COM2] and UART1 of SMDK2440 with a serial cable!!! \n");
    Uart_Printf("Then, press any key........\n");
    Uart_Select(1);
    Uart_Getch();
    //--------------------------->


    Uart_Port_Return();	
}

