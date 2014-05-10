/*******************************************************************************
*
* FILE         : fimg_global.c
* TITLE        : FIMGSE Register-Level API 
* AUTHOR       : Jaeho Kim
* CREATED      : 1 April 2006
*
* COPYRIGHT    : 2005 by Samsung Electronics Limited.
*                All rights reserved.  No part of this software, either
*                material or conceptual may be copied or distributed,
*                transmitted, transcribed, stored in a retrieval system
*                or translated into any human or computer language in any
*                form by any means, electronic, mechanical, manual or
*                other-wise, or disclosed to third parties without the
*                express written permission of Samsung Electronics. 
*                Semiconductor Business, System LSI Division,  Mobile 
*                Solution Development, Graphics IP Team in AP.
*
* DESCRIPTION  : APIs to control global registers
*
* PLATFORM     : ALL
* HISTORY      : 	 
* $RCSfile: fimg_global.c,v $
* $Revision: 1.5 $
* $Author: cheolkyoo.kim $
* $Date: 2006/05/03 05:31:43 $
* $Locker:  $
* 
* $Source: C:/CVS/CVSrepository/FIMG-3DSE_SW/fimg3dse_fpga/fimg3d/device/fimg_global.c,v $
* $State: Exp $
* $Log: fimg_global.c,v $
* Revision 1.5  2006/05/03 05:31:43  cheolkyoo.kim
* no message
*
* Revision 1.4  2006/04/13 10:56:31  cheolkyoo.kim
* Replace FG_TRUE with FGL_TRUE.
*
* Revision 1.3  2006/04/10 05:18:41  im.jaeho.kim
* comments are added
*
* Revision 1.2  2006/04/06 06:27:37  im.jaeho.kim
* no message
*
*
*******************************************************************************/

#include "fgl.h"
#include "register.h"

/***************************************************************************** 
 * FUNCTIONS: fglGetPipelineStatus
 * SYNOPSIS: This function obtains status of the pipeline
 * PARAMETERS: [OUT] status: the pointer of FGL_PipelineStatus
 * RETURNS: FGL_ERR_NO_ERROR - always.
 * ERRNO:
 *****************************************************************************/

FGL_Error
fglGetPipelineStatus(FGL_PipelineStatus *status)
{
	unsigned int bits = 0;

	//bits = READREG(FGGB_PIPESTATE);
	READREGP(FGGB_PIPESTATE, bits);

	// [0]: host fifo 
	status->isNotEmptyHOSTFIFO = !(bits & FGL_PIPESTATE_HOSTFIFO);
	// [1]: host interface 
	status->isNotEmptyHI = !(bits & FGL_PIPESTATE_HI);
	// [2]: FIFO between host interface and vertex shader
	status->isNotEmptyHI2VS = !(bits & FGL_PIPESTATE_HI2VS);
	// [3]: vertex cache
	status->isNotEmptyVC = !(bits & FGL_PIPESTATE_VC);
	// [4]: vertex shader
	status->isNotEmptyVS = !(bits & FGL_PIPESTATE_VS);
	// [8]: primitive engine
	status->isNotEmptyPE = !(bits & FGL_PIPESTATE_PE);
	// [9]: triangle setup engine
	status->isNotEmptyTSE = !(bits & FGL_PIPESTATE_TSE);
	// [10]: raster engine
	status->isNotEmptyRA = !(bits & FGL_PIPESTATE_RA);
#if 0
	// [12]: pixel shader 0
	status->isNotEmptyPS0 = !(bits & FGL_PIPESTATE_PS0);
	// [13]: pixel shader 1
	status->isNotEmptyPS1 = !(bits & FGL_PIPESTATE_PS1);
	// [16]: per-fragment 0
	status->isNotEmptyPF0 = !(bits & FGL_PIPESTATE_PF0);
	// [17]: per-fragment 1
	status->isNotEmptyPF1 = !(bits & FGL_PIPESTATE_PF1);
#else
	// [12:13]: pixel shader
	status->isNotEmptyPS = !((bits & FGL_PIPESTATE_PS) == FGL_PIPESTATE_PS);
	// [16:17]: per-fragment 0
	status->isNotEmptyPF = !((bits & FGL_PIPESTATE_PF) == FGL_PIPESTATE_PF);
#endif

	return FGL_ERR_NO_ERROR;
}

/***************************************************************************** 
 * FUNCTIONS: fglFlush
 * SYNOPSIS: This function flushes the fimg3d pipeline
 * PARAMETERS: [IN] pipelineFlags: Specified pipeline states are flushed
 * RETURNS: FGL_ERR_NO_ERROR - always.
 * ERRNO:
 *****************************************************************************/

FGL_Error
fglFlush(unsigned int pipelineFlags)
{
	unsigned int data;

	do
	{
		READREGP(FGGB_PIPESTATE, data);
	}
	while(data & pipelineFlags);

	return FGL_ERR_NO_ERROR;
}


/***************************************************************************** 
 * FUNCTIONS: fglClearCache
 * SYNOPSIS: This function clears the caches
 * PARAMETERS: [IN] clearFlags: Specified caches are cleared
 * RETURNS: FGL_ERR_NO_ERROR - always.
 * ERRNO:
 *****************************************************************************/

FGL_Error
fglClearCache(unsigned int clearFlags)
{
	unsigned int data;

	WRITEREG(FGGB_CACHECTL, clearFlags); // start clearing the cache

	do
	{
		//data = READREG(FGGB_CACHECTL);
		READREGP(FGGB_CACHECTL, data);
	}
	while((data & clearFlags) != 0);

	return FGL_ERR_NO_ERROR;
}

/***************************************************************************** 
 * FUNCTIONS: fglSoftReset
 * SYNOPSIS: This function resets FIMG-3DSE, but the SFR values are not affected
 * PARAMETERS:
 * RETURNS: FGL_ERR_NO_ERROR - always.
 * ERRNO:
 *****************************************************************************/

FGL_Error
fglSoftReset(void)
{
	unsigned int i;
	
	WRITEREG(FGGB_RST, FGL_TRUE);
	
	/* delay */
	for(i = 0; i < 50; i++)
		; /* NULL */

	WRITEREG(FGGB_RST, FGL_FALSE);

	return FGL_ERR_NO_ERROR;
}

/***************************************************************************** 
 * FUNCTIONS: fglGetInterrupt
 * SYNOPSIS: This function returns a bool which interrupt is generated or not
 *           In later implementation, programmer can connect this register
 *           to some conditions except pipeline status.
 *           
 * PARAMETERS: [OUT] isGenInterrupt: interrupt bit
 * RETURNS: FGL_ERR_NO_ERROR - always.
 * ERRNO:
 *****************************************************************************/

FGL_Error
fglGetInterrupt(FG_BOOL* isGenInterrupt)
{
	//*isGenInterrupt = READREG(FGGB_INTPENDING);
	READREGP(FGGB_INTPENDING, *isGenInterrupt);
	*isGenInterrupt &= FGL_TRUE; // return first bit only

	return FGL_ERR_NO_ERROR;
}

/***************************************************************************** 
 * FUNCTIONS: fglPendInterrupt
 * SYNOPSIS: This function makes currunt interrupt to pend
 * PARAMETERS:
 * RETURNS: FGL_ERR_NO_ERROR - always.
 * ERRNO:
 *****************************************************************************/

FGL_Error
fglPendInterrupt(void)
{
	WRITEREG(FGGB_INTPENDING, FGL_TRUE);

	return FGL_ERR_NO_ERROR;
}

/***************************************************************************** 
 * FUNCTIONS: fglEnableInterrupt
 * SYNOPSIS: This function enables the FIMG-3DSE interrupt
 * PARAMETERS:
 * RETURNS: FGL_ERR_NO_ERROR - always.
 * ERRNO:
 *****************************************************************************/

FGL_Error
fglEnableInterrupt(void)
{
	WRITEREG(FGGB_INTMASK, FGL_TRUE);
	return FGL_ERR_NO_ERROR;
}
	
/***************************************************************************** 
 * FUNCTIONS: fglDisableInterrupt
 * SYNOPSIS: This function disables the FIMG-3DSE interrupt
 * PARAMETERS:
 * RETURNS: FGL_ERR_NO_ERROR - always.
 * ERRNO:
 *****************************************************************************/

FGL_Error
fglDisableInterrupt(void)
{
	WRITEREG(FGGB_INTMASK, FGL_FALSE);
	return FGL_ERR_NO_ERROR;
}


/***************************************************************************** 
 * FUNCTIONS: fglSetInterruptBlock
 * SYNOPSIS: This function sets pipeline blocks to generate interrupt.
 * PARAMETERS: [IN] pipeMask: Oring PIPESTATE_XXXX block of generating interrupt
 * RETURNS: FGL_ERR_NO_ERROR - always.
 * ERRNO:
 *****************************************************************************/

FGL_Error
fglSetInterruptBlock(unsigned int pipeMask)
{
	WRITEREG(FGGB_PIPEMASK, pipeMask);
	return FGL_ERR_NO_ERROR;
}

/***************************************************************************** 
 * FUNCTIONS: fglSetInterruptState
 * SYNOPSIS: This function sets an interrupt generated state of each block
 * PARAMETERS: [IN] status: each block state for interrupt to occur
 * RETURNS: FGL_ERR_NO_ERROR - always.
 * ERRNO:
 *****************************************************************************/

FGL_Error
fglSetInterruptState(FGL_PipelineStatus status)
{
	unsigned int bitPipeState;

	bitPipeState = status.isNotEmptyHOSTFIFO << FGL_PIPESTATE_HOSTFIFO_BIT;
	bitPipeState |= status.isNotEmptyHI << FGL_PIPESTATE_HI_BIT;
	bitPipeState |= status.isNotEmptyHI2VS << FGL_PIPESTATE_HI2VS_BIT;
	bitPipeState |= status.isNotEmptyVS << FGL_PIPESTATE_VS_BIT;
	bitPipeState |= status.isNotEmptyVC << FGL_PIPESTATE_VC_BIT;
	bitPipeState |= status.isNotEmptyPS << FGL_PIPESTATE_PS0_BIT;
	bitPipeState |= status.isNotEmptyPF << FGL_PIPESTATE_PF0_BIT;
	
	bitPipeState |= status.isNotEmptyPS << FGL_PIPESTATE_PS1_BIT;
	bitPipeState |= status.isNotEmptyPF << FGL_PIPESTATE_PF1_BIT;

	WRITEREG(FGGB_PIPETGTSTATE, bitPipeState);

	return FGL_ERR_NO_ERROR;
}

/***************************************************************************** 
 * FUNCTIONS: fglGetInterruptState
 * SYNOPSIS: This function returns the value of pipeline-state when interrupt
 *           is to occur
 * PARAMETERS: [OUT] status: Each block state of pipeline
 * RETURNS: FGL_ERR_NO_ERROR - always.
 * ERRNO:
 *****************************************************************************/

FGL_Error
fglGetInterruptState(FGL_PipelineStatus* status)
{
	unsigned int bitPipeState;

	 //bitPipeState = READREG(FGGB_PIPEINTSTATE);
	 READREGP(FGGB_PIPEINTSTATE, bitPipeState);

	status->isNotEmptyHOSTFIFO = bitPipeState & FGL_PIPESTATE_HOSTFIFO;
	status->isNotEmptyHI = bitPipeState & FGL_PIPESTATE_HI;
	status->isNotEmptyHI2VS = bitPipeState & FGL_PIPESTATE_HI2VS;
	status->isNotEmptyVS = bitPipeState & FGL_PIPESTATE_VS;
	status->isNotEmptyVC = bitPipeState & FGL_PIPESTATE_VC;
	status->isNotEmptyPS = bitPipeState & FGL_PIPESTATE_PS0;
	status->isNotEmptyPF = bitPipeState & FGL_PIPESTATE_PF0;
	
	status->isNotEmptyPS |= (bitPipeState & FGL_PIPESTATE_PS1);
	status->isNotEmptyPF |= (bitPipeState & FGL_PIPESTATE_PF1);

	return FGL_ERR_NO_ERROR;
}
