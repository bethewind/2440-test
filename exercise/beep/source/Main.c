#include "beep.h"
#include "common.h"
int Main()
{
	Beep_Init() ;
	while(1)
	{	
		Beep_On()  ; Delay() ; 
		Beep_Off() ; Delay() ;	
	} 
	return 0;	
}






