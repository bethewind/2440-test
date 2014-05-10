/**
 * Samsung Project
 * Copyright (c) 2007 Mobile XG, Samsung Electronics, Inc.
 * All right reserved.
 *
 * This software is the confidential and proprietary information
 * of Samsung Electronics Inc. ("Confidential Information"). You
 * shall not disclose such Confidential Information and shall use
 * it only in accordance with the terms of the license agreement
 * you entered into with Samsung Electronics.
 */

/**
 * @file	register.h
 * @brief	This is the file that has definitions of the register map and etc.
 * @author	Cheolkyoo Kim
 * @version	1.5
 */

/**
 * 	@brief
 *		FIMGSE 3D Hardware Register Defines
 *		FG[GB]_xxxx : Global control/state
 *		FGHI_xxxx : Host Interface
 *		FGVS_xxxx : Vertex Shaders
 *		FGPE_xxxx : Primitive Engine (Clipping, Viewport-mapping, etc)
 *		FGRE_xxxx : Raster Engine (Setup, Rasterizer, etc)
 *		FGPS_xxxx : Pixel Shaders
 *		FGTU_xxxx : Texture Units
 *		FGPF_xxxx : Per-Fragment Units
 */
 
#if !defined(__FIMG_3DREGS_H__)
#define __FIMG_3DREGS_H__

#include "fglconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

#define FIMG_BASE   		0x72000000


/*****************************************************************************************
 **     REGISTER NAME      	OFFSET            R/W  DESCRIPTION              INITIAL VALUE
 *****************************************************************************************/

/*****************************************************************************
 *	GLOBAL REGISTER
 *****************************************************************************/
/* R  	The status of pipeline	*/
#define FG_PIPELINE_STATUS	    (FIMG_BASE+0x0)
/* R/W 	Cache control register  */
#define FG_CACHE_CTRL		    (FIMG_BASE+0x4)
/* W 	Hwrdware blocks reset	FFFF_FFFFh */
#define FG_SW_RESET			    (FIMG_BASE+0x8)
/* R/W Interrupt pendign register 0000_0000h */
#define FG_INT_PENDING		    (FIMG_BASE+0x40)
/* R/W Enables of Disables interrupts.	0000_0000h */
#define FG_INT_MASK			    (FIMG_BASE+0x44)
/* R/W Specifies the blocks in FIMG-3DSE
 * which are candidates to generate interrupts.
 * 0000_0000h */
#define FG_PIPELINE_MASK	    (FIMG_BASE+0x48)
/* R/W Specifies the value of pipeline-state
 * when interrupts are to occur.	0000_0000h */
#define FG_PIPELINE_TARGET_STATE (FIMG_BASE+0x4C)
/* R 	Captures the first pipeline-state when several interrupts occur.
 * 0000_0000h */
#define FG_PIPELINE_INT_STATE	(FIMG_BASE+0x50)

/* R  	The status of pipeline	*/
#define FGGB_PIPESTATE		(FIMG_BASE+0x0)
/* R/W 	Cache control register  */
#define FGGB_CACHECTL		(FIMG_BASE+0x4)
/* W 	Hwrdware blocks reset	FFFF_FFFFh*/
#define FGGB_RST			(FIMG_BASE+0x8)
/* R 	Hwrdware version */
#define FGGB_VERSION			(FIMG_BASE+0x10)
/* R/W Interrupt pendign register 0000_0000h*/
#define FGGB_INTPENDING		(FIMG_BASE+0x40)
/* R/W Enables of Disables interrupts.	0000_0000h */
#define FGGB_INTMASK		(FIMG_BASE+0x44)
/* R/W Specifies the blocks in FIMG-3DSE
 * which are candidates to generate interrupts.	0000_0000h */
#define FGGB_PIPEMASK		(FIMG_BASE+0x48)
/* R/W Specifies the value of pipeline-state when interrupts are to occur.
 * 0000_0000h */
#define FGGB_PIPETGTSTATE	(FIMG_BASE+0x4C)
/* R 	Captures the first pipeline-state when several interrupts occur.
 * 0000_0000h */
#define FGGB_PIPEINTSTATE	(FIMG_BASE+0x50)



/*-----------------------------------------------------------------------------
 	Cache Control ( Vertex Texture, Texture, Color, Depth Cache )
 -----------------------------------------------------------------------------*/
#define	rFIMG_CACHE				(*(volatile unsigned *)FG_CACHE_CTRL)
#define BIT_ZCACHE0_FLUSH		(0x1)   	/// Depth cache flush
#define BIT_ZCACHE1_FLUSH		(0x1<<1)
#define BIT_CCACHE0_FLUSH		(0x1<<4)	/// Color cache flush
#define BIT_CCACHE1_FLUSH		(0x1<<5)
#define BIT_TCACHE0_CLEAR		(0x1<<8)	/// Texture cache flush
#define BIT_TCACHE1_CLEAR		(0x1<<9)
#define BIT_VTCACHE_CLEAR		(0x1<<12)	/// Vertex cache flush
#define BIT_CACHE_ALLMASK		(0x3fff)

/*****************************************************************************
 *	HOST INTERFACE REGISTER
 *****************************************************************************/
/* R   The num of empty slots of FIFO */
#define FGHI_FIFO_EMPTY_SPACE		(FIMG_BASE+0x8000)

	/* W	  The input port of FIFO */
#define FGHI_FIFO_ENTRY			(FIMG_BASE+0xC000)


/* R/W Host interface control */
#define FGHI_HI_CTRL				(FIMG_BASE+0x8008)
/* R/W Index offset register */
#define FGHI_IDX_OFFSET				(FIMG_BASE+0x800C)
/* R/W Destination address register of attribute to copy in VB */
#define FGHI_VTXBUF_ADDR			(FIMG_BASE+0x8010)
/* W   Entry point register of VB */
#define FGHI_VTXBUF_ENTRY   	    (FIMG_BASE+0xE000)

/* R/W Input attribute N control reg */
#define FGHI_ATTR0              	(FIMG_BASE+0x8040)
#define FGHI_ATTR1              	(FIMG_BASE+0x8044)
#define FGHI_ATTR2              	(FIMG_BASE+0x8048)
#define FGHI_ATTR3              	(FIMG_BASE+0x804C)
#define FGHI_ATTR4              	(FIMG_BASE+0x8050)
#define FGHI_ATTR5              	(FIMG_BASE+0x8054)
#define FGHI_ATTR6              	(FIMG_BASE+0x8058)
#define FGHI_ATTR7              	(FIMG_BASE+0x805C)
#define FGHI_ATTR8              	(FIMG_BASE+0x8060)
#define FGHI_ATTR9              	(FIMG_BASE+0x8064)
#define FGHI_ATTRA              	(FIMG_BASE+0x8068)
#define FGHI_ATTRB              	(FIMG_BASE+0x806C)
#define FGHI_ATTRC              	(FIMG_BASE+0x8070)
#define FGHI_ATTRD              	(FIMG_BASE+0x8074)
#define FGHI_ATTRE              	(FIMG_BASE+0x8078)
#define FGHI_ATTRF              	(FIMG_BASE+0x807C)

/* R/W Vertex buffer control of input attribute N */
#define FGHI_VTXBUF_CTRL0          (FIMG_BASE+0x8080)
#define FGHI_VTXBUF_CTRL1          (FIMG_BASE+0x8084)
#define FGHI_VTXBUF_CTRL2          (FIMG_BASE+0x8088)
#define FGHI_VTXBUF_CTRL3          (FIMG_BASE+0x808C)
#define FGHI_VTXBUF_CTRL4          (FIMG_BASE+0x8090)
#define FGHI_VTXBUF_CTRL5          (FIMG_BASE+0x8094)
#define FGHI_VTXBUF_CTRL6          (FIMG_BASE+0x8098)
#define FGHI_VTXBUF_CTRL7          (FIMG_BASE+0x809C)
#define FGHI_VTXBUF_CTRL8          (FIMG_BASE+0x80A0)
#define FGHI_VTXBUF_CTRL9          (FIMG_BASE+0x80A4)
#define FGHI_VTXBUF_CTRLA          (FIMG_BASE+0x80A8)
#define FGHI_VTXBUF_CTRLB          (FIMG_BASE+0x80AC)
#define FGHI_VTXBUF_CTRLC          (FIMG_BASE+0x80B0)
#define FGHI_VTXBUF_CTRLD          (FIMG_BASE+0x80B4)
#define FGHI_VTXBUF_CTRLE          (FIMG_BASE+0x80B8)
#define FGHI_VTXBUF_CTRLF          (FIMG_BASE+0x80BC)

/* R/W Vertex buffer base address of input attribute N */
#define FGHI_VTXBUF_BASE0          (FIMG_BASE+0x80C0)
#define FGHI_VTXBUF_BASE1          (FIMG_BASE+0x80C4)
#define FGHI_VTXBUF_BASE2          (FIMG_BASE+0x80C8)
#define FGHI_VTXBUF_BASE3          (FIMG_BASE+0x80CC)
#define FGHI_VTXBUF_BASE4          (FIMG_BASE+0x80D0)
#define FGHI_VTXBUF_BASE5          (FIMG_BASE+0x80D4)
#define FGHI_VTXBUF_BASE6          (FIMG_BASE+0x80D8)
#define FGHI_VTXBUF_BASE7          (FIMG_BASE+0x80DC)
#define FGHI_VTXBUF_BASE8          (FIMG_BASE+0x80E0)
#define FGHI_VTXBUF_BASE9          (FIMG_BASE+0x80E4)
#define FGHI_VTXBUF_BASEA          (FIMG_BASE+0x80E8)
#define FGHI_VTXBUF_BASEB          (FIMG_BASE+0x80EC)
#define FGHI_VTXBUF_BASEC          (FIMG_BASE+0x80F0)
#define FGHI_VTXBUF_BASED          (FIMG_BASE+0x80F4)
#define FGHI_VTXBUF_BASEE          (FIMG_BASE+0x80F8)
#define FGHI_VTXBUF_BASEF          (FIMG_BASE+0x80FC)


/*****************************************************************************
 *	VERTEX SHADER REGISTER
 *****************************************************************************/
/// Configuration Register
/* W   Configuration register of the vertex shader */
#define FGVS_CONFIG				(FIMG_BASE+0x1C800)
/* R   Internal status register */
#define FGVS_STATUS				(FIMG_BASE+0x1C804)
/* R/W Start and end address of the vertex shader program */
#define FGVS_PC_RANGE			(FIMG_BASE+0x20000)
/* R/W The number of attributes for the input and output registers*/
#define FGVS_ATTRIB_NUM			(FIMG_BASE+0x20004)
/* R/W Index of input attributes 0~3 */
#define FGVS_IN_ATTRIB_IDX0		(FIMG_BASE+0x20008)
/* R/W Index of input attributes 4~7 */
#define FGVS_IN_ATTRIB_IDX1		(FIMG_BASE+0x2000C)
/* R/W Index of input attributes 8~11 */
#define FGVS_IN_ATTRIB_IDX2		(FIMG_BASE+0x20010)
/* R/W Index of output attributes 0~3 */
#define FGVS_OUT_ATTRIB_IDX0	(FIMG_BASE+0x20014)
/* R/W Index of output attributes 4~7 */
#define FGVS_OUT_ATTRIB_IDX1	(FIMG_BASE+0x20018)
/* R/W Index of output attributes 8~11 */
#define FGVS_OUT_ATTRIB_IDX2	(FIMG_BASE+0x2001C)

/// Address Map of Vertex Shader
/* R/W Reseved[16KB] Actual[16KB] VS's instruction */
#define FGVS_INSTMEM_SADDR		(FIMG_BASE+0x10000)
/* R/W register */
#define FGVS_INSTMEM_EADDR		(FIMG_BASE+0x11FFF)
/* R/W Reseved[16KB] Actual[4KB] VS's constant float */
#define FGVS_CFLOAT_SADDR 		(FIMG_BASE+0x14000)
/* R/W register */
#define FGVS_CFLOAT_EADDR 		(FIMG_BASE+0x14FFF)
/* R/W Reseved[ 1KB] Actual[256B] VS's constant integer */
#define FGVS_CINT_SADDR 		(FIMG_BASE+0x18000)
/* R/W register */
#define FGVS_CINT_EADDR 		(FIMG_BASE+0x1803F)
/* R/W Reseved[ 1KB] Actual[4B] VS's Boolean register */
#define FGVS_CBOOL_SADDR 		(FIMG_BASE+0x18400)
#define FGVS_CBOOL_EADDR 		(FIMG_BASE+0x18400)

/*-----------------------------------------------------------------------------
  VERTEX SHADER FIELD
 -----------------------------------------------------------------------------*/
#define VSIN_ATTRIB_NUM_MASK    0x000F0000
#define VSIN_ATTRIB_MAX_SIZE    10
#define VSIN_ATTRIB_MIN_SIZE    1
#define VSOUT_ATTRIB_NUM_MASK   0x0000000F
#define VSOUT_ATTRIB_MAX_SIZE   10
#define VSOUT_ATTRIB_MIN_SIZE   1

//#define FGVS_INSTMEM_SIZE		0x2000 	/* 8*1024(Byte) = 2048(WORD) = 0x800(Hex) */
//#define FGVS_CFLOAT_SIZE		0x1000 	/* 4*1024(Byte) = 1024(WORD) = 0x400(Hex) */
//#define FGVS_CINT_SIZE		0x40 	/* 256(Byte) = 64(WORD) = 0x40(Hex) */
//#define FGVS_CBOOL_SIZE		0x1 	/* 4(Byte) = 1(WORD) = 0x1(Hex) */

#define FGVS_INSTMEM_SIZE	0x7FF  /** 8*1024(Byte) = 2048(WORD) = 0x800(Hex) */
#define FGVS_CFLOAT_SIZE	0x3FF  /** 4*1024(Byte) = 1024(WORD) = 0x400(Hex) */
#define FGVS_CINT_SIZE		0xF    /** 256(Byte) = 64(WORD) = 0x40(Hex) */
#define FGVS_CBOOL_SIZE		0x1    /** 4(Byte) = 1(WORD) = 0x1(Hex) */

#define IN_ATTRIB_NUM_MASK			0x000F0000
#define OUT_ATTRIB_NUM_MASK			0x0000000F

/// configuration register
#define PROGRAM_COUNT_COPYOUT		1
/// Related to PC range register
#define PROGRAM_COUNT_END_SHIFT 	16
/// Related to index register of in/output attribute
#define ATTRIBUTE_NUM_SHIFT			16

#define FGSP_ATTRIBUTE_INDEX0		0
#define FGSP_ATTRIBUTE_INDEX1		1
#define FGSP_ATTRIBUTE_INDEX2		2
#define FGSP_ATTRIBUTE_INDEX3		3
#define FGSP_ATTRIBUTE_INDEX4		4
#define FGSP_ATTRIBUTE_INDEX5		5
#define FGSP_ATTRIBUTE_INDEX6		6
#define FGSP_ATTRIBUTE_INDEX7		7
#define FGSP_ATTRIBUTE_INDEX8		8
#define FGSP_ATTRIBUTE_INDEX9		9
#define FGSP_ATTRIBUTE_INDEXA		10
#define FGSP_ATTRIBUTE_INDEXB		11

#define FGSP_MAX_ATTRIBTBL_SIZE		12

#define FGSP_ATTRIB_IO_IDX_MASK     0xF
#define FGSP_ATTRIB_IO_IDX_SHIFT    0x8

/// Pre-defined Attributes for input/output semantic decleration
#define FGSP_ATRBDEF_POSITION 		0x10
#define FGSP_ATRBDEF_NORMAL   		0x20
#define FGSP_ATRBDEF_PCOLOR   		0x40
#define FGSP_ATRBDEF_SCOLOR   		0x41
#define FGSP_ATRBDEF_TEXTURE0 		0x80
#define FGSP_ATRBDEF_TEXTURE1 		0x81
#define FGSP_ATRBDEF_TEXTURE2 		0x82
#define FGSP_ATRBDEF_TEXTURE3 		0x83
#define FGSP_ATRBDEF_TEXTURE4 		0x84
#define FGSP_ATRBDEF_TEXTURE5 		0x85
#define FGSP_ATRBDEF_TEXTURE6 		0x86
#define FGSP_ATRBDEF_TEXTURE7 		0x87
#define FGSP_ATRBDEF_POINTSIZE		0x01
#define FGSP_ATRBDEF_USERDEF0 		0x02
#define FGSP_ATRBDEF_USERDEF1 		0x03
#define FGSP_ATRBDEF_USERDEF2 		0x04
#define FGSP_ATRBDEF_USERDEF3 		0x05


/*****************************************************************************
 *	PRIMITIVE ENGINE REGISTER
 *****************************************************************************/
/* R/W 	Vertex context format definition */
#define FGPE_VTX_CONTEXT			(FIMG_BASE+0x30000)
/* R/W 	The X coord of viewport center */
#define FGPE_VIEWPORT_CENTER_XCOORD	(FIMG_BASE+0x30004)
/* R/W 	The Y coord of viewport center */
#define FGPE_VIEWPORT_CENTER_YCOORD	(FIMG_BASE+0x30008)
/* R/W 	The half of viewport width */
#define FGPE_VIEWPORT_HALF_WIDTH 	(FIMG_BASE+0x3000C)
/* R/W 	The negative half of viewport height */
#define FGPE_VIEWPORT_HALF_HEIGHT 	(FIMG_BASE+0x30010)
/* R/W 	The half of depth range far minus near  3F00_0000h */
#define FGPE_DEPTH_HALF_DISTANCE 	(FIMG_BASE+0x30014)
/* R/W 	The half of depth range far plus near   3F00_0000h */
#define FGPE_DEPTH_CENTER 		    (FIMG_BASE+0x30018)


/*****************************************************************************
 *	RASTER REGISTER
 *****************************************************************************/
/// Triangle Setup
/* RW 	Indicate sampling position */
#define FGRA_PIXEL_SAMPOS			(FIMG_BASE+0x38000)
/* RW 	Depth offset enable */
#define FGRA_DEPTH_OFFSET_EN		(FIMG_BASE+0x38004)
/* RW 	Depth offset factor	*/
#define FGRA_DEPTH_OFFSET_FACTOR    (FIMG_BASE+0x38008)
/* RW 	Depth offset units */
#define FGRA_DEPTH_OFFSET_UNIT 		(FIMG_BASE+0x3800C)
/* RW 	Depth offset implementation constant r vlaue 3380_0002h */
#define FGRA_DEPTH_OFFSET_RVAL 		(FIMG_BASE+0x38010)
/* RW 	Back-face culling control */
#define FGRA_BACKFACE_CULL	        (FIMG_BASE+0x38014)
/* RW 	Y clip coordinate */
#define FGRA_CLIP_YCORD 	        (FIMG_BASE+0x38018)

/// Raster
/* R/W	Indicate LOD calculation control */
#define FGRA_LOD_CTRL		        (FIMG_BASE+0x3C000)
/* R/W	X clip coordinate */
#define FGRA_CLIP_XCORD			    (FIMG_BASE+0x3C004)
/* RW	Point width */
#define FGRA_POINT_WIDTH		    (FIMG_BASE+0x3801C)
#define FGRA_PWIDTH					FGRA_POINT_WIDTH
/* RW Point width min. value */
#define FGRA_POINT_SIZE_MIN		    (FIMG_BASE+0x38020)
#define FGRA_PSIZE_MIN				FGRA_POINT_SIZE_MIN
/* RW Point width max. value */
#define FGRA_POINT_SIZE_MAX		    (FIMG_BASE+0x38024)
#define FGRA_PSIZE_MAX				FGRA_POINT_SIZE_MAX
/* RW Texture cood. replace control */
#define FGRA_COORD_REPLACE		    (FIMG_BASE+0x38028)
#define FGRA_LWIDTH					FGRA_LINE_WIDTH
/* RW	Line width */
#define FGRA_LINE_WIDTH		        (FIMG_BASE+0x3802C)
/// RASTER ENGINE definitions
#define BITSHIFT_RA_FACECULL_ENABLE		3
#define BITSHIFT_RA_FACECULL_FACEDIR	2

#define MAXVALUE_RA_COORD		((1 << 12) - 1)
#define BITSHIFT_RA_CLIP_MAX	16

#define MAXVALUE_RA_LOD			((1 << 24) - 1)

#define BITSHIFT_RA_LODCON7		21
#define BITSHIFT_RA_LODCON6		18
#define BITSHIFT_RA_LODCON5		15
#define BITSHIFT_RA_LODCON4		12
#define BITSHIFT_RA_LODCON3		9
#define BITSHIFT_RA_LODCON2		6
#define BITSHIFT_RA_LODCON1		3

#define MAXVALUE_RA_COODREPLACE		7
/**-----------------------------------------------------------------------------
  RASTER ENGINE REGISTER(Include Triangle Setup Engine)
 -----------------------------------------------------------------------------*/
/** X clip control bits. */
#define XCLIP_MIN_CLEAR 			0xFFFFF800
#define XCLIP_MIN_SHIFT				0
#define XCLIP_MAX_CLEAR				0xF800FFFF
#define XCLIP_MAX_SHIFT				16
/** Y clip control bits. */
#define YCLIP_MIN_CLEAR				0xFFFFF800
#define YCLIP_MIN_SHIFT				0
#define YCLIP_MAX_CLEAR				0xF800FFFF
#define YCLIP_MAX_SHIFT				16


/*****************************************************************************
	FRAGMENT(PIXEL) SHADER REGISTER
 *****************************************************************************/

/* R/W Pixel shader execution mode control register */
#define FGPS_EXE_MODE			(FIMG_BASE+0x4C800)
/* R/W Start address of the pixel shader program */
#define FGPS_PC_START     	    (FIMG_BASE+0x4C804)
/* R/W End address of the pixel shader program */
#define FGPS_PC_END       	    (FIMG_BASE+0x4C808)
/* R/W Copy FGPS_PCStart to program counter */
#define FGPS_PC_COPY      	    (FIMG_BASE+0x4C80C)
/* R/W Num of attribute for current context */
#define FGPS_ATTRIB_NUM	        (FIMG_BASE+0x4C810)
/* R/W Stauts signal which means
 * that PS Input Buffer initialization is not completed */
#define FGPS_INBUF_STATUS    	(FIMG_BASE+0x4C814)
/// Address Map of Vertex Shader
/* R/W	Reseved[16KB] Actual[8KB] PS's instruction register	xxxx_xxxxh */
#define FGPS_INSTMEM_SADDR		(FIMG_BASE+0x40000)
#define FGPS_INSTMEM_EADDR		(FIMG_BASE+0x41FFF)
/* R/W	Reseved[16KB] Actual[4KB] PS's constant float register xxxx_xxxxh */
#define FGPS_CFLOAT_SADDR		(FIMG_BASE+0x44000)
#define FGPS_CFLOAT_EADDR		(FIMG_BASE+0x44FFF)
/* R/W	Reseved[ 1KB] Actual[64B] PS's constant integer register xxxx_xxxxh */
#define FGPS_CINT_SADDR			(FIMG_BASE+0x48000)
#define FGPS_CINT_EADDR			(FIMG_BASE+0x4803F)
/* R/W	Reseved[ 1KB] Actual[4B] PS's Boolean register xxxx_xxxxh */
#define FGPS_CBOOL_SADDR		(FIMG_BASE+0x48400)
#define FGPS_CBOOL_EADDR		(FIMG_BASE+0x48400)

/**-----------------------------------------------------------------------------
  FRAGMENT SHADER FIELD
 -----------------------------------------------------------------------------*/
#define FSIN_ATTRIB_MAX_SIZE    9
#define FSIN_ATTRIB_MIN_SIZE    1
#define FSOUT_ATTRIB_MAX_SIZE   1
#define FSOUT_ATTRIB_MIN_SIZE   1

/** 16*1024(Byte) = 4096(WORD) = 0x1000(Hex) */
#define FGPS_INSTMEM_SIZE		0x7FF
/** 4*1024(Byte) = 1024(WORD) = 0x400(Hex) */
#define FGPS_CFLOAT_SIZE		0x3FF
#define FGPS_CINT_SIZE			0xF 	/** 64(Byte) = 16(WORD) = 0x10(Hex) */
#define FGPS_CBOOL_SIZE			0x1 	/** 4(Byte) = 1(WORD) = 0x1(Hex) */

/// Related to execution mode register
#define HOST_ACCESS_MODE			0
#define PS_EXECUTION_MODE			1
#define PS_ISNOT_EMPTY_MASK			0x00003717

/// Related to input buffer status register
#define INPUT_BUFFER_READY			0
#define INPUT_BUFFER_NOT_READY 		1
#define PS_INBUF_STATUS_MASK		0x00000001


/*****************************************************************************
 *	TEXTURE UNITS REGISTER
 *****************************************************************************/
#define FGTU_TEX0_CTRL           (FIMG_BASE+0x60000) /* R/W */
#define FGTU_TEX0_USIZE          (FIMG_BASE+0x60004) /* R/W */
#define FGTU_TEX0_VSIZE          (FIMG_BASE+0x60008) /* R/W */
#define FGTU_TEX0_PSIZE          (FIMG_BASE+0x6000C) /* R/W */
#define FGTU_TEX0_L1_OFFSET      (FIMG_BASE+0x60010)
#define FGTU_TEX0_L2_OFFSET      (FIMG_BASE+0x60014)
#define FGTU_TEX0_L3_OFFSET      (FIMG_BASE+0x60018)
#define FGTU_TEX0_L4_OFFSET      (FIMG_BASE+0x6001C)
#define FGTU_TEX0_L5_OFFSET      (FIMG_BASE+0x60020)
#define FGTU_TEX0_L6_OFFSET      (FIMG_BASE+0x60024)
#define FGTU_TEX0_L7_OFFSET      (FIMG_BASE+0x60028)
#define FGTU_TEX0_L8_OFFSET      (FIMG_BASE+0x6002C)
#define FGTU_TEX0_L9_OFFSET      (FIMG_BASE+0x60030)
#define FGTU_TEX0_L10_OFFSET     (FIMG_BASE+0x60034)
#define FGTU_TEX0_L11_OFFSET     (FIMG_BASE+0x60038)
#define FGTU_TEX0_MIN_LEVEL      (FIMG_BASE+0x6003C)
#define FGTU_TEX0_MAX_LEVEL      (FIMG_BASE+0x60040)
#define FGTU_TEX0_BASE_ADDR      (FIMG_BASE+0x60044)
#define FGTU_TEX1_CTRL           (FIMG_BASE+0x60050)
#define FGTU_TEX1_USIZE          (FIMG_BASE+0x60054)
#define FGTU_TEX1_VSIZE          (FIMG_BASE+0x60058)
#define FGTU_TEX1_PSIZE          (FIMG_BASE+0x6005C)
#define FGTU_TEX1_L1_OFFSET      (FIMG_BASE+0x60060)
#define FGTU_TEX1_L2_OFFSET      (FIMG_BASE+0x60064)
#define FGTU_TEX1_L3_OFFSET      (FIMG_BASE+0x60068)
#define FGTU_TEX1_L4_OFFSET      (FIMG_BASE+0x6006C)
#define FGTU_TEX1_L5_OFFSET      (FIMG_BASE+0x60070)
#define FGTU_TEX1_L6_OFFSET      (FIMG_BASE+0x60074)
#define FGTU_TEX1_L7_OFFSET      (FIMG_BASE+0x60078)
#define FGTU_TEX1_L8_OFFSET      (FIMG_BASE+0x6007C)
#define FGTU_TEX1_L9_OFFSET      (FIMG_BASE+0x60080)
#define FGTU_TEX1_L10_OFFSET     (FIMG_BASE+0x60084)
#define FGTU_TEX1_L11_OFFSET     (FIMG_BASE+0x60088)
#define FGTU_TEX1_MIN_LEVEL      (FIMG_BASE+0x6008C)
#define FGTU_TEX1_MAX_LEVEL      (FIMG_BASE+0x60090)
#define FGTU_TEX1_BASE_ADDR      (FIMG_BASE+0x60094)
#define FGTU_TEX2_CTRL           (FIMG_BASE+0x600A0)
#define FGTU_TEX2_USIZE          (FIMG_BASE+0x600A4)
#define FGTU_TEX2_VSIZE          (FIMG_BASE+0x600A8)
#define FGTU_TEX2_PSIZE          (FIMG_BASE+0x600AC)
#define FGTU_TEX2_L1_OFFSET      (FIMG_BASE+0x600B0)
#define FGTU_TEX2_L2_OFFSET      (FIMG_BASE+0x600B4)
#define FGTU_TEX2_L3_OFFSET      (FIMG_BASE+0x600B8)
#define FGTU_TEX2_L4_OFFSET      (FIMG_BASE+0x600BC)
#define FGTU_TEX2_L5_OFFSET      (FIMG_BASE+0x600C0)
#define FGTU_TEX2_L6_OFFSET      (FIMG_BASE+0x600C4)
#define FGTU_TEX2_L7_OFFSET      (FIMG_BASE+0x600C8)
#define FGTU_TEX2_L8_OFFSET      (FIMG_BASE+0x600CC)
#define FGTU_TEX2_L9_OFFSET      (FIMG_BASE+0x600D0)
#define FGTU_TEX2_L10_OFFSET     (FIMG_BASE+0x600D4)
#define FGTU_TEX2_L11_OFFSET     (FIMG_BASE+0x600D8)
#define FGTU_TEX2_MIN_LEVEL      (FIMG_BASE+0x600DC)
#define FGTU_TEX2_MAX_LEVEL      (FIMG_BASE+0x600E0)
#define FGTU_TEX2_BASE_ADDR      (FIMG_BASE+0x600E4)
#define FGTU_TEX3_CTRL           (FIMG_BASE+0x600F0)
#define FGTU_TEX3_USIZE          (FIMG_BASE+0x600F4)
#define FGTU_TEX3_VSIZE          (FIMG_BASE+0x600F8)
#define FGTU_TEX3_PSIZE          (FIMG_BASE+0x600FC)
#define FGTU_TEX3_L1_OFFSET      (FIMG_BASE+0x60100)
#define FGTU_TEX3_L2_OFFSET      (FIMG_BASE+0x60104)
#define FGTU_TEX3_L3_OFFSET      (FIMG_BASE+0x60108)
#define FGTU_TEX3_L4_OFFSET      (FIMG_BASE+0x6010C)
#define FGTU_TEX3_L5_OFFSET      (FIMG_BASE+0x60110)
#define FGTU_TEX3_L6_OFFSET      (FIMG_BASE+0x60114)
#define FGTU_TEX3_L7_OFFSET      (FIMG_BASE+0x60118)
#define FGTU_TEX3_L8_OFFSET      (FIMG_BASE+0x6011C)
#define FGTU_TEX3_L9_OFFSET      (FIMG_BASE+0x60120)
#define FGTU_TEX3_L10_OFFSET     (FIMG_BASE+0x60124)
#define FGTU_TEX3_L11_OFFSET     (FIMG_BASE+0x60128)
#define FGTU_TEX3_MIN_LEVEL      (FIMG_BASE+0x6012C)
#define FGTU_TEX3_MAX_LEVEL      (FIMG_BASE+0x60130)
#define FGTU_TEX3_BASE_ADDR      (FIMG_BASE+0x60134)
#define FGTU_TEX4_CTRL           (FIMG_BASE+0x60140)
#define FGTU_TEX4_USIZE          (FIMG_BASE+0x60144)
#define FGTU_TEX4_VSIZE          (FIMG_BASE+0x60148)
#define FGTU_TEX4_PSIZE          (FIMG_BASE+0x6014C)
#define FGTU_TEX4_L1_OFFSET      (FIMG_BASE+0x60150)
#define FGTU_TEX4_L2_OFFSET      (FIMG_BASE+0x60154)
#define FGTU_TEX4_L3_OFFSET      (FIMG_BASE+0x60158)
#define FGTU_TEX4_L4_OFFSET      (FIMG_BASE+0x6015C)
#define FGTU_TEX4_L5_OFFSET      (FIMG_BASE+0x60160)
#define FGTU_TEX4_L6_OFFSET      (FIMG_BASE+0x60164)
#define FGTU_TEX4_L7_OFFSET      (FIMG_BASE+0x60168)
#define FGTU_TEX4_L8_OFFSET      (FIMG_BASE+0x6016C)
#define FGTU_TEX4_L9_OFFSET      (FIMG_BASE+0x60170)
#define FGTU_TEX4_L10_OFFSET     (FIMG_BASE+0x60174)
#define FGTU_TEX4_L11_OFFSET     (FIMG_BASE+0x60178)
#define FGTU_TEX4_MIN_LEVEL      (FIMG_BASE+0x6017C)
#define FGTU_TEX4_MAX_LEVEL      (FIMG_BASE+0x60180)
#define FGTU_TEX4_BASE_ADDR      (FIMG_BASE+0x60184)
#define FGTU_TEX5_CTRL           (FIMG_BASE+0x60190)
#define FGTU_TEX5_USIZE          (FIMG_BASE+0x60194)
#define FGTU_TEX5_VSIZE          (FIMG_BASE+0x60198)
#define FGTU_TEX5_PSIZE          (FIMG_BASE+0x6019C)
#define FGTU_TEX5_L1_OFFSET      (FIMG_BASE+0x601A0)
#define FGTU_TEX5_L2_OFFSET      (FIMG_BASE+0x601A4)
#define FGTU_TEX5_L3_OFFSET      (FIMG_BASE+0x601A8)
#define FGTU_TEX5_L4_OFFSET      (FIMG_BASE+0x601AC)
#define FGTU_TEX5_L5_OFFSET      (FIMG_BASE+0x601B0)
#define FGTU_TEX5_L6_OFFSET      (FIMG_BASE+0x601B4)
#define FGTU_TEX5_L7_OFFSET      (FIMG_BASE+0x601B8)
#define FGTU_TEX5_L8_OFFSET      (FIMG_BASE+0x601BC)
#define FGTU_TEX5_L9_OFFSET      (FIMG_BASE+0x601C0)
#define FGTU_TEX5_L10_OFFSET     (FIMG_BASE+0x601C4)
#define FGTU_TEX5_L11_OFFSET     (FIMG_BASE+0x601C8)
#define FGTU_TEX5_MIN_LEVEL      (FIMG_BASE+0x601CC)
#define FGTU_TEX5_MAX_LEVEL      (FIMG_BASE+0x601D0)
#define FGTU_TEX5_BASE_ADDR      (FIMG_BASE+0x601D4)
#define FGTU_TEX6_CTRL           (FIMG_BASE+0x601E0)
#define FGTU_TEX6_USIZE          (FIMG_BASE+0x601E4)
#define FGTU_TEX6_VSIZE          (FIMG_BASE+0x601E8)
#define FGTU_TEX6_PSIZE          (FIMG_BASE+0x601EC)
#define FGTU_TEX6_L1_OFFSET      (FIMG_BASE+0x601F0)
#define FGTU_TEX6_L2_OFFSET      (FIMG_BASE+0x601F4)
#define FGTU_TEX6_L3_OFFSET      (FIMG_BASE+0x601F8)
#define FGTU_TEX6_L4_OFFSET      (FIMG_BASE+0x601FC)
#define FGTU_TEX6_L5_OFFSET      (FIMG_BASE+0x60200)
#define FGTU_TEX6_L6_OFFSET      (FIMG_BASE+0x60204)
#define FGTU_TEX6_L7_OFFSET      (FIMG_BASE+0x60208)
#define FGTU_TEX6_L8_OFFSET      (FIMG_BASE+0x6020C)
#define FGTU_TEX6_L9_OFFSET      (FIMG_BASE+0x60210)
#define FGTU_TEX6_L10_OFFSET     (FIMG_BASE+0x60214)
#define FGTU_TEX6_L11_OFFSET     (FIMG_BASE+0x60218)
#define FGTU_TEX6_MIN_LEVEL      (FIMG_BASE+0x6021C)
#define FGTU_TEX6_MAX_LEVEL      (FIMG_BASE+0x60220)
#define FGTU_TEX6_BASE_ADDR      (FIMG_BASE+0x60224)
#define FGTU_TEX7_CTRL           (FIMG_BASE+0x60230)
#define FGTU_TEX7_USIZE          (FIMG_BASE+0x60234)
#define FGTU_TEX7_VSIZE          (FIMG_BASE+0x60238)
#define FGTU_TEX7_PSIZE          (FIMG_BASE+0x6023C)
#define FGTU_TEX7_L1_OFFSET      (FIMG_BASE+0x60240)
#define FGTU_TEX7_L2_OFFSET      (FIMG_BASE+0x60244)
#define FGTU_TEX7_L3_OFFSET      (FIMG_BASE+0x60248)
#define FGTU_TEX7_L4_OFFSET      (FIMG_BASE+0x6024C)
#define FGTU_TEX7_L5_OFFSET      (FIMG_BASE+0x60250)
#define FGTU_TEX7_L6_OFFSET      (FIMG_BASE+0x60254)
#define FGTU_TEX7_L7_OFFSET      (FIMG_BASE+0x60258)
#define FGTU_TEX7_L8_OFFSET      (FIMG_BASE+0x6025C)
#define FGTU_TEX7_L9_OFFSET      (FIMG_BASE+0x60260)
#define FGTU_TEX7_L10_OFFSET     (FIMG_BASE+0x60264)
#define FGTU_TEX7_L11_OFFSET     (FIMG_BASE+0x60268)
#define FGTU_TEX7_MIN_LEVEL      (FIMG_BASE+0x6026C)
#define FGTU_TEX7_MAX_LEVEL      (FIMG_BASE+0x60270)
#define FGTU_TEX7_BASE_ADDR      (FIMG_BASE+0x60274)
#define FGTU_COLOR_KEY1 		 (FIMG_BASE+0x60280) /* R/W Color Key1 */
#define FGTU_COLOR_KEY2 		 (FIMG_BASE+0x60284) /* R/W Color Key2 */
#define FGTU_COLOR_KEY_YUV 		 (FIMG_BASE+0x60288) /* R/W YUV Color Key */
#define FGTU_COLOR_KEY_MASK 	 (FIMG_BASE+0x6028C) /* R/W Color Key Mask */
/* R/W Palette address for indexed texture */
#define FGTU_PALETTE_ADDR 	     (FIMG_BASE+0x60290)
/* R/W Palette entry point */
#define FGTU_PALETTE_ENTRY 	     (FIMG_BASE+0x60294)

/* R/W Vertex texture n¡¯s control */
#define FGTU_VTXTEX0_CTRL 		 (FIMG_BASE+0x602C0)
/* R/W Vertex texture base level */
#define FGTU_VTXTEX0_BASE_ADDR 	 (FIMG_BASE+0x602C4)
#define FGTU_VTXTEX1_CTRL 		 (FIMG_BASE+0x602C8)
#define FGTU_VTXTEX1_BASE_ADDR 	 (FIMG_BASE+0x602CC)
#define FGTU_VTXTEX2_CTRL 		 (FIMG_BASE+0x602D0)
#define FGTU_VTXTEX2_BASE_ADDR 	 (FIMG_BASE+0x602D4)
#define FGTU_VTXTEX3_CTRL 		 (FIMG_BASE+0x602D8)
#define FGTU_VTXTEX3_BASE_ADDR 	 (FIMG_BASE+0x602DC)

#define FGL_MIN_TEXTURE_WIDTH	    1
#define FGL_MIN_TEXTURE_HEIGHT	    1
#define FGL_MIN_TEXTURE_DEPTH	    1
#define FGL_MAX_TEXTURE_WIDTH	    2048
#define FGL_MAX_TEXTURE_HEIGHT	    2048
#define FGL_MAX_TEXTURE_DEPTH	    2048
#define FGL_MAX_TEXTURE_UNITS       8

/// Texture Unit Definitions
#define MAXVALUE_TU_TEX_SIZE		((1 << 11) - 1)

#define BITINTERVAL_TU_UNIT			(0x50)
#define MAXVALUE_TU_UNIT			(0x7)
#define MAXVALUE_TU_MIPMAP_LEVEL	(11)

#define MAXVALUE_TU_CK_UNIT			1
#define BITSHIFT_TU_CK_RED			16
#define BITSHIFT_TU_CK_GREEN		8

#define MAXVALUE_TU_CK_MASK			7
#define MAXVALUE_TU_PAL_ADDR		255

#define BITSHIFT_TU_VTX_UMODE		10
#define BITSHIFT_TU_VTX_VMODE		8
#define BITSHIFT_TU_VTX_USIZE		4


/*****************************************************************************
	PER-FRAGMENT REGISTER
 *****************************************************************************/
/* R/W 	Scissor X coordinate */
#define FGPF_SCISSOR_XCORD		(FIMG_BASE+0x70000)
/* R/W 	Scissor Y coordinate */
#define FGPF_SCISSOR_YCORD		(FIMG_BASE+0x70004)
/* R/W 	Alpha test */
#define FGPF_ALPHA       		(FIMG_BASE+0x70008)
/* R/W 	Frontface stencil */
#define FGPF_FRONTFACE_STENCIL 	(FIMG_BASE+0x7000C)
/* R/W 	Backface stencil  */
#define FGPF_BACKFACE_STENCIL 	(FIMG_BASE+0x70010)
/* R/W 	Depth test  */
#define FGPF_DEPTH       		(FIMG_BASE+0x70014)
/* R/W 	Constant color for Blending  */
#define FGPF_BLEND_COLOR		(FIMG_BASE+0x70018)
/* R/W 	Blending  */
#define FGPF_BLEND 				(FIMG_BASE+0x7001C)
/* R/W 	RGBA color logical operation  */
#define FGPF_LOGIC_OP 			(FIMG_BASE+0x70020)
/* R/W 	Color buffer write mask  */
#define FGPF_COLOR_MASK 		(FIMG_BASE+0x70024)
/* R/W 	Depth buffer write mask  */
#define FGPF_STENCIL_DEPTH_MASK (FIMG_BASE+0x70028)
/* R/W 	Color buffer control */
#define FGPF_COLORBUF_CTRL 		(FIMG_BASE+0x7002C)
/* R/W 	Depth buffer base address */
#define FGPF_DEPTHBUF_ADDR		(FIMG_BASE+0x70030)
/* R/W 	Frame buffer base address */
#define FGPF_COLORBUF_ADDR 	    (FIMG_BASE+0x70034)
/* R/W 	Frame buffer width */
#define FGPF_COLORBUF_WIDTH 	(FIMG_BASE+0x70038)

#define FGPF_FB_MAX_STRIDE      2048

#define COLOR_MASK_ALPHA		0x00000001
#define COLOR_MASK_RED			0x00000002
#define COLOR_MASK_GREEN		0x00000004
#define COLOR_MASK_BLUE			0x00000008
#define DEPTH_MAX_VALUE		    0xFFFFFF
#define DEPTH_MIN_VALUE		    0x0
#define DEPTH_ENABLE_MASK		0x1
#define STENCIL_ENABLE_MASK     0x1



#ifdef _FIMG3DSE_VER_1_2
/*****************************************************************************
	DMA CONTROL REGISTER
 *****************************************************************************/
	/* R/W DMA proority control 0000_0000h */
    #define FIMG_DMA_CNTL0          (FIMG_BASE+0x80000)
    #define FIMG_DMA_CNTL1          (FIMG_BASE+0x80004)
    #define FIMG_DMA_CNTL2          (FIMG_BASE+0x80008)
#endif

/*****************************************************************************
	GLOBAL REGISTER
 *****************************************************************************/
/* W  	Each block's reset signal 				FFFF_FFFFh*/
//#define FIMG_GLOBREG_SWRESET 			0x00000000
//#define SW_RESET_VS					0x00000001
//#define SW_RESET_RESERVED0			0x0000000E
//#define SW_RESET_VSMASK				0x0000000F
//#define SW_RESET_PE					0x00000010
//#define SW_RESET_RESERVED1			0x000000E0
//#define SW_RESET_RE					0x00000100
//#define SW_RESET_RESERVED2			0x00000E00
//#define SW_RESET_PS					0x00001000
//#define SW_RESET_RESERVED3			0x0000E000
//#define SW_RESET_PF					0x00010000
//#define SW_RESET_RESERVED4			0xFFFE0000
#define SW_RESET_ALL				0x00011111


/*****************************************************************************
	HOST INTERFACE REGISTER
 *****************************************************************************/
//#define FIMG_HISFR_PRIMITIVE 				0x08000	/* r/W 	Primitive's sort */
#define PRIMITIVE_CLRMASK 			0xFFFFFFF0
#define PRIMITIVE_TRIANGLE 			0x00000001
#define PRIMITIVE_TRIANGLESTRIP 	0x00000002
#define PRIMITIVE_TRIANGLEFAN 		0x00000003

/* r/W 	Vertex's format refer to geometry		*/
//#define FIMG_HISFR_VF0 					0x08004
/* r/W 	Vertex's format refer to texture		*/
//#define FIMG_HISFR_VF1 					0x08008
#define VERTEX_FORMAT_X				0x8
#define VERTEX_FORMAT_XY			0x9
#define VERTEX_FORMAT_XYZ			0xA
#define VERTEX_FORMAT_XYZW			0xB

/* r/W 	Vertex's data type refer to geometry	*/
//#define FIMG_HISFR_VFT0 					0x0800C
/* r/W 	Vertex's data type refer to texture		*/
//#define FIMG_HISFR_VFT1 					0x08010
#define VTX_DATATYPE_BYTE			0x0
#define VTX_DATATYPE_SHORT			0x1
#define VTX_DATATYPE_INT			0x2
#define VTX_DATATYPE_FLOAT			0x3
#define VTX_DATATYPE_UBYTE			0x4
#define VTX_DATATYPE_USHORT			0x5
#define VTX_DATATYPE_UINT			0x6
#define VTX_DATATYPE_FIXED			0x7
#define VTX_DATATYPE_NORMAL_BYTE	0x8
#define VTX_DATATYPE_NORMAL_SHORT	0x9
#define VTX_DATATYPE_NORMAL_INT		0xA
#define VTX_DATATYPE_NORMAL_UBYTE	0xC
#define VTX_DATATYPE_NORMAL_USHORT	0xD
#define VTX_DATATYPE_NORMAL_UINT	0xE
#define VTX_DATATYPE_NORMAL_FIXED	0xF

/*****************************************************************************
 *	TEXTURE CONTROL REGISTER FELID DEFINE
 *****************************************************************************/
#define TEXCTRL_TEXTURE_TYPE_SHIFT       27
#define TEXCTRL_COLOR_KEY_SHIFT          21
#define TEXCTRL_EXPANSION_SHIFT          20
#define TEXCTRL_PALETTE_FORMAT_SHIFT     17
#define TEXCTRL_TEXTURE_FORMAT_SHIFT     12
#define TEXCTRL_U_WRAPMODE_SHIFT         10
#define TEXCTRL_V_WRAPMODE_SHIFT         8
#define TEXCTRL_P_WRAPMODE_SHIFT         6
#define TEXCTRL_NONPARAMETRIC_SHIFT      4
#define TEXCTRL_MAG_FILTER_SHIFT         3
#define TEXCTRL_MIN_FILTER_SHIFT         2
#define TEXCTRL_MIPMAP_FILTER_SHIFT      0

/*****************************************************************************
	PER-FRAGMENT REGISTER FELID DEFINE
 *****************************************************************************/
#define SCISSOR_MAX_SIZE		        (2048)


/*****************************************************************************
	PROBE WATCH POINTS FOR FIMG-3DSE V1.X
 *****************************************************************************/
#define FGWP_DEBUG_ENABLE 	(FIMG_BASE+0x200)  /* DebugOn           */
#define FGWP_IN_HI_GRP0 	(FIMG_BASE+0x204)  /* Watch_InHI_Grp0   */
#define FGWP_IN_HI_GRP1 	(FIMG_BASE+0x208)  /* Watch_InHI_Grp1   */
#define FGWP_IN_VC_VS 	    (FIMG_BASE+0x20C)  /* Watch_InVC_VS     */  
#define FGWP_IN_PE 	        (FIMG_BASE+0x210)  /* Watch_InPE        */
#define FGWP_IN_TSE_RA 	    (FIMG_BASE+0x214)  /* Watch_InTSE_RA    */
#define FGWP_IN_PS0 	    (FIMG_BASE+0x218)  /* Watch_InPS0       */      
#define FGWP_IN_PS1 	    (FIMG_BASE+0x21C)  /* Watch_InPS1       */  
#define FGWP_IN_VTU_TU 	    (FIMG_BASE+0x220)  /* Watch_InVTU_TU    */
#define FGWP_IN_TCACHE 	    (FIMG_BASE+0x224)  /* Watch_InTCache    */
#define FGWP_IN_PF 	        (FIMG_BASE+0x228)  /* Watch_InPF        */
#define FGWP_IN_CZCACHE 	(FIMG_BASE+0x22C)  /* Watch_InCZCache   */
#define FGWP_IN_AXIRB 	    (FIMG_BASE+0x230)  /* Watch_InAXIArb    */
#define FGWP_IN_DMA 	    (FIMG_BASE+0x234)  /* Watch_InDMA       */


/***********************************************************************
	@USAGE
	WRITE
		if you want to write SFR by 32-bits, then
			outw(SFR_ADDRESS, (unsigned int)WDATA);
		if by 16-bits, then
			outs(SFR_ADDRESS, (unsigned short int)WDATA);
		if by 8-bits, then
			outb(SFR_ADDRESS, (unsigned char)WDATA);
	READ
		if you read SFR by 32-bits, then
			(unsigned int)RDATA = inw(SFR_ADDRESS);
		if by 16-bits, then
			(unsigned short int)RDATA = ins(SFR_ADDRESS);
		if by 8-bits, then
			(unsigned char)RDATA = inb(SFR_ADDRESS);
*************************************************************************/

typedef volatile unsigned char 	*vbptr;
typedef volatile unsigned short *vsptr;
typedef volatile unsigned int 	*vwptr;
typedef volatile float			*vfptr;

#define READREGB(Port)		(*((vbptr) (Port)))
#define READREGPB(Port, Y)	(Y =*((vbptr) (Port)))
#define WRITEREGB(Port, X)	(*((vbptr) (Port)) = (unsigned char) (X))

#define READREGS(Port)		(*((vsptr) (Port)))
#define READREGPS(Port, Y)	(Y =*((vsptr) (Port)))
#define WRITEREGS(Port, X)	(*((vsptr) (Port)) = (unsigned short) (X))

#define READREG(Port)		(*((vwptr) (Port)))
#define READREGP(Port, Y)	(Y =*((vwptr) (Port)))
#define WRITEREG(Port, X)	(*((vwptr) (Port)) = (unsigned int) (X))

#define READREGF(Port)		(*((vfptr) (Port)))
#define READREGPF(Port, Y)	(Y =*((vfptr) (Port)))
#define WRITEREGF(Port, X)	(*((vfptr) (Port)) = (float) (X))

#ifdef __cplusplus
}
#endif


#endif /* __FIMG_3DREGS_H__ */

/*----------------------------------------< End of file >---------------------------------------------*/
