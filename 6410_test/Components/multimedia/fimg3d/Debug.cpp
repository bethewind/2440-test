/******************************************************************************
*
* NAME         : fimg_debug.c
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
*	$RCSfile: fimg_debug.c,v $
*	$Revision: 1.2 $
*	$Author: cheolkyoo.kim $
*	$Date: 2006/05/08 05:41:23 $
*	$Locker:  $
*
*	$Source: C:/CVS/CVSrepository/FIMG-3DSE_SW/fimg3dse_fpga/fimg3d/common/fimg_debug.c,v $
*	$State: Exp $
*	$Log: fimg_debug.c,v $
*	Revision 1.2  2006/05/08 05:41:23  cheolkyoo.kim
*	no message
*	
*	Revision 1.1  2006/04/13 09:23:12  cheolkyoo.kim
*	Initial import of register level API
*	
*
*****************************************************************************/
#include "fgl.h"
#include "Config.h"
//#include "Macros.h"
#include "Debug.h"
#include "SysUtility.h"

//#include "uart.h"




#if defined(FGL_DEBUG)

/****************************************************************************
 *  FUNCTIONS
 ****************************************************************************/

/*----------------------------------------------------------------------------
<function>
	FUNCTION   : MGLDebugAssertFail
	PURPOSE    : To indicate to the user that a debug assertion has failed and
	             to prevent the program from continuing.
	PARAMETERS : In : pszFile - The name of the source file where the assertion failed
	             In : uLine - The line number of the failed assertion
	RETURNS    : NEVER!
</function>
------------------------------------------------------------------------------*/
void fglAssertFail(char* pszFile, unsigned int uLine)
{
	fglDbgPrintf(FGLDBG_FATAL, pszFile, uLine, (char *) "Debug assertion failed!");

	while (1);
}


/*----------------------------------------------------------------------------
<function>
	FUNCTION   : fglDbgPrintf
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
						char*			pszFilepath,
						unsigned int	uLine,
						char*			pszFormat,
						...
				  )
{
	static char szMessage[FGL_MAX_DEBUG_MESSAGE_LEN+1];
	char* pszEndOfMessage = FIMG_NULL;
	char* pszFilename = FIMG_NULL;
	char* pszCurrentChar;
	va_list ArgList;

	if (uDebugLevel > g_uMGLDebugLevel)
	{
		fglSysStrncpy(szMessage, "FIMG-3DSE: ", FGL_MAX_DEBUG_MESSAGE_LEN);

		pszEndOfMessage = &szMessage[fglSysStrlen(szMessage)];

		va_start(ArgList, pszFormat);
		vsprintf((char *)pszEndOfMessage, (char *)pszFormat, ArgList);
		va_end(ArgList);

		pszFilename = pszFilepath;
		pszCurrentChar = pszFilepath;

		/* Strip the path from the filename */
		while (*pszCurrentChar != FGL_STRING_TERMINATOR)
		{
			if (FGL_IS_FILE_SEPARATOR(*pszCurrentChar))
			{
				pszFilename = pszCurrentChar + 1;
			}
			++pszCurrentChar;
		}

		pszEndOfMessage = &szMessage[fglSysStrlen(szMessage)];

//daedoo
//		sprintf((char *)pszEndOfMessage, " (%u,%s)\n", uLine, pszFilename);

		//fprintf(stderr, (char *)szMessage);
		UART_Printf("%s\n", (char *)szMessage);
	}
}

#endif

