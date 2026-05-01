#ifndef ADC_CONFIG_H
#define ADC_CONFIG_H

#define ADC_REF_MV              5000U   /* Reference voltage in millivolts */
#define ADC_MAX_VALUE           1023U   /* 10-bit ADC full scale           */
#define ADCON1_CONFIG           0x80    /* Right-justify, Vref = VDD       */
#define ADC_CONVERSION_DELAY_MS 2U      /* Acquisition settle time (ms)    */

#endif
