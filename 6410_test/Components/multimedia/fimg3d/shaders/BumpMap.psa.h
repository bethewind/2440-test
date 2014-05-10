#if !defined(__BUMPMAP_PSA_H__)
#define __BUMPMAP_PSA_H__

static const unsigned BumpMap_psa[] = {
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
  0x00000002,
  // INSTRUCTION SIZE
  0x00000048,
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
  0x00000001,
  // ---------------------------------------------
  // Instruction Area
  // - 0x0, 0x4, 0x8, 0xc (Data at 0xC will not be written.)
  // ---------------------------------------------
  0x02000000, 0x0002e400, 0x04402be4, 0x00000000,
  0x00000000, 0x010b0000, 0x08c02aff, 0x00000000,
  0x02000000, 0x010ae400, 0x033828ff, 0x00000000,
  0x00000000, 0x01080000, 0x00b828e4, 0x00000000,
  0x00000000, 0x01080002, 0x023827e4, 0x00000000,
  0x00000000, 0x01075502, 0x033825e4, 0x00000000,
  0x00000000, 0x01050000, 0x00b825e4, 0x00000000,
  0x01000000, 0x0001e407, 0x107823e4, 0x00000000,
  0x00000000, 0x01030000, 0x00b823e4, 0x00000000,
  0x00000000, 0x01030000, 0x00b823e4, 0x00000000,
  0x00000000, 0x01030002, 0x023826e4, 0x00000000,
  0x00000000, 0x01065502, 0x033824e4, 0x00000000,
  0x00000000, 0x01040000, 0x00b824e4, 0x00000000,
  0x00000000, 0x0002e407, 0x107821e4, 0x00000000,
  0x05000000, 0x0104e401, 0x044029e4, 0x00000000,
  0x00000000, 0x0109e400, 0x037822ff, 0x00000000,
  0x02000000, 0x0101e401, 0x027820e4, 0x00000000,
  0x00000000, 0x01000000, 0x00f810e4, 0x00000000,
  // ---------------------------------------------
  // Constant Float Registers (0 ~ 255)
  // - x , y , z , w
  // ---------------------------------------------
  0xbf000000, 0x40000000, 0x00000000, 0x00000000,
};

#endif /*__BUMPMAP_PSA_H__*/
