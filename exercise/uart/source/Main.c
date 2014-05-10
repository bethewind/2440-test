#include "uart.h"



int Main()
{
	unsigned char a ;
	Uart0_Init(115200) ;
	
	while(1)
	{	
		//a = getc() ;
		putc('a') ;	
			
	} 
	return 0;	
}






