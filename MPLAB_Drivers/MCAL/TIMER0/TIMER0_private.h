#ifndef TIMER0_PRIVATE_H
#define TIMER0_PRIVATE_H

/*
 * TIMER0_private.h - MPLAB XC8 Compatible
 *
 * TMR0, OPTION_REG, and INTCON are all provided by XC8's <xc.h>.
 * The manual volatile-pointer definitions are removed.
 *
 * NOTE: OPTION_REG is a reserved keyword in XC8's device headers.
 *       Any reference to OPTION_REG_CUSTOM in the original code is
 *       replaced by OPTION_REG here.
 */

/* INTCON bit positions */
#define T0IF_BIT   2   /* Timer0 overflow interrupt flag */
#define T0IE_BIT   5   /* Timer0 overflow interrupt enable */
#define GIE_BIT    7   /* Global interrupt enable */

#endif
