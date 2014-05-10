#include "led.h"
#include "2440addr.h"


void Led_Init(void)
{
	rGPBCON &= ~(3 << 10);
	rGPBCON |= (1<<10) ;
	rGPBUP &=  ~(1 << 5) ;
	rGPBDAT |= (1 <<5);
}













/*void Led1_On(void)
{
	rGPBDAT &= (~(1 << 5));//LED1ON ;1111 1101 1111
}
void Led1_Off(void)
{
	rGPBDAT |= (1 << 5) ;
}*/