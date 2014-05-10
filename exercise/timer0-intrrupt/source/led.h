#ifndef __COMMON_H__
#define __COMMON_H__
/*流水灯实验*/
#define LED1 (1<<10) 		//只需定义是哪一位即可
#define LED1ON (~(1 << 5))	//为了避免对其他位有影响，因此采取这种方式
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