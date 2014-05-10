#include "config.h"
#include "uart.h"


#define TXD0READY (1 << 2)
#define RXD0READY (1 << 0)
#define PCLK 50000000//时钟源设为PCLK
	           

void Uart0_Init(unsigned int bandrate)
{
	rGPHCON &= ~((3 << 4) | (3 << 6)) ;//GPH2-GPH3是RX/TX
	rGPHCON |= ((2 << 4) | (2 << 6)) ;//GPH2--TXD[0];GPH3--RXD[0]

	rGPHUP = 0x00 ;
	
	rULCON0 |= 0x03 ;    			//8位数据，1位停止位
	rUCON0 = 0x05 ;      			
	rUBRDIV0  = PCLK / bandrate / 16 - 1 ;
	rURXH0 = 0;
}

void putc(unsigned char c)
{
	
	rUTXH0 = c ;
	while(!(rUTRSTAT0 & TXD0READY)) ;//等待上个字符发送完毕
}
unsigned char getc(void)
{
	unsigned char c ;
	while(!(rUTRSTAT0 & RXD0READY)) ;
	c = rURXH0 ;
	return c ;
	
}

void print(unsigned char *p,unsigned int n) //打印字符串的函数
{
	unsigned int  i = 0 ;
	
	for(i = 0 ; i < n ; i++)
	{
		putc(*p++) ;
		
	}
}




