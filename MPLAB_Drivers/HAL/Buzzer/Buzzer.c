#include "Buzzer.h"
#include "Buzzer_config.h"
#include "../../MCAL/GPIO/GPIO_interface.h"

void Buzzer_Init(void)
{
    /* Set the buzzer pin as an output */
    GPIO_SetPinDirection(BUZZER_PORT, BUZZER_PIN, GPIO_OUTPUT);
    
    /* Ensure the buzzer starts turned off */
    Buzzer_Off();
}

void Buzzer_On(void)
{
    GPIO_SetPinValue(BUZZER_PORT, BUZZER_PIN, BUZZER_ON_STATE);
}

void Buzzer_Off(void)
{
    /* The "OFF" state is simply the logical NOT of the "ON" state */
    if (BUZZER_ON_STATE == GPIO_HIGH)
    {
        GPIO_SetPinValue(BUZZER_PORT, BUZZER_PIN, GPIO_LOW);
    }
    else
    {
        GPIO_SetPinValue(BUZZER_PORT, BUZZER_PIN, GPIO_HIGH);
    }
}

void Buzzer_Toggle(void)
{
    /* Read current state and invert it */
    u8 current_state = GPIO_GetPinValue(BUZZER_PORT, BUZZER_PIN);
    
    if (current_state == GPIO_HIGH)
    {
        GPIO_SetPinValue(BUZZER_PORT, BUZZER_PIN, GPIO_LOW);
    }
    else
    {
        GPIO_SetPinValue(BUZZER_PORT, BUZZER_PIN, GPIO_HIGH);
    }
}