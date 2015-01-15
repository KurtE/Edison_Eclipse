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
// Main screen support functions.
//=============================================================================

#include "globals.h"
#include "tft_screen.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>


// Define what our current screen is.
TFTScreen *TFTScreen::_pScreenCur = NULL;

extern void LaunchProgram(std::string strCmd);

//==========================================================================
// Helper function to let main function have us setup for the main screen.
//=============================================================================
void TFTScreen::setCurrentScreen(TFTScreen *pscreenNew) {
	if (TFTScreen::curScreen()) {
		// Lets walk all of the objects in the old screen and tell them they are not active
		TFTScreen::curScreen()->setVisible(false);
	}
	TFTScreen::_pScreenCur = pscreenNew;
	TFTScreen::curScreen()->setVisible(true);

	// And draw the new display.
	TFTScreen::curScreen()->draw();
}

//=============================================================================================================
//  Screen Constructor
//=============================================================================================================
TFTScreen::TFTScreen(uint16_t wClr, TFTDisplayObject **ppdisp, uint8_t cdisp, TFTScreen *pscreenNext, TFTScreen *pscreenPrev) {
	_wClr = wClr;
	_ppdisp = ppdisp;
	_cdisp = cdisp;
	_pscreenNext = pscreenNext;
	_pscreenPrev = pscreenPrev;
}

//=============================================================================================================
//  Screen Draw
//=============================================================================================================
void TFTScreen::draw(void) {
	tft.fillScreen(_wClr);
	//printf("Screen Draw called: %d\n", _cdisp);

	for(uint8_t i=0; i < _cdisp; i++) {
		//printf("OBJ %lx \n", (unsigned long)(void*)_ppdisp[i]);
		_ppdisp[i]->draw();
	}
}

//=============================================================================================================
//  Set visible
//=============================================================================================================
void TFTScreen::setVisible(bool fVisible) {
	tft.fillScreen(_wClr);
	visible(false);	// first set our object visible

	// And set the state of all of the objects...
	for(uint8_t i=0; i < _cdisp; i++) {
		_ppdisp[i]->visible(fVisible);
	}
}

//=============================================================================================================
//  Screen ProcessTouch
//=============================================================================================================
uint16_t TFTScreen::processTouch(uint16_t x, uint16_t y) {
	uint16_t wTouch;
	for(uint8_t i=0; i < _cdisp; i++) {
		wTouch = _ppdisp[i]->processTouch(x, y);
		if (wTouch != 0xffff) {
			// We will handle some of the standard commands here.
			//printf("Touch id %d\n", wTouch);
			switch (wTouch)
			{
			case CMD_PREV:
				TFTScreen::setCurrentScreen(_pscreenPrev);
				return 0xffff;	// don't have subclass process.
			case CMD_NEXT:
				TFTScreen::setCurrentScreen(_pscreenNext);
				return 0xffff;	// don't have subclass process.
			}
			return wTouch;
		}
	}
	return 0xffff;
}

//=============================================================================================================
//  Screen IdleTime
//=============================================================================================================
uint16_t TFTScreen::idleTime(void) {
	return 50;	// delay time...
}



// Lets define our screens here.
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


TFTButton g_btnup(270, 10, 48, 34, DCLR_BUTTON_GREY, DCLR_BUTTON_YELLOW, ILI9341_BLACK, "Prv", CMD_PREV);
TFTButton g_btndn(270, 55, 48, 34, DCLR_BUTTON_GREY, DCLR_BUTTON_YELLOW, ILI9341_BLACK, "Nxt", CMD_NEXT);
TFTButton g_btnok(270, 100, 48, 34, DCLR_BUTTON_GREY, DCLR_BUTTON_YELLOW, ILI9341_BLACK, "Ok", CMD_OK);
TFTText g_txtTitle(0, 0, 160, 16, -1, -1, ILI9341_RED, "Kurt's PhantomX", -1);

//=============================================================================
// Define the global objects
//=============================================================================
// forward references
class TFTRunScreen;
extern TFTHexScreen g_hex_screen;
extern TFTMainScreen g_main_screen;
extern TFTRunScreen g_run_screen;

//==========================================================================
// Helper function to let main function have us setup for the main screen.
//=============================================================================
void InitScreens(void) {
	TFTScreen::setCurrentScreen((TFTScreen*)&g_main_screen);

}

