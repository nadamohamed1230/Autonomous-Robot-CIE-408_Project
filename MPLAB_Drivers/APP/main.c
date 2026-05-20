/*
 * =============================================================================
 * main.c  —  PIC16F877A Final Implementation
 * =============================================================================
 *
 * ALL FIXES APPLIED:
 *  1.  Stack Depth     : Max depth 6 (safe within 8-level hardware limit).
 *  2.  UART Overrun    : OERR hardware lockout cleared instantly in ISR.
 *  3.  Race Conditions : PORTB single-read snapshot passed to ultrasonic ISR.
 *  4.  Acoustics       : Round-Robin trigger — one sensor per 96 ms cycle.
 *  5.  Dead Man Switch : 800 ms timeout — safe margin over 500 ms heartbeat.
 *  6.  Telemetry Race  : Ultrasonic getters called once per value.
 *  7.  RBIF            : Cleared inside Ultrasonic_IOC_ISR.
 *  8.  PWM / No Motion : Timer2 started inside PWM_Init (PWM.c fix).
 *  9.  Motor Config    : H_Bridge_config.h fixed — IN1/IN2=Left, IN3/IN4=Right.
 * 10.  Front Sensor    : Readings < 3 cm filtered as 0 (floating pin noise).
 * 11.  Speed Flicker   : Apply_Command only called when cmd or speed changes.
 * 12.  AEB Resume      : active_cmd not overwritten — Pi heartbeat resumes.
 *                        Ring buffer drained during override.
 * 13.  Stall Alert     : CMD_BEEP (0x05) — Pi sends this when stall detected.
 *                        PIC plays double beep to alert physically.
 *
 * UART PROTOCOL  (Pi → PIC)  2-byte packet: [CMD][SPEED 0-100]
 *   0x00 STOP | 0x01 FORWARD | 0x02 REVERSE | 0x03 LEFT | 0x04 RIGHT
 *   0x05 BEEP  (stall alert — Pi sends when car appears stuck)
 *
 * TELEMETRY  (PIC → Pi)  9-byte frame every 96 ms:
 *   [0xAA][Front][Back][Right][LRPM_H][LRPM_L][RRPM_H][RRPM_L][0x55]
 *
 * SAFETY FEATURES
 *   1. AEB        : Front >= 3 cm and < 15 cm while forward → stop + buzzer
 *   2. Dead Man   : No UART in 800 ms → stop + slow buzzer blink
 *   3. Traction   : RPM tracking via TC_Module
 *   4. Stall Alert: Pi detects stuck car → CMD_BEEP → PIC double-beeps
 * =============================================================================
 */

#include "../SERVICES/config.h"
#include "../SERVICES/System.h"
#include "../SERVICES/STD_TYPES.h"
#include "../SERVICES/Bit_Math.h"
#include "../SERVICES/Ring_Buffer/Ring_Buffer.h"
#include "../SERVICES/TC_Module/TC_Module.h"
#include "../MCAL/USART/USART_interface.h"
#include "../MCAL/GPIO/GPIO_interface.h"
#include "../HAL/H_Bridge/H_Bridge.h"
#include "../HAL/Ultrasonic/Ultrasonic.h"
#include "../HAL/Encoder/Encoder.h"
#include "../HAL/Buzzer/Buzzer.h"

/* ============================================================================
   CONSTANTS
   ============================================================================ */
#define AEB_FRONT_CM             15U
#define SENSOR_MIN_CM             3U
#define HEARTBEAT_TIMEOUT_TICKS  50U
#define TELEM_PERIOD_TICKS        6U
#define BUZZER_BLINK_TICKS       20U
#define TELEM_HEADER             0xAAU
#define TELEM_FOOTER             0x55U

/* CMD_BEEP: sent by Pi when stall is detected (car physically stuck).
   Must be > CMD_RIGHT so existing command range stays intact.          */
#define CMD_BEEP                 0x05U

/* ============================================================================
   STATE
   ============================================================================ */
static volatile u8 rx_stage        = 0U;
static volatile u8 pending_cmd     = 0x00U;
static volatile u8 pending_speed   = 0U;
static volatile u8 heartbeat_ticks = 0U;
static volatile u8 connection_lost = 0U;
static volatile u8 telem_tick      = 0U;
static volatile u8 buzzer_tick     = 0U;
static volatile u8 flag_telem      = 0U;
static volatile u8 flag_rpm        = 0U;
static volatile u8 last_rb4        = 0U;
static          u8 active_cmd      = CMD_STOP;
static          u8 active_speed    = 0U;

/* ============================================================================
   TIMER0
   ============================================================================ */
static void Timer0_Init(void)
{
    CLR_BIT(OPTION_REG, 5);
    CLR_BIT(OPTION_REG, 3);
    SET_BIT(OPTION_REG, 0);
    SET_BIT(OPTION_REG, 1);
    SET_BIT(OPTION_REG, 2);
    TMR0 = 6U;
    CLR_BIT(INTCON, 2);
    SET_BIT(INTCON, 5);
}

/* ============================================================================
   APPLY COMMAND
   Handles all 6 commands including CMD_BEEP.
   CMD_BEEP plays a double beep (blocking ~400 ms) — car is already stopped
   when Pi sends this so the brief block is harmless.
   ============================================================================ */
static void Apply_Command(u8 cmd, u8 speed)
{
    switch (cmd)
    {
        case CMD_FORWARD:
            H_Bridge_Forward(speed);
            break;

        case CMD_REVERSE:
            H_Bridge_Reverse(speed);
            break;

        case CMD_LEFT:
            H_Bridge_TurnLeft(speed);
            break;

        case CMD_RIGHT:
            H_Bridge_TurnRight(speed);
            break;

        case CMD_STOP:
            H_Bridge_Stop();
            break;

        case CMD_BEEP:
            /* Double beep — stall alert from Pi.
             * Car is stopped before Pi sends this command.             */
            Buzzer_On();  Delay_ms(200U);
            Buzzer_Off(); Delay_ms(100U);
            Buzzer_On();  Delay_ms(200U);
            Buzzer_Off();
            break;

        default:
            H_Bridge_Stop();
            break;
    }
}

/* ============================================================================
   SENSOR FILTER
   ============================================================================ */
static u8 Filter_Sensor(u16 raw)
{
    if (raw < SENSOR_MIN_CM) return 0U;
    if (raw > 255U)          return 255U;
    return (u8)raw;
}

/* ============================================================================
   TELEMETRY
   ============================================================================ */
static void Send_Telemetry(void)
{
    u16 lrpm = TC_GetLeftRPM();
    u16 rrpm = TC_GetRightRPM();

    u8 f = Filter_Sensor(Ultrasonic_GetDistance());
    u8 b = Filter_Sensor(Ultrasonic_GetBackDistance());
    u8 r = Filter_Sensor(Ultrasonic_GetRightDistance());

    UART_Write(TELEM_HEADER);
    UART_Write(f);
    UART_Write(b);
    UART_Write(r);
    UART_Write((u8)(lrpm >> 8));
    UART_Write((u8)(lrpm & 0xFFU));
    UART_Write((u8)(rrpm >> 8));
    UART_Write((u8)(rrpm & 0xFFU));
    UART_Write(TELEM_FOOTER);
}

/* ============================================================================
   SAFETY CHECK
   ============================================================================ */
static u8 Safety_Check(void)
{
    if (connection_lost)
    {
        H_Bridge_Stop();
        active_cmd   = CMD_STOP;
        active_speed = 0U;
        return 1U;
    }

    if (active_cmd == CMD_FORWARD)
    {
        u16 front = Ultrasonic_GetDistance();
        if ((front >= SENSOR_MIN_CM) && (front < AEB_FRONT_CM))
        {
            H_Bridge_Stop();
            Buzzer_On();
            
            /* --- THE FIX --- */
            active_cmd   = CMD_STOP;   // Tell the PIC it is safely stopped!
            active_speed = 0U;         // Reset the speed memory to 0
            /* --------------- */
            
            return 1U;
        }
    }

    Buzzer_Off();
    return 0U;
}

/* ============================================================================
   RING BUFFER DRAIN
   ============================================================================ */
static void Drain_RingBuffer(void)
{
    u8 dummy = 0U;
    while (RingBuffer_IsAvailable() >= 2U)
    {
        RingBuffer_Read(&dummy);
        RingBuffer_Read(&dummy);
    }
}

/* ============================================================================
   MAIN
   ============================================================================ */
void main(void)
{
    ADCON1 = 0x06U;

    GPIO_Init();
    Buzzer_Init();
    H_Bridge_Init();
    Ultrasonic_Init();
    Encoder_Init();
    RingBuffer_Init();
    TC_Init();
    Timer0_Init();

    UART_TX_Init();
    UART_RX_Init();

    SET_BIT(INTCON, 4);
    CLR_BIT(INTCON, 1);
    SET_BIT(INTCON, 6);
    SET_BIT(INTCON, 7);

    /* Ready: 2 short beeps */
    Buzzer_On();  Delay_ms(150U);
    Buzzer_Off(); Delay_ms(150U);
    Buzzer_On();  Delay_ms(150U);
    Buzzer_Off();

    while (1)
    {
        u8 override = Safety_Check();

        if (override)
        {
            Drain_RingBuffer();
        }
        else
        {
            if (RingBuffer_IsAvailable() >= 2U)
            {
                u8 cmd   = 0U;
                u8 speed = 0U;
                RingBuffer_Read(&cmd);
                RingBuffer_Read(&speed);

                /* CMD_BEEP always executes — it is not a motion command
                 * so active_cmd/speed optimisation does not apply.     */
                if (cmd == CMD_BEEP)
                {
                    Apply_Command(CMD_BEEP, 0U);
                }
                else if (cmd != active_cmd || speed != active_speed)
                {
                    active_cmd   = cmd;
                    active_speed = speed;
                    Apply_Command(cmd, speed);
                }
                else
                {
                    active_cmd = cmd;
                }
            }
        }

        if (flag_rpm)
        {
            flag_rpm = 0U;
            TC_UpdateRPM();
        }

        if (flag_telem)
        {
            flag_telem = 0U;
            Send_Telemetry();
            Ultrasonic_Trigger_RoundRobin();
        }
    }
}

/* ============================================================================
   ISR
   ============================================================================ */
void __interrupt() ISR(void)
{
    if (RCSTAbits.OERR)
    {
        RCSTAbits.CREN = 0;
        RCSTAbits.CREN = 1;
    }

    if (GET_BIT(PIR1, 5) && GET_BIT(PIE1, 5))
    {
        u8 byte = RCREG;
        heartbeat_ticks = 0U;

        if (connection_lost)
        {
            RingBuffer_Init();
            connection_lost = 0U;
            rx_stage        = 0U;
        }

        if (rx_stage == 0U)
        {
            /* Accept CMD_STOP through CMD_BEEP (0x00 – 0x05) */
            if (byte <= CMD_BEEP)
            {
                pending_cmd = byte;
                rx_stage    = 1U;
            }
        }
        else
        {
            pending_speed = (byte > 100U) ? 100U : byte;
            RingBuffer_Write(pending_cmd);
            RingBuffer_Write(pending_speed);
            rx_stage = 0U;
        }
    }

    if (GET_BIT(INTCON, 2) && GET_BIT(INTCON, 5))
    {
        TMR0 = 6U;
        CLR_BIT(INTCON, 2);

        TractionControl_Tick();
        if (TC_IsRPMWindowReady()) flag_rpm = 1U;

        if (++telem_tick >= TELEM_PERIOD_TICKS)
        {
            telem_tick = 0U;
            flag_telem = 1U;
        }

        if (heartbeat_ticks < HEARTBEAT_TIMEOUT_TICKS)
        {
            heartbeat_ticks++;
        }
        else if (!connection_lost)
        {
            connection_lost = 1U;
            rx_stage        = 0U;
        }

        if (connection_lost)
        {
            if (++buzzer_tick >= BUZZER_BLINK_TICKS)
            {
                buzzer_tick = 0U;
                Buzzer_Toggle();
            }
        }
        else
        {
            buzzer_tick = 0U;
        }
    }

    if (GET_BIT(INTCON, 1) && GET_BIT(INTCON, 4))
    {
        Encoder_Left_Count++;
        CLR_BIT(INTCON, 1);
    }

    if (GET_BIT(INTCON, 0) && GET_BIT(INTCON, 3))
    {
        u8 portb_val = PORTB;
        u8 rb4_now   = GET_BIT(portb_val, 4);
        if (rb4_now && !last_rb4) Encoder_Right_Count++;
        last_rb4 = rb4_now;
        Ultrasonic_IOC_ISR(portb_val);
    }
}