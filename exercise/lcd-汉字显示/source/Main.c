#include "lcd.h"
#include "uart.h"
unsigned char table[]={0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x41,0x42,0x43,0x44,0x45,0x46} ;

int Main()
{
	unsigned char str[] = "A�л�" ;
	unsigned char *pStr ;
	unsigned int i;
	pStr = str ;
	Uart0_Init(115200) ;
	
		for(i = 0 ; i < 5 ; i++)
		{
			putc(table[*(pStr+i)/16]) ;
			putc(table[*(pStr+i)%16]) ;
			putc(' ') ;
		}
	
	 
	return 0;	
}






/*int Main()
{
	unsigned char *pStr = "�л�" ;
	Lcd_Init() ;
	Lcd_ClearScr(0xFFFF) ;
	while(1)
	{	
		Lcd_Printf(34,130,0,"������ʾʾʵ��\n");
		Lcd_Printf(80,50,0,"������ʾʾʵ��\n");
		
	} 
	return 0;	
}
*/
	

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
/*{
			
			Lcd_printf(34,10,RGB( 0x4F,0x4F,0x4F),RGB( 0x00,0x00,0x00),1,"    ARM�������������ʵս");
			Lcd_printf(34,30,RGB( 0x4F,0x4F,0x4F),RGB( 0x00,0x00,0x00),1,"             ----���ƶ��ǲ���");
			Lcd_printf(34,70,RGB( 0x4F,0x4F,0x4F),RGB( 0x00,0x00,0x00),1,"             ��  ��: ��Сǿ ");
			Lcd_printf(34,90,RGB( 0x4F,0x4F,0x4F),RGB( 0x00,0x00,0x00),1,"             �����ࣺ��Ӣ��  ����");
			
			
			
			
			
			Lcd_printf(34,130,RGB( 0x4F,0x4F,0x4F),RGB( 0x00,0x00,0x00),1,"�ۺ�ʵսƪ֮TFT���Һ��������ʾʵ�� ");
			Lcd_printf(34,150,RGB( 0x4F,0x4F,0x4F),RGB( 0x00,0x00,0x00),1,"         Email��armdesign@163.com");
			Lcd_printf(34,170,RGB( 0x4F,0x4F,0x4F),RGB( 0x00,0x00,0x00),1,"      �����ί�������ϱʻ����� ");
			Lcd_printf(34,190,RGB( 0x4F,0x4F,0x4F),RGB( 0x00,0x00,0x00),1," ��Сǿ ���� ������ ��� ��Ӣ�� ");
			Lcd_printf(34,210,RGB( 0x4F,0x4F,0x4F),RGB( 0x00,0x00,0x00),1,"��С�� ��衻� ���� ������ �̷���");
		
		}*/