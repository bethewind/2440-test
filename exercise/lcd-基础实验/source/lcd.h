/**************************************************************
The initial and control for TFT LCD
**************************************************************/
#ifndef __LCD_H__
#define __LCD_H__


#define MVAL		    (0)
#define INVVDEN		    (1)			//0=normal       1=inverted
#define HWSWP		    (1)			//Half word swap control
#define PNRMODE		   (3)			// 设置为TFT屏
#define BPPMODE		   (12)			// 设置为16bpp模式

//TFT_SIZE
#define LCD_XSIZE_TFT 	(320)	
#define LCD_YSIZE_TFT 	(240)

//Timing parameter for 3.5' LCDjjj


#define CLKVAL_TFT 	(7) 

#define VBPD 		(14)			//垂直同步信号的后肩
#define VFPD 		(11)			//垂直同步信号的前肩
#define VSPW 		(2)			//垂直同步信号的脉宽

#define HBPD 		(37)			//水平同步信号的后肩
#define HFPD 		(19)			//水平同步信号的前肩
#define HSPW 		(29)
	
#define HOZVAL_TFT	(320-1)
#define LINEVAL_TFT	(240-1)		//决定LCD显示的行数


#define BPPMODE_TFT           	  (12)
	
#define FRM565_TFT           	  (1) 
#define INVVCLK_TFT           	  (1)	
#define INVVLINE_TFT           	  (1) 
#define INVVFRAME_TFT             (1)
	
#define INVVD_TFT           	  (0) 
#define INVVDEN_TFT           	  (0)	
#define PWREN_TFT           	  (0)


extern void Lcd_Init(void) ;
extern void Lcd_ClearScr( unsigned int c) ;
extern void Paint_Bmp(const unsigned char bmp[]) ;





#endif
