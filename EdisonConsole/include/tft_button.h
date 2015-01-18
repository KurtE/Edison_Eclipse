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
// Simple push button
//=============================================================================

#ifndef BUTTON_H_
#define BUTTON_H_
#include "tft_display_object.h"

//-----------------------------------------------------------------------------
// Class Button
//-----------------------------------------------------------------------------
class TFTButton  :
public TFTDisplayObject {
public:
  TFTButton (uint16_t x, uint16_t y, uint16_t dx, uint16_t dy, uint16_t wBtnClr, uint16_t wHiClr, uint16_t color_text, std::string str, uint16_t wVal, bool fEnabled=true);
  virtual void draw(void);
  virtual uint16_t processTouch(uint16_t x, uint16_t y);
  virtual void  	enable(bool fEnabled) ;

  // some button specific functions
  inline void SetPressed(boolean fPressed)  {
    control_logically_pressed_ = fPressed;
  };
private:
  uint16_t _wBtnClr;
  uint16_t _wHiClr;
  uint16_t color_text_;
  std::string _str;
  uint16_t  _wVal;

  //        boolean control_logically_pressed_;
};






#endif /* BUTTON_H_ */
