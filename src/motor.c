// Controls stepper motors through A4988 drivers.
// Uses GPIO for STEP pulse generation, DIR, and active-low EN.

#include "motor.h"
#include "board_pins.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"

void motor_init(void) {
    gpio_init(MOTOR1_STEP_PIN);
    gpio_set_dir(MOTOR1_STEP_PIN, GPIO_OUT);
    gpio_put(MOTOR1_STEP_PIN, 0);

    gpio_init(MOTOR1_DIR_PIN);
    gpio_set_dir(MOTOR1_DIR_PIN, GPIO_OUT);
    gpio_put(MOTOR1_DIR_PIN, 1);

    gpio_init(MOTOR1_EN_PIN);
    gpio_set_dir(MOTOR1_EN_PIN, GPIO_OUT);
    // A4988 ENABLE is active-low: 0 = enabled, 1 = disabled
    motor_set_enabled(true);
}

void motor_set_enabled(bool enabled) {
    gpio_put(MOTOR1_EN_PIN, enabled ? 0 : 1);
}

void motor_set_dir(bool clockwise) {
    gpio_put(MOTOR1_DIR_PIN, clockwise ? 1 : 0);
}

void motor_step_once(uint32_t pulse_delay_ms) {
    gpio_put(MOTOR1_STEP_PIN, 1);
    sleep_ms(pulse_delay_ms);
    gpio_put(MOTOR1_STEP_PIN, 0);
    sleep_ms(pulse_delay_ms);
}

void motor2_init(void) {
    gpio_init(MOTOR2_STEP_PIN);
    gpio_set_dir(MOTOR2_STEP_PIN, GPIO_OUT);
    gpio_put(MOTOR2_STEP_PIN, 0);

    gpio_init(MOTOR2_DIR_PIN);
    gpio_set_dir(MOTOR2_DIR_PIN, GPIO_OUT);
    gpio_put(MOTOR2_DIR_PIN, 1);

    gpio_init(MOTOR2_EN_PIN);
    gpio_set_dir(MOTOR2_EN_PIN, GPIO_OUT);
    // A4988 ENABLE is active-low: 0 = enabled, 1 = disabled
    motor2_set_enabled(true);
}

void motor2_set_enabled(bool enabled) {
    gpio_put(MOTOR2_EN_PIN, enabled ? 0 : 1);
}

void motor2_set_dir(bool clockwise) {
    gpio_put(MOTOR2_DIR_PIN, clockwise ? 1 : 0);
}

void motor2_step_once(uint32_t pulse_delay_ms) {
    gpio_put(MOTOR2_STEP_PIN, 1);
    sleep_ms(pulse_delay_ms);
    gpio_put(MOTOR2_STEP_PIN, 0);
    sleep_ms(pulse_delay_ms);
}