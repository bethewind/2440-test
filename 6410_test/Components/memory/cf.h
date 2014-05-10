#ifndef __CF_H__
#define __CF_H__

#ifdef __cplusplus
extern "C" 
{
#endif

#define	PERF_TEST_PCCARD		1
#define	PERF_TEST_ATA			1
#define	PERF_TEST_PIO			1
#define	PERF_TEST_UDMA		1

#define	INDIRECT_MODE	0	// using EBI path 
#define	DIRECT_MODE	1	// using dedicated path for CF

// define Direct/Indirect Mode
//#define CF_EBI_MODE		INDIRECT_MODE

// ATA, Basic PC card select
#define ATA_MUX				CFCON_BASE+0x1800

#define CF_WRITE_BUF (_DRAM_BaseAddress + 0x1000000)

void Test_ATA(void);
void ATA_Test(void);
void Test_PCCARD(void);		   
void PCCARD_Test(void);		   

/*=========================================================================
 *        	             ATA Commands
 *=========================================================================
 */
typedef enum ATA_COMMANDS
{
	eATA_CMD_IDENTIFYDEVICE		= 0xec,
	eATA_CMD_READSECTOR			= 0x20,
	eATA_CMD_READMULTIPLE		= 0xc4,
	eATA_CMD_READDMA				= 0xc8,
	eATA_CMD_WRITESECTOR		= 0x30,
	eATA_CMD_WRITEMULTIPLE		= 0xc5,
	eATA_CMD_WRITEDMA			= 0xca,
	eATA_CMD_SETFEATURES			= 0xef,
	
}
eATA_COMMANDS;

typedef enum ACCESS_SIZE
{
	eBYTE		= 0,
	eHWORD		= 1,
	eWORD		= 2,
}
eACCESS_SIZE;

typedef enum CF_TASKFILE_Id
{
	eCF_TASKFILE_DATA		= 0,
	eCF_TASKFILE_FEATURE		= 1,
	eCF_TASKFILE_SECTOR		= 2,
	eCF_TASKFILE_LOWLBA		= 3,
	eCF_TASKFILE_MIDLBA		= 4,
	eCF_TASKFILE_HIGHLBA		= 5,
	eCF_TASKFILE_DEVICE		= 6,
	eCF_TASKFILE_STATUS		= 7,
	eCF_TASKFILE_COMMAND 	= 7,
	eCF_TASKFILE_ALTANATE 	= 8,
	eCF_TASKFILE_CONTROL	 	= 8,
}
eCF_TASKFILE_Id;

typedef enum CF_MUX_OUTPUT
{
	eCF_MUX_OUTPUT_ENABLE	= 0,
	eCF_MUX_OUTPUT_DISABLE	= 1,
}
eCF_MUX_OUTPUT;

typedef enum CF_MUX_CARDPWR
{
	eCF_MUX_CARDPWR_ON		= 0,
	eCF_MUX_CARDPWR_OFF	= 1,
}
eCF_MUX_CARDPWR;

typedef enum CF_MUX_IDEMODE
{
	eCF_MUX_MODE_PCCARD		= 0,
	eCF_MUX_MODE_IDE		= 1,
}
eCF_MUX_IDEMODE;
//=========================================================================
//					          	          MUX_REG REGISTER AREA
//=========================================================================
/*---------------------------------- APIs of MUX_REG Registers   ---------------------------------*/
void CF_SetMUXReg(eCF_MUX_OUTPUT, eCF_MUX_CARDPWR, eCF_MUX_IDEMODE);
void CF_SetEBI(u8);
void CF_SetMem(eACCESS_SIZE , u32 , u32 );
#ifdef __cplusplus
}
#endif


#endif // __CF_H__

