#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "motor.h"
#include "keypad.h"

#define STEP_PIN 10

int main(void) {
    stdio_init_all();
    sleep_ms(1500);

    printf("[TEST] Motor + Keypad (Rows 1-2 only)\n");
    printf("[KEYS] 1/2/3=speed, A=CW, B=CCW, 4=run, 5=stop, 6=reset count\n");

    motor_init();
    keypad_init();

    bool running = false;
    bool dir_cw = true;
    uint32_t half_period_us = 20000; // ~25 steps/sec to start
    uint32_t step_count = 0;
    bool step_pin_high = false;
    uint64_t last_edge_us = time_us_64();

    motor_set_dir(dir_cw);
    gpio_put(STEP_PIN, 0);

    while (true) {
        char key = keypad_poll();
        if (key != '\0') {
            switch (key) {
                case '1':
                    half_period_us = 20000;
                    printf("[SPEED] Slow (~25 steps/s)\n");
                    break;
                case '2':
                    half_period_us = 10000;
                    printf("[SPEED] Medium (~50 steps/s)\n");
                    break;
                case '3':
                    half_period_us = 5000;
                    printf("[SPEED] Fast (~100 steps/s)\n");
                    break;
                case 'A':
                    dir_cw = true;
                    motor_set_dir(dir_cw);
                    printf("[DIR] CW\n");
                    break;
                case 'B':
                    dir_cw = false;
                    motor_set_dir(dir_cw);
                    printf("[DIR] CCW\n");
                    break;
                case '4':
                    running = true;
                    last_edge_us = time_us_64();
                    printf("[STATE] Running\n");
                    break;
                case '5':
                    running = false;
                    step_pin_high = false;
                    gpio_put(STEP_PIN, 0);
                    printf("[STATE] Stopped (steps=%lu)\n", (unsigned long)step_count);
                    break;
                case '6':
                    step_count = 0;
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
                        printf("[MOTOR] Steps: %lu\n", (unsigned long)step_count);
                    }
                }
            }
        }

        sleep_ms(1);
    }
}