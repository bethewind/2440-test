/******************************************************************************
*
* NAME         : fimg_sysutil.h
* TITLE        : FIMGSE ver. 1.0 Register Level API
* AUTHOR       : Thomas, Kim
* CREATED      : 12 Apr 2006
*
* COPYRIGHT    : Copyright(c) 2005-2006 by Samsung Electronics Limited. All
*                rights reserved. No part of this software, either material
*                or conceptual may be copied or distributed, transmitted,
*                transcribed, stored in a retrieval system or translated into
*                any human or computer language in any form by any means,
*                electronic, mechanical, manual or other-wise, or disclosed
*                to third parties without the express written permission of
*                Samsung Electronics. Semiconductor Business, System LSI
*                Division, Mobile Solution Development, Graphics IP Team
*                in Mobile Next Generation Technology.
*
* DESCRIPTION  : Provides debug functionality
*
* PLATFORM     : ALL
* HISTORY      : 2006. 4. 12 created
*	
******************************************************************************/
#if !defined(__SHADER_UTILITY_H__)
#define __SHADER_UTILITY_H__

//#include "config.h"

//extern "C" {
//#include "fgl.h"
//}

/****************************************************************************
 *  Enumerated types
 ****************************************************************************/
typedef enum ShaderInstructSet_Tag
{
    /*Instruction				    dec hex	bin	    Group	                    Instruction	Description */
    SHADER_INST_NOP		 =  0,	/*	0	00	000000	arithmetic (floating-point)	nop			no operation                                         */
    SHADER_INST_MOV		 =  1,	/*	1	01	000001	arithmetic (floating-point)	mov			move                                                 */
    SHADER_INST_MOVA	 =  2,	/*	2	02	000010	arithmetic (floating-point)	mova		move address                                         */
    SHADER_INST_MOVC	 =  3,	/*	3	03	000011	arithmetic (floating-point)	movc		mov with coordinate                                  */
    SHADER_INST_ADD		 =  4,	/*	4	04	000100	arithmetic (floating-point)	add			add                                                  */
    SHADER_INST_MUL		 =  6,	/*	6	06	000110	arithmetic (floating-point)	mul			multiply                                             */
    SHADER_INST_MUL_LIT	 =  7,	/*	7	07	000111	arithmetic (floating-point)	mul_lit		multiply for a lighting calculation                  */
    SHADER_INST_DP3		 =  8,	/*	8	08	001000	arithmetic (floating-point)	dp3			3D dot product                                       */
    SHADER_INST_DP4		 =  9,	/*	9	09	001001	arithmetic (floating-point)	dp4			4D dot product                                       */
    SHADER_INST_DPH		 = 10,	/*	10	0A	001010	arithmetic (floating-point)	dph			homogeneous dot product                              */
    SHADER_INST_DST		 = 11,	/*	11	0B	001011	arithmetic (floating-point)	dst			distance                                             */
    SHADER_INST_EXP		 = 12,	/*	12	0C	001100	arithmetic (floating-point)	exp			exponent                                             */
    SHADER_INST_EXP_LIT	 = 13,	/*	13	0D	001101	arithmetic (floating-point)	exp_lit		exponent for a lighting calculation                  */
    SHADER_INST_LOG		 = 14,	/*	14	0E	001110	arithmetic (floating-point)	log			logarithm                                            */
    SHADER_INST_LOG_LIT	 = 15,	/*	15	0F	001111	arithmetic (floating-point)	log_lit		logarithm for a lighting calculation                 */
    SHADER_INST_RCP		 = 16,	/*	16	10	010000	arithmetic (floating-point)	rcp			reciprocal                                           */
    SHADER_INST_RSQ		 = 17,	/*	17	11	010001	arithmetic (floating-point)	rsq			reciprocal square root                               */
    SHADER_INST_DP2ADD	 = 18,	/*	18	12	010010	arithmetic (floating-point)	dp2add		2D dot product with a scalar addtion                 */
    SHADER_INST_MAX		 = 20,	/*	20	14	010100	arithmetic (floating-point)	max			find channel wise maximum of two source operands     */
    SHADER_INST_MIN		 = 21,	/*	21	15	010101	arithmetic (floating-point)	min			find channel wise minimum of two source operands     */
    SHADER_INST_SGE		 = 22,	/*	22	16	010110	arithmetic (floating-point)	sge			set 1.0 if greater than or equal                     */
    SHADER_INST_SLT		 = 23,	/*	23	17	010111	arithmetic (floating-point)	slt			set 1.0 if less than                                 */
    SHADER_INST_SETP_EQ	 = 24,	/*	24	18	011000	arithmetic (floating-point)	setp_eq		set predicate register when equal                    */
    SHADER_INST_SETP_GE	 = 25,	/*	25	19	011001	arithmetic (floating-point)	setp_ge		set predicate register when greater than or equal    */
    SHADER_INST_SETP_GT	 = 26,	/*	26	1A	011010	arithmetic (floating-point)	setp_gt		set predicate register when greater than             */
    SHADER_INST_SETP_NE	 = 27,	/*	27	1B	011011	arithmetic (floating-point)	setp_ne		set predicate register when not equal                */
    SHADER_INST_CMP		 = 28,	/*	28	1C	011100	arithmetic (floating-point)	cmp			Compare                                              */
    SHADER_INST_MAD		 = 29,	/*	29	1D	011101	arithmetic (floating-point)	mad			multiply and add                                     */
    SHADER_INST_FRC		 = 30,	/*	30	1E	011110	arithmetic (floating-point)	frc			fraction                                             */
    SHADER_INST_TEXLD	 = 32,	/*	32	20	100000	texture						texld		texture load                                         */
    SHADER_INST_CUBEDIR	 = 33,	/*	33	21	100001	arithmetic (floating-point)	cubedir		find texture coordinate for cube map                 */
    SHADER_INST_MAXCOMP	 = 34,	/*	34	22	100010	arithmetic (floating-point)	maxcomp		find maximum value in each channel of source operand */
    SHADER_INST_TEXLDC	 = 35,	/*	35	23	100011	texture						texldc		texture load with coordinate                         */
    SHADER_INST_TEXKILL	 = 39,	/*	39	27	100111	texture						texkill		texel kill                                           */
    SHADER_INST_MOVIPS	 = 40,	/*	40	28	101000	arithmetic (integer)		movips		mov integer with push                                */
    SHADER_INST_ADDI	 = 41,	/*	41	29	101001	arithmetic (integer)		addi		add integer                                          */
    SHADER_INST_B		 = 48,	/*	48	30	110000	flow control				b			branch                                               */
    SHADER_INST_BF		 = 49,	/*	49	31	110001	flow control				bf			branch false                                         */
    SHADER_INST_BP		 = 52,	/*	52	34	110100	flow control				bp			branch with pop                                      */
    SHADER_INST_BFP		 = 53,	/*	53	35	110101	flow control				bfp			branch false with pop                                */
    SHADER_INST_BZP		 = 54,	/*	54	36	110110	flow control				bzp			branch zero with pop                                 */
    SHADER_INST_CALL	 = 56,	/*	56	38	111000	flow control				call		call                                                 */
    SHADER_INST_CALLNZ	 = 57,	/*	57	39	111001	flow control				callnz		call not zero                                        */
    SHADER_INST_RET		 = 60,	/*	60	3C	111100	flow control				ret			return                                               */
} ShaderInstructSet;



typedef enum DestinRegisterType_Tag
{
	DESTIN_REGISTER_OUTPUT = 0,
	DESTIN_REGISTER_TEMPORARY,
	DESTIN_REGISTER_RESERVED,
	DESTIN_REGISTER_PREDICATE,
	DESTIN_REGISTER_ADDRESS,
	DESTIN_REGISTER_LOOPCOUNT
} DestinRegisterType;

typedef enum SourceRegisterType_Tag
{
	SOURCE_REGISTER_INPUT = 0,
	SOURCE_REGISTER_TEMPORARY,
	SOURCE_REGISTER_CFLOAT,
	SOURCE_REGISTER_CINTEGER,
	SOURCE_REGISTER_LOOPCOUNT,
	SOURCE_REGISTER_CBOOL,
	SOURCE_REGISTER_PREDICATE
} SourceRegisterType;

typedef enum SourceModifierType_Tag
{
	SOURCE_MODIFIER_NORMAL = 0,
	SOURCE_MODIFIER_NEGATE,
	SOURCE_MODIFIER_ABSOLUTE,
	SOURCE_MODIFIER_ABSOLUTE_NEGATE
} SourceModifierType;


typedef struct InstructOpcode_Tag
{
	unsigned int u3op;
	unsigned int uOpcode;
} InstOpcode, *pInstOpcode;

typedef struct InstructDestin_Tag
{
	unsigned int uMask;
	unsigned int uModifier;
	unsigned int uA;
	unsigned int uRegType;
	unsigned int uRegNum;
	int          uTargetAddr;
} InstDst, *pInstDst;

typedef struct InstructSource0_Tag
{
	unsigned int uSwizzle;
	unsigned int uModifier;
	unsigned int uAr;
	unsigned int uRegType;
	unsigned int uExtNum;
	unsigned int uRegNum;
} InstSrc0, *pInstSrc0;

typedef struct InstructSource1_Tag
{
	unsigned int uSwizzle;
	unsigned int uModifier;
	unsigned int uP;
	unsigned int uPn;
	unsigned int uAr;
	unsigned int uRegType;
	unsigned int uPa;
	unsigned int uPch;
	unsigned int uRegNum;
} InstSrc1, *pInstSrc1;

typedef struct InstructSource2_Tag
{
	unsigned int uSwizzle;
	unsigned int uModifier;
	unsigned int uAr;
	unsigned int uRegType;
	unsigned int uRegNum;
} InstSrc2, *pInstSrc2;

typedef struct ShaderInstruct_Tag
{
	InstOpcode Opcode;
	InstDst    Dest;
	InstSrc0   Src0;
	InstSrc1   Src1;
	InstSrc2   Src2;
} ShaderInstruct, *pShaderInstruct;


typedef struct ShaderInstructList_Tag
{
	unsigned int	uOperandSize;
    char            *InstructName;            
} ShaderInstructList, *pShaderInstructList;



unsigned int
fglDasmVertexShader(void);

unsigned int
fglDasmFragmentShader(void);

unsigned int
fglDasmConstFloatReg(bool isVertexShader, unsigned int uSizeConstFloat);

unsigned int
fglDasmConstIntegerReg(bool isVertexShader, unsigned int uSizeConstInteger);

unsigned int
fglDasmConstBoolReg(bool isVertexShader, unsigned int uSizeConstBool);


#endif	/* __SHADER_UTILITY_H__ */
