/*-------------------------------------------------------------------
 * File Name : gps_test.c
 * Contents  : Test code for GIB IP in S3C6410
 * Authors    : Dusang.Kim
 *
 * Notes:
 *   1. Don't remove any author name & revision history
 *   2. 6410 - used DMA Channel 1
 * 
 * Copyright (c) 2007 SAMSUNG Electronics.
 *--------------------------------------------------------------------
 */

#include "system.h"
#include "gib.h"
#include "gpio.h"

void Test_GIB_Reset(void);
void Test_GIB_RegistersAccess(void);
void Test_GIB_TCXO(void);
void Test_GIB_Snap(void);
void Test_GIB_Interrupt(void);
void Test_GIB_rf_config(void);
void Test_GIB_RealSatellitesSnapViaRF(void);
u8 GIB_AutoTest(void);

#ifdef FPGA_6410
void GIB_Test(void)
{
	u32 i;
	u32 iSel;

	FUNC_MENU g_aGIBTestFunc[]=
	{
#endif
#ifdef Silicon_6410
void GIB_Test(void)
{
	static u32 i;
	u32 iSel;
	u32 reg;

	const testFuncMenu g_aGIBTestFunc[] =		
	{
#endif
		0,                                 				"Exit",
		Test_GIB_Reset,						"Test_GIB_Reset",
		Test_GIB_RegistersAccess,			"Test_GIB_RegistersAccess",
		Test_GIB_TCXO,						"Test_GIB_TCXO",
		Test_GIB_Snap,						"Test_GIB_Snap",
		Test_GIB_Interrupt,					"Test_GIB_Interrupt",
		Test_GIB_rf_config,					"Test_GIB_rf_config",
		Test_GIB_RealSatellitesSnapViaRF,	"Test_GIB_RealSatellitesSnapViaRF",
		GIB_AutoTest,						"Auto Test",
		0,0
	};
	
	#ifdef FPGA_6410
		SCON_GpsPowerOn();
	#endif
	#ifdef Silicon_6410
		reg=REG32(0x7E00F810); // 0x7E00F810 = Normal CFG
		reg&=~(1<<11);
		REG32(0x7E00F810) = reg;

		REG32(0x7e00f110) = 0x10000000; //select normal DMA(defalut is sdma) 
	#endif
	
	//i = REG32(0x7E00F90C);
	//while(!(i&0x0100));

	//Delay(10000);
	
	while(1)
	{

 		Disp("\n");
		Disp("\t [ 6410 Test ] \n");

		for (i=0; (int)(g_aGIBTestFunc[i].desc)!=0; i++)
			Disp("%2d: %s\n", i, g_aGIBTestFunc[i].desc);

		Disp("\nSelect the function to test : ");
		iSel = UART_GetIntNum();
		Disp("\n");

		if (iSel == 0)
			break;
		else if (iSel>0 && iSel<(sizeof(g_aGIBTestFunc)/8-1))
		{	
			(g_aGIBTestFunc[iSel].func) ();
		}
	}
}




/* [1] */
void Test_GIB_Reset(void)
{
	Disp("\n--- [Cgx5500 Reset Test] ---------------------------\n");
	GIB_TestCoreReset();	
	GIB_TestTcxoReset();	
} 



/* [2] */
void Test_GIB_RegistersAccess(void)
{
	Disp("\n ______[ CGX5500 'RO' Register \"Reset Value\" check Start ]_____\n");
	GIB_TestRegistersDefaultValue();
	Disp("\n[ Checking End (No Error is no problem)]");
	Disp("\n\n\n");
	
	Disp("\n ______[ CGX5500 'Write' Register \"W/R\" check Start ]_____\n");
	GIB_TestRegistersWrite();			
	Disp("\n[ Checking End (No Error is no problem)]");
	Disp("\n\n\n");
}



/* [3] */
void Test_GIB_TCXO(void)
{
	Disp("\n--- [Cgx5500 TCXO Counter Test] --------------------\n");
	GIB_TestTcxoCounter();
}



/* [4] */
void Test_GIB_Snap(void)
{
	Disp("\n--- [Cgx5500 Snap Test] ----------------------------\n");
	GIB_TestSnap1(128, 1);

	Disp("Press \"ENTER\" key\n");
	UART_GetIntNum();
	
	GIB_TestSnap1(256, 0);
	Disp("Press \"ENTER\" key\n");
	UART_GetIntNum();
}



/* [5] */
void Test_GIB_Interrupt(void)
{
	Disp("\n--- [Cgx5500 Interrupt Test] -----------------------\n");
	GIB_TestOverrunInterrupt();	
} 



/* [6] */
void Test_GIB_rf_config(void)
{
	Disp("\n--- [Cgx5500 RF config Test] -----------------------\n");	
	GIB_InitGpio();
	GIB_InitRf();	
}



/* [7] */
void Test_GIB_RealSatellitesSnapViaRF(void)
{
	Disp("\n--- [Cgx5500 Snap Test via RF] ---------------------\n");	
	Test_GIB_rf_config();
	GIB_TestSnap1(128, 0); //original, 1==test mode, 0==real mode
	//GIB_TestSnap1(1, 0); //original, 1==test mode, 0==real mode



	Disp("Press \"ENTER\" key\n");	
}

#if 1
u8 GIB_AutoTest(void)
{
	u32 uErrCnt;

	Disp("\n [ CGX5500 'RO' Register \"Reset Value\" check Start ]\n");
	uErrCnt = GIB_AutoTestRegistersDefaultValue();

	if(uErrCnt == 0)
	{
		Disp("\n[ GIB Auto Test.....................OK ]\n");
		return true;
	}		

	else
	{
		Disp("\n[ GIB Auto Test (Fail CNT : %d)...............FAIL ]\n", uErrCnt);
		return false;
	}
	
	/*
	Disp("\n [ CGX5500 'Write' Register \"W/R\" check Start ]\n");
	GIB_TestRegistersWrite();			
	Disp("\n[ Checking End (No Error is no problem)]");
	Disp("\n\n\n");
	*/
}
#endif

