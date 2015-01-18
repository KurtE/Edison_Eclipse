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
#include "tft_button.h"
#include "tft_screen.h"
#include <string.h>

//=============================================================================
// Simple push button
//=============================================================================


//=============================================================================================================
//  Display Object: Button
//=============================================================================================================
TFTButton::TFTButton(uint16_t x, uint16_t y, uint16_t dx, uint16_t dy, uint16_t wBtnClr, uint16_t wHiClr, uint16_t color_text, std::string str, uint16_t wVal, bool fEnabled)  {
  // Save away all of the data...
  // first the base objects stuff...
  _x = x;
  _w =dx;
  _y = y;
  _h = dy;
  control_enabled_ = fEnabled;        // default to enabled...
  control_visible_ = false;

  _wBtnClr = wBtnClr;
  _wHiClr = wHiClr;
  color_text_ = color_text;
  _str = str;
  _wVal = wVal;
  control_logically_pressed_ = false;
}

//=============================================================================================================
// TFTButton::draw
//=============================================================================================================

void TFTButton::draw() {

	if (!control_visible_)
		return;	// don't draw if it is not currently displayed.

    // BUGBUG:: Hard coded font size... 10x16
    uint16_t wXText = _x + _w/2 - _str.size()*10/2;  // Could precalc and save also verify that text fits...
    uint16_t wYText = _y + _h/2 - 16/2;

   // printf("%d %d %d BTN %s\n", _x, _y, control_logically_pressed_, _str.c_str());
    if (control_logically_pressed_) {
        tft.fillRect(_x, _y, _w, _h, _wHiClr);
        tft.drawRect(_x+2, _y+2, _w-4, _h-4, _wBtnClr);

        // Now output graphic text...
        DisplayGraphicString(wXText, wYText , 3, color_text_, -1,  _str.c_str());

  }
  else {
        tft.fillRect(_x, _y, _w, _h, _wBtnClr);
        tft.drawRect(_x+2, _y+2, _w-4, _h-4, _wHiClr);

        // Now output graphic text...
        if (control_enabled_)
        	DisplayGraphicString(wXText, wYText , 3, color_text_, -1, _str.c_str());
        else
        	DisplayGraphicString(wXText, wYText , 3, tft.color565(0x80, 0x80, 0x80), -1, _str.c_str());  // BUGBUG hard coded...
    }
}


//=============================================================================================================
// TFTTextBox:: enable
//=============================================================================================================
void TFTButton::enable(bool fEnabled) {
	control_enabled_ = fEnabled;

	if (control_visible_)
		draw();	// update the display
}

//=============================================================================================================
// TFTTextBox:: ProcessTouch
//
// Assumes that we got a button down event, we pass in the X, Y from this event and it will check to see if the coordinates
// are within the object and process it. If the up happens in the same object then we will return the object ID, else we will
//
//=============================================================================================================
uint16_t TFTButton::processTouch(uint16_t x, uint16_t y) {


    if (!control_visible_ || !control_enabled_ || !((x >= _x) && (x <= (_x+_w)) && (y >= _y) && (y <= (_y+_h))))
        return 0xffff;        // Special value that says Not within the object...

    SetPressed(true);
    draw();    // Show it as depressed.

    // Now lets wait for the touch to release...
    while (ts.touched()) {
        TFTScreen::getTouchPoint(&x, &y);
    }

    SetPressed(false);
    draw();

    // And see if we were still inside the button
    if ((x >= _x) && (x <= (_x+_w)) && (y >= _y) && (y <= (_y+_h)))
        return _wVal;

    return 0xfffe;    // not on the button any more...
}
