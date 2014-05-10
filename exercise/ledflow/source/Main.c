#include "ledflow.h"
#include "common.h"
int Main()
{
	Led_Init() ;
	while(1)
	{	
		Led1_On() ;Delay() ;Led1_Off() ;
		Led2_On() ;Delay() ;Led2_Off() ;
		Led3_On() ;Delay() ;Led3_Off() ;
		Led4_On() ;Delay() ;Led4_Off() ;
		
	} 
	return 0;	
}






