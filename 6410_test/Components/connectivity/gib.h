 /*----------------------------------------------------------------------
 *
 * Filename: gps.h
 *
 * Contents: Header for gps.c
 *
 * History: 
 * 	1) 2007,JAN,23th	Original, CellGuide
 * 	2) 2007,JAN,23th	CellGuide - added TCXO test, reset and snap test
 *
 * Notes:
 *   1. Don't remove any author name & revision history
 *   2. 6410 - used DMA Channel 1
 *
 * Copyright (c) 2007 CellGuide LTD.
 *
 *----------------------------------------------------------------------
 */
#ifndef _GIB_H_
#define _GIB_H_

#ifndef NULL
	#define NULL 0
#endif

#define	Silicon_6410
//#define	FPGA_6410


#define SNAP_DELAY_SEC (1)
#define WORDS_IN_LINE 4

#define REG32(r) 	*((volatile u32*)(r))
			   //(*(volatile u32 *)(addr))

void GIB_TestCoreReset(void);
void GIB_TestRegistersDefaultValue(void);
void GIB_TestRegistersWrite(void);
void GIB_regWrite(int aIndex, u32 aValue);
void GIB_regRead(int aIndex, u32 aExpected);

void GIB_TestTcxoReset(void);
void GIB_TestTcxoCounter(void);

void GIB_TestSnap1(u32 aLengthMs, u32 aTestMode);
void GIB_TestOverrunInterrupt(void);
void GIB_InitGpio(void);
void GIB_InitRf(void);

void GIB_CoreReset(void);
void GIB_TimerReset(void);
unsigned long GIB_ReversBits(unsigned long SnapData);

u32 GIB_AutoTestRegistersDefaultValue(void);
void GIB_AutoregRead(int aIndex, u32 aExpected);

#endif
