/*******************************************************************************
 *
 *	ViP model for FIMGSE
 *		version 1.0
 *
 *	fimg_sysutil.c
 *
 *	by cheolkyoo.kim
 *	Graphics IP Team in AP, Mobile Solution Development,
 *	System LSI Division, Semiconductor Business,
 *	Samsung Electronics
 *
 *	Copyright (c) 2005 FIMG team
 *
 *	All rights reserved. No part of this program may be reproduced, stored
 *	in a retrieval system, or tranmitted, in any form or by any means,
 *	electronic, mechanical, photocopying, recording, or otherwise, without
 *	the prior written permission of the author.
 *
 *	2006. 1. 3	by cheolkyoo.kim
 *		Provides functions to manage memory, register.
 *	
 ******************************************************************************/
/****************************************************************************
 *  INCLUDES
 ****************************************************************************/
#include <stdio.h>
#include <string.h>
//#include <math.h>
#include "system.h"
#include "library.h"
//nclude "util.h"
//#include "Config.h"
//#include "Defines.h"

#include "register.h"
#include "fgl.h"
#include "ShaderUtility.h"


/****************************************************************************
 *  LOCAL FUNCTION
 ****************************************************************************/

unsigned int 
_DestUpdateMask(unsigned int uMaskVal, char *StrBuf);

unsigned int
_SrcSwizzle(unsigned int uSwizzleVal, char *StrBuf);

#if 0
char *InstructName[] = 
{
	"nop",		/* no operation        */
	"mov",		/* move                */
	"mova",	    /* move address        */
	"movc",	    /* mov with coordinate */
	"add",		/* add                 */
	"na",       /* reserved */
	"mul",	 	/* multiply                             */
	"mul_lit",	/* multiply for a lighting calculation  */
	"dp3",		/* 3D dot product                       */
	"dp4",		/* 4D dot product                       */
	"dph",		/* homogeneous dot product              */
	"dst",		/* distance                             */
	"exp",		/* exponent                             */
	"exp_lit",	/* exponent for a lighting calculation  */
	"log",		/* logarithm                            */
	"log_lit",	/* logarithm for a lighting calculation */
	"rcp",		/* reciprocal                           */
	"rsq",		/* reciprocal square root               */
	"dp2add",   /* 2D dot product with a scalar addtion */
	"na",       /* reserved */
	"max",  	/* find channel wise maximum of two source operands  */
	"min",  	/* find channel wise minimum of two source operands  */
	"sge",  	/* set 1.0 if greater than or equal                  */
	"slt",  	/* set 1.0 if less than                              */
	"setp_eq",	/* set predicate register when equal                 */
	"setp_ge",	/* set predicate register when greater than or equal */
	"setp_gt",	/* set predicate register when greater than          */
	"setp_ne",	/* set predicate register when not equal             */
	"cmp",		/* Compare                                           */
	"mad",		/* multiply and add                                  */
	"frc",		/* fraction                                          */
	"na",	    /* reserved */
	"texld",	/* texture load                                         */
	"cubedir",	/* find texture coordinate for cube map                 */
	"maxcomp",	/* find maximum value in each channel of source operand */
	"texldc",   /* texture load with coordinate                         */
	"na",       /* reserved */
	"na",       /* reserved */
	"na",       /* reserved */
	"texkill",	/* texel kill             */
	"movips",	/* mov integer with push  */
	"addi",	    /* add integer            */
	"na",       /* reserved */
	"na",       /* reserved */
	"na",       /* reserved */
	"na",       /* reserved */
	"na",       /* reserved */
	"na",       /* reserved */
	"b",        /* branch                */
	"bf",	    /* branch false          */
	"na",       /* reserved */                         
	"na",       /* reserved */                         
	"bp",       /* branch with pop       */
	"bfp", 	    /* branch false with pop */
	"bzp", 	    /* branch zero with pop  */
	"na", 	    /* reserved */
	"call", 	/* call                  */
	"callnz",	/* call not zero         */
	"na",       /* reserved */
	"na",       /* reserved */
	"ret" 		/* return                */
};
#endif

ShaderInstructList InstructList[] = 
{
	{ 0, "nop"		},	/* no operation        */
	{ 1, "mov"		},	/* move                */
	{ 1, "mova"		},  /* move address        */
	{ 2, "movc"		},  /* mov with coordinate */
	{ 2, "add"		},	/* add                 */
	{ 0, "na"       }, 	/* reserved */
	{ 2, "mul"	 	},	/* multiply                             */
	{ 2, "mul_lit"	},	/* multiply for a lighting calculation  */
	{ 2, "dp3"		},	/* 3D dot product                       */
	{ 2, "dp4"		},	/* 4D dot product                       */
	{ 2, "dph"		},	/* homogeneous dot product              */
	{ 2, "dst"		},	/* distance                             */
	{ 1, "exp"		},	/* exponent                             */
	{ 1, "exp_lit"	},	/* exponent for a lighting calculation  */
	{ 1, "log"		},	/* logarithm                            */
	{ 1, "log_lit"	},	/* logarithm for a lighting calculation */
	{ 1, "rcp"		},	/* reciprocal                           */
	{ 1, "rsq"		},	/* reciprocal square root               */
	{ 3, "dp2add"   }, 	/* 2D dot product with a scalar addtion */
	{ 0, "na"       }, 	/* reserved */
	{ 2, "max"  	},	/* find channel wise maximum of two source operands  */
	{ 2, "min"  	},	/* find channel wise minimum of two source operands  */
	{ 2, "sge"  	},	/* set 1.0 if greater than or equal                  */
	{ 2, "slt"  	},	/* set 1.0 if less than                              */
	{ 2, "setp_eq"	},	/* set predicate register when equal                 */
	{ 2, "setp_ge"	},	/* set predicate register when greater than or equal */
	{ 2, "setp_gt"	},	/* set predicate register when greater than          */
	{ 2, "setp_ne"	},	/* set predicate register when not equal             */
	{ 3, "cmp"		},	/* Compare                                           */
	{ 3, "mad"		},	/* multiply and add                                  */
	{ 1, "frc"		},	/* fraction                                          */
	{ 0, "na"	    },	/* reserved */
	{ 2, "texld"	},	/* texture load                                         */
	{ 1, "cubedir"	},	/* find texture coordinate for cube map                 */
	{ 1, "maxcomp"	},	/* find maximum value in each channel of source operand */
	{ 3, "texldc"   }, 	/* texture load with coordinate                         */
	{ 0, "na"       }, 	/* reserved */
	{ 0, "na"       }, 	/* reserved */
	{ 0, "na"       }, 	/* reserved */
	{ 1, "texkill"	},	/* texel kill             */
	{ 1, "movips"	},	/* mov integer with push  */
	{ 2, "addi" 	},  /* add integer            */
	{ 0, "na"       }, 	/* reserved */
	{ 0, "na"       }, 	/* reserved */
	{ 0, "na"       }, 	/* reserved */
	{ 0, "na"       }, 	/* reserved */
	{ 0, "na"       }, 	/* reserved */
	{ 0, "na"       }, 	/* reserved */
	{ 0, "b"        }, 	/* branch                */
	{ 1, "bf"	    },	/* branch false          */
	{ 0, "na"		}, 	/* reserved */                         
	{ 0, "na"		}, 	/* reserved */                         
	{ 0, "bp"		}, 	/* branch with pop       */
	{ 1, "bfp"	 	},  /* branch false with pop */
	{ 1, "bzp"	 	},  /* branch zero with pop  */
	{ 0, "na" 	    },	/* reserved */
	{ 0, "call" 	},	/* call                  */
	{ 1, "callnz"	},	/* call not zero         */
	{ 0, "na"       }, 	/* reserved */
	{ 0, "na"       }, 	/* reserved */
	{ 0, "ret" 		}	/* return                */
};

char *DestRegTypeName[] = 
{
 /* Register	   Name							Count	R/W		#Read     Port_Dim	RelAddr		Req_dcl  */
	"o",		/* Output Register				10		W		1			4		aL			Yes      */
	"r",		/* Temporary Register			32		R/W		2 (or 3)	4		aL			No       */
    "na", 
	"p",		/* Predicate  Register			7		R/W		1			1		No			No       */
	"a0",		/* Address Register				1		R/W		1			4		No			No       */
	"aL"		/* Loop Count  Register			4		R		1			4		No			No       */
};

char *VertSrcRegTypeName[] = 
{
/*  Register	   Name							Count	R/W		#Read     Port_Dim	RelAddr		Req_dcl  */
	"v",		/* Input Register				10/8	R		1			4		aL			Yes      */
	"r",		/* Temporary Register			32		R/W		2 (or 3)	4		aL			No       */
	"c",		/* Constant Float Register		256		R		1			4		aL/a0		No       */
	"i",		/* Constant Integer Register	16		R		1			4		No			No       */
	"aL",		/* Loop Count  Register			4		R		1			4		No			No       */
	"b",		/* Constant Boolean Register	16		R		1			1		No			No       */
	"p"			/* Predicate  Register			7		R/W		1			1		No			No       */
};

char *FragSrcRegTypeName[] = 
{
/*  Register	   Name							Count	R/W		#Read     Port_Dim	RelAddr		Req_dcl  */
	"v",		/* Input Register				10/8	R		1			4		aL			Yes      */
	"r",		/* Temporary Register			32		R/W		2 (or 3)	4		aL			No       */
	"c",		/* Constant Float Register		256		R		1			4		aL/a0		No       */
	"i",		/* Constant Integer Register	16		R		1			4		No			No       */
	"aL",		/* Loop Count  Register			4		R		1			4		No			No       */
	"b",		/* Constant Boolean Register	16		R		1			1		No			No       */
	"p",		/* Predicate  Register			7		R/W		1			1		No			No       */
	"s",		/* Sampler Register				4/8		R		1			-		No			Yes      */
	"d",		/* LOD Register					8		R		1			-		No			Yes      */
	"vFace",	/* Face Register				1		R		1			1		No			Yes      */
	"vPos"		/* Position    Register			1		R		1			4		No			Yes      */
};


#define FGL_BITFIELD_MASK(fields) ((0xFFFFFFFF>>(31-(1?fields))) & (0xFFFFFFFF<<(0?fields)))

/* Extract a bitfield */
/* Use with 2nd arg as colon separated bitfield defn - no brackets round 2nd arg */
/* e.g. FGL_EXTRACT_BITFIELD(0x00003c00, 11:8) == c */
#define FGL_GET_BITFIELD(flags, fields) (((flags)&FGL_BITFIELD_MASK(fields))>>(0?fields))


unsigned int
fglDasmVertexShader()
{
    bool isThreeOperand = false;
    unsigned int uPCRange = READREG(FGVS_PC_RANGE);
    unsigned int uPCStart = FGL_GET_BITFIELD(uPCRange, 8:0);
    unsigned int uPCEnd   = FGL_GET_BITFIELD(uPCRange, 24:16);
    unsigned int Instruction[3] = { 0, };
    
    unsigned int *pInstMemAddr = (unsigned int *)(FGVS_INSTMEM_SADDR + uPCStart * sizeof(unsigned int) * 4);
    
    ShaderInstruct VtxSdrInst;
    char Assembly[256];
    char TempBuf[256];
    char TempMask[5];
    
    unsigned int uMaxConstFloat = 0;
    unsigned int uMaxConstInt = 0;
    unsigned int uMaxConstBool = 0;
    
    UART_Printf("\n\n");
    UART_Printf(">Instruction slots of vertex shader\n\n");	//Disp
    
    for(int i = uPCStart; i < (uPCEnd + 1); i++)
    {
         Instruction[0] = READREG(pInstMemAddr++);
         Instruction[1] = READREG(pInstMemAddr++);
         Instruction[2] = READREG(pInstMemAddr++);
         pInstMemAddr++;
         
// source 2
    	VtxSdrInst.Src2.uRegNum   = FGL_GET_BITFIELD(Instruction[0], 4:0);
    	VtxSdrInst.Src2.uRegType  = FGL_GET_BITFIELD(Instruction[0], 10:8);
    	VtxSdrInst.Src2.uAr       = FGL_GET_BITFIELD(Instruction[0], 11:11);
    	VtxSdrInst.Src2.uModifier = FGL_GET_BITFIELD(Instruction[0], 15:14);
    	VtxSdrInst.Src2.uSwizzle  = FGL_GET_BITFIELD(Instruction[0], 23:16);
// source 1
    	VtxSdrInst.Src1.uRegNum   = FGL_GET_BITFIELD(Instruction[0], 28:24);
    	VtxSdrInst.Src1.uPch      = FGL_GET_BITFIELD(Instruction[0], 30:29);
    	VtxSdrInst.Src1.uPa       = FGL_GET_BITFIELD(Instruction[0], 31:31);    	
    	VtxSdrInst.Src1.uRegType  = FGL_GET_BITFIELD(Instruction[1], 2:0);
    	VtxSdrInst.Src1.uAr       = FGL_GET_BITFIELD(Instruction[1], 3:3);
    	VtxSdrInst.Src1.uPn       = FGL_GET_BITFIELD(Instruction[1], 4:4);
     	VtxSdrInst.Src1.uP        = FGL_GET_BITFIELD(Instruction[1], 5:5);
     	VtxSdrInst.Src1.uModifier = FGL_GET_BITFIELD(Instruction[1], 7:6);
  	    VtxSdrInst.Src1.uSwizzle  = FGL_GET_BITFIELD(Instruction[1], 15:8);
// source 0
    	VtxSdrInst.Src0.uRegNum   = FGL_GET_BITFIELD(Instruction[1], 20:16);
    	VtxSdrInst.Src0.uExtNum   = FGL_GET_BITFIELD(Instruction[1], 23:21);
    	VtxSdrInst.Src0.uRegType  = FGL_GET_BITFIELD(Instruction[1], 26:24);
    	VtxSdrInst.Src0.uAr       = FGL_GET_BITFIELD(Instruction[1], 29:27);
    	VtxSdrInst.Src0.uModifier = FGL_GET_BITFIELD(Instruction[1], 31:30);
    	VtxSdrInst.Src0.uSwizzle  = FGL_GET_BITFIELD(Instruction[2], 7:0);
// destination
    	VtxSdrInst.Dest.uRegNum   = FGL_GET_BITFIELD(Instruction[2], 12:8);
    	VtxSdrInst.Dest.uRegType  = FGL_GET_BITFIELD(Instruction[2], 15:13);
    	VtxSdrInst.Dest.uA        = FGL_GET_BITFIELD(Instruction[2], 16:16);
    	VtxSdrInst.Dest.uModifier = FGL_GET_BITFIELD(Instruction[2], 18:17);
    	VtxSdrInst.Dest.uMask     = FGL_GET_BITFIELD(Instruction[2], 22:19);
    	VtxSdrInst.Dest.uTargetAddr = 0;
// opcode
    	VtxSdrInst.Opcode.uOpcode = FGL_GET_BITFIELD(Instruction[2], 28:23);
    	VtxSdrInst.Opcode.u3op    = FGL_GET_BITFIELD(Instruction[2], 29:29);

        unsigned int uRegNum = 0;

        // data processing instruction
        if(VtxSdrInst.Opcode.uOpcode < SHADER_INST_B)
        {
            strcpy(Assembly, " ");
            strcat(Assembly, InstructList[VtxSdrInst.Opcode.uOpcode].InstructName);
            //strcat(Assembly, tex2d[i].texName);
            
            if(VtxSdrInst.Dest.uModifier == 1)
                strcat(Assembly, "_sat");
            
            // destination
            strcat(Assembly, " ");
            
            //unsigned int ret = _DestUpdateMask(VtxSdrInst.Dest.uMask, &TempMask[0]);
            if(_DestUpdateMask(VtxSdrInst.Dest.uMask, &TempMask[0]) == 1)
                sprintf(TempBuf, "%s%d", DestRegTypeName[VtxSdrInst.Dest.uRegType], VtxSdrInst.Dest.uRegNum);
            else
                sprintf(TempBuf, "%s%d.%s", DestRegTypeName[VtxSdrInst.Dest.uRegType], VtxSdrInst.Dest.uRegNum, TempMask);
            
            strcat(Assembly, TempBuf);
            strcat(Assembly, ", ");
            
            // source 0
            if(VtxSdrInst.Src0.uRegType == SOURCE_REGISTER_CFLOAT)
            {
                uRegNum = VtxSdrInst.Src0.uRegNum + (VtxSdrInst.Src0.uExtNum << 5);
                if(uRegNum > uMaxConstFloat) uMaxConstFloat = uRegNum;
            }
            else
                uRegNum = VtxSdrInst.Src0.uRegNum;
            
            if(VtxSdrInst.Src0.uRegType == SOURCE_REGISTER_CINTEGER)
                if(VtxSdrInst.Src0.uRegNum > uMaxConstInt) uMaxConstInt = VtxSdrInst.Src0.uRegNum;
            
            if(VtxSdrInst.Src0.uRegType == SOURCE_REGISTER_CBOOL)
                if(VtxSdrInst.Src0.uRegNum > uMaxConstBool) uMaxConstBool = VtxSdrInst.Src0.uRegNum;
                
            if(_SrcSwizzle(VtxSdrInst.Src0.uSwizzle, &TempMask[0]) == 1)
            {
                if(VtxSdrInst.Src0.uModifier == SOURCE_MODIFIER_NEGATE)
                    sprintf(TempBuf, "-%s%d", VertSrcRegTypeName[VtxSdrInst.Src0.uRegType], uRegNum);
	            else if(VtxSdrInst.Src0.uModifier == SOURCE_MODIFIER_ABSOLUTE)
	                sprintf(TempBuf, "|%s%d|", VertSrcRegTypeName[VtxSdrInst.Src0.uRegType], uRegNum);
	            else if(VtxSdrInst.Src0.uModifier == SOURCE_MODIFIER_ABSOLUTE_NEGATE)
	                sprintf(TempBuf, "-|%s%d|", VertSrcRegTypeName[VtxSdrInst.Src0.uRegType], uRegNum);
	            else 
                    sprintf(TempBuf, "%s%d", VertSrcRegTypeName[VtxSdrInst.Src0.uRegType], uRegNum);
            }
            else
            {
                if(VtxSdrInst.Src0.uModifier == SOURCE_MODIFIER_NEGATE)
                    sprintf(TempBuf, "-%s%d.%s", VertSrcRegTypeName[VtxSdrInst.Src0.uRegType], uRegNum, TempMask);
	            else if(VtxSdrInst.Src0.uModifier == SOURCE_MODIFIER_ABSOLUTE)
	                sprintf(TempBuf, "|%s%d.%s|", VertSrcRegTypeName[VtxSdrInst.Src0.uRegType], uRegNum, TempMask);
	            else if(VtxSdrInst.Src0.uModifier == SOURCE_MODIFIER_ABSOLUTE_NEGATE)
	                sprintf(TempBuf, "-|%s%d.%s|", VertSrcRegTypeName[VtxSdrInst.Src0.uRegType], uRegNum, TempMask);
	            else 
                    sprintf(TempBuf, "%s%d.%s", VertSrcRegTypeName[VtxSdrInst.Src0.uRegType], uRegNum, TempMask);
            }
            
            strcat(Assembly, TempBuf);
         
            if(InstructList[VtxSdrInst.Opcode.uOpcode].uOperandSize > 1)
            {
                strcat(Assembly, ", ");
                // source 1
                if(VtxSdrInst.Src1.uRegType == SOURCE_REGISTER_CFLOAT)
                {
                    uRegNum = VtxSdrInst.Src1.uRegNum + (VtxSdrInst.Src0.uExtNum << 5);
                    if(uRegNum > uMaxConstFloat) uMaxConstFloat = uRegNum;
                }
                else
                    uRegNum = VtxSdrInst.Src1.uRegNum;
                    
                if(VtxSdrInst.Src1.uRegType == SOURCE_REGISTER_CINTEGER)
                    if(VtxSdrInst.Src1.uRegNum > uMaxConstInt) uMaxConstInt = VtxSdrInst.Src1.uRegNum;
                
                if(VtxSdrInst.Src1.uRegType == SOURCE_REGISTER_CBOOL)
                    if(VtxSdrInst.Src1.uRegNum > uMaxConstBool) uMaxConstBool = VtxSdrInst.Src1.uRegNum;
/*            
                if(_SrcSwizzle(VtxSdrInst.Src1.uSwizzle, &TempMask[0]) == 1)
                    sprintf(TempBuf, "%s%d", VertSrcRegTypeName[VtxSdrInst.Src1.uRegType], uRegNum);
                else
                    sprintf(TempBuf, "%s%d.%s", VertSrcRegTypeName[VtxSdrInst.Src1.uRegType], uRegNum, TempMask);
*/
                if(_SrcSwizzle(VtxSdrInst.Src1.uSwizzle, &TempMask[0]) == 1)
                {
                    if(VtxSdrInst.Src1.uModifier == SOURCE_MODIFIER_NEGATE)
                        sprintf(TempBuf, "-%s%d", VertSrcRegTypeName[VtxSdrInst.Src1.uRegType], uRegNum);
    	            else if(VtxSdrInst.Src1.uModifier == SOURCE_MODIFIER_ABSOLUTE)
    	                sprintf(TempBuf, "|%s%d|", VertSrcRegTypeName[VtxSdrInst.Src1.uRegType], uRegNum);
    	            else if(VtxSdrInst.Src1.uModifier == SOURCE_MODIFIER_ABSOLUTE_NEGATE)
    	                sprintf(TempBuf, "-|%s%d|", VertSrcRegTypeName[VtxSdrInst.Src1.uRegType], uRegNum);
    	            else 
                        sprintf(TempBuf, "%s%d", VertSrcRegTypeName[VtxSdrInst.Src1.uRegType], uRegNum);
                }
                else
                {
                    if(VtxSdrInst.Src1.uModifier == SOURCE_MODIFIER_NEGATE)
                        sprintf(TempBuf, "-%s%d.%s", VertSrcRegTypeName[VtxSdrInst.Src1.uRegType], uRegNum, TempMask);
    	            else if(VtxSdrInst.Src1.uModifier == SOURCE_MODIFIER_ABSOLUTE)
    	                sprintf(TempBuf, "|%s%d.%s|", VertSrcRegTypeName[VtxSdrInst.Src1.uRegType], uRegNum, TempMask);
    	            else if(VtxSdrInst.Src1.uModifier == SOURCE_MODIFIER_ABSOLUTE_NEGATE)
    	                sprintf(TempBuf, "-|%s%d.%s|", VertSrcRegTypeName[VtxSdrInst.Src1.uRegType], uRegNum, TempMask);
    	            else 
                        sprintf(TempBuf, "%s%d.%s", VertSrcRegTypeName[VtxSdrInst.Src1.uRegType], uRegNum, TempMask);
                }
                
                strcat(Assembly, TempBuf);
                
                // 3rd operand
                if(isThreeOperand)
                {
                    strcat(Assembly, ", ");
                    // source 2
                    if(VtxSdrInst.Src2.uRegType == SOURCE_REGISTER_CFLOAT)
                    {
                        uRegNum = VtxSdrInst.Src2.uRegNum + (VtxSdrInst.Src0.uExtNum << 5);
                        if(uRegNum > uMaxConstFloat) uMaxConstFloat = uRegNum;
                    }
                    else
                        uRegNum = VtxSdrInst.Src2.uRegNum;
                        
                    if(VtxSdrInst.Src2.uRegType == SOURCE_REGISTER_CINTEGER)
                        if(VtxSdrInst.Src2.uRegNum > uMaxConstInt) uMaxConstInt = VtxSdrInst.Src2.uRegNum;
                    
                    if(VtxSdrInst.Src2.uRegType == SOURCE_REGISTER_CBOOL)
                        if(VtxSdrInst.Src2.uRegNum > uMaxConstBool) uMaxConstBool = VtxSdrInst.Src2.uRegNum;
/*            
                    if(_SrcSwizzle(VtxSdrInst.Src2.uSwizzle, &TempMask[0]) == 1)
                        sprintf(TempBuf, "%s%d", VertSrcRegTypeName[VtxSdrInst.Src2.uRegType], uRegNum);
                    else
                        sprintf(TempBuf, "%s%d.%s", VertSrcRegTypeName[VtxSdrInst.Src2.uRegType], uRegNum, TempMask);
*/
                    if(_SrcSwizzle(VtxSdrInst.Src2.uSwizzle, &TempMask[0]) == 1)
                    {
                        if(VtxSdrInst.Src2.uModifier == SOURCE_MODIFIER_NEGATE)
                            sprintf(TempBuf, "-%s%d", VertSrcRegTypeName[VtxSdrInst.Src2.uRegType], uRegNum);
        	            else if(VtxSdrInst.Src2.uModifier == SOURCE_MODIFIER_ABSOLUTE)
        	                sprintf(TempBuf, "|%s%d|", VertSrcRegTypeName[VtxSdrInst.Src2.uRegType], uRegNum);
        	            else if(VtxSdrInst.Src2.uModifier == SOURCE_MODIFIER_ABSOLUTE_NEGATE)
        	                sprintf(TempBuf, "-|%s%d|", VertSrcRegTypeName[VtxSdrInst.Src2.uRegType], uRegNum);
        	            else 
                            sprintf(TempBuf, "%s%d", VertSrcRegTypeName[VtxSdrInst.Src2.uRegType], uRegNum);
                    }
                    else
                    {
                        if(VtxSdrInst.Src2.uModifier == SOURCE_MODIFIER_NEGATE)
                            sprintf(TempBuf, "-%s%d.%s", VertSrcRegTypeName[VtxSdrInst.Src2.uRegType], uRegNum, TempMask);
        	            else if(VtxSdrInst.Src2.uModifier == SOURCE_MODIFIER_ABSOLUTE)
        	                sprintf(TempBuf, "|%s%d.%s|", VertSrcRegTypeName[VtxSdrInst.Src2.uRegType], uRegNum, TempMask);
        	            else if(VtxSdrInst.Src2.uModifier == SOURCE_MODIFIER_ABSOLUTE_NEGATE)
        	                sprintf(TempBuf, "-|%s%d.%s|", VertSrcRegTypeName[VtxSdrInst.Src2.uRegType], uRegNum, TempMask);
        	            else 
                            sprintf(TempBuf, "%s%d.%s", VertSrcRegTypeName[VtxSdrInst.Src2.uRegType], uRegNum, TempMask);
                    }
                    
                    strcat(Assembly, TempBuf);
                    isThreeOperand = false;
                }
            }
            
            
            if(VtxSdrInst.Opcode.u3op)
            {
                isThreeOperand = true;
            }
        } 
        else /* flow control instruction*/
        {
        	VtxSdrInst.Dest.uRegNum   = 0;
        	VtxSdrInst.Dest.uRegType  = 0;
        	VtxSdrInst.Dest.uA        = 0;
        	VtxSdrInst.Dest.uModifier = 0;
        	VtxSdrInst.Dest.uMask     = 0;

            bool isMinusSign; // = FGL_GET_BITFIELD(Instruction[2], 16:16);
            unsigned int uTargetAddr = FGL_GET_BITFIELD(Instruction[2], 15:8);
            
            if(FGL_GET_BITFIELD(Instruction[2], 16:16) == 1)
                isMinusSign = true;
            else
                isMinusSign = false;
            
            if(isMinusSign)
                VtxSdrInst.Dest.uTargetAddr = (int)uTargetAddr * (-1);
            else
                VtxSdrInst.Dest.uTargetAddr = uTargetAddr;
            
            //strcpy(Assembly, InstructList[VtxSdrInst.Opcode.uOpcode].InstructName);
            strcpy(Assembly, " ");
            strcat(Assembly, InstructList[VtxSdrInst.Opcode.uOpcode].InstructName);
            
            // destination
            if(SHADER_INST_RET != (ShaderInstructSet)VtxSdrInst.Opcode.uOpcode)
            {
                sprintf(TempBuf, " %d", VtxSdrInst.Dest.uTargetAddr);
                strcat(Assembly, TempBuf);
            }
            
            // source 0
            if(InstructList[VtxSdrInst.Opcode.uOpcode].uOperandSize > 0)
            {
                strcat(Assembly, ", ");
                if(VtxSdrInst.Src0.uRegType == SOURCE_REGISTER_CFLOAT)
                {
                    uRegNum = VtxSdrInst.Src0.uRegNum + (VtxSdrInst.Src0.uExtNum << 5);
                    if(uRegNum > uMaxConstFloat) uMaxConstFloat = uRegNum;
                }
                else
                    uRegNum = VtxSdrInst.Src0.uRegNum;
                    
                if(VtxSdrInst.Src0.uRegType == SOURCE_REGISTER_CINTEGER)
                    if(VtxSdrInst.Src0.uRegNum > uMaxConstInt) uMaxConstInt = VtxSdrInst.Src0.uRegNum;
                
                if(VtxSdrInst.Src0.uRegType == SOURCE_REGISTER_CBOOL)
                    if(VtxSdrInst.Src0.uRegNum > uMaxConstBool) uMaxConstBool = VtxSdrInst.Src0.uRegNum;
            
                if(_SrcSwizzle(VtxSdrInst.Src0.uSwizzle, &TempMask[0]) == 1)
                    sprintf(TempBuf, "%s%d", VertSrcRegTypeName[VtxSdrInst.Src0.uRegType], uRegNum);
                else
                    sprintf(TempBuf, "%s%d.%s", VertSrcRegTypeName[VtxSdrInst.Src0.uRegType], uRegNum, TempMask);
                
                strcat(Assembly, TempBuf);
            }
        }

        UART_Printf("%s\n", Assembly);
    }
    
    
    if(uMaxConstFloat)
    {
        fglDasmConstFloatReg(true, uMaxConstFloat);        
    }

    if(uMaxConstInt)
    {
        fglDasmConstIntegerReg(true, uMaxConstFloat);        
    }

    if(uMaxConstBool)
    {
        fglDasmConstBoolReg(true, uMaxConstFloat);        
    }
    
    return ( uPCEnd - uPCStart);
}


unsigned int
fglDasmFragmentShader()
{
    bool isThreeOperand = false;
    unsigned int uPCStart;
    unsigned int uPCEnd;
    unsigned int Instruction[3] = { 0, };
    
    unsigned int *pInstMemAddr; // = (unsigned int *)(FGPS_INSTMEM_SADDR + uPCStart * sizeof(unsigned int) * 4);
    
    //ShaderInstruct VtxSdrInst;
    ShaderInstruct FrgSdrInst;
    
    char Assembly[256];
    char TempBuf[256];
    char TempMask[5];
    
    unsigned int uMaxConstFloat = 0;
    unsigned int uMaxConstInt = 0;
    unsigned int uMaxConstBool = 0;

	WRITEREG(FGPS_EXE_MODE, 0);
	uPCStart = READREG(FGPS_PC_START); 			// Start address of pixel shader program
	uPCEnd = READREG(FGPS_PC_END); 				// End address of pixel shader program
	WRITEREG(FGPS_EXE_MODE, 1);

    pInstMemAddr = (unsigned int *)(FGPS_INSTMEM_SADDR + uPCStart * sizeof(unsigned int) * 4);
    
    UART_Printf("\n\n");
    UART_Printf("> Instruction slots of fragment shader\n\n");
    
    for(int i = uPCStart; i < (uPCEnd + 1); i++)
    {
         Instruction[0] = READREG(pInstMemAddr++);
         Instruction[1] = READREG(pInstMemAddr++);
         Instruction[2] = READREG(pInstMemAddr++);
         pInstMemAddr++;
         
// source 2
    	FrgSdrInst.Src2.uRegNum   = FGL_GET_BITFIELD(Instruction[0], 4:0);
    	FrgSdrInst.Src2.uRegType  = FGL_GET_BITFIELD(Instruction[0], 10:8);
    	FrgSdrInst.Src2.uAr       = FGL_GET_BITFIELD(Instruction[0], 11:11);
    	FrgSdrInst.Src2.uModifier = FGL_GET_BITFIELD(Instruction[0], 15:14);
    	FrgSdrInst.Src2.uSwizzle  = FGL_GET_BITFIELD(Instruction[0], 23:16);
// source 1
    	FrgSdrInst.Src1.uRegNum   = FGL_GET_BITFIELD(Instruction[0], 28:24);
    	FrgSdrInst.Src1.uPch      = FGL_GET_BITFIELD(Instruction[0], 30:29);
    	FrgSdrInst.Src1.uPa       = FGL_GET_BITFIELD(Instruction[0], 31:31);    	
    	FrgSdrInst.Src1.uRegType  = FGL_GET_BITFIELD(Instruction[1], 2:0);
    	FrgSdrInst.Src1.uAr       = FGL_GET_BITFIELD(Instruction[1], 3:3);
    	FrgSdrInst.Src1.uPn       = FGL_GET_BITFIELD(Instruction[1], 4:4);
     	FrgSdrInst.Src1.uP        = FGL_GET_BITFIELD(Instruction[1], 5:5);
     	FrgSdrInst.Src1.uModifier = FGL_GET_BITFIELD(Instruction[1], 7:6);
  	    FrgSdrInst.Src1.uSwizzle  = FGL_GET_BITFIELD(Instruction[1], 15:8);
// source 0
    	FrgSdrInst.Src0.uRegNum   = FGL_GET_BITFIELD(Instruction[1], 20:16);
    	FrgSdrInst.Src0.uExtNum   = FGL_GET_BITFIELD(Instruction[1], 23:21);
    	FrgSdrInst.Src0.uRegType  = FGL_GET_BITFIELD(Instruction[1], 26:24);
    	FrgSdrInst.Src0.uAr       = FGL_GET_BITFIELD(Instruction[1], 29:27);
    	FrgSdrInst.Src0.uModifier = FGL_GET_BITFIELD(Instruction[1], 31:30);
    	FrgSdrInst.Src0.uSwizzle  = FGL_GET_BITFIELD(Instruction[2], 7:0);
// destination
    	FrgSdrInst.Dest.uRegNum   = FGL_GET_BITFIELD(Instruction[2], 12:8);
    	FrgSdrInst.Dest.uRegType  = FGL_GET_BITFIELD(Instruction[2], 15:13);
    	FrgSdrInst.Dest.uA        = FGL_GET_BITFIELD(Instruction[2], 16:16);
    	FrgSdrInst.Dest.uModifier = FGL_GET_BITFIELD(Instruction[2], 18:17);
    	FrgSdrInst.Dest.uMask     = FGL_GET_BITFIELD(Instruction[2], 22:19);
    	FrgSdrInst.Dest.uTargetAddr = 0;
// opcode
    	FrgSdrInst.Opcode.uOpcode = FGL_GET_BITFIELD(Instruction[2], 28:23);
    	FrgSdrInst.Opcode.u3op    = FGL_GET_BITFIELD(Instruction[2], 29:29);

        unsigned int uRegNum = 0;

        // data processing instruction
        if(FrgSdrInst.Opcode.uOpcode < SHADER_INST_B)
        {
            strcpy(Assembly, " ");
            strcat(Assembly, InstructList[FrgSdrInst.Opcode.uOpcode].InstructName);
            //strcat(Assembly, tex2d[i].texName);
            
            if(FrgSdrInst.Dest.uModifier == 1)
                strcat(Assembly, "_sat");
            
            // destination
            strcat(Assembly, " ");
            
            //unsigned int ret = _DestUpdateMask(VtxSdrInst.Dest.uMask, &TempMask[0]);
            if(FrgSdrInst.Dest.uRegType != DESTIN_REGISTER_OUTPUT)
            {
                if(_DestUpdateMask(FrgSdrInst.Dest.uMask, &TempMask[0]) == 1)
                    sprintf(TempBuf, "%s%d", DestRegTypeName[FrgSdrInst.Dest.uRegType], FrgSdrInst.Dest.uRegNum);
                else
                    sprintf(TempBuf, "%s%d.%s", DestRegTypeName[FrgSdrInst.Dest.uRegType], FrgSdrInst.Dest.uRegNum, TempMask);
            } else
            {
                if(_DestUpdateMask(FrgSdrInst.Dest.uMask, &TempMask[0]) == 1)
                    sprintf(TempBuf, "%s", "oColor");
                else
                    sprintf(TempBuf, "%s.%s", "oColor", TempMask);
            }
            
            strcat(Assembly, TempBuf);
            strcat(Assembly, ", ");
            
            // source 0
            if(FrgSdrInst.Src0.uRegType == SOURCE_REGISTER_CFLOAT)
            {
                uRegNum = FrgSdrInst.Src0.uRegNum + (FrgSdrInst.Src0.uExtNum << 5);
                if(uRegNum > uMaxConstFloat) uMaxConstFloat = uRegNum;
            }
            else
                uRegNum = FrgSdrInst.Src0.uRegNum;
            
            if(FrgSdrInst.Src0.uRegType == SOURCE_REGISTER_CINTEGER)
                if(FrgSdrInst.Src0.uRegNum > uMaxConstInt) uMaxConstInt = FrgSdrInst.Src0.uRegNum;
            
            if(FrgSdrInst.Src0.uRegType == SOURCE_REGISTER_CBOOL)
                if(FrgSdrInst.Src0.uRegNum > uMaxConstBool) uMaxConstBool = FrgSdrInst.Src0.uRegNum;
                
            if(_SrcSwizzle(FrgSdrInst.Src0.uSwizzle, &TempMask[0]) == 1)
            {
                if(FrgSdrInst.Src0.uModifier == SOURCE_MODIFIER_NEGATE)
                    sprintf(TempBuf, "-%s%d", FragSrcRegTypeName[FrgSdrInst.Src0.uRegType], uRegNum);
	            else if(FrgSdrInst.Src0.uModifier == SOURCE_MODIFIER_ABSOLUTE)
	                sprintf(TempBuf, "|%s%d|", FragSrcRegTypeName[FrgSdrInst.Src0.uRegType], uRegNum);
	            else if(FrgSdrInst.Src0.uModifier == SOURCE_MODIFIER_ABSOLUTE_NEGATE)
	                sprintf(TempBuf, "-|%s%d|", FragSrcRegTypeName[FrgSdrInst.Src0.uRegType], uRegNum);
	            else 
                    sprintf(TempBuf, "%s%d", FragSrcRegTypeName[FrgSdrInst.Src0.uRegType], uRegNum);
            }
            else
            {
                if(FrgSdrInst.Src0.uModifier == SOURCE_MODIFIER_NEGATE)
                    sprintf(TempBuf, "-%s%d.%s", FragSrcRegTypeName[FrgSdrInst.Src0.uRegType], uRegNum, TempMask);
	            else if(FrgSdrInst.Src0.uModifier == SOURCE_MODIFIER_ABSOLUTE)
	                sprintf(TempBuf, "|%s%d.%s|", FragSrcRegTypeName[FrgSdrInst.Src0.uRegType], uRegNum, TempMask);
	            else if(FrgSdrInst.Src0.uModifier == SOURCE_MODIFIER_ABSOLUTE_NEGATE)
	                sprintf(TempBuf, "-|%s%d.%s|", FragSrcRegTypeName[FrgSdrInst.Src0.uRegType], uRegNum, TempMask);
	            else 
                    sprintf(TempBuf, "%s%d.%s", FragSrcRegTypeName[FrgSdrInst.Src0.uRegType], uRegNum, TempMask);
            }
            
            strcat(Assembly, TempBuf);
         
            if(InstructList[FrgSdrInst.Opcode.uOpcode].uOperandSize > 1)
            {
                strcat(Assembly, ", ");
                // source 1
                if(FrgSdrInst.Src1.uRegType == SOURCE_REGISTER_CFLOAT)
                {
                    uRegNum = FrgSdrInst.Src1.uRegNum + (FrgSdrInst.Src0.uExtNum << 5);
                    if(uRegNum > uMaxConstFloat) uMaxConstFloat = uRegNum;
                }
                else
                    uRegNum = FrgSdrInst.Src1.uRegNum;
                    
                if(FrgSdrInst.Src1.uRegType == SOURCE_REGISTER_CINTEGER)
                    if(FrgSdrInst.Src1.uRegNum > uMaxConstInt) uMaxConstInt = FrgSdrInst.Src1.uRegNum;
                
                if(FrgSdrInst.Src1.uRegType == SOURCE_REGISTER_CBOOL)
                    if(FrgSdrInst.Src1.uRegNum > uMaxConstBool) uMaxConstBool = FrgSdrInst.Src1.uRegNum;
                
                if(_SrcSwizzle(FrgSdrInst.Src1.uSwizzle, &TempMask[0]) == 1)
                {
                    if(FrgSdrInst.Src1.uModifier == SOURCE_MODIFIER_NEGATE)
                        sprintf(TempBuf, "-%s%d", FragSrcRegTypeName[FrgSdrInst.Src1.uRegType], uRegNum);
    	            else if(FrgSdrInst.Src1.uModifier == SOURCE_MODIFIER_ABSOLUTE)
    	                sprintf(TempBuf, "|%s%d|", FragSrcRegTypeName[FrgSdrInst.Src1.uRegType], uRegNum);
    	            else if(FrgSdrInst.Src1.uModifier == SOURCE_MODIFIER_ABSOLUTE_NEGATE)
    	                sprintf(TempBuf, "-|%s%d|", FragSrcRegTypeName[FrgSdrInst.Src1.uRegType], uRegNum);
    	            else 
                        sprintf(TempBuf, "%s%d", FragSrcRegTypeName[FrgSdrInst.Src1.uRegType], uRegNum);
                }
                else
                {
                    if(FrgSdrInst.Src1.uModifier == SOURCE_MODIFIER_NEGATE)
                        sprintf(TempBuf, "-%s%d.%s", FragSrcRegTypeName[FrgSdrInst.Src1.uRegType], uRegNum, TempMask);
    	            else if(FrgSdrInst.Src1.uModifier == SOURCE_MODIFIER_ABSOLUTE)
    	                sprintf(TempBuf, "|%s%d.%s|", FragSrcRegTypeName[FrgSdrInst.Src1.uRegType], uRegNum, TempMask);
    	            else if(FrgSdrInst.Src1.uModifier == SOURCE_MODIFIER_ABSOLUTE_NEGATE)
    	                sprintf(TempBuf, "-|%s%d.%s|", FragSrcRegTypeName[FrgSdrInst.Src1.uRegType], uRegNum, TempMask);
    	            else 
                        sprintf(TempBuf, "%s%d.%s", FragSrcRegTypeName[FrgSdrInst.Src1.uRegType], uRegNum, TempMask);
                }
                
                strcat(Assembly, TempBuf);
                
                // 3rd operand
                if(isThreeOperand)
                {
                    strcat(Assembly, ", ");
                    // source 2
                    if(FrgSdrInst.Src2.uRegType == SOURCE_REGISTER_CFLOAT)
                    {
                        uRegNum = FrgSdrInst.Src2.uRegNum + (FrgSdrInst.Src0.uExtNum << 5);
                        if(uRegNum > uMaxConstFloat) uMaxConstFloat = uRegNum;
                    }
                    else
                        uRegNum = FrgSdrInst.Src2.uRegNum;
                        
                    if(FrgSdrInst.Src2.uRegType == SOURCE_REGISTER_CINTEGER)
                        if(FrgSdrInst.Src2.uRegNum > uMaxConstInt) uMaxConstInt = FrgSdrInst.Src2.uRegNum;
                    
                    if(FrgSdrInst.Src2.uRegType == SOURCE_REGISTER_CBOOL)
                        if(FrgSdrInst.Src2.uRegNum > uMaxConstBool) uMaxConstBool = FrgSdrInst.Src2.uRegNum;
                    
                    if(_SrcSwizzle(FrgSdrInst.Src2.uSwizzle, &TempMask[0]) == 1)
                    {
                        if(FrgSdrInst.Src2.uModifier == SOURCE_MODIFIER_NEGATE)
                            sprintf(TempBuf, "-%s%d", FragSrcRegTypeName[FrgSdrInst.Src2.uRegType], uRegNum);
        	            else if(FrgSdrInst.Src2.uModifier == SOURCE_MODIFIER_ABSOLUTE)
        	                sprintf(TempBuf, "|%s%d|", FragSrcRegTypeName[FrgSdrInst.Src2.uRegType], uRegNum);
        	            else if(FrgSdrInst.Src2.uModifier == SOURCE_MODIFIER_ABSOLUTE_NEGATE)
        	                sprintf(TempBuf, "-|%s%d|", FragSrcRegTypeName[FrgSdrInst.Src2.uRegType], uRegNum);
        	            else 
                            sprintf(TempBuf, "%s%d", FragSrcRegTypeName[FrgSdrInst.Src2.uRegType], uRegNum);
                    }
                    else
                    {
                        if(FrgSdrInst.Src2.uModifier == SOURCE_MODIFIER_NEGATE)
                            sprintf(TempBuf, "-%s%d.%s", FragSrcRegTypeName[FrgSdrInst.Src2.uRegType], uRegNum, TempMask);
        	            else if(FrgSdrInst.Src2.uModifier == SOURCE_MODIFIER_ABSOLUTE)
        	                sprintf(TempBuf, "|%s%d.%s|", FragSrcRegTypeName[FrgSdrInst.Src2.uRegType], uRegNum, TempMask);
        	            else if(FrgSdrInst.Src2.uModifier == SOURCE_MODIFIER_ABSOLUTE_NEGATE)
        	                sprintf(TempBuf, "-|%s%d.%s|", FragSrcRegTypeName[FrgSdrInst.Src2.uRegType], uRegNum, TempMask);
        	            else 
                            sprintf(TempBuf, "%s%d.%s", FragSrcRegTypeName[FrgSdrInst.Src2.uRegType], uRegNum, TempMask);
                    }
                    
                    strcat(Assembly, TempBuf);
                    isThreeOperand = false;
                }
            }
            
            
            if(FrgSdrInst.Opcode.u3op)
            {
                isThreeOperand = true;
            }
        } 
        else /* flow control instruction*/
        {
        	FrgSdrInst.Dest.uRegNum   = 0;
        	FrgSdrInst.Dest.uRegType  = 0;
        	FrgSdrInst.Dest.uA        = 0;
        	FrgSdrInst.Dest.uModifier = 0;
        	FrgSdrInst.Dest.uMask     = 0;
            
            bool isMinusSign;
            unsigned int uTargetAddr = FGL_GET_BITFIELD(Instruction[2], 15:8);

            if(FGL_GET_BITFIELD(Instruction[2], 16:16) == 1)
                isMinusSign = true;
            else
                isMinusSign = false;
            
            //if(isMinusSign)
            //    FrgSdrInst.Dest.uTargetAddr = -uTargetAddr; // * (-1);
            if(isMinusSign)
                FrgSdrInst.Dest.uTargetAddr = (int)uTargetAddr * (-1);
            else
                FrgSdrInst.Dest.uTargetAddr = uTargetAddr;
            
            //strcpy(Assembly, InstructList[VtxSdrInst.Opcode.uOpcode].InstructName);
            strcpy(Assembly, " ");
            strcat(Assembly, InstructList[FrgSdrInst.Opcode.uOpcode].InstructName);

            
            // destination
            if(SHADER_INST_RET != (ShaderInstructSet)FrgSdrInst.Opcode.uOpcode)
            {
                sprintf(TempBuf, " %d", FrgSdrInst.Dest.uTargetAddr);
                strcat(Assembly, TempBuf);
            }
            
            // source 0
            if(InstructList[FrgSdrInst.Opcode.uOpcode].uOperandSize > 0)
            {
                strcat(Assembly, ", ");
                if(FrgSdrInst.Src0.uRegType == SOURCE_REGISTER_CFLOAT)
                {
                    uRegNum = FrgSdrInst.Src0.uRegNum + (FrgSdrInst.Src0.uExtNum << 5);
                    if(uRegNum > uMaxConstFloat) uMaxConstFloat = uRegNum;
                }
                else
                    uRegNum = FrgSdrInst.Src0.uRegNum;
                    
                if(FrgSdrInst.Src0.uRegType == SOURCE_REGISTER_CINTEGER)
                    if(FrgSdrInst.Src0.uRegNum > uMaxConstInt) uMaxConstInt = FrgSdrInst.Src0.uRegNum;
                
                if(FrgSdrInst.Src0.uRegType == SOURCE_REGISTER_CBOOL)
                    if(FrgSdrInst.Src0.uRegNum > uMaxConstBool) uMaxConstBool = FrgSdrInst.Src0.uRegNum;
            
                if(_SrcSwizzle(FrgSdrInst.Src0.uSwizzle, &TempMask[0]) == 1)
                    sprintf(TempBuf, "%s%d", FragSrcRegTypeName[FrgSdrInst.Src0.uRegType], uRegNum);
                else
                    sprintf(TempBuf, "%s%d.%s", FragSrcRegTypeName[FrgSdrInst.Src0.uRegType], uRegNum, TempMask);
                
                strcat(Assembly, TempBuf);
            }
        }

        UART_Printf("%s\n", Assembly);
    }
    
    
    if(uMaxConstFloat)
    {
        fglDasmConstFloatReg(false, uMaxConstFloat);        
    }
    
    if(uMaxConstInt)
    {
        fglDasmConstIntegerReg(false, uMaxConstFloat);        
    }

    if(uMaxConstBool)
    {
        fglDasmConstBoolReg(false, uMaxConstFloat);        
    }

    return ( uPCEnd - uPCStart);
}

unsigned int
fglDasmConstFloatReg(bool isVertexShader, unsigned int uSizeConstFloat)
{
    unsigned int *pInstMemAddr;

    if(isVertexShader)
        pInstMemAddr = (unsigned int *)FGVS_CFLOAT_SADDR;
    else
        pInstMemAddr = (unsigned int *)FGPS_CFLOAT_SADDR;
    
    float ConstFloat[4] = { 0.f, };
    
    UART_Printf("\n\n");
    UART_Printf(">The constant floating-point numbers\n");
    
    for(int i = 0; i < (uSizeConstFloat + 1); i++)
    {
         ConstFloat[0] = READREGF(pInstMemAddr++);
         ConstFloat[1] = READREGF(pInstMemAddr++);
         ConstFloat[2] = READREGF(pInstMemAddr++);
         ConstFloat[3] = READREGF(pInstMemAddr++);
         
         UART_Printf(" def c%d, %f, %f, %f, %f\n", i, ConstFloat[0], ConstFloat[1], ConstFloat[2], ConstFloat[3]);
    }
    
    UART_Printf("\n\n");

    return 0;
}

unsigned int
fglDasmConstIntegerReg(bool isVertexShader, unsigned int uSizeConstInteger)
{
    unsigned int *pInstMemAddr;
    
    if(isVertexShader)
        pInstMemAddr = (unsigned int *)FGVS_CINT_SADDR;
    else
        pInstMemAddr = (unsigned int *)FGPS_CINT_SADDR;
    
    int ConstFloat[4] = { 0, };
    
    UART_Printf("\n\n");
    UART_Printf(">The constant integer numbers\n");
    
    for(int i = 0; i < (uSizeConstInteger + 1); i++)
    {
         ConstFloat[0] = READREG(pInstMemAddr++);
         ConstFloat[1] = READREG(pInstMemAddr++);
         ConstFloat[2] = READREG(pInstMemAddr++);
         ConstFloat[3] = READREG(pInstMemAddr++);
         
         UART_Printf(" def i%d, %d, %d, %d, %d\n", i, ConstFloat[0], ConstFloat[1], ConstFloat[2], ConstFloat[3]);
    }
    
    UART_Printf("\n\n");

    return 0;
}

unsigned int
fglDasmConstBoolReg(bool isVertexShader, unsigned int uSizeConstBool)
{
    unsigned int uConstBool;
    
    if(isVertexShader)
        uConstBool = READREG(FGVS_CBOOL_SADDR);
    else
        uConstBool = READREG(FGPS_CBOOL_SADDR);
    
    UART_Printf("\n\n");
    UART_Printf(">The constant boolean numbers\n");
    
    for(int i = 0; i < 16; i++)
    {
        UART_Printf(" def b%d, %d\n", i, (uConstBool & (i+1))? 1:0);
    }
    
    UART_Printf("\n\n");

    return 0;
}


//#undef FGL_BITFIELD_MASK(fields)
//#undef FGL_GET_BITFIELD(flags, fields)


unsigned int
_DestUpdateMask(unsigned int uMaskVal, char *StrBuf)
{
    unsigned int i = 0;

    if(uMaskVal == 0xF)
    {
        StrBuf[i++] = '\0';
        return i;
    }
    
    if(uMaskVal & 0x1)
        StrBuf[i++] = 'x';
    
    if(uMaskVal & 0x2)
        StrBuf[i++] = 'y';
    
    if(uMaskVal & 0x4)
        StrBuf[i++] = 'z';
    
    if(uMaskVal & 0x8)
        StrBuf[i++] = 'w';
    
    StrBuf[i++] = '\0';
    
    return i;
}


unsigned int
_SrcSwizzle(unsigned int uSwizzleVal, char *StrBuf)
{
    unsigned int i = 0;
    
    if(uSwizzleVal == 0xE4)
    {
        StrBuf[i++] = '\0';
        return i;
    } else if(uSwizzleVal == 0x0)
    {
        StrBuf[i++] = 'x';
        StrBuf[i++] = '\0';
        return i;
    } else if(uSwizzleVal == 0x55)
    {
        StrBuf[i++] = 'y';
        StrBuf[i++] = '\0';
        return i;
    } else if(uSwizzleVal == 0xAA)
    {
        StrBuf[i++] = 'z';
        StrBuf[i++] = '\0';
        return i;
    } else if(uSwizzleVal == 0xFF)
    {
        StrBuf[i++] = 'w';
        StrBuf[i++] = '\0';
        return i;
    }
    
    for(int j = 0; j < 4; j++)
    {
        unsigned int uChannel = (uSwizzleVal >> (2 * j)) & 0x3;
        switch(uChannel)
        {
        case 0:
            StrBuf[i++] = 'x';
            break;
        case 1:
            StrBuf[i++] = 'y';
            break;
        case 2:
            StrBuf[i++] = 'z';
            break;
        case 3:
            StrBuf[i++] = 'w';
            break;
        }
    }
        
    StrBuf[i++] = '\0';
    
    return i;
}
