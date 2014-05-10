#include "ledflow.h"
#include "isrservice.h"
#include "interrupt.h"
#include "timer.h"

void IO_Init() ;

unsigned int flag = 0 ;

int Main()
{
	IO_Init() ;
	
	while(1)
	{	
		if(flag ) 
		{
			
			Led2_On() ;
		}
		else
		{
			Led2_Off() ;
		}
	} 
	return 0;	
}

void IO_Init()
{
	Led_Init() ;
	Timer0_Init() ;
	Timer0_Interrupt_Init() ;
	Isr_Init() ;
}


