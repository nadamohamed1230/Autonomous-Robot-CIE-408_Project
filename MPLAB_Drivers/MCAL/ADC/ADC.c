/*
 * ADC.c - MPLAB XC8 Compatible
 *
 * Uses XC8's <xc.h> SFR names (ADCON0, ADCON1, ADRESH, ADRESL, TRISA).
 * Delay_ms() is the macro from System.h wrapping __delay_ms().
 */

#include <xc.h>
#include "ADC_interface.h"
#include "ADC_config.h"
#include "ADC_private.h"
#include "../../SERVICES/System.h"

void ADC_Init(void)
{
    TRISA   = 0xFF;             /* All PORTA pins as input (analog)   */
    ADCON1  = ADCON1_CONFIG;    /* Right-justify, Vref = VDD          */

    /* ADC ON, Fosc/8 clock, start on channel 0 */
    ADCON0  = 0x81;
    Delay_ms(ADC_CONVERSION_DELAY_MS);
}

u16 ADC_Read(u8 channel)
{
    /* Select channel */
    ADCON0 = (u8)((ADCON0 & ~ADC_CHANNEL_MASK) | (u8)(channel << 3));

    /* Acquisition delay */
    Delay_ms(20);

    /* Start conversion */
    ADCON0 |= (1 << GO_nDONE_BIT);

    /* Wait for completion */
    while (ADCON0 & (1 << GO_nDONE_BIT));

    return ((u16)ADRESH << 8) | ADRESL;
}

u16 ADC_Read_mV(u8 channel)
{
    unsigned long voltage;
    u16 raw = ADC_Read(channel);

    voltage = (unsigned long)raw * ADC_REF_MV;
    voltage = voltage / ADC_MAX_VALUE;

    return (u16)voltage;
}

u16 ADC_ReadTemperatureC(u8 channel)
{
    return ADC_Read_mV(channel) / 10U;
}
