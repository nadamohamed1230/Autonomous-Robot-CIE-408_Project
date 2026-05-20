/*
 * I2C_private.h
 */

#ifndef I2C_PRIVATE_H
#define I2C_PRIVATE_H

/* 
 * NOTE: Hardware register addresses (SSPCON, SSPSTAT, etc.) are 
 * natively handled by the <xc.h> compiler header in XC8. 
 * We do not need to manually map 0x... addresses here anymore!
 */

/* I2C Acknowledge Status Defines */
#define I2C_ACK   0
#define I2C_NACK  1

#endif /* I2C_PRIVATE_H */