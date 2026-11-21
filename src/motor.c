// Controls two stepper motors through A4988 drivers.
// Uses PIO for STEP pulse generation and GPIO for direction,
// enable, and other driver control signals.

#include "motor.h"
#include "board_pins.h"
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/gpio.h"