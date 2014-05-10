#if !defined(__TEXCUBEMAP_VSA_H__)
#define __TEXCUBEMAP_VSA_H__

static const unsigned TexCubemap_vsa[] = {
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
  0x00000002,
  // OUTPUT ATTRIBUTE SIZE
  0x00000002,
  // SAMPLER SIZE
  0x00000000,
  // INSTRUCTION SIZE
  0x00000034,
  // CONSTANT FLOAT SIZE
  0x00000000,
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
  // Input Attribute Table
  // ---------------------------------------------
  0x00000010,
  0x00000020,
  // ---------------------------------------------
  // Output Attribute Table
  // ---------------------------------------------
  0x00000010,
  0x00000080,
  // ---------------------------------------------
  // Instruction Area
  // - 0x0, 0x4, 0x8, 0xc (Data at 0xC will not be written.)
  // ---------------------------------------------
  0x00000000, 0x0000e402, 0x048800e4, 0x00000000,
  0x04000000, 0x0000e402, 0x048821e4, 0x00000000,
  0x05000000, 0x0000e402, 0x049021e4, 0x00000000,
  0x06000000, 0x0000e402, 0x04a021e4, 0x00000000,
  0x08000000, 0x0001e402, 0x040820e4, 0x00000000,
  0x09000000, 0x0001e402, 0x041020e4, 0x00000000,
  0x0a000000, 0x0001e402, 0x042020e4, 0x00000000,
  0x01000000, 0x0000e402, 0x049000e4, 0x00000000,
  0x01000000, 0x0100e401, 0x044020e4, 0x00000000,
  0x02000000, 0x0000e402, 0x04a000e4, 0x00000000,
  0x00000000, 0x0100ff01, 0x024020ff, 0x00000000,
  0x03000000, 0x0000e402, 0x24c000e4, 0x00000000,
  0x00e44101, 0x0100e401, 0x0eb801ff, 0x00000000,
};

#endif /*__TEXCUBEMAP_VSA_H__*/
