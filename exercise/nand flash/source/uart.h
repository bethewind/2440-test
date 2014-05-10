#ifndef __UART_H__
#define __UART_H__


extern void Uart0_Init(unsigned int bandrate) ;
extern void putc(unsigned char c) ;
extern unsigned char getc(void) ;

#endif