#ifndef __COMMON_H__
#define __COMMON_H__
/*��ˮ��ʵ��*/
#define LED1 (1<<10) 		//ֻ�趨������һλ����
#define LED1ON (~(1 << 5))	//Ϊ�˱��������λ��Ӱ�죬��˲�ȡ���ַ�ʽ
#define LED1OFF ((1 << 5))

#define LED2 (1<<12)
#define LED2ON (~(1 << 6))
#define LED2OFF ((1 << 6))

#define LED3 (1<<14)
#define LED3ON (~(1 << 7))
#define LED3OFF ((1 << 7))

#define LED4 (1<<16)
#define LED4ON (~(1 << 8))
#define LED4OFF ((1 << 8))

extern void Led_Init();
extern void Led1_On();
extern void Led1_Off();
extern void Led2_On();
extern void Led2_Off();
extern void Led3_On();
extern void Led3_Off();
extern void Led4_On();
extern void Led4_Off();
extern void Led_Flow();
#endif