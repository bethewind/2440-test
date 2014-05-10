#include "timer.h"
#include"isrservice.h"
#include "uart.h"
#include "lcd.h"
#include "pic.h"
#include "pic1.h"
#include "pic2.h"
#include "pic3.h"
#include "pic4.h"
#include "led.h"
void IO_Init(void) ;

unsigned int flag = 0 ;

int Main(void)
{
	IO_Init() ;
	while(1)
	{
		switch(flag)
		{
			case 1:
				Paint_Bmp(pic) ;
				break ;
			case 2:
				Paint_Bmp(pic1) ;
				break ;
			case 3:
				Paint_Bmp(pic2) ;
				break ;
			case 4:
				Paint_Bmp(pic3) ;
				break ;	
			case 5:
				Paint_Bmp(pic4) ;
				break ;
			default:
			 
			   break ;	
		}
	} 
	
	return 0;	
}

void IO_Init(void)
{
    Timer0_Init() ;
    Timer0_Interrupt_Init() ;
	Isr_Init() ;
	Lcd_Init() ;
}

