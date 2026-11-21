
// Reads the TCRT5000 optical sensor using the ADC.
// Detects card passage, filters readings, and reports errors.

#include "card_sensor.h"
#include "board_pins.h"
#include "pico/stdlib.h"
#include "hardware/adc.h"