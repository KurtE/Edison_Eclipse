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

#ifndef TFT_SCREEN_H_
#define TFT_SCREEN_H_
#include "tft_display_object.h"
#include "tft_text_box.h"
#include "tft_button.h"
#include "tft_text.h"

//-----------------------------------------------------------------------------
// Class Screen
//-----------------------------------------------------------------------------
class TFTScreen :
public TFTDisplayObject {
public:
	TFTScreen(uint16_t color_background, TFTDisplayObject **ppdisp, uint8_t cdisp, TFTScreen *pscreenNext, TFTScreen *pscreenPrev);
	  virtual void draw(void);
	  virtual uint16_t processTouch(uint16_t x, uint16_t y);
	  virtual void setVisible(bool fVisible);
	  virtual uint16_t idleTime(void);				// Call when nothing touched to allow background stuff to happen...

	  static bool getTouchPoint(uint16_t *pwX, uint16_t *pwY);	// currently setup as static function so only one version...

static void setCurrentScreen(TFTScreen* pscreen);
static inline TFTScreen* curScreen(void) {return _pScreenCur;};
protected:
  uint16_t color_background_;
  TFTDisplayObject **_ppdisp;
  uint8_t _cdisp;			// count of display objects
  TFTScreen  *_pscreenNext;	// Next Screen
  TFTScreen  *_pscreenPrev;	// Previous screen

static TFTScreen *_pScreenCur; // Our current Screen;
};


// Define our Screens.
extern TFTScreen *g_pscreenCur;
void InitScreens(void);
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
extern TFTButton g_btnup;
extern TFTButton g_btndn;
extern TFTButton g_btnok;
extern TFTText g_txtTitle;

// Not sure the best way to setup forward references...
class TFTRunScreen;
class TFTHexScreen;
class TFTMainScreen;

extern TFTHexScreen g_hex_screen;
extern TFTMainScreen g_main_screen;
extern TFTRunScreen g_run_screen;

extern void LaunchProgram(std::string strCmd);

#endif /* TFT_SCREEN_H_ */
