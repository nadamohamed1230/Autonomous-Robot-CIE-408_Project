/*
 * I2C_interface.h
 * Master Mode I2C Driver for PIC16F877A
 */

#ifndef I2C_INTERFACE_H
#define I2C_INTERFACE_H

/* =========================================================
   Function Prototypes
   ========================================================= */

/* Initializes the I2C module with a specific baud rate (e.g., 100000 for 100kHz) */
void I2C_Init(const unsigned long baud_rate);

/* Waits until the I2C bus is completely idle to prevent collisions */
void I2C_Wait(void);

/* Sends a Start condition */
void I2C_Start(void);

/* Sends a Repeated Start condition (used when changing from Write to Read) */
void I2C_RepeatedStart(void);

/* Sends a Stop condition to release the bus */
void I2C_Stop(void);

/* Transmits one byte of data over the bus */
void I2C_Write(unsigned char data);

/* 
 * Reads one byte of data from the bus.
 * Parameter 'ack': 0 to send ACK (more bytes coming), 1 to send NACK (last byte)
 */
unsigned char I2C_Read(unsigned char ack);

#endif /* I2C_INTERFACE_H */