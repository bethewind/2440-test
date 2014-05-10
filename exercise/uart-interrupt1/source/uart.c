#include "config.h"
#include "uart.h"


#define TXD0READY (1 << 2)
#define RXD0READY (1 << 0)
#define PCLK 50000000//ʱ��Դ��ΪPCLK
	           

void Uart0_Init(unsigned int bandrate)
{
	rGPHCON &= ~((3 << 4) | (3 << 6)) ;//GPH2-GPH3��RX/TX
	rGPHCON |= ((2 << 4) | (2 << 6)) ;//GPH2--TXD[0];GPH3--RXD[0]

	rGPHUP = 0x00 ;
	
	rULCON0 |= 0x03 ;    			//8λ���ݣ�1λֹͣλ
	rUCON0 = 05 ;      			
	rUBRDIV0  = PCLK / bandrate / 16 - 1 ;
	rURXH0 = 0;
}



