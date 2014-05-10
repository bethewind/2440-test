/*
 * led.S
 *
 *  Created on: 2014-2-20
 *      Author: cyj
 */


.text
.equ WTCON,0x53000000
.equ GPFCON,0x56000050
.equ GPFDAT,0x56000054
.global _start
_start:
		ldr r0,=WTCON
		mov r1,#0x0
		str r1,[r0]
		ldr r0,=GPFCON
		mov r1,#0x0100
		str r1,[r0]
		ldr r0,=GPFDAT
		mov r1,#0x0
		str r1,[r0]
main_loop:
		b main_loop
