#include "2440addr.h"

void Irq_Init(void)
{
	rINTMSK   &= ~((1 << 0) | (1 << 1) | (1 << 2) | (1 << 4)) ;//ʹ���ⲿ�ж�0��1 ��2 ,4
	rEINTMASK &= (~(1 << 4)) ;//ʹ���ⲿ�ж�4	
}


