/******************************************************************************
*
* NAME         : fimg_sysutil.h
* TITLE        : FIMGSE ver. 1.0 Register Level API
* AUTHOR       : Thomas, Kim
* CREATED      : 12 Apr 2006
*
* COPYRIGHT    : Copyright(c) 2005-2006 by Samsung Electronics Limited. All
*                rights reserved. No part of this software, either material
*                or conceptual may be copied or distributed, transmitted,
*                transcribed, stored in a retrieval system or translated into
*                any human or computer language in any form by any means,
*                electronic, mechanical, manual or other-wise, or disclosed
*                to third parties without the express written permission of
*                Samsung Electronics. Semiconductor Business, System LSI
*                Division, Mobile Solution Development, Graphics IP Team
*                in Mobile Next Generation Technology.
*
* DESCRIPTION  : Provides debug functionality
*
* PLATFORM     : ALL
* HISTORY      : 2006. 4. 12 created
* CVS
*	$RCSfile: fimg_sysutil.h,v $
*	$Revision: 1.2 $
*	$Author: cheolkyoo.kim $
*	$Date: 2006/05/08 05:39:11 $
*	$Locker:  $
*
*	$Source: C:/CVS/CVSrepository/FIMG-3DSE_SW/fimg3dse_fpga/fimg3d/common/fimg_sysutil.h,v $
*	$State: Exp $
*	$Log: fimg_sysutil.h,v $
*	Revision 1.2  2006/05/08 05:39:11  cheolkyoo.kim
*	no message
*	
*	Revision 1.1  2006/04/13 10:53:26  cheolkyoo.kim
*	Initial import for register level API
*	
******************************************************************************/
#if !defined(__FIMG_SYSUTIL_H__)
#define __FIMG_SYSUTIL_H__

#include "config.h"

extern "C" {
#include "fgl.h"
}


#ifndef PI
    #define PI 3.14159f
#endif


struct SurfaceInfo
{
	unsigned int	width;
	unsigned int	height;
	FGL_PixelFormat	pixelFormat;
	unsigned int	stride;
    void *          pFBBaseAddr;
};

typedef struct SurfaceInfo* pSurfaceInfo;

/****************************************************************************
 *  FUNCTIONS
 ****************************************************************************/
#if TARGET_PLATFORM == FPGA_BOARD
    #define fglSysStrncpy(_a_,_b_,_c_) strncpy((char *)(_a_),(char *)(_b_),_c_)
    #define fglSysStrncat strncat
    #define fglSysSprintf(_a_,_b_,...) sprintf((char *)(_a_),(char *)(_b_),__VA_ARGS__)
    #define fglSysStrlen(_a_) strlen((char *)(_a_)) 
#endif


void fglSysInit(float fX, float fY, float fWidth, float fHeight, float fNear, float fFar);

void fglSysCacheFlush(void);

void fglSysZCacheFlush(void);

void fglSysCCacheFlush(void);

void fglSysCacheClear(void);

void fglSysTexCacheClear(void);

void fglSysVTexCacheClear(void);

void fglSysPollingReg(unsigned int addr, unsigned int expect_val);

void fglSysPollingMask(unsigned int addr, unsigned int expect_val, unsigned int mask);

unsigned int fglSysReadReg(unsigned int addr);

void fglSysWriteReg(unsigned int	addr, unsigned int val);

void fglSysModifyReg(unsigned int addr, unsigned int	mask, unsigned int val);

void fglSysDelay(unsigned int loop_cnt);

#if 0
void 
fglSysModifyFB(unsigned int	fb_addr, unsigned int offset, unsigned int mask, unsigned int value);
#endif

void fglSysTransferToPorts(unsigned int* p_source_data, volatile unsigned int* p_slaveport, unsigned int num_dwords);

void fglSysTransferToPort (unsigned int	*psrc_data, unsigned int size);


unsigned int fglSysReservePortAlloc(unsigned int	request_slots, unsigned int	preferred_min);


FGL_Error fglSysAssembleToBMP (
    					unsigned int 	width,
    					unsigned int 	height,
    					unsigned int 	stride,
    					FGL_PixelFormat	pixelFormat,
    					void*			pSourceAddr,
    					void*			pImageData
    				); 

FGL_Error fglSysWriteBMPFile(const char* pFileName, SurfaceInfo & surfaceInfo);


FGL_Error fglSysWritePPMFile(const char* pFileName, SurfaceInfo & surfaceInfo);


void fglDumpContext(const char *pFileName);

#if 1

void fglScalef(float * const pMat, float x, float y, float z);

void fglTranslatef(float* const pMat, float x, float y, float z);

void fglRotatef(float* const pMat, const float angle, float x, float y, float z);

void fglMultMatrixf(float* pRstMat, const float* const pMat1, const float* const pMat2);

void fglMatrixIdentity(float* const pMat);

void fglMatrixTranspose(float* const pMatOut, const float* const pMatIn);

void fglMatrixOrtho(float *pMat, const float w, const float h, const float zn, const float zf);
#endif
	
void fglSwapBuffer(unsigned int uSelect);				   

unsigned int fglWriteVertexShaderConstFloat(unsigned int offset, unsigned int size, float *pdata);

unsigned int fglWriteFragmentShaderConstFloat(unsigned int offset, unsigned int size, float *pdata);

void fglEnableProbeWatchPoints(void);

void fglDisableProbeWatchPoints(void);

void fglProbeWatchPoints(void);


#endif	/* __FIMG_SYSUTIL_H__ */

