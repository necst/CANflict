#pragma once

// The peripheral interface that is being implemented
#include "../peripherals/GPIO.h"

// Platform-specific header
#include "stm32l5xx_hal.h"

// ------------------- GPIO Interface Implementation ---------------------------

// Implement structs
struct CANflict_GPIO_t {
  GPIO_TypeDef *port;
  uint32_t pin;
};

// Implement functions
static inline bool CANflict_GPIO_init(struct CANflict_GPIO_t *gpio,
                                      bool input) {
  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(gpio->port, gpio->pin, GPIO_PIN_SET);
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = gpio->pin;
  GPIO_InitStruct.Mode = input ? GPIO_MODE_INPUT : GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(gpio->port, &GPIO_InitStruct);

  return true;
}

static inline bool CANflict_GPIO_deinit(struct CANflict_GPIO_t *gpio) {
  return true;
}

static inline bool CANflict_GPIO_getValue(struct CANflict_GPIO_t *gpio) {
  return HAL_GPIO_ReadPin(gpio->port, gpio->pin);
}

static inline bool CANflict_GPIO_setValue(struct CANflict_GPIO_t *gpio,
                                          bool val) {
  HAL_GPIO_WritePin(gpio->port, gpio->pin, val);
  return true;
}
