/*
 support 512 page and 2048 page NAND Flash
*/
//#include <string.h>

//#include "def.h"
#include "2440addr.h"
#include "nand.h"


static void NF_Reset()
{
	NF_Enable();
	NF_Enable_RB();
	NF_Send_Cmd(CMD_RESET);  
	NF_Check_Busy();
	NF_Disable();
}


void NF_Init(void)
{
	rGPACON &= ~(0X3F << 17) ;
	rGPACON |= (0X3F << 17) ;
	rNFCONF = (TACLS<<12)|(TWRPH0<<8)|(TWRPH1<<4);
	rNFCONT = (0<<12)|(1<<0);
	rNFSTAT = 0;
	NF_Reset();
}
void NF_ReadID(unsigned char *buf)
{
	/*char	pMID;
	char	pDID;
	char	nBuff;
	char	n4thcycle;
	char    n5thcycle ;*/
	int	i;

	NF_Enable();    
	NF_Enable_RB();
	NF_Send_Cmd(CMD_READID);	// read id command
	NF_Send_Addr(0x0);
	for ( i = 0; i < 100; i++ );
								
    *buf 		= NF_Read_Byte();
	*(buf+1)	= NF_Read_Byte();
	*(buf+2)    = NF_Read_Byte();
	*(buf+3)	= NF_Read_Byte();
	*(buf+4)	= NF_Read_Byte();
	NF_Disable();

	//return (pDID);
}

char rNF_ReadID()
{
	char	pMID;
	char	pDID;
	char	nBuff;
	char	n4thcycle;
	char    n5thcycle ;
	int	i;

	NF_Enable();    
	NF_Enable_RB();
	NF_Send_Cmd(CMD_READID);	
	NF_Send_Addr(0x0);
	for ( i = 0; i < 100; i++ );
	pMID 		= NF_Read_Byte();
	pDID		= NF_Read_Byte();
	nBuff     	= NF_Read_Byte();
	n4thcycle	= NF_Read_Byte();
	NF_Disable();

	return (pDID);
}

static void SB_ReadPage(unsigned int addr, unsigned char * to)//��γ������ڣ�Nand Flashÿҳ��С��512���ֽ�
{
	unsigned int i;

	NF_Reset();

	//  Enable the chip
	NF_Enable();
	NF_Enable_RB();

	// Issue Read command
	NF_Send_Cmd(CMD_READ1);

	//  Set up address
	NF_Send_Addr(0x00);
	NF_Send_Addr((addr) & 0xff);
	NF_Send_Addr((addr >> 8) & 0xff);
	NF_Send_Addr((addr >> 16) & 0xff);


	NF_Check_Busy();		// wait tR(max 12us)

	for (i = 0; i < 512; i++)
	{
		to[i] =  NF_Read_Byte();
	}

	NF_Disable();

}

void LB_ReadPage(unsigned int addr, unsigned char * dstaddr)//��γ������ڣ�Nand Flashÿҳ��С��2048���ֽ�
{
	unsigned int i;												//����û���õ�ECCУ�飻ֻ�Ǽ򵥵Ķ�ȡ����

	NF_Reset();										//�����addrʵ���Ͼ���ҳ��

	//  Enable the chip
	NF_Enable();   
	NF_Enable_RB();

	// Issue Read command
	NF_Send_Cmd(CMD_READ1);//CMD_READ1= 0x00
								//  Set up address
	NF_Send_Addr(0x00);				//��ҳ��ȡ�Ļ��е�ַ��Ϊ0���ɣ�
	NF_Send_Addr(0x00);
	NF_Send_Addr((addr) & 0xff);
	NF_Send_Addr((addr >> 8) & 0xff);
	NF_Send_Addr((addr >> 16) & 0x1);

	NF_Send_Cmd(CMD_READ2);//CMD_READ12= 0x30

	NF_Check_Busy();		// wait tR(max 12us)

	for (i = 0; i < 2048; i++)
	{
		dstaddr[i] =  NF_Read_Byte();
	}

	NF_Disable();

}

void NF_ReadPage(unsigned int block,unsigned int page, unsigned char * dstaddr)//��γ������ڣ�Nand Flashÿҳ��С��2048���ֽ�
{
	unsigned int i;												//����û���õ�ECCУ�飻ֻ�Ǽ򵥵Ķ�ȡ����
    unsigned int blockPage = (block<<6)+page;
	NF_Reset();										//�����addrʵ���Ͼ���ҳ��

	NF_Enable();   
	NF_Enable_RB();

	
	NF_Send_Cmd(CMD_READ1);		//CMD_READ1= 0x00
								
	NF_Send_Addr(0x00);				
	NF_Send_Addr(0x00);
	NF_Send_Addr((blockPage) & 0xff);
	NF_Send_Addr((blockPage >> 8) & 0xff);
	NF_Send_Addr((blockPage >> 16) & 0x1);

	NF_Send_Cmd(CMD_READ2);   //CMD_READ12= 0x30

	NF_Check_Busy();		

	for (i = 0; i < 2048; i++)
	{
		dstaddr[i] =  NF_Read_Byte();
	}

	NF_Disable();

}




void NF_WritePage(unsigned int block,unsigned int page, unsigned char *buffer)
{
	unsigned int i;
	unsigned int blockPage = (block<<6)+page;
	unsigned char *bufPt = buffer;
	
	NF_Reset();										//�����addrʵ���Ͼ���ҳ��
	NF_Enable();   //������ʹ�� 
	NF_Enable_RB(); //����RnB����ģʽ
	
	NF_Send_Cmd(CMD_WRITE1); /* д��һ������ */
	
	NF_Send_Addr(0x00);				//��ҳд��Ļ��е�ַ��Ϊ0���ɣ�
	NF_Send_Addr(0x00);
	NF_Send_Addr((blockPage) & 0xff);
	NF_Send_Addr((blockPage >> 8) & 0xff);
	NF_Send_Addr((blockPage >> 16) & 0x1);
	for(i=0;i<2048;i++)
	{
		NF_Send_Data(*bufPt++); /* дһ��ҳ512�ֽڵ�Nand FlashоƬ */
	}
	
	NF_Send_Cmd(CMD_WRITE2);
	NF_Check_Busy();	
	NF_Disable(); 
}
int NF_EraseBlock(unsigned int block)
{
	unsigned int blocknum=(block<<6);
	int i;
	NF_Reset();
	NF_Enable(); 
	NF_Enable_RB();	
	NF_Send_Cmd(CMD_ERASE1);
	
	NF_Send_Addr( blocknum & 0xff); 	
	NF_Send_Addr((blocknum>>8) & 0xff); 
	NF_Send_Addr((blocknum>>16) & 0xff); 
	
	NF_Send_Cmd(CMD_ERASE2) ; 
 	for (i = 0; i < 1000; i++) ;   
	
	NF_Check_Busy() ;	
	
	
	NF_Disable() ; 
	
	return 1 ;
	
}

unsigned char NF_ReadPage_ECC(unsigned int addr, unsigned char * dstaddr)
{
	unsigned int i,mecc0,secc;												//����û���õ�ECCУ�飻ֻ�Ǽ򵥵Ķ�ȡ����

	NF_Reset();										//�����addrʵ���Ͼ���ҳ��
	NF_MECC_UnLock() ;//����main��ECC
	//  Enable the chip
	NF_Enable();   
	NF_Enable_RB();

	// Issue Read command
	NF_Send_Cmd(CMD_READ1);//CMD_READ1= 0x00
								//  Set up address
	NF_Send_Addr(0x00);				//��ҳ��ȡ�Ļ��е�ַ��Ϊ0���ɣ�
	NF_Send_Addr(0x00);
	NF_Send_Addr((addr) & 0xff);
	NF_Send_Addr((addr >> 8) & 0xff);
	NF_Send_Addr((addr >> 16) & 0x1);

	NF_Send_Cmd(CMD_READ2);//CMD_READ12= 0x30

	NF_Check_Busy();		// wait tR(max 12us)

	for (i = 0; i < 2048; i++)
	{
		dstaddr[i] =  NF_Read_Byte();
	}
	
	NF_MECC_Lock() ; //����main��ECC
	NF_SECC_UnLock() ;
	
	mecc0 = NF_RDDATA() ;//����ط�ע��ǰ���Ѿ�����2047�ˣ�������Ŷ����Ǻ����������
	rNFMECCD0 = ((mecc0 & 0xff00) << 8) | (mecc0 & 0xff) ;//����rNFMECCD0��32λ�ģ�������ʱ��NAND FLASH��8λ��I/O�ӿڹ�ϵ������ֻ�õ���[7:0]��[23:16] ;
	rNFMECCD1 = ((mecc0 & 0xff000000) >> 8) | ((mecc0 & 0xff0000) >> 16) ; 
	
	NF_SECC_Lock() ;
	secc = NF_RDDATA() ;
	rNFSECCD = ((secc & 0xff00) << 8) | (secc & 0xff) ;
	
	NF_Disable();
	
	if((rNFESTAT0 & 0xf) == 0)
		return 0x66 ;//���У����ȷ���򷵻�0x66
	else
	    return 0x44 ; 
}


unsigned char NF_RamdomRead(unsigned int block, unsigned int page, unsigned int add)
{
	unsigned char buf ;
	unsigned int page_number = (block<<6) + page;
	NF_Reset();										
	NF_Enable(); 
	NF_Enable_RB(); 
	
	NF_Send_Cmd(CMD_READ1); 
	
								
	NF_Send_Addr(0x00); 
	NF_Send_Addr(0x00);
	NF_Send_Addr((page_number) & 0xff); 
	NF_Send_Addr((page_number >> 8) & 0xff); 
	NF_Send_Addr((page_number >> 16) & 0xff); 

	NF_Send_Cmd(CMD_READ2); //ҳ����������2:0x30
	
	NF_Check_Busy(); 

	NF_Send_Cmd(CMD_RANDOMREAD1); 
	
								
	NF_Send_Addr((char)(add&0xff)); 
	NF_Send_Addr((char)((add>>8)&0x0f)); 
	
	NF_Send_Cmd(CMD_RANDOMREAD2); 
	  

	NF_Check_Busy(); 

	buf = NF_Read_Byte() ;
	
	NF_Disable(); //�ر�Ƭѡ
	
	return buf; //��ȡ����
}


unsigned char NF_RamdomWrite(unsigned int block,unsigned int page, unsigned int add, unsigned char dat)
{
	unsigned int page_number = (block<<6)+page;
	NF_Enable(); 
	NF_Enable_RB(); 
	
	NF_Send_Cmd(CMD_WRITE1); 
		
	NF_Send_Addr(0x00); 
	NF_Send_Addr(0x00); 
	NF_Send_Addr((page_number) & 0xff); 
	NF_Send_Addr((page_number >> 8) & 0xff); 
	NF_Send_Addr((page_number >> 16) & 0xff); 
	
	NF_Send_Cmd(CMD_RANDOMWRITE); 
	
	
	NF_Send_Addr((char)(add&0xff)); 
	NF_Send_Addr((char)((add>>8)&0x0f)); 
	
	NF_Send_Data(dat); 
	
	NF_Send_Cmd(CMD_WRITE2); 
	
	
	NF_Check_Busy();	
	
	NF_Disable(); 

}
void RdNF2SDRAM(void)
{
	unsigned int i;
	unsigned int start_addr = 0x0;
	unsigned char * dstaddr = (unsigned char *)0x30000000; 
	unsigned int size = 0x100000;
	NF_Init();
	switch(rNF_ReadID())
	{
		case 0xD6:
			for(i = (start_addr >> 9); size > 0; )
			{
				SB_ReadPage(i, dstaddr);
				size -= 512;
				dstaddr += 512;
				i ++;
			}
			break;

		case 0xDA:
			for(i = (start_addr >> 11); size > 0;i ++ )
			{
				//LB_ReadPage(i, dstaddr);
				NF_ReadPage(i/64,i%64, dstaddr);
				size -= 2048;
				dstaddr += 2048;
				
			}
			break;
	}
}		


