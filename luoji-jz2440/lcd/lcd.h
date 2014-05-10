/*
 * lcd.h
 *
 *  Created on: 2014-3-27
 *      Author: cyj
 */

#ifndef LCD_H_
#define LCD_H_

void Lcd_Init();
void DrawLine(int x1,int y1,int x2,int y2,unsigned short color);
void PutPixel(unsigned long x, unsigned long y, unsigned short color);
void Clear_Screen(unsigned short color);
void Test_Lcd_480_272_16bpp();

#endif /* LCD_H_ */
