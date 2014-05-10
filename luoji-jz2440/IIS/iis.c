/*
 * S2440lib.c
 *
 *  Created on: 2014-3-12
 *      Author: cyj
 */

#include "addr2440.h"
#include "2440lib.h"
#include "iis.h"

//L3接口
#define L3C (1<<4)              //GPB4 = L3CLOCK
#define L3D (1<<3)             //GPB3 = L3DATA
#define L3M (1<<2)             //GPB2 = L3MODE
void WriteL3Data(unsigned char regaddr, unsigned char data) {
    int i, j;
    //start condition: L3M = High L3C = High
    rGPBDAT |= L3M;
    rGPBDAT |= L3C;
    rGPBDAT |= L3M;
    rGPBDAT |= L3C;
    for (i = 0; i < 100; i++)
        ; //delay
    //control register address
    for (j = 0; j < 8; j++) {
        if (regaddr & 0x80) {
            rGPBDAT &= ~L3C;
            rGPBDAT |= L3D;
            for (i = 0; i < 10; i++)
                ; //delay
            rGPBDAT |= L3C;
            for (i = 0; i < 10; i++)
                ; //delay
        } else {
            rGPBDAT &= ~L3C;
            rGPBDAT &= ~L3D;
            for (i = 0; i < 10; i++)
                ; //delay
            rGPBDAT |= L3C;
            for (i = 0; i < 10; i++)
                ; //delay
        }
        regaddr = regaddr << 1;
    }
    //control register data bits
    for (j = 0; j < 8; j++) {
        if (data & 0x80) {
            rGPBDAT &= ~L3C;
            rGPBDAT |= L3D;
            for (i = 0; i < 10; i++)
                ; //delay
            rGPBDAT |= L3C;
            for (i = 0; i < 10; i++)
                ; //delay
        } else {
            rGPBDAT &= ~L3C;
            rGPBDAT &= ~L3D;
            for (i = 0; i < 10; i++)
                ; //delay
            rGPBDAT |= L3C;
            for (i = 0; i < 10; i++)
                ; //delay
        }
        data = data << 1;
    }
    rGPBDAT &= ~L3M;
    for (i = 0; i < 1000; i++)
        ;
    rGPBDAT |= L3M;
    rGPBDAT |= L3C;
}

void playsound(unsigned char *buffer, int length) {

    int count,i;
    char flag;
    // init wm8976
    rGPBDAT = (rGPBDAT & ~(L3M | L3D | L3C)) | (L3M | L3C); //L3Mode=H, L3Clock=H
    WriteL3Data((0x3 << 1) + 1, 0xef); //RMIXEN,LMIXEN,DACENR,DACENL
    WriteL3Data((0x1 << 1) + 0, 0x1f); //biasen,BUFIOEN.VMIDSEL=11b
    WriteL3Data((0x2 << 1) + 1, 0x80); //ROUT1EN LOUT1EN
    WriteL3Data((0x6 << 1) + 0, 0x0); //SYSCLK=MCLK
    WriteL3Data((0x4 << 1) + 0, 0x10); //16bit
    WriteL3Data((0x2B << 1) + 0, 0x10); //BTL OUTPUT
    WriteL3Data((0x9 << 1) + 0, 0x50); //Jack detect enable
    WriteL3Data((0xD << 1) + 0, 0x21); //Jack detect
    WriteL3Data((0x7 << 1) + 0, 0x01); //Jack detect

    //配置s3c2440的IIS寄存器PCLK=50MHZ
    //预分频器为3，所以CDCLK=PCLK/(3+1)=16.928kHz
    rIISPSR = 3 << 5 | 3;
    //无效DMA，输入空闲，预分频器有效
    rIISCON = (0 << 5) | (0 << 4) | (0 << 3) | (1 << 2) | (1 << 1);
    //PCLK为时钟源，输出模式，IIS模式，每个声道16位，CODECLK=256fs，SCLK=32fs
    rIISMOD = (0 << 9) | (0 << 8) | (2 << 6) | (0 << 5) | (0 << 4) | (1 << 3)
            | (0 << 2) | (1 << 0);
    rIISFCON = (0 << 15) | (1 << 13); //输出FIFO正常模式，输出FIFO使能
    flag = 1;
    count = 0;
    //开启IIS
    rIISCON |= 0x1;
    while (flag)
    {
        if ((rIISCON & (1 << 7)) == 0) //检查输出FIFO是否为空
                {
            //FIFO中的数据为16位，深度为32
            //当输出FIFO为空时，一次性向FIFO写入32个16位数据
            for (i = 0; i < 32; i++)
            {
                rIISFIFO=(buffer[2*i+count])+(buffer[2*i+1+count]<<8);
            }
            count += 64;
            if (count > length)
                flag = 0; //音频数据传输完，则退出
        }
    }
    rIISCON = 0x0; //关闭IIS
}
