// Main program entry point - Dual A4988 motors + existing keypad.c + 0x3E I2C LCD

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "board_pins.h"
#include "motor.h"
#include "keypad.h"
#include "lcd.h"

static void lcd_render_status(bool m1_run, bool m2_run, char last_key) {
    char line0[17];
    char line1[17];

    if (m1_run && m2_run) {
        snprintf(line0, sizeof(line0), "BOTH MOTORS: ON ");
    } else if (!m1_run && !m2_run) {
        snprintf(line0, sizeof(line0), "BOTH MOTORS: OFF");
    } else if (m1_run) {
        snprintf(line0, sizeof(line0), "M1:ON  M2:PAUSED");
    } else {
        snprintf(line0, sizeof(line0), "M1:PAUSED M2:ON ");
    }

    snprintf(line1, sizeof(line1), "M1:%c M2:%c Key:%c ",
             m1_run ? 'R' : 'P',
             m2_run ? 'R' : 'P',
             last_key ? last_key : '-');

    lcd_print_at(0, 0, line0);
    lcd_print_at(0, 1, line1);
}

int main(void) {
    stdio_init_all();
    sleep_ms(1500);

    printf("[INIT] Starting Motor 1, Motor 2, Keypad, and 0x3E LCD...\n");
    motor_init();
    motor_set_dir(true);
    motor_set_enabled(true);

    motor2_init();
    motor2_set_dir(true);
    motor2_set_enabled(true);

    keypad_init();

    // Keep your existing TFT / sensor / SD init calls here as needed
    lcd_init(0x3Eu);

    bool m1_run = true;
    bool m2_run = true;
    char last_key = '-';
    bool ui_dirty = true;

    const uint32_t step_interval_us = 15000u;
    const uint32_t pulse_high_us    = 10u;

    uint32_t m1_last_us = time_us_32();
    uint32_t m2_last_us = time_us_32();
    bool m1_pin_high = false;
    bool m2_pin_high = false;

    while (true) {
        uint32_t now_us = time_us_32();

        char key = keypad_poll();
        if (key != '\0') {
            last_key = key;
            if (key == '1') {
                m1_run = !m1_run;
                if (!m1_run && m1_pin_high) {
                    gpio_put(MOTOR1_STEP_PIN, 0);
                    m1_pin_high = false;
                }
            } else if (key == '2') {
                m2_run = !m2_run;
                if (!m2_run && m2_pin_high) {
                    gpio_put(MOTOR2_STEP_PIN, 0);
                    m2_pin_high = false;
                }
            } else if (key == 'A' || key == '3') {
                m1_run = true;
                m2_run = true;
            } else if (key == 'B' || key == '0') {
                m1_run = false;
                m2_run = false;
                gpio_put(MOTOR1_STEP_PIN, 0);
                gpio_put(MOTOR2_STEP_PIN, 0);
                m1_pin_high = false;
                m2_pin_high = false;
            }
            ui_dirty = true;
            printf("[KEY] Pressed '%c' -> M1=%s, M2=%s\n",
                   key, m1_run ? "RUN" : "PAUSE", m2_run ? "RUN" : "PAUSE");
        }

        if (m1_run) {
            uint32_t elapsed1 = now_us - m1_last_us;
            if (!m1_pin_high && elapsed1 >= step_interval_us) {
                gpio_put(MOTOR1_STEP_PIN, 1);
                m1_pin_high = true;
                m1_last_us = now_us;
            } else if (m1_pin_high && elapsed1 >= pulse_high_us) {
                gpio_put(MOTOR1_STEP_PIN, 0);
                m1_pin_high = false;
            }
        }

        if (m2_run) {
            uint32_t elapsed2 = now_us - m2_last_us;
            if (!m2_pin_high && elapsed2 >= step_interval_us) {
                gpio_put(MOTOR2_STEP_PIN, 1);
                m2_pin_high = true;
                m2_last_us = now_us;
            } else if (m2_pin_high && elapsed2 >= pulse_high_us) {
                gpio_put(MOTOR2_STEP_PIN, 0);
                m2_pin_high = false;
            }
        }

        if (ui_dirty) {
            lcd_render_status(m1_run, m2_run, last_key);
            ui_dirty = false;
        }
    }

    return 0;
}