/**************************************************************************************
* 
*	Project Name : S3C6400 Validation
*
*	Copyright 2006 by Samsung Electronics, Inc.
*	All rights reserved.
*
*	Project Description :
*		This software is only for validating functions of the S3C6400.
*		Anybody can use this software without our permission.
*  
*--------------------------------------------------------------------------------------
* 
*	File Name : scatter_load.c
*  
*	File Description : This file implements the scatterload function.
*
*	Author : Heemyung.Noh
*	Dept. : AP Development Team
*	Created Date : 2007/09/04
* 
*	History
*	- Created(Heemyung.Noh 2007/09/04)
*  
**************************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "def.h"

void ScatterMemcpy (volatile u32* d, volatile u32* s, u32 l)
{
  volatile u32 i;

  for (i=0,l/=4 ; i<l ; i++)
    d[i] = s[i];
}

void ScatterMemset (volatile u32* d, u32 v, u32 l)
{
  volatile u32 i;

  for (i=0,l/=4 ; i<l ; i++)
    d[i] = v;
}

#define ScatterLoadRegion(__region) \
{ \
  extern u32 Load$$ ## __region ## $$Base, Image$$ ## __region ## $$Base, Image$$ ## __region ## $$Length, Image$$ ## __region ## $$ZI$$Base, Image$$ ## __region ## $$ZI$$Length; \
  if (&Load$$ ## __region ## $$Base != &Image$$ ## __region ## $$Base) \
    ScatterMemcpy(&Image$$ ## __region ## $$Base, &Load$$ ## __region ## $$Base, (u32)&Image$$ ## __region ## $$Length); \
  if ((u32)&Image$$ ## __region ## $$ZI$$Length != 0) \
    ScatterMemset(&Image$$ ## __region ## $$ZI$$Base, 0, (u32)&Image$$ ## __region ## $$ZI$$Length); \
}

void ScatterLoad (void)
{
  ScatterLoadRegion(IRAM);
  if(USE_TCM)
  {
  	ScatterLoadRegion(ITCM);
  	ScatterLoadRegion(DTCM);
  }
}
