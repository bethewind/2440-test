/*******************************************************************************
 *
 *	ViP model for FIMGSE
 *		version 1.0
 *
 *	fimg_config.h
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
 *	2006.  3.  27	by cheolkyoo.kim
 *		Controls configuration of FIMG build variants
 *
 *	$RCSfile: fimg_config.h,v $
 *	$Revision: 1.1 $
 *	$Author: cheolkyoo.kim $
 *	$Date: 2006/03/30 02:24:31 $
 *	$Locker:  $
 *
 *	$Source: C:/CVS/CVSrepository/FIMG-3DSE_SW/fimg3dse_fpga/fimg3d/include/fimg_config.h,v $
 *	$State: Exp $
 *	$Log: fimg_config.h,v $
 *	Revision 1.1  2006/03/30 02:24:31  cheolkyoo.kim
 *	Thomas-20060330@comment: Initial commit
 *	
 ******************************************************************************/
#if !defined(__CONFIG_H__)
#define __CONFIG_H__

/*----------------------------------------------------------------------------*/

#define FIMG_MAJOR 		1
#define FIMG_MINOR 		0
#define FIMG_BUILD 		00

/*----------------------------------------------------------------------------*/
/*
#define FIMG_DEBUG
#define VIP_WORKAROUND
#define FPGA_WORKAROUND

#define HOST_RENDER_STATUS		0x00000100
#define HOST_EXCUTE_FUNCTION	0x00000104
#define HOST_ERROR_CODE			0x00000108
*/
#define _FIMG3DSE_VER_1_1       /* Cancel */

#define _FIMG3DSE_VER_1_2       1
#define _FIMG3DSE_VER_1_2_1     2


#define TARGET_PLATFORM FPGA_BOARD

#if TARGET_PLATFORM == FPGA_BOARD
#define DUMP_FRAME_BUFFER
#endif


/*----------------------------------------------------------------------------*/

#if defined(FIMG_DEBUG)
#define FIMG_PDUMP
#define FIMG_TRACE
#endif

	#define FIMG_LCDCON1		    	0x77100020
    	#define FIMG_COLOR_BUFFER	    	0x51000000
    	#define FIMG_DEPTH_BUFFER	    	0x51500000
	#define FIMG_TEXTURE_MEMORY	0x52000000
    	#define FIMG_GEOMETRY_MEMORY	0x52500000

	#define SCREEN_WIDTH_SIZE		800
	#define SCREEN_HEIGHT_SIZE		480
	#define LCD_HORIZONTAL_SIZE 	800
	#define LCD_VERTICAL_SIZE		480
	#define BYTES_PER_PIXEL			4
	#define LCD_XSIZE_TFT_240320	(800)
	#define LCD_YSIZE_TFT_240320	(480)
	#define LCD_FRAME_BUFFER_XSIZE	(800)
	#define LCD_FRAME_BUFFER_YSIZE	(480)

	#define CLEAR_SCREEN_SIZE	(SCREEN_WIDTH_SIZE * SCREEN_HEIGHT_SIZE * BYTES_PER_PIXEL)
	#define FRAME_BUFFER_SIZE	(LCD_FRAME_BUFFER_XSIZE * LCD_FRAME_BUFFER_YSIZE * BYTES_PER_PIXEL)

#endif	/* __CONFIG_H__ */


