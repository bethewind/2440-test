/*
 * init.c
 *
 *  Created on: 2014-3-5
 *      Author: cyj
 */
#include "addr2440.h"

void EINT_Handle() {
    unsigned long oft = INTOFFSET;
    unsigned long val = GPFDAT;

    switch( oft )
    {
        // S2被按下
        case 0:
        {
            GPFDAT |= (0x7<<4);   // 所有LED熄灭
            if (val &(1<<4))
            	GPFDAT &= ~(1<<4);      // LED1点亮
            else
            	GPFDAT |= (1<<4);
            break;
        }

        default:
            break;
    }

    //清中断
    SRCPND = 1<<oft;
    INTPND = 1<<oft;
}
