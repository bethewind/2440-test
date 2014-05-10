/*******************************************************************************
 *
 *	ViP model for FIMGSE
 *		version 1.0
 *
 *	SFRegRwTest.c
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
 *	2006.  1. 28 by cheolkyoo.kim
 *
 *  Description -
 *
 *	$RCSfile: SFRegRwTest.cpp,v $
 *	$Revision: 1.7 $
 *	$Author: cheolkyoo.kim $
 *	$Date: 2006/05/08 05:37:19 $
 *	$Locker:  $
 *
 *	$Source: C:/CVS/CVSrepository/FIMG-3DSE_SW/fimg3dse_fpga/fimg3d/src/SFRegRwTest.cpp,v $
 *	$State: Exp $
 *	$Log: SFRegRwTest.cpp,v $
 *	Revision 1.7  2006/05/08 05:37:19  cheolkyoo.kim
 *	no message
 *
 *	Revision 1.6  2006/05/08 05:34:27  cheolkyoo.kim
 *	no message
 *
 *	Revision 1.5  2006/04/13 11:54:17  cheolkyoo.kim
 *	no message
 *
 *	Revision 1.4  2006/04/05 04:06:09  cheolkyoo.kim
 *	Replaced ~_XCORD and ~_YCORD to ~_XCOORD and ~_YCOORD.
 *
 *	Revision 1.3  2006/04/05 03:31:23  cheolkyoo.kim
 *	Modify FGPE_VIEWPORT_CENTER_XCOORD to FGPE_VIEWPORT_CENTER_XCOORD
 *
 *	Revision 1.2  2006/03/30 04:27:44  cheolkyoo.kim
 *	Thomas-20060330@comment: Add to code using directive WIN32_VIP for VIP
 *
 *	Revision 1.1  2006/03/10 08:29:08  cheolkyoo.kim
 *	Initial import of FIMG-3DSE_SW package
 *
 ******************************************************************************/
/****************************************************************************
 *  INCLUDES
 ****************************************************************************/
#include "stdio.h"
#include "system.h"
#include "library.h"
#include "register.h"
#include "Fimg3DTest.h"
#include "Defines.h"
#include "def.h"

/****************************************************************************
 *  DEFINES
 ****************************************************************************/

unsigned int sfr_rw_test(pdefault_reg p_default_reg, unsigned int size);
unsigned int shader_slot_test(pshader_slot p_shader_slot, unsigned int size);

int SFRegRwTest(void)

{
	int sel;
	int ret = 0;
	int slot_ret = 0;
	
	const char *rw_menu[]=
	{
	    "EXIT",
	    "Global Registers",
	    "Host Interface Registers",
	    "Vertex Shader Registers & Memory Slots",
        "Primitive Engine Registers",
        "Raster Engine Registers",
        "Pixel Shader Registers & Momory Slots",
        "Texture Unit Registers",
        "Per-Fragment Registers",
        0
	};

    default_reg gb_default_regs[] =
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


    default_reg hi_default_regs[] =
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


    default_reg vs_default_regs[] =
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


    shader_slot vs_shader_slots[] =
    {
    	{r_shader_instruction_slot,	{FGVS_INSTMEM_SADDR,	FGVS_INSTMEM_EADDR, FGVS_INSTMEM_SIZE}},
    	{r_shader_const_float_slot, {FGVS_CFLOAT_SADDR,		FGVS_CFLOAT_EADDR,	FGVS_CFLOAT_SIZE}},
    	{r_shader_const_int_slot,	{FGVS_CINT_SADDR, 		FGVS_CINT_EADDR, 	FGVS_CINT_SIZE}},
    	{r_shader_const_bool_slot,  {FGVS_CBOOL_SADDR, 		FGVS_CBOOL_EADDR, 	FGVS_CBOOL_SIZE}}
    };


    default_reg pe_default_regs[] =
    {
    	{r_vtx_context,			  1, {FGPE_VTX_CONTEXT,			    0}},
    	{r_viewport_center_xcord, 1, {FGPE_VIEWPORT_CENTER_XCOORD,  0}},
    	{r_viewport_center_ycord, 1, {FGPE_VIEWPORT_CENTER_YCOORD,  0}},
    	{r_viewport_half_width,	  1, {FGPE_VIEWPORT_HALF_WIDTH,	    0}},
    	{r_viewport_half_height,  1, {FGPE_VIEWPORT_HALF_HEIGHT, 	0}},
    	{r_half_depth_range,      1, {FGPE_DEPTH_HALF_DISTANCE,	    0}},
    	{r_average_near_far, 	  1, {FGPE_DEPTH_CENTER,			0}}
    };

    default_reg ra_default_regs[] =
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


    default_reg ps_default_regs[] =
    {
    	{r_exe_mode,		1, {FGPS_EXE_MODE,		0}},
    	{r_pc_start,		1, {FGPS_PC_START,		0}},
    	{r_pc_end,			1, {FGPS_PC_END,		0}},
    	{r_pc_copy,			1, {FGPS_PC_COPY,		0}},
    	{r_ps_attrib_num, 	1, {FGPS_ATTRIB_NUM,	0}},
    	{r_inbuf_status,	0, {FGPS_INBUF_STATUS,	0}}
    };


    shader_slot ps_shader_slots[] =
    {
    	{r_shader_instruction_slot,	{FGPS_INSTMEM_SADDR,	FGPS_INSTMEM_EADDR, FGPS_INSTMEM_SIZE}},
    	{r_shader_const_float_slot, {FGPS_CFLOAT_SADDR,		FGPS_CFLOAT_EADDR,	FGPS_CFLOAT_SIZE}},
    	{r_shader_const_int_slot,	{FGPS_CINT_SADDR, 		FGPS_CINT_EADDR, 	FGPS_CINT_SIZE}},
    	{r_shader_const_bool_slot,  {FGPS_CBOOL_SADDR, 		FGPS_CBOOL_EADDR, 	FGPS_CBOOL_SIZE}}
    };


    default_reg tu_default_regs[] =
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

    default_reg pf_default_regs[] =
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


	while(1)
	{
		UART_Printf("\nSFR, Memory Slot Read/Write Test Vector\n");
		for (int i=0; (int)rw_menu[i]!=0; i++)
			UART_Printf("%2d: %s\n", i, rw_menu[i]);

		UART_Printf("\nSelect the function to test : ");
		sel = UART_GetIntNum();
		UART_Printf("\n");

		if(sel == 0)
		    break;

		switch(sel)
		{
		case 1:
		    UART_Printf("Global register R/W test start. \n");
	        ret = sfr_rw_test((pdefault_reg)gb_default_regs, (unsigned int)gb_reg_count);
	        UART_Printf("Global register R/W test end. \n");
			break;
		case 2:
        	UART_Printf("Host Interface register R/W test start. \n");
        	ret = sfr_rw_test((pdefault_reg)hi_default_regs, (unsigned int)hi_reg_count);
        	UART_Printf("Host Interface register R/W test end. \n");
			break;
		case 3:
        	UART_Printf("Vertex shader contol register R/W test start.\n");
        	ret = sfr_rw_test((pdefault_reg)vs_default_regs, (unsigned int)vs_reg_count);
         	UART_Printf("Host Interface register R/W test end. \n");
       	    UART_Printf("Vertex shader memory slots R/W test start.\n");
       	    slot_ret = shader_slot_test((pshader_slot)vs_shader_slots, (unsigned int)r_shader_count);
       	    UART_Printf("Vertex shader memory slots R/W test end.\n");
			break;
		case 4:
        	UART_Printf("Primitive engine register R/W test start. \n");
        	ret = sfr_rw_test((pdefault_reg)pe_default_regs, (unsigned int)pe_reg_count);
        	UART_Printf("Primitive engine register R/W test end. \n");
			break;
		case 5:
        	UART_Printf("Raster engine register R/W test start. \n");
        	ret = sfr_rw_test((pdefault_reg)ra_default_regs, (unsigned int)ra_reg_count);
        	UART_Printf("Raster engine register R/W test end. \n");
			break;
		case 6:
        	UART_Printf("Pixel shader contol register & memory slots R/W test start.\n");
        	ret = sfr_rw_test((pdefault_reg)ps_default_regs, (unsigned int)ps_reg_count);
        	UART_Printf("Pixel shader contol register & memory slots R/W test end.\n");
        	UART_Printf("Pixel shader memory slots R/W test start.\n");
           	slot_ret = shader_slot_test((pshader_slot)ps_shader_slots, (unsigned int)r_shader_count);
       	    UART_Printf("Pixel shader memory slots R/W test end.\n");
			break;
		case 7:
        	UART_Printf("Texture unit register R/W test start. \n");
        	ret = sfr_rw_test((pdefault_reg)tu_default_regs, (unsigned int)tu_reg_count);
        	UART_Printf("Texture unit register R/W test end. \n");
			break;
		case 8:
        	UART_Printf("Per-fragment register R/W test start. \n");
        	ret = sfr_rw_test((pdefault_reg)pf_default_regs, (unsigned int)pf_reg_count);
        	UART_Printf("Per-fragment register R/W test end. \n");
			break;
		default:
			break;
		}

    	if(ret > 0 )
    	{
    		UART_Printf("Total %d error occurred to %s.\n", ret, rw_menu[sel]);
    		ret = 0;
    	}

    	if(slot_ret != NO_ERROR)
    	{
    		UART_Printf("Break out, Error occured to 0x%8X in %s.\n", slot_ret, rw_menu[sel]);
    	}
    	else
    	{
    		UART_Printf("Memory slot r/w test in %s ----------------------- OK.\n", rw_menu[sel]);
    	}

	}

	return NO_ERROR;
}


unsigned int sfr_rw_test(pdefault_reg p_default_reg, unsigned int size)
{
	unsigned int num_error = 0;

	do
	{
		if(p_default_reg->writable)
		{

    		WRITEREG(p_default_reg->reg.addr, p_default_reg->reg.val);

    		if(p_default_reg->reg.val != READREG(p_default_reg->reg.addr))
    		{
    			UART_Printf(">>>0x%8X ---------------------- FAIL\n", p_default_reg->reg.addr);
    			num_error++;
    		}
    		else {
    		    UART_Printf("0x%8X ---------------------- OK\n", p_default_reg->reg.addr);
    		}
		}
		else {
		    UART_Printf("0x%8X ---------------------- ONLY READ\n", p_default_reg->reg.addr);
		}

		++p_default_reg;

	} while(--size != 0);

	return num_error;
}


unsigned int shader_slot_test(pshader_slot p_shader_slot, unsigned int size)
{
	unsigned int mem_size;
	unsigned int start_addr;
	unsigned int comp_value = 0;
	unsigned int fault_addr = 0;

	do
	{
		mem_size = p_shader_slot->mem.size;
		start_addr = p_shader_slot->mem.start_addr;

		switch(p_shader_slot->index)
		{
		case r_shader_instruction_slot:
		case r_shader_const_float_slot:
			comp_value = 0xFFFFFFFF;
			break;

		case r_shader_const_int_slot:
			comp_value = 0x00FFFFFF;
			break;
		case r_shader_const_bool_slot:
			comp_value = 0x0000FFFF;
			break;
		}

		do
		{
			WRITEREG(start_addr, comp_value);

			if((start_addr & 0x0000000F) != 0xC )
			{
				if(comp_value != READREG(start_addr))
			    {
    				fault_addr = start_addr;
    				return fault_addr;
				}
			}

			start_addr += 4;

		} while(--mem_size != 0);

		++p_shader_slot;

	} while(--size != 0);

	return NO_ERROR;
}
