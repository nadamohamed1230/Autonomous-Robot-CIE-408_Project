/*
 * PWM.c - MPLAB XC8 Compatible
 *
 * Uses XC8's <xc.h> SFR names (T2CON, PR2, CCP1CON, CCPR1L, TRISC, etc.).
 */

#include <xc.h>
#include "PWM_interface.h"
#include "PWM_private.h"
#include "PWM_config.h"
#include "../../SERVICES/Bit_Math.h"

void PWM_Init(u8 pwm_mode, u16 frequency)
{
    u8  prescaler  = PWM_TIMER2_PRESCALE;
    u16 pr2_value;

    /* Calculate PR2: Frequency = Fosc / (4 * (PR2+1) * Prescaler) */
    pr2_value = (u16)((OSC_FREQ / (4UL * frequency * prescaler)) - 1UL);

    /* Clear Timer2 control register */
    T2CON = 0;

    /* Set prescaler bits */
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
        CCP1CON = 0x0C;   /* PWM mode */
        CCPR1L  = 0;      /* 0% duty cycle */
        CLR_BIT(TRISC, PWM_CCP1_PIN);   /* RC2 as output */
    }
    else if (pwm_mode == PWM_MODE_CCP2)
    {
        CCP2CON = 0x0C;   /* PWM mode */
        CCPR2L  = 0;      /* 0% duty cycle */
        CLR_BIT(TRISC, PWM_CCP2_PIN);   /* RC1 as output */
    }
}

void PWM_SetDutyCycle(u8 pwm_mode, u8 duty_cycle)
{
    u16 dc_value;

    if (duty_cycle > 100) duty_cycle = 100;

    /* dc_value = (PR2 + 1) * 4 * duty% / 100  (10-bit result) */
    dc_value = (u16)(((u16)(PR2 + 1) * 4u * duty_cycle) / 100u);

    if (pwm_mode == PWM_MODE_CCP1)
    {
        CCPR1L  = (u8)(dc_value >> 2);
        /* Bits 1:0 of dc_value go to CCP1CON<5:4> for full 10-bit resolution */
        CCP1CON = (CCP1CON & 0xCF) | (u8)((dc_value & 0x03) << 4);
    }
    else if (pwm_mode == PWM_MODE_CCP2)
    {
        CCPR2L  = (u8)(dc_value >> 2);
        CCP2CON = (CCP2CON & 0xCF) | (u8)((dc_value & 0x03) << 4);
    }
}

void PWM_Enable(u8 pwm_mode)
{
    (void)pwm_mode;         /* Both channels share Timer2 */
    SET_BIT(T2CON, TMR2ON);
}

void PWM_Disable(u8 pwm_mode)
{
    (void)pwm_mode;
    CLR_BIT(T2CON, TMR2ON);
}
