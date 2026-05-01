/*
 * Ultrasonic.c - MPLAB XC8 Compatible
 *
 * KEY CHANGES:
 * 1. Manual Timer1 register definitions replaced by XC8's <xc.h> names
 *    (T1CON, TMR1H, TMR1L).
 * 2. mikroC's Delay_us() replaced by the Delay_us() macro from System.h
 *    which wraps XC8's __delay_us().
 */

#include <xc.h>
#include "Ultrasonic.h"
#include "Ultrasonic_config.h"
#include "../../MCAL/GPIO/GPIO_interface.h"
#include "../../SERVICES/System.h"

void Ultrasonic_Init(void)
{
    GPIO_SetPinDirection(ULTRASONIC_PORT, ULTRASONIC_TRIG_PIN, GPIO_OUTPUT);
    GPIO_SetPinDirection(ULTRASONIC_PORT, ULTRASONIC_ECHO_PIN, GPIO_INPUT);
    GPIO_SetPinValue(ULTRASONIC_PORT, ULTRASONIC_TRIG_PIN, GPIO_LOW);

    /* Timer1: 1:4 prescaler, Timer1 OFF
     * At Fosc=16MHz: instruction clock=4MHz (0.25us/tick)
     * With 1:4 prescaler: 1 tick = 1.0 us  (ideal for distance timing) */
    T1CON = 0x20;
}

u16 Ultrasonic_GetDistance(void)
{
    u16 time_us    = 0;
    u16 timeout    = 0;

    /* Send 10 us trigger pulse */
    GPIO_SetPinValue(ULTRASONIC_PORT, ULTRASONIC_TRIG_PIN, GPIO_HIGH);
    Delay_us(10);
    GPIO_SetPinValue(ULTRASONIC_PORT, ULTRASONIC_TRIG_PIN, GPIO_LOW);

    /* Wait for Echo HIGH (start of return pulse) */
    while (GPIO_GetPinValue(ULTRASONIC_PORT, ULTRASONIC_ECHO_PIN) == GPIO_LOW)
    {
        timeout++;
        if (timeout > 20000u) return 0;   /* Sensor not connected / no object */
    }

    /* Reset and start Timer1 */
    TMR1H = 0;
    TMR1L = 0;
    T1CON |= 0x01;   /* TMR1ON = 1 */

    /* Wait for Echo LOW (end of return pulse) */
    while (GPIO_GetPinValue(ULTRASONIC_PORT, ULTRASONIC_ECHO_PIN) == GPIO_HIGH)
    {
        if (TMR1H > 250u) break;   /* Overflow guard (~65 ms) */
    }

    /* Stop Timer1 */
    T1CON &= (u8)~0x01;

    /* Read 16-bit elapsed time in microseconds */
    time_us = ((u16)TMR1H << 8) | TMR1L;

    /* Distance (cm) = time_us / 58
     * (Speed of sound ~343 m/s; round-trip halved gives /58 us/cm) */
    return (u16)(time_us / 58u);
}
