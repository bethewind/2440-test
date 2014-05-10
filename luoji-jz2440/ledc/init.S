/*
 * init.S
 *
 *  Created on: 2014-2-21
 *      Author: cyj
 */


.text
.equ WTCON,0x53000000
.global _start
_start:
        ldr r0,=WTCON
        mov r1,#0x0
        str r1,[r0]

        ldr sp,=4*1024

        bl main
main_loop:
            b main_loop
