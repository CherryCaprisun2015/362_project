#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "hardware/spi.h"

//THIS IS HOW ITS SOLDERED ON MY BOARD -- MY SD CARD IS NOT WIRED OR ANYTHING YET, I was just making it all talk to eachother 

#define TFT_SPI_PORT    spi0
#define TFT_PIN_DC      20
#define TFT_PIN_CS      17
#define TFT_PIN_SCK     18
#define TFT_PIN_MOSI    19
#define TFT_PIN_RST     16

#define TFT_WIDTH       240
#define TFT_HEIGHT      320

// RGB565 color helpers
#define TFT_BLACK       0x0000
#define TFT_WHITE       0xFFFF
#define TFT_RED         0xF800
#define TFT_GREEN       0x07E0
#define TFT_BLUE        0x001F
#define TFT_NAVY        0x000F
#define TFT_YELLOW      0xFFE0

void tft_init(void);
void tft_fill_screen(uint16_t color);
void tft_fill_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
void tft_draw_pixel(uint16_t x, uint16_t y, uint16_t color);