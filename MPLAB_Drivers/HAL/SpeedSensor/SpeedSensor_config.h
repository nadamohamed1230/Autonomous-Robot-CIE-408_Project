#ifndef SPEEDSENSOR_CONFIG_H
#define SPEEDSENSOR_CONFIG_H

#include "../../MCAL/GPIO/GPIO_interface.h"

/* D0 pin of MH speed sensor MUST be on RB0 (INT0 pin) */
#define SPEEDSENSOR_PORT  GPIO_PORTB
#define SPEEDSENSOR_PIN   GPIO_PIN0

#endif
