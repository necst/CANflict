#ifndef CANFLICT_GPIORECEIVER
#define CANFLICT_GPIORECEIVER

// Interface to implement (receiver or sender)
#include "../CANflictReceiver.h"

// Peripherals used
#include "../platforms/peripherals/GPIO.h"
#include "../platforms/peripherals/TIM.h"

// ----------------- Receiver Interface Implementation -------------------------
struct CANflict_ReceiverConf_t {
  struct CANflict_GPIO_t *gpio;
  struct CANflict_TIM_t *tim;
  uint32_t reloadVal;
  bool blocking;

  // Private members, not set by the user
  bool *buf;
  uint32_t len;
  uint32_t curBit;
};
// -----------------------------------------------------------------------------

// Global timer config. Needs to be global to be accessed by interrupts.
static struct CANflict_ReceiverConf_t *g_CANflict_RXTIM_Conf;

// Callback triggered on each bit
static inline void timerCallback(void) {
  struct CANflict_ReceiverConf_t *conf = g_CANflict_RXTIM_Conf;

  if (conf->curBit < conf->len) {
    conf->buf[conf->curBit] = CANflict_GPIO_getValue(conf->gpio);
    ++(conf->curBit);
  }
}

// ----------------- Receiver Interface Implementation -------------------------
// Implement rcv functions
static inline bool
CANflict_receiver_init(struct CANflict_ReceiverConf_t *conf) {
  bool initStatus = CANflict_GPIO_init(conf->gpio, /*input=*/true);

  initStatus &= CANflict_Timer_init(conf->tim);
  initStatus &= CANflict_Timer_setReloadVal(conf->tim, conf->reloadVal);
  initStatus &= CANflict_Timer_setCallback(conf->tim, timerCallback);

  g_CANflict_RXTIM_Conf = conf;
  conf->buf = NULL;
  conf->len = 0U;
  conf->curBit = 0U;
  return initStatus;
}

static inline bool
CANflict_receiver_deinit(struct CANflict_ReceiverConf_t *conf) {
  if (conf != g_CANflict_RXTIM_Conf)
    return false;

  CANflict_Timer_stop(conf->tim);

  g_CANflict_RXTIM_Conf = NULL;
  conf->buf = NULL;
  conf->len = 0U;
  conf->curBit = 0U;

  bool status = CANflict_GPIO_deinit(conf->gpio);
  status &= CANflict_Timer_deinit(conf->tim);

  return status;
}

static inline uint32_t
CANflict_receiver_rcvBits(struct CANflict_ReceiverConf_t *conf, uint8_t *buf,
                          uint32_t nBits) {
  if (conf != g_CANflict_RXTIM_Conf)
    return 0U;

  conf->buf == buf;
  conf->len = nBits;

  CANflict_Timer_start(conf->tim);

  uint32_t bitsToWait = (conf->blocking) ? conf->len : 0U;
  while (conf->curBit < bitsToWait)
    ;

  if (conf->blocking)
    CANflict_Timer_stop(conf->tim);

  return conf->curBit;
}

#endif
