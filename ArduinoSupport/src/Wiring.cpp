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
//=============================================================================
//Kurts Port support library for linux boxes include Raspberry Pi
//=============================================================================
#include "ArduinoDefs.h"
#include <stdint.h>
#include <time.h>


long min(long a, long b)
{
    if (a <= b)
        return a;
    return b;
}


long max(long a, long b)
{
    if (a >= b)
        return a;
    return b;
}


unsigned long millis(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC,  &ts );
    return ( ts.tv_sec * 1000 + ts.tv_nsec / 1000000L );
}


unsigned long micros(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC,  &ts );
    return ( ts.tv_sec * 1000000L + ts.tv_nsec / 1000L );
}


extern "C" void __cxa_pure_virtual()
{
    while (1);
}


long map(long x, long in_min, long in_max, long out_min, long out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

