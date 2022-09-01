#pragma once

///\file CANflict_sender.h
///\brief This file defines the public interface of CANflict senders.
///
/// Each sender must provide an implementation for all the functions and structs
/// defined here.
/// \note This file is meant to be included by senders, not by user code.

#include <stdbool.h>
#include <stdint.h>

///\brief Configuration struct.
///
/// This struct is used to identify a specific instance of a sender. It should
/// contain:
/// 1. The configurations struct of each peripheral used by the sender
/// 2. Any additional parameter that the user might want to tweak
struct CANflict_SenderConf_t;

///\brief Initialize the peripheral
///\return true if the initialization was successful
static inline bool CANflict_sender_init(struct CANflict_SenderConf_t *conf);

///\brief Prepare the sender for the next message.
///\return how many bits will be sent (0 means error).
///
/// This function can be called ahead of time to minimize the overhead when
/// injecting bits. Typical tasks performed by this functions should be:
/// 1. check if the bitstream is well-formed for the chosen technique
/// 2. encode the message with peripheral-specific features (endianness,
/// padding, stuffing etc...)
/// 3. return the number of bits that will be sent (0 if the message cannot be
/// sent)
static inline uint32_t
CANflict_sender_setMsgToSend(struct CANflict_SenderConf_t *conf,
                             const bool bitstream[], const uint32_t nBits);

///\brief Start sending the message on the bus as soon as possible.
///\return how many bits have been sent (0 means error).
static inline uint32_t
CANflict_sender_sendMsg(struct CANflict_SenderConf_t *conf);

///\brief De-initialize the peripheral.
///\return true if the de-initialization was successful.
static inline bool CANflict_sender_deinit(struct CANflict_SenderConf_t *conf);
