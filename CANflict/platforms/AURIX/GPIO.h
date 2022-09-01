#pragma once

// The peripheral interface that is being implemented
#include "../peripherals/GPIO.h"

// Platform-specific header
#include <IfxPort.h>
#include <IfxPort_reg.h>

// ------------------- GPIO Interface Implementation ---------------------------

// Implement structs
struct CANflict_GPIO_t {
  Ifx_P *port;
  uint8_t pin;
};

// Implement functions
static inline bool CANflict_GPIO_init(struct CANflict_GPIO_t *gpio,
                                      bool input) {
  if (input) {
    IfxPort_setPinModeInput(gpio->port, gpio->pin, IfxPort_InputMode_pullUp);
    IfxPort_setPinPadDriver(gpio->port, gpio->pin,
                            IfxPort_PadDriver_cmosAutomotiveSpeed2);
  } else {
    IfxPort_setPinHigh(gpio->port, gpio->pin);
    IfxPort_setPinModeOutput(gpio->port, gpio->pin, IfxPort_OutputMode_pushPull,
                             IfxPort_OutputIdx_general);
    IfxPort_setPinPadDriver(gpio->port, gpio->pin,
                            IfxPort_PadDriver_cmosAutomotiveSpeed1);
  }
  return true;
}

static inline bool CANflict_GPIO_deinit(struct CANflict_GPIO_t *gpio) {
  return true;
}

static inline bool CANflict_GPIO_getValue(struct CANflict_GPIO_t *gpio) {
  return IfxPort_getPinState(gpio->port, gpio->pin);
}

static inline bool CANflict_GPIO_setValue(struct CANflict_GPIO_t *gpio,
                                          bool val) {
  IfxPort_setPinState(gpio->port, gpio->pin,
                      (val) ? IfxPort_State_high : IfxPort_State_low);
  return true;
}
