// MOTOR Includes
#ifndef MOTOR_H
#define MOTOR_H

#include <stdbool.h>
#include <stdint.h>

// Motor 1 pins (existing)
#ifndef MOTOR1_STEP_PIN
#define MOTOR1_STEP_PIN 10
#endif

#ifndef MOTOR1_DIR_PIN
#define MOTOR1_DIR_PIN 11
#endif

#ifndef MOTOR1_EN_PIN
#define MOTOR1_EN_PIN 12
#endif

// Motor 2 pins (GPIO6 = STEP, GPIO7 = DIR, GPIO9 = EN)
#ifndef MOTOR2_STEP_PIN
#define MOTOR2_STEP_PIN 6
#endif

#ifndef MOTOR2_DIR_PIN
#define MOTOR2_DIR_PIN 7
#endif

#ifndef MOTOR2_EN_PIN
#define MOTOR2_EN_PIN 9
#endif

// Motor 1 functions (existing API)
void motor_init(void);
void motor_set_enabled(bool enabled);
void motor_set_dir(bool clockwise);
void motor_step_once(uint32_t pulse_delay_ms);

// Motor 2 functions
void motor2_init(void);
void motor2_set_enabled(bool enabled);
void motor2_set_dir(bool clockwise);
void motor2_step_once(uint32_t pulse_delay_ms);

#endif