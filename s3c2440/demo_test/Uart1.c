//====================================================================
// File Name : uart1.c
// Function  : S3C2440 UART-1 test code
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
#include "uart1.h"

void __irq Uart1_TxInt(void);
void __irq Uart1_RxIntOrErr(void);
void __irq Uart1_TxDmaDone(void);
void __irq Uart1_RxDmaDone(void);
void __irq Uart1_RxDmaOrErr(void);
void __irq Uart1_TxFifo(void);
void __irq Uart1_RxFifoOrErr(void);
void __irq Uart1_AfcTx(void);
void __irq Uart1_AfcRxOrErr(void);

void __sub_Uart1_RxInt(void);
void __sub_Uart1_RxFifo(void);
void __sub_Uart1_RxAfc(void);
void __sub_Uart1_RxErrInt(void);

volatile static char *uart1TxStr;
volatile static char *uart1RxStr;

extern U32 Pclk;

//---------------------------------------UART1 test function-------------------------------------
void __irq Uart1_TxInt(void)
{
    rINTSUBMSK|=(BIT_SUB_RXD1|BIT_SUB_TXD1|BIT_SUB_ERR1);	// Just for the safety
    if(*uart1TxStr != '\0')
    {
    	WrUTXH1(*uart1TxStr++);
        ClearPending(BIT_UART1);	// Clear master pending
        rSUBSRCPND=(BIT_SUB_TXD1);
        rINTSUBMSK&=~(BIT_SUB_TXD1);	// Unmask sub int
    }
    else
    {
    	isTxInt=0;
        ClearPending(BIT_UART1);	// Clear master pending
        rSUBSRCPND=(BIT_SUB_TXD1);
        rINTMSK|=(BIT_UART1);		// Unmask sub int
    }
}

void __irq Uart1_RxIntOrErr(void)
{
    rINTSUBMSK|=(BIT_SUB_RXD1|BIT_SUB_TXD1|BIT_SUB_ERR1);
    if(rSUBSRCPND&BIT_SUB_RXD1) __sub_Uart1_RxInt();
    else __sub_Uart1_RxErrInt();

    ClearPending(BIT_UART1);
    rSUBSRCPND=(BIT_SUB_RXD1|BIT_SUB_ERR1);	// Clear Sub int pending    
    rINTSUBMSK&=~(BIT_SUB_RXD1|BIT_SUB_ERR1);    
}

void __irq Uart1_TxDmaDone(void)
{
    rDMASKTRIG1=0x0;	// Stop Dma1
    isDone=0;
    rINTMSK |= BIT_DMA1;
    ClearPending(BIT_DMA1);
}

void __irq Uart1_RxDmaDone(void)
{
    rDMASKTRIG1=0x0;	//DMA1 Channel Off
    isDone=0;
    *(uart1RxStr+5)='\0';
    rINTMSK|=(BIT_DMA1);
    ClearPending(BIT_DMA1);
}

void __irq Uart1_RxDmaOrErr(void)
{
    rINTSUBMSK|=(BIT_SUB_RXD1|BIT_SUB_TXD1|BIT_SUB_ERR1);
    if(rSUBSRCPND&BIT_SUB_RXD1) Uart_Printf("Error : UART1 Rx Interrupt is occured!!!\n");
    else __sub_Uart1_RxErrInt();

    ClearPending(BIT_UART1);
    rSUBSRCPND=(BIT_SUB_ERR1);	// Clear Sub int pending    
    rINTSUBMSK&=~(BIT_SUB_ERR1);    
}


void __irq Uart1_RxFifoOrErr(void)
{
    rINTSUBMSK|=(BIT_SUB_RXD1|BIT_SUB_TXD1|BIT_SUB_ERR1);
    if(rSUBSRCPND&BIT_SUB_RXD1) __sub_Uart1_RxFifo();
    else __sub_Uart1_RxErrInt();
    ClearPending(BIT_UART1);
    rSUBSRCPND=(BIT_SUB_RXD1|BIT_SUB_ERR1);
    rINTSUBMSK&=~(BIT_SUB_RXD1|BIT_SUB_ERR1);
}

void __sub_Uart1_RxInt(void)
{
    if(RdURXH1()!='\r') 
    {
    	Uart_Printf("%c",RdURXH1());
        *uart1RxStr++ =(char) RdURXH1();
    }
    else
    {
    	isRxInt=0;
    	*uart1RxStr='\0';
    	Uart_Printf("\n");
    }
}

void __sub_Uart1_RxFifo(void)
{
    while(rUFSTAT1&0x7f)	//During the Rx FIFO is not empty
    {
	rx_point++;
	if(rx_point<5)
		 rx_filesize |= (RdURXH1()<<(8*(rx_point-1))); // First 4-bytes mean file size
	else if(rx_point>(rx_filesize-2))	
	{
		rx_dncs |= (RdURXH1()<<(8*(1-(rx_filesize-rx_point)))); //Last 2-bytes mean checksum.
		if(rx_point==rx_filesize) rx_isdone=0;
	}
	else
		rx_checksum+=RdURXH1();
    }
}

  
void __sub_Uart1_RxErrInt(void)
{
    switch(rUERSTAT1)//to clear and check the status of register bits
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


void Test_Uart1_Int(void)
{
	U8 ch;
	int iBaud;
	
    Uart_Port_Set(); 
    Uart_Select(1);    

	/******** For operating clock test *********/
	Uart_Printf("[Uart channel 0 Interrupt test]\n");
	Uart_Printf("Select operating clock 1. PCLK(D)    2. UCLK     3. FCLK/n  \n");
	ch = Uart_Getch();
	switch (ch)
	{
	case '2' :
		rMISCCR = rMISCCR & ~(7<<8) | (1<<10); // CLKOUT1 = PCLK
		Uart_Uextclk_En(1, 115200, Pclk);
		break;
	case '3' :
		Uart_Printf("Type the baudrate and then change the same baudrate of host, too.\n");
       Uart_Printf("Baudrate (ex 9600, 115200[D], 921600) : ");
		iBaud = Uart_GetIntNum();
		if (iBaud == -1) iBaud = 115200;
		Uart_Fclkn_En(1, iBaud);
		Uart_Getch();
		break;
	default :
		Uart_Pclk_En(1, 115200);
	}

#if 0	
	/******** Select UART or IrDA *********/
	Uart_Printf("Select 1. UART(D) or 2. IrDA mode\nChoose : ");
	if (Uart_Getch() == '2')
		rULCON1 |= (1<<6); // IrDA mode
	else
		rULCON1 &= ~(1<<6); // UART mode
#endif

    /*********** UART1 Tx test with interrupt ***********/  
    isTxInt=1;
    uart1TxStr="ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890->UART1 Tx interrupt test is good!!!!\r\n";
    Uart_Printf("[Uart channel 1 Tx Interrupt Test]\n");

    pISR_UART1=(unsigned)Uart1_TxInt;

   
    rULCON1=(0<<6)|(0<<3)|(0<<2)|(3);	// Normal,No parity,One stop bit, 8bit
    rUCON1 |= (TX_INTTYPE<<9)|(RX_INTTYPE<<8)|(0<<7)|(0<<6)|(0<<5)|(0<<4)|(1<<2)|(1);
    //Clock,Tx:Def,Rx:Def,Rx timeout:x,Rx error int:x,Loop-back:x,Send break:x,Tx:int,Rx:int
    Uart_TxEmpty(1); //wait until tx buffer is empty.
    rINTMSK=~(BIT_UART1);
    rINTSUBMSK=~(BIT_SUB_TXD1);

    while(isTxInt);

    /*********** UART1 Rx test with interrupt ***********/
    isRxInt=1;
    uart1RxStr=(volatile char *)UARTBUFFER;
    Uart_Printf("\n[Uart channel 1 Rx Interrupt Test]:\n");
    Uart_Printf("After typing ENTER key, you will see the characters which was typed by you.");
    Uart_Printf("\nTo quit, press ENTER key.!!!\n");
    
    pISR_UART1 =(unsigned)Uart1_RxIntOrErr;

    rULCON1=(0<<6)|(0<<3)|(0<<2)|(3);	// Normal,No parity,One stop bit, 8bit
    rUCON1 |= (1<<6);
    //Clock,Tx:pulse,Rx:pulse,Rx timeout:x,Rx error int:o,Loop-back:x,Send break:x,Tx:int,Rx:int

    // Clear Int Pending and Unmask
    ClearPending(BIT_UART1);
    rINTMSK=~(BIT_UART1);
    rSUBSRCPND=(BIT_SUB_RXD1|BIT_SUB_ERR1);
    rINTSUBMSK=~(BIT_SUB_RXD1|BIT_SUB_ERR1);
    
    while(isRxInt);

    rINTSUBMSK|=(BIT_SUB_RXD1|BIT_SUB_TXD1|BIT_SUB_ERR1);
    rINTMSK|=(BIT_UART1);

    Uart_Printf("%s\n",(char *)UARTBUFFER);
	Uart_Printf("Change the baudrate of terminal to 115200 bps and Press Enter.\n");
	Uart_Port_Return();
}

void Test_Uart1_Dma(void)
{
    Uart_Port_Set(); 
    Uart_Select(1);    
    /*********** UART1 Tx test with DMA1 ***********/ 
    isDone=1;
    uart1TxStr="ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890->UART1 Tx Test by DMA1 is good!!!!\r\n";
    Uart_Printf("\n[Uart channel 1 DMA1 Tx Test]\n");
    Uart_TxEmpty(1);

    pISR_DMA1  =(unsigned)Uart1_TxDmaDone;

    rULCON1=(0<<6)|(0<<3)|(0<<2)|(3);	// Normal,No parity,One stop bit, 8bit
    rUCON1 |= (TX_INTTYPE<<9)|(RX_INTTYPE<<8)|(0<<7)|(0<<6)|(0<<5)|(0<<4)|(3<<2)|(1);
    //Clock,Tx:Def,Rx:Def,Rx timeout:x,Rx error int:x,Loop-back:x,Send break:x,Tx:dma1,Rx:int

    /***DMA1 init***/
    rDISRC1=(U32)uart1TxStr;	// Start address
    rDISRCC1=(0<<1)|(0);		// AHB,Increment
    rDIDST1=(U32)UTXH1;			// Memory buffer Address
    rDIDSTC1=(1<<1)|(1);		// APB,Fixed
    rDCON1=((unsigned int)1<<31)|(0<<30)|(1<<29)|(0<<28)|(0<<27)|(1<<24)|(1<<23)|(1<<22)|(0<<20)|strlen((char*)uart1TxStr);
    //handshake, sync PCLK, TC int, single tx, single service, Uart1, H/W request,auto-reload off, Byte size Tx, Tx count value
    rINTMSK=~(BIT_DMA1);
    rDMASKTRIG1=(0<<2)|(1<<1)|(0);    //no-stop, DMA1 channel on, no-SW trigger 

    while(isDone);

    /*********** UART1 Rx test with DMA1 ***********/ 
    isDone=1;
    uart1RxStr=(char *)UARTBUFFER;

    Uart_Printf("\n[Uart channel 1 DMA1 Rx Test]\n");
    Uart_Printf("Type any five keys!!!\n");    
    Uart_Printf("Then you will see what you typed.\n");

    pISR_DMA1=(unsigned)Uart1_RxDmaDone;
    pISR_UART1=(unsigned)Uart1_RxDmaOrErr;

    rULCON1=(0<<6)|(0<<3)|(0<<2)|(3);	// Normal,No parity,One stop bit, 8bit
    rUCON1 &= 0x400;	// For the PCLK <-> UCLK fuction    
    rUCON1 |= (TX_INTTYPE<<9)|(RX_INTTYPE<<8)|(0<<7)|(1<<6)|(0<<5)|(0<<4)|(1<<2)|(3);
    //Clock,Tx:Def,Rx:Def,Rx timeout:x,Rx error int:o,Loop-back:x,Send break:x,Tx:int,Rx:dma1

    /***DMA1 init***/
    rDISRC1=(U32)URXH1;			// Start address
    rDISRCC1=(1<<1)|(1);		// APB,Fixed
    rDIDST1=(U32)uart1RxStr;	// Memory buffer Address
    rDIDSTC1= (0<<1)|(0);		// AHB,Increment
    rDCON1=((unsigned int)1<<31)|(0<<30)|(1<<29)|(0<<28)|(0<<27)|(1<<24)|(1<<23)|(1<<22)|(0<<20)|(5);
    //handshake, sync PCLK, TC int, single tx, single service, Uart1, H/W request,auto-reload off, Byte size Tx, Tx count value

    // Clear Int Pending and Unmask
    ClearPending(BIT_UART1);
    rINTMSK=~(BIT_DMA1|BIT_UART1);
    rSUBSRCPND=(BIT_SUB_RXD1|BIT_SUB_TXD1|BIT_SUB_ERR1);
    rINTSUBMSK=~(BIT_SUB_ERR1);
    rDMASKTRIG1=(0<<2)|(1<<1)|(0);    //no-stop, DMA1 channel on, no-SW trigger 

    while(isDone);

    Uart_Printf("%s\n",uart1RxStr);
    
    Uart_Port_Return();
}


void __irq Uart1_TxFifo(void)
{
    rINTSUBMSK|=(BIT_SUB_RXD1|BIT_SUB_TXD1|BIT_SUB_ERR1);	// Just for the safety
    while (!(rUFSTAT1 & 0x4000) && (*uart1TxStr != '\0')) 	//until tx fifo full or end of string
    	WrUTXH1(*uart1TxStr++);	

    if(*uart1TxStr == '\0') 
    {
    	rINTMSK|=BIT_UART1;
        rSUBSRCPND=BIT_SUB_TXD1;	// Clear Sub int pending
        ClearPending(BIT_UART1);	// Clear master pending
    }
    else 
    {
        ClearPending(BIT_UART1);	// Clear master pending
        rSUBSRCPND=BIT_SUB_TXD1;	// Clear Sub int pending
        rINTSUBMSK&=~(BIT_SUB_TXD1);	// Unmask sub int
    }
}

void Test_Uart1_Fifo(void)
{
    Uart_Port_Set(); 
    Uart_Select(1);
    /******* UART1 Tx FIFO test with interrupt ********/     
    Uart_Printf("[Uart channel 1 Tx FIFO Interrupt Test]\n");
    Uart_TxEmpty(1);	//wait until tx buffer is empty.
    
      
    /* <Tx Trigger Level:8Byte> */    
    uart1TxStr="ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890->UART1 Tx FIFO interrupt(8byte) test is good!!!!\r\n";
    pISR_UART1=(U32)Uart1_TxFifo;
    rULCON1=(0<<6)|(0<<3)|(0<<2)|(3);	// Normal,No parity,One stop bit, 8bit
    rUCON1 |= (TX_INTTYPE<<9)|(RX_INTTYPE<<8)|(0<<7)|(0<<6)|(0<<5)|(0<<4)|(1<<2)|(1);
    //Clock,Tx:Def,Rx:Def,Rx timeout:x,Rx error int:x,Loop-back:x,Send break:x,Tx:int,Rx:int
    rUFCON1=(2<<6)|(1<<4)|(1<<2)|(1<<1)|(1);
    //Tx and Rx FIFO Trigger Level:8byte,Tx and Rx FIFO Reset,FIFO on
    rINTMSK=~(BIT_UART1);
    rINTSUBMSK=~(BIT_SUB_TXD1);
    Delay(500);
    rUFCON1=(2<<6)|(1<<4)|(1<<2)|(1<<1)|(0);
    //Tx and Rx FIFO Trigger Level:8byte,Tx and Rx FIFO Reset,FIFO off

    /* <Tx Trigger Level:12Byte> */    
    uart1TxStr="ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890->UART1 Tx FIFO interrupt(12byte) test is good!!!!\r\n";
    pISR_UART1=(unsigned)Uart1_TxFifo;
    rULCON1=(0<<6)|(0<<3)|(0<<2)|(3);	// Normal,No parity,One stop bit, 8bit
    rUCON1 |= (TX_INTTYPE<<9)|(RX_INTTYPE<<8)|(0<<7)|(0<<6)|(0<<5)|(0<<4)|(1<<2)|(1);
    //Clock,Tx:Def,Rx:Def,Rx timeout:x,Rx error int:x,Loop-back:x,Send break:x,Tx:int,Rx:int
    rUFCON1=(3<<6)|(2<<4)|(1<<2)|(1<<1)|(1);
    //Tx and Rx FIFO Trigger Level:12byte,Tx and Rx FIFO Reset,FIFO on
    rINTMSK=~(BIT_UART1);
    rINTSUBMSK=~(BIT_SUB_TXD1);
    Delay(500);
    rUFCON1=(3<<6)|(2<<4)|(1<<2)|(1<<1)|(0);
    //Tx and Rx FIFO Trigger Level:12byte,Tx and Rx FIFO Reset,FIFO off

    
    /******* UART1 Rx FIFO test with interrupt ********/     
    rx_dncs=0;
    rx_point=0;
    rx_isdone=1;
    rx_filesize=0;
    rx_checksum=0;
    Uart_Printf("[Uart channel 1 Rx FIFO Interrupt Test]\n");

    pISR_UART1=(unsigned)Uart1_RxFifoOrErr;

    rULCON1=(0<<6)|(0<<3)|(0<<2)|(3);	// Normal,No parity,One stop bit, 8bit
    rUCON1 |= (TX_INTTYPE<<9)|(RX_INTTYPE<<8)|(1<<7)|(1<<6)|(0<<5)|(0<<4)|(1<<2)|(1);
    //Clock,Tx:Def,Rx:Def,Rx timeout:o,Rx error int:o,Loop-back:x,Send break:x,Tx:int,Rx:int
    rUFCON1=(1<<6)|(0<<4)|(1<<2)|(1<<1)|(1);
    //Tx and Rx FIFO Trigger Level:4byte,Tx and Rx FIFO Reset,FIFO on

    // Clear Int Pending and Unmask 
    ClearPending(BIT_UART1);
    rINTMSK=~(BIT_UART1);
    rSUBSRCPND=(BIT_SUB_RXD1|BIT_SUB_TXD1|BIT_SUB_ERR1);
    rINTSUBMSK=~(BIT_SUB_RXD1|BIT_SUB_ERR1);

    Uart_Printf("Download the target file[*.bin] by Uart1\n");
	
    while(rx_isdone);

    rINTMSK |= (BIT_UART1);	
    rINTSUBMSK|=(BIT_SUB_RXD1|BIT_SUB_TXD1|BIT_SUB_ERR1);

    rUFCON1=(3<<6)|(2<<4)|(1<<2)|(1<<1)|(0);
    //Tx and Rx FIFO Trigger Level:12byte,Tx and Rx FIFO Reset,FIFO off
			
    if(rx_dncs==(rx_checksum&0xffff)) 
	Uart_Printf("\nDownload test OK!!!\n");
    else 
	Uart_Printf("\nError!!!\n");

    Uart_Port_Return();	
}



void __irq Uart1_AfcTx(void)
{
    rINTSUBMSK|=(BIT_SUB_RXD1|BIT_SUB_TXD1|BIT_SUB_ERR1);

    if(tx_cnt < (AFC_BUFLEN))
    {
		Uart_Printf("%d,",*txdataPt);
		WrUTXH1(*txdataPt++);
		tx_cnt++;
        ClearPending(BIT_UART1);
        rSUBSRCPND=(BIT_SUB_TXD1);
        rINTSUBMSK&=~(BIT_SUB_TXD1);
    }
    else
    {
    	tx_end=1;
        while(rUFSTAT1 & 0x7f00);	//Until FIFO is empty
        ClearPending(BIT_UART1);
        rSUBSRCPND=(BIT_SUB_TXD1);
    	rINTMSK|=(BIT_UART1);
    }
}

void __irq Uart1_AfcRxOrErr(void)
{
    rINTSUBMSK|=(BIT_SUB_RXD1|BIT_SUB_TXD1|BIT_SUB_ERR1);
    if(rSUBSRCPND&BIT_SUB_RXD1) __sub_Uart1_RxAfc();
    else __sub_Uart1_RxErrInt();

    rSUBSRCPND=(BIT_SUB_RXD1|BIT_SUB_TXD1|BIT_SUB_ERR1);
    rINTSUBMSK&=~(BIT_SUB_RXD1|BIT_SUB_ERR1);
    ClearPending(BIT_UART1);
}

void __sub_Uart1_RxAfc(void)
{
    while( rUFSTAT1 & 0x7f )
    {
	*rxdataPt=rURXH1;
	Uart_Printf("%d,",*rxdataPt++);
	rx_cnt++;
    }
    if(rx_cnt == AFC_BUFLEN)
    {
    	rx_end=1;
    	rINTMSK|=BIT_UART1;
    }
}


void Test_Uart1_AfcTx(void)
{
    int i;
    Uart_Port_Set(); 
    //U32 safc_rGPHCON,safc_rGPHDAT,safc_rGPHUP;
    tx_cnt=0;
    tx_end=0;
    txdataFl=(volatile U8 *)UARTBUFFER;
    txdataPt=(volatile U8 *)UARTBUFFER;
    for(i=0;i<AFC_BUFLEN;i++) 
    	*txdataFl++=i;	// Initialize the AFC data
    Uart_Select(1);
    Uart_Printf("[Uart channel 1 AFC Tx Test]\n");
    Uart_Printf("This test should be configured two boards.\n");
    Uart_Printf("Connect Tx and Rx Board with twitsted(rx/tx, nCTS/nRTS) cable .\n");
    Uart_Printf("\nConnect PC[COM1 or COM2] and UART0 of SMDK2440 with a serial cable!!! \n");
    Uart_Printf("Then, press any key........\n");
    Uart_Select(0);	// Change the uart port    
    Uart_Getch();
    Uart_Printf("Start Rx first and then press any key to start Tx.....\n");
    Uart_Getch();
    
    pISR_UART1=(unsigned) Uart1_AfcTx;

    rULCON1=(0<<6)|(0<<3)|(0<<2)|(3);	// Normal,No parity,One stop bit, 8bit
    rUCON1 &= 0x400;	// For the PCLK <-> UCLK fuction    
    rUCON1 |= (1<<9)|(1<<8)|(0<<7)|(0<<6)|(0<<5)|(0<<4)|(1<<2)|(1);
    //Clock,Tx:Lev,Rx:Lev,Rx timeout:x,Rx error int:x,Loop-back:x,Send break:x,Tx:int,Rx:int
    rUFCON1=(1<<6)|(0<<4)|(1<<2)|(1<<1)|(1);
    //Tx and Rx FIFO Trigger Level:4byte,Tx and Rx FIFO Reset,FIFO on
    rUMCON1=0x10;   // Enable Uart0 AFC 

    rINTMSK=~(BIT_UART1);
    rINTSUBMSK=~(BIT_SUB_RXD1|BIT_SUB_TXD1|BIT_SUB_ERR1);

    while(!tx_end);

  //  rINTMSK |= (BIT_UART1);	
    rINTSUBMSK|=(BIT_SUB_RXD1|BIT_SUB_TXD1|BIT_SUB_ERR1);
    rUFCON1=(3<<6)|(2<<4)|(1<<2)|(1<<1)|(0);
    //Tx and Rx FIFO Trigger Level:12byte,Tx and Rx FIFO Reset,FIFO off
    Uart_Printf("\nEnd Tx, transfer data count=%d\n",tx_cnt);

    Uart_Printf("\n connect  between PC[COM1 or COM2] and UART0 of SMDK2440!!! \n");
    Uart_Printf("Start Rx first and then press any key to start Tx.....\n");
    Uart_Select(1);
    Uart_Getch();
	
    Uart_Port_Return();
}

void Test_Uart1_AfcRx(void)
{
    unsigned int i;
    rx_cnt=0;
    rx_end=0;
    afc_err=0;
    rxdataCk=(volatile U8 *)UARTBUFFER;
    rxdataPt=(volatile U8 *)UARTBUFFER;
    Uart_Port_Set(); 
    Uart_Select(1);
    Uart_Printf("[Uart channel 1 AFC Tx Test]\n");
    Uart_Printf("This test should be configured two boards.\n");
    Uart_Printf("Connect Tx and Rx Board with twitsted(rx/tx, nCTS/nRTS) cable .\n");
    Uart_Printf("\nConnect PC[COM1 or COM2] and UART0 of SMDK2440 with a serial cable!!! \n");
    Uart_Printf("Then, press any key........\n");
    Uart_Select(0);	// Change the uart port    
    Uart_Getch();
    Uart_Printf("Start Rx first and then press any key to start Tx.....\n");
    Uart_Getch();
    
    pISR_UART1=(unsigned) Uart1_AfcRxOrErr;

    rULCON1=(0<<6)|(0<<3)|(0<<2)|(3);	// Normal,No parity,One stop bit, 8bit
    rUCON1 &= 0x400;	// For the PCLK <-> UCLK fuction    
    rUCON1 |= (1<<9)|(1<<8)|(1<<7)|(1<<6)|(0<<5)|(0<<4)|(1<<2)|(1);
    //Clock,Tx:Lev,Rx:Lev,Rx timeout:o,Rx error int:o,Loop-back:x,Send break:x,Tx:int,Rx:int
    rUFCON1=(1<<6)|(0<<4)|(1<<2)|(1<<1)|(1);
    //Tx and Rx FIFO Trigger Level:4byte,Tx and Rx FIFO Reset,FIFO on
    rUMCON1=0x10;   // Enable Uart1 AFC 

  
    // Clear Int Pending and Unmask 
//    rSUBSRCPND=(BIT_SUB_RXD1|BIT_SUB_TXD1|BIT_SUB_ERR1);
    rINTSUBMSK=~(BIT_SUB_RXD1|BIT_SUB_ERR1);
//    ClearPending(BIT_UART1);
    rINTMSK=~(BIT_UART1);

    while(!rx_end);

//    rINTMSK |= (BIT_UART1);	
    rINTSUBMSK|=(BIT_SUB_RXD1|BIT_SUB_TXD1|BIT_SUB_ERR1);
    rUFCON1=(3<<6)|(2<<4)|(1<<2)|(1<<1)|(0);
    //Tx and Rx FIFO Trigger Level:12byte,Tx and Rx FIFO Reset,FIFO off
    Uart_Printf("\nEnd Rx, receive data count=%d\n",rx_cnt);
    for(i=0;i<AFC_BUFLEN;i++) 
    	if(i-(*rxdataCk++)) {
    		Uart_Printf("i=%d\n",i);
    		afc_err++;
    		}
    if(afc_err)
    	Uart_Printf("AFC test fail!! Error count=%d\n",afc_err);
    else
    	Uart_Printf("AFC test is good!!\n");

    Uart_Printf("\n connect between PC[COM1 or COM2] and UART0 of SMDK2440!!! \n");
    Uart_Printf("Press any key to start Rx and then Star Tx....\n");
    Uart_Select(1);
    Uart_Getch();
	
    Uart_Port_Return();
}



