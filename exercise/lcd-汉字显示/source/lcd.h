/**************************************************************
The initial and control for TFT LCD
**************************************************************/
#ifndef __LCD_H__
#define __LCD_H__


#define MVAL		    (0)
#define INVVDEN		    (1)			//0=normal       1=inverted
#define HWSWP		    (1)			//Half word swap control
#define PNRMODE		   (3)			// ����ΪTFT��
#define BPPMODE		   (12)			// ����Ϊ16bppģʽ

//TFT_SIZE
#define LCD_XSIZE_TFT 	(320)	
#define LCD_YSIZE_TFT 	(240)

//Timing parameter for 3.5' LCDjjj


#define CLKVAL_TFT 	(7) 

#define VBPD 		(14)			//��ֱͬ���źŵĺ��
#define VFPD 		(11)			//��ֱͬ���źŵ�ǰ��
#define VSPW 		(2)			//��ֱͬ���źŵ�����

#define HBPD 		(37)			//ˮƽͬ���źŵĺ��
#define HFPD 		(19)			//ˮƽͬ���źŵ�ǰ��
#define HSPW 		(29)
	
#define HOZVAL_TFT	(320-1)
#define LINEVAL_TFT	(240-1)		//����LCD��ʾ������


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
