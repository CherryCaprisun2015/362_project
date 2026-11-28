#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "hardware/i2c.h"

#define LCD_I2C_PORT      i2c1
#define LCD_SDA_PIN       2
#define LCD_SCL_PIN       3
#define LCD_I2C_BAUD_HZ   100000

// Common PCF8574 backpack addresses: 0x27 or 0x3F
#define LCD_DEFAULT_ADDR  0x27

void lcd_init(uint8_t addr);
void lcd_clear(void);
void lcd_set_cursor(uint8_t col, uint8_t row);
void lcd_print(const char *str);
void lcd_print_at(uint8_t col, uint8_t row, const char *str);
uint8_t lcd_scan_bus(void);