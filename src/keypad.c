#include "keypad.h"
#include "board_pins.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"

#define KEYPAD_ROWS 4
#define KEYPAD_COLS 4
#define DEBOUNCE_TIME_US 25000

static const uint row_pins[KEYPAD_ROWS] = {
    KEYPAD_ROW1_PIN, // 30
    KEYPAD_ROW2_PIN, // 29
    KEYPAD_ROW3_PIN, // 28
    KEYPAD_ROW4_PIN  // 27
};

static const uint col_pins[KEYPAD_COLS] = {
    KEYPAD_COL1_PIN, // 34
    KEYPAD_COL2_PIN, // 33
    KEYPAD_COL3_PIN, // 32
    KEYPAD_COL4_PIN  // 31
};

static const char keymap[KEYPAD_ROWS][KEYPAD_COLS] = {
    { '1', '2', '3', 'A' },
    { '4', '5', '6', 'B' },
    { '7', '8', '9', 'C' },
    { '*', '0', '#', 'D' }
};

static char last_raw_key = '\0';
static char stable_key = '\0';
static uint64_t last_change_us = 0;

void keypad_init(void) {
    for (int c = 0; c < KEYPAD_COLS; c++) {
        gpio_init(col_pins[c]);
        gpio_set_dir(col_pins[c], GPIO_OUT);
        gpio_put(col_pins[c], 1); // Idle HIGH
    }

    for (int r = 0; r < KEYPAD_ROWS; r++) {
        gpio_init(row_pins[r]);
        gpio_set_dir(row_pins[r], GPIO_IN);
        gpio_pull_up(row_pins[r]);
    }

    last_raw_key = '\0';
    stable_key = '\0';
    last_change_us = time_us_64();
}

static char keypad_scan_raw(void) {
    char detected_key = '\0';
    int pressed_count = 0;

    for (int c = 0; c < KEYPAD_COLS; c++) {
        gpio_put(col_pins[c], 0); // Drive active column LOW
        sleep_us(20);

        for (int r = 0; r < KEYPAD_ROWS; r++) {
            if (!gpio_get(row_pins[r])) {
                detected_key = keymap[r][c];
                pressed_count++;
            }
        }

        gpio_put(col_pins[c], 1); // Return column HIGH
        sleep_us(10);
    }

    if (pressed_count == 1) {
        return detected_key;
    }

    return '\0';
}

char keypad_poll(void) {
    char raw_key = keypad_scan_raw();
    uint64_t now = time_us_64();

    if (raw_key != last_raw_key) {
        last_raw_key = raw_key;
        last_change_us = now;
    }

    if ((now - last_change_us) >= DEBOUNCE_TIME_US) {
        if (raw_key != stable_key) {
            stable_key = raw_key;
            if (stable_key != '\0') {
                return stable_key;
            }
        }
    }

    return '\0';
}