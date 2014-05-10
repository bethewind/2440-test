#include "led.h"
#include "config.h"
#include "common.h"

void Led_Init()
{
	rGPBCON &= ~((3 << 10) | (3 << 12) | (3 << 14) |(3 << 16));
	rGPBCON |= (LED1 | LED2 | LED3 | LED4) ;
	rGPBDAT |= LED1OFF | LED2OFF | LED3OFF | LED4OFF ;//还是初始化的问题
}

void Led1_On()
{
	rGPBDAT &= 0xFDF;//LED1ON ;1111 1101 1111
}
void Led1_Off()
{
	rGPBDAT |= LED1OFF ;
}
void Led2_On()
{
	rGPBDAT &= LED2ON ;
}
void Led2_Off()
{
	rGPBDAT |= LED2OFF ;
}
void Led3_On()
{
	rGPBDAT &= LED3ON ;
}
void Led3_Off()
{
	rGPBDAT |= LED3OFF ;
}
void Led4_On()
{
	rGPBDAT &= LED4ON ;
}
void Led4_Off()
{
	rGPBDAT |= LED4OFF ;
}
void Led_Flow()
{
	Led1_On();Delay1s() ;Led1_Off() ;
	Led2_On();Delay1s() ;Led2_Off() ;
	Led3_On();Delay1s() ;Led3_Off() ;
	Led4_On();Delay1s() ;Led4_Off() ;
}