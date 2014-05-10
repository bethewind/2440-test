#include "ledflow.h"
#include "2440addr.h"

void Led_Init(void)
{
	rGPBCON &= ~((3 << 10) | (3 << 12) | (3 << 14) |(3 << 16));
	rGPBCON |= ((1<<10) | (1<<12) | (1<<14) | (1<<16)) ;
	rGPBUP  &=  ~((1 << 5) | (1 << 6) | (1 << 7) || (1 << 8) ) ;
	rGPBDAT |= (1 << 5) | (1 << 6) | (1 << 7) | (1 << 8) ;
}









