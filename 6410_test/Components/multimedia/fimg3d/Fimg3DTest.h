/*
* FPGA Test Vector for FIMG-3DSE ver 1.x Developed by Graphics Team
*
* Copyright 2007 by Mobie neXt Generation Technology, Samsung Electronics, Inc.,
* San#24, Nongseo-Dong, Giheung-Gu, Yongin, Korea. All rights reserved.
*
* This software is the confidential and proprietary information
* of Samsung Electronics, Inc. ("Confidential Information"). You
* shall not disclose such Confidential Information and shall use
* it only in accordance with the terms of the license agreement
* you entered into with Samsung.
*/
/**
* @file fimg3d_test.h
* @brief file defines the test vector and table which test for FIMG-3DSE ver 1.2.
*
* @author Thomas, Kim (cheolkyoo.kim@samsung.com)
* @version 1.2
*/

#ifndef _FIMG3D_TEST_H_
#define _FIMG3D_TEST_H_


#include "system.h"
#include "fgl.h"
#include "Config.h"
#include "SysUtility.h"
#include "Matrix4.h"
#include "library.h"

#ifdef __cplusplus
extern "C" {
#endif


#if 1


typedef enum
{
    FIMG_NONE_BLOCK,
    FIMG_HOST_INTERFACE,
    FIMG_VERTEX_SHADER,
    FIMG_PRIMITIVE_ENGINE,
    FIMG_RASTER_ENGINE, 
    FIMG_FRAGMENT_SHADER,
    FIMG_TEXTURE_UNIT,
    FIMG_PERFRAGMENT_UNIT,
    FIMG_ALL_PIPELINE 
} FimgSubBlock;


typedef enum
{
	FIMGPT_RGB16_555,            
	FIMGPT_RGB16_565,            
	FIMGPT_ARGB16_4444,          
	FIMGPT_ARGB16_1555,          
	FIMGPT_XRGB32_0888,	         
	FIMGPT_ARGB32_8888          
} SurfacePixelFmt ;

typedef enum
{
    BMP_FILE = 1,
    PPM_FILE,
    NO_USE
} FBDumpFileFmt;

typedef enum
{
    SEMIHOST_PROJECT_RELATIVE = 1,
    SEMIHOST_ABSOLUTE_PATH,
    DIRECT_ACCESS_MMC
} TexFilePath;


//typedefs

typedef struct 
{
	unsigned int uIndex;
	FimgSubBlock TargetBlock;
	const char* pVectorId;
	int (*fp)(void); 
	const char* pVectorName;
	const char* pTextureFile;
	const char* p3DObjectFile;
} TestVectorList;


struct Context3D
{
    u8    bShadeMode;
    u8    bIsAllTest;
    u8    bIsSpecificTest;
    u8    bIsUseXmlConfig;
    u8    bSwapBackBuffer;		
    	
    float	X;
    float	Y;	
    float	Width;
    float	Height;
    float	Near;
    float	Far;	
    int		FBOffsize;
    //float   ProjMat[16];
    unsigned int nNumOfData;
    unsigned int nNumOfVertices;
    unsigned int FrontFBAddr;
    unsigned int BackFBAddr;

    unsigned int uScreenPhyHSize;
    unsigned int uScreenPhyVSize;
    FBDumpFileFmt FBDumpFileFmt;
    TexFilePath  TexureFilePath;

    unsigned int uStartPointVector;

/*
    unsigned int uWindowWidthSize;
    unsigned int uWindowHeightSize;	
    unsigned int uWindowSurface;
    unsigned int uFrameSurface;
*/

};

typedef struct Test_Func_Tag
{
	int (*func)(Context3D *);
	const char *funcName;
} Test_Func;

typedef struct Test_Texfmt_Tag
{
	unsigned int    index;
	unsigned int    offset;
	const char*     texName;
} Test_Texfmt;

typedef struct Test_FBfmt_Tag
{
	unsigned int    index;
	const char*     format;
	const char*     dumpid;
} Test_FBfmt;



//Constant definitions

#if 0
void G3D::SetDoubleBufAddr(unsigned int uFrontFbAddr, unsigned int uBackFbAddr)
{
	m_FrontFBAddr = uFrontFbAddr;
	m_BackFBAddr = uBackFbAddr;
	drvsys_clear_buf((unsigned int*)m_FrontFBAddr, FRAME_BUFFER_SIZE, 0x0);
	drvsys_clear_buf((unsigned int*)m_BackFBAddr, FRAME_BUFFER_SIZE, 0x0);
}

void G3D::SetScreenPhyicalSize(unsigned int uPhysicalHSize, unsigned int uPhysicalvSize)
{
    if(uPhysicalHSize > 1 && uPhysicalvSize > 1)
    {
        m_uScreenPhyHSize = uPhysicalHSize;
        m_uScreenPhyVSize = uPhysicalvSize;  
    }
}
#endif

//const unsigned int   TexLoadAddr = 0x60000000;
//const unsigned int   FrameBufAddr = 0x61000000;
 
//Variable declarations(extern)


//Function declarations

int SFRegRwTest(void);

// HI
int BurstTransfer(void); 
int VtxCache(void); 
int VBMixedIdx(void); 
int VBAutoIncre(void); 
int VBIdxTrans(void); 
int VtxDataType(void); 
// VS
int Lighting(void);
int Transform(void);
int DisplaceVtx(void);
// PE
int Tri(void);
int TriFan(void);
int TriStrip(void);
int ClipTri(void);
int ClipTriFan0(void);
int ClipTriFan1(void);
int ClipTriStrip(void);
int NWTriFan0(void);
int NWTriFan1(void);
int NWTriStrip(void);
int Viewport(void);
// RA
int Point(void);
int Line(void);
int LineLoop(void);
int LineStrip(void);
int PointSprite(void);
int SmallTri(void);
int SamplingPos(void);
int DepthOffset(void);
int BackfaceCull(void);
int XYClipping(void);
// PS
int ProgramCtrl(void);
int BumpMap(void);
int Fog(void);
int MultiTex(void);
int Imaging(void);
// PF
int Scissor(void);
int Alpha(void);
int Stencil(void);
int Depth(void);
int Blend(void);
int LogicOp(void);
int ColorWriteMask(void);
int DepthWriteMask(void);
int FrameBufferCtrl(void);
int FrameBufFmt(FGL_PixelFormat fmt);
// TU
int Texture2DFormat(void);
int TexFmt2D(FGL_TexelFormat format, unsigned int offset);
int MipmapFmt8888(void); 
int TexCubemap(void);
int TexFmt3D8888(void);
int TexMipMapFilter(void);
int TexUVMode(void);
int TexLOD(void);
int TexFmts3tc(void);
// MODEL
int Pawn(void);
int Teapot(void);
int Venus(void);
int Bunny(void);
int Cow(void);
int Dragon(void);

//DEMO
int Demo_Teapot(void);
int Vase(void);
int Demo_Particles(void);


/****************************************************************************
 *  ARM ASSEMBLER FUNCTION
 ****************************************************************************/


#endif


#ifdef __cplusplus
}
#endif

extern "C" void drvsys_clear_buf(
					unsigned int 	*buf, 
					unsigned int 	size, 
					unsigned int 	fill
				);

#endif /* _FIMG3D_TEST_H_ */

