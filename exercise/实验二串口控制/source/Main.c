#include "led.h"
#include "uart.h"
#include "lcd.h"

void IO_Init(void) ;
extern const unsigned char __CHS[] ;
int Main(void)
{
	
	
	IO_Init() ;
	
	while(1)
	{
		switch(getc())
		{
			case '0' :
				Led1_On() ;
				break ;
			case '1' :
				Led1_Off() ;
				break ;
			case '2' :
				Lcd_Printf(10,40,254,"控制命令接收成功！") ;
				break ;
			case '3' :
				Lcd_PowerEnable(0) ;
				break ;	
			default :
			    break ;
		}
	  
		
		
	} 
	
	return 0;	
}

void IO_Init(void)
{
    Uart0_Init(115200) ;
	Led_Init() ;
	Lcd_Init() ;
	
}

