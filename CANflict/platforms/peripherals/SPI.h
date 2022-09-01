#pragma once

///\file SPI.h
///\brief This file defines the interface of an abstract SPI peripheral.

#include <stdbool.h>
#include <stdint.h>

struct CANflict_SPI_t;

static inline bool CANflict_SPI_init(struct CANflict_SPI_t *spi);
static inline bool CANflict_SPI_deinit(struct CANflict_SPI_t *spi);
static inline bool CANflict_SPI_setBaudrate(struct CANflict_SPI_t *spi,
                                            uint32_t baudrateHz);

///\param buf buffer to store the received bytes.
///\param byteLen number of bytes to read.
///\return the number of bytes read
static inline uint32_t CANflict_SPI_read(struct CANflict_SPI_t *spi,
                                         uint8_t *buf, uint32_t byteLen);

///\param msgBytes array bytes to write, in big-endian format.
///\param byteLen number of bytes to write.
///\return the number of bytes sent
static inline uint32_t CANflict_SPI_write(struct CANflict_SPI_t *spi,
                                          uint8_t *msgBytes, uint32_t byteLen);
