#include <string.h>
#include "def.h"
#include "option.h"
#include "2440addr.h"
#include "2440lib.h"
#include "2440slib.h" 
#include "Am29f800.h"

//Revision History
// 2001.8.25:purnnamu:If the data is downloaded by ICE, the writing isn't done correct.
 
int AM29F800_ProgFlash(U32 realAddr,U16 data);
void AM29F800_EraseSector(int targetAddr);
int AM29F800_CheckId(void);
int BlankCheck(int targetAddr,int targetSize);
int _WAIT(void);

static void InputTargetAddr(void);

// Because KS32C41000_A1 is connected to AM29LV800_A0,
// the addr parameter has to be a WORD address, so called in AMD specification.

#define _WR(addr,data)	*((U16 *)(addr<<1))=(U16)data
#define _RD(addr)	( *((U16 *)(addr<<1)) )
#define _RESET()	_WR(0x0,0xf0f0)
#define BADDR2WADDR(addr)   (addr>>1)
extern U32 downloadAddress;
extern U32 downloadProgramSize;
static U32 srcAddress;
static U32 srcOffset;
static U32 targetAddress;
static U32 targetSize;


void ProgramAM29F800(void)
{
    int i;

    InputTargetAddr();
    
    if(downloadProgramSize==0)
    {
        Uart_Printf("The data must be downloaded using ICE from %x\n",downloadAddress);
    	srcAddress=downloadAddress;
    }
    else
    	srcAddress=downloadAddress+4; //to discard the data head for the size
    	

    Uart_Printf("[Check AM29LVxxx]\n");
    switch(AM29F800_CheckId())  //04.01.12 junon
    {
	case 1 :
	    Uart_Printf("This device is AM29LV200B!\n");	
	    break;
	case 2 :
	    Uart_Printf("This device is AM29LV400B!\n");	
	    break;
	case 3 :
	    Uart_Printf("This device is AM29LV800B!\n");	
	    break;
	case 4 :
	    Uart_Printf("This device is AM29LV160B!\n");	
	    break;
    	default:
	    Uart_Printf("ID Check Error!!!\n");
	    return;
    }
    
    Uart_Printf("\nErase the sector:0x%x.\n",targetAddress);
    
    AM29F800_EraseSector(targetAddress);
    if(!BlankCheck(targetAddress,targetSize))
    {
	Uart_Printf("Blank Check Error!!!\n");
	return;
    }

    Uart_Printf("\nStart of the data writing.\n");

    for(i=0x0;i<targetSize;i+=2) 
    {
        AM29F800_ProgFlash(  i+targetAddress,*( (U16 *)(srcAddress+srcOffset+i) )  );
        if((i%0x1000)==0)Uart_Printf("%x ",i);
    }
    Uart_Printf("\nEnd of the data writing!!!\n");

    _RESET();

    Uart_Printf("\nVerifying Start.\n");
    for(i=0x0;i<targetSize;i+=2) 
    {
        if(*( (U16 *)(i+targetAddress) )!=*( (U16 *)(srcAddress+srcOffset+i) )  )
	{    
	    Uart_Printf("%x=verify error\n",i+targetAddress);
	    return;
	}
        if((i%0x1000)==0)Uart_Printf("%x ",i);
    }
    Uart_Printf("\nVerifying End!!!\n");

    Uart_Printf("Do you want another programming without additional download? [y/n]\n");
    if(Uart_Getch()=='y')ProgramAM29F800();
}



static void InputTargetAddr(void)
{
    static U32 nextTargetAddr=0,nextSrcOffset=0;
    Uart_Printf("[AM29F800 Writing Program]\n");

    Uart_Printf("\nCAUTION: Check AM29LV800 BYTE#(47) pin is connected to VDD.\n");

    Uart_Printf("\nSource size:0h~%xh\n",downloadProgramSize);
    Uart_Printf("\nAvailable Target/Source Address:\n"); 
    Uart_Printf("    0x0, 0x4000, 0x6000, 0x8000,0x10000,0x20000,0x30000,0x40000,\n");
    Uart_Printf("0x50000,0x60000,0x70000,0x80000,0x90000,0xa0000,0xb0000,0xc0000,\n");
    Uart_Printf("0xd0000,0xe0000,0xf0000\n");

    Uart_Printf("Input source offset[0x%x]:",nextSrcOffset);
    srcOffset=Uart_GetIntNum();
    if(srcOffset==(U32)(-1))srcOffset=nextSrcOffset;
    
    Uart_Printf("Input target address among above addresses[0x%x]:",nextTargetAddr);
    targetAddress=Uart_GetIntNum();
    if(targetAddress==(U32)(-1))targetAddress=nextTargetAddr;
 
    if(targetAddress<0x4000)targetSize=0x4000;
    else if(targetAddress<0x6000)targetSize=0x2000;
    else if(targetAddress<0x8000)targetSize=0x2000;
    else if(targetAddress<0x10000)targetSize=0x8000; 
    else targetSize=0x10000;

    Uart_Printf("source offset=0x%x\n",srcOffset);
    Uart_Printf("target address=0x%x\n",targetAddress);
    Uart_Printf("target block size=0x%x\n",targetSize);
    
    if(srcOffset<0x4000)nextSrcOffset=0x4000;
    else if(srcOffset<0x6000)nextSrcOffset=0x6000;
    else if(srcOffset<0x8000)nextSrcOffset=0x8000;
    else if(srcOffset<0x10000)nextSrcOffset=0x10000; 
    else nextSrcOffset=srcOffset+0x10000;
    
    if(targetAddress<0x4000)nextTargetAddr=0x4000;
    else if(targetAddress<0x6000)nextTargetAddr=0x6000;
    else if(targetAddress<0x8000)nextTargetAddr=0x8000;
    else if(targetAddress<0x10000)nextTargetAddr=0x10000; 
    else nextTargetAddr=targetAddress+0x10000;
 

}




int AM29F800_CheckId(void)
{
    U16 manId,devId;

    _RESET();
    
    _WR(0x555,0xaaaa);
    _WR(0x2aa,0x5555);
    _WR(0x555,0x9090);
    manId=_RD(0x0);

    _RESET(); // New 5V AM29F800 needs this command. 
    _WR(0x555,0xaaaa);
    _WR(0x2aa,0x5555);
    _WR(0x555,0x9090);
    devId=_RD(0x1);

    _RESET();   

    Uart_Printf("Manufacture ID(0x0001)=%4x, Device ID(0x22xx)=%4x\n",manId,devId);

    if(manId==0x0001 && devId==0x22bf) return 1; // AM29LV200B
    else if(manId==0x0001 && devId==0x22ba) return 2; // AM29LV400B
    else if(manId==0x0001 && devId==0x225b) return 3; // AM29LV800B
    else if(manId==0x0001 && devId==0x2249) return 4; // AM29LV160B        
    else if(manId==0x0004 && devId==0x2249) return 4; // 29LV160B fairchild        
    else return 0;
}




void AM29F800_EraseSector(int targetAddr)
{
        Uart_Printf("Sector Erase is started!\n");

        _RESET();

        _WR(0x555,0xaaaa);
        _WR(0x2aa,0x5555);
        _WR(0x555,0x8080);
        _WR(0x555,0xaaaa);
        _WR(0x2aa,0x5555);
        _WR(BADDR2WADDR(targetAddr),0x3030);
        _WAIT();
        _RESET();
}




int BlankCheck(int targetAddr,int targetSize)
{
    int i,j;
    for(i=0;i<targetSize;i+=2)
    {
	j=*((U16 *)(i+targetAddr));
	if( j!=0xffff)
	{
	    Uart_Printf("E:%x=%x\n",(i+targetAddr),j);
	    return 0;
	}
    }
    return 1;
}




int _WAIT(void) //Check if the bit6 toggle ends.
{
        volatile U16 flashStatus,old;

	old=*((volatile U16 *)0x0);

        while(1)
	{
	    flashStatus=*((volatile U16 *)0x0);
            if( (old&0x40) == (flashStatus&0x40) )break;
            if( flashStatus&0x20 )
	    {
		//Uart_Printf("[DQ5=1:%x]\n",flashStatus);
		old=*((volatile U16 *)0x0);
		flashStatus=*((volatile U16 *)0x0);
		if( (old&0x40) == (flashStatus&0x40) )
		    return 0;
		else return 1;
	    }
	    //Uart_Printf(".");
	    old=flashStatus;
        }
        //Uart_Printf("!\n");
	return 1;
}




int AM29F800_ProgFlash(U32 realAddr,U16 data)
{
	volatile U16 *tempPt;

        tempPt=(volatile U16 *)realAddr;
        _WR(0x555,0xaaaa);
        _WR(0x2aa,0x5555);
        _WR(0x555,0xa0a0);
        *tempPt=data;
        
	return _WAIT();

}




