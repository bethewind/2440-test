
#include "2440addr.h"
#include "lcd.h"

#include <stdarg.h>

#define LOW21BITS(n)	((n) & 0x1fffff)	// To get lower 21bits

#define Lcd_Enable()  rLCDCON1 |= 1

volatile unsigned short LCD_BUFFER[240][320];
extern unsigned char __CHS[];

static void Lcd_Config(void)
{
	   rGPCCON = 0xaaaa02a9; 
       rGPDCON = 0xaaaaaaaa; 

       rLCDCON1 = (CLKVAL_TFT << 8)|(3 <<5 )|(BPPMODE_TFT << 1)  ;
    
       rLCDCON2 = (VBPD << 24)|(LINEVAL_TFT << 14)|(VFPD << 6)|(VSPW);
       rLCDCON3 = (HBPD << 19)|(HOZVAL_TFT << 8)|(HFPD);
       rLCDCON4 = (HSPW);
       rLCDCON5 = (FRM565_TFT << 11) | (INVVCLK_TFT << 10) | (INVVLINE_TFT << 9) | (INVVFRAME_TFT << 8)    | (HWSWP) ;
                
                                                                                       
       rLCDSADDR1 = (((unsigned int)LCD_BUFFER >> 22) << 21) | LOW21BITS((unsigned int)LCD_BUFFER >> 1);
       rLCDSADDR2 = LOW21BITS( ((unsigned int)LCD_BUFFER + (LCD_YSIZE_TFT * LCD_XSIZE_TFT *2) ) >> 1 );
       rLCDSADDR3 = (0 << 11) | (LCD_XSIZE_TFT / 1);
        
}



void Lcd_PowerEnable(int powerEnable)
{
	
	rGPGCON = rGPGCON & (~(3<<8)) |(3<<8); 
	rGPGDAT = rGPGDAT | (1<<4) ;
	
	rLCDCON5 = rLCDCON5 & (~(1<<3)) | (powerEnable<<3);   
	
}




void PutPixel(unsigned int x,unsigned int y, unsigned short c )
{
	if ( (x < 320) && (y < 240) )
		LCD_BUFFER[(y)][(x)] = c;
}



void Lcd_Print_ZW(unsigned int x,unsigned int y,unsigned short int QW,unsigned int c)
{
       unsigned short int i,j;
       unsigned char *pZK,mask,buf;

       pZK = &__CHS[ (  ( (QW >> 8) - 1 )*94 + (QW & 0x00FF)- 1 )*32 ];
       for( i = 0 ; i < 16 ; i++ )
       {
              
              mask = 0x80;
              buf = pZK[i*2];
              for( j = 0 ; j < 8 ; j++ )
              {
                     if( buf & mask )
                     {
                            PutPixel(x+j,y+i,c);
                     }
                     mask = mask >> 1;
              } 
        
              
              mask = 0x80;
              buf = pZK[i*2 + 1];
              for( j = 0 ; j < 8 ; j++ )
              {
                     if( buf & mask )
                     {
                            PutPixel(x+j + 8,y+i,c);
                     }
                    mask = mask >> 1;
              }                 
       }
}


void Lcd_Printf(unsigned int x,unsigned int y,unsigned int c,char *fmt,...)
{
       unsigned char LCD_Printf_Buf[256];
       va_list ap;
       unsigned char *pStr = LCD_Printf_Buf;
       unsigned int i = 0;

       va_start(ap,fmt);
       vsprintf(LCD_Printf_Buf,fmt,ap);
       va_end(ap);
	 
       while(*pStr != 0 )
       {
              switch(*pStr)
              {
                 case '\n' :
                        {
		
                           break;
                        }

                 default:
                        {
                           if( *pStr > 0xA0 & *(pStr+1) > 0xA0 )  
                           {
                                  Lcd_Print_ZW( x , y , ((*pStr - 0xA0) << 8) + *(pStr+1) - 0xA0 , c );
                                  pStr++;
                                //  i++;
                                  x += 16;
                           }
                           break;
                        }
              }
		
              pStr++;
           
       }
   
}
void Lcd_Init(void)
{
	Lcd_Config() ;
	Lcd_Enable() ;
	Lcd_PowerEnable(1);
}





