/********************************************************************************
* 
*	Project Name : S3C6410 Validation
*
*	Copyright 2006 by Samsung Electronics, Inc.
*	All rights reserved.
*
*	Project Description :
*		This software is only for verifying functions of the S3C6410.
*		Anybody can use this software without our permission.
*  
*--------------------------------------------------------------------------------
* 
*	File Name : lcd.h
*  
*	File Description :
*
*	Author	: Yoh-Han Lee
*	Dept. : AP Development Team
*	Created Date : 2006/12/27
*	Version : 0.2 
* 
*	History
*	- Created (2006/12/27)
*   - Release Ver 0.1 (2007/03/29)
*   - Release Ver 0.2 (2007/06/07)
********************************************************************************/
 
#ifndef __LCD_H__
#define __LCD_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "def.h"
#include "sfr6410.h"
#include "library.h"
#include "gpio.h"
#include "Option.h"

/////////////////////////////////////////////////////
// LCD Module Type 

#define LCD_MODULE_TYPE			LTE480WV_RGB

#define LTE480WV_RGB			0
#define LTV350QV_RGB			1
#define LTS222QV_CPU			2
#define LTS222QV_SRGB			3
#define LTP700WV_RGB			4
#define LTV300GV_RGB			5
#define LMS480KC01_RGB			6
#define LTS222QV_PRGB			7

/////////////////////////////////////////////////////
// I80 CPU I/F CS

#define LCD_CPUIF_CS			CS_MAIN
#define CS_MAIN					0
#define CS_SUB					1

/////////////////////////////////////////////////////
// LCD Clock Source Selection

#define LCD_CLOCK_SOURCE		LCLK_HCLK	

#define LCLK_HCLK				1
#define LCLK_MOUT_EPLL			2
#define LCLK_DOUT_MPLL			3
#define LCLK_FIN_EPLL			4
#define LCLK_27M				5

/////////////////////////////////////////////////////
// LCD Frame Rate
#define LCD_TU_Out_TEST 	0
#define LCD_FRM_RATE     		60	//LCD Frame Rate (Hz)

/////////////////////////////////////////////////////
// LCD Frame Buffer Start Address  

#define LCD_FRAMEBUFFER_ST   	0x52000000


/////////////////////////////////////////////////////
// LCD Special Function Registers 

enum LCD_SFR
{
	//Video Control
	rVIDCON0		= 0x000,
	rVIDCON1		= 0x004,
	rVIDCON2		= 0x008,	
	//Video Time Control 
	rVIDTCON0       = 0x010,
	rVIDTCON1       = 0x014,
	rVIDTCON2       = 0x018,
	rVIDTCON3       = 0x01c,
	//Window Control
	rWINCON0	    = 0x020,
	rWINCON1        = 0x024,
	rWINCON2	    = 0x028,
	rWINCON3	    = 0x02C,
	rWINCON4	    = 0x030,
	//Video Window Position Control
	rVIDOSD0A       = 0x040,
	rVIDOSD0B       = 0x044,
	rVIDOSD0C       = 0x048,
	rVIDOSD1A       = 0x050,
	rVIDOSD1B       = 0x054,
	rVIDOSD1C       = 0x058,
	rVIDOSD1D       = 0x05C,	
	rVIDOSD2A       = 0x060,
	rVIDOSD2B       = 0x064,
	rVIDOSD2C       = 0x068,
	rVIDOSD2D       = 0x06c,	
	rVIDOSD3A       = 0x070,
	rVIDOSD3B       = 0x074,
	rVIDOSD3C       = 0x078,
	rVIDOSD4A       = 0x080,
	rVIDOSD4B       = 0x084,
	rVIDOSD4C       = 0x088,
	//Window Buffer Start Address
	rVIDW00ADD0B0   = 0x0A0,
	rVIDW00ADD0B1   = 0x0A4,
	rVIDW01ADD0B0   = 0x0A8,
	rVIDW01ADD0B1   = 0x0AC,
	rVIDW02ADD0     = 0x0B0,
	rVIDW03ADD0     = 0x0B8,
	rVIDW04ADD0     = 0x0C0,
	//Window Buffer End Address
	rVIDW00ADD1B0   = 0x0D0,
	rVIDW00ADD1B1   = 0x0D4,
	rVIDW01ADD1B0   = 0x0D8,
	rVIDW01ADD1B1   = 0x0DC,
	rVIDW02ADD1     = 0x0E0,
	rVIDW03ADD1     = 0x0E8,
	rVIDW04ADD1     = 0x0F0,
       //Window Buffer Size
	rVIDW00ADD2     = 0x100,
	rVIDW01ADD2     = 0x104,
	rVIDW02ADD2     = 0x108,
	rVIDW03ADD2     = 0x10C,
	rVIDW04ADD2     = 0x110,
	//Indicate the Video Interrupt Control
	rVIDINTCON0     = 0x130,	
	//Video Interrupt Pending 
	rVIDINTCON1     = 0x134,	
	//Color Key Control/Value 
	rW1KEYCON0      = 0x140,
	rW1KEYCON1      = 0x144,
	rW2KEYCON0      = 0x148,
	rW2KEYCON1      = 0x14C,
	rW3KEYCON0      = 0x150,
	rW3KEYCON1      = 0x154,
	rW4KEYCON0      = 0x158,
	rW4KEYCON1      = 0x15C,
	//DithMode
	rDITHMODE       = 0x170,
       //Window Control
	rWIN0MAP        = 0x180,
	rWIN1MAP        = 0x184,
	rWIN2MAP        = 0x188,
	rWIN3MAP        = 0x18C,
	rWIN4MAP        = 0x190,
	//Window Palette Control
	rWPALCON        = 0x1A0,
	//I80/RGB Trigger Control
	rTRIGCON        = 0x1A4,
	rITUIFCON0	   = 0x1A8,
	
    //I80 Interface Control for Main/Sub LDI 
	rI80IFCONA0     = 0x1B0,
	rI80IFCONA1     = 0x1B4,
	rI80IFCONB0     = 0x1B8,
	rI80IFCONB1     = 0x1BC,
	//I80 Interface LDI Command Cotrol
	rLDI_CMDCON0    = 0x1D0,
	rLDI_CMDCON1    = 0x1D4,
	//I80 Interface System Command Control
	rSIFCCON0       = 0x1E0,
	rSIFCCON1       = 0x1E4,
	rSIFCCON2       = 0x1E8,
	//I80 Interface LDI Command
	rLDI_CMD0       = 0x280,
	rLDI_CMD1       = 0x284,
	rLDI_CMD2       = 0x288,
	rLDI_CMD3       = 0x28C,
	rLDI_CMD4       = 0x290,
	rLDI_CMD5       = 0x294,
	rLDI_CMD6       = 0x298,
	rLDI_CMD7       = 0x29C,
	rLDI_CMD8       = 0x2A0,
	rLDI_CMD9       = 0x2A4,
	rLDI_CMD10      = 0x2A8,
	rLDI_CMD11      = 0x2AC,
	//Window Palette Data
	rW2PDATA01      = 0x300,
	rW2PDATA23      = 0x304,
	rW2PDATA45      = 0x308,
	rW2PDATA67      = 0x30C,
	rW2PDATA89      = 0x310,
	rW2PDATAAB      = 0x314,
	rW2PDATACD      = 0x318,
	rW2PDATAEF      = 0x31C,
	rW3PDATA01      = 0x320,
	rW3PDATA23      = 0x324,
	rW3PDATA45      = 0x328,
	rW3PDATA67      = 0x32C,
	rW3PDATA89      = 0x330,
	rW3PDATAAB      = 0x334,
	rW3PDATACD      = 0x338,
	rW3PDATAEF      = 0x33C,
	rW4PDATA01      = 0x340,
	rW4PDATA23      = 0x344,
	//Win0 Palette Ram Access Address(not SFR)
	W0PRAMSTART    = 0x400,
	//Win1 Palette Ram Access Address(not SFR)
	W1PRAMSTART    = 0x800
};

/////////////////////////////////////////////////////
// LCD SFR Write  

#define LcdcOutp32(offset, x) Outp32(LCD_BASE+offset, x)
#define LcdcOutp16(offset, x)  Outp16(LCD_BASE+offset, x)
#define LcdcInp32(offset, x) Inp32(LCD_BASE+offset, x)
#if 0 // SMDK 
#define LCD_DCLK_Lo		GPIO_SetDataEach(eGPIO_C, eGPIO_5 , 0); //GPC[5]:SPI_SCLK -> Low
#define LCD_DCLK_Hi		GPIO_SetDataEach(eGPIO_C, eGPIO_5 , 1); //GPC[5]:SPI_SCLK -> High
#define LCD_DSERI_Lo	GPIO_SetDataEach(eGPIO_C, eGPIO_6 , 0); //GPC[6]:SPI_MOSI -> Low
#define LCD_DSERI_Hi	GPIO_SetDataEach(eGPIO_C, eGPIO_6 , 1); //GPC[6]:SPI_MOSI -> High 
#define LCD_DEN_Lo		GPIO_SetDataEach(eGPIO_C, eGPIO_7 , 0); //GPC[7]:SPI_CS -> Low 
#define LCD_DEN_Hi		GPIO_SetDataEach(eGPIO_C, eGPIO_7 , 1); //GPC[7]:SPI_CS -> High 
#else // Integreted board. test
#define LCD_DCLK_Lo		GPIO_SetDataEach(eGPIO_C, eGPIO_1 , 0); //GPC[5]:SPI_SCLK -> Low
#define LCD_DCLK_Hi		GPIO_SetDataEach(eGPIO_C, eGPIO_1 , 1); //GPC[5]:SPI_SCLK -> High
#define LCD_DSERI_Lo	GPIO_SetDataEach(eGPIO_C, eGPIO_2 , 0); //GPC[6]:SPI_MOSI -> Low
#define LCD_DSERI_Hi	GPIO_SetDataEach(eGPIO_C, eGPIO_2 , 1); //GPC[6]:SPI_MOSI -> High 
#define LCD_DEN_Lo		GPIO_SetDataEach(eGPIO_C, eGPIO_3 , 0); //GPC[7]:SPI_CS -> Low 
#define LCD_DEN_Hi		GPIO_SetDataEach(eGPIO_C, eGPIO_3 , 1); //GPC[7]:SPI_CS -> High 
#endif


// VIDCON0
// MSB or LSB Out in RGB Seiral I/F
#define MSB_OUT_IN_SERIAL       (0<<30)
#define LSB_OUT_IN_SERIAL       (1<<30)

#define INTERLACE               (1<<29)
#define PROGRESSIVE             (0<<29)

#define VIDOUT_RGBIF            (0<<26)
#define VIDOUT_TV               (1<<26)
#define VIDOUT_I80IF0           (2<<26)
#define VIDOUT_I80IF1           (3<<26)
#define VIDOUT_TVNRGBIF         (4<<26)
#define VIDOUT_TVNI80IF0        (6<<26)
#define VIDOUT_TVNI80IF1        (7<<26)

#define PNRMODE_RGB_P           (0<<17)
#define PNRMODE_BGR_P	        (1<<17)
#define PNRMODE_RGB_S        	(2<<17)
#define PNRMODE_BGR_S	        (3<<17)
#define PNRMODE_MASK            (3<<17)

#define CLKVALUP_ALWAYS         (0<<16)
#define CLKVALUP_ST_FRM         (1<<16)

#define CLKVAL_F(n)             (((n)&0xFF)<<6)

#define VCLKFREE_NORMAL          (0<<5)

#define VCLKEN_DISABLE          (1<<5)
#define VCLKEN_ENABLE           (0<<5)

#define CLKDIR_F(n)             (((n)&0x1)<<4)
#define CLKDIR_DIRECT           (0<<4)
#define CLKDIR_DIVIDED          (1<<4)
#define CLKDIR_MASK             (1<<4)

#define CLKSEL_F_HCLK           (0<<2)
#define CLKSEL_F_SYSCON         (1<<2)
#define CLKSEL_F_EXTCLK         (3<<2)

#define ENVID_DISABLE           (0<<1)
#define ENVID_F_DISABLE         (0<<0)

// CPU Interface Data Format (MAIN LDI)
#define MAIN_16_MODE            (0<<20)
#define MAIN_16PLUS2_MODE       (1<<20)
#define MAIN_9PLUS9_MODE        (2<<20)
#define MAIN_16PLUS8_MODE       (3<<20)
#define MAIN_18_MODE            (4<<20)
#define MAIN_8PLUS8_MODE		(5<<20)

// CPU Interface Data Format (SUB LDI)
#define SUB_16_MODE             (0<<23)
#define SUB_16PLUS2_MODE        (1<<23)
#define SUB_9PLUS9_MODE         (2<<23)
#define SUB_16PLUS8_MODE        (3<<23)
#define SUB_18_MODE             (4<<23)
#define SUB_8PLUS8_MODE			(5<<23)

// Main Control1 register (VIDCON1)
#define IVCLK_FALL_EDGE         (0<<7)
#define IVCLK_RISE_EDGE         (1<<7)
#define IHSYNC_NORMAL           (0<<6)
#define IHSYNC_INVERT           (1<<6)
#define IVSYNC_NORMAL           (0<<5)
#define IVSYNC_INVERT           (1<<5)
#define IVDEN_NORMAL            (0<<4)
#define IVDEN_INVERT            (1<<4)

// Time Control0 register (VIDTCON0)
#define VBPDE(n)                (((n)&0xFF)<<24)
#define VBPD(n)	                (((n)&0xFF)<<16)
#define VFPD(n)                 (((n)&0xFF)<<8)
#define VSPW(n)                 (((n)&0xFF)<<0)

// Time Control1 register (VIDTCON1)
#define VFPDE(n)                (((n)&0xFF)<<24)
#define HBPD(n)                 (((n)&0xFF)<<16)
#define HFPD(n)                 (((n)&0xFF)<<8)
#define HSPW(n)                 (((n)&0xFF)<<0)

// Time Control2 register (VIDTCON2)
#define LINEVAL(n)              (((n)&0x7FF)<<11)
#define HOZVAL(n)               (((n)&0x7FF)<<0)

// v3.2
#define RSADD4TO1(n)            (((n)&0xF)<<23)

// WINCON0 SFR for local path
#define W0DMA                   (0<<22)
#define W0LOCAL_POST            (1<<22)
#define W0ENLOCAL_MASK          (1<<22)

// WINCON1 SFR for local path
#define W1DMA                   (0<<22)
#define W1LOCAL                 (1<<22)
#define W1ENLOCAL_MASK          (1<<22)
#define W1ENLOCAL_CIM           (1<<23)
#define W1ENLOCAL_POST          (0<<23)
#define W1LOCAL_CIM_MASK        (1<<23)

#define W0BUF0                  (0<<20)
#define W0BUF1                  (1<<20)
#define W0BUF_MASK              (1<<20)
#define W0BUFAUTO_DISABLE       (0<<19)
#define W0BUFAUTO_ENABLE        (1<<19)
#define W0BUFAUTO_MASK          (1<<19)
#define IN_LOCAL_RGB            (0<<13)
#define IN_LOCAL_YUV            (1<<13)
#define INRGB_MASK              (1<<13)

// WINCON1,2,3,4 SFR's
#define BITSWP_DISABLE          (0<<18)
#define BITSWP_ENABLE           (1<<18)
#define BYTSWP_DISABLE          (0<<17)
#define BYTSWP_ENABLE           (1<<17)
#define HAWSWP_DISABLE          (0<<16)
#define HAWSWP_ENABLE           (1<<16)

#define MAX_BURSTLEN_16WORD     (0<<9)
#define MAX_BURSTLEN_8WORD      (1<<9)
#define MAX_BURSTLEN_4WORD      (2<<9)

#define MIN_BURSTLEN_16WORD     (0<<11)
#define MIN_BURSTLEN_8WORD      (1<<11)
#define MIN_BURSTLEN_4WORD      (2<<11)

#define BLD_PIX_PLANE           (0<<6)
#define BLD_PIX_PIXEL           (1<<6)
#define BLD_PIX_MASK            (1<<6)

#define BPPMODE_F_1BPP          (0<<2)
#define BPPMODE_F_2BPP          (1<<2)
#define BPPMODE_F_4BPP          (2<<2)
#define BPPMODE_F_8BPP_PAL      (3<<2)
#define BPPMODE_F_8BPP_NOPAL    (4<<2)
#define BPPMODE_F_16BPP_565     (5<<2)
#define BPPMODE_F_16BPP_A555    (6<<2)
#define BPPMODE_F_18BPP_666     (8<<2)
#define BPPMODE_F_24BPP_888     (11<<2)
#define BPPMODE_F_24BPP_A887    (0xc<<2)
#define BPPMODE_F_MASK          (0xf<<2)

// BLEND_SELECT_MODE
#define BLEND_ALPHA0_PLANE      (0<<1)
#define BLEND_ALPHA1_PLANE      (1<<1)

#define BLEND_AEN_PIXEL         (0<<1)
#define BLEND_DATA_PIXEL        (1<<1)

#define ALPHA_SEL_0             ~(1<<1)

#define BLEND_COLORKEY_AREA     (0<<1)

#define BLEND_SEL_MASK          (1<<1)

#define ENWIN_F_DISABLE         (0<<0)
#define ENWIN_F_ENABLE          (1<<0)

#define COLOR_MAP_ENABLE        (1<<24)

// WINDOW POSITION CONTROL A REGISTER
#define OSD_LTX_F(n)            (((n)&0x7FF)<<11)
#define OSD_LTY_F(n)            (((n)&0x7FF)<<0)

// WINDOW POSITION CONTROL B REGISTER
#define OSD_RBX_F(n)            (((n)&0x7FF)<<11)
#define OSD_RBY_F(n)            (((n)&0x7FF)<<0)

// WINDOW0 POSITION CONTROL C REGISTER
#define OSDEN_DISABLE           (0<<24)
#define OSDEN_ENABLE            (1<<24)
#define OSDSIZE(n)              (((n)&0xFFFFFF)<<0)

// WINDOW1,2,3,4 POSITION CONTROL C REGISTER
#define ALPHA0_R(n)             (((n)&0xF)<<20)
#define ALPHA0_G(n)             (((n)&0xF)<<16)
#define ALPHA0_B(n)             (((n)&0xF)<<12)
#define ALPHA1_R(n)             (((n)&0xF)<<8)
#define ALPHA1_G(n)             (((n)&0xF)<<4)
#define ALPHA1_B(n)             (((n)&0xF)<<0)

// Color Key Control Register (W1KEYCON0)
#if 1
#define KEYBLEN_DISABLE         (0<<26)
#else
#define KEYBLEN_DISABLE         ~(1<<26)
#endif
#define KEYBLEN_ENABLE          (1<<26)
#define KEYEN_F_DISABLE         ~(1<<25)
#define KEYEN_F_ENABLE          (1<<25)
#define DIRCON_MATCH_FG_IMAGE   (0<<24)
#define DIRCON_MATCH_BG_IMAGE   (1<<24)

// LCD INTERRUPT CONTROL REGISTER (VIDINTCON)
#define SYSMAINCON_DISABLE      (0<<19)
#define SYSMAINCON_ENABLE       (1<<19)
#define SYSSUBCON_DISABLE       (0<<18)
#define SYSSUBCON_ENABLE        (1<<18)
#define SYSIFDONE_DISABLE       (0<<17)
#define SYSIFDONE_ENABLE        (1<<17)
#define FRAMESEL0_BACK          (0<<15)
#define FRAMESEL0_VSYNC         (1<<15)
#define FRAMESEL0_ACTIVE        (2<<15)
#define FRAMESEL0_FRONT         (3<<15)
#define FRAMESEL1_NONE          (0<<13)
#define FRAMESEL1_BACK          (1<<13)
#define FRAMESEL1_VSYNC         (2<<13)
#define FRAMESEL1_FRONT         (3<<13)
#define INTFRMEN_DISABLE        (0<<12)
#define INTFRMEN_ENABLE         (1<<12)
#define FRAMEINT_MASK           (0x1f<<12)
#define FIFOSEL_WIN4            (1<<11)
#define FIFOSEL_WIN3            (1<<10)
#define FIFOSEL_WIN2            (1<<9)
#define FIFOSEL_WIN1            (1<<6)
#define FIFOSEL_WIN0            (1<<5)
#define FIFOSEL_ALL             (FIFOSEL_WIN0 | FIFOSEL_WIN1 | FIFOSEL_WIN2 | FIFOSEL_WIN3 | FIFOSEL_WIN4)
#define FIFOLEVEL_25            (0<<2)
#define FIFOLEVEL_50            (1<<2)
#define FIFOLEVEL_75            (2<<2)
#define FIFOLEVEL_EMPTY         (3<<2)
#define FIFOLEVEL_FULL          (4<<2)
#define INTFIFOEN_DISABLE       (0<<1)
#define INTFIFOEN_ENABLE        (1<<1)
#define INTEN_DISABLE           (0<<0)
#define INTEN_ENABLE            (1<<0)
#define INTEN_MASK              (1<<0)

#define INTI80PEND_CLEAR        (1<<2)
#define INTFRMPEND_CLEAR        (1<<1)
#define INTFIFOPEND_CLEAR       (1<<0)

// DITHERING CONTROL 1 REGISTER (DITHMODE)
#define RDITHPOS_8BIT           (0<<5)
#define RDITHPOS_6BIT           (1<<5)
#define RDITHPOS_5BIT           (2<<5)
#define GDITHPOS_8BIT           (0<<3)
#define GDITHPOS_6BIT           (1<<3)
#define GDITHPOS_5BIT           (2<<3)
#define BDITHPOS_8BIT           (0<<1)
#define BDITHPOS_6BIT           (1<<1)
#define BDITHPOS_5BIT           (2<<1)
#define RGB_DITHPOS_MASK        (0x3f<<1)
#define DITHERING_DISABLE       (0<<0)
#define DITHERING_ENABLE        (1<<0)
#define DITHERING_MASK          (1<<0)

// Main LCD System Interface Control
#define LCD_CS_SETUP(n)         (((n)&0xF)<<16)
#define LCD_WR_SETUP(n)         (((n)&0xF)<<12)
#define LCD_WR_ACT(n)           (((n)&0xF)<<8)
#define LCD_WR_HOLD(n)	        (((n)&0xF)<<4)
#define RSPOL_LOW               (0<<2)
#define RSPOL_HIGH              (1<<2)
#define SUCCEUP_ONETIME         (0<<1)
#define SUCCEUP_TRIGGERED       (1<<1)
#define SYSIFEN_DISABLE         (0<<0)
#define SYSIFEN_ENABLE          (1<<0)

// added for V3.2
#define RSADD_MSB_HIGH          (1<<2)
#define RSADD_MSB_LOW           (0<<2)

#define VIDEO_STOP              (0)
#define VIDEO_START             (1)

#define WINDOW_STOP             (0)
#define WINDOW_START            (1)

//Timing parameter for LTV350QV(SAMSUNG)
#define VBPD_LTV350QV           ((5-1)&0xff)
#define VFPD_LTV350QV           ((3-1)&0xff)
#define VSPW_LTV350QV           ((4-1)&0xff)
#define HBPD_LTV350QV           ((5-1)&0xff)
#define HFPD_LTV350QV           ((3-1)&0xff)			
#define HSPW_LTV350QV           ((10-1)&0xff)

//Timing parameter for LTP700WV(SAMSUNG)
#if 1 // real daa
#define VBPD_LTP700WV           ((7-1)&0xff)
#define VFPD_LTP700WV           ((5-1)&0xff)
#define VSPW_LTP700WV           ((1-1)&0xff)
#define HBPD_LTP700WV           ((13-1)&0xff)
#define HFPD_LTP700WV           ((8-1)&0xff)			
#define HSPW_LTP700WV           ((3-1)&0xff)
#else // ITU test
#define VBPD_LTP700WV           ((2-1)&0xff)
#define VFPD_LTP700WV           ((2-1)&0xff)
#define VSPW_LTP700WV           ((2-1)&0xff)
#define HBPD_LTP700WV           ((2-1)&0xff)
#define HFPD_LTP700WV           ((2-1)&0xff)			
#define HSPW_LTP700WV           ((2-1)&0xff)
#endif


//Timing parameter for LMS480CQ(SAMSUNG)
#define VBPD_LMS480CQ           ((21-1)&0xff) 
#define VFPD_LMS480CQ           ((8-1)&0xff)
#define VSPW_LMS480CQ           ((0)&0xff)
#define HBPD_LMS480CQ           ((104-1)&0xff)
#define HFPD_LMS480CQ           ((10-1)&0xff)			
#define HSPW_LMS480CQ           ((0)&0xff)


//Timing parameter for LTS222Q
#define VBPD_LTS222QV           ((7-1)&0xff) 
#define VFPD_LTS222QV           ((10-1)&0xff)
#define VSPW_LTS222QV           ((3-1)&0xff)
#define HBPD_LTS222QV           ((2-1)&0xff) 
#define HFPD_LTS222QV           ((2-1)&0xff)			
#define HSPW_LTS222QV           ((1-1)&0xff)



//Timing parameter for LTS222Q
#define VBPD_LTS222QVP           ((10-1)&0xff) 
#define VFPD_LTS222QVP           ((11-1)&0xff)
#define VSPW_LTS222QVP           ((4-1)&0xff)
#define HBPD_LTS222QVP           ((2-1)&0xff) 
#define HFPD_LTS222QVP           ((7-1)&0xff)			
#define HSPW_LTS222QVP           ((4-1)&0xff)



//Timing parameter for LTS_222Q I80 I/F
#if 0 
#define CS_SETUP_TIME           (0&0xf)
#define WR_SETUP_TIME           (1&0xf)
#define WR_ACT_TIME             (6&0xf)
#define WR_HOLD_TIME            (0&0xf)
#else
#define CS_SETUP_TIME           (0xf&0xf)
#define WR_SETUP_TIME          (0xf&0xf)
#define WR_ACT_TIME              (0xf&0xf)
#define WR_HOLD_TIME            (0xf&0xf)
#endif


//Timing parameter for LTV300GV(SAMSUNG)
#define VBPD_LTV300GV           ((26-1)&0xff)
#define VFPD_LTV300GV           ((18-1)&0xff)
#define VSPW_LTV300GV           ((1-1)&0xff)
#define HBPD_LTV300GV           ((135-1)&0xff)
#define HFPD_LTV300GV           ((24-1)&0xff)			
#define HSPW_LTV300GV           ((1-1)&0xff)

// RGB macro will be used to V3.3
// RGB Writer (WINCONw-0x0014c)
#define ONETIME_WRITE           (0<<22)
#define ENABLE_RGBWR            (1<<0)
#define DISBALE_RGBWR           (0<<0)

// CPU I/F Masking
#define LCD_OUT_MASKING         ~(0X3<<26)
#define MAIN_DATA_MASKING       ~(0X7<<20)
#define SUB_DATA_MASKING        ~(0X7<<23)

// Trigger // In V4.0, offset 1B8, 1BC
#define SW_TRIG_MODE            (0X1<<0)
#define SW_TRIGGER              (0x1<<1)

#define H_CLK_INPUT             (0x0<<2)
#define EXT_CLK_0_INPUT         (0X1<<2)
#define EXT_CLK_1_INPUT         (0x3<<2)

// RGB I/F Mode
#define RGB_PARALLEL_MODE       (0x0<<17)
#define BGR_PARALLEL_MODE       (0x1<<17)
#define RGB_SERIAL_MODE         (0x2<<17)
#define BGR_SERIAL_MODE         (0x3<<17)


/////////////////////////////////////////////////////
//typedef

typedef enum 
{
	MAIN, SUB
} CPUIF_LDI;

typedef enum 
{
	DISABLE_AUTO_FRM,
	PER_TWO_FRM,
	PER_FOUR_FRM,
	PER_SIX_FRM,	
	PER_EIGHT_FRM,
	PER_TEN_FRM,
	PER_TWELVE_FRM,	
	PER_FOURTEEN_FRM,
	PER_SIXTEEN_FRM,
	PER_EIGHTEEN_FRM,
	PER_TWENTY_FRM,
	PER_TWENTY_TWO_FRM,
	PER_TWENTY_FOUR_FRM,
	PER_TWENTY_SIX_FRM,
	PER_TWENTY_EIGHT_FRM,
	PER_THIRTY_FRM
} CPU_AUTO_CMD_RATE;

typedef enum 
{
	LCD_DISABLE_CMD, LCD_NORMAL_CMD, LCD_AUTO_CMD, LCD_NORMAL_N_AUTO_CMD, LCD_MANUAL_CMD
} CPU_COMMAND_MODE;

typedef enum 
{
	CPU_16BIT, CPU_16_2BIT, CPU_9_9BIT, CPU_16_8BIT, CPU_18BIT, CPU_8_8BIT
} CPU_OUTPUT_DATAFORMAT;

typedef enum 
{
	WIN0, WIN1, WIN2, WIN3, WIN4
}LCD_WINDOW;

#if 0
typedef enum
{
	PAL1, PAL2, PAL4, PAL8,
	RGB8, ARGB8, RGB16, ARGB16, RGB18, RGB24, RGB30, ARGB24,
	YC420, YC422, // Non-interleave
	CRYCBY, CBYCRY, YCRYCB, YCBYCR, YUV444 // Interleave
} CSPACE;
#endif

typedef enum 
{
	MATCH_FG_IMAGE, MATCH_BG_IMAGE
} COLOR_KEY_DIRECTION;

typedef enum 
{
	PER_PLANE, PER_PIXEL
} BLENDING_APPLIED_UNIT;

typedef enum 
{
	ALPHA0_PLANE, ALPHA1_PLANE, DATA_PIXEL, AEN_PIXEL, COLORKEY_AREA
} BLENDING_SELECT_MODE;

typedef enum 
{
	IN_POST, IN_CIM
} LCD_LOCAL_INPUT;

typedef enum
{
	LOCALIN_RGB,
	LOCALIN_YCbCr
} LOCAL_INPUT_COLORSPACE;

typedef enum
{
	LCD_RGB, LCD_TV, LCD_I80F, LCD_I80S,
	LCD_TVRGB, LCD_TVI80F, LCD_TVI80S
} LCD_LOCAL_OUTPUT;

typedef enum
{
	SRC_HCLK, SRC_MOUT_EPLL, SRC_DOUT_MPLL, SRC_FIN_EPLL, SRC_27M
} CLK_SRC;

typedef enum
{
	HCLK_SRC, ECLK0_SRC, ECLK1_SRC
} CLK_SRC1;	//This is added for code compatibility with other Multimedia IP for example TV Scaler. 07/05/10

typedef enum
{
	MOUT_EPLL, DOUT_MPLL, FIN_EPLL
} CLK_SRC_FROMSYSCON;

typedef enum
{
	PROGRESSIVE_MODE, INTERLACE_MODE
} LCD_SCAN_MODE;

typedef enum 
{
	LCD_FIFO_INT, LCD_FRAME_INT, LCD_SYSIF_INT
}LCD_INT;

typedef enum 
{
	LCD_SYSMAIN_INT,   LCD_SYSSUB_INT
}LCD_INT_SYS;

typedef enum 
{
	FIFO_25,  FIFO_50, FIFO_75, FIFO_EMPTY, FIFO_FULL
}FIFO_INT_LEVEL;

typedef enum 
{
	FIFO_INT_WIN0, FIFO_INT_WIN1, FIFO_INT_WIN2, FIFO_INT_WIN3, FIFO_INT_WIN4
}FIFO_INT_SEL;

typedef enum 
{
	FINT_BACKPORCH, FINT_VSYNC, FINT_ACTIVE, FINT_FRONTPORCH
}FRAME_INT_SEL;

typedef enum 
{
	SWAP_BIT_DISABLE, SWAP_BIT_ENABLE, 
	SWAP_BYTE_DISABLE, SWAP_BYTE_ENABLE, 
	SWAP_HALFWORD_DISABLE, SWAP_HALFWORD_ENABLE
} SWAP_MODE;

typedef enum 
{
	BURST_16WORD, BURST_8WORD, BURST_4WORD
} BURST_LENGTH;

typedef enum
{
	PNR_Parallel_RGB, PNR_Parallel_BGR, PNR_Serial_RGB, PNR_Serial_BGR
}PNR_MODE;

typedef enum
{
	DITHER_5BIT, DITHER_6BIT, DITHER_8BIT
}DITHER_BIT;

typedef enum
{
	CLK_DIRECT, CLK_DIVIDED
}CLK_DIR_DIV;

///////////////////////////////////////////////////////////////
typedef struct
{
	u32 m_uImgStAddr[5];
	u32 m_uDoubleBufImgStAddr[2];
	u32 m_uDoubleBufStAddr[2];
	u32 m_uDoubleBufEndAddr[2];

	u32 m_uFbStAddr[5];
	u32 m_uFbEndAddr[5];
	u32 m_uFrmHSz[5];
	u32 m_uFrmVSz[5];
	u32 m_uViewHSz[5];
	u32 m_uViewVSz[5];

	u32 m_uPageWidth[5];
	u32 m_uOffsetSz[5];
	u32 m_uLcdHSz;
	u32 m_uLcdVSz;
	u32 m_uLcdStX;
	u32 m_uLcdStY;
	u32 m_uBppMode;
	u32 m_uPnrMode;
	u32 m_uBytes;
	u32 m_uBits;
	u32 m_uBytSwpMode;
	u32 m_uHawSwpMode;
	u32 m_uMaxBurstLen;
	u32 m_uMinBurstLen;
	u32 m_uVidOutFormat;
	u32 m_uDitherMode;
	u8 m_bIsAutoBuf;
	u8 m_bIsNormalCall;
	u32 m_uScanMode;
	u32 m_uVideoClockSource;
	u32 m_uI80MainOutDataFormat;
	u32 m_uI80SubOutDataFormat;
	u32 m_bIsDelayVsync;
	
	u8 m_bIsLocalCalled[3];
	LCD_LOCAL_INPUT m_eLocalIn[3];
	LOCAL_INPUT_COLORSPACE m_uLocalInColorSpace[3];
	
}LCDC;


#if 0 //For LCD SFR W/R Test
REGINFO		sLcdRegInfo[] = 
{ 

	{"			", LCD_BASE+0x00, 32, RW, DPDB, 0, 0},   
	{"			", LCD_BASE+0x04, 32, RW, DPDB, 0, 0},
	{"			", LCD_BASE+0x08, 32, RW, DPDB, 0, 0},
	{"			", LCD_BASE+0x10, 32, RW, DPDB, 0, 0},   
	{"			", LCD_BASE+0x14, 32, RW, DPDB, 0, 0},
	{"			", LCD_BASE+0x18, 32, RW, DPDB, 0, 0},
	{"			", LCD_BASE+0x1C, 32, RW, DPDB, 0, 0},   
	{"			", LCD_BASE+0x20, 32, RW, DPDB, 0, 0},
	{"			", LCD_BASE+0x24, 32, RW, DPDB, 0, 0},
	{"			", LCD_BASE+0x28, 32, RW, DPDB, 0, 0},   
	{"			", LCD_BASE+0x2C, 32, RW, DPDB, 0, 0},
	{"			", LCD_BASE+0x30, 32, RW, DPDB, 0, 0},
	{"			", LCD_BASE+0x40, 32, RW, DPDB, 0, 0},   
	{"			", LCD_BASE+0x44, 32, RW, DPDB, 0, 0},
	{"			", LCD_BASE+0x48, 32, RW, DPDB, 0, 0},
	{"			", LCD_BASE+0x50, 32, RW, DPDB, 0, 0},   
	{"			", LCD_BASE+0x54, 32, RW, DPDB, 0, 0},
	{"			", LCD_BASE+0x58, 32, RW, DPDB, 0, 0},
	{"			", LCD_BASE+0x5C, 32, RW, DPDB, 0, 0},   
	{"			", LCD_BASE+0x60, 32, RW, DPDB, 0, 0},
	{"			", LCD_BASE+0x64, 32, RW, DPDB, 0, 0},
	{"			", LCD_BASE+0x68, 32, RW, DPDB, 0, 0},   
	{"			", LCD_BASE+0x70, 32, RW, DPDB, 0, 0},
	{"			", LCD_BASE+0x74, 32, RW, DPDB, 0, 0},
	{"			", LCD_BASE+0x78, 32, RW, DPDB, 0, 0},   
	{"			", LCD_BASE+0x80, 32, RW, DPDB, 0, 0},
	{"			", LCD_BASE+0x84, 32, RW, DPDB, 0, 0},
	{"			", LCD_BASE+0x88, 32, RW, DPDB, 0, 0},   
	{"			", LCD_BASE+0xA0, 32, RW, DPDB, 0, 0},
	{"			", LCD_BASE+0xA4, 32, RW, DPDB, 0, 0},
	{"			", LCD_BASE+0xA8, 32, RW, DPDB, 0, 0},   
	{"			", LCD_BASE+0xAC, 32, RW, DPDB, 0, 0},
	{"			", LCD_BASE+0xB0, 32, RW, DPDB, 0, 0},
	{"			", LCD_BASE+0xB8, 32, RW, DPDB, 0, 0},   
	{"			", LCD_BASE+0xC0, 32, RW, DPDB, 0, 0},
	{"			", LCD_BASE+0xD0, 32, RW, DPDB, 0, 0},
	{"			", LCD_BASE+0xD4, 32, RW, DPDB, 0, 0},   
	{"			", LCD_BASE+0xD8, 32, RW, DPDB, 0, 0},
	{"			", LCD_BASE+0xDC, 32, RW, DPDB, 0, 0},
	{"			", LCD_BASE+0xE0, 32, RW, DPDB, 0, 0},   
	{"			", LCD_BASE+0xE8, 32, RW, DPDB, 0, 0},
	{"			", LCD_BASE+0xF0, 32, RW, DPDB, 0, 0},
	{"			", LCD_BASE+0x100, 32, RW, DPDB, 0, 0},   
	{"			", LCD_BASE+0x104, 32, RW, DPDB, 0, 0},
	{"			", LCD_BASE+0x108, 32, RW, DPDB, 0, 0},
	{"			", LCD_BASE+0x10C, 32, RW, DPDB, 0, 0},   
	{"			", LCD_BASE+0x110, 32, RW, DPDB, 0, 0},
	{"			", LCD_BASE+0x130, 32, RW, DPDB, 0, 0},
	{"			", LCD_BASE+0x134, 32, RW, DPDB, 0, 0},   
	{"			", LCD_BASE+0x140, 32, RW, DPDB, 0, 0},
	{"			", LCD_BASE+0x148, 32, RW, DPDB, 0, 0},
	{"			", LCD_BASE+0x14C, 32, RW, DPDB, 0, 0},   
	{"			", LCD_BASE+0x150, 32, RW, DPDB, 0, 0},
	{"			", LCD_BASE+0x154, 32, RW, DPDB, 0, 0},
	{"			", LCD_BASE+0x158, 32, RW, DPDB, 0, 0},   
	{"			", LCD_BASE+0x15C, 32, RW, DPDB, 0, 0},
	{"			", LCD_BASE+0x170, 32, RW, DPDB, 0, 0},
	{"			", LCD_BASE+0x180, 32, RW, DPDB, 0, 0},   
	{"			", LCD_BASE+0x184, 32, RW, DPDB, 0, 0},
	{"			", LCD_BASE+0x188, 32, RW, DPDB, 0, 0},
	{"			", LCD_BASE+0x18C, 32, RW, DPDB, 0, 0},   
	{"			", LCD_BASE+0x190, 32, RW, DPDB, 0, 0},
	{"			", LCD_BASE+0x1A0, 32, RW, DPDB, 0, 0},
	{"			", LCD_BASE+0x1A4, 32, RW, DPDB, 0, 0},   
	{"			", LCD_BASE+0x1B0, 32, RW, DPDB, 0, 0},
	{"			", LCD_BASE+0x1B4, 32, RW, DPDB, 0, 0},
	{"			", LCD_BASE+0x1B8, 32, RW, DPDB, 0, 0},   
	{"			", LCD_BASE+0x1BC, 32, RW, DPDB, 0, 0},
	{"			", LCD_BASE+0x1D0, 32, RW, DPDB, 0, 0},
	{"			", LCD_BASE+0x1D4, 32, RW, DPDB, 0, 0},   
	{"			", LCD_BASE+0x1E0, 32, RW, DPDB, 0, 0},
	{"			", LCD_BASE+0x1E4, 32, RW, DPDB, 0, 0},
	{"			", LCD_BASE+0x1E8, 32, RW, DPDB, 0, 0},   
	{"			", LCD_BASE+0x280, 32, RW, DPDB, 0, 0},
	{"			", LCD_BASE+0x284, 32, RW, DPDB, 0, 0},
	{"			", LCD_BASE+0x288, 32, RW, DPDB, 0, 0},   
	{"			", LCD_BASE+0x28C, 32, RW, DPDB, 0, 0},
	{"			", LCD_BASE+0x290, 32, RW, DPDB, 0, 0},
	{"			", LCD_BASE+0x294, 32, RW, DPDB, 0, 0},   
	{"			", LCD_BASE+0x298, 32, RW, DPDB, 0, 0},
	{"			", LCD_BASE+0x29C, 32, RW, DPDB, 0, 0},	
	{"			", LCD_BASE+0x2A0, 32, RW, DPDB, 0, 0},   
	{"			", LCD_BASE+0x2A4, 32, RW, DPDB, 0, 0},
	{"			", LCD_BASE+0x2A8, 32, RW, DPDB, 0, 0},
	{"			", LCD_BASE+0x2AC, 32, RW, DPDB, 0, 0},   
	{"			", LCD_BASE+0x300, 32, RW, DPDB, 0, 0},
	{"			", LCD_BASE+0x304, 32, RW, DPDB, 0, 0},
	{"			", LCD_BASE+0x308, 32, RW, DPDB, 0, 0},   
	{"			", LCD_BASE+0x30C, 32, RW, DPDB, 0, 0},
	{"			", LCD_BASE+0x310, 32, RW, DPDB, 0, 0},
	{"			", LCD_BASE+0x314, 32, RW, DPDB, 0, 0},   
	{"			", LCD_BASE+0x318, 32, RW, DPDB, 0, 0},
	{"			", LCD_BASE+0x31C, 32, RW, DPDB, 0, 0},
	{"			", LCD_BASE+0x320, 32, RW, DPDB, 0, 0},   
	{"			", LCD_BASE+0x324, 32, RW, DPDB, 0, 0},
	{"			", LCD_BASE+0x328, 32, RW, DPDB, 0, 0},
	{"			", LCD_BASE+0x32C, 32, RW, DPDB, 0, 0},
	{"			", LCD_BASE+0x330, 32, RW, DPDB, 0, 0},   
	{"			", LCD_BASE+0x334, 32, RW, DPDB, 0, 0},
	{"			", LCD_BASE+0x338, 32, RW, DPDB, 0, 0},
	{"			", LCD_BASE+0x33C, 32, RW, DPDB, 0, 0},   
	{"			", LCD_BASE+0x340, 32, RW, DPDB, 0, 0},
	{"			", LCD_BASE+0x344, 32, RW, DPDB, 0, 0},
};
#endif


/////////////////////////////////////////////////////
//LCD API Functions

// Top Layer
void LCD_SimpleBasicDisp(void);

void LCD_SetPort(void);
void LCD_SetPort_ByPass(void);

void LCD_InitLDI(CPUIF_LDI eLdiMode);

void LCD_InitDISPC(CSPACE eBpp, u32 uFbAddr, LCD_WINDOW eWin, u8 bIsDoubleBuf);
void LCD_InitDISPC1(u32 uHSize, u32 uVSize, CSPACE eBpp, u32 uFbAddr, LCD_WINDOW eWin, u8 bIsDoubleBuf);
void LCD_GetFrmSz(u32* uFrmHSz, u32* uFrmVSz, LCD_WINDOW eWin);
void LCD_Start(void);
void LCD_Stop(void);
void LCD_PerFrameOff(void);

// Middle Layer
void LCD_InitBase(void);
void LCD_InitBase1(u32 uLcdX, u32 uLcdY);
void LCD_InitBase2(CLK_SRC eVclkSrc, PNR_MODE ePnrMode, CPU_OUTPUT_DATAFORMAT eCpuOutDataFormat);

void LCD_InitWin(CSPACE eBpp, u32 uFrameH, u32 uFrameV, u32 uX_Frame, u32 uY_Frame,  u32 uViewH, u32 uViewV,
		u32 uX_Lcd, u32 uY_Lcd, u32 uFbAddr, LCD_WINDOW eWin, u8 bIsDoubleBuf);

void LCD_InitDISPCForFifoIn(LCD_LOCAL_INPUT eInLocal, LCD_WINDOW eWin);

void LCD_GetClkValAndClkDir(u32 uLcdHSz, u32 uLcdVSz, u32* uClkVal, u32* uClkDir, CLK_SRC eClkSrc);

void LCD_GetFbEndAddr(u32* uFbAddr, LCD_WINDOW eWin); 
void LCD_GetDoubleBufAddr(u32* uFbAddr, u32 uWhichAddr, LCD_WINDOW eWin);
void LCD_EnableAutoBuf(u32 uMode, LCD_WINDOW eWin);
void LCD_GetBufIdx(u32* uBufNum, LCD_WINDOW eWin);

void LCD_InitLTV350QV_RGB(void);
void LCD_InitLTP700WV_RGB(void);
void LCD_ExitLTP700WV_RGB(void);
void LCD_SetI80Timing(
	u8 ucCsSetupTime, u8 ucWrSetupTime, u8 ucWrActTime, u8 ucWrHoldTime,
	CPUIF_LDI eLdi);
void LCD_SetAutoCmdRate(CPU_AUTO_CMD_RATE eCmdRate, CPUIF_LDI eLdi);
void LCD_InitLTS222QV_CPU(CPUIF_LDI eLdiMode);
void LCD_InitLTS222QV_SRGB(void);
void LCD_InitStartPosOnLcd(CPUIF_LDI eLdiMode);
void LCD_SetViewPosOnLcd(u32 uX_Lcd, u32 uY_Lcd, LCD_WINDOW eWin);
void LCD_SetViewPosOnFrame(u32 uX_Frame, u32 uY_Frame, LCD_WINDOW eWin);
void LCD_SetActiveFrameBuf(u32 uWhichBuf, LCD_WINDOW eWin);

void LCD_SetColorMapOnOff(u32 uOnOff, LCD_WINDOW eWin);
void LCD_SetColorMapValue(u32 uVal, LCD_WINDOW eWin);

void LCD_PutCmdToLdi_ManualCmd(u32 uData[], CPUIF_LDI eLdiMode);

void LCD_Trigger(void);

void LCD_InitWinRegs(LCD_WINDOW eWin);

void LCD_InitWinForFifoIn(LCD_LOCAL_INPUT eLocalIn, LCD_WINDOW eWin);
void LCD_InitWinForFifoIn1(LCD_LOCAL_INPUT eLocalIn, LCD_WINDOW eWin, LOCAL_INPUT_COLORSPACE eLocalInSCS);

void LCD_SetWinOnOff(u32 uOnOff, LCD_WINDOW eWin);
void LCD_SetAllWinOnOff(u32 uOnOff);
void LCD_SetAllWinColorMapOnOff(u32 uOnOff);
void LCD_SetAllWinColorKeyOnOff(u32 uOnOff);
void LCD_SetAlpha(u8 ucA0R, u8 ucA0G, u8 ucA0B, u8 ucA1R, u8 ucA1G, u8 ucA1B, LCD_WINDOW eWin);

void LCD_SetColorKeyOnOff(u32 uOnOff, LCD_WINDOW eWin);
void LCD_SetColorKey(u32 uMaskKey, u32 uColorKey, COLOR_KEY_DIRECTION eDir, u8 bIsBlending, LCD_WINDOW eWin);
void LCD_SetBlendingType(BLENDING_APPLIED_UNIT eBldPix, BLENDING_SELECT_MODE eBldMode, LCD_WINDOW eWin);

u8 LCD_CheckPaletteRAM(LCD_WINDOW eWin);
void LCD_SetPaletteCtrl(CSPACE eBpp, LCD_WINDOW eWin);
void LCD_ChangePaletteClk(u32 uWhichClk);
void LCD_SetPaletteData(u32 uIdx, u32 uPalData, LCD_WINDOW eWin);
void LCD_SetByteSwap(SWAP_MODE eBitSwap, SWAP_MODE eByteSwap, SWAP_MODE eHawSwap, LCD_WINDOW eWin);
void LCD_SetBurstLength(BURST_LENGTH eBurstLength, LCD_WINDOW eWin);

void LCD_SetScanMode(LCD_SCAN_MODE eScanMode);
void LCD_SetOutputPath(LCD_LOCAL_OUTPUT eOutLocal);
void LCD_SetClkSrc(CLK_SRC eClkSrc);
void LCD_SetClkSrc1(CLK_SRC eClkSrc); //This is added for code compatibility with other Multimedia IP, for example TV Scaler. 07/05/10

void LCD_SelClkSrcFromSYSCON(CLK_SRC_FROMSYSCON eClkSrcFromSyscon); //This is added for code compatibility with other Multimedia IP, for example TV Scaler. 07/05/10
void LCD_SetClkVal(u8 uClkVal);
void LCD_SetClkDirOrDiv(CLK_DIR_DIV eClkDirDiv);

void LCD_SetCpuOutputFormat(CPU_OUTPUT_DATAFORMAT eCpuOutDataFormat);

void LCD_EnableRgbPort(void);
void LCD_DisableRgbPort(void);

void LCD_DisplayPartialArea(
	u32 uSrcStX, u32 uSrcStY, u32 uDstStX, u32 uDstStY,
	u32 uPartialHSz, u32 uPartialVsz, u32 uSrcFbStAddr,
	LCD_WINDOW eWin);

void LCD_EnableDithering(u32 uEnable, DITHER_BIT eRedDitherBit, DITHER_BIT eGreenDitherBit, DITHER_BIT eBlueDitherBit);

void LCD_InitInt(LCD_INT eLcdInt, LCD_INT_SYS eLcdIntSys, FIFO_INT_LEVEL eFifoIntLevel, FIFO_INT_SEL eFifoIntSel, FRAME_INT_SEL eFIntSel );
void LCD_EnableInt(void);
void LCD_DisableInt(void);
void LCD_ClearInt(LCD_INT eLcdInt);

// Bottom Layer
void LCD_PutCmdtoLTV350QV(u16 addr, u16 data);
void LCD_SpiSendByte(u16 uData);
void delayLoop(u32 count);

void LCD_PutCmdToLdi_NormalCmd(u32 uData[], u32 uDataNum, CPUIF_LDI eLdiMode);
void LCD_PutDataOrCmdToLdi(u32 uData[], u32 uDataNum,
	CPUIF_LDI eLdiMode, CPU_COMMAND_MODE eCmdMode, u8 IsCmd);
void LCD_PutDataOrCmdToLdi_SelCmdMode(u32 uData[], u32 uDataNum, u32 uModeSel[],
	CPUIF_LDI eLdiMode, u8 IsCmd, u8 IsNormalCmdStart);
void LCD_PutDataToLdi(u16 uData, CPUIF_LDI eLdiMode);
void LCD_PutCmdToLdi_AutoCmd(u32 uData[], u32 uDataNum, CPUIF_LDI eLdiMode);
void LCD_GetDataFromLdi(u32* uData, CPUIF_LDI eLdiMode);
void LCD_GetCmdFromLdi(u32* uCmd, CPUIF_LDI eLdiMode);

void LCD_WriteCmd(CPUIF_LDI eLdiMode, u16 uAddr, u16 uData);

u8 LCD_IsFrameDone(void);
void ConvertCSpaceToString(CSPACE eBpp, char* pcBpp);

 void __irq Isr_LCD_FrameCount(void);

void LCD_InitLMS480KCO_RGB(void);
void LCD_PutCmdtoLMS480KCO(u16 addr, u16 data);

void LCD_InitWinForFifoIn2(LCD_LOCAL_INPUT eLocalIn, LCD_WINDOW eWin, LOCAL_INPUT_COLORSPACE eLocalInSCS, u32 uLcdHsize, u32 uLcdVsize);


void LCD_InitBt601And656If(u32 uHsz, u32 uVsz)	;
void LCD_SetBt601And656CtrlClk(u32 bIsVsyncDelayed, u32 bIs656clkInverted, u32 bIs601HrefInverted, u32 bIs601VsyncInverted,
								 u32 bIsHsyncInverted, u32 bIs601FieldInverted, u32 bIs601ClkInverted);

void LCD_SetBT601VsyncDly(u32 DelayCycle);


void LCD_SetInterlace(void);
void LCD_SetITUInterface(void);
void LCD_SetITUInterface_656(void);


void LCD_InitLTS222QV_PRGB(void);




#ifdef __cplusplus
}
#endif

#endif //__LCD_H__

