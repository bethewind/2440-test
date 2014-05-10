#include "beep.h"
#include "2440addr.h"

void Beep_Init(void)
{
	rGPBCON &= ~(3 << 0) ;
	rGPBCON |= (1<<0) ;
	rGPBUP  &=  ~(1 << 0) ;
	rGPBDAT &= ~(1 << 0);
}










