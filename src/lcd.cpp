/// \file lcd.cpp LCD functions

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

#include <targets/LPC2378.h>
#include <olimex-lpc2378-stk/lcd.h>
#include <olimex-lpc2378-stk/fonts.h>

void initialize_SSP0( void )
{
  unsigned int i, dummy;

  // Assign pins to SCK0, MOSI0 and MISO0
  PINSEL3 |= (3<<16) | (3<<14) | (3<<8);

  // The P1.21 pin controls the #CS signal of the LCD.
  // Set the pin as output
  FIO1DIR |= 1<<21;

  // Deactivate the #CS
  LCD_CS_1;

  // Set up power for the SSP0 module
  PCONP |= 1<<21;

  // Configuration

  SSP0CR1 = 0;
  SSP0IMSC = 0;
  SSP0DMACR = 0;
  SSP0CPSR = 8; // SSP0 clock divider
  SSP0CR0 = (9-1);
  SSP0CR1 |= SSP0CR1_SSE; // Enable SSP0

  // Empty the receiving buffer
  for (i = 0; i < 8; i++ )  dummy = SSP0DR;
}

void LCD_initialize_pwm_backlight( void )
{
  PINSEL3 = (PINSEL3 & ~(3<<20)) | 2<<20;
  PCONP |= 1<<6;
  PWM1TCR = PWM1TCR_Counter_Reset;

  PWM1MCR = PWM1MCR_PWMMR0R;
  PWM1PCR = PWM1PCR_PWMENA6;

  PWM1PR = 0;
  PWM1MR0 = 0xFF;
  PWM1LER |= PWM1LER_Enable_PWM_Match_0_Latch;
  PWM1MR6 = 0;
  PWM1LER |= PWM1LER_Enable_PWM_Match_6_Latch;
  PWM1TCR |= PWM1TCR_PWM_Enable;
  PWM1TCR &= ~PWM1TCR_Counter_Reset;
  PWM1TCR |= PWM1TCR_Counter_Enable;

  LCD_adjust_backlight( 0 ); /// Initially off
}

void LCD_adjust_backlight( unsigned char intensity )
{
  PWM1MR6 = intensity;
  PWM1LER |= PWM1LER_Enable_PWM_Match_6_Latch;
}

void LCD_command( unsigned char command )
{
  volatile unsigned char dummy;

  LCD_CS_0;

  while( !(SSP0SR & SSP0SR_TNF) );
  SSP0DR = command & 0xFF;
  while( SSP0SR & SSP0SR_BSY );
  dummy = SSP0DR;

  LCD_CS_1;
}

void LCD_datum( unsigned char datum )
{
  volatile unsigned char dummy;

  LCD_CS_0;

  while( !(SSP0SR & SSP0SR_TNF) );
  SSP0DR = datum | 0x100;
  while( SSP0SR & SSP0SR_BSY );
  dummy = SSP0DR;

  LCD_CS_1;
}

void LCD_maximum_backlight( void )
{
    LCD_adjust_backlight( 255 );
}

void LCD_turn_off_backlight( void )
{
    LCD_adjust_backlight( 0 );
}

void initialize_LCD( void )
{

    initialize_SSP0();

    /* Configure the P1.21 pin as output to handle the Chip Select
     * input pin of the LCD, which is low-level active
     */
    FIO1DIR |= 1<<21;

    /* Configure the P3.25 pin as output to handle the Reset input of
     * the LCD, which is low-level active
     */
    FIO3DIR |= 1<<25;

    LCD_initialize_pwm_backlight();

    LCD_maximum_backlight();

    // Make LCD hardware reset
    LCD_CS_0;
    LCD_RESET_0;
    delay( 10000 );
    LCD_RESET_1;
    delay( 10000 );
    LCD_CS_1;

    LCD_command( DISCTL );
    LCD_datum( 0x00 );
    LCD_datum( 0x20 );
    LCD_datum( 0x0C );
    LCD_datum( 0x00 );

    LCD_command( OSCON );
    delay( 10000);

    LCD_command( SLPOUT );

    LCD_command( VOLCTR );
    LCD_datum( 0x24 );
    LCD_datum( 0x03 );

    LCD_command( DISINV );

    LCD_command( COMSCN );
    LCD_datum( 0x01 );

    LCD_command( PWRCTR );
    LCD_datum( 0x0F );
    delay( 10000 );

    LCD_command( DATCTL );
    LCD_datum( 0x01 );
    LCD_datum( 0x00 );
    LCD_datum( 0x02 );

    LCD_command(DISON);
}

void LCD_print_character( char c, int x, int y, int size, int color, int background_color )
{
    extern const unsigned char FONT6x8[97][8];
    extern const unsigned char FONT8x8[97][8];
    extern const unsigned char FONT8x16[97][16];

    int i,j;
    unsigned int nCols;
    unsigned int nRows;
    unsigned int nBytes;
    unsigned char PixelRow;
    unsigned char Mask;
    unsigned int Word0;
    unsigned int Word1;
    unsigned char *pFont;
    unsigned char *pChar;
    unsigned char *FontTable[] = {
        (unsigned char *)FONT6x8,
        (unsigned char *)FONT8x8,
        (unsigned char *)FONT8x16
        };


    pFont = (unsigned char *)FontTable[size];

    nCols = *pFont;
    nRows = *(pFont + 1);
    nBytes = *(pFont + 2);

    pChar = pFont + (nBytes * (c - 0x1F)) + nBytes - 1;

    LCD_command( PASET );
    LCD_datum( x );
    LCD_datum( x + nRows - 1 );

    LCD_command(CASET);
    LCD_datum( y );
    LCD_datum( y + nCols - 1 );

    LCD_command( RAMWR );

    for (i = nRows - 1; i >= 0; i--)
    {

        PixelRow = *pChar--;

        Mask = 0x80;
        for (j = 0; j < nCols; j += 2)
        {
            if ((PixelRow & Mask) == 0)
                Word0 = background_color;
            else
                Word0 = color;

            Mask = Mask >> 1;

            if ((PixelRow & Mask) == 0)
                Word1 = background_color;
            else
                Word1 = color;

            Mask = Mask >> 1;

            LCD_datum( (Word0 >> 4) & 0xFF );
            LCD_datum( ( (Word0 & 0xF) << 4) | ( (Word1 >> 8) & 0xF ) );
            LCD_datum( Word1 & 0xFF );
        }
    }
}

void LCD_print_string( char *str, int x, int y, int size, int color, int background_color )
{

    while( *str)
    {

        LCD_print_character( *str++, x, y, size, color, background_color );

        if( size == SMALL_FONT )
          y = y + 6;
        else if( size == MEDIUM_FONT )
          y = y + 8;
        else
          y = y + 8;

        if( y > 131 ) break;
    }
}

void delay( volatile unsigned int t )
{
  while(t--);
}

void LCD_clear( void )
{
    unsigned long i;

    LCD_command(PASET);
    LCD_datum(0);
    LCD_datum(131);


    LCD_command(CASET);
    LCD_datum(0);
    LCD_datum(131);

    LCD_command(RAMWR);
    for(i = 0; i < ((131 * 131) / 2); i++)
    {
        LCD_datum( (WHITE >> 4) & 0xFF);
        LCD_datum( ( (WHITE & 0xF) << 4 ) | ( (WHITE >> 8) & 0xF ) );
        LCD_datum( WHITE & 0xFF );
    }
}

void LCD_pixel( int x, int y, int color )
{

    LCD_command( PASET );
    LCD_datum(x);
    LCD_datum(x);

    LCD_command( CASET );
    LCD_datum(y);
    LCD_datum(y);

    LCD_command(RAMWR);
    LCD_datum( (color >> 4) & 0xFF);
    LCD_datum(((color & 0xF) << 4) | ((color >> 8) & 0xF));
    LCD_datum(color & 0xFF);
}

void LCD_line( int x1, int y1, int x2, int y2, int color )
{
    int   d, dx, dy;
    int   Aincr, Bincr, xincr, yincr;
    int   x, y;
    int   temp;

    dx = x2 - x1;
    if( dx < 0 ) dx = -dx;

    dy = y2 - y1;
    if( dy < 0 ) dy = -dy;

    if( dx >= dy )
    {
        if (x1 > x2)
        {
            temp = x1;
            x1 = x2;
            x2 = temp;

            temp = y1;
            y1 = y2;
            y2 = temp;
        }

        if (y2 > y1)
            yincr = 1;
        else
            yincr = -1;

        d = 2 * dy - dx;

        Aincr = 2 * (dy - dx);
        Bincr = 2 * dy;

        x = x1;
        y = y1;

        LCD_pixel( x, y, color );

        for (x = x1 + 1; x <= x2; x++ )
        {
            if (d >= 0)
            {
                y += yincr;
                d += Aincr;
            }
            else
                d += Bincr;

            LCD_pixel( x, y, color );
        }
    }
    else
    {
        if (y1 > y2)
        {
            temp = x1;
            x1 = x2;
            x2 = temp;

            temp = y1;
            y1 = y2;
            y2 = temp;
        }

        if (x2 > x1)
            xincr = 1;
        else
            xincr = -1;

        d = 2 * dx - dy;

        Aincr = 2 * (dx - dy);
        Bincr = 2 * dx;

        x = x1;
        y = y1;

        LCD_pixel( x, y, color );

        for (y=y1+1; y<=y2; y++)
        {
            if (d >= 0)
            {
                x += xincr;
                d += Aincr;
            }
            else
                d += Bincr;

            LCD_pixel( x, y, color );
        }
    }
}

void LCD_rectangle(int x0, int y0, int x1, int y1, unsigned char fill, int color )
{
    int xmin, xmax, ymin, ymax;
    int i;

    if( fill )
    {

        xmin = (x0 <= x1) ? x0 : x1;
        xmax = (x0 > x1) ? x0 : x1;
        ymin = (y0 <= y1) ? y0 : y1;
        ymax = (y0 > y1) ? y0 : y1;

        LCD_command( PASET );
        LCD_datum( xmin );
        LCD_datum( xmax );

        LCD_command( CASET );
        LCD_datum( ymin );
        LCD_datum( ymax );

        LCD_command( RAMWR );

        for (i = 0; i < ((((xmax - xmin + 1) * (ymax - ymin + 1)) / 2) + 130); i++)
        {
            LCD_datum((color >> 4) & 0xFF);
            LCD_datum(((color & 0xF) << 4) | ((color >> 8) & 0xF));
            LCD_datum(color & 0xFF);
        }
    }
    else
    {
        LCD_line( x0, y0, x1, y0, color );
        LCD_line( x0, y1, x1, y1, color );
        LCD_line( x0, y0, x0, y1, color );
        LCD_line( x1, y0, x1, y1, color );
    }
}

void LCD_circumference( int x0, int y0, int radius, int color )
{
    int f = 1 - radius;
    int ddF_x = 0;
    int ddF_y = -2*radius;
    int x = 0;
    int y = radius;

    LCD_pixel( x0, y0 + radius, color );
    LCD_pixel( x0, y0 - radius, color );
    LCD_pixel( x0 + radius, y0, color );
    LCD_pixel( x0 - radius, y0, color );

    while( x < y )
    {
        if( f >= 0 )
        {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }

        x++;
        ddF_x += 2;
        f += ddF_x + 1;

        LCD_pixel( x0 + x, y0 + y, color );
        LCD_pixel( x0 - x, y0 + y, color );
        LCD_pixel( x0 + x, y0 - y, color );
        LCD_pixel( x0 - x, y0 - y, color );
        LCD_pixel( x0 + y, y0 + x, color );
        LCD_pixel( x0 - y, y0 + x, color );
        LCD_pixel( x0 + y, y0 - x, color );
        LCD_pixel( x0 - y, y0 - x, color );
    }
}
