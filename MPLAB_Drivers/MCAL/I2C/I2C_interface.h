#ifndef I2C_INTERFACE_H
#define I2C_INTERFACE_H

#include "../../SERVICES/STD_TYPES.h"

void I2C_Init(void);
void I2C_Start(void);
void I2C_ReStart(void);
void I2C_Stop(void);
u8   I2C_WriteByte(u8 tx_byte);
u8   I2C_ReadByte(void);
void I2C_SendAck(void);
void I2C_SendNack(void);

#endif
