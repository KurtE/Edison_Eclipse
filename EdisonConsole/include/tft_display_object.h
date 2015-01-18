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
// Some Display Controls definition
//=============================================================================

#ifndef DISPLAY_H_
#define DISPLAY_H_

#include <stdint.h>
#include <stdarg.h>
#include <Adafruit_GFX.h>
#include <ArduinoDefs.h>
//#include <SPI.h>
#include <Adafruit_ILI9341.h>
#include <string>


class TFTDisplayObject {
public:
  virtual void  	enable(bool fEnabled) ;
  inline bool 		enabled(void) {return control_enabled_;};
  inline void		visible(bool fVisble) {control_visible_ = fVisble;};
  inline bool 		visible(void) {return control_visible_;};
  inline void		pressed(bool fPressed) {control_logically_pressed_ = fPressed;};


  virtual void 		draw(void)=0;
  virtual uint16_t  processTouch(uint16_t x, uint16_t y)=0;

  inline uint16_t width() {return _w;};
  inline uint16_t height() {return _h;};
  friend class OLEDSGC;
protected:
  uint16_t    _x;             //TL
  uint16_t    _y;
  uint16_t    _w;
  uint16_t    _h;
  bool 	      control_enabled_ : 1;
  bool 	      control_logically_pressed_ : 1;
  bool        control_visible_ : 1;
};


//-----------------------------------------------------------------------------
// Helper functions.
//-----------------------------------------------------------------------------
extern void DisplayGraphicString(word x, word y, byte bFont, word wColor, uint32_t ulBGColor, const char *pb);


#endif /* DISPLAY_H_ */
