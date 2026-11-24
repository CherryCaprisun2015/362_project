
// Controls the color TFT display through SPI.
// Handles graphics, text, dealing animations, and status displays.

#include "tft.h"
#include "board_pins.h"
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"

static inline void tft_cs_low(void)  { gpio_put(TFT_PIN_CS, 0); }
static inline void tft_cs_high(void) { gpio_put(TFT_PIN_CS, 1); }
static inline void tft_dc_cmd(void)  { gpio_put(TFT_PIN_DC, 0); }
static inline void tft_dc_data(void) { gpio_put(TFT_PIN_DC, 1); }

static void tft_write_cmd(uint8_t cmd) {
    tft_dc_cmd();
    tft_cs_low();
    spi_write_blocking(TFT_SPI_PORT, &cmd, 1);
    tft_cs_high();
}

static void tft_write_data(const uint8_t *data, size_t len) {
    if (len == 0) return;
    tft_dc_data();
    tft_cs_low();
    spi_write_blocking(TFT_SPI_PORT, data, len);
    tft_cs_high();
}

static void tft_reset(void) {
    gpio_put(TFT_PIN_RST, 0);
    sleep_ms(20);
    gpio_put(TFT_PIN_RST, 1);
    sleep_ms(120);
}

static void tft_set_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    uint8_t col_buf[4] = {
        (uint8_t)(x0 >> 8), (uint8_t)(x0 & 0xFF),
        (uint8_t)(x1 >> 8), (uint8_t)(x1 & 0xFF)
    };
    uint8_t row_buf[4] = {
        (uint8_t)(y0 >> 8), (uint8_t)(y0 & 0xFF),
        (uint8_t)(y1 >> 8), (uint8_t)(y1 & 0xFF)
    };

    tft_write_cmd(0x2A); // Column address set
    tft_write_data(col_buf, sizeof(col_buf));

    tft_write_cmd(0x2B); // Page/row address set
    tft_write_data(row_buf, sizeof(row_buf));

    tft_write_cmd(0x2C); // Memory write
}

void tft_init(void) {
    spi_init(TFT_SPI_PORT, 16 * 1000 * 1000);
    gpio_set_function(TFT_PIN_SCK, GPIO_FUNC_SPI);
    gpio_set_function(TFT_PIN_MOSI, GPIO_FUNC_SPI);

    gpio_init(TFT_PIN_CS);
    gpio_set_dir(TFT_PIN_CS, GPIO_OUT);
    gpio_put(TFT_PIN_CS, 1);

    gpio_init(TFT_PIN_DC);
    gpio_set_dir(TFT_PIN_DC, GPIO_OUT);
    gpio_put(TFT_PIN_DC, 0);

    gpio_init(TFT_PIN_RST);
    gpio_set_dir(TFT_PIN_RST, GPIO_OUT);
    gpio_put(TFT_PIN_RST, 1);

    tft_reset();

    tft_write_cmd(0x01); // Software reset
    sleep_ms(120);

    tft_write_cmd(0x11); // Sleep out
    sleep_ms(120);

    uint8_t pixfmt = 0x55; // 16-bit RGB565
    tft_write_cmd(0x3A);
    tft_write_data(&pixfmt, 1);

    uint8_t madctl = 0x48; // Memory access control
    tft_write_cmd(0x36);
    tft_write_data(&madctl, 1);

    tft_write_cmd(0x29); // Display ON
    sleep_ms(20);
}

void tft_draw_pixel(uint16_t x, uint16_t y, uint16_t color) {
    if (x >= TFT_WIDTH || y >= TFT_HEIGHT) return;
    tft_set_window(x, y, x, y);
    uint8_t px[2] = { (uint8_t)(color >> 8), (uint8_t)(color & 0xFF) };
    tft_write_data(px, sizeof(px));
}

void tft_fill_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
    if (x >= TFT_WIDTH || y >= TFT_HEIGHT || w == 0 || h == 0) return;
    if (x + w > TFT_WIDTH)  w = TFT_WIDTH - x;
    if (y + h > TFT_HEIGHT) h = TFT_HEIGHT - y;

    tft_set_window(x, y, (uint16_t)(x + w - 1), (uint16_t)(y + h - 1));

    uint8_t hi = (uint8_t)(color >> 8);
    uint8_t lo = (uint8_t)(color & 0xFF);
    uint8_t line[TFT_WIDTH * 2];

    for (uint16_t i = 0; i < w; i++) {
        line[i * 2] = hi;
        line[i * 2 + 1] = lo;
    }

    for (uint16_t r = 0; r < h; r++) {
        tft_write_data(line, (size_t)w * 2);
    }
}

void tft_fill_screen(uint16_t color) {
    tft_fill_rect(0, 0, TFT_WIDTH, TFT_HEIGHT, color);
}