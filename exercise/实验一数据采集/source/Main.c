

//unsigned int flag = 0 ;
/***********************************************************************
* 数据格式：
*   0      1      2     3    4     5  6  7 
*  千位  百位  十位  个位  空格  O   K  !
*
*
***********************************************************************/
#include "adc.h"
#include "uart.h"
#include "nand.h"
#include "string.h"
void IO_Init(void) ;


int Main(void)
{
	struct ADCVALUE
	{
		char head[14] ;
		unsigned char adcValue[4] ;
		char confirm[14] ;
	} valFormt[64] ;
	
	int i,j,value ;
	struct ADCVALUE recBuf[64] ;
	
	IO_Init() ;
	
	while(1)
	{	
		for(i = 0 ; i < 64 ; i++ )//共采集64次就可以
		{
			strcpy(valFormt[i].head,"The Value :   ") ;
			value = ADC_Read();
			value = 10 * value *3.3 / 1023 ;
			valFormt[i].adcValue[0] = (value/10 + '0') ;
			valFormt[i].adcValue[1] =  '.' ;
			valFormt[i].adcValue[2] = value%10 + '0' ;
			valFormt[i].adcValue[3] = 'V' ;
			strcpy(valFormt[i].confirm,"    test over! ") ;
			
		}
		NF_EraseBlock(18) ;
		NF_WritePage(18,5, (unsigned char *)valFormt) ;
		NF_ReadPage(18,5, (unsigned char *)recBuf) ;
		for(i = 0 ; i < 64 ; i++)
		{
			for(j = 0 ; j < 14 ; j++ )
			{
				putc(recBuf[i].head[j]) ;
			}
			for(j = 0 ; j < 4 ; j++ )
			{
				putc(recBuf[i].adcValue[j]) ;
			}
			
			for(j = 0 ; j < 14 ; j++ )
			{
				putc(recBuf[i].confirm[j]) ;
			}
			putc('\r') ;
			
	  }
		
		
	} 
	
	return 0;	
}

void IO_Init(void)
{
    Uart0_Init(115200) ;
	ADC_Init(2) ;
	NF_Init() ;
	
}

