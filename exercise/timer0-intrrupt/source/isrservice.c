#include"config.h"
#include"isrservice.h"

extern unsigned int flag ;

void Isr_Init(void)
{
	pISR_TIMER0 = (U32)Timer0_Isr ;
	
}

void __irq Timer0_Isr(void)
{
	flag = !flag ;
	rSRCPND |= 1 << 10 ;
	rINTPND |= 1 << 10 ;
}







