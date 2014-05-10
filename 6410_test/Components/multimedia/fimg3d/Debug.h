/******************************************************************************
*
* NAME         : Debug.h
* TITLE        : FIMG-3DSE ver. 1.0 Register Level API
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
*	$RCSfile: Debug.h,v $
*	$Revision: 1.3 $
*	$Author: inhosens.lee $
*	$Date: 2006/05/08 05:11:45 $
*	$Locker:  $
*
*	$Source: C:/CVS/CVSrepository/FIMG-3DSE_SW/fimg3dse_fpga/fimg3d/common/Debug.h,v $
*	$State: Exp $
*	$Log: Debug.h,v $
*	Revision 1.3  2006/05/08 05:11:45  inhosens.lee
*	FGL_Error variable was included into FGL_DEBUG macro
*	
*	Revision 1.2  2006/04/13 10:53:52  cheolkyoo.kim
*	no message
*	
*	Revision 1.1  2006/04/13 09:23:13  cheolkyoo.kim
*	Initial import of register level API
*	
*
*****************************************************************************/
#if !defined(__FIMG_DEBUG_H__)
#define __FIMG_DEBUG_H__

#if defined __cplusplus
extern "C" {
#endif


#define FGL_MAX_DEBUG_MESSAGE_LEN	(512)

/* These are privately used by FGL_debug.c, use the DBG_ defines instead */
enum
{
	FGLDBG_LEVEL_FATAL,
	FGLDBG_LEVEL_ERROR,
	FGLDBG_LEVEL_WARNING,
	FGLDBG_LEVEL_MESSAGE,
	FGLDBG_LEVEL_VERBOSE
};


#define FGLDBG_FATAL		    FGLDBG_LEVEL_FATAL,__FILE__,__LINE__
#define FGLDBG_ERROR		    FGLDBG_LEVEL_ERROR,__FILE__,__LINE__
#define FGLDBG_WARNING		FGLDBG_LEVEL_WARNING,__FILE__,__LINE__
#define FGLDBG_MESSAGE		    FGLDBG_LEVEL_MESSAGE,__FILE__,__LINE__
#define FGLDBG_VERBOSE		    FGLDBG_LEVEL_VERBOSE,__FILE__,__LINE__

#define FGLDBG_FATAL_NA		    FGLDBG_LEVEL_FATAL,FIMG_NULL,0
#define FGLDBG_ERROR_NA		    FGLDBG_LEVEL_ERROR,FIMG_NULL,0
#define FGLDBG_WARNING_NA		FGLDBG_LEVEL_WARNING,FIMG_NULL,0
#define FGLDBG_MESSAGE_NA		FGLDBG_LEVEL_MESSAGE,FIMG_NULL,0
#define FGLDBG_VERBOSE_NA		FGLDBG_LEVEL_VERBOSE,FIMG_NULL,0


#define FGL_DPFAPI(X) X


#if defined(FGL_DEBUG)

	#define FGL_ASSERT(EXPR) if (!(EXPR)) fglAssertFail((char *) __FILE__, __LINE__);
	#define FGL_DPFVAARG(_n_,_f_,_l_,...) fglDbgPrintf(_n_,(char *)_f_,_l_,(char *)__VA_ARGS__)
	#define FGL_DPF(X)	FGL_DPFVAARG X

	#define FGL_DPFAPI(X)   {												    	    \
           						if( X != FGL_ERR_NO_ERROR )		                		\
        							fglAssertFail((char *) __FILE__, __LINE__);	    	\
        					}




/*----------------------------------------------------------------------------
<function>
	FUNCTION   : FGLDebugAssertFail
	PURPOSE    : To indicate to the user that a debug assertion has failed and
	             to prevent the program from continuing.
	PARAMETERS : In : pszFile - The name of the source file where the assertion failed
	             In : uLine - The line number of the failed assertion
	RETURNS    : NEVER!
</function>
------------------------------------------------------------------------------*/
void fglAssertFail (
						char*	        pszFile,
						unsigned int	uLine
				   );


/*----------------------------------------------------------------------------
<function>
	FUNCTION   : FGLDebugPrintf
	PURPOSE    : To output a debug message to the user
	PARAMETERS : In : uDebugLevel - The current debug level
	             In : pszFile - The source file generating the message
	             In : uLine - The line of the source file
	             In : pszFormat - The message format string
	             In : ... - Zero or more arguments for use by the format string
	RETURNS    : Nothing
</function>
------------------------------------------------------------------------------*/
void fglDbgPrintf (
						unsigned int	uDebugLevel,
						char*	        pszFilepath,
						unsigned int	uLine,
						char*	        pszFormat,
						...
				  );

#else	/* FGL_DEBUG */

	#define FGL_ASSERT(EXPR) ((void)(EXPR))
	#define FGL_DPF(...)
	#define FGL_DPFAPI(X) X

#endif	/* FGL_DEBUG */



#if defined __cplusplus
}
#endif

#endif	/* __FIMG_DEBUG_H__ */



