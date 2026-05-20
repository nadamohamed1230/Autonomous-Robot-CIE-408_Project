/*
 * PWM.c - MPLAB XC8 Compatible
 *
 * FIX: PWM_Init() now starts Timer2 (SET_BIT T2CON TMR2ON) after
 * configuring all registers. Previously Timer2 was never started,
 * so CCP output was permanently LOW — ENA/ENB = 0V — no motor current.
 */

#include <xc.h>
#include "PWM_interface.h"
#include "PWM_private.h"
#include "PWM_config.h"
#include "../../SERVICES/Bit_Math.h"

void PWM_Init(u8 pwm_mode, u16 frequency)
{
    u8  prescaler = PWM_TIMER2_PRESCALE;
    u16 pr2_value;

    /* Calculate PR2: Frequency = Fosc / (4 x (PR2+1) x Prescaler) */
    pr2_value = (u16)((OSC_FREQ / (4UL * frequency * prescaler)) - 1UL);

    /* Clear Timer2 control register */
    T2CON = 0;

    /* Set prescaler bits in T2CON */
    if (prescaler == 1)
    {
        CLR_BIT(T2CON, T2CKPS0);
        CLR_BIT(T2CON, T2CKPS1);
    }
    else if (prescaler == 4)
    {
        SET_BIT(T2CON, T2CKPS0);
        CLR_BIT(T2CON, T2CKPS1);
    }
    else /* prescaler == 16 */
    {
        CLR_BIT(T2CON, T2CKPS0);
        SET_BIT(T2CON, T2CKPS1);
    }

    PR2 = (u8)pr2_value;

    if (pwm_mode == PWM_MODE_CCP1)
    {
        CCP1CON = 0x0C;                  /* PWM mode           */
        CCPR1L  = 0;                     /* 0% duty initially  */
        CLR_BIT(TRISC, PWM_CCP1_PIN);   /* RC2 as output      */
    }
    else if (pwm_mode == PWM_MODE_CCP2)
    {
        CCP2CON = 0x0C;                  /* PWM mode           */
        CCPR2L  = 0;                     /* 0% duty initially  */
        CLR_BIT(TRISC, PWM_CCP2_PIN);   /* RC1 as output      */
    }

    /* ── FIX: Start Timer2 ────────────────────────────────────────────────
     * Without this line, T2CON.TMR2ON stays 0 → Timer2 never runs →
     * CCP has no clock → output pin held LOW → ENA/ENB = 0V → no motion.
     * This was the root cause of all motor commands being silently ignored.
     * ─────────────────────────────────────────────────────────────────────*/
    SET_BIT(T2CON, TMR2ON);
}

void PWM_SetDutyCycle(u8 pwm_mode, u8 duty_cycle)
{
    u16 dc_value;

    if (duty_cycle > 100U) duty_cycle = 100U;

    /* dc_value = (PR2 + 1) x 4 x duty% / 100  (10-bit result) */
    dc_value = (u16)(((u16)(PR2 + 1U) * 4U * duty_cycle) / 100U);

    if (pwm_mode == PWM_MODE_CCP1)
    {
        CCPR1L  = (u8)(dc_value >> 2U);
        CCP1CON = (CCP1CON & 0xCFU) | (u8)((dc_value & 0x03U) << 4U);
    }
    else if (pwm_mode == PWM_MODE_CCP2)
    {
        CCPR2L  = (u8)(dc_value >> 2U);
        CCP2CON = (CCP2CON & 0xCFU) | (u8)((dc_value & 0x03U) << 4U);
    }
}

void PWM_Enable(u8 pwm_mode)
{
    (void)pwm_mode;           /* Both channels share Timer2 */
    SET_BIT(T2CON, TMR2ON);
}

void PWM_Disable(u8 pwm_mode)
{
    (void)pwm_mode;
    CLR_BIT(T2CON, TMR2ON);
}