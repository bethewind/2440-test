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
* @file fimg3d_test.cpp
* @brief
*
* @author Thomas, Kim (cheolkyoo.kim@samsung.com)
* @version 1.2
*/

#include <stdio.h>
#include <string.h>
#include "system.h"
#include "def.h"
#include "library.h"
#include "Config.h"
#include "SysUtility.h"
#include "ShaderUtility.h"
#include "Fimg3DTest.h"
#include "VectorsList.h"
#include "timer.h"
#include "lcd.h"
#include "sdhc.h"
#include "fat.h"

static SDHC oSdhc;

char gFileName[256];
char gTexPathName[256];
char gObjPathName[256];
unsigned int g_nTestedVectorCount = 0;
unsigned int g_nTestFailCount = 0;	
unsigned int g_nTestSuccessCount = 0;

struct SurfaceInfo gSurfaceInfo;
struct Context3D  gFimgContext;

int _SetScreenResol(Context3D* p3DContext);
int _SetScreenResol(Context3D* p3DContext);
int _SetScreenResol(Context3D* p3DContext);
int _SetDumpImageFmt(Context3D* p3DContext);
int _SetShadingMode(Context3D* p3DContext);
int _SetTexImageDir(Context3D* p3DContext);
int _SetConfigFile(Context3D* p3DContext);
void _PrintConfiguration(void);
int _DisassembleShader(void);

int ConfigureTest(void);
int LoadTextureImage(const char *filename);
int LoadGeometryData(const char *filename);
void DumpFrameBuffer(const char *filename);
int CompareFrameBuffer(const char *filename);




#ifdef __cplusplus
extern "C"{
#endif

void FIMG3D_Test(void);

#ifdef __cplusplus
}
#endif

void FIMG3D_Test(void)
{
	UART_Printf("\nFIMG3D DEMO START!!!\n");

	int sel;
	float fElapsedTime;
	unsigned int uFrameBufferBase = FIMG_COLOR_BUFFER;
	unsigned int uFrontFbAddr;
	FimgSubBlock FimgTestBlock;
	int TestArray[100];
	unsigned int ErrVectorList[57] = { 0, };
	
	CSPACE eBpp = RGB24;

	strcpy(gTexPathName, "D:/PROJECTS/S3C6410/Program/XG_3D_ORI/fimg3d/textures/");
	strcpy(gObjPathName, "D:/PROJECTS/S3C6410/Program/XG_3D_ORI/fimg3d/models/");

	// frame buffer dump
	gSurfaceInfo.width = 800;
	gSurfaceInfo.height = 480;
	gSurfaceInfo.pixelFormat = (FGL_PixelFormat)FGL_PIXEL_ARGB0888;
	gSurfaceInfo.stride = 800;
	gSurfaceInfo.pFBBaseAddr = (void *)FIMG_COLOR_BUFFER;

	gFimgContext.bShadeMode = true;
 	gFimgContext.bIsAllTest = false;
	gFimgContext.bIsUseXmlConfig = false;
 	gFimgContext.uScreenPhyHSize = 800;
	gFimgContext.uScreenPhyVSize = 480;
	gFimgContext.FBDumpFileFmt = NO_USE;
	//gFimgContext.TexureFilePath = SEMIHOST_PROJECT_RELATIVE;
	gFimgContext.TexureFilePath = DIRECT_ACCESS_MMC;

	gFimgContext.uStartPointVector = 0;
	gFimgContext.bIsSpecificTest = false;

	LCD_SetPort();
	LCD_InitLDI(MAIN);
	LCD_InitDISPC(eBpp, uFrameBufferBase, WIN0, true);
	LCD_SetWinOnOff(1, WIN0);
	LCD_Start();

	// -daedoo
	fglSysInit(0.0, 0.0, 800, 480, 0.0, 1.0);
    	//fglEnableProbeWatchPoints();
    
	UART_Printf("\nThis is Test Routine for FIMG-3DSE v1.5\n");
    
	#if 0	// DEMO

	while(GetKey() == 0)
	{
		LoadTextureImage("Particles.in");
		Demo_Particles();

		LoadTextureImage("Teapot.tex");
		Demo_Teapot();
		
		LoadTextureImage("Vase.tex");
		Vase();
	}	

	#elif 0

	LoadTextureImage("Particles.in");
	
	while(GetKey() == 0)
	{
		Demo_Particles();
	}	

	#elif 0

	LoadTextureImage("Teapot.tex");
	
	while(GetKey() == 0)
	{
		Demo_Teapot();
	}	

	#elif 0

	LoadTextureImage("Vase.tex");
	
	while(GetKey() == 0)
	{
		Vase();
	}	

	#endif
	
	while(1)
	{
	UART_Printf("\n");
    	UART_Printf("0 Exit     \t\t1 Test configure \t2 Register R/W test\n");
    	UART_Printf("3 Test all \t\t4 Host interface \t5 Vertex shader\n");
    	UART_Printf("6 Primitive engine\t7 Raster engine \t8 fragment shader\n");
    	UART_Printf("9 Texture unit\t\t10 Perfragment unit\t11 Models\n");
	UART_Printf("\nSelect the block to test : ");
		sel = UART_GetIntNum();
    	UART_Printf("\n\n");

	if (sel == 0)
		break;

    	switch(sel)
    	{
    	case 1:
            if(ConfigureTest() != NO_ERROR)
                UART_Printf("\n %-15s .......................... FAIL\n\n", "Configuration");
            else
                UART_Printf("\n %-15s .......................... OK\n\n", "Configuration");
            break;
    	case 2:
            if(SFRegRwTest() != NO_ERROR)
                UART_Printf("\n %-15s .......................... FAIL\n\n", "Configuration");
            else
                UART_Printf("\n %-15s .......................... OK\n\n", "Configuration");
            
		 //eBpp = RGB16;
		 eBpp = RGB24;

	        fglSoftReset();

		 LCD_SetWinOnOff(0, WIN0);
		 LCD_InitDISPC(eBpp, FIMG_COLOR_BUFFER, WIN0, true);
		 LCD_SetWinOnOff(1, WIN0);
		 
	        fglSysInit(0.0, 0.0, 800, 480, 0.0, 1.0);
            
            break;
    	case 3:
    	    gFimgContext.bIsAllTest = true;
    		break;
    	case 4:
            FimgTestBlock = FIMG_HOST_INTERFACE;
    		break;
    	case 5:
            FimgTestBlock = FIMG_VERTEX_SHADER;
    		break;
    	case 6:
            FimgTestBlock = FIMG_PRIMITIVE_ENGINE;
    		break;
    	case 7:
            FimgTestBlock = FIMG_RASTER_ENGINE;
    		break;
    	case 8:
            FimgTestBlock = FIMG_FRAGMENT_SHADER;
    		break;
    	case 9:
            FimgTestBlock = FIMG_TEXTURE_UNIT;
    		break;
    	case 10:
            FimgTestBlock = FIMG_PERFRAGMENT_UNIT;
    		break;
    	case 11:
            FimgTestBlock = FIMG_ALL_PIPELINE;
    		break;
    	default:
            FimgTestBlock = FIMG_NONE_BLOCK;
    		break;
    	}

		if(sel == 3)
		{
			int i = 0;
			unsigned int uTotalBmpPixel = gSurfaceInfo.width * gSurfaceInfo.height * 3; /* BMP 24bit */
	
			if(gFimgContext.bIsSpecificTest)
				i = gFimgContext.uStartPointVector;
            
			for (; i < (sizeof(Vectors)/sizeof(TestVectorList) - 1); i++)
    			{
				if(Vectors[i].uIndex != 0)
				{
					if(Vectors[i].pTextureFile != NULL) 
					{
						if(LoadTextureImage(Vectors[i].pTextureFile) != NO_ERROR)
						{
							UART_Printf("S/W ERROR >> LoadTextureImage is failed\n");
     							break;
						}
					}
            		
					if(Vectors[i].p3DObjectFile != NULL) 
					{
						if(LoadGeometryData(Vectors[i].p3DObjectFile) != NO_ERROR)
						{
        	                			UART_Printf("S/W ERROR >> LoadGeometryData is failed\n");
        	                			break;
        	            			}
        	        		}
            		
            				if(NO_ERROR == (Vectors[i].fp)())
            				{
            		   			UART_Printf("\n Rendering %-15s .......................... OK\n", Vectors[i].pVectorName);

                       			if(gFimgContext.FBDumpFileFmt != NO_USE)
                        			{
                             			if(Vectors[i].pVectorId != NULL)
                                			DumpFrameBuffer(Vectors[i].pVectorId);
                             
                             		g_nTestSuccessCount++;
                        			}
                        else
                        {
                            if(Vectors[i].pVectorId != NULL)
                            {
                                unsigned int uResult = CompareFrameBuffer(Vectors[i].pVectorId);
                                if( uResult != 0)
                                {
                                    UART_Printf("Verify [%d] %-15s ................... FAIL\n", (i+1), Vectors[i].pVectorName);
                                    UART_Printf("Error pixel %4.1f%% is different against %s.bmp file\n\n", 
                                        ((float)uResult/(float)uTotalBmpPixel*100.f), Vectors[i].pVectorId);    
                                    g_nTestFailCount++;
                                    ErrVectorList[i] = 1;
                                } 
                                else
                                {
                                    UART_Printf("Verify [%d] %-15s ................... PASS\n\n", (i+1), Vectors[i].pVectorName);
                                    g_nTestSuccessCount++;
                                }
                            }
                        }
                    }
            		else
            		{
                		UART_Printf("\n Rendering %-15s .......................... FAIL\n\n", Vectors[i].pVectorName);
	            		g_nTestFailCount++;
            		}
            		
            		g_nTestedVectorCount++;
    		    }
    		    
    		}
    		
    		gFimgContext.bIsAllTest = false;
		UART_Printf("\n\nTotal tested vectors count is %d",    g_nTestedVectorCount);
		UART_Printf("\n               fail             %d",    g_nTestFailCount);
		UART_Printf("\n               successful       %d\n",  g_nTestSuccessCount);

    		
    		if(g_nTestFailCount)
    		{
    		    UART_Printf("\n\n**************<< Errored vector list >>**************\n");

    		    for(int i = 0; i < sizeof(ErrVectorList)/sizeof(unsigned int); i++)
    		    {
    		        if(ErrVectorList[i])
    		            UART_Printf("\t%-15s,\t%-15s\n", Vectors[i].pVectorName, Vectors[i].pVectorId);
    		        
    		        ErrVectorList[i] = 0;
    		    }
    		    
    		    UART_Printf("*****************************************************\n\n");
    		}

			g_nTestedVectorCount = 0;
			g_nTestFailCount = 0;
			g_nTestSuccessCount = 0;
		}
		
		if(sel > 3 && sel < 12)
		{
    			while(1)
	    		{
	         		int j = 0;
	        		UART_Printf("%2d: %s\n", j++, "Exit");

	        		for (int i = 0; i < (sizeof(Vectors)/sizeof(TestVectorList) - 1); i++)
	        		{
	        		    if(Vectors[i].TargetBlock == FimgTestBlock)
	        		    {
	        			    TestArray[j] = i;
	        			    UART_Printf("%2d: %s\n", j++, Vectors[i].pVectorName);
	        		    }
	        		}

	        		UART_Printf("\nSelect the sub-function to test : ");
	        		sel = UART_GetIntNum();

				if (sel == 0)
					break;
	                
	                //drvsys_clear_buf((unsigned int*)FIMG_COLOR_BUFFER, CLEAR_SCREEN_SIZE, 0);
	                //drvsys_clear_buf((unsigned int*)FIMG_DEPTH_BUFFER, CLEAR_SCREEN_SIZE, 0xFFFFFFFF);
	                
	        		sel = TestArray[sel];
	        		
	    	        if(Vectors[sel].pTextureFile != NULL) 
	    	        {
	    	            if(LoadTextureImage(Vectors[sel].pTextureFile) != NO_ERROR)
	    	                break;
	    	        }
	    	        
	    	        if(Vectors[sel].p3DObjectFile != NULL) 
	    	        {
	    	            if(LoadGeometryData(Vectors[sel].p3DObjectFile) != NO_ERROR)
	    	                break;
	    	        }
					
				StartTimer(0);
	                	if(NO_ERROR == (Vectors[sel].fp)())
	        		{
					fElapsedTime = StopTimer(0);
		        		UART_Printf("\n %-15s .......................... OK", Vectors[sel].pVectorName);
					UART_Printf("\n Elapsed time = %.2f(us), Estimate FPS = %.2f(133MHz)\n\n", fElapsedTime, (1/(fElapsedTime/1000000)));

	                    if(gFimgContext.FBDumpFileFmt != NO_USE)
	                    {
	                         if(Vectors[sel].pVectorId != NULL)
	                            DumpFrameBuffer(Vectors[sel].pVectorId);
	                    }
#if 0
	                    else
	                    {
	                        if(Vectors[sel].pVectorId != NULL)
	                        {
	                            unsigned int uResult = CompareFrameBuffer(Vectors[sel].pVectorId);
	                            if( uResult != 0)
	                                UART_Printf("Compare golden image with frame buffer is failed, Error pixel %d\n\n", uResult);    
	                            else
	                                UART_Printf("\n Verify %-15s ................... OK\n\n", Vectors[sel].pVectorName);
	                        }
	                    }
#endif
	                }
	        		else
	        		{
	            		UART_Printf("\n %-15s .......................... FAIL\n\n", Vectors[sel].pVectorName);
	        		}
	        	    
	   		    }
		}
    }

	LCD_SetWinOnOff(0, WIN0);

}

void DumpFrameBuffer(const char *filename)
{
    if(gFimgContext.FBDumpFileFmt == BMP_FILE)
    {
        strcpy(gFileName, filename);
        strcat(gFileName, ".bmp");
        fglSysWriteBMPFile(gFileName, gSurfaceInfo);
    }
    else if(gFimgContext.FBDumpFileFmt == PPM_FILE)
    {
        strcpy(gFileName, filename);
        strcat(gFileName, ".ppm");
        fglSysWritePPMFile(gFileName, gSurfaceInfo);
    }
}

int CompareFrameBuffer(const char *filename)
{
    
    //unsigned int uSizeRead = 0;
    u32 uSizeRead = 0;
    unsigned int MismatchPixelCnt = 0;
    unsigned int uCopyDestAddr = (FIMG_GEOMETRY_MEMORY + 0x1000000);
    unsigned int uBmpBaseAddr = (FIMG_GEOMETRY_MEMORY + 0x2000000);
    void *pImageData = (void *)uBmpBaseAddr;

	unsigned char *pGoldenImageData = (unsigned char *)(uCopyDestAddr + 0x36);
	unsigned char *pNewImagenData	= (unsigned char *)uBmpBaseAddr;

    
    // Copy texture image to dram

    if (!SDHC_OpenMediaWithMode(4, SDHC_POLLING_MODE, SDHC_HCLK, 4, SDHC_CHANNEL_1, &oSdhc)) {
    	UART_Printf("\nError: OpenMedia() failed.");
    }

    if (!FAT_LoadFileSystem(&oSdhc)) {
    	UART_Printf("\nError: LoadFileSystem() failed.");
    }
   	//oSdhc.ReadFile(filename, uCopyDestAddr, uSizeRead);
	FAT_ReadFile3(filename, uCopyDestAddr, &uSizeRead, &oSdhc);

    if(uSizeRead == 0) {
    	UART_Printf("\nError: golden image file load fail");
    }

	FAT_UnloadFileSystem(&oSdhc);
	SDHC_CloseMedia(&oSdhc);

	UART_Printf("\n%s golden image file loading done.\n", filename);

	/* convert surface data to 24-bit RGB format */
	int ret = fglSysAssembleToBMP (	gSurfaceInfo.width,
        								gSurfaceInfo.height,
        								gSurfaceInfo.stride,
        								gSurfaceInfo.pixelFormat,
        								gSurfaceInfo.pFBBaseAddr,
        								pImageData
        						  );

	if(ret != FGL_ERR_NO_ERROR)
	{
		UART_Printf("Failed to assemble BMP file.\n");
        return ERROR;
	}

	for(int i = 0; i < (uSizeRead - 0x36); i++)
	{
	    if(*pGoldenImageData++ != *pNewImagenData++)
	        MismatchPixelCnt++;
	}

	return MismatchPixelCnt;
}


int LoadTextureImage(const char *filename)
{

    u32 uSizeRead = 0;

	UART_Printf("\nTexture image load at 0x%08X. wait...", FIMG_TEXTURE_MEMORY);

    if(gFimgContext.TexureFilePath == SEMIHOST_PROJECT_RELATIVE)
    {
    	strcpy(gFileName, gTexPathName);
    	strcat(gFileName, filename);
    	LoadFromFile(gFileName, FIMG_TEXTURE_MEMORY);
    	UART_Printf("\n%s texture file loading done.", gFileName);
    }

    else if(gFimgContext.TexureFilePath == DIRECT_ACCESS_MMC)
    {
	    if (!SDHC_OpenMediaWithMode(4, SDHC_POLLING_MODE, SDHC_HCLK, 4, SDHC_CHANNEL_1, &oSdhc)) {
	    	UART_Printf("\nError: OpenMedia() failed.");
	    }

	    if (!FAT_LoadFileSystem(&oSdhc)) {
	    	UART_Printf("\nError: LoadFileSystem() failed.");
	    }

		FAT_ReadFile3(filename, FIMG_TEXTURE_MEMORY, &uSizeRead, &oSdhc);

	    if(uSizeRead == 0) {
	    	UART_Printf("\nError: golden image file load fail");
	    }

		FAT_UnloadFileSystem(&oSdhc);
		SDHC_CloseMedia(&oSdhc);

	    UART_Printf("\n%s golden image file loading done.\n", filename);

    }

    else if(gFimgContext.TexureFilePath == SEMIHOST_ABSOLUTE_PATH) {
    	strcpy(gFileName, gTexPathName);
    	strcat(gFileName, filename);
    	LoadFromFile(gFileName, FIMG_TEXTURE_MEMORY);
        UART_Printf("\n%s texture file loading done.", gFileName);
    }
    else {
        UART_Printf("\n%sError: The location of texture images is not correct.");
        return ERROR;
    }

    return NO_ERROR;
}

int LoadGeometryData(const char *filename)
{
    	u32 uSizeRead = 0;
	//unsigned int uCopyDestAddr = (FIMG_GEOMETRY_MEMORY + 0x1000000);
	unsigned int uCopyDestAddr = (FIMG_GEOMETRY_MEMORY);

	UART_Printf("\nGeometry data load at 0x%08X. wait...", FIMG_GEOMETRY_MEMORY);

    if(gFimgContext.TexureFilePath == SEMIHOST_PROJECT_RELATIVE)
    {
    	strcpy(gFileName, gObjPathName);
    	strcat(gFileName, filename);
    	LoadFromFile(gFileName, FIMG_GEOMETRY_MEMORY);
    	UART_Printf("\n%s geometry data loading done.", gFileName);
    }
    else if(gFimgContext.TexureFilePath == DIRECT_ACCESS_MMC)
    {
    	// Copy texture image to dram

	    if (!SDHC_OpenMediaWithMode(4, SDHC_POLLING_MODE, SDHC_HCLK, 4, SDHC_CHANNEL_1, &oSdhc)) {
	    	UART_Printf("\nError: OpenMedia() failed.");
	    }

	    if (!FAT_LoadFileSystem(&oSdhc)) {
	    	UART_Printf("\nError: LoadFileSystem() failed.");
	    }
		FAT_ReadFile3(filename, uCopyDestAddr, &uSizeRead, &oSdhc);

	    if(uSizeRead == 0) {
	    	UART_Printf("\nError: golden image file load fail");
    }

	FAT_UnloadFileSystem(&oSdhc);
	SDHC_CloseMedia(&oSdhc);

    UART_Printf("\n%s golden image file loading done.\n", filename);

    }
    else if(gFimgContext.TexureFilePath == SEMIHOST_ABSOLUTE_PATH) {
    	strcpy(gFileName, gObjPathName);
    	strcat(gFileName, filename);
    	LoadFromFile(gFileName, FIMG_GEOMETRY_MEMORY);
        UART_Printf("\n%s geometry data file loading done.", gFileName);
    }
    else {
        UART_Printf("\n%sError: The location of geometry data is not correct.");
        return ERROR;
    }

    return NO_ERROR;
}

int ConfigureTest(void)
{
	int sel;

	const Test_Func Config[] =
	{
	    0,                "EXIT", 
	    _SetScreenResol,  "UART_Printflay Resolution Setting", 
	    _SetDumpImageFmt, "File Format To Dump Frame Buffer",
	    _SetShadingMode,  "Shading Mode To Render 3D object",
	    _SetTexImageDir,  "Texture Image Location setting",
	    _SetConfigFile,   "Configuable XML File setting",
	    0,                "Print Configurations",
	    0,                "Start to Test a Specific Vector",
	    0,                "Disassemble loaded shader programs",  
	    0,                0 
	};

	while(1)
	{

		UART_Printf("\nFIMG-3DSE Test Congiguration Setting\n");
		for (int i=0; (int)Config[i].funcName != 0; i++)
			UART_Printf("%2d: %s\n", i, Config[i].funcName);

		UART_Printf("\nSelect the functions to set : ");
		sel = UART_GetIntNum();
		UART_Printf("\n");

		if(sel == 0)
		    break;

        if(sel == 6)
            _PrintConfiguration();
        
        if(sel == 7)
        {
    		for(int i = 0; i < (sizeof(Vectors)/sizeof(TestVectorList) - 1) ; i++)
    		{
    		    UART_Printf("%02d. %-15s\t", (i+1), Vectors[i].pVectorName);
    		    if(i%3 == 0) UART_Printf("\n");
    		}
    		
    		UART_Printf("\n\nSelect the number of vector : ");
    		gFimgContext.uStartPointVector = UART_GetIntNum();
    		gFimgContext.uStartPointVector--;
    		UART_Printf("\n");
    		
    		if(gFimgContext.uStartPointVector < sizeof(Vectors)/sizeof(TestVectorList))
    		    gFimgContext.bIsSpecificTest = true;
    		else
    		    UART_Printf("Oops! >> Out of range in vector list\n");
        }
        
        if(sel == 8)
            _DisassembleShader();
            //UART_Printf("Oops! >> Under construction!\n\n");
        
        if(sel > 0 && sel < (sizeof(Config)/sizeof(Test_Func) - 4))
        {
    		if((Config[sel].func)(&gFimgContext) == NO_ERROR)
    		{
    		    //UART_Printf("\n %-15s .......................... OK\n", Config[sel].funcName);
    		    UART_Printf("\nPrinting set information\n");
    		    _PrintConfiguration();
    		}
    		else
        		UART_Printf("\n %-15s .......................... FAIL\n", Config[sel].funcName);
        }
	}

    return NO_ERROR;
}

 
int Texture2DFormat(void)
{
 	int ret;
    int sel;
    char FileName[256]; 
    unsigned int uTotalBmpPixel = gSurfaceInfo.width * gSurfaceInfo.height * 3; /* BMP 24bit */ 

    Test_Texfmt tex2d[] =
    {
        0, 		0,        	"Exit",
        0, 		0x80,     	"A1R5G5B5",
        1, 		0x20100,  	"R5G6B5",
        2, 		0x40180,  	"A4R4G4B4",
        //3, 	0x60200,  	"DEPTH24(N/A)"
        4, 		0x80280,  	"A8L8",
        5, 		0xA0300,  	"L8",
        6, 		0xB0380,  	"A8R8G8B8",
        7, 		0xF0380,  	"P1",
        8, 		0xF2380,  	"P2",
        9, 		0xF6380,  	"P4",
        10,		0xFE380,  	"P8",
        //11,	0x0,    	"S3TC",
        12,		0x10E380, 	"Y1VY0U",
        13,		0x12E380, 	"VY1UY0",
        14,		0x14E380, 	"Y1UY0V",
        15,		0x16E380,  	"UY1VY0",
        0,      0,          0
    };

	if(gFimgContext.bIsAllTest)
	{
        for(int i = 1; i < (sizeof(tex2d)/sizeof(Test_Texfmt) - 1); i++)
        {
            ret = TexFmt2D((FGL_TexelFormat)tex2d[i].index, tex2d[i].offset);

    		if(ret == NO_ERROR) 
    		{
    		    UART_Printf("\n Rendering %-15s .......................... OK\n", tex2d[i].texName);
                strcpy(FileName, "V150_TU_BD_0001_");
                strcat(FileName, tex2d[i].texName);
    		    
                if(gFimgContext.FBDumpFileFmt != NO_USE)
                {
    		    DumpFrameBuffer(FileName);
    		    g_nTestSuccessCount++;
    		}
    		else 
    		{
                    unsigned int uResult = CompareFrameBuffer(FileName);
                    if( uResult != 0)
                    {
                        UART_Printf("Verify TexFmt2D_%-15s ................... FAIL\n", tex2d[i].texName);
                        UART_Printf("Error pixel %4.1f%% is different against %s.bmp file\n\n", 
                            ((float)uResult/(float)uTotalBmpPixel*100.f), FileName);    
                        g_nTestFailCount++;
                    } 
                    else
                    {
                        UART_Printf("Verify TexFmt2D_%-15s ................... PASS\n\n", tex2d[i].texName);
                        g_nTestSuccessCount++;
                    }
                }
       		}
    		else 
    		{
    		    UART_Printf("\n Rendering %-15s .......................... FAIL\n", tex2d[i].texName);
    		    g_nTestFailCount++;
    		}

    		g_nTestedVectorCount++;
        }
	}
	else
	{
    	while(1)
    	{
    		UART_Printf("\n2D Texture Format\n");
    		for (int i = 0; (int)tex2d[i].texName != 0; i++)
    		{
      			UART_Printf("%2d: %-13s", i, tex2d[i].texName);
  		        if((i+1)%3 == 0) UART_Printf("\n");
    		}

    		UART_Printf("\nSelect the function to test : ");
    		sel = UART_GetIntNum();
    		UART_Printf("\n");

    		if(sel == 0)
    		    break;

    		ret = TexFmt2D((FGL_TexelFormat)tex2d[sel].index, tex2d[sel].offset);

     		if(ret == NO_ERROR)
    		{
    		    UART_Printf("\n %-15s .......................... OK\n", tex2d[sel].texName);
                if(gFimgContext.FBDumpFileFmt != NO_USE)
                {
                    strcpy(FileName, "V150_TU_BD_0001_");
                    strcat(FileName, tex2d[sel].texName);
                    DumpFrameBuffer(FileName);
                }
    		}
    		else
        		UART_Printf("\n %-15s .......................... FAIL\n", tex2d[sel].texName);
    	}
	}

    return NO_ERROR;
} 
 

int FrameBufferCtrl(void)
{
	int sel;
	int ret;
	char FileName[256];
	CSPACE eBpp;
	SurfaceInfo surface;
	unsigned int uTotalBmpPixel = gSurfaceInfo.width * gSurfaceInfo.height * 3; /* BMP 24bit */

	memcpy(&surface, &gSurfaceInfo, sizeof(gSurfaceInfo));

	Test_FBfmt fbfmt[]=
	{
	    0,  "EXIT",              0, 
	    1,  "FrameFmtRGB555",    "555",    //16
	    2,  "FrameFmtRGB565",    "565",    //16
	    3,  "FrameFmtARGB4444",  "4444",    //16
        4,  "FrameFmtARGB1555",  "1555",    //16
        5,  "FrameFmtRGBX888",   "0888",    //24
        6,  "FrameFmtARGB8888",  "8888",    //32
        0,  0,                   0
	};

	if(gFimgContext.bIsAllTest)
	{
	    for(int i = 1; i < (sizeof(fbfmt)/sizeof(Test_FBfmt) - 1); i++)
	    {
	        surface.pixelFormat = (FGL_PixelFormat)(i-1);
	        ret = FrameBufFmt((FGL_PixelFormat)(i-1));

    		if(ret == NO_ERROR)
    		{
    		    UART_Printf("\n Rendering%-15s .......................... OK\n", fbfmt[i].format);
                strcpy(FileName, "V150_PF_BD_0009_");
                strcat(FileName, fbfmt[i].dumpid);
     		    //DumpFrameBuffer(FileName);
    		    //g_nTestSuccessCount++;
                if(gFimgContext.FBDumpFileFmt != NO_USE)
                {
    		    DumpFrameBuffer(FileName);
    		    g_nTestSuccessCount++;
    		}
    		else
    		{
                    unsigned int uResult = CompareFrameBuffer(FileName);
                    if( uResult != 0)
                    {
                        UART_Printf(" Verify %-165s ................... FAIL\n", fbfmt[i].format);
                        UART_Printf("Error pixel %4.1f%% is different against %s.bmp file\n\n", 
                            ((float)uResult/(float)uTotalBmpPixel*100.f), FileName);    
 
  		        g_nTestFailCount++;
    		}
                    else
                    {
                        UART_Printf(" Verify %-16s ................... PASS\n\n", fbfmt[i].format);
                        g_nTestSuccessCount++;
                    }
                }
    		}
    		else
    		{
      		    UART_Printf("\n Rendering %-15s .......................... FAIL\n", fbfmt[i].format);
  		        g_nTestFailCount++;
	    }

	    g_nTestedVectorCount++;
	    }
	}
	else
	{
    	while(1)
    	{
    		UART_Printf("\nFrame Buffer Format Sub-Test Vector\n");
    		for (int i=0; (int)fbfmt[i].format != 0; i++)
    			UART_Printf("%2d: %s\n", i, fbfmt[i].format);

    		UART_Printf("\nSelect the Frame Buffer Format : ");
    		sel = UART_GetIntNum();
    		UART_Printf("\n");

    		if(sel == 0)
    		{
	    		eBpp = RGB16;
	    		//eBpp = RGB24;
    		    	
			LCD_SetWinOnOff(0, WIN0);
			LCD_InitDISPC(eBpp, FIMG_COLOR_BUFFER, WIN0, true);
			LCD_SetWinOnOff(1, WIN0);

			fglSysInit(0.0, 0.0, 240, 320, 0.0, 1.0);
			
    		    	break;
    		}
    		
    		if(sel > 0 && sel < 5)
    		{
	    		eBpp = RGB16;
	    		//eBpp = RGB24;
				
			LCD_SetWinOnOff(0, WIN0);
			LCD_InitDISPC(eBpp, FIMG_COLOR_BUFFER, WIN0, true);
			LCD_SetWinOnOff(1, WIN0);

    		} 
    		else
    		{
	    		eBpp = RGB16;
	    		//eBpp = RGB24;
			LCD_SetWinOnOff(0, WIN0);
			LCD_InitDISPC(eBpp, FIMG_COLOR_BUFFER, WIN0, true);
			LCD_SetWinOnOff(1, WIN0);
    		}

    		ret = FrameBufFmt((FGL_PixelFormat)(sel-1));

     		if(ret == NO_ERROR)
    		{
    		    UART_Printf("\n %-15s .......................... OK\n", fbfmt[sel].format);
                if(gFimgContext.FBDumpFileFmt != NO_USE)
                {
                    strcpy(FileName, "V150_PF_BD_0009_");
                    strcat(FileName, fbfmt[sel].dumpid);
        		    DumpFrameBuffer(FileName);
                }
    		}
    		else
        		UART_Printf("\n %-15s .......................... FAIL\n", fbfmt[sel].format);
    	}
	}
	
	return NO_ERROR;
}

void _PrintConfiguration(void)
{
    char* temp[2];
    
    switch(gFimgContext.FBDumpFileFmt)
    {
    case BMP_FILE:
        temp[0] = "BMP";
        break;
    case PPM_FILE:
        temp[0] = "PPM";
        break;
    case NO_USE:
        temp[0] = "NO USE";
        break;
    default:
        break;
    }
    
    switch(gFimgContext.TexureFilePath)
    {
    case SEMIHOST_PROJECT_RELATIVE:
        temp[1] = "Project relative";
        break;
    case SEMIHOST_ABSOLUTE_PATH:
        temp[1] = "Absolute path";
        break;
    case DIRECT_ACCESS_MMC:
        temp[1] = "Use MMC";
        break;
    default:
        break;
    }
    
    UART_Printf("\n********************************************************");
    UART_Printf("\n*      FIMG-3DSE Ver 1.5 FPGA Test Configuration       *");
    UART_Printf("\n********************************************************");
    UART_Printf("\n     UART_Printflay Resolution ........... %d x %d", gFimgContext.uScreenPhyHSize, gFimgContext.uScreenPhyVSize);
    UART_Printf("\n       Dump File Format ........... %s", temp[0]);
    UART_Printf("\n           Shading Mode ........... %s", (gFimgContext.bShadeMode)?"true":"false");
    UART_Printf("\n Texture Image Location ........... %s", temp[1]);
    UART_Printf("\n   Configuable XML File ........... %s", (gFimgContext.bShadeMode)?"true":"false");
    UART_Printf("\n********************************************************\n");
}

int _SetScreenResol(Context3D * p3DContext)
{
	int sel;
	CSPACE eBpp = RGB24;
	unsigned int uResolHSize = 0;
	unsigned int uResolVSize = 0; 
	
	UART_Printf("\nResolution\n");
	UART_Printf("0 Skip     \t1 Default(240x320)\n");
	UART_Printf("2 320x240  \t3 480x360   \t4 640x480\n");
	UART_Printf("5 1280x1024\t6 1600x1200 \t7 1920x1080\n");

	UART_Printf("\nSelect the resolution to surface : ");
	sel = UART_GetIntNum();
	UART_Printf("\n");

	switch(sel)
	{
	case 1:
        uResolHSize = 240; 
        uResolVSize = 320;
        break;
	case 2:
        uResolHSize = 320; 
        uResolVSize = 240;
        break;
	case 3:
        uResolHSize = 480; 
        uResolVSize = 360;
		break;
	case 4:
        uResolHSize = 640; 
        uResolVSize = 480;
		break;
	case 5:
        uResolHSize = 1280; 
        uResolVSize = 1024;
		break;
	case 6:
        uResolHSize = 1600; 
        uResolVSize = 1200;
		break;
	case 7:
        uResolHSize = 1920; 
        uResolVSize = 1080;
		break;
	default:
        uResolHSize = 240;  
        uResolVSize = 320;
		break;
	}
	
	p3DContext->uScreenPhyHSize = uResolHSize; 
	p3DContext->uScreenPhyVSize = uResolVSize;

	LCD_SetWinOnOff(0, WIN0);
	LCD_InitDISPC(eBpp, FIMG_COLOR_BUFFER, WIN0, true);
	LCD_SetWinOnOff(1, WIN0);

	fglSysInit(0.0, 0.0, (float)uResolHSize, (float)uResolVSize, 0.0, 1.0);

	return NO_ERROR;
}

int _SetDumpImageFmt(Context3D * p3DContext)
{
	UART_Printf("\nFrame buffer dump image format");
	UART_Printf("\n0. Skip    1. BMP(default)    2. PPM       3. No use");
	UART_Printf("\nSelect the format : ");
	int sel = UART_GetIntNum();
	UART_Printf("\n");

	if (sel > 0 && sel < 4)
	    p3DContext->FBDumpFileFmt = (FBDumpFileFmt)sel;
    
    return NO_ERROR;
}

int _SetShadingMode(Context3D * p3DContext)
{
	UART_Printf("\nFlat and smooth shading are specified. The default is SMOOTH\n");
	UART_Printf("0. Skip  \t1. SMOOTH(default)  \t2. FLAT\n");
	UART_Printf("\nSelects flat or smooth shading : ");
	int sel = UART_GetIntNum();
	UART_Printf("\n");

	if (sel == 1)
		p3DContext->bShadeMode = true;
	else if (sel == 2)
		p3DContext->bShadeMode = false;
    
	return NO_ERROR;
}

int _SetTexImageDir(Context3D * p3DContext)
{
	UART_Printf("\nTexture image directory loacation for example:");
	UART_Printf("\n\tRVD: ../../fimg3d/media/ (relative path)");
	UART_Printf("\n\tT32: C:/fimg3dse_fpga/fimg3d/media/(absolute path)\n");
	UART_Printf("\n 0. Skip");
	UART_Printf("\n 1. RVD(Project Relative)");
	UART_Printf("\n 2. T32(absolute path)");
	UART_Printf("\n 3. MMC(recommanded)\n");
	UART_Printf("\nSelect the location of texture images : ");
	int sel = UART_GetIntNum();
	UART_Printf("\n");

	if (sel > 0 && sel < 4)
        p3DContext->TexureFilePath = (TexFilePath)sel;

    if (sel == 2)
    {
    	UART_Printf("\nEnter the path name: ");
    	gets(gTexPathName);
    }
    
    return NO_ERROR;
}

int _SetConfigFile(Context3D * p3DContext)
{

    UART_Printf("\nXML format configuration file option\n");
    UART_Printf("0. No use  \t1. Use \n");
    UART_Printf("\nSelects the number if you use the XML configure file: ");
    int sel = UART_GetIntNum();
    UART_Printf("\n");

    if (sel == 0)
        p3DContext->bIsUseXmlConfig = false;
    else if (sel == 1)
        p3DContext->bIsUseXmlConfig = true;

    return NO_ERROR;
}


int _DisassembleShader()
{
	fglDasmVertexShader();
	fglDasmFragmentShader();
	return NO_ERROR;
}

