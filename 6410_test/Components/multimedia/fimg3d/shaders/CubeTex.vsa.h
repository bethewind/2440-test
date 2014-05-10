#if !defined(__CUBETEX_VSA_H__)
#define __CUBETEX_VSA_H__

static const unsigned CubeTex_vsa[] = {
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
  0x00000004,
  // OUTPUT ATTRIBUTE SIZE
  0x00000004,
  // SAMPLER SIZE
  0x00000000,
  // INSTRUCTION SIZE
  0x00000080,
  // CONSTANT FLOAT SIZE
  0x00000028,
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
  0x00000040,
  0x00000080,
  // ---------------------------------------------
  // Output Attribute Table
  // ---------------------------------------------
  0x00000010,
  0x00000040,
  0x00000080,
  0x00000081,
  // ---------------------------------------------
  // Instruction Area
  // - 0x0, 0x4, 0x8, 0xc (Data at 0xC will not be written.)
  // ---------------------------------------------
  0x00000000, 0x0000e402, 0x048800e4, 0x00000000,
  0x08000000, 0x0208e402, 0x04c020e4, 0x00000000,
  0x01000000, 0x0000e402, 0x049000e4, 0x00000000,
  0x00000000, 0x01000000, 0x08c020ff, 0x00000000,
  0x02000000, 0x0000e402, 0x04a000e4, 0x00000000,
  0x08000000, 0x0100e402, 0x037821ff, 0x00000000,
  0x07000000, 0x0101e402, 0x027820e4, 0x00000000,
  0x04000000, 0x0001e402, 0x048823e4, 0x00000000,
  0x05000000, 0x0001e402, 0x049023e4, 0x00000000,
  0x06000000, 0x0001e402, 0x04a023e4, 0x00000000,
  0x00000000, 0x0100e401, 0x04c022e4, 0x00000000,
  0x03000000, 0x0103a401, 0x04a022a4, 0x00000000,
  0x00000000, 0x01020000, 0x08c022ff, 0x00000000,
  0x00000000, 0x01020000, 0x08c023aa, 0x00000000,
  0x02000000, 0x0100ff01, 0x037822e4, 0x00000000,
  0x03000000, 0x0103ff01, 0x037820a4, 0x00000000,
  0x03000000, 0x0000e402, 0x04c000e4, 0x00000000,
  0x02000000, 0x0100e401, 0x04c023e4, 0x00000000,
  0x01000000, 0x0100e401, 0x04c022e4, 0x00000000,
  0x01000000, 0x0001e400, 0x04c020e4, 0x00000000,
  0x00000000, 0x01030000, 0x072022ff, 0x00000000,
  0x09000000, 0x01020002, 0x032022aa, 0x00000000,
  0x00000000, 0x01020000, 0x062022aa, 0x00000000,
  0x00000000, 0x01000000, 0x08c020ff, 0x00000000,
  0x01000000, 0x0100e400, 0x037820ff, 0x00000000,
  0x00000000, 0x4207e400, 0x227821e4, 0x00000000,
  0x02aa0102, 0x0102e400, 0x0ef822ff, 0x00000000,
  0x00000000, 0x0101e401, 0x04c023e4, 0x00000000,
  0x09000000, 0x01025502, 0x027801e4, 0x00000000,
  0x03000000, 0x0103ff01, 0x224022ff, 0x00000000,
  0x02e40101, 0x0100ff41, 0x0ef803e4, 0x00000000,
  0x00000000, 0x00030000, 0x00f802e4, 0x00000000,
  // ---------------------------------------------
  // Constant Float Registers (0 ~ 255)
  // - x , y , z , w
  // ---------------------------------------------
  0x00000000, 0x00000000, 0x00000000, 0x00000000,
  0x00000000, 0x00000000, 0x00000000, 0x00000000,
  0x00000000, 0x00000000, 0x00000000, 0x00000000,
  0x00000000, 0x00000000, 0x00000000, 0x00000000,
  0x00000000, 0x00000000, 0x00000000, 0x00000000,
  0x00000000, 0x00000000, 0x00000000, 0x00000000,
  0x00000000, 0x00000000, 0x00000000, 0x00000000,
  0x00000000, 0x00000000, 0x00000000, 0x00000000,
  0x00000000, 0x00000000, 0x00000000, 0x00000000,
  0x42000000, 0x3e4ccccd, 0x00000000, 0x00000000,
};

#endif /*__CUBETEX_VSA_H__*/