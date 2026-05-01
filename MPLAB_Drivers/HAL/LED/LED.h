#ifndef LED_H
#define LED_H

#include "../../SERVICES/STD_TYPES.h"

#define LED_OFF  0
#define LED_ON   1

void LED_Init(u8 Port, u8 Pin);
void LED_On(u8 Port, u8 Pin);
void LED_Off(u8 Port, u8 Pin);
void LED_Toggle(u8 Port, u8 Pin);

#endif
