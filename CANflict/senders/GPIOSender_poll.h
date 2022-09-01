#pragma once

// Interface to implement (receiver or sender)
#include "../CANflictSender.h"

// Peripherals used
#include "../platforms/peripherals/GPIO.h"

// Helper struct
struct CANflict_GPIOSender_msg {
  const bool *buf;
  uint32_t len;
};

// ----------------- Sender Interface Implementation ---------------------------

// Implement structs
struct CANflict_SenderConf_t {
  struct CANflict_GPIO_t *gpio;
  uint32_t cyclesBetweenBits;
  struct CANflict_GPIOSender_msg msg; // Used internally, no need to set this
};

// Implement functions

static inline bool CANflict_sender_init(struct CANflict_SenderConf_t *conf) {
  conf->msg.buf = NULL;
  conf->msg.len = 0U;
  return CANflict_GPIO_init(conf->gpio, /*input=*/false);
}

static inline bool CANflict_sender_deinit(struct CANflict_SenderConf_t *conf) {
  // Reset GPIO value to HIGH
  CANflict_GPIO_setValue(conf->gpio, true);
  conf->msg.buf = NULL;
  conf->msg.len = 0U;
  return CANflict_GPIO_deinit(conf->gpio);
}

static inline uint32_t
CANflict_sender_setMsgToSend(struct CANflict_SenderConf_t *conf,
                             const bool bitstream[], const uint32_t nBits) {
  if (bitstream == NULL)
    return 0U;

  conf->msg.buf = bitstream;
  conf->msg.len = nBits;
  return nBits;
}

static inline uint32_t
CANflict_sender_sendMsg(struct CANflict_SenderConf_t *conf) {
  if (conf->msg.buf == NULL || conf->msg.len == 0)
    return 0U;

  for (uint32_t curBit = 0; curBit < conf->msg.len; curBit++) {
    // Wait for next bit
    for (unsigned i = 0; i < conf->cyclesBetweenBits; ++i)
      ;

    // Read bit
    CANflict_GPIO_setValue(conf->gpio, conf->msg.buf[curBit]);
  }

  return conf->msg.len;
}
