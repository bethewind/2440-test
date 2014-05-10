/**
 * HOSTIF.C
 *
 * @author	SONG JAE GEUN
 * @version         1.0
 */


#include <string.h>
#include <stdio.h>
#include "system.h"
#include "library.h"
#include "hostif.h"
#include "sfr6410.h"
#include "sysc.h"
#include "intc.h"
#include "gpio.h"
#include "modemif.h"

enum {

	rHOSTIF_CTRL			= (HOSTIF_SFR_BASE+0x00),
	rHOSTIF_CONV			= (HOSTIF_SFR_BASE+0x04),
	rHOSTIF_TMP				= (HOSTIF_SFR_BASE+0x08),
	
	rHOSTIF_IMB				= (HOSTIF_SFR_BASE+0x10),
	rHOSTIF_OMB				= (HOSTIF_SFR_BASE+0x14),

	rHOSTIF_STAT			= (HOSTIF_SFR_BASE+0x20),
	rHOSTIF_STAT1			= (HOSTIF_SFR_BASE+0x24),
	rHOSTIF_STAT2			= (HOSTIF_SFR_BASE+0x28),

	rHOSTIF_INTE			= (HOSTIF_SFR_BASE+0x30),
	rHOSTIF_INTE1			= (HOSTIF_SFR_BASE+0x34),
	rHOSTIF_INTE2			= (HOSTIF_SFR_BASE+0x38),

	rHOSTIF_MEM_00			= (HOSTIF_SFR_BASE+0x800)
};

extern u8	 g_ucIMBFlag;
extern u8 g_ucOMBFlag;

// AP side ISR
void __irq Isr_HOSTIF_AP(void)
{
#if 0
	u32 temp;
	
	INTC_Disable(NUM_HOSTIF);

	UART_Printf("HOST I/F Interrupt occured.....\n");

	temp = Inp32(rHOSTIF_IMB);
	UART_Printf("MODEM to AP Message = 0x%08x\n", temp);
	
	HOSTIF_Int2Clear(INT2_IMB_FILLED);
		
	INTC_Enable(NUM_HOSTIF);
	INTC_ClearVectAddr();
#else
	u32 temp, message;
	
	INTC_Disable(NUM_HOSTIF);

	temp = Inp32(rHOSTIF_STAT2);

	UART_Printf("HOST I/F AP side Interrupt occured!!! 0x%x\n",temp);

	if(temp >> INT2_RBURST_DONE)
	{
		UART_Printf("Repeated Burst Write done....\n");
	}

	else if (temp >> INT2_IMB_FILLED)
	{
		HOSTIF_Int2Clear(INT2_IMB_FILLED);

		message = Inp32(rHOSTIF_IMB);	
		UART_Printf("MODEM to AP Message = 0x%08x\n", message);

		g_ucIMBFlag = 1;

	}	

	else if ( temp >> INT2_OMB_EMPTY)
	{
		UART_Printf("OMB EMPTY!!!!\n");
	}

	else		
		UART_Printf("Over/Under run occured....\n");
		
	INTC_Enable(NUM_HOSTIF);
	INTC_ClearVectAddr();

#endif
}

void __irq Isr_HOSTIF_MODEM(void)
{
	u32 message=0;
	u32 uTemp=0;
	
	GPIO_EINT0ClrPend(4);
	MODEMIF_IntClear_Int2MSM();

	UART_Printf("HOST I/F MODEM side Interrupt occured!!!\n");	

	HOSTIF_SelectBank(BANK0);
	uTemp = Inp32(rSTAT);
	UART_Printf("rSTAT = 0x%x\n", uTemp);
	
	
	HOSTIF_SelectBank(BANK1);
	
	uTemp = Inp32(rSTAT1);
	Outp32(rSTAT1, uTemp|(1<<INT1_OMB_FILLED));

	message = HOSTIF_ReadOutMailBox();
	UART_Printf("Message = 0x%x\n", message);

	g_ucOMBFlag = 1;

	INTC_ClearVectAddr();
}

void HOSTIF_IntEnable(u16 source)
{
/*
	u16 int_source = (source == OMB) ? INT_OMB_FILLED : INT_IMB_EMPTY;
		
	HOSTIF_SelectBank(BANK1);
	Outp16(rINTE1, int_source);
*/	
}

void HOSTIF_Int1Enable(HOSTIF_INTE1 source)
{
	u16 temp;

	HOSTIF_SelectBank(BANK1);
	
	if (source == INT1_ALL)
		Outp16(rINTE1, source);
	else
	{
		temp = Inp16(rINTE1);
		Outp16(rINTE1, temp|(1<<source));
		//temp = Inp16(rINTE1);
		//UART_Printf("INTE1 = 0x%x\n", temp);
	}
}

void HOSTIF_Int1AllDisable(void)
{
	HOSTIF_SelectBank(BANK1);
	Outp16(rINTE1, 0x0);
}

void HOSTIF_Int1Clear(HOSTIF_INTE1 source)
{
	u32 temp;
	
	HOSTIF_SelectBank(BANK1);
	
	temp = Inp32(rHOSTIF_STAT1);
	Outp32(rHOSTIF_STAT1, temp|(1<<source));

}

void HOSTIF_Int2Enable(HOSTIF_INTE2 source)
{
	u32 temp;

	if (source == INT2_ALL)
		Outp32(rHOSTIF_INTE2, source);
	else
	{
		temp = Inp32(rHOSTIF_INTE2);
		Outp32(rHOSTIF_INTE2, temp|(1<<source));
	}	
}

void HOSTIF_Int2Clear(HOSTIF_INTE2 source)
{
	u32 temp;

	temp = Inp32(rHOSTIF_STAT2);
	Outp32(rHOSTIF_STAT2, temp|(1<<source));
}

void HOSTIF_AP_Init(void)
{
	*(volatile unsigned *)0x70000004 =(0<<28)|(0<<24)|(0xf<<16)|(1<<12)|(0<<8)|(0<<4)|(0<<0);

	// SROM BUS 16bit setting
	SYSC_16bitBUS();

	// Modem I/F Set Port
	GPIO_SetFunctionAll(eGPIO_K, 0x22222222, 0x22222222);	 //GPN[31:0] -> DATA[15:0]
	GPIO_SetFunctionAll(eGPIO_L, 0x22222222, 0x2222222);	 //GPN[31:0] -> ADDR[17:0]
	GPIO_SetFunctionAll(eGPIO_M, 0x222222, 0);	 //GPM[23:0] -> HOST I/F CS, CSn_MAIN, CSn_SUB, WEn, OEn, INTRn
	//GPIO_SetFunctionAll(eGPIO_N, 0x55555555, 0);	 //GPN3 : AP Reset, GPN8 : MODEM Reset Output

	// HOST I/F Interrupt Setting
	HOSTIF_Int2Enable(INT2_ALL);
	//Outp32(rHOSTIF_INTE2, 0xfffeffff);
	
	//AP Interrupt Setting (HOST I/F)
	INTC_SetVectAddr(NUM_HOSTIF, Isr_HOSTIF_AP);
	INTC_Enable(NUM_HOSTIF);

}

void HOSTIF_MODEM_Init(void)
{
	*(volatile unsigned *)0x70000004 =(0<<28)|(0<<24)|(0xf<<16)|(1<<12)|(0<<8)|(0<<4)|(0<<0);
	//*(volatile unsigned *)0x70000004 =(0<<28)|(0<<24)|(0x7<16)|(1<<12)|(0<<8)|(0<<4)|(0<<0);

 	// ROM Bus 16bit setting
	SYSC_16bitBUS();
	
	// Modem I/F Interrupt request GPIO setting
	GPIO_SetPullUpDownEach(eGPIO_N, eGPIO_4, 0x0); // Pull Down Disable
	GPIO_SetEint0(4, Falling_Edge, eDLYFLT, 0);

	// Clear the previous pending states
	GPIO_EINT0ClrPend(4);
	GPIO_EINT0DisMask(4);

	// Interrupt Setting (MODEM : EINT)
	INTC_SetVectAddr(NUM_EINT1, Isr_HOSTIF_MODEM);		// NUM_EINT1 : Eint[11:4]
	INTC_Enable(NUM_EINT1);

	// MODEM I/F Inerrupt Clear
	MODEMIF_IntClear_Int2MSM();

	// HOST I/F Interrupt Setting
	HOSTIF_Int1Enable(INT1_OMB_FILLED);

	// HOLD Delay Margin
	HOSTIF_SelectBank(BANK1);
	Outp16(rCTRL1, 0x3);

}


//=======================================
void HOSTIF_MODEM_To_AP_Reset(void)
{
	HOSTIF_SelectBank(BANK11);
	Outp16(rCTRL, 0x1);
}

//============================
void HOSTIF_SelectBank(u16 bsel)
{
	Outp16(rBSEL, bsel);
}

void HOSTIF_SetTotalTransferSize(u16 transfersize)
{
	Outp32(rDATA, transfersize);
}

void HOSTIF_SetMode(u32 dir, u32 nWord, u32 rep)
{
	u16 burstLength=((nWord&0x1FF)<<4);
	u16 repeatBurstMode = (rep == 0) ? RBURST_DISABLE : RBURST_ENABLE;
	//u16 transferDst = (dst == 0) ? DEST_LOWER : DEST_UPPER;	
	u16 transferDst = 0;
	u16 readWrite = (dir == HOSTIF_WRITE) ? TRANS_WR : TRANS_RD;
	u16 mode;

	//Assert(nWord<MAX_BURST_SIZE);
			
	mode = burstLength|repeatBurstMode|transferDst|readWrite;
	Outp16(rCTRL, mode);
}

void HOSTIF_WaitForReady(u8 statusMode)
{
	u32 cnt;
	u16 uRead;

	for (cnt=0; cnt<0xff; cnt++)
	{
		uRead = Inp16(rSTAT);
		if (uRead & statusMode)
			return;
		else
			Delay(1);
	}

	UART_Printf("WaitValid Error!!\n");
	return;
}

void HOSTIF_WaitForWriteReady(u32 uWord)
{
	u32 cnt;
	u16 uWordCnt;
	u16 uRead;

	for (cnt=0; cnt<0xff; cnt++)
	{	
		uRead = Inp16(rSTAT);
		uWordCnt = ((uRead>>8)&0xFF)*2;
		if (uWordCnt >= uWord)
			return;
		else
			Delay(1);
	}
	
	UART_Printf("WaitForWriteReady() Error!!\n");
	return;
}

void HOSTIF_WriteData(u32 data)
{
	Outp32(rDATA, (u32)data);
}

void HOSTIF_Write(u32 addr, u32 *data, u32 nWord, u32 rep)
{
	//u32 i, bLen;

	Assert(nWord <= MAX_BURST_SIZE); // less than 2^16

	HOSTIF_SelectBank(BANK0);
	HOSTIF_WaitForWriteReady(nWord); // apply to both single write and burst write
	HOSTIF_SetMode(HOSTIF_WRITE, nWord, rep);

	HOSTIF_SelectBank(BANK8);
	HOSTIF_WriteData(addr);	
	HOSTIF_WriteData((*(u32 *)data));
}

void HOSTIF_SingleWrite(u32 addr, u32 data)
{
	HOSTIF_Write(addr, &data, 1, 0);
}

void HOSTIF_BurstWrite(u32 addr, u32 *data, u32 nWord)
{
#if 0	// MAX SIZE BURST WRITE
	u32 i, src, bLen;

	Assert(nWord <= MAX_BURST_SIZE); // less than 2^16

	HOSTIF_SelectBank(BANK0);
	HOSTIF_WaitForWriteReady(nWord); // apply to both single write and burst write
	HOSTIF_SetMode(HOSTIF_WRITE, nWord, 0);

	HOSTIF_SelectBank(BANK8);
	HOSTIF_WriteData(addr);	
	
	for(i=0 ; i<nWord ; i++)
		HOSTIF_WriteData(*((u32 *)data+i));
	
#else	// MASS DATA SIZE TEST	

	u32 quotient = nWord/MAX_BURST_SIZE;
	u32 remains = nWord%MAX_BURST_SIZE;
	u32 i, j, k;
	u32 dest;

	for(i=0 ; i<quotient ; i++)
	{
		dest = addr + i*MAX_BURST_SIZE*4;

		HOSTIF_SelectBank(BANK0);
		HOSTIF_WaitForWriteReady(MAX_BURST_SIZE); // apply to both single write and burst write
		//HOSTIF_WaitForWriteReady(128); // apply to both single write and burst write
		HOSTIF_SetMode(HOSTIF_WRITE, MAX_BURST_SIZE, 0);

		HOSTIF_SelectBank(BANK8);
		HOSTIF_WriteData(dest);		// address
		
		for(j=0 ; j<MAX_BURST_SIZE ; j++)
			HOSTIF_WriteData(*((u32 *)data+i*MAX_BURST_SIZE+j));
	}

	if(remains>0)
	{
		dest = addr + quotient*MAX_BURST_SIZE*4;
		
		HOSTIF_SelectBank(BANK0);
		HOSTIF_WaitForWriteReady(MAX_BURST_SIZE); // apply to both single write and burst write
		//HOSTIF_WaitForWriteReady(128); // apply to both single write and burst write
		HOSTIF_SetMode(HOSTIF_WRITE, remains, 0);

		HOSTIF_SelectBank(BANK8);
		HOSTIF_WriteData(dest);	

		for(k=0 ; k<remains ; k++)
		{
			HOSTIF_WriteData(*((u32 *)data+quotient*MAX_BURST_SIZE+k));
		}	
	}
#endif
}


void HOSTIF_RepeatedBurstWrite(u32 addr, u32 *data, u32 nWord)
{
	u32 i;

	HOSTIF_SelectBank(BANK0);
	HOSTIF_WaitForWriteReady(MAX_BURST_SIZE); // apply to both single write and burst write
	//HOSTIF_WaitForWriteReady(128); // apply to both single write and burst write
	HOSTIF_SetMode(HOSTIF_WRITE, MAX_BURST_SIZE, 1);

	HOSTIF_SelectBank(BANK8);
	HOSTIF_SetTotalTransferSize(nWord);
	HOSTIF_WriteData(addr);	

	for(i=0 ; i<nWord ; i++)
		HOSTIF_WriteData(*((u32 *)data+i));
}

u32 HOSTIF_ReadData(void)
{
	u32 data;

	data = Inp16(rDATAL);
	data |= ((Inp16(rDATAH))<<16);
	
	return (u32)data;
}

u32 HOSTIF_Read(u32 addr, u32 nWord)
{

	u32 data;
	u32 head, add;
	
	Assert(nWord <= MAX_BURST_SIZE);

	HOSTIF_SelectBank(BANK0);	
	HOSTIF_WaitForWriteReady(2); // 2 for mode, addr setting
	HOSTIF_SetMode(HOSTIF_READ, nWord, 0);

	//SetAddr(addr);
	HOSTIF_SelectBank(BANK8);
	HOSTIF_WriteData(addr);	

	HOSTIF_SelectBank(BANK0);		
	HOSTIF_WaitForReady(RFIFO_READY);
	//GetData(data, uWord);  

	HOSTIF_SelectBank(BANK8);			
	head = HOSTIF_ReadData();
	add = HOSTIF_ReadData();
	data = HOSTIF_ReadData();

	return data;

}

u32 HOSTIF_SingleRead(u32 addr)
{
	u32 data;
	data = HOSTIF_Read(addr, 1);
	return data;
}

void HOSTIF_BurstRead(u32 addr, u16 *data, u32 nWord)
{
	u32 i, j, k;
	u32 dest;
	u32 head, add;
	u32 quotient = nWord/MAX_BURST_SIZE;
	u32 remains = nWord%MAX_BURST_SIZE;

	for(i=0 ; i<quotient ; i++)
	{
		dest = addr + i*MAX_BURST_SIZE*4;

		HOSTIF_SelectBank(BANK0);	
		HOSTIF_WaitForWriteReady(2); // 2 for mode, addr setting	UART_Printf("0    ");
		HOSTIF_SetMode(HOSTIF_READ, MAX_BURST_SIZE, 0);

		HOSTIF_SelectBank(BANK8);	
		HOSTIF_WriteData(dest);
		HOSTIF_SelectBank(BANK0);	
		HOSTIF_WaitForReady(RFIFO_READY);

		HOSTIF_SelectBank(BANK8);	
		head = HOSTIF_ReadData();
		add = HOSTIF_ReadData();

		for(j=0; j<MAX_BURST_SIZE ; j++)
			*((u32 *)data+i*MAX_BURST_SIZE+j) = HOSTIF_ReadData();
	}

	if(remains>0)
	{
		dest = addr + quotient*MAX_BURST_SIZE*4;
		
		HOSTIF_SelectBank(BANK0);	
		HOSTIF_WaitForWriteReady(2); // 2 for mode, addr setting	UART_Printf("0    ");
		HOSTIF_SetMode(HOSTIF_READ, MAX_BURST_SIZE, 0);

		HOSTIF_SelectBank(BANK8);	
		HOSTIF_WriteData(dest);
		HOSTIF_SelectBank(BANK0);	
		HOSTIF_WaitForReady(RFIFO_READY);

		HOSTIF_SelectBank(BANK8);	
		head = HOSTIF_ReadData();
		add = HOSTIF_ReadData();

		for(k=0; k<remains ; k++) 
			*((u32 *)data+quotient*MAX_BURST_SIZE+k) = HOSTIF_ReadData();

	}

}

void HOSTIF_WriteInMailBox(u32 data)
{
	Outp16(rIMBL, (u16)data);	
	Outp16(rIMBH, (u16)(data >> 16));	
}

void HOSTIF_InMailBox(u32 protocol)
{
	HOSTIF_SelectBank(BANK2);
	HOSTIF_WriteInMailBox(protocol);
}

void HOSTIF_OutMailBox(u32 protocol)
{
	Outp32(rHOSTIF_OMB, protocol);
}

u32 HOSTIF_ReadOutMailBox(void)
{
	u32 data;
	
	HOSTIF_SelectBank(BANK3);
	data = Inp16(rOMBL);
	data |= ((Inp16(rOMBH))<<16);
	
	return (u32)data;
}

void HOSTIF_TestProtoReg(void)
{
	u32 data;

	HOSTIF_SelectBank(0);

	Outp16(rINTE, 0xabcd);

	data = Inp16(rINTE);
	UART_Printf("INTE = 0x%x\n", data);

}

