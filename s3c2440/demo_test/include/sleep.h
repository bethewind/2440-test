#ifndef __STOP_H__
#define __STOP_H__

//Main function
void Test_SleepMode(void);
void Test_StopMode(void);
void Test_InitSDRAM(void);
void Check_SleepWakeUp(void);
void Check_SleepWakeUp_100Hz(void);
void Test_BattFaultInterrupt(void);

// Sub function
void Test_SleepMode_100Hz(void);
void SetAlarmWakeUp(void);
void ConfigSleepGPIO(void);
void ConfigMiscIO(void);
void RestoreGPIO(void);


static void __irq BattFaultInt(void);

#endif /*__STOP_H__*/
