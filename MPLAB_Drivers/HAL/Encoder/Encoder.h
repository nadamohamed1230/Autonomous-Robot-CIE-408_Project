/*
 * File:   Encoder.h
 * Author: Ashar Salama
 * Description: Header file for the MH-Sensor Optical Encoder
 */

#ifndef ENCODER_H
#define ENCODER_H

#include "../../SERVICES/STD_TYPES.h"
#include "Encoder_config.h" /* Bring in the hardware configuration */

/* Expose the count variables so main.c's ISR can increment them directly for speed */
extern volatile u32 Encoder_Left_Count;
extern volatile u32 Encoder_Right_Count;

/* Function Prototypes */
void Encoder_Init(void);
u32 Encoder_GetLeftCount(void);
u32 Encoder_GetRightCount(void);
void Encoder_ResetCounts(void); 

#endif  /* ENCODER_H */