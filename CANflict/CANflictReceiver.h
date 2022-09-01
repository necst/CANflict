#pragma once

///\file CANflict_receiver.h
///\brief This file defines the public interface of CANflict receivers.
///
/// Each receiver must provide an implementation for all the functions and
/// structs defined here.
/// \note This file is meant to be included by receivers, not by user code.

#include <stdbool.h>
#include <stdint.h>


///\brief Configuration struct, defined by the specific receiver
struct CANflict_ReceiverConf_t;

///\brief Initialize the peripheral
///\return true if the initialization was successful
static inline bool CANflict_receiver_init(struct CANflict_ReceiverConf_t *conf);

///\brief Start recording incoming traffic on the CAN bus
///\param[out] buf The final content is implementation-defined
///\return how many bits were effectively received
static inline uint32_t
CANflict_receiver_rcvBits(struct CANflict_ReceiverConf_t *conf, uint8_t *buf,
                          uint32_t nBits);

///\brief De-initialize the peripheral
///\return true if the de-initialization was successful
static inline bool
CANflict_receiver_deinit(struct CANflict_ReceiverConf_t *conf);
