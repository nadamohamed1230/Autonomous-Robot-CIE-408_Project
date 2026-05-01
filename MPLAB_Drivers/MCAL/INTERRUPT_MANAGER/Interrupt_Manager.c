/*
 * Interrupt_Manager.c - MPLAB XC8 Compatible
 *
 * KEY CHANGE: mikroC uses    void interrupt() { ... }
 *             XC8 requires   void __interrupt() isr(void) { ... }
 *
 * The __interrupt() attribute tells XC8 this is the ISR and generates the
 * correct RETFIE instruction. Without it the PIC will crash on an interrupt.
 */

#include <xc.h>
#include "../USART/USART_interface.h"
#include "../EXT_INT/EXT_INT0.h"
#include "../TIMER0/TIMER0_interface.h"
#include "../../SERVICES/Bit_Math.h"

void __interrupt() isr(void)
{
    /* Each handler checks its own flag internally */
    EXT_INT0_ISR_Handler();
    TIMER0_ISR_Handler();

    /* If UART RX interrupt is enabled, check PIR1.RCIF (bit 5) */
    if (GET_BIT(PIR1, 5) && GET_BIT(PIE1, 5))
    {
        UART_ISR();
    }
}
