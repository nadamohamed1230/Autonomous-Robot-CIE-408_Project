/*
 * LCD_I2C.c - MPLAB XC8 Compatible
 *
 * No direct register access here. The only change is that Delay_ms()
 * now resolves to __delay_ms() through System.h, which requires a
 * compile-time constant. All existing call-sites already pass constants,
 * so no further edits are needed.
 */

#include "LCD_I2C.h"
#include "../../MCAL/I2C/I2C_interface.h"
#include "../../SERVICES/System.h"
#include "../../MCAL/I2C/I2C_config.h"

static u8 backlight_state = 0x08;   /* Bit 3 high = backlight ON */

/* Send one nibble over I2C and pulse the Enable pin */
static void LCD_I2C_WriteNibble(u8 data_nibble, u8 rs_mode)
{
    u8 i2c_data = data_nibble | backlight_state | rs_mode;

    I2C_Start();
    I2C_Write(LCD_I2C_ADDRESS);

    I2C_Write(i2c_data | 0x04);    /* EN = 1 */
    Delay_ms(1);

    I2C_Write(i2c_data & ~0x04);   /* EN = 0 */
    Delay_ms(1);

    I2C_Stop();
}

void LCD_SendCommand(u8 command)
{
    LCD_I2C_WriteNibble(command & 0xF0,        0x00);   /* Upper nibble, RS=0 */
    LCD_I2C_WriteNibble((command << 4) & 0xF0, 0x00);   /* Lower nibble, RS=0 */
}

void LCD_SendData(u8 char_data)
{
    LCD_I2C_WriteNibble(char_data & 0xF0,        0x01);  /* Upper nibble, RS=1 */
    LCD_I2C_WriteNibble((char_data << 4) & 0xF0, 0x01);  /* Lower nibble, RS=1 */
}

void LCD_Init(void)
{
    /* Initializes the I2C bus at 100kHz */
    I2C_Init(I2C_MASTER_BAUD_RATE);
    Delay_ms(50);                       /* Wait for LCD power-up          */

    /* HD44780 4-bit initialisation sequence */
    LCD_I2C_WriteNibble(0x30, 0);  Delay_ms(5);
    LCD_I2C_WriteNibble(0x30, 0);  Delay_ms(1);
    LCD_I2C_WriteNibble(0x30, 0);
    LCD_I2C_WriteNibble(0x20, 0);  /* Switch to 4-bit mode               */

    LCD_SendCommand(0x28);          /* 4-bit, 2 lines, 5x8 font           */
    LCD_SendCommand(0x0C);          /* Display ON, cursor OFF             */
    LCD_SendCommand(0x06);          /* Auto-increment, no display shift   */
    LCD_Clear();
}

void LCD_SendString(char* str)
{
    while (*str != '\0')
    {
        LCD_SendData((u8)*str++);
    }
}

void LCD_SetCursor(u8 row, u8 col)
{
    u8 address = (row == 0u) ? (u8)(0x80u + col) : (u8)(0xC0u + col);
    LCD_SendCommand(address);
}

void LCD_Clear(void)
{
    LCD_SendCommand(0x01);
    Delay_ms(2);
}
