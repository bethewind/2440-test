#if !defined(__TEX_BYPASS_PSA_H__)
#define __TEX_BYPASS_PSA_H__

static const unsigned tex_bypass_psa[] = {
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
  0x00000000,
  // OUTPUT ATTRIBUTE SIZE
  0x00000000,
  // SAMPLER SIZE
  0x00000000,
  // INSTRUCTION SIZE
  0x00000008,
  // CONSTANT FLOAT SIZE
  0x00000020,
  // CONSTANT INTEGER SIZE
  0x00000010,
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
  0x00000000, 0x0001e407, 0x107810e4, 0x00000000,
  0x00000000, 0x00000000, 0x1e000000, 0x00000000,
  // ---------------------------------------------
  // Constant Float Registers (0 ~ 255)
  // - x , y , z , w
  // ---------------------------------------------
  0x3f34fdf4, 0x3f000000, 0x3e4ccccd, 0x40400000,
  0x3efffeb0, 0xbf34fdf4, 0xbefffeb0, 0x00000000,
  0xbefff2e5, 0xbf34fdf4, 0x3efffeb0, 0x00000000,
  0x3fa66666, 0xbfa66666, 0x3f333333, 0x45a87800,
  0xbf000000, 0xbf000000, 0x3f000000, 0x3f800000,
  0x00000000, 0x00000000, 0x00000000, 0x00000000,
  0xbfe66666, 0x3f99999a, 0x40400000, 0x40466666,
  0x40a00000, 0xbf800000, 0xbf800000, 0x3f800000,
  // ---------------------------------------------
  // Constant Integer Registers (0 ~ 15)
  // - w/z/y/x ( w channel will not be written. )
  // - each channel has 8-bit width
  // ---------------------------------------------
  0x00020305,
  0x00010404,
  0x00030808,
  0x00021305,
  0x00010424,
  0x00030106,
  0x00020205,
  0x00010407,
  0x00030308,
  0x00020301,
  0x00010402,
  0x00030800,
  0x00020305,
  0x00010404,
  0x00030818,
  0x00030808,
};

#endif /*__TEX_BYPASS_PSA_H__*/
