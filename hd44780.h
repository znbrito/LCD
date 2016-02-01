/*****************************************************************************
Title  :   HD44780 Library
Author :   SA Development (edited by José Brito v1.12 in 17/1/2016)
Version:   1.11 (v1.12)
*****************************************************************************/

#ifndef HD44780_H
#define HD44780_H

#include "hd44780_settings.h"
#include "inttypes.h"

//LCD Constants for HD44780
#define LCD_CLR                 0    // DB0: clear display

#define LCD_HOME                1    // DB1: return to home position

#define LCD_ENTRY_MODE          2    // DB2: set entry mode
#define LCD_ENTRY_INC           1    // DB1: 1=increment, 0=decrement
#define LCD_ENTRY_SHIFT         0    // DB0: 1=display shift on

#define LCD_DISPLAYMODE         3    // DB3: turn LCD/cursor on
#define LCD_DISPLAYMODE_ON      2    // DB2: turn display on
#define LCD_DISPLAYMODE_CURSOR  1    // DB1: turn cursor on
#define LCD_DISPLAYMODE_BLINK   0    // DB0: blinking cursor

#define LCD_MOVE                4    // DB4: move cursor/display
#define LCD_MOVE_DISP           3    // DB3: move display (0-> cursor)
#define LCD_MOVE_RIGHT          2    // DB2: move right (0-> left)

#define LCD_FUNCTION            5    // DB5: function set
#define LCD_FUNCTION_8BIT       4    // DB4: set 8BIT mode (0->4BIT mode)
#define LCD_FUNCTION_2LINES     3    // DB3: two lines (0->one line)
#define LCD_FUNCTION_10DOTS     2    // DB2: 5x10 font (0->5x7 font)

#define LCD_CGRAM               6    // DB6: set CG RAM address
#define LCD_DDRAM               7    // DB7: set DD RAM address

#define LCD_BUSY                7    // DB7: LCD is busy

//LCD Display coordinates for the 2 line for 16 column display
/*
      Y
      |
     _________________________________
  	/								  \
X - | 0 1 2 3 4 5 6 7 8 9 A B C D E F |  Initial cursor position (x,y) = (0,0)
	| 1	 							  |
	\_________________________________/

*/

//Library functions

void lcd_init(); // initializes the LCD display, its cursor (initially invisible) and the I/O ports. This is the first function to be used in any program that uses this library
void lcd_command(uint8_t cmd); // function useful for changing specific bits in the data/command register to be send to the LCD controller. This function can be used to write other functions
void lcd_clrscr(); // clears the LCD display and sets the cursor to the (x,y) = (0,0) position without changing the cursor configurations (blinking and/or underline)
void lcd_home(); // sets the cursor to the (x,y) = (0,0) position without changing the cursor configurations (blinking and/or underline)
void lcd_goto(uint8_t pos); // sets the cursor position: (0x00) -> (x,y) = (0,0) ||| (0x0F) -> (x,y) = (15,0) ||| (0x40) -> (x,y) = (0,1) ||| (0x4F) -> (x,y) = (15,1)
void lcd_putc(char c); // writes the input char c in the cursor position
void lcd_puts(const char *s); // writes the input string (constant char) s in the cursor position
void lcd_puts_P(const char *progmem_s); // ??? (writes the input string [constant char] s in the flash memory of the HD44780 controller ???)
void lcd_blink_cursor(); // makes the cursor blink
void lcd_underline_cursor(); // underlines the cursor
void lcd_underline_blink_cursor(); // underlines the cursor and makes it blink
void lcd_no_cursor(); // turns off the cursor (it doesn't blink and/or isn't underlined)
void lcd_shift_cursor_right(); // makes a shift right to the cursor, i.e., moves the cursor position one column to the right
void lcd_shift_cursor_left(); // makes a shift left to the cursor, i.e., moves the cursor position one column to the left
void lcd_shift_display_right(); // makes a shift right to the display (both lines), i.e., moves what's written in it to the right
void lcd_shift_display_left();  // makes a shift left to the display (both lines), i.e., moves what's written in it to the left
void lcd_write_backwards(); // makes future strings or chars being written backwards, from right to left instead of the usual left to right
void lcd_write_normally(); // makes future strings or chars being written normally, from right to left (default). Only calls this function if you called function lcd_write_backwards() before!
void lcd_write_speed_slow(); // When called, future strings or chars are written slower. You can change this "slow" by changing the delay value in line 397 in the file "hd44780.c"
void lcd_write_speed_normal(); // When called, future strings or chars are written instantaneously (default). Only can this function if you called lcd_write_speed_slow() before!


#if RW_LINE_IMPLEMENTED==1
uint8_t lcd_getc();
#endif

#if (LCD_DISPLAYS>1)
void lcd_use_display(int ADisplay);
#endif

#endif

//Important Notes

/* -> Dont't forget to configure the file hd44780_settings_example.h according to your LCD I/O pins. After that, rename the file to hd44780_settings.h
   and copy it, this file and the hd44780.c file to your project folder;

   -> In the main program you always have to type #include "hd44780.h". In the main function type lcd_init(); to initilalize the LCD display and
   the I/O ports that you already have configured in the hd44780_settings.h file. After that you can start using the other functions of this library;

   ->Sometimes the first character that you write in the LCD display can be random (for unknown reasons to me). To correct that you can write a
   character and then clear it so that the first random character you type won't be seen. After that you can start typing the rest of your main program
   and include the other functions to type in the LCD display without any random character appearing. An example code to do this is

   lcd_init(); // initializes the LCD and the I/O ports
   lcd_putc('A'); // writes "A" in the LCD display
   lcd_clrscr(); // erases the "A" and places the cursor in its initiall position (x,y) = (0,0)

   -> In the lcd_write function you can change the velocity of the input texts by applying a delay in the beginning of the function, as it's shown
   in the hd44780.c file . This is useful for you to see what happens when you write something in the LCD display
*/
