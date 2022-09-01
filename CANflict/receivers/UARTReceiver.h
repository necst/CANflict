#pragma once

// Interface to implement (receiver or receiver)
#include "../CANflictReceiver.h"

// Peripherals used
#include "../platforms/peripherals/UART.h"

// ----------------- receiver Interface Implementation ------------------------
// Implement receiver struct
struct CANflict_ReceiverConf_t {
  struct CANflict_UART_t *uart;
  uint32_t freqHz;
  enum CANFLICT_UART_wordLen wordLen;  // how many bits in the payload
  enum CANflict_UART_stopBit stopBits; // how many stopbits
};

// Implement receiver functions

static inline bool
CANflict_receiver_init(struct CANflict_ReceiverConf_t *conf) {
  if (conf->uart == NULL)
    return false;

  bool result = CANflict_UART_init(conf->uart);
  result |= CANflict_UART_setBaudrate(conf->uart, conf->freqHz);
  result |= CANflict_UART_configure(conf->uart, conf->wordLen, conf->stopBits);

  return result;
}

static inline uint32_t CANflict_receiver_rcvBits(
    struct CANflict_ReceiverConf_t *conf,
    uint8_t *buf, // should be at least (nbits / 8) + 1 bytes long
    uint32_t nBits) {
  if (buf == NULL || conf == NULL)
    return 0U;

  uint32_t result = CANflict_UART_read(conf->uart, buf, (nBits / 8) + 1);
  return result * 8;
}

static inline bool
CANflict_receiver_deinit(struct CANflict_ReceiverConf_t *conf) {
  return CANflict_UART_deinit(conf->uart);
}
