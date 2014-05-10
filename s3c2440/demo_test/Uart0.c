//====================================================================
// File Name : uart0.c
// Function  : S3C2440 UART-0 test code
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
#include "def.h"
#include "2440addr.h"
#include "2440lib.h"
#include "uart0.h"

void Uart_Port_Set(void);
void Uart_Port_Return(void);

void __irq Uart0_TxInt(void);
void __irq Uart0_RxIntOrErr(void);
void __irq Uart0_TxDmaDone(void);
void __irq Uart0_RxDmaDone(void);
void __irq Uart0_RxDmaOrErr(void);
void __irq Uart0_TxFifo(void);
void __irq Uart0_RxFifoOrErr(void);
void __irq Uart0_AfcTx(void);
void __irq Uart0_AfcRxOrErr(void);
void __irq Uart0_RxOverrunErr(void);

void __sub_Uart0_RxInt(void);
void __sub_Uart0_RxFifo(void);
void __sub_Uart0_RxAfc(void);
void __sub_Uart0_RxErrInt(void);

void Uart_Fclkn_Dis(void); // for 2440A


volatile U16 rx_dncs;
volatile U32 rx_point,rx_isdone,rx_filesize,rx_checksum;
volatile U32 isDone,isTxInt,isRxInt,tx_cnt,tx_end,rx_cnt,rx_end,afc_err;
volatile U8 *txdataPt;
volatile U8 *txdataFl;
volatile U8 *rxdataPt;
volatile U8 *rxdataCk;
volatile U32 *dbg_data;
U8 temp_exit;


volatile U32 save_rGPHCON,save_rGPHDAT,save_rGPHUP;
volatile U32 save_ULCON0,save_UCON0,save_UFCON0,save_UMCON0,save_UBRDIV0;
volatile U32 save_ULCON1,save_UCON1,save_UFCON1,save_UMCON1,save_UBRDIV1;
volatile U32 save_ULCON2,save_UCON2,save_UFCON2,save_UMCON2,save_UBRDIV2;

volatile static char *uart0TxStr;
volatile static char *uart0RxStr;

//volatile static char *uart2TxStr;
//volatile static char *uart2RxStr;

extern U32 Fclk, Pclk;


void Uart_Port_Set(void)
{
    //Push UART GPIO port configuration
    save_rGPHCON=rGPHCON; 
    save_rGPHDAT=rGPHDAT;
    save_rGPHUP=rGPHUP;
    //Configure UART port
    rGPHCON&=0x3c0000;
    rGPHCON|=0x2aaaa;	// enable all uart channel
    rGPHUP|=0x1ff;	//Uart port pull-up disable
	rGPGCON|=(0xf<<18); // nRTS1, nCTS1
	rGPGUP|=(0x3<<9);	
    //rINTSUBMSK=0x7ff;   //SUBINT ALL MASK
    //Push Uart control registers 
    save_ULCON0=rULCON0;   		
    save_UCON0=rUCON0;	   	
    save_UFCON0=rUFCON0;   		
    save_UMCON0=rUMCON0;   		
    save_ULCON1=rULCON1;		
    save_UCON1 =rUCON1;		
    save_UFCON1=rUFCON1;		
    save_UMCON1=rUMCON1;	
    save_ULCON2=rULCON2;		
    save_UCON2 =rUCON2;		
    save_UFCON2=rUFCON2;		
    save_UMCON2=rUMCON2;		
	save_UBRDIV0=rUBRDIV0;
	save_UBRDIV1=rUBRDIV1;
	save_UBRDIV2=rUBRDIV2;
    //Initialize UART1,2 port
}       
        
void Uart_Port_Return(void)
{       
    //Pop UART GPIO port configuration
    rGPHCON=save_rGPHCON;
    rGPHDAT=save_rGPHDAT;
    rGPHUP=save_rGPHUP;
    //Pop Uart control registers 
    rULCON0=save_ULCON0;   		
    rUCON0 =save_UCON0;   	
    rUFCON0=save_UFCON0;   		
    rUMCON0=save_UMCON0;   		
    rULCON1=save_ULCON1;		
    rUCON1 =save_UCON1;	
    rUFCON1=save_UFCON1;		
    rUMCON1=save_UMCON1;
    rULCON2=save_ULCON2;		
    rUCON2 =save_UCON2;	
    rUFCON2=save_UFCON2;		
    rUMCON2=save_UMCON2;
	rUBRDIV0=save_UBRDIV0;
	rUBRDIV1=save_UBRDIV1;
	rUBRDIV2=save_UBRDIV2;
	Uart_Fclkn_Dis();
}

// 2440X usage enable
void Uart_Uextclk_En(int ch,int baud, int clock)
{
    if(ch == 0) {
	   	rUCON0  = rUCON0 & ~(1<<11) |(1<<10);	// Select UEXTCLK
     	rUBRDIV0=( (int)(clock/16./baud) -1 );	//Baud rate divisior register
    }
    else if(ch==1){
	   	rUCON1  = rUCON1 & ~(1<<11) |(1<<10);	// Select UEXTCLK
    	rUBRDIV1=( (int)(clock/16./baud) -1 );	//Baud rate divisior register
    }
    else {
	   	rUCON2  = rUCON2 & ~(1<<11) |(1<<10);	// Select UEXTCLK
       rUBRDIV2=( (int)(clock/16./baud) -1 );	//Baud rate divisior register
    }
}

//2440X usage enable
void Uart_Pclk_En(int ch, int baud)
{
    if(ch == 0) {
    	rUCON0 &= ~(3<<10);	// Select PCLK
		rUBRDIV0=( (int)(Pclk/16./baud+0.5) -1 );	//Baud rate divisior register
   }
    else if(ch==1){
    	rUCON1 &= ~(3<<10);	// Select PCLK
		rUBRDIV1=( (int)(Pclk/16./baud+0.5) -1 );	//Baud rate divisior register
   }
    else {
    	rUCON2 &= ~(3<<10);	// Select PCLK
	   	rUBRDIV2=( (int)(Pclk/16./baud+0.5) -1 );	//Baud rate divisior register
   }
}

void Uart_Fclkn_En(int ch, int baud) // for 2440A
{	
	int clock = PCLK;//Pclk;

	Uart_Printf("Current FCLK is %d\n", Fclk);
#if 1
	// input clock divider setting.
	if ( (Fclk>290000000) && (Fclk<300000000) ) // for 296MHz
	{
		rUCON0 = (rUCON0 & ~(0xf<<12)) | (0xe<<12);  // FCLK divider 14(n=20), for max 921.6kbps
		rUCON1 &= ~(0xf<<12); // 0 setting
		rUCON2 &= ~(0xf<<12); // 0 setting
		clock = Fclk / 20;
		Uart_Printf("1 : %d\n", clock);
	}
	else if ( (Fclk>395000000) && (Fclk<405000000) ) // for 399MHz
	{
		rUCON1 = (rUCON1 & ~(0xf<<12)) | (0x6<<12);  // FCLK divider 6(n=27), for max 921.6kbps
		rUCON0 &= ~(0xf<<12); // 0 setting
		rUCON2 &= ~(0xf<<12); // 0 setting
		clock = Fclk / 27;
		Uart_Printf("2 : %d\n", clock);
	}
	else if ( (Fclk>525000000) && (Fclk<535000000) ) // for 530MHz
	{
		rUCON1 |= (0xf<<12);  // FCLK divider 15(n=36), for max. 921.6kbps
		rUCON0 &= ~(0xf<<12); // 0 setting
		rUCON2 &= ~(0xf<<12); // 0 setting
		clock = Fclk / 36;
		Uart_Printf("3 : %d\n", clock);
	}
	rUCON2 |= (1<<15); // enable FCLK/n
#else
	// In 921.6kbps case of following code, Fclk must be 296352000
	rUCON0 = rUCON0 & ~(0xf<<12) | (0xe<<12);  // FCLK divider 14(n=20), for max 921.6kbps
	rUCON1 &= ~(0xf<<12); // 0 setting
	rUCON2 &= ~(0xf<<12); // 0 setting
	clock = Fclk / 20;
	rUCON2 |= (1<<15); // enable FCLK/n
#endif 

	// select buadrate.
    if(ch == 0) {
    	rUCON0 |= (3<<10);	// Select FCLK/n
		rUBRDIV0=( (int)(clock/16./baud+0.5) -1 );	//Baud rate divisior register
   }
    else if(ch==1){
    	rUCON1 |= (3<<10);	// Select FCLK/n
		rUBRDIV1=( (int)(clock/16./baud+0.5) -1 );	//Baud rate divisior register
   }
    else {
    	rUCON2 |= (3<<10);	// Select FCLK/n
	   	rUBRDIV2=( (int)(clock/16./baud+0.5) -1 );	//Baud rate divisior register
   }

	// S/W work-around for using FCLK/n
	rGPHCON = rGPHCON & ~(3<<16); //GPH8(UEXTCLK) input 
	Delay(1);
	rGPHCON = rGPHCON & ~(3<<16) | (1<<17); //GPH8(UEXTCLK) UEXTCLK
}

void Uart_Fclkn_Dis(void) // for 2440A
{
	// S/W work-around for using FCLK/n
	rGPHCON = rGPHCON & ~(3<<16); //GPH8(UEXTCLK) input 
	Delay(1);
	rGPHCON = rGPHCON & ~(3<<16) | (1<<17); //GPH8(UEXTCLK) UEXTCLK
}

//---------------------------------------UART0 test function-------------------------------------
void __irq Uart0_TxInt(void)
{
    rINTSUBMSK|=(BIT_SUB_RXD0|BIT_SUB_TXD0|BIT_SUB_ERR0);	// Just for the safety
    if(*uart0TxStr != '\0') 
    {
        WrUTXH0(*uart0TxStr++);
	    ClearPending(BIT_UART0);		// Clear master pending
	    rSUBSRCPND=(BIT_SUB_TXD0);		// Clear Sub int pending
	    rINTSUBMSK&=~(BIT_SUB_TXD0);	// Unmask sub int
    }
    else
    {
    	isTxInt=0;
    	ClearPending(BIT_UART0);		// Clear master pending
        rSUBSRCPND=(BIT_SUB_TXD0);		// Clear Sub int pending
        rINTMSK|=(BIT_UART0);
    }
}

void __irq Uart0_RxIntOrErr(void)
{
    rINTSUBMSK|=(BIT_SUB_RXD0|BIT_SUB_TXD0|BIT_SUB_ERR0);
    if(rSUBSRCPND&BIT_SUB_RXD0) __sub_Uart0_RxInt();
    else __sub_Uart0_RxErrInt();
    ClearPending(BIT_UART0); 
    rSUBSRCPND=(BIT_SUB_RXD0|BIT_SUB_ERR0);	// Clear Sub int pending    
    rINTSUBMSK&=~(BIT_SUB_RXD0|BIT_SUB_ERR0);    
}

void __sub_Uart0_RxInt(void)
{
    if(RdURXH0()!='\r') 
    {
    	Uart_Printf("%c",RdURXH0());
    	*uart0RxStr++ =(char)RdURXH0();
    }
    else
    {
    	isRxInt=0;
    	*uart0RxStr='\0';
    	Uart_Printf("\n");
    }
}

void __sub_Uart0_RxErrInt(void)
{
	U32 iStatus;

	iStatus = rUERSTAT0;
	
    switch(iStatus)//to clear and check the status of register bits
    {
	case 1:
	    Uart_Printf("Overrun error!\n");
	    break;
	case 2:
	    Uart_Printf("Parity error!\n");
	    break;
	case 4:
	    Uart_Printf("Frame error!\n");
	    break;
	case 6:
	    Uart_Printf("Parity, Frame error!\n");
	    break;
	case 8:
		Uart_Printf("Breake detect\n");
		break;
	case 0xa:
		Uart_Printf("Parity error & Break detect!\n");
		break;
	case 0xc:
		Uart_Printf("Frame error & Breake detect\n");
		break;
	case 0xe:
		Uart_Printf("Parity, Frame error & Break detect!\n");
		break;
	default :
		Uart_Printf("Unknown error : 0x%x\n", iStatus);
	    break;
    }
	
   	isRxInt=0;
}


void Test_Uart0_Int(void)
{
	U8 ch;
	int iBaud;
	
    Uart_Port_Set(); 

    Uart_Select(1);    
    Uart_Printf("\nConnect PC[COM1 or COM2] and UART0 of SMDK2440 with a serial cable!!! \n");
    Uart_Printf("Then, press any key........\n");
    Uart_Select(0);
    Uart_Getch();

	/******** For operating clock test *********/
	Uart_Printf("[Uart channel 0 Interrupt test]\n");
	Uart_Printf("Select operating clock 1. PCLK(D)    2. UCLK     3. FCLK/n  \nChoose : ");
	ch = Uart_Getch();	
	switch (ch)
	{
	case '2' :
		rMISCCR = rMISCCR & ~(7<<8) | (1<<10); // CLKOUT1 = PCLK
		Uart_Uextclk_En(0, 115200, Pclk);
		break;
	case '3' :
		Uart_Printf("Type the baudrate and then change the same baudrate of host, too.\n");
			 Uart_Printf("Baudrate (ex 9600, 115200[D], 921600) : ");
		iBaud = Uart_GetIntNum();
		if (iBaud == -1) iBaud = 115200;
		Uart_Fclkn_En(0, iBaud);
		Uart_Getch();
		break;
	default :
		Uart_Pclk_En(0, 115200);
	}

#if 0       
	/******** Select UART or IrDA *********/
	Uart_Printf("Select 1. UART(D) or  2. IrDA mode\nChoose : ");
	if (Uart_Getch() == '2')
		rULCON0 |= (1<<6); // IrDA mode
	else
		rULCON0 &= ~(1<<6); // UART mode
#endif

	/*********** UART0 Tx test with interrupt ***********/  
    isTxInt=1;
    uart0TxStr="ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890->UART0 Tx interrupt test is good!!!!\r\n";
    Uart_Printf("[Uart channel 0 Tx Interrupt Test]\n");

    pISR_UART0=(unsigned)Uart0_TxInt;

    rULCON0=(0<<6)|(0<<3)|(0<<2)|(3);	// Normal,No parity,One stop bit, 8bit
    rUCON0 |= (TX_INTTYPE<<9)|(RX_INTTYPE<<8)|(0<<7)|(0<<6)|(0<<5)|(0<<4)|(1<<2)|(1);
    //Clock,Tx:Def,Rx:Def,Rx timeout:x,Rx error int:x,Loop-back:x,Send break:x,Tx:int,Rx:int
    Uart_TxEmpty(1); //wait until tx buffer is empty.
    rINTMSK=~(BIT_UART0);
    rINTSUBMSK=~(BIT_SUB_TXD0);

    while(isTxInt);
    
    /*********** UART0 Rx test with interrupt ***********/
    isRxInt=1;
    uart0RxStr=(char *)UARTBUFFER;
    Uart_Printf("\n[Uart channel 0 Rx Interrupt Test]:\n");
    Uart_Printf("After typing characters and ENTER key  you will see the characters which was typed by you.");
    Uart_Printf("\nTo quit, press ENTER key.!!!\n");
	
    Uart_TxEmpty(1); //wait until tx buffer is empty.
    pISR_UART0 =(unsigned)Uart0_RxIntOrErr;
    rULCON0=(0<<6)|(0<<3)|(0<<2)|(3);	// Normal,No parity,One stop bit, 8bit
    rUCON0 |= (TX_INTTYPE<<9)|(RX_INTTYPE<<8)|(0<<7)|(1<<6)|(0<<5)|(0<<4)|(1<<2)|(1);
    //Clock,Tx:pulse,Rx:pulse,Rx timeout:x,Rx error int:o,Loop-back:x,Send break:x,Tx:int,Rx:int

    // Clear Int Pending and Unmask    
    ClearPending(BIT_UART0);
    rINTMSK=~(BIT_UART0);
    rSUBSRCPND=(BIT_SUB_TXD0|BIT_SUB_RXD0|BIT_SUB_ERR0);    
    rINTSUBMSK=~(BIT_SUB_RXD0|BIT_SUB_ERR0);
    
    while(isRxInt);
    
    rINTSUBMSK|=(BIT_SUB_RXD0|BIT_SUB_ERR0);
    rINTMSK|=(BIT_UART0);
 
    Uart_Printf("%s\n",(char *)UARTBUFFER);

     Uart_Printf("\nConnect PC[COM1 or COM2] and UART1 of SMDK2440 with a serial cable!!! \n");
    Uart_Printf("Then, press any key........\n");
   	
    Uart_Select(1);
    Uart_Getch();
	
    Uart_Port_Return();
}


void __irq Uart0_TxDmaDone(void)
{
    rDMASKTRIG0=0x0;	// Stop Dma0
    isDone=0;
    rINTMSK |= BIT_DMA0;
    ClearPending(BIT_DMA0);
}

void __irq Uart0_RxDmaOrErr(void)
{
    rINTSUBMSK|=(BIT_SUB_RXD0|BIT_SUB_TXD0|BIT_SUB_ERR0);
    if(rSUBSRCPND&BIT_SUB_ERR0)  __sub_Uart0_RxErrInt();
    
    ClearPending(BIT_UART0);
    rSUBSRCPND=(BIT_SUB_ERR0);	// Clear Sub int pending    
    rINTSUBMSK&=~(BIT_SUB_ERR0);    
}

void __irq Uart0_RxDmaDone(void)
{
    rDMASKTRIG0=0x0;	//DMA0 Channel Off
    isDone=0;
    *(uart0RxStr+5)='\0';
    rINTMSK|=(BIT_DMA0);
    ClearPending(BIT_DMA0);
}

void Test_Uart0_Dma(void)
{
    Uart_Port_Set(); 
    Uart_Select(1);    
    /*********** UART0 Tx test with DMA0 ***********/ 
    isDone=1;
    uart0TxStr="ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890->UART0 Tx Test by DMA0 is good!!!!\r\n";
    Uart_Printf("\n[Uart channel 0 DMA0 Tx Test]\n");
    Uart_TxEmpty(1);

    Uart_Printf("\nConnect PC[COM1 or COM2] and UART0 of SMDK2440 with a serial cable!!! \n");
    Uart_Printf("Then, press any key........\n");
   	
    Uart_Select(0);
    Uart_Getch();

   
    pISR_DMA0  =(unsigned)Uart0_TxDmaDone;

    rULCON0=(0<<6)|(0<<3)|(0<<2)|(3);	// Normal,No parity,One stop bit, 8bit
    rUCON0 = (TX_INTTYPE<<9)|(RX_INTTYPE<<8)|(0<<7)|(0<<6)|(0<<5)|(0<<4)|(2<<2)|(0);
    //Clock,Tx:Def,Rx:Def,Rx timeout:x,Rx error int:x,Loop-back:x,Send break:x,Tx:dma0,Rx:x

    /***DMA0 init***/
    rDISRC0=(U32)uart0TxStr;	// Start address
    rDISRCC0=(0<<1)|(0);		// AHB,Increment
    rDIDST0=(U32)UTXH0;			// Memory buffer Address
    rDIDSTC0=(1<<1)|(1);		// APB,Fixed
    rDCON0=((unsigned int)1<<31)|(0<<30)|(1<<29)|(0<<28)|(0<<27)|(1<<24)|(1<<23)|(1<<22)|(0<<20)|strlen((char*)uart0TxStr);
    //handshake, sync PCLK, TC int, single tx, single service, Uart0, H/W request,auto-reload off, Byte size Tx, Tx count value
    rINTMSK=~(BIT_DMA0);
    rDMASKTRIG0=(0<<2)|(1<<1)|(0);    //no-stop, DMA0 channel on, no-SW trigger 

    while(isDone);

    /*********** UART0 Rx test with DMA0 ***********/ 
    isDone=1;
    uart0RxStr=(char *)UARTBUFFER;
   
    Uart_Printf("\n[Uart channel 0 DMA0 Rx Test]\n");
    Uart_Printf("Type any five keys!!!\n");    
    Uart_Printf("Then you will see what you typed.\n");

    pISR_DMA0=(unsigned)Uart0_RxDmaDone;
    pISR_UART0=(unsigned)Uart0_RxDmaOrErr;

    
    rULCON0=(0<<6)|(0<<3)|(0<<2)|(3);	// Normal,No parity,One stop bit, 8bit
    rUCON0 &= 0x400;	// For the PCLK <-> UCLK fuction  
    rUCON0 |= (TX_INTTYPE<<9)|(RX_INTTYPE<<8)|(0<<7)|(1<<6)|(0<<5)|(0<<4)|(1<<2)|(2);
    //Clock,Tx:Def,Rx:Def,Rx timeout:x,Rx error int:o,Loop-back:x,Send break:x,Tx:int,Rx:dma0

    
    /***DMA0 init***/
    rDISRC0=(U32)URXH0;			// Start address
    rDISRCC0=(1<<1)|(1);		// APB,Fixed
    rDIDST0=(U32)uart0RxStr;	        // Memory buffer Address
    rDIDSTC0= (0<<1)|(0);		// AHB,Increment
    rDCON0=((unsigned int)1<<31)|(0<<30)|(1<<29)|(0<<28)|(0<<27)|(1<<24)|(1<<23)|(1<<22)|(0<<20)|(5);
    //handshake, sync PCLK, TC int, single tx, single service, Uart0, H/W request,auto-reload off, Byte size Tx, Tx count value

    // Clear Int Pending and Unmask    
    ClearPending(BIT_UART0);
    rINTMSK=~(BIT_DMA0|BIT_UART0);
    rSUBSRCPND=(BIT_SUB_TXD0|BIT_SUB_RXD0|BIT_SUB_ERR0);        
    rINTSUBMSK=~(BIT_SUB_ERR0);
    rDMASKTRIG0=(0<<2)|(1<<1)|(0);    //no-stop, DMA0 channel on, no-SW trigger 

    while(isDone);

    Uart_Printf("%s\n",uart0RxStr);

     Uart_Printf("\nConnect PC[COM1 or COM2] and UART1 of SMDK2440 with a serial cable!!! \n");
    Uart_Printf("Then, press any key........\n");
   	
    Uart_Select(1);
    Uart_Getch();
	
    Uart_Port_Return();
}

volatile U32 *fifo_cnt; //temporary for fifo count test
volatile U32 fcnt = 0;

void __irq Uart0_TxFifo(void)
{
    rINTSUBMSK|=(BIT_SUB_RXD0|BIT_SUB_TXD0|BIT_SUB_ERR0);	// Just for the safety
	rINTMSK|=BIT_UART0;
	rSUBSRCPND=BIT_SUB_TXD0;	// Clear Sub int pending
	ClearPending(BIT_UART0);	// Clear master pending

	*fifo_cnt++ = ++fcnt;
	*fifo_cnt++ = rUFCON0;
	*fifo_cnt++ = (rUFSTAT0>>8)&0x3f;
	*fifo_cnt = 0;

    while (!(rUFSTAT0 & 0x4000) && (*uart0TxStr != '\0')) 	//until tx fifo full or end of string
    	WrUTXH0(*uart0TxStr++);	

    if(*uart0TxStr != '\0') 
    {
        rINTSUBMSK&=~(BIT_SUB_TXD0);	// Unmask sub int
        rINTMSK&=~(BIT_UART0);
    }
}

void __irq Uart0_RxFifoOrErr(void)
{
    rINTSUBMSK|=(BIT_SUB_RXD0|BIT_SUB_TXD0|BIT_SUB_ERR0);
    if(rSUBSRCPND&BIT_SUB_RXD0) __sub_Uart0_RxFifo();
    else __sub_Uart0_RxErrInt();
    ClearPending(BIT_UART0);
    rSUBSRCPND=(BIT_SUB_RXD0|BIT_SUB_ERR0);	// Clear Sub int pending    
    rINTSUBMSK&=~(BIT_SUB_RXD0|BIT_SUB_ERR0);    
}

void __sub_Uart0_RxFifo(void)
{
    while(rUFSTAT0&0x7f)	//During the Rx FIFO is not empty
    {
	rx_point++;
	if(rx_point<5)
		rx_filesize |= (RdURXH0()<<(8*(rx_point-1))); // First 4-bytes mean file size
	else if(rx_point>(rx_filesize-2))	
	{
		rx_dncs |= (RdURXH0()<<(8*(1-(rx_filesize-rx_point)))); //Last 2-bytes mean checksum.
		if(rx_point==rx_filesize) rx_isdone=0;
	}
	else
		rx_checksum+=RdURXH0();
    }
}

void Test_Uart0_Fifo(void)
{
//	int i;
	
    Uart_Port_Set(); 
    Uart_Select(1);
    /******* UART0 Tx FIFO test with interrupt ********/     
    Uart_Printf("[Uart channel 0 Tx FIFO Interrupt Test]\n");
    Uart_TxEmpty(1);	//wait until tx buffer is empty.

    Uart_Printf("\nConnect PC[COM1 or COM2] and UART0 of SMDK2440 with a serial cable!!! \n");
    Uart_Printf("Then, press any key........\n");
   	
    Uart_Select(0);
    Uart_Getch();

	fifo_cnt = (U32 *)_NONCACHE_STARTADDRESS; // temporary buffer
		
    /* <Tx Trigger Level:empty> */    
    uart0TxStr="ABCDEFGHIJKLMNOPQRSTUVWXYZ->UART0 Tx FIFO interrupt(TL 48byte) test is good!!!!\r\n";
    pISR_UART0=(U32)Uart0_TxFifo;
    rULCON0=(0<<6)|(0<<3)|(0<<2)|(3);	// Normal,No parity,One stop bit, 8bit
    rUCON0 &= 0x400;	// For the PCLK <-> UCLK fuction    
    rUCON0 |= (TX_INTTYPE<<9)|(RX_INTTYPE<<8)|(0<<7)|(0<<6)|(0<<5)|(0<<4)|(1<<2)|(0);
    //Clock,Tx:Def,Rx:Def,Rx timeout:x,Rx error int:x,Loop-back:x,Send break:x,Tx:int,Rx:x
    rUFCON0=(3<<6)|(2<<4)|(1<<2)|(1<<1)|(1);
    //Tx and Rx FIFO Trigger Level:8byte,Tx and Rx FIFO Reset,FIFO on
    rINTMSK=~(BIT_UART0);
    rINTSUBMSK=~(BIT_SUB_TXD0);
    Delay(500);
	rUFCON0=(3<<6)|(2<<4)|(1<<2)|(1<<1)|(0);

// edited by junon
	/* <Tx Trigger Level:16Byte> */ 	 
	rUFCON0=(2<<6)|(2<<4)|(1<<2)|(1<<1)|(1);
	uart0TxStr="ABCDEFGHIJKLMNOPQRSTUVWXYZ->UART0 Tx FIFO interrupt(TL 32byte) test is good!!!!\r\n";
	rINTMSK=~(BIT_UART0);
	rINTSUBMSK=~(BIT_SUB_RXD0|BIT_SUB_TXD0|BIT_SUB_ERR0);
	Delay(500);

	/* <Tx Trigger Level:32Byte> */ 	 
	rUFCON0=(1<<6)|(2<<4)|(1<<2)|(1<<1)|(1);
	uart0TxStr="ABCDEFGHIJKLMNOPQRSTUVWXYZ->UART0 Tx FIFO interrupt(TL 16byte) test is good!!!!\r\n";
	rINTMSK=~(BIT_UART0);
	rINTSUBMSK=~(BIT_SUB_RXD0|BIT_SUB_TXD0|BIT_SUB_ERR0);
	Delay(500);

	/* <Tx Trigger Level:48Byte> */ 	 
	rUFCON0=(0<<6)|(2<<4)|(1<<2)|(1<<1)|(1);
	uart0TxStr="ABCDEFGHIJKLMNOPQRSTUVWXYZ->UART0 Tx FIFO interrupt(TL 0byte) test is good!!!!\r\n";
	rINTMSK=~(BIT_UART0);
	rINTSUBMSK=~(BIT_SUB_RXD0|BIT_SUB_TXD0|BIT_SUB_ERR0);
	Delay(500);
	rUFCON0=(0<<6)|(2<<4)|(1<<2)|(1<<1)|(0);
	//Tx and Rx FIFO Trigger Level:48byte,Tx and Rx FIFO Reset,FIFO off

	Uart_Printf("Saved FIFO current count in ISR!! Interrupt count : %d\n",fcnt);
	fifo_cnt = (U32 *)_NONCACHE_STARTADDRESS;
	while(*fifo_cnt)
		Uart_Printf("[0x%x,%d,0x%x,%d] ", fifo_cnt, *fifo_cnt++, *fifo_cnt++,*fifo_cnt++);
	fcnt = 0;
	
    /******* UART0 Rx FIFO test with interrupt ********/     
    rx_dncs=0;
    rx_point=0;
    rx_isdone=1;
    rx_filesize=0;
    rx_checksum=0;
    Uart_Printf("[Uart channel 0 Rx FIFO Interrupt Test]\n");
//----------------------------------------------------
    pISR_UART0=(unsigned)Uart0_RxFifoOrErr;

    rULCON0=(0<<6)|(0<<3)|(0<<2)|(3);	// Normal,No parity,One stop bit, 8bit
    rUCON0 &= 0x400;	// For the PCLK <-> UCLK fuction
    rUCON0 |= (TX_INTTYPE<<9)|(RX_INTTYPE<<8)|(1<<7)|(1<<6)|(0<<5)|(0<<4)|(1<<2)|(1);
    //Clock,Tx:Def,Rx:Def,Rx timeout:o,Rx error int:o,Loop-back:x,Send break:x,Tx:int,Rx:int
    rUFCON0=(1<<6)|(2<<4)|(1<<2)|(1<<1)|(1);
    //Tx and Rx FIFO Trigger Level:4byte,Tx and Rx FIFO Reset,FIFO on

    // Clear Int Pending and Unmask 
    ClearPending(BIT_UART0);
    rINTMSK=~(BIT_UART0);
    rSUBSRCPND=(BIT_SUB_RXD0|BIT_SUB_TXD0|BIT_SUB_ERR0);
    rINTSUBMSK=~(BIT_SUB_RXD0|BIT_SUB_ERR0);

    Uart_Printf("Download the target file[*.bin] by Uart0\n");
	
    while(rx_isdone);

    rINTMSK |= (BIT_UART0);	
    rINTSUBMSK|=(BIT_SUB_RXD0|BIT_SUB_TXD0|BIT_SUB_ERR0);

    rUFCON0=(3<<6)|(2<<4)|(1<<2)|(1<<1)|(0);
    //Tx and Rx FIFO Trigger Level:12byte,Tx and Rx FIFO Reset,FIFO off
			
    if(rx_dncs==(rx_checksum&0xffff)) 
	Uart_Printf("\nDownload test OK!!!\n");
    else 
	Uart_Printf("\nError!!!\n");

    Uart_Printf("\nConnect PC[COM1 or COM2] and UART1 of SMDK2440 with a serial cable!!! \n");
    Uart_Printf("Then, press any key........\n");
   	
    Uart_Select(1);
    Uart_Getch();
	
    Uart_Port_Return();	
}



void __irq Uart0_AfcTx(void)
{
    rINTSUBMSK|=(BIT_SUB_RXD0|BIT_SUB_TXD0|BIT_SUB_ERR0);

    if(tx_cnt<AFC_BUFLEN)
    {
    	Uart_Printf("%d,",*txdataPt);
    	WrUTXH0(*txdataPt++);
		tx_cnt++;
        ClearPending(BIT_UART0);
        rSUBSRCPND=(BIT_SUB_TXD0);
        rINTSUBMSK&=~(BIT_SUB_TXD0);
    }
    else
    {
  	 	tx_end=1;
        while(rUFSTAT0 & 0x7f00);	//Until FIFO is empty
        ClearPending(BIT_UART0);
        rSUBSRCPND=(BIT_SUB_TXD0);
    	rINTMSK|=BIT_UART0;
    }
}

void __irq Uart0_AfcRxOrErr(void)
{
    rINTSUBMSK|=(BIT_SUB_RXD0|BIT_SUB_TXD0|BIT_SUB_ERR0);
    if(rSUBSRCPND&BIT_SUB_RXD0) __sub_Uart0_RxAfc();    
    else __sub_Uart0_RxErrInt();

    ClearPending(BIT_UART0);
    rSUBSRCPND=(BIT_SUB_RXD0|BIT_SUB_TXD0|BIT_SUB_ERR0);
    rINTSUBMSK&=~(BIT_SUB_RXD0|BIT_SUB_TXD0|BIT_SUB_ERR0);
}

void __sub_Uart0_RxAfc(void)
{
    while( rUFSTAT0 & 0x7f )
    {
	*rxdataPt=rURXH0;
	Uart_Printf("%d,",*rxdataPt++);
	rx_cnt++;
    }
    if(rx_cnt == AFC_BUFLEN) 
    {
   	rx_end=1;
    	rINTMSK|=BIT_UART0;
    }
}
 
void Test_Uart0_AfcTx(void)
{
    int i;
    tx_cnt=0;
    tx_end=0;
    txdataFl=(volatile U8 *)UARTBUFFER;
    txdataPt=(volatile U8 *)UARTBUFFER;
    for(i=0;i<AFC_BUFLEN;i++) *txdataFl++=i;	// Initialize the AFC data
    Uart_Port_Set(); 
    Uart_Select(1);
    Uart_Printf("[Uart channel 0 AFC Tx Test]\n");
    Uart_Printf("This test should be configured two boards.\n");
    Uart_Printf("Connect Tx and Rx Board with twitsted(rx/tx, nCTS/nRTS) cable .\n");
     
   
    pISR_UART0=(unsigned) Uart0_AfcTx;

    rULCON0=(0<<6)|(0<<3)|(0<<2)|(3);	// Normal,No parity,One stop bit, 8bit
    rUCON0 &= 0x400;	// For the PCLK <-> UCLK fuction    
    rUCON0 |= (1<<9)|(1<<8)|(0<<7)|(0<<6)|(0<<5)|(0<<4)|(1<<2)|(1);
    //Clock,Tx:Lev,Rx:Lev,Rx timeout:x,Rx error int:x,Loop-back:x,Send break:x,Tx:int,Rx:int
    rUFCON0=(1<<6)|(0<<4)|(1<<2)|(1<<1)|(1);
    //Tx and Rx FIFO Trigger Level:4byte,Tx and Rx FIFO Reset,FIFO on
    rUMCON0=0x10;   // Enable Uart0 AFC 

    Uart_Printf("\nKeep the connection between PC[COM1 or COM2] and UART1 of SMDK2410!!! \n");
    Uart_Printf("Press any key to start Rx and then Star Tx....\n");
    Uart_TxEmpty(1);
    Uart_Getch();
  
    // Clear Int Pending and Unmask 
    rINTMSK=~(BIT_UART0);
    rINTSUBMSK=~(BIT_SUB_TXD0);
	
     while(!tx_end);

     rINTMSK|=(BIT_UART0);
    rINTSUBMSK|=(BIT_SUB_RXD0|BIT_SUB_TXD0|BIT_SUB_ERR0);
    rUFCON0=(3<<6)|(2<<4)|(1<<2)|(1<<1)|(0);
    //Tx and Rx FIFO Trigger Level:12byte,Tx and Rx FIFO Reset,FIFO off
    Uart_Printf("\nEnd Tx, transfer data count=%d\n",tx_cnt);
	

    Uart_Port_Return();
}

void Test_Uart0_AfcRx(void)
{
    unsigned int i;
    rx_cnt=0;
    rx_end=0;
    afc_err=0;
    rxdataCk=(volatile U8 *)UARTBUFFER;
    rxdataPt=(volatile U8 *)UARTBUFFER;
    Uart_Port_Set(); 
    Uart_Select(1);
    Uart_Printf("[Uart channel 0 AFC Rx Test]\n");
    Uart_Printf("This test should be configured two boards.\n");
    Uart_Printf("Connect Tx and Rx Board with twitsted(rx/tx, nCTS/nRTS) cable .\n");
    
    pISR_UART0=(unsigned) Uart0_AfcRxOrErr;

    rULCON0=(0<<6)|(0<<3)|(0<<2)|(3);	// Normal,No parity,One stop bit, 8bit
    rUCON0 &= 0x400;	// For the PCLK <-> UCLK fuction    
    rUCON0 |= (1<<9)|(1<<8)|(1<<7)|(1<<6)|(0<<5)|(0<<4)|(1<<2)|(1);
    //Clock,Tx:Lev,Rx:Lev,Rx timeout:o,Rx error int:o,Loop-back:x,Send break:x,Tx:o,Rx:o
    
    rUFCON0=(1<<6)|(0<<4)|(1<<2)|(1<<1)|(1);
    //Tx and Rx FIFO Trigger Level:4byte,Tx and Rx FIFO Reset,FIFO on
    rUMCON0=0x10;   // Enable Uart0 AFC 

    
    Uart_Printf("\nKeep the connection between PC[COM1 or COM2] and UART0 of SMDK2410!!! \n");
    Uart_Printf("Press any key to start Rx and then Star Tx....\n");
    Uart_Getch();

 
	   // Clear Int Pending and Unmask 
     rSUBSRCPND=(BIT_SUB_RXD0|BIT_SUB_TXD0|BIT_SUB_ERR0);
     rINTSUBMSK=~(BIT_SUB_RXD0|BIT_SUB_ERR0);
     ClearPending(BIT_UART0);
     rINTMSK=~(BIT_UART0);

    while(!rx_end);

  //  rINTMSK|=BIT_UART0;
    rINTSUBMSK|=(BIT_SUB_RXD0|BIT_SUB_TXD0|BIT_SUB_ERR0);
    rUFCON0=(3<<6)|(2<<4)|(1<<2)|(1<<1)|(0);
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

    Uart_Port_Return();
}


// added by junon start
void __irq Uart0_RxOverrunErr(void)
{
    rINTSUBMSK|=(BIT_SUB_RXD0|BIT_SUB_TXD0|BIT_SUB_ERR0);
    if(rSUBSRCPND&BIT_SUB_ERR0) 
	{
		__sub_Uart0_RxErrInt();
    	ClearPending(BIT_UART0); 
	    rSUBSRCPND=(BIT_SUB_RXD0|BIT_SUB_ERR0);	// Clear Sub int pending    
		return;
    }
    rINTSUBMSK&=~(BIT_SUB_RXD0|BIT_SUB_ERR0);    
}


void Test_Uart0_RxErr(void) // need two serial port cables.
{
//	U8 ch;
	U8 cError;
	
    Uart_Port_Set(); 
   
    Uart_Select(1);
    Uart_Printf("\nConnect PC[COM1 or COM2] and UART0 of SMDK2440 with a serial cable!!! \n");
    Uart_Printf("In this case, Uart0 : test port,  Uart1 : debug port\n");
    Uart_Printf("Then, press any key........\n");
//    Uart_Select(0);	// Change the uart port    
    Uart_Getch();
			
  	while(1)
	{
	    /*********** UART0 Rx test with interrupt ***********/
	    isRxInt=1;
	    uart0RxStr=(char *)UARTBUFFER;			
	    Uart_Printf("\n[Uart channel 0 Rx Error Check]\n");
	    Uart_TxEmpty(1); //wait until tx buffer is empty.

		rUFCON0=(1<<6)|(0<<4)|(1<<2)|(1<<1)|0; // FIFO disable

		// for 2440A. add Frame error, Parity error, Break detect check.
	    Uart_Printf("\n1. Overrun Error check[D]   2. Frame error   3. Parity error  \n"); 
		cError = Uart_Getch();
	    if (cError== '2')
		{
			pISR_UART0 =(unsigned)Uart0_RxIntOrErr;

			rULCON0=(0<<6)|(4<<3)|(0<<2)|(0); // Normal,No parity,One stop bit, 7bit
			rUCON0 = (TX_INTTYPE<<9)|(RX_INTTYPE<<8)|(0<<7)|(1<<6)|(0<<5)|(0<<4)|(1<<2)|(1);
			//Clock,Tx:pulse,Rx:pulse,Rx timeout:x,Rx error int:o,Loop-back:x,Send break:x,Tx:int,Rx:int
			Uart_Printf("This port was set 7 data bit, no parity, 1 stop bit. Send just characters..\n");
	    }
		else if (cError== '3')
		{
			pISR_UART0 =(unsigned)Uart0_RxIntOrErr;
		
			rULCON0=(0<<6)|(5<<3)|(0<<2)|(3); // Normal,Even parity,One stop bit, 8bit
			rUCON0 = (TX_INTTYPE<<9)|(RX_INTTYPE<<8)|(0<<7)|(1<<6)|(0<<5)|(0<<4)|(1<<2)|(1);
			//Clock,Tx:pulse,Rx:pulse,Rx timeout:x,Rx error int:o,Loop-back:x,Send break:x,Tx:int,Rx:int
			Uart_Printf("This port was set 8 data bit, even parity, 1 stop bit. Send just characters..\n");
		}
	    else 
		{
			pISR_UART0 = (unsigned)Uart0_RxOverrunErr;

			rULCON0=(0<<6)|(0<<3)|(0<<2)|(3); // Normal,No parity,One stop bit, 8bit
		    rUCON0 = (TX_INTTYPE<<9)|(RX_INTTYPE<<8)|(0<<7)|(1<<6)|(0<<5)|(0<<4)|(1<<2)|(1);
		    //Clock,Tx:pulse,Rx:pulse,Rx timeout:x,Rx error int:o,Loop-back:x,Send break:x,Tx:int,Rx:int

			Uart_Printf("1. Using FIFO	2. Not using FIFO[D] \n");
			if (Uart_Getch() == '1') 
			{
				rUFCON0 |= 1;
				Uart_Printf("Press Any key as 65 times in UART0 terminal window..\n");
				Uart_Printf("then Overrun error will be occured.. \n"); 
			}
			else
			{
				rUFCON0 &= ~1;
				Uart_Printf("Press Any key as 2 times in UART0 terminal window..\n");
				Uart_Printf("then Overrun error will be occured.. \n");
			}
		}
		
	    // Clear Int Pending and Unmask    
	    rSUBSRCPND=(BIT_SUB_TXD0|BIT_SUB_RXD0|BIT_SUB_ERR0);    
	    ClearPending(BIT_UART0);
	    rINTSUBMSK=~(BIT_SUB_RXD0|BIT_SUB_ERR0);
	    rINTMSK=~(BIT_UART0);

	    while(isRxInt);

		// UART0 mask
    	rINTSUBMSK|=(BIT_SUB_TXD0|BIT_SUB_RXD0|BIT_SUB_ERR0);
		rINTMSK|=(BIT_UART0);

		rUFCON0 |= 3<<1; // fifo reset
		Uart_Printf("1. One more  2. Exit[D] \n");
		if (Uart_Getch() == '1') continue;
		else break;
	}	

    Uart_Port_Return();	
}

#if 0
void Enter_Uart0_ISR(void)
{
	rINTSUBMSK|=(BIT_SUB_RXD0|BIT_SUB_TXD0|BIT_SUB_ERR0);
	rINTMSK |= BIT_UART0;
	ClearPending(BIT_UART0);
	rSUBSRCPND=(BIT_SUB_RXD0|BIT_SUB_TXD0|BIT_SUB_ERR0);	
}

void Exit_Uart0_ISR(void)
{
	rINTSUBMSK&=~(BIT_SUB_RXD0|BIT_SUB_TXD0|BIT_SUB_ERR0);
	rINTMSK &= ~(BIT_UART0);
}

void __irq Uart0_Cts(void)
{
	Enter_Uart0_ISR();

	Exit_Uart0_ISR();	
}

void Test_Rts_Rx(void)
{
    int i = 1;

    Uart_Port_Set(); 
    //U32 safc_rGPHCON,safc_rGPHDAT,safc_rGPHUP;
    tx_cnt=0;
    Uart_Select(1);
    Uart_Printf("[Uart channel 0 CTS Rx Test]\n");
    Uart_Printf("Check nCTS0 signal with oscilloscope! \n");
	Uart_Printf("When FIFO nuber is 32, Active nRTS0... \n");
    
//    pISR_UART0=(unsigned) Uart0_Cts;

    rULCON0=(0<<6)|(0<<3)|(0<<2)|(3);	// Normal,No parity,One stop bit, 8bit
    rUCON0 |= (1<<9)|(1<<8)|(0<<7)|(0<<6)|(0<<5)|(0<<4)|(1<<2)|(1);
    //Clock,Tx:Lev,Rx:Lev,Rx timeout:x,Rx error int:x,Loop-back:x,Send break:x,Tx:int,Rx:int
    rUFCON0=(3<<6)|(3<<4)|(1<<2)|(1<<1)|(1);
    //Tx and Rx FIFO Trigger Level:32byte,Tx and Rx FIFO Reset,FIFO on
    rUMCON0=0x10;   // Enable Uart0 AFC 

//	rINTMSK&=~(BIT_UART0);
//	rINTSUBMSK&=~(BIT_SUB_RXD0);
	rINTMSK |= (BIT_UART0); 
	rINTSUBMSK|=(BIT_SUB_RXD0|BIT_SUB_TXD0|BIT_SUB_ERR0);

	while( Uart_Getch() != '\r');  // just press any key at 32 times..

	Uart_Printf("\nEnd Tx, transfer data count=%d\n",tx_cnt);

	Uart_Select(0);
    Uart_Port_Return();
}
#endif

void Test_Uart0_Temp(void)
{
	U8 ch;
	int iBaud;
	volatile char *cStartBuf;
	
    Uart_Port_Set(); 

    Uart_Select(1);    

	/******** For operating clock test *********/
	Uart_Printf("[Uart channel 0 Interrupt test]\n");
	Uart_Printf("Select operating clock 1. PCLK    2. UCLK     3. FCLK/n(D)  \nChoose : ");
	ch = Uart_Getch();	
	switch (ch)
	{
	case '1' :
		Uart_Pclk_En(0, 115200);
		break;
	case '2' :
		rMISCCR = rMISCCR & ~(7<<8) | (1<<10); // CLKOUT1 = PCLK
		Uart_Uextclk_En(0, 115200, Pclk);
		break;
	default :
		Uart_Printf("Type the baudrate and then change the same baudrate of host, too.\n");
		Uart_Printf("Baudrate (ex 9600, 115200, 921600[D]) : ");
		iBaud = Uart_GetIntNum();
		if (iBaud == -1) iBaud = 921600;
		Uart_Fclkn_En(0, iBaud);
		Uart_Getch();
		break;
	}

	/*********** UART0 Tx test with interrupt ***********/  
    isTxInt=1;
    uart0TxStr="ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890->UART0 Tx interrupt test is good!!!!\r\n";
	cStartBuf = uart0TxStr;
    Uart_Printf("[Uart channel 0 Tx Interrupt Test]\n");

    pISR_UART0=(unsigned)Uart0_TxInt;

    rULCON0=(0<<6)|(0<<3)|(0<<2)|(3);	// Normal,No parity,One stop bit, 8bit
    rUCON0 |= (TX_INTTYPE<<9)|(RX_INTTYPE<<8)|(0<<7)|(0<<6)|(0<<5)|(0<<4)|(1<<2)|(1);
    //Clock,Tx:Def,Rx:Def,Rx timeout:x,Rx error int:x,Loop-back:x,Send break:x,Tx:int,Rx:int
	Uart_Printf("BRDIV0 = 0x%x\n", rUBRDIV0);
	Uart_Printf("UCON0 = 0x%x\n", rUCON0);
	Uart_Printf("UCON1 = 0x%x\n", rUCON1);
	Uart_Printf("Press enter key to exit \n");

    Uart_TxEmpty(0); //wait until tx buffer is empty.
    rINTMSK&=~(BIT_UART0);
    rINTSUBMSK&=~(BIT_SUB_TXD0);

	
    while(1)
	{
		if (isTxInt==0)
		{
			uart0TxStr = cStartBuf;
			rINTMSK&=~(BIT_UART0);
			rINTSUBMSK&=~(BIT_SUB_TXD0|BIT_SUB_RXD0|BIT_SUB_ERR0);
			isTxInt=1;
		}
		if (Uart_GetKey()=='\r') break;		
   	}
    
    rINTSUBMSK|=(BIT_SUB_RXD0|BIT_SUB_ERR0);
    rINTMSK|=(BIT_UART0);
 
    Uart_Port_Return();
}

// junon end

//---------------------------------------UART0 test function-------------------------------------
