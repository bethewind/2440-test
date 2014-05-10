#include "2440addr.h"

void Timer0_Interrupt_Init(void)
{

	rINTMSK &= ~(1 << 10) ;
}

