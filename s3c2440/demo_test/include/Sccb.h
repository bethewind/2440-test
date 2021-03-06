/*
*
* History:
*	Dec 15, 2002	Generated by DonGo.
*
*/


#ifndef __SCCB_H__
#define __SCCB_H__

#ifdef __cplusplus
extern "C" {
#endif

// GP9:SCL, GP8:SDA
#define SCL			(1<<14)
#define SDA			(1<<15)

#define SCL0			(rGPEDAT &= ~SCL)
#define SCL1			(rGPEDAT |=	SCL)
#define SDA0			(rGPEDAT &= ~SDA)
#define SDA1			(rGPEDAT |=	SDA)
//#define SDAOUT 		SDA
#define GetSDA		(rGPEDAT)

// Change GP8(I2CSDA) as input/output mode.
#define	SET_SDA_INPUT		(rGPECON &= ~(3<<30))
#define	SET_SDA_OUTPUT	(rGPECON = (rGPECON & ~(2<<30)) |(1<<30) )


// Function prototypes
unsigned char getCIS(unsigned char bSubAddr);
void Init_Sccb_Port(void);
void Prt_Cis(void);
void Sccb_Start(void);
void Sccb_Stop(void);
void Sccb_Ack(void);
void Sccb_Write_8bit(U8 Write_Byte);
unsigned char Sccb_Read_8bit(void);
void Init_Cis_YCbCr8bit(void);
void Init_Camera_Module_YCbCr_VGA(void);
void Init_Camera_Module_YCbCr_CIF(void);
void Test_SCCB(void);
void Test_Camera_Module_YCbCr_VGA(void);






#ifdef __cplusplus
}
#endif
#endif /*__SCCB_H__*/

