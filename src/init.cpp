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

#include <olimex-lpc2378-stk/init.h>

/* =================================================================
 * Función:	Inicializar_LPC2378
 * -----------------------------------------------------------------
 * Utilidad:	Inicializar los relojes y configurar el LPC2378.
 *
 * Entrada:		Ninguna.
 *
 * Salida:		Ninguna.
 * =================================================================
 */
void Inicializar_LPC2378(void)
{

/* Constantes para el PLL
 */
#define	PLLCFG_MSEL     (24 - 1)// Multiplicador PLL - 1
#define	PLLCFG_NSEL     (2 - 1)	// Predivisor PLL - 1

	/* Configurar el System Control and Status register
	 */

	/* GPIOM = 1 => High speed GPIO en P0 y P1.
	 * EMC Reset Disable = 0.
	 * MCIPWR Active Level = 0 => MCIPWR low.
	 * OSCRANGE = 0 => XTAL entre 1 MHz y 20 MHz.
	 * OSCEN = 1 => Habilitar oscilador principal.
	 */

	SCS = 0x21;
	while(!(SCS & 1<<6)); // Esperar a que el oscilador esté preparado.

	/* ----	Ajustar PLL --------------------------------------------
	 */

	/* Deshabilitar y desconectar el PLL.
	 */
	PLLCON = 0;
	PLLFEED = 0xAA;
	PLLFEED = 0x55;

	/* Seleccionar el oscilador principal como fuente para el PLL.
	 */

	CLKSRCSEL = 1;

	/* Configurar el PLL.
	 */
	PLLCFG = (PLLCFG_NSEL<<16) | PLLCFG_MSEL;
	PLLFEED = 0xAA;
	PLLFEED = 0x55;

	/* Habilitar el PLL.
	 */
	PLLCON |= PLLCON_PLLE;
	PLLFEED = 0xAA;
	PLLFEED = 0x55;

	/* Cambiar el divisor de reloj de la CPU.
	 */
	CCLKCFG = 4 - 1; // 72 MHz

	/* Cambiar el divisor de reloj del módulo USB.
	 */
	USBCLKCFG = 6 - 1; // 48 MHz

	/* Esperar hasta que el PLL se enganche.
	 */
	while(!(PLLSTAT & PLLSTAT_PLOCK));

	/* Pasar al reloj PLL.
	 */
	PLLCON = PLLCON_PLLE | PLLCON_PLLC;
	PLLFEED = 0xAA;
	PLLFEED = 0x55;

	/* ----	Ajustar MAM --------------------------------------------
	 */

/* Definiciones del Memory Accelerator Module (MAM).
 */
/* #define	MAMCR_Val	0x00000002 */
#define	MAMCR_Val	0x00000001  /* MAM parcialmente habilitado. */
#define	MAMTIM_Val	0x00000003

	/* Configurar el Memory Accelerator Module (MAM).
	 */
	MAMCR = 0;
	MAMTIM = MAMTIM_Val;
	MAMCR = MAMCR_Val;

	/* ----	Ajustar mapeado de memoria -----------------------------
	 */

	/* Mapeado de memoria (cuando los vectores de interrupción están en RAM).
	 */
#define RAM_INTVEC 0
#if(RAM_INTVEC != 0)
	MEMMAP = 2;
#endif

}
