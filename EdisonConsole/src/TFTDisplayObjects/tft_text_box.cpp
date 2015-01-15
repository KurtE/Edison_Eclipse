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
#include "tft_text_box.h"
#include <string.h>

//=============================================================================
// Simple scrollable text box
//=============================================================================

//=============================================================================================================
//  Display Object: TFTTextBox
//=============================================================================================================
 TFTTextBox::TFTTextBox(uint16_t x, uint16_t y, uint16_t dx, uint16_t dy, uint32_t wClr, uint16_t wTextClr, uint32_t wOutlineClr)  {
  // Save away all of the data...
  // first the base objects stuff...
  _x = x;
  _w = dx;
  _y = y;
  _h = dy;
  _fEnabled = true;        // default to enabled...
  _fVisible = false;

  _wClr = wClr;
  _wTextClr = wTextClr;
  _wOutlineClr = wOutlineClr;
  _cLines = 0;	// Count of lines currently in the list;
  _cLinesAdded = 0;
  _fBatchMode = 0;						// default not in group add mode
  _cLinesVisible = _h/CHAR_HEIGHT;		//
  _iTop = 0;							// index of the top one we will display
  _iAdd = 0;							// index of next one we will add
 }

 //=============================================================================================================
 //  Display Object: TFTTextBox - destructor
 //=============================================================================================================
TFTTextBox::~TFTTextBox() {
	_fVisible = false;	// don't display
	clear();			// and clear out our list.
}

 //=============================================================================================================
// TFTTextBox:: draw
//=============================================================================================================
void TFTTextBox::draw() {
	if(!_fVisible)
		return;
	if (_wClr != (uint32_t)-1)
		tft.fillRect(_x, _y, _w, _h, _wClr);
	if (_wOutlineClr != (uint32_t)-1)
		tft.drawRect(_x, _y, _w, _h, _wOutlineClr);

	// See if we have any Text to display...
	update(false);	// Leave the top where it was...

}

//=============================================================================================================
// TFTTextBox:: processTouch
//
// Assumes that we got a button down event, we pass in the X, Y from this event and it will check to see if the coordinates
// are within the object and process it. If the up happens in the same object then we will return the object ID, else we will
//=============================================================================================================
uint16_t TFTTextBox::processTouch(uint16_t x, uint16_t y) {
	return 0xffff;      // Special value that says did not handle touch
}

//=============================================================================================================
// TFTTextBox:: add
//=============================================================================================================
void TFTTextBox::add(char *psz) {
	uint8_t len = strlen(psz);

	char *pszNew = (char*)malloc(len);		// Allocate a new string
	if (!pszNew)
		return;	// we failed;
	strcpy(pszNew, psz);	// Save away the new string.


	if (_cLines == MAX_LINES)
		free(_astrs[_iAdd]);		// We filled our buffer, so free up string that used to be there.
	else
		_cLines++;					// Increment count of lines we have.

	_astrs[_iAdd++]	= pszNew;		// save our new string here

	if (_iAdd == MAX_LINES)
		_iAdd = 0;

	_cLinesAdded++;					// Increment how many lines we have added...

	// If we are not in batch mode call our update function to show the new line.
	if (!_fBatchMode && _fVisible)
		update(true);
}

//=============================================================================================================
// TFTTextBox:: add
//=============================================================================================================
void TFTTextBox::add(std::string str) {			// Add a text string using c++ strings
	add(str.c_str());		// simply call off with the c string...
}

//=============================================================================================================
// TFTTextBox:: update - This function is called by use if we are in the delay mode or internal to our
//		add functions to do the actual drawing on the screen...  This will take care of scrolling current
//		stuff up in the box and then drawing the new text lines.
//=============================================================================================================
void TFTTextBox::update(bool fScrolltoLast) {						// tell the control we are done adding stuff so update now.
	// Assume we are in the area (0, 205) - (110-314)

	if (_cLinesAdded == 0)
		return;	// nothing to update.
#ifdef USE_SCROLLING
	uint16_t awColors[320];
	// Pass one no scrolling off of the bottom yet.
	if (_cLinesAdded < _cLinesVisible) {
		// ? should I just redraw everythng?
	}

	for (int y=(_y+CHAR_HEIGHT+1); y < (_y+_h-2); y++) {
		tft.readRect(_x+1, y, _w-2, 1, awColors);  // try to read one row at a time...
		tft.writeRect(_x+1, y-CHAR_HEIGHT, _w-2, 1, awColors);  // try to read one row at a time...
	}
#endif

	tft.setTextSize(2);
    tft.setTextColor(_wTextClr, ILI9341_BLACK);

    int iTextLine = 0;	// assume we will draw the whole thing...
    if (fScrolltoLast) { 	// May need to update the top pointer...
		if (_cLines >= _cLinesVisible ) {
			if (_iAdd >= _cLinesVisible)
				_iTop = _iAdd-_cLinesVisible;
			else
				_iTop = MAX_LINES + _iAdd - _cLinesVisible;
		} else {
			iTextLine = _cLinesVisible - _cLines;
			if (_iAdd >= _cLines)
				_iTop = _iAdd-_cLines;
			else
				_iTop = MAX_LINES + _iAdd - _cLines;
		}
    }

	int y = _y + iTextLine*CHAR_HEIGHT + 2;	// figure out where to start drawing...
	int iLine = _iTop;
    for(; iTextLine < _cLinesVisible; iTextLine++) {
    	char *psz = _astrs[iLine++];
    	int iLen = strlen(psz);

    	tft.setCursor(_x+2,y);
    	tft.print((char*)psz);

    	// Blank out rest of line...
    	int x = _x+2 + iLen*CHAR_WIDTH;
    	if (x < (_x + _w -2))
    		tft.fillRect(x, y, (_x+_w-4)-x, CHAR_HEIGHT, ILI9341_BLACK);
    	if (iLine == MAX_LINES)
    		iLine = 0;
    	y += CHAR_HEIGHT;

    }
    _cLinesAdded = 0;
}

//=============================================================================================================
// TFTTextBox:: add
//=============================================================================================================
void TFTTextBox::clear() {						// clear the list
	// free any strings we may have allocated.
	for (int i=0; i < _cLines; i++) {
		free(_astrs[i]);
	}
	_cLines = 0;
	_iTop = 0;
	_iAdd = 0;	// make sure all counters are reset.
	if (_fVisible)
		tft.fillRect(_x+2, _y+2, _w-4, _h-4, ILI9341_BLACK);
}

//=============================================================================================================
// TFTTextBox:: add
//=============================================================================================================
void TFTTextBox::top(int iTop) {					// set the top line number
}
