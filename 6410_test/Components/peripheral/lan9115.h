#ifndef __LAN9115_H__
#define __LAN9115_H__


#ifdef __cplusplus
extern "C" {
#endif

#define	ChipID	0x0115

//System Control and Status Registers
#define 	ID_REV				(0x50)
#define 	HW_CFG				(0x74)
#define 	MAC_CSR_CMD		(0xA4)
#define 	MAC_CSR_DATA		(0xA8)
#define 	E2P_CMD			(0xB0)
#define 	E2P_DATA			(0xB4)

//CSR
#define	MAC_CR_IDX			(1)
#define	ADDRH_IDX			(2)
#define	ADDRL_IDX			(3)
#define	HASHH_IDX			(4)
#define	HASHL_IDX			(5)
#define	MII_ACC_IDX		(6)
#define	MII_DATA_IDX		(7)
#define	FLOW_IDX			(8)
#define	VLAN1_IDX			(9)
#define	VLAN2_IDX			(0xA)
#define	WUFF_IDX			(0xB)
#define	WUCSR_IDX			(0xC)

//EEPROM command
#define 	E2P_READ			(0x0<<28)
#define 	E2P_EWDS			(0x1<<28)
#define 	E2P_EWEN			(0x2<<28)
#define 	E2P_WRITE			(0x3<<28)
#define 	E2P_WRAL			(0x4<<28)
#define 	E2P_ERASE			(0x5<<28)
#define 	E2P_ERAL			(0x6<<28)
#define 	E2P_RELOAD			(0x7<<28)

#define 	E2P_MAC_ADDRH		(0x1234)
#define 	E2P_MAC_ADDRL		(0x56789ABC)
#define 	E2P_FIRST_BYTE		(0xA5)

typedef struct LAN9115_CSR
{
	u32 MAC_CR;
	u32 ADDRH;
	u32 ADDRL;
	u32 HASHH;
	u32 HASHL;
	u32 MII_ACC;
	u32 MII_DATA;
	u32 FLOW;
	u32 VLAN1;
	u32 VLAN2;
	u32 WUFF;
	u32 WUCSR;
}oLAN9115_CSR;


u32 smsc9115_reg_read(u32 reg);

#ifdef __cplusplus
}
#endif

#endif /* __CS8900_H__ */




