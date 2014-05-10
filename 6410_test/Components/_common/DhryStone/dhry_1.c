/*======================================================================

 Project Name : S3C2443 verification project

 Copyright 2006 by Samsung Electronics, Inc.
 All rights reserved.

 Project Description :
 This software is only for verifying functions of the S3C2443. 
 Anybody can use this code without our permission.

 File Name    : strata16.c
 Description  : Dhrystone performance test code 
 Author       : Junon Jeon
 Dept         : AP
 Created Date : 2006.06.23
 Version      : 0.0
 History
   R0.0 (2006.06.23): Junon draft
		- This code is derived from dhrystone.c of S3C2440 test code.
		
=======================================================================*/
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "dhry.h"

#include "option.h"
#include "def.h"
#include "sfr6410.h"
#include "library.h"
#include "timer.h"

#ifndef REG
#define REG
        /* REG becomes defined as empty */
        /* i.e. no register variables   */
#endif

/* Global Variables: */

Rec_Pointer     Ptr_Glob,
                Next_Ptr_Glob;
int             Int_Glob;
Boolean         Bool_Glob;
char            Ch_1_Glob,
                Ch_2_Glob;
int             Arr_1_Glob [50];
int             Arr_2_Glob [50] [50];

/* extern char     *malloc (); */
						  
//Enumeration     Func_1 ();
  /* forward declaration necessary since Enumeration may not simply be int */

#ifndef REG
        Boolean Reg = false;
#define REG
        /* REG becomes defined as empty */
        /* i.e. no register variables   */
#else
        Boolean Reg = true;
#endif

/* variables for time measurement: */

#ifdef TIMES
struct tms      time_info;
extern  int     times ();
                /* see library function "times" */
#define Too_Small_Time (2*HZ)
                /* Measurements should last at least about 2 seconds */
#endif
#ifdef TIME
extern long     time();
                /* see library function "time"  */
#define Too_Small_Time 2
                /* Measurements should last at least 2 seconds */
#endif
#ifdef MSC_CLOCK
extern clock_t clock();
#define Too_Small_Time (2*HZ)
#endif

#ifdef MCU_S3C6410 
#define HZ (7813)  
//#define HZ (100000)  
#define Too_Small_Time (2*HZ)
#endif


long            Begin_Time,
                End_Time,
                User_Time;
float           Microseconds,
                Dhrystones_Per_Second;


u32 NO_OF_RUNS = 7000000;
//u32 NO_OF_RUNS = 50000;
#define MEGA	(1000000)

void Dhry_Timer_Setup(int divider);
int Dhry_Timer_Stop(void);

extern u32	g_MPLL, g_ARMCLK, g_HCLK, g_PCLK, g_MCLK;

enum WatchDog_REG
{
	rWTCON_T= WDT_BASE+0x00,
	rWTDAT_T = WDT_BASE+0x04,
	rWTCNT_T = WDT_BASE+0x08,
	rWTCLRINT_T = WDT_BASE+0x0C
};
	 
//*************************[ Timer for Dhrystone]********************************
void Dhry_Timer_Setup(int divider)  //0:16us,1:32us 2:64us 3:128us
{
	u32 uTemp;
	
    Outp32(rWTCON_T,((g_PCLK/1000000-1)<<8)|(divider<<3));  //Watch-dog timer control register
    Outp32(rWTDAT_T,0xffff);  //Watch-dog timer data register
    Outp32(rWTCNT_T,0xffff);  //Watch-dog count register

      // Watch-dog timer enable & interrupt  disable
    uTemp = Inp32(rWTCON_T);
    uTemp = (uTemp & ~(1<<5) & ~(1<<2)) |(1<<5);  
    Outp32(rWTCON_T, uTemp);

	//while(1)
	//  UART_Printf("rWTCON:%x, CNT:%x\n", rWTCON, rWTCNT);
}

#define Dhry_Timer_Start()	(*(u32 *)(rWTCON_T) = (*(u32 *)(rWTCON_T) & ~(1<<5) & ~(1<<2)) |(1<<5))

//=================================================================
int Dhry_Timer_Stop(void)
{
    Outp32(rWTCON_T,((g_PCLK/1000000-1)<<8));
    return (0xffff - Inp32(rWTCNT_T));
}

//rb1004
#if 0
void Dhrystone(void)
{
	UART_Printf("Dhrystone!!!\n");

#if 0
	// clock out selection
	rGPHCON = (rGPHCON&~(0xf<<26))|(0xa<<26); // use CLKOUT0, CLKOUT1 
	rMISCCR = (rMISCCR&~(0xff<<4))|(3<<8)|(2<<4); // [10:8]=CLKSEL1-3:HCLK, [6:4]=CLKSEL0-2:ARMCLK
	// manual pll change
	Manual_Change_Pll();

	SystemCLK(1);
	ChangeSDRAMParameter(g_HCLK);

	if(ARMCLK<400000000) 
	{
		NO_OF_RUNS = 100000;
	}
	else if(ARMCLK<550000000)
	{
		NO_OF_RUNS = 200000;
	}
	else
	{
		NO_OF_RUNS = 500000;
	}
/*
	if(ARMCLK<250000000) 
	{
		NO_OF_RUNS = 3000000;
	}
	else if(ARMCLK<450000000)
	{
		NO_OF_RUNS = 5000000;
	}
	else
	{
		NO_OF_RUNS = 7000000;
	}
*/
#endif
	Dhry_1();
}
#endif

void Dhry_1(void)
  /* main program, corresponds to procedures        */
  /* Main and Proc_0 in the Ada version             */
{
        One_Fifty       Int_1_Loc;
  REG   One_Fifty       Int_2_Loc;
        One_Fifty       Int_3_Loc;
  REG   char            Ch_Index;
        Enumeration     Enum_Loc;
        Str_30          Str_1_Loc;
        Str_30          Str_2_Loc;
  REG   int             Run_Index;
  REG   int             Number_Of_Runs;
	REG float mipsps;


  /* Initializations */

  Next_Ptr_Glob = (Rec_Pointer) malloc (sizeof (Rec_Type));
  Ptr_Glob = (Rec_Pointer) malloc (sizeof (Rec_Type));

  Ptr_Glob->Ptr_Comp                    = Next_Ptr_Glob;
  Ptr_Glob->Discr                       = Ident_1;
  Ptr_Glob->variant.var_1.Enum_Comp     = Ident_3;
  Ptr_Glob->variant.var_1.Int_Comp      = 40;
  strcpy (Ptr_Glob->variant.var_1.Str_Comp, 
          "DHRYSTONE PROGRAM, SOME STRING");
  strcpy (Str_1_Loc, "DHRYSTONE PROGRAM, 1'ST STRING");

  Arr_2_Glob [8][7] = 10;
        /* Was missing in published program. Without this statement,    */
        /* Arr_2_Glob [8][7] would have an undefined value.             */
        /* Warning: With 16-Bit processors and Number_Of_Runs > 32000,  */
        /* overflow may occur for this array element.                   */

  UART_Printf ("\n");
  UART_Printf ("Dhrystone Benchmark, Version 2.1 (Language: C)\n");
  UART_Printf ("\n");
  if (Reg)
  {
    UART_Printf ("Program compiled with 'register' attribute\n");
    UART_Printf ("\n");
  }
  else
  {
    UART_Printf ("Program compiled without 'register' attribute\n");
    UART_Printf ("\n");
  }
  UART_Printf ("Please give the number of runs through the benchmark: ");
  
  printf ("Please give the number of runs through the benchmark: ");
  {
    int n;
    n = UART_GetIntNum();
    Number_Of_Runs = n;
  }
  printf ("\n");

  UART_Printf ("Execution starts, %d runs through Dhrystone\n", Number_Of_Runs);

  /***************/
  /* Start timer */
  /***************/
 
#ifdef TIMES
  times (&time_info);
  Begin_Time = (long) time_info.tms_utime;
#endif
#ifdef TIME
  Begin_Time = time ( (long *) 0);
#endif
#ifdef MSC_CLOCK
  Begin_Time = clock();
#endif
#ifdef MCU_S3C6410
	Begin_Time = 0;
	//SetResStopwatch(10000); // us order, 10000us = 10ms
	StartTimer(0);
//  Dhry_Timer_Setup(3);   //if Watch-dog timer is used, divider=3 : 128us(all MCLK)
//10us:@20MHz-50,@40MHz-100,@60MHz-150,@66MHz-165,@73MHz:183=10.0274us
#endif
	
	// Timer start.
//	Dhry_Timer_Start();	// Timer start

  for (Run_Index = 1; Run_Index <= Number_Of_Runs; ++Run_Index)
  {
    Proc_5();
    Proc_4();
      /* Ch_1_Glob == 'A', Ch_2_Glob == 'B', Bool_Glob == true */
    Int_1_Loc = 2;
    Int_2_Loc = 3;
    strcpy (Str_2_Loc, "DHRYSTONE PROGRAM, 2'ND STRING");
    Enum_Loc = Ident_2;
    Bool_Glob = ! Func_2 (Str_1_Loc, Str_2_Loc);
      /* Bool_Glob == 1 */
    while (Int_1_Loc < Int_2_Loc)  /* loop body executed once */
    {
      Int_3_Loc = 5 * Int_1_Loc - Int_2_Loc;
        /* Int_3_Loc == 7 */
      Proc_7 (Int_1_Loc, Int_2_Loc, &Int_3_Loc);
        /* Int_3_Loc == 7 */
      Int_1_Loc += 1;
    } /* while */
      /* Int_1_Loc == 3, Int_2_Loc == 3, Int_3_Loc == 7 */
    Proc_8 (Arr_1_Glob, Arr_2_Glob, Int_1_Loc, Int_3_Loc);
      /* Int_Glob == 5 */
    Proc_1 (Ptr_Glob);
    for (Ch_Index = 'A'; Ch_Index <= Ch_2_Glob; ++Ch_Index)
                             /* loop body executed twice */
    {
      if (Enum_Loc == Func_1 (Ch_Index, 'C'))
          /* then, not executed */
        {
        Proc_6 (Ident_1, &Enum_Loc);
        strcpy (Str_2_Loc, "DHRYSTONE PROGRAM, 3'RD STRING");
        Int_2_Loc = Run_Index;
        Int_Glob = Run_Index;
        }
    }
      /* Int_1_Loc == 3, Int_2_Loc == 3, Int_3_Loc == 7 */
    Int_2_Loc = Int_2_Loc * Int_1_Loc;
    Int_1_Loc = Int_2_Loc / Int_3_Loc;
    Int_2_Loc = 7 * (Int_2_Loc - Int_3_Loc) - Int_1_Loc;
      /* Int_1_Loc == 1, Int_2_Loc == 13, Int_3_Loc == 7 */
    Proc_2 (&Int_1_Loc);
      /* Int_1_Loc == 5 */

  } /* loop "for Run_Index" */

  /**************/
  /* Stop timer */
  /**************/
  
#ifdef TIMES
  times (&time_info);
  End_Time = (long) time_info.tms_utime;
#endif
#ifdef TIME
  End_Time = time ( (long *) 0);
#endif
#ifdef MSC_CLOCK
  End_Time = clock();
#endif

#ifdef MCU_S3C6410  
	End_Time = StopTimer(0); 	   // us order

//  End_Time = Dhry_Timer_Stop();
//  UART_Printf("rWTCNT=0x%x\n",rWTCNT);
//  UART_Printf("End_Time=0x%x\n",End_Time);
#endif

#if 0
  UART_Printf ("Execution ends\n");
  UART_Printf ("\n");
  UART_Printf ("Final values of the variables used in the benchmark:\n");
  UART_Printf ("\n");
  UART_Printf ("Int_Glob:            %d\n", Int_Glob);
  UART_Printf ("        should be:   %d\n", 5);
  UART_Printf ("Bool_Glob:           %d\n", Bool_Glob);
  UART_Printf ("        should be:   %d\n", 1);
  UART_Printf ("Ch_1_Glob:           %c\n", Ch_1_Glob);
  UART_Printf ("        should be:   %c\n", 'A');
  UART_Printf ("Ch_2_Glob:           %c\n", Ch_2_Glob);
  UART_Printf ("        should be:   %c\n", 'B');
  UART_Printf ("Arr_1_Glob[8]:       %d\n", Arr_1_Glob[8]);
  UART_Printf ("        should be:   %d\n", 7);
  UART_Printf ("Arr_2_Glob[8][7]:    %d\n", Arr_2_Glob[8][7]);
  UART_Printf ("        should be:   Number_Of_Runs + 10\n");
  UART_Printf ("Ptr_Glob->\n");
  UART_Printf ("  Ptr_Comp:          %d\n", (int) Ptr_Glob->Ptr_Comp);
  UART_Printf ("        should be:   (implementation-dependent)\n");
  UART_Printf ("  Discr:             %d\n", Ptr_Glob->Discr);
  UART_Printf ("        should be:   %d\n", 0);
  UART_Printf ("  Enum_Comp:         %d\n", Ptr_Glob->variant.var_1.Enum_Comp);
  UART_Printf ("        should be:   %d\n", 2);
  UART_Printf ("  Int_Comp:          %d\n", Ptr_Glob->variant.var_1.Int_Comp);
  UART_Printf ("        should be:   %d\n", 17);
  UART_Printf ("  Str_Comp:          %s\n", Ptr_Glob->variant.var_1.Str_Comp);
  UART_Printf ("        should be:   DHRYSTONE PROGRAM, SOME STRING\n");
  UART_Printf ("Next_Ptr_Glob->\n");
  UART_Printf ("  Ptr_Comp:          %d\n", (int) Next_Ptr_Glob->Ptr_Comp);
  UART_Printf ("        should be:   (implementation-dependent), same as above\n");
  UART_Printf ("  Discr:             %d\n", Next_Ptr_Glob->Discr);
  UART_Printf ("        should be:   %d\n", 0);
  UART_Printf ("  Enum_Comp:         %d\n", Next_Ptr_Glob->variant.var_1.Enum_Comp);
  UART_Printf ("        should be:   %d\n", 1);
  UART_Printf ("  Int_Comp:          %d\n", Next_Ptr_Glob->variant.var_1.Int_Comp);
  UART_Printf ("        should be:   %d\n", 18);
  UART_Printf ("  Str_Comp:          %s\n",
                                Next_Ptr_Glob->variant.var_1.Str_Comp);
  UART_Printf ("        should be:   DHRYSTONE PROGRAM, SOME STRING\n");
  UART_Printf ("Int_1_Loc:           %d\n", Int_1_Loc);
  UART_Printf ("        should be:   %d\n", 5);
  UART_Printf ("Int_2_Loc:           %d\n", Int_2_Loc);
  UART_Printf ("        should be:   %d\n", 13);
  UART_Printf ("Int_3_Loc:           %d\n", Int_3_Loc);
  UART_Printf ("        should be:   %d\n", 7);
  UART_Printf ("Enum_Loc:            %d\n", Enum_Loc);
  UART_Printf ("        should be:   %d\n", 1);
  UART_Printf ("Str_1_Loc:           %s\n", Str_1_Loc);
  UART_Printf ("        should be:   DHRYSTONE PROGRAM, 1'ST STRING\n");
  UART_Printf ("Str_2_Loc:           %s\n", Str_2_Loc);
  UART_Printf ("        should be:   DHRYSTONE PROGRAM, 2'ND STRING\n");
  UART_Printf ("\n");
#endif

  User_Time = End_Time - Begin_Time;

  if (User_Time < Too_Small_Time)
  {
    UART_Printf ("Measured time too small to obtain meaningful results\n");
    UART_Printf ("Please increase number of runs\n");
	UART_Printf ("user_Time:%f us,Min_time:%f\n", (float)User_Time, (float)Too_Small_Time);
	UART_Printf ("Begin:%lf, End:%lf\n", (float)Begin_Time, (float)End_Time);
    UART_Printf ("\n");
  }
  else
  {
#ifdef MCU_S3C6410
    Microseconds = (float) User_Time / (float) Number_Of_Runs;
    Dhrystones_Per_Second = (float) Number_Of_Runs / ((float)User_Time/Mic_secs_Per_Second);
#else	
#ifdef TIME
    Microseconds = (float) User_Time * Mic_secs_Per_Second 
                        / (float) Number_Of_Runs;
    Dhrystones_Per_Second = (float) Number_Of_Runs / (float) User_Time;
#else
    Microseconds = (float) User_Time * Mic_secs_Per_Second 
                        / ((float) HZ * ((float) Number_Of_Runs));
    Dhrystones_Per_Second = ((float) HZ * (float) Number_Of_Runs)
                        / (float) User_Time;
#endif
#endif

    UART_Printf ("Processing time=%6.4f[sec]\n", (float)User_Time/1000000);  
    UART_Printf ("Microseconds for one run through Dhrystone: %10.5f \n", Microseconds);
    UART_Printf ("Dhrystones per Second: %10.5f \n", Dhrystones_Per_Second);
    
    UART_Printf ("MIPS:%10.5f \n", Dhrystones_Per_Second/1757);
	mipsps =  (Dhrystones_Per_Second/1757.)/((float)g_ARMCLK/MEGA);
	UART_Printf ("MIPS/Mhz(%10.5f/%5.1f):%8.4f \n", Dhrystones_Per_Second/1757.0, (float)g_ARMCLK/MEGA, mipsps);
  }
}



void Dhry_run(void)
  /* main program, corresponds to procedures        */
  /* Main and Proc_0 in the Ada version             */
{
	  int n;
	  
        One_Fifty       Int_1_Loc;
  REG   One_Fifty       Int_2_Loc;
        One_Fifty       Int_3_Loc;
  REG   char            Ch_Index;
        Enumeration     Enum_Loc;
        Str_30          Str_1_Loc;
        Str_30          Str_2_Loc;
  REG   int             Run_Index;
  REG   int             Number_Of_Runs;
//rb1004
//	REG float mipsps;


  /* Initializations */

  Next_Ptr_Glob = (Rec_Pointer) malloc (sizeof (Rec_Type));
  Ptr_Glob = (Rec_Pointer) malloc (sizeof (Rec_Type));

  Ptr_Glob->Ptr_Comp                    = Next_Ptr_Glob;
  Ptr_Glob->Discr                       = Ident_1;
  Ptr_Glob->variant.var_1.Enum_Comp     = Ident_3;
  Ptr_Glob->variant.var_1.Int_Comp      = 40;
  strcpy (Ptr_Glob->variant.var_1.Str_Comp, 
          "DHRYSTONE PROGRAM, SOME STRING");
  strcpy (Str_1_Loc, "DHRYSTONE PROGRAM, 1'ST STRING");

  Arr_2_Glob [8][7] = 10;
        /* Was missing in published program. Without this statement,    */
        /* Arr_2_Glob [8][7] would have an undefined value.             */
        /* Warning: With 16-Bit processors and Number_Of_Runs > 32000,  */
        /* overflow may occur for this array element.                   */

#if 0
  UART_Printf ("\n");
  UART_Printf ("Dhrystone Benchmark, Version 2.1 (Language: C)\n");
  UART_Printf ("\n");
  if (Reg)
  {
    UART_Printf ("Program compiled with 'register' attribute\n");
    UART_Printf ("\n");
  }
  else
  {
    UART_Printf ("Program compiled without 'register' attribute\n");
    UART_Printf ("\n");
  }
  UART_Printf ("Please give the number of runs through the benchmark: ");
  
 #endif
 

  
  n=100000;//Change this...
  //n=10000000;//Change this...

//  Number_Of_Runs = NO_OF_RUNS;
  Number_Of_Runs = n;
 
 #if 0 
  UART_Printf ("\n");

  UART_Printf ("Execution starts, %d runs through Dhrystone\n", Number_Of_Runs);
 
 #endif
  /***************/
  /* Start timer */
  /***************/
 
#ifdef TIMES
  times (&time_info);
  Begin_Time = (long) time_info.tms_utime;
#endif
#ifdef TIME
  Begin_Time = time ( (long *) 0);
#endif
#ifdef MSC_CLOCK
  Begin_Time = clock();
#endif
#ifdef MCU_S3C6410
//	Begin_Time = 0;
//	SetResStopwatch(10000); // us order, 10000us = 10ms
//	StartStopwatch();
//  Dhry_Timer_Setup(3);   //if Watch-dog timer is used, divider=3 : 128us(all MCLK)
//10us:@20MHz-50,@40MHz-100,@60MHz-150,@66MHz-165,@73MHz:183=10.0274us
#endif
	
	// Timer start.
//	Dhry_Timer_Start();	// Timer start

  for (Run_Index = 1; Run_Index <= Number_Of_Runs; ++Run_Index)
  {
    Proc_5();
    Proc_4();
      /* Ch_1_Glob == 'A', Ch_2_Glob == 'B', Bool_Glob == true */
    Int_1_Loc = 2;
    Int_2_Loc = 3;
    strcpy (Str_2_Loc, "DHRYSTONE PROGRAM, 2'ND STRING");
    Enum_Loc = Ident_2;
    Bool_Glob = ! Func_2 (Str_1_Loc, Str_2_Loc);
      /* Bool_Glob == 1 */
    while (Int_1_Loc < Int_2_Loc)  /* loop body executed once */
    {
      Int_3_Loc = 5 * Int_1_Loc - Int_2_Loc;
        /* Int_3_Loc == 7 */
      Proc_7 (Int_1_Loc, Int_2_Loc, &Int_3_Loc);
        /* Int_3_Loc == 7 */
      Int_1_Loc += 1;
    } /* while */
      /* Int_1_Loc == 3, Int_2_Loc == 3, Int_3_Loc == 7 */
    Proc_8 (Arr_1_Glob, Arr_2_Glob, Int_1_Loc, Int_3_Loc);
      /* Int_Glob == 5 */
    Proc_1 (Ptr_Glob);
    for (Ch_Index = 'A'; Ch_Index <= Ch_2_Glob; ++Ch_Index)
                             /* loop body executed twice */
    {
      if (Enum_Loc == Func_1 (Ch_Index, 'C'))
          /* then, not executed */
        {
        Proc_6 (Ident_1, &Enum_Loc);
        strcpy (Str_2_Loc, "DHRYSTONE PROGRAM, 3'RD STRING");
        Int_2_Loc = Run_Index;
        Int_Glob = Run_Index;
        }
    }
      /* Int_1_Loc == 3, Int_2_Loc == 3, Int_3_Loc == 7 */
    Int_2_Loc = Int_2_Loc * Int_1_Loc;
    Int_1_Loc = Int_2_Loc / Int_3_Loc;
    Int_2_Loc = 7 * (Int_2_Loc - Int_3_Loc) - Int_1_Loc;
      /* Int_1_Loc == 1, Int_2_Loc == 13, Int_3_Loc == 7 */
    Proc_2 (&Int_1_Loc);
      /* Int_1_Loc == 5 */

  } /* loop "for Run_Index" */

  /**************/
  /* Stop timer */
  /**************/
  
#ifdef TIMES
  times (&time_info);
  End_Time = (long) time_info.tms_utime;
#endif
#ifdef TIME
  End_Time = time ( (long *) 0);
#endif
#ifdef MSC_CLOCK
  End_Time = clock();
#endif

#ifdef MCU_S3C6410  
//	End_Time = EndStopwatch(); 	   // us order

//  End_Time = Dhry_Timer_Stop();
//  UART_Printf("rWTCNT=0x%x\n",rWTCNT);
//  UART_Printf("End_Time=0x%x\n",End_Time);
#endif

#if 0
  UART_Printf ("Execution ends\n");
  UART_Printf ("\n");
  UART_Printf ("Final values of the variables used in the benchmark:\n");
  UART_Printf ("\n");
  UART_Printf ("Int_Glob:            %d\n", Int_Glob);
  UART_Printf ("        should be:   %d\n", 5);
  UART_Printf ("Bool_Glob:           %d\n", Bool_Glob);
  UART_Printf ("        should be:   %d\n", 1);
  UART_Printf ("Ch_1_Glob:           %c\n", Ch_1_Glob);
  UART_Printf ("        should be:   %c\n", 'A');
  UART_Printf ("Ch_2_Glob:           %c\n", Ch_2_Glob);
  UART_Printf ("        should be:   %c\n", 'B');
  UART_Printf ("Arr_1_Glob[8]:       %d\n", Arr_1_Glob[8]);
  UART_Printf ("        should be:   %d\n", 7);
  UART_Printf ("Arr_2_Glob[8][7]:    %d\n", Arr_2_Glob[8][7]);
  UART_Printf ("        should be:   Number_Of_Runs + 10\n");
  UART_Printf ("Ptr_Glob->\n");
  UART_Printf ("  Ptr_Comp:          %d\n", (int) Ptr_Glob->Ptr_Comp);
  UART_Printf ("        should be:   (implementation-dependent)\n");
  UART_Printf ("  Discr:             %d\n", Ptr_Glob->Discr);
  UART_Printf ("        should be:   %d\n", 0);
  UART_Printf ("  Enum_Comp:         %d\n", Ptr_Glob->variant.var_1.Enum_Comp);
  UART_Printf ("        should be:   %d\n", 2);
  UART_Printf ("  Int_Comp:          %d\n", Ptr_Glob->variant.var_1.Int_Comp);
  UART_Printf ("        should be:   %d\n", 17);
  UART_Printf ("  Str_Comp:          %s\n", Ptr_Glob->variant.var_1.Str_Comp);
  UART_Printf ("        should be:   DHRYSTONE PROGRAM, SOME STRING\n");
  UART_Printf ("Next_Ptr_Glob->\n");
  UART_Printf ("  Ptr_Comp:          %d\n", (int) Next_Ptr_Glob->Ptr_Comp);
  UART_Printf ("        should be:   (implementation-dependent), same as above\n");
  UART_Printf ("  Discr:             %d\n", Next_Ptr_Glob->Discr);
  UART_Printf ("        should be:   %d\n", 0);
  UART_Printf ("  Enum_Comp:         %d\n", Next_Ptr_Glob->variant.var_1.Enum_Comp);
  UART_Printf ("        should be:   %d\n", 1);
  UART_Printf ("  Int_Comp:          %d\n", Next_Ptr_Glob->variant.var_1.Int_Comp);
  UART_Printf ("        should be:   %d\n", 18);
  UART_Printf ("  Str_Comp:          %s\n",
                                Next_Ptr_Glob->variant.var_1.Str_Comp);
  UART_Printf ("        should be:   DHRYSTONE PROGRAM, SOME STRING\n");
  UART_Printf ("Int_1_Loc:           %d\n", Int_1_Loc);
  UART_Printf ("        should be:   %d\n", 5);
  UART_Printf ("Int_2_Loc:           %d\n", Int_2_Loc);
  UART_Printf ("        should be:   %d\n", 13);
  UART_Printf ("Int_3_Loc:           %d\n", Int_3_Loc);
  UART_Printf ("        should be:   %d\n", 7);
  UART_Printf ("Enum_Loc:            %d\n", Enum_Loc);
  UART_Printf ("        should be:   %d\n", 1);
  UART_Printf ("Str_1_Loc:           %s\n", Str_1_Loc);
  UART_Printf ("        should be:   DHRYSTONE PROGRAM, 1'ST STRING\n");
  UART_Printf ("Str_2_Loc:           %s\n", Str_2_Loc);
  UART_Printf ("        should be:   DHRYSTONE PROGRAM, 2'ND STRING\n");
  UART_Printf ("\n");
#endif

  User_Time = End_Time - Begin_Time;

//  if (User_Time < Too_Small_Time)
   if (0)
  {
    UART_Printf ("Measured time too small to obtain meaningful results\n");
    UART_Printf ("Please increase number of runs\n");
	UART_Printf ("user_Time:%f us,Min_time:%f\n", (float)User_Time, (float)Too_Small_Time);
	UART_Printf ("Begin:%lf, End:%lf\n", (float)Begin_Time, (float)End_Time);
    UART_Printf ("\n");
  }
  else
  {
#ifdef TIME
    Microseconds = (float) User_Time / (float) Number_Of_Runs;
    Dhrystones_Per_Second = (float) Number_Of_Runs / ((float)User_Time/1000000);
#else
    Microseconds = (float) User_Time / ((float) HZ * ((float) Number_Of_Runs));
    Dhrystones_Per_Second = ((float) HZ * (float) Number_Of_Runs)
                        / (float) User_Time/1000000;
#endif

    Microseconds = (float) User_Time / (float) Number_Of_Runs;
    Dhrystones_Per_Second = (float) Number_Of_Runs / ((float)User_Time/1000000);
#if 0
    UART_Printf ("Processing time=%6.4f[sec]\n", (float)User_Time/1000000);  
    UART_Printf ("Microseconds for one run through Dhrystone: %10.5f \n", Microseconds);
    UART_Printf ("Dhrystones per Second: %10.5f \n", Dhrystones_Per_Second);
    UART_Printf ("MIPS:%10.5f \n", Dhrystones_Per_Second/1757);
	mipsps =  (Dhrystones_Per_Second/1757.)/((float)ARMCLK/MEGA);
	UART_Printf ("MIPS/Mhz(%10.5f/%5.1f):%8.4f \n", Dhrystones_Per_Second/1757.0, (float)ARMCLK/MEGA, mipsps);
 #endif
  }
}


void Proc_1 (REG Rec_Pointer Ptr_Val_Par)
/******************/
    /* executed once */
{
  REG Rec_Pointer Next_Record = Ptr_Val_Par->Ptr_Comp;  
                                        /* == Ptr_Glob_Next */
  /* Local variable, initialized with Ptr_Val_Par->Ptr_Comp,    */
  /* corresponds to "rename" in Ada, "with" in Pascal           */
  
  structassign (*Ptr_Val_Par->Ptr_Comp, *Ptr_Glob); 
  Ptr_Val_Par->variant.var_1.Int_Comp = 5;
  Next_Record->variant.var_1.Int_Comp 
        = Ptr_Val_Par->variant.var_1.Int_Comp;
  Next_Record->Ptr_Comp = Ptr_Val_Par->Ptr_Comp;
  Proc_3 (&Next_Record->Ptr_Comp);
    /* Ptr_Val_Par->Ptr_Comp->Ptr_Comp 
                        == Ptr_Glob->Ptr_Comp */
  if (Next_Record->Discr == Ident_1)
    /* then, executed */
  {
    Next_Record->variant.var_1.Int_Comp = 6;
    Proc_6 (Ptr_Val_Par->variant.var_1.Enum_Comp, 
           &Next_Record->variant.var_1.Enum_Comp);
    Next_Record->Ptr_Comp = Ptr_Glob->Ptr_Comp;
    Proc_7 (Next_Record->variant.var_1.Int_Comp, 10, 
           &Next_Record->variant.var_1.Int_Comp);
  }
  else /* not executed */
    structassign (*Ptr_Val_Par, *Ptr_Val_Par->Ptr_Comp);
} /* Proc_1 */


void Proc_2 (One_Fifty *Int_Par_Ref)
/******************/
    /* executed once */
    /* *Int_Par_Ref == 1, becomes 4 */
{
  One_Fifty  Int_Loc;  
  Enumeration   Enum_Loc;

  Int_Loc = *Int_Par_Ref + 10;
  do /* executed once */
    if (Ch_1_Glob == 'A')
      /* then, executed */
    {
      Int_Loc -= 1;
      *Int_Par_Ref = Int_Loc - Int_Glob;
      Enum_Loc = Ident_1;
    } /* if */
  while (Enum_Loc != Ident_1); /* true */
} /* Proc_2 */


void Proc_3 (Rec_Pointer *Ptr_Ref_Par)
/******************/
    /* executed once */
    /* Ptr_Ref_Par becomes Ptr_Glob */
{
  if (Ptr_Glob != Null)
    /* then, executed */
    *Ptr_Ref_Par = Ptr_Glob->Ptr_Comp;
  Proc_7 (10, Int_Glob, &Ptr_Glob->variant.var_1.Int_Comp);
} /* Proc_3 */


void Proc_4 (void) /* without parameters */
/*******/
    /* executed once */
{
  Boolean Bool_Loc;

  Bool_Loc = Ch_1_Glob == 'A';
  Bool_Glob = Bool_Loc | Bool_Glob;
  Ch_2_Glob = 'B';
} /* Proc_4 */


void Proc_5 (void) /* without parameters */
/*******/
    /* executed once */
{
  Ch_1_Glob = 'A';
  Bool_Glob = false;
} /* Proc_5 */


        /* Procedure for the assignment of structures,          */
        /* if the C compiler doesn't support this feature       */
#ifdef  NOSTRUCTASSIGN
memcpy (d, s, l)
register char   *d;
register char   *s;
register int    l;
{
        while (l--) *d++ = *s++;
}
#endif
