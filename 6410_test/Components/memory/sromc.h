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
*	File Name : dmc.h
*  
*	File Description : This file implements the API functons for Static Memory controller.
*
*	Author : Wonjoon.jang
*	Dept. : AP Development Team
*	Created Date : 2007/01/06
*	Version : 0.1 
* 
*	History
*	- Created(Wonjoon.jang 2007/01/06)
*  
**************************************************************************************/
#ifndef __SROMC_H__
#define __SROMC_H__


#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

#include "def.h"
#include "dma.h"

#define		MEMIF_NUM		5

typedef struct EBI_Inform
{
//	u8			ucRecDone;	
	u8			ucIntNum;
	
	DMA_UNIT	eDMACon;	
	DMA_CH		eDMACh;
	u32			uLLIBaseAddr;		
	u32			uSrcAddr;
	u32			uDstAddr;
	u8			bSrcFixed;
	u8			bDstFixed;
	DATA_SIZE	eDataSz;
	u32			uDataCnt;
	DREQ_SRC	eSrcReq;
	DREQ_SRC	eDstReq;
	BURST_MODE	eBurstMode;
	DMAC		oEBIDma;

	void 		(* pHandler)(void) __irq;
}
EBI_oInform;

typedef enum Mem_Type
{
	eMEM_DMC0		=	0,
	eMEM_NAND		=	1,	
	eMEM_SROMC	=	2,
	eMEM_OneNAND	=	3,	
	eMEM_CF		=	4,	
}Mem_eType;

#if 0
typedef enum Bank_Timing
{
	eCLK0	=	0,	
	eCLK1	=	0,		// Tacc Only
	eCLK2	=	1,	
	eCLK3	= 	2,
	eCLK4	= 	3,
	eCLK5	= 	4,
	eCLK6	= 	5,
	eCLK7	= 	6,
	eCLK8	= 	7,
	eCLK9	= 	8,
	eCLK10	= 	9,
	eCLK11	= 	10,
	eCLK12	= 	11,
	eCLK13	= 	12,
	eCLK14	= 	13,
	eCLK15	= 	14,
	eCLK16	= 	15,
	eCLK17	= 	16,
	eCLK18	= 	17,
	eCLK19	= 	18,
	eCLK20	= 	19,
	eCLK21	= 	20,
	eCLK22	= 	21,
	eCLK23	= 	22,
	eCLK24	= 	23,
	eCLK25	= 	24,
	eCLK26	= 	25,
	eCLK27	= 	26,
	eCLK28	= 	27,
	eCLK29	= 	28,
	eCLK30	= 	29,
	eCLK31	= 	30,
	eCLK32	=	31
}Bank_eTiming;
#else
typedef enum Bank_Timing
{
	eCLK0	=	0,	
	eCLK1	=	1,		// Tacc Only
	eCLK2	=	2,	
	eCLK3	= 	3,
	eCLK4	= 	4,
	eCLK5	= 	5,
	eCLK6	= 	6,
	eCLK7	= 	7,
	eCLK8	= 	8,
	eCLK9	= 	9,
	eCLK10	= 	10,
	eCLK11	= 	11,
	eCLK12	= 	12,
	eCLK13	= 	13,
	eCLK14	= 	14,
	eCLK15	= 	15,
	eCLK16	= 	16,
	eCLK17	= 	17,
	eCLK18	= 	18,
	eCLK19	= 	19,
	eCLK20	= 	20,
	eCLK21	= 	21,
	eCLK22	= 	22,
	eCLK23	= 	23,
	eCLK24	= 	24,
	eCLK25	= 	25,
	eCLK26	= 	26,
	eCLK27	= 	27,
	eCLK28	= 	28,
	eCLK29	= 	29,
	eCLK30	= 	30,
	eCLK31	= 	31,
	eCLK32	=	32
}Bank_eTiming;

#endif

typedef enum Page_Mode
{
	eNor_Mode	=	0,	
	eD4_Mode	=	1,		
}Page_eMode;

typedef enum Byte_CTL
{
	eDis_CTL	=	0,	
	eEn_CTL		=	1
}Byte_eCTL;

typedef enum WAIT_CTL
{
	eDis_WAIT		=	0,	
	eEn_WAIT		=	1
}WAIT_eCTL;

typedef enum Data_Width
{
	e8bit		=	0,	
	e16bit		=	1
}Data_eWidth;

typedef enum SROMC_BANK
{
	eBank0		= 0,
	eBank1		= 1,
	eBank2		= 2,
	eBank3		= 3,
	eBank4		= 4,
	eBank5		= 5,	
	eBankMAX	= 5
}SROMC_eBANK;

extern u32 bank_of_nor;
extern volatile unsigned int downloadFileSize;


void	SROMC_Init(void);
void	SROMC_SetBusWidth(SROMC_eBANK uBank, Data_eWidth eDWidth);
void	SROMC_SetWaitControl(SROMC_eBANK uBank, WAIT_eCTL eWAITCTL);
void	SROMC_SetByteEnable(SROMC_eBANK uBank, Byte_eCTL eByteCTL);
void SROMC_SetTiming(	SROMC_eBANK uBank, Bank_eTiming eTacs, Bank_eTiming eTcos,Bank_eTiming eTacc, 
							 					Bank_eTiming eTcoh, Bank_eTiming eTcah, Bank_eTiming eTacp,
							 					Page_eMode ePageMode);

void	SROMC_SetBank(u8 , Byte_eCTL , WAIT_eCTL , Data_eWidth , Page_eMode,
	Bank_eTiming , Bank_eTiming ,Bank_eTiming , Bank_eTiming , Bank_eTiming , Bank_eTiming );

void EBI_SetDMAParams(Mem_eType );
void SMC_SetLLI(u32 , u32 , DATA_SIZE , BURST_MODE , u32 );
void EBI_CloseDMA(Mem_eType );

#ifdef __cplusplus
}
#endif

#endif //__SROMC_H__



