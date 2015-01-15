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
// Simple scrollable text box
//=============================================================================
#ifndef TEXTBOX_H_
#define TEXTBOX_H_
#include "tft_display_object.h"

//-----------------------------------------------------------------------------
// Class TFTTextBox
//-----------------------------------------------------------------------------
class TFTTextBox  :
public TFTDisplayObject {
public:
	// From Display Object
	TFTTextBox (uint16_t x, uint16_t y, uint16_t dx, uint16_t dy, uint32_t wClr, uint16_t wTextClr, uint32_t wOutlineClr);
	~TFTTextBox();
  	virtual void draw(void);
  	virtual uint16_t processTouch(uint16_t x, uint16_t y);

  	// methods specific to TextBox
  	void add(char* psz);		// add a text string, using old style strings
  	void add(std::string str);			// Add a text string using c++ strings
  	void update(bool fScrolltoLast);	// tell the control we are done adding stuff so update now.
  	void batchUpdateMode(uint8_t fGroupMode = 0);		// Are we in batch add mode?
  	void clear();						// clear the list
  	int	 lines() {return _cLines;};						// count of items in the list.
  	void top(int iTop);					// set the top line number
  	int  top() {return _iTop;};							// return the top line number

  	static const int MAX_LINES = 100;
  	static const int CHAR_WIDTH = 12;
  	static const int CHAR_HEIGHT = 16;

private:

	uint32_t _wClr;
	uint16_t _wTextClr;
	uint32_t _wOutlineClr;
	uint16_t _iTop;							// Which one is at the top of the display
	uint8_t _cLinesVisible;					// how many lines can I display given the height.
	uint8_t _iAdd;							// index where we should add the next one...
	uint16_t _cLines;
	uint16_t _cLinesAdded; 					// How many lines were added;
	uint8_t _fBatchMode;					// Batch add Mode?
	char *_astrs[MAX_LINES];

	//        bool _fPressed;
};



#endif /* TEXTBOX_H_ */
