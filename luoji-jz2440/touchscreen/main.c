/*
 * main.c
 *
 *  Created on: 2014-2-21
 *      Author: cyj
 */
#include "2440lib.h"
#include "addr2440.h"
#include "init2440.h"
#include "lcd.h"

#define wait_down_int() (rADCTSC = 0xd3)
#define wait_up_int()   (rADCTSC = 0x1d3)
#define mode_auto_xy()  (rADCTSC = 0x0c)
#define Read_Adcdata0() (int)(rADCDAT0 & 0x3ff)
#define Read_Adcdata1() (int)(rADCDAT1 & 0x3ff)
#define PRSCVL(n)       ((n)<<6)

static void Isr_Tc(void)
{
    if (rADCDAT0 & 0x8000)
    {
        Uart_Printf("Stylus Up!!\n");
        wait_down_int();    /* 进入"等待中断模式"，等待触摸屏被按下 */
    }
    else
    {
        Uart_Printf("Stylus Down: ");

        mode_auto_xy();     /* 进入自动(连续) X/Y轴坐标转换模式 */

        /* 设置位[0]为1，启动A/D转换
         * 注意：ADCDLY为50000，PCLK = 50MHz，
         *       要经过(1/50MHz)*50000=1ms之后才开始转换X坐标
         *       再经过1ms之后才开始转换Y坐标
         */
        rADCCON |= 1;
    }

    // 清INT_TC中断
    SUBSRCPND |= (0x1<<9);
}

static void Isr_Adc(void)
{
    // 打印X、Y坐标值
    int x = Read_Adcdata0();
    int y = Read_Adcdata1();
    Uart_Printf("xdata = %d, ydata = %d\n", x, y);
    DrawCross(x,y,(unsigned short)(0xff<<11 | 0x00<<5 | 0x00));

    wait_up_int();

    // 清INT_ADC中断
    SUBSRCPND |= (0x1<<10);
}

void EINT_Handle() {
    int i;
    unsigned long offset = INTOFFSET;
    if (offset == 31) {
        if (SUBSRCPND & (0x1<<9))
            Isr_Tc();

        if (SUBSRCPND & (0x1<<10))
            Isr_Adc();
    }

    SRCPND |= 1<<offset;
    INTPND |= 1<<offset;
}

void Test_TouchScreen()
{
    INTMSK &= ~(0x1<<31);          // 开启ADC总中断
    INTSUBMSK &= ~(0x1<<9);  // 开启INT_TC中断，即触摸屏被按下或松开时产生中断
    INTSUBMSK &= ~(0x1<<10); // 开启INT_ADC中断，即A/D转换结束时产生中断

    rADCCON = (1<<14) | (49<<6);
    rADCDLY = 50000;
}

int main() {

	Uart_SendString("Helloiis\n");
    Lcd_Init();
    Clear_Screen((unsigned short)(0x00<<11 | 0x00<<5 | 0x00));
	Test_TouchScreen();
	wait_down_int();
    Uart_SendString("Helloiis over\n");
    while (1);


    return 0;
}
