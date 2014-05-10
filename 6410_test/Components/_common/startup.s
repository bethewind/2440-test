;/*************************************************************************************
; 
;	Project Name : S3C6410 Validation
;
;	Copyright 2006 by Samsung Electronics, Inc.
;	All rights reserved.
;
;	Project Description :
;		This software is only for validating functions of the S3C6410.
;		Anybody can use this software without our permission.
;  
;--------------------------------------------------------------------------------------
; 
;	File Name : startup.s
;  
;	File Description : This file implements the startup procedure.
;
;	Author	: Heemyung.Noh
;	Dept. : AP Development Team
;	Created Date : 2008/02/20
;	Version : 0.1 
; 
;	History
;	- Created for Scatterloading&TCM option(Heemyung.Noh 2008/02/20)
;  
;*************************************************************************************/

				GET		option.inc

;=======================================================
;						ENTRY  
;=======================================================

				AREA    Init,CODE,READONLY

				ENTRY 
ResetHandler
				
				LDR R13, =top_of_stacks

	[ (USE_TCM = 1)
; ----------------------------------------
;				Initialize TCM 
; ----------------------------------------
; TCM0(D-TCM, I-TCM) Configuration
				mov 	r0,#0x0
				mcr		p15,0,r0,c9,c2,0		; Write TCM Selection register

				ldr 	r0,=ITCM0_BaseAddress
				mcr		p15,0,r0,c9,c1,1		; Write Instruction TCM0 region register
								
				ldr 	r0,=DTCM0_BaseAddress
				mcr		p15,0,r0,c9,c1,0		; Write Data TCM0 region register

; TCM1(D-TCM, I-TCM) Configuration
				mov 	r0,#0x1
				mcr		p15,0,r0,c9,c2,0		; Write TCM Selection register

				ldr 	r0,=ITCM1_BaseAddress
				mcr		p15,0,r0,c9,c1,1		; Write Instruction TCM1 region register
								
				ldr 	r0,=DTCM1_BaseAddress
				mcr		p15,0,r0,c9,c1,0		; Write Data TCM0 region register
	]
				
				IMPORT ScatterLoad
				LDR	R0, =ScatterLoad
				MOV	LR, PC
				BX	R0
				
				EXPORT	__main
__main
				;; initialise the C library (which calls main())
				IMPORT	__rt_entry
				LDR	R0, =__rt_entry
				BX   	R0
				
			   	b		.
;=======================================================			   	
		        END
