/*
 * ============================================================================
 * TC_Module.h — Traction Control & Motor Command Module
 * Target  : PIC16F877A @ 16 MHz
 * ============================================================================
 */

#ifndef TC_MODULE_H
#define TC_MODULE_H

#include "../../SERVICES/STD_TYPES.h"

/* Command Definitions (Must match main.c) */
#define CMD_STOP      0x00U
#define CMD_FORWARD   0x01U
#define CMD_REVERSE   0x02U
#define CMD_LEFT      0x03U
#define CMD_RIGHT     0x04U

/* * 6 ticks of Timer0 (16ms * 6 = ~96ms window) 
 * Used to calculate RPM consistently
 */
#define TC_RPM_WINDOW_TICKS  6U 

/* ============================================================================
   FUNCTION PROTOTYPES
   ============================================================================ */

/* Initialize the module */
void TC_Init(void);

/* Called every 16ms inside the Timer0 Interrupt Callback */
void TractionControl_Tick(void);

/* Applies the requested movement and speed to the H-Bridge */
void TC_ApplyCommand(u8 cmd, u8 speed_percent);

/* Returns 1 if the 96ms window has passed and RPM needs calculating */
u8 TC_IsRPMWindowReady(void);

/* Calculates the exact RPM based on encoder ticks since the last check */
void TC_UpdateRPM(void);

/* Getters for Telemetry */
u16 TC_GetLeftRPM(void);
u16 TC_GetRightRPM(void);

#endif /* TC_MODULE_H */