//=============================================================================
// File Name : Timer.h
// Function  : S3C2440 Watch-Dog and Timer Test Head File
// Program   : Shin, On Pil (SOP)
// Date      : March 21, 2002
// Version   : 0.0
// History
//   0.0 : Programming start (March 21,2002) -> SOP
//   0.1 : Modified by Yoh-Han Lee
//=============================================================================

#ifndef __TIMER_H__

#define __TIMER_H__

/* Timer Interrupt Service Routines */
void __irq Timer0Done(void); //Timer0
void __irq Timer1Done(void); //Timer1
void __irq Timer2Done(void); //Timer2
void __irq Timer3Done(void); //Timer3
void __irq Timer4Done(void); //Timer4
/* DMA Interrupt Service Routine */ 
static void __irq Dma0Done(void); //DMA0
static void __irq Dma1Done(void); //DMA1
static void __irq Dma2Done(void); //DMA2
static void __irq Dma3Done(void); //DMA3 

void Timer_Test(void);
void Test_TimerNormal(void); 
void Test_TimerInt(void);
void Test_TimerDma(void);
static void DMA_M2M(int ch,int srcAddr,int dstAddr,int tc,int dsz,int burst);

void __irq Wdt_Int(void);
void Test_WDT_IntReq(void);

#endif  //__TIMER_H__
