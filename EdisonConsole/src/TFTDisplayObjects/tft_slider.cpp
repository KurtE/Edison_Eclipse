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
#include "tft_slider.h"
#include <string.h>
//=============================================================================================================
//  Display Object: slider
//=============================================================================================================
TFTSlider::TFTSlider (uint16_t x, uint16_t y, uint16_t dx, uint16_t dy, uint16_t wClr, uint16_t wHiClr, uint16_t wBackClr, uint16_t wMin, uint16_t wMax, uint16_t wVal) {
  // first the base objects stuff...
  _x = x;
  _w = dx;
  _y = y;
  _h = dy;
  _fEnabled = true;        // default to enabled...
  _fVisible = false;
  _fVert = (dy > dx);

  // Now the slider specific fields
  _wClr = wClr;
  _wHiClr = wHiClr;
  _wBackClr = wBackClr;
  _fPressed = false;

  _wMin = wMin;
  _wMax = wMax;
  _wVal = wVal;
  mapValueToDisplayValue();
}

void TFTSlider::mapValueToDisplayValue(void) {
  if (_fVert)
    // In vertical we start to fill from bottom to top...
    _wDispVal = map(_wVal, _wMin, _wMax, _y+_h, _y+1);
  else
    _wDispVal = map(_wVal, _wMin, _wMax, _x+1, _x+_w);
}

uint16_t TFTSlider::mapDisplayValueToValue(uint16_t wXorY) {
  if (_fVert)
    return map(wXorY, _y+1, _y+_h, _wMax, _wMin);

  return map(wXorY, _x+1, _x+_w, _wMin, _wMax);
}


void TFTSlider::draw(void) {
	if (!_fVisible)
		return;	// don't draw if it is not currently displayed.

	// Start off real simple simple filled rect to where the current value is... May also have simple text value...
	//First lets draw the outline frame around our object...
	tft.drawRect(_x, _y, _w, _h, _wClr);
	if (_fVert) {
		// In vertical we start to fill from bottom to top...
		if ( _wDispVal > (_y+1))
			tft.fillRect(_x+1, _y+1, _w-2,  _wDispVal-_y, _wBackClr);
		if ( _wDispVal < (_y+_h-1))
			tft.fillRect(_x+1,  _wDispVal, _w-2, _h - (_wDispVal-_y), _wHiClr);
	} else {
		if ( _wDispVal > (_x+1))
			tft.fillRect(_x+1, _y+1,  _wDispVal-_x, _h-2, _wHiClr);
		if ( _wDispVal < (_x+_w-1))
			tft.fillRect( _wDispVal+1, _y+1, _w - (_wDispVal-_x), _h-2, _wBackClr);
	}
}

uint16_t TFTSlider::processTouch(uint16_t x, uint16_t y) {

	if (!_fVisible || !((x >= _x) && (x <= (_x+_w)) && (y >= _y) && (y <= (_y+_h))))
		return 0x0;        // Special value that says Not within the object...

	SetPressed(true);
	draw();    // Show it as depressed.

	// Now lets wait for the touch to release...
    while (ts.touched()) {
      GetTouchPoint(&x, &y);
      if (_fVert) {
    	  if ((y >= _y) && (y <= (_y+_h))) {
    		  _wDispVal = y;
    		  draw();    // try to redraw....
    	  }
      }
      else {
    	  if ((x >= _x) && (x <= (_x+_w))) {
    		  _wDispVal = x;
    		  draw();    // try to redraw....
    	  }
      }
	}
    while (!ts.bufferEmpty()) {
		GetTouchPoint(&x, &y);
	}

	SetPressed(false);

	// And see if we were still inside the button
	if ((x >= _x) && (x < (_x+_w)) && (y >= _y) && (y < (_y+_h))) {
		if (_fVert)
			SetValue(mapDisplayValueToValue(y));
		else
			SetValue(mapDisplayValueToValue(x));
		return 1;
	}

	// Otherwise make sure we are mapped back to the correct value...
	mapValueToDisplayValue();
	draw();
	return 0xffff;    // not on the button any more...
}


void TFTSlider::SetValue(uint16_t wVal) {
  if (wVal != _wVal) {
    _wVal = wVal;

    mapValueToDisplayValue();

    draw();    // redraw the slider
  }
}

