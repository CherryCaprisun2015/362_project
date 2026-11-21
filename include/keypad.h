
#pragma once

// Initialize the keypad GPIO pins.
void keypad_init(void);

// Check for a new key press.
// Returns '\0' if no new key is pressed.
char keypad_poll(void);