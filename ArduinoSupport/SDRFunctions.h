//=============================================================================
//Project Linux Eclipse projects
//Description: Phoenix software
//
//
// Arduino(ish) defines to get the Phoenix software to run on  Linux
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//
//=============================================================================
//======================================================================================
// SDRFunctions.h - A way to allow multiple things register to use the sound system.
//    will release the others if necessary.
//======================================================================================
#ifndef _SDRFUNCTIONS_H_
#define _SDRFUNCTIONS_H_

#define MAX_SDRFUNCS   3    // define how big our array should be 2 is all I know of...

typedef void (*SDRFunc)(void);
extern unsigned char SDRRegisterReleaseFunction(SDRFunc func);
extern void SDRSetCurrent(unsigned char uID);

#endif
