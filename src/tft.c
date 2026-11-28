#include "tft.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/spi.h"

#ifndef TFT_SPI_PORT
#define TFT_SPI_PORT   spi0
#endif
#ifndef TFT_SCK_PIN
#define TFT_SCK_PIN    18u
#endif
#ifndef TFT_MOSI_PIN
#define TFT_MOSI_PIN   19u
#endif
#ifndef TFT_CS_PIN
#define TFT_CS_PIN     17u
#endif
#ifndef TFT_RST_PIN
#define TFT_RST_PIN    16u
#endif
#ifndef TFT_DC_PIN
#define TFT_DC_PIN     20u
#endif

#define TFT_BLACK      0x0000u
#define TFT_GREEN      0x07E0u
#define TFT_RED        0xF800u

static void tft_cmd(uint8_t cmd) {
    gpio_put(TFT_DC_PIN, 0);
    gpio_put(TFT_CS_PIN, 0);
    spi_write_blocking(TFT_SPI_PORT, &cmd, 1);
    gpio_put(TFT_CS_PIN, 1);
}

static void tft_data(const uint8_t *data, size_t len) {
    gpio_put(TFT_DC_PIN, 1);
    gpio_put(TFT_CS_PIN, 0);
    spi_write_blocking(TFT_SPI_PORT, data, len);
    gpio_put(TFT_CS_PIN, 1);
}

static void tft_set_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    uint8_t col_buf[4] = {
        (uint8_t)(x0 >> 8), (uint8_t)(x0 & 0xFFu),
        (uint8_t)(x1 >> 8), (uint8_t)(x1 & 0xFFu)
    };
    uint8_t row_buf[4] = {
        (uint8_t)(y0 >> 8), (uint8_t)(y0 & 0xFFu),
        (uint8_t)(y1 >> 8), (uint8_t)(y1 & 0xFFu)
    };
    tft_cmd(0x2Au);
    tft_data(col_buf, 4);
    tft_cmd(0x2Bu);
    tft_data(row_buf, 4);
    tft_cmd(0x2Cu);
}

static void tft_fill_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color565) {
    if (w == 0 || h == 0) return;
    tft_set_window(x, y, (uint16_t)(x + w - 1u), (uint16_t)(y + h - 1u));
    uint8_t pixel[2] = { (uint8_t)(color565 >> 8), (uint8_t)(color565 & 0xFFu) };
    uint32_t total = (uint32_t)w * (uint32_t)h;

    gpio_put(TFT_DC_PIN, 1);
    gpio_put(TFT_CS_PIN, 0);
    for (uint32_t i = 0; i < total; i++) {
        spi_write_blocking(TFT_SPI_PORT, pixel, 2);
    }
    gpio_put(TFT_CS_PIN, 1);
}

void tft_fill_screen(uint16_t color565) {
    tft_fill_rect(0, 0, 240u, 320u, color565);
}

void tft_init(void) {
    spi_init(TFT_SPI_PORT, 10000000u);
    gpio_set_function(TFT_SCK_PIN, GPIO_FUNC_SPI);
    gpio_set_function(TFT_MOSI_PIN, GPIO_FUNC_SPI);

    gpio_init(TFT_CS_PIN);
    gpio_set_dir(TFT_CS_PIN, GPIO_OUT);
    gpio_put(TFT_CS_PIN, 1);

    gpio_init(TFT_DC_PIN);
    gpio_set_dir(TFT_DC_PIN, GPIO_OUT);
    gpio_put(TFT_DC_PIN, 0);

    gpio_init(TFT_RST_PIN);
    gpio_set_dir(TFT_RST_PIN, GPIO_OUT);

    gpio_put(TFT_RST_PIN, 0);
    sleep_ms(20);
    gpio_put(TFT_RST_PIN, 1);
    sleep_ms(120);

    tft_cmd(0x01u);
    sleep_ms(120);

    tft_cmd(0x3Au);
    uint8_t pixfmt = 0x55u;
    tft_data(&pixfmt, 1);

    tft_cmd(0x36u);
    uint8_t madctl = 0x48u;
    tft_data(&madctl, 1);

    tft_cmd(0x11u);
    sleep_ms(120);

    tft_cmd(0x29u);
    sleep_ms(50);

    tft_fill_screen(TFT_BLACK);
}

void tft_show_motor_status(bool m1_run, bool m2_run, char last_key) {
    (void)last_key;
    tft_fill_screen(TFT_BLACK);
    tft_fill_rect(20u, 40u, 200u, 80u, m1_run ? TFT_GREEN : TFT_RED);
    tft_fill_rect(20u, 160u, 200u, 80u, m2_run ? TFT_GREEN : TFT_RED);
}