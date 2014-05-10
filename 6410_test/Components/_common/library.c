/**************************************************************************************
* 
*	Project Name : S3C6410 Validation
*
*	Copyright 2006~2008 by Samsung Electronics, Inc.
*	All rights reserved.
*
*	Project Description :
*		This software is only for validating functions of the S3C6410.
*		Anybody can use this software without our permission.
*  
*--------------------------------------------------------------------------------------
* 
*	File Name : library.c
*  
*	File Description : This file implements library functions.
*
*	Author : Haksoo,Kim
*	Dept. : AP Development Team
*	Created Date : 2006/11/08
*	Version : 0.1 
* 
*	History
*	- Created(Haksoo,Kim 2006/11/08)
*	- Added DownloadImageThruUsbOtg function (Haksoo,Kim 2007/01/24)
*	- Edited RPRINTF (OnPil,Shin (SOP) 2008/03/01)
*	- Added Pause() by OnPil, Shin (SOP) on 2008/03/04
*  
**************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>

#include "option.h"
#include "library.h"
#include "sfr6410.h"
#include "system.h"
#include "uart.h"
#include "gpio.h"
#include "otg_dev.h"
#include "intc.h"
#include "register_addr.h"

#define IMAGE_MAXSIZE 1600

//////////
// Function Name : InitUartPort
// Function Description : This function initializes gpio for debugging uart ch.
// Input :  ch, uart ch number
//			flowControl, whether flow control or not
// Output : NONE
// Version :
void InitUartPort(u8 ch, u8 flowControl)
{
	switch (ch)
	{
		default:
		case 0:
			if(flowControl == TRUE)
			{
				GPIO_SetFunctionEach(eGPIO_A, eGPIO_0, 2);
				GPIO_SetFunctionEach(eGPIO_A, eGPIO_1, 2);
				GPIO_SetFunctionEach(eGPIO_A, eGPIO_2, 2);
				GPIO_SetFunctionEach(eGPIO_A, eGPIO_3, 2);
			}
			else
			{
				GPIO_SetFunctionEach(eGPIO_A, eGPIO_0, 2);
				GPIO_SetFunctionEach(eGPIO_A, eGPIO_1, 2);
			}
			break;
			
		case 1:
			if(flowControl == TRUE)
			{
				GPIO_SetFunctionEach(eGPIO_A, eGPIO_4, 2);
				GPIO_SetFunctionEach(eGPIO_A, eGPIO_5, 2);
				GPIO_SetFunctionEach(eGPIO_A, eGPIO_6, 2);
				GPIO_SetFunctionEach(eGPIO_A, eGPIO_7, 2);
			}
			else
			{
				GPIO_SetFunctionEach(eGPIO_A, eGPIO_4, 2);
				GPIO_SetFunctionEach(eGPIO_A, eGPIO_5, 2);
			}
			break;
			
		case 2:
			GPIO_SetFunctionEach(eGPIO_B, eGPIO_0, 2);
			GPIO_SetFunctionEach(eGPIO_B, eGPIO_1, 2);
			break;
			
		case 3:
			GPIO_SetFunctionEach(eGPIO_B, eGPIO_2, 2);
			GPIO_SetFunctionEach(eGPIO_B, eGPIO_3, 2);
			break;		
	}
	
	return;	
}

//////////
// Function Name : InitLED
// Function Desctiption : This function initializes gpio for debugging LED
// Input : NONE
// Output : NONE
// Version :
void InitLED(void)
{
	GPIO_SetFunctionEach(eGPIO_N, eGPIO_12, 1);
	GPIO_SetFunctionEach(eGPIO_N, eGPIO_13, 1);
	GPIO_SetFunctionEach(eGPIO_N, eGPIO_14, 1);
	GPIO_SetFunctionEach(eGPIO_N, eGPIO_15, 1);	
	return;
}

//////////
// Function Name : DisplayLED
// Function Desctiption : This function controls debugging LED
// Input : data, LED value 
// Output : NONE
// Version :
void DisplayLED(u8 data)
{
	u32 temp;

	temp = GPIO_GetDataAll(eGPIO_N);
	temp = (temp & ~(0xf000))|((data&0xf)<<12);
	GPIO_SetDataAll(eGPIO_N, temp);

	return;
}


//////////
// Function Name : OpenConsole
// Function Description : This function opens uart and LED for debugging
// Input : NONE
// Output : NONE
// Version : 
void OpenConsole( void)
{	
	u8 ch=0;	//uart channel for debugging
	
	InitUartPort(ch, FALSE);
	UART_InitDebugCh(ch, 115200);
	InitLED();

	return;
}

//////////
// Function Name : GetIntNum
// Function Description : This function gets the number which a user enters
// Input : NONE
// Output : number, number which a user enters
// Version : 
s32 GetIntNum( void)
{
    char str[30];
    char *string = str;
    int base     = 10;
    int minus    = 0;
    int result   = 0;
    int lastIndex;    
    int i,j;
    
    gets(string);


	i=0; j=0;
	do {
		if (string[j]==0x8) {
			if (i>0)	i--; 
		} else 
			string[i++]=string[j];
	} while(string[j++]!=0);


    if(string[0]=='-') {
        minus = 1;
        string++;
    }
    
    if(string[0]=='0' && (string[1]=='x' || string[1]=='X')) {
        base    = 16;
        string += 2;
    }
    
    lastIndex = strlen(string) - 1;
    
    if(lastIndex<0)
        return -1;
    
    if(string[lastIndex]=='h' || string[lastIndex]=='H' ) {
        base = 16;
        string[lastIndex] = 0;
        lastIndex--;
    }

    if(base==10) {
        result = atoi(string);
        result = minus ? (-1*result):result;
    }
    else {
        for(i=0;i<=lastIndex;i++) {
            if(isalpha(string[i])) {
                if(isupper(string[i]))
                    result = (result<<4) + string[i] - 'A' + 10;
                else
                    result = (result<<4) + string[i] - 'a' + 10;
            }
            else
                result = (result<<4) + string[i] - '0';
        }
        result = minus ? (-1*result):result;
    }
    return result;
}

//////////
// Function Name : DownloadImageThruUart
// Function Description : This function downloads a certain image through uart
// Input : DownloadAddress, address to download the image 
// Output : FileSize, size of downloaded image
// Version : 
u32 DownloadImageThruUart(u8 *DownloadAddress)
{
	char buf[4];
	int i;
	u32	FileSize;
	u16	CheckSum=0,dnCS;

	UART_Printf("\nDownloadAddress : 0x%08x\n",DownloadAddress);
	UART_Printf("STATUS : ");

	//	To get the file size.
	for(i=0;i<4;i++)
		buf[i]=UART_Getc();

	FileSize=(buf[0])+(buf[1]<<8)+(buf[2]<<16)+(buf[3]<<24);
	FileSize-=4;
	
	for(i=0;i<FileSize;i++) {
		*(DownloadAddress+i)=UART_Getc();
		if((i&0x3ff)==0)
			putchar('#');
	}

	for(i=0;i<(FileSize-2);i++)
		CheckSum+=*((u8 *)(DownloadAddress+i));

	dnCS=*((u8 *)(DownloadAddress+FileSize-2))+
		(*( (u8 *)(DownloadAddress+FileSize-1) )<<8);

	if(CheckSum!=dnCS) {
		UART_Printf("\nChecksum Error!!! MEM : %x  DN : %x\n",CheckSum,dnCS);
		FileSize=0;
	} else {
		FileSize-=2;
		UART_Printf("\n%d bytes downloaded OK.\n",FileSize);
	}
    	
	return FileSize;
}

//////////
// Function Name : DownloadImageThruUsbOtg
// Function Description : This function downloads a certain image through usb otg
// Input : DownloadAddress, address to download the image
// Output : FileSize, size of downloaded image
// Version : 
extern USB_OPMODE eOpMode;
extern USB_SPEED eSpeed;
extern u8 download_run;
extern u32 tempDownloadAddress;
extern void __irq Isr_UsbOtg(void);
u8 g_bUsbInitFirst = true;
u32 DownloadImageThruUsbOtg(u8 *DownloadAddress)
{
	u32 uDownAddr, uDownFileSize, pDownPt, i;
	USB_SPEED eUsbSpeed;

	INTC_SetVectAddr(NUM_OTG, Isr_UsbOtg);
	INTC_Enable(NUM_OTG);
	
	if(g_bUsbInitFirst == true)
	{
		OTGDEV_InitOtg(eSpeed);
		g_bUsbInitFirst = false;
	}
		
	while(1)
	{
		if(OTGDEV_IsUsbOtgSetConfiguration()==true)
		{		
			OTGDEV_CheckEnumeratedSpeed(&eUsbSpeed);
			if(eUsbSpeed == USB_HIGH)
			{
				UART_Printf("\n!!! USB host is connected (Speed : High) !!!\n\n");
				DisplayLED(0xf);
			}
			else if(eUsbSpeed == USB_FULL)
			{
				UART_Printf("\n!!! USB host is connected (Speed : Full) !!!\n\n");
				DisplayLED(0x6);
			}
		
			break;
		}
	}

	OTGDEV_ClearDownFileInfo();

	OTGDEV_SetOpMode(eOpMode);
	
	if(DownloadAddress!=0)
	{
		tempDownloadAddress = (u32)DownloadAddress;
	}
	else
	{	
	    UART_Printf("Enter a new temporary download address(0x...):");
	    tempDownloadAddress=(u32)UART_GetIntNum();
	    if(tempDownloadAddress==0xffffffff)
	    {
	    	tempDownloadAddress=DefaultDownloadAddress;
	    }		
	}
	
	UART_Printf("The temporary download address is 0x%x.\n\n",tempDownloadAddress);
	
	UART_Printf("Select a file to download in DNW\n");
	UART_Printf("If you want to quit, press 'x' key\n");

	i = 0;

	do
	{
		OTGDEV_GetDownFileInfo(&uDownAddr, &uDownFileSize, &pDownPt);
		
		if(GetKey() == 'x')
		{
			return 0;
		}      
       
		if(i%0x100000==0)
			DisplayLED(0x6);
		if(i%0x100000==0x80000)
			DisplayLED(0x9);
		i++;		
	}while((uDownAddr == 0) ||(uDownFileSize == 0));
	
	UART_Printf("\n[ADDRESS:%xh,TOTAL:%d(0x%x)]\n", uDownAddr, uDownFileSize, (uDownFileSize-10));

	do
	{
		OTGDEV_GetDownFileInfo(&uDownAddr, &uDownFileSize, &pDownPt);

		if(GetKey() == 'x')
		{
			return 0;
		} 
	}while((pDownPt - uDownAddr)<(uDownFileSize - 8));

	OTGDEV_VerifyChecksum();
	
	return (uDownFileSize-10);
}

//////////
// Function Name : UploadImageThruUsbOtg
// Function Description : This function uploads a certain image through usb otg
// Input : NONE
// Output : NONE
// Version : 
void UploadImageThruUsbOtg(void)
{
	u32 uUpAddr, uUpFileSize, pUpPt, i;
	USB_SPEED eUsbSpeed;

	INTC_SetVectAddr(NUM_OTG, Isr_UsbOtg);
	INTC_Enable(NUM_OTG);
	
	if(g_bUsbInitFirst == true)
	{
		OTGDEV_InitOtg(eSpeed);
		g_bUsbInitFirst = false;
	}
		
	while(1)
	{
		if(OTGDEV_IsUsbOtgSetConfiguration()==true)
		{		
			OTGDEV_CheckEnumeratedSpeed(&eUsbSpeed);
			if(eUsbSpeed == USB_HIGH)
			{
				UART_Printf("\n!!! USB host is connected (Speed : High) !!!\n\n");
				DisplayLED(0xf);
			}
			else if(eUsbSpeed == USB_FULL)
			{
				UART_Printf("\n!!! USB host is connected (Speed : Full) !!!\n\n");
				DisplayLED(0x6);
			}
		
			break;
		}
	}

	OTGDEV_ClearDownFileInfo();
	OTGDEV_ClearUpFileInfo();

	OTGDEV_SetOpMode(eOpMode); 

	UART_Printf(" Configure the upload address and size in DNW\n");
	UART_Printf(" If you want to quit, press 'x' key\n");

	i = 0;

	do
	{
		OTGDEV_GetUpFileInfo(&uUpAddr, &uUpFileSize, &pUpPt);
	
		if(GetKey() == 'x')
		{
			return;
		}      
       
		if(i%0x100000==0)
			DisplayLED(0x6);
		if(i%0x100000==0x80000)
			DisplayLED(0x9);
		i++;		
	}while((uUpAddr == 0) || (uUpFileSize == 0));
	
	UART_Printf("\n[ADDRESS:%xh,TOTAL:%d]\n", uUpAddr, uUpFileSize);

	do
	{
		OTGDEV_GetUpFileInfo(&uUpAddr, &uUpFileSize, &pUpPt);

		if(GetKey() == 'x')
		{
			return;
		} 
	}while((pUpPt - uUpAddr)<uUpFileSize);
}

//////////
// Function Name : Delay
// Function Description : 
// Input : usec, time in 100us unit
// Output : NONE
// Version : 
u32 delayLoopCount;
void Delay(u32 usec)
{
	u32 i=0;
    
    for(;usec>0;usec--)
    {
    	for(i=0;i<delayLoopCount;i++);
    }
}

//////////
// Function Name : Pow
// Function Description : Calculates x raise to the power of y
// Input : x, y
// Output : uResult - result value
// Version : 
u32 Pow(u32 x, u32 y)
{
    u32 i, uResult = 1;
    
    for(i=0 ; i<y ; i++)
    {
    	uResult = uResult*x;
    }
    
    return uResult;
}  

//////////
// Function Name : Copy
// Function Description : Copy from src address to dst address by words count
// Input : sa, da, words
// Output : void
// Version : 
void Copy(u32 sa, u32 da, u32 words)
{
	u32 i;	
	for (i=0; i<words; i++)
		*(u32 *)(da+i*4) = *(u32 *)(sa+i*4);
}

//////////
// Function Name : Copy8
// Function Description : Copy from src address to dst address by bytes count
// Input : sa, da, bytes
// Output : void
// Version : 
void Copy8(u32 sa, u32 da, u32 bytes)
{
	u32 i;
	for (i=0; i<bytes; i++)
		*(u8 *)(da+i) = *(u8 *)(sa+i);
}

//////////
// Function Name : Copy16
// Function Description : Copy from src address to dst address by bytes count
// Input : sa, da, bytes
// Output : void
// Version : 
void Copy16(u32 sa, u32 da, u32 Hwords)
{
	u32 i;
	for (i=0; i<Hwords; i++)
		*(u16 *)(da+i*2) = *(u16 *)(sa+i*2);
}

//////////
// Function Name : Compare
// Function Description : compare data
// Input : a0, a1, words
// Output : ret
// Version : 
u8 Compare( u32 a0,  u32 a1,  u32 words)
{
	volatile u32 d0,d1;
	volatile u32 *pD0, *pD1;
	u8 ret = true;
	u32  ecnt = 0;
	u32 i;
	
	pD0 = (volatile u32 *)(a0);
	pD1 = (volatile u32 *)(a1);
	
	//UART_Printf("\n");	

	for (i=0; i<words; i++)
	{
		
		d0=*pD0;
		d1=*pD1;

		if (d0!= d1) 
		{
			ret = false;
			UART_Printf(" %08x=%08x <-> %08x=%08x\n", pD0, d0, pD1, d1);
			ecnt++;
		}
		pD0++;
		pD1++;
	}
/*
	if (ret == false)
	{
		Assert(0);		
		UART_Printf("\n");
	}
*/
	return ret;
}

//////////
// Function Name : Dump32
// Function Description : dump data
// Input : addr, words
// Output : ret
// Version : 
void Dump32(u32 addr, u32 words)
{
	int i, j;
	u32 *pMem;

	pMem = (u32 *)(addr/4*4);

	UART_Printf("\n");
	for(i=0; i<words; )
	{
		UART_Printf(" %04x: ", i);

		for(j=0; j<8; j++)
			UART_Printf("%08x ", *pMem++),
			i++;
		UART_Printf("\n");
	}
}

//////////
// Function Name : Dump16
// Function Description : dump data
// Input : addr, hwords
// Output : ret
// Version : 
void Dump16(u32 addr, u32 hwords)
{
	int i, j;
	u16 *pMem;

	pMem = (u16 *)(addr/2*2);

	UART_Printf("\n");
	for(i=0; i<hwords; )
	{
		UART_Printf(" %04x: ", i);

		for(j=0; j<16; j++)
			UART_Printf("%04x ", *pMem++),
			i++;
		UART_Printf("\n");
	}
}


//////////
// Function Name : Dump8
// Function Description : dump data
// Input : addr, bytes
// Output : ret
// Version :
void Dump8(u32 addr, u32 bytes)
{
	int i, j;
	u8 *pMem;

	pMem = (u8 *)addr;

	UART_Printf("\n");
	for(i=0; i<bytes; )
	{
		UART_Printf(" %04x: ", i);

		for(j=0; j<16; j++)
			UART_Printf("%02x ", *pMem++),
			i++;
		UART_Printf("\n");
	}
}

//////////
// Function Name : Stop
// Function Description : This function is called from "Assert"
// Version : 
void Stop(void) 
{ 
	while(1); 
}


//////////
// Function Name : LoadFromFile
// Function Description : This function download the file from Host(PC)
// Input :  fileName - file name
//			uDstAddr - address to download
// Output : file size
// Version :
// added by rb1004
u32 LoadFromFile(const char* fileName, u32 uDstAddr)
{
	u32 uFileSize;
	
#if (SEMIHOSTING)
	u32 read;
	u32 i, uQuotient, uRemainder, uWriteAddr;

	FILE* fp = fopen(fileName, "rb");
	Assert(fp);
	fseek(fp, 0, SEEK_END);
	uFileSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
    //rewind(fp);
	uWriteAddr = uDstAddr;
    	uQuotient = uFileSize/10000;
	uRemainder = uFileSize%10000;

	for(i=0; i<uQuotient ; i++)
	{
		read = fread((void *)uWriteAddr, sizeof(unsigned char), 10000, fp);
		Assert(read==10000);
		uWriteAddr += 10000;
	}
	read = fread((void *)uWriteAddr, sizeof(unsigned char), uRemainder, fp);
	Assert(read==uRemainder);	
	
	fclose(fp);
	
#else

	UART_Printf(" Sorry! It is not ready.\n");

#endif

	return uFileSize;
}

void LoadFromFile1(const char* fileName, u32 uDstAddr, u32* uFileSize)
{

#if SEMIHOSTING

	u32 read, i, uQuotient, uRemainder, uWriteAddr;

	FILE* fp = fopen(fileName, "rb");
	Assert(fp);
	fseek(fp, 0, SEEK_END);
	*uFileSize = ftell(fp);
    	rewind(fp);

	uWriteAddr = uDstAddr;
    	uQuotient = *uFileSize/10000;
	uRemainder = *uFileSize%10000;

	for(i=0; i<uQuotient ; i++)
	{
		read = fread((void *)uWriteAddr, sizeof(unsigned char), 10000, fp);
		Assert(read==10000);
		uWriteAddr += 10000;
	}
	read = fread((void *)uWriteAddr, sizeof(unsigned char), uRemainder, fp);
	Assert(read==uRemainder);	
	
	fclose(fp);


	//read = fread((void *)uDstAddr, sizeof(unsigned char), *uFileSize, fp);
	//Assert(read==*uFileSize);
	
	//fclose(fp);
	
#else

	UART_Printf(" Sorry! It is not ready.\n");

#endif
}


//////////
// Function Name : SaveToFile
// Function Description : This function upload the file to Host(PC)
// Input :  fileName - file name
//		  fileSize - upload file size
//		  uSrcAddr - address to download
// Output : None
// Version :
// added by rb1004
void SaveToFile(const char* fileName, u32 fileSize, u32 uSrcAddr)
{
#if (SEMIHOSTING)
	u32 written;
	u32 i, uQuotient, uRemainder, uWriteAddr;

	FILE* fp = fopen(fileName, "wb");
	Assert(fp);

	uWriteAddr = uSrcAddr;
    	uQuotient = fileSize/10000;
	uRemainder = fileSize%10000;


	//written = fwrite((void *)uSrcAddr, sizeof(unsigned char), fileSize, fp);
	//Assert(written == fileSize);
	
	for(i=0; i<uQuotient ; i++)
	{
		written = fwrite((void *)uWriteAddr, sizeof(unsigned char), 10000, fp);
		Assert(written==10000);
		uWriteAddr += 10000;
	}
	written = fwrite((void *)uWriteAddr, sizeof(unsigned char), uRemainder, fp);
	Assert(written==uRemainder);	

	fclose(fp);
#else

	UART_Printf(" Sorry! It is not ready.\n");

#endif
}


//////////
// Function Name : ConvertBmpToRgb16bpp
// Function Description : This function converts the BMP format to 16RGB format
// Input :  fromAddr - source address
//			toAddr - destination address
//			xSize - image x size
//			ySize - image y size
// Output : file size
// Version :
void ConvertBmpToRgb16bpp(u32 fromAddr, u32 toAddr, u32 xSize, u32 ySize)
{
	u32 i,x,y;
	u16 *lcdFramePtr;
	u16 *tempPtr;
	u16 temp[IMAGE_MAXSIZE];
	u32 tempBuffer = fromAddr + 0x400000; // up to 1600x1200
	u8 b,g,r;
	
	u8 *srcPtr=(u8 *)(fromAddr+54);
	lcdFramePtr=(u16 *)(toAddr);

	for(y=0;y<ySize;y++) {
		for(x=0;x<xSize*3;x++) {
			b=*srcPtr++;
			g=*srcPtr++;
			r=*srcPtr++;

			*lcdFramePtr++=(r>>3)<<11|(g>>2)<<5|(b>>3)<<0;
		}
	}

	lcdFramePtr=(u16 *)(toAddr+xSize*ySize*2-2);
	tempPtr=(u16 *)(tempBuffer);
	for(y=0;y<xSize*ySize;y++) {
		*tempPtr++=*lcdFramePtr--;
	}

	lcdFramePtr=(u16 *)(toAddr);
	tempPtr=(u16 *)(tempBuffer);
	for(y=0;y<ySize;y++) {
		for(x=0;x<xSize;x++) {
			temp[(xSize-1)-x]=*tempPtr++;
		}
		if(x==xSize) {
			for(i=0;i<xSize;i++)
				*lcdFramePtr++=temp[i];
		}
	}
}


//////////
// Function Name : ConvertBmpToRgb24bpp
// Function Description : This function converts the BMP format to 24RGB format
// Input :  fromAddr - source address
//			toAddr - destination address
//			xSize - image x size
//			ySize - image y size
// Output : file size
// Version :
void ConvertBmpToRgb24bpp(u32 fromAddr, u32 toAddr, u32 xSize, u32 ySize)
{
	u32 i,x,y;
	u32 *lcdFramePtr,*tempPtr;
	u32 temp[IMAGE_MAXSIZE];
	u32 tempBuffer = fromAddr + 0x760000; // up to 1600x1200
    u8 b,g,r;
	
	u8 *srcPtr=(u8 *)(fromAddr+54);
	lcdFramePtr=(u32 *)(toAddr);
   	for(y=0;y<xSize*ySize;y++) {
    	b=*srcPtr++;
    	g=*srcPtr++;
	    r=*srcPtr++;

	    *lcdFramePtr++=(r<<16)|(g<<8)|(b<<0);
	}

	lcdFramePtr=(u32 *)(toAddr+xSize*ySize*4-4);
	tempPtr=(u32 *)(tempBuffer);
    for(y=0;y<xSize*ySize;y++) {
	    *tempPtr++=*lcdFramePtr--;
    }

	lcdFramePtr=(u32 *)(toAddr);
	tempPtr=(u32 *)(tempBuffer);
    for(y=0;y<ySize;y++) {
	    for(x=0;x<xSize;x++) {
			temp[(xSize-1)-x]=*tempPtr++;
	    }
	    if(x==xSize) {	    	
		   	for(i=0;i<xSize;i++)
		    	*lcdFramePtr++=temp[i];
		}
	}	

}

void ConvertImgSzToNumber(IMG_SIZE eSize, u32* uHsz, u32* uVsz)
{
	if (eSize == UXGA)
		*uHsz = 1600, *uVsz = 1200; 
	else if (eSize == SXGA)
		*uHsz = 1280, *uVsz = 1024; 
	else if (eSize == SVGA)
		*uHsz = 800, *uVsz = 600;		
	else if (eSize == VGA)
		*uHsz = 640, *uVsz = 480; 
	else if (eSize == QVGA)
		*uHsz = 320, *uVsz = 240; 
	else if (eSize == QQVGA)
		*uHsz = 160, *uVsz = 120; 
	else if (eSize == CIF)
		*uHsz = 352, *uVsz = 288; 
	else if (eSize == QCIF)
		*uHsz = 172, *uVsz = 144; 
	else
		Assert(0);
}


REGINFO		sRegInfo[] = 
{
#if 0
	//MFC
 	{"rCodeRun           ", MFC_BASE+0x00, 1 , WO, DPDB, 0, 0},
 	{"rCodeDownLoad      ", MFC_BASE+0x04, 32, WO, DPDB, 0, 0},
 	{"rHostIntReq        ", MFC_BASE+0x08, 1 , WO, DPDB, 0, 0},
 	{"rBitIntClear       ", MFC_BASE+0x0C, 1 , WO, DPDB, 0, 0},
 	{"rBitIntSts         ", MFC_BASE+0x10, 1 , RO, DPDB, 0, 0},
 	{"rBitCodeReset      ", MFC_BASE+0x14, 1 , WO, DPDB, 0, 0},
 	{"rBitCurPc          ", MFC_BASE+0x18, 14, RO, DPDB, 0, 0},
 	//
 	{"rCodeBufAddr       ", MFC_BASE+0x100, 32, RW, DPDB, 0, 0},
 	{"rWorkBufAddr       ", MFC_BASE+0x104, 32, RW, DPDB, 0, 0},
 	{"rParaBufAddr       ", MFC_BASE+0x108, 32, RW, DPDB, 0, 0},
 	{"rBitStreamCtrl     ", MFC_BASE+0x10C, 4 , RW, DPDB, 0, 0},
 	{"rFrameMemCtrl      ", MFC_BASE+0x110, 1 , RW, DPDB, 0, 0},
 	{"rDecFuncCtrl       ", MFC_BASE+0x114, 1 , RW, DPDB, 0, 0},
 	{"rBitWorkBufCtrl    ", MFC_BASE+0x11C, 1 , RW, DPDB, 0, 0},
 	{"rBitStreamRdPtr0   ", MFC_BASE+0x120, 32, RW, DPDB, 0, 0},
 	{"rBitStreamWrPtr0   ", MFC_BASE+0x124, 32, RW, DPDB, 0, 0},
 	{"rBitStreamRdPtr1   ", MFC_BASE+0x128, 32, RW, DPDB, 0, 0},
 	{"rBitStreamWrPtr1   ", MFC_BASE+0x12C, 32, RW, DPDB, 0, 0},
 	{"rBitStreamRdPtr2   ", MFC_BASE+0x130, 32, RW, DPDB, 0, 0},
 	{"rBitStreamWrPtr2   ", MFC_BASE+0x134, 32, RW, DPDB, 0, 0},
 	{"rBitStreamRdPtr3   ", MFC_BASE+0x138, 32, RW, DPDB, 0, 0},
 	{"rBitStreamWrPtr3   ", MFC_BASE+0x13C, 32, RW, DPDB, 0, 0},
 	{"rBitStreamRdPtr4   ", MFC_BASE+0x140, 32, RW, DPDB, 0, 0},
 	{"rBitStreamWrPtr4   ", MFC_BASE+0x144, 32, RW, DPDB, 0, 0},
 	{"rBitStreamRdPtr5   ", MFC_BASE+0x148, 32, RW, DPDB, 0, 0},
 	{"rBitStreamWrPtr5   ", MFC_BASE+0x14C, 32, RW, DPDB, 0, 0},
 	{"rBitStreamRdPtr6   ", MFC_BASE+0x150, 32, RW, DPDB, 0, 0},
 	{"rBitStreamWrPtr6   ", MFC_BASE+0x154, 32, RW, DPDB, 0, 0},
 	{"rBitStreamRdPtr7   ", MFC_BASE+0x158, 32, RW, DPDB, 0, 0},
 	{"rBitStreamWrPtr7   ", MFC_BASE+0x15C, 32, RW, DPDB, 0, 0},
 	{"rBusyFlag          ", MFC_BASE+0x160, 1 , RO, DPDB, 0, 0},
 	{"rRunCommand        ", MFC_BASE+0x164, 3 , RW, DPDB, 0, 0},
 	{"rRunIndex          ", MFC_BASE+0x168, 3 , RW, DPDB, 0, 0},
 	{"rRunCodStd         ", MFC_BASE+0x16C, 3 , RW, DPDB, 0, 0},
 	{"rIntEnable         ", MFC_BASE+0x170, 16, RW, DPDB, 0, 0},
 	{"rIntReason         ", MFC_BASE+0x174, 16, RW, DPDB, 0, 0},
 	//
 	{"MFC_BASE+0x180     ", MFC_BASE+0x180, 32, RW, DPDB, 0, 0},
 	{"MFC_BASE+0x184     ", MFC_BASE+0x184, 32, RW, DPDB, 0, 0},
 	{"MFC_BASE+0x188     ", MFC_BASE+0x188, 32, RW, DPDB, 0, 0},
 	{"MFC_BASE+0x18C     ", MFC_BASE+0x18C, 32, RW, DPDB, 0, 0},
 	{"MFC_BASE+0x190     ", MFC_BASE+0x190, 32, RW, DPDB, 0, 0},
 	{"MFC_BASE+0x194     ", MFC_BASE+0x194, 32, RW, DPDB, 0, 0},
 	{"MFC_BASE+0x198     ", MFC_BASE+0x198, 32, RW, DPDB, 0, 0},
 	{"MFC_BASE+0x19C     ", MFC_BASE+0x19C, 32, RW, DPDB, 0, 0},
 	{"MFC_BASE+0x1A0     ", MFC_BASE+0x1A0, 32, RW, DPDB, 0, 0},
 	{"MFC_BASE+0x1A4     ", MFC_BASE+0x1A4, 32, RW, DPDB, 0, 0},
 	{"MFC_BASE+0x1A8     ", MFC_BASE+0x1A8, 32, RW, DPDB, 0, 0},
 	{"MFC_BASE+0x1AC     ", MFC_BASE+0x1AC, 32, RW, DPDB, 0, 0},
 	{"MFC_BASE+0x1B0     ", MFC_BASE+0x1B0, 32, RW, DPDB, 0, 0},
 	{"MFC_BASE+0x1B4     ", MFC_BASE+0x1B4, 32, RW, DPDB, 0, 0},
 	{"MFC_BASE+0x1B8     ", MFC_BASE+0x1B8, 32, RW, DPDB, 0, 0},
 	{"MFC_BASE+0x1BC     ", MFC_BASE+0x1BC, 32, RW, DPDB, 0, 0},
 	{"MFC_BASE+0x1C0     ", MFC_BASE+0x1C0, 32, RW, DPDB, 0, 0},
 	{"MFC_BASE+0x1C4     ", MFC_BASE+0x1C4, 32, RW, DPDB, 0, 0},
 	{"MFC_BASE+0x1C8     ", MFC_BASE+0x1C8, 32, RW, DPDB, 0, 0},
 	{"MFC_BASE+0x1CC     ", MFC_BASE+0x1CC, 32, RW, DPDB, 0, 0},
 	{"MFC_BASE+0x1D0     ", MFC_BASE+0x1D0, 32, RW, DPDB, 0, 0},
 	{"MFC_BASE+0x1D4     ", MFC_BASE+0x1D4, 32, RW, DPDB, 0, 0},
 	{"MFC_BASE+0x1D8     ", MFC_BASE+0x1D8, 32, RW, DPDB, 0, 0},
#endif	//MFC

#if 0
	//HSI
 	{"rTX_STATUS_REG     ", HSITX_BASE+0x00, 32, RO, DPDB, 0, 0},
 	{"rTX_CONFIG_REG     ", HSITX_BASE+0x04, 32, RW, DPDB, 0, 0},
 	{"rTX_INTSRC_REG     ", HSITX_BASE+0x0C, 5 , RO, DPDB, 0, 0},
 	{"rTX_INTMSK_REG     ", HSITX_BASE+0x10, 5 , RW, DPDB, 0, 0},
 	{"rTX_SWRST_REG      ", HSITX_BASE+0x14, 1 , RW, DPDB, 0, 0},
 	{"rTX_CHID_REG       ", HSITX_BASE+0x18, 32, WO, DPDB, 0, 0},
 	{"rTX_DATA_REG       ", HSITX_BASE+0x1C, 32, WO, DPDB, 0, 0},
 	//
 	{"rRX_STATUS_REG     ", HSIRX_BASE+0x00, 32, RO, DPDB, 0, 0},
 	{"rRX_CONFIG0_REG    ", HSIRX_BASE+0x04, 30, RW, DPDB, 0, 0},
 	{"rRX_CONFIG1_REG    ", HSIRX_BASE+0x08, 32, RW, DPDB, 0, 0},
 	{"rRX_INTSRC_REG     ", HSIRX_BASE+0x0C, 8 , RO, DPDB, 0, 0},
 	{"rRX_INTMSK_REG     ", HSIRX_BASE+0x10, 8 , RW, DPDB, 0, 0},
 	{"rRX_SWRST_REG      ", HSIRX_BASE+0x14, 1 , RW, DPDB, 0, 0},
 	{"rRX_CHID_REG       ", HSIRX_BASE+0x18, 3 , RO, DPDB, 0, 0},
 	{"rRX_DATA_REG       ", HSIRX_BASE+0x1C, 32, RO, DPDB, 0, 0},
#endif	//HSI
	 	
#if 0
	//OTG
 	{"rOPHYPWR           ", USBOTG_PHY_BASE+0x00, 4 , RW, DPDB, 0, 0},
 	{"rOPHYCLK           ", USBOTG_PHY_BASE+0x04, 7 , RW, DPDB, 0, 0},
 	{"rORSTCON           ", USBOTG_PHY_BASE+0x08, 3 , RW, DPDB, 0, 0},
 	//
 	{"rGOTGCTL           ", USBOTG_LINK_BASE+0x00, 20, RW, DPDB, 0, 0},
 	{"rGOTGINT           ", USBOTG_LINK_BASE+0x04, 20, RW, DPDB, 0, 0},
 	{"rGAHBCFG           ", USBOTG_LINK_BASE+0x08, 9 , RW, DPDB, 0, 0},
 	{"rGRSTCTL           ", USBOTG_LINK_BASE+0x10, 32, RW, DPDB, 0, 0},
 	{"rGINTSTS           ", USBOTG_LINK_BASE+0x14, 32, RW, DPDB, 0, 0},
 	{"rGINTMSK           ", USBOTG_LINK_BASE+0x18, 32, RW, DPDB, 0, 0},
 	{"rGRXSTSR           ", USBOTG_LINK_BASE+0x1C, 21, RO, DPDB, 0, 0},
 	{"rGRXSTSTP          ", USBOTG_LINK_BASE+0x20, 21, RO, DPDB, 0, 0},
 	{"rGRXFSIZ           ", USBOTG_LINK_BASE+0x24, 16, RW, DPDB, 0, 0},
 	{"rGNPTXFSIZ         ", USBOTG_LINK_BASE+0x28, 32, RW, DPDB, 0, 0},
 	{"rGNPTXSTS          ", USBOTG_LINK_BASE+0x2C, 31, RO, DPDB, 0, 0},
 	//
 	{"rHPTXFSIZ          ", USBOTG_LINK_BASE+0x100, 32, RW, DPDB, 0, 0},
 	{"rDPTXFSIZ1         ", USBOTG_LINK_BASE+0x104, 32, RW, DPDB, 0, 0},
 	{"rDPTXFSIZ2         ", USBOTG_LINK_BASE+0x108, 32, RW, DPDB, 0, 0},
 	{"rDPTXFSIZ3         ", USBOTG_LINK_BASE+0x10C, 32, RW, DPDB, 0, 0},
 	{"rDPTXFSIZ4         ", USBOTG_LINK_BASE+0x110, 32, RW, DPDB, 0, 0},
 	{"rDPTXFSIZ5         ", USBOTG_LINK_BASE+0x114, 32, RW, DPDB, 0, 0},
 	{"rDPTXFSIZ6         ", USBOTG_LINK_BASE+0x118, 32, RW, DPDB, 0, 0},
 	{"rDPTXFSIZ7         ", USBOTG_LINK_BASE+0x11C, 32, RW, DPDB, 0, 0},
 	{"rDPTXFSIZ8         ", USBOTG_LINK_BASE+0x120, 32, RW, DPDB, 0, 0},
 	{"rDPTXFSIZ9         ", USBOTG_LINK_BASE+0x124, 32, RW, DPDB, 0, 0},
 	{"rDPTXFSIZ10        ", USBOTG_LINK_BASE+0x128, 32, RW, DPDB, 0, 0},
 	{"rDPTXFSIZ11        ", USBOTG_LINK_BASE+0x12C, 32, RW, DPDB, 0, 0},
 	{"rDPTXFSIZ12        ", USBOTG_LINK_BASE+0x130, 32, RW, DPDB, 0, 0},
 	{"rDPTXFSIZ13        ", USBOTG_LINK_BASE+0x134, 32, RW, DPDB, 0, 0},
 	{"rDPTXFSIZ14        ", USBOTG_LINK_BASE+0x138, 32, RW, DPDB, 0, 0},
 	{"rDPTXFSIZ15        ", USBOTG_LINK_BASE+0x13C, 32, RW, DPDB, 0, 0},
 	//
 	{"rHCFG              ", USBOTG_LINK_BASE+0x400, 3 , RW, DPDB, 0, 0},
 	{"rHFNUM             ", USBOTG_LINK_BASE+0x408, 32, RO, DPDB, 0, 0},
 	{"rHPTXSTS           ", USBOTG_LINK_BASE+0x410, 32, RO, DPDB, 0, 0},
 	{"rHAINT             ", USBOTG_LINK_BASE+0x414, 16, RO, DPDB, 0, 0},
 	{"rHAINTMSK          ", USBOTG_LINK_BASE+0x418, 16, RW, DPDB, 0, 0},
 	{"rHPRT              ", USBOTG_LINK_BASE+0x440, 19, RW, DPDB, 0, 0},
 	{"rHCCHAR0           ", USBOTG_LINK_BASE+0x500, 32, RW, DPDB, 0, 0},
 	{"rHCCHAR1           ", USBOTG_LINK_BASE+0x520, 32, RW, DPDB, 0, 0},
 	{"rHCCHAR2           ", USBOTG_LINK_BASE+0x540, 32, RW, DPDB, 0, 0},
 	{"rHCCHAR3           ", USBOTG_LINK_BASE+0x560, 32, RW, DPDB, 0, 0},
 	{"rHCCHAR4           ", USBOTG_LINK_BASE+0x580, 32, RW, DPDB, 0, 0},
 	{"rHCCHAR5           ", USBOTG_LINK_BASE+0x5A0, 32, RW, DPDB, 0, 0},
 	{"rHCCHAR6           ", USBOTG_LINK_BASE+0x5C0, 32, RW, DPDB, 0, 0},
 	{"rHCCHAR7           ", USBOTG_LINK_BASE+0x5E0, 32, RW, DPDB, 0, 0},
 	{"rHCCHAR8           ", USBOTG_LINK_BASE+0x600, 32, RW, DPDB, 0, 0},
 	{"rHCCHAR9           ", USBOTG_LINK_BASE+0x620, 32, RW, DPDB, 0, 0},
 	{"rHCCHAR10          ", USBOTG_LINK_BASE+0x640, 32, RW, DPDB, 0, 0},
 	{"rHCCHAR11          ", USBOTG_LINK_BASE+0x660, 32, RW, DPDB, 0, 0},
 	{"rHCCHAR12          ", USBOTG_LINK_BASE+0x680, 32, RW, DPDB, 0, 0},
 	{"rHCCHAR13          ", USBOTG_LINK_BASE+0x6A0, 32, RW, DPDB, 0, 0},
 	{"rHCCHAR14          ", USBOTG_LINK_BASE+0x6C0, 32, RW, DPDB, 0, 0},
 	{"rHCCHAR15          ", USBOTG_LINK_BASE+0x6E0, 32, RW, DPDB, 0, 0},
 	{"rHCCPLT0           ", USBOTG_LINK_BASE+0x504, 32, RW, DPDB, 0, 0},
 	{"rHCCPLT1           ", USBOTG_LINK_BASE+0x524, 32, RW, DPDB, 0, 0},
 	{"rHCCPLT2           ", USBOTG_LINK_BASE+0x544, 32, RW, DPDB, 0, 0},
 	{"rHCCPLT3           ", USBOTG_LINK_BASE+0x564, 32, RW, DPDB, 0, 0},
 	{"rHCCPLT4           ", USBOTG_LINK_BASE+0x584, 32, RW, DPDB, 0, 0},
 	{"rHCCPLT5           ", USBOTG_LINK_BASE+0x5A4, 32, RW, DPDB, 0, 0},
 	{"rHCCPLT6           ", USBOTG_LINK_BASE+0x5C4, 32, RW, DPDB, 0, 0},
 	{"rHCCPLT7           ", USBOTG_LINK_BASE+0x5E4, 32, RW, DPDB, 0, 0},
 	{"rHCCPLT8           ", USBOTG_LINK_BASE+0x604, 32, RW, DPDB, 0, 0},
 	{"rHCCPLT9           ", USBOTG_LINK_BASE+0x624, 32, RW, DPDB, 0, 0},
 	{"rHCCPLT10          ", USBOTG_LINK_BASE+0x644, 32, RW, DPDB, 0, 0},
 	{"rHCCPLT11          ", USBOTG_LINK_BASE+0x664, 32, RW, DPDB, 0, 0},
 	{"rHCCPLT12          ", USBOTG_LINK_BASE+0x684, 32, RW, DPDB, 0, 0},
 	{"rHCCPLT13          ", USBOTG_LINK_BASE+0x6A4, 32, RW, DPDB, 0, 0},
 	{"rHCCPLT14          ", USBOTG_LINK_BASE+0x6C4, 32, RW, DPDB, 0, 0},
 	{"rHCCPLT15          ", USBOTG_LINK_BASE+0x6E4, 32, RW, DPDB, 0, 0},
 	{"rHCINT0            ", USBOTG_LINK_BASE+0x508, 32, RW, DPDB, 0, 0},
 	{"rHCINT1            ", USBOTG_LINK_BASE+0x528, 32, RW, DPDB, 0, 0},
 	{"rHCINT2            ", USBOTG_LINK_BASE+0x548, 32, RW, DPDB, 0, 0},
 	{"rHCINT3            ", USBOTG_LINK_BASE+0x568, 32, RW, DPDB, 0, 0},
 	{"rHCINT4            ", USBOTG_LINK_BASE+0x588, 32, RW, DPDB, 0, 0},
 	{"rHCINT5            ", USBOTG_LINK_BASE+0x5A8, 32, RW, DPDB, 0, 0},
 	{"rHCINT6            ", USBOTG_LINK_BASE+0x5C8, 32, RW, DPDB, 0, 0},
 	{"rHCINT7            ", USBOTG_LINK_BASE+0x5E8, 32, RW, DPDB, 0, 0},
 	{"rHCINT8            ", USBOTG_LINK_BASE+0x608, 32, RW, DPDB, 0, 0},
 	{"rHCINT9            ", USBOTG_LINK_BASE+0x628, 32, RW, DPDB, 0, 0},
 	{"rHCINT10           ", USBOTG_LINK_BASE+0x648, 32, RW, DPDB, 0, 0},
 	{"rHCINT11           ", USBOTG_LINK_BASE+0x668, 32, RW, DPDB, 0, 0},
 	{"rHCINT12           ", USBOTG_LINK_BASE+0x688, 32, RW, DPDB, 0, 0},
 	{"rHCINT13           ", USBOTG_LINK_BASE+0x6A8, 32, RW, DPDB, 0, 0},
 	{"rHCINT14           ", USBOTG_LINK_BASE+0x6C8, 32, RW, DPDB, 0, 0},
 	{"rHCINT15           ", USBOTG_LINK_BASE+0x6E8, 32, RW, DPDB, 0, 0},
 	{"rHCINTMSK0         ", USBOTG_LINK_BASE+0x50C, 32, RW, DPDB, 0, 0},
 	{"rHCINTMSK1         ", USBOTG_LINK_BASE+0x52C, 32, RW, DPDB, 0, 0},
 	{"rHCINTMSK2         ", USBOTG_LINK_BASE+0x54C, 32, RW, DPDB, 0, 0},
 	{"rHCINTMSK3         ", USBOTG_LINK_BASE+0x56C, 32, RW, DPDB, 0, 0},
 	{"rHCINTMSK4         ", USBOTG_LINK_BASE+0x58C, 32, RW, DPDB, 0, 0},
 	{"rHCINTMSK5         ", USBOTG_LINK_BASE+0x5AC, 32, RW, DPDB, 0, 0},
 	{"rHCINTMSK6         ", USBOTG_LINK_BASE+0x5CC, 32, RW, DPDB, 0, 0},
 	{"rHCINTMSK7         ", USBOTG_LINK_BASE+0x5EC, 32, RW, DPDB, 0, 0},
 	{"rHCINTMSK8         ", USBOTG_LINK_BASE+0x60C, 32, RW, DPDB, 0, 0},
 	{"rHCINTMSK9         ", USBOTG_LINK_BASE+0x62C, 32, RW, DPDB, 0, 0},
 	{"rHCINTMSK10        ", USBOTG_LINK_BASE+0x64C, 32, RW, DPDB, 0, 0},
 	{"rHCINTMSK11        ", USBOTG_LINK_BASE+0x66C, 32, RW, DPDB, 0, 0},
 	{"rHCINTMSK12        ", USBOTG_LINK_BASE+0x68C, 32, RW, DPDB, 0, 0},
 	{"rHCINTMSK13        ", USBOTG_LINK_BASE+0x6AC, 32, RW, DPDB, 0, 0},
 	{"rHCINTMSK14        ", USBOTG_LINK_BASE+0x6CC, 32, RW, DPDB, 0, 0},
 	{"rHCINTMSK15        ", USBOTG_LINK_BASE+0x6EC, 32, RW, DPDB, 0, 0},
 	{"rHCTSIZ0           ", USBOTG_LINK_BASE+0x510, 32, RW, DPDB, 0, 0},
 	{"rHCTSIZ1           ", USBOTG_LINK_BASE+0x530, 32, RW, DPDB, 0, 0},
 	{"rHCTSIZ2           ", USBOTG_LINK_BASE+0x550, 32, RW, DPDB, 0, 0},
 	{"rHCTSIZ3           ", USBOTG_LINK_BASE+0x570, 32, RW, DPDB, 0, 0},
 	{"rHCTSIZ4           ", USBOTG_LINK_BASE+0x590, 32, RW, DPDB, 0, 0},
 	{"rHCTSIZ5           ", USBOTG_LINK_BASE+0x5B0, 32, RW, DPDB, 0, 0},
 	{"rHCTSIZ6           ", USBOTG_LINK_BASE+0x5D0, 32, RW, DPDB, 0, 0},
 	{"rHCTSIZ7           ", USBOTG_LINK_BASE+0x5F0, 32, RW, DPDB, 0, 0},
 	{"rHCTSIZ8           ", USBOTG_LINK_BASE+0x610, 32, RW, DPDB, 0, 0},
 	{"rHCTSIZ9           ", USBOTG_LINK_BASE+0x630, 32, RW, DPDB, 0, 0},
 	{"rHCTSIZ10          ", USBOTG_LINK_BASE+0x650, 32, RW, DPDB, 0, 0},
 	{"rHCTSIZ11          ", USBOTG_LINK_BASE+0x670, 32, RW, DPDB, 0, 0},
 	{"rHCTSIZ12          ", USBOTG_LINK_BASE+0x690, 32, RW, DPDB, 0, 0},
 	{"rHCTSIZ13          ", USBOTG_LINK_BASE+0x6B0, 32, RW, DPDB, 0, 0},
 	{"rHCTSIZ14          ", USBOTG_LINK_BASE+0x6D0, 32, RW, DPDB, 0, 0},
 	{"rHCTSIZ15          ", USBOTG_LINK_BASE+0x6F0, 32, RW, DPDB, 0, 0},
 	{"rHCDMA0            ", USBOTG_LINK_BASE+0x514, 32, RW, DPDB, 0, 0},
 	{"rHCDMA1            ", USBOTG_LINK_BASE+0x534, 32, RW, DPDB, 0, 0},
 	{"rHCDMA2            ", USBOTG_LINK_BASE+0x554, 32, RW, DPDB, 0, 0},
 	{"rHCDMA3            ", USBOTG_LINK_BASE+0x574, 32, RW, DPDB, 0, 0},
 	{"rHCDMA4            ", USBOTG_LINK_BASE+0x594, 32, RW, DPDB, 0, 0},
 	{"rHCDMA5            ", USBOTG_LINK_BASE+0x5B4, 32, RW, DPDB, 0, 0},
 	{"rHCDMA6            ", USBOTG_LINK_BASE+0x5D4, 32, RW, DPDB, 0, 0},
 	{"rHCDMA7            ", USBOTG_LINK_BASE+0x5F4, 32, RW, DPDB, 0, 0},
 	{"rHCDMA8            ", USBOTG_LINK_BASE+0x614, 32, RW, DPDB, 0, 0},
 	{"rHCDMA9            ", USBOTG_LINK_BASE+0x634, 32, RW, DPDB, 0, 0},
 	{"rHCDMA10           ", USBOTG_LINK_BASE+0x654, 32, RW, DPDB, 0, 0},
 	{"rHCDMA11           ", USBOTG_LINK_BASE+0x674, 32, RW, DPDB, 0, 0},
 	{"rHCDMA12           ", USBOTG_LINK_BASE+0x694, 32, RW, DPDB, 0, 0},
 	{"rHCDMA13           ", USBOTG_LINK_BASE+0x6B4, 32, RW, DPDB, 0, 0},
 	{"rHCDMA14           ", USBOTG_LINK_BASE+0x6D4, 32, RW, DPDB, 0, 0},
 	{"rHCDMA15           ", USBOTG_LINK_BASE+0x6F4, 32, RW, DPDB, 0, 0},
 	//
 	{"rDCFG              ", USBOTG_LINK_BASE+0x800, 23, RW, DPDB, 0, 0},
 	{"rDCTL              ", USBOTG_LINK_BASE+0x804, 12, RW, DPDB, 0, 0},
 	{"rDSTS              ", USBOTG_LINK_BASE+0x808, 22, RO, DPDB, 0, 0},
 	{"rDIEPMSK           ", USBOTG_LINK_BASE+0x810, 7 , RW, DPDB, 0, 0},
 	{"rDOEPMSK           ", USBOTG_LINK_BASE+0x814, 7 , RW, DPDB, 0, 0},
 	{"rDAINT             ", USBOTG_LINK_BASE+0x818, 32, RO, DPDB, 0, 0},
 	{"rDAINTMSK          ", USBOTG_LINK_BASE+0x81C, 32, RW, DPDB, 0, 0},
 	{"rDTKNQR1           ", USBOTG_LINK_BASE+0x820, 32, RO, DPDB, 0, 0},
 	{"rDTKNQR2           ", USBOTG_LINK_BASE+0x824, 32, RO, DPDB, 0, 0},
 	{"rDVBUSDIS          ", USBOTG_LINK_BASE+0x828, 16, RW, DPDB, 0, 0},
 	{"rDVBUSPULSE        ", USBOTG_LINK_BASE+0x82C, 12, RW, DPDB, 0, 0},
 	{"rDTKNQR3           ", USBOTG_LINK_BASE+0x830, 32, RO, DPDB, 0, 0},
 	{"rDTKNQR4           ", USBOTG_LINK_BASE+0x834, 32, RO, DPDB, 0, 0},
 	{"rDIEPCTL0          ", USBOTG_LINK_BASE+0x900, 32, RW, DPDB, 0, 0},
 	{"rDIEPINT0          ", USBOTG_LINK_BASE+0x908, 6 , RW, DPDB, 0, 0},
 	{"rDIEPTSIZ0         ", USBOTG_LINK_BASE+0x910, 20, RW, DPDB, 0, 0},
 	{"rDIEPDMA0          ", USBOTG_LINK_BASE+0x914, 32, RW, DPDB, 0, 0},
 	{"rDIEPCTL1          ", USBOTG_LINK_BASE+0x920, 32, RW, DPDB, 0, 0},
 	{"rDIEPINT1          ", USBOTG_LINK_BASE+0x928, 6 , RW, DPDB, 0, 0},
 	{"rDIEPTSIZ1         ", USBOTG_LINK_BASE+0x930, 31, RW, DPDB, 0, 0},
 	{"rDIEPDMA1          ", USBOTG_LINK_BASE+0x934, 32, RW, DPDB, 0, 0},
 	{"rDIEPCTL2          ", USBOTG_LINK_BASE+0x940, 32, RW, DPDB, 0, 0},
 	{"rDIEPINT2          ", USBOTG_LINK_BASE+0x948, 6 , RW, DPDB, 0, 0},
 	{"rDIEPTSIZ2         ", USBOTG_LINK_BASE+0x950, 31, RW, DPDB, 0, 0},
 	{"rDIEPDMA2          ", USBOTG_LINK_BASE+0x954, 32, RW, DPDB, 0, 0},
 	{"rDIEPCTL3          ", USBOTG_LINK_BASE+0x960, 32, RW, DPDB, 0, 0},
 	{"rDIEPINT3          ", USBOTG_LINK_BASE+0x968, 6 , RW, DPDB, 0, 0},
 	{"rDIEPTSIZ3         ", USBOTG_LINK_BASE+0x970, 31, RW, DPDB, 0, 0},
 	{"rDIEPDMA3          ", USBOTG_LINK_BASE+0x974, 32, RW, DPDB, 0, 0},
 	{"rDIEPCTL4          ", USBOTG_LINK_BASE+0x980, 32, RW, DPDB, 0, 0},
 	{"rDIEPINT4          ", USBOTG_LINK_BASE+0x988, 6 , RW, DPDB, 0, 0},
 	{"rDIEPTSIZ4         ", USBOTG_LINK_BASE+0x990, 31, RW, DPDB, 0, 0},
 	{"rDIEPDMA4          ", USBOTG_LINK_BASE+0x994, 32, RW, DPDB, 0, 0},
 	{"rDIEPCTL5          ", USBOTG_LINK_BASE+0x9A0, 32, RW, DPDB, 0, 0},
 	{"rDIEPINT5          ", USBOTG_LINK_BASE+0x9A8, 6 , RW, DPDB, 0, 0},
 	{"rDIEPTSIZ5         ", USBOTG_LINK_BASE+0x9B0, 31, RW, DPDB, 0, 0},
 	{"rDIEPDMA5          ", USBOTG_LINK_BASE+0x9B4, 32, RW, DPDB, 0, 0},
 	{"rDIEPCTL6          ", USBOTG_LINK_BASE+0x9C0, 32, RW, DPDB, 0, 0},
 	{"rDIEPINT6          ", USBOTG_LINK_BASE+0x9C8, 6 , RW, DPDB, 0, 0},
 	{"rDIEPTSIZ6         ", USBOTG_LINK_BASE+0x9D0, 31, RW, DPDB, 0, 0},
 	{"rDIEPDMA6          ", USBOTG_LINK_BASE+0x9D4, 32, RW, DPDB, 0, 0},
 	{"rDIEPCTL7          ", USBOTG_LINK_BASE+0x9E0, 32, RW, DPDB, 0, 0},
 	{"rDIEPINT7          ", USBOTG_LINK_BASE+0x9E8, 6 , RW, DPDB, 0, 0},
 	{"rDIEPTSIZ7         ", USBOTG_LINK_BASE+0x9F0, 31, RW, DPDB, 0, 0},
 	{"rDIEPDMA7          ", USBOTG_LINK_BASE+0x9F4, 32, RW, DPDB, 0, 0},
 	{"rDIEPCTL8          ", USBOTG_LINK_BASE+0xA00, 32, RW, DPDB, 0, 0},
 	{"rDIEPINT8          ", USBOTG_LINK_BASE+0xA08, 6 , RW, DPDB, 0, 0},
 	{"rDIEPTSIZ8         ", USBOTG_LINK_BASE+0xA10, 31, RW, DPDB, 0, 0},
 	{"rDIEPDMA8          ", USBOTG_LINK_BASE+0xA14, 32, RW, DPDB, 0, 0},
 	{"rDIEPCTL9          ", USBOTG_LINK_BASE+0xA20, 32, RW, DPDB, 0, 0},
 	{"rDIEPINT9          ", USBOTG_LINK_BASE+0xA28, 6 , RW, DPDB, 0, 0},
 	{"rDIEPTSIZ9         ", USBOTG_LINK_BASE+0xA30, 31, RW, DPDB, 0, 0},
 	{"rDIEPDMA9          ", USBOTG_LINK_BASE+0xA34, 32, RW, DPDB, 0, 0},
 	{"rDIEPCTL10         ", USBOTG_LINK_BASE+0xA40, 32, RW, DPDB, 0, 0},
 	{"rDIEPINT10         ", USBOTG_LINK_BASE+0xA48, 6 , RW, DPDB, 0, 0},
 	{"rDIEPTSIZ10        ", USBOTG_LINK_BASE+0xA50, 31, RW, DPDB, 0, 0},
 	{"rDIEPDMA10         ", USBOTG_LINK_BASE+0xA54, 32, RW, DPDB, 0, 0},
 	{"rDIEPCTL11         ", USBOTG_LINK_BASE+0xA60, 32, RW, DPDB, 0, 0},
 	{"rDIEPINT11         ", USBOTG_LINK_BASE+0xA68, 6 , RW, DPDB, 0, 0},
 	{"rDIEPTSIZ11        ", USBOTG_LINK_BASE+0xA70, 31, RW, DPDB, 0, 0},
 	{"rDIEPDMA11         ", USBOTG_LINK_BASE+0xA74, 32, RW, DPDB, 0, 0},
 	{"rDIEPCTL12         ", USBOTG_LINK_BASE+0xA80, 32, RW, DPDB, 0, 0},
 	{"rDIEPINT12         ", USBOTG_LINK_BASE+0xA88, 6 , RW, DPDB, 0, 0},
 	{"rDIEPTSIZ12        ", USBOTG_LINK_BASE+0xA90, 31, RW, DPDB, 0, 0},
 	{"rDIEPDMA12         ", USBOTG_LINK_BASE+0xA94, 32, RW, DPDB, 0, 0},
 	{"rDIEPCTL13         ", USBOTG_LINK_BASE+0xAA0, 32, RW, DPDB, 0, 0},
 	{"rDIEPINT13         ", USBOTG_LINK_BASE+0xAA8, 6 , RW, DPDB, 0, 0},
 	{"rDIEPTSIZ13        ", USBOTG_LINK_BASE+0xAB0, 31, RW, DPDB, 0, 0},
 	{"rDIEPDMA13         ", USBOTG_LINK_BASE+0xAB4, 32, RW, DPDB, 0, 0},
 	{"rDIEPCTL14         ", USBOTG_LINK_BASE+0xAC0, 32, RW, DPDB, 0, 0},
 	{"rDIEPINT14         ", USBOTG_LINK_BASE+0xAC8, 6 , RW, DPDB, 0, 0},
 	{"rDIEPTSIZ14        ", USBOTG_LINK_BASE+0xAD0, 31, RW, DPDB, 0, 0},
 	{"rDIEPDMA14         ", USBOTG_LINK_BASE+0xAD4, 32, RW, DPDB, 0, 0},
 	{"rDIEPCTL15         ", USBOTG_LINK_BASE+0xAE0, 32, RW, DPDB, 0, 0},
 	{"rDIEPINT15         ", USBOTG_LINK_BASE+0xAE8, 6 , RW, DPDB, 0, 0},
 	{"rDIEPTSIZ15        ", USBOTG_LINK_BASE+0xAF0, 31, RW, DPDB, 0, 0},
 	{"rDIEPDMA15         ", USBOTG_LINK_BASE+0xAF4, 32, RW, DPDB, 0, 0},
 	{"rDOEPCTL0          ", USBOTG_LINK_BASE+0xB00, 32, RW, DPDB, 0, 0},
 	{"rDOEPINT0          ", USBOTG_LINK_BASE+0xB08, 6 , RW, DPDB, 0, 0},
 	{"rDOEPTSIZ0         ", USBOTG_LINK_BASE+0xB10, 31, RW, DPDB, 0, 0},
 	{"rDOEPDMA0          ", USBOTG_LINK_BASE+0xB14, 32, RW, DPDB, 0, 0},
 	{"rDOEPCTL1          ", USBOTG_LINK_BASE+0xB20, 32, RW, DPDB, 0, 0},
 	{"rDOEPINT1          ", USBOTG_LINK_BASE+0xB28, 6 , RW, DPDB, 0, 0},
 	{"rDOEPTSIZ1         ", USBOTG_LINK_BASE+0xB30, 31, RW, DPDB, 0, 0},
 	{"rDOEPDMA1          ", USBOTG_LINK_BASE+0xB34, 32, RW, DPDB, 0, 0},
 	{"rDOEPCTL2          ", USBOTG_LINK_BASE+0xB40, 32, RW, DPDB, 0, 0},
 	{"rDOEPINT2          ", USBOTG_LINK_BASE+0xB48, 6 , RW, DPDB, 0, 0},
 	{"rDOEPTSIZ2         ", USBOTG_LINK_BASE+0xB50, 31, RW, DPDB, 0, 0},
 	{"rDOEPDMA2          ", USBOTG_LINK_BASE+0xB54, 32, RW, DPDB, 0, 0},
 	{"rDOEPCTL3          ", USBOTG_LINK_BASE+0xB60, 32, RW, DPDB, 0, 0},
 	{"rDOEPINT3          ", USBOTG_LINK_BASE+0xB68, 6 , RW, DPDB, 0, 0},
 	{"rDOEPTSIZ3         ", USBOTG_LINK_BASE+0xB70, 31, RW, DPDB, 0, 0},
 	{"rDOEPDMA3          ", USBOTG_LINK_BASE+0xB74, 32, RW, DPDB, 0, 0},
 	{"rDOEPCTL4          ", USBOTG_LINK_BASE+0xB80, 32, RW, DPDB, 0, 0},
 	{"rDOEPINT4          ", USBOTG_LINK_BASE+0xB88, 6 , RW, DPDB, 0, 0},
 	{"rDOEPTSIZ4         ", USBOTG_LINK_BASE+0xB90, 31, RW, DPDB, 0, 0},
 	{"rDOEPDMA4          ", USBOTG_LINK_BASE+0xB94, 32, RW, DPDB, 0, 0},
 	{"rDOEPCTL5          ", USBOTG_LINK_BASE+0xBA0, 32, RW, DPDB, 0, 0},
 	{"rDOEPINT5          ", USBOTG_LINK_BASE+0xBA8, 6 , RW, DPDB, 0, 0},
 	{"rDOEPTSIZ5         ", USBOTG_LINK_BASE+0xBB0, 31, RW, DPDB, 0, 0},
 	{"rDOEPDMA5          ", USBOTG_LINK_BASE+0xBB4, 32, RW, DPDB, 0, 0},
 	{"rDOEPCTL6          ", USBOTG_LINK_BASE+0xBC0, 32, RW, DPDB, 0, 0},
 	{"rDOEPINT6          ", USBOTG_LINK_BASE+0xBC8, 6 , RW, DPDB, 0, 0},
 	{"rDOEPTSIZ6         ", USBOTG_LINK_BASE+0xBD0, 31, RW, DPDB, 0, 0},
 	{"rDOEPDMA6          ", USBOTG_LINK_BASE+0xBD4, 32, RW, DPDB, 0, 0},
 	{"rDOEPCTL7          ", USBOTG_LINK_BASE+0xBE0, 32, RW, DPDB, 0, 0},
 	{"rDOEPINT7          ", USBOTG_LINK_BASE+0xBE8, 6 , RW, DPDB, 0, 0},
 	{"rDOEPTSIZ7         ", USBOTG_LINK_BASE+0xBF0, 31, RW, DPDB, 0, 0},
 	{"rDOEPDMA7          ", USBOTG_LINK_BASE+0xBF4, 32, RW, DPDB, 0, 0},
 	{"rDOEPCTL8          ", USBOTG_LINK_BASE+0xC00, 32, RW, DPDB, 0, 0},
 	{"rDOEPINT8          ", USBOTG_LINK_BASE+0xC08, 6 , RW, DPDB, 0, 0},
 	{"rDOEPTSIZ8         ", USBOTG_LINK_BASE+0xC10, 31, RW, DPDB, 0, 0},
 	{"rDOEPDMA8          ", USBOTG_LINK_BASE+0xC14, 32, RW, DPDB, 0, 0},
 	{"rDOEPCTL9          ", USBOTG_LINK_BASE+0xC20, 32, RW, DPDB, 0, 0},
 	{"rDOEPINT9          ", USBOTG_LINK_BASE+0xC28, 6 , RW, DPDB, 0, 0},
 	{"rDOEPTSIZ9         ", USBOTG_LINK_BASE+0xC30, 31, RW, DPDB, 0, 0},
 	{"rDOEPDMA9          ", USBOTG_LINK_BASE+0xC34, 32, RW, DPDB, 0, 0},
 	{"rDOEPCTL10         ", USBOTG_LINK_BASE+0xC40, 32, RW, DPDB, 0, 0},
 	{"rDOEPINT10         ", USBOTG_LINK_BASE+0xC48, 6 , RW, DPDB, 0, 0},
 	{"rDOEPTSIZ10        ", USBOTG_LINK_BASE+0xC50, 31, RW, DPDB, 0, 0},
 	{"rDOEPDMA10         ", USBOTG_LINK_BASE+0xC54, 32, RW, DPDB, 0, 0},
 	{"rDOEPCTL11         ", USBOTG_LINK_BASE+0xC60, 32, RW, DPDB, 0, 0},
 	{"rDOEPINT11         ", USBOTG_LINK_BASE+0xC68, 6 , RW, DPDB, 0, 0},
 	{"rDOEPTSIZ11        ", USBOTG_LINK_BASE+0xC70, 31, RW, DPDB, 0, 0},
 	{"rDOEPDMA11         ", USBOTG_LINK_BASE+0xC74, 32, RW, DPDB, 0, 0},
 	{"rDOEPCTL12         ", USBOTG_LINK_BASE+0xC80, 32, RW, DPDB, 0, 0},
 	{"rDOEPINT12         ", USBOTG_LINK_BASE+0xC88, 6 , RW, DPDB, 0, 0},
 	{"rDOEPTSIZ12        ", USBOTG_LINK_BASE+0xC90, 31, RW, DPDB, 0, 0},
 	{"rDOEPDMA12         ", USBOTG_LINK_BASE+0xC94, 32, RW, DPDB, 0, 0},
 	{"rDOEPCTL13         ", USBOTG_LINK_BASE+0xCA0, 32, RW, DPDB, 0, 0},
 	{"rDOEPINT13         ", USBOTG_LINK_BASE+0xCA8, 6 , RW, DPDB, 0, 0},
 	{"rDOEPTSIZ13        ", USBOTG_LINK_BASE+0xCB0, 31, RW, DPDB, 0, 0},
 	{"rDOEPDMA13         ", USBOTG_LINK_BASE+0xCB4, 32, RW, DPDB, 0, 0},
 	{"rDOEPCTL14         ", USBOTG_LINK_BASE+0xCC0, 32, RW, DPDB, 0, 0},
 	{"rDOEPINT14         ", USBOTG_LINK_BASE+0xCC8, 6 , RW, DPDB, 0, 0},
 	{"rDOEPTSIZ14        ", USBOTG_LINK_BASE+0xCD0, 31, RW, DPDB, 0, 0},
 	{"rDOEPDMA14         ", USBOTG_LINK_BASE+0xCD4, 32, RW, DPDB, 0, 0},
 	{"rDOEPCTL15         ", USBOTG_LINK_BASE+0xCE0, 32, RW, DPDB, 0, 0},
 	{"rDOEPINT15         ", USBOTG_LINK_BASE+0xCE8, 6 , RW, DPDB, 0, 0},
 	{"rDOEPTSIZ15        ", USBOTG_LINK_BASE+0xCF0, 31, RW, DPDB, 0, 0},
 	{"rDOEPDMA15         ", USBOTG_LINK_BASE+0xCF4, 32, RW, DPDB, 0, 0},
#endif	//OTG
	 	
#if 0
	// SROMC
	{"rSROM_BW					", SROM_BASE+0x00, 24, RW, DPDB, 0, 0x0},   	

	{"rSROM_BC0					", SROM_BASE+0x04, 32, RW, DPDB, 0, 0x0},   	
	{"rSROM_BC1					", SROM_BASE+0x08, 32, RW, DPDB, 0, 0x0},   	
	{"rSROM_BC2					", SROM_BASE+0x0C, 32, RW, DPDB, 0, 0x0},   	
	{"rSROM_BC3					", SROM_BASE+0x10, 32, RW, DPDB, 0, 0x0},   	
	{"rSROM_BC4					", SROM_BASE+0x14, 32, RW, DPDB, 0, 0x0},   	
	{"rSROM_BC5					", SROM_BASE+0x18, 32, RW, DPDB, 0, 0x0},   	
#endif

#if 1
	// CFCON
	{"rMUX_REG					", CFCON_BASE+0x1800, 3, RW, DPDB, 0, 0x6},   	
	{"rPCCARD_CNFG&STATUS		", CFCON_BASE+0x1820, 14, RW, DPDB, 0xFF, 0xF07},   
	{"rPCCARD_INTMSK&SRC		", CFCON_BASE+0x1824, 11, RW, DPDB, 0xF8, 0x600},   
	{"rPCCARD_ATTR				", CFCON_BASE+0x1828, 23, RW, DPDB, 0, 0x31909},   
	{"rPCCARD_I/O				", CFCON_BASE+0x182C, 23, RW, DPDB, 0, 0x31909},   
	{"rPCCARD_COMM				", CFCON_BASE+0x1830, 23, RW, DPDB, 0, 0x31909},   
	{"rATA_CONTROL				", CFCON_BASE+0x1900,  1, RW, DPDB, 0, 0x0},   
	{"rATA_STATUS				", CFCON_BASE+0x1904, 6, RO, DPDB, 0, 0},   
	{"rATA_COMMAND				", CFCON_BASE+0x1908, 2, RW, DPDB, 0, 0x0},   
	{"rATA_SWRST				", CFCON_BASE+0x190C, 1, RW, DPDB, 0, 0x0},   
	{"rATA_IRQ					", CFCON_BASE+0x1910, 5, RW, DPDB, 0, 0x0},   
	{"rATA_IRQ_MASK				", CFCON_BASE+0x1914, 5, RW, DPDB, 0, 0x1F},   
	{"rATA_CFG					", CFCON_BASE+0x1918, 10, RW, DPDB, 0, 0x0},   

	{"rATA_PIO_TIME				", CFCON_BASE+0x192C, 20, RW, DPDB, 0, 0x1C238},   
	{"rATA_UDMA_TIME			", CFCON_BASE+0x1930, 28, RW, DPDB, 0, 0x20B1362},   
	{"rATA_XFR_NUM				", CFCON_BASE+0x1934, 32, RW, DPDB, 0x1, 0x0},   
	{"rATA_XFR_CNT				", CFCON_BASE+0x1938, 32, RW, DPDB, 0x1, 0x0},   
	{"rATA_TBUF_START			", CFCON_BASE+0x193C, 32, RW, DPDB, 0x3, 0x0},   
	{"rATA_TBUF_SIZE				", CFCON_BASE+0x1940, 32, RW, DPDB, 0x1F, 0x0},   
	{"rATA_SBUF_START			", CFCON_BASE+0x1944, 32, RW, DPDB, 0x3, 0x0},   
	{"rATA_SBUF_SIZE			", CFCON_BASE+0x1948, 32, RW, DPDB, 0x1F, 0x0},   
	{"rATA_CADR_TBUF			", CFCON_BASE+0x194C, 32, RW, DPDB, 0x3, 0x0},   
	{"rATA_CADR_SBUF			", CFCON_BASE+0x1950, 32, RW, DPDB, 0x3, 0x0},   
	{"rATA_PIO_DTR				", CFCON_BASE+0x1954, 16, WO, DPDB, 0, 0x0},   
	{"rATA_PIO_FED				", CFCON_BASE+0x1958,   8, WO, DPDB, 0, 0x0},   
	{"rATA_PIO_SCR				", CFCON_BASE+0x195C,   8, WO, DPDB, 0, 0x0},   
	{"rATA_PIO_LLR				", CFCON_BASE+0x1960,   8, WO, DPDB, 0, 0x0},   
	{"rATA_PIO_LMR				", CFCON_BASE+0x1964,   8, WO, DPDB, 0, 0x0},   
	{"rATA_PIO_LHR				", CFCON_BASE+0x1968,   8, WO, DPDB, 0, 0x0},   
	{"rATA_PIO_DVR				", CFCON_BASE+0x196C,   8, WO, DPDB, 0, 0x0},   
	{"rATA_PIO_CSD				", CFCON_BASE+0x1970,   8, WO, DPDB, 0, 0x0},   
	{"rATA_PIO_DAD				", CFCON_BASE+0x1974,   8, WO, DPDB, 0, 0x0},   
	{"rATA_PIO_RDATA			", CFCON_BASE+0x197C, 16, RO, DPDB, 0, 0x0},   
	{"rBUS_FIFO_STATUS			", CFCON_BASE+0x1990, 19, RO, DPDB, 0, 0x0},   
	{"rATA_FIFO_STATUS			", CFCON_BASE+0x1994, 31, RO, DPDB, 0, 0x0},   
#endif

#if 0
	// Nand
	{"rNFCONF			", NFCON_BASE+0x00, 32, RW, DPDB, 0, 0},   	
	{"rNFCONT			", NFCON_BASE+0x04, 19, RW, DPDB, 0, 0},   
	{"rNFCMMD			", NFCON_BASE+0x08, 8 , RW, DPDB, 0, 0},   
	{"rNFADDR			", NFCON_BASE+0x0C, 8 , RW, DPDB, 0, 0},   
	{"rNFDATA			", NFCON_BASE+0x10, 32, RW, DPDB, 0, 0},   
	{"rNFMECCD0			", NFCON_BASE+0x14, 32, RW, DPDB, 0, 0},   
	{"rNFMECCD1			", NFCON_BASE+0x18, 32, RW, DPDB, 0, 0},   
	{"rNFSECCD			", NFCON_BASE+0x1C, 32, RW, DPDB, 0, 0},   
	{"rNFSBLK			", NFCON_BASE+0x20, 24, RW, DPDB, 0, 0},   
	{"rNFEBLK			", NFCON_BASE+0x24, 24, RW, DPDB, 0, 0},   
	{"rNFSTAT			", NFCON_BASE+0x28, 24, RW, DPDB, 0, 0},   
	{"rNFECCERR0		", NFCON_BASE+0x2C, 25, RO, DPDB, 0, 0},   
	{"rNFECCERR1		", NFCON_BASE+0x30, 26, RO, DPDB, 0, 0},   
	{"rNFMECC0			", NFCON_BASE+0x34, 32, RO, DPDB, 0, 0},   
	{"rNFMECC1			", NFCON_BASE+0x38, 24, RO, DPDB, 0, 0},   
	{"rNFSECC			", NFCON_BASE+0x3C, 16, RO, DPDB, 0, 0},   
	{"rNFMLCBITPT		", NFCON_BASE+0x40, 32, RO, DPDB, 0, 0},   
	
	// ADCTS
	{"rADCCON			", ADCTS_BASE+0x00, 16,	RW, DPDB, 0, 0x1FC4},   	
	{"rADCTSC			", ADCTS_BASE+0x04,	9,	RW, DPDB, 0, 0x58},   
	{"rADCDLY			", ADCTS_BASE+0x08, 17,	RW, DPDB, 0, 0xff},   
	{"rADCDAT0			", ADCTS_BASE+0x0C, 16,	RO, DPDB, 0, 0x00},   
	{"rADCDAT1			", ADCTS_BASE+0x10, 16,	RO, DPDB, 0, 0x00},   
	{"rADCUPDN			", ADCTS_BASE+0x14, 2,	RW, DPDB, 0, 0x0},   
	{"rADCCLRINT		", ADCTS_BASE+0x18, 1,	WO, DPDB, 0, 0},   
	{"rADCCLRINTPNDNUP	", ADCTS_BASE+0x1C, 1,	WO, DPDB, 0, 0},   	// Edited by SOP at 2008/03/05   

	// I2S0
	{"rI2S0CON			", I2S0_BASE+0x00, 7, RW, DPDB, 0, 0},   	
	{"rI2S0MOD			", I2S0_BASE+0x04, 13, RW, DPDB, 0, 0},   
	{"rI2S0FIC			", I2S0_BASE+0x08, 16 , RW, DPDB, 0, 0},   
	{"rI2S0PSR			", I2S0_BASE+0x0C, 16 , RW, DPDB, 0, 0},   
	{"rI2S0TXD			", I2S0_BASE+0x10, 32, WO, DPDB, 0, 0},   
	{"rI2S0RXD			", I2S0_BASE+0x14, 32, RO, DPDB, 0, 0},   

	// I2S1
	{"rI2S1CON			", I2S1_BASE+0x00, 7, RW, DPDB, 0, 0},   	
	{"rI2S1MOD			", I2S1_BASE+0x04, 13, RW, DPDB, 0, 0},   
	{"rI2S1FIC			", I2S1_BASE+0x08, 16 , RW, DPDB, 0, 0},   
	{"rI2S1PSR			", I2S1_BASE+0x0C, 16 , RW, DPDB, 0, 0},   
	{"rI2S1TXD			", I2S1_BASE+0x10, 32, WO, DPDB, 0, 0},   
	{"rI2S1RXD			", I2S1_BASE+0x14, 32, RO, DPDB, 0, 0},   
	//------------------ DMAC1 -----------------------------
	{"rDMAC0IntStatus			", DMA0_BASE+0x00, 8, RO, DPDB, 0, 0},   	
	{"rDMAC0IntTCStatus		", DMA0_BASE+0x04, 8, RO, DPDB, 0, 0},   
	{"rDMAC0IntTCClear		", DMA0_BASE+0x08, 8 , WO, DPDB, 0, 0},   
	{"rDMAC0IntErrorStatus		", DMA0_BASE+0x0C, 8 , RO, DPDB, 0, 0},   
	{"rDMAC0IntErrClr			", DMA0_BASE+0x10, 8, WO, DPDB, 0, 0},   
	{"rDMAC0RawIntTCStatus	", DMA0_BASE+0x14, 8, RO, DPDB, 0, 0},   
	{"rDMAC0RawIntErrorStatus	", DMA0_BASE+0x18, 8, RO, DPDB, 0, 0},   
	{"rDMAC0EnbldChns		", DMA0_BASE+0x1C, 8, RO, DPDB, 0, 0},   
	{"rDMAC0SoftBReq			", DMA0_BASE+0x20, 16, RW, DPDB, 0, 0},   
	{"rDMAC0SoftSReq			", DMA0_BASE+0x24, 16, RW, DPDB, 0, 0},   
	{"rDMAC0SoftLBReq			", DMA0_BASE+0x28, 16, RW, DPDB, 0, 0},   
	{"rDMAC0SoftLSReq			", DMA0_BASE+0x2C, 16, RW, DPDB, 0, 0},   
	{"rDMAC0Sync			", DMA0_BASE+0x34, 16, RW, DPDB, 0, 0},   
	
	// Ch0
	{"rDMACC0SrcAddr		", DMA0_BASE+0x100, 32, RW, DPDB, 0, 0},   
	{"rDMACC0DestAddr		", DMA0_BASE+0x104, 32, RW, DPDB, 0, 0},   
	{"rDMACC0LLI			", DMA0_BASE+0x108, 32, RW, DPPB, 0xFFFFFFFD, 0},   
	{"rDMACC0Control0		", DMA0_BASE+0x10C, 32, RW, DPPB, 0xFFFFF000, 0},   
	{"rDMACC0Control1		", DMA0_BASE+0x110, 25, RW, DPPB, 0x0, 0},   // not WO, but this register acts like WO.
	{"rDMACC0Configuration	", DMA0_BASE+0x114, 19, RW, DPDB, 0, 0},   
	// Ch1
	{"rDMACC1SrcAddr		", DMA0_BASE+0x120, 32, RW, DPDB, 0, 0},   
	{"rDMACC1DestAddr		", DMA0_BASE+0x124, 32, RW, DPDB, 0, 0},   
	{"rDMACC1LLI				", DMA0_BASE+0x128, 32, RW, DPPB, 0xFFFFFFFD, 0},   
	{"rDMACC1Control0		", DMA0_BASE+0x12C, 32, RW, DPPB, 0xFFFFF000, 0},   
	{"rDMACC1Control1		", DMA0_BASE+0x130, 25, RW, DPPB, 0x0, 0},   // not WO, but this register acts like WO.
	{"rDMACC1Configuration	", DMA0_BASE+0x134, 19, RW, DPDB, 0, 0},   
	// Ch2
	{"rDMACC2SrcAddr		", DMA0_BASE+0x140, 32, RW, DPDB, 0, 0},   
	{"rDMACC2DestAddr		", DMA0_BASE+0x144, 32, RW, DPDB, 0, 0},   
	{"rDMACC2LLI				", DMA0_BASE+0x148, 32, RW, DPPB, 0xFFFFFFFD, 0},   
	{"rDMACC2Control0		", DMA0_BASE+0x14C, 32, RW, DPPB, 0xFFFFF000, 0},   
	{"rDMACC2Control1		", DMA0_BASE+0x150, 25, RW, DPPB, 0x0, 0},   // not WO, but this register acts like WO.
	{"rDMACC2Configuration	", DMA0_BASE+0x154, 19, RW, DPDB, 0, 0},   
	// Ch3
	{"rDMACC3SrcAddr		", DMA0_BASE+0x160, 32, RW, DPDB, 0, 0},   
	{"rDMACC3DestAddr		", DMA0_BASE+0x164, 32, RW, DPDB, 0, 0},   
	{"rDMACC3LLI				", DMA0_BASE+0x168, 32, RW, DPPB, 0xFFFFFFFD, 0},   
	{"rDMACC3Control0		", DMA0_BASE+0x16C, 32, RW, DPPB, 0xFFFFF000, 0},   
	{"rDMACC3Control1		", DMA0_BASE+0x170, 25, RW, DPPB, 0x0, 0},   // not WO, but this register acts like WO.
	{"rDMACC3Configuration	", DMA0_BASE+0x174, 19, RW, DPDB, 0, 0},   
	// Ch4
	{"rDMACC4SrcAddr		", DMA0_BASE+0x180, 32, RW, DPDB, 0, 0},   
	{"rDMACC4DestAddr		", DMA0_BASE+0x184, 32, RW, DPDB, 0, 0},   
	{"rDMACC4LLI				", DMA0_BASE+0x188, 32, RW, DPPB, 0xFFFFFFFD, 0},   
	{"rDMACC4Control0		", DMA0_BASE+0x18C, 32, RW, DPPB, 0xFFFFF000, 0},   
	{"rDMACC4Control1		", DMA0_BASE+0x190, 25, RW, DPPB, 0x0, 0},   // not WO, but this register acts like WO.
	{"rDMACC4Configuration	", DMA0_BASE+0x194, 19, RW, DPDB, 0, 0},   
	// Ch5
	{"rDMACC5SrcAddr		", DMA0_BASE+0x1A0, 32, RW, DPDB, 0, 0},   
	{"rDMACC5DestAddr		", DMA0_BASE+0x1A4, 32, RW, DPDB, 0, 0},   
	{"rDMACC5LLI				", DMA0_BASE+0x1A8, 32, RW, DPPB, 0xFFFFFFFD, 0},   
	{"rDMACC5Control0		", DMA0_BASE+0x1AC, 32, RW, DPPB, 0xFFFFF000, 0},   
	{"rDMACC5Control1		", DMA0_BASE+0x1B0, 25, RW, DPPB, 0x0, 0},   // not WO, but this register acts like WO.
	{"rDMACC5Configuration	", DMA0_BASE+0x1B4, 19, RW, DPDB, 0, 0},   
	// Ch6
	{"rDMACC6SrcAddr		", DMA0_BASE+0x1C0, 32, RW, DPDB, 0, 0},   
	{"rDMACC6DestAddr		", DMA0_BASE+0x1C4, 32, RW, DPDB, 0, 0},   
	{"rDMACC6LLI				", DMA0_BASE+0x1C8, 32, RW, DPPB, 0xFFFFFFFD, 0},   
	{"rDMACC6Control0		", DMA0_BASE+0x1CC, 32, RW, DPPB, 0xFFFFF000, 0},   
	{"rDMACC6Control1		", DMA0_BASE+0x1D0, 25, RW, DPPB, 0x0, 0},   // not WO, but this register acts like WO.
	{"rDMACC6Configuration	", DMA0_BASE+0x1D4, 19, RW, DPDB, 0, 0},   
	// Ch7
	{"rDMACC7SrcAddr		", DMA0_BASE+0x1E0, 32, RW, DPDB, 0, 0},   
	{"rDMACC7DestAddr		", DMA0_BASE+0x1E4, 32, RW, DPDB, 0, 0},   
	{"rDMACC7LLI				", DMA0_BASE+0x1E8, 32, RW, DPPB, 0xFFFFFFFD, 0},   
	{"rDMACC7Control0		", DMA0_BASE+0x1EC, 32, RW, DPPB, 0xFFFFF000, 0},   
	{"rDMACC7Control1		", DMA0_BASE+0x1F0, 25, RW, DPPB, 0x0, 0},   // not WO, but this register acts like WO.
	{"rDMACC7Configuration	", DMA0_BASE+0x1F4, 19, RW, DPDB, 0, 0},   

	{"rDMAC0Configuration		", DMA0_BASE+0x30, 3,   RW, DPDB, 0, 0},   

	//------------------ DMAC1 -----------------------------
	{"rDMAC1IntStatus			", DMA1_BASE+0x00, 8, RO, DPDB, 0, 0},   	
	{"rDMAC1IntTCStatus			", DMA1_BASE+0x04, 8, RO, DPDB, 0, 0},   
	{"rDMAC1IntTCClear			", DMA1_BASE+0x08, 8 , WO, DPDB, 0, 0},   
	{"rDMAC1IntErrorStatus		", DMA1_BASE+0x0C, 8 , RO, DPDB, 0, 0},   
	{"rDMAC1IntErrClr			", DMA1_BASE+0x10, 8, WO, DPDB, 0, 0},   
	{"rDMAC1RawIntTCStatus		", DMA1_BASE+0x14, 8, RO, DPDB, 0, 0},   
	{"rDMAC1RawIntErrorStatus	", DMA1_BASE+0x18, 8, RO, DPDB, 0, 0},   
	{"rDMAC1EnbldChns			", DMA1_BASE+0x1C, 8, RO, DPDB, 0, 0},   
	{"rDMAC1SoftBReq			", DMA1_BASE+0x20, 16, RW, DPDB, 0, 0},   
	{"rDMAC1SoftSReq			", DMA1_BASE+0x24, 16, RW, DPDB, 0, 0},   
	{"rDMAC1SoftLBReq			", DMA1_BASE+0x28, 16, RW, DPDB, 0, 0},   
	{"rDMAC1SoftLSReq			", DMA1_BASE+0x2C, 16, RW, DPDB, 0, 0},   
	{"rDMAC1Sync				", DMA1_BASE+0x34, 16, RW, DPDB, 0, 0},   
                                                                 	
	// Ch0
	{"rDMAC1C0SrcAddr		", DMA1_BASE+0x100, 32, RW, DPDB, 0, 0},   
	{"rDMAC1C0DestAddr		", DMA1_BASE+0x104, 32, RW, DPDB, 0, 0},   
	{"rDMAC1C0LLI			", DMA1_BASE+0x108, 32, RW, DPPB, 0xFFFFFFFD, 0},   
	{"rDMAC1C0Control0		", DMA1_BASE+0x10C, 32, RW, DPPB, 0xFFFFF000, 0},   
	{"rDMAC1C0Control1		", DMA1_BASE+0x110, 25, RW, DPPB, 0x0, 0},   // not WO, but this register acts like WO.
	{"rDMAC1C0Configuration	", DMA1_BASE+0x114, 19, RW, DPDB, 0, 0},   
	// Ch1                        
	{"rDMAC1C1SrcAddr		", DMA1_BASE+0x120, 32, RW, DPDB, 0, 0},   
	{"rDMAC1C1DestAddr		", DMA1_BASE+0x124, 32, RW, DPDB, 0, 0},   
	{"rDMAC1C1LLI			", DMA1_BASE+0x128, 32, RW, DPPB, 0xFFFFFFFD, 0},   
	{"rDMAC1C1Control0		", DMA1_BASE+0x12C, 32, RW, DPPB, 0xFFFFF000, 0},   
	{"rDMAC1C1Control1		", DMA1_BASE+0x130, 25, RW, DPPB, 0x0, 0},   // not WO, but this register acts like WO.
	{"rDMAC1C1Configuration	", DMA1_BASE+0x134, 19, RW, DPDB, 0, 0},   
	// Ch2 
	{"rDMAC1C2SrcAddr		", DMA1_BASE+0x140, 32, RW, DPDB, 0, 0},   
	{"rDMAC1C2DestAddr		", DMA1_BASE+0x144, 32, RW, DPDB, 0, 0},   
	{"rDMAC1C2LLI			", DMA1_BASE+0x148, 32, RW, DPPB, 0xFFFFFFFD, 0},   
	{"rDMAC1C2Control0		", DMA1_BASE+0x14C, 32, RW, DPPB, 0xFFFFF000, 0},   
	{"rDMAC1C2Control1		", DMA1_BASE+0x150, 25, RW, DPPB, 0x0, 0},   // not WO, but this register acts like WO.
	{"rDMAC1C2Configuration	", DMA1_BASE+0x154, 19, RW, DPDB, 0, 0},   
	// Ch3                   
	{"rDMAC1C3SrcAddr		", DMA1_BASE+0x160, 32, RW, DPDB, 0, 0},   
	{"rDMAC1C3DestAddr		", DMA1_BASE+0x164, 32, RW, DPDB, 0, 0},   
	{"rDMAC1C3LLI			", DMA1_BASE+0x168, 32, RW, DPPB, 0xFFFFFFFD, 0},   
	{"rDMAC1C3Control0		", DMA1_BASE+0x16C, 32, RW, DPPB, 0xFFFFF000, 0},   
	{"rDMAC1C3Control1		", DMA1_BASE+0x170, 25, RW, DPPB, 0x0, 0},   // not WO, but this register acts like WO.
	{"rDMAC1C3Configuration	", DMA1_BASE+0x174, 19, RW, DPDB, 0, 0},   
	// Ch4                  
	{"rDMAC1C4SrcAddr		", DMA1_BASE+0x180, 32, RW, DPDB, 0, 0},   
	{"rDMAC1C4DestAddr		", DMA1_BASE+0x184, 32, RW, DPDB, 0, 0},   
	{"rDMAC1C4LLI			", DMA1_BASE+0x188, 32, RW, DPPB, 0xFFFFFFFD, 0},   
	{"rDMAC1C4Control0		", DMA1_BASE+0x18C, 32, RW, DPPB, 0xFFFFF000, 0},   
	{"rDMAC1C4Control1		", DMA1_BASE+0x190, 25, RW, DPPB, 0x0, 0},   // not WO, but this register acts like WO.
	{"rDMAC1C4Configuration	", DMA1_BASE+0x194, 19, RW, DPDB, 0, 0},   
	// Ch5               
	{"rDMAC1C5SrcAddr		", DMA1_BASE+0x1A0, 32, RW, DPDB, 0, 0},   
	{"rDMAC1C5DestAddr		", DMA1_BASE+0x1A4, 32, RW, DPDB, 0, 0},   
	{"rDMAC1C5LLI			", DMA1_BASE+0x1A8, 32, RW, DPPB, 0xFFFFFFFD, 0},   
	{"rDMAC1C5Control0		", DMA1_BASE+0x1AC, 32, RW, DPPB, 0xFFFFF000, 0},   
	{"rDMAC1C5Control1		", DMA1_BASE+0x1B0, 25, RW, DPPB, 0x0, 0},   // not WO, but this register acts like WO.
	{"rDMAC1C5Configuration	", DMA1_BASE+0x1B4, 19, RW, DPDB, 0, 0},   
	// Ch6
	{"rDMAC1C6SrcAddr		", DMA1_BASE+0x1C0, 32, RW, DPDB, 0, 0},   
	{"rDMAC1C6DestAddr		", DMA1_BASE+0x1C4, 32, RW, DPDB, 0, 0},   
	{"rDMAC1C6LLI			", DMA1_BASE+0x1C8, 32, RW, DPPB, 0xFFFFFFFD, 0},   
	{"rDMAC1C6Control0		", DMA1_BASE+0x1CC, 32, RW, DPPB, 0xFFFFF000, 0},   
	{"rDMAC1C6Control1		", DMA1_BASE+0x1D0, 25, RW, DPPB, 0x0, 0},   // not WO, but this register acts like WO.
	{"rDMAC1C6Configuration	", DMA1_BASE+0x1D4, 19, RW, DPDB, 0, 0},   
	// Ch7
	{"rDMAC1C7SrcAddr		", DMA1_BASE+0x1E0, 32, RW, DPDB, 0, 0},   
	{"rDMAC1C7DestAddr		", DMA1_BASE+0x1E4, 32, RW, DPDB, 0, 0},   
	{"rDMAC1C7LLI			", DMA1_BASE+0x1E8, 32, RW, DPPB, 0xFFFFFFFD, 0},   
	{"rDMAC1C7Control0		", DMA1_BASE+0x1EC, 32, RW, DPPB, 0xFFFFF000, 0},   
	{"rDMAC1C7Control1		", DMA1_BASE+0x1F0, 25, RW, DPPB, 0x0, 0},   // not WO, but this register acts like WO.
	{"rDMAC1C7Configuration	", DMA1_BASE+0x1F4, 19, RW, DPDB, 0, 0},   

	{"rDMAC1Configuration		", DMA1_BASE+0x30, 3,   RW, DPDB, 0, 0},   	

	//------------------ SDMAC0 -----------------------------
	{"rSDMAC0IntStatus			", SDMA0_BASE+0x00, 8, RO, DPDB, 0, 0},   	
	{"rSDMAC0IntTCStatus			", SDMA0_BASE+0x04, 8, RO, DPDB, 0, 0},   
	{"rSDMAC0IntTCClear			", SDMA0_BASE+0x08, 8 , WO, DPDB, 0, 0},   
	{"rSDMAC0IntErrorStatus		", SDMA0_BASE+0x0C, 8 , RO, DPDB, 0, 0},   
	{"rSDMAC0IntErrClr			", SDMA0_BASE+0x10, 8, WO, DPDB, 0, 0},   
	{"rSDMAC0RawIntTCStatus		", SDMA0_BASE+0x14, 8, RO, DPDB, 0, 0},   
	{"rSDMAC0RawIntErrorStatus	", SDMA0_BASE+0x18, 8, RO, DPDB, 0, 0},   
	{"rSDMAC0EnbldChns			", SDMA0_BASE+0x1C, 8, RO, DPDB, 0, 0},   
	{"rSDMAC0SoftBReq			", SDMA0_BASE+0x20, 16, RW, DPDB, 0, 0},   
	{"rSDMAC0SoftSReq			", SDMA0_BASE+0x24, 16, RW, DPDB, 0, 0},   
	{"rSDMAC0SoftLBReq			", SDMA0_BASE+0x28, 16, RW, DPDB, 0, 0},   
	{"rSDMAC0SoftLSReq			", SDMA0_BASE+0x2C, 16, RW, DPDB, 0, 0},   
	{"rSDMAC0Sync				", SDMA0_BASE+0x34, 16, RW, DPDB, 0, 0},   
                                                                 	
	// Ch0
	{"rSDMAC0C0SrcAddr		", SDMA0_BASE+0x100, 32, RW, DPDB, 0, 0},   
	{"rSDMAC0C0DestAddr		", SDMA0_BASE+0x104, 32, RW, DPDB, 0, 0},   
	{"rSDMAC0C0LLI			", SDMA0_BASE+0x108, 32, RW, DPPB, 0xFFFFFFFD, 0},   
	{"rSDMAC0C0Control0		", SDMA0_BASE+0x10C, 32, RW, DPPB, 0xFFFFF000, 0},   
	{"rSDMAC0C0Control1		", SDMA0_BASE+0x110, 25, RW, DPPB, 0x0, 0},   // not WO, but this register acts like WO.
	{"rSDMAC0C0Configuration", SDMA0_BASE+0x114, 19, RW, DPDB, 0, 0},   
	// Ch1
	{"rSDMAC0C1SrcAddr		", SDMA0_BASE+0x120, 32, RW, DPDB, 0, 0},   
	{"rSDMAC0C1DestAddr		", SDMA0_BASE+0x124, 32, RW, DPDB, 0, 0},   
	{"rSDMAC0C1LLI			", SDMA0_BASE+0x128, 32, RW, DPPB, 0xFFFFFFFD, 0},   
	{"rSDMAC0C1Control0		", SDMA0_BASE+0x12C, 32, RW, DPPB, 0xFFFFF000, 0},   
	{"rSDMAC0C1Control1		", SDMA0_BASE+0x130, 25, RW, DPPB, 0x0, 0},   // not WO, but this register acts like WO.
	{"rSDMAC0C1Configuration", SDMA0_BASE+0x134, 19, RW, DPDB, 0, 0},   
	// Ch2
	{"rSDMAC0C2SrcAddr		", SDMA0_BASE+0x140, 32, RW, DPDB, 0, 0},   
	{"rSDMAC0C2DestAddr		", SDMA0_BASE+0x144, 32, RW, DPDB, 0, 0},   
	{"rSDMAC0C2LLI			", SDMA0_BASE+0x148, 32, RW, DPPB, 0xFFFFFFFD, 0},   
	{"rSDMAC0C2Control0		", SDMA0_BASE+0x14C, 32, RW, DPPB, 0xFFFFF000, 0},   
	{"rSDMAC0C2Control1		", SDMA0_BASE+0x150, 25, RW, DPPB, 0x0, 0},   // not WO, but this register acts like WO.
	{"rSDMAC0C2Configuration", SDMA0_BASE+0x154, 19, RW, DPDB, 0, 0},   
	// Ch3
	{"rSDMAC0C3SrcAddr		", SDMA0_BASE+0x160, 32, RW, DPDB, 0, 0},   
	{"rSDMAC0C3DestAddr		", SDMA0_BASE+0x164, 32, RW, DPDB, 0, 0},   
	{"rSDMAC0C3LLI			", SDMA0_BASE+0x168, 32, RW, DPPB, 0xFFFFFFFD, 0},   
	{"rSDMAC0C3Control0		", SDMA0_BASE+0x16C, 32, RW, DPPB, 0xFFFFF000, 0},   
	{"rSDMAC0C3Control1		", SDMA0_BASE+0x170, 25, RW, DPPB, 0x0, 0},   // not WO, but this register acts like WO.
	{"rSDMAC0C3Configuration", SDMA0_BASE+0x174, 19, RW, DPDB, 0, 0},   
	// Ch4
	{"rSDMAC0C4SrcAddr		", SDMA0_BASE+0x180, 32, RW, DPDB, 0, 0},   
	{"rSDMAC0C4DestAddr		", SDMA0_BASE+0x184, 32, RW, DPDB, 0, 0},   
	{"rSDMAC0C4LLI			", SDMA0_BASE+0x188, 32, RW, DPPB, 0xFFFFFFFD, 0},   
	{"rSDMAC0C4Control0		", SDMA0_BASE+0x18C, 32, RW, DPPB, 0xFFFFF000, 0},   
	{"rSDMAC0C4Control1		", SDMA0_BASE+0x190, 25, RW, DPPB, 0x0, 0},   // not WO, but this register acts like WO.
	{"rSDMAC0C4Configuration", SDMA0_BASE+0x194, 19, RW, DPDB, 0, 0},   
	// Ch5
	{"rSDMAC0C5SrcAddr		", SDMA0_BASE+0x1A0, 32, RW, DPDB, 0, 0},   
	{"rSDMAC0C5DestAddr		", SDMA0_BASE+0x1A4, 32, RW, DPDB, 0, 0},   
	{"rSDMAC0C5LLI			", SDMA0_BASE+0x1A8, 32, RW, DPPB, 0xFFFFFFFD, 0},   
	{"rSDMAC0C5Control0		", SDMA0_BASE+0x1AC, 32, RW, DPPB, 0xFFFFF000, 0},   
	{"rSDMAC0C5Control1		", SDMA0_BASE+0x1B0, 25, RW, DPPB, 0x0, 0},   // not WO, but this register acts like WO.
	{"rSDMAC0C5Configuration", SDMA0_BASE+0x1B4, 19, RW, DPDB, 0, 0},   
	// Ch6 
	{"rSDMAC0C6SrcAddr		", SDMA0_BASE+0x1C0, 32, RW, DPDB, 0, 0},   
	{"rSDMAC0C6DestAddr		", SDMA0_BASE+0x1C4, 32, RW, DPDB, 0, 0},   
	{"rSDMAC0C6LLI			", SDMA0_BASE+0x1C8, 32, RW, DPPB, 0xFFFFFFFD, 0},   
	{"rSDMAC0C6Control0		", SDMA0_BASE+0x1CC, 32, RW, DPPB, 0xFFFFF000, 0},   
	{"rSDMAC0C6Control1		", SDMA0_BASE+0x1D0, 25, RW, DPPB, 0x0, 0},   // not WO, but this register acts like WO.
	{"rSDMAC0C6Configuration", SDMA0_BASE+0x1D4, 19, RW, DPDB, 0, 0},   
	// Ch7
	{"rSDMAC0C7SrcAddr		", SDMA0_BASE+0x1E0, 32, RW, DPDB, 0, 0},   
	{"rSDMAC0C7DestAddr		", SDMA0_BASE+0x1E4, 32, RW, DPDB, 0, 0},   
	{"rSDMAC0C7LLI			", SDMA0_BASE+0x1E8, 32, RW, DPPB, 0xFFFFFFFD, 0},   
	{"rSDMAC0C7Control0		", SDMA0_BASE+0x1EC, 32, RW, DPPB, 0xFFFFF000, 0},   
	{"rSDMAC0C7Control1		", SDMA0_BASE+0x1F0, 25, RW, DPPB, 0x0, 0},   // not WO, but this register acts like WO.
	{"rSDMAC0C7Configuration", SDMA0_BASE+0x1F4, 19, RW, DPDB, 0, 0},   
    
	{"rSDMAC0Configuration	", SDMA0_BASE+0x30, 3,   RW, DPDB, 0, 0},   		
	
	//------------------ SDMAC1 -----------------------------
	{"rSDMAC1IntStatus			", SDMA1_BASE+0x00, 8, RO, DPDB, 0, 0},   	
	{"rSDMAC1IntTCStatus		", SDMA1_BASE+0x04, 8, RO, DPDB, 0, 0},   
	{"rSDMAC1IntTCClear			", SDMA1_BASE+0x08, 8 , WO, DPDB, 0, 0},   
	{"rSDMAC1IntErrorStatus		", SDMA1_BASE+0x0C, 8 , RO, DPDB, 0, 0},   
	{"rSDMAC1IntErrClr			", SDMA1_BASE+0x10, 8, WO, DPDB, 0, 0},   
	{"rSDMAC1RawIntTCStatus		", SDMA1_BASE+0x14, 8, RO, DPDB, 0, 0},   
	{"rSDMAC1RawIntErrorStatus	", SDMA1_BASE+0x18, 8, RO, DPDB, 0, 0},   
	{"rSDMAC1EnbldChns			", SDMA1_BASE+0x1C, 8, RO, DPDB, 0, 0},   
	{"rSDMAC1SoftBReq			", SDMA1_BASE+0x20, 16, RW, DPDB, 0, 0},   
	{"rSDMAC1SoftSReq			", SDMA1_BASE+0x24, 16, RW, DPDB, 0, 0},   
	{"rSDMAC1SoftLBReq			", SDMA1_BASE+0x28, 16, RW, DPDB, 0, 0},   
	{"rSDMAC1SoftLSReq			", SDMA1_BASE+0x2C, 16, RW, DPDB, 0, 0},   
	{"rSDMAC1Sync				", SDMA1_BASE+0x34, 16, RW, DPDB, 0, 0},   
                                                                 	
	// Ch0
	{"rSDMAC1C0SrcAddr		", SDMA1_BASE+0x100, 32, RW, DPDB, 0, 0},   
	{"rSDMAC1C0DestAddr		", SDMA1_BASE+0x104, 32, RW, DPDB, 0, 0},   
	{"rSDMAC1C0LLI			", SDMA1_BASE+0x108, 32, RW, DPPB, 0xFFFFFFFD, 0},   
	{"rSDMAC1C0Control0		", SDMA1_BASE+0x10C, 32, RW, DPPB, 0xFFFFF000, 0},   
	{"rSDMAC1C0Control1		", SDMA1_BASE+0x110, 25, RW, DPPB, 0x0, 0},   // not WO, but this register acts like WO.
	{"rSDMAC1C0Configuration", SDMA1_BASE+0x114, 19, RW, DPDB, 0, 0},   
	// Ch1
	{"rSDMAC1C1SrcAddr		", SDMA1_BASE+0x120, 32, RW, DPDB, 0, 0},   
	{"rSDMAC1C1DestAddr		", SDMA1_BASE+0x124, 32, RW, DPDB, 0, 0},   
	{"rSDMAC1C1LLI			", SDMA1_BASE+0x128, 32, RW, DPPB, 0xFFFFFFFD, 0},   
	{"rSDMAC1C1Control0		", SDMA1_BASE+0x12C, 32, RW, DPPB, 0xFFFFF000, 0},   
	{"rSDMAC1C1Control1		", SDMA1_BASE+0x130, 25, RW, DPPB, 0x0, 0},   // not WO, but this register acts like WO.
	{"rSDMAC1C1Configuration", SDMA1_BASE+0x134, 19, RW, DPDB, 0, 0},   
	// Ch2
	{"rSDMAC1C2SrcAddr		", SDMA1_BASE+0x140, 32, RW, DPDB, 0, 0},   
	{"rSDMAC1C2DestAddr		", SDMA1_BASE+0x144, 32, RW, DPDB, 0, 0},   
	{"rSDMAC1C2LLI			", SDMA1_BASE+0x148, 32, RW, DPPB, 0xFFFFFFFD, 0},   
	{"rSDMAC1C2Control0		", SDMA1_BASE+0x14C, 32, RW, DPPB, 0xFFFFF000, 0},   
	{"rSDMAC1C2Control1		", SDMA1_BASE+0x150, 25, RW, DPPB, 0x0, 0},   // not WO, but this register acts like WO.
	{"rSDMAC1C2Configuration", SDMA1_BASE+0x154, 19, RW, DPDB, 0, 0},   
	// Ch3
	{"rSDMAC1C3SrcAddr		", SDMA1_BASE+0x160, 32, RW, DPDB, 0, 0},   
	{"rSDMAC1C3DestAddr		", SDMA1_BASE+0x164, 32, RW, DPDB, 0, 0},   
	{"rSDMAC1C3LLI			", SDMA1_BASE+0x168, 32, RW, DPPB, 0xFFFFFFFD, 0},   
	{"rSDMAC1C3Control0		", SDMA1_BASE+0x16C, 32, RW, DPPB, 0xFFFFF000, 0},   
	{"rSDMAC1C3Control1		", SDMA1_BASE+0x170, 25, RW, DPPB, 0x0, 0},   // not WO, but this register acts like WO.
	{"rSDMAC1C3Configuration", SDMA1_BASE+0x174, 19, RW, DPDB, 0, 0},   
	// Ch4
	{"rSDMAC1C4SrcAddr		", SDMA1_BASE+0x180, 32, RW, DPDB, 0, 0},   
	{"rSDMAC1C4DestAddr		", SDMA1_BASE+0x184, 32, RW, DPDB, 0, 0},   
	{"rSDMAC1C4LLI			", SDMA1_BASE+0x188, 32, RW, DPPB, 0xFFFFFFFD, 0},   
	{"rSDMAC1C4Control0		", SDMA1_BASE+0x18C, 32, RW, DPPB, 0xFFFFF000, 0},   
	{"rSDMAC1C4Control1		", SDMA1_BASE+0x190, 25, RW, DPPB, 0x0, 0},   // not WO, but this register acts like WO.
	{"rSDMAC1C4Configuration", SDMA1_BASE+0x194, 19, RW, DPDB, 0, 0},   
	// Ch5
	{"rSDMAC1C5SrcAddr		", SDMA1_BASE+0x1A0, 32, RW, DPDB, 0, 0},   
	{"rSDMAC1C5DestAddr		", SDMA1_BASE+0x1A4, 32, RW, DPDB, 0, 0},   
	{"rSDMAC1C5LLI			", SDMA1_BASE+0x1A8, 32, RW, DPPB, 0xFFFFFFFD, 0},   
	{"rSDMAC1C5Control0		", SDMA1_BASE+0x1AC, 32, RW, DPPB, 0xFFFFF000, 0},   
	{"rSDMAC1C5Control1		", SDMA1_BASE+0x1B0, 25, RW, DPPB, 0x0, 0},   // not WO, but this register acts like WO.
	{"rSDMAC1C5Configuration", SDMA1_BASE+0x1B4, 19, RW, DPDB, 0, 0},   
	// Ch6
	{"rSDMAC1C6SrcAddr		", SDMA1_BASE+0x1C0, 32, RW, DPDB, 0, 0},   
	{"rSDMAC1C6DestAddr		", SDMA1_BASE+0x1C4, 32, RW, DPDB, 0, 0},   
	{"rSDMAC1C6LLI			", SDMA1_BASE+0x1C8, 32, RW, DPPB, 0xFFFFFFFD, 0},   
	{"rSDMAC1C6Control0		", SDMA1_BASE+0x1CC, 32, RW, DPPB, 0xFFFFF000, 0},   
	{"rSDMAC1C6Control1		", SDMA1_BASE+0x1D0, 25, RW, DPPB, 0x0, 0},   // not WO, but this register acts like WO.
	{"rSDMAC1C6Configuration", SDMA1_BASE+0x1D4, 19, RW, DPDB, 0, 0},   
	// Ch7
	{"rSDMAC1C7SrcAddr		", SDMA1_BASE+0x1E0, 32, RW, DPDB, 0, 0},   
	{"rSDMAC1C7DestAddr		", SDMA1_BASE+0x1E4, 32, RW, DPDB, 0, 0},   
	{"rSDMAC1C7LLI			", SDMA1_BASE+0x1E8, 32, RW, DPPB, 0xFFFFFFFD, 0},   
	{"rSDMAC1C7Control0		", SDMA1_BASE+0x1EC, 32, RW, DPPB, 0xFFFFF000, 0},   
	{"rSDMAC1C7Control1		", SDMA1_BASE+0x1F0, 25, RW, DPPB, 0x0, 0},   // not WO, but this register acts like WO.
	{"rSDMAC1C7Configuration", SDMA1_BASE+0x1F4, 19, RW, DPDB, 0, 0},   

	{"rSDMAC1Configuration	", SDMA1_BASE+0x30, 3,   RW, DPDB, 0, 0},   		
#endif
	

#if 0
//  UART0
/* debug channel
    {"rUlCon0          ", UART_BASE+0x00, 7, RW, DPDB, 0, 0},
    {"rUCon0           ", UART_BASE+0x04, 12, RW, DPDB, 0, 0},
    {"rUfCon0          ", UART_BASE+0x08, 8, RW, DPDB, 0, 0},
    {"rUmCon0          ", UART_BASE+0x0C, 8, RW, DPDB, 0, 0},
    {"rUtrStat0        ", UART_BASE+0x10, 3, RO, DPDB, 0, 0},
    {"rUerStat0        ", UART_BASE+0x14, 4, RO, DPDB, 0, 0},
    {"rUfStat0         ", UART_BASE+0x18, 15, RO, DPDB, 0, 0},
    {"rUmStat0         ", UART_BASE+0x1C, 5, RO, DPDB, 0, 0},
    {"rUtxh0           ", UART_BASE+0x20, 8, WO, DPDB, 0, 0},
    {"rUrxh0           ", UART_BASE+0x24, 8, RO, DPDB, 0, 0},
    {"rUbrDiv0         ", UART_BASE+0x28, 16, RW, DPDB, 0, 0},
    {"rUdivSlot0       ", UART_BASE+0x2C, 16, RW, DPDB, 0, 0},
    {"rUintP0          ", UART_BASE+0x30, 4, RW, DPDB, 0, 0},
    {"rUintSp0         ", UART_BASE+0x34, 4, RW, DPDB, 0, 0},
    {"rUintM0          ", UART_BASE+0x38, 4, RW, DPDB, 0, 0},
*/
//  UART1
    {"rUlCon1          ", UART_BASE+0x400+0x00, 7,  RW, DPDB, 0, 0},
    {"rUCon1           ", UART_BASE+0x400+0x04, 12, RW, DPDB, 0, 0},
    {"rUfCon1          ", UART_BASE+0x400+0x08, 8,  RW, DPDB, 0, 0},
    {"rUmCon1          ", UART_BASE+0x400+0x0C, 8,  RW, DPDB, 0, 0},
    {"rUtrStat1        ", UART_BASE+0x400+0x10, 3,  RO, DPDB, 0, 0},
    {"rUerStat1        ", UART_BASE+0x400+0x14, 4,  RO, DPDB, 0, 0},
    {"rUfStat1         ", UART_BASE+0x400+0x18, 15, RO, DPDB, 0, 0},
    {"rUmStat1         ", UART_BASE+0x400+0x1C, 5,  RO, DPDB, 0, 0},
    {"rUtxh1           ", UART_BASE+0x400+0x20, 8,  WO, DPDB, 0, 0},
    {"rUrxh1           ", UART_BASE+0x400+0x24, 8,  RO, DPDB, 0, 0},
    {"rUbrDiv1         ", UART_BASE+0x400+0x28, 16, RW, DPDB, 0, 0},
    {"rUdivSlot1       ", UART_BASE+0x400+0x2C, 16, RW, DPDB, 0, 0},
    {"rUintP1          ", UART_BASE+0x400+0x30, 4,  RW, DPDB, 0, 0},
    {"rUintSp1         ", UART_BASE+0x400+0x34, 4,  RW, DPDB, 0, 0},
    {"rUintM1          ", UART_BASE+0x400+0x38, 4,  RW, DPDB, 0, 0},
//  UART2
    {"rUlCon2          ", UART_BASE+(0x400*2)+0x00, 7,  RW, DPDB, 0, 0},
    {"rUCon2           ", UART_BASE+(0x400*2)+0x04, 12, RW, DPDB, 0, 0},
    {"rUfCon2          ", UART_BASE+(0x400*2)+0x08, 8,  RW, DPDB, 0, 0},
    //{"rUmCon2          ", UART_BASE+(0x400*2)+0x0C, 8,  RW, DPDB, 0, 0},
    {"rUtrStat2        ", UART_BASE+(0x400*2)+0x10, 3,  RO, DPDB, 0, 0},
    {"rUerStat2        ", UART_BASE+(0x400*2)+0x14, 4,  RO, DPDB, 0, 0},
    {"rUfStat2         ", UART_BASE+(0x400*2)+0x18, 15, RO, DPDB, 0, 0},
    //{"rUmStat2         ", UART_BASE+(0x400*2)+0x1C, 5,  RO, DPDB, 0, 0},
    {"rUtxh2           ", UART_BASE+(0x400*2)+0x20, 8,  WO, DPDB, 0, 0},
    {"rUrxh2           ", UART_BASE+(0x400*2)+0x24, 8,  RO, DPDB, 0, 0},
    {"rUbrDiv2         ", UART_BASE+(0x400*2)+0x28, 16, RW, DPDB, 0, 0},
    {"rUdivSlot2       ", UART_BASE+(0x400*2)+0x2C, 16, RW, DPDB, 0, 0},
    {"rUintP2          ", UART_BASE+(0x400*2)+0x30, 4,  RW, DPDB, 0, 0},
    {"rUintSp2         ", UART_BASE+(0x400*2)+0x34, 4,  RW, DPDB, 0, 0},
    {"rUintM2          ", UART_BASE+(0x400*2)+0x38, 4,  RW, DPDB, 0, 0},    

//  UART3
    {"rUlCon3          ", UART_BASE+(0x400*3)+0x00, 7,  RW, DPDB, 0, 0},
    {"rUCon3           ", UART_BASE+(0x400*3)+0x04, 12, RW, DPDB, 0, 0},
    {"rUfCon3          ", UART_BASE+(0x400*3)+0x08, 8,  RW, DPDB, 0, 0},
    //{"rUmCon3          ", UART_BASE+(0x400*3)+0x0C, 8,  RW, DPDB, 0, 0},
    {"rUtrStat3        ", UART_BASE+(0x400*3)+0x10, 3,  RO, DPDB, 0, 0},
    {"rUerStat3        ", UART_BASE+(0x400*3)+0x14, 4,  RO, DPDB, 0, 0},
    {"rUfStat3         ", UART_BASE+(0x400*3)+0x18, 15, RO, DPDB, 0, 0},
    //{"rUmStat3         ", UART_BASE+(0x400*3)+0x1C, 5,  RO, DPDB, 0, 0},
    {"rUtxh3           ", UART_BASE+(0x400*3)+0x20, 8,  WO, DPDB, 0, 0},
    {"rUrxh3           ", UART_BASE+(0x400*3)+0x24, 8,  RO, DPDB, 0, 0},
    {"rUbrDiv3         ", UART_BASE+(0x400*3)+0x28, 16, RW, DPDB, 0, 0},
    {"rUdivSlot3       ", UART_BASE+(0x400*3)+0x2C, 16, RW, DPDB, 0, 0},
    {"rUintP3          ", UART_BASE+(0x400*3)+0x30, 4,  RW, DPDB, 0, 0},
    {"rUintSp3         ", UART_BASE+(0x400*3)+0x34, 4,  RW, DPDB, 0, 0},
    {"rUintM3          ", UART_BASE+(0x400*3)+0x38, 4,  RW, DPDB, 0, 0},
#endif	
};

void RPRINTF(REGINFO sReg, u32 uPattern, u8 uRegStatus, s32 Nth,u8 uInDetail)
{
#if 0
	
	if(uInDetail)
		UART_Printf("\n%s,0x%X,0x%X,0x%X", sReg.name, sReg.uAddr, uPattern, sReg.rValue);	
	else
	{	
		if(!Nth)	
		UART_Printf("\n%s,0x%X\t", sReg.name, sReg.uAddr);
	}
	if(uRegStatus)
	{
		if(uInDetail)
			UART_Printf("\t[%d]",Nth);
		else
			UART_Printf("%d)",Nth);			
	}
#else		// Edited by SOP on 2008/03/01
	if(uInDetail)
		UART_Printf("\n%20s (0x%08X): Pattern 0x%08X, Read 0x%08X", sReg.name, sReg.uAddr, uPattern, sReg.rValue);	
	else
	{	
		if(!Nth)	
		UART_Printf("\n%20s, 0x%08X\t", sReg.name, sReg.uAddr);
	}
	if(uRegStatus)
	{
		if(uInDetail)
			UART_Printf(", Error Pattern Bit[%d]",Nth);
		else
			UART_Printf("%d)",Nth);			
	}

#endif	
	return;
}

void TestSFR(void)
{
	volatile u32 *pAddr;	
	s32 i,j, count = sizeof(sRegInfo)/sizeof(REGINFO), nPattern;
	u32 uRegStatus, uWritePatternWithMask, uReadWithMask;	
	
	
	
	u32 uPatternShitfOneBit[] = 
	{
		0x00000001,		0x00000002,		0x00000004,		0x00000008,		0x00000010,		0x00000020,		0x00000040,		0x00000080,
		0x00000100,		0x00000200,		0x00000400,		0x00000800,		0x00001000,		0x00002000,		0x00004000,		0x00008000,
		0x00010000,		0x00020000,		0x00040000,		0x00080000,		0x00100000,		0x00200000,		0x00400000,		0x00800000,
		0x01000000,		0x02000000,		0x04000000,		0x08000000,		0x10000000,		0x20000000,		0x40000000,		0x80000000,
	};
	
	u32 uDefaultBitMask[] =
	{
		0x00000001,		0x00000003,		0x00000007,		0x0000000F,		0x0000001F,		0x0000003F,		0x0000007F,		0x000000FF,
		0x000001FF,		0x000003FF,		0x000007FF,		0x00000FFF,		0x00001FFF,		0x00003FFF,		0x00007FFF,		0x0000FFFF,
		0x0001FFFF,		0x0003FFFF,		0x0007FFFF,		0x000FFFFF,		0x001FFFFF,		0x003FFFFF,		0x007FFFFF,		0x00FFFFFF,
		0x01FFFFFF,		0x03FFFFFF,		0x07FFFFFF,		0x0FFFFFFF,		0x1FFFFFFF,		0x3FFFFFFF,		0x7FFFFFFF,		0xFFFFFFFF,
	};

	
	for(i=0; i<count; i++)
	{		
		pAddr = (u32*)sRegInfo[i].uAddr;		
		
		if(sRegInfo[i].uFlag == DPDB)
		{		
			if(sRegInfo[i].uRWType == RW)
			{
				for(j=0; j< sRegInfo[i].uBitLen ; j++)
				{			
					uWritePatternWithMask = (uPatternShitfOneBit[j] &uDefaultBitMask[j]);
					*pAddr = uWritePatternWithMask;					
					
					uReadWithMask = (*pAddr & uDefaultBitMask[j]);	
					sRegInfo[i].rValue = uReadWithMask;
					
					uRegStatus = (	uWritePatternWithMask !=uReadWithMask );					
					RPRINTF(sRegInfo[i], uPatternShitfOneBit[j], uRegStatus, j, 1);
				}
			}
		}		
		else if(sRegInfo[i].uFlag == DPPB)
		{		
			nPattern = (sizeof(uPatternShitfOneBit)/sizeof(uPatternShitfOneBit[0]));			
			if(sRegInfo[i].uRWType == RW)
			{
				for(j=0; j<nPattern; j++)
				{
					if(uPatternShitfOneBit[j] & sRegInfo[i].uPrivateBitMask)
					{
						uWritePatternWithMask = (uPatternShitfOneBit[j] & sRegInfo[i].uPrivateBitMask);
						*pAddr = uWritePatternWithMask;
						
						uReadWithMask = (*pAddr & sRegInfo[i].uPrivateBitMask);
						sRegInfo[i].rValue = uReadWithMask;
						
						uRegStatus = (	uWritePatternWithMask != uReadWithMask );												
						RPRINTF(sRegInfo[i], uPatternShitfOneBit[j], uRegStatus, j, 1);					
					}
				}
			}			
		}					
	}
	UART_Printf("\n\nTest Complete!\n\n");

	
	return ;
}

//////////
// Function Name : GetBitPosition
// Function Description : This function return the Bit Position of uValue
// Input :  	uValue - Value
// Output : 	Bit position
// Version :
// added by rb1004
u32 GetBitPosition(u32 uValue)
{
	u8 bitshift;

	for(bitshift=0 ; bitshift<32 ; bitshift++)
	{
	  if(uValue & (1<<bitshift))
	  	return bitshift;
	}
	return 0;  
}


//added by rb1004...2007.03.09
void UART_Printf(const char *fmt,...)
{
    va_list ap;
    char string[256];
	int i;

    va_start(ap, fmt);
    vsprintf(string, fmt, ap);
    for (i = 0; string[i]; i++)
		UART_Putc(string[i]);
    va_end(ap);
}

//////////
// Function Name : Inp32Not4ByteAlign
// Function Description : This function plays a similar role to Inp32(addr) when address is not aligned with 4bytes
// Input :  addr
// Output : data
// Version :
u32 Inp32Not4ByteAlign(u32 addr)
{
	u32 Temp1;
	u32 Temp2;
	u32 Temp3;
	u32 Temp4;
	
	Temp1 = Inp8(addr);
	addr++;
	Temp2 = Inp8(addr);
	Temp2 = Temp2<<8;
	addr++;
	Temp3 = Inp8(addr);
	Temp3 = Temp3<<16;
	addr++;
	Temp4 = Inp8(addr);
	Temp4 = Temp4<<24;
	return (Temp1 | Temp2 |Temp3 |Temp4 );
}


//////////
// Function Name : Pause
// Function Description : This function is Pause
// Input :  none
// Output : none
// Version : 0.1
void Pause(void)			// Added by SOP on 2008/03/04
{
	UART_Printf("\nPress any key continue ...\n");
	UART_Getc();
}