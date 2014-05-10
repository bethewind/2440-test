#include "timer.h"
#include "config.h"





void Timer0_Init(void)
{
	rTCFG0 &= ~(0xFF) ;
	rTCFG0 |= 99 ;
	rTCFG1 &= ~(0xf) ;
	rTCFG1 |= 0x02 ;
	rTCNTB0 = 62500 ;//1s÷–∂œ“ª¥Œ
	rTCON |=  (1 << 1)  ;
	rTCON = 0x09 ;
	
}