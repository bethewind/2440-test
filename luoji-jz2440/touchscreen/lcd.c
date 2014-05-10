/*
 * S2440lib.c
 *
 *  Created on: 2014-3-12
 *      Author: cyj
 */

#include "addr2440.h"
#include "2440lib.h"
#include "lcd.h"

#define FRAMEBUFFERBASE 0x30400000
#define Xsize 480
#define Ysize 272

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
    LCDCON1 = (4<<8) | (0x3<<5) | (0xc<<1) | 1;
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

//绘制“十”字型

void DrawCross(unsigned long x, unsigned long y, unsigned short color)
{
       int i;
       for(i=x-10;i<x+11;i++)
              PutPixel(i,y, color);
       for(i=y-10;i<y+11;i++)
              PutPixel(x,i, color);
}

void PutPixel(unsigned long x, unsigned long y, unsigned short color)
{
    unsigned short* address;
    address = (unsigned short*)FRAMEBUFFERBASE + (y*Xsize+x);
    *address = color;
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
    Lcd_Init();
    Clear_Screen((unsigned short)(0x00<<11 | 0x00<<5 | 0x00));
    DrawLine(0  , 0  , 479, 271  , (unsigned short)(0xff<<11 | 0x00<<5 | 0x00));     // 红色
}
