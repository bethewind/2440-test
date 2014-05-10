#if !defined(__LIGHTING_VSA_H__)
#define __LIGHTING_VSA_H__

static const unsigned Lighting_vsa[] = {
  // ---------------------------------------------
  // Shader Object Header
  // ---------------------------------------------
  // MAGIC (VS)
  0x20205356,
  // VERSION (3.0)
  0xFFFF0003,
  // HEADER SIZE (64 Bytes)
  0x00000040,
  // INPUT  ATTRIBUTE SIZE
  0x00000000,
  // OUTPUT ATTRIBUTE SIZE
  0x00000000,
  // SAMPLER SIZE
  0x00000000,
  // INSTRUCTION SIZE
  0x000000B0,
  // CONSTANT FLOAT SIZE
  0x00000038,
  // CONSTANT INTEGER SIZE
  0x00000000,
  // CONSTANT BOOLEAN SIZE
  0x00000000,
  // RESERVED
  0x00000000,
  0x00000000,
  0x00000000,
  0x00000000,
  0x00000000,
  0x00000000,
  // ---------------------------------------------
  // Pre-defined Attributes for input/output
  //   semantic decleration
  //
  // - FGSP_ATRBDEF_POSITION  0x10
  // - FGSP_ATRBDEF_NORMAL    0x20
  // - FGSP_ATRBDEF_PCOLOR    0x40
  // - FGSP_ATRBDEF_SCOLOR    0x41
  // - FGSP_ATRBDEF_TEXTURE0  0x80
  // - FGSP_ATRBDEF_TEXTURE1  0x81
  // - FGSP_ATRBDEF_TEXTURE2  0x82
  // - FGSP_ATRBDEF_TEXTURE3  0x83
  // - FGSP_ATRBDEF_TEXTURE4  0x84
  // - FGSP_ATRBDEF_TEXTURE5  0x85
  // - FGSP_ATRBDEF_TEXTURE6  0x86
  // - FGSP_ATRBDEF_TEXTURE7  0x87
  // - FGSP_ATRBDEF_POINTSIZE 0x01
  // - FGSP_ATRBDEF_USERDEF0  0x02
  // - FGSP_ATRBDEF_USERDEF1  0x03
  // - FGSP_ATRBDEF_USERDEF2  0x04
  // - FGSP_ATRBDEF_USERDEF3  0x05
  // ---------------------------------------------
  // ---------------------------------------------
  // Instruction Area
  // - 0x0, 0x4, 0x8, 0xc (Data at 0xC will not be written.)
  // ---------------------------------------------
  0x00000000, 0x0200e400, 0x048820e4, 0x00000000,
  0x00000000, 0x0201e400, 0x049020e4, 0x00000000,
  0x00000000, 0x0202e400, 0x04a020e4, 0x00000000,
  0x00000000, 0x0203e400, 0x04c020e4, 0x00000000,
  0x00000000, 0x01000000, 0x00f822e4, 0x00000000,
  0x00000000, 0x02090000, 0x00b820e4, 0x00000000,
  0x04000000, 0x0100a402, 0x037820a4, 0x00000000,
  0x00000000, 0x01000000, 0x00b821e4, 0x00000000,
  0x00000000, 0x02069040, 0x02702090, 0x00000000,
  0x00000000, 0x0100f901, 0x040820f9, 0x00000000,
  0x00000000, 0x01000000, 0x08882000, 0x00000000,
  0x00000000, 0x0100f901, 0x03382400, 0x00000000,
  0x04000000, 0x0001a401, 0x040820a4, 0x00000000,
  0x0d000000, 0x01000002, 0x0a402000, 0x00000000,
  0x00000000, 0x020a0000, 0x00b820e4, 0x00000000,
  0x05000000, 0x0100a402, 0x033820a4, 0x00000000,
  0x00000000, 0x0100ff01, 0x037823a4, 0x00000000,
  0x00000000, 0x01030000, 0x00b820e4, 0x00000000,
  0x00000000, 0x0207a440, 0x023823a4, 0x00000000,
  0x03000000, 0x0103a401, 0x044021a4, 0x00000000,
  0x00000000, 0x01010000, 0x28c021ff, 0x00000000,
  0x03a40104, 0x0101a401, 0x0eb823ff, 0x00000000,
  0x03000000, 0x0103a401, 0x044021a4, 0x00000000,
  0x00000000, 0x01010000, 0x08c021ff, 0x00000000,
  0x03000000, 0x0101a401, 0x033823ff, 0x00000000,
  0x03000000, 0x0001a401, 0x044021a4, 0x00000000,
  0x0d000000, 0x01010002, 0x0a1823ff, 0x00000000,
  0x00000000, 0x020c0000, 0x00e02300, 0x00000000,
  0x00000000, 0x0103e400, 0x27a023e4, 0x00000000,
  0x03e40000, 0x0103ff01, 0x03a023aa, 0x00000000,
  0x00000000, 0x0103e400, 0x06a023aa, 0x00000000,
  0x00000000, 0x01030000, 0x00c021aa, 0x00000000,
  0x0d000000, 0x41000042, 0x0c8861ff, 0x00000000,
  0x00000000, 0x06010000, 0x18800100, 0x00000000,
  0x00000000, 0x020d0000, 0x00c02100, 0x00000000,
  0x01000000, 0x0208a401, 0x023821a4, 0x00000000,
  0x00000000, 0x0101a401, 0x023821a4, 0x00000000,
  0x00000000, 0x020b0000, 0x00b820e4, 0x00000000,
  0x05000000, 0x0100a402, 0x233820a4, 0x00000000,
  0x01a40101, 0x0100ff01, 0x0eb825a4, 0x00000000,
  0x05000000, 0x0002a401, 0x023801a4, 0x00000000,
  0x00000000, 0x020d0000, 0x00c00155, 0x00000000,
  0x00000000, 0x01020000, 0x00f800e4, 0x00000000,
  0x00000000, 0x00000000, 0x1e000000, 0x00000000,
  // ---------------------------------------------
  // Constant Float Registers (0 ~ 255)
  // - x , y , z , w
  // ---------------------------------------------
  0x40624630, 0x00000000, 0x40624630, 0x00000000,
  0x40200000, 0x40624630, 0xc0200000, 0x00000000,
  0x3f1745d0, 0xbf55ee9a, 0xbf1745d0, 0xc08d1746,
  0x3f000000, 0xbf3504f7, 0xbf000000, 0x40b00000,
  0x3e4ccccd, 0x3e99999a, 0x3f19999a, 0x00000000,
  0x3dcccccd, 0x3dcccccd, 0x3dcccccd, 0x00000000,
  0x40a00000, 0x40a00000, 0x40a00000, 0x00000000,
  0x00000000, 0x00000000, 0x40400000, 0x00000000,
  0x00000000, 0x00000000, 0x00000000, 0x00000000,
  0x3f19999a, 0x3f19999a, 0x3f19999a, 0x00000000,
  0x3f19999a, 0x3f19999a, 0x3f19999a, 0x00000000,
  0x00000000, 0x00000000, 0x00000000, 0x00000000,
  0x00000000, 0x00000000, 0x00000000, 0x00000000,
  0x00000000, 0x3f800000, 0x00000000, 0x00000000,
};

#endif /*__LIGHTING_VSA_H__*/