#include <string.h>
#include <stdlib.h>
#include "2440addr.h"
#include "2440lib.h"
#include "def.h"
#include "irda.h"

#define IrDA_BUFLEN 0x100

void IrDA_Port_Set(void);
void IrDA_Port_Return(void);

void __irq IrDA_TxInt(void);
void __irq IrDA_RxOrErr(void);
void __sub_IrDA_RxInt(void);
void __sub_IrDA_RxErrInt(void);

volatile U8 *IrDAdataPt;
volatile U8 *IrDAdataFl;
volatile U32 IrDA_cnt,IrDA_end,IrDA_err,IrDA_BAUD;
volatile U32 sirda_rGPBCON,sirda_rGPBDAT,sirda_rGPBUP;
volatile U32 sirda_rGPHCON,sirda_rGPHDAT,sirda_rGPHUP;
volatile U32 sirda_ULCON2,sirda_UCON2,sirda_UFCON2,sirda_UMCON2,sirda_UBRDIV2;


void * func_irda_test[][2]=
{	
//									    "0123456789012345" max 15자 로한정하여 comment하세요.
//IrDA
	(void *)Test_IrDA_Rx, 				"UART2 IrDA Rx  ",
	(void *)Test_IrDA_Tx, 				"UART2 IrDA Tx  ",
	0,0
};

void IrDA_Test(void)
{
	int i;
	
	Uart_Printf("\n======  IrDA Test program start ======\n");
		
	while(1)
	{
		i=0;
		Uart_Printf("\n\n");
		Uart_Printf("1: UART2 IrDA Rx\n");
		Uart_Printf("2: UART2 IrDA Tx\n");
		

		Uart_Printf("\nPress Enter key to exit : ");
		i = Uart_GetIntNum();
		if(i==-1) break;		// return.
//		if(i>=0 && (i<((sizeof(func_irda_test)-1)/8)) )	// select and execute...
//			( (void (*)(void)) (func_irda_test[i][0]) )();
	    switch (i)
		{
		   case 1:	Test_IrDA_Rx();
		            break;
		   case 2:	Test_IrDA_Tx();
		            break;
		   default:
		            break;
		}
	}
	
	Uart_Printf("\n====== IrDA Test program end ======\n");
}




void IrDA_Port_Set(void)
{
    //Push UART GPIO port configuration
    sirda_rGPBCON=rGPBCON; 
    sirda_rGPBDAT=rGPBDAT;
    sirda_rGPBUP=rGPBUP;
    sirda_rGPHCON=rGPHCON; 
    sirda_rGPHDAT=rGPHDAT;
    sirda_rGPHUP=rGPHUP;
    //Configure IrDA port
    rGPBDAT|=(1<<1);	
    rGPBCON&=0x3ffff3;
    rGPBCON|=(1<<2);	// Output(nIrDATXDEN)
    rGPBUP|=(1<<1);	//Uart port pull-up disable
    rGPHCON&=0x3c0fff;
    rGPHCON|=0x2a000;	// TXD2,RXD2
    rGPHUP|=0x1c0;	//Uart port pull-up disable
    //Push Uart control registers 
    sirda_ULCON2=rULCON2;		
    sirda_UCON2 =rUCON2;		
    sirda_UFCON2=rUFCON2;		
    sirda_UMCON2=rUMCON2;
    sirda_UBRDIV2=rUBRDIV2;
}       
        
void IrDA_Port_Return(void)
{       
    //Pop UART GPIO port configuration
    rGPBCON=sirda_rGPBCON;
    rGPBDAT=sirda_rGPBDAT;
    rGPBUP =sirda_rGPBUP;
    rGPHCON=sirda_rGPHCON;
    rGPHDAT=sirda_rGPHDAT;
    rGPHUP =sirda_rGPHUP;
    //Pop Uart control registers 
    rULCON2=sirda_ULCON2;		
    rUCON2 =sirda_UCON2;	
    rUFCON2=sirda_UFCON2;		
    rUMCON2=sirda_UMCON2;
    rUBRDIV2=sirda_UBRDIV2;
}

void __irq IrDA_TxInt(void)
{
    rINTSUBMSK|=(BIT_SUB_RXD2|BIT_SUB_TXD2|BIT_SUB_ERR2);
    if(IrDA_cnt < (IrDA_BUFLEN))
    {
	Uart_Printf("%d,",*IrDAdataPt);
	WrUTXH2(*IrDAdataPt++);
	IrDA_cnt++;
	ClearPending(BIT_UART2);
        rSUBSRCPND=(BIT_SUB_TXD2);
        rINTSUBMSK&=~(BIT_SUB_TXD2);
    }
    else
    {
	IrDA_end=1;
	while(rUFSTAT2 & 0x2f0);	//Until FIFO is empty
	while(!(rUTRSTAT2 & 0x4));	//Until Tx shifter is empty
        ClearPending(BIT_UART2);
	rINTMSK|=BIT_UART2;
    }
}

void __irq IrDA_RxOrErr(void)
{
    rINTSUBMSK|=(BIT_SUB_RXD2|BIT_SUB_TXD2|BIT_SUB_ERR2);	// Just for the safety
    if(rSUBSRCPND&BIT_SUB_RXD2) __sub_IrDA_RxInt();
    else __sub_IrDA_RxErrInt();
    rSUBSRCPND=(BIT_SUB_RXD2|BIT_SUB_TXD2|BIT_SUB_ERR2);
    rINTSUBMSK&=~(BIT_SUB_RXD2|BIT_SUB_ERR2);
    ClearPending(BIT_UART2);
}

void __sub_IrDA_RxInt(void)
{
    while( (rUFSTAT2 & 0x100) || (rUFSTAT2 & 0xf)  )
    {
	*IrDAdataPt=rURXH2;
	*IrDAdataPt++;
	IrDA_cnt++;
    }
    if(IrDA_cnt >= IrDA_BUFLEN)
    {
    	IrDA_end=1;
    	rINTMSK|=BIT_UART2;
    }
}

void __sub_IrDA_RxErrInt(void)
{
    switch(rUERSTAT2)//to clear and check the status of register bits
    {
	case '1':
	    Uart_Printf("Overrun error\n");
	    break;
	case '2':
	    Uart_Printf("Parity error\n");
	    break;
	case '4':
	    Uart_Printf("Frame error\n");
	    break;
	case '8':
	    Uart_Printf("Breake detect\n");
	    break;
	default :
	    break;
    }
}

void Test_IrDA_Tx(void)
{
    int i;
    IrDA_cnt=0;
    IrDA_end=0;
    IrDAdataFl=(volatile U8 *)IrDABUFFER;
    IrDAdataPt=(volatile U8 *)IrDABUFFER;
    IrDA_Port_Set();
    Uart_Select(1);
    for(i=0;i<IrDA_BUFLEN;i++) *IrDAdataFl++=i;	// Initialize IrDA Tx data
    pISR_UART2=(U32)IrDA_TxInt;
    Uart_Printf("\nSelect the baud rate\n");	// Select IrDA baud rate
    Uart_Printf("1)9600    2)19200    3)38400    4)57600    5)115200\n"); 
    i=Uart_Getch ();
    switch(i)
    {
	case '1':
	IrDA_BAUD=9600;
        break;
	
	case '2':
        IrDA_BAUD=19200;
        break;
	
	case '3':
        IrDA_BAUD=38400;
        break;
	
	case '4':
        IrDA_BAUD=57600;
        break;
	
	case '5':
        IrDA_BAUD=115200;
        break;
	
	default:
        break;
    }
    rUBRDIV2=( (int)(PCLK/16./IrDA_BAUD) -1 );
    Uart_Printf("rUBRDIV2=%d\n", rUBRDIV2);
    Uart_Printf("[UART IrDA Tx Test]\n");
    Uart_Printf("This test should be configured two boards.\n");
    Uart_Printf("Start Rx first and press any key and...\n");
    Uart_TxEmpty(1);
    rGPBDAT &= ~(1<<1);	// Enable nIrDATXDEN - GPB1
    rUFCON2=(1<<6)|(0<<4)|(1<<2)|(1<<1)|(1);
    //Tx and Rx FIFO Trigger Level:4byte,Tx and Rx FIFO Reset,FIFO on
    rUCON2=(0<<10)|(1<<9)|(1<<8)|(0<<7)|(0<<6)|(0<<5)|(0<<4)|(0<<2)|(0); // From H/W    
    rULCON2=(1<<6)|(0<<3)|(0<<2)|(3);	// IrDA,No parity,One stop bit, 8bit
    Uart_Getch();
    rUCON2 |= (1<<2); // Tx enable
    Uart_Printf("Now... Tx with IrDA\n");
    rINTMSK=~(BIT_UART2);
    rINTSUBMSK=~(BIT_SUB_RXD2|BIT_SUB_TXD2|BIT_SUB_ERR2);

    while(!IrDA_end);

    rINTSUBMSK|=(BIT_SUB_RXD2|BIT_SUB_TXD2|BIT_SUB_ERR2);
    rUFCON2=(3<<6)|(2<<4)|(1<<2)|(1<<1)|(0);
    rGPBDAT|=(1<<1);	// Disable nIrDATXDEN        
    Uart_Printf("\nEnd Tx, transfer data count=%d\n",IrDA_cnt);
    IrDA_Port_Return();
}

void Test_IrDA_Rx(void)
{
    unsigned int i;
    IrDA_cnt=0;
    IrDA_end=0;
    IrDA_err=0;
		
    IrDAdataPt =(volatile U8 *)IrDABUFFER; // 0 initialize
    for(i=0;i<IrDA_BUFLEN;i++) *IrDAdataPt++=0;	// Initialize IrDA Tx data
    IrDAdataPt =(volatile U8 *)IrDABUFFER;
		
    IrDA_Port_Set();    
    Uart_Select(0);
    pISR_UART2=(unsigned)IrDA_RxOrErr;
    Uart_Printf("\nSelect the baud rate\n");	// Select IrDA baud rate
    Uart_Printf("1)9600    2)19200    3)38400    4)57600    5)115200\n"); 
    i=Uart_Getch();
    switch(i)
    {
	case '1':
            IrDA_BAUD=9600;
        break;
	
	case '2':
            IrDA_BAUD=19200;
        break;
	
	case '3':
            IrDA_BAUD=38400;
        break;
	
	case '4':
            IrDA_BAUD=57600;
        break;
	
	case '5':
            IrDA_BAUD=115200;
        break;
	
	default:
	break;
    }
    rUBRDIV2=( (int)(PCLK/16./IrDA_BAUD) -1 );
    Uart_Printf("rUBRDIV2=%d\n", rUBRDIV2);
    Uart_Printf("[UART IrDA Rx Test]\n");
    Uart_Printf("This test should be configured two boards.\n");
    Uart_Printf("Press any key to start Rx and then Start Tx....\n");
    rUFCON2=(1<<6)|(0<<4)|(1<<2)|(1<<1)|(1);
    //Tx and Rx FIFO Trigger Level:4byte,Tx and Rx Reset,FIFO En
    rUCON2=(0<<10)|(1<<9)|(1<<8)|(0<<7)|(1<<6)|(0<<5)|(0<<4)|(0<<2)|(0); // From H/W
    rULCON2=(1<<6)|(0<<3)|(0<<2)|(3);	// IrDA,No parity,One stop bit, 8bit
    Uart_Getch();
    Uart_Printf("Now... Rx with IrDA\n");
    rUCON2 |= 1; // Rx enable
    rINTMSK=~(BIT_UART2);
    rINTSUBMSK=~(BIT_SUB_RXD2|BIT_SUB_ERR2);

    while(!IrDA_end) if(Uart_GetKey()=='\r') break;
    
    rINTSUBMSK|=(BIT_SUB_RXD2|BIT_SUB_TXD2|BIT_SUB_ERR2);
    rUFCON2=(3<<6)|(2<<4)|(1<<2)|(1<<1)|(0);
    Uart_Printf("\nEnd Rx, receive data count=%d\n",IrDA_cnt);

    IrDAdataPt =(volatile U8 *)IrDABUFFER;
    for(i=0;i<IrDA_BUFLEN;i++)
    {
		if(i-(*IrDAdataPt))
		{
		    Uart_Printf("i=%d,",i);
		    IrDA_err++;
		}
		else
			Uart_Printf("%d,",*IrDAdataPt++);
    }
    if(IrDA_err)
    	Uart_Printf("IrDA test fail!! Error count=%d\n",IrDA_err);
    else
    	Uart_Printf("\nIrDA test is good!!\n");
    IrDA_Port_Return();
}
