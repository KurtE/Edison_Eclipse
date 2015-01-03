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
// SDRFunctions.cpp - A way to allow multiple things register to use the sound system.
//    will relese the others if necessary.
//======================================================================================
#include "SDRFunctions.h"

static SDRFunc g_aSDRFuncs[MAX_SDRFUNCS];
unsigned char  g_cSDRFuncs = 0;
unsigned char  g_iSCRFuncsCur = 0xff;

unsigned char SDRRegisterReleaseFunction(SDRFunc func)
{
    if (g_cSDRFuncs < MAX_SDRFUNCS)
    {
        g_aSDRFuncs[g_cSDRFuncs] = func;
        return (g_cSDRFuncs++);
    }
    return 0xff;
}

void SDRSetCurrent(unsigned char uID)
{
    if (uID != g_iSCRFuncsCur) 
    {
        for (unsigned char i = 0; i < g_cSDRFuncs; i++)
        {
            if (i != uID)
                g_aSDRFuncs[i]();
        }
        
        g_iSCRFuncsCur = uID;
    }    
}
