#ifndef CANFLICT_GPIOSENDER
#define CANFLICT_GPIOSENDER

// Interface to implement (receiver or sender)
#include "../CANflictSender.h"

// Peripherals used
#include "../platforms/peripherals/GPIO.h"
#include "../platforms/peripherals/TIM.h"

// ----------------- Sender Interface Implementation ---------------------------
// Implement sender struct
struct CANflict_SenderConf_t {
  struct CANflict_GPIO_t *gpio;
  struct CANflict_TIM_t *tim;
  uint32_t reloadVal;
  bool blocking;

  const bool *buf;
  uint32_t len;
  uint32_t curBit;
};
// -----------------------------------------------------------------------------

// Global timer config. Needs to be global to be accessed by interrupts.
static struct CANflict_SenderConf_t *g_CANflict_TIM_Conf;

// Callback triggered on each bit
static inline void timerCallback(void) {
  uint32_t curBit = g_CANflict_TIM_Conf->curBit;

  if (curBit < g_CANflict_TIM_Conf->len) {
    CANflict_GPIO_setValue(g_CANflict_TIM_Conf->gpio,
                           g_CANflict_TIM_Conf->buf[curBit]);
    ++(g_CANflict_TIM_Conf->curBit);
  } else {
    // Leave the bus in recessive state
    CANflict_GPIO_setValue(g_CANflict_TIM_Conf->gpio, true);
    CANflict_Timer_stop(g_CANflict_TIM_Conf->tim);
  }
}

// ----------------- Sender Interface Implementation ---------------------------
// Implement sender functions
static inline bool CANflict_sender_init(struct CANflict_SenderConf_t *conf) {
  bool initStatus = CANflict_GPIO_init(conf->gpio, /*input=*/false);

  initStatus &= CANflict_Timer_init(conf->tim);
  initStatus &= CANflict_Timer_setReloadVal(conf->tim, conf->reloadVal);
  initStatus &= CANflict_Timer_setCallback(conf->tim, timerCallback);

  g_CANflict_TIM_Conf = conf;
  conf->buf = NULL;
  conf->len = 0U;
  conf->curBit = 0U;
  return initStatus;
}

static inline bool CANflict_sender_deinit(struct CANflict_SenderConf_t *conf) {
  if (conf != g_CANflict_TIM_Conf)
    return false;

  CANflict_Timer_stop(conf->tim);
  CANflict_GPIO_setValue(conf->gpio, true);

  g_CANflict_TIM_Conf = NULL;
  conf->buf = NULL;
  conf->len = 0U;
  conf->curBit = 0U;

  bool status = CANflict_GPIO_deinit(conf->gpio);
  status &= CANflict_Timer_deinit(conf->tim);

  return status;
}

static inline uint32_t
CANflict_sender_setMsgToSend(struct CANflict_SenderConf_t *conf,
                             const bool bitstream[], const uint32_t nBits) {
  if (bitstream == NULL)
    return 0U;

  g_CANflict_TIM_Conf->buf = bitstream;
  g_CANflict_TIM_Conf->len = nBits;
  g_CANflict_TIM_Conf->curBit = 0U;
  return nBits;
}

static inline uint32_t
CANflict_sender_sendMsg(struct CANflict_SenderConf_t *conf) {
  if (g_CANflict_TIM_Conf->buf == NULL)
    return 0U;

  CANflict_Timer_start(g_CANflict_TIM_Conf->tim);

  uint32_t bitsToWait =
      (g_CANflict_TIM_Conf->blocking) ? g_CANflict_TIM_Conf->len : 1U;
  while (g_CANflict_TIM_Conf->curBit < bitsToWait)
    ;

  return g_CANflict_TIM_Conf->curBit;
}

#endif
