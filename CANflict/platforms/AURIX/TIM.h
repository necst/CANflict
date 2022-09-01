#pragma once

#include "config.h"

// The peripheral interface that is being implemented
#include "../peripherals/TIM.h"

// Platform-specific header
#include <IfxGpt12.h>
#include <IfxPort.h>
#include <Ifx_Types.h>

IFX_INTERRUPT(interruptGpt12, 0, ISR_PRIORITY_GPT12_TIMER);

static inline void CANflict_TIM_defaultCallback(void) {
  while (1)
    ;
}
// Timer callback - executed on TIM interrupt
static CANflict_TIM_Callback_t g_CANflict_TIM_callback =
    CANflict_TIM_defaultCallback;

// Implement structs
struct CANflict_TIM_t {
  Ifx_GPT12 *timer;
  uint16_t reloadVal;
};

static struct CANflict_TIM_t *g_timer;

// Define the interrupt handler
void interruptGpt12(void) {
  IfxGpt12_T3_setTimerValue(g_timer->timer, g_timer->reloadVal);
  g_CANflict_TIM_callback();
}

// ------------------- TIM Interface Implementation -------------------------

// Implement functions
static inline bool CANflict_Timer_init(struct CANflict_TIM_t *tim) {
  g_timer = tim;

  // Initialize the GPT12 module
  IfxGpt12_enableModule(tim->timer);
  // Set T3 to timer mode
  IfxGpt12_T3_setMode(tim->timer, IfxGpt12_Mode_timer);
  IfxGpt12_T3_setTimerDirection(tim->timer, IfxGpt12_TimerDirection_down);
  IfxGpt12_setGpt1BlockPrescaler(tim->timer, IfxGpt12_Gpt1BlockPrescaler_4);
  IfxGpt12_T3_setTimerPrescaler(tim->timer, IfxGpt12_TimerInputPrescaler_1);

  // Get the interrupt address
  volatile Ifx_SRC_SRCR *src = IfxGpt12_T3_getSrc(tim->timer);
  // Initialize ISR
  IfxSrc_init(src, ISR_PROVIDER_GPT12_TIMER, ISR_PRIORITY_GPT12_TIMER);
  // Enable IRQ
  IfxSrc_enable(src);

  return true;
}

static inline bool CANflict_Timer_deinit(struct CANflict_TIM_t *tim) {
  if (g_timer != tim)
    return false;

  IfxGpt12_disableModule(tim->timer);
  return true;
}

static inline bool CANflict_Timer_start(struct CANflict_TIM_t *tim) {
  if (g_timer != tim)
    return false;

  IfxGpt12_T3_run(tim->timer, IfxGpt12_TimerRun_start);
  return true;
}
static inline bool CANflict_Timer_stop(struct CANflict_TIM_t *tim) {
  if (g_timer != tim)
    return false;

  IfxGpt12_T3_run(tim->timer, IfxGpt12_TimerRun_stop);
  return true;
}

static inline bool CANflict_Timer_setReloadVal(struct CANflict_TIM_t *tim,
                                               uint32_t reloadVal) {
  if (g_timer != tim)
    return false;

  g_timer->reloadVal = reloadVal;
  IfxGpt12_T3_setTimerValue(tim->timer, reloadVal);
  return true;
}
static inline bool
CANflict_Timer_setCallback(struct CANflict_TIM_t *tim,
                           CANflict_TIM_Callback_t callback) {
  if (g_timer != tim)
    return false;

  g_CANflict_TIM_callback = callback;
  return true;
}
