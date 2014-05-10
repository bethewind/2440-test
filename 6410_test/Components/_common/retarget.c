/**************************************************************************************
* 
*	Project Name : S3C6410 Validation
*
*	Copyright 2006 by Samsung Electronics, Inc.
*	All rights reserved.
*
*	Project Description :
*		This software is only for validating functions of the S3C6410.
*		Anybody can use this software without our permission.
*  
*--------------------------------------------------------------------------------------
* 
*	File Name : retarget.c
*  
*	File Description : This file retargets the standard I/O functions.
*
*	Author : Haksoo,Kim
*	Dept. : AP Development Team
*	Created Date : 2006/11/08
*	Version : 0.1 
* 
*	History
*	- Created(Haksoo,Kim 2006/11/08)
*  
**************************************************************************************/

/*
** Copyright (C) ARM Limited, 2004. All rights reserved.
*/

/*
** This file contains re-implementations of functions whose
** C library implementations rely on semihosting.
** I/O is targeted to the Versatile serial port A.  
*/

#include <stdio.h>

#include "option.h"
#include "library.h"
#include "sfr6410.h"
#include "system.h"
#include "uart.h"


#if !SEMIHOSTING

/*
** Importing __use_no_semihosting_swi ensures that our image doesn't link
** with any C Library code that makes direct use of semihosting.
**
*/
//+daedoo
//#if SEMIHOSTING
#pragma import(__use_no_semihosting_swi)
//#endif

/*
** Retargeted I/O
** ==============
** The following C library functions make use of semihosting
** to read or write characters to the debugger console: fputc(),
** fgetc(), and _ttywrch().  They must be retargeted to write to
** the Versatile AP UART.  __backspace() must also be retargeted
** with this layer to enable scanf().  See the Compiler and
** Libraries Guide.
*/

/*
** These must be defined to avoid linking in stdio.o from the
** C Library
*/
//+daedoo
//#if SEMIHOSTING
struct __FILE { int handle;   /* Add whatever you need here */};
FILE __stdout;
FILE __stdin;
//#endif

/*
** __backspace must return the last char read to the stream
** fgetc() needs to keep a record of whether __backspace was
** called directly before it
*/
int last_char_read;
int backspace_called;

int fputc(int ch, FILE *f)
{
    UART_Putc(ch);

    return ch;
}

int fgetc(FILE *f)
{
    int ch;
    
    ch=UART_Getc();
	UART_Putc(ch);

	switch(ch){
	case 0xd  :	ch=0xa; 
				break;
	case '\b' : UART_Putc(' ');
				UART_Putc('\b');
				break;
	}
    
    return ch;
}

char *gets( char *s)
{
    int c;
	int i=0;
    
    while((c = UART_Getc())!=0xa) {
        if(c=='\b') {
        	if (i>0) i--;
        } else {
        	s[i++]=(char)c;
        }
    }
    s[i]='\0';
	return s;
}

void _ttywrch(int ch)
{
    UART_Putc(ch);
}

/*
** The effect of __backspace() should be to return the last character
** read from the stream, such that a subsequent fgetc() will
** return the same character again.
*/

int __backspace(FILE *f)
{
    backspace_called = TRUE;
    return 1;
}

/* END of Retargeted I/O */

/*
** Exception Signaling and Handling
** ================================
** The C library implementations of ferror() uses semihosting directly
** and must therefore be retargeted.  This is a minimal reimplementation.
** _sys_exit() is called after the user's main() function has exited.  The C library
** implementation uses semihosting to report to the debugger that the application has
** finished executing.
*/

int ferror(FILE *f)
{
    return EOF;
}

void _sys_exit(int return_code)
{
    while(1);
}

#endif

