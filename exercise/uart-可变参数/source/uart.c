#include "config.h"
#include "uart.h"
#include <stdarg.h>

#define PCLK 50000000//ʱ��Դ��ΪPCLK
	           

void Uart0_Init(unsigned int bandrate)
{
	rGPHCON &= ~((3 << 4) | (3 << 6)) ;//GPH2-GPH3��RX/TX
	rGPHCON |= ((2 << 4) | (2 << 6)) ;//GPH2--TXD[0];GPH3--RXD[0]

	rGPHUP = 0x00 ;
	
	rULCON0 |= 0x03 ;    			//8λ���ݣ�1λֹͣλ
	rUCON0 = 0x05 ;      			
	rUBRDIV0  = PCLK / bandrate / 16 - 1 ;
	rURXH0 = 0;
}


static void Uart0_SendByte(int data)
{
    if(data == '\n')
    {
        while(!(rUTRSTAT0 & (1 << 2)));//�ȴ��������
      rUTXH0 = '\r';
    }
    while(!(rUTRSTAT0 & (1 << 2)));   
     rUTXH0 = data  ;    
} 

static void Uart0_SendString(char *pt)
{
    while(*pt)
        Uart0_SendByte(*pt++);
}

void Uart0_Printf(const char *fmt,...)
{
    va_list ap;
    char string[50];

    va_start(ap,fmt);
    vsprintf(string,fmt,ap);
    va_end(ap);
    Uart0_SendString(string);
}


