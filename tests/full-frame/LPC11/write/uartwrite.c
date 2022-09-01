/*
 * \brief Example of how to use the UARTSender on the LPC11C24 board.
 *
 * To conduct this experiment, the LPC11C24 board has been wired to an MCP2551
 * CAN transceiver as follows:
 *      - Pin 1 of the MCP2551 to the board's PIO1_7 (UART1_TX)
 *      - Pin 2 of the MCP2551 to the board's GND
 *      - Pin 3 of the MCP2551 to an external 5V source
 *      - Pin 4 of the MCP2551 to the board's PIO0_9
 *      - Pin 8 of the MCP2551 to ground with a 10K resistor
 *      - Pin 6 and 7 of the MCP2551 to CANL and CANH, respectively
 */

#include "board.h"
#include "chip.h"
#include <platforms/NXP/platform.h>
#include <senders/UARTSender.h>


static const bool MSG_BITS[] = {0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,
                                1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1,
                                0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 0,
                                1, 1, 0, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1};
static const uint32_t MSG_BIT_LEN = sizeof(MSG_BITS);
static const uint8_t MSG_BYTE_LEN = (MSG_BIT_LEN / 8) + 1;

#define BAUDRATE 1000000 // = 1 MBit/s

int main(void) {
  // Init board
  SystemCoreClockUpdate();
  Board_Init();

  // CANflict Setup
  struct CANflict_UART_t uart;
  uart.uart = LPC_USART;

  struct CANflict_SenderConf_t conf;
  conf.uart = &uart;
  conf.freqHz = BAUDRATE;
  conf.wordLen = 8;
  conf.stopBits = 1;

  uint8_t msgBytes[MSG_BYTE_LEN];
  conf.msgBytes = msgBytes;
  conf.byteLen = MSG_BYTE_LEN;

  CANflict_sender_init(&conf);

  // Send continuously with CANflict
  while (1) {
    for (int i = 0; i < 1000000; ++i)
      /* busy wait between messages */;
    CANflict_sender_setMsgToSend(&conf, MSG_BITS, MSG_BIT_LEN);
    CANflict_sender_sendMsg(&conf);
  }

  return 0;
}
