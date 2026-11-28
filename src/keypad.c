#include "keypad.h"
#include "board_pins.h"

#include <stdint.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

// epic constants #goodpractice 🙏🙏🙏🙏🙏🙏🙏🙏
#define KEYPAD_ROWS 4
#define KEYPAD_COLS 4
#define DEBOUNCE_TIME_US 25000

static const uint row_pins[KEYPAD_ROWS] = {
    KEYPAD_ROW1_PIN,
    KEYPAD_ROW2_PIN,
    KEYPAD_ROW3_PIN,
    KEYPAD_ROW4_PIN
};

static const uint col_pins[KEYPAD_COLS] = {
    KEYPAD_COL1_PIN,
    KEYPAD_COL2_PIN,
    KEYPAD_COL3_PIN,
    KEYPAD_COL4_PIN
};

static const char keymap[KEYPAD_ROWS][KEYPAD_COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

// Debouncing info
static char last_raw_key = '\0';
static char stable_key = '\0';
static uint64_t last_change_us = 0;

//#initdat
// initalized keyoad gpio pins and deboundngg vars
void keypad_init(void) {

    // init cols
    for (int col = 0; col < KEYPAD_COLS; col++) {
        gpio_init(col_pins[col]);
        gpio_set_dir(col_pins[col], GPIO_IN);
        gpio_pull_up(col_pins[col]);
    }

    //init rows
    for (int row = 0; row < KEYPAD_ROWS; row++) {
        gpio_init(row_pins[row]);
        gpio_disable_pulls(row_pins[row]);
        gpio_put(row_pins[row], 0);
        gpio_set_dir(row_pins[row], GPIO_IN);
    }

    last_raw_key = '\0';
    stable_key = '\0';
    last_change_us = time_us_64();
}

//scan the keypad matrix
static char keypad_scan_raw(void) {

    char detected_key = '\0';
    int pressed_count = 0;

    for (int row = 0; row < KEYPAD_ROWS; row++) {

        // activate a row (drive low and set to output)
        gpio_put(row_pins[row], 0);
        gpio_set_dir(row_pins[row], GPIO_OUT);

        // wait a bit.... dont rush the board... take it slow...
        // seriously tho, let electric sig settle before reading
        sleep_us(5);

        // read cols to see whuch key is pressed... pull low if pressed
        for (int col = 0; col < KEYPAD_COLS; col++) {
            if (!gpio_get(col_pins[col])) {
                detected_key = keymap[row][col];
                pressed_count++;
            }
        }

        // Release row to high impedance.
        gpio_set_dir(row_pins[row], GPIO_IN);
    }

    // return key or \0 if theres no key
    if (pressed_count == 1) {
        return detected_key;
    }

    return '\0';
}


// Debounce aal that
char keypad_poll(void) {

    char raw_key = keypad_scan_raw();
    uint64_t now = time_us_64();

    // reset the deboundct controller if the key is different from the last
    if (raw_key != last_raw_key) {
        last_raw_key = raw_key;
        last_change_us = now;
    }

    // Wait until the reading has remained
    // unchanged for the debounce interval.
    if ((now - last_change_us) >= DEBOUNCE_TIME_US) {

        if (raw_key != stable_key) {
            stable_key = raw_key;

            // Report new presses, not releases.
            if (stable_key != '\0') {
                return stable_key;
            }
        }
    }

    return '\0';
}