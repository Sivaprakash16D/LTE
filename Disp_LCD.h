/*****************************************************************
 * Driver for  LCD Module (16 characters by 2 lines )
 4-Bit Data Line
*****************************************************************/

 /*
 *	LCD interface header file
 *	
 */




extern void Write2Lcd(unsigned char);	// Write2Lcd

extern void ClearLcd(void);				// Clear Lcd

/* write a string of characters to the LCD */

extern void lcd_puts(const char * s);

/* Go to the specified position */

extern void lcd_goto(unsigned char pos);
	
/* intialize the LCD - call before anything else */

extern void InitLCDDisplay(void);

extern void lcd_putch(char);

/*	Set the cursor position */

#define	lcd_cursor(x)	lcd_write(((x)&0x7F)|0x80)
