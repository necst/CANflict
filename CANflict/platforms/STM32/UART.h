//#pragma once
//
//// The peripheral interface that is being implemented
//#include "../peripherals/UART.h"
//
//// Platform-specific header
//#include <board.h>
//
// static void CANflict_UART_InitPins(void) {
//#if (defined(BOARD_NXP_XPRESSO_11U14) || defined(BOARD_NGX_BLUEBOARD_11U24))
//  Chip_IOCON_PinMuxSet(
//      LPC_IOCON, 0, 18,
//      IOCON_FUNC1 | IOCON_MODE_INACT); /* PIO0_18 used for RXD */
//  Chip_IOCON_PinMuxSet(
//      LPC_IOCON, 0, 19,
//      IOCON_FUNC1 | IOCON_MODE_INACT); /* PIO0_19 used for TXD */
//#elif(defined(BOARD_NXP_XPRESSO_11C24) || defined(BOARD_MCORE48_1125))
//  Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_6,
//                       (IOCON_FUNC1 | IOCON_MODE_INACT)); /* RXD */
//  Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_7,
//                       (IOCON_FUNC1 | IOCON_MODE_INACT)); /* TXD */
//#else
//#error "No Pin muxing defined for UART operation"
//#endif
//}
//
//// ------------------- UART Interface Implementation
///---------------------------
//
// struct CANflict_UART_t {
//  LPC_USART_T* uart;
//};
//
// static inline bool CANflict_UART_init(struct CANflict_UART_t* uart) {
//  CANflict_UART_InitPins();
//
//  Chip_UART_Init(uart->uart);
//  Chip_UART_TXEnable(uart->uart);
//
//  return true;
//}
//
// static inline bool CANflict_UART_configure(struct CANflict_UART_t* uart,
//                                          enum CANFLICT_UART_wordLen wordLen,
//                                          enum CANflict_UART_stopBit stopBits)
//                                          {
//  uint32_t wordLengthBits = 0U;
//  switch (wordLen) {
//    case CANFLICT_UART_wordLen_5:
//      wordLengthBits = UART_LCR_WLEN5;
//      break;
//    case CANFLICT_UART_wordLen_6:
//      wordLengthBits = UART_LCR_WLEN6;
//      break;
//    case CANFLICT_UART_wordLen_7:
//      wordLengthBits = UART_LCR_WLEN7;
//      break;
//    case CANFLICT_UART_wordLen_8:
//      wordLengthBits = UART_LCR_WLEN8;
//      break;
//    default:
//      return false;
//      break;
//  }
//
//  uint32_t stopBitBits = 0U;
//  switch (stopBits) {
//    case CANflict_UART_stopBit_1:
//      stopBitBits = UART_LCR_SBS_1BIT;
//      break;
//    case CANflict_UART_stopBit_2:
//      stopBitBits = UART_LCR_SBS_2BIT;
//      break;
//    default:
//      return false;
//      break;
//  }
//
//  Chip_UART_ConfigData(uart->uart, (wordLengthBits | stopBitBits));
//  return true;
//}
//
// static inline bool CANflict_UART_deinit(struct CANflict_UART_t* uart) {
//  Chip_UART_DeInit(uart->uart);
//  Chip_UART_TXDisable(uart->uart);
//
//  return true;
//}
//
// static inline bool CANflict_UART_setBaudrate(struct CANflict_UART_t* uart,
//                                            uint32_t baudrateHz) {
//  Chip_UART_SetBaud(uart->uart, baudrateHz);
//  return true;
//}
//
// static inline uint32_t CANflict_UART_read(struct CANflict_UART_t* uart,
//                                         uint8_t* buf,
//                                         uint32_t byteLen) {
//  // TODO: might be useful to implement also a non-blocking version
//  return Chip_UART_ReadBlocking(uart->uart, buf, byteLen);
//}
//
// static inline uint32_t CANflict_UART_write(struct CANflict_UART_t* uart,
//                                          uint8_t* msgBytes,
//                                          uint32_t byteLen) {
//  // TODO: might be useful to implement also a non-blocking version
//  return Chip_UART_SendBlocking(uart->uart, msgBytes, byteLen);
//}
