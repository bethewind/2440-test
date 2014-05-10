#include "config.h"
#include "uart.h"
#include "nand.h"


unsigned char table[]={0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x41,0x42,0x43,0x44,0x45,0x46} ;
void IO_Init() ;

int Main()
{
	unsigned char recv ;
	
	IO_Init() ;
	
	NF_EraseBlock(17) ;
	NF_RamdomWrite(17,6,6,0xA0) ;
	recv = NF_RamdomRead(17,6,6) ;
	while(1)
	{
			putc(table[recv/16]) ;
			putc(table[recv%16]) ;
			putc('  ') ;
				
	}
	return 1 ;
}
void IO_Init()
{
	Uart0_Init(115200) ;
	NF_Init() ;

}



/*int Main()
{
	unsigned char flag,recv,buf[2048],sbuf[2048] ;
	unsigned int i=1 ;//µÚ¼¸Ò³
	IO_Init() ;
	for(i = 0 ; i < 2048 ; i++)
	{
		buf[i] = i ;
	}
	
	//NF_EraseBlock(17) ;
	
	/*if(NF_EraseBlock(7))
	{
		Led1_On() ;
	}
	NF_WritePage(17,4,buf) ;
	//flag = NF_RamdomWrite(17,4,6,0x45) ;
	//recv = NF_RamdomRead(17,4,6) ;
	//NF_ReadPage_ECC(17,4, sbuf);
	NF_ReadPage(17,4,sbuf) ;
	while(1)
	{
		
		for(i = 0 ; i < 2048 ; i++)
		{
			putc(table[sbuf[i]/16]) ;
			putc(table[sbuf[i]%16]) ;
			putc('  ') ;
		}*/
		
		/*if(flag == 0x66)
		{	
			for(i = 0 ; i < 6 ; i ++)
			{
				putc(table[sbuf[i]/16]) ;
				putc(table[sbuf[i]%16]) ;
			} 
		}*/
		/*putc((unsigned char)table[recv/16]) ;
		putc((unsigned char)table[recv%16]) ;
		
	}
	return 1 ;
}*/
