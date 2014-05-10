    GET option.inc
	GET memcfg.inc
	GET 2440addr.inc

;Pre-defined constants

USERMODE    EQU 	0x10
FIQMODE     EQU 	0x11
IRQMODE     EQU 	0x12
SVCMODE     EQU 	0x13
ABORTMODE   EQU 	0x17
UNDEFMODE   EQU 	0x1b
MODEMASK    EQU 	0x1f
NOINT       EQU 	0xc0

;The location of stacks

UserStack	EQU	(_STACK_BASEADDRESS-0x3800)	;0x33ff4800 ~
SVCStack	EQU	(_STACK_BASEADDRESS-0x2800)	;0x33ff5800 ~
UndefStack	EQU	(_STACK_BASEADDRESS-0x2400)	;0x33ff5c00 ~
AbortStack	EQU	(_STACK_BASEADDRESS-0x2000)	;0x33ff6000 ~
IRQStack	EQU	(_STACK_BASEADDRESS-0x1000)	;0x33ff7000 ~
FIQStack	EQU	(_STACK_BASEADDRESS-0x0)	;0x33ff8000 ~



 	MACRO
$HandlerLabel HANDLER $HandleAddr

$HandlerLabel
	sub	    sp,sp,#4			;decrement sp(to store jump address)
	stmfd	sp!,{r0}			;PUSH the work register to stack(lr does not push because it return to original address)
	ldr     r0,=$HandleAddr		;load the address of HandleXXX to r0
	ldr     r0,[r0]	 			;load the contents(service routine start address) of HandleXXX
	str     r0,[sp,#4]      	;store the contents(ISR) of HandleXXX to stack
	ldmfd   sp!,{r0,pc}     	;POP the work register and pc(jump to ISR)
	MEND


	IMPORT  |Image$$RO$$Base|	; Base of ROM code
	IMPORT  |Image$$RO$$Limit|  ; End of ROM code (=start of ROM data)
	IMPORT  |Image$$RW$$Base|   ; Base of RAM to initialise
	IMPORT  |Image$$ZI$$Base|   ; Base and limit of area
	IMPORT  |Image$$ZI$$Limit|  ; to zero initialise
	IMPORT  Main		; The main entry of mon program
	IMPORT  RdNF2SDRAM	; Copy Image from Nand Flash to SDRAM

	
	AREA    Init,CODE,READONLY
	ENTRY
ResetEntry

	b	ResetHandler
	b	HandlerUndef	;handler for Undefined mode
	b	HandlerSWI		;handler for SWI interrupt
	b	HandlerPabort	;handler for PAbort
	b	HandlerDabort	;handler for DAbort
	b	.				;reserved
	b	HandlerIRQ		;handler for IRQ interrupt
	b	HandlerFIQ		;handler for FIQ interrupt

	
HandlerFIQ		HANDLER HandleFIQ
HandlerIRQ		HANDLER HandleIRQ
HandlerUndef	HANDLER HandleUndef
HandlerSWI		HANDLER HandleSWI
HandlerDabort	HANDLER HandleDabort
HandlerPabort	HANDLER HandlePabort

IsrIRQ
	sub	sp,sp,#4       ;reserved for PC
	stmfd	sp!,{r8-r9}

	ldr	r9,=INTOFFSET
	ldr	r9,[r9]
	ldr	r8,=HandleEINT0
	add	r8,r8,r9,lsl #2
	ldr	r8,[r8]
	str	r8,[sp,#8]
	ldmfd	sp!,{r8-r9,pc}

	LTORG


ResetHandler

	ldr	r0,=WTCON       ;watch dog disable
	ldr	r1,=0x0
	str	r1,[r0]

	ldr	r0,=INTMSK
	ldr	r1,=0xffffffff  ;all interrupt disable
	str	r1,[r0]

	ldr	r0,=INTSUBMSK
	ldr	r1,=0x7fff		;all sub interrupt disable
	str	r1,[r0]
	
	;To reduce PLL lock time, adjust the LOCKTIME register.
	ldr	r0,=LOCKTIME
	ldr	r1,=0xffffff
	str	r1,[r0]

	ldr	r0,=CLKDIVN
	ldr	r1,=CLKDIV_VAL		; 0=1:1:1, 1=1:1:2, 2=1:2:2, 3=1:2:4, 4=1:4:4, 5=1:4:8, 6=1:3:3, 7=1:3:6.
	str	r1,[r0]

	;CLKDIV_VAL>1 means Fclk:Hclk is not 1:1.
	[ CLKDIV_VAL>1 		; means Fclk:Hclk is not 1:1.
			mrc p15,0,r0,c1,c0,0
			orr r0,r0,#0xc0000000;R1_nF:OR:R1_iA
			mcr p15,0,r0,c1,c0,0
			|
			mrc p15,0,r0,c1,c0,0
			bic r0,r0,#0xc0000000;R1_iA:OR:R1_nF
			mcr p15,0,r0,c1,c0,0
		]
	;Configure UPLL
		ldr	r0,=UPLLCON
		ldr	r1,=((U_MDIV<<12)+(U_PDIV<<4)+U_SDIV)	;Fin = 12.0MHz, UCLK = 48MHz
		str	r1,[r0]
		nop	; at least 7-clocks delay must be inserted for setting hardware be completed.
		nop
		nop
		nop
		nop
		nop
		nop
		;Configure MPLL
		ldr	r0,=MPLLCON
		ldr	r1,=((M_MDIV<<12)+(M_PDIV<<4)+M_SDIV)		;Fin = 12.0MHz, FCLK = 200MHz
		str	r1,[r0]
	
	;Set memory control registers
 		
 	adrl r0, SMRDATA	;be careful!
 	ldmia r0,{r1-r13}
	ldr	r0,=BWSCON
	stmia r0,{r1-r13}

	bl	InitStacks

;===========================================================
	
	ldr	r0, =BWSCON
	ldr	r0, [r0]
	ands	r0, r0, #6		;OM[1:0] != 0, NOR FLash boot
	bne	copy_proc_beg		;do not read nand flash
	adr	r0, ResetEntry		;OM[1:0] == 0, NAND FLash boot
	cmp	r0, #0				;if use Multi-ice, 
	bne	copy_proc_beg		;do not read nand flash for boot
	;nop
;===========================================================
nand_boot_beg

		bl RdNF2SDRAM

		ldr	pc, =copy_proc_beg
;===========================================================
copy_proc_beg
	adr	r0, ResetEntry
	ldr	r2, BaseOfROM
	cmp	r0, r2
	ldreq	r0, TopOfROM
	beq	InitRam	
	ldr r3, TopOfROM
0	
	ldmia	r0!, {r4-r7}
	stmia	r2!, {r4-r7}
	cmp	r2, r3
	bcc	%B0
	
	sub	r2, r2, r3
	sub	r0, r0, r2				
		
InitRam	
	ldr	r2, BaseOfBSS
	ldr	r3, BaseOfZero	
0
	cmp	r2, r3
	ldrcc	r1, [r0], #4
	strcc	r1, [r2], #4
	bcc	%B0	

	mov	r0,	#0
	ldr	r3,	EndOfBSS
1	
	cmp	r2,	r3
	strcc	r0, [r2], #4
	bcc	%B1


;===========================================================
  	; Setup IRQ handler

	
	ldr	r0,=HandleIRQ	;This routine is needed
	ldr	r1,=IsrIRQ	;if there is not 'subs pc,lr,#4' at 0x18, 0x1c
	str	r1,[r0]

 	b	Main	;Do not use main() because ......
 
InitStacks
	mrs	r0,cpsr
	bic	r0,r0,#MODEMASK
	orr	r1,r0,#UNDEFMODE|NOINT
	msr	cpsr_cxsf,r1		;UndefMode
	ldr	sp,=UndefStack		; UndefStack=0x33FF_5C00

	orr	r1,r0,#ABORTMODE|NOINT
	msr	cpsr_cxsf,r1		;AbortMode
	ldr	sp,=AbortStack		; AbortStack=0x33FF_6000

	orr	r1,r0,#IRQMODE|NOINT
	msr	cpsr_cxsf,r1		;IRQMode
	ldr	sp,=IRQStack		; IRQStack=0x33FF_7000

	orr	r1,r0,#FIQMODE|NOINT
	msr	cpsr_cxsf,r1		;FIQMode
	ldr	sp,=FIQStack		; FIQStack=0x33FF_8000

	bic	r0,r0,#MODEMASK|NOINT
	orr	r1,r0,#SVCMODE
	msr	cpsr_cxsf,r1		;SVCMode
	ldr	sp,=SVCStack		; SVCStack=0x33FF_5800

	;USER mode has not be initialized.

	mov	pc,lr
	;The LR register will not be valid if the current mode is not SVC mode.

	LTORG

SMRDATA 
	DCD 0x22011000
	DCD 0x00000700   ;GCS0
	DCD 0x00000700   ;GCS1
	DCD 0x00000700   ;GCS2
	DCD 0x00000700   ;GCS3
	DCD 0x00000700   ;GCS4
	DCD 0x00000700   ;GCS5
	DCD ((B6_MT<<15)+(B6_Trcd<<2)+(B6_SCAN))    ;GCS6
	DCD ((B7_MT<<15)+(B7_Trcd<<2)+(B7_SCAN))    ;GCS7
	DCD ((REFEN<<23)+(TREFMD<<22)+(Trp<<20)+(Tsrc<<18)+REFCNT)

	;DCD 0x32	    ;SCLK power saving mode, BANKSIZE 64M/64M
	DCD 0xB1
	DCD 0x30	    ;MRSR6 CL=3clk
	DCD 0x30	    ;MRSR7 CL=3clk
	
BaseOfROM	DCD	|Image$$RO$$Base|
TopOfROM	DCD	|Image$$RO$$Limit|
BaseOfBSS	DCD	|Image$$RW$$Base|
BaseOfZero	DCD	|Image$$ZI$$Base|
EndOfBSS	DCD	|Image$$ZI$$Limit|
	
	ALIGN

	AREA RamData, DATA, READWRITE

	^   _ISR_STARTADDRESS		; _ISR_STARTADDRESS=0x33FF_FF00
HandleReset 	#   4
HandleUndef 	#   4
HandleSWI		#   4
HandlePabort    #   4
HandleDabort    #   4
HandleReserved  #   4
HandleIRQ		#   4
HandleFIQ		#   4


;IntVectorTable  ;@0x33FF_FF20
HandleEINT0		#   4
HandleEINT1		#   4
HandleEINT2		#   4
HandleEINT3		#   4
HandleEINT4_7	#   4
HandleEINT8_23	#   4
HandleCAM		#   4		
HandleBATFLT	#   4
HandleTICK		#   4
HandleWDT		#   4
HandleTIMER0 	#   4
HandleTIMER1 	#   4
HandleTIMER2 	#   4
HandleTIMER3 	#   4
HandleTIMER4 	#   4
HandleUART2  	#   4
;@0x33FF_FF60
HandleLCD 		#   4
HandleDMA0		#   4
HandleDMA1		#   4
HandleDMA2		#   4
HandleDMA3		#   4
HandleMMC		#   4
HandleSPI0		#   4
HandleUART1		#   4
HandleNFCON		#   4		
HandleUSBD		#   4
HandleUSBH		#   4
HandleIIC		#   4
HandleUART0 	#   4
HandleSPI1 		#   4
HandleRTC 		#   4
HandleADC 		#   4
;@0x33FF_FFA0
	END
