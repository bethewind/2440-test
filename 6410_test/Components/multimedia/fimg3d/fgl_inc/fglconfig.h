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
 * @file	fglconfig.h
 * @brief	This is the configuration header file for fgl.
 * @author	Cheolkyoo Kim
 * @version	1.5
 */

#if !defined(__FIMG_CONFIG_H__)
#define __FIMG_CONFIG_H__

/*----------------------------------------------------------------------------*/

#define FIMG_MAJOR 		1
#define FIMG_MINOR 		0
#define FIMG_BUILD 		00


#define _FIMG3DSE_VER_1_1       /* Cancel */

#define _FIMG3DSE_VER_1_2       1
#define _FIMG3DSE_VER_1_2_1     2

#define TARGET_FIMG_VERSION     _FIMG3DSE_VER_1_2_1

#define _FIMG_PIPELINE_SINGLE

#define FPGA_BOARD 		2

#define TARGET_PLATFORM FPGA_BOARD


#define DUMP_FRAME_BUFFER

#if defined(FIMG_DEBUG)
#define FIMG_PDUMP
#define FIMG_TRACE
#endif



#endif	/* __FIMG_CONFIG_H__ */


