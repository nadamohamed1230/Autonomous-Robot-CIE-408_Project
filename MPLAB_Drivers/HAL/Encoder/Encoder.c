/*
 * File:   Encoder.c
 * Author: Ashar Salama
 * Description: Source file for the MH-Sensor Optical Encoders (Left & Right)
 */

#include <xc.h>
#include "Encoder.h"
#include "../../SERVICES/Bit_Math.h"

/* Define the actual variables in memory */
volatile u32 Encoder_Left_Count = 0;
volatile u32 Encoder_Right_Count = 0;

void Encoder_Init(void)
{
    /* ========================================================
     * 1. LEFT SENSOR CONFIGURATION (External Interrupt INT0)
     * ======================================================== */
    /* Set Left Pin as an Input */
    SET_BIT(ENCODER_LEFT_TRIS, ENCODER_LEFT_PIN_BIT);

    /* Trigger interrupt on the Rising Edge */
    SET_BIT(OPTION_REG, 6); // OPTION_REG INTEDG bit

    /* Clear the interrupt flag so it starts fresh */
    CLR_BIT(INTCON, 1);     // INTCON INTF bit

    /* Enable the INT0 External Interrupt */
    SET_BIT(INTCON, 4);     // INTCON INTE bit

    /* ========================================================
     * 2. RIGHT SENSOR CONFIGURATION (Interrupt On Change)
     * ======================================================== */
    /* Set Right Pin as an Input */
    SET_BIT(ENCODER_RIGHT_TRIS, ENCODER_RIGHT_PIN_BIT);
    
    /* Hardware Quirk: Read PORT to establish the initial state 
       before clearing the flag and enabling the interrupt */
    u8 dummy = ENCODER_RIGHT_PORT;       
    
    /* Clear the Interrupt On Change flag */
    CLR_BIT(INTCON, 0);     // INTCON RBIF bit
    
    /* Enable the Interrupt On Change */
    SET_BIT(INTCON, 3);     // INTCON RBIE bit
    
    /* Note: Global and Peripheral interrupts are already enabled in main.c */
}

u32 Encoder_GetLeftCount(void)
{
    /* Return the current tick count for the left wheel */
    return Encoder_Left_Count;
}

u32 Encoder_GetRightCount(void)
{
    /* Return the current tick count for the right wheel */
    return Encoder_Right_Count;
}

void Encoder_ResetCounts(void)
{
    /* Zero out the counters (Used when starting a new parking maneuver) */
    Encoder_Left_Count = 0;
    Encoder_Right_Count = 0;
}