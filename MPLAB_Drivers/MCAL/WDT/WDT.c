#include <xc.h>
#include "WDT.h"

void WDT_Init(void)
{
    /* On PIC16F877A, WDT is enabled via config fuses.
       Here, we can assign the prescaler to the WDT using the OPTION_REG.
       PSA=1 assigns prescaler to WDT. PS2:PS0 = 111 gives 1:128 rate (~2.3 seconds) */
    OPTION_REG |= 0x0F; 
}

void WDT_Refresh(void)
{
    /* Assembly instruction to clear the Watchdog Timer */
    CLRWDT(); 
}