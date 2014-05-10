//====================================================================
// File Name : IIC.h
// Function  : S3C2440 IIC Test Program Head file
// Program   : Shin, On Pil (SOP)
// Date      : March 20, 2002
// Version   : 0.0
// History
//   0.0 : Programming start (March 11, 2002) -> SOP
//====================================================================

#ifndef __IIC_H__
#define __IIC_H__

#define WRDATA      (1)
#define POLLACK     (2)
#define RDDATA      (3)
#define SETRDADDR   (4)

#define IICBUFSIZE 0x20

void Test_Iic(void);
void Test_Iic2(void);
void Wr24C080(ULONG slvAddr,ULONG addr,UCHAR data);
void Rd24C080(ULONG slvAddr,ULONG addr,UCHAR *data);
void _Wr24C080(ULONG slvAddr,ULONG addr,UCHAR data);
void _Rd24C080(ULONG slvAddr,ULONG addr,UCHAR *data);

void __irq IicInt(void);
void IicPoll(void);
void Run_IicPoll(void);
void Iic_Test(void);


#endif    //__IIC_H__
