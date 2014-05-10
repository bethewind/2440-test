#include "led.h"
#include "timer.h"
int Main()
{
	int flag = 0 ;
	Led_Init() ;
	Timer0_Init() ;
	
	while(1)
	{	
		if(rSRCPND & (1 << 10))
		{
			flag = !flag ;
			rSRCPND |= (1 << 10) ;
		}
		if (1 == flag)
		{
			Led1_On() ;
			
		}
		else
		{
			Led1_Off() ;
		}
				
	} 
	return 0;	
}






