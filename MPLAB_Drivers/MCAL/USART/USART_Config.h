#ifndef UART_CONFIG_H
#define UART_CONFIG_H

/* CPU frequency - must match _XTAL_FREQ in System.h */
#define FOSC           16000000UL

/* Desired baud rate */
#define UART_BAUDRATE  9600UL

/* High-speed baud rate mode (1 = BRGH set) */
#define UART_HIGH_SPEED  1

/* SPBRG value for 9600 baud at 16 MHz with BRGH=1:
 * SPBRG = (Fosc / (16 * Baud)) - 1 = (16000000 / (16*9600)) - 1 = 103 */
#define UART_SPBRG_VALUE  103

#endif
