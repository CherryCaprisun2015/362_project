
// Reads game profiles stored on the MicroSD card.
// Handles profile loading, parsing, and configuration data.

#include "sd_profiles.h"
#include "game.h"
#include "board_pins.h"
#include "pico/stdlib.h"
#include "hardware/spi.h"