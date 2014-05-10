/**************************************************************************************
* 
*	Project Name : S3C6410 Validation
*
*	Copyright 2006~2008 by Samsung Electronics, Inc.
*	All rights reserved.
*
*	Project Description :
*		This software is only for validating functions of the S3C6410.
*		Anybody can use this software without our permission.
*  
*--------------------------------------------------------------------------------------
* 
*	File Name : iic.h
*  
*	File Description : This file implements the API functons for IIC
*
*	Author : Woojin.Kim
*	Dept. : AP Development Team
*	Created Date : 2007/01/11
*	Version : 0.1 
* 
*	History
*	- Created(Woojin.Kim 2007/01/11)
*	- Edited (OnPil, Shin (SOP) 2008/02/29 ~ 2008/03/04)
*  
**************************************************************************************/
 
#ifndef __IIC_H__
#define __IIC_H__

#include "def.h"
#include "sfr6410.h"
#include "register_addr.h"		// Added by SOP on 20008/03/01

enum IIC0_SFR			// Channel0, Added by SOP on 2008/03/01
{
	rIICCON0		= I2C0_BASE+0x00,
	rIICSTAT0		= I2C0_BASE+0x04,
	rIICADD0		= I2C0_BASE+0x08,
	rIICDS0			= I2C0_BASE+0x0C,
	rIICLC0			= I2C0_BASE+0x10
};

enum IIC1_SFR			// Channel1, Added by SOP on 2008/03/01
{
	rIICCON1		= I2C1_BASE+0x00,
	rIICSTAT1		= I2C1_BASE+0x04,
	rIICADD1		= I2C1_BASE+0x08,
	rIICDS1			= I2C1_BASE+0x0C,
	rIICLC1			= I2C1_BASE+0x10
};

#define	SlaveRX			(0)
#define	SlaveTX			(1)
#define	MasterRX		(2)
#define	MasterTX		(3)

// Channel 0
void __irq Isr_IIC(void);
void IIC_Open(u32 ufreq);
void IIC_OpenPolling(u32 ufreq);
void IIC_Close(void);
void IIC_SetWrite(u8 cSlaveAddr, u8 * pData, u32 uDataLen);
void IIC_SetRead(u8 cSlaveAddr, u8 * pData, u32 uDataLen);
u8 IIC_Status(void);

void IIC_MasterWrP(u8 cSlaveAddr,u8 * pData);
void IIC_SlaveRdP(u8 *pSlaveAddr,u8 *pData);
void IIC_MasterRdP(u8 cSlaveAddr,u8 * pData);
void IIC_SlaveWrP(u8 *pSlaveAddr,u8 *pData);
void IIC_SlaveWrInt(u8 *pSlaveAddr,u8 *pData);
void IIC_SlaveRdInt(u8 *pSlaveAddr,u8 *pData);

void IIC_Write(u8 cSlaveAddr,  u8 cAddr, u8 cData);
void IIC_Write16(u8 cSlaveAddr, u8 cAddr, u16 cData);
void IIC_Read(u8 cSlaveAddr,  u8 cAddr, u8 * cData);
void IIC_Wait(void);

// Channel 1	, Added by SOP on 2008/03/03
void __irq Isr_IIC1(void);
void IIC1_Open(u32 ufreq);		
void IIC1_OpenPolling(u32 ufreq);
void IIC1_Close(void);			
void IIC1_SetWrite(u8 cSlaveAddr, u8 * pData, u32 uDataLen);
void IIC1_SetRead(u8 cSlaveAddr, u8 * pData, u32 uDataLen);

void IIC1_MasterWrP(u8 cSlaveAddr,u8 * pData);
void IIC1_SlaveRdP(u8 *pSlaveAddr,u8 *pData);
void IIC1_MasterRdP(u8 cSlaveAddr,u8 * pData);
void IIC1_SlaveWrP(u8 *pSlaveAddr,u8 *pData);
void IIC1_SlaveWrInt(u8 *pSlaveAddr,u8 *pData);
void IIC1_SlaveRdInt(u8 *pSlaveAddr,u8 *pData);

void IIC1_Write(u8 cSlaveAddr,  u16 cAddr, u8 cData);
void IIC1_Read(u8 cSlaveAddr, u16 cAddr, u8 *cData);

void ClockInform(void);			// Added by SOP on 2008/03/27

#endif //__IIC_H__
