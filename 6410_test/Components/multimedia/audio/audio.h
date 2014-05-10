#ifndef __AUDIO_H__
#define __AUDIO_H__
 
  
#ifdef __cplusplus
extern "C" {
#endif

#include "def.h"
#include "option.h"
#include "library.h"
#include "sfr6410.h"
#include "system.h"
#include "intc.h"

#define AK2430				1		//SMDK board has not this CODEC
#define WM9713				2		//There is some noise in WM9713 ADC Path 
#define WM8753				3	
#define STAC9767				4
#define WM8580				5
#define WM8990				6

#define WM8580MASTERCLK	CDCLK_I2SCDCLK
#define WM8753MASTERCLK	CDCLK_I2SCDCLK
#define CDCLK_I2SCDCLK		0
#define CDCLK_OSC			1
#define CDCLK_I2SMULTI		2

#define AUDIO_NUM			2
#define WM8753ID			0x34
#define WM8580ID			0x36
#define WM8990ID			0x34	//In SMDK 6400 Rev0.2, IIC Line of WM8753 or WM8580  shoud be disconnected when WM8990 is connected

typedef enum
{
	AUDIO_PORT0 = 0,
	AUDIO_PORT1 = 1
}AUDIO_PORT;

typedef enum
{
	I2SFormat 				= 0,
	MSBJustified 				= 1,
	LSBJustified 				= 2,
	AFTER_PCMSYNC_HIGH 	= 3,
	DURING_PCMSYNC_HIGH 	= 4,
	ACLinkSlot34				= 5,
	ACLinkSlot6				= 6,
}SerialDataFormat;

typedef enum
{
	Master = 0,
	Slave = 1,
}OPMode;

typedef enum
{
	LINEIN 	= 0,
	MICIN	= 1,
}AudioSource;

typedef enum
{
	Word16 	= 0,
	Word8	= 1,
	Word24	= 2,
	Word20	= 3,
}PCMWordLength;

typedef enum
{
	SMDK_I2S 			= 0,
	SMDK_PCM 			= 1,
	SMDK_AC97 			= 2,
	SMDK_I2SMULTI 		= 3,
}SMDKLine;

typedef enum
{
	CodecSource_I2S = 0,
	CodecSource_OSC = 1
}CodecClkSource;



void Audio_InitIICPort(void);
void Audio_CloseIICPort(void);

void WM8753_IICWrite(u8 rhs_uRegAddr, u32 rhs_uData);
void WM8580_IICWrite(u8 rhs_uRegAddr, u32 rhs_uData);
void WM8990_IICWrite(u8 rhs_uRegAddr, u32 rhs_uData);

void STAC9767_InitPCMIn(SerialDataFormat eFormat, u32 eSampleRate, OPMode eOpmode, PCMWordLength eWordLen, AudioSource eSource, SMDKLine eSmdkline);
void STAC9767_InitPCMOut(SerialDataFormat eFormat, u32 eSampleRate, OPMode eOpmode, PCMWordLength eWordLen, SMDKLine eSmdkline);
void WM8753_CodecInitPCMIn(SerialDataFormat eFormat, u32 eSampleRate, OPMode eOpmode, PCMWordLength eWordLen, AudioSource eSource, SMDKLine eSmdkline);
void WM8753_CodecInitPCMOut(SerialDataFormat eFormat, u32 eSampleRate, OPMode eOpmode, PCMWordLength eWordLen, SMDKLine eSmdkline);
void WM8580_CodecCLKOut(u32 uClockout);
void WM8580_CodecInitPCMIn(SerialDataFormat eFormat, u32 eSampleRate, OPMode eOpmode, PCMWordLength eWordLen, AudioSource eSource, SMDKLine eSmdkline);
;void WM8580_CodecInitPCMOut(SerialDataFormat eFormat, u32 eSampleRate, OPMode eOpmode, PCMWordLength eWordLen, SMDKLine eSmdkline);
void WM9713_CodecInitPCMIn(SerialDataFormat eFormat, u32 eSampleRate, OPMode eOpmode, PCMWordLength eWordLen, AudioSource eSource,SMDKLine eSmdkline);
void WM9713_InitPCMOut(SerialDataFormat eFormat, u32 eSampleRate, OPMode eOpmode, PCMWordLength eWordLen, SMDKLine eSmdkline);
void WM8990_CodecInitPCMIn(SerialDataFormat eFormat, u32 eSampleRate, OPMode eOpmode, PCMWordLength eWordLen, AudioSource eSource, SMDKLine eSmdkline);
void WM8990_CodecInitPCMOut(SerialDataFormat eFormat, u32 eSampleRate, OPMode eOpmode, PCMWordLength eWordLen, SMDKLine eSmdkline);
void Parser_32to24(u32* rhs_pStartAddr, u32 rhs_uSize, u32 *rhs_pNewAddr);
void Parser_24to32(u32* rhs_pStartAddr, u32 rhs_uSize, u32 *rhs_pNewAddr);
void Decomp32to24(u32* rhs_pStartAddr, u32 rhs_uSize, u32 *rhs_pNewAddr);
u32* Wave_Parser(u32 uWaveStartAddr, u8* uTotch, u32* uSampleRate, u8* uBitperch, u32* uSize);


#ifdef __cplusplus
}
#endif

#endif /*__AUDIO_H__*/

