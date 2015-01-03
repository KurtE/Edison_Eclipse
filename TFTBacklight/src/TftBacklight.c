/*
 * Author: Brendan Le Foll <brendan.le.foll@intel.com>
 * Copyright (c) 2014 Intel Corporation.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>

#include "mraa/gpio.h"
#include "mraa/pwm.h"
int main(int argc, char **argv)

{

    mraa_platform_t platform = mraa_get_platform_type();

    //mraa_gpio_context gpio;
    char board_name[] = "Some weird devboard that isn't recognised...";



    switch (platform) {

        case MRAA_INTEL_GALILEO_GEN1:

            strcpy(board_name, "Intel Galileo Gen1");

            break;

        case MRAA_INTEL_GALILEO_GEN2:

            strcpy(board_name, "Intel Galileo Gen2");

            break ;

        default:

        strcpy(board_name, "Intel Edison w minibreakout");


    }

    //gpio = mraa_gpio_init(3);
    mraa_pwm_context pwm;
    pwm = mraa_pwm_init(3);

    fprintf(stdout, "Welcome to libmraa\n Version: %s\n Running on %s\n",

   mraa_get_version(), board_name);

//    fprintf("pwm: %x\n\r", (void*)pwm);
    mraa_pwm_period_ms(pwm, 10);
    mraa_pwm_enable(pwm, 1);
    float rValue = 0;
    float rInc = .25;
    for(;;) {
    	mraa_pwm_write(pwm, rValue);
    	printf("%f\n\r", rValue);
    	rValue+=rInc;
    	if (rValue >= 1.0)
    		rInc = -.25;
    	if (rValue <= 0.0)
    		rInc = 0.25f;
    	usleep(500000);
    }
    //mraa_gpio_dir(gpio, MRAA_GPIO_OUT);
    //mraa_gpio_write(gpio, 1);
    //mraa_gpio_close(gpio);
    return 0;

}
