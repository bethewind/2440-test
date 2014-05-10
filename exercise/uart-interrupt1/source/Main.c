#include "isrservice.h"
#include "interrupt.h"
#include "uart.h"
#include "ledflow.h"
#include "common.h"
void IO_Init() ;

int Main()
{
	IO_Init() ;
	
	while(1)
	{	
		Led2_On() ;
	} 
	return 0;	
}

void IO_Init()
{

	Uart0_Init(115200) ;
	Uart0_Interrupt_Init() ;
	Isr_Init() ;
	Led_Init() ;
}


