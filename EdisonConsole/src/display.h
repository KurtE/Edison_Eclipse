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


class DISPOBJ {
public:
  inline void    Enable(boolean fEnabled);

  virtual void draw(void)=0;
  virtual uint16_t processTouch(uint16_t x, uint16_t y)=0;

  inline uint16_t width() {return _w;};
  inline uint16_t height() {return _h;};
  friend class OLEDSGC;
protected:
  uint16_t    _x;             //TL
  uint16_t    _y;
  uint16_t    _w;
  uint16_t    _h;
boolean _fEnabled :
  1;
boolean _fPressed :
  1;
boolean _fTouchActive :
  1;
};
//-----------------------------------------------------------------------------
// Class Button
//-----------------------------------------------------------------------------
class BUTTON  :
public DISPOBJ {
public:
  BUTTON (uint16_t x, uint16_t y, uint16_t dx, uint16_t dy, uint16_t wBtnClr, uint16_t wHiClr, uint16_t wTextClr, std::string str, uint16_t wVal);
  virtual void draw(void);
  virtual uint16_t processTouch(uint16_t x, uint16_t y);

  // some button specific functions
  inline void SetPressed(boolean fPressed)  {
    _fPressed = fPressed;
  };
private:
  uint16_t _wBtnClr;
  uint16_t _wHiClr;
  uint16_t _wTextClr;
  std::string _str;
  uint16_t  _wVal;

  //        boolean _fPressed;
};

//-----------------------------------------------------------------------------
// Class TEXT
//-----------------------------------------------------------------------------
class TEXT  :
public DISPOBJ {
public:
  TEXT (uint16_t x, uint16_t y, uint16_t dx, uint16_t dy, uint32_t wClr, uint16_t wHiClr, uint16_t wTextClr, std::string str, uint16_t wVal);
  virtual void draw(void);
  virtual uint16_t processTouch(uint16_t x, uint16_t y);
private:
  uint32_t _wClr;
  uint16_t _wHiClr;
  uint16_t _wTextClr;
  std::string _str;
  uint16_t  _wVal;

  //        boolean _fPressed;
};

//-----------------------------------------------------------------------------
// Class SCROLLTEXT
//-----------------------------------------------------------------------------
class SCROLLTEXT  :
public DISPOBJ {
public:
  SCROLLTEXT (uint16_t x, uint16_t y, uint16_t dx, uint16_t dy, uint32_t wClr, uint16_t wTextClr, uint32_t wOutlineClr);
  virtual void draw(void);
  virtual uint16_t processTouch(uint16_t x, uint16_t y);
  void AddTextLine(unsigned char* psz);
private:
  uint32_t _wClr;
  uint16_t _wTextClr;
  uint32_t _wOutlineClr;

  //        boolean _fPressed;
};

//-----------------------------------------------------------------------------
// class SLIDER
//-----------------------------------------------------------------------------
class SLIDER  :
public DISPOBJ {
public:
  SLIDER (uint16_t x, uint16_t y, uint16_t dx, uint16_t dy, uint16_t wClr, uint16_t wHiClr, uint16_t wBackClr, uint16_t wMin, uint16_t wMax, uint16_t wVal);
  virtual void draw(void);
  virtual uint16_t processTouch(uint16_t x, uint16_t y);

  // some button specific functions
  inline uint16_t GetValue(void) {
    return _wVal;
  };
  void SetValue(uint16_t wVal);

  inline void SetPressed(boolean fPressed)  {
    _fPressed = fPressed;
  };
private:
  inline void MapValToDispVal(void);
  inline uint16_t MapDispValToVal(uint16_t wXorY);

  uint16_t _wClr;
  uint16_t _wHiClr;
  uint16_t _wBackClr;
  boolean _fPressed;
  boolean _fVert;        // is the slider vertical or horizontal?

  uint16_t _wMin;
  uint16_t _wMax;
  uint16_t _wVal;

  uint16_t _wDispVal;        // This is the split X or Y value of what the current value maps to..
};


#endif /* DISPLAY_H_ */
