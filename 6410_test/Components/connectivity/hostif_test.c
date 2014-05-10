/**
 * HOSTIF_TEST.C
 *
 * @author	SONG JAE GEUN
 * @version         1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "system.h"
#include "option.h"
#include "library.h"
#include "intc.h"
#include "sysc.h"

#include "nand.h"
#include "hostif.h"
#include "timer.h"
#include "modemif.h"

u8	g_ucIMBFlag;
u8	g_ucOMBFlag;

extern NAND_oInform NAND_Inform[NAND_CONNUM];

extern void NANDT_READ_Simple(void);

void Test_HOSTIF_AP_Int(void)
{
	HOSTIF_AP_Init();

	while(!Getc());
	
	INTC_Disable(NUM_HOSTIF);
}

void Test_HOSTIF_MODEM_Int(void)
{	
	HOSTIF_MODEM_Init();

	while(!Getc());
	
	INTC_Disable(NUM_EINT1);
}

void Test_Single_Read(void)
{
	u32 addr, temp;

	HOSTIF_MODEM_Init();
	
	UART_Printf("\nInput HOST I/F SingleRead AP's Address(Destination) : ");
	addr = UART_GetIntNum();

	temp = HOSTIF_SingleRead(addr);
	UART_Printf("\nRead Data = 0x%x\n", temp);
	
}

void Test_Single_ReadWrite(void)
{
	u32 addr, data, temp;

	HOSTIF_MODEM_Init();
	
	UART_Printf("\nInput HOST I/F SingleWrite AP's Address(Destination's Address) : ");
	addr = UART_GetIntNum();

	UART_Printf("\nInput HOST I/F SingleWrite Host's DATA that writes to the AP : ");
	data = UART_GetIntNum();
	
	HOSTIF_SingleWrite(addr, data);
	temp = HOSTIF_SingleRead(addr);
	UART_Printf("\nRead data from AP's target address = 0x%x\n", temp);

	if (data == temp) 
		UART_Printf("\nHOST I/F Single Read/Write test completed!!!\n");
	else
		UART_Printf("HOST I/F Single Read/Write test error!!!\n");		
	
}

void Test_Burst_Read(void)
{
	u32 addr, data, size;
	//u32* pSrcaddr;

	UART_Printf("\nInput HOST I/F BurstRead AP's Source Address (Source): ");
	addr = UART_GetIntNum();

	UART_Printf("\nInput HOST I/F BurstRead Host's Data Address (Destination): ");
	data = UART_GetIntNum();

	UART_Printf("\nInput HOST I/F BurstRead DataSize : ");
	size = UART_GetIntNum();

	HOSTIF_BurstRead(addr, (u16 *)data, size);

}

void Test_Burst_ReadWrite(void)
{
	u32 addr, data, size;
	//u32* pSrcaddr;

	UART_Printf("\nInput HOST I/F BurstWrite AP's Address (Destination): ");
	addr = UART_GetIntNum();

	UART_Printf("\nInput HOST I/F BurstWrite Host's Data Address (Source) : ");
	data = UART_GetIntNum();

	UART_Printf("\nInput HOST I/F BurstWrite DataSize : ");
	size = UART_GetIntNum();

	HOSTIF_BurstWrite(addr, (u32 *)data, size);
	
	UART_Printf("\nInput HOST I/F BurstRead AP's Source Address (Source): ");
	addr = UART_GetIntNum();

	UART_Printf("\nInput HOST I/F BurstRead Host's Data Address (Destination): ");
	data = UART_GetIntNum();

	UART_Printf("\nInput HOST I/F BurstWrite DataSize : ");
	size = UART_GetIntNum();

	HOSTIF_BurstRead(addr, (u16 *)data, size);

}

void Test_RepeatedBurst_Write(void)
{
	u32 addr, data, size;
	//u32* pSrcaddr;

	UART_Printf("\nInput HOST I/F RepeatedBurstWrite AP's Address(Destination) : ");
	addr = UART_GetIntNum();

	UART_Printf("\nInput HOST I/F RepeatedBurstWrite Host's Data Address(Source) : ");
	data = UART_GetIntNum();

	UART_Printf("\nInput HOST I/F RepeatedBurstWrite DataSize : ");
	size = UART_GetIntNum();

	HOSTIF_RepeatedBurstWrite(addr, (u32 *) data, size);
}

void Test_Send_InMailBox(void)
{
	u32	uProtocol = 0x1234ABCD;
	
	HOSTIF_MODEM_Init();

	UART_Printf("Send 0x%x to IMB of AP side...", uProtocol);

	HOSTIF_InMailBox(uProtocol);

	UART_Printf("Done!!\n");

	INTC_Disable(NUM_EINT1);
}

void Test_Receive_InMailBox(void)
{
	g_ucIMBFlag = 0;

	// Write some value to OMB to clear OMB_EMPTY Interrupt
	HOSTIF_OutMailBox(0xcdef1234);
	
	HOSTIF_AP_Init();
	
	UART_Printf("Waiting for IMB data from Modem side...\n");
	// Wait Receive IMB
	while(g_ucIMBFlag == 0);

	INTC_Disable(NUM_HOSTIF);
	
	UART_Printf("IMB Received\n");

}

void Test_Send_OutMailBox(void)
{
	u32	uProtocol = 0xabcd5678;


	UART_Printf("Send 0x%x to OMB ...", uProtocol);

	HOSTIF_OutMailBox(uProtocol);
	
	UART_Printf("Done!!\n");

	INTC_Disable(NUM_HOSTIF);
}

void Test_Receive_OutMailBox(void)
{
	u32 message;
	u32 uTemp;
	
	HOSTIF_MODEM_Init();

	// Clear OMB_FILLED
	//HOSTIF_SelectBank(BANK1);
	//uTemp = Inp32(rSTAT1);
	//Outp32(rSTAT1, uTemp|(1<<INT1_OMB_FILLED));
	
	message = HOSTIF_ReadOutMailBox();
	UART_Printf("Message = 0x%x\n", message);

	UART_Printf("Waiting for OMB data from AP side...\n");
	// Wait Receive IMB
	while(g_ucOMBFlag == 0);

	INTC_Disable(NUM_EINT1);
	
	UART_Printf("OMB Received\n");

}

#define MODEM_DRAM_BUFFER_ADDRESS	0x52000000
#define AP_BL_ADDRESS					0x0c000000
#define AP_OS_ADDRESS					0x50100000

void Test_MODEM_Booting(void)
{
	u32 data, lengh;
	u32 maxlengh = 0x1000;

	// MODEM I/F GPIO Setting
	MODEMIF_Set_Modem_Booting_Port();
	Delay(1000);
	
	// MODEM Booting AP Reset
	MODEMIF_AP_Reset();

	// SROM BUS 16bit setting
	SYSC_16bitBUS();

	// MODEM bootloader copy to DPSRAM
	NAND_Inform[0].uNandType = NAND_Normal8bit;
	NAND_Inform[0].uAddrCycle = 4;
	NAND_Inform[0].uBlockNum = 4096;
	NAND_Inform[0].uPageNum = 32;
	NAND_Inform[0].uPageSize = NAND_PAGE_512;
	NAND_Inform[0].uSpareSize = NAND_SPARE_16;
	NAND_Inform[0].uECCtest = 0;
	NAND_Inform[0].uSpareECCtest = 0;		// This line should be added for spare area ecc in S3C6410. If not, nand ecc would be fail right after reading first 512bytes. 080313. derrick.
	NAND_Inform[0].uTacls = 0;
	NAND_Inform[0].uTwrph0 = 25+10;	//Pad delay : about 10ns
	NAND_Inform[0].uTwrph1 = 15+5;	//tWH : 15ns

	NAND_Init(0);
	NAND_ReadMultiPage(0, 10, 0, (u8 *)MODEM_DRAM_BUFFER_ADDRESS, maxlengh);
//	NAND_ReadMultiPage(0, 0, 0, (u8 *)0x52000000, maxlengh);

#if 0	// Single Write
	for (lengh = 0 ; lengh <= maxlengh ; lengh+=4)
	{
		data = Inp32(MODEM_DRAM_BUFFER_ADDRESS + lengh);
		HOSTIF_SingleWrite(AP_BL_ADDRESS+lengh, data);
	}
#elif 0	// Burst Write
	HOSTIF_BurstWrite(AP_BL_ADDRESS, (u32 *)MODEM_DRAM_BUFFER_ADDRESS, maxlengh);
#else	// Repeated Burst Write
	HOSTIF_RepeatedBurstWrite(AP_BL_ADDRESS, (u32 *) MODEM_DRAM_BUFFER_ADDRESS, maxlengh);
#endif
	data = 0x53000000;

#if 0	// just for confirming boot data
	for (lengh = 0 ; lengh <= maxlengh ; lengh+=4)
		*(unsigned int*)(data+lengh) = HOSTIF_SingleRead(AP_BL_ADDRESS+lengh);
#endif

	UART_Printf("Press any key and BOOT CODE(0x0c000000) start!!!\n");
	Getc();
	// Write Boot Done...
	HOSTIF_MODEM_To_AP_Reset();

	HOSTIF_SelectBank(BANK11);
	data = Inp16(rCTRL);
	UART_Printf("BootDone:%d\n",data);
	Outp16(rCTRL, 0x0);
	data = Inp16(rCTRL);
	UART_Printf("BootDone Clear:%d\n",data);	
	
}

void Test_MODEM_Booting_OS(void)
{
	u32 data, lengh;
	u32 maxlengh = 0x1000;
	u32 uMaxOSLength = 0x1f00000;	// about 33MBytes
	NAND_eERROR eNandErr=eNAND_NoError;
	

	// MODEM I/F GPIO Setting
	MODEMIF_Set_Modem_Booting_Port();
	Delay(1000);
	
	// MODEM Booting AP Reset
	MODEMIF_AP_Reset();

	// SROM BUS 16bit setting
	SYSC_16bitBUS();

	// MODEM bootloader copy to DPSRAM
	NAND_Inform[0].uNandType = NAND_Normal8bit;
	NAND_Inform[0].uAddrCycle = 4;
	NAND_Inform[0].uBlockNum = 4096;
	NAND_Inform[0].uPageNum = 32;
	NAND_Inform[0].uPageSize = NAND_PAGE_512;
	NAND_Inform[0].uSpareSize = NAND_SPARE_16;
	NAND_Inform[0].uECCtest = 0;
	NAND_Inform[0].uSpareECCtest = 0;		// This line should be added for spare area ecc in S3C6410. If not, nand ecc would be fail right after reading first 512bytes. 080313. derrick.
	NAND_Inform[0].uTacls = 0;
	NAND_Inform[0].uTwrph0 = 25+10;	//Pad delay : about 10ns
	NAND_Inform[0].uTwrph1 = 15+5;	//tWH : 15ns

	NAND_Init(0);

	UART_Printf("Copying Nand BL(size:%dbytes) to DRAM(0x%x)......",maxlengh,MODEM_DRAM_BUFFER_ADDRESS);
	eNandErr = NAND_ReadMultiPage(0, 10, 0, (u8 *)MODEM_DRAM_BUFFER_ADDRESS, maxlengh);

	if ( eNandErr != eNAND_NoError ) 
	{
		UART_Printf("Nand Read Error(error code:0x%x)!!\n", eNandErr);
		return;
	}
	else
	{
		UART_Printf("Done!!\n");
	}
	
	UART_Printf("Writing Nand BL to AP's Stepping Stone......");

	for (lengh = 0 ; lengh <= maxlengh ; lengh+=4)
	{
		data = Inp32(MODEM_DRAM_BUFFER_ADDRESS + lengh);
		HOSTIF_SingleWrite(AP_BL_ADDRESS+lengh, data);
	}

	UART_Printf("Done!!\n");

	UART_Printf("Copying OS Image(size:%dbytes) to DRAM(0x%x)......",uMaxOSLength,MODEM_DRAM_BUFFER_ADDRESS);
	eNandErr = NAND_ReadMultiPage(0, 11, 0, (u8 *)MODEM_DRAM_BUFFER_ADDRESS, uMaxOSLength);
	if ( eNandErr != eNAND_NoError ) 
	{
		UART_Printf("Nand Read Error(error code:0x%x)!!\n", eNandErr);
		return;
	}
	else
	{
		UART_Printf("Done!!\n");
	}

	UART_Printf("Writing OS Image to AP's DRAM Area(0x%x)......", AP_OS_ADDRESS);
	
#if 0	// Single Write
	for (lengh = 0 ; lengh <= uMaxOSLength ; lengh+=4)
	{
		data = Inp32(MODEM_DRAM_BUFFER_ADDRESS + lengh);
		HOSTIF_SingleWrite(AP_OS_ADDRESS+lengh, data);
	}
#elif 0	// Burst Write
	HOSTIF_BurstWrite(AP_OS_ADDRESS, (u32 *)MODEM_DRAM_BUFFER_ADDRESS, uMaxOSLength/4);
#elif 1	// Repeated Burst Write
	HOSTIF_RepeatedBurstWrite(AP_OS_ADDRESS, (u32 *) MODEM_DRAM_BUFFER_ADDRESS, uMaxOSLength/4);
#endif
	UART_Printf("Done!!\n");
	
#if 1
	data = 0x53000000;
	for (lengh = 0 ; lengh <= uMaxOSLength ; lengh+=4)
		*(unsigned int*)(data+lengh) = HOSTIF_SingleRead(0x50100000+lengh);
#endif

	UART_Printf("Press any key and BOOT CODE(0x0c000000) start!!! \n");
	Getc();

	HOSTIF_MODEM_To_AP_Reset();
	
}

void Test_HOSTIF_Performance(void)
{
	u16 temp;
	u32 lengh;

	HOSTIF_MODEM_Init();

	// SingleWrite	
	StartTimer(0);
	for (lengh = 0 ; lengh < 0x1000 ; lengh+=4)
		HOSTIF_SingleWrite(0x52000000+lengh, lengh);
	temp = StopTimer(0);
	UART_Printf("4Kb SingleWrite time = %dus\n",temp);
	Getc();

	//SingleRead
	StartTimer(0);
	for (lengh = 0 ; lengh < 0x1000 ; lengh+=4)
		HOSTIF_SingleRead(0x52000000+lengh);
	temp = StopTimer(0);
	UART_Printf("4Kb SingleRead time = %dus\n",temp);
	Getc();
	
	// BurstWrite
	StartTimer(0);
	HOSTIF_BurstWrite(0x52000000, (u32 *)0x74100000, 1024);
	temp = StopTimer(0);
	UART_Printf("4Kb BurstWrite time = %dus\n",temp);
	Getc();
	
	//BurstRead
	StartTimer(0);
	HOSTIF_BurstRead(0x74100000, (u16 *)0x52000000, 1024);	
	temp = StopTimer(0);
	UART_Printf("4Kb BurstRead time = %dus\n",temp);
	Getc();


	//RepeatedBurstWrite
	StartTimer(0);
	HOSTIF_RepeatedBurstWrite(0x52000000, (u32 *)0x74100000, 1024);
	temp = StopTimer(0);
	UART_Printf("4Kb RepeatedBurstWrite time = %dus\n",temp);
	Getc();
	
	UART_Printf("\nHOST I/F Performance test end\n");

}

void Test_HSTIF_Debug(void)
{
	HOSTIF_TestProtoReg();
}	

void HOSTIF_Test(void)
{
	int i, sel;

	//FUNC_MENU menu[]=
	const testFuncMenu menu[]=
	{
		0,                  		                	"Exit",
		Test_HOSTIF_MODEM_Int,	"HOST I/F MODEM Interrupt Init	",
		Test_HOSTIF_AP_Int,		"HOST I/F AP Interrupt Init		",

		Test_Single_Read,			"Single Read		",
		Test_Single_ReadWrite,		"Single Write/Read		",
		Test_Burst_Read,			"Burst Read		",
		Test_Burst_ReadWrite,		"Burst Write/Read		",
		Test_RepeatedBurst_Write,	"Repeated Burst Write	",

		Test_Send_InMailBox,		"Send In-MailBox (Modem Side)	",
		Test_Receive_InMailBox,		"Receive In-MailBox (AP Side)	",
		Test_Send_OutMailBox,		"Send Out-MailBox	(AP Side) 	",
		Test_Receive_OutMailBox,	"Receive Out-MailBox (Modem Side)	",

		Test_MODEM_Booting,		"MODEM Booting Test	",
		Test_MODEM_Booting_OS,	"MODEM Booting for OS Test	",	// Not yet implemented... 030813 derrick
		Test_HOSTIF_Performance,	"HOST I/F Performance		",

		Test_HSTIF_Debug,			"HOST I/F Debugging		",
		0,0
	};

	while(1)
	{
		UART_Printf("\n");
		for (i=0; (int)(menu[i].desc)!=0; i++) {
			UART_Printf("%2d: %s\n", i, menu[i].desc);
		}

		UART_Printf("\nSelect the function to test : ");
		sel = UART_GetIntNum();
		UART_Printf("\n");

		if (sel == 0)
			break;
		else if (sel>0 && sel<(sizeof(menu)/8-1))
			(menu[sel].func) ();
	}
}


