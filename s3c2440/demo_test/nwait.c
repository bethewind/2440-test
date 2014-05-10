#include <string.h>
#include "def.h"
#include "option.h"
#include "2440addr.h"
#include "2440lib.h"

void WaitRd8(void);
void WaitWr8(void);
void WaitRd32(void);
void WaitWr32(void);
void WaitRd16(void);
void WaitWr16(void);
unsigned save_rGPBUP,save_rGPBDAT,save_rGPBCON;


#define BUS8		(0)
#define BUS16		(1)
#define BUS32		(2)
#define ENWAIT		(1)
#define SRAMBE03	(1)

#define B4_Tacs		(0x0)	//0clk
#define B4_Tcos		(0x0)	//0clk
#define B4_Tacc		(0x2)	//3//clk
#define B4_Tcoh		(0x0)	//0clk
#define B4_Tah		(0x0)	//0clk
#define B4_Tacp		(0x0)	//2//clk
#define B4_PMC		(0x0)	//no page mode

#define	CS_nGCS4	0x20000000
// Work-around with nWAIT.

void Test_WaitPin(void)
{
    U32 savePLLCON,saveCLKDIVN;
	int i;

    saveCLKDIVN=rCLKDIVN;
    savePLLCON=rMPLLCON;

    Uart_TxEmpty(1);
    ChangeClockDivider(12,12);	    // 1:2:4 
    ChangeMPllValue(246,13,1);  //Fin=12MHz FCLK=100MHz for ALTERA
    Uart_Init(101606000/4,115200);		// PCLK is about 25MHz

    rMISCCR=0x330;	//HCLK
    rGPHCON=(rGPHCON & (~(0x3<<18))) | (0x2<<18);   //GPH9=CLKOUT0

    Uart_Printf("Select test bus width(32bit(0)/16bit(1)/8bit(2): ");
    i=Uart_GetIntNum();

    switch(i) {
		case 0:
		    WaitWr32();
		    Uart_Printf("\n");
		    WaitRd32();
		    Uart_Printf("\n");
		    break;
		case 1:
			WaitWr16();
			Uart_Printf("\n");
			WaitRd16();
			Uart_Printf("\n");
		break;
		case 2:
			WaitWr8();
			Uart_Printf("\n");
			WaitRd8();
			Uart_Printf("\n");
		break;
		default:
		break;
	}
		
    Uart_TxEmpty(1);
    rCLKDIVN=saveCLKDIVN;
    rMPLLCON=savePLLCON;
    Uart_Init(0,115200);    
}

void WaitRd8(void)
{
    U8 readVal;
    int i;
	
 	Uart_Printf("[nGCS4,8-bit,read,nWAIT Test].\n");
    rBWSCON=rBWSCON & ~(0xf<<16)|(BUS8<<16)|(ENWAIT<<18); //nGCS4 
    rBANKCON4=((B4_Tacs<<13)+(B4_Tcos<<11)+(B4_Tacc<<8)+(B4_Tcoh<<6)+(B4_Tah<<4)+(B4_Tacp<<2)+(B4_PMC));

	Uart_Printf("rBWSCON:%x\n", rBWSCON);
	Uart_Printf("rBANKCON4:%x\n", rBANKCON4);
	
    for(i=0;i<2;i++); //wait until the bank configuration is in effect.

	Uart_Printf("Press any key to stop...\n");
	while(1) {
	    readVal=(*(volatile U8 *)CS_nGCS4);
		if(Uart_GetKey()!=0) break;
	}
    readVal&=0xf;//data[3:0]
    Uart_Printf("read_data=%x.\n",readVal);
}


void WaitWr8(void)
{
    int i;

    Uart_Printf("[nGCS4,8-bit,write,nWAIT Test].\n");

    rBWSCON=rBWSCON & ~(0xf<<16)|(BUS8<<16)|(ENWAIT<<18); //nGCS4 
    rBANKCON4=((B4_Tacs<<13)+(B4_Tcos<<11)+(B4_Tacc<<8)+(B4_Tcoh<<6)+(B4_Tah<<4)+(B4_Tacp<<2)+(B4_PMC));

	Uart_Printf("rBWSCON:%x\n", rBWSCON);
	Uart_Printf("rBANKCON4:%x\n", rBANKCON4);

	for(i=0;i<2;i++); //wait until the bank configuration is in effect.

	Uart_Printf("Press any key to stop...\n");
	while(1) {
	    *((volatile U8 *)CS_nGCS4)=0;
		if(Uart_GetKey()!=0) break;
	}
}



void WaitRd16(void)
{
    U16 readVal;
    int i;

    Uart_Printf("[nGCS4,16-bit,read,nWAIT Test].\n");

    rBWSCON=rBWSCON & ~(0xf<<16)|(BUS16<<16)|(ENWAIT<<18); //nGCS4 
    rBANKCON4=((B4_Tacs<<13)+(B4_Tcos<<11)+(B4_Tacc<<8)+(B4_Tcoh<<6)+(B4_Tah<<4)+(B4_Tacp<<2)+(B4_PMC));

	Uart_Printf("rBWSCON:%x\n", rBWSCON);
	Uart_Printf("rBANKCON4:%x\n", rBANKCON4);
	
    for(i=0;i<2;i++); //wait until the bank configuration is in effect.

	Uart_Printf("Press any key to stop...\n");
	while(1) {
	    readVal=(*(volatile U16 *)CS_nGCS4);
		if(Uart_GetKey()!=0) break;
	}
    readVal&=0xf;//data[3:0]
    Uart_Printf("read_data=%x\n",readVal);
}


void WaitWr16(void)
{
    int i;

    Uart_Printf("[nGCS4,16-bit,write,nWAIT Test].\n");

    rBWSCON=rBWSCON & ~(0xf<<16)|(BUS16<<16)|(ENWAIT<<18); //nGCS4 
    rBANKCON4=((B4_Tacs<<13)+(B4_Tcos<<11)+(B4_Tacc<<8)+(B4_Tcoh<<6)+(B4_Tah<<4)+(B4_Tacp<<2)+(B4_PMC));
    
    Uart_Printf("rBWSCON:%x\n", rBWSCON);
    Uart_Printf("rBANKCON4:%x\n", rBANKCON4);
	
    for(i=0;i<2;i++); //wait until the bank configuration is in effect.

    Uart_Printf("Press any key to stop...\n");
    while(1) 
    {
        *((volatile U16 *)CS_nGCS4)=0;
        if(Uart_GetKey()!=0) 
    	    break;
    }
}

void WaitRd32(void)
{
//    U32 readVal;
    int i;

    Uart_Printf("[nGCS4,32-bit,read,nWAIT Test].\n");

    rBWSCON=rBWSCON & ~(0xf<<16)|(BUS32<<16)|(ENWAIT<<18); //nGCS4 
    rBANKCON4=((B4_Tacs<<13)+(B4_Tcos<<11)+(B4_Tacc<<8)+(B4_Tcoh<<6)+(B4_Tah<<4)+(B4_Tacp<<2)+(B4_PMC));

    Uart_Printf("rBWSCON:%x\n", rBWSCON);
    Uart_Printf("rBANKCON4:%x\n", rBANKCON4);
	
    for(i=0;i<2;i++); //wait until the bank configuration is in effect.

    Uart_Printf("Press any key to stop...\n");
    while(1) 
    {
       // readVal=(*(volatile U32 *)CS_nGCS4);
    	if(Uart_GetKey()!=0) 
	    break;
    }
}


void WaitWr32(void)
{
    
    int i;
    
    Uart_Printf("[nGCS4,32-bit,write,nWAIT Test].\n");

    rBWSCON=rBWSCON & ~(0xf<<16)|(BUS32<<16)|(ENWAIT<<18); //nGCS4 
    rBANKCON4=((B4_Tacs<<13)+(B4_Tcos<<11)+(B4_Tacc<<8)+(B4_Tcoh<<6)+(B4_Tah<<4)+(B4_Tacp<<2)+(B4_PMC));

    Uart_Printf("rBWSCON:%x\n", rBWSCON);
    Uart_Printf("rBANKCON4:%x\n", rBANKCON4);
	
    for(i=0;i<2;i++); //wait until the bank configuration is in effect.

    Uart_Printf("Press any key to stop...\n");
    while(1) 
    {
        *((volatile U32 *)CS_nGCS4)=0;
	if(Uart_GetKey()!=0) 
	    break;	
    }
}

void Test_XBREQ(void)
{
    U32 saveGPBCON;	 //  saveGPBCON,

    saveGPBCON=rGPBCON;
    rGPBCON=0x2800;

    Uart_Printf("Press any key!!\n");
    Uart_Getch();
    
    rGPBCON=saveGPBCON;
}
