/*
 * USART.c - MPLAB XC8 Compatible
 *
 * Uses XC8's <xc.h> SFR names (TXSTA, RCSTA, SPBRG, TXREG, RCREG,
 * PIR1, PIE1, INTCON). Bit-field names are replaced with bit-position
 * macros and the Bit_Math helpers to avoid XC8 bit-field naming conflicts.
 */

#include <xc.h>
#include "USART_interface.h"
#include "USART_Private.h"
#include "USART_Config.h"
#include "../../SERVICES/STD_TYPES.h"
#include "../../SERVICES/Bit_Math.h"

static void (*UART_Callback)(u8) = 0;

void UART_RX_Init(void)
{
    SET_BIT(TXSTA, BRGH_BIT);      /* High-speed baud rate */
    SPBRG = UART_SPBRG_VALUE;      /* Baud rate register   */
    CLR_BIT(TXSTA, SYNC_BIT);      /* Asynchronous mode    */
    SET_BIT(RCSTA, SPEN_BIT);      /* Enable serial port   */
    SET_BIT(RCSTA, CREN_BIT);      /* Continuous receive   */
    SET_BIT(PIE1,  RCIE_BIT);      /* Enable RX interrupt  */
    SET_BIT(INTCON, PEIE_BIT);     /* Peripheral int enable */
    SET_BIT(INTCON, GIE_BIT);      /* Global int enable    */
}

void UART_TX_Init(void)
{
    SET_BIT(TXSTA, BRGH_BIT);      /* High-speed baud rate */
    SPBRG = UART_SPBRG_VALUE;      /* Baud rate register   */
    CLR_BIT(TXSTA, SYNC_BIT);      /* Asynchronous mode    */
    SET_BIT(RCSTA, SPEN_BIT);      /* Enable serial port   */
    SET_BIT(TXSTA, TXEN_BIT);      /* Enable transmitter   */
}

void UART_Write(u8 Data)
{
    while (!GET_BIT(TXSTA, TRMT_BIT));  /* Wait until TX shift register empty */
    TXREG = Data;
}

u8 UART_Read(void)
{
    while (!GET_BIT(PIR1, RCIF_BIT));   /* Wait for received byte */
    return RCREG;
}

u8 UART_TX_Empty(void)
{
    return GET_BIT(TXSTA, TRMT_BIT);
}

void UART_SetCallback(void (*Callback)(u8))
{
    if (Callback != 0)
    {
        UART_Callback = Callback;
    }
}

void UART_ISR(void)
{
    u8 received = RCREG;
    if (UART_Callback != 0)
    {
        UART_Callback(received);
    }
}
