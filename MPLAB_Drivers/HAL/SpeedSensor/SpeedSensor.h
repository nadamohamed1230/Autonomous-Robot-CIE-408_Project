#ifndef SPEEDSENSOR_H
#define SPEEDSENSOR_H

#include "../../SERVICES/STD_TYPES.h"

void SpeedSensor_Init(void);
u16  SpeedSensor_GetPulseCount(void);
void SpeedSensor_Increment(void);

#endif
