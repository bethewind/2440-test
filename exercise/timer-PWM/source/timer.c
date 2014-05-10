#include "timer.h"
#include "config.h"





void Timer0_Init(void)
{
	rGPBCON &= ~(3 << 0) ;
	rGPBCON |= 1 << 1 ;
	
	rTCFG0 &= ~(0xFF) ;
	rTCFG0 |= 99 ;
	rTCFG1 &= ~(0xf) ;
	rTCFG1 |= 0x02 ;
	rTCNTB0 = 62 ;//1sÖÐ¶ÏÒ»´Î
	rTCMPB0 = rTCNTB0/6 ;
	rTCON |=  (1 << 1)  ;
	rTCON = 0x0D ;
	
}