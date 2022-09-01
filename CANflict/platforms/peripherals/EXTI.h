#pragma once

///\file EXTI.h
///\brief This file defines the interface of an abstract GPIO interrupt (usually
///called external interrupts) peripheral.

#include <stdint.h>

bool CANflict_EXTI_init();
bool CANflict_EXTI_deinit();
bool CANflict_EXTI_callback(uint8_t fallingEdge); // TODO: setConversionCallback
