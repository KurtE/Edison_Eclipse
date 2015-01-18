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

#include "globals.h"
#include "tft_text.h"
#include <string.h>

//=============================================================================
// Simple scrollable text box
//=============================================================================

//=============================================================================================================
//  Display Object: Text
//=============================================================================================================
 TFTText::TFTText(uint16_t x, uint16_t y, uint16_t dx, uint16_t dy, uint32_t color_background, uint16_t wHiClr, uint16_t color_text, std::string str, uint16_t wVal)  {
  // Save away all of the data...
  // first the base objects stuff...
  _x = x;
  _w =dx;
  _y = y;
  _h = dy;
  control_enabled_ = true;        // default to enabled...
  control_visible_ = false;

  color_background_ = color_background;
  _wHiClr = wHiClr;
  color_text_ = color_text;
 _str = str;
  _wVal = wVal;
}


void TFTText::draw() {
	if (!control_visible_)
		return;	// don't draw if it is not currently displayed.

	if (color_background_ != (uint32_t)-1)
		tft.fillRect(_x, _y, _w, _h, color_background_);

	// Now output graphic text...
	DisplayGraphicString(_x, _y , 3, color_text_, -1, (const char*)(_str.c_str()));
}

// Assumes that we got a button down event, we pass in the X, Y from this event and it will check to see if the coordinates
// are within the object and process it. If the up happens in the same object then we will return the object ID, else we will
//
uint16_t TFTText::processTouch(uint16_t x, uint16_t y) {
	return 0xffff;      // Special value that says did not handle touch
}

