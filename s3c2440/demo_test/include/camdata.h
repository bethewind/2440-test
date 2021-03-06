/*****************************************
  NAME: camdata.h
  DESC: data for initializing camera module
  HISTORY:  2004.01.16:draft ver 0.0
 *****************************************/

#ifndef __CAMDATA_H__
#define __CAMDATA_H__

#ifdef __cplusplus
extern "C"{
#endif

#include "Camdef.h"

#define	CAM_ACT_HSIZE	(640)	// Max 640
#define	CAM_ACT_VSIZE	(480)	// Max 480
#define	CAM_STX		(0x2f)	//47
#define	CAM_STY		(6)		//6
#define	CAM_ENDX		(((CAM_ACT_HSIZE)/4) + CAM_STX)
#define	CAM_ENDY		((CAM_ACT_VSIZE/2) + CAM_STY-1)


const unsigned char Ov7620_Yuv16bit[][2] = {

	{0x12, 0x80},			// Camera Soft reset. Self cleared after reset.
	{0x17, CAM_STX},		//
	{0x18, CAM_ENDX},	// (207-47)*4 = 640
	{0x19, CAM_STY},		//
	{0x1a, CAM_ENDY},	// ((244-5)+1)*2=480

	{0x01, 0x80},		// set blue gain
	{0x02, 0x80},		// set red gain
	{0x03, 0xb0},
	{0x06, 0x70},		// set brightness
	{0x07, 0x00},
	{0x0c, 0x24},		// set blue background
	{0x0d, 0x24},		// set red background
	{0x10, 0xff},		// set exposure time, brightness control
	{0x11, 0x40},		// set frame rate 27Mhz/[(0+1)*2]=13.5MHz
	{0x12, 0x24},		// set YUV mode, enable AGC/AWB, mirror image.
	{0x15, 0x01},		// Use PCLK rising edge to latch data
	{0x16, 0x03},		//
	{0x12, 0x64},		// set YUV mode, enable AGC/AWB, mirror image.
	{0x13, 0x01},		// YUV 16-bit format, Disable AEC/AGC/AWB
	{0x14, 0x84},		// turn off GAMMA
	{0x24, 0x3a},		//
	{0x25, 0x60},		//
	{0x28, 0x20},		// Progressive mode.
	{0x2d, 0x95},		//
	{0x67, 0x92},		//
	{0x74, 0x00}		//
};


const unsigned char Ov7620_YCbCr8bit[][2] = {

	{0x12, 0x80},		// Camera Soft reset. Self cleared after reset.
	{0x00, 0x00},
	{0x01, 0x80},		// set blue gain
	{0x02, 0x80},		// set red gain
	{0x03, 0xb0},
	{0x06, 0x60},		// set brightness
	{0x0c, 0x24},		// set blue background
	{0x0d, 0x24},		// set red background
	{0x10, 0xff},		// set exposure time, brightness control
	{0x11, 0x80},		// set frame rate CLK_input = PCLK
	{0x13, 0x21},		// set 8 Bit YUV mode	
	{0x14, 0x94}, 
	{0x16, 0x03},		//
	{0x17, 0x2f},		//
	{0x18, 0xcf},	        // (207-47)*4 = 640
	{0x19, 0x06},		//
	{0x1a, 0xf5},	        // ((244-5)+1)*2=480
	{0x1b, 0x00},
	{0x20, 0x00},
	{0x21, 0x80},
	{0x22, 0x80},
	{0x23, 0x00},
	{0x26, 0xa2},
	{0x27, 0xea},
	{0x29, 0x00},
	{0x2a, 0x00},
	{0x2b, 0x00},
	{0x2c, 0x88},
	{0x2e, 0x80},
	{0x2f, 0x44},
	{0x60, 0x27},
	{0x61, 0x82},
	{0x62, 0x5f},
	{0x63, 0xd5},
	{0x64, 0x57},
	{0x65, 0x83},
	{0x66, 0x55},
	{0x68, 0xcf},
	{0x69, 0x76},
	{0x6a, 0x22},
	{0x6b, 0x00},
	{0x6c, 0x08},
	{0x6d, 0x48},	
	{0x6e, 0x80},
	{0x6f, 0x0c},
	{0x70, 0x89},
	{0x71, 0x00},
	{0x72, 0x14},
	{0x73, 0x54},	
	{0x75, 0x0e},
	{0x76, 0x00},
	{0x77, 0xff},
	{0x78, 0x80},
	{0x79, 0x80},
	{0x7a, 0x80},
	{0x7b, 0xe6},
	{0x7c, 0x00},	
	{0x24, 0x10},
	{0x25, 0x8a},
	{0x28, 0x20},		
	{0x2d, 0x95},
	{0x67, 0x92},
	{0x74, 0x00},

	{0x15, 0x41},		// inversed PCLK : falling edge to get data, 8 Bit -U-V- sequence....
	{0x12, 0x34}, 	// enable AGC/AWB, mirror image. YUYV-must set at last..
};

const unsigned char Au70h[][2] = {

    {0x01,0x0b}, /*0x04*/ //5x5 interpolation VGA mode
#if AU70H_VIDEO_SIZE==640
	{0x08,0x00},
	{0x09,0xc0},
	{0x0a,0x01},
	{0x0b,0x00},
	{0x0c,0x01},
	{0x0d,0xe0},
	{0x0e,0x02},
	{0x0f,0x80},
#elif AU70H_VIDEO_SIZE==1152
	{0x08,0x00},
	{0x09,0x02},
	{0x0a,0x00},
	{0x0b,0x02},
	{0x0c,0x03},
	{0x0d,0x60},
	{0x0e,0x04},
	{0x0f,0x80},
#endif
	{0x18,0x01},
	{0x19,0x3f},
	{0x1a,0x13},
	{0x1b,0x7f},
	{0x1c,0x08},
	{0x20,0x13},
	{0x21,0x01},
	{0x22,0x01},
	{0x23,0x01},
	{0x27,0xf0},
	{0x30,0xfe},
	{0x31,0x31}, //YCbCr 8bit 0x31,0x11
	{0x32,0x00},
	{0x33,0x03},
	{0x67,0xa5},  //???
	{0x68,0x2c},
	{0x69,0x2a},
	{0x73,0x79},
	{0x75,0x03},
	{0x76,0x08},
	{0x77,0x35},
	{0x81,0x19},
	{0x90,0x01},
	{0x91,0x06},
	{0x92,0x1a},
	{0x93,0x80},

	{0x6a,0x01},
	{0x6b,0x38},
	{0x6c,0x80},
	
	{0x6d,0x09},
	{0x6e,0xc4},
	{0x6f,0x00},
	
	{0x60,0x99},
	
	{0x63,0x03},
	{0x64,0xa9},
	{0x65,0x80},
	
	{0x66,0x30},
	{0x70,0x44},
	{0x71,0x00},
	
	//{0x14,0x8},
	//{0x16,0x8},
	
	{0x70,0x45},
	{0x71,0x02},
	{0x66,0x70}
};

const unsigned char S5X532_YCbCr8bit[][2] = 
{ 
	// page 5
	{0xec,0x05},
	{0x08,0x55},
	{0x0a,0x75},
	{0x0c,0x90},
	{0x0e,0x18},
	{0x12,0x09},
	{0x14,0x9d},
	{0x16,0x90},
	{0x1a,0x18},
	{0x1c,0x0c},
	{0x1e,0x09},
	{0x20,0x06},
	{0x22,0x20},
	{0x2a,0x00},
	{0x2d,0x04},
	{0x12,0x24},
	// page 3
	{0xec,0x03},
	{0x0c,0x09},
	{0x6c,0x09},
	{0x2b,0x10}, // momo clock inversion
	// page 2
	{0xec,0x02},
	{0x03,0x09},
	{0x05,0x08},
	{0x06,0x01},
	{0x07,0xf8},
	{0x15,0x25},
	{0x30,0x29},
	{0x36,0x12},
	{0x38,0x04},
	{0x1b,0x77}, // 24MHz : 0x77, 12MHz : 0x22
	{0x1c,0x77}, // 24MHz : 0x77, 12MHz : 0x22
	// page 1
	{0xec,0x01},
	{0x00,0x03}, // 
	{0x0a,0x08}, // 0x0-QQVGA, 0x06-CIF, 0x02-QCIF, 0x08-VGA, 0x04-QVGA, 0x0a-SXGA
	{0x0c,0x00}, // Pattern selectio. 0-CIS, 1-Color bar, 2-Ramp, 3-Blue screen
	{0x10,0x27}, // 0x21-ITU-R656(CbYCrY), 0x25-ITU-R601(CbYCrY), 0x26-ITU-R601(YCrYCb)
	{0x50,0x21},		// Hblank
	{0x51,0x00},		// Hblank
	{0x52,0xA1},		// Hblank
	{0x53,0x02},		// Hblank
	{0x54,0x01},		// Vblank
	{0x55,0x00},		// Vblank
	{0x56,0xE1},		// Vblank 		
	{0x57,0x01},		// Vblank
	{0x58,0x21},		// Hsync
	{0x59,0x00},		// Hsync
	{0x5a,0xA1},		// Hsync
	{0x5b,0x02},		// Hsync
	{0x5c,0x03},		// Vref
	{0x5d,0x00},		// Vref
	{0x5e,0x05},		// Vref
	{0x5f,0x00},		// Vref	
	{0x70,0x0E},
	{0x71,0xD6},
	{0x72,0x30},
	{0x73,0xDB},
	{0x74,0x0E},
	{0x75,0xD6},
	{0x76,0x18},
	{0x77,0xF5},
	{0x78,0x0E},
	{0x79,0xD6},
	{0x7a,0x28},
	{0x7b,0xE6},
	{0x50,0x00},
	{0x5c,0x00},

	// page 0
	{0xec,0x00},
	{0x79,0x01},
	{0x58,0x90},
	{0x59,0xA0},
	{0x5a,0x50},
	{0x5b,0x70},
	{0x5c,0xD0},
	{0x5d,0xC0},
	{0x5e,0x28},
	{0x5f,0x08},
	{0x50,0x90},
	{0x51,0xA0},
	{0x52,0x50},
	{0x53,0x70},
	{0x54,0xD0},
	{0x55,0xC0},
	{0x56,0x28},
	{0x57,0x00},
	{0x48,0x90},
	{0x49,0xA0},
	{0x4a,0x50},
	{0x4b,0x70},
	{0x4c,0xD0},
	{0x4d,0xC0},
	{0x4e,0x28},
	{0x4f,0x08},
	{0x72,0x82}, // main clock = 24MHz:0xd2, 16M:0x82, 12M:0x54
	{0x75,0x05} // absolute vertical mirror.  junon

};

const unsigned char S5X3A1_YCbCr8bit[][2] = 
{ 
	// page 0
	{0xec,0x00},
//	{0x72,0xd2}, //main clock = 27MHz:0x00, 24MHz:0xd2, ...
	{0x01,0x00}, //SXGA
	// page 1
	{0xec,0x01},
	{0x0b,0x10}, // href type
	{0x5c,0x01}, // href type
	{0x5d,0x00}, // href type
	{0x5e,0x01}, // href type
	{0x5f,0x04}, // href type
	
	// page 0
	{0xec,0x00}
};

#ifdef __cplusplus
}
#endif

#endif /*__CAMDATA_H__*/

