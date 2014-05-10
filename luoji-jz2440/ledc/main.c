/*
 * main.c
 *
 *  Created on: 2014-2-21
 *      Author: cyj
 */
#define GPFCON (*(volatile unsigned long*)0x56000050)
#define GPFDAT (*(volatile unsigned long*)0x56000054)


int main() {
    GPFCON = (1 << (2 * 4));
    GPFDAT = 0x0;
    return 0;
}
