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
 
/*============================================================================
 * Fichero: lcd.h
 *----------------------------------------------------------------------------
 * Funciones para manejar el LCD de la placa LCD-2378-STK.
 * Las funciones son válidas para manejar un LCD con controlador
 * EPSON S1D15G10 y no para los basados en el controlador Philips PCF8833.
 *
 *============================================================================
 */

#include <targets/LPC2378.h>
#include <olimex-lpc2378-stk/lcd.h>
#include <olimex-lpc2378-stk/fonts.h>

/*============================================================================
 * Función: SSP0_inicializar
 *----------------------------------------------------------------------------
 * Utilidad: Inicializar el interfaz SSP0 que se usa en la comunicación
 *           con el LCD.
 *
 * Entrada: Ninguna.
 *
 * Salida:  Ninguna.
 *============================================================================
 */
void SSP0_inicializar( void )
{
  unsigned int i, dummy;

  /* Asignar pines a SCK0, MOSI0 y MISO0.
   */
  PINSEL3 |= (3<<16) | (3<<14) | (3<<8);
  
  /* El pin P1.21 maneja la señal #CS del LCD.
   * Configurar el pin como salida.
   */
  FIO1DIR |= 1<<21;

  /* Desactivar el #CS.
   */
  LCD_CS_1;

  /* Aplicar alimentación al periférico SSP0.
   */

  PCONP |= 1<<21;

  /* Configuración.
   */

  SSP0CR1 = 0;
  SSP0IMSC = 0;
  SSP0DMACR = 0;
  SSP0CPSR = 8;           /* Divisor para reloj del SSP0 */
  SSP0CR0 = (9-1);
  SSP0CR1 |= SSP0CR1_SSE; /* Habilitar el SSP0 */

  /* Vaciar el buffer de recepción.
   */

  for (i = 0; i < 8; i++ )  dummy = SSP0DR;
}

/*============================================================================
 * Función: LCD_inicializar_pwm_backlight
 *----------------------------------------------------------------------------
 * Utilidad: Inicializar el canal PWM conectado a los LEDs de retroiluminación
 *           del LCD. El PWM puede usarse para ajustar el nivel de retroilu-
 *           minación.
 *
 * Entrada: Ninguna.
 *
 * Salida:  Ninguna.
 *============================================================================
 */
void LCD_inicializar_pwm_backlight( void )
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

  LCD_ajustar_backlight( 0 ); /* Inicialmente apagado */
}

/*============================================================================
 * Función: LCD_ajustar_backlight
 *----------------------------------------------------------------------------
 * Utilidad: Ajustar la intensidad de la retroiluminación.
 *
 * Entrada: luz: nivel de iluminación.
 *
 * Salida:  Ninguna.
 *============================================================================
 */
void LCD_ajustar_backlight( unsigned char luz )
{
  PWM1MR6 = luz;
  PWM1LER |= PWM1LER_Enable_PWM_Match_6_Latch;
}

/*============================================================================
 * Función: LCD_comando
 *----------------------------------------------------------------------------
 * Utilidad: Enviar un comando al LCD.
 *
 * Entrada: comando: comando a enviar.
 *
 * Salida:  Ninguna.
 *============================================================================
 */
void LCD_comando( unsigned char comando )
{
  volatile unsigned char dummy;

  LCD_CS_0;

  while( !(SSP0SR & SSP0SR_TNF) );
  SSP0DR = comando & 0xFF;
  while( SSP0SR & SSP0SR_BSY );
  dummy = SSP0DR;  

  LCD_CS_1;
}

/*============================================================================
 * Función: LCD_dato
 *----------------------------------------------------------------------------
 * Utilidad: Enviar un dato al LCD.
 *
 * Entrada: dato: dato a enviar.
 *
 * Salida:  Ninguna.
 *============================================================================
 */
void LCD_dato( unsigned char dato )
{
  volatile unsigned char dummy;

  LCD_CS_0;

  while( !(SSP0SR & SSP0SR_TNF) );
  SSP0DR = dato | 0x100;
  while( SSP0SR & SSP0SR_BSY );
  dummy = SSP0DR;  

  LCD_CS_1;
}

/*============================================================================
 * Función: LCD_backlight_maximo
 *----------------------------------------------------------------------------
 * Utilidad: Ajustar la retroiluminación del LCD al máximo.
 *
 * Entrada: Ninguna.
 *
 * Salida:  Ninguna.
 *============================================================================
 */
void LCD_backlight_maximo( void )
{
    LCD_ajustar_backlight( 255 );
}

/*============================================================================
 * Función: LCD_apagar_backlight
 *----------------------------------------------------------------------------
 * Utilidad: Apagar la retroiluminación del LCD.
 *
 * Entrada: Ninguna.
 *
 * Salida:  Ninguna.
 *============================================================================
 */
void LCD_apagar_backlight( void )
{
    LCD_ajustar_backlight( 0 );
}

/*============================================================================
 * Función: LCD_inicializar
 *----------------------------------------------------------------------------
 * Utilidad: Inicilizar el LCD.
 *
 * Entrada: Ninguna.
 *
 * Salida:  Ninguna.
 *============================================================================
 */
void LCD_inicializar( void )
{

    SSP0_inicializar();

    /* Configurar pin P1.21 como salida para manejar la entrada
     * chip select del LCD, activa a nivel bajo.
     */
    FIO1DIR |= 1<<21;

    /* Configurar pin P3.25 como salida para manejar la entrada
     * de reset del LCD, activa a nivel bajo.
     */
    FIO3DIR |= 1<<25;

    LCD_inicializar_pwm_backlight();

    /* Ajustar la retroiluminación al máximo.
     */
    LCD_backlight_maximo();
    
    /* Provocar un reset por hardware del LCD.
     */
    LCD_CS_0;
    LCD_RESET_0;
    retardo( 10000 );
    LCD_RESET_1;
    retardo( 10000 );
    LCD_CS_1;

    LCD_comando( DISCTL );
    LCD_dato( 0x00 );
    LCD_dato( 0x20 );
    LCD_dato( 0x0C );
    LCD_dato( 0x00 );

    LCD_comando( OSCON );
    retardo( 10000);

    LCD_comando( SLPOUT );

    LCD_comando( VOLCTR );
    LCD_dato( 0x24 );
    LCD_dato( 0x03 );

    LCD_comando( DISINV );

    LCD_comando( COMSCN );
    LCD_dato( 0x01 );
	
    LCD_comando( PWRCTR );
    LCD_dato( 0x0F );
    retardo( 10000 );
	
    LCD_comando( DATCTL );
    LCD_dato( 0x01 );
    LCD_dato( 0x00 );
    LCD_dato( 0x02 );

    LCD_comando(DISON);
}

/*============================================================================
 * Función: LCD_imprimir_caracter
 *----------------------------------------------------------------------------
 * Utilidad: 
 *
 * Entrada: 
 *
 * Salida:  Ninguna.
 *============================================================================
 */
void LCD_imprimir_caracter( char c, int x, int y, int tamano, int color, int color_fondo )
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


    pFont = (unsigned char *)FontTable[tamano];

    nCols = *pFont;
    nRows = *(pFont + 1);
    nBytes = *(pFont + 2);

    pChar = pFont + (nBytes * (c - 0x1F)) + nBytes - 1;

    LCD_comando( PASET );
    LCD_dato( x );
    LCD_dato( x + nRows - 1 );

    LCD_comando(CASET);
    LCD_dato( y );
    LCD_dato( y + nCols - 1 );

    LCD_comando( RAMWR );

    for (i = nRows - 1; i >= 0; i--)
    {

        PixelRow = *pChar--;
    
        Mask = 0x80;
        for (j = 0; j < nCols; j += 2)
        {        
            if ((PixelRow & Mask) == 0)
                Word0 = color_fondo;
            else
                Word0 = color;

            Mask = Mask >> 1;
    
            if ((PixelRow & Mask) == 0)
                Word1 = color_fondo;
            else
                Word1 = color;
    
            Mask = Mask >> 1;
            
            LCD_dato( (Word0 >> 4) & 0xFF );
            LCD_dato( ( (Word0 & 0xF) << 4) | ( (Word1 >> 8) & 0xF ) );
            LCD_dato( Word1 & 0xFF );
        }
    }
}

/*============================================================================
 * Función: LCD_imprimir_cadena
 *----------------------------------------------------------------------------
 * Utilidad: 
 *
 * Entrada: 
 *
 * Salida:  Ninguna.
 *============================================================================
 */
void LCD_imprimir_cadena( char *str, int x, int y, int tamano, int color, int color_fondo )
{

    while( *str)
    {
  
        LCD_imprimir_caracter( *str++, x, y, tamano, color, color_fondo );
    
        if( tamano == FONT_PEQUENO )
          y = y + 6;
        else if( tamano == FONT_MEDIANO )
          y = y + 8;
        else
          y = y + 8;
              
        if( y > 131 ) break;
    }
}

/*============================================================================
 * Función: retardo
 *----------------------------------------------------------------------------
 * Utilidad: Función para generar retardos por software.
 *
 * Entrada: Ninguna.
 *
 * Salida:  Ninguna.
 *============================================================================
 */
void retardo( volatile unsigned int t )
{
  while(t--);
}

/*============================================================================
 * Función: LCD_borrar
 *----------------------------------------------------------------------------
 * Utilidad: Borrar la pantalla LCD rellenando toda la pantalla de color
 *           blanco.
 *
 * Entrada: Ninguna.
 *
 * Salida:  Ninguna.
 *============================================================================
 */
void LCD_borrar( void )
{
    unsigned long i;

    LCD_comando(PASET);
    LCD_dato(0);
    LCD_dato(131);


    LCD_comando(CASET);
    LCD_dato(0);
    LCD_dato(131);

    LCD_comando(RAMWR);
    for(i = 0; i < ((131 * 131) / 2); i++)
    {
        LCD_dato( (WHITE >> 4) & 0xFF);
        LCD_dato( ( (WHITE & 0xF) << 4 ) | ( (WHITE >> 8) & 0xF ) );
        LCD_dato( WHITE & 0xFF );
    }
}

/*============================================================================
 * Función: LCD_pixel
 *----------------------------------------------------------------------------
 * Utilidad: Dibujar un pixel en la pantalla LCD.
 *
 * Entrada: x: coordenada x.
 *          y: coordenada y.
 *          color: color para el pixel.
 *
 * Salida:  Ninguna.
 *============================================================================
 */
void LCD_pixel( int x, int y, int color )
{

    LCD_comando( PASET );
    LCD_dato(x);
    LCD_dato(x);

    LCD_comando( CASET );
    LCD_dato(y);
    LCD_dato(y);

    LCD_comando(RAMWR);
    LCD_dato( (color >> 4) & 0xFF);
    LCD_dato(((color & 0xF) << 4) | ((color >> 8) & 0xF));
    LCD_dato(color & 0xFF);
}

/*============================================================================
 * Función:   LCD_linea
 *----------------------------------------------------------------------------
 * Utilidad:  Dibujar un segmento de línea usando el algoritmo de Bresenham.
 *
 * Entrada:   x1: coordenada x del origen del segmento.
 *            y1: coordenada y del origen del segmento.
 *            x2: coordenada x del final del segmento.
 *            y2: coordenada y del final del segmento.
 *            color: color de la línea.
 *
 * Salida:
 *============================================================================
 */
void LCD_linea( int x1, int y1, int x2, int y2, int color )
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

/*============================================================================
 * Función:   LCD_rectangulo
 *----------------------------------------------------------------------------
 * Utilidad:  Dibujar un rectángulo opcionalmente relleno.
 *
 * Entrada:   x0, y0: coordenadas de una esquina del rectangulo.
 *            x1, y1: coordenadas de la esquina opuesta.
 *            relleno: 0 => no rellenar, 1 => rellenar
 *            color: color del contorno y de relleno.
 *
 * Salida:    Ninguna.
 *============================================================================
 */
void LCD_rectangulo(int x0, int y0, int x1, int y1, unsigned char relleno, int color )
{
    int xmin, xmax, ymin, ymax;
    int i;
    
    if( relleno )
    {

        xmin = (x0 <= x1) ? x0 : x1;
        xmax = (x0 > x1) ? x0 : x1;
        ymin = (y0 <= y1) ? y0 : y1;
        ymax = (y0 > y1) ? y0 : y1;
  
        LCD_comando( PASET );
        LCD_dato( xmin );
        LCD_dato( xmax );
  
        LCD_comando( CASET );
        LCD_dato( ymin );
        LCD_dato( ymax );
  
        LCD_comando( RAMWR );
  
        for (i = 0; i < ((((xmax - xmin + 1) * (ymax - ymin + 1)) / 2) + 130); i++)
        {
            LCD_dato((color >> 4) & 0xFF);
            LCD_dato(((color & 0xF) << 4) | ((color >> 8) & 0xF));
            LCD_dato(color & 0xFF);
        }
    }
    else
    {
        LCD_linea( x0, y0, x1, y0, color );
        LCD_linea( x0, y1, x1, y1, color );
        LCD_linea( x0, y0, x0, y1, color );
        LCD_linea( x1, y0, x1, y1, color );
    }
}

/*============================================================================
 * Función:   LCD_circunferencia
 *----------------------------------------------------------------------------
 * Utilidad:  Dibujar una circunderencia usando el algoritmo de Bresenham.
 *
 * Entrada:   x0, y0: coordenadas del centro.
 *            readio: radio de la circunferencia.
 *            color: color de la circunferencia.
 *
 * Salida:    Ninguna.
 *============================================================================
 */
void LCD_circunferencia( int x0, int y0, int radio, int color )
{
    int f = 1 - radio;
    int ddF_x = 0;
    int ddF_y = -2*radio;
    int x = 0;
    int y = radio;

    LCD_pixel( x0, y0 + radio, color );
    LCD_pixel( x0, y0 - radio, color );
    LCD_pixel( x0 + radio, y0, color );
    LCD_pixel( x0 - radio, y0, color );

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


