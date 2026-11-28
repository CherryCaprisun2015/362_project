#ifndef LCD_H
#define LCD_H

#include <stdbool.h>
#include <stdint.h>

uint8_t lcd_scan_bus(void);
void lcd_init(uint8_t addr);
void lcd_clear(void);
void lcd_set_cursor(uint8_t col, uint8_t row);
void lcd_print(const char *str);
void lcd_print_at(uint8_t col, uint8_t row, const char *str);

#endif