#ifndef ULTRASONIC_H
#define ULTRASONIC_H

#include "../../SERVICES/STD_TYPES.h"

/* Live distance variables updated automatically in the background */
/* CHANGED from u8 to u16 to prevent overflow on distances > 255cm */
extern volatile u16 Dist_Front;
extern volatile u16 Dist_Back;
extern volatile u16 Dist_Left;
extern volatile u16 Dist_Right;

/* Function Prototypes */
/* Function Prototypes */
void Ultrasonic_Init(void);
void Ultrasonic_Trigger_RoundRobin(void);      /* Replaces TriggerAll */
void Ultrasonic_IOC_ISR(u8 current_portb);     /* Now accepts the PORTB snapshot */

/* Getters */
u16 Ultrasonic_GetDistance(void);
u16 Ultrasonic_GetBackDistance(void);
u16 Ultrasonic_GetLeftDistance(void);
u16 Ultrasonic_GetRightDistance(void);

#endif