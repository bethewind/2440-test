/**************************************************************
The initial and control for 16Bpp TFT LCD
**************************************************************/


#include "2440addr.h"
#include "lcd.h"

#define LOW21BITS(n)	((n) & 0x1fffff)	// To get lower 21bits

#define Lcd_Enable()  rLCDCON1 |= 1

volatile unsigned short LCD_BUFFER[240][320];


static void Lcd_Config(void)
{
	   rGPCCON = 0xaaaa02a9; 
       rGPDCON = 0xaaaaaaaa; 

       rLCDCON1 = (CLKVAL_TFT << 8)|(3 <<5 )|(BPPMODE_TFT << 1)  ;
    
       rLCDCON2 = (VBPD << 24)|(LINEVAL_TFT << 14)|(VFPD << 6)|(VSPW);
       rLCDCON3 = (HBPD << 19)|(HOZVAL_TFT << 8)|(HFPD);
       rLCDCON4 = (HSPW);
       rLCDCON5 = (FRM565_TFT << 11) | (INVVCLK_TFT << 10) | (INVVLINE_TFT << 9) | (INVVFRAME_TFT << 8) | (HWSWP) ;
                
                                                                                       
       rLCDSADDR1 = (((unsigned int)LCD_BUFFER >> 22) << 21) | LOW21BITS((unsigned int)LCD_BUFFER >> 1);
       rLCDSADDR2 = LOW21BITS( ((unsigned int)LCD_BUFFER + (LCD_YSIZE_TFT * LCD_XSIZE_TFT *2) ) >> 1 );
       rLCDSADDR3 = (0 << 11) | (LCD_XSIZE_TFT / 1);
       rLCDINTMSK |= (3);                                                    // MASK LCD Sub Interrupt
       rTCONSEL &= ~((1 << 0)| (1 << 1) | (1 << 2)) ;                                                                 // Disable LPC3480
       rTPAL = 0; 
}


/**************************************************************
TFT LCD 电源控制引脚使能
**************************************************************/
static void Lcd_PowerEnable(int powerEnable)
{
	
	rGPGCON = rGPGCON & (~(3<<8)) |(3<<8); 
	rGPGDAT = rGPGDAT | (1<<4) ;
	
	rLCDCON5 = rLCDCON5 & (~(1<<3)) | (powerEnable<<3);   
	
}



/**************************************************************
TFT LCD单个象素的显示数据输出
**************************************************************/
void PutPixel(unsigned int x,unsigned int y, unsigned short c )
{
	if ( (x < 320) && (y < 240) )
		LCD_BUFFER[(y)][(x)] = c;
}

/**************************************************************
TFT LCD全屏填充特定颜色单元或清屏
**************************************************************/
void Lcd_ClearScr( unsigned int c)
{
	unsigned int x,y ;
		
    for( y = 0 ; y < 240 ; y++ )
    {
    	for( x = 0 ; x < 320 ; x++ )
    	{
			LCD_BUFFER[y][x] = c ;
    	}
    }
}



void Paint_Bmp(const unsigned char bmp[])
{
	int x,y;
	unsigned short c;
	int p = 0;
	
    for( y = 0 ; y < 240 ; y++ )
    {
    	for( x = 0 ; x < 320 ; x++ )
    	{
    		c = bmp[p+1] | (bmp[p]<<8) ;
			if ( ( x < 320) && ( y < 240) )
				LCD_BUFFER[y][x] = c ;
			p = p + 2 ;
    	}
    }
}

void Lcd_Init(void)
{
	Lcd_Config() ;
	Lcd_Enable() ;
	Lcd_PowerEnable(1);
}





