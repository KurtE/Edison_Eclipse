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
// Hex screen
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

//=============================================================================
// Class definition
//=============================================================================

class TFTHexScreen : public TFTScreen {
public:
	TFTHexScreen(uint16_t color_background, TFTDisplayObject **ppdisp, uint8_t cdisp, TFTScreen* pscreenNext, TFTScreen* pscreenPrev) :
		TFTScreen(color_background, ppdisp, cdisp, pscreenNext, pscreenPrev) {};

	  virtual uint16_t processTouch(uint16_t x, uint16_t y);
};

//=============================================================================
// Define graphic objects specific to this screen
//=============================================================================
static TFTButton _btn0(1, KPD_ROW1_Y,  KPD_BWIDTH, KPD_BHEIGHT, DCLR_BUTTON_GREY, ILI9341_GREEN, ILI9341_BLACK, "0", 0);
static TFTButton _btn1(1*KPD_DELTAX + 1, KPD_ROW1_Y,  KPD_BWIDTH, KPD_BHEIGHT, DCLR_BUTTON_GREY, ILI9341_GREEN, ILI9341_BLACK, "1", 1);
static TFTButton _btn2(2*KPD_DELTAX + 1, KPD_ROW1_Y,  KPD_BWIDTH, KPD_BHEIGHT, DCLR_BUTTON_GREY, ILI9341_GREEN, ILI9341_BLACK, "2", 2);
static TFTButton _btn3(3*KPD_DELTAX + 1, KPD_ROW1_Y,  KPD_BWIDTH, KPD_BHEIGHT, DCLR_BUTTON_GREY, ILI9341_GREEN, ILI9341_BLACK, "3", 3);
static TFTButton _btn4(4*KPD_DELTAX + 1, KPD_ROW1_Y,  KPD_BWIDTH, KPD_BHEIGHT, DCLR_BUTTON_GREY, ILI9341_GREEN, ILI9341_BLACK, "4", 4);
static TFTButton _btn5(5*KPD_DELTAX + 1, KPD_ROW1_Y,  KPD_BWIDTH, KPD_BHEIGHT, DCLR_BUTTON_GREY, ILI9341_GREEN, ILI9341_BLACK, "5", 5);
static TFTButton _btn6(6*KPD_DELTAX + 1, KPD_ROW1_Y,  KPD_BWIDTH, KPD_BHEIGHT, DCLR_BUTTON_GREY, ILI9341_GREEN, ILI9341_BLACK, "6", 6);
static TFTButton _btn7(7*KPD_DELTAX + 1, KPD_ROW1_Y,  KPD_BWIDTH, KPD_BHEIGHT, DCLR_BUTTON_GREY, ILI9341_GREEN, ILI9341_BLACK, "7", 7);

static TFTButton _btn8(1, KPD_ROW2_Y,  KPD_BWIDTH, KPD_BHEIGHT, DCLR_BUTTON_GREY, ILI9341_GREEN, ILI9341_BLACK, "8", 8);
static TFTButton _btn9(1*KPD_DELTAX + 1, KPD_ROW2_Y,  KPD_BWIDTH, KPD_BHEIGHT, DCLR_BUTTON_GREY, ILI9341_GREEN, ILI9341_BLACK, "9", 9);
static TFTButton _btna(2*KPD_DELTAX + 1, KPD_ROW2_Y,  KPD_BWIDTH, KPD_BHEIGHT, DCLR_BUTTON_GREY, ILI9341_GREEN, ILI9341_BLACK, "A",10);
static TFTButton _btnb(3*KPD_DELTAX + 1, KPD_ROW2_Y,  KPD_BWIDTH, KPD_BHEIGHT, DCLR_BUTTON_GREY, ILI9341_GREEN, ILI9341_BLACK, "B",11);
static TFTButton _btnc(4*KPD_DELTAX + 1, KPD_ROW2_Y,  KPD_BWIDTH, KPD_BHEIGHT, DCLR_BUTTON_GREY, ILI9341_GREEN, ILI9341_BLACK, "C",12);
static TFTButton _btnd(5*KPD_DELTAX + 1, KPD_ROW2_Y,  KPD_BWIDTH, KPD_BHEIGHT, DCLR_BUTTON_GREY, ILI9341_GREEN, ILI9341_BLACK, "D",13);
static TFTButton _btne(6*KPD_DELTAX + 1, KPD_ROW2_Y,  KPD_BWIDTH, KPD_BHEIGHT, DCLR_BUTTON_GREY, ILI9341_GREEN, ILI9341_BLACK, "D",14);
static TFTButton _btnf(7*KPD_DELTAX + 1, KPD_ROW2_Y,  KPD_BWIDTH, KPD_BHEIGHT, DCLR_BUTTON_GREY, ILI9341_GREEN, ILI9341_BLACK, "F",15);

//=============================================================================
// Create our global Object.
//=============================================================================

TFTDisplayObject *_hexscreenobjs[] = {&_btn0,&_btn1, &_btn2, &_btn3, &_btn4, &_btn5, &_btn6, &_btn7,
		&_btn8, &_btn9, &_btna, &_btnb, &_btnc, &_btnd, &_btne, &_btnf,
		&g_btnup,&g_btndn, &g_btnok};

TFTHexScreen  g_hex_screen(ILI9341_RED, _hexscreenobjs, sizeof(_hexscreenobjs)/sizeof(_hexscreenobjs[0]),
		(TFTScreen*)&g_main_screen, (TFTScreen*)&g_run_screen);

//=============================================================================
// Process Touch
//=============================================================================
uint16_t TFTHexScreen::processTouch(uint16_t x, uint16_t y)
{
	uint16_t wTouch = TFTScreen::processTouch(x, y);
	if (wTouch != 0xffff) {
		// TBD
	}
	return wTouch;
}

// will probably also define idle time...





