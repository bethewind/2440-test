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
* @file test_list.h
* @brief
*
* @author Thomas, Kim (cheolkyoo.kim@samsung.com)
* @version 1.2
*/

#ifndef _VECTORS_LIST_H_
#define _VECTORS_LIST_H_

#if 0
    #include "./test_vectors/BurstTransfer.cpp"
    #include "./test_vectors/BurstTransfer.cpp"
    #include "./test_vectors/VtxCache.cpp"
    #include "./test_vectors/VBMixedIdx.cpp"
    #include "./test_vectors/VBAutoIncre.cpp"
    #include "./test_vectors/VBIdxTrans.cpp"
    #include "./test_vectors/VtxDataType.cpp"
    #include "./test_vectors/Lighting.cpp"
    #include "./test_vectors/Transform.cpp"
    #include "./test_vectors/DisplaceVtx.cpp"
    #include "./test_vectors/Tri.cpp"
    #include "./test_vectors/TriFan.cpp"
    #include "./test_vectors/TriStrip.cpp"
    #include "./test_vectors/ClipTri.cpp"
    #include "./test_vectors/ClipTriFan0.cpp"
    #include "./test_vectors/ClipTriFan1.cpp"
    #include "./test_vectors/ClipTriStrip.cpp"
    #include "./test_vectors/NWTriFan0.cpp"
    #include "./test_vectors/NWTriFan1.cpp"
    #include "./test_vectors/NWTriStrip.cpp"
    #include "./test_vectors/Viewport.cpp"
    #include "./test_vectors/SmallTri.cpp"
    #include "./test_vectors/SamplingPos.cpp"
    #include "./test_vectors/DepthOffset.cpp"
    #include "./test_vectors/BackfaceCull.cpp"
    #include "./test_vectors/XYClipping.cpp"
    #include "./test_vectors/Line.cpp"
    #include "./test_vectors/LineLoop.cpp"
    #include "./test_vectors/LineStrip.cpp"
    #include "./test_vectors/Point.cpp"
    #include "./test_vectors/PointSprite.cpp"
    #include "./test_vectors/ProgramCtrl.cpp"
    #include "./test_vectors/BumpMap.cpp"
    #include "./test_vectors/Fog.cpp"
    #include "./test_vectors/MultiTex.cpp"
    #include "./test_vectors/Imaging.cpp"
    #include "./test_vectors/Texture2DFormat.cpp"
    #include "./test_vectors/TexFmt3D8888.cpp"
    #include "./test_vectors/TexCubemap.cpp"
    #include "./test_vectors/MipmapFmt8888.cpp"
    #include "./test_vectors/TexUVMode.cpp"
    #include "./test_vectors/TexLOD.cpp"
    #include "./test_vectors/TexFmts3tc.cpp"
    #include "./test_vectors/Scissor.cpp"
    #include "./test_vectors/Alpha.cpp"
    #include "./test_vectors/Stencil.cpp"
    #include "./test_vectors/Depth.cpp"
    #include "./test_vectors/Blend.cpp"
    #include "./test_vectors/LogicOp.cpp"
    #include "./test_vectors/ColorWriteMask.cpp"
    #include "./test_vectors/DepthWriteMask.cpp"
    #include "./test_vectors/FrameBufFmt.cpp"
    #include "./test_vectors/Pawn.cpp"
    #include "./test_vectors/Teapot.cpp"
    #include "./test_vectors/Venus.cpp"
    #include "./test_vectors/Bunny.cpp"
    #include "./test_vectors/Cow.cpp"
    #include "./test_vectors/Dragon.cpp"
#endif


const TestVectorList Vectors[] =
{
    {   1,	FIMG_HOST_INTERFACE, 	"V150_HI_BD_0001", 	BurstTransfer,	"BurstTransfer",	0,				            "Bishop.dat"},
    {   2,	FIMG_HOST_INTERFACE, 	"V150_HI_BD_0002", 	VtxCache,     	"VtxCache",     	0,						0},
    {   3,	FIMG_HOST_INTERFACE, 	"V150_HI_BD_0003", 	VBMixedIdx,   	"VBMixedIdx",   	0,						0},
    {   4,	FIMG_HOST_INTERFACE, 	"V150_HI_BD_0004", 	VBAutoIncre,  	"VBAutoIncre",  	0,						0},
    {   5,	FIMG_HOST_INTERFACE, 	"V150_HI_BD_0005", 	VBIdxTrans,   	"VBIdxTrans",   	0,						0},
    {   6,	FIMG_HOST_INTERFACE, 	"V150_HI_BD_0006", 	VtxDataType,  	"VtxDataType",  	0,						0},
    {   7,	FIMG_VERTEX_SHADER, 	"V150_VS_BD_0001", 	Lighting,     		"Lighting",     		0,                      			0},
    {   8,	FIMG_VERTEX_SHADER, 	"V150_VS_BD_0002", 	Transform,    	"Transform",    	"TranformTexFmt1555.in",	0},
    {   9,	FIMG_VERTEX_SHADER, 	"V150_VS_BD_0003", 	DisplaceVtx,  	"DisplaceVtx",  	"DisplaceVtxFmt8888.in",	"Flag.dat"},
    {  10,	FIMG_PRIMITIVE_ENGINE, 	"V150_PE_BD_0001", 	Tri,          		"Tri",             		0,						0},
    {  11,	FIMG_PRIMITIVE_ENGINE, 	"V150_PE_BD_0002", 	TriFan,       		"TriFan",     	       0,						0},
    {  12,	FIMG_PRIMITIVE_ENGINE, 	"V150_PE_BD_0003", 	TriStrip,     		"TriStrip",      		0,						0},
    {  13,	FIMG_PRIMITIVE_ENGINE, 	"V150_PE_BD_0004", 	ClipTri,      		"ClipTri",         	0,						0},
    {  14,	FIMG_PRIMITIVE_ENGINE, 	"V150_PE_BD_0005", 	ClipTriFan0,  	"ClipTriFan0",      	0,						0},
    {  15,	FIMG_PRIMITIVE_ENGINE, 	"V150_PE_BD_0006", 	ClipTriFan1,  	"ClipTriFan1",      	0,						0},
    {  16,	FIMG_PRIMITIVE_ENGINE, 	"V150_PE_BD_0007", 	ClipTriStrip, 		"ClipTriStrip",     	0,						0},
    {  17,	FIMG_PRIMITIVE_ENGINE, 	"V150_PE_BD_0008", 	NWTriFan0,    	"NWTriFan0",        	0,						0},
    {  18,	FIMG_PRIMITIVE_ENGINE, 	"V150_PE_BD_0009", 	NWTriFan1,    	"NWTriFan1",        	0,						0},
    {  19,	FIMG_PRIMITIVE_ENGINE, 	"V150_PE_BD_0010", 	NWTriStrip,   	"NWTriStrip",       	0,						0},
    {  20,	FIMG_PRIMITIVE_ENGINE, 	"V150_PE_BD_0011", 	Viewport,   	  	"Viewport",         	0,						0},
    {  21,	FIMG_RASTER_ENGINE,		"V150_RA_BD_0001", 	SmallTri,    	 	"SmallTri",         	0,          					0},
    {  22,	FIMG_RASTER_ENGINE,		"V150_RA_BD_0002", 	SamplingPos,  	"SamplingPos",      0,          					0},
    {  23,	FIMG_RASTER_ENGINE,		"V150_RA_BD_0003", 	DepthOffset, 	"DepthOffset",      	0,          					"Sphere.dat"},
    {  24,	FIMG_RASTER_ENGINE,		"V150_RA_BD_0004", 	BackfaceCull, 	"BackfaceCull",     	0,          					0},
    {  25,	FIMG_RASTER_ENGINE,		"V150_RA_BD_0005", 	XYClipping,   	"XYClipping",       	0,    		      				0},
    {  26,	FIMG_RASTER_ENGINE,		"V150_RA_BD_0006", 	Line,         		"Line",             	0,         	 				0},
    {  27,	FIMG_RASTER_ENGINE,		"V150_RA_BD_0007", 	LineLoop,   	  	"LineLoop",         	0,     	     				0},
    {  28,	FIMG_RASTER_ENGINE,		"V150_RA_BD_0008", 	LineStrip,   	 	"LineStrip",       	0,          					0},
    {  29,	FIMG_RASTER_ENGINE,		"V150_RA_BD_0009", 	Point,        		"Point",            	0,          					0},
    {  30,	FIMG_RASTER_ENGINE,		"V150_RA_BD_0010", 	PointSprite,  	"PointSprite",      	"flash.dds",			    	"PointSprite.dat"},
    {  31,	FIMG_FRAGMENT_SHADER,	"V150_FS_BD_0001", 	ProgramCtrl,  	"ProgramCtrl",      	"ProgramCtrl.in",    		0},
    {  32,	FIMG_FRAGMENT_SHADER,	"V150_FS_BD_0002", 	BumpMap,      	"BumpMap",          	"BumpMap.in",        		0},
    {  33,	FIMG_FRAGMENT_SHADER,	"V150_FS_BD_0003", 	Fog,          		"Fog",              	"Earth.dds",         			"Fog.dat"},
    {  34,	FIMG_FRAGMENT_SHADER,	"V150_FS_BD_0004", 	MultiTex,   	  	"MultiTex",         	"MultiTex.in",       			"Sphere.dat"},
    {  35,	FIMG_FRAGMENT_SHADER,	"V150_FS_BD_0005", 	Imaging,   	   	"Imaging",          	"ImagingFmt8888.in", 		0},
    {  36,	FIMG_TEXTURE_UNIT,		0, 	                		Texture2DFormat,		"Texture2DFormat","TexFmt2D.in",              0},
    {  37,	FIMG_TEXTURE_UNIT,		"V150_TU_BD_0002", 	TexFmt3D8888,   "TexFmt3D8888",   "TexFmt3D8888.in",          0},
    {  38,	FIMG_TEXTURE_UNIT,		"V150_TU_BD_0003", 	TexCubemap,    	 "TexCubemap",       "TexCubemapARGB888.raw",    "Cubemap.dat"},
    //{  38,	FIMG_TEXTURE_UNIT,		"V150_TU_BD_0003", 	TexCubemap,    	 "TexCubemap",       "CubeTexFmt8888.in",    "Cubemap.dat"},
    {  39,	FIMG_TEXTURE_UNIT,		"V150_TU_BD_0004", 	MipmapFmt8888,  "MipmapFmt8888","MipmapFmt8888.in",         0},
    {  40,	FIMG_TEXTURE_UNIT,		"V150_TU_BD_0005", 	TexUVMode,      	"TexUVMode",        	"TexUVMode.in",             0},
    {  41,	FIMG_TEXTURE_UNIT,		"V150_TU_BD_0006", 	TexLOD,         	"TexLOD",           "TexLOD.dds",               0},
    {  42,	FIMG_TEXTURE_UNIT,		"V150_TU_BD_0007", 	TexFmts3tc,     	"TexFmts3tc",       "TexFmts3tc.in",            0},
    {  43,	FIMG_PERFRAGMENT_UNIT, 	"V150_PF_BD_0001", 	Scissor,        		"Scissor",          0,							0},
    {  44,	FIMG_PERFRAGMENT_UNIT, 	"V150_PF_BD_0002", 	Alpha,          		"Alpha",            0,							0},
    {  45,	FIMG_PERFRAGMENT_UNIT, 	"V150_PF_BD_0003", 	Stencil,        		"Stencil",          0,							0},
    {  46,	FIMG_PERFRAGMENT_UNIT, 	"V150_PF_BD_0004", 	Depth,          		"Depth",            0,							0},
    {  47,	FIMG_PERFRAGMENT_UNIT, 	"V150_PF_BD_0005", 	Blend,          		"Blend",            0,							0},
    {  48,	FIMG_PERFRAGMENT_UNIT, 	"V150_PF_BD_0006", 	LogicOp,        	"LogicOp",          0,							0},
    {  49,	FIMG_PERFRAGMENT_UNIT, 	"V150_PF_BD_0007", 	ColorWriteMask, 	"ColorWriteMask",   0,							0},
    {  50,	FIMG_PERFRAGMENT_UNIT, 	"V150_PF_BD_0008", 	DepthWriteMask,	"DepthWriteMask",   0,							0},
    {  51,	FIMG_PERFRAGMENT_UNIT, 	0, 	                		FrameBufferCtrl,"FrameBufFmt",      0,							0},
    {  52,	FIMG_ALL_PIPELINE, 		"V150_MD_BD_0001", 	Pawn,           		"Pawn",				0, 				            "Pawn.dat"	},
    {  53,	FIMG_ALL_PIPELINE, 		"V150_MD_BD_0002", 	Teapot,         	"Teapot",   		0, 				            "Teapot.dat"},
    {  54,	FIMG_ALL_PIPELINE, 		"V150_MD_BD_0003", 	Venus,          		"Venus",    		0, 				            "Venus.dat"	},
    {  55,	FIMG_ALL_PIPELINE, 		"V150_MD_BD_0004", 	Bunny,          	"Bunny", 			0, 				            "Bunny.dat"	},
    {  56,	FIMG_ALL_PIPELINE, 		"V150_MD_BD_0005", 	Cow,            		"Cow", 		    	0, 				            "Cow.dat"	},
    {  57,	FIMG_ALL_PIPELINE, 		"V150_MD_BD_0006", 	Dragon,         	"Dragon",			0, 				            "Dragon.dat"},
    {   0,	FIMG_NONE_BLOCK, 		0,                  0, 				0, 					0, 							0}
};

#endif /* _VECTORS_LIST_H_ */
