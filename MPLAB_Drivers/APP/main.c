/*
 * main.c - MPLAB XC8 Compatible
 *
 * KEY CHANGES from mikroC version:
 * 1. Added #pragma config fuses (required by XC8; mikroC used a GUI for this).
 * 2. #include <xc.h> replaces mikroC's implicit device header.
 * 3. void interrupt() replaced by void __interrupt() isr(void).
 * 4. Raw INTCON/PORTB register access in ISR replaced by XC8 SFR names.
 * 5. Delay_ms() resolves to __delay_ms() via System.h.
 *
 * IMPORTANT: Verify your crystal frequency matches _XTAL_FREQ in System.h
 * and the FOSC config bit below. Default is 16 MHz HS oscillator.
 */

/* =========================================================
   CONFIGURATION BITS  (replaces mikroC's Edit Project GUI)
   Adjust these to match your hardware. Common settings shown.
   ========================================================= */
#pragma config FOSC  = HS       /* High-Speed crystal oscillator          */
#pragma config WDTE  = OFF      /* Watchdog Timer disabled                */
#pragma config PWRTE = ON       /* Power-up Timer enabled (good practice) */
#pragma config BOREN = ON       /* Brown-out Reset enabled                */
#pragma config LVP   = OFF      /* Low-Voltage ICSP disabled              */
#pragma config CPD   = OFF      /* Data EEPROM code protection off        */
#pragma config WRT   = OFF      /* Flash write protection off             */
#pragma config CP    = OFF      /* Code protection off                    */

#include <xc.h>
#include "../SERVICES/STD_TYPES.h"
#include "../SERVICES/System.h"
#include "../MCAL/GPIO/GPIO_interface.h"
#include "../MCAL/I2C/I2C_interface.h"
#include "../HAL/H_Bridge/H_Bridge.h"
#include "../HAL/Ultrasonic/Ultrasonic.h"
#include "../HAL/SpeedSensor/SpeedSensor.h"
#include "../HAL/LCD/LCD_I2C.h"

/* =========================================================
   Globals
   ========================================================= */
char dist_text[6];
char speed_text[6];

/* =========================================================
   Lightweight integer-to-string converter
   (replaces mikroC's WordToStr() library function)
   ========================================================= */
void IntToString(u16 num, char* str)
{
    u8   i = 0;
    u8   j = 0;
    char temp[6];

    if (num == 0u)
    {
        str[0] = '0';
        str[1] = '\0';
        return;
    }

    while (num > 0u)
    {
        temp[i++] = (char)((num % 10u) + '0');
        num /= 10u;
    }

    while (i > 0u)
    {
        str[j++] = temp[--i];
    }
    str[j] = '\0';
}

/* =========================================================
   Interrupt Service Routine
   XC8 syntax: void __interrupt() isr(void)
   mikroC syntax was: void interrupt() { ... }
   ========================================================= */
void __interrupt() isr(void)
{
    /* INT0 (RB0) fired — speed sensor pulse */
    if (INTCONbits.INTF)
    {
        SpeedSensor_Increment();
        INTCONbits.INTF = 0;    /* Clear the flag */
    }

    /* Add other interrupt sources here (Timer0, USART RX, etc.) as needed */
}

/* =========================================================
   Main
   ========================================================= */
void main(void)
{
    u16 distance = 0;
    u16 pulses   = 0;

    GPIO_Init();
    H_Bridge_Init();
    Ultrasonic_Init();
    SpeedSensor_Init();
    LCD_Init();

    Delay_ms(500);

    LCD_SetCursor(0, 2);
    LCD_SendString("Robot System");
    LCD_SetCursor(1, 4);
    LCD_SendString("Online!");
    Delay_ms(1500);
    LCD_Clear();

    while (1)
    {
        distance = Ultrasonic_GetDistance();
        pulses   = SpeedSensor_GetPulseCount();

        IntToString(distance, dist_text);
        IntToString(pulses,   speed_text);

        LCD_SetCursor(0, 0);
        LCD_SendString("Dist: ");
        LCD_SendString(dist_text);
        LCD_SendString(" cm  ");

        LCD_SetCursor(1, 0);
        LCD_SendString("Spd:  ");
        LCD_SendString(speed_text);
        LCD_SendString(" pls ");

        if (distance == 0u)
        {
            H_Bridge_Stop();
        }
        else if (distance < 20u)
        {
            H_Bridge_Stop();
            Delay_ms(200);
            H_Bridge_SetSpeed(50);
            H_Bridge_Reverse();
            Delay_ms(400);
            H_Bridge_SetSpeed(70);
            H_Bridge_TurnRight();
            Delay_ms(500);
            H_Bridge_Stop();
            Delay_ms(200);
        }
        else
        {
            H_Bridge_SetSpeed(60);
            H_Bridge_Forward();
        }

        Delay_ms(60);
    }
}
