#ifndef I2C_PRIVATE_H
#define I2C_PRIVATE_H

/*
 * I2C_private.h - MPLAB XC8 Compatible
 *
 * SSPCON, SSPCON2, SSPSTAT, SSPBUF, SSPADD, TRISC are provided by <xc.h>.
 */

/* SSPCON bit positions */
#define SSPEN_BIT    5

/* SSPCON2 bit positions */
#define SEN_BIT      0   /* Start condition enable  */
#define RSEN_BIT     1   /* Repeated start enable   */
#define PEN_BIT      2   /* Stop condition enable   */
#define RCEN_BIT     3   /* Receive enable          */
#define ACKEN_BIT    4   /* Acknowledge sequence    */
#define ACKDT_BIT    5   /* Acknowledge data        */
#define ACKSTAT_BIT  6   /* Acknowledge status      */

/* SSPSTAT bit positions */
#define BF_BIT       0   /* Buffer full             */

/* TRISC bit positions for I2C pins */
#define SDA_BIT      4   /* RC4 = SDA               */
#define SCL_BIT      3   /* RC3 = SCL               */

#endif
