/*----------------------------------------------------------------------
 *
 * Filename: fat.cpp
 *
 * Contents:
 *
 * Contributors: Lee Yoonsu, Kang sungtae
 *
 * Copyright (c) 2006 SAMSUNG Electronics.
 *
 *----------------------------------------------------------------------
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "library.h"
#include "system.h"
#include "fat.h"

/*------------------------
 * FAT Data Structure
 *------------------------
 *
 * Boot Sector and BPB Structure (p.9)
 *------------------------------------
 *
 * m_usBpbBytsPerSec;
 *
 *   Count of bytes per sector.
 *   This value may take on only the following values: 512, 1024, 2048 or 4096.
 *   If maximum compatibility with old implementations is desired,
 *   only the value 512 should be used.
 *   There is a lot of FAT code in the world that is basically "hard wired" to 512 bytes per sector
 *   and doesn't bother to check this field to make sure it is 512.
 *   Microsoft operating systems will properly support 1024, 2048, and 4096.
 *   Note: Do not misinterpret these statements about maximum compatibility.
 *   If the media being recorded has a physical sector size N,
 *   you must use N and this must still be less than or equal to 4096.
 *   Maximum compatibility is achieved by only using media with specific sector sizes.
 *
 * m_ucBpbSecPerClus;
 *
 *   Number of sectors per allocation unit.
 *   This value must be a power of 2 that is greater than 0.
 *   The legal values are 1, 2, 4, 8, 16, 32, 64, and 128.
 *   Note however, that a value should never be used that results in
 *   a "bytes per cluster" value (m_usBpbBytsPerSec * m_ucBpbSecPerClus) greater than 32K (32 * 1024).
 *   There is a misconception that values greater than this are OK.
 *   Values that cause a cluster size greater than 32K bytes do not work properly; do not try to define one.
 *   Some versions of some systems allow 64K bytes per cluster value.
 *
 * m_usBpbRsvdSecCnt;
 *
 *   Number of reserved sectors in the Reserved region of the volume
 *   starting at the first sector of the volume.
 *   This field must not be 0.
 *   For FAT12 and FAT16 volumes, this value should never be anything
 *   other than 1. For FAT32 volumes, this value is typically 32.
 *   There is a lot of FAT code in the world "hard wired"
 *   to 1 reserved sector for FAT12 and FAT16 volumes and
 *   that doesn't bother to check this field to make sure it is 1.
 *   Microsoft operating systems will properly support any non-zero value in this field.
 *   Many application setup programs will not work correctly on such a FAT volume.
 *
 * m_usBpbRootEntCnt;
 *
 *   For FAT12 and FAT16 volumes,
 *   this field contains the count of 32-byte directory entries in the root directory.
 *   For FAT32 volumes, this field must be set to 0.
 *   For FAT12 and FAT16 volumes,
 *   this value should always specify a count that
 *   when multiplied by 32 results in an even multiple of m_usBpbBytsPerSec.
 *   For maximum compatibility, FAT16 volumes should use the value 512.
 *
 * USHORT BpbTotSec16;
 *
 *   This field is the old 16-bit total count of sectors on the volume.
 *   This count includes the count of all sectors in all four regions of the volume.
 *   This field can be 0; if it is 0, then BPB_TotSec32 must be non-zero.
 *   For FAT32 volumes, this field must be 0.
 *   For FAT12 and FAT16 volumes, this field contains the sector count,
 *   and BPB_TotSec32 is 0 if the total sector count "fits" (is less than 0x10000).
 *
 * m_ucBpbMedia;
 *
 *   non-removable(fixed) media - 0xF8
 *   removable media - 0xF0
 *   legal values - 0xF0, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF.
 *
 * m_uBpbHiddSec;
 *
 *   Count of hidden sectors preceding the partition
 *   that contains this FAT volume.
 *   This field is generally only relevant for media visible on interrupt 0x13.
 *   This field should always be zero on media that are not partitioned.
 *   Exactly what value is appropriate is operating system specific.
 *
 * DWORD BPB_TotSec32;
 *
 *   new 32-bit total count of sectors on the volume.
 *   This count includes the count of all sectors in all four regions of the volume.
 *   This field can be 0; if it is 0, then BpbTotSec16 must be non-zero.
 *   For FAT32 volumes, this field must be non-zero.
 *   For FAT12/FAT16 volumes, this field contains the sector count
 *   if BpbTotSec16 is 0 (count is greater than or equal to 0x10000).
 *
 * m_uBpbTotSectors;
 *
 *   Integration of BpbTotSec16 and BPB_TotSec32
 *
 * m_uBpbRootClus;
 *
 *   FAT32 only.
 *   This is set to the cluster number of the first cluster of the root directory,
 *   usually 2 but not required to be 2.
 *
 * m_nEOC;
 *
 *   End of Cluster Mark
 *
 * FAT32 FSInfo Sector Structure and Backup Boot Sector (pp.21-22)
 *-----------------------------------------------------------------
 *
 * m_uFsiFreeCount;
 *
 *   the last known free cluster count on the volume.
 *   0xFFFFFFFF --> free count: unknown, so must be computed.
 *   range check: field value <= volume cluster count
 *
 * m_uFsiNxtFree;
 *
 *   This is a hint for the FAT driver.
 *   It indicates the cluster number at which the driver should start looking for free clusters.
 *   Because a FAT32 FAT is large, it can be rather time consuming
 *   if there are a lot of allocated clusters at the start of the FAT
 *   and the driver starts looking for a free cluster starting at cluster 2.
 *   Typically this value is set to the last cluster number that the driver allocated.
 *   0xFFFFFFFF --> there is no hint and the driver should start looking at cluster 2.
 *   Any other value can be used, but should be checked first
 *   to make sure it is a valid cluster number for the volume.
 *
 * m_uFsiLeadSig;
 *
 *   Value 0x41615252.
 *   This lead signature is used to validate that this is in fact an FSInfo sector.
 *
 * m_ucFileEntryHead replace the below data structure..
 *
 *   char DIR_Name[11];     	// p.23 refer to algorithm (DIR_Name[0])
 *   BYTE DIR_CrtTimeTenth; 	// file creation time millisecond stamp, valid: 0~199
 *   USHORT DIR_CrtTime;    	// Time file was created.
 *   USHORT DIR_CrtDate;    	// Date file was created.
 *   USHORT DIR_LstAccDate; 	// Last access date (read or write)
 *   USHORT DIR_WrtTime;    	// Time of last write. file creation is included in this case.
 *   USHORT DIR_WrtDate;    	// Date of last write. file creation is included in this case.
 *
 * FAT Long Directory Entry Structure (p.26~27)
 *---------------------------------------------
 *
 * m_ucLDirOrd;
 *
 *   The nOrder of this entry in the sequence of long dir entries
 *   associated with the short dir entry at the end of the long dir set.
 *   If masked with 0x40 (LAST_LONG_ENTRY),
 *   this indicates the entry is the last long dir entry in a set of long dir entries.
 *   All valid sets of long dir entries must begin with an entry having this mask.
 *
 * m_ucLDirAttr;
 *
 *   Attributes - must be ATTR_LONG_NAME
 */

typedef enum
{
 	IS_FAT12 = 3,
 	IS_FAT16 = 4,
 	IS_FAT32 = 8
} FATTYPE_T;

#if 0
typedef enum
{
	BUFF_READ,
	BUFF_WRITE
} BUFF_STATE;
#endif

typedef enum
{
	CLUSTER_CHAIN,
	CLUSTER_BPLUS,
	CLUSTER_BMINUS
} COPY_MODE;

#define UNICODE2CHAR 0
#define CHAR2UNICODE 1


#define FAT_FREE_CLUSTER 0x0
#define LAST_CLUSTER(x) (((x&oFat.m_nEOC)==oFat.m_nEOC)?1:0)


#define PATH_NAME_CHAR_SEPARATOR '.'
#define PATH_NAME_CHAR_DELIMITER '/'
#define PATH_NAME_CHAR_END       '\0'
#define PATH_NAME_CHAR_VALID(c) (((c>='A')&&(c<='Z'))|| \
	((c>='0')&&(c<='9'))||(c=='-')||(c=='_')||(c=='.')||(c=='/')||(c=='\0')||(c==' '))

#define FREE_ENTRY_TAG0 0xe5
#define TERM_ENTRY_TAG0 0x00
#define DOT_ENTRY_TAG0 0x2e
#define FREE_ENTRY_TAG 0x20e5
#define TERM_ENTRY_TAG 0x0000
#define DOT_ENTRY_TAG 0x202e
#define DOTDOT_ENTRY_TAG 0x2e2e

///#define ENTRY_COPY(p, x) memncpy(p, HD_dir_entry(x, 0), DIR_ENTRY_SIZE)

// Little Endian value caculation
#define LSB_GET_4BYTES(x) ((*((x)+3)) << 24) | ((*((x)+2)) << 16) | \
                            ((*((x)+1)) << 8) | ((*((x))))
#define LSB_GET_3BYTES(x) ((*((x)+2)) << 16) | \
                            ((*((x)+1)) << 8) | ((*((x))))
#define LSB_GET_2BYTES(x) ((*((x)+1)) << 8 ) | ((*((x))))
#define UNICODEtOCHAR(x)    ((*((x)+1)) << 16) | ((*((x))))

#define LSB_SET_4BYTES(p, x)    { *((p)+3)= 0x0ff&(x>>24) ;      \
                                *((p)+2) = 0x0ff&(x>>16);        \
                                *((p)+1) = 0x0ff&(x>>8);         \
                                *(p) = 0x00ff&x;  }
#define LSB_SET_3BYTES(p, x)    { *((p)+2) = 0xff&(x>>16);      \
                                *((p)+1) = 0xff&(x>>8);         \
                                *(p) = 0xff&x;  }
#define LSB_SET_2BYTES(p, x)    { *((p)+1) = 0xff&(x>>8);       \
                                *(p) = 0xff&x;  }

// FAT Volume Initialization  (p.20)
//   This is the table for FAT16 drives. NOTE that this table includes
//   entries for disk sizes larger than 512 MB even though typically
//   only the entries for disks < 512 MB in size are used.
//   The way this table is accessed is to look for the first entry
//   in the table for which the disk size is less than or equal
//   to the DiskSize field in that table entry.  For this table to
//   work properly m_usBpbRsvdSecCnt must be 1
//   must be 2, and m_usBpbRootEntCnt must be 512. Any of these values
//   being different may require the first table entries DiskSize value
//   to be changed otherwise the cluster count may be to low for FAT16.


DSKSZTOSECPERCLUS DskTableFAT16 [] = {
	{        8400,   0}, 	     // disks up to 4.1 MB, the 0 value for SecPerClusVal trips an ucNerror
	{      32680,   2},  	     // disks up to  16 MB,  1k cluster
	{    262144,   4},   	     // disks up to 128 MB,  2k cluster
	{   524288,    8},   	     // disks up to 256 MB,  4k cluster
	{ 1048576,  16},     	     // disks up to 512 MB,  8k cluster
	     	     // The entries after this point are not used unless FAT16 is forced
	{ 2097152,  32},     	     // disks up to   1 GB, 16k cluster
	{ 4194304,  64},     	     // disks up to   2 GB, 32k cluster
	{ 0xFFFFFFFF, 0}     	     // any disk greater than 2GB, 0 value for SecPerClusVal trips an ucNerror
};

//   This is the table for FAT32 drives. NOTE that this table includes
//   entries for disk sizes smaller than 512 MB even though typically
//   only the entries for disks >= 512 MB in size are used.
//   The way this table is accessed is to look for the first entry
//   in the table for which the disk size is less than or equal
//   to the DiskSize field in that table entry. For this table to
//   work properly m_usBpbRsvdSecCnt must be 32
//   must be 2. Any of these values being different may require the first
//   table entries DiskSize value to be changed otherwise the cluster count
//   may be to low for FAT32.

DSKSZTOSECPERCLUS DskTableFAT32 [] = {
	{       66600,   0},         // disks up to 32.5 MB, the 0 value for SecPerClusVal trips an ucNerror
	{     532480,   1},          // disks up to 260  MB,  .5k cluster
	{ 16777216,   8},            // disks up to   8  GB,   4k cluster
	{ 33554432, 16},             // disks up to   16 GB,   8k cluster
	{ 67108864, 32},             // disks up to   32 GB,  16k cluster
	{ 0xFFFFFFFF, 64}            // disks greater than 32GB, 32k cluster
};


// in DIR_Attr,
#define ATTR_READ_ONLY	0x01
#define ATTR_HIDDEN		0x02
#define ATTR_SYSTEM		0x04
#define ATTR_VOLUME_ID	0x08
#define ATTR_DIRECTORY	0x10
#define ATTR_ARCHIVE	0x20
#define ATTR_LONG_NAME	0x0f         //ATTR_READ_ONLY | ATTR_HIDDEN | ATTR_SYSTEM | ATTR_VOLUME_ID
#define ATTR_LONG_NAME_MASK  0x3f    //ATTR_READ_ONLY|ATTR_HIDDEN|ATTR_SYSTEM|ATTR_VOLUME_ID|ATTR_DIRECTORY|ATTR_ARCHIVE
#define ATTR_FILES_ONLY 0x2f         // (ATTR_READ_ONLY|ATTR_HIDDEN|ATTR_SYSTEM|ATTR_ARCHIVE|ATTR_VOLUME_ID)
#define ATTR_DIR_FILES  0x3f         //(ATTR_FILES_ONLY|ATTR_DIRECTORY)
#define WRITE_CLUSTER
#define MAX_FILE_HANDLES        2
#define HDF_APPEND		0x0000
#define HDF_INSERT		0x0100
FATTYPE_T FATType;		             // FAT Type determination
//static HDFHANDLE *HDp, HDFhandle[MAX_FILE_HANDLES];  // each handle data for each file

FAT oFat;

//////////
// File Name : FAT_ReadFile1
// File Description : File Read function
// Input : file index, destination address, HSMMC_descriptor
// Output : NONE.
u8 FAT_ReadFile1(s32 nIdx, u32 uDestAddr, SDHC* sCh)
{
	return (FAT_ReadFile(nIdx, (u8 *)uDestAddr, sCh) == 0) ? false : true;
}

//////////
// File Name : FAT_ReadFile2
// File Description : File Read function with file size.
// Input : file index, destination address, file size pointer, HSMMC_descriptor
// Output : NONE.
u8 FAT_ReadFile2(s32 nIdx, u32 uDstAddr, u32* uFileSize, SDHC* sCh)
{
	*uFileSize = FAT_ReadFile(nIdx, (u8 *)uDstAddr, sCh);
	return (*uFileSize == 0) ? false : true;
}

//////////
// File Name : FAT_ReadFile3
// File Description : File Read function
// Input : file name, destination address, file size pointer, HSMMC_descriptor
// Output : NONE.
u8 FAT_ReadFile3(const char* pFileName, u32 uDstAddr, u32* uFileSize, SDHC* sCh)
{
	int index;

	FAT_GetFileIndex(pFileName, (s32*)&index, sCh);
	*uFileSize = FAT_ReadFile(index, (u8 *)uDstAddr, sCh);
	return (*uFileSize == 0) ? false : true;
}

//////////
// File Name : FAT_ReadFile5
// File Description : File Read function
// Input : file name, block position, block count
// Output : NONE.
u8 FAT_ReadFile5(const char* pFileName, u32 nBlockPos, u32 nBlocks, u32 uDestAddr, SDHC* sCh)
{
	int index;

	FAT_GetFileIndex(pFileName, (s32*)&index, sCh);
	return FAT_ReadFile4(index, nBlockPos, nBlocks, uDestAddr, sCh);
}

void FAT_GetNumOfFilesAndDirs(u32* uCount, SDHC* sCh)
{
	*uCount = oFat.m_nFileIndex;
}


///////////////////////////////
//   Array Manipulation     //
///////////////////////////////
u8 FAT_CopyArray(u8* a, const u8* b, u32 n, SDHC* sCh)
{
	u32 i;

	for(i=0; i<n; i++)
	{
		*(a+i) = *(b+i);
	}
	return true;
}

void FAT_CompareArray(u8* a, const u8* b, u32 n, int* nComArray, SDHC* sCh)
{
	u32 i;

	for(i=0; i<n; i++)
	{
		if (*(a+i) < *(b+i))
		{
			*nComArray = -1;
		}
		else if (*(a+i) > *(b+i))
		{
			*nComArray = 1;
		}
	}
	// a is equal to b..
	*nComArray = 0;
}

//-----------------------------------------------------------------------
// In FAT32,
//  byte Directory Entry Structure (p.23~25)
//  DIR_Name, DIR_Attr, DIR_CrtTimeTenth, DIR_CrtTime, DIR_CrtDate,
//  DIR_LstAccDate, DIR_WrtTime, DIR_WrtDate
// In DIR_Attr,
//  FAT Long Directory Entry Structure (p.26~27)
//  Organization and Association of Short & Long Directory Entries (p.27)
//-----------------------------------------------------------------------

// Sequence of Long Directory Entries (p.27)

//-----------------------------------------------------------------------
// ChkSum()
//  Returns an unsigned byte checksum computed on an unsigned byte
//  array.  The array must be 11 bytes long and is assumed to contain
//  a name stored in the format of a MS-DOS directory entry.
// Passed:  pFcbName  Pointer to an unsigned byte array assumed to be
//                   11 bytes long.
// Returns: ucSum     An 8-bit unsigned checksum of the array pointed
//                   to by pFcbName.
//-----------------------------------------------------------------------

void FAT_ComputeChkSum (u8 *pFcbName, u8* uSum, SDHC* sCh)
{
	short sFcbNameLen;
	u8    ucSum;

	ucSum = 0;
	for (sFcbNameLen=11; sFcbNameLen!=0; sFcbNameLen--)     // NOTE: The operation is an unsigned char rotate right
	{

		ucSum = ((ucSum & 1) ? 0x80 : 0) + (ucSum >> 1) + *pFcbName++;
	}
	*uSum = ucSum;
}

// The Basis-Name Generation Algorithm

// void BasisNameGeneration(void)
//  {
//  1.  The UNICODE name passed to the file system is converted to upper case.
//  2.	The upper cased UNICODE name is converted to OEM.
//      if   (the uppercased UNICODE glyph does not exist as an OEM glyph in the OEM code page)
//        or (the OEM glyph is invalid in an 8.3 name)
//      {
//	     Replace the glyph to an OEM '_' (underscore) character.
//	     Set a "lossy conversion" flag.
//      }
//  3.	Strip all leading and embedded spaces from the long name.
//  4.	Strip all leading periods from the long name.
//  5.	While	(not at end of the long name)
//  	  and	(char is not a period)
//	  and	(total chars copied < 8)
//      {
//	  Copy characters into primary portion of the basis name
//      }
//  6.  Insert a dot at the end of the primary components of the basis-name
//      iff the basis name has an extension after the last period in the name.
//  7.	Scan for the last embedded period in the long name.
//      If	(the last embedded period was found)
//      {
//	   While     (not at end of the long name)
//	     and     (total chars copied < 3)
//	   {
//		Copy characters into extension portion of the basis name
//	   }
//      }
//   }


/**********************************************************/
/* Generate a short name with a long file name            */
/**********************************************************/
void FAT_GenerateSNameFromLName(short *s, char *p, int* nFlag, SDHC* sCh)  // implementation of BasisNameGeneration()
{
	char c[2][9];
	int i, n, j[2]={0, 0}, nExt=0, nLossyFlag=0;

	while (*s)
	{
		short k = *s++;
		if (k>=0x61&&k<=0x7a)
			k -= 0x20;   // UNICODE name convert to Upper case. (k>=a && k<=z)
		if ((k&0x7f)!=k)                   //if k is minus
		{
			k = '_'; nLossyFlag = 1;
		} // if not ascii, replace it with '_' and lossy flag is set.
		if (k=='+' || k==',' || k==':' || k=='=' || k=='[' || k==']')
		{  // if invalid, do as the above.
			k = '_'; nLossyFlag = 1;
		}
		if (k==' ')
		{
			nLossyFlag = 1;
			continue;
		} // Strip all the spaces.
		if (k=='.'&&j[0]==0)
		{
			nLossyFlag = 1;
			continue;
		}  // Strip all the periods in the name.
		if (k!='.'&&nExt>1)
		{
			for (n=0; (n<j[1])&&(j[0]+n<8); n++)
				c[0][j[0]+n] = c[1][n];           // copy char into primary portion of the basis name
			j[0]+= n;  //j[o]=8
			j[1] = 0;
			nExt = 1;
		}
		if (k!='.'&&j[nExt]<8)
		{
			c[nExt][j[nExt]++] = (char)k;
			continue;
		}
		else if (k=='.')
		{
			if (nExt) nLossyFlag = 1;
				nExt++;
			continue;
		}
		else
			nLossyFlag = 1;
	}
	for (i=j[0]; i<8; i++)
		c[0][i] = ' ';
	c[0][8] = 0;  // Primary portion
	strcpy(p, c[0]);
	for (i=j[1]; i<3; i++)
		c[1][i] = ' ';
	c[1][3] = 0;  // Extension portion
	strcat(p, c[1]);
	*nFlag = nLossyFlag;
}

// The Numeric-Tail Generation Algorithm (p.31)

// void NumericTailGeneration(void)
// {
//	if	(a "lossy conversion" was not flagged)
//	   and	(the long name fits within the 8.3 naming conventions)
//	   and	(the basis-name does not collide with any existing short name)
//	{
//	   The short name is only the basis-name without the numeric tail.
//	}
//	else
//	{
//	   Insert a numeric-tail "~n" to the end of the primary name
//	   such that the value of the "~n" is chosen so that the name
//	   thus formed does not collide with any existing short name
//	   and that the primary name does not exceed eight characters in nLength.
//	}

// Validating the contents of a directory
//  	if  (((m_ucLDirAttr & ATTR_LONG_NAME_MASK) == (ATTR_LONG_NAME)) && (m_ucLDirOrd != 0xE5))
//	{
		//  Found an active long name sub-component.  //
//	}
//	if  (((m_ucLDirAttr & ATTR_LONG_NAME_MASK) != (ATTR_LONG_NAME)) && (m_ucLDirOrd != 0xE5))
//	{
//		if ((DIR_Attr & (ATTR_DIRECTORY | ATTR_VOLUME_ID)) == 0x00) {
			//  Found a file.  //
//		} else if ((DIR_Attr & (ATTR_DIRECTORY | ATTR_VOLUME_ID)) == ATTR_DIRECTORY) {
			//  Found a directory.  //
//		} else if ((DIR_Attr & (ATTR_DIRECTORY | ATTR_VOLUME_ID)) == ATTR_VOLUME_ID) {
			//  Found a volume label.  //
//		} else {
			//  Found an invalid directory entry.  //
//		}
//	}
// }  End of NumericTailGeneration()

/**********************************************************/
/* Append a numeric tail for short file name              */
/**********************************************************/
void FAT_GenerateNumericTail(char *p, int n, SDHC* sCh)  //Implementation of NumericTailGeneration()
{
	char c[8];
	int i, k, m=n, nDigitNum=1;

	// invalid numeric tail //
	if (n<1 || n>999999 || p ==NULL)
		return;
	// get nDigitNum //
	while (m>=10)
	{
		m /= 10; nDigitNum++;
	}
	// get tail in char //
	k = nDigitNum;
	c[k+1]=0;
	do
	{
		m=n%10; n/=10; c[k--]=m+'0';
	}
	while (n>=10);
	c[0] = '~';
	// insert //
	for (k=0; k<8; k++)
		if (p[k]==' ')
			break;
	if (k+nDigitNum<8)
	{
		for (i=0; i<nDigitNum; i++)
		p[k+i] = c[i];
	}
	else
	{
		for (i=0; i<nDigitNum; i++)
		p[8-nDigitNum+i] = c[i];
	}
}

void FAT_ReadSector(u8 *dest, int lba, int nblocks, SDHC* sCh)
{
	Assert(dest);
	Assert(nblocks);

	SDHC_ReadBlocks(lba, nblocks, (u32)dest, sCh);
}


void FAT_WriteSector(u8 *src, int lba, int nblocks, SDHC* sCh)
{
	Assert(src);
	Assert(nblocks);

	SDHC_WriteBlocks(lba, nblocks, (u32)src, sCh);
}

/******************************************************************/
/* From cluster to physical sector number.                        */
/******************************************************************/

void FAT_FromClusterToLba(int cluster, int* nLba, SDHC* sCh)
{
	if (cluster < 2)
	{
		*nLba = oFat.m_nPosRoot;
		return; /// 7.20 need
	}
	*nLba = oFat.m_nFirstDataSector + (cluster-2)*oFat.m_ucBpbSecPerClus + SDCARD_OFFSET_SECTOR;
}


/*******************************************************************/
/* collect a long file name in unicode format from directory entry */
/*******************************************************************/

void FAT_CollectLFileName(short *s, u8 *p, int* nLfname, SDHC* sCh)
{
	static int nLength;
	int i, j, nPosi, nHead, nOrder, ucNerror=0;
L:
	if (ucNerror)
	{
		Assert(0);
		if (nHead) oFat.m_nOrphanCount++;
		oFat.m_nNthEntry = 0;
		*nLfname =  -1;
	}

	nHead = ((p[0]&0x40)==0x40);
	if (nHead && oFat.m_nNthEntry>1)
	{
		ucNerror = 1; goto L;
	}
	else if (!nHead && oFat.m_nNthEntry<=1)
	{
		ucNerror = 2; goto L;
	}
	nOrder = p[0]&(~0x40);
	if (nOrder==0||nOrder==0xe5)
	{
		ucNerror = 3; goto L;
	}
	if (nHead && oFat.m_nNthEntry<=1)
		oFat.m_nNthEntry = nOrder;
	else if (oFat.m_nNthEntry == nOrder+1)
		oFat.m_nNthEntry = nOrder;
	else
	{
		ucNerror = 4; goto L;
	}
	Assert(oFat.m_nNthEntry>0);
	nPosi = 13*(oFat.m_nNthEntry-1);
	for (i = 0; i < 5; i++)  // 1-5
		s[nPosi+i] = LSB_GET_2BYTES(p+1+i*2);
	for (i = 0; i < 6; i++)  // 6-11
		s[nPosi+5+i] = LSB_GET_2BYTES(p+14+i*2);
	for (i = 0; i < 2; i++)  // 12-13
		s[nPosi+11+i] = LSB_GET_2BYTES(p+28+i*2);
	if (nHead)
	{
		nLength = nPosi+13;
		for (i=0; i<13; i++)
			if (s[nPosi+i]==0)
			{
				nLength = nPosi+i;
				break;
			}
		for (j=i+1; j<13; j++)
			if (s[nPosi+j]!=-1)
			{
				ucNerror = 5; goto L;
			}
	}
	if (oFat.m_nNthEntry==1)
	{
		if (!nLength)
		{
			ucNerror = 6; goto L;
		}
		*nLfname = nLength;
	}
	else
		*nLfname =  0;
}


/**********************************************************/
/* Pass out to directory entry from a long file name     */
/**********************************************************/
void FAT_PassoutLFileName(short *s, u8* cFname, int nOrder, int* nDirecEntry, SDHC* sCh)
{
//	char cSfname[SF_NAME_LENGTH+1];
	short *nPosi = s+13*(nOrder-1), sTmp[13];
	int i, k=0, nHead=nOrder, nMaxOrder;
	int nUniChr;
	int nLen;

	FAT_GetUnicodeStrLen(s, &nUniChr, sCh);
	nLen = nUniChr;
	nMaxOrder = (nLen+12)/13;
	if (nLen<=0 || nOrder<=0 || nOrder>nMaxOrder)
		*nDirecEntry = 0;
	if (nOrder == nMaxOrder)
	{
		nHead |= 0x40; k = nLen%13;
	}
	if (nHead==0xe5) nHead++;

	oFat.m_ucFileEntryHead[0] = nHead;
	oFat.m_ucFileEntryHead[11] = ATTR_LONG_NAME;
	oFat.m_ucFileEntryHead[12] = 0;
	FAT_ComputeChkSum(cFname, &oFat.m_ucFileEntryHead[13], sCh);
	LSB_SET_2BYTES(oFat.m_ucFileEntryHead+26, 0);
	if (k)
		for (i=0; i<13; i++)
		sTmp[i] = ((i<k)?(*(nPosi+i)):((i==k)?0:0xffff));
	else
		for (i=0; i<13; i++)
			sTmp[i] = *(nPosi+i);
	for (i = 0; i < 5; i++)  // 1-5 */
		LSB_SET_2BYTES(oFat.m_ucFileEntryHead+1+i*2, *(sTmp+i));
	for (i = 0; i < 6; i++)  // 6-11 */
		LSB_SET_2BYTES(oFat.m_ucFileEntryHead+14+i*2, *(sTmp+5+i));
	for (i = 0; i < 2; i++)  // 12-13 */
		LSB_SET_2BYTES(oFat.m_ucFileEntryHead+28+i*2, *(sTmp+11+i));
	*nDirecEntry = 1;
}

//***********************************************************************/
/* Check if p is a short name entry to know previous entry is a orphan  */
/************************************************************************/

void FAT_CheckOrphan(u8 *p, int* nOrphan, SDHC* sCh)
{
	if (p[0]==FREE_ENTRY_TAG0)
		*nOrphan = 1;
	if (p[0]==TERM_ENTRY_TAG0)
		*nOrphan = 1;
	if ((p[11]&ATTR_LONG_NAME)==ATTR_LONG_NAME)
		*nOrphan =  1;
	if ((p[11]&ATTR_VOLUME_ID)==ATTR_VOLUME_ID)
		*nOrphan = 1;
	*nOrphan = 0;
}

/***********************************************************/
/* Fat32 short file name conversion from outside to inside */
/***********************************************************/
void FAT_FormatSFileName(u8 *fname, char *name, int* nSfnameconv, SDHC* sCh)
{
	char *p=name;
	int j=0, nFoundSeparator=0, nFoundSpace=0;

	if (p==NULL || *p==0)
		*nSfnameconv = -1;
	else if (*p == PATH_NAME_CHAR_SEPARATOR)
	{
		fname[j++] = *p++;
		if (*p == PATH_NAME_CHAR_SEPARATOR)
		{
			fname[j++] = *p++;
		}
		if ((*p != PATH_NAME_CHAR_DELIMITER) && (*p != PATH_NAME_CHAR_END))
		{
			*nSfnameconv = -2;
		}
		fname[j] = PATH_NAME_CHAR_END;
		*nSfnameconv = 0;
	}
	while (*p)
	{
		if (!PATH_NAME_CHAR_VALID(*p))     // invalid char
		{
			*nSfnameconv = -3;
		}
		else if (*p==' ')
		{
			nFoundSpace = 1;
			fname[j++]=*p++;
		}
		else if (*p==PATH_NAME_CHAR_SEPARATOR)
		{
			if (nFoundSeparator)         // 2 or more
				*nSfnameconv = -4;
			else if (!j)                 // no base name
				*nSfnameconv =   -5;
			nFoundSeparator = 1;
			while (j<SF_BASE_LENGTH)
				fname[j++]=' ';          // 8 character max
			p++;
		}
		else if ((*p==PATH_NAME_CHAR_DELIMITER)||(*p==PATH_NAME_CHAR_END))
		{
			if (!j)
				*nSfnameconv = -5;        // no base name
			else
				while (j<SF_NAME_LENGTH)
					fname[j++]=' ';      // 8+3
		}
		else
		{
			if (j==SF_BASE_LENGTH)
				nFoundSpace = 0;
			else if (nFoundSpace)
				*nSfnameconv = -6;
			fname[j++] = *p++;
		}
		if (j==SF_NAME_LENGTH)
			break;   // 8+3
	}
	while (j<SF_NAME_LENGTH)
		fname[j++]=' ';
	fname[j]=PATH_NAME_CHAR_END;   // NULL
	*nSfnameconv = 1;
}

/*************************************************************/
/* Check if two long file name is match or not               */
/*************************************************************/
void FAT_Compare2LFileName(u32 slength, short *s, char *p, int* nChkmatch, SDHC* sCh)
{
	u32 i;

	if (slength!=strlen(p))
		*nChkmatch = 0;
	for (i=0; i<slength; i++) {
		if (s[i]==p[i] || ((s[i]-0x20)==p[i]&&s[i]>=0x61&&s[i]<=0x71))
			continue;
		else
			*nChkmatch = 0;
	}
	*nChkmatch = 1;
}
/************************************************************/
/* Get the unicode character string nLength		            */
/************************************************************/
void FAT_GetUnicodeStrLen(short *s, int* nUniChr, SDHC* sCh)
{
	int i;

	if (s==NULL)
		*nUniChr = 0;
	for (i=0; i<260; i++)
	{
		if (*s==0)
			break;
		else
			s++;
	}
	*nUniChr = i;
}


/************************************************************/
/* Convert unicode to char (direct=0) or reverse (direct=1) */
/************************************************************/
void FAT_ConvertUnicode(short *s, char *p, int direct, int* nConvUni, SDHC* sCh)
{
	int i, nLen=0;

	if (direct==UNICODE2CHAR)
	{
		FAT_GetUnicodeStrLen(s, &nLen, sCh);
		for (i=0; i<=nLen; i++)
			*p++ = (char)*s++;
	}
	else if (direct==CHAR2UNICODE)
	{
		nLen = strlen(p);
		for (i=0; i<=nLen; i++)
			*s++ = *p++;
		s = s-nLen-1;
		p = p-nLen-1;
	}
	*nConvUni =  nLen;
}

/*****************************************************************/
/* check if two long file name in unicode format is match or not */
/*****************************************************************/
void FAT_Compare2UnicodeLFileName(short *s1, short *s2, int* nComLFName, SDHC* sCh)
{
	while (*s1)
	{
		if (*s1++!=*s2++)
		*nComLFName = 0;
	}
	if (*s2)
		*nComLFName = 0;
	else
		*nComLFName = 1;
}

void FAT_CopyUnicodeLFileName(short *s1, short *s2, int* nCoLname, SDHC* sCh)
{
	int i=0;

	while (*s2)
	{
		i++;
		*s1++=*s2++;
	}
	*s1 = *s2;
	*nCoLname = i;
}

/************************************************/
/* Get nHead cluster from a file entry           */
/************************************************/
void FAT_GetEntryCluster(u8 *p, u32* nEntCluster, SDHC* sCh)  // p: FAT32 Byte Directory Entry Structure
{
	int sTmp, tmp1;

	sTmp = LSB_GET_2BYTES(p+20);      // high word of this entry's 1st cluster number
	tmp1 = LSB_GET_2BYTES(p+26);      // low word
	sTmp = (sTmp << 16) + tmp1;
	if (sTmp==0) sTmp=2;              // for root directory
		*nEntCluster = sTmp;
}
/**********************************************/
/* Set nHead cluster in a file entry           */
/**********************************************/
void FAT_SetEntryCluster(u8 *p, u32 cluster, SDHC* sCh)  // p: FAT32 Byte Directory Entry Structure
{
	int sTmp = (cluster>>16)&0xffff;  // high word

	cluster &= 0xffff;                // low word
	LSB_SET_2BYTES(p+20, sTmp);       // left: LSB address, right: value
	LSB_SET_2BYTES(p+26, cluster);
}

/************************************************/
/* set date and time in a file entry           */
/************************************************/
void FAT_SetDateAndTime(u8 *p, SDHC* sCh)      // p: FAT32 Byte Directory Entry Structure (oFat.m_ucFileEntryHead)
{
#if 1
	LSB_SET_2BYTES(p+14, 0xAE37);    // DIR_CrtTime
	LSB_SET_2BYTES(p+16, 0x309E);    // DIR_CrtDate
	LSB_SET_2BYTES(p+22, 0x612A);    // DIR_WrtTime
	LSB_SET_2BYTES(p+24, 0x2f5c);    // DIR_WrtDate
	LSB_SET_2BYTES(p+18, 0x309e);    // DIR_LstAccDate
#else
	LSB_SET_2BYTES(p+14, oFat.m_nFat32FileTime);  // DIR_CrtTime
	LSB_SET_2BYTES(p+16, oFat.m_nFat32FileDate);  // DIR_CrtDate
	LSB_SET_2BYTES(p+22, oFat.m_nFat32FileTime);  // DIR_WrtTime
	LSB_SET_2BYTES(p+24, oFat.m_nFat32FileDate);  // DIR_WrtDate
	LSB_SET_2BYTES(p+18, oFat.m_nFat32FileDate);  // DIR_LstAccDate
#endif
}

/************************************************/
/* Generate a special directory entry                              */
/* ucSpecEnt : must be 2-level Pointer.. by song.            */
/************************************************/
void FAT_GenerateDirEntry(int tag, int cluster, u8** ucSpecEnt, SDHC* sCh)
{
	int i;

	for (i=0; i<DIR_ENTRY_SIZE; i++)     // Initialize oFat.m_ucFileEntryHead.
	{
		oFat.m_ucFileEntryHead[i] = 0;
	}
	LSB_SET_2BYTES(oFat.m_ucFileEntryHead, tag);
	if (tag==DOT_ENTRY_TAG||tag==DOTDOT_ENTRY_TAG) // 0x202e or 0x2e2e, ". " or ".."
	{
		for (i=2; i<SF_NAME_LENGTH; i++) // Insert space in DIR_Name[2] ~ DIR_Name[10]
		{
			oFat.m_ucFileEntryHead[i] = 0x20;
		}
		oFat.m_ucFileEntryHead[11] = ATTR_DIRECTORY;
		FAT_SetDateAndTime(oFat.m_ucFileEntryHead, sCh);
		FAT_SetEntryCluster(oFat.m_ucFileEntryHead, cluster, sCh);
	}
	LSB_SET_4BYTES(oFat.m_ucFileEntryHead+28, 0);   // 32-bit DWORD holding this file's size in bytes
	// youngbo.song - set pointer value..
	*ucSpecEnt = oFat.m_ucFileEntryHead;   // FAT32 Byte Directory Entry Structure
}

void FAT_CopyEntry(u8* p, u32 x, SDHC* sCh)
{
	u8* pTargetPointer;

	FAT_GenerateDirEntry(x, 0, &pTargetPointer, sCh);
	FAT_CopyAblockOfMem(p, pTargetPointer, DIR_ENTRY_SIZE, sCh);
}

/***********************************/
/* A block of memory copy	       */
/***********************************/
void FAT_CopyAblockOfMem(u8 *target, u8 *source, int bytes, SDHC* sCh)
{
	int i;

	for (i=0; i<bytes; i++)
		*target++ = *source++;
}

/********************************************************/
/* Conversion: sector and offset unifying          		*/
/********************************************************/
// only used in DIR_search_file_entry()
void FAT_UnifySector(int *sector, int *offset, int sector1, int offset1, SDHC* sCh)
{
	*sector = sector1 + offset1/oFat.m_usBpbBytsPerSec;
	*offset = offset1%oFat.m_usBpbBytsPerSec;
}

/*****************************************************************/
/* Get next cluster with current_cluster in a FAT table          */
/*****************************************************************/
void FAT_GetNextCluster(int cluster, int* nNextClu, SDHC* sCh)
{
	int nSector, nOffset, nNextcluster;
	int nFatEnt;

	FAT_ClusterFatAddr(cluster, &nSector, &nOffset, sCh);
	FAT_GetFat(nSector, nOffset, &nNextcluster, &nFatEnt, sCh);
	if (nFatEnt <= 0 )
	{
		Assert(0);
		*nNextClu = -2;
	}

	if (nNextcluster >= oFat.m_nEOC)
	{
		*nNextClu = -1;
	}
	else if (FATType==IS_FAT32)
		*nNextClu = (nNextcluster & oFat.m_nMaskCluster);
	else
		*nNextClu = nNextcluster;
}


/*****************************************************************************/
/* Write a fat entry for that cluster number into oFat.m_ucBuffer in a FAT table	 */
/*****************************************************************************/
void FAT_SetFat(int offset, u32 fat_entry, u8*p, SDHC* sCh)
{
	//u8 *p = oFat.m_pFatBuffStart;
	u32 uOriginal;

#if 0
	if(LastFatSector == -1)
	{
		LastFatSector = 0;
	}
	ReadSector(FatBuffStart, LastFatSector+oFat.m_nPosFat+SDCARD_OFFSET_SECTOR, 1);
#endif
	// in FAT32
	uOriginal = LSB_GET_4BYTES(p+offset);
	fat_entry = (fat_entry&oFat.m_nMaskCluster)+(uOriginal&(~oFat.m_nMaskCluster));
	///UART_Printf("fat_entry content: 0x%x, offset = 0x%x, uOriginal: 0x%x in SetFat()\n", fat_entry, offset, uOriginal);  // LYS 040429 test
	LSB_SET_4BYTES(p+offset, fat_entry);
	///CopyArray(see, FatBuffStart, SECBUFFSIZE);
	oFat.m_nLastFatChanged = 1;
}


/*********************************************************************/
/* Write the fat content from oFat.m_ucBuffer to fat area of HD if needed */
/*********************************************************************/
void FAT_WriteFatContent(int* nFatContent, SDHC* sCh) //WriteFatContent
{
	u8 *p = oFat.m_pFatBuffStart;

	if (oFat.m_nLastFatChanged)
	{
		///UART_Printf("flush_fat(): oFat.m_nLastFatSector - %d\n", oFat.m_nLastFatSector);
		//WriteSector(p, oFat.m_nLastFatSector+oFat.m_nPosFat+SDCARD_OFFSET_SECTOR, 1);
		FAT_WriteSector(p, oFat.m_nLastFatSector+oFat.m_uBpbHiddSec+oFat.m_nPosFat+SDCARD_OFFSET_SECTOR, 1, sCh);// sunny modify
		oFat.m_nLastFatChanged = 0;
	}
	*nFatContent = 1;
}

/************************************************************************/
/* conversion: cluster to its fat's sector and offset in a FAT table    */
/************************************************************************/
void FAT_ClusterFatAddr(int cluster, int *sector, int *offset, SDHC* sCh)
{
	int nFatOffSet = (cluster * FATType)/2;

	*sector = nFatOffSet / oFat.m_usBpbBytsPerSec;
	*offset = nFatOffSet - (*sector) * oFat.m_usBpbBytsPerSec;
}

/*****************************************************************/
/* read a fat entry from oFat.m_ucBuffer with specified sector and offset */
/*****************************************************************/
void FAT_GetFat(int sector, int offset, int *fat_entry, int* nFatEnt, SDHC *sCh)
{
	u8 *p = oFat.m_pFatBuffStart;

	if ((sector<0)||(sector>=oFat.m_nFatSz))
	{
		*nFatEnt = -1;
		return;
	}
	if (sector != oFat.m_nLastFatSector)
	{
		int i=1;  // in fat16, 32
		if (oFat.m_nLastFatChanged)
		{
			//UART_Printf("why fat change.. in GetFat()\n");
			FAT_WriteSector(p, oFat.m_nLastFatSector+oFat.m_nPosFat+SDCARD_OFFSET_SECTOR, i, sCh);
			oFat.m_nLastFatChanged = 0;
			Delay(10000); /// need delay between write and read op. why??
		}
		FAT_ReadSector(p, sector+oFat.m_nPosFat+oFat.m_uBpbHiddSec+SDCARD_OFFSET_SECTOR, i, sCh); // sunny 060818
		oFat.m_nLastFatSector = sector;
	}
	// in FAT32
	*fat_entry = (LSB_GET_4BYTES(p+offset))&oFat.m_nMaskCluster;
	///UART_Printf("fat_entry: 0x%x in GetFat()\n", *fat_entry);  // LYS 040429 test

	*nFatEnt = 1;
}

/*****************************************************************/
/* append cluster chain to another chain                         */
/*****************************************************************/
void FAT_AppendCluster(int cluster, int nextcluster, int* nAnoChain, SDHC* sCh)
{
	int sector, offset, sTmp;
	int nFatEnt;

	FAT_ClusterFatAddr(cluster, &sector, &offset, sCh);
	FAT_GetFat(sector, offset, &sTmp, &nFatEnt, sCh);
	if (nFatEnt<=0) {
		Assert(0);
		UART_Printf("append cluster fat ERROR\n");
		// ucNerror Display
		*nAnoChain = -1;
	}
	FAT_SetFat(offset, nextcluster, oFat.m_pFatBuffStart, sCh);

	FAT_WriteFatContent(nAnoChain, sCh);
}

/********************************************************************/
/* write a file entry in directory with file nLength and headcluster */
/********************************************************************/
void FAT_WriteFileHead(const char *name, u8 ucAttr, u32 nLength, u32 cluster, int* nFileEnt, SDHC* sCh)
{
	int i;

	if (cluster<2)
	{
		*nFileEnt = -2;
		return;
	}
	ucAttr &= 0x3f;
	if ((ucAttr&ATTR_DIRECTORY) == ATTR_DIRECTORY)
		nLength = 0;
	UART_Printf("name: %s\n", name);
	FAT_ConvertToDiscEntryFormat(name, oFat.m_cpEntryName, sCh);
	UART_Printf("oFat.m_cpEntryName: %s\n", oFat.m_cpEntryName);

	FAT_CopyArray(oFat.m_ucFileEntryHead, (u8*)oFat.m_cpEntryName, SF_NAME_LENGTH, sCh);
	UART_Printf("oFat.m_ucFileEntryHead: ");
	for(i=0; i<SF_NAME_LENGTH; i++) {
		UART_Printf("%c ", oFat.m_ucFileEntryHead[i]);
	}
	UART_Printf("\n");

	oFat.m_ucFileEntryHead[11] = ucAttr;
	oFat.m_ucFileEntryHead[12] = 0x18;  //NTRes  // LYS 040427
	oFat.m_ucFileEntryHead[13] = 0x3D;  //CrtTimeTenth  // LYS 040427  //LYS 040430
	FAT_SetDateAndTime(oFat.m_ucFileEntryHead, sCh);
	FAT_SetEntryCluster(oFat.m_ucFileEntryHead, cluster, sCh);
	LSB_SET_4BYTES(oFat.m_ucFileEntryHead+28, nLength);
	*nFileEnt = 0;
}

void FAT_WriteSFFileHead(const char *name, u8 ucAttr, u32 nLength, u32 cluster, int* nFileEnt, SDHC* sCh)
{
	int i;

	//int flength;
	//if ((flength=FormatSFileName(oFat.m_ucFsfname, (char*)name))<0) return -1;
	if (cluster<2)
	{
		*nFileEnt = -2;
		return;
	}
	//name = oFat.m_ucFsfname;
	ucAttr &= 0x3f;
	if ((ucAttr&ATTR_DIRECTORY) == ATTR_DIRECTORY)
	nLength = 0;
	//strncpy(oFat.m_ucFileEntryHead, name, strlen(name));  //LYS 040102 (char*) type casting
	UART_Printf("name: %s\n", name);

	FAT_CopyArray(oFat.m_ucFileEntryHead, (u8*)name, SF_NAME_LENGTH, sCh);
	UART_Printf("oFat.m_ucFileEntryHead: ");
	for(i=0; i<SF_NAME_LENGTH; i++) {
		UART_Printf("%c ", oFat.m_ucFileEntryHead[i]);
	}
	UART_Printf("\n");

	oFat.m_ucFileEntryHead[11] = ucAttr;
	oFat.m_ucFileEntryHead[12] = 0x18;  //NTRes  // LYS 040427
	oFat.m_ucFileEntryHead[13] = 0x3D;  //CrtTimeTenth  // LYS 040427  //LYS 040430
	FAT_SetDateAndTime(oFat.m_ucFileEntryHead, sCh);
	FAT_SetEntryCluster(oFat.m_ucFileEntryHead, cluster, sCh);
	LSB_SET_4BYTES(oFat.m_ucFileEntryHead+28, nLength);
	*nFileEnt = 0;
}

/*******************************************************/
/* release a cluster chain with specified headcluster  */
/*******************************************************/
// headcluster: cluster to be deleted in FAT table
// nLength: short file or dir name: 1
//         long file or dir name: the number of entries to be used
void FAT_ReleaseClusterChain(int headcluster, int nLength, int* nRelCluster, SDHC* sCh)
{
	int sector, offset, sTmp, count=0;
	int nFatContent;

	if (headcluster<=FAT_FREE_CLUSTER)
	{
		*nRelCluster = -1;  // FAT_FREE_CLUSTER == 0
		return;
	}
	if (LAST_CLUSTER(headcluster))
	{
		*nRelCluster = -2;  // headcluster == oFat.m_nEOC (End Of Cluster)
		return;
	}
	oFat.m_nLastFreeCluster = headcluster;

	while (!LAST_CLUSTER(headcluster)) {
		FAT_ClusterFatAddr(headcluster, &sector, &offset, sCh);
		FAT_GetFat(sector, offset, &headcluster, &sTmp, sCh);
		Assert(sTmp>0);
		FAT_SetFat(offset, FAT_FREE_CLUSTER, oFat.m_pFatBuffStart, sCh);
		if (!headcluster) {
			Assert(0);
			UART_Printf("release_cluster_chain(): headcluster: %d\n", headcluster);
			break;
		}
		if (++count==nLength) break;
	}

	FAT_WriteFatContent(&nFatContent, sCh);
	*nRelCluster = headcluster;
}

/*******************************************************/
/* allocate a cluster chain with specified size        */
/*******************************************************/
// FAT table maker

void FAT_AllocateClusterChain(int ucMode, int *numberofcluster, int nPosi, int rstart, int rend, int* uAlloCluster, SDHC* sCh)
{
	int start_sector, offset, nHeadCluster=0, pcluster=0;
	int cluster=nPosi, clucount=oFat.m_nBpbCountOfClusters+2, size=*numberofcluster;
	int nFatContent1;
	int nFatContent2;

	if (size<=0)
	{
		*uAlloCluster = 0;
		return;
	}
	while (cluster<clucount) {
		int res;
		int sTmp;

		if (rstart) {
			if (cluster>=rstart&&cluster<rend) {
				Assert(rend>rstart);
				cluster=rend;
				if (cluster>=clucount)
					break;
			}
		}

		FAT_ClusterFatAddr(cluster, &start_sector, &offset, sCh);  // offset: in bytes
		UART_Printf("allo_cchain(): start_sector: %d, offset: %d\n", start_sector, offset);

		FAT_GetFat(start_sector, offset, &sTmp, &res, sCh);
		Assert(res>0);
		if ((sTmp&oFat.m_nMaskCluster) == FAT_FREE_CLUSTER) 
		{
			int psector, poffset, entry;
			if (!nHeadCluster) 
				nHeadCluster = cluster;
			if (pcluster) 
			{  /* do link here */
				FAT_ClusterFatAddr(pcluster, &psector, &poffset, sCh);
				FAT_GetFat(psector, poffset, &entry, &res, sCh);
				Assert(res>0);
				Assert(!(entry&oFat.m_nMaskCluster));
				FAT_SetFat(poffset, cluster, oFat.m_pFatBuffStart, sCh);
				///WriteFatContent();  // LYS 040429 add  //LYS 040507 delete to speed up
			}
			pcluster = cluster;
			if (--size==0) 
			{     // end processing */
				FAT_ClusterFatAddr(pcluster, &psector, &poffset, sCh);
				FAT_GetFat(psector, poffset, &entry, &res, sCh);
				Assert(res>0);
				Assert(!(entry&oFat.m_nMaskCluster));
				FAT_SetFat(poffset, oFat.m_nEOC, oFat.m_pFatBuffStart, sCh);

				FAT_WriteFatContent(&nFatContent1, sCh);
				*uAlloCluster = nHeadCluster;
				return;
			}
		}
		cluster++;
	}

	FAT_WriteFatContent(&nFatContent2, sCh);
	// in case of absent memory
	if (ucMode) { // do not release cluster chain */
		*numberofcluster -= size;
	}
	else if (nHeadCluster) { // release cluster chain */
		int nRelCluster;
		FAT_ReleaseClusterChain(nHeadCluster, 1, &nRelCluster, sCh);
		*numberofcluster = nHeadCluster = 0;
	}
	*uAlloCluster = nHeadCluster;
}

/***************************************************************************/
/* for new cluster, we setup 32 entries to termination for long name space */
/***************************************************************************/
//only used in DIR_search_file_entry()
void FAT_AppendProcess(u8 *ucBuffer, int sector, int termination, SDHC* sCh)
{
	int i, k;

	if (termination==3)
		Assert(oFat.m_ucBpbSecPerClus>=2);
	for (k=0; k<termination-1; k++) {
		u8 *p = ucBuffer;
		for (i=0; i<16; i++) {
			FAT_CopyEntry(p, TERM_ENTRY_TAG, sCh);  //TERM_ENTRY_TAG == 0x0000
			p += DIR_ENTRY_SIZE;
		}
		FAT_WriteSector(ucBuffer, sector+k, 1, sCh);
	}
}

/************************************************************************/
/* for term entry, we check if next entry is avaible and do exit        */
/************************************************************************/
//only used in DIR_search_file_entry()
void FAT_ProcessTerm(u8 *nPosi, u8 *ucBuffer, int sector, int num, int* nNextEnt, SDHC* sCh)
{
	u8 *p = nPosi+DIR_ENTRY_SIZE;
	u8 *buff_end=ucBuffer+oFat.m_usBpbBytsPerSec;

	if (num > 1) {
		FAT_CopyEntry(nPosi, FREE_ENTRY_TAG, sCh);
		*nNextEnt = 1;
	}
	else if (p < buff_end) {
		if (p[0] != TERM_ENTRY_TAG0) {
			FAT_CopyEntry(p, TERM_ENTRY_TAG, sCh);
			FAT_WriteSector(ucBuffer, sector, 1, sCh);
		}
		*nNextEnt =  0;
	}
	else {
		*nNextEnt = 2;
	}
}

/*************************************************************/
/* long file name processing: collect, compare and overwrite */
/*************************************************************/
//only used in DIR_search_file_entry()
void FAT_ProcessLfname(short *lfname, u8 *nPosi, u8 *ucBuffer, int sector, int *pcsum, int* nProLFname, SDHC* sCh)
{
	u8 *buff_end = ucBuffer+oFat.m_usBpbBytsPerSec;
	int match=-1;
	int nLfname;
	int ucNerror;

	FAT_CollectLFileName(oFat.m_sClfName, nPosi, &nLfname, sCh);
	ucNerror = nLfname;

	if (ucNerror>0)
	{
		u8 *p = nPosi+DIR_ENTRY_SIZE;    // p: directory entry structure
		int nOrphan;
		FAT_CheckOrphan(p, &nOrphan, sCh);
		if (p<buff_end && nOrphan )
		{
			ucNerror = -1;
		}
		else
		{
			*pcsum = nPosi[13];
			FAT_Compare2UnicodeLFileName(oFat.m_sClfName, lfname, &match, sCh);
		}
	}
	if (ucNerror<=0)
	{
		if (ucNerror==-1)
		{ // auto erase invalid entry */
			FAT_CopyEntry(nPosi, FREE_ENTRY_TAG, sCh);
			FAT_WriteSector(ucBuffer, sector, 1, sCh);
		}
		*pcsum = 0;
	}
	*nProLFname = match; // -1 no compare; 0 doesn't match; 1 match */
}
/************************************************************************/
/* match entry processing: check sum, short name compare and info save  */
/************************************************************************/
void FAT_MatchProcess(int match, int checksum, u8 *p, ISOFILE *pfile, int* nMatEnt, SDHC* sCh)
{
	int i, j, ok=0;
	int nComArray;
	u32 nEntCluster;

	if (match)
	{
		u8 temp;
		FAT_ComputeChkSum(p, &temp, sCh);
		if (checksum== temp)
			ok = 1;
		else
		{
			ok = 1; Assert(0);
			//EPRINTF(("csum:%x:%x\n", checksum, ComputeChkSum(p)));
		}
	}
	else
	{
		j = -1;
		for (i = 0; i<=SF_NAME_LENGTH; i++)
		{
			if (p[i] == '~')
				j = i;
			if ((i>=j) && (j >= 0)) {
				p[i] = oFat.m_ucFsfname[i];
			}
		}

		for (i=SF_NAME_LENGTH; i>0; i--) {
			if (oFat.m_ucFsfname[i-1] != ' ') break;
		}

		FAT_CompareArray(oFat.m_ucFsfname, p, i, &nComArray, sCh);
		ok = !nComArray;  //LYS 040102 strncmp() was used.
	}
	if (ok) {
		// Calculate start cluster and save in location */
		pfile->size = LSB_GET_4BYTES(p+28);

		FAT_GetEntryCluster(p, &nEntCluster, sCh);
		pfile->addr = nEntCluster;
		pfile->ucAttr = p[11];
		*nMatEnt = 1;
	}
	else
		*nMatEnt = 0;
}

/************************************************************************/
/* match entry processing: check sum, short name compare and info save  */
/************************************************************************/
void FAT_AttributeFile(u8 *p, int* nAttriFile, SDHC* sCh)  // p: FAT32 byte directory entry structure
{
	int res;

	/* the ucAttr match checking */
	if (((p[11]&ATTR_VOLUME_ID)==ATTR_VOLUME_ID)&&((p[11]&ATTR_LONG_NAME)!=ATTR_LONG_NAME))
	{ // volume ID */
		res=0;
	}
	else if ((p[11]&ATTR_LONG_NAME)==ATTR_LONG_NAME && p[0]!=FREE_ENTRY_TAG0)
	{ // long name */  // 0xe5 == no file or dir in this entry.
		res=1;
	}
	else if (p[0]==DOT_ENTRY_TAG0)
	{	// . entry */
		res=2;
	}
	else
	{
		res=3;		// other else */
	}
	*nAttriFile = res;
}

/************************************************************************/
/* search the specified file on hd, always using unicode long file name */
/************************************************************************/
void FAT_SearchDirFileEntry(int location, short *lfname, int entry_num, u8 ucAttr,
	ISOFILE *pfile, int *sector, int *offset, int *clus, int* nSpecFile, SDHC* sCh)
{
	char cSfname[SF_NAME_LENGTH+1];
	u8 *buff_start, *buff_end;
	int precluster;
	int cluster = ((location)?location:oFat.m_uBpbRootClus);
	int checksum;
	int match=0, sector_offset=0, termination=0, num=entry_num;
	int start_sector, ftype, longfilename=0;
	int nSfnameconv;
	int temp;
	int nAnoChain;

	if ((lfname!=NULL&&entry_num!=1)||(entry_num<1||entry_num>32))
		*nSpecFile = 0;
	if (lfname)
	{
		FAT_GenerateSNameFromLName(lfname, cSfname, &temp, sCh);

		FAT_FormatSFileName(oFat.m_ucFsfname, cSfname, &nSfnameconv, sCh);
		if (nSfnameconv < 0)
			*nSpecFile = 0;
	}
	//buff_start = oFat.m_ucDirBuffStart;
	FAT_InitBuffSearchMem(&buff_start, sCh);  // LYS 040407
	buff_end = buff_start + oFat.m_usBpbBytsPerSec;
	*clus = cluster;

	while (!LAST_CLUSTER(cluster))
	{
		u8 *p = buff_start;
		int nLba;

		FAT_FromClusterToLba(cluster, &nLba, sCh);
		start_sector = nLba + sector_offset;
		FAT_ReadSector(buff_start, start_sector, 1, sCh);
		if (termination > 1)
		{
			FAT_AppendProcess(buff_start, start_sector, termination, sCh);
			if (termination==3)
			{
				FAT_UnifySector(sector, offset, start_sector, (num-1)*DIR_ENTRY_SIZE, sCh);
			}
			*nSpecFile = 1;
		}
		while (p<buff_end)
		{
			FAT_UnifySector(sector, offset, start_sector, p-buff_start, sCh);
			if(p[0]==TERM_ENTRY_TAG0 || termination==1) /*terminating */
			{
				if (lfname!=NULL)
					*nSpecFile = 0;
				FAT_ProcessTerm(p, buff_start, start_sector, num, &termination, sCh);
				if (!termination)
					*nSpecFile = 1;
				if (termination==1)
					num--;
				else
					break;
				longfilename=0;
			}
			else if (p[0]==FREE_ENTRY_TAG0) /*deleted file(dir) */
			{
				if (lfname==NULL) {
					if (num>1)
						num--;
					else
						*nSpecFile = 1;
				}
				longfilename=0;
			}
			else if ((lfname!=NULL)&&(p[11]&ucAttr)==p[11]) /* match a file we want */
			{
				num = entry_num;
				FAT_AttributeFile(p, &ftype, sCh);
				if (ftype==1) {  // long name
					FAT_ProcessLfname(lfname, p, buff_start, start_sector, &checksum, &match, sCh);
					if (!match)
						longfilename = 1;
					else {
						longfilename = 0;
						if (match<0)
							match = 0;
					}
				}
				else if (ftype>=2&&!longfilename) {  // 2: . entry, more than 3: short name
					int nMatEnt;
					FAT_MatchProcess(match, checksum, p, pfile, &nMatEnt, sCh);
					if (nMatEnt) {
						*nSpecFile = 1;   // short name compare using checksum, oFat.m_ucFsfname
					}
					match = 0;
					longfilename = 0;
				}
			}
			else
			{
				num = entry_num;
				longfilename = 0;
			}
			if (match)
			{
				match++;
				if (match==3)
				{
					match = 0;
					checksum = 0;
				}
			}
			p += DIR_ENTRY_SIZE;
		} // end of while (p<buff_end)
		if (termination) {
			FAT_WriteSector(buff_start, start_sector, 1, sCh);
		}
		sector_offset++;
		if (!cluster) {
			if (sector_offset>=oFat.m_usRootDirSectors)
				break;
		}
		else if (sector_offset>=oFat.m_ucBpbSecPerClus) {
			num = entry_num;
			sector_offset = 0;
			precluster = cluster;
			FAT_GetNextCluster(cluster, &cluster, sCh);
			Assert(cluster!=0);
			if (termination&&LAST_CLUSTER(cluster))
			{
				int size=1;
				FAT_AllocateClusterChain(0, &size, 2, 0, 0, &cluster, sCh);
				Assert(cluster);
				if (!cluster)
					break;

				FAT_AppendCluster(precluster, cluster, &nAnoChain, sCh);
				if (termination==1)
					termination = 3;
			}
		}
		*clus = cluster;
	}  // end of while (!LAST_CLUSTER(cluster))
	if (termination)
	{
		FAT_CopyEntry(buff_end-DIR_ENTRY_SIZE, TERM_ENTRY_TAG, sCh);
		FAT_WriteSector(buff_start, start_sector, 1, sCh);
	}
	*sector= *offset = 0;
	*nSpecFile = 0;
}

/********************************************************************/
/* create a empty file with specified size on hd, lfname in unicode */
/********************************************************************/
// return value: nHeadCluster <-- HD_allo_cchain(, ,) is right??
void FAT_CreateFile(int location,  const char *cFname, u8 ucAttr, int nLength, int* nEmpFile, SDHC* sCh)
{
	u8 p[SECBUFFSIZE*MAXSEC_PER_CLUSTER];  //LYS 040525
	int entries, numberofcluster;//nPosi, 
	u32  uHeadCluster;
	int  nSector, nOffset, nDirCluster;
	int  nLen = strlen(cFname);
	int nFileEnt, m;
	char  cSfname[11];
//	u32 i;
	int nEntOffset;
	int nNextCluster;
	int uAlloCluster;
	short*  spLFUnicodeName;
	int    nLossFlag;
	int    nDirecEntry;
	const char*  cTempSfname;
	int nCurrDirNum, nNextDirNum;
	u32  uDIR_FstClus, uDIR_FileSize;
	u8* pLastEntryAddr;

	Assert(nLen>0);

	if(FAT_FindDirEntryOffsetFromName(cFname, &nEntOffset, sCh))
	{ // file or directory exist, verify ok
		UART_Printf("The same name file already exist: file No. %d\n", nEntOffset);
		*nEmpFile = -3;
		return;
	}

	if ((ucAttr&ATTR_DIRECTORY) == ATTR_DIRECTORY) {
		numberofcluster = 1;
	}
	else {
		numberofcluster = (nLength+oFat.m_nBytesPerCluster-1)/oFat.m_nBytesPerCluster;
	}

	// sunny 060818
	if (nLen<=SF_NAME_LENGTH)
		entries = 1;				// short file name directory entry
	else
		entries = (nLen+12)/13 + 1;  // long file name directory entry

	FAT_FindFreeEntry(entries, &nDirCluster, &nOffset, sCh);	// find the free file entry for the current file
	UART_Printf("offset: %d, cluster: %d\n", nOffset, nDirCluster);
	FAT_FromClusterToLba(nDirCluster, &nSector, sCh);
	FAT_ReadSector(p, nSector, oFat.m_ucBpbSecPerClus, sCh);

	//Need to store the entry in the second root cluster, then need to update the FAT to fix the second root cluster
	if (entries!=1 &&(oFat.m_uDirEntryPerCulster-nOffset)<entries) 
	{
		pLastEntryAddr = (p + DIR_ENTRY_SIZE*(nOffset-1));
		FAT_GetEntryCluster(pLastEntryAddr, &uDIR_FstClus, sCh);
		uDIR_FileSize = LSB_GET_4BYTES(pLastEntryAddr + 28);
		// Get the next cluster which store the remainder file info.
		nNextCluster = uDIR_FstClus + (uDIR_FileSize+oFat.m_nBytesPerCluster-1)/oFat.m_nBytesPerCluster;

		FAT_UpdateRootCluInFat(nNextCluster, sCh);
	}

	FAT_UpdateFileDataCluInFat(numberofcluster, &uAlloCluster, sCh);
	if ((uHeadCluster=uAlloCluster)==0)
	{
		*nEmpFile = -4;      /* no disc free space */
		return;
	}

	if (entries==1)		// short file entry
	{
		//SetDateTime(0x309B, 0x762D);  // LYS 040426 LYS 040427
		FAT_WriteFileHead(cFname, ucAttr, nLength, uHeadCluster, &nFileEnt, sCh);  //it makes fill oFat.m_ucFileEntryHead content(32bytes)
		FAT_CopyAblockOfMem(p+nOffset*32, oFat.m_ucFileEntryHead, DIR_ENTRY_SIZE, sCh);
		FAT_WriteSector(p, nSector, oFat.m_ucBpbSecPerClus, sCh);
		*nEmpFile = uHeadCluster;
	}
	else	// Long file entry
	{
		FAT_ConvChNameToUniName((char*)cFname,&spLFUnicodeName, sCh);
		FAT_GenerateSNameFromLName(oFat.m_sUdLFname, cSfname, &nLossFlag, sCh);
		cTempSfname = cSfname;

		if ((oFat.m_uDirEntryPerCulster-nOffset)>=entries)  // the current cluster can hold all the long file dir entries
		{
			for (m=0; m<entries; m++)
			{
				if (m<(entries-1))
					FAT_PassoutLFileName(oFat.m_sUdLFname, (u8*)cSfname, entries-m-1, &nDirecEntry, sCh);
				else
					FAT_WriteSFFileHead(cTempSfname, ucAttr, nLength, uHeadCluster, &nFileEnt, sCh);

				FAT_CopyAblockOfMem(p+nOffset*32, oFat.m_ucFileEntryHead, DIR_ENTRY_SIZE, sCh);
				nOffset ++;
			}
			FAT_WriteSector(p, nSector, oFat.m_ucBpbSecPerClus, sCh);
			*nEmpFile = uHeadCluster;
		}
		else
		{
			nCurrDirNum = oFat.m_uDirEntryPerCulster-nOffset;
			nNextDirNum = entries-nCurrDirNum;
			for (m=0; m<nCurrDirNum; m++)
			{
				FAT_PassoutLFileName(oFat.m_sUdLFname, (u8*)cSfname, entries-m-1, &nDirecEntry, sCh);
				FAT_CopyAblockOfMem(p+nOffset*32, oFat.m_ucFileEntryHead, DIR_ENTRY_SIZE, sCh);
				nOffset ++;
			}
			FAT_WriteSector(p, nSector, oFat.m_ucBpbSecPerClus, sCh);

			FAT_FromClusterToLba(nNextCluster, &nSector, sCh);
			FAT_ReadSector(p, nSector, oFat.m_ucBpbSecPerClus, sCh);

			for(m=0; m<nNextDirNum; m++)
			{
				if (m!=(nNextDirNum-1))
					FAT_PassoutLFileName(oFat.m_sUdLFname, (u8*)cSfname, nNextDirNum-m-1, &nDirecEntry, sCh);
				else
					FAT_WriteFileHead(cTempSfname, ucAttr, nLength, uHeadCluster, &nFileEnt, sCh);

				FAT_CopyAblockOfMem(p+m*32, oFat.m_ucFileEntryHead, DIR_ENTRY_SIZE, sCh);
			}
			FAT_WriteSector(p, nSector, oFat.m_ucBpbSecPerClus, sCh);
			*nEmpFile = uHeadCluster;
		}
	}
}

/*****************************************************************************
 * Fat has two part: one is to link the file data, another is to link the root(store the file dir entry)       
 *****************************************************************************/

void FAT_UpdateFileDataCluInFat(int numberofcluster, int* uAlloCluster, SDHC* sCh)
{
	int i, start_sector=0, offset=0;
	int nCluster=2;
	int clucount=oFat.m_nBpbCountOfClusters+2;
	int curhead, curlength=0;
	int nNextClu;
	int nPreOffset;   //the offset of the previous cluster
	int nPreSector; 
	u8 ucFatBuff[512];
	u8 bCrossSectorFlag=false;
	u8 bFreeClustExist=false;
	int res;
	int sTmp;

	if (numberofcluster<=0) {
		*uAlloCluster = 0;
		return;
	}

	while (nCluster<clucount)
	{
		FAT_ClusterFatAddr(nCluster, &start_sector, &offset, sCh);  // offset: in bytes
		FAT_GetFat(start_sector, offset, &sTmp, &res, sCh);
		Assert(res>0);

		if ((sTmp&oFat.m_nMaskCluster) == FAT_FREE_CLUSTER)
		{
			bFreeClustExist = true;
			if (!curlength)	// the headcluster and the coresponding offset
			{
				nPreOffset =offset; 
				curhead = nCluster;
			}
			else
				nNextClu = nCluster;

			curlength++;
			if (curlength>1 && bCrossSectorFlag==false)
				FAT_SetFat(nPreOffset, nNextClu, oFat.m_pFatBuffStart, sCh);

			if (bCrossSectorFlag) {
				FAT_SetFat(nPreOffset, nNextClu, ucFatBuff, sCh);
				bCrossSectorFlag = false;
				FAT_WriteSector(ucFatBuff, nPreSector+oFat.m_uBpbHiddSec+oFat.m_nPosFat+SDCARD_OFFSET_SECTOR, 1, sCh);// sunny modify
			}

			nPreOffset =offset; 
			if (curlength>=numberofcluster) {
				FAT_SetFat(nPreOffset, oFat.m_nEOC, oFat.m_pFatBuffStart, sCh);
				FAT_WriteSector(oFat.m_pFatBuffStart, start_sector+oFat.m_uBpbHiddSec+oFat.m_nPosFat+SDCARD_OFFSET_SECTOR, 1, sCh);// sunny modify
				break;
			}
		}

		if (offset==508)
			//UART_Printf("just for debug\n");
			Putc('.');

		if (offset==508 && bFreeClustExist==true)	//the end of one sector
		{
			bFreeClustExist = false;
			nPreSector = start_sector;
			bCrossSectorFlag = true;
			for(i=0; i<512; i++)
				ucFatBuff[i] =*(oFat.m_pFatBuffStart++);
		}
		nCluster++;
	}
	*uAlloCluster = curhead;
}

void FAT_UpdateRootCluInFat(int nNextClu, SDHC* sCh)
{
	int  nStartClu = 2;
	int  sTmp;//res, 
	int  start_sector, offset;
	u8*  pFatTempBuff = oFat.m_pFatBuffStart;

	while(1)
	{
		FAT_ClusterFatAddr(nStartClu, &start_sector, &offset, sCh); 
		FAT_ReadSector(pFatTempBuff, start_sector+oFat.m_nPosFat+oFat.m_uBpbHiddSec+SDCARD_OFFSET_SECTOR, 1, sCh); 
		sTmp = (LSB_GET_4BYTES(pFatTempBuff+offset))&oFat.m_nMaskCluster;
		if((sTmp&oFat.m_nMaskCluster) == oFat.m_nEOC)
			break;
		else
			nStartClu = sTmp;
	}
	FAT_SetFat(offset, nNextClu, pFatTempBuff, sCh);
	FAT_WriteSector(pFatTempBuff, start_sector+oFat.m_uBpbHiddSec+oFat.m_nPosFat+SDCARD_OFFSET_SECTOR, 1, sCh);

	FAT_ClusterFatAddr(nNextClu, &start_sector, &offset, sCh);  // offset: in bytes
	FAT_ReadSector(pFatTempBuff, start_sector+oFat.m_nPosFat+oFat.m_uBpbHiddSec+SDCARD_OFFSET_SECTOR, 1, sCh);
	FAT_SetFat(offset, oFat.m_nEOC, pFatTempBuff, sCh);
	FAT_WriteSector(pFatTempBuff, start_sector+oFat.m_uBpbHiddSec+oFat.m_nPosFat+SDCARD_OFFSET_SECTOR, 1, sCh);
}
/******************************************************************
scan a continuous cluster block with specified size
if success return the specified nLength
if failed return the maximum nLength of continuous cluster block
nLength - specified nLength
nPosi - nHead cluster
******************************************************************/
void FAT_ScanCluster(int frag, int nLength, int *nPosi, int rstart, int rend, u32* uCluBlk, SDHC* sCh)  // LYS 040424 big question
{
	int start_sector, offset;
	int cluster=2;
	int clucount=oFat.m_nBpbCountOfClusters+2;
	int curhead, curlength=0;
	int prehead, prelength=0;

	while (cluster<clucount)
	{
		int res, sTmp; // uint? or int?
		if (rstart)
		{
			if (cluster>=rstart&&cluster<rend)
			{
				Assert(rend>rstart);
				cluster=rend;
				if (cluster>=clucount)
					break;
			}
		}
		FAT_ClusterFatAddr(cluster, &start_sector, &offset, sCh);  //
		//UART_Printf("start_sector: %d, offset: %d\n", start_sector, offset);  // LYS 040424
		FAT_GetFat(start_sector, offset, &sTmp, &res, sCh);
		if (res<=0)
			Assert(0);
		if ((sTmp&oFat.m_nMaskCluster) == FAT_FREE_CLUSTER) // 0
		{
			if (!curlength)
				curhead = cluster;
			if (++curlength>=nLength)
				break;
		}
		else if (!frag) {
			if (prelength<curlength) {
				prehead = curhead;
				prelength = curlength;
			}
			curlength = 0;
		}
		cluster++;
		///VCX_service();  //HW dependent routine
	} // end of while
	if (prelength>curlength)
	{
		curhead = prehead;
		curlength = prelength;
	}
	*nPosi = curhead;
	*uCluBlk = curlength;
}

/*******************************************************/
/* create a (small) file with the whole data in oFat.m_ucBuffer */
/*******************************************************/
void FAT_CreateSFile(int location, const char *lfname, u8 ucAttr, int nLength, u8 *data, int* nSFile, SDHC* sCh)
{
	int headcluster;
	int numberofsector;
	int remains; // LYS 040426
	int i;  // LYS 040426
	int nLba;
	int nLen;

	FAT_CreateFile(location, lfname, ucAttr, nLength, &headcluster, sCh);

	if (headcluster < 2)
	{ 
		*nSFile = -1;
		return;
	}
	numberofsector = (nLength+oFat.m_usBpbBytsPerSec-1) / oFat.m_usBpbBytsPerSec;

#if 1 // LYS 040426 fill the zero for the padding.. LYS 040617
	remains = nLength % oFat.m_usBpbBytsPerSec;
	for(i=0;i<SECBUFFSIZE-remains;i++)
	{
		*((volatile u8*)(data+nLength+i)) = 0;
	}
#endif
	FAT_FromClusterToLba(headcluster, &nLba, sCh);
	FAT_WriteSector(data, nLba, numberofsector, sCh);

	nLen = strlen(lfname);
	for (i=0; i<nLen; i++)
		oFat.m_pDDE[oFat.m_nFileIndex].filename[i] = *(lfname+i);
	oFat.m_pDDE[oFat.m_nFileIndex].dirflag = ucAttr;
	oFat.m_pDDE[oFat.m_nFileIndex].location = headcluster;
	oFat.m_pDDE[oFat.m_nFileIndex].nFileSz = nLength;
	oFat.m_nFileIndex++;
	*nSFile = headcluster;
}


void FAT_WriteSFile(char *fileName, int nLength, u8* srcAddr, int* nWrFile, SDHC* sCh)
{
	int nSFile;
	FAT_CreateSFile(FIRST_DATA_CLUSTER, fileName, ATTR_ARCHIVE, nLength, srcAddr, &nSFile, sCh);

	if(nSFile>=0) {
		FAT_SetFSInfo(sCh);
		FAT_CopyFatTableToBackup(sCh);
		*nWrFile = 1;
	}
	else {
		*nWrFile = 0;
	}
}


/********************************************************/
/* create a file as directory 				            */
/********************************************************/
// headsector: the cluster to write '.', '..'
// return value: headsector
void FAT_CreateDirectory(int location, char *dirname, int* nCrFile, SDHC* sCh)
{
	//uchar *hp = oFat.m_ucDirBuffStart;
	u8* hp;
	int headsector, headcluster;
	u8* ucSpecEnt1;
	u8* ucSpecEnt2;

	FAT_InitBuffDirMem(&hp, sCh);  // Caution: CreateFile() uses oFat.m_pBufferFileMem[]...

	FAT_CreateFile(location, dirname, ATTR_DIRECTORY, 0, &headcluster, sCh);
	UART_Printf("dir create: headcluster: %d\n", headcluster);
	if (headcluster<2)
		*nCrFile = -1;
	FAT_FromClusterToLba(headcluster, &headsector, sCh);
	UART_Printf("dir create: headsector: %d, oFat.m_nPosRoot: %d\n", headsector, oFat.m_nPosRoot);  //2022

	if (headsector != oFat.m_nPosRoot)
	{
		FAT_ReadSector(hp, headsector, 1, sCh);
		FAT_GenerateDirEntry(DOT_ENTRY_TAG, headcluster, &ucSpecEnt1, sCh);
		FAT_CopyAblockOfMem(hp, ucSpecEnt1, DIR_ENTRY_SIZE, sCh);
		//hp += DIR_ENTRY_SIZE;
		//CopyAblockOfMem(hp, GenerateDirEntry(DOTDOT_ENTRY_TAG, nPwd), DIR_ENTRY_SIZE);
		FAT_GenerateDirEntry(DOTDOT_ENTRY_TAG, oFat.m_nPwd, &ucSpecEnt2, sCh);
		FAT_CopyAblockOfMem(hp+DIR_ENTRY_SIZE, ucSpecEnt2, DIR_ENTRY_SIZE, sCh);
		//hp += DIR_ENTRY_SIZE;
		FAT_CopyEntry(hp+2*DIR_ENTRY_SIZE, TERM_ENTRY_TAG, sCh);
		FAT_WriteSector(hp, headsector, 1, sCh);  // directory create
	}
	*nCrFile = headcluster;
}

/********************************************************/
/* delete file from hd with unicode long file name      */
/********************************************************/
void FAT_DeleteFile(char *cSfname, u8 ucAttr, int* nDelFile, SDHC* sCh)  //ucAttr not used...
{
	u8 p[SECBUFFSIZE*MAXSEC_PER_CLUSTER];
	int sector, offset;//, cluster;
//	int index=-1;
	int clusterLength;  //LYS 040510
	u32 nHeadCluster; //kst 060308
	int nRelCluster;
#if 1 // LYS 040114
	//cluster = FindClusterFromName(cSfname);
	int nEntOffset;
	FAT_FindDirEntryOffsetFromName(cSfname, &nEntOffset, sCh);
	if((offset=nEntOffset) < 0)
	{ // file or directory non exist, can not delete..
		*nDelFile = -3;
	}
	//cluster = (location)? location:oFat.m_uBpbRootClus;
	//cluster = FindDirEntryClusterFromName(cSfname);
	//sector = FromClusterToLba(cluster);  //sector means DIR entry.....
	///offset = DIR_ENTRY_SIZE * index;
	sector = oFat.m_nFirstDataSector;  //LYS 040508
	FAT_FindClusterFromName(cSfname, &nHeadCluster, sCh);
	//UART_Printf("cluster: %d, sector: %d, offset: %d, nHeadCluster: %d in file_delete()\n", cluster, sector, offset, nHeadCluster);
#endif
	/* delete short name entry */
	FAT_ReadSector(p, sector, /*2*/oFat.m_ucBpbSecPerClus, sCh);
	clusterLength = LSB_GET_4BYTES(p+28); // LYS 040510
	FAT_CopyEntry(p+offset, FREE_ENTRY_TAG, sCh);

	/* write back to disc and release cluster chain */
	FAT_WriteSector(p, sector, /*2*/oFat.m_ucBpbSecPerClus, sCh);
	//ReleaseClusterChain(nHeadCluster, 1);  // LYS 040424 big question

	FAT_ReleaseClusterChain(nHeadCluster, clusterLength, &nRelCluster, sCh);  // LYS 040510 fat table erase

	*nDelFile = 0;
}


// LYS 040512
void FAT_File_Delete(char* fileName, int* nFileDel, SDHC* sCh)
{
	int nDelFile;

	FAT_DeleteFile(fileName, ATTR_ARCHIVE, &nDelFile, sCh);
	if(nDelFile >= 0)
	{
		FAT_SetFSInfo(sCh);
		FAT_CopyFatTableToBackup(sCh);
		*nFileDel = 1;
	}
	else
	{
		*nFileDel = 0;
	}
}




/********************************************************************/
/* create a empty file with specified size on hd, lfname in unicode */
/********************************************************************/
void FAT_FileCreateCluster(int location, short *lfname, ISOFILE *oldpf, int* nFCluser, SDHC* sCh)
{
	ISOFILE pfile;
	char cSfname[SF_NAME_LENGTH+1];
	//uchar *p=oFat.m_ucDirBuffStart;
	u8* p;
	int  i, entries, prev_sec;
	int sector, offset, cluster, nLen;
	int temp;
	int nSfnameconv;
	int nSpecFile1;
	int nSpecFile2;

	FAT_GetUnicodeStrLen(lfname, &nLen, sCh);
	FAT_InitBuffFileMem(&p, sCh);  // LYS 040407

	if (nLen<=0) {
		Assert(0);
		*nFCluser = -1;
	}

	FAT_GenerateSNameFromLName(lfname, cSfname, &temp, sCh);
	FAT_FormatSFileName(oFat.m_ucFsfname, cSfname, &nSfnameconv, sCh);
	if (nSfnameconv < 0)
		*nFCluser = -2;

	FAT_SearchDirFileEntry(location, lfname, 1, ATTR_DIR_FILES, &pfile, &sector, &offset, &cluster, &nSpecFile1, sCh);
	if (nSpecFile1)
		*nFCluser = -3;      /* file exist in target directory */

	entries = (nLen+12)/13+1;

	FAT_SearchDirFileEntry(location, 0, entries, 0, 0, &sector, &offset, &cluster, &nSpecFile2, sCh);
	if (!nSpecFile2)
		*nFCluser = -4; /* no more dir entry space */

	/* create file nHead */
	prev_sec = sector;
	FAT_ReadSector(p, sector, 1, sCh);
	for (i=0; i<entries; i++) {
		if (i) {
			//int nDirecEntry;
			//PassoutLFileName(lfname, i, nDirecEntry);
		}
		else {
			int nFileEnt;
			FAT_WriteFileHead((char*)oFat.m_ucFsfname, oldpf->ucAttr, oldpf->size, oldpf->addr, &nFileEnt, sCh);
		}
		Assert(sector>=oFat.m_nPosRoot);
		Assert(offset>=0);
		FAT_CopyAblockOfMem(p+offset, oFat.m_ucFileEntryHead, DIR_ENTRY_SIZE, sCh);
		offset -= DIR_ENTRY_SIZE;
		if (offset<0&&sector>oFat.m_nPosRoot)
		{
			sector--;
			offset += oFat.m_usBpbBytsPerSec;
		}
		if (prev_sec!=sector )
		{
			///HD_write(prev_sec, 1, (short *)p);
			FAT_WriteSector(p, prev_sec, 1, sCh);
			FAT_ReadSector(p, sector, 1, sCh);
			prev_sec = sector;
		}
	}
	///HD_write(sector, 1, (short *)p);
	FAT_WriteSector(p, sector, 1, sCh);
	*nFCluser = sector;
}


/********************************************************/
/* delete file from hd with unicode long file name      */
/********************************************************/
void FAT_RenameFile(int location, short *nlfname, short *lfname, u8 ucAttr, int* nReFile, SDHC* sCh)
{
	char cSfname[SF_NAME_LENGTH+1];
	u8 Ord, Attr;
	u8 *p;
	int sector, offset, cluster, nLen;//nHeadCluster,
	ISOFILE pfile;
	int temp;
	int nSfnameconv;
	int nSpecFile;
	int nFCluser;
	int nLba;

	FAT_InitBuffFileMem(&p, sCh);

	/* new file name check */
	FAT_GetUnicodeStrLen(nlfname, &nLen, sCh);
	if (nLen<=0) {
		Assert(0);
		*nReFile = -1;
	}

	FAT_GenerateSNameFromLName(nlfname, cSfname, &temp, sCh);
	FAT_FormatSFileName(oFat.m_ucFsfname, cSfname, &nSfnameconv, sCh);
	if (nSfnameconv < 0)
		*nReFile = -2;
	/* old file name check */
	FAT_SearchDirFileEntry(location, lfname, 1, ATTR_DIR_FILES, &pfile, &sector, &offset, &cluster, &nSpecFile, sCh);
	if (!nSpecFile)
		*nReFile = -1;      /* no file exist in current directory */
//	if ((nHeadCluster=pfile.addr)==0)
	if (pfile.addr==0)
		*nReFile = -2;      /* funny file means disc ucNerror happens */

	/* delete short name entry */
	FAT_ReadSector(p, sector, 1, sCh);
	FAT_CopyEntry(p+offset, FREE_ENTRY_TAG, sCh);
	do
	{ /* long file entry checking */
		if (offset > 0)
			offset -= DIR_ENTRY_SIZE;
		else
		{
			Assert(offset==0);

			FAT_FromClusterToLba(cluster, &nLba, sCh);
			if (sector > nLba)
			{
				sector--;
				offset = oFat.m_ucBpbSecPerClus-DIR_ENTRY_SIZE;
				FAT_ReadSector(p, sector, 1, sCh);
			}
			else
				break;
		}
		Ord = (p+offset)[0];
		Attr = (p+offset)[11];
		if (Ord==FREE_ENTRY_TAG0)
			break;
		if ((Attr&ATTR_LONG_NAME)!=ATTR_LONG_NAME)
			break;
		FAT_CopyEntry(p+offset, FREE_ENTRY_TAG, sCh);
		if ((Ord&0x40)==0x40)
			break;
	} while (1);
	/* write back to disc and release cluster chain */
	///HD_write(sector, 1, (short *)p);

	FAT_FileCreateCluster(location, nlfname, &pfile, &nFCluser, sCh);
	*nReFile = 0;
}

/********************************************************/
/* copy file from hd to hd with unicode long file name  */
/********************************************************/
void FAT_CopyFile(short *lfname, short *lfname1, int* nCopFile, SDHC* sCh)
{
	int sector, offset, cluster;//nHeadCluster, 
	ISOFILE pfile, pfile1;
	int target;
	int nSpecFile1;
	int nSpecFile2;
	int nDelFile;
	int nEmpFile;

	target = oFat.m_nTwd;

	FAT_SearchDirFileEntry(oFat.m_nPwd, lfname1, 1, ATTR_DIR_FILES, &pfile1,
		&sector, &offset, &cluster, &nSpecFile1, sCh);
	if (!nSpecFile1)
		*nCopFile = -1;      /* no such file */

	FAT_SearchDirFileEntry(target, lfname, 1, ATTR_DIR_FILES, &pfile,
		&sector, &offset, &cluster, &nSpecFile2, sCh);
	if (nSpecFile2) {
		FAT_DeleteFile((char*)lfname, ATTR_DIR_FILES, &nDelFile, sCh);  //lfname temp
	}

	FAT_CreateFile(target, (char*)lfname, 0, pfile1.size, &nEmpFile, sCh);
//	if ((nHeadCluster = nEmpFile) < 0)
	if (nEmpFile < 0)
		*nCopFile = -2;
	//HD_set_ctsk(nHeadCluster, pfile1.addr, pfile1.size, CLUSTER_CHAIN); // status and copy manager
	// copy ok message need..
	*nCopFile = 0;
}

/********************************************************/
/* convert char name to unicode name 			*/
/* modify spUnicode->level 2 pointer                       */
/********************************************************/
void FAT_ConvChNameToUniName(char *p, short** spUnicode, SDHC* sCh)
{
	int nConvUni;
	FAT_ConvertUnicode(oFat.m_sUdLFname, p, CHAR2UNICODE, &nConvUni, sCh);
	*spUnicode = oFat.m_sUdLFname;
}


/*******************************************************/
/* HD_quick_format                                     */
/*******************************************************/
void FAT_QuickFormat(SDHC* sCh)
{
	u8 *p = oFat.m_pFatBuffStart;
	int i, entry, sector, offset;

	FAT_InitHardDisk(sCh);
	/* make clean data */
	for (i=0; i<SECBUFFSIZE/*256*/; i++) {
		p[i] = 0;
	}
	entry = (oFat.m_nEOC & 0xffffff00) | (oFat.m_ucBpbMedia & 0xff);
	FAT_ClusterFatAddr(0, &sector, &offset, sCh); // 0번째 cluster의 physical addr(sector, offset)을 알아낸다.
	FAT_SetFat(offset, entry, oFat.m_pFatBuffStart, sCh);
	FAT_ClusterFatAddr(1, &sector, &offset, sCh); // 1번째 cluster의 physical addr(sector, offset)을 알아낸다.
	FAT_SetFat(offset, oFat.m_nEOC, oFat.m_pFatBuffStart, sCh);
	oFat.m_nLastFatChanged = 0;
	oFat.m_nLastFatSector = -1;

	/* write back to hard disc */
	///HD_write(oFat.m_nPosFat, 1, p);
	///HD_write(oFat.m_nPosFatb, 1, p);
	FAT_WriteSector(p, oFat.m_nPosFat, 1, sCh);
	FAT_WriteSector(p, oFat.m_nPosFatb, 1, sCh);

	for (i=0; i<4; i++)
		oFat.m_pFatBuffer[i] = 0;  //first fat_entry makes 0
	///for (i=1; i<oFat.m_nFatSz; i++) HD_write(oFat.m_nPosFat+i, 1, p);
	///for (i=1; i<oFat.m_nFatSz; i++) HD_write(oFat.m_nPosFatb+i, 1, p);
	///HD_write(posRoot, 1, oFat.m_pFatBuffer);
	for (i=1; i<oFat.m_nFatSz; i++)
		FAT_WriteSector(p, oFat.m_nPosFat+i, 1, sCh);
	for (i=1; i<oFat.m_nFatSz; i++)
		FAT_WriteSector(p, oFat.m_nPosFatb+i, 1, sCh);
	FAT_WriteSector(oFat.m_pFatBuffer, oFat.m_nPosRoot, 1, sCh);
}

// kst 0302
void FAT_Init(SDHC* sCh)
{
	oFat.m_nLastFreeCluster=2;
	oFat.m_nLastFatChanged=0;
	oFat.m_nLastFatSector=-1;
	oFat.m_nTwd=-1;
	oFat.m_nPwd=-1;
	oFat.m_nDepthNow=0;
	oFat.m_nFileIndex=0;
}

u8 FAT_InitHardDisk(SDHC *sCh)
{
	u8 buff[SECBUFFSIZE];
	int i;

	for(i=0;i<SECBUFFSIZE;i++)
		buff[i] = 0;
	FAT_Init(sCh);

	/* Read Partition Table */
	FAT_SetupBoot(buff, sCh);
	if(FATType == IS_FAT32)
		FAT_GetFSInfo(sCh);

	FAT_InitBuff(sCh);

	return true;
}

void FAT_InitBuff(SDHC *sCh)
{
//	int i;
	oFat.m_nLastFatSector = -1;
	oFat.m_nLastFatChanged = 0;

	// read the the first sector of FAT1 to the addr--FatBuffStart
	//ReadSector(oFat.m_pFatBuffStart, oFat.m_nPosFat+SDCARD_OFFSET_SECTOR, 1);
	FAT_ReadSector(oFat.m_pFatBuffStart, oFat.m_nPosFat+oFat.m_uBpbHiddSec+SDCARD_OFFSET_SECTOR, 1, sCh);//sunny modify

	oFat.m_nFatBuffSize = oFat.m_usBpbBytsPerSec;    // must be one sector size 512 //
	oFat.m_nDirBuffSize=  oFat.m_usBpbBytsPerSec;    // must be oFat.m_usBpbBytsPerSec //

	// pTransBuffStart = (uchar *)draoFat.m_cached(SBV_start)+oFat.m_nDirBuffSize;
	oFat.m_nTransBuffSize = oFat.m_nBytesPerCluster; // must be one cluster size 4096 //
}

// FAT32 only
void FAT_GetFSInfo(SDHC *sCh)
{
	UART_Printf("GetFSInfo\n");
	FAT_ReadSector(oFat.m_pFsInfo, oFat.m_ucBpbFsInfo+SDCARD_OFFSET_SECTOR+oFat.m_uFirstSectorAfterMbr, 1, sCh);

	oFat.m_uFsiLeadSig = LSB_GET_4BYTES(oFat.m_pFsInfo+0);
	Assert(oFat.m_uFsiLeadSig == 0x41615252);

	oFat.m_uFsiFreeCount = LSB_GET_4BYTES(oFat.m_pFsInfo+488);
	oFat.m_uFsiNxtFree = LSB_GET_4BYTES(oFat.m_pFsInfo+492);
	UART_Printf("oFat.m_uFsiFreeCount: %d, oFat.m_uFsiNxtFree: %d in GetFSInfo()\n", oFat.m_uFsiFreeCount, oFat.m_uFsiNxtFree);
}

void FAT_SetFSInfo(SDHC *sCh)
{
	int  index;
	int  offset, clusterLength;
	int  maxCluster, tempCluster;
	int  length;
	u8   attr1;
	int i;
	UART_Printf("SetFSInfo\n");

	// To find max cluster
	maxCluster = 0;
	for (i=0; i<oFat.m_nFileIndex; i++) {
		tempCluster = oFat.m_pDDE[i].location;
		if (maxCluster < tempCluster) {
			maxCluster = tempCluster;
			index = i;
		}
	}

	length = oFat.m_pDDE[index].nFileSz;
	attr1 = oFat.m_pDDE[index].dirflag;
	offset = length % oFat.m_nBytesPerCluster;
	clusterLength = length /oFat.m_nBytesPerCluster;

	UART_Printf("maxCluster: %d, nLength: %d\n", maxCluster, length);
	oFat.m_uFsiNxtFree = maxCluster + clusterLength + ((offset>0)?1:0) + ((attr1 ==0x10)?1:0);
	oFat.m_uFsiFreeCount = (oFat.m_uBpbTotSectors - ((oFat.m_uFsiNxtFree-2)*oFat.m_ucBpbSecPerClus + oFat.m_nFirstDataSector))/oFat.m_ucBpbSecPerClus;  //LYS 040430 modify (important!!)

	UART_Printf("oFat.m_uFsiNxtFree: %d, oFat.m_uFsiFreeCount: %d in SetFSInfo()\n", oFat.m_uFsiNxtFree, oFat.m_uFsiFreeCount);

	LSB_SET_4BYTES(oFat.m_pFsInfo+488, oFat.m_uFsiFreeCount);
	LSB_SET_4BYTES(oFat.m_pFsInfo+492, oFat.m_uFsiNxtFree);
	oFat.m_uFsiLeadSig = 0x41615252;
	LSB_SET_4BYTES(oFat.m_pFsInfo+0, oFat.m_uFsiLeadSig);
	FAT_WriteSector(oFat.m_pFsInfo, 1+oFat.m_uFirstSectorAfterMbr+SDCARD_OFFSET_SECTOR, 1, sCh);
}

#if 1
void FAT_BootRealloc(u8* buff, SDHC *sCh)
{
	u8* ptr;
	u16 sectorEnd;
	u8 firstByte;
	//u32 firstSectorAfterMbr;
	u32 totalSectorNum;

	sectorEnd = LSB_GET_2BYTES(buff+510);
	if(sectorEnd != 0xaa55) {   //should be buff[510] == 0x55, buff[511] == 0xaa
		UART_Printf("Error: No FAT boot sector!!\n");
		return;
	}

	firstByte = *((u8*)(buff+0));
	if(firstByte == 0xeb || firstByte == 0xe9) {
		oFat.m_uFirstSectorAfterMbr =0;
		UART_Printf("FAT file system: boot sector is 0th sector.\n");
		return;
	}

	ptr = (u8*)(buff+0x1be);
	/// firstSectorAfterMbr = LSB_GET_4BYTES(ptr+0x08);
	oFat.m_uFirstSectorAfterMbr = LSB_GET_4BYTES(ptr+0x08); ///
	totalSectorNum = LSB_GET_4BYTES(ptr+0x0c);

	UART_Printf("oFat.m_uFirstSectorAfterMbr=0x%x\n", oFat.m_uFirstSectorAfterMbr); ///
	UART_Printf("totalSectorNum = %d\n", totalSectorNum );
	FAT_ReadSector(buff, oFat.m_uFirstSectorAfterMbr, 1, sCh);  // read reallocated boot sector
	return;
}
#endif

/************************************************************************/
/* Sets up the location of the FAT.                                     */
/************************************************************************/
void FAT_SetupBoot(u8 *buff, SDHC *sCh)
{
	int rightSectorEnd;
	u8 BPB_NumFATs;
//	int i;
	UART_Printf("SetupBoot\n");
	FAT_ReadSector(buff, 0+SDCARD_OFFSET_SECTOR, 1, sCh);
	// ReadSector(buff, 2, 1);
	FAT_BootRealloc(buff, sCh);

	// sanity check for Signature word of Sector 0 of FAT volume
	rightSectorEnd = LSB_GET_2BYTES(buff+510);
	Assert(rightSectorEnd == 0xaa55);    // buff[510] == 0x55, buff[511] = 0xaa

	oFat.m_nPosFat = LSB_GET_2BYTES(buff+14); // ReservedSectors, it already inlucde the boot sector
	// Compute 1st sector of cluster 2(data region of the disk)
	oFat.m_usBpbRootEntCnt = LSB_GET_2BYTES(buff+17);
	oFat.m_usBpbBytsPerSec = LSB_GET_2BYTES(buff + 11);
	// We are hardwired to 512 bytes per sector throughout
	Assert(oFat.m_usBpbBytsPerSec == 512);
	oFat.m_nDWordPerSector = oFat.m_usBpbBytsPerSec>>2;
	oFat.m_usRootDirSectors = ((oFat.m_usBpbRootEntCnt *DIR_ENTRY_SIZE) + (oFat.m_usBpbBytsPerSec -1)) / oFat.m_usBpbBytsPerSec;
	oFat.m_nFatSz = LSB_GET_2BYTES(buff+22);
	if (!oFat.m_nFatSz) {
		oFat.m_nFatSz = LSB_GET_4BYTES(buff+36);
	}
	oFat.m_nPosFatb = oFat.m_nPosFat + oFat.m_nFatSz;   // oFat.m_nPosFat: FAT1 starting sector, oFat.m_nPosFatb: FAT2 starting sector.
	UART_Printf("oFat.m_nPosFat: %d, oFat.m_nPosFatb: %d, oFat.m_nFatSz: %d\n", oFat.m_nPosFat, oFat.m_nPosFatb, oFat.m_nFatSz);
	BPB_NumFATs = *(buff+16);  // Number of FATS
	oFat.m_ucBpbMedia = * (buff+21); // Number of FATS
	oFat.m_uBpbHiddSec = LSB_GET_2BYTES(buff+28); //reallocation factor
	/* posRoot is real posRoot only for FAT12/16 */
	oFat.m_nPosRoot = oFat.m_nPosFat + (BPB_NumFATs*oFat.m_nFatSz) + oFat.m_uBpbHiddSec;

	oFat.m_nFirstDataSector = oFat.m_nPosRoot + oFat.m_usRootDirSectors;

	UART_Printf("oFat.m_nFirstDataSector: %d\n", oFat.m_nFirstDataSector);

	/* FAT Type Determination */
	{
		u32 DataSec;
		oFat.m_ucBpbSecPerClus = *(buff+13);

		Assert(oFat.m_ucBpbSecPerClus <= MAXSEC_PER_CLUSTER); //dboy 060509

		oFat.m_uBpbTotSectors = LSB_GET_2BYTES(buff+19);
		if (!oFat.m_uBpbTotSectors) {
			oFat.m_uBpbTotSectors = LSB_GET_4BYTES(buff+32);
		}
		DataSec = oFat.m_uBpbTotSectors - oFat.m_nFirstDataSector;
		oFat.m_nBpbCountOfClusters = DataSec / oFat.m_ucBpbSecPerClus;
		oFat.m_uBpbRootClus = 0;

		if (oFat.m_nBpbCountOfClusters < 4085) {
			FATType = IS_FAT12;
			oFat.m_unBadCluster = 0x0ff7;
			oFat.m_nMaskCluster = 0x0fff;
			oFat.m_nEOC = 0x0ff8;
		}
		else if (oFat.m_nBpbCountOfClusters < 65525) {
			FATType = IS_FAT16;
			oFat.m_unBadCluster = 0xfff7;
			oFat.m_nMaskCluster = 0xffff;
			oFat.m_nEOC = 0xfff8;
		}
		else {
			FATType = IS_FAT32;
			oFat.m_unBadCluster = 0x0ffffff7;
			oFat.m_nMaskCluster = 0x0fffffff;
			oFat.m_nEOC = 0x0fffffff;   //oFat.m_nEOC = 0x0ffffff8;
			oFat.m_uBpbRootClus = LSB_GET_4BYTES(buff+44);  // root dir의 1st cluster의 cluster number (normally 2)
			FAT_FromClusterToLba(oFat.m_uBpbRootClus, &oFat.m_nPosRoot, sCh);
			oFat.m_ucBpbFsInfo = LSB_GET_2BYTES(buff+48); 
		}
		oFat.m_nTwd = oFat.m_nPwd = 0;

		oFat.m_nBytesPerCluster = oFat.m_usBpbBytsPerSec * oFat.m_ucBpbSecPerClus;
		oFat.m_nDWordPerCluster = (oFat.m_nBytesPerCluster>>2);
		if(FATType == IS_FAT32) {
			UART_Printf("FATType: FAT%d, oFat.m_uBpbRootClus=%x\n", FATType*4, oFat.m_uBpbRootClus);
			oFat.m_uDirEntryPerCulster = oFat.m_nBytesPerCluster/32;
		} 
		else {  // Other than FAT32, RootClus is 0
			UART_Printf("FATType: FAT%d\n", FATType*4);
			oFat.m_uDirEntryPerCulster = (FATType == IS_FAT16) ? oFat.m_nBytesPerCluster/16 : oFat.m_nBytesPerCluster/12;
		}
		UART_Printf("oFat.m_nBpbCountOfClusters: %d, oFat.m_nBytesPerCluster:%d, oFat.m_ucBpbSecPerClus:%d\n",
		oFat.m_nBpbCountOfClusters, oFat.m_nBytesPerCluster, oFat.m_ucBpbSecPerClus);
	}
}

/**************************************************************************/
/* Return the location of the next directory sector. Note that for FAT12  */
/* and FAT16, the root directory is of a fixed size and the normal cluster*/
/* chaining does not apply.                                               */
/**************************************************************************/
void FAT_NextDirSector(int where, int* nNexDiSec, SDHC *sCh)
{
	static int sectors_left_to_read_in_cluster;
	static int where_now, cluster_now;
	static int chain_type;      // 1 means flat (root & !fat32). 0 means cluster based.

	if (where != 0)    // Start of a new chain //
	{
		chain_type = 0;
		sectors_left_to_read_in_cluster = oFat.m_ucBpbSecPerClus - 1;
		if (where == -1) // Root dir //
		{
			where_now = oFat.m_nPosRoot;
			if (FATType == IS_FAT32)
				cluster_now = oFat.m_uBpbRootClus;
			else
				chain_type = 1;
		}
		else   // Normal dir //
		{
			FAT_FromClusterToLba(where, &where_now, sCh);
			cluster_now = where;
		}
	}
	else // Get next directory sector //
	{
		if (chain_type)
			where_now++;
		else
		{
			if (sectors_left_to_read_in_cluster == 0)
			{
				int nNextClu;
				FAT_GetNextCluster(cluster_now, &nNextClu, sCh);
				cluster_now = nNextClu;
				FAT_FromClusterToLba(cluster_now, &where_now, sCh);
				sectors_left_to_read_in_cluster = oFat.m_ucBpbSecPerClus - 1;
			}
			else
			{
				where_now++;
				sectors_left_to_read_in_cluster--;
			}
		}
	}
	if(cluster_now == -1)
		*nNexDiSec = -1;
	else
		*nNexDiSec = where_now;
}


/************************************************************************/
/* Read and fill in a FAT32 directory info. Return the number of        */
/* entries in the directory.                                            */
/************************************************************************/
void FAT_ReadDirFat32( int where, SDHC *sCh)
{
	int cur_cluster;
	int start, ret;// i,
	int nLongDirStatus =0;
//	u8 ucBufferSec[SECBUFFSIZE*MAXSEC_PER_CLUSTER];
	cur_cluster = where;
	oFat.m_nDirCluster[oFat.m_uDirClusterNum] = cur_cluster;
	do
	{
		FAT_FromClusterToLba(cur_cluster, &start, sCh);
		//ReadSector(&(ucBufferSec[0]), start, oFat.m_ucBpbSecPerClus);
		//CopyArray(oFat.m_pSecBuff[0], &(ucBufferSec[0]), oFat.m_nBytesPerCluster);
		FAT_ReadSector(oFat.m_pSecBuff[0], start, oFat.m_ucBpbSecPerClus, sCh);	// sunny 060818

		ret = FAT_FindClusterInDirectory(&cur_cluster, &nLongDirStatus, sCh);
		if (ret!=0) {
			oFat.m_uDirClusterNum ++;
			oFat.m_nDirCluster[oFat.m_uDirClusterNum] = cur_cluster;
		}
	}
	while (ret!=0);
}

void FAT_ConvertToDiscEntryFormat(const char* s, char* cpDiscEntFormat, SDHC *sCh)  // Convert filename to DiscEntryFormat
{
	int i, j, k;
	int s_len;
	int bingo;
	///char nameTemp[SF_NAME_LENGTH+1];
	#define DOTDETECT 1
	#define ENDNAME 2
	// .을 기준으로 이름과 확장자의 분리.
	// return value: oFat.m_pDDE[i].filename와 같은 format
	for(i=0; i<SF_NAME_LENGTH; i++)  // initialize the array
	{
		cpDiscEntFormat[i] = ' ';
	}
	cpDiscEntFormat[SF_NAME_LENGTH] = 0;
	bingo = 0;
	s_len = strlen(s);
	for (i=0; i<s_len; i++)
	{
		if (s[i] == 0x2e)  //'.'
		{
			if (i==0)
			{
				UART_Printf("Illegal name convention\n");
				cpDiscEntFormat = 0;
			}
			else
			{
				bingo = DOTDETECT;
				break;
			}
		}
		else if (s[i] == 0x00)
		{
			bingo = ENDNAME;
			break;
		}
		else
		{	
			if (s[i]>=0x61 && s[i]<=0x7a)	// lower case
				cpDiscEntFormat[i] = s[i] - 0x20;
			else
				cpDiscEntFormat[i] = s[i] ;
		}
	}
	j=i;

	if (bingo == DOTDETECT)
	{
		for(i=j+1, k=SF_BASE_LENGTH; i<s_len; i++, k++) //SF_BASE_LENGTH == 8
		{
			if (s[i] == 0x00)
			{
				break;
			}
			else
			{
				if (s[i]>=0x61 && s[i]<=0x7a)	// lower case
					cpDiscEntFormat[k] = s[i] - 0x20;
				else
					cpDiscEntFormat[k] = s[i] ;
			}
		}
	}
	else if (bingo == ENDNAME) // '\0' found
	{
	}
	else  // . no found
	{
	}
	///cpDiscEntFormat = nameTemp;
}

void FAT_CompareFName(const char* s, char* t, int* nFname, SDHC *sCh)
{
	int i;
	char* nameConverted;  // LYS 040112 added
	// .을 기준으로 이름과 확장자의 분리.
	// t: oFat.m_pDDE[i].filename
	FAT_ConvertToDiscEntryFormat(s, nameConverted, sCh);
	for (i=0; i<SF_NAME_LENGTH; i++)
	{
		if (nameConverted[i] == t[i])
		{
			continue;
		}
		else if (nameConverted[i]<t[i])
		{
			*nFname = -1;
		}
		else if (nameConverted[i]>t[i])
		{
			*nFname = 1;
		}
	}
	*nFname = 0;
}

void FAT_CompareFileNames(const char* s, const char* t, int* nFileName, SDHC *sCh) /// 3.7
{
	int i;
	int length=0;

	for (i=0; i<256; i++)
	{
		if(*(s+i) == 0)
			break;
		length++;
	}
//	UART_Printf("file name length=%d\n", length);

	for (i=0; i<length; i++)
	{
		if (s[i] == t[i])
		{
			continue;
		}
		else if (length <= 12) // short file name
		{
			if (s[i] - t[i] == 0x20) // same letter with different case
			{
				continue;
			}
			else
			{
				*nFileName = 1;
				return;
			}

		}
		else if (s[i] < t[i])
		{
			*nFileName = -1;
			return;
		}
		else if (s[i] > t[i])
		{
			*nFileName = 1;
			return;
		}
	}
	*nFileName = 0;
}

void FAT_GetFileIndex(const char* s, s32* index, SDHC *sCh) /// 3.7
{
	int i;
	int j=0;

	for(i=0; i<DDEMAXSIZE; i++)
	{
		int nFname;
		FAT_CompareFileNames(s, oFat.m_pDDE[i].filename, &nFname, sCh);
		if(!nFname) {
			j=1;
			break;
		}
	}
	UART_Printf("file index: %d\n", i);

	*index = (j==1)? i:-1;
}

u8 FAT_GetFileIndex1(const char *pFileName, const char *pFileExt, u32* uIdx, SDHC *sCh)
{
	char aFileName[MAX_FILE_LENGTH];
	u32 idx, uRefIdx;

	strcpy(aFileName, pFileName);
	idx = strlen(pFileName);
	while (1)
	{
		if (aFileName[--idx] == '.')
			break;
	}
	strcpy(&aFileName[idx+1], pFileExt);

	uRefIdx = *uIdx;

	for(uRefIdx=0; uRefIdx < DDEMAXSIZE; uRefIdx++)
	{
		if (strcmp(aFileName, oFat.m_pDDE[uRefIdx].filename) == 0)
			break;
	}

	*uIdx = uRefIdx;

	if (*uIdx != DDEMAXSIZE)
		return true;
	else
		return false;
}

//void FAT_GetFileName(s32 index, const char* fileName, SDHC *sCh)
void FAT_GetFileName(s32 index, char* fileName, SDHC *sCh)
{
	Assert(index < DDEMAXSIZE);
//	fileName = oFat.m_pDDE[index].filename;	
	strcpy(fileName, oFat.m_pDDE[index].filename);
}

void FAT_GetFileExt(s32 index, char* fileExt, SDHC *sCh)
{
	u32 i;
	u32 length=0;
	int j = -1;
	u32 dotPos[5];  // 5: arbitrary number
	u32 uExtIdx;

	Assert(index < DDEMAXSIZE);

	for (i=0; i<256; i++)
	{
		if(*((char*)(oFat.m_pDDE[index].filename+i)) == 0)
			break;
		length++;
	}

	for (i=0; i<length; i++)
	{
		if (oFat.m_pDDE[index].filename[i] == '.')
		{
			j++;
			dotPos[j] = i;
			//UART_Printf("i=%d\n", dotPos[j]);
		}
	}
	if (j == -1)
		//fileExt = (char*)0;
		strcpy(fileExt, (char*)0); // khlee

	else
	{
		uExtIdx = dotPos[j]+1;
		//fileExt = (char*)(oFat.m_pDDE[index].filename + uExtIdx);
		strcpy(fileExt, (char*)(oFat.m_pDDE[index].filename + uExtIdx));
	}
	//UART_Printf("ext = %s\n", fileExt);
}

void FAT_GetFileSize(s32 index, s32* nFileSize, SDHC *sCh)
{
	Assert(index < DDEMAXSIZE);
	*nFileSize = oFat.m_pDDE[index].nFileSz;
}

u32 FAT_ReadFile(s32 nIdx,  u8* p_uDestAddr, SDHC *sCh)
{
//	u8* p_uDestAddr= (u8*)uDestAddr;

	int start_cluster;
	int size_byte;
	int size_sector, offset;
	int index;
//	int i;
	int nLba;

	index = nIdx;
	if (index != -1) {
		start_cluster = oFat.m_pDDE[index].location;
		size_byte = oFat.m_pDDE[index].nFileSz;
		size_sector = size_byte/oFat.m_usBpbBytsPerSec;
		offset = size_byte%oFat.m_usBpbBytsPerSec;
		if(offset) {
			size_sector = size_sector+1;
		}
		UART_Printf("File name: %s\n", oFat.m_pDDE[index].filename);
		UART_Printf("cluster: %d, size(byte): %d, size(sector): %d, offset: %d\n", start_cluster, size_byte, size_sector, offset);

		FAT_FromClusterToLba(start_cluster, &nLba, sCh);
		UART_Printf("  size(sector): %d, lba from cluster: %d\n", size_sector, nLba);
		FAT_ReadSector(p_uDestAddr, nLba, size_sector, sCh);
		return size_byte;
//		return true;
	}
	return false;
}



u8 FAT_ReadFile4(s32 nDirIdx, u32 nBlockPos, u32 nBlocks, u32 uDestAddr, SDHC *sCh)
{
	int start_cluster = oFat.m_pDDE[nDirIdx].location;
	int size_byte = oFat.m_pDDE[nDirIdx].nFileSz;
	int size_sector = (size_byte+oFat.m_usBpbBytsPerSec-1)/oFat.m_usBpbBytsPerSec;
	int nBlockToRead;
	int nLba;
	int nStBlock;

	Assert(nDirIdx != -1);

	if (nBlockPos > size_sector)
		return false;

	if ((nBlockPos+nBlocks) > size_sector)
		nBlockToRead = size_sector;
	else
		nBlockToRead = nBlocks;

	FAT_FromClusterToLba(start_cluster, &nLba, sCh);
	nStBlock = nLba+nBlockPos;

//	UART_Printf("  sectors=%d, stBlock=%d\n", nBlocks, nStBlock);
	FAT_ReadSector((u8*)uDestAddr, nStBlock, nBlocks, sCh);

	if (nBlockToRead == nBlocks)
	//	return nBlocks*oFat.m_usBpbBytsPerSec;
		return true;
	else
	//	return (size_byte - nBlockPos*oFat.m_usBpbBytsPerSec);
		return false;
}

u8 FAT_ReadFile6(s32 nDirIdx, u32 uFileOffset, u32 uBytes, u32 uDestAddr, u32* uReturnAddr, SDHC *sCh)//kst060404
{
	int start_cluster = oFat.m_pDDE[nDirIdx].location;
	int size_byte = oFat.m_pDDE[nDirIdx].nFileSz;
//	int size_sector = (size_byte+oFat.m_usBpbBytsPerSec-1)/oFat.m_usBpbBytsPerSec;
	int nLba;
	int nStBlock;
	int nEndBlock;
	int nBlocks;
	int nBytesToRead;

	Assert(nDirIdx != -1);

	if (size_byte < uFileOffset)
		return false;
	if (size_byte < uBytes)
		return false;

	if ((uFileOffset+uBytes) > size_byte)
		nBytesToRead = size_byte;
	else
		nBytesToRead = uBytes;

	FAT_FromClusterToLba(start_cluster, &nLba, sCh);
	nStBlock = nLba + uFileOffset/oFat.m_usBpbBytsPerSec;
	nEndBlock = nLba + (uFileOffset+uBytes+oFat.m_usBpbBytsPerSec-1)/oFat.m_usBpbBytsPerSec;
	nBlocks = nEndBlock-nStBlock+1;

	*uReturnAddr = uDestAddr + uFileOffset%oFat.m_usBpbBytsPerSec;

	FAT_ReadSector((u8*)uDestAddr, nStBlock, nBlocks, sCh);

	if (nBytesToRead == uBytes)
	//	return uBytes;
		return true;

	else
	//	return (size_byte - uBytes);
	return false;

}


u8 FAT_WriteFile(const char *pFileName, s32 nLen, u32 uSrcAddr, SDHC* sCh)
{
	int nSFile;
	FAT_CreateSFile(FIRST_DATA_CLUSTER, pFileName, ATTR_ARCHIVE, nLen, (u8*)uSrcAddr, &nSFile, sCh);
	if(nSFile>=0)
	{
		FAT_SetFSInfo(sCh);
		FAT_CopyFatTableToBackup(sCh);
		return 1;
	}
	else
	{
		return 0;
	}
}


void FAT_ShortFileNameArray(DISC_DIR_ENTRY* da,  char* pShortFileName, SDHC *sCh)
{
	int j, k;
	char c;
	u8 dotTemp = false;

	for(j=0, k=0; j<14; j++)
	{
		c = pShortFileName[j];
		if(c != ' ')
		{
			if(dotTemp)
			{
				da->filename[k++] = '.';
				da->filename[k++] = pShortFileName[j];
				dotTemp = false;
			}
			else	// sunny modify
			{
				if(j==8)
				{
					da->filename[k++] = '.';
					da->filename[k++] = pShortFileName[j];
					dotTemp = false;
				}
				else
					da->filename[k++] = pShortFileName[j];
			}
		}
		else
		{ //space
			if(j < SF_BASE_LENGTH)
			{  // within 8 character
				if((da->dirflag & ATTR_ARCHIVE) == ATTR_ARCHIVE)
					dotTemp = true;  // to insert dot
			}
		}
	}
}

void FAT_DisplayFileNames(SDHC *sCh)
{
	int i;//, j;
	for (i=0/*, j=0*/; i<oFat.m_nFileIndex; i++)
	{
		if ((oFat.m_pDDE[i].dirflag & ATTR_ARCHIVE) == ATTR_ARCHIVE)		// file
		{
				UART_Printf("%02d -- %12s\n", i, oFat.m_pDDE[i].filename);
		}
		else if((oFat.m_pDDE[i].dirflag & ATTR_DIRECTORY) == ATTR_DIRECTORY) // normal directory
		{
			if ((oFat.m_pDDE[i].dirflag & ATTR_LONG_NAME) == ATTR_LONG_NAME) // long directory name
				UART_Printf("%s \n", oFat.m_pDDE[i].filename);
			else
				UART_Printf("%s\n", oFat.m_pDDE[i].filename);
		} else if(((oFat.m_pDDE[i].dirflag & ATTR_VOLUME_ID) == ATTR_VOLUME_ID)&&
		          ((oFat.m_pDDE[i].dirflag & ATTR_LONG_NAME) != ATTR_LONG_NAME) ) {   //root directory
			UART_Printf("ROOT\n");
		}
	}
}


void FAT_InitializeSecBuff(SDHC *sCh)
{
	int i, j;
	for(i=0; i<SECMAXSIZE; i++)
	{
		for(j=0; j<SECBUFFSIZE*MAXSEC_PER_CLUSTER; j++)
		{
			oFat.m_pSecBuff[i][j] = 0;
		}
	}
}
void FAT_InitalizeDirCluster(SDHC *sCh)
{
	int i;

	oFat.m_uDirClusterNum = 0;
	for (i=0; i<DIRCLUSTER; i++)
		oFat.m_nDirCluster[i] = 0;
}
void FAT_InitializeBeforeIndex(SDHC *sCh)
{
	int i;
	for(i=0;i<8;i++) // max depth ==8
	{
		oFat.m_nBeforeIndex[i] = 0;
	}
}

void FAT_InitializeDiscEntry(SDHC *sCh)
{
	int i, j;
	for(i=0; i<DDEMAXSIZE; i++)
	{
		for(j=0; j<255; j++) // sunny modify j<24--> j<255
		{
			oFat.m_pDDE[i].filename[j] = 0;
		}
		oFat.m_pDDE[i].dirflag = 0;
		oFat.m_pDDE[i].location = -1;
		oFat.m_pDDE[i].nFileSz = -1;
	}
	oFat.m_nFileIndex = 0;  // LYS 040113
}


void FAT_GetTotalNumOfFiles(u32* uNumOfFiles, SDHC *sCh)
{
	u32 i;

	for(i=0; i<DDEMAXSIZE; i++)
	{
		if (oFat.m_pDDE[i].nFileSz == -1)
			break;
	}
	*uNumOfFiles = i; 
}


// LYS 040107 add
// idxCluster: current cluster to looking at
// nDepthNow: root: 0, sub: nDepthNow++; parent: nDepthNow--;  MAX: 8
// recursive function

int FAT_FindClusterInDirectory(int* idxCluster, int* nLongDirStatus, SDHC *sCh)
{
	int j;
	u8 c;
	int clusterTemp;

	u8 attrTemp;
	int index;
//	int parentDirCluster;
	u16  nTotalOrder;
	//index = FindSecBuffIndex(idxCluster);  //idxCluster: 2 -> root directory

	u8* pLastEntryAddr;
//	u16 usDIR_FstClusHI, usDIR_FstClusLO;
	u32 uDIR_FileSize, uDIR_FstClus;
	char cShortFileName[SF_NAME_LENGTH];

	u8 ucBufferSec[SECBUFFSIZE*MAXSEC_PER_CLUSTER];	
//	u8 ucLdirChksum;
	int i, k, nPos, nOrder;
	u8 *pDirStrAddr;
	u8 ucDirChkSum;
	u32 nEntCluster;
	int nLba;
	int nBSecBuff;

	index = 0;		// sunny modify
	j=0;

	while(j<oFat.m_uDirEntryPerCulster) // sunny modify: j<32
	{
		c = *(oFat.m_pSecBuff[index]+DIR_ENTRY_SIZE*j + 0);  //DIR_Name[0]
		if(c == 0xe5) // deleted file or dir
		{
			j++;
		}
		else if(c == 0x00)	// no more entries in this cluster.
		{
			return 0;
		}
		else if(c == '.') 	// current or parent directory
		{
			if(*(oFat.m_pSecBuff[index]+DIR_ENTRY_SIZE*j + 1) == '.') // ..
			{
// parentDirCluster - naver referenced.
//				parentDirCluster = clusterTemp;
			}
			j++;
		}
		else
		{  // normal dir or file
			attrTemp = *(oFat.m_pSecBuff[index]+DIR_ENTRY_SIZE*j + 11); // DIR_ATTR
			oFat.m_pDDE[oFat.m_nFileIndex].dirflag = attrTemp;
			// sunny add
			if (attrTemp == 0xF)	// Long Directory Entry Structure
			{
				attrTemp = *(oFat.m_pSecBuff[index]+DIR_ENTRY_SIZE*j);  // oFat.m_ucLDirOrd
				nTotalOrder = attrTemp&(~0x40);

				if (nTotalOrder==0||nTotalOrder==0xe5)
				{
					Assert(0);
				}
				else
				{
					for(k=nTotalOrder; k>0; k--)
					{
						// To judge whether the dir go cross the boundary of the dir cluster
						if ((j + nTotalOrder - k)<oFat.m_uDirEntryPerCulster)
						{
							pDirStrAddr = (oFat.m_pSecBuff[index] + DIR_ENTRY_SIZE*(j + nTotalOrder - k));
							nOrder = (*pDirStrAddr)&(~0x40);
							nPos = 13*(nOrder-1);
							// ucLdirChksum - naver referenced.
							//ucLdirChksum = *(pDirStrAddr + 13);
							//if (ucDirChkSum != ucLdirChksum)
							//	Assert(0);
							for (i = 0; i < 5; i++)  /* 1-5 */
								oFat.m_pDDE[oFat.m_nFileIndex].filename[nPos+i] = UNICODEtOCHAR(pDirStrAddr+1+i*2);
							for (i = 0; i < 6; i++)  /* 6-11 */
								oFat.m_pDDE[oFat.m_nFileIndex].filename[nPos+5+i] = UNICODEtOCHAR(pDirStrAddr+14+i*2);
							for (i = 0; i < 2; i++)  /* 12-13 */
								oFat.m_pDDE[oFat.m_nFileIndex].filename[nPos+11+i] = UNICODEtOCHAR(pDirStrAddr+28+i*2);
						}
						else
						{
							*nLongDirStatus = 1;
							pLastEntryAddr = (oFat.m_pSecBuff[index] + DIR_ENTRY_SIZE*(j-1));
							//break;
							FAT_GetEntryCluster(pLastEntryAddr, &uDIR_FstClus, sCh);
							uDIR_FileSize = LSB_GET_4BYTES(pLastEntryAddr + 28);
							// Get the next cluster which store the remainder file info.
							*idxCluster = uDIR_FstClus + (uDIR_FileSize+oFat.m_nBytesPerCluster-1)/oFat.m_nBytesPerCluster;
							return 1;
						}
					}


					// Get the corresponding short Directory Entry Structure.
					// To judge whether the dir go cross the boundary of the dir cluster
					if((j + nTotalOrder)<oFat.m_uDirEntryPerCulster)
					{
						pDirStrAddr = (oFat.m_pSecBuff[index] + DIR_ENTRY_SIZE*(j + nTotalOrder));
						attrTemp = *(oFat.m_pSecBuff[index]+DIR_ENTRY_SIZE*(j + nTotalOrder) + 11); //DIR_ATTR
						oFat.m_pDDE[oFat.m_nFileIndex].dirflag = oFat.m_pDDE[oFat.m_nFileIndex].dirflag | attrTemp;
						nPos = 0;
						for (i=0; i<11; i++)
							cShortFileName[nPos+i] = *(pDirStrAddr+i);

						FAT_ComputeChkSum((u8*)cShortFileName, &ucDirChkSum, sCh);

						FAT_GetEntryCluster(oFat.m_pSecBuff[index]+DIR_ENTRY_SIZE*(j + nTotalOrder), &nEntCluster, sCh);
						clusterTemp = nEntCluster;

						oFat.m_pDDE[oFat.m_nFileIndex].nFileSz = LSB_GET_4BYTES(oFat.m_pSecBuff[index]+DIR_ENTRY_SIZE*(j+nTotalOrder)+28);
						oFat.m_pDDE[oFat.m_nFileIndex].location = clusterTemp;
					}
					else
					{
						*nLongDirStatus =2;
						pLastEntryAddr = (oFat.m_pSecBuff[index] + DIR_ENTRY_SIZE*(j-1));
						break;
					}

					j = j + nTotalOrder + 1;
					if (j==oFat.m_uDirEntryPerCulster)
					pLastEntryAddr = (oFat.m_pSecBuff[index] + DIR_ENTRY_SIZE*(j-1));

				}
				oFat.m_nFileIndex++;
			}
			else // Short Directory Entry Structure
			{
				FAT_GetEntryCluster(oFat.m_pSecBuff[index]+DIR_ENTRY_SIZE*j, &nEntCluster, sCh);
				clusterTemp = nEntCluster;
				oFat.m_pDDE[oFat.m_nFileIndex].nFileSz = LSB_GET_4BYTES(oFat.m_pSecBuff[0]+DIR_ENTRY_SIZE*j+28);
				oFat.m_pDDE[oFat.m_nFileIndex].location = clusterTemp;
				// The short dir is the corresponding short dir of the last long file
				if ((j==0)&&(*nLongDirStatus==2))
				{
					attrTemp = *(oFat.m_pSecBuff[index] + 11); // DIR_ATTR
					oFat.m_pDDE[oFat.m_nFileIndex].dirflag = oFat.m_pDDE[oFat.m_nFileIndex].dirflag | attrTemp;
					*nLongDirStatus = 0;
				}
				else
				{
					FAT_CopyArray((u8*)cShortFileName, oFat.m_pSecBuff[index]+DIR_ENTRY_SIZE*j, SF_NAME_LENGTH, sCh);
					FAT_ShortFileNameArray(&oFat.m_pDDE[oFat.m_nFileIndex], cShortFileName, sCh);
					oFat.m_pDDE[oFat.m_nFileIndex].filename[SF_NAME_LENGTH+1] = '\0';
				}
				j++;
				oFat.m_nFileIndex++;

				if (j==oFat.m_uDirEntryPerCulster)	//the last entry is short directory entry
					pLastEntryAddr = (oFat.m_pSecBuff[index] + DIR_ENTRY_SIZE*(j -1));
			}

			if((attrTemp & ATTR_DIRECTORY) == ATTR_DIRECTORY) {
				FAT_PushIndex(j, sCh);
				UART_Printf("dir entry cluster: %d\n", clusterTemp); //LYS 040115
				FAT_FromClusterToLba(clusterTemp, &nLba, sCh);			
				FAT_ReadSector(ucBufferSec, nLba, oFat.m_ucBpbSecPerClus, sCh);
				FAT_FindBlankSecBuff(&nBSecBuff, sCh);
				FAT_CopyArray(oFat.m_pSecBuff[nBSecBuff], ucBufferSec, oFat.m_nBytesPerCluster, sCh);
				FAT_FindClusterInDirectory(&clusterTemp, nLongDirStatus, sCh);  // recursive call in sub directory .. khlee
				FAT_PopIndex(&j, sCh);
			} //  insert here (LAST_cluster
		}
	}

	FAT_GetEntryCluster(pLastEntryAddr, &uDIR_FstClus, sCh);
	uDIR_FileSize = LSB_GET_4BYTES(pLastEntryAddr + 28);
	// Get the next cluster which store the remainder file info.
	*idxCluster = uDIR_FstClus + (uDIR_FileSize+oFat.m_nBytesPerCluster-1)/oFat.m_nBytesPerCluster;
	return 1;
}


void FAT_FindBlankSecBuff(int* nBSecBuff, SDHC *sCh)
{
	int i;
	u8 c;

	for(i=0; i<SECMAXSIZE; i++)
	{
		c = oFat.m_pSecBuff[i][0];
		if(c == 0x00)
		{
			*nBSecBuff = i;
			return; /// 7.20 need
		}
	}
	*nBSecBuff = SECMAXSIZE-1;
}

void FAT_FindSecBuffIndex(int idxCluster, int* nSecBuffIndex, SDHC *sCh)
{
	int i, cl;

	if(FATType == IS_FAT32)
	{
		if(idxCluster == 2)
		{ // root directory
			*nSecBuffIndex = 0;
		}
	}
	else if(FATType == IS_FAT16)
	{
		if(idxCluster == 0)
		{ // root directory
			*nSecBuffIndex = 0;
		}
	}

	for(i=0; i<SECMAXSIZE; i++)
	{
		u32 nEntCluster;
		FAT_GetEntryCluster(oFat.m_pSecBuff[i], &nEntCluster, sCh);
		cl = nEntCluster;  // current directory의 cluster number..  // root: 0--> 2
		UART_Printf("%d ", cl);
		if(cl == idxCluster)
		{
			UART_Printf("cl: %d, i: %d\n", cl, i);
			*nSecBuffIndex = i;
			return; /// 7.20 need
		}
	}
	*nSecBuffIndex = -1;   // not found
}


void FAT_PushIndex(int index, SDHC *sCh)
{
	if(oFat.m_nDepthNow < MAXDIRDEPTH)
	{
		oFat.m_nBeforeIndex[oFat.m_nDepthNow++] = index;
	}
	else
	{
		UART_Printf("PushIndex() overflow\n");
	}
}

void FAT_PopIndex(int* nIndex, SDHC *sCh)
{
	if(oFat.m_nDepthNow > 0) {
		*nIndex = oFat.m_nBeforeIndex[--oFat.m_nDepthNow];
	} else {
		UART_Printf("PopIndex() underflow\n");
		*nIndex = -1;
	}
}
void FAT_FindFreeEntry(u8 ucEntries, int* nCluster, int* nOffset, SDHC *sCh)
{
    int  nDirEntryCluster, nSector;
	u8   ucBufferSec[SECBUFFSIZE*MAXSEC_PER_CLUSTER];
	u8 	 c, j;
	int  nFreeEntryNum=0;
	int i;

	for (i=0; i<=oFat.m_uDirClusterNum; i++)
	{
		nDirEntryCluster = oFat.m_nDirCluster[i];
		FAT_FromClusterToLba(nDirEntryCluster, &nSector, sCh);

		FAT_ReadSector(&(ucBufferSec[0]), nSector, oFat.m_ucBpbSecPerClus, sCh);
		FAT_CopyArray(oFat.m_pSecBuff[0], &(ucBufferSec[0]), oFat.m_nBytesPerCluster, sCh);
		j=0;
		nFreeEntryNum = 0;
		while(j<oFat.m_uDirEntryPerCulster) 
		{
			c = *(oFat.m_pSecBuff[0]+DIR_ENTRY_SIZE*j + 0);  //DIR_Name[0]
			if (c == 0xe5) 		// deleted file or dir, it's free entry
			{
				nFreeEntryNum++;
				if (nFreeEntryNum==ucEntries)
				{
					*nCluster = nDirEntryCluster;
					*nOffset = j+1-nFreeEntryNum;
					break;
				}
			}
			else if (c == 0x00)	// the dir is free and there are no allocated directory entries after this ont
			{
				nFreeEntryNum ++;
				*nCluster = nDirEntryCluster;
				*nOffset = j+1-nFreeEntryNum;
				break;
			}
			else 				// current or parent directory or file entry
			{
				nFreeEntryNum = 0;
			}
			j++;
		}
	}
}
void FAT_FindFreeEntryOffsetInDirEntry(int idx, int* nFreeEntryOffset, SDHC *sCh)
{
	u8 c;
	int i;
	UART_Printf("Find Offset in DIR entry at index %d: ", idx);
	//for(i=0; i<(CLUSTERSIZE/DIR_ENTRY_SIZE); i++) {
	for(i=0; i<(oFat.m_nBytesPerCluster/DIR_ENTRY_SIZE); i++) {
		// c = *(oFat.m_pSecBuff[idx]+DIR_ENTRY_SIZE*i + 0);
		c = oFat.m_pSecBuff[idx][DIR_ENTRY_SIZE*i];  //DIR_Name[0]
		UART_Printf("0x%x ", c);
		if (c == 0x00) {  // free entry
			*nFreeEntryOffset = DIR_ENTRY_SIZE*i;
			return; /// 7.20 need
		}
	}
	*nFreeEntryOffset = -1; // not found
}

//return value: dde index
void FAT_FindMaxLocation(int endIndex, int* nDdeMaxindex, SDHC *sCh)
{
	int i, max=0, index=-1;
	for(i=0; i<endIndex; i++) {
		if(max<oFat.m_pDDE[i].location) {
			max = oFat.m_pDDE[i].location;
			index = i;
		}
	}
	*nDdeMaxindex =  index;
}

void FAT_ChangeDir(int hcluster, int ucMode, int* nChgDir, SDHC *sCh)
{
	int nHsector;
	//uchar *p = oFat.m_ucDirBuffStart;
	u8* p;

	FAT_InitBuffDirMem(&p, sCh);  // LYS 040407
	if (hcluster==0)
		hcluster = oFat.m_uBpbRootClus;
	if (hcluster<2||hcluster>=oFat.m_nBpbCountOfClusters+2)
		*nChgDir = -1;
	if (nHsector != oFat.m_nPosRoot) {
		p[0] = p[DIR_ENTRY_SIZE] = 0;
		FAT_FromClusterToLba(hcluster, &nHsector, sCh);
		FAT_ReadSector(p, nHsector, 1, sCh);
		if (p[0]!=0x2e || p[DIR_ENTRY_SIZE]!=0x2e)
			*nChgDir = -2;
	}
	if (ucMode==0) {
		oFat.m_nPwd = hcluster;
	}
	else if (ucMode==1) {
		oFat.m_nTwd = hcluster;
	}
	*nChgDir = 1;
}

void FAT_FindClusterFromName(char* s, u32* uClusterName, SDHC *sCh)
{
	int i, j, k;
	char* c;
	u8 sameName=false;
	//To use this function, oFat.m_pSecBuff[][] must always up-to-date..
	//After searching the exact name, extract the cluster number in the DIR entry.
	FAT_ConvertToDiscEntryFormat(s, c, sCh);
	for(i=0; i<SECMAXSIZE; i++) {
		//for(j=0; j<CLUSTERSIZE/DIR_ENTRY_SIZE; j++) {
		for(j=0; j<oFat.m_nBytesPerCluster/DIR_ENTRY_SIZE; j++) {
			for(k=0; k<SF_NAME_LENGTH; k++) {
				if(c[k] != *(oFat.m_pSecBuff[i] + DIR_ENTRY_SIZE*j + k)) {
					sameName = false;
					break;
				} else {
					if(k==SF_NAME_LENGTH-1) {
						sameName = true;
					}
				}
			}
			if(sameName == true) {
				u32 nEntCluster;
				FAT_GetEntryCluster(oFat.m_pSecBuff[i] + DIR_ENTRY_SIZE*j, &nEntCluster, sCh);
				*uClusterName = nEntCluster;
			}
		}
	}
	*uClusterName = 0;  //not found

}

void FAT_FindDirEntryClusterFromName(char* s, u32* uEntCluster, SDHC *sCh)
{
	int i, j, k;
	char* c;
	u8 sameName=false;
	//To use this function, oFat.m_pSecBuff[][] must always up-to-date..
	//After searching the exact name, extract the cluster number in the DIR entry.
	FAT_ConvertToDiscEntryFormat(s, c, sCh);
	for(i=0; i<SECMAXSIZE; i++) {
		//for(j=0; j<CLUSTERSIZE/DIR_ENTRY_SIZE; j++) {
		for(j=0; j<oFat.m_nBytesPerCluster/DIR_ENTRY_SIZE; j++) {
			for(k=0; k<SF_NAME_LENGTH; k++) {
				if(c[k] != *(oFat.m_pSecBuff[i] + DIR_ENTRY_SIZE*j + k)) {
					sameName = false;
					break;
				}
				else {
					if(k==SF_NAME_LENGTH-1) {
						sameName = true;
					}
				}
			}
			if(sameName == true) {
				u32 nEntCluster;
				FAT_GetEntryCluster(oFat.m_pSecBuff[i], &nEntCluster, sCh);
				*uEntCluster = nEntCluster;
			}
		}
	}
	*uEntCluster = 0;  //not found
}

u8 FAT_FindDirEntryOffsetFromName(const char* s, int* nEntOffset, SDHC *sCh)
{
	int  i, j;
	u8 sameName=false;
    int  nWritingFileLen = strlen(s);
	int  nExistingFileLen;

	for (i=0; i<oFat.m_nFileIndex; i++)
	{
		nExistingFileLen = strlen(oFat.m_pDDE[i].filename);
		if (nWritingFileLen==nExistingFileLen)
		{
			for(j=0; j<nExistingFileLen; j++)
			{	
				// character insensitive, so FOOBAR, foobar and FooBar are looked as same.
				if((s[j] != oFat.m_pDDE[i].filename[j] )&&(s[j] != oFat.m_pDDE[i].filename[j]-32 )&&(s[j] != oFat.m_pDDE[i].filename[j] +32)) 
				{
					sameName = false;
					break;
				} 
				else
					sameName = true;
			}
			if(sameName == true) 
			{
				*nEntOffset = i;
				return true;
			}
		}
	}
	*nEntOffset = -1;  //not found
	return false;
}

void FAT_InitializeEntry(SDHC *sCh)
{
	///InitializeSecBuff();
	FAT_InitalizeDirCluster(sCh);
	FAT_InitializeBeforeIndex(sCh);
	FAT_InitializeDiscEntry(sCh);
}

void FAT_ReadFileSystemInfo(SDHC *sCh)
{
	FAT_InitializeEntry(sCh);
	if(FATType == IS_FAT32)
	{
		FAT_ReadDirFat32(FIRST_DATA_CLUSTER, sCh);
	} else if(FATType == IS_FAT16)
	{  //LYS 040901
		FAT_ReadDirFat32(0, sCh);
	}
	UART_Printf("entryNum = %d, in ReadFileSystemInfo()\n", oFat.m_nFileIndex); // LYS 040423
	FAT_DisplayFileNames(sCh);
}

// To make 512B oFat.m_ucBuffer initialized to 0.
// This is used in HD_file_create(), HD_file_delete(), etc.

void FAT_InitBuffFileMem(u8** ucpInitBufferFile, SDHC *sCh)
{
	int i;
	for(i=0; i<SECBUFFSIZE; i++) {
		oFat.m_pBufferFileMem[i] = 0;
	}
	// youngbo.song modify - level 2 pointer.
	*ucpInitBufferFile = oFat.m_pBufferFileMem;
}

void FAT_InitBuffDirMem(u8** ucpInitBufferDir, SDHC *sCh)
{
	int i;
	for(i=0; i<SECBUFFSIZE; i++) {
		oFat.m_pBufferDirMem[i] = 0;
	}
	// youngbo.song modify - level 2 pointer.
	*ucpInitBufferDir = oFat.m_pBufferDirMem;
}


void FAT_InitBuffSearchMem(u8** ucpInitBufferSearch, SDHC *sCh)
{
	int i;
	for(i=0; i<SECBUFFSIZE; i++) {
		oFat.m_pBufferSearchMem[i] = 0;
	}
	// youngbo.song modify - level 2 pointer.
	*ucpInitBufferSearch = oFat.m_pBufferSearchMem;
}


void FAT_CopyFatTableToBackup(SDHC *sCh)
{
	int i;
	u8* p = oFat.m_pFatBuffer;
	UART_Printf("oFat.m_pFatBuffer=0x%x\n", (u32)p);

	UART_Printf("oFat.m_nLastFatSector: %d in copy()\n", oFat.m_nLastFatSector);
	for(i=0; i<=oFat.m_nLastFatSector; i++)
	{
		FAT_ReadSector(p, i+oFat.m_nPosFat+oFat.m_uBpbHiddSec+SDCARD_OFFSET_SECTOR, 1, sCh);		
		FAT_WriteSector(p, i+oFat.m_nPosFatb+oFat.m_uBpbHiddSec+SDCARD_OFFSET_SECTOR, 1, sCh);
	}
}

/**********************************************************/
/*  date format convention for fat32,
    the result store in oFat.m_nFat32FileTime and oFat.m_nFat32FileDate
    date format: 12/06/2000 -> 0x20001206
        09/53/20 ->     0x095320			  */
/**********************************************************/
void FAT_SetDateTime(int date, int time, SDHC *sCh)
{
	oFat.m_nFat32FileTime = time&0xffff;
	oFat.m_nFat32FileDate = date&0xffff;
}

// LYS 040531
void FAT_Lower2Capital(char* s, SDHC *sCh)
{
	int i;
	int nLength;

	for(i=0; i<SF_NAME_LENGTH+2; i++) {
		if(*(s+i) == 0) {
			nLength = i;
			break;
		}
	}

	for(i=0; i<nLength; i++) {
		if(*(s+i) >= 0x61 && *(s+i) <= 0x7A) {  // lower case
			*(s+i) -= 0x20;
		}
	}
}

u8 FAT_LoadFileSystem(SDHC *sCh) 
{
	int i;

 	oFat.m_pDDE = (DISC_DIR_ENTRY*)malloc(sizeof(DISC_DIR_ENTRY[DDEMAXSIZE]));
	for(i=0; i < SECMAXSIZE ; i++)
		oFat.m_pSecBuff[i] = (u8*)malloc(sizeof(u8[SECBUFFSIZE*MAXSEC_PER_CLUSTER]));

	oFat.m_pFatBuffStart = (u8*)malloc(sizeof(u8[SECBUFFSIZE]));
	oFat.m_pFatBuffer = (u8*)malloc(sizeof(u8[SECBUFFSIZE]));
	oFat.m_pFsInfo = (u8*)malloc(sizeof(u8[SECBUFFSIZE]));
	oFat.m_pBufferFileMem = (u8*)malloc(sizeof(u8[SECBUFFSIZE]));
	oFat.m_pBufferDirMem = (u8*)malloc(sizeof(u8[SECBUFFSIZE]));
	oFat.m_pBufferSearchMem = (u8*)malloc(sizeof(u8[SECBUFFSIZE]));

	if (!FAT_InitHardDisk(sCh))
		return false;

	FAT_ReadFileSystemInfo(sCh);
	return true;
}

void FAT_UnloadFileSystem(SDHC *sCh) 
{
	int i;

	free(oFat.m_pDDE);
	for(i=0; i<SECMAXSIZE ; i++)
		free(oFat.m_pSecBuff[i]);

	free(oFat.m_pFatBuffStart);
	free(oFat.m_pFatBuffer);
	free(oFat.m_pFsInfo);
	free(oFat.m_pBufferFileMem);
	free(oFat.m_pBufferDirMem);
	free(oFat.m_pBufferSearchMem);
}

