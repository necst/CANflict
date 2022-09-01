#pragma once

#include "config.h"

// The peripheral interface that is being implemented
#include "../peripherals/TIM.h"

// Platform-specific header
#include <board.h>

static inline void CANflict_TIM_defaultCallback(void) {
  while (1)
    ;
}
// Timer callback - executed on TIM interrupt
static CANflict_TIM_Callback_t g_CANflict_TIM_callback =
    CANflict_TIM_defaultCallback;

// Define the interrupt handler for the selected timer
#if (CANFLICT_LPC11_TIMER == CANFLICT_LPC11_TIMER16_0)
void TIMER16_0_IRQHandler(void) {
  if (Chip_TIMER_MatchPending(LPC_TIMER16_0, 1)) {
    Chip_TIMER_ClearMatch(LPC_TIMER16_0, 1);
    g_CANflict_TIM_callback();
  }
}
#elif (CANFLICT_LPC11_TIMER == CANFLICT_LPC11_TIMER16_1)
void TIMER16_1_IRQHandler(void) {
  if (Chip_TIMER_MatchPending(LPC_TIMER16_1, 1)) {
    Chip_TIMER_ClearMatch(LPC_TIMER16_1, 1);
    g_CANflict_TIM_callback();
  }
}
#elif (CANFLICT_LPC11_TIMER == CANFLICT_LPC11_TIMER32_0)
void TIMER32_0_IRQHandler(void) {
  if (Chip_TIMER_MatchPending(LPC_TIMER32_0, 1)) {
    Chip_TIMER_ClearMatch(LPC_TIMER32_0, 1);
    g_CANflict_TIM_callback();
  }
}
#elif (CANFLICT_LPC11_TIMER == CANFLICT_LPC11_TIMER32_1)
void TIMER32_1_IRQHandler(void) {
  if (Chip_TIMER_MatchPending(LPC_TIMER32_1, 1)) {
    Chip_TIMER_ClearMatch(LPC_TIMER32_1, 1);
    g_CANflict_TIM_callback();
  }
}
#endif

// ------------------- TIM Interface Implementation -------------------------

// Implement structs
struct CANflict_TIM_t {
  LPC_TIMER_T *timer;
};

// Implement functions

static inline bool CANflict_Timer_init(struct CANflict_TIM_t *tim) {
#if (CANFLICT_LPC11_TIMER == CANFLICT_LPC11_TIMER16_0)
  tim->timer = LPC_TIMER16_0;
#elif (CANFLICT_LPC11_TIMER == CANFLICT_LPC11_TIMER16_1)
  tim->timer = LPC_TIMER16_1;
#elif (CANFLICT_LPC11_TIMER == CANFLICT_LPC11_TIMER32_0)
  tim->timer = LPC_TIMER32_0;
#elif (CANFLICT_LPC11_TIMER == CANFLICT_LPC11_TIMER32_1)
  tim->timer = LPC_TIMER32_1;
#endif
  Chip_TIMER_Init(tim->timer);
  Chip_TIMER_Reset(tim->timer);

  // Enable interrupt generation
  Chip_TIMER_MatchEnableInt(tim->timer, 1);
  Chip_TIMER_ResetOnMatchEnable(tim->timer, 1);

  // Enable interrupt handling

#if (CANFLICT_LPC11_TIMER == CANFLICT_LPC11_TIMER16_0)
  NVIC_ClearPendingIRQ(TIMER_16_0_IRQn);
  NVIC_EnableIRQ(TIMER_16_0_IRQn);
#elif (CANFLICT_LPC11_TIMER == CANFLICT_LPC11_TIMER16_1)
  NVIC_ClearPendingIRQ(TIMER_16_1_IRQn);
  NVIC_EnableIRQ(TIMER_16_1_IRQn);
#elif (CANFLICT_LPC11_TIMER == CANFLICT_LPC11_TIMER32_0)
  NVIC_ClearPendingIRQ(TIMER_32_0_IRQn);
  NVIC_EnableIRQ(TIMER_32_0_IRQn);
#elif (CANFLICT_LPC11_TIMER == CANFLICT_LPC11_TIMER32_1)
  NVIC_ClearPendingIRQ(TIMER_32_1_IRQn);
  NVIC_EnableIRQ(TIMER_32_1_IRQn);
#endif

  return true;
}
static inline bool CANflict_Timer_deinit(struct CANflict_TIM_t *tim) {
  Chip_TIMER_DeInit(tim->timer);
  return true;
}

static inline bool CANflict_Timer_start(struct CANflict_TIM_t *tim) {
  Chip_TIMER_Enable(tim->timer);
  return true;
}
static inline bool CANflict_Timer_stop(struct CANflict_TIM_t *tim) {
  Chip_TIMER_Disable(tim->timer);
  return true;
}

static inline bool CANflict_Timer_setReloadVal(struct CANflict_TIM_t *tim,
                                               uint32_t reloadVal) {
  Chip_TIMER_SetMatch(tim->timer, 1, reloadVal);
  return true;
}
static inline bool
CANflict_Timer_setCallback(struct CANflict_TIM_t *tim,
                           CANflict_TIM_Callback_t callback) {
  g_CANflict_TIM_callback = callback;
  return true;
}
