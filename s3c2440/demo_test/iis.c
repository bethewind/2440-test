/*=================================================

	File Name: Iis.c
	Description: S3C2440A IIS controller Function Test
   	Version: 0.1 

   	History:
             0.0: 2002. 03. 06, Programming Start by SOP.
             0.1: 2004. 02. 04, Modified by Y. H. Lee.
		  - IIS Master Tx: Play Sample Wave File.
		  - IIS Master Rx: Record Sound using MIC and Play it.
		  - IIS maser clock selection is supported.
		  
===================================================*/

#include "2440addr.h"
#include "2440lib.h"
#include "def.h"
#include "iis.h"

#define REC_LEN_IIS 0x200000	
#define PLAY_IIS    0
#define RECORD_IIS  1
 
//#define DataCount   0x10000     //IIS Master/Slave Data Rx/Tx Count
//#define DataDisplay 0x100		//IIS Master Data Display Count
//#define PollMode    0	     	//Polling Mode
//#define DMA2Mode    1	//DMA2 Mode

#define L3C (1<<4)		  //GPB4 = L3CLOCK
#define L3D (1<<3)		  //GPB3 = L3DATA
#define L3M (1<<2)		  //GPB2 = L3MODE

char which_Buf = 1;
char Rec_Done = 0;
char mute = 1;
char IIS_MasterClk_Sel = 0; 

unsigned char *rec_buf;
unsigned char  *Buf,*_temp;
unsigned int size, fs; 
unsigned int save_B, save_E, save_PB, save_PE;

extern U32 Pclk;

float IIS_Codec_CLK;

//extern void Calc_Clock(int print_msg);
//extern U32 Mdiv, Pdiv, Sdiv, Fclk, Hclk, Pclk, Hdivn, Pdivn, Hclk_Ratio, Pclk_Ratio, Ref_Cnt;

void	ChangeDMA2(void);
void IIS_Port_Init(void);
void IIS_Port_Return(void);
void Download_Wave_File(void);

void Select_IIS_Master_CLK(void);
void IIS_RecSound_DMA1(unsigned char *start_addr, U32 rec_size);
void IIS_PlayWave_DMA2(unsigned char *start_addr, U32 play_size);

void _WrL3Addr(U8 data);
void _WrL3Data(U8 data,int halt);

void __irq DMA1_Rec_Done(void);
void __irq DMA2_Done(void);
void __irq RxInt(void);
void __irq Muting(void);

void * func_iis_test[][2]=
{	
	//IIS Function Test Item
	(void *)Play_Wave_Iis,		"Play Wave File. ",
    	(void *)Record_Sound_Iis,		"Record Sound via MIC and Play it. ",
    	0,0
};

void IIS_Test(void)
{
	int i;
		
	while(1)
	{
		i=0;
		Uart_Printf("\n\n================== IIS Function Test ==================\n\n");

		Uart_Printf("1: Play Wave File. \n");
		Uart_Printf("2: Record Sound via MIC and Play it.\n");
		

		Uart_Printf("\n=======================================================\n");
		Uart_Printf("\nPress Enter key to exit : ");
		i = Uart_GetIntNum();
		if(i==-1) break;		// return.
//		if(i>=0 && (i<((sizeof(func_iis_test)-1)/8)) )	// select and execute...
//			( (void (*)(void)) (func_iis_test[i][0]) )();
         switch(i)
		 {
		     case 1:  Play_Wave_Iis();
			          break;
		     case 2:  Record_Sound_Iis();
			          break;
		     default: Uart_Printf("Please enter(1~2) to choose!");
			          break;					  		 
		 }
	}
	
	//Uart_Printf("\n====== IIS Test program end ======\n");
}

//Play Wave File
void Play_Wave_Iis(void)
{
	Uart_Printf("\nPlay Wave File.\n");

    	IIS_Port_Init();
    	Select_IIS_Master_CLK();

	Download_Wave_File();

	Init1341(PLAY_IIS);
	IIS_PlayWave_DMA2(Buf + 0x30, size);

	IIS_Port_Return();
    
    	mute = 1;
}

//Record Sound using MIC
void Record_Sound_Iis(void)
{
	
	Uart_Printf("\nRecord Sound via MIC.\n");

    	IIS_Port_Init(); 
	Select_IIS_Master_CLK();
	
	Init1341(RECORD_IIS);

	rec_buf = (unsigned char *)0x31000000;
	IIS_RecSound_DMA1(rec_buf, REC_LEN_IIS);

      	////////////////////////////////////////////////////////////////////////
	
	Init1341(PLAY_IIS);
	IIS_PlayWave_DMA2(rec_buf, REC_LEN_IIS);

	IIS_Port_Return();
	
	mute = 1;
}


/* Sub-Routines */ 

//Setting Port related to IIS  
void IIS_Port_Init(void)
{
	save_B  = rGPBCON;	 
    	save_E  = rGPECON;	 
    	save_PB = rGPBUP;
    	save_PE = rGPEUP;
    	
	//----------------------------------------------------------
	//   PORT B GROUP
	//Ports  :   GPB4    GPB3   GPB2  
	//Signal :  L3CLOCK L3DATA L3MODE
	//Setting:   OUTPUT OUTPUT OUTPUT 
	//	     [9:8]   [7:6}  [5:4]
	//Binary :     01  ,   01    01 
	//----------------------------------------------------------    
    	rGPBUP  = rGPBUP  & ~(0x7<<2) | (0x7<<2);   //The pull up function is disabled GPB[4:2] 1 1100    
    	rGPBCON = rGPBCON & ~(0x3f<<4) | (0x15<<4); //GPB[4:2]=Output(L3CLOCK):Output(L3DATA):Output(L3MODE)

	//----------------------------------------------------------
	//   PORT E GROUP
	//Ports  :  GPE4    GPE3   GPE2  GPE1    GPE0 
	//Signal : I2SSDO  I2SSDI CDCLK I2SSCLK I2SLRCK 
	//Binary :   10  ,   10     10 ,  10	10    
	//----------------------------------------------------------
    	rGPEUP  = rGPEUP  & ~(0x1f)  | 0x1f;    //The pull up function is disabled GPE[4:0] 1 1111
    	rGPECON = rGPECON & ~(0x3ff) | 0x2aa;   //GPE[4:0]=I2SSDO:I2SSDI:CDCLK:I2SSCLK:I2SLRCK

    	rGPFUP   = ((rGPFUP   & ~(1<<0)) | (1<<0));     //GPF0
    	rGPFCON  = ((rGPFCON  & ~(3<<0)) | (1<<1));     //GPF0=EINT0    
    	rEXTINT0 = ((rEXTINT0 & ~(7<<0)) | (2<<0));     //EINT0=falling edge triggered  
}

void IIS_Port_Return(void)
{
	rGPBCON = save_B;
    	rGPECON = save_E;
    	rGPBUP  = save_PB;
    	rGPEUP  = save_PE;
}

void Download_Wave_File(void)
{
    //	unsigned int temp;
	
    	pISR_UART1 = (unsigned)RxInt;

    	rINTMSK    = ~(BIT_UART1);
    	rINTSUBMSK = ~(BIT_SUB_RXD1);

    	//Non-cacheable area = 0x31000000 ~ 0x33feffff
    	Buf   = (unsigned char *)0x31000000;
    	_temp = Buf;

       Uart_Printf("\n\nDownload the PCM(no ADPCM) file by DNW serial port(With header)!!\n");
       Uart_Printf("Download Start Address: 0x%x\n", Buf);

       while(((unsigned int)_temp - (unsigned int)Buf) < 4)
       {
   	 	Led_Display(0);
   	  	Delay(1500);
   	  	Led_Display(15);
   		Delay(1500);
       }
       
       size = *(Buf) | *(Buf + 1)<<8 | *(Buf + 2)<<16 | *(Buf + 3)<<24;
       Uart_Printf("\nNow, Downloading... [ File Size : %7d  	    0]", size);
	 //  temp =size;
      
    	while(((unsigned int)_temp - (unsigned int)Buf) < size) 
    	{
	  	Uart_Printf("\b\b\b\b\b\b\b\b%7d ",(unsigned int)_temp - (unsigned int)Buf);
		Delay(5000);
    	}
    	Uart_Printf("\b\b\b\b\b\b\b\b%7d ]\n",(unsigned int)_temp - (unsigned int)Buf);

	rINTSUBMSK |= BIT_SUB_RXD1;
   
       size = *(Buf + 0x2c) | *(Buf + 0x2d)<<8 | *(Buf + 0x2e)<<16 | *(Buf + 0x2f)<<24;
       size = (size>>1)<<1;
       //size = temp -0x30;

       fs   = *(Buf + 0x1c) | *(Buf + 0x1d)<<8 | *(Buf + 0x1e)<<16 | *(Buf + 0x1f)<<24;

       Uart_Printf("PCM File Size = %d\n", size);
       Uart_Printf("Sampling Frequency of PCM data = %d Hz\n", fs);

}

void IIS_RecSound_DMA1(unsigned char *start_addr, U32 rec_size)
{
	pISR_DMA1  = (unsigned)DMA1_Rec_Done;

	if (IIS_MasterClk_Sel == 0)	//IIS Master Clock Source = PCLK
      	{
      		rIISCON = (1<<4) + (1<<3) + (1<<1);	   
      		//Bit[1]    IIS prescaler enable
		//Bit[3]    Tx idle 
      		//Bit[4]    Rx DMA enable 
    		rIISMOD = (0<<9)+(0<<8) + (1<<6) + (0<<5) + (0<<4) + (1<<3) + (1<<2) + (1<<0);
    		//Bit[1:0] Serial bit clock => 32fs,
    		//Bit[2]    Master clock => 384fs, 
    		//Bit[3]    Serial data bit per channel => 16bit,
    		//Bit[4]    Serial interface format => IIS compatible format
    		//Bit[5]    Active level of left/right channel => Low for left channel
    		//Bit[7:6] Tx/Rx mode select => Rx mode
    		//Bit[8]    Master/Slave mode => Master mode
    		//Bit[9]    Master clock => PCLK
    		rIISFCON = (1<<14) + (1<<12);	  
    		//Bit[13]  Rx FIFO enable
		//Bit[15]  Rx FIFO access mode => DMA   
		
		Uart_Printf("\nIISLRCK = %d Hz", (int) IIS_Codec_CLK/384);
      	}
      	else	   //IIS Master Clock Source = MPLLin
      	{
		rIISCON = (1<<4) + (1<<3) + (1<<1);	   
      		//Bit[1]    IIS prescaler enable
		//Bit[3]    Tx idle 
      		//Bit[4]    Rx DMA enable 
    		rIISMOD = (0<<9)+(0<<8) + (1<<6) + (0<<5) + (0<<4) + (1<<3) + (1<<2) + (1<<0);
    		//Bit[1:0] Serial bit clock => 32fs,
    		//Bit[2]    Master clock => 384fs, 
    		//Bit[3]    Serial data bit per channel => 16bit,
    		//Bit[4]    Serial interface format => IIS compatible format
    		//Bit[5]    Active level of left/right channel => Low for left channel
    		//Bit[7:6] Tx/Rx mode select => Rx mode
    		//Bit[8]    Master/Slave mode => Master mode
    		//Bit[9]    Master clock => PCLK
    		rIISFCON = (1<<14) + (1<<12);	  
    		//Bit[13]  Rx FIFO enable
		//Bit[15]  Rx FIFO access mode => DMA   

		Uart_Printf("\nIISLRCK = %d Hz", (int) IIS_Codec_CLK/384);
      	}

 	rINTMSK = ~(BIT_DMA1);

	//--- DMA1 Initialize
       rDISRCC1 = (1<<1) + (1<<0);   
       rDISRC1  = ((U32)IISFIFO);    
       rDIDSTC1 = (0<<1) + (0<<0);     
       rDIDST1  = (int)start_addr;                            
       rDCON1   = ((unsigned int)1<<31)+(0<<30)+(1<<29)+(0<<28)+(0<<27)+(2<<24)+(1<<23)+(1<<22)+(1<<20)+(rec_size/2);
       rDMASKTRIG1 = (0<<2) + (1<<1) + 0;    //No-stop, DMA1 channel on, No-sw trigger

	Uart_Printf("\n\nAre you ready to record voice via MIC on SMDK2440?");
       Uart_Printf("\nPress any key to start record!\n");
       Uart_Getch();
       Uart_Printf("Recording...\n");
   
   	//IIS Rx start
       rIISCON |= 0x1;
   
       while(!Rec_Done)
	{
        	Uart_Printf(".");
         	Delay(2000);
       }
   
       Rec_Done = 0;

	//IIS Rx stop
       Delay(10);				//For end of H/W Rx
       rIISCON     = 0x0;			//IIS stop
       rDMASKTRIG1 = (1<<2);	//DMA1 stop
	rIISFCON    = 0x0;			//For FIFO flush

	rINTMSK  |= (BIT_DMA1);
    	Uart_Printf("\nEnd of Record!\n");	
}


void IIS_PlayWave_DMA2(unsigned char *start_addr, U32 play_size)
{
	pISR_DMA2  = (unsigned)DMA2_Done;
    	pISR_EINT0 = (unsigned)Muting;

    	if (IIS_MasterClk_Sel == 0)	//IIS Master Clock Source = PCLK
      	{
      		rIISCON = (1<<5) + (1<<2) + (1<<1);	   
      		//Bit[1]    IIS prescaler enable
		//Bit[2]    Rx idle 
      		//Bit[5]    Tx DMA enable 
    		rIISMOD = (0<<9)+(0<<8) + (2<<6) + (0<<5) + (0<<4) + (1<<3) + (1<<2) + (1<<0);
    		//Bit[1:0] Serial bit clock => 32fs,
    		//Bit[2]    Master clock => 384fs, 
    		//Bit[3]    Serial data bit per channel => 16bit,
    		//Bit[4]    Serial interface format => IIS compatible format
    		//Bit[5]    Active level of left/right channel => Low for left channel
    		//Bit[7:6] Tx/Rx mode select => Tx mode
    		//Bit[8]    Master/Slave mode => Master mode
    		//Bit[9]    Master clock => PCLK
    		rIISFCON = (1<<15) + (1<<13);	  
    		//Bit[13]  Tx FIFO enable
		//Bit[15]  Tx FIFO access mode => DMA   
		
		Uart_Printf("\nIISLRCK = %d Hz", (int) IIS_Codec_CLK/384);
      	}
      	else	   //IIS Master Clock Source = MPLLin
      	{
		rIISCON = (1<<5) + (1<<2) + (0<<1);	   
		//Bit[1]    IIS prescaler disable
		//Bit[2]    Rx idle 
      		//Bit[3]    Tx DMA enable 
    		rIISMOD = (1<<9)+(0<<8) + (2<<6) + (0<<5) + (0<<4) + (1<<3) + (1<<2) + (1<<0);
		//Bit[1:0] Serial bit clock => 32fs,
    		//Bit[2]    Master clock => 384fs, 
    		//Bit[3]    Serial data bit per channel => 16bit,
    		//Bit[4]    Serial interface format => IIS compatible format
    		//Bit[5]    Active level of left/right channel => Low for left channel
    		//Bit[7:6] Tx/Rx mode select => Tx mode
    		//Bit[8]    Master/Slave mode => Master mode
    		//Bit[9]    Master clock => MPLLin
		rIISFCON = (1<<15) + (1<<13);
		//Bit[13]  Tx FIFO enable
		//Bit[15]  Tx FIFO access mode => DMA   

		Uart_Printf("\nIISLRCK = %d Hz", (int) IIS_Codec_CLK/384);
      	}

	rINTMSK    = ~(BIT_EINT0 | BIT_DMA2);
	
	//DMA2 Register Setting 
	rDISRC2  = (int)(start_addr); 
       rDISRCC2 = (0<<1) + (0<<0); 		  
       rDIDST2  = ((U32)IISFIFO);			
	rDIDSTC2 = (0<<2) + (1<<1) + (1<<0);				
	rDCON2   = ((unsigned int)1<<31)+(0<<30)+(1<<29)+(0<<28)+(0<<27)+(0<<24)+(1<<23)+(0<<22)+(1<<20)+(play_size/2);
       rDMASKTRIG2 = (0<<2) + (1<<1) + (0<<0);	    //No-stop, DMA2 channel On, and No-sw trigger 
    
	Uart_Printf("\nIf you want to mute or no mute, Push the 'EIN0' button repeatedly.\n");
    	Uart_Printf("Press any key to exit!!!\n");
	Uart_Printf("\nPlay...\n");
  
	//IIS Tx Start
    	rIISCON |= 0x1;		 //IIS Interface start
		
    	while(!Uart_GetKey());
    	
    	//IIS Tx Stop
    	Delay(10);			 //For end of H/W Tx
    	rIISCON	&= ~(1<<0);	    //IIS Interface stop

    	rDMASKTRIG2  = (1<<2);	 //DMA2 stop
	rIISFCON     = 0x0;	    //For FIFO flush
	rINTMSK |= (BIT_EINT0 | BIT_DMA2);
	 	
	Uart_Printf("\nEnd of Play!\n");
}

void Select_IIS_Master_CLK(void)
{
	int sel;
	
	//Uart_Printf("\nIIS Master Clock Source => MPLLin\n");
       Uart_Printf("\nSelect IIS Master Clock Source\n");
	Uart_Printf("0:PCLK, 1:MPLLin\n");
	sel = Uart_GetIntNum();		
	
	switch(sel)
	{
		case 0 : 
		//IIS Pre-scaler Setting
	  	rIISPSR = (5<<5) + 5;  
		rIISMOD = (0<<9);
		//In case PCLK = 50 MHz, IIS Codec CLK = 50/(5+1) = 8.34MHz
		Uart_Printf("\nIIS Master CLK(PCLK) = %4.2f MHz", (float)Pclk/MEGA);
		IIS_Codec_CLK = (float)Pclk/6;
		Uart_Printf("\nIIS Codec CLK = %4.2f MHz", IIS_Codec_CLK/MEGA);

		IIS_MasterClk_Sel = 0;	
      		break;

		case 1 : 
		rIISMOD = (1<<9);
		Uart_Printf("\nIIS Master CLK(MPLLin) = %4.2f MHz", (float)FIN/MEGA);
		IIS_Codec_CLK = (float)FIN;
		Uart_Printf("\nIIS Codec CLK = %4.2f MHz", IIS_Codec_CLK/MEGA);

		IIS_MasterClk_Sel = 1;
		break;

		default : 
		rIISMOD = (1<<9);
		Uart_Printf("\nIIS Master CLK(MPLLin) = %4.2f MHz", (float)FIN/MEGA);
		IIS_Codec_CLK = (float)FIN;
		Uart_Printf("\nIIS Codec CLK = %4.2f MHz", IIS_Codec_CLK/MEGA);

		IIS_MasterClk_Sel = 1;
		break;		
	}
}


//Initialization of UDA1341 Audio Codec using L3 Interface 
void Init1341(char mode)
{
 	//Port Initialize
	//----------------------------------------------------------
	//   PORT B GROUP
	//Ports  :   GPB4    GPB3   GPB2  
	//Signal :  L3CLOCK L3DATA   L3MODE
	//Setting:  OUTPUT   OUTPUT  OUTPUT 
	//	          [9:8]       [7:6]      [5:4]
	//Binary :    01,          01,        01 
	//----------------------------------------------------------    
       rGPBDAT = rGPBDAT & ~(L3M|L3C|L3D) |(L3M|L3C); //Start condition : L3M=H, L3C=H
       rGPBUP  = rGPBUP  & ~(0x7<<2) |(0x7<<2);	 //The pull up function is disabled GPB[4:2] 1 1100    
    	rGPBCON = rGPBCON & ~(0x3f<<4) |(0x15<<4);     //GPB[4:2]=Output(L3CLOCK):Output(L3DATA):Output(L3MODE)

	//L3 Interface
    	_WrL3Addr(0x14 + 2);     //STATUS (000101xx+10)
    	//_WrL3Data(0x60,0);	 //0,1,10, 000,0 : Status 0,Reset, 256fs,IIS-bus,no DC-filtering
	_WrL3Data(0x50,0);	 //0,1,01, 000,0 : Status 0,Reset, 384fs,IIS-bus,no DC-filtering
	
    	//_WrL3Addr(0x14 + 2);     //STATUS (000101xx+10)
    	//_WrL3Data(0x20,0);	 //0,0,10, 000,0 : Status 0,No reset,256fs,IIS-bus,no DC-filtering
    
    	_WrL3Addr(0x14 + 2);     //STATUS (000101xx+10)
    	_WrL3Data(0x81,0);	 //bit[7:0] => 1,0,0,0, 0,0,01 
	//Status 1,Gain of DAC 0 dB,Gain of ADC 0dB,ADC non-inverting,DAC non-inverting
	//Single speed playback,ADC-Off DAC-On
    
	//Record
    	if(mode)
    	{
	  	_WrL3Addr(0x14 + 2);    //STATUS (000101xx+10)
		//_WrL3Data(0xa2,0);	//1,0,1,0, 0,0,10 
		//Status 1,Gain of DAC 0 dB,Gain of ADC 6dB,ADC non-inverting,DAC non-inverting
	       //Single speed playback,ADC-On DAC-Off	 
					  
	       _WrL3Data(0xe2,0);	//bit[7:0] => 1,1,1,0, 0,0,11 
		//Status 1,Gain of DAC 6 dB,Gain of ADC 6dB,ADC non-inverting,DAC non-inverting
		//Single speed playback,ADC-On DAC-On					     

		//_WrL3Addr(0x14 + 0);    //DATA0 (000101xx+00)
		//_WrL3Data(0x3f,0);	//00,11 1111  : Volume control (6 bits)  
		//_WrL3Data(0x00,0);	//00,00 0000  : Volume control (6 bits) 0dB    

       	//_WrL3Addr(0x14 + 0);    //DATA0 (000101xx+00)
       	//_WrL3Data(0x7b,0);	//01,11 10,11 : Data0, Bass Boost 18~24dB, Treble 6dB		 
       
              //_WrL3Addr(0x14 + 0);    //DATA0 (000101xx+00)	  
       	//_WrL3Data(0xc4,0);	//1100 0,100  : Extended addr(3bits), 100 
       	//_WrL3Data(0x91,0);	//100,1 00,01 : DATA0, Enable AGC, 00, input amplifier gain channel 2 (2bits)					    
       
              _WrL3Addr(0x14 + 0);    //DATA0 (000101xx+00)
       	_WrL3Data(0xc2,0);	//1100 0,010  : Extended addr(3bits), 010 
              _WrL3Data(0x4d,0);	//010,0 11,01 : DATA0, MIC Amplifier Gain 9dB, input channel 1 select(input channel 2 off) 
       	//_WrL3Data(0x59,0);	//010,1 10,01 : DATA0, MIC Amplifier Gain 27dB, input channel 1 select(input channel 2 off)
              //_WrL3Data(0x41,0);	//010,0 00,01 : DATA0, MIC Amplifier Gain -3dB, input channel 1 select(input channel 2 off)
     	}
}

void _WrL3Addr(U8 data)
{	 
 	S32 i,j;

    	rGPBDAT  = rGPBDAT & ~(L3D | L3M | L3C) | L3C;	//L3D=L, L3M=L(in address mode), L3C=H

    	for(j=0;j<4;j++);	 //tsu(L3) > 190ns

	//GPB[4:2]=L3C:L3D:L3M
    	for(i=0;i<8;i++)	//LSB first
    	{
	  	if(data & 0x1)	//If data's LSB is 'H'
	  	{
			rGPBDAT &= ~L3C;	 //L3C=L
			rGPBDAT |= L3D;		 //L3D=H		 
			for(j=0;j<4;j++);	        //tcy(L3) > 500ns
			rGPBDAT |= L3C;		 //L3C=H
			rGPBDAT |= L3D;		 //L3D=H
			for(j=0;j<4;j++);	        //tcy(L3) > 500ns
	  	}
	  	else		//If data's LSB is 'L'
	  	{
			rGPBDAT &= ~L3C;	//L3C=L
			rGPBDAT &= ~L3D;	//L3D=L
			for(j=0;j<4;j++);	       //tcy(L3) > 500ns
			rGPBDAT |= L3C;	       //L3C=H
			rGPBDAT &= ~L3D;	//L3D=L
			for(j=0;j<4;j++);	       //tcy(L3) > 500ns		
	  	}
	  	data >>= 1;
    	}

    	rGPBDAT  = rGPBDAT & ~(L3D | L3M | L3C) | (L3C | L3M);	 //L3M=H,L3C=H   
}

void _WrL3Data(U8 data,int halt)
{
 	S32 i,j;

    	if(halt)
    	{
	  	rGPBDAT  = rGPBDAT & ~(L3D | L3M | L3C) | L3C;   //L3C=H(while tstp, L3 interface halt condition)	  
	  	for(j=0;j<4;j++);		//tstp(L3) > 190ns
    	}

    	rGPBDAT  = rGPBDAT & ~(L3D | L3M | L3C) | (L3C | L3M);   //L3M=H(in data transfer mode)	  
    	for(j=0;j<4;j++);		//tsu(L3)D > 190ns

	//GPB[4:2]=L3C:L3D:L3M
    	for(i=0;i<8;i++)
    	{
	  	if(data & 0x1)	//if data's LSB is 'H'
	  	{
	     		rGPBDAT &= ~L3C;		//L3C=L
	     		rGPBDAT |= L3D;			//L3D=H

	     		for(j=0;j<4;j++);			//tcy(L3) > 500ns
	     		rGPBDAT |= (L3C | L3D);	//L3C=H,L3D=H
	     		for(j=0;j<4;j++);		 	//tcy(L3) > 500ns
	  	}
	  	else		//If data's LSB is 'L'
	  	{
	     		rGPBDAT &= ~L3C;	//L3C=L
	     		rGPBDAT &= ~L3D;	//L3D=L
	     		for(j=0;j<4;j++);		//tcy(L3) > 500ns
	     		rGPBDAT |= L3C;		//L3C=H
	     		rGPBDAT &= ~L3D;	//L3D=L
	     		for(j=0;j<4;j++);		//tcy(L3) > 500ns
	  	}
		data >>= 1;		//For check next bit
    	}

    	rGPBDAT  = rGPBDAT & ~(L3D | L3M | L3C) | (L3C | L3M);    //L3M=H,L3C=H
}

/* ISRs */
void __irq DMA1_Rec_Done(void)
{
    ClearPending(BIT_DMA1);     //Clear pending bit
    Rec_Done = 1;
} 

void __irq DMA2_Done(void)
{
    	ClearPending(BIT_DMA2); //Clear pending bit
    	Uart_Printf("\n~~~");
	//Uart_Printf("\nrIISMOD=0x%x\n",rIISMOD);
}

void __irq RxInt(void)
{
    rSUBSRCPND = BIT_SUB_RXD1;	    //Clear pending bit (Requested)
    rSUBSRCPND;
    ClearPending(BIT_UART1);

    *_temp ++= RdURXH1(); 
}

void __irq Muting(void)
{
    ClearPending(BIT_EINT0);		//Clear pending bit

    if(mute)    //Mute
    {
	  _WrL3Addr(0x14 + 0);		//DATA0 (000101xx+00)
	  _WrL3Data(0xa4,0);		  //10,1,00,1,00 : after, no de-emp, mute, flat 
	  mute = 0;
	  Uart_Printf("\nMute on...\n");
    }
    else	  //No mute
    {
	  _WrL3Addr(0x14 + 0);		//DATA0 (000101xx+00)
	  _WrL3Data(0xa0,0);		  //10,1,00,0,00 : after, no de-emp, no mute, flat 
	  mute = 1;
	  Uart_Printf("\nMute off...\n");
    }
}

