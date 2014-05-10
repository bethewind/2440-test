#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "audio.h"
#include "def.h"
#include "option.h"
#include "library.h"
#include "sfr6410.h"
#include "system.h"
#include "intc.h"
#include "gpio.h"

#include "iic.h"
#include "ac97.h"


void Audio_InitIICPort(void)
{
	// Set I2C Port
	GPIO_SetPullUpDownEach(eGPIO_B, eGPIO_5, 0);	// Set GPIO PortB pull-up/down disable
	GPIO_SetPullUpDownEach(eGPIO_B, eGPIO_6, 0);	// Set GPIO PortB pull-up/down disable
	GPIO_SetFunctionEach(eGPIO_B, eGPIO_5, 2);		// Set GPIO PortB as I2C SCL
	GPIO_SetFunctionEach(eGPIO_B, eGPIO_6, 2);		// Set GPIO PortB as I2C SDA
}

void Audio_InitIIC(void)
{
	INTC_SetVectAddr(NUM_IIC,Isr_IIC);
	INTC_Enable(NUM_IIC);	
	IIC_Open(100000);
}

void Audio_CloseIICPort(void)
{
	IIC_Close();	
	INTC_Disable(NUM_IIC);
}

void Audio_InitAC97Port(AUDIO_PORT ePort)
{
	if (ePort == AUDIO_PORT0)
	{
		//Set GPD: AC97 Port
		GPIO_SetFunctionAll(eGPIO_D, 0x44444, 0); 
		GPIO_SetPullUpDownAll(eGPIO_D, 0x0);	
	}					
	else if (ePort == AUDIO_PORT1)
	{
		//Set GPE: AC97 Port
		GPIO_SetFunctionAll(eGPIO_E, 0x44444, 0); 
		GPIO_SetPullUpDownAll(eGPIO_E, 0x0);	
	}

	Delay(1000);			
}

void Parser_32to24(u32* rhs_pStartAddr, u32 rhs_uSize, u32 *rhs_pNewAddr)
{
	u8* pBufferAddr = (u8 *)rhs_pNewAddr;				//able to change
	u8* pTempAddr = (u8 *)rhs_pStartAddr;
	s32 uSize = (rhs_uSize/4)*3;						//for word access
	s32 uCnt = 0;
	
	do
	{
		for (uCnt = 0;uCnt < 3;uCnt++)
		{
			*pBufferAddr++ = *pTempAddr++;			
			uSize--;
		}
		pTempAddr++;
	}while(uSize >= 0);
	UART_Printf("End Address is 0x%x\n", (u32) pBufferAddr);
}

void Parser_24to32(u32* rhs_pStartAddr, u32 rhs_uSize, u32 *rhs_pNewAddr)
{
	u8* pBufferAddr = (u8 *)rhs_pNewAddr;				//able to change
	u8* pTempAddr = (u8 *)rhs_pStartAddr;
	s32 uSize = (rhs_uSize/3)*4;						//for word access
	s32 uCnt = 0;
	
	do
	{
		for (uCnt = 0;uCnt < 3;uCnt++)
		{
			*pBufferAddr++ = *pTempAddr++;			
			uSize--;
		}
		*pBufferAddr++ = 0;
	}while(uSize >= 0);
}

void Decomp32to24(u32* rhs_pStartAddr, u32 rhs_uSize, u32 *rhs_pNewAddr)
{
	u32* pBufferAddr = rhs_pNewAddr;				//able to change
	u32* pTempAddr = rhs_pStartAddr;
	s32 sSize = rhs_uSize;
	u32 uTemp;	
	
	do
	{
		uTemp = *pTempAddr;
		uTemp = uTemp >> 8;
		*pBufferAddr++ = uTemp;
		sSize = sSize - 4;		
	}while(sSize >= 0);

}

//////////
// Function Name : WM8753_IICWrite
// Function Desctiption : CMD to WM8753 Codec using IIC I/F
// Input : RegAddr(7bit), Data(9bit)
// Output : None
// Version : 0.0
// Author : Sung-Hyun, Na

void WM8753_IICWrite(u8 rhs_uRegAddr, u32 rhs_uData)
{
	u8 ucRegAddr, ucData;
	ucData = (u8)(rhs_uData & 0xff);
	ucRegAddr = ((rhs_uData & 0x100) >> 8) | (rhs_uRegAddr << 1);
	IIC_Write(WM8753ID, ucRegAddr, ucData);			//Codec ID : 0x34	
}

//////////
// Function Name : WM8753_IICWrite
// Function Desctiption : CMD to WM8753 Codec using IIC I/F
// Input : RegAddr(7bit), Data(9bit)
// Output : None
// Version : 0.0
// Author : Sung-Hyun, Na

void WM8580_IICWrite(u8 rhs_uRegAddr, u32 rhs_uData)
{
	u8 ucRegAddr, ucData;
	ucData = (u8)(rhs_uData & 0xff);
	ucRegAddr = ((rhs_uData & 0x100) >> 8) | (rhs_uRegAddr << 1);
	IIC_Write(WM8580ID, ucRegAddr, ucData);			//Codec ID : 0x34
}

void WM8990_IICWrite(u8 rhs_uRegAddr, u32 rhs_uData)
{
	//u8 ucRegAddr;
	//u16 ucData;
	//ucData = (u8)(rhs_uData & 0xff);
	//ucRegAddr = ((rhs_uData & 0x100) >> 8) | (rhs_uRegAddr << 1);
	IIC_Write16(WM8990ID, rhs_uRegAddr, (u16) rhs_uData);			//Codec ID : 0x34
}

void WM8990_IICRead(u8 rhs_uRegAddr, u32* rhs_upData)
{

}


void STAC9767_InitPCMIn(SerialDataFormat eFormat, u32 eSampleRate, OPMode eOpmode, PCMWordLength eWordLen, AudioSource eSource, SMDKLine eSmdkline)
{
	AC97_CodecCmd(WRITE,0x00,0x683F);			//codec soft reset 			
	AC97_CodecCmd(WRITE,0x2A,0x0001);		//variable rate enable	
	UART_Printf("VRA Enable(1)/Disable(0): 0x%x\n",(0x1&AC97_CodecCmd(READ,0x2A,0x0001)));

	if(eSampleRate == 8000)					//VRA On: 8KHz	
		AC97_CodecCmd(WRITE,0x32,0x1f40);		
	else if(eSampleRate == 44100)				//VRA On: 44.1KHz
		AC97_CodecCmd(WRITE,0x32,0xac44);		
	else if(eSampleRate == 22050)				//VRA On: 22.05KHz
		AC97_CodecCmd(WRITE,0x32,0x5622);	 	
	else 									//VRA Off: 48KHz
		AC97_CodecCmd(WRITE,0x32,0xbb80);	
	AC97_CodecCmd(WRITE,0x26,(1<<9));		//all power on except DAC Block
	UART_Printf("\nAC97 Codec 0x26 Reg.: 0x%x\n\n", AC97_CodecCmd(READ,0x26,0x0000));
	if ( eSource == LINEIN)
	{
		AC97_CodecCmd(WRITE,0x10,0x1010);			//line in volume on
		AC97_CodecCmd(WRITE,0x6e,0x0000);			//All Analog Mode, ADC Input select => left slot3, right slot4
		AC97_CodecCmd(WRITE,0x1a,0x0505);			//record source select => Stereo Mix
		AC97_CodecCmd(WRITE,0x1c,0x0909);			//record gain is initial
		AC97_CodecCmd(WRITE,0x78,0x0001);			//ADC HPF Bypass
		AC97_CodecCmd(WRITE,0x20,0x0000);			//General Reg.
	}
	else 
	{
		AC97_CodecCmd(WRITE,0x20,0x0000);			//MIC1 Selected
		AC97_CodecCmd(WRITE,0x6e,0x0024);			//ADC Input Slot => left slot6, right slot9, MIC GAIN VAL =1
		AC97_CodecCmd(WRITE,0x0e,0x0040);			//BOOSTEN =1
		AC97_CodecCmd(WRITE,0x1a,0x0000);			//Left, Right => MIC
		AC97_CodecCmd(WRITE,0x1c,0xff);		
		AC97_CodecCmd(WRITE,0x78,0x0001);			//ADC HPF Bypass
	}		
}

void STAC9767_InitPCMOut(SerialDataFormat eFormat, u32 eSampleRate, OPMode eOpmode, PCMWordLength eWordLen, SMDKLine eSmdkline)
{	
	
	AC97_CodecCmd(WRITE,0x00,0x683F);		//codec soft reset 	
	
	AC97_CodecCmd(WRITE,0x2A,0x0001);	//variable rate enable	
	//UART_Printf("\nVRA Enable(1)/Disable(0): 0x%x\n", (0x1&AC97_CodecCmd(READ,0x2A,0x0001)));

	if(eSampleRate==8000){
	//DAC Sampling frequency 8kHz
	AC97_CodecCmd(WRITE,0x2C,0x1f40);
	}
	else if(eSampleRate==48000){
	//DAC Sampling frequency 48kHz
	AC97_CodecCmd(WRITE,0x2C,0xbb80);
	}
	else if(eSampleRate==44100){
	//DAC Sampling frequency 44.1kHz
	AC97_CodecCmd(WRITE,0x2C,0xac44);
	}
	else if(eSampleRate==22050){
	//DAC Sampling frequency 22.05kHz
	AC97_CodecCmd(WRITE,0x2C,0x5622);		
	}

	AC97_CodecCmd(WRITE,0x26, (1<<8));	// all power on except ADC blcok
	UART_Printf("AC97 Codec 0x26 Reg.: 0x%x\n\n", AC97_CodecCmd(READ,0x26,0x0000));

	AC97_CodecCmd(WRITE,0x18,0x0000);		// PCM out volume on
	AC97_CodecCmd(WRITE,0x20,0x0000);		// general purpose
	AC97_CodecCmd(WRITE,0x04,0x1A1A);	// Aux out(HP out) volume on
	
	AC97_CodecCmd(READ,0x04,0x00000);	//HP out volume 
	}


//////////
// Function Name : 
// Function Desctiption : 
// Input : Serial Data Format, Sample Rate, Operation mode, PCM Word Length, SMDK Line CodecClk Source
// Output : None
// Version : 0.0
// Author : Sunghyun, Na
void WM8753_CodecInitPCMIn(SerialDataFormat eFormat, u32 eSampleRate, OPMode eOpmode, PCMWordLength eWordLen, AudioSource eSource, SMDKLine eSmdkline)
{	
	Audio_InitIICPort();
	Audio_InitIIC();
	WM8753_IICWrite(0x1f, 0x000);	//Reset
	WM8753_IICWrite(0x01, 0x008);	//DAC Control Mute	
	WM8753_IICWrite(0x02, 0x000);	//ADC Control
	
	//PCM Audio I/F 16bit
	if (eFormat == DURING_PCMSYNC_HIGH)
	{
		WM8753_IICWrite(0x03, 0x1b3);
		WM8753_IICWrite(0x04, 0x002);	//HiFi Audio I/F	
	}
	else if (eFormat == AFTER_PCMSYNC_HIGH)
	{
		WM8753_IICWrite(0x03, 0x1a3);
		WM8753_IICWrite(0x04, 0x002);	//HiFi Audio I/F	
	}
	
	// I2S I/F 16bit, 24bit
	else if (eFormat == I2SFormat)
	{
		
		if ( eWordLen == Word24)
		{
			WM8753_IICWrite(0x03, 0x10a | (u32) eOpmode << 6);	//Voice I/F
			WM8753_IICWrite(0x04, 0x00a | (u32) eOpmode << 6);	//HiFi Audio I/F	
		}
		else if ( eWordLen == Word16)
		{
			WM8753_IICWrite(0x03, 0x102 | (u32) eOpmode << 6);	//Voice I/F
			WM8753_IICWrite(0x04, 0x002 | (u32) eOpmode << 6);	//HiFi Audio I/F
		}
	}
	else if (eFormat == MSBJustified)
	{
		
		if ( eWordLen == Word24)
		{
			WM8753_IICWrite(0x03, 0x109 | (u32) eOpmode << 6);	//Voice I/F
			WM8753_IICWrite(0x04, 0x009 | (u32) eOpmode << 6);	//HiFi Audio I/F
		}
		else if ( eWordLen == Word16)
		{
			WM8753_IICWrite(0x03, 0x101 | (u32) eOpmode << 6);	//Voice I/F
			WM8753_IICWrite(0x04, 0x001 | (u32) eOpmode << 6);	//HiFi Audio I/F
		}

	}
	else if (eFormat == LSBJustified)
	{
		if ( eWordLen == Word24)
		{
			WM8753_IICWrite(0x03, 0x108 | (u32) eOpmode << 6);	//Voice I/F
			WM8753_IICWrite(0x04, 0x008 | (u32) eOpmode << 6);	//HiFi Audio I/F
		}
		else if ( eWordLen == Word16)
		{
			WM8753_IICWrite(0x03, 0x100 | (u32) eOpmode << 6);	//Voice I/F
			WM8753_IICWrite(0x04, 0x000 | (u32) eOpmode << 6);	//HiFi Audio I/F
		}
	}	
	
	

	if ( eOpmode == Master)
	{
		if (eSmdkline ==  SMDK_PCM)
			WM8753_IICWrite(0x05, 0x000);	//Interface Control Voice Codec and Direction of VFX(I2SLRCLK) 
		else
			WM8753_IICWrite(0x05, 0x008);	//Interface Control HiFI AUDIO DAC with Voice Codec ADC	and Direction of LRC(I2SLRCLK) 
			
		WM8753_IICWrite(0x07, 0x097); 	//Sample Rate Control (2), Don't care in case of slave mode
		#if (WM8753MASTERCLK == CDCLK_I2SCDCLK)
		WM8753_IICWrite(0x34, 0x004);	//Clock Control << Clock for Voice Codec = MCLK or PLL1 Voice Codec same as HiFi DAC
		if ( eSampleRate == 48000)		WM8753_IICWrite(0x06, 0x000);	//CDCLK : 24.576MHz, Sync Rate : 48KHz
		else if (eSampleRate == 44100)		WM8753_IICWrite(0x06, 0x020);	//CDCLK : 16.934MHz, Sync Rate : 44.1KHz	0x022			
		else 							WM8753_IICWrite(0x06, 0x000);
		#elif (WM8753MASTERCLK == CDCLK_OSC)		
		WM8753_IICWrite(0x34, 0x004);	//Clock Control << Clock for Voice Codec = MCLK or PLL1 Voice Codec same as HiFi DAC			 		
		#endif		
	}
	else 
	{
		if (eSmdkline ==  SMDK_PCM)
			WM8753_IICWrite(0x05, 0x042);	//Interface Control Voice Codec and Direction of VFX(I2SLRCLK) 
		else
			WM8753_IICWrite(0x05, 0x089);	//Interface Control HiFI AUDIO DAC with Voice Codec ADC	and Direction of LRC(I2SLRCLK) 
		
		#if 1
		WM8753_IICWrite(0x34, 0x004);	//Clock Control << Clock for Voice Codec = MCLK for Voice Codec and HiFi DAC
		WM8753_IICWrite(0x35, 0x000);	//PLL1 Control (1)		
		WM8753_IICWrite(0x36, 0x083);	//PLL1 Control (2)
		WM8753_IICWrite(0x37, 0x024);	//PLL1 Control (3)
		WM8753_IICWrite(0x38, 0x1ba);	//PLL1 Control (4)
		#else
		WM8753_IICWrite(0x34, 0x014);	//Clock Control << Clock for Voice Codec = PLL1 for Voice Codec and HiFi DAC
		WM8753_IICWrite(0x35, 0x047);	//PLL1 Control (1) : PLL1 Enable Fin = 12MHz, Fout = 12.288MHz 
		WM8753_IICWrite(0x36, 0x103);	//PLL1 Control (2) : PLL1 N Value & K Value N : 0x8 K : 0xC49BA
		WM8753_IICWrite(0x37, 0x024);	//PLL1 Control (3) : PLL1 K Value
		WM8753_IICWrite(0x38, 0x1ba);	//PLL1 Control (4) : PLL1 K Value			
		#endif
		
		WM8753_IICWrite(0x39, 0x000);	//PLL2 Control (1)
		WM8753_IICWrite(0x3a, 0x083);	//PLL2 Control (2)
		WM8753_IICWrite(0x3b, 0x024);	//PLL2 Control (3)
		WM8753_IICWrite(0x3c, 0x1ba);	//PLL2 Control (4)		

		// Normal Mode for 12.288MHz In
		switch(eSampleRate)
		{
			case 8000:
				WM8753_IICWrite(0x07, 0x127); 	//Sample Rate Control (2) In Master Mode it is decided that divider of I2SSCLK(VXCLK, BCLK) : I2SSCLK = I2SCDCLK / 4 0x093 /8 0x0df / 16 0x127 
				WM8753_IICWrite(0x06, 0x00c);	//Sample Rate Control 		12.288MHz In, 8Khz
				break;
			case 11025:
				WM8753_IICWrite(0x07, 0x127); 	//Sample Rate Control (2) In Master Mode it is decided that divider of I2SSCLK(VXCLK, BCLK) : I2SSCLK = I2SCDCLK / 4 0x093 /8 0x0df / 16 0x127 
				WM8753_IICWrite(0x06, 0x030);	//Sample Rate Control 		12.288MHz In, 11.025Khz
				break;
			case 16000:

				WM8753_IICWrite(0x07, 0x127); 	//Sample Rate Control (2) In Master Mode it is decided that divider of I2SSCLK(VXCLK, BCLK) : I2SSCLK = I2SCDCLK / 4 0x093 /8 0x0df / 16 0x127 
				WM8753_IICWrite(0x06, 0x014);	//Sample Rate Control 		12.288MHz In, 16Khz
				break;
			case 22050:
				WM8753_IICWrite(0x07, 0x127); 	//Sample Rate Control (2) In Master Mode it is decided that divider of I2SSCLK(VXCLK, BCLK) : I2SSCLK = I2SCDCLK / 4 0x093 /8 0x0df / 16 0x127 
				WM8753_IICWrite(0x06, 0x032);	//Sample Rate Control 		12.288MHz In, 22.05Khz
				break;
			case 32000:

				WM8753_IICWrite(0x07, 0x0df); 	//Sample Rate Control (2) In Master Mode it is decided that divider of I2SSCLK(VXCLK, BCLK) : I2SSCLK = I2SCDCLK / 4 0x093 /8 0x0df / 16 0x127 
				WM8753_IICWrite(0x06, 0x018);	//Sample Rate Control 		12.288MHz In, 32Khz
				break;
			case 44100:
				WM8753_IICWrite(0x07, 0x093); 	//Sample Rate Control (2) In Master Mode it is decided that divider of I2SSCLK(VXCLK, BCLK) : I2SSCLK = I2SCDCLK / 4 0x093 /8 0x0df / 16 0x127 
				WM8753_IICWrite(0x06, 0x020);	//Sample Rate Control 		12.288MHz In, 44.1Khz
				break;
			case 48000:
				WM8753_IICWrite(0x07, 0x093); 	//Sample Rate Control (2) In Master Mode it is decided that divider of I2SSCLK(VXCLK, BCLK) : I2SSCLK = I2SCDCLK / 4 0x093 /8 0x0df / 16 0x127 
				WM8753_IICWrite(0x06, 0x000);	//Sample Rate Control 		12.288MHz In, 48Khz
				break;
			case 88200:

				WM8753_IICWrite(0x07, 0x003); 	//Sample Rate Control (2) In Master Mode it is decided that divider of I2SSCLK(VXCLK, BCLK) : I2SSCLK = I2SCDCLK /1 0x003 / 4 0x093 /8 0x0df / 16 0x127 
				WM8753_IICWrite(0x06, 0x03c);	//Sample Rate Control 		12.288MHz In, 88.2Khz
				break;
			case 96000:
				WM8753_IICWrite(0x07, 0x003); 	//Sample Rate Control (2) In Master Mode it is decided that divider of I2SSCLK(VXCLK, BCLK) : I2SSCLK = I2SCDCLK / 4 0x093 /8 0x0df / 16 0x127 
				WM8753_IICWrite(0x06, 0x01c);	//Sample Rate Control 		12.288MHz In, 96Khz
				break;
			default:
				WM8753_IICWrite(0x07, 0x0df); 	//Sample Rate Control (2) In Master Mode it is decided that divider of I2SSCLK(VXCLK, BCLK) : I2SSCLK = I2SCDCLK / 4 0x093 /8 0x0df / 16 0x127 
				WM8753_IICWrite(0x06, 0x000);	//Sample Rate Control 		12.288MHz In, 48Khz
				break;
		}
	}
	
	WM8753_IICWrite(0x08, 0x0ff); 	//Left DAC Volume
	WM8753_IICWrite(0x09, 0x0ff); 	//Right DAC Volume
	WM8753_IICWrite(0x0c, 0x07b); 	//ALC 1
	WM8753_IICWrite(0x0d, 0x000); 	//ALC 2
	WM8753_IICWrite(0x0e, 0x032); 	//ALC 3
	WM8753_IICWrite(0x0f, 0x000); 	//Noise Gate 
	WM8753_IICWrite(0x10, 0x00f); 	//Left ADC Volume
	WM8753_IICWrite(0x11, 0x00f); 	//Right ADC Volume
	WM8753_IICWrite(0x12, 0x0c0); 	//Additional Control
	WM8753_IICWrite(0x13, 0x000); 	//Three D Control
	
	if (eSource == MICIN)
	{
		WM8753_IICWrite(0x14, 0x0e0); 	//Power Management(1) << Power up VMID[50K ohm]; VREF >>
		WM8753_IICWrite(0x15, 0x1ff); 	//Power Management(2) << Power up ADCL/R >>
	}
	else 
	{
		WM8753_IICWrite(0x14, 0x0c0); 	//Power Management(1) << Power up VMID[50K ohm]; VREF >>
		WM8753_IICWrite(0x15, 0x00c); 	//Power Management(2) << Power up ADCL/R >>
	}
	
	WM8753_IICWrite(0x16, 0x000); 	//Power Management(3)
	WM8753_IICWrite(0x17, 0x000); 	//Power Management(4)
	WM8753_IICWrite(0x18, 0x000); 	//ID Register
	WM8753_IICWrite(0x19, 0x000); 	//Codec Interupt Polarty
	WM8753_IICWrite(0x1a, 0x000);	//Codec Interupt Enable
	WM8753_IICWrite(0x1b, 0x000);	//GPIO Control (1)
	WM8753_IICWrite(0x1c, 0x000);	//GPIO Control (2)

	WM8753_IICWrite(0x20, 0x055);	//Record Mix (1)
	WM8753_IICWrite(0x21, 0x005);	//Record Mix (2)
	WM8753_IICWrite(0x22, 0x050);	//Left Out Mix(1)
	WM8753_IICWrite(0x23, 0x055);	//Left Out Mix(2)
	WM8753_IICWrite(0x24, 0x050);	//Rigth Out Mix(1)
	WM8753_IICWrite(0x25, 0x055);	//Right Out Mix(2)
	WM8753_IICWrite(0x26, 0x050);	//Mono Out Mix(1)
	WM8753_IICWrite(0x27, 0x055);	//Mono Out Mix(2)

	WM8753_IICWrite(0x28, 0x079);	//LOUT1 Volume
	WM8753_IICWrite(0x29, 0x079);	//ROUT1 Volume
	WM8753_IICWrite(0x2a, 0x079);	//LOUT2 Volume
	WM8753_IICWrite(0x2b, 0x079);	//ROUT2 Volume
	WM8753_IICWrite(0x2c, 0x079);	//MONO Out
	WM8753_IICWrite(0x2d, 0x000);	//Output Control
	WM8753_IICWrite(0x2e, 0x005);	//ADC Input Mode << Set L/R ADC input select to Line 1/2 '01' >>
	
	// MIC IN
	// WM8753_IICWrite(0x2f, 0x060);	//Input Control (1)
	// WM8753_IICWrite(0x30, 0x002);	//Input Control (2)
	// WM8753_IICWrite(0x33, 0x001);	//MIC Bias Comp Control 

	// LINE IN
	WM8753_IICWrite(0x2f, 0x000);	//Input Control (1)
	WM8753_IICWrite(0x30, 0x000);	//Input Control (2)
	WM8753_IICWrite(0x33, 0x000);	//MIC Bias Comp Control 	
	
	WM8753_IICWrite(0x31, 0x097);	//Left Input Volume
	WM8753_IICWrite(0x32, 0x097);	//Right Input Volume	
	
	WM8753_IICWrite(0x3d, 0x000);	//Bias Control
	WM8753_IICWrite(0x3f, 0x000);	//Additional Control
	
	Audio_CloseIICPort();
	
}

void WM8753_CodecInitPCMOut(SerialDataFormat eFormat, u32 eSampleRate, OPMode eOpmode, PCMWordLength eWordLen, SMDKLine eSmdkline)
{
	Audio_InitIICPort();	
	Audio_InitIIC();
	
	WM8753_IICWrite(0x1f, 0x000);	//Reset

	WM8753_IICWrite(0x14, 0x0dc);	//Power Management(1)
	WM8753_IICWrite(0x17, 0x003);	//Power Management(4)
	Delay(1000);
	
	WM8753_IICWrite(0x01, 0x000);	//DAC Control No Mute
	WM8753_IICWrite(0x02, 0x000);	//ADC Control

	if (eFormat == DURING_PCMSYNC_HIGH)
	{
		WM8753_IICWrite(0x03, 0x1b3);
		WM8753_IICWrite(0x04, 0x0b3);	//HiFi Audio I/F	 
	}
	else if (eFormat == AFTER_PCMSYNC_HIGH)
	{
		WM8753_IICWrite(0x03, 0x1a3);
		WM8753_IICWrite(0x04, 0x0a3);	//HiFi Audio I/F
	}
	else if (eFormat == I2SFormat)
	{
		
		if ( eWordLen == Word24)
		{
			WM8753_IICWrite(0x03, 0x10a |(u32) eOpmode << 6);	//Voice I/F
			WM8753_IICWrite(0x04, 0x00a |(u32) eOpmode << 6);	//HiFi Audio I/F	
		}
		else if ( eWordLen == Word16)
		{
			WM8753_IICWrite(0x03, 0x102 |(u32) eOpmode << 6);	//Voice I/F
			WM8753_IICWrite(0x04, 0x002 |(u32) eOpmode << 6);	//HiFi Audio I/F
		}
	}
	else if (eFormat == MSBJustified)
	{
		
		if ( eWordLen == Word24)
		{
			WM8753_IICWrite(0x03, 0x109 |(u32) eOpmode << 6);	//Voice I/F
			WM8753_IICWrite(0x04, 0x009 |(u32) eOpmode << 6);	//HiFi Audio I/F
		}
		else if ( eWordLen == Word16)
		{
			WM8753_IICWrite(0x03, 0x101 |(u32) eOpmode << 6);	//Voice I/F
			WM8753_IICWrite(0x04, 0x001 |(u32) eOpmode << 6);	//HiFi Audio I/F
		}

	}
	else if (eFormat == LSBJustified)
	{
		if ( eWordLen == Word24)
		{
			WM8753_IICWrite(0x03, 0x108 |(u32) eOpmode << 6);	//Voice I/F
			WM8753_IICWrite(0x04, 0x008 |(u32) eOpmode << 6);	//HiFi Audio I/F
		}
		else if ( eWordLen == Word16)
		{
			WM8753_IICWrite(0x03, 0x100 |(u32) eOpmode << 6);	//Voice I/F
			WM8753_IICWrite(0x04, 0x000 |(u32) eOpmode << 6);	//HiFi Audio I/F
		}
	}	
	
	if ( eOpmode == Master)				//Codec Slave 
	{
		if (eSmdkline ==  SMDK_PCM)
		{
			WM8753_IICWrite(0x05, 0x000);	//Interface Control Voice Codec and Direction of VFX(I2SLRCLK) 
			WM8753_IICWrite(0x20, 0x055);	//Record Mix (1)
			WM8753_IICWrite(0x21, 0x005);	//Record Mix (2)
			WM8753_IICWrite(0x22, 0x050);	//Left Out Mix(1)
			WM8753_IICWrite(0x23, 0x155);	//Left Out Mix(2)
			WM8753_IICWrite(0x24, 0x050);	//Rigth Out Mix(1)
			WM8753_IICWrite(0x25, 0x155);	//Right Out Mix(2)
			WM8753_IICWrite(0x26, 0x050);	//Mono Out Mix(1)
			WM8753_IICWrite(0x27, 0x055);	//Mono Out Mix(2)
		}
		else
		{
			WM8753_IICWrite(0x05, 0x008);	//Interface Control HiFI AUDIO DAC with Voice Codec ADC	and Direction of LRC(I2SLRCLK) 
			WM8753_IICWrite(0x20, 0x055);	//Record Mix (1)
			WM8753_IICWrite(0x21, 0x005);	//Record Mix (2)
			WM8753_IICWrite(0x22, 0x150);	//Left Out Mix(1)
			WM8753_IICWrite(0x23, 0x055);	//Left Out Mix(2)
			WM8753_IICWrite(0x24, 0x150);	//Rigth Out Mix(1)
			WM8753_IICWrite(0x25, 0x055);	//Right Out Mix(2)
			WM8753_IICWrite(0x26, 0x050);	//Mono Out Mix(1)
			WM8753_IICWrite(0x27, 0x055);	//Mono Out Mix(2)
		}
			
			WM8753_IICWrite(0x07, 0x097); 	//Sample Rate Control (2), Don't care in case of slave mode
		#if (WM8753MASTERCLK == CDCLK_I2SCDCLK)				
			WM8753_IICWrite(0x34, 0x004);	//Clock Control << Clock for Voice Codec = MCLK or PLL1 Voice Codec same as HiFi DAC			
			
			if ( eSampleRate == 48000)		WM8753_IICWrite(0x06, 0x000);	//Sync Rate Control 512fs 	CDCLK : 24.576MHz, Sync Rate : 48KHz
			else if (eSampleRate == 44100)		WM8753_IICWrite(0x06, 0x020);	//Sync Rate Control 384fs 	CDCLK : 16.934MHz, Sync Rate : 44.1KHz
			
		#elif (WM8753MASTERCLK == CDCLK_OSC)				//16.9344 OSC on SMDK 6400 Rev0.2	
			
			WM8753_IICWrite(0x34, 0x004);	//Clock Control << Clock for Voice Codec = MCLK or PLL1 Voice Codec same as HiFi DAC
			WM8753_IICWrite(0x35, 0x000);	//PLL1 Control (1)
			WM8753_IICWrite(0x36, 0x083);	//PLL1 Control (2)
			WM8753_IICWrite(0x37, 0x024);	//PLL1 Control (3)
			WM8753_IICWrite(0x38, 0x1ba);	//PLL1 Control (4)

			WM8753_IICWrite(0x39, 0x000);	//PLL2 Control (1)
			WM8753_IICWrite(0x3a, 0x083);	//PLL2 Control (2)
			WM8753_IICWrite(0x3b, 0x024);	//PLL2 Control (3)
			WM8753_IICWrite(0x3c, 0x1ba);	//PLL2 Control (4)

			//12MHz Oscillator
			if (eSampleRate == 8000)
				WM8753_IICWrite(0x06, ( (0x3<<1) |(1<<0) ));
			else if (eSampleRate == 11025) 
				WM8753_IICWrite(0x06, ( (0x19<<1) | (1<<0) ));
			else if (eSampleRate == 16000) 
				WM8753_IICWrite(0x06, ( (0x0a<<1) | (1<<0) ));
			else if (eSampleRate == 22050) 
				WM8753_IICWrite(0x06, ( (0x1b<<1) | (1<<0) ));
			else if (eSampleRate == 32000) 
				WM8753_IICWrite(0x06, ( (0x0c<<1) | (1<<0) ));
			else if (eSampleRate == 44100) 
				WM8753_IICWrite(0x06, ( (0x11<<1) | (1<<0) ));
			else if (eSampleRate == 48000) 
				WM8753_IICWrite(0x06, ( (0x0<<1) | (1<<0) ));
			else if (eSampleRate == 88200) 
				WM8753_IICWrite(0x06, ( (0x1f<<1) | (1<<0) ));
			else if (eSampleRate == 96000) 
				WM8753_IICWrite(0x06, ( (0x0e<<1) | (1<<0) ));
			else
				WM8753_IICWrite(0x06, ( (0x00<<1) | (1<<0) ));		
		#endif
	}

	else									// Codec Master  Clk Source is 12 MHz OSCIN from 6410 on I2SCDCLK
	{
		if (eSmdkline ==  SMDK_PCM)
		{
			WM8753_IICWrite(0x05, 0x042);	//Interface Control Voice Codec and Direction of VFX(I2SLRCLK) 		0x042
			WM8753_IICWrite(0x20, 0x055);	//Record Mix (1)
			WM8753_IICWrite(0x21, 0x005);	//Record Mix (2)
			WM8753_IICWrite(0x22, 0x050);	//Left Out Mix(1)
			WM8753_IICWrite(0x23, 0x155);	//Left Out Mix(2)
			WM8753_IICWrite(0x24, 0x050);	//Rigth Out Mix(1)
			WM8753_IICWrite(0x25, 0x155);	//Right Out Mix(2)
			WM8753_IICWrite(0x26, 0x050);	//Mono Out Mix(1)
			WM8753_IICWrite(0x27, 0x055);	//Mono Out Mix(2)
		}
		
		else

		{
			WM8753_IICWrite(0x05, 0x089);	//Interface Control HiFI AUDIO DAC with Voice Codec ADC	and Direction of LRC(I2SLRCLK) 0x089
			WM8753_IICWrite(0x20, 0x055);	//Record Mix (1)
			WM8753_IICWrite(0x21, 0x005);	//Record Mix (2)
			WM8753_IICWrite(0x22, 0x150);	//Left Out Mix(1)
			WM8753_IICWrite(0x23, 0x055);	//Left Out Mix(2)
			WM8753_IICWrite(0x24, 0x150);	//Rigth Out Mix(1)
			WM8753_IICWrite(0x25, 0x055);	//Right Out Mix(2)
			WM8753_IICWrite(0x26, 0x050);	//Mono Out Mix(1)
			WM8753_IICWrite(0x27, 0x055);	//Mono Out Mix(2)		
		}	
		
		
		#if 1							// No use PLL1 
		WM8753_IICWrite(0x34, 0x004);	//Clock Control << Clock for Voice Codec = MCLK for Voice Codec and HiFi DAC		
		WM8753_IICWrite(0x35, 0x000);	//PLL1 Control (1)		
		WM8753_IICWrite(0x36, 0x083);	//PLL1 Control (2)
		WM8753_IICWrite(0x37, 0x024);	//PLL1 Control (3)
		WM8753_IICWrite(0x38, 0x1ba);	//PLL1 Control (4)
		#else							// Using PLL1
		WM8753_IICWrite(0x34, 0x014);	//Clock Control << Clock for Voice Codec = PLL1 for Voice Codec and HiFi DAC
		WM8753_IICWrite(0x35, 0x047);	//PLL1 Control (1) : PLL1 Enable Fin = 12MHz, Fout = 12.288MHz 
		WM8753_IICWrite(0x36, 0x103);	//PLL1 Control (2) : PLL1 N Value & K Value N : 0x8 K : 0xC49BA
		WM8753_IICWrite(0x37, 0x024);	//PLL1 Control (3) : PLL1 K Value
		WM8753_IICWrite(0x38, 0x1ba);	//PLL1 Control (4) : PLL1 K Value			
		#endif 	
		WM8753_IICWrite(0x39, 0x000);	//PLL2 Control (1)
		WM8753_IICWrite(0x3a, 0x083);	//PLL2 Control (2)
		WM8753_IICWrite(0x3b, 0x024);	//PLL2 Control (3)
		WM8753_IICWrite(0x3c, 0x1ba);	//PLL2 Control (4)				
		switch(eSampleRate)
		{
			case 8000:
				WM8753_IICWrite(0x07, 0x127); 	//Sample Rate Control (2) In Master Mode it is decided that divider of I2SSCLK(VXCLK, BCLK) : I2SSCLK = I2SCDCLK / 4 0x093 /8 0x0df / 16 0x127
				WM8753_IICWrite(0x06, 0x00c);	//Sample Rate Control 	12.288MHz In, 8Khz
				break;
			case 11025:
				WM8753_IICWrite(0x07, 0x127); 	//Sample Rate Control (2) In Master Mode it is decided that divider of I2SSCLK(VXCLK, BCLK) : I2SSCLK = I2SCDCLK / 4 0x093 /8 0x0df / 16 0x127
				WM8753_IICWrite(0x06, 0x030);	//Sample Rate Control 	12.288MHz In, 11.025Khz
				break;
			case 16000:

				WM8753_IICWrite(0x07, 0x127); 	//Sample Rate Control (2) In Master Mode it is decided that divider of I2SSCLK(VXCLK, BCLK) : I2SSCLK = I2SCDCLK / 4 0x093 /8 0x0df / 16 0x127
				WM8753_IICWrite(0x06, 0x014);	//Sample Rate Control 	12MHz In, 16Khz
				break;
			case 22050:
				WM8753_IICWrite(0x07, 0x127); 	//Sample Rate Control (2) In Master Mode it is decided that divider of I2SSCLK(VXCLK, BCLK) : I2SSCLK = I2SCDCLK / 4 0x093 /8 0x0df / 16 0x127
				WM8753_IICWrite(0x06, 0x032);	//Sample Rate Control 	12MHz In, 22.05Khz
				break;
			case 32000:

				WM8753_IICWrite(0x07, 0x0df); 	//Sample Rate Control (2) In Master Mode it is decided that divider of I2SSCLK(VXCLK, BCLK) : I2SSCLK = I2SCDCLK / 4 0x093 /8 0x0df / 16 0x127
				WM8753_IICWrite(0x06, 0x018);	//Sample Rate Control 	12MHz In, 32Khz
				break;
			case 44100:
				WM8753_IICWrite(0x07, 0x093); 	//Sample Rate Control (2) In Master Mode it is decided that divider of I2SSCLK(VXCLK, BCLK) : I2SSCLK = I2SCDCLK / 4 0x093 /8 0x0df / 16 0x127
				WM8753_IICWrite(0x06, 0x020);	//Sample Rate Control 	12MHz In, 44.1Khz
				break;
			case 48000:
				WM8753_IICWrite(0x07, 0x093); 	//Sample Rate Control (2) In Master Mode it is decided that divider of I2SSCLK(VXCLK, BCLK) : I2SSCLK = I2SCDCLK / 4 0x093 /8 0x0df / 16 0x127
				WM8753_IICWrite(0x06, 0x000);	//Sample Rate Control 	12MHz In, 48Khz
				break;
			case 88200:

				WM8753_IICWrite(0x07, 0x003); 	//Sample Rate Control (2) In Master Mode it is decided that divider of I2SSCLK(VXCLK, BCLK) : I2SSCLK = I2SCDCLK / 4 0x093 /8 0x0df / 16 0x127
				WM8753_IICWrite(0x06, 0x03c);	//Sample Rate Control 	12MHz In, 88.2Khz
				break;
			case 96000:
				WM8753_IICWrite(0x07, 0x003); 	//Sample Rate Control (2) In Master Mode it is decided that divider of I2SSCLK(VXCLK, BCLK) : I2SSCLK = I2SCDCLK / 4 0x093 /8 0x0df / 16 0x127
				WM8753_IICWrite(0x06, 0x01c);	//Sample Rate Control 	12MHz In, 96Khz
				break;
			default:
				WM8753_IICWrite(0x07, 0x0df); 	//Sample Rate Control (2) In Master Mode it is decided that divider of I2SSCLK(VXCLK, BCLK) : I2SSCLK = I2SCDCLK / 4 0x093 /8 0x0df / 16 0x127
				WM8753_IICWrite(0x06, 0x000);	//Sample Rate Control 	12MHz In, 48Khz
				break;				

		}
	}
	
	
	WM8753_IICWrite(0x08, 0x0ff); 	//Left DAC Volume
	WM8753_IICWrite(0x09, 0x0ff); 	//Right DAC Volume
	WM8753_IICWrite(0x0a, 0x00f); 	//Bass Control
	WM8753_IICWrite(0x0b, 0x00f); 	//Treble Control
	WM8753_IICWrite(0x0c, 0x07b); 	//ALC 1
	WM8753_IICWrite(0x0d, 0x000); 	//ALC 2
	WM8753_IICWrite(0x0e, 0x032); 	//ALC 3
	WM8753_IICWrite(0x0f, 0x000); 	//Noise Gate 
	WM8753_IICWrite(0x10, 0x1c3); 	//Left ADC Volume
	WM8753_IICWrite(0x11, 0x1c3); 	//Right ADC Volume
	WM8753_IICWrite(0x12, 0x0c0); 	//Additional Control
	WM8753_IICWrite(0x13, 0x000); 	//Three D Control
	WM8753_IICWrite(0x15, 0x000); 	//Power Management(2) << Power up ADCL/R 0x00c>>
	WM8753_IICWrite(0x18, 0x000); 	//ID Register
	WM8753_IICWrite(0x19, 0x000); 	//Codec Interupt Polarty
	WM8753_IICWrite(0x1a, 0x000);	//Codec Interupt Enable
	WM8753_IICWrite(0x1b, 0x000);	//GPIO Control (1)
	WM8753_IICWrite(0x1c, 0x000);	//GPIO Control (2)	
	
	WM8753_IICWrite(0x28, 0x159);	//LOUT1 Volume
	WM8753_IICWrite(0x29, 0x159);	//ROUT1 Volume
	WM8753_IICWrite(0x2a, 0x059);	//LOUT2 Volume
	WM8753_IICWrite(0x2b, 0x059);	//ROUT2 Volume
	WM8753_IICWrite(0x2c, 0x079);	//MONO Out
	WM8753_IICWrite(0x2d, 0x000);	//Output Control
	WM8753_IICWrite(0x2e, 0x005);	//ADC Input Mode << Set L/R ADC input select to Line 1/2 '01' >>
	WM8753_IICWrite(0x2f, 0x000);	//Input Control (1)
	WM8753_IICWrite(0x30, 0x000);	//Input Control (2)
	WM8753_IICWrite(0x31, 0x097);	//Left Input Volume
	WM8753_IICWrite(0x32, 0x097);	//Right Input Volume
	WM8753_IICWrite(0x33, 0x000);	//MIC Bias Comp Control	

	WM8753_IICWrite(0x3d, 0x000);	//Bias Control
	WM8753_IICWrite(0x3f, 0x000);	//Additional Control
		
	WM8753_IICWrite(0x16, 0x180); 	//Power Management(3)

	Audio_CloseIICPort();
}
void WM8580_CodecCLKOut(u32 uClockout)
{
	Audio_InitIICPort();
	Audio_InitIIC();

	WM8580_IICWrite(0x33, 0x038);								//Power Down 2 : OSC, PLLA and PLLB enable, SPDI/F Clk Disabled
	WM8580_IICWrite(0x07, 0x1f4);								//PLLB 4 : MCLK OUT OSCCLKOUT
	Audio_CloseIICPort();	

}
void WM8580_CodecInitPCMIn(SerialDataFormat eFormat, u32 eSampleRate, OPMode eOpmode, PCMWordLength eWordLen, AudioSource eSource, SMDKLine eSmdkline)
{
	Audio_InitIICPort();
	Audio_InitIIC();

	WM8580_IICWrite(0x35, 0x000);								//Reset WM8580	
	WM8580_IICWrite(0x32, 0x07c);								//Power Down 1 : ADC Enable, DACs Disable, Digital Interfase Enable	
	WM8580_IICWrite(0x33, 0x03f);								//Power Down 2 : OSC, PLL and SPDI/F Clk Disabled
		
	if ( eSmdkline == SMDK_I2SMULTI )								//Primary Audio Interface : I2S Multichannel
	{
		WM8580_IICWrite(0x08, 0x03c);							//CLKSEL : MCLK is source of All DACs, ADC & SPDI MCLK
		if ( eOpmode == Master)									//Master : 6410		Slave : WM8580
		{			
			WM8580_IICWrite(0x09, 0x000);						//PAIF 1	Master Reciever	 			
			WM8580_IICWrite(0x0a, 0x000);						//PAIF 2	Master Transmitte 	
		}		
		else if ( eOpmode == Slave)								//Master : WM8580 Slave : 6410
		{
			WM8580_IICWrite(0x09, 0x000);						//PAIF 1	Master Reciever	 768fs, 64 BCLKs/LRCLK 		//256fs : 010 384fs : 011 512fs : 100 768fs : 101
			WM8580_IICWrite(0x0a, 0x025);						//PAIF 2	Master Transmitte 0x112 or 0x102		
		}		
		WM8580_IICWrite(0x1d, 0x140);							//ADC Control 1		
		if (eFormat == I2SFormat)
		{
			WM8580_IICWrite(0x0c, 0x182 | eWordLen << 2);		//PAIF 3	Receiver Audio format
			WM8580_IICWrite(0x0d, 0x082 | eWordLen << 2);		//PAIF 4
		}
		else if (eFormat == LSBJustified)
		{
			WM8580_IICWrite(0x0c, 0x181 | eWordLen << 2);		//PAIF 3	Receiver Audio format
			WM8580_IICWrite(0x0d, 0x082 | eWordLen << 2);		//PAIF 4
		}
		else if (eFormat == MSBJustified)
		{
			WM8580_IICWrite(0x0c, 0x180 | eWordLen << 2);		//PAIF 3	Receiver Audio format
			WM8580_IICWrite(0x0d, 0x080 | eWordLen << 2);		//PAIF 4
		}
	}

	else if ( eSmdkline == SMDK_I2S || eSmdkline == SMDK_PCM)	//Secondry Audio Interface : I2S, PCM IF
	{
		WM8580_IICWrite(0x08, 0x01c);							//CLKSEL : MCLK is source of All ADC, DAC & SPDIF
		if ( eOpmode == Master)									// Master : 6410	Slave : WM8580	
		{	
			WM8580_IICWrite(0x0b, 0x0c0);						//SAIF 1	Slave Interface	 			
		}		
		else if ( eOpmode == Slave)								//Master : WM8580 	Slave : 6410
		{
			WM8580_IICWrite(0x0b, 0x0e2);						//SAIF 1	Master Reciever	 768fs, 64 BCLKs/LRCLK 		//256fs : 010 384fs : 011 512fs : 100 768fs : 101
		}	
		WM8580_IICWrite(0x1d, 0x140);							//ADC Control 1		
		if (eFormat == I2SFormat)
		{
			WM8580_IICWrite(0x0e, 0x0c2 | eWordLen << 2);		//SAIF 2	Receiver Audio format
			WM8580_IICWrite(0x0d, 0x102 | eWordLen << 2);		//PAIF 4	Receiver Audio format
		}
		else if (eFormat == LSBJustified)
		{
			WM8580_IICWrite(0x0e, 0x0c0 | eWordLen << 2);		//SAIF 2	Receiver Audio format
			WM8580_IICWrite(0x0d, 0x100 | eWordLen << 2);		//PAIF 4	Receiver Audio format
		}
		else if (eFormat == MSBJustified)
		{
			WM8580_IICWrite(0x0e, 0x0c1 | eWordLen << 2);		//SAIF 2	Receiver Audio format
			WM8580_IICWrite(0x0d, 0x101 | eWordLen << 2);		//PAIF 4	Receiver Audio format
		}
		else if (eFormat == AFTER_PCMSYNC_HIGH)
		{
			WM8580_IICWrite(0x0e, 0x0e3);						//SAIF 2	Receiver Audio format		0x0e3
			WM8580_IICWrite(0x0d, 0x133);						//PAIF 4	Receiver Audio format		0x133
			
		}
		else if (eFormat == DURING_PCMSYNC_HIGH)
		{
			WM8580_IICWrite(0x0e, 0x0f3);						//SAIF 2	Receiver Audio format		0x0f3
			WM8580_IICWrite(0x0d, 0x123);						//PAIF 4	Receiver Audio format		0x123
		}	
	}	
	Audio_CloseIICPort();	
	/*
	//Secondrt Audio Interface (SAIF) RX Configuration
	WM8580_IICWrite(0x0b, 0x000);				//SAIF 1	
	WM8580_IICWrite(0x0e, 0x000);				//SAIF 2		
	
	WM8580_IICWrite(0x05, 0x000);				//PLL B
	WM8580_IICWrite(0x06, 0x000);				//PLL B
	WM8580_IICWrite(0x07, 0x000);				//PLL B	
	
	WM8580_IICWrite(0x0f, 0x000);				//DAC Control 1	
	WM8580_IICWrite(0x10, 0x100);				//DAC Control 2
	WM8580_IICWrite(0x11, 0x100);				//DAC Control 3
	WM8580_IICWrite(0x12, 0x100);				//DAC Control 4
	WM8580_IICWrite(0x13, 0x100);				//DAC Control 5
	
	WM8580_IICWrite(0x14, 0x100);				//Digital Attenuation DACL1
	WM8580_IICWrite(0x15, 0x100);				//Digital Attenuation DACR2
	WM8580_IICWrite(0x16, 0x100);				//Digital Attenuation DACL2
	WM8580_IICWrite(0x17, 0x100);				//Digital Attenuation DACR2 
	WM8580_IICWrite(0x18, 0x100);				//Digital Attenuation DACL3
	WM8580_IICWrite(0x19, 0x100);				//Digital Attenuation DACR3	
	WM8580_IICWrite(0x1c, 0x100);				//Master Digital Attenuation
	
	
	WM8580_IICWrite(0x1e, 0x100);				//SPDTXCHAN 0
	WM8580_IICWrite(0x1f, 0x100);				//SPDTXCHAN 1	
	WM8580_IICWrite(0x20, 0x200);				//SPDTXCHAN 2
	WM8580_IICWrite(0x21, 0x200);				//SPDTXCHAN 3
	WM8580_IICWrite(0x22, 0x200);				//SPDTXCHAN 4
	WM8580_IICWrite(0x23, 0x200);				//SPDTXCHAN 5
	
	WM8580_IICWrite(0x24, 0x200);				//SPD MODE
	WM8580_IICWrite(0x25, 0x200);				//INTMASK
	WM8580_IICWrite(0x26, 0x200);				//GPO1 Control
	WM8580_IICWrite(0x27, 0x200);				//GPO2 Control
	WM8580_IICWrite(0x28, 0x200);				//GPO3 Control
	WM8580_IICWrite(0x29, 0x200);				//GPO4 Control
	WM8580_IICWrite(0x2a, 0x200);				//GPO5 Control 
	
	WM8580_IICWrite(0x2b, 0x200);				//INTSTAT 
	
	WM8580_IICWrite(0x2c, 0x200);				//SPDRXCHAN 1
	WM8580_IICWrite(0x2d, 0x200);				//SPDRXCHAN 2
	WM8580_IICWrite(0x2e, 0x200);				//SPDRXCHAN 3
	WM8580_IICWrite(0x2f, 0x200);				//SPDRXCHAN 4	
	WM8580_IICWrite(0x30, 0x300);				//SPDRXCHAN 5
	
	WM8580_IICWrite(0x31, 0x300);				//SPTSTAT	
	
	WM8580_IICWrite(0x34, 0x300);				//Read Back
	*/
}

;void WM8580_CodecInitPCMOut(SerialDataFormat eFormat, u32 eSampleRate, OPMode eOpmode, PCMWordLength eWordLen, SMDKLine eSmdkline)
{
	Audio_InitIICPort();
	Audio_InitIIC();

	WM8580_IICWrite(0x35, 0x000);								//Reset WM8580
		
	if ( eSmdkline == SMDK_I2SMULTI )								//Primary Audio Interface : I2S Multichannel
	{
		WM8580_IICWrite(0x32, 0x022);								//Power Down 1: ADC Disable, DACs Enable, Digital Interface Enable 
		WM8580_IICWrite(0x33, 0x03f);							//Power Down 2: OSC, PLL and SPDI/F Clk Disabled
		WM8580_IICWrite(0x08, 0x030);							//CLKSEL : All ADC, DAC & SPDIF CLk source are MCLK		
		if ( eOpmode == Master)									//Master : 6410		Slave : WM8580
		{
			WM8580_IICWrite(0x09, 0x000);						//PAIF 1					
			WM8580_IICWrite(0x0a, 0x000);						//PAIF 2	
		}	
		else if ( eOpmode == Slave)								// Master : WM8580 	Slave : 6410
		{
			WM8580_IICWrite(0x09, 0x025);						//PAIF 1	Master Reciever	 768fs, 64 BCLKs/LRCLK 		//256fs : 010 384fs : 011 512fs : 100 768fs : 101
			WM8580_IICWrite(0x0a, 0x000);						//PAIF 2	Master Transmitte 0x112 or 0x102		
		}	
		
		if (eFormat == I2SFormat)
		{
			WM8580_IICWrite(0x0c, 0x182 | eWordLen << 2);		//PAIF 3	Receiver Audio format
			WM8580_IICWrite(0x0d, 0x082 | eWordLen << 2);		//PAIF 4
		}
		else if (eFormat == LSBJustified)
		{
			WM8580_IICWrite(0x0c, 0x181 | eWordLen << 2);		//PAIF 3	Receiver Audio format
			WM8580_IICWrite(0x0d, 0x082 | eWordLen << 2);		//PAIF 4

		}
		else if (eFormat == MSBJustified)
		{
			WM8580_IICWrite(0x0c, 0x180 | eWordLen << 2);		//PAIF 3	Receiver Audio format
			WM8580_IICWrite(0x0d, 0x080 | eWordLen << 2);		//PAIF 4
		}
	}

	else if ( eSmdkline == SMDK_I2S || eSmdkline == SMDK_PCM)	//Secondry Audio Interface : I2S Multichannel
	{
		WM8580_IICWrite(0x32, 0x03a);							//Power Down 1: ADC Disable, DAC1 Enable, DAC2 and DAC3 are disable, Digital Interface Enable 
		WM8580_IICWrite(0x33, 0x038);							//Power Down 2: OSC, PLL Enable, SPDI/F Clk Disabled
		WM8580_IICWrite(0x08, 0x010);							//CLKSEL : All ADC, DAC & SPDIF CLk source are MCLK		
		if ( eOpmode == Master)									// Master : 6410	Slave : WM8580
		{
			WM8580_IICWrite(0x0b, 0x0c2);						//SAIF 1	Master Transmiter Reciever	 			
		}
		
		else if ( eOpmode == Slave)								// Master : WM8580 	Slave : 6410
		{
			WM8580_IICWrite(0x0b, 0x0e2);						//SAIF 1	Slave Reciever	 768fs, 64 BCLKs/LRCLK 		//256fs : 010 384fs : 011 512fs : 100 768fs : 101
		}	
			
		if (eFormat == I2SFormat)
		{
			WM8580_IICWrite(0x0e, 0x042 | eWordLen << 2);		//SAIF 2	Receiver Audio format
			WM8580_IICWrite(0x0c, 0x102 | eWordLen << 2);		//PAIF 3	Receiver Audio format
			
		}
		else if (eFormat == LSBJustified)
		{
			WM8580_IICWrite(0x0e, 0x040 | eWordLen << 2);		//SAIF 2	Receiver Audio format
			WM8580_IICWrite(0x0c, 0x100 | eWordLen << 2);		//PAIF 3	Receiver Audio format			
		}
		else if (eFormat == MSBJustified)
		{
			WM8580_IICWrite(0x0e, 0x041 | eWordLen << 2);		//SAIF 2	Receiver Audio format
			WM8580_IICWrite(0x0c, 0x101 | eWordLen << 2);		//PAIF 3	Receiver Audio format			
		}
		else if (eFormat == AFTER_PCMSYNC_HIGH)
		{
			WM8580_IICWrite(0x0e, 0x063);		//SAIF 2	Receiver Audio format		0x063
			WM8580_IICWrite(0x0c, 0x123);		//PAIF 3	Receiver Audio format		0x123			
						
		}
		else if (eFormat == DURING_PCMSYNC_HIGH)
		{
			WM8580_IICWrite(0x0e, 0x073);		//SAIF 2	Receiver Audio format		0x073
			WM8580_IICWrite(0x0c, 0x133);		//PAIF 3	Receiver Audio format		0x133	
			
		}
	}
		// Add to DAC Control upto FIFO control of S3C6400
	WM8580_IICWrite(0x0f, 0x024);				//DAC Control 1	
	if (eFormat == DURING_PCMSYNC_HIGH || eFormat == AFTER_PCMSYNC_HIGH)
		WM8580_IICWrite(0x10, 0x005);				//DAC Control 2			Left & Right DAC Souce : Left Channel
	else
		WM8580_IICWrite(0x10, 0x009);				//DAC Control 2			Left DAC : Left Channel, Right DAC : Right Channel
	
	WM8580_IICWrite(0x1c, 0x1cf);				//Master Digital Attenuation
	
	Audio_CloseIICPort();

	/*switch(eSampleRate)
		{
			case 96000:
				//24.576MHz 
				WM8580_IICWrite(0x00, 0x1ba);				//PLL A K-Value : C49BA 
				WM8580_IICWrite(0x01, 0x024);				//PLL A N-Value : 8
				WM8580_IICWrite(0x02, 0x082);				//PLL A Divider : 
				WM8580_IICWrite(0x03, 0x002);				//PLL A 
				break;
			case 48000:
				//12.288MHz 
				WM8580_IICWrite(0x00, 0x1ba);				//PLL A K-Value : C49BA 
				WM8580_IICWrite(0x01, 0x024);				//PLL A N-Value : 8
				WM8580_IICWrite(0x02, 0x082);				//PLL A 
				WM8580_IICWrite(0x03, 0x006);				//PLL A 
				break;
			case 32000:
				//8.192MHz 
				WM8580_IICWrite(0x00, 0x1ba);				//PLL A K-Value : C49BA 
				WM8580_IICWrite(0x01, 0x024);				//PLL A N-Value : 8
				WM8580_IICWrite(0x02, 0x082);				//PLL A 
				WM8580_IICWrite(0x03, 0x00c);				//PLL A 
				break;
			case 16000:
				//6.144MHz 
				WM8580_IICWrite(0x00, 0x1ba);				//PLL A K-Value : C49BA 
				WM8580_IICWrite(0x01, 0x024);				//PLL A N-Value : 8
				WM8580_IICWrite(0x02, 0x082);				//PLL A 
				WM8580_IICWrite(0x03, 0x00a);				//PLL A 
				break;
			case 8000:
				//4.096MHz 
				WM8580_IICWrite(0x00, 0x1ba);				//PLL A K-Value : C49BA 
				WM8580_IICWrite(0x01, 0x024);				//PLL A N-Value : 8
				WM8580_IICWrite(0x02, 0x082);				//PLL A 
				WM8580_IICWrite(0x03, 0x00e);				//PLL A 
				break;
			case 88200:
				//22.5792MHz 
				WM8580_IICWrite(0x00, 0x089);				//PLL A K-Value : 21B089
				WM8580_IICWrite(0x01, 0x0d8);				//PLL A N-Value : 7
				WM8580_IICWrite(0x02, 0x078);				//PLL A 
				WM8580_IICWrite(0x03, 0x002);				//PLL A 
				break;
			case 44100:
				//11.2896MHz 
				WM8580_IICWrite(0x00, 0x089);				//PLL A K-Value : 21B089 
				WM8580_IICWrite(0x01, 0x0d8);				//PLL A N-Value : 7
				WM8580_IICWrite(0x02, 0x078);				//PLL A 
				WM8580_IICWrite(0x03, 0x006);				//PLL A 
				break;
			case 22050:
				//5.6448MHz 
				WM8580_IICWrite(0x00, 0x089);				//PLL A K-Value : 21B089 
				WM8580_IICWrite(0x01, 0x0d8);				//PLL A N-Value : 7
				WM8580_IICWrite(0x02, 0x078);				//PLL A 
				WM8580_IICWrite(0x03, 0x00a);				//PLL A 
				break;
			case 11025:
				//2.8224MHz 
				WM8580_IICWrite(0x00, 0x089);				//PLL A K-Value : 21B089 
				WM8580_IICWrite(0x01, 0x0d8);				//PLL A N-Value : 7
				WM8580_IICWrite(0x02, 0x078);				//PLL A 
				WM8580_IICWrite(0x03, 0x002);				//PLL A 
				break;
			default:
				//24.576MHz 
				WM8580_IICWrite(0x00, 0x1ba);				//PLL A K-Value : C49BA 
				WM8580_IICWrite(0x01, 0x024);				//PLL A N-Value : 8
				WM8580_IICWrite(0x02, 0x082);				//PLL A 
				WM8580_IICWrite(0x03, 0x002);				//PLL A 
				break;
		}		


		*/

}

void WM9713_CodecInitPCMIn(SerialDataFormat eFormat, u32 eSampleRate, OPMode eOpmode, PCMWordLength eWordLen, AudioSource eSource,SMDKLine eSmdkline)
{
	AC97_CodecCmd(WRITE,0x26, 0x4f00);		// Enable PR5(Internal Clock, AC-link I/F)
	AC97_CodecCmd(WRITE,0x26, 0x4700);		// Enable PR3(VREF, I/P PGA's, DAC's, ADC's, Mixer, O/P's) 	
	AC97_CodecCmd(WRITE,0x3c, 0xfbff);			// Enable MBIAS generator
	Delay(1000);	
	AC97_CodecCmd(WRITE,0x26, 0x4300);		// Enable PR2(I/P PGA's and mixers)
	
	if (eFormat == ACLinkSlot34)					// Enable ADC L/R
		AC97_CodecCmd(WRITE,0x3C, 0xfbc3);	
	else if (eFormat == ACLinkSlot6)
		AC97_CodecCmd(WRITE,0x3C, 0xfbcf);	
	
	AC97_CodecCmd(WRITE,0x26, 0x4200);		// Enable Stereo ADC 				//4100
	AC97_CodecCmd(WRITE,0x26, 0x0200);		// Enable PR6 (O/P PGA's)						
	AC97_CodecCmd(WRITE,0x3E, 0xff9f);			// Enable LINE L/R PGA				26 0100
				
	
	AC97_CodecCmd(WRITE,0x2A,0x1);			//Variable Rate Enable	
	UART_Printf("VRA Enable(1)/Disable(0): 0x%x\n",(0x1&AC97_CodecCmd(READ,0x2A,0x0001)));
	if(eSampleRate == 8000)						//ADC Sampling frequency 8kHz
	{	
		AC97_CodecCmd(WRITE,0x32,0x1f40);	
	}	
	else if(eSampleRate == 44100)					//ADC Sampling frequency 44.1kHz
	{		
		AC97_CodecCmd(WRITE,0x32,0xac44);
	}
	else if(eSampleRate == 22050)					//ADC Sampling frequency 22.05kHz
	{		
		AC97_CodecCmd(WRITE,0x32,0x5622);	 	
	}
	else 										//ADC Sampling frequency 48kHz
	{		
		AC97_CodecCmd(WRITE,0x32,0xbb80);
	}
	//UART_Printf("\nAC97 Codec 0x32 Reg.: 0x%x\n\n", AC97_CodecCmd(READ,0x32,0x0000));
	if ( eFormat == ACLinkSlot34)
	{
		//AC97_CodecCmd(WRITE,0x3C, 0xfbc3);	// Enable ADC L/R					//fbf3
		AC97_CodecCmd(WRITE,0x14, 0xfe12);	// Record Mux Source Selection: LINE L/R
		AC97_CodecCmd(WRITE,0x1c, 0x00a0);
		AC97_CodecCmd(WRITE,0x04, 0x0303);
		AC97_CodecCmd(WRITE,0x12, 0x1010);	// Unmute ADC and Set ADC Recoding Volume	
	}
	else if (eFormat == ACLinkSlot6)
	{
		AC97_CodecCmd(WRITE,0x5C, 0x2);		// ADC Slot Mapping: Left(Slot 6), Right(Slot 9)	
		AC97_CodecCmd(WRITE,0x14, 0xfe12);	// Record Mux Source Selection: LINE L/R
		AC97_CodecCmd(WRITE,0x1c, 0x00a0);
		AC97_CodecCmd(WRITE,0x12, 0x1010);	// Unmute ADC and Set ADC Recoding Volume		
	}		

	////////////////////////////////////////////////////////
	// codec setting to hear sound while PCM encoding 
	//uAc97cmd = AC97_CodecCmd(READ,0x14, 0x0000);
	//uAc97cmd =  uAc97cmd &  ~(3 << 14) & (~ (5 << 11));
	//AC97_CodecCmd(WRITE,0x14, 0x1612);
	//AC97_CodecCmd(WRITE,0x1c, 0x00a0);
	//AC97_CodecCmd(WRITE,0x04,0x0707);
	//UART_Printf("%x\n",AC97_CodecCmd(READ,0x14, 0x0000));	
	//UART_Printf("%x\n",AC97_CodecCmd(READ,0x0a, 0x0000));
	////////////////////////////////////////////////////////
	
}

void WM9713_InitPCMOut(SerialDataFormat eFormat, u32 eSampleRate, OPMode eOpmode, PCMWordLength eWordLen, SMDKLine eSmdkline)
{	
 	AC97_CodecCmd(WRITE,0x26, 0x4f00);	// Enable PR5(Internal Clock, AC-link I/F)
	AC97_CodecCmd(WRITE,0x26, 0x4700);	// Enable PR3(VREF, I/P PGA's, DAC's, ADC's, Mixer, O/P's)
 	
	AC97_CodecCmd(WRITE,0x3c, 0xfbff);		// Enable MBIAS generator
	Delay(1000);

	AC97_CodecCmd(WRITE,0x26, 0x4300);	// Enable PR2(I/P PGA's and mixers)
	AC97_CodecCmd(WRITE,0x3C, 0xfbf3);		// Enable HPL/R Mixer
	AC97_CodecCmd(WRITE,0x26, 0x4100);	// Enable PR1(Stereo DAC)
	AC97_CodecCmd(WRITE,0x3C, 0xfb33);	// Enable DAC L/R
	AC97_CodecCmd(WRITE,0x26, 0x0100);	// Enable PR6 (O/P PGA's)
	AC97_CodecCmd(WRITE,0x3E, 0xf9ff);		// Enable PR6 (O/P PGA's)

	AC97_CodecCmd(WRITE,0x2A,0x1);		//Variable Rate Enable	

	if(eSampleRate==8000){
	//DAC Sampling frequency 8kHz
	AC97_CodecCmd(WRITE,0x2C,0x1f40);
	}
	else if(eSampleRate==48000){
	//DAC Sampling frequency 48kHz
	AC97_CodecCmd(WRITE,0x2C,0xbb80);
	}
	else if(eSampleRate==44100){
	//DAC Sampling frequency 44.1kHz
	AC97_CodecCmd(WRITE,0x2C,0xac44);
	}
	else if(eSampleRate==22050){
	//DAC Sampling frequency 22.05kHz
	AC97_CodecCmd(WRITE,0x2C,0x5622);		
	}

	AC97_CodecCmd(WRITE,0x12,0x8000);		//Disable ADC because it makes noise when ADC is turn on
	AC97_CodecCmd(WRITE,0x1c, 0x00a0);	// HPL/R PGA input select HPMIXL/R
	AC97_CodecCmd(WRITE,0x04,0x0707);		// Set HPL/R Volume 
	AC97_CodecCmd(READ,0x04,0x00000);	//HP out volume 
	AC97_CodecCmd(WRITE,0x0c,0x6808);		// Unmute DAC to HP mixer path
	AC97_CodecCmd(WRITE,0x04,0x0A0A);	// Unmute HPL/R
		
	
}

void WM8990_CodecInitPCMIn(SerialDataFormat eFormat, u32 eSampleRate, OPMode eOpmode, PCMWordLength eWordLen, AudioSource eSource, SMDKLine eSmdkline)
{
	Audio_InitIICPort();
	Audio_InitIIC();
	#if 1
	WM8990_IICWrite(0x00, 0x8990);			// Software Codec Reset 
	WM8990_IICWrite(0x01, 0x0003);			// Enable Vmid = 50K, Vref
	Delay(1000);	
	
	WM8990_IICWrite(0x02, 0x0303);			// Enable AIN MUX, ADC 
	WM8990_IICWrite(0x03, 0x0000);			// Disable Output	

	if ( eSmdkline == SMDK_I2S )					
	{
		if ( eOpmode == Slave)					// Codec master
		{
			WM8990_IICWrite(0x08, 0x8000);
			WM8990_IICWrite(0x0a, 0x0400);
			WM8990_IICWrite(0x06, 0x000a);
			WM8990_IICWrite(0x07, 0x0000);
		}
		else										// Codec Slave
		{
			WM8990_IICWrite(0x08,  0x0000);
		}
	}
	
	else if ( eSmdkline == SMDK_PCM )	
	{
		if ( eOpmode == Slave)					// Codec master
		{
			WM8990_IICWrite(0x07, 0x0000);
			WM8990_IICWrite(0x08, 0x8000);
			WM8990_IICWrite(0x0a, 0x0400);
			WM8990_IICWrite(0x06, 0x000a);
		}
		else										// Codec Slave
		{
			WM8990_IICWrite(0x08,  (u32) eOpmode << 14 | 0x2000);
		}
	}
	
	
	switch(eFormat)
	{
		case MSBJustified:
			if (eWordLen == Word24)
				WM8990_IICWrite(0x04, 0x4048);		//24bit Left justified 			
			else 
				WM8990_IICWrite(0x04, 0x4008);		//16bit Left justified 		
			break;
			
		case LSBJustified:
			if (eWordLen == Word24)
				WM8990_IICWrite(0x04, 0x4040);		//24bit Right justified 			
			else 
				WM8990_IICWrite(0x04, 0x4000);		//16bit Right justified 	
			break;
			
		case AFTER_PCMSYNC_HIGH:
			WM8990_IICWrite(0x04, 0x4118);			//16bit DSP Format		
			break;
			
		case DURING_PCMSYNC_HIGH:
			WM8990_IICWrite(0x04, 0x4198);			//16bit DSP Format
			break;
			
		case I2SFormat:
		default:
			if (eWordLen == Word24)
				WM8990_IICWrite(0x04, 0x4050);		//24bit I2S 			
			else 
				WM8990_IICWrite(0x04, 0x4010);		//16bit I2S
			break;
		
				
	}
	WM8990_IICWrite(0x2b, 0x0140);			// Input Left Mixer
	WM8990_IICWrite(0x2c, 0x0140);			// Input Right Mixer	
	
	WM8990_IICWrite(0x0e, 0x0100);			// Enable HPF

	WM8990_IICWrite(0x0f, 0x01c0);			// Left ADC Volume	
	WM8990_IICWrite(0x10, 0x01c0);			// Right ADC Volume
	
	#else
	//Will be updated for MIC IN
	WM8990_IICWrite(0x01, 0x0003);			// Enable Vmid = 50K, Vref, SPK
	WM8990_IICWrite(0x02, 0x6242);			// Enable AIN, ADC, LIN1 Enable_CLK
	WM8990_IICWrite(0x03, 0x0000);			// Disable Output
	WM8990_IICWrite(0x04, 0x4010);			// I2S 16Bit
	WM8990_IICWrite(0x18, 0x011f);			// Lin2Vol = +30dB	
	WM8990_IICWrite(0x28, 0x0030);			// LI2MNBST = 0dB
	WM8990_IICWrite(0x29, 0x0020);			// LI2MNBST = 0dB
	WM8990_IICWrite(0x27, 0x0000);			// AIN MODE = 00, From Left Input Mixer	
	WM8990_IICWrite(0x0e, 0x0100);			// Enable HPF
	WM8990_IICWrite(0x0f, 0x01c0);			// LADCVOL
	#endif
}

void WM8990_CodecInitPCMOut(SerialDataFormat eFormat, u32 eSampleRate, OPMode eOpmode, PCMWordLength eWordLen, SMDKLine eSmdkline)
{
	Audio_InitIICPort();
	Audio_InitIIC();
	
	WM8990_IICWrite(0x00, 0x8990);			// Software Codec Reset 
	WM8990_IICWrite(0x39, 0x0046);			// Enable POBCTRL, SOFT_ST, BUFIOEN and BUFDCOPEN
	WM8990_IICWrite(0x01, 0x0300);			// Enable L/R Out
	WM8990_IICWrite(0x03, 0x0033);			// Enable left and fight output mixers (DACL & DACR remain enabled)
	WM8990_IICWrite(0x38, 0x0000);			// Disable DIS_LOUT and DIS_ROUT	
	WM8990_IICWrite(0x01, 0x3020);			// Enable WMID SEL = 2 * 50K Ohm Divider (L/R Out remain)
	Delay(50);
	
	WM8990_IICWrite(0x01, 0x0303);			// Enable VREF, Disable OUT3, AC Coupled mode
	WM8990_IICWrite(0x39, 0x0000);			// Disable POBCTRL and SOFT_ST, BUFIOEN and BUFDCONPEN remain enabled
	
	//if ( eSmdkline == SMDK_I2S )					// Codec Operation is Slave
	//	WM8990_IICWrite(0x08,  (int) eOpmode << 15 | 0x0);
	//else if ( eSmdkline == SMDK_PCM )	
	//	WM8990_IICWrite(0x08,  (int) eOpmode << 14 | 0x2000);

	if ( eSmdkline == SMDK_I2S )					// Codec Operation is Slave
	{
		if ( eOpmode == Slave)
		{
			WM8990_IICWrite(0x06, 0x000a);
			WM8990_IICWrite(0x07, 0x0000);
			WM8990_IICWrite(0x08, 0x8800);
			WM8990_IICWrite(0x09, 0x0802);
			WM8990_IICWrite(0x0a, 0x0400);
			
		}
		else
			WM8990_IICWrite(0x08,  0x0000);
	}
	else if ( eSmdkline == SMDK_PCM )	
	{
		if ( eOpmode == Slave)
		{
			WM8990_IICWrite(0x07, 0x0000);
			WM8990_IICWrite(0x08, 0x8000);
			WM8990_IICWrite(0x0a, 0x0400);
			WM8990_IICWrite(0x06, 0x000a);
		}
		else
			WM8990_IICWrite(0x08,  (int) eOpmode << 14 | 0x2000);
	}
	
	switch(eFormat)
	{
		case MSBJustified:
			if (eWordLen == Word24)
				WM8990_IICWrite(0x04, 0x4048);		//24bit Left justified 			
			else 
				WM8990_IICWrite(0x04, 0x4008);		//16bit Left justified 		
			break;
			
		case LSBJustified:
			if (eWordLen == Word24)
				WM8990_IICWrite(0x04, 0x4040);		//24bit Right justified 			
			else 
				WM8990_IICWrite(0x04, 0x4000);		//16bit Right justified 	
			break;
			
		case AFTER_PCMSYNC_HIGH:
			WM8990_IICWrite(0x04, 0x4118);			//16bit PCM I/F 		
			break;
			
		case DURING_PCMSYNC_HIGH:
			WM8990_IICWrite(0x04, 0x4198);			//16bit PCM I/F 	
			break;
			
		case I2SFormat:
		default:
			if (eWordLen == Word24)
				WM8990_IICWrite(0x04, 0x4050);		//24bit I2S 			
			else 
				WM8990_IICWrite(0x04, 0x4010);		//16bit I2S
			break;
		
				
	}
	
	WM8990_IICWrite(0x2d, 0x0001);			// Enable Left DAC to left mixer
	WM8990_IICWrite(0x2e, 0x0001);			// Enable Right DAC to right mixer	
	Delay(50);

	WM8990_IICWrite(0x1c, 0x00e7);			// LOUT VOL (HP) = -18dB
	WM8990_IICWrite(0x1d, 0x01e7);			// ROUT VOL (HP) = -18dB, Enable OPVU -> load volume setting to both left and right
	WM8990_IICWrite(0x0a, 0x00c0);			// DAC Digital Soft Mute enable an DAC unmute set
}

u32* Wave_Parser(u32 uWaveStartAddr, u8* uTotch, u32* uSampleRate, u8* uBitperch, u32* uSize)
{
	u32* uWaveDataAddr;
	
	u8* pch=(u8 *)(uWaveStartAddr+8);//8 is wave loc offset
	u16* u16data=(u16 *)(uWaveStartAddr+0x16);
	u32* u32data=(u32 *)(uWaveStartAddr+0x18);
	
	if(pch[0] == 'W' && pch[1] == 'A' && pch[2] == 'V' && pch[3] == 'E')
	{
		*uTotch = *u16data;
		*uSampleRate = *u32data;
		
		*uBitperch=*(unsigned short *)(uWaveStartAddr+0x22);
		
		
		pch=(unsigned char *)(uWaveStartAddr+0x28-4);
		if(pch[0] == 'd' && pch[1] == 'a' && pch[2] == 't' && pch[3] == 'a')
		{
			*uSize = *(unsigned int *)(uWaveStartAddr+0x28);
			*uWaveDataAddr = (uWaveStartAddr+0x28+4);			
		}
		else
		{
			pch=(unsigned char *)(uWaveStartAddr+0x40-4);
			if(pch[0] == 'd' && pch[1] == 'a' && pch[2] == 't' && pch[3] == 'a')		
			{
				*uSize = *(unsigned int *)(uWaveStartAddr+0x40);
				*uWaveDataAddr = (uWaveStartAddr+0x40+4);				
			}
			else
			{
			
				pch=(unsigned char *)(uWaveStartAddr+0x4c-4);
				if(pch[0] == 'd' && pch[1] == 'a' && pch[2] == 't' && pch[3] == 'a')		
				{
					*uSize = *(unsigned int *)(uWaveStartAddr+0x4c);
					*uWaveDataAddr = (uWaveStartAddr+0x4c+4);				
				}
				else
				{			
					pch=(unsigned char *)(uWaveStartAddr+0x7c-4);
					if(pch[0] == 'd' && pch[1] == 'a' && pch[2] == 't' && pch[3] == 'a')		
					{
						*uSize = *(unsigned int *)(uWaveStartAddr+0x7c);
						*uWaveDataAddr = (uWaveStartAddr+0x7c+4);					
					}
					else
						*uSize = 0;
				}
			}
			
		}		
		
		UART_Printf("wave parsed result : totch : %d, samplerate : %d, bitperch : %d, size : %d byte",
				*uTotch, *uSampleRate, *uBitperch, *uSize);
		
		return uWaveDataAddr;
	}
	else return 0;
}

