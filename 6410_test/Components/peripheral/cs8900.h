#ifndef __CS8900_H__
#define __CS8900_H__

/*
 * Bus interface registers
 */

#define PKTPG_EISA_NUMBER           0x0000
#define PKTPG_PRDCT_ID_CODE         0x0002
#define PKTPG_IO_BASE_ADDR          0x0020
#define PKTPG_INTERRUPT_NUMBER      0x0022
#define PKTPG_DMA_CHANNEL_NUMBER    0x0024
#define PKTPG_DMA_START_OF_FRAME    0x0026
#define PKTPG_DMA_FRAME_COUNT       0x0028
#define PKTPG_RX_DMA_BYTE_COUNT     0x002a
#define PKTPG_MEMORY_BASE_ADDR      0x002c
#define PKTPG_BOOT_PROM_BASE_ADDR   0x0030
#define PKTPG_BOOT_PROM_ADDR_MASK   0x0034
#define PKTPG_EEPROM_COMMAND        0x0040
#define PKTPG_EEPROM_DATA           0x0042
#define PKTPG_RX_FRAME_BYTE_COUNT   0x0050

/*
 * Status and control registers
 */
#define PKTPG_ISQ                   0x0120
#define PKTPG_RX_CFG                0x0102
#define PKTPG_RX_EVENT              0x0124
#define PKTPG_RX_CTL                0x0104
#define PKTPG_TX_CFG                0x0106
#define PKTPG_TX_EVENT              0x0128
#define PKTPG_TX_CMD_ST             0x0108
#define PKTPG_BUF_CFG               0x010a
#define PKTPG_BUF_EVENT             0x012c
#define PKTPG_RX_MISS               0x0130
#define PKTPG_TX_COL                0x0132
#define PKTPG_LINE_CTL              0x0112
#define PKTPG_LINE_ST               0x0134
#define PKTPG_SELF_CTL              0x0114
#define PKTPG_SELF_ST               0x0136
#define PKTPG_BUS_CTL               0x0116
#define PKTPG_BUS_ST                0x0138
#define PKTPG_TEST_CTL              0x0118
#define PKTPG_AUI_TIME_DOMAIN_REF   0x013c

/*
 * Initiate transmit registers
 */
#define PKTPG_TX_CMD_REQ            0x0144
#define PKTPG_TX_LENGTH             0x0146

/*
 * Address filter registers
 */
#define PKTPG_LOGICAL_ADDR_FILTER   0x0150
#define PKTPG_INDIVISUAL_ADDR       0x0158

/*
 * Frame locations
 */
#define PKTPG_RX_STATUS             0x0400
#define PKTPG_RX_LENGTH             0x0402
#define PKTPG_RX_FRAME              0x0404
#define PKTPG_TX_FRAME              0x0a00

/*
 * Bit masks
 */
#define SELF_CTL_RESET              0x0040
#define SELF_CTL_LOW_BITS           0x0015

#define SELF_ST_INITD               0x0080
#define SELF_ST_SIBUSY              0x0100

#define BUS_ST_TX_BID_ERR           0x0080
#define BUS_ST_RDY_4_TX_NOW         0x0100

#define BUS_CTL_MEMORY_E            0x0400
#define BUS_CTL_ENABLE_IRQ          0x8000
#define BUS_CTL_IOCHRDYE            0x1000
#define BUS_CTL_LOW_BITS            0x0017

#define TX_CMD_START_5              0x0000
#define TX_CMD_START_381            0x0040
#define TX_CMD_START_1021           0x0080
#define TX_CMD_START_ALL            0x00c0
#define TX_CMD_FORCE                0x0100
#define TX_CMD_ONECOLL              0x0200
#define TX_CMD_NO_CRC               0x1000
#define TX_CMD_NO_PAD               0x2000
#define TX_CMD_LOW_BITS             0x0009

#define ISQ_REG_NUM                 0x003f
#define ISQ_REG_CONTENT             0xffc0

#define RX_EVENT_RX_OK              0x0100
#define RX_EVENT_HASHED             0x0200
#define RX_EVENT_IND_ADDR           0x0400
#define RX_EVENT_BROADCAST          0x0800

#define RX_CTL_RX_OK_A              0x0100
#define RX_CTL_IND_ADDR_A           0x0400
#define RX_CTL_BROADCAST_A          0x0800
#define RX_CTL_LOW_BITS             0x0005

#define RX_CFG_SKIP                 0x0040
#define RX_CFG_RX_OK_I_E            0x0100
#define RX_CFG_LOW_BITS             0x0003

#define BUF_EVENT_RDY_4_TX          0x0100

#define BUF_CFG_RDY_4_TX_I_E        0x0100
#define BUF_CFG_LOW_BITS            0x000b

#define TX_CFG_LOSS_OF_CRC_I_E      0x0040
#define TX_CFG_SQE_ERROR_I_E        0x0080
#define TX_CFG_TX_OK_I_E            0x0100
#define TX_CFG_OUT_OF_WINDOW_I_E    0x0200
#define TX_CFG_JABBER_I_E           0x0400
#define TX_CFG_ANYCOLL_I_E          0x0800
#define TX_CFG_16_COLL_I_E          0x8000
#define TX_CFG_ALL                  0x8fc0
#define TX_CFG_LOW_BITS             0x0007

#define TX_EVENT_LOSS_OF_CRS        0x0040
#define TX_EVENT_SQE_ERROR          0x0080
#define TX_EVENT_TX_OK              0x0100
#define TX_EVENT_OUT_OF_WINDOW      0x0200
#define TX_EVENT_JABBER             0x0400
#define TX_EVENT_NUM_TX_COLL        0x7800
#define TX_EVENT_16_COLL            0x8000

/* One of the values of 0, 1, 2, 3 */
#define INTERRUPT_NUMBER            0x0000

#define LINE_CTL_10_BASE_T          0x0000
#define LINE_CTL_AUI_ONLY           0x0100
#define LINE_CTL_RX_ON              0x0040
#define LINE_CTL_TX_ON              0x0080
#define LINE_CTL_MOD_BACKOFF        0x0800
#define LINE_CTL_LOW_BITS           0x0013

#define LINE_ST_LINK_OK             0x0080
#define LINE_ST_AUI                 0x0100
#define LINE_ST_10BT                0x0200
#define LINE_ST_POLARITY_OK         0x1000
#define LINE_ST_CRS                 0x4000

#define TEST_CTL_FDX                0x4000
#define TEST_CTL_LOW_BITS           0x0019

/*
 * Register numbers
 */
#define REG_NUM_RX_EVENT            0x0004
#define REG_NUM_TX_EVENT            0x0008
#define REG_NUM_BUF_EVENT           0x000c
#define REG_NUM_RX_MISS             0x0010
#define REG_NUM_TX_COL              0x0012


/*
 * I/O mode register mapping
 */
#define IO_RX_TX_DATA_0             0x0000
#define IO_RX_TX_DATA_1             0x0002
#define IO_TX_CMD                   0x0004
#define IO_TX_LENGTH                0x0006
#define IO_ISQ                      0x0008
#define IO_PACKET_PAGE_POINTER      0x000A
#define IO_PACKET_PAGE_DATA_0       0x000C
#define IO_PACKET_PAGE_DATA_1       0x000E


		/* CS8900 signature read from PacketPage Pointer port at reset	*/
#define CS8900_SIGNATURE			0x3000
#define CS8900_EISA_NUMBER			0x630e		/* CS8900 EISA number	*/
#define CS8900_PRDCT_ID				0x0000		/* CS8900 product ID	*/
#define CS8900_PRDCT_ID_MASK		0xe0ff


#ifdef __cplusplus
extern "C" {
#endif

void CS8900_Test(void);


#ifdef __cplusplus
}
#endif

#endif /* __CS8900_H__ */




