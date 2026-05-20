/*
 * H_Bridge.c - Updated with integrated Speed Control (PWM)
 */

#include "H_Bridge.h"
#include "H_Bridge_config.h"
#include "../../MCAL/GPIO/GPIO_interface.h"
#include "../../MCAL/PWM/PWM_interface.h"

void H_Bridge_Init(void)
{
    /* 1. Set Direction Pins as Output (PORTD) */
    GPIO_SetPinDirection(H_BRIDGE_DIR_PORT, H_BRIDGE_IN1_PIN, GPIO_OUTPUT);
    GPIO_SetPinDirection(H_BRIDGE_DIR_PORT, H_BRIDGE_IN2_PIN, GPIO_OUTPUT);
    GPIO_SetPinDirection(H_BRIDGE_DIR_PORT, H_BRIDGE_IN3_PIN, GPIO_OUTPUT);
    GPIO_SetPinDirection(H_BRIDGE_DIR_PORT, H_BRIDGE_IN4_PIN, GPIO_OUTPUT);

    /* 2. Force the PWM pins (RC1/CCP2 and RC2/CCP1) to be Outputs */
    GPIO_SetPinDirection(GPIO_PORTC, GPIO_PIN1, GPIO_OUTPUT); // ENB
    GPIO_SetPinDirection(GPIO_PORTC, GPIO_PIN2, GPIO_OUTPUT); // ENA

    /* 3. Initialize PWM Channels (CCP1 and CCP2) */
    PWM_Init(H_BRIDGE_PWM_MODE_A, H_BRIDGE_PWM_FREQ);
    PWM_Init(H_BRIDGE_PWM_MODE_B, H_BRIDGE_PWM_FREQ);

    /* 4. Start in a safe state */
    H_Bridge_Stop();
    
    /* 5. Enable the PWM output */
    PWM_Enable(H_BRIDGE_PWM_MODE_A);
    PWM_Enable(H_BRIDGE_PWM_MODE_B);
}

/**
 * @brief Sets the speed for both motors using PWM
 * @param speed_percentage Value from 0 to 100
 */
void H_Bridge_SetSpeed(u8 speed_percentage)
{
    /* H_BRIDGE_PWM_MODE_A is ENA (Left), MODE_B is ENB (Right) */
    PWM_SetDutyCycle(H_BRIDGE_PWM_MODE_A, speed_percentage); 
    PWM_SetDutyCycle(H_BRIDGE_PWM_MODE_B, speed_percentage);
}

void H_Bridge_SetMotorSpeeds(u8 left_speed, u8 right_speed)
{
    PWM_SetDutyCycle(H_BRIDGE_PWM_MODE_A, left_speed); 
    PWM_SetDutyCycle(H_BRIDGE_PWM_MODE_B, right_speed);
}

void H_Bridge_SetSpeedLeft(u8 speed)
{
    /* Update ONLY the Left Motor */
    PWM_SetDutyCycle(H_BRIDGE_PWM_MODE_A, speed);
}

void H_Bridge_SetSpeedRight(u8 speed)
{
    /* Update ONLY the Right Motor */
    PWM_SetDutyCycle(H_BRIDGE_PWM_MODE_B, speed);
}


/* ============================================================================
   MOVEMENT COMMANDS
   ============================================================================ */

void H_Bridge_Forward(u8 speed)
{
    /* Left Forward (IN1=1, IN2=0) | Right Forward (IN3=1, IN4=0) */
    GPIO_SetPinValue(H_BRIDGE_DIR_PORT, H_BRIDGE_IN1_PIN, GPIO_HIGH);
    GPIO_SetPinValue(H_BRIDGE_DIR_PORT, H_BRIDGE_IN2_PIN, GPIO_LOW);
    
    GPIO_SetPinValue(H_BRIDGE_DIR_PORT, H_BRIDGE_IN3_PIN, GPIO_HIGH);
    GPIO_SetPinValue(H_BRIDGE_DIR_PORT, H_BRIDGE_IN4_PIN, GPIO_LOW);

    /* Apply Speed */
    H_Bridge_SetSpeed(speed);
}

void H_Bridge_Reverse(u8 speed)
{
    /* Left Reverse (IN1=0, IN2=1) | Right Reverse (IN3=0, IN4=1) */
    GPIO_SetPinValue(H_BRIDGE_DIR_PORT, H_BRIDGE_IN1_PIN, GPIO_LOW);
    GPIO_SetPinValue(H_BRIDGE_DIR_PORT, H_BRIDGE_IN2_PIN, GPIO_HIGH);
    
    GPIO_SetPinValue(H_BRIDGE_DIR_PORT, H_BRIDGE_IN3_PIN, GPIO_LOW);
    GPIO_SetPinValue(H_BRIDGE_DIR_PORT, H_BRIDGE_IN4_PIN, GPIO_HIGH);

    /* Apply Speed */
    H_Bridge_SetSpeed(speed);
}

void H_Bridge_TurnRight(u8 speed)
{
    /* Left Forward (IN1=1, IN2=0) | Right Reverse (IN3=0, IN4=1) */
    GPIO_SetPinValue(H_BRIDGE_DIR_PORT, H_BRIDGE_IN1_PIN, GPIO_HIGH);
    GPIO_SetPinValue(H_BRIDGE_DIR_PORT, H_BRIDGE_IN2_PIN, GPIO_LOW);
    
    GPIO_SetPinValue(H_BRIDGE_DIR_PORT, H_BRIDGE_IN3_PIN, GPIO_LOW);
    GPIO_SetPinValue(H_BRIDGE_DIR_PORT, H_BRIDGE_IN4_PIN, GPIO_HIGH);

    /* Apply Speed */
    H_Bridge_SetSpeed(speed);
}

void H_Bridge_TurnLeft(u8 speed)
{
    /* Left Reverse (IN1=0, IN2=1) | Right Forward (IN3=1, IN4=0) */
    GPIO_SetPinValue(H_BRIDGE_DIR_PORT, H_BRIDGE_IN1_PIN, GPIO_LOW);
    GPIO_SetPinValue(H_BRIDGE_DIR_PORT, H_BRIDGE_IN2_PIN, GPIO_HIGH);
    
    GPIO_SetPinValue(H_BRIDGE_DIR_PORT, H_BRIDGE_IN3_PIN, GPIO_HIGH);
    GPIO_SetPinValue(H_BRIDGE_DIR_PORT, H_BRIDGE_IN4_PIN, GPIO_LOW);

    /* Apply Speed */
    H_Bridge_SetSpeed(speed);
}

void H_Bridge_Stop(void)
{
    /* Brake: All pins Low */
    GPIO_SetPinValue(H_BRIDGE_DIR_PORT, H_BRIDGE_IN1_PIN, GPIO_LOW);
    GPIO_SetPinValue(H_BRIDGE_DIR_PORT, H_BRIDGE_IN2_PIN, GPIO_LOW);
    GPIO_SetPinValue(H_BRIDGE_DIR_PORT, H_BRIDGE_IN3_PIN, GPIO_LOW);
    GPIO_SetPinValue(H_BRIDGE_DIR_PORT, H_BRIDGE_IN4_PIN, GPIO_LOW);

    /* Force PWM to 0% for absolute safety */
    H_Bridge_SetSpeed(0);
}

void H_Bridge_CutMotors(void)
{
    H_Bridge_Stop();
}