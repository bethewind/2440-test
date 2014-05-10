//====================================================================
// File Name : Nand.c
// Function  : S3C2440 8-bit interface Nand Test program(this program use K9k2g16.c).
// Date      : May xx, 2003
// Version   : 0.0
// History
//   R0.0 (200305xx): Modified for 2440 from 2410. -> DonGo
//====================================================================


/**************** K9s1208 NAND flash ********************/
// 1block=(512+16)bytes x 32pages
// 4096block
// Block: A[23:14], Page: [13:9]
/**************** K9K2G16 NAND flash *******************/
// 1block=(2048+64)bytes x 64pages
// 2048block
// Block: A[23:14], page: [13:9]
/*****************************************************/
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "def.h"
#include "option.h"
#include "2440addr.h"
#include "2440lib.h"
#include "2440slib.h" 

#include "Nand.h"
#include "k9k2g16.h"


#define BAD_CHECK	(0)
#define ECC_CHECK	(0)

#define ASM		1
#define C_LANG	2
#define DMA		3
#define TRANS_MODE 3

U8 NF8_Spare_Data[16];

U32 srcAddress;
U32 targetBlock;	    // Block number (0 ~ 4095)
U32 targetSize;	    // Total byte size 

extern U32 downloadAddress;
extern U32 downloadProgramSize;
volatile int NFConDone;

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

//=============    spare area configuration    ====================================
//     0		1		2		3		4		5		6		7		8		9		0xa ...	
//  Mecc0   Mecc1     Mecc2         FF           FF      Bad Block     FF           FF        Secc0     Secc1          FF
//====================================================================


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
#define NF_RDDATA8() 		((*(volatile unsigned char*)0x4E000010) )
#define NF_WRDATA(data) 	{rNFDATA=data;}
#define NF_WRDATA8(data) 	{rNFDATA8=data;}

// RnB Signal
#define NF_CLEAR_RB()    		{rNFSTAT |= (1<<2);}	// Have write '1' to clear this bit.
#define NF_DETECT_RB()    		{while(!(rNFSTAT&(1<<2)));}

#define ID_K9S1208V0M	0xec76
#define ID_K9K2G16U0M	0xecca

// HCLK=100Mhz
#define TACLS		7	// 1-clk(0ns) 
#define TWRPH0		7	// 3-clk(25ns)
#define TWRPH1		7	// 1-clk(10ns)  //TACLS+TWRPH0+TWRPH1>=50ns


static U8 se8Buf[16]={
	0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff
};
/*
static U32 se16Buf[32/2]={
	0xffffffff,0xffffffff,0xffffffff,0xffffffff,
	0xffffffff,0xffffffff,0xffffffff,0xffffffff,
	0xffffffff,0xffffffff,0xffffffff,0xffffffff,
	0xffffffff,0xffffffff,0xffffffff,0xffffffff
};
*/
void __irq NFCon_Int(void);


//*************************************************



void * n8_func[][2]=
{
	(void *)NF8_Print_Id,			"Read ID         ",
	(void *)Nand_Reset,				"Nand reset      ",
	(void *)Test_NF8_Block_Erase,	"Block erase     ",
	(void *)Test_NF8_Page_Read,		"Page read       ",
	(void *)Test_NF8_Page_Write,	"Page write      ",
	(void *)Test_NF8_Rw,			"Nand R/W test   ",
	(void *) NF8_PrintBadBlockNum,	"Check Badblock  ",
	(void *)Test_NF8_Lock,			"Nand Block lock ",
	(void *)Test_NF8_SoftUnLock,	"Soft Unlock     ",
	(void *)NF8_Program,			"K9S1208 Program ",	
	0,0
};

void Test_Nand(void)
{
	U8 ch;
	U32 gpacon;
	
	Uart_Printf("Nand test\n");

	gpacon = rGPACON;

	rGPACON = (rGPACON &~(0x3f<<17)) | (0x3f<<17);
	// GPA     22         21           20      19     18     17
	//           nFCE   nRSTOUT  nFRE  nFWE   ALE   CLE

	Uart_Printf("Select Nand flash type, Normal(1)/Advanced(2) : ");

	ch=Uart_Getch();

	Uart_Printf("%c\n\n", ch);
	
	switch(ch) {
		case '1':
			Test_K9S1208();		// in Nand.c
		break;
		case '2':
			Test_K9K2G16();		// in K9K2h16.c
		break;
		default:
		break;
	}

	rGPACON = gpacon;
}


void Test_K9S1208(void)
{
	int i;
//	U8 ch;

	Uart_Printf("\nK9S1208 Nand flash test start.\n");

	NF8_Init();

	while(1) {
		PrintSubMessage();
		Uart_Printf("\nSelect(-1 to exit): ");
		i = Uart_GetIntNum();
		
		if(i==-1) break;
		
//		if(i>=0 && (i<(sizeof(n8_func)/8)) ) 
//	   	( (void (*)(void)) (n8_func[i][0]) )();	// execute selected function.
        switch (i)
		{
		    case 1:	 NF8_Print_Id();
			         break;
		    case 2:	 Nand_Reset();
			         break;
		    case 3:	 Test_NF8_Block_Erase();
			         break;
		    case 4:	 Test_NF8_Page_Read();
			         break;
		    case 5:	 Test_NF8_Page_Write();
			         break;
		    case 6:	 Test_NF8_Rw();	
			         break;
		    case 7:	 NF8_PrintBadBlockNum();
			         break;
		    case 8:	 Test_NF8_Lock();	
			         break;
		    case 9:	 Test_NF8_SoftUnLock();
			         break;
		    case 10:	 NF8_Program();
			         break;

		    default:
			         break;
		}
	}

}

U8 Read_Status(void)
{
	// Read status
	U8 ch;
	int i;
	
	NF_nFCE_L();

	NF_CMD(0x70);
	for(i=0; i<10; i++);
	ch = NF_RDDATA();

	NF_nFCE_H();

	return ch;
}

void NF8_Print_Id(void)
{
	U16 id;
	U8 maker, device;

//	NF8_Init();
	id = NF8_CheckId();
	
	device = (U8)id;
	maker = (U8)(id>>8);
	
	Uart_Printf("Maker:%x, Device:%x\n", maker, device);
}


void Test_NF8_Block_Erase(void)
{
	U32 block=0;

	Uart_Printf("SMC(K9S1208V0M) NAND Block erase\n");

	if((Read_Status()&0x80)==0) {
		Uart_Printf("Write protected.\n");
		return;
	}

	Uart_Printf("Block # to erase: ");
	block = Uart_GetIntNum();

//	NF8_Init();
	
	if(NF8_EraseBlock(block)==FAIL) return;

	Uart_Printf("%d-block erased.\n", block);

}

void Test_NF8_Page_Read(void)
{
	U32 block=0, page=0;
	U32 i;
	unsigned char * downPt;
	
	downPt=(unsigned char *)_NONCACHE_STARTADDRESS;
		
	Uart_Printf("SMC(K9S1208V0M) NAND Page Read.\n");

	Uart_Printf("Block # to read: ");
	block = Uart_GetIntNum();
	Uart_Printf("Page # to read: ");
	page = Uart_GetIntNum();

	if(NF8_ReadPage(block, page, (U8 *)downPt )==FAIL) {
		Uart_Printf("Read error.\n");
	} else {
		Uart_Printf("Read OK.\n");
	}
	// Print data.
	Uart_Printf("Read data(%d-block,%d-page)\n", block, page);
	
	for(i=0; i<512; i++) {
		if((i%16)==0) Uart_Printf("\n%4x: ", i);
		Uart_Printf("%02x ", *(U8 *)downPt++);
		}
	Uart_Printf("\n");
	Uart_Printf("Spare:");
	for(i=0; i<16; i++) {
	 	Uart_Printf("%02x ", NF8_Spare_Data[i]);
	}
	Uart_Printf("\n");

}

void Test_NF8_Page_Write(void)
{
	U32 block=0, page=0;
	int i, offset;
	unsigned char * srcPt;
	srcPt=(unsigned char *)0x31100000;
		
	Uart_Printf("SMC(K9S1208V0M) NAND Page Write.\n");
	Uart_Printf("You must erase block before you write data!!! \n");

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
		if(offset==-1) *srcPt++ = rand()%0xff;
	#else
		if(offset==-1) *srcPt++ = i%0xff;
	#endif
		else *srcPt++ =  i+offset;
	}
       srcPt=(unsigned char *)0x31100000;
	Uart_Printf("Write data[%d block, %d page].\n", block, page);
	if(NF8_WritePage(block, page, srcPt)==FAIL) {
		Uart_Printf("Write Error.\n");
	} else {
		Uart_Printf("Write OK.\n");
	}

 	Uart_Printf("Write data is");
	for(i=0; i<512; i++) {
		if((i%16)==0) Uart_Printf("\n%4x: ", i);
		Uart_Printf("%02x ", *srcPt++);
	}
	Uart_Printf("\n");

	Uart_Printf("Spare:");
	for(i=0; i<16; i++) {
	 	Uart_Printf("%02x ", NF8_Spare_Data[i]);
	}
	Uart_Printf("\n\n");

}


void Test_NF8_Rw(void)
{
	U32 block=0, page=0;
	U32 i, error;//, offset; //, status=FAIL
	int offset;
	unsigned char *srcPt, *dstPt;
	srcPt=(unsigned char *)0x31100000;
	dstPt=(unsigned char *)0x31200000;
		
	Uart_Printf("SMC(K9S1208V0M) NAND Flash R/W test.\n");

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
		if(offset == -1) *srcPt++ = rand()%0xff;
	#else
		if(offset == -1) *srcPt++ = i%0xff;
	#endif
		else *srcPt++ = i+offset;
	}
	


	srcPt=(unsigned char *)0x31100000;
	dstPt=(unsigned char *)0x31200000;
	// Block erase
	Uart_Printf("%d block erase.\n", block);
	if(NF8_EraseBlock(block)==FAIL) return;

	Uart_Printf("Write data[%d block, %d page].\n", block, page);
	if(NF8_WritePage(block, page, srcPt)==FAIL) return;
	
	Uart_Printf("Read data.\n");
	if(NF8_ReadPage(block, page, dstPt)==FAIL) return;

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


void NF8_Program(void)
{
//    unsigned long interrupt_reservoir;
    int i;
    int programError=0;
	U8 *srcPt,*saveSrcPt;
	U32 blockIndex;

  	Uart_Printf("\n[SMC(K9S1208V0M) NAND Flash writing program]\n");
       Uart_Printf("The program buffer: 0x30100000~0x31ffffff\n");

//	NF8_Init();

      rINTMSK = BIT_ALLMSK; 	
      srcAddress=0x30100000; 

      InputTargetBlock();
	
      srcPt=(U8 *)srcAddress;
      blockIndex=targetBlock;

     while(1) {
        saveSrcPt=srcPt;	

		#if BAD_CHECK
		if(NF8_IsBadBlock(blockIndex)==FAIL) {
		    blockIndex++;   // for next block
		    continue;
		}
		#endif

		if(NF8_EraseBlock(blockIndex)==FAIL) {
		    blockIndex++;   // for next block
		    continue;
		}

		// After 1-Block erase, Write 1-Block(32 pages).
		for(i=0;i<32;i++) {
		    if(NF8_WritePage(blockIndex,i,srcPt)==FAIL) {// block num, page num, buffer
		        programError=1;
		        break;
		    }

		#if ECC_CHECK
		    if(NF8_ReadPage(blockIndex,i,srcPt)==FAIL) {
				Uart_Printf("ECC Error(block=%d,page=%d!!!\n",blockIndex,i);
		    }
		#endif
			
		srcPt+=512;	// Increase buffer addr one pase size
		if(i==0)  Uart_Printf(".");
			//Uart_Printf("\b\b\b\b\b\b\b\b%04d,%02d]", blockIndex, i);
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


void InputTargetBlock(void)
{
	U32 no_block, no_page, no_byte;
	
	Uart_Printf("\nSource size:0h~%xh\n",downloadProgramSize);
	Uart_Printf("\nAvailable target block number: 0~4095\n");
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
	
	no_block = (U32)((targetSize/512)/32);
	no_page = (U32)((targetSize/512)%32);
	no_byte = (U32)(targetSize%512);
	Uart_Printf("File:%d[%d-block,%d-page,%d-bytes].\n", targetSize, no_block, no_page, no_byte);
}


void NF8_PrintBadBlockNum(void)
{
    int i;
    U16 id;

    Uart_Printf("\n[SMC(K9S1208V0M) NAND Flash bad block check]\n");
    
    id=NF8_CheckId();
    Uart_Printf("ID=%x(0xec76)\n",id);
    if(id!=0xec76)
    return;
    for(i=0;i<4096;i++)  	NF8_IsBadBlock(i);   // Print bad block
}


void Test_NF8_Lock(void)
{
       U32 num;
       U32 S_block, E_block;
	Uart_Printf("SMC(K9S1208V0M) NAND Lock Test !!!\n");
	Uart_Printf("Select Lock type, Softlock(1)/Lock-tight(2) : ");

	num=Uart_GetIntNum();;
			
	Uart_Printf("\nEnter programmable start block address ");
	S_block = Uart_GetIntNum();
	Uart_Printf("Enter programmable end block address ");
       E_block = Uart_GetIntNum();

	rNFSBLK=(S_block<<5);
	rNFEBLK=(E_block<<5);
	
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


void Test_NF8_SoftUnLock(void)
{
//       U32 S_block, E_block;
	Uart_Printf("SMC(K9S1208V0M) NAND SoftUnLock Test !!!\n");
	
	rNFSBLK=0x0;
	rNFEBLK=0x0;

	rNFCONT&=~(1<<12);
	if(rNFCONT&(1<<13)){
		rNFCONT&=~(1<<13);
		Uart_Printf("Lock-tight\n ");
		Uart_Printf("You can't unlock Protected blocks !!!\n ");
		Uart_Printf("%d block ~ %d block are Programmable\n ", (rNFSBLK>>5), ((rNFEBLK>>5)-1));
	}
       else Uart_Printf("All blocks are Programmable\n ");
}


void PrintSubMessage(void)
{
	Uart_Printf("\n\n");
	//display menu
	Uart_Printf("1 :Read ID   \n");
	Uart_Printf("2 :Nand reset  \n");
	Uart_Printf("3 :Block erase  \n");
	Uart_Printf("4 :Page read  \n");
	Uart_Printf("5 :Page write \n");
	Uart_Printf("\n");
	Uart_Printf("6 :Nand R/W test\n");
	Uart_Printf("7 :Check Badblock \n");
	Uart_Printf("8 :Nand Block lock \n");
	Uart_Printf("9 :Soft Unlock  \n");
	Uart_Printf("10:K9S1208 Program\n");

}


static int NF8_EraseBlock(U32 block)
{
	U32 blockPage=(block<<5);
//    int i;

    NFConDone=0;
    rNFCONT|=(1<<9);
    rNFCONT|=(1<<10);
    pISR_NFCON= (unsigned)NFCon_Int;
    rSRCPND=BIT_NFCON;
    rINTMSK=~(BIT_NFCON);
	
#if BAD_CHECK
    if(NF8_IsBadBlock(block))
	return FAIL;
#endif

	NF_nFCE_L();
    
	NF_CMD(0x60);   // Erase one block 1st command, Block Addr:A9-A25
	// Address 3-cycle
	NF_ADDR(blockPage&0xff);	    // Page number=0
	NF_ADDR((blockPage>>8)&0xff);   
	NF_ADDR((blockPage>>16)&0xff);


	NF_CLEAR_RB();
	NF_CMD(0xd0);	// Erase one blcok 2nd command
//	NF_DETECT_RB();
       while(NFConDone==0);
	 rNFCONT&=~(1<<9);
	 rNFCONT&=~(1<<10); // Disable Illegal Access Interrupt
	 if(rNFSTAT&0x8) return FAIL;

	NF_CMD(0x70);   // Read status command

      if (NF_RDDATA()&0x1) // Erase error
      {	
    	NF_nFCE_H();
	Uart_Printf("[ERASE_ERROR:block#=%d]\n",block);
//	NF8_MarkBadBlock(block);
	return FAIL;
       }
       else 
       {
    	NF_nFCE_H();
       return OK;
       }
}


void __irq NFCon_Int(void)
{
       NFConDone=1;
	rINTMSK|=BIT_NFCON;
	ClearPending(BIT_NFCON);
	if(rNFSTAT&0x8) Uart_Printf("Illegal Access is detected!!!\n");
//	else Uart_Printf("RnB is Detected!!!\n"); 
}


static int NF8_IsBadBlock(U32 block)
{
//       int i;
       unsigned int blockPage;
	U8 data;
    
    
       blockPage=(block<<5);	// For 2'nd cycle I/O[7:5] 
    
	NF_nFCE_L();
	NF_CLEAR_RB();

	NF_CMD(0x50);		 // Spare array read command
	NF_ADDR((512+5)&0xf);		 // Read the mark of bad block in spare array(M addr=5), A4-A7:Don't care
	NF_ADDR(blockPage&0xff);	 // The mark of bad block is in 0 page
	NF_ADDR((blockPage>>8)&0xff);	 // For block number A[24:17]
	NF_ADDR((blockPage>>16)&0xff);  // For block number A[25]
	 
	NF_DETECT_RB();	 // Wait tR(max 12us)

       data=NF_RDDATA();

	NF_nFCE_H();    

     if(data!=0xff)
     {
    	Uart_Printf("[block %d has been marked as a bad block(%x)]\n",block,data);
    	return FAIL;
     }
     else
     {
    	return OK;
     }
}


static int NF8_MarkBadBlock(U32 block)
{
       int i;
	U32 blockPage=(block<<5);
 
     se8Buf[0]=0xff;
     se8Buf[1]=0xff;    
     se8Buf[2]=0xff;    
     se8Buf[5]=0x44;   // Bad blcok mark=44
    
	NF_nFCE_L(); 
	NF_CMD(0x50);   //????
	NF_CMD(0x80);   // Write 1st command
    
	NF_ADDR(0x0);		    // The mark of bad block is 
	NF_ADDR(blockPage&0xff);	    // marked 5th spare array 
	NF_ADDR((blockPage>>8)&0xff);   // in the 1st page.
	NF_ADDR((blockPage>>16)&0xff);  //
    
	for(i=0;i<16;i++)
      {
	   NF_WRDATA(se8Buf[i]);	// Write spare array
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

static int NF8_ReadPage(U32 block,U32 page,U8 *buffer)
{
//       int i;
       unsigned int blockPage;
	U32 Mecc, Secc;
	U8 *bufPt=buffer;
//	U8 se[16], ecc0, ecc1, ecc2, ecc3,a,b,c,d,e;
    
       blockPage=(block<<5)+page;
	NF_RSTECC();    // Initialize ECC
	NF_MECC_UnLock();
    
	NF_nFCE_L();    

	NF_CLEAR_RB();
	NF_CMD(0x00);	// Read command
	NF_ADDR(0); 	// Column = 0
	NF_ADDR(blockPage&0xff);		//
	NF_ADDR((blockPage>>8)&0xff);	// Block & Page num.
	NF_ADDR((blockPage>>16)&0xff);	//
	NF_DETECT_RB();
	 
	
      #if TRANS_MODE==C_LANG
	    for(i=0;i<512;i++) {
	    	*bufPt++=NF_RDDATA8();	// Read one page
	    }
      #elif TRANS_MODE==DMA
		// Nand to memory dma setting
	    rSRCPND=BIT_DMA0;	// Init DMA src pending.
	    rDISRC0=NFDATA; 	// Nand flash data register
	    rDISRCC0=(0<<1) | (1<<0); //arc=AHB,src_addr=fix
	    rDIDST0=(unsigned)bufPt;
	    rDIDSTC0=(0<<1) | (0<<0); //dst=AHB,dst_addr=inc;
	    rDCON0=((unsigned int)1<<31)|(1<<30)|(1<<29)|(1<<28)|(1<<27)|(0<<23)|(1<<22)|(2<<20)|(512/4/4);
		//Handshake,AHB,interrupt,(4-burst),whole,S/W,no_autoreload,word,count=128;

		// DMA on and start.
	    rDMASKTRIG0=(1<<1)|(1<<0);

		while(!(rSRCPND & BIT_DMA0));	// Wait until Dma transfer is done.
		
	    rSRCPND=BIT_DMA0;

      #elif TRANS_MODE==ASM
	    __RdPage512(bufPt);
      #endif

  /*  
	  NF_MECC_Lock();
	  
	  rNFMECCD0=NF_RDDATA();
	  
	  NF_nFCE_H();	  
	  
	  if ((rNFESTAT0&0x3) == 0x0) 	  return OK;
	  else   return FAIL;
*/

	 NF_MECC_Lock();

	 NF_SECC_UnLock();
	 Mecc=NF_RDDATA();
	 rNFMECCD0=((Mecc&0xff00)<<8)|(Mecc&0xff);
	 rNFMECCD1=((Mecc&0xff000000)>>8)|((Mecc&0xff0000)>>16);
	
	 NF_SECC_Lock();
	 NF8_Spare_Data[0]=Mecc&0xff;
	 NF8_Spare_Data[1]=(Mecc&0xff00)>>8;
	 NF8_Spare_Data[2]=(Mecc&0xff0000)>>16;
	 NF8_Spare_Data[3]=(Mecc&0xff000000)>>24;
	 NF_RDDATA();  // read 4~7
	 Secc=NF_RDDATA();
	 rNFSECCD=((Secc&0xff00)<<8)|(Secc&0xff);
	 NF8_Spare_Data[8]=Secc&0xff;
	 NF8_Spare_Data[9]=(Secc&0xff00)>>8;
	 NF8_Spare_Data[10]=(Secc&0xff0000)>>16;
	 NF8_Spare_Data[11]=(Secc&0xff000000)>>24;
	 NF_nFCE_H();    

	 if ((rNFESTAT0&0xf) == 0x0){
	       Uart_Printf("ECC OK!\n");
		return OK;
	 }
	 else {
		Uart_Printf("ECC FAIL!\n");
	       return FAIL;
	 }


}


static int NF8_WritePage(U32 block,U32 page,U8 *buffer)
{
    int i;
	U32 blockPage, Mecc, Secc;
	U8 *bufPt=buffer;

	NFConDone=0;
       rNFCONT|=(1<<9);
       rNFCONT|=(1<<10);
       pISR_NFCON= (unsigned)NFCon_Int;
       rSRCPND=BIT_NFCON;
       rINTMSK=~(BIT_NFCON);
	  
	NF_RSTECC();    // Initialize ECC
       NF_MECC_UnLock();
	blockPage=(block<<5)+page;

	NF_nFCE_L(); 
	NF_CMD(0x0);//??????
	NF_CMD(0x80);   // Write 1st command
	NF_ADDR(0);			    // Column 0
	NF_ADDR(blockPage&0xff);	    //
	NF_ADDR((blockPage>>8)&0xff);   // Block & page num.
	NF_ADDR((blockPage>>16)&0xff);  //

	
#if TRANS_MODE==C_LANG
     
	for(i=0;i<512;i++) {
		NF_WRDATA8(*bufPt++);	// Write one page to NFM from buffer
    }
#elif TRANS_MODE==DMA
      
	// Memory to Nand dma setting
	rSRCPND=BIT_DMA0;	// Init DMA src pending.
	rDISRC0=(unsigned)bufPt; 	// Nand flash data register
	rDISRCC0=(0<<1) | (0<<0); //arc=AHB,src_addr=inc
	rDIDST0=NFDATA;
	rDIDSTC0=(0<<1) | (1<<0); //dst=AHB,dst_addr=fix;
	rDCON0=((unsigned int)1<<31)|(1<<30)|(1<<29)|(0<<28)|(1<<27)|(0<<23)|(1<<22)|(2<<20)|(512/4);
	//  only unit transfer in writing!!!!
	//Handshake,AHB,interrupt,(unit),whole,S/W,no_autoreload,word,count=128;
	
	// DMA on and start.
	rDMASKTRIG0=(1<<1)|(1<<0);
	
	while(!(rSRCPND & BIT_DMA0));	// Wait until Dma transfer is done.
	rSRCPND=BIT_DMA0;	
#endif
/*
      NF_MECC_Lock();
// Get ECC data.
	// Spare data for 8bit
	// byte  0   1    2    3    4   5
	// ecc  [0]  [1]  [2]  [3]  x   [Bad marking]
	Mecc = rNFMECC0;
	se8Buf[0]=(U8)(Mecc&0xff);
	se8Buf[1]=(U8)((Mecc>>8) & 0xff);
	se8Buf[2]=(U8)((Mecc>>16) & 0xff);
	se8Buf[3]=(U8)((Mecc>>24) & 0xff);
	se8Buf[5]=0xffff;		// Marking good block

	//Write extra data(ECC, bad marking)
	for(i=0;i<16;i++) {
		NF_WRDATA8(se8Buf[i]);	// Write spare array(ECC and Mark)
		NF8_Spare_Data[i]=se8Buf[i];
    }  

 	NF_CLEAR_RB();
	NF_CMD(0x10);	 // Write 2nd command
	NF_DETECT_RB();

	NF_CMD(0x70);   // Read status command   
    
	for(i=0;i<3;i++);  //twhr=60ns
     
    if (NF_RDDATA()&0x1) {// Page write error
    	
		Uart_Printf("[PROGRAM_ERROR:block#=%d]\n",block);
		NF8_MarkBadBlock(block);
		 NF_nFCE_H();
		return FAIL;
    } else {
    	   NF_nFCE_H();
	  return OK;
	   
	}
*/

       NF_MECC_Lock();
	// Get ECC data.
	// Spare data for 8bit
	// byte  0     1    2     3     4          5               6      7            8         9
	// ecc  [0]  [1]  [2]  [3]    x   [Bad marking]                    SECC0  SECC1
	Mecc = rNFMECC0;
	se8Buf[0]=(U8)(Mecc&0xff);
	se8Buf[1]=(U8)((Mecc>>8) & 0xff);
	se8Buf[2]=(U8)((Mecc>>16) & 0xff);
	se8Buf[3]=(U8)((Mecc>>24) & 0xff);
	se8Buf[5]=0xff;		// Marking good block

	NF_SECC_UnLock();
	//Write extra data(ECC, bad marking)
	for(i=0;i<4;i++) {
		NF_WRDATA8(se8Buf[i]);	// Write spare array(Main ECC)
		NF8_Spare_Data[i]=se8Buf[i];
    	}  
      NF_SECC_Lock(); 
	Secc=rNFSECC; 
	se8Buf[8]=(U8)(Secc&0xff);
	se8Buf[9]=(U8)((Secc>>8) & 0xff);
	for(i=4;i<16;i++) {
		NF_WRDATA8(se8Buf[i]);  // Write spare array(Spare ECC and Mark)
		NF8_Spare_Data[i]=se8Buf[i];
	}  
 	NF_CLEAR_RB();
	NF_CMD(0x10);	 // Write 2nd command
//	NF_DETECT_RB();
	while(NFConDone==0);
	 rNFCONT&=~(1<<9);
	 rNFCONT&=~(1<<10); // Disable Illegal Access Interrupt
	 if(rNFSTAT&0x8) return FAIL;

	NF_CMD(0x70);   // Read status command   
    
	for(i=0;i<3;i++);  //twhr=60ns
    
       if (NF_RDDATA()&0x1) {// Page write error
    	       NF_nFCE_H();
		Uart_Printf("[PROGRAM_ERROR:block#=%d]\n",block);
		NF8_MarkBadBlock(block);
		return FAIL;
       } else {
    	      NF_nFCE_H();
	      return OK;
	}

}


static U16 NF8_CheckId(void)
{
    int i;
	U16 id;//, id4th;
    
	NF_nFCE_L();
      	NF_CMD(0x90);
	NF_ADDR(0x0);
	for (i=0; i<10; i++);
	Uart_Printf("NFSTAT: 0x%x\n", rNFSTAT);
       id=NF_RDDATA8()<<8;	// Maker code 0xec
       id|=NF_RDDATA8();	// Devide code(K9S1208V:0x76), (K9K2G16U0M:0xca)

	NF_nFCE_H();
       return id;
}


void Nand_Reset(void)
{
    int i;
   
	NF_nFCE_L();

	NF_CLEAR_RB();
	NF_CMD(0xFF);	//reset command
	for(i=0;i<10;i++);  //tWB = 100ns. //??????
	NF_DETECT_RB();
	
	NF_nFCE_H();

}

static void NF8_Init(void)
{
	// for S3C2440

	rNFCONF = (TACLS<<12)|(TWRPH0<<8)|(TWRPH1<<4)|(0<<0);	
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


