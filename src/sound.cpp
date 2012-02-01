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

#include <targets/LPC2378.h>
#include <olimex-lpc2378-stk/sound.h>

/*
 * ATENCIÓN: Si se está usando el fichero Philips_LPC230X_Startup.s
 * proporcionado por CrossStudio las declaraciones de los prototipos
 * de funciones de interrupción sólo incluirán el atributo
 *
 *         __attribute__ ((interrupt ("IRQ")))
 *
 * si se ha definido el símbolo VECTORED_IRQ_INTERRUPTS
 */

void ISR_Timer0( void ) __attribute__ ((interrupt ("IRQ")));
void habilitar_IRQ( void );

const unsigned short *ptr_sonido;
int contador_sonido;

void inicializar_reproduccion_sonido ( void )
{
  /* Configurar el pin P1.26 como salida del DAC. */
  PINSEL1 = (PINSEL1 & ~(3<<20) ) | (2<<20);
  /* Quitar el pull-up y el pull-down de P1.26 */
  PINMODE1 = (PINMODE1 & ~(3<<20) ) | (2<<20);

  /* Programar el Timer 0 para que provoque un evento "match" cada 125
   * us.
   */
  /* Resetear el timer 0 */
  T0TCR = T0TCR_Counter_Reset;
  T0TCR = 0;

  /* Seleccionar "interrupt on match" y "reset on match" con MR0 */
  T0MCR |= ( T0MCR_MR0I | T0MCR_MR0R );

  /* Programar una preescala de 18 en el T0PR. Esto hará que el timer
   * counter TC se incremente cada microsegundo ya que PCLK es 18 MHz.
   */
  T0PR = 18 - 1;

  /* Programar en le match register para medir 125 us. */
  T0MCR = 125 - 1;

  /* Configurar y habilitar la interrupción del Timer 0. */
  MEMMAP = 2; /* Mapear los vectores de excepción en RAM si el
                 programa se vuelca allí */

  /* Dirección de la función de interrupción */
  VICVectAddr4 = (unsigned long)ISR_Timer0;

  /* Ajustar la prioridad más baja: 15 */
  VICVectPriority4 = 15;

  /* Habilitar la interrupción del Timer 0 */
  VICIntEnable |= 1<<4;

  habilitar_IRQ();
}

void reproducir_sonido ( const unsigned short *sptr, int tamano )
{
  ptr_sonido = sptr;
  contador_sonido = tamano;
  /* Poner en marcha el timer. */
  T0TCR = T0TCR_Counter_Enable;
}

void ISR_Timer0 ( void ) /* __attribute__((interrupt ("IRQ"))) */
{
  T0IR = T0IR_MR0;

  DACR = (*ptr_sonido)<<6;
  ptr_sonido++;

  if ( --contador_sonido == 0 )
    T0TCR = 0;

  /*
   * ATENCIÓN: Si se está usando el fichero Philips_LPC230X_Startup.s
   * proporcionado por CrossStudio la siguiente línea sólo se pondrá
   * si se ha definido el símbolo VECTORED_IRQ_INTERRUPTS.
   */

  VICAddress = 0;
}

void habilitar_IRQ ( void )
{
  asm ("stmfd sp!,{r0}");
  asm ("mrs  r0,CPSR");
  asm ("bic  r0,r0,#0x80");
  asm ("msr  CPSR_c,r0");
  asm ("ldmfd sp!,{r0}");
}
