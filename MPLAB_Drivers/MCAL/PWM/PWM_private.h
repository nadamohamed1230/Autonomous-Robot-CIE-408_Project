#ifndef PWM_PRIVATE_H
#define PWM_PRIVATE_H

/*
 * PWM_private.h - MPLAB XC8 Compatible
 *
 * All Timer2 and CCP register symbols (T2CON, PR2, CCP1CON, CCPR1L, etc.)
 * are provided by XC8's <xc.h>. The manual address-cast definitions are removed.
 */

/* Timer2 Control Bit positions (used with Bit_Math macros) */
#define T2CKPS0   0   /* Prescaler select bit 0 */
#define T2CKPS1   1   /* Prescaler select bit 1 */
#define TMR2ON    2   /* Timer2 enable bit       */

/* CCP PWM mode value written to CCPxCON */
#define CCP_PWM_MODE  0x0C   /* bits [3:0] = 1100 */

#endif
