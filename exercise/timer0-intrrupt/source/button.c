#include "button.h"
#include "config.h"

#define KEY1 (2 << 2) 
#define KEY2 (2 << 8)
#define KEY3 (2 << 4)
#define KEY4 (2 << 0)

void Key_Init(void)
{
	rGPFCON &= ~((3 << 0) | (3 << 2) | (3 << 4) | (3 << 8)) ;
	rGPFCON |= KEY1 | KEY2 | KEY3 | KEY4 ;
	rGPFDAT |= (1 << 0) | (1 << 1) | (1 << 2 ) | (1 << 4) ;//将键盘对应的四个引脚置为高电平，完成初始化工作	
}


int Key_Scan() //该函数在本次试验中没有用到
{
	int keynum = 0 ;
	if((rGPFDAT & (0 << 1)) == 0 )
	{
		keynum = 1 ;
	}
	if((rGPFDAT & (0 << 4)) == 0 )
	{
		keynum = 2 ;
	}	 
	if((rGPFDAT & (0 << 2)) == 0 ) 
	{
		keynum = 3 ;
	}	
	if((rGPFDAT & (1 << 0)) == 0 )
	{
		keynum = 4 ;
	}	 
	return keynum ;	
}
