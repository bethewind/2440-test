#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "def.h"
#include "option.h"
#include "library.h"
#include "sfr6410.h"
#include "sromc.h"
#include "system.h"
#include "sysc.h"
#include "dma.h"
#include "intc.h"
#include "sromc.h"
#include "gpio.h"
#include "cs8900.h"


#if 0
#define CS8900_Tacs	(eCLK0)	// 0clk		address set-up
#define CS8900_Tcos	(eCLK5)	// 4clk		chip selection set-up
#define CS8900_Tacc	(eCLK15)	// 14clk	access cycle
#define CS8900_Tcoh	(eCLK2)	// 1clk		chip selection hold
#define CS8900_Tah	(eCLK5)	// 4clk		address holding time
#define CS8900_Tacp	(eCLK7)	// 6clk		page mode access cycle
#else
#define CS8900_Tacs	(eCLK0)	// 0clk		address set-up
#define CS8900_Tcos	(eCLK3)	// 2clk		chip selection set-up
#define CS8900_Tacc	(eCLK21)	// 20clk	access cycle
#define CS8900_Tcoh	(eCLK0)	// 0clk		chip selection hold
#define CS8900_Tah	(eCLK0)	// 0clk		address holding time
#define CS8900_Tacp	(eCLK2)	// 1clk		page mode access cycle
#endif

#define CS8900_PMC	(0x0)	// normal(1data)

#define PUBLIC
#define PRIVATE						static

#define IOREAD(o)					((unsigned short)*((volatile unsigned short *)(dwEthernetIOBase + (o))))
#define IOWRITE(o, d)				*((volatile unsigned short*)(dwEthernetIOBase + (o))) = (unsigned short)(d)

//#define DBGMSG						EdbgOutputDebugString
#define MAX_COUNT					0x100000

#define CS8900_BASE_ADDRESS			0x10000000	//bank 0
#define CS8900_IOMODE_ASSERT		(0x01000000)	//address24 
#define CS8900_IOBASE_OFFSET		(0x300)	// IO Base Address

#define CS8900DBG_PROBE				(1 << 0)

PRIVATE unsigned int dwEthernetIOBase;

#define READ_REG					ReadReg
#define WRITE_REG					WriteReg

void Detect_CS8900(void);



void * func_cs8900_test[][2]=
{	
    //        				"123456789012345678"
    //Clock test
    (void *)Detect_CS8900,			"Detect CS8900   ",
    0,0
};

void CS8900_Test(void)
{
    int i;

	while(1)
	{
		i=0;
		UART_Printf("\n\n");
		while(1)
		{   //display menu
		    UART_Printf("%2d:%s",i,func_cs8900_test[i][1]);
		    i++;
		    if((int)(func_cs8900_test[i][0])==0)
		    {
			UART_Printf("\n");
			break;
		    }
		    if((i%3)==0)
		    UART_Printf("\n");
	}

	UART_Printf("\nSelect (\"-1\" to exit) : ");
	i = UART_GetIntNum();
	if(i==-1) 
	    break;		// return.
	if(i>=0 && (i<((sizeof(func_cs8900_test)-1)/8)) )	// select and execute...
	    ( (void (*)(void)) (func_cs8900_test[i][0]) )();
    }
    UART_Printf("\nCS8900_Test program end.\n");
}



PRIVATE unsigned short 
ReadReg(unsigned short offset)
{
	IOWRITE(IO_PACKET_PAGE_POINTER, offset);
	return IOREAD(IO_PACKET_PAGE_DATA_0);
}

PRIVATE void 
WriteReg(unsigned short offset, unsigned short data)
{
	IOWRITE(IO_PACKET_PAGE_POINTER, offset);
	IOWRITE(IO_PACKET_PAGE_DATA_0 , data);
}

void Detect_CS8900(void)
{
	unsigned int iobase = 0;
	volatile unsigned short signature=0;
	volatile unsigned short eisa_number=0;
	volatile unsigned short prod_id=0;
	unsigned short offset=0;
	s32	iBankSel = 0;
	s32	iTacs = 0;
	s32	iTcos = 0;
	s32	iTacc = 0;
	s32	iTacp = 0;
	s32	iTcoh = 0;
	s32	iTcah = 0;

	UART_Printf("1)nCS1	2)nCS2	3)nCS3	4)nCS4	5)nCS5\n");
	UART_Printf("\nSelect SROM Bank(\"0\" to exit) : ");
	iBankSel = UART_GetIntNum();
	if(iBankSel==0 || iBankSel > 5) 
	    return;		// return.

	UART_Printf("\nSelect Tacs(0~15, default 0) : ");
	iTacs = UART_GetIntNum();
	if (iTacs == -1) 
		iTacs = eCLK0;
	UART_Printf("\nSelect Tcos(0~15, default 3) : ");
	iTcos = UART_GetIntNum();
	if (iTcos == -1) 
		iTcos = eCLK3;
	UART_Printf("\nSelect Tacc(0~31, default 21) : ");
	iTacc = UART_GetIntNum();
	if (iTacc == -1) 
		iTacc = eCLK21;	
	UART_Printf("\nSelect Tacp(0~15, default 2) : ");
	iTacp = UART_GetIntNum();
	if (iTacp == -1) 
		iTacp = eCLK2;	
	UART_Printf("\nSelect Tcoh(0~15, default 0) : ");
	iTcoh = UART_GetIntNum();
	if (iTcoh == -1) 
		iTcoh = eCLK0;		
	UART_Printf("\nSelect Tcah(0~15, default 0) : ");
	iTcah = UART_GetIntNum();
	if (iTcah == -1) 
		iTcah = eCLK0;		

	
	SROMC_SetBank(iBankSel, eEn_CTL, eEn_WAIT, e16bit, eNor_Mode, 
					(Bank_eTiming)iTacs, (Bank_eTiming)iTcos, (Bank_eTiming)iTacc, (Bank_eTiming)iTcoh, (Bank_eTiming)iTcah, (Bank_eTiming)iTacp);
	GPIO_SetMem0DrvStrength(0x3);		// Drive Strength Control... 0x3 is maximum value(14mA@VDDmem=1.8V)
	
	////////////////////////////////////////////////////////
	// bank5 = 0x14000000
	// cs8900 cs base address 
	// ADDR24 - high -> ioread/write
	// ADDR24 - low --> mem read/write
	////////////////////////////////////////////////////////
	// io read/write base address 
	// = 0x14000000 + 0x01000000 (ADDR24 high) + 0x300 (cs8900 io base offset) = 0x15000300
	//------------------------------------------------------
	// memory read/write base address 
	// = 0x14000000 + 0x00000000 (ADDR24 low) = 0x14000000
	////////////////////////////////////////////////////////
//	iobase = CS8900_BASE_ADDRESS+CS8900_IOMODE_ASSERT+CS8900_IOBASE_OFFSET;
	iobase = CS8900_BASE_ADDRESS+0x08000000*(iBankSel)+CS8900_IOBASE_OFFSET;
	
	UART_Printf(" CS8900 Probe()... CS:%d, IOBASE : 0x%x\r\n", iBankSel, iobase);

	dwEthernetIOBase  = iobase;

	do 
	{			
											/* Check the EISA registration number.	*/
		offset = PKTPG_EISA_NUMBER;
		eisa_number = READ_REG(offset);
		UART_Printf("Eisa Number1 = 0x%04x\r\n", eisa_number);
		if (eisa_number != CS8900_EISA_NUMBER)
		{
			UART_Printf("Eisa Number Error = 0x%04x\r\n", eisa_number);
			break;
		}

		signature = IOREAD(IO_PACKET_PAGE_POINTER);
		UART_Printf("Signature = 0x%04x\r\n", signature);
		if (signature != (CS8900_SIGNATURE+offset))
		{
			UART_Printf("Signature Error = 0x%04x\r\n", signature);
			break;
		}

		offset = PKTPG_PRDCT_ID_CODE;
		prod_id = READ_REG(offset);
		UART_Printf("Product ID = 0x%04x\r\n", prod_id);
		if (( prod_id & CS8900_PRDCT_ID_MASK)!= CS8900_PRDCT_ID)
		{
			UART_Printf("Product ID Error = 0x%04x\r\n", prod_id);
			break;
		}

		signature = IOREAD(IO_PACKET_PAGE_POINTER);
		UART_Printf("Signature = 0x%04x\r\n", signature);
		if (signature != (CS8900_SIGNATURE+offset))
		{
			UART_Printf("Signature Error = 0x%04x\r\n", signature);
			break;
		}

		UART_Printf("CS8900 is Detected..\n");

		WRITE_REG(0, 0);
	} while (0);

}





