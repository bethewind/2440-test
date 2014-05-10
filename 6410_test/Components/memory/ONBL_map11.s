;/**************************************************************************************
;* 
;*	Project Name : S3C6400 Validation
;*
;*	Copyright 2007 by Samsung Electronics, Inc.
;*	All rights reserved.
;*
;*	Project Description :
;*		This software is only for validating functions of the S3C6400.
;*		Anybody can use this software without our permission.
;*  
;*--------------------------------------------------------------------------------------
;* 
;*	File Name : onbl.s
;*  
;*	File Description : 	BootLoader1 for OneNAND boot buffer.
;*						It should be smaller than 1KB.
;*						Load the image from page 1 to 63 (126KB).
;*						And move it into DownloadAddress and then jump.
;*
;*	Author : Heemyung.noh
;*	Dept. : AP Development Team
;*	Created Date : 2007/01/31
;*	Version : 0.1 
;* 
;*	History
;*	- Created(Heemyung.noh 2007/01/31)
;*  
;**************************************************************************************/


				GET		Option.inc
			;	GET		mDiracIII.inc

			
				AREA    ONBL1,CODE,READONLY
				
				EXPORT	OneNandDirectBootCopy
				
OneNandDirectBootCopy			
				mov		r5,#0x0004			;	read from page 1 and sector 0 in block 0

				ldr		r7,=DownloadAddress	;	download address


				ldr		r1,=0x20c3c800		;	'R1' indicate Start Buffer Register

				ldr		r2,[r1,#0x84]
				orr		r2,r2,#0x100
				str		r2,[r1,#0x84]		;	ECC bypass operation

download_image
				ldr		r1,=0x20c3c800		;	'R1' indicate Start Buffer Register
				sub		r0,r1,#0x0400		;	'R0' has OneNAND SFR base address
				mov 	r4,#0x104

				mov		r3,#0				;	block 0
				str		r3,[r0,#0x00]		;	Write 'DFS, FBA' of Flash

				str		r5,[r0,#0x1c]		;	Write 'FPA, FSA' of Flash

				mov		r2,#0x0800			;	Load into DataBuffer0
				str		r2,[r1,#0x00]		;	Write 'BSA, BSC' of DataRAM

				str		r3,[r0,#0x04]		;	Select DataRAM for DDP as 0

				str		r3,[r1,r4]		;	Write 0 to interrupt register
				
				str		r3,[r1,#0x80]		;	Write 'Load' Command

wait_int_reg
				ldr		r2,[r1,r4]
				tst		r2,#0x8000
				beq		wait_int_reg		;	Wait for INT register low to high transition

 [ 1==0
				ldr		r6,=0x20c00800		;	Address of DataBuffer0
				mov		r8,#0x80			;	Amount of move (64 * 8 * 4 = 2KB)
move_page
				;ldmia	r6!,{r0-r3,r9-r12}
				ldr		r0,[r6,#0x00]
				ldr		r1,[r6,#0x04]
				ldr		r2,[r6,#0x08]
				ldr		r3,[r6,#0x0c]
				ldr		r9,[r6,#0x10]
				ldr		r10,[r6,#0x14]
				ldr		r11,[r6,#0x18]
				ldr		r12,[r6,#0x1c]
				add		r6,r6,#0x20
				
				ldr 	r4,=0xffff0000
				bic		r0,r0,r4
				bic		r1,r1,r4
				mov		r1,r1,LSL #16
				orr		r0,r0,r1		

				bic		r2,r2,r4
				bic		r3,r3,r4
				mov		r3,r3,LSL #16
				orr		r2,r2,r3

				bic		r9,r9,r4
				bic		r10,r10,r4
				mov		r10,r10,LSL #16
				orr		r9,r9,r10

				bic		r11,r11,r4
				bic		r12,r12,r4
				mov		r12,r12,LSL #16
				orr		r11,r11,r12
																
				stmia	r7!,{r0,r2,r9,r11}	;	Copy the image from DataBuffer0 into destination.
				subs	r8,r8,#0x1
				bne		move_page
 |
				ldr		r6,=0x20c00800		;	Address of DataBuffer0
				mov		r8,#0x200			;	Amount of move (64 * 8 * 4 = 2KB)
move_page
				;ldmia	r6!,{r0-r3,r9-r12}
				ldr		r0,[r6,#0x00]
				ldr		r1,[r6,#0x04]
				add		r6,r6,#0x08
				
				ldr 	r4,=0xffff0000
				bic		r0,r0,r4
				bic		r1,r1,r4
				mov		r1,r1,LSL #16
				orr		r0,r0,r1		
																
				str		r0,[r7,#0x00]	;	Copy the image from DataBuffer0 into destination.
				add		r7,r7,#0x04
				subs	r8,r8,#0x1
				bne		move_page
 ]

				add		r5,r5,#0x0004		;	Increase page number
				cmp		r5,#0x00fc			;	Repeat movement to page number 63 (126KB)
				blt		download_image

				ldr		r0,=DownloadAddress
				mov		pc,r0				;	Jump to download address

				EXPORT	OneNand4burstPageRead
				
OneNand4burstPageRead
				; r0 : Controller Register Base Address
				; r1 : OneNand Device Source Address
				; r2 : Destination Address(DRAM)
				stmdb	r13!,{r0-r3,r9-r12,lr}
				mov		r3,#0x80						; Count to move (0x80*4burst*4byte = 2KB)
read_page				
				ldmia	r1,{r9-r12}					; 
				stmia	r2!,{r9-r12}					; 
				subs		r3,r3,#0x1						; Count decrement
				bne		read_page 
 
				ldmia	r13!,{r0-r3,r9-r12,pc}

				EXPORT	OneNand4burstPageRead_test
				
OneNand4burstPageRead_test
				; r0 : Controller Register Base Address
				; r1 : OneNand Device Source Address
				; r2 : Destination Address(DRAM)
				stmdb	r13!,{r0-r3,r9-r12,lr}
				mov		r3,#0x10						; Count to move (0x80*4burst*4byte = 2KB)
read_page_test				
				ldmia	r1,{r9-r12}					; 
				stmia	r2!,{r9-r12}					; 
				subs		r3,r3,#0x1						; Count decrement
				bne		read_page_test
 
				ldmia	r13!,{r0-r3,r9-r12,pc}
				

				EXPORT	OneNand4burstSpareRead
 [ 1==1
OneNand4burstSpareRead
				; r0 : Controller Register Base Address
				; r1 : OneNand Device Source Address
				; r2 : Destination Address(DRAM)
				stmdb	r13!,{r0-r3,r9-r12,lr}
				mov		r3,#0x4						; Count to move (0x4*4burst*4byte)
read_spare				
				ldmia	r1,{r9-r12}					; 
				stmia	r2!,{r9-r12}					; 
				subs		r3,r3,#0x1						; Count decrement
				bne		read_spare 
 
				ldmia	r13!,{r0-r3,r9-r12,pc}
 |
OneNand4burstSpareRead
				; r0 : Controller Register Base Address
				; r1 : OneNand Device Source Address
				; r2 : Destination Address(DRAM)
				stmdb	r13!,{r0-r5,r9-r12,lr}
				mov		r3,#0x4						; Count to move (0x4*4burst*4byte)
				ldr		r4, =0x7e00fa0c
read_spare		
				ldr		r5, [r4]
				cmp		r5, #0x12
				beq		read_spare_return
				
				ldmia	r1,{r9-r12}					; 
				stmia	r2!,{r9-r12}					; 
				subs		r3,r3,#0x1						; Count decrement
				bne		read_spare 

read_spare_return
				ldmia	r13!,{r0-r5,r9-r12,pc}
 ]
				
				EXPORT OneNand4burstPageWrite
OneNand4burstPageWrite
				; r0 : Controller Register Base Address
				; r1 : Source Address(DRAM)
				; r2 : OneNand Device Destination Address
				stmdb	r13!,{r0-r3,r9-r12,lr}
				mov		r3,#0x80						; Count to move (0x80*4burst*4byte = 2KB)
write_page				
				ldmia	r1!,{r9-r12}					; 
				stmia	r2,{r9-r12}					; 
				subs		r3,r3,#0x1						; Count decrement
				bne		write_page 
 
				ldmia	r13!,{r0-r3,r9-r12,pc}

				EXPORT	OneNand4burstSpareWrite
OneNand4burstSpareWrite
				; r0 : Controller Register Base Address
				; r1 : Source Address(DRAM)
				; r2 : OneNand Device Destination Address
				stmdb	r13!,{r0-r3,r9-r12,lr}
				mov		r3,#0x4						; Count to move (0x80*4burst*4byte = 2KB)
write_spare				
				ldmia	r1!,{r9-r12}					; 
				stmia	r2,{r9-r12}					; 
				subs		r3,r3,#0x1						; Count decrement
				bne		write_spare 
 
				ldmia	r13!,{r0-r3,r9-r12,pc}



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;; MAP01 Access : Fixed address
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
				EXPORT	OneNand8burstPageRead
OneNand8burstPageRead
				; r0 : Controller Register Base Address
				; r1 : OneNand Device Source Address
				; r2 : Destination Address(DRAM)
				stmdb	r13!,{r0-r3,r5-r12,lr}
				mov		r3,#0x40						; Count to move (0x40*8burst*4byte = 2KB)
read_page_8			
				ldmia	r1,{r5-r12}					; 
				stmia	r2!,{r5-r12}					; 
				subs		r3,r3,#0x1						; Count decrement
				bne		read_page_8	 
 
				ldmia	r13!,{r0-r3,r5-r12,pc}
				
				EXPORT	OneNand8burstSpareRead
OneNand8burstSpareRead
				; r0 : Controller Register Base Address
				; r1 : OneNand Device Source Address
				; r2 : Destination Address(DRAM)
				stmdb	r13!,{r0-r3,r5-r12,lr}
				mov		r3,#0x2						; Count to move (0x40*8burst*4byte = 2KB)
read_page_s8			
				ldmia	r1,{r5-r12}					; 
				stmia	r2!,{r5-r12}					; 
				subs		r3,r3,#0x1						; Count decrement
				bne		read_page_s8	 
 
				ldmia	r13!,{r0-r3,r5-r12,pc}

				
				EXPORT OneNand8burstPageWrite
OneNand8burstPageWrite
				; r0 : Controller Register Base Address
				; r1 : Source Address(DRAM)
				; r2 : OneNand Device Destination Address
				stmdb	r13!,{r0-r3,r5-r12,lr}
				mov		r3,#0x40						; Count to move (0x40*8burst*4byte = 2KB)
write_page_8					
				ldmia	r1!,{r5-r12}					; 
				stmia	r2,{r5-r12}					; 
				subs		r3,r3,#0x1						; Count decrement
				bne		write_page_8	 
 
				ldmia	r13!,{r0-r3,r5-r12,pc}


				EXPORT OneNand8burstSpareWrite
OneNand8burstSpareWrite
				; r0 : Controller Register Base Address
				; r1 : Source Address(DRAM)
				; r2 : OneNand Device Destination Address
				stmdb	r13!,{r0-r3,r5-r12,lr}
				mov		r3,#0x40						; Count to move (0x40*8burst*4byte = 2KB)
write_page_s8					
				ldmia	r1!,{r5-r12}					; 
				stmia	r2,{r5-r12}					; 
				subs		r3,r3,#0x1						; Count decrement
				bne		write_page_s8
 
				ldmia	r13!,{r0-r3,r5-r12,pc}

				
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;; Buffer Access : Increment address
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
				EXPORT	OneNand8burstPageReadDir
				
OneNand8burstPageReadDir
				; r0 : Controller Register Base Address
				; r1 : OneNand Device Source Address
				; r2 : Destination Address(DRAM)
				stmdb	r13!,{r0-r3,r5-r12,lr}
				mov		r3,#0x40						; Count to move (0x40*8burst*4byte = 2KB)
read_page_map00				
				ldmia	r1!,{r5-r12}					; 
				stmia	r2!,{r5-r12}					; 
				subs		r3,r3,#0x1						; Count decrement
				bne		read_page_map00	 
 
				ldmia	r13!,{r0-r3,r5-r12,pc}
				

				EXPORT OneNand8burstPageWriteDir
OneNand8burstPageWriteDir
				; r0 : Controller Register Base Address
				; r1 : Source Address(DRAM)
				; r2 : OneNand Device Destination Address
				stmdb	r13!,{r0-r3,r5-r12,lr}
				mov		r3,#0x40						; Count to move (0x40*8burst*4byte = 2KB)
write_page_map00					
				ldmia	r1!,{r5-r12}					; 
				stmia	r2!,{r5-r12}					; 
				subs		r3,r3,#0x1						; Count decrement
				bne		write_page_map00	 
 
				ldmia	r13!,{r0-r3,r5-r12,pc}				
				LTORG


		        END
