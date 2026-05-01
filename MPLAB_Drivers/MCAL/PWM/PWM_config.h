#ifndef PWM_CONFIG_H
#define PWM_CONFIG_H

/* Oscillator frequency (must match your hardware crystal) */
#define OSC_FREQ             16000000UL  /* 16 MHz */

/* Default PWM frequency in Hz */
#define PWM_FREQUENCY        5000U       /* 5 kHz  */

/* Timer2 prescaler: 1, 4, or 16 */
#define PWM_TIMER2_PRESCALE  1

/* PIC16F877A PWM output pins (fixed by hardware) */
#define PWM_CCP1_PIN   2    /* CCP1 output on RC2 */
#define PWM_CCP2_PIN   1    /* CCP2 output on RC1 */

#endif
