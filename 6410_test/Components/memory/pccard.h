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
*	File Name : pccard_6400.h
*  
*	File Description : This file defines the register access function
*						and declares prototypes of adcts funcions
*
*	Author : Sunil,Roe
*	Dept. : AP Development Team
*	Created Date : 2007/1/17
*	Version : 0.1 
* 
*	History
*	- Created(Sunil,Roe 2007/1/17)
*  
**************************************************************************************/
#ifndef __PCCARD6400_H__
#define __PCCARD6400_H__
 
  
#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
//#include "etc_6400.h"
#include "cf.h"

#define	ENABLE					1
#define	DISABLE					0

#define	MASK					1
#define	UNMASK					0

#define	RESET					1
#define	NORESET				0

#define	PCCARD_CONNUM		1

#define 	PCCARD_CON0			0
//#define	PCCARD_CON1			1

typedef enum PCCARD_Id
{
	ePCCARD_CNFG_STATUS		= 0,
	ePCCARD_INTMSK_SRC		= 1,
	ePCCARD_ATTR				= 2,
	ePCCARD_IO					= 3,
	ePCCARD_COMM				= 4,
}
ePCCARD_Id;

typedef enum PCCARD_ATT_Id
{
	ePCCARD_CONFIG_OPTION	= 0x200,
	ePCCARD_CONFIG_STATUS	= 0x202,
	ePCCARD_CONFIG_REPLACE	= 0x204,
	ePCCARD_CONFIG_SOCKET	= 0x206,
}
ePCCARD_ATT_Id;

typedef enum PCCARD_MODE
{
	ePCCARD_COMMON_MEMORY	= 0,		
	ePCCARD_PRIMARY_IO		= 2,
	ePCCARD_SECONDARY_IO		= 3,
	ePCCARD_CONTIGUOUS_IO	= 1,
}
ePCCARD_MODE;

typedef enum PCCARD_WIDTH
{
	ePCCARD_WIDTH_8BIT		= 0,
	ePCCARD_WIDTH_16BIT		= 1,
}
ePCCARD_WIDTH;

typedef struct PCCARD_Inform
{
	u32		PCCARD_DATA;
	u32		PCCARD_FEATURE;
	u32		PCCARD_SECTOR;
	u32		PCCARD_LOWLBA;
	u32		PCCARD_MIDLBA;
	u32		PCCARD_HIGHLBA;
	u32		PCCARD_DEVICE;
	u32		PCCARD_STATUS;
	u32		PCCARD_COMMAND;
	u32*	puPCCARDWriteBuf;
	u32*	puPCCARDReadBuf;
}
PCCARD_oInform;

/*---------------------------------- Init Functions  ---------------------------------*/
//=========================================================================
//					          	          PCCARD Controller AREA
//=========================================================================
/*---------------------------------- Init Functions  ---------------------------------*/
u8	PCCARD_Init(u8);
void PCCARD_InitBuffer(u8 );
void PCCARD_InitPort(u8);
void PCCARD_ReturnPort(u8);
/*---------------------------------- APIs of PCCARD_CNFG&STATUS Registers ---------------------------------*/
void PCCARD_SetConfig(u8, ePCCARD_WIDTH);
/*---------------------------------- APIs of PCCARD_INTMSK&SRC Registers ---------------------------------*/
void PCCARD_ResetCard(u8 , u8 );
void PCCARD_SetINTMask(u8, u8);
/*---------------------------------- APIs of PCCARD_ATTR Registers ---------------------------------*/

/*---------------------------------- APIs of PCCARD_IO Registers ---------------------------------*/

/*---------------------------------- APIs of PCCARD_COMM Registers ---------------------------------*/
void PCCARD_SetCardAccessTiming(u8 , u8 , u8 , u8 );
/*---------------------------------- APIs of general PCCARD ---------------------------------*/
u8	PCCARD_Init(u8 );
void PCCARD_SetSFRBaseAddr(u8 , u32 );
u32 PCCARD_GetRegAddr(u8 , ePCCARD_Id );
u32 PCCARD_GetRegValue(u8 , ePCCARD_Id );
void PCCARD_SetRegValue(u8 , ePCCARD_Id , u32 );
//=========================================================================
//					          	          TASK FILE REGISTER AREA
//=========================================================================
void PCCARD_ChangePCCARDMode(u8 , ePCCARD_MODE );
void PCCARD_SetTaskFileBaseAddr(u8 , u32 );
u8	PCCARD_GetTaskFileRegValue(u8 , eCF_TASKFILE_Id );
u16	PCCARD_GetTaskFileRegValue16(u8, eCF_TASKFILE_Id);
void PCCARD_SetTaskFileRegValue(u8 , eCF_TASKFILE_Id , u32 );
void PCCARD_SetTaskFileRegValue16(u8 , eCF_TASKFILE_Id , u16 );
void PCCARD_SetDevicePosition(u8 ucCon, u32 uLba, u32 uSectorCount);
void PCCARD_ReadBlocks(u8, u32, u32, u32);
void PCCARD_WriteBlocks(u8, u32, u32, u32);
//=========================================================================
//					          	          ATTRIBUTE MEMORY AREA
//=========================================================================
void PCCARD_SetAttConfigBaseAddr(u8 , u32);
void PCCARD_SetCardMode(u8 , u8 , ePCCARD_MODE );
u8 PCCARD_GetAttRegValue(u8 , u8);
void PCCARD_SetAttRegValue(u8 , u8, u8 );
u8 PCCARD_GetAttribData(u8 , u8 );
void PCCARD_SetAttribData(u8 , u8 , u8 );

/*
//protected:	
	void PCCARD_ReadPccardRegister(unsigned int nRegister, unsigned char *data);
	void PCCARD_WritePccardRegister(unsigned int nRegister, unsigned char nValue);
	void PCCARD_GetDataFromPccardDevice(unsigned short *uData);
	void PCCARD_PutDataToPccardDevice(unsigned short nData);
//public:	
	u8 PCCARD_ChangePccardMode(unsigned int mode);
	u8 PCCARD_OpenPccardMediaWithMode(unsigned int mode);
	u8 PCCARD_ClosePccardMedia(void);

	u8 PCCARD_ReadBlocks(unsigned int uStBlock, unsigned int uBlocks, unsigned int uBufAddr);
	u8 PCCARD_WriteBlocks(unsigned int uStBlock, unsigned int uBlocks, unsigned int uBufAddr);
*/

#ifdef __cplusplus
}
#endif

#endif
