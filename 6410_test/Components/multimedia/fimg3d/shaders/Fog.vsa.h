#if !defined(__FOG_VSA_H__)
#define __FOG_VSA_H__

static const unsigned Fog_vsa[] = {
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
  0x0000002C,
  // CONSTANT FLOAT SIZE
  0x00000024,
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
  0x00000000, 0x0200e400, 0x048800e4, 0x00000000,
  0x00000000, 0x0201e400, 0x049000e4, 0x00000000,
  0x00000000, 0x0202e400, 0x04a000e4, 0x00000000,
  0x01000000, 0x0204e400, 0x04c020e4, 0x00000000,
  0x07000000, 0x01000002, 0x0a4020ff, 0x00000000,
  0x00000000, 0x0203e400, 0x048820e4, 0x00000000,
  0x05000000, 0x0100e402, 0x237801ff, 0x00000000,
  0x06000206, 0x01005502, 0x0e880300, 0x00000000,
  0x00000000, 0x02080000, 0x00f003f9, 0x00000000,
  0x00000000, 0x01000000, 0x00c00000, 0x00000000,
  0x00000000, 0x00020000, 0x00f802e4, 0x00000000,
  // ---------------------------------------------
  // Constant Float Registers (0 ~ 255)
  // - x , y , z , w
  // ---------------------------------------------
  0x00000000, 0x00000000, 0x00000000, 0x00000000,
  0x00000000, 0x00000000, 0x00000000, 0x00000000,
  0x00000000, 0x00000000, 0x00000000, 0x00000000,
  0x00000000, 0x00000000, 0x00000000, 0x00000000,
  0x3f0e00d2, 0x3f40b2ea, 0x3e622920, 0x3e99999a,
  0x3f800000, 0x3f800000, 0x3f800000, 0x3f800000,
  0x3f93b15b, 0xbd9d883c, 0x00000000, 0x00000000,
  0x00000000, 0x00000000, 0x00000000, 0x00000000,
  0x00000000, 0x00000000, 0x00000000, 0x3f800000,
};

#endif /*__FOG_VSA_H__*/
