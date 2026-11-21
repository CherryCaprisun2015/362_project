// Controls the 16x2 character LCD through I2C.
// Handles initialization, cursor positioning, and text output.

#include "lcd.h"
#include "board_pins.h"
#include "pico/stdlib.h"
#include "hardware/i2c.h"