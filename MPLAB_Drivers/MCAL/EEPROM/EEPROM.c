#include <xc.h>
#include "EEPROM.h"

void EEPROM_WriteByte(u8 address, u8 data)
{
    EEADR = address;    /* Set the address */
    EEDATA = data;      /* Set the data */
    
    EECON1bits.EEPGD = 0; /* Point to Data memory */
    EECON1bits.WREN = 1;  /* Enable writes */
    
    /* CRITICAL SAFETY SEQUENCE - DO NOT MODIFY */
    u8 gie_state = INTCONbits.GIE; /* Save global interrupt state */
    INTCONbits.GIE = 0;            /* Disable interrupts */
    
    EECON2 = 0x55;      /* Write sequence part 1 */
    EECON2 = 0xAA;      /* Write sequence part 2 */
    EECON1bits.WR = 1;  /* Begin Write */
    
    INTCONbits.GIE = gie_state;    /* Restore interrupts */
    
    /* Wait for write operation to finish */
    while (EECON1bits.WR == 1);
    
    EECON1bits.WREN = 0;  /* Disable writes for safety */
}

u8 EEPROM_ReadByte(u8 address)
{
    EEADR = address;      /* Set the address */
    EECON1bits.EEPGD = 0; /* Point to Data memory */
    EECON1bits.RD = 1;    /* Trigger Read */
    
    return EEDATA;        /* Return the data */
}