/*
 * System.c - MPLAB XC8 Compatible
 *
 * In XC8, __delay_ms() and __delay_us() are built-in compiler macros that
 * expand inline. They are defined in System.h via the Delay_ms/Delay_us
 * macros. This file is kept for project structure consistency but no
 * runtime delay function is required here.
 */

#include "System.h"

/* No runtime functions needed - delays are handled by XC8 built-in macros */
