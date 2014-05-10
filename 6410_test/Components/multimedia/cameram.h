#ifndef __CAMERAM_H__
#define __CAMERAM_H__

#include "iic.h"
#include "gpio.h"

#ifndef CAM_MCLK_FREQ
#define CAM_MCLK_FREQ (16*1000000)
#endif

typedef enum
{
	SUB_SAMPLING0, 
	SUB_SAMPLING2, // 1/2 sub sampling
	SUB_SAMPLING4   // 1/4 sub sampling
} SUB_SAMPLING;

typedef enum
{
 	CCIR601, CCIR656
} CAM_ATTR;

typedef struct
{
	u8   m_ucSlaveAddr;
} CAMERA;

void CAMERA_InitOV7620(CAM_ATTR eCcir, u32 uBits, u8 bInv, CSPACE eSrcFmt);
void CAMERA_InitS5K3AAE_VGA(void);
//void CAMERA_InitS5K4AAF(CAM_ATTR eCcir, CSPACE eSrcFmt, IMG_SIZE eSize);
void CAMERA_InitS5K3BAF(CAM_ATTR eCcir, CSPACE eSrcFmt,SUB_SAMPLING eSub);
//void CAMERA_SetSizeInS5K4AAF(IMG_SIZE eSrcSize); 
void CAMERA_SetSizeInS5K3BAF(SUB_SAMPLING eSub);
void CAMERA_SetSizeInS5K3AAE(IMG_SIZE eSrcSize);

void CAMERA_IICOpen(int Frequency);
void CAMERA_IICClose(void);
void CAMERA_SetupCameraModule( char CameraIICID, char CameraModulData[][2], int CameraModuleDataSize);
void CAMERA_WriteCameraModule(u8 CameraIICID, u8 Addr, u8 Data);
void CAMERA_ReadCameraModule( char CameraIICID, char Addr, char *Data);
void CAMERA_Read_Write(u32 uCAMTYPE,CAM_ATTR eCcir, CSPACE eSrcFmt, SUB_SAMPLING eSub);

void CAMERA_SetReg(u8 ucSubAddr, u8 ucData);
void CAMERA_GetReg(u8 ucSubAddr, u8* ucData);

void CAMERA_InitA3AFX_QVGA_15FR(void);
void CAMERA_InitA3AFX_QVGA_20FR(void);
void CAMERA_InitA3AFX_QCIF_30FR(void);
void CAMERA_InitA3AFX_QCIF_15FR(void);
void CAMERA_InitS5K3AAE(ITU_R_STANDARD eItuR, CSPACE eSrcFmt, IMG_SIZE eSrcSz);

void CAMERA_InitS5K4BA(ITU_R_STANDARD eItuR, CSPACE eSrcFmt, IMG_SIZE eSrcSz);

void CAMERA_InitS5K4BAF(CAM_ATTR eCcir, CSPACE eSrcFmt, SUB_SAMPLING eSub);
void CAMERA_SetSizeInS5K4BAF(SUB_SAMPLING eSub);
void CAMERA_InitS5K4BAF2(CAM_ATTR eCcir, CSPACE eSrcFmt, SUB_SAMPLING eSub);
// for test jungil 
void CAMERA_Init_K5K4CA();
void CAMERA_SetReg_4CA(u16 ucSubAddr, u16 ucData);



#endif //__CAMERA_H__

