;/******************************************************************
;* Copyright ARM Limited 2004.  All rights reserved.
;*	
;*
;* sysutil_a.s
;*	Assembler graphic routines
;*
;* Target:	ARM26EJ-S
;******************************************************************/
 
;-------------------------------------------------------------------
;	Module	: sysutil_a.s
;	Scope	: Graphical manipulation routines
;-------------------------------------------------------------------

;-------------------------------------------------------------------
; Include Files:
;-------------------------------------------------------------------

	;INCLUDE	sysutil_h.s

;-------------------------------------------------------------------
; Constant Declarations:
;-------------------------------------------------------------------

;-------------------------------------------------------------------
; Macro Declarations:
;-------------------------------------------------------------------

;-------------------------------------------------------------------
; Structure Declarations:
;-------------------------------------------------------------------

;-------------------------------------------------------------------
; Local Variables Declarations:
;-------------------------------------------------------------------

;-------------------------------------------------------------------
; Local Function Prototypes:
;-------------------------------------------------------------------

;-------------------------------------------------------------------
; Global Variable Declarations:
;-------------------------------------------------------------------

;-------------------------------------------------------------------
; Global Function Prototypes:
;-------------------------------------------------------------------

;-------------------------------------------------------------------

	AREA GraphicFn, CODE, READONLY
	CODE32
	ALIGN	4
	;.arm
	;.text

;-------------------------------------------------------------------
;
;	Function: drvsys_clear_buf 
;   
;   void drvsys_clear_buf(unsigned long*, long, unsigned long)
;
;	Purpose : Set entire buffer to a known value.
;
;	Assumptions : Buffer is on a suitable 8, 4, or 2 word boundary
;
;	Parameters : 
;	  r0	*unsigned long		Buffer pointer.
;	  r1	long				Size in bytes
;	  r2	unsigned long		Fill value 
;	Returns :
;	  	
;
;-------------------------------------------------------------------

	GLOBAL	drvsys_clear_buf
	;.type	drvsys_clear_buf,function
drvsys_clear_buf

	STMFD	sp!, {r4-r10}			; Save work registers
	
	;// Branch to routine from buffer width
	
	ANDS	r10,r1,#&1F				; multiple of 32 bytes?
	BEQ		cb_32_start
	ANDS	r10,r1,#&F				; multiple of 16 bytes?
	BEQ		cb_16_start
	
	;// Else must be multiple of 8 bytes so do this routine
	
cb_08_start
	MOV     r1,r1,lsr #3			; Adjust buf size to no. transfers.
	MOV		r3,r2					; Fill another reg with value

cb_08_loop
	STMIA	r0!,{r2-r3}				; write to buffer (1 write = 2 words)
	
	SUBS	r1,r1,#1				; dec buffer size counter
    BNE		cb_08_loop				; loop until count is negative
    B		cb_end
    
	;// Multiple of 32 so do this routine
	
cb_32_start
	MOV     r1,r1,lsr #5			; Adjust buffer size to no. transfers.
	MOV		r3,r2					; Fill up more registers 
	MOV		r4,r2					; with the fill value
	MOV		r5,r2						
	MOV		r6,r2						
	MOV		r7,r2						
	MOV		r8,r2						
	MOV		r9,r2						

cb_32_loop
	STMIA	r0!,{r2-r9}				; write to buffer (1 write = 8 words)
	
	SUBS	r1,r1,#1				; dec buffer size counter
    BNE		cb_32_loop				; loop until count is negative
    B		cb_end

	;// Multiple of 16 so do this routine
	
cb_16_start
	MOV     r1,r1,lsr #4			; Adjust buffer size to no. transfers.
	MOV		r3,r2					; Fill up more registers 
	MOV		r4,r2					; with the fill value
	MOV		r5,r2						

cb_16_loop
	STMIA	r0!,{r2-r5}				; write to buffer (1 write = 4 words)
	
	SUBS	r1,r1,#1				; dec buffer size counter
    BNE		cb_16_loop				; loop until count is negative

	;// End this routine
	
cb_end    
	LDMFD	sp!,{r4-r10}			; Restore the work registers.
	MOV		pc,lr					; Return

	;.size	drvsys_clear_buf,.setbuf_End-drvsys_clear_buf;

;-------------------------------------------------------------------

;-------------------------------------------------------------------
;
;	Function: drysys_image_blit 
;
;   void drysys_image_blit(tImage*, int, int, tImage*)
;
;	Purpose : Copy an image onto another image.
;
;	Assumptions : Images are on a suitable 8, 4, 2, or 1 word
;				  boundary & 16bpp
;
;	Parameters : 
;	  r0	tImage*		destination pointer
;	  r1	int			x on dst
;	  r2	int			y on dst
;	  r3	tImage*		source pointer
;	Returns :
;	  	
;
;-------------------------------------------------------------------

	GLOBAL	drysys_image_blit
	;.type	drysys_image_blit,function
drysys_image_blit

	STMFD	sp!,{r4-r12,r14}		; Save work registers
	
	;// Setup initial values required
	
	LDRH	r14,[r0,#0]				; load dst width(pixel)
	LDR		r0,[r0,#4]				; load dst data
	MLA		r4,r2,r14,r1			; (y * dst width) + x
	ADD		r0,r0,r4,lsl #1			; move on dst data by (bytes)r4

	LDRH	r1,[r3,#0]				; load src width(pixel)
	LDRH	r2,[r3,#2]				; load src height(pixel)
	
	SUB		r14,r14,r1				; Skip len(pixel) = dst->width - src->width
	MOV		r14,r14,lsl #1			; make skip len bytes
	
	LDR		r3,[r3,#4]				; load src data

	;// Branch to routine from src width
	
	ANDS	r5,r1,#&F				; multiple of 16 pixels?
	BEQ		bb_16_start
	ANDS	r5,r1,#&7				; multiple of 8 pixels?
	BEQ		bb_08_start
	ANDS	r5,r1,#&3				; multiple of 4 pixels?
	BEQ		bb_04_start
	
	;// Else must be multiple of 2 so do this routine
	
bb_02_start
	MOV		r1,r1,lsr #1			; 2 pixel at a time
	
bb_02_loop1
	MOV		r4,r1					; Set loop2 counter
	
bb_02_loop2
	LDR		r5,[r3],#4				; Load 1 reg with src & move src pointer
	SUBS	r4,r4,#1				; dec loop2 counter
	STR		r5,[r0],#4				; Store in dst & move dst pointer
	BNE		bb_02_loop2				; until negative

	ADD		r0,r0,r14				; move on dstPtr by SkipLen
	SUBS	r2,r2,#1				; dec loop1 counter
	BNE		bb_02_loop1				; until negative
	B		bb_end
	
	;// Multiple of 16 so do this routine
	
bb_16_start
	MOV		r1,r1,lsr #4			; 16 pixel at a time
	
bb_16_loop1
	MOV		r4,r1					; Set loop2 counter
	
bb_16_loop2
	LDMIA	r3!,{r5-r12}			; Load 8 reg with src & move src pointer
	SUBS	r4,r4,#1				; decrement loop2 counter
	STMIA	r0!,{r5-r12}			; Store in dst & move dst pointer
	BNE		bb_16_loop2				; until negative

	ADD		r0,r0,r14				; move on dstPtr by SkipLen
	SUBS	r2,r2,#1				; decrement loop1 counter
	BNE		bb_16_loop1				; until negative
	B		bb_end
	
	;// Multiple of 8 so do this routine
	
bb_08_start
	MOV		r1,r1,lsr #3			; 8 pixel at a time
	
bb_08_loop1
	MOV		r4,r1					; Set loop2 counter
	
bb_08_loop2
	LDMIA	r3!,{r5-r8}				; Load 4 reg with src & move src pointer
	SUBS	r4,r4,#1				; decrement loop2 counter
	STMIA	r0!,{r5-r8}				; Store in dst & move dst pointer
	BNE		bb_08_loop2				; until negative

	ADD		r0,r0,r14				; move on dstPtr by SkipLen
	SUBS	r2,r2,#1				; decrement loop1 counter
	BNE		bb_08_loop1				; until negative
	B		bb_end
	
	;// Multiple of 4 so do this routine
	
bb_04_start
	MOV		r1,r1,lsr #2			; 4 pixel at a time
	
bb_04_loop1
	MOV		r4,r1					; Set loop2 counter
	
bb_04_loop2
	LDMIA	r3!,{r5-r6}				; Load 2 reg with src & move src pointer
	SUBS	r4,r4,#1				; decrement loop2 counter
	STMIA	r0!,{r5-r6}				; Store in dst & move dst pointer
	BNE		bb_04_loop2				; until negative

	ADD		r0,r0,r14				; move on dstPtr by SkipLen
	SUBS	r2,r2,#1				; decrement loop1 counter
	BNE		bb_04_loop1				; until negative

	;// End this routine
	
bb_end	
	LDMFD	sp!,{r4-r12,r14}		; Restore work registers.
	MOV		pc,lr					; Return

	;.size	drysys_image_blit,.setbuf_End-drysys_image_blit;

;-------------------------------------------------------------------

;-------------------------------------------------------------------
;
;	Function: drvsys_wired_rect void drvsys_wired_rect(tImage*, int, int, int, int, color)
;
;	Purpose : Draw an outlined rectangled.
;
;	Assumptions : 16bpp
;
;	Parameters : 
;	  r0	tImage*		destination pointer
;	  r1	int			x on dst
;	  r2	int			y on dst
;	  r3	int			width of rect
;	  r12	int			height of rect
;	  r14	color		color of rect
;	Returns :
;	  	
;
;-------------------------------------------------------------------

	GLOBAL	drvsys_wired_rect
	;.type	drvsys_wired_rect,function
drvsys_wired_rect

	STMFD	sp!,{r4-r12,r14}		; Save work registers
	ADD		r14,r13,#&28			; move on lr
	LDMIA	r14,{r12,r14}			; get params 5 & 6
	
	;// Setup initial values required
	
	LDRH	r10,[r0,#0]				; load dst width(pixel)
	LDR		r0,[r0,#4]				; load dst data
	MLA		r4,r2,r10,r1			; (y * dst width) + x
	ADD		r0,r0,r4,lsl #1			; move on dst data by (bytes)r4
	SUB		r5,r12,#1				; calculate bottom line pointer
	MUL		r2,r5,r10				; r2 = dst data + (dst width * (height - 1))
	ADD		r2,r0,r2,lsl #1
	
	STMFD	sp!,{r10}				; Save dst width on stack
	
	ORR		r4,r14,r14,lsl #16		; r4 = color as word
	MOV		r5,r4					; Fill up more registers 
	MOV		r6,r4					; with the fill value
	MOV		r7,r4						
	MOV		r8,r4						
	MOV		r9,r4						
	MOV		r10,r4						
	MOV		r11,r4						
	
	;// Draw top/bottom horizontal lines first
	
	;// Write first pixel if unaligned X position
	
	ANDS	r14,r1,#&1				; X word aligned ?
	STRNEH	r4,[r0],#2				; X not word aligned
	STRNEH	r4,[r2],#2				; so write 1 pixel to top/bottom
	
wr_start
	;// Branch to routine from rect width
	
	SUBS	r14,r3,r14				; reduce width by a drawn pixel
	CMPNE	r14,#1					; check new width > 1
	BLE		wr_last					; 1 or 0 pixels left to draw
	
	TST		r14,#&E					; multiple of 16 pixels?
	BEQ		wr_16_start
	TST		r14,#&6					; multiple of 8 pixels?
	BEQ		wr_08_start
	TST		r14,#&2					; multiple of 4 pixels?
	BEQ		wr_04_start
	
	;// Else must be a multiple of 2 so do this routine
	
wr_02_start
	MOV		r14,r14,lsr #1			; set loop to no. of transfers

wr_02_loop
	STR		r4,[r0],#4				; write top line (1 write = 2 pixels)
	SUBS	r14,r14,#1				; dec loop counter (width)
	STR 	r4,[r2],#4				; write bottom line (1 write = 2 pixels)
    BNE		wr_02_loop				; loop until count is negative
    B		wr_last					; finished inner loop
    
	;// Multiple of 16 so do this routine
	
wr_16_start
	MOV		r14,r14,lsr #4			; set loop to no. of transfers

wr_16_loop
	STMIA	r0!,{r4-r11}			; write top line (1 write = 16 pixels)
	SUBS	r14,r14,#1				; dec loop counter (width)
	STMIA	r2!,{r4-r11}			; write bottom line (1 write = 16 pixels)
    BNE		wr_16_loop				; loop until count is negative
    B		wr_last					; finished inner loop

	;// Multiple of 8 so do this routine
	
wr_08_start
	MOV		r14,r14,lsr #3			; set loop to no. of transfers

wr_08_loop
	STMIA	r0!,{r4-r7}				; write top line (1 write = 8 pixels)
	SUBS	r14,r14,#1				; dec loop counter (width)
	STMIA	r2!,{r4-r7}				; write bottom line (1 write = 8 pixels)
    BNE		wr_08_loop				; loop until count is negative
    B		wr_last					; finished inner loop

	;// Multiple of 4 so do this routine
	
wr_04_start
	MOV		r14,r14,lsr #2			; set loop to no. of transfers

wr_04_loop
	STMIA	r0!,{r4-r5}				; write top line (1 write = 4 pixels)
	SUBS	r14,r14,#1				; dec loop counter (width)
	STMIA	r2!,{r4-r5}				; write bottom line (1 write = 4 pixels)
    BNE		wr_04_loop				; loop until count is negative
									; finished inner loop
									
wr_last
    ;// Write last pixel if (odd & aligned) or (even & unaligned)

	LDMFD	sp!,{r10}				; Load dst width
	AND		r14,r1,#&1				; X word aligned
    SUB		r14,r3,r14				; reduce by alignment
    ANDS	r14,r14,#&1				; 1 more pixel ?
	STRNEH	r4,[r0],#2				; Draw last pixel of these lines
	STRNEH	r4,[r2],#2

	;// Set initial values required for vertical sides
	
	MOV		r5,r10,lsl #1			; r5 = width in bytes
	SUB		r12,r12,#2				; reduce height by 2
	SUB		r9,r10,r3				; r9 = dst width - width
	ADD		r0,r0,r9,lsl #1			; move dst->left pointer by r9
	ADD		r2,r0,r3,lsl #1			; set dst->right pointer = 
	SUB		r2,r2,#2				; dst->left + width - 1
	
wr_vert_loop
	;// Draw left/right vertical lines
	
	STRH	r4,[r0],r5				; Write left side & move byte width
	SUBS	r12,r12,#1				; dec loop counter (height)
	STRH	r4,[r2],r5				; write right side & move byte width
	BNE		wr_vert_loop			; loop until count is negative
	
	;// End this routine
	
	LDMFD	sp!,{r4-r12,r14}		; Restore the work registers.
	MOV		pc,lr					; Return

	;.size	drvsys_wired_rect,.setbuf_End-drvsys_wired_rect;

;-------------------------------------------------------------------

	END

