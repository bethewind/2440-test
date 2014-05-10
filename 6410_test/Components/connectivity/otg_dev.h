/**************************************************************************************
* 
*	Project Name : S3C6410 Validation
*
*	Copyright 2006 by Samsung Electronics, Inc.
*	All rights reserved.
*
*	Project Description :
*		This software is only for validating functions of the S3C6410.
*		Anybody can use this software without our permission.
*  
*--------------------------------------------------------------------------------------
* 
*	File Name : otg_dev.h
*  
*	File Description : This file declares prototypes of USB OTG API funcions and defines some values.
*
*	Author : Haksoo,Kim
*	Dept. : AP Development Team
*	Created Date : 2007/01/23
*	Version : 0.1 
* 
*	History
*	- Created(Haksoo,Kim 2007/01/23)
*  
**************************************************************************************/
 
#ifndef __OTGDEV_H__
#define __OTGDEV_H__

//=====================================================================================
//
#define CONTROL_EP						0
#define BULK_IN_EP						1
#define BULK_OUT_EP						2

#define FULL_SPEED_CONTROL_PKT_SIZE		64
#define FULL_SPEED_BULK_PKT_SIZE		64

#define HIGH_SPEED_CONTROL_PKT_SIZE		64
#define HIGH_SPEED_BULK_PKT_SIZE		512

#define RX_FIFO_SIZE					512
#define NPTX_FIFO_START_ADDR			RX_FIFO_SIZE
#define NPTX_FIFO_SIZE					512
#define PTX_FIFO_SIZE					512

// string descriptor
#define LANGID_US_L                 	(0x09)
#define LANGID_US_H                 	(0x04)

// Feature Selectors
#define EP_STALL          		0
#define DEVICE_REMOTE_WAKEUP    1
#define TEST_MODE				2

/* Test Mode Selector*/
#define TEST_J					1
#define TEST_K					2
#define TEST_SE0_NAK			3
#define TEST_PACKET				4
#define TEST_FORCE_ENABLE		5

typedef enum
{
	USB_CPU, USB_DMA
} USB_OPMODE;

typedef enum
{
	USB_HIGH, USB_FULL, USB_LOW
} USB_SPEED;

typedef enum
{
	EP_TYPE_CONTROL, EP_TYPE_ISOCHRONOUS, EP_TYPE_BULK, EP_TYPE_INTERRUPT
}EP_TYPE;

typedef struct
{
	u8 bLength;
	u8 bDescriptorType;
	u8 bcdUSBL;
	u8 bcdUSBH;
	u8 bDeviceClass;
	u8 bDeviceSubClass;
	u8 bDeviceProtocol;
	u8 bMaxPacketSize0;
	u8 idVendorL;
	u8 idVendorH;
	u8 idProductL;
	u8 idProductH;
	u8 bcdDeviceL;
	u8 bcdDeviceH;
	u8 iManufacturer;
	u8 iProduct;
	u8 iSerialNumber;
	u8 bNumConfigurations;
} USB_DEVICE_DESCRIPTOR;

typedef struct
{
	u8 bLength;
	u8 bDescriptorType;
	u8 wTotalLengthL;
	u8 wTotalLengthH;
	u8 bNumInterfaces;
	u8 bConfigurationValue;
	u8 iConfiguration;
	u8 bmAttributes;
	u8 maxPower;
} USB_CONFIGURATION_DESCRIPTOR;

typedef struct
{
	u8 bLength;
	u8 bDescriptorType;
	u8 bInterfaceNumber;
	u8 bAlternateSetting;
	u8 bNumEndpoints;
	u8 bInterfaceClass;
	u8 bInterfaceSubClass;
	u8 bInterfaceProtocol;
	u8 iInterface;
} USB_INTERFACE_DESCRIPTOR;

typedef struct
{
	u8 bLength;
	u8 bDescriptorType;
	u8 bEndpointAddress;
	u8 bmAttributes;
	u8 wMaxPacketSizeL;
	u8 wMaxPacketSizeH;
	u8 bInterval;
} USB_ENDPOINT_DESCRIPTOR;

typedef struct
{
	USB_DEVICE_DESCRIPTOR oDescDevice;
	USB_CONFIGURATION_DESCRIPTOR oDescConfig;
	USB_INTERFACE_DESCRIPTOR oDescInterface;
	USB_ENDPOINT_DESCRIPTOR oDescEndpt1;
	USB_ENDPOINT_DESCRIPTOR oDescEndpt2;
	USB_ENDPOINT_DESCRIPTOR oDescEndpt3;
	USB_ENDPOINT_DESCRIPTOR oDescEndpt4;
} USB_DESCRIPTORS;

typedef struct
{
	u8 bmRequestType;  
	u8 bRequest;       
	u8 wValue_L;       
	u8 wValue_H;       
	u8 wIndex_L;       
	u8 wIndex_H;       
	u8 wLength_L;      
	u8 wLength_H;      
} DEVICE_REQUEST;

typedef struct
{
	USB_DESCRIPTORS m_oDesc;
	DEVICE_REQUEST m_oDeviceRequest;

	u32  m_uEp0State;
	u32  m_uEp0SubState;
	USB_OPMODE m_eOpMode;
	USB_SPEED m_eSpeed;
	u32  m_uControlEPMaxPktSize;
	u32  m_uBulkInEPMaxPktSize;
	u32  m_uBulkOutEPMaxPktSize;
	u32  m_uDownloadAddress;
	u32  m_uDownloadFileSize;
	u32  m_uUploadAddr;
	u32  m_uUploadSize;
	u8*  m_pDownPt;
	u8*  m_pUpPt;
	u32  m_uIsUsbOtgSetConfiguration;
	u32  m_uDeviceRequestLength;
} OTGDEV;


typedef struct
{
	u8 ConfigurationValue;
} USB_CONFIGURATION_SET;

typedef struct
{
	u8 Device;
	u8 Interface;
	u8 EndpointCtrl;
	u8 EndpointIn;
	u8 EndpointOut;
} USB_GET_STATUS;

typedef struct
{
	u8 AlternateSetting;
} USB_INTERFACE_GET;


//=====================================================================================
// prototypes of API functions
void OTGDEV_InitOtg(USB_SPEED eSpeed);

void OTGDEV_HandleEvent(void);
void OTGDEV_HandleEvent_EP0(void);
void OTGDEV_TransferEp0(void);
void OTGDEV_HandleEvent_BulkIn(void);
void OTGDEV_HandleEvent_BulkOut(u32 fifoCntByte);


//-------------------------------------------------------------------------------------
// prototypes of sub functions
void OTGDEV_InitPhyCon(void);
void OTGDEV_SoftResetCore(void);
void OTGDEV_WaitCableInsertion(void);
void OTGDEV_InitCore(void);
void OTGDEV_CheckCurrentMode(u8 *pucMode);
void OTGDEV_SetSoftDisconnect(void);
void OTGDEV_ClearSoftDisconnect(void);
void OTGDEV_InitDevice(void);


void OTGDEV_SetAllOutEpNak(void);
void OTGDEV_ClearAllOutEpNak(void);
void OTGDEV_SetMaxPktSizes(USB_SPEED eSpeed);
void OTEDEV_SetOtherSpeedConfDescTable(u32 length);
void OTGDEV_SetEndpoint(void);
void OTGDEV_SetDescriptorTable(void);

void OTGDEV_CheckEnumeratedSpeed(USB_SPEED *eSpeed);

void OTGDEV_SetInEpXferSize(EP_TYPE eType, u32 uPktCnt, u32 uXferSize);
void OTGDEV_SetOutEpXferSize(EP_TYPE eType, u32 uPktCnt, u32 uXferSize);

void OTGDEV_WrPktEp0(u8 *buf, int num);
void OTGDEV_PrintEp0Pkt(u8 *pt, u8 count);
void OTGDEV_WrPktBulkInEp(u8 *buf, int num);
void OTGDEV_RdPktBulkOutEp(u8 *buf, int num);

//=====================================================================================
// prototypes of API functions
void OTGDEV_ClearDownFileInfo(void);
void OTGDEV_GetDownFileInfo(u32* uDownAddr, u32* uDownFileSize, u32* pDownPt);
void OTGDEV_ClearUpFileInfo(void);
void OTGDEV_GetUpFileInfo(u32* uUpAddr, u32* uUpFileSize, u32* pUpPt);
u8 OTGDEV_IsUsbOtgSetConfiguration(void);
void OTGDEV_SetOpMode(USB_OPMODE eMode);
void OTGDEV_VerifyChecksum(void);

//=====================================================================================
// extern variables
extern u8 download_run;
extern u32 tempDownloadAddress;
extern u8 g_bSuspendResume;


#endif 
