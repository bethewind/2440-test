/*
 * main.c
 *
 *  Created on: 2014-2-21
 *      Author: cyj
 */
#define GPFCON (*(volatile unsigned long*)0x56000050)
#define GPFDAT (*(volatile unsigned long*)0x56000054)

#define GPF4_MASK (3 << (2 * 4))
#define GPF0_MASK (3 << (2 * 0))

#define GPF4_CON (1 << (2 * 4)) // out
#define GPF0_CON (0 << (2 * 0)) // in

int main() {
	//把某位赋0,先把那位清0,再赋值。。赋1直接或。
    GPFCON &= ~(GPF4_MASK | GPF0_MASK); //清0
    GPFCON |= (GPF4_CON | GPF0_CON); //赋值


    while (1) {
    	unsigned long gpfDat = GPFDAT;
        if (gpfDat & 0x00000001) { // 1, 断开  判断某位是否为1
            GPFDAT |= (1 << 4);
        } else {
            GPFDAT &= ~(1 << 4);
        }
    }
    return 0;
}
