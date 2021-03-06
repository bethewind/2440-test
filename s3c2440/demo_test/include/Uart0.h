#ifndef __UART0_H__
#define __UART0_H__

#define UARTBUFFER  _NONCACHE_STARTADDRESS
#define TX_INTTYPE 1	//0:Tx interrupt type is pulse, 1:Tx interrupt type is level
#define RX_INTTYPE 1	//0:Rx interrupt type is pulse, 1:Rx interrupt type is level
#define AFC_BUFLEN  255

extern volatile U16 rx_dncs;
extern volatile U32 rx_point,rx_isdone,rx_filesize,rx_checksum;
extern volatile U32 isDone,isTxInt,isRxInt,tx_cnt,tx_end,rx_cnt,rx_end,afc_err;
extern volatile U8 *txdataPt;
extern volatile U8 *txdataFl;
extern volatile U8 *rxdataPt;
extern volatile U8 *rxdataCk;
extern volatile U32 *dbg_data;

extern void Uart_Port_Set(void);
extern void Uart_Port_Return(void);
extern void Uart_Uextclk_En(int ch,int baud, int clock);
extern void Uart_Pclk_En(int, int);
extern void Uart_Fclkn_En(int ch, int baud);

void Test_Uart0_Int(void);
void Test_Uart0_Dma(void);
void Test_Uart0_Fifo(void);
void Test_Uart0_AfcTx(void);
void Test_Uart0_AfcRx(void);
void Test_Rts_Rx(void);
void Test_Uart0_RxErr(void);
void Test_Uart0_Temp(void);


#endif /*__UART0_H__*/

