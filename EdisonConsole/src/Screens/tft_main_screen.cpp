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
// Main screen
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

class TFTMainScreen : public TFTScreen {
public:
	TFTMainScreen(uint16_t wClr, TFTDisplayObject **ppdisp, uint8_t cdisp, TFTScreen* pscreenNext, TFTScreen* pscreenPrev) :
		TFTScreen(wClr, ppdisp, cdisp, pscreenNext, pscreenPrev) {};

	  virtual uint16_t processTouch(uint16_t x, uint16_t y);
};

//=============================================================================
// Define graphic objects specific to this screen
//=============================================================================
#define CMD_SHUTDOWN 100
#define CMD_PHOENIXFIXED 101
#define CMD_PHOENIXFLOAT 102
static TFTButton _btnshutdown(10, 50, 250, KPD_BHEIGHT, DCLR_BUTTON_GREY, ILI9341_RED, ILI9341_BLACK, "Shutdown", CMD_SHUTDOWN);
static TFTButton _btnphoenixfixed(10, 100, 250, KPD_BHEIGHT, DCLR_BUTTON_GREY, ILI9341_RED, ILI9341_BLACK, "Phoenix fixed", CMD_PHOENIXFIXED);
static TFTButton _btnphoenixfloat(10, 150, 250, KPD_BHEIGHT, DCLR_BUTTON_GREY, ILI9341_RED, ILI9341_BLACK, "Hello World", CMD_PHOENIXFLOAT);


TFTDisplayObject *_mainscreenobjs[] = {&g_btnup,&g_btndn,
		&_btnshutdown, &_btnphoenixfixed, &_btnphoenixfloat,
		&g_txtTitle};

TFTMainScreen g_main_screen(ILI9341_BLACK, _mainscreenobjs, sizeof(_mainscreenobjs)/sizeof(_mainscreenobjs[0]),
		(TFTScreen*)&g_run_screen, (TFTScreen*)&g_hex_screen);


//=============================================================================
// Process Touch
//=============================================================================
uint16_t TFTMainScreen::processTouch(uint16_t x, uint16_t y)
{
	uint16_t wTouch = TFTScreen::processTouch(x, y);
	if (wTouch != 0xffff) {
		switch (wTouch) {
		case CMD_SHUTDOWN:
			// Simple just shut us down now...
			// Note May add other stuff like trying to power XBEE down and the like as well
			tft.fillScreen(ILI9341_BLACK);	// make the display go black
		    mraa_pwm_write(pwmBacklight,0.0);  // Back light go down...

			system("shutdown now");

			break;
		case CMD_PHOENIXFIXED:
			LaunchProgram("/home/root/Eclipse/Phantom_Phoenix_fixed");
			break;
		case CMD_PHOENIXFLOAT:
			LaunchProgram("/home/root/Eclipse/2_cpp_helloworld");
			break;
		}
	}
	return wTouch;
}

