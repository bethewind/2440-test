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

void Uart_SendByte(int data);

//====================================================================
void Uart_SendString(char *pt);

void Uart_Printf(char *fmt,...);


#endif /* A2440LIB_H_ */
