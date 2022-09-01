#pragma once

// Interface to implement (receiver or sender)
#include "../CANflictReceiver.h"

// Peripherals used
#include "../platforms/peripherals/GPIO.h"

struct CANflict_ReceiverConf_t {
  struct CANflict_GPIO_t *gpio;
  uint64_t cyclesBetweenBits;
  uint64_t initialCycles;
};

static inline bool
CANflict_receiver_init(struct CANflict_ReceiverConf_t *conf) {
  return CANflict_GPIO_init(conf->gpio, /*input=*/true);
}

static inline bool
CANflict_receiver_deinit(struct CANflict_ReceiverConf_t *conf) {
  return CANflict_GPIO_deinit(conf->gpio);
}

static inline uint32_t
CANflict_receiver_rcvBits(struct CANflict_ReceiverConf_t *conf, uint8_t *buf,
                          uint32_t nBits) {
  //  if (buf == NULL || nBits == 0)
  //    return 0U;

  //  struct CANflict_GPIO_t* gpio = conf->gpio;
  uint64_t curBit = 0;

  // Wait for sampling point
  //  for (unsigned i = 0; i < conf->initialCycles; ++i)
  //    ;

  // Read first bit
  //  buf[curBit] = CANflict_GPIO_getValue(conf->gpio);
  //  ++curBit;

  while (curBit < nBits) {

    // Read bit
    buf[curBit] = CANflict_GPIO_getValue(conf->gpio);
    ++curBit;

    // Wait for next bit
    for (unsigned i = 0; i < conf->cyclesBetweenBits; ++i)
      ;
  }

  return curBit;
}
