//====================================================================
// File Name : strata32.c
// Function  : S3C2440 Intel Strata NOR Flash
// Program   : Lee, Sang Jo (LSJ)
// Date      : June 14, 2002
// Version   : 0.0
// History
//   0.0 : Programming start (June 14, 2002) -> LSJ
//         Arrangement source code(8/01/2002)-> SOP   
//====================================================================

#include <stdlib.h>
#include <string.h>
#include "def.h"
#include "option.h"
#include "2440addr.h"
#include "2440lib.h"
#include "2440slib.h"
#include "strata32.h"

static void InputAddresses(void);

static int  Strata_ProgFlash(U32 realAddr,U32 data);
static void Strata_EraseSector(int targetAddr);
static int  Strata_CheckID(int targetAddr);
static int  Strata_CheckDevice(int targetAddr);
static int  Strata_CheckBlockLock(int targetAddr);
static int  Strata_BlankCheck(int targetAddr,int targetSize);
//static int  _WAIT(void);

extern U32 downloadAddress;
extern U32 downloadProgramSize;

static U32 srcAddress;
static U32 targetOffset; 
static U32 targetAddress; 
static U32 targetSize; 

// Because S3C2440 is connected to Intel StrataFlash 28F128J3A,
// the addr parameter has to be a WORD address, so called in Intel specification.

    // by chc
#define _WR(addr,data)  *((volatile U32 *)(addr))=(U32)data 
#define _RD(addr)       ( *((volatile U32 *)(addr)) )       
    // _RESET() : Read Array
#define _RESET()    _WR(targetAddress,0x00ff00ff)

extern U32 downloadAddress;
extern U32 downloadProgramSize;

int error_erase=0;       // Read Status Register, SR.5
int error_program=0;     // Read Status Register, SR.4

//==========================================================================================
int Strata_CheckID(int targetAddr) 
{
    _RESET();
    _WR(targetAddr, 0x00900090); 
    return _RD(targetAddr); // Read Identifier Code, including lower, higher 16-bit, 8MB, Intel Strate Flash ROM
                            // targetAddress must be the beginning location of a Block Address
}

//==========================================================================================
int Strata_CheckDevice(int targetAddr) 
{
    _RESET();
    _WR(targetAddr, 0x00900090);
    return _RD(targetAddr+0x4); // Read Device Code, including lower, higher 16-bit, 8MB, Intel Strate Flash ROM
                                // targetAddress must be the beginning location of a Block Address
}

//==========================================================================================
int Strata_CheckBlockLock(int targetAddr) 
{
    _RESET();
    _WR(targetAddr, 0x00900090);
    return _RD(targetAddr+0x8); // Read Block Lock configuration, 
                                // targetAddress must be the beginning location of a Block Address
}

void Strata_Unlock(int targetAddr) 
{
    _RESET();
    _WR(targetAddr, 0x00600060);
    _WR(targetAddr, 0x00D000D0);
}

void Strata_SetBlockLock(int targetAddr)
{
	_RESET();
	_WR(targetAddr, 0x00600060);
	_WR(targetAddr, 0x00010001);
}

//==========================================================================================
void Strata_EraseSector(int targetAddress) 
{
    unsigned long ReadStatus;
    unsigned long bSR5;     // Erase and Clear Lock-bits Status, lower 16bit, 8MB Intel Strate Flash ROM
    unsigned long bSR5_2;   // Erase and Clear Lock-bits Status, higher 16bit, 8MB Intel Strate Flash ROM
    unsigned long bSR7;     // Write State Machine Status, lower 16bit, 8MB Intel Strate Flash ROM
    unsigned long bSR7_2;   // Write State Machine Status, higher 16bit, 8MB Intel Strate Flash ROM
    //_RESET();
//  _WR(targetAddress, 0x00200020);
//  _WR(targetAddress, 0x00d000d0);
    _WR(targetAddress, 0x00200020); // Block Erase, First Bus Cycle, targetAddress is the address withint the block
    _WR(targetAddress, 0x00d000d0); // Block Erase, Second Bus Cycle, targetAddress is the address withint the block
    
    //_RESET();
    _WR(targetAddress, 0x00700070); // Read Status Register, First Bus Cycle, targetAddress is any valid address within the device
    ReadStatus=_RD(targetAddress);  // Read Status Register, Second Bus Cycle, targetAddress is any valid address within the device
    bSR7=ReadStatus & (1<<7);       // lower 16-bit 8MB Strata
    bSR7_2=ReadStatus & (1<<(7+16));// higher 16-bit 8MB Strata
    while(!bSR7 | !bSR7_2) 
    {
        _WR(targetAddress, 0x00700070);
        ReadStatus=_RD(targetAddress);
        bSR7=ReadStatus & (1<<7);
        bSR7_2=ReadStatus & (1<<(7+16));
//      Uart_Printf("wait !!\n");
    }

    _WR(targetAddress, 0x00700070); // When the block erase is complete, status register bit SR.5 should be checked. 
                    // If a block erase error is detected, the status register should be cleared before
                    // system software attempts correct actions.
    ReadStatus=_RD(targetAddress);  
    bSR5=ReadStatus & (1<<5);           // lower 16-bit 8MB Strata 
    bSR5_2=ReadStatus & (1<<(5+16));    // higher 16-bit 8MB Strata 
    if (bSR5==0 && bSR5_2==0) 
    {
        Uart_Printf("Block_%x Erase O.K. \n",targetAddress);
    } 
    else 
    {
        //Uart_Printf("Error in Block Erasure!!\n");
        _WR(targetAddress, 0x00500050); // Clear Status Register
        error_erase=1;                  // But not major, is it casual ?
    }

    _RESET();   // write 0xffh(_RESET()) after the last opoeration to reset the device to read array mode.
}

//==========================================================================================
int Strata_BlankCheck(int targetAddr,int targetSize) 
{
    int i,j;
    for (i=0; i<targetSize; i+=4) 
    {
        j=*((volatile U32 *)(i+targetAddr));
        if (j!=0xffffffff)      // In erasure it changes all block dta to 0xff
        {
            Uart_Printf("E : %x = %x\n", (i+targetAddr), j);
            return 0;
        }
    }
    return 1;
}

//==========================================================================================
int Strata_ProgFlash(U32 realAddr,U32 data) 
{
    volatile U32 *ptargetAddr;
    unsigned long ReadStatus;
    unsigned long bSR4;    // Erase and Clear Lock-bits Status, lower 16bit, 8MB Intel Strate Flash ROM
    unsigned long bSR4_2;  // Erase and Clear Lock-bits Status, higher 16bit, 8MB Intel Strate Flash ROM
    unsigned long bSR7;    // Write State Machine Status, lower 16bit, 8MB Intel Strate Flash ROM
    unsigned long bSR7_2;  // Write State Machine Status, higher 16bit, 8MB Intel Strate Flash ROM

    ptargetAddr = (volatile U32 *)realAddr;
    //_RESET();

    _WR(realAddr, 0x00400040);  // realAddr is any valid adress within the device
                                // Word/Byte Program(or 0x00100010 can be used)
    *ptargetAddr=data;          // 32 bit data

    //_RESET();
    _WR(realAddr, 0x00700070);  // Read Status Register
    ReadStatus=_RD(realAddr);   // realAddr is any valid address within the device
    bSR7=ReadStatus & (1<<7);
    bSR7_2=ReadStatus & (1<<(7+16));
    while(!bSR7 || !bSR7_2) 
    {
        // _RESET();
        _WR(realAddr, 0x00700070);        // Read Status Register
        ReadStatus=_RD(realAddr);
        bSR7=ReadStatus & (1<<7);
        bSR7_2=ReadStatus & (1<<(7+16));
    }
    
    _WR(realAddr, 0x00700070); 
    ReadStatus=_RD(realAddr);             // Real Status Register
    bSR4=ReadStatus & (1<<4);
    bSR4_2=ReadStatus & (1<<(4+16));
    
    if (bSR4==0 && bSR4_2==0) 
    {
        //Uart_Printf("Successful Program!!\n");
        ;
    } 
    else 
    {
        //Uart_Printf("Error Program!!\n");
        _WR(realAddr, 0x00500050);          // Clear Status Register
        error_program=1;                    // But not major, is it casual ?
    }

    _RESET();
    return 0;
}

#define TARGET_ADDR_28F128      0x08000000  // nGCS4, 128MB area
#define SOURCE_ADDR_FOR_28F128  0x31000000  // After 16MB of SDRAM
                                            // 0x30000000 - 0x30ffffff : Area for this test program

//==========================================================================================                                            
void Program28F128J3A(void)
{
// FlashROM write program must reside at RAM region NOT ROM region
// In reading and writing all interrupts are disabled because the flash ROM
// strongly dislike to be disturbed by other stuff.
// And the region of flash ROM must be I/O region which means NO cacheable
// and NO bufferable in MMU. Check it out !!!
// 2001.6.18. Mon. It's local rain. I'll hope it eliminates the drought in Korea. by chc

//    unsigned long interrupt_reservoir;
    int i;

    Uart_Printf("\n[ 28F128J3A Flash Writing Program ]\n\n");
    Uart_Printf("     *** Very Important Notes ***\n");
    Uart_Printf("1. 28F128J3A must be located at 0x08000000.\n"
            " J1:1-2, J2:2-3, J3:2-3, J4:1-2 \n");
    Uart_Printf("2. After programming, 28F128J3A may be located at 0x0.\n"
			 " J1:2-3, J2:1-2, J3:1-2, J4:2-3 \n");

    rINTMSK = BIT_ALLMSK;   
    targetAddress=TARGET_ADDR_28F128;
    targetSize=downloadProgramSize;
    //downloadAddress=0x31000000;

    if(targetSize==0)
    {
        Uart_Printf("\nThe data must be downloaded using ICE or USB from 0x31000000\n");
        srcAddress=downloadAddress; 
    }
    else
    { 
        srcAddress=downloadAddress+4; //to discard the data head for the size
    }

    InputAddresses(); //srcAddress,targetSize,targetOffset will be determined.      
    Uart_Printf("Source base address(0x31000000) = 0x%x\n",srcAddress);
    Uart_Printf("Target base address(0x08000000) = 0x%x\n",targetAddress);
    Uart_Printf("Target offset      (0x0)        = 0x%x\n",targetOffset);
    Uart_Printf("Target size        (0x20000*n)  = 0x%x\n",targetSize);

    if ( (Strata_CheckID(targetAddress) & 0xffff) != 0x0089 )       // ID number = 0x0089
    {
		Uart_Printf("Read ID : 0x%x\n", Strata_CheckID(targetAddress));
        Uart_Printf("Identification check error !!\n");
        return ;
    }

    if ( (Strata_CheckDevice(targetAddress) & 0xffff) != 0x0018 )   // Device number=0x0018
    {
        Uart_Printf("Device check error !!\n");
        return ;
    }

    Uart_Printf("\nErase the sector : 0x%x.\n", targetAddress);

    for(i=0;i<targetSize;i+=0x20000)
    {
        Strata_EraseSector(targetAddress+targetOffset+i);
    }
    
    if(!Strata_BlankCheck(targetAddress+targetOffset,targetSize))
    {
        Uart_Printf("Blank Check Error!!!\n");
        return;
    }

    Uart_Printf("\nStart of the data writing...\n");

    for (i=0; i<targetSize; i+=4) 
    {
        Strata_ProgFlash(i+targetAddress+targetOffset, *((U32 *)(srcAddress+i)));
        if(i%0x10000==0xfffc)
            Uart_Printf("[%x]",(i+4)/0x10000);
    }
    Uart_Printf("\nEnd of the data writing \n");

    _RESET();

    Uart_Printf("Verifying Start...\n");
    for (i=0; i<targetSize; i+=4) 
    {
        if (*((U32 *)(i+targetAddress+targetOffset)) !=*((U32 *)(srcAddress+i))) 
        {
            Uart_Printf("verify error  src %08x = %08x\n", srcAddress+i, *((U32 *)(srcAddress+i)));
            Uart_Printf("verify error  des %08x = %08x\n", i+targetAddress+targetOffset, *((U32 *)(i+targetAddress)));
            return;
        }
    }
    Uart_Printf("Verifying End!!!");
}


void Erase28F128J3A(void)  // added by junon 10/29
{
// FlashROM write program must reside at RAM region NOT ROM region
// In reading and writing all interrupts are disabled because the flash ROM
// strongly dislike to be disturbed by other stuff.
// And the region of flash ROM must be I/O region which means NO cacheable
// and NO bufferable in MMU. Check it out !!!
// 2001.6.18. Mon. It's local rain. I'll hope it eliminates the drought in Korea. by chc

//    unsigned long interrupt_reservoir;
    int i;

    Uart_Printf("\n[ 28F128J3A Flash Writing Program ]\n\n");
    Uart_Printf("     *** Very Important Notes ***\n");
    Uart_Printf("1. 28F128J3A must be located at 0x08000000.\n"
            " J1:1-2, J2:2-3, J3:2-3, J4:1-2 \n");
    Uart_Printf("2. After programming, 28F128J3A may be located at 0x0.\n"
			 " J1:2-3, J2:1-2, J3:1-2, J4:2-3 \n");

    rINTMSK = BIT_ALLMSK;   
    targetAddress=TARGET_ADDR_28F128;
    targetSize=downloadProgramSize;
    //downloadAddress=0x31000000;

    if(targetSize==0)
    {
        Uart_Printf("\nThe data must be downloaded using ICE or USB from 0x31000000\n");
        srcAddress=downloadAddress; 
    }
    else
    { 
        srcAddress=downloadAddress+4; //to discard the data head for the size
    }

    InputAddresses(); //srcAddress,targetSize,targetOffset will be determined.      
    Uart_Printf("Source base address(0x31000000) = 0x%x\n",srcAddress);
    Uart_Printf("Target base address(0x08000000) = 0x%x\n",targetAddress);
    Uart_Printf("Target offset      (0x0)        = 0x%x\n",targetOffset);
    Uart_Printf("Target size        (0x20000*n)  = 0x%x\n",targetSize);

    if ( Strata_CheckID(targetAddress)  != 0x00890089 )       // ID number = 0x0089
    {
		Uart_Printf("Read ID : 0x%x\n", Strata_CheckID(targetAddress));
        Uart_Printf("Identification check error !!\n");
        return ;
    }

    if ( Strata_CheckDevice(targetAddress) != 0x00180018 )   // Device number=0x0018
    {
        Uart_Printf("Device check error !!\n");
        return ;
    }
		
#if 1 // lock bit setting test 
//	Strata_SetBlockLock(targetAddress+0x10000); //just test
    for(i=0;i<10;i++)
		Uart_Printf("%d block value is %d\n", i, Strata_CheckBlockLock(targetAddress+0x20000*i)&0x00010001);
//	    if ( Strata_CheckBlockLock(targetAddress+0x20000*i) != 0x0 )   // Device number=0x0018
//	        Uart_Printf("%d block is locked !!\n", i);
	Strata_Unlock(targetAddress);
	for(i=0;i<10;i++)
	Uart_Printf("%d block value is %d\n", i, Strata_CheckBlockLock(targetAddress+0x20000*i)&0x00010001);
#endif
//	Strata_Unlock(targetAddress);

    Uart_Printf("\nErase the sector : 0x%x.\n", targetAddress);

    for(i=0;i<targetSize;i+=0x20000)
    {
        Strata_EraseSector(targetAddress+targetOffset+i);
    }
    
    if(!Strata_BlankCheck(targetAddress+targetOffset,targetSize))
    {
        Uart_Printf("Blank Check Error!!!\n");
        return;
    }

}


//==========================================================================================
static void InputAddresses(void)
{
    Uart_Printf("\n[ 28F128J3A Writing Program ]\n");

    Uart_Printf("\nSource size [0x?] : 0h~%xh\n",downloadProgramSize);
    Uart_Printf("\nAvailable Target Offset Address [0x?] : \n"); 
    Uart_Printf("0h,20000h,40000h, ..., 1ce0000h\n");
    Uart_Printf("Input target address offset [0x?] : ");
    targetOffset=Uart_GetIntNum();
    if(targetSize==0)
    {
        Uart_Printf("Input target size [0x?] : ");
        targetSize=Uart_GetIntNum();
    }
}
