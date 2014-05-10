#if !defined(__FOG_PSA_H__)
#define __FOG_PSA_H__

static const unsigned Fog_psa[] = {
  // ---------------------------------------------
  // Shader Object Header
  // ---------------------------------------------
  // MAGIC (PS)
  0x20205350,
  // VERSION (3.0)
  0xFFFF0003,
  // HEADER SIZE (64 Bytes)
  0x00000040,
  // INPUT  ATTRIBUTE SIZE
  0x00000003,
  // OUTPUT ATTRIBUTE SIZE
  0x00000000,
  // SAMPLER SIZE
  0x00000001,
  // INSTRUCTION SIZE
  0x00000010,
  // CONSTANT FLOAT SIZE
  0x00000004,
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
  0x00000040,
  0x00000080,
  0x00000081,
  // ---------------------------------------------
  // Pre-defined Types for sampler
  //
  // - FGSP_SAMPLER_2D     0x01
  // - FGSP_SAMPLER_CUBE   0x02
  // - FGSP_SAMPLER_VOLUMN 0x04
  // ---------------------------------------------
  // ---------------------------------------------
  // Sampler Type Table
  // ---------------------------------------------
  0x00000001,
  // ---------------------------------------------
  // Instruction Area
  // - 0x0, 0x4, 0x8, 0xc (Data at 0xC will not be written.)
  // ---------------------------------------------
  0x00000000, 0x0001e407, 0x303820e4, 0x00000000,
  0x00e44200, 0x0000e401, 0x2eb820e4, 0x00000000,
  0x00e40200, 0x0002e401, 0x0eb81000, 0x00000000,
  0x00000000, 0x00000000, 0x00c010e4, 0x00000000,
  // ---------------------------------------------
  // Constant Float Registers (0 ~ 255)
  // - x , y , z , w
  // ---------------------------------------------
  0x3f800000, 0x3f800000, 0x3f800000, 0x3f800000,
};

#endif /*__FOG_PSA_H__*/