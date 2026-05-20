#ifndef BUZZER_CONFIG_H
#define BUZZER_CONFIG_H

#include "../../MCAL/GPIO/GPIO_interface.h"

/* Define the physical hardware connection */
#define BUZZER_PORT     GPIO_PORTC
#define BUZZER_PIN      GPIO_PIN5

/* Define hardware behavior: 
   Some buzzers turn on with 5V (HIGH), some with 0V (LOW). 
   We define the "ON" state here so the logic is adaptable. */
#define BUZZER_ON_STATE GPIO_HIGH

#endif /* BUZZER_CONFIG_H */