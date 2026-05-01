#ifndef PWM_INTERFACE_H
#define PWM_INTERFACE_H

#include "../../SERVICES/STD_TYPES.h"

/* PWM channel selection */
#define PWM_MODE_CCP1   0
#define PWM_MODE_CCP2   1

void PWM_Init(u8 pwm_mode, u16 frequency);
void PWM_SetDutyCycle(u8 pwm_mode, u8 duty_cycle);
void PWM_Enable(u8 pwm_mode);
void PWM_Disable(u8 pwm_mode);

#endif
