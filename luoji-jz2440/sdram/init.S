/*
 * init.S
 *
 *  Created on: 2014-2-21
 *      Author: cyj
 */

.equ WTCON,0x53000000
.equ SDRAMBASE,0x30000000
.equ SDRAMEND,0x34000000
.equ BWSCON,0x48000000
.equ BOOTINTERNAL,0x0

.text
.global _start
_start:
        bl disable_watchdog

        bl init_sdram

        bl copy_to_mem

        ldr pc,=call_main
call_main:
        ldr sp,=SDRAMEND
        bl main
disable_watchdog:
        ldr r0,=WTCON
        mov r1,#0x0
        str r1,[r0]
        mov pc,lr

init_sdram:
            ldr r0,=BWSCON
            adrl r1,mem_cfg_val
            add r3,r0,#52
loop:       ldr r2,[r1],#4
            str r2,[r0],#4
            cmp r0,r3
            bne loop
            mov pc,lr
copy_to_mem:
            ldr r0,=SDRAMBASE
            ldr r1,=BOOTINTERNAL
            add r3,r0,#(4*1024)
loop1:       ldr r2,[r1],#4
            str r2,[r0],#4
            cmp r0,r3
            bne loop1
            mov pc,lr

.align 4
mem_cfg_val:
    .long   0x22011110      @ BWSCON
    .long   0x00000700      @ BANKCON0
    .long   0x00000700      @ BANKCON1
    .long   0x00000700      @ BANKCON2
    .long   0x00000700      @ BANKCON3
    .long   0x00000700      @ BANKCON4
    .long   0x00000700      @ BANKCON5
    .long   0x00018005      @ BANKCON6
    .long   0x00018005      @ BANKCON7
    .long   0x008C07A3      @ REFRESH
    .long   0x000000B1      @ BANKSIZE
    .long   0x00000030      @ MRSRB6
    .long   0x00000030      @ MRSRB7
