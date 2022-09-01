#pragma once

#include "config.h" // Configuration header for this platform

// Peripherals includes
#include "ADC.h"
#include "GPIO.h"
#include "I2C.h"
#include "SPI.h"
#include "UART.h"

/// \note To use the timer interrupt, the user must set the CANFLICT_LPC11_TIMER
/// define to one of the values defined in config.h.
#ifdef CANFLICT_LPC11_TIMER
#if CANFLICT_LPC11_TIMER == CANFLICT_LPC11_TIMER16_0 ||                        \
    CANFLICT_LPC11_TIMER == CANFLICT_LPC11_TIMER16_1 ||                        \
    CANFLICT_LPC11_TIMER == CANFLICT_LPC11_TIMER32_0 ||                        \
    CANFLICT_LPC11_TIMER == CANFLICT_LPC11_TIMER32_1
#include "TIM.h"
#else
#error                                                                         \
    "CANflict: Wrong value for CANFLICT_LPC11_TIMER: see the target's config.h for details on the possible values"
#endif
#else
#warning                                                                       \
    "CANflict: the CANFLICT_LPC11_TIMER macro is not set, skipping timer header inclusion for the current target"
#endif
