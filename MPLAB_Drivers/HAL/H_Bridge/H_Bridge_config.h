#ifndef H_BRIDGE_CONFIG_H
#define H_BRIDGE_CONFIG_H

#include "../../MCAL/GPIO/GPIO_interface.h"
#include "../../MCAL/PWM/PWM_interface.h"

/* H-Bridge Direction Control Pins (connected to L298N IN1-IN4) */
#define H_BRIDGE_DIR_PORT   GPIO_PORTD
#define H_BRIDGE_IN1_PIN    GPIO_PIN0   /* Motor A (Left)  - Forward */
#define H_BRIDGE_IN2_PIN    GPIO_PIN1   /* Motor A (Left)  - Reverse */
#define H_BRIDGE_IN3_PIN    GPIO_PIN2   /* Motor B (Right) - Forward */
#define H_BRIDGE_IN4_PIN    GPIO_PIN3   /* Motor B (Right) - Reverse */

/* PWM channels for Enable pins (ENA = CCP1/RC2, ENB = CCP2/RC1) */
#define H_BRIDGE_PWM_MODE_A  PWM_MODE_CCP1
#define H_BRIDGE_PWM_MODE_B  PWM_MODE_CCP2

/* PWM frequency in Hz */
#define H_BRIDGE_PWM_FREQ    5000U

#endif
