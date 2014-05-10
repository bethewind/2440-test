/**
 * hostif.h
 *
 * @author		daedoo
 * @version         1.0
 */

#ifndef __HOSTIF_H__
#define __HOSTIF_H__


#include "def.h"

// rCTRL REGISTER
#define SET_WAKEUP_BY_CPU		(1<<15)
#define RESET_WAKEUP_BY_CPU	(0<<15)	
#define BLEN(n)					(((n)&0x1FF)<<4)
#define RBURST_DISABLE			(0<<3)
#define RBURST_ENABLE			(1<<3)
#define RESET_SET				(1<<2)
#define DEST_LOWER				(0<<1)
#define DEST_UPPER				(1<<1)
#define TRANS_WR				(0<<0)
#define TRANS_RD				(1<<0)

// INTE REGISTER
#define INT_WFIFO_PEMPTY		(1<<7)
#define INT_WFIFO				(1<<1)
#define INT_RFIFO				(1<<0)


// STATUS REGISTER
#define RFIFO_READY				(1<<0)

#define MAX_BURST_SIZE		256
#define MAX_DATA_SIZE		16383
#define MAX_WBUF_SIZE  		128


#define	HOSTIF_PROTOCOL_BASE		0x10002000

enum {

	rCTRL					= (HOSTIF_PROTOCOL_BASE+0x00),
	rINTE					= (HOSTIF_PROTOCOL_BASE+0x02),
	rSTAT					= (HOSTIF_PROTOCOL_BASE+0x04),

	rCTRL1					= (HOSTIF_PROTOCOL_BASE+0x00),
	rINTE1					= (HOSTIF_PROTOCOL_BASE+0x02),
	rSTAT1					= (HOSTIF_PROTOCOL_BASE+0x04),

	rBSEL					= (HOSTIF_PROTOCOL_BASE+0x06),

	rIMBL					= (HOSTIF_PROTOCOL_BASE+0x00),
	rIMBH					= (HOSTIF_PROTOCOL_BASE+0x02),
	
	rOMBL					= (HOSTIF_PROTOCOL_BASE+0x00),
	rOMBH					= (HOSTIF_PROTOCOL_BASE+0x02),

	rDATA					= (HOSTIF_PROTOCOL_BASE+0x00),
	rDATAL					= (HOSTIF_PROTOCOL_BASE+0x00),
	rDATAH					= (HOSTIF_PROTOCOL_BASE+0x02)
};


enum {
			       // [00], [01], [10]
	BANK0, 		// CTRL, INTE, STAT
	BANK1,		// CTRL1, INTE1, STAT1
	BANK2, 		// IMBL, IMBH
	BANK3,		// OMBL, OMBH
	BANK4, 		// BANK4_00, BANK4_01, BANK4_10
	BANK5,		// BANK5_00, BANK5_01, BANK5_10
	BANK6, 		// Reserved
	BANK7,		// Reserved
	BANK8, 		// DATAL, DATAH
	BANK9, 		// Reserved
	BANK10,		// Reserved
	BANK11,		// SYS_CTRL
	BANK12, 	// Reserved
	BANK13,		// Reserved
	BANK14, 	// Reserved
	BANK15		// Reserved
};

enum DIRECTION
{
	HOSTIF_WRITE, HOSTIF_READ
};


typedef enum
{
	RFIFO				= 0,
	WFIFO				= 1,
	WFIFO_PEMPTY		= 7,
} HOSTIF_INTE;

typedef enum
{
	INT1_OMB_FILLED		= 0,
	INT1_IMB_EMPTY		= 1,
	INT1_ALL				= 3,
} HOSTIF_INTE1;

typedef enum
{
	INT2_ALL				= 0x0007efff,
	INT2_RBURST_DONE		= 18,
	INT2_IMB_FILLED		= 17,
	INT2_OMB_EMPTY		= 16,
	INT2_LFIFO_OVER_RUN	= 7,
	INT2_LFIFO_UNDER_RUN	= 6,
	INT2_WFIFO_OVER_RUN	= 5,
	INT2_WFIFO_UNDER_RUN	= 4,
	INT2_RFIFO_OVER_RUN	= 3,
	INT2_RFIFO_UNDER_RUN	= 2,
	INT2_WBUF_OVER_RUN	= 1,
	INT2_RBUF_UNDER_RUN	= 0,
} HOSTIF_INTE2;

void __irq Isr_HOSTIF_AP(void);
void __irq Isr_HOSTIF_MODEM(void);

void HOSTIF_IntEnable(u16 source);
void HOSTIF_Int1Enable(HOSTIF_INTE1 source);
void HOSTIF_Int2Enable(HOSTIF_INTE2 source);

void HOSTIF_Int1Clear(HOSTIF_INTE1 source);
void HOSTIF_Int2Clear(HOSTIF_INTE2 source);

void HOSTIF_AP_Init(void);
void HOSTIF_MODEM_Init(void);
void HOSTIF_MODEM_To_AP_Reset(void);
void HOSTIF_SelectBank(u16 bsel);

void HOSTIF_SetTotalTransferSize(u16 transfersize);
void HOSTIF_SetMode(u32 dir, u32 nWord, u32 rep);

void HOSTIF_WaitForReady(u8 statusMode);
void HOSTIF_WaitForWriteReady(u32 uWord);

void HOSTIF_WriteData(u32 data);
void HOSTIF_Write(u32 addr, u32 *data, u32 nWord, u32 rep);
void HOSTIF_SingleWrite(u32 addr, u32 data);
void HOSTIF_BurstWrite(u32 addr, u32 *data, u32 nWord);
void HOSTIF_RepeatedBurstWrite(u32 addr, u32 *data, u32 nWord);

u32 HOSTIF_ReadData(void);
u32 HOSTIF_Read(u32 addr, u32 nWord);
u32 HOSTIF_SingleRead(u32 addr);

void HOSTIF_BurstRead(u32 addr, u16 *data, u32 nWord);
void HOSTIF_WriteInMailBox(u32 data);
void HOSTIF_InMailBox(u32 protocol);

void HOSTIF_OutMailBox(u32 protocol);
u32 HOSTIF_ReadOutMailBox(void);

void HOSTIF_TestProtoReg(void);

#endif /*__HOSTIF_H__*/

