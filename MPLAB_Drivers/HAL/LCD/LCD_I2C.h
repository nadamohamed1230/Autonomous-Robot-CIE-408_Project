#ifndef LCD_I2C_H
#define LCD_I2C_H

#include "../../SERVICES/STD_TYPES.h"

/* PCF8574 I2C address (8-bit write address).
 * 0x4E = 0x27 << 1  (most common module)
 * 0x7E = 0x3F << 1  (alternate address - try if 0x4E does not work) */
#define LCD_I2C_ADDRESS  0x4E

void LCD_Init(void);
void LCD_SendCommand(u8 command);
void LCD_SendData(u8 char_data);
void LCD_SendString(char* str);
void LCD_SetCursor(u8 row, u8 col);
void LCD_Clear(void);

#endif
