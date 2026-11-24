// Main program entry point - motor bring-up test.

#include <stdio.h>
#include "pico/stdlib.h"
#include "motor.h"

int main(void) {
    stdio_init_all();
    sleep_ms(1500); // Give USB serial time to connect

    printf("[MOTOR] Initializing GPIO10 (STEP), GPIO11 (DIR), GPIO12 (EN)...\n");
    motor_init();
    motor_set_dir(true);

    uint32_t step_count = 0;

    while (true) {
        motor_step_once(20); // ~25 steps/sec (slow enough to watch/hear)
        step_count++;

        if (step_count % 50 == 0) {
            printf("[MOTOR] Steps sent: %lu\n", (unsigned long)step_count);
        }
    }

    return 0;
}