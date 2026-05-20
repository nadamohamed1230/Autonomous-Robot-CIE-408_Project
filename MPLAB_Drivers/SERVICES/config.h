/*
 * config.h 
 * PIC16F877A Configuration Bits and Clock Speed
 */

#ifndef CONFIG_H
#define CONFIG_H

/* =========================================================
   Hardware Configuration Bits
   ========================================================= */
#pragma config FOSC  = HS       /* High-Speed crystal oscillator (16MHz)  */
#pragma config WDTE  = OFF      /* Watchdog Timer disabled                */
#pragma config PWRTE = ON       /* Power-up Timer enabled (adds stability)*/
#pragma config BOREN = ON       /* Brown-out Reset enabled                */
#pragma config LVP   = OFF      /* Low-Voltage ICSP disabled (CRITICAL!)  */
#pragma config CPD   = OFF      /* Data EEPROM code protection off        */
#pragma config WRT   = OFF      /* Flash write protection off             */
#pragma config CP    = OFF      /* Code protection off                    */

/* =========================================================
   System Clock Definition
   Required by XC8 for __delay_ms() and __delay_us() math.
   ========================================================= */
#define _XTAL_FREQ 20000000     /* 16 MHz */

#endif /* CONFIG_H */