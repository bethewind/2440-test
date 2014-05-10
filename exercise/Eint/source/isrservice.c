#include"config.h"
#include"isrservice.h"
#include"ledflow.h"

void Isr_Init(void)
{
	pISR_EINT0 = (unsigned int)Eint0_Isr ;
	pISR_EINT1 = (unsigned int)Eint1_Isr ;
	pISR_EINT2 = (unsigned int)Eint2_Isr ;
	pISR_EINT4_7 = (unsigned int)Eint4_7_Isr ;/**/
}


void __irq Eint0_Isr(void)
{
	Led1_On();Delay1s() ;Led1_Off() ;
	
	rSRCPND |= 1 << 0 ;//清除这一位的话，会自动清除INTOFFSET这一位的；
	rINTPND |= 1 << 0 ;
	
}

void __irq Eint1_Isr(void)
{
	Led2_On();Delay1s() ;Led2_Off() ;
	
	rSRCPND |= 1 << 1 ;//清除这一位的话，会自动清除INTOFFSET这一位的；
	rINTPND |= 1 << 1 ;
}

void __irq Eint2_Isr(void)
{
	Led3_On();Delay1s() ;Led3_Off() ;
	
	rSRCPND |= 1 << 2 ;//清除这一位的话，会自动清除INTOFFSET这一位的；
	rINTPND |= 1 << 2 ;
}

void __irq Eint4_7_Isr(void)
{
	
	if(rEINTPEND & (1 << 4) ) //EINTPEND记载着具体是哪个外部中断发生了
	{
		Led4_On();Delay1s() ;Led4_Off() ;
	
		rEINTPEND |= 1 << 4 ;
	}
	rSRCPND   |= 1 << 4 ;
	rINTPND   |= 1 << 4 ;
}







