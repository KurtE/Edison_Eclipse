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
// Simple  slider
//=============================================================================

#ifndef TFT_DISPLAY_SLIDER_H_
#define TFT_DISPLAY_SLIDER_H_
#include "tft_display_object.h"

//-----------------------------------------------------------------------------
// class TFTSlider
//-----------------------------------------------------------------------------
class TFTSlider  :
public TFTDisplayObject {
public:
  TFTSlider (uint16_t x, uint16_t y, uint16_t dx, uint16_t dy, uint16_t wClr, uint16_t wHiClr, uint16_t wBackClr, uint16_t wMin, uint16_t wMax, uint16_t wVal);
  virtual void draw(void);
  virtual uint16_t processTouch(uint16_t x, uint16_t y);

  // some button specific functions
  inline uint16_t GetValue(void) {
    return _wVal;
  };
  void SetValue(uint16_t wVal);

  inline void SetPressed(bool fPressed)  {
    _fPressed = fPressed;
  };
private:
  inline void mapValueToDisplayValue(void);
  inline uint16_t mapDisplayValueToValue(uint16_t wXorY);

  uint16_t _wClr;
  uint16_t _wHiClr;
  uint16_t _wBackClr;
  bool _fPressed;
  bool _fVert;        // is the slider vertical or horizontal?

  uint16_t _wMin;
  uint16_t _wMax;
  uint16_t _wVal;

  uint16_t _wDispVal;        // This is the split X or Y value of what the current value maps to..
};




#endif /* TFT_DISPLAY_SLIDER_H_ */
