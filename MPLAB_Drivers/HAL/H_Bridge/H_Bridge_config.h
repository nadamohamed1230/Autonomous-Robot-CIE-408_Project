#ifndef H_BRIDGE_CONFIG_H
#define H_BRIDGE_CONFIG_H

#include "../../MCAL/GPIO/GPIO_interface.h"
#include "../../MCAL/PWM/PWM_interface.h"

/*
 * =============================================================================
 * H_Bridge driver convention (from H_Bridge.c comments):
 *   IN1/IN2 + MODE_A (ENA) = LEFT  motor
 *   IN3/IN4 + MODE_B (ENB) = RIGHT motor
 *
 * Physical wiring confirmed by hardware test:
 *   RD0/RD1 → L298N IN1/IN2 → Right motor
 *   RD2/RD3 → L298N IN3/IN4 → Left motor
 *   RC2 (CCP1) → ENA
 *   RC1 (CCP2) → ENB
 *
 * FIX: map driver's LEFT to physical RD2/RD3, RIGHT to physical RD0/RD1
 * =============================================================================
 *
 *  Driver label | PIC Pin | L298N physical | Motor
 *  -------------+---------+----------------+-------
 *  IN1 (L Fwd)  |  RD2    |  IN3           | Left  Forward
 *  IN2 (L Rev)  |  RD3    |  IN4           | Left  Reverse
 *  IN3 (R Fwd)  |  RD0    |  IN1           | Right Forward
 *  IN4 (R Rev)  |  RD1    |  IN2           | Right Reverse
 *  ENA (Left)   |  RC2    |  ENA           | Left  Enable (CCP1)
 *  ENB (Right)  |  RC1    |  ENB           | Right Enable (CCP2)
 */

#define H_BRIDGE_DIR_PORT    GPIO_PORTD

#define H_BRIDGE_IN1_PIN     GPIO_PIN2   /* RD2 — Left  Forward */
#define H_BRIDGE_IN2_PIN     GPIO_PIN3   /* RD3 — Left  Reverse */
#define H_BRIDGE_IN3_PIN     GPIO_PIN0   /* RD0 — Right Forward */
#define H_BRIDGE_IN4_PIN     GPIO_PIN1   /* RD1 — Right Reverse */

#define H_BRIDGE_PWM_MODE_A  PWM_MODE_CCP1   /* RC2 = ENA — Left  motor */
#define H_BRIDGE_PWM_MODE_B  PWM_MODE_CCP2   /* RC1 = ENB — Right motor */

#define H_BRIDGE_PWM_FREQ    10000U

#endif