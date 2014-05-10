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
*	File Name : otg_dev.c
*  
*	File Description : This file implements the API functions for USB OTG.
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

#include "option.h"
#include "library.h"
#include "sfr6410.h"
#include "system.h"
#include "otg_dev.h"
#include "sysc.h"
#include "timer.h"

//#define DBG_USBOTG
#ifdef DBG_USBOTG
	#define DbgUsb	UART_Printf
#else
	#define DbgUsb(...)
#endif
//#define DBG_USBOTG_0
#ifdef DBG_USBOTG_0
	#define DbgUsb0	UART_Printf
#else
	#define DbgUsb0(...)
#endif

//==========================
// OTG LINK CORE REGISTERS
//==========================
enum USBOTG_REGS
{
	//==============================================================================================
	// Core Global Registers
	GOTGCTL		= (USBOTG_LINK_BASE + 0x000),		// OTG Control & Status
	GOTGINT		= (USBOTG_LINK_BASE + 0x004),		// OTG Interrupt
	GAHBCFG		= (USBOTG_LINK_BASE + 0x008),		// Core AHB Configuration
	GUSBCFG		= (USBOTG_LINK_BASE + 0x00C),		// Core USB Configuration
	GRSTCTL		= (USBOTG_LINK_BASE + 0x010),		// Core Reset
	GINTSTS		= (USBOTG_LINK_BASE + 0x014),		// Core Interrupt
	GINTMSK		= (USBOTG_LINK_BASE + 0x018),		// Core Interrupt Mask
	GRXSTSR		= (USBOTG_LINK_BASE + 0x01C),		// Receive Status Debug Read/Status Read
	GRXSTSP		= (USBOTG_LINK_BASE + 0x020),		// Receive Status Debug Pop/Status Pop
	GRXFSIZ		= (USBOTG_LINK_BASE + 0x024),		// Receive FIFO Size
	GNPTXFSIZ	= (USBOTG_LINK_BASE + 0x028),		// Non-Periodic Transmit FIFO Size
	GNPTXSTS	= (USBOTG_LINK_BASE + 0x02C),		// Non-Periodic Transmit FIFO/Queue Status

	HPTXFSIZ	= (USBOTG_LINK_BASE + 0x100),		// Host Periodic Transmit FIFO Size
	DPTXFSIZ1	= (USBOTG_LINK_BASE + 0x104),		// Device Periodic Transmit FIFO-1 Size
	DPTXFSIZ2	= (USBOTG_LINK_BASE + 0x108),		// Device Periodic Transmit FIFO-2 Size
	DPTXFSIZ3	= (USBOTG_LINK_BASE + 0x10C),		// Device Periodic Transmit FIFO-3 Size
	DPTXFSIZ4	= (USBOTG_LINK_BASE + 0x110),		// Device Periodic Transmit FIFO-4 Size
	DPTXFSIZ5	= (USBOTG_LINK_BASE + 0x114),		// Device Periodic Transmit FIFO-5 Size
	DPTXFSIZ6	= (USBOTG_LINK_BASE + 0x118),		// Device Periodic Transmit FIFO-6 Size
	DPTXFSIZ7	= (USBOTG_LINK_BASE + 0x11C),		// Device Periodic Transmit FIFO-7 Size
	DPTXFSIZ8	= (USBOTG_LINK_BASE + 0x120),		// Device Periodic Transmit FIFO-8 Size
	DPTXFSIZ9	= (USBOTG_LINK_BASE + 0x124),		// Device Periodic Transmit FIFO-9 Size
	DPTXFSIZ10	= (USBOTG_LINK_BASE + 0x128),		// Device Periodic Transmit FIFO-10 Size
	DPTXFSIZ11	= (USBOTG_LINK_BASE + 0x12C),		// Device Periodic Transmit FIFO-11 Size
	DPTXFSIZ12	= (USBOTG_LINK_BASE + 0x130),		// Device Periodic Transmit FIFO-12 Size
	DPTXFSIZ13	= (USBOTG_LINK_BASE + 0x134),		// Device Periodic Transmit FIFO-13 Size
	DPTXFSIZ14	= (USBOTG_LINK_BASE + 0x138),		// Device Periodic Transmit FIFO-14 Size
	DPTXFSIZ15	= (USBOTG_LINK_BASE + 0x13C),		// Device Periodic Transmit FIFO-15 Size
	
	//==============================================================================================
	// Host Mode Registers
	//------------------------------------------------
	// Host Global Registers
	HCFG		= (USBOTG_LINK_BASE + 0x400),		// Host Configuration
	HFIR		= (USBOTG_LINK_BASE + 0x404),		// Host Frame Interval
	HFNUM		= (USBOTG_LINK_BASE + 0x408),		// Host Frame Number/Frame Time Remaining
	HPTXSTS		= (USBOTG_LINK_BASE + 0x410),		// Host Periodic Transmit FIFO/Queue Status
	HAINT		= (USBOTG_LINK_BASE + 0x414),		// Host All Channels Interrupt
	HAINTMSK	= (USBOTG_LINK_BASE + 0x418),		// Host All Channels Interrupt Mask

	//------------------------------------------------
	// Host Port Control & Status Registers
	HPRT		= (USBOTG_LINK_BASE + 0x440),		// Host Port Control & Status

	//------------------------------------------------
	// Host Channel-Specific Registers
	HCCHAR0		= (USBOTG_LINK_BASE + 0x500),		// Host Channel-0 Characteristics
	HCSPLT0		= (USBOTG_LINK_BASE + 0x504),		// Host Channel-0 Split Control
	HCINT0		= (USBOTG_LINK_BASE + 0x508),		// Host Channel-0 Interrupt
	HCINTMSK0	= (USBOTG_LINK_BASE + 0x50C),		// Host Channel-0 Interrupt Mask
	HCTSIZ0		= (USBOTG_LINK_BASE + 0x510),		// Host Channel-0 Transfer Size
	HCDMA0		= (USBOTG_LINK_BASE + 0x514),		// Host Channel-0 DMA Address

	
	//==============================================================================================
	// Device Mode Registers
	//------------------------------------------------
	// Device Global Registers
	DCFG 		= (USBOTG_LINK_BASE + 0x800),		// Device Configuration
	DCTL 		= (USBOTG_LINK_BASE + 0x804),		// Device Control
	DSTS 		= (USBOTG_LINK_BASE + 0x808),		// Device Status
	DIEPMSK 	= (USBOTG_LINK_BASE + 0x810),		// Device IN Endpoint Common Interrupt Mask
	DOEPMSK 	= (USBOTG_LINK_BASE + 0x814),		// Device OUT Endpoint Common Interrupt Mask
	DAINT 		= (USBOTG_LINK_BASE + 0x818),		// Device All Endpoints Interrupt
	DAINTMSK 	= (USBOTG_LINK_BASE + 0x81C),		// Device All Endpoints Interrupt Mask
	DTKNQR1 	= (USBOTG_LINK_BASE + 0x820),		// Device IN Token Sequence Learning Queue Read 1
	DTKNQR2 	= (USBOTG_LINK_BASE + 0x824),		// Device IN Token Sequence Learning Queue Read 2
	DVBUSDIS 	= (USBOTG_LINK_BASE + 0x828),		// Device VBUS Discharge Time
	DVBUSPULSE 	= (USBOTG_LINK_BASE + 0x82C),		// Device VBUS Pulsing Time
	DTKNQR3 	= (USBOTG_LINK_BASE + 0x830),		// Device IN Token Sequence Learning Queue Read 3
	DTKNQR4 	= (USBOTG_LINK_BASE + 0x834),		// Device IN Token Sequence Learning Queue Read 4
	
	//------------------------------------------------
	// Device Logical IN Endpoint-Specific Registers
	DIEPCTL0 	= (USBOTG_LINK_BASE + 0x900),		// Device IN Endpoint 0 Control
	DIEPINT0 	= (USBOTG_LINK_BASE + 0x908),		// Device IN Endpoint 0 Interrupt
	DIEPTSIZ0 	= (USBOTG_LINK_BASE + 0x910),		// Device IN Endpoint 0 Transfer Size
	DIEPDMA0 	= (USBOTG_LINK_BASE + 0x914),		// Device IN Endpoint 0 DMA Address
	//------------------------------------------------
	// Device Logical OUT Endpoint-Specific Registers
	DOEPCTL0 	= (USBOTG_LINK_BASE + 0xB00),		// Device OUT Endpoint 0 Control
	DOEPINT0 	= (USBOTG_LINK_BASE + 0xB08),		// Device OUT Endpoint 0 Interrupt
	DOEPTSIZ0 	= (USBOTG_LINK_BASE + 0xB10),		// Device OUT Endpoint 0 Transfer Size
	DOEPDMA0 	= (USBOTG_LINK_BASE + 0xB14),		// Device OUT Endpoint 0 DMA Address
	
	//------------------------------------------------
	// Power & clock gating registers
	PCGCCTRL	= (USBOTG_LINK_BASE + 0xE00),

	//------------------------------------------------
	// Endpoint FIFO address
	EP0_FIFO	= (USBOTG_LINK_BASE + 0x1000)
	
 };

 
enum BULK_IN_EP_CSR
{
	bulkIn_DIEPCTL		= (DIEPCTL0 + 0x20*BULK_IN_EP),
	bulkIn_DIEPINT		= (DIEPINT0 + 0x20*BULK_IN_EP),
	bulkIn_DIEPTSIZ		= (DIEPTSIZ0 + 0x20*BULK_IN_EP),
	bulkIn_DIEPDMA		= (DIEPDMA0 + 0x20*BULK_IN_EP)
};

enum BULK_OUT_EP_CSR
{
	bulkOut_DOEPCTL		= (DOEPCTL0 + 0x20*BULK_OUT_EP),
	bulkOut_DOEPINT		= (DOEPINT0 + 0x20*BULK_OUT_EP),
	bulkOut_DOEPTSIZ	= (DOEPTSIZ0 + 0x20*BULK_OUT_EP),
	bulkOut_DOEPDMA		= (DOEPDMA0 + 0x20*BULK_OUT_EP)
};

enum EP_FIFO_ADDR
{
	control_EP_FIFO		= (EP0_FIFO + 0x1000*CONTROL_EP),
	bulkIn_EP_FIFO		= (EP0_FIFO + 0x1000*BULK_IN_EP),
	bulkOut_EP_FIFO		= (EP0_FIFO + 0x1000*BULK_OUT_EP)
};


//==========================
// OTG PHY CORE REGISTERS
//==========================
enum OTGPHYC_REG
{
	PHYPWR		= (USBOTG_PHY_BASE+0x00),
	PHYCTRL		= (USBOTG_PHY_BASE+0x04),
	RSTCON		= (USBOTG_PHY_BASE+0x08),
	PHYTUNE		= (USBOTG_PHY_BASE+0x20)
};


//==============================================================================================
// definitions related to Standard Device Requests
enum EP_INDEX
{
	EP0, EP1, EP2, EP3, EP4
};

//------------------------------------------------
// EP0 state
enum EP0_STATE
{
	EP0_STATE_INIT              	= 0,
	EP0_STATE_GD_DEV_0				= 11,
	EP0_STATE_GD_DEV_1				= 12,
	EP0_STATE_GD_DEV_2				= 13,
	EP0_STATE_GD_CFG_0          	= 21,
	EP0_STATE_GD_CFG_1          	= 22,
	EP0_STATE_GD_CFG_2          	= 23,
	EP0_STATE_GD_CFG_3          	= 24,
	EP0_STATE_GD_CFG_4          	= 25,
	EP0_STATE_CONFIGURATION_GET		= 26,
	EP0_STATE_GD_STR_I0         	= 30,
	EP0_STATE_GD_STR_I1         	= 31,
	EP0_STATE_GD_STR_I2         	= 32,
	EP0_STATE_GD_DEV_QUALIFIER  	= 33,
	EP0_STATE_INTERFACE_GET			= 34,	
	EP0_STATE_GET_STATUS0			= 35,
	EP0_STATE_GET_STATUS1			= 36,
	EP0_STATE_GET_STATUS2			= 37,
	EP0_STATE_GET_STATUS3			= 38,
	EP0_STATE_GET_STATUS4			= 39,
	EP0_STATE_GD_OTHER_SPEED		= 40,
	EP0_STATE_GD_CFG_ONLY_0     	= 41,
	EP0_STATE_GD_CFG_ONLY_1     	= 42,
	EP0_STATE_GD_IF_ONLY_0      	= 44,
	EP0_STATE_GD_IF_ONLY_1      	= 45,
	EP0_STATE_GD_EP0_ONLY_0     	= 46,
	EP0_STATE_GD_EP1_ONLY_0     	= 47,
	EP0_STATE_GD_EP2_ONLY_0     	= 48,
	EP0_STATE_GD_EP3_ONLY_0     	= 49,
	EP0_STATE_GD_OTHER_SPEED_HIGH_1	= 51,
	EP0_STATE_GD_OTHER_SPEED_HIGH_2	= 52,
	EP0_STATE_GD_OTHER_SPEED_HIGH_3	= 53,
	EP0_STATE_TEST_MODE				= 60
};

// SPEC1.1

// Standard bmRequestType (direction)
// #define DEVICE_bmREQUEST_TYPE(oDeviceRequest)  ((m_poDeviceRequest->bmRequestType) & 0x80)
enum DEV_REQUEST_DIRECTION
{
	HOST_TO_DEVICE				= 0x00,
	DEVICE_TO_HOST				= 0x80
};

// Standard bmRequestType (Type)
// #define DEVICE_bmREQUEST_TYPE(oDeviceRequest)  ((m_poDeviceRequest->bmRequestType) & 0x60)
enum DEV_REQUEST_TYPE
{
	STANDARD_TYPE               = 0x00,
	CLASS_TYPE                  = 0x20,
	VENDOR_TYPE                 = 0x40,
	RESERVED_TYPE               = 0x60
};

// Standard bmRequestType (Recipient)
// #define DEVICE_bmREQUEST_RECIPIENT(oDeviceRequest)  ((m_poDeviceRequest->bmRequestType) & 0x07)
enum DEV_REQUEST_RECIPIENT
{
	DEVICE_RECIPIENT			= 0,
	INTERFACE_RECIPIENT			= 1,
	ENDPOINT_RECIPIENT			= 2,
	OTHER_RECIPIENT				= 3
};

// Standard bRequest codes
enum STANDARD_REQUEST_CODE
{
	STANDARD_GET_STATUS         = 0,
	STANDARD_CLEAR_FEATURE      = 1,
	STANDARD_RESERVED_1         = 2,
	STANDARD_SET_FEATURE        = 3,
	STANDARD_RESERVED_2         = 4,
	STANDARD_SET_ADDRESS        = 5,
	STANDARD_GET_DESCRIPTOR     = 6,
	STANDARD_SET_DESCRIPTOR     = 7,
	STANDARD_GET_CONFIGURATION  = 8,
	STANDARD_SET_CONFIGURATION  = 9,
	STANDARD_GET_INTERFACE      = 10,
	STANDARD_SET_INTERFACE      = 11,
	STANDARD_SYNCH_FRAME        = 12
};


// Descriptor types
enum DESCRIPTOR_TYPE
{
	DEVICE_DESCRIPTOR           = 1,
	CONFIGURATION_DESCRIPTOR    = 2,
	STRING_DESCRIPTOR           = 3,
	INTERFACE_DESCRIPTOR        = 4,
	ENDPOINT_DESCRIPTOR         = 5,
	DEVICE_QUALIFIER            = 6,
	OTHER_SPEED_CONFIGURATION   = 7,
	INTERFACE_POWER				= 8
};

// configuration descriptor: bmAttributes
enum CONFIG_ATTRIBUTES
{
	CONF_ATTR_DEFAULT       	= 0x80, // in Spec 1.0, it was BUSPOWERED bit.
	CONF_ATTR_REMOTE_WAKEUP 	= 0x20,
	CONF_ATTR_SELFPOWERED   	= 0x40
};

// endpoint descriptor
enum ENDPOINT_ATTRIBUTES
{
	EP_ADDR_IN              = 0x80,
	EP_ADDR_OUT             = 0x00,

	EP_ATTR_CONTROL         = 0x0,
	EP_ATTR_ISOCHRONOUS     = 0x1,
	EP_ATTR_BULK            = 0x2,
	EP_ATTR_INTERRUPT       = 0x3
};

// Descriptor size
enum DESCRIPTOR_SIZE
{
	DEVICE_DESC_SIZE            = 18,
	STRING_DESC0_SIZE           = 4,
	STRING_DESC1_SIZE           = 22,
	STRING_DESC2_SIZE           = 44,
	CONFIG_DESC_SIZE            = 9,
	INTERFACE_DESC_SIZE         = 9,
	ENDPOINT_DESC_SIZE          = 7,
	DEVICE_QUALIFIER_SIZE       = 10,
	OTHER_SPEED_CFG_SIZE 		= 9
};
#define CONFIG_DESC_TOTAL_SIZE   	(CONFIG_DESC_SIZE+INTERFACE_DESC_SIZE+ENDPOINT_DESC_SIZE*2)
//32 <cfg desc>+<if desc>+<endp0 desc>+<endp1 desc>


//=====================================================================
//definitions related to CSR setting

// GOTGCTL
#define B_SESSION_VALID				(0x1<<19)
#define A_SESSION_VALID				(0x1<<18)

// GAHBCFG
#define PTXFE_HALF					(0<<8)
#define PTXFE_ZERO					(1<<8)
#define NPTXFE_HALF					(0<<7)
#define NPTXFE_ZERO					(1<<7)
#define MODE_SLAVE					(0<<5)
#define MODE_DMA					(1<<5)
#define BURST_SINGLE				(0<<1)
#define BURST_INCR					(1<<1)
#define BURST_INCR4					(3<<1)
#define BURST_INCR8					(5<<1)
#define BURST_INCR16				(7<<1)
#define GBL_INT_UNMASK				(1<<0)
#define GBL_INT_MASK				(0<<0)

// GRSTCTL
#define AHB_MASTER_IDLE				(1u<<31)
#define CORE_SOFT_RESET				(0x1<<0)

// GINTSTS/GINTMSK core interrupt register
#define INT_RESUME              	(1u<<31)
#define INT_DISCONN              	(0x1<<29)
#define INT_CONN_ID_STS_CNG			(0x1<<28)
#define INT_OUT_EP					(0x1<<19)
#define INT_IN_EP					(0x1<<18)
#define INT_ENUMDONE				(0x1<<13)
#define INT_RESET               	(0x1<<12)
#define INT_SUSPEND             	(0x1<<11)
#define INT_TX_FIFO_EMPTY			(0x1<<5)
#define INT_RX_FIFO_NOT_EMPTY		(0x1<<4)
#define INT_SOF						(0x1<<3)
#define INT_DEV_MODE				(0x0<<0)
#define INT_HOST_MODE				(0x1<<1)

// GRXSTSP STATUS
#define OUT_PKT_RECEIVED			(0x2<<17)
#define SETUP_PKT_RECEIVED			(0x6<<17)

// DCTL device control register
#define NORMAL_OPERATION			(0x1<<0)
#define SOFT_DISCONNECT				(0x1<<1)
#define	TEST_J_MODE					(TEST_J<<4)
#define	TEST_K_MODE					(TEST_K<<4)
#define	TEST_SE0_NAK_MODE			(TEST_SE0_NAK<<4)
#define	TEST_PACKET_MODE			(TEST_PACKET<<4)
#define	TEST_FORCE_ENABLE_MODE		(TEST_FORCE_ENABLE<<4)
#define TEST_CONTROL_FIELD			(0x7<<4)

// DAINT device all endpoint interrupt register
#define INT_IN_EP0					(0x1<<0)
#define INT_IN_EP1					(0x1<<1)
#define INT_IN_EP3					(0x1<<3)
#define INT_OUT_EP0					(0x1<<16)
#define INT_OUT_EP2					(0x1<<18)
#define INT_OUT_EP4					(0x1<<20)

// DIEPCTL0/DOEPCTL0 device control IN/OUT endpoint 0 control register
#define DEPCTL_EPENA				(0x1<<31)
#define DEPCTL_EPDIS				(0x1<<30)
#define DEPCTL_SNAK					(0x1<<27)
#define DEPCTL_CNAK					(0x1<<26)
#define DEPCTL_CTRL_TYPE			(EP_TYPE_CONTROL<<18)
#define DEPCTL_ISO_TYPE				(EP_TYPE_ISOCHRONOUS<<18)
#define DEPCTL_BULK_TYPE			(EP_TYPE_BULK<<18)
#define DEPCTL_INTR_TYPE			(EP_TYPE_INTERRUPT<<18)
#define DEPCTL_USBACTEP				(0x1<<15)
#define DEPCTL0_MPS_64				(0x0<<0)
#define DEPCTL0_MPS_32				(0x1<<0)
#define DEPCTL0_MPS_16				(0x2<<0)
#define DEPCTL0_MPS_8				(0x3<<0)

// DIEPCTLn/DOEPCTLn device control IN/OUT endpoint n control register

// DIEPMSK/DOEPMSK device IN/OUT endpoint common interrupt mask register
// DIEPINTn/DOEPINTn device IN/OUT endpoint interrupt register
#define BACK2BACK_SETUP_RECEIVED  		(0x1<<6)
#define INTKN_TXFEMP					(0x1<<4)
#define NON_ISO_IN_EP_TIMEOUT			(0x1<<3)
#define CTRL_OUT_EP_SETUP_PHASE_DONE	(0x1<<3)
#define AHB_ERROR						(0x1<<2)
#define TRANSFER_DONE					(0x1<<0)

 

//=====================================================================
// setting the device qualifier descriptor and a string descriptor
const u8 aDeviceQualifierDescriptor[] =
{
	0x0a,                   //  0 desc size
	0x06,                   //  1 desc type (DEVICE_QUALIFIER)
	0x00,                   //  2 USB release
	0x02,                   //  3 => 2.00
	0xFF,                   //  4 class
	0x00,                   //  5 subclass
	0x00,                   //  6 protocol
	64,          			//  7 max pack size
	0x01,                   //  8 number of other-speed configuration
	0x00,                   //  9 reserved
};

const u8 aOtherSpeedConfiguration_full[] =
{
	0x09,                   //  0 desc size
	0x07,                   //  1 desc type (other speed)
	0x20,                   //  2 Total length of data returned
	0x00,                   //  3 
	0x01,                   //  4 Number of interfaces supported by this speed configuration
	0x01,                   //  5 value to use to select configuration
	0x00,                   //  6 index of string desc
	CONF_ATTR_DEFAULT|CONF_ATTR_SELFPOWERED,   //  7 same as configuration desc
	0x19,                   //  8 same as configuration desc
	
};

const u8 aOtherSpeedConfiguration_fullTotal[] =
{
  0x09, 0x07 ,0x20 ,0x00 ,0x01 ,0x01 ,0x00 ,0xC0 ,0x19,
  0x09 ,0x04 ,0x00 ,0x00 ,0x02 ,0xff ,0x00 ,0x00 ,0x00,
  0x07 ,0x05 ,0x83 ,0x02 ,0x40 ,0x00 ,0x00,
  0x07 ,0x05 ,0x04 ,0x02 ,0x40 ,0x00 ,0x00
};

const u8 aOtherSpeedConfiguration_high[] =
{
	0x09,                   //  0 desc size
	0x07,                   //  1 desc type (other speed)
	0x20,                   //  2 Total length of data returned
	0x00,                   //  3 
	0x01,                   //  4 Number of interfaces supported by this speed configuration
	0x01,                   //  5 value to use to select configuration
	0x00,                   //  6 index of string desc
	CONF_ATTR_DEFAULT|CONF_ATTR_SELFPOWERED,   //  7 same as configuration desc
	0x19,                   //  8 same as configuration desc
	
};

const u8 aOtherSpeedConfiguration_highTotal[] =
{
  0x09, 0x07 ,0x20 ,0x00 ,0x01 ,0x01 ,0x00 ,0xC0 ,0x19,
  0x09 ,0x04 ,0x00 ,0x00 ,0x02 ,0xff ,0x00 ,0x00 ,0x00,
  0x07 ,0x05 ,0x81 ,0x02 ,0x00 ,0x02 ,0x00,
  0x07 ,0x05 ,0x02 ,0x02 ,0x00 ,0x02 ,0x00
};

const u8 aDescStr0[]=
{
	4, STRING_DESCRIPTOR, LANGID_US_L, LANGID_US_H, // codes representing languages
};

const u8 aDescStr1[]= // Manufacturer
{
	(0x14+2), STRING_DESCRIPTOR,
	'S', 0x0, 'y', 0x0, 's', 0x0, 't', 0x0, 'e', 0x0, 'm', 0x0, ' ', 0x0, 'M', 0x0,
	'C', 0x0, 'U', 0x0,
};

const u8 aDescStr2[]= // Product
{
	(0x2a+2), STRING_DESCRIPTOR,
	'S', 0x0, 'E', 0x0, 'C', 0x0, ' ', 0x0, 'S', 0x0, '3', 0x0, 'C', 0x0, '6', 0x0,
	'4', 0x0, '1', 0x0, '0', 0x0, 'X', 0x0, ' ', 0x0, 'T', 0x0, 'e', 0x0, 's', 0x0,
	't', 0x0, ' ', 0x0, 'B', 0x0, '/', 0x0, 'D', 0x0
};


//=====================================================================
// global varibles used in several functions
OTGDEV oOtgDev;
USB_GET_STATUS oStatusGet;
USB_INTERFACE_GET oInterfaceGet;

u16 g_usConfig;
u16 g_usUploadPktLength=0;
u8 g_bTransferEp0 = false;

//////////
// Function Name : OTGDEV_InitOtg
// Function Desctiption : This function initializes OTG PHY and LINK.
// Input : eSpeed, USB Speed (high or full)
// Output : NONE
// Version :
void OTGDEV_InitOtg(USB_SPEED eSpeed)
{
	u8 ucMode;
	
	Outp32SYSC(0x804,Inp32SYSC(0x804)&~(1<<17));	//enable OTG clock pad
	Outp32SYSC(0x900,Inp32SYSC(0x900)|(1<<16));	//unmask usb signal
	 
	oOtgDev.m_eSpeed = eSpeed;
	oOtgDev.m_uIsUsbOtgSetConfiguration = 0;
	oOtgDev.m_uEp0State = EP0_STATE_INIT;
	oOtgDev.m_uEp0SubState = 0;
	OTGDEV_InitPhyCon();
	OTGDEV_SoftResetCore();
	OTGDEV_WaitCableInsertion();
	OTGDEV_InitCore();
	OTGDEV_CheckCurrentMode(&ucMode);
	if (ucMode == INT_DEV_MODE)
	{
		OTGDEV_SetSoftDisconnect();
		Delay(10);
		OTGDEV_ClearSoftDisconnect();
		OTGDEV_InitDevice();
	}
	else
	{
		UART_Printf("Error : Current Mode is Host\n");
		return;
	}
}

//////////
// Function Name : OTGDEV_HandleEvent
// Function Desctiption : This function handles various OTG interrupts of Device mode.
// Input : NONE
// Output : NONE
// Version :
void OTGDEV_HandleEvent(void)
{
	u32 uGIntStatus, uDStatus;
	u32 ep_int_status, ep_int;

	uGIntStatus = Inp32(GINTSTS); // System status read
	Outp32(GINTSTS, uGIntStatus); // Interrupt Clear		
	DbgUsb0("GINTSTS : %x \n", uGIntStatus);

	if (uGIntStatus & INT_RESET) // Reset interrupt
	{
		Outp32(DCTL,Inp32(DCTL) & ~(TEST_CONTROL_FIELD));
		OTGDEV_SetAllOutEpNak();
		oOtgDev.m_uEp0State = EP0_STATE_INIT;
		Outp32(DAINTMSK,((1<<BULK_OUT_EP)|(1<<CONTROL_EP))<<16|((1<<BULK_IN_EP)|(1<<CONTROL_EP)));
		Outp32(DOEPMSK, CTRL_OUT_EP_SETUP_PHASE_DONE|AHB_ERROR|TRANSFER_DONE);
		Outp32(DIEPMSK, INTKN_TXFEMP|NON_ISO_IN_EP_TIMEOUT|AHB_ERROR|TRANSFER_DONE);
		Outp32(GRXFSIZ, RX_FIFO_SIZE);					// Rx FIFO Size
		Outp32(GNPTXFSIZ, NPTX_FIFO_SIZE<<16| NPTX_FIFO_START_ADDR<<0);	// Non Periodic Tx FIFO Size		
		OTGDEV_ClearAllOutEpNak();
		Outp32(DCFG, Inp32(DCFG)&~(0x7f<<4));	//clear device address
		if(g_bSuspendResume==true)
		{
			UART_Printf("\n [USB_Diag_Log]  : Reset Mode\n");
		}
		else
		{
			DbgUsb("\n [USB_Diag_Log]  : Reset Mode\n");
		}
		
		Outp32(PCGCCTRL, Inp32(PCGCCTRL)&~(1<<0));	//start pclk
		DbgUsb("rOTHERS=0x%08x\n",Inp32SYSC(0x900));
	}

	if (uGIntStatus & INT_ENUMDONE) // Device Speed Detection interrupt
	{
		DbgUsb("\n [USB_Diag_Log]  : Speed Detection interrupt \n");
		
		uDStatus = Inp32(DSTS); // System status read

		if (((uDStatus&0x6) >>1) == USB_HIGH) 			// Set if Device is High speed or Full speed
		{
			DbgUsb("\n [USB_Diag_Log]  : High Speed Detection\n");
			OTGDEV_SetMaxPktSizes(USB_HIGH);
		}
		else if(((uDStatus&0x6) >>1) == USB_FULL)
		{
			DbgUsb("\n [USB_Diag_Log]  : full Speed Detection\n");
			OTGDEV_SetMaxPktSizes(USB_FULL);
		}
		else
		{
			UART_Printf("\n Error:Neither High_Speed nor Full_Speed\n");
			return;
		}
		
		OTGDEV_SetEndpoint();
		OTGDEV_SetDescriptorTable();
	}
	
	if (uGIntStatus & INT_RESUME)
	{
		if(g_bSuspendResume==true)
		{
			UART_Printf("\n [USB_Diag_Log]  : Resume Mode \n");
		}
		else
		{
			DbgUsb("\n [USB_Diag_Log]  : Resume Mode \n");
		}
		
		Outp32(PCGCCTRL, Inp32(PCGCCTRL)&~(1<<0));	//start pclk
	}

	if (uGIntStatus & INT_SUSPEND)
	{
		if(g_bSuspendResume==true)
		{
			UART_Printf("\n [USB_Diag_Log]  : Suspend Mode\n");
		}
		else
		{
			DbgUsb("\n [USB_Diag_Log]  : Suspend Mode\n");
		}
		
		Outp32(PCGCCTRL, Inp32(PCGCCTRL)|(1<<0));	//stop pclk
		DbgUsb("rOTHERS=0x%08x\n",Inp32SYSC(0x900));
	}

	if(uGIntStatus & INT_RX_FIFO_NOT_EMPTY)
	{
		u32 GrxStatus;
		u32 fifoCntByte;

		DbgUsb("INT_RX_FIFO_NOT_EMPTY\n");
		Outp32(GINTMSK,  INT_RESUME|INT_OUT_EP|INT_IN_EP|INT_ENUMDONE|INT_RESET|INT_SUSPEND);
		
		GrxStatus = Inp32(GRXSTSP);
		DbgUsb("GRXSTSP : %x \n", GrxStatus);
	
		if ((GrxStatus & SETUP_PKT_RECEIVED) == SETUP_PKT_RECEIVED) 
		{
			DbgUsb("SETUP_PACKET_RECEIVED\n");
			OTGDEV_HandleEvent_EP0();
			g_bTransferEp0=true;
		}
		else if ((GrxStatus & OUT_PKT_RECEIVED) == OUT_PKT_RECEIVED)
		{
			fifoCntByte = (GrxStatus & 0x7ff0)>>4;
			if(((GrxStatus&0xf)==BULK_OUT_EP)&&(fifoCntByte))
			{				
				DbgUsb("Bulk Out : OUT_PKT_RECEIVED\n");
				OTGDEV_HandleEvent_BulkOut(fifoCntByte);
				if( oOtgDev.m_eOpMode == USB_CPU )
					Outp32(GINTMSK, INT_RESUME|INT_OUT_EP|INT_IN_EP|INT_ENUMDONE|INT_RESET|INT_SUSPEND|INT_RX_FIFO_NOT_EMPTY);
				return;
			}
		}
		Outp32(GINTMSK, INT_RESUME|INT_OUT_EP|INT_IN_EP|INT_ENUMDONE|INT_RESET
			|INT_SUSPEND|INT_RX_FIFO_NOT_EMPTY); //gint unmask
	}

	if ((uGIntStatus & INT_IN_EP) || (uGIntStatus & INT_OUT_EP))
	{
		u32 uDmaEnCheck;
		s32 uRemainCnt; 
		
		ep_int = Inp32(DAINT);
		DbgUsb("DAINT : %x \n", ep_int);
		
		if (ep_int & (1<<CONTROL_EP))
		{
			ep_int_status = Inp32(DIEPINT0);
			DbgUsb("DIEPINT0 : %x \n", ep_int_status);
			
			if (ep_int_status & INTKN_TXFEMP)
			{
				if (g_bTransferEp0==true)
				{
					OTGDEV_TransferEp0();
					g_bTransferEp0 = false;
				}
			}
			
			Outp32(DIEPINT0, ep_int_status); // Interrupt Clear
		}
		
		if (ep_int & ((1<<CONTROL_EP)<<16))
		{
			ep_int_status = Inp32(DOEPINT0);
			DbgUsb("DOEPINT0 : %x \n", ep_int_status);
			
			OTGDEV_SetOutEpXferSize(EP_TYPE_CONTROL, 1, oOtgDev.m_uControlEPMaxPktSize);
			Outp32(DOEPCTL0, 1u<<31|1<<26);		//ep0 enable, clear nak
			
			Outp32(DOEPINT0, ep_int_status); 		// Interrupt Clear
		}

		if(ep_int & (1<<BULK_IN_EP))
		{
			ep_int_status = Inp32(bulkIn_DIEPINT);
 			DbgUsb0("bulkIn_DIEPINT : %x \n", ep_int_status);
			Outp32(bulkIn_DIEPINT, ep_int_status); // Interrupt Clear	
			
			uDmaEnCheck = Inp32(GAHBCFG);
			
			if (oOtgDev.m_eOpMode == USB_CPU)
			{
				if (ep_int_status&TRANSFER_DONE)
				{
					Outp32(bulkIn_DIEPINT, TRANSFER_DONE); // Interrupt Clear
					
					oOtgDev.m_pUpPt += g_usUploadPktLength;
					OTGDEV_HandleEvent_BulkIn();
				}
			}
			
			else if ((uDmaEnCheck&MODE_DMA)&&(ep_int_status&TRANSFER_DONE))
			{
				DbgUsb("DMA IN : Transfer Done\n");

				oOtgDev.m_pUpPt = (u8 *)Inp32(bulkIn_DIEPDMA);
				uRemainCnt = oOtgDev.m_uUploadSize- ((u32)oOtgDev.m_pUpPt - oOtgDev.m_uUploadAddr);
				
				if (uRemainCnt>0) 
				{
					u32 uPktCnt, uRemainder;
					uPktCnt = (u32)(uRemainCnt/oOtgDev.m_uBulkInEPMaxPktSize);
					uRemainder = (u32)(uRemainCnt%oOtgDev.m_uBulkInEPMaxPktSize);
					if(uRemainder != 0)
					{
						uPktCnt += 1;
					}
					DbgUsb("uRemainCnt : %d \n", uRemainCnt);
					if (uPktCnt> 1023)
					{
						OTGDEV_SetInEpXferSize(EP_TYPE_BULK, 1023, (oOtgDev.m_uBulkInEPMaxPktSize*1023));
					}
					else
					{
						OTGDEV_SetInEpXferSize(EP_TYPE_BULK, uPktCnt, uRemainCnt);
					}
					Outp32(bulkIn_DIEPCTL, 1u<<31|1<<26|2<<18|1<<15|BULK_IN_EP<<11|oOtgDev.m_uBulkInEPMaxPktSize<<0);		//ep1 enable, clear nak, bulk, usb active, next ep1, max pkt
				}
				else
				{
					DbgUsb("DMA IN : Transfer Complete\n");
				}
			}
		}

		if (ep_int & ((1<<BULK_OUT_EP)<<16))
		{
			ep_int_status = Inp32(bulkOut_DOEPINT);
			Outp32(bulkOut_DOEPINT, ep_int_status); 		// Interrupt Clear
			DbgUsb0("bulkOut_DOEPINT : %x \n", ep_int_status);
			uDmaEnCheck = Inp32(GAHBCFG);
			if ((uDmaEnCheck&MODE_DMA)&&(ep_int_status&TRANSFER_DONE))
			{
				DbgUsb("DMA OUT : Transfer Done\n");
				oOtgDev.m_pDownPt = (u8 *)Inp32(bulkOut_DOEPDMA);

				uRemainCnt = oOtgDev.m_uDownloadFileSize - ((u32)oOtgDev.m_pDownPt - oOtgDev.m_uDownloadAddress + 8);
				
				if (uRemainCnt>0) 
				{
					u32 uPktCnt, uRemainder;
					uPktCnt = (u32)(uRemainCnt/oOtgDev.m_uBulkOutEPMaxPktSize);
					uRemainder = (u32)(uRemainCnt%oOtgDev.m_uBulkOutEPMaxPktSize);
					if(uRemainder != 0)
					{
						uPktCnt += 1;
					}
					DbgUsb("uRemainCnt : %d \n", uRemainCnt);
					if (uPktCnt> 1023)
					{
						OTGDEV_SetOutEpXferSize(EP_TYPE_BULK, 1023, (oOtgDev.m_uBulkOutEPMaxPktSize*1023));
					}
					else
					{
						OTGDEV_SetOutEpXferSize(EP_TYPE_BULK, uPktCnt, uRemainCnt);
					}
					Outp32(bulkOut_DOEPCTL, 1u<<31|1<<26|2<<18|1<<15|oOtgDev.m_uBulkOutEPMaxPktSize<<0);		//ep3 enable, clear nak, bulk, usb active, next ep3, max pkt 64
				}
				else
				{
					DbgUsb("DMA OUT : Transfer Complete\n");
				}
			}
		}
	}
}

//////////
// Function Name : OTGDEV_HandleEvent_EP0
// Function Desctiption : This function is called when Setup packet is received.
// Input : NONE
// Output : NONE
// Version :
#define TEST_PKT_SIZE 53

u8 TestPkt [TEST_PKT_SIZE] = {
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,					//JKJKJKJK x 9
    0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,						//JJKKJJKK x 8
    0xEE,0xEE,0xEE,0xEE,0xEE,0xEE,0xEE,0xEE,						//JJJJKKKK x 8
    0xFE,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,	//JJJJJJJKKKKKKK x8 - '1'
    0x7F,0xBF,0xDF,0xEF,0xF7,0xFB,0xFD,								//'1' + JJJJJJJK x 8
    0xFC,0x7E,0xBF,0xDF,0xEF,0xF7,0xFB,0xFD,0x7E					//{JKKKKKKK x 10},JK
};
void OTGDEV_HandleEvent_EP0(void)
{
	u16 i;
	u32 ReadBuf[64]={0x0000, };
	u16 setaddress;
	u32 uRemoteWakeUp=false;
	
	for(i=0;i<2;i++)
	{
		ReadBuf[i] = Inp32(EP0_FIFO);
	}

	oOtgDev.m_oDeviceRequest.bmRequestType=ReadBuf[0];
	oOtgDev.m_oDeviceRequest.bRequest=ReadBuf[0]>>8;
	oOtgDev.m_oDeviceRequest.wValue_L=ReadBuf[0]>>16;
	oOtgDev.m_oDeviceRequest.wValue_H=ReadBuf[0]>>24;
	oOtgDev.m_oDeviceRequest.wIndex_L=ReadBuf[1];
	oOtgDev.m_oDeviceRequest.wIndex_H=ReadBuf[1]>>8;
	oOtgDev.m_oDeviceRequest.wLength_L=ReadBuf[1]>>16;
	oOtgDev.m_oDeviceRequest.wLength_H=ReadBuf[1]>>24;

	OTGDEV_PrintEp0Pkt((u8 *)&oOtgDev.m_oDeviceRequest, 8);

	switch (oOtgDev.m_oDeviceRequest.bRequest)
	{
		case STANDARD_SET_ADDRESS:
			setaddress = (oOtgDev.m_oDeviceRequest.wValue_L); // Set Address Update bit
			Outp32(DCFG, 1<<18|setaddress<<4|oOtgDev.m_eSpeed<<0);
			DbgUsb("\n MCU >> Set Address : %d \n", setaddress);
			oOtgDev.m_uEp0State = EP0_STATE_INIT;
			break;

		case STANDARD_SET_DESCRIPTOR:
			DbgUsb("\n MCU >> Set Descriptor \n");
			break;

		case STANDARD_SET_CONFIGURATION:
			DbgUsb("\n MCU >> Set Configuration \n");
			g_usConfig = oOtgDev.m_oDeviceRequest.wValue_L; // Configuration value in configuration descriptor
			oOtgDev.m_uIsUsbOtgSetConfiguration = 1;
			oOtgDev.m_uEp0State = EP0_STATE_INIT;
			break;

		case STANDARD_GET_CONFIGURATION:
			oOtgDev.m_uEp0State = EP0_STATE_CONFIGURATION_GET;
			break;

		case STANDARD_GET_DESCRIPTOR:
			switch (oOtgDev.m_oDeviceRequest.wValue_H)
			{
				case DEVICE_DESCRIPTOR:
					oOtgDev.m_uDeviceRequestLength = (u32)((oOtgDev.m_oDeviceRequest.wLength_H << 8) |
						oOtgDev.m_oDeviceRequest.wLength_L);
					DbgUsb("\n MCU >> Get Device Descriptor = 0x%x \n",oOtgDev.m_uDeviceRequestLength);
					oOtgDev.m_uEp0State = EP0_STATE_GD_DEV_0;
					break;

				case CONFIGURATION_DESCRIPTOR:
					oOtgDev.m_uDeviceRequestLength = (u32)((oOtgDev.m_oDeviceRequest.wLength_H << 8) |
						oOtgDev.m_oDeviceRequest.wLength_L);
					DbgUsb("\n MCU >> Get Configuration Descriptor = 0x%x \n",oOtgDev.m_uDeviceRequestLength);

					if (oOtgDev.m_uDeviceRequestLength > CONFIG_DESC_SIZE){
					// === GET_DESCRIPTOR:CONFIGURATION+INTERFACE+ENDPOINT0+ENDPOINT1 ===
					// Windows98 gets these 4 descriptors all together by issuing only a request.
					// Windows2000 gets each descriptor seperately.
					// oOtgDev.m_uEpZeroTransferLength = CONFIG_DESC_TOTAL_SIZE;
						oOtgDev.m_uEp0State = EP0_STATE_GD_CFG_0;
					}
					else
						oOtgDev.m_uEp0State = EP0_STATE_GD_CFG_ONLY_0; // for win2k
					break;

				case STRING_DESCRIPTOR :
					DbgUsb("\n MCU >> Get String Descriptor \n");
					switch(oOtgDev.m_oDeviceRequest.wValue_L)
					{
						case 0:
							oOtgDev.m_uEp0State = EP0_STATE_GD_STR_I0;
							break;
						case 1:
							oOtgDev.m_uEp0State = EP0_STATE_GD_STR_I1;
							break;
						case 2:
							oOtgDev.m_uEp0State = EP0_STATE_GD_STR_I2;
							break;
						default:
								break;
					}
					break;
				case ENDPOINT_DESCRIPTOR:
					DbgUsb("\n MCU >> Get Endpoint Descriptor \n");
					switch(oOtgDev.m_oDeviceRequest.wValue_L&0xf)
					{
					case 0:
						oOtgDev.m_uEp0State=EP0_STATE_GD_EP0_ONLY_0;
						break;
					case 1:
						oOtgDev.m_uEp0State=EP0_STATE_GD_EP1_ONLY_0;
						break;
					default:
						break;
					}
					break;

				case DEVICE_QUALIFIER:
					oOtgDev.m_uDeviceRequestLength = (u32)((oOtgDev.m_oDeviceRequest.wLength_H << 8) |
						oOtgDev.m_oDeviceRequest.wLength_L);
					DbgUsb("\n MCU >> Get Device Qualifier Descriptor = 0x%x \n",oOtgDev.m_uDeviceRequestLength);
					oOtgDev.m_uEp0State = EP0_STATE_GD_DEV_QUALIFIER;
					break;
					
			   	case OTHER_SPEED_CONFIGURATION :
					DbgUsb(("\n MCU >> Get OTHER_SPEED_CONFIGURATION \n"));
					oOtgDev.m_uDeviceRequestLength = (u32)((oOtgDev.m_oDeviceRequest.wLength_H << 8) |
						oOtgDev.m_oDeviceRequest.wLength_L);
					oOtgDev.m_uEp0State = EP0_STATE_GD_OTHER_SPEED;
					break;
					
			}
			break;

		case STANDARD_CLEAR_FEATURE:
			DbgUsb("\n MCU >> Clear Feature \n");
			switch (oOtgDev.m_oDeviceRequest.bmRequestType)
			{
				case DEVICE_RECIPIENT:
					if (oOtgDev.m_oDeviceRequest.wValue_L == 1)
						uRemoteWakeUp = false;
					break;

				case ENDPOINT_RECIPIENT:
					if (oOtgDev.m_oDeviceRequest.wValue_L == 0)
					{
						if ((oOtgDev.m_oDeviceRequest.wIndex_L & 0x7f) == CONTROL_EP)
							oStatusGet.EndpointCtrl= 0;

						if ((oOtgDev.m_oDeviceRequest.wIndex_L & 0x7f) == BULK_IN_EP) // IN  Endpoint
							oStatusGet.EndpointIn= 0;

						if ((oOtgDev.m_oDeviceRequest.wIndex_L & 0x7f) == BULK_OUT_EP) // OUT Endpoint
							oStatusGet.EndpointOut= 0;
					}
					break;

				default:
					break;
			}
			oOtgDev.m_uEp0State = EP0_STATE_INIT;
			break;

		case STANDARD_SET_FEATURE:
			DbgUsb("\n MCU >> Set Feature \n");
			switch (oOtgDev.m_oDeviceRequest.bmRequestType)
			{
				case DEVICE_RECIPIENT:
					if (oOtgDev.m_oDeviceRequest.wValue_L == 1)
						uRemoteWakeUp = true;
						break;

				case ENDPOINT_RECIPIENT:
					if (oOtgDev.m_oDeviceRequest.wValue_L == 0)
					{
						if ((oOtgDev.m_oDeviceRequest.wIndex_L & 0x7f) == CONTROL_EP)
							oStatusGet.EndpointCtrl= 1;

						if ((oOtgDev.m_oDeviceRequest.wIndex_L & 0x7f) == BULK_IN_EP)
							oStatusGet.EndpointIn= 1;

						if ((oOtgDev.m_oDeviceRequest.wIndex_L & 0x7f) == BULK_OUT_EP)
							oStatusGet.EndpointOut= 1;
					}
					break;

				default:
					break;
			}

			//=======================================================
			
			switch (oOtgDev.m_oDeviceRequest.wValue_L) {

				case EP_STALL:
					// TBD: additional processing if required
					break;

				
				case TEST_MODE:						
					if ((0 != oOtgDev.m_oDeviceRequest.wIndex_L ) ||(0 != oOtgDev.m_oDeviceRequest.bmRequestType)) 
						break;

					oOtgDev.m_uEp0State = EP0_STATE_TEST_MODE;

					break;

				default:
					oOtgDev.m_uEp0State = EP0_STATE_INIT;
					break;
			}
			//=======================================================
			break;

		case STANDARD_GET_STATUS:
			switch(oOtgDev.m_oDeviceRequest.bmRequestType)
			{
				case  (0x80):	//device
					oStatusGet.Device=((u8)uRemoteWakeUp<<1)|0x1;		// SelfPowered
					oOtgDev.m_uEp0State = EP0_STATE_GET_STATUS0;
					break;

				case  (0x81):	//interface
					oStatusGet.Interface=0;
					oOtgDev.m_uEp0State = EP0_STATE_GET_STATUS1;
					break;

				case  (0x82):	//endpoint
					if ((oOtgDev.m_oDeviceRequest.wIndex_L & 0x7f) == CONTROL_EP)
						oOtgDev.m_uEp0State = EP0_STATE_GET_STATUS2;

					if ((oOtgDev.m_oDeviceRequest.wIndex_L & 0x7f) == BULK_IN_EP)
						oOtgDev.m_uEp0State = EP0_STATE_GET_STATUS3;

					if ((oOtgDev.m_oDeviceRequest.wIndex_L & 0x7f) == BULK_OUT_EP)
						oOtgDev.m_uEp0State = EP0_STATE_GET_STATUS4;
					break;

				default:
					break;
			}
			break;

		case STANDARD_GET_INTERFACE:
			oOtgDev.m_uEp0State = EP0_STATE_INTERFACE_GET;
			break;

		case STANDARD_SET_INTERFACE:
			oInterfaceGet.AlternateSetting= oOtgDev.m_oDeviceRequest.wValue_L;
			oOtgDev.m_uEp0State = EP0_STATE_INIT;
			break;

		case STANDARD_SYNCH_FRAME:
			oOtgDev.m_uEp0State = EP0_STATE_INIT;
			break;

		default:
			break;
	}
	
	OTGDEV_SetInEpXferSize(EP_TYPE_CONTROL, 1, oOtgDev.m_uControlEPMaxPktSize);
	
	Outp32(DIEPCTL0, ((1<<26)|(CONTROL_EP<<11)|(0<<0)));	//clear nak, next ep0, 64byte
		
}

//////////
// Function Name : OTGDEV_TransferEp0
// Function Desctiption : This function is called during data phase of control transfer.
// Input : NONE
// Output : NONE
// Version :
void OTGDEV_TransferEp0(void)
{

	DbgUsb("TransferEp0 , oOtgDev.m_uEp0State == %d\n", oOtgDev.m_uEp0State);

	switch (oOtgDev.m_uEp0State)
	{
		case EP0_STATE_INIT:
			OTGDEV_SetInEpXferSize(EP_TYPE_CONTROL, 1, 0);
			Outp32(DIEPCTL0, (1u<<31)|(1<<26)|(BULK_IN_EP<<11)|(0<<0));	//ep0 enable, clear nak, next ep0, 64byte
			DbgUsb("EndpointZeroTransfer(EP0_STATE_INIT)\n");
			break;

		// === GET_DESCRIPTOR:DEVICE ===
		case EP0_STATE_GD_DEV_0:
			if (oOtgDev.m_uDeviceRequestLength<=DEVICE_DESC_SIZE)
			{
				OTGDEV_SetInEpXferSize(EP_TYPE_CONTROL, 1, oOtgDev.m_uDeviceRequestLength);
				Outp32(DIEPCTL0, (1u<<31)|(1<<26)|(CONTROL_EP<<11)|(0<<0));	//ep0 enable, clear nak, next ep0, max 64byte
				OTGDEV_WrPktEp0(((u8 *)&(oOtgDev.m_oDesc.oDescDevice))+0, oOtgDev.m_uDeviceRequestLength); // EP0_PKT_SIZE
			}
			else
			{
				OTGDEV_SetInEpXferSize(EP_TYPE_CONTROL, 1, DEVICE_DESC_SIZE);
				Outp32(DIEPCTL0, (1u<<31)|(1<<26)|(CONTROL_EP<<11)|(0<<0));	//ep0 enable, clear nak, next ep0, max 64byte
				OTGDEV_WrPktEp0(((u8 *)&(oOtgDev.m_oDesc.oDescDevice))+0, DEVICE_DESC_SIZE); // EP0_PKT_SIZE
			}
			oOtgDev.m_uEp0State = EP0_STATE_INIT;
			DbgUsb("EndpointZeroTransfer(EP0_STATE_GD_DEV_0)\n");
			break;

		// === GET_DESCRIPTOR:CONFIGURATION+INTERFACE+ENDPOINT0+ENDPOINT1 ===
		// Windows98 gets these 4 descriptors all together by issuing only a request.
		// Windows2000 gets each descriptor seperately.
		// === GET_DESCRIPTOR:CONFIGURATION ONLY for WIN2K===
		case EP0_STATE_GD_CFG_0:
			if(oOtgDev.m_uDeviceRequestLength<=CONFIG_DESC_TOTAL_SIZE)
			{
				OTGDEV_SetInEpXferSize(EP_TYPE_CONTROL, 1, oOtgDev.m_uDeviceRequestLength);
				Outp32(DIEPCTL0, (1u<<31)|(1<<26)|(CONTROL_EP<<11)|(0<<0));	//ep0 enable, clear nak, next ep0, max 64byte
				OTGDEV_WrPktEp0(((u8 *)&(oOtgDev.m_oDesc.oDescConfig))+0, oOtgDev.m_uDeviceRequestLength); // EP0_PKT_SIZE
			}
			else
			{
				OTGDEV_SetInEpXferSize(EP_TYPE_CONTROL, 1, CONFIG_DESC_TOTAL_SIZE);
				Outp32(DIEPCTL0, (1u<<31)|(1<<26)|(CONTROL_EP<<11)|(0<<0));	//ep0 enable, clear nak, next ep0, max 64byte
				OTGDEV_WrPktEp0(((u8 *)&(oOtgDev.m_oDesc.oDescConfig))+0, CONFIG_DESC_TOTAL_SIZE); // EP0_PKT_SIZE
			}
			oOtgDev.m_uEp0State = EP0_STATE_INIT;
			DbgUsb("EndpointZeroTransfer(EP0_STATE_GD_CFG)\n");
			break;

		case EP0_STATE_GD_DEV_QUALIFIER:	//only supported in USB 2.0
			if(oOtgDev.m_uDeviceRequestLength<=10)
			{
				OTGDEV_SetInEpXferSize(EP_TYPE_CONTROL, 1, oOtgDev.m_uDeviceRequestLength);
				Outp32(DIEPCTL0, (1u<<31)|(1<<26)|(CONTROL_EP<<11)|(0<<0));	//ep0 enable, clear nak, next ep0, 64byte
				OTGDEV_WrPktEp0((u8 *)aDeviceQualifierDescriptor+0, oOtgDev.m_uDeviceRequestLength);
			}
			else
			{
				OTGDEV_SetInEpXferSize(EP_TYPE_CONTROL, 1, 10);
				Outp32(DIEPCTL0, (1u<<31)|(1<<26)|(CONTROL_EP<<11)|(0<<0));	//ep0 enable, clear nak, next ep0, 64byte
				OTGDEV_WrPktEp0((u8 *)aDeviceQualifierDescriptor+0, 10);
			}
			oOtgDev.m_uEp0State = EP0_STATE_INIT;
			break;		

        case EP0_STATE_GD_OTHER_SPEED:
			OTEDEV_SetOtherSpeedConfDescTable(oOtgDev.m_uDeviceRequestLength);
			break;	

		// === GET_DESCRIPTOR:CONFIGURATION ONLY===
		case EP0_STATE_GD_CFG_ONLY_0:
			DbgUsb("[DBG : EP0_STATE_GD_CFG_ONLY]\n");
			if(oOtgDev.m_uDeviceRequestLength<=CONFIG_DESC_SIZE)
			{
				OTGDEV_SetInEpXferSize(EP_TYPE_CONTROL, 1, oOtgDev.m_uDeviceRequestLength);
				Outp32(DIEPCTL0, (1u<<31)|(1<<26)|(CONTROL_EP<<11)|(0<<0));	//ep0 enable, clear nak, next ep0, max 64byte
				OTGDEV_WrPktEp0(((u8 *)&(oOtgDev.m_oDesc.oDescConfig))+0, oOtgDev.m_uDeviceRequestLength); // EP0_PKT_SIZE
			}
			else
			{
				OTGDEV_SetInEpXferSize(EP_TYPE_CONTROL, 1, CONFIG_DESC_SIZE);
				Outp32(DIEPCTL0, (1u<<31)|(1<<26)|(CONTROL_EP<<11)|(0<<0));	//ep0 enable, clear nak, next ep0, max 64byte
				OTGDEV_WrPktEp0(((u8 *)&(oOtgDev.m_oDesc.oDescConfig))+0, CONFIG_DESC_SIZE); // EP0_PKT_SIZE
			}
			oOtgDev.m_uEp0State = EP0_STATE_INIT;
			break;

		// === GET_DESCRIPTOR:INTERFACE ONLY===

		case EP0_STATE_GD_IF_ONLY_0:
			if(oOtgDev.m_uDeviceRequestLength<=INTERFACE_DESC_SIZE)
			{
				OTGDEV_SetInEpXferSize(EP_TYPE_CONTROL, 1, oOtgDev.m_uDeviceRequestLength);
				Outp32(DIEPCTL0, (1u<<31)|(1<<26)|(CONTROL_EP<<11)|(0<<0));	//ep0 enable, clear nak, next ep0, max 64byte
				OTGDEV_WrPktEp0(((u8 *)&(oOtgDev.m_oDesc.oDescInterface))+0, oOtgDev.m_uDeviceRequestLength);	// INTERFACE_DESC_SIZE
			}
			else
			{
				OTGDEV_SetInEpXferSize(EP_TYPE_CONTROL, 1, INTERFACE_DESC_SIZE);
				Outp32(DIEPCTL0, (1u<<31)|(1<<26)|(CONTROL_EP<<11)|(0<<0));	//ep0 enable, clear nak, next ep0, max 64byte
				OTGDEV_WrPktEp0(((u8 *)&(oOtgDev.m_oDesc.oDescInterface))+0, INTERFACE_DESC_SIZE);	// INTERFACE_DESC_SIZE
			}
			oOtgDev.m_uEp0State = EP0_STATE_INIT;
			break;

		// === GET_DESCRIPTOR:ENDPOINT 1 ONLY===
		case EP0_STATE_GD_EP0_ONLY_0:
			OTGDEV_SetInEpXferSize(EP_TYPE_CONTROL, 1, ENDPOINT_DESC_SIZE);
			Outp32(DIEPCTL0, (1u<<31)|(1<<26)|(CONTROL_EP<<11)|(0<<0));	//ep0 enable, clear nak, next ep0, 64byte
			OTGDEV_WrPktEp0(((u8 *)&(oOtgDev.m_oDesc.oDescEndpt1))+0, ENDPOINT_DESC_SIZE);
			oOtgDev.m_uEp0State = EP0_STATE_INIT;
			break;

		// === GET_DESCRIPTOR:ENDPOINT 2 ONLY===
		case EP0_STATE_GD_EP1_ONLY_0:
			OTGDEV_SetInEpXferSize(EP_TYPE_CONTROL, 1, ENDPOINT_DESC_SIZE);
			Outp32(DIEPCTL0, (1u<<31)|(1<<26)|(CONTROL_EP<<11)|(0<<0));	//ep0 enable, clear nak, next ep0, 64byte
			OTGDEV_WrPktEp0(((u8 *)&(oOtgDev.m_oDesc.oDescEndpt2))+0, ENDPOINT_DESC_SIZE);
			oOtgDev.m_uEp0State = EP0_STATE_INIT;
			break;

		// === GET_DESCRIPTOR:STRING ===
		case EP0_STATE_GD_STR_I0:
			DbgUsb("[GDS0_0]");
			OTGDEV_SetInEpXferSize(EP_TYPE_CONTROL, 1, STRING_DESC0_SIZE);
			Outp32(DIEPCTL0, (1u<<31)|(1<<26)|(CONTROL_EP<<11)|(0<<0));	//ep0 enable, clear nak, next ep0, 64byte
			OTGDEV_WrPktEp0((u8 *)aDescStr0, STRING_DESC0_SIZE);
			oOtgDev.m_uEp0State = EP0_STATE_INIT;
			break;

		case EP0_STATE_GD_STR_I1:
			DbgUsb("[GDS1_%d]", oOtgDev.m_uEp0SubState);
			if ((oOtgDev.m_uEp0SubState*oOtgDev.m_uControlEPMaxPktSize+oOtgDev.m_uControlEPMaxPktSize)<sizeof(aDescStr1))
			{
				OTGDEV_SetInEpXferSize(EP_TYPE_CONTROL, 1, oOtgDev.m_uControlEPMaxPktSize);
				Outp32(DIEPCTL0, (1u<<31)|(1<<26)|(CONTROL_EP<<11)|(0<<0));	//ep0 enable, clear nak, next ep0, 64byte
				OTGDEV_WrPktEp0((u8 *)aDescStr1+(oOtgDev.m_uEp0SubState*oOtgDev.m_uControlEPMaxPktSize), oOtgDev.m_uControlEPMaxPktSize);
				oOtgDev.m_uEp0State = EP0_STATE_GD_STR_I1;
				oOtgDev.m_uEp0SubState++;
			}
			else
			{
				OTGDEV_SetInEpXferSize(EP_TYPE_CONTROL, 1, sizeof(aDescStr1)-(oOtgDev.m_uEp0SubState*oOtgDev.m_uControlEPMaxPktSize));
				Outp32(DIEPCTL0, (1u<<31)|(1<<26)|(CONTROL_EP<<11)|(0<<0));	//ep0 enable, clear nak, next ep0, 64byte
				OTGDEV_WrPktEp0((u8 *)aDescStr1+(oOtgDev.m_uEp0SubState*oOtgDev.m_uControlEPMaxPktSize), sizeof(aDescStr1)-(oOtgDev.m_uEp0SubState*oOtgDev.m_uControlEPMaxPktSize));
				oOtgDev.m_uEp0State = EP0_STATE_INIT;
				oOtgDev.m_uEp0SubState = 0;
			}
			break;

		case EP0_STATE_GD_STR_I2:
			DbgUsb("[GDS2_%d]", oOtgDev.m_uEp0SubState);
			if ((oOtgDev.m_uEp0SubState*oOtgDev.m_uControlEPMaxPktSize+oOtgDev.m_uControlEPMaxPktSize)<sizeof(aDescStr2))
			{
				OTGDEV_SetInEpXferSize(EP_TYPE_CONTROL, 1, oOtgDev.m_uControlEPMaxPktSize);
				Outp32(DIEPCTL0, (1u<<31)|(1<<26)|(CONTROL_EP<<11)|(0<<0));	//ep0 enable, clear nak, next ep0, 64byte
				OTGDEV_WrPktEp0((u8 *)aDescStr2+(oOtgDev.m_uEp0SubState*oOtgDev.m_uControlEPMaxPktSize), oOtgDev.m_uControlEPMaxPktSize);
				oOtgDev.m_uEp0State = EP0_STATE_GD_STR_I2;
				oOtgDev.m_uEp0SubState++;
			}
			else
			{
				OTGDEV_SetInEpXferSize(EP_TYPE_CONTROL, 1, sizeof(aDescStr2)-(oOtgDev.m_uEp0SubState*oOtgDev.m_uControlEPMaxPktSize));
				Outp32(DIEPCTL0, (1u<<31)|(1<<26)|(CONTROL_EP<<11)|(0<<0));	//ep0 enable, clear nak, next ep0, 64byte
				DbgUsb("[E]");
				OTGDEV_WrPktEp0((u8 *)aDescStr2+(oOtgDev.m_uEp0SubState*oOtgDev.m_uControlEPMaxPktSize), sizeof(aDescStr2)-(oOtgDev.m_uEp0SubState*oOtgDev.m_uControlEPMaxPktSize));
				oOtgDev.m_uEp0State = EP0_STATE_INIT;
				oOtgDev.m_uEp0SubState = 0;
			}
			break;

		case EP0_STATE_CONFIGURATION_GET:
			OTGDEV_SetInEpXferSize(EP_TYPE_CONTROL, 1, 1);
			Outp32(DIEPCTL0, (1u<<31)|(1<<26)|(CONTROL_EP<<11)|(0<<0));	//ep0 enable, clear nak, next ep0, 64byte
			Outp32(EP0_FIFO, g_usConfig);
			oOtgDev.m_uEp0State = EP0_STATE_INIT;
			break;

		case EP0_STATE_INTERFACE_GET:
			OTGDEV_SetInEpXferSize(EP_TYPE_CONTROL, 1, 1);
			Outp32(DIEPCTL0, (1u<<31)|(1<<26)|(CONTROL_EP<<11)|(0<<0));	//ep0 enable, clear nak, next ep0, 64byte
			OTGDEV_WrPktEp0((u8 *)&oInterfaceGet+0, 1);
			oOtgDev.m_uEp0State = EP0_STATE_INIT;
			break;


		case EP0_STATE_GET_STATUS0:
			OTGDEV_SetInEpXferSize(EP_TYPE_CONTROL, 1, 1);
			Outp32(DIEPCTL0, (1u<<31)|(1<<26)|(CONTROL_EP<<11)|(0<<0));	//ep0 enable, clear nak, next ep0, 64byte
			OTGDEV_WrPktEp0((u8 *)&oStatusGet+0, 1);
			oOtgDev.m_uEp0State = EP0_STATE_INIT;
			break;

		case EP0_STATE_GET_STATUS1:
			OTGDEV_SetInEpXferSize(EP_TYPE_CONTROL, 1, 1);
			Outp32(DIEPCTL0, (1u<<31)|(1<<26)|(CONTROL_EP<<11)|(0<<0));	//ep0 enable, clear nak, next ep0, 64byte
			OTGDEV_WrPktEp0((u8 *)&oStatusGet+1, 1);
			oOtgDev.m_uEp0State = EP0_STATE_INIT;
			break;

		case EP0_STATE_GET_STATUS2:
			OTGDEV_SetInEpXferSize(EP_TYPE_CONTROL, 1, 1);
			Outp32(DIEPCTL0, (1u<<31)|(1<<26)|(CONTROL_EP<<11)|(0<<0));	//ep0 enable, clear nak, next ep0, 64byte
			OTGDEV_WrPktEp0((u8 *)&oStatusGet+2, 1);
			oOtgDev.m_uEp0State = EP0_STATE_INIT;
			break;

		case EP0_STATE_GET_STATUS3:
			OTGDEV_SetInEpXferSize(EP_TYPE_CONTROL, 1, 1);
			Outp32(DIEPCTL0, (1u<<31)|(1<<26)|(CONTROL_EP<<11)|(0<<0));	//ep0 enable, clear nak, next ep0, 64byte
			OTGDEV_WrPktEp0((u8 *)&oStatusGet+3, 1);
			oOtgDev.m_uEp0State = EP0_STATE_INIT;
			break;

		case EP0_STATE_GET_STATUS4:
			OTGDEV_SetInEpXferSize(EP_TYPE_CONTROL, 1, 1);
			Outp32(DIEPCTL0, (1u<<31)|(1<<26)|(CONTROL_EP<<11)|(0<<0));	//ep0 enable, clear nak, next ep0, 64byte
			OTGDEV_WrPktEp0((u8 *)&oStatusGet+4, 1);
			oOtgDev.m_uEp0State = EP0_STATE_INIT;
			break;

		case EP0_STATE_TEST_MODE:					
			switch(oOtgDev.m_oDeviceRequest.wIndex_H)
			{	
				u32 uTemp;
				
				case TEST_J:
					//Set Test J
					OTGDEV_SetInEpXferSize(EP_TYPE_CONTROL, 1, 0);
					Outp32(DIEPCTL0, (1u<<31)|(1<<26)|(CONTROL_EP<<11)|(0<<0));	//ep0 enable, clear nak, next ep0, 64byte
					UART_Printf ("Test_J\n");
					uTemp = Inp32(DCTL);
					uTemp = uTemp & ~(TEST_CONTROL_FIELD) | (TEST_J_MODE);						
					Outp32(DCTL,uTemp);
					oOtgDev.m_uEp0State = EP0_STATE_INIT;
					break;

				case TEST_K:
					//Set Test K
					OTGDEV_SetInEpXferSize(EP_TYPE_CONTROL, 1, 0);
					Outp32(DIEPCTL0, (1u<<31)|(1<<26)|(CONTROL_EP<<11)|(0<<0));	//ep0 enable, clear nak, next ep0, 64byte
					UART_Printf ("Test_K\n");
					uTemp = Inp32(DCTL);
					uTemp = uTemp & ~(TEST_CONTROL_FIELD) | (TEST_K_MODE);						
					Outp32(DCTL,uTemp);
					oOtgDev.m_uEp0State = EP0_STATE_INIT;
					break;

				case TEST_SE0_NAK:
					//Set Test SE0_NAK
					OTGDEV_SetInEpXferSize(EP_TYPE_CONTROL, 1, 0);
					Outp32(DIEPCTL0, (1u<<31)|(1<<26)|(CONTROL_EP<<11)|(0<<0));	//ep0 enable, clear nak, next ep0, 64byte
					UART_Printf ("Test_SE0_NAK\n");
					uTemp = Inp32(DCTL);
					uTemp = uTemp & ~(TEST_CONTROL_FIELD) | (TEST_SE0_NAK_MODE);						
					Outp32(DCTL,uTemp);
					oOtgDev.m_uEp0State = EP0_STATE_INIT;
					break;

				case TEST_PACKET:
					//Set Test Packet
					OTGDEV_SetInEpXferSize(EP_TYPE_CONTROL, 1, 0);
					Outp32(DIEPCTL0, (1u<<31)|(1<<26)|(CONTROL_EP<<11)|(0<<0));	//ep0 enable, clear nak, next ep0, 64byte
					OTGDEV_SetInEpXferSize(EP_TYPE_CONTROL, 1, TEST_PKT_SIZE);
					Outp32(DIEPCTL0, (1u<<31)|(1<<26)|(CONTROL_EP<<11)|(0<<0));	//ep0 enable, clear nak, next ep0, 64byte
					OTGDEV_WrPktEp0(TestPkt, TEST_PKT_SIZE);
					UART_Printf ("Test_Packet\n");
					uTemp = Inp32(DCTL);
					uTemp = uTemp & ~(TEST_CONTROL_FIELD) | (TEST_PACKET_MODE);						
					Outp32(DCTL,uTemp);
					oOtgDev.m_uEp0State = EP0_STATE_INIT;
					break;	
					
				case TEST_FORCE_ENABLE:
					//Set Test Force Enable
					OTGDEV_SetInEpXferSize(EP_TYPE_CONTROL, 1, 0);
					Outp32(DIEPCTL0, (1u<<31)|(1<<26)|(CONTROL_EP<<11)|(0<<0));	//ep0 enable, clear nak, next ep0, 64byte
					UART_Printf ("Test_Force_Enable\n");
					uTemp = Inp32(DCTL);
					uTemp = uTemp & ~(TEST_CONTROL_FIELD) | (TEST_FORCE_ENABLE_MODE);						
					Outp32(DCTL,uTemp);
					oOtgDev.m_uEp0State = EP0_STATE_INIT;
					break;	
			}

		default:
			break;
	}
}


//////////
// Function Name : OTGDEV_HandleEvent_BulkIn
// Function Desctiption : This function handles bulk in transfer in CPU mode.
// Input : NONE
// Output : NONE
// Version :
void OTGDEV_HandleEvent_BulkIn(void)
{
	u8* BulkInBuf;
	u32 uRemainCnt;

	DbgUsb("CPU_MODE Bulk In Function\n");
	
//	Delay(1);
	
	BulkInBuf = oOtgDev.m_pUpPt;
	
	uRemainCnt = oOtgDev.m_uUploadSize- ((u32)oOtgDev.m_pUpPt - oOtgDev.m_uUploadAddr);

	if (uRemainCnt > oOtgDev.m_uBulkInEPMaxPktSize)
	{
		g_usUploadPktLength = oOtgDev.m_uBulkInEPMaxPktSize;
		OTGDEV_SetInEpXferSize(EP_TYPE_BULK, 1, g_usUploadPktLength);
		Outp32(bulkIn_DIEPCTL, 1u<<31|1<<26|2<<18|1<<15|oOtgDev.m_uBulkInEPMaxPktSize<<0);		//ep3 enable, clear nak, bulk, usb active, next ep3, max pkt 64
		OTGDEV_WrPktBulkInEp(BulkInBuf, g_usUploadPktLength);
	}	
	else if(uRemainCnt > 0)
	{		
		g_usUploadPktLength = uRemainCnt;
		OTGDEV_SetInEpXferSize(EP_TYPE_BULK, 1, g_usUploadPktLength);
		Outp32(bulkIn_DIEPCTL, 1u<<31|1<<26|2<<18|1<<15|oOtgDev.m_uBulkInEPMaxPktSize<<0);		//ep3 enable, clear nak, bulk, usb active, next ep3, max pkt 64
		OTGDEV_WrPktBulkInEp(BulkInBuf, g_usUploadPktLength);
	}
	else
	{
		g_usUploadPktLength = 0;
	}
}

//////////
// Function Name : OTGDEV_HandleEvent_BulkOut
// Function Desctiption : This function handles bulk out transfer.
// Input : NONE
// Output : NONE
// Version :
void OTGDEV_HandleEvent_BulkOut(u32 fifoCntByte)
{
	u8 TempBuf[16];
	u32 uCheck;

	if (oOtgDev.m_uDownloadFileSize==0)
	{
		if (fifoCntByte == 10)
		{		
			OTGDEV_RdPktBulkOutEp((u8 *)TempBuf, 10);
			uCheck = *((u8 *)(TempBuf+8)) + (*((u8 *)(TempBuf+9))<<8);

			if (uCheck==0x1)
			{
				oOtgDev.m_uUploadAddr =
					*((u8 *)(TempBuf+0))+
					(*((u8 *)(TempBuf+1))<<8)+
					(*((u8 *)(TempBuf+2))<<16)+
					(*((u8 *)(TempBuf+3))<<24);

				oOtgDev.m_uUploadSize =
					*((u8 *)(TempBuf+4))+
					(*((u8 *)(TempBuf+5))<<8)+
					(*((u8 *)(TempBuf+6))<<16)+
					(*((u8 *)(TempBuf+7))<<24);
				oOtgDev.m_pUpPt=(u8 *)oOtgDev.m_uUploadAddr;
				DbgUsb("UploadAddress : 0x%x, UploadSize: %d\n", oOtgDev.m_uUploadAddr, oOtgDev.m_uUploadSize);
 	
				if (oOtgDev.m_eOpMode == USB_DMA)
				{
					if (oOtgDev.m_uUploadSize>0)
					{
						u32 uPktCnt, uRemainder;
						
						DbgUsb("Dma Start for IN PKT \n");

						Outp32(GAHBCFG, MODE_DMA|BURST_INCR4|GBL_INT_UNMASK);
						Outp32(GINTMSK, INT_RESUME|INT_OUT_EP|INT_IN_EP|INT_ENUMDONE|INT_RESET|INT_SUSPEND); //gint unmask

						Outp32(bulkIn_DIEPDMA, (u32)oOtgDev.m_pUpPt);

						uPktCnt = (u32)(oOtgDev.m_uUploadSize/oOtgDev.m_uBulkInEPMaxPktSize);
						uRemainder = (u32)(oOtgDev.m_uUploadSize%oOtgDev.m_uBulkInEPMaxPktSize);
						if(uRemainder != 0)
						{
							uPktCnt += 1;
						}
						if (uPktCnt > 1023)
						{
							OTGDEV_SetInEpXferSize(EP_TYPE_BULK, 1023, (oOtgDev.m_uBulkInEPMaxPktSize*1023));							
						}
						else
						{
							OTGDEV_SetInEpXferSize(EP_TYPE_BULK, uPktCnt, oOtgDev.m_uUploadSize);
						}
					
						Outp32(bulkIn_DIEPCTL, 1u<<31|1<<26|2<<18|1<<15|BULK_IN_EP<<11|oOtgDev.m_uBulkInEPMaxPktSize<<0);		//ep1 enable, clear nak, bulk, usb active, next ep1, max pkt						
					}		
				}
				else	//CPU_MODE
				{
					OTGDEV_HandleEvent_BulkIn();
				}  
			}
			oOtgDev.m_uDownloadFileSize=0;
			return;
		}
		else
		{
			OTGDEV_RdPktBulkOutEp((u8 *)TempBuf, 8);
			DbgUsb("downloadFileSize==0, 1'st BYTE_READ_CNT_REG : %x\n", fifoCntByte);
			
			if(download_run==true)
			{
				oOtgDev.m_uDownloadAddress=
					*((u8 *)(TempBuf+0))+
					(*((u8 *)(TempBuf+1))<<8)+
					(*((u8 *)(TempBuf+2))<<16)+
					(*((u8 *)(TempBuf+3))<<24);
			}
			else
			{
				oOtgDev.m_uDownloadAddress=tempDownloadAddress;
			}

			oOtgDev.m_uDownloadFileSize=
				*((u8 *)(TempBuf+4))+
				(*((u8 *)(TempBuf+5))<<8)+
				(*((u8 *)(TempBuf+6))<<16)+
				(*((u8 *)(TempBuf+7))<<24);

			oOtgDev.m_pDownPt=(u8 *)oOtgDev.m_uDownloadAddress;
			DbgUsb("downloadAddress : 0x%x, downloadFileSize: %d\n", oOtgDev.m_uDownloadAddress, oOtgDev.m_uDownloadFileSize);

			OTGDEV_RdPktBulkOutEp((u8 *)oOtgDev.m_pDownPt, fifoCntByte-8); // The first 8-bytes are deleted.
			oOtgDev.m_pDownPt += fifoCntByte-8;
			
			if (oOtgDev.m_eOpMode == USB_CPU)
			{
				OTGDEV_SetOutEpXferSize(EP_TYPE_BULK, 1, oOtgDev.m_uBulkOutEPMaxPktSize);
				Outp32(bulkOut_DOEPCTL, 1u<<31|1<<26|2<<18|1<<15|oOtgDev.m_uBulkOutEPMaxPktSize<<0);		//ep3 enable, clear nak, bulk, usb active, next ep3, max pkt 64
			}
			else
			{
				if (oOtgDev.m_uDownloadFileSize>oOtgDev.m_uBulkOutEPMaxPktSize)
				{
					u32 uPktCnt, uRemainder;
					
					DbgUsb("downloadFileSize!=0, Dma Start for 2nd OUT PKT \n");
					Outp32(GINTMSK, INT_RESUME|INT_OUT_EP|INT_IN_EP|INT_ENUMDONE|INT_RESET|INT_SUSPEND); //gint unmask
					Outp32(GAHBCFG, MODE_DMA|BURST_INCR4|GBL_INT_UNMASK);
					Outp32(bulkOut_DOEPDMA, (u32)oOtgDev.m_pDownPt);
					uPktCnt = (u32)(oOtgDev.m_uDownloadFileSize-oOtgDev.m_uBulkOutEPMaxPktSize)/oOtgDev.m_uBulkOutEPMaxPktSize;
					uRemainder = (u32)((oOtgDev.m_uDownloadFileSize-oOtgDev.m_uBulkOutEPMaxPktSize)%oOtgDev.m_uBulkOutEPMaxPktSize);
					if(uRemainder != 0)
					{
						uPktCnt += 1;
					}
					if (uPktCnt > 1023)
					{
						OTGDEV_SetOutEpXferSize(EP_TYPE_BULK, 1023, (oOtgDev.m_uBulkOutEPMaxPktSize*1023));
					}
					else
					{
						OTGDEV_SetOutEpXferSize(EP_TYPE_BULK, uPktCnt, (oOtgDev.m_uDownloadFileSize-oOtgDev.m_uBulkOutEPMaxPktSize));
					}
					Outp32(bulkOut_DOEPCTL, 1u<<31|1<<26|2<<18|1<<15|oOtgDev.m_uBulkOutEPMaxPktSize<<0);		//ep3 enable, clear nak, bulk, usb active, next ep3, max pkt 64
				}		
			}
		}
	}

	else
	{
		if (oOtgDev.m_eOpMode == USB_CPU)
		{
			DbgUsb("downloadFileSize!=0, 2nd BYTE_READ_CNT_REG : %x\n", fifoCntByte);
			OTGDEV_RdPktBulkOutEp((u8 *)oOtgDev.m_pDownPt, fifoCntByte);
			oOtgDev.m_pDownPt += fifoCntByte;
			
			OTGDEV_SetOutEpXferSize(EP_TYPE_BULK, 1, oOtgDev.m_uBulkOutEPMaxPktSize);
			Outp32(bulkOut_DOEPCTL, 1u<<31|1<<26|2<<18|1<<15|oOtgDev.m_uBulkOutEPMaxPktSize<<0);		//ep3 enable, clear nak, bulk, usb active, next ep3, max pkt 64
		}
	}
}


//////////
// Function Name : OTGDEV_InitPhyCon
// Function Desctiption : This function initializes OTG Phy.
// Input : NONE
// Output : NONE
// Version :
void OTGDEV_InitPhyCon(void)
{
	Outp32(PHYPWR, 0x0);
	
	//if 48Mhz Oscillator is used
	Outp32(PHYCTRL, 0x20);
	//if 48Mhz X-tal is used
	//Outp32(PHYCTRL, 0x00);

	//if 24Mhz Oscillator is used
	//Outp32(PHYCTRL, 0x23);
	//if 24Mhz X-tal is used
	//Outp32(PHYCTRL, 0x03);

	//if 12Mhz Oscillator is used
	//Outp32(PHYCTRL, 0x22);
	//if 12Mhz X-tal is used
	//Outp32(PHYCTRL, 0x02);
	
	//PHY Tuning
	//Outp32(PHYTUNE, 0x00271B98);
							
	Outp32(RSTCON, 0x1);
	Delay(10);
	Outp32(RSTCON, 0x0);
	Delay(10);
}

//////////
// Function Name : OTGDEV_SoftResetCore
// Function Desctiption : This function soft-resets OTG Core and then unresets again.
// Input : NONE
// Output : NONE
// Version :
void OTGDEV_SoftResetCore(void)
{
	u32 uTemp;

	Outp32(GRSTCTL, CORE_SOFT_RESET);

	do
	{
		uTemp = Inp32(GRSTCTL);
	}while(!(uTemp & AHB_MASTER_IDLE));
	
}

//////////
// Function Name : OTGDEV_WaitCableInsertion
// Function Desctiption : This function checks if the cable is inserted.
// Input : NONE
// Output : NONE
// Version :
void OTGDEV_WaitCableInsertion(void)
{
	u32 uTemp, i=0;
	u8 ucFirst=1;

	do
	{
		Delay(10);
		
		uTemp = Inp32(GOTGCTL);

		if (uTemp & (B_SESSION_VALID|A_SESSION_VALID))
		{
			break;
		}
		else if(ucFirst == 1)
		{
			UART_Printf("\nInsert an OTG cable into the connector!\n");
			ucFirst = 0;
		}
		
		if(i%0x400==0)
			DisplayLED(0x1);
		if(i%0x400==0x100)
			DisplayLED(0x2);
		if(i%0x400==0x200)
			DisplayLED(0x4);
		if(i%0x400==0x300)
			DisplayLED(0x8);
		i++;

	}while(1);
}

//////////
// Function Name : OTGDEV_InitCore
// Function Desctiption : This function initializes OTG Link Core.
// Input : NONE
// Output : NONE
// Version :
void OTGDEV_InitCore(void)
{	
	Outp32(GAHBCFG, PTXFE_HALF|NPTXFE_HALF|MODE_SLAVE|BURST_SINGLE|GBL_INT_UNMASK);
	
	Outp32(GUSBCFG, 0<<15			// PHY Low Power Clock sel
					|1<<14			// Non-Periodic TxFIFO Rewind Enable
					|0x5<<10		// Turnaround time
					|0<<9|0<<8		// [0:HNP disable, 1:HNP enable][ 0:SRP disable, 1:SRP enable] H1= 1,1							
					|0<<7			// Ulpi DDR sel
					|0<<6			// 0: high speed utmi+, 1: full speed serial
					|0<<4			// 0: utmi+, 1:ulpi
					|1<<3			// phy i/f  0:8bit, 1:16bit
					|0x7<<0			// HS/FS Timeout*
					);
}

//////////
// Function Name : OTGDEV_CheckCurrentMode
// Function Desctiption : This function checks the current mode.
// Input : pucMode, current mode(device or host)
// Output : NONE
// Version :
void OTGDEV_CheckCurrentMode(u8 *pucMode)
{
	u32 uTemp;

	uTemp = Inp32(GINTSTS);
	*pucMode = uTemp & 0x1;
}

//////////
// Function Name : OTGDEV_SetSoftDisconnect
// Function Desctiption : This function puts the OTG device core in the disconnected state.
// Input : NONE
// Output : NONE
// Version :
void OTGDEV_SetSoftDisconnect(void)
{
	u32 uTemp;

	uTemp = Inp32(DCTL);
	uTemp |= SOFT_DISCONNECT;
	Outp32(DCTL, uTemp);
}

//////////
// Function Name : OTGDEV_ClearSoftDisconnect
// Function Desctiption : This function makes the OTG device core to exit from the disconnected state.
// Input : NONE
// Output : NONE
// Version :
void OTGDEV_ClearSoftDisconnect(void)
{
	u32 uTemp;

	uTemp = Inp32(DCTL);
	uTemp = uTemp & ~SOFT_DISCONNECT;
	Outp32(DCTL, uTemp);
}

//////////
// Function Name : OTGDEV_InitDevice
// Function Desctiption : This function configures OTG Core to initial settings of device mode.
// Input : NONE
// Output : NONE
// Version :
void OTGDEV_InitDevice(void)
{
	Outp32(DCFG, 1<<18|oOtgDev.m_eSpeed<<0);				// [][1: full speed(30Mhz) 0:high speed]

	Outp32(GINTMSK, INT_RESUME|INT_OUT_EP|INT_IN_EP|INT_ENUMDONE|INT_RESET
					|INT_SUSPEND|INT_RX_FIFO_NOT_EMPTY);	//gint unmask	
}



//////////
// Function Name : OTGDEV_SetAllOutEpNak
// Function Desctiption : This function sets NAK bit of all EPs.
// Input : NONE
// Output : NONE
// Version :
void OTGDEV_SetAllOutEpNak(void)
{
	u8 i;
	u32 uTemp;
	
	for(i=0;i<16;i++)
	{
		uTemp = Inp32(DOEPCTL0+0x20*i);
		uTemp |= DEPCTL_SNAK;
		Outp32(DOEPCTL0+0x20*i, uTemp);
	}
}

//////////
// Function Name : OTGDEV_ClearAllOutEpNak
// Function Desctiption : This function clears NAK bit of all EPs.
// Input : NONE
// Output : NONE
// Version :
void OTGDEV_ClearAllOutEpNak(void)
{
	u8 i;
	u32 uTemp;
	
	for(i=0;i<16;i++)
	{
		uTemp = Inp32(DOEPCTL0+0x20*i);
		uTemp |= (DEPCTL_EPENA|DEPCTL_CNAK);
		Outp32(DOEPCTL0+0x20*i, uTemp);
	}
}

//////////
// Function Name : OTGDEV_SetMaxPktSizes
// Function Desctiption : This function sets the max packet sizes of USB transfer types according to the speed.
// Input : eSpeed, usb speed(high or full)
// Output : NONE
// Version :
void OTGDEV_SetMaxPktSizes(USB_SPEED eSpeed)
{
	if (eSpeed == USB_HIGH)
	{
		oOtgDev.m_eSpeed = USB_HIGH;
		oOtgDev.m_uControlEPMaxPktSize = HIGH_SPEED_CONTROL_PKT_SIZE;
		oOtgDev.m_uBulkInEPMaxPktSize = HIGH_SPEED_BULK_PKT_SIZE;
		oOtgDev.m_uBulkOutEPMaxPktSize = HIGH_SPEED_BULK_PKT_SIZE;
	}
	else
	{
		oOtgDev.m_eSpeed = USB_FULL;
		oOtgDev.m_uControlEPMaxPktSize = FULL_SPEED_CONTROL_PKT_SIZE;
		oOtgDev.m_uBulkInEPMaxPktSize = FULL_SPEED_BULK_PKT_SIZE;
		oOtgDev.m_uBulkOutEPMaxPktSize = FULL_SPEED_BULK_PKT_SIZE;
	}
}

//////////
// Function Name : OTEDEV_SetOtherSpeedConfDescTable
// Function Desctiption : This function sets the endpoint-specific CSRs.
// Input : NONE
// Output : NONE
// Version :
void OTEDEV_SetOtherSpeedConfDescTable(u32 length)
{
	// Standard device descriptor
	if (oOtgDev.m_eSpeed == USB_HIGH) 
	{	
	    if (length ==9)
	    {	    
	    	OTGDEV_SetInEpXferSize(EP_TYPE_CONTROL, 1, 9);
			Outp32(DIEPCTL0, (1u<<31)|(1<<26)|(CONTROL_EP<<11)|(0<<0));	//ep0 enable, clear nak, next ep0, max 64byte
			OTGDEV_WrPktEp0(((u8 *)&aOtherSpeedConfiguration_full)+0, 9); // EP0_PKT_SIZE
		}
	    else if(length ==32)
		{	    
	    	OTGDEV_SetInEpXferSize(EP_TYPE_CONTROL, 1, 32);
			Outp32(DIEPCTL0, (1u<<31)|(1<<26)|(CONTROL_EP<<11)|(0<<0));	//ep0 enable, clear nak, next ep0, max 64byte
			OTGDEV_WrPktEp0(((u8 *)&aOtherSpeedConfiguration_fullTotal)+0, 32); // EP0_PKT_SIZE			
	    }
	}
	else
	{
	    if (length ==9)
	    {	    
	    	OTGDEV_SetInEpXferSize(EP_TYPE_CONTROL, 1, 9);
			Outp32(DIEPCTL0, (1u<<31)|(1<<26)|(CONTROL_EP<<11)|(0<<0));	//ep0 enable, clear nak, next ep0, max 64byte
			OTGDEV_WrPktEp0(((u8 *)&aOtherSpeedConfiguration_high)+0, 9); // EP0_PKT_SIZE
		}
	    else if(length ==32)
		{	    
	    	OTGDEV_SetInEpXferSize(EP_TYPE_CONTROL, 1, 32);
			Outp32(DIEPCTL0, (1u<<31)|(1<<26)|(CONTROL_EP<<11)|(0<<0));	//ep0 enable, clear nak, next ep0, max 64byte
			OTGDEV_WrPktEp0(((u8 *)&aOtherSpeedConfiguration_highTotal)+0, 32); // EP0_PKT_SIZE
	    }	 			
	}
	oOtgDev.m_uEp0State = EP0_STATE_INIT;
}

//////////
// Function Name : OTGDEV_SetEndpoint
// Function Desctiption : This function sets the endpoint-specific CSRs.
// Input : NONE
// Output : NONE
// Version :
void OTGDEV_SetEndpoint(void)
{
	// Unmask DAINT source
	Outp32(DIEPINT0, 0xff);
	Outp32(DOEPINT0, 0xff);	
	Outp32(bulkIn_DIEPINT, 0xff);
	Outp32(bulkOut_DOEPINT, 0xff);
	
	// Init For Ep0
	Outp32(DIEPCTL0, ((1<<26)|(CONTROL_EP<<11)|(0<<0)));	//MPS:64bytes
	Outp32(DOEPCTL0, (1u<<31)|(1<<26)|(0<<0));		//ep0 enable, clear nak
}

//////////
// Function Name : OTGDEV_SetDescriptorTable
// Function Desctiption : This function sets the standard descriptors.
// Input : NONE
// Output : NONE
// Version :
void OTGDEV_SetDescriptorTable(void)
{
	// Standard device descriptor
	oOtgDev.m_oDesc.oDescDevice.bLength=DEVICE_DESC_SIZE;	//0x12 
	oOtgDev.m_oDesc.oDescDevice.bDescriptorType=DEVICE_DESCRIPTOR;
	oOtgDev.m_oDesc.oDescDevice.bDeviceClass=0xFF; // 0x0
	oOtgDev.m_oDesc.oDescDevice.bDeviceSubClass=0x0;
	oOtgDev.m_oDesc.oDescDevice.bDeviceProtocol=0x0;
	oOtgDev.m_oDesc.oDescDevice.bMaxPacketSize0=oOtgDev.m_uControlEPMaxPktSize;
	oOtgDev.m_oDesc.oDescDevice.idVendorL=0xE8;	//0x45;
	oOtgDev.m_oDesc.oDescDevice.idVendorH=0x04;	//0x53;
	oOtgDev.m_oDesc.oDescDevice.idProductL=0x34; //0x00
	oOtgDev.m_oDesc.oDescDevice.idProductH=0x12; //0x64
	oOtgDev.m_oDesc.oDescDevice.bcdDeviceL=0x00;
	oOtgDev.m_oDesc.oDescDevice.bcdDeviceH=0x01;
	oOtgDev.m_oDesc.oDescDevice.iManufacturer=0x1; // index of string descriptor
	oOtgDev.m_oDesc.oDescDevice.iProduct=0x2;	// index of string descriptor
	oOtgDev.m_oDesc.oDescDevice.iSerialNumber=0x0;
	oOtgDev.m_oDesc.oDescDevice.bNumConfigurations=0x1;
	
	oOtgDev.m_oDesc.oDescDevice.bcdUSBL=0x00;
	oOtgDev.m_oDesc.oDescDevice.bcdUSBH=0x02; 	// Ver 2.0

	// Standard configuration descriptor
	oOtgDev.m_oDesc.oDescConfig.bLength=CONFIG_DESC_SIZE; // 0x9 bytes
	oOtgDev.m_oDesc.oDescConfig.bDescriptorType=CONFIGURATION_DESCRIPTOR;
	oOtgDev.m_oDesc.oDescConfig.wTotalLengthL=CONFIG_DESC_TOTAL_SIZE; 
	oOtgDev.m_oDesc.oDescConfig.wTotalLengthH=0;
	oOtgDev.m_oDesc.oDescConfig.bNumInterfaces=1;
// dbg    descConf.bConfigurationValue=2; // why 2? There's no reason.
	oOtgDev.m_oDesc.oDescConfig.bConfigurationValue=1;
	oOtgDev.m_oDesc.oDescConfig.iConfiguration=0;
	oOtgDev.m_oDesc.oDescConfig.bmAttributes=CONF_ATTR_DEFAULT|CONF_ATTR_SELFPOWERED; // bus powered only.
	oOtgDev.m_oDesc.oDescConfig.maxPower=25; // draws 50mA current from the USB bus.

	// Standard interface descriptor
	oOtgDev.m_oDesc.oDescInterface.bLength=INTERFACE_DESC_SIZE; // 9
	oOtgDev.m_oDesc.oDescInterface.bDescriptorType=INTERFACE_DESCRIPTOR;
	oOtgDev.m_oDesc.oDescInterface.bInterfaceNumber=0x0;
	oOtgDev.m_oDesc.oDescInterface.bAlternateSetting=0x0; // ?
	oOtgDev.m_oDesc.oDescInterface.bNumEndpoints = 2;	// # of endpoints except EP0
	oOtgDev.m_oDesc.oDescInterface.bInterfaceClass=0xff; // 0x0 ?
	oOtgDev.m_oDesc.oDescInterface.bInterfaceSubClass=0x0;
	oOtgDev.m_oDesc.oDescInterface.bInterfaceProtocol=0x0;
	oOtgDev.m_oDesc.oDescInterface.iInterface=0x0;

	// Standard endpoint0 descriptor
	oOtgDev.m_oDesc.oDescEndpt1.bLength=ENDPOINT_DESC_SIZE;
	oOtgDev.m_oDesc.oDescEndpt1.bDescriptorType=ENDPOINT_DESCRIPTOR;
	oOtgDev.m_oDesc.oDescEndpt1.bEndpointAddress=BULK_IN_EP|EP_ADDR_IN; // 2400Xendpoint 1 is IN endpoint.
	oOtgDev.m_oDesc.oDescEndpt1.bmAttributes=EP_ATTR_BULK;
	oOtgDev.m_oDesc.oDescEndpt1.wMaxPacketSizeL=(u8)oOtgDev.m_uBulkInEPMaxPktSize; // 64
	oOtgDev.m_oDesc.oDescEndpt1.wMaxPacketSizeH=(u8)(oOtgDev.m_uBulkInEPMaxPktSize>>8);
	oOtgDev.m_oDesc.oDescEndpt1.bInterval=0x0; // not used

	// Standard endpoint1 descriptor
	oOtgDev.m_oDesc.oDescEndpt2.bLength=ENDPOINT_DESC_SIZE;
	oOtgDev.m_oDesc.oDescEndpt2.bDescriptorType=ENDPOINT_DESCRIPTOR;
	oOtgDev.m_oDesc.oDescEndpt2.bEndpointAddress=BULK_OUT_EP|EP_ADDR_OUT; // 2400X endpoint 3 is OUT endpoint.
	oOtgDev.m_oDesc.oDescEndpt2.bmAttributes=EP_ATTR_BULK;
	oOtgDev.m_oDesc.oDescEndpt2.wMaxPacketSizeL=(u8)oOtgDev.m_uBulkOutEPMaxPktSize; // 64
	oOtgDev.m_oDesc.oDescEndpt2.wMaxPacketSizeH=(u8)(oOtgDev.m_uBulkOutEPMaxPktSize>>8);
	oOtgDev.m_oDesc.oDescEndpt2.bInterval=0x0; // not used
}

//////////
// Function Name : OTGDEV_CheckEnumeratedSpeed
// Function Desctiption : This function checks the current usb speed.
// Input : eSpeed, usb speed(high or full)
// Output : NONE
// Version :
void OTGDEV_CheckEnumeratedSpeed(USB_SPEED *eSpeed)
{
	u32 uDStatus;
	
	uDStatus = Inp32(DSTS); // System status read

	*eSpeed = (USB_SPEED)((uDStatus&0x6) >>1);
}




//////////
// Function Name : OTGDEV_SetInEpXferSize
// Function Desctiption : This function sets DIEPTSIZn CSR according to input parameters.
// Input : eType, transfer type
//			uPktCnt, packet count to transfer
//			uXferSize, transfer size
// Output : NONE
// Version :
void OTGDEV_SetInEpXferSize(EP_TYPE eType, u32 uPktCnt, u32 uXferSize)
{	
	if(eType == EP_TYPE_CONTROL)
	{
		Outp32(DIEPTSIZ0, (uPktCnt<<19)|(uXferSize<<0));
	}
	else if(eType == EP_TYPE_BULK)
	{
		Outp32(bulkIn_DIEPTSIZ, (1<<29)|(uPktCnt<<19)|(uXferSize<<0));
	}
}

//////////
// Function Name : OTGDEV_SetOutEpXferSize
// Function Desctiption : This function sets DOEPTSIZn CSR according to input parameters.
// Input : eType, transfer type
//			uPktCnt, packet count to transfer
//			uXferSize, transfer size
// Output : NONE
// Version :
void OTGDEV_SetOutEpXferSize(EP_TYPE eType, u32 uPktCnt, u32 uXferSize)
{	
	if(eType == EP_TYPE_CONTROL)
	{
		Outp32(DOEPTSIZ0, (3<<29)|(uPktCnt<<19)|(uXferSize<<0));
	}
	else if(eType == EP_TYPE_BULK)
	{
		Outp32(bulkOut_DOEPTSIZ, (uPktCnt<<19)|(uXferSize<<0));
	}
}

//////////
// Function Name : OTGDEV_WrPktEp0
// Function Desctiption : This function reads data from the buffer and writes the data on EP0 FIFO.
// Input : buf, address of the data buffer to write on Control EP FIFO
//			num, size of the data to write on Control EP FIFO(byte count)
// Output : NONE
// Version :
void OTGDEV_WrPktEp0(u8 *buf, int num)
{
	int i;
	u32 Wr_Data=0;

	for(i=0;i<num;i+=4)
	{
		//rb1004...080309 : Generated the "alignment error" when the compile option is -O3(best code)
		//Wr_Data = ((*(buf+3))<<24)|((*(buf+2))<<16)|((*(buf+1))<<8)|*buf;
		Wr_Data = 0;
		Wr_Data |= ((*(buf+3))<<24);
		Wr_Data |= ((*(buf+2))<<16);
		Wr_Data |= ((*(buf+1))<<8);
		Wr_Data |= *buf;
		Outp32(control_EP_FIFO, Wr_Data);
		buf += 4;
	}
}

//////////
// Function Name : OTGDEV_PrintEp0Pkt
// Function Desctiption : This function reads data from the buffer and displays the data.
// Input : pt, address of the data buffer to read
//			count, size of the data to read(byte count)
// Output : NONE
// Version :
void OTGDEV_PrintEp0Pkt(u8 *pt, u8 count)
{
	int i;
	DbgUsb("[DBG:");
	for(i=0;i<count;i++)
		DbgUsb("%x,", pt[i]);
	DbgUsb("]\n");
}


//////////
// Function Name : OTGDEV_WrPktBulkInEp
// Function Desctiption : This function reads data from the buffer and writes the data on Bulk In EP FIFO.
// Input : buf, address of the data buffer to write on Bulk In EP FIFO
//			num, size of the data to write on Bulk In EP FIFO(byte count)
// Output : NONE
// Version :
void OTGDEV_WrPktBulkInEp(u8 *buf, int num)
{
	int i;
	u32 Wr_Data=0;

	for(i=0;i<num;i+=4)
	{
		Wr_Data=((*(buf+3))<<24)|((*(buf+2))<<16)|((*(buf+1))<<8)|*buf;
		Outp32(bulkIn_EP_FIFO, Wr_Data);
		buf += 4;
	}
}

//////////
// Function Name : OTGDEV_RdPktBulkOutEp
// Function Desctiption : This function reads data from Bulk Out EP FIFO and writes the data on the buffer.
// Input : buf, address of the data buffer to write
//			num, size of the data to read from Bulk Out EP FIFO(byte count)
// Output : NONE
// Version :
void OTGDEV_RdPktBulkOutEp(u8 *buf, int num)
{
	int i;
	u32 Rdata;

	for (i=0;i<num;i+=4)
	{
		Rdata = Inp32(bulkOut_EP_FIFO);

		buf[i] = (u8)Rdata;
		buf[i+1] = (u8)(Rdata>>8);
		buf[i+2] = (u8)(Rdata>>16);
		buf[i+3] = (u8)(Rdata>>24);
	}
}



//////////
// Function Name : OTGDEV_ClearDownFileInfo
// Function Desctiption : This function clears variables related to file-downloading.
// Input : NONE
// Output : NONE
// Version :
void OTGDEV_ClearDownFileInfo(void)
{
	oOtgDev.m_uDownloadAddress = 0;
	oOtgDev.m_uDownloadFileSize = 0;
	oOtgDev.m_pDownPt = 0;
}

//////////
// Function Name : OTGDEV_GetDownFileInfo
// Function Desctiption : This function reads download file info. and write it on input pointer parameters.
// Input : uDownAddr, download start address
//			uDownFileSize, download file size
//			pDownPt, current memory address to write data on
// Output : NONE
// Version :
void OTGDEV_GetDownFileInfo(u32* uDownAddr, u32* uDownFileSize, u32* pDownPt)
{
	u32 uDmaEnCheck;
	
	*uDownAddr = oOtgDev.m_uDownloadAddress;
	*uDownFileSize = oOtgDev.m_uDownloadFileSize;

	uDmaEnCheck = Inp32(GAHBCFG);
	if ((uDmaEnCheck&MODE_DMA))	//DMA mode
	{
		*pDownPt = Inp32(bulkOut_DOEPDMA);
	}
	else	//CPU mode
	{
		*pDownPt = (u32)oOtgDev.m_pDownPt;
	}
}

//////////
// Function Name : OTGDEV_ClearUpFileInfo
// Function Desctiption : This function clears variables related to file-uploading.
// Input : NONE
// Output : NONE
// Version :
void OTGDEV_ClearUpFileInfo(void)
{
	oOtgDev.m_uUploadAddr= 0;
	oOtgDev.m_uUploadSize= 0;
	oOtgDev.m_pUpPt = 0;
}

//////////
// Function Name : OTGDEV_GetUpFileInfo
// Function Desctiption : This function reads upload file info. and write it on input pointer parameters.
// Input : uUpAddr, upload start address
//			uUpFileSize, upload file size
//			pUpPt, current memory address to read data from
// Output : NONE
// Version :
void OTGDEV_GetUpFileInfo(u32* uUpAddr, u32* uUpFileSize, u32* pUpPt)
{
	u32 uDmaEnCheck;
	
	*uUpAddr = oOtgDev.m_uUploadAddr;
	*uUpFileSize = oOtgDev.m_uUploadSize;

	uDmaEnCheck = Inp32(GAHBCFG);
	if ((uDmaEnCheck&MODE_DMA))	//DMA mode
	{
		*pUpPt = Inp32(bulkIn_DIEPDMA);
	}
	else	//CPU mode
	{
		*pUpPt = (u32)oOtgDev.m_pUpPt;
	}
}




//////////
// Function Name : OTGDEV_IsUsbOtgSetConfiguration
// Function Desctiption : This function checks if Set Configuration is received from the USB Host.
// Input : NONE
// Output : configuration result
// Version :
u8 OTGDEV_IsUsbOtgSetConfiguration(void)
{
	if (oOtgDev.m_uIsUsbOtgSetConfiguration == 0)
		return false;
	else
		return true;
}

//////////
// Function Name : OTGDEV_SetOpMode
// Function Desctiption : This function sets CSRs related to the operation mode.
// Input : eMode, operation mode(cpu or dma)
// Output : NONE
// Version :
void OTGDEV_SetOpMode(USB_OPMODE eMode)
{
	oOtgDev.m_eOpMode = eMode;

	Outp32(GINTMSK, INT_RESUME|INT_OUT_EP|INT_IN_EP|INT_ENUMDONE|INT_RESET|INT_SUSPEND|INT_RX_FIFO_NOT_EMPTY); //gint unmask
	Outp32(GAHBCFG, MODE_SLAVE|BURST_SINGLE|GBL_INT_UNMASK);

	OTGDEV_SetOutEpXferSize(EP_TYPE_BULK, 1, oOtgDev.m_uBulkOutEPMaxPktSize);
	OTGDEV_SetInEpXferSize(EP_TYPE_BULK, 1, 0);

	Outp32(bulkOut_DOEPCTL, 1u<<31|1<<26|2<<18|1<<15|oOtgDev.m_uBulkOutEPMaxPktSize<<0);		//bulk out ep enable, clear nak, bulk, usb active, max pkt
	Outp32(bulkIn_DIEPCTL, 0u<<31|1<<27|2<<18|1<<15|oOtgDev.m_uBulkInEPMaxPktSize<<0);			//bulk in ep enable, clear nak, bulk, usb active, max pkt
}

//////////
// Function Name : OTGDEV_VerifyChecksum
// Function Desctiption : This function calculates the checksum by summing all downloaded data
//						and then compares the result with the checksum value which DNW sent.
// Input : NONE
// Output : NONE
// Version :
void OTGDEV_VerifyChecksum(void)
{
	u8 *cs_start, *cs_end;
	u16 dnCS;
	u16 checkSum;
	u8 bSkip=false;
	
	UART_Printf("\nChecksum is being calculated....\n");
	UART_Printf("(If you want to skip, press 'x' key)\n\n");

	// checksum calculation
	cs_start = (u8*)oOtgDev.m_uDownloadAddress;
	cs_end = (u8*)(oOtgDev.m_uDownloadAddress+oOtgDev.m_uDownloadFileSize-10);
	checkSum = 0;
	while(cs_start < cs_end)
	{
		checkSum += *cs_start++;
		if (GetKey()=='x')
		{
			bSkip = true;
			break;
		}
	}

	dnCS=*cs_end+((*(cs_end+1))<<8);

	if (bSkip == true)
	{
		UART_Printf("Checksum check is skipped.\n");
	}
	else if (checkSum == dnCS)
	{
		UART_Printf("Checksum O.K.\n\n");
	}
	else
	{
		UART_Printf("Checksum Value => MEM:%x DNW:%x\n",checkSum,dnCS);
		UART_Printf("Checksum failed.\n\n");
	}
}
