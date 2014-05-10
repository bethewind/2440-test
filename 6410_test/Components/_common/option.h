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
*	File Name : option.h
*  
*	File Description : This file defines basic setting and configuration.
*
*	Author : Haksoo,Kim
*	Dept. : AP Development Team
*	Created Date : 2006/11/08
*	Version : 0.1 
* 
*	History
*	- Created(Haksoo,Kim 2006/11/08)
*  
**************************************************************************************/

#ifndef __OPTION_H__
#define __OPTION_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "def.h"

// semi-hosting
#define	SEMIHOSTING					(false)

#define	FIN							12000000

#define	DMC0						(FALSE)
#define	DMC1						(!(DMC0))
#if	(DMC0)
#define	_DRAM_BaseAddress			0x40000000
#elif (DMC1)
#define	_DRAM_BaseAddress			0x50000000
#endif

#define	_DRAM0_BaseAddress			0x40000000

#define	_SMC_BaseAddress			0x10000000

#define	_Exception_Vector			(_DRAM_BaseAddress+0x07ffff00)
#define	_MMUTT_BaseAddress			(_DRAM_BaseAddress+0x07ff8000)
#define	DefaultDownloadAddress		(_DRAM_BaseAddress+0x00200000)

//rb1004
#define 	CODEC_MEM_ST			(_DRAM_BaseAddress+0x01000000)
#define 	CODEC_MEM_LMT  			(_DRAM_BaseAddress+0x02000000)	

// Derrick
#define	SMDK_MODE				0
#define	ASB_MODE				1
#define	TEST_CODE_MODE		ASB_MODE

#define	ASB_ONBL_ADDRESS		(_DRAM_BaseAddress+0x01000000)
#define	ASB_BL2_ADDRESS		(_DRAM_BaseAddress+0x01000800)
#define	ASB_FW_ADDRESS		(_DRAM_BaseAddress+0x01040000)
#define	ASB_MFC1_ADDRESS		(_DRAM_BaseAddress+0x01A00000)
#define	ASB_MFC2_ADDRESS		(_DRAM_BaseAddress+0x01A20000)
#define	ASB_MFC3_ADDRESS		(_DRAM_BaseAddress+0x01D20000)
#define	ASB_BIN1_ADDRESS		(_DRAM_BaseAddress+0x02120000)
#define	ASB_BIN2_ADDRESS		(_DRAM_BaseAddress+0x05000000)
#ifdef __cplusplus
}
#endif

#endif /*__OPTION_H__*/

