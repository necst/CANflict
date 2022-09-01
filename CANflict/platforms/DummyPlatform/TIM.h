#pragma once

// The peripheral interface that is being implemented
#include "../peripherals/TIM.h"

// Platform-specific code
#include <stdio.h>

static inline void CANflict_TIM_defaultCallback(void) {
  printf("Error! No callback set.");
}
static CANflict_TIM_Callback_t g_CANflict_TIM_callback =
    CANflict_TIM_defaultCallback;

// ------------------- TIM Interface Implementation -------------------------

// Implement structs
struct CANflict_TIM_t {
  uint32_t id;
  bool state;
};
struct CANflict_TIM_t *g_CANflict_tim;

// Implement functions

static inline bool CANflict_Timer_init(struct CANflict_TIM_t *tim) {
  g_CANflict_tim = tim;
  tim->state = false;

  printf("Timer %u initialized\n", tim->id);
  return true;
}

static inline bool CANflict_Timer_deinit(struct CANflict_TIM_t *tim) {
  if (g_CANflict_tim != tim) {
    printf("Wrong timer\n");
    return false;
  }

  printf("Timer %u deinitialized\n", tim->id);
  tim->state = false;
  return true;
}

static inline bool CANflict_Timer_start(struct CANflict_TIM_t *tim) {
  if (g_CANflict_tim != tim) {
    printf("Wrong timer\n");
    return false;
  }

  printf("Timer %u started\n", tim->id);
  tim->state = true;

  while (tim->state) {
    for (int i = 0; i < 1000000; i++)
      ;
    printf("Running timer %u callback\n", tim->id);
    g_CANflict_TIM_callback();
  }
  return true;
}

static inline bool CANflict_Timer_stop(struct CANflict_TIM_t *tim) {
  if (g_CANflict_tim != tim) {
    printf("Wrong timer\n");
    return false;
  }

  printf("Timer %u stopped\n", tim->id);
  tim->state = false;
  return true;
}

static inline bool CANflict_Timer_setFreq(struct CANflict_TIM_t *tim,
                                          uint32_t freqHz) {
  if (g_CANflict_tim != tim) {
    printf("Wrong timer\n");
    return false;
  }
  printf("Timer %u frequency set to %d\n", tim->id, freqHz);
  return true;
}

static inline bool
CANflict_Timer_setCallback(struct CANflict_TIM_t *tim,
                           CANflict_TIM_Callback_t callback) {
  if (g_CANflict_tim != tim) {
    printf("Wrong timer\n");
    return false;
  }

  printf("Timer %u callback set\n", tim->id);
  g_CANflict_TIM_callback = callback;
}
