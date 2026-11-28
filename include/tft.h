#ifndef TFT_H
#define TFT_H

#include <stdbool.h>
#include <stdint.h>

void tft_init(void);
void tft_fill_screen(uint16_t color565);
void tft_show_motor_status(bool m1_run, bool m2_run, char last_key);

#endif