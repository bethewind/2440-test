/*****************************************
  NAME: dma.c
  DESC: DMA memory2memory test
  HISTORY:
  2003.05.15:Leon YH KIM: draft ver 0.0
 *****************************************/

#include <string.h>
#include "def.h"
#include "option.h"
#include "2440addr.h"
#include "2440lib.h"
#include "2440slib.h" 

static void __irq Dma0Done(void);
static void __irq Dma1Done(void);
static void __irq Dma2Done(void);
static void __irq Dma3Done(void);
void DMA_M2M(int ch,int srcAddr,int dstAddr,int tc,int dsz,int burst);

typedef struct tagDMA
{
    volatile U32 DISRC;	    //0x0
    volatile U32 DISRCC;    //0x4
    volatile U32 DIDST;	    //0x8
    volatile U32 DIDSTC;    //0xc
    volatile U32 DCON;	    //0x10
    volatile U32 DSTAT;	    //0x14
    volatile U32 DCSRC;	    //0x18
    volatile U32 DCDST;	    //0x1c
    volatile U32 DMASKTRIG; //0x20
}DMA;

static volatile int dmaDone;

void Test_DMA(void)
{
    DMA_M2M(0,_NONCACHE_STARTADDRESS,_NONCACHE_STARTADDRESS+0x800000, 0x8000,2,1); //word,burst
    //DMA Ch 0
    DMA_M2M(0,_NONCACHE_STARTADDRESS,_NONCACHE_STARTADDRESS+0x800000,0x80000,0,0); //byte,single
    DMA_M2M(0,_NONCACHE_STARTADDRESS,_NONCACHE_STARTADDRESS+0x800000,0x40000,1,0); //halfword,single
    DMA_M2M(0,_NONCACHE_STARTADDRESS,_NONCACHE_STARTADDRESS+0x800000,0x20000,2,0); //word,single
    DMA_M2M(0,_NONCACHE_STARTADDRESS,_NONCACHE_STARTADDRESS+0x800000,0x20000,0,1); //byte,burst
    DMA_M2M(0,_NONCACHE_STARTADDRESS,_NONCACHE_STARTADDRESS+0x800000,0x10000,1,1); //halfword,burst
    DMA_M2M(0,_NONCACHE_STARTADDRESS,_NONCACHE_STARTADDRESS+0x800000, 0x8000,2,1); //word,burst

    //DMA Ch 1
    DMA_M2M(1,_NONCACHE_STARTADDRESS,_NONCACHE_STARTADDRESS+0x800000,0x80000,0,0); //byte,single
    DMA_M2M(1,_NONCACHE_STARTADDRESS,_NONCACHE_STARTADDRESS+0x800000,0x40000,1,0); //halfword,single
    DMA_M2M(1,_NONCACHE_STARTADDRESS,_NONCACHE_STARTADDRESS+0x800000,0x20000,2,0); //word,single
    DMA_M2M(1,_NONCACHE_STARTADDRESS,_NONCACHE_STARTADDRESS+0x800000,0x20000,0,1); //byte,burst
    DMA_M2M(1,_NONCACHE_STARTADDRESS,_NONCACHE_STARTADDRESS+0x800000,0x10000,1,1); //halfword,burst
    DMA_M2M(1,_NONCACHE_STARTADDRESS,_NONCACHE_STARTADDRESS+0x800000, 0x8000,2,1); //word,burst

    //DMA Ch 2
    DMA_M2M(2,_NONCACHE_STARTADDRESS,_NONCACHE_STARTADDRESS+0x800000,0x80000,0,0); //byte,single
    DMA_M2M(2,_NONCACHE_STARTADDRESS,_NONCACHE_STARTADDRESS+0x800000,0x40000,1,0); //halfword,single
    DMA_M2M(2,_NONCACHE_STARTADDRESS,_NONCACHE_STARTADDRESS+0x800000,0x20000,2,0); //word,single
    DMA_M2M(2,_NONCACHE_STARTADDRESS,_NONCACHE_STARTADDRESS+0x800000,0x20000,0,1); //byte,burst
    DMA_M2M(2,_NONCACHE_STARTADDRESS,_NONCACHE_STARTADDRESS+0x800000,0x10000,1,1); //halfword,burst
    DMA_M2M(2,_NONCACHE_STARTADDRESS,_NONCACHE_STARTADDRESS+0x800000, 0x8000,2,1); //word,burst

    //DMA Ch 3
    DMA_M2M(3,_NONCACHE_STARTADDRESS,_NONCACHE_STARTADDRESS+0x800000,0x80000,0,0); //byte,single
    DMA_M2M(3,_NONCACHE_STARTADDRESS,_NONCACHE_STARTADDRESS+0x800000,0x40000,1,0); //halfword,single
    DMA_M2M(3,_NONCACHE_STARTADDRESS,_NONCACHE_STARTADDRESS+0x800000,0x20000,2,0); //word,single
    DMA_M2M(3,_NONCACHE_STARTADDRESS,_NONCACHE_STARTADDRESS+0x800000,0x20000,0,1); //byte,burst
    DMA_M2M(3,_NONCACHE_STARTADDRESS,_NONCACHE_STARTADDRESS+0x800000,0x10000,1,1); //halfword,burst
    DMA_M2M(3,_NONCACHE_STARTADDRESS,_NONCACHE_STARTADDRESS+0x800000, 0x8000,2,1); //word,burst   

}

void DMA_M2M(int ch,int srcAddr,int dstAddr,int tc,int dsz,int burst)
{
    int i,time;
    volatile U32 memSum0=0,memSum1=0;
    DMA *pDMA;
    int length;
    
    length=tc*(burst ? 4:1)*((dsz==0)+(dsz==1)*2+(dsz==2)*4);
    
    Uart_Printf("[DMA%d MEM2MEM Test]\n",ch);

    switch(ch)
    {
    case 0:
    	pISR_DMA0=(int)Dma0Done;
    	rINTMSK&=~(BIT_DMA0);  
    	pDMA=(void *)0x4b000000;
    	break;
    case 1:
    	pISR_DMA1=(int)Dma1Done;
    	rINTMSK&=~(BIT_DMA1);  
    	pDMA=(void *)0x4b000040;
    	break;
    case 2:
	pISR_DMA2=(int)Dma2Done;
    	rINTMSK&=~(BIT_DMA2);  
    	pDMA=(void *)0x4b000080;
	break;
    case 3:
        pISR_DMA3=(int)Dma3Done;
       	rINTMSK&=~(BIT_DMA3);  
       	pDMA=(void *)0x4b0000c0;
        break;
    }
                                                                                                                            
    Uart_Printf("DMA%d %8xh->%8xh,size=%xh(tc=%xh),dsz=%d,burst=%d\n",ch, srcAddr,dstAddr,length,tc,dsz,burst);

    Uart_Printf("Initialize the src.\n");
    
    for(i=srcAddr;i<(srcAddr+length);i+=4)
    {
    	*((U32 *)i)=i^0x55aa5aa5;
    	memSum0+=i^0x55aa5aa5;
    }

    Uart_Printf("DMA%d start\n",ch);
    
    dmaDone=0;
    
    pDMA->DISRC=srcAddr;
    pDMA->DISRCC=(0<<1)|(0<<0); // inc,AHB
    pDMA->DIDST=dstAddr;
    pDMA->DIDSTC=(0<<2) | (0<<1)|(0<<0); // inc,AHB
    pDMA->DCON=tc|((unsigned int)1<<31)|(1<<30)|(1<<29)|(burst<<28)|(1<<27)|\
    	        (0<<23)|(1<<22)|(dsz<<20)|(tc);
    		//HS,AHB,TC interrupt,whole, SW request mode,relaod off
    pDMA->DMASKTRIG=(1<<1)|1; //DMA on, SW_TRIG

    Timer_Start(3);//128us resolution	      
    while(dmaDone==0);
    time=Timer_Stop();
    
    Uart_Printf("DMA transfer done. time=%f, %fMB/S\n",(float)time/ONESEC3, length/((float)time/ONESEC3)/1000000.);
    rINTMSK=BIT_ALLMSK;
    
    for(i=dstAddr;i<dstAddr+length;i+=4)
    {
    	memSum1+=*((U32 *)i)=i^0x55aa5aa5;
    }
    
    Uart_Printf("memSum0=%x,memSum1=%x\n",memSum0,memSum1);
    if(memSum0==memSum1)
    	Uart_Printf("DMA test result--------------------------------------O.K.\n");
    else 
    	Uart_Printf("DMA test result--------------------------------------ERROR!!!\n");

}


static void __irq Dma0Done(void)
{
    ClearPending(BIT_DMA0);
    dmaDone=1;
}

static void __irq Dma1Done(void)
{
    ClearPending(BIT_DMA1);
    dmaDone=1;
}

static void __irq Dma2Done(void)
{
    ClearPending(BIT_DMA2);
    dmaDone=1;
}

static void __irq Dma3Done(void)
{
    ClearPending(BIT_DMA3);
    dmaDone=1;
}

