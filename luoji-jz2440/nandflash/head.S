/*
 * init.S
 *
 *  Created on: 2014-2-21
 *      Author: cyj
 */


.text
.global _start
_start:
        ldr sp,=4*1024

        bl disable_watchdog
        bl init_sdram
        bl init_clock
        bl init_uart

        bl init_nand

        bl nand_read2

        ldr sp,=0x34000000
        ldr pc,=main
main_loop:
            b main_loop
