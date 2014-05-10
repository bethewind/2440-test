/************************************************************
File Name	: camif.c
Descriptions
 -S3C2440 camera test routines & basic libraries
History
 - July 23, 2003. Draft Version 0.0 by purnnamu
 - Janualy 15, 2004. Modifed by Boaz

Copyright (c) 2004 SAMSUNG Electronics.
# However, Anybody can use this code without our permission.  
*************************************************************/

#include <ctype.h>
#include "def.h"
#include "2440addr.h"
#include "2440lib.h"
#include "camif.h"
#include "lcdlib.h"
#include "glib.h"
#include "camproset.h" // for camera setting
//#include ".\bmp\pqvga16bsm5.h"
#include ".\bmp\422jpeg.h"
#include ".\bmp\420jpeg.h"
#include ".\bmp\foreman_cif_420.H"
extern void Lcd_Start(U32 mode);
extern void Lcd_Port_Return(void);

volatile U32 camTestMode;
volatile U32 camCodecCaptureCount;
volatile U32 camPviewCaptureCount;
volatile U32 camCodecStatus;
volatile U32 camPviewStatus;
volatile U32 amount;

U32 save_GPJCON, save_GPJDAT, save_GPJUP;

U8 flagCaptured_P = 0;
U8 flagCaptured_C = 0;


void * func_camera_test[][2]=
{	
	(void *)Test_CamPreview,		"Preview Test ",
	(void *)Test_CamCodec,			"Codec Test ",
	(void *)Camera_Iic_Test,		"IIC interface test ",
	(void *)Test_YCbCr_to_RGB,	"YCbCr2RGB test",
	0,0
};

void Camera_Test(void)
{
	int i;
	
	Uart_Printf("\n======  Camera Interface Test Start ======\n");

	CamReset();

	// Initializing camif
	rCLKCON |= (1<<19); // enable camclk
	CamPortSet();	
	ChangeUPllValue(60, 4, 1);		// UPLL clock = 96MHz, PLL input 16.9344MHz
	rCLKDIVN|=(1<<3); // UCLK 48MHz setting for UPLL 96MHz
	// 0:48MHz, 1:24MHz, 2:16MHz, 3:12MHz...
	// Camera clock = UPLL/[(CAMCLK_DIV+1)X2]
	Uart_Printf("1...\n");
	switch(USED_CAM_TYPE)
	{
	case CAM_AU70H :
		if (AU70H_VIDEO_SIZE==1152)
			SetCAMClockDivider(CAMCLK24000000); //Set Camera Clock for SXGA
		if (AU70H_VIDEO_SIZE==640)
			SetCAMClockDivider(CAMCLK16000000); //Set Camera Clock for VGA
		break;
	case CAM_S5X3A1 :
		SetCAMClockDivider(CAMCLK24000000); //Set Camera Clock for SXGA
		break;
	default : 	// 24MHz
		SetCAMClockDivider(CAMCLK24000000); //Set Camera Clock 24MHz s5x532, ov7620
		break;
	}

	Uart_Printf("2...\n");
	// Initializing camera module
	CamModuleReset(); // s5x532 must do this..
	Delay(500); // ready time of s5x433, s5x532 IIC interface. needed...
	CameraModuleSetting();

	Uart_Printf("Initializing end...\n");
	
	while(1)
	{
		i=0;
		Uart_Printf("\n\n");
		Uart_Printf("1: Preview Test\n");	
		Uart_Printf("2: Codec Test \n");
		Uart_Printf("3: IIC interface test\n");
		Uart_Printf("4: YCbCr2RGB test\n\n");

		Uart_Printf("\nPress only Enter key to exit : ");
		i = Uart_GetIntNum();
		if(i==-1) break;		// return.
//		if(i>=0 && (i<((sizeof(func_camera_test)-1)/8)) )	// select and execute...
//			( (void (*)(void)) (func_camera_test[i][0]) )();
        switch(i)
		{
		    case 1:	 Test_CamPreview();
			         break;
		    case 2:	 Test_CamCodec();
			         break;
		    case 3:	 Camera_Iic_Test();
			         break;
		    case 4:	 Test_YCbCr_to_RGB();
			         break;
		    default:
			         break;

		}
	}
	
	Uart_Printf("\n====== Camera IF Test program end ======\n");

//	CamModuleReset(); // s5x532 must do this..
	rCLKCON &= ~(1<<19); // disable camclk
	
}


void CamPortSet(void)
{
	save_GPJCON = rGPJCON;
	save_GPJDAT = rGPJDAT;
	save_GPJUP = rGPJUP;

	rGPJCON = 0x2aaaaaa;
	rGPJDAT = 0;
	rGPJUP = 0;
}

void CamPortReturn(void)
{
	rGPJCON = save_GPJCON;
	rGPJDAT = save_GPJDAT;
	rGPJUP = save_GPJUP;
}

void CamPreviewIntUnmask(void)
{
    rINTSUBMSK &= ~(BIT_SUB_CAM_P);//INT CAMERA Port A ENABLE 
    rINTMSK &= ~(BIT_CAM);
}

void CamCodecIntUnmask(void)
{
    rINTSUBMSK &= ~(BIT_SUB_CAM_C);//INT CAMERA Port B ENABLE 
    rINTMSK &= ~(BIT_CAM);
}

void CamPreviewIntMask(void)
{
    rINTSUBMSK |= BIT_SUB_CAM_P;//INT CAMERA Port A ENABLE 
    rINTMSK |= (BIT_CAM);
}

void CamCodecIntMask(void)
{
    rINTSUBMSK |= BIT_SUB_CAM_C;//INT CAMERA Port B ENABLE 
    rINTMSK |= (BIT_CAM);
}


/******************************************************
 *                                                                      							*    
 *                       camera interface initialization                    		*
 *                                                                            					*     
 *******************************************************/

void CamReset(void)
{
	rCIGCTRL =rCIGCTRL | ((unsigned int)1<<31); //camera I/F soft reset
	Delay(10);
	rCIGCTRL = rCIGCTRL &~((unsigned int)1<<31);
}

void CamModuleReset(void)
{
	switch(USED_CAM_TYPE)
	{
	case CAM_OV7620 : // reset - active high
	case CAM_S5X532 : // reset - active low, but H/W inverted.. so, in this case active high
	case CAM_S5X433 : // reset - active low, but H/W inverted.. so, in this case active high
	case CAM_S5X3A1 : // reset - active low, but H/W inverted.. so, in this case active high
		rCIGCTRL |= (1<<30);	  //external camera reset high
		Delay(30);
		rCIGCTRL &= ~(1<<30);	//external camera reset low
		break;
	case CAM_AU70H : // reset - active low
	default :
		rCIGCTRL &= ~(1<<30);	//external camera reset low
		Delay(10);
		rCIGCTRL |= (1<<30); //external camera reset high
		break;
	}
}

// 0:48MHz, 1:24MHz, 2:16MHz, 3:12MHz...
// Camera clock = UPLL/[(CAMCLK_DIV+1)X2]
void SetCAMClockDivider(int divn) 
{
	rCAMDIVN = (rCAMDIVN & ~(0xf))|(1<<4)|(divn); // CAMCLK is divided..
}

/* Description of Parameters
CoDstWidth: Destination Width of Codec Path
CoDstHeight: Destination Height of Codec Path
PrDstWidth: Destination Width of Preview Path
PrDstHeight: Destination Height of Preview Path
WinHorOffset: Size of Window Offset for Horizontal Direction
WinVerOffset: Size of Window Offset for Vertical Direction
CoFrameBuffer: Start Address for Codec DMA
PrFrameBuffer: Start Address for Previe DMA
*/
void CamInit(U32 CoDstWidth, U32 CoDstHeight, U32 PrDstWidth, U32 PrDstHeight, 
			U32 WinHorOffset, U32 WinVerOffset,  U32 CoFrameBuffer, U32 PrFrameBuffer)
{
	U32 WinOfsEn;
	U32 divisor, multiplier;
	U32 MainBurstSizeY, RemainedBurstSizeY, MainBurstSizeC, RemainedBurstSizeC, MainBurstSizeRGB, RemainedBurstSizeRGB;
	U32 H_Shift, V_Shift, PreHorRatio, PreVerRatio, MainHorRatio, MainVerRatio;
	U32 SrcWidth, SrcHeight;
	U32 ScaleUp_H_Co, ScaleUp_V_Co, ScaleUp_H_Pr, ScaleUp_V_Pr;

	//constant for calculating codec dma address
	if(CAM_CODEC_OUTPUT)
		divisor=2; //CCIR-422
	else
		divisor=4; //CCIR-420
		
	//constant for calculating preview dma address
	if(CAM_PVIEW_OUTPUT)
		multiplier=4;
	else
		multiplier=2;
	
	if(WinHorOffset==0 && WinVerOffset==0)
		WinOfsEn=0;
	else
		WinOfsEn=1;

	SrcWidth=CAM_SRC_HSIZE-WinHorOffset*2;
	SrcHeight=CAM_SRC_VSIZE-WinVerOffset*2;

	if(SrcWidth>=CoDstWidth) ScaleUp_H_Co=0; //down
	else ScaleUp_H_Co=1;		//up

	if(SrcHeight>=CoDstHeight) ScaleUp_V_Co=0;
	else ScaleUp_V_Co=1;		

	if(SrcWidth>=PrDstWidth) ScaleUp_H_Pr=0; //down
	else ScaleUp_H_Pr=1;		//up

	if(SrcHeight>=PrDstHeight) ScaleUp_V_Pr=0;   // edited 040225
	else ScaleUp_V_Pr=1;		

	////////////////// common control setting
	rCIGCTRL |= (1<<26)|(0<<27); // inverse PCLK, test pattern
	rCIWDOFST = (1<<30)|(0xf<<12); // clear overflow 
	rCIWDOFST = 0;	
	rCIWDOFST=((unsigned int)WinOfsEn<<31)|(WinHorOffset<<16)|(WinVerOffset);
	rCISRCFMT=((unsigned int)CAM_ITU601<<31)|(0<<30)|(0<<29)|(CAM_SRC_HSIZE<<16)|(CAM_ORDER_YCBYCR<<14)|(CAM_SRC_VSIZE);

	////////////////// codec port setting
	if (CAM_CODEC_4PP)
	{
		rCICOYSA1=CoFrameBuffer;
		rCICOYSA2=rCICOYSA1+CoDstWidth*CoDstHeight+2*CoDstWidth*CoDstHeight/divisor;
		rCICOYSA3=rCICOYSA2+CoDstWidth*CoDstHeight+2*CoDstWidth*CoDstHeight/divisor;
		rCICOYSA4=rCICOYSA3+CoDstWidth*CoDstHeight+2*CoDstWidth*CoDstHeight/divisor;
		
		rCICOCBSA1=rCICOYSA1+CoDstWidth*CoDstHeight;
		rCICOCBSA2=rCICOYSA2+CoDstWidth*CoDstHeight;
		rCICOCBSA3=rCICOYSA3+CoDstWidth*CoDstHeight;
		rCICOCBSA4=rCICOYSA4+CoDstWidth*CoDstHeight;

		rCICOCRSA1=rCICOCBSA1+CoDstWidth*CoDstHeight/divisor;
		rCICOCRSA2=rCICOCBSA2+CoDstWidth*CoDstHeight/divisor;
		rCICOCRSA3=rCICOCBSA3+CoDstWidth*CoDstHeight/divisor;
		rCICOCRSA4=rCICOCBSA4+CoDstWidth*CoDstHeight/divisor;
	}
	else
	{
		rCICOYSA1=CoFrameBuffer;
		rCICOYSA2=rCICOYSA1;
		rCICOYSA3=rCICOYSA1;
		rCICOYSA4=rCICOYSA1;
		
		rCICOCBSA1=rCICOYSA1+CoDstWidth*CoDstHeight;
		rCICOCBSA2=rCICOCBSA1;
		rCICOCBSA3=rCICOCBSA1;
		rCICOCBSA4=rCICOCBSA1;
		
		rCICOCRSA1=rCICOCBSA1+CoDstWidth*CoDstHeight/divisor;
		rCICOCRSA2=rCICOCRSA1;
		rCICOCRSA3=rCICOCRSA1;
		rCICOCRSA4=rCICOCRSA1;
	}
	rCICOTRGFMT=((unsigned int)CAM_CODEC_IN_422<<31)|(CAM_CODEC_OUTPUT<<30)|(CoDstWidth<<16)|(CAM_FLIP_NORMAL<<14)|(CoDstHeight);

	CalculateBurstSize(CoDstWidth, &MainBurstSizeY, &RemainedBurstSizeY);
	CalculateBurstSize(CoDstWidth/2, &MainBurstSizeC, &RemainedBurstSizeC);
	rCICOCTRL=(MainBurstSizeY<<19)|(RemainedBurstSizeY<<14)|(MainBurstSizeC<<9)|(RemainedBurstSizeC<<4);

	CalculatePrescalerRatioShift(SrcWidth, CoDstWidth, &PreHorRatio, &H_Shift);
	CalculatePrescalerRatioShift(SrcHeight, CoDstHeight, &PreVerRatio, &V_Shift);
	MainHorRatio=(SrcWidth<<8)/(CoDstWidth<<H_Shift);
	MainVerRatio=(SrcHeight<<8)/(CoDstHeight<<V_Shift);
    			
	rCICOSCPRERATIO=((10-H_Shift-V_Shift)<<28)|(PreHorRatio<<16)|(PreVerRatio);
	rCICOSCPREDST=((SrcWidth/PreHorRatio)<<16)|(SrcHeight/PreVerRatio); 
	rCICOSCCTRL=(CAM_SCALER_BYPASS_OFF<<31)|(ScaleUp_H_Co<<30)|(ScaleUp_V_Co<<29)|(MainHorRatio<<16)|(MainVerRatio);

	rCICOTAREA=CoDstWidth*CoDstHeight;

	///////////////// preview port setting
	if (CAM_PVIEW_4PP) // codec view mode
	{
		rCIPRCLRSA1=PrFrameBuffer;
		rCIPRCLRSA2=rCIPRCLRSA1+PrDstWidth*PrDstHeight*multiplier;
		rCIPRCLRSA3=rCIPRCLRSA2+PrDstWidth*PrDstHeight*multiplier;
		rCIPRCLRSA4=rCIPRCLRSA3+PrDstWidth*PrDstHeight*multiplier;
	}	
	else // direct preview mode
	{
		rCIPRCLRSA1=LCDFRAMEBUFFER;
		rCIPRCLRSA2=LCDFRAMEBUFFER;
		rCIPRCLRSA3=LCDFRAMEBUFFER;
		rCIPRCLRSA4=LCDFRAMEBUFFER;
	}	

	rCIPRTRGFMT=(PrDstWidth<<16)|(CAM_FLIP_NORMAL<<14)|(PrDstHeight);

	if (CAM_PVIEW_OUTPUT==CAM_RGB24B)
		CalculateBurstSize(PrDstWidth*2, &MainBurstSizeRGB, &RemainedBurstSizeRGB);
	else // RGB16B
		CalculateBurstSize(PrDstWidth*2, &MainBurstSizeRGB, &RemainedBurstSizeRGB);
   	rCIPRCTRL=(MainBurstSizeRGB<<19)|(RemainedBurstSizeRGB<<14);

	CalculatePrescalerRatioShift(SrcWidth, PrDstWidth, &PreHorRatio, &H_Shift);
	CalculatePrescalerRatioShift(SrcHeight, PrDstHeight, &PreVerRatio, &V_Shift);
	MainHorRatio=(SrcWidth<<8)/(PrDstWidth<<H_Shift);
	MainVerRatio=(SrcHeight<<8)/(PrDstHeight<<V_Shift);
	rCIPRSCPRERATIO=((10-H_Shift-V_Shift)<<28)|(PreHorRatio<<16)|(PreVerRatio);		 
	rCIPRSCPREDST=((SrcWidth/PreHorRatio)<<16)|(SrcHeight/PreVerRatio);
	rCIPRSCCTRL=((unsigned int)1<<31)|(CAM_PVIEW_OUTPUT<<30)|(ScaleUp_H_Pr<<29)|(ScaleUp_V_Pr<<28)|(MainHorRatio<<16)|(MainVerRatio);
    
	rCIPRTAREA= PrDstWidth*PrDstHeight;
}



/********************************************************
 CalculateBurstSize - Calculate the busrt lengths
 
 Description:	
 - dstHSize: the number of the byte of H Size.
 
*/
void CalculateBurstSize(U32 hSize,U32 *mainBurstSize,U32 *remainedBurstSize)
{
	U32 tmp;	
	tmp=(hSize/4)%16;
	switch(tmp) {
		case 0:
			*mainBurstSize=16;
			*remainedBurstSize=16;
			break;
		case 4:
			*mainBurstSize=16;
			*remainedBurstSize=4;
			break;
		case 8:
			*mainBurstSize=16;
			*remainedBurstSize=8;
			break;
		default: 
			tmp=(hSize/4)%8;
			switch(tmp) {
				case 0:
					*mainBurstSize=8;
					*remainedBurstSize=8;
					break;
				case 4:
					*mainBurstSize=8;
					*remainedBurstSize=4;
				default:
					*mainBurstSize=4;
					tmp=(hSize/4)%4;
					*remainedBurstSize= (tmp) ? tmp: 4;
					break;
			}
			break;
	}		    	    		
}



/********************************************************
 CalculatePrescalerRatioShift - none
 
 Description:	
 - none
 
*/
void CalculatePrescalerRatioShift(U32 SrcSize, U32 DstSize, U32 *ratio,U32 *shift)
{
	if(SrcSize>=64*DstSize) {
		Uart_Printf("ERROR: out of the prescaler range: SrcSize/DstSize = %d(< 64)\n",SrcSize/DstSize);
		while(1);
	}
	else if(SrcSize>=32*DstSize) {
		*ratio=32;
		*shift=5;
	}
	else if(SrcSize>=16*DstSize) {
		*ratio=16;
		*shift=4;
	}
	else if(SrcSize>=8*DstSize) {
		*ratio=8;
		*shift=3;
	}
	else if(SrcSize>=4*DstSize) {
		*ratio=4;
		*shift=2;
	}
	else if(SrcSize>=2*DstSize) {
		*ratio=2;
		*shift=1;
	}
	else {
		*ratio=1;
		*shift=0;
	}    	
}


/********************************************************
 CamCaptureStart - Start camera capture operation.
 
 Description:	
 - mode= CAM_CODEC_CAPTURE_ENABLE_BIT or CAM_PVIEW_CAPTURE_ENABLE_BIT or both
  
*/
void CamCaptureStart(U32 mode)
{ 
    
	if(mode&CAM_CODEC_SCALER_CAPTURE_ENABLE_BIT) {
		camCodecStatus=CAM_STARTED;
		rCICOSCCTRL|=CAM_CODEC_SACLER_START_BIT;
	}
	
	if(mode&CAM_PVIEW_SCALER_CAPTURE_ENABLE_BIT) {
		camPviewStatus=CAM_STARTED;
		rCIPRSCCTRL|=CAM_PVIEW_SACLER_START_BIT;
	}

	if(mode&CAM_CAMIF_GLOBAL_CAPTURE_ENABLE_BIT) {
		camCodecStatus=CAM_STARTED;
		rCICOSCCTRL|=CAM_CAMIF_GLOBAL_CAPTURE_ENABLE_BIT;
	}
	
	rCIIMGCPT|=CAM_CAMIF_GLOBAL_CAPTURE_ENABLE_BIT|mode;
}


void CamCaptureStop(void)
{
	camCodecStatus=CAM_STOP_ISSUED;
	camPviewStatus=CAM_STOP_ISSUED;
}


void _CamCodecStopHw(void)
{
	rCICOSCCTRL &= ~CAM_CODEC_SACLER_START_BIT; //stop codec scaler.
	rCIIMGCPT &= ~CAM_CODEC_SCALER_CAPTURE_ENABLE_BIT; //stop capturing for codec scaler.
	if(!(rCIIMGCPT & CAM_PVIEW_SCALER_CAPTURE_ENABLE_BIT))
		rCIIMGCPT &= ~CAM_CAMIF_GLOBAL_CAPTURE_ENABLE_BIT; //stop capturing for preview scaler if needed.
	rCICOCTRL |= (1<<2); //Enable last IRQ at the end of frame capture.
		       //NOTE:LastIrqEn bit should be set after clearing CAPTURE_ENABLE_BIT & SCALER_START_BIT
}

void _CamPviewStopHw(void)
{
	rCIPRSCCTRL &= ~CAM_PVIEW_SACLER_START_BIT; //stop preview scaler.
	rCIIMGCPT &= ~CAM_PVIEW_SCALER_CAPTURE_ENABLE_BIT; //stop capturing for preview scaler.
	if(!(rCIIMGCPT&CAM_CODEC_SCALER_CAPTURE_ENABLE_BIT))
		rCIIMGCPT &= ~CAM_CAMIF_GLOBAL_CAPTURE_ENABLE_BIT; //stop capturing for codec scaler if needed.
	rCIPRCTRL |= (1<<2); //Enable last IRQ at the end of frame capture.
       	//NOTE:LastIrqEn bit should be set after clearing CAPTURE_ENABLE_BIT & SCALER_START_BIT
}


void __irq CamIsr(void)
{

	U32 completedFrameIndex;

	if (rSUBSRCPND&BIT_SUB_CAM_C)
	{
		Uart_Printf("[C]");
		CamCodecIntMask();
		rSUBSRCPND |= BIT_SUB_CAM_C;
		ClearPending(BIT_CAM);
		switch(camCodecStatus) {
			case CAM_STOP_ISSUED:
				_CamCodecStopHw();
				camCodecStatus=CAM_LAST_CAPTURING;
				Uart_Printf("cr=%x\n", rCICOCTRL);
				//Uart_Printf("cS1\n");
				break;
			case CAM_LAST_CAPTURING:
				camCodecStatus=CAM_STOPPED;
				CamCodecIntMask();			
				//Uart_Printf("cS2\n");
				return;
			case CAM_STARTED:
				flagCaptured_C = 1;
//				_CamCodecStopHw();
				if(camTestMode&CAM_TEST_MODE_CODEC)	{
					if(camCodecCaptureCount>0) 
						completedFrameIndex=(((rCICOSTATUS>>26)&0x3)+4-2)%4;   
					Uart_Printf("FrameIndex:%d\n",completedFrameIndex);
				}
				else {
					//Uart_Printf("Just Capturing without display");
				}
				break;
			case CAM_CODEC_SCALER_BYPASS_STATE:
				//Uart_Printf("cBP\n");
				break;
			default:
				break;
		}
			
		CamCodecIntUnmask();
	    camCodecCaptureCount++;	 
	}
	else
	{
		Uart_Printf("[P]"); 
		CamPreviewIntMask();
		rSUBSRCPND |= BIT_SUB_CAM_P;
		ClearPending(BIT_CAM) 	
		switch(camPviewStatus) {
			case CAM_STOP_ISSUED:
				_CamPviewStopHw();
				camPviewStatus=CAM_LAST_CAPTURING;	 
				Uart_Printf("pr=%x\n", rCIPRCTRL);
				//Uart_Printf("pS1\n");
				break;
			case CAM_LAST_CAPTURING:
				camPviewStatus=CAM_STOPPED;
				CamPreviewIntMask();
				//Uart_Printf("pS2\n"); 	
				return;
			case CAM_STARTED:
				flagCaptured_P = 1;
				if(camTestMode&CAM_TEST_MODE_PVIEW) {
					if(camPviewCaptureCount >0) 
						completedFrameIndex=(((rCIPRSTATUS>>26)&0x3)+4-2)%4;
					//Uart_Printf("FrameIndex:%d\n",completedFrameIndex);
				}
				else {
					//Uart_Printf("Preview Image Captured\n");
				} 		
			default:
				break;			
		} 	
		
		CamPreviewIntUnmask();
		camPviewCaptureCount++;
	}
}



/******************************************************************************
 *                                                                            *    
 *                   camera interface interrupts & controls                   *
 *                                                                            *     
 ******************************************************************************/


U32 Conv_YCbCr_Rgb(U8 y0, U8 y1, U8 cb0, U8 cr0)  // second solution... by junon
{
	// bit order is
	// YCbCr = [Cr0 Y1 Cb0 Y0], RGB=[R1,G1,B1,R0,G0,B0].
	
	int r0, g0, b0, r1, g1, b1;
	U32 rgb0, rgb1, rgb;
 
	#if 1 // 4 frames/s @192MHz, 12MHz ; 6 frames/s @450MHz, 12MHz
	r0 = YCbCrtoR(y0, cb0, cr0);
	g0 = YCbCrtoG(y0, cb0, cr0);
	b0 = YCbCrtoB(y0, cb0, cr0);
	r1 = YCbCrtoR(y1, cb0, cr0);
	g1 = YCbCrtoG(y1, cb0, cr0);
	b1 = YCbCrtoB(y1, cb0, cr0);
	#endif

	if (r0>255 ) r0 = 255;
	if (r0<0) r0 = 0;
	if (g0>255 ) g0 = 255;
	if (g0<0) g0 = 0;
	if (b0>255 ) b0 = 255;
	if (b0<0) b0 = 0;

	if (r1>255 ) r1 = 255;
	if (r1<0) r1 = 0;
	if (g1>255 ) g1 = 255;
	if (g1<0) g1 = 0;
	if (b1>255 ) b1 = 255;
	if (b1<0) b1 = 0;
	
	// 5:6:5 16bit format
	rgb0 = (((U16)r0>>3)<<11) | (((U16)g0>>2)<<5) | (((U16)b0>>3)<<0);	//RGB565.
	rgb1 = (((U16)r1>>3)<<11) | (((U16)g1>>2)<<5) | (((U16)b1>>3)<<0);	//RGB565.

	rgb = (rgb1<<16) | rgb0;

	return(rgb);
}


void Display_Cam_Image(U32 size_x, U32 size_y)
{
	U8 *buffer_y, *buffer_cb, *buffer_cr;
//	U8 y0,y1,cb0,cr0;
//	int r0,r1,g0,g1,b0,b1;
	U32 rgb_data0;//, rgb_data1; 
	U32 x, y;
	int temp;

	if (CAM_CODEC_4PP)
		temp = (((rCICOSTATUS>>26)&0x3)+4-2)%4; // current frame memory block
	else
		temp = 4;
	//Uart_Printf("Current Frame memory %d\n", temp);

	switch (temp) // current frame mem - 2
	{
	case 0:
		buffer_y = (U8 *)rCICOYSA1;
		buffer_cb = (U8 *)rCICOCBSA1;
		buffer_cr = (U8 *)rCICOCRSA1;
		break;
	case 1:
		buffer_y = (U8 *)rCICOYSA2;
		buffer_cb = (U8 *)rCICOCBSA2;
		buffer_cr = (U8 *)rCICOCRSA2;
		break;
	case 2:
		buffer_y = (U8 *)rCICOYSA3;
		buffer_cb = (U8 *)rCICOCBSA3;
		buffer_cr = (U8 *)rCICOCRSA3;
		break;
	case 3:
		buffer_y = (U8 *)rCICOYSA4;
		buffer_cb = (U8 *)rCICOCBSA4;
		buffer_cr = (U8 *)rCICOCRSA4;
		break;
	default :
		buffer_y = (U8 *)rCICOYSA1;
		buffer_cb = (U8 *)rCICOCBSA1;
		buffer_cr = (U8 *)rCICOCRSA1;
		break;
	}

	//Uart_Printf("End setting : Y-0x%x, Cb-0x%x, Cr-0x%x\n", buffer_y, buffer_cb, buffer_cr);	
#if 0
	// for checking converting time 
	rGPGCON = (rGPGCON&~(1<<23))|(1<<22); //EINT19 -> GPG11 output
	rGPGUP |= (1<<11);
	rGPGDAT &= ~(1<<11);
	Delay(90);
	rGPGDAT |=(1<<11);
	
	rgb_data0 = 0;
#endif

#if CAM_CODEC_OUTPUT==CAM_CCIR420
	for (y=0;y<size_y;y++) // YCbCr 4:2:0 format
	{
		for (x=0;x<size_x;x+=2)
		{		
			rgb_data0 = Conv_YCbCr_Rgb(*buffer_y++, *buffer_y++, *buffer_cb++, *buffer_cr++);
			frameBuffer16BitTft240320[y][x/2] = rgb_data0;

			if ( (x==(size_x-2)) && ((y%2)==0) ) // when x is last pixel & y is even number
			{
				buffer_cb -= size_x/2;
				buffer_cr -= size_x/2;
			} 	
		}
	}	
#else
	for (y=0;y<size_y;y++) // YCbCr 4:2:2 format
	{
		for (x=0;x<size_x;x+=2)
		{ 	
			rgb_data0 = Conv_YCbCr_Rgb(*buffer_y++, *buffer_y++, *buffer_cb++, *buffer_cr++);
			frameBuffer16BitTft240320[y][x/2] = rgb_data0;
		}
	} 
#endif

//	memcpy((unsigned char*)0x30100000,  frameBuffer16BitTft240320, 320*240*2); // QCIF=178*144*2
#if 0		
	rGPGDAT &= ~(1<<11);
	Delay(30);
	rGPGDAT |=(1<<11);
	rGPGCON = (rGPGCON&~(1<<22))|(1<<23); // GPG11 output -> EINT19
#endif
}

#if 0
void Display_Cam_Image(int offset_x, int offset_y, int size_x, int size_y)
{
	U8* CamFrameAddr, LcdFrameAddr;
	int i, temp;
	
	Lcd_MoveViewPort(offset_x, offset_y, USED_LCD_TYPE);	

	switch(camPviewCaptureCount%4)
	{
	case 2 : 
		temp = rCIPRCLRSA1;
		break;
	case 3 : 
		temp = rCIPRCLRSA2;
		break;
	case 0 : 
		temp = rCIPRCLRSA3;
		break;
	case 1 :
	default:
		temp = rCIPRCLRSA4;
		break;
	}

	*CamFrameAddr = temp;
	*LcdFrameAddr = LCDFRAMEBUFFER;
	for(i=0;i<size_y;i++)
	{
		memcpy(LcdFrameAddr, CamFrameAddr, size_x*2);
		*LcdFrameAddr += size_x*4; // added virtual screen
		*CamFrameAddr += size_x*2; 
	}
}
#endif

void Test_CamPreview(void)
{

//	U8 flag;
	U32 i,*data;	//	,j,k,value,
	
	Uart_Printf("\n[ Camera Preview Test ]\n");

	//camera global variables
	camTestMode=CAM_TEST_MODE_PVIEW;
	camCodecCaptureCount=0;
	camPviewCaptureCount=0;
	camPviewStatus=CAM_STOPPED;
	camCodecStatus=CAM_STOPPED;
	flagCaptured_P=0;

	//Initialize LCD
#if CAM_PVIEW_OUTPUT==CAM_RGB24B
	Lcd_Start(MODE_TFT_24BIT_240320);
	Lcd_EnvidOnOff(0);
	// for 240x320 24bit setting
	rLCDCON5 &= ~1; // halfword swap disable -> Camera data is halfword swap type, 24bit
	rLCDSADDR3 &= ~(0x7ff<<11); // offset size 0
	rLCDSADDR2=M5D( (LCDFRAMEBUFFER+(LCD_XSIZE_TFT_240320*LCD_YSIZE_TFT_240320*4))>>1 );
	Lcd_EnvidOnOff(1);
	data = (U32 *)LCDFRAMEBUFFER;
	for (i=0;i<240*320;i++) // 24bpp test
	{
		if (i<240*80) *data++ = 0x000000ff; // blue
		else if ((i<240*160)&&(i>=240*80)) *data++ = 0x0000ff00; //green
		else if ((i<240*240)&&(i>=240*160)) *data++ = 0x00ff0000; //red
		else if (i>=240*240) *data++ = 0xff000000; // black
	}
#else // RGB16B
	Lcd_Start(MODE_TFT_16BIT_240320);
	Lcd_EnvidOnOff(0);
	// for 240x320 16bit haltword swap type setting
	rLCDCON5 |= 1; // halfword swap enable -> Camera data is halfword swap type, 16bit
	rLCDSADDR3 &= ~(0x7ff<<11); // offset size 0
	rLCDSADDR2=M5D( (LCDFRAMEBUFFER+(LCD_XSIZE_TFT_240320*LCD_YSIZE_TFT_240320*2))>>1 );
	Lcd_EnvidOnOff(1);
	data = (U32 *)LCDFRAMEBUFFER;
	for (i=0;i<240*160;i++) // 16bpp test
	{ 
		if (i<240*40) *data++ = 0x001f001f; // blue
		else if ((i<240*80)&&(i>=240*40)) *data++ = 0x07e007e0; //green
		else if ((i<240*120)&&(i>=240*80)) *data++ = 0xf800f800; //red
		else if (i>=240*120) *data++ = 0xf800001f; // blue & red
	}
#endif
	Uart_Printf("\nTFT 64K color mode test 1. Press any key!\n");
	Uart_Getch(); 	
 
 Uart_Printf(" preview sc control = %x\n", rCIPRSCCTRL);

	// Initialize Camera interface
	switch(USED_CAM_TYPE)
	{
	case CAM_S5X532 : // defualt for test : data-falling edge, ITU601, YCbCr
		CamInit(640, 480, 240, 320, 112, 20,  CAM_FRAMEBUFFER_C, CAM_FRAMEBUFFER_P);
		break;
	case CAM_S5X3A1 : // defualt for test : data-falling edge, YCbCr
		CamInit(640, 480, 240, 320, 120, 100,	CAM_FRAMEBUFFER_C, CAM_FRAMEBUFFER_P);
		rCISRCFMT = rCISRCFMT & ~((unsigned int)1<<31); // ITU656
//		rCIGCTRL &= ~(1<<26); // inverse PCLK, test pattern
		break;
	default : 	
		CamInit(640, 480, 240, 320, 0, 0,  CAM_FRAMEBUFFER_C, CAM_FRAMEBUFFER_P);
		break;
	}
	Uart_Printf(" preview sc control = %x\n", rCIPRSCCTRL);

	// Start Capture	
	rSUBSRCPND |= BIT_SUB_CAM_C|BIT_SUB_CAM_P;
	ClearPending(BIT_CAM);
	pISR_CAM = (U32)CamIsr;    
	CamPreviewIntUnmask();
	CamCaptureStart(CAM_PVIEW_SCALER_CAPTURE_ENABLE_BIT);
	Uart_Printf("Press Enter key to exit!\n");
	while (1)
	{
		if (flagCaptured_P)
		{
			flagCaptured_P = 0;
//			Uart_Printf("Enter Cam A port, count = %d\n",camCodecCaptureCount);
		}
		if (Uart_GetKey()== '\r') break;			
	}
    
	CamCaptureStop();

	Uart_Printf("\nWait until the current frame capture is completed.\n");
	while(camPviewStatus!=CAM_STOPPED)
		if (Uart_GetKey()== '\r') break;			

	Uart_Printf(" CIS format = %x\n", rCISRCFMT);
	Uart_Printf(" image cap = %x\n", rCIIMGCPT);
	Uart_Printf(" preview sc control = %x\n", rCIPRSCCTRL);
	Uart_Printf(" preview control = %x\n", rCIPRCTRL);
	Uart_Printf(" codec sc control = %x\n", rCICOSCCTRL);
	Uart_Printf(" codec control = %x\n", rCICOCTRL);
	Uart_Printf(" pr addr1 = %x\n", rCIPRCLRSA1);
	Uart_Printf(" pr addr2 = %x\n", rCIPRCLRSA2);


	Uart_Printf("camCodecCaptureCount=%d\n",camCodecCaptureCount);
	Uart_Printf("camPviewCaptureCount=%d\n",camPviewCaptureCount);
//	CamPreviewIntMask();
}



void Test_CamCodec(void)
{

	U8  fBypass='1';//	  flag,
//	U32 i,j,k,value, *data;
	
	Uart_Printf("\n[ Camera Codec Test ]\n");

	//camera global variables
	camTestMode=CAM_TEST_MODE_CODEC;
	camCodecCaptureCount=0;
	camPviewCaptureCount=0;
	camPviewStatus=CAM_STOPPED;
	camCodecStatus=CAM_STOPPED;
	flagCaptured_C=0;

	//Initialize LCD
	Lcd_Start(MODE_TFT_16BIT_240320);
	Glib_FilledRectangle(0,0,239,79,0x001f); // B
	Glib_FilledRectangle(0,80,239,159,0x07e0); // G
	Glib_FilledRectangle(0,160,239,239,0xf800); // R
	Glib_FilledRectangle(0,240,239,319,0xffff); 	 
	Uart_Printf("\nTFT 64K color mode test 1. Press any key!\n");
	Uart_Getch(); 	

	// Initialize Camera interface
	switch(USED_CAM_TYPE)
	{
	case CAM_S5X532 : // defualt for test : data-falling edge, ITU601, YCbCr
		CamInit(240, 320, 240, 320, 112, 20,  CAM_FRAMEBUFFER_C, CAM_FRAMEBUFFER_P);
		break;
	case CAM_S5X3A1 : // defualt for test : data-falling edge, ITU601, YCbCr
		CamInit(240, 320, 240, 320, 0, 0, CAM_FRAMEBUFFER_C, CAM_FRAMEBUFFER_P);
		rCISRCFMT &= ~((unsigned int)1<<31); // ITU656
		break;
	default : 	
		CamInit(240, 320, 240, 320, 0, 0,  CAM_FRAMEBUFFER_C, CAM_FRAMEBUFFER_P);	
		break;
	}

	// Start Capture	
	rSUBSRCPND |= BIT_SUB_CAM_C|BIT_SUB_CAM_P;
	ClearPending(BIT_CAM);
	pISR_CAM = (U32)CamIsr;    
	CamCodecIntUnmask();

	Uart_Printf("Select Code mode : 1. Scaler mode(D)   2. Bypass mode\n");
	fBypass = Uart_Getch();
	if (fBypass=='2'){
		CamInit(CAM_SRC_HSIZE, CAM_SRC_VSIZE, 240, 320, 0, 0, CAM_FRAMEBUFFER_C, CAM_FRAMEBUFFER_P);
		CamCaptureStart(CAM_CAMIF_GLOBAL_CAPTURE_ENABLE_BIT); //bypass
	}
	else
		CamCaptureStart(CAM_CODEC_SCALER_CAPTURE_ENABLE_BIT); //scaler

	// for test simultanious
//	rCIIMGCPT |= CAM_PVIEW_SCALER_CAPTURE_ENABLE_BIT;
//	rCIPRSCCTRL |= CAM_PVIEW_SACLER_START_BIT;

	
	Uart_Printf("Press Enter key to exit!\n");
	while (1)
	{
		if (flagCaptured_C)
		{
			flagCaptured_C = 0;
			if (fBypass=='2')
				Display_Cam_Image(CAM_SRC_HSIZE, CAM_SRC_VSIZE);
			else 
				Display_Cam_Image(240, 320);
//			Uart_Printf("Enter Cam B port, count = %d\n",camPviewCaptureCount);
		}
//		if (camCodecCaptureCount>=1) 
//		{ 
//			Uart_Printf("cnt = %d\n", camCodecCaptureCount); 
//		    break;			
//		}
		if (Uart_GetKey()== '\r') break;			
	}
    
	CamCaptureStop();

	Uart_Printf("\nWait until the current frame capture is completed.\n");
	while(camCodecStatus!=CAM_STOPPED)
		if (Uart_GetKey()== '\r') break;			
		
	Uart_Printf(" CIS format = %x\n", rCISRCFMT);
	Uart_Printf(" image cap = %x\n", rCIIMGCPT);
	Uart_Printf(" preview sc control = %x\n", rCIPRSCCTRL);
	Uart_Printf(" preview control = %x\n", rCIPRCTRL);
	Uart_Printf(" codec sc control = %x\n", rCICOSCCTRL);
	Uart_Printf(" codec control = %x\n", rCICOCTRL);
	Uart_Printf(" codec status = %x\n", rCICOSTATUS);
	
	Uart_Printf("camCodecCaptureCount=%d\n",camCodecCaptureCount);
	Uart_Printf("camPviewCaptureCount=%d\n",camPviewCaptureCount);
//	CamCodecIntMask();
}


void Test_YCbCr_to_RGB(void)
{
	U8 *buffer_y, *buffer_cb, *buffer_cr;
	int size_x, size_y, R, G, B, rgb_data; 
	int x, y;	
	int pos_x = 0;
	int pos_y = 0;
	U8 cSelType = 0;
	U8 *cBuffer;

	Lcd_Start(MODE_TFT_16BIT_240320);
	
	size_x = PQVGA_XSIZE;
	size_y = PQVGA_YSIZE;

	Uart_Printf("Select   1. 420[D] PQVGA    2. 422 PQVGA    3. 420 CIF  : \n");
	cSelType = Uart_Getch();
	if (cSelType == '\r') cSelType = '1';

	switch (cSelType)
	{
	case '1' : 
		cBuffer = (U8 *)c420jpeg;
		break;		
	case '2' : 
		cBuffer = (U8 *)c422jpeg;
		break;
	case '3' : 
		cBuffer = (U8 *)foreman_cif_420;
		size_x = CIF_XSIZE;
		size_y = CIF_YSIZE;
		break;
	}

	buffer_y = (U8 *)(cBuffer);
	buffer_cb = (U8 *)(cBuffer + size_x*size_y); // add y size
	buffer_cr = (U8 *)(buffer_cb + size_x*size_y/4); // add y,cb size
	if (cSelType == '2')
		buffer_cr = (U8 *)(buffer_cb + size_x*size_y/2); // add y,cb size
		

	Uart_Printf("End setting : Y-0x%x, Cb-0x%x, Cr-0x%x\n", buffer_y, buffer_cb, buffer_cr);	
	Uart_Printf("Address : Y-0x%x, foreman-0x%x, fa-0x%x\n", *buffer_y, cBuffer, *cBuffer);
	
	Glib_FilledRectangle(0,0,239,159,0xaaaa); 	 
	Glib_FilledRectangle(0,160,239,320,0xf800); 	 
	Uart_Printf("\nTFT 64K color mode test 1. Press any key!\n");
	Uart_Getch(); 	

	for (y=0;y<size_y;y++) // YCbCr 4:2:0 format
		for (x=0;x<size_x;x++)
		{	
			R = YCbCrtoR(*buffer_y, *buffer_cb, *buffer_cr);
			G = YCbCrtoG(*buffer_y, *buffer_cb, *buffer_cr);
			B = YCbCrtoB(*buffer_y, *buffer_cb, *buffer_cr);

			if (R>255 ) R = 255;
			if (G>255 ) G = 255;
			if (B>255 ) B = 255;
			if (R<0 ) R = 0;
			if (G<0 ) G = 0;
			if (B<0 ) B = 0;

			// 5:6:5 16bit format
			rgb_data =(R&0xf8)<<8;  // R 5bits
			rgb_data|=(G&0xfc)<<3; // G 6bits
			rgb_data|=(B&0xf8)>>3; // B 5bits
			PutPixel(pos_x+x, pos_y+y, rgb_data);

			// next usable bits..
			buffer_y++;
			if (x%2) // when x is odd number
			{
				buffer_cb++;
				buffer_cr++;
			}
			if (cSelType != '2')
				if ( (x==(size_x-1)) && ((y%2)==0) ) // when x is last pixel & y is even number
				{
					buffer_cb -= size_x/2;
					buffer_cr -= size_x/2;
				}		
		}

    Uart_Printf("Virtual Screen Test(TFT 64K color). Press any key[ijkm\\r]!\n");
    MoveViewPort(MODE_TFT_16BIT_240320); // user can adjust screen..
    
    Lcd_MoveViewPort(0,0,MODE_TFT_16BIT_240320); // return 0,0
    Glib_ClearScr(0, MODE_TFT_16BIT_240320);
    Lcd_EnvidOnOff(0);
    Lcd_PowerEnable(0, 0);
    Lcd_Port_Return();
    
}

