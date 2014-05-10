/**************************************************************
function:led test
 **************************************************************/

#include "ledflow.h"

void IO_Init() ;
extern void C_Swi_Handler(unsigned num) ;

__swi(0x01) void led1(void);
__swi(0x02) void led2(void);
__swi(0x03) void led3(void);
__swi(0x04) void led4(void);

int Main()
{
	IO_Init() ;
	while(1)
	{	
		led1() ;
		led2() ;
		led3() ;
		led4() ;
	} 
	return 0;	
}

void IO_Init()
{
	
	Led_Init() ;
	
	
}

void  C_Swi_Handler(unsigned num)
{
	switch(num)
	{
		case  0x01:
			 Led1_On() ;  break ;
		case  0x02:
			 Led2_On() ;  break ;
		case  0x03:
			 Led3_On() ;  break ;
		case  0x04:
			 Led4_On() ;  break ;
		default	 : 
			 break ;
	}
}