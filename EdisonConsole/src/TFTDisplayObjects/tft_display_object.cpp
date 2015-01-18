///=============================================================================
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

#include "globals.h"
#include "tft_display_object.h"


//#############################################################################################################
//=============================================================================================================
//  Main display object implementation.
//=============================================================================================================
void  TFTDisplayObject::enable(bool fEnabled)  {
	control_enabled_ = fEnabled;
}


//=============================================================================================================
//  Display Helper functions.
//=============================================================================================================
//=============================================================================
// OLEDDisplayGraphicString
//=============================================================================
void DisplayGraphicString(word x, word y, byte bFont, word wColor, uint32_t ulBGColor, const char *pb) {
  tft.setCursor(x, y);
  tft.setTextSize(2);

  if (ulBGColor != (uint32_t)-1)
    tft.setTextColor(wColor, ulBGColor);
  else
    tft.setTextColor(wColor);

  tft.print(pb);
}



