/*****************************************
  NAME: Adcts.c
  DESC: ADC & Touch screen test
  HISTORY:
  2003.09.23:Leon YH KIM: draft ver 1.0
 *****************************************/
#include <string.h>
#include "def.h"
#include "2440addr.h"
#include "2440lib.h"


#define REQCNT 30
#define ADCPRS 9	//YH 0627
#define LOOP 1

int ReadAdc(int ch);    //Return type is int, Declare Prototype function
void __irq AdcTsAuto(void);

int count=0;
volatile int xdata, ydata;

void Test_Adc(void)
{
    int a0=0,a1=0,a2=0,a3=0; //Initialize variables
    int a4=0,a5=0,a6=0,a7=0; //Initialize variables

    Uart_Printf("The ADC_IN are adjusted to the following values.\n");        
    Uart_Printf("Push any key to exit!!!\n");    
    Uart_Printf("ADC conv. freq.=%d(Hz)\n",(int)(PCLK/(ADCPRS+1.))); // ADC Freq. = PCLK/(ADCPSR+1), ADC conversion time = 5CYCLES*(1/(ADC Freq.))
    
    while(Uart_GetKey()==0)
    {

    a0=ReadAdc(0);
    a1=ReadAdc(1);
    a2=ReadAdc(2);
    a3=ReadAdc(3);
    a4=ReadAdc(4);    
    a5=ReadAdc(5);
    a6=ReadAdc(6);
    a7=ReadAdc(7);    


    Uart_Printf("AIN0: %04d AIN1: %04d AIN2: %04d AIN3: %04d", a0,a1,a2,a3);
    Uart_Printf("AIN4: %04d AIN5: %04d AIN6: %04d AIN7: %04d \n", a4,a5,a6,a7);
   }
    
    rADCCON=(0<<14)+(19<<6)+(7<<3)+(1<<2);     //stand by mode to reduce power consumption 
    Uart_Printf("rADCCON = 0x%x\n", rADCCON);
}

        
int ReadAdc(int ch)
{
    int i;
    static int prevCh=-1;

    if(prevCh!=ch)
        {
        rADCCON=(1<<14)+(ADCPRS<<6)+(ch<<3);   //setup channel, ADCPRS
        for(i=0;i<LOOP;i++);    //delay to set up the next channel
        prevCh=ch;
        }
    rADCCON=(1<<14)+(ADCPRS<<6)+(ch<<3);   //setup channel, ADCPRS
    rADCTSC = rADCTSC & 0xfb;     //Normal ADC conversion & No TS operation
    rADCCON|=0x1;   //start ADC

    while(rADCCON & 0x1);          //check if Enable_start is low
    while(!(rADCCON & 0x8000));        //check if EC(End of Conversion) flag is high
    return (rADCDAT0&0x3ff);
}

    
void Test_AdcTs(void)
{
   
    rADCDLY=50000;                  //Normal conversion mode delay about (1/3.6864M)*50000=13.56ms
    rADCCON=(1<<14)+(ADCPRS<<6);   //ADCPRS En, ADCPRS Value

    Uart_Printf("[ADC touch screen test.]\n");

    rADCTSC=0xd3;  //Wfait,XP_PU,XP_Dis,XM_Dis,YP_Dis,YM_En

    pISR_ADC = (int)AdcTsAuto;
	rINTMSK=~BIT_ADC;       //ADC Touch Screen Mask bit clear
	rINTSUBMSK=~(BIT_SUB_TC);

	Uart_Printf("\nType any key to exit!!!\n");
	Uart_Printf("\nStylus Down, please...... \n");
	Uart_Getch();

	rINTSUBMSK|=BIT_SUB_TC;
	rINTMSK|=BIT_ADC;
	Uart_Printf("[Touch Screen Test.]\n");

}


void __irq AdcTsAuto(void)
{
//	int i;
	U32 saveAdcdly;

    if(rADCDAT0&0x8000)
    {
	Uart_Printf("\nStylus Up!!\n");
	rADCTSC&=0xff;	// Set stylus down interrupt bit
    }
    else 
	Uart_Printf("\nStylus Down!!\n");

	rADCTSC=(1<<3)|(1<<2);         //Pull-up disable, Seq. X,Y postion measure.
	saveAdcdly=rADCDLY;
	rADCDLY=40000;                 //Normal conversion mode delay about (1/50M)*40000=0.8ms

	rADCCON|=0x1;                   //start ADC

		while(rADCCON & 0x1);		//check if Enable_start is low
		while(!(rADCCON & 0x8000));        //check if EC(End of Conversion) flag is high, This line is necessary~!!
		
            while(!(rSRCPND & (BIT_ADC)));  //check if ADC is finished with interrupt bit

            xdata=(rADCDAT0&0x3ff);
            ydata=(rADCDAT1&0x3ff);

	 //YH 0627, To check Stylus Up Interrupt.
	 rSUBSRCPND|=BIT_SUB_TC;
	 ClearPending(BIT_ADC);
	 rINTSUBMSK=~(BIT_SUB_TC);
	 rINTMSK=~(BIT_ADC);
			 
	 rADCTSC =0xd3;    //Waiting for interrupt
	 rADCTSC=rADCTSC|(1<<8); // Detect stylus up interrupt signal.

			while(1)		//to check Pen-up state
			{
			 if(rSUBSRCPND & (BIT_SUB_TC))	//check if ADC is finished with interrupt bit
				 {
					Uart_Printf("Stylus Up Interrupt~!\n");
					break;	//if Stylus is up(1) state
				}
			}	

    Uart_Printf("count=%d XP=%04d, YP=%04d\n", count++, xdata, ydata);    //X-position Conversion data            

	rADCDLY=saveAdcdly; 
	rADCTSC=rADCTSC&~(1<<8); // Detect stylus Down interrupt signal.
    rSUBSRCPND|=BIT_SUB_TC;
    rINTSUBMSK=~(BIT_SUB_TC);	// Unmask sub interrupt (TC)     
    ClearPending(BIT_ADC);
}
