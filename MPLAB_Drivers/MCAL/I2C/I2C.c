/*
 * I2C.c - MPLAB XC8 Compatible
 * 
 * Master Mode I2C Driver for PIC16F877A
 * Includes hardware polling (I2C_Wait) to prevent SSPBUF collisions.
 */

#include <xc.h>
#include "../../SERVICES/STD_TYPES.h"
#include "../../SERVICES/System.h" /* For _XTAL_FREQ */
#include "I2C_interface.h"
#include "I2C_config.h"
#include "I2C_private.h"

/* =========================================================
   Wait Function (The Traffic Controller)
   Halts the processor until the I2C bus is completely idle.
   ========================================================= */
void I2C_Wait(void)
{
    /* 
     * SSPSTATbits.R_nW : Transmit in progress flag
     * SSPCON2 & 0x1F   : Start, Stop, Repeated Start, Receive, and Acknowledge flags
     * Loop runs until ALL of these hardware flags drop to 0.
     */
    while ((SSPCON2 & 0x1F) || (SSPSTATbits.R_nW));
}

/* =========================================================
   Initialization
   ========================================================= */
void I2C_Init(const unsigned long baud_rate)
{
    /* Set SDA (RC4) and SCL (RC3) as input pins to let the pull-up resistors work */
    TRISCbits.TRISC3 = 1;
    TRISCbits.TRISC4 = 1;

    /* 
     * SSPCON: 
     * SSPEN = 1 (Enable MSSP)
     * SSPM = 1000 (I2C Master mode, clock = FOSC / (4 * (SSPADD+1)))
     */
    SSPCON = 0x28; 

    /* SSPCON2: Clear all bits */
    SSPCON2 = 0x00;

    /* SSPSTAT: Slew rate control disabled for Standard Speed (100kHz and 1MHz) */
    SSPSTAT = 0x80;

    /* Calculate baud rate dynamically based on _XTAL_FREQ from System.h */
    SSPADD = (unsigned char)((_XTAL_FREQ / (4 * baud_rate)) - 1);
}

/* =========================================================
   Start Condition
   ========================================================= */
void I2C_Start(void)
{
    I2C_Wait();         /* Wait for bus to be free */
    SSPCON2bits.SEN = 1; /* Initiate Start condition on SDA and SCL pins */
}

/* =========================================================
   Stop Condition
   ========================================================= */
void I2C_Stop(void)
{
    I2C_Wait();         /* Wait for bus to be free */
    SSPCON2bits.PEN = 1; /* Initiate Stop condition on SDA and SCL pins */
}

/* =========================================================
   Repeated Start Condition (Useful for reading)
   ========================================================= */
void I2C_RepeatedStart(void)
{
    I2C_Wait();          /* Wait for bus to be free */
    SSPCON2bits.RSEN = 1; /* Initiate Repeated Start condition */
}

/* =========================================================
   Write Data Byte
   ========================================================= */
void I2C_Write(unsigned char data)
{
    I2C_Wait();             /* Wait for bus to be free */
    SSPBUF = data;          /* Load data into transmit buffer */
    
    /* Wait specifically for the transmission to finish */
    while(!PIR1bits.SSPIF); 
    
    PIR1bits.SSPIF = 0;     /* Clear the interrupt flag manually */
}

/* =========================================================
   Read Data Byte
   ========================================================= */
unsigned char I2C_Read(unsigned char ack)
{
    unsigned char temp;

    I2C_Wait();
    SSPCON2bits.RCEN = 1;   /* Enable Receive mode */
    
    /* Wait for buffer to fill with received data */
    while(!SSPSTATbits.BF);      
    temp = SSPBUF;          /* Read the data */

    I2C_Wait();
    /* Send Acknowledge (0 = ACK, 1 = NACK) */
    SSPCON2bits.ACKDT = (ack) ? 0 : 1; 
    SSPCON2bits.ACKEN = 1;  /* Initiate Acknowledge sequence */

    return temp;
}