#include "lcd.h"
#include "pic.h"



int Main()
{
	Lcd_Init() ; ;
	while(1)
	{	
		
		Paint_Bmp(pic) ;
	} 
	return 0;	
}



/*int Main()
{
	//int i = 0 ;
	IO_Init() ;
	while(1)
	{	
		//Uart_SendByte('a') ;
		//Uart_SendString("wangxiao\n") ;
			//Uart_Printf("The value of i is:%d\n",i) ;
		//for(i = 0 ; i < 12 ; i++)
		{
			
			Paint_Bmp(0,0,320,240,Picture0) ;
			Delay1s() ;
			Paint_Bmp(0,0,320,240,Picture1) ;
			Delay1s() ;
			Paint_Bmp(0,0,320,240,Picture2) ;
			Delay1s() ;
			Paint_Bmp(0,0,320,240,Picture9) ;
			Delay1s() ;
			Paint_Bmp(0,0,320,240,Picture6) ;
			Delay1s() ;
			Paint_Bmp(0,0,320,240,Picture5) ;
			Delay1s() ;
			Paint_Bmp(0,0,320,240,Picture4) ;
			Delay1s() ;
			Paint_Bmp(0,0,320,240,Picture12) ;
			Delay1s() ;
			Paint_Bmp(0,0,320,240,Picture8) ;
			Delay1s() ;
			Paint_Bmp(0,0,320,240,Picture3) ;
			Delay1s() ;
			Paint_Bmp(0,0,320,240,Picture10) ;
			Delay1s() ;
			Paint_Bmp(0,0,320,240,Picture11) ;
			Delay1s() ;
			Paint_Bmp(0,0,320,240,Picture7) ;
			Delay1s() ;
			
			
			
		}
		
		
	} 
	return 0;	
}*/
