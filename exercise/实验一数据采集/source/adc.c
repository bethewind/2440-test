#include "2440addr.h"
#include "adc.h"

#define PRSCEN        1 //允许预分频
#define PRSCVL        49 //预分频值
#define STDBM         0 //正常工作模式
#define READ_START    0 //读数时不进行AD转换
#define Adc_Start()   rADCCON |= 1



void ADC_Init(unsigned char channal)//unsigned char channal
{
	
	rADCCON &= (~((1 << 14) | (0xff << 6) | (0x7 << 3) |(1 << 2) | (1 << 1) | (1 << 0))) ;//这一句必须加上，否则程序不执行呢？？？？？？
	rADCCON |= (PRSCEN << 14) | (PRSCVL << 6) |(channal << 3) |(STDBM << 2)|(READ_START << 1) ; 
	
}

int ADC_Read(void)
{
	
	Adc_Start() ;
	
	while(rADCCON & (1 << 0)) ;//ADC真正开始后，位[0]会自动清零
	
	while(!(rADCCON & (1 << 15))) ;
	
	return ((int)(rADCDAT0 & 0x3ff)) ;
}