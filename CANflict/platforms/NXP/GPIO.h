#pragma once

// The peripheral interface that is being implemented
#include "../peripherals/GPIO.h"

// Platform-specific header
#include <board.h>

// ------------------- GPIO Interface Implementation ---------------------------

// Implement structs
struct CANflict_GPIO_t {
  LPC_GPIO_T *gpioPeriph;
  uint8_t port;
  uint8_t pin;
};

// Implement functions
static inline bool CANflict_GPIO_init(struct CANflict_GPIO_t *gpio,
                                      bool input) {
  if (input)
    Chip_GPIO_SetPinDIRInput(gpio->gpioPeriph, gpio->port, gpio->pin);
  else
    Chip_GPIO_SetPinDIROutput(gpio->gpioPeriph, gpio->port, gpio->pin);
  return true;
}

static inline bool CANflict_GPIO_deinit(struct CANflict_GPIO_t *gpio) {
  return true;
}

static inline bool CANflict_GPIO_getValue(struct CANflict_GPIO_t *gpio) {
  return Chip_GPIO_GetPinState(gpio->gpioPeriph, gpio->port, gpio->pin);
}

static inline bool CANflict_GPIO_setValue(struct CANflict_GPIO_t *gpio,
                                          bool val) {
  Chip_GPIO_SetPinState(gpio->gpioPeriph, gpio->port, gpio->pin, val);
  return true;
}
