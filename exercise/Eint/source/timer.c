#include "timer.h"
#include "config.h"





void Timer0_Init(void)
{
	rTCFG0 &=~(0xff) ;
	rTCFG0 |= 99 ;
	rTCFG1 &= ~(0xf) ;
	rTCFG1 |= 0X02 ;
	rTCNTB0 = 62500 ;//1s�ж�һ��
	
	rTCON |= (1 << 1) ;//�ֶ�����
	rTCON = 0x09 ; //�Զ����أ�����ֶ�����λ��������ʱ��
	
}