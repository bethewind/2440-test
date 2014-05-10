/************************************************ 
  NAME    : CPUSPEED.C
  DESC	  : Analyze where the speed bottleneck is.
  	    1) the code runs only on the cache.
  Revision: 2001.5.17: purnnamu: draft
  Revision: 2003.3.xx: DonGo: modified for 5410.
 ************************************************/
#include <string.h>
#include "def.h"
#include "option.h"

#include "2440addr.h"
#include "2440lib.h"

#include "mmu.h"
#include "cpuspeed.h"

void CpuSpeedFunc1(void);
void CpuSpeedFunc2(void);

#define	LED_DISPLAY(data)	    (rGPFDAT = (rGPFDAT & ~(0xf<<4)) | ((~data & 0xf)<<4))


#define	WHICH_CPU	2440	// 2440.


#define	TEST_STADDR		(0x30f00000)
#define	TEST_ENDADDR	(TEST_STADDR+0xff)	//256 bytes



void Test_CpuSpeed(void)
{
    int i,base;//,j
    U32 uLockPt,bypass;

	// added for testing 2440.
    Uart_Printf("[CPU Core Speed Test]\n");

	// Set MMU enable and on/off I/D-cache.
	Uart_Printf("[MMU enable]\n");
	MMU_EnableMMU();
    Uart_Printf("[ICache enable]\n");
	MMU_EnableICache();
    Uart_Printf("[DCache enable]\n");
	MMU_EnableDCache(); //DCache should be turned on after MMU is turned on.

    //Uart_Printf("[FCLK:HCLK:PCLK] = [%d:%d:%d]MHz\n", FCLK/1000000, HCLK/1000000, PCLK/1000000);
    Uart_Printf("DCache locked area: %xH~%xH\n", TEST_STADDR, TEST_ENDADDR);
    Uart_Printf("ICache locked area: %x~%x(256B boundary)\n",
    	(U32)CpuSpeedFunc1,(U32)CpuSpeedFunc2);
    Uart_Printf("LCD is disabled.\n");
    //LCD_DisplayControl(0);
    rLCDCON1&=~1; // ENVID=OFF
    LED_DISPLAY(0x1);	// LED 1

	Uart_Printf("<<LED status>>\n");
	Uart_Printf("(1) LED4 blink: R/W OK.\n");
	Uart_Printf("(2) LED4 OFF: Multiply error(R/W OK).\n");
	Uart_Printf("(3) LED4/7 ON: R/W Error.\n");
	
 	Uart_Printf("Cache lock-down.\n");
	
	
    //========== ICache lock-down ==========
    MMU_SetICacheLockdownBase(10<<26);  	// The following code will be filled between cache line 10~63.
    base=10;
    bypass=1;
    uLockPt=(U32)CpuSpeedFunc1&0xffffffe0;

    for(;uLockPt<(U32)CpuSpeedFunc2;uLockPt+=0x20)
    {
        if(((uLockPt%0x100)==0)&&(uLockPt>(U32)CpuSpeedFunc1)) base++;
       
		MMU_InvalidateICacheMVA(uLockPt);

        if(bypass==1) MMU_SetICacheLockdownBase(base<<26);  
	
		MMU_PrefetchICacheMVA(uLockPt);

    	if(bypass==1) //to put the current code outside base 9
    	{
    	    bypass=0;
    	    base=0;
            uLockPt-=0x20; //restore uLockPt
    	}
    }
    base++;
    MMU_SetICacheLockdownBase(base<<26);  // 256

    if(base>10)
    	Uart_Printf("ERROR:ICache lockdown base overflow\n");
    
    Uart_Printf("lockdown ICache line=0~%d\n",base-1);


    //========== DCache lock-down ==========
    base=0;
    uLockPt=(U32)CpuSpeedFunc1&0xffffffe0;

    //Function should be cached in DCache because of the literal pool(LDR Rn,=0xxxxx). ??
    for(;uLockPt<(U32)CpuSpeedFunc2;uLockPt+=0x20)
    {
    	if(((uLockPt%0x100)==0)&&(uLockPt>(U32)CpuSpeedFunc1))
    	    base++;
		
    	MMU_CleanInvalidateDCacheMVA(uLockPt);

        MMU_SetDCacheLockdownBase(base<<26);  
	    *((volatile U32 *)(uLockPt));
    }
    base++;
    MMU_SetDCacheLockdownBase(base<<26);  


    for(i=TEST_STADDR;i<TEST_ENDADDR;i+=4)*((U32 *)i)=0x55555555;
    
    for(i=0;i<0x100;i+=0x20)
    {
      	MMU_CleanInvalidateDCacheMVA(TEST_STADDR+i);

        MMU_SetDCacheLockdownBase(base<<26);  
        *((volatile U32 *)(TEST_STADDR+i));
    }

    base++;
    MMU_SetDCacheLockdownBase(base<<26);  


   	
    Uart_Printf("lockdown DCache line=0~%d\n",base-1);


    //========== Check the line is really cache-filled ==========
//#if 1
    //for(uLockPt=(U32)CpuSpeedFunc1;uLockPt <= (U32)CpuSpeedFunc2-4*8;uLockPt+=4)
    //{
	//*((U32 *)uLockPt)=0xffffffff; //*((U32 *)uLockPt);
	//*((U32 *)uLockPt)=*((U32 *)uLockPt);
   // }
//#endif */   	
// SDRAM Self refresh
	
	LED_DISPLAY(0x2);

	//Uart_Getch();
		// Set clock out pad.
	rGPHCON = (rGPHCON & ~(3<<20)) | (2<<20);	// GPH10  = CLKOUT1.
	rMISCCR = (rMISCCR & ~(7<<8)) | (2<<8);		// CLKOUT1 = FCLK
    CpuSpeedFunc1();

}    



void CpuSpeedFunc1(void)
{
    int i,j;
    i=0;

	
    for(i=0;i<10;i++) {
		LED_DISPLAY(i%16);
		for(j=0;j<600000;j++);
    }
				
	//The following code should not use the stack memory.
    // because the stack memory is not DCache-locked.
    // It's should be checked using disassembly code.

	#if 1
    // Set clock frequency.
	ChangeClockDivider(14,12);
	
//	ChangeMPllValue(0xa1, 0x3,0x1);	// FCLK=202MHz
//	ChangeMPllValue(150, 6, 0);		// FCLK=237MHz
//	ChangeMPllValue(180, 3, 0);	// FCLK=451MHz
	ChangeMPllValue(214, 3, 0);	// FCLK=523.8MHz
//	ChangeMPllValue(0xa7,0x4,0x0);	// FCLK=350MHz
//	ChangeMPllValue(0x55,0x1,0x0);	// FCLK=372MHz

//	ChangeClockDivider(14,12);		// 400:100:50
//	ChangeMPllValue(92,1,0);		// FCLK=400MHz

	UPDATE_REFRESH(133000000);		// UPDATE_REFRES(HCLK);

	#endif

	#if 0
	rREFRESH |= 1<<22;	// SDRAM1 self refresh.
	#endif
	

    LED_DISPLAY(0x0);

    while(1)	// only for test: caching area
    {
        for(i=0;i<0x100;i+=4)
            *(volatile U32 *)(TEST_STADDR+i)=0x12345678*i+i;	// Write data.

        for(i=0;i<0x100;i+=4) {
            if(*(volatile U32 *)(TEST_STADDR+i)!=0x12345678*i+i) {	// Error
		        LED_DISPLAY(0x5);
		        while(1);
            }
			
            *(volatile U32 *)(TEST_STADDR+i)=0x0;	// Clear memory.
        }

    	LED_DISPLAY(0x0);
	
        i=0;
        i++;
        i=i*0x12345678;	// i=1.
	if(i==0x12345678) LED_DISPLAY(0x1);	//OK.
	else LED_DISPLAY(0x0);
    }    
}

void CpuSpeedFunc2(void){}



