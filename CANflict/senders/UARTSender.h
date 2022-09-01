#pragma once

// Interface to implement (receiver or sender)
#include "../CANflictSender.h"

// Peripherals used
#include "../platforms/peripherals/UART.h"

// ----------------- Sender Interface Implementation ---------------------------
// Implement sender struct
struct CANflict_SenderConf_t {
  struct CANflict_UART_t *uart;
  uint32_t freqHz;
  enum CANFLICT_UART_wordLen wordLen;  // how many bits in the payload
  enum CANflict_UART_stopBit stopBits; // how many stopbits
  uint8_t *msgBytes; // this buffer should be provided by the user
  uint32_t byteLen;
};

// Implement sender functions

static inline bool CANflict_sender_init(struct CANflict_SenderConf_t *conf) {
  if (conf->uart == NULL || conf->msgBytes == NULL)
    return false;

  bool result = CANflict_UART_init(conf->uart);
  result |= CANflict_UART_setBaudrate(conf->uart, conf->freqHz);
  result |= CANflict_UART_configure(conf->uart, conf->wordLen, conf->stopBits);

  return result;
}

// Implements both message encoding and checking for the bitstream. To enable
// the bitstream to be sent via UART, it must be divisible into valid UART
// packets. Each packet is `conf->wordLen` bits long, must start with a 0 and
// the last `conf->stopbits` bits must be 1. The rest of the packet is free from
// limitations.
static inline uint32_t
CANflict_sender_setMsgToSend(struct CANflict_SenderConf_t *conf,
                             const bool bitstream[], const uint32_t nBits) {
  if (conf == NULL)
    return 0U;

  const uint32_t pktLen = (conf->wordLen + conf->stopBits + /*startBit=*/1);
  uint32_t curPkt = 0U;
  bool first = true;
  uint32_t i;
  for (i = 0; i < nBits; i++) {
    const uint32_t bitIndexInPkt = (i % pktLen);
    const bool curBit = bitstream[i];

    if (bitIndexInPkt == 0) {
      // First bit must be 0
      if (curBit != 0)
        return false;

      // Increment packet number
      if (first)
        first = false;
      else
        curPkt++;

      // Zero out current packet
      conf->msgBytes[curPkt] = 0U;
    } else if (bitIndexInPkt >= (pktLen - conf->stopBits)) {
      // Stop bits must be 1
      if (curBit != 1)
        return false;
    } else {
      conf->msgBytes[curPkt] |= (curBit << (bitIndexInPkt - 1));
    }
  }

  // Add padding at the end
  while (i % pktLen != 0) {
    const uint32_t bitIndexInPkt = (i % pktLen);

    conf->msgBytes[curPkt] |= (1 << (bitIndexInPkt - 1));
    i++;
  }

  conf->byteLen = curPkt + 1;
  return conf->byteLen * 8;
}

static inline uint32_t
CANflict_sender_sendMsg(struct CANflict_SenderConf_t *conf) {
  if (conf == NULL)
    return 0U;

  return CANflict_UART_write(conf->uart, conf->msgBytes, conf->byteLen);
}

static inline bool CANflict_sender_deinit(struct CANflict_SenderConf_t *conf) {
  conf->msgBytes = NULL;
  conf->byteLen = 0U;
  return CANflict_UART_deinit(conf->uart);
}
