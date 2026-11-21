
#include <stdio.h>
#include "pico/stdlib.h"

int main(void) {
    stdio_init_all();

    // Initialize the Proton's red LED (GP22)
    gpio_init(22);
    gpio_set_dir(22, GPIO_OUT);

    while (true) {
        gpio_put(22, 1);
        sleep_ms(500);

        gpio_put(22, 0);
        sleep_ms(500);
    }

    return 0;
}