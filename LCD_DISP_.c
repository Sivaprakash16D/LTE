#include <p30f6012A.h>
#include <string.h>
#include "LCD_DISP.h"

#define LD4  _RD7
#define LD3	 _RD6
#define LD2	 _RD5
#define LD1	 _RD4


#define LCD_RW _RD2
#define LCD_RS _RD1
#define LCD_CS _RD3


union {
	struct {
		unsigned char LcdBit0 : 1 ;
		unsigned char LcdBit1 : 1 ;
		unsigned char LcdBit2 : 1 ;
		unsigned char LcdBit3 : 1 ;
		unsigned char LcdBit4 : 1 ;
		unsigned char LcdBit5 : 1 ;
		unsigned char LcdBit6 : 1 ;
		unsigned char LcdBit7 : 1 ;
	} LCDbit ;
	unsigned char all ;
}LCDDATA ;




void InitLCDPortIO(void)
{
  
 
  TRISD	 &= 0xff00;		
 

}

/*

void LCD_Strobe(void)
{   
	LCD_CS=1;
	Delay_ms(1);
	LCD_CS=0;
}
void Write2Lcd(unsigned char LData2Write)
{
	Delay_ms(1);
	LCDDATA.all = LData2Write;

	LD1 = LCDDATA.LCDbit.LcdBit4;Nop();
	LD2 = LCDDATA.LCDbit.LcdBit5;Nop();
	LD3 = LCDDATA.LCDbit.LcdBit6;Nop();
	LD4 = LCDDATA.LCDbit.LcdBit7;Nop();
	
	
	LCD_Strobe();

	LD1 = LCDDATA.LCDbit.LcdBit0;Nop();
	LD2 = LCDDATA.LCDbit.LcdBit1;Nop();
	LD3 = LCDDATA.LCDbit.LcdBit2;Nop();
	LD4 = LCDDATA.LCDbit.LcdBit3;Nop();


	LCD_Strobe();


}


void ClearLcd(void)
{
	LCD_RS = 0;
	Write2Lcd(0x1);
	Delay_ms(2);
}


void InitLCDDisplay(void)
{

//initialises the display

	InitLCDPortIO();

	LCD_RW	=0;	Nop();
	
	LCD_RS	=0;	Nop();
    LCD_CS 	=0;	Nop();

	Delay_ms(15);	// wait 15mSec after power applied,


    LCDDATA.all = 0x30;
	LD1 = LCDDATA.LCDbit.LcdBit4; 	Nop();
	LD2 = LCDDATA.LCDbit.LcdBit5;	Nop();
	LD3 = LCDDATA.LCDbit.LcdBit6;	Nop();
	LD4 = LCDDATA.LCDbit.LcdBit7;	Nop();


	LCD_Strobe();
	
    Delay_ms(5);

	LCD_Strobe();
	Delay_ms(1);	

	LCD_Strobe();	
	Delay_ms(1);	

	

	LCDDATA.all = 0x20;
	LD1 = LCDDATA.LCDbit.LcdBit4; Nop();
	LD2 = LCDDATA.LCDbit.LcdBit5; Nop();
	LD3 = LCDDATA.LCDbit.LcdBit6; Nop();
	LD4 = LCDDATA.LCDbit.LcdBit7; Nop();	// Four bit mode


	LCD_Strobe();

    Write2Lcd(0x28);
	Write2Lcd(0x14);
	Write2Lcd(0x0C);
   // Write2Lcd(0xF);
	ClearLcd();			// Clear screen
	Write2Lcd(0x6); 	// Set entry Mode
	
}

void lcd_puts(const char * s)
{
	LCD_RS = 1;			// write characters
	Nop();
	LCD_RW = 0;
	Nop();
	while(*s)
	Write2Lcd(*s++);
}


// write one character to the LCD

void lcd_putch(char c)
{

	LCD_RS = 1;	Nop();		// write characters
	LCD_RW = 0; Nop();

	Write2Lcd( c );
}


// Go to the specified position


void lcd_goto(unsigned char pos)
{
	LCD_RS = 0;Nop();
	if(pos < 16) Write2Lcd(0x80 |pos);
	else Write2Lcd(0xc0 |(pos - 16));
}

*/

/*******************As per in MW5004***********************/

void LcdDisplay(char Mode,char *Buff)
{
	unsigned char i;
	
	LCD_RW=0;Nop();
	LCD_RS=0;Nop();

	if(Mode==TOP)
	LCDDATA.all = 0x80;
	else if(Mode==BOTTOM)
	LCDDATA.all = 0xc0;
	SendDataToLCD();

	for(i=0;i<16;i++)
	{
		LCD_RS=1;Nop();
		LCD_RW=0;Nop();

		if(Buff[i]=='\0')
			Buff[i]=' ';
		
		LCDDATA.all = Buff[i];
		SendDataToLCD();
		LCD_RS=0;Nop();
		LCD_RW=1;Nop();
	//	LCDDelay(25);
	}
}

/*
void ClearLcdDisplay(char Mode)
{
	char LcdBuff[32];

	memcpy(LcdBuff,"                ",16);
	memcpy(LcdBuff+16,"                ",16);
	if(Mode==TOP || Mode == TOP_BOTTOM)
		LcdDisplay(TOP,LcdBuff);
	if(Mode == BOTTOM || Mode == TOP_BOTTOM)
		LcdDisplay(BOTTOM,LcdBuff+16);

}	

*/
void SendDataToLCD()
{

	LD1 = LCDDATA.LCDbit.LcdBit4;Nop();
	LD2 = LCDDATA.LCDbit.LcdBit5;Nop();
	LD3 = LCDDATA.LCDbit.LcdBit6;Nop();
	LD4 = LCDDATA.LCDbit.LcdBit7;Nop();

	LCD_CS=1;
	LCDDelay(20);
	LCD_CS=0;
	LCDDelay(20);
	
	LD1 = LCDDATA.LCDbit.LcdBit0;Nop();
	LD2 = LCDDATA.LCDbit.LcdBit1;Nop();
	LD3 = LCDDATA.LCDbit.LcdBit2;Nop();
	LD4 = LCDDATA.LCDbit.LcdBit3;Nop();

	LCD_CS=1;
	LCDDelay(20);
	LCD_CS=0;
	LCDDelay(20);
}

void LCDDelay(unsigned int DVal)
{
	while(DVal-- > 0)
	{
		//Nop();
	}
}


void InitLCDDisplay(void)
{
	InitLCDPortIO();
//initialises the display
	LCD_RW	=0;	Nop();
	LCD_RS	=0;	Nop();
    LCD_CS 	=0;	Nop();
	Delay_ms(15);

	LCDDATA.all =0x30;
	LD1 = LCDDATA.LCDbit.LcdBit4;Nop();
	LD2 = LCDDATA.LCDbit.LcdBit5;Nop();
	LD3 = LCDDATA.LCDbit.LcdBit6;Nop();
	LD4 = LCDDATA.LCDbit.LcdBit7;Nop();

	LCD_CS=1;
	LCDDelay(100);
	LCD_CS=0;
	LCDDelay(100);

	LCDDATA.all =0x30;
	LD1 = LCDDATA.LCDbit.LcdBit4;Nop();
	LD2 = LCDDATA.LCDbit.LcdBit5;Nop();
	LD3 = LCDDATA.LCDbit.LcdBit6;Nop();
	LD4 = LCDDATA.LCDbit.LcdBit7;Nop();

	LCD_CS=1;
	LCDDelay(100);
	LCD_CS=0;
	LCDDelay(100);

	LCDDATA.all =0x30;
	LD1 = LCDDATA.LCDbit.LcdBit4;Nop();
	LD2 = LCDDATA.LCDbit.LcdBit5;Nop();
	LD3 = LCDDATA.LCDbit.LcdBit6;Nop();
	LD4 = LCDDATA.LCDbit.LcdBit7;Nop();
	LCD_CS=1;
	LCDDelay(100);
	LCD_CS=0;
	LCDDelay(100);

	LCDDATA.all =0x20;

	LD1 = LCDDATA.LCDbit.LcdBit4;Nop();
	LD2 = LCDDATA.LCDbit.LcdBit5;Nop();
	LD3 = LCDDATA.LCDbit.LcdBit6;Nop();
	LD4 = LCDDATA.LCDbit.LcdBit7;Nop();

	LCD_CS=1;
	LCDDelay(100);
	LCD_CS=0;
	LCDDelay(100);

	LCDDATA.all =0x28;
	SendDataToLCD();
	LCDDelay(200);

	LCDDATA.all =0x14;
	SendDataToLCD();
	LCDDelay(200);
	
	LCDDATA.all =0x0c;
	SendDataToLCD();
	LCDDelay(200);

	LCDDATA.all = 0x06;
	SendDataToLCD();
	LCDDelay(200);
	
	LCDDATA.all =0x01;
	SendDataToLCD();
	LCDDelay(200);
	
	Delay_ms(15);
	return;

}
