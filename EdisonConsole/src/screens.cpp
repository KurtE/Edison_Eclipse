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
#include "screens.h"
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
SCREEN *g_pscreenCur = NULL;
extern void LaunchProgram(std::string strCmd);

//=============================================================================================================
//  Screen Constructor
//=============================================================================================================
SCREEN::SCREEN(uint16_t wClr, DISPOBJ **ppdisp, uint8_t cdisp, SCREEN *pscreenNext, SCREEN *pscreenPrev) {
	_wClr = wClr;
	_ppdisp = ppdisp;
	_cdisp = cdisp;
	_pscreenNext = pscreenNext;
	_pscreenPrev = pscreenPrev;
}

//=============================================================================================================
//  Screen Draw
//=============================================================================================================
void SCREEN::draw(void) {
	tft.fillScreen(_wClr);
	printf("Screen Draw called: %d\n", _cdisp);

	for(uint8_t i=0; i < _cdisp; i++) {
		//printf("OBJ %lx \n", (unsigned long)(void*)_ppdisp[i]);
		_ppdisp[i]->draw();
	}
}

//=============================================================================================================
//  Screen ProcessTouch
//=============================================================================================================
uint16_t SCREEN::processTouch(uint16_t x, uint16_t y) {
	uint16_t wTouch;
	for(uint8_t i=0; i < _cdisp; i++) {
		wTouch = _ppdisp[i]->processTouch(x, y);
		if (wTouch != 0xffff) {
			// We will handle some of the standard commands here.
			printf("Touch id %d\n", wTouch);
			switch (wTouch)
			{
			case CMD_PREV:
				g_pscreenCur = _pscreenPrev;
				g_pscreenCur->draw();
				return 0xffff;	// don't have subclass process.
			case CMD_NEXT:
				g_pscreenCur = _pscreenNext;
				g_pscreenCur->draw();
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
uint16_t SCREEN::idleTime(void) {
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


BUTTON g_btnup(270, 10, 48, 34, DCLR_BUTTON_GREY, DCLR_BUTTON_YELLOW, ILI9341_BLACK, "Prv", CMD_PREV);
BUTTON g_btndn(270, 55, 48, 34, DCLR_BUTTON_GREY, DCLR_BUTTON_YELLOW, ILI9341_BLACK, "Nxt", CMD_NEXT);
BUTTON g_btnok(270, 100, 48, 34, DCLR_BUTTON_GREY, DCLR_BUTTON_YELLOW, ILI9341_BLACK, "Ok", CMD_OK);
TEXT g_txtTitle(0, 0, 160, 16, -1, -1, ILI9341_RED, "Kurt's PhantomX", -1);

//=============================================================================
// Hex Input screen
//=============================================================================
static BUTTON _btn0(1, KPD_ROW1_Y,  KPD_BWIDTH, KPD_BHEIGHT, DCLR_BUTTON_GREY, ILI9341_GREEN, ILI9341_BLACK, "0", 0);
static BUTTON _btn1(1*KPD_DELTAX + 1, KPD_ROW1_Y,  KPD_BWIDTH, KPD_BHEIGHT, DCLR_BUTTON_GREY, ILI9341_GREEN, ILI9341_BLACK, "1", 1);
static BUTTON _btn2(2*KPD_DELTAX + 1, KPD_ROW1_Y,  KPD_BWIDTH, KPD_BHEIGHT, DCLR_BUTTON_GREY, ILI9341_GREEN, ILI9341_BLACK, "2", 2);
static BUTTON _btn3(3*KPD_DELTAX + 1, KPD_ROW1_Y,  KPD_BWIDTH, KPD_BHEIGHT, DCLR_BUTTON_GREY, ILI9341_GREEN, ILI9341_BLACK, "3", 3);
static BUTTON _btn4(4*KPD_DELTAX + 1, KPD_ROW1_Y,  KPD_BWIDTH, KPD_BHEIGHT, DCLR_BUTTON_GREY, ILI9341_GREEN, ILI9341_BLACK, "4", 4);
static BUTTON _btn5(5*KPD_DELTAX + 1, KPD_ROW1_Y,  KPD_BWIDTH, KPD_BHEIGHT, DCLR_BUTTON_GREY, ILI9341_GREEN, ILI9341_BLACK, "5", 5);
static BUTTON _btn6(6*KPD_DELTAX + 1, KPD_ROW1_Y,  KPD_BWIDTH, KPD_BHEIGHT, DCLR_BUTTON_GREY, ILI9341_GREEN, ILI9341_BLACK, "6", 6);
static BUTTON _btn7(7*KPD_DELTAX + 1, KPD_ROW1_Y,  KPD_BWIDTH, KPD_BHEIGHT, DCLR_BUTTON_GREY, ILI9341_GREEN, ILI9341_BLACK, "7", 7);

static BUTTON _btn8(1, KPD_ROW2_Y,  KPD_BWIDTH, KPD_BHEIGHT, DCLR_BUTTON_GREY, ILI9341_GREEN, ILI9341_BLACK, "8", 8);
static BUTTON _btn9(1*KPD_DELTAX + 1, KPD_ROW2_Y,  KPD_BWIDTH, KPD_BHEIGHT, DCLR_BUTTON_GREY, ILI9341_GREEN, ILI9341_BLACK, "9", 9);
static BUTTON _btna(2*KPD_DELTAX + 1, KPD_ROW2_Y,  KPD_BWIDTH, KPD_BHEIGHT, DCLR_BUTTON_GREY, ILI9341_GREEN, ILI9341_BLACK, "A",10);
static BUTTON _btnb(3*KPD_DELTAX + 1, KPD_ROW2_Y,  KPD_BWIDTH, KPD_BHEIGHT, DCLR_BUTTON_GREY, ILI9341_GREEN, ILI9341_BLACK, "B",11);
static BUTTON _btnc(4*KPD_DELTAX + 1, KPD_ROW2_Y,  KPD_BWIDTH, KPD_BHEIGHT, DCLR_BUTTON_GREY, ILI9341_GREEN, ILI9341_BLACK, "C",12);
static BUTTON _btnd(5*KPD_DELTAX + 1, KPD_ROW2_Y,  KPD_BWIDTH, KPD_BHEIGHT, DCLR_BUTTON_GREY, ILI9341_GREEN, ILI9341_BLACK, "D",13);
static BUTTON _btne(6*KPD_DELTAX + 1, KPD_ROW2_Y,  KPD_BWIDTH, KPD_BHEIGHT, DCLR_BUTTON_GREY, ILI9341_GREEN, ILI9341_BLACK, "D",14);
static BUTTON _btnf(7*KPD_DELTAX + 1, KPD_ROW2_Y,  KPD_BWIDTH, KPD_BHEIGHT, DCLR_BUTTON_GREY, ILI9341_GREEN, ILI9341_BLACK, "F",15);

DISPOBJ *_hexscreenobjs[] = {&_btn0,&_btn1, &_btn2, &_btn3, &_btn4, &_btn5, &_btn6, &_btn7,
		&_btn8, &_btn9, &_btna, &_btnb, &_btnc, &_btnd, &_btne, &_btnf,
		&g_btnup,&g_btndn, &g_btnok};

class HexScreen : public SCREEN {
public:
	HexScreen(uint16_t wClr, DISPOBJ **ppdisp, uint8_t cdisp, SCREEN* pscreenNext, SCREEN* pscreenPrev) :
		SCREEN(wClr, ppdisp, cdisp, pscreenNext, pscreenPrev) {};

	  virtual uint16_t processTouch(uint16_t x, uint16_t y);
};

uint16_t HexScreen::processTouch(uint16_t x, uint16_t y)
{
	uint16_t wTouch = SCREEN::processTouch(x, y);
	if (wTouch != 0xffff) {
		// TBD
	}
	return wTouch;
}

// will probably also define idle time...





//=============================================================================
// Define the global objects
//=============================================================================
// forward references
class RunScreen;
extern HexScreen hexscreen;
extern MainScreen mainscreen;
extern RunScreen runscreen;

HexScreen  hexscreen(ILI9341_RED, _hexscreenobjs, sizeof(_hexscreenobjs)/sizeof(_hexscreenobjs[0]),
		(SCREEN*)&mainscreen, (SCREEN*)&runscreen);

//==========================================================================
// Helper function to let main function have us setup for the main screen.
//=============================================================================
void InitScreens(void) {
	g_pscreenCur = (SCREEN*)&mainscreen;

}

//==========================================================================
// Helper function to let main function have us setup for the main screen.
//=============================================================================
void SwitchScreen(SCREEN *pscreenNew) {
	g_pscreenCur = pscreenNew;

	// And draw the new display.
	g_pscreenCur->draw();
}
