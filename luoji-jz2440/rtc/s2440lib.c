/*
 * S2440lib.c
 *
 *  Created on: 2014-3-12
 *      Author: cyj
 */

#include "addr2440.h"
#include "2440lib.h"
typedef   char  * va_list;

void Uart_SendByte(int data)
{
        if(data=='\n')
        {
            while(!(UTRSTAT0 & 0x2));
           // Delay(1);                 //because the slow response of hyper_terminal
            UTXH0 = (unsigned char)('\r');
        }
        while(!(UTRSTAT0 & 0x2));   //Wait until THR is empty.
      //  Delay(1);
        UTXH0 = (unsigned char)(data);

}

//====================================================================
void Uart_SendString(char *pt)
{
    while(*pt)
        Uart_SendByte(*pt++);
}

void putc(unsigned char c)
{

    while (!(UTRSTAT0 & TXD0READY));

    UTXH0 = c;
}


unsigned char getc(void)
{

    while (!(UTRSTAT0 & RXD0READY));

    return URXH0;
}
