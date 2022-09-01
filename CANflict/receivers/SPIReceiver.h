#pragma once

// Interface to implement (receiver or sender)
#include "../CANflictReceiver.h"

// Peripherals used
#include "../platforms/peripherals/SPI.h"
#include <stdlib.h>

// ----------------- receiver Interface Implementation -------------------------
// Implement receiver struct
struct CANflict_ReceiverConf_t {
  struct CANflict_SPI_t *spi;
  uint32_t freqHz;
};

// Implement receiver functions
static inline bool
CANflict_receiver_init(struct CANflict_ReceiverConf_t *conf) {
  if (conf->spi == NULL)
    return false;

  bool result = CANflict_SPI_init(conf->spi);
  result |= CANflict_SPI_setBaudrate(conf->spi, conf->freqHz);
  return result;
}

static inline uint32_t CANflict_receiver_rcvBits(
    struct CANflict_ReceiverConf_t *conf,
    uint8_t *buf, // should be at least (nbits / 8) + 1 bytes long
    uint32_t nBits) {
  if (buf == NULL || conf == NULL)
    return 0U;

  uint32_t result = CANflict_SPI_read(conf->spi, buf, (nBits / 8) + 1);
  return result * 8;
}

static inline bool
CANflict_receiver_deinit(struct CANflict_ReceiverConf_t *conf) {
  return CANflict_SPI_deinit(conf->spi);
}
