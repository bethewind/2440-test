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
	if(rSUBSRCPND & (1 << 0))//�����ж�
	{
		buf = rURXH0 ;
		rUTXH0 = buf ;
		Led1_On() ;
		rSUBSRCPND |= 1 << 0 ;//��������ж� 
	}
	if(rSUBSRCPND & (1 << 1))//�����ж�
	{
		Led2_On() ;
		rSUBSRCPND |= 1 << 1 ;
	}
	rSRCPND |= 1 << 28 ;
	rINTPND |= 1 << 28 ;
}







