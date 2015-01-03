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
#include "display.h"


//#############################################################################################################

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

//=============================================================================
// GetTouchPoint - Get the touch status associated with our display
//=============================================================================
boolean GetTouchPoint(uint16_t *pwX, uint16_t *pwY)
{
   TS_Point p = ts.getPoint();

  // Scale from ~0->4000 to tft.width using the calibration #'s
  *pwX = map(p.y, TS_MINX, TS_MAXX, tft.width(), 0);
  *pwY = map(p.x, TS_MINY, TS_MAXY, 0, tft.height());
  return true;
}



//=============================================================================================================
//  Display Object Implementation.
//=============================================================================================================
inline void  DISPOBJ::Enable(boolean fEnabled) {
  _fEnabled = fEnabled;
}



//=============================================================================================================
//  Display Object: Button
//=============================================================================================================
BUTTON::BUTTON(uint16_t x, uint16_t y, uint16_t dx, uint16_t dy, uint16_t wBtnClr, uint16_t wHiClr, uint16_t wTextClr, std::string str, uint16_t wVal)  {
  // Save away all of the data...
  // first the base objects stuff...
  _x = x;
  _w =dx;
  _y = y;
  _h = dy;
  _fEnabled = true;        // default to enabled...

  _wBtnClr = wBtnClr;
  _wHiClr = wHiClr;
  _wTextClr = wTextClr;
  _str = str;
  _wVal = wVal;
  _fPressed = false;
}


void BUTTON::draw() {

    // BUGBUG:: Hard coded font size... 10x16
    uint16_t wXText = _x + _w/2 - _str.size()*10/2;  // Could precalc and save also verify that text fits...
    uint16_t wYText = _y + _h/2 - 16/2;

    printf("%d %d %d BTN %s\n", _x, _y, _fPressed, _str.c_str());
    if (_fPressed) {
        tft.fillRect(_x, _y, _w, _h, _wHiClr);
        tft.drawRect(_x+2, _y+2, _w-4, _h-4, _wBtnClr);

        // Now output graphic text...
        DisplayGraphicString(wXText, wYText , 3, _wTextClr, -1,  _str.c_str());

  }
  else {
        tft.fillRect(_x, _y, _w, _h, _wBtnClr);
        tft.drawRect(_x+2, _y+2, _w-4, _h-4, _wHiClr);

        // Now output graphic text...
       DisplayGraphicString(wXText, wYText , 3, _wTextClr, -1, _str.c_str());
    }
}

// Assumes that we got a button down event, we pass in the X, Y from this event and it will check to see if the coordinates
// are within the object and process it. If the up happens in the same object then we will return the object ID, else we will
//
uint16_t BUTTON::processTouch(uint16_t x, uint16_t y) {


    if (!((x >= _x) && (x <= (_x+_w)) && (y >= _y) && (y <= (_y+_h))))
        return 0xffff;        // Special value that says Not within the object...

    SetPressed(true);
    draw();    // Show it as depressed.

    // Now lets wait for the touch to release...
    while (ts.touched()) {
        GetTouchPoint(&x, &y);
    }

    SetPressed(false);
    draw();

    // And see if we were still inside the button
    if ((x >= _x) && (x <= (_x+_w)) && (y >= _y) && (y <= (_y+_h)))
        return _wVal;

    return 0xfffe;    // not on the button any more...
}

//=============================================================================================================
//  Display Object: Text
//=============================================================================================================
 TEXT::TEXT(uint16_t x, uint16_t y, uint16_t dx, uint16_t dy, uint32_t wClr, uint16_t wHiClr, uint16_t wTextClr, std::string str, uint16_t wVal)  {
  // Save away all of the data...
  // first the base objects stuff...
  _x = x;
  _w =dx;
  _y = y;
  _h = dy;
  _fEnabled = true;        // default to enabled...

  _wClr = wClr;
  _wHiClr = wHiClr;
  _wTextClr = wTextClr;
 _str = str;
  _wVal = wVal;
}


void TEXT::draw() {

	if (_wClr != (uint32_t)-1)
		tft.fillRect(_x, _y, _w, _h, _wClr);

	// Now output graphic text...
	DisplayGraphicString(_x, _y , 3, _wTextClr, -1, (const char*)(_str.c_str()));
}

// Assumes that we got a button down event, we pass in the X, Y from this event and it will check to see if the coordinates
// are within the object and process it. If the up happens in the same object then we will return the object ID, else we will
//
uint16_t TEXT::processTouch(uint16_t x, uint16_t y) {
	return 0xffff;      // Special value that says did not handle touch
}

//=============================================================================================================
//  Display Object: SCROLLText
//=============================================================================================================
 SCROLLTEXT::SCROLLTEXT(uint16_t x, uint16_t y, uint16_t dx, uint16_t dy, uint32_t wClr, uint16_t wTextClr, uint32_t wOutlineClr)  {
  // Save away all of the data...
  // first the base objects stuff...
  _x = x;
  _w = dx;
  _y = y;
  _h = dy;
  _fEnabled = true;        // default to enabled...

  _wClr = wClr;
  _wTextClr = wTextClr;
  _wOutlineClr = wOutlineClr;
}


void SCROLLTEXT::draw() {

	if (_wClr != (uint32_t)-1)
		tft.fillRect(_x, _y, _w, _h, _wClr);
	if (_wOutlineClr != (uint32_t)-1)
		tft.drawRect(_x, _y, _w, _h, _wOutlineClr);
}

// Assumes that we got a button down event, we pass in the X, Y from this event and it will check to see if the coordinates
// are within the object and process it. If the up happens in the same object then we will return the object ID, else we will
//
uint16_t SCROLLTEXT::processTouch(uint16_t x, uint16_t y) {
	return 0xffff;      // Special value that says did not handle touch
}

void SCROLLTEXT::AddTextLine(unsigned char *psz) {
	// Assume we are in the area (0, 205) - (110-314)
	uint16_t awColors[320];

	for (int y=(_y+17); y < (_y+_h-2); y++) {
		tft.readRect(_x+1, y, _w-2, 1, awColors);  // try to read one row at a time...
		tft.writeRect(_x+1, y-16, _w-2, 1, awColors);  // try to read one row at a time...
	}
	tft.setTextSize(2);
    tft.setTextColor(_wTextClr);

	tft.fillRect(_x+2, _y+_h-19, _w-4, 16, ILI9341_BLACK);
	tft.setCursor(_x+2,_y+_h-19);
	tft.print((char*)psz);
}


//=============================================================================================================
//  Display Object: slider
//=============================================================================================================
SLIDER::SLIDER (uint16_t x, uint16_t y, uint16_t dx, uint16_t dy, uint16_t wClr, uint16_t wHiClr, uint16_t wBackClr, uint16_t wMin, uint16_t wMax, uint16_t wVal) {
  // first the base objects stuff...
  _x = x;
  _w = dx;
  _y = y;
  _h = dy;
  _fEnabled = true;        // default to enabled...
  _fVert = (dy > dx);

  // Now the slider specific fields
  _wClr = wClr;
  _wHiClr = wHiClr;
  _wBackClr = wBackClr;
  _fPressed = false;

  _wMin = wMin;
  _wMax = wMax;
  _wVal = wVal;
  MapValToDispVal();
}

void SLIDER::MapValToDispVal(void) {
  if (_fVert)
    // In vertical we start to fill from bottom to top...
    _wDispVal = map(_wVal, _wMin, _wMax, _y+_h, _y+1);
  else
    _wDispVal = map(_wVal, _wMin, _wMax, _x+1, _x+_w);
}

uint16_t SLIDER::MapDispValToVal(uint16_t wXorY) {
  if (_fVert)
    return map(wXorY, _y+1, _y+_h, _wMax, _wMin);

  return map(wXorY, _x+1, _x+_w, _wMin, _wMax);
}


void SLIDER::draw(void) {
  // Start off real simple simple filled rect to where the current value is... May also have simple text value...
  //First lets draw the outline frame around our object...
  tft.drawRect(_x, _y, _w, _h, _wClr);
  if (_fVert) {
    // In vertical we start to fill from bottom to top...
    if ( _wDispVal > (_y+1))
      tft.fillRect(_x+1, _y+1, _w-2,  _wDispVal-_y, _wBackClr);
    if ( _wDispVal < (_y+_h-1))
      tft.fillRect(_x+1,  _wDispVal, _w-2, _h - (_wDispVal-_y), _wHiClr);
  }
  else {
    if ( _wDispVal > (_x+1))
      tft.fillRect(_x+1, _y+1,  _wDispVal-_x, _h-2, _wHiClr);
    if ( _wDispVal < (_x+_w-1))
      tft.fillRect( _wDispVal+1, _y+1, _w - (_wDispVal-_x), _h-2, _wBackClr);
  }
}

uint16_t SLIDER::processTouch(uint16_t x, uint16_t y) {

	if (!((x >= _x) && (x <= (_x+_w)) && (y >= _y) && (y <= (_y+_h))))
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
			SetValue(MapDispValToVal(y));
		else
			SetValue(MapDispValToVal(x));
		return 1;
	}

	// Otherwise make sure we are mapped back to the correct value...
	MapValToDispVal();
	draw();
	return 0xffff;    // not on the button any more...
}


void SLIDER::SetValue(uint16_t wVal) {
  if (wVal != _wVal) {
    _wVal = wVal;

    MapValToDispVal();

    draw();    // redraw the slider
  }
}



