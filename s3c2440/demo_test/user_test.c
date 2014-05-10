#include <stdio.h>
#include <string.h>
#include "def.h"
#include "option.h"
#include "2440addr.h"
#include "2440lib.h"

#include "User_Test.h"

#include "User_Test1.h"
#include "User_Test2.h"
#include "memtest.h"
#include "userlib.h"

void SwReset(void);

void * func_user_test[][2]=
{	
	//                          "0123456789012345"
	(void *)User_Test1,			"user test 1(dvs)",	// max 15자 로한정하여 comment하세요.
	(void *)User_Test2,			"user test 2	 ",
	(void *)Mem_Test,			"Memory test	 ",	
	(void *)Firm_DVS_Onoff,		"Firm_DVS_Onoff	 ",	
	(void *)DVS_Onoff_Man,		"DVS_Onoff(manual) ", 	
	(void *)SwReset,		"rst                    ", 	

	0,0
};


extern void Calc_Clock(int pring_msg);
extern U32 Mdiv, Pdiv, Sdiv, Fclk, Hclk, Pclk, Hdivn, Pdivn, Hclk_Ratio, Pclk_Ratio, Ref_Cnt;

void SwReset(void)
{
rWTDAT=0x1000;
rWTCNT=0x1000;
rWTCON= (0x80<<8) | (1<<5) | (1<<0);
while(1);
}
void User_Test(void)
{
	int i;

	while(1)
	{
		i=0;
		Uart_Printf("\n\n");

		Uart_Printf("01:user test 1(dvs)\n");
		Uart_Printf("02:user test 2	 \n");
		Uart_Printf("03:Memory test	\n");
		Uart_Printf("04:Firm_DVS_Onoff	\n");
		Uart_Printf("05:DVS_Onoff(manual)\n");
		Uart_Printf("06:rst \n");
		Uart_Printf("\nSelect (\"-1\" to exit) : ");
		i = Uart_GetIntNum();
		if(i==-1) break;		// return.
		switch(i)
		{
			case 1:	  User_Test1();
			          break;
			case 2:	  User_Test2();
			          break;
			case 3:	  Mem_Test(1);
			          break;
			case 4:	  Firm_DVS_Onoff();
			          break;
			case 5:	  DVS_Onoff_Man();
			          break;
			case 6:   SwReset();
			          break;
		    default: 
			          break;

		}
//		if(i>=0 && (i<((sizeof(func_user_test)-1)/8)) )	// select and execute...
//			( (void (*)(void)) (func_user_test[i][0]) )();
	}
	
	Uart_Printf("\nUser_Test program end.\n");
}




