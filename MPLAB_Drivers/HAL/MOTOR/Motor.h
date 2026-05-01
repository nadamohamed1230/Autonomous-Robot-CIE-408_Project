#ifndef MOTOR_H
#define MOTOR_H

#include "../../SERVICES/STD_TYPES.h"

#define MOTOR_DUTY_25   25
#define MOTOR_DUTY_50   50
#define MOTOR_DUTY_75   75
#define MOTOR_DUTY_100  100

void Motor_Init(void);
void Motor_SetSpeed(u8 duty_cycle);
void Motor_SpeedUp(void);
void Motor_Start(void);
void Motor_Stop(void);
u8   Motor_GetCurrentSpeed(void);

#endif
