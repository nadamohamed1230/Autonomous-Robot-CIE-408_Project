#ifndef EEPROM_H
#define EEPROM_H

#include "../../SERVICES/STD_TYPES.h"

void EEPROM_WriteByte(u8 address, u8 data);
u8 EEPROM_ReadByte(u8 address);

#endif /* EEPROM_H */