#ifndef H_BRIDGE_H
#define H_BRIDGE_H

#include "../../SERVICES/STD_TYPES.h"

void H_Bridge_Init(void);
void H_Bridge_SetSpeed(u8 duty_cycle);
void H_Bridge_Forward(void);
void H_Bridge_Reverse(void);
void H_Bridge_TurnRight(void);
void H_Bridge_TurnLeft(void);
void H_Bridge_Stop(void);

#endif
