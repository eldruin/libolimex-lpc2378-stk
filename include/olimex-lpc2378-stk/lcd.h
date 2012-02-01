// LCD functions
//

/* Copyright 2008 Victor Manuel Sánchez Corbacho.
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

/* Conexiones LCD:
 *
 * LCD     Microcontrolador
 * ------------------------
 * SCK        P1.20/PWM1.2/SCK0
 * DIO        Directamente a P1.23/PWM1.4/MISO0
 *              a través de una resistencia de 10K a P1.24/PWM1.5/MOSI0
 * #CS        P1.21/PWM1.3/SSEL0
 * #RESET     P3.25/MAT0.0/PWM1.2 y pull-up de 33k a 3.3 V.
 * Backlight  P1.26/PWM1.6/CAP0.0
 */

#define LCD_RESET_0 FIO3CLR = 1<<25
#define LCD_RESET_1 FIO3SET = 1<<25

#define LCD_CS_0 FIO1CLR = 1<<21
#define LCD_CS_1 FIO1SET = 1<<21

#define USAR_PWM_BACKLIGHT 0

#define DISON 0xAF // Display on
#define DISOFF 0xAE // Display off
#define DISNOR 0xA6 // Normal display
#define DISINV 0xA7 // Inverse display
#define COMSCN 0xBB // Common scan direction
#define DISCTL 0xCA // Display control
#define SLPIN 0x95 // Sleep in
#define SLPOUT 0x94 // Sleep out
#define PASET 0x75 // Page address set
#define CASET 0x15 // Column address set
#define DATCTL 0xBC // Data scan direction, etc.
#define RGBSET8 0xCE // 256-color position set
#define RAMWR 0x5C // Writing to memory
#define RAMRD 0x5D // Reading from memory
#define PTLIN 0xA8 // Partial display in
#define PTLOUT 0xA9 // Partial display out
#define RMWIN 0xE0 // Read and modify write
#define RMWOUT 0xEE // End
#define ASCSET 0xAA // Area scroll set
#define SCSTART 0xAB // Scroll start set
#define OSCON 0xD1 // Internal oscillation on
#define OSCOFF 0xD2 // Internal oscillation off
#define PWRCTR 0x20 // Power control
#define VOLCTR 0x81 // Electronic volume control
#define VOLUP 0xD6 // Increment electronic control by 1
#define VOLDOWN 0xD7 // Decrement electronic control by 1
#define TMPGRD 0x82 // Temperature gradient set
#define EPCTIN 0xCD // Control EEPROM
#define EPCOUT 0xCC // Cancel EEPROM control
#define EPMWR 0xFC // Write into EEPROM
#define EPMRD 0xFD // Read from EEPROM
#define EPSRRD1 0x7C // Read register 1
#define EPSRRD2 0x7D // Read register 2
#define NOP 0x25 // NOP instruction

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

#define RELLENO 1
#define NO_RELLENO 0

#define FONT_PEQUENO 0
#define FONT_MEDIANO 1
#define FONT_GRANDE 2

#define COLOR_RGB(r, g, b) ((r<<8) | (g << 4) | b)

#ifdef __cplusplus
extern "C" {
#endif

void LCD_inicializar( void );
void SSP0_inicializar( void );
void LCD_inicializar_pwm_backlight( void );
void LCD_ajustar_backlight( unsigned char luz );
void LCD_comando( unsigned char comando );
void LCD_dato( unsigned char dato );
void LCD_backlight_maximo( void );
void LCD_apagar_backlight( void );
void LCD_borrar( void );
void LCD_pixel(int x, int y, int color);
void LCD_linea(int x0, int y0, int x1, int y1, int color);
void LCD_rectangulo(int x0, int y0, int x1, int y1, unsigned char fill, int color );
void LCD_circunferencia( int x0, int y0, int radio, int color );
void LCD_imprimir_caracter( char c, int x, int y, int tamano, int color, int color_fondo );
void LCD_imprimir_cadena( char *str, int x, int y, int tamno, int color, int color_fondo );
void retardo( volatile unsigned int t );

#ifdef __cplusplus
};
#endif

#endif
