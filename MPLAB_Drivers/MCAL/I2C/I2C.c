/*
 * I2C.c - MPLAB XC8 Compatible
 *
 * Uses XC8's <xc.h> SFR names: SSPCON, SSPCON2, SSPSTAT, SSPBUF, SSPADD, TRISC.
 */

#include <xc.h>
#include "I2C_interface.h"
#include "I2C_config.h"
#include "I2C_private.h"
#include "../../SERVICES/Bit_Math.h"

void I2C_Init(void)
{
    /* SDA (RC4) and SCL (RC3) must be inputs before enabling MSSP */
    TRISC |= (1 << SDA_BIT) | (1 << SCL_BIT);

    /* I2C Master mode (0b1000), SSPEN enabled */
    SSPCON  = (1 << SSPEN_BIT) | 0x08;
    SSPADD  = (u8)I2C_BRG_VALUE;

    /* Standard speed, sample at middle of output */
    SSPSTAT &= ~(1 << 7);
}

void I2C_Start(void)
{
    SET_BIT(SSPCON2, SEN_BIT);
    while (GET_BIT(SSPCON2, SEN_BIT));
}

void I2C_ReStart(void)
{
    SET_BIT(SSPCON2, RSEN_BIT);
    while (GET_BIT(SSPCON2, RSEN_BIT));
}

void I2C_Stop(void)
{
    SET_BIT(SSPCON2, PEN_BIT);
    while (GET_BIT(SSPCON2, PEN_BIT));
}

u8 I2C_WriteByte(u8 tx_byte)
{
    SSPBUF = tx_byte;
    while (GET_BIT(SSPSTAT, BF_BIT));
    /* Wait until no control bits are active */
    while (SSPCON2 & ((1 << SEN_BIT)  | (1 << RSEN_BIT) |
                      (1 << PEN_BIT)  | (1 << RCEN_BIT) |
                      (1 << ACKEN_BIT)));
    return GET_BIT(SSPCON2, ACKSTAT_BIT);   /* 0 = ACK received */
}

u8 I2C_ReadByte(void)
{
    SET_BIT(SSPCON2, RCEN_BIT);
    while (!GET_BIT(SSPSTAT, BF_BIT));
    return SSPBUF;
}

void I2C_SendAck(void)
{
    CLR_BIT(SSPCON2, ACKDT_BIT);
    SET_BIT(SSPCON2, ACKEN_BIT);
    while (GET_BIT(SSPCON2, ACKEN_BIT));
}

void I2C_SendNack(void)
{
    SET_BIT(SSPCON2, ACKDT_BIT);
    SET_BIT(SSPCON2, ACKEN_BIT);
    while (GET_BIT(SSPCON2, ACKEN_BIT));
}
