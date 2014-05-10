/*******************************************************************************

	File Name: AC97.c
	Description: S3C2440A AC97 Controller Function Test Code
   	Version: 0.2 
   	History:
             0.0: First draft
             0.1: 2003. 12. 22, Following test codes were modified by Yoh-Han Lee           
                   - AC97 PCM Out in DMA mode
                   - AC97 PCM In in DMA mode
             0.2: 2004. 02. 12, Programmed and tested by Yoh-Han Lee
                   - Volume up/down and Mute on/off are available, when codec plays PCM data.
		     - AC97 power down mode test is supported.	
                   - AC97 reset timing check is added.
                   - Variable ADC/DAC Selection is supported. 
                   - AC97 PCM Out in the interrupt mode is supported. Thanks to Y. M. Lee.
                   - AC97 PCM In using interrupt mode is supported.
                   - AC97 MIC In using interrupt or DMA is added.
                   		 
********************************************************************************/

#include <stdlib.h>
#include <string.h>
#include "option.h"
#include "2440addr.h"
#include "2440lib.h"
#include "def.h"
#include "ac97.h"

#define AC97_REC_LEN 0xfffff*4
#define DOWN_BUF _NONCACHE_STARTADDRESS
#define PCM_OUT_TRIGGER 8
#define PCM_IN_TRIGGER 8
#define AC97_PCM_OUT_THRESHOLD (1<<18)
#define AC97_PCM_IN_THRESHOLD (1<<17)
#define AC97_MIC_IN_THRESHOLD (1<<16)

U32 save_AC97_rGPECON, save_AC97_rGPEDAT, save_AC97_rGPEUP;
U32 AC97_size, AC97_fs;
U32 Output_Volume,Input_Volume;
U32 *Rec_AC97_BUF, *Play_AC97_BUF, *End_AC97_BUF;
static int delayLoopCount;

S16 PCM_Out_INT_Exit = 0;
S16 PCM_In_INT_Exit =0;

U8 *AC97_BUF,*AC97_temp;
U8 Up_Down_Volume;

char AC97_Rec_Done = 0;
char Codec_Ready_Irq;
char AC97_mute = 1;
char Codec_Ready = 1;

void AC97_Port_Init(void);
void AC97_Port_Return(void);
void AC97_Init(void);
void Download_PCM_File(void);
void AC97_CodecInit_PCMOut(U16 AC97_fs);
void AC97_CodecInit_PCMIn(U16 AC97_fs);
void AC97_CodecInit_MICIn(U16 AC97_fs);
void AC97_CodecInit_PD(void);
void AC97_CodecExit_PCMOut(void);
void AC97_CodecExit_PCMIn(U16 DACs_off);
void AC97_CodecExit_MICIn(U16 DACs_off);
void AC97_PCMout_DMA1(U32 PCM_Size);
void AC97_PCMout_INT(U32 PCM_Size);
void AC97_PCMin_DMA2(U32 PCM_Size);
void AC97_PCMin_INT(U32 PCM_Size);
void AC97_MICin_DMA3(U32 MIC_Size);
void AC97_MICin_INT(U32 PCM_Size);
void Delay_Init(void);

void PCM_In_Volume( U8 Up_Down_Volume);
void PCM_Out_Volume( U8 Up_Down_Volume);

void AC97_Controller_State(void);
void Delay_After_CommandWrite(int time);
U16 AC97_Select_SamplingRate(void);
U16 AC97_Codec_Cmd(U8 CMD_Read, U8 CMD_Offset, U16 CMD_Data);


static void __irq DMA1_Play_Done(void);
static void __irq DMA2_Rec_Done(void);
static void __irq DMA3_Rec_Done(void);
static void __irq RxInt(void);
static void __irq Muting(void);
void __irq AC97_Codec_Ready(void);
void __irq Irq_AC97_PCMout(void);
void __irq Irq_AC97_PCMin(void);
void __irq Irq_AC97_MICin(void);

void * func_ac97_test[][2]=
{	
	//AC97 Function Test Item
    	(void *)PCMout_Test_AC97,   		"Play Wave File    ",
    	(void *)PCMin_Test_AC97,     		"Record Sound via LineIn and Play it  ",   
    	(void *)MICin_Test_AC97,      		"Record Voice via MIC and Play it      ", 
       (void *)Powerdown_Test_AC97,    	"AC97 Power Down   ",      
       (void *)Reset_Test_AC97,      		"AC97 Reset Timing Check ",
	0,0
};

void AC97_Test(void)
{
	int i;

	AC97_Port_Init();
	
	while(1)
	{
		i=0;
		Uart_Printf("\n\n==================== AC97 Function Test ====================\n\n");
		Uart_Printf("1: Play Wave File \n");
		Uart_Printf("2: Record Sound via LineIn and Play it \n");
		Uart_Printf("3: Record Voice via MIC and Play it \n");
		Uart_Printf("4: AC97 Power Down \n");
		Uart_Printf("5: AC97 Reset Timing Check\n");
		Uart_Printf("\n============================================================");
		Uart_Printf("\nSelect #Item or Press enter key to exit:");
		i = Uart_GetIntNum();
		if(i==-1) break;		// return.	
//		if(i>=0 && (i<((sizeof(func_ac97_test)-1)/8)) )	// select and execute...
//			( (void (*)(void)) (func_ac97_test[i][0]) )();
        switch (i)
	    {
		    case 1:	 PCMout_Test_AC97();
			         break;
		    case 2:	 PCMin_Test_AC97();
			         break;
		    case 3:	 MICin_Test_AC97();
			         break;
		    case 4:	 Powerdown_Test_AC97();
			         break;
		    case 5:	 Reset_Test_AC97();
			         break;
		    default:
			         break;
		}
	}

	 rAC_GLBCTRL = 0;
}


void PCMout_Test_AC97(void)
{
    	int i;
    	//U8 Char;	

	AC97_Port_Init();
	Delay(1000);
		
       //Uart_Printf("Download PCM Wave File? (y/n)");
       //Char=Uart_Getch();
   
       //if( (Char == 'y') | (Char == 'Y') ) 
       Download_PCM_File();
   
       AC97_Init();

	if(Codec_Ready)
	{
       	Uart_Printf("\nSelect PCM Out Operation Mode\n");
		Uart_Printf("0: Interrupt, 1: DMA\n");
		i = Uart_GetIntNum();

		AC97_CodecInit_PCMOut(AC97_fs);
           
       	switch(i)
       	{
   			case 0:
			AC97_PCMout_INT(AC97_size);
   			break;
   	
   			case 1:
   			AC97_PCMout_DMA1(AC97_size);
   			break;
   
   			default:
   			AC97_PCMout_DMA1(AC97_size);
   			break;
       	}      
       
     		AC97_CodecExit_PCMOut();
	}
	
       AC97_mute = 1;
       Codec_Ready =1;
       PCM_Out_INT_Exit = 0;
       
       AC97_Port_Return();
}


void PCMin_Test_AC97(void)
{
	int i;
	U32 Sampling_Rate;
       AC97_Rec_Done = 0;
      
       AC97_Port_Init();
	   
       Sampling_Rate = AC97_Select_SamplingRate();
          
     	AC97_Init();	

       if(Codec_Ready)
	{
		Uart_Printf("\nSelect PCM In/Out Operation Mode\n");
		Uart_Printf("0: Interrupt, 1: DMA\n");
		i = Uart_GetIntNum();

       	AC97_CodecInit_PCMIn(Sampling_Rate);

		switch(i)
       	{
   			case 0:
       		AC97_PCMin_INT(AC97_REC_LEN);

			//Play Recorded Data
	    		AC97_CodecInit_PCMOut(Sampling_Rate);
			AC97_PCMout_INT(AC97_REC_LEN);
       		break;

			case 1:
         		AC97_PCMin_DMA2(AC97_REC_LEN);
			
         		//Play Recorded Data
	    		AC97_CodecInit_PCMOut(Sampling_Rate);
	   		AC97_PCMout_DMA1(AC97_REC_LEN);
	   		break;
  			
			default:
			AC97_PCMin_DMA2(AC97_REC_LEN);
         		
	    		//Play Recorded Data
	    		AC97_CodecInit_PCMOut(Sampling_Rate);
	   		AC97_PCMout_DMA1(AC97_REC_LEN);
   			break;
		}
		AC97_CodecExit_PCMIn(1);
       }	

   	Codec_Ready =1;
       AC97_mute = 1;
       PCM_Out_INT_Exit = 0;
       PCM_In_INT_Exit = 0;

   	AC97_Port_Return();
}

void MICin_Test_AC97(void)
{
	U16 i, Sampling_Rate;
	AC97_Rec_Done = 0;
	
	AC97_Port_Init();
       Delay(1000);

       Sampling_Rate = AC97_Select_SamplingRate();
       
	AC97_Init();	

	if(Codec_Ready)
	{
		Uart_Printf("\nSelect MIC In Operation Mode\n");
		Uart_Printf("0: Interrupt, 1: DMA\n");
		i = Uart_GetIntNum();

       	AC97_CodecInit_MICIn(Sampling_Rate);

		switch(i)
       	{
			case 0:
			AC97_MICin_INT(AC97_REC_LEN/2);

			//Play Recorded Data
	    		AC97_CodecInit_PCMOut(Sampling_Rate);
	   		AC97_PCMout_DMA1(AC97_REC_LEN/2);	
			break;

			case 1:
			AC97_MICin_DMA3(AC97_REC_LEN/2);

         		//Play Recorded Data
	    		AC97_CodecInit_PCMOut(Sampling_Rate);
	   		AC97_PCMout_DMA1(AC97_REC_LEN/2);
			break;	

			default:
			AC97_MICin_DMA3(AC97_REC_LEN/2);

         		//Play Recorded Data
	    		AC97_CodecInit_PCMOut(Sampling_Rate);
	   		AC97_PCMout_DMA1(AC97_REC_LEN/2);	
			break;
 		}	
		
  		AC97_CodecExit_MICIn(1);
	}

	Codec_Ready =1;
	AC97_mute = 1;
       PCM_In_INT_Exit = 0;

	AC97_Port_Return();
}

void Powerdown_Test_AC97(void)
{
	int i;

	AC97_Port_Init();
	Delay(1000);

	AC97_Init();

	if(Codec_Ready)
	{
	  	AC97_CodecInit_PD();
	 
	 	//Normal
	 	Uart_Printf("\nNormal\n");
	 	AC97_Controller_State();
	 	Uart_Printf("AC97 Codec Powerdown Ctrl/Stat Reg. Value (at 0x26): 0x%x\n", AC97_Codec_Cmd(1,0x26,0x0000));
	 
	 	//ADCs off
	 	Uart_Printf("\n=>ADCs off PR0\n");
	 	AC97_Codec_Cmd(0,0x26,(1<<8));
	 	AC97_Controller_State();
	 	Uart_Printf("AC97 Codec Powerdown Ctrl/Stat Reg. Value (at 0x26): 0x%x\n", AC97_Codec_Cmd(1,0x26,0x0000));
	 
	 	//DACs off
	 	Uart_Printf("\n=>DACs off PR1\n");
	 	AC97_Codec_Cmd(0,0x26,(1<<8)|(1<<9));
	 	AC97_Controller_State();
	 	Uart_Printf("AC97 Codec Powerdown Ctrl/Stat Reg. Value (at 0x26): 0x%x\n", AC97_Codec_Cmd(1,0x26,0x0000));	
	 
	 	//Analog off
	 	Uart_Printf("\n=>Analog off PR2\n");
	 	AC97_Codec_Cmd(0,0x26,(1<<8)|(1<<9)|(1<<10));
	 	AC97_Controller_State();
	 	Uart_Printf("AC97 Codec Powerdown Ctrl/Stat Reg. Value (at 0x26): 0x%x\n", AC97_Codec_Cmd(1,0x26,0x0000));	
	 
	 	//Digital I/F off
	 	Uart_Printf("\n=>Digital I/F off PR4\n");
	 	AC97_Codec_Cmd(0,0x26,(1<<8)|(1<<9)|(1<<10)|(1<<12));
	 	AC97_Controller_State();
	 	//Uart_Printf("AC97 Codec Powerdown Ctrl/Stat 0x26 Reg.: 0x%x\n", AC97_Codec_Cmd(1,0x26,0x0000));	
	 
	 	//Shut off AC-Link
	 	Uart_Printf("\n=>Shut off AC-Link\n");
	 	rAC_GLBCTRL &= ~(1<<2);
	 	AC97_Controller_State();
	 	//Uart_Printf("AC97 Codec Powerdown Ctrl/Stat 0x26 Reg.: 0x%x\n", AC97_Codec_Cmd(1,0x26,0x0000));	
	 	  	
	 	while(1)
	 	{
	 		Uart_Printf("\nPress enter key for Warm Reset");
	 		i = Uart_GetIntNum();
	 		if(i==-1) break;	
	 	}
	 
	 	//Warm Reset
	 	Uart_Printf("\n=>Warm Reset\n");
	 	rAC_GLBCTRL = (1<<1);
	 	AC97_Controller_State();
	 	rAC_GLBCTRL &= ~(1<<1);
	 	
	 	rAC_GLBCTRL |= (1<<2);
	 	AC97_Controller_State();
	 	rAC_GLBCTRL |= (1<<3);
	 	Uart_Printf("AC97 Codec Powerdown Ctrl/Stat Reg. Value (at 0x26): 0x%x\n", AC97_Codec_Cmd(1,0x26,0x0000));	
	 
	 	//Cold Reset
	 	Uart_Printf("\n=>Cold Reset\n");
	 	rAC_GLBCTRL |= (1<<0);
	 	AC97_Controller_State();
	 	rAC_GLBCTRL &= ~(1<<0);
	 	
	 	rAC_GLBCTRL |= (1<<2);
	 	AC97_Controller_State();
	 	rAC_GLBCTRL |= (1<<3);
	 	AC97_Controller_State();
	 	Uart_Printf("AC97 Codec Powerdown Ctrl/Stat Reg. Value (at 0x26): 0x%x\n", AC97_Codec_Cmd(1,0x26,0x0000));	
	}

	Codec_Ready =1;
	AC97_Port_Return();
}

void Reset_Test_AC97(void)
{
	int i;
	
	AC97_Port_Init();
	Delay(1000);

	AC97_Init();

	if(Codec_Ready)
	{
	 	AC97_CodecInit_PD();

		//Normal
		Uart_Printf("\nNormal\n");
		AC97_Controller_State();
		Uart_Printf("AC97 Codec Powerdown Ctrl/Stat Reg. Value (at 0x26): 0x%x\n", AC97_Codec_Cmd(1,0x26,0x0000));	
	
		//ADCs off
		Uart_Printf("\n=>ADCs off PR0\n");
		AC97_Codec_Cmd(0,0x26,(1<<8));
		AC97_Controller_State();
		Uart_Printf("AC97 Codec Powerdown Ctrl/Stat Reg. Value (at 0x26): 0x%x\n", AC97_Codec_Cmd(1,0x26,0x0000));	
		
		//DACs off
		Uart_Printf("\n=>DACs off PR1\n");
		AC97_Codec_Cmd(0,0x26,(1<<8)|(1<<9));
		AC97_Controller_State();
		Uart_Printf("AC97 Codec Powerdown Ctrl/Stat Reg. Value (at 0x26): 0x%x\n", AC97_Codec_Cmd(1,0x26,0x0000));	

		//Analog off
		Uart_Printf("\n=>Analog off PR2\n");
		AC97_Codec_Cmd(0,0x26,(1<<8)|(1<<9)|(1<<10));
		AC97_Controller_State();
		Uart_Printf("AC97 Codec Powerdown Ctrl/Stat Reg. Value (at 0x26): 0x%x\n", AC97_Codec_Cmd(1,0x26,0x0000));	
		
		//Digital I/F off
		Uart_Printf("\n=>Digital I/F off PR4\n");
		AC97_Codec_Cmd(0,0x26,(1<<8)|(1<<9)|(1<<10)|(1<<12));
		AC97_Controller_State();
		
		//Shut off AC-Link
		Uart_Printf("\n=>Shut off AC-Link\n");
		rAC_GLBCTRL &= ~(1<<2);
		//AC97_Codec_Cmd(0,0x26,(1<<8)|(1<<9)|(1<<10)|(1<<12));
		AC97_Controller_State();

		//Warm Reset Timing Check
		Uart_Printf("\nWarm Reset Timing Check...");
		Uart_Printf("\n	Probe SYNC and BIT_CLK.");
		Uart_Printf("\n	Trigger SYNC Rising Edge.");
		
		while(1)
		{
			Uart_Printf("\nPress enter key for Warm Reset Timing Check...");
			i = Uart_GetIntNum();
			if(i==-1) break;	
		}

		Uart_Printf("\n=>Warm Reset\n");
		rAC_GLBCTRL = (1<<1);
		AC97_Controller_State();
		rAC_GLBCTRL &= ~(1<<1);

		rAC_GLBCTRL |= (1<<2);
		AC97_Controller_State();
		rAC_GLBCTRL |= (1<<3);
		Uart_Printf("AC97 Codec Powerdown Ctrl/Stat Reg. Value (at 0x26): 0x%x\n", AC97_Codec_Cmd(1,0x26,0x0000));	

		Uart_Printf("\nPress any key.\n");
		Uart_Getch();
		
		//Cold Reset Timing Check
		Uart_Printf("\nCold Reset Timing Check...");
		Uart_Printf("\n	Probe RESET#, BIT_CLK and SDATA_IN.");
		Uart_Printf("\n	Trigger RESET# Rising Edge.");


		while(1)
		{
			Uart_Printf("\nPress enter key for Cold Reset Timing Check...");
			i = Uart_GetIntNum();
			if(i==-1) break;	
		}

		rAC_GLBCTRL = 0x1;	
		Delay(1000);			
		rAC_GLBCTRL = 0x0;	
		Delay(1000);		

		Uart_Printf("\nPress any key to exit.\n");
		Uart_Getch();
	}

	Codec_Ready =1;
	AC97_Port_Return();	
}

/* Functional Sub-Routines */
void AC97_Port_Init(void)
{
	//Push AC97 GPIO port configuration
	save_AC97_rGPEDAT=rGPEDAT;
	save_AC97_rGPECON=rGPECON; 
	save_AC97_rGPEUP=rGPEUP;

	//---------------------------------------------------------------------
	//   PORT E GROUP
	//Ports  :  GPE4    		        GPE3                 GPE2            GPE1              GPE0 
	//Signal :  AC_SDATA_OUT   AC_SDATA_IN   AC_nRESET   AC_BIT_CLK   AC_SYNC 
	//Binary :   11,                     11,                   11,                11,                 11    
	//---------------------------------------------------------------------
	rGPECON = rGPECON & ~(0x3ff) | 0x3ff;   //GPE[4:0]=AC_SDATA_OUT   AC_SDATA_IN   AC_nRESET   AC_BIT_CLK   AC_SYNC
	rGPEUP  = rGPEUP  & ~(0x1f)  | 0x1f;    //The pull up function is disabled GPE[4:0] 1 1111
    	
	//For EINT0 Push Button 	
    	rGPFUP   = ((rGPFUP   & ~(1<<0)) | (1<<0));     //GPF0
    	rGPFCON  = ((rGPFCON  & ~(3<<0)) | (1<<1));     //GPF0=EINT0    
    	rEXTINT0 = ((rEXTINT0 & ~(7<<0)) | (2<<0));     //EINT0=falling edge triggered 
	
}

void AC97_Port_Return(void)
{
	//Pop AC97 GPIO port configuration
	rGPECON=save_AC97_rGPECON; 
	rGPEDAT=save_AC97_rGPEDAT;
	rGPEUP=save_AC97_rGPEUP;
}

void AC97_Init(void)
{
	int i=0;
//	U8 ch;

	Uart_Printf("\nAC97 Initialization...\n");	
	
 	//Cold Reset 
	rAC_GLBCTRL = 0x1;	// controller and codec cold reset
	Delay(1000);			// delay for controller safety reset
	rAC_GLBCTRL = 0x0;	// controller and codec normal mode
	Delay(1000);		
	rAC_GLBCTRL = 0x1;							
	Delay(1000);									
	rAC_GLBCTRL = 0x0;						
	Delay(1000);	
	
	//AC-link On
	rAC_GLBCTRL = (1<<2);
	Delay(1000);	
	AC97_Controller_State();

	//Transfer data enable using AC-link
	rAC_GLBCTRL |= (1<<3);	// AC97 Data transfer active 
	Delay(1000);	
	AC97_Controller_State();
	Uart_Printf("\nAC97-Link On...\n");
       
       //Codec Ready Check using Codec Ready Interrupt
       Codec_Ready_Irq =0;	
	pISR_WDT_AC97= (unsigned)AC97_Codec_Ready;	

	ClearPending(BIT_WDT_AC97);	
	rSUBSRCPND=(BIT_SUB_AC97);

   	rINTMSK=~(BIT_WDT_AC97);			
   	rINTSUBMSK=~(BIT_SUB_AC97);

   	rAC_GLBCTRL |= 0x400000;
   	
	while(!Codec_Ready_Irq)
	{

	 	Uart_Printf(".");
         	Delay(3000);
         	i++;
         	
         	if(i==20)
		break;
	 }

	Uart_Printf("\n");

	if(i==20)
	{
		Uart_Printf("\nAC97 codec is not ready.");
		Uart_Printf("\nCheck on connection between 2440A and AC97 Codec.\n");
		//Uart_Printf("\nRN1, RN3 and R280 instead of RN2, RN4 and R281 must be used for AC97 link on SMDK2440 Base Board (Rev 0.18).\n");
		Uart_Printf("\nBye. ");
		Codec_Ready = 0;
	}
}

void AC97_CodecInit_PD(void)
{
	Uart_Printf("\nAC97 Codec Soft Reset\n");
	AC97_Codec_Cmd(0,0x00,0x683F);		//Codec Soft Reset : 16bit In/Out  (stac9766/67) 	
	Uart_Printf("AC97 Codec 0x26 Reg.: 0x%x\n\n", AC97_Codec_Cmd(1,0x26,0x0000));		
}

void AC97_CodecInit_PCMIn( U16 AC97_fs)
{
	
	AC97_Codec_Cmd(0,0x00,0x683F);		//codec soft reset 	
	
	AC97_Codec_Cmd(0,0x2A,0x0001);		//variable rate enable	
	Uart_Printf("VRA Enable(1)/Disable(0): 0x%x\n",(0x1&AC97_Codec_Cmd(1,0x2A,0x0001)));
	
	if(AC97_fs==48000){
	//ADC Sampling frequency 48kHz
	AC97_Codec_Cmd(0,0x32,0xbb80);	
	}
	else if(AC97_fs==44100){
	//ADC Sampling frequency 44.1kHz
	AC97_Codec_Cmd(0,0x32,0xac44);
	}
	else if(AC97_fs==22050){
	//ADC Sampling frequency 22.05kHz
	AC97_Codec_Cmd(0,0x32,0x5622);	 	
	}
	
	AC97_Codec_Cmd(0,0x26,(1<<9));		//all power on except DAC Block
	Uart_Printf("\nAC97 Codec 0x26 Reg.: 0x%x\n\n", AC97_Codec_Cmd(1,0x26,0x0000));

	AC97_Codec_Cmd(0,0x10,0x1010);		//line in volume on
	AC97_Codec_Cmd(0,0x6e,0x0000);		//All Analog Mode, ADC Input select => left slot3, right slot4
	AC97_Codec_Cmd(0,0x1a,0x0505);		//record source select => Stereo Mix
	AC97_Codec_Cmd(0,0x1c,0x0909);		//record gain is initial
	AC97_Codec_Cmd(0,0x78,0x0001);		//ADC HPF Bypass
	AC97_Codec_Cmd(0,0x20,0x0000);		//General Reg.
	
	Input_Volume = 	AC97_Codec_Cmd(1,0x10,0x0000);	      //Line In volume	
	
}

void AC97_CodecInit_PCMOut( U16 AC97_fs)
{
	
	AC97_Codec_Cmd(0,0x00,0x683F);		//codec soft reset	

	AC97_Codec_Cmd(0,0x2A,0x0001);		//variable rate enable	
	//Uart_Printf("\nVRA Enable(1)/Disable(0): 0x%x\n", (0x1&AC97_Codec_Cmd(1,0x2A,0x0001)));
	
	if(AC97_fs==48000){
	//DAC Sampling frequency 48kHz
	AC97_Codec_Cmd(0,0x2C,0xbb80);
	}
	else if(AC97_fs==44100){
	//DAC Sampling frequency 44.1kHz
	AC97_Codec_Cmd(0,0x2C,0xac44);
	}
	else if(AC97_fs==22050){
	//DAC Sampling frequency 22.05kHz
	AC97_Codec_Cmd(0,0x2C,0x5622);		
	}

	AC97_Codec_Cmd(0,0x26, (1<<8));		// all power on except ADC blcok
	Uart_Printf("AC97 Codec 0x26 Reg.: 0x%x\n\n", AC97_Codec_Cmd(1,0x26,0x0000));

	AC97_Codec_Cmd(0,0x18,0x0000);		// PCM out volume on
	AC97_Codec_Cmd(0,0x20,0x0000);		// general purpose
	AC97_Codec_Cmd(0,0x04,0x1A1A);		// Aux out(HP out) volume on
	
	Output_Volume = AC97_Codec_Cmd(1,0x04,0x00000);	//HP out volume 	
}

void AC97_CodecInit_MICIn(U16 AC97_fs)
{
	AC97_Codec_Cmd(0,0x00,0x683F);		//codec soft reset 	
	
	AC97_Codec_Cmd(0,0x2A,0x0001);		//variable rate enable	
	Uart_Printf("VRA Enable(1)/Disable(0): 0x%x\n",(0x1&AC97_Codec_Cmd(1,0x2A,0x0001)));
	
	if(AC97_fs==48000){
	//ADC Sampling frequency 48kHz
	AC97_Codec_Cmd(0,0x32,0xbb80);	
	}
	else if(AC97_fs==44100){
	//ADC Sampling frequency 44.1kHz
	AC97_Codec_Cmd(0,0x32,0xac44);
	}
	else if(AC97_fs==22050){
	//ADC Sampling frequency 22.05kHz
	AC97_Codec_Cmd(0,0x32,0x5622);	 	
	}
	
	AC97_Codec_Cmd(0,0x26,(1<<9));		//all power on except DAC Block
	Uart_Printf("\nAC97 Codec 0x26 Reg.: 0x%x\n\n", AC97_Codec_Cmd(1,0x26,0x0000));
	
	AC97_Codec_Cmd(0,0x20,0x0000);		//MIC1 Selected
	AC97_Codec_Cmd(0,0x6e,0x0024);		//ADC Input Slot => left slot6, right slot9, MIC GAIN VAL =1 
	AC97_Codec_Cmd(0,0x0e,0x0040);		//BOOSTEN =1
	AC97_Codec_Cmd(0,0x1a,0x0000);		//Left, Right => MIC
	AC97_Codec_Cmd(0,0x1c,0xff);		
	AC97_Codec_Cmd(0,0x78,0x0001);		//ADC HPF Bypass
	
	Input_Volume = 	AC97_Codec_Cmd(1,0x1c,0x0000);	      //Record Volume	
}

U16 AC97_Codec_Cmd(U8 CMD_Read, U8 CMD_Offset, U16 CMD_Data)
{ 
	 U16 Codec_Stat;

	 if(CMD_Read == 0)
	 {
	 	rAC_CODEC_CMD = (0<<23)|(CMD_Offset<<16)|(CMD_Data<<0);
		Delay_After_CommandWrite(1);	//30us delay.  
    	rAC_CODEC_CMD |= (1<<23);	//To receive SLOTREQ bits when VRA is '1'.
	 }
      else if (CMD_Read ==1) 
     {
		rAC_CODEC_CMD = (1<<23)|(CMD_Offset<<16)|(CMD_Data<<0);
		Delay(1000);
		
    	Codec_Stat = (U16)(rAC_CODEC_STAT & 0xFFFF);
    	Delay(1000);
    	return Codec_Stat;
    }
    else 
    	 return 0;
    return 0;     
}

void PCM_In_Volume( U8 Up_Down_Volume)
{
	if( ( Up_Down_Volume == 'u') | (Up_Down_Volume == 'U') ) 
	{	
		if (Input_Volume == 0x0000) 
		{
			Uart_Printf("Limit Volume Range!\n");
		} 
		else 
		{
			Input_Volume -= 0x0101;
			AC97_Codec_Cmd(0,0x10, Input_Volume);	// PCM In Volume Up	
			Uart_Printf("PCM In Volume Level : 0x%x\n", Input_Volume);
		}
	}
				
	if ( ( Up_Down_Volume == 'd') | (Up_Down_Volume == 'D') ) 
	{	
		if (Input_Volume == 0x1F1F) 
		{
			Uart_Printf("Limit Volume Range!\n");
		} 
		else 
		{
			Input_Volume += 0x0101;
			AC97_Codec_Cmd(0,0x10, Input_Volume);	// PCM In Volume Down
			Uart_Printf("PCM In Volume Level : 0x%4x\n", Input_Volume);
		}
	}
}

void PCM_Out_Volume(U8 Up_Down_Volume)
{
	if( ( Up_Down_Volume == 'u') | (Up_Down_Volume == 'U') )
	{	
		if (Output_Volume == 0x0000) 
		{
			Uart_Printf("\nLimit Volume Range!");
		} 
		else 
		{
			Output_Volume -= 0x0101;
			AC97_Codec_Cmd(0,0x04, Output_Volume);		// PCM out Volume Up
			Uart_Printf("\nHeadphone Volume Level (In AC97 Codec 04h Reg.): 0x%x", Output_Volume);
		}
	}
				
	if ( ( Up_Down_Volume == 'd') | (Up_Down_Volume == 'D') ) 
	{	
		if (Output_Volume == 0x1F1F) 
		{
			Uart_Printf("\nLimit Volume Range!");
		} 
		else 
		{
			Output_Volume += 0x0101;
			AC97_Codec_Cmd(0,0x04, Output_Volume);		// PCM out Volume Down
			Uart_Printf("\nHeadphone Volume Level (In AC97 Codec 04h Reg.): 0x%x", Output_Volume);
		}
	}
}

void Download_PCM_File(void)
{
       pISR_UART1 = (unsigned)RxInt;

       rINTMSK    = ~( BIT_UART1);
       rINTSUBMSK = ~(BIT_SUB_RXD1);  

       AC97_BUF   = (U8 *)DOWN_BUF;
    	AC97_temp = AC97_BUF;
   
	Uart_Printf("\nDownload the PCM(no ADPCM) file via Serial Port Transmit in DNW (With header & CS)!\n");
	Uart_Printf("Max of PCM Size: 4M bytes\n");

	while(((unsigned int)AC97_temp - (unsigned int)AC97_BUF) < 4)
       {
       	Led_Display(0);
       	Delay(1500);
       	Led_Display(15);
       	Delay(1500);
    	}

    	AC97_size = *(AC97_BUF) | *(AC97_BUF + 1)<<8 | *(AC97_BUF + 2)<<16 | *(AC97_BUF + 3)<<24;
	
    	Uart_Printf("\nNow, Downloading... [ File Size : %7d(      0)]",AC97_size);

    	while(((unsigned int)AC97_temp - (unsigned int)AC97_BUF) < AC97_size)
       Uart_Printf("\b\b\b\b\b\b\b\b%7d)",(unsigned int)AC97_temp - (unsigned int)AC97_BUF);

    	Uart_Printf("\b\b\b\b\b\b\b\b%7d)]\n",(unsigned int)AC97_temp - (unsigned int)AC97_BUF);

    	rINTSUBMSK |= BIT_SUB_RXD1;

    	AC97_size = *(AC97_BUF + 0x2c) | *(AC97_BUF + 0x2d)<<8 | *(AC97_BUF + 0x2e)<<16 | *(AC97_BUF + 0x2f)<<24;
    	AC97_size = (AC97_size>>1)<<1;
    
       AC97_fs   = *(AC97_BUF + 0x1c) | *(AC97_BUF + 0x1d)<<8 | *(AC97_BUF + 0x1e)<<16 | *(AC97_BUF + 0x1f)<<24;

   	Play_AC97_BUF = (U32 *)(AC97_BUF + 0x30);	//DNW Header + PCM File Header => OffSet: 0x30
  	
 	Uart_Printf("Sample PCM Data Size = %d\n", AC97_size);
	Uart_Printf("Sampling Frequency = %d Hz\n", AC97_fs);
}


void AC97_PCMout_DMA1(U32 PCM_Size)
{
	pISR_EINT0= (unsigned)Muting;
	pISR_DMA1= (unsigned)DMA1_Play_Done;
	rINTMSK = ~(BIT_DMA1|BIT_EINT0);
	
	Play_AC97_BUF=(U32 *)(DOWN_BUF +0x30);
	
	//DMA Ch1 for PCMInitialize
	rDISRC1  = (int)Play_AC97_BUF;                 
	rDISRCC1 = (0<<1) + (0<<0);		//The source is in the system bus(AHB), Increment      
	rDIDST1  = ((U32)0x5b000018);  	//PCM Out Data Fifo    
	rDIDSTC1 = (1<<1) + (1<<0);           //The destination is in the peripheral bus(APB), Fixed  
	rDCON1   = ((unsigned int)1<<31)+(0<<30)+(1<<29)+(0<<28)+(0<<27)+(5<<24)+(1<<23)+(0<<22)+(2<<20)+(PCM_Size/4);
	rDMASKTRIG1 = (0<<2) + (1<<1) + (0<<0);          //No-stop[2], DMA1 channel On[1], No-sw trigger[0] 

	//AC97  Initialize
	Uart_Printf("\nConnect Head Phone Line In to CON21 (Head Phone Out) on SMDK2440 and Press Key.\n");
	Uart_Getch();
	Uart_Printf("\nNow Play...\n");
	Uart_Printf("To Volume Up, Press the 'u' key.\n");
	Uart_Printf("To Volume Down, Press the 'd' key.\n");
	Uart_Printf("To Mute On/Off, Press EINT0 Button.\n");
	Uart_Printf("\nIf you want to exit, Press the 'x' key.\n");
	Uart_Printf("Headphone Volume Register = 0x%x\n", Output_Volume);

	//Transfer data enable  using AC-Link
	rAC_GLBCTRL = 0x200C;
	//Uart_Printf("\nWrite Value to AC_GLBCTRL Reg. =>0x200C\n");
	//Uart_Printf("Read Value from AC_GLBCTRL Reg. =>0x%x\n", rAC_GLBCTRL);
	
	Delay(1000);    

	  	while(1)
	    	{
			//Uart_Printf("STAT3: 0x%x		CURR_TC: 0x%x		DCDST3: 0x%x\n", rDSTAT3&0x300000, rDSTAT3&0xfffff, rDCDST3);
	    		Up_Down_Volume=Uart_Getch();

			if( (Up_Down_Volume == 'x') | (Up_Down_Volume == 'X')) 
			break;
	    		PCM_Out_Volume(Up_Down_Volume);
	   	}

	ClearPending(BIT_DMA1); 

	rDMASKTRIG1  = (1<<2);       //DMA1 stop
	rINTMSK|= (BIT_DMA1 | BIT_EINT0);

	 //AC97 PCM In Channel Finish 
	 rAC_GLBCTRL &= ~(1<<13); //PCM Out Transfer Mode Off
	 Uart_Printf("\nEnd of Play!\n");
}


void AC97_PCMout_INT(U32 PCM_Size)
{
	 //Record AC97_BUF initialize
       Rec_AC97_BUF   = (U32 *)(DOWN_BUF);
	Play_AC97_BUF = (Rec_AC97_BUF + 0x30);
	End_AC97_BUF = (Rec_AC97_BUF + 0x30 + PCM_Size/4);

	//IRQ Initialization
	pISR_WDT_AC97= (unsigned)Irq_AC97_PCMout;	

	Uart_Printf("\nConnect Head Phone Line In to CON21 on SMDK2440 and Press any key.\n");
	Uart_Getch();
	Uart_Printf("\nNow Play...\n");
	Uart_Printf("Headphone Volume Register = 0x%x\n", Output_Volume);

	ClearPending(BIT_WDT_AC97);	
	rSUBSRCPND=(BIT_SUB_AC97);

   	rINTMSK=~(BIT_WDT_AC97);			
   	rINTSUBMSK=~(BIT_SUB_AC97);

	rAC_GLBCTRL = 0x4100C;	//PCM Out channel threshold INT enable, PIO Mode On

	while(1)
	{
	       if(PCM_Out_INT_Exit == 1)
		break;	
	}

	rAC_GLBCTRL &= ~(1<<12); //PCM Out Transfer PIO Mode Off
		
	rINTSUBMSK|=(BIT_SUB_WDT|BIT_SUB_AC97);
	rINTMSK|=(BIT_WDT_AC97);

	Uart_Printf("\nEnd of Play!\n");
}

void AC97_PCMin_DMA2(U32 PCM_Size)
{
	Rec_AC97_BUF   = (U32 *)(DOWN_BUF);  

	//IRQ Initialize	
	pISR_DMA2  = (unsigned)DMA2_Rec_Done;
	rINTMSK = ~(BIT_DMA2);
	      
       //DMA2 Initialize
	rDISRC2  = ((U32)0x5B000018);         //PCM Input Data FIFO
	rDISRCC2 = (1<<1) + (1<<0);          //APB, Fix  
       rDIDST2  = (int)Rec_AC97_BUF;   //Record AC97_BUF initializ
       rDIDSTC2 = (0<<1) + (0<<0);          //AHB, Increment
       rDCON2   = ((unsigned int)1<<31)+(0<<30)+(1<<29)+(0<<28)+(0<<27)+(5<<24)+(1<<23)+(1<<22)+(2<<20)+(PCM_Size/4);

	Uart_Printf("Connect Sound Line Out to CON25(Line In) on SMDK2440\n");
       Uart_Printf("Press any key to start record.\n");
    	Uart_Getch();
   	Uart_Printf("Recording...\n");

       rDMASKTRIG2 = (0<<2) + (1<<1) + 0;    //No-stop, DMA2 channel on, No-sw trigger

	// Transfer data enable  using AC-Link
       rAC_GLBCTRL = 0x80C;	// Transfer data enable  using AC-Link

       //Uart_Printf("\nWrite Value to AC_GLBCTRL Reg. =>0x80C\n");
	//Uart_Printf("Read Value from AC_GLBCTRL Reg. =>0x%x\n", rAC_GLBCTRL);

       while(AC97_Rec_Done ==0)
       {
        	Uart_Printf(".");
         	Delay(3000);
         	//Uart_Printf("STAT2: 0x%x		CURR_TC: 0x%x		DCDST2: 0x%x\n", rDSTAT2&0x300000, rDSTAT2&0xfffff, rDCDST2);
       }
    		
       AC97_Rec_Done = 0;
       rDMASKTRIG2 = (1<<2);     //DMA2 stop
     
       rINTMSK |= BIT_DMA2;

       //AC97 PCM In Channel Finish 
       rAC_GLBCTRL &= ~(1<<11); //PCM In Transfer Mode Off
  	Delay(1000); 

  	Uart_Printf("\nEnd of Record!\n");
}

void AC97_PCMin_INT(U32 PCM_Size)
{
	//Record AC97_BUF initialize
       Rec_AC97_BUF   = (U32 *)(DOWN_BUF);
	Play_AC97_BUF = Rec_AC97_BUF + 0x30;
	End_AC97_BUF = (Rec_AC97_BUF + 0x30 + PCM_Size/4);

	//IRQ Initialization
	pISR_WDT_AC97= (unsigned)Irq_AC97_PCMin;	

	Uart_Printf("Connect Sound Line Out to CON25(Line In) on SMDK2440\n");
       Uart_Printf("Press any key to start record.\n");
    	Uart_Getch();
   	Uart_Printf("Recording...\n");

	ClearPending(BIT_WDT_AC97);	
	rSUBSRCPND=(BIT_SUB_AC97);

   	rINTMSK=~(BIT_WDT_AC97);			
   	rINTSUBMSK=~(BIT_SUB_AC97);
		
	rAC_GLBCTRL = 0x2040C;	//PCM In channel threshold INT enable, PIO Mode On
	
	while(1)
	{
	       if(PCM_In_INT_Exit == 1)
		break;	
	}

	rAC_GLBCTRL &= ~(1<<10); //PCM In Transfer PIO Mode Off
		
	rINTSUBMSK|=(BIT_SUB_AC97);
	rINTMSK|=(BIT_WDT_AC97);

	Uart_Printf("\nEnd of Record!\n");	
}

void AC97_MICin_DMA3(U32 MIC_Size)
{
	Rec_AC97_BUF   = (U32 *)(DOWN_BUF);  

	//IRQ Initialize	
	pISR_DMA3  = (unsigned)DMA3_Rec_Done;
	rINTMSK=~(BIT_DMA3);
	
       //DMA3 Initialize
	rDISRC3  = ((U32)0x5B00001C);         //MIC Input Data FIFO
	rDISRCC3 = (1<<1) + (1<<0);          //APB, Fix  
       rDIDST3  = (int)Rec_AC97_BUF;   //Record AC97_BUF initializ
       rDIDSTC3 = (0<<1) + (0<<0);          //AHB, Increment
       rDCON3   = ((unsigned int)1<<31)+(0<<30)+(1<<29)+(0<<28)+(0<<27)+(5<<24)+(1<<23)+(1<<22)+(2<<20)+(MIC_Size/4);

	Uart_Printf("Are you ready to record voice via MIC on SMDK2440?\n");
       Uart_Printf("Press any key to start record.\n");
    	Uart_Getch();
   	Uart_Printf("Recording...\n");

       rDMASKTRIG3 = (0<<2) + (1<<1) + 0;    //No-stop, DMA3 channel on, No-sw trigger

	// Transfer data enable  using AC-Link
       rAC_GLBCTRL = 0x20C;	// Transfer data enable  using AC-Link

       while(AC97_Rec_Done ==0)
       {
        	Uart_Printf(".");
        	Delay(3000);
        	//Uart_Printf("STAT3: 0x%x		CURR_TC: 0x%x		DCDST3: 0x%x\n", rDSTAT3&0x300000, rDSTAT3&0xfffff, rDCDST3);
       }
    		
       AC97_Rec_Done = 0;
       rDMASKTRIG3 = (1<<2);     //DMA3 stop
     
       rINTMSK |= BIT_DMA3;

       //AC97 MIC In Channel Finish 
       rAC_GLBCTRL &= ~(1<<9); //MIC In Transfer Mode Off
 
  	Uart_Printf("\nEnd of Record!\n");	
}

void AC97_MICin_INT(U32 PCM_Size)
{
	//Record AC97_BUF initialize
       Rec_AC97_BUF   = (U32 *)(DOWN_BUF);
	Play_AC97_BUF = Rec_AC97_BUF + 0x30;
	End_AC97_BUF = (Rec_AC97_BUF + 0x30 + PCM_Size/4);

	//IRQ Initialization
	pISR_WDT_AC97= (unsigned)Irq_AC97_MICin;	

	Uart_Printf("Are you ready to record voice via MIC on SMDK2440?\n");
       Uart_Printf("Press any key to start record.\n");
    	Uart_Getch();
   	Uart_Printf("Recording...\n");

	ClearPending(BIT_WDT_AC97);	
	rSUBSRCPND=(BIT_SUB_AC97);

   	rINTMSK=~(BIT_WDT_AC97);			
   	rINTSUBMSK=~(BIT_SUB_AC97);
		
	rAC_GLBCTRL = 0x1010C;	//MIC In channel threshold INT enable, PIO Mode On
	
	while(1)
	{
	       if(PCM_In_INT_Exit == 1)
		break;	
	}

	rAC_GLBCTRL &= ~(1<<8); //MIC In Transfer PIO Mode Off
		
	rINTSUBMSK|=(BIT_SUB_AC97);
	rINTMSK|=(BIT_WDT_AC97);

	Uart_Printf("\nEnd of Record!\n");
}

void AC97_CodecExit_PCMOut(void)
{
	//DACs off
	Uart_Printf("\n\n=>DACs off PR1\n");
	AC97_Codec_Cmd(0,0x26,(1<<8)|(1<<9));
	AC97_Controller_State();
	Uart_Printf("AC97 Codec Powerdown Ctrl/Stat 0x26 Reg.: 0x%x\n", AC97_Codec_Cmd(1,0x26,0x0000));	

	//Analog off
	Uart_Printf("\n=>Analog off PR2\n");
	AC97_Codec_Cmd(0,0x26,(1<<8)|(1<<9)|(1<<10));
	AC97_Controller_State();
	Uart_Printf("AC97 Codec Powerdown Ctrl/Stat 0x26 Reg.: 0x%x\n", AC97_Codec_Cmd(1,0x26,0x0000));	

	//Digital I/F off
	Uart_Printf("\n=>Digital I/F off PR4\n");
	AC97_Codec_Cmd(0,0x26,(1<<8)|(1<<9)|(1<<10)|(1<<12));
	AC97_Controller_State();
	Uart_Printf("AC97 Codec Powerdown Ctrl/Stat 0x26 Reg.: 0x%x\n", AC97_Codec_Cmd(1,0x26,0x0000));	

}

void AC97_CodecExit_PCMIn(U16 DACs_off)
{
	//ADCs off
	Uart_Printf("\n\n=>ADCs off PR0\n");
	AC97_Codec_Cmd(0,0x26,(1<<8));
	AC97_Controller_State();
	Uart_Printf("AC97 Codec Powerdown Ctrl/Stat 0x26 Reg.: 0x%x\n", AC97_Codec_Cmd(1,0x26,0x0000));

	if(DACs_off == 1)
	{
		//DACs off
		Uart_Printf("\n\n=>DACs off PR1\n");
		AC97_Codec_Cmd(0,0x26,(1<<8)|(1<<9));
		AC97_Controller_State();
		Uart_Printf("AC97 Codec Powerdown Ctrl/Stat 0x26 Reg.: 0x%x\n", AC97_Codec_Cmd(1,0x26,0x0000));
	}
	//Analog off
	Uart_Printf("\n=>Analog off PR2\n");
	AC97_Codec_Cmd(0,0x26,(1<<8)|(1<<9)|(1<<10));
	AC97_Controller_State();
	Uart_Printf("AC97 Codec Powerdown Ctrl/Stat 0x26 Reg.: 0x%x\n", AC97_Codec_Cmd(1,0x26,0x0000));	

	//Digital I/F off
	Uart_Printf("\n=>Digital I/F off PR4\n");
	AC97_Codec_Cmd(0,0x26,(1<<8)|(1<<9)|(1<<10)|(1<<12));
	AC97_Controller_State();
	Uart_Printf("AC97 Codec Powerdown Ctrl/Stat 0x26 Reg.: 0x%x\n", AC97_Codec_Cmd(1,0x26,0x0000));	
}

void AC97_CodecExit_MICIn(U16 DACs_off)
{
	//ADCs off
	Uart_Printf("\n\n=>ADCs off PR0\n");
	AC97_Codec_Cmd(0,0x26,(1<<8));
	AC97_Controller_State();
	Uart_Printf("AC97 Codec Powerdown Ctrl/Stat 0x26 Reg.: 0x%x\n", AC97_Codec_Cmd(1,0x26,0x0000));

	if(DACs_off == 1)
	{
		//DACs off
		Uart_Printf("\n\n=>DACs off PR1\n");
		AC97_Codec_Cmd(0,0x26,(1<<8)|(1<<9));
		AC97_Controller_State();
		Uart_Printf("AC97 Codec Powerdown Ctrl/Stat 0x26 Reg.: 0x%x\n", AC97_Codec_Cmd(1,0x26,0x0000));
	}
	
	//Analog off
	Uart_Printf("\n=>Analog off PR2\n");
	AC97_Codec_Cmd(0,0x26,(1<<8)|(1<<9)|(1<<10));
	AC97_Controller_State();
	Uart_Printf("AC97 Codec Powerdown Ctrl/Stat 0x26 Reg.: 0x%x\n", AC97_Codec_Cmd(1,0x26,0x0000));	

	//Digital I/F off
	Uart_Printf("\n=>Digital I/F off PR4\n");
	AC97_Codec_Cmd(0,0x26,(1<<8)|(1<<9)|(1<<10)|(1<<12));
	AC97_Controller_State();
	Uart_Printf("AC97 Codec Powerdown Ctrl/Stat 0x26 Reg.: 0x%x\n", AC97_Codec_Cmd(1,0x26,0x0000));	

}

U16 AC97_Select_SamplingRate(void)
{
	int i;
	
	Uart_Printf("\nSelect ADC/DAC Rate\n");
	Uart_Printf("0:22.05kHz, 1:44.1kHz, 2:48kHz\n");
	i = Uart_GetIntNum();	

	   switch(i)
       {
		case 0:
              return 22050;
//       	break;
       	
       	case 1:
       	return 44100;
//       	break;

       	case 2:
       	return 48000;
//       	break;
       
       	default:
       	return 48000;
//       	break;
	}      
}

void AC97_Controller_State(void)
{
	U8 state;
    
    	state=(U8)(rAC_GLBSTAT);

     	switch(state)
    	{
		case 0:
		Uart_Printf("AC97 Controller State: Idle\n"); 
		break;
	
		case 1:
			Uart_Printf("AC97 Controller State: Init\n"); 
		break;

		case 2:
		       Uart_Printf("AC97 Controller State: Ready\n"); 
		break;
		
		case 3:
			Uart_Printf("AC97 Controller State: Active\n"); 
		break;

		case 4:
			Uart_Printf("AC97 Controller State: LP\n"); 
		break;

		case 5:
			Uart_Printf("AC97 Controller State: Warm\n"); 
		break;

		default:
		break;
     	}
}

void Delay_After_CommandWrite(int time)
{
      	// time=0: adjust the Delay function by WatchDog timer.
      	// time>0: the number of loop time
      	// Delay time resolution has 30us.

 	int i, adjust=0;

    	if(time==0)
    	{
       	time   = 300;
        	adjust = 1;
        	delayLoopCount = 500;

            	//PCLK/1M,Watch-dog disable,1/64,interrupt disable,reset disable
        	rWTCON = ((PCLK/1000000-1)<<8)|(2<<3); 
        	rWTDAT = 0xffff;                              //for first update
        	rWTCNT = 0xffff;                              //resolution=64us @any PCLK 
        	rWTCON = ((PCLK/1000000-1)<<8)|(2<<3)|(1<<5); //Watch-dog timer start
    	}

    	for(;time>0;time--)
       	for(i=0;i<delayLoopCount;i++);

    	if(adjust==1)
    	{
        	rWTCON = ((PCLK/1000000-1)<<8)|(2<<3); //Watch-dog timer stop
        	i = 0xffff - rWTCNT;                     //1 count  => 64us,  300*500 cycle runtime = 64*i us
        	delayLoopCount = 4500000/(i*64);         //300*500:64*i=1*x:30 -> x=150000*30/(64*i)   
    	}
    		
}

void Delay_Init(void)
{
	Delay(0);	//Delay Time Resolution => 100us Adjust 
	Delay_After_CommandWrite(0);	//Delay Time Resolution => 30us Adjust 
}

/* ISRs */
void __irq AC97_Codec_Ready(void)
{
	Delay_Init();

	if ( (rAC_GLBSTAT& 0x400000))
	{
		Codec_Ready_Irq=1;
		Uart_Printf("Codec Ready!\n");	
		rAC_GLBCTRL &= ~(0x400000);	// codec ready interrupt disable
	}

	rSUBSRCPND=(BIT_SUB_AC97);
	ClearPending(BIT_WDT_AC97);

	rINTSUBMSK &= ~(BIT_SUB_AC97);
	rINTMSK &=~(BIT_WDT_AC97);
}

void __irq DMA1_Play_Done(void)
{
      	ClearPending(BIT_DMA1);               
	Uart_Printf("\n~~~");
}

void __irq DMA2_Rec_Done(void)
{
    	ClearPending(BIT_DMA2);               
    	AC97_Rec_Done = 1;
}

void __irq DMA3_Rec_Done(void)
{
	ClearPending(BIT_DMA3);               
    	AC97_Rec_Done = 1;	    	
}

void __irq Irq_AC97_PCMout(void)
{
	U32 i, AC97_Stat;
	
	rINTMSK |=(BIT_WDT_AC97);
	rINTSUBMSK|=(BIT_SUB_AC97);		

	AC97_Stat = rAC_GLBSTAT;
	
	if (AC97_Stat & AC97_PCM_OUT_THRESHOLD)
	{
			for(i=0; i<PCM_OUT_TRIGGER; i++)
		       {
			 	rAC_PCMDATA = *(Play_AC97_BUF++);			

				if(Play_AC97_BUF == End_AC97_BUF)  
				break;	
			}			
	}

	if(Play_AC97_BUF == End_AC97_BUF)
	{
		rAC_GLBCTRL &= ~(0x40000);	//PCM Out channel threshold INT disable
		PCM_Out_INT_Exit =1;	
	}
	
	rSUBSRCPND=(BIT_SUB_AC97);
	ClearPending(BIT_WDT_AC97);

	rINTSUBMSK &= ~(BIT_SUB_AC97);	
	rINTMSK &= ~(BIT_WDT_AC97);	 	
}

void __irq Irq_AC97_PCMin(void)
{
	U32 AC97_Stat, i; 

	rINTMSK |=(BIT_WDT_AC97);
	rINTSUBMSK|=(BIT_SUB_AC97);	

	AC97_Stat = rAC_GLBSTAT;

	if (AC97_Stat & AC97_PCM_IN_THRESHOLD)
	{
			for(i=0; i<PCM_IN_TRIGGER; i++)
		       {
			 	 *(Play_AC97_BUF++) = rAC_PCMDATA;			

				if(Play_AC97_BUF == End_AC97_BUF)  
				break;	
			}			
	}

	if(Play_AC97_BUF == End_AC97_BUF)
	{
		rAC_GLBCTRL &= ~(1<<17);	//PCM In channel threshold INT disable
		PCM_In_INT_Exit =1;	
	}

	rSUBSRCPND=(BIT_SUB_AC97);
	ClearPending(BIT_WDT_AC97);

	rINTSUBMSK &= ~(BIT_SUB_AC97);	
	rINTMSK &= ~(BIT_WDT_AC97);	
}

void __irq Irq_AC97_MICin(void)
{
	U32 AC97_Stat, i; 

	rINTMSK |=(BIT_WDT_AC97);
	rINTSUBMSK|=(BIT_SUB_AC97);	

	AC97_Stat = rAC_GLBSTAT;

	if (AC97_Stat & AC97_MIC_IN_THRESHOLD)
	{
			for(i=0; i<PCM_IN_TRIGGER; i++)
		       {
			 	 *(Play_AC97_BUF++) = rAC_MICDATA;			

				if(Play_AC97_BUF == End_AC97_BUF)  
				break;	
			}			
	}

	if(Play_AC97_BUF == End_AC97_BUF)
	{
		rAC_GLBCTRL &= ~(1<<16);	//PCM In channel threshold INT disable
		PCM_In_INT_Exit =1;	
	}

	rSUBSRCPND=(BIT_SUB_AC97);
	ClearPending(BIT_WDT_AC97);

	rINTSUBMSK &= ~(BIT_SUB_AC97);	
	rINTMSK &= ~(BIT_WDT_AC97);	
}

void __irq RxInt(void)
{
    	rSUBSRCPND = BIT_SUB_RXD1;         
    	rSUBSRCPND;
    	ClearPending(BIT_UART1);

    	*AC97_temp ++= RdURXH1(); 
}

void __irq Muting(void)
{

	ClearPending(BIT_EINT0);               

    	if(AC97_mute)    //AC97_mute
    	{
     		AC97_mute = 0;
     		AC97_Codec_Cmd(0,0x04, Output_Volume|0x8000);		
        	Uart_Printf("\nAC97 Mute On...\n");
    	}
    	else        //No AC97_mute
    	{

        	AC97_mute = 1;
        	AC97_Codec_Cmd(0,0x04, Output_Volume& ~(0x8000));
        	Uart_Printf("\nAC97 Mute Off...\n");
    	}
}

