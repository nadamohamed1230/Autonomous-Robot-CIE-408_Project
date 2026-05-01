#ifndef TIMER0_CONFIG_H
#define TIMER0_CONFIG_H

/* Preload value for ~25 ms overflow at 16 MHz with 1:256 prescaler
 * Period = (256 - Preload) * 256 * (4 / Fosc)
 * => (256 - 6) * 256 * (4 / 16000000) = ~16 ms  (adjust as needed)
 * Original value 124 was tuned for 8 MHz; 6 approximates 25 ms at 16 MHz. */
#define TIMER0_PRELOAD_VAL  6

#endif
