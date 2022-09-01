#ifndef CANFLICT_ABSTRACT_TIM
#define CANFLICT_ABSTRACT_TIM

///\file Timer.h
///\brief This file defines the interface of an abstract Timer peripheral.

#include <stdbool.h>
#include <stdint.h>

struct CANflict_TIM_t;
typedef void (*CANflict_TIM_Callback_t)();

static inline bool CANflict_Timer_init(struct CANflict_TIM_t *tim);
static inline bool CANflict_Timer_deinit(struct CANflict_TIM_t *tim);
static inline bool CANflict_Timer_setReloadVal(struct CANflict_TIM_t *tim,
                                               uint32_t reloadVal);
static inline bool CANflict_Timer_setCallback(struct CANflict_TIM_t *tim,
                                              CANflict_TIM_Callback_t callback);
static inline bool CANflict_Timer_start(struct CANflict_TIM_t *tim);
static inline bool CANflict_Timer_stop(struct CANflict_TIM_t *tim);

#endif
