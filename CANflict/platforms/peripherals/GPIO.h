#pragma once

///\file GPIO.h
///\brief This file defines an abstract interface for GPIO peripherals.

#include <stdbool.h>
#include <stdint.h>

struct CANflict_GPIO_t;

static inline bool CANflict_GPIO_init(struct CANflict_GPIO_t *gpio, bool input);
static inline bool CANflict_GPIO_deinit(struct CANflict_GPIO_t *gpio);
static inline bool CANflict_GPIO_getValue(struct CANflict_GPIO_t *gpio);
static inline bool CANflict_GPIO_setValue(struct CANflict_GPIO_t *gpio,
                                          bool val);
