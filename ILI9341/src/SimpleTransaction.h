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

#ifndef SIMPLETRANSACTION_H_
#define SIMPLETRANSACTION_H_
#include <pthread.h>

extern pthread_mutex_t g_lockSPI;
void inline BeginTranscation() {
	pthread_mutex_lock(&g_lockSPI);

}
void inline EndTransaction() {
	pthread_mutex_unlock(&g_lockSPI);
}

// Simple functions to initialize the mutex and destroy it at the end if last usage of it.
extern void InitTransction();
extern void DestroyTransaction();

#endif /* SIMPLETRANSACTION_H_ */
