#include "2440addr.h"

void Uart0_Interrupt_Init(void)
{

	rINTMSK 	&= ~(1 << 28) ;
	rINTSUBMSK 	&= ~((1 << 0) | (1 << 1)) ;
	
}

