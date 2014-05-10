/*
 * 2440lib.h
 *
 *  Created on: 2014-3-12
 *      Author: cyj
 */

#ifndef A2440LIB_H_
#define A2440LIB_H_

#define TXD0READY   (1<<2)
#define RXD0READY   (1)

#include "lib/printf.h"

void Uart_SendByte(int data);

//====================================================================
void Uart_SendString(char *pt);

void putc(unsigned char c);

unsigned char getc(void);


#endif /* A2440LIB_H_ */
