/** \file sound.cpp Sound functions
 *
 * \warning If the Philips_LPC230X_Startup.s file provided by
 * CrossStudio is being used, the interruption function declarations
 * should only include the attribute\n\n
 *         __attribute__ ((interrupt ("IRQ")))\n\n
 * if the symbol VECTORED_IRQ_INTERRUPTS has been defined.
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

#include <targets/LPC2378.h>
#include <olimex-lpc2378-stk/sound.h>

/// Play a sample routine
void ISR_Timer0( void ) __attribute__ ((interrupt ("IRQ")));
/// Enable IRQs
void enable_IRQ( void );

/// Global samples array for access from the IRQ function
const unsigned short *samples_array;
/// Global sample counter for access from the IRQ function
int sample_counter;

void initialize_sound_playback ( void )
{
  // Configure the P1.26 pin as DAC output
  PINSEL1 = (PINSEL1 & ~(3<<20) ) | (2<<20);
  // Disable pull-up and pull-down on the P1.26 pin
  PINMODE1 = (PINMODE1 & ~(3<<20) ) | (2<<20);

  // Program the Timer 0 to throw a "match" every 125 us
  // Reset Timer 0
  T0TCR = T0TCR_Counter_Reset;
  T0TCR = 0;

  // Select "interrupt on match" and "reset on match" with MR0
  T0MCR |= ( T0MCR_MR0I | T0MCR_MR0R );

  // Program a prescaling of 18 in the T0PR. This will make the Timer
  // Counter (TC) increase every microsecond because PCLK is 18 MHz.
  T0PR = 18 - 1;

  // Program the match register to measure 125 us
  T0MCR = 125 - 1;

  // Configure and enable the Timer 0 interruption -----------------
  // Map the exception vectors in RAM if the program is loaded there
  MEMMAP = 2;

  // Interruption function adress
  VICVectAddr4 = (unsigned long)ISR_Timer0;

  // Set the lowest priority: 15
  VICVectPriority4 = 15;

  // Enable the Timer 0 interruption
  VICIntEnable |= 1<<4;

  enable_IRQ();
}

void play_sound ( const unsigned short *sptr, int samples )
{
  samples_array = sptr;
  sample_counter = samples;
  // Start the timmer
  T0TCR = T0TCR_Counter_Enable;
}

void ISR_Timer0 ( void ) /* __attribute__((interrupt ("IRQ"))) */
{
  T0IR = T0IR_MR0;

  DACR = (*samples_array)<<6;
  samples_array++;

  if ( --sample_counter == 0 )
    T0TCR = 0;

  /*
   * WARNING: if the Philips_LPC230X_Startup.s file provided by
   * CrossStudio is being used, the following line should be present
   * only if the symbol VECTORED_IRQ_INTERRUPTS has been defined.
   */

  VICAddress = 0;
}

void enable_IRQ ( void )
{
  asm ("stmfd sp!,{r0}");
  asm ("mrs  r0,CPSR");
  asm ("bic  r0,r0,#0x80");
  asm ("msr  CPSR_c,r0");
  asm ("ldmfd sp!,{r0}");
}
