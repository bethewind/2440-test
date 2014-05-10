#include "adc.h"
#include "uart.h"

void IO_Init(void) ;
//unsigned int flag = 0 ;

int Main(valueoid)
{
	int value ;
	
	IO_Init() ;
	while(1)
	{	
		value = ADC_Read();
		putc(value/1000 + '0') ;
		putc(value%1000/100 + '0') ;
		putc(value%100/10 + '0') ;
		putc(value%10 + '0') ;
		
		Delay() ;
	
	} 
	return 0;	
}

void IO_Init(void)
{
	//Led_Init() ;
	//Isr_Init() ;
	//Irq_Init() ;
	ADC_Init(2) ;
	Uart0_Init(115200) ;
}

