
//===================================================================
// NAME		: modemif_test.c	
// DESC		: S3C6410 DPSRAM test 
// History	: 01.03.2007 ver 0.0 by JAE GEUN. SONG
// 			: 03.06.2008 ver 0.1 by Sun il, Roe	for S3C6410
//			:				Modem DMA functions are added.
//			:				Modem DMA means DMA transaction between DPSRAM and DRAM in AP side.
//===================================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "option.h"
#include "sfr6410.h"
#include "def.h"
#include "intc.h"
#include "library.h"
#include "sysc.h"
#include "nand.h"
#include "modemif.h"
#include "gpio.h"
#include "timer.h"
#include "dma.h"


DMAC oModemif0;
DMAC oAPRx0, oAPTx0;
u8		ucModemDMAPort;

extern volatile int g_ApDataCnt;
extern volatile int g_MsmDataCnt;
extern volatile int g_DmaDone;
volatile int	g_DmaDoneMTx, g_DmaDoneMRx, g_DmaDoneATx, g_DmaDoneARx;
volatile int 	g_APRx0DmaDone;
extern volatile int g_ModemReadDone;			
extern volatile int g_ModemWriteDone;		// Write Done flag of Modem side
extern volatile int g_APReadDone;				// Read Done flag of AP side
extern volatile int g_APDMAReadDone;			// DMA Read Done flag of AP side
extern volatile int g_APDMAWriteDone;			// DMA Write Done flag of AP side
extern volatile int g_ModemReadyReply;
extern volatile int g_ModemReadDoneReply;

void MODEM_BlockCopy(u32, u32, u32 ); 

//MODEM side DMA TX ISR when AP RX using normal RX operation
void __irq Dma0Done_MODEMTX(void)
{
  	DMACH_ClearIntPending(&oModemif0);
	//UART_Printf ("DMA ISR %d\n", g_DmaDone);

	if (g_DmaDone == 0) {
		MODEM_PRINTF(" 0xff00(ModemSideDMA:TxBufferToTxQueue) --->\n");
		Outp16(DPSRAM_BASE_MODEM+INT2AP_ADR, 0xff00);
		MODEM_PRINTF(" ----------------------------\n");
	} else {
		MODEM_PRINTF(" 0xee00(ModemSideDMA:TxBufferToTxQueue) --->\n");
		Outp16(DPSRAM_BASE_MODEM+INT2AP_ADR, 0xee00);
		MODEM_PRINTF(" ----------------------------\n");
	}	
	
	g_DmaDone++;
	
	//DMACH_Setup(DMA_A, 0x0, MSM_TX_BUFFER+(g_DmaDone*0x1000) , 0, MSM_TX_Queue, 0, WORD, 0x400, DEMAND, MEM, MEM, BURST4, &oModemif0);
	DMACH_Setup(DMA_A, 0x0, MSM_TX_BUFFER+(g_DmaDone*MODEM_TX_BUFLEN) , 0, MSM_TX_Queue, 0, WORD, MODEM_TX_BUFLEN/WORD, DEMAND, MEM, MEM, BURST4, &oModemif0);

	INTC_ClearVectAddr();
}

//MODEM side DMA RX ISR when AP RX using normal TX operation
void __irq Dma0Done_MODEMRX(void)
{
  	DMACH_ClearIntPending(&oModemif0);
	//UART_Printf ("DMA ISR %d\n", g_DmaDone);
#if 0
	if (g_DmaDone == 0) {
		MODEM_PRINTF(" 0xff00(ModemSideDMA:TxBufferToTxQueue) --->\n");
		Outp16(DPSRAM_BASE_MODEM+INT2AP_ADR, 0xff00);
		MODEM_PRINTF(" ----------------------------\n");
	} else {
		MODEM_PRINTF(" 0xee00(ModemSideDMA:TxBufferToTxQueue) --->\n");
		Outp16(DPSRAM_BASE_MODEM+INT2AP_ADR, 0xee00);
		MODEM_PRINTF(" ----------------------------\n");
	}	
#else	
	if(g_DmaDone <= BLK_COUNT) {
		MODEM_PRINTF(" 0x00ee(ModemtoAP) --->\n");
		MODEM_PRINTF(" ------------------------\n");
		Outp16(DPSRAM_BASE_MODEM+INT2AP_ADR, 0x00ee);
	} else {
		MODEM_PRINTF(" 0x0011(ModemtoAP) --->\n");
		MODEM_PRINTF(" ------------------------\n");
		Outp16(DPSRAM_BASE_MODEM+INT2AP_ADR, 0x0011);
	}
#endif	
	g_DmaDone++;
	
	//DMACH_Setup(DMA_A, 0x0, MSM_TX_BUFFER+(g_DmaDone*0x1000) , 0, MSM_TX_Queue, 0, WORD, 0x400, DEMAND, MEM, MEM, BURST4, &oModemif0);
	DMACH_Setup(DMA_A, 0x0, MSM_TX_Queue, 0, MSM_TX_BUFFER+(g_DmaDone*AP_TX_BUFLEN), 0, WORD, AP_TX_BUFLEN/WORD, DEMAND, MEM, MEM, BURST4, &oModemif0);

	INTC_ClearVectAddr();
}

// MODEM side DMA TX ISR when AP RX using DMA RX operation
void __irq Dma0Done_ModemDMATx(void)
{
  	DMACH_ClearIntPending(&oModemif0);
	//UART_Printf ("DMA ISR %d\n", g_DmaDone);

	if (g_DmaDoneMTx == 0) {
		MODEM_PRINTF(" Write 0x1 to DMA_RX_ADR0 of AP in order to request AP RX0 DMA --->\n");
		Outp16(DPSRAM_BASE_MODEM+DMA_RX_ADDR0+ucModemDMAPort*2, 0xff00);
		MODEM_PRINTF(" ----------------------------\n");
	} else {
		MODEM_PRINTF(" Write 0x1 to DMA_RX_ADR0 of AP in order to request AP RX0 DMA --->\n");
		Outp16(DPSRAM_BASE_MODEM+DMA_RX_ADDR0+ucModemDMAPort*2, 0xee00);
		MODEM_PRINTF(" ----------------------------\n");
	}	
	
	g_DmaDoneMTx++;
	UART_Printf ("DMA ISR Cnt : %d\n", g_DmaDoneMTx);
	
	//DMACH_Setup(DMA_A, 0x0, MSM_TX_BUFFER+(g_DmaDone*0x1000) , 0, MSM_TX_Queue, 0, WORD, 0x400, DEMAND, MEM, MEM, BURST4, &oModemif0);
	DMACH_Setup(DMA_A, 0x0, MSM_TX_BUFFER+(g_DmaDoneMTx*MODEM_TX_BUFLEN), 0, MSM_TX_Queue, 0, WORD, MODEM_TX_BUFLEN/WORD, DEMAND, MEM, MEM, BURST4, &oModemif0);
	
	INTC_ClearVectAddr();
}

//MODEM side DMA RX ISR when AP RX using DMA TX operation
void __irq Dma0Done_ModemDMARX(void)
{
  	DMACH_ClearIntPending(&oModemif0);
	//UART_Printf ("DMA ISR %d\n", g_DmaDone);
	g_DmaDoneMRx++;
#if 0
	if (g_DmaDone == 0) {
		MODEM_PRINTF(" 0xff00(ModemSideDMA:TxBufferToTxQueue) --->\n");
		Outp16(DPSRAM_BASE_MODEM+INT2AP_ADR, 0xff00);
		MODEM_PRINTF(" ----------------------------\n");
	} else {
		MODEM_PRINTF(" 0xee00(ModemSideDMA:TxBufferToTxQueue) --->\n");
		Outp16(DPSRAM_BASE_MODEM+INT2AP_ADR, 0xee00);
		MODEM_PRINTF(" ----------------------------\n");
	}	
#else	
	if(g_DmaDoneMRx < BLK_COUNT) {
		MODEM_PRINTF(" 0x00bb(ModemtoAP) --->\n");
		MODEM_PRINTF(" ------------------------\n");
		//Outp16(DPSRAM_BASE_MODEM+INT2AP_ADR, 0x00bb);
		Outp16(DPSRAM_BASE_MODEM+DMA_TX_ADDR0+ucModemDMAPort*2, 0x00bb);
		Inp16(DPSRAM_BASE_MODEM + DMA_TX_ADDR0+ucModemDMAPort*2);
		
		DMACH_Setup(DMA_A, 0x0, MSM_RX_Queue, 0, MSM_RX_BUFFER+(g_DmaDoneMRx*AP_TX_BUFLEN), 0, WORD, AP_TX_BUFLEN/WORD, DEMAND, MEM, MEM, BURST4, &oModemif0);
	} else {
		MODEM_PRINTF(" 0x00dd(ModemtoAP) --->\n");
		MODEM_PRINTF(" ------------------------\n");
		Outp16(DPSRAM_BASE_MODEM+INT2AP_ADR, 0x00dd);
	}
#endif	
	

	INTC_ClearVectAddr();
}

//AP DMA ISR
void __irq Dma0Done_APDMATx(void)
{
	u32	uProtocol=0;

	INTC_Disable(NUM_DMA0);
  	DMACH_ClearIntPending(&oAPTx0);

	g_DmaDoneATx++;

	uProtocol = (Inp16(DPSRAM_BASE_AP + DMA_TX_ADDR0+ucModemDMAPort*2))&0xffff;
	//uProtocol = (Inp16(DPSRAM_BASE_AP + INT2AP_ADR))&0xffff;
	MODEM_PRINTF("AP DMA Tx Received : (Protocol=0x%x,Count=%d)\n", uProtocol, g_DmaDoneATx);

	if(uProtocol == 0x00bb)
	{
		if (g_DmaDoneATx < DMA_COUNT) {
			MODEM_PRINTF("<--- 0x00bb(APtoModem)\n");
			MODEM_PRINTF(" ------------------------\n");
			Outp16(DPSRAM_BASE+INT2MSM_ADR, 0x00bb);

			INTC_Enable(NUM_DMA0);
			DMACH_Setup(DMA_A, 0x0, AP_TX_BUFFER+(g_DmaDoneATx*AP_TX_BUFLEN), 0, AP_TX_Queue, 0, WORD, AP_TX_BUFLEN/WORD,DEMAND, MEM, (DREQ_SRC)(DMA0_MODEM_TX+ucModemDMAPort), BURST4, &oAPTx0);
			DMACH_Start(&oAPTx0);
		} else if (g_DmaDoneATx == DMA_COUNT) {
			MODEM_PRINTF("<--- 0x00bb(APtoModem)\n");
			MODEM_PRINTF(" ------------------------\n");
			Outp16(DPSRAM_BASE+INT2MSM_ADR, 0x00bb);
		}	
	}
	
	INTC_ClearVectAddr();
}

//AP DMA ISR
void __irq Dma0Done_APDMARx(void)
{
	u32	uProtocol=0;

	INTC_Disable(NUM_DMA0);
  	DMACH_ClearIntPending(&oAPRx0);

	uProtocol = (Inp16(DPSRAM_BASE_AP + DMA_RX_ADDR0+ucModemDMAPort*2))&0xffff;
	MODEM_PRINTF("AP Rx DMA Received : (Protocol=0x%x,Count=%d)\n", uProtocol, g_DmaDoneARx);

	if( uProtocol == 0xff00)
	{
		g_DmaDoneARx++;
		if (g_DmaDoneARx < DMA_COUNT) {
			MODEM_PRINTF("<--- 0xee00(APtoModem)\n");
			MODEM_PRINTF(" ------------------------\n");
			Outp16(DPSRAM_BASE+INT2MSM_ADR, 0xee00);
		} else {
			UART_Printf("<--- 0x1100(APtoModem)\n");
			UART_Printf(" ------------------------\n");
			Outp16(DPSRAM_BASE+INT2MSM_ADR, 0x1100);
		}	
	}
	else if(uProtocol == 0xee00)
	{
		g_DmaDoneARx++;
		if (g_DmaDoneARx < DMA_COUNT) {
			MODEM_PRINTF("<--- 0xee00(APtoModem)\n");
			MODEM_PRINTF(" ------------------------\n");
			Outp16(DPSRAM_BASE+INT2MSM_ADR, 0xee00);
		} else {
			UART_Printf("<--- 0x1100(APtoModem)\n");
			UART_Printf(" ------------------------\n");
			Outp16(DPSRAM_BASE+INT2MSM_ADR, 0x1100);
		}	
	}
#if 0	
	else if (uProtocol == 0x2200)
	{
		UART_Printf("[AP received Modem's interrupt]\n");
		
	}
	else if(uProtocol == 0x1100)
	{
		UART_Printf("[AP Rx DMA completed data reading]\n");
	}	
#endif	
	INTC_Enable(NUM_DMA0);
	DMACH_Setup(DMA_A, 0x0, AP_RX_Queue, 0, AP_RX_BUFFER+(g_DmaDoneARx*MODEM_TX_BUFLEN), 0, WORD, MODEM_TX_BUFLEN/WORD, DEMAND, (DREQ_SRC)(DMA0_MODEM_RX+ucModemDMAPort), MEM, BURST4, &oAPRx0);
	DMACH_Start(&oAPRx0);
	
	INTC_ClearVectAddr();
}

//======================= Interrupt Test Functions ===========================
void Test_AP_TX_INTR(void)
{
	u32	uLootCnt = 0;
	//Message
	UART_Printf("AP TX Interrupt Test\n");

	g_ModemReadDoneReply = 0;
	
	// Modem i/f setting
	MODEMIF_AP_Init(true, true);

	Outp16(DPSRAM_BASE_AP+INT2MSM_ADR, 0x0022);

	while(*(volatile u16 *)(DPSRAM_BASE_AP+INT2MSM_ADR) != 0x0);		// busy waiting to clear INT2MSM register
	
	UART_Printf("AP TX Interrupt Test OK %d!!\n", uLootCnt++);

}

void Test_MODEM_TX_INTR(void)
{
	UART_Printf("MODEM TX to test AP's Modem interrupt Test!!!\n\n");
	
	g_DmaDone = 0;

	MODEMIF_MODEM_Init(true, true);

	Outp16(DPSRAM_BASE_MODEM+INT2AP_ADR, 0x2200);
	

	// to AP message "STOP"
	//Outp16(DPSRAM_BASE_MODEM+INT2AP_ADR, 0x1100);

}

void Test_MODEM_SetClear_INTR(void)
{
	u16 usInt2AP = 0;
	u16 usTemp = 0;
	
	MODEMIF_MODEM_Init(true, true);

	usTemp = (Inp16(DPSRAM_BASE_MODEM+INT2MSM_ADR))&0xffff;
	UART_Printf("[ReadValue:0x%x]\n", usTemp);
	Outp16(DPSRAM_BASE_MODEM+INT2MSM_ADR, usTemp);

	Getc();
	UART_Printf(" > input interrupt set value(0x0~0xFFFF)\n");
	usInt2AP = (u16)UART_GetIntNum();
	usTemp = (Inp16(DPSRAM_BASE_MODEM+INT2MSM_ADR))&0xffff;
	UART_Printf("[ReadValue:0x%x, InputValue:0x%x]\n", usTemp,usInt2AP);
	Outp16(DPSRAM_BASE_MODEM+INT2MSM_ADR, usInt2AP);
}

//======================= Normal Operational Functions ===========================
void Test_AP_TX(void)
{

	u32 length,i;
	u32	*txdata;
	u32 *aptx_buf;

	//Message
	UART_Printf("AP TX\n");

	g_ModemReadyReply		= 0;
	
	txdata = (u32 *)(AP_TX_BUFFER);

	// Modem i/f setting
	MODEMIF_AP_Init(true, true);

	// AP TX Buffer data
	for(length=0 ; length <= (AP_TX_BUFLEN*BLK_COUNT) ; length++)
	{
		*(txdata+length) = length;
	}

	UART_Printf("<--- 0x00ff(APtoModem)\n");
	UART_Printf(" ------------------------\n");
	Outp16(DPSRAM_BASE_AP+INT2MSM_ADR, 0x00ff);
	while(!g_ModemReadyReply);

	for(i=0 ; i<BLK_COUNT ; i++)
	{
		g_ModemReadDoneReply = 0;
		
		// TX Buffer pointer setting
		txdata = (u32 *)(AP_TX_BUFFER+(i*AP_TX_BUFLEN));
		aptx_buf = (u32 *)(AP_TX_Queue);

		for (length = 0 ; length < AP_TX_BUFLEN ; length++)
		{
			//UART_Printf("(%x,%x,%x) ", length,aptx_buf,*txdata);
			*(u32 *)(aptx_buf ++) = *(txdata++);
		}
		UART_Printf("<--- 0x00ee(APtoModem)\n");
		UART_Printf(" ------------------------\n");
		Outp16(DPSRAM_BASE_AP+INT2MSM_ADR, 0x00ee);
		
		while(!g_ModemReadDoneReply);
		UART_Printf(" Block Count = 0x%x\n", i);
	}
	UART_Printf("<--- 0x0011(APtoModem)\n");
	UART_Printf(" ------------------------\n");
	Outp16(DPSRAM_BASE+INT2MSM_ADR, 0x0011);
	UART_Printf("AP TX END\n");

}

void Test_AP_RX(void)
{
	g_ApDataCnt = 0;
	g_APReadDone = 0;
	
	MODEMIF_AP_Init(true, true);

	while(!((g_ApDataCnt == DMA_COUNT)&&(g_APReadDone == 1)));
	//UART_Printf("cnt = 0x%x, dma_cnt = 0x%x\n", g_ApDataCnt, DMA_COUNT);
	
	INTC_Disable(NUM_MSM);
}

void Test_MODEM_TX(void)
{
	u32 length;
	u32	*txdata;

	UART_Printf("MODEM TX TEST!!!\n\n");
	
	g_DmaDone = 0;
	g_ModemWriteDone=0;

	txdata = (u32 *)(MSM_TX_BUFFER);
	
	MODEMIF_MODEM_Init(true, true);

	for(length=0 ; length <= DMA_COUNT*MODEM_TX_BUFLEN/WORD ; length++)
	{
		*(txdata+length) = length;
	}

	// DMA Init & Setup
     	DMAC_InitCh(DMA0, DMA_ALL, &oModemif0);
	INTC_SetVectAddr(NUM_DMA0,  Dma0Done_MODEMTX);
	INTC_Enable(NUM_DMA0);
	DMACH_Setup(DMA_A, 0x0, MSM_TX_BUFFER, 0, MSM_TX_Queue, 0, WORD, MODEM_TX_BUFLEN/WORD, DEMAND, MEM, MEM, BURST4, &oModemif0);

        // DMA start
        UART_Printf("[ MODEM DMA START!!!]\n");	
	DMACH_Start(&oModemif0);

	// Wait Tx
	while(! ((g_DmaDone == DMA_COUNT) && (g_ModemWriteDone==1)));

	INTC_Disable(NUM_DMA0);
	
	// to AP message "STOP"
	UART_Printf(" 0x1100(ModemtoAP:STOP) --->\n");
	UART_Printf(" ----------------------------\n");
	Outp16(DPSRAM_BASE_MODEM+INT2AP_ADR, 0x1100);

}

void Test_MODEM_RX(void)
{
	g_DmaDone = 0;
	g_MsmDataCnt = 0;
	g_ModemReadDone	= 0;
	
	MODEMIF_MODEM_Init(true, true);

	// DMA Init & Setup
     	DMAC_InitCh(DMA0, DMA_ALL, &oModemif0);
	INTC_SetVectAddr(NUM_DMA0,  Dma0Done_MODEMRX);
	INTC_Enable(NUM_DMA0);
	DMACH_Setup(DMA_A, 0x0, MSM_TX_Queue, 0, MSM_TX_BUFFER, 0, WORD, AP_TX_BUFLEN/WORD, DEMAND, MEM, MEM, BURST4, &oModemif0);

        // DMA start
        UART_Printf("[ MODEM side Rx DMA START!!!]\n");	
	//DMACH_Start(&oModemif0);

	while(!((g_MsmDataCnt == BLK_COUNT)&&(g_ModemReadDone == 1)));
	UART_Printf("cnt = 0x%x, dma_cnt = 0x%x\n", g_MsmDataCnt, BLK_COUNT);
	
	INTC_Disable(NUM_EINT1);
}

//======================= Normal Operational Functions ===========================
void Test_AP_DMA_TX(void)
{

	u32 length;
	u32	*txdata;

	//Message
	UART_Printf("AP TX\n");

	g_DmaDoneATx = 0;

	g_ModemReadyReply		= 0;

	UART_Printf(" > Select  AP's modem DMA port(0~1)\n");
	ucModemDMAPort = (u8)UART_GetIntNum();
	UART_Printf("\n");
	if( (ucModemDMAPort!=0) && (ucModemDMAPort!=1))
	{
		UART_Printf(" --> Error!! You have been entered invalid DMA port number!!\n");
		return;
	}

	// Modem i/f setting
	MODEMIF_AP_Init(true, true);

	// AP TX Buffer data
	txdata = (u32 *)(AP_TX_BUFFER);
	for(length=0 ; length <= (AP_TX_BUFLEN*BLK_COUNT) ; length++)
	{
		*(txdata+length) = length;
	}

	UART_Printf("<--- 0x00aa(APtoModem)\n");
	UART_Printf(" ------------------------\n");
	Outp16(DPSRAM_BASE_AP+INT2MSM_ADR, 0x00aa);

	// DMA Init & Setup
     	DMAC_InitCh(DMA0, DMA_ALL, &oAPTx0);
	INTC_SetVectAddr(NUM_DMA0,  Dma0Done_APDMATx);
	INTC_Enable(NUM_DMA0);
	DMACH_Setup(DMA_A, 0x0, AP_TX_BUFFER, 0, AP_TX_Queue, 0, WORD, AP_TX_BUFLEN/WORD, DEMAND, MEM, (DREQ_SRC)(DMA0_MODEM_TX+ucModemDMAPort), BURST4, &oAPTx0);

	// Modem I/F DMA setup
	SYSC_SelectDMA((DMASELECT_eID)(eSEL_MODEM_TX0+ucModemDMAPort), 1);		// Set MODEM RX0 DMA as Normal DMA
	MODEMIF_SET_DMA_REQ_ADDR(MODEMIF_AP_TX, (MODEMIF_DMA_REQ_NUM)(MODEMIF_TX_0+ucModemDMAPort), DMA_TX_ADDR0+ucModemDMAPort*2);
	MODEMIF_SET_APDMA(MODEMIF_AP_TX, (MODEMIF_DMA_REQ_NUM)(MODEMIF_TX_0+ucModemDMAPort), 1);	// Enable AP DMA RX0 Request
	
        // DMA start
        UART_Printf("[ AP Tx DMA START!!!]\n");	
	 DMACH_Start(&oAPTx0);

	while(!g_ModemReadyReply);

	while(!((g_DmaDoneATx == DMA_COUNT)&&(g_APDMAWriteDone == 1)));
	
	UART_Printf("AP TX END\n");

}

void Test_AP_DMA_RX(void)
{
	g_DmaDoneARx = 0;
	g_APDMAReadDone = 0;
	
	UART_Printf(" > Select  AP's modem DMA port(0~1)\n");
	ucModemDMAPort = (u8)UART_GetIntNum();
	UART_Printf("\n");
	if( (ucModemDMAPort!=0) && (ucModemDMAPort!=1))
	{
		UART_Printf(" --> Error!! You have been entered invalid DMA port number!!\n");
		return;
	}
	
	MODEMIF_AP_Init(true, true);

	// DMA Init & Setup
     	DMAC_InitCh(DMA0, DMA_ALL, &oAPRx0);
	INTC_SetVectAddr(NUM_DMA0,  Dma0Done_APDMARx);
	INTC_Enable(NUM_DMA0);
	DMACH_Setup(DMA_A, 0x0, AP_RX_Queue, 0, AP_RX_BUFFER, 0, WORD, MODEM_TX_BUFLEN/WORD, DEMAND, (DREQ_SRC)(DMA0_MODEM_RX+ucModemDMAPort), MEM, BURST4, &oAPRx0);

	// Modem I/F DMA setup
	SYSC_SelectDMA((DMASELECT_eID)(eSEL_MODEM_RX0+ucModemDMAPort), 1);		// Set MODEM RX0 DMA as Normal DMA
	MODEMIF_SET_DMA_REQ_ADDR(MODEMIF_AP_RX, (MODEMIF_DMA_REQ_NUM)(MODEMIF_RX_0+ucModemDMAPort), DMA_RX_ADDR0+ucModemDMAPort*2);
	MODEMIF_SET_APDMA(MODEMIF_AP_RX, (MODEMIF_DMA_REQ_NUM)(MODEMIF_RX_0+ucModemDMAPort), 1);	// Enable AP DMA RX0 Request
	
        // DMA start
        UART_Printf("[ AP Rx DMA START!!!]\n");	
	 DMACH_Start(&oAPRx0);
	
	while(!((g_DmaDoneARx == DMA_COUNT)&&(g_APDMAReadDone == 1)));
	//UART_Printf("cnt = 0x%x, dma_cnt = 0x%x\n", g_DmaDoneARx, DMA_COUNT);
	
	INTC_Disable(NUM_MSM);
}

void Test_MODEM_DMA_TX(void)
{
	u32 length;
	u32	*txdata;

	UART_Printf("MODEM DMA TX TEST!!!\n");

	UART_Printf(" > Select  AP's modem DMA port(0~1)\n");
	ucModemDMAPort = (u8)UART_GetIntNum();
	UART_Printf("\n");
	if( (ucModemDMAPort!=0) && (ucModemDMAPort!=1))
	{
		UART_Printf(" --> Error!! You have been entered invalid DMA port number!!\n");
		return;
	}
	
	g_DmaDoneMTx = 0;
	g_ModemWriteDone=0;

	txdata = (u32 *)(MSM_TX_BUFFER);
	
	MODEMIF_MODEM_Init(true, true);

	for(length=0 ; length < DMA_COUNT*MODEM_TX_BUFLEN/WORD ; length++)
	{
		*(txdata+length) = length;
	}

	// DMA Init & Setup
     	DMAC_InitCh(DMA0, DMA_ALL, &oModemif0);
	INTC_SetVectAddr(NUM_DMA0,  Dma0Done_ModemDMATx);
	INTC_Enable(NUM_DMA0);
	DMACH_Setup(DMA_A, 0x0, MSM_TX_BUFFER, 0, MSM_TX_Queue, 0, WORD, MODEM_TX_BUFLEN/WORD, DEMAND, MEM, MEM, BURST4, &oModemif0);

        // DMA start
        UART_Printf("[ MODEM DMA START!!!]\n");	
	DMACH_Start(&oModemif0);

	// Wait Tx
	while(! ((g_DmaDoneMTx == DMA_COUNT) && (g_ModemWriteDone==1)));

	INTC_Disable(NUM_DMA0);
	
	// to AP message "STOP"
	UART_Printf(" 0x3300(ModemtoAP:DMA STOP) --->\n");
	UART_Printf(" ----------------------------\n");
	Outp16(DPSRAM_BASE_MODEM+INT2AP_ADR, 0x3300);

}

void Test_MODEM_DMA_RX(void)
{
	g_DmaDoneMRx = 0;
	//g_MsmDataCnt = 0;
	g_ModemReadDone	= 0;

	UART_Printf(" > Select  AP's modem DMA port(0~1)\n");
	ucModemDMAPort = (u8)UART_GetIntNum();
	UART_Printf("\n");
	if( (ucModemDMAPort!=0) && (ucModemDMAPort!=1))
	{
		UART_Printf(" --> Error!! You have been entered invalid DMA port number!!\n");
		return;
	}
	
	MODEMIF_MODEM_Init(true, true);

	// DMA Init & Setup
     	DMAC_InitCh(DMA0, DMA_ALL, &oModemif0);
	INTC_SetVectAddr(NUM_DMA0,  Dma0Done_ModemDMARX);
	INTC_Enable(NUM_DMA0);
	DMACH_Setup(DMA_A, 0x0, MSM_RX_Queue, 0, MSM_RX_BUFFER, 0, WORD, AP_TX_BUFLEN/WORD, DEMAND, MEM, MEM, BURST4, &oModemif0);

        // DMA start
        UART_Printf("[ MODEM side Rx DMA START!!!]\n");	
	//DMACH_Start(&oModemif0);

	while(!((g_DmaDoneMRx == BLK_COUNT)&&(g_ModemReadDone == 1)));
	UART_Printf("cnt = 0x%x, dma_cnt = 0x%x\n", g_DmaDoneMRx, BLK_COUNT);
	
	INTC_Disable(NUM_EINT1);
}

//======================= Concurrent & Performance Functions ===========================
void Test_AP_DMA_TX_RX(void)
{
	g_DmaDoneARx = 0;
	g_APDMAReadDone = 0;
	
	UART_Printf(" > Select  AP's modem DMA port(0~1)\n");
	ucModemDMAPort = (u8)UART_GetIntNum();
	UART_Printf("\n");
	if( (ucModemDMAPort!=0) && (ucModemDMAPort!=1))
	{
		UART_Printf(" --> Error!! You have been entered invalid DMA port number!!\n");
		return;
	}
	
	MODEMIF_AP_Init(true, true);

	// DMA Init & Setup
     	DMAC_InitCh(DMA0, DMA_ALL, &oAPRx0);
	INTC_SetVectAddr(NUM_DMA0,  Dma0Done_APDMARx);
	INTC_Enable(NUM_DMA0);
	DMACH_Setup(DMA_A, 0x0, AP_RX_Queue, 0, AP_RX_BUFFER, 0, WORD, MODEM_TX_BUFLEN/WORD, DEMAND, (DREQ_SRC)(DMA0_MODEM_RX+ucModemDMAPort), MEM, BURST4, &oAPRx0);

	// Modem I/F DMA setup
	SYSC_SelectDMA((DMASELECT_eID)(eSEL_MODEM_RX0+ucModemDMAPort), 1);		// Set MODEM RX0 DMA as Normal DMA
	MODEMIF_SET_DMA_REQ_ADDR(MODEMIF_AP_RX, (MODEMIF_DMA_REQ_NUM)(MODEMIF_RX_0+ucModemDMAPort), DMA_RX_ADDR0+ucModemDMAPort*2);
	MODEMIF_SET_APDMA(MODEMIF_AP_RX, (MODEMIF_DMA_REQ_NUM)(MODEMIF_RX_0+ucModemDMAPort), 1);	// Enable AP DMA RX0 Request
	
        // DMA start
        UART_Printf("[ AP Rx DMA START!!!]\n");	
	 DMACH_Start(&oAPRx0);
	
	while(!((g_DmaDoneARx == DMA_COUNT)&&(g_APDMAReadDone == 1)));
	//UART_Printf("cnt = 0x%x, dma_cnt = 0x%x\n", g_DmaDoneARx, DMA_COUNT);
	
	INTC_Disable(NUM_MSM);
}

void Test_MODEM_DMA_TX_RX(void)
{
	g_DmaDoneMRx = 0;
	//g_MsmDataCnt = 0;
	g_ModemReadDone	= 0;

	UART_Printf(" > Select  AP's modem DMA port(0~1)\n");
	ucModemDMAPort = (u8)UART_GetIntNum();
	UART_Printf("\n");
	if( (ucModemDMAPort!=0) && (ucModemDMAPort!=1))
	{
		UART_Printf(" --> Error!! You have been entered invalid DMA port number!!\n");
		return;
	}
	
	MODEMIF_MODEM_Init(true, true);

	// DMA Init & Setup
     	DMAC_InitCh(DMA0, DMA_ALL, &oModemif0);
	INTC_SetVectAddr(NUM_DMA0,  Dma0Done_ModemDMARX);
	INTC_Enable(NUM_DMA0);
	DMACH_Setup(DMA_A, 0x0, MSM_RX_Queue, 0, MSM_RX_BUFFER, 0, WORD, AP_TX_BUFLEN/WORD, DEMAND, MEM, MEM, BURST4, &oModemif0);

        // DMA start
        UART_Printf("[ MODEM side Rx DMA START!!!]\n");	
	//DMACH_Start(&oModemif0);

	while(!((g_DmaDoneMRx == BLK_COUNT)&&(g_ModemReadDone == 1)));
	UART_Printf("cnt = 0x%x, dma_cnt = 0x%x\n", g_DmaDoneMRx, BLK_COUNT);
	
	INTC_Disable(NUM_EINT1);
}

void Test_MODEM_Performance(void)
{

	u32 length, temp;
	u32	*txdata;

	//Message
	UART_Printf("MODEM I/F Performance Test!!!\n");

	g_DmaDone = 0;
	
	txdata = (u32 *)(MSM_TX_BUFFER);

	// Modem i/f setting
	MODEMIF_MODEM_Init(true, true);

	// MODEM TX Buffer data
	for(length=0 ; length <= DMA_COUNT*MODEM_TX_BUFLEN ; length++)
		*(txdata+length) = length;
	
     	DMAC_InitCh(DMA0, DMA_ALL, &oModemif0);
	INTC_SetVectAddr(NUM_DMA0,  Dma0Done_MODEMTX);
	INTC_Enable(NUM_DMA0);

        // DMA Channel Set-up
	DMACH_Setup(DMA_A, 0x0, MSM_TX_BUFFER, 0, MSM_TX_Queue, 0, WORD, 0x400, DEMAND, 
	                       MEM, MEM, BURST4, &oModemif0);

	StartTimer(0);

	Outp16(DPSRAM_BASE_MODEM+INT2AP_ADR, 0xff00);

        // Enable DMA
	DMACH_Start(&oModemif0);

	while(g_DmaDone < DMA_COUNT);

	INTC_Disable(NUM_DMA0);
	
	Outp16(DPSRAM_BASE_MODEM+INT2AP_ADR, 0x1100);

	temp = StopTimer(0);
//	UART_Printf("%dKB R/W Time = %dus\n",(4*DMA_COUNT), temp);	
	UART_Printf("%dKB R/W Time = %dus(%fMB/s)\n",(4*DMA_COUNT), temp, (float)((float)(4.*(float)DMA_COUNT/1000.)/(float)((float)temp/1000000.)));

	INTC_Disable(NUM_EINT1);

}


//======================= AP Booting & SleepWakeup Functions ===========================

extern NAND_oInform NAND_Inform[NAND_CONNUM];

void Test_AP_Booting(void)
{
	// MODEM I/F GPIO Setting
	MODEMIF_SetPort();

	// MODEM bootloader copy to DPSRAM
	NAND_Inform[0].uNandType = NAND_Normal8bit;
	NAND_Inform[0].uAddrCycle = 4;
	NAND_Inform[0].uBlockNum = 4096;
	NAND_Inform[0].uPageNum = 32;
	NAND_Inform[0].uPageSize = NAND_PAGE_512;
	NAND_Inform[0].uSpareSize = NAND_SPARE_16;
	NAND_Inform[0].uECCtest = 0;
	NAND_Inform[0].uSpareECCtest = 0;		// This line should be added for spare area ecc in S3C6410. If not, nand ecc would be fail right after reading first 512bytes. 080313. derrick.
	NAND_Inform[0].uTacls = 0;
	NAND_Inform[0].uTwrph0 = 25+10;	//Pad delay : about 10ns
	NAND_Inform[0].uTwrph1 = 15+5;	//tWH : 15ns
	
	NAND_Init(0);
	NAND_ReadMultiPage(0, 10, 0, (u8 *)0x74100000, 0x1000);
	Delay(100);

	// AP Booting MODEM Reset
	MODEMIF_MODEM_Reset();

}

void Test_SleepWakeup(void)
{
	//Outp16(DPSRAM_BASE_MODEM+INT2AP_ADR, 0xff00);

	//MODEMIF_MODEM_Init(true, true);

	UART_Printf("MODEM Wake-up source!!!\n");
	
	//Outp16(DPSRAM_BASE_MODEM+INT2AP_ADR, 0xff00);
	Outp32(SLEEP_WAKEUP, 0xffff);

}

void Test_WakeupClear(void)
{
	Outp32(SLEEP_WAKEUP_CLEAR, 0xffff);
}

//======================= DPSRAM Test Functions ===========================
void Test_Display_MEM(void)
{
	u32 i, length, add, size;

	UART_Printf("\nMEM DUMP Address : ");
	add = UART_GetIntNum();
	UART_Printf("add = 0x%x\n", add);

	UART_Printf("\nMEM DUMP Size : ");
	size = UART_GetIntNum();
	UART_Printf("size = 0x%x\n", size);

	UART_Printf("Press any key and DPSRAM R/W test start!!!\n");
	Getc();
	
		for (length = 0 ; length <= size ; length+=4)
		{
				i = Inp32(add + length);
				UART_Printf("%08x    ", i);
		}

	UART_Printf("\nDPSRAM Test........OK!!!\n");

}

void Test_DPSRAM(void)
{
	// ALL DPSRAM TEST
	u32 i, length;

	// Modem i/f setting
	MODEMIF_MODEM_Init(true, true);

	UART_Printf("Press any key and DPSRAM R/W test start!!!\n");
	Getc();
	
	for(i=0; i<=0xffffffff; i++)
	{
		for (length = 0 ; length <= 0x1ffc ; length+=4)
		{
			*(unsigned int *)(DPSRAM_BASE + length) = i;

			if(*(unsigned int *)(DPSRAM_BASE + length) != i)
				UART_Printf("Error: %08x\n", DPSRAM_BASE + length);
		}
			UART_Printf("%08x ", i);
	}	

	UART_Printf("DPSRAM Test........OK!!!\n");

}

void Test_DPSRAM_OneTime(void)
{
	// ALL DPSRAM TEST
	u32 length;

	// Modem i/f setting
	MODEMIF_MODEM_Init(true, true);

	UART_Printf("Press any key and DPSRAM R/W test through ROM I/F start!!!\n");
	Getc();
	
	for (length = 0 ; length <= 0x1ffc ; length+=4)
	{
		*(unsigned int *)(DPSRAM_BASE_MODEM + length) = length;

		if(*(unsigned int *)(DPSRAM_BASE_MODEM + length) != length)
			UART_Printf("Error: %08x\n", DPSRAM_BASE_MODEM + length);
	}

	UART_Printf("DPSRAM Test........OK!!!\n");

}


void Test_BLOCKCOPY(void)
{
	//memcpy((int *)0x51000000, (int *) 0x74100000, 0x1000); 
	//MODEM_BlockCopy((int *)0x74100000, (int *)0x51000000, 0x4); 
	MODEM_BlockCopy((u32)((int *)0x74100000), (u32)((int *)0x51100000), 128); 
}

//////////////////////////////////////////////////////////////////////////////
void MODEM_Test(void)
{
	int i=0, nSel;

	const testFuncMenu modemif_function[]=
	{
		0,                          			"Exit",	
		Test_AP_TX_INTR,			"Test XhiINTR Interrupt signal",
		Test_MODEM_TX_INTR,		"Test MODEM TX to test AP's Modem Interrupt	",
		Test_MODEM_SetClear_INTR,	"Test AP's Interrupt Set & Clear in Modem side 	",
		0,							"---------------------------------------",
		Test_AP_TX,					"Test AP TX	",
		Test_AP_RX,					"Test AP RX	",
		Test_MODEM_TX,				"Test MODEM TX	",
		Test_MODEM_RX,				"Test MODEM RX    ",
		0,							"---------------------------------------",
		Test_AP_DMA_TX,			"Test AP TX using DMA	",
		Test_AP_DMA_RX,			"Test AP RX using DMA	",
		Test_MODEM_DMA_TX,		"Test MODEM TX using DMA	",
		Test_MODEM_DMA_RX,		"Test MODEM RX using DMA   ",
		0,							"---------------------------------------",
		//Test_AP_DMA_TX_RX,		"Test AP RX/TX concurrently using DMA   ",
		//Test_MODEM_DMA_TX_RX,	"Test MODEM RX/TX concurrently using DMA   ",
		Test_MODEM_Performance,	"Test MODEM I/F Performance	",
		0,							"---------------------------------------",
		Test_AP_Booting,			"Test AP Booting		",
		Test_SleepWakeup,			"Test Wake-Up Sleep Mode		",
		Test_WakeupClear, 			"Test Wake-Up Sleep Mode Clear		",
		0,							"---------------------------------------",
		Test_Display_MEM,			"Dump Memory	",
		Test_DPSRAM,				"Test DPSRAM (AP side)	",
		Test_DPSRAM_OneTime,		"Test DPSRAM (Modem side)	",
		Test_BLOCKCOPY,			"Test Block Copy	",
		0,							0
	};
	
	while (1)
	{
		UART_Printf("\n");
		for (i=0; (int)(modemif_function[i].desc)!=0; i++)
		{
			UART_Printf("%2d: %s\n", i,modemif_function[i].desc);
		}

		UART_Printf("==========================================================\n");			
		UART_Printf("\nSelect the function to test : ");

		nSel = UART_GetIntNum();

		UART_Printf("\n");

		if (nSel == 0)
			break;
		if (nSel>0 && nSel<(sizeof(modemif_function)/8-1))
			 (modemif_function[nSel].func)();
	}		
}

