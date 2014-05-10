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
*	File Name : dmc.h
*  
*	File Description : This file implements the API functons for DRAM controller.
*
*	Author : Wonjoon.jang
*	Dept. : AP Development Team
*	Created Date : 2007/01/05
*	Version : 0.1 
* 
*	History
*	- Created(Wonjoon.jang 2007/01/05)
*  
**************************************************************************************/
#ifndef __DMC_H__
#define __DMC_H__


#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

#include "def.h"



typedef enum 
{
	eDDR_REF	=	7800,	// 7800ns
	eDDR_RAS	=	45,		// min. 45ns
	eDDR_RC	=	68,		// min. 67.5ns
	eDDR_RCD	=	23,		// min. 22.5ns
	eDDR_RFC	=	80,		// min. 80ns
	eDDR_RP	=	23,		// min. 22.5ns
	eDDR_RRD	=	15,		// min. 15ns
	eDDR_WR	=	15,		// min. 15ns
	eDDR_XSR	=	120,		// min 120ns
	eAP_bit		=  	0,
	eRow_bit		=	2,		// 13bit
	eCol_bit		=	2		// 10bit
}K4X51163_ePara;

typedef enum 
{
	eALLEN		=  	0,
	eStopEN		=	1		// Mem Clock is dynamically stopped
}DMC_eStopCLK;

typedef enum 
{
	eDIS_AutoPD	=  	0,
	eEN_AutoPD		=	1		
}DMC_eAutoPD;



void	DMC_Init(u8 uController, DMC_eStopCLK eStopClk, DMC_eAutoPD eAutoPD, u32 uPDprd );
void DMC_RdSTATUS (u8 uController);


#ifdef __cplusplus
}
#endif

#endif //__DMC_H__



