#ifndef ADC_PRIVATE_H
#define ADC_PRIVATE_H

/*
 * ADC_private.h - MPLAB XC8 Compatible
 *
 * ADCON0, ADCON1, ADRESH, ADRESL, TRISA are provided by XC8's <xc.h>.
 */

/* ADCON0 bit positions */
#define ADON_BIT       0   /* ADC on/off */
#define GO_nDONE_BIT   2   /* Start conversion / busy flag */

/* Channel selection mask in ADCON0 bits [5:3] */
#define ADC_CHANNEL_MASK  0x38

#endif
