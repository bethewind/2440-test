#include "common.h"
#include "ledflow.h"
#include "button.h"
#include "isrservice.h"


void IO_Init() ;


int Main()
{
	IO_Init() ;
	
	while(1)
	{	
	//	Led4_On();Delay1s() ;Led4_Off() ;
	
	} 
	return 0;	
}

void IO_Init()
{
	Led_Init() ;
	Key_Init() ;
	Isr_Init() ;
	Irq_Init() ;

}
