// Main program entry point.
// Initializes system components and runs the main application loop.

#include "pico/stdlib.h"
#include "motor.h"
#include "card_sensor.h"
#include "keypad.h"
#include "lcd.h"
#include "tft.h"
#include "sd_profiles.h"
#include "game.h"
#include "dealer.h"

#include <stdio.h>
#include "pico/stdlib.h"

int main(void) {
    // init serial communication.
    stdio_init_all();

    // init keypad.
    keypad_init();

        printf("Press a key or else........\n\n");

    while (true) {

        // Check 4 a new key press
        char key = keypad_poll();

        if (key != '\0') {
            printf("[KEYPAD] Key pressed: %c\n", key);
        }

        // give it some tieme,,,,
        sleep_ms(2);
    };

    // this should never run, trust🙏🙏🙏🙏🙏🙏🙏
    return 0;
}

