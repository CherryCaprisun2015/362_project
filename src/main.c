#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "motor.h"
#include "keypad.h"
#include "tft.h"

#define STEP_PIN 10

static void tft_show_status(bool running, bool dir_cw, uint32_t half_period_us, uint32_t step_count, char last_key) {
    uint16_t bg = running ? TFT_NAVY : TFT_BLACK;
    uint16_t banner = running ? TFT_GREEN : TFT_RED;

    tft_fill_screen(bg);
    tft_fill_rect(12, 16, 216, 44, banner);

    // Visual status bars for running state, direction, and speed tier
    tft_fill_rect(12, 76, dir_cw ? 180 : 90, 28, TFT_BLUE);

    uint16_t speed_w = (half_period_us <= 5000) ? 216 : (half_period_us <= 10000) ? 144 : 72;
    tft_fill_rect(12, 118, speed_w, 28, TFT_YELLOW);

    // Step progress indicator bar
    uint16_t step_w = (uint16_t)((step_count % 200) * 216u / 200u);
    tft_fill_rect(12, 162, 216, 24, TFT_WHITE);
    if (step_w > 0) {
        tft_fill_rect(12, 162, step_w, 24, TFT_GREEN);
    }

    (void)last_key;
}

int main(void) {
    stdio_init_all();
    sleep_ms(1500);

    printf("[TEST] Motor + Keypad + SPI TFT\n");
    printf("[KEYS] 1/2/3=speed, A=CW, B=CCW, 4=run, 5=stop, 6=reset count\n");

    motor_init();
    keypad_init();
    tft_init();

    bool running = false;
    bool dir_cw = true;
    uint32_t half_period_us = 20000; // ~25 steps/sec to start
    uint32_t step_count = 0;
    bool step_pin_high = false;
    uint64_t last_edge_us = time_us_64();
    bool ui_dirty = true;
    char last_key = '-';

    motor_set_dir(dir_cw);
    gpio_put(STEP_PIN, 0);

    while (true) {
        char key = keypad_poll();
        if (key != '\0') {
            last_key = key;
            switch (key) {
                case '1':
                    half_period_us = 20000;
                    ui_dirty = true;
                    printf("[SPEED] Slow (~25 steps/s)\n");
                    break;
                case '2':
                    half_period_us = 10000;
                    ui_dirty = true;
                    printf("[SPEED] Medium (~50 steps/s)\n");
                    break;
                case '3':
                    half_period_us = 5000;
                    ui_dirty = true;
                    printf("[SPEED] Fast (~100 steps/s)\n");
                    break;
                case 'A':
                    dir_cw = true;
                    motor_set_dir(dir_cw);
                    ui_dirty = true;
                    printf("[DIR] CW\n");
                    break;
                case 'B':
                    dir_cw = false;
                    motor_set_dir(dir_cw);
                    ui_dirty = true;
                    printf("[DIR] CCW\n");
                    break;
                case '4':
                    running = true;
                    last_edge_us = time_us_64();
                    ui_dirty = true;
                    printf("[STATE] Running\n");
                    break;
                case '5':
                    running = false;
                    step_pin_high = false;
                    gpio_put(STEP_PIN, 0);
                    ui_dirty = true;
                    printf("[STATE] Stopped (steps=%lu)\n", (unsigned long)step_count);
                    break;
                case '6':
                    step_count = 0;
                    ui_dirty = true;
                    printf("[COUNT] Reset to 0\n");
                    break;
                default:
                    break;
            }
        }

        if (running) {
            uint64_t now = time_us_64();
            if ((now - last_edge_us) >= half_period_us) {
                last_edge_us = now;
                step_pin_high = !step_pin_high;
                gpio_put(STEP_PIN, step_pin_high ? 1 : 0);

                if (!step_pin_high) {
                    step_count++;
                    if (step_count % 50 == 0) {
                        ui_dirty = true;
                        printf("[MOTOR] Steps: %lu\n", (unsigned long)step_count);
                    }
                }
            }
        }

        if (ui_dirty) {
            tft_show_status(running, dir_cw, half_period_us, step_count, last_key);
            ui_dirty = false;
        }

        sleep_ms(1);
    }
}