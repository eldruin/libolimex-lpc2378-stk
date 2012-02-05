/** \file lcd.h \brief LCD functions
 *
 * Functions to handle the LCD on the board.
 * The functions are valid to control any LCD with an
 * EPSON S1D15G10 controller but not for one based on the
 * Philips PCF8833 controller
 *
 * LCD connections:
 * \verbatim
   LCD        Microcontroller
   ------------------------
   SCK        P1.20/PWM1.2/SCK0
   DIO        Direct to P1.23/PWM1.4/MISO0
              through a 10K resistence to P1.24/PWM1.5/MOSI0
   #CS        P1.21/PWM1.3/SSEL0
   #RESET     P3.25/MAT0.0/PWM1.2 and 33k pull-up at 3.3 V
   Backlight  P1.26/PWM1.6/CAP0.0
   \endverbatim
 */


/* Copyright 2008 Victor Manuel Sánchez Corbacho.
 * Copyright 2012 Diego Barrios Romero.
 *
 * This file is part of the Olimex-LPC2378-STK library.
 *
 * Olimex-LPC2378-STK library is free software: you can redistribute it
 * and/or modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * Olimex-LPC2378-STK library is distributed in the hope that it will
 * be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Olimex-LPC2378-STK library.  If not, see
 * <http://www.gnu.org/licenses/>.
 */

#ifndef __LCD_H__
#define __LCD_H__


#define LCD_RESET_0 FIO3CLR = 1<<25
#define LCD_RESET_1 FIO3SET = 1<<25

#define LCD_CS_0 FIO1CLR = 1<<21
#define LCD_CS_1 FIO1SET = 1<<21

#define USE_PWM_BACKLIGHT 0

#define DISON 0xAF ///< Display on
#define DISOFF 0xAE ///< Display off
#define DISNOR 0xA6 ///< Normal display
#define DISINV 0xA7 ///< Inverse display
#define COMSCN 0xBB ///< Common scan direction
#define DISCTL 0xCA ///< Display control
#define SLPIN 0x95 ///< Sleep in
#define SLPOUT 0x94 ///< Sleep out
#define PASET 0x75 ///< Page address set
#define CASET 0x15 ///< Column address set
#define DATCTL 0xBC ///< Data scan direction, etc.
#define RGBSET8 0xCE ///< 256-color position set
#define RAMWR 0x5C ///< Writing to memory
#define RAMRD 0x5D ///< Reading from memory
#define PTLIN 0xA8 ///< Partial display in
#define PTLOUT 0xA9 ///< Partial display out
#define RMWIN 0xE0 ///< Read and modify write
#define RMWOUT 0xEE ///< End
#define ASCSET 0xAA ///< Area scroll set
#define SCSTART 0xAB ///< Scroll start set
#define OSCON 0xD1 ///< Internal oscillation on
#define OSCOFF 0xD2 ///< Internal oscillation off
#define PWRCTR 0x20 ///< Power control
#define VOLCTR 0x81 ///< Electronic volume control
#define VOLUP 0xD6 ///< Increment electronic control by 1
#define VOLDOWN 0xD7 ///< Decrement electronic control by 1
#define TMPGRD 0x82 ///< Temperature gradient set
#define EPCTIN 0xCD ///< Control EEPROM
#define EPCOUT 0xCC ///< Cancel EEPROM control
#define EPMWR 0xFC ///< Write into EEPROM
#define EPMRD 0xFD ///< Read from EEPROM
#define EPSRRD1 0x7C ///< Read register 1
#define EPSRRD2 0x7D ///< Read register 2
#define NOP 0x25 ///< NOP instruction

// 12-bit color definitions
#define WHITE 0xFFF
#define BLACK 0x000
#define RED 0xF00
#define GREEN 0x0F0
#define BLUE 0x00F
#define CYAN 0x0FF
#define MAGENTA 0xF0F
#define YELLOW 0xFF0
#define BROWN 0xB22
#define ORANGE 0xFA0
#define PINK 0xF6A

#define FILL 1 ///< Fill with the color
#define NO_FILL 0 ///< Do not fill with the color

#define SMALL_FONT 0 ///< Small font size
#define MEDIUM_FONT 1 ///< Medium font size
#define BIG_FONT 2 ///< Big font size

/** Turn 3 3-bit colors into a 12-bit color
 * \param r Red color [0-7]
 * \param g Green color [0-7]
 * \param b Blue color [0-7]
 */
#define RGB_COLOR(r, g, b) ((r<<8) | (g << 4) | b)

#ifdef __cplusplus
extern "C" {
#endif

  /// Initializes the LCD
  void initialize_LCD( void );

  /** Initialize the SSP0 interface that's used in the communication
   * with the LCD
   */
  void initialize_SSP0( void );

  /** Initialize the PWM channel connected to the backlight LEDs of
   * the LCD. The PWM can be used to adjust the intensity of the
   * backlight.
   */
  void LCD_initialize_pwm_backlight( void );

  /** Adjust the intensity of the backlight of the screen
   * \param intensity Intensity of the backlight
   */
  void LCD_adjust_backlight( unsigned char intensity );

  /** Send a command to the LCD
   * \param command Command to send
   */
  void LCD_command( unsigned char command );

  /** Send a datum to the LCD
   * param datum Datum to send
   */
  void LCD_datum( unsigned char datum );

  /// Adjust LCD backlight to the maximum
  void LCD_maximum_backlight( void );

  /// Turn off LCD backlight
  void LCD_turn_off_backlight( void );

  /// Clear the LCD screen by filling it with white color
  void LCD_clear( void );

  /** Draw a pixel in the LCD screen
   * \param x X coordinate
   * \param y Y coordinate
   * \param color Pixel color
   */
  void LCD_pixel(int x, int y, int color);

  /** Draw a line segment using the Bresenham algorithm
   * \param x0 X coordinate of the origin of the segment
   * \param y0 Y coordinate of the origin of the segment
   * \param x1 X coordinate of the end of the segment
   * \param y1 Y coordinate of the end of the segment
   * \param color Line color
   */
  void LCD_line(int x0, int y0, int x1, int y1, int color);

  /** Draw an optionally-filled rectangle
   * \param x0 X coordinate of a corner of the rectangle
   * \param y0 Y coordinate of a corner of the rectangle
   * \param x1 X coordinate of the opposite corner of the rectangle
   * \param y1 Y coordinate of the opposite corner of the rectangle
   * \param fill Fill in the rectangle: FILL, NO_FILL
   * \param color Rectangle perimeter and fill color
   */
  void LCD_rectangle(int x0, int y0, int x1, int y1, unsigned char fill, int color );

  /** Draw a circumference using the Bresenham algorithm
   * \param x0 X coordinate of the center
   * \param y0 Y coordinate of the center
   * \param radius Radius of the circumference
   * \param color Color of the circumference
   */
  void LCD_circumference( int x0, int y0, int radius, int color );

  /** Print a character in the LCD screen
   * \param c Character
   * \param x X coordinate
   * \param y Y coordinate
   * \param size Font size: SMALL_FONT, MEDIUM_FONT or BIG_FONT
   * \param color Character color
   * \param background_color Background color
   */
  void LCD_print_character( char c, int x, int y, int size, int color, int background_color );

  /** Print a string in the LCD screen
   * Specifically, it iterates over the string printing each character
   * with the LCD_print_character() function and updating the printing
   * coordinate.
   * \param str String pointer
   * \param x X coordinate
   * \param y Y coordinate
   * \param size Font size: SMALL_FONT, MEDIUM_FONT or BIG_FONT
   * \param color Character color
   * \param background_color Background color
   */
  void LCD_print_string( char *str, int x, int y, int size, int color, int background_color );

  /// Function to generate delays by software
  void delay( volatile unsigned int t );

#ifdef __cplusplus
};
#endif

#endif
