// MOTOR Includes
#ifndef MOTOR_H
#define MOTOR_H

#include <stdbool.h>
#include <stdint.h>

#ifndef MOTOR1_STEP_PIN
#define MOTOR1_STEP_PIN 10
#endif

#ifndef MOTOR1_DIR_PIN
#define MOTOR1_DIR_PIN 11
#endif

#ifndef MOTOR1_EN_PIN
#define MOTOR1_EN_PIN 12
#endif

void motor_init(void);
void motor_set_enabled(bool enabled);
void motor_set_dir(bool clockwise);
void motor_step_once(uint32_t pulse_delay_ms);

#endif