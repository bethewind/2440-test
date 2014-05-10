#ifndef __BEEP_H__
#define __BEEP_H__

#include "2440addr.h"

#define Beep_On()   {rGPBDAT |= (1 << 0);}
#define Beep_Off()  {rGPBDAT &= ~(1 << 0);} 

extern void Beep_Init(void);

#endif





