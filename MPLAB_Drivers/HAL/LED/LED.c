/*
 * LED.c - MPLAB XC8 Compatible
 * No direct register access; uses GPIO abstraction. No changes needed.
 */

#include "LED.h"
#include "../../MCAL/GPIO/GPIO_interface.h"

void LED_Init(u8 Port, u8 Pin)
{
    GPIO_SetPinDirection(Port, Pin, GPIO_OUTPUT);
}

void LED_On(u8 Port, u8 Pin)
{
    GPIO_SetPinValue(Port, Pin, GPIO_HIGH);
}

void LED_Off(u8 Port, u8 Pin)
{
    GPIO_SetPinValue(Port, Pin, GPIO_LOW);
}

void LED_Toggle(u8 Port, u8 Pin)
{
    u8 current = GPIO_GetPinValue(Port, Pin);
    GPIO_SetPinValue(Port, Pin, (current == GPIO_HIGH) ? GPIO_LOW : GPIO_HIGH);
}
