#pragma once

// Interface to implement (receiver or sender)
#include "../CANflictSender.h"

// Peripherals used
#include "../platforms/peripherals/SPI.h"
#include <stdlib.h>

// ----------------- Sender Interface Implementation ---------------------------
// Implement sender struct
struct CANflict_SenderConf_t {
  struct CANflict_SPI_t *spi;
  uint32_t freqHz;
  uint8_t nSamples;
  // The following fields are used internally and should not be set by the user.
  uint8_t *msgBytes;
  uint32_t byteLen;
};

// Implement sender functions

static inline bool CANflict_sender_init(struct CANflict_SenderConf_t *conf) {
  if (conf->spi == NULL)
    return false;

  bool result = CANflict_SPI_init(conf->spi);
  result |= CANflict_SPI_setBaudrate(conf->spi, conf->freqHz * conf->nSamples);
  conf->msgBytes = NULL;
  return result;
}

static inline uint32_t
CANflict_sender_setMsgToSend(struct CANflict_SenderConf_t *conf,
                             const bool bitstream[], const uint32_t nBits) {
  if (conf == NULL)
    return 0U;

  uint8_t curBit = 0;
  uint8_t bitCnt = 0;
  uint8_t nBytes = ((nBits / 8) + ((nBits % 8) > 0)) * conf->nSamples;

  if (conf->msgBytes != NULL)
    free(conf->msgBytes);

  conf->msgBytes = malloc(nBytes);

  for (uint8_t curByte = 0; curByte < nBytes; ++curByte) {
    // Transform TX message from Bits to Bytes
    uint8_t content = 0;
    for (uint8_t j = 1; j <= 8; j++) {
      uint8_t bitToSend = 1; // If past the last bit, send '1'

      if (curBit < nBits)
        bitToSend = bitstream[curBit];

      content |= bitToSend << (8 - j);

      if (bitCnt < (conf->nSamples - 1)) {
        bitCnt++;
      } else {
        curBit++;
        bitCnt = 0;
      }
    }
    conf->msgBytes[curByte] = (content & 0xFFU);
  }
  conf->byteLen = nBytes;
  return nBytes * 8;
}

static inline uint32_t
CANflict_sender_sendMsg(struct CANflict_SenderConf_t *conf) {
  if (conf == NULL)
    return 0U;

  bool result = CANflict_SPI_write(conf->spi, conf->msgBytes, conf->byteLen);
  return result;
}

static inline bool CANflict_sender_deinit(struct CANflict_SenderConf_t *conf) {
  conf->msgBytes = NULL;
  conf->byteLen = 0U;

  if (conf->msgBytes != NULL)
    free(conf->msgBytes);

  return CANflict_SPI_deinit(conf->spi);
}
