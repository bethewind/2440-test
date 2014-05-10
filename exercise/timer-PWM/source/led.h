#ifndef __LED_H__
#define __LED_H__

#include "2440addr.h"

#define Led1_On()      {rGPBDAT &= (~(1 << 5));} 
#define Led1_Off()     {rGPBDAT |= (1 << 5);} 

extern void Led_Init(void);

#endif



//extern void Led1_On(void);
//extern void Led1_Off(void);


