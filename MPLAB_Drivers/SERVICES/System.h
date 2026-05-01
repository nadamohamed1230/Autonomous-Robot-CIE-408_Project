#ifndef SYSTEM_H
#define SYSTEM_H

/*
 * System.h - MPLAB XC8 Compatible
 *
 * _XTAL_FREQ must be defined before including this header (or in the project
 * preprocessor symbols). It is used by XC8's built-in __delay_ms() and
 * __delay_us() functions.
 *
 * Example: #define _XTAL_FREQ 16000000UL   (for 16 MHz crystal)
 */

#ifndef _XTAL_FREQ
    #define _XTAL_FREQ 16000000UL   /* Default: 16 MHz - change to match your hardware */
#endif

#include <xc.h>

/* Delay macros - XC8 requires a compile-time constant argument */
#define Delay_ms(ms)   __delay_ms(ms)
#define Delay_us(us)   __delay_us(us)

/* Software reset - jump to reset vector */
#define SYSTEM_RESET() asm("goto 0")

#endif
