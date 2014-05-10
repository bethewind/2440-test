/*******************************************************************************
 *
 *	ViP model for FIMGSE
 *		version 1.0
 *
 *	fimg_sysutil.c
 *
 *	by cheolkyoo.kim
 *	Graphics IP Team in AP, Mobile Solution Development,
 *	System LSI Division, Semiconductor Business,
 *	Samsung Electronics
 *
 *	Copyright (c) 2005 FIMG team
 *
 *	All rights reserved. No part of this program may be reproduced, stored
 *	in a retrieval system, or tranmitted, in any form or by any means,
 *	electronic, mechanical, photocopying, recording, or otherwise, without
 *	the prior written permission of the author.
 *
 *	2006. 1. 3	by cheolkyoo.kim
 *		Provides functions to manage memory, register.
 *
 *	$RCSfile: fimg_sysutil.c,v $
 *	$Revision: 1.4 $
 *	$Author: cheolkyoo.kim $
 *	$Date: 2006/05/08 05:39:11 $
 *	$Locker:  $
 *
 *	$Source: C:/CVS/CVSrepository/FIMG-3DSE_SW/fimg3dse_fpga/fimg3d/common/fimg_sysutil.c,v $
 *	$State: Exp $
 *	$Log: fimg_sysutil.c,v $
 *	Revision 1.4  2006/05/08 05:39:11  cheolkyoo.kim
 *	no message
 *	
 *	Revision 1.3  2006/04/13 10:53:52  cheolkyoo.kim
 *	no message
 *	
 *	Revision 1.2  2006/04/05 03:30:09  cheolkyoo.kim
 *	no message
 *	
 *	Revision 1.1  2006/03/10 08:29:08  cheolkyoo.kim
 *	Initial import of FIMG-3DSE_SW package
 *	
 ******************************************************************************/
/****************************************************************************
 *  INCLUDES
 ****************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "system.h"
#include "library.h"
#include "fgl.h"
#include "Config.h"
#include "register.h"
#include "Defines.h"
#include "SysUtility.h"

#include "dma.h"
#include "intc.h"


//using namespace std;

//#include "uart.h"
extern void uratPrintf(const char *fmt,...);

/*****************************************************************************
 *  DEFINES 
 *****************************************************************************/
// Dump frame buffer define
#define BMP_HEADER_SIZE		(14)
#define BMP_INFO_SIZE		(40)

/*****************************************************************************
 *  CONSTANT MACRO 
 *****************************************************************************/
#define FGL_MAX(A,B)	((B)>(A)?(B):(A))
#define FGL_MIN(A,B)	((B)<(A)?(B):(A))

/*****************************************************************************
 *  FUNCTION MACRO 
 *****************************************************************************/
/*****************************************************************************
 *  TYPEDEFS 
 *****************************************************************************/
/*****************************************************************************
 *  ENUMS 
 *****************************************************************************/
/*****************************************************************************
 *  EXTERN 
 *****************************************************************************/
const default_reg gb_default_regs[] =
{
	{ r_pipeline_state,	0, {FG_PIPELINE_STATUS,	        0} },
	{ r_cache_ctrl,	    1, {FG_CACHE_CTRL, 	            0} },
	{ r_sw_reset,		0, {FG_SW_RESET,		        0} },
	{ r_int_pending,	1, {FG_INT_PENDING, 	        0} },
	{ r_int_mask,		1, {FG_INT_MASK,	        	0} },
	{ r_pipeline_mask,	1, {FG_PIPELINE_MASK,       	0} },
	{ r_pipeline_taget,	1, {FG_PIPELINE_TARGET_STATE,	0} },
	{ r_int_state,	    0, {FG_PIPELINE_INT_STATE, 	    0} }
};


const default_reg hi_default_regs[] =
{
	{ r_fifo_empty_space, 0, {FGHI_FIFO_EMPTY_SPACE, 0} },
	{ r_fifo_entry_port,  0, {FGHI_FIFO_ENTRY,      0} },
	{ r_hi_ctrl,          1, {FGHI_HI_CTRL,         0} },
	{ r_idx_offset,       1, {FGHI_IDX_OFFSET,      0} },
	{ r_vtxbuf_addr,      1, {FGHI_VTXBUF_ADDR,     0} },
	{ r_vtxbuf_entry_port,0, {FGHI_VTXBUF_ENTRY,    0} },
	{ r_attrib_ctrl0, 1, {FGHI_ATTR0, 0} },
	{ r_attrib_ctrl1, 1, {FGHI_ATTR1, 0} },
	{ r_attrib_ctrl2, 1, {FGHI_ATTR2, 0} },
	{ r_attrib_ctrl3, 1, {FGHI_ATTR3, 0} },
	{ r_attrib_ctrl4, 1, {FGHI_ATTR4, 0} },
	{ r_attrib_ctrl5, 1, {FGHI_ATTR5, 0} },
	{ r_attrib_ctrl6, 1, {FGHI_ATTR6, 0} },
	{ r_attrib_ctrl7, 1, {FGHI_ATTR7, 0} },
	{ r_attrib_ctrl8, 1, {FGHI_ATTR8, 0} },
	{ r_attrib_ctrl9, 1, {FGHI_ATTR9, 0} },
	{ r_attrib_ctrla, 1, {FGHI_ATTRA, 0} },
	{ r_attrib_ctrlb, 1, {FGHI_ATTRB, 0} },
	{ r_attrib_ctrlc, 1, {FGHI_ATTRC, 0} },
	{ r_attrib_ctrld, 1, {FGHI_ATTRD, 0} },
	{ r_attrib_ctrle, 1, {FGHI_ATTRE, 0} },
	{ r_attrib_ctrlf, 1, {FGHI_ATTRF, 0} },
	{ r_vtxbuf_ctrl0, 1, {FGHI_VTXBUF_CTRL0, 0} },
	{ r_vtxbuf_ctrl1, 1, {FGHI_VTXBUF_CTRL1, 0} },
	{ r_vtxbuf_ctrl2, 1, {FGHI_VTXBUF_CTRL2, 0} },
	{ r_vtxbuf_ctrl3, 1, {FGHI_VTXBUF_CTRL3, 0} },
	{ r_vtxbuf_ctrl4, 1, {FGHI_VTXBUF_CTRL4, 0} },
	{ r_vtxbuf_ctrl5, 1, {FGHI_VTXBUF_CTRL5, 0} },
	{ r_vtxbuf_ctrl6, 1, {FGHI_VTXBUF_CTRL6, 0} },
	{ r_vtxbuf_ctrl7, 1, {FGHI_VTXBUF_CTRL7, 0} },
	{ r_vtxbuf_ctrl8, 1, {FGHI_VTXBUF_CTRL8, 0} },
	{ r_vtxbuf_ctrl9, 1, {FGHI_VTXBUF_CTRL9, 0} },
	{ r_vtxbuf_ctrla, 1, {FGHI_VTXBUF_CTRLA, 0} },
	{ r_vtxbuf_ctrlb, 1, {FGHI_VTXBUF_CTRLB, 0} },
	{ r_vtxbuf_ctrlc, 1, {FGHI_VTXBUF_CTRLC, 0} },
	{ r_vtxbuf_ctrld, 1, {FGHI_VTXBUF_CTRLD, 0} },
	{ r_vtxbuf_ctrle, 1, {FGHI_VTXBUF_CTRLE, 0} },
	{ r_vtxbuf_ctrlf, 1, {FGHI_VTXBUF_CTRLF, 0} },
	{ r_vtxbuf_base0, 1, {FGHI_VTXBUF_BASE0, 0} },
	{ r_vtxbuf_base1, 1, {FGHI_VTXBUF_BASE1, 0} },
	{ r_vtxbuf_base2, 1, {FGHI_VTXBUF_BASE2, 0} },
	{ r_vtxbuf_base3, 1, {FGHI_VTXBUF_BASE3, 0} },
	{ r_vtxbuf_base4, 1, {FGHI_VTXBUF_BASE4, 0} },
	{ r_vtxbuf_base5, 1, {FGHI_VTXBUF_BASE5, 0} },
	{ r_vtxbuf_base6, 1, {FGHI_VTXBUF_BASE6, 0} },
	{ r_vtxbuf_base7, 1, {FGHI_VTXBUF_BASE7, 0} },
	{ r_vtxbuf_base8, 1, {FGHI_VTXBUF_BASE8, 0} },
	{ r_vtxbuf_base9, 1, {FGHI_VTXBUF_BASE9, 0} },
	{ r_vtxbuf_basea, 1, {FGHI_VTXBUF_BASEA, 0} },
	{ r_vtxbuf_baseb, 1, {FGHI_VTXBUF_BASEB, 0} },
	{ r_vtxbuf_basec, 1, {FGHI_VTXBUF_BASEC, 0} },
	{ r_vtxbuf_based, 1, {FGHI_VTXBUF_BASED, 0} },
	{ r_vtxbuf_basee, 1, {FGHI_VTXBUF_BASEE, 0} },
	{ r_vtxbuf_basef, 1, {FGHI_VTXBUF_BASEF, 0} }
};


const default_reg vs_default_regs[] =
{
	{r_vs_config,      	1, {FGVS_CONFIG, 			0}},
	{r_inter_status,    0, {FGVS_STATUS, 			0}},
	{r_pc_range, 		1, {FGVS_PC_RANGE, 			0}},
	{r_vs_attrib_num,   1, {FGVS_ATTRIB_NUM, 		0}},
	{r_in_attrib_idx0,  1, {FGVS_IN_ATTRIB_IDX0,	0}},
	{r_in_attrib_idx1,  1, {FGVS_IN_ATTRIB_IDX1,	0}},
	{r_in_attrib_idx2,  1, {FGVS_IN_ATTRIB_IDX2,	0}},
	{r_out_attrib_idx0, 1, {FGVS_OUT_ATTRIB_IDX0,	0}},
	{r_out_attrib_idx1, 1, {FGVS_OUT_ATTRIB_IDX1,	0}},
	{r_out_attrib_idx2, 1, {FGVS_OUT_ATTRIB_IDX2,	0}}
};


const shader_slot vs_shader_slots[] =
{
	{r_shader_instruction_slot,	{FGVS_INSTMEM_SADDR,	FGVS_INSTMEM_EADDR, FGVS_INSTMEM_SIZE}},
	{r_shader_const_float_slot, {FGVS_CFLOAT_SADDR,		FGVS_CFLOAT_EADDR,	FGVS_CFLOAT_SIZE}},
	{r_shader_const_int_slot,	{FGVS_CINT_SADDR, 		FGVS_CINT_EADDR, 	FGVS_CINT_SIZE}},
	{r_shader_const_bool_slot,  {FGVS_CBOOL_SADDR, 		FGVS_CBOOL_EADDR, 	FGVS_CBOOL_SIZE}}
};


const default_reg pe_default_regs[] =
{
	{r_vtx_context,			  1, {FGPE_VTX_CONTEXT,			    0}},
	{r_viewport_center_xcord, 1, {FGPE_VIEWPORT_CENTER_XCOORD,  0}},
	{r_viewport_center_ycord, 1, {FGPE_VIEWPORT_CENTER_YCOORD,  0}},
	{r_viewport_half_width,	  1, {FGPE_VIEWPORT_HALF_WIDTH,	    0}},
	{r_viewport_half_height,  1, {FGPE_VIEWPORT_HALF_HEIGHT, 	0}},
	{r_half_depth_range,      1, {FGPE_DEPTH_HALF_DISTANCE,	    0}},
	{r_average_near_far, 	  1, {FGPE_DEPTH_CENTER,			0}}
};

const default_reg ra_default_regs[] =
{
	{r_pixel_sampos,        1, {FGRA_PIXEL_SAMPOS,			0}},
	{r_depth_offset_en,     1, {FGRA_DEPTH_OFFSET_EN,		0}},
	{r_depth_offset_factor, 1, {FGRA_DEPTH_OFFSET_FACTOR,	0}},
	{r_depth_offset_unit,   1, {FGRA_DEPTH_OFFSET_UNIT,	    0}},
	{r_depth_offset_rval,   1, {FGRA_DEPTH_OFFSET_RVAL,	    0}},
	{r_backface_cull,       1, {FGRA_BACKFACE_CULL,		    0}},
	{r_clip_ycord,          1, {FGRA_CLIP_YCORD,			0}},
	{r_lod_ctrl,            1, {FGRA_LOD_CTRL,				0}},
	{r_clip_xcord,          1, {FGRA_CLIP_XCORD,			0}},
	{r_point_width,         1, {FGRA_POINT_WIDTH,		    0}},
	{r_point_size_min,      1, {FGRA_POINT_SIZE_MIN,		0}},
	{r_point_size_max,      1, {FGRA_POINT_SIZE_MAX,		0}},
	{r_coord_replace,       1, {FGRA_COORD_REPLACE,			0}},
	{r_line_width,          1, {FGRA_LINE_WIDTH,			0}}
};


const default_reg ps_default_regs[] =
{
	{r_exe_mode,		1, {FGPS_EXE_MODE,		0}},
	{r_pc_start,		1, {FGPS_PC_START,		0}},
	{r_pc_end,			1, {FGPS_PC_END,		0}},
	{r_pc_copy,			1, {FGPS_PC_COPY,		0}},
	{r_ps_attrib_num, 	1, {FGPS_ATTRIB_NUM,	0}},
	{r_inbuf_status,	0, {FGPS_INBUF_STATUS,	0}}
};


const shader_slot ps_shader_slots[] =
{
	{r_shader_instruction_slot,	{FGPS_INSTMEM_SADDR,	FGPS_INSTMEM_EADDR, FGPS_INSTMEM_SIZE}},
	{r_shader_const_float_slot, {FGPS_CFLOAT_SADDR,		FGPS_CFLOAT_EADDR,	FGPS_CFLOAT_SIZE}},
	{r_shader_const_int_slot,	{FGPS_CINT_SADDR, 		FGPS_CINT_EADDR, 	FGPS_CINT_SIZE}},
	{r_shader_const_bool_slot,  {FGPS_CBOOL_SADDR, 		FGPS_CBOOL_EADDR, 	FGPS_CBOOL_SIZE}}
};


const default_reg tu_default_regs[] =
{
	{r_tex0_ctrl,           1, {FGTU_TEX0_CTRL, 		0}},
	{r_tex0_usize,          1, {FGTU_TEX0_USIZE,	 	0}},
	{r_tex0_vsize,          1, {FGTU_TEX0_VSIZE,	 	0}},
	{r_tex0_psize,          1, {FGTU_TEX0_PSIZE,	 	0}},
	{r_tex0_l1_offset,      1, {FGTU_TEX0_L1_OFFSET, 	0}},
	{r_tex0_l2_offset,      1, {FGTU_TEX0_L2_OFFSET, 	0}},
	{r_tex0_l3_offset,      1, {FGTU_TEX0_L3_OFFSET, 	0}},
	{r_tex0_l4_offset,      1, {FGTU_TEX0_L4_OFFSET, 	0}},
	{r_tex0_l5_offset,      1, {FGTU_TEX0_L5_OFFSET, 	0}},
	{r_tex0_l6_offset,      1, {FGTU_TEX0_L6_OFFSET, 	0}},
	{r_tex0_l7_offset,      1, {FGTU_TEX0_L7_OFFSET, 	0}},
	{r_tex0_l8_offset,      1, {FGTU_TEX0_L8_OFFSET, 	0}},
	{r_tex0_l9_offset,      1, {FGTU_TEX0_L9_OFFSET, 	0}},
	{r_tex0_l10_offset,     1, {FGTU_TEX0_L10_OFFSET, 	0}},
	{r_tex0_l11_offset,     1, {FGTU_TEX0_L11_OFFSET, 	0}},
	{r_tex0_min_level,      1, {FGTU_TEX0_MIN_LEVEL, 	0}},
	{r_tex0_max_level,      1, {FGTU_TEX0_MAX_LEVEL, 	0}},
	{r_tex0_base_addr,      1, {FGTU_TEX0_BASE_ADDR, 	0}},
	{r_tex1_ctrl,           1, {FGTU_TEX1_CTRL, 		0}},
	{r_tex1_usize,          1, {FGTU_TEX1_USIZE,	 	0}},
	{r_tex1_vsize,          1, {FGTU_TEX1_VSIZE,	 	0}},
	{r_tex1_psize,          1, {FGTU_TEX1_PSIZE,	 	0}},
	{r_tex1_l1_offset,      1, {FGTU_TEX1_L1_OFFSET, 	0}},
	{r_tex1_l2_offset,      1, {FGTU_TEX1_L2_OFFSET, 	0}},
	{r_tex1_l3_offset,      1, {FGTU_TEX1_L3_OFFSET, 	0}},
	{r_tex1_l4_offset,      1, {FGTU_TEX1_L4_OFFSET, 	0}},
	{r_tex1_l5_offset,      1, {FGTU_TEX1_L5_OFFSET, 	0}},
	{r_tex1_l6_offset,      1, {FGTU_TEX1_L6_OFFSET, 	0}},
	{r_tex1_l7_offset,      1, {FGTU_TEX1_L7_OFFSET, 	0}},
	{r_tex1_l8_offset,      1, {FGTU_TEX1_L8_OFFSET, 	0}},
	{r_tex1_l9_offset,      1, {FGTU_TEX1_L9_OFFSET, 	0}},
	{r_tex1_l10_offset,     1, {FGTU_TEX1_L10_OFFSET, 	0}},
	{r_tex1_l11_offset,     1, {FGTU_TEX1_L11_OFFSET, 	0}},
	{r_tex1_min_level,      1, {FGTU_TEX1_MIN_LEVEL, 	0}},
	{r_tex1_max_level,      1, {FGTU_TEX1_MAX_LEVEL, 	0}},
	{r_tex1_base_addr,      1, {FGTU_TEX1_BASE_ADDR, 	0}},
	{r_tex2_ctrl,           1, {FGTU_TEX2_CTRL, 		0}},
	{r_tex2_usize,          1, {FGTU_TEX2_USIZE, 		0}},
	{r_tex2_vsize,          1, {FGTU_TEX2_VSIZE, 		0}},
	{r_tex2_psize,          1, {FGTU_TEX2_PSIZE, 		0}},
	{r_tex2_l1_offset,      1, {FGTU_TEX2_L1_OFFSET, 	0}},
	{r_tex2_l2_offset,      1, {FGTU_TEX2_L2_OFFSET, 	0}},
	{r_tex2_l3_offset,      1, {FGTU_TEX2_L3_OFFSET, 	0}},
	{r_tex2_l4_offset,      1, {FGTU_TEX2_L4_OFFSET, 	0}},
	{r_tex2_l5_offset,      1, {FGTU_TEX2_L5_OFFSET, 	0}},
	{r_tex2_l6_offset,      1, {FGTU_TEX2_L6_OFFSET, 	0}},
	{r_tex2_l7_offset,      1, {FGTU_TEX2_L7_OFFSET, 	0}},
	{r_tex2_l8_offset,      1, {FGTU_TEX2_L8_OFFSET, 	0}},
	{r_tex2_l9_offset,      1, {FGTU_TEX2_L9_OFFSET, 	0}},
	{r_tex2_l10_offset,     1, {FGTU_TEX2_L10_OFFSET, 	0}},
	{r_tex2_l11_offset,     1, {FGTU_TEX2_L11_OFFSET, 	0}},
	{r_tex2_min_level,      1, {FGTU_TEX2_MIN_LEVEL, 	0}},
	{r_tex2_max_level,      1, {FGTU_TEX2_MAX_LEVEL, 	0}},
	{r_tex2_base_addr,      1, {FGTU_TEX2_BASE_ADDR, 	0}},
	{r_tex3_ctrl,           1, {FGTU_TEX3_CTRL, 		0}},
	{r_tex3_usize,          1, {FGTU_TEX3_USIZE, 		0}},
	{r_tex3_vsize,          1, {FGTU_TEX3_VSIZE, 		0}},
	{r_tex3_psize,          1, {FGTU_TEX3_PSIZE, 		0}},
	{r_tex3_l1_offset,      1, {FGTU_TEX3_L1_OFFSET, 	0}},
	{r_tex3_l2_offset,      1, {FGTU_TEX3_L2_OFFSET, 	0}},
	{r_tex3_l3_offset,      1, {FGTU_TEX3_L3_OFFSET, 	0}},
	{r_tex3_l4_offset,      1, {FGTU_TEX3_L4_OFFSET, 	0}},
	{r_tex3_l5_offset,      1, {FGTU_TEX3_L5_OFFSET, 	0}},
	{r_tex3_l6_offset,      1, {FGTU_TEX3_L6_OFFSET, 	0}},
	{r_tex3_l7_offset,      1, {FGTU_TEX3_L7_OFFSET, 	0}},
	{r_tex3_l8_offset,      1, {FGTU_TEX3_L8_OFFSET, 	0}},
	{r_tex3_l9_offset,      1, {FGTU_TEX3_L9_OFFSET, 	0}},
	{r_tex3_l10_offset,     1, {FGTU_TEX3_L10_OFFSET, 	0}},
	{r_tex3_l11_offset,     1, {FGTU_TEX3_L11_OFFSET, 	0}},
	{r_tex3_min_level,      1, {FGTU_TEX3_MIN_LEVEL, 	0}},
	{r_tex3_max_level,      1, {FGTU_TEX3_MAX_LEVEL, 	0}},
	{r_tex3_base_addr,      1, {FGTU_TEX3_BASE_ADDR, 	0}},
	{r_tex4_ctrl,           1, {FGTU_TEX4_CTRL, 		0}},
	{r_tex4_usize,          1, {FGTU_TEX4_USIZE, 		0}},
	{r_tex4_vsize,          1, {FGTU_TEX4_VSIZE, 		0}},
	{r_tex4_psize,          1, {FGTU_TEX4_PSIZE, 		0}},
	{r_tex4_l1_offset,      1, {FGTU_TEX4_L1_OFFSET, 	0}},
	{r_tex4_l2_offset,      1, {FGTU_TEX4_L2_OFFSET, 	0}},
	{r_tex4_l3_offset,      1, {FGTU_TEX4_L3_OFFSET, 	0}},
	{r_tex4_l4_offset,      1, {FGTU_TEX4_L4_OFFSET, 	0}},
	{r_tex4_l5_offset,      1, {FGTU_TEX4_L5_OFFSET, 	0}},
	{r_tex4_l6_offset,      1, {FGTU_TEX4_L6_OFFSET, 	0}},
	{r_tex4_l7_offset,      1, {FGTU_TEX4_L7_OFFSET, 	0}},
	{r_tex4_l8_offset,      1, {FGTU_TEX4_L8_OFFSET, 	0}},
	{r_tex4_l9_offset,      1, {FGTU_TEX4_L9_OFFSET, 	0}},
	{r_tex4_l10_offset,     1, {FGTU_TEX4_L10_OFFSET, 	0}},
	{r_tex4_l11_offset,     1, {FGTU_TEX4_L11_OFFSET, 	0}},
	{r_tex4_min_level,      1, {FGTU_TEX4_MIN_LEVEL, 	0}},
	{r_tex4_max_level,      1, {FGTU_TEX4_MAX_LEVEL, 	0}},
	{r_tex4_base_addr,      1, {FGTU_TEX4_BASE_ADDR, 	0}},
	{r_tex5_ctrl,           1, {FGTU_TEX5_CTRL, 		0}},
	{r_tex5_usize,          1, {FGTU_TEX5_USIZE, 		0}},
	{r_tex5_vsize,          1, {FGTU_TEX5_VSIZE, 		0}},
	{r_tex5_psize,          1, {FGTU_TEX5_PSIZE, 		0}},
	{r_tex5_l1_offset,      1, {FGTU_TEX5_L1_OFFSET, 	0}},
	{r_tex5_l2_offset,      1, {FGTU_TEX5_L2_OFFSET, 	0}},
	{r_tex5_l3_offset,      1, {FGTU_TEX5_L3_OFFSET, 	0}},
	{r_tex5_l4_offset,      1, {FGTU_TEX5_L4_OFFSET, 	0}},
	{r_tex5_l5_offset,      1, {FGTU_TEX5_L5_OFFSET, 	0}},
	{r_tex5_l6_offset,      1, {FGTU_TEX5_L6_OFFSET, 	0}},
	{r_tex5_l7_offset,      1, {FGTU_TEX5_L7_OFFSET, 	0}},
	{r_tex5_l8_offset,      1, {FGTU_TEX5_L8_OFFSET, 	0}},
	{r_tex5_l9_offset,      1, {FGTU_TEX5_L9_OFFSET, 	0}},
	{r_tex5_l10_offset,     1, {FGTU_TEX5_L10_OFFSET, 	0}},
	{r_tex5_l11_offset,     1, {FGTU_TEX5_L11_OFFSET, 	0}},
	{r_tex5_min_level,      1, {FGTU_TEX5_MIN_LEVEL, 	0}},
	{r_tex5_max_level,      1, {FGTU_TEX5_MAX_LEVEL, 	0}},
	{r_tex5_base_addr,      1, {FGTU_TEX5_BASE_ADDR, 	0}},
	{r_tex6_ctrl,           1, {FGTU_TEX6_CTRL, 		0}},
	{r_tex6_usize,          1, {FGTU_TEX6_USIZE, 		0}},
	{r_tex6_vsize,          1, {FGTU_TEX6_VSIZE, 		0}},
	{r_tex6_psize,          1, {FGTU_TEX6_PSIZE, 		0}},
	{r_tex6_l1_offset,      1, {FGTU_TEX6_L1_OFFSET, 	0}},
	{r_tex6_l2_offset,      1, {FGTU_TEX6_L2_OFFSET, 	0}},
	{r_tex6_l3_offset,      1, {FGTU_TEX6_L3_OFFSET, 	0}},
	{r_tex6_l4_offset,      1, {FGTU_TEX6_L4_OFFSET, 	0}},
	{r_tex6_l5_offset,      1, {FGTU_TEX6_L5_OFFSET, 	0}},
	{r_tex6_l6_offset,      1, {FGTU_TEX6_L6_OFFSET, 	0}},
	{r_tex6_l7_offset,      1, {FGTU_TEX6_L7_OFFSET, 	0}},
	{r_tex6_l8_offset,      1, {FGTU_TEX6_L8_OFFSET, 	0}},
	{r_tex6_l9_offset,      1, {FGTU_TEX6_L9_OFFSET, 	0}},
	{r_tex6_l10_offset,     1, {FGTU_TEX6_L10_OFFSET, 	0}},
	{r_tex6_l11_offset,     1, {FGTU_TEX6_L11_OFFSET, 	0}},
	{r_tex6_min_level,      1, {FGTU_TEX6_MIN_LEVEL, 	0}},
	{r_tex6_max_level,      1, {FGTU_TEX6_MAX_LEVEL, 	0}},
	{r_tex6_base_addr,      1, {FGTU_TEX6_BASE_ADDR, 	0}},
	{r_tex7_ctrl,           1, {FGTU_TEX7_CTRL, 		0}},
	{r_tex7_usize,          1, {FGTU_TEX7_USIZE, 		0}},
	{r_tex7_vsize,          1, {FGTU_TEX7_VSIZE, 		0}},
	{r_tex7_psize,          1, {FGTU_TEX7_PSIZE, 		0}},
	{r_tex7_l1_offset,      1, {FGTU_TEX7_L1_OFFSET, 	0}},
	{r_tex7_l2_offset,      1, {FGTU_TEX7_L2_OFFSET, 	0}},
	{r_tex7_l3_offset,      1, {FGTU_TEX7_L3_OFFSET, 	0}},
	{r_tex7_l4_offset,      1, {FGTU_TEX7_L4_OFFSET, 	0}},
	{r_tex7_l5_offset,      1, {FGTU_TEX7_L5_OFFSET, 	0}},
	{r_tex7_l6_offset,      1, {FGTU_TEX7_L6_OFFSET, 	0}},
	{r_tex7_l7_offset,      1, {FGTU_TEX7_L7_OFFSET, 	0}},
	{r_tex7_l8_offset,      1, {FGTU_TEX7_L8_OFFSET, 	0}},
	{r_tex7_l9_offset,      1, {FGTU_TEX7_L9_OFFSET, 	0}},
	{r_tex7_l10_offset,     1, {FGTU_TEX7_L10_OFFSET, 	0}},
	{r_tex7_l11_offset,     1, {FGTU_TEX7_L11_OFFSET, 	0}},
	{r_tex7_min_level,      1, {FGTU_TEX7_MIN_LEVEL, 	0}},
	{r_tex7_max_level,      1, {FGTU_TEX7_MAX_LEVEL, 	0}},
	{r_tex7_base_addr,      1, {FGTU_TEX7_BASE_ADDR, 	0}},
	{r_color_key1,	   		1, {FGTU_COLOR_KEY1,		0}},
	{r_color_key2,	   		1, {FGTU_COLOR_KEY2,		0}},
	{r_color_key_yuv,		1, {FGTU_COLOR_KEY_YUV,	    0}},
	{r_color_key_mask,		1, {FGTU_COLOR_KEY_MASK,	0}},
	{r_pallete_addr,     	1, {FGTU_PALETTE_ADDR,		0}},
	{r_pallete_entry,    	1, {FGTU_PALETTE_ENTRY,	    0}},
	{r_vtxtex0_ctrl,     	1, {FGTU_VTXTEX0_CTRL,		0}},
	{r_vtxtex0_base_addr,	1, {FGTU_VTXTEX0_BASE_ADDR, 0}},
	{r_vtxtex1_ctrl,     	1, {FGTU_VTXTEX1_CTRL,		0}},
	{r_vtxtex1_base_addr,	1, {FGTU_VTXTEX1_BASE_ADDR, 0}},
	{r_vtxtex2_ctrl,     	1, {FGTU_VTXTEX2_CTRL,		0}},
	{r_vtxtex2_base_addr,	1, {FGTU_VTXTEX2_BASE_ADDR, 0}},
	{r_vtxtex3_ctrl,     	1, {FGTU_VTXTEX3_CTRL,		0}},
	{r_vtxtex3_base_addr,	1, {FGTU_VTXTEX3_BASE_ADDR, 0}}
};

const default_reg pf_default_regs[] =
 {
 	{r_scissor_xcord,    	1, {FGPF_SCISSOR_XCORD,	    0}},
 	{r_scissor_ycord,    	1, {FGPF_SCISSOR_YCORD,	    0}},
 	{r_alpha,            	1, {FGPF_ALPHA,			    0}},
 	{r_frontface_stencil,	1, {FGPF_FRONTFACE_STENCIL, 0}},
 	{r_backface_stencil, 	1, {FGPF_BACKFACE_STENCIL,	0}},
 	{r_depth,            	1, {FGPF_DEPTH,			    0}},
 	{r_blend_color,      	1, {FGPF_BLEND_COLOR,		0}},
 	{r_blend,            	1, {FGPF_BLEND,			    0}},
 	{r_logic_op,         	1, {FGPF_LOGIC_OP,			0}},
 	{r_color_mask,       	1, {FGPF_COLOR_MASK,		0}},
 	{r_depth_mask,       	1, {FGPF_STENCIL_DEPTH_MASK,0}},
 	{r_colorbuf_ctrl,    	1, {FGPF_COLORBUF_CTRL,	    0}},
  	{r_depthbuf_addr,    	1, {FGPF_DEPTHBUF_ADDR,	    0}},
 	{r_colorbuf_addr,    	1, {FGPF_COLORBUF_ADDR,	    0}},
    {r_colorbuf_width,   	1, {FGPF_COLORBUF_WIDTH,	0}}
};

 
/*****************************************************************************
 *  NON_STATIC GLOBALS 
 *****************************************************************************/


/*****************************************************************************
 *  STATIC GLOBALS 
 *****************************************************************************/
static FGL_Error 
_AssembleToPPM (
					unsigned int 	width,
					unsigned int 	height,
					unsigned int 	stride,
					FGL_PixelFormat	pixelFormat,
					void*			pSourceAddr,
					void*			pImageData
				);



/***************************************************************************** 
 * FUNCTIONS: Fimg3DInit
 * SYNOPSIS: 
 * EXTERNAL EFFECTS: 
 * PARAMETERS: NONE
 * RETURNS: NONE
 * ERRNO:
 *****************************************************************************/
#if 0
void 
fglSysInit(void)
{
	float	Ox;
	float	Oy;
	float	Pxb2;
	float	Pyb2;
	float	Fsnb2;
	float	Fpnb2;
	
	unsigned int uMinX;
	unsigned int uMaxX; 
	unsigned int uMinY;
	unsigned int uMaxY; 
	unsigned int uClipXVal;
	unsigned int uClipYVal;
	
	// Viewport set
	Pxb2 = m_Width/2.0f;
	Pyb2 = (-m_Height/2.0f);
	Ox = m_X + Pxb2;
	Oy = (LCD_VERTICAL_SIZE - m_Y) + Pyb2;
	Fsnb2 = (m_Far - m_Near)/2.0f;
	Fpnb2 = (m_Far + m_Near)/2.0f; 

	WRITEREG(FGPE_VIEWPORT_CENTER_XCOORD, *((unsigned int *)&Ox)); // Viewport center x coordinate	
	WRITEREG(FGPE_VIEWPORT_CENTER_YCOORD, *((unsigned int *)&Oy)); // Viewport center y coordinate	
	WRITEREG(FGPE_VIEWPORT_HALF_WIDTH, *((unsigned int *)&Pxb2)); // Half of viewport width
	WRITEREG(FGPE_VIEWPORT_HALF_HEIGHT, *((unsigned int *)&Pyb2)); // Half of viewport height
	WRITEREG(FGPE_DEPTH_HALF_DISTANCE, *((unsigned int *)&Fsnb2)); // Half of subtract far from near
	WRITEREG(FGPE_DEPTH_CENTER, *((unsigned int *)&Fpnb2)); // Half of add fat to near
	
	// Window X/Y clipping region set
	uMinX = (unsigned int)((m_X < 0.0) ? 0 : m_X);
	uMinX = (uMinX > SCREEN_WIDTH_SIZE) ? SCREEN_WIDTH_SIZE: uMinX;
	uMaxX = (unsigned int)(((m_X + m_Width) < 0.0) ? 0 : (m_X + m_Width));
	uMaxX = (uMaxX > SCREEN_WIDTH_SIZE) ? SCREEN_WIDTH_SIZE: uMaxX;

	uMinY = (unsigned int)((m_Y < 0.0) ? 0 : m_Y);
	uMinY = (uMinY > LCD_VERTICAL_SIZE) ? LCD_VERTICAL_SIZE: uMinY;
	
	uMaxY = (unsigned int)(((m_Y + m_Height) < 0.0) ? 0 : (m_Y + m_Height));
	//uMaxY = (uMaxY > SCREEN_HEIGHT_SIZE) ? SCREEN_HEIGHT_SIZE: uMaxY;
	uMaxY = (uMaxY > LCD_VERTICAL_SIZE) ? LCD_VERTICAL_SIZE: uMaxY;

	uClipXVal = (uMinX << XCLIP_MIN_SHIFT) | (uMaxX << XCLIP_MAX_SHIFT);
	uClipYVal = ((unsigned int)(LCD_VERTICAL_SIZE - uMaxY) << YCLIP_MIN_SHIFT) | 
				((unsigned int)(LCD_VERTICAL_SIZE - uMinY) << YCLIP_MAX_SHIFT);

	WRITEREG(FGRA_CLIP_XCORD, uClipXVal); // x window coordinate clipping
	WRITEREG(FGRA_CLIP_YCORD, uClipYVal); // y window coordinate clipping
	
	// Color buffer type set 0x000F2F35
	WRITEREG(FGPF_COLORBUF_CTRL, 0x000F2F35); // Color buffer pixel type set
	WRITEREG(FGPF_COLORBUF_ADDR, FIMG_COLOR_BUFFER); // Color buffer base address
	WRITEREG(FGPF_COLORBUF_WIDTH, SCREEN_WIDTH_SIZE); // Frame buffer width


	m_uWindowSurface = uWindowWidthSize * uWindowHeightSize * BYTES_PER_PIXEL;
	m_uFrameSurface = uScreenPhyHSize * uScreenPhyVSize * BYTES_PER_PIXEL;
	m_FBOffsize = uWindowWidthSize * BYTES_PER_PIXEL * (int)m_Y;

	drvsys_clear_buf((unsigned int*)FIMG_COLOR_BUFFER, m_uFrameSurface, 0x0);

	return;
}
#endif
/***************************************************************************** 
 * FUNCTIONS: Fimg3DInit
 * SYNOPSIS: 
 * EXTERNAL EFFECTS: 
 * PARAMETERS: NONE
 * RETURNS: NONE
 * ERRNO:
 *****************************************************************************/
void 
fglSysInit(float fX, float fY, float fWidth, float fHeight, float fNear, float fFar)
{
	//float	Pxb2;
	//float	Pyb2;
	unsigned int uMinX;
	unsigned int uMaxX; 
	unsigned int uMinY;
	unsigned int uMaxY; 
	unsigned int uClipXVal;
	unsigned int uClipYVal;
	
	// Viewport set
	//Pxb2 = fWidth/2.0f;
	//Pyb2 = (-fHeight/2.0f);

	unsigned int uWindowWidthSize;
	//unsigned int uWindowHeightSize;
	
	if(fWidth > 1.0 && fHeight > 1.0)
	{
    	uWindowWidthSize = (unsigned int)fWidth;
    	//uWindowHeightSize = (unsigned int)fHeight; 
	}
	else
	{
    	uWindowWidthSize = 240;
    	//uWindowHeightSize = 320; 
	}

    //unsigned int uScreenPhyHSize = SCREEN_WIDTH_SIZE;
    unsigned int uScreenPhyVSize = SCREEN_HEIGHT_SIZE;

	fglSetViewportParams(FGL_TRUE, fX, fY, fWidth, fHeight, uScreenPhyVSize);
	fglSetDepthRange(fNear, fFar);
	/*WRITEREGF(FGPE_VIEWPORT_CENTER_XCOORD, fX + Pxb2); // Viewport center x coordinate	
	WRITEREGF(FGPE_VIEWPORT_CENTER_YCOORD, (uScreenPhyVSize - fY) + Pyb2); // Viewport center y coordinate	
	WRITEREGF(FGPE_VIEWPORT_HALF_WIDTH, Pxb2); // Half of viewport width
	WRITEREGF(FGPE_VIEWPORT_HALF_HEIGHT, Pyb2); // Half of viewport height
	WRITEREGF(FGPE_DEPTH_HALF_DISTANCE, (fFar - fNear)/2.0f); // Half of subtract far from near
	WRITEREGF(FGPE_DEPTH_CENTER, (fFar + fNear)/2.0f); // Half of add fat to near
	*/
	// Window X/Y clipping region set
	uMinX = (unsigned int)((fX < 0.0) ? 0 : fX);
	uMinX = (uMinX > uWindowWidthSize) ? uWindowWidthSize: uMinX;
	uMaxX = (unsigned int)(((fX + fWidth) < 0.0) ? 0 : (fX + fWidth));
	uMaxX = (uMaxX > uWindowWidthSize) ? uWindowWidthSize: uMaxX;

	uMinY = (unsigned int)((fY < 0.0) ? 0 : fY);
	uMinY = (uMinY > uScreenPhyVSize) ? uScreenPhyVSize: uMinY;
	uMaxY = (unsigned int)(((fY + fHeight) < 0.0) ? 0 : (fY + fHeight));
	uMaxY = (uMaxY > uScreenPhyVSize) ? uScreenPhyVSize: uMaxY;

	uClipXVal = (uMinX << XCLIP_MIN_SHIFT) | (uMaxX << XCLIP_MAX_SHIFT);
	uClipYVal = ((unsigned int)(uScreenPhyVSize - uMaxY) << YCLIP_MIN_SHIFT) | 
				((unsigned int)(uScreenPhyVSize - uMinY) << YCLIP_MAX_SHIFT);

	WRITEREG(FGRA_CLIP_XCORD, uClipXVal);
	WRITEREG(FGRA_CLIP_YCORD, uClipYVal);
	
	// Color buffer type set 0x000F2F35
	WRITEREG(FGPF_COLORBUF_CTRL, 0x000F2F35); // Color buffer pixel type set
	WRITEREG(FGPF_COLORBUF_ADDR, FIMG_COLOR_BUFFER); // Color buffer base address
	WRITEREG(FGPF_COLORBUF_WIDTH, uWindowWidthSize); // Frame buffer width

	//m_uWindowSurface = uWindowWidthSize * uWindowHeightSize * BYTES_PER_PIXEL;
	//m_uFrameSurface = uScreenPhyHSize * uScreenPhyVSize * BYTES_PER_PIXEL;
	//m_FBOffsize = uWindowWidthSize * BYTES_PER_PIXEL * (int)fY;

	//drvsys_clear_buf((unsigned int*)FIMG_COLOR_BUFFER, m_uFrameSurface, 0x0);

	return;
}



/***************************************************************************** 
 * FUNCTIONS: fglSyscache_flush
 * SYNOPSIS: to enables the depth cache
 * EXTERNAL EFFECTS: global variable
 * PARAMETERS: NONE
 * RETURNS: NONE
 * ERRNO:
 *****************************************************************************/
void 
fglSysCacheFlush (void)
{
	//cheolkyoo-20051017@notice: Maybe to do something
    unsigned int val;

    while(FGL_ZERO != rFIMG_CACHE);

    val = rFIMG_CACHE;
    val |= ( BIT_CCACHE0_FLUSH | BIT_CCACHE1_FLUSH |
     		  BIT_ZCACHE0_FLUSH | BIT_ZCACHE1_FLUSH );
    rFIMG_CACHE = val;

    while(FGL_ZERO != rFIMG_CACHE);
    
}


/***************************************************************************** 
 * FUNCTIONS: fglSyswrite_reg
 * SYNOPSIS: to enables the depth cache
 * EXTERNAL EFFECTS: global variable
 * PARAMETERS: in : reg_addr - the address of the register base
 *	           in : offset - the register offset
 *             in : value - the value to write
 * RETURNS: NONE
 * ERRNO:
 *****************************************************************************/
void 
fglSysZCacheFlush (void)
{
	//cheolkyoo-20051017@notice: Maybe to do something
	//while( 0x0 != rFIMG_CACHE );
    rFIMG_CACHE |= ( BIT_ZCACHE0_FLUSH | BIT_ZCACHE1_FLUSH );
}


/***************************************************************************** 
 * FUNCTIONS: fglSysccache_flush
 * SYNOPSIS: to enables the depth cache
 * EXTERNAL EFFECTS: global variable
 * PARAMETERS: NONE
 * RETURNS: NONE
 * ERRNO:
 *****************************************************************************/
void 
fglSysCCacheFlush (void)
{
	//cheolkyoo-20051017@notice: Maybe to do something
	//while( 0x0 != rFIMG_CACHE );
    rFIMG_CACHE |= ( BIT_CCACHE0_FLUSH | BIT_CCACHE1_FLUSH );
}


/***************************************************************************** 
 * FUNCTIONS: fglSyscache_flush
 * SYNOPSIS: to enables the depth cache
 * EXTERNAL EFFECTS: global variable
 * PARAMETERS: NONE
 * RETURNS: NONE
 * ERRNO:
 *****************************************************************************/
void 
fglSysCacheClear (void)
{
	//cheolkyoo-20051017@notice: Maybe to do something
    unsigned int val;
    
    while(FGL_ZERO != rFIMG_CACHE);

    val = rFIMG_CACHE;
    val |= ( BIT_TCACHE0_CLEAR | BIT_TCACHE1_CLEAR | 
    		  BIT_VTCACHE_CLEAR );
    rFIMG_CACHE = val;
    
    while(FGL_ZERO != rFIMG_CACHE);
   
}


/***************************************************************************** 
 * FUNCTIONS: fglSystcache_clear
 * SYNOPSIS: to enables the depth cache
 * EXTERNAL EFFECTS: global variable
 * PARAMETERS: in : reg_addr - the address of the register base
 *	           in : offset - the register offset
 *             in : value - the value to write
 * RETURNS: NONE
 * ERRNO:
 *****************************************************************************/
void 
fglSysTexCacheClear (void)
{
	//cheolkyoo-20051017@notice: Maybe to do something
	//while( 0x0 != rFIMG_CACHE );
    rFIMG_CACHE |= ( BIT_TCACHE0_CLEAR | BIT_TCACHE1_CLEAR );
}


/***************************************************************************** 
 * FUNCTIONS: fglSysvtcache_clear
 * SYNOPSIS: to enables the depth cache
 * EXTERNAL EFFECTS: global variable
 * PARAMETERS: NONE
 * RETURNS: NONE
 * ERRNO:
 *****************************************************************************/
void 
fglSysVTexCacheClear (void)
{
	//cheolkyoo-20051017@notice: Maybe to do something
	//while( 0x0 != rFIMG_CACHE );
    rFIMG_CACHE |= BIT_VTCACHE_CLEAR;
}


/***************************************************************************** 
 * FUNCTIONS: fglSysread_reg
 * SYNOPSIS: to read a value from a device register.
 * EXTERNAL EFFECTS: global variable
 * PARAMETERS: in : uregaddr - the address of the register base
 *	           in : uoffset - the register offset
 *	           out: ph_3dstate - receives the private info pointer
 * RETURNS: the value read from the register
 * ERRNO:
 *****************************************************************************/
unsigned int 
fglSysReadReg( unsigned int addr )
{
	//assert((offset & 3) == 0);
	return (unsigned int)(*((volatile unsigned int*)addr));
}


/***************************************************************************** 
 * FUNCTIONS: fglSyswrite_reg
 * SYNOPSIS: to write a value to a device register
 * EXTERNAL EFFECTS: global variable
 * PARAMETERS: in : reg_addr - the address of the register base
 *	           in : offset - the register offset
 *             in : value - the value to write
 * RETURNS: NONE
 * ERRNO:
 *****************************************************************************/
void 
fglSysWriteReg (
					unsigned int	addr,
					unsigned int	val
				)
{
	//assert((uoffset & 3) == 0);
	//fimg_pdump_writereg(uoffset, uvalue);
	(*(volatile unsigned int*)(addr) = (unsigned int)(val));
}

/***************************************************************************** 
 * FUNCTIONS: fglSysmodify_reg
 * SYNOPSIS: to modify selected bits of a register
 * EXTERNAL EFFECTS: global variable
 * PARAMETERS: in : reg_addr - the address of the register base
 *	           in : offset - the register offset
 *	           in : mask - a bit mask of the bits to change
 *	           in : value - the value to write  
 * RETURNS: NONE
 * ERRNO:
 *****************************************************************************/
void 
fglSysModifyReg (
					unsigned int	addr,
					unsigned int	mask,
					unsigned int	val
				 )
{
	unsigned int reg_val = 0;

	//assert((uoffset & 3) == 0);
	//assert((uvalue & (~umask)) == 0);

	reg_val = fglSysReadReg(addr);
	reg_val = (reg_val & (~mask)) | val;
	fglSysWriteReg(addr, reg_val);
}


/***************************************************************************** 
 * FUNCTIONS: fglSyspolling_reg
 * SYNOPSIS: to perform hardware-specific setup for a rasterizer state.
 * EXTERNAL EFFECTS: global variable
 * PARAMETERS: in : 
 *	           out: ph_3dstate - receives the private info pointer
 * RETURNS: FIMG_NO_ERROR, if successful
 * ERRNO:
 *****************************************************************************/
void 
fglSysPollingReg (
						unsigned int addr, 
						unsigned int expect_val
				   )
{
	unsigned int uReadReg;
	do
	{
	    READREGP(addr, uReadReg); 
	}while(expect_val != uReadReg);

	//while(expect_val != READREG(addr));
}


/***************************************************************************** 
 * FUNCTIONS: fglSyspolling_mask
 * SYNOPSIS: to perform hardware-specific setup for a rasterizer state.
 * EXTERNAL EFFECTS: global variable
 * PARAMETERS: in : addr
 *	           in : expect_val
 *	           in : mask - receives the private info pointer
 * RETURNS: FIMG_NO_ERROR, if successful
 * ERRNO:
 *****************************************************************************/
void 
fglSysPollingMask (
						unsigned int 	addr, 
						unsigned int 	expect_val, 
						unsigned int 	mask
 			  	 	)
{
#if 0
     unsigned int readVal;
     readVal = READREG(addr);
     
     while ( expect_val != (readVal & mask) )
     {
   	     uartPrintf("Pipeline status = %08x\n", readVal);
   	     readVal = READREG(addr);
     }
#endif
#if 0
     unsigned int readVal;
     readVal = READREG(addr);
     
     while ( expect_val != (readVal & mask) )
     {
   	     //uartPrintf("1 Pipeline status = %08x\n", readVal);
   	     readVal = READREG(addr);
     }

     readVal = READREG(addr);
     
     while ( expect_val != (readVal & mask) )
     {
   	     //uartPrintf("1 Pipeline status = %08x\n", readVal);
   	     readVal = READREG(addr);
     }

     readVal = READREG(addr);
     
     while ( expect_val != (readVal & mask) )
     {
   	     //uartPrintf("1 Pipeline status = %08x\n", readVal);
   	     readVal = READREG(addr);
     }
#endif
#if 0
     readVal = READREG(addr);
     
     if( expect_val == (readVal & mask) )
     {
   	     uartPrintf("2 Pipeline status = %08x\n", readVal);
   	     //readVal = READREG(addr);
     }
     else
     {
   	     uartPrintf("[ERROR] 2 Pipeline status = %08x\n", readVal);
     }

     readVal = READREG(addr);
     
     if( expect_val == (readVal & mask) )
     {
   	     uartPrintf("3 Pipeline status = %08x\n", readVal);
   	     //readVal = READREG(addr);
     }
     else
     {
   	     uartPrintf("[ERROR] 3 Pipeline status = %08x\n", readVal);
     }
 #endif    
     //while ( expect_val != (READREG(addr) & mask) );

	unsigned int uReadReg;
	do
	{
	    READREGP(addr, uReadReg); 
	}while(expect_val != (uReadReg & mask));
}


/***************************************************************************** 
 * FUNCTION   : fglSystransfer_to_port
 * PURPOSE    : To copy a data buffer to a slaveport
 * PARAMETERS : in : psrc_data - Points to the first DWORD of source data
 *              in : pdst_port - Points to the slaveport
 *              in : size - The number of DWORDs to copy
 * RETURNS    : 
 *****************************************************************************/    

void 
fglSysDelay( unsigned int loop_cnt )
{
	unsigned int	i;
	unsigned int	count = 0;
	
	for(i=0; i < loop_cnt; i++)
		count++;
}



/***************************************************************************** 
 * FUNCTION   : fglSystransfer_to_port
 * PURPOSE    : To copy a data buffer to a slaveport
 * PARAMETERS : in : psrc_data - Points to the first DWORD of source data
 *              in : pdst_port - Points to the slaveport
 *              in : size - The number of DWORDs to copy
 * RETURNS    : 
 *****************************************************************************/
void 
fglSysTransferToPorts (
							unsigned int			*psrc_data, 
							volatile unsigned int	*pdst_port, 
							unsigned int 			size 
						)
{
	unsigned int offset = (unsigned int)psrc_data & 0x0000001F;
	unsigned int alloc_size = 0;

	if(psrc_data == FGL_NULL || pdst_port == FGL_NULL || size == 0)
	{
		// For debug
		//WRITEREG(0x00000104, 0xFFFFFFFF);
		return;
	}

	do
	{
		do
		{
			alloc_size = fglSysReservePortAlloc(size, 1);
		} while (alloc_size == 0);
		
		/* Write out dwords until we reach 32-byte alignment */
		if (offset > 0)
		{
			unsigned int before_align = 8 - (offset >> 2);

			if (before_align > alloc_size)
			{
				before_align = alloc_size;
				alloc_size = 0;
			}
			else
			{
				alloc_size -= before_align;
			}

			while (before_align-- > 0)
			{
				WRITEREG(pdst_port, *(psrc_data++));
				size --;
			}
			offset = (unsigned int)psrc_data & 0x0000001F;			
		}

		/* This works on aligned data */
		while (alloc_size & 0xFFFFFFF8)
		{

            unsigned int a0, a1, a2, a3, a4, a5, a6, a7;
			__asm
			{
    			LDMIA psrc_data!,{a0, a1, a2, a3, a4, a5, a6, a7}
    			STMIA pdst_port, {a0, a1, a2, a3, a4, a5, a6, a7}
    			//LDMIA psrc_data!,{v1-v8}
    			//STMIA pdst_port,{v1-v8}
			}

			alloc_size -= 8;
			size -= 8;
		}
	} while (size & 0xFFFFFFF8);

	/* Write out the last < 8 dwords */
	while (size-- > 0)
	{
		WRITEREG(pdst_port, *psrc_data);
		++psrc_data;
	}
}


static volatile bool gbOccurTCInt = false ; //Transfer Complete
static DMAC oDmac1;

void __irq Isr_DmaDone_Port(void)
{
	DMACH_ClearIntPending(&oDmac1);		
	//UART_Printf("D\n");
	gbOccurTCInt = true;
	INTC_ClearVectAddr();
}



void 
fglSysTransferToPort (
							unsigned int	*psrc_data, 
							unsigned int 	size 
						)
{
#if 1	// ORI
	//unsigned int *pdst_port = FGHI_FIFO_ENTRY; 

	unsigned int offset = (unsigned int)psrc_data & 0x0000001F;
	unsigned int alloc_size = 0;

	if(psrc_data == FGL_NULL || size == 0)
	{
		// For debug
		//WRITEREG(0x00000104, 0xFFFFFFFF);
		return;
	}

	do
	{
		do
		{
			alloc_size = fglSysReservePortAlloc(size, 1);
		} while (alloc_size == 0);
		
		/* Write out dwords until we reach 32-byte alignment */
		if (offset > 0)
		{
			unsigned int before_align = 8 - (offset >> 2);

			if (before_align > alloc_size)
			{
				before_align = alloc_size;
				alloc_size = 0;
			}
			else
			{
				alloc_size -= before_align;
			}

			while (before_align-- > 0)
			{
				WRITEREG(FGHI_FIFO_ENTRY, *(psrc_data++));
				size --;
			}
			offset = (unsigned int)psrc_data & 0x0000001F;			
		}

		/* This works on aligned data */
		while (alloc_size & 0xFFFFFFF8)
		{

            unsigned int a0, a1, a2, a3, a4, a5, a6, a7;
			__asm
			{
    			LDMIA psrc_data!,{a0, a1, a2, a3, a4, a5, a6, a7}
    			STMIA FGHI_FIFO_ENTRY, {a0, a1, a2, a3, a4, a5, a6, a7}
    			//LDMIA psrc_data!,{v1-v8}
    			//STMIA pdst_port,{v1-v8}
			}
			alloc_size -= 8;
			size -= 8;
		}
	} while (size & 0xFFFFFFF8);

	/* Write out the last < 8 dwords */
	while (size-- > 0)
	{
		WRITEREG(FGHI_FIFO_ENTRY, *psrc_data);
		++psrc_data;
	}
	
#else	// Increment FIFO Entry

	unsigned int offset = (unsigned int)psrc_data & 0x0000001F;
	unsigned int alloc_size = 0;
	u32 uQuotient, uRemainder;
	u32 uUnitSize = 16;
	u32 uAddSize = uUnitSize*4;
	u32 i, j;

	int src_data;
	int dst_data;

	src_data = (int)psrc_data;
	dst_data = (int)FGHI_FIFO_ENTRY;		
			
    	uQuotient = size/uUnitSize;
	uRemainder = size%uUnitSize;

	//UART_Printf("size=%d, uQuotient=%d, uRemainder=%d\n", size,uQuotient ,uRemainder);

	INTC_SetHandler(NUM_DMA1, Isr_DmaDone_Port);
	INTC_Enable(NUM_DMA1);
	DMAC_InitCh(DMA1, (DMA_CH)DMA_A, &oDmac1);

	#if 1	// ori
	if(psrc_data == FGL_NULL || size == 0)
	{
		return;
	}

	do
	{
		alloc_size = fglSysReservePortAlloc(size, uUnitSize);
	} while (alloc_size == 0);

	#endif
		
	if (offset > 0)
	{
		#if 0	// single

		for(i=0 ; i<=(size*4) ; i+=4)
		{
			WRITEREG((FGHI_FIFO_ENTRY+i), *(psrc_data++));
			//UART_Printf("FIFO=0x%x, data = 0x%x\n", (FGHI_FIFO_ENTRY+i), *(psrc_data));			
		}

		#elif 0	// size

		for(i=0 ; i<uQuotient ; i++)
		{
			for(j=0 ; j<uAddSize ; j+=4)
			{
				WRITEREG((FGHI_FIFO_ENTRY+j), *(psrc_data));
				psrc_data++;
				//UART_Printf("FIFO=0x%x, data = 0x%x\n", (FGHI_FIFO_ENTRY+j), *(psrc_data+j));			
			}
		}	

		for (i=0 ; i<uRemainder*4 ; i+=4)
		{
			WRITEREG((FGHI_FIFO_ENTRY+i), *(psrc_data));
			psrc_data++;
			//UART_Printf("uRemainder FIFO=0x%x, data = 0x%x\n", (FGHI_FIFO_ENTRY+i), *(psrc_data+i));			
		}

		#elif 0	// mem block copy

			#if 0	// MEM_BLOCK
			
			MEM_BlockCopy(src_data, dst_data, size);

			#elif 1	// SIZE
			
			for(i=0 ; i<uQuotient ; i++)
			{
				MEM_BlockCopy(src_data, dst_data, uUnitSize);
				src_data = src_data + uAddSize;
			}
		
			psrc_data += uUnitSize;
			Delay(1);

			do
			{
				alloc_size = fglSysReservePortAlloc(size, uRemainder);
			} while (alloc_size == 0);
			
			for (i=0 ; i<uRemainder*4 ; i+=4)
			{
				WRITEREG((FGHI_FIFO_ENTRY+i), *(psrc_data));
				psrc_data++;
			}	
			#endif

		#elif 1	// DMA

		u32 temp;

		//temp = psrc_data;
		
		for(i=0; i<uQuotient ; i++)
		{
			DMACH_Setup(DMA_A, 0x0, (src_data+i*uAddSize), 0, FGHI_FIFO_ENTRY, 0, WORD, uUnitSize, DEMAND, 
						  MEM, MEM2_3D, BURST4, &oDmac1);
			
			DMACH_Start(&oDmac1);
			
			while(!gbOccurTCInt); //Interrupt mode
			//psrc_data += uAddSize;

			Delay(1);
			//UART_Printf("DMA END!!!\n");

			do
			{
				alloc_size = fglSysReservePortAlloc(size, uRemainder);
			} while (alloc_size == 0);
			
		}

			psrc_data += uUnitSize;
			Delay(1);

			do
			{
				alloc_size = fglSysReservePortAlloc(size, uRemainder);
			} while (alloc_size == 0);


		

		for (i=0 ; i<uRemainder*4 ; i+=4)
		{
			WRITEREG((FGHI_FIFO_ENTRY+i), *(psrc_data));
			psrc_data++;
		}	
		#endif
	}

	#if 0
		while (alloc_size & 0xFFFFFFF8)
		{

            unsigned int a0, a1, a2, a3, a4, a5, a6, a7;
			__asm
			{
	    			LDMIA psrc_data!,{a0, a1, a2, a3, a4, a5, a6, a7}
	    			STMIA FGHI_FIFO_ENTRY, {a0, a1, a2, a3, a4, a5, a6, a7}
			}
			alloc_size -= 8;
			size -= 8;
		}


	while (size-- > 0)
	{
		WRITEREG(FGHI_FIFO_ENTRY, *psrc_data);
		++psrc_data;
	}
	#endif
	
#endif
}


/***************************************************************************** 
 * FUNCTION   : fglSysreserve_port_alloc
 * PURPOSE    : To request slaveport DWORD slots in order to manage flow control
 * PARAMETERS : in : request_slots - The number of DWORD slots requested
 *	            in : preferred_min - Minimum number of DWORDs to wait for 
 *                   (if possible)
 * RETURNS    : The number of DWORD slots allocated
 *****************************************************************************/
unsigned int 
fglSysReservePortAlloc(
							unsigned int	request_slots, 
							unsigned int	preferred_min 
						 )
{
	unsigned int allocate_slots = 0;
	volatile unsigned int free_slots;

	if(request_slots == 0)
	{
		return 0;
	}

	//free_slots = READREG(FGHI_FIFO_EMPTY_SPACE);
	READREGP(FGHI_FIFO_EMPTY_SPACE, free_slots);
	allocate_slots = FGL_MIN(free_slots, request_slots);

	while (allocate_slots < preferred_min)
	{
		//free_slots = READREG(FGHI_FIFO_EMPTY_SPACE);
		READREGP(FGHI_FIFO_EMPTY_SPACE, free_slots);
		allocate_slots = FGL_MIN(free_slots, request_slots);
	}

	return allocate_slots;
}



/******************************************************************************
 * DUMP SURFACE LOCAL FUNCTIONS
 ******************************************************************************/

/***********************************************************************
 * Function Name  : _WriteBMPFile
 * Inputs         : pFileName - The path of the BMP file
 *                  width - Horizontal resolution
 *					height - Vertical resolution
 *					pImageData - Pointer to the buffer that contains
 *                               the pixel data.
 * Outputs        : None
 * Returns        : mgl_no_err, if successful.
 * Description    : To write image data to a BMP file.
 ***********************************************************************/
FGL_Error 
fglSysWriteBMPFile(const char* pFileName, SurfaceInfo & surfaceInfo) 
{
    FGL_Error ret = FGL_ERR_UNKNOWN;
    
#ifdef DUMP_FRAME_BUFFER
	FILE *fp = 0;
	void *pImageData;
	unsigned int 	uColours = 1 << 24;
	unsigned int 	uImageSize = 0;
	unsigned short	uBitsPerPixel = 24;

	if (pFileName == FGL_NULL)
	{
		//printf("Need a File Name.");
		return ret;
	}

	/* Allocate temporal buffer to store pixel data in RGB 24-bit format */
	//uImageSize = surfaceInfo->width * surfaceInfo->height * 3;
	uImageSize = (surfaceInfo.width * surfaceInfo.height * 24) >> 3;
	//(UART_PrintflayInfo.uXRes * UART_PrintflayInfo.uYRes * 24) >> 3;
	pImageData = (void *)0x64000000;//malloc(uImageSize);

	if (pImageData == FGL_NULL)
	{
		//printf("Cannot allocate temporal buffer.");
		return FGL_ERR_UNKNOWN;
	}

	/* convert surface data to 24-bit RGB format */
	ret = fglSysAssembleToBMP (		surfaceInfo.width,
								surfaceInfo.height,
								surfaceInfo.stride,
								surfaceInfo.pixelFormat,
								surfaceInfo.pFBBaseAddr,
								pImageData
						  );

	if(ret != FGL_ERR_NO_ERROR)
	{
		//printf("Failed to produce screen dump file.");
		delete pImageData;
		return  FGL_ERR_UNKNOWN;
	}

	//daedoo
	#if SEMIHOSTING
	fp = fopen(pFileName, "wb");
	#endif

	//uImageSize =  (width * height * 24) >> 3;

	if (fp != 0)
	{
		unsigned int 	BMPHeader[(BMP_HEADER_SIZE + BMP_INFO_SIZE + 7) >> 2];
		unsigned char   *pHeaderStart = NULL;
		unsigned short  *pHeaderShort = NULL;
		unsigned int    *pHeaderLong = NULL;
		unsigned int 	uBytesWritten = 0;

		/* Write BMP header structure */

		/* To make sure that 32-bit accesses are 32-bit aligned, we have to be a bit tricky */
 		pHeaderStart = ((unsigned char*)&BMPHeader) + 2;	/* Start on 16-bit alignment */
 		pHeaderStart[0]	= 'B';
 		pHeaderStart[1]	= 'M';

		pHeaderLong = (unsigned int *)&pHeaderStart[2];
		*(pHeaderLong++)	= BMP_HEADER_SIZE + BMP_INFO_SIZE + uImageSize;	/* File size */

		pHeaderShort = (unsigned short*)pHeaderLong;
		*(pHeaderShort++) = 0;	/* Reserved */
		*(pHeaderShort++) = 0;	/* Reserved */

		pHeaderLong = (unsigned int *)pHeaderShort;
		*(pHeaderLong++) = BMP_HEADER_SIZE + BMP_INFO_SIZE;	/* Offset to image data from SOF */

		/* Append BMP info structure */
		*(pHeaderLong++) = BMP_INFO_SIZE;		/* Bitmap info header size */
		*(pHeaderLong++) = surfaceInfo.width;
		*(pHeaderLong++) = surfaceInfo.height;

		pHeaderShort = (unsigned short*)pHeaderLong;
		*(pHeaderShort++) = 1;				/* Colour planes */
		*(pHeaderShort++) = uBitsPerPixel;	/* uBitsPerPixel */

		pHeaderLong = (unsigned int *)pHeaderShort;
		*(pHeaderLong++) = 0;			/* No compression */
		*(pHeaderLong++) = uImageSize;
		*(pHeaderLong++) = 4096;		/* Arbitrary pixels per metre */
		*(pHeaderLong++) = 4096;
		*(pHeaderLong++) = uColours;	/* Number of colours */
		*(pHeaderLong) = 0;				/* Important colours (?) */
//DAEDOO
#if SEMIHOSTING
		uBytesWritten = fwrite(pHeaderStart, 1, BMP_HEADER_SIZE + BMP_INFO_SIZE, fp);
#endif

		if (uBytesWritten == (BMP_HEADER_SIZE + BMP_INFO_SIZE))
		{
#if SEMIHOSTING
			uBytesWritten = fwrite(pImageData, 1, uImageSize, fp);
#endif

			if (uBytesWritten == uImageSize)
			{
				ret	= FGL_ERR_NO_ERROR;
			}
		}
		else
		{
			ret	= FGL_ERR_INVALID_VALUE;
			//printf("Wrote wrong number of bytes to BMP file.");
		}
//daedoo
		#if SEMIHOSTING
		fclose(fp);
		#endif
	}
	else
	{
		ret	= FGL_ERR_INVALID_VALUE;
		//printf("Failed to open %s for writing screen dump.");
	}
#else
    ret	= FGL_ERR_NO_SUPPORT;
#endif /* End of DUMP_FRAME_BUFFER */
	return ret;
}

/***********************************************************************
 * Function Name  : _CompileBMPImage
 * Inputs         : width - Horizontal resolution
 *					height - Vertical resolution
 *					stride - Surface stride (width * Bpp)
 *					pixelFormat - Source surface pixel format
 *					pSourceAddr - Pointer to the source surface
 *					pImageData - Pointer to the buffer that will receive
 *                               the data.
 * Outputs        : None
 * Returns        : mgl_no_err, if successful.
 * Description    : To produce image data in BMP format.
 ***********************************************************************/
FGL_Error 
fglSysAssembleToBMP (
						unsigned int 	width,
						unsigned int 	height,
						unsigned int 	stride,
						FGL_PixelFormat	pixelFormat,
						void*			pSourceAddr,
						void*			pImageData
				)
{
	FGL_Error 		ret	= FGL_ERR_NO_ERROR;
	unsigned int 	uHorizCtr	= 0;
	unsigned int 	uVertCtr	= 0;
	unsigned int 	uSourceStep	= 0;
	unsigned char*	pSourceData	= NULL;
	unsigned char*	pDestData	= NULL;
	unsigned char	red	= 0;
	unsigned char	green = 0;
	unsigned char	blue = 0;

	uSourceStep = (32 >> 3);
	
	pDestData = (unsigned char *)pImageData;

	uVertCtr = height;

	do
	{
		--uVertCtr;
		pSourceData = ((unsigned char*)pSourceAddr + ((stride * 4) * uVertCtr));
			
		for (uHorizCtr = 0; uHorizCtr < width; ++uHorizCtr)
		{
			
			switch (pixelFormat)
			{
				case FGL_PIXEL_RGB565:
				{
					red	= (unsigned char)((pSourceData[1] & 0x0F8) | (pSourceData[1] >> 5));
					green = (unsigned char)((pSourceData[1] << 5) | ((pSourceData[0] >> 3) & 0x1C) |
								((pSourceData[1] >> 1) & 0x03));
					blue = (unsigned char)(((pSourceData[0] << 3) & 0xF8) | ((pSourceData[0] >> 2) & 0x07));
					break;
				}
				case FGL_PIXEL_RGB555:
				case FGL_PIXEL_ARGB1555:
				{
					red	= (unsigned char)(((pSourceData[1] << 1) & 0xF8) | ((pSourceData[1] >> 4) & 0x07));
					green = (unsigned char)((pSourceData[1] << 6) | ((pSourceData[0] >> 2) & 0x38) |
								((pSourceData[1] << 1) & 0x06) | (pSourceData[0] >> 7));
					blue = (unsigned char)((pSourceData[0] << 3) | ((pSourceData[0] >> 2) & 0x07));
					break;
				}
				case FGL_PIXEL_ARGB0888:
				{
					red	= pSourceData[2];
					green = pSourceData[1];
					blue = pSourceData[0];
					break;
				}
				case FGL_PIXEL_ARGB4444:
				{
					red	= (unsigned char)((pSourceData[1] << 4) | (pSourceData[1] & 0x0F));
					green = (unsigned char)((pSourceData[0] & 0xF0) | (pSourceData[0] >> 4));
					blue = (unsigned char)((pSourceData[0] << 4) | (pSourceData[0] & 0x0F));
					break;
				}
				case FGL_PIXEL_ARGB8888:
				{
					red	= pSourceData[2];
					green = pSourceData[1];
					blue = pSourceData[0];
					break;
				}
				default:
				{
					ret = FGL_ERR_INVALID_PARAMETER;
					break;
				}
			}

			*(pDestData++) = blue;
			*(pDestData++) = green;
			*(pDestData++) = red;

			pSourceData += uSourceStep;
		}

	} while (uVertCtr != 0);


	return ret;
}


/***********************************************************************
 * Function Name  : _WriteBMPFile
 * Inputs         : pFileName - The path of the BMP file
 *                  width - Horizontal resolution
 *					height - Vertical resolution
 *					pImageData - Pointer to the buffer that contains
 *                               the pixel data.
 * Outputs        : None
 * Returns        : mgl_no_err, if successful.
 * Description    : To write image data to a BMP file.
 ***********************************************************************/
FGL_Error 
fglSysWritePPMFile	(
                     	const char*		pFileName, 
                    	SurfaceInfo &   surfaceInfo 
					)
{
    FGL_Error ret = FGL_ERR_UNKNOWN;
	FILE *fp = 0;
	void *pImageData;
//	unsigned int 	uColours = 1 << 24;
	unsigned int 	uImageSize = 0;
//	unsigned short	uBitsPerPixel = 24;

	if (pFileName == NULL)
	{
		//printf("Need a File Name.");
		return ret;
	}

	/* Allocate temporal buffer to store pixel data in RGB 24-bit format */
	//uImageSize = surfaceInfo->width * surfaceInfo->height * 3;
	uImageSize = (surfaceInfo.width * surfaceInfo.height * 24) >> 3;
	//(UART_PrintflayInfo.uXRes * UART_PrintflayInfo.uYRes * 24) >> 3;
	pImageData = (void *)0x64000000;//malloc(uImageSize);

	if (pImageData == NULL)
	{
		//printf("Cannot allocate temporal buffer.");
		return FGL_ERR_UNKNOWN;
	}

	/* convert surface data to 24-bit RGB format */
	ret = _AssembleToPPM (		surfaceInfo.width,
								surfaceInfo.height,
								surfaceInfo.stride,
								surfaceInfo.pixelFormat,
								surfaceInfo.pFBBaseAddr,
								pImageData
						  );

	if(ret != FGL_ERR_NO_ERROR)
	{
		//printf("Failed to produce screen dump file.");
		delete pImageData;
		return  FGL_ERR_UNKNOWN;
	}

//daedoo
	#if SEMIHOSTING
	fp = fopen(pFileName, "wb");
	#endif

	if (fp != 0)
	{
		unsigned int 	uBytesWritten = 0;

//daedoo
#if SEMIHOSTING
		/* Write PPM header structure */
		fprintf(fp, "P6\n");
		fprintf(fp, "%0d %0d\n", surfaceInfo.width, surfaceInfo.height);
		fprintf(fp, "255\n");
		
		
		uBytesWritten = fwrite(pImageData, 1, uImageSize, fp);

		if (uBytesWritten == uImageSize)
		{
			ret	= FGL_ERR_NO_ERROR;
		}
		else
		{
			ret	= FGL_ERR_INVALID_VALUE;
			//printf("Wrote wrong number of bytes to BMP file.");
		}
//daedoo

		fclose(fp);
#endif
	}
	else
	{
		ret	= FGL_ERR_INVALID_VALUE;
		//printf("Failed to open %s for writing screen dump.");
	}

	return ret;
}

static FGL_Error 
_AssembleToPPM (
					unsigned int 	width,
					unsigned int 	height,
					unsigned int 	stride,
					FGL_PixelFormat	pixelFormat,
					void*			pSourceAddr,
					void*			pImageData
				)
{

	FGL_Error 		ret	= FGL_ERR_NO_ERROR;
	unsigned int 	uHorizCtr	= 0;
	unsigned int 	uVertCtr	= 0;
	unsigned int 	uSourceStep	= 0;
	unsigned char*	pSourceData	= NULL;
	unsigned char*	pDestData	= NULL;
	unsigned char	red	= 0;
	unsigned char	green = 0;
	unsigned char	blue = 0;

	switch (pixelFormat)
	{
		case FGL_PIXEL_RGB565:
		case FGL_PIXEL_RGB555:
		case FGL_PIXEL_ARGB1555:
		case FGL_PIXEL_ARGB4444:
		{
			uSourceStep = (16 >> 3);
			break;
		}
		case FGL_PIXEL_ARGB0888:
		case FGL_PIXEL_ARGB8888:
		{
			uSourceStep = (32 >> 3);
			break;
		}
		default:
		{
			return FGL_ERR_INVALID_PARAMETER;
		}
	}

	pDestData = (unsigned char *)pImageData;

	for(uVertCtr = 0; uVertCtr < height; uVertCtr++)
	{
		//--uVertCtr;
		pSourceData = ((unsigned char*)pSourceAddr + ((stride * 4) * uVertCtr));
			
		for (uHorizCtr = 0; uHorizCtr < width; ++uHorizCtr)
		{
			switch (pixelFormat)
			{
				case FGL_PIXEL_RGB565:
				{
					red	= (unsigned char)((pSourceData[1] & 0x0F8) | (pSourceData[1] >> 5));
					green = (unsigned char)((pSourceData[1] << 5) | ((pSourceData[0] >> 3) & 0x1C) |
								((pSourceData[1] >> 1) & 0x03));
					blue = (unsigned char)(((pSourceData[0] << 3) & 0xF8) | ((pSourceData[0] >> 2) & 0x07));
					break;
				}
				case FGL_PIXEL_RGB555:
				case FGL_PIXEL_ARGB1555:
				{
					red	= (unsigned char)(((pSourceData[1] << 1) & 0xF8) | ((pSourceData[1] >> 4) & 0x07));
					green = (unsigned char)((pSourceData[1] << 6) | ((pSourceData[0] >> 2) & 0x38) |
								((pSourceData[1] << 1) & 0x06) | (pSourceData[0] >> 7));
					blue = (unsigned char)((pSourceData[0] << 3) | ((pSourceData[0] >> 2) & 0x07));
					break;
				}
				case FGL_PIXEL_ARGB0888:
				{
					red	= pSourceData[0];
					green = pSourceData[1];
					blue = pSourceData[2];
					break;
				}
				case FGL_PIXEL_ARGB4444:
				{
					red	= (unsigned char)((pSourceData[1] << 4) | (pSourceData[1] & 0x0F));
					green = (unsigned char)((pSourceData[0] & 0xF0) | (pSourceData[0] >> 4));
					blue = (unsigned char)((pSourceData[0] << 4) | (pSourceData[0] & 0x0F));
					break;
				}
				case FGL_PIXEL_ARGB8888:
				{
					red	= pSourceData[2];
					green = pSourceData[1];
					blue = pSourceData[0];
					break;
				}
				default:
				{
					ret = FGL_ERR_INVALID_PARAMETER;
					break;
				}
			}

			*(pDestData++) = blue;
			*(pDestData++) = green;
			*(pDestData++) = red;

			pSourceData += uSourceStep;
		}
	}
	
	//} while (uVertCtr != 0);

	return ret;

}


void fglDumpContext(const char *pFileName)
{
	FILE *fp = 0;
	int i = 0;
	int size = 0;
//	unsigned int num_error = 0;
	unsigned int read_reg = 0;
	pdefault_reg p_default_reg = 0;

    pdefault_reg reg_groups[8] = 
    {
        (pdefault_reg)gb_default_regs,    
        (pdefault_reg)hi_default_regs,    
        (pdefault_reg)vs_default_regs,    
        (pdefault_reg)pe_default_regs,
        (pdefault_reg)ra_default_regs,    
        (pdefault_reg)ps_default_regs,    
        (pdefault_reg)tu_default_regs,    
        (pdefault_reg)pf_default_regs
    };
    
    unsigned int reg_size[8] = 
    {
        gb_reg_count,   
        hi_reg_count,   
        vs_reg_count,   
        pe_reg_count,
        ra_reg_count,   
        ps_reg_count,   
        tu_reg_count,   
        pf_reg_count
    };

//daedoo
	#if SEMIHOSTING
	fp = fopen(pFileName, "wb");
	#endif

	if (fp != 0)
	{
        
        for(i = 0; i < 8; i++)
        {
            //p_default_reg = (pdefault_reg)dump_reg_group[i].regs;
            //size = (unsigned int)dump_reg_group[i].size;
            p_default_reg = (pdefault_reg)reg_groups[i];
            size = reg_size[i];
       	    do
        	{
        		if(p_default_reg->writable)
        		{
                    //read_reg = READREG(p_default_reg->reg.addr);
                    READREGP(p_default_reg->reg.addr, read_reg);
                   // if(read_reg != 0)
//daedoo
             		#if SEMIHOSTING
                        fprintf(fp, "0x%08X 0x%08X\n", p_default_reg->reg.addr, read_reg);
			#endif
        		}

        		++p_default_reg;

        	} while(--size != 0);        
        }
	}

//daedoo	
	#if SEMIHOSTING
	fclose(fp);
	#endif
}

#if 1

void fglScalef(float * const pMat, float x, float y, float z)
{
	pMat[ 0] *= x;
	pMat[ 1] *= y;
	pMat[ 2] *= z;
	pMat[ 4] *= x;
	pMat[ 5] *= y;
	pMat[ 6] *= z;
	pMat[ 8] *= x;
	pMat[ 9] *= y;
	pMat[10] *= z;
	pMat[12] *= x;
	pMat[13] *= y;
	pMat[14] *= z;
}

void fglTranslatef(float * const pMat, float x, float y, float z)
{	
	pMat[ 3] = pMat[ 0] * x + pMat[ 1] * y + pMat[ 2] * z + pMat[ 3]; 
	pMat[ 7] = pMat[ 4] * x + pMat[ 5] * y + pMat[ 6] * z + pMat[ 7];
	pMat[11] = pMat[ 8] * x + pMat[ 9] * y + pMat[10] * z + pMat[11];	
}

void fglRotatef(float * const pMat, const float angle, float x, float y, float z)
{	
	float modval = sqrt(x*x + y*y + z*z); 
	
	float x1 = x/modval; 
	float y1 = y/modval; 
	float z1 = z/modval; 

	float mat[9];

	float fSine = (float)sin(angle); 
	float fCosine = (float)cos(angle); 
	float ci = 1 - fCosine; 

	mat[ 0] = (x1 * x1) * ci + fCosine;
	mat[ 1] = x1 * y1 * ci - z1 * fSine;
	mat[ 2] = x1 * z1 * ci + y1 * fSine;

	mat[ 3] = x1 * y1 * ci + z1 * fSine;
	mat[ 4] = (y1 * y1) * ci + fCosine;
	mat[ 5] = y1 * z1 * ci - x1 * fSine;

	mat[ 6] = x1 * z1 * ci - y1 * fSine ;
	mat[ 7] = y1 * z1 * ci + x1 * fSine ;
	mat[ 8] = (z1 * z1) * ci +  fCosine ;
	
	x1 = pMat[ 0] * mat[ 0] + pMat[ 1] * mat[ 3] + pMat[ 2] * mat[ 6];
	y1 = pMat[ 0] * mat[ 1] + pMat[ 1] * mat[ 4] + pMat[ 2] * mat[ 7];
	pMat[2] = pMat[ 0] * mat[ 2] + pMat[ 1] * mat[ 5] + pMat[ 2] * mat[ 8];
	pMat[0] = x1; 
	pMat[1] = y1;

	x1 = pMat[ 4] * mat[ 0] + pMat[ 5] * mat[ 3] + pMat[ 6] * mat[ 6];
	y1 = pMat[ 4] * mat[ 1] + pMat[ 5] * mat[ 4] + pMat[ 6] * mat[ 7];
	pMat[ 6] = pMat[ 4] * mat[ 2] + pMat[ 5] * mat[ 5] + pMat[ 6] * mat[ 8];
	pMat[ 4] = x1; 
	pMat[ 5] = y1;

	x1 = pMat[ 8] * mat[ 0] + pMat[ 9] * mat[ 3] + pMat[10] * mat[ 6];
	y1 = pMat[ 8] * mat[ 1] + pMat[ 9] * mat[ 4] + pMat[10] * mat[ 7];
	pMat[10] = pMat[ 8] * mat[ 2] + pMat[ 9] * mat[ 5] + pMat[10] * mat[ 8];
	pMat[ 8] = x1; 
	pMat[ 9] = y1;

	x1 = pMat[12] * mat[ 0] + pMat[13] * mat[ 3] + pMat[14] * mat[ 6];
	y1 = pMat[12] * mat[ 1] + pMat[13] * mat[ 4] + pMat[14] * mat[ 7];
	pMat[14] = pMat[12] * mat[ 2] + pMat[13] * mat[ 5] + pMat[14] * mat[ 8];
	pMat[12] = x1; 
	pMat[13] = y1;
}

void fglMultMatrixf(float * pRstMat, const float * const pMat1, const float * const pMat2)
{
	pRstMat[ 0] = pMat1[ 0] * pMat2[ 0] + pMat1[ 1] * pMat2[ 4] + pMat1[ 2] * pMat2[ 8] +pMat1[ 3] * pMat2[12];
	pRstMat[ 1] = pMat1[ 0] * pMat2[ 1] + pMat1[ 1] * pMat2[ 5] + pMat1[ 2] * pMat2[ 9] +pMat1[ 3] * pMat2[13];
	pRstMat[ 2] = pMat1[ 0] * pMat2[ 2] + pMat1[ 1] * pMat2[ 6] + pMat1[ 2] * pMat2[10] +pMat1[ 3] * pMat2[14];
	pRstMat[ 3] = pMat1[ 0] * pMat2[ 3] + pMat1[ 1] * pMat2[ 7] + pMat1[ 2] * pMat2[11] +pMat1[ 3] * pMat2[15];
                                                                  
	pRstMat[ 4] = pMat1[ 4] * pMat2[ 0] + pMat1[ 5] * pMat2[ 4] + pMat1[ 6] * pMat2[ 8] +pMat1[ 7] * pMat2[12];
	pRstMat[ 5] = pMat1[ 4] * pMat2[ 1] + pMat1[ 5] * pMat2[ 5] + pMat1[ 6] * pMat2[ 9] +pMat1[ 7] * pMat2[13];
	pRstMat[ 6] = pMat1[ 4] * pMat2[ 2] + pMat1[ 5] * pMat2[ 6] + pMat1[ 6] * pMat2[10] +pMat1[ 7] * pMat2[14];
	pRstMat[ 7] = pMat1[ 4] * pMat2[ 3] + pMat1[ 5] * pMat2[ 7] + pMat1[ 6] * pMat2[11] +pMat1[ 7] * pMat2[15];
                                           
	pRstMat[ 8] = pMat1[ 8] * pMat2[ 0] + pMat1[ 9] * pMat2[ 4] + pMat1[10] * pMat2[ 8] +pMat1[11] * pMat2[12];
	pRstMat[ 9] = pMat1[ 8] * pMat2[ 1] + pMat1[ 9] * pMat2[ 5] + pMat1[10] * pMat2[ 9] +pMat1[11] * pMat2[13];
	pRstMat[10] = pMat1[ 8] * pMat2[ 2] + pMat1[ 9] * pMat2[ 6] + pMat1[10] * pMat2[10] +pMat1[11] * pMat2[14];
	pRstMat[11] = pMat1[ 8] * pMat2[ 3] + pMat1[ 9] * pMat2[ 7] + pMat1[10] * pMat2[11] +pMat1[11] * pMat2[15];
                    
	pRstMat[12] = pMat1[12] * pMat2[ 0] + pMat1[13] * pMat2[ 4] + pMat1[14] * pMat2[ 8] +pMat1[15] * pMat2[12];
	pRstMat[13] = pMat1[12] * pMat2[ 1] + pMat1[13] * pMat2[ 5] + pMat1[14] * pMat2[ 9] +pMat1[15] * pMat2[13];
	pRstMat[14] = pMat1[12] * pMat2[ 2] + pMat1[13] * pMat2[ 6] + pMat1[14] * pMat2[10] +pMat1[15] * pMat2[14];
	pRstMat[15] = pMat1[12] * pMat2[ 3] + pMat1[13] * pMat2[ 7] + pMat1[14] * pMat2[11] +pMat1[15] * pMat2[15];
}

void fglMatrixIdentity(float * const pMat)
{
	pMat[ 0]=1.0f;	pMat[ 4]=0.0f;	pMat[ 8]=0.0f;	pMat[12]=0.0f;
	pMat[ 1]=0.0f;	pMat[ 5]=1.0f;	pMat[ 9]=0.0f;	pMat[13]=0.0f;
	pMat[ 2]=0.0f;	pMat[ 6]=0.0f;	pMat[10]=1.0f;	pMat[14]=0.0f;
	pMat[ 3]=0.0f;	pMat[ 7]=0.0f;	pMat[11]=0.0f;	pMat[15]=1.0f;
}

/****************************************************************************
 * Function Name  : MatrixTranspose
 * Inputs		  : *pMatIn
 * Outputs		  : *pMatOut
 * Globals used	  : None
 * Description    : Compute the transpose matrix of pMatIn.
****************************************************************************/
void fglMatrixTranspose (
							float * 	  const pMatOut,
							const float * const pMatIn
						)
{
	pMatOut[ 0]=pMatIn[ 0];	pMatOut[ 4]=pMatIn[ 1];	pMatOut[ 8]=pMatIn[ 2];	pMatOut[12]=pMatIn[ 3];
	pMatOut[ 1]=pMatIn[ 4];	pMatOut[ 5]=pMatIn[ 5];	pMatOut[ 9]=pMatIn[ 6];	pMatOut[13]=pMatIn[ 7];
	pMatOut[ 2]=pMatIn[ 8];	pMatOut[ 6]=pMatIn[ 9];	pMatOut[10]=pMatIn[10];	pMatOut[14]=pMatIn[11];
	pMatOut[ 3]=pMatIn[12];	pMatOut[ 7]=pMatIn[13];	pMatOut[11]=pMatIn[14];	pMatOut[15]=pMatIn[15];
}

/*****************************************************************************
 * Function Name  : PVRTMatrixOrthoRH
 * Inputs		  : w, h, zn, zf
 * Outputs		  : m
 * Globals used	  : None
 * Description    : Create an orthographic matrix.
*****************************************************************************/
void 
fglMatrixOrtho (
                	float *pMat,
                	const float w,
                	const float h,
                	const float zn,
                	const float zf
                )
{
	pMat[ 0] = 2 / w;
	pMat[ 1] = 0;
	pMat[ 2] = 0;
	pMat[ 3] = 0;

	pMat[ 4] = 0;
	pMat[ 5] = 2 / h;
	pMat[ 6] = 0;
	pMat[ 7] = 0;

	pMat[ 8] = 0;
	pMat[ 9] = 0;
	pMat[10] = 1 / (zn - zf);
	pMat[11] = zn / (zn - zf);
	//pMat[10] = 2 / (zn - zf);
	//pMat[11] = (zn + zf) / (zn - zf);

	pMat[12] = 0;
	pMat[13] = 0;
	pMat[14] = 0;
	pMat[15] = 1;
}
#endif

/****************************************************************************
 * Function Name  : MatrixTranspose
 * Inputs		  : *pMatIn
 * Outputs		  : *pMatOut
 * Globals used	  : None
 * Description    : High level function to swap the front and back buffers.
****************************************************************************/
//#define FIMG_LCDCON1    0x6ff10000

void fglSwapBuffer(unsigned int uSelect)
{

#if 1
	unsigned int  data;
	
	if(uSelect == 0 || uSelect == 1)
	{
	
		READREGP(FIMG_LCDCON1, data);
		data = (data & ~(1<<20)) | uSelect<<20;
		WRITEREG(FIMG_LCDCON1, data);
	}
#else

	unsigned int  data;
	
	if(uSelect == 0 || uSelect == 1)
	{
	
		READREGP(FIMG_LCDCON1, data);
		if(data&0xffdfffff)
			//WRITEREG(FIMG_LCDCON1, data&(0x1<<20));
			WRITEREG(FIMG_LCDCON1, data|(0x1<<20));
		else
			//WRITEREG(FIMG_LCDCON1, data|(0x1<<20));
			WRITEREG(FIMG_LCDCON1, data&(0x1<<20));
	}


#endif
}



unsigned int 
fglWriteVertexShaderConstFloat( 
                                    unsigned int offset, 
                                    unsigned int size, /* count of float */
                                    float *pdata
                               )
{
    unsigned int ConstFloatAddr = FGVS_CFLOAT_SADDR;
    
    ConstFloatAddr += (offset * 4);
    
    for(int i = 0; i < size; i++)
    {
        WRITEREGF(ConstFloatAddr, *pdata++);	    
        ConstFloatAddr += 4;
        offset++;
	}
	
	return offset;
}


unsigned int 
fglWriteFragmentShaderConstFloat( 
                                    unsigned int offset, 
                                    unsigned int size, /* count of float */
                                    float *pdata
                                )
{
    unsigned int ConstFloatAddr = FGPS_CFLOAT_SADDR;
    
    ConstFloatAddr += (offset * 4);
    
    for(int i = 0; i < size; i++)
    {
        WRITEREGF(ConstFloatAddr, *pdata++);	    
        ConstFloatAddr += 4;
        offset++;
	}
	
	return offset;
}

#define FGL_BITFIELD_MASK(fields) ((0xFFFFFFFF>>(31-(1?fields))) & (0xFFFFFFFF<<(0?fields)))

/* Extract a bitfield */
/* Use with 2nd arg as colon separated bitfield defn - no brackets round 2nd arg */
/* e.g. FGL_EXTRACT_BITFIELD(0x00003c00, 11:8) == c */
#define FGL_GET_BITFIELD(flags, fields) (((flags)&FGL_BITFIELD_MASK(fields))>>(0?fields))

void
fglEnableProbeWatchPoints(void)
{
    WRITEREG(FGWP_DEBUG_ENABLE, 1);
}

void
fglDisableProbeWatchPoints(void)
{
    WRITEREG(FGWP_DEBUG_ENABLE, 0);
}

void
fglProbeWatchPoints(void)
{
    unsigned int nWatchPointVal = 0;
    
    UART_Printf("\n********************************************************");
    UART_Printf("\n*      FIMG-3DSE Ver 1.x Probe Watch Points            *");
    UART_Printf("\n********************************************************");

    READREGP(FGWP_IN_HI_GRP0,    nWatchPointVal);
    UART_Printf("\n        Watch_InHI_Grp0     ........... 0x%08X", FGWP_IN_HI_GRP0);
    UART_Printf("\n        +        ATTR_Index ........... 0x%08X", FGL_GET_BITFIELD(nWatchPointVal, 31:16));
    UART_Printf("\n        +        ATTR_Count ........... 0x%08X", FGL_GET_BITFIELD(nWatchPointVal, 15:0));

    READREGP(FGWP_IN_HI_GRP1,    nWatchPointVal);
    UART_Printf("\n        Watch_InHI_Grp1     ........... 0x%08X", FGWP_IN_HI_GRP1);
    UART_Printf("\n        +        FullVSFIFO ........... 0x%08X", FGL_GET_BITFIELD(nWatchPointVal, 31:31));
    UART_Printf("\n        +      StallBusCtrl ........... 0x%08X", FGL_GET_BITFIELD(nWatchPointVal, 30:30));
    UART_Printf("\n        +         FlagBegin ........... 0x%08X", FGL_GET_BITFIELD(nWatchPointVal, 29:29));
    UART_Printf("\n        +         BC_Attrib ........... 0x%08X", FGL_GET_BITFIELD(nWatchPointVal, 28:28));
    UART_Printf("\n        +          BC_Index ........... 0x%08X", FGL_GET_BITFIELD(nWatchPointVal, 27:26));
    UART_Printf("\n        +        BC_BusCtrl ........... 0x%08X", FGL_GET_BITFIELD(nWatchPointVal, 25:24));
    UART_Printf("\n        + FIFOHOST_PntEntry ........... 0x%08X", FGL_GET_BITFIELD(nWatchPointVal, 23:22));
    UART_Printf("\n        +  FIFOHOST_PntBase ........... 0x%08X", FGL_GET_BITFIELD(nWatchPointVal, 21:20));
    UART_Printf("\n        +   FIFOHOST_Empty3 ........... 0x%08X", FGL_GET_BITFIELD(nWatchPointVal, 19:19));
    UART_Printf("\n        +   FIFOHOST_Empty2 ........... 0x%08X", FGL_GET_BITFIELD(nWatchPointVal, 18:18));
    UART_Printf("\n        +   FIFOHOST_Empty1 ........... 0x%08X", FGL_GET_BITFIELD(nWatchPointVal, 17:17));
    UART_Printf("\n        +   FIFOHOST_Empty0 ........... 0x%08X", FGL_GET_BITFIELD(nWatchPointVal, 16:16));
    UART_Printf("\n        +    FIFOHOST_Full3 ........... 0x%08X", FGL_GET_BITFIELD(nWatchPointVal, 15:15));
    UART_Printf("\n        +    FIFOHOST_Full2 ........... 0x%08X", FGL_GET_BITFIELD(nWatchPointVal, 14:14));
    UART_Printf("\n        +    FIFOHOST_Full1 ........... 0x%08X", FGL_GET_BITFIELD(nWatchPointVal, 13:13));
    UART_Printf("\n        +    FIFOHOST_Full0 ........... 0x%08X", FGL_GET_BITFIELD(nWatchPointVal, 12:12));
    UART_Printf("\n        +    ATTR_PntAttrib ........... 0x%08X", FGL_GET_BITFIELD(nWatchPointVal, 11:8));
    UART_Printf("\n        +        ATTR_Count ........... 0x%08X", FGL_GET_BITFIELD(nWatchPointVal, 7:0));

    READREGP(FGWP_IN_VC_VS,      nWatchPointVal);
    UART_Printf("\n        Watch_InVC_VS ................. 0x%08X", FGWP_IN_VC_VS);
    UART_Printf("\n        +           WatchVC ........... 0x%08X", FGL_GET_BITFIELD(nWatchPointVal, 30:16));
    UART_Printf("\n        +       AttribValid ........... 0x%08X", FGL_GET_BITFIELD(nWatchPointVal, 30:30));
    UART_Printf("\n        +        InAttribRd ........... 0x%08X", FGL_GET_BITFIELD(nWatchPointVal, 29:29));
    UART_Printf("\n        +       FifoOutFull ........... 0x%08X", FGL_GET_BITFIELD(nWatchPointVal, 28:28));
    UART_Printf("\n        +         NumAttrib ........... 0x%08X", FGL_GET_BITFIELD(nWatchPointVal, 27:24));
    UART_Printf("\n        +       FifoOutSlot ........... 0x%08X", FGL_GET_BITFIELD(nWatchPointVal, 23:20));
    UART_Printf("\n        +       PntTailSlot ........... 0x%08X", FGL_GET_BITFIELD(nWatchPointVal, 19:16));
    UART_Printf("\n        +           WatchVS ........... 0x%08X", FGL_GET_BITFIELD(nWatchPointVal, 13:0));

    READREGP(FGWP_IN_PE,         nWatchPointVal);
    UART_Printf("\n        Watch_InPE .................... 0x%08X", FGWP_IN_PE);
    UART_Printf("\n        +           WatchPE ........... 0x%08X", FGL_GET_BITFIELD(nWatchPointVal, 18:0));

    READREGP(FGWP_IN_TSE_RA,     nWatchPointVal);
    UART_Printf("\n        Watch_InTSE_RA ................ 0x%08X", FGWP_IN_TSE_RA);
    UART_Printf("\n        +          WatchTSE ........... 0x%08X", FGL_GET_BITFIELD(nWatchPointVal, 22:16));
    UART_Printf("\n        +           WatchRA ........... 0x%08X", FGL_GET_BITFIELD(nWatchPointVal, 7:0));

    READREGP(FGWP_IN_PS0,        nWatchPointVal);
    UART_Printf("\n        Watch_InPS0 ................... 0x%08X", FGWP_IN_PS0);
    UART_Printf("\n        +          WatchPS0 ........... 0x%08X", FGL_GET_BITFIELD(nWatchPointVal, 31:0));

    READREGP(FGWP_IN_PS1,        nWatchPointVal);
    UART_Printf("\n        Watch_InPS1 ................... 0x%08X", FGWP_IN_PS1);
    UART_Printf("\n        +          WatchPS1 ........... 0x%08X", FGL_GET_BITFIELD(nWatchPointVal, 31:0));

    READREGP(FGWP_IN_VTU_TU,     nWatchPointVal);
    UART_Printf("\n        Watch_InVTU_TU ................ 0x%08X", FGWP_IN_VTU_TU);
    UART_Printf("\n        +          WatchVTU ........... 0x%08X", FGL_GET_BITFIELD(nWatchPointVal, 18:16));
    UART_Printf("\n        +         WatchTU_0 ........... 0x%08X", FGL_GET_BITFIELD(nWatchPointVal, 11:8));
    UART_Printf("\n        +         WatchTU_1 ........... 0x%08X", FGL_GET_BITFIELD(nWatchPointVal, 3:0));

    READREGP(FGWP_IN_TCACHE,     nWatchPointVal);
    UART_Printf("\n        Watch_InTCache ................ 0x%08X", FGWP_IN_HI_GRP0);
    UART_Printf("\n        +     WatchTL2Cache ........... 0x%08X", FGL_GET_BITFIELD(nWatchPointVal, 27:24));
    UART_Printf("\n        +        WatchTCArb ........... 0x%08X", FGL_GET_BITFIELD(nWatchPointVal, 18:16));
    UART_Printf("\n        +   WatchTL1Cache_1 ........... 0x%08X", FGL_GET_BITFIELD(nWatchPointVal, 11:8));
    UART_Printf("\n        +   WatchTL1Cache_1 ........... 0x%08X", FGL_GET_BITFIELD(nWatchPointVal, 3:0));

    READREGP(FGWP_IN_PF,         nWatchPointVal);
    UART_Printf("\n        Watch_InPF .................... 0x%08X", FGWP_IN_PF);
    UART_Printf("\n        +          WatchPF1 ........... 0x%08X", FGL_GET_BITFIELD(nWatchPointVal, 25:16));
    UART_Printf("\n        +          WatchPF0 ........... 0x%08X", FGL_GET_BITFIELD(nWatchPointVal, 9:0));

    READREGP(FGWP_IN_CZCACHE,    nWatchPointVal);
    UART_Printf("\n        Watch_InCZCache ............... 0x%08X", FGWP_IN_CZCACHE);
    UART_Printf("\n        +       WatchCCache ........... 0x%08X", FGL_GET_BITFIELD(nWatchPointVal, 29:16));
    UART_Printf("\n        +       WatchZCache ........... 0x%08X", FGL_GET_BITFIELD(nWatchPointVal, 13:0));

    READREGP(FGWP_IN_AXIRB,      nWatchPointVal);
    UART_Printf("\n        Watch_InAXIArb ................ 0x%08X", FGWP_IN_AXIRB);
    UART_Printf("\n        +  WatchAXIArbiter2 ........... 0x%08X", FGL_GET_BITFIELD(nWatchPointVal, 21:16));
    UART_Printf("\n        +  WatchAXIArbiter1 ........... 0x%08X", FGL_GET_BITFIELD(nWatchPointVal, 13:8));
    UART_Printf("\n        +  WatchAXIArbiter0 ........... 0x%08X", FGL_GET_BITFIELD(nWatchPointVal, 5:0));

    READREGP(FGWP_IN_DMA,        nWatchPointVal);
    UART_Printf("\n        Watch_InDMA ................... 0x%08X", FGWP_IN_DMA);
    UART_Printf("\n        +  WatchDMA_TC[9:3] ........... 0x%08X", FGL_GET_BITFIELD(nWatchPointVal, 31:16));
    UART_Printf("\n        +    WatchDMA_TC[0] ........... 0x%08X", FGL_GET_BITFIELD(nWatchPointVal, 15:0));
    UART_Printf("\n        +      WatchDMA_PF1 ........... 0x%08X", FGL_GET_BITFIELD(nWatchPointVal, 15:0));
    UART_Printf("\n        +      WatchDMA_PF0 ........... 0x%08X", FGL_GET_BITFIELD(nWatchPointVal, 15:0));

    UART_Printf("\n********************************************************\n");

}

//#undef FGL_BITFIELD_MASK(fields)
//#undef FGL_GET_BITFIELD(flags, fields)
