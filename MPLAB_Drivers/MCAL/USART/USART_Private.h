#ifndef UART_PRIVATE_H
#define UART_PRIVATE_H

/*
 * USART_Private.h - MPLAB XC8 Compatible
 *
 * All USART register and bit symbols are provided by XC8's <xc.h>.
 * Manual volatile-pointer definitions are removed.
 */

/* ---- TXSTA bit positions ---- */
#define TXEN_BIT   5   /* Transmit enable        */
#define BRGH_BIT   2   /* High-speed baud rate   */
#define SYNC_BIT   4   /* Sync / Async mode      */
#define TRMT_BIT   1   /* Transmit shift reg empty */

/* ---- RCSTA bit positions ---- */
#define SPEN_BIT   7   /* Serial port enable     */
#define CREN_BIT   4   /* Continuous receive     */

/* ---- PIR1 bit positions ---- */
#define RCIF_BIT   5   /* USART receive interrupt flag */

/* ---- PIE1 bit positions ---- */
#define RCIE_BIT   5   /* USART receive interrupt enable */

/* ---- INTCON bit positions ---- */
#define PEIE_BIT   6   /* Peripheral interrupt enable */
#define GIE_BIT    7   /* Global interrupt enable     */

#endif
