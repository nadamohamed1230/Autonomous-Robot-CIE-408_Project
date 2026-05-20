#include <xc.h>
#include "../USART/USART_interface.h"
#include "../EXT_INT/EXT_INT0.h"
#include "../TIMER0/TIMER0_interface.h"
#include "../../SERVICES/Bit_Math.h"

// void __interrupt() isr(void)
// {
//     /* 1. Let the MCAL driver handle the External Interrupt 0 logic */
//     EXT_INT0_ISR_Handler();

//     /* 2. Check Timer0 */
//     TIMER0_ISR_Handler();

//     /* 3. Check UART RX */
//     if (GET_BIT(PIR1, 5) && GET_BIT(PIE1, 5))
//     {
//         UART_ISR();
//     }
// }