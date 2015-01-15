//=============================================================================
//Project Linux Eclipse projects
//Description: Phoenix software
//
//  This code is free software; you can redistribute it and/or
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
// Simple Transaction calls for both the ILI9341 and the STMPE610 to fix
// issues associated with calling them from multiple threads.
//=============================================================================

#include "SimpleTransaction.h"
#include <stdint.h>
// Mutex to only allow one thing at a time to use SPI...  Sort of a big hammer...
// Hopefully no deadlocks...
pthread_mutex_t g_lockSPI;
static uint8_t g_bInitCalls = 0;

void InitTransaction() {
	g_bInitCalls++;
	if (g_bInitCalls == 1)
		pthread_mutex_init(&g_lockSPI, NULL);

}

void DestroyTransaction() {
	g_bInitCalls--;
	if(g_bInitCalls == 0)
		pthread_mutex_destroy(&g_lockSPI);

}



