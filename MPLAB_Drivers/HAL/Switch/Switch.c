/*
 * Switch.c - MPLAB XC8 Compatible
 * No direct register access; uses GPIO abstraction. No changes needed.
 */

#include "Switch.h"
#include "../../MCAL/GPIO/GPIO_interface.h"

void SWITCH_Init(u8 port, u8 pin)
{
    GPIO_SetPinDirection(port, pin, GPIO_INPUT);
}

u8 SWITCH_Read(u8 port, u8 pin)
{
    return GPIO_GetPinValue(port, pin);
}
