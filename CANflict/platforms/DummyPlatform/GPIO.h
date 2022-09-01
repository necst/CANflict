#pragma once

// The peripheral interface that is being implemented
#include "../peripherals/GPIO.h"

// Platform-specific code
#include <stdio.h>

struct CANflict_GPIO_t {
  uint8_t pin;
  uint8_t port;
};

static inline bool CANflict_GPIO_init(struct CANflict_GPIO_t *gpio,
                                      bool input) {
  printf("[GPIO] Initializing pin P_%x_%x", gpio->pin, gpio->port);

  if (input)
    printf(" as input\n");
  else
    printf(" as output\n");

  return true;
}

static inline bool CANflict_GPIO_deinit(struct CANflict_GPIO_t *gpio) {
  printf("[GPIO] Deinitialized pin P_%x_%x\n", gpio->pin, gpio->port);
  return true;
}

static inline bool CANflict_GPIO_getValue(struct CANflict_GPIO_t *gpio) {
  printf("[GPIO] Reading pin P_%x_%x\n", gpio->pin, gpio->port);
  return false;
}

static inline bool CANflict_GPIO_setValue(struct CANflict_GPIO_t *gpio,
                                          bool val) {
  printf("[GPIO] Writing value %d to pin P_%x_%x\n", val, gpio->pin,
         gpio->port);
  return true;
}
