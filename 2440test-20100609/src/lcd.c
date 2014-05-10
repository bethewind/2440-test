/**************************************************************
The initial and control for 640×480 16Bpp TFT LCD----VGA
**************************************************************/

#include "def.h"
#include "option.h"
#include "2440addr.h"
#include "2440lib.h"
#include "2440slib.h" 


extern const unsigned char sunflower_240x320[];
extern const unsigned char sunflower_800x480[];
extern const unsigned char sunflower_1024x768[];
extern const unsigned char sunflower_640x480[];

#define LCD_XSIZE  LCD_WIDTH
#define LCD_YSIZE  LCD_HEIGHT
#define SCR_XSIZE  LCD_WIDTH
#define SCR_YSIZE  LCD_HEIGHT

volatile static unsigned short LCD_BUFFER[SCR_YSIZE][SCR_XSIZE];

/**************************************************************
640×480 TFT LCD数据和控制端口初始化
**************************************************************/
static void Lcd_Port_Init( void )
{
    rGPCUP=0xffffffff; // Disable Pull-up register
    rGPCCON=0xaaaa02a8; //Initialize VD[7:0],VM,VFRAME,VLINE,VCLK

    rGPDUP=0xffffffff; // Disable Pull-up register
    rGPDCON=0xaaaaaaaa; //Initialize VD[15:8]
}

/**************************************************************
640×480 TFT LCD功能模块初始化
**************************************************************/
static void LCD_Init(void)
{
#define	M5D(n)	((n)&0x1fffff)
#define LCD_ADDR ((U32)LCD_BUFFER)
	rLCDCON1 = (LCD_PIXCLOCK << 8) | (3 <<  5) | (12 << 1);
   	rLCDCON2 = (LCD_UPPER_MARGIN << 24) | ((LCD_HEIGHT - 1) << 14) | (LCD_LOWER_MARGIN << 6) | (LCD_VSYNC_LEN << 0);
   	rLCDCON3 = (LCD_RIGHT_MARGIN << 19) | ((LCD_WIDTH  - 1) <<  8) | (LCD_LEFT_MARGIN << 0);
   	rLCDCON4 = (13 <<  8) | (LCD_HSYNC_LEN << 0);

#if !defined(LCD_CON5)
#    define LCD_CON5 ((1<<11) | (1 << 9) | (1 << 8) | (1 << 3) | (1 << 0))
#endif
    rLCDCON5   =  LCD_CON5;

    rLCDSADDR1 = ((LCD_ADDR >> 22) << 21) | ((M5D(LCD_ADDR >> 1)) <<  0);
    rLCDSADDR2 = M5D((LCD_ADDR + LCD_WIDTH * LCD_HEIGHT * 2) >> 1);
    rLCDSADDR3 = LCD_WIDTH;        

    rLCDINTMSK |= 3;
  	rTCONSEL   &= (~7);
 

   	rTPAL     = 0x0;
   	rTCONSEL &= ~((1<<4) | 1);

    
}

/**************************************************************
LCD视频和控制信号输出或者停止，1开启视频输出
**************************************************************/
static void Lcd_EnvidOnOff(int onoff)
{
    if(onoff==1)
	rLCDCON1|=1; // ENVID=ON
    else
	rLCDCON1 =rLCDCON1 & 0x3fffe; // ENVID Off
}

/**************************************************************
320×240 8Bpp TFT LCD 电源控制引脚使能
**************************************************************/
static void Lcd_PowerEnable(int invpwren,int pwren)
{
    //GPG4 is setted as LCD_PWREN
    rGPGUP = rGPGUP|(1<<4); // Pull-up disable
    rGPGCON = rGPGCON|(3<<8); //GPG4=LCD_PWREN
    
    //Enable LCD POWER ENABLE Function
    rLCDCON5 = rLCDCON5&(~(1<<3))|(pwren<<3);   // PWREN
    rLCDCON5 = rLCDCON5&(~(1<<5))|(invpwren<<5);   // INVPWREN
}


/**************************************************************
640×480 TFT LCD单个象素的显示数据输出
**************************************************************/
static void PutPixel(U32 x,U32 y,U16 c)
{
    if(x<SCR_XSIZE && y<SCR_YSIZE)
		LCD_BUFFER[(y)][(x)] = c;
}

/**************************************************************
640×480 TFT LCD全屏填充特定颜色单元或清屏
**************************************************************/
static void Lcd_ClearScr( U16 c)
{
	unsigned int x,y ;
		
    for( y = 0 ; y < SCR_YSIZE ; y++ )
    {
    	for( x = 0 ; x < SCR_XSIZE ; x++ )
    	{
			LCD_BUFFER[y][x] = c ;
    	}
    }
}

/**************************************************************
LCD屏幕显示垂直翻转
// LCD display is flipped vertically
// But, think the algorithm by mathematics point.
//   3I2
//   4 I 1
//  --+--   <-8 octants  mathematical cordinate
//   5 I 8
//   6I7
**************************************************************/
static void Glib_Line(int x1,int y1,int x2,int y2, U16 color)
{
	int dx,dy,e;
	dx=x2-x1; 
	dy=y2-y1;
    
	if(dx>=0)
	{
		if(dy >= 0) // dy>=0
		{
			if(dx>=dy) // 1/8 octant
			{
				e=dy-dx/2;
				while(x1<=x2)
				{
					PutPixel(x1,y1,color);
					if(e>0){y1+=1;e-=dx;}	
					x1+=1;
					e+=dy;
				}
			}
			else		// 2/8 octant
			{
				e=dx-dy/2;
				while(y1<=y2)
				{
					PutPixel(x1,y1,color);
					if(e>0){x1+=1;e-=dy;}	
					y1+=1;
					e+=dx;
				}
			}
		}
		else		   // dy<0
		{
			dy=-dy;   // dy=abs(dy)

			if(dx>=dy) // 8/8 octant
			{
				e=dy-dx/2;
				while(x1<=x2)
				{
					PutPixel(x1,y1,color);
					if(e>0){y1-=1;e-=dx;}	
					x1+=1;
					e+=dy;
				}
			}
			else		// 7/8 octant
			{
				e=dx-dy/2;
				while(y1>=y2)
				{
					PutPixel(x1,y1,color);
					if(e>0){x1+=1;e-=dy;}	
					y1-=1;
					e+=dx;
				}
			}
		}	
	}
	else //dx<0
	{
		dx=-dx;		//dx=abs(dx)
		if(dy >= 0) // dy>=0
		{
			if(dx>=dy) // 4/8 octant
			{
				e=dy-dx/2;
				while(x1>=x2)
				{
					PutPixel(x1,y1,color);
					if(e>0){y1+=1;e-=dx;}	
					x1-=1;
					e+=dy;
				}
			}
			else		// 3/8 octant
			{
				e=dx-dy/2;
				while(y1<=y2)
				{
					PutPixel(x1,y1,color);
					if(e>0){x1-=1;e-=dy;}	
					y1+=1;
					e+=dx;
				}
			}
		}
		else		   // dy<0
		{
			dy=-dy;   // dy=abs(dy)

			if(dx>=dy) // 5/8 octant
			{
				e=dy-dx/2;
				while(x1>=x2)
				{
					PutPixel(x1,y1,color);
					if(e>0){y1-=1;e-=dx;}	
					x1-=1;
					e+=dy;
				}
			}
			else		// 6/8 octant
			{
				e=dx-dy/2;
				while(y1>=y2)
				{
					PutPixel(x1,y1,color);
					if(e>0){x1-=1;e-=dy;}	
					y1-=1;
					e+=dx;
				}
			}
		}	
	}
}


/**************************************************************
在LCD屏幕上用颜色填充一个矩形
**************************************************************/
static void Glib_FilledRectangle(int x1,int y1,int x2,int y2, U16 color)
{
    int i;

    for(i=y1;i<=y2;i++)
	Glib_Line(x1,i,x2,i,color);
}

/**************************************************************
在LCD屏幕上指定坐标点画一个指定大小的图片
**************************************************************/
static void Paint_Bmp(int x0,int y0,int h,int l,const unsigned char *bmp)
{
	int x,y;
	U32 c;
	int p = 0;
	
    for( y = 0 ; y < l ; y++ )
    {
    	for( x = 0 ; x < h ; x++ )
    	{
    		c = bmp[p+1] | (bmp[p]<<8) ;

			if ( ( (x0+x) < SCR_XSIZE) && ( (y0+y) < SCR_YSIZE) )
				LCD_BUFFER[y0+y][x0+x] = c ;

    		p = p + 2 ;
    	}
    }
}

/**************************************************************
**************************************************************/
void TFT_LCD_Init(void)
{

    LCD_Init();
	LcdBkLtSet( 70 ) ;
	Lcd_PowerEnable(0, 1);
    Lcd_EnvidOnOff(1);		//turn on vedio

    Lcd_ClearScr( (0x00<<11) | (0x00<<5) | (0x00) );  

#if defined(LCD_N35) || defined(LCD_T35)
	Paint_Bmp(0, 0, 240, 320, sunflower_240x320);
#elif defined(LCD_A70)
	Paint_Bmp(0, 0, 800, 480, sunflower_800x480);
#elif defined(LCD_L80)
	Paint_Bmp(0, 0, 640, 480, sunflower_640x480);
#elif defined(LCD_VGA1024768)
    Paint_Bmp(0, 0, 1024, 768, sunflower_1024x768);
#endif    
}

/**************************************************************
**************************************************************/
void TFT_LCD_Test(void)
{
#if defined(LCD_N35) || defined(LCD_T35)
	Uart_Printf("\nTest TFT LCD 240x320!\n");
#elif defined(LCD_A70)
	Uart_Printf("\nTest TFT LCD 800×480!\n");
#elif defined(LCD_L80)
	Uart_Printf("\nTest TFT LCD 640×480!\n");
#elif defined(LCD_VGA1024768)
    Uart_Printf("\nTest VGA 1024x768!\n");
#endif    


    Lcd_Port_Init();
    LCD_Init();
    Lcd_EnvidOnOff(1);		//turn on vedio

	Lcd_ClearScr( (0x00<<11) | (0x00<<5) | (0x00)  )  ;		//clear screen
	Uart_Printf( "\nLCD clear screen is finished! press any key to continue!\n" );
    Uart_Getch() ;		//wait uart input

	Lcd_ClearScr( (0x1f<<11) | (0x3f<<5) | (0x1f)  )  ;		//clear screen
	Uart_Printf( "LCD clear screen is finished! press any key to continue!\n" );
    Uart_Getch() ;		//wait uart input

	Lcd_ClearScr(0xffff);		//fill all screen with some color
	#define LCD_BLANK		30
	#define C_UP		( LCD_XSIZE - LCD_BLANK*2 )
	#define C_RIGHT		( LCD_XSIZE - LCD_BLANK*2 )
	#define V_BLACK		( ( LCD_YSIZE - LCD_BLANK*4 ) / 6 )
	Glib_FilledRectangle( LCD_BLANK, LCD_BLANK, ( LCD_XSIZE - LCD_BLANK ), ( LCD_YSIZE - LCD_BLANK ),0x0000);		//fill a Rectangle with some color

	Glib_FilledRectangle( (LCD_BLANK*2), (LCD_BLANK*2 + V_BLACK*0), (C_RIGHT), (LCD_BLANK*2 + V_BLACK*1),0x001f);		//fill a Rectangle with some color
	Glib_FilledRectangle( (LCD_BLANK*2), (LCD_BLANK*2 + V_BLACK*1), (C_RIGHT), (LCD_BLANK*2 + V_BLACK*2),0x07e0);		//fill a Rectangle with some color
	Glib_FilledRectangle( (LCD_BLANK*2), (LCD_BLANK*2 + V_BLACK*2), (C_RIGHT), (LCD_BLANK*2 + V_BLACK*3),0xf800);		//fill a Rectangle with some color
	Glib_FilledRectangle( (LCD_BLANK*2), (LCD_BLANK*2 + V_BLACK*3), (C_RIGHT), (LCD_BLANK*2 + V_BLACK*4),0xffe0);		//fill a Rectangle with some color
	Glib_FilledRectangle( (LCD_BLANK*2), (LCD_BLANK*2 + V_BLACK*4), (C_RIGHT), (LCD_BLANK*2 + V_BLACK*5),0xf81f);		//fill a Rectangle with some color
	Glib_FilledRectangle( (LCD_BLANK*2), (LCD_BLANK*2 + V_BLACK*5), (C_RIGHT), (LCD_BLANK*2 + V_BLACK*6),0x07ff);		//fill a Rectangle with some color
   	Uart_Printf( "LCD color test, please look! press any key to continue!\n" );
    Uart_Getch() ;		//wait uart input

#if defined(LCD_N35) || defined(LCD_T35)
	Paint_Bmp(0, 0, 240, 320, sunflower_240x320);
#elif defined(LCD_A70)
	Paint_Bmp(0, 0, 800, 480, sunflower_800x480);
#elif defined(LCD_L80)
	Paint_Bmp(0, 0, 640, 480, sunflower_640x480);
#elif defined(LCD_VGA1024768)
    Paint_Bmp(0, 0, 1024, 768, sunflower_1024x768);
#endif    
   	Uart_Printf( "LCD paint a bmp, please look! press any key to continue! \n" );
    Uart_Getch() ;		//wait uart input

    Lcd_EnvidOnOff(0);		//turn off vedio
}
//*************************************************************
