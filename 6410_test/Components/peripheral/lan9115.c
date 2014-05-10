
/**************************************************************************************
* 
*	Project Name : S3C6410 Validation
*
*	Copyright 2006 by Samsung Electronics, Inc.
*	All rights reserved.
*
*	Project Description :
*		This software is only for validating functions of the S3C6410.
*		Anybody can use this software without our permission.
*  
*--------------------------------------------------------------------------------------
* 
*	File Name : lan9115.c
*  
*	File Description : This file implements the test functons for LAN9115 Ethernet Chip.
*
*	Author : Heemyung.noh
*	Dept. : AP Development Team
*	Created Date : 2008/03/25
*	Version : 0.1 
* 
*	History
*	- Created(Heemyung.noh 2008/03/25)
*  
**************************************************************************************/

#include "def.h"
#include "option.h"
#include "system.h"
#include "lan9115.h"
#include "sromc.h"
#include "library.h"
#include "timer.h"

#define BANK0_BASE_ADDRESS 	0x10000000 //Bank0 base address
#define MAC_CSR_CMD_Write(uIndex, uData)	(*(u32 *)(Lan9115_Base + uIndex) = uData);	

u32 Lan9115_Base;
u32 Lan9115_Bank;

u32 LAN9115_reg_read(u32 uAddr)
{
	return Inp32(Lan9115_Base + uAddr);
}	

void LAN9115_reg_write(u32 uAddr, u32 uData)
{
	Outp32(Lan9115_Base + uAddr, uData);
}

void LAN9115_MAC_CSR_write(u32 uIndex, u32 uData)
{
//	u32 reg_val;

	Outp32(Lan9115_Base + MAC_CSR_DATA, uData);
	Outp32(Lan9115_Base + MAC_CSR_CMD, ((u32)(1<<31)|(uIndex&0xFF)));
	
	while(Inp32(Lan9115_Base + MAC_CSR_CMD) & (u32)(1<<31));
}

u32 LAN9115_MAC_CSR_read(u32 uIndex)
{
	Outp32(Lan9115_Base + MAC_CSR_CMD, (1<<31)|(1<<30) | (uIndex&0xFF));

	while(Inp32(Lan9115_Base + MAC_CSR_CMD) & (u32)(1<<31));
	
	return Inp32(Lan9115_Base + MAC_CSR_DATA);
}

u32 LAN9115_EEPROM_Cmd(u32 uCommand, u32 uAddr, u8 uData)
{
	u32 uRead_Data = 0;

	Outp32(Lan9115_Base + E2P_CMD, 0);
	
	switch(uCommand)
	{
		case E2P_READ 	:	Outp32(Lan9115_Base + E2P_CMD, ((u32)(1<<31)|uCommand|(uAddr&0xFF)));
							while(Inp32(Lan9115_Base + E2P_CMD) & (u32)(1<<31));
							uRead_Data = Inp32(Lan9115_Base + E2P_DATA);
							break;
		case E2P_RELOAD	:	
		case E2P_EWDS	:	
		case E2P_EWEN	:
		case E2P_ERASE	:
		case E2P_ERAL	:	Outp32(Lan9115_Base + E2P_CMD, ((u32)(1<<31)|uCommand|(uAddr&0xFF)));
							while(Inp32(Lan9115_Base + E2P_CMD) & (u32)(1<<31));
							break;
		case E2P_WRITE	:
		case E2P_WRAL	: 	Outp32(Lan9115_Base + E2P_DATA, uData);
							Outp32(Lan9115_Base + E2P_CMD, ((u32)(1<<31)|uCommand|(uAddr&0xFF)));
							while(Inp32(Lan9115_Base + E2P_CMD) & (u32)(1<<31));
							break;
		default			: 	break;
	}

	if(Inp32(Lan9115_Base + E2P_CMD) & (1<<9))
	{
		UART_Printf("EPC Time-Out was occured[Addr:0x%02x]\n",uAddr);
		Outp32(Lan9115_Base + E2P_CMD, Inp32(Lan9115_Base + E2P_CMD) | (1<<9));
	}
	return uRead_Data;
}


void LAN9115_SW_Reset(void)
{
	UART_Printf("[LAN9115_SW_Reset]\n");
	
	Outp32(Lan9115_Base + HW_CFG, (1<<0));

	while(Inp32(Lan9115_Base + HW_CFG) & (1<<0));
}

u32 LAN9115_SMC_Setting(void)
{
	u32 iBankSel, iTacs, iTcos, iTacc, iTacp, iTcoh, iTcah;
	
	UART_Printf("[LAN9115_SMC_Setting]\n");


	UART_Printf("1)nCS1	2)nCS2	3)nCS3	4)nCS4	5)nCS5\n");
	UART_Printf("Select SROM Bank(\"0\" to exit) : \n");
	iBankSel = UART_GetIntNum();
	if(iBankSel==0 || iBankSel > 5) 
	    return;		// return.
	if(iBankSel == 2)
		Outp32(0x7e00f120, Inp32(0x7e00f120)|(1<<1));
	else if(iBankSel == 3)
		Outp32(0x7e00f120, Inp32(0x7e00f120)|(1<<3));
	
	UART_Printf("Select Tacs(0~15, default 0) : \n");
	iTacs = UART_GetIntNum();
	if (iTacs == -1) 
		iTacs = eCLK0;
	UART_Printf("Select Tcos(0~15, default 0) : \n");
	iTcos = UART_GetIntNum();
	if (iTcos == -1) 
		iTcos = eCLK0;
	UART_Printf("Select Tacc(0~31, default 5) : \n");
	iTacc = UART_GetIntNum();
	if (iTacc == -1) 
		iTacc = eCLK5;	
	UART_Printf("Select Tacp(0~15, default 0) : \n");
	iTacp = UART_GetIntNum();
	if (iTacp == -1) 
		iTacp = eCLK0;	
	UART_Printf("Select Tcoh(0~15, default 2) : \n");
	iTcoh = UART_GetIntNum();
	if (iTcoh == -1) 
		iTcoh = eCLK2;		
	UART_Printf("Select Tcah(0~15, default 0) : \n");
	iTcah = UART_GetIntNum();
	if (iTcah == -1) 
		iTcah = eCLK0;		
	
	SROMC_SetBank(iBankSel, eEn_CTL, eDis_WAIT, e16bit, eNor_Mode, 
					(Bank_eTiming)iTacs, (Bank_eTiming)iTcos, (Bank_eTiming)iTacc, (Bank_eTiming)iTcoh, (Bank_eTiming)iTcah, (Bank_eTiming)iTacp);

	return iBankSel;

}


void LAN9115_Detect(void)
{
	u32 reg = 0;
	
	UART_Printf("[LAN9115_Detect]\n");

	reg = LAN9115_reg_read(ID_REV);

	if (((reg>>16)&0xffff) != ChipID)
	{
		UART_Printf("ID Error = 0x%x\n", ((reg>>16)&0xffff));
		UART_Printf("Press Any Key...!\n");
		UART_Getc();
		return;
	}
	
	UART_Printf("Device ID = 0x%x\n", (reg>>16)&0xffff);
	UART_Printf("Revision number = 0x%x\n", reg&0xffff);
	UART_Printf("Lan9115 is Detected..\n");
	UART_Printf("\n");
}

void LAN9115_MAC_CSR_Synchronizer(void)
{
	u32 reg = 0;
	oLAN9115_CSR oLAN_MAC_CSR;
	
	UART_Printf("[MAC_CSR_Synchronizer]\n");

	//LAN9115_SW_Reset();
		
	oLAN_MAC_CSR.MAC_CR = LAN9115_MAC_CSR_read(MAC_CR_IDX);
	oLAN_MAC_CSR.ADDRH = LAN9115_MAC_CSR_read(ADDRH_IDX);
	oLAN_MAC_CSR.ADDRL = LAN9115_MAC_CSR_read(ADDRL_IDX);
	oLAN_MAC_CSR.HASHH = LAN9115_MAC_CSR_read(HASHH_IDX);
	oLAN_MAC_CSR.HASHL = LAN9115_MAC_CSR_read(HASHL_IDX);
	oLAN_MAC_CSR.MII_ACC = LAN9115_MAC_CSR_read(MII_ACC_IDX);
	oLAN_MAC_CSR.MII_DATA = LAN9115_MAC_CSR_read(MII_DATA_IDX);
	oLAN_MAC_CSR.FLOW = LAN9115_MAC_CSR_read(FLOW_IDX);
	oLAN_MAC_CSR.VLAN1 = LAN9115_MAC_CSR_read(VLAN1_IDX);
	oLAN_MAC_CSR.VLAN2 = LAN9115_MAC_CSR_read(VLAN2_IDX);
	oLAN_MAC_CSR.WUFF = LAN9115_MAC_CSR_read(WUFF_IDX);
	oLAN_MAC_CSR.WUCSR = LAN9115_MAC_CSR_read(WUCSR_IDX);

	UART_Printf("MAC_CR : 0x%08x\n", oLAN_MAC_CSR.MAC_CR);
	UART_Printf("ADDRH : 0x%08x\n", oLAN_MAC_CSR.ADDRH);
	UART_Printf("ADDRL : 0x%08x\n", oLAN_MAC_CSR.ADDRL);
	UART_Printf("HASHH : 0x%08x\n", oLAN_MAC_CSR.HASHH);
	UART_Printf("HASHL : 0x%08x\n", oLAN_MAC_CSR.HASHL);
	UART_Printf("MII_ACC : 0x%08x\n", oLAN_MAC_CSR.MII_ACC);
	UART_Printf("MII_DATA : 0x%08x\n", oLAN_MAC_CSR.MII_DATA);
	UART_Printf("FLOW : 0x%08x\n", oLAN_MAC_CSR.FLOW);
	UART_Printf("VLAN1 : 0x%08x\n", oLAN_MAC_CSR.VLAN1);
	UART_Printf("VLAN2 : 0x%08x\n", oLAN_MAC_CSR.VLAN2);
	UART_Printf("WUFF : 0x%08x\n", oLAN_MAC_CSR.WUFF);
	UART_Printf("WUCSR : 0x%08x\n", oLAN_MAC_CSR.WUCSR);
	UART_Printf("\n");

#if 0
	// MAC Address Change
	UART_Printf("MAC Address Change Test\n");
	LAN9115_MAC_CSR_write(ADDRH_IDX, 0xabcd);
	LAN9115_MAC_CSR_write(ADDRL_IDX, 0x12345678);
	oLAN_MAC_CSR.ADDRH = LAN9115_MAC_CSR_read(ADDRH_IDX);
	oLAN_MAC_CSR.ADDRL = LAN9115_MAC_CSR_read(ADDRL_IDX);	
	UART_Printf("ADDRH : 0x%08x\n", oLAN_MAC_CSR.ADDRH);
	UART_Printf("ADDRL : 0x%08x\n", oLAN_MAC_CSR.ADDRL);
#endif	
	UART_Printf("\n");
	
}

void LAN9115_EEPROM_Test(void)
{
	u32 i, uE2P_Data;
	
	UART_Printf("[LAN9115_EEPROM_Test]\n");

	LAN9115_EEPROM_Cmd(E2P_EWEN, 0, 0);
	LAN9115_EEPROM_Cmd(E2P_ERAL, 0, 0);
	
	UART_Printf("EEPROM data read\n");
	for(i=0 ; i<128 ; i++)
	{
		uE2P_Data = LAN9115_EEPROM_Cmd(E2P_READ, i, 0);
		UART_Printf("[Address : Data] = [0x%02x : 0x%02x]\n", i, uE2P_Data);
	}

	UART_Printf("EEPROM data write\n");
	LAN9115_EEPROM_Cmd(E2P_WRITE, 0, E2P_FIRST_BYTE);
	LAN9115_EEPROM_Cmd(E2P_WRITE, 1, (E2P_MAC_ADDRH&0xFF00)>>8);
	LAN9115_EEPROM_Cmd(E2P_WRITE, 2, (E2P_MAC_ADDRH&0xFF));
	LAN9115_EEPROM_Cmd(E2P_WRITE, 3, (E2P_MAC_ADDRL&0xFF000000)>>24);
	LAN9115_EEPROM_Cmd(E2P_WRITE, 4, (E2P_MAC_ADDRL&0xFF0000)>>16);
	LAN9115_EEPROM_Cmd(E2P_WRITE, 5, (E2P_MAC_ADDRL&0xFF00)>>8);
	LAN9115_EEPROM_Cmd(E2P_WRITE, 6, (E2P_MAC_ADDRL&0xFF));
	
	for(i=7 ; i<128 ; i++)
	{
		LAN9115_EEPROM_Cmd(E2P_WRITE, i, (u8)i);
	}

	UART_Printf("EEPROM data read\n");
	for(i=0 ; i<128 ; i++)
	{
		uE2P_Data = LAN9115_EEPROM_Cmd(E2P_READ, i, 0);
		UART_Printf("[Address : Data] = [0x%02x : 0x%02x]\n", i, uE2P_Data);
	}	

}



void * func_lan9115_test[][2]=
{	
	LAN9115_Detect,						"Detect LAN9115       ",
	LAN9115_SW_Reset,					"SW reset         ",
	LAN9115_MAC_CSR_Synchronizer,		"MAC CSR Synchronizer  ",
	LAN9115_EEPROM_Test,					"EEPROM Test    ",
	0,0
};

void Test_LAN9115(void)
{
    	int i;

	Lan9115_Bank = LAN9115_SMC_Setting();
	Lan9115_Base = BANK0_BASE_ADDRESS+0x08000000*(Lan9115_Bank);

	while(1)
	{
		i=0;
		while(1)
		{   //display menu
			UART_Printf("%2d:%s",i,func_lan9115_test[i][1]);
			i++;
			if((int)(func_lan9115_test[i][0])==0)
			{
				UART_Printf("\n");
				break;
			}
			if((i%3)==0)
			UART_Printf("\n");
		}

		UART_Printf("\nSelect (Press enter key to exit) : ");
		i = UART_GetIntNum();
		
		if(i==-1) 
			break;		// return.
			
		if(i>=0 && (i<((sizeof(func_lan9115_test)-1)/8)) )	// select and execute...
			( (void (*)(void)) (func_lan9115_test[i][0]) )();
	}

	Outp32(0x7e00f120, Inp32(0x7e00f120)&(~((1<<1)|(1<<3))));
	
	UART_Printf("CS8900_Test program end.\n");
	UART_Printf("\n");
}

