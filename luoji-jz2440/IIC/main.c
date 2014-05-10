/*
 * main.c
 *
 *  Created on: 2014-2-21
 *      Author: cyj
 */
#include "2440lib.h"
#include "addr2440.h"
#include "init2440.h"

#define GPF4_MASK (3 << (2 * 4))
#define GPF0_MASK (3 << (2 * 0))

#define GPF4_CON (1 << (2 * 4)) // out
#define GPF0_CON (2 << (2 * 0)) // Eint
static unsigned char _iicData[0x20];
static volatile int _iicDataCount;
static volatile int _iicStatus;
static volatile int _iicMode;
static int _iicPt;

void EINT_Handle() {
    int i;
    unsigned long offset = INTOFFSET;
    if (offset == 27) {
        Uart_SendString("IIC interrupt\n");
        switch (_iicMode) {
        case 1:
            if ((_iicDataCount--) == 0) {
                rIICSTAT = 0xd0;                //Stop MasTx condition
                rIICCON  = 0xaf;                //Resumes IIC operation.
                for(i=0;i<100;i++);//Delay(1);                       //Wait until stop condtion is in effect.
                break;
            }
            rIICDS = _iicData[_iicPt++];
            for(i=0;i<10;i++);                  //for setup time until rising edge of IICSCL
            rIICCON = 0xaf;
            break;
        case 0:
            if ((_iicDataCount--) == 0) {
                break;
            }
            rIICDS = _iicData[_iicPt++];
            for(i=0;i<10;i++);                  //for setup time until rising edge of IICSCL
            rIICCON = 0xaf;
            break;
        case 2:
            if ((_iicDataCount--) == 0) {
                _iicData[_iicPt++] = rIICDS;
                rIICSTAT = 0x90;                //Stop MasTx condition
                rIICCON  = 0xaf;                //Resumes IIC operation.
                for(i=0;i<100;i++);//Delay(1);                       //Wait until stop condtion is in effect.
                break;
            }
            _iicData[_iicPt++] = rIICDS;
            for(i=0;i<10;i++);                  //for setup time until rising edge of IICSCL
            if (_iicDataCount == 0)
                rIICCON = 0x2f;
            else
                rIICCON = 0xaf;
            break;
        }
    }

    SRCPND |= 1<<offset;
    INTPND |= 1<<offset;
}

void Wr24C080(unsigned long slvaddr, unsigned long address, unsigned char data)
{
    int i;
    _iicData[0] = (unsigned char)address;
    _iicData[1] = data;
    _iicDataCount = 2;
    _iicPt = 0;
    _iicMode = 1;

    rIICDS = slvaddr;
    rIICSTAT = 0xf0; // 11110000
    while(_iicDataCount!=-1);
}

void Rd24C080(unsigned long slvaddr, unsigned long address, unsigned char* data)
{
    int i;
    _iicData[0] = (unsigned char)address;
    _iicDataCount = 1;
    _iicPt = 0;
    _iicMode = 0;

    rIICDS = slvaddr;
    rIICSTAT = 0xf0; // 11110000
    while(_iicDataCount!=-1);

    _iicDataCount = 1;
    _iicPt = 0;
    _iicMode = 2;

    rIICDS = slvaddr;
    rIICSTAT = 0xb0; // 10110000
    //rIICCON       = 0xaf;               //Resumes IIC operation.
    while(_iicDataCount!=-1);
    *data = _iicData[1];
}

void Test_IIC()
{
    int i,j;
    static unsigned char data[256];
    rGPECON = (0xa<<28); //GPE15:IICSDA , GPE14:IICSCL
    INTMSK &= ~(1<<27); // I2c interrupt
    //Enable ACK, Prescaler IICCLK=PCLK/16, Enable interrupt, Transmit clock value Tx clock=IICCLK/16
    // If PCLK 50.7MHz, IICCLK = 3.17MHz, Tx Clock = 0.198MHz
    rIICCON = (1<<7) | (0<<6) | (1<<5) | (0xf);
    rIICADD  = 0x10;                    //2440 slave address = [7:1]
    rIICSTAT = 0x10;                    //IIC bus data output enable(Rx/Tx)

    Uart_Printf("Write test data into AT24C02\n");

    for(i=0;i<256;i++)
        Wr24C080(0xa0,(unsigned char)i,i);

    for(i=0;i<256;i++)
        data[i] = 0;

    Uart_Printf("Read test data from AT24C02\n");

    for(i=0;i<256;i++)
        Rd24C080(0xa0,(unsigned char)i,&(data[i]));

        //Line changed 0 ~ f
    for(i=0;i<16;i++)
    {
        for(j=0;j<16;j++)
            Uart_Printf("%2x ",data[i*16+j]);
        Uart_Printf("\n");
    }
    INTMSK |= (1<<27);
}

int main() {
    disable_watchdog();
    init_sdram();
	init_clock();
	init_uart();

	Uart_SendString("Hellocyj");
	//rGPBCON = 0x2; // GPB0 = TOUt0

    GPFCON &= ~(GPF4_MASK); //清0
    GPFCON |= (GPF4_CON); //赋值
    GPFDAT |= (1 << 4);

    Test_IIC();



    while (1);


    return 0;
}
