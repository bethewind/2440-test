#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "def.h"
#include "option.h"
#include "2440addr.h"
#include "2440lib.h"
#include "ctype.h"

#include "userlib.h"

extern void Uart_GetString(char *string);


/************************** Manual register setting *****************************/

char sbuf[80], scmd1[20], scmd2[20], scmd3[20], scmd4[20];
//char *sbuf, *scmd1, *scmd2, *scmd3, *scmd4;
U32 cmd2, cmd3, cmd4;
char pre_scmd[10][80];

const void *command_set[][2] = {
	(void *)Memory_Dump,			"dump",
	(void *)Multimem_Write,			"mw",
	(void *)Halfword_Read,			"hr",
	(void *)Halfword_Write,			"hw",
	(void *)Halfword_Write_Multi,	"hwm",
	(void *)Word_Write,				"w",
	(void *)Word_Write_Or,			"ow",
	(void *)Word_Read,				"r",
	(void *)Fill_Memory,			"fi",
	// Add the test function. add manset_Usage too.
	0,0,0
};

void Manset_Usage(void)
{
//	int i;

	Uart_Printf("dump:memory dump(ex:dump addr length)\n");
	Uart_Printf("mw:multiple write.\n");
	Uart_Printf("hr:halfword read.\n");
	Uart_Printf("hw:halfword write.\n");
	Uart_Printf("hwm:halfword write multi.\n");
	Uart_Printf("w:word write.\n");
	Uart_Printf("ow:word write with OR logic.\n");
	Uart_Printf("r:word read.\n");
	Uart_Printf("fi:memory fill(ex:fi addr length value).\n");
	Uart_Printf("P: execute previous command.\n");

}


void Manual_Register_Set(void)
{
	U32 i=0, j=0;

	Uart_Printf("Manual regiter set. (q)uit.\n");
	Uart_Printf("Usage(?): >> <command> <opr1> (<opr2>)\n");
	Manset_Usage();

	while (1) {
		Uart_Printf(">> ");
		Uart_GetString(sbuf);

		if(*sbuf==NULL) continue;
		else if(*sbuf=='P') Execute_Precmd();

		else if(*sbuf==ESC_KEY) break;
		else if(*sbuf==ESC_KEY) {
			Manset_Usage();
		} else {
			Get_Cmd(sbuf);

			for(i=0; (int)(command_set[i][0])!=NULL; i++) {
				if(!strcmp((command_set[i][1]), scmd1)) {
					((void (*)(void)) (command_set[i][0]))();
					for(j=MAX_PRECMD_SIZE-1; j>0; j--) {
						//if((int)pre_scmd[j][0]==NULL) continue;
						strcpy(pre_scmd[j], pre_scmd[j-1]);
					}

					strcpy(pre_scmd[j], sbuf);
					break;
				}
			}
		} // end of if(*sbuf==NULL)...

	}

}




void Get_Cmd(char incmd[])
{
	int cmd_level=0, i, j, overlap=0;

	scmd1[0]='\0';
	scmd2[0]='\0';
	scmd3[0]='\0';
	scmd4[0]='\0';

	for(j=0, i=0; incmd[i]!=NULL; i++) {
		if(incmd[i] != ' ' && incmd[i] != 0x09 && incmd[i] != ',') {
			overlap=0;
			switch(cmd_level) {
				case 0:
					scmd1[j] = incmd[i];
					scmd1[j+1]='\0';
				//if(DEBUG_LEVEL>0) Uart_Printf("(%c)", scmd1[j]);
				break;
				case 1:
					scmd2[j] = incmd[i];
					scmd2[j+1]='\0';
				//if(DEBUG_LEVEL>0) Uart_Printf("%c", scmd2[j]);
				break;
				case 2:
					scmd3[j] = incmd[i];
					scmd3[j+1]='\0';
				//if(DEBUG_LEVEL>0) Uart_Printf("[%c]", scmd3[j]);
				break;
				case 3:
					scmd4[j] = incmd[i];
					scmd4[j+1]='\0';
				//if(DEBUG_LEVEL>0) Uart_Printf("[%c]", cmd4[j]);
				break;

			}
			j++;
		} else {
			j=0;
			if(overlap==0) {
				cmd_level++;
			}
			overlap=1;
		}
	}

	// Make argument.
	cmd2 = Get_Num(scmd2);		// atox(scmd2);
	cmd3 = Get_Num(scmd3);		// : Character to hexa(decimal)
	cmd4 = Get_Num(scmd4);		// : Character to hexa(decimal)

	if(cmd2==0xffffffff) cmd2=0;
	if(cmd3==0xffffffff) cmd3=0;
	if(cmd4==0xffffffff) cmd4=0;
}

U32 Get_Num(char *string)
{
    int base=10;
    int minus=0;
    int lastIndex;
    int result=0;
    int i;

    if(string[0]=='-')
    {
	minus=1;
	string++;
    }

    if(string[0]=='0' && (string[1]=='x' || string[1]=='X'))
    {
	base=16;
	string+=2;
    }

    lastIndex=strlen(string)-1;
    if(lastIndex < 0) 
	  return -1;

    if( string[lastIndex]=='h' || string[lastIndex]=='H' )
    {
	base=16;
	string[lastIndex]=0;
	lastIndex--;
    }

    if(base==10)
    {
	result=atoi(string);
	result=minus ? (-1*result):result;
    }
    else
    {
	for(i=0;i<=lastIndex;i++)
	{
	    if(isalpha(string[i]))
	    {
		if(isupper(string[i]))
		    result=(result<<4)+string[i]-'A'+10;
		else
		    result=(result<<4)+string[i]-'a'+10;
	    }
	    else
	    {
		result=(result<<4)+string[i]-'0';
	    }
	}
	result=minus ? (-1*result):result;
    }
    return result;
}



void Execute_Precmd(void)
{
	int cmd_sel=0;//, exec_cmd=1; nt=0,  ci=0, 
//	char ch;
    int i;

	for(i=0; i<MAX_PRECMD_SIZE && pre_scmd[i][0]!=NULL; i++) {
		Uart_Printf("%d: [%s]\n", i, pre_scmd[i]);
	}
	if(i==0) return;
	Uart_Printf("select num(-1 to exit)? ");
	cmd_sel = Uart_GetIntNum();
	
	if(cmd_sel != -1) {
		Uart_Printf("\n>> %s", pre_scmd[cmd_sel]);
		Uart_Printf("\n");
		Get_Cmd(pre_scmd[cmd_sel]);		// Make sbuf -> scmd...

		for(i=0; (int)(command_set[i][0])!=NULL; i++) {
			if(!strcmp((command_set[i][1]), scmd1)) {
				((void (*)(void)) (command_set[i][0]))();
				break;
			}
		}
	}

}

void Uart_AppendString(char *string)
{
    char *string2 = string;
    char c;

	string = (string + strlen(string));

    while((c = Uart_Getch())!='\r') {
        if(c=='\b') {
            if( (int)string2 < (int)string ) {
                Uart_Printf("\b \b");
                string--;
            }
        } else {
            *string++ = c;
            Uart_SendByte(c);
        }
    }
    *string='\0';
    Uart_SendByte('\n');
}

//================================================================
void Memory_Dump(void)
{
	int i;

	Uart_Printf("memory dump\n");
	for(i=0; i<cmd3; i+=4) {
		Uart_Printf("  %08xh: %08xh\n", (cmd2+i), *(U32 *)(cmd2+i));
		if(Uart_GetKey()!=NULL) {
 			Uart_Printf("Break at %x\n", cmd2+i);
 			break;
 		}
	}
}

void Multimem_Write(void)
{
	U32 i;
	S32 itmp;
	char tmps[20];

	if(cmd2%4) cmd2=(cmd2/4)*4;
	Uart_Printf("Multiple memory write (q)uit.\n");

	for(i=0; ; ) {
		Uart_Printf("  %08xh ? ", cmd2+i);
		Uart_GetString(tmps);
		if(*tmps=='q') break;

		itmp = Get_Num(tmps);
 		if(itmp!=-1) *(U32 *)(cmd2+i) = (U32)itmp;

		i+=4;
	}
}

void Fill_Memory(void)
{
	U32 i;//, itmp;
//	char tmps[20];

	if(cmd2%4) cmd2=(cmd2/4)*4;
	Uart_Printf("Fill memory[%08xh - %08xh with %xh].\n", cmd2, cmd2 + cmd3, cmd4);

	for(i=0; i<cmd3; i+=4) {
 		*(U32 *)(cmd2+i) = cmd4;
 		if(Uart_GetKey()!=NULL) {
 			Uart_Printf("Break at %x\n", cmd2+i);
 			break;
 		}
	}

}

void Halfword_Read(void)
{
	Uart_Printf("  [%08xh:%04xh]\n", cmd2, *(U16 *)cmd2);
}

void Halfword_Write(void)
{
	*(U16 *)cmd2 = (U16)cmd3;
	Uart_Printf("  [%08xh<-%04xh]\n", cmd2, (U16)cmd3);
}



void Halfword_Write_Multi(void)
{
//	char *string;
	short int  indata;

	Uart_Printf("  Addr:%08xh\n", cmd2);

	while(1) {
		Uart_Printf("  data: ");
		indata = Uart_GetIntNum();
		if(indata==-1) break;
		*(U16 *)cmd2 = indata;
		Uart_Printf("  [%08xh<-%04xh]\n", cmd2, indata);
	}

}

void Word_Write(void)
{
	if(cmd2%4) cmd2=(cmd2/4)*4;
	*(U32 *)cmd2 = (U32)cmd3;
	Uart_Printf("  [%08xh<-%08xh]\n", cmd2, cmd3);

}

void Word_Write_Or(void)
{
	if(cmd2%4) cmd2=(cmd2/4)*4;
	*(U32 *)cmd2 |= (U32)cmd3;
	Uart_Printf("  [%08xh |= %08xh]\n", cmd2, cmd3);
}

void Word_Read(void)
{
	if(cmd2%4) cmd2=(cmd2/4)*4;
	Uart_Printf("  [%08xh:%08xh]\n", cmd2, *(U32 *)cmd2);
}

void user_delay(unsigned int cnt)
{
	int i, j;

	for(j=0; j<cnt; j++)
		for(i=0; i<500; i++);
}

/***************************************************************************/

/**************************** Clock calculation ********************************/
// Clock calculation. Current clock information calc.
// Register value : MPLLCON(Mdiv, Pdiv, Sdiv), CLKDIVN(Hdivn, Pdivn).
// Hclk = Fclk/Hclk_Ratio, Pclk = Fclk/Hclk_Ratio/Pclk_Ratio.
// Refresh count(Ref_Cnt) = 2048+1-(Hclk*7.8)
void Calc_Clock(int print_msg);
U32 Mdiv, Pdiv, Sdiv, Fclk, Hclk, Pclk, Hdivn, Pdivn, Hclk_Ratio, Pclk_Ratio, Ref_Cnt;

// Register value : Mdiv, Pdiv, Sdiv, Hdivn, Pdivn.
// Hclk = Fclk/Hclk_Ratio, Pclk = Fclk/Hclk_Ratio/Pclk_Ratio.

#define	LCDCLK	(5000000)	//5MHz
void Calc_Clock(int print_msg)
{
	unsigned int mpll_val, sval, lcd_clk_val, tmpi;

	// Get MPS value from register.
	mpll_val = rMPLLCON;
	Mdiv = (mpll_val&(0xff<<12))>>12;
	Pdiv = (mpll_val&(0x3f<<4))>>4;
	Sdiv = (mpll_val&0x3);

	// Get HDIVN, PDIVN value from register.
	Hdivn = ((rCLKDIVN&0x6)>>1);
	Pdivn = (rCLKDIVN&1);

		
	// Get the HCLK and PCLK ratio
	switch(Hdivn) {
		case 0: Hclk_Ratio=1; break;
		case 1: Hclk_Ratio=2; break;
		case 2: Hclk_Ratio=4; break;
		case 3: Hclk_Ratio=3; break;
	}
	switch(Pdivn) {
		case 0: Pclk_Ratio=1; break;
		case 1: Pclk_Ratio=2; break;
	}

	tmpi = rCAMDIVN&(3<<8);
	if(tmpi!=0) {
		if(Hclk_Ratio==3 || Hclk_Ratio==4)
		Hclk_Ratio *= 2;
	}

	// Get sval for calc Fout.
	//sval=(int)pow(2,Sdiv);
	switch(Sdiv) {
		case 0: sval = 1; break;
		case 1: sval = 2; break;
		case 2: sval = 4; break;
		case 3: sval = 8; break;
		default: sval = 0; break;
	}

	// Calc Fclk, Hclk, Pclk
	#if CPU2440A==TRUE
	Fclk=(U32)( (((float)Mdiv+8)*FIN*2.0)/( ((float)Pdiv+2)*(float)sval) );	// pll2188x.
	#else
	Fclk=(U32)( (((float)Mdiv+8)*FIN)/( ((float)Pdiv+2)*(float)sval) );		// pll2115x
	#endif
	
	Hclk =(U32)( ((float)Fclk/(float)Hclk_Ratio) );
	Pclk = (U32)( (((float)Fclk/(float)Hclk_Ratio)/(float)Pclk_Ratio));

	// Calc refresh count.
	Ref_Cnt = 2048+1-(U32)( (float)( ((float)Hclk*7.8)/MEGA) );

	Uart_TxEmpty(1);
	Uart_Init(Pclk, 115200);
	
	
	if(print_msg!=0) {
		Uart_Printf("MPLLVal [M:%xh(%d),P:%xh,S:%xh]\n", Mdiv, Mdiv, Pdiv, Sdiv);
		Uart_Printf("F/H/PCLK=[1:%d:%d=", Hclk_Ratio, Hclk_Ratio*Pclk_Ratio);
		//Uart_Printf("%4.3f:%4.3f:%4.3f MHz]\n", (float)Fclk/MEGA, (float)Hclk/MEGA, (float)Pclk/MEGA);
		Uart_Printf("%4.1f:%4.1f:%4.1f MHz]\n", (float)Fclk/MEGA, (float)Hclk/MEGA, (float)Pclk/MEGA);
		Uart_Printf("Refcnt:%d\n", Ref_Cnt);
	}

	lcd_clk_val = (Hclk/(2.0*LCDCLK)+0.5)-1;
	rLCDCON1 = (rLCDCON1 & ~(0x3ff<<8)) | lcd_clk_val<<8;
	//Uart_Printf("CLKVAL:%d\n", lcd_clk_val);

}
/***************************************************************************/


//*************************[ Ticker ]********************************
unsigned int Init_Ticker(int divider)  //0:16us,1:32us 2:64us 3:128us
{
	unsigned int Div_us;
	
    rWTCON = ((Pclk/1000000-1)<<8)|(divider<<3);  //Watch-dog timer control register
    rWTDAT = 0xffff;  //Watch-dog timer data register
    rWTCNT = 0xffff;  //Watch-dog count register

	pISR_WDT_AC97=(int)Ticker_int;

	ClearPending(BIT_WDT_AC97);	
	rSUBSRCPND=(BIT_SUB_WDT);

	rINTMSK&=~(BIT_WDT_AC97);
	rINTSUBMSK&=~(BIT_SUB_WDT);
	
	switch(divider) {
		case 0: Div_us=16; break;
		case 1: Div_us=32; break;
		case 2: Div_us=64; break;
		case 3: Div_us=128; break;
		default: Div_us=0; break;
	}

	return Div_us;
}



//=================================================================
int Ticker_Stop(void)
{
    rWTCON = ((Pclk/1000000-1)<<8);
    return (0xffff - rWTCNT);
}

static void __irq Ticker_int(void)
{
	rINTSUBMSK|=(BIT_SUB_WDT);		// Just for the safety

	rSUBSRCPND=(BIT_SUB_WDT);		// Clear Sub int pending
	ClearPending(BIT_WDT_AC97);		// Clear master pending
	
	Uart_Printf("Error: Ticker timer reaches 0.\n");

}

