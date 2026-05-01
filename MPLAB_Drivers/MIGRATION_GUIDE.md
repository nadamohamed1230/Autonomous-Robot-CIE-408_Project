# mikroC → MPLAB XC8 Migration Guide
### PIC16F877A Autonomous 4WD Robot Driver Library

---

## What Changed and Why

This document explains every change made when porting from **mikroC PRO for PIC**
to **MPLAB IDE + XC8 Compiler**. Understanding these changes will help you apply
the same fixes to any new files you write.

---

## 1. Device Header: `<xc.h>` replaces mikroC's implicit header

**mikroC** automatically included a device header behind the scenes.  
**XC8** requires you to include it explicitly:

```c
#include <xc.h>   // Add this at the top of every .c file
```

`<xc.h>` gives you all Special Function Register (SFR) names like `TRISA`,
`PORTA`, `INTCON`, `T2CON`, `CCP1CON`, `SSPCON2`, etc. — you do **not**
need to define them yourself.

---

## 2. Register Definitions Removed from `_private.h` Files

**mikroC** allowed you to define registers manually as volatile pointers:

```c
// mikroC style (no longer needed)
#define TRISA  (*(u8*)0x85)
#define PORTA  (*(u8*)0x05)
```

**XC8** already declares all these in `<xc.h>`. Keeping both causes
**redefinition errors**. All manual register address definitions have been
**removed** from `*_private.h` files. The `_private.h` files now only
contain bit-position constants.

---

## 3. Interrupt Service Routine (ISR) Syntax — MOST CRITICAL CHANGE

| IDE      | Syntax |
|----------|--------|
| mikroC   | `void interrupt() { ... }` |
| XC8      | `void __interrupt() isr(void) { ... }` |

Without `__interrupt()`, XC8 will **not** generate a `RETFIE` instruction
and the PIC will crash or hang on every interrupt.

```c
// XC8 — correct
void __interrupt() isr(void)
{
    if (INTCONbits.INTF)
    {
        SpeedSensor_Increment();
        INTCONbits.INTF = 0;
    }
}
```

XC8 also provides **bit-field access** via structs (e.g. `INTCONbits.GIE`,
`INTCONbits.INTF`) which are cleaner than bit-math macros, though both work.

---

## 4. Configuration Bits (`#pragma config`) — Required in XC8

**mikroC** set config fuses through *Edit Project → Configuration Bits* GUI.  
**XC8** requires them as `#pragma config` lines in `main.c`:

```c
#pragma config FOSC  = HS    // High-Speed crystal
#pragma config WDTE  = OFF   // Watchdog off
#pragma config PWRTE = ON    // Power-up timer on
#pragma config BOREN = ON    // Brown-out reset on
#pragma config LVP   = OFF   // Low-voltage programming off
#pragma config CP    = OFF   // Code protection off
```

**If you omit these, default fuse values are used which may prevent the PIC
from running at all** (e.g. wrong oscillator selected).

---

## 5. Delay Functions

| IDE    | Milliseconds       | Microseconds      |
|--------|--------------------|-------------------|
| mikroC | `Delay_ms(x)`      | `Delay_us(x)`     |
| XC8    | `__delay_ms(x)`    | `__delay_us(x)`   |

`System.h` defines compatibility macros so you keep using `Delay_ms()` /
`Delay_us()` without changing any call sites:

```c
#define Delay_ms(ms)  __delay_ms(ms)
#define Delay_us(us)  __delay_us(us)
```

**Important:** XC8's `__delay_ms()` and `__delay_us()` require:
- `_XTAL_FREQ` to be defined (set in `System.h` — default 16 MHz)
- The argument to be a **compile-time constant** (no variable delays)

`System.c`'s runtime `Vdelay_ms()` wrapper is removed — it is not needed.

---

## 6. `OPTION_REG` Naming

**mikroC** let you name it `OPTION_REG_CUSTOM` to avoid conflicts.  
**XC8** defines `OPTION_REG` in `<xc.h>` — use it directly:

```c
// mikroC
OPTION_REG_CUSTOM = 0x87;

// XC8
OPTION_REG = 0x87;
```

---

## 7. `di()` / `ei()` for Atomic Operations

**mikroC** used raw bit manipulation to disable/re-enable global interrupts.  
**XC8** provides `di()` and `ei()` built-in functions that are safer:

```c
di();                  // Disable global interrupts
count = shared_var;    // Safe read of ISR-shared variable
shared_var = 0;
ei();                  // Re-enable global interrupts
```

---

## 8. Include Path Case Sensitivity

The original code had inconsistent casing (`BIT_MATH.h` vs `Bit_Math.h`
vs `bit_math.h`). All files now use `Bit_Math.h` consistently. MPLAB on
Windows is case-insensitive, but if you ever build on Linux/Mac this matters.

---

## How to Set Up a New MPLAB XC8 Project

1. **File → New Project → Standalone Project**
2. Device: **PIC16F877A**
3. Compiler: **XC8**
4. Add all `.c` files from `APP/`, `MCAL/`, `HAL/`, and `SERVICES/`
5. Add all folder paths to **Project Properties → XC8 Compiler → Include directories**:
   - `../SERVICES`
   - `../MCAL/GPIO`
   - `../MCAL/PWM`  *(etc. — or just add the root)*
6. In **Project Properties → XC8 Compiler → Preprocessing and messages**,
   add the global define: `_XTAL_FREQ=16000000`  
   *(or leave it — System.h defines it as a fallback)*
7. Build. Fix any remaining path issues using the error log.

---

## File-by-File Change Summary

| File | Changes |
|------|---------|
| `SERVICES/System.h` | Removed mikroC guards; `Delay_ms`/`Delay_us` now wrap `__delay_ms`/`__delay_us`; added `_XTAL_FREQ` default |
| `SERVICES/System.c` | Removed `Vdelay_ms()` — no longer needed |
| `MCAL/GPIO/GPIO_private.h` | Removed all manual register address definitions |
| `MCAL/GPIO/GPIO.c` | Added `#include <xc.h>` |
| `MCAL/PWM/PWM_private.h` | Removed register address defs; kept bit-position constants |
| `MCAL/PWM/PWM.c` | Added `#include <xc.h>`; fixed 10-bit duty cycle (now writes lower 2 bits to `CCPxCON<5:4>`) |
| `MCAL/TIMER0/TIMER0_private.h` | Removed register defs; `OPTION_REG_CUSTOM` → `OPTION_REG` |
| `MCAL/TIMER0/TIMER0.c` | Added `#include <xc.h>`; `OPTION_REG_CUSTOM` → `OPTION_REG` |
| `MCAL/TIMER0/TIMER0_config.h` | Adjusted preload for 16 MHz (was tuned for 8 MHz) |
| `MCAL/USART/USART_Private.h` | Removed register address defs; kept bit-position constants |
| `MCAL/USART/USART_Config.h` | Added computed `UART_SPBRG_VALUE` for 16 MHz |
| `MCAL/USART/USART.c` | Added `#include <xc.h>`; uses bit-position macros instead of raw names |
| `MCAL/I2C/I2C_private.h` | Removed register address defs |
| `MCAL/I2C/I2C.c` | Added `#include <xc.h>`; `SSPCON_REG` → `SSPCON`, etc. |
| `MCAL/EXT_INT/EXT_INT0_private.h` | Removed register defs; `OPTION_REG_CUSTOM` → `OPTION_REG` |
| `MCAL/EXT_INT/EXT_INT0.c` | Added `#include <xc.h>`; `OPTION_REG_CUSTOM` → `OPTION_REG` |
| `MCAL/ADC/ADC_private.h` | Removed register address defs |
| `MCAL/ADC/ADC.c` | Added `#include <xc.h>`; `ADRESH_REG` → `ADRESH`, etc. |
| `MCAL/INTERRUPT_MANAGER/Interrupt_Manager.c` | `void interrupt()` → `void __interrupt() isr(void)` |
| `HAL/Ultrasonic/Ultrasonic.c` | Removed manual Timer1 register defs; `Delay_us` → macro |
| `HAL/SpeedSensor/SpeedSensor.c` | Raw register addresses → `OPTION_REG`/`INTCON`; `di()`/`ei()` for atomic access |
| `HAL/LCD/LCD_I2C.c` | Fixed `while(*str > 0)` → `while(*str != '\0')` (safer null check) |
| `APP/main.c` | Added `#pragma config` fuses; `void interrupt()` → `void __interrupt() isr(void)`; added `#include <xc.h>` |
| All `HAL/*.c` | No logic changes (they use driver abstractions, not raw registers) |
