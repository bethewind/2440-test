;Start address of each stacks,
_STACK_BASEADDRESS	EQU 0x33ff8000
_ISR_STARTADDRESS	EQU 0x33ffff00




CLKDIV_VAL	EQU	3	



M_MDIV		EQU	92		;Fin=12.0MHz Fout=200MHz
M_PDIV		EQU	4
M_SDIV		EQU	1		; 2440A
	
	
	
U_MDIV		EQU	56		;Fin=12.0MHz Fout=48MHz
U_PDIV		EQU	2
U_SDIV		EQU	2		; 2440A
   	
	



	END

