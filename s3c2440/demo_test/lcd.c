//====================================================================
// File Name : LCD.c
// Function  : S3C2440 LCD test code
// Program   : 
// Date      : Apr. 21, 2003
// Version   : 0.0
// History
// -2410
//   0.0 : Programming start  ???
// -2440
//   R0.0 (20030421): Modified for 2440. -> Junon
//====================================================================

#include <string.h>
#include "def.h"
#include "2440addr.h"
#include "2440lib.h"
#include "2440slib.h"
#include "lcdlib.h"
#include "glib.h"
#include "lcd.h"
#include "DEMO256.h"


void Lcd_Port_Init(void);
void Lcd_Port_Return(void);
void Lcd_Palette1Bit_Init(void);
void Lcd_Palette8Bit_Init(void);
void __irq Lcd_Int_Frame(void);
void __irq Lcd_Int_Fifo(void);
void __irq Lcd_Int_Fifo_640480(void);
void Select_Tcon(void);

void Test_Lcd_Palette_Ram(void);
void Test_Lcd_Underrun(void);
void Test_Lcd_Tft_16Bit_800600(void);
void Test_Lcd_Tft_16Bit_800600_On(void);


unsigned save_rGPCUP,save_rGPCDAT,save_rGPCCON;
unsigned save_rGPDUP,save_rGPDDAT,save_rGPDCON; 
unsigned lcd_count;


void * func_lcd_test[][2]=
{	
//									    "0123456789012345" max 15자 로한정하여 comment하세요.
//LCD 	
	(void *)Test_Lcd_Palette_Ram, 					"LCD Palette RAM",
	
	(void *)Test_Lcd_Stn_1Bit,							"STN 1Bit       ",
	(void *)Test_Lcd_Stn_2Bit,							"STN 2Bit       ",
	(void *)Test_Lcd_Stn_4Bit,							"STN 4Bit       ", 	 

	(void *)Test_Lcd_Cstn_8Bit, 							"CSTN 8Bit      ",
	(void *)Test_Lcd_Cstn_8Bit_On,					"CSTN 8Bit On   ", 	 
	(void *)Test_Lcd_Cstn_12Bit,						"CSTN 12Bit     ",
	(void *)Test_Lcd_Cstn_16Bit,						"CSTN 16Bit     ",

	(void *)Test_Lcd_Tft_8Bit_240320, 			"TFT240320 8Bit ",
	(void *)Test_Lcd_Tft_8Bit_240320_On,		"TFT240320 8 On ", 	 
	(void *)Test_Lcd_Tft_16Bit_240320,			"TFT240320 16Bit",
	(void *)Test_Lcd_Tft_8Bit_240320_Bmp, 	"TFT240320 Bmp  ",
	(void *)Test_Lcd_Tft_8Bit_240320_IntFr, "TFT240320 FrINT",
	(void *)Test_Lcd_Tft_8Bit_240320_IntFi, "TFT240320 FiINT",

	(void *)Test_Lcd_Tft_1Bit_640480, 			"TFT640480 1Bit ",
	(void *)Test_Lcd_Tft_8Bit_640480, 			"TFT640480 8Bit ",
	(void *)Test_Lcd_Tft_16Bit_640480,			"TFT640480 16Bit",

	(void *)Test_Lcd_Tft_8Bit_640480_Bswp,	"TFT640480 BSWP ",
	(void *)Test_Lcd_Tft_8Bit_640480_Palette, "TFT640480 Palet",  
	(void *)Test_Lcd_Tft_16Bit_640480_Hwswp,"TFT640480 HWSWP",

	(void *)Test_Lcd_Tft_1Bit_800600, 			"TFT800600 1Bit ",
	(void *)Test_Lcd_Tft_8Bit_800600, 			"TFT800600 8Bit ",
	(void *)Test_Lcd_Tft_16Bit_800600, 			"TFT800600 16Bit",
	
	(void *)Test_Lcd_Underrun,							"LCD Underrun   ",

	0,0
};

void Lcd_Test(void)
{
	int i;
	while(1)
	{
		i=0;
		Uart_Printf("\n\n");
			
		Uart_Printf("\n======  LCD Test program start ======\n");
		Uart_Printf("1 :LCD Palette RAM\n");
		Uart_Printf("2 :STN 1Bit       \n");
		Uart_Printf("3 :STN 2Bit\n");
		Uart_Printf("4 :STN 3Bit\n");
		Uart_Printf("\n");
		Uart_Printf("5 :CSTN 8Bit\n");
		Uart_Printf("6 :CSTN 8Bit on\n");
		Uart_Printf("7 :CSTN 12Bit\n");
		Uart_Printf("8 :CSTN 16Bit\n");
		Uart_Printf("\n");
		Uart_Printf("9 :TFT240320 8Bit\n");
		Uart_Printf("10:TFT240320 8 On\n");
		Uart_Printf("11:TFT240320 16Bit\n");
		Uart_Printf("12:TFT240320 Bmp\n");
		Uart_Printf("13:TFT240320 FrINT\n");
		Uart_Printf("14:TFT240320 FiINT\n");
		Uart_Printf("\n");
		Uart_Printf("15:TFT640480 1Bit\n");
		Uart_Printf("16:TFT640480 8Bit\n");
		Uart_Printf("17:TFT640480 16Bit\n");
		Uart_Printf("\n");
		Uart_Printf("18:TFT640480 BSWP\n");
		Uart_Printf("19:TFT640480 Palet\n");
		Uart_Printf("20:TFT640480 HWSWP\n");
		Uart_Printf("\n");
		Uart_Printf("21:TFT800600 1Bit\n");
		Uart_Printf("22:TFT800600 8Bit\n");
		Uart_Printf("23:TFT800600 16Bit\n");
		Uart_Printf("\n");
		Uart_Printf("24:LCD Underrun\n");
		Uart_Printf("\n");

		Uart_Printf("\nPress only Enter key to exit : ");
		i = Uart_GetIntNum();
		if(i==-1) break;		// return.
//		if(i>=0 && (i<((sizeof(func_lcd_test)-1)/8)) )	// select and execute...
//			( (void (*)(void)) (func_lcd_test[i][0]) )();
        switch(i)
		{
		    case 1:	  Test_Lcd_Palette_Ram();
			          break;
		    case 2:	  Test_Lcd_Stn_1Bit();
			          break;
		    case 3:	  Test_Lcd_Stn_2Bit();
			          break;
		    case 4:	  Test_Lcd_Stn_4Bit();
			          break;
		    case 5:	  Test_Lcd_Cstn_8Bit(); 
			          break;
		    case 6:	  Test_Lcd_Cstn_8Bit_On();
			          break;
		    case 7:	  Test_Lcd_Cstn_12Bit();
			          break;
		    case 8:	  Test_Lcd_Cstn_16Bit();
			          break;
		    case 9:	  Test_Lcd_Tft_8Bit_240320(); 
			          break;
		    case 10:	  Test_Lcd_Tft_8Bit_240320_On();
			          break;
		    case 11:	  Test_Lcd_Tft_16Bit_240320();
			          break;
		    case 12:	  Test_Lcd_Tft_8Bit_240320_Bmp();
			          break;
		    case 13:	  Test_Lcd_Tft_8Bit_240320_IntFr();
			          break;
		    case 14:	  Test_Lcd_Tft_8Bit_240320_IntFi();
			          break;
		    case 15:	  Test_Lcd_Tft_1Bit_640480(); 
			          break;
		    case 16:	  Test_Lcd_Tft_8Bit_640480(); 
			          break;
		    case 17:	  Test_Lcd_Tft_16Bit_640480();
			          break;
		    case 18:	  Test_Lcd_Tft_8Bit_640480_Bswp();
			          break;
		    case 19:	  Test_Lcd_Tft_8Bit_640480_Palette(); 
			          break;
		    case 20:	  Test_Lcd_Tft_16Bit_640480_Hwswp();
			          break;
		    case 21:	  Test_Lcd_Tft_1Bit_800600();
			          break;
		    case 22:	  Test_Lcd_Tft_8Bit_800600();
			          break;
		    case 23:	  Test_Lcd_Tft_16Bit_800600();
			          break;
		    case 24:	  Test_Lcd_Underrun();
			          break;
		    default:
			          break;
		}
	}
	
	Uart_Printf("\n====== LCD Test program end ======\n");
}


void Lcd_Port_Init(void)
{
    save_rGPCCON=rGPCCON;
    save_rGPCDAT=rGPCDAT;
    save_rGPCUP=rGPCUP;
    save_rGPDCON=rGPDCON;
    save_rGPDDAT=rGPDDAT;
    save_rGPDUP=rGPDUP;
    rGPCUP=0xffffffff; // Disable Pull-up register
    rGPCCON=0xaaaaaaaa; //Initialize VD[7:0],LCDVF[2:0],VM,VFRAME,VLINE,VCLK,LEND 
    rGPDUP=0xffffffff; // Disable Pull-up register
    rGPDCON=0xaaaaaaaa; //Initialize VD[23:8]
    Uart_Printf("Initializing GPIO ports..........\n");
}

void Lcd_Port_Return(void)
{
    rGPCCON=save_rGPCCON;
    rGPCDAT=save_rGPCDAT;
    rGPCUP=save_rGPCUP;
    rGPDCON=save_rGPDCON;
    rGPDDAT=save_rGPDDAT;
    rGPDUP=save_rGPDUP;
}

void Lcd_Palette1Bit_Init(void)
{
    U32 *palette;
    palette=(U32 *)PALETTE;
    *palette++=0x0; // Black
    *palette=0xffff; // White
}

void Lcd_Palette8Bit_Init(void)
{
    int i;	
    U32 *palette;

	palette=(U32 *)PALETTE;
	if (rLCDCON5&(1<<11)) // if 5:6:5 format
		for(i=0;i<256;i++) *palette++=DEMO256pal[i];
	else // if 5:5:5:1 format
		for(i=0;i<256;i++) *palette++=(DEMO256pal[i]&0xffc1)|((DEMO256pal[i]&0x1f)<<1);	
}

void Lcd_Start(U32 mode)
{
// LCD initialize
	Lcd_Port_Init();
	Lcd_Init(mode);
	Glib_Init(mode);
	Select_Tcon();
	Lcd_PowerEnable(0, 1);
	Lcd_EnvidOnOff(1);
	Glib_ClearScr(0, mode);
}

//Just interrupt test..
void __irq Lcd_Int_Frame(void)
{
    rLCDINTMSK|=3; // Mask LCD SUB Interrupt

    // GPG4 is LCD_PWREN 
    rGPGDAT&=(~(1<<4)); // GPG4=Low
    Delay(50); // 5ms
    rGPGDAT|=(1<<4); //GPG4=High

    rLCDSRCPND=2; // Clear LCD SUB Interrupt source pending
    rLCDINTPND=2; // Clear LCD SUB Interrupt pending
    rLCDINTMSK&=(~(2)); // Unmask LCD FRAME Interrupt
    ClearPending(BIT_LCD);
}


void __irq Lcd_Int_Fifo(void)
{
    rLCDINTMSK|=3; // Mask LCD SUB Interrupt

    if((lcd_count%20)==0) Uart_Printf("\n");
    Uart_Printf(".");
    lcd_count++;

    rLCDSRCPND=1; // Clear LCD SUB Interrupt source pending
    rLCDINTPND=1; // Clear LCD SUB Interrupt pending
    rLCDINTMSK&=(~(1)); // Unmask LCD FRAME Interrupt
    ClearPending(BIT_LCD);
}

void Test_Lcd_Stn_1Bit(void)
{
    int i,j;
    Lcd_Port_Init();
    Lcd_Init(MODE_STN_1BIT);
    Glib_Init(MODE_STN_1BIT);
    Lcd_EnvidOnOff(1); // Enable ENVID Bit
    Uart_Printf("[STN Mono(1bit/1pixel) LCD Test]\n");              

    Glib_ClearScr(0, MODE_STN_1BIT);
    for(j=0;j<LCD_YSIZE_STN;j+=16)
		for(i=0;i<LCD_XSIZE_STN;i+=16)
		    Glib_FilledRectangle(i,j,i+15,j+15,((j+i)/16)%2);
    Uart_Printf("STN Mono test 1. Press any key!\n");
    Uart_Getch();  	

    Glib_ClearScr(0, MODE_STN_1BIT);
    Glib_FilledRectangle(160,0,319,239,1);    //Half Screen
    Uart_Printf("STN Mono test 2. Press any key!\n");
    Uart_Getch();  	

    Glib_ClearScr(0, MODE_STN_1BIT); 
    Glib_Rectangle(0,0,319,239,1);   // #0
    Glib_Line(0,0,319,239,1);        // 00
    Glib_Line(0,239,319,0,1);

    Glib_Rectangle(0+320,0,319+320,239,1);   // 0#
    Glib_Line(0+320,0,319+320,239,1);        // 00
    Glib_Line(0+320,239,319+320,0,1);
    Glib_FilledRectangle(50+320,50,269+320,189,1);

    Glib_Rectangle(0,0+240,319,239+240,1);   // 00
    Glib_Line(0,0+240,319,239+240,1);        // #0
    Glib_Line(0,239+240,319,0+240,1);
    Glib_FilledRectangle(50,50+240,269,189+240,1);
    
    Glib_Rectangle(0+320,0+240,319+320,239+240,1);   // 00	
    Glib_Line(0+320,0+240,319+320,239+240,1);        // 0#
    Glib_Line(0+320,239+240,319+320,0+240,1);
    Glib_Rectangle(50+320,50+240,269+320,189+240,1);

    Uart_Printf("Virtual Screen Test(STN Mono). Press any key[ijkm\\r]!\n");
    MoveViewPort(MODE_STN_1BIT);
    Lcd_MoveViewPort(0,0,MODE_STN_1BIT);
    Lcd_EnvidOnOff(0); // Disable ENVID Bit
    Lcd_Port_Return();
}


void Test_Lcd_Stn_2Bit(void)
{
    int i,j,k;
    Lcd_Port_Init();
    Lcd_Init(MODE_STN_2BIT);
    Glib_Init(MODE_STN_2BIT);
    Lcd_EnvidOnOff(1); // Enable ENVID Bit
    Uart_Printf("[STN 4 gray(2bit/1pixel) LCD Test]\n");

    Glib_ClearScr(0, MODE_STN_2BIT);
    j=0;
    for(i=0;i<320;i+=80)
        Glib_FilledRectangle(0+i,0,79+i,239,j++);
    Uart_Printf("4 gray mode test 1. Press any key!\n");
    Uart_Getch();  	

    Glib_ClearScr(0, MODE_STN_2BIT);
    j=0;
    for(i=0;i<320;i+=80)
    {
    	Glib_FilledRectangle(0+i,0,79+i,119,j);
    	Glib_FilledRectangle(0+i,120,79+i,239,3-j);
    	j++;
    }
    Uart_Printf("4 gray mode test 2. Press any key!\n");
    Uart_Getch();  	

    Glib_ClearScr(0, MODE_STN_2BIT);
    j=0;
    for(i=0;i<240;i+=60)
    {
    	Glib_FilledRectangle(i,i,i+59,i+59,j);
    	j++;
    }
    Uart_Printf("4 gray mode test 3. Press any key!\n");
    Uart_Getch();  	

    Glib_ClearScr(0, MODE_STN_2BIT);
    k=0;
    for(i=160;i<480;i+=80)
    {
    	for(j=120;j<360;j+=60)
    	{
    	    Glib_FilledRectangle(i,j,i+79,j+59,k%4);
    	    k++;
    	}
    	k+=2;;
    }

    // #0
    // 00
    Glib_Rectangle(0,0,319,239,3);   
    Glib_Line(0,0,319,239,3);        
    Glib_Line(0,239,319,0,3);

    // 0#
    // 00
    Glib_Rectangle(0+320,0,319+320,239,3);          
    Glib_Line(0+320,0,319+320,239,3);        
    Glib_Line(0+320,239,319+320,0,3);

    // 00
    // #0
    Glib_Rectangle(0,0+240,319,239+240,3);          
    Glib_Line(0,0+240,319,239+240,3);        
    Glib_Line(0,239+240,319,0+240,3);

    // 00
    // 0#
    Glib_Line(0+320,0+240,319+320,239+240,3);        
    Glib_Line(0+320,239+240,319+320,0+240,3);
    Glib_Rectangle(50+320,50+240,269+320,189+240,3);

    Uart_Printf("Virtual Screen Test(STN 4 gray). Press any key[ijkm\\r]!\n");
    MoveViewPort(MODE_STN_2BIT);
    Lcd_MoveViewPort(0,0,MODE_STN_2BIT);
    Lcd_EnvidOnOff(0); // Disable ENVID Bit
    Lcd_Port_Return();
}


void Test_Lcd_Stn_4Bit(void)
{
    int i,j,k;
    Lcd_Port_Init();
    Lcd_Init(MODE_STN_4BIT);
    Glib_Init(MODE_STN_4BIT);
    Lcd_EnvidOnOff(1);
    Uart_Printf("[STN 16 gray(4bit/1pixel) LCD Test]\n");

    Glib_ClearScr(0, MODE_STN_4BIT);
    j=0;
    for(i=0;i<320;i+=20)
        Glib_FilledRectangle(0+i,0,19+i,239,j++);
    Uart_Printf("16 gray mode test 1. Press any key!\n");
    Uart_Getch();  	

    Glib_ClearScr(0, MODE_STN_4BIT);
    j=0;
    for(i=0;i<320;i+=20)
    {
    	Glib_FilledRectangle(0+i,0,19+i,119,j);
    	Glib_FilledRectangle(0+i,120,19+i,239,15-j);
    	j++;
    }
    Uart_Printf("16 gray mode test 2. Press any key!\n");
    Uart_Getch();  	


    Glib_ClearScr(0, MODE_STN_4BIT);
    j=0;
    for(i=0;i<240;i+=30)
    {
    	Glib_FilledRectangle(i,i,i+29,i+29,j);
    	Glib_FilledRectangle(i+110,i,i+29+110,i+29,j+8);
    	j++;
    }
    Uart_Printf("4 gray mode test 3. Press any key!\n");
    Uart_Getch();  	 


    Glib_ClearScr(0, MODE_STN_4BIT);
    k=0;
    for(i=160;i<480;i+=40)
    	for(j=120;j<360;j+=30)
    	{
    	    Glib_FilledRectangle(i,j,i+39,j+29,k%16);
    	    k++;
    	}
    		    
    // #0		    
    // 00		    
    Glib_Rectangle(0,0,319,239,15);   
    Glib_Line(0,0,319,239,15);        
    Glib_Line(0,239,319,0,15);

    // 0#
    // 00
    Glib_Rectangle(0+320,0,319+320,239,15);
    Glib_Line(0+320,0,319+320,239,15);        
    Glib_Line(0+320,239,319+320,0,15);

    // 00
    // #0
    Glib_Rectangle(0,0+240,319,239+240,15);
    Glib_Line(0,0+240,319,239+240,15);        
    Glib_Line(0,239+240,319,0+240,15);

    // 00
    // 0#
    Glib_Rectangle(0+320,0+240,319+320,239+240,15);
    Glib_Line(0+320,0+240,319+320,239+240,15);     
    Glib_Line(0+320,239+240,319+320,0+240,15);
    Glib_Rectangle(50+320,50+240,269+320,189+240,15);

    Uart_Printf("Virtual Screen Test(STN 16 gray). Press any key[ijkm\\r]!\n");
    MoveViewPort(MODE_STN_4BIT);
    Lcd_MoveViewPort(0,0,MODE_STN_4BIT);
    Lcd_EnvidOnOff(0);
    Lcd_Port_Return();
}


void Test_Lcd_Cstn_8Bit(void)
{
    int i,j,k;
    Lcd_Port_Init();
    Lcd_Init(MODE_CSTN_8BIT);
    Glib_Init(MODE_CSTN_8BIT);
    Lcd_CstnOnOff(1); // Enable ENVID Bit and GPB5 output
    Uart_Printf("[CSTN 256 COLOR(8bit/1pixel) LCD TEST]\n");
    Uart_Printf("     R:0   ...    7 \n");
    Uart_Printf("G:0  B0:1 B0:1 B0:1 \n");
    Uart_Printf("G:.   2:3  2:3  2:3 \n");
    Uart_Printf("G:.  B0:1 B0:1 B0:1 \n");
    Uart_Printf("G:.   2:3  2:3  2:3 \n");
    Uart_Printf("G:.  B0:1 B0:1 B0:1 \n");
    Uart_Printf("G:7   2:3  2:3  2:3 \n");

    
    Glib_ClearScr(0, MODE_CSTN_8BIT);
    for(j=0;j<240;j++)
		for(i=0;i<320;i++) //RRRGGGBB
		    PutPixel(i,j,((i/40)<<5)+((j/30)<<2)+(((j/15)%2)<<1)+((i/20)%2));
    Uart_Printf("CSTN 256 color mode test 1. Press any key!\n");
    Uart_Getch();  	
  
    Glib_ClearScr(0, MODE_CSTN_8BIT);
    k=0;
    for(i=160;i<480;i+=20)
    	for(j=120;j<360;j+=15)
    	{
    	    Glib_FilledRectangle(i,j,i+19,j+14,k);
    	    k++;
    	}

    // #0		    
    // 00		    
    Glib_Rectangle(0,0,319,239,255);   
    Glib_Line(0,0,319,239,255);        
    Glib_Line(0,239,319,0,255);

    // 0#
    // 00
    Glib_Rectangle(0+320,0,319+320,239,255);
    Glib_Line(0+320,0,319+320,239,255);        
    Glib_Line(0+320,239,319+320,0,255);

    // 00
    // #0
    Glib_Rectangle(0,0+240,319,239+240,255);
    Glib_Line(0,0+240,319,239+240,255);        
    Glib_Line(0,239+240,319,0+240,255);

    // 00
    // 0#
    Glib_Rectangle(0+320,0+240,319+320,239+240,255);
    Glib_Line(0+320,0+240,319+320,239+240,255);     
    Glib_Line(0+320,239+240,319+320,0+240,255);
    Glib_Rectangle(50+320,50+240,269+320,189+240,255);

    Uart_Printf("Virtual Screen Test(CSTN 256 color). Press any key[ijkm\\r]!\n");
    MoveViewPort(MODE_CSTN_8BIT);
    Lcd_MoveViewPort(0,0,MODE_CSTN_8BIT);
    Lcd_CstnOnOff(0); // Disable ENVID Bit and GPB5 output
    Lcd_Port_Return();
}


void Test_Lcd_Cstn_8Bit_On(void)
{
    int i,j;
    Lcd_Port_Init();
    Lcd_Init(MODE_CSTN_8BIT);
    Glib_Init(MODE_CSTN_8BIT);
    Lcd_CstnOnOff(1); // Enable ENVID Bit and GPB5 output
    Uart_Printf("[CSTN LCD(8bit/1pixel) WILL BE TURNED ON EVEN DURING OTHER TEST]\n");
    Uart_Printf("If you'd like to off the CSTN LCD, then excute other LCD test program,\n");     
    for(j=0;j<240;j++)
	for(i=0;i<320;i++)	//RRRGGGBB
	    PutPixel(i,j,((i/40)<<5)+((j/30)<<2)+(((j/15)%2)<<1)+((i/20)%2));
}


void Test_Lcd_Cstn_12Bit(void)
{
    int i,j,k;
	U32 cMode;
	
    Uart_Printf("[CSTN 4K COLOR(12bit/1pixel) LCD TEST]\n");
	Uart_Printf(" Select 12 bpp data format  1. Packed[D]   2. Unpacked\n");
	if (Uart_Getch() == '2')
		cMode = MODE_CSTN_12BIT_UP;
	else cMode = MODE_CSTN_12BIT;

	Lcd_Port_Init();
    Lcd_Init(cMode);
    Glib_Init(cMode);
    Lcd_CstnOnOff(1); // Enable ENVID Bit and GPB5 output
 
    Glib_ClearScr(0, cMode);
    k=0;
    for(j=0;j<240;j+=80)
        for(i=0;i<320;i+=20)
		{   
	 	   if((k<16)|(k>31))
		    {
		        Glib_FilledRectangle(i,j,i+19,j+79,((k%16)<<(4*(k/16))));
		        k++;
		    }
		    else
		    {
		        Glib_FilledRectangle(i,j,i+19,j+79,((k-(1+2*(k%16)))<<(4*(k/16))));
		        k++;
		    }
		}
    Uart_Printf("CSTN 4K color mode test 1. Press any key!\n");
    Uart_Getch();  	

#if 1 // just test
	i = 0;
	while(1)		
	{
		if (Uart_Getch() == '\r') break;
		Lcd_CstnOnOff(0); // Enable ENVID Bit and GPB5 output
		Delay(1);
		Lcd_CstnOnOff(1); // Enable ENVID Bit and GPB5 output
	}
	Lcd_Port_Return();			
#else
    Glib_ClearScr(0, cMode);
    for(j=0;j<240;j++)
		for(i=0;i<320;i++)
		    PutPixel(i,j,((i/40)<<9)+((j/30)<<5)+(((j/15)%2)<<1)+((i/20)%2));
    Uart_Printf("CSTN 4K color mode test 2. Press any key!\n");
    Uart_Getch();  	

    Glib_ClearScr(0, cMode);
    k=0;
    for(i=160;i<480;i+=20)
    	for(j=120;j<360;j+=15)
    	{
    	    Glib_FilledRectangle(i,j,i+19,j+14,(((k&0xf)<<8)|((k%4)<<4)|(k&0xf0)) );
    	    k++;
    	}

    // #0		    
    // 00		    
    Glib_Rectangle(0,0,319,239,0xf00);   
    Glib_Line(0,0,319,239,0x0f0);        
    Glib_Line(0,239,319,0,0x00f);
    // 0#
    // 00
    Glib_Rectangle(0+320,0,319+320,239,0xfff);
    Glib_Line(0+320,0,319+320,239,0xfff);        
    Glib_Line(0+320,239,319+320,0,0xfff);
    // 00
    // #0
    Glib_Rectangle(0,0+240,319,239+240,0xfff);
    Glib_Line(0,0+240,319,239+240,0xfff);        
    Glib_Line(0,239+240,319,0+240,0xfff);
    // 00
    // 0#
    Glib_Rectangle(0+320,0+240,319+320,239+240,0xfff);
    Glib_Line(0+320,0+240,319+320,239+240,0xfff);     
    Glib_Line(0+320,239+240,319+320,0+240,0xfff);
    Glib_Rectangle(50+320,50+240,269+320,189+240,0xfff);

    Uart_Printf("Virtual Screen Test(CSTN 4K color). Press any key[ijkm\\r]!\n");
    MoveViewPort(cMode);
    Lcd_MoveViewPort(0,0,cMode);
    Lcd_CstnOnOff(0); // Disable ENVID Bit and GPB5 output 
    Lcd_Port_Return();
#endif
}

void Test_Lcd_Cstn_16Bit(void)
{
    int i,j,k,c;
    Lcd_Port_Init();
    Lcd_Init(MODE_CSTN_16BIT);
    Glib_Init(MODE_CSTN_16BIT);
    Lcd_CstnOnOff(1); // Enable ENVID Bit and GPB5 output
    Uart_Printf("[CSTN 4K COLOR 16BPP mode LCD TEST]\n");
 
    Glib_ClearScr(0, MODE_CSTN_16BIT);
    k=0;
    for(j=0;j<240;j+=80)
        for(i=0;i<320;i+=20)
		{   
	 	   if(j==0)
		    {
		    	c = (k%16)<<1; //blue
		    }
			 else if (j==80)
			 {
				 c = (15-k%16)<<7; //green 
			 }
	 	    else
		    {
		    	c = (k%16)<<12; //red
		    }
			Glib_FilledRectangle(i,j,i+19,j+79,c);
			k++;
		}
    Uart_Printf("CSTN 4K color mode test 1. Press any key!\n");
    Uart_Getch();  	

    Glib_ClearScr(0, MODE_CSTN_16BIT);
    for(j=0;j<240;j++)
		for(i=0;i<320;i++)
		    PutPixel(i,j,((i/40)<<13)+((j/30)<<8)+(((j/15)%2)<<2)+((i/20)%2));
    Uart_Printf("CSTN 4K color mode test 2. Press any key!\n");
    Uart_Getch();  	

    Glib_ClearScr(0, MODE_CSTN_16BIT);
    k=0;
    for(i=160;i<480;i+=20)
    	for(j=120;j<360;j+=15)
    	{
    	    Glib_FilledRectangle(i,j,i+19,j+14,(((k&0xf)<<12)|((k%4)<<7)|(k&0x8)) );
    	    k++;
    	}

    // #0		    
    // 00		    
    Glib_Rectangle(0,0,319,239,0xf00);   
    Glib_Line(0,0,319,239,0x0f0);        
    Glib_Line(0,239,319,0,0x00f);
    // 0#
    // 00
    Glib_Rectangle(0+320,0,319+320,239,0xfff);
    Glib_Line(0+320,0,319+320,239,0xfff);        
    Glib_Line(0+320,239,319+320,0,0xfff);
    // 00
    // #0
    Glib_Rectangle(0,0+240,319,239+240,0xfff);
    Glib_Line(0,0+240,319,239+240,0xfff);        
    Glib_Line(0,239+240,319,0+240,0xfff);
    // 00
    // 0#
    Glib_Rectangle(0+320,0+240,319+320,239+240,0xfff);
    Glib_Line(0+320,0+240,319+320,239+240,0xfff);     
    Glib_Line(0+320,239+240,319+320,0+240,0xfff);
    Glib_Rectangle(50+320,50+240,269+320,189+240,0xfff);

    Uart_Printf("Virtual Screen Test(CSTN 4K color). Press any key[ijkm\\r]!\n");
    MoveViewPort(MODE_CSTN_16BIT);
    Lcd_MoveViewPort(0,0,MODE_CSTN_16BIT);
    Lcd_CstnOnOff(0); // Disable ENVID Bit and GPB5 output
    Lcd_Port_Return();
}



void Select_Tcon()
{
	U8 ch=0;

	Uart_Printf("Select LCD controller type [1-LPC3600  2-LCC3600  3-General TFT] : ");
	ch = Uart_GetIntNum();

	switch (ch)
	{
	case 1 :
		Lcd_Lpc3600Enable(); // Enable LPC3600
		break;
	case 2 :
		Lcd_Lcc3600Enable();
		break;
	case 3 : 
		Lcd_TconDisable();
		break;
	default :
		Lcd_Lcc3600Enable(); // Enable LCC3600
		break;
	}		
}

void Test_Lcd_Tft_8Bit_240320(void)
{
    int i,j,k;

    Lcd_Port_Init();
    Lcd_Palette8Bit_Init(); // Initialize 256 palette 
    Lcd_Init(MODE_TFT_8BIT_240320);
    Glib_Init(MODE_TFT_8BIT_240320);

	Uart_Printf("[TFT 256 COLOR(8bit/1pixel) LCD TEST]\n");
	Select_Tcon();
	
    Lcd_PowerEnable(0, 1);
    rTPAL = (1<<24)|((0xff)<<16); // Enable Temporary Palette : Red
    Lcd_EnvidOnOff(1); // Enable ENVID Bit
    Uart_Printf("\nTFT 256 color mode test 1. Press any key!\n");
    Uart_Getch(); 

    Glib_ClearScr(0, MODE_TFT_8BIT_240320); // Fill the LCD panel with Black Color
    k=0;
    for(i=0;i<320;i+=20)
		for(j=0;j<240;j+=15)
		{ 
		    Glib_FilledRectangle(j,i,j+14,i+19,(k%256));
		    k++;
		}
    rTPAL = 0; // Disable Temporary Palette
    Uart_Printf("TFT 256 color mode test 2. Press any key!\n");
    Uart_Getch();  	

    Glib_ClearScr(0, MODE_TFT_8BIT_240320); // Fill the LCD panel with Black Color
    k=0;
    for(i=0;i<320;i+=10)
		for(j=0;j<240;j+=10)
		{ 
		    Glib_FilledRectangle(j,i,j+9,i+9,(k%256));
		    k+=30;
		}
    Uart_Printf("TFT 256 color mode test 3. Press any key!\n");
    Uart_Getch();  	

    Glib_ClearScr(0, MODE_TFT_8BIT_240320);
    k=0;
    for(i=80;i<240;i+=10)
    {
    	for(j=60;j<180;j+=10)
    	{
    	    Glib_FilledRectangle(j,i,j+9,i+9,k);
    	    k++;
    	}
    }
    Glib_Rectangle(0,0,239,319,255);   
    Glib_Line(0,0,239,319,255);        
    Glib_Line(0,319,239,0,255);
    
    Glib_Rectangle(0+240,0,239+240,319,255);
    Glib_Line(0+240,0,239+240,319,255);        
    Glib_Line(0+240,319,239+240,0,255);
    
    Glib_Rectangle(0,0+320,239,319+320,255);
    Glib_Line(0,0+320,239,319+320,255);        
    Glib_Line(0,319+320,239,0+320,255);
    
    Glib_Rectangle(0+240,0+320,239+240,319+320,255);
    Glib_Line(0+240,0+320,239+240,319+320,255);     
    Glib_Line(0+240,319+320,239+240,0+320,255);

    Glib_Rectangle(10+240,10+320,480-11,640-11,255);

    Uart_Printf("Virtual Screen Test(TFT 256 color). Press any key[ijkm\\r]!\n");
    MoveViewPort(MODE_TFT_8BIT_240320);
    Lcd_MoveViewPort(0,0,MODE_TFT_8BIT_240320);
    Glib_ClearScr(0, MODE_TFT_8BIT_240320);
    Lcd_EnvidOnOff(0);
    Lcd_PowerEnable(0, 0);
    Lcd_Port_Return();
}


void Test_Lcd_Tft_16Bit_240320(void)
{
    int i,j,k;

    Lcd_Port_Init();
    Lcd_Init(MODE_TFT_16BIT_240320);
    Glib_Init(MODE_TFT_16BIT_240320);

    Uart_Printf("[TFT 64K COLOR(16bit/1pixel) LCD TEST]\n");
	Select_Tcon();

    Lcd_PowerEnable(0, 1);
    Lcd_EnvidOnOff(1);

    Glib_ClearScr(0, MODE_TFT_16BIT_240320);
    Glib_FilledRectangle(0,0,239,159,0xf800);    
    Glib_FilledRectangle(0,160,239,320,0xf800);    
    Uart_Printf("\nTFT 64K color mode test 1. Press any key!\n");
    Uart_Getch();  	
    
    Glib_ClearScr(0, MODE_TFT_16BIT_240320);
    Glib_Rectangle(0,0,239,319,0x07e0);   
    Glib_FilledRectangle(0,0,20,20,65535);   
    Glib_Rectangle(220,300,239,319,65535);   
    Glib_Line(0,0,239,319,0x1f);        
    Glib_Line(239,0,0,319,0xf800);
    Uart_Printf("TFT 64K color mode test 2. Press any key!\n");
    Uart_Getch();  	

    Glib_ClearScr(0, MODE_TFT_16BIT_240320);
    k=0;
    for(i=80;i<240;i+=10)
    {
    	for(j=60;j<180;j+=10)
    	{
    	    Glib_FilledRectangle(j,i,j+9,i+9,k);
    	    k++;
    	}
    }

    Glib_Rectangle(0,0,239,319,65535);   
    Glib_Line(0,0,239,319,65535);        
    Glib_Line(0,319,239,0,65535);
    
    Glib_Rectangle(0+240,0,239+240,319,255);
    Glib_Line(0+240,0,239+240,319,255);        
    Glib_Line(0+240,319,239+240,0,255);
    
    Glib_Rectangle(0,0+320,239,319+320,255);
    Glib_Line(0,0+320,239,319+320,255);        
    Glib_Line(0,319+320,239,0+320,255);
    
    Glib_Rectangle(0+240,0+320,239+240,319+320,255);
    Glib_Line(0+240,0+320,239+240,319+320,255);     
    Glib_Line(0+240,319+320,239+240,0+320,255);
    Glib_Rectangle(40+240,40+320,480-41,640-41,0x1f);

    Uart_Printf("Virtual Screen Test(TFT 64K color). Press any key[ijkm\\r]!\n");
    MoveViewPort(MODE_TFT_16BIT_240320);
    Lcd_MoveViewPort(0,0,MODE_TFT_16BIT_240320);
    Glib_ClearScr(0, MODE_TFT_16BIT_240320);
    Lcd_EnvidOnOff(0);
    Lcd_PowerEnable(0, 0);
    Lcd_Port_Return();
}


void Test_Lcd_Tft_8Bit_240320_On(void)
{
    int i,j,k=0;
    Lcd_Port_Init();
    Lcd_Palette8Bit_Init();
    Lcd_Init(MODE_TFT_8BIT_240320);
    Glib_Init(MODE_TFT_8BIT_240320);

	Select_Tcon();
    Lcd_PowerEnable(0, 1);    
    Uart_Printf("\n[TFT LCD(240x320 ,8bpp) WILL BE TURNED ON EVEN DURING OTHER TEST]\n");
    Uart_Printf("If you'd like to off the TFT LCD, then excute other LCD test program,\n");     

    for(j=0;j<320;j++)
        for(i=0;i<240;i++)
            PutPixel(i,j,((int)DEMO256[k++]));
            
    Lcd_EnvidOnOff(1);
}


void Test_Lcd_Tft_8Bit_240320_Bmp(void)
{
    int i,j,k=0;
    Lcd_Port_Init();
    Lcd_Palette8Bit_Init();
    Lcd_Init(MODE_TFT_8BIT_240320);
    Glib_Init(MODE_TFT_8BIT_240320);
	Select_Tcon();
    Lcd_PowerEnable(0, 1);    // Enable LCD_PWREN

    for(j=0;j<320;j++)
        for(i=0;i<240;i++)
            PutPixel(i,j,((int)DEMO256[k++]));

    Lcd_EnvidOnOff(1); // Enable ENVID    

    Uart_Printf("Test palette & RGB format(5:6:5 ->5:5:5:1)..........\n");        
    Uart_Printf("then press any key\n");        
    Uart_Getch();
    Lcd_EnvidOnOff(0);
		
    rLCDCON5&=~(1<<11); // 5:5:5:1 Palette Setting & put pixel with 5551 format
    Lcd_Palette8Bit_Init(); //  palette change to 5:5:5:1 format..
    
    Uart_Printf("One more ,please...\n");
    Uart_Getch();    
    Lcd_EnvidOnOff(1);

    Uart_Printf("Press any key to quit!\n");        
    Uart_Getch();
    Glib_ClearScr(0, MODE_TFT_8BIT_240320);
    Delay(5000);
    Lcd_EnvidOnOff(0);
    rLCDCON5|=(1<<11); // 5:6:5 Palette Setting
    Lcd_Port_Return();
}


void Test_Lcd_Tft_8Bit_240320_IntFr(void)
{
    int i,j,k=0;
    Lcd_Port_Init();
    Lcd_Palette8Bit_Init();
    Lcd_Init(MODE_TFT_8BIT_240320);
    Glib_Init(MODE_TFT_8BIT_240320);

	Select_Tcon();
    Lcd_PowerEnable(0, 1);    
    rLCDCON5|=(1<<11); // 5:6:5 Palette Setting
    
    for(j=0;j<320;j++)
        for(i=0;i<240;i++)
            PutPixel(i,j,((int)DEMO256[k++]));

    //--------LCD frame interrupt test---------------------------------------------------START
    //The content of GPG4 is changed to OUTPUT('01'). 
    //We will check the frame interrupt with using Logic Analyzer. 
    rGPGUP=rGPGUP&(~(1<<4))|(1<<4); // Pull-up disable
    rGPGDAT=rGPGDAT&(~(1<<4))|(1<<4); // GPG4=High
    rGPGCON=rGPGCON&(~(3<<8))|(1<<8); // GPG4=Output
   
    pISR_LCD=(unsigned)Lcd_Int_Frame;
    rINTMSK=~(BIT_LCD);
    rLCDINTMSK=(1<<2)|(0<<1)|(1); // 8Words Trigger Level,Unmask Frame int,mask Fifo int
    //--------LCD frame interrupt test-----------------------------------------------------END
 
    Lcd_EnvidOnOff(1); // Enable ENVID    
    Uart_Printf("Press any key to quit!\n");        
    Uart_Getch();
    rLCDINTMSK|=3; // Mask LCD SUB Interrupt
    rINTMSK|=(BIT_LCD); // Mask LCD INT
    Lcd_EnvidOnOff(0);
    Lcd_Port_Return();
}


void Test_Lcd_Tft_8Bit_240320_IntFi(void)
{
    int i,j,k=0;
    Lcd_Port_Init();
    Lcd_Palette8Bit_Init();
    Lcd_Init(MODE_TFT_8BIT_240320);
    Glib_Init(MODE_TFT_8BIT_240320);
	Select_Tcon();
    Lcd_PowerEnable(0, 1);    
    rLCDCON5|=(1<<11); // 5:6:5 Palette Setting
    
    for(j=0;j<320;j++)
        for(i=0;i<240;i++)
            PutPixel(i,j,((int)DEMO256[k++]));

    Lcd_EnvidOnOff(1); // Enable ENVID    
    //--------LCD FIFO interrupt test---------------------------------------------------START
    pISR_LCD=(unsigned)Lcd_Int_Fifo;
    rINTMSK=~(BIT_LCD);
    rLCDINTMSK=(0<<2)|(1<<1)|(0); // 4Words Trigger Level,Mask Frame int,Unmask Fifo int
    //--------LCD FIFO interrupt test-----------------------------------------------------END
 
    Uart_Printf("Press any key to quit!\n");        
    Uart_Getch();
    rLCDINTMSK|=3; // Mask LCD SUB Interrupt
    rINTMSK|=(BIT_LCD); // Mask LCD INT
    Lcd_EnvidOnOff(0);
    Lcd_Port_Return();
}



//640480
void __irq Lcd_Int_Fifo_640480(void)
{
    int i;
    rLCDINTMSK|=3; // Mask LCD SUB Interrupt

    // GPG4 is .... 
    rGPGDAT&=(~(1<<4)); // GPG4=Low
    for(i=0;i<50;i++); // GPG4=Low
    rGPGDAT|=(1<<4); //GPG4=High

    rLCDSRCPND=1; // Clear LCD SUB Interrupt source pending
    rLCDINTPND=1; // Clear LCD SUB Interrupt pending
    rLCDINTMSK&=(~(1)); // Unmask LCD FRAME Interrupt
    ClearPending(BIT_LCD);
}


void Test_Lcd_Tft_1Bit_640480(void)
{
    int i,j;

	Lcd_Palette1Bit_Init();

	Lcd_Start(MODE_TFT_1BIT_640480);
    Uart_Printf("[TFT Mono(1bit/1pixel) LCD TEST]\n");
    Glib_ClearScr(0,MODE_TFT_1BIT_640480);

    rTPAL = (1<<24)|((0xff)<<8); // Enable Temporary Palette : Green
    Uart_Printf("TFT Mono mode test 1. Press any key!\n");
    Uart_Getch();  	

    rTPAL = 0;
    Glib_FilledRectangle(0,0,639,239,1);
    Glib_FilledRectangle(0,239,639,479,0);   
    Uart_Printf("TFT Mono mode test 2. Press any key!\n");
    Uart_Getch();  	

    Glib_FilledRectangle(0,0,319,479,1);
    Glib_FilledRectangle(320,0,639,479,0);   
    Uart_Printf("TFT Mono mode test 3. Press any key!\n");
    Uart_Getch();  	

    for(j=0;j<LCD_YSIZE_TFT_640480;j+=20)
	for(i=320;i<640;i+=20)
	    Glib_FilledRectangle(i,j,i+19,j+19,((j+i)/20)%2);
    Uart_Printf("TFT Mono mode test 4. Press any key!\n");
    Uart_Getch();  	

    Glib_ClearScr(0, MODE_TFT_1BIT_640480);
    Glib_FilledRectangle(0+320,0+240,1279-320,959-240,1);
    
    // #0		    
    // 00		    
    Glib_Rectangle(0,0,639,479,1);   
    Glib_Line(0,0,639,479,1);        
    Glib_Line(0,479,639,0,1);

    // 0#
    // 00
    Glib_Rectangle(0+640,0,639+640,479,1);   
    Glib_Line(0+640,0,639+640,479,1);        
    Glib_Line(0+640,479,639+640,0,1);

    // 00
    // #0
    Glib_Rectangle(0,0+480,639,479+480,1);   
    Glib_Line(0,0+480,639,479+480,1);        
    Glib_Line(0,479+480,639,0+480,1);

    // 00
    // 0#
    Glib_Rectangle(0+640,0+480,639+640,479+480,1);   
    Glib_Line(0+640,0+480,639+640,479+480,1);        
    Glib_Line(0+640,479+480,639+640,0+480,1);
    Glib_Rectangle(50+640,50+480,639+640-50,479+480-50,1);   

    Uart_Printf("Virtual Screen Test(TFT 256 color). Press any key[ijkm\\r]!\n");
    MoveViewPort(MODE_TFT_1BIT_640480);
    Lcd_MoveViewPort(0,0,MODE_TFT_1BIT_640480);
    Lcd_EnvidOnOff(0);
    Lcd_Port_Return();
}



void Test_Lcd_Tft_8Bit_640480(void)
{
    int i,j,k;

    Lcd_Palette8Bit_Init();

	Lcd_Start(MODE_TFT_8BIT_640480);
    Uart_Printf("[TFT 256 COLOR(8bit/1pixel) LCD TEST]\n");
    Glib_ClearScr(0, MODE_TFT_8BIT_640480);
    rTPAL = (1<<24)|((0xff)<<0); // Enable Temporary Palette : Blue
    Uart_Printf("TFT 256 color mode test 1. Press any key!\n");
    Uart_Getch();  	

    k=0;
    for(i=0;i<640;i+=40)
	for(j=0;j<480;j+=30)
	{ 
	    Glib_FilledRectangle(i,j,i+39,j+29,(k%256));
	    k++;
	}
    rTPAL = 0;
    Uart_Printf("TFT 256 color mode test 2. Press any key!\n");
    Uart_Getch();  	

    Glib_ClearScr(0, MODE_TFT_8BIT_640480);
    k=0;
    for(i=0;i<640;i+=20)
		for(j=0;j<480;j+=20)
		{ 
		    Glib_FilledRectangle(i,j,i+19,j+19,(k%256));
		    k+=30;
		}
    Uart_Printf("TFT 256 color mode test 3. Press any key!\n");
    Uart_Getch();  	

    Glib_ClearScr(0, MODE_TFT_8BIT_640480);
    k=0;
    for(i=160;i<480;i+=20)
    	for(j=120;j<360;j+=15)
    	{
    	    Glib_FilledRectangle(i,j,i+19,j+14,k);
    	    k++;
    	}

    // #0		    
    // 00		    
    Glib_Rectangle(0,0,319,239,255);   
    Glib_Line(0,0,319,239,255);        
    Glib_Line(0,239,319,0,255);

    // 0#
    // 00
    Glib_Rectangle(0+320,0,319+320,239,255);
    Glib_Line(0+320,0,319+320,239,255);        
    Glib_Line(0+320,239,319+320,0,255);

    // 00
    // #0
    Glib_Rectangle(0,0+240,319,239+240,255);
    Glib_Line(0,0+240,319,239+240,255);        
    Glib_Line(0,239+240,319,0+240,255);

    // 00
    // 0#
    Glib_Rectangle(0+320,0+240,319+320,239+240,255);
    Glib_Line(0+320,0+240,319+320,239+240,255);     
    Glib_Line(0+320,239+240,319+320,0+240,255);
    Glib_Rectangle(50+320,50+240,269+320,189+240,255);

    Uart_Printf("TFT 256 color mode test 4. Press any key!\n");
    Uart_Getch();  	
   
    Glib_ClearScr(0, MODE_TFT_8BIT_640480);
    k=0;
    for(i=320;i<960;i+=40)
    	for(j=240;j<720;j+=30)
    	{
    	    Glib_FilledRectangle(i,j,i+39,j+29,(k%256));
    	    k++;
    	}

    // #0		    
    // 00		    
    Glib_Rectangle(0,0,639,479,255);   
    Glib_Line(0,0,639,479,255);        
    Glib_Line(0,479,639,0,255);

    // 0#
    // 00
    Glib_Rectangle(0+640,0,639+640,479,255);   
    Glib_Line(0+640,0,639+640,479,255);        
    Glib_Line(0+640,479,639+640,0,255);

    // 00
    // #0
    Glib_Rectangle(0,0+480,639,479+480,255);   
    Glib_Line(0,0+480,639,479+480,255);        
    Glib_Line(0,479+480,639,0+480,255);

    // 00
    // 0#
    Glib_Rectangle(0+640,0+480,639+640,479+480,255);   
    Glib_Line(0+640,0+480,639+640,479+480,255);        
    Glib_Line(0+640,479+480,639+640,0+480,255);
    Glib_Rectangle(50+640,50+480,639+640-50,479+480-50,255);   

    Uart_Printf("Virtual Screen Test(TFT 256 color). Press any key[ijkm\\r]!\n");
    MoveViewPort(MODE_TFT_8BIT_640480);
    Lcd_MoveViewPort(0,0,MODE_TFT_8BIT_640480);
    Lcd_EnvidOnOff(0);
    Lcd_Port_Return();
}


void Test_Lcd_Tft_8Bit_640480_Palette(void)
{
    int i,j,k;
    unsigned char cdata, p_red, p_green, p_blue;
    U32 *palette;

    Lcd_Palette8Bit_Init();

	Lcd_Start(MODE_TFT_8BIT_640480);
    Uart_Printf("[TFT 256 COLOR(8bit/1pixel) Palette TEST]\n");
    Glib_ClearScr(0, MODE_TFT_8BIT_640480);
    rTPAL = (1<<24)|((0xff)<<0); // Enable Temporary Palette : Blue
    Uart_Printf("Temporary palette test. Press any key!\n");    
    Uart_Getch();  	

    k=0;
    for(i=0;i<640;i+=40)
	for(j=0;j<480;j+=30)
	{ 
	    Glib_FilledRectangle(i,j,i+39,j+29,(k%256));
	    k++;
	}
    rTPAL = 0;
    Uart_Printf("Press any key to test writing palette during non-active.\n");

    palette=(U32 *)PALETTE;
    for(cdata=0;cdata<255;cdata++)
    {
	p_red=(cdata | 0xe0);
	p_green=(cdata | 0x1c);
	p_blue=(cdata | 0x03);

        while((rLCDCON5>>19)==2);
    	*palette++=((U32)((p_red<<8)|(p_green<<6)|(p_blue<<3)));
    	Uart_Printf("VSTATUS=%x\n",(rLCDCON5>>19));
    	Uart_Printf("cdata=%d\n",(int)cdata);
    }
    *palette=0x0;    
    Uart_Getch();
    Lcd_EnvidOnOff(0);
    Lcd_Port_Return();
}


void Test_Lcd_Tft_8Bit_640480_Bswp(void)
{
    int k;

    Lcd_Palette8Bit_Init();
	Lcd_Start(MODE_TFT_8BIT_640480);
    rLCDCON5 &= 0x1ffffd;    //BSWP Disable
    Uart_Printf("[TFT 256 COLOR(16bit/1pixel) BSWP TEST]\n");

    Glib_ClearScr(0, MODE_TFT_8BIT_640480);
    Glib_Rectangle(0,0,639,479,0xff);
    Glib_Rectangle(1,1,638,478,0x03);
    Glib_Rectangle(0+100,0+100,639-100,479-100,0xff);
    Glib_Rectangle(1+100,1+100,638-100,478-100,0x03);
    Glib_Line(0,0,639,479,0xe0);     
    Glib_Line(639,0,0,479,0x1c);
    Uart_Printf("TFT 256 color BSWP test. Press any key excet return!\n");

    while(Uart_Getch()!='\r')
    {
        rLCDCON5 &= 0x1ffffd;    //BSWP Disable
        rLCDCON5 |= ((k%2)<<1);  //BSWP Enable
        k++;
    }

    Lcd_EnvidOnOff(0);
    Lcd_Port_Return();
}


void Test_Lcd_Tft_16Bit_640480(void)
{
    int i,j,k;

	Lcd_Start(MODE_TFT_16BIT_640480);
    Uart_Printf("[TFT 64K COLOR(16bit/1pixel) LCD TEST]\n");

    Glib_ClearScr(0, MODE_TFT_16BIT_640480);
    Glib_FilledRectangle(0,0,639,479,0xf800);    
    Uart_Printf("TFT 64K color mode test 1. Press any key!\n");
    Uart_Getch();  	

    Glib_ClearScr(0, MODE_TFT_16BIT_640480);
    Glib_Rectangle(0,0,639,479,0x07e0);   
    Glib_FilledRectangle(0,0,20,20,65535);   
    Glib_Rectangle(620,460,639,479,65535);   
    Glib_Line(0,0,639,479,0x1f);        
    Glib_Line(639,0,0,479,0xf800);
    Uart_Printf("TFT 64K color mode test 2. Press any key!\n");
    Uart_Getch();  	

    Glib_ClearScr(0, MODE_TFT_16BIT_640480);
    k=0;
    for(i=160;i<480;i+=20)
    	for(j=120;j<360;j+=15)
    	{
	    	while((rLCDCON5>>19)==2);
    	    Glib_FilledRectangle(i,j,i+19,j+14,k);
    	    k+=500;
    	}

    // #0		    
    // 00		    
    Glib_Rectangle(0,0,319,239,65535);   
    Glib_Line(0,0,319,239,65535);        
    Glib_Line(0,239,319,0,65535);

    // 0#
    // 00
    Glib_Rectangle(0+320,0,319+320,239,255);
    Glib_Line(0+320,0,319+320,239,255);        
    Glib_Line(0+320,239,319+320,0,255);

    // 00
    // #0
    Glib_Rectangle(0,0+240,319,239+240,255);
    Glib_Line(0,0+240,319,239+240,255);        
    Glib_Line(0,239+240,319,0+240,255);

    // 00
    // 0#
    Glib_Rectangle(0+320,0+240,319+320,239+240,255);
    Glib_Line(0+320,0+240,319+320,239+240,255);     
    Glib_Line(0+320,239+240,319+320,0+240,255);
    Glib_Rectangle(50+320,50+240,269+320,189+240,255);

    Uart_Printf("TFT 64K color mode test 3. Press any key!\n");
    Uart_Getch();  	

    Glib_ClearScr(0, MODE_TFT_16BIT_640480);
    k=0;
    for(i=320;i<960;i+=40)
    	for(j=240;j<720;j+=30)
    	{
    	    Glib_FilledRectangle(i,j,i+39,j+29,k);
    	    k+=500;
    	}

    // #0		    
    // 00		    
    Glib_Rectangle(0,0,639,479,65535);   
    Glib_Line(0,0,639,479,65535);        
    Glib_Line(0,479,639,0,65535);

    // 0#
    // 00
    Glib_Rectangle(0+640,0,639+640,479,65535);   
    Glib_Line(0+640,0,639+640,479,65535);        
    Glib_Line(0+640,479,639+640,0,65535);

    // 00
    // #0
    Glib_Rectangle(0,0+480,639,479+480,65535);   
    Glib_Line(0,0+480,639,479+480,65535);        
    Glib_Line(0,479+480,639,0+480,65535);

    // 00
    // 0#
    Glib_Rectangle(0+640,0+480,639+640,479+480,65535);   
    Glib_Line(0+640,0+480,639+640,479+480,65535);        
    Glib_Line(0+640,479+480,639+640,0+480,65535);
    Glib_Rectangle(50+640,50+480,639+640-50,479+480-50,65535);   

    Uart_Printf("Virtual Screen Test(TFT 64K color). Press any key[ijkm\\r]!\n");
    MoveViewPort(MODE_TFT_16BIT_640480);
    Lcd_MoveViewPort(0,0,MODE_TFT_16BIT_640480);
    Lcd_EnvidOnOff(0);
    Lcd_Port_Return();
}


void Test_Lcd_Tft_16Bit_640480_Hwswp(void)
{
    int k;

	Lcd_Start(MODE_TFT_16BIT_640480);
    rLCDCON5 &= 0x1ffffe;    //HWSWP Disable
    Uart_Printf("[TFT 64K COLOR(16bit/1pixel) HWSWP TEST]\n");

    Glib_ClearScr(0, MODE_TFT_16BIT_640480);
    Glib_Rectangle(0,0,639,479,0xffff);
    Glib_Rectangle(1,1,638,478,0xf800);
    Glib_Rectangle(0+100,0+100,639-100,479-100,0xffff);
    Glib_Rectangle(1+100,1+100,638-100,478-100,0xf800);
    Glib_Line(0,0,639,479,0x7e0);     
    Glib_Line(639,0,0,479,0x1f);
    Uart_Printf("TFT 64K color HWSWP test. Press any key except return key! Enter to exit..\n");

    while(Uart_Getch()!='\r')
    {
        rLCDCON5 &= 0x1ffffe;    //HWSWP Disable
        rLCDCON5 |= (k%2);       //HWSWP Enable
        k++;
    }

    Lcd_EnvidOnOff(0);
    Lcd_Port_Return();
}


void Test_Lcd_Tft_1Bit_800600(void)
{
//    int i,j;

	Lcd_Palette1Bit_Init();
	Lcd_Start(MODE_TFT_1BIT_800600);

	Uart_Printf("[TFT Mono(1bit/1pixel) LCD TEST]\n");

    rTPAL = (1<<24)|((0xff)<<0); // Enable Temporary Palette : Blue
    Uart_Printf("TFT Mono mode test 1. Press any key!\n");
    Uart_Getch();  	

    rTPAL = 0; // Disable TPAL
    Glib_FilledRectangle(0,0,LCD_XSIZE_TFT_800600-1,LCD_YSIZE_TFT_800600-1,1);
    Uart_Printf("TFT Mono mode test 2. Press any key!\n");
    Uart_Getch();  	

    Glib_FilledRectangle(200,200,LCD_XSIZE_TFT_800600-201,LCD_YSIZE_TFT_800600-201,0);   
    Uart_Printf("TFT Mono mode test 3. Press any key!\n");
    Uart_Getch();  	

    Glib_ClearScr(0, MODE_TFT_1BIT_800600);
    Glib_Rectangle(200,200,SCR_XSIZE_TFT_800600-201,SCR_YSIZE_TFT_800600-201,1);
    
    // #0		    
    // 00		    
    Glib_Rectangle(0,0,LCD_XSIZE_TFT_800600-1,LCD_YSIZE_TFT_800600-1,1);   
    Glib_Line(0,0,LCD_XSIZE_TFT_800600-1,LCD_YSIZE_TFT_800600-1,1);        
    Glib_Line(0,LCD_YSIZE_TFT_800600-1,LCD_XSIZE_TFT_800600-1,0,1);

    // 0#
    // 00
    Glib_Rectangle(LCD_XSIZE_TFT_800600,0,SCR_XSIZE_TFT_800600-1,LCD_YSIZE_TFT_800600-1,1);   
    Glib_Rectangle(LCD_XSIZE_TFT_800600+100,100,SCR_XSIZE_TFT_800600-101,LCD_YSIZE_TFT_800600-101,1);   
    
    // 00
    // #0
    Glib_Rectangle(0,LCD_YSIZE_TFT_800600,LCD_XSIZE_TFT_800600-1,SCR_YSIZE_TFT_800600-1,1);   
    Glib_Rectangle(200,LCD_YSIZE_TFT_800600+200,LCD_XSIZE_TFT_800600-201,SCR_YSIZE_TFT_800600-201,1);   

    // 00
    // 0#
    Glib_Rectangle(LCD_XSIZE_TFT_800600,LCD_YSIZE_TFT_800600,SCR_XSIZE_TFT_800600-1,SCR_YSIZE_TFT_800600-1,1);   
    Glib_Line(LCD_XSIZE_TFT_800600,LCD_YSIZE_TFT_800600,SCR_XSIZE_TFT_800600-1,SCR_YSIZE_TFT_800600-1,1);        
    Glib_Line(LCD_XSIZE_TFT_800600,SCR_YSIZE_TFT_800600-1,SCR_XSIZE_TFT_800600-1,LCD_YSIZE_TFT_800600,1);

    Uart_Printf("Virtual Screen Test(Mono). Press any key[ijkm\\r]!\n");
    MoveViewPort(MODE_TFT_1BIT_800600);
    Lcd_MoveViewPort(0,0,MODE_TFT_1BIT_800600);
//--------LCD FIFO interrupt test---------------------------------------------------START
    rLCDINTMSK|=3; // Mask LCD SUB Interrupt
    rINTMSK|=(BIT_LCD); // Mask LCD INT
//--------LCD FIFO interrupt test-----------------------------------------------------END   
    Lcd_EnvidOnOff(0);
    Lcd_Port_Return();
}


void Test_Lcd_Tft_8Bit_800600(void)
{
    int i,j,k;

    Lcd_Palette8Bit_Init();
	Lcd_Start(MODE_TFT_8BIT_800600);

    Uart_Printf("[TFT 256 COLOR(8bit/1pixel) LCD TEST]\n");

    rTPAL = (1<<24)|((0xff)<<0); // Enable Temporary Palette : Blue
//--------LCD FIFO interrupt test---------------------------------------------------START
	Lcd_EnvidOnOff(1); // Enable ENVID   
	//The content of GPG4 is changed to OUTPUT('01'). 
	//We will check the frame interrupt with using Logic Analyzer. 
	rGPGUP=rGPGUP&(~(1<<4))|(1<<4); // Pull-up disable
	rGPGDAT=rGPGDAT&(~(1<<4))|(1<<4); // GPG4=High
	rGPGCON=rGPGCON&(~(3<<8))|(1<<8); // GPG4=Output

	pISR_LCD=(unsigned)Lcd_Int_Frame;
	rINTMSK=~(BIT_LCD);
	rLCDINTMSK=(0<<2)|(1<<1)|(0); // 4Words Trigger Level,Mask Frame int,Unmask Fifo int
//        rLCDINTMSK=(1<<2)|(1<<1)|(0); // 8Words Trigger Level,Mask Frame int,Unmask Fifo int    
//        Lcd_EnvidOnOff(1); // Enable ENVID       
//--------LCD FIFO interrupt test-----------------------------------------------------END
    Uart_Printf("TFT 256 color mode test 1. Press any key!\n");
    Uart_Getch();  	

    Glib_ClearScr(0, MODE_TFT_8BIT_800600);
    rTPAL = 0;
    k=0;
    for(i=0;i<LCD_XSIZE_TFT_800600;i+=40)
		for(j=0;j<LCD_YSIZE_TFT_800600;j+=30)
		{ 
		    Glib_FilledRectangle(i,j,i+39,j+29,(k%256));
		    k++;
		}

    Uart_Printf("TFT 256 color mode test 2. Press any key!\n");
    Uart_Getch();  	

    Glib_ClearScr(0xff, MODE_TFT_8BIT_800600);
    Uart_Printf("TFT 256 color mode test 3. Press any key!\n");
    Uart_Getch();  	

    Glib_ClearScr(0, MODE_TFT_8BIT_800600);
    k=0;
    for(i=(SCR_XSIZE_TFT_800600/4);i<(SCR_XSIZE_TFT_800600*(3/4));i+=40)
    	for(j=SCR_YSIZE_TFT_800600/4;j<(SCR_YSIZE_TFT_800600*(3/4));j+=30)
    	{
    	    Glib_FilledRectangle(i,j,i+39,j+29,(k%256));
    	    k++;
    	}

    // #0		    
    // 00		    
    Glib_Rectangle(0,0,LCD_XSIZE_TFT_800600-1,LCD_YSIZE_TFT_800600-1,0xff);   
    Glib_Line(0,0,LCD_XSIZE_TFT_800600-1,LCD_YSIZE_TFT_800600-1,0xff);        
    Glib_Line(0,LCD_YSIZE_TFT_800600-1,LCD_XSIZE_TFT_800600-1,0,0xff);

    // 0#
    // 00
    Glib_Rectangle(LCD_XSIZE_TFT_800600,0,SCR_XSIZE_TFT_800600-1,LCD_YSIZE_TFT_800600-1,0xf0);   
    Glib_Rectangle(LCD_XSIZE_TFT_800600+100,100,SCR_XSIZE_TFT_800600-101,LCD_YSIZE_TFT_800600-101,0xf0);   
    
    // 00
    // #0
    Glib_Rectangle(0,LCD_YSIZE_TFT_800600,LCD_XSIZE_TFT_800600-1,SCR_YSIZE_TFT_800600-1,0x0f);   
    Glib_Rectangle(200,LCD_YSIZE_TFT_800600+200,LCD_XSIZE_TFT_800600-201,SCR_YSIZE_TFT_800600-201,0x0f);   

    // 00
    // 0#
    Glib_Rectangle(LCD_XSIZE_TFT_800600,LCD_YSIZE_TFT_800600,SCR_XSIZE_TFT_800600-1,SCR_YSIZE_TFT_800600-1,0xff);   
    Glib_Line(LCD_XSIZE_TFT_800600,LCD_YSIZE_TFT_800600,SCR_XSIZE_TFT_800600-1,SCR_YSIZE_TFT_800600-1,0xff);        
    Glib_Line(LCD_XSIZE_TFT_800600,SCR_YSIZE_TFT_800600-1,SCR_XSIZE_TFT_800600-1,LCD_YSIZE_TFT_800600,0xff);

    Uart_Printf("Virtual Screen Test(TFT 256 color). Press any key[ijkm\\r]!\n");
    MoveViewPort(MODE_TFT_8BIT_800600);
    Lcd_MoveViewPort(0,0,MODE_TFT_8BIT_800600);
//--------LCD FIFO interrupt test---------------------------------------------------START
    rLCDINTMSK|=3; // Mask LCD SUB Interrupt
    rINTMSK|=(BIT_LCD); // Mask LCD INT
//--------LCD FIFO interrupt test-----------------------------------------------------END
    Lcd_EnvidOnOff(0);
    Lcd_Port_Return();
}


void Test_Lcd_Tft_16Bit_800600(void)
{
    int i,j,k;

	Lcd_Start(MODE_TFT_16BIT_800600);
    Uart_Printf("[TFT 64K COLOR(16bit/1pixel) LCD TEST]\n");

    Glib_ClearScr(0, MODE_TFT_16BIT_800600);
    Glib_FilledRectangle(0,0,799,599,0xf800);    
    Uart_Printf("TFT 64K color mode test 1. Press any key!\n");
    Uart_Getch();  	

    Glib_ClearScr(0, MODE_TFT_16BIT_800600);
    Glib_Rectangle(0,0,799,479,0x07e0);   
    Glib_FilledRectangle(0,0,20,20,65535);   
    Glib_Rectangle(620,460,639,479,65535);   
    Glib_Line(0,0,639,479,0x1f);        
    Glib_Line(639,0,0,479,0xf800);
    Uart_Printf("TFT 64K color mode test 2. Press any key!\n");
    Uart_Getch();  	

    Glib_ClearScr(0, MODE_TFT_16BIT_800600);
    k=0;
    for(i=160;i<480;i+=20)
    	for(j=120;j<360;j+=15)
    	{
	    	while((rLCDCON5>>19)==2);
    	    Glib_FilledRectangle(i,j,i+19,j+14,k);
    	    k+=500;
    	}

    // #0		    
    // 00		    
    Glib_Rectangle(0,0,319,239,65535);   
    Glib_Line(0,0,319,239,65535);        
    Glib_Line(0,239,319,0,65535);

    // 0#
    // 00
    Glib_Rectangle(0+320,0,319+320,239,255);
    Glib_Line(0+320,0,319+320,239,255);        
    Glib_Line(0+320,239,319+320,0,255);

    // 00
    // #0
    Glib_Rectangle(0,0+240,319,239+240,255);
    Glib_Line(0,0+240,319,239+240,255);        
    Glib_Line(0,239+240,319,0+240,255);

    // 00
    // 0#
    Glib_Rectangle(0+320,0+240,319+320,239+240,255);
    Glib_Line(0+320,0+240,319+320,239+240,255);     
    Glib_Line(0+320,239+240,319+320,0+240,255);
    Glib_Rectangle(50+320,50+240,269+320,189+240,255);

    Uart_Printf("TFT 64K color mode test 3. Press any key!\n");
    Uart_Getch();  	

    Glib_ClearScr(0, MODE_TFT_16BIT_800600);
    k=0;
    for(i=320;i<960;i+=40)
    	for(j=240;j<720;j+=30)
    	{
    	    Glib_FilledRectangle(i,j,i+39,j+29,k);
    	    k+=500;
    	}

    // #0		    
    // 00		    
    Glib_Rectangle(0,0,639,479,65535);   
    Glib_Line(0,0,639,479,65535);        
    Glib_Line(0,479,639,0,65535);

    // 0#
    // 00
    Glib_Rectangle(0+640,0,639+640,479,65535);   
    Glib_Line(0+640,0,639+640,479,65535);        
    Glib_Line(0+640,479,639+640,0,65535);

    // 00
    // #0
    Glib_Rectangle(0,0+480,639,479+480,65535);   
    Glib_Line(0,0+480,639,479+480,65535);        
    Glib_Line(0,479+480,639,0+480,65535);

    // 00
    // 0#
    Glib_Rectangle(0+640,0+480,639+640,479+480,65535);   
    Glib_Line(0+640,0+480,639+640,479+480,65535);        
    Glib_Line(0+640,479+480,639+640,0+480,65535);
    Glib_Rectangle(50+640,50+480,639+640-50,479+480-50,65535);   

    Uart_Printf("Virtual Screen Test(TFT 64K color). Press any key[ijkm\\r]!\n");
    MoveViewPort(MODE_TFT_16BIT_800600);
    Lcd_MoveViewPort(0,0,MODE_TFT_16BIT_800600);
    Lcd_EnvidOnOff(0);
    Lcd_Port_Return();
}


void Test_Lcd_Tft_16Bit_800600_On(void)
{
    int i,j,k=0;

    Lcd_Start(MODE_TFT_16BIT_800600);

    Uart_Printf("\n[TFT LCD(800x600 ,16bpp) WILL BE TURNED ON EVEN DURING OTHER TEST]\n");
    Uart_Printf("If you'd like to off the TFT LCD, then excute other LCD test program,\n");     

    for(i=0;i<LCD_XSIZE_TFT_800600;i+=40)
		for(j=0;j<LCD_YSIZE_TFT_800600;j+=30)
		{ 
		    Glib_FilledRectangle(i,j,i+39,j+29,(k%65535));
		    k+=150;
		}
}

void Test_Lcd_Palette_Ram(void)
{
    int i;	
    U32 *palette;

    Uart_Printf("Palette ram test has started.\n");
    Uart_Printf("LCD Display may be stopped.\n");
    Lcd_EnvidOnOff(0);
//    Uart_Printf("VSTATUS : %x, HSTATUS : %x \n", (rLCDCON5&(15<<3))>>15, (rLCDCON5&(13<<3))>>13);
	
    palette=(U32 *)PALETTE;
    for(i=0;i<256;i++)
    {
		*palette++=i;
    }	
	
    palette=(U32 *)PALETTE;
    for(i=0;i<256;i++)
    {
		if(((*palette++)&0xffff) != i)
		Uart_Printf("Error is occured during palette ram testing at [0x%x]\n", i);
    }	

    Uart_Printf("Palette Ram Test OK!!!\n");
    Uart_Printf("Press any key to continue.\n");
    Uart_Getch();
}


void Test_Lcd_Underrun(void)
{
	Uart_Printf("Test heavy bus access by write buffer!!\n");

	Uart_Printf("Select LCD : 1-240x320(LCC3600)[D]   2-800x600(General)\n");
	if (Uart_Getch() == '2')
		Test_Lcd_Tft_16Bit_800600_On();	
	else Test_Lcd_Tft_8Bit_240320_On();	

	Uart_Printf("If you want to stop test and continue test, then Press any key!\n");
	
	while(1){
		    SDRAMtest();
		if(Uart_GetKey()) break;
   	}

}

