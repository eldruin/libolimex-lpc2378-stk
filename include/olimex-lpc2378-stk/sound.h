/// \file sound.h Sound functions

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

#ifndef __SOUND_H__
#define __SOUND_H__

/// Initialize sound playback
void initialize_sound_playback( void );

/** \brief Play a sound
 *
 * Given by an array of samples and the number of samples
 * to play. Specifically, it sets up the sound to be used and starts
 * the timmer.
 *
 * It's necesary to call initialize_sound_playback() function before
 * calling this function
 * \param sptr samples array pointer
 * \param samples number of samples
 */
void play_sound ( const unsigned short *sptr, int samples );

#endif
