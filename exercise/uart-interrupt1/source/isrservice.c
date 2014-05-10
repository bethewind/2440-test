#include"config.h"
#include"isrservice.h"
#include"ledflow.h"

void Isr_Init(void)
{
	pISR_UART0 = (U32)Uart0_Isr ;
	
}

void __irq Uart0_Isr(void)
{
	unsigned char buf ;
	if(rSUBSRCPND & (1 << 0))//接收中断
	{
		buf = rURXH0 ;
		rUTXH0 = buf ;
		Led1_On() ;
		rSUBSRCPND |= 1 << 0 ;//清除接收中断 
	}
	if(rSUBSRCPND & (1 << 1))//发送中断
	{
		Led2_On() ;
		rSUBSRCPND |= 1 << 1 ;
	}
	rSRCPND |= 1 << 28 ;
	rINTPND |= 1 << 28 ;
}







