#ifndef __K9K2G16_H__
#define __K9K2G16_H__


////////////////////////////// 16-bit ////////////////////////////////
// Main function
void Test_K9K2G16(void);
void NF16_Program(void);

// Sub function

void Test_NF16_Rw(void);
void Print16SubMessage(void);
void Test_NF16_Page_Write(void);
void Test_NF16_Page_Read(void);
void Test_NF16_Block_Erase(void);
void Test_NF16_Lock(void);
void Test_NF16_SoftUnLock(void);


int NF16_CheckECC(void);

void NF16_PrintBadBlockNum(void);


//*************** H/W dependent functions ***************
void NF16_Print_Id(void);
static U32 NF16_CheckId(void);
static int NF16_EraseBlock(U32 blockNum);
static int NF16_ReadPage(U32 block,U32 page,U32 *buffer);
static int NF16_WritePage(U32 block,U32 page,U32 *buffer);
static int NF16_IsBadBlock(U32 block);
static int NF16_MarkBadBlock(U32 block);
void K9K2G16_PrintBadBlockNum(void);
void InputTargetBlock16(void);

static void NF16_Init(void);


void Wr_Row_Addr(U32 blockPage);

//*******************************************************


#endif /*__K9K2G16_H__*/
