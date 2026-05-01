#ifndef SWITCH_H
#define SWITCH_H

#include "../../SERVICES/STD_TYPES.h"

void SWITCH_Init(u8 port, u8 pin);
u8   SWITCH_Read(u8 port, u8 pin);

#endif
