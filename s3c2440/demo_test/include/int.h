//====================================================================
// File Name : 2440int.h
// Function  : S3C2440 Interrupt Test Head File
// Program   : Shin, On Pil (SOP)
// Date      : March 12, 2002
// Version   : 0.0
// History
//   0.0 : Programming start (March 12, 2002) -> SOP
//   0.1 : modified for s3c2440 (May xx, 2003) -> DonGo
//====================================================================

#ifndef __2440INT_H__
#define __2440INT_H__

void Test_Int(void);

void Test_Fiq(void);
void Change_IntPriorities(void);

#endif     //__2440INT_H__
