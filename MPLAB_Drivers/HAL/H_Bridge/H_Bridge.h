#ifndef H_BRIDGE_H
#define H_BRIDGE_H

#include "../../SERVICES/STD_TYPES.h"

void H_Bridge_Init(void);
void H_Bridge_Forward(u8 speed);
void H_Bridge_Reverse(u8 speed);
void H_Bridge_TurnLeft(u8 speed);
void H_Bridge_TurnRight(u8 speed);
void H_Bridge_Stop(void); /* Stop stays as void because speed is 0 */
void H_Bridge_CutMotors(void);

#endif