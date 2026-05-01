#ifndef I2C_CONFIG_H
#define I2C_CONFIG_H

#define I2C_FOSC        16000000UL
#define I2C_CLOCK_FREQ  100000UL
#define I2C_BRG_VALUE   ((I2C_FOSC / (4UL * I2C_CLOCK_FREQ)) - 1U)  /* = 39 */

#endif
