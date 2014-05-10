#ifndef __ISRSERVICE_h__
#define __ISRSERVICE_h__

void Isr_Init(void) ;
void __irq Eint0_Isr(void) ;
void __irq Eint1_Isr(void) ;
void __irq Eint2_Isr(void) ;
void __irq Eint4_7_Isr(void) ;


#endif