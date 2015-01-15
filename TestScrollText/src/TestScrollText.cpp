/*
 * EdisonConsole.cpp
 *
 *  Created on: Dec 24, 2014
 *      Author: Kurt
 */
#include <signal.h>
#include <stdint.h>
#include <stdarg.h>
#include <ArduinoDefs.h>
#include "globals.h"
#include "tft_display_object.h"

extern void setup(void);
extern void loop(void);

// The STMPE610 uses hardware SPI on the shield, and #8
Adafruit_STMPE610 ts = Adafruit_STMPE610(STMPE_CS);

SCROLLTEXT txtMsgs(10, 100, 250, 130, ILI9341_BLACK, ILI9341_WHITE, ILI9341_GREEN);


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

    // Draw our Text message object.
    txtMsgs.draw();
}



//=============================================================================
// Loop: the main arduino main Loop function
//=============================================================================

int iLoopCnt = 0;
void loop(void)
{
	char szLine[50];
	sprintf(szLine, "Line: %d", iLoopCnt++);
	txtMsgs.AddTextLine((unsigned char*)szLine);

	delay(2000);
}

