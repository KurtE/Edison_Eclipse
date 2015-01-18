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
#include "tft_screen.h"
#include <string.h>

//=============================================================================
// Simple scrollable text box
//=============================================================================

//=============================================================================================================
//  Display Object: TFTTextBox
//=============================================================================================================
 TFTTextBox::TFTTextBox(uint16_t x, uint16_t y, uint16_t dx, uint16_t dy, uint32_t color_background, uint16_t color_text, uint32_t color_outline)  {
  // Save away all of the data...
  // first the base objects stuff...
  _x = x;
  _w = dx;
  _y = y;
  _h = dy;
  control_enabled_ = true;        // default to enabled...
  control_visible_ = false;

  color_background_ = color_background;
  color_text_ = color_text;
  color_outline_ = color_outline;
  count_lines_ = 0;	// Count of lines currently in the list;
  count_lines_added_ = 0;
  in_batch_mode_ = 0;						// default not in group add mode
  count_lines_visible_ = _h/CHAR_HEIGHT;		//
  index_top_ = 0;							// index of the top one we will display
  index_add_ = 0;							// index of next one we will add
 }

 //=============================================================================================================
 //  Display Object: TFTTextBox - destructor
 //=============================================================================================================
TFTTextBox::~TFTTextBox() {
	control_visible_ = false;	// don't display
	clear();			// and clear out our list.
}

 //=============================================================================================================
// TFTTextBox:: draw
//=============================================================================================================
void TFTTextBox::draw() {
	if(!control_visible_)
		return;
	if (color_background_ != (uint32_t)-1)
		tft.fillRect(_x, _y, _w, _h, color_background_);
	if (color_outline_ != (uint32_t)-1)
		tft.drawRect(_x, _y, _w, _h, color_outline_);

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
	if (!control_visible_ || !control_enabled_  || !((x >= _x) && (x <= (_x+_w)) && (y >= _y) && (y <= (_y+_h))))
		return 0x0;        // Special value that says Not within the object...
	if (count_lines_ <= count_lines_visible_)
		return 0;	// Not enough stuff in list to scroll.

//	pressed(true);
//	draw();    // Show it as depressed.
	// Now lets wait for the touch to release...
	// May do a couple different ways to do this.  The first is if the user presses near the top or bottom and holds there,
	// we might scroll up or down there like maybe one line every time unit.  If they click and then drag up or down may
	// scroll appropriately in the direction they do...

	uint16_t y_down = y;		// remember the position the touch went down.
//	uint16_t y_prev_move = y;	// remember if the user has moved plus or minus some delta from last detected move
	int delta_y_in_lines;		// how many lines up or down has the user moved finger?
//	unsigned long time_start = millis();
	int top_index_start = top();

	while (ts.touched()) {

      TFTScreen::getTouchPoint(&x, &y);
	  delta_y_in_lines = ((int)y - (int)y_down) / CHAR_HEIGHT;
//      if (abs(y-y_prev_move) > Y_DEAD_ZONE) {  // reset timer if we moved enough...
//    	  time_start = millis();
//    	  y_prev_move = y;
//      }
      // first pass simply try to scroll to new line...
      // It will take care of bounds for us and maybe redraw...
      top(top_index_start-delta_y_in_lines);
	}
    while (!ts.bufferEmpty()) {
		TFTScreen::getTouchPoint(&x, &y);
	}

	pressed(false);
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


	if (count_lines_ == MAX_LINES)
		free(text_pointer_array_[index_add_]);		// We filled our buffer, so free up string that used to be there.
	else
		count_lines_++;					// Increment count of lines we have.

	text_pointer_array_[index_add_++]	= pszNew;		// save our new string here

	if (index_add_ == MAX_LINES)
		index_add_ = 0;

	count_lines_added_++;					// Increment how many lines we have added...

	// If we are not in batch mode call our update function to show the new line.
	if (!in_batch_mode_ && control_visible_)
		update(true);
}


//=============================================================================================================
// TFTTextBox:: batchUpdateMode
//=============================================================================================================
void TFTTextBox::batchUpdateMode(uint8_t fGroupMode) {	// Are we in batch add mode?
	if (in_batch_mode_ != fGroupMode) {
		in_batch_mode_ = fGroupMode;	// save away new mode

		// and exited batch mode call the update
		if (!in_batch_mode_)
			update(true);
	}
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
void TFTTextBox::update(bool make_last_line_visible) {						// tell the control we are done adding stuff so update now.
	// If no lines, just bail
	if (count_lines_ == 0)
		return;	// nothing to display.

	// If we are not in the added new items mode, only draw if we have to.

	if (make_last_line_visible && (count_lines_added_ == 0))
		return;	// nothing to update.

	tft.setTextSize(2);
    tft.setTextColor(color_text_, ILI9341_BLACK);

    int iTextLine = 0;	// assume we will draw the whole thing...
    if (make_last_line_visible) { 	// May need to update the top pointer...
		if (count_lines_ >= count_lines_visible_ ) {
			if (index_add_ >= count_lines_visible_)
				index_top_ = index_add_-count_lines_visible_;
			else
				index_top_ = MAX_LINES + index_add_ - count_lines_visible_;
		} else {
			iTextLine = count_lines_visible_ - count_lines_;
			if (index_add_ >= count_lines_)
				index_top_ = index_add_-count_lines_;
			else
				index_top_ = MAX_LINES + index_add_ - count_lines_;
		}
    } else if (count_lines_ < count_lines_visible_){
    	iTextLine = count_lines_visible_ - count_lines_;
    }

	int y = _y + iTextLine*CHAR_HEIGHT + 2;	// figure out where to start drawing...
	int iLine = index_top_;
    for(; iTextLine < count_lines_visible_; iTextLine++) {
    	char *psz = text_pointer_array_[iLine++];
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
    count_lines_added_ = 0;
}

//=============================================================================================================
// TFTTextBox:: add
//=============================================================================================================
void TFTTextBox::clear() {						// clear the list
	// free any strings we may have allocated.
	for (int i=0; i < count_lines_; i++) {
		free(text_pointer_array_[i]);
	}
	count_lines_ = 0;
	index_top_ = 0;
	index_add_ = 0;	// make sure all counters are reset.
	if (control_visible_)
		tft.fillRect(_x+2, _y+2, _w-4, _h-4, ILI9341_BLACK);
}

//=============================================================================================================
// TFTTextBox:: add
//=============================================================================================================
void TFTTextBox::top(int iTop) {					// set the top line number
	int current_top_value = index_top_;

	if (count_lines_ <= count_lines_visible_)
		index_top_ = 0;	// ignore what is passed in and simply set zero
	else  {
		if (iTop < 0)
			index_top_ = iTop + MAX_LINES;
		else if (iTop > MAX_LINES)
			index_top_ = iTop - MAX_LINES;
		else
			index_top_ = iTop;
		if ((count_lines_ < MAX_LINES) && ((index_top_ + count_lines_visible_) > count_lines_))
			index_top_ = count_lines_ - count_lines_visible_;
	}

	if (index_top_ != current_top_value)
		update(false);	// call update which should just redraw inside...

}
