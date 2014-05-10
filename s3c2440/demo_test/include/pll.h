#ifndef __PLL_H__
#define __PLL_H__

// Main function
void Test_PLL(void);
void Manual_Change_Pll(void);
void Manual_Change_Pll_Iostrength(void);
void Test_PllOnOff(void);
void Test_Locktime(void);

// Sub function
U32 Calc_Locktime(U32 lock_time);

#endif /*__PLL_H__*/
