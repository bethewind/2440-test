/****************************************************************
 NAME: retarget.c
 DESC: console I/O redirection, heap base, stack base. 
 HISTORY:
 Jan.01.2002:purnnamu: first draft. rewrite 'retarget.c' from ARM.
 Jul.30.2002:purnnamu: first draft for S3C2420X. 
 Nov.29.2003:junon: first draft for S3C2440. 
 ****************************************************************/

#include <stdio.h>

#include "option.h"
#include "2440lib.h"
#if ADS10==TRUE
#include <rt_misc.h>
#endif

#if USE_MAIN

#if SEMIHOSTING
    #ifdef __thumb
    /* Thumb Semihosting SWI */
    #define SemiSWI 0xAB
    #else
    /* ARM Semihosting SWI */
    #define SemiSWI 0x123456
    #endif

/* Exit */
__swi(SemiSWI) void _Exit(unsigned op, unsigned except);
#define Exit() _Exit (0x18,0x20026)

#endif //SEMIHOSTING

#if !SEMIHOSTING
struct __FILE { int handle;   /* Add whatever you need here */};
FILE __stdout;
FILE __stdin;
//NOTE.
// 1. If __FILE is redefined by user, the fgetc(),fputc(),ferror(),defined by user,are used.
// 2. __stdout,__stdin should be redefined .
#endif //!SEMIHOSTING


#if !SEMIHOSTING
// If the semihosting is not used, 
// redefine fputc(),fgetc(),ferror() for printf/scanf through UART
int fputc(int ch, FILE *f)
{
    /* Place your implementation of fputc here */
    /* e.g. write a character to a UART, or to the debugger console with SWI WriteC */
    Uart_SendByte(ch);
    return ch;
}

int ferror(FILE *f)
{   /* Your implementation of ferror */
    return EOF;
}

int fgetc(FILE *f)
{
    char ch;
    ch=Uart_Getch();
    
    Uart_SendByte(ch);
    if(ch=='\b')Uart_SendString(" \b");
    
    return ch;
}


void _ttywrch(int ch)
{
    Uart_SendByte(ch);
}
#endif //!SEMIHOSTING

void _sys_exit(int return_code)
{
#if !SEMIHOSTING		
    while(1);
#else    	
    Exit();         /* for debugging */
label:  goto label; /* endless loop */
#endif
}


extern unsigned int Image$$ZI$$Limit;

__value_in_regs struct __initial_stackheap __user_initial_stackheap(
        unsigned R0, unsigned SP, unsigned R2, unsigned SL)
{
    struct __initial_stackheap config;
    
    config.heap_base = (unsigned int)&Image$$ZI$$Limit;
    config.stack_base = SP; //SP has the current mode sp register value.

    return config;
}


#endif //USE_MAIN
/*
To place heap_base directly above the ZI area, use e.g:
    extern unsigned int Image$$ZI$$Limit;
    config.heap_base = (unsigned int)&Image$$ZI$$Limit;
(or &Image$$region_name$$ZI$$Limit for scatterloaded images)

To specify the limits for the heap & stack, use e.g:
    config.heap_limit = SL;
    config.stack_limit = SL;
*/

