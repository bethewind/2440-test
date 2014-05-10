#include "uart.h"

int Main()
{
	unsigned int a = 10 ;
	Uart0_Init(115200) ;
	
	while(1)
	{	
		Uart0_Printf("Uart0_Printf test output is:%d\n",a) ;
	} 
	return 0;	
}






