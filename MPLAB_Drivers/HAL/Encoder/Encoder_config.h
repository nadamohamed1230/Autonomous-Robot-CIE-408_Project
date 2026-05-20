/*
 * File:   Encoder_config.h
 * Author: Ashar Salama
 * Description: Configuration file for the Optical Encoders
 */

#ifndef ENCODER_CONFIG_H
#define ENCODER_CONFIG_H

/* =========================================================================
 * HARDWARE MAPPING
 * NOTE: Due to PIC16F877A hardware limitations:
 * - Left Encoder MUST be on RB0 (to trigger INT0 interrupt)
 * - Right Encoder MUST be on RB4-RB7 (to trigger Interrupt-On-Change)
 * ========================================================================= */

/* Left Encoder (INT0) */
#define ENCODER_LEFT_PORT       PORTB
#define ENCODER_LEFT_TRIS       TRISB
#define ENCODER_LEFT_PIN_BIT    0

/* Right Encoder (Interrupt On Change) */
#define ENCODER_RIGHT_PORT      PORTB
#define ENCODER_RIGHT_TRIS      TRISB
#define ENCODER_RIGHT_PIN_BIT   4

#endif  /* ENCODER_CONFIG_H */