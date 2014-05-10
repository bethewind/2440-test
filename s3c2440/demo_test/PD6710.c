#include "def.h"
#include "option.h"
#include "2440addr.h"
#include "2440lib.h"
#include "2440slib.h" 

#include "PD6710.h"

/*
	DESCRIPTIONS OF THE S3C2440 BOARD
	
	EINT3(GPF3)=nIRQ_PCMCIA(nINT_P_CON)=nINTR
	EINT8(GPG0)=IRQ_PCMCIA(INT_P_DEV)=IRQ3

	A24=1, I/O area (A26 should have been used.)
	A24=0, mem area
	AEN=nGCS2, 

	absolute address		
	0x10000000~0x10FFFFFF: memory area
	0x11000000~0x11FFFFFF: I/O area
*/	


int  PD6710_Init(void);
void PD6710_InitBoard(void);
void PD6710_InitInterrupt(void);
void PD6710_CardEnable(void);
void PD6710_Wr(U8 index, U8 data);
U8 PD6710_Rd(U8 index);
void PD6710_Modify(U8 index,U8 mask,U8 data);
void PD6710_CommonMemAccess(void);
void PD6710_AttrMemAccess(void);

U8 Card_RdAttrMem(U32 memaddr);
U8 Card_RdIO(U32 ioaddr);
void Card_WrIO(U32 ioaddr,U8 data);

void PrintCIS(void);

void __irq IsrPD6710Card(void);
void __irq IsrPD6710Management(void);

volatile int isCardInsert=0;

void _dbg(int i)
{
    Uart_Printf("<%d>",i);
    //Uart_Printf("INT_GENERAL_CTRL=%x\n",PD6710_Rd(INT_GENERAL_CTRL));
}

void Test_PD6710(void)
{
    Uart_Printf("[PD6710 test for reading pc_card CIS]\n");
    Uart_Printf("Insert PC card!!!\n");
    PD6710_InitBoard();
    PD6710_Init();
    PD6710_InitInterrupt();

    //Check if CD1,2 is L. If nCD1,2 is L, the card has been inserted.
    //In this case, the card management interrupt isn't occurred.
    if((PD6710_Rd(INTERFACE_STATUS)&0xc)==0xc) 
    {
	Uart_Printf("Card is inserted.\n");
	Delay(2000); 
	//For card contact stablization. This delay is needed for stable operation.
	//If this delay isn't here, CF card will not be identified.
	PD6710_CardEnable();
	PrintCIS();
    }

    while(1)
    {
	   if(Uart_GetKey()==0x1b) break;
		Uart_Printf(".");
		Delay(10000);
    }
}


//IRQ stack size is enlarged to 4096 byte.
void PrintCIS(void)
{
    int i,j;
    int cisEnd=0;
    static U8 str[16];
    U8 c;
    Uart_Printf("[Card Information Structure]\n");
	
    PD6710_AttrMemAccess();

    //search the end of CIS
    while(1)
    {	 
	    c=Card_RdAttrMem((cisEnd)*2);
		if(c ==0xff)	//0xff= termination tuple	
		    break;
		Uart_Printf("cisEnd:%d, CIS RD:%x\n", cisEnd, c);
		cisEnd++;	
		cisEnd+=Card_RdAttrMem((cisEnd)*2)+1;
    }
    Uart_Printf("cisEnd=0~%x\n",cisEnd);			

    for(i=0;i<=cisEnd*2;i+=2)
    {
		c=Card_RdAttrMem(i);
		str[(i%0x20)/2]=c;
		Uart_Printf("%2x,",c);
		if((i%0x20)>=0x1e)
		{
		    Uart_Printf("//");
		    for(j=0;j<0x10;j++)
			if(str[j]>=' ' && str[j]<=127)Uart_Printf("%c",str[j]);
			else Uart_Printf(".");
		    Uart_Printf("\n");
		}
    }
    Uart_Printf("\n");
}


#define B6710_Tacs	(0x0)	// 0clk
#define B6710_Tcos	(0x3)	// 4clk
#define B6710_Tacc	(0x7)	// 14clk
#define B6710_Tcoh	(0x1)	// 1clk
#define B6710_Tah	(0x0)	// 0clk
#define B6710_Tacp	(0x3)	// 6clk
#define B6710_PMC	(0x0)	// normal(1data)





void PD6710_InitBoard(void)
{
//Initialize S3C2440 for PD6710
    rGPFCON=rGPFCON&~(3<<6)|(2<<6);	//EINT3(GPF3)=nIRQ_PCMCIA(nINT_P_CON)=nINTR
    rGPGCON=rGPGCON&~(3<<0)|(2<<0);     //EINT8(GPG0)=IRQ_PCMCIA(INT_P_DEV)=IRQ3

    rBWSCON=rBWSCON&~(0xf<<8)|(0xd<<8);	  //nGCS2=nUB/nLB(nSBHE),nWAIT,16-bit

    rBANKCON2=((B6710_Tacs<<13)+(B6710_Tcos<<11)+(B6710_Tacc<<8)+(B6710_Tcoh<<6)\
		+(B6710_Tah<<4)+(B6710_Tacp<<2)+(B6710_PMC));
}


void PD6710_InitInterrupt(void)
{
    //EINT7(GPE7)=nINT_P_CON=nINTR
    //EINT6(GPE6)=nINT_P_DEV=IRQ3

    rEXTINT0=rEXTINT0&~(7<<12)|(2<<12);
	//EINT3(GPF3)=nIRQ_PCMCIA(nINT_P_CON)=nINTR =falling edge
    rEXTINT1=rEXTINT1&~(7<<0)|(4<<0);   	
	//EINT8(GPG0)=IRQ_PCMCIA(INT_P_DEV)=IRQ3 =rising edge

    pISR_EINT3=(U32)IsrPD6710Management;	//nINT_P_CON
    pISR_EINT8_23=(U32)IsrPD6710Card;		//nINT_P_DEV
    rSRCPND = BIT_EINT3|BIT_EINT8_23; //to clear the previous pending states
    rINTPND = BIT_EINT3|BIT_EINT8_23;

    rINTMSK=~(BIT_EINT3|BIT_EINT8_23);  
    rEINTMASK=rEINTMASK&~(1<<8)|(0<<8); //EINTMASK[8]=enable interrupt.
}


int PD6710_Init(void)
{
    //Initialize PD-6710
    PD6710_Wr(POWER_CTRL,(0<<7)|(1<<5)|(0<<4)|(0<<0)); 
	//output2card_disable,AUTO_POWER,VCC_POWER_OFF,Vpp1=0V
		
    PD6710_Wr(INT_GENERAL_CTRL,(1<<7)|(0<<5)|(1<<4)|(3<<0)); 
	//nSTSCHG=status change(not RI input),mem_card_interface,
	//manage_int=-INTR pin, nINT_P_DEV=IRQ3

    PD6710_Wr(MANAGEMENT_INT_CONFIG,(1<<0)|(1<<3)|(3<<4)); 
	//status_change_int_enable, card_detect_int_enable, IRQ disabled(IRQ_3)
	//Is nINTR pin used???
    PD6710_Wr(IO_WINDOW_CTRL,0|(0<<3));
	//IO0=8bit,timing_set_0

    // I/O windows must never include 3e0h and 3e1h 		
    // IO AREA=0x3f8~0x3ff(COM1) ->0x3f8~0x3ff
    PD6710_Wr(SYS_IO_MAP0_START_L,0xf8);	
    PD6710_Wr(SYS_IO_MAP0_START_H,0x3);
    PD6710_Wr(SYS_IO_MAP0_END_L,0xff);
    PD6710_Wr(SYS_IO_MAP0_END_H,0x3);           
    PD6710_Wr(CARD_IO_MAP0_OFFSET_L,0x0);
    PD6710_Wr(CARD_IO_MAP0_OFFSET_H,0x0);

    //PD6710_Wr(SYS_MEM_MAP0_START_L,0x10); 
    //If this is memory window, the lowest 64KB should be reserved.
    PD6710_Wr(SYS_MEM_MAP0_START_L,0x0); //MEM0=8bit data width
    	//To access CIS, I have known that memory window bus width should be
    	//  8-bit by experiment. 
    	//Originally, CIS uses 8-bit bus. But, Why not in 16-bit bus width?
    PD6710_Wr(SYS_MEM_MAP0_START_H,0x0);
    PD6710_Wr(SYS_MEM_MAP0_END_L,0x0f);		//0x0 ~ 0xffff
    PD6710_Wr(SYS_MEM_MAP0_END_H,0x0|(0<<6)); //timing_set_0
    PD6710_Wr(CARD_MEM_MAP0_OFFSET_L,0x0);
    PD6710_Wr(CARD_MEM_MAP0_OFFSET_H,0x0|(1<<6)); //nREG=active 
    	//MEM AREA=0x0~0xFFFF ->0x0~0xFFFFFF
    
    PD6710_Wr(MAPPING_ENABLE,1|(1<<6));
    	//memory map 0 enabled, I/O map 0 enabled
    
    PD6710_Wr(MISC_CTRL1,(0<<7)|(1<<4)|(1<<3)|(1<<2)|(1<<1)); 
    	//INPACK_ignored,speak_enable,edge_irq_intr,edge_management_intr,nVCC_3_enabled(temp)
    PD6710_Wr(MISC_CTRL2,1|(1<<1)|(1<<4)); 
    	//25Mhz_bypass,low_power_dynamic,IRQ12=drive_LED
    
    PD6710_Wr(FIFO_CTRL,0x80);	//Flush FIFO
    //before configuring timing register, FIFO should be cleared.

    //default access time is 300ns
    PD6710_Wr(SETUP_TIMING0,0x2);	//80ns(no spec)
    PD6710_Wr(CMD_TIMING0,0x8);		//320ns(by spec,25Mhz clock)
    PD6710_Wr(RECOVERY_TIMING0,0x2);	//80ns(no spec)
    
    PD6710_Wr(CHIP_INFO,0x0);
    if((PD6710_Rd(CHIP_INFO)&0xc0)!=0xc0 || (PD6710_Rd(CHIP_INFO)&0xc0)!=0x00 )
    {
    	Uart_Printf("PD6710 hardware identification error!!!\n");
    	return 0;
    }
    return 1;
}


void PD6710_CardEnable(void) //after insertion interrupt
{
    if(PD6710_Rd(MISC_CTRL1)&0x1)  //MISC_CTRL1[0] 0=3.3V  1=5.0V 
    {
    	PD6710_Modify(MISC_CTRL1,0x2,0x0); //nVCC_5_enabled
    	Uart_Printf("5.0V card is detected.\n");
    }
    else
    {
    	PD6710_Modify(MISC_CTRL1,0x2,0x2); //nVCC_3_enabled
    	Uart_Printf("3.3V card is detected.\n");
    }
    PD6710_Modify(POWER_CTRL,(1<<4)|3,(1<<4)|1);
    	//VCC_POWER_on,VPP=Vcc(3.3V or 5.0V)
    Delay(100);
    	//Delay 10ms should be here for power stabilization.
    	//If this time is not here, the program may halt 
	// in case that the pc card has been inserted before power_on   
	//But, Why?	
    	
    Delay(10); //RESET should be Hi-Z for minimum 1ms
    PD6710_Modify(INT_GENERAL_CTRL,(1<<6),0); //RESET=active(H)
    PD6710_Modify(POWER_CTRL,(1<<7),(1<<7)); //output2card_enable(RESET=Hi-Z -> output)
    PD6710_Modify(INT_GENERAL_CTRL,(1<<6),0); //RESET=active(H)	
    Delay(1); //wait for minimum 10us
    PD6710_Modify(INT_GENERAL_CTRL,(1<<6),(1<<6)); //RESET=inactive(L)
    Delay(200);	//wait for 20ms
    //READY pin isn't available in mem_io_card mode.
    //So, don't check READY pin on I/O card.
    //while(!(PD6710_Rd(INTERFACE_STATUS)&0x20)); //INTERFACE_STATUS[5]=READY_PIN_STATUS
    PD6710_Modify(INT_GENERAL_CTRL,(1<<5),(1<<5));	//mem_card -> mem_io_card
    
    Delay(5000); 
	//If this delay isn't here, some CF card will not be identified.
   	//In oder to remove this delay, I think, we have to check READY signal.
}	


void PD6710_Wr(U8 index, U8 data)
{

    rPD6710_INDEX=index;
    rPD6710_DATA=data;

}


U8 PD6710_Rd(U8 index)
{
    //nREG=L
    rPD6710_INDEX=index;
    return rPD6710_DATA;
}


void PD6710_Modify(U8 index,U8 mask,U8 data)
{
    //nREG=L
    rPD6710_INDEX=index;
    rPD6710_DATA=(rPD6710_DATA)&~mask|data;
}


U8 Card_RdAttrMem(U32 memaddr)
{
    //nREG=L
    return *((volatile U8 *)(PD6710_MEM_BASE_ADDRESS+memaddr));
}


U8 Card_RdIO(U32 ioaddr)
{
    return *((volatile U8 *)(PD6710_IO_BASE_ADDRESS+ioaddr));
}    


void Card_WrIO(U32 ioaddr,U8 data)
{
    *((volatile U8 *)(PD6710_IO_BASE_ADDRESS+ioaddr))=data;
}


/*
1)	I/O access: nREG=L(automatic)
	attribute memory access: nREG=L
	common memory access: nREG=H
2)	Before accessing Common memory area,
	PD6710_BeginCommonMemAccess() should be called to make nREG 'H'.
*/	
void PD6710_CommonMemAccess(void)
{
    PD6710_Modify(CARD_MEM_MAP0_OFFSET_H,(1<<6),(0<<6)); //nREG=inactive, H
}


void PD6710_AttrMemAccess(void)
{
    PD6710_Modify(CARD_MEM_MAP0_OFFSET_H,(1<<6),(1<<6)); //nREG=active, L
}


void __irq IsrPD6710Management(void)    //nINT_P_CON
{
    U8 cardStat;
    //ClearPending(BIT_EINT7);     
    Uart_Printf("\nPD6710 interrupt is occurred.\n");
    
    Delay(2000); 
	//For card contact stablization. This delay is needed for operation stability
	//If this delay isn't here, some CF card may not be identified.
 	
    cardStat=PD6710_Rd(CARD_STAT_CHANGE);
    //Card detect changed?
    if(cardStat&0x8)
    {
	//check if CD1,2 is L.
	if((PD6710_Rd(INTERFACE_STATUS)&0xc)==0xc) 
	{
	    Uart_Printf("Card is inserted.\n");
	    PD6710_CardEnable();
	    PrintCIS();
	}
	else
	{
	    Uart_Printf("Card is ejected.\n");
	    PD6710_Init(); //can be removed.
	}
    }
    ClearPending(BIT_EINT3); 
	//For level interrupt, the int pending should be cleared at the end of ISR.
}


void __irq IsrPD6710Card(void)	//nINT_P_DEV
{
    rEINTPEND=(1<<8); //EINTPEND[8] is cleared.
    ClearPending(BIT_EINT8_23);
    Uart_Printf("PC card interrupt is occurred.\n");
}



