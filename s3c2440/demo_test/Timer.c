/******************************************************************************/
// 
// File Name   : Timer.c
// Description : S3C2440 Timer and Watch-dog Function Test 
// Version     : 0.2 
// Date    	   : July 3, 2003
//   0.0 : Programming start (February 27,2002 in 2410) -> SOP
//   0.1 : Modified by Y. H. Lee
//   0.2 : edited by junon
//   
/*******************************************************************************/

/* Include Files */
#include <string.h>
#include "def.h"
#include "option.h"
#include "2440addr.h"
#include "2440lib.h"
#include "timer.h"

/* Structure Define for DMA Special Registers */
typedef struct tagDMA
{
    volatile U32 DISRC;	    //0x0
    volatile U32 DISRCC;    //0x4
    volatile U32 DIDST;	    //0x8
    volatile U32 DIDSTC;    //0xc
    volatile U32 DCON;	    //0x10
    volatile U32 DSTAT;	    //0x14
    volatile U32 DCSRC;	    //0x18
    volatile U32 DCDST;	    //0x1c
    volatile U32 DMASKTRIG; //0x20
}DMA;

static volatile int dmaDone; 
int TestMode; 

volatile int isWdtInt;

//=========================================================================
//
//  Timer input clock frequency = PCLK/{prescaler value+1}/{divider value}
//  {prescaler value} = 0 ~ 255, {divider value} = 2,4,8,16
//
//=========================================================================

/******************** Selection Routine for Timer Test ********************/ 
void * func_timer_test[][2]=
{	
	(void *)Test_TimerNormal, 			"Timer Normal    ",
	(void *)Test_TimerInt, 				"Timer Interrupt ",
	(void *)Test_TimerDma,				"Timer DMA       ",
	(void *)Test_WDT_IntReq, 			"WDT Interrupt   ",
	0,0
};


void Timer_Test(void)
{
	int i;
	
	while(1)
	{
		Uart_Printf("\n\n============== Timer Function Test ==============\n");

		i=0;
		Uart_Printf("\n");
		Uart_Printf("1: Timer Normal  \n");
		Uart_Printf("2: Timer Interrupt \n");
		Uart_Printf("3: Timer DMA      \n");
		Uart_Printf("4: WDT Interrupt  \n");

		Uart_Printf("\n=================================================\n");
	
		Uart_Printf("\nPress Enter key to exit : ");
		i = Uart_GetIntNum();
		if(i==-1) break;		// return.
		switch(i)
		{
		    case 1:	Test_TimerNormal();
			        break;
		    case 2:	Test_TimerInt();
			        break;
		    case 3:	Test_TimerDma();
			        break;
		    case 4:	Test_WDT_IntReq();
			        break;
		    default:
			        break;
		}
//		if(i>=0 && (i<((sizeof(func_timer_test)-1)/8)) )	// select and execute...
//			( (void (*)(void)) (func_timer_test[i][0]) )();
	}
	
}



/******************** PWM Timer Normal Operation Test ********************/
void Test_TimerNormal(void)
{
    int save_B,save_G,save_H,save_PB,save_PG,save_PH,save_MI;
    char key;//, toggle;
    
    /* Save Current Register related to PWM Port */
    //Save Current GPB, G, H Configure Registers 
    save_B  = rGPBCON;
    save_G  = rGPGCON;
    save_H  = rGPHCON;
    //Save Pull-Up Disable Register     
    save_PB = rGPBUP;
    save_PG = rGPGUP;
    save_PH = rGPHUP;
    //Save Miscellaneous Control Register 
    save_MI = rMISCCR;   
    
    /* Setting Port related to PWM */ 
    // PortB
    rGPBUP  = rGPBUP  & ~(0x1f)     | 0x1f; //GPB[4:0] Pull Up
    rGPBCON = rGPBCON & ~(0x3ff)    | 0x2 | 0x2 << 2 | 0x2 << 4 | 0x2 << 6 | 0x2 << 8; //TCKL0, TOUT[3:0]   
    // PortG
    rGPGUP  = rGPGUP  & ~(0x800)    | 0x800; //GPG11 Pull Up
    rGPGCON = rGPGCON & ~(0xc00000) | 0xc00000; //TCLK1 
    // PortH
    rGPHUP  = rGPHUP  & ~(0x200)    | 0x200; //GPH9 Pull Up
    rGPHCON = rGPHCON & ~(0x3<<18)  | (0x2<<18); //CLKOUT0    
    // Miscellaneous Control Register
    rMISCCR = rMISCCR & ~(0xf0)     | 0x40; //Select PCLK with CLKOUT0

    Uart_Printf("[Select Timer Test Setting]\n");
    Uart_Printf("a. Prescaler 1, 0: 0, Dead zone Disable, MUX 4~0: 1/2, (H/L)duty 50 \nb. Prescaler 1, 0: 8, Dead zone Enable, MUX 4~0: 1/16, (H/L)duty 50 \nc. (H/L)duty 0, TCNT =< TCMP, Inverter On\nd. TCLK0, TCLK1 Input Divider Test\n");
    Uart_Printf("Select [a ~ d]: \n");
        
    key = Uart_Getch();
    Uart_Printf("%c\n\n",key);    
	
	/* Timer Configuration Reg.0, 1(TCFG0, TCFG1) Setting */
	switch(key)
    {
        case 'a':
           	
    		rTCFG0 = rTCFG0 & ~(0xffffff) | 0x00000; //Dead zone=0, Prescaler1=0, Prescaler0=0
           	rTCFG1 = 0x0; //All Interrupt, MUX 4 ~ 0: 1/2            
            
            Uart_Printf("Prescaler1=0, Prescaler0=0, Dead Zone Disable, MUX 4 ~ 0: 1/2, Duty 50% \n");
            break;
        
        case 'b':    
            
            rTCFG0 = rTCFG0 & ~(0xffffff) | (0xc8)<<16 | (0x7)<<8 | (0x7); //Dead zone=0, Prescaler1=7, Prescaler0=7
			rTCFG1 = rTCFG1 & ~(0xffffff) | (0x3)<<16 | (0x3)<<12 | (0x3)<<8 | (0x3)<<4 | (0x3); //All Interrupt, MUX 4 ~ 0: 1/16
                        
            Uart_Printf("Prescaler1=8, Prescaler0=8, Dead zone=200 Enable, MUX 4 ~ 0: 1/16, duty = 50%, Consider Dead zone effect\n");        		
            break;
            
        case 'c':    
            
            rTCFG0 = rTCFG0 & 0x0;	//Dead zone=0, Prescaler1=0, Prescaler0=0
			rTCFG1 = rTCFG1 & 0x0;	//All Interrupt, MUX 4 ~ 0: 1/2
               		
            Uart_Printf("(H/L)Duty 0, TCNT =< TCMP, Inverter On\n");
            break;
        
        case 'd':
           	
    		rTCFG0 = rTCFG0 & ~(0xffffff) | 0x00000; //Dead zone=0, Prescaler1=0, Prescaler0=0
           	rTCFG1 = rTCFG1 & ~(0xffffff) | 0x4 | 0x4 << 4 | 0x4 << 8 | 0x4 << 12 | 0x4 << 16;	//All Interrupt, MUX 4 ~ 0: 1/2            
            Uart_Printf("TCLK0, TCLK1 Input Divider Test(if TCLK0, TCLK1=25.4MHZ)\n");
            break;
         
        default:
            rGPBCON = save_B; 
            rGPGCON = save_G;       
            rGPHCON = save_H;           
            rGPBUP  = save_PB;
            rGPGUP  = save_PG;
            rGPHUP  = save_PH;
            rMISCCR = save_MI;
            return;
    }
    
    //(1/(PCLK/(Prescaler+1)/divider) * count(Max 65535) = Timer clock (Frequency)
    //(1/(50.8MHz/1/2))  *  2000 = 0.0787 msec ( 12.7 KHz)
    //(1/(50.8MHz/1/2))  *  4000 = 0.1575 msec ( 6.35 KHz)
    //(1/(50.8MHz/1/2))  *  5000 = 0.1969 msec ( 5.080 KHz)
    //(1/(50.8MHz/1/2))  *  10000 = 0.3937 msec ( 2.54 KHz)
    
    //(1/(50.8MHz/8/16)) *  2000 = 5.039 msec ( 198.45 Hz)
    //(1/(50.8MHz/8/16)) *  4000 = 10.079 msec ( 99.216 KHz)
    //(1/(50.8MHz/8/16)) *  5000 = 12.598 msec ( 79.38 Hz) 
    //(1/(50.8MHz/8/16)) *  10000 = 25.197 msec ( 39.687 Hz)
                          
    rTCNTB0 = 2000;      
    rTCNTB1 = 4000;       
    rTCNTB2 = 5000;       
    rTCNTB3 = 10000;      
                                                
    rTCMPB0 =  2000 - 1000; //(H/L)duty 50%
    rTCMPB1 =  4000 - 2000;
    rTCMPB2 =  5000 - 2500;
    rTCMPB3 = 10000 - 5000;
	
	rTCON  = rTCON & ~(0xffffff) | 0x1<<1 | 0x1<<9 | 0x1<<13 | 0x1<<17 | 0x1<<21 ; //Manual update 
	
	switch(key)
	{
		case 'a':
		
     	rTCON   = rTCON & ~(0xffffff) | 0x599909;  //Auto reload, Inverter off, No operation, Start, Dead zone Disable
		
		break;
		
		case 'b':
	
     	rTCNTB0 = rTCNTB0 & ~(0xffff) | 500; //(1/(50.8MHz/8/16)) *  500 = 1.273 msec ( 793.8 Hz)
     	rTCMPB0 =  500 - 250; //(H/L)duty 50%
     	
     	rTCON = rTCON & ~(0xffffff) | 0x6aaa0a;  //Auto reload, Inverter off, Manual update, Stop, Dead zone Disable
    	rTCON = rTCON & ~(0xffffff) | 0x599909| (0x1) << 4;  //Auto reload, Inverter off, No operation, Start, Dead zone Enable
		
    	break;
    	
    	case 'c':
	
     	rTCNTB0 = rTCNTB0 & ~(0xffff) | 1000; 
     	rTCMPB0 =  1000;      
     	
     	rTCNTB1 = rTCNTB1 & ~(0xffff) | 1000; 
     	rTCMPB1 =  1000; 
     	
     	rTCNTB2 = rTCNTB2 & ~(0xffff) | 1000; 
     	rTCMPB2 =  1000; 
     	
     	rTCNTB3 = rTCNTB3 & ~(0xffff) | 1000; 
     	rTCMPB3 =  1000; 
     		
     	rTCON = rTCON & ~(0xffffff) | 0x1 << 1 | 0x1 << 9 | 0x1 << 13 | 0x1 << 17 | 0x1 << 21;  //Auto reload disable, Inverter off, Manual update, Stop, Dead zone Disable
		rTCON = rTCON & ~(0xffffff) | 0x1 | 0x1 << 2 | 0x1 <<8 | 0x1 << 10| 0x1 << 12 | 0x1 << 14 | 0x1 << 16 | 0x1 << 18| 0x1 << 20;  //Auto reload enable, Inverter On, No Operation, Start, Dead zone Disable
		
		Uart_Printf("rTCNTB0 = Dec:%d, rTCNTO0 = Dec:%d, rTCMPB0 = Dec:%d\n",rTCNTB0, rTCNTO0, rTCMPB0);
    	break;
    
    	case 'd':
    
		rTCON   = rTCON & ~(0xffffff) | 0x599909;  //Auto reload, Inverter off, No operation, Start, Dead zone Disable
		break;
    
     	default:
        break;
    }
          
    if(key=='a' && (PCLK==(203200000/4)))  
    {
        
      	Uart_Printf("PCLK 50.8MHz, Timer TOUT0 : 0.0787 msec ( 12.70 KHz)\n");
      	Uart_Printf("PCLK 50.8MHz, Timer TOUT1 : 0.1575 msec (  6.35 KHz)\n");
        Uart_Printf("PCLK 50.8MHz, Timer TOUT2 : 0.1969 msec (  5.08 KHz)\n");
        Uart_Printf("PCLK 50.8MHz, Timer TOUT3 : 0.3937 msec (  2.54 KHz)\n");        
    }    
      
    else if(key=='b' && (PCLK==(203200000/4)))  
    {
        Uart_Printf("PCLK 50.8MHz, Timer TOUT0 :   1.273 msec (793.8 KHz)\n");
        Uart_Printf("PCLK 50.8MHz, Timer TOUT1 : /TOUT0 \n");
        Uart_Printf("PCLK 50.8MHz, Timer TOUT2 : 13.6094 msec ( 73.478 Hz)\n");
        Uart_Printf("PCLK 50.8MHz, Timer TOUT3 : 27.2189 msec ( 36.739 Hz)\n");                        
    }    
    
    else if(key=='d' && (PCLK==(203200000/4)))	 
    {
        Uart_Printf("then Timer TOUT 0 : 0.0787 msec ( 12.70 KHz)\n");
        Uart_Printf("then Timer TOUT 1 : 0.1575 msec (  6.35 KHz)\n");
        Uart_Printf("then Timer TOUT 2 : 0.1969 msec (  5.08 KHz)\n");
        Uart_Printf("then Timer TOUT 3 : 0.3937 msec (  2.54 KHz)\n");                        
    }
    
    Uart_Printf("\nProbe PCLK.\n");
    Uart_Printf("Probe TOUT0.\n");
    Uart_Printf("Probe TOUT1.\n");
    Uart_Printf("Probe TOUT2.\n");
    Uart_Printf("Probe TOUT3.\n");
    
    Uart_Printf("\nCheck PWM (Pulse Width Modulation) Output\n");
    Uart_Printf("Press any key.\n");
    Uart_Getch();   
    
    /* Stop Timer0, 1, 2, 3, 4 */
    rTCON   = 0x0;	//One-shot, Inverter off, No operation, Dead zone disable, Stop Timer 
	
	rGPBCON = save_B; 
    rGPGCON = save_G;       
    rGPHCON = save_H;    
    rGPBUP  = save_PB;
    rGPGUP  = save_PG;
    rGPHUP  = save_PH;
    rMISCCR = save_MI;

}

/******************** Timer Interrupt 0/1/2/3/4 test ********************/										
volatile int variable0,variable1,variable2,variable3,variable4;
int j;

void Test_TimerInt(void)
{
    variable0 = 0;variable1 = 0;variable2 = 0;variable3 = 0;variable4 = 0;
		
	/* Timer0,1,2,3,4 Interrupt service is available */ 
    rINTMSK = ~(BIT_TIMER4 | BIT_TIMER3 | BIT_TIMER2 | BIT_TIMER1 | BIT_TIMER0);
	// Uart_Printf("rINTMSK (After)  = 0x%8x   <= Timer4,3,2,1 Bit[14:10]\n",rINTMSK);

    /* Timer0,1,2,3,4 Interrupt Service Routine Entry Point Determine */ 
    pISR_TIMER0 = (int)Timer0Done;
    pISR_TIMER1 = (int)Timer1Done;
    pISR_TIMER2 = (int)Timer2Done;
    pISR_TIMER3 = (int)Timer3Done;
    pISR_TIMER4 = (int)Timer4Done;

    Uart_Printf("\n[Timer 0,1,2,3,4 Interrupt Test]\n\n");

    rTCFG0 = rTCFG0 & ~(0xffffff) | 0xff | 0xff<<8; //Dead zone=0,Prescaler1=255(0x0f),Prescaler0=255(0x0f)
    rTCFG1  =rTCFG1 & ~(0xffffff) | 0x001233; //All interrupt,Mux4=1/2,Mux3=1/4,Mux2=1/8,Mux1=1/16,Mux0=1/16
	
    rTCNTB0 = 30000;         
    rTCNTB1 = 30000;           
    rTCNTB2 = 30000;           
    rTCNTB3 = 30000;        
    rTCNTB4 = 30000;         
    
    rTCMPB0 = 15000;
    rTCMPB1 = 15000;
    rTCMPB2 = 15000;
    rTCMPB3 = 15000;
    
    rTCON  = rTCON & ~(0xffffff) | 0x1<<1 | 0x1<<9 | 0x1<<13 | 0x1<<17 | 0x1<<21 ; //Manual update 	
    
   
    rTCON  = rTCON & ~(0xffffff) | 0x1 | 0x1<<3 ; //Timer 0 Start, Auto-reload 
    rTCON  = rTCON | 0x1<<8 | 0x1<<11 ; //Timer 1 Start, Auto-reload
    rTCON  = rTCON | 0x1<<12 | 0x1<<15 ; //Timer 2 Start, Auto-reload
    rTCON  = rTCON | 0x1<<16 | 0x1<<19 ; //Timer 3 Start, Auto-reload
    rTCON  = rTCON | 0x1<<20 | 0x1<<22 ; //Timer 4 Start, Auto-reload   
    
    //Auto reload,Inverter off,No operation,Dead zone disable,Start
	
    while(1)
    {
     if(variable4 == 8)
     break;
    }
      
    //Delay(1); //To compensate timer error(<1 tick period)
    
    rTCON = 0x0; //Stop Timers
   
    if(variable4==8 && variable3==4 && variable2==2 && variable1==1 && variable0==1)
    {
        Uart_Printf("Timer 0,1,2,3,4 Auto-reload Interrupt Test => OK!\n");
    }        
    else
    {
        Uart_Printf("Timer 0,1,2,3,4 Auto-reload Interrupt Test => Fail............\n");
    }    
    Uart_Printf("Timer0: %d (=1)\nTimer1: %d (=1)\nTimer2: %d (=2)\nTimer3: %d (=4)\nTimer4: %d (=8)\n",
        variable0,variable1,variable2,variable3,variable4);        
	
	/* OneShot Test */
  	variable0 = 0;variable1 = 0;variable2 = 0;variable3 = 0;variable4 = 0;
  	
  	rTCON   = rTCON & ~(0xffffff) | 0x6aaa0a;         
  	//Auto reload, Inverter off, Manual update, Dead zone disable, Stop  
    rTCON   = rTCON & ~(0xffffff) | 0x1 |(0x1)<<8| (0x1)<<12 | (0x1)<<16 | (0x1)<<20 ; 
    //T0,1,2,3,4= One-shot,Inverter off,No operation,Dead zone disable,Start

	while(variable0 == 0);     
    
    Delay(1); //To compensate timer error(<1 tick period)
    rTCON = 0x0; //One-shot, Inverter off, No operation, Dead zone disable, Stop
   
    if(variable4==1 && variable3==1 && variable2==1 && variable1==1 && variable0==1)
    {
        Uart_Printf("Timer 0,1,2,3,4 One-shot Interrupt Test => OK!\n");
    }        
    else
    {
        Uart_Printf("Timer 0,1,2,3,4 One-shot Interrupt Test => Fail............\n");
    }    
    	Uart_Printf("Timer0: %d (=1)\nTimer1: %d (=1)\nTimer2: %d (=1)\nTimer3: %d (=1)\nTimer4: %d (=1)\n",
        variable0,variable1,variable2,variable3,variable4);        

	/* Timer0,1,2,3,4 Interrupt Service is masked */
    rINTMSK |= (BIT_TIMER4 | BIT_TIMER3 | BIT_TIMER2 | BIT_TIMER1 | BIT_TIMER0); 
	Uart_Printf("Press any key\n");      
	while(!Uart_Getch());       //Key board press check      

}

/****** Timer Interrupt Request ******/ 
void __irq Timer0Done(void)
{
    rSRCPND = BIT_TIMER0;       //Clear pending bit
    rINTPND = BIT_TIMER0;
    rINTPND;                    //Prevent an double interrupt pending
    variable0++;    
}

void __irq Timer1Done(void)
{
    rSRCPND = BIT_TIMER1;       //Clear pending bit
    rINTPND = BIT_TIMER1;
    rINTPND;
    variable1++;    
}

void __irq Timer2Done(void)
{
    rSRCPND = BIT_TIMER2;       //Clear pending bit
    rINTPND = BIT_TIMER2;
    rINTPND;
    variable2++;    
}

void __irq Timer3Done(void)
{
    rSRCPND = BIT_TIMER3;       //Clear pending bit
    rINTPND = BIT_TIMER3;
    rINTPND;
    variable3++;    
}

void __irq Timer4Done(void)
{
    rSRCPND = BIT_TIMER4;       //Clear pending bit
    rINTPND = BIT_TIMER4;
    rINTPND;
    variable4++;    
}

/******************** Timer DMA Request Test ********************/										
void Test_TimerDma(void)
{
    int i;
    
    rTCFG0 = rTCFG0 & ~(0xffffff) | 0x0; //Dead zone=0, Prescaler0,1 = 0    
    rTCFG1 = rTCFG1 & ~(0xffffff) | 0x3 << 0 | 0x1 << 20; //Mux0 1/16, DMA Request Channel 0
    
    rTCNTB0 = 65500;   
  	rTCMPB0 = 30000;
  	
  	rTCNTB1 = 65500;   
  	rTCMPB1 = 30000;
  	
  	rTCNTB2 = 65500;   
  	rTCMPB2 = 30000;
  	
  	rTCNTB3 = 65500;   
  	rTCMPB3 = 30000;
  	
  	rTCNTB4 = 65500;   
  	
  	rTCON  = rTCON & ~(0xffffff) | 0x1<<1 | 0x1<<9 | 0x1<<13 | 0x1<<17 | 0x1<<21 ; //Manual update 
  	
  	Uart_Printf("\nTimer 0 DMA Request Test\n\n");
  	
  	rTCON = 0x1 | 0x1<<3; //Auto reload, Start
   	for(i=0 ; i < 4 ; i++)
   	DMA_M2M(0,_NONCACHE_STARTADDRESS,_NONCACHE_STARTADDRESS+0x800000,0x80000,0,0); //byte,single
	rTCON  = 0x0; //Timer Stop

	Uart_Printf("\nTimer 1 DMA Request Test\n\n");
  	
  	rTCFG1 = rTCFG1 & ~(0xffffff) | 0x3 << 4 | 0x2 << 20; //Mux0 1/16, DMA Request Channel 2
  	rTCON = 0x1<<8 | 0x1<<11; //Auto reload, Start
  	for(i=0 ; i < 4 ; i++) 
   	DMA_M2M(2,_NONCACHE_STARTADDRESS,_NONCACHE_STARTADDRESS+0x800000,0x80000,0,0); //byte,single
	rTCON  = 0x0; 
	
	Uart_Printf("\nTimer 2 DMA Request Test\n\n");
  	
  	rTCFG1 = rTCFG1 & ~(0xffffff) | 0x3 << 8 | 0x3 << 20; //Mux0 1/16, DMA Request Channel 3
  	rTCON = 0x1<<12 | 0x1<<15; //Auto reload, Start
  	for(i=0 ; i < 4 ; i++) 
   	DMA_M2M(3,_NONCACHE_STARTADDRESS,_NONCACHE_STARTADDRESS+0x800000,0x80000,0,0); //byte,single
	rTCON  = 0x0; 

	Uart_Printf("\nTimer 3 DMA Request Test\n\n");
  	
  	rTCFG1 = rTCFG1 & ~(0xffffff) | 0x3 << 12 | 0x4 << 20; //Mux0 1/16, DMA Request Channel 0
  	rTCON = 0x1<<16 | 0x1<<19; //Auto reload, Start
  	for(i=0 ; i < 4 ; i++) 
   	DMA_M2M(0,_NONCACHE_STARTADDRESS,_NONCACHE_STARTADDRESS+0x800000,0x80000,0,0); //byte,single
	rTCON  = 0x0; 

	Uart_Printf("\nTimer 4 DMA Request Test\n\n");
  	
  	rTCFG1 = rTCFG1 & ~(0xffffff) | 0x3 << 16 | 0x5 << 20; //Mux0 1/16, DMA Request Channel 2
  	rTCON = 0x1<<20 | 0x1<<22; //Auto reload, Start
  	for(i=0 ; i < 4 ; i++) 
   	DMA_M2M(2,_NONCACHE_STARTADDRESS,_NONCACHE_STARTADDRESS+0x800000,0x80000,0,0); //byte,single
	rTCON  = 0x0;
	
	Uart_Printf("Press any key\n");      
	while(!Uart_Getch());       //Key board press check      

}

/***** Memory to Memory DMA Service Routine *****/
void DMA_M2M(int ch,int srcAddr,int dstAddr,int tc,int dsz,int burst)
{
    int i,time;
    volatile U32 memSum0=0,memSum1=0;
    DMA *pDMA; //Pointer for DMA Special Register Structure 
    int length;
    
    length=tc*(burst ? 4:1)*((dsz==0)+(dsz==1)*2+(dsz==2)*4); //total data size(byte address)
    //     Counter value * burst size * data size     
    
    Uart_Printf("[DMA%d MEM2MEM Test]\n",ch);

    switch(ch)
    {
    case 0:
    	pISR_DMA0=(int)Dma0Done; //DMA0 interrupt Vector Table Initialzation
    	rINTMSK&=~(BIT_DMA0);    //DMA0 interrupt Enable
    	pDMA=(void *)0x4b000000; //pDMA ponter indicate first addr. of DMA0 Special Register 
    	break;
    case 1:
    	pISR_DMA1=(int)Dma1Done; //DMA1 interrupt Vector Table Initialzation
    	rINTMSK&=~(BIT_DMA1);  
    	pDMA=(void *)0x4b000040;
    	break;
    case 2:
		pISR_DMA2=(int)Dma2Done; //DMA2 interrupt Vector Table Initialzation
    	rINTMSK&=~(BIT_DMA2);  
    	pDMA=(void *)0x4b000080;
	break;
    case 3:
        pISR_DMA3=(int)Dma3Done; //DMA3 interrupt Vector Table Initialzation
       	rINTMSK&=~(BIT_DMA3);  
       	pDMA=(void *)0x4b0000c0;
        break;
    }
                                                                                                                            
    Uart_Printf("DMA%d %8xh->%8xh,size=%xh(tc=%xh),dsz=%d,burst=%d\n",ch,srcAddr,dstAddr,length,tc,dsz,burst);

    Uart_Printf("Initialize the src.\n");
    
    for(i=srcAddr;i<(srcAddr+length);i+=4)
    {
    	*((U32 *)i)=i^0x55aa5aa5;
    	memSum0+=i^0x55aa5aa5;
    }

    
    Uart_Printf("DMA%d start\n",ch);
    
    dmaDone=0;
    
    /* Value Setting of Current DMA Special Registers */
    pDMA->DISRC=srcAddr;
    pDMA->DISRCC=(0<<1)|(0<<0); //inc,AHB
    pDMA->DIDST=dstAddr;
    pDMA->DIDSTC=(0<<1)|(0<<0); //inc,AHB
    
    pDMA->DCON=tc|((unsigned int)1<<31)|(1<<30)|(1<<29)|(burst<<28)|(1<<27)|(0x3<<24)|(1<<23)|(1<<22)|(dsz<<20)|(tc);
    //HS,AHB,TC interrupt,whole, HWSource request mode,relaod off
    
    pDMA->DMASKTRIG= 0x1<<1; //DMA on,
	
	/* DMA Start */
    Timer_Start(3);//128us resolution	      
    
    while(dmaDone==0);
    
    time=Timer_Stop();
    
    Uart_Printf("DMA transfer done. time=%f, %fMB/S\n",(float)time/ONESEC3, length/((float)time/ONESEC3)/1000000.);
    
    rINTMSK=BIT_ALLMSK;
    
    /* Check DMA Result */
    for(i=dstAddr;i<dstAddr+length;i+=4)
    {
    	memSum1+=*((U32 *)i)=i^0x55aa5aa5;
    }
    
    Uart_Printf("memSum0=%x,memSum1=%x\n",memSum0,memSum1);
    if(memSum0==memSum1)
    	Uart_Printf("DMA test result--------------------------------------O.K.\n");
    else 
    	Uart_Printf("DMA test result--------------------------------------ERROR!!!\n");
}

/***** DMA Interrupt Service Routine *****/ 
static void __irq Dma0Done(void)
{
    ClearPending(BIT_DMA0);
    dmaDone=1;
}

static void __irq Dma1Done(void)
{
    ClearPending(BIT_DMA1);
    dmaDone=1;
}

static void __irq Dma2Done(void)
{
    ClearPending(BIT_DMA2);
    dmaDone=1;
}

static void __irq Dma3Done(void)
{
    ClearPending(BIT_DMA3);
    dmaDone=1;
}


void Test_WDT_IntReq(void)
{
	
	/* WDT ISR addr Initialization */ 
	pISR_WDT_AC97 = (unsigned)Wdt_Int;
	isWdtInt = 0;

	//f_watchdog = PCLK / (Prescaler value + 1 ) / Division_factor
	Uart_Printf("[WatchDog Timer Interrupt Request Test]\n");

	/* WDT INTMSK Enable */
	ClearPending(BIT_WDT_AC97);	
	rINTMSK&=~(BIT_WDT_AC97); 		//Watch dog Interrupt service is available
	rSUBSRCPND=(BIT_SUB_WDT);
	rINTSUBMSK&=~(BIT_SUB_WDT);


	/* WDT Interrupt Test */

	// 1. Clock division_factor 128
	Uart_Printf("\nClock Division Factor: 128(dec), Prescaler: 7812(dec)\n");
	rWTCON   = ((PCLK/1000000-1)<<8) | (3<<3) | (1<<2);	//Clock division 128, Interrupt enable

	rWTDAT   = 7812 ;          
	rWTCNT   = 7812 ;
	rWTCON   = rWTCON | (1<<5); 	//Watch-dog timer enable
	while(isWdtInt != 10);

	// 2. Clock division_factor 64
	isWdtInt = 0;
	Uart_Printf("\nClock Division Factor: 64(dec), Prescaler: 7812(dec)\n");
	rWTCON   = ((PCLK/1000000-1)<<8) | (2<<3) | (1<<2);	//Clock division 64, Interrupt enable

	rWTDAT   = 7812 ;          
	rWTCNT   = 7812 ;
	rWTCON   = rWTCON | (1<<5); 	//Watch-dog timer enable
	while(isWdtInt != 10);

	// 3. Clock division_factor 32
	isWdtInt = 0;
	Uart_Printf("\nClock Division Factor: 32(dec), Prescaler: 7812(dec)\n");
	rWTCON   = ((PCLK/1000000-1)<<8) | (1<<3) | (1<<2);	//Clock division 32, Interrupt enable

	rWTDAT   = 7812 ;          
	rWTCNT   = 7812 ;
	rWTCON   = rWTCON | (1<<5); 	//Watch-dog timer enable
	while(isWdtInt != 10);

	// 4. Clock division_factor 16
	isWdtInt = 0;
	Uart_Printf("\nClock Division Factor: 16(dec), Prescaler: 7812(dec)\n");
	rWTCON   = ((PCLK/1000000-1)<<8) | (0<<3) | (1<<2);	//Clock division 16, Interrupt enable

	rWTDAT   = 7812 ;          
	rWTCNT   = 7812 ;
	rWTCON   = rWTCON | (1<<5); 	//Watch-dog timer enable
	while(isWdtInt != 10);

	// 5. Prescaler: 128
	isWdtInt = 0;
	Uart_Printf("\n\nPrescaler: 128(dec), Clock Division Factor: 128(dec)\n");
	rWTCON   = (128<<8) | (3<<3) | (1<<2);	//Clock division 128, Interrupt enable

	rWTDAT   = 3077 ;          
	rWTCNT   = 3077 ;
	rWTCON   = rWTCON | (1<<5); 	//Watch-dog timer enable
	while(isWdtInt != 10);

	// 6. Prescaler: 64
	isWdtInt = 0;
	Uart_Printf("\nPrescaler: 64(dec), Clock Division Factor: 128(dec)\n");
	rWTCON   = (64<<8) | (3<<3) | (1<<2);	//Clock division 64, Interrupt enable

	rWTDAT   = 3077 ;          
	rWTCNT   = 3077 ;
	rWTCON   = rWTCON | (1<<5); 	//Watch-dog timer enable
	while(isWdtInt != 10);

	// 7. Prescaler: 32
	isWdtInt = 0;
	Uart_Printf("\nPrescaler: 32(dec), Clock Division Factor: 128(dec)\n");
	rWTCON   = (32<<8) | (3<<3) | (1<<2);	//Clock division 32, Interrupt enable

	rWTDAT   = 3077 ;          
	rWTCNT   = 3077 ;
	rWTCON   = rWTCON | (1<<5); 	//Watch-dog timer enable
	while(isWdtInt != 10);

	// 8. Prescaler: 16
	isWdtInt = 0;
	Uart_Printf("\nPrescaler: 16(dec), Clock Division Factor: 128(dec)\n");
	rWTCON   = (16<<8) | (3<<3) | (1<<2);	//Clock division 16, Interrupt enable

	rWTDAT   = 3077;          
	rWTCNT   = 3077 ;
	rWTCON   = rWTCON | (1<<5); 	//Watch-dog timer enable
	while(isWdtInt != 10);


	/* WDT Reset Test */
	rWTCON = (128<<8) | (3<<3) | (1); 	//Prescaler=0x31(49),Clock division 128,Reset enable

	Uart_Printf("\nI will restart after 2 sec.\n");
	rWTCNT = 3077;          

	rWTCON = rWTCON | (1<<5); 	//Watch-dog timer Interrupt enable

	while(1);

//	rINTSUBMSK|=(BIT_SUB_WDT);

}


/***** WDT Interrupt Service Rotine *****/
void __irq Wdt_Int(void)
{
	rSUBSRCPND=(BIT_SUB_WDT);		// Clear Sub int pending
	ClearPending(BIT_WDT_AC97);		// Clear master pending

	 Uart_Printf("%d ",++isWdtInt);
}


