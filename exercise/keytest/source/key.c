/***************************************************************/
/*Created by WangXiaoqiang.
/*CopyRight Reserved.
/****************************************************************/


#include "key.h"
#include "2440addr.h"

void Key_Init(void)
{
	rGPFCON &= ~((3 << 0) | (3 << 2) | (3 << 4) | (3 << 8)) ;
	rGPFCON |= KEY1 | KEY2 | KEY3 | KEY4 ;
	rGPFDAT |= (1 << 0) | (1 << 1) | (1 << 2) | (1 << 4) ;//将键盘对应的四个引脚置为高电平，完成初始化工作	
}


int Key_Scan(void)
{
	int keynum = 0 ;
	//rGPFDAT = 0XFFF ;
	if((rGPFDAT & (1 << 1)) == 0 )
	{
		keynum = 1 ;
	}
	if((rGPFDAT & (1 << 4)) == 0 )
	{
		keynum = 2 ;
	}	 
	if((rGPFDAT & (1 << 2)) == 0 ) 
	{
		keynum = 3 ;
	}	
	if((rGPFDAT & (1 << 0)) == 0 )
	{
		keynum = 4 ;
	}	 
	return keynum ;	
}
