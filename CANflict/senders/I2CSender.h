#pragma once

// Interface to implement (receiver or sender)
#include "../CANflictSender.h"

// Peripherals used
#include "../platforms/peripherals/I2C.h"

#include <stdlib.h>

// ----------------- Sender Interface Implementation ---------------------------
// Implement sender struct
struct CANflict_SenderConf_t {
  struct CANflict_I2C_t *I2C;

  // Note: the duration of the start/stop condition and the inter-frame
  // space is generally a constant and depends on the specific device.
  // Therefore, the baudrate should be chosen so that all of these durations
  // are a mutiple of the bit duration.
  // The duration in bits of the start/stop condition and the inter-frame
  // space can then be calculated as baudrate(1/s)*duration(s).
  uint32_t freqHz;
  uint8_t startCondDuration;  ///< length of the start condition in bits
  uint8_t stopCondDuration;   ///< length of the stop condition in bits
  uint8_t interFrameDuration; ///< length of the inter frame space, in bits

  // The following fields are used internally and should not be set by the user
  uint8_t *msgBytes;
  bool *rwBits;
  uint32_t byteLen;
};

// Implement sender functions

static inline bool CANflict_sender_init(struct CANflict_SenderConf_t *conf) {
  if (conf->I2C == NULL || conf->msgBytes == NULL)
    return false;

  bool result = CANflict_I2C_init(conf->I2C);
  result |= CANflict_I2C_setBaudrate(conf->I2C, conf->freqHz);
  return result;
}

/*
 * I2C frame formatting for CAN communication:
 *  _____________ _______________ ___________ _______ _____________          __
 * |             |               |           |       |             |  inter |
 * | start cond  | payload       |  R/W bit  |  ACK  |  stop cond  |  frame |
 * |_____________|_______________|___________|_______|_____________|  space |__
 *   ^^^^^^^^^^      ^^^^^^^^^^^^^^^^^^^^      ^^^^^      ^^^^^^       ^^^
 *      0                byte to send            1          0           1
 */
static inline uint32_t
CANflict_sender_setMsgToSend(struct CANflict_SenderConf_t *conf,
                             const bool bitstream[], const uint32_t nBits) {
  if (conf == NULL)
    return 0U;

  static const uint8_t payloadLen = 7;
  static const uint8_t rwBitLen = 1;
  static const uint8_t ackSlotLen = 1;

  const uint8_t payloadStart = conf->startCondDuration;
  const uint8_t payloadEnd = conf->startCondDuration + payloadLen - 1;
  const uint8_t rwBitPos = payloadEnd + 1;
  const uint8_t ackPos = payloadEnd + 2;
  const uint8_t pktEnd = payloadEnd + 2 + conf->stopCondDuration;

  const uint32_t pktBitLen = conf->startCondDuration + payloadLen + rwBitLen +
                             ackSlotLen + conf->stopCondDuration +
                             conf->interFrameDuration;

  conf->byteLen = (nBits / pktBitLen) + ((nBits % pktBitLen) > 0);
  conf->msgBytes = (uint8_t *)malloc(conf->byteLen);
  conf->rwBits = (bool *)malloc(conf->byteLen);

  uint32_t curPkt = 0U;
  bool first = true;
  uint32_t i;
  for (i = 0; i < nBits; i++) {
    const uint32_t bitIndexInPkt = (i % pktBitLen);
    const bool curBit = bitstream[i];

    // Increment packet number
    if (bitIndexInPkt == 0) {
      if (first)
        first = false;
      else
        curPkt++;

      conf->msgBytes[curPkt] = 0U;
    }

    if (bitIndexInPkt < payloadStart) {
      // Start condition must be 0
      if (curBit != 0)
        return 0U;

    } else if (bitIndexInPkt <= payloadEnd) {
      uint8_t bitIndexInPayload =
          payloadLen - (bitIndexInPkt - conf->startCondDuration);
      // Note: the payload byte gets left-shifted by the peripheral
      conf->msgBytes[curPkt] |= (curBit << (bitIndexInPayload - 1));
      printf("curbit: %d   pkt: %x\n", curBit, conf->msgBytes[curPkt]);

    } else if (bitIndexInPkt == rwBitPos) {
      conf->rwBits[curPkt] = curBit;
      printf("rwbit: %d   pkt: %x\n", curBit, conf->msgBytes[curPkt]);

    } else if (bitIndexInPkt == ackPos) {
      // Ack must be 1
      if (curBit != 1)
        return 0U;

    } else if (bitIndexInPkt <= pktEnd) {
      // Stop condition must be 0
      if (curBit != 0)
        return 0U;

    } else if (bitIndexInPkt > pktEnd) {
      // Inter-frame bits must be 1
      if (curBit != 1)
        return 0U;

    } else {
      // This should never be the case
      return 0U;
    }
  }

  conf->byteLen = curPkt + 1;
  return curPkt * 8;
}

static inline uint32_t
CANflict_sender_sendMsg(struct CANflict_SenderConf_t *conf) {
  if (conf == NULL)
    return 0U;

  bool result = CANflict_I2C_write(conf->I2C, conf->msgBytes, conf->rwBits,
                                   conf->byteLen);
  free(conf->msgBytes);
  free(conf->rwBits);
  return result;
}

static inline bool CANflict_sender_deinit(struct CANflict_SenderConf_t *conf) {
  conf->msgBytes = NULL;
  conf->rwBits = NULL;
  conf->byteLen = 0U;
  return CANflict_I2C_deinit(conf->I2C);
}
