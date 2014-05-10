#include <stdio.h>
#include <string.h>
#include "def.h"
#include "option.h"
#include "2440addr.h"
#include "2440lib.h"

#include "Uart_test.h"

#include "Uart0.h"
#include "Uart1.h"
#include "Uart2.h"


void * func_uart_test[][2]=
{	
//									    "0123456789012345" max 15자 로한정하여 comment하세요.
//UART
	(void *)Test_Uart0_Int, 								"UART0 Int      ",
	(void *)Test_Uart0_Dma, 								"UART0 DMA      ",
	(void *)Test_Uart0_Fifo,								"UART0 FIFO     ",
	(void *)Test_Uart0_AfcTx, 							"UART0 AFC Tx   ",
	(void *)Test_Uart0_AfcRx, 							"UART0 AFC Rx   ",
	(void *)Test_Uart0_RxErr,							"UART0 RxErr    ",


	(void *)Test_Uart1_Int, 								"UART1 Int      ",
	(void *)Test_Uart1_Dma, 								"UART1 DMA      ",
	(void *)Test_Uart1_Fifo,								"UART1 FIFO     ",
	(void *)Test_Uart1_AfcTx, 							"UART1 AFC Tx   ",
	(void *)Test_Uart1_AfcRx, 							"UART1 AFC Rx   ",

	(void *)Test_Uart2_Int, 								"UART2 Int      ",
	(void *)Test_Uart2_Dma, 								"UART2 DMA      ",
	(void *)Test_Uart2_Fifo,								"UART2 FIFO     ",

	//	(void *)Test_Rts_Rx,									"UART0 RTS test ",
		(void *)Test_Uart0_Temp,							"UART0 temp      ",
	0,0
};

void Uart_Test(void)
{
	int i;
	
	Uart_Printf("\n======  UART Test program start ======\n");
		
	while(1)
	{
		i=0;
		Uart_Printf("01: UART0 Int\n");
		Uart_Printf("02: UART0 DMA\n");
		Uart_Printf("03: UART0 FIFO  \n");
		Uart_Printf("04: UART0 AFC Tx\n");
		Uart_Printf("05: UART0 AFC Rx\n");
		Uart_Printf("06: UART0 RxErr\n");
		Uart_Printf("\n\n");
		Uart_Printf("07: UART1 Int\n");
		Uart_Printf("08: UART1 DMA\n");
		Uart_Printf("09: UART1 FIFO \n");
		Uart_Printf("10: UART1 AFC Tx\n");
		Uart_Printf("11: UART1 AFC Rx\n");
		Uart_Printf("\n\n");
		Uart_Printf("12: UART2 Int\n");
		Uart_Printf("13: UART2 DMA \n");
		Uart_Printf("14: UART2 FIFO\n");
		Uart_Printf("\n\n");
		Uart_Printf("15: UART0 temp\n");
		Uart_Printf("\n======================================\n");

		Uart_Printf("\nPress Enter key to exit : ");
		i = Uart_GetIntNum();
		if(i==-1) break;		// return.
		switch(i)
		{
		    case 1:	 Test_Uart0_Int();
			         break;
		    case 2:	 Test_Uart0_Dma();
			         break;
		    case 3:	 Test_Uart0_Fifo();
			         break;
		    case 4:	 Test_Uart0_AfcTx();
			         break;
		    case 5:	 Test_Uart0_AfcRx();
			         break;
		    case 6:	 Test_Uart0_RxErr();
			         break;
		    case 7:	 Test_Uart1_Int();
			         break;
		    case 8:	 Test_Uart1_Dma();
			         break;
		    case 9:	 Test_Uart1_Fifo();
			         break;
		    case 10: Test_Uart1_AfcTx();
			         break;
		    case 11: Test_Uart1_AfcRx();
			         break;
		    case 12: Test_Uart2_Int();
			         break;
		    case 13: Test_Uart2_Dma();
			         break;
		    case 14: Test_Uart2_Fifo();
			         break;
		    case 15: Test_Uart0_Temp();
			         break;
		    default: Uart_Printf("Please enter(1~15) to choose! ");
			         break;
	
		}

//		if(i>=0 && (i<((sizeof(func_uart_test)-1)/8)) )	// select and execute...
//			( (void (*)(void)) (func_uart_test[i][0]) )();
	}
	
	Uart_Printf("\n====== UART Test program end ======\n");
}




