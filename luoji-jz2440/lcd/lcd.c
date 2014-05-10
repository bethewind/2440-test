/*
 * S2440lib.c
 *
 *  Created on: 2014-3-12
 *      Author: cyj
 */

#include "addr2440.h"
#include "2440lib.h"
#include "lcd.h"
#include "pic.h"

#define FRAMEBUFFERBASE 0x30400000
#define Xsize 480
#define Ysize 272

unsigned char zhao[]=                  //赵

{

0x08, 0x00, 0x08, 0x00, 0x08, 0x04, 0x7E, 0x84, 0x08, 0x48, 0x08, 0x28, 0xFF, 0x10, 0x08, 0x10,

0x28, 0x28, 0x2F, 0x28, 0x28, 0x44, 0x28, 0x84, 0x58, 0x00, 0x48, 0x00, 0x87, 0xFE, 0x00, 0x00

};

unsigned char chun[]=                 //春

{

0x01, 0x00, 0x01, 0x00, 0x3F, 0xFC, 0x01, 0x00, 0x1F, 0xF8, 0x02, 0x00, 0xFF, 0xFE, 0x04, 0x20,

0x08, 0x18, 0x3F, 0xEE, 0xC8, 0x24, 0x0F, 0xE0, 0x08, 0x20, 0x08, 0x20, 0x0F, 0xE0, 0x00, 0x00

};

unsigned char jiang[]=                 //江

{

0x20, 0x00, 0x10, 0x00, 0x13, 0xFC, 0x00, 0x40, 0x88, 0x40, 0x48, 0x40, 0x50, 0x40, 0x10, 0x40,

0x10, 0x40, 0x20, 0x40, 0xE0, 0x40, 0x20, 0x40, 0x20, 0x40, 0x2F, 0xFE, 0x20, 0x00, 0x00, 0x00,

};

unsigned char ASCII_A[]=           //A

{

0x00, 0x00, 0x00, 0x10, 010, 0x18, 0x28, 0x28, 0x24, 0x3C, 0x44, 0x42, 0x42, 0xE7, 0x00, 0x00

};

unsigned char ASCII_R[]=           //R

{

0x00, 0x00, 0x00, 0xFC, 0x42, 0x42, 0x42, 0x7C, 0x48, 0x48, 0x44, 0x44, 0x42, 0xE3, 0x00, 0x00

};

unsigned char ASCII_M[]=          //M

{

0x00, 0x00, 0x00, 0xEE, 0x6C, 0x6C, 0x6C, 0x6C, 0x54, 0x54, 0x54, 0x54, 0x54, 0xD6, 0x00, 0x00

};



void Lcd_Init()
{
    rGPBCON &= ~(0x3);
    rGPBCON |= 0x1;  // output
    rGPBDAT |= 0x1; // high,KEYBOARD

    rGPCCON = 0xaaaaaaaa;
    rGPCUP |= 0xffff; // up disable
    rGPDCON = 0xaaaaaaaa;
    rGPDUP |= 0xffff; // up disable

    rGPGCON |= (0x3<<8); // LCDPOWER
    rGPGUP  |= (0x1<<4); // updisable

    // lcd config
    // VCLK = HCLK / [(CLKVAL + 1) × 2] ; HCLK = 100MHZ; 5MHZ<=VCLK<=12MHZ,type=9MHZ,so CLKVAL=4
    LCDCON1 = (4<<8) | (0x3<<5) | (0xc<<1) | 0;
    // VSPW+1 =tvp=10,VBPD+1=tvb=2,LINEVAL +1=tvd=272,VFPD+1=tvf=2
    LCDCON2 = ((2-1)<<24) | ((272-1)<<14) | ((2-1)<<6) | ((10-1)<<0);
    // HSPW+1=thp=41,HBPD+1=thb=2,HOZVAL+1=thd=480,HFPD+1=thf=2
    LCDCON3 = ((2-1)<<19) | ((480-1)<<8) | ((2-1)<<0);
    LCDCON4 = ((41-1)<<0);
    LCDCON5 = (1<<11) | (1<<9) | (1<<8) | (1<<3) | (1<<2) | (1<<0);

    LCDSADDR1 = (((FRAMEBUFFERBASE>>22)&0x1ff)<<21) | ((FRAMEBUFFERBASE>>1)&0x1fffff);
    LCDSADDR2  = ((FRAMEBUFFERBASE>>1)&0x1fffff) + (480+0) * 272;
    LCDSADDR3 = (0<<11) | 480;

    TPAL = 0;

    LCDCON1 |= 1;
}

void Paint_Bmp(const unsigned char bmp[])
{
	int x,y;
	unsigned short c;
	int p = 0;

    for( y = 0 ; y < 272 ; y++ )
    {
    	for( x = 0 ; x < 362; x++ )
    	{
    		c = bmp[p+1] | (bmp[p]<<8) ;
    		if ( ( x < 362) && ( y < 272) )
    			PutPixel(x,y,c);
			p = p + 2 ;
    	}
    }
}

void DrawLine(int x1,int y1,int x2,int y2,unsigned short color)
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
            else        // 2/8 octant
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
        else           // dy<0
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
            else        // 7/8 octant
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
        dx=-dx;     //dx=abs(dx)
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
            else        // 3/8 octant
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
        else           // dy<0
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
            else        // 6/8 octant
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

void PutPixel(unsigned long x, unsigned long y, unsigned short color)
{
    unsigned short* address;
    address = (unsigned short*)FRAMEBUFFERBASE + (y*Xsize+x);
    *address = color;
}

//绘制大小为16×16的中文字符

void Draw_Text16(unsigned int x,unsigned int y,unsigned short color,const unsigned char ch[])

{

       unsigned short int i,j;

       unsigned char mask,buffer;



       for(i=0;i<16;i++)

       {

              mask=0x80;                  //掩码

              buffer=ch[i*2];             //提取一行的第一个字节

              for(j=0;j<8;j++)

              {

                     if(buffer&mask)

                     {

                            PutPixel(x+j,y+i,color);        //为笔画上色

                     }

                     mask=mask>>1;

              }

              mask=0x80;                  //掩码

              buffer=ch[i*2+1];         //提取一行的第二个字节

              for(j=0;j<8;j++)

              {

                     if(buffer&mask)

                     {

                            PutPixel(x+j+8,y+i,color);           //为笔画上色

                     }

                     mask=mask>>1;

              }

       }

}



//绘制大小为8×16的ASCII码

void Draw_ASCII(unsigned int x,unsigned int y,unsigned short color,const unsigned char ch[])

{

       unsigned short int i,j;

       unsigned char mask,buffer;



       for(i=0;i<16;i++)

       {

              mask=0x80;

              buffer=ch[i];

              for(j=0;j<8;j++)

              {

                     if(buffer&mask)

                     {

                            PutPixel(x+j,y+i,color);

                     }

                     mask=mask>>1;

              }

       }

}

void Clear_Screen(unsigned short color)
{
    int i,j;
    for (i=0;i<Ysize;i++) {
        for (j=0;j<Xsize;j++) {
        	PutPixel(j,i,color);
        }
    }
}

void Test_Lcd_480_272_16bpp()
{
	int i;
    Lcd_Init();
    Clear_Screen((unsigned short)(0x00<<11 | 0x00<<5 | 0x00));
    //DrawLine(0  , 0  , 479, 271  , (unsigned short)(0xff<<11 | 0x00<<5 | 0x00));     // 红色
    //for(i=0;i<100;i++);
     Paint_Bmp(pic);
    /*
    unsigned short color = (unsigned short)(0xff<<11 | 0x00<<5 | 0x00);
    Draw_Text16(50,100,color,zhao);

    Draw_Text16(66,100,color,chun);

    Draw_Text16(82,100,color,jiang);

    Draw_ASCII(50,120,color,ASCII_A);

    Draw_ASCII(58,120,color,ASCII_R);

    Draw_ASCII(66,120,color,ASCII_M);
    */

}
