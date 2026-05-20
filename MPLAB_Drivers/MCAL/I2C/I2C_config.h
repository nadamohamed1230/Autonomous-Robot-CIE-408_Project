/*
 * I2C_config.h - MPLAB XC8 Compatible
 * 
 * User configuration settings for the I2C Master Driver.
 * This keeps hardware-specific settings out of the main application.
 */

#ifndef I2C_CONFIG_H
#define I2C_CONFIG_H

/* 
 * ---------------------------------------------------------
 * I2C MASTER CLOCK SPEED (Baud Rate)
 * ---------------------------------------------------------
 * Standard Mode: 100000  (100 kHz) - Best for LCDs (PCF8574)
 * Fast Mode:     400000  (400 kHz) - For high-speed sensors
 * 1 MHz Mode:    1000000 (1 MHz)   - Requires specific EEPROMs
 */
#define I2C_MASTER_BAUD_RATE    100000

#endif /* I2C_CONFIG_H */