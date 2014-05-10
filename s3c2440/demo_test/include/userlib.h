
#ifndef __MAN_SET_H__
#define __MAN_SET_H__

#ifdef __cplusplus
extern "C" {
#endif


// Main function
void Manual_Register_Set(void);

// Sub function
char Uart_Kbhit(void);
void Get_Cmd(char incmd[]);
unsigned int Get_Num(char *string);
void Memory_Dump(void);
void Multimem_Write(void);
void Halfword_Read(void);
void Halfword_Write(void);
void Word_Write(void);
void Word_Write_Or(void);
void Word_Read(void);
void Fill_Memory(void);
void Execute_Precmd(void);
void Halfword_Write_Multi(void);

void Uart_AppendString(char *string);

void Manset_Usage(void);

void user_delay(unsigned int);


unsigned int Init_Ticker(int divider);
int Ticker_Stop(void);
// Watch-dog timer enable & interrupt  disable
#define Ticker_Start()	(rWTCON = (rWTCON & ~(1<<5) | (1<<2)) |(1<<5))
static void __irq Ticker_int(void);

#ifdef __cplusplus
}
#endif


#ifndef DB_LEVEL
	#define	DEBUG_LEVEL		1
#else
	#define	DEBUG_LEVEL 	DB_LEVEL
#endif

#define	MAX_PRECMD_SIZE 10


#endif /*__MAN_SET_H__*/
