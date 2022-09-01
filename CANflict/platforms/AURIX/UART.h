#ifndef CANFLICT_UART_AURIX
#define CANFLICT_UART_AURIX

#include "config.h"

// The peripheral interface that is being implemented
#include "../peripherals/UART.h"

// Platform-specific header
#include <IfxAsclin_Asc.h>
#include <IfxCpu_Irq.h>

// ------------------- UART Interface Implementation ---------------------------

struct CANflict_UART_t {
  Ifx_ASCLIN *uart;
  IfxAsclin_Rx_In *rxPin;
  IfxAsclin_Tx_Out *txPin;
  size_t txBufferSize;
  void *txBuffer;
  size_t rxBufferSize;
  void *rxBuffer;
};

static IfxAsclin_Asc g_ascHandle;

IFX_INTERRUPT(asclin1TxISR, 0, ISR_PRIORITY_UART_TX);
void asclin1TxISR(void) { IfxAsclin_Asc_isrTransmit(&g_ascHandle); }

IFX_INTERRUPT(asclin1RxISR, 0, ISR_PRIORITY_UART_RX);
void asclin1RxISR(void) { IfxAsclin_Asc_isrReceive(&g_ascHandle); }

static inline bool CANflict_UART_init(struct CANflict_UART_t *uart) {
  IfxAsclin_Asc_Config ascConfig;
  IfxAsclin_Asc_initModuleConfig(&ascConfig, uart->uart);

  // ISR priorities and interrupt target
  ascConfig.interrupt.txPriority = ISR_PRIORITY_UART_TX;
  ascConfig.interrupt.rxPriority = ISR_PRIORITY_UART_RX;
  ascConfig.interrupt.typeOfService = ISR_PROVIDER_UART;

  // FIFO configuration
  ascConfig.txBuffer = uart->txBuffer;
  ascConfig.txBufferSize = uart->txBufferSize;
  ascConfig.rxBuffer = uart->rxBuffer;
  ascConfig.rxBufferSize = uart->rxBufferSize;

  // Pin configuration
  const IfxAsclin_Asc_Pins pins = {
      NULL_PTR,
      IfxPort_InputMode_pullUp, /* CTS pin not used */
      uart->rxPin,
      IfxPort_InputMode_pullUp, /* RX pin           */
      NULL_PTR,
      IfxPort_OutputMode_pushPull, /* RTS pin not used */
      uart->txPin,
      IfxPort_OutputMode_pushPull, /* TX pin           */
      IfxPort_PadDriver_cmosAutomotiveSpeed1};
  ascConfig.pins = &pins;

  IfxAsclin_Asc_initModule(&g_ascHandle, &ascConfig);
  return true;
}

static inline bool
CANflict_UART_configure(struct CANflict_UART_t *uart,
                        enum CANFLICT_UART_wordLen wordLen,
                        enum CANflict_UART_stopBit stopBits) {
  if (uart == NULL || uart->uart != g_ascHandle.asclin)
    return false;

  IfxAsclin_DataLength wordLengthBits;
  switch (wordLen) {
  case CANFLICT_UART_wordLen_5:
    wordLengthBits = IfxAsclin_DataLength_5;
    break;
  case CANFLICT_UART_wordLen_6:
    wordLengthBits = IfxAsclin_DataLength_6;
    break;
  case CANFLICT_UART_wordLen_7:
    wordLengthBits = IfxAsclin_DataLength_7;
    break;
  case CANFLICT_UART_wordLen_8:
    wordLengthBits = IfxAsclin_DataLength_8;
    break;
  default:
    return false;
    break;
  }
  IfxAsclin_setDataLength(uart->uart, wordLengthBits);

  IfxAsclin_StopBit stopBitBits;
  switch (stopBits) {
  case CANflict_UART_stopBit_1:
    stopBitBits = IfxAsclin_StopBit_0;
    break;
  case CANflict_UART_stopBit_2:
    stopBitBits = IfxAsclin_StopBit_1;
    break;
  default:
    return false;
    break;
  }
  IfxAsclin_setStopBit(uart->uart, stopBitBits);

  return true;
}

static inline bool CANflict_UART_deinit(struct CANflict_UART_t *uart) {
  if (uart == NULL || uart->uart != g_ascHandle.asclin)
    return false;

  IfxAsclin_Asc_disableModule(&g_ascHandle);
  return true;
}

static inline bool CANflict_UART_setBaudrate(struct CANflict_UART_t *uart,
                                             uint32_t baudrateHz) {
  if (uart == NULL || uart->uart != g_ascHandle.asclin)
    return false;

  // temporarily set the clock source for baudrate configuration
  IfxAsclin_setClockSource(uart->uart, IfxAsclin_ClockSource_ascFastClock);
  IfxAsclin_setBitTiming(uart->uart, baudrateHz, IfxAsclin_OversamplingFactor_4,
                         IfxAsclin_SamplePointPosition_3,
                         IfxAsclin_SamplesPerBit_one);
  // disabling the clock again
  IfxAsclin_setClockSource(uart->uart, IfxAsclin_ClockSource_noClock);
  return true;
}

static inline uint32_t CANflict_UART_read(struct CANflict_UART_t *uart,
                                          uint8_t *buf, uint32_t byteLen) {
  if (uart == NULL || uart->uart != g_ascHandle.asclin)
    return false;

  for (int i = 0; i < byteLen; ++i)
    buf[i] = IfxAsclin_Asc_blockingRead(&g_ascHandle);
  return byteLen;
}

static inline uint32_t CANflict_UART_write(struct CANflict_UART_t *uart,
                                           uint8_t *msgBytes,
                                           uint32_t byteLen) {
  if (uart == NULL || uart->uart != g_ascHandle.asclin)
    return false;

  // TODO: might be useful to implement also a non-blocking version
  for (int i = 0; i < byteLen; ++i)
    IfxAsclin_Asc_blockingWrite(&g_ascHandle, msgBytes[i]);

  return byteLen;
}

#endif
