#include "2440addr.h"
#include "adc.h"

#define PRSCEN        1 //����Ԥ��Ƶ
#define PRSCVL        49 //Ԥ��Ƶֵ
#define STDBM         0 //��������ģʽ
#define READ_START    0 //����ʱ������ADת��
#define Adc_Start()   rADCCON |= 1



void ADC_Init(unsigned char channal)//unsigned char channal
{
	
	rADCCON &= (~((1 << 14) | (0xff << 6) | (0x7 << 3) |(1 << 2) | (1 << 1) | (1 << 0))) ;//��һ�������ϣ��������ִ���أ�����������
	rADCCON |= (PRSCEN << 14) | (PRSCVL << 6) |(channal << 3) |(STDBM << 2)|(READ_START << 1) ; 
	
}

int ADC_Read(void)
{
	
	Adc_Start() ;
	
	while(rADCCON & (1 << 0)) ;//ADC������ʼ��λ[0]���Զ�����
	
	while(!(rADCCON & (1 << 15))) ;
	
	return ((int)(rADCDAT0 & 0x3ff)) ;
}