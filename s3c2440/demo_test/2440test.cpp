//====================================================================
// File Name : 2440test.c
// Function  : S3C2440 Test Main Menu
// Program   : Shin, On Pil (SOP)
// Date      : May 30, 2002
// Version   : 0.0
// History
//   0.0 : Programming start (February 20,2002) -> SOP
//   1.0 (20020530) : First release for customer
//   1.1 (20020801) : Strata NOR Flash Added and etc. -> SOP
//   R0.0 (20030415): Modified for 2440. -> DonGo
//====================================================================

#include <stdlib.h>
#include <string.h>

#include "def.h"
#include "option.h"
#include "2440addr.h"
#include "2440lib.h" // 03.11.27 junon

#if USE_MAIN
#include <stdio.h>
#endif


#ifdef __cplusplus
extern "C" {
#endif
    void xmain(void);
    int  delayLoopCount;
#ifdef __cplusplus
}
#endif


#if USE_MAIN		// USE_MAIN=TRUE
// Ensure no functions that use semihosting SWIs are linked in from the C library
	#if !SEMIHOSTING	// SEMIHOSTING=FALSE
	#pragma import(__use_no_semihosting_swi) 
	#endif //!SEMIHOSTING
	int main(void)
#else
	#ifdef  __cplusplus	// __cplusplus=TRUE
	extern "C" void CEntry(void) //To retain the C naming method
	#else 
	void CEntry(void)
	#endif //__cplusplus
#endif //(ADS10 && USE_MAIN)
{
	main();
}

