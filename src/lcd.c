#include <stdio.h>
#include "lcd.h"
#include "board_pins.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"

// Native I2C control bytes for AiP31068L / similar controllers at 0x3E
#define AIP_CTRL_CMD    0x00u
#define AIP_CTRL_DATA   0x40u

static uint8_t s_lcd_addr = 0x3Eu;
static bool s_i2c_ready = false;

static void lcd_bus_ensure_init(void) {
    if (s_i2c_ready) {
        return;
    }
    i2c_init(LCD_I2C_PORT, LCD_I2C_BAUD_HZ);
    gpio_set_function(LCD_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(LCD_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(LCD_SDA_PIN);
    gpio_pull_up(LCD_SCL_PIN);
    s_i2c_ready = true;
}

static bool lcd_write_pair(uint8_t control, uint8_t value) {
    lcd_bus_ensure_init();
    uint8_t buf[2] = { control, value };
    int ret = i2c_write_blocking_until(
        LCD_I2C_PORT,
        s_lcd_addr,
        buf,
        2,
        false,
        make_timeout_time_ms(10)
    );
    return ret == 2;
}

static void lcd_cmd(uint8_t cmd) {
    lcd_write_pair(AIP_CTRL_CMD, cmd);
    if (cmd == 0x01u || cmd == 0x02u) {
        sleep_ms(2);
    } else {
        sleep_us(50);
    }
}

static void lcd_data(uint8_t ch) {
    lcd_write_pair(AIP_CTRL_DATA, ch);
    sleep_us(50);
}

uint8_t lcd_scan_bus(void) {
    lcd_bus_ensure_init();
    uint8_t found = 0;

    printf("[I2C] Scanning bus on SDA=%d, SCL=%d...\n", LCD_SDA_PIN, LCD_SCL_PIN);
    for (uint8_t addr = 0x08u; addr <= 0x77u; addr++) {
        uint8_t dummy = 0;
        int ret = i2c_write_blocking_until(
            LCD_I2C_PORT,
            addr,
            &dummy,
            1,
            false,
            make_timeout_time_ms(5)
        );
        if (ret == 1) {
            printf("[I2C] Found device at 0x%02X\n", addr);
            if (found == 0) {
                found = addr;
            }
        }
    }

    if (found == 0) {
        printf("[I2C] No I2C device found.\n");
    }
    return found;
}

void lcd_init(uint8_t addr) {
    s_lcd_addr = (addr != 0) ? addr : 0x3Eu;
    lcd_bus_ensure_init();

    sleep_ms(50);
    // Function set: 2 lines, 5x8 font (0x38 = 8-bit interface, 2 lines, 5x8)
    lcd_cmd(0x38u);
    sleep_us(100);
    lcd_cmd(0x0Cu); // Display ON, cursor OFF, blink OFF
    sleep_us(100);
    lcd_cmd(0x01u); // Clear display
    sleep_ms(2);
    lcd_cmd(0x06u); // Entry mode: increment DDRAM address, no shift
    sleep_us(100);
}

void lcd_clear(void) {
    lcd_cmd(0x01u);
}

void lcd_set_cursor(uint8_t col, uint8_t row) {
    static const uint8_t row_offsets[2] = { 0x00u, 0x40u };
    if (row > 1u) row = 1u;
    if (col > 15u) col = 15u;
    lcd_cmd((uint8_t)(0x80u | (row_offsets[row] + col)));
}

void lcd_print(const char *str) {
    if (!str) return;
    while (*str) {
        lcd_data((uint8_t)*str++);
    }
}

void lcd_print_at(uint8_t col, uint8_t row, const char *str) {
    lcd_set_cursor(col, row);
    lcd_print(str);
}