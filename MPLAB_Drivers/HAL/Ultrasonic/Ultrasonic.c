/*
 * Ultrasonic.c — Non-Blocking Interrupt-Driven Driver
 * Sensors: Front (RB5), Back (RB6), Right (RB7)
 * Triggers: Front (RA0), Back (RA1), Right (RA2)
 */

#include <xc.h>
#include "Ultrasonic.h"
#include "Ultrasonic_config.h"
#include "../../MCAL/GPIO/GPIO_interface.h"
#include "../../SERVICES/System.h"

/* ── Live distance variables ─────────────────────────────────────────────── */
volatile u16 Dist_Front = 0;
volatile u16 Dist_Back  = 0;
volatile u16 Dist_Right = 0;

/* ── Internal state ──────────────────────────────────────────────────────── */
static volatile u16 t_start_F           = 0;
static volatile u16 t_start_B           = 0;
static volatile u16 t_start_R           = 0;
static volatile u8  last_portb_state    = 0;
static volatile u8  current_sensor_turn = 0;  /* 0=Front, 1=Back, 2=Right */

/* ============================================================================
   INIT
   ============================================================================ */
void Ultrasonic_Init(void)
{
    /* Trigger pins as output, pulled LOW */
    GPIO_SetPinDirection(TRIG_PORT, TRIG_FRONT_PIN, GPIO_OUTPUT);
    GPIO_SetPinDirection(TRIG_PORT, TRIG_BACK_PIN,  GPIO_OUTPUT);
    GPIO_SetPinDirection(TRIG_PORT, TRIG_RIGHT_PIN, GPIO_OUTPUT);

    GPIO_SetPinValue(TRIG_PORT, TRIG_FRONT_PIN, GPIO_LOW);
    GPIO_SetPinValue(TRIG_PORT, TRIG_BACK_PIN,  GPIO_LOW);
    GPIO_SetPinValue(TRIG_PORT, TRIG_RIGHT_PIN, GPIO_LOW);

    /* Echo pins as input */
    GPIO_SetPinDirection(ECHO_PORT, ECHO_FRONT_PIN, GPIO_INPUT);
    GPIO_SetPinDirection(ECHO_PORT, ECHO_BACK_PIN,  GPIO_INPUT);
    GPIO_SetPinDirection(ECHO_PORT, ECHO_RIGHT_PIN, GPIO_INPUT);

    /* Timer1: 1 tick = 1 µs @ 16 MHz, prescaler 1:4 */
    TMR1H = 0;
    TMR1L = 0;
    T1CON = 0x21;

    /* Enable PORTB interrupt-on-change */
    last_portb_state  = PORTB;
    INTCONbits.RBIE   = 1;
    INTCONbits.RBIF   = 0;
}

/* ============================================================================
   ROUND-ROBIN TRIGGER
   Fires ONE sensor per call: Front → Back → Right → Front …
   Called every 96 ms from the telemetry block in main.c.
   Each sensor refreshes every 288 ms — no acoustic collisions.
   ============================================================================ */
void Ultrasonic_Trigger_RoundRobin(void)
{
    if (current_sensor_turn == 0)
    {
        GPIO_SetPinValue(TRIG_PORT, TRIG_FRONT_PIN, GPIO_HIGH);
        Delay_us(10);
        GPIO_SetPinValue(TRIG_PORT, TRIG_FRONT_PIN, GPIO_LOW);
        current_sensor_turn = 1;
    }
    else if (current_sensor_turn == 1)
    {
        GPIO_SetPinValue(TRIG_PORT, TRIG_BACK_PIN, GPIO_HIGH);
        Delay_us(10);
        GPIO_SetPinValue(TRIG_PORT, TRIG_BACK_PIN,  GPIO_LOW);
        current_sensor_turn = 2;
    }
    else
    {
        GPIO_SetPinValue(TRIG_PORT, TRIG_RIGHT_PIN, GPIO_HIGH);
        Delay_us(10);
        GPIO_SetPinValue(TRIG_PORT, TRIG_RIGHT_PIN, GPIO_LOW);
        current_sensor_turn = 0;
    }
}

/* ============================================================================
   IOC ISR
   Accepts the PORTB snapshot already read in main.c ISR.
   No second PORTB read — mismatch latch stays intact for the encoder.
   u8 cast removed — Dist_* are u16 so distances > 255 cm work correctly.
   RBIF cleared here (inside the driver) for safety and encapsulation.
   ============================================================================ */
void Ultrasonic_IOC_ISR(u8 current_portb)
{
    u16 current_time = ((u16)TMR1H << 8) | TMR1L;
    u8  changed_pins = current_portb ^ last_portb_state;

    /* Front — RB5 */
    if (changed_pins & (1U << 5))
    {
        if (current_portb & (1U << 5))
            t_start_F = current_time;
        else
            Dist_Front = (current_time - t_start_F) / 58U;
    }

    /* Back — RB6 */
    if (changed_pins & (1U << 6))
    {
        if (current_portb & (1U << 6))
            t_start_B = current_time;
        else
            Dist_Back = (current_time - t_start_B) / 58U;
    }

    /* Right — RB7 */
    if (changed_pins & (1U << 7))
    {
        if (current_portb & (1U << 7))
            t_start_R = current_time;
        else
            Dist_Right = (current_time - t_start_R) / 58U;
    }

    last_portb_state  = current_portb;
    INTCONbits.RBIF   = 0;   /* Clear inside driver — safe even if called outside ISR */
}

/* ============================================================================
   GETTERS
   ============================================================================ */
u16 Ultrasonic_GetDistance(void)     { return Dist_Front; }
u16 Ultrasonic_GetBackDistance(void) { return Dist_Back;  }
u16 Ultrasonic_GetRightDistance(void){ return Dist_Right; }