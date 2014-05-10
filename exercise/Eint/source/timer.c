#include "timer.h"
#include "config.h"





void Timer0_Init(void)
{
	rTCFG0 &=~(0xff) ;
	rTCFG0 |= 99 ;
	rTCFG1 &= ~(0xf) ;
	rTCFG1 |= 0X02 ;
	rTCNTB0 = 62500 ;//1s中断一次
	
	rTCON |= (1 << 1) ;//手动更新
	rTCON = 0x09 ; //自动加载，清除手动更新位，启动定时器
	
}