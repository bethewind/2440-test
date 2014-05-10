@******************************************************************************
@ File: head.S
@ 功能: 设置SDRAM，将程序复制到SDRAM，然后跳到SDRAM继续执行
@******************************************************************************       

.text
.global _start
_start:
    ldr sp, =4096           @ 设置栈指针，以下都是C函数，调用前需要设好栈
    bl  disable_watch_dog   @ 关闭WATCHDOG，否则CPU会不断重启
    bl  clock_init          @ 设置MPLL，改变FCLK、HCLK、PCLK
    bl  memsetup            @ 设置存储控制器以使用SDRAM
    bl  init_nand           @ 初始化NAND Flash
    
                            @ 复制代码到SDRAM中
    bl nand_read2
    
    bl  clean_bss           @ 清除bss段，未初始化或初值为0的全局/静态变量保存在bss段

    ldr sp,=0x34000000
    ldr pc, =main           @ 调用main函数
halt_loop:
    b   halt_loop
    
