/// \file init.cpp Initialization function

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
#include <olimex-lpc2378-stk/init.h>

void initialize_LPC2378(void)
{

// PLL constants
#define	PLLCFG_MSEL     (24 - 1) // PLL multiplier - 1
#define	PLLCFG_NSEL     (2 - 1)	// PLL predivider - 1

	// Configure the System Control and Status register

	/* GPIOM = 1 => High speed GPIO in P0 and P1.
	 * EMC Reset Disable = 0.
	 * MCIPWR Active Level = 0 => MCIPWR low.
	 * OSCRANGE = 0 => XTAL between 1 MHz and 20 MHz.
	 * OSCEN = 1 => Enable main oscillator.
	 */

	SCS = 0x21;
	while(!(SCS & 1<<6)); // Wait for the oscillator to become ready

	// ----	Ajust PLL --------------------------------------------

	// Disable and disconnect PLL
	PLLCON = 0;
	PLLFEED = 0xAA;
	PLLFEED = 0x55;

	// Select the main oscillator as source for PLL

	CLKSRCSEL = 1;

	// Configure PLL.
	PLLCFG = (PLLCFG_NSEL<<16) | PLLCFG_MSEL;
	PLLFEED = 0xAA;
	PLLFEED = 0x55;

	// Enable PLL.
	PLLCON |= PLLCON_PLLE;
	PLLFEED = 0xAA;
	PLLFEED = 0x55;

	// Change the CPU clock divider
	CCLKCFG = 4 - 1; // 72 MHz

	// Change the USB module clock divider
	USBCLKCFG = 6 - 1; // 48 MHz

	// Wait for the PLL to lock
	while(!(PLLSTAT & PLLSTAT_PLOCK));

	// Transite to PLL clock
	PLLCON = PLLCON_PLLE | PLLCON_PLLC;
	PLLFEED = 0xAA;
	PLLFEED = 0x55;

	// ----	Ajust MAM --------------------------------------------

// Memory Accelerator Module (MAM) definitions
/* #define	MAMCR_Val	0x00000002 */
#define	MAMCR_Val	0x00000001  // MAM partially enabled
#define	MAMTIM_Val	0x00000003

	// Configure the Memory Accelerator Module (MAM).
	MAMCR = 0;
	MAMTIM = MAMTIM_Val;
	MAMCR = MAMCR_Val;

	// ----	Ajust memory mapping ----------------------------------

	// Memory mapping (when the interruption vectors are in RAM)
#define RAM_INTVEC 0
#if(RAM_INTVEC != 0)
	MEMMAP = 2;
#endif

}
