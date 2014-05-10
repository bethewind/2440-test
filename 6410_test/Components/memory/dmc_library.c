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
*	File Name : dmc_library.c
*  
*	File Description : This file implements the API functons for DRAM controller.
*
*	Author : Sekwang Kim
*	Dept. : AP Development Team
*	Created Date : 2007/01/05
*	Version : 0.1 
* 
*	History
*	- Created(boaz.kim 2008/03/17)*     
*   
**************************************************************************************/
//#include "def.h"
//#include "library.h"


#define	rMEMSTAT		0x7e001000		// 0x7E001000 (DMC1 Case)
#define	rMEMCCMD		0x7e001004		// 0x7E001004
#define	rDIRECTCMD		0x7e001008 	// 0x7E001008
#define	rMEMCFG		0x7e00100c		// 0x7E00100C
#define	rREFRESH		0x7e001010		// 0x7E001010
#define	rCASLAT			0x7e001014		// 0x7E001014
#define	rT_DQSS		0x7e001018		// 0x7E001018
#define	rT_MRD			0x7e00101c 		// 0x7E00101C
#define	rT_RAS			0x7e001020 		// 0x7E001020
#define	rT_RC			0x7e001024			// 0x7E001024
#define	rT_RCD			0x7e001028 		// 0x7E001028
#define	rT_RFC			0x7e00102c 		// 0x7E00102C
#define	rT_RP			0x7e001030			// 0x7E001030
#define	rT_RRD			0x7e001034 		// 0x7E001034
#define	rT_WR			0x7e001038			// 0x7E001038
#define	rT_WTR			0x7e00103c 		// 0x7E00103C
#define	rT_XP			0x7e001040			// 0x7E001040
#define	rT_XSR			0x7e001044 		// 0x7E001044
#define	rT_ESR			0x7e001048 		// 0x7E001048
#define Outp32DMC(addr, data)		(*(volatile unsigned int *)(addr) = (data))
//#define Outp32DMC(addr, data)		UART_Printf("addr:0x%x, data:0x%x\n", addr, data)


#define	C_REFRESH		78000
#define	C_RAS			450
#define	C_RC			675
#define	C_RCD			225
#define	C_RP			225
#define	C_RFC			800
#define	C_RRD			150
#define	C_WR			150
#define	C_XSR			120
#define	C_ESR			120



void DMC_ChangeStateFromLowPowerToReady(void)
{

	
	Outp32DMC(rMEMCCMD, 0x2);//Set Wakeup
	while(((*(volatile unsigned int *)rMEMSTAT)&0x3)!=2) ; //Waiting for Pause State
	
	
	Outp32DMC(rMEMCCMD, 0x0);//Set Ready
	while(((*(volatile unsigned int *)rMEMSTAT)&0x3)!=1) ; //Waiting for Pause State
	
}


void DMC_ChangeStateFromReadyToLowPower(void)
{

	Outp32DMC(rMEMCCMD, 0x3); //Set Pause
	while(((*(volatile unsigned int *)rMEMSTAT)&0x3)!=2) ; //Waiting for Pause State

	
	Outp32DMC(rMEMCCMD, 0x1); //Set Power Down
	while(((*(volatile unsigned int *)rMEMSTAT)&0x3)!=3) ; //Waiting for LowPower State	
}

void DMC_ChangeSDRAMParameter(unsigned int uMemType, unsigned int uMemClk)
{

	unsigned int uVal;

	DMC_ChangeStateFromReadyToLowPower();

	uVal=(C_REFRESH/10./(1000./uMemClk))+1;
	Outp32DMC(rREFRESH, uVal);
	uVal=(C_RAS/10./(1000./uMemClk))+1;
	Outp32DMC(rT_RAS, uVal);
	uVal=(C_RC/10./(1000./uMemClk))+1;
	Outp32DMC(rT_RC, uVal);
	uVal=(C_RCD/10./(1000./uMemClk))+1;
	Outp32DMC(rT_RCD, uVal);
	uVal=(C_RFC/10./(1000./uMemClk))+1;
	Outp32DMC(rT_RFC, uVal);
	uVal=(C_RP/10./(1000./uMemClk))+1;
	Outp32DMC(rT_RP, uVal);
	uVal=(C_RRD/10./(1000./uMemClk))+1;
	Outp32DMC(rT_RRD, uVal);
	uVal=(C_WR/10./(1000./uMemClk))+1;
	Outp32DMC(rT_WR, uVal);
	uVal=(C_XSR/10./(1000./uMemClk))+1;
	Outp32DMC(rT_XSR, uVal);
	uVal=(C_ESR/10./(1000./uMemClk))+1;
	Outp32DMC(rT_ESR, uVal);

	DMC_ChangeStateFromLowPowerToReady();
}

void DMC_PreChangeSDRAMParameter(void)
{

	unsigned uVal;
	DMC_ChangeStateFromReadyToLowPower();

	uVal=(C_REFRESH/10./(1000./20))+1;
	Outp32DMC(rREFRESH, uVal);
	uVal=(C_RAS/10./(1000./133))+1;
	Outp32DMC(rT_RAS, uVal);
	uVal=(C_RC/10./(1000./133))+1;
	Outp32DMC(rT_RC, uVal);
	uVal=(C_RCD/10./(1000./133))+1;
	Outp32DMC(rT_RCD, uVal);
	uVal=(C_RFC/10./(1000./133))+1;
	Outp32DMC(rT_RFC, uVal);
	uVal=(C_RP/10./(1000./133))+1;
	Outp32DMC(rT_RP, uVal);
	uVal=(C_RRD/10./(1000./133))+1;
	Outp32DMC(rT_RRD, uVal);
	uVal=(C_WR/10./(1000./133))+1;
	Outp32DMC(rT_WR, uVal);
	uVal=(C_XSR/10./(1000./133))+1;
	Outp32DMC(rT_XSR, uVal);
	uVal=(C_ESR/10./(1000./133))+1;
	Outp32DMC(rT_ESR, uVal);

	DMC_ChangeStateFromLowPowerToReady();

}

//void DMC_


