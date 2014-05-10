/*
 * init.S
 *
 *  Created on: 2014-2-21
 *      Author: cyj
 */


.text
.global _start
_start:
RESET:
      b Reset_handler
@ 0x04: 未定义指令中止模式的向量地址
HandleUndef:
    b   HandleUndef

@ 0x08: 管理模式的向量地址，通过SWI指令进入此模式
HandleSWI:
    b   HandleSWI

@ 0x0c: 指令预取终止导致的异常的向量地址
HandlePrefetchAbort:
    b   HandlePrefetchAbort

@ 0x10: 数据访问终止导致的异常的向量地址
HandleDataAbort:
    b   HandleDataAbort

@ 0x14: 保留
HandleNotUsed:
    b   HandleNotUsed

@ 0x18: 中断模式的向量地址
    b   HandleIRQ

@ 0x1c: 快中断模式的向量地址
HandleFIQ:
    b   HandleFIQ

Reset_handler:
        ldr sp,=4*1024
        @bl disable_watchdog
        @bl init_sdram
        @bl init_clock
        @bl init_uart

        msr cpsr_c,#0xd2 @进IRQ模式
        ldr sp,=3*1024

        msr cpsr_c,#0xd3 @回管理模式

        msr cpsr_c,#0x53 @开IRQ

        ldr pc,=main
main_loop:
            b main_loop
HandleIRQ:
        sub lr,lr,#4
        stmdb sp!,{r0-r12,lr}
        ldr lr, =int_return             @ 设置调用ISR即EINT_Handle函数后的返回地址
        ldr pc, =EINT_Handle            @ 调用中断服务函数，在interrupt.c中
int_return:
        ldmia   sp!,    { r0-r12,pc }^  @ 中断返回, ^表示将spsr的值复制到cpsr


