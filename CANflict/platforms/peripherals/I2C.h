#pragma once

///\file I2C.h
///\brief This file defines the interface of an abstract I2C peripheral.

#include <stdbool.h>
#include <stdint.h>


struct CANflict_I2C_t;

static inline bool CANflict_I2C_init(struct CANflict_I2C_t *I2C);
static inline bool CANflict_I2C_deinit(struct CANflict_I2C_t *I2C);
static inline bool CANflict_I2C_setBaudrate(struct CANflict_I2C_t *I2C,
                                            uint32_t baudrateHz);

///\param bytes an array of bytes to be sent. Each byte will be shifted right by
///             1 before being sent.
///\param rwbits least significat bit of each byte to send.
///\param len length of both the \a bytes and \a rwbits arrays
///\param interFrameCycles number of cycles to wait between bits
///\return the number of bytes sent.
static inline uint32_t CANflict_I2C_write(struct CANflict_I2C_t *I2C,
                                          uint8_t *bytes, bool *rwBits,
                                          uint8_t len);
