//====================================================================
// File Name  : swi.c
// Function	: S3C2440 SWI Test
// Date		: March 20, 2002
// Version	: 0.0
// History
//   0.0 : Programming start (March 12, 2002) -> SOP
//   0.01: Modified for 2440, DonGo
//====================================================================

#include "def.h"
#include "option.h"
#include "2440addr.h"
#include "2440lib.h"
#include "2440slib.h" 
#include "swi.h"
#include "swis.h"

extern  void SWI_ISR(void);


__swi(0x87) void SwiTest(void);

int swiVar;

//----------------------------------------------------------------------
void __irq Isr_SwiTest(void)
{
    rINTMSK = rINTMSK | BIT_UART0;
    ClearPending(BIT_UART0);    
    SwiTest();
    swiVar++;

}

//**********************[Test_SwiIrq ]***********************************
void Test_SwiIrq(void)
{
    int i;
    swiVar = 1;
    
    Uart_Printf("rUCON0 = %x\n",rUCON0);
    Uart_Printf("[ SWI Test ]\n");
    Uart_Printf("swiVar = %d\n",swiVar);
    Uart_TxEmpty(1);
      //UART0 Tx interrupt bit in rINTPND will be set.
    pISR_UART0 = (U32)Isr_SwiTest;
    pISR_SWI   = (U32)SWI_ISR;
    rINTMSK    = rINTMSK & ~(BIT_UART0);
    for(i=0;i<10000;i++);
    rINTMSK = rINTMSK | BIT_UART0;
    Uart_Printf("swiVar = %d\n",swiVar);
}
