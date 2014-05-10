/***************************************************************/
/*Created by WangXiaoqiang.
/*CopyRight Reserved.
/****************************************************************/

#include "key.h"
#include "ledflow.h"

void Board_Init(void)
{
	Led_Init() ;
	Key_Init() ;
}

int Main()
{
	Board_Init() ;
	while(1)
	{	
		switch(Key_Scan())
		{
			case 1:	Led1_On() ;	break ;
			case 2:	Led2_On() ;	break ;
			case 3:	Led3_On() ;	break ;
			case 4:	Led4_On() ; break ;
			default : break ;
		}
	} 
	return 0;	
}





