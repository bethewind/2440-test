/*
 * S2440lib.c
 *
 *  Created on: 2014-3-12
 *      Author: cyj
 */

#include "addr2440.h"
#include "2440lib.h"
typedef   char  * va_list;

#define   _INTSIZEOF(n)   ((sizeof(n) + sizeof(int) - 1) & ~(sizeof(int) - 1))

#define   va_start(ap,v)   (ap = (va_list)&v + _INTSIZEOF(v))

#define   va_arg(ap,t)     (*(t*)((ap += _INTSIZEOF(t)) -  _INTSIZEOF(t)))

#define   va_end(ap)      (ap = (va_list)0)

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


void Uart_Printf(char *fmt,...)
{
    char *p;
    va_list ap;

    //获得第一个实际参数的起始地址
    va_start(ap,fmt);

    //分析fmt指向的字符串
    for(p = fmt; *p;p ++)
    {
        if(*p == '%')
        {
            p ++;
            switch(*p)
            {
            //整形十进制数
            case 'd':
                Uart_SendByte(va_arg(ap,int));
                break;

            //字符
            case 'c':
                //变参传递char类型变量时，编译器在
                //编译的时候将其提升为int类型
                Uart_SendByte(va_arg(ap,int));
                break;

            //字符串
            case 's':
                //地址占用4个字节
                Uart_SendString((char *)va_arg(ap,int));
                break;
            }

        }else{
            Uart_SendByte(*p);
        }
    }

    //将ap赋值为NULL
    va_end(ap);
}
