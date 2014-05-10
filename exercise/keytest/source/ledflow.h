#ifndef __LEDFLOW_H__
#define __LEDFLOW_H__

#include "2440addr.h"

#define Led1_On()      {rGPBDAT &= (~(1 << 5));} 
#define Led1_Off()     {rGPBDAT |= (1 << 5);} 
#define Led2_On()      {rGPBDAT &= (~(1 << 6));} 
#define Led2_Off()     {rGPBDAT |= (1 << 6);} 
#define Led3_On()      {rGPBDAT &= (~(1 << 7));} 
#define Led3_Off()     {rGPBDAT |= (1 << 7);} 
#define Led4_On()      {rGPBDAT &= (~(1 << 8));} 
#define Led4_Off()     {rGPBDAT |= (1 << 8);} 



extern void Led_Init(void);

#endif



//extern void Led1_On(void);
//extern void Led1_Off(void);


