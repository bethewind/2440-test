//====================================================================
// File Name : K9k2g16.c
// Function  : S3C2440 16-bit interface Nand Test program(this program used by nand.c).
// Date      : May xx, 2003
// Version   : 0.0
// History
//   R0.0 (200305xx): Modified for 2440 from 2410. -> DonGo
//====================================================================


/**************** K9s1206 NAND flash ********************/
// 1block=(NF8_PAGE_BYTE+16)bytes x 32pages
// 4096block
// Block: A[23:14], Page: [13:9]
/**************** K9K2G16 NAND flash *******************/
// 1block=(2048+64)bytes x 64pages
// 2048block
// Block: A[23:14], page: [13:9]
/*****************************************************/

#include <string.h>
#include <stdlib.h>
#include "def.h"
#include "option.h"
#include "2440addr.h"
#include "2440lib.h"
#include "2440slib.h" 

#include "K9K2G16.h"
#include "nand.h"
#define BAD_CHECK	(0)
#define ECC_CHECK	(0)

#define C_LANG	1
#define DMA		2
#define TRANS_MODE16  2


extern void Nand_Reset(void);

extern U32 srcAddress;
extern U32 targetBlock;	    // Block number (0 ~ 4095)
extern U32 targetSize;	    // Total byte size 

extern U32 downloadAddress;
extern U32 downloadProgramSize;

U32 NF16_Spare_Data[32/2];
volatile int NFConDone16;


void __irq NFCon_Int16(void);


void * n16_func[][2]=
{
//	(void *)Nand_Reset,				"NF reset            ",
	(void *)Test_NF16_Block_Erase,	"Block erase         ",
	(void *)Test_NF16_Page_Read,	"Page read           ",
	(void *)Test_NF16_Page_Write,	"Page write          ",
	(void *)Test_NF16_Rw,			"Nand R/W test       ",
	(void *)NF16_Program,			"K9K2G16_Program     ",
	(void *)NF16_Print_Id,			"Read ID             ",
	(void *)K9K2G16_PrintBadBlockNum,			"Check bad block     ",
	(void *)Test_NF16_Lock,         "Nand Block lock test",
	(void *)Test_NF16_SoftUnLock,         "Soft Unlock test      ",
	0,0
};




void Test_K9K2G16(void)
{
	int i;
//	U32 blocknum, pagenum;

	Uart_Printf("\nK9K2G16 Nand flash test start.\n");

	NF16_Init();
	
	while(1) {
		Print16SubMessage();
		Uart_Printf("\nSelect(-1 to exit): ");
		i = Uart_GetIntNum();
		//Uart_Printf("IN:%d.\n\n", i);
		if(i==-1) break;
		
		if(i>=0 && (i<(sizeof(n16_func)/8)) ) 
	    	( (void (*)(void)) (n16_func[i][0]) )();	// execute selected function.
	}

}

void Print16SubMessage(void)
{
	int i;
	
	i=0;	
	Uart_Printf("\n\n");
	while(1)
	{   //display menu
		Uart_Printf("%2d:%s",i,n16_func[i][1]);
	    i++;
	    if((int)(n16_func[i][0])==0)
	    {
			Uart_Printf("\n");
			break;
	    }
	    if((i%4)==0) Uart_Printf("\n");
	}
}



void NF16_Print_Id(void)
{
	U32 id;
	U8 maker, device;//, id4th;

//	NF16_Init();
	
	id = NF16_CheckId();
	device = (U8)id;
	maker = (U8)(id>>8);
	
	Uart_Printf("Maker:%x, Device:%x\n", maker, device);
}



void Test_NF16_Block_Erase(void)
{
	U32 block=0;

	Uart_Printf("SOP(K9K2G160UM) NAND Block erase\n");

	Uart_Printf("Block # to erase: ");
	block = Uart_GetIntNum();

	if(NF16_EraseBlock(block)==FAIL) return;

	Uart_Printf("%d-block erased.\n", block);

}

void Test_NF16_Lock(void)
{
       U32 num;
       U32 S_block, E_block;
	Uart_Printf("SOP(K9K2G16U0M) NAND Lock Test !!!\n");
	Uart_Printf("Select Lock type, Softlock(1)/Lock-tight(2) : ");

	num=Uart_GetIntNum();;
			
	Uart_Printf("\nEnter programmable start block address ");
	S_block = Uart_GetIntNum();
	Uart_Printf("Enter programmable end block address ");
       E_block = Uart_GetIntNum();

	rNFSBLK=(S_block<<6);
	rNFEBLK=(E_block<<6);

	
	if(num==1){
		rNFCONT|=(1<<12);
              Uart_Printf("Software Locked\n ");
	}
	if(num==2){
		rNFCONT|=(1<<13);
              Uart_Printf("Lock-tight: To clear Lock-tight, reset S3C2440!!!\n ");
	}
      Uart_Printf("%d block ~ %d block are Programmable\n ", S_block, (E_block-1));
}


void Test_NF16_SoftUnLock(void)
{
//       U32 S_block, E_block;
	Uart_Printf("SOP(K9K2G16U0M) NAND SoftUnLock Test !!!\n");
	
	rNFSBLK=0x0;
	rNFEBLK=0x0;

	rNFCONT&=~(1<<12);
	if(rNFCONT&(1<<13)){
	   rNFCONT&=~(1<<13);
	   Uart_Printf("Lock-tight\n ");
	   Uart_Printf("You can't unlock Protected blocks !!!\n ");
	   Uart_Printf("%d block ~ %d block are Programmable\n ", (rNFSBLK>>6), ((rNFEBLK>>6)-1));
	}
       else Uart_Printf("All blocks are Programmable\n ");
}


void Test_NF16_Page_Read(void)
{
	U32 block=0, page=0;
	U32 i;

       unsigned int * srcPt;
	srcPt=(unsigned int *)0x31000000;
		
	Uart_Printf("SOP(K9K2G16U0M) NAND Page Read.\n");

	Uart_Printf("Block # to read: ");
	block = Uart_GetIntNum();
	Uart_Printf("Page # to read: ");
	page = Uart_GetIntNum();

	if(NF16_ReadPage(block, page, srcPt)==FAIL) {
		Uart_Printf("Read error.\n");
	} else {
		Uart_Printf("Read OK.\n");
	};

	// Print data.
	Uart_Printf("Read data(%d-block,%d-page)\n", block, page);
	for(i=0; i<512; i++) {
		if((i%4)==0) Uart_Printf("\n%2x: ", i);
		Uart_Printf("%08x ", *srcPt++);
	}
	Uart_Printf("\n");
	Uart_Printf("Spare:");
	for(i=0; i<16; i++) {
	 	Uart_Printf("%08x ", NF16_Spare_Data[i]);
	}
	Uart_Printf("\n");

}



void Test_NF16_Page_Write(void)
{
	U32 block=0, page=0;
	int i, offset;
	unsigned int *srcPt;
	srcPt=(unsigned int *)0x31100000;
	
		
	Uart_Printf("SOP(K9K2G16U0M) NAND Page Write.\n");

	Uart_Printf("Block # to write: ");
	block = Uart_GetIntNum();
	Uart_Printf("Page # to write: ");
	page = Uart_GetIntNum();
	Uart_Printf("offset data(-1:random): ");
	offset = Uart_GetIntNum();

#if ADS10==TRUE	
	srand(0);
#endif

	
	// Init wdata.
	for(i=0; i<512; i++) {
	#if ADS10==TRUE
		if(offset==-1) *srcPt++ = rand()%0xffffffff;
	#else
		if(offset==-1) *srcPt++ = i;
	#endif
		else *srcPt++ =i+offset;
	}
     
       srcPt=(unsigned int *)0x31100000;
	Uart_Printf("Write data[%d block, %d page].\n", block, page);
	if(NF16_WritePage(block, page, srcPt)==FAIL) {
		Uart_Printf("Write Error.\n");
	} else {
		Uart_Printf("Write OK.\n");
	
		Uart_Printf("Write data is");
		for(i=0; i<512; i++) {
			if((i%4)==0) Uart_Printf("\n%2x: ", i);
			Uart_Printf("%08x ", *srcPt++);
		}
		Uart_Printf("\n");

		Uart_Printf("Spare:");
		for(i=0; i<32/2; i++) {
		 	Uart_Printf("%08x ", NF16_Spare_Data[i]);
			}
		Uart_Printf("\n\n");
	}
}



void Test_NF16_Rw(void)
{
	U32 block=0, page=0;
	U32 i, error;//, offset; //, status=FAIL
	int offset;
	unsigned int *srcPt, *dstPt;
	srcPt=(unsigned int *)0x31100000;
	dstPt=(unsigned int *)0x31200000;
		
	Uart_Printf("SOP(K9K2G16U0M) NAND Flash R/W test.\n");

	Uart_Printf("Block number: ");
	block = Uart_GetIntNum();
	Uart_Printf("Page nember: ");
	page = Uart_GetIntNum();
	Uart_Printf("offset data(-1:random): ");
	offset = Uart_GetIntNum();

#if ADS10==TRUE 
	srand(0); 
#endif
	
	// Init R/W data.
	for(i=0; i<512; i++) *dstPt++=0x0;

	for(i=0; i<512; i++) {
	#if ADS10==TRUE	
		if(offset==-1) *srcPt++= rand()%0xffffffff;
	#else
		if(offset==-1) *srcPt++= i;
	#endif
		else *srcPt++ = i+offset;
	}

       srcPt=(unsigned int *)0x31100000;
	dstPt=(unsigned int *)0x31200000;
	// Block erase
	Uart_Printf("%d block erase.\n", block);
	if(NF16_EraseBlock(block)==FAIL) return;

	Uart_Printf("Write data[%d block, %d page].\n", block, page);
	if(NF16_WritePage(block, page, srcPt)==FAIL) return;
	
	Uart_Printf("Read data.\n");
	if(NF16_ReadPage(block, page, dstPt)==FAIL) return;

	Uart_Printf("Checking data.\n");
	for(error=0, i=0; i<512; i++) {
		if(*srcPt++!=*dstPt++) {
			Uart_Printf("Error:%d[W:%x,R:%x]\n", i, *srcPt, *dstPt);
			error++;
		}
	}
	if(error!=0) 
		Uart_Printf("Fail to R/W test(%d).\n", error);
	else
		Uart_Printf("R/W test OK.\n");
}



void NF16_Program(void)
{
    int i;//, page_num;
    int programError=0;
    U32 *srcPt,*saveSrcPt;
    U32 blockIndex;

    Uart_Printf("\n[SOP(K9K2G16U0M) NAND Flash writing program]\n");
    Uart_Printf("The program buffer: 0x30100000~0x31ffffff\n");

//     NF16_Init();

     rINTMSK = BIT_ALLMSK; 	
     srcAddress=0x30100000; 

     InputTargetBlock16();

     srcPt=(U32 *)srcAddress;
     blockIndex=targetBlock;

     while(1) {
             saveSrcPt=srcPt;	
              
     	#if BAD_CHECK
     	  if(NF16_IsBadBlock(blockIndex)==FAIL) {
		    blockIndex++;   // for next block
		    continue;
	  }
     	#endif
		
     	if(NF16_EraseBlock(blockIndex)==FAIL) {
		    blockIndex++;   // for next block
		    continue;
     	}
   	// After 1-Block erase, Write 1-Block(64 pages).
	for(i=0;i<64;i++) {
		    if(NF16_WritePage(blockIndex,i,srcPt)==FAIL) {// block num, page num, buffer
		        programError=1;
		        break;
		    }

	#if ECC_CHECK
		    if(NF16_ReadPage(blockIndex,i,srcPt)==FAIL) {
			Uart_Printf("ECC Error(block=%d,page=%d!!!\n",blockIndex,i);
		    }
	#endif

	srcPt+=512;	// Increase buffer addr one pase size
    	if(i==0)  Uart_Printf(".");
	if((U32)srcPt>=(srcAddress+targetSize)) // Check end of buffer
	break;	// Exit for loop
    }
		
	if(programError==1) {
	    blockIndex++;
	    srcPt=saveSrcPt;
	    programError=0;
	    continue;
	}

	if((U32)srcPt>=(srcAddress+targetSize)) break;	// Exit while loop

		blockIndex++;

    }

}


void K9K2G16_PrintBadBlockNum(void)
{
    int i;
//    U32 id;

    Uart_Printf("\n[SMC(K9S1208V0M) NAND Flash bad block check]\n");
    
//    NF16_Init();

    for(i=0;i<2048;i++)
    {
	NF16_IsBadBlock(i);   // Print bad block
    }
}






//*************************************************
//*************************************************
//**           H/W dependent functions           **
//************************************************* 
//*************************************************

//The code is made for bi-endian mode

// block0: reserved for boot strap
// block1~4095: used for OS image
// badblock SE: xx xx xx xx xx 00 ....
// good block SE: ECC0 ECC1 ECC2 FF FF FF ....


#define NF_MECC_UnLock()         {rNFCONT&=~(1<<5);}
#define NF_MECC_Lock()         {rNFCONT|=(1<<5);}
#define NF_SECC_UnLock()         {rNFCONT&=~(1<<6);}
#define NF_SECC_Lock()         {rNFCONT|=(1<<6);}

#define NF_CMD(cmd)			{rNFCMD=cmd;}
#define NF_ADDR(addr)		{rNFADDR=addr;}	
#define NF_nFCE_L()			{rNFCONT&=~(1<<1);}
#define NF_nFCE_H()			{rNFCONT|=(1<<1);}
#define NF_RSTECC()			{rNFCONT|=(1<<4);}
#define NF_RDDATA() 		(rNFDATA)
#define NF_RDDATA16() 		((*(volatile unsigned short*)0x4E000010) )

#define NF_WRDATA(data) 	{rNFDATA=data;}

// RnB Signal
#define NF_CLEAR_RB()    		{rNFSTAT |= (1<<2);}	// Have write '1' to clear this bit.
#define NF_DETECT_RB()    		{while(!(rNFSTAT&(1<<2)));}	// edge detect.


#define ID_K9S1208V0M	0xec76
#define ID_K9K2G16U0M	0xecca

#define BAD_MARK	(0xffff4444)

#if 1
// HCLK=100Mhz
#define TACLS		0	// 1-clk(0ns) 
#define TWRPH0		6	// 3-clk(25ns)
#define TWRPH1		0	// 1-clk(10ns)  //TACLS+TWRPH0+TWRPH1>=50ns
#else
// HCLK=50Mhz
#define TACLS		0  //1clk(0ns)
#define TWRPH0		1  //2clk(25ns)
#define TWRPH1		0  //1clk(10ns)
#endif


static U32 se16Buf[32/2]={
	0xffffffff,0xffffffff,0xffffffff,0xffffffff,
	0xffffffff,0xffffffff,0xffffffff,0xffffffff,
	0xffffffff,0xffffffff,0xffffffff,0xffffffff,
	0xffffffff,0xffffffff,0xffffffff,0xffffffff
};


void InputTargetBlock16(void)
{
	U32 no_block, no_page, no_byte;
	
	Uart_Printf("\nSource size:0h~%xh\n",downloadProgramSize);
	Uart_Printf("\nAvailable target block number: 0~2048\n");
	Uart_Printf("Input target block number:");
    targetBlock=Uart_GetIntNum();	// Block number(0~4095)
    if(targetSize==0)
    {
    	#if 0
    	Uart_Printf("Input target size(0x4000*n):");
    	targetSize=Uart_GetIntNum();	// Total byte size
	#else
   	Uart_Printf("Input program file size(bytes): ");
    	targetSize=Uart_GetIntNum();	// Total byte size
    	#endif
    }
	
	no_block = (U32)((targetSize/2048)/64);
	no_page = (U32)((targetSize/2048)%64);
	no_byte = (U32)(targetSize%2048);
	Uart_Printf("File:%d[%d-block,%d-page,%d-bytes].\n", targetSize, no_block, no_page, no_byte);
}


static int NF16_EraseBlock(U32 block)
{
    U32 blockPage=(block<<6);
//    int i;

    NFConDone16=0;
    rNFCONT|=(1<<9); //Enable RnB Interrupt
    rNFCONT|=(1<<10); //Enable Illegal Access Interrupt
    pISR_NFCON= (unsigned)NFCon_Int16;
    rSRCPND=BIT_NFCON;
    rINTMSK=~(BIT_NFCON);

#if BAD_CHECK
    if(NF16_IsBadBlock(block))
	return FAIL;
#endif

	NF_nFCE_L();
    
	NF_CMD(0x60);   // Erase one block 1st command, Block Addr:A11-A27
	// Address 3-cycle
	NF_ADDR(blockPage&0xff);	    // A[18:11]
	NF_ADDR((blockPage>>8)&0xff);   // A[26:19]
	NF_ADDR((blockPage>>16)&0xff);	// A27


	NF_CLEAR_RB();
	NF_CMD(0xd0);	// Erase one blcok 2nd command

	 while(NFConDone16==0);
	 rNFCONT&=~(1<<9);   // Disable RnB Interrupt
	 rNFCONT&=~(1<<10); // Disable Illegal Access Interrupt
	 if(rNFSTAT&0x8) return FAIL;
 
	NF_CMD(0x70);   // Read status command

    if (NF_RDDATA()&0x1) // Erase error
    {	
    	NF_nFCE_H();
	Uart_Printf("[ERASE_ERROR:block#=%d]\n",block);
//	NF16_MarkBadBlock(block);
	return FAIL;
    }
    else 
    {
    	NF_nFCE_H();
        return OK;
    }
}

void __irq NFCon_Int16(void)
{
       NFConDone16=1;
	rINTMSK|=BIT_NFCON;
	ClearPending(BIT_NFCON);
	if(rNFSTAT&0x8)	Uart_Printf("Illegal Access is detected!!!\n");
//	else Uart_Printf("RnB is Detected!!!\n"); 
  }



static int NF16_IsBadBlock(U32 block)
{
//    int i;
    unsigned int blockPage;
    U32 bad_block_data;
    
    
    blockPage=(block<<6);	// For 2'nd cycle I/O[7:5] 
    
	NF_nFCE_L();

	NF_CLEAR_RB();

	NF_CMD(0x00);		 // read command
	NF_ADDR((1024+6)&0xff);			// 2060 = 0x080c
	NF_ADDR(((1024+6)>>8)&0xff);		// A[10:8]
	NF_ADDR((blockPage)&0xff);		// A[11;18]
	NF_ADDR((blockPage>>8)&0xff);	// A[26:19]
	NF_ADDR((blockPage>>16)&0xff);	// A27
	NF_CMD(0x30);	// 2'nd command
	NF_DETECT_RB();	 // Wait tR(max 12us)

    bad_block_data=NF_RDDATA();

	NF_nFCE_H();    

       if(bad_block_data!=0xffffffff)
    {
    	Uart_Printf("[ block %d has been marked as a bad block(%x)]\n",block,bad_block_data);
    	return FAIL;
    }
    else
    {
     	return OK;
    }
}


static int NF16_MarkBadBlock(U32 block)
{
    int i;
    U32 blockPage=(block<<6);
 
    se16Buf[0]=0xffffffff;
    se16Buf[1]=0xffffffff;    
    se16Buf[3]=BAD_MARK;   // Bad blcok mark=0x4444
    
	NF_nFCE_L(); 
	NF_CMD(0x80);   // Write 1st command
	
	NF_ADDR((1024+6)&0xff);			// 2060 = 0x080c
	NF_ADDR(((1024+6)>>8)&0xff);	// A[10:8]
	NF_ADDR((blockPage)&0xff);	// A[11;18]
	NF_ADDR((blockPage>>8)&0xff);	// A[26:19]
	NF_ADDR((blockPage>>16)&0xff);	// A[27]
	
	
    for(i=0;i<32/2;i++) {
		NF_WRDATA(se16Buf[i]);	// Write spare array
    }

	NF_CLEAR_RB();
	NF_CMD(0x10);   // Write 2nd command
	NF_DETECT_RB();

	NF_CMD(0x70);
    
	for(i=0;i<3;i++);  //twhr=60ns////??????
    
    if (NF_RDDATA()&0x1) // Spare arrray write error
    {	
    	NF_nFCE_H();
    	Uart_Printf("[Program error is occurred but ignored]\n");
    }
    else 
    {
    	NF_nFCE_H();
    }

    Uart_Printf("[block #%d is marked as a bad block]\n",block);
    return OK;
}


static int NF16_ReadPage(U32 block,U32 page,U32 *buffer)
{
    int i;
    unsigned int blockPage;
//    U32 ecc_low,ecc_high;
    U32 Mecc0, Mecc1, Secc;
//    U16 ecc0, ecc1, ecc2, ecc3;
    U32 *bufPt=buffer;
//    U32 se[16];	   
    
    blockPage=(block<<6)+page;
	
    NF_RSTECC();    // Initialize ECC
    NF_MECC_UnLock();
    
    NF_nFCE_L();    

    NF_CLEAR_RB();
    NF_CMD(0x00);	// Read command
    NF_ADDR(0); 	// Column (A[7:0]) = 0
    NF_ADDR(0);		// A[10:8]
    NF_ADDR((blockPage)&0xff);	// A[18:11]
    NF_ADDR((blockPage>>8)&0xff);	// A[26:19]
    NF_ADDR((blockPage>>16)&0xff);  // A27
    NF_CMD(0x30);	// 2'nd command
    NF_DETECT_RB();

#if TRANS_MODE16==C_LANG
    for(i=0;i<512;i++) {
    	*bufPt++=NF_RDDATA();	// Read one page
    }
#elif TRANS_MODE16==DMA
	// Nand to memory dma setting
    rSRCPND=BIT_DMA0;	// Init DMA src pending.
    rDISRC0=NFDATA; 	// Nand flash data register
    rDISRCC0=(0<<0) | (1<<0); //arc=AHB,src_addr=fix
    rDIDST0=(unsigned)bufPt;
    rDIDSTC0=(0<<1) | (0<<0); //dst=AHB,dst_addr=inc;
    rDCON0=((unsigned int)1<<31)|(1<<30)|(1<<29)|(1<<28)|(1<<27)|(0<<23)|(1<<22)|(2<<20)|(2048/4/4);
	//Handshake,AHB,interrupt,(4-burst),whole,S/W,no_autoreload,word,count=128;

	// DMA on and start.
    rDMASKTRIG0=(1<<1)|(1<<0);

	while(!(rSRCPND & BIT_DMA0));	// Wait until Dma transfer is done.
    rSRCPND=BIT_DMA0;
#endif

    NF_MECC_Lock();
//ECC check by software
/*  
   
   ecc_low=rNFMECC0;  // ECC 0~3 for data[7:0]
	  ecc_high=rNFMECC1;  // ECC 3rd and 4th
   #if 0
	  ecc0 = (U16)((ecc_high) <<8) | (U16)(ecc_low&0xff);
	  ecc1 = (U16)((ecc_high>>8)<<8) | (U16)((ecc_low>>8)&0xff);
	  ecc2 = (U16)((ecc_high>>16)<<8) | (U16)((ecc_low>>16)&0xff);
	  ecc3 = (U16)((ecc_high>>24)<<8) | (U16)((ecc_low>>24)&0xff);	  
   
	  ecc_low = (U32)ecc1<<16 | (U32)ecc0;
	  ecc_high = (U32)ecc3<<16 | (U32)ecc2;
   #endif
	  
	  for(i=0;i<16;i++) {
		  se[i]=NF_RDDATA();  // Read spare array with 4byte width
		  NF16_Spare_Data[i]=se[i];
	  }
	  
	  //__RdPage528(pPage);
	  NF_nFCE_H();	  
   
	  if(ecc_low==se[0] && ecc_high==se[1]) {
		   return OK;
	  } else {
		  Uart_Printf("[ECC ERROR(RD):read:%x,%x, reg:%x,%x]\n",
		  se[0],se[1],ecc_low,ecc_high);
		  return FAIL;
	  } 	  
*/	  

 // ECC check by hardware
  
   NF_SECC_UnLock();
   Mecc0=NF_RDDATA();
   Mecc1=NF_RDDATA();
   NF_SECC_Lock();
   
   rNFMECCD0=((Mecc1&0xff00)<<16)|((Mecc0&0xff00)<<8)|((Mecc1&0xff)<<8)|((Mecc0&0xff));
   rNFMECCD1=(Mecc1&0xff000000)|((Mecc0&0xff000000)>>8)|((Mecc1&0xff0000)>>8)|((Mecc0&0xff0000)>>16);
  
   NF16_Spare_Data[0]=Mecc0;
   NF16_Spare_Data[1]=Mecc1;
   NF16_Spare_Data[2]=NF_RDDATA();
   NF16_Spare_Data[3]=NF_RDDATA();
   
//   Uart_Printf("NFSECC:0x%x\n", rNFSECC);
   Secc=NF_RDDATA();
   rNFSECCD=(Secc&0xff000000)|((Secc&0xff0000)>>8)|((Secc&0xff00)<<8)|(Secc&0xff);
  
   NF16_Spare_Data[4]=Secc;

   for(i=5;i<16;i++) {
    	NF16_Spare_Data[i]=NF_RDDATA();	// Read spare array with 4byte width
       }
  
    NF_nFCE_H();    

   if (((rNFESTAT0&0xf) == 0x0) &&((rNFESTAT1&0xf) == 0x0)){
       Uart_Printf("ECC OK !!!\n");
       return OK;
       }
    else {
	Uart_Printf("ECC FAIL !!!\n");
	return FAIL;
    	}

 
}



static int NF16_WritePage(U32 block,U32 page,U32 *buffer)
{
    int i;
    U32 blockPage, Secc;//,Mecc0, Mecc1
    U32 *bufPt=buffer;
	
	blockPage=(block<<6)+page;
	
	NF_RSTECC();	// Initialize ECC
       NF_MECC_UnLock();
	   
	NF_nFCE_L(); 
	NF_CMD(0x80);   // Write 1st command
	NF_ADDR(0); 	// Column (A[7:0]) = 0
	NF_ADDR(0);		// A[10:8]
	NF_ADDR((blockPage)&0xff);	// A[18:11]
	NF_ADDR((blockPage>>8)&0xff);	// A[26:19]
	NF_ADDR((blockPage>>16)&0xff);  // A7

	// Write page data.
	
#if TRANS_MODE16==C_LANG
	for(i=0;i<512;i++) {
		NF_WRDATA(*bufPt++);	// Write one page to NFM from buffer
	}
#elif TRANS_MODE16==DMA
	// Memory to Nand dma setting
	rSRCPND=BIT_DMA0;	// Init DMA src pending.
	rDISRC0=(unsigned)bufPt;	// Nand flash data register
	rDISRCC0=(0<<1) | (0<<0); //arc=AHB,src_addr=inc
	rDIDST0=NFDATA;
	rDIDSTC0=(0<<1) | (1<<0); //dst=AHB,dst_addr=fix;
	rDCON0=((unsigned int)1<<31)|(1<<30)|(1<<29)|(0<<28)|(1<<27)|(0<<23)|(1<<22)|(2<<20)|(2048/4);
	//Handshake,AHB,interrupt,unit,whole,S/W,no_autoreload,word,count=128;

	// DMA on and start.
	rDMASKTRIG0=(1<<1)|(1<<0);

	while(!(rSRCPND & BIT_DMA0));	// Wait until Dma transfer is done.
	rSRCPND=BIT_DMA0;	
#endif
/*    
     NF_MECC_Lock();
// Get ECC data.
    se16Buf[0]=rNFMECC0;	// Ecc lower
    se16Buf[1]=rNFMECC1;	// Ecc high
    se16Buf[3]=0xffffffff;	// Marking good block(lower 16-bit is valid)

	//Write extra data(ECC, bad marking)
	for(i=0;i<16;i++) {
		NF_WRDATA(se16Buf[i]);	// Write spare array(ECC and Mark)
		NF16_Spare_Data[i]=se16Buf[i];
    }  

 	NF_CLEAR_RB();
	NF_CMD(0x10);	 // Write 2nd command
	NF_DETECT_RB();
 
	NF_CMD(0x70);   // Read status command   
    
	for(i=0;i<3;i++);  //twhr=60ns
    
    if (NF_RDDATA()&0x1) {// Page write error
    	NF_nFCE_H();
		Uart_Printf("[PROGRAM_ERROR:block#=%d]\n",block);
		NF16_MarkBadBlock(block);
		return FAIL;
    } else {
    	NF_nFCE_H();
		return OK;
	}
	*/


  NF_MECC_Lock();
//   Uart_Printf("NFMECC0:0x%x, NFMECC1:0x%x\n", rNFMECC0,rNFMECC1);
   se16Buf[0]=rNFMECC0;
   se16Buf[1]=rNFMECC1;
   se16Buf[3]=0xffffffff;	// Marking good block(lower 16-bit is valid)

    
  // check spare ecc
    NF_SECC_UnLock();
    for(i=0;i<4;i++) {
		NF_WRDATA(se16Buf[i]);	// Write spare array(ECC and Mark)
		NF16_Spare_Data[i]=se16Buf[i];
      }  
    NF_SECC_Lock();
 //   Uart_Printf("NFSECC:0x%x\n", rNFSECC);
    Secc=rNFSECC;
    se16Buf[4]=Secc;
   
    for(i=4;i<16;i++) {
		NF_WRDATA(se16Buf[i]);	// Write spare array(ECC and Mark)
		NF16_Spare_Data[i]=se16Buf[i];
      }    
    NF_CLEAR_RB();
    NF_CMD(0x10);	 // Write 2nd command
#if 1
       NF_DETECT_RB();
#else
       while(!((rNFSTAT&0x4)||(rNFSTAT&0x8)));
	
	if(rNFSTAT&0x8){
		Uart_Printf("Illegal Access is detected!!!\n");
              return FAIL;
	}
#endif
	NF_CMD(0x70);   // Read status command   
    
	for(i=0;i<3;i++);  //twhr=60ns
    
    if (NF_RDDATA()&0x1) {// Page write error
    	NF_nFCE_H();
		Uart_Printf("[PROGRAM_ERROR:block#=%d]\n",block);
		NF16_MarkBadBlock(block);
		return FAIL;
    } else {
    	NF_nFCE_H();
		return OK;
	}


}



static U32 NF16_CheckId(void)
{
    int i;
    U32 id;
    U16 id1, id2, id3, id4;
    
	NF_nFCE_L();

	NF_CLEAR_RB();
	NF_CMD(0x90);
	NF_ADDR(0x0);
	for (i=0; i<10; i++);

//read by halfword !!!
	id1 = NF_RDDATA16();	// read 4byte.
	id2 = NF_RDDATA16();
	id3 = NF_RDDATA16();
	id4 = NF_RDDATA16();

	id=((id4<<24)|(id3<<16)|(id2<<8)|id1);
	
 	NF_nFCE_H();

    return id;
}



static void NF16_Init(void)
{
	// for S3C2440

	rNFCONF = (TACLS<<12)|(TWRPH0<<8)|(TWRPH1<<4)|(1<<0);	
	// TACLS		[14:12]	CLE&ALE duration = HCLK*TACLS.
	// TWRPH0		[10:8]	TWRPH0 duration = HCLK*(TWRPH0+1)
	// TWRPH1		[6:4]	TWRPH1 duration = HCLK*(TWRPH1+1)
	// AdvFlash(R)	[3]		Advanced NAND, 0:256/512, 1:1024/2048
	// PageSize(R)	[2]		NAND memory page size
	//						when [3]==0, 0:256, 1:512 bytes/page.
	//						when [3]==1, 0:1024, 1:2048 bytes/page.
	// AddrCycle(R)	[1]		NAND flash addr size
	//						when [3]==0, 0:3-addr, 1:4-addr.
	//						when [3]==1, 0:4-addr, 1:5-addr.
	// BusWidth(R/W) [0]	NAND bus width. 0:8-bit, 1:16-bit.
	
	rNFCONT = (0<<13)|(0<<12)|(0<<10)|(0<<9)|(0<<8)|(1<<6)|(1<<5)|(1<<4)|(1<<1)|(1<<0);
	// Lock-tight	[13]	0:Disable lock, 1:Enable lock.
	// Soft Lock	[12]	0:Disable lock, 1:Enable lock.
	// EnablillegalAcINT[10]	Illegal access interupt control. 0:Disable, 1:Enable
	// EnbRnBINT	[9]		RnB interrupt. 0:Disable, 1:Enable
	// RnB_TrandMode[8]		RnB transition detection config. 0:Low to High, 1:High to Low
	// SpareECCLock	[6]		0:Unlock, 1:Lock
	// MainECCLock	[5]		0:Unlock, 1:Lock
	// InitECC(W)	[4]		1:Init ECC decoder/encoder.
	// Reg_nCE		[1]		0:nFCE=0, 1:nFCE=1.
	// NANDC Enable	[0]		operating mode. 0:Disable, 1:Enable.

//	rNFSTAT = 0;
    
//    Nand_Reset();
}




