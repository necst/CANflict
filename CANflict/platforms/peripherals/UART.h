#ifndef CANFLICT_UART_ABSTRACT
#define CANFLICT_UART_ABSTRACT

///\file UART.h
///\brief This file defines the interface of an abstract UART peripheral.

#include <stdbool.h>
#include <stdint.h>

struct CANflict_UART_t;

static inline bool CANflict_UART_init(struct CANflict_UART_t *uart);
static inline bool CANflict_UART_deinit(struct CANflict_UART_t *uart);
static inline bool CANflict_UART_setBaudrate(struct CANflict_UART_t *uart,
                                             uint32_t baudrateHz);

enum CANFLICT_UART_wordLen {
  CANFLICT_UART_wordLen_5 = 5,
  CANFLICT_UART_wordLen_6 = 6,
  CANFLICT_UART_wordLen_7 = 7,
  CANFLICT_UART_wordLen_8 = 8
};

enum CANflict_UART_stopBit {
  CANflict_UART_stopBit_1 = 1,
  CANflict_UART_stopBit_2 = 2
};

///\param wordLen number of bits in each UART packet, from 5 to 8.
///\param stopBits number of stop bits at the end of each UART packet, either 1
///                or 2.
///\return false if the configuration cannot be applied.
static inline bool CANflict_UART_configure(struct CANflict_UART_t *uart,
                                           enum CANFLICT_UART_wordLen wordLen,
                                           enum CANflict_UART_stopBit stopBits);
///\return the number of bytes read
static inline uint32_t CANflict_UART_read(struct CANflict_UART_t *uart,
                                          uint8_t *buf, uint32_t byteLen);

///\param msgBytes array of bytes containing the payload of each UART packet to
///                send.
///\param byteLen number of packets to send.
///\return the number of packets sent
static inline uint32_t CANflict_UART_write(struct CANflict_UART_t *uart,
                                           uint8_t *msgBytes, uint32_t byteLen);

#endif
