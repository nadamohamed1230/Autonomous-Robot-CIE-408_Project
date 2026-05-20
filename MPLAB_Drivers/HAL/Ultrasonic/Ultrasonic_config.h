#ifndef ULTRASONIC_CONFIG_H
#define ULTRASONIC_CONFIG_H

#include "../../MCAL/GPIO/GPIO_interface.h"

/*
 * TRIGGER PINS (Outputs)
 * Safely on PORTA (Remember to set ADCON1 = 0x06 in main.c!)
 */
#define TRIG_PORT       GPIO_PORTA
#define TRIG_FRONT_PIN  GPIO_PIN0   /* RA0 */
#define TRIG_BACK_PIN   GPIO_PIN1   /* RA1 */
#define TRIG_RIGHT_PIN  GPIO_PIN2   /* RA2 */
/*
 * ECHO PINS (Hardware Interrupts)
 * Safely on PORTB. RB4 is left completely empty for the Right Encoder!
 */
#define ECHO_PORT       GPIO_PORTB
#define ECHO_FRONT_PIN  GPIO_PIN5   /* RB5 */
#define ECHO_BACK_PIN   GPIO_PIN6   /* RB6 */
#define ECHO_RIGHT_PIN  GPIO_PIN7   /* RB7 */

#endif /* ULTRASONIC_CONFIG_H */