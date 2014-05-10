;Memory Area
;GCS6 32bit(64MB) SDRAM(0x3000_0000-0x33ff_ffff)



	 ;When 100MHz HCLK is used.
;Bank 6 parameter
B6_MT		EQU	0x3	;SDRAM
B6_Trcd		EQU	0x0	;2clk
B6_SCAN		EQU	0x1	;9bit

;Bank 7 parameter
B7_MT		EQU	0x3	;SDRAM
B7_Trcd		EQU	0x0;2clk
B7_SCAN		EQU	0x1	;9bit

;REFRESH parameter
REFEN		EQU	0x1	;Refresh enable
TREFMD		EQU	0x0	;CBR(CAS before RAS)/Auto refresh
Trp         EQU 0x0  ;2clk
Tsrc		EQU	0x1	;5clk	Trc= Trp(3)+Tsrc(5) = 8clock
REFCNT		EQU	1268;1463;1268	;HCLK=105Mhz, (2048+1-7.81*100);75M->1463

	
	
	END
