/*************************************************
  NAME: iis_dual.c
  DESC: S3C2440 IIS (UDA1341) simultaneous record & play test 
  HISTORY:
  OCT.17.2002:purnnamu: first release
  NOV.05.2002:purnnamu: Rec start/stop randomly.
  APR.01.2003:ky.shim: minor modification.
 *************************************************/

#include <math.h>
#include <stdlib.h>
 
#include "2440addr.h"
#include "2440lib.h"
#include "def.h"
#include "iis.h"

//declarations of some functions in 2440iis.c
void IIS_PortSetting(void);
void _WrL3Addr(U8 data);
void _WrL3Data(U8 data,int halt);


void Init1341RecPlay(void);

void AudioInit(void);
void AudioBufCopy(void);
void AudioInStart(void);
void AudioInStop(void);
void AudioOutStart(void);
void AudioOutStop(void);
void AudioOutRandom(void);

void __irq IsrAudioIn(void);
void __irq IsrAudioOut(void);
void __irq IsrAudioMute(void);

void InterruptLatencyStart(void);
void InterruptLatencyStop(void);
void __irq IsrTimer0_InterruptLatency(void);


#define MAX_INDEX_AUDIO_BUF	(200)    
#define AUDIO_BUF_SIZE		(1000)	 //500 stereo samples

U16 (*playBuf)[AUDIO_BUF_SIZE];
U16 (*recBuf) [AUDIO_BUF_SIZE];
volatile int indexPlayBuf,indexRecBuf;
volatile int stopPlay,stopRec;
volatile int audioOutActive,audioInActive;


//------------------------------------------------------------------------------
//      SMDK2440 IIS Configuration
// GPB4 = L3CLOCK, GPB3 = L3DATA, GPB2 = L3MODE
// GPE4 = I2SSDO,  GPE3 = I2SSDI, GPE2 = CDCLK, GPE1 = I2SSCLK, GPE0 = I2SLRCK  
//------------------------------------------------------------------------------



//**************************************************
//                     IIS test
//**************************************************

void Test_IisRecPlay(void)
{
	
//    int i;
    int ranPlay=FALSE;
    char key;
    
#if ADS10==TRUE	
	srand(0);
#endif

    Uart_Printf("[ IIS (Master Mode,Rec&Play) Test(UDA1341) ]\n");
    Uart_TxEmpty(1);
	
	ChangeClockDivider(14,12);		// 1:4:8    
	ChangeMPllValue(233,6,0);		// 361.2672MHz
	Uart_Init(45158400,115200);	   
        
    IIS_PortSetting();
    Init1341RecPlay();
    
    playBuf=(U16 (*)[AUDIO_BUF_SIZE])0x31000000;
    recBuf=(U16 (*)[AUDIO_BUF_SIZE])(0x31000000+sizeof(U16)*AUDIO_BUF_SIZE*MAX_INDEX_AUDIO_BUF);
    
    indexPlayBuf=indexRecBuf=0;
    audioOutActive=audioInActive=FALSE;
    stopPlay=stopRec=FALSE;
    
    //for(i=0x31000000;i<0x31100000;i+=4)*((U32 *)i)=0;
    
    AudioInit();
    pISR_DMA1  = (U32)IsrAudioIn;
    pISR_DMA2  = (U32)IsrAudioOut;
    pISR_EINT0 = (U32)IsrAudioMute;
    
    
    rSRCPND    = (BIT_EINT0 | BIT_DMA1 | BIT_DMA2);
    rINTPND    = rINTPND;
    rINTMSK    = ~(BIT_EINT0 | BIT_DMA1 | BIT_DMA2);   
    
    Uart_Printf("If you want to mute or no mute push the 'EIN0' key repeatedly\n");
    Uart_Printf("Sampling Frequency = %d Hz\n",44100);
    

    while(1)
    {
    	Uart_Printf("\n[Select Menu] (Rec Play Begin End Toggle Random)\n"
    	            "1:R_B 2:R_E 3:PR_T 4:P_B 5:P_E 6:Copy:R->P 7:IL_B 8:IL_E 9:Exit\n");
    	            
    	while((key=Uart_GetKey())==0)
    	{
    	    if(ranPlay==TRUE)
    	    	AudioOutRandom();
    	}

    	switch(key)
    	{
    	case '1':
    	    AudioInStart();
    	    break;
    	case '2':
    	    AudioInStop();
    	    break;
    	case '3':
			
/*
    	    if(ranPlay==FALSE)
    	    {
    	    	ranPlay=TRUE;
    	    	Uart_Printf("Random Play Stop/Start begins.\n");
    	    }
    	    else
    	    {
    	    	ranPlay=FALSE;
    	    	Uart_Printf("Random Play Stop/Start ends.\n");
    	    }
*/
	    break;
    	case '4':
    	    AudioOutStart();
    	    break;
    	case '5':
    	    AudioOutStop();
    	    break;
    	case '6':
    	    AudioBufCopy();
            break;
        case '7':
            InterruptLatencyStart();
	    break;
        case '8':
            InterruptLatencyStop();
   	    break;
    	case '9':
    	    return;
    	default:
    	    break;    
    	}
    }
	
	//ChangeClockDivider(14,12);		// 1:2:4    
   //ChangeMPllValue(92,1,0);		// 400MHz
	//Uart_Init(FCLK/8,115200); 
}


void AudioBufCopy(void)
{
    int i,j;
    Uart_Printf("[Audio Buffer Copy recBuf -> playBuf]\n");    
    
    if(audioInActive==TRUE)
    {
    	Uart_Printf("Rec is ongoing. Please Stop Recording.\n");
    	return;
    }
    
    for(i=0;i<MAX_INDEX_AUDIO_BUF;i++)
        for(j=0;j<AUDIO_BUF_SIZE;j++)
            playBuf[i][j]=recBuf[i][j];
}


void InterruptLatencyStart(void)
{
    Uart_Printf("[Random Interrupt Latency Start]\n");	
    pISR_TIMER0 = (U32)IsrTimer0_InterruptLatency;  //to make a worst condition
    rTCFG0=(3<<0);  //Timer0&1Prescaler=(3+1)
    rTCFG1=(3<<0);  //Timer0=PCLK/4/16
    rTCNTB0=1;
    rTCON=(1<<1); //T0ManualUpdate
    rTCON=(1<<0); //T0Start
    
    rINTMSK&=~(BIT_TIMER0);
}

void InterruptLatencyStop(void)
{
    rINTMSK|=BIT_TIMER0;
     
    Uart_Printf("[Random Interrupt Latency Stop]\n");	
}


void AudioInit(void)
{
    //DMA1 for AudioIn
    rDMASKTRIG1 = (1<<2);  	//Dma1Stop

    //DMA2 for AudioOut
    rDMASKTRIG2 = (1<<2);	//Dma2Stop 

    //IIS
    rIISCON=(1<<5)|(1<<4)|(1<<1);
        //TxDMAReqEn,RxDMAReqEn,IISPrescalerEn    
    rIISMOD=(0<<8)|(3<<6)|(0<<4)|(1<<3)|(0<<2)|(1<<0);
        //Master,Tx&Rx,IisFormat,16bit,256fs,32fs
    rIISPSR=(3<<5)|(3<<0);   
    	//PreScalerA=45Mhz/4,PreScalerB=45Mhz/4
    rIISFCON=(1<<15)|(1<<14)|(1<<13)|(0<<12);
    	//TxFifo=DMA,RxFifo=DMA,TxFifoEn,RxFifoDis
    rIISCON|=(1<<0);
        //IIS Enable;
        //IIS will not start because DMA is not ready.
}




//**************************************************
//                     AudioOut
//**************************************************


void AudioOutStart(void)
{
    if(audioOutActive!=FALSE)
    {
    	Uart_Printf("[N/A]");
    	return;
    }
    	
    Uart_Printf("[AudioOut Start]\n");    	
    stopPlay=FALSE;
    audioOutActive=TRUE;	
    indexPlayBuf=0;
    
    //Init DMA Ch 2
    rDISRC2 = (U32)(playBuf[indexPlayBuf++]);
    
    rDISRCC2 = (0<<1)|(0<<0); //Src=AHB,Increment
    rDIDST2 = (U32)IISFIFO;  // Tx FIFO address
    rDIDSTC2 = (1<<1)|(1<<0); //Dst=APB,Fixed;
    rDCON2 = ((unsigned int)1<<31)|(0<<30)|(1<<29)|(0<<28)|(0<<27)|(0<<24)|(1<<23)|(1<<22)|(1<<20)|(AUDIO_BUF_SIZE);
        //handshake,Sync=APB,IntEn,unit,single,dst=I2SSDO,HwReqMode,NoAutoReload,Halfword,
    rDMASKTRIG2 = (1<<1); 	//DMA2En
 

    //IIS will start to work.
}


#define DMA_OFF_TIMEOUT		(1000)

void AudioOutStop(void)
{
    Uart_Printf("\n[AudioOut Stop]\n");    	
    stopPlay=TRUE;
}


void AudioOutRandom(void)
{
    static int  state=0;//, cnt=0;
	
#if ADS10==TRUE
    if( (rand()/(RAND_MAX/2000000)) < 1 )
#else
    if(cnt++%2)
#endif
    {
   	switch(state++)
   	{
   	case 0:
   	    AudioOutStart();
   	    state=1;
   	    break;
   	case 1:
   	    state=2;
   	    break;
   	case 2:
   	    AudioOutStop();
   	    state=0;
   	    break;
   	default:
	    break;
	}
    } 
}


void __irq IsrAudioOut(void)  //DMA2 done interrupt
{
    int timeOut=0;
    
    //NMI doesn't using auto-reload.	
	
    ClearPending(BIT_DMA2);

    if(stopPlay==TRUE)
    {	    
    #if 1
    	rDMASKTRIG2|= (1<<2);  //NMI 
    #else 
    	rDMASKTRIG2= (1<<2);   //StopDMA2  //recommended
    #endif	

    	while (rDMASKTRIG2&(1<<1) && (timeOut++ < DMA_OFF_TIMEOUT));
    	if(timeOut >= DMA_OFF_TIMEOUT)
      	    Uart_Printf("ERROR: DMA2 Stop timed out\n");
	rDMASKTRIG2 &= ~(1<<2); //not needed.... remove!!!
	audioOutActive=FALSE;
	Uart_Printf("<PE>");
    }  
    else
    {
	rDISRC2 = (U32)(playBuf[indexPlayBuf++]);
    	if(indexPlayBuf==MAX_INDEX_AUDIO_BUF)
    	{
    	    indexPlayBuf=0;
     	}
    	rDMASKTRIG2 |= (1<<1);   //enable DMA2  
 
     	if((indexPlayBuf%10)==0)
    	{
    	    if(indexPlayBuf==0)WrUTXH0(';');
    	    else WrUTXH0(',');
    	}
     }
}


//**************************************************
//                     AudioIn
//**************************************************

void AudioInStart(void)
{
    if(audioInActive!=FALSE)
    {
    	Uart_Printf("[N/A]");
    	return;
    }
        	
    Uart_Printf("[AudioIn Start]\n");    
    stopRec=FALSE;
    audioInActive=TRUE;	
    indexRecBuf=0;    
    
    //Init DMA Ch 1
    rDISRC1 = (U32)IISFIFO;  // Rx FIFO address
    rDISRCC1 = (1<<1)|(1<<0); //Src=APB,Fixed;
    rDIDST1 = (U32)(recBuf[indexRecBuf++]);
    rDIDSTC1 = (0<<1)|(0<<0); //DSt=AHB,Increment
    rDCON1 = ((unsigned int)1<<31)|(0<<30)|(1<<29)|(0<<28)|(0<<27)|(2<<24)|(1<<23)|(0<<22)|(1<<20)|(AUDIO_BUF_SIZE);
        //handshake,Sync=APB,IntEn,unit,single,dst=I2SSDI,HwReqMode,AutoReload,Halfword,
    rDMASKTRIG1 = (1<<1); //DMA1En
  
    rIISFCON|=(1<<12);  //RxFifoEn
    rIISCON|=(1<<4);    //RxDmaServiceRequestEn
    
    while((rDSTAT1&0xFFFFF) == 0);   
    
    rDIDST1 = (U32)(recBuf[indexRecBuf++]);
    
    //IIS will start to work.
}




void AudioInStop(void)
{
    Uart_Printf("\n[AudioIn Stop]\n");    	
    stopRec=TRUE;
}




void __irq IsrAudioIn(void) //DMA1 done interrupt
{
    int timeOut=0;
    
    ClearPending(BIT_DMA1);                 //Clear pending bit

    while((rDSTAT1 & 0xFFFFF) == 0);  //wait until the current autoreload is completed

    if(stopRec==TRUE)
    {
    	rIISCON&=~(1<<4);    //RxDmaServiceRequestDis
        rIISFCON&=~(1<<12);   //RxFifoDis	
    #if 1
    	rDMASKTRIG1|= (1<<2);  //NMI
    #else 
    	rDMASKTRIG1= (1<<2);   //recommended
    #endif	
    	while (rDMASKTRIG1&(1<<1) && (timeOut++ < DMA_OFF_TIMEOUT));
    
    	if(timeOut >= DMA_OFF_TIMEOUT)
      	    Uart_Printf("ERROR: DMA1 Stop timed out\n");

    	rDMASKTRIG1 &= ~(1<<2); //not needed.... remove!!!

    	audioInActive=FALSE;
    	Uart_Printf("[RE]");
    }
    else
    {
    	rDIDST1 = (U32)(recBuf[indexRecBuf++]);
    	if(indexRecBuf==MAX_INDEX_AUDIO_BUF)
    	{
    	    indexRecBuf=0;
    	}
    	if((indexRecBuf%10)==0)
    	{
    	    if(indexRecBuf==0)WrUTXH0(':');
    	    else WrUTXH0('.');
    	}
    }
    
}


void __irq IsrAudioMute(void)
{
    static int mute=0;
    
    ClearPending(BIT_EINT0);

    if(mute)    //Mute
    {
        _WrL3Addr(0x14 + 0);            //DATA0 (000101xx+00)
        _WrL3Data(0xa4,0);              //10,1,00,1,00 : after, no de-emp, mute, flat 
        mute = 0;
        Uart_Printf("\n[Mute]\n");
    }
    else        //No mute
    {
        _WrL3Addr(0x14 + 0);            //DATA0 (000101xx+00)
        _WrL3Data(0xa0,0);              //10,1,00,0,00 : after, no de-emp, no mute, flat 
        mute = 1;
        Uart_Printf("\n[No Mute]\n");
    }
}

//The following ISR makes an interrupt latency random time for audioIn and audioOut
void __irq IsrTimer0_InterruptLatency(void)
{
    static U16 count=0;
    ClearPending(BIT_TIMER0);
    count++;
    if(count==0x0)count=1;
    Delay(count&0x7f);
    Led_Display(count);

    rTCNTB0=count;
    rTCON=(1<<1); //T0ManualUpdate
    rTCON=(1<<0);  //T0Start  
    
}



//**************************************************
//                     UDA1341
//**************************************************

#define L3C (1<<4)            //GPB4 = L3CLOCK
#define L3D (1<<3)            //GPB3 = L3DATA
#define L3M (1<<2)            //GPB2 = L3MODE

void Init1341RecPlay(void)
{
    //Port Initialize
    //----------------------------------------------------------
    //   PORT B GROUP
    //Ports  :   GPB4    GPB3   GPB2  
    //Signal :  L3CLOCK L3DATA L3MODE
    //Setting:   OUTPUT OUTPUT OUTPUT 
    //           [9:8]   [7:6}  [5:4]
    //Binary :     01  ,   01    01 
    //----------------------------------------------------------    
    rGPBDAT = rGPBDAT & ~(L3M|L3C|L3D) |(L3M|L3C); //Start condition : L3M=H, L3C=H
    rGPBUP  = rGPBUP  & ~(0x7<<2) |(0x7<<2);       //The pull up function is disabled GPB[4:2] 1 1100    
    rGPBCON = rGPBCON & ~(0x3f<<4) |(0x15<<4);     //GPB[4:2]=Output(L3CLOCK):Output(L3DATA):Output(L3MODE)

    //L3 Interface
    _WrL3Addr(0x14 + 2);     //STATUS (000101xx+10)
    _WrL3Data(0x60,0);       //0,1,10,000,0 : Reset,256fs,no DCfilter,iis

    _WrL3Addr(0x14 + 2);     //STATUS (000101xx+10)
    _WrL3Data(0x20,0);       //0,0,10,000,0 : No reset,256fs,no DCfilter,iis
    
/*    
    //play
    _WrL3Addr(0x14 + 2);     //STATUS (000101xx+10)
    _WrL3Data(0x81,0);       //1,0,0,0,0,0,01 : OGS=0,IGS=0,ADC_NI,DAC_NI,sngl speed,AoffDon
        
    //record
    if(mode)
    {
        _WrL3Addr(0x14 + 2);     //STATUS (000101xx+10)
        _WrL3Data(0xa2,0);       //1,0,1,0,0,0,10 : OGS=0,IGS=1,ADC_NI,DAC_NI,sngl speed,AonDoff

        _WrL3Addr(0x14 + 0);     //DATA0 (000101xx+00)
        _WrL3Data(0xc2,0);       //11000,010  : DATA0, Extended addr(010) 
        _WrL3Data(0x4d,0);       //010,011,01 : DATA0, MS=9dB, Ch1=on Ch2=off, 
    }
*/
    _WrL3Addr(0x14 + 2);     //STATUS (000101xx+10)
    _WrL3Data(0xa3,0);       //1,0,1,0,0,0,11 : OGS=0,IGS=1,ADC_NI,DAC_NI,sngl speed,AonDon

    _WrL3Addr(0x14 + 0);     //DATA0 (000101xx+00)
    _WrL3Data(0xc2,0);       //11000,010  : DATA0, Extended addr(010) 
    _WrL3Data(0x4d,0);       //010,011,01 : DATA0, MS=9dB, Ch1=on Ch2=off, 
}

void IIS_PortSetting(void)
{
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

