//=============================================================================
//Project Linux Eclipse projects
//Description: Phoenix software
//
//  This code is free software; you can redistribute it and/or
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
// Definitions for the different screen used in the Console application
//=============================================================================

#ifndef SCREENS_H_
#define SCREENS_H_
#include "display.h"


//-----------------------------------------------------------------------------
// Class Screen
//-----------------------------------------------------------------------------
class SCREEN :
public DISPOBJ {
public:
	SCREEN(uint16_t wClr, DISPOBJ **ppdisp, uint8_t cdisp, SCREEN *pscreenNext, SCREEN *pscreenPrev);
	  virtual void draw(void);
	  virtual uint16_t processTouch(uint16_t x, uint16_t y);
	  virtual uint16_t idleTime(void);				// Call when nothing touched to allow background stuff to happen...

protected:
  uint16_t _wClr;
  DISPOBJ **_ppdisp;
  uint8_t _cdisp;			// count of display objects
  SCREEN  *_pscreenNext;	// Next Screen
  SCREEN  *_pscreenPrev;	// Previous screen
};


// Define our Screens.
extern SCREEN *g_pscreenCur;
void InitScreens(void);
boolean GetTouchPoint(uint16_t *pwX, uint16_t *pwY);
#define CMD_OK	 200
#define CMD_PREV 204
#define CMD_NEXT 205

//=============================================================================
// Constants and objects used in multiple screens
//=============================================================================
#define KPD_BWIDTH     38 // kludge for X2 should be X1+width-1...
#define KPD_BHEIGHT    36
#define KPD_DELTAX     40
#define KPD_ROW1_Y     152
#define KPD_ROW2_Y     195

#define DCLR_BUTTON_GREY   0xc718
#define DCLR_BUTTON_GREEN    ILI9341_GREEN
#define DCLR_BUTTON_YELLOW   ILI9341_YELLOW


// Define the standard buttons...
extern BUTTON g_btnup;
extern BUTTON g_btndn;
extern BUTTON g_btnok;
extern TEXT g_txtTitle;

// Not sure the best way to setup forward references...
class RunScreen;
class HexScreen;
class MainScreen;

extern HexScreen hexscreen;
extern MainScreen mainscreen;
extern RunScreen runscreen;

extern void LaunchProgram(std::string strCmd);

#endif /* SCREENS_H_ */
