/*
 * TIMER0.c - MPLAB XC8 Compatible
 *
 * Uses XC8's <xc.h> SFR names (TMR0, OPTION_REG, INTCON).
 * OPTION_REG_CUSTOM from the original code is replaced by OPTION_REG.
 */

#include <xc.h>
#include "TIMER0_interface.h"
#include "TIMER0_private.h"
#include "TIMER0_config.h"
#include "../../SERVICES/Bit_Math.h"

static void (*TIMER0_Callback)(void) = 0;

void TIMER0_Init(void)
{
    /* Internal clock source, 1:256 prescaler assigned to Timer0
     * OPTION_REG bits [2:0] = 111 => prescaler 1:256
     * Bit 5 (T0CS) = 0 => internal clock
     * Bit 3 (PSA)  = 0 => prescaler assigned to Timer0           */
    OPTION_REG = 0x87;
    TMR0 = TIMER0_PRELOAD_VAL;
}

void TIMER0_SetPreload(u8 Preload)
{
    TMR0 = Preload;
}

void TIMER0_EnableInterrupt(void)
{
    SET_BIT(INTCON, T0IE_BIT);
    SET_BIT(INTCON, GIE_BIT);
}

void TIMER0_DisableInterrupt(void)
{
    CLR_BIT(INTCON, T0IE_BIT);
}

void TIMER0_SetCallback(void (*ptr)(void))
{
    TIMER0_Callback = ptr;
}

void TIMER0_ISR_Handler(void)
{
    if (GET_BIT(INTCON, T0IF_BIT) == 1)
    {
        TMR0 = TIMER0_PRELOAD_VAL;      /* Reload */
        CLR_BIT(INTCON, T0IF_BIT);      /* Clear flag */

        if (TIMER0_Callback != 0)
        {
            TIMER0_Callback();
        }
    }
}
