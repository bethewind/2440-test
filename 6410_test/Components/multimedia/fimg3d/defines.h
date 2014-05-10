/******************************************************************************
*
* NAME         : fimg_defs.h
* TITLE        : FIMGSE ver. 1.0 Low Level API
* AUTHOR       : Thomas, Kim
* CREATED      : 12 Sep 2005
*
* COPYRIGHT    : Copyright(c) 2005 by Samsung Electronics Limited. All rights
*                reserved.  No part of this software, either material or
*                conceptual may be copied or distributed, transmitted,
*                transcribed, stored in a retrieval system or translated into
*                any human or computer language in any form by any means,
*                electronic, mechanical, manual or other-wise, or disclosed
*                to third parties without the express written permission of
*                Samsung Electronics. Semiconductor Business, System LSI
*                Division, Mobile Solution Development, Graphics IP Team
*                in AP.
*
* DESCRIPTION  : FIMGSE Hardware Definitions
*
* PLATFORM     : ALL
* HISTORY      : 2005. 9. 12 created
* CVS
* $RCSfile: fimg_defs.h,v $
* $Revision: 1.3 $
* $Author: inhosens.lee $
* $Date: 2006/06/02 00:43:41 $
* $Locker:  $
*
* $Source: C:/CVS/CVSrepository/FIMG-3DSE_SW/fimg3dse_fpga/fimg3d/common/fimg_defs.h,v $
* $State: Exp $
* $Log: fimg_defs.h,v $
* Revision 1.3  2006/06/02 00:43:41  inhosens.lee
* adding FGRA registers for _FIMG_V_1_1
*
* Revision 1.1  2006/03/10 08:29:08  cheolkyoo.kim
* Initial import of FIMG-3DSE_SW package
*
*****************************************************************************/
#if !defined(__FIMG_DEFS_H__)
#define __FIMG_DEFS_H__

/*****************************************************************************
  INCLUDES
 *****************************************************************************/

/*****************************************************************************
  DEFINES
 *****************************************************************************/
#define FIMG_DEFAULT_DEPTH_VALUE		(0.0f)

#define FIMG_MIN_DISPLAY_WIDTH			(16)
#define FIMG_MIN_DISPLAY_HEIGHT			(16)
#define FIMG_MAX_DISPLAY_WIDTH			(2048)
#define FIMG_MAX_DISPLAY_HEIGHT			(2048)
#define FIMG_MAX_DISPLAY_STRIDE			(16384)

#define FIMG_MIN_TEXTURE_WIDTH	        (1)
#define FIMG_MIN_TEXTURE_HEIGHT	        (1)
#define FIMG_MAX_TEXTURE_WIDTH	        (2048)
#define FIMG_MAX_TEXTURE_HEIGHT	        (2048)


/****************************************************************************
 *  MACROS
 ****************************************************************************/



/*****************************************************************************
 *  TYPEDEFS
 *****************************************************************************/

/* Register table entry structure */
typedef struct reg_entry_tag
{
	unsigned int	addr;
	unsigned int	val;
} reg_entry, *preg_entry;


typedef struct default_reg_tag
{
	unsigned int 	index;
	unsigned int    writable;
	reg_entry		reg;
} default_reg, *pdefault_reg;

/* Shader memory entry structure */
typedef struct shader_mem_tag
{
	unsigned int	start_addr;
	unsigned int	end_addr;
	unsigned int	size;
} shader_mem, *pshader_mem;

typedef struct shader_slot_tag
{
	unsigned int	index;
	shader_mem		mem;
} shader_slot, *pshader_slot;

/* constant macros */
/* function macros */
/* typedefs */
/* enums */

/*****************************************************************************
 *  ENUMS
 *****************************************************************************/
#if 0
	typedef enum fimg_roreg_tag
	{
		fimg_roreg_emptyspace,
		fimg_roreg_count,
		fimg_roreg_forcedword = 0x7FFFFFFF
	} fimg_roreg, *pfimg_roreg;
#endif

typedef enum fg_globreg_tag
{
	r_pipeline_state,
	r_cache_ctrl,
	r_sw_reset,
	r_int_pending,
	r_int_mask,
	r_pipeline_mask,
	r_pipeline_taget,
	r_int_state,
	gb_reg_count,
	gb_forced_word = 0x7FFFFFFF
} fg_gbreg, *pfg_gbreg;


typedef enum fimg_hireg_tag
{
	r_fifo_empty_space,
	r_fifo_entry_port,
	r_hi_ctrl,
	r_idx_offset,
	r_vtxbuf_addr,
	r_vtxbuf_entry_port,
	r_attrib_ctrl0,
	r_attrib_ctrl1,
	r_attrib_ctrl2,
	r_attrib_ctrl3,
	r_attrib_ctrl4,
	r_attrib_ctrl5,
	r_attrib_ctrl6,
	r_attrib_ctrl7,
	r_attrib_ctrl8,
	r_attrib_ctrl9,
	r_attrib_ctrla,
	r_attrib_ctrlb,
	r_attrib_ctrlc,
	r_attrib_ctrld,
	r_attrib_ctrle,
	r_attrib_ctrlf,
 	r_vtxbuf_ctrl0,
	r_vtxbuf_ctrl1,
	r_vtxbuf_ctrl2,
	r_vtxbuf_ctrl3,
	r_vtxbuf_ctrl4,
	r_vtxbuf_ctrl5,
	r_vtxbuf_ctrl6,
	r_vtxbuf_ctrl7,
	r_vtxbuf_ctrl8,
	r_vtxbuf_ctrl9,
	r_vtxbuf_ctrla,
	r_vtxbuf_ctrlb,
	r_vtxbuf_ctrlc,
	r_vtxbuf_ctrld,
	r_vtxbuf_ctrle,
	r_vtxbuf_ctrlf,
	r_vtxbuf_base0,
	r_vtxbuf_base1,
	r_vtxbuf_base2,
	r_vtxbuf_base3,
	r_vtxbuf_base4,
	r_vtxbuf_base5,
	r_vtxbuf_base6,
	r_vtxbuf_base7,
	r_vtxbuf_base8,
	r_vtxbuf_base9,
	r_vtxbuf_basea,
	r_vtxbuf_baseb,
	r_vtxbuf_basec,
	r_vtxbuf_based,
	r_vtxbuf_basee,
	r_vtxbuf_basef,
	hi_reg_count,
	hi_forced_word = 0x7FFFFFFF
} fimg_hi_reg, *pfimg_hi_reg;

typedef enum fimg_vs_reg_tag
{
	r_vs_config,
	r_inter_status,
	r_pc_range,
	r_vs_attrib_num,
	r_in_attrib_idx0,
	r_in_attrib_idx1,
	r_in_attrib_idx2,
	r_out_attrib_idx0,
	r_out_attrib_idx1,
	r_out_attrib_idx2,
	vs_reg_count,
	vs_forced_word = 0x7FFFFFFF
} fimg_vs_reg, *pfimg_vs_reg;


typedef enum fimg_pe_reg_tag
{
	r_vtx_context,
	r_viewport_center_xcord,
	r_viewport_center_ycord,
	r_viewport_half_width,
	r_viewport_half_height,
	r_half_depth_range,
	r_average_near_far,
	pe_reg_count,
	pe_forced_word = 0x7FFFFFFF
} fimg_pe_reg, *pfimg_pe_reg;


typedef enum fimg_ra_reg_tag
{
	r_pixel_sampos,
	r_depth_offset_en,
	r_depth_offset_factor,
	r_depth_offset_unit,
	r_depth_offset_rval,
	r_backface_cull,
	r_clip_ycord,
	r_lod_ctrl,
	r_clip_xcord,
#ifdef _FIMG3DSE_VER_1_1
	r_point_width,
	r_point_size_min,
	r_point_size_max,
	r_coord_replace,
	r_line_width,
#endif
	ra_reg_count,
	ra_forced_word = 0x7FFFFFFF
} fimg_ra_reg, *pfimg_ra_reg;


typedef enum fimg_ps_reg_tag
{
	r_exe_mode,
	r_pc_start,
	r_pc_end,
	r_pc_copy,
	r_ps_attrib_num,
	r_inbuf_status,
	ps_reg_count,
	ps_forced_word = 0x7FFFFFFF
} fimg_ps_reg, *pfimg_ps_reg;


typedef enum fimg_tu_reg_tag
{
#ifdef _FIMG3DSE_VER_1_2
	r_tex0_ctrl,
	r_tex0_usize,
	r_tex0_vsize,
	r_tex0_psize,
	r_tex0_l1_offset,
	r_tex0_l2_offset,
	r_tex0_l3_offset,
	r_tex0_l4_offset,
	r_tex0_l5_offset,
	r_tex0_l6_offset,
	r_tex0_l7_offset,
	r_tex0_l8_offset,
	r_tex0_l9_offset,
	r_tex0_l10_offset,
	r_tex0_l11_offset,
	r_tex0_min_level,
	r_tex0_max_level,
	r_tex0_base_addr,
	r_tex1_ctrl,
	r_tex1_usize,
	r_tex1_vsize,
	r_tex1_psize,
	r_tex1_l1_offset,
	r_tex1_l2_offset,
	r_tex1_l3_offset,
	r_tex1_l4_offset,
	r_tex1_l5_offset,
	r_tex1_l6_offset,
	r_tex1_l7_offset,
	r_tex1_l8_offset,
	r_tex1_l9_offset,
	r_tex1_l10_offset,
	r_tex1_l11_offset,
	r_tex1_min_level,
	r_tex1_max_level,
	r_tex1_base_addr,
	r_tex2_ctrl,
	r_tex2_usize,
	r_tex2_vsize,
	r_tex2_psize,
	r_tex2_l1_offset,
	r_tex2_l2_offset,
	r_tex2_l3_offset,
	r_tex2_l4_offset,
	r_tex2_l5_offset,
	r_tex2_l6_offset,
	r_tex2_l7_offset,
	r_tex2_l8_offset,
	r_tex2_l9_offset,
	r_tex2_l10_offset,
	r_tex2_l11_offset,
	r_tex2_min_level,
	r_tex2_max_level,
	r_tex2_base_addr,
	r_tex3_ctrl,
	r_tex3_usize,
	r_tex3_vsize,
	r_tex3_psize,
	r_tex3_l1_offset,
	r_tex3_l2_offset,
	r_tex3_l3_offset,
	r_tex3_l4_offset,
	r_tex3_l5_offset,
	r_tex3_l6_offset,
	r_tex3_l7_offset,
	r_tex3_l8_offset,
	r_tex3_l9_offset,
	r_tex3_l10_offset,
	r_tex3_l11_offset,
	r_tex3_min_level,
	r_tex3_max_level,
	r_tex3_base_addr,
	r_tex4_ctrl,
	r_tex4_usize,
	r_tex4_vsize,
	r_tex4_psize,
	r_tex4_l1_offset,
	r_tex4_l2_offset,
	r_tex4_l3_offset,
	r_tex4_l4_offset,
	r_tex4_l5_offset,
	r_tex4_l6_offset,
	r_tex4_l7_offset,
	r_tex4_l8_offset,
	r_tex4_l9_offset,
	r_tex4_l10_offset,
	r_tex4_l11_offset,
	r_tex4_min_level,
	r_tex4_max_level,
	r_tex4_base_addr,
	r_tex5_ctrl,
	r_tex5_usize,
	r_tex5_vsize,
	r_tex5_psize,
	r_tex5_l1_offset,
	r_tex5_l2_offset,
	r_tex5_l3_offset,
	r_tex5_l4_offset,
	r_tex5_l5_offset,
	r_tex5_l6_offset,
	r_tex5_l7_offset,
	r_tex5_l8_offset,
	r_tex5_l9_offset,
	r_tex5_l10_offset,
	r_tex5_l11_offset,
	r_tex5_min_level,
	r_tex5_max_level,
	r_tex5_base_addr,
	r_tex6_ctrl,
	r_tex6_usize,
	r_tex6_vsize,
	r_tex6_psize,
	r_tex6_l1_offset,
	r_tex6_l2_offset,
	r_tex6_l3_offset,
	r_tex6_l4_offset,
	r_tex6_l5_offset,
	r_tex6_l6_offset,
	r_tex6_l7_offset,
	r_tex6_l8_offset,
	r_tex6_l9_offset,
	r_tex6_l10_offset,
	r_tex6_l11_offset,
	r_tex6_min_level,
	r_tex6_max_level,
	r_tex6_base_addr,
	r_tex7_ctrl,
	r_tex7_usize,
	r_tex7_vsize,
	r_tex7_psize,
	r_tex7_l1_offset,
	r_tex7_l2_offset,
	r_tex7_l3_offset,
	r_tex7_l4_offset,
	r_tex7_l5_offset,
	r_tex7_l6_offset,
	r_tex7_l7_offset,
	r_tex7_l8_offset,
	r_tex7_l9_offset,
	r_tex7_l10_offset,
	r_tex7_l11_offset,
	r_tex7_min_level,
	r_tex7_max_level,
	r_tex7_base_addr,
#else
	r_tex0_ctrl,
	r_tex0_base_level,
	r_tex0_max_level,
	r_tex0_base_addr,
	r_tex1_ctrl,
	r_tex1_base_level,
	r_tex1_max_level,
	r_tex1_base_addr,
	r_tex2_ctrl,
	r_tex2_base_level,
	r_tex2_max_level,
	r_tex2_base_addr,
	r_tex3_ctrl,
	r_tex3_base_level,
	r_tex3_max_level,
	r_tex3_base_addr,
	r_tex4_ctrl,
	r_tex4_base_level,
	r_tex4_max_level,
	r_tex4_base_addr,
	r_tex5_ctrl,
	r_tex5_base_level,
	r_tex5_max_level,
	r_tex5_base_addr,
	r_tex6_ctrl,
	r_tex6_base_level,
	r_tex6_max_level,
	r_tex6_base_addr,
	r_tex7_ctrl,
	r_tex7_base_level,
	r_tex7_max_level,
	r_tex7_base_addr,
#endif
	r_color_key1,
	r_color_key2,
	r_color_key_yuv,
	r_color_key_mask,
	r_pallete_addr,
	r_pallete_entry,
	r_vtxtex0_ctrl,
	r_vtxtex0_base_addr,
	r_vtxtex1_ctrl,
	r_vtxtex1_base_addr,
	r_vtxtex2_ctrl,
	r_vtxtex2_base_addr,
	r_vtxtex3_ctrl,
	r_vtxtex3_base_addr,
	tu_reg_count,
	tu_forced_word = 0x7FFFFFFF
} fimg_tu_reg, *pfimg_tu_reg;

/* Per-Fragment Unit*/
typedef enum fimg_pf_reg_tag
{
	r_scissor_xcord,
	r_scissor_ycord,
	r_alpha,
	r_frontface_stencil,
	r_backface_stencil,
	r_depth,
	r_blend_color,
	r_blend,
	r_logic_op,
	r_color_mask,
	r_depth_mask,
	r_colorbuf_ctrl,
	r_depthbuf_addr,
	r_colorbuf_addr,
	r_colorbuf_width,
	pf_reg_count,
	pf_forced_word = 0x7FFFFFFF
} fimg_pf_reg, *pfimg_pf_reg;


typedef enum fimg_shader_slot_tag
{
	r_shader_instruction_slot,
	r_shader_const_float_slot,
	r_shader_const_int_slot,
	r_shader_const_bool_slot,
	r_shader_count,
	r_shader_forced_word = 0x7FFFFFFF
} fimg_shader_slot, *pfimg_shader_slot;


#endif	/* __FIMG_DEFS_H__ */

/*-----------------------------< End of file >-------------------------------*/
