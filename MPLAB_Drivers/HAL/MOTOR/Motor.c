/*
 * Motor.c - MPLAB XC8 Compatible
 * No direct register access; uses PWM abstraction. No changes needed.
 */

#include "Motor.h"
#include "../../MCAL/PWM/PWM_interface.h"

static u8 current_speed_index = 0;
static const u8 speed_levels[4] = {25, 50, 75, 100};

void Motor_Init(void)
{
    PWM_Init(PWM_MODE_CCP1, 5000);
    PWM_Enable(PWM_MODE_CCP1);
    Motor_SetSpeed(MOTOR_DUTY_25);
}

void Motor_SetSpeed(u8 duty_cycle)
{
    PWM_SetDutyCycle(PWM_MODE_CCP1, duty_cycle);
}

void Motor_SpeedUp(void)
{
    current_speed_index = (current_speed_index + 1u) % 4u;
    Motor_SetSpeed(speed_levels[current_speed_index]);
}

void Motor_Start(void)
{
    PWM_Enable(PWM_MODE_CCP1);
}

void Motor_Stop(void)
{
    PWM_Disable(PWM_MODE_CCP1);
    current_speed_index = 0;
}

u8 Motor_GetCurrentSpeed(void)
{
    return speed_levels[current_speed_index];
}
