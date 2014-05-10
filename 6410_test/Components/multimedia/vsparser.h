#include "def.h"
#include "system.h"
#include "mfc.h"
#include "library.h"
u8 Vsparser(MFC_CODEC_MODE eDecType, u32  uBufferStartAddr, u32 uBufferByteSize,  u8 bIsHeader, u32 * uFrameStartAddr, 
u32 *uFrameSize, u32* uNumOfFrames, u8* bIsMpeg4ShortHeader); //outputs


