
//===================================================================
// NAME		: modemif.c
// DESC		: S3C6410 DPSRAM test 
// History	: 01.03.2007 ver 0.0 by JAE GUEN. SONG
// 			: 03.06.2008 ver 0.1 by Sun il, Roe	for S3C6410
//===================================================================


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sfr6410.h"
#include "system.h"
#include "option.h"
#include "library.h"
#include "def.h"
#include "sysc.h"
#include "sromc.h"
#include "modemif.h"
#include "gpio.h"
#include "intc.h"
#include "dma.h"
#include "timer.h"

volatile int g_ApDataCnt;
volatile int g_MsmDataCnt;

volatile int g_ModemReadDone;			// Read Done flag of Modem side
volatile int g_ModemWriteDone;		// Write Done flag of Modem side
volatile int g_APReadDone;				// Read Done flag of AP side
volatile int g_APDMAReadDone;			// DMA Read Done flag of AP side
volatile int g_APDMAWriteDone;		// DMA Write Done flag of AP side

volatile int g_ModemReadDoneReply;	// Read Done Reply flag from Modem to AP
volatile int g_ModemReadyReply;		// Modem Ready Reply flag from Modem to AP

extern u8		ucModemDMAPort;

extern volatile int g_DmaDone;
extern volatile int	g_DmaDoneMTx, g_DmaDoneMRx, g_DmaDoneATx, g_DmaDoneARx;
extern DMAC oAPRx0, oAPTx0;
extern DMAC oModemif0;

//static DMAC oModemif0;
//static DMAC oModemif_ap0;

void MODEM_BlockCopy(u32, u32, u32 ); 
//for MODEM EINT
void __irq Isr_AP2MODEM(void)
{
	u32 temp = 0;
	//u32 lengh = 0;
	//u32 *rx_data, *rx_buf;

	// ENT Pending Clear & AP2MODEM INT Clear & Message
	GPIO_EINT0ClrPend(4);

	//for(temp=0;temp<400000000;temp++);		// just for interrupt test

	temp = MODEMIF_IntClear_Int2MSM();
	MODEM_PRINTF("Modem Received = 0x%x\n", temp);

	//////////////////////////////
	////// MODEM TX ISR //////////
	/////////////////////////////

	if (temp == 0xff00)
		UART_Printf("[AP start data transfer]\n");

	else if (temp == 0xee00)
	{
		//UART_Printf("[ MODEM DMA ReSTART:%d !!!]\n", g_DmaDoneMTx);	

		//DMA START
		//Outp32(0x75000114, 0x8001);
		DMACH_Start(&oModemif0);
	}

	else if (temp ==0x1100)
	{
		UART_Printf("[MODEM TX completed.]\n\n");
		g_ModemWriteDone = 1;
	}
	

	//////////////////////////
	////// AP TX ISR //////////
	/////////////////////////
	else if (temp == 0x00aa)
	{
		UART_Printf("[AP TX Started.....MODEM OK!!!]\n");
		UART_Printf(" 0x00aa(ModemtoAP) --->\n");
		UART_Printf(" ------------------------\n");
		Outp16(DPSRAM_BASE_MODEM+INT2AP_ADR, 0x00aa);
		Outp16(DPSRAM_BASE_MODEM+DMA_TX_ADDR0+ucModemDMAPort*2, 0x00bb);
		temp = (Inp16(DPSRAM_BASE_MODEM + DMA_TX_ADDR0+ucModemDMAPort*2))&0xffff;
	}
	else if (temp == 0x00bb)
	{
		g_MsmDataCnt++;
		DMACH_Start(&oModemif0);
	}
	else if (temp == 0x00dd)
	{
		UART_Printf("[MODEM RX Done]\n");
		g_ModemReadDone	= 1;
	}
	else if (temp == 0x00ff)
	{
		UART_Printf("[AP TX Started.....MODEM OK!!!]\n");
		UART_Printf(" 0x00ff(ModemtoAP) --->\n");
		UART_Printf(" ------------------------\n");
		Outp16(DPSRAM_BASE_MODEM+INT2AP_ADR, 0x00ff);
	}
		
	else if (temp == 0x00ee) 
	{
		UART_Printf("[AP transfer data to SDRAM]\n");
#if 0
		rx_buf = (u32 *)(MSM_RX_BUFFER+(g_MsmDataCnt*AP_TX_BUFLEN));
		rx_data = (u32 *)(MSM_RX_Queue);
		//UART_Printf("rx_buf = 0x%x, rx_data = 0x%x\n", rx_buf, rx_data);

		for (lengh = 0 ; lengh <= AP_TX_BUFLEN ; lengh++)
			*(u32 *)(rx_buf++) = *(rx_data++);

		UART_Printf("[g_MsmDataCnt= 0x%x ]\n", g_MsmDataCnt);	
		g_MsmDataCnt++;

		//if(g_MsmDataCnt < BLK_COUNT)
		if(g_MsmDataCnt <= BLK_COUNT) {
			UART_Printf(" 0x00ee(ModemtoAP) --->\n");
			UART_Printf(" ------------------------\n");
			Outp16(DPSRAM_BASE_MODEM+INT2AP_ADR, 0x00ee);
		} else {
			UART_Printf(" 0x0011(ModemtoAP) --->\n");
			UART_Printf(" ------------------------\n");
			Outp16(DPSRAM_BASE_MODEM+INT2AP_ADR, 0x0011);
		}
#else
		g_MsmDataCnt++;
		DMACH_Start(&oModemif0);
#endif
	}

	else if (temp == 0x0011)
	{
		UART_Printf("[MODEM RX Done]\n");
		g_ModemReadDone	= 1;
	}

	else if (temp == 0x0022)
	{
		UART_Printf("[MODEM RX : AP TX Interrupt Done]\n");
	}
	else
		UART_Printf("Error = 0x%04x\n",temp);		
	
	INTC_ClearVectAddr();

}


//for AP ISR
void __irq Isr_MODEM2AP(void)
{
	u32 temp = 0;
	//u32 lengh = 0;
	//u32 *rx_data, *rx_buf;

	INTC_Disable(NUM_MSM);
	
	temp = (Inp16(DPSRAM_BASE_AP + INT2AP_ADR))&0xffff;
	MODEM_PRINTF("AP Received = 0x%x\n", temp);
	MODEMIF_IntClear_Int2AP();

	//////////////////////////////
	////// MODEM TX ISR //////////
	/////////////////////////////
	
	if( temp == 0xff00)
	{
		//UART_Printf("[MODEM start data transfer]\n");


		#if 0	// C code

		   rx_buf = (u32 *)(AP_RX_BUFFER);
		   rx_data = (u32 *)(AP_RX_Queue);
		   //UART_Printf("rx_buf = 0x%x, rx_data = 0x%x\n", rx_buf, rx_data);

			for (lengh = 0 ; lengh <= MODEM_TX_BUFLEN ; lengh++)
				*(u32 *)(rx_buf++) = *(rx_data++);

		#elif 0	// MEMCOPY

			rx_buf = (u32 *)(AP_RX_BUFFER);
			rx_data = (u32 *)(AP_RX_Queue);

			memcpy(rx_buf, rx_data, MODEM_TX_BUFLEN); 

		#elif 1	// LOAD MULTIPLE (Assem)

			//MODEM_BlockCopy(0x74100000, 0x51100000, 128); 
			//MODEM_BlockCopy(AP_RX_Queue, AP_RX_BUFFER, MODEM_TX_BUFLEN/8); 
			MODEM_BlockCopy(AP_RX_Queue, AP_RX_BUFFER, 128); 

		#endif

		g_ApDataCnt++;
		//g_ApDataCnt = DMA_COUNT;		// just for test
		if(g_ApDataCnt < DMA_COUNT) {
			MODEM_PRINTF("<--- 0xee00(APtoModem)\n");
			MODEM_PRINTF(" ------------------------\n");
			Outp16(DPSRAM_BASE+INT2MSM_ADR, 0xee00);
		} else {
			UART_Printf("<--- 0x1100(APtoModem)\n");
			UART_Printf(" ------------------------\n");
			Outp16(DPSRAM_BASE+INT2MSM_ADR, 0x1100);
		}
	}
	
	else if(temp == 0xee00)
	{
		//UART_Printf("\n[AP transfer data to SDRAM]\n");
		

		#if 0	// C code
		
		rx_buf = (u32 *)(AP_RX_BUFFER+(g_ApDataCnt*MODEM_TX_BUFLEN));
		rx_data = (u32 *)(AP_RX_Queue);
		//UART_Printf("rx_buf = 0x%x, rx_data = 0x%x\n", rx_buf, rx_data);
			for (lengh = 0 ; lengh <= MODEM_TX_BUFLEN ; lengh++)
				*(u32 *)(rx_buf++) = *(rx_data++);
			
		#elif	0	// MEMCOPY

				rx_buf = (u32 *)(AP_RX_BUFFER+(g_ApDataCnt*MODEM_TX_BUFLEN));
				rx_data = (u32 *)(AP_RX_Queue);

				memcpy(rx_buf, rx_data, MODEM_TX_BUFLEN); 

		#elif	1	// LOAD MULTIPLE (Assem)
				
				//MODEM_BlockCopy(0x74100000, (0x51100000+(g_ApDataCnt*0x1000)), 128); 	// 119ms
				//MODEM_BlockCopy(AP_RX_Queue, (AP_RX_BUFFER+(g_ApDataCnt*MODEM_TX_BUFLEN)), MODEM_TX_BUFLEN/8); 	//181ms
				MODEM_BlockCopy(AP_RX_Queue, (AP_RX_BUFFER+(g_ApDataCnt*MODEM_TX_BUFLEN)), 128); 	//181ms
		#endif
		
		g_ApDataCnt++;
		UART_Printf("[CNT= 0x%x ]\n", g_ApDataCnt);	

		if(g_ApDataCnt < DMA_COUNT) {
			MODEM_PRINTF("<--- 0xee00(APtoModem)\n");
			MODEM_PRINTF(" ------------------------\n");
			Outp16(DPSRAM_BASE+INT2MSM_ADR, 0xee00);
		} else {
			UART_Printf("<--- 0x1100(APtoModem)\n");
			UART_Printf(" ------------------------\n");
			Outp16(DPSRAM_BASE+INT2MSM_ADR, 0x1100);
		}

	}
	else if (temp == 0x2200)
	{
		UART_Printf("[AP received Modem's interrupt]\n");
		
	}
	else if(temp == 0x1100)
	{
		UART_Printf("[AP completed data reading]\n");
		g_APReadDone = 1;
	}
	else if(temp == 0x3300)
	{
		UART_Printf("[AP completed data reading using DMA]\n");
		g_APDMAReadDone = 1;
	}

	/////////////////////
	////// AP TX ISR /////
	////////////////////
	else if (temp == 0x00aa)
	{
		UART_Printf("MODEM RX READY OK....\n");
		g_ModemReadyReply = 1;
	}
	else if (temp == 0x00bb)
	{
		DMACH_Start(&oAPTx0);
	}
	else if (temp == 0x00dd)
	{
		UART_Printf("[AP TX Done!!!]\n");
		UART_Printf("<--- 0x00dd(APtoModem)\n");
		UART_Printf(" ------------------------\n");
		Outp16(DPSRAM_BASE+INT2MSM_ADR, 0x00dd);
		g_APDMAWriteDone = 1;
	}
	else if (temp == 0x00ff)
	{
		UART_Printf("MODEM RX READY OK....\n");
		g_ModemReadyReply = 1;
	}

	else if (temp == 0x00ee)
	{
		UART_Printf("MODEM RX....\n");
		g_ModemReadDoneReply = 1;
		//UART_Printf("\n[MODEM READ OK], g_ModemReadDoneReply = 0x%x\n", g_ModemReadDoneReply);
	}	

	else if (temp == 0x0011)
	{
		UART_Printf("[AP TX Done!!!]\n");
		Outp16(DPSRAM_BASE+INT2MSM_ADR, 0x0011);
	}
	
	else
	UART_Printf("[Error!!!: 0x%x ]\n",temp);	

	INTC_Enable(NUM_MSM);
	INTC_ClearVectAddr();

}

void MODEMIF_Int2AP(u32 int_add)
{
	Outp32(rINT2AP, int_add);
}

void MODEMIF_Int2Modem(u32 int_add)
{
	Outp32(rINT2MSM, int_add);
}
void MODEMIF_IntClear_Int2AP(void)
{
	Outp32(rMSMINTCLR, 0xffff);
}

u16 MODEMIF_IntClear_Int2MSM(void)
{
	u16 temp;

	temp = (Inp16(DPSRAM_BASE_MODEM+INT2MSM_ADR))&0xffff;
	Outp16(DPSRAM_BASE_MODEM+INT2MSM_ADR, temp);

	return temp;
}

void MODEMIF_Set_Modem_Booting_Port(void)
{
	//Set MODEM I/F GPIO Port
	GPIO_SetFunctionAll(eGPIO_N, 0x55555555, 0);	 //GPN3 : AP Reset, GPN8 : MODEM Reset Output
}

void MODEMIF_SetPort(void)
{
	//Set MODEM I/F GPIO Port
	GPIO_SetFunctionAll(eGPIO_K, 0x22222222, 0x22222222);	 //GPN[31:0] -> DATA[15:0]
	GPIO_SetFunctionAll(eGPIO_L, 0x22222222, 0x2222222);	 //GPN[31:0] -> ADDR[17:0]
	GPIO_SetFunctionAll(eGPIO_M, 0x222222, 0);	 //GPM[23:0] -> HOST I/F CS, CSn_MAIN, CSn_SUB, WEn, OEn, INTRn
	GPIO_SetFunctionAll(eGPIO_N, 0x55555555, 0);	 //GPN3 : AP Reset, GPN8 : MODEM Reset Output
}

void MODEMIF_MODEM_Reset(void)
{
	GPIO_SetDataEach(eGPIO_N, eGPIO_8, 1);
	Delay(10);
	GPIO_SetDataEach(eGPIO_N, eGPIO_8, 0);
	Delay(10);
	GPIO_SetDataEach(eGPIO_N, eGPIO_8, 1);
	Delay(10);
}

void MODEMIF_AP_Reset(void)
{
	GPIO_SetDataEach(eGPIO_N, eGPIO_3, 1);
	Delay(10);
	GPIO_SetDataEach(eGPIO_N, eGPIO_3, 0);
	Delay(10);
	GPIO_SetDataEach(eGPIO_N, eGPIO_3, 1);
	Delay(10);
}

void MODEMIF_AP_Init(u8 INT2MSMEN, u8 INT2APEN)
{
	// Modem I/F Set Port
	MODEMIF_SetPort();
	//SYSC_16bitBUS();

	//Interrupt Setting (MODEM, EINT)Isr_AP2MODEM
	INTC_SetVectAddr(NUM_MSM, Isr_MODEM2AP);		// for Modem Rx
	INTC_Enable(NUM_MSM);

	//MODEM I/F Interrupt enable
	Outp32(rMIFCON, (INT2MSMEN<<3)|(INT2APEN<<2));

	//INT Address Setting
	MODEMIF_Int2AP(INT2AP_ADR);		
	MODEMIF_Int2Modem(INT2MSM_ADR);

	//INT2AP Clear
	MODEMIF_IntClear_Int2AP();
}

void MODEMIF_MODEM_Init(u8 INT2MSMEN, u8 INT2APEN)
{
	//write
	//*(volatile unsigned *)0x70000004 =(0<<28)|(1<<24)|(0x3<<16)|(0<<12)|(1<<8)|(0<<4)|(0<<0);	// Fail. for SMDK6400 Rev0.2
	*(volatile unsigned *)0x70000004 =(0<<28)|(3<<24)|(0xb<<16)|(2<<12)|(2<<8)|(0<<4)|(0<<0);	// OK. for SMDK6400 Rev0.2
	//*(volatile unsigned *)0x70000004 =(0<<28)|(2<<24)|(0xf<<16)|(1<<12)|(2<<8)|(0<<4)|(0<<0);	// OK. for SMDK6400 Rev0.2

 	// ROM Bus 16bit setting
	SYSC_16bitBUS();
	
	// Modem I/F Interrupt request GPIO setting
	GPIO_SetPullUpDownEach(eGPIO_N, eGPIO_4, 0x0); // Pull Down Disable
	GPIO_SetEint0(4, Falling_Edge, eDLYFLT, 0);

	// Interrupt Setting (MODEM : EINT)
	INTC_SetVectAddr(NUM_EINT1, Isr_AP2MODEM);		// NUM_EINT1 : Eint[11:4]
	INTC_Enable(NUM_EINT1);

	// MODEM I/F Interrupt enable
	Outp32(rMIFCON, (INT2MSMEN<<3)|(INT2APEN<<2));

	// Clear the previous pending states
	GPIO_EINT0ClrPend(4);
	GPIO_EINT0DisMask(4);

	// AP2MODEM Interrupt Clear
	MODEMIF_IntClear_Int2MSM();
}

void MODEMIF_SET_APDMA(MODEMIF_XFER_MODE eRxTx, MODEMIF_DMA_REQ_NUM eSrcNum, u8 ucEn)
{
	u32	rTempReg = 0;
	
	Assert(eRxTx==MODEMIF_AP_TX || eRxTx==MODEMIF_AP_RX);

	rTempReg = Inp32(rMIFCON);
	rTempReg = rTempReg | (ucEn<<(eSrcNum+16) );
	Outp32(rMIFCON, rTempReg);
}

void MODEMIF_SET_DMA_REQ_ADDR(MODEMIF_XFER_MODE eRxTx, MODEMIF_DMA_REQ_NUM eSrcNum, u16 usAddr)
{
	Assert(eRxTx==MODEMIF_AP_TX || eRxTx==MODEMIF_AP_RX);

	if ( eRxTx == MODEMIF_AP_TX )			//	AP Tx --> Modem Rx
	{
		Outp32(rDMA_TX_ADR, usAddr<<(eSrcNum*16));
	}
	else if ( eRxTx == MODEMIF_AP_RX )		//	AP Rx <-- Modem Tx
	{
		Outp32(rDMA_RX_ADR, usAddr<<((eSrcNum-2)*16));
	}	
}

