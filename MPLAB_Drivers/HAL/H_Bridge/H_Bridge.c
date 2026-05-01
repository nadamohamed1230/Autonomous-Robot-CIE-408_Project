/*
 * H_Bridge.c - MPLAB XC8 Compatible
 *
 * No direct register access here - everything goes through GPIO and PWM
 * drivers, so no changes from the original logic are needed.
 */

#include "H_Bridge.h"
#include "H_Bridge_config.h"
#include "../../MCAL/GPIO/GPIO_interface.h"
#include "../../MCAL/PWM/PWM_interface.h"

void H_Bridge_Init(void)
{
    GPIO_SetPinDirection(H_BRIDGE_DIR_PORT, H_BRIDGE_IN1_PIN, GPIO_OUTPUT);
    GPIO_SetPinDirection(H_BRIDGE_DIR_PORT, H_BRIDGE_IN2_PIN, GPIO_OUTPUT);
    GPIO_SetPinDirection(H_BRIDGE_DIR_PORT, H_BRIDGE_IN3_PIN, GPIO_OUTPUT);
    GPIO_SetPinDirection(H_BRIDGE_DIR_PORT, H_BRIDGE_IN4_PIN, GPIO_OUTPUT);

    PWM_Init(H_BRIDGE_PWM_MODE_A, H_BRIDGE_PWM_FREQ);
    PWM_Init(H_BRIDGE_PWM_MODE_B, H_BRIDGE_PWM_FREQ);

    H_Bridge_Stop();
    PWM_Enable(H_BRIDGE_PWM_MODE_A);
    PWM_Enable(H_BRIDGE_PWM_MODE_B);
}

void H_Bridge_SetSpeed(u8 duty_cycle)
{
    PWM_SetDutyCycle(H_BRIDGE_PWM_MODE_A, duty_cycle);
    PWM_SetDutyCycle(H_BRIDGE_PWM_MODE_B, duty_cycle);
}

void H_Bridge_Forward(void)
{
    GPIO_SetPinValue(H_BRIDGE_DIR_PORT, H_BRIDGE_IN1_PIN, GPIO_HIGH);
    GPIO_SetPinValue(H_BRIDGE_DIR_PORT, H_BRIDGE_IN2_PIN, GPIO_LOW);
    GPIO_SetPinValue(H_BRIDGE_DIR_PORT, H_BRIDGE_IN3_PIN, GPIO_HIGH);
    GPIO_SetPinValue(H_BRIDGE_DIR_PORT, H_BRIDGE_IN4_PIN, GPIO_LOW);
}

void H_Bridge_Reverse(void)
{
    GPIO_SetPinValue(H_BRIDGE_DIR_PORT, H_BRIDGE_IN1_PIN, GPIO_LOW);
    GPIO_SetPinValue(H_BRIDGE_DIR_PORT, H_BRIDGE_IN2_PIN, GPIO_HIGH);
    GPIO_SetPinValue(H_BRIDGE_DIR_PORT, H_BRIDGE_IN3_PIN, GPIO_LOW);
    GPIO_SetPinValue(H_BRIDGE_DIR_PORT, H_BRIDGE_IN4_PIN, GPIO_HIGH);
}

void H_Bridge_TurnRight(void)
{
    /* Left side forward, right side reverse */
    GPIO_SetPinValue(H_BRIDGE_DIR_PORT, H_BRIDGE_IN1_PIN, GPIO_HIGH);
    GPIO_SetPinValue(H_BRIDGE_DIR_PORT, H_BRIDGE_IN2_PIN, GPIO_LOW);
    GPIO_SetPinValue(H_BRIDGE_DIR_PORT, H_BRIDGE_IN3_PIN, GPIO_LOW);
    GPIO_SetPinValue(H_BRIDGE_DIR_PORT, H_BRIDGE_IN4_PIN, GPIO_HIGH);
}

void H_Bridge_TurnLeft(void)
{
    /* Left side reverse, right side forward */
    GPIO_SetPinValue(H_BRIDGE_DIR_PORT, H_BRIDGE_IN1_PIN, GPIO_LOW);
    GPIO_SetPinValue(H_BRIDGE_DIR_PORT, H_BRIDGE_IN2_PIN, GPIO_HIGH);
    GPIO_SetPinValue(H_BRIDGE_DIR_PORT, H_BRIDGE_IN3_PIN, GPIO_HIGH);
    GPIO_SetPinValue(H_BRIDGE_DIR_PORT, H_BRIDGE_IN4_PIN, GPIO_LOW);
}

void H_Bridge_Stop(void)
{
    GPIO_SetPinValue(H_BRIDGE_DIR_PORT, H_BRIDGE_IN1_PIN, GPIO_LOW);
    GPIO_SetPinValue(H_BRIDGE_DIR_PORT, H_BRIDGE_IN2_PIN, GPIO_LOW);
    GPIO_SetPinValue(H_BRIDGE_DIR_PORT, H_BRIDGE_IN3_PIN, GPIO_LOW);
    GPIO_SetPinValue(H_BRIDGE_DIR_PORT, H_BRIDGE_IN4_PIN, GPIO_LOW);
}
