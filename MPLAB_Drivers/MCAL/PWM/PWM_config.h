#ifndef PWM_CONFIG_H
#define PWM_CONFIG_H

/* Oscillator frequency — must match your crystal */
#define OSC_FREQ             20000000UL  /* 16 MHz */

/*
 * PWM FREQUENCY FIX — was 1 kHz, now 10 kHz
 * ============================================
 * At 1 kHz the motor time constant (τ = L/R ≈ 1ms) equals the PWM period.
 * During the ON phase at 50% duty (0.5ms), current only reaches 39% of max.
 * Result: insufficient torque at any speed below 100% → motor whines.
 *
 * At 10 kHz (period = 100µs << τ = 1ms), the motor inductance smooths the
 * current into essentially DC at all duty cycles → smooth torque at all speeds.
 *
 * PR2 = (OSC_FREQ / (4 x PRESCALE x FREQUENCY)) - 1
 *     = (16,000,000 / (4 x 4 x 10,000)) - 1
 *     = (16,000,000 / 160,000) - 1
 *     = 100 - 1
 *     = 99   ← fits in u8 (max 255) ✓
 *
 * PWM resolution: 4 x (PR2+1) = 400 counts — plenty for motor control.
 */
#define PWM_FREQUENCY        10000U   /* 10 kHz — was 1000U (motor whine fix) */
#define PWM_TIMER2_PRESCALE  4        /* was 16 — prescaler 1:4 for 10 kHz   */

/* PIC16F877A PWM output pins (fixed by hardware) */
#define PWM_CCP1_PIN   2    /* CCP1 output on RC2 = ENA */
#define PWM_CCP2_PIN   1    /* CCP2 output on RC1 = ENB */

#endif