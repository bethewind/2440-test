//===================================================================
// File Name : Dvstest.c
// Function  : S3C2440A 
// Date      : Feb 02, 2004
// Version   : 0.0
// History
//  0.00: Feb.xx.2004:DonGo: first draft version for DVS.
//===================================================================

#ifndef __DVSTEST_H__
#define __DVSTEST_H__

/**************** Main function *****************/
void Dvs_Test(void);

/**************** Sub function *****************/
void Set_Lcd_Tft_16Bit_240320_Dvs(void);
void __irq Lcd_Int_Frame_ForDvs(void);
void __irq Timer0_intr(void);
void Timer_Setting(void);


void Led_Onoff(int Led_No, int On_Off);

#define LED1	(1<<4)
#define LED2	(1<<5)
#define LED3	(1<<6)
#define LED4	(1<<7)
#define ON		1
#define OFF		0

#endif    //__DVSTEST_H__
