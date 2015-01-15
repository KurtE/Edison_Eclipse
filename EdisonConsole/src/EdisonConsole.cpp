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
// Simple Console application for the Edison based Phantom Phoenix
// Program uses an Adafruit 2.8" TFT display with Touch
//=============================================================================

#include <signal.h>
#include <stdint.h>
#include <stdarg.h>
#include <ArduinoDefs.h>
#include "globals.h"
#include "tft_display_object.h"
#include "tft_screen.h"

extern void setup(void);
extern void loop(void);

// The STMPE610 uses hardware SPI on the shield, and #8
Adafruit_STMPE610 ts = Adafruit_STMPE610(STMPE_CS);


 // Use hardware SPI (on Uno, #13, #12, #11) and the above for CS/DC
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

// Backlight for TFT
uint8_t bBacklight = 100;
unsigned long ulLastTouch;
mraa_pwm_context pwmBacklight;


// Size of the color selection boxes and the paintbrush size
#define BOXSIZE 40
#define PENRADIUS 3
int oldcolor, currentcolor;



int main()
{
    setup();

    for(;;)
    {
        loop();
    }
}

//--------------------------------------------------------------------------
// SignalHandler - Try to free up things like servos if we abort.
//--------------------------------------------------------------------------
void SignalHandler(int sig){
    printf("Caught signal %d\n", sig);

    // Free up the servos if they are active
    // lets cleanup...
    tft.end();  // try to cleanup.
   exit(1);

}


//--------------------------------------------------------------------------
// SETUP: the main arduino setup function.
//--------------------------------------------------------------------------
void setup()
{
    // Install signal handler to allow us to do some cleanup...
    struct sigaction sigIntHandler;

    sigIntHandler.sa_handler = SignalHandler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;

    sigaction(SIGINT, &sigIntHandler, NULL);

    if (!ts.begin()) {
        printf("Couldn't start touchscreen controller\n");
    }

    // Lets initialize the TFT display.
    tft.begin();
    tft.setRotation(3);
    tft.fillScreen(ILI9341_BLACK);

    pwmBacklight = mraa_pwm_init(TFT_BACKLIGHT);
    mraa_pwm_period_ms(pwmBacklight, 1);
    mraa_pwm_enable(pwmBacklight, 1);
    mraa_pwm_write(pwmBacklight, (float)bBacklight/100.0);
    ulLastTouch = millis();

    // Call the Init screens
    InitScreens();
    TFTScreen::curScreen()->draw();
}



//=============================================================================
// Loop: the main arduino main Loop function
//=============================================================================


void loop(void)
{
	// Start off like simple touch paint program...
    if (ts.bufferEmpty()) {
    	if (bBacklight && (millis() - ulLastTouch) > SCREENDIMTIMEMS) {
    		bBacklight -= 25;
    	    mraa_pwm_write(pwmBacklight, (float)bBacklight/100.0);
    	    ulLastTouch = millis();
    	}
    	// See if current screen wants to do something during idle
    	uint16_t wDelay = TFTScreen::curScreen()->idleTime();
    	usleep(wDelay);
	    return;
    }

    if (bBacklight < 100) {
    	bBacklight = 100;
	    mraa_pwm_write(pwmBacklight, 1.0);
	    do {
			while (!ts.bufferEmpty()) {
				ts.getPoint();
				usleep(1);
			}
	    } while (ts.touched());
		ulLastTouch = millis();
	    return;
    }

    // Retrieve a point and map it to our screen coordinates
    uint16_t x, y;
    GetTouchPoint(&x, &y);

    // Call off to current screen to prcess touch event.
    TFTScreen::curScreen()->processTouch(x, y);
    ulLastTouch = millis();
}

// Lets try to define a screen of objects
static TFTText _mst1(0, 0, 100, 16, -1, -1, ILI9341_WHITE, "Kurt's Edison", 0);
static TFTText _mst2(0, 16, 100, 16, -1, -1, ILI9341_WHITE, "Voltage", 0);
TFTDisplayObject *TFTMainScreen[] = {&_mst1, &_mst2};

