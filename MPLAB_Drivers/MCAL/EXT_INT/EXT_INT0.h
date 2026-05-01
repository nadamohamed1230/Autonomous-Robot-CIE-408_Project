#ifndef EXT_INT0_H
#define EXT_INT0_H

#include "../../SERVICES/STD_TYPES.h"
#include "EXT_INT0_Cfg.h"

void EXT_INT0_Init(void);
void EXT_INT0_Enable(void);
void EXT_INT0_Disable(void);
void EXT_INT0_SetEdge(u8 edge_type);
void EXT_INT0_SetCallback(void (*ptr)(void));
void EXT_INT0_ISR_Handler(void);

#endif
