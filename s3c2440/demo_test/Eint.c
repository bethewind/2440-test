/*****************************************
  NAME: eint.c
  DESC: External interrupt test
  HISTORY:
  2001.04.03:purnnamu: draft ver 0.0
  2003.05.xx:DonGo: modifed for 2440 ver 0.1
 *****************************************/

#include <string.h>
#include "def.h"
#include "option.h"
#include "2440addr.h"
#include "2440lib.h"
#include "2440slib.h" 

#include "eint.h"

static void __irq Eint0Int(void)
{
    ClearPending(BIT_EINT0);
    Uart_Printf("EINT0 interrupt is occurred.\n");
}

static void __irq Eint2Int(void)
{
    ClearPending(BIT_EINT2);
    Uart_Printf("EINT2 interrupt is occurred.\n");
}

static void __irq Eint11_19(void)
{
    if(rEINTPEND==(1<<11))
    {
	Uart_Printf("EINT11 interrupt is occurred.\n");
	rEINTPEND=(1<<11);
    }
    else if(rEINTPEND==(1<<19))
    {
	Uart_Printf("EINT19 interrupt is occurred.\n");
	rEINTPEND=(1<<19);
    }
    else
    {
	Uart_Printf("rEINTPEND=0x%x\n",rEINTPEND);
	rEINTPEND=(1<<19)|(1<<11);
    }
    ClearPending(BIT_EINT8_23);
}

// SMDK2440 EINT S/W
// EINT0/2/11/19
void Test_Eint(void)
{
//    int i;
    int extintMode;

    Uart_Printf("[External Interrupt Test through PF0/2/11/19]\n");

    Uart_Printf("1.L-LEVEL  2.H-LEVEL  3.F-EDGE  4.R-EDGE  5.B-EDGE\n");
    Uart_Printf("Select the external interrupt type.\n");
    extintMode=Uart_Getch();

    //extintMode='3';
    rGPFCON = (rGPFCON & 0xffcc)|(1<<5)|(1<<1);		//PF0/2 = EINT0/2
    rGPGCON = (rGPGCON & 0xff3fff3f)|(1<<23)|(1<<7);	//PG3/11 = EINT11/19
     
    switch(extintMode)
    {
    case '1':
    	rEXTINT0 = (rEXTINT0 & ~((7<<8)  | (0x7<<0))) | 0x0<<8 | 0x0<<0; //EINT0/2=low level triggered
	rEXTINT1 = (rEXTINT1 & ~(7<<12)) | 0x0<<12; //EINT11=low level triggered
	rEXTINT2 = (rEXTINT2 & ~(7<<12)) | 0x0<<12; //EINT19=low level triggered
    	break;
    case '2':
    	rEXTINT0 = (rEXTINT0 & ~((7<<8)  | (0x7<<0))) | 0x1<<8 | 0x1<<0; //EINT0/2=high level triggered
	rEXTINT1 = (rEXTINT1 & ~(7<<12)) | 0x1<<12; //EINT11=high level triggered
	rEXTINT2 = (rEXTINT2 & ~(7<<12)) | 0x1<<12; //EINT19=high level triggered
    	break;
    case '3':
    	rEXTINT0 = (rEXTINT0 & ~((7<<8)  | (0x7<<0))) | 0x2<<8 | 0x2<<0; //EINT0/2=falling edge triggered
	rEXTINT1 = (rEXTINT1 & ~(7<<12)) | 0x2<<12; //EINT11=falling edge triggered
	rEXTINT2 = (rEXTINT2 & ~(7<<12)) | 0x2<<12; //EINT19=falling edge triggered
    	break;
    case '4':
    	rEXTINT0 = (rEXTINT0 & ~((7<<8)  | (0x7<<0))) | 0x4<<8 | 0x4<<0; //EINT0/2=rising edge triggered
	rEXTINT1 = (rEXTINT1 & ~(7<<12)) | 0x4<<12; //EINT11=rising edge triggered
	rEXTINT2 = (rEXTINT2 & ~(7<<12)) | 0x4<<12; //EINT19=rising edge triggered
     	break;
    case '5':
    	rEXTINT0 = (rEXTINT0 & ~((7<<8)  | (0x7<<0))) | 0x6<<8 | 0x6<<0; //EINT0/2=both edge triggered
	rEXTINT1 = (rEXTINT1 & ~(7<<12)) | 0x6<<12; //EINT11=both edge triggered
	rEXTINT2 = (rEXTINT2 & ~(7<<12)) | 0x6<<12; //EINT19=both edge triggered
    	break;
    default:
    	break;
    }

    Uart_Printf("Press the EINT0/2/11/19 buttons or Press any key to exit.\n");

    pISR_EINT0=(U32)Eint0Int;
    pISR_EINT2=(U32)Eint2Int;
    pISR_EINT8_23=(U32)Eint11_19;

    rEINTPEND = 0xffffff;
    rSRCPND = BIT_EINT0|BIT_EINT2|BIT_EINT8_23; //to clear the previous pending states
    rINTPND = BIT_EINT0|BIT_EINT2|BIT_EINT8_23;
    
    rEINTMASK=~( (1<<11)|(1<<19) );
    rINTMSK=~(BIT_EINT0|BIT_EINT2|BIT_EINT8_23);
		
    Uart_Getch();
    
    rEINTMASK=0xffffff;
    rINTMSK=BIT_ALLMSK;
}

