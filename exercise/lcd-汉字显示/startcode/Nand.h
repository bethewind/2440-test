#ifndef __NAND_H
#define __NAND_H

#define NUM_BLOCKS			0x1000	//  64 MB Smartmedia card.
#define SECTOR_SIZE			512
#define SPARE_SIZE			16
#define PAGES_PER_BLOCK			32

//  For flash chip that is bigger than 32 MB, we need to have 4 step address
//  
#define NFCONF_INIT			0xF830  // 512-byte 4 Step Address
#define NEED_EXT_ADDR			1
//#define NFCONF_INIT			0xA830  // 256-byte 4 Step Address
//#define NEED_EXT_ADDR			0

//#define NFCONF_INIT			0xF840

//  NAND Flash Command. only for K9F1208UOM

#define CMD_READ1			0x00	//  Read
//#define CMD_READ1			0x01	//  Read1
//#define CMD_READ2			0x50	//  Read2
#define CMD_READ2			0x30	//  Read3---���������K9F2G08UOB���Եģ�ҳ��û�з�Ϊ�����֣����������ָ��ɣ�
#define CMD_READID			0x90	//  ReadID
#define CMD_WRITE1			0x80	//  Write phase 1
#define CMD_WRITE2			0x10	//  Write phase 2
#define CMD_ERASE1			0x60	//  Erase phase 1
#define CMD_ERASE2			0xd0	//  Erase phase 2
#define CMD_STATUS			0x70	//  Status read
#define CMD_RESET			0xff	//  Reset
#define CMD_RANDOMREAD1		0x05 	//�������������1
#define CMD_RANDOMREAD2 	0xE0 	//�������������2
#define CMD_RANDOMWRITE 	0x85 	//����д����

/*
//********NAND FLASH CMMOND especially for K9F2G08UMB
#define CMD_READ1 0x00 //ҳ����������1
#define CMD_READ2 0x30 //ҳ����������2
#define CMD_READID 0x90 //��ID����
#define CMD_WRITE1 0x80 //ҳд��������1
#define CMD_WRITE2 0x10 //ҳд��������2
#define CMD_ERASE1 0x60 //�������������1
#define CMD_ERASE2 0xd0 //�������������2
#define CMD_STATUS 0x70 //��״̬����
#define CMD_RESET 0xff //��λ
#define CMD_RANDOMREAD1 0x05 //�������������1
#define CMD_RANDOMREAD2 0xE0 //�������������2
#define CMD_RANDOMWRITE 0x85 //����д����

*/


//  Status bit pattern
#define STATUS_READY			0x40	//  Ready
#define STATUS_ERROR			0x01	//  Error

//  Status bit pattern
#define STATUS_READY			0x40
#define STATUS_ERROR			0x01

#define NF_Send_Cmd(cmd)	{rNFCMD  = (cmd); }
#define NF_Send_Addr(addr)	{rNFADDR = (addr); }
#define NF_Send_Data(data)	{rNFDATA8 = (data); }	
#define NF_Enable()			{rNFCONT &= ~(1<<1); }           //nand flash������ʹ��
#define NF_Disable()		{rNFCONT |= (1<<1); }
#define NF_Enable_RB()		{rNFSTAT |= (1<<2); }         //����RnB����ģʽ��
#define NF_Check_Busy()		{while(!(rNFSTAT&(1<<2)));}  //�൱�ڵȴ�RnB��1----��˵��nand flash��æ��
#define NF_Read_Byte()		(rNFDATA8)


#define NF_RSTECC()			{rNFCONT |= (1<<4); }
#define NF_RDMECC()			(rNFMECC0 )
#define NF_RDSECC()			(rNFSECC )
#define NF_RDDATA()			(rNFDATA)

#define NF_WAITRB()			{while(!(rNFSTAT&(1<<0)));} 
#define NF_MECC_UnLock()	{rNFCONT &= ~(1<<5); }
#define NF_MECC_Lock()		{rNFCONT |= (1<<5); }
#define NF_SECC_UnLock()	{rNFCONT &= ~(1<<6); }
#define NF_SECC_Lock()		{rNFCONT |= (1<<6); }

#define	RdNFDat8()			(rNFDATA8)	//byte access
#define	RdNFDat()			RdNFDat8()	//for 8 bit nand flash, use byte access
#define	WrNFDat8(dat)		(rNFDATA8 = (dat))	//byte access
#define	WrNFDat(dat)		WrNFDat8()	//for 8 bit nand flash, use byte access

#define pNFCONF				rNFCONF 
#define pNFCMD				rNFCMD  
#define pNFADDR				rNFADDR 
#define pNFDATA				rNFDATA 
#define pNFSTAT				rNFSTAT 
#define pNFECC				rNFECC0  

//#define NF_CE_L()			NF_nFCE_L()
//#define NF_CE_H()			NF_nFCE_H()
#define NF_DATA_R()			rNFDATA
#define NF_ECC()			rNFECC0

// HCLK=100Mhz
#define TACLS				1	// 1-clk(0ns) 
#define TWRPH0				4	// 3-clk(25ns)
#define TWRPH1				0	// 1-clk(10ns)  //TACLS+TWRPH0+TWRPH1>=50ns




extern char rNF_ReadID() ;
extern void NF_Init(void) ;
extern void NF_ReadPage(unsigned int block,unsigned int page, unsigned char * dstaddr) ;
extern void LB_ReadPage(unsigned int addr, unsigned char * dstaddr) ;//��γ������ڣ�Nand Flashÿҳ��С��2048���ֽ�
extern void NF_WritePage(unsigned int block,unsigned int page, unsigned char *buffer) ;
extern int  NF_EraseBlock(unsigned int block) ;//
extern unsigned char NF_RamdomRead(unsigned int block,unsigned int page,unsigned int add);
extern unsigned char NF_RamdomWrite(unsigned int block,unsigned int page,unsigned int add,unsigned char dat) ;

//extern void rRdNF2SDRAM(void) ;




#endif /*__NAND_H__*/
