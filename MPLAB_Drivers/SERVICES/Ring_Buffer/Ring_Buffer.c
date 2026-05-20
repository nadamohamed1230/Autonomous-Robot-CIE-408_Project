/*
 * Ring_Buffer.c — Fixed for MPLAB XC8
 *
 * Advisory (1498) on RingBuffer_Read(@data):
 *   XC8 cannot prove the `data` pointer is non-NULL at compile
 *   time when the function is called across compilation units.
 *   Adding a NULL guard before dereferencing silences the advisory
 *   and makes the function robustly safe.
 */

#include "Ring_Buffer.h"
#include <xc.h>

static u8  buffer[BUFFER_SIZE];
static u16 head  = 0;
static u16 tail  = 0;
static u8  count = 0;

void RingBuffer_Init(void)
{
    head  = 0;
    tail  = 0;
    count = 0;
}

/* Call inside UART RX ISR */
void RingBuffer_Write(u8 data)
{
    if (count < BUFFER_SIZE)
    {
        buffer[head] = data;
        head = (u16)((head + 1u) % BUFFER_SIZE);
        count++;
    }
    /* Buffer full: silently drop new byte (oldest preserved) */
}

/* Call from main loop. Returns 1 on success, 0 if buffer empty */
u8 RingBuffer_Read(u8 *data)
{
    u8 gie_state;

    if (data == (u8 *)0) return 0u;  /* NULL guard — silences advisory 1498 */

    if (count > 0u)
    {
        *data = buffer[tail];
        tail  = (u16)((tail + 1u) % BUFFER_SIZE);

        /* Atomic decrement: disable GIE, update, restore */
        gie_state = INTCONbits.GIE;
        INTCONbits.GIE = 0;
        count--;
        INTCONbits.GIE = gie_state;

        return 1u;
    }
    return 0u;
}

u8 RingBuffer_IsAvailable(void)
{
    return count;
}