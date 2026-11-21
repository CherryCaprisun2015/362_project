
// Controls the color TFT display through SPI.
// Handles graphics, text, dealing animations, and status displays.

#include "tft.h"
#include "board_pins.h"
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"