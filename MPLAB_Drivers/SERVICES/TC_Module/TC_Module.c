/*
 * ============================================================================
 * TC_Module.c — Traction Control & Motor Command Module
 * ============================================================================
 */

#include "TC_Module.h"
#include "../../HAL/H_Bridge/H_Bridge.h"
#include "../../HAL/Encoder/Encoder.h"

/* Internal State Variables */
static volatile u8  tc_tick_counter = 0;
static volatile u8  rpm_window_ready = 0;

static u16 current_left_rpm = 0;
static u16 current_right_rpm = 0;

/* Snapshots of encoder counts for calculating the difference */
static u32 last_left_count = 0;
static u32 last_right_count = 0;

/* * Encoder PPR (Pulses Per Revolution). 
 * Adjust this if your encoder wheel has more/less than 20 slots!
 */
#define ENCODER_PPR 20U 
#define RPM_MULTIPLIER 600UL /* (60s * 1000ms/96ms) / 20 PPR ≈ 31.25 -> Simplified multiplier */

/* ============================================================================
   IMPLEMENTATIONS
   ============================================================================ */

void TC_Init(void) {
    tc_tick_counter = 0;
    rpm_window_ready = 0;
    current_left_rpm = 0;
    current_right_rpm = 0;
    
    last_left_count = Encoder_Left_Count;
    last_right_count = Encoder_Right_Count;
}

void TractionControl_Tick(void) {
    tc_tick_counter++;
    
    /* If 6 ticks (~96ms) have passed, flag the main loop to calculate RPM */
    if (tc_tick_counter >= TC_RPM_WINDOW_TICKS) {
        rpm_window_ready = 1;
        tc_tick_counter = 0;
    }
}

u8 TC_IsRPMWindowReady(void) {
    return rpm_window_ready;
}

void TC_UpdateRPM(void) {
    /* 1. Clear the flag */
    rpm_window_ready = 0;

    /* 2. Safely grab the current ticks */
    u32 current_left = Encoder_Left_Count;
    u32 current_right = Encoder_Right_Count;

    /* 3. Calculate how many ticks happened in the last ~96ms */
    u32 delta_left = current_left - last_left_count;
    u32 delta_right = current_right - last_right_count;

    /* 4. Update snapshots for next time */
    last_left_count = current_left;
    last_right_count = current_right;

    /* 5. Convert ticks to actual RPM */
    current_left_rpm = (u16)((delta_left * RPM_MULTIPLIER) / ENCODER_PPR);
    current_right_rpm = (u16)((delta_right * RPM_MULTIPLIER) / ENCODER_PPR);
}

u16 TC_GetLeftRPM(void) {
    return current_left_rpm;
}

u16 TC_GetRightRPM(void) {
    return current_right_rpm;
}

void TC_ApplyCommand(u8 cmd, u8 speed_percent) {
    /* Safety check: cap speed at 100% */
    if (speed_percent > 100) {
        speed_percent = 100;
    }

    /* Route the command to the actual H-Bridge hardware */
    switch (cmd) {
        case CMD_FORWARD:
            H_Bridge_Forward(speed_percent);
            break;
            
        case CMD_REVERSE:
            H_Bridge_Reverse(speed_percent);
            break;
            
        case CMD_LEFT:
            H_Bridge_TurnLeft(speed_percent);
            break;
            
        case CMD_RIGHT:
            H_Bridge_TurnRight(speed_percent);
            break;
            
        case CMD_STOP:
        default:
            H_Bridge_Stop();
            break;
    }
}