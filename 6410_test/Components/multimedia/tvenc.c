/**************************************************************************************
* 
*	Project Name : S3C6410 Validation
*
*	Copyright 2007 by Samsung Electronics, Inc.
*	All rights reserved.
*
*	Project Description :
*		This software is only for validating functions of the S3C6410.
*		Anybody can use this software without our permission.
*  
*--------------------------------------------------------------------------------------
* 
*	File Name : tvenc_test.c
*  
*	File Description : This file implements the API functons for TV Encoder controller.
*
*	Author : Heemyung.noh
*	Dept. : AP Development Team
*	Created Date : 2007/01/15
*	Version : 0.1 
* 
*	History
*	- Created(Heemyung.noh 2007/01/15)
*  
**************************************************************************************/


#include <math.h>

#include "sfr6410.h"
#include "system.h"
#include "library.h"
#include "tvenc.h"
#include "intc.h"

enum TVENC_REG 
{
// TV Controller
	TVCTRL              = TVENC_BASE+0x00,
	VBPORCH             = TVENC_BASE+0x04,
	HBPORCH             = TVENC_BASE+0x08,
	HENHOFFSET          = TVENC_BASE+0x0C,
	VDEMOWSIZE          = TVENC_BASE+0x10,
	HDEMOWSIZE          = TVENC_BASE+0x14,
	INIMAGESIZE         = TVENC_BASE+0x18,

// Encoder
	PEDCTRL             = TVENC_BASE+0x1C,
	YCFILTERBW          = TVENC_BASE+0x20,
	HUECTRL             = TVENC_BASE+0x24,
	FSCCTRL             = TVENC_BASE+0x28,
	FSCDTOMANCTRL       = TVENC_BASE+0x2C,
	BGCTRL              = TVENC_BASE+0x34,
	BGHVAVCTRL          = TVENC_BASE+0x38,
	BWSrtVal  			= TVENC_BASE+0x3C,
	DCAPL  	 			= TVENC_BASE+0x40,

// Image Enhancer
	CONTRABRIGHT        = TVENC_BASE+0x44,
	CBCRGAINCTRL        = TVENC_BASE+0x48,
	DEMOWINCTRL         = TVENC_BASE+0x4C,
	FTCA  				= TVENC_BASE+0x50,
	BWTiltHDLY  			= TVENC_BASE+0x54,	
	BWGAIN              		= TVENC_BASE+0x58,
	BWStrCTRL  			= TVENC_BASE+0x5C,	
	SHARPCTRL           = TVENC_BASE+0x60,
	GAMMACTRL           = TVENC_BASE+0x64,
	FSCAUXCTRL          = TVENC_BASE+0x68,
	SYNCSIZECTRL        = TVENC_BASE+0x6C,
	BURSTCTRL           = TVENC_BASE+0x70,
	MACROBURSTCTRL      = TVENC_BASE+0x74,
	ACTVIDPOSCTRL       = TVENC_BASE+0x78,
	ENCCTRL             = TVENC_BASE+0x7C,
	MUTECTRL            = TVENC_BASE+0x80,
	MACROVISION0        = TVENC_BASE+0x84,
	MACROVISION1        = TVENC_BASE+0x88,
	MACROVISION2        = TVENC_BASE+0x8C,
	MACROVISION3        = TVENC_BASE+0x90,
	MACROVISION4        = TVENC_BASE+0x94,
	MACROVISION5        = TVENC_BASE+0x98,
	MACROVISION6        = TVENC_BASE+0x9C,
	VBIOn  				= TVENC_BASE+0xA0,
	TVConFSMState		= TVENC_BASE+0xA4,
	IPInfo				= TVENC_BASE+0xA8	
};

// tv controller control sfr set
enum TVCTRL_BIT
{
	TVC_FIFOURINT_DIS   = 0<<16,
	TVC_FIFOURINT_ENA   = 1<<16,
	TVC_FIFOURINT_OCCUR	= 1<<12,
	TVC_OUTTYPE_C       = 0<<8,
	TVC_OUTTYPE_S       = 1<<8,
	TVC_OUTFMT_NTSC_M   = 0<<4,
	TVC_OUTFMT_NTSC_J   = 1<<4,
	TVC_OUTFMT_PAL_BDG  = 2<<4,
	TVC_OUTFMT_PAL_M    = 3<<4,
	TVC_OUTFMT_PAL_NC    = 4<<4,
	TVC_OFF             = 0<<0,
	TVC_ON              = 1<<0
};

// vertical back porch control
#define VBP_VEFBPD(n)   (((n)&0x1FF)<<16)
#define VBP_VOFBPD(n)   (((n)&0xFF)<<0)
enum VBPORCH_BIT
{
	VBP_VEFBPD_NTSC     = 0x11C<<16,
	VBP_VEFBPD_PAL      = 0x14F<<16,
	VBP_VOFBPD_NTSC     = 0x15<<0,
	VBP_VOFBPD_PAL      = 0x16<<0
};

// horizontal back porch end point
#define HBP_HSPW(n)     (((n)&0xFF)<<16)
#define HBP_HBPD(n)     (((n)&0x7FF)<<0)
enum HBPORCH_BIT
{
	HBP_HSPW_NTSC       = 0x80<<16,
	HBP_HSPW_PAL        = 0x80<<16,
	HBP_HBPD_NTSC       = 0xF4<<0,
	HBP_HBPD_PAL        = 0x108<<0
};

// horizontal enhancer offset
#define HEO_VAWCC(n)    (((n)&0x3F)<<24)
#define HEO_HAWCC(n)    (((n)&0xFF)<<16)
#define HEO_DTO(n)      (((n)&0x7)<<8)
#define HEO_HEOV(n)     (((n)&0x1F)<<0)
enum HENHOFFSET_BIT
{
//	HEO_DTO_NTSC        = 0x5<<8,
//	HEO_DTO_PAL         = 0x5<<8,
	//rb1004
	HEO_DTO_NTSC        = 0x4<<8,
	HEO_DTO_PAL         = 0x4<<8,	
	
	HEO_HEOV_NTSC       = 0x1A<<0,
	HEO_HEOV_PAL        = 0x1A<<0
};


// vertical demo window size
#define VDW_VDWS(n)     (((n)&0x1FF)<<16)
#define VDW_VDWSP(n)    (((n)&0x1FF)<<0)
enum VDEMOWSIZE_BIT
{
	VDW_VDWS_DEF        = 0xF0<<16,
	VDW_VDWSP_DEF       = 0x0<<0
};

// horizontal demo window size
#define HDW_HDWEP(n)    (((n)&0x7FF)<<16)
#define HDW_HDWSP(n)    (((n)&0x7FF)<<0)
enum HDEMOWSIZE_BIT
{
	HDW_HDWEP_DEF       = 0x5A0<<16,
	HDW_HDWSP_DEF       = 0x0<<0
};

// input image size
#define IIS_HEIGHT(n)   (((n)&0x3FF)<<16)
#define IIS_WIDTH(n)    (((n)&0x7FF)<<0)

// encoder pedestal control
enum PEDCTRL_BIT
{
	EPC_PED_ON          = 0<<0,
	EPC_PED_OFF         = 1<<0
};

// yc filter bandwidth control
enum YCFILTERBW_BIT
{
	YFB_YBW_60          = 0<<4,
	YFB_YBW_38          = 1<<4,
	YFB_YBW_31          = 2<<4,
	YFB_YBW_26          = 3<<4,
	YFB_YBW_21          = 4<<4,
	YFB_CBW_12          = 0<<0,
	YFB_CBW_10          = 1<<0,
	YFB_CBW_08          = 2<<0,
	YFB_CBW_06          = 3<<0
};

// hue control
#define HUE_CTRL(n)     (((n)&0xFF)<<0)

// fsc control
#define FSC_CTRL(n)     (((n)&0x7FFF)<<0)

// fsc dto manually control enable
#define FDM_CTRL(n)     (((n)&0x7FFFFFFF)<<0)

// background control
#define BGC_BGYOFS(n)   (((n)&0xF)<<0)
enum BGCTRL_BIT
{
	BGC_SME_DIS         = 0<<8,
	BGC_SME_ENA         = 1<<8,
	BGC_BGCS_BLACK      = 0<<4,
	BGC_BGCS_BLUE       = 1<<4,
	BGC_BGCS_RED        = 2<<4,
	BGC_BGCS_MAGENTA    = 3<<4,
	BGC_BGCS_GREEN      = 4<<4,
	BGC_BGCS_CYAN       = 5<<4,
	BGC_BGCS_YELLOW     = 6<<4,
	BGC_BGCS_WHITE      = 7<<4
};

// background vav & hav control
#define BVH_BG_HL(n)    (((n)&0xFF)<<24)
#define BVH_BG_HS(n)    (((n)&0xFF)<<16)
#define BVH_BG_VL(n)    (((n)&0xFF)<<8)
#define BVH_BG_VS(n)    (((n)&0xFF)<<0)

// sync size control
#define SSC_HSYNC(n)    (((n)&0x3FF)<<0)
enum SYNCSIZECTRL_BIT
{
	SSC_HSYNC_NTSC      = 0x3D<<0,
	SSC_HSYNC_PAL       = 0x3E<<0
};

// burst signal control
#define BSC_BEND(n)     (((n)&0x3FF)<<16)
#define BSC_BSTART(n)   (((n)&0x3FF)<<0)
enum BURSTCTRL_BIT
{
	BSC_BEND_NTSC       = 0x69<<16,
	BSC_BEND_PAL        = 0x6A<<16,
	BSC_BSTART_NTSC     = 0x49<<0,
	BSC_BSTART_PAL      = 0x4A<<0
};

// macrovision burst signal control
#define MBS_BSTART(n)   (((n)&0x3FF)<<0)
enum MACROBURSTCTRL_BIT
{
	MBS_BSTART_NTSC     = 0x41<<0,
	MBS_BSTART_PAL      = 0x42<<0
};

// active video position control
#define AVP_AVEND(n)    (((n)&0x3FF)<<16)
#define AVP_AVSTART(n)  (((n)&0x3FF)<<0)
enum ACTVIDPOSCTRL_BIT
{
	AVP_AVEND_NTSC      = 0x348<<16,
	AVP_AVEND_PAL       = 0x352<<16,
	AVP_AVSTART_NTSC    = 0x78<<0,
	AVP_AVSTART_PAL     = 0x82<<0
};

// encoder control
enum ENCCTRL_BIT
{
	ENC_BGEN_DIS        = 0<<0,
	ENC_BGEN_ENA        = 1<<0
};

#define NTSC_WIDTH		(720)
#define NTSC_HEIGHT		(480)
#define PAL_WIDTH		(720)
#define PAL_HEIGHT		(576)

//////////
// Function Name : TVENC_TurnOnOff
// Function Description : TV Encoder On/Off
// Input : 	uOnOff - 1 : On
//					0 : Off
// Output : None
void TVENC_TurnOnOff(u8 uOnOff)
{
	u32 uTemp;

	uTemp = Inp32(TVCTRL);

	if (uOnOff)
		Outp32(TVCTRL, uTemp|TVC_ON);
	else
		Outp32(TVCTRL, uTemp&~TVC_ON);
}


//////////
// Function Name : TVENC_SetImageSize
// Function Description : Set the input image size
// Input : 	uWSize - Width of Input Image(Real image size * 2)
//			uHSize -	Height of nput Image
// Output : None
void TVENC_SetImageSize(u32 uWSize, u32 uHSize)
{
	Outp32(INIMAGESIZE, IIS_HEIGHT(uHSize) | IIS_WIDTH(uWSize));
}


//////////
// Function Name : TVENC_ClearUnderrunInt
// Function Description : Clear the Under-run Interrupt pending bit
// Input : None
// Output : None
void TVENC_ClearUnderrunInt(void)
{
	u32 uTemp;

	uTemp = Inp32(TVCTRL);
	Outp32(TVCTRL, uTemp|TVC_FIFOURINT_OCCUR);
}


//////////
// Function Name : TVENC_EnableMacroVision
// Function Description : Enable Macrovision
// Input : 	eTvmode - TV output format
//			ePattern - Macrovision pattern
// Output : None
void TVENC_EnableMacroVision(TV_STANDARDS eTvmode, eMACROPATTERN ePattern)
{
	switch(ePattern)
	{
		case eAGC4L : 	Outp32(MACROVISION0, 0x2115D73E);
						Outp32(MACROVISION1, 0x02050515);
						Outp32(MACROVISION2, 0x00241B1B);
						Outp32(MACROVISION3, 0x000007F8);
						Outp32(MACROVISION4, 0x01600F0F);
						Outp32(MACROVISION5, 0x0405000A);
						Outp32(MACROVISION6, 0x000003FF);
						break;
		case eAGC2L : 	Outp32(MACROVISION0, 0x25111D3E);
						Outp32(MACROVISION1, 0x00070101);
						Outp32(MACROVISION2, 0x00241B1B);
						Outp32(MACROVISION3, 0x000007F8);
						Outp32(MACROVISION4, 0x01600F0F);
						Outp32(MACROVISION5, 0x0405000A);
						Outp32(MACROVISION6, 0x000003FF);
						break;
		case eN01 	: 	Outp32(MACROVISION0, 0x2115173E);
						Outp32(MACROVISION1, 0x03050515);
						Outp32(MACROVISION2, 0x00231C19);
						Outp32(MACROVISION3, 0x7E070FF8);
						Outp32(MACROVISION4, 0x01910EF0);
						Outp32(MACROVISION5, 0x02030705);
						Outp32(MACROVISION6, 0x000003C3);
						break;
		case eN02 	: 	Outp32(MACROVISION0, 0x1A2A2F3E);
						Outp32(MACROVISION1, 0x03040236);
						Outp32(MACROVISION2, 0x001D2524);
						Outp32(MACROVISION3, 0x6DCF36B8);
						Outp32(MACROVISION4, 0x00701323);
						Outp32(MACROVISION5, 0x050A0302);
						Outp32(MACROVISION6, 0x000003A0);
						break;
		case eP01	: 	Outp32(MACROVISION0, 0x2A221A3E);
						Outp32(MACROVISION1, 0x00020522);
						Outp32(MACROVISION2, 0x00143D1C);
						Outp32(MACROVISION3, 0x015403FE);
						Outp32(MACROVISION4, 0x00607EFE);
						Outp32(MACROVISION5, 0x07040008);
						Outp32(MACROVISION6, 0x00000155);
						break;
		case eP02 	: 	Outp32(MACROVISION0, 0x2A221A3E);
						Outp32(MACROVISION1, 0x03020522);
						Outp32(MACROVISION2, 0x002B1223);
						Outp32(MACROVISION3, 0x1F4378C6);
						Outp32(MACROVISION4, 0x01F0A353);
						Outp32(MACROVISION5, 0x02030C0B);
						Outp32(MACROVISION6, 0x00000385);
						break;
		default : break;
	}
}


//////////
// Function Name : TVENC_DisableMacroVision
// Function Description : Disable Macrovision
// Input : None
// Output : None
void TVENC_DisableMacroVision(void)
{
	u32 uTemp;

	uTemp = Inp32(MACROVISION0);
	uTemp &= ~(0xFF);
	Outp32(MACROVISION0, uTemp);
}


//////////
// Function Name : TVENC_SetTvConMode
// Function Description : Set the TV output format & type
// Input : 	eTvmode - TV output format
//			tvout - TV output type
// Output : None
void TVENC_SetTvConMode(TV_STANDARDS eTvmode, eTV_CONN_TYPE eTvout)
{
	u16 usOutporttype, usOutsigtype;
	u32 uTemp;

	//FscDTOManual setting disable
	Outp32(FSCDTOMANCTRL, 0);
	
	switch (eTvmode)
	{
#if 0
		case PAL_N :
			Outp32(VBPORCH, VBP_VEFBPD_PAL|VBP_VOFBPD_PAL);
			Outp32(HBPORCH, HBP_HSPW_PAL|HBP_HBPD_PAL);
			Outp32(HENHOFFSET, HEO_DTO_PAL|HEO_HEOV_PAL);
			Outp32(PEDCTRL, EPC_PED_ON);		
			Outp32(YCFILTERBW, YFB_YBW_26|YFB_CBW_06);	
			Outp32(SYNCSIZECTRL, SSC_HSYNC_PAL);
			Outp32(BURSTCTRL, BSC_BEND_PAL|BSC_BSTART_PAL);
			Outp32(MACROBURSTCTRL, MBS_BSTART_PAL);
			Outp32(ACTVIDPOSCTRL, AVP_AVEND_PAL|AVP_AVSTART_PAL);
			break;			
#endif
		case PAL_NC : //PAL type
		case PAL_BGHID :
			Outp32(VBPORCH, VBP_VEFBPD_PAL|VBP_VOFBPD_PAL);
			Outp32(HBPORCH, HBP_HSPW_PAL|HBP_HBPD_PAL);
			Outp32(HENHOFFSET, HEO_DTO_PAL|HEO_HEOV_PAL);
			Outp32(PEDCTRL, EPC_PED_OFF);	
			Outp32(YCFILTERBW, YFB_YBW_26|YFB_CBW_06);	//rb1004....PAL_NC:3.58205MHz????
			Outp32(SYNCSIZECTRL, SSC_HSYNC_PAL);
			Outp32(BURSTCTRL, BSC_BEND_PAL|BSC_BSTART_PAL);
			Outp32(MACROBURSTCTRL, MBS_BSTART_PAL);
			Outp32(ACTVIDPOSCTRL, AVP_AVEND_PAL|AVP_AVSTART_PAL);
			//rb1004... Sub_carrier reset enable
			Outp32(FSCAUXCTRL, 0x11);
			break;

		case NTSC_443:
			Outp32(VBPORCH, VBP_VEFBPD_NTSC|VBP_VOFBPD_NTSC);
			Outp32(HBPORCH, HBP_HSPW_NTSC|HBP_HBPD_NTSC);
			Outp32(HENHOFFSET, HEO_DTO_NTSC|HEO_HEOV_NTSC);
			Outp32(PEDCTRL, EPC_PED_ON);
			Outp32(YCFILTERBW, YFB_YBW_26|YFB_CBW_06);
			Outp32(SYNCSIZECTRL, SSC_HSYNC_NTSC);
			Outp32(BURSTCTRL, BSC_BEND_NTSC|BSC_BSTART_NTSC);
			Outp32(MACROBURSTCTRL, MBS_BSTART_NTSC);
			Outp32(ACTVIDPOSCTRL, AVP_AVEND_NTSC|AVP_AVSTART_NTSC);
			//rb1004... Sub_carrier reset enable
			Outp32(FSCAUXCTRL, 0x01);
			break;

		case NTSC_J	:
			Outp32(VBPORCH, VBP_VEFBPD_NTSC|VBP_VOFBPD_NTSC);
			Outp32(HBPORCH, HBP_HSPW_NTSC|HBP_HBPD_NTSC);
			Outp32(HENHOFFSET, HEO_DTO_NTSC|HEO_HEOV_NTSC);
			Outp32(PEDCTRL, EPC_PED_OFF);
			Outp32(YCFILTERBW, YFB_YBW_21|YFB_CBW_06);
			Outp32(SYNCSIZECTRL, SSC_HSYNC_NTSC);
			Outp32(BURSTCTRL, BSC_BEND_NTSC|BSC_BSTART_NTSC);
			Outp32(MACROBURSTCTRL, MBS_BSTART_NTSC);
			Outp32(ACTVIDPOSCTRL, AVP_AVEND_NTSC|AVP_AVSTART_NTSC);
			//rb1004... Sub_carrier reset enable
			Outp32(FSCAUXCTRL, 0x01);
			break;	
			
		case PAL_M 	:	
			Outp32(VBPORCH, VBP_VEFBPD_NTSC|VBP_VOFBPD_NTSC);
			Outp32(HBPORCH, HBP_HSPW_NTSC|HBP_HBPD_NTSC);
			Outp32(HENHOFFSET, HEO_DTO_NTSC|HEO_HEOV_NTSC);
			Outp32(PEDCTRL, EPC_PED_ON);
			Outp32(YCFILTERBW, YFB_YBW_21|YFB_CBW_06);
			Outp32(SYNCSIZECTRL, SSC_HSYNC_NTSC);
			Outp32(BURSTCTRL, BSC_BEND_NTSC|BSC_BSTART_NTSC);
			Outp32(MACROBURSTCTRL, MBS_BSTART_NTSC);
			Outp32(ACTVIDPOSCTRL, AVP_AVEND_NTSC|AVP_AVSTART_NTSC);
			//rb1004... Sub_carrier reset enable
			Outp32(FSCAUXCTRL, 0x11);
			break;			
		case NTSC_M	:
		default :			
			Outp32(VBPORCH, VBP_VEFBPD_NTSC|VBP_VOFBPD_NTSC);
			Outp32(HBPORCH, HBP_HSPW_NTSC|HBP_HBPD_NTSC);
			Outp32(HENHOFFSET, HEO_DTO_NTSC|HEO_HEOV_NTSC);
			Outp32(PEDCTRL, EPC_PED_ON);
			Outp32(YCFILTERBW, YFB_YBW_21|YFB_CBW_06);
			Outp32(SYNCSIZECTRL, SSC_HSYNC_NTSC);
			Outp32(BURSTCTRL, BSC_BEND_NTSC|BSC_BSTART_NTSC);
			Outp32(MACROBURSTCTRL, MBS_BSTART_NTSC);
			Outp32(ACTVIDPOSCTRL, AVP_AVEND_NTSC|AVP_AVSTART_NTSC);
			//rb1004... Sub_carrier reset enable
			Outp32(FSCAUXCTRL, 0x01);
			break;
	}
			
	if (eTvout == eS_VIDEO)
	{
		Outp32(YCFILTERBW, YFB_YBW_60|YFB_CBW_06);
		usOutporttype = TVC_OUTTYPE_S;
	}
	else
		usOutporttype = TVC_OUTTYPE_C;

	switch (eTvmode)
	{
		case NTSC_M :
			usOutsigtype = TVC_OUTFMT_NTSC_M;
			break;
		case NTSC_J :
			usOutsigtype = TVC_OUTFMT_NTSC_J;
			break;
		case PAL_BGHID:
			usOutsigtype = TVC_OUTFMT_PAL_BDG;
			break;
		case PAL_M :
			usOutsigtype = TVC_OUTFMT_PAL_M;
			break;
		case PAL_NC :
			usOutsigtype = TVC_OUTFMT_PAL_NC;
			break;
	}

	uTemp = Inp32(TVCTRL);
	Outp32(TVCTRL, (uTemp&~(0x1F<<4))|usOutporttype|usOutsigtype);

}


//////////
// Function Name : TVENC_SetDemoWinSize
// Function Description : Set the enhancing demo window size
// Input : 	uHsz - Horizontal demo window size
//			uVsz - Vertical demo window size
//			uHst - Horizontal demo window start point
//			uVst - Vertical demo window start point
// Output : None
void TVENC_SetDemoWinSize(u32 uHsz, u32 uVsz, u32 uHst, u32 uVst)
{
	Outp32(VDEMOWSIZE, VDW_VDWS(uVsz)|VDW_VDWSP(uVst));
	Outp32(HDEMOWSIZE, HDW_HDWEP(uVsz)|HDW_HDWSP(uVst));
}


//////////
// Function Name : TVENC_SetEncPedestal
// Function Description : Control the encoder pedestal 
// Input : 	bOnOff - TRUE : pedestal on
//					FALSE : pedestal off
// Output : None
void TVENC_SetEncPedestal(u8 bOnOff)
{
	if (bOnOff)
		Outp32(PEDCTRL, EPC_PED_ON);
	else
		Outp32(PEDCTRL, EPC_PED_OFF);
}


//////////
// Function Name : TVENC_SetSubCarrierFreq
// Function Description : Control the Fsc 
// Input : 	uFreq - Current DTO set value + uFreq[14:0]*(2^9)
// Output : 	None
void TVENC_SetSubCarrierFreq(u32 uFreq)
{
	Outp32(FSCCTRL, FSC_CTRL(uFreq));
}


//////////
// Function Name : TVENC_SetFscDTO
// Function Description : Fsc DTO manual control & enable
// Input : 	uVal - FscDTOManual[30:0] = Fsc*(2^33)/Fclk[MHz]
// Output : 	None
void TVENC_SetFscDTO(u32 uVal)
{
	u32 uTemp;

	uTemp = (u32)((u32)(1<<31)|(uVal & 0x7FFFFFFF));
	Outp32(FSCDTOMANCTRL, uTemp);
}


//////////
// Function Name : TVENC_DisableFscDTO
// Function Description : Disable the Fsc DTO manual control
// Input : 	None
// Output : 	None
void TVENC_DisableFscDTO(void)
{
	u32 uTemp;

	uTemp = Inp32(FSCDTOMANCTRL);
	uTemp &= ~(1<<31);
	Outp32(FSCDTOMANCTRL, uTemp);
}


//////////
// Function Name : TVENC_SetBackGround
// Function Description : Set the background color & luminance offset and enable/disable soft mixed enable
// Input : 	bSmeUsed - Soft mixed enable or disable
//			uColNum - Background color select
//			uLumaOffset - Background luminance offset
// Output : 	None
void TVENC_SetBackGround(u8 bSmeUsed, u32 uColNum, u32 uLumaOffset)
{
	u32 uBgColor;

	switch (uColNum)
	{
		case 0 : uBgColor = BGC_BGCS_BLACK; break;
		case 1 : uBgColor = BGC_BGCS_BLUE; break;
		case 2 : uBgColor = BGC_BGCS_RED; break;
		case 3 : uBgColor = BGC_BGCS_MAGENTA; break;
		case 4 : uBgColor = BGC_BGCS_GREEN; break;
		case 5 : uBgColor = BGC_BGCS_CYAN; break;
		case 6 : uBgColor = BGC_BGCS_YELLOW; break;
		case 7 : uBgColor = BGC_BGCS_WHITE; break;
	}

	if (bSmeUsed)
		Outp32(BGCTRL, BGC_SME_ENA|uBgColor|BGC_BGYOFS(uLumaOffset));
	else
		Outp32(BGCTRL, BGC_SME_DIS|uBgColor|BGC_BGYOFS(uLumaOffset));
}


//////////
// Function Name : TVENC_SetBgVavHav
// Function Description : Set the background VAV & HAV
// Input : 	uHavLen - Background HAV Length
//			uVavLen - Background VAV Length
//			uHavSt - Background HAV Start Position
//			uVavSt - Background VAV Start Position
// Output : 	None
void TVENC_SetBgVavHav(u32 uHavLen, u32 uVavLen, u32 uHavSt, u32 uVavSt)
{
	Outp32(BGHVAVCTRL, BVH_BG_HL(uHavLen)|BVH_BG_HS(uHavSt)|BVH_BG_VL(uVavLen)|BVH_BG_VS(uVavSt));
}


//////////
// Function Name : TVENC_SetHuePhase
// Function Description : Set the HUE Phase
// Input : 	uInc - HUE Phase value( 0x00 : 0' Phase,  0xFF : 58.5938' Phase)
// Output : 	None
void TVENC_SetHuePhase(u32 uInc)
{
	Outp32(HUECTRL, HUE_CTRL(uInc));
}


//////////
// Function Name : TVENC_GetHuePhase
// Function Description : Get the current HUE Phase value
// Input : 	None
// Output : 	the current HUE Phase value
u32 TVENC_GetHuePhase(void)
{
	u32 uTemp;

	uTemp = Inp32(HUECTRL);
	return (uTemp&0xFF);

	//rb1004....6400
	//return Inp32(HUECTRL);
}


//////////
// Function Name : TVENC_SetContrast
// Function Description : Set the Contrast Gain
// Input : 	uContrast - Contrast gain value
// Output : 	None
void TVENC_SetContrast(u32 uContrast)
{
	u32 uTemp;

	uTemp = Inp32(CONTRABRIGHT);
	uTemp = (uTemp&~(0xFF<<0)) | (uContrast<<0);
	Outp32(CONTRABRIGHT, uTemp);
}


//////////
// Function Name : TVENC_GetContrast
// Function Description : Get the current Contrast gain value
// Input : 	None
// Output : 	the current Contrast gain value
u32 TVENC_GetContrast(void)
{
	u32 uTemp;

	uTemp = Inp32(CONTRABRIGHT);
	return (uTemp & 0xFF);
}


//////////
// Function Name : TVENC_SetBright
// Function Description : Set the Brightness Gain
// Input : 	uBright - Brightness gain value
// Output : 	None
void TVENC_SetBright(u32 uBright)
{
	u32 uTemp;

	uTemp = Inp32(CONTRABRIGHT);
	uTemp = (uTemp&~(0xFF<<16)) | (uBright<<16);
	Outp32(CONTRABRIGHT, uTemp);
}


//////////
// Function Name : TVENC_GetBright
// Function Description : Get the current Brightness gain value
// Input : 	None
// Output : 	the current Brightness gain value
u32 TVENC_GetBright(void)
{
	u32 uTemp;

	uTemp = Inp32(CONTRABRIGHT);
	uTemp = (uTemp & (0xFF<<16))>>16;
	return uTemp;
}


//////////
// Function Name : TVENC_SetCbGain
// Function Description : Set the Cb Gain
// Input : 	uCbGain - Cb gain value
// Output : 	None
void TVENC_SetCbGain(u32 uCbGain)
{
	u32 uTemp;

	uTemp = Inp32(CBCRGAINCTRL);
	uTemp = (uTemp&~(0xFF<<0)) | (uCbGain<<0);
	Outp32(CBCRGAINCTRL, uTemp);
}


//////////
// Function Name : TVENC_GetCbGain
// Function Description : Get the current Cb gain value
// Input : 	None
// Output : 	the current Cb gain value
u32 TVENC_GetCbGain(void)
{
	u32 uTemp;

	uTemp = Inp32(CBCRGAINCTRL);
	uTemp = (uTemp&(0xFF<<0));
	return uTemp;
}


//////////
// Function Name : TVENC_SetCrGain
// Function Description : Set the Cr Gain
// Input : 	uCrGain - Cr gain value
// Output : 	None
void TVENC_SetCrGain(u32 uCrGain)
{
	u32 uTemp;

	uTemp = Inp32(CBCRGAINCTRL);
	uTemp = (uTemp&~(0xFF<<16)) | (uCrGain<<16);
	Outp32(CBCRGAINCTRL, uTemp);
}


//////////
// Function Name : TVENC_GetCrGain
// Function Description : Get the current Cr gain value
// Input : 	None
// Output : 	the current Cr gain value
u32 TVENC_GetCrGain(void)
{
	u32 uTemp;

	uTemp = Inp32(CBCRGAINCTRL);
	uTemp = (uTemp&(0xFF<<16))>>16;
	return uTemp;
}


//////////
// Function Name : TVENC_EnableGammaControl
// Function Description : Enable/Disable Gamma control
// Input : 	bEnable - Enable or Disable
// Output : 	None
void TVENC_EnableGammaControl(u8 bEnable)
{
	u32 uTemp;

	uTemp = Inp32(GAMMACTRL);
	
	if(bEnable == TRUE)
		uTemp |= (1<<12);
	else
		uTemp &= ~(1<<12);
		
	Outp32(GAMMACTRL, uTemp);
}


//////////
// Function Name : TVENC_SetGammaGain
// Function Description : Set the Gamma Gain
// Input : 	uGamma - Gamma gain value
// Output : 	None
void TVENC_SetGammaGain(u32 uGamma)
{
	u32 uTemp;

	uTemp = Inp32(GAMMACTRL);
	uTemp = (uTemp&~(0x7<<8)) | (uGamma<<8);
	Outp32(GAMMACTRL, uTemp);
}


//////////
// Function Name : TVENC_GetGammaGain
// Function Description : Get the current Gamma gain value
// Input : 	None
// Output : 	the current Gamma gain value
u32 TVENC_GetGammaGain(void)
{
	u32 uTemp;

	uTemp = Inp32(GAMMACTRL);
	uTemp = (uTemp&(0x7<<8))>>8;
	return uTemp;
}


//////////
// Function Name : TVENC_EnableMuteControl
// Function Description : Enable/Disable the Video Mute control
// Input : 	bEnable - Enable or Disable
// Output : 	None
void TVENC_EnableMuteControl(u8 bEnable)
{
	u32 uTemp;

	uTemp = Inp32(MUTECTRL);
	
	if(bEnable == FALSE)
		uTemp |= (1<<0);
	else
		uTemp &= ~(1<<0);
		
	Outp32(MUTECTRL, uTemp);
}


//////////
// Function Name : TVENC_SetMuteYCbCr
// Function Description : Set the Y, Cb, Cr Mute component
// Input : 	uY - Mute Y component
//			uCb - Mute Cb component
//			uCr -Mute Cr component
// Output : 	None
void TVENC_SetMuteYCbCr(u32 uY, u32 uCb, u32 uCr)
{
	u32 uTemp;

	uTemp = Inp32(MUTECTRL);
	uTemp = (uTemp&~(0xFFFFFF<<8)) | ((uCr&0xFF)<<24) | ((uCb&0xFF)<<16) | ((uY&0xFF)<<8);
	Outp32(MUTECTRL, uTemp);
}


//////////
// Function Name : TVENC_GetMuteYCbCr
// Function Description : Get the Y, Cb, Cr Mute component
// Input : 	Output variable address
// Output : 	uY - Mute Y component
//			uCb - Mute Cb component
//			uCr -Mute Cr component
void TVENC_GetMuteYCbCr(u32 *uY, u32 *uCb, u32 *uCr)
{
	u32 uTemp;

	uTemp = Inp32(MUTECTRL);
	*uY= (uTemp&(0xFF<<8))>>8;
	*uCb = (uTemp&(0xFF<<16))>>16;
	*uCr = (uTemp&((u32)(0xFF<<24)))>>24;
}


//////////
// Function Name : TVENC_GetActiveWinCenter
// Function Description : Get the Active window center position
// Input : 	Output variable address
// Output : 	uVer - Vertical active window center position
//			uHor - Horizontal active window center position
void TVENC_GetActiveWinCenter(u32 *uVer, u32 *uHor)
{
	u32 uTemp;

	uTemp = Inp32(HENHOFFSET);
	*uVer= (uTemp&(0x3F<<24))>>24;
	*uHor = (uTemp&(0xFF<<16))>>16;
}


//////////
// Function Name : TVENC_SetActiveWinCenter
// Function Description : Set the Active window center position
// Input : 	uVer - Vertical active window center position
//			uHor - Horizontal active window center position
// Output : 	None
void TVENC_SetActiveWinCenter(u32 uVer, u32 uHor)
{
	u32 uTemp;

	uTemp = Inp32(HENHOFFSET);
	uTemp = (uTemp&~(0x3FFF<<16)) | ((uVer&0x3F)<<24) | ((uHor&0xFF)<<16);
	Outp32(HENHOFFSET, uTemp);
}


//////////
// Function Name : TVENC_EnableEnhancerDemoWindow
// Function Description : Enable/Disable enhancer demo window
// Input : 	bEnable - Enable or Disable
// Output : 	None
void TVENC_EnableEnhancerDemoWindow(u8 bEnable)
{
	u32 uTemp;

	uTemp = Inp32(DEMOWINCTRL);
	
	if(bEnable == TRUE)
		uTemp |= (1<<24);
	else
		uTemp &= ~(1<<24);
		
	Outp32(DEMOWINCTRL, uTemp);	
}


//////////
// Function Name : TVENC_SetDemoWinSize
// Function Description : Get the enhancing demo window size
// Input : 	Output variable address
// Output : 	uVWinSize - Vertical demo window size
//			uVStart - Vertical demo window start point
//			uHWinSize - Horizontal demo window size
//			uHStart - Horizontal demo window start point
void TVENC_GetEnhancerDemoWindow(u32 *uVWinSize, u32 *uVStart, u32 *uHWinSize, u32 *uHStart)
{
	u32 uTemp;

	uTemp = Inp32(VDEMOWSIZE);
	*uVWinSize = (uTemp&(0x1FF<<16))>>16;
	*uVStart = uTemp&0x1FF;
	
	uTemp = Inp32(HDEMOWSIZE);
	*uHWinSize = (uTemp&(0x7FF<<16))>>16;
	*uHStart = uTemp&0x7FF;	
}


//////////
// Function Name : TVENC_SetEnhancerDemoWindow
// Function Description : Set the enhancing demo window size
// Input : 	uVWinSize - Vertical demo window size
//			uVStart - Vertical demo window start point
//			uHWinSize - Horizontal demo window size
//			uHStart - Horizontal demo window start point
// Output : 	None
void TVENC_SetEnhancerDemoWindow(u32 uVWinSize, u32 uVStart, u32 uHWinSize, u32 uHStart)
{
	u32 uTemp;

	uTemp = ((uVWinSize&0x1FF)<<16) | (uVStart&0x1FF);
	Outp32(VDEMOWSIZE, uTemp);

	uTemp = ((uHWinSize&0x7FF)<<16) | (uHStart&0x7FF);
	Outp32(HDEMOWSIZE, uTemp);	
}


//////////
// Function Name : TVENC_EnableBackground
// Function Description : Enable/Disable background control
// Input : 	bEnable - Enable or Disable
// Output : 	None
void TVENC_EnableBackground(u8 bEnable)
{
	u32 uTemp;

	uTemp = Inp32(BGCTRL);
	if(bEnable == TRUE)
		uTemp |= (1<<8);
	else
		uTemp &= ~(1<<8);
	Outp32(BGCTRL, uTemp);

	uTemp = Inp32(ENCCTRL);
	if(bEnable == TRUE)
		uTemp |= (1<<0);
	else
		uTemp &= ~(1<<0);
	Outp32(ENCCTRL, uTemp);
}


//////////
// Function Name : TVENC_GetBackground
// Function Description : Get the background VAV & HAV
// Input : 	Output variable address
// Output : 	uColor - Background color select
//			uHStart - Background HAV Start Position
//			uVStart - Background VAV Start Position
//			uHVisualSize - Background HAV Length
//			uVVisualSize - Background VAV Length
void TVENC_GetBackground(u32 *uColor, u32 *uHStart, u32 *uVStart, u32 *uHVisualSize, u32 *uVVisualSize)
{
	u32 uTemp;

	uTemp = Inp32(BGCTRL);
	*uColor = (uTemp&(0x07<<4))>>4;

	uTemp = Inp32(BGHVAVCTRL);
	*uHStart = (uTemp&(0xFF<<16))>>16;      
	*uVStart = (uTemp&(0xFF<<0))>>0;  
	*uHVisualSize = (uTemp&((u32)(0xFF<<24)))>>24;
	*uVVisualSize = (uTemp&(0xFF<<8))>>8;
}


//////////
// Function Name : TVENC_SetBackground
// Function Description : Set the background VAV & HAV
// Input : 	uColor - Background color select
//			uHStart - Background HAV Start Position
//			uVStart - Background VAV Start Position
//			uHVisualSize - Background HAV Length
//			uVVisualSize - Background VAV Length
// Output : 	None
void TVENC_SetBackground(u32 uColor, u32 uHStart, u32 uVStart, u32 uHVisualSize, u32 uVVisualSize)
{
	u32 uTemp;

//	Inp32(BGCTRL, uTemp);
//	uTemp = (uTemp & ~(0x7<<4)) | ((uColor&0x07)<<4);
//	Outp32(BGCTRL, uTemp);

	TVENC_SetBackGround(TRUE, uColor, 0);
	
	uTemp = ((uHVisualSize&0xFF)<<24) | ((uHStart&0xFF)<<16) | ((uVVisualSize&0xFF)<<8) | (uVStart&0xFF);
	Outp32(BGHVAVCTRL, uTemp);
}


//////////
// Function Name : TVENC_SetSharpness
// Function Description : Set the Sharpness gain
// Input : 	uSharpness - Sharpness gain
// Output : 	None
void TVENC_SetSharpness(u32 uSharpness)
{
	u32 uTemp;

	uTemp = Inp32(SHARPCTRL);
	uTemp = (uTemp&~(0x3F)) | (uSharpness&0x3f);
	Outp32(SHARPCTRL, uTemp);
}


//////////
// Function Name : TVENC_GetSharpness
// Function Description : Get the current Sharpness gain
// Input : 	None
// Output : 	uSharpness - Sharpness gain
u32 TVENC_GetSharpness(void)
{
	u32 uTemp;

	uTemp = Inp32(SHARPCTRL);
	uTemp = uTemp&0x3F;
	return uTemp;
}

void __irq Isr_TVFifoUnderrun(void)
{
	UART_Printf("@Isr_FifoUnderrun\n");

	TVENC_ClearUnderrunInt();
	INTC_ClearVectAddr();
}


void TVENC_DisplayTVout(TV_STANDARDS eTvmode, eTV_CONN_TYPE eTvout, u32 uSizeX, u32 uSizeY)
{
	TVENC_SetTvConMode(eTvmode, eTvout);
	
	INTC_Enable(NUM_TVENC);
	INTC_SetVectAddr(NUM_TVENC, Isr_TVFifoUnderrun);

	TVENC_SetImageSize(uSizeX*2, uSizeY);
	TVENC_TurnOnOff(1);
}



