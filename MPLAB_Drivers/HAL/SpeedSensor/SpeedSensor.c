/*
 * SpeedSensor.c - MPLAB XC8 Compatible
 *
 * KEY CHANGES:
 * 1. Raw volatile-pointer register accesses replaced by XC8's <xc.h>
 *    register names (OPTION_REG, INTCON).
 * 2. Global interrupt disable/enable uses di()/ei() — XC8 built-ins —
 *    which safely save and restore the GIE bit.
 */

#include <xc.h>
#include "SpeedSensor.h"
#include "SpeedSensor_config.h"
#include "../../MCAL/GPIO/GPIO_interface.h"
#include "../../SERVICES/Bit_Math.h"

volatile u16 speed_pulse_count = 0;

void SpeedSensor_Init(void)
{
    /* RB0 as input */
    GPIO_SetPinDirection(SPEEDSENSOR_PORT, SPEEDSENSOR_PIN, GPIO_INPUT);

    /* Rising-edge trigger: OPTION_REG bit 6 (INTEDG) = 1 */
    SET_BIT(OPTION_REG, 6);

    /* Enable INT0: INTCON bit 4 (INTE) = 1 */
    SET_BIT(INTCON, 4);

    /* Global interrupt enable: INTCON bit 7 (GIE) = 1 */
    SET_BIT(INTCON, 7);
}

void SpeedSensor_Increment(void)
{
    speed_pulse_count++;
}

u16 SpeedSensor_GetPulseCount(void)
{
    u16 count;

    di();                           /* Disable interrupts (XC8 built-in) */
    count = speed_pulse_count;
    speed_pulse_count = 0;
    ei();                           /* Re-enable interrupts               */

    return count;
}
