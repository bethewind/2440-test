/*----------------------------------------------------------------------
 *
 * Filename: post_test.h
 *
 * Contents: Testing application of "class POST"
 *
 * Authors: 
 *
 * Notes:
 *
 * Copyright (c) 2003 SAMSUNG Electronics.
 *
 *----------------------------------------------------------------------
 */

#ifndef __POST_TEST_H__
#define __POST_TEST_H__

//#include "post.h"

#define IMG_YC420       "../images/420_240x320result(rotate90).yuv"
#define POST_YUV_FILE_NAME1      "D:/MyWork/MyProject/S3C6410/SW/TestCode/6410_Test_Rev02/Components/multimedia/image/CatureYUV/Captured1.yuv"
#define POST_YUV_FILE_NAME2      "D:/MyWork/MyProject/S3C6410/SW/TestCode/6410_Test_Rev02/Components/multimedia/image/CatureYUV/Captured2.yuv"

#define POST_IMAGE_PATTERN	0
#define POST_IMAGE_HEADER		1
#define POST_IMAGE_SOURCE		POST_IMAGE_PATTERN

#define POST_IMAGE_BMP_SLIDE			(TRUE)

#if (POST_IMAGE_SORCE == POST_IMAGE_HEADER)
#include "Guitar240320_16rgb.h"
#include "Guitar240320_24.h"
#include "guitar240320_YCrYCb422.h"
#include "guitar240320_YCbYCr422.h"
#include "guitar240320_CrYCbY422.h"
#include "guitar240320_CbYCrY422.h"
#include "guitar240320_YCbYCr420.h"
#include "RacingGirl320240.h"
#include "sun320240_16bpp.h"
#include "Yepp320x240.h"
#endif

#endif

