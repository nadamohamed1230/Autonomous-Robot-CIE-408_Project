#ifndef ADC_INTERFACE_H
#define ADC_INTERFACE_H

#include "../../SERVICES/STD_TYPES.h"

void ADC_Init(void);
u16  ADC_Read(u8 channel);
u16  ADC_Read_mV(u8 channel);
u16  ADC_ReadTemperatureC(u8 channel);

#endif
