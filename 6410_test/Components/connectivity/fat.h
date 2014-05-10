//====================================================================
// File Name : fat32.h
// Function  : S3C2410 FAT File System Test Program Head file
// Program   : Kang, Sung Tae
// Date      : Dec. 26, 2003
// Version   : 0.0
// History
//   0.0 : Programming start (Dec. 26, 2003) -> YS
//====================================================================


#ifndef __FAT_H__
#define __FAT_H__

#include "sdhc.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SECBUFFSIZE 512
#define MAXSEC_PER_CLUSTER 64//8 // 64 ~512 fat16 format 512->fat32 format(4) 
#define DDEMAXSIZE 1024// It should be MAXSEC_PER_CLUSTER*SECBUFFSIZE/32=1024
#define FIRST_DATA_CLUSTER 2
#define SECMAXSIZE 1 // dboy060509 only use root folder
#define MAXDIRDEPTH 8
#define DIRCLUSTER  64
#define SF_BASE_LENGTH 8
#define SF_EXT_LENGTH 3
#define SF_NAME_LENGTH (8+3)
#define DIR_ENTRY_SIZE 32
#define SDCARD_OFFSET_SECTOR 0
#define MAX_FILE_LENGTH 256 // wms 060510

typedef struct
{
	u32  size;
	u32  addr;
	u8   ucAttr;
	char filename[11];
} ISOFILE;

typedef struct
{
	char filename[MAX_FILE_LENGTH]; // File or (sub)directory name
	u8   dirflag; // 0x10: directory mask, 0x20: file mask*/
	int  location; // Logical location
	int  nFileSz; // Data length
} DISC_DIR_ENTRY;

typedef struct
{
	u32 uDiskSize;
	u8  uSecPerClusVal;
} DSKSZTOSECPERCLUS;

#if 0 // khlee
struct HDFHANDLE
{
	int m_nPosi; // fseek position
	int m_nCluster; // current cluster
	u32 m_uFsize; // file size in byte
	int m_nHsector; // file nHead position
	short m_sHoffset;
#ifdef WRITE_CLUSTER
	short m_sClusOffset; // byte offset in cluster
	u8* m_pClusBuff; // cluster buff, one cluster size
#endif
	u8  m_ucClusterNeed; // need a new cluster if 1
	u8  m_ucAttr; // file attribute
	u8  m_ucState; // 0-closed; 1-open; 2-writing
	u8  m_ucNerror; // m_ucNerror infomation m_ucState
		// please keep m_ucBuffer address start from even byte position !!!
	u8  m_ucBuffer[512]; // sector buff, one sector size
	int m_nNclusters; // # of clusters in the file
	int m_nHeadCluster; // nHead cluster
	u8  m_ucMode; // m_ucMode used in HD_fopen
} ;
#endif

typedef struct {
	// Boot Sector and BPB Structure (p.9)
	u16 m_usBpbBytsPerSec;
	u8  m_ucBpbSecPerClus;
	u16 m_usBpbRsvdSecCnt;
	u16 m_usBpbRootEntCnt;
	u8  m_ucBpbMedia;
	u32 m_uBpbHiddSec;
	u32 m_uBpbTotSectors;
	u8  m_cardType;

	// FAT32 Structure Starting at Offset 36
	u32  m_uBpbRootClus;
	u8   m_ucBpbFsInfo;
	int  m_unBadCluster;
	int  m_nMaskCluster;
	int  m_nEOC;
	int  m_nFirstDataSector;
	int  m_nBpbCountOfClusters;
	int  m_nFatBuffSize;

	u8  *m_pFatBuffStart;
	u8  *m_pFatBuffer;
	u8  *m_pFsInfo;
	u8  *m_pBufferFileMem;
	u8  *m_pBufferDirMem;
	u8  *m_pBufferSearchMem;

	// FAT32 FSInfo Sector Structure and Backup Boot Sector (pp.21-22)
	u32 m_uFsiFreeCount;
	u32 m_uFsiNxtFree;
	u32 m_uFsiLeadSig;

	// FAT Directory Structure
	// in FAT32,
	// byte Directory Entry Structure (p.23~25)
	u8  m_ucFsfname[SF_NAME_LENGTH+1];
	u8  m_ucFileEntryHead[DIR_ENTRY_SIZE];
	int  m_nFat32FileTime;
	int  m_nFat32FileDate;

	// For long file name processing
	int m_nNthEntry;
	int m_nOrphanCount;
	short m_sClfName[260];
	short m_sUdLFname[260];

	int m_nDirBuffSize;
	int m_nTransBuffSize;
	int m_nPosFat;
	int m_nPosFatb;
	int m_nPosRoot;
	int m_nFatSz;
	int m_nBytesPerCluster;
	int m_uDirEntryPerCulster;
	int m_nDWordPerCluster;
	int m_nDWordPerSector;
//	u8  m_pSecBuff[SECMAXSIZE][SECBUFFSIZE*MAXSEC_PER_CLUSTER];
//	DISC_DIR_ENTRY m_pDDE[DDEMAXSIZE];
	u8 *m_pSecBuff[SECMAXSIZE];
	DISC_DIR_ENTRY *m_pDDE;

	u32  m_nDirCluster[DIRCLUSTER];
	u32  m_uDirClusterNum;
	// FAT Long Directory Entry Structure (p.26~27)
	u8  m_ucLDirOrd;
	u8  m_ucLDirAttr;
	u16 m_usRootDirSectors;
	int m_nBeforeIndex[MAXDIRDEPTH];
//	u8  m_ucBufferSec[SECBUFFSIZE*MAXSEC_PER_CLUSTER]; //dboy 060509
	char* m_cpEntryName;
	int m_nLastFreeCluster;
	int m_nLastFatChanged;
	int m_nLastFatSector;
	u32 m_uFirstSectorAfterMbr;//kst060404

	int m_nTwd; // initialize 0 in boot_setup()
	int m_nPwd;
	int m_nDepthNow;
	int m_nFileIndex;
//	int p;
} FAT;

u8 FAT_LoadFileSystem(SDHC *sCh);
void FAT_UnloadFileSystem(SDHC *sCh);

u8 FAT_ReadFile1(s32 nIdx, u32 uDestAddr, SDHC *sCh);
u8 FAT_ReadFile2(s32 nIdx, u32 uDstAddr, u32* uFileSize, SDHC *sCh);
u8 FAT_ReadFile3(const char* pFileName, u32 uDstAddr, u32* uFileSize, SDHC *sCh);
u8 FAT_ReadFile4(s32 nDirIdx, u32 nBlockPos, u32 nBlocks, u32 uDestAddr, SDHC *sCh);
u8 FAT_ReadFile5(const char* pFileName, u32 nBlockPos, u32 nBlocks, u32 uDestAddr, SDHC *sCh);
u8 FAT_ReadFile6(s32 nDirIdx, u32 uFileOffset, u32 uBytes, u32 uDestAddr, u32* uReturnAddr, SDHC *sCh);
u8 FAT_WriteFile(const char *pFileName, s32 nLen, u32 uSrcAddr, SDHC *sCh);

u8 FAT_StartReadingBlocks(u32 uStBlock, u32 uBlocks, u32 uBufAddr, SDHC *sCh);
u8 FAT_StartWritingBlocks(u32 uStBlock, u32 uBlocks, u32 uBufAddr, SDHC *sCh);
	
void FAT_GetFileIndex(const char* s, s32* nFIndex, SDHC *sCh);
u8 FAT_GetFileIndex1(const char *pFileName, const char *pFileExt, u32* uIdx, SDHC *sCh);
//void FAT_GetFileName(s32 index, const char* fileName, SDHC *sCh);
void FAT_GetFileName(s32 index, char* fileName, SDHC *sCh);
//void FAT_GetFileExt(s32 index, const char* fileExt, SDHC *sCh);
void FAT_GetFileExt(s32 index, char* fileExt, SDHC *sCh); // khlee
void FAT_GetFileSize(s32 index, s32* nFileSize, SDHC *sCh);
void FAT_GetTotalNumOfFiles(u32* uNumOfFiles, SDHC *sCh);
void FAT_GetNumOfFilesAndDirs(u32* uCount, SDHC *sCh);

//private:
	
void FAT_ReadFileSystemInfo(SDHC *sCh);
void FAT_Init(SDHC *sCh);
u8 FAT_InitHardDisk(SDHC *sCh);
u32  FAT_ReadFile(s32 nIdx, u8* p_uDestAddr, SDHC *sCh);
void FAT_CompareFileNames(const char* s, const char* t, int* nFileName, SDHC *sCh);
void FAT_WriteSFile(char *fileName, int nLength, u8* srcAddr, int* nWrFile, SDHC *sCh);
void FAT_File_Delete(char* fileName, int* nFileDel, SDHC *sCh);

u8 FAT_CopyArray(u8* a, const u8* b, u32 n, SDHC *sCh);
void FAT_CompareArray(u8* a, const u8* b, u32 n, int* nComArray, SDHC *sCh);

void FAT_ComputeChkSum (u8 *pFcbName, u8* uSum, SDHC *sCh);
void FAT_ReadSector(u8 *dest, int lba, int nblocks, SDHC *sCh);
void FAT_WriteSector(u8 *src, int lba, int nblocks, SDHC *sCh);
void FAT_FromClusterToLba(int cluster, int* nLba, SDHC *sCh);
void FAT_CollectLFileName(short *s, u8 *p, int* nLfname, SDHC *sCh);
void FAT_PassoutLFileName(short *s, u8* cFname, int nOrder, int* nDirecEntry, SDHC *sCh);
void FAT_CheckOrphan(u8 *p, int* nOrphan, SDHC *sCh);
void FAT_GenerateSNameFromLName(short *s, char *p, int* nFlag, SDHC *sCh);
void FAT_GenerateNumericTail(char *p, int n, SDHC *sCh);
void FAT_FormatSFileName(u8 *fname, char *name, int* nSfnameconv, SDHC *sCh);
void FAT_Compare2LFileName(u32 slength, short *s, char *p, int* nChkmatch, SDHC *sCh);
void FAT_GetUnicodeStrLen(short *s, int* nUniChr, SDHC *sCh);
void FAT_ConvertUnicode(short *s, char *p, int direct, int* nConvUni, SDHC *sCh);
void FAT_Compare2UnicodeLFileName(short *s1, short *s2, int* nComLFName, SDHC *sCh);
void FAT_CopyUnicodeLFileName(short *s1, short *s2, int* nCoLname, SDHC *sCh);
// in a directory entry
void FAT_GetEntryCluster(u8 *p, u32* nEntCluster, SDHC *sCh);
void FAT_SetEntryCluster(u8 *p, u32 cluster, SDHC *sCh);
void FAT_SetDateAndTime(u8 *p, SDHC *sCh);
void FAT_GenerateDirEntry(int tag, int cluster, u8** ucSpecEnt, SDHC *sCh);

void FAT_CopyEntry(u8* p, u32 x, SDHC *sCh);
void FAT_CopyAblockOfMem(u8 *target, u8 *source, int bytes, SDHC *sCh);
void FAT_UnifySector(int *sector, int *offset, int sector1, int offset1, SDHC *sCh);
// Update the file data cluster in fat
void FAT_UpdateFileDataCluInFat(int numberofcluster, int* uAlloCluster, SDHC *sCh);
// Update the root cluster in fat
void FAT_UpdateRootCluInFat(int nNextClu, SDHC *sCh);

void FAT_GetNextCluster(int cluster, int* nNextClu, SDHC *sCh);
void FAT_SetFat(int offset, u32 fat_entry, u8*p, SDHC *sCh);
void FAT_WriteFatContent(int* nFatContent, SDHC *sCh);
void FAT_ClusterFatAddr(int cluster, int *sector, int *offset, SDHC *sCh);
void FAT_GetFat(int sector, int offset, int *fat_entry, int* nFatEnt, SDHC *sCh);

void FAT_AppendCluster(int cluster, int nextcluster, int* nAnoChain, SDHC *sCh);
void FAT_WriteFileHead(const char *name, u8 ucAttr, u32 nLength, u32 cluster, int* nFileEnt, SDHC *sCh);
void FAT_WriteSFFileHead(const char *name, u8 ucAttr, u32 nLength, u32 cluster, int* nFileEnt, SDHC *sCh);

void FAT_ReleaseClusterChain(int headcluster, int nLength, int* nRelCluster, SDHC *sCh);
void FAT_AllocateClusterChain(int ucMode, int *numberofcluster, int nPosi,
	int rstart, int rend, int* uAlloCluster, SDHC *sCh);
void FAT_SearchDirFileEntry(int location, short *lfname, int entry_num, u8 ucAttr,
	ISOFILE *pfile, int *sector, int *offset, int *clus, int* nSpecFile, SDHC *sCh);
void FAT_CreateFile(int location, const char *cSfname, u8 ucAttr, int nLength, int* nEmpFile, SDHC *sCh);

void FAT_ScanCluster(int frag, int nLength, int *nPosi, int rstart, int rend, u32* uCluBlk, SDHC *sCh);
void FAT_CreateSFile(int location, const char *lfname, u8 ucAttr, int nLength, u8 *data, int* nSFile, SDHC *sCh);
void FAT_CreateDirectory(int location, char *dirname, int* nCrFile, SDHC *sCh);
void FAT_DeleteFile(char *cSfname, u8 ucAttr, int* nDelFile, SDHC *sCh);
void FAT_FileCreateCluster(int location, short *lfname, ISOFILE *oldpf, int* nFCluser, SDHC *sCh);
void FAT_RenameFile(int location, short *nlfname, short *lfname, u8 ucAttr, int* nReFile, SDHC *sCh);
void FAT_CopyFile(short *lfname, short *lfname1, int* nCopFile, SDHC *sCh);
void FAT_ConvChNameToUniName(char *p, short** spUnicode, SDHC *sCh);
void FAT_QuickFormat(SDHC* sCh);

void FAT_InitBuff(SDHC *sCh);
void FAT_SetupBoot(u8 *buff, SDHC *sCh);
void FAT_GetFSInfo(SDHC *sCh);
void FAT_SetFSInfo(SDHC *sCh);
void FAT_NextDirSector(int where, int* nNexDiSec, SDHC *sCh);

void FAT_ReadDirFat32(int where, SDHC *sCh);

void FAT_ConvertToDiscEntryFormat(const char* s, char* cpDiscEntFormat, SDHC *sCh);
void FAT_CompareFName(const char* s, char* t, int* nFname, SDHC *sCh);

void FAT_ShortFileNameArray(DISC_DIR_ENTRY* da,  char* pShortFileName, SDHC *sCh);
void FAT_DisplayFileNames(SDHC *sCh);
void FAT_InitializeSecBuff(SDHC *sCh);
void FAT_InitializeBeforeIndex(SDHC *sCh);
void FAT_InitializeDiscEntry(SDHC *sCh);
void FAT_InitalizeDirCluster(SDHC *sCh);	// sunny 060818
int  FAT_FindClusterInDirectory(int* idxCluster, int* nLongDirStatus, SDHC *sCh);
void FAT_FindBlankSecBuff(int* nBSecBuff, SDHC *sCh);
void FAT_FindSecBuffIndex(int idxCluster, int* nSecBuffIndex, SDHC *sCh);
void FAT_PushIndex(int index, SDHC *sCh);
void FAT_PopIndex(int* nIndex, SDHC *sCh);
void FAT_FindFreeEntry(u8 ucEntries, int* nCluster, int* nOffset, SDHC *sCh);	// sunny 060818
void FAT_FindFreeEntryOffsetInDirEntry(int idx, int* nFreeEntryOffset, SDHC *sCh);
void FAT_FindMaxLocation(int endIndex, int* nDdeMaxindex, SDHC *sCh);
void FAT_ChangeDir(int hcluster, int ucMode, int* nChgDir, SDHC *sCh);
void FAT_FindClusterFromName(char* s, u32* uClusterName, SDHC *sCh);
void FAT_FindDirEntryClusterFromName(char* s, u32* uEntCluster, SDHC *sCh);
u8 FAT_FindDirEntryOffsetFromName(const char* s, int* nEntOffset, SDHC *sCh);
void FAT_InitializeEntry(SDHC *sCh);

void FAT_InitBuffFileMem(u8** ucpInitBufferFile, SDHC *sCh); // LYS 040407
void FAT_InitBuffDirMem(u8** ucpInitBufferDir, SDHC *sCh); // LYS 040407
void FAT_InitBuffSearchMem(u8** ucpInitBufferSearch, SDHC *sCh); // LYS 040407
void FAT_CopyFatTableToBackup(SDHC *sCh); // LYS 040426
void FAT_SetDateTime(int date, int time, SDHC *sCh);
void FAT_ower2Capital(char* s, SDHC *sCh); //LYS 040531

void FAT_AppendProcess(u8 *buffer, int sector, int termination, SDHC *sCh);
void FAT_ProcessTerm(u8 *nPosi, u8 *ucBuffer, int sector, int num, int* nNextEnt, SDHC *sCh);
void FAT_ProcessLfname(short *lfname, u8 *nPosi, u8 *ucBuffer,
	int sector, int *pcsum, int* nProLFname, SDHC *sCh);
void FAT_MatchProcess(int match, int checksum, u8 *p, ISOFILE *pfile, int* nMatEnt, SDHC *sCh);
void FAT_AttributeFile(u8 *p, int* nAttriFile, SDHC *sCh);
void FAT_BootRealloc(u8* buff, SDHC *sCh);

#ifdef __cplusplus
}
#endif

#endif // __FAT_H__

