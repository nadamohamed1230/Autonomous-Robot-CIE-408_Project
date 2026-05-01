/*
 * EXT_INT0.c - MPLAB XC8 Compatible
 *
 * Uses XC8's <xc.h> SFR names (INTCON, OPTION_REG).
 * OPTION_REG_CUSTOM is replaced by OPTION_REG.
 */

#include <xc.h>
#include "EXT_INT0.h"
#include "EXT_INT0_private.h"
#include "../GPIO/GPIO_interface.h"
#include "../../SERVICES/Bit_Math.h"

static void (*INT0_Callback)(void) = 0;

void EXT_INT0_SetEdge(u8 edge_type);

void EXT_INT0_Init(void)
{
    GPIO_SetPinDirection(GPIO_PORTB, GPIO_PIN0, GPIO_INPUT);
    CLR_BIT(INTCON, INTF_BIT);          /* Clear any stale flag        */
    CLR_BIT(OPTION_REG, 7);             /* Enable PORTB weak pull-ups  */
    EXT_INT0_SetEdge(EXT_INT0_DEFAULT_EDGE);
}

void EXT_INT0_Enable(void)
{
    CLR_BIT(INTCON, INTF_BIT);          /* Clear flag before enabling  */
    SET_BIT(INTCON, INTE_BIT);          /* Enable INT0                 */
    SET_BIT(INTCON, GIE_BIT);           /* Enable global interrupts    */
}

void EXT_INT0_Disable(void)
{
    CLR_BIT(INTCON, INTE_BIT);
}

void EXT_INT0_SetEdge(u8 edge_type)
{
    if (edge_type == INT0_RISING_EDGE)
        SET_BIT(OPTION_REG, INTEDG_BIT);
    else
        CLR_BIT(OPTION_REG, INTEDG_BIT);
}

void EXT_INT0_SetCallback(void (*ptr)(void))
{
    INT0_Callback = ptr;
}

void EXT_INT0_ISR_Handler(void)
{
    if (GET_BIT(INTCON, INTF_BIT) == 1)
    {
        CLR_BIT(INTCON, INTF_BIT);      /* Clear flag immediately */
        if (INT0_Callback != 0)
        {
            INT0_Callback();
        }
    }
}
