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
	rUCON0 = 05 ;      			
	rUBRDIV0  = PCLK / bandrate / 16 - 1 ;
	rURXH0 = 0;
}



