#ifndef EXT_INT0_PRIVATE_H
#define EXT_INT0_PRIVATE_H

/*
 * EXT_INT0_private.h - MPLAB XC8 Compatible
 *
 * INTCON and OPTION_REG are provided by XC8's <xc.h>.
 */

/* INTCON bit positions */
#define INTF_BIT    1   /* INT0 interrupt flag  */
#define INTE_BIT    4   /* INT0 interrupt enable */
#define GIE_BIT     7   /* Global interrupt enable */

/* OPTION_REG bit positions */
#define INTEDG_BIT  6   /* Interrupt edge select (1=rising, 0=falling) */

#endif
